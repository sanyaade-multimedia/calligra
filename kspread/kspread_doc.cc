/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

#include <qregexp.h>

#include <kstandarddirs.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <ksconfig.h>

#include <koTemplateChooseDia.h>

#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_undo.h"
#include "kspread_view.h"

#include "koDocumentInfo.h"

#include "KSpreadDocIface.h"

using namespace std;

static const int CURRENT_SYNTAX_VERSION = 1;
// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.1";

/*****************************************************************************
 *
 * KSpreadDoc
 *
 *****************************************************************************/

QPtrList<KSpreadDoc>* KSpreadDoc::s_docs = 0;
int KSpreadDoc::s_docId = 0;

QPtrList<KSpreadDoc>& KSpreadDoc::documents()
{
    if ( s_docs == 0 )
        s_docs = new QPtrList<KSpreadDoc>;
    return *s_docs;
}

KSpreadDoc::KSpreadDoc( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
  : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
    m_pageBorderColor( Qt::red )
{
  m_bDelayCalculation = false;

  if ( s_docs == 0 )
      s_docs = new QPtrList<KSpreadDoc>;
  s_docs->append( this );

  setInstance( KSpreadFactory::global(), false );

  // Set a name if there is no name specified
  if ( !name )
  {
      QString tmp( "Document%1" );
      tmp = tmp.arg( s_docId++ );
      setName( tmp.local8Bit());//tmp.latin1() );
  }

  m_iTableId = 1;
  m_dcop = 0;
  m_pMap = 0L;
  m_bLoading = false;

  m_defaultGridPen.setColor( lightGray );
  m_defaultGridPen.setWidth( 1 );
  m_defaultGridPen.setStyle( SolidLine );

  initInterpreter();

  m_pMap = new KSpreadMap( this, "Map" );

  m_pUndoBuffer = new KSpreadUndo( this );

  // Make us scriptable if the document has a name
  if ( name )
      dcopObject();

  m_iCompletionMode=KGlobalSettings::CompletionAuto;

  m_bVerticalScrollBarShow=true;
  m_bHorizontalScrollBarShow=true;
  m_bShowColHeader=true;
  m_bShowRowHeader=true;
  m_iIndentValue=10;
  m_EMoveTo=KSpread::Bottom;
  m_bShowError=false;
  m_EMethodOfCalc=SumOfNumber;
  m_bShowCommentIndicator=true;
  m_bShowTabBar=true;
  m_bShowFormulaBar=true;
  m_bShowStatusBar=true;
  m_pKSpellConfig=0;

  m_bDontCheckUpperWord=false;
  m_bDontCheckTitleCase=false;
  m_unit = KoUnit::U_MM;
}

bool KSpreadDoc::initDoc()
{
    QString f;
    KoTemplateChooseDia::ReturnType ret;

    ret = KoTemplateChooseDia::choose( KSpreadFactory::global(), f, "application/x-kspread",
                                       "*.ksp", i18n("KSpread"), KoTemplateChooseDia::NoTemplates );

    if ( ret == KoTemplateChooseDia::File )
    {
	KURL url;
	url.setPath(f);
	return openURL( url );
    }

    if ( ret == KoTemplateChooseDia::Empty )
    {
	KConfig *config = KSpreadFactory::global()->config();
	int _page=1;
	if( config->hasGroup("Parameters" ))
	{
		config->setGroup( "Parameters" );
		_page=config->readNumEntry( "NbPage",1 ) ;
	}

	for( int i=0; i<_page; i++ )
	{
		KSpreadTable *t = createTable();
		m_pMap->addTable( t );
	}

	resetURL();
	setEmpty();
	initConfig();
	return true;
    }

    return false;
}


void KSpreadDoc::initConfig()
{
    KSpellConfig ksconfig;
    KConfig *config = KSpreadFactory::global()->config();
    if( config->hasGroup("KSpell kspread" ) )
    {
        config->setGroup( "KSpell kspread" );
        ksconfig.setNoRootAffix(config->readNumEntry ("KSpell_NoRootAffix", 0));
        ksconfig.setRunTogether(config->readNumEntry ("KSpell_RunTogether", 0));
        ksconfig.setDictionary(config->readEntry ("KSpell_Dictionary", ""));
        ksconfig.setDictFromList(config->readNumEntry ("KSpell_DictFromList", FALSE));
        ksconfig.setEncoding(config->readNumEntry ("KSpell_Encoding", KS_E_ASCII));
        ksconfig.setClient(config->readNumEntry ("KSpell_Client", KS_CLIENT_ISPELL));
        setKSpellConfig(ksconfig);

        setDontCheckUpperWord(config->readBoolEntry("KSpell_IgnoreUppercaseWords", false));
        setDontCheckTitleCase(config->readBoolEntry("KSpell_IgnoreTitleCaseWords", false));
    }
}

KoView* KSpreadDoc::createViewInstance( QWidget* parent, const char* name )
{
    if ( name == 0 )
        name = "View";
    return new KSpreadView( parent, name, this );
}

bool KSpreadDoc::saveChildren( KoStore* _store )
{
  return m_pMap->saveChildren( _store );
}

QDomDocument KSpreadDoc::saveXML()
{
  //Terminate current cell edition, if any
  QPtrListIterator<KoView> it( views() );

  /* don't pull focus away from the editor if this is just a background
     autosave */
  if (!isAutosaving())
  {
    for (; it.current(); ++it )
      static_cast<KSpreadView *>( it.current() )->deleteEditor( true );
  }

  QDomDocument doc = createDomDocument( "spreadsheet", CURRENT_DTD_VERSION );
  QDomElement spread = doc.documentElement();
  spread.setAttribute( "editor", "KSpread" );
  spread.setAttribute( "mime", "application/x-kspread" );
  spread.setAttribute( "syntaxVersion", CURRENT_SYNTAX_VERSION );

  /* Backwards compatibility with KSpread < 1.2
     Looks like a hack, but it saves us to define an export filter for this issue.
  
     In KSpread < 1.2, the paper format was per map, since 1.2 it's per sheet.
     To enable KSpread < 1.2 to open these files, we store the page layout of the first sheet 
     for the whole map as the map paper layout. */
  if ( specialOutputFlag() == KoDocument::SaveAsKOffice1dot1 /* so it's KSpread < 1.2 */)
  {
    KSpreadTable* firstTable = m_pMap->firstTable();
    
    QDomElement paper = doc.createElement( "paper" );
    paper.setAttribute( "format", firstTable->paperFormatString() );
    paper.setAttribute( "orientation", firstTable->orientationString() );
    spread.appendChild( paper );
    QDomElement borders = doc.createElement( "borders" );
    borders.setAttribute( "left", firstTable->leftBorder() );
    borders.setAttribute( "top", firstTable->topBorder() );
    borders.setAttribute( "right", firstTable->rightBorder() );
    borders.setAttribute( "bottom", firstTable->bottomBorder() );
    paper.appendChild( borders );
    QDomElement head = doc.createElement( "head" );
    paper.appendChild( head );
    if ( !firstTable->headLeft().isEmpty() )
    {
      QDomElement left = doc.createElement( "left" );
      head.appendChild( left );
      left.appendChild( doc.createTextNode( firstTable->headLeft() ) );
    }
    if ( !firstTable->headMid().isEmpty() )
    {
      QDomElement center = doc.createElement( "center" );
      head.appendChild( center );
      center.appendChild( doc.createTextNode( firstTable->headMid() ) );
    }
    if ( !firstTable->headRight().isEmpty() )
    {
      QDomElement right = doc.createElement( "right" );
      head.appendChild( right );
      right.appendChild( doc.createTextNode( firstTable->headRight() ) );
    }
    QDomElement foot = doc.createElement( "foot" );
    paper.appendChild( foot );
    if ( !firstTable->footLeft().isEmpty() )
    {
      QDomElement left = doc.createElement( "left" );
      foot.appendChild( left );
      left.appendChild( doc.createTextNode( firstTable->footLeft() ) );
    }
    if ( !firstTable->footMid().isEmpty() )
    {
      QDomElement center = doc.createElement( "center" );
      foot.appendChild( center );
      center.appendChild( doc.createTextNode( firstTable->footMid() ) );
    }
    if ( !firstTable->footRight().isEmpty() )
    {
      QDomElement right = doc.createElement( "right" );
      foot.appendChild( right );
      right.appendChild( doc.createTextNode( firstTable->footRight() ) );
    }
  }
  
  QDomElement locale = m_locale.save( doc );
  spread.appendChild( locale );

  if(m_refs.count()!=0)
  {
     QDomElement areaname = saveAreaName( doc );
     spread.appendChild( areaname );
  }

  QDomElement e = m_pMap->save( doc );
  spread.appendChild( e );

  setModified( false );

  return doc;
}

bool KSpreadDoc::loadChildren( KoStore* _store )
{
    return m_pMap->loadChildren( _store );
}

bool KSpreadDoc::loadXML( QIODevice *, const QDomDocument& doc )
{
    emit sigProgress( 0 );
  m_bLoading = TRUE;

  // <spreadsheet>
  QDomElement spread = doc.documentElement();

  if ( spread.attribute( "mime" ) != "application/x-kspread" && spread.attribute( "mime" ) != "application/vnd.kde.kspread" )
  {
    m_bLoading = false;
    setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-kspread or application/vnd.kde.kspread, got %1" ).arg( spread.attribute("mime") ) );
    return false;
  }

  m_syntaxVersion = KSpreadDoc::getAttribute( spread, "syntaxVersion", 0 );
  if ( m_syntaxVersion > CURRENT_SYNTAX_VERSION )
  {
      int ret = KMessageBox::warningContinueCancel(
          0, i18n("This document was created with a newer version of KSpread (syntax version: %1)\n"
                  "Opening it in this version of KSpread will lose some information.").arg(m_syntaxVersion),
          i18n("File format mismatch"), i18n("Continue") );
      if ( ret == KMessageBox::Cancel )
      {
          setErrorMessage( "USER_CANCELED" );
          return false;
      }
  }

  // <locale>
  QDomElement locale = spread.namedItem( "locale" ).toElement();
  if ( !locale.isNull() )
      m_locale.load( locale );

  emit sigProgress( 5 );

  m_refs.clear();
  //<areaname >
  QDomElement areaname = spread.namedItem( "areaname" ).toElement();
  if ( !areaname.isNull())
        loadAreaName(areaname);

  emit sigProgress( 40 );
  // In case of reload (e.g. from konqueror)
  m_pMap->tableList().clear(); // it's set to autoDelete


  // <map>
  QDomElement mymap = spread.namedItem( "map" ).toElement();
  if ( mymap.isNull() )
  {
      setErrorMessage( i18n("Invalid document. No map tag.") );
      m_bLoading = false;
      return false;
  }
  if ( !m_pMap->loadXML( mymap ) )
  {
      m_bLoading = false;
      return false;
  }

  //Backwards compatibility with older versions for paper layout
  if ( m_syntaxVersion < 1.2 )
  {
    // <paper>
    QDomElement paper = spread.namedItem( "paper" ).toElement();
    if ( !paper.isNull() )
    {
      QString format = paper.attribute( "format" );
      QString orientation = paper.attribute( "orientation" );

      // <borders>
      QDomElement borders = paper.namedItem( "borders" ).toElement();
      if ( !borders.isNull() )
      {
          float left = borders.attribute( "left" ).toFloat();
          float right = borders.attribute( "right" ).toFloat();
          float top = borders.attribute( "top" ).toFloat();
          float bottom = borders.attribute( "bottom" ).toFloat();
          
          //apply to all tables  
          QPtrListIterator<KSpreadTable> it ( m_pMap->tableList() );
          for( ; it.current(); ++it )
          {
            it.current()->setPaperLayout( left, top, right, bottom, format, orientation );
          }
      }
      
      QString hleft, hright, hcenter;
      QString fleft, fright, fcenter;
      // <head>
      QDomElement head = paper.namedItem( "head" ).toElement();
      if ( !head.isNull() )
      {
        QDomElement left = head.namedItem( "left" ).toElement();
        if ( !left.isNull() )
          hleft = left.text();
        QDomElement center = head.namedItem( "center" ).toElement();
        if ( !center.isNull() )
        hcenter = center.text();
        QDomElement right = head.namedItem( "right" ).toElement();
        if ( !right.isNull() )
          hright = right.text();
      }
      // <foot>
      QDomElement foot = paper.namedItem( "foot" ).toElement();
      if ( !foot.isNull() )
      {
        QDomElement left = foot.namedItem( "left" ).toElement();
        if ( !left.isNull() )
          fleft = left.text();
        QDomElement center = foot.namedItem( "center" ).toElement();
        if ( !center.isNull() )
          fcenter = center.text();
        QDomElement right = foot.namedItem( "right" ).toElement();
        if ( !right.isNull() )
          fright = right.text();
      }
      QPtrListIterator<KSpreadTable> it ( m_pMap->tableList() );
      for( ; it.current(); ++it )
      {
        it.current()->setHeadFootLine( hleft, hcenter, hright, fleft, fcenter, fright);
      }
    }
    
  }
  
  emit sigProgress( 90 );
  initConfig();
  emit sigProgress(-1);
  return true;
}

bool KSpreadDoc::completeLoading( KoStore* /* _store */ )
{
  kdDebug(36001) << "------------------------ COMPLETING --------------------" << endl;

  m_bLoading = false;

  m_pMap->update();

  kdDebug(36001) << "------------------------ COMPLETION DONE --------------------" << endl;

  setModified( FALSE );

  return true;
}

void KSpreadDoc::setDefaultGridPen( const QPen& p )
{
    m_defaultGridPen = p;
}

KSpreadTable* KSpreadDoc::createTable()
{
  QString s( i18n("Table%1") );
  s = s.arg( m_iTableId++ );
  //KSpreadTable *t = new KSpreadTable( m_pMap, s.latin1() );
  KSpreadTable *t = new KSpreadTable( m_pMap, s );
  t->setTableName( s, TRUE ); // huh? (Werner)
  return t;
}

void KSpreadDoc::resetInterpreter()
{
  destroyInterpreter();
  initInterpreter();

  // Update the cell content
  // TODO
  /* KSpreadTable *t;
  for ( t = m_pMap->firstTable(); t != 0L; t = m_pMap->nextTable() )
  t->initInterpreter(); */

  // Perhaps something changed. Lets repaint
  emit sig_updateView();
}


void KSpreadDoc::addTable( KSpreadTable *_table )
{
  m_pMap->addTable( _table );

  setModified( TRUE );

  emit sig_addTable( _table );
}


void KSpreadDoc::initInterpreter()
{
  m_pInterpreter = new KSpreadInterpreter( this );

  // Create the module which is used to evaluate all formulas
  m_module = m_pInterpreter->module( "kspread" );
  m_context.setScope( new KSScope( m_pInterpreter->globalNamespace(), m_module ) );

  // Find all scripts
  m_kscriptModules = KSpreadFactory::global()->dirs()->findAllResources( "extensions", "*.ks", TRUE );

  // Remove dupes
  QMap<QString,QString> m;
  for( QStringList::Iterator it = m_kscriptModules.begin(); it != m_kscriptModules.end(); ++it )
  {
    int pos = (*it).findRev( '/' );
    if ( pos != -1 )
    {
      QString name = (*it).mid( pos + 1 );
      pos = name.find( '.' );
      if ( pos != -1 )
        name = name.left( pos );
      m[ name ] = *it;
    }
  }

  // Load and execute the scripts
  QMap<QString,QString>::Iterator mip = m.begin();
  for( ; mip != m.end(); ++mip )
  {
    kdDebug(36001) << "SCRIPT="<<  mip.key() << ", " << mip.data() << endl;
    KSContext context;
    QStringList args;
    if ( !m_pInterpreter->runModule( context, mip.key(), mip.data(), args ) )
    {
        if ( context.exception() )
            KMessageBox::error( 0L, context.exception()->toString( context ) );
        // else ... well, nothing to show...
    }
  }
}

void KSpreadDoc::destroyInterpreter()
{
    m_context.setValue( 0 );
    m_context.setScope( 0 );
    m_context.setException( 0 );

    m_module = 0;

    m_pInterpreter = 0;
}

void KSpreadDoc::undo()
{
  m_pUndoBuffer->undo();
}

void KSpreadDoc::redo()
{
  m_pUndoBuffer->redo();
}

void KSpreadDoc::enableUndo( bool _b )
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
      static_cast<KSpreadView *>( it.current() )->enableUndo( _b );
}

void KSpreadDoc::enableRedo( bool _b )
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
      static_cast<KSpreadView *>( it.current() )->enableRedo( _b );
}

void KSpreadDoc::paintContent( QPainter& painter, const QRect& rect, bool transparent, double /*zoomX*/, double /*zoomY*/ )
{
    KSpreadTable* table = m_pMap->firstTable();
    if ( !table )
        return;
    kdDebug(36001)<<"paintContent-------------------------------------\n";
    // ### TODO support zooming
    paintContent( painter, rect, transparent, table,false );
}

void KSpreadDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/, KSpreadTable* table, bool drawCursor )
{
    if ( isLoading() )
        return;

    // if ( !transparent )
    // painter.eraseRect( rect );

    int xpos;
    int ypos;
    int left_col = table->leftColumn( rect.x(), xpos );
    int right_col = table->rightColumn( rect.right() );
    int top_row = table->topRow( rect.y(), ypos );
    int bottom_row = table->bottomRow( rect.bottom() );

    QPen pen;
    pen.setWidth( 1 );
    painter.setPen( pen );

    int left = xpos;
    RowLayout *row_lay;
    ColumnLayout *col_lay;
    KSpreadCell *cell;
    for ( int y = top_row; y <= bottom_row; y++ )
    {
        row_lay = table->rowLayout( y );
        xpos = left;

        for ( int x = left_col; x <= right_col; x++ )
        {
            col_lay = table->columnLayout( x );

            cell = table->cellAt( x, y );
            cell->paintCell( rect, painter, QPoint(xpos, ypos),
                             QPoint(x,y), drawCursor );

            xpos += col_lay->width();
        }

        ypos += row_lay->height();
    }
}

KSpreadDoc::~KSpreadDoc()
{
  destroyInterpreter();

  if ( m_pUndoBuffer )
    delete m_pUndoBuffer;

  delete m_dcop;
  s_docs->removeRef(this);
  kdDebug(36001) << "alive 1" << endl;
  delete m_pMap;
}

DCOPObject* KSpreadDoc::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KSpreadDocIface( this );

    return m_dcop;
}

void KSpreadDoc::addAreaName(const QRect &_rect,const QString & name,const QString & tableName)
{
  setModified( true );
  Reference tmp;
  tmp.rect = _rect;
  tmp.table_name = tableName;
  tmp.ref_name = name;
  m_refs.append( tmp);
}

void KSpreadDoc::removeArea( const QString & name)
{
  QValueList<Reference>::Iterator it2;
  for ( it2 = m_refs.begin(); it2 != m_refs.end(); ++it2 )
        {
        if((*it2).ref_name==name)
                {
                m_refs.remove(it2);
                return;
                }
        }
}

void KSpreadDoc::changeAreaTableName(const QString & oldName,const QString & tableName)
{
  QValueList<Reference>::Iterator it2;
  for ( it2 = m_refs.begin(); it2 != m_refs.end(); ++it2 )
        {
        if((*it2).table_name==oldName)
                   (*it2).table_name=tableName;
        }
}

QRect KSpreadDoc::getRectArea(const QString  &_tableName)
{
  QValueList<Reference>::Iterator it2;
  for ( it2 = m_refs.begin(); it2 != m_refs.end(); ++it2 )
        {
        if((*it2).ref_name==_tableName)
                {
                return (*it2).rect;
                }
        }
  return QRect(-1,-1,-1,-1);
}

QDomElement KSpreadDoc::saveAreaName( QDomDocument& doc )
{
   QDomElement element = doc.createElement( "areaname" );
   QValueList<Reference>::Iterator it2;
   for ( it2 = m_refs.begin(); it2 != m_refs.end(); ++it2 )
        {
        QDomElement e = doc.createElement("reference");
        QDomElement tabname = doc.createElement( "tabname" );
        tabname.appendChild( doc.createTextNode( (*it2).table_name ) );
        e.appendChild( tabname );
        QDomElement refname = doc.createElement( "refname" );
        refname.appendChild( doc.createTextNode( (*it2).ref_name ) );
        e.appendChild( refname );
        QDomElement rect = doc.createElement( "rect" );
        rect.setAttribute( "left-rect", ((*it2).rect).left() );
        rect.setAttribute( "right-rect",((*it2).rect).right() );
        rect.setAttribute( "top-rect", ((*it2).rect).top() );
        rect.setAttribute( "bottom-rect", ((*it2).rect).bottom() );
        e.appendChild( rect );
        element.appendChild(e);
        }
 return element;
}

void KSpreadDoc::loadAreaName( QDomElement& element )
{
QDomElement tmp=element.firstChild().toElement();
for( ; !tmp.isNull(); tmp=tmp.nextSibling().toElement()  )
    {
    if ( tmp.tagName() == "reference" )
    {
        QString tabname;
        QString refname;
        int left=0;
        int right=0;
        int top=0;
        int bottom=0;
        QDomElement tableName = tmp.namedItem( "tabname" ).toElement();
        if ( !tableName.isNull() )
                {
                tabname=tableName.text();
                }
        QDomElement referenceName = tmp.namedItem( "refname" ).toElement();
        if ( !referenceName.isNull() )
                {
                refname=referenceName.text();
                }
        QDomElement rect =tmp.namedItem( "rect" ).toElement();
        if(!rect.isNull())
                {
                bool ok;
                if ( rect.hasAttribute( "left-rect" ) )
                        left=rect.attribute("left-rect").toInt( &ok );
                if ( rect.hasAttribute( "right-rect" ) )
		  right=rect.attribute("right-rect").toInt( &ok );
                if ( rect.hasAttribute( "top-rect" ) )
                        top=rect.attribute("top-rect").toInt( &ok );
                if ( rect.hasAttribute( "bottom-rect" ) )
                        bottom=rect.attribute("bottom-rect").toInt( &ok );
                 }
        QRect _rect;
        _rect.setCoords(left,top,right,bottom);
        addAreaName(_rect,refname,tabname);
        }
    }
}

void KSpreadDoc::addStringCompletion(const QString &stringCompletion)
{
   if(listCompletion.items().contains(stringCompletion)==0)
           listCompletion.addItem(stringCompletion);
}

void KSpreadDoc::refreshInterface()
{
   emit sig_refreshView();
}

void KSpreadDoc::setKSpellConfig(KSpellConfig _kspell)
{
  if(m_pKSpellConfig==0)
    m_pKSpellConfig=new KSpellConfig();

  m_pKSpellConfig->setNoRootAffix(_kspell.noRootAffix ());
  m_pKSpellConfig->setRunTogether(_kspell.runTogether ());
  m_pKSpellConfig->setDictionary(_kspell.dictionary ());
  m_pKSpellConfig->setDictFromList(_kspell.dictFromList());
  m_pKSpellConfig->setEncoding(_kspell.encoding());
  m_pKSpellConfig->setClient(_kspell.client());
}

void KSpreadDoc::refreshLocale()
{
    emit sig_refreshLocale();
}


void KSpreadDoc::emitBeginOperation()
{
   KoDocument::emitBeginOperation();
   m_bDelayCalculation = true;
}

void KSpreadDoc::emitEndOperation()
{
   KSpreadTable *t = NULL;
   CellBinding* b = NULL;

   m_bDelayCalculation = false;
   for ( t = m_pMap->firstTable(); t != NULL; t = m_pMap->nextTable() )
   {
      t->update();

      for (b = t->firstCellBinding(); b != NULL; b = t->nextCellBinding())
      {
	b->cellChanged(NULL);
      }
   }
   KoDocument::emitEndOperation();
}


bool KSpreadDoc::delayCalculation()
{
   return m_bDelayCalculation;
}

void KSpreadDoc::updateBorderButton()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
      static_cast<KSpreadView *>( it.current() )->updateBorderButton();
}

void KSpreadDoc::insertTable( KSpreadTable * table )
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
	((KSpreadView*)it.current())->insertTable( table );
}

void KSpreadDoc::takeTable( KSpreadTable * table )
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
	((KSpreadView*)it.current())->removeTable( table );
}

void KSpreadDoc::setUnit( KoUnit::Unit _unit )
{
    m_unit = _unit;
}
#include "kspread_doc.moc"


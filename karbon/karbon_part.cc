/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>

#include "vcommand.h"
#include "vpath.h"
#include "vhandle.h"

#include "karbon_part.h"
#include "karbon_view.h"


// TODO: remove these after debugging:
#include <qwmatrix.h>
#include "vccmd_ellipse.h"
#include "vccmd_star.h"
#include "vccmd_spiral.h"

KarbonPart::KarbonPart( QWidget* parentWidget, const char* widgetName,
	QObject* parent, const char* name, bool singleViewMode )
	: KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
	m_commandHistory = new VCommandHistory( this );

	// create a layer. we need at least one:
	m_layers.append( new VLayer() );
	m_activeLayer = m_layers.getLast();

	// create one and only handle:
	m_handle = new VHandle();
}

KarbonPart::~KarbonPart()
{
	// delete all layers:
	QPtrListIterator<VLayer> itr( m_layers );
	for ( ; itr.current() ; ++itr )
		delete( itr.current() );

	// delete the command-history:
	delete m_commandHistory;

	delete m_handle;
}

bool
KarbonPart::initDoc()
{
// TODO: remove the whole test code:
/*
	VCCmdEllipse e( this, 50, 100, 150, 250 );
	VPath* elly = e.createPath();

	VCCmdStar r( this, 110, 150, 80, 30, 8 );
	VPath* rect = r.createPath();

	insertObject( elly );
	insertObject( rect );

	VCCmdSpiral t(this, 100, 100, 50.0, 25, 0.2, true);
	VPath *spi = t.createPath();

	insertObject(spi);

	VPath* obj = elly->booleanOp( rect, 0 );

	if ( obj )
	{
		QWMatrix m;
		m.translate( 250, 200 );
		obj->transform( m );
		insertObject( obj );
	}

	obj = rect->booleanOp( elly, 0 );

	if ( obj )
	{
		QWMatrix m;
		m.translate( 250, 0 );
		obj->transform( m );
		insertObject( obj );
	}
*/

	return true;
}

KoView*
KarbonPart::createViewInstance( QWidget* parent, const char* name )
{
	return new KarbonView( this, parent, name );
}

bool
KarbonPart::loadXML( QIODevice*, const QDomDocument& )
{
	// TODO load the document from the QDomDocument
	return true;
}

QDomDocument
KarbonPart::saveXML()
{
	QDomDocument doc( "karbon" );

	doc.appendChild(
		doc.createProcessingInstruction(
			"xml",
			"version=\"0.1\" encoding=\"UTF-8\"") );

	QDomElement app = doc.createElement( "karbon14" );
	app.setAttribute( "editor", "karbon14 0.0.1" );
	app.setAttribute( "mime", "application/x-karbon" );
	app.setAttribute( "version", "0.1" );
	doc.appendChild( app );

	return doc;
}

void
KarbonPart::insertObject( const VObject* object )
{
	// don't repaint here explicitely. some commands might want to insert many
	// objects.
	activeLayer()->insertObject( object );
	setModified( true );
}

void
KarbonPart::selectAllObjects()
{
	// select objects from all layers
	QPtrListIterator<VLayer> itr( m_layers );
	for ( ; itr.current() ; ++itr )
		itr.current()->selectAllObjects();
}

void
KarbonPart::selectObjects( const QRect &rect )
{
	// clear handle
	m_handle->reset();

	QPtrList<VObject> list;
	// select objects from all layers
	QPtrListIterator<VLayer> itr( m_layers );
	for ( ; itr.current() ; ++itr )
	{
		// select all objects within the rect coords
		const QPtrList<VObject> objects = itr.current()->objects();
		QPtrListIterator<VObject> oitr = objects;
    	for ( ; oitr.current(); ++oitr )
	    {
	        if( oitr.current()->state() != VObject::deleted &&
	            oitr.current()->boundingBox( 1 ).intersects( rect ) )
			{
				oitr.current()->setState( VObject::selected );
				list.append( oitr.current() );
			}
		}
	}

	// now add the selected items to the handle
	QPtrListIterator<VObject> oitr( list );
	for ( ; oitr.current() ; ++oitr )
		m_handle->addObject( oitr.current() );
}

void
KarbonPart::unselectObjects()
{
	// unselect objects from all layers
	QPtrListIterator<VLayer> itr( m_layers );
	for ( ; itr.current() ; ++itr )
		itr.current()->unselectObjects();
}

void
KarbonPart::deleteObjects( QPtrList<VObject> &list )
{
	// delete selected objects from all layers
	/*QPtrListIterator<VLayer> itr( m_layers );
	for ( ; itr.current() ; ++itr )
		itr.current()->deleteObjects( list );*/
	m_handle->deleteObjects( list );
}

void
KarbonPart::undeleteObjects( QPtrList<VObject> &list )
{
	m_handle->undeleteObjects( list );
}

void
KarbonPart::addCommand( VCommand* cmd )
{
	m_commandHistory->addCommand( cmd );
	setModified( true );
	repaintAllViews();
}

void
KarbonPart::purgeHistory()
{
	// remove "deleted" objects from all layers:
	QPtrListIterator<VLayer> itr( m_layers );
	for ( ; itr.current() ; ++itr )
		itr.current()->removeDeletedObjects();

	m_commandHistory->clear();
}

void
KarbonPart::repaintAllViews( bool /*erase*/ )
{
	QPtrListIterator<KoView> itr( views() );
	for ( ; itr.current() ; ++itr )
// TODO: any better solution for this?
//		static_cast<KarbonView*> ( itr.current() )->canvasWidget()->repaintAll(
// erase );
 		static_cast<KarbonView*>( itr.current() )->
			canvasWidget()->repaintAll( true );
}


void
KarbonPart::paintContent( QPainter& /*p*/, const QRect& /*rect*/,
	bool /*transparent*/, double /*zoomX*/, double /*zoomY*/ )
{
	kdDebug() << "part->paintContent()" << endl;
}

void
KarbonPart::drawHandle( QPainter &p, const double zoomFactor ) const
{
	m_handle->draw( p, zoomFactor );
}

#include "karbon_part.moc"

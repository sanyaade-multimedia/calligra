/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qcursor.h>
#include <qiconset.h>
#include <qpainter.h>

#include <kaction.h>
#include <klocale.h>
#include <kstdaction.h>

#include "karbon_factory.h"
#include "karbon_part.h"
#include "karbon_view.h"
#include "vctool_ellipse.h"
#include "vctool_polygon.h"
#include "vctool_rectangle.h"
#include "vctool_roundrect.h"
#include "vctool_sinus.h"
#include "vctool_spiral.h"
#include "vctool_star.h"
#include "vmtool_select.h"

#include "vmcmd_delete.h"

#include <kdebug.h>

VTool* KarbonView::s_currentTool = 0L;

KarbonView::KarbonView( KarbonPart* part, QWidget* parent, const char* name )
	: KoView( part, parent, name ), m_part( part )
{
	setInstance( KarbonFactory::instance() );

	setXMLFile( QString::fromLatin1( "karbon.rc" ) );
	initActions();

	m_canvas = new VCanvas( this, part );
	m_canvas->viewport()->installEventFilter( this );
	m_canvas->setGeometry( 0, 0, width(), height() );

	// initial tool is select-tool:
	selectTool();
	m_selectToolAction->setChecked( true );
}

KarbonView::~KarbonView()
{
	delete m_canvas;
	m_canvas = 0L;
}

void
KarbonView::updateReadWrite( bool /*rw*/ )
{
}

void
KarbonView::resizeEvent( QResizeEvent* /*event*/ ) {
	m_canvas->resize( width(), height() );
}

void
KarbonView::editCut()
{
}

void
KarbonView::editCopy()
{
}

void
KarbonView::editPaste()
{
}

void
KarbonView::editSelectAll()
{
	// TODO : hmm should this be a proper Command or not ?
	m_part->selectAllObjects();
	m_part->repaintAllViews();
}

void
KarbonView::editDeleteSelection()
{
	m_part->addCommand( new VMCmdDelete( m_part ) );
}

void
KarbonView::ellipseTool()
{
	s_currentTool = VCToolEllipse::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::polygonTool()
{
	s_currentTool = VCToolPolygon::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::rectangleTool()
{
	s_currentTool = VCToolRectangle::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::roundRectTool()
{
	s_currentTool = VCToolRoundRect::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::selectTool()
{
	s_currentTool = VMToolSelect::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
}

void
KarbonView::sinusTool()
{
	s_currentTool = VCToolSinus::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::spiralTool()
{
	s_currentTool = VCToolSpiral::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::starTool()
{
	s_currentTool = VCToolStar::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::zoomChanged()
{
	bool bOK;
	double zoomFactor = m_zoomAction->currentText().toDouble( &bOK ) / 100.0;
	m_canvas->setZoomFactor( zoomFactor );
	m_canvas->repaintContents( true );
}

void
KarbonView::initActions()
{
	// edit
	KStdAction::cut( this, SLOT( editCut() ), actionCollection(),
		"edit_cut" );
	KStdAction::copy( this, SLOT( editCopy() ), actionCollection(),
		"edit_copy");
	KStdAction::paste( this, SLOT( editPaste() ), actionCollection(),
		"edit_paste" );
  	KStdAction::selectAll( this, SLOT( editSelectAll() ), actionCollection(),
		"edit_selectall" );
  	new KAction( i18n( "&Delete" ), "editdelete", 0, this, SLOT( editDeleteSelection() ), actionCollection(),
		"editdelete" );

	// tools:
	m_ellipseToolAction = new KToggleAction(
		i18n("&Ellipse"), "ellipse", 0, this, SLOT( ellipseTool() ),
		actionCollection(), "tool_ellipse" );
	m_polygonToolAction = new KToggleAction(
		i18n("&Polygon"), "polygon", 0, this, SLOT( polygonTool() ),
		actionCollection(), "tool_polygon" );
	m_rectangleToolAction = new KToggleAction(
		i18n("&Rectangle"), "rectangle", 0, this, SLOT( rectangleTool() ),
		actionCollection(), "tool_rectangle" );
	m_roundRectToolAction = new KToggleAction(
		i18n("&Round Rectangle"), "roundrect", 0, this, SLOT( roundRectTool() ),
		actionCollection(), "tool_roundrect" );
	m_sinusToolAction = new KToggleAction(
		i18n("S&inus"), "sinus", 0, this, SLOT( sinusTool() ),
		actionCollection(), "tool_sinus" );
	m_selectToolAction = new KToggleAction(
		i18n("&Select Objects"), "select", 0, this, SLOT( selectTool() ),
		actionCollection(), "tool_select" );
	m_spiralToolAction = new KToggleAction(
		i18n("S&piral"), "spiral", 0, this, SLOT( spiralTool() ),
		actionCollection(), "tool_spiral" );
	m_starToolAction = new KToggleAction(
		i18n("S&tar"), "star", 0, this, SLOT( starTool() ),
		actionCollection(), "tool_star" );

	m_ellipseToolAction->setExclusiveGroup( "Tools" );
	m_polygonToolAction->setExclusiveGroup( "Tools" );
	m_rectangleToolAction->setExclusiveGroup( "Tools" );
	m_roundRectToolAction->setExclusiveGroup( "Tools" );
	m_sinusToolAction->setExclusiveGroup( "Tools" );
	m_selectToolAction->setExclusiveGroup( "Tools" );
	m_spiralToolAction->setExclusiveGroup( "Tools" );
	m_starToolAction->setExclusiveGroup( "Tools" );

	// zoom:
	m_zoomAction = new KSelectAction( i18n( "&Zoom" ), 0, this, SLOT( zoomChanged() ), actionCollection(),
		"view_zoom" );
	QStringList stl;
	stl
		<< i18n( "25%" )
		<< i18n( "50%" )
		<< i18n( "100%" )
		<< i18n( "200%" );
	m_zoomAction->setItems( stl );
	m_zoomAction->setCurrentItem( 2 );
	m_zoomAction->setEditable( true );
}

void
KarbonView::paintEverything( QPainter& /*p*/, const QRect& /*rect*/,
	bool /*transparent*/)
{
	kdDebug() << "view->paintEverything()" << endl;
}

bool
KarbonView::eventFilter( QObject* object, QEvent* event )
{
	if ( object == m_canvas->viewport() )
		return s_currentTool->eventFilter( this, event );
	else
		return false;
}

#include "karbon_view.moc"

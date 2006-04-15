/* This file is part of the KDE project

   Copyright 2006 Robert Knight <robertknight@gmail.com>
   Copyright 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2004 David Faure <faure@kde.org>
   Copyright 2004-2005 Meni Livne <livne@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Hamish Rodda <rodda@kde.org>
   Copyright 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2003 Lukas Tinkl <lukas@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Daniel Naber <daniel.naber@t-online.de>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 1999-2000 Stephan Kulow <coolo@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Michael Reiher <michael.reiher.gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <assert.h>
#include <float.h>
#include <stdlib.h>

#include <qapplication.h>
#include <qbuffer.h>
#include <qclipboard.h>
#include <qdrawutil.h>
#include <qlabel.h>
#include <qpoint.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QTextStream>
#include <QDragMoveEvent>
#include <Q3CString>
#include <QDragLeaveEvent>
#include <QWheelEvent>
#include <QDropEvent>
#include <QList>
#include <QPixmap>

#include <kcursor.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <krun.h>
#include <kmimetype.h>
#include <ksharedptr.h>
#include <kwordwrap.h>

#include <KoOasisStore.h>
#include <KoStore.h>
#include <KoStoreDrag.h>
#include <KoXmlWriter.h>
#include <KoDocumentChild.h>
#include <KoRect.h>

#include "commands.h"
#include "kspread_doc.h"
#include "kspread_editors.h"
#include "kspread_global.h"
#include "kspread_locale.h"
#include "kspread_map.h"
#include "kspread_sheet.h"
#include "kspread_undo.h"
#include "kspread_util.h"
#include "kspread_view.h"
#include "selection.h"

#include "kspread_canvas.h"

// TODO Stefan: undefine/remove, if non-contiguous selections don't work
//              properly or if we are sure, that they do. ;-)
#define NONCONTIGUOUSSELECTION

#define MIN_SIZE 10

using namespace KSpread;

class Canvas::Private
{
  public:
    ComboboxLocationEditWidget *posWidget;
    KSpread::EditWidget *editWidget;
    KSpread::CellEditor *cellEditor;

    View *view;
    QTimer* scrollTimer;

    // Non visible range left from current screen
    // Example: If the first visible column is 'E', then xOffset stores
    // the width of the invisible columns 'A' to 'D'.
    double xOffset;

    // Non visible range on top of the current screen
    // Example: If the first visible row is '5', then yOffset stores
    // the height of the invisible rows '1' to '4'.
    double yOffset;

    // Used to draw the grey grid that is usually only visible on the
    // screen, but not by printing on paper.
    QPen defaultGridPen;

    // see setLastEditorWithFocus, lastEditorWithFocus
    Canvas::EditorType focusEditorType;

    QLabel *validationInfo;

    // true if the user is to choose a cell.
    bool chooseCell;

    // True when the mouse button is pressed
    bool mousePressed;

    // If the user is dragging around with the mouse then this tells us what he is doing.
    // The user may want to mark cells or he started in the lower right corner
    // of the marker which is something special. The values for the 2 above
    // methods are called 'Mark' and 'ResizeCell' or 'AutoFill' depending
    // on the mouse button used. By default this variable holds
    // the value 'NoAction'.
    Canvas::MouseActions mouseAction;

    // If we use the lower right corner of the marker to start autofilling, then this
    // rectangle conatins all cells that were already marker when the user started
    // to mark the rectangle which he wants to become autofilled.
    QRect autoFillSource;

    // Start coordinates for drag and drop
    QPoint dragStart;
    bool dragging;

    // Used to indicate whether the user started drawing a rubber band rectangle
    bool rubberBandStarted;
    QPoint rubberBandStart;
    QPoint rubberBandEnd;

    // If the mouse is over some anchor ( in the sense of HTML anchors )
    QString anchor;

    bool mouseSelectedObject;
    bool drawContour;
    ModifyType modType;
    /**
     * Saves the last mouse position during mouse move events.
     */
    QPoint m_savedMousePos;

    //---- stuff needed for resizing ----
    /// object which gets resized
    EmbeddedObject *m_resizeObject;
    /// ratio of the object ( width / height )
    double m_ratio;
    bool m_isResizing;
    /// The last position of the mouse during moving
    KoPoint m_origMousePos;

    //---- stuff needed for moving ----
    bool m_isMoving;
    KoPoint m_moveStartPoint;

    /// size of the object at when resizing is started
    KoRect m_rectBeforeResize;
    /// Start position for move with mouse
    KoPoint m_moveStartPosMouse;

    /// object which is selected and should be shown above all the other objects
    EmbeddedObject * m_objectDisplayAbove;

   // bool mouseOverHighlightRangeSizeGrip;

    // The row and column of 1) the last cell under mouse pointer, 2) the last focused cell, and
    // the last spoken cell.
    int prevSpokenPointerRow;
    int prevSpokenPointerCol;
    int prevSpokenFocusRow;
    int prevSpokenFocusCol;
    int prevSpokenRow;
    int prevSpokenCol;
};



/****************************************************************
 *
 * Canvas
 *
 ****************************************************************/

Canvas::Canvas (View *_view)
  : QWidget( _view, "", /*WNorthWestGravity*/ Qt::WStaticContents| Qt::WResizeNoErase | Qt::WNoAutoErase )
{
  d = new Private;

  d->cellEditor = 0;
  d->chooseCell = false;
  d->validationInfo = 0L;

  QWidget::setFocusPolicy( Qt::StrongFocus );

  d->dragStart = QPoint( -1, -1 );
  d->dragging = false;


  d->defaultGridPen.setColor( Qt::lightGray );
  d->defaultGridPen.setWidth( 1 );
  d->defaultGridPen.setStyle( Qt::SolidLine );

  d->xOffset = 0.0;
  d->yOffset = 0.0;
  d->view = _view;
  // m_eAction = DefaultAction;
  d->mouseAction = NoAction;
  d->rubberBandStarted = false;
  // m_bEditDirtyFlag = false;

  //Now built afterwards(David)
  //d->editWidget = d->view->editWidget();
  d->posWidget = d->view->posWidget();

  setBackgroundMode( Qt::PaletteBase );

  setMouseTracking( true );
  d->mousePressed = false;
  d->mouseSelectedObject = false;
  d->drawContour = false;
  d->modType = MT_NONE;

  d->m_resizeObject = 0L;
  d->m_ratio = 0.0;
  d->m_isMoving = false;
  d->m_objectDisplayAbove = false;
  d->m_isResizing = false;

  d->prevSpokenPointerRow = -1;
  d->prevSpokenPointerCol = -1;
  d->prevSpokenFocusRow = -1;
  d->prevSpokenFocusCol = -1;
  d->prevSpokenRow = -1;
  d->prevSpokenCol = -1;

  d->scrollTimer = new QTimer( this );
  connect (d->scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );

  if( d->view)
  {
    connect( d->view, SIGNAL( autoScroll( const QPoint & )), this, SLOT( slotAutoScroll( const QPoint &)));
  }
  setFocus();
  installEventFilter( this );
  setAcceptDrops( true );
  setInputMethodEnabled( true ); // ensure using the InputMethod

  setWindowFlags(Qt::WNoAutoErase);
}

Canvas::~Canvas()
{
  delete d->scrollTimer;
  delete d->validationInfo;
  delete d;
}

KSpread::View* Canvas::view() const
{
  return d->view;
}

Doc* Canvas::doc() const
{
  return d->view->doc();
}

void Canvas::setEditWidget( KSpread::EditWidget * ew )
{
  d->editWidget = ew;
}

KSpread::EditWidget* Canvas::editWidget() const
{
  return d->editWidget;
}

CellEditor* Canvas::editor() const
{
  return d->cellEditor;
}

double Canvas::xOffset() const
{
  return d->xOffset;
}

double Canvas::yOffset() const
{
  return d->yOffset;
}

void Canvas::setXOffset( double _xOffset )
{
  d->xOffset = _xOffset;
  kDebug(36001) << "setXOffset(): XOffset before scrollToCell: "
		 << d->xOffset << endl;
  scrollToCell( marker() );
  kDebug(36001) << "setXOffset(): XOffset after scrollToCell: "
		 << d->xOffset << endl;
}

void Canvas::setYOffset( double _yOffset )
{
  d->yOffset = _yOffset;
  kDebug(36001) << "setyOffset(): YOffset before scrollToCell: "
		 << d->yOffset << endl;
  scrollToCell( marker() );
  kDebug(36001) << "setYOffset(): YOffset after scrollToCell: "
		 << d->yOffset << endl;
}

const QPen& Canvas::defaultGridPen() const
{
  return d->defaultGridPen;
}

void Canvas::setLastEditorWithFocus( Canvas::EditorType type )
{
  d->focusEditorType = type;
}

Canvas::EditorType Canvas::lastEditorWithFocus() const
{
  return d->focusEditorType;
}


bool Canvas::eventFilter( QObject *o, QEvent *e )
{
  /* this canvas event filter acts on events sent to the line edit as well
     as events to this filter itself.
  */
  if ( !o || !e )
    return true;
  switch ( e->type() )
  {
  case QEvent::KeyPress:
  {
    QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
    if ((keyev->key()==Qt::Key_Tab) || (keyev->key()==Qt::Key_Backtab))
    {
      keyPressEvent ( keyev );
      return true;
    }
    break;
  }
  case QEvent::InputMethod:
  {
      //QIMEvent * imev = static_cast<QIMEvent *>(e);
      //processIMEvent( imev );
      //break;
  }
  case QEvent::ToolTip:
  {
    QHelpEvent* helpEvent = static_cast<QHelpEvent*>( e );
    showToolTip( helpEvent->pos() );
  }
  default:
    break;
  }
  return false;
}

bool Canvas::focusNextPrevChild( bool )
{
    return true; // Don't allow to go out of the canvas widget by pressing "Tab"
}

Selection* Canvas::selectionInfo() const
{
  return d->view->selectionInfo();
}

Selection* Canvas::choice() const
{
  return d->view->choice();
}

QRect Canvas::selection() const
{
  return d->view->selectionInfo()->selection();
}

QPoint Canvas::marker() const
{
    return d->view->selectionInfo()->marker();
}

int Canvas::markerColumn() const
{
    return d->view->selectionInfo()->marker().x();
}

int Canvas::markerRow() const
{
    return d->view->selectionInfo()->marker().y();
}

double Canvas::zoom() const
{
  return d->view->zoom();
}

void Canvas::setChooseMode(bool state)
{
  d->chooseCell = state;
}

bool Canvas::chooseMode() const
{
  return d->chooseCell;
}

void Canvas::startChoose()
{
  if ( d->chooseCell )
    return;

  choice()->clear();
  choice()->setSheet(activeSheet());

  // It is important to enable this AFTER we set the rect!
  d->chooseCell = true;
}

void Canvas::startChoose( const QRect& rect )
{
  if (d->chooseCell)
    return;

  choice()->setSheet(activeSheet());
  choice()->initialize(rect);

  // It is important to enable this AFTER we set the rect!
  d->chooseCell = true;
}

void Canvas::endChoose()
{
  // While entering a formula the choose mode is turned on and off.
  // Clear the choice even if we are not in choose mode. Otherwise,
  // cell references will stay highlighted.
  if (!choice()->isEmpty())
  {
    choice()->clear();
    update();
  }

  if ( !d->chooseCell )
    return;

  d->chooseCell = false;

  Sheet *sheet = choice()->sheet();
  if (sheet)
  {
    d->view->setActiveSheet(sheet);
  }
}

HBorder* Canvas::hBorderWidget() const
{
  return d->view->hBorderWidget();
}

VBorder* Canvas::vBorderWidget() const
{
  return d->view->vBorderWidget();
}

QScrollBar* Canvas::horzScrollBar() const
{
  return d->view->horzScrollBar();
}

QScrollBar* Canvas::vertScrollBar() const
{
  return d->view->vertScrollBar();
}

Sheet* Canvas::findSheet( const QString& _name ) const
{
  return d->view->doc()->map()->findSheet( _name );
}

Sheet* Canvas::activeSheet() const
{
  return d->view->activeSheet();
}

void Canvas::validateSelection()
{
  Sheet* sheet = activeSheet();
  if (!sheet)
  {
    return;
  }

    if ( selectionInfo()->isSingular() )
    {
        int col = selectionInfo()->marker().x();
        int row = selectionInfo()->marker().y();
        Cell * cell = sheet->cellAt( col,row );
        if ( cell && cell->getValidity(0) && cell->getValidity()->displayValidationInformation)
        {
            QString title = cell->getValidity(0)->titleInfo;
            QString message = cell->getValidity(0)->messageInfo;
            if ( title.isEmpty() && message.isEmpty() )
                return;

            if ( !d->validationInfo )
                d->validationInfo = new QLabel(  this );
            kDebug()<<" display info validation\n";
            double u = cell->dblWidth( col );
            double v = cell->dblHeight( row );
            double xpos = sheet->dblColumnPos( markerColumn() ) - xOffset();
            double ypos = sheet->dblRowPos( markerRow() ) - yOffset();
            // Special treatment for obscured cells.
            if ( cell->isObscured() && cell->isPartOfMerged() )
            {
                cell = cell->obscuringCells().first();
                int moveX = cell->column();
                int moveY = cell->row();

                // Use the obscuring cells dimensions
                u = cell->dblWidth( moveX );
                v = cell->dblHeight( moveY );
                xpos = sheet->dblColumnPos( moveX );
                ypos = sheet->dblRowPos( moveY );
            }
            //d->validationInfo->setGeometry( 3, y + 3, len + 2, hei + 2 );
            d->validationInfo->setAlignment( Qt::AlignVCenter );
            QPainter painter;
            painter.begin( this );
            int len = 0;
            int hei = 0;
            QString resultText;
            if ( !title.isEmpty() )
            {
                len = painter.fontMetrics().width( title );
                hei = painter.fontMetrics().height();
                resultText = title + "\n";
            }
            if ( !message.isEmpty() )
            {
                int i = 0;
                int pos = 0;
                QString t;
                do
                {
                    i = message.indexOf( "\n", pos );
                    if ( i == -1 )
                        t = message.mid( pos, message.length() - pos );
                    else
                    {
                        t = message.mid( pos, i - pos );
                        pos = i + 1;
                    }
                    hei += painter.fontMetrics().height();
                    len = qMax( len, painter.fontMetrics().width( t ) );
                }
                while ( i != -1 );
                resultText += message;
            }
            painter.end();
            d->validationInfo->setText( resultText );

            KoRect unzoomedMarker( xpos - xOffset()+u,
                                   ypos - yOffset()+v,
                                   len,
                                   hei );
            QRect marker( d->view->doc()->zoomRect( unzoomedMarker ) );

            d->validationInfo->setGeometry( marker );
            d->validationInfo->show();
        }
        else
        {
            delete d->validationInfo;
            d->validationInfo = 0L;
        }
    }
    else
    {
        delete d->validationInfo;
        d->validationInfo = 0L;
    }
}


void Canvas::scrollToCell(QPoint location) const
{
  Sheet* sheet = activeSheet();
  if (sheet == NULL)
    return;

  kDebug(36001) << "------------------------------------------------" << endl;
  kDebug(36001) << "scrollToCell(): at location [" << location.x() << ","
  		 << location.y() << "]" << endl;

  /* we don't need this cell ptr, but this call is necessary to update the
     scroll bar correctly.  I don't like having that as part of the cellAt function
     but I suppose that's ok for now.
  */
  Cell* cell = sheet->cellAt(location.x(), location.y(), true);
  Q_UNUSED(cell);

  double  unzoomedWidth  = d->view->doc()->unzoomItX( width() );
  double  unzoomedHeight = d->view->doc()->unzoomItY( height() );

  kDebug(36001) << "Unzoomed view size: [" << unzoomedWidth << ","
		 << unzoomedHeight << "]" << endl;

  // xpos is the position of the cell in the current window in unzoomed
  // document coordinates.
  double xpos;
  if ( sheet->layoutDirection()==Sheet::LeftToRight )
    xpos = sheet->dblColumnPos( location.x() ) - xOffset();
  else
    xpos = unzoomedWidth - sheet->dblColumnPos( location.x() ) + xOffset();
  double ypos = sheet->dblRowPos( location.y() ) - yOffset();

  kDebug(36001) << "Position: [" << xpos << "," << ypos << "]" << endl;

  double minY = 40.0;
  double maxY = unzoomedHeight - 40.0;
  kDebug(36001) << "Canvas::scrollToCell : height=" << height() << endl;
  kDebug(36001) << "Canvas::scrollToCell : width=" << width() << endl;

  if ( sheet->layoutDirection()==Sheet::RightToLeft ) {
    // Right to left sheet.

    double minX = unzoomedWidth - 100.0; // less than that, we scroll
    double maxX = 100.0; // more than that, we scroll

    // kDebug() << "rtl2: XPos: " << xpos << ", min: " << minX << ", maxX: " << maxX << ", Offset: " << xOffset() << endl;

    // Do we need to scroll left?
    if ( xpos > minX )
      horzScrollBar()->setValue( horzScrollBar()->maxValue() -
                                  d->view->doc()->zoomItX( xOffset() - xpos + minX ) );

    // Do we need to scroll right?
    else if ( xpos < maxX )
    {
      double horzScrollBarValue = xOffset() - xpos + maxX;
      double horzScrollBarValueMax = sheet->sizeMaxX() - unzoomedWidth;

      //We don't want to display any area > KS_colMax widths
      if ( horzScrollBarValue > horzScrollBarValueMax )
        horzScrollBarValue = horzScrollBarValueMax;

      horzScrollBar()->setValue( horzScrollBar()->maxValue() -
                                  d->view->doc()->zoomItX( horzScrollBarValue ) );
    }
  }
  else {
    // Left to right sheet.

    double minX = 100.0; // less than that, we scroll
    double maxX = unzoomedWidth - 100.0; // more than that, we scroll

    kDebug() << "ltr: XPos: " << xpos << ", min: " << minX << ", maxX: " << maxX << endl;

    // Do we need to scroll left?
    if ( xpos < minX )
      horzScrollBar()->setValue( d->view->doc()->zoomItX( xOffset() + xpos - minX ) );

    // Do we need to scroll right?
    else if ( xpos > maxX )
    {
      double horzScrollBarValue = xOffset() + xpos - maxX;
      double horzScrollBarValueMax = sheet->sizeMaxX() - unzoomedWidth;

      //We don't want to display any area > KS_colMax widths
      if ( horzScrollBarValue > horzScrollBarValueMax )
        horzScrollBarValue = horzScrollBarValueMax;

      horzScrollBar()->setValue( d->view->doc()->zoomItX( horzScrollBarValue ) );
    }
  }

  // do we need to scroll up
  if ( ypos < minY )
    vertScrollBar()->setValue( d->view->doc()->zoomItY( yOffset() + ypos - minY ) );

  // do we need to scroll down
  else if ( ypos > maxY )
  {
    double vertScrollBarValue = yOffset() + ypos - maxY;
    double vertScrollBarValueMax = sheet->sizeMaxY() - unzoomedHeight;

    //We don't want to display any area > KS_rowMax heights
    if ( vertScrollBarValue > vertScrollBarValueMax )
      vertScrollBarValue = vertScrollBarValueMax;

    vertScrollBar()->setValue( d->view->doc()->zoomItY( vertScrollBarValue ) );
  }
}

void Canvas::slotScrollHorz( int _value )
{
  Sheet * sheet = activeSheet();

  if ( sheet == 0L )
    return;

  kDebug(36001) << "slotScrollHorz: value = " << _value << endl;
  //kDebug(36001) << kBacktrace() << endl;

  if ( sheet->layoutDirection()==Sheet::RightToLeft )
    _value = horzScrollBar()->maxValue() - _value;

  double unzoomedValue = d->view->doc()->unzoomItX( _value );
  double dwidth = d->view->doc()->unzoomItX( width() );

  d->view->doc()->emitBeginOperation(false);

  if ( unzoomedValue < 0.0 ) {
    kDebug (36001)
      << "Canvas::slotScrollHorz: value out of range (unzoomedValue: "
      << unzoomedValue << ")" << endl;
    unzoomedValue = 0.0;
  }

  double xpos = sheet->dblColumnPos( qMin( KS_colMax, d->view->activeSheet()->maxColumn()+10 ) ) - d->xOffset;
  if ( unzoomedValue > ( xpos + d->xOffset ) )
    unzoomedValue = xpos + d->xOffset;

  sheet->enableScrollBarUpdates( false );

  // Relative movement
  int dx = d->view->doc()->zoomItX( d->xOffset - unzoomedValue );


  /* what cells will need painted now? */
  QRect area = visibleCells();
  double tmp;
  if (dx > 0)
  {
    area.setRight( area.left() );
    area.setLeft( sheet->leftColumn( unzoomedValue, tmp ) );
  }
  else
  {
    area.setLeft( area.right() );
    area.setRight( sheet->rightColumn( dwidth  + unzoomedValue ) );
  }

  sheet->setRegionPaintDirty(area);

  // New absolute position
  kDebug(36001) << "slotScrollHorz(): XOffset before setting: "
		 << d->xOffset << endl;
  d->xOffset = unzoomedValue;
  kDebug(36001) << "slotScrollHorz(): XOffset after setting: "
		 << d->xOffset << endl;

  if ( sheet->layoutDirection()==Sheet::RightToLeft )
    dx = -dx;

  scroll( dx, 0 );

  hBorderWidget()->scroll( dx, 0 );

  sheet->enableScrollBarUpdates( true );

  d->view->doc()->emitEndOperation( sheet->visibleRect( this ) );
}

void Canvas::slotScrollVert( int _value )
{
  if ( activeSheet() == 0L )
    return;

  d->view->doc()->emitBeginOperation(false);
  double unzoomedValue = d->view->doc()->unzoomItY( _value );

  if ( unzoomedValue < 0 )
  {
    unzoomedValue = 0;
    kDebug (36001) << "Canvas::slotScrollVert: value out of range (unzoomedValue: " <<
                       unzoomedValue << ")" << endl;
  }

  double ypos = activeSheet()->dblRowPos( qMin( KS_rowMax, d->view->activeSheet()->maxRow()+10 ) );
  if ( unzoomedValue > ypos )
      unzoomedValue = ypos;

  activeSheet()->enableScrollBarUpdates( false );

  // Relative movement
  int dy = d->view->doc()->zoomItY( d->yOffset - unzoomedValue );


  /* what cells will need painted now? */
  QRect area = visibleCells();
  double tmp;
  if (dy > 0)
  {
    area.setBottom(area.top());
    area.setTop(activeSheet()->topRow(unzoomedValue, tmp));
  }
  else
  {
    area.setTop(area.bottom());
    area.setBottom(activeSheet()->bottomRow(d->view->doc()->unzoomItY(height()) +
                                            unzoomedValue));
  }

  activeSheet()->setRegionPaintDirty( area );

  // New absolute position
  d->yOffset = unzoomedValue;
  scroll( 0, dy );
  vBorderWidget()->scroll( 0, dy );

  activeSheet()->enableScrollBarUpdates( true );

  d->view->doc()->emitEndOperation( activeSheet()->visibleRect( this ) );
}

void Canvas::slotMaxColumn( int _max_column )
{
  int oldValue = horzScrollBar()->maxValue() - horzScrollBar()->value();
  double xpos = activeSheet()->dblColumnPos( qMin( KS_colMax, _max_column + 10 ) ) - xOffset();
  double unzoomWidth = d->view->doc()->unzoomItX( width() );

  //Don't go beyond the maximum column range (KS_colMax)
  double sizeMaxX = activeSheet()->sizeMaxX();
  if ( xpos > sizeMaxX - xOffset() - unzoomWidth )
    xpos = sizeMaxX - xOffset() - unzoomWidth;

  horzScrollBar()->setRange( 0, d->view->doc()->zoomItX( xpos + xOffset() ) );

  if ( activeSheet()->layoutDirection()==Sheet::RightToLeft )
    horzScrollBar()->setValue( horzScrollBar()->maxValue() - oldValue );
}

void Canvas::slotMaxRow( int _max_row )
{
  double ypos = activeSheet()->dblRowPos( qMin( KS_rowMax, _max_row + 10 ) ) - yOffset();
  double unzoomHeight = d->view->doc()->unzoomItY( height() );

  //Don't go beyond the maximum row range (KS_rowMax)
  double sizeMaxY = activeSheet()->sizeMaxY();
  if ( ypos > sizeMaxY - yOffset() - unzoomHeight )
    ypos = sizeMaxY - yOffset() - unzoomHeight;

  vertScrollBar()->setRange( 0, d->view->doc()->zoomItY( ypos + yOffset() ) );
}

void Canvas::mouseMoveEvent( QMouseEvent * _ev )
{
  // Dont allow modifications if document is readonly. Selecting is no modification
  if ( (!d->view->koDocument()->isReadWrite()) && (d->mouseAction!=Mark))
    return;

  if ( d->mousePressed && d->modType != MT_NONE )
  {
    KoPoint docPoint ( doc()->unzoomPoint( _ev->pos() ) );
    docPoint += KoPoint( xOffset(), yOffset() );

    if ( d->modType == MT_MOVE )
    {
      if ( !d->m_isMoving )
      {
        d->m_moveStartPoint = objectRect( false ).topLeft();
        d->m_isMoving = true;
      }
      moveObjectsByMouse( docPoint, _ev->modifiers() & Qt::AltModifier || _ev->state() & Qt::ControlModifier );
    }
    else if ( d->m_resizeObject )
    {
      if ( !d->m_isResizing )
        d->m_isResizing = true;

      bool keepRatio = d->m_resizeObject->isKeepRatio();
      if ( _ev->modifiers() & Qt::AltModifier )
      {
        keepRatio = true;
      }
      docPoint  = KoPoint( doc()->unzoomPoint( _ev->pos() ) );
      resizeObject( d->modType, docPoint, keepRatio );
    }
    return;
  }


  /*if ( d->mousePressed && d->m_resizeObject && d->modType != MT_NONE )
  {
    if ( !d->m_isMoving )
    {
      d->m_isMoving = true;
      update();
    }
    else
      update( d->m_boundingRealRect );


    QRect drawingRect;

    if ( d->modType == MT_MOVE )
    {
      drawingRect = QRect( _ev->pos() - d->m_origPos, d->m_origSize );
      d->m_boundingRealRect = drawingRect;
    }
    else
    {
      drawingRect = doc()->zoomRect( calculateNewGeometry(d->modType,  _ev->pos().x(), _ev->pos().y() ) );
      drawingRect.moveBy( (int)( -xOffset() * doc()->zoomedResolutionX() ) , (int)( -yOffset() * doc()->zoomedResolutionY() ) );
    }

    // Autoscrolling
    if ( ( d->modType == MT_MOVE && drawingRect.top() < 0 ) ||  ( d->modType != MT_MOVE && _ev->pos().y() < 0 ) )
    {
      vertScrollBar()->setValue ((int) ( vertScrollBar()->value() -
          autoScrollAccelerationY( - drawingRect.top() ) ) );
    }
    else if ( ( d->modType == MT_MOVE && drawingRect.bottom() > height() ) ||  ( d->modType != MT_MOVE && _ev->pos().y() > height() ) )
    {
      vertScrollBar()->setValue ((int) ( vertScrollBar()->value() +
          autoScrollAccelerationY ( drawingRect.bottom() - height() ) ) );
    }
    if ( ( d->modType == MT_MOVE && drawingRect.left() < 0 ) ||  ( d->modType != MT_MOVE && _ev->pos().x() < 0 ) )
    {
      horzScrollBar()->setValue ((int) ( horzScrollBar()->value() -
          autoScrollAccelerationX( - drawingRect.left() ) ) );
    }
    else if ( ( d->modType == MT_MOVE && drawingRect.right() > width() ) ||  ( d->modType != MT_MOVE && _ev->pos().x() > width() )  )
    {
      horzScrollBar()->setValue ((int) (horzScrollBar()->value() +
          autoScrollAccelerationX( drawingRect.right() - width() ) ) );
    }

    if ( drawingRect.left() < 0 )
    {
        drawingRect.setRight( drawingRect.right() -drawingRect.left() );
        drawingRect.setLeft( 0 );
    }
    if ( drawingRect.top() < 0 )
    {
        drawingRect.setBottom( drawingRect.bottom() -drawingRect.top() );
        drawingRect.setTop( 0 );
    }

    d->m_boundingRealRect = drawingRect; //redraw this area next time the mouse has been moved

    //update( d->m_boundingRealRect );
    QPainter p(this);
    p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
    p.setPen( QPen( black, 0, DotLine ) );
    p.drawRect( drawingRect );
    p.end();
    return;
}*/

  if ( d->dragging )
  {
    return;
  }
  if ( d->dragStart.x() != -1 )
  {
    QPoint p ( (int) _ev->pos().x() + (int) xOffset(),
               (int) _ev->pos().y() + (int) yOffset() );

    if ( ( d->dragStart - p ).manhattanLength() > 4 )
    {
      d->dragging = true;
      startTheDrag();
      d->dragStart.setX( -1 );
    }
    d->dragging = false;
    return;
  }

  // Get info about where the event occurred - this is duplicated
  // in ::mousePressEvent, needs to be separated into one function
  Sheet *sheet = activeSheet();
  if ( !sheet )
  {
    return;
  }

  if ( d->mouseSelectedObject )
  {
    EmbeddedObject *obj = 0;
    QPoint p ( (int) _ev->x(),
              (int) _ev->y() );
    if ( ( obj = getObject( p, activeSheet() ) ) && obj->isSelected() )
    {
      KoRect const bound = obj->geometry();
      QRect zoomedBound = doc()->zoomRect( KoRect(bound.left(), bound.top(),
      bound.width(),
      bound.height() ) );
      zoomedBound.translate( (int)(-xOffset() * doc()->zoomedResolutionX() ), (int)(-yOffset() * doc()->zoomedResolutionY() ));
      setCursor( obj->getCursor( p, d->modType, zoomedBound ) );
      return;
    }
  }

  double dwidth = d->view->doc()->unzoomItX( width() );
  double ev_PosX;
  if ( sheet->layoutDirection()==Sheet::RightToLeft )
  {
    ev_PosX = dwidth - d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  }
  else
  {
    ev_PosX = d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  }
  double ev_PosY = d->view->doc()->unzoomItY( _ev->pos().y() ) + yOffset();

  // In which cell did the user click ?
  double xpos;
  double ypos;
  int col = sheet->leftColumn( ev_PosX, xpos );
  int row  = sheet->topRow( ev_PosY, ypos );

  // you cannot move marker when col > KS_colMax or row > KS_rowMax
  if ( col > KS_colMax || row > KS_rowMax )
  {
    kDebug(36001) << "Canvas::mouseMoveEvent: col or row is out of range: "
                   << "col: " << col << " row: " << row << endl;
    return;
  }


  //*** Highlighted Range Resize Handling ***
  if (d->mouseAction == ResizeSelection)
  {
    choice()->update(QPoint(col,row));
    return;
  }

  //Check to see if the mouse is over a highlight range size grip and if it is, change the cursor
  //shape to a resize arrow
  if (highlightRangeSizeGripAt(ev_PosX,ev_PosY))
  {
    if ( sheet->layoutDirection()==Sheet::RightToLeft )
      setCursor( Qt::sizeBDiagCursor );
    else
      setCursor( Qt::sizeFDiagCursor );
    return;
  }

  QRect rct( (d->chooseCell ? choice() : selectionInfo())->lastRange() );

  QRect r1;
  QRect r2;

  double lx = sheet->dblColumnPos( rct.left() );
  double rx = sheet->dblColumnPos( rct.right() + 1 );
  double ty = sheet->dblRowPos( rct.top() );
  double by = sheet->dblRowPos( rct.bottom() + 1 );

  r1.setLeft( (int) (lx - 1) );
  r1.setTop( (int) (ty - 1) );
  r1.setRight( (int) (rx + 1) );
  r1.setBottom( (int) (by + 1) );

  r2.setLeft( (int) (lx + 1) );
  r2.setTop( (int) (ty + 1) );
  r2.setRight( (int) (rx - 1) );
  r2.setBottom( (int) (by - 1) );

  // Test whether the mouse is over some anchor
  {
    Cell *cell = sheet->visibleCellAt( col, row );
    QString anchor;
    if ( sheet->layoutDirection()==Sheet::RightToLeft )
    {
      anchor = cell->testAnchor( d->view->doc()->zoomItX( cell->dblWidth() - ev_PosX + xpos ),
                                 d->view->doc()->zoomItY( ev_PosY - ypos ) );
    }
    else
    {
      anchor = cell->testAnchor( d->view->doc()->zoomItX( ev_PosX - xpos ),
                                 d->view->doc()->zoomItY( ev_PosY - ypos ) );
    }
    if ( !anchor.isEmpty() && anchor != d->anchor )
    {
      setCursor( KCursor::handCursor() );
    }

    d->anchor = anchor;
  }

  // Test wether mouse is over the selection handle
  QRect selectionHandle = d->view->selectionInfo()->selectionHandleArea();
  if ( selectionHandle.contains( QPoint( d->view->doc()->zoomItX( ev_PosX ),
                                         d->view->doc()->zoomItY( ev_PosY ) ) ) )
  {
    //If the cursor is over the handle, than it might be already on the next cell.
    //Recalculate the cell!
    col  = sheet->leftColumn( ev_PosX - d->view->doc()->unzoomItX( 2 ), xpos );
    row  = sheet->topRow( ev_PosY - d->view->doc()->unzoomItY( 2 ), ypos );

    if ( !sheet->isProtected() )
    {
      if ( sheet->layoutDirection()==Sheet::RightToLeft )
        setCursor( Qt::sizeBDiagCursor );
      else
        setCursor( Qt::sizeFDiagCursor );
    }
  }
  else if ( !d->anchor.isEmpty() )
  {
    if ( !sheet->isProtected() )
      setCursor( KCursor::handCursor() );
  }
  else if ( r1.contains( QPoint( (int) ev_PosX, (int) ev_PosY ) )
            && !r2.contains( QPoint( (int) ev_PosX, (int) ev_PosY ) ) )
  {
    setCursor( KCursor::handCursor() );
  }
  else if ( d->chooseCell )
  {
	//Visual cue to indicate that the user can drag-select the choice selection
	setCursor( KCursor::crossCursor() );
  }
  else
  {
	//Nothing special is happening, use a normal arrow cursor
    setCursor( Qt::arrowCursor );
  }

  // No marking, selecting etc. in progess? Then quit here.
  if ( d->mouseAction == NoAction )
    return;

  // Set the new extent of the selection
  (d->chooseCell ? choice() : selectionInfo())->update(QPoint(col,row));
}

void Canvas::mouseReleaseEvent( QMouseEvent* /*_ev*/)
{
  if ( d->scrollTimer->isActive() )
    d->scrollTimer->stop();

  d->mousePressed = false;
  d->view->disableAutoScroll();

  if ( d->modType != MT_NONE /*&& d->m_resizeObject && d->m_resizeObject->isSelected() */)
  {
    switch ( d->modType )
    {
      case MT_MOVE:
      {
        KoPoint move( objectRect( false ).topLeft() - d->m_moveStartPosMouse );
        if ( move != KoPoint( 0, 0 ) )
        {
          KCommand *cmd= activeSheet()->moveObject( view(), move.x(), move.y() );
          if(cmd)
            doc()->addCommand( cmd );
        } else
        {
          repaint();
        }
        d->m_isMoving = false;
        break;
      }
      case MT_RESIZE_UP: case MT_RESIZE_LF: case MT_RESIZE_RT: case MT_RESIZE_LU: case MT_RESIZE_LD: case MT_RESIZE_RU: case MT_RESIZE_RD:
        finishResizeObject( i18n("Resize Object") );
        break;
      case MT_RESIZE_DN:
        finishResizeObject( i18n("Resize Object"), false );
        break;
      default:
        break;
    }
    return;
  }

  Sheet *sheet = activeSheet();
  if ( !sheet )
    return;

  Selection* selectionInfo = d->view->selectionInfo();
  QRect s( selectionInfo->lastRange() );

  // The user started the drag in the lower right corner of the marker ?
  if ( d->mouseAction == ResizeCell && !sheet->isProtected() )
  {
    sheet->mergeCells(selectionInfo->lastRange());
    d->view->updateEditWidget();
  }
  else if ( d->mouseAction == AutoFill && !sheet->isProtected() )
  {
    QRect dest = s;
    sheet->autofill( d->autoFillSource, dest );

    d->view->updateEditWidget();
  }
  // The user started the drag in the middle of a cell ?
  else if ( d->mouseAction == Mark && !d->chooseCell )
  {
    d->view->updateEditWidget();
  }

  d->mouseAction = NoAction;
  d->dragging = false;
  d->dragStart.setX( -1 );
}

void Canvas::processClickSelectionHandle( QMouseEvent *event )
{
  // Auto fill ? That is done using the left mouse button.
  if ( event->button() == Qt::LeftButton )
  {
    d->mouseAction = AutoFill;
    d->autoFillSource = selectionInfo()->lastRange();
  }
  // Resize a cell (done with the right mouse button) ?
  // But for that to work there must not be a selection.
  else if ( event->button() == Qt::MidButton && selectionInfo()->isSingular())
  {
    d->mouseAction = ResizeCell;
  }

  return;
}

void Canvas::processLeftClickAnchor()
{
    bool isRefLink = localReferenceAnchor( d->anchor );
    bool isLocalLink = (d->anchor.indexOf("file:") == 0);
    if ( !isRefLink )
    {
	QString type=KMimeType::findByURL(d->anchor, 0, isLocalLink)->name();

	if ( KRun::isExecutableFile( d->anchor , type ) )
	{
        	//QString question = i18n("Do you want to open this link to '%1'?\n").arg(d->anchor);

          	//question += i18n("Note that opening a link to a local file may "
                          //   "compromise your system's security.");

		QString question = i18n("This link points to the program or script '%1'.\n"
					"Malicious programs can harm your computer.  Are you sure that you want to run this program?").arg(d->anchor);
        	// this will also start local programs, so adding a "don't warn again"
        	// checkbox will probably be too dangerous
        	int choice = KMessageBox::warningYesNo(this, question, i18n("Open Link?"));
        	if ( choice != KMessageBox::Yes )
        	{
			return;
            		//(void) new KRun( d->anchor );
        	}
	}

	new KRun(d->anchor, this);
    }
    else
    {
      selectionInfo()->initialize(Region(d->view, d->anchor));
    }
}

bool Canvas::highlightRangeSizeGripAt(double x, double y)
{
  if (!d->chooseCell)
		return 0;

  Region::ConstIterator end = choice()->constEnd();
  for (Region::ConstIterator it = choice()->constBegin(); it != end; ++it)
  {
    // TODO Stefan: adapt to Selection::selectionHandleArea
    KoRect visibleRect;
    sheetAreaToRect((*it)->rect().normalized(), visibleRect);

    QPoint bottomRight((int) visibleRect.right(), (int) visibleRect.bottom());
    QRect handle( ( (int) bottomRight.x() - 6 ),
                  ( (int) bottomRight.y() - 6 ),
                  ( 6 ),
                  ( 6 ) );

    if (handle.contains(QPoint((int) x,(int) y)))
			{
				return true;
			}
	}

	return false;
}

void Canvas::mousePressEvent( QMouseEvent * _ev )
{
  if ( _ev->button() == Qt::LeftButton )
  {
    d->mousePressed = true;
    d->view->enableAutoScroll();
  }

  if ( activeSheet() && _ev->button() == Qt::LeftButton)
  {
    d->m_moveStartPosMouse = objectRect( false ).topLeft();
    EmbeddedObject *obj = getObject( _ev->pos(), activeSheet() );

    if ( obj )
    {
       // use ctrl + Button to select / deselect object
      if ( _ev->modifiers() & Qt::ControlModifier && obj->isSelected() )
        deselectObject( obj );
      else if ( _ev->modifiers() & Qt::ControlModifier )
      {
        if ( d->modType == MT_NONE)
          return;

        selectObject( obj );
        raiseObject( obj );
        d->m_moveStartPosMouse = objectRect( false ).topLeft();
      }
      else
      {
        if ( d->modType != MT_MOVE || !obj->isSelected() )
            deselectAllObjects();

        selectObject( obj );

        raiseObject( obj );
        d->m_moveStartPosMouse = objectRect( false ).topLeft();
      }

      // start resizing
      if ( d->modType != MT_MOVE && d->modType != MT_NONE && !obj->isProtect() )
      {
        deselectAllObjects();
        selectObject( obj );
        raiseObject( obj );

        d->m_resizeObject = obj;

        d->m_ratio = static_cast<double>( obj->geometry().width() ) /
            static_cast<double>( obj->geometry().height() );
        d->m_rectBeforeResize = obj->geometry();
      }

      KoPoint docPoint ( doc()->unzoomPoint( _ev->pos() ) );
      docPoint += KoPoint( xOffset(), yOffset() );
      d->m_origMousePos = docPoint;
      d->m_moveStartPosMouse = objectRect( false ).topLeft();
      return;
    }
    else
    {
      d->modType = MT_NONE;
      if ( !( _ev->modifiers() & Qt::ShiftModifier ) && !( _ev->state() & Qt::ControlModifier ) )
        deselectAllObjects();
    }
  }

  // Get info about where the event occurred - this is duplicated
  // in ::mouseMoveEvent, needs to be separated into one function
  Sheet *sheet = activeSheet();
  if ( !sheet )
  {
    return;
  }

  double dwidth = d->view->doc()->unzoomItX( width() );
  double ev_PosX;
  if ( sheet->layoutDirection()==Sheet::RightToLeft )
  {
    ev_PosX = dwidth - d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  }
  else
  {
    ev_PosX = d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  }
  double ev_PosY = d->view->doc()->unzoomItY( _ev->pos().y() ) + yOffset();

  // In which cell did the user click ?
  double xpos;
  double ypos;
  int col  = sheet->leftColumn( ev_PosX, xpos );
  int row  = sheet->topRow( ev_PosY, ypos );
  // you cannot move marker when col > KS_colMax or row > KS_rowMax
  if ( col > KS_colMax || row > KS_rowMax )
  {
    kDebug(36001) << "Canvas::mousePressEvent: col or row is out of range: "
                   << "col: " << col << " row: " << row << endl;
    return;
  }

  // you cannot move marker when col > KS_colMax or row > KS_rowMax
  if ( col > KS_colMax || row > KS_rowMax )
  {
    kDebug(36001) << "Canvas::mousePressEvent: col or row is out of range: "
                   << "col: " << col << " row: " << row << endl;
    return;
  }

  if (d->chooseCell && highlightRangeSizeGripAt(ev_PosX,ev_PosY))
  {
    choice()->setActiveElement(QPoint(col,row));
    d->mouseAction = ResizeSelection;
    return;
  }

  // We were editing a cell -> save value and get out of editing mode
  if ( d->cellEditor && !d->chooseCell )
  {
    deleteEditor( true ); // save changes
  }

  d->scrollTimer->start( 50 );

  // Did we click in the lower right corner of the marker/marked-area ?
  if ( selectionInfo()->selectionHandleArea().contains( QPoint( d->view->doc()->zoomItX( ev_PosX ),
                                                                d->view->doc()->zoomItY( ev_PosY ) ) ) )
  {
    processClickSelectionHandle( _ev );
    return;
  }


  // TODO Stefan: adapt to non-cont. selection
  {
    // start drag ?
    QRect rct( selectionInfo()->lastRange() );

    QRect r1;
    QRect r2;
    {
      double lx = sheet->dblColumnPos( rct.left() );
      double rx = sheet->dblColumnPos( rct.right() + 1 );
      double ty = sheet->dblRowPos( rct.top() );
      double by = sheet->dblRowPos( rct.bottom() + 1 );

      r1.setLeft( (int) (lx - 1) );
      r1.setTop( (int) (ty - 1) );
      r1.setRight( (int) (rx + 1) );
      r1.setBottom( (int) (by + 1) );

      r2.setLeft( (int) (lx + 1) );
      r2.setTop( (int) (ty + 1) );
      r2.setRight( (int) (rx - 1) );
      r2.setBottom( (int) (by - 1) );
    }

    d->dragStart.setX( -1 );

    if ( r1.contains( QPoint( (int) ev_PosX, (int) ev_PosY ) )
         && !r2.contains( QPoint( (int) ev_PosX, (int) ev_PosY ) ) )
    {
      d->dragStart.setX( (int) ev_PosX );
      d->dragStart.setY( (int) ev_PosY );

      return;
    }
  }

  //  kDebug() << "Clicked in cell " << col << ", " << row << endl;

  // Extending an existing selection with the shift button ?
  if ((_ev->modifiers() & Qt::ShiftModifier) &&
      d->view->koDocument()->isReadWrite() &&
      !selectionInfo()->isColumnOrRowSelected())
  {
    (d->chooseCell ? choice() : selectionInfo())->update(QPoint(col,row));
    return;
  }


  // Go to the upper left corner of the obscuring object if cells are merged
  Cell *cell = sheet->cellAt( col, row );
  if (cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    col = cell->column();
    row = cell->row();
  }

  switch (_ev->button())
  {
    case Qt::LeftButton:
      if (!d->anchor.isEmpty())
      {
        // Hyperlink pressed
        processLeftClickAnchor();
      }
#ifdef NONCONTIGUOUSSELECTION
      else if ( _ev->modifiers() & Qt::ControlModifier )
      {
        if (d->chooseCell)
        {
#if 0 // TODO Stefan: remove for NCS of choices
          // Start a marking action
          d->mouseAction = Mark;
          // extend the existing selection
          choice()->extend(QPoint(col,row), activeSheet());
#endif
        }
        else
        {
          // Start a marking action
          d->mouseAction = Mark;
          // extend the existing selection
          selectionInfo()->extend(QPoint(col,row), activeSheet());
        }
// TODO Stefan: simplification, if NCS of choices is working
/*        (d->chooseCell ? choice() : selectionInfo())->extend(QPoint(col,row), activeSheet());*/
      }
#endif
      else
      {
        // Start a marking action
        d->mouseAction = Mark;
        // reinitialize the selection
        (d->chooseCell ? choice() : selectionInfo())->initialize(QPoint(col,row), activeSheet());
      }
      break;
    case Qt::MidButton:
      // Paste operation with the middle button?
      if ( d->view->koDocument()->isReadWrite() && !sheet->isProtected() )
      {
        (d->chooseCell ? choice() : selectionInfo())->initialize( QPoint( col, row ), activeSheet() );
        sheet->paste(selectionInfo()->lastRange(), true, Paste::Normal,
                     Paste::OverWrite, false, 0, false, QClipboard::Selection);
        sheet->setRegionPaintDirty(*selectionInfo());
      }
      break;
    case Qt::RightButton:
      if (!selectionInfo()->contains( QPoint( col, row ) ))
      {
        // No selection or the mouse press was outside of an existing selection?
        (d->chooseCell ? choice() : selectionInfo())->initialize(QPoint(col,row), activeSheet());
      }
      break;
    default:
      break;
  }

  scrollToCell(selectionInfo()->marker());
  if ( !d->chooseCell )
  {
    d->view->updateEditWidgetOnPress();
  }
  updatePosWidget();

  // Context menu?
  if ( _ev->button() == Qt::RightButton )
  {
    // TODO: Handle anchor // TODO Stefan: ???
    QPoint p = mapToGlobal( _ev->pos() );
    d->view->openPopupMenu( p );
  }
}

void Canvas::startTheDrag()
{
  Sheet * sheet = activeSheet();
  if ( !sheet )
    return;

  // right area for start dragging
  TextDrag * d = new TextDrag( this );
  setCursor( KCursor::handCursor() );

  QDomDocument doc = sheet->saveCellRegion(*selectionInfo());

  // Save to buffer
  QBuffer buffer;
  buffer.open( QIODevice::WriteOnly );
  QTextStream str( &buffer );
  str.setCodec( "UTF-8" );
  str << doc;
  buffer.close();

  d->setPlain( sheet->copyAsText( selectionInfo() ) );
  d->setKSpread( buffer.buffer() );

  d->dragCopy();
  setCursor( KCursor::arrowCursor() );
}

void Canvas::mouseDoubleClickEvent( QMouseEvent*  _ev)
{

  EmbeddedObject *obj;
  if ( ( obj = getObject( _ev->pos(), activeSheet() ) ) )
  {
    switch ( obj->getType() )
    {
      case OBJECT_KOFFICE_PART: case OBJECT_CHART:
      {
        dynamic_cast<EmbeddedKOfficeObject*>(obj)->activate( view(), this );
        return;
        break;
      }
      default:
      {
        view()->extraProperties();
        return;
        break;
      }
    }
  }

  if ( d->view->koDocument()->isReadWrite() && activeSheet() )
    createEditor(true);
}

void Canvas::wheelEvent( QWheelEvent* _ev )
{
  if ( _ev->orientation() == Qt::Vertical )
  {
    if ( vertScrollBar() )
      QApplication::sendEvent( vertScrollBar(), _ev );
  }
  else if ( horzScrollBar() )
  {
    QApplication::sendEvent( horzScrollBar(), _ev );
  }
}

void Canvas::paintEvent( QPaintEvent* _ev )
{
  if ( d->view->doc()->isLoading() )
    return;

  Sheet* sheet = activeSheet();
  if ( !sheet )
    return;

  // ElapsedTime et( "Canvas::paintEvent" );

  double dwidth = d->view->doc()->unzoomItX( width() );
  KoRect rect = d->view->doc()->unzoomRect( _ev->rect() & QWidget::rect() );
  if ( sheet->layoutDirection()==Sheet::RightToLeft )
    rect.moveBy( -xOffset(), yOffset() );
  else
    rect.moveBy( xOffset(), yOffset() );

  KoPoint tl = rect.topLeft();
  KoPoint br = rect.bottomRight();

  double tmp;
  int left_col;
  int right_col;
  //Philipp: I don't know why we need the +1, but otherwise we don't get it correctly
  //Testcase: Move a dialog slowly up left. Sometimes the top/left most points are not painted
  if ( sheet->layoutDirection()==Sheet::RightToLeft )
  {
    right_col = sheet->leftColumn( dwidth - tl.x(), tmp );
    left_col  = sheet->rightColumn( dwidth - br.x() + 1.0 );
  }
  else
  {
    left_col  = sheet->leftColumn( tl.x(), tmp );
    right_col = sheet->rightColumn( br.x() + 1.0 );
  }
  int top_row = sheet->topRow( tl.y(), tmp );
  int bottom_row = sheet->bottomRow( br.y() + 1.0 );

  QRect vr( QPoint(left_col, top_row),
            QPoint(right_col, bottom_row) );
  d->view->doc()->emitBeginOperation( false );
  sheet->setRegionPaintDirty( vr );
  d->view->doc()->emitEndOperation( vr );
}

void Canvas::focusInEvent( QFocusEvent* )
{
  if ( !d->cellEditor )
    return;

  //kDebug(36001) << "d->chooseCell : " << ( d->chooseCell ? "true" : "false" ) << endl;
  // If we are in editing mode, we redirect the
  // focus to the CellEditor or EditWidget
  // And we know which, using lastEditorWithFocus.
  // This screws up <Tab> though (David)
  if ( lastEditorWithFocus() == EditWidget )
  {
    d->editWidget->setFocus();
    //kDebug(36001) << "Focus to EditWidget" << endl;
    return;
  }

  //kDebug(36001) << "Redirecting focus to editor" << endl;
  d->cellEditor->setFocus();
}

void Canvas::focusOutEvent( QFocusEvent* )
{
    if ( d->scrollTimer->isActive() )
        d->scrollTimer->stop();
    d->mousePressed = false;
    d->view->disableAutoScroll();
}

void Canvas::dragMoveEvent( QDragMoveEvent * _ev )
{
  Sheet * sheet = activeSheet();
  if ( !sheet )
  {
    _ev->ignore();
    return;
  }

  _ev->accept( TextDrag::canDecode( _ev ) );

  double dwidth = d->view->doc()->unzoomItX( width() );
  double xpos = sheet->dblColumnPos( selectionInfo()->lastRange().left() );
  double ypos = sheet->dblRowPos( selectionInfo()->lastRange().top() );
  double width  = sheet->columnFormat( selectionInfo()->lastRange().left() )->dblWidth( this );
  double height = sheet->rowFormat( selectionInfo()->lastRange().top() )->dblHeight( this );

  QRect r1 ((int) xpos - 1, (int) ypos - 1, (int) width + 3, (int) height + 3);

  double ev_PosX;
  if (sheet->layoutDirection()==Sheet::RightToLeft)
    ev_PosX = dwidth - d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  else
    ev_PosX = d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();

  double ev_PosY = d->view->doc()->unzoomItY( _ev->pos().y() ) + yOffset();

  if ( r1.contains( QPoint ((int) ev_PosX, (int) ev_PosY) ) )
    _ev->ignore( r1 );
}

void Canvas::dragLeaveEvent( QDragLeaveEvent * )
{
  if ( d->scrollTimer->isActive() )
    d->scrollTimer->stop();
}

void Canvas::dropEvent( QDropEvent * _ev )
{
  d->dragging = false;
  if ( d->scrollTimer->isActive() )
    d->scrollTimer->stop();
  Sheet * sheet = activeSheet();
  if ( !sheet || sheet->isProtected() )
  {
    _ev->ignore();
    return;
  }

  double dwidth = d->view->doc()->unzoomItX( width() );
  double xpos = sheet->dblColumnPos( selectionInfo()->lastRange().left() );
  double ypos = sheet->dblRowPos( selectionInfo()->lastRange().top() );
  double width  = sheet->columnFormat( selectionInfo()->lastRange().left() )->dblWidth( this );
  double height = sheet->rowFormat( selectionInfo()->lastRange().top() )->dblHeight( this );

  QRect r1 ((int) xpos - 1, (int) ypos - 1, (int) width + 3, (int) height + 3);

  double ev_PosX;
  if (sheet->layoutDirection()==Sheet::RightToLeft)
    ev_PosX = dwidth - d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  else
    ev_PosX = d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();

  double ev_PosY = d->view->doc()->unzoomItY( _ev->pos().y() ) + yOffset();

  if ( r1.contains( QPoint ((int) ev_PosX, (int) ev_PosY) ) )
  {
    _ev->ignore( );
    return;
  }
  else
    _ev->accept( );

  double tmp;
  int col = sheet->leftColumn( ev_PosX, tmp );
  int row = sheet->topRow( ev_PosY, tmp );

  if ( !TextDrag::canDecode( _ev ) )
  {
    _ev->ignore();
    return;
  }

  QByteArray b;

  bool makeUndo = true;

  if ( _ev->provides( TextDrag::selectionMimeType() ) )
  {
    if ( TextDrag::target() == _ev->source() )
    {
      if ( !d->view->doc()->undoLocked() )
      {
        UndoDragDrop * undo
          = new UndoDragDrop(d->view->doc(), sheet, *selectionInfo(),
                             QRect(col, row,
                                   selectionInfo()->boundingRect().width(),
                                   selectionInfo()->boundingRect().height()));
        d->view->doc()->addCommand( undo );
        makeUndo = false;
      }
      sheet->deleteSelection( selectionInfo(), false );
    }


    b = _ev->encodedData( TextDrag::selectionMimeType() );
    sheet->paste( b, QRect( col, row, 1, 1 ), makeUndo );

    if ( _ev->source() == this )
      _ev->acceptAction();
    _ev->accept();
  }
  else
  {
    QString text;
    if ( !Q3TextDrag::decode( _ev, text ) )
    {
      _ev->ignore();
      return;
    }
    //    if ( TextDrag::target() == _ev->source() )
    //      sheet->deleteSelection( selectionInfo() );

    sheet->pasteTextPlain( text, QRect( col, row, 1, 1 ) );
    _ev->accept();
    if ( _ev->source() == this )
      _ev->acceptAction();

    return;
  }
}

void Canvas::resizeEvent( QResizeEvent* _ev )
{
    if (!activeSheet())
	    return;


    double ev_Width = d->view->doc()->unzoomItX( _ev->size().width() );
    double ev_Height = d->view->doc()->unzoomItY( _ev->size().height() );

    // workaround to allow horizontal resizing and zoom changing when sheet
    // direction and interface direction don't match (e.g. an RTL sheet on an
    // LTR interface)
    if ( activeSheet() && activeSheet()->layoutDirection()==Sheet::RightToLeft && !QApplication::isRightToLeft() )
    {
        int dx = _ev->size().width() - _ev->oldSize().width();
        scroll(dx, 0);
    }
    else if ( activeSheet() && activeSheet()->layoutDirection()==Sheet::LeftToRight && QApplication::isRightToLeft() )
    {
        int dx = _ev->size().width() - _ev->oldSize().width();
        scroll(-dx, 0);
    }

    // If we rise horizontally, then check if we are still within the valid area (KS_colMax)
    if ( _ev->size().width() > _ev->oldSize().width() )
    {
        int oldValue = horzScrollBar()->maxValue() - horzScrollBar()->value();

        if ( ( xOffset() + ev_Width ) >
               d->view->doc()->zoomItX( activeSheet()->sizeMaxX() ) )
        {
            horzScrollBar()->setRange( 0, d->view->doc()->zoomItX( activeSheet()->sizeMaxX() - ev_Width ) );
            if ( activeSheet()->layoutDirection()==Sheet::RightToLeft )
                horzScrollBar()->setValue( horzScrollBar()->maxValue() - oldValue );
        }
    }
    // If we lower vertically, then check if the range should represent the maximum range
    else if ( _ev->size().width() < _ev->oldSize().width() )
    {
        int oldValue = horzScrollBar()->maxValue() - horzScrollBar()->value();

        if ( horzScrollBar()->maxValue() ==
             int( d->view->doc()->zoomItX( activeSheet()->sizeMaxX() ) - ev_Width ) )
        {
            horzScrollBar()->setRange( 0, d->view->doc()->zoomItX( activeSheet()->sizeMaxX() - ev_Width ) );
            if ( activeSheet()->layoutDirection()==Sheet::RightToLeft )
                horzScrollBar()->setValue( horzScrollBar()->maxValue() - oldValue );
        }
    }

    // If we rise vertically, then check if we are still within the valid area (KS_rowMax)
    if ( _ev->size().height() > _ev->oldSize().height() )
    {
        if ( ( yOffset() + ev_Height ) >
             d->view->doc()->zoomItY( activeSheet()->sizeMaxY() ) )
        {
            vertScrollBar()->setRange( 0, d->view->doc()->zoomItY( activeSheet()->sizeMaxY() - ev_Height ) );
        }
    }
    // If we lower vertically, then check if the range should represent the maximum range
    else if ( _ev->size().height() < _ev->oldSize().height() )
    {
        if ( vertScrollBar()->maxValue() ==
             int( d->view->doc()->zoomItY( activeSheet()->sizeMaxY() ) - ev_Height ) )
        {
            vertScrollBar()->setRange( 0, d->view->doc()->zoomItY( activeSheet()->sizeMaxY() - ev_Height ) );
        }
    }
}

QPoint Canvas::cursorPos()
{
  QPoint cursor;
  if (d->chooseCell && !choice()->isEmpty())
    cursor = choice()->cursor();
  else
    cursor = selectionInfo()->cursor();

  return cursor;
}

QRect Canvas::moveDirection( KSpread::MoveTo direction, bool extendSelection )
{
  kDebug(36001) << "Canvas::moveDirection" << endl;

  QPoint destination;
  QPoint cursor = cursorPos();

  QPoint cellCorner = cursor;
  Cell* cell = activeSheet()->cellAt(cursor.x(), cursor.y());

  /* cell is either the same as the marker, or the cell that is forced obscuring
     the marker cell
  */
  if (cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    cellCorner = QPoint(cell->column(), cell->row());
  }

  /* how many cells must we move to get to the next cell? */
  int offset = 0;
  RowFormat *rl = NULL;
  ColumnFormat *cl = NULL;
  switch (direction)
    /* for each case, figure out how far away the next cell is and then keep
       going one row/col at a time after that until a visible row/col is found

       NEVER use cell->column() or cell->row() -- it might be a default cell
    */
  {
    case Bottom:
      offset = cell->mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
      rl = activeSheet()->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) <= KS_rowMax) && rl->isHide())
      {
        offset++;
        rl = activeSheet()->rowFormat( cursor.y() + offset );
      }

      destination = QPoint(cursor.x(), qMin(cursor.y() + offset, KS_rowMax));
      break;
    case Top:
      offset = (cellCorner.y() - cursor.y()) - 1;
      rl = activeSheet()->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) >= 1) && rl->isHide())
      {
        offset--;
        rl = activeSheet()->rowFormat( cursor.y() + offset );
      }
      destination = QPoint(cursor.x(), qMax(cursor.y() + offset, 1));
      break;
    case Left:
      offset = (cellCorner.x() - cursor.x()) - 1;
      cl = activeSheet()->columnFormat( cursor.x() + offset );
      while ( ((cursor.x() + offset) >= 1) && cl->isHide())
      {
        offset--;
        cl = activeSheet()->columnFormat( cursor.x() + offset );
      }
      destination = QPoint(qMax(cursor.x() + offset, 1), cursor.y());
      break;
    case Right:
      offset = cell->mergedXCells() - (cursor.x() - cellCorner.x()) + 1;
      cl = activeSheet()->columnFormat( cursor.x() + offset );
      while ( ((cursor.x() + offset) <= KS_colMax) && cl->isHide())
      {
        offset++;
        cl = activeSheet()->columnFormat( cursor.x() + offset );
      }
      destination = QPoint(qMin(cursor.x() + offset, KS_colMax), cursor.y());
      break;
    case BottomFirst:
      offset = cell->mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
      rl = activeSheet()->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) <= KS_rowMax) && rl->isHide())
      {
        ++offset;
        rl = activeSheet()->rowFormat( cursor.y() + offset );
      }

      destination = QPoint( 1, qMin( cursor.y() + offset, KS_rowMax ) );
      break;
  }

  if (extendSelection)
  {
    (d->chooseCell ? choice() : selectionInfo())->update(destination);
  }
  else
  {
    (d->chooseCell ? choice() : selectionInfo())->initialize(destination, activeSheet());
  }
  d->view->updateEditWidget();

  return QRect( cursor, destination );
}

void Canvas::processEnterKey(QKeyEvent* event)
{
  // array is true, if ctrl+alt are pressed
  bool array = (event->modifiers() & Qt::AltModifier) &&
      (event->modifiers() & Qt::ControlModifier);

  /* save changes to the current editor */
  if (!d->chooseCell)
  {
    deleteEditor(true, array);
  }

  /* use the configuration setting to see which direction we're supposed to move
     when enter is pressed.
  */
  KSpread::MoveTo direction = d->view->doc()->getMoveToValue();

  //if shift Button clicked inverse move direction
  if (event->modifiers() & Qt::ShiftModifier)
  {
    switch( direction )
    {
     case Bottom:
      direction = Top;
      break;
     case Top:
      direction = Bottom;
      break;
     case Left:
      direction = Right;
      break;
     case Right:
      direction = Left;
      break;
     case BottomFirst:
      direction = BottomFirst;
      break;
    }
  }

  /* never extend a selection with the enter key -- the shift key reverses
     direction, not extends the selection
  */
  QRect r( moveDirection( direction, false ) );
  d->view->doc()->emitEndOperation( r );
}

void Canvas::processArrowKey( QKeyEvent *event)
{
  /* NOTE:  hitting the tab key also calls this function.  Don't forget
     to account for it
  */

  /* save changes to the current editor */
  if (!d->chooseCell)
  {
    deleteEditor( true );
  }

  KSpread::MoveTo direction = Bottom;
  bool makingSelection = event->modifiers() & Qt::ShiftModifier;

  switch (event->key())
  {
  case Qt::Key_Down:
    direction = Bottom;
    break;
  case Qt::Key_Up:
    direction = Top;
    break;
  case Qt::Key_Left:
    if (activeSheet()->layoutDirection()==Sheet::RightToLeft)
      direction = Right;
    else
      direction = Left;
    break;
  case Qt::Key_Right:
    if (activeSheet()->layoutDirection()==Sheet::RightToLeft)
      direction = Left;
    else
      direction = Right;
    break;
  case Qt::Key_Tab:
      direction = Right;
      break;
  case Qt::Key_Backtab:
      //Shift+Tab moves to the left
      direction = Left;
      makingSelection = false;
      break;
  default:
    Q_ASSERT(false);
    break;
  }

  QRect r( moveDirection( direction, makingSelection ) );
  d->view->doc()->emitEndOperation( r );
}

void Canvas::processEscapeKey(QKeyEvent * event)
{
  if ( d->cellEditor )
    deleteEditor( false );

  if ( view()->isInsertingObject() )
  {
    view()->resetInsertHandle();
    setCursor( Qt::arrowCursor );
    return;
  }

  event->accept(); // ?
  QPoint cursor = cursorPos();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );

  if ( d->mousePressed /*&& toolEditMode == TEM_MOUSE */)
  {
    switch (d->modType)
    {
      case MT_RESIZE_UP:
      case MT_RESIZE_DN:
      case MT_RESIZE_LF:
      case MT_RESIZE_RT:
      case MT_RESIZE_LU:
      case MT_RESIZE_LD:
      case MT_RESIZE_RU:
      case MT_RESIZE_RD:
      {
        QRect oldBoundingRect = doc()->zoomRect( d->m_resizeObject->geometry()/*getRepaintRect()*/);
        d->m_resizeObject->setGeometry( d->m_rectBeforeResize );
        oldBoundingRect.translate( (int)( -xOffset()*doc()->zoomedResolutionX() ) ,
                            (int)( -yOffset() * doc()->zoomedResolutionY()) );

        activeSheet()->setRegionPaintDirty( oldBoundingRect );
        repaint( oldBoundingRect );
        repaintObject( d->m_resizeObject );
        d->m_ratio = 0.0;
        d->m_resizeObject = 0;
        d->m_isResizing = false;
        view()->disableAutoScroll();
        d->mousePressed = false;
        d->modType = MT_NONE;
        break;
      }
      case MT_MOVE:
      {
        if ( d->m_isMoving )
        {
          KoPoint move( d->m_moveStartPoint - objectRect( false ).topLeft() );
          activeSheet()->moveObject( view(), move, false );
          view()->disableAutoScroll();
          d->mousePressed = false;
          d->modType = MT_NONE;
          d->m_isMoving = false;
          update();
        }
        break;
      }
      default:
        break;
    }
  }
}

bool Canvas::processHomeKey(QKeyEvent* event)
{
  bool makingSelection = event->modifiers() & Qt::ShiftModifier;
  Sheet* sheet = activeSheet();

  if ( d->cellEditor )
  // We are in edit mode -> go beginning of line
  {
    QApplication::sendEvent( d->editWidget, event );
    return false;
  }
  else
  {
    QPoint destination;
    /* start at the first used cell in the row and cycle through the right until
       we find a cell that has some output text.  But don't look past the current
       marker.
       The end result we want is to move to the left to the first cell with text,
       or just to the first column if there is no more text to the left.

       But why?  In excel, home key sends you to the first column always.
       We might want to change to that behavior.
    */

    if (event->modifiers() & Qt::ControlModifier)
    {
      /* ctrl + Home will always just send us to location (1,1) */
      destination = QPoint( 1, 1 );
    }
    else
    {
      QPoint marker = d->chooseCell ? choice()->marker() : selectionInfo()->marker();

      Cell * cell = sheet->getFirstCellRow(marker.y());
      while (cell != NULL && cell->column() < marker.x() && cell->isEmpty())
      {
        cell = sheet->getNextCellRight(cell->column(), cell->row());
      }

      int col = ( cell ? cell->column() : 1 );
      if ( col == marker.x())
        col = 1;
      destination = QPoint(col, marker.y());
    }

    if ( selectionInfo()->marker() == destination )
    {
      d->view->doc()->emitEndOperation( QRect( destination, destination ) );
      return false;
    }

    if (makingSelection)
    {
      (d->chooseCell ? choice() : selectionInfo())->update(destination);
    }
    else
    {
      (d->chooseCell ? choice() : selectionInfo())->initialize(destination, activeSheet());
    }
  }
  return true;
}

bool Canvas::processEndKey( QKeyEvent *event )
{
  bool makingSelection = event->modifiers() & Qt::ShiftModifier;
  Sheet* sheet = activeSheet();
  Cell* cell = NULL;
  QPoint marker = d->chooseCell ? choice()->marker() : selectionInfo()->marker();

  // move to the last used cell in the row
  // We are in edit mode -> go beginning of line
  if ( d->cellEditor )
  {
    QApplication::sendEvent( d->editWidget, event );
    d->view->doc()->emitEndOperation( QRect( marker, marker ) );
    return false;
  }
  else
  {
    int col = 1;

    cell = sheet->getLastCellRow(marker.y());
    while (cell != NULL && cell->column() > markerColumn() && cell->isEmpty())
    {
      cell = sheet->getNextCellLeft(cell->column(), cell->row());
    }

    col = (cell == NULL) ? KS_colMax : cell->column();

    QPoint destination( col, marker.y() );
    if ( destination == marker )
    {
      d->view->doc()->emitEndOperation( QRect( destination, destination ) );
      return false;
    }

    if (makingSelection)
    {
      (d->chooseCell ? choice() : selectionInfo())->update(destination);
    }
    else
    {
      (d->chooseCell ? choice() : selectionInfo())->initialize(destination, activeSheet());
    }
  }
  return true;
}

bool Canvas::processPriorKey(QKeyEvent *event)
{
  bool makingSelection = event->modifiers() & Qt::ShiftModifier;
  if (!d->chooseCell)
  {
    deleteEditor( true );
  }

  QPoint marker = d->chooseCell ? choice()->marker() : selectionInfo()->marker();

  QPoint destination(marker.x(), qMax(1, marker.y() - 10));
  if ( destination == marker )
  {
    d->view->doc()->emitEndOperation( QRect( destination, destination ) );
    return false;
  }

  if (makingSelection)
  {
    (d->chooseCell ? choice() : selectionInfo())->update(destination);
  }
  else
  {
    (d->chooseCell ? choice() : selectionInfo())->initialize(destination, activeSheet());
  }
  return true;
}

bool Canvas::processNextKey(QKeyEvent *event)
{
  bool makingSelection = event->modifiers() & Qt::ShiftModifier;

  if (!d->chooseCell)
  {
    deleteEditor( true /*save changes*/ );
  }

  QPoint marker = d->chooseCell ? choice()->marker() : selectionInfo()->marker();
  QPoint destination(marker.x(), qMax(1, marker.y() + 10));

  if ( marker == destination )
  {
    d->view->doc()->emitEndOperation( QRect( destination, destination ) );
    return false;
  }

  if (makingSelection)
  {
    (d->chooseCell ? choice() : selectionInfo())->update(destination);
  }
  else
  {
    (d->chooseCell ? choice() : selectionInfo())->initialize(destination, activeSheet());
  }
  return true;
}

void Canvas::processDeleteKey(QKeyEvent* /* event */)
{
  if ( isObjectSelected() )
  {
    d->view->doc()->emitEndOperation( activeSheet()->visibleRect( this ) );
    d->view->deleteSelectedObjects();
    return;
  }

  activeSheet()->clearTextSelection( selectionInfo() );
  d->editWidget->setText( "" );

  QPoint cursor = cursorPos();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );
  return;
}

void Canvas::processF2Key(QKeyEvent* /* event */)
{
  d->editWidget->setFocus();
  if ( d->cellEditor )
    d->editWidget->setCursorPosition( d->cellEditor->cursorPosition() - 1 );
  d->editWidget->cursorForward( false );


  QPoint cursor = cursorPos();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );
  return;
}

void Canvas::processF4Key(QKeyEvent* event)
{
  /* passes F4 to the editor (if any), which will process it
   */
  if ( d->cellEditor )
  {
    d->cellEditor->handleKeyPressEvent( event );
//    d->editWidget->setFocus();
    d->editWidget->setCursorPosition( d->cellEditor->cursorPosition() );
  }
  QPoint cursor = cursorPos();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );
  return;
}

void Canvas::processOtherKey(QKeyEvent *event)
{
  // No null character ...
  if ( event->text().isEmpty() || !d->view->koDocument()->isReadWrite()
       || !activeSheet() || activeSheet()->isProtected() )
  {
    event->accept();
  }
  else
  {
    if ( !d->cellEditor && !d->chooseCell )
    {
      // Switch to editing mode
      createEditor( CellEditor );
      d->cellEditor->handleKeyPressEvent( event );
    }
    else if ( d->cellEditor )
      d->cellEditor->handleKeyPressEvent( event );
  }

  QPoint cursor = cursorPos();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );

  return;
}

bool Canvas::processControlArrowKey( QKeyEvent *event )
{
  bool makingSelection = event->modifiers() & Qt::ShiftModifier;

  Sheet* sheet = activeSheet();
  Cell* cell = NULL;
  Cell* lastCell;
  QPoint destination;
  bool searchThroughEmpty = true;
  int row;
  int col;

  QPoint marker = d->chooseCell ? choice()->marker() : selectionInfo()->marker();

  /* here, we want to move to the first or last cell in the given direction that is
     actually being used.  Ignore empty cells and cells on hidden rows/columns */
  switch ( event->key() )
  {
    //Ctrl+Qt::Key_Up
   case Qt::Key_Up:

    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.y() != 1))
    {
      lastCell = cell;
      row = marker.y()-1;
      cell = sheet->cellAt(cell->column(), row);
      while ((cell != NULL) && (row > 0) && (!cell->isEmpty()) )
      {
        if (!(sheet->rowFormat(cell->row())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        row--;
        if ( row > 0 )
          cell = sheet->cellAt(cell->column(), row);
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellUp(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->rowFormat(cell->row())->isHide())))
      {
        cell = sheet->getNextCellUp(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      row = 1;
    else
      row = cell->row();

    while ( sheet->rowFormat(row)->isHide() )
    {
      row++;
    }

    destination.setX(marker.x());
    destination.setY(row);
    break;

    //Ctrl+Qt::Key_Down
   case Qt::Key_Down:

    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.y() != KS_rowMax))
    {
      lastCell = cell;
      row = marker.y()+1;
      cell = sheet->cellAt(cell->column(), row);
      while ((cell != NULL) && (row < KS_rowMax) && (!cell->isEmpty()) )
      {
        if (!(sheet->rowFormat(cell->row())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        row++;
        cell = sheet->cellAt(cell->column(), row);
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellDown(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->rowFormat(cell->row())->isHide())))
      {
        cell = sheet->getNextCellDown(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      row = marker.y();
    else
      row = cell->row();

    while ( sheet->rowFormat(row)->isHide() )
    {
      row--;
    }

    destination.setX(marker.x());
    destination.setY(row);
    break;

  //Ctrl+Qt::Key_Left
  case Qt::Key_Left:

  if ( sheet->layoutDirection()==Sheet::RightToLeft )
  {
    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != KS_colMax))
    {
      lastCell = cell;
      col = marker.x()+1;
      cell = sheet->cellAt(col, cell->row());
      while ((cell != NULL) && (col < KS_colMax) && (!cell->isEmpty()) )
      {
        if (!(sheet->columnFormat(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col++;
        cell = sheet->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellRight(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->columnFormat(cell->column())->isHide())))
      {
        cell = sheet->getNextCellRight(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = marker.x();
    else
      col = cell->column();

    while ( sheet->columnFormat(col)->isHide() )
    {
      col--;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
  else
  {
    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != 1))
    {
      lastCell = cell;
      col = marker.x()-1;
      cell = sheet->cellAt(col, cell->row());
      while ((cell != NULL) && (col > 0) && (!cell->isEmpty()) )
      {
        if (!(sheet->columnFormat(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col--;
        if ( col > 0 )
            cell = sheet->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellLeft(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->columnFormat(cell->column())->isHide())))
      {
        cell = sheet->getNextCellLeft(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = 1;
    else
      col = cell->column();

    while ( sheet->columnFormat(col)->isHide() )
    {
      col++;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
    break;

  //Ctrl+Qt::Key_Right
  case Qt::Key_Right:

  if ( sheet->layoutDirection()==Sheet::RightToLeft )
  {
    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != 1))
    {
      lastCell = cell;
      col = marker.x()-1;
      cell = sheet->cellAt(col, cell->row());
      while ((cell != NULL) && (col > 0) && (!cell->isEmpty()) )
      {
        if (!(sheet->columnFormat(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col--;
        if ( col > 0 )
            cell = sheet->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellLeft(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->columnFormat(cell->column())->isHide())))
      {
        cell = sheet->getNextCellLeft(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = 1;
    else
      col = cell->column();

    while ( sheet->columnFormat(col)->isHide() )
    {
      col++;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
  else
  {
    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != KS_colMax))
    {
      lastCell = cell;
      col = marker.x()+1;
      cell = sheet->cellAt(col, cell->row());
      while ((cell != NULL) && (col < KS_colMax) && (!cell->isEmpty()) )
      {
        if (!(sheet->columnFormat(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col++;
        cell = sheet->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellRight(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->columnFormat(cell->column())->isHide())))
      {
        cell = sheet->getNextCellRight(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = marker.x();
    else
      col = cell->column();

    while ( sheet->columnFormat(col)->isHide() )
    {
      col--;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
    break;

  }

  if ( marker == destination )
  {
    d->view->doc()->emitEndOperation( QRect( destination, destination ) );
    return false;
  }

  if (makingSelection)
  {
    (d->chooseCell ? choice() : selectionInfo())->update(destination);
  }
  else
  {
    (d->chooseCell ? choice() : selectionInfo())->initialize(destination, activeSheet());
  }
  return true;
}


void Canvas::keyPressEvent ( QKeyEvent * _ev )
{
  Sheet * sheet = activeSheet();

  if ( !sheet || formatKeyPress( _ev ))
    return;

  // Dont handle the remaining special keys.
  if ( _ev->modifiers() & ( Qt::AltModifier | Qt::ControlModifier ) &&
       (_ev->key() != Qt::Key_Down) &&
       (_ev->key() != Qt::Key_Up) &&
       (_ev->key() != Qt::Key_Right) &&
       (_ev->key() != Qt::Key_Left) &&
       (_ev->key() != Qt::Key_Home) &&
       (_ev->key() != Qt::Key_Enter) &&
       (_ev->key() != Qt::Key_Return) &&
       (_ev->key() != KGlobalSettings::contextMenuKey()))
  {
    QWidget::keyPressEvent( _ev );
    return;
  }

  // Always accept so that events are not
  // passed to the parent.
  _ev->accept();

  d->view->doc()->emitBeginOperation(false);
  if ( _ev->key() == KGlobalSettings::contextMenuKey() ) {
    int row = markerRow();
    int col = markerColumn();
    KoPoint kop(sheet->columnPos(col, this), sheet->rowPos(row, this));
    QPoint p = d->view->doc()->zoomPoint(kop);
    p = mapToGlobal(p);
    d->view->openPopupMenu( p );
  }
  switch( _ev->key() )
  {
   case Qt::Key_Return:
   case Qt::Key_Enter:
    processEnterKey( _ev );
    return;
    break;
   case Qt::Key_Down:
   case Qt::Key_Up:
   case Qt::Key_Left:
   case Qt::Key_Right:
   case Qt::Key_Tab: /* a tab behaves just like a right/left arrow */
   case Qt::Key_Backtab:  /* and so does Shift+Tab */
    if (_ev->modifiers() & Qt::ControlModifier)
    {
      if ( !processControlArrowKey( _ev ) )
        return;
    }
    else
    {
      processArrowKey( _ev );
      return;
    }
    break;

   case Qt::Key_Escape:
    processEscapeKey( _ev );
    return;
    break;

   case Qt::Key_Home:
    if ( !processHomeKey( _ev ) )
      return;
    break;

   case Qt::Key_End:
    if ( !processEndKey( _ev ) )
      return;
    break;

   case Qt::Key_PageUp:  /* Page Up */
    if ( !processPriorKey( _ev ) )
      return;
    break;

   case Qt::Key_PageDown:   /* Page Down */
    if ( !processNextKey( _ev ) )
      return;
    break;

   case Qt::Key_Delete:
    processDeleteKey( _ev );
    return;
    break;

   case Qt::Key_F2:
    processF2Key( _ev );
    return;
    break;

   case Qt::Key_F4:
    processF4Key( _ev );
    return;
    break;

   default:
    processOtherKey( _ev );
    return;
    break;
  }

  //most process*Key methods call emitEndOperation, this only gets called in some situations
  // (after some move operations)
  d->view->doc()->emitEndOperation( sheet->visibleRect( this ) );
  return;
}
#if 0
void Canvas::processIMEvent( QIMEvent * event )
{
  d->view->doc()->emitBeginOperation( false );
  if ( !d->cellEditor && !d->chooseCell )
  {
    // Switch to editing mode
    createEditor( CellEditor );
    d->cellEditor->handleIMEvent( event );
  }

  QPoint cursor;

  if ( d->chooseCell )
  {
    cursor = choice()->cursor();
    /* if the cursor is unset, pretend we're starting at the regular cursor */
    if (cursor.x() == 0 || cursor.y() == 0)
      cursor = choice()->cursor();
  }
  else
    cursor = selectionInfo()->cursor();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );
}
#endif

bool Canvas::formatKeyPress( QKeyEvent * _ev )
{
  if (!(_ev->modifiers() & Qt::ControlModifier ))
    return false;

  int key = _ev->key();
  if ( key != Qt::Key_Exclam && key != Qt::Key_At && key != Qt::Key_Ampersand
       && key != Qt::Key_Dollar && key != Qt::Key_Percent && key != Qt::Key_AsciiCircum
       && key != Qt::Key_NumberSign )
    return false;

  Cell  * cell = 0L;
  Sheet * sheet = activeSheet();

  d->view->doc()->emitBeginOperation(false);

  if ( !d->view->doc()->undoLocked() )
  {
    QString dummy;
    UndoCellFormat * undo = new UndoCellFormat( d->view->doc(), sheet, *selectionInfo(), dummy );
    d->view->doc()->addCommand( undo );
  }

  Region::ConstIterator end(selectionInfo()->constEnd());
  for (Region::ConstIterator it = selectionInfo()->constBegin(); it != end; ++it)
  {
    QRect rect = (*it)->rect().normalized();

  int right  = rect.right();
  int bottom = rect.bottom();

  if ( util_isRowSelected(rect) )
  {
    for ( int r = rect.top(); r <= bottom; ++r )
    {
      cell = sheet->getFirstCellRow( r );
      while ( cell )
      {
        if ( cell->isPartOfMerged() )
        {
          cell = sheet->getNextCellRight( cell->column(), r );
          continue;
        }

        formatCellByKey (cell, _ev->key(), rect);

        cell = sheet->getNextCellRight( cell->column(), r );
      } // while (cell)
      RowFormat * rw = sheet->nonDefaultRowFormat( r );
      QPen pen;
      switch ( _ev->key() )
      {
       case Qt::Key_Exclam:
        rw->setFormatType (Number_format);
        rw->setPrecision( 2 );
        break;

       case Qt::Key_Dollar:
        rw->setFormatType (Money_format);
        rw->setPrecision( d->view->doc()->locale()->fracDigits() );
        break;

       case Qt::Key_Percent:
        rw->setFormatType (Percentage_format);
        break;

       case Qt::Key_At:
        rw->setFormatType( SecondeTime_format );
        break;

       case Qt::Key_NumberSign:
        rw->setFormatType( ShortDate_format );
        break;

       case Qt::Key_AsciiCircum:
        rw->setFormatType( Scientific_format );
        break;

       case Qt::Key_Ampersand:
        if ( r == rect.top() )
        {
          pen = QPen( d->view->borderColor(), 1, Qt::SolidLine);
          rw->setTopBorderPen( pen );
        }
        if ( r == rect.bottom() )
        {
          pen = QPen( d->view->borderColor(), 1, Qt::SolidLine);
          rw->setBottomBorderPen( pen );
        }
        break;

       default:
         d->view->doc()->emitEndOperation( rect );
        return false;
      }
      sheet->emit_updateRow( rw, r );
    }

    d->view->doc()->emitEndOperation( rect );
    return true;
  }

  if ( util_isColumnSelected(rect) )
  {
    for ( int c = rect.left(); c <= right; ++c )
    {
      cell = sheet->getFirstCellColumn( c );
      while ( cell )
      {
        if ( cell->isPartOfMerged() )
        {
          cell = sheet->getNextCellDown( c, cell->row() );
          continue;
        }

        formatCellByKey (cell, _ev->key(), rect);

        cell = sheet->getNextCellDown( c, cell->row() );
      }

      ColumnFormat * cw = sheet->nonDefaultColumnFormat( c );
      QPen pen;
      switch ( _ev->key() )
      {
       case Qt::Key_Exclam:
        cw->setFormatType( Number_format );
        cw->setPrecision( 2 );
        break;

       case Qt::Key_Dollar:
        cw->setFormatType( Money_format );
        cw->setPrecision( d->view->doc()->locale()->fracDigits() );
        break;

       case Qt::Key_Percent:
        cw->setFormatType( Percentage_format );
        break;

       case Qt::Key_At:
        cw->setFormatType( SecondeTime_format );
        break;

       case Qt::Key_NumberSign:
        cw->setFormatType( ShortDate_format );
        break;

       case Qt::Key_AsciiCircum:
        cw->setFormatType( Scientific_format );
        break;

       case Qt::Key_Ampersand:
        if ( c == rect.left() )
        {
          pen = QPen( d->view->borderColor(), 1, Qt::SolidLine);
          cw->setLeftBorderPen( pen );
        }
        if ( c == rect.right() )
        {
          pen = QPen( d->view->borderColor(), 1, Qt::SolidLine);
          cw->setRightBorderPen( pen );
        }
        break;

       default:
         d->view->doc()->emitEndOperation( rect );
         return false;
      }
      sheet->emit_updateColumn( cw, c );
    }
    d->view->doc()->emitEndOperation( rect );
    return true;
  }

  for ( int row = rect.top(); row <= bottom; ++row )
  {
    for ( int col = rect.left(); col <= right; ++ col )
    {
      cell = sheet->nonDefaultCell( col, row );

      if ( cell->isPartOfMerged() )
        continue;

      formatCellByKey (cell, _ev->key(), rect);
    } // for left .. right
  } // for top .. bottom

  }
  _ev->accept();

  d->view->doc()->emitEndOperation( *selectionInfo() );
  return true;
}

bool Canvas::formatCellByKey (Cell *cell, int key, const QRect &rect)
{
  QPen pen;
  switch (key)
  {
    case Qt::Key_Exclam:
    cell->convertToDouble ();
    cell->format()->setFormatType (Number_format);
    cell->format()->setPrecision( 2 );
    break;

    case Qt::Key_Dollar:
    cell->convertToMoney ();
    break;

    case Qt::Key_Percent:
    cell->convertToPercent ();
    break;

    case Qt::Key_At:
    cell->convertToTime ();
    break;

    case Qt::Key_NumberSign:
    cell->convertToDate ();
    break;

    case Qt::Key_AsciiCircum:
    cell->format()->setFormatType (Scientific_format);
    cell->convertToDouble ();
    break;

    case Qt::Key_Ampersand:
    if ( cell->row() == rect.top() )
    {
      pen = QPen( d->view->borderColor(), 1, Qt::SolidLine);
      cell->setTopBorderPen( pen );
    }
    if ( cell->row() == rect.bottom() )
    {
      pen = QPen( d->view->borderColor(), 1, Qt::SolidLine);
      cell->setBottomBorderPen( pen );
    }
    if ( cell->column() == rect.left() )
    {
      pen = QPen( d->view->borderColor(), 1, Qt::SolidLine);
      cell->setLeftBorderPen( pen );
    }
    if ( cell->column() == rect.right() )
    {
      pen = QPen( d->view->borderColor(), 1, Qt::SolidLine);
      cell->setRightBorderPen( pen );
    }
    break;
  } // switch

  return true;
}


void Canvas::slotAutoScroll(const QPoint &scrollDistance)
{
  QPoint d = scrollDistance;
  horzScrollBar()->setValue( horzScrollBar()->value() + d.x() );
  vertScrollBar()->setValue( vertScrollBar()->value() + d.y() );
}

void Canvas::doAutoScroll()
{
    if ( !d->mousePressed )
    {
        d->scrollTimer->stop();
        return;
    }
    bool select = false;
    QPoint pos = mapFromGlobal( QCursor::pos() );

    //Provide progressive scrolling depending on the mouse position
    if ( pos.y() < 0 )
    {
        vertScrollBar()->setValue ((int) (vertScrollBar()->value() -
                                   autoScrollAccelerationY( - pos.y())));
        select = true;
    }
    else if ( pos.y() > height() )
    {
        vertScrollBar()->setValue ((int) (vertScrollBar()->value() +
                                   autoScrollAccelerationY (pos.y() - height())));
        select = true;
    }

    if ( pos.x() < 0 )
    {
        horzScrollBar()->setValue ((int) (horzScrollBar()->value() -
                                   autoScrollAccelerationX( - pos.x() )));
        select = true;
    }
    else if ( pos.x() > width() )
    {
        horzScrollBar()->setValue ((int) (horzScrollBar()->value() +
                                 autoScrollAccelerationX( pos.x() - width())));
        select = true;
    }

    if ( select )
    {
        QMouseEvent * event = new QMouseEvent(QEvent::MouseMove, pos, 0, 0);
        mouseMoveEvent( event );
        delete event;
    }

    //Restart timer
    d->scrollTimer->start( 50 );
}

double Canvas::autoScrollAccelerationX( int offset )
{
    switch( static_cast<int>( offset / 20 ) )
    {
        case 0: return 5.0;
        case 1: return 20.0;
        case 2: return d->view->doc()->unzoomItX( width() );
        case 3: return d->view->doc()->unzoomItX( width() );
        default: return d->view->doc()->unzoomItX( (int) (width() * 5.0) );
    }
}

double Canvas::autoScrollAccelerationY( int offset )
{
    switch( static_cast<int>( offset / 20 ) )
    {
        case 0: return 5.0;
        case 1: return 20.0;
        case 2: return d->view->doc()->unzoomItY( height() );
        case 3: return d->view->doc()->unzoomItY( height() );
        default: return d->view->doc()->unzoomItY( (int) (height() * 5.0) );
    }
}


KSpread::EmbeddedObject *Canvas::getObject( const QPoint &pos, Sheet *_sheet )
{
  QPoint const p ( (int) pos.x() ,
              (int) pos.y() );

  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
  {
    if ( object->sheet() == _sheet )
    {
        KoRect const bound = ( object )->geometry();
        QRect zoomedBound = doc()->zoomRect( KoRect(bound.left(), bound.top(),
                                bound.width(),
                                bound.height() ) );
        zoomedBound.translate( (int)( -xOffset() * doc()->zoomedResolutionX() ), (int)( -yOffset() * doc()->zoomedResolutionY() ) );
         if ( zoomedBound.contains( p ) )
              return object;
    }
  }
  return 0;
}

void Canvas::selectObject( EmbeddedObject *obj )
{
  if ( obj->sheet() != activeSheet() || obj->isSelected() )
    return;
  obj->setSelected( true );
  repaintObject( obj );

  d->mouseSelectedObject = true;
  emit objectSelectedChanged();
  deleteEditor( true );
}

void Canvas::deselectObject( EmbeddedObject *obj )
{
  if ( obj->sheet() != activeSheet() || !obj->isSelected() )
    return;
  obj->setSelected( false );
  repaintObject( obj );

  d->mouseSelectedObject = false;
  emit objectSelectedChanged();
}

void Canvas::selectAllObjects()
{
  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
  {
    if ( object->sheet() == activeSheet() )
      object->setSelected( true );
  }

   d->mouseSelectedObject = true;
//   emit objectSelectedChanged();
}

void Canvas::deselectAllObjects()
{
  if( activeSheet()->numSelected() == 0 )
    return;

  //lowerObject();

  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
      deselectObject( object );

   d->mouseSelectedObject = false;
//   emit objectSelectedChanged();
}



void Canvas::setMouseSelectedObject(bool b)
{
  d->mouseSelectedObject = b;
  emit objectSelectedChanged();
}

bool Canvas::isObjectSelected()
{
  return d->mouseSelectedObject;
}


void Canvas::moveObjectsByMouse( KoPoint &pos, bool keepXorYunchanged )
{
  KoRect rect( objectRect( false ) );
  KoPoint move( 0, 0 );
  double diffx = pos.x() - d->m_origMousePos.x();
  double diffy = pos.y() - d->m_origMousePos.y();

  move = KoPoint( diffx, diffy );
  d->m_origMousePos = pos;

    // unwind last snapping
  KoRect movedRect( rect );
  movedRect.moveBy( diffx, diffy );

    // don't move object off canvas
  KoPoint diffDueToBorders(0,0);
//   KoRect pageRect( m_activePage->getPageRect() );
  if ( rect.left() + move.x() < 0/*pageRect.left()*/ )
    diffDueToBorders.setX( -rect.left() - move.x() );
//   else if ( rect.right() + move.x() > pageRect.right() )
//     diffDueToBorders.setX( pageRect.right() - (rect.right() + move.x()) );


  //kDebug() << "rect.top() + move.y():" << rect.top() + move.y()<< endl;
  if ( rect.top() + move.y() < 0 )
    diffDueToBorders.setY( -rect.top() - move.y() );
//   else if ( rect.bottom() + move.y() > pageRect.bottom() )
//     diffDueToBorders.setY( pageRect.bottom() - (rect.bottom() + move.y()) );

//   m_moveSnapDiff += diffDueToBorders;
  move += diffDueToBorders;

//   movedRect.moveBy( m_moveSnapDiff.x(), m_moveSnapDiff.y() );
  if ( keepXorYunchanged )
  {
    KoPoint diff( d->m_moveStartPosMouse - movedRect.topLeft() );
    if ( fabs( diff.x() ) > fabs( diff.y() ) )
    {
//       m_moveSnapDiff.setY( /*m_moveSnapDiff.y() + */m_moveStartPosMouse.y() - movedRect.y() );
      movedRect.moveTopLeft( KoPoint( movedRect.x(), d->m_moveStartPosMouse.y() ) );
      move.setY( movedRect.y() - rect.y() );
    }
    else
    {
//       m_moveSnapDiff.setX( /*m_moveSnapDiff.x() + */m_moveStartPosMouse.x() - movedRect.x() );
      movedRect.moveTopLeft( KoPoint( d->m_moveStartPosMouse.x(), movedRect.y() ) );
      move.setX( movedRect.x() - rect.x() );
    }
  }

  if ( move != KoPoint( 0, 0 ) )
  {
        //kDebug(33001) << "moveObjectsByMouse move = " << move << endl;
    activeSheet()->moveObject( view(), move, false );
  }
}


void Canvas::resizeObject( ModifyType _modType, const KoPoint & point, bool keepRatio )
{
    EmbeddedObject *obj = d->m_resizeObject;

    KoRect objRect = obj->geometry();
    objRect.moveBy( -xOffset(), -yOffset() );
    QRect oldBoundingRect( doc()->zoomRect( objRect ) );

    bool left = false;
    bool right = false;
    bool top = false;
    bool bottom = false;
    if ( _modType == MT_RESIZE_UP || _modType == MT_RESIZE_LU || _modType == MT_RESIZE_RU )
    {
        top = true;
//         snapStatus |= KoGuides::SNAP_HORIZ;
    }
    if ( _modType == MT_RESIZE_DN || _modType == MT_RESIZE_LD || _modType == MT_RESIZE_RD )
    {
        bottom = true;
//         snapStatus |= KoGuides::SNAP_HORIZ;
    }
    if ( _modType == MT_RESIZE_LF || _modType == MT_RESIZE_LU || _modType == MT_RESIZE_LD )
    {
        left = true;
//         snapStatus |= KoGuides::SNAP_VERT;
    }
    if ( _modType == MT_RESIZE_RT || _modType == MT_RESIZE_RU || _modType == MT_RESIZE_RD )
    {
        right = true;
//         snapStatus |= KoGuides::SNAP_VERT;
    }

    double newLeft = objRect.left();
    double newRight = objRect.right();
    double newTop = objRect.top();
    double newBottom = objRect.bottom();
    if ( top )
    {
        if ( point.y() < objRect.bottom() - MIN_SIZE )
        {
            newTop = point.y();
        }
        else
        {
            newTop = objRect.bottom() - MIN_SIZE;
        }
    }
    if ( bottom )
    {
        if ( point.y() > objRect.top() + MIN_SIZE )
        {
            newBottom = point.y();
        }
        else
        {
            newBottom = objRect.top() + MIN_SIZE;
        }
    }
    if ( left )
    {
        if ( point.x() < objRect.right() - MIN_SIZE )
        {
            newLeft = point.x();
        }
        else
        {
            newLeft = objRect.right() - MIN_SIZE;
        }
    }
    if ( right )
    {
        if ( point.x() > objRect.left() + MIN_SIZE )
        {
            newRight = point.x();
        }
        else
        {
            newRight = objRect.left() + MIN_SIZE;
        }
    }

  double width = newRight - newLeft;
  double height = newBottom - newTop;

  if ( keepRatio && d->m_ratio != 0 )
  {
    if ( ( top || bottom ) && ( right || left ) )
    {
      if ( height * height * d->m_ratio > width * width / d->m_ratio )
      {
        width = height * d->m_ratio;
      }
      else
      {
        height = width / d->m_ratio;
      }
    }
    else if ( top || bottom )
    {
      width = height * d->m_ratio;
    }
    else
    {
      height = width / d->m_ratio;
    }

    if ( top )
    {
      newTop = objRect.bottom() - height;
    }
    else
    {
      newBottom = objRect.top() + height;
    }
    if ( left )
    {
      newLeft = objRect.right() - width;
    }
    else
    {
      newRight = objRect.right() + width;
    }
  }

  if ( newLeft != objRect.left() || newRight != objRect.right() || newTop != objRect.top() || newBottom != objRect.bottom() )
  {
        // resizeBy and moveBy have to been used to make it work with rotated objects
        obj->resizeBy( width - objRect.width(), height - objRect.height() );

        if ( objRect.left() != newLeft || objRect.top() != newTop )
        {
            obj->moveBy( KoPoint( newLeft - objRect.left(), newTop - objRect.top() ) );
        }

//     if ( doc()->showGuideLines() && !m_disableSnapping )
//     {
//       KoRect rect( obj->getRealRect() );
//       KoPoint sp( rect.topLeft() );
//       if ( right )
//       {
//         sp.setX( rect.right() );
//       }
//       if ( bottom )
//       {
//         sp.setY( rect.bottom() );
//       }
//       m_gl.repaintSnapping( sp, snapStatus );
//     }

    repaint( oldBoundingRect );
    repaintObject( obj );
    emit objectSizeChanged();
  }
}


void Canvas::finishResizeObject( const QString &/*name*/, bool /*layout*/ )
{
  if ( d->m_resizeObject )
  {
    KoPoint move = KoPoint( d->m_resizeObject->geometry().x() - d->m_rectBeforeResize.x(),
                            d->m_resizeObject->geometry().y() - d->m_rectBeforeResize.y() );
    KoSize size = KoSize( d->m_resizeObject->geometry().width() - d->m_rectBeforeResize.width(),
                          d->m_resizeObject->geometry().height() - d->m_rectBeforeResize.height() );

    if ( ( d->m_resizeObject->geometry() ) != d->m_rectBeforeResize )
    {
        ChangeObjectGeometryCommand *resizeCmd = new ChangeObjectGeometryCommand( d->m_resizeObject, move, size );
        // the command is not executed as the object is allready resized.
        doc()->addCommand( resizeCmd );
    }

//     if ( layout )
//       doc()->layout( m_resizeObject );

    d->m_ratio = 0.0;
    d->m_isResizing = false;
    repaintObject( d->m_resizeObject );
    d->m_resizeObject = NULL;
  }
}

void Canvas::raiseObject( EmbeddedObject *object )
{
    if ( doc()->embeddedObjects().count() <= 1 )
        return;

    if ( d->m_objectDisplayAbove == 0 )
    {
        if ( activeSheet()->numSelected() == 1 )
        {
            d->m_objectDisplayAbove = object;
        }
    }
}

void Canvas::lowerObject()
{
    d->m_objectDisplayAbove = 0;
}

void Canvas::displayObjectList( QList<EmbeddedObject*> &list )
{
  list = doc()->embeddedObjects();

    if ( d->m_objectDisplayAbove )
    {
        // it can happen that the object is no longer there e.g. when
        // the insert of the object is undone
        int pos = doc()->embeddedObjects().indexOf( d->m_objectDisplayAbove );
        if ( pos != -1 && d->m_objectDisplayAbove->isSelected() )
        {
            list.removeAt( pos );
            list.append( d->m_objectDisplayAbove );
        }
        else
        {
            //tz not possible due to const. should const be removed?
            //m_objectDisplayAbove = 0;
        }
    }
}


KoRect Canvas::objectRect( bool all ) const
{
  return activeSheet()->getRealRect( all );
}

void Canvas::deleteEditor (bool saveChanges, bool array)
{
  if ( !d->cellEditor )
    return;


	//There may be highlighted areas on the sheet which will need to be erased
	setSelectionChangePaintDirty( activeSheet() , *choice() );

  	d->editWidget->setEditMode( false );

  QString t = d->cellEditor->text();
  // Delete the cell editor first and after that update the document.
  // That means we get a synchronous repaint after the cell editor
  // widget is gone. Otherwise we may get painting errors.
  delete d->cellEditor;
  d->cellEditor = 0;

  if ( saveChanges )
  {
      if ( t.at(0)=='=' )
      {
          //a formula
          int openParenthese = t.count( '(' );
          int closeParenthese = t.count( ')' );
          int diff = QABS( openParenthese - closeParenthese );
          if ( openParenthese > closeParenthese )
          {
              for (int i=0; i < diff;i++)
              {
                  t=t+')';
              }
          }
      }
    d->view->setText (t, array);
  }
  else
  {
    d->view->updateEditWidget();
  }

  setFocus();
}


void Canvas::createEditor(bool captureArrowKeys)
{
  if (!activeSheet())
    return;

  Cell * cell = activeSheet()->nonDefaultCell( markerColumn(), markerRow(), false );

  if ( !createEditor( CellEditor , true , captureArrowKeys ) )
      return;
  if ( cell )
      d->cellEditor->setText( cell->text() );
}

bool Canvas::createEditor( EditorType ed, bool addFocus, bool captureArrowKeys )
{
  Sheet * sheet = activeSheet();

  //Ensure that the choice always has a sheet associated
  //with it.
  //
  //FIXME:  This really doesn't make sense.  The concept of
  //the 'choice selection' having only one sheet is wrong - because
  //different parts of the choice selection may be on different
  //sheets (at least, this doesn't work at the moment, but it should
  //do).  REVIEW after KSpread 1.5 has been released.
  //
  //This is a temporary workaround for now.  It fixes the problem
  //where the editor would be hidden because the choice's sheet
  //was not the active sheet.
  if (!choice()->sheet())
  	choice()->setSheet( activeSheet() );

  if ( !d->cellEditor )
  {
    Cell * cell = sheet->nonDefaultCell( marker().x(), marker().y(), false );

    if ( sheet->isProtected() && !cell->format()->notProtected( marker().x(), marker().y() ) )
      return false;

    if ( ed == CellEditor )
    {
      d->editWidget->setEditMode( true );
      d->cellEditor = new KSpread::CellEditor( cell, this, captureArrowKeys );
    }

    double w, h;
    double min_w = cell->dblWidth( markerColumn() );
    double min_h = cell->dblHeight( markerRow() );
    if ( cell->isDefault() )
    {
      w = min_w;
      h = min_h;
      //kDebug(36001) << "DEFAULT" << endl;
    }
    else
    {
      w = cell->extraWidth();
      h = cell->extraHeight();
      //kDebug(36001) << "HEIGHT=" << min_h << " EXTRA=" << h << endl;
    }

    double xpos = sheet->dblColumnPos( markerColumn() ) - xOffset();

    Sheet::LayoutDirection sheetDir = sheet->layoutDirection();
    bool rtlText = cell->strOutText().isRightToLeft();

    // if sheet and cell direction don't match, then the editor's location
    // needs to be shifted backwards so that it's right above the cell's text
    if ( w > 0 && ( ( sheetDir == Sheet::RightToLeft && !rtlText ) ||
                    ( sheetDir == Sheet::LeftToRight && rtlText  ) ) )
      xpos -= w - min_w;

    // paint editor above correct cell if sheet direction is RTL
    if ( sheetDir == Sheet::RightToLeft )
    {
      double dwidth = d->view->doc()->unzoomItX( width() );
      double w2 = qMax( w, min_w );
      xpos = dwidth - w2 - xpos;
    }

    double ypos = sheet->dblRowPos( markerRow() ) - yOffset();
    QPalette p = d->cellEditor->palette();
    QColorGroup g( p.active() );

    QColor color = cell->format()->textColor( markerColumn(), markerRow() );
    if ( !color.isValid() )
        color = QApplication::palette().active().text();
    g.setColor( QColorGroup::Text, color);

    color = cell->bgColor( markerColumn(), markerRow() );
    if ( !color.isValid() )
        color = g.base();
    g.setColor( QColorGroup::Background, color );

    d->cellEditor->setPalette( QPalette( g, p.disabled(), g ) );
    QFont tmpFont = cell->format()->textFont( markerColumn(), markerRow() );
    tmpFont.setPointSizeFloat( 0.01 * d->view->doc()->zoom() * tmpFont.pointSizeFloat() );
    d->cellEditor->setFont( tmpFont );

    KoRect rect( xpos, ypos, w, h ); //needed to circumvent rounding issue with height/width


    QRect zoomedRect=d->view->doc()->zoomRect( rect );
	/*zoomedRect.setLeft(zoomedRect.left()-2);
	zoomedRect.setRight(zoomedRect.right()+4);
	zoomedRect.setTop(zoomedRect.top()-1);
	zoomedRect.setBottom(zoomedRect.bottom()+2);*/

    d->cellEditor->setGeometry( zoomedRect );
    d->cellEditor->setMinimumSize( QSize( d->view->doc()->zoomItX( min_w ), d->view->doc()->zoomItY( min_h ) ) );
    d->cellEditor->show();
    //kDebug(36001) << "FOCUS1" << endl;
    //Laurent 2001-12-05
    //Don't add focus when we create a new editor and
    //we select text in edit widget otherwise we don't delete
    //selected text.
  //  startChoose();

    if ( addFocus )
        d->cellEditor->setFocus();

	setSelectionChangePaintDirty(sheet, *selectionInfo());
	paintUpdates();
  }

  return true;
}

void Canvas::repaintObject( EmbeddedObject *obj )
{
	//Calculate where the object appears on the canvas widget and then repaint that part of the widget
	QRect canvasRelativeGeometry = doc()->zoomRect( obj->geometry() );
	canvasRelativeGeometry.translate( (int)( -xOffset()*doc()->zoomedResolutionX() ) ,
			   			(int)( -yOffset() * doc()->zoomedResolutionY()) );

    update( canvasRelativeGeometry );

 /* if ( !obj->isSelected() )
  {
    KoRect g = obj->geometry();
    g.moveBy( -xOffset(), -yOffset() );
    QRect geometry( doc()->zoomRect( g ) );

    update( geometry );
  }
  else
  {
    QPainter p(this);
    p.translate( -xOffset() * doc()->zoomedResolutionX() , -yOffset() * doc()->zoomedResolutionY() );
    obj->draw(&p); //this goes faster than calling repaint
    p.end();
  }*/
}

void Canvas::copyOasisObjects()
{
    // We'll create a store (ZIP format) in memory
    QBuffer buffer;
    Q3CString mimeType = "application/vnd.oasis.opendocument.spreadsheet";
    KoStore* store = KoStore::createStore( &buffer, KoStore::Write, mimeType );
    Q_ASSERT( store );
    Q_ASSERT( !store->bad() );
    KoOasisStore oasisStore( store );

    KoXmlWriter* manifestWriter = oasisStore.manifestWriter( mimeType );

    QString plainText;
    KoPicture picture;
    if ( !doc()->saveOasisHelper( store, manifestWriter, Doc::SaveSelected, &plainText, &picture )
         || !oasisStore.closeManifestWriter() )
    {
        delete store;
        return;
    }
    delete store;
#warning TODO KDE4 portage
#if 0
    KMultipleDrag* multiDrag = new KMultipleDrag();
    if ( !plainText.isEmpty() )
        multiDrag->addDragObject( new Q3TextDrag( plainText, 0 ) );
    if ( !picture.isNull() )
        multiDrag->addDragObject( picture.dragObject( 0 ) );
    KoStoreDrag* storeDrag = new KoStoreDrag( mimeType, 0 );
    kDebug() << k_funcinfo << "setting zip data: " << buffer.buffer().size() << " bytes." << endl;
    storeDrag->setEncodedData( buffer.buffer() );
    multiDrag->addDragObject( storeDrag );

    //save the objects as pictures too so that other programs can access them
    QList<EmbeddedObject*>::Iterator itObject( doc()->embeddedObjects() );
    itObject.toFirst();
    if ( object )
    {
      KoRect kr = objectRect(false);
      QRect r( kr.toQRect() );
      QPixmap pixmap( r.width(), r.height() );
      pixmap.fill( "white" );
      QPainter p(&pixmap);
      foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
      {
        if ( object->isSelected() )
          p.drawPixmap( object->geometry().toQRect().left() - r.left(), object->geometry().toQRect().top() - r.top(), object->toPixmap( 1.0 , 1.0 ) );
      }
      p.end();
      if (!pixmap.isNull())
      {
        Q3ImageDrag *imagedrag = new Q3ImageDrag( pixmap.convertToImage() );
        multiDrag->addDragObject( imagedrag );
      }
    }

    Q3DragObject *dragObject = multiDrag;
    QApplication::clipboard()->setData( dragObject, QClipboard::Clipboard );
#endif
}

void Canvas::closeEditor()
{
  if ( d->chooseCell )
    return;

  if ( d->cellEditor )
  {
    deleteEditor( true ); // save changes
  }
}

void Canvas::updateEditor()
{
  if (!d->chooseCell)
    return;

  Sheet* sheet = activeSheet();
  if (!sheet)
    return;

  if (d->cellEditor)
  {
    if (choice()->sheet() != sheet)
    {
      d->cellEditor->hide();
    }
    else
    {
      d->cellEditor->show();
    }
    d->cellEditor->updateChoice();
  }
}

void Canvas::setSelectionChangePaintDirty(Sheet* sheet, const Region& region)
{
  sheet->setRegionPaintDirty(region); // TODO should the paintDirtyList be in Canvas?
}


void Canvas::updatePosWidget()
{
    QString buffer;
    // No selection, or only one cell merged selected
    if ( selectionInfo()->isSingular() )
    {
        if (activeSheet()->getLcMode())
        {
            buffer = "L" + QString::number( markerRow() ) +
		"C" + QString::number( markerColumn() );
        }
        else
        {
            buffer = Cell::columnName( markerColumn() ) +
		QString::number( markerRow() );
        }
    }
    else
    {
        if (activeSheet()->getLcMode())
        {
          buffer = QString::number( (selectionInfo()->lastRange().bottom()-selectionInfo()->lastRange().top()+1) )+"Lx";
          if ( util_isRowSelected( selectionInfo()->lastRange() ) )
            buffer+=QString::number((KS_colMax-selectionInfo()->lastRange().left()+1))+"C";
            else
              buffer+=QString::number((selectionInfo()->lastRange().right()-selectionInfo()->lastRange().left()+1))+"C";
        }
        else
        {
                //encodeColumnLabelText return @@@@ when column >KS_colMax
                //=> it's not a good display
                //=> for the moment I display pos of marker
          buffer=Cell::columnName( selectionInfo()->lastRange().left() ) +
                    QString::number(selectionInfo()->lastRange().top()) + ":" +
                    Cell::columnName( qMin( KS_colMax, selectionInfo()->lastRange().right() ) ) +
                    QString::number(selectionInfo()->lastRange().bottom());
                //buffer=activeSheet()->columnLabel( m_iMarkerColumn );
                //buffer+=tmp.setNum(m_iMarkerRow);
        }
  }

    if (buffer != d->posWidget->lineEdit()->text())
      d->posWidget->lineEdit()->setText(buffer);
}

void Canvas::equalizeRow()
{
  QRect s( selection() );
  RowFormat *rl = d->view->activeSheet()->rowFormat(s.top());
  int size=rl->height(this);
  if ( s.top() == s.bottom() )
      return;
  for(int i=s.top()+1;i<=s.bottom();i++)
  {
      Sheet *sheet = activeSheet();
      if ( !sheet )
          return;
      size=qMax(d->view->activeSheet()->rowFormat(i)->height(this),size);
  }
  d->view->vBorderWidget()->equalizeRow(size);
}

void Canvas::equalizeColumn()
{
  QRect s( selection() );
  ColumnFormat *cl = d->view->activeSheet()->columnFormat(s.left());
  int size=cl->width(this);
  if ( s.left() == s.right() )
      return;

  for(int i=s.left()+1;i<=s.right();i++)
  {
    size=qMax(d->view->activeSheet()->columnFormat(i)->width(this),size);
  }
  d->view->hBorderWidget()->equalizeColumn(size);
}

QRect Canvas::cellsInArea( const QRect area ) const
{
	KoRect unzoomedRect = d->view->doc()->unzoomRect( area );

       	unzoomedRect.translate( (int)xOffset(), (int)yOffset() );

  	double tmp;
  	int left_col = activeSheet()->leftColumn( unzoomedRect.left(), tmp );
  	int right_col = activeSheet()->rightColumn( unzoomedRect.right() );
  	int top_row = activeSheet()->topRow( unzoomedRect.top(), tmp );
  	int bottom_row = activeSheet()->bottomRow( unzoomedRect.bottom() );

  	return QRect( left_col, top_row,
                right_col - left_col + 1, bottom_row - top_row + 1 );
}

QRect Canvas::visibleCells() const
{
	return cellsInArea( QRect(0,0,width(),height()) );

}


//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

void Canvas::paintUpdates()
{
  if (activeSheet() == NULL)
    return;

  QPainter painter(this);

  //Save clip region
  QRegion rgnComplete( painter.clipRegion() );
  QMatrix matrix;
  if ( d->view )
  {
    matrix = d->view->matrix();
  }
  else
  {
    matrix = painter.worldMatrix();
  }


  paintChildren( painter, matrix );

  painter.save();
  clipoutChildren( painter );

  KoRect unzoomedRect = d->view->doc()->unzoomRect( QRect( 0, 0, width(), height() ) );
  // unzoomedRect.translate( xOffset(), yOffset() );


  /* paint any visible cell that has the paintDirty flag */
  QRect range = visibleCells();
  Cell* cell = NULL;

  double topPos = activeSheet()->dblRowPos(range.top());
  double leftPos = activeSheet()->dblColumnPos(range.left());

  KoPoint dblCorner( leftPos - xOffset(), topPos - yOffset() );

  int x;
  int y;

  int right  = range.right();
  int bottom = range.bottom();
  Sheet * sheet = activeSheet();

#if 0
  kDebug(36001)
    << "================================================================"
    << endl;
  kDebug(36001) << "painting dirty cells " << endl;
#endif

  QLinkedList<QPoint>  mergedCellsPainted;
  for ( x = range.left(); x <= right; ++x )
  {
    for ( y = range.top(); y <= bottom; ++y )
    {
      if ( sheet->cellIsPaintDirty( QPoint( x, y ) ) )
      {
        cell = sheet->cellAt( x, y );

        // recalc and relayout only for non default cells
        if (!cell->isDefault())
        {
          if (cell->calcDirtyFlag()) cell->calc();
          if (cell->layoutDirtyFlag()) cell->makeLayout( painter, x, y );
        }

       /* bool paintBordersBottom = false;
        bool paintBordersRight = false;
        bool paintBordersLeft = false;
	bool paintBordersTop = false; */

	int paintBorder=Cell::Border_None;

        QPen bottomPen( cell->effBottomBorderPen( x, y ) );
        QPen rightPen( cell->effRightBorderPen( x, y ) );
        QPen leftPen( cell->effLeftBorderPen( x, y ) );
        QPen topPen( cell->effTopBorderPen( x, y ) );

        // paint right border if rightmost cell or if the pen is more "worth" than the left border pen
        // of the cell on the left or if the cell on the right is not painted. In the latter case get
        // the pen that is of more "worth"
        if ( x >= KS_colMax )
         // paintBordersRight = true;
		paintBorder |= Cell::Border_Right;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x + 1, y ) ) )
          {
            //paintBordersRight = true;
		  paintBorder |= Cell::Border_Right;
            if ( cell->effRightBorderValue( x, y ) < sheet->cellAt( x + 1, y )->effLeftBorderValue( x + 1, y ) )
              rightPen = sheet->cellAt( x + 1, y )->effLeftBorderPen( x + 1, y );
          }
        else
        {
         // paintBordersRight = true;
		paintBorder |= Cell::Border_Right;
          if ( cell->effRightBorderValue( x, y ) < sheet->cellAt( x + 1, y )->effLeftBorderValue( x + 1, y ) )
            rightPen = sheet->cellAt( x + 1, y )->effLeftBorderPen( x + 1, y );
        }

        // similiar for other borders...
        // bottom border:
        if ( y >= KS_rowMax )
         // paintBordersBottom = true;
		paintBorder |= Cell::Border_Bottom;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x, y + 1 ) ) )
          {
            if ( cell->effBottomBorderValue( x, y ) > sheet->cellAt( x, y + 1 )->effTopBorderValue( x, y + 1 ) )
             // paintBordersBottom = true;
		    paintBorder |= Cell::Border_Bottom;
          }
        else
        {
          //paintBordersBottom = true;
		paintBorder |= Cell::Border_Bottom;
          if ( cell->effBottomBorderValue( x, y ) < sheet->cellAt( x, y + 1 )->effTopBorderValue( x, y + 1 ) )
            bottomPen = sheet->cellAt( x, y + 1 )->effTopBorderPen( x, y + 1 );
        }

        // left border:
        if ( x == 1 )
         // paintBordersLeft = true;
		paintBorder |= Cell::Border_Left;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x - 1, y ) ) )
          {
           // paintBordersLeft = true;
		  paintBorder |= Cell::Border_Left;
            if ( cell->effLeftBorderValue( x, y ) < sheet->cellAt( x - 1, y )->effRightBorderValue( x - 1, y ) )
              leftPen = sheet->cellAt( x - 1, y )->effRightBorderPen( x - 1, y );
          }
        else
        {
		paintBorder |= Cell::Border_Left;
          if ( cell->effLeftBorderValue( x, y ) < sheet->cellAt( x - 1, y )->effRightBorderValue( x - 1, y ) )
            leftPen = sheet->cellAt( x - 1, y )->effRightBorderPen( x - 1, y );
        }

        // top border:
        if ( y == 1 )
        //  paintBordersTop = true;
		paintBorder |= Cell::Border_Top;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x, y - 1 ) ) )
          {
          //  paintBordersTop = true;
		  paintBorder |= Cell::Border_Top;
            if ( cell->effTopBorderValue( x, y ) < sheet->cellAt( x, y - 1 )->effBottomBorderValue( x, y - 1 ) )
              topPen = sheet->cellAt( x, y - 1 )->effBottomBorderPen( x, y - 1 );
          }
        else
        {
        //  paintBordersTop = true;
		paintBorder |= Cell::Border_Top;
          if ( cell->effTopBorderValue( x, y ) < sheet->cellAt( x, y - 1 )->effBottomBorderValue( x, y - 1 ) )
            topPen = sheet->cellAt( x, y - 1 )->effBottomBorderPen( x, y - 1 );
        }

	cell->paintCell( unzoomedRect, painter, d->view, dblCorner,
			 QPoint( x, y), paintBorder,
			 rightPen,bottomPen,leftPen,topPen,
			 mergedCellsPainted);


      }
      dblCorner.setY( dblCorner.y() + sheet->rowFormat( y )->dblHeight( ) );
    }
    dblCorner.setY( topPos - yOffset() );
    dblCorner.setX( dblCorner.x() + sheet->columnFormat( x )->dblWidth( ) );
  }

  /* now paint the selection */
  //Nb.  No longer necessary to paint choose selection here as the cell reference highlight
  //stuff takes care of this anyway

  paintHighlightedRanges(painter, unzoomedRect);
  paintNormalMarker(painter, unzoomedRect);

  //restore clip region with children area
  painter.restore();
  //painter.setClipRegion( rgnComplete );
}



void Canvas::clipoutChildren( QPainter& painter ) const
{
  QRegion rgn = painter.clipRegion();
  if ( rgn.isEmpty() )
    rgn = QRegion( QRect( 0, 0, width(), height() ) );

  const double horizontalOffset = -xOffset() * doc()->zoomedResolutionX();
  const double verticalOffset = -yOffset() * doc()->zoomedResolutionY();

  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
  {
    if ( ( object )->sheet() == activeSheet() )
    {
	QRect childGeometry = doc()->zoomRect( object->geometry());

	//The clipping region is given in device coordinates
	//so subtract the current offset (scroll position) of the canvas
	childGeometry.translate( (int)horizontalOffset , (int)verticalOffset );

	if (painter.window().intersects(childGeometry))
		rgn -= childGeometry;

      //painter.fillRect( doc()->zoomRect( object->geometry() ), QColor("red" ) );
    }
  }

  painter.setClipRegion( rgn );
}

QRect Canvas::painterWindowGeometry( const QPainter& painter ) const
{
  QRect zoomedWindowGeometry = painter.window();

  zoomedWindowGeometry.translate( (int)( xOffset() * doc()->zoomedResolutionX() ) , (int)( yOffset() * doc()->zoomedResolutionY() ) );

	return zoomedWindowGeometry;
}

void Canvas::paintChildren( QPainter& painter, QMatrix& /*matrix*/ )
{
  if ( doc()->embeddedObjects().isEmpty() )
    return;

  painter.save();
  painter.translate( -xOffset() * doc()->zoomedResolutionX() , -yOffset() * doc()->zoomedResolutionY() );

  const QRect zoomedWindowGeometry = painterWindowGeometry( painter );
  const Sheet* sheet = activeSheet();

  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
  {
    QRect const zoomedObjectGeometry = doc()->zoomRect( object->geometry() );
    if ( ( object )->sheet() == activeSheet() &&
           zoomedWindowGeometry.intersects( zoomedObjectGeometry ) )
    {
	    //To prevent unnecessary redrawing of the embedded object, we only repaint
	    //if one or more of the cells underneath the object has been marked as 'dirty'.

	   QRect canvasRelativeGeometry = zoomedObjectGeometry;
	   canvasRelativeGeometry.translate( (int)( -xOffset()*doc()->zoomedResolutionX() ) ,
			   			(int)( -yOffset() * doc()->zoomedResolutionY()) );

	   const QRect cellsUnderObject=cellsInArea( canvasRelativeGeometry );
	   bool redraw=false;

	   for (int x=cellsUnderObject.left();x<=cellsUnderObject.right();x++)
	   {
		for (int y=cellsUnderObject.top();y<=cellsUnderObject.bottom();y++)
	     		if ( sheet->cellIsPaintDirty( QPoint(x,y) ) )
			{
				redraw=true;
				break;
			}
		if (redraw)
			break;
	   }

       	  if ( redraw )
      		object->draw( &painter );
    }
  }
  painter.restore();
}

void Canvas::paintHighlightedRanges(QPainter& painter, const KoRect& /*viewRect*/)
{
  QList<QColor> colors = choice()->colors();
  QBrush nullBrush;
  int index = 0;
  Region::ConstIterator end(choice()->constEnd());
  for (Region::ConstIterator it = choice()->constBegin(); it != end; ++it)
  {
    //Only paint ranges or cells on the current sheet
    if ((*it)->sheet() != activeSheet())
    {
      index++;
      continue;
    }

    QRect region = (*it)->rect().normalized();

		//double positions[4];
		//bool paintSides[4];
		KoRect unzoomedRect;

		sheetAreaToVisibleRect(region,unzoomedRect);
		//Convert region from sheet coordinates to canvas coordinates for use with the painter
		//retrieveMarkerInfo(region,viewRect,positions,paintSides);

    QPen highlightPen( colors[(index) % colors.size()] ); // (*it)->color() );
		painter.setPen(highlightPen);

		//Adjust the canvas coordinate - rect to take account of zoom level

		QRect zoomedRect;

		zoomedRect.setCoords (	d->view->doc()->zoomItX(unzoomedRect.left()),
					d->view->doc()->zoomItY(unzoomedRect.top()),
					d->view->doc()->zoomItX(unzoomedRect.right()),
					d->view->doc()->zoomItY(unzoomedRect.bottom()) );

		//Now adjust the highlight rectangle is slightly inside the cell borders (this means that multiple highlighted cells
		//look nicer together as the borders do not clash)

		zoomedRect.setLeft(zoomedRect.left()+1);
		zoomedRect.setTop(zoomedRect.top()+1);
		zoomedRect.setRight(zoomedRect.right()-1);
		zoomedRect.setBottom(zoomedRect.bottom()-1);

		painter.setBrush(nullBrush);
		painter.drawRect(zoomedRect);

		//Now draw the size grip (the little rectangle on the bottom right-hand corner of the range which the user can
		//click and drag to resize the region)


    QBrush sizeGripBrush( colors[(index) % colors.size()] ); // (*it)->color());
		QPen sizeGripPen( Qt::white );

		painter.setPen(sizeGripPen);
		painter.setBrush(sizeGripBrush);

		painter.drawRect(zoomedRect.right()-3,zoomedRect.bottom()-3,6,6);
    index++;
  }
}

void Canvas::paintNormalMarker(QPainter& painter, const KoRect &viewRect)
{
  //Only the active element (the one with the anchor) will be drawn with a border

  if( d->chooseCell )
	return;

  if (d->cellEditor)
	return;

	if (!selectionInfo()->activeElement())
			return;

	QRect range=selectionInfo()->activeElement()->rect().normalized();

  	double positions[4];
  	bool paintSides[4];

    bool current = QRect(selectionInfo()->anchor(), selectionInfo()->marker()).normalized() == range;
    QPen pen( Qt::black, 2 );
    painter.setPen( pen );

    retrieveMarkerInfo( selectionInfo()->extendToMergedAreas(range), viewRect, positions, paintSides );

    double left =   positions[0];
    double top =    positions[1];
    double right =  positions[2];
    double bottom = positions[3];

    bool paintLeft =   paintSides[0];
    bool paintTop =    paintSides[1];
    bool paintRight =  paintSides[2];
    bool paintBottom = paintSides[3];

    /* the extra '-1's thrown in here account for the thickness of the pen.
      want to look like this:                     not this:
                              * * * * * *                     * * * *
                              *         *                   *         *
                              *         *                   *         *
    */
    int l = 1;

    if ( paintTop )
    {
      painter.drawLine( d->view->doc()->zoomItX( left ) - l,      d->view->doc()->zoomItY( top ),
                        d->view->doc()->zoomItX( right ) + l, d->view->doc()->zoomItY( top ) );
    }
    if ( activeSheet()->layoutDirection()==Sheet::RightToLeft )
    {
      if ( paintRight )
      {
        painter.drawLine( d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( top ),
                          d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( bottom ) );
      }
      if ( paintLeft && paintBottom && current )
      {
        /* then the 'handle' in the bottom left corner is visible. */
        painter.drawLine( d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( top ),
                          d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( bottom ) - 3 );
        painter.drawLine( d->view->doc()->zoomItX( left ) + 4,  d->view->doc()->zoomItY( bottom ),
                          d->view->doc()->zoomItX( right ) + l + 1, d->view->doc()->zoomItY( bottom ) );
        painter.fillRect( d->view->doc()->zoomItX( left ) - 2, d->view->doc()->zoomItY( bottom ) -2, 5, 5,
                          painter.pen().color() );
      }
      else
      {
        if ( paintLeft )
        {
          painter.drawLine( d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( top ),
                            d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( bottom ) );
        }
        if ( paintBottom )
        {
          painter.drawLine( d->view->doc()->zoomItX( left ) - l,  d->view->doc()->zoomItY( bottom ),
                            d->view->doc()->zoomItX( right ) + l + 1, d->view->doc()->zoomItY( bottom ));
        }
      }
    }
    else // activeSheet()->layoutDirection()==Sheet::LeftToRight
    {
      if ( paintLeft )
      {
        painter.drawLine( d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( top ),
                          d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( bottom ) );
      }
      if ( paintRight && paintBottom && current )
      {
        /* then the 'handle' in the bottom right corner is visible. */
        painter.drawLine( d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( top ),
                          d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( bottom ) - 3 );
        painter.drawLine( d->view->doc()->zoomItX( left ) - l,  d->view->doc()->zoomItY( bottom ),
                          d->view->doc()->zoomItX( right ) - 3, d->view->doc()->zoomItY( bottom ) );
        painter.fillRect( d->view->doc()->zoomItX( right ) - 2, d->view->doc()->zoomItY( bottom ) - 2, 5, 5,
                          painter.pen().color() );
      }
      else
      {
        if ( paintRight )
        {
          painter.drawLine( d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( top ),
                            d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( bottom ) );
        }
        if ( paintBottom )
        {
          painter.drawLine( d->view->doc()->zoomItX( left ) - l,  d->view->doc()->zoomItY( bottom ),
                            d->view->doc()->zoomItX( right ) + l + 1, d->view->doc()->zoomItY( bottom ) );
        }
      }
    }
}

void Canvas::sheetAreaToRect(const QRect& sheetArea, KoRect& rect)
{
	Sheet* sheet=activeSheet();

	if ( sheet->layoutDirection()==Sheet::RightToLeft )
	{
		rect.setLeft(sheet->dblColumnPos( sheetArea.right()+1 ) );
		rect.setRight(sheet->dblColumnPos( sheetArea.left() ));
	}
	else
	{
		rect.setLeft(sheet->dblColumnPos( sheetArea.left() ));
		rect.setRight(sheet->dblColumnPos( sheetArea.right()+1 ));
	}

	rect.setTop(sheet->dblRowPos(sheetArea.top()));
	rect.setBottom(sheet->dblRowPos(sheetArea.bottom()+1));

}

void Canvas::sheetAreaToVisibleRect( const QRect& sheetArea,
					    KoRect& visibleRect )
{
	Sheet* sheet=activeSheet();

	if (!sheet)
		return;

	double dwidth=d->view->doc()->unzoomItX(width());
	double xpos;
	double x;

	if ( sheet->layoutDirection()==Sheet::RightToLeft )
	{
		xpos = dwidth - sheet->dblColumnPos( sheetArea.right() ) + xOffset();
		x    = dwidth - sheet->dblColumnPos( sheetArea.left() ) + xOffset();
	}
	else
	{
		xpos = sheet->dblColumnPos( sheetArea.left() ) - xOffset();
		x    = sheet->dblColumnPos( sheetArea.right() ) - xOffset();
	}

	double ypos = sheet->dblRowPos(sheetArea.top())-yOffset();

	const ColumnFormat *columnFormat = sheet->columnFormat( sheetArea.right() );
	double tw = columnFormat->dblWidth( );
	double w = x - xpos + tw;

	double y = sheet->dblRowPos( sheetArea.bottom() ) - yOffset();
	const RowFormat* rowFormat = sheet->rowFormat( sheetArea.bottom() );
	double th = rowFormat->dblHeight( );
	double h = ( y - ypos ) + th;

	/* left, top, right, bottom */
	if ( sheet->layoutDirection()==Sheet::RightToLeft )
	{
		visibleRect.setLeft(xpos - tw );
		visibleRect.setRight(xpos - tw + w );
	}
	else
	{
		visibleRect.setLeft(xpos );
		visibleRect.setRight(xpos + w );
	}
	visibleRect.setTop(ypos);
	visibleRect.setBottom(ypos + h);
}

void Canvas::retrieveMarkerInfo( const QRect &marker,
                                        const KoRect &viewRect,
                                        double positions[],
                                        bool paintSides[] )
{

	Sheet* sheet=activeSheet();

	if (!sheet) return;

	KoRect visibleRect;
	sheetAreaToVisibleRect(marker,visibleRect);


 /* Sheet * sheet = activeSheet();
  if ( !sheet )
    return;

  double dWidth = d->view->doc()->unzoomItX( width() );

  double xpos;
  double x;
  if ( sheet->layoutDirection()==Sheet::RightToLeft )
  {
    xpos = dWidth - sheet->dblColumnPos( marker.right() ) + xOffset();
    x    = dWidth - sheet->dblColumnPos( marker.left() ) + xOffset();
  }
  else
  {
    xpos = sheet->dblColumnPos( marker.left() ) - xOffset();
    x    = sheet->dblColumnPos( marker.right() ) - xOffset();
  }
  double ypos = sheet->dblRowPos( marker.top() ) - yOffset();

  const ColumnFormat *columnFormat = sheet->columnFormat( marker.right() );
  double tw = columnFormat->dblWidth( );
  double w = x - xpos + tw;

  double y = sheet->dblRowPos( marker.bottom() ) - yOffset();
  const RowFormat* rowFormat = sheet->rowFormat( marker.bottom() );
  double th = rowFormat->dblHeight( );
  double h = ( y - ypos ) + th;

	//left, top, right, bottom
  if ( sheet->layoutDirection()==Sheet::RightToLeft )
  {
    positions[0] = xpos - tw;
    positions[2] = xpos - tw + w;
  }
  else
  {
    positions[0] = xpos;
    positions[2] = xpos + w;
  }
  positions[1] = ypos;
	positions[3] = ypos + h;*/

  /* these vars are used for clarity, the array for simpler function arguments  */
	double left = visibleRect.left();
	double top = visibleRect.top();
	double right = visibleRect.right();
	double bottom = visibleRect.bottom();

  /* left, top, right, bottom */
  paintSides[0] = (viewRect.left() <= left) && (left <= viewRect.right()) &&
                (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  paintSides[1] = (viewRect.top() <= top) && (top <= viewRect.bottom())
               && (right >= viewRect.left()) && (left <= viewRect.right());
  if ( sheet->layoutDirection()==Sheet::RightToLeft )
    paintSides[2] = (viewRect.left() <= right ) &&
                    (right - 1 <= viewRect.right()) &&
                    (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  else
    paintSides[2] = (viewRect.left() <= right ) &&
                    (right <= viewRect.right()) &&
                    (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  paintSides[3] = (viewRect.top() <= bottom) && (bottom <= viewRect.bottom())
               && (right >= viewRect.left()) && (left <= viewRect.right());

  positions[0] = qMax( left,   viewRect.left() );
  positions[1] = qMax( top,    viewRect.top() );
  positions[2] = qMin( right,  viewRect.right() );
  positions[3] = qMin( bottom, viewRect.bottom() );
}


/****************************************************************
 *
 * VBorder
 *
 ****************************************************************/

VBorder::VBorder( QWidget *_parent, Canvas *_canvas, View *_view)
    : QWidget( _parent, "", /*WNorthWestGravity*/Qt::WStaticContents | Qt::WResizeNoErase | Qt::WNoAutoErase )
{
  m_pView = _view;
  m_pCanvas = _canvas;
  m_lSize = 0L;

  setMouseTracking( true );
  m_bResize = false;
  m_bSelection = false;
  m_iSelectionAnchor=1;
  m_bMousePressed = false;

  m_scrollTimer = new QTimer( this );
  connect (m_scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );
}


VBorder::~VBorder()
{
    delete m_scrollTimer;
}

QSize VBorder::sizeHint() const
{
  return QSize( 40, 10 );
}


void VBorder::mousePressEvent( QMouseEvent * _ev )
{
  if ( !m_pView->koDocument()->isReadWrite() )
    return;

  if ( _ev->button() == Qt::LeftButton )
    m_bMousePressed = true;

  const Sheet *sheet = m_pCanvas->activeSheet();
  if (!sheet)
	  return;

  double ev_PosY = m_pCanvas->d->view->doc()->unzoomItY( _ev->pos().y() ) + m_pCanvas->yOffset();
  double dHeight = m_pCanvas->d->view->doc()->unzoomItY( height() );
  m_bResize = false;
  m_bSelection = false;

  // We were editing a cell -> save value and get out of editing mode
  if ( m_pCanvas->editor() )
  {
    m_pCanvas->deleteEditor( true ); // save changes
  }

  m_scrollTimer->start( 50 );

  // Find the first visible row and the y position of this row.
  double y;
  int row = sheet->topRow( m_pCanvas->yOffset(), y );

  // Did the user click between two rows?
  while ( y < ( dHeight + m_pCanvas->yOffset() ) && ( !m_bResize ) )
  {
    double h = sheet->rowFormat( row )->dblHeight();
    row++;
    if ( row > KS_rowMax )
      row = KS_rowMax;
    if ( ( ev_PosY >= y + h - 2 ) &&
         ( ev_PosY <= y + h + 1 ) &&
         !( sheet->rowFormat( row )->isHide() && row == 1 ) )
      m_bResize = true;
    y += h;
  }

  //if row is hide and it's the first row
  //you mustn't resize it.
  double tmp2;
  int tmpRow = sheet->topRow( ev_PosY - 1, tmp2 );
  if ( sheet->rowFormat( tmpRow )->isHide() && tmpRow == 1 )
      m_bResize = false;

  // So he clicked between two rows ?
  if ( m_bResize )
  {
    // Determine row to resize
    double tmp;
    m_iResizedRow = sheet->topRow( ev_PosY - 1, tmp );
    if ( !sheet->isProtected() )
      paintSizeIndicator( _ev->pos().y(), true );
  }
  else
  {
    m_bSelection = true;

    double tmp;
    int hit_row = sheet->topRow( ev_PosY, tmp );
    if ( hit_row > KS_rowMax )
        return;

    m_iSelectionAnchor = hit_row;

    if ( !m_pView->selectionInfo()->contains( QPoint(1, hit_row) ) ||
         !( _ev->button() == Qt::RightButton ) ||
         !m_pView->selectionInfo()->isRowSelected() )
    {
      QPoint newMarker( 1, hit_row );
      QPoint newAnchor( KS_colMax, hit_row );
#ifdef NONCONTIGUOUSSELECTION
      if (_ev->modifiers() == Qt::ControlModifier)
      {
        m_pView->selectionInfo()->extend(QRect(newAnchor, newMarker));
      }
      else
#endif
      if (_ev->modifiers() == Qt::ShiftModifier)
      {
        m_pView->selectionInfo()->update(newMarker);
      }
      else
      {
        m_pView->selectionInfo()->initialize(QRect(newAnchor, newMarker));
      }
    }

    if ( _ev->button() == Qt::RightButton )
    {
      QPoint p = mapToGlobal( _ev->pos() );
      m_pView->popupRowMenu( p );
      m_bSelection = false;
    }
    m_pView->updateEditWidget();
  }
}

void VBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();

    m_bMousePressed = false;

    if ( !m_pView->koDocument()->isReadWrite() )
        return;

    Sheet *sheet = m_pCanvas->activeSheet();
    if (!sheet)
	    return;

    double ev_PosY = m_pCanvas->d->view->doc()->unzoomItY( _ev->pos().y() ) + m_pCanvas->yOffset();

    if ( m_bResize )
    {
        // Remove size indicator painted by paintSizeIndicator
        QPainter painter;
        painter.begin( m_pCanvas );
        painter.setCompositionMode( QPainter::CompositionMode_DestinationOut );
        painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );
        painter.end();

        int start = m_iResizedRow;
        int end = m_iResizedRow;
        QRect rect;
        rect.setCoords( 1, m_iResizedRow, KS_colMax, m_iResizedRow );
        if ( m_pView->selectionInfo()->isRowSelected() )
        {
          if ( m_pView->selectionInfo()->contains( QPoint( 1, m_iResizedRow ) ) )
            {
                start = m_pView->selectionInfo()->lastRange().top();
                end = m_pView->selectionInfo()->lastRange().bottom();
                rect = m_pView->selectionInfo()->lastRange();
            }
        }

        double height = 0.0;
        double y = sheet->dblRowPos( m_iResizedRow );
        if ( ev_PosY - y <= 0.0 )
            height = 0.0;
        else
            height = ev_PosY - y;

        if ( !sheet->isProtected() )
        {
          if ( !m_pCanvas->d->view->doc()->undoLocked() )
          {
            //just resize
            if ( height != 0.0 )
            {
              // TODO Stefan: replace this
              UndoResizeColRow *undo = new UndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), rect );
                m_pCanvas->d->view->doc()->addCommand( undo );
            }
          }

          for( int i = start; i <= end; i++ )
          {
            RowFormat *rl = sheet->nonDefaultRowFormat( i );
            if ( height != 0.0 )
            {
              if ( !rl->isHide() )
                rl->setDblHeight( height );
            }
            else
            {
              sheet->hideRow(*m_pView->selectionInfo());
            }
          }

          delete m_lSize;
          m_lSize = 0;
        }
    }
    else if ( m_bSelection )
    {
      QRect rect = m_pView->selectionInfo()->lastRange();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if ( m_frozen )
        {
            kDebug(36001) << "selected: T " << rect.top() << " B " << rect.bottom() << endl;

            int i;
            RowFormat * row;
            QList<int> hiddenRows;

            for ( i = rect.top(); i <= rect.bottom(); ++i )
            {
                row = m_pView->activeSheet()->rowFormat( i );
                if ( row->isHide() )
                {
                    hiddenRows.append(i);
                }
            }

            if ( hiddenRows.count() > 0 )
              m_pView->activeSheet()->showRow(*m_pView->selectionInfo());
        }
    }

    m_bSelection = false;
    m_bResize = false;
}

void VBorder::equalizeRow( double resize )
{
  Sheet *sheet = m_pCanvas->activeSheet();
  Q_ASSERT( sheet );

  QRect selection( m_pView->selectionInfo()->selection() );
  if ( !m_pCanvas->d->view->doc()->undoLocked() )
  {
     UndoResizeColRow *undo = new UndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), selection );
     m_pCanvas->d->view->doc()->addCommand( undo );
  }
  RowFormat *rl;
  for ( int i = selection.top(); i <= selection.bottom(); i++ )
  {
     rl = sheet->nonDefaultRowFormat( i );
     resize = qMax( 2.0, resize);
     rl->setDblHeight( resize );
  }
}

void VBorder::mouseDoubleClickEvent(QMouseEvent*)
{
  Sheet *sheet = m_pCanvas->activeSheet();
  if (!sheet)
    return;

  if ( !m_pView->koDocument()->isReadWrite() || sheet->isProtected() )
    return;

  sheet->adjustRow(*m_pCanvas->selectionInfo());
}


void VBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  if ( !m_pView->koDocument()->isReadWrite() )
    return;

  Sheet *sheet = m_pCanvas->activeSheet();

  if (!sheet)
     return;

  double ev_PosY = m_pCanvas->d->view->doc()->unzoomItY( _ev->pos().y() ) + m_pCanvas->yOffset();
  double dHeight = m_pCanvas->d->view->doc()->unzoomItY( height() );

  // The button is pressed and we are resizing ?
  if ( m_bResize )
  {
    if ( !sheet->isProtected() )
      paintSizeIndicator( _ev->pos().y(), false );
  }
  // The button is pressed and we are selecting ?
  else if ( m_bSelection )
  {
    double y;
    int row = sheet->topRow( ev_PosY, y );
    if ( row > KS_rowMax )
      return;

    QPoint newAnchor = m_pView->selectionInfo()->anchor();
    QPoint newMarker = m_pView->selectionInfo()->marker();
    newMarker.setY( row );
    newAnchor.setY( m_iSelectionAnchor );
    m_pView->selectionInfo()->update(newMarker);

    if ( _ev->pos().y() < 0 )
      m_pCanvas->vertScrollBar()->setValue( m_pCanvas->d->view->doc()->zoomItY( ev_PosY ) );
    else if ( _ev->pos().y() > m_pCanvas->height() )
    {
      if ( row < KS_rowMax )
      {
        RowFormat *rl = sheet->rowFormat( row + 1 );
        y = sheet->dblRowPos( row + 1 );
        m_pCanvas->vertScrollBar()->setValue ((int) (m_pCanvas->d->view->doc()->zoomItY
              (ev_PosY + rl->dblHeight()) - dHeight));
      }
    }
  }
  // No button is pressed and the mouse is just moved
  else
  {

     //What is the internal size of 1 pixel
    const double unzoomedPixel = m_pCanvas->d->view->doc()->unzoomItY( 1 );
    double y;
    int tmpRow = sheet->topRow( m_pCanvas->yOffset(), y );

    while ( y < m_pCanvas->d->view->doc()->unzoomItY( height() ) + m_pCanvas->yOffset() )
    {
      double h = sheet->rowFormat( tmpRow )->dblHeight();
      //if col is hide and it's the first column
      //you mustn't resize it.
      if ( ev_PosY >= y + h - 2 * unzoomedPixel &&
           ev_PosY <= y + h + unzoomedPixel &&
           !( sheet->rowFormat( tmpRow )->isHide() && tmpRow == 1 ) )
      {
        setCursor( Qt::splitVCursor );
        return;
      }
      y += h;
      tmpRow++;
    }
    setCursor( Qt::arrowCursor );
  }
}

void VBorder::doAutoScroll()
{
    if ( !m_bMousePressed )
    {
        m_scrollTimer->stop();
        return;
    }

    QPoint pos( mapFromGlobal( QCursor::pos() ) );

    if ( pos.y() < 0 || pos.y() > height() )
    {
        QMouseEvent * event = new QMouseEvent( QEvent::MouseMove, pos, 0, 0 );
        mouseMoveEvent( event );
        delete event;
    }

    //Restart timer
    m_scrollTimer->start( 50 );
}

void VBorder::wheelEvent( QWheelEvent* _ev )
{
  if ( m_pCanvas->vertScrollBar() )
    QApplication::sendEvent( m_pCanvas->vertScrollBar(), _ev );
}


void VBorder::paintSizeIndicator( int mouseY, bool firstTime )
{
    Sheet *sheet = m_pCanvas->activeSheet();
    if (!sheet)
	    return;

    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setCompositionMode( QPainter::CompositionMode_DestinationOut );

    if ( !firstTime )
      painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );

    m_iResizePos = mouseY;

    // Dont make the row have a height < 2 pixel.
    int y = m_pCanvas->d->view->doc()->zoomItY( sheet->dblRowPos( m_iResizedRow ) - m_pCanvas->yOffset() );
    if ( m_iResizePos < y + 2 )
        m_iResizePos = y;

    painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );

    painter.end();

    QString tmpSize;
    if ( m_iResizePos != y )
        tmpSize = i18n("Height: %1 %2").arg( KoUnit::toUserValue( m_pCanvas->doc()->unzoomItY( m_iResizePos - y ),
                                                               m_pView->doc()->unit() ) )
                                       .arg( m_pView->doc()->unitName() );
    else
        tmpSize = i18n( "Hide Row" );

    painter.begin( this );
    int len = painter.fontMetrics().width( tmpSize );
    int hei = painter.fontMetrics().height();
    painter.end();

    if ( !m_lSize )
    {
          m_lSize = new QLabel( m_pCanvas );

          if ( sheet->layoutDirection()==Sheet::RightToLeft )
            m_lSize->setGeometry( m_pCanvas->width() - len - 5,
                                                  y + 3, len + 2, hei + 2 );
          else
            m_lSize->setGeometry( 3, y + 3, len + 2,hei + 2 );

          m_lSize->setAlignment( Qt::AlignVCenter );
          m_lSize->setText( tmpSize );
          m_lSize->setPalette( QToolTip::palette() );
          m_lSize->show();
    }
    else
    {
          if ( sheet->layoutDirection()==Sheet::RightToLeft )
            m_lSize->setGeometry( m_pCanvas->width() - len - 5,
                                                  y + 3, len + 2, hei + 2 );
          else
            m_lSize->setGeometry( 3, y + 3, len + 2,hei + 2 );

          m_lSize->setText( tmpSize );
    }
}

void VBorder::updateRows( int from, int to )
{
    Sheet *sheet = m_pCanvas->activeSheet();
    if ( !sheet )
        return;

    int y0 = sheet->rowPos( from, m_pCanvas );
    int y1 = sheet->rowPos( to+1, m_pCanvas );
    update( 0, y0, width(), y1-y0 );
}

void VBorder::paintEvent( QPaintEvent* _ev )
{
  Sheet *sheet = m_pCanvas->activeSheet();
  if ( !sheet )
    return;

  QPainter painter( this );
  QColor highlightColor = View::highlightColor();
  QPen pen( Qt::black, 1 );
  painter.setPen( pen );
  // painter.setBackgroundColor( QColorGroup(palette()).base() );

  // painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  painter.setClipRect( _ev->rect() );

  double yPos;
  //Get the top row and the current y-position
  int y = sheet->topRow( (m_pCanvas->d->view->doc()->unzoomItY( _ev->rect().y() ) + m_pCanvas->yOffset()), yPos );
  //Align to the offset
  yPos = yPos - m_pCanvas->yOffset();
  int width = m_pCanvas->d->view->doc()->zoomItX( YBORDER_WIDTH );

  QFont normalFont = painter.font();
  if ( m_pCanvas->d->view->doc()->zoom() < 100 )
  {
    normalFont.setPointSizeFloat( 0.01 * m_pCanvas->d->view->doc()->zoom() *
                                  normalFont.pointSizeFloat() );
  }
  QFont boldFont = normalFont;
  boldFont.setBold( true );

  //Loop through the rows, until we are out of range
  while ( yPos <= m_pCanvas->d->view->doc()->unzoomItY( _ev->rect().bottom() ) )
  {
    bool selected = (m_pView->selectionInfo()->isRowSelected(y));
    bool highlighted = (!selected && m_pView->selectionInfo()->isRowAffected(y));

    const RowFormat *row_lay = sheet->rowFormat( y );
    int zoomedYPos = m_pCanvas->d->view->doc()->zoomItY( yPos );
    int height = m_pCanvas->d->view->doc()->zoomItY( yPos + row_lay->dblHeight() ) - zoomedYPos;

    if ( selected )
    {
      QBrush fillSelected( highlightColor );
      qDrawPlainRect ( &painter, 0, zoomedYPos, width, height+1, highlightColor.dark(150),
           1, &fillSelected );
    }
    else if ( highlighted )
    {
      QBrush fillHighlighted( highlightColor );
      qDrawPlainRect ( &painter, 0, zoomedYPos, width, height+1, highlightColor.dark(150),
           1, &fillHighlighted );
    }
    else
    {
      QColor c = QColorGroup(palette()).background();
      QBrush fill( c );
      qDrawPlainRect ( &painter, 0, zoomedYPos, width, height+1, c.dark(150),
           1, &fill );
    }

    QString rowText = QString::number( y );

    // Reset painter
    painter.setFont( normalFont );
    painter.setPen( QColorGroup(palette()).text() );

    if ( selected )
      painter.setPen( QColorGroup(palette()).highlightedText() );
    else if ( highlighted )
      painter.setFont( boldFont );

    int len = painter.fontMetrics().width( rowText );
    if (!row_lay->isHide())
        painter.drawText( ( width-len )/2, zoomedYPos +
                          ( height + painter.fontMetrics().ascent() -
                            painter.fontMetrics().descent() ) / 2, rowText );

    yPos += row_lay->dblHeight();
    y++;
  }
}


void VBorder::focusOutEvent( QFocusEvent* )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();
    m_bMousePressed = false;
}


/****************************************************************
 *
 * HBorder
 *
 ****************************************************************/

HBorder::HBorder( QWidget *_parent, Canvas *_canvas,View *_view )
    : QWidget( _parent, "", /*WNorthWestGravity*/ Qt::WStaticContents| Qt::WResizeNoErase | Qt::WNoAutoErase )
{
  m_pView = _view;
  m_pCanvas = _canvas;
  m_lSize = 0L;
  setMouseTracking( true );
  m_bResize = false;
  m_bSelection = false;
  m_iSelectionAnchor=1;
  m_bMousePressed = false;

  m_scrollTimer = new QTimer( this );
  connect( m_scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );
}


HBorder::~HBorder()
{
    delete m_scrollTimer;
}

QSize HBorder::sizeHint() const
{
  return QSize( 40, 10 );
}

void HBorder::mousePressEvent( QMouseEvent * _ev )
{
  if (!m_pView->koDocument()->isReadWrite())
    return;

  if ( _ev->button() == Qt::LeftButton )
    m_bMousePressed = true;

  const Sheet *sheet = m_pCanvas->activeSheet();
  if (!sheet)
	return;

  // We were editing a cell -> save value and get out of editing mode
  if ( m_pCanvas->editor() )
  {
      m_pCanvas->deleteEditor( true ); // save changes
  }

  m_scrollTimer->start( 50 );

  double ev_PosX;
  double dWidth = m_pCanvas->d->view->doc()->unzoomItX( width() );
  if ( sheet->layoutDirection()==Sheet::RightToLeft )
    ev_PosX = dWidth - m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
  else
    ev_PosX = m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
  m_bResize = false;
  m_bSelection = false;

  // Find the first visible column and the x position of this column.
  double x;

  const double unzoomedPixel = m_pCanvas->d->view->doc()->unzoomItX( 1 );
  if ( sheet->layoutDirection()==Sheet::RightToLeft )
  {
    int tmpCol = sheet->leftColumn( m_pCanvas->xOffset(), x );

    kDebug() << "evPos: " << ev_PosX << ", x: " << x << ", COL: " << tmpCol << endl;
    while ( ev_PosX > x && ( !m_bResize ) )
    {
      double w = sheet->columnFormat( tmpCol )->dblWidth();

      kDebug() << "evPos: " << ev_PosX << ", x: " << x << ", w: " << w << ", COL: " << tmpCol << endl;

      ++tmpCol;
      if ( tmpCol > KS_colMax )
        tmpCol = KS_colMax;
      //if col is hide and it's the first column
      //you mustn't resize it.

      if ( ev_PosX >= x + w - unzoomedPixel &&
           ev_PosX <= x + w + unzoomedPixel &&
           !( sheet->columnFormat( tmpCol )->isHide() && tmpCol == 1 ) )
      {
        m_bResize = true;
      }
      x += w;
    }

    //if col is hide and it's the first column
    //you mustn't resize it.
    double tmp2;
    tmpCol = sheet->leftColumn( dWidth - ev_PosX + 1, tmp2 );
    if ( sheet->columnFormat( tmpCol )->isHide() && tmpCol == 0 )
    {
      kDebug() << "No resize: " << tmpCol << ", " << sheet->columnFormat( tmpCol )->isHide() << endl;
      m_bResize = false;
    }

    kDebug() << "Resize: " << m_bResize << endl;
  }
  else
  {
    int col = sheet->leftColumn( m_pCanvas->xOffset(), x );

    // Did the user click between two columns?
    while ( x < ( dWidth + m_pCanvas->xOffset() ) && ( !m_bResize ) )
    {
      double w = sheet->columnFormat( col )->dblWidth();
      col++;
      if ( col > KS_colMax )
        col = KS_colMax;
      if ( ( ev_PosX >= x + w - unzoomedPixel ) &&
         ( ev_PosX <= x + w + unzoomedPixel ) &&
           !( sheet->columnFormat( col )->isHide() && col == 1 ) )
        m_bResize = true;
      x += w;
    }

    //if col is hide and it's the first column
    //you mustn't resize it.
    double tmp2;
    int tmpCol = sheet->leftColumn( ev_PosX - 1, tmp2 );
    if ( sheet->columnFormat( tmpCol )->isHide() && tmpCol == 1 )
      m_bResize = false;
  }

  // So he clicked between two rows ?
  if ( m_bResize )
  {
    // Determine the column to resize
    double tmp;
    if ( sheet->layoutDirection()==Sheet::RightToLeft )
    {
      m_iResizedColumn = sheet->leftColumn( ev_PosX - 1, tmp );
      // kDebug() << "RColumn: " << m_iResizedColumn << ", PosX: " << ev_PosX << endl;

      if ( !sheet->isProtected() )
        paintSizeIndicator( _ev->pos().x(), true );
    }
    else
    {
      m_iResizedColumn = sheet->leftColumn( ev_PosX - 1, tmp );

      if ( !sheet->isProtected() )
        paintSizeIndicator( _ev->pos().x(), true );
    }

    // kDebug() << "Column: " << m_iResizedColumn << endl;
  }
  else
  {
    m_bSelection = true;

    double tmp;
    int hit_col = sheet->leftColumn( ev_PosX, tmp );
    if ( hit_col > KS_colMax )
        return;

    m_iSelectionAnchor = hit_col;

    if ( !m_pView->selectionInfo()->contains( QPoint( hit_col, 1 ) ) ||
         !( _ev->button() == Qt::RightButton ) ||
         !m_pView->selectionInfo()->isColumnSelected() )
    {
      QPoint newMarker( hit_col, 1 );
      QPoint newAnchor( hit_col, KS_rowMax );
#ifdef NONCONTIGUOUSSELECTION
      if (_ev->modifiers() == Qt::ControlModifier)
      {
        m_pView->selectionInfo()->extend(QRect(newAnchor, newMarker));
      }
      else
#endif
      if (_ev->modifiers() == Qt::ShiftModifier)
      {
        m_pView->selectionInfo()->update(newMarker);
      }
      else
      {
        m_pView->selectionInfo()->initialize(QRect(newAnchor, newMarker));
      }
    }

    if ( _ev->button() == Qt::RightButton )
    {
      QPoint p = mapToGlobal( _ev->pos() );
      m_pView->popupColumnMenu( p );
      m_bSelection = false;
    }
    m_pView->updateEditWidget();
  }
}

void HBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();

    m_bMousePressed = false;

    if ( !m_pView->koDocument()->isReadWrite() )
      return;

    Sheet * sheet = m_pCanvas->activeSheet();
    if (!sheet)
	    return;

    if ( m_bResize )
    {
        double dWidth = m_pCanvas->d->view->doc()->unzoomItX( width() );
        double ev_PosX;

        // Remove size indicator painted by paintSizeIndicator
        QPainter painter;
        painter.begin( m_pCanvas );
        painter.setCompositionMode( QPainter::CompositionMode_DestinationOut );
        painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );
        painter.end();

        int start = m_iResizedColumn;
        int end   = m_iResizedColumn;
        QRect rect;
        rect.setCoords( m_iResizedColumn, 1, m_iResizedColumn, KS_rowMax );
        if ( m_pView->selectionInfo()->isColumnSelected() )
        {
            if ( m_pView->selectionInfo()->contains( QPoint( m_iResizedColumn, 1 ) ) )
            {
                start = m_pView->selectionInfo()->lastRange().left();
                end   = m_pView->selectionInfo()->lastRange().right();
                rect  = m_pView->selectionInfo()->lastRange();
            }
        }

        double width = 0.0;
        double x;

        if ( sheet->layoutDirection()==Sheet::RightToLeft )
          ev_PosX = dWidth - m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
        else
          ev_PosX = m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();

        x = sheet->dblColumnPos( m_iResizedColumn );

        if ( ev_PosX - x <= 0.0 )
          width = 0.0;
        else
          width = ev_PosX - x;

        if ( !sheet->isProtected() )
        {
          if ( !m_pCanvas->d->view->doc()->undoLocked() )
          {
            //just resize
            if ( width != 0.0 )
            {
              // TODO Stefan: replace this
                UndoResizeColRow *undo = new UndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), rect );
                m_pCanvas->d->view->doc()->addCommand( undo );
            }
          }

          for( int i = start; i <= end; i++ )
          {
            ColumnFormat *cl = sheet->nonDefaultColumnFormat( i );
            if ( width != 0.0 )
            {
                if ( !cl->isHide() )
                    cl->setDblWidth( width );
            }
            else
            {
              sheet->hideColumn(*m_pView->selectionInfo());
            }
          }

          delete m_lSize;
          m_lSize = 0;
        }
    }
    else if ( m_bSelection )
    {
        QRect rect = m_pView->selectionInfo()->lastRange();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if ( m_frozen )
        {
            kDebug(36001) << "selected: L " << rect.left() << " R " << rect.right() << endl;

            int i;
            ColumnFormat * col;
            QList<int> hiddenCols;

            for ( i = rect.left(); i <= rect.right(); ++i )
            {
                col = m_pView->activeSheet()->columnFormat( i );
                if ( col->isHide() )
                {
                    hiddenCols.append(i);
                }
            }

            if ( hiddenCols.count() > 0 )
              m_pView->activeSheet()->showColumn(*m_pView->selectionInfo());
        }
    }

    m_bSelection = false;
    m_bResize = false;
}

void HBorder::equalizeColumn( double resize )
{
  Sheet *sheet = m_pCanvas->activeSheet();
  Q_ASSERT( sheet );

  QRect selection( m_pView->selectionInfo()->selection() );
  if ( !m_pCanvas->d->view->doc()->undoLocked() )
  {
      UndoResizeColRow *undo = new UndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), selection );
      m_pCanvas->d->view->doc()->addCommand( undo );
  }
  ColumnFormat *cl;
  for ( int i = selection.left(); i <= selection.right(); i++ )
  {
      cl = sheet->nonDefaultColumnFormat( i );
      resize = qMax( 2.0, resize );
      cl->setDblWidth( resize );
  }

}

void HBorder::mouseDoubleClickEvent(QMouseEvent*)
{
  Sheet *sheet = m_pCanvas->activeSheet();
  if (!sheet)
    return;

  if ( !m_pView->koDocument()->isReadWrite() || sheet->isProtected() )
    return;

  sheet->adjustColumn(*m_pCanvas->selectionInfo());
}

void HBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  if ( !m_pView->koDocument()->isReadWrite() )
    return;

  Sheet *sheet = m_pCanvas->activeSheet();

  if (!sheet)
    return;

  double dWidth = m_pCanvas->d->view->doc()->unzoomItX( width() );
  double ev_PosX;
  if ( sheet->layoutDirection()==Sheet::RightToLeft )
    ev_PosX = dWidth - m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
  else
    ev_PosX = m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();

  // The button is pressed and we are resizing ?
  if ( m_bResize )
  {
    if ( !sheet->isProtected() )
        paintSizeIndicator( _ev->pos().x(), false );
  }
  // The button is pressed and we are selecting ?
  else if ( m_bSelection )
  {
    double x;
    int col = sheet->leftColumn( ev_PosX, x );

    if ( col > KS_colMax )
      return;

    QPoint newMarker = m_pView->selectionInfo()->marker();
    QPoint newAnchor = m_pView->selectionInfo()->anchor();
    newMarker.setX( col );
    newAnchor.setX( m_iSelectionAnchor );
    m_pView->selectionInfo()->update(newMarker);

    if ( sheet->layoutDirection()==Sheet::RightToLeft )
    {
      if ( _ev->pos().x() < width() - m_pCanvas->width() )
      {
        ColumnFormat *cl = sheet->columnFormat( col + 1 );
        x = sheet->dblColumnPos( col + 1 );
        m_pCanvas->horzScrollBar()->setValue ( m_pCanvas->horzScrollBar()->maxValue() - (int)
            (m_pCanvas->d->view->doc()->zoomItX (ev_PosX + cl->dblWidth()) - m_pCanvas->d->view->doc()->unzoomItX( m_pCanvas->width() )));
      }
      else if ( _ev->pos().x() > width() )
        m_pCanvas->horzScrollBar()->setValue( m_pCanvas->horzScrollBar()->maxValue() - m_pCanvas->d->view->doc()->zoomItX( ev_PosX - dWidth + m_pCanvas->d->view->doc()->unzoomItX( m_pCanvas->width() ) ) );
    }
    else
    {
      if ( _ev->pos().x() < 0 )
        m_pCanvas->horzScrollBar()->setValue( m_pCanvas->d->view->doc()->zoomItX( ev_PosX ) );
      else if ( _ev->pos().x() > m_pCanvas->width() )
      {
        if ( col < KS_colMax )
        {
          ColumnFormat *cl = sheet->columnFormat( col + 1 );
          x = sheet->dblColumnPos( col + 1 );
          m_pCanvas->horzScrollBar()->setValue ((int)
              (m_pCanvas->d->view->doc()->zoomItX (ev_PosX + cl->dblWidth()) - dWidth));
        }
      }
    }

  }
  // No button is pressed and the mouse is just moved
  else
  {
     //What is the internal size of 1 pixel
    const double unzoomedPixel = m_pCanvas->d->view->doc()->unzoomItX( 1 );
    double x;

    if ( sheet->layoutDirection()==Sheet::RightToLeft )
    {
      int tmpCol = sheet->leftColumn( m_pCanvas->xOffset(), x );

      while ( ev_PosX > x )
      {
        double w = sheet->columnFormat( tmpCol )->dblWidth();
        ++tmpCol;

        //if col is hide and it's the first column
        //you mustn't resize it.
        if ( ev_PosX >= x + w - unzoomedPixel &&
             ev_PosX <= x + w + unzoomedPixel &&
             !( sheet->columnFormat( tmpCol )->isHide() && tmpCol == 0 ) )
        {
          setCursor( Qt::splitHCursor );
          return;
        }
        x += w;
      }
      setCursor( Qt::arrowCursor );
    }
    else
    {
      int tmpCol = sheet->leftColumn( m_pCanvas->xOffset(), x );

      while ( x < m_pCanvas->d->view->doc()->unzoomItY( width() ) + m_pCanvas->xOffset() )
      {
        double w = sheet->columnFormat( tmpCol )->dblWidth();
        //if col is hide and it's the first column
        //you mustn't resize it.
        if ( ev_PosX >= x + w - unzoomedPixel &&
             ev_PosX <= x + w + unzoomedPixel &&
             !( sheet->columnFormat( tmpCol )->isHide() && tmpCol == 1 ) )
        {
          setCursor( Qt::splitHCursor );
          return;
        }
        x += w;
        tmpCol++;
      }
      setCursor( Qt::arrowCursor );
    }
  }
}

void HBorder::doAutoScroll()
{
    if ( !m_bMousePressed )
    {
        m_scrollTimer->stop();
        return;
    }

    QPoint pos( mapFromGlobal( QCursor::pos() ) );

    if ( pos.x() < 0 || pos.x() > width() )
    {
        QMouseEvent * event = new QMouseEvent( QEvent::MouseMove, pos, 0, 0 );
        mouseMoveEvent( event );
        delete event;
    }

    //Restart timer
    m_scrollTimer->start( 50 );
}

void HBorder::wheelEvent( QWheelEvent* _ev )
{
  if ( m_pCanvas->horzScrollBar() )
    QApplication::sendEvent( m_pCanvas->horzScrollBar(), _ev );
}

void HBorder::resizeEvent( QResizeEvent* _ev )
{
  // workaround to allow horizontal resizing and zoom changing when sheet
  // direction and interface direction don't match (e.g. an RTL sheet on an
  // LTR interface)
  if ( m_pCanvas->activeSheet() && m_pCanvas->activeSheet()->layoutDirection()==Sheet::RightToLeft && !QApplication::isRightToLeft() )
  {
    int dx = _ev->size().width() - _ev->oldSize().width();
    scroll(dx, 0);
  }
  else if ( m_pCanvas->activeSheet() && m_pCanvas->activeSheet()->layoutDirection()==Sheet::LeftToRight && QApplication::isRightToLeft() )
  {
    int dx = _ev->size().width() - _ev->oldSize().width();
    scroll(-dx, 0);
  }
}

void HBorder::paintSizeIndicator( int mouseX, bool firstTime )
{
    Sheet *sheet = m_pCanvas->activeSheet();
    if (!sheet)
	    return;

    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setCompositionMode( QPainter::CompositionMode_DestinationOut );

    if ( !firstTime )
      painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );

    if ( sheet->layoutDirection()==Sheet::RightToLeft )
      m_iResizePos = mouseX + m_pCanvas->width() - width();
    else
      m_iResizePos = mouseX;

    // Dont make the column have a width < 2 pixels.
    int x = m_pCanvas->d->view->doc()->zoomItX( sheet->dblColumnPos( m_iResizedColumn ) - m_pCanvas->xOffset() );

    if ( sheet->layoutDirection()==Sheet::RightToLeft )
    {
      x = m_pCanvas->width() - x;

      if ( m_iResizePos > x - 2 )
          m_iResizePos = x;
    }
    else
    {
      if ( m_iResizePos < x + 2 )
          m_iResizePos = x;
    }

    painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );

    painter.end();

    QString tmpSize;
    if ( m_iResizePos != x )
        tmpSize = i18n("Width: %1 %2")
                  .arg( KGlobal::locale()->formatNumber( KoUnit::toUserValue( m_pCanvas->doc()->unzoomItX( (sheet->layoutDirection()==Sheet::RightToLeft) ? x - m_iResizePos : m_iResizePos - x ),
                                                                           m_pView->doc()->unit() )))
                  .arg( m_pView->doc()->unitName() );
    else
        tmpSize = i18n( "Hide Column" );

    painter.begin( this );
    int len = painter.fontMetrics().width( tmpSize );
    int hei = painter.fontMetrics().height();
    painter.end();

    if ( !m_lSize )
    {
        m_lSize = new QLabel( m_pCanvas );

        if ( sheet->layoutDirection()==Sheet::RightToLeft )
          m_lSize->setGeometry( x - len - 5, 3, len + 2, hei + 2 );
        else
          m_lSize->setGeometry( x + 3, 3, len + 2, hei + 2 );

        m_lSize->setAlignment( Qt::AlignVCenter );
        m_lSize->setText( tmpSize );
        m_lSize->setPalette( QToolTip::palette() );
        m_lSize->show();
    }
    else
    {
        if ( sheet->layoutDirection()==Sheet::RightToLeft )
          m_lSize->setGeometry( x - len - 5, 3, len + 2, hei + 2 );
        else
          m_lSize->setGeometry( x + 3, 3, len + 2, hei + 2 );

        m_lSize->setText( tmpSize );
    }
}

void HBorder::updateColumns( int from, int to )
{
    Sheet *sheet = m_pCanvas->activeSheet();
    if ( !sheet )
        return;

    int x0 = sheet->columnPos( from, m_pCanvas );
    int x1 = sheet->columnPos( to+1, m_pCanvas );
    update( x0, 0, x1-x0, height() );
}

void HBorder::paintEvent( QPaintEvent* _ev )
{
  Sheet * sheet = m_pCanvas->activeSheet();
  if ( !sheet )
    return;

  QColor   highlightColor = View::highlightColor();
  QPainter painter( this );
  QPen pen( Qt::black, 1 );
  painter.setPen( pen );

  painter.setClipRect( _ev->rect() );

  // painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  double xPos;
  int x;

  if ( sheet->layoutDirection()==Sheet::RightToLeft )
  {
    //Get the left column and the current x-position
    x = sheet->leftColumn( int( m_pCanvas->d->view->doc()->unzoomItX( width() ) - m_pCanvas->d->view->doc()->unzoomItX( _ev->rect().x() ) + m_pCanvas->xOffset() ), xPos );
    //Align to the offset
    xPos = m_pCanvas->d->view->doc()->unzoomItX( width() ) - xPos + m_pCanvas->xOffset();
  }
  else
  {
    //Get the left column and the current x-position
    x = sheet->leftColumn( int( m_pCanvas->d->view->doc()->unzoomItX( _ev->rect().x() ) + m_pCanvas->xOffset() ), xPos );
    //Align to the offset
    xPos = xPos - m_pCanvas->xOffset();
  }

  int height = m_pCanvas->d->view->doc()->zoomItY( Format::globalRowHeight() + 2 );

  QFont normalFont = painter.font();
  if ( m_pCanvas->d->view->doc()->zoom() < 100 )
  {
    normalFont.setPointSizeFloat( 0.01 * m_pCanvas->d->view->doc()->zoom() *
                                  normalFont.pointSizeFloat() );
  }
  QFont boldFont = normalFont;
  boldFont.setBold( true );

  if ( sheet->layoutDirection()==Sheet::RightToLeft )
  {
    if ( x > KS_colMax )
      x = KS_colMax;

    xPos -= sheet->columnFormat( x )->dblWidth();

    //Loop through the columns, until we are out of range
    while ( xPos <= m_pCanvas->d->view->doc()->unzoomItX( _ev->rect().right() ) )
    {
      bool selected = (m_pView->selectionInfo()->isColumnSelected(x));
      bool highlighted = (!selected && m_pView->selectionInfo()->isColumnAffected(x));

      const ColumnFormat * col_lay = sheet->columnFormat( x );
      int zoomedXPos = m_pCanvas->d->view->doc()->zoomItX( xPos );
      int width = m_pCanvas->d->view->doc()->zoomItX( xPos + col_lay->dblWidth() ) - zoomedXPos;

      if ( selected )
      {
        QBrush fillSelected( highlightColor );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, highlightColor.dark(150),
           1, &fillSelected );
      }
      else if ( highlighted )
      {
        QBrush fillHighlighted( highlightColor );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, highlightColor.dark(150),
           1, &fillHighlighted );
      }
      else
      {
        QColor c = QColorGroup(palette()).background();
        QBrush fill( c );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, c.dark(150),
           1, &fill );
      }

      // Reset painter
      painter.setFont( normalFont );
      painter.setPen( QColorGroup(palette()).text() );

      if ( selected )
        painter.setPen( QColorGroup(palette()).highlightedText() );
      else if ( highlighted )
        painter.setFont( boldFont );
      if ( !m_pView->activeSheet()->getShowColumnNumber() )
      {
        QString colText = Cell::columnName( x );
        int len = painter.fontMetrics().width( colText );
        if ( !col_lay->isHide() )
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2, colText );
      }
      else
      {
        QString tmp;
        int len = painter.fontMetrics().width( tmp.setNum(x) );
        if (!col_lay->isHide())
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2,
                            tmp.setNum(x) );
      }
      xPos += col_lay->dblWidth();
      --x;
    }
  }
  else
  {
    //Loop through the columns, until we are out of range
    while ( xPos <= m_pCanvas->d->view->doc()->unzoomItX( _ev->rect().right() ) )
    {
      bool selected = (m_pView->selectionInfo()->isColumnSelected(x));
      bool highlighted = (!selected && m_pView->selectionInfo()->isColumnAffected(x));

      const ColumnFormat *col_lay = sheet->columnFormat( x );
      int zoomedXPos = m_pCanvas->d->view->doc()->zoomItX( xPos );
      int width = m_pCanvas->d->view->doc()->zoomItX( xPos + col_lay->dblWidth() ) - zoomedXPos;

      if ( selected )
      {
        QBrush fillSelected( highlightColor );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, highlightColor.dark(),
           1, &fillSelected );
      }
      else if ( highlighted )
      {
        QBrush fillHighlighted( highlightColor );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, highlightColor.dark(),
           1, &fillHighlighted );
      }
      else
      {
        QColor c = QColorGroup(palette()).background();
        QBrush fill( c );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, c.dark(150),
           1, &fill );
      }

      // Reset painter
      painter.setFont( normalFont );
      painter.setPen( QColorGroup(palette()).text() );

      if ( selected )
        painter.setPen( QColorGroup(palette()).highlightedText() );
      else if ( highlighted )
        painter.setFont( boldFont );
      if ( !m_pView->activeSheet()->getShowColumnNumber() )
      {
        QString colText = Cell::columnName( x );
        int len = painter.fontMetrics().width( colText );
        if (!col_lay->isHide())
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2, colText );
      }
      else
      {
        QString tmp;
        int len = painter.fontMetrics().width( tmp.setNum(x) );
        if (!col_lay->isHide())
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2,
                            tmp.setNum(x) );
      }
      xPos += col_lay->dblWidth();
      ++x;
    }
  }
}


void HBorder::focusOutEvent( QFocusEvent* )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();
    m_bMousePressed = false;
}


// find the label for the tip
// this is a hack of course, because it's not available from QToolTip
QLabel *tip_findLabel()
{
    QWidgetList widgets = QApplication::allWidgets();
    foreach ( QWidget* widget, widgets )
    {
        if ( widget->isA("QTipLabel") )
            return static_cast<QLabel*>(widget);
    }
    return 0;
}

void Canvas::showToolTip( const QPoint& p )
{
    Sheet *sheet = activeSheet();
    if ( !sheet )
        return;

    // Over which cell is the mouse ?
    double ypos, xpos;
    double dwidth = doc()->unzoomItX( width() );
    int col;
    if ( sheet->layoutDirection()==Sheet::RightToLeft )
      col = sheet->leftColumn( (dwidth - doc()->unzoomItX( p.x() ) +
                                              xOffset()), xpos );
    else
      col = sheet->leftColumn( (doc()->unzoomItX( p.x() ) +
                                     xOffset()), xpos );


    int row = sheet->topRow( (doc()->unzoomItY( p.y() ) +
                                   yOffset()), ypos );

    const Cell* cell = sheet->visibleCellAt( col, row );
    if ( !cell )
        return;

#if 0
    // Quick cut
    if( cell->strOutText().isEmpty() )
        return;
#endif
    // displayed tool tip, which has the following priorities:
    //  - cell content if the cell dimension is too small
    //  - cell comment
    //  - hyperlink
    QString tipText;
    QString comment = cell->format()->comment( col, row );

    // If cell is too small, show the content
    if ( cell->testFlag( Cell::Flag_CellTooShortX ) ||
         cell->testFlag( Cell::Flag_CellTooShortY ) )
    {
        tipText = cell->strOutText();
    }

    // Show hyperlink, if any
    if ( tipText.isEmpty() )
    {
      tipText = cell->link();
    }

    // Nothing to display, bail out
    if ( tipText.isEmpty() && comment.isEmpty() )
      return;

    // Cut if the tip is ridiculously long
    const int maxLen = 256;
    if ( tipText.length() > maxLen )
        tipText = tipText.left(maxLen).append("...");

    // Determine position and width of the current cell.
    double u = cell->dblWidth( col );
    double v = cell->dblHeight( row );

    // Special treatment for obscured cells.
    if ( cell->isObscured() && cell->isPartOfMerged() )
    {
      cell = cell->obscuringCells().first();
      const int moveX = cell->column();
      const int moveY = cell->row();

      // Use the obscuring cells dimensions
      u = cell->dblWidth( moveX );
      v = cell->dblHeight( moveY );
      xpos = sheet->dblColumnPos( moveX );
      ypos = sheet->dblRowPos( moveY );
    }

    // Get the cell dimensions
    QRect marker;
    bool insideMarker = false;

    if ( sheet->layoutDirection()==Sheet::RightToLeft )
    {
      KoRect unzoomedMarker( dwidth - u - xpos + xOffset(),
                             ypos - yOffset(),
                             u,
                             v );

      marker = doc()->zoomRect( unzoomedMarker );
      insideMarker = marker.contains( p );
    }
    else
    {
      KoRect unzoomedMarker( xpos - xOffset(),
                             ypos - yOffset(),
                             u,
                             v );

      marker = doc()->zoomRect( unzoomedMarker );
      insideMarker = marker.contains( p );
    }

    // No use if mouse is somewhere else
    if ( !insideMarker )
        return;

    // Find the tipLabel
    // NOTE: if we failed, check again when the tip is shown already
    QLabel* tipLabel = tip_findLabel();

    // Ensure that it is plain text
    // Not funny if (intentional or not) <a> appears as hyperlink
    if ( tipLabel )
         tipLabel->setTextFormat( Qt::PlainText );

    QFontMetrics fm = tipLabel? tipLabel->fontMetrics() : fontMetrics();
    const QRect r( 0, 0, 200, -1 );
    // Wrap the text if too long
    if ( tipText.length() > 16 )
    {
        KWordWrap* wrap = KWordWrap::formatText( fm, r, 0, tipText );
        tipText = wrap->wrappedString();
        delete wrap;
    }
    // Wrap the comment if too long
    if ( comment.length() > 16 )
    {
      KWordWrap* wrap = KWordWrap::formatText( fm, r, 0, comment );
      comment = wrap->wrappedString();
      delete wrap;
    }

    // Show comment, if any
    if ( tipText.isEmpty() )
    {
      tipText = comment;
    }
    else if ( !comment.isEmpty() )
    {
      //Add 2 extra lines and a text, when both should be in the tooltip
      if ( !comment.isEmpty() )
        comment = "\n\n" + i18n("Comment:") + "\n" + comment;

      tipText += comment;
    }

    // Now we shows the tip
    QToolTip::showText( marker.bottomRight(), tipText, this );

    // Here we try to find the tip label again
    // Reason: the previous tip_findLabel might fail if no tip has ever shown yet
    if ( !tipLabel )
    {
      tipLabel = tip_findLabel();
      if ( tipLabel )
        tipLabel->setTextFormat( Qt::PlainText );
    }

}

#include "kspread_canvas.moc"

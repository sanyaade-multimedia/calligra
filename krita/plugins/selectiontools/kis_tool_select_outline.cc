/*
 *  kis_tool_select_freehand.h - part of Krayon^WKrita
 *
 *  Copyright (c) 2000 John Califf <jcaliff@compuzone.net>
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2004 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <qapplication.h>
#include <qpainter.h>
#include <qregion.h>
#include <qwidget.h>

#include <kaction.h>
#include <kdebug.h>
#include <kcommand.h>
#include <klocale.h>

#include <kis_layer.h>
#include <kis_selection_options.h>
#include <kis_canvas_controller.h>
#include <kis_canvas_subject.h>
#include <kis_cursor.h>
#include <kis_image.h>
#include <kis_tool_select_outline.h>
#include <kis_vec.h>
#include <kis_undo_adapter.h>
#include <kis_button_press_event.h>
#include <kis_button_release_event.h>
#include <kis_move_event.h>
#include "kis_selected_transaction.h"
#include "kis_painter.h"
#include "kis_paintop_registry.h"

KisToolSelectOutline::KisToolSelectOutline()
    : super()
{
    setName("tool_select_outline");
    setCursor(KisCursor::selectCursor());

    m_subject = 0;
    m_dragging = false;
    m_optWidget = 0;
    m_selectAction = SELECTION_ADD;
}

KisToolSelectOutline::~KisToolSelectOutline()
{
}

void KisToolSelectOutline::activate()
{
    super::activate();

    if (!m_optWidget)
        return;

    m_optWidget -> slotActivated();
}

void KisToolSelectOutline::update (KisCanvasSubject *subject)
{
    m_subject = subject;
    super::update(m_subject);
}

void KisToolSelectOutline::buttonPress(KisButtonPressEvent *event)
{
    if (event -> button() == LeftButton) {
        m_dragging = true;

        m_dragStart = event -> pos();
        m_dragEnd = event -> pos();
        m_points.clear();
        m_points.append(m_dragStart);
    }
}

void KisToolSelectOutline::move(KisMoveEvent *event)
{
    if (m_dragging) {
        m_dragStart = m_dragEnd;
        m_dragEnd = event -> pos();
        m_points.append (m_dragEnd);
        // draw new lines on canvas
        draw();
    }
}

void KisToolSelectOutline::buttonRelease(KisButtonReleaseEvent *event)
{
    if (!m_subject)
        return;

    if (m_dragging && event -> button() == LeftButton) {
        m_dragging = false;
        clear();

        KisImageSP img = m_subject -> currentImg();

        if (img) {
            QApplication::setOverrideCursor(KisCursor::waitCursor());
            KisLayerSP layer = img -> activeLayer();
            bool hasSelection = layer -> hasSelection();

            //XXX: Fix string
            KisSelectedTransaction *t = new KisSelectedTransaction(i18n("Outline Selection"), layer.data());
            KisSelectionSP selection = layer -> selection();

            if (!hasSelection) {
                selection -> clear();
            }

            KisPainter painter(selection.data());

            painter.setPaintColor(KisColor(Qt::black, selection->colorSpace()));
            painter.setFillStyle(KisPainter::FillStyleForegroundColor);
            painter.setStrokeStyle(KisPainter::StrokeStyleNone);
            painter.setBrush(m_subject -> currentBrush());
            painter.setOpacity(OPACITY_OPAQUE);
            KisPaintOp * op = KisPaintOpRegistry::instance() -> paintOp("paintbrush", &painter);
            painter.setPaintOp(op);    // And now the painter owns the op and will destroy it.

            switch (m_selectAction) {
            case SELECTION_ADD:
                painter.setCompositeOp(COMPOSITE_OVER);
                break;
            case SELECTION_SUBTRACT:
                painter.setCompositeOp(COMPOSITE_SUBTRACT);
                break;
            default:
                break;
            }

            painter.paintPolygon(m_points);


            if(hasSelection)
                layer->emitSelectionChanged(painter.dirtyRect());
            else
                layer->emitSelectionChanged();

            if (img -> undoAdapter())
                img -> undoAdapter() -> addCommand(t);

            QApplication::restoreOverrideCursor();
        }

        m_points.clear();
    }
}

void KisToolSelectOutline::paint(QPainter& gc)
{
    draw(gc);
}

void KisToolSelectOutline::paint(QPainter& gc, const QRect&)
{
    draw(gc);
}

void KisToolSelectOutline::draw()
{
    if (m_subject) {
        KisCanvasController *controller = m_subject -> canvasController();
        QWidget *canvas = controller -> canvas();
        QPainter gc(canvas);

        draw(gc);
    }
}

void KisToolSelectOutline::draw(QPainter& gc)
{
    if (!m_subject)
        return;

    if (m_dragging && !m_points.empty()) {
        QPen pen(Qt::white, 0, Qt::DotLine);

        gc.setPen(pen);
        gc.setRasterOp(Qt::XorROP);

        KisCanvasController *controller = m_subject -> canvasController();
        KisPoint start, end;
        QPoint startPos;
        QPoint endPos;

        startPos = controller -> windowToView(m_dragStart.floorQPoint());
        endPos = controller -> windowToView(m_dragEnd.floorQPoint());
        gc.drawLine(startPos, endPos);
    }
}

void KisToolSelectOutline::clear()
{
    if (m_subject) {
        KisCanvasController *controller = m_subject -> canvasController();
        QWidget *canvas = controller -> canvas();
        QPainter gc(canvas);

        QPen pen(Qt::white, 0, Qt::DotLine);

        gc.setPen(pen);
        gc.setRasterOp(Qt::XorROP);

        KisPoint start, end;
        QPoint startPos;
        QPoint endPos;

        for (KisPointVector::iterator it = m_points.begin(); it != m_points.end(); ++it) {

            if (it == m_points.begin())
            {
                start = (*it);
            } else {
                end = (*it);

                startPos = controller -> windowToView(start.floorQPoint());
                endPos = controller -> windowToView(end.floorQPoint());

                gc.drawLine(startPos, endPos);

                start = end;
            }
        }
    }
}

void KisToolSelectOutline::setup(KActionCollection *collection)
{
    m_action = static_cast<KRadioAction *>(collection -> action(name()));

    if (m_action == 0) {
        m_action = new KRadioAction(i18n("&Outline Select"),
                        "tool_outline_selection",
                        0,
                        this,
                        SLOT(activate()),
                        collection,
                        name());
        Q_CHECK_PTR(m_action);
        m_action -> setExclusiveGroup("tools");
        m_action -> setToolTip(i18n("Select an outline"));
        m_ownAction = true;
    }
}


QWidget* KisToolSelectOutline::createOptionWidget(QWidget* parent)
{
    m_optWidget = new KisSelectionOptions(parent, m_subject);
    Q_CHECK_PTR(m_optWidget);
    m_optWidget -> setCaption(i18n("Outline Selection"));

    connect (m_optWidget, SIGNAL(actionChanged(int)), this, SLOT(slotSetAction(int)));

    return m_optWidget;
}

QWidget* KisToolSelectOutline::optionWidget()
{
        return m_optWidget;
}

void KisToolSelectOutline::slotSetAction(int action) {
    if (action >= SELECTION_ADD && action <= SELECTION_SUBTRACT)
        m_selectAction =(enumSelectionMode)action;
}

#include "kis_tool_select_outline.moc"


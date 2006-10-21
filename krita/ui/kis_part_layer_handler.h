/*
 *  Copyright (c) 2006 Bart Coppens <kde@bartcoppens.be>
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
#ifndef KIS_PART_LAYER_HANDLER_
#define KIS_PART_LAYER_HANDLER_

#include <QObject>

//Added by qt3to4:
#include <QKeyEvent>

#include <KoQueryTrader.h> // KoDocumentEntry

#include "kis_types.h"

#include "kis_doc2.h"
#include "kis_view2.h"

class QKeyEvent;
class KoPointerEvent;

class KisPartLayerHandler : public QObject {
Q_OBJECT
public:
    KisPartLayerHandler(KisView2* view, const KoDocumentEntry& entry,
                        KisGroupLayerSP parent, KisLayerSP above);
signals:
    void sigGotMoveEvent(KoPointerEvent* event);
    void sigGotKeyPressEvent(QKeyEvent* event);
    void handlerDone();

protected slots:

    void gotMoveEvent(KoPointerEvent* event);
    void gotButtonPressEvent(KoPointerEvent* event);
    void gotButtonReleaseEvent(KoPointerEvent* event);
    void gotKeyPressEvent(QKeyEvent* event);
protected:
    void done();
    KisGroupLayerSP m_parent;
    KisLayerSP m_above;
    KisView2* m_view;
    KoDocumentEntry m_entry;
    QPoint m_start;
    QPoint m_end;
    bool m_started;
};

#endif // KIS_PART_LAYER_HANDLER

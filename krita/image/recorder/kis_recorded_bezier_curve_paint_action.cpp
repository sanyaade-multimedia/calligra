/*
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
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

#include "recorder/kis_recorded_bezier_curve_paint_action.h"
#include <QDomDocument>
#include <QDomElement>

#include <KoColor.h>
#include <KoColorModelStandardIds.h>
#include <KoCompositeOp.h>
#include <KoColorSpace.h>
#include "kis_node.h"
#include "kis_mask_generator.h"
#include "kis_painter.h"
#include "kis_paint_information.h"
#include "kis_paintop_registry.h"
#include "recorder/kis_recorded_action_factory_registry.h"
#include "kis_resource_server_provider.h"
#include "kis_transaction.h"
#include "kis_undo_adapter.h"
#include "kis_paintop_settings.h"
#include "kis_paintop_preset.h"
#include "kis_paint_device.h"
#include "kis_image.h"
#include "kis_layer.h"
#include "kis_node_query_path.h"

struct KisRecordedBezierCurvePaintAction::Private {
    struct BezierCurveSlice {
        KisPaintInformation point1;
        QPointF control1;
        QPointF control2;
        KisPaintInformation point2;
    };
    QList<BezierCurveSlice> infos;
};

KisRecordedBezierCurvePaintAction::KisRecordedBezierCurvePaintAction(
    const KisNodeQueryPath& path,
    const KisPaintOpPresetSP preset)
        : KisRecordedPaintAction("BezierCurvePaintAction", i18n("Bezier curve"), path, preset)
        , d(new Private)
{
}

KisRecordedBezierCurvePaintAction::KisRecordedBezierCurvePaintAction(const KisRecordedBezierCurvePaintAction& rhs) : KisRecordedPaintAction(rhs), d(new Private(*rhs.d))
{

}

KisRecordedBezierCurvePaintAction::~KisRecordedBezierCurvePaintAction()
{
    delete d;
}

void KisRecordedBezierCurvePaintAction::addPoint(const KisPaintInformation& point1,
        const QPointF& control1,
        const QPointF& control2,
        const KisPaintInformation& point2)
{
    Private::BezierCurveSlice slice;
    slice.point1 = point1;
    slice.control1 = control1;
    slice.control2 = control2;
    slice.point2 = point2;
    d->infos.append(slice);
}

void KisRecordedBezierCurvePaintAction::playPaint(const KisPlayInfo&, KisPainter* painter) const
{
    dbgImage << "play bezier curve paint with " << d->infos.size() << " points";
    if (d->infos.size() <= 0) return;
    double savedDist = 0.0;
    painter->paintAt(d->infos[0].point1);
    for (int i = 0; i < d->infos.size(); i++) {
        dbgImage << d->infos[i].point1.pos() << " to " << d->infos[i].point2.pos();
        savedDist = painter->paintBezierCurve(d->infos[i].point1, d->infos[i].control1, d->infos[i].control2, d->infos[i].point2, savedDist);
    }
}

void KisRecordedBezierCurvePaintAction::toXML(QDomDocument& doc, QDomElement& elt) const
{
    KisRecordedPaintAction::toXML(doc, elt);
    QDomElement waypointsElt = doc.createElement("Waypoints");
    foreach(const Private::BezierCurveSlice & info, d->infos) {
        QDomElement infoElt = doc.createElement("Waypoint");
        // Point1
        QDomElement point1Elt = doc.createElement("Point1");
        info.point1.toXML(doc, point1Elt);
        infoElt.appendChild(point1Elt);
        // Control1
        QDomElement control1Elt = doc.createElement("Control1");
        control1Elt.setAttribute("x", info.control1.x());
        control1Elt.setAttribute("y", info.control1.y());
        infoElt.appendChild(control1Elt);
        // Control2
        QDomElement control2Elt = doc.createElement("Control2");
        control2Elt.setAttribute("x", info.control2.x());
        control2Elt.setAttribute("y", info.control2.y());
        infoElt.appendChild(control2Elt);
        // Point2
        QDomElement point2Elt = doc.createElement("Point2");
        info.point2.toXML(doc, point2Elt);
        infoElt.appendChild(point2Elt);

        waypointsElt.appendChild(infoElt);
    }
    elt.appendChild(waypointsElt);
}

KisRecordedAction* KisRecordedBezierCurvePaintAction::clone() const
{
    return new KisRecordedBezierCurvePaintAction(*this);
}


KisRecordedBezierCurvePaintActionFactory::KisRecordedBezierCurvePaintActionFactory() :
        KisRecordedPaintActionFactory("BezierCurvePaintAction")
{
}

KisRecordedBezierCurvePaintActionFactory::~KisRecordedBezierCurvePaintActionFactory()
{

}

KisRecordedAction* KisRecordedBezierCurvePaintActionFactory::fromXML(const QDomElement& elt)
{
    KisNodeQueryPath pathnode = nodeQueryPathFromXML(elt);

    // Decode pressets
    KisPaintOpPresetSP paintOpPreset = paintOpPresetFromXML(elt);

    KisRecordedBezierCurvePaintAction* rplpa = new KisRecordedBezierCurvePaintAction(pathnode, paintOpPreset);

    setupPaintAction(rplpa, elt);

    QDomElement wpElt = elt.firstChildElement("Waypoints");
    if (!wpElt.isNull()) {
        QDomNode nWp = wpElt.firstChild();
        while (!nWp.isNull()) {
            QDomElement eWp = nWp.toElement();
            if (!eWp.isNull() && eWp.tagName() == "Waypoint") {
                QDomElement control1Elt = eWp.firstChildElement("Control1");
                QDomElement control2Elt = eWp.firstChildElement("Control2");
                rplpa->addPoint(KisPaintInformation::fromXML(eWp.firstChildElement("Point1")),
                                QPointF(control1Elt.attribute("x", "0.0").toDouble(),
                                        control1Elt.attribute("y", "0.0").toDouble()),
                                QPointF(control2Elt.attribute("x", "0.0").toDouble(),
                                        control2Elt.attribute("y", "0.0").toDouble()),
                                KisPaintInformation::fromXML(eWp.firstChildElement("Point2")));
            }
            nWp = nWp.nextSibling();
        }
    } else {
        dbgImage << "Warning: no <Waypoints /> found";
    }
    return rplpa;
}



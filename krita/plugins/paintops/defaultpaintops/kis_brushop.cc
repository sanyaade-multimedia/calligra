/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2004-2008 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2004 Clarence Dang <dang@kde.org>
 *  Copyright (c) 2004 Adrian Page <adrian@pagenet.plus.com>
 *  Copyright (c) 2004 Cyrille Berger <cberger@cberger.net>
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

#include "kis_brushop.h"

#include <string.h>

#include <QRect>
#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDomElement>
#include <QHBoxLayout>
#include <qtoolbutton.h>

#include <kis_image.h>
#include <kis_debug.h>

#include <KoColorTransformation.h>
#include <KoColor.h>
#include <KoInputDevice.h>

#include <widgets/kcurve.h>
#include <kis_brush.h>
#include <kis_datamanager.h>
#include <kis_global.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_paintop.h>
#include <kis_properties_configuration.h>
#include <kis_selection.h>
#include <kis_brush_option.h>
#include <kis_pressure_darken_option.h>
#include <kis_pressure_opacity_option.h>
#include <kis_pressure_size_option.h>
#include <kis_paint_action_type_option.h>

KisPaintOp * KisBrushOpFactory::createOp(const KisPaintOpSettingsSP settings,
                                         KisPainter * painter, KisImageSP image)
{
    Q_UNUSED( image );
    kDebug() << settings;

    const KisBrushOpSettings *brushopSettings = dynamic_cast<const KisBrushOpSettings *>(settings.data());
    Q_ASSERT(settings != 0 || brushopSettings != 0);

    KisPaintOp * op = new KisBrushOp(brushopSettings, painter);
    Q_CHECK_PTR(op);
    return op;
}

KisPaintOpSettingsSP KisBrushOpFactory::settings(QWidget * parent, const KoInputDevice& inputDevice, KisImageSP /*image*/)
{
    return new KisBrushOpSettings(parent);
}

KisPaintOpSettingsSP KisBrushOpFactory::settings(KisImageSP image)
{
    Q_UNUSED(image);
    return new KisBrushOpSettings(0);
}



KisBrushOpSettings::KisBrushOpSettings(QWidget *parent)
    : KisPaintOpSettings()
{
    kDebug() << "creating settings " << this;
    m_optionsWidget = new KisPaintOpOptionsWidget(parent);
    m_optionsWidget->setObjectName("brush option widget");

    m_brushOption = new KisBrushOption();
    m_sizeOption = new KisPressureSizeOption();
    m_opacityOption = new KisPressureOpacityOption();
    m_darkenOption = new KisPressureDarkenOption();
    m_paintActionTypeOption = new KisPaintActionTypeOption();

    m_optionsWidget->addPaintOpOption(m_brushOption);
    m_optionsWidget->addPaintOpOption(m_sizeOption);
    m_optionsWidget->addPaintOpOption(m_opacityOption);
    m_optionsWidget->addPaintOpOption(m_darkenOption);
    m_optionsWidget->addPaintOpOption(m_paintActionTypeOption);
}

void KisBrushOpSettings::fromXML(const QDomElement& elt)
{
#if 0
    QDomElement e = elt.firstChildElement("Params");
    if(!e.isNull())
    {
        KisPropertiesConfiguration kpc;
        kpc.fromXML(e);
        m_size->setChecked( kpc.getBool( "PressureSize", false) );
        m_opacity->setChecked( kpc.getBool( "PressureOpacity", false) );
        m_darken->setChecked( kpc.getBool( "PressureDarken", false) );
        m_customSize = kpc.getBool( "CustomSize", false);
        m_customOpacity = kpc.getBool( "CustomOpacity", false);
        m_customDarken = kpc.getBool( "CustomDarken", false);
        for(int i = 0; i < 256; i++)
        {
            if( m_customSize )
                m_sizeCurve[i] = kpc.getDouble( QString("SizeCurve%0").arg(i), i / 255.0 );
            if( m_customOpacity )
                m_opacityCurve[i] = kpc.getDouble( QString("OpacityCurve%0").arg(i), i / 255.0 );
            if( m_customDarken )
                m_darkenCurve[i] = kpc.getDouble( QString("DarkenCurve%0").arg(i), i / 255.0 );
        }
    }
#endif
}

void KisBrushOpSettings::toXML(QDomDocument& doc, QDomElement& rootElt) const
{
#if 0
    KisPropertiesConfiguration kpc;
    kpc.setProperty("PressureSize", m_size->isChecked());
    kpc.setProperty("PressureOpacity", m_opacity->isChecked());
    kpc.setProperty("PressureDarken", m_darken->isChecked());
    kpc.setProperty("CustomSize", m_customSize);
    kpc.setProperty("CustomOpacity", m_customOpacity);
    kpc.setProperty("CustomDarken", m_customDarken);

    for(int i = 0; i < 256; i++)
    {
        if( m_customSize )
            kpc.setProperty( QString("SizeCurve%0").arg(i), m_sizeCurve[i] );
        if( m_customOpacity )
            kpc.setProperty( QString("OpacityCurve%0").arg(i), m_opacityCurve[i] );
        if( m_customDarken )
            kpc.setProperty( QString("DarkenCurve%0").arg(i), m_darkenCurve[i] );
    }

    QDomElement paramsElt = doc.createElement( "Params" );
    rootElt.appendChild( paramsElt );
    kpc.toXML( doc, paramsElt);
#endif
}


KisPaintOpSettingsSP KisBrushOpSettings::clone() const
{

    KisBrushOpSettings* s = new KisBrushOpSettings(0);
#if 0
    s->m_size->setChecked( m_size->isChecked() );
    s->m_opacity->setChecked( m_opacity->isChecked() );
    s->m_darken->setChecked( m_darken->isChecked() );
    s->m_customSize = m_customSize;
    s->m_customOpacity = m_customOpacity;
    s->m_customDarken = m_customDarken;
    memcpy(s->m_sizeCurve, m_sizeCurve, 256*sizeof(double));
    memcpy(s->m_opacityCurve, m_opacityCurve, 256*sizeof(double));
    memcpy(s->m_darkenCurve, m_darkenCurve, 256*sizeof(double));

#endif
    return s;

}


KisBrushOp::KisBrushOp(const KisBrushOpSettings *settings, KisPainter *painter)
    : KisBrushBasedPaintOp(painter)
    , m_pressureSize(true)
    , m_pressureOpacity(false)
    , m_pressureDarken(false)
    , m_customSize(false)
    , m_customOpacity(false)
    , m_customDarken(false)
{
#if 0
    if (settings != 0) {
        m_pressureSize = settings->varySize();
        m_pressureOpacity = settings->varyOpacity();
        m_pressureDarken = settings->varyDarken();
        m_customSize = settings->customSize();
        m_customOpacity = settings->customOpacity();
        m_customDarken = settings->customDarken();
        if (m_customSize) {
            memcpy(m_sizeCurve, settings->sizeCurve(), 256 * sizeof(double));
        }
        if (m_customOpacity) {
            memcpy(m_opacityCurve, settings->opacityCurve(), 256 * sizeof(double));
        }
        if (m_customDarken) {
            memcpy(m_darkenCurve, settings->darkenCurve(), 256 * sizeof(double));
        }
    }
#endif
}

KisBrushOp::~KisBrushOp()
{
}

void KisBrushOp::paintAt(const KisPaintInformation& info)
{
    KisPaintInformation adjustedInfo(info);
    if (!m_pressureSize)
        adjustedInfo.setPressure( PRESSURE_DEFAULT );
    else if (m_customSize)
        adjustedInfo.setPressure(scaleToCurve(adjustedInfo.pressure(), m_sizeCurve));


    // Painting should be implemented according to the following algorithm:
    // retrieve brush
    // if brush == mask
    //          retrieve mask
    // else if brush == image
    //          retrieve image
    // subsample (mask | image) for position -- pos should be double!
    // apply filters to mask (color | gradient | pattern | etc.
    // composite filtered mask into temporary layer
    // composite temporary layer into target layer
    // @see: doc/brush.txt

    if (!painter()->device()) return;

    KisBrush *brush = m_brush;

    Q_ASSERT(brush);
    if (!brush) return;
    if (! brush->canPaintFor(adjustedInfo) )
        return;

    KisPaintDeviceSP device = painter()->device();
    double pScale = KisPaintOp::scaleForPressure( adjustedInfo.pressure() ); // TODO: why is there scale and pScale that seems to contains the same things ?
    QPointF hotSpot = brush->hotSpot( pScale, pScale );
    QPointF pt = info.pos() - hotSpot;

    // Split the coordinates into integer plus fractional parts. The integer
    // is where the dab will be positioned and the fractional part determines
    // the sub-pixel positioning.
    qint32 x;
    double xFraction;
    qint32 y;
    double yFraction;

    splitCoordinate(pt.x(), &x, &xFraction);
    splitCoordinate(pt.y(), &y, &yFraction);

    KisPaintDeviceSP dab = KisPaintDeviceSP(0);

    quint8 origOpacity = painter()->opacity();
    KoColor origColor = painter()->paintColor();

    if (m_pressureOpacity) {
        if (!m_customOpacity)
            painter()->setOpacity((qint8)(origOpacity * info.pressure()));
        else
            painter()->setOpacity((qint8)(origOpacity * scaleToCurve(info.pressure(), m_opacityCurve)));
    }

    if (m_pressureDarken) {
        KoColor darkened = origColor;
        // Darken docs aren't really clear about what exactly the amount param can have as value...
        quint32 darkenAmount;
        if (!m_customDarken)
            darkenAmount = (qint32)(255  - 75 * info.pressure());
        else
            darkenAmount = (qint32)(255  - 75 * scaleToCurve(info.pressure(), m_darkenCurve));

        KoColorTransformation* transfo = darkened.colorSpace()->createDarkenAdjustment(darkenAmount, false, 0.0);
        transfo->transform(origColor.data(), darkened.data(), 1);
        painter()->setPaintColor(darkened);
        delete transfo;
    }

    double scale = KisPaintOp::scaleForPressure( adjustedInfo.pressure() );

    QRect dabRect = QRect(0, 0, brush->maskWidth(scale, 0.0),
                          brush->maskHeight(scale, 0.0));
    QRect dstRect = QRect(x, y, dabRect.width(), dabRect.height());


    if ( painter()->bounds().isValid() ) {
        dstRect &= painter()->bounds();
    }

    if (dstRect.isNull() || dstRect.isEmpty() || !dstRect.isValid()) return;

    qint32 sx = dstRect.x() - x;
    qint32 sy = dstRect.y() - y;
    qint32 sw = dstRect.width();
    qint32 sh = dstRect.height();

    if (brush->brushType() == IMAGE || brush->brushType() == PIPE_IMAGE) {
        dab = brush->image(device->colorSpace(), scale, 0.0, adjustedInfo, xFraction, yFraction);
    }
    else {
        dab = cachedDab( );
        KoColor color = painter()->paintColor();
        color.convertTo( dab->colorSpace() );
        brush->mask(dab, color, scale, scale, 0.0, info, xFraction, yFraction);
    }

    painter()->bltSelection(dstRect.x(), dstRect.y(), painter()->compositeOp(), dab, painter()->opacity(), sx, sy, sw, sh);

    painter()->setOpacity(origOpacity);
    painter()->setPaintColor(origColor);

}

double KisBrushOp::paintLine(const KisPaintInformation &pi1,
                             const KisPaintInformation &pi2,
                             double savedDist )
{
    KisPaintInformation adjustedInfo1(pi1);
    KisPaintInformation adjustedInfo2(pi2);
/*    if ( !m_pressureSize ) {
        adjustedInfo1.setPressure( PRESSURE_DEFAULT );
        adjustedInfo2.setPressure( PRESSURE_DEFAULT );
    }*/
    return KisPaintOp::paintLine( adjustedInfo1, adjustedInfo2, savedDist );
}

#include "kis_brushop.moc"

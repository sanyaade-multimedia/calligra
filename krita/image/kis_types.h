/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
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
#ifndef KISTYPES_H_
#define KISTYPES_H_

#include <QVector>
#include <QMap>
#include <QPoint>

#include <kis_shared_ptr.h>

#include "kis_shared_ptr_vector.h"

/**
 * Define lots of shared pointer versions of Krita classes.
 * Shared pointer classes have the advantage of near automatic
 * memory management (but take care of circular references)
 * and the disadvantage that inheritiance relations are no longer
 * recognizable
 */

class KisImage;
typedef KisSharedPtr<KisImage> KisImageSP;

class KisPaintDevice;
typedef KisSharedPtr<KisPaintDevice> KisPaintDeviceSP;
typedef KisSharedPtrVector<KisPaintDevice> vKisPaintDeviceSP;
typedef vKisPaintDeviceSP::iterator vKisPaintDeviceSP_it;
typedef vKisPaintDeviceSP::const_iterator vKisPaintDeviceSP_cit;

class KisLayer;
typedef KisSharedPtr<KisLayer> KisLayerSP;
typedef KisSharedPtrVector<KisLayer> vKisLayerSP;
typedef vKisLayerSP::iterator vKisLayerSP_it;
typedef vKisLayerSP::const_iterator vKisLayerSP_cit;

class KisPartLayer;
typedef KisSharedPtr<KisPartLayer> KisPartLayerSP;

class KisPaintLayer;
typedef KisSharedPtr<KisPaintLayer> KisPaintLayerSP;

class KisAdjustmentLayer;
typedef KisSharedPtr<KisAdjustmentLayer> KisAdjustmentLayerSP;

class KisGroupLayer;
typedef KisSharedPtr<KisGroupLayer> KisGroupLayerSP;

class KisSelection;
typedef KisSharedPtr<KisSelection> KisSelectionSP;

class KisBackground;
typedef KisSharedPtr<KisBackground> KisBackgroundSP;

class KisHistogram;
typedef KisSharedPtr<KisHistogram> KisHistogramSP;

class KisPaintOpFactory;
typedef KisSharedPtr<KisPaintOpFactory> KisPaintOpFactorySP;

typedef QVector<QPoint> vKisSegments;

//class KisGuide;
//typedef KisSharedPtr<KisGuide> KisGuideSP;

class KisAlphaMask;
typedef KisSharedPtr<KisAlphaMask> KisAlphaMaskSP;

class KisFilter;
typedef KisSharedPtr<KisFilter> KisFilterSP;

#endif // KISTYPES_H_

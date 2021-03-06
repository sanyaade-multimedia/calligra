/* This file is part of the KDE project

   Copyright 2018 Dag Andersen <danders@get2net.dk>

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


#include "ConfigObjectBase.h"

using namespace KoChart;

ConfigObjectBase::ConfigObjectBase(QWidget *parent)
    : QWidget(parent)
    , chart(0)
{
    
}

ConfigObjectBase::ConfigObjectBase(QList<ChartType> types, QWidget *parent)
    : QWidget(parent)
    , chart(0)
    , chartTypes(types)
{
    
}

ConfigObjectBase::~ConfigObjectBase()
{

}

void ConfigObjectBase::setChartTypes(QList<ChartType> types)
{
    chartTypes = types;
}

void ConfigObjectBase::open(ChartShape *shape)
{
    chart = shape;
}

void ConfigObjectBase::updateData(ChartType type, ChartSubtype subtype)
{
    Q_UNUSED(type)
    Q_UNUSED(subtype)
}

void ConfigObjectBase::blockSignals(bool block)
{
    blockSignals(this, block);
}

void ConfigObjectBase::blockSignals(QWidget *w, bool block)
{
    QList<QWidget*> lst = w->findChildren<QWidget*>();
    for (int i = 0; i < lst.count(); ++i) {
        lst.at(i)->blockSignals(block);
    }
}

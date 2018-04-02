/* This file is part of the KDE project
   Copyright (C) 2006-2007 Dag Andersen <danders@get2net.dk>
   Copyright (C) 2016 Dag Andersen <danders@get2net.dk>
   
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
   Boston, MA 02110-1301, USA.
*/

#ifndef KPlato_DateTimeTester_h
#define KPlato_DateTimeTester_h

#include <QObject>

#include "kptdatetime.h"

namespace KPlato
{

class DateTimeTester : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void subtractDay();
    void subtractHour();
    void subtractMinute();
    void subtractSecond();
    void subtractMillisecond();

    void addDay();
    void addHour();
    void addMinute();
    void addSecond();
    void addMillisecond();

    void timeZones();
};

} //namespace KPlato

#endif
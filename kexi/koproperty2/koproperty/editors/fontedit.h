/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2005-2008 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_FONTEDIT_H
#define KPROPERTY_FONTEDIT_H

#include <QtCore/QVariant>
#include <KLineEdit>

#include "Factory.h"

namespace KoProperty
{

/*
class FontEdit : public KLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue USER true)
public:
    FontEdit(QWidget *parent = 0);
    ~FontEdit();
    QString value() const;
    void setValue(const QString& value);
signals:
    void commitData( QWidget * editor );
private slots:
    void slotTextChanged( const QString & text );
private:
    bool m_slotTextChangedEnabled : 1;
};*/

class KOPROPERTY_EXPORT FontDelegate : public EditorCreatorInterface, 
                     public ValuePainterInterface
{
public:
    FontDelegate() {}
    virtual QWidget * createEditor( int type, QWidget *parent, 
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual void paint( QPainter * painter, 
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

}

#endif

/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jarosław Staniek <staniek@kde.org>

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

#include "kexisectionheader.h"
#include <kexiutils/utils.h>
#include <kexiutils/SmallToolButton.h>
#include <KexiView.h>

#include <kdebug.h>

#include <QLabel>
#include <QBoxLayout>
#include <QEvent>
#include <QApplication>

//! @internal
class KexiSectionHeader::Private
{
public:
    Private() {
    }

    Qt::Orientation orientation;
    QLabel *lbl;
    QBoxLayout *lyr;
    QWidget *lbl_b;
    QHBoxLayout *lbl_bLyr;
};

//==========================

KexiSectionHeader::KexiSectionHeader(const QString &caption,
                                     Qt::Orientation o, QWidget* parent)
        : QWidget(parent)
        , d(new Private())
{
    d->orientation = o;
    d->lyr = new QBoxLayout(
        d->orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight,
        this);
    d->lyr->setContentsMargins(0, 0, 0, 0);
    d->lyr->setSpacing(0);

    d->lbl_b = new QWidget(this);
    d->lbl_bLyr = new QHBoxLayout(lbl_b);
    lbl_bLyr->setMargin(0);
    d->lyr->addWidget(d->lbl_b);
    d->lbl = new QLabel(caption, d->lbl_b);
    d->lbl_bLyr->addWidget(lbl);
    d->lbl->setContentsMargins(6, 0, 0, 0);
    d->lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    d->lbl->setAutoFillBackground(true);
    d->lbl->installEventFilter(this);
    installEventFilter(this);
    setWindowTitle(caption);
}

KexiSectionHeader::~KexiSectionHeader()
{
    delete d;
}

void KexiSectionHeader::setWidget(QWidget * widget)
{
    QLayoutItem *item = d->lyr->itemAt(1); //for sanity
    if (!widget || (item && widget == item->widget()))
        return;
    if (item)
        d->lyr->removeItem(item);
//! @todo disconnect
    d->lyr->addWidget(widget);
    widget->installEventFilter(this);
    KexiView* view = dynamic_cast<KexiView*>(widget);
    if (view) {
        connect(view, SIGNAL(focus(bool)), this, SLOT(slotFocus(bool)));
        d->lbl->setBuddy(view);
    }
}

void KexiSectionHeader::addButton(const KIcon& icon, const QString& toolTip,
                                  const QObject * receiver, const char * member)
{
    KexiSmallToolButton *btn = new KexiSmallToolButton(icon, QString(), d->lbl_b);
    d->lbl_bLyr->addWidget(btn);
    if (receiver && member) {
        connect(btn, SIGNAL(clicked()), receiver, member);
    }
    if (!toolTip.isEmpty())
        btn->setToolTip(toolTip);
}

bool KexiSectionHeader::eventFilter(QObject *o, QEvent *e)
{
    if (o == d->lbl && e->type() == QEvent::MouseButtonRelease) {
        QLayoutItem *item = d->lyr->itemAt(1);
        if (item && item->widget())
            item->widget()->setFocus();
    }
    return QWidget::eventFilter(o, e);
}

void KexiSectionHeader::slotFocus(bool in)
{
    kDebug() << in;
    in = in || qApp->focusWidget() == this;
    QPalette pal(d->lbl->palette());
    pal.setBrush(QPalette::Window,
                 palette().brush(in ? QPalette::Highlight : d->lbl->backgroundRole()));
    pal.setBrush(QPalette::WindowText,
                 palette().brush(in ? QPalette::HighlightedText : d->lbl->foregroundRole()));
    d->lbl->setPalette(pal);
}

QSize KexiSectionHeader::sizeHint() const
{
    QLayoutItem *item = d->lyr->itemAt(1);
    if (!item || !item->widget())
        return QWidget::sizeHint();
    QSize s(item->widget()->sizeHint());
    return QSize(s.width(), d->lbl->sizeHint().height() + s.height());
}

void KexiSectionHeader::setCaption(const QString& caption)
{
    d->lbl->setText(caption);
}

QString KexiSectionHeader::caption() const
{
    return d->lbl->text();
}


/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWSHAPECONFIGFACTORY_H
#define KWSHAPECONFIGFACTORY_H

#include <KoShapeConfigFactoryBase.h>

#include <QObject>

class KoShape;
class KWFrame;
class KWDocument;

/// \internal
class FrameConfigSharedState : public QObject
{
    Q_OBJECT
public:
    explicit FrameConfigSharedState(KWDocument *document);
    ~FrameConfigSharedState();

    void addUser();
    void removeUser();

    KWDocument *document() const {
        return m_document;
    }

    bool keepAspectRatio() const {
        return m_protectAspectRatio;
    }
    void setKeepAspectRatio(bool on);

Q_SIGNALS:
    void keepAspectRatioChanged(bool keep);

private:
    int m_refcount;
    bool m_protectAspectRatio; // states if the user has this boolean set right now.
    KWDocument *m_document;
};

/// factory to create a KWFrameConnectSelector widget
class KWFrameConnectSelectorFactory : public KoShapeConfigFactoryBase
{
public:
    /// constructor
    explicit KWFrameConnectSelectorFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWFrameConnectSelectorFactory() {}

    /// reimplemented method from superclass
    KoShapeConfigWidgetBase *createConfigWidget(KoShape *shape);
    /// reimplemented method from superclass
    QString name() const;

    /// reimplemented method from superclass
    bool showForShapeId(const QString &id) const;
    /// reimplemented method from superclass
    int sortingOrder() const {
        return 15;
    }

private:
    FrameConfigSharedState *m_state;
};

/// factory to create a KWAnchoringProperties widget
class KWAnchoringFactory : public KoShapeConfigFactoryBase
{
public:
    /// constructor
    explicit KWAnchoringFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWAnchoringFactory() {}

    /// reimplemented method from superclass
    KoShapeConfigWidgetBase *createConfigWidget(KoShape *shape);
    /// reimplemented method from superclass
    QString name() const;
    /// reimplemented method from superclass
    int sortingOrder() const {
        return 0;
    }
    /// reimplemented method from superclass
    bool showForShapeId(const QString &id) const;

private:
    FrameConfigSharedState *m_state;
};

/// factory to create a KWFrameRunaroundProperties widget
class KWRunAroundPropertiesFactory : public KoShapeConfigFactoryBase
{
public:
    /// constructor
    explicit KWRunAroundPropertiesFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWRunAroundPropertiesFactory() {}

    /// reimplemented method from superclass
    KoShapeConfigWidgetBase *createConfigWidget(KoShape *shape);
    /// reimplemented method from superclass
    QString name() const;
    /// reimplemented method from superclass
    bool showForShapeId(const QString &id) const;

    /// reimplemented method from superclass
    int sortingOrder() const {
        return 5;
    }

private:
    FrameConfigSharedState *m_state;
};


#endif

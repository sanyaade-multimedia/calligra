/*
 *  Copyright (c) 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOELEMENTREFERENCE_H
#define KOELEMENTREFERENCE_H

#include <QSharedDataPointer>
#include <QSharedData>
#include <QUuid>

class KoXmlWriter;
class KoXmlElement;

#include "koodf_export.h"

class KoElementReferenceData : public QSharedData
{
public:

    KoElementReferenceData()
    {
        xmlid = QUuid::createUuid().toString();
        xmlid.remove('{');
        xmlid.remove('}');
    }

    KoElementReferenceData(const KoElementReferenceData &other)
        : QSharedData(other)
        , xmlid(other.xmlid)
    {
    }

    ~KoElementReferenceData() {}

    QString xmlid;
};
/**
 * KoElementReference is used to store unique identifiers for elements in an odf document.
 * Element references are saved as xml:id and optionally for compatibility also as draw:id
 * and text:id.
 *
 * You can use element references wherever you would have used a QString to refer to the id
 * of an object.
 *
 * Element references are implicitly shared, so you can and should pass them along by value.
 */
class KOODF_EXPORT KoElementReference
{
public:

    enum SaveOption {
        XMLID = 0x0,
        DRAWID = 0x1,
        TEXTID = 0x2
    };
    Q_DECLARE_FLAGS(SaveOptions, SaveOption)

    KoElementReference();
    KoElementReference(const QString &xmlid);
    KoElementReference(const KoElementReference &other);
    KoElementReference &operator=(const KoElementReference &rhs);
    bool operator==(const KoElementReference &other);
    bool operator!=(const KoElementReference &other);

    /**
     * @return true if the xmlid is valid, i.e., not null
     */
    bool isValid() const;

    /**
     * @brief loadOdf creates a new KoElementReference from the given element. If the element
     *   does not have an xml:id, draw:id or text:id attribute, and invalid element reference
     *   is returned.
     * @param element the element that may contain xml:id, text:id or draw:id. xml:id has
     *    priority.
     * @return a new element reference
     */
    static KoElementReference loadOdf(const KoXmlElement &element);

    /**
     * @brief saveOdf saves this element reference into the currently open element in the xml writer.
     * @param writer the writer we save to
     * @param saveOptions determins which attributes we save.
     */
    void saveOdf(KoXmlWriter *writer, SaveOptions saveOptions) const;


private:



    QSharedDataPointer<KoElementReferenceData> d;

};

 Q_DECLARE_OPERATORS_FOR_FLAGS(KoElementReference::SaveOptions)

#endif // KOELEMENTREFERENCE_H

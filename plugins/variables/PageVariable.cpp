/* This file is part of the KDE project
 * Copyright (C) 2007 Pierre Ducroquet <pinaraf@gmail.com>
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
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

#include "PageVariable.h"

#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoProperties.h>
#include <kdebug.h>
#include <KoShape.h>
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>
#include <KoTextPage.h>

PageVariable::PageVariable()
    : KoVariable(true),
    m_type(PageCount),
    m_pageselect(0),
    m_pageadjust(0)
{
}

void PageVariable::setProperties(const KoProperties *props) {
    if (props->boolProperty("count")) {
        m_type = PageCount;
    } else {
        m_type = PageNumber;
    }
}

void PageVariable::propertyChanged(Property property, const QVariant &value) {
    switch(m_type) {
        case PageCount:
            if(property == KoInlineObject::PageCount) {
                setValue(value.toString());
            }
            break;
        case PageNumber:
            break;
    }
}

void PageVariable::variableMoved(const KoShape *shape, const QTextDocument *document, int posInDocument) {
    Q_UNUSED(document);
    Q_UNUSED(posInDocument);
    switch (m_type) {
        case PageCount:
            break;
        case PageNumber:
            if (value().isEmpty() || ! m_fixed) {
                if (KoTextShapeData *shapeData = dynamic_cast<KoTextShapeData *>(shape ? shape->userData() : 0)) {
                    KoTextPage* page = shapeData->page();
                    const int pagenumber = page->pageNumber(m_pageselect, m_pageadjust);
                    setValue(pagenumber >= 0 ? QString::number(pagenumber + 1) : QString());
                }
            }
            break;
    }
}

void PageVariable::saveOdf( KoShapeSavingContext & context )
{
    KoXmlWriter *writer = &context.xmlWriter();
    switch(m_type) {
        case PageCount:
            // <text:page-count>3</text:page-count>
            writer->startElement("text:page-count", false);
            writer->addTextNode(value());
            writer->endElement();
            break;
        case PageNumber:
            // <text:page-number text:select-page="current" text:page-adjust="2" text:fixed="true">3</text:page-number>
            writer->startElement("text:page-number", false);

            if(m_pageadjust == 0)
                writer->addAttribute("text:select-page", "current");
            else if(m_pageadjust == -1)
                writer->addAttribute("text:select-page", "previous");
            else if(m_pageadjust == 1)
                writer->addAttribute("text:select-page", "next");

            if (m_pageadjust != 0)
                writer->addAttribute("text:page-adjust", QString::number(m_pageadjust));

            if (m_fixed)
                writer->addAttribute("text:fixed", "true");

            writer->addTextNode(value());
            writer->endElement();
            break;
    }
}

bool PageVariable::loadOdf( const KoXmlElement & element, KoShapeLoadingContext & context )
{
    Q_UNUSED( context );
    const QString localName( element.localName() );
    if ( localName == "page-count" ) {
        m_type = PageCount;
    }
    else if ( localName == "page-number" ) {
        m_type = PageNumber;

        // The text:select-page attribute is used to display the number of the previous or the following
        // page rather than the number of the current page.
        QString pageselect = element.attributeNS( KoXmlNS::text, "select-page", QString() );
        if (pageselect == "previous")
            m_pageselect = -1;
        else if (pageselect == "next")
            m_pageselect = 1;
        else // "current"
            m_pageselect = 0;

        // The value of a page number field can be adjusted by a specified number, allowing the display
        // of page numbers of following or preceding pages. The adjustment amount is specified using
        // the text:page-adjust attribute.
        m_pageadjust = element.attributeNS( KoXmlNS::text, "page-adjust", QString() ).toInt();

        // The text:fixed attribute specifies whether or not the value of a field element is fixed. If
        // the value of a field is fixed, the value of the field element to which this attribute is
        // attached is preserved in all future edits of the document. If the value of the field is not
        // fixed, the value of the field may be replaced by a new value when the document is edited.
        m_fixed = element.attributeNS( KoXmlNS::text, "fixed", QString() ) == "true";
    }
    return true;
}

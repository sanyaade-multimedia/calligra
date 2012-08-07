/* This file is part of the KDE project

   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
   Copyright (C) 2012 Inge Wallin            <inge@lysator.liu.se>

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


// Own
#include "htmlconvert.h"

// Qt
#include <QStringList>
#include <QBuffer>

// KDE
#include <kdebug.h>

// Calligra
#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

// EPUB filter
#include "libepub/EpubFile.h"


static void handleStyles(KoXmlNode &stylesNode,
                         QHash<QString, StyleInfo*> &styles);
static void handleStyleAttributes(KoXmlElement &propertiesElement, QList<QString> &attList,
                                  StyleInfo *styleInfo);

static void createHtmlHead(KoXmlWriter *writer, QHash<QString, QString> &metaData);


// All handleTag*() are named after the tag in the ODF that they handle.
static void handleCharacterData(KoXmlNode &node, KoXmlWriter *bodyWriter,
                                QHash<QString, StyleInfo*> &styles);
static void handleTagTable(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                           QHash<QString, StyleInfo*> &styles);
static void handleTagFrame(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                           QHash<QString, StyleInfo*> &styles);
static void handleTagP(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles);
static void handleTagSpan(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                          QHash<QString, StyleInfo*> &styles);
static void handleTagPageBreak(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                               QHash<QString, StyleInfo*> &styles);
static void handleTagH(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles);
static void handleTagList(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles);
static void handleTagA(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles);
static void handleTagTab(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles);
static void handleTagTableOfContent(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles);
static void handleTagLineBreak(KoXmlWriter *bodyWriter);

static void handleTagTableOfContentBody(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles);


static void handleUnknownTags(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles);


StyleInfo::StyleInfo()
    : hasBreakBefore(false)
    , inUse(false)
{
}

// ================================================================
//                         Style parsing



KoFilter::ConversionStatus parseStyles(KoStore *odfStore,
                                       QHash<QString, StyleInfo*> &styles)
{
    //kDebug(30517) << "parse content.xml styles";
    if (!odfStore->open("content.xml")) {
        kError(30517) << "Unable to open input file! content.xml" << endl;
        return KoFilter::FileNotFound;
    }

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug() << "Error occurred while parsing styles.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return KoFilter::ParsingError;
    }

    // Get the node that contains the styles.
    KoXmlNode stylesNode = doc.documentElement();
    stylesNode = KoXml::namedItemNS(stylesNode, KoXmlNS::office, "automatic-styles");

    // Collect attributes in the styles.
    handleStyles(stylesNode, styles);

    odfStore->close(); // end of parsing styles in content.xml

    // ----------------------------------------------------------------

    //kDebug(30517) << "************ parse styles.xml styles **********************";
    if (!odfStore->open("styles.xml")) {
        kError(30517) << "Unable to open input file! style.xml" << endl;
        return KoFilter::FileNotFound;
    }
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug() << "Error occurred while parsing styles.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return KoFilter::ParsingError;
    }

    // Parse properties of the named styles referred by the automatic
    // styles. Only those styles that are actually used in the
    // document are converted.
    stylesNode = doc.documentElement();
    stylesNode = KoXml::namedItemNS(stylesNode, KoXmlNS::office, "styles");

    // Collect attributes in the styles.
    handleStyles(stylesNode, styles);

    odfStore->close();
    return KoFilter::OK;
}

void handleStyles(KoXmlNode &stylesNode,
                  QHash<QString, StyleInfo*> &styles)
{
    QString attribute;
    QList <QString> attList;

    KoXmlElement styleElement;
    forEachElement (styleElement, stylesNode) {
        StyleInfo *styleInfo = new StyleInfo;

        QString styleName  = styleElement.attribute("name");
        QString parentName = styleElement.attribute("parent-style-name");
        styleInfo->parent = parentName;

        // Limit picture size to 99% of the page size whatever that may be.
        if (styleElement.attribute("family") == "graphic") {
            attList << "max-heigh: 99%" << "max-width: 99%";
            attList << "width: auto" << "height: auto";
        }

        styleInfo->hasBreakBefore = false;
        KoXmlElement propertiesElement;
        forEachElement (propertiesElement, styleElement) {
            //Check for fo:break-before
            if (propertiesElement.hasAttribute("break-before")) {
                //kDebug(30517) << "Found break-before in style" << styleName;
                styleInfo->hasBreakBefore = true;
            }
            handleStyleAttributes(propertiesElement, attList, styleInfo);
        }
        styles.insert(styleName, styleInfo);
        attList.clear();
    }
}

void handleStyleAttributes(KoXmlElement &propertiesElement, QList<QString> &attList,
                           StyleInfo *styleInfo)
{
    // font properties
    QString attribute = propertiesElement.attribute("font-family");
    if (!attribute.isEmpty()) {
        attribute = '"' + attribute + '"';
        attList << ("font-family:" + attribute);
        styleInfo->attributes.insert("font-family", attribute);
    }

    QStringList attributes;
    attributes
        // font
        << "font-style" << "font-variant" << "font-weight" << "font-size"
        // text
        << "text-indent" << "text-align" << "text-decoration" << "white-space"
        // color
        << "color" << "background-color"
        // visual formatting
        << "width" << "min-width" << "max-width"
        << "height" << "min-height" << "max-height" << "line-height" << "vertical-align"
        // border
        << "border-top-width" << "border-bottom-width"
        << "border-left-width" << "border-right-width" << "border-width"
        // border
        << "border-top-color" << "border-bottom-color"
        << "border-left-color" << "border-right-color" << "border-color"
        // border
        << "border-top-style" << "border-bottom-style"
        << "border-left-style" << "border-right-style" << "border-style"
        << "border-top" << "border-bottom" << "border-left" << "border-right" << "border"
        // padding
        << "padding-top" << "padding-bottom" << "padding-left" << "padding-right" << "padding"
        << "margin-top" << "margin-bottom" << "margin-left" << "margin-right" << "margin"
        << "auto";

    foreach(const QString &attrName, attributes) {
        QString attrVal = propertiesElement.attribute(attrName);

        if (!attrVal.isEmpty()) {
            attList << attrName + ':' + attrVal;
            styleInfo->attributes.insert(attrName, attrVal);
        }
    }

    // Visual Display Model
    attribute = propertiesElement.attribute("writing-mode");
    if (!attribute.isEmpty()) {
        if (attribute == "rl")
            attribute = "rtl";
        else if (attribute == "lr")
            attribute = "ltr";
        else
            attribute = "inherited";
        attList << ("direction:" + attribute);
        styleInfo->attributes.insert("direction", attribute);
    }

    // Lists and numbering
    if (propertiesElement.hasAttribute("num-format")) {
        attribute = propertiesElement.attribute("num-format");
        if (!attribute.isEmpty()) {
            if (attribute == "1")
                attribute = "decimal";
            else if (attribute == "i")
                attribute = "lower-roman";
            else if (attribute == "I")
                attribute = "upper-roman";
            else if (attribute == "a")
                attribute = "lower-alpha";
            else if (attribute == "A")
                attribute = "upper-alpha";
            else
                attribute = "decimal";
        }
        styleInfo->attributes.insert("list-style-type:", attribute);
        styleInfo->attributes.insert("list-style-position:", "outside");
    }
    else if (propertiesElement.hasAttribute("bullet-char")){
        attribute = propertiesElement.attribute("bullet-char");
        if (!attribute.isEmpty()) {
            switch (attribute[0].unicode()) {
            case 0x2022:
                attribute = "disc";
                break;
            case 0x25CF:
                attribute = "disc";
                break;
            case 0x25CB:
                attribute = "circle";
                break;
            case 0x25A0:
                attribute = "square";
                break;
            default:
                attribute = "disc";
                break;
            }
        }
        styleInfo->attributes.insert("list-style-type:", attribute);
        styleInfo->attributes.insert("list-style-position:", "outside");
    }
}


// ================================================================
//                         HTML conversion


KoFilter::ConversionStatus convertContent(KoStore *odfStore, QHash<QString, QString> &metaData,
                                          EpubFile *epub,
                                          QHash<QString, StyleInfo*> &styles)
{
    if (!odfStore->open("content.xml")) {
        kDebug(30517) << "Can not open content.html .";
        return KoFilter::FileNotFound;
    }

    QByteArray htmlContent;
    QBuffer *outBuf = new QBuffer(&htmlContent);
    KoXmlWriter *bodyWriter = new KoXmlWriter(outBuf);

    // ----------------------------------------------------------------
    // Parse body from content.xml

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug(30517) << "Error occurred while parsing content.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return KoFilter::ParsingError;
    }

    KoXmlNode currentNode = doc.documentElement();
    KoXmlElement nodeElement;    // currentNode as Element

    currentNode = KoXml::namedItemNS(currentNode, KoXmlNS::office, "body");
    currentNode = KoXml::namedItemNS(currentNode, KoXmlNS::office, "text");

    // Write the beginning of the output.
    bodyWriter->startElement("html");
    bodyWriter->addAttribute("xmlns", "http://www.w3.org/1999/xhtml");
    createHtmlHead(bodyWriter, metaData);
    bodyWriter->startElement("body");

    QString prefix = "body";    // FIXME: Change to "chapter". Beware of hardcoded "body" here and there.
    int      id = 1;            // Number of current output chapter.

    forEachElement (nodeElement, currentNode) {

        //kDebug(30517) << nodeElement.tagName() <<pFlag;
        if ((nodeElement.localName() == "p" && nodeElement.namespaceURI() == KoXmlNS::text)
                || (nodeElement.localName() == "h" && nodeElement.namespaceURI() == KoXmlNS::text)) {

            // A break-before in the style means create a new chapter here,
            // but only if it is a top-level paragraph and not at the very first node.
            StyleInfo *style = styles.value(nodeElement.attribute("style-name"));
            if (style && style->hasBreakBefore) {
                //kDebug(30517) << "Found paragraph with style with break-before -- breaking new chapter";

                // This paragraph is at top level so we should close
                // the html file and start on the next file.
                bodyWriter->endElement();
                bodyWriter->endElement();

                // Write output file to the epub object.
                QString fileId = prefix + QString::number(id);
                QString fileName = "OEBPS/" + fileId + ".html";
                epub->addContentFile(fileId, fileName, "application/xhtml+xml", htmlContent);

                // Prepare for the next file.
                htmlContent.clear();
                delete bodyWriter;
                delete outBuf;
                outBuf = new QBuffer(&htmlContent);
                bodyWriter = new KoXmlWriter(outBuf);
                id++;

                // Write the beginning of the output for the next file.
                bodyWriter->startElement("html");
                bodyWriter->addAttribute("xmlns", "http://www.w3.org/1999/xhtml");
                createHtmlHead(bodyWriter, metaData);
                bodyWriter->startElement("body");
            }
            if (nodeElement.localName() == "p")
                handleTagP(nodeElement, bodyWriter, styles);
            else
                handleTagH(nodeElement, bodyWriter, styles);
        }
        else if (nodeElement.localName() == "span" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagSpan(nodeElement, bodyWriter, styles);
        }
        else if (nodeElement.localName() == "table" && nodeElement.namespaceURI() == KoXmlNS::table) {
            // Handle table
            handleTagTable(nodeElement, bodyWriter, styles);
        }
        else if (nodeElement.localName() == "frame" && nodeElement.namespaceURI() == KoXmlNS::draw)  {
            // Handle frame
            bodyWriter->startElement("div");
            handleTagFrame(nodeElement, bodyWriter, styles);
            bodyWriter->endElement(); // end div
        }
        else if (nodeElement.localName() == "soft-page-break" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagPageBreak(nodeElement, bodyWriter, styles);
        }
        else if (nodeElement.localName() == "list" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagList(nodeElement, bodyWriter, styles);
        }
        else if (nodeElement.localName() == "a" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagA(nodeElement, bodyWriter, styles);
        }
        else if (nodeElement.localName() == "table-of-content" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagTableOfContent(nodeElement, bodyWriter, styles);
        }
        else if (nodeElement.localName() == "line-break" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(bodyWriter);
        }
        else {
            bodyWriter->startElement("div");
            handleUnknownTags(nodeElement, bodyWriter, styles);
            bodyWriter->endElement();
        }
    }

    bodyWriter->endElement(); // body
    bodyWriter->endElement(); // html

    // Write output of the last file to the epub object.
    QString fileId = prefix + QString::number(id);
    QString fileName = "OEBPS/" + fileId + ".html";
    epub->addContentFile(fileId, fileName, "application/xhtml+xml", htmlContent);

    delete bodyWriter;
    delete outBuf;

    odfStore->close();
    return KoFilter::OK;
}


void createHtmlHead(KoXmlWriter *writer, QHash<QString, QString> &metaData)
{
    writer->startElement("head");

    writer->startElement("title");
    writer->addTextNode(metaData.value("title"));
    writer->endElement(); // title

    writer->startElement("meta");
    writer->addAttribute("http-equiv", "Content-Type");
    writer->addAttribute("content", "text/html; charset=utf-8");
    writer->endElement(); // meta

    // write meta tag
    // m-meta <Tagname, Text>
    // <meta name = "Tagname" content = "Text" />
    foreach (const QString &name, metaData.keys()) {
        // Title is handled above.
        if (name == "title")
            continue;

        writer->startElement("meta");
        writer->addAttribute("name", name);
        writer->addAttribute("content", metaData.value(name));
        writer->endElement(); // meta
    }

    writer->startElement("link");
    writer->addAttribute("href", "styles.css");
    writer->addAttribute("type", "text/css");
    writer->addAttribute("rel", "stylesheet");
    writer->endElement(); // link

    writer->endElement(); // head
}


void handleTagTable(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                    QHash<QString, StyleInfo*> &styles)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("table");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }
    bodyWriter->addAttribute("border", "1");

    //===== table-row ======
    KoXmlElement tableElement;
    forEachElement (tableElement, nodeElement) {
        if (tableElement.localName() != "table-column" && tableElement.namespaceURI() == KoXmlNS::table) {
            bodyWriter->startElement("tr");

            // ===== table-cell ======
            KoXmlElement cellElement;
            forEachElement (cellElement, tableElement) {
                QString styleName = cellElement.attribute("style-name");
                StyleInfo *styleInfo = styles.value(styleName);
                bodyWriter->startElement("td");
                if (styleInfo) {
                    styleInfo->inUse = true;
                    bodyWriter->addAttribute("class", styleName);
                }

                // ==== cell text ====
                KoXmlElement cellTextElement;
                forEachElement (cellTextElement, cellElement) {
                    QString cellTag = cellTextElement.localName();
                    if (cellTag == "p" && cellTextElement.namespaceURI() == KoXmlNS::text)
                        handleTagP(cellTextElement, bodyWriter, styles);
                    else if (cellTag == "table" && cellTextElement.namespaceURI() == KoXmlNS::table)
                        handleTagTable(cellTextElement, bodyWriter, styles);
                    else if (cellTag == "frame" && cellTextElement.namespaceURI() == KoXmlNS::draw)
                        handleTagFrame(cellTextElement, bodyWriter, styles);
                }
                bodyWriter->endElement(); // td
            } // end for write tag cell
            bodyWriter->endElement(); //tr
        } // end while write tag row
    }

    bodyWriter->endElement();
}

void handleTagFrame(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                    QHash<QString, StyleInfo*> &styles)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("img");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }
    bodyWriter->addAttribute("alt", "(No Description)");

    // Check image tag to find image source
    KoXmlElement imgElement = nodeElement.firstChildElement();
    QString imgSrc = imgElement.attribute("href").section('/', 1);
    bodyWriter->addAttribute("src", imgSrc);
    bodyWriter->endElement(); // end img
}

void handleTagP(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                QHash<QString, StyleInfo*> &styles)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("p");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }

    KoXmlNode node = nodeElement.firstChild();
    KoXmlElement element = node.toElement();
    // We have characterData or image or span or s  or soft-page break in a tag p
    // FIXME: we should add if there are more tags.
    while (!node.isNull()) {

        if (node.isText())
            handleCharacterData(node, bodyWriter, styles);
        else if (element.localName() == "span" && element.namespaceURI() == KoXmlNS::text)
            handleTagSpan(element, bodyWriter, styles);
        else if (element.localName() == "frame" && element.namespaceURI() == KoXmlNS::draw)
            handleTagFrame(element, bodyWriter, styles);
        else if (element.localName() == "soft-page-break" && element.namespaceURI() == KoXmlNS::text)
            handleTagPageBreak(element, bodyWriter, styles);
        else if (element.localName() == "a" && element.namespaceURI() == KoXmlNS::text)
            handleTagA(element, bodyWriter, styles);
        else if (element.localName() == "s" && element.namespaceURI() == KoXmlNS::text) {
            bodyWriter->addTextNode("\u00a0");
        }
        else if (element.localName() == "line-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(bodyWriter);
        }
        else
            handleUnknownTags(element, bodyWriter, styles);

        node = node.nextSibling();
        element = node.toElement();
    }
    bodyWriter->endElement();
}

void handleCharacterData(KoXmlNode &node, KoXmlWriter *bodyWriter,
                         QHash<QString, StyleInfo*> &styles)
{
    Q_UNUSED(styles);

    KoXmlText charData = node.toText();
    bodyWriter->addTextNode(charData.data());
}

void handleTagSpan(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                   QHash<QString, StyleInfo*> &styles)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("span");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }

    KoXmlNode node = nodeElement.firstChild();
    KoXmlElement element = node.toElement();
    // We have characterData or image or span or s  or soft-page break in a tag p
    // FIXME: we should add if there are more tags.
    while (!node.isNull()) {

        if (node.isText())
            handleCharacterData(node, bodyWriter, styles);
        else if (element.localName() == "span" && element.namespaceURI() == KoXmlNS::text)
            handleTagSpan(element, bodyWriter, styles);
        else if (element.localName() == "frame" && element.namespaceURI() == KoXmlNS::draw)
            handleTagFrame(element, bodyWriter, styles);
        else if (element.localName() == "soft-page-break" && element.namespaceURI() == KoXmlNS::text)
            handleTagPageBreak(element, bodyWriter, styles);
        else if (element.localName() == "a" && element.namespaceURI() == KoXmlNS::text)
            handleTagA(element, bodyWriter, styles);
        else if (element.localName() == "tab" && element.namespaceURI() == KoXmlNS::text)
            handleTagTab(element, bodyWriter, styles);
        else if (element.localName() == "s" && element.namespaceURI() == KoXmlNS::text) {
            bodyWriter->addTextNode("\u00a0");
        }
        else if (element.localName() == "line-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(bodyWriter);
        }
        else
            handleUnknownTags(element, bodyWriter, styles);

        node = node.nextSibling();
        element = node.toElement();
    }

    bodyWriter->endElement(); // span
}

void handleTagPageBreak(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                        QHash<QString, StyleInfo*> &styles)
{
    Q_UNUSED(styles);

    bodyWriter->addTextNode(nodeElement.text().toUtf8());
}

void handleTagH(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                QHash<QString, StyleInfo*> &styles)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("h1");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }

    KoXmlNode node = nodeElement.firstChild();
    KoXmlElement element = node.toElement();
    // We have characterData or image or span or s  or soft-page break in a tag p
    // FIXME: we should add if there are more tags.

    while (!node.isNull()) {

        if (node.isText())
            handleCharacterData(node, bodyWriter, styles);
        else if (element.localName() == "span" && element.namespaceURI() == KoXmlNS::text)
            handleTagSpan(element, bodyWriter, styles);
        else if (element.localName() == "frame" && element.namespaceURI() == KoXmlNS::draw)
            handleTagFrame(element, bodyWriter, styles);
        else if (element.localName() == "soft-page-break" && element.namespaceURI() == KoXmlNS::text)
            handleTagPageBreak(element, bodyWriter, styles);
        else if (element.localName() == "a" && element.namespaceURI() == KoXmlNS::text)
            handleTagA(element, bodyWriter, styles);
        else if (element.localName() == "s" && element.namespaceURI() == KoXmlNS::text) {
            bodyWriter->addTextNode("\u00a0");
        }
        else if (element.localName() == "line-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(bodyWriter);
        }
        else
            handleUnknownTags(element, bodyWriter, styles);

        node = node.nextSibling();
        element = node.toElement();
    }
    bodyWriter->endElement();
}

void handleTagList(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                         QHash<QString, StyleInfo *> &styles)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("ul");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }

    KoXmlElement listItem;
    forEachElement (listItem, nodeElement) {
        bodyWriter->startElement("li");
        KoXmlElement listItemElement;
        forEachElement (listItemElement, listItem) {
            if (listItemElement.localName() == "p" && listItemElement.namespaceURI() == KoXmlNS::text)
                handleTagP(listItemElement, bodyWriter, styles);
        }
        bodyWriter->endElement();
    }
    bodyWriter->endElement();
}

void handleTagA (KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                 QHash<QString, StyleInfo *> &styles)
{
    bodyWriter->startElement("a");
    bodyWriter->addAttribute("href", nodeElement.attribute("href"));

    // it can be more just for now too see more documents
    KoXmlElement child = nodeElement.firstChildElement();
    if (child.localName() == "span" && child.namespaceURI() == KoXmlNS::text)
        handleTagSpan(child, bodyWriter, styles);

    bodyWriter->endElement();
}

void handleTagTab (KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                   QHash<QString, StyleInfo *> &styles)
{
    for (int i = 0; i <10; i++)
        bodyWriter->addTextNode("\u00a0");
}

void handleTagTableOfContent (KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                              QHash<QString, StyleInfo *> &styles)
{
    KoXmlNode indexBody = KoXml::namedItemNS(nodeElement, KoXmlNS::text, "index-body");
    KoXmlElement  element;
    forEachElement (element, indexBody) {
        if (element.localName() == "index-title" && element.namespaceURI() == KoXmlNS::text) {
            KoXmlElement titleElement;
            forEachElement (titleElement, element) {
                if (titleElement.localName() == "p" && titleElement.namespaceURI() == KoXmlNS::text)
                    handleTagP(titleElement, bodyWriter, styles);
            }
        }// end of index-title
        else
            handleTagTableOfContentBody(element, bodyWriter, styles);
    }
}

void handleTagTableOfContentBody (KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                                  QHash<QString, StyleInfo *> &styles)
{
    if (nodeElement.localName() == "p" && nodeElement.namespaceURI() == KoXmlNS::text) {
        bodyWriter->startElement("p");
        KoXmlElement element;
        // We have characterData or image or span or s  or soft-page break in a tag p
        // FIXME: we should add if there are more tags.

        forEachElement (element, nodeElement) {
            if (element.localName() == "span" && element.namespaceURI() == KoXmlNS::text) {
                kDebug(30517)<<"in span ****";
                handleTagSpan(element, bodyWriter, styles);
            }
            else if (element.localName() == "a" && element.namespaceURI() == KoXmlNS::text) {
                // We ignore many elemetns here especially Tag a. just check tags, we want.
                KoXmlElement child = element.firstChildElement();
                if (child.localName() == "span" && child.namespaceURI() == KoXmlNS::text)
                    handleTagSpan(child, bodyWriter, styles);
            }
            else if (element.localName() == "s" && element.namespaceURI() == KoXmlNS::text) {
                bodyWriter->addTextNode("\u00a0");
            }
        }
        bodyWriter->endElement();
    }
}

void handleTagLineBreak (KoXmlWriter *bodyWriter)
{
    bodyWriter->startElement("br");
    bodyWriter->endElement();
}

void handleUnknownTags (KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                        QHash<QString, StyleInfo *> &styles)
{
    //just go dipper to find known tags

    KoXmlNode node = nodeElement.firstChild();
    KoXmlElement element = node.toElement();
    while (!node.isNull()) {

        if (node.isText())
            handleCharacterData(node, bodyWriter, styles);
        else if (element.localName() == "p" && element.namespaceURI() == KoXmlNS::text)
            handleTagP(element, bodyWriter, styles);
        else if (element.localName() == "span" && element.namespaceURI() == KoXmlNS::text)
            handleTagSpan(element, bodyWriter, styles);
        else if (element.localName() == "frame" && element.namespaceURI() == KoXmlNS::draw)
            handleTagFrame(element, bodyWriter, styles);
        else if (element.localName() == "soft-page-break" && element.namespaceURI() == KoXmlNS::text)
            handleTagPageBreak(element, bodyWriter, styles);
        else if (element.localName() == "a" && element.namespaceURI() == KoXmlNS::text)
            handleTagA(element, bodyWriter, styles);
        else if (element.localName() == "s" && element.namespaceURI() == KoXmlNS::text) {
            bodyWriter->addTextNode("\u00a0");
        }
        else if (element.localName() == "line-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(bodyWriter);
        }
        else
            handleUnknownTags(element, bodyWriter, styles);

        node = node.nextSibling();
        element = node.toElement();
    }
}

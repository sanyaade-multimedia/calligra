/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include <qtextstream.h>
#include <qfile.h>
#include <qdom.h>
#include <qpainter.h>
#include <qevent.h>
#include <qstring.h>

#include "bracketelement.h"
#include "contextstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "kformulacontainer.h"
#include "matrixelement.h"
#include "numberelement.h"
#include "operatorelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "symbolelement.h"
#include "textelement.h"
#include "kformulacommand.h"


KFormulaContainer::KFormulaContainer()
{
    rootElement = new FormulaElement(this);
    dirty = true;
    testDirty();
    //undoStack.setAutoDelete(true);
    //redoStack.setAutoDelete(true);
}

KFormulaContainer::~KFormulaContainer()
{
    delete rootElement;
}


FormulaCursor* KFormulaContainer::createCursor()
{
    FormulaCursor* cursor = new FormulaCursor(rootElement);
    connect(this, SIGNAL(elementWillVanish(BasicElement*)),
            cursor, SLOT(elementWillVanish(BasicElement*)));
    connect(this, SIGNAL(formulaLoaded(FormulaElement*)),
            cursor, SLOT(formulaLoaded(FormulaElement*)));
    return cursor;
}

void KFormulaContainer::destroyCursor(FormulaCursor* cursor)
{
    delete cursor;
}


/**
 * Gets called just before the child is removed from
 * the element tree.
 */
void KFormulaContainer::elementRemoval(BasicElement* child)
{
    emit elementWillVanish(child);
}

/**
 * Gets called whenever something changes and we need to
 * recalc.
 */
void KFormulaContainer::changed()
{
    dirty = true;
}


/**
 * Draws the whole thing.
 */
void KFormulaContainer::draw(QPainter& painter)
{
    rootElement->draw(painter, context);
}


void KFormulaContainer::execute(KFormulaCommand *command)
{
    command->execute();
    if (!command->isSenseless()) {
        history.addCommand(command, false);
    }
    else {
        delete command;
    }
}

void KFormulaContainer::addText(FormulaCursor* cursor, QChar ch)
{
    setActiveCursor(cursor);
    if (cursor->isSelection()) {
 	KFCRemoveSelection* command = new KFCRemoveSelection(this, BasicElement::beforeCursor);
 	execute(command);
    }

    KFCAddText* command = new KFCAddText(this, ch);
    execute(command);
}

void KFormulaContainer::addNumber(FormulaCursor* cursor, QChar ch)
{
    setActiveCursor(cursor);
    if (cursor->isSelection()) {
 	KFCRemoveSelection* command = new KFCRemoveSelection(this, BasicElement::beforeCursor);
 	execute(command);
    }

    KFCAddNumber* command = new KFCAddNumber(this, ch);
    execute(command);
}

void KFormulaContainer::addOperator(FormulaCursor* cursor, QChar ch)
{
    setActiveCursor(cursor);
    if (cursor->isSelection()) {
 	KFCRemoveSelection* command = new KFCRemoveSelection(this, BasicElement::beforeCursor);
 	execute(command);
    }

    KFCAddOperator* command = new KFCAddOperator(this, ch);
    execute(command);
}

void KFormulaContainer::addBracket(FormulaCursor* cursor, char left, char right)
{
    setActiveCursor(cursor);
    KFCAddBracket* command = new KFCAddBracket(this, left, right);
    execute(command);
}

void KFormulaContainer::addFraction(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    KFCAddFraction* command = new KFCAddFraction(this);
    execute(command);
}


void KFormulaContainer::addRoot(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    KFCAddRoot* command = new KFCAddRoot(this);
    execute(command);
}


void KFormulaContainer::addSymbol(FormulaCursor* cursor,
                                  Artwork::SymbolType type)
{
    setActiveCursor(cursor);
    KFCAddSymbol* command = new KFCAddSymbol(this, type);
    execute(command);
}

void KFormulaContainer::addMatrix(FormulaCursor* cursor, int rows, int columns)
{
    setActiveCursor(cursor);
    if (cursor->isSelection()) {
 	KFCRemoveSelection* command = new KFCRemoveSelection(this, BasicElement::beforeCursor);
 	execute(command);
    }

    KFCAddMatrix* command = new KFCAddMatrix(this, rows, columns);
    execute(command);
}


void KFormulaContainer::addLowerLeftIndex(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        KFCAddIndex* command = new KFCAddIndex(this, element, element->getLowerLeft());
 	execute(command);
    }
    else {
        addGenericIndex(cursor, element->getLowerLeft());
    }
}

void KFormulaContainer::addUpperLeftIndex(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        KFCAddIndex* command = new KFCAddIndex(this, element, element->getUpperLeft());
 	execute(command);
    }
    else {
        addGenericIndex(cursor, element->getUpperLeft());
    }
}

void KFormulaContainer::addLowerRightIndex(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        KFCAddIndex* command = new KFCAddIndex(this, element, element->getLowerRight());
 	execute(command);
    }
    else {
        addGenericIndex(cursor, element->getLowerRight());
    }
}

void KFormulaContainer::addUpperRightIndex(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        KFCAddIndex* command = new KFCAddIndex(this, element, element->getUpperRight());
 	execute(command);
    }
    else {
        addGenericIndex(cursor, element->getUpperRight());
    }
}


/**
 * Just search an element that gets an index. Don't create one
 * if there is non.
 */
void KFormulaContainer::addGenericLowerIndex(FormulaCursor* cursor)
{
    SymbolElement* symbol = cursor->getActiveSymbolElement();
    if (symbol != 0) {
        addGenericIndex(cursor, symbol->getLowerIndex());
    }
}

/**
 * Just search an element that gets an index. Don't create one
 * if there is non.
 */
void KFormulaContainer::addGenericUpperIndex(FormulaCursor* cursor)
{
    SymbolElement* symbol = cursor->getActiveSymbolElement();
    if (symbol != 0) {
        addGenericIndex(cursor, symbol->getUpperIndex());
    }
    else {
        RootElement* root = cursor->getActiveRootElement();
        if (root != 0) {
            addGenericIndex(cursor, root->getIndex());
        }
    }
}

/**
 * Helper function that inserts the index it was called with.
 */
void KFormulaContainer::addGenericIndex(FormulaCursor* cursor, ElementIndexPtr index)
{
    setActiveCursor(cursor);
    if (!index->hasIndex()) {
        KFCAddGenericIndex* command = new KFCAddGenericIndex(this, index);
        execute(command);
    }
    else {
        index->moveToIndex(cursor, BasicElement::afterCursor);
        cursor->setSelection(false);
    }
}


void KFormulaContainer::removeSelection(FormulaCursor* cursor,
                                        BasicElement::Direction direction)
{
    setActiveCursor(cursor);
    KFCRemove* command = new KFCRemove(this, direction);
    execute(command);
}


void KFormulaContainer::replaceElementWithMainChild(FormulaCursor* cursor,
                                                    BasicElement::Direction direction)
{
    setActiveCursor(cursor);
    if (!cursor->isSelection()) {
        KFCRemoveEnclosing* command = new KFCRemoveEnclosing(this, direction);
        execute(command);
    }
}


void KFormulaContainer::paste(FormulaCursor* cursor, QMimeSource* source)
{
    setActiveCursor(cursor);
    if (source->provides("application/x-kformula")) {
        QByteArray data = source->encodedData("application/x-kformula");
        //cerr << data <<"aaa"<< endl;
        QDomDocument formula;
        formula.setContent(data);

        QList<BasicElement> list;
        list.setAutoDelete(true);
        if (cursor->buildElementsFromDom(formula, list)) {
            KFCPaste* command = new KFCPaste(this, list);
            execute(command);
        }
    }
}

void KFormulaContainer::undo()
{
    history.undo();
}


void KFormulaContainer::undo(FormulaCursor *cursor)
{
    setActiveCursor(cursor);
    undo();
}

void KFormulaContainer::redo()
{
    history.redo();
}

void KFormulaContainer::redo(FormulaCursor *cursor)
{
    setActiveCursor(cursor);
    redo();
}

// void KFormulaContainer::pushUndoStack(KFormulaCommand *command) 
// { 
//     undoStack.push(command); 

// //emit signals

// }

// void KFormulaContainer::pushRedoStack(KFormulaCommand *command) 
// { 
//     redoStack.push(command); 

// //emit signals

// }

QRect KFormulaContainer::boundingRect()
{
    return QRect(0, 0, rootElement->getWidth(), rootElement->getHeight());
}


void KFormulaContainer::testDirty()
{
    if (dirty) {
        dirty = false;
        rootElement->calcSizes(context);
        emit formulaChanged();
    }
}

QDomDocument KFormulaContainer::domData()
{
    QDomDocument doc("KFORMULA");
    doc.appendChild(rootElement->getElementDom(doc));
    return doc;
}

void KFormulaContainer::save(QString file)
{
    QFile f(file);
    if(!f.open(IO_Truncate | IO_ReadWrite)) {
        cerr << "Error" << endl;
        return;
    }
    QCString data=domData().toCString();
    cerr << (const char *)data << endl;    
  
    QTextStream str(&f);
    domData().save(str,4);
    f.close();
}	

void KFormulaContainer::load(QString file)
{
    QFile f(file);
    if (!f.open(IO_ReadOnly)) {
        cerr << "Error" << endl;
        return;
    }
    QDomDocument doc;
    if (!doc.setContent(&f)) {
        f.close();
        return;
    }
    QDomElement fe = doc.firstChild().toElement();
    if (!fe.isNull()) {
        FormulaElement* root = new FormulaElement(this);
        if (root->buildFromDom(fe)) {
            delete rootElement;
            rootElement = root;
            dirty = true;
            testDirty();
            //cleanRedoStack();
            //cleanUndoStack();
            history.clear();

            emit formulaLoaded(rootElement);
        }
        else {
            delete root;
            cerr << "Error constructing element tree." << endl;
        }
    }
    f.close();
}

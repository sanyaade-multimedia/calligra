/* This file is part of the KDE project
   Copyright (C) 2001 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "KWordDocIface.h"

#include "kwdoc.h"

#include <dcopclient.h>
#include <koVariable.h>
KWordDocIface::KWordDocIface( KWDocument *doc_ )
    : KoDocumentIface( doc_ )
{
   doc = doc_;
}


int KWordDocIface::getNumPages()
{
    return doc->getPages();
}

int KWordDocIface::getNumFrameSets()
{
    return doc->getNumFrameSets();
}

unsigned int KWordDocIface::getColumns()
{
    return doc->getColumns();
}

double KWordDocIface::ptTopBorder()
{
    return doc->ptTopBorder();
}

double KWordDocIface::ptBottomBorder()
{
    return doc->ptBottomBorder();
}

double KWordDocIface::ptLeftBorder()
{
    return doc->ptLeftBorder();
}

double KWordDocIface::ptRightBorder()
{
    return doc->ptRightBorder();
}

double KWordDocIface::ptPaperHeight()
{
    return doc->ptPaperHeight();
}

double KWordDocIface::ptPaperWidth()
{
    return doc->ptPaperWidth();
}

double KWordDocIface::ptColumnWidth()
{
    return doc->ptColumnWidth();
}

double KWordDocIface::ptColumnSpacing()
{
    return doc->ptColumnSpacing();
}

double KWordDocIface::ptPageTop( int pgNum )
{
    return doc->ptPageTop(pgNum);
}

double KWordDocIface::gridX()
{
    return doc->gridX();
}

double KWordDocIface::gridY()
{
    return doc->gridY();
}

void KWordDocIface::setGridX(double _gridx)
{
    doc->setGridX(_gridx);
}

void KWordDocIface::setGridY(double _gridy)
{
    doc->setGridY(_gridy);
}

QString KWordDocIface::getUnitName()
{
    return doc->getUnitName();
}

double KWordDocIface::getIndentValue()
{
    return doc->getIndentValue();
}

void KWordDocIface::setIndentValue(double _ind)
{
    doc->setIndentValue(_ind);
}

int KWordDocIface::getNbPagePerRow()
{
    return doc->getNbPagePerRow();
}

void KWordDocIface::setNbPagePerRow(int _nb)
{
    doc->setNbPagePerRow(_nb);
}

int KWordDocIface::defaultColumnSpacing()
{
    return doc->defaultColumnSpacing();
}

void KWordDocIface::setDefaultColumnSpacing(int _val)
{
    doc->setDefaultColumnSpacing(_val);
}

int KWordDocIface::maxRecentFiles()
{
    return doc->maxRecentFiles();
}

void KWordDocIface::setUndoRedoLimit(int _val)
{
    doc->setUndoRedoLimit(_val);
}


void KWordDocIface::recalcAllVariables()
{
    //recalc all variable
    doc->recalcVariables(VT_ALL);
}

void KWordDocIface::recalcVariables(int _var)
{
    doc->recalcVariables(_var);
}

void KWordDocIface::recalcVariables(const QString &varName)
{
    if(varName=="VT_DATE")
        doc->recalcVariables(0);
    else if(varName=="VT_TIME")
        doc->recalcVariables(2);
    else if(varName=="VT_PGNUM")
        doc->recalcVariables(4);
    else if(varName=="VT_CUSTOM")
        doc->recalcVariables(6);
    else if(varName=="VT_SERIALLETTER")
        doc->recalcVariables(7);
    else if(varName=="VT_FIELD")
        doc->recalcVariables(8);
    else if(varName=="VT_ALL")
        doc->recalcVariables(256);
}


bool KWordDocIface::showRuler() const
{
    return doc->showRuler();
}

bool KWordDocIface::dontCheckUpperWord()
{
    return doc->dontCheckUpperWord();
}

bool KWordDocIface::dontCheckTitleCase() const
{
    return doc->dontCheckTitleCase();
}

bool KWordDocIface::showdocStruct() const
{
    return doc->showdocStruct();
}

bool KWordDocIface::viewFrameBorders() const
{
    return doc->viewFrameBorders();
}

void KWordDocIface::setHeaderVisible( bool b)
{
    doc->setHeaderVisible(b);
    doc->refreshGUIButton();
}

void KWordDocIface::setFooterVisible( bool b)
{
    doc->setFooterVisible( b);
    doc->refreshGUIButton();
}

void KWordDocIface::setViewFrameBorders( bool b )
{
    doc->setViewFrameBorders( b );
    doc->refreshGUIButton();
}

void KWordDocIface::setShowRuler(bool b)
{
    doc->setShowRuler(b);
    doc->refreshGUIButton();
    doc->reorganizeGUI();
}

bool KWordDocIface::viewFormattingChars() const
{
    return doc->viewFormattingChars();
}

void KWordDocIface::setViewFormattingChars(bool b)
{
    doc->setViewFormattingChars(b);
    doc->refreshGUIButton();
}

void KWordDocIface::setShowDocStruct(bool b)
{
    doc->setShowDocStruct(b);
    doc->refreshGUIButton();
    doc->reorganizeGUI();
}

int KWordDocIface::startingPage()
{
    return doc->getVariableCollection()->variableSetting()->startingPage();
}

void KWordDocIface::setStartingPage(int nb)
{
    doc->getVariableCollection()->variableSetting()->setStartingPage(nb);
    doc->recalcVariables(VT_PGNUM);
}

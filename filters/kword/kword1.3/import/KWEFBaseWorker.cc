//

/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <qbuffer.h>
#include <qimage.h>

#include <kdebug.h>

#include <koPicture.h>

#include "KWEFStructures.h"
#include "KWEFBaseWorker.h"
#include "KWEFKWordLeader.h"

//
// At first, define all methods that do something real!
//

QImage KWEFBaseWorker::loadAndConvertToImage(const QString& strName, const QString& inExtension) const
{
    QIODevice* io=getSubFileDevice(strName);
    if (!io)
    {
        // NO message error, as there must be already one
        return QImage();
    }

    kdDebug(30508) << "Picture " << strName << " has size: " << io->size() << endl;

    KoPicture picture;
    if (!picture.load(io, inExtension)) // we do not care about KoPictureKey
    {
        kdWarning(30508) << "Could not read picture: " << strName << " (KWEFBaseWorker::loadAndConvertToImage)" << endl;
        return QImage();
    }
    
    return picture.generateImage(picture.getOriginalSize()); // ### TODO: KoPicture::getOriginalSize is bad for cliparts
}

bool KWEFBaseWorker::loadAndConvertToImage(const QString& strName, const QString& inExtension, const QString& outExtension, QByteArray& image) const
{
    QImage qimage(loadAndConvertToImage(strName,inExtension));
    
    if (qimage.isNull())
    {
        kdWarning(30508) << "Could not load image (KWEFBaseWorker::loadAndConvertToImage)" <<endl;
        return false;
    }
    
    QImageIO imageIO;
    imageIO.setImage(qimage);

    QBuffer buffer(image); // A QBuffer is a QIODevice
    if (!buffer.open(IO_WriteOnly))
    {
        kdWarning(30508) << "Could not open buffer! (KWEFBaseWorker::loadAndConvertToImage)" << endl;
        return false;
    }

    imageIO.setIODevice(&buffer);
    imageIO.setFormat(outExtension.utf8());

    if (!imageIO.write())
    {
        kdWarning(30508) << "Could not write converted image! (KWEFBaseWorker::loadAndConvertToImage)" << endl;
        return false;
    }
    buffer.close();

    return true;
}


//
// Define all methods returning true
//

#define DO_TRUE_DEFINITION(string) \
    bool KWEFBaseWorker::string \
    {\
        return true;\
    }

DO_TRUE_DEFINITION (doFullDocumentInfo (const KWEFDocumentInfo&))
DO_TRUE_DEFINITION (doVariableSettings (const VariableSettingsData &))
DO_TRUE_DEFINITION (doFullPaperFormat (const int, const double, const double, const int))
DO_TRUE_DEFINITION (doFullPaperBorders (const double, const double, const double, const double))
DO_TRUE_DEFINITION (doPageInfo(int,int))
DO_TRUE_DEFINITION (doOpenHead (void))
DO_TRUE_DEFINITION (doCloseHead (void))
DO_TRUE_DEFINITION (doOpenBody (void))
DO_TRUE_DEFINITION (doCloseBody (void))
DO_TRUE_DEFINITION (doOpenStyles (void))
DO_TRUE_DEFINITION (doCloseStyles (void))
DO_TRUE_DEFINITION (doFullDefineStyle (LayoutData&))
DO_TRUE_DEFINITION (doOpenSpellCheckIgnoreList (void))
DO_TRUE_DEFINITION (doCloseSpellCheckIgnoreList (void))
DO_TRUE_DEFINITION (doFullSpellCheckIgnoreWord (const QString&))
DO_TRUE_DEFINITION (doHeader(const HeaderData&))
DO_TRUE_DEFINITION (doFooter(const FooterData&))


/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#include <stdlib.h>

#include <kdebug.h>

#include "fileheader.h"	/* for the use of _header (color and underlined) */
#include "format.h"

/* Get the set of info. about a text format */
void Format::analyse(const QDomNode balise)
{
	/* <FORMAT id="1" pos="0" len="17"> */
	setId(getAttr(balise, "ID").toInt());
	//setPos(getAttr(balise, "pos").toInt());
	//setTaille (getAttr(balise, "len").toInt());
}


/***************************************************************************
                          aicolor.cpp  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "aicolor.h"
#include <stdlib.h>

AIColor::AIColor(){
  ctype = AIColor::CT_CMYK;
  cdata.cmykdata.cvalue = 0;
  cdata.cmykdata.mvalue = 0;
  cdata.cmykdata.yvalue = 0;
  cdata.cmykdata.kvalue = 0;
  _freedata = NULL;
}

AIColor::AIColor( const AIColor& value ){
  ctype = value.ctype;
  memcpy (&cdata, &value.cdata, sizeof (cdata));
  _freedata = NULL;
}
AIColor::AIColor( double c, double m, double y, double k ){
  ctype = AIColor::CT_CMYK;
  cdata.cmykdata.cvalue = 0;
  cdata.cmykdata.mvalue = 0;
  cdata.cmykdata.yvalue = 0;
  cdata.cmykdata.kvalue = 1;
  _freedata = NULL;
}
AIColor::AIColor( double c, double m, double y, double k, const char* colorname, double gray ){
  ctype = AIColor::CT_CMYK_Key;
  cdata.cmykdata.cvalue = c;
  cdata.cmykdata.mvalue = m;
  cdata.cmykdata.yvalue = y;
  cdata.cmykdata.kvalue = k;
  cdata.cmykdata.colorname = strdup (colorname);
  _freedata = cdata.cmykdata.colorname;
}
AIColor::AIColor( double gray ){
  ctype = AIColor::CT_Gray;
  cdata.graydata = gray;
  _freedata = NULL;
}

AIColor::~AIColor(){
//  if (_freedata) free (_freedata);
}

void AIColor::toRGB (double &r, double &g, double &b)
{
  switch (ctype)
  {
    case CT_CMYK :
    case CT_CMYK_Key :
      r = 1 - cdata.cmykdata.cvalue - cdata.cmykdata.kvalue;
      g = 1 - cdata.cmykdata.mvalue - cdata.cmykdata.kvalue;
      b = 1 - cdata.cmykdata.yvalue - cdata.cmykdata.kvalue;
      break;
    case CT_Gray :
      r = cdata.graydata;
      g = cdata.graydata;
      b = cdata.graydata;
    default :
      qDebug ("unknown colortype %d", ctype);
  }
}

void AIColor::toCMYK (double &c, double &m, double &y, double &k)
{
  switch (ctype)
  {
    case CT_CMYK :
    case CT_CMYK_Key :
      c = cdata.cmykdata.cvalue;
      m = cdata.cmykdata.mvalue;
      y = cdata.cmykdata.yvalue;
      k = cdata.cmykdata.kvalue;
      break;
    case CT_Gray :
      c = 0;
      m = 0;
      y = 0;
      k = cdata.graydata;
    default :
      qDebug ("unknown colortype %d", ctype);
  }
}



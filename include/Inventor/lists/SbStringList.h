#ifndef COIN_SBSTRINGLIST_H
#define COIN_SBSTRINGLIST_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2004 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org/> for more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no/>.
 *
\**************************************************************************/

#include <Inventor/lists/SbPList.h>

class SbString;

class COIN_DLL_API SbStringList : public SbPList {
public:
  SbStringList(void) { }
  SbStringList(const int sizehint) : SbPList(sizehint) { }

  void append(SbString * string) {
    SbPList::append((void*)string);
  }
  int find(SbString * string) const {
    return SbPList::find((void*) string);
  }
  void insert(SbString * string, int insertbefore) {
    SbPList::insert((void*)string, insertbefore);
  }
  SbString *& operator[](const int idx) const {
    return (SbString*&) ((*(const SbPList*)this)[idx]);
  }
  const SbString ** getArrayPtr(void) const {
    return (const SbString**) SbPList::getArrayPtr();
  }
};

#endif // !COIN_SBSTRINGLIST_H

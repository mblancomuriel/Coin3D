#ifndef COIN_SOLISTS_H
#define COIN_SOLISTS_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2001 by Systems in Motion.  All rights reserved.
 *  
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  version 2 as published by the Free Software Foundation.  See the
 *  file LICENSE.GPL at the root directory of this source distribution
 *  for more details.
 *
 *  If you desire to use Coin with software that is incompatible
 *  licensewise with the GPL, and / or you would like to take
 *  advantage of the additional benefits with regard to our support
 *  services, please contact Systems in Motion about acquiring a Coin
 *  Professional Edition License.  See <URL:http://www.coin3d.org> for
 *  more information.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  <URL:http://www.sim.no>, <mailto:support@sim.no>
 *
\**************************************************************************/

#if defined(COIN_INTERNAL)
#error "Do not include SoLists.h internally."
#endif // COIN_INTERNAL

// This file's sole purpose is to provide compatibility with the way
// Open Inventor lays out its header files for the list classes.

#include <Inventor/lists/SbIntList.h>
#include <Inventor/lists/SbPList.h>
#include <Inventor/lists/SoBaseList.h>
#include <Inventor/lists/SoDetailList.h>
#include <Inventor/lists/SoEnabledElementsList.h>
#include <Inventor/lists/SoEngineOutputList.h>
#include <Inventor/lists/SoEngineList.h>
#include <Inventor/lists/SoFieldList.h>
#include <Inventor/lists/SoNodeList.h>
#include <Inventor/lists/SoPathList.h>
#include <Inventor/lists/SoPickedPointList.h>
#include <Inventor/lists/SoTypeList.h>

#endif // !COIN_SOLISTS_H

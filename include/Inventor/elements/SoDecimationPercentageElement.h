#ifndef COIN_SODECIMATIONPERCENTAGEELEMENT_H
#define COIN_SODECIMATIONPERCENTAGEELEMENT_H

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

#include <Inventor/elements/SoFloatElement.h>

class COIN_DLL_API SoDecimationPercentageElement : public SoFloatElement {
  typedef SoFloatElement inherited;

  SO_ELEMENT_HEADER(SoDecimationPercentageElement);
public:
  static void initClass(void);
protected:
  virtual ~SoDecimationPercentageElement();

public:
  virtual void init(SoState * state);
  static void set(SoState * const state, SoNode * const node,
                   const float complexity);
  static void set(SoState * const state, const float complexity);
  static float get(SoState * const state);
  static float getDefault(void);

};

#endif // !COIN_SODECIMATIONPERCENTAGEELEMENT_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2003 by Systems in Motion.  All rights reserved.
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
 *  See <URL:http://www.coin3d.org> for  more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

/*!
  \class SoSFVec4f SoSFVec4f.h Inventor/fields/SoSFVec4f.h
  \brief The SoSFVec4f class is a container for an SbVec4f vector.
  \ingroup fields

  This field is used where nodes, engines or other field containers
  needs to store a single vector with four elements.

  \sa SoMFVec4f

*/

#include <Inventor/fields/SoSFVec4f.h>

#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoSubFieldP.h>

SO_SFIELD_SOURCE(SoSFVec4f, SbVec4f, const SbVec4f &);


// Override from parent class.
void
SoSFVec4f::initClass(void)
{
  SO_SFIELD_INTERNAL_INIT_CLASS(SoSFVec4f);
}

// No need to document readValue() and writeValue() here, as the
// necessary information is provided by the documentation of the
// parent classes.
#ifndef DOXYGEN_SKIP_THIS

SbBool
SoSFVec4f::readValue(SoInput * in)
{
  return 
    in->read(this->value[0]) && 
    in->read(this->value[1]) &&
    in->read(this->value[2]) &&
    in->read(this->value[3]);
}

// Write integer value to output stream. Also used from SoMFVec4f
// class.
void
sosfvec4f_write_value(SoOutput * out, const SbVec4f & v)
{
  out->write(v[0]);
  if (!out->isBinary()) out->write(' ');
  out->write(v[1]);
  if (!out->isBinary()) out->write(' ');
  out->write(v[2]);
  if (!out->isBinary()) out->write(' ');
  out->write(v[3]);
}

void
SoSFVec4f::writeValue(SoOutput * out) const
{
  sosfvec4f_write_value(out, this->getValue());
}

#endif // DOXYGEN_SKIP_THIS


/*!
  Set value of vector.
*/
void
SoSFVec4f::setValue(const float x, const float y, const float z, const float w)
{
  this->setValue(SbVec4f(x, y, z, w));
}

/*!
  Set value of vector.
*/
void
SoSFVec4f::setValue(const float xyzw[4])
{
  this->setValue(SbVec4f(xyzw));
}

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2000 by Systems in Motion. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation. See the
 *  file LICENSE.LGPL at the root directory of the distribution for
 *  more details.
 *
 *  If you want to use Coin for applications not compatible with the
 *  LGPL, please contact SIM to acquire a Professional Edition license.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  http://www.sim.no support@sim.no Voice: +47 22114160 Fax: +47 22207097
 *
\**************************************************************************/

/*!
  \class SoShapeStyleElement Inventor/elements/SoShapeStyleElement.h
  \brief The SoShapeStyleElement class is yet to be documented.

  FIXME: write doc.
*/

#include <Inventor/elements/SoShapeStyleElement.h>

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoLightModelElement.h>

#include <coindefs.h> // COIN_STUB()
#include <assert.h>

#define FLAG_LIGHTING     0x0001
#define FLAG_TEXENABLED   0x0002
#define FLAG_TEXFUNC      0x0004
#define FLAG_BBOXCMPLX    0x0008
#define FLAG_INVISIBLE    0x0010
#define FLAG_ABORTCB      0x0020
#define FLAG_SCREENDOOR   0x0040
#define FLAG_OVERRIDE     0x0080

#define FLAG_TEXMASK (FLAG_TEXENABLED|FLAG_TEXFUNC)
#define FLAG_DELAYMASK (FLAG_BBOXCMPLX|FLAG_INVISIBLE|FLAG_ABORTCB)

SO_ELEMENT_SOURCE(SoShapeStyleElement);

/*!
  This static method initializes static data for the SoShapeStyleElement class.
*/

void
SoShapeStyleElement::initClass(void)
{
  SO_ELEMENT_INIT_CLASS(SoShapeStyleElement, inherited);
}

/*!
  The destructor.
*/

SoShapeStyleElement::~SoShapeStyleElement()
{
}

//! FIXME: write doc.

void
SoShapeStyleElement::init(SoState * state)
{
  inherited::init(state);
  this->flags = FLAG_LIGHTING;
}

//! FIXME: write doc.

void
SoShapeStyleElement::push(SoState * state)
{
  inherited::push(state);
  SoShapeStyleElement * prev = (SoShapeStyleElement *) this->getNextInStack();
  this->flags = prev->flags;
}

//! FIXME: write doc.

void
SoShapeStyleElement::pop(SoState * state, const SoElement * prevTopElement)
{
  inherited::pop(state, prevTopElement);
}

//! FIXME: write doc.

SbBool
SoShapeStyleElement::matches(const SoElement * element) const
{
  const SoShapeStyleElement * elem =
    (const SoShapeStyleElement*) element;
  return this->flags == elem->flags;
}

//! FIXME: write doc.

SoElement *
SoShapeStyleElement::copyMatchInfo(void) const
{
  SoShapeStyleElement * elem =
    (SoShapeStyleElement*) this->getTypeId().createInstance();
  elem->flags = this->flags;
  return elem;
}

//! FIXME: write doc.

const SoShapeStyleElement *
SoShapeStyleElement::get(SoState * const state)
{
  return (const SoShapeStyleElement *)
    SoElement::getConstElement(state, classStackIndex);
}

//! FIXME: write doc.

void
SoShapeStyleElement::setDrawStyle(SoState * const state,
                                  const int32_t value)
{
  SoShapeStyleElement * elem = getElement(state);
  if (value == (int32_t)SoDrawStyleElement::INVISIBLE) {
    elem->flags |= FLAG_INVISIBLE;
  }
  else {
    elem->flags &= ~FLAG_INVISIBLE;
  }
}

//! FIXME: write doc.

void
SoShapeStyleElement::setComplexityType(SoState * const state,
                                       const int32_t value)
{
  SoShapeStyleElement * elem = getElement(state);
  if (value == (int32_t) SoComplexityTypeElement::BOUNDING_BOX) {
    elem->flags |= FLAG_BBOXCMPLX;
  }
  else {
    elem->flags &= ~FLAG_BBOXCMPLX;
  }
}

//! FIXME: write doc.

void
SoShapeStyleElement::setTransparencyType(SoState * const state,
                                         const int32_t value)
{
  SoShapeStyleElement * elem = getElement(state);
  if (value == (int32_t) SoGLRenderAction::SCREEN_DOOR) {
    elem->flags |= FLAG_SCREENDOOR;
  }
  else {
    elem->flags &= ~FLAG_SCREENDOOR;
  }
}

//! FIXME: write doc.

void
SoShapeStyleElement::setTextureEnabled(SoState * const state,
                                       const SbBool value)
{
  SoShapeStyleElement * elem = getElement(state);
  if (value) {
    elem->flags |= FLAG_TEXENABLED;
  }
  else {
    elem->flags &= ~FLAG_TEXENABLED;
  }
}

//! FIXME: write doc.

void
SoShapeStyleElement::setTextureFunction(SoState * const state,
                                        const SbBool value)
{
  SoShapeStyleElement * elem = getElement(state);
  if (value) {
    elem->flags |= FLAG_TEXFUNC;
  }
  else {
    elem->flags &= ~FLAG_TEXFUNC;
  }
}

//! FIXME: write doc.

void
SoShapeStyleElement::setLightModel(SoState * const state,
                                   const int32_t value)
{
  SoShapeStyleElement * elem = getElement(state);

  if (value != (int32_t) SoLightModelElement::BASE_COLOR) {
    elem->flags |= FLAG_LIGHTING;
  }
  else {
    elem->flags &= ~FLAG_LIGHTING;
  }
}

//! FIXME: write doc.

void
SoShapeStyleElement::setOverrides(SoState * const state,
                                  const SbBool value)
{
  SoShapeStyleElement * elem = getElement(state);

  if (value) {
    elem->flags |= FLAG_OVERRIDE;
  }
  else {
    elem->flags &= ~FLAG_OVERRIDE;
  }
}

//! FIXME: write doc.

SbBool
SoShapeStyleElement::isScreenDoor(SoState * const state)
{
  const SoShapeStyleElement * elem = getConstElement(state);
  return (elem->flags & FLAG_SCREENDOOR) != 0;
}

//! FIXME: write doc.

void
SoShapeStyleElement::print(FILE * /* file */) const
{
  COIN_STUB();
}

/*!
  FIXME: write doc.
*/

SbBool
SoShapeStyleElement::mightNotRender() const
{
  return (this->flags & FLAG_DELAYMASK) != 0;
}

/*!
  FIXME: write doc.
*/

SbBool
SoShapeStyleElement::needNormals() const
{
  return (this->flags & FLAG_LIGHTING) != 0;
}

/*!
  FIXME: write doc.
*/

SbBool
SoShapeStyleElement::needTexCoords() const
{
  return (this->flags & FLAG_TEXMASK) == FLAG_TEXENABLED;
}

/*!
  FIXME: write doc.
*/

int
SoShapeStyleElement::getRenderCaseMask() const
{
  COIN_STUB();
  return 0;
}

/*!
  Returns the current modifiable instance (might cause a push())
*/
SoShapeStyleElement *
SoShapeStyleElement::getElement(SoState * const state)
{
  return (SoShapeStyleElement *)
    SoElement::getElement(state, classStackIndex);
}
/*!
  Returns the current read-only instance.
*/
const SoShapeStyleElement *
SoShapeStyleElement::getConstElement(SoState * const state)
{
  return (const SoShapeStyleElement *)
    SoElement::getConstElement(state, classStackIndex);
}

#undef FLAG_LIGHTING
#undef FLAG_TEXENABLED
#undef FLAG_TEXFUNC
#undef FLAG_BBOXCMPLX
#undef FLAG_INVISIBLE
#undef FLAG_ABORTCB
#undef FLAG_SCREENDOOR
#undef FLAG_OVERRIDE
#undef FLAG_TEXMASK
#undef FLAG_DELAYMASK

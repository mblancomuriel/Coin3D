/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2001 by Systems in Motion. All rights reserved.
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
  \class SoMaterial SoMaterial.h Inventor/nodes/SoMaterial.h
  \brief The SoMaterial class is a node type for setting up material values for scene geometry.
  \ingroup nodes

  Subsequent geometry-type nodes in the scene graph will use values
  from the material "pool" of the traversal state.

  Note that values from a material node will \e replace the previous
  values from in the traversal state.

  VRML V1.0 has a special case, where the fields ambientColor,
  diffuseColor and specularColor contains zero values, and
  emissiveColor contains one or more values. The values in emissiveColor
  should then be treated as precalculated lighting, and the other 
  fields should be ignored.

  You can detect this case by checking the values of the material
  elements when the scene graph is traversed using an
  SoCallbackAction. SoDiffuseColorElement, SoAmbientColorElement, and
  SoSpecularColorElement will contain one value with a completely
  black color (0.0f, 0.0f, 0.0f), SoShininessElement will contains one
  value with the value 0.0f, and SoEmissiveColorElement will contain
  one or more values. It is done like this to make rendering work
  correctly on systems that do not test for this specific case.

  You should only check for this case when you're traversing a VRML
  V1.0 file scene graph, of course. See SoNode::getNodeType() for
  information about VRML V1.0 versus Inventor.

  When the scene graph is rendered using an SoGLRenderAction,
  the elements will be set differently to optimize rendering.
  The SoDiffuseColorElement will be set to the values in emissiveColor,
  and the light model will be set to BASE_COLOR. 

  The transparency values will always be treated normally.

  \sa SoMaterialBinding, SoPackedColor
*/

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSubNodeP.h>

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoGLAmbientColorElement.h>
#include <Inventor/elements/SoGLDiffuseColorElement.h>
#include <Inventor/elements/SoGLEmissiveColorElement.h>
#include <Inventor/elements/SoGLPolygonStippleElement.h>
#include <Inventor/elements/SoGLShininessElement.h>
#include <Inventor/elements/SoGLSpecularColorElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/elements/SoTransparencyElement.h>
#include <Inventor/elements/SoShapeStyleElement.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <stdlib.h>

/*!
  \var SoMFColor SoMaterial::ambientColor

  Ambient material part color values.

  The ambient part of the material is not influenced by the lights in
  the scene, but which is always added to the geometry surfaces.
*/
/*!
  \var SoMFColor SoMaterial::diffuseColor

  Diffuse material part color values.

  The diffuse part is combined with the light emitted from the scene's
  light sources.
*/
/*!
  \var SoMFColor SoMaterial::specularColor

  Specular material part color values.
*/
/*!
  \var SoMFColor SoMaterial::emissiveColor

  The color of the light "emitted" by the geometry surfaces.
*/
/*!
  \var SoMFFloat SoMaterial::shininess

  Shininess values. Decides how the light from light sources are
  distributed across the geometry surfaces. Valid range is from 0.0
  (which gives a dim appearance), to 1.0 (glossy-looking surfaces).
*/
/*!
  \var SoMFFloat SoMaterial::transparency

  Transparency values. Valid range is from 0.0 (completely opaque,
  which is the default) to 1.0 (completely transparent,
  i.e. invisible).
*/

// defines for materialtype
#define TYPE_UNKNOWN            0
#define TYPE_NORMAL             1
#define TYPE_VRML1_ONLYEMISSIVE 2 // special case in vrml1

static SbColor * one_black_color = NULL;

static void material_cleanup(void)
{
  delete one_black_color;
}


// *************************************************************************

SO_NODE_SOURCE(SoMaterial);

/*!
  Constructor.
*/
SoMaterial::SoMaterial(void)
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoMaterial);

  SO_NODE_ADD_FIELD(ambientColor, (0.2f, 0.2f, 0.2f));
  SO_NODE_ADD_FIELD(diffuseColor, (0.8f, 0.8f, 0.8f));
  SO_NODE_ADD_FIELD(specularColor, (0.0f, 0.0f, 0.0f));
  SO_NODE_ADD_FIELD(emissiveColor, (0.0f, 0.0f, 0.0f));
  SO_NODE_ADD_FIELD(shininess, (0.2f));
  SO_NODE_ADD_FIELD(transparency, (0.0f));
  
  this->materialtype = TYPE_NORMAL;
}

/*!
  Destructor.
*/
SoMaterial::~SoMaterial()
{
}

// Doc from superclass.
void
SoMaterial::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoMaterial);

  SO_ENABLE(SoGLRenderAction, SoGLAmbientColorElement);
  SO_ENABLE(SoGLRenderAction, SoGLDiffuseColorElement);
  SO_ENABLE(SoGLRenderAction, SoGLEmissiveColorElement);
  SO_ENABLE(SoGLRenderAction, SoGLSpecularColorElement);
  SO_ENABLE(SoGLRenderAction, SoGLShininessElement);
  SO_ENABLE(SoGLRenderAction, SoTransparencyElement);
  SO_ENABLE(SoGLRenderAction, SoGLPolygonStippleElement);

  SO_ENABLE(SoPickAction, SoAmbientColorElement);
  SO_ENABLE(SoPickAction, SoDiffuseColorElement);
  SO_ENABLE(SoPickAction, SoEmissiveColorElement);
  SO_ENABLE(SoPickAction, SoSpecularColorElement);
  SO_ENABLE(SoPickAction, SoShininessElement);
  SO_ENABLE(SoPickAction, SoTransparencyElement);

  SO_ENABLE(SoCallbackAction, SoAmbientColorElement);
  SO_ENABLE(SoCallbackAction, SoDiffuseColorElement);
  SO_ENABLE(SoCallbackAction, SoEmissiveColorElement);
  SO_ENABLE(SoCallbackAction, SoSpecularColorElement);
  SO_ENABLE(SoCallbackAction, SoShininessElement);
  SO_ENABLE(SoCallbackAction, SoTransparencyElement);
}

// Doc from superclass.
void
SoMaterial::GLRender(SoGLRenderAction * action)
{
  SoState * state = action->getState();
  if (SoShapeStyleElement::isScreenDoor(state) &&
      ! this->transparency.isIgnored() &&
      ! SoOverrideElement::getTransparencyOverride(state)) {
    float t = this->transparency[0];
    SoGLPolygonStippleElement::setTransparency(state, t);
    SoGLPolygonStippleElement::set(state, t >= 1.0f/255.0f);
  }

  // vrml1 has a special feature when only emissiveColor contains
  // values. The emissiveColor values should then be treated as
  // precalculated lighting. We emulate this by disabling lighting,
  // and setting the diffuseColor element to the emissiveColor values.
  // This is much faster than using the emissive color element.
  //                                             pederb, 2000-02-19
  if (this->getMaterialType() == TYPE_VRML1_ONLYEMISSIVE) {
    uint32_t flags = SoOverrideElement::getFlags(state);
#define TEST_OVERRIDE(bit) ((SoOverrideElement::bit & flags) != 0)
    // disable lighting
    if (!TEST_OVERRIDE(LIGHT_MODEL)) { 
      SoLightModelElement::set(state, this, SoLightModelElement::BASE_COLOR);
      if (this->isOverride()) {
        SoOverrideElement::setLightModelOverride(state, this, TRUE);
      }
    }
    // the transparency element should be treated normally
    if (!this->transparency.isIgnored() && !TEST_OVERRIDE(TRANSPARENCY)) {
      SoTransparencyElement::set(action->getState(),
                                 this,
                                 this->transparency.getNum(),
                                 this->transparency.getValues(0));
      if (this->isOverride()) {
        SoOverrideElement::setTransparencyOverride(state, this, TRUE);
      }
    }
    if (!TEST_OVERRIDE(DIFFUSE_COLOR)) {
      // set diffuse element to the emissive values for fast rendering.
      SoDiffuseColorElement::set(state, this, this->emissiveColor.getNum(),
                                 this->emissiveColor.getValues(0));
      if (this->isOverride()) {
        SoOverrideElement::setDiffuseColorOverride(state, this, TRUE);
      }
    }
#undef TEST_OVERRIDE
  }
  else {
    // set material elements the normal way
    SoMaterial::doAction(action);
  }
}

// Doc from superclass.
void
SoMaterial::doAction(SoAction * action)
{
  SoState * state = action->getState();

  uint32_t flags = SoOverrideElement::getFlags(state);
#define TEST_OVERRIDE(bit) ((SoOverrideElement::bit & flags) != 0)

  if (!this->ambientColor.isIgnored() && this->ambientColor.getNum() &&
      !TEST_OVERRIDE(AMBIENT_COLOR)) {
    SoAmbientColorElement::set(state,
                               this,
                               this->ambientColor.getNum(),
                               this->ambientColor.getValues(0));
    if (this->isOverride()) {
      SoOverrideElement::setAmbientColorOverride(state, this, TRUE);
    }
  }
  if (!this->diffuseColor.isIgnored() && this->diffuseColor.getNum() &&
      !TEST_OVERRIDE(DIFFUSE_COLOR)) {
    // Note: the override flag bit values for diffuseColor and
    // transparency are equal (done like that to match SGI/TGS
    // Inventor behavior), so overriding one will also override the
    // other.
    SoDiffuseColorElement::set(action->getState(),
                               this,
                               this->diffuseColor.getNum(),
                               this->diffuseColor.getValues(0));
    if (this->isOverride()) {
      SoOverrideElement::setDiffuseColorOverride(state, this, TRUE);
    }
  }
  if (!this->emissiveColor.isIgnored() && this->emissiveColor.getNum() &&
      !TEST_OVERRIDE(EMISSIVE_COLOR)) {
    SoEmissiveColorElement::set(action->getState(),
                                this,
                                this->emissiveColor.getNum(),
                                this->emissiveColor.getValues(0));
    if (this->isOverride()) {
      SoOverrideElement::setEmissiveColorOverride(state, this, TRUE);
    }

  }
  if (!this->specularColor.isIgnored() && this->specularColor.getNum() &&
      !TEST_OVERRIDE(SPECULAR_COLOR)) {
    SoSpecularColorElement::set(action->getState(),
                                this,
                                this->specularColor.getNum(),
                                this->specularColor.getValues(0));
    if (this->isOverride()) {
      SoOverrideElement::setSpecularColorOverride(state, this, TRUE);
    }
  }
  if (!this->shininess.isIgnored() && this->shininess.getNum() &&
      !TEST_OVERRIDE(SHININESS)) {
    SoShininessElement::set(action->getState(),
                            this,
                            this->shininess.getNum(),
                            this->shininess.getValues(0));
    if (this->isOverride()) {
      SoOverrideElement::setShininessOverride(state, this, TRUE);
    }
  }
  if (!this->transparency.isIgnored() && this->transparency.getNum() &&
      !TEST_OVERRIDE(TRANSPARENCY)) {
    // Note: the override flag bit values for diffuseColor and
    // transparency are equal (done like that to match SGI/TGS
    // Inventor behavior), so overriding one will also override the
    // other.
    SoTransparencyElement::set(action->getState(),
                               this,
                               this->transparency.getNum(),
                               this->transparency.getValues(0));
    if (this->isOverride()) {
      SoOverrideElement::setTransparencyOverride(state, this, TRUE);
    }
  }
#undef TEST_OVERRIDE
}

// Doc from superclass.
void
SoMaterial::callback(SoCallbackAction * action)
{
  // If file is a vrml1 file, and only the emissiveColor field
  // contains values, set ambient, diffuse and specular to (0,0,0) to
  // enable users to detect this special case. If somebody use their
  // own rendering code, based on SoCallbackAction, the geometry will
  // be rendered correctly, since only the emissiveColor will
  // contribute to the final calculated color. The GLRender() method
  // has a special (faster) way of dealing with this special case.
  //
  if (this->getMaterialType() == TYPE_VRML1_ONLYEMISSIVE) {
    SoState * state = action->getState();
    if (one_black_color == NULL) {
      one_black_color = new SbColor(0.0f, 0.0f, 0.0f);
      atexit(material_cleanup);
    }
    static float one_shininess[1] = {0.0f};

    uint32_t flags = SoOverrideElement::getFlags(state);
#define TEST_OVERRIDE(bit) ((SoOverrideElement::bit & flags) != 0)
    
    if (!TEST_OVERRIDE(AMBIENT_COLOR)) {
      SoAmbientColorElement::set(state,
                                 this,
                                 1, one_black_color);
      if (this->isOverride()) {
        SoOverrideElement::setAmbientColorOverride(state, this, TRUE);
      }
    }
    if (!TEST_OVERRIDE(DIFFUSE_COLOR)) {
      SoDiffuseColorElement::set(action->getState(),
                                 this,
                                 1, one_black_color);
    if (this->isOverride()) {
      SoOverrideElement::setDiffuseColorOverride(state, this, TRUE);
    }
    }
    if (!TEST_OVERRIDE(EMISSIVE_COLOR)) {
      SoEmissiveColorElement::set(action->getState(),
                                  this,
                                  this->emissiveColor.getNum(),
                                  this->emissiveColor.getValues(0));
      if (this->isOverride()) {
        SoOverrideElement::setEmissiveColorOverride(state, this, TRUE);
      } 
    }
    if (!TEST_OVERRIDE(SPECULAR_COLOR)) {
      SoSpecularColorElement::set(action->getState(),
                                  this,
                                  1, one_black_color);
      if (this->isOverride()) {
        SoOverrideElement::setSpecularColorOverride(state, this, TRUE);
      }
    }
    if (!TEST_OVERRIDE(SHININESS)) {
      SoShininessElement::set(action->getState(),
                              this,
                              1, one_shininess);
      if (this->isOverride()) {
        SoOverrideElement::setShininessOverride(state, this, TRUE);
      }
    }
    if (!this->transparency.isIgnored() && this->transparency.getNum() &&
        !TEST_OVERRIDE(TRANSPARENCY)) {
      SoTransparencyElement::set(action->getState(),
                                 this,
                                 this->transparency.getNum(),
                                 this->transparency.getValues(0));
      if (this->isOverride()) {
        SoOverrideElement::setTransparencyOverride(state, this, TRUE);
      }
    }
#undef TEST_OVERRIDE
  }
  else {
    // set material elements the normal way
    SoMaterial::doAction(action);
  }
}

void
SoMaterial::notify(SoNotList *list)
{
  SoField * f = list->getLastField();
  if (f) this->materialtype = TYPE_UNKNOWN;
  inherited::notify(list);
}

//
// used to test for special vrml1 case
//
int 
SoMaterial::getMaterialType(void)
{
  if (this->getNodeType() != SoNode::VRML1) return TYPE_NORMAL;
  else {
    if (this->materialtype == TYPE_UNKNOWN) {
      if (!this->diffuseColor.isIgnored() && this->diffuseColor.getNum() == 0 &&
          !this->ambientColor.isIgnored() && this->ambientColor.getNum() == 0 &&
          !this->specularColor.isIgnored() && this->specularColor.getNum() == 0 &&
          !this->emissiveColor.isIgnored() && this->emissiveColor.getNum()) {
        this->materialtype = TYPE_VRML1_ONLYEMISSIVE;
      }
      else {
        this->materialtype = TYPE_NORMAL;
      }
    }
    return this->materialtype;
  }
}

#undef TYPE_UNKNOWN
#undef TYPE_NORMAL
#undef TYPE_ONLYEMISSIVE

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2005 by Systems in Motion.  All rights reserved.
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
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  <URL:http://www.sim.no/>.
 *
\**************************************************************************/

#include <Inventor/misc/SoJavaScriptEngine.h>
#include <Inventor/C/glue/spidermonkey.h>

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFImage.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFRotation.h>

#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoMFTime.h>
#include <Inventor/fields/SoMFColor.h>
#include <Inventor/fields/SoMFNode.h>
#include <Inventor/fields/SoMFVec2f.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/fields/SoMFRotation.h>


#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#include <Inventor/errors/SoDebugError.h>


// FIXME: toString() missing for all classes

// "namespace" for all vrml classes
struct CoinVrmlJs {
  struct ClassDescriptor {
    JSClass cls;
    JSFunctionSpec * functions;
  };

  static ClassDescriptor SFColor;
  static ClassDescriptor SFNode;
  static ClassDescriptor SFRotation;
  static ClassDescriptor SFVec2f;
  static ClassDescriptor SFVec3f;

  static ClassDescriptor MFColor;
  static ClassDescriptor MFFloat;
  static ClassDescriptor MFInt32;
  static ClassDescriptor MFNode;
  static ClassDescriptor MFRotation;
  static ClassDescriptor MFString;
  static ClassDescriptor MFTime;
  static ClassDescriptor MFVec2f;
  static ClassDescriptor MFVec3f;

  static char * SFColorAliases[];
  static char * SFRotationAliases[];
  static float SFdefaultValues[];
  static float SFRotationDefaultValues[];
};

char * CoinVrmlJs::SFColorAliases[] = {"r", "g", "b"};
char * CoinVrmlJs::SFRotationAliases[] = {"x", "y", "z", "angle"};
float CoinVrmlJs::SFdefaultValues[] = {0.0, 0.0, 0.0, 0.0};
float CoinVrmlJs::SFRotationDefaultValues[] = {0.0, 1.0, 0.0, 0.0};

// Macros for instance checking
#define JSVAL_IS_SFVEC2F(cx, jsval) (JSVAL_IS_OBJECT(jsval) && spidermonkey()->JS_InstanceOf(cx, JSVAL_TO_OBJECT(jsval), &CoinVrmlJs::SFVec2f.cls, NULL))
#define JSVAL_IS_SFVEC3F(cx, jsval) (JSVAL_IS_OBJECT(jsval) && spidermonkey()->JS_InstanceOf(cx, JSVAL_TO_OBJECT(jsval), &CoinVrmlJs::SFVec3f.cls, NULL))
#define JSVAL_IS_SFCOLOR(cx, jsval) (JSVAL_IS_OBJECT(jsval) && spidermonkey()->JS_InstanceOf(cx, JSVAL_TO_OBJECT(jsval), &CoinVrmlJs::SFColor.cls, NULL))
#define JSVAL_IS_SFROTATION(cx, jsval) (JSVAL_IS_OBJECT(jsval) && spidermonkey()->JS_InstanceOf(cx, JSVAL_TO_OBJECT(jsval), &CoinVrmlJs::SFRotation.cls, NULL))

// Handlers
#define SFColorHandler CoinVrmlJsSFHandler<SbColor, 3, CoinVrmlJs::SFColorAliases, CoinVrmlJs::SFdefaultValues>
#define SFRotationHandler CoinVrmlJsSFHandler<SbVec4f, 4, CoinVrmlJs::SFRotationAliases, CoinVrmlJs::SFRotationDefaultValues>
#define SFVec2fHandler CoinVrmlJsSFHandler<SbVec2f, 2, CoinVrmlJs::SFRotationAliases, CoinVrmlJs::SFdefaultValues>
#define SFVec3fHandler CoinVrmlJsSFHandler<SbVec3f, 3, CoinVrmlJs::SFRotationAliases, CoinVrmlJs::SFdefaultValues>

#define MFColorHandler CoinVrmlJsMFHandler<SoMFColor, SoSFColor, &CoinVrmlJs::MFColor>
#define MFFloatHandler CoinVrmlJsMFHandler<SoMFFloat, SoSFFloat, &CoinVrmlJs::MFFloat>
#define MFInt32Handler CoinVrmlJsMFHandler<SoMFInt32, SoSFInt32, &CoinVrmlJs::MFInt32>
#define MFNodeHandler CoinVrmlJsMFHandler<SoMFNode, SoSFNode, &CoinVrmlJs::MFNode>
#define MFRotationHandler CoinVrmlJsMFHandler<SoMFRotation, SoSFRotation, &CoinVrmlJs::MFRotation>
#define MFStringHandler CoinVrmlJsMFHandler<SoMFString, SoSFString, &CoinVrmlJs::MFString>
#define MFTimeHandler CoinVrmlJsMFHandler<SoMFTime, SoSFTime, &CoinVrmlJs::MFTime>
#define MFVec2fHandler CoinVrmlJsMFHandler<SoMFVec2f, SoSFVec2f, &CoinVrmlJs::MFVec2f>
#define MFVec3fHandler CoinVrmlJsMFHandler<SoMFVec3f, SoSFVec3f, &CoinVrmlJs::MFVec3f>

// Factory methods for converting to javascript objects
static JSObject * SFColorFactory(JSContext * cx, const SbColor & self);
static JSObject * SFNodeFactory(JSContext * cx, SoNode * container);
static JSObject * SFRotationFactory(JSContext * cx, const SbRotation & self);
static JSObject * SFVec2fFactory(JSContext * cx, const SbVec2f & self);
static JSObject * SFVec3fFactory(JSContext * cx, const SbVec3f & self);

// getIndex returns -1 if id is not an alias or in range 0-max
static JSBool getIndex(JSContext * cx, jsval id, char * aliases[], int max)
{
  int index;

  if (JSVAL_IS_INT(id)) {
    index = JSVAL_TO_INT(id);
    if (index < 0 || index >= max) {
      spidermonkey()->JS_ReportError(cx, "index must be between 0 and %d", max);
      return -1;
    }
    return index;
  }
  else {
    JSString * jsstr = spidermonkey()->JS_ValueToString(cx, id);
    const char * str = spidermonkey()->JS_GetStringBytes(jsstr);

    for (index=0; index<max; ++index) {
      if (strcmp(str, aliases[index]) == 0) {
        return index;
      }
    }
    return -1;
  }
}

// *************************************************************************
// handlers

// FIXME: number of aliases must not be lower than max. This may lead to
// unsafe programming. 20050721 erikgors.
template <class Base, int max, char * aliases[], float defaultValues[]>
struct CoinVrmlJsSFHandler {
  static JSBool get(JSContext * cx, JSObject * obj, jsval id, jsval * rval)
  {
    int index = getIndex(cx, id, aliases, max);
    if (index == -1) {
      return JSVAL_FALSE;
    }

    Base * data = (Base *)spidermonkey()->JS_GetPrivate(cx, obj);
    assert(data != NULL);
    float var = (*data)[index];
    assert(spidermonkey()->JS_NewDoubleValue(cx, (double)var, rval));
    return JSVAL_TRUE;
  }

  static JSBool set(JSContext * cx, JSObject * obj, jsval id, jsval * val)
  {
    int index = getIndex(cx, id, aliases, max);
    if (index == -1) {
      return JSVAL_FALSE;
    }

    SbColor * data = (SbColor *)spidermonkey()->JS_GetPrivate(cx, obj);
    assert(data != NULL);

    // FIXME: number may be NaN, PositiveInfinity and NegativeInfinity.
    // Should be checked for every time we run JS_ValueToNumber.
    // ie: "blipp" will become NaN. 20050720 erikgors.
    double number;
    spidermonkey()->JS_ValueToNumber(cx, *val, &number);
    (*data)[index] = (float)number;
    return JSVAL_TRUE;
  }
  
  static JSBool constructor(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval)
  {
    float vals[max];

    // convert all arguments to numbers or use defaultValues if missing
    uint32_t i;
    for (i=0; i<max; ++i) {
      vals[i] = defaultValues[i];
      if (i<argc) {
        double val;
        if (spidermonkey()->JS_ValueToNumber(cx, argv[i], &val)) {
          vals[i] = (float)val;
        }
        else {
          spidermonkey()->JS_ReportError(cx, "WARNING: failed converting argument %d "
                                             "to a double", i + 1);
        }
      }
    }

    Base * data = new Base(vals);
    spidermonkey()->JS_SetPrivate(cx, obj, data);
    *rval = OBJECT_TO_JSVAL(obj);
    return JSVAL_TRUE;
  }
  static void destructor(JSContext * cx, JSObject * obj)
  {
    Base * data = (Base *)spidermonkey()->JS_GetPrivate(cx, obj);
    // FIXME: We cannot assume this since the class object itself is an
    // instance of this JSClass. kintel 20050804.
    //    assert(data != NULL);
    delete data;
  }
};

template <class MFFieldClass, class SFFieldClass, CoinVrmlJs::ClassDescriptor * desc>
struct CoinVrmlJsMFHandler {
  static JSBool constructor(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval)
  {
    JSObject * array = spidermonkey()->JS_NewArrayObject(cx, 0, NULL);
    // add gc protection
    assert(spidermonkey()->JS_AddRoot(cx, array));
    spidermonkey()->JS_SetPrivate(cx, obj, array);

    SFFieldClass * field = (SFFieldClass *)SFFieldClass::createInstance();
    uintN i;

    for (i=0; i<argc; ++i) {
      if (SoJavaScriptEngine::jsval2field(cx, argv[i], field)) {
        assert(spidermonkey()->JS_SetElement(cx, array, i, &argv[i]));
      }
      else {
        // FIXME: should we insert a default value? 20050727 erikgors.
        spidermonkey()->JS_ReportError(cx, "argv %d is of wrong type", i);
      }
    }
    delete field;
    return JSVAL_TRUE;
  }

  static void destructor(JSContext * cx, JSObject * obj)
  {
    JSObject * array = spidermonkey()->JS_GetPrivate(cx, obj);
    // FIXME: We cannot assume this since the class object itself is an
    // instance of this JSClass. kintel 20050804.
    //    assert(array != NULL);
    assert(spidermonkey()->JS_RemoveRoot(cx, array));
  }

  static JSObject * init(JSContext * cx, JSObject * obj)
  {   
    return spidermonkey()->JS_InitClass(cx, obj, NULL, &desc->cls,
                                        constructor, 0,
                                        NULL, MFFunctions, NULL, NULL);
  }

  static void resize(JSContext * cx, JSObject * array, uint32_t newLength)
  {
    uint32_t length;
    assert(spidermonkey()->JS_GetArrayLength(cx, array, &length));

    if (length > newLength) {
      spidermonkey()->JS_SetArrayLength(cx, array, newLength);
    }
    else {
      SoType type = MFFieldClass::getClassTypeId();

      // expand and fill with new objects
      for (; length<newLength; ++length) {
        jsval val;

        if (type == SoMFInt32::getClassTypeId() ||
            type == SoMFFloat::getClassTypeId()) {
          val = INT_TO_JSVAL(0);
        }
        else if (type == SoMFString::getClassTypeId()) {
          JSString * str = spidermonkey()->JS_NewStringCopyZ(cx, "");
          val = STRING_TO_JSVAL(str);
        }
        else if (type == SoMFNode::getClassTypeId()) {
          // All elements not explicitly initialized are set to NULL
          val = JSVAL_VOID;
        }
        else if (type == SoMFColor::getClassTypeId()) {
          JSObject * newObj =
            spidermonkey()->JS_NewObject(cx, &CoinVrmlJs::SFColor.cls, NULL, NULL);
          assert(newObj != NULL);
          SFColorHandler::constructor(cx, newObj, 0, NULL, &val);
          val = OBJECT_TO_JSVAL(newObj);
        }
        else if (type == SoMFRotation::getClassTypeId()) {
          JSObject * newObj =
            spidermonkey()->JS_NewObject(cx, &CoinVrmlJs::SFRotation.cls, NULL, NULL);
          assert(newObj != NULL);
          SFRotationConstructor(cx, newObj, 0, NULL, &val);
          val = OBJECT_TO_JSVAL(newObj);
        }
        else if (type == SoMFVec2f::getClassTypeId()) {
          JSObject * newObj =
            spidermonkey()->JS_NewObject(cx, &CoinVrmlJs::SFVec2f.cls, NULL, NULL);
          assert(newObj != NULL);
          SFVec2fHandler::constructor(cx, newObj, 0, NULL, &val);
          val = OBJECT_TO_JSVAL(newObj);
        }
        else if (type == SoMFVec3f::getClassTypeId()) {
          JSObject * newObj =
            spidermonkey()->JS_NewObject(cx, &CoinVrmlJs::SFVec3f.cls, NULL, NULL);
          assert(newObj != NULL);
          SFVec3fHandler::constructor(cx, newObj, 0, NULL, &val);
          val = OBJECT_TO_JSVAL(newObj);
        }
        else {
          assert(0 && "this should not happen");
        }

        assert(spidermonkey()->JS_SetElement(cx, array, length, &val));
      }
    }
  }

  static JSBool get(JSContext * cx, JSObject * obj, jsval id, jsval * rval)
  {
    
    JSObject * array = (JSObject *)spidermonkey()->JS_GetPrivate(cx, obj);

    if (JSVAL_IS_INT(id)) {
      assert(array != NULL);
      int index = JSVAL_TO_INT(id);
      return spidermonkey()->JS_GetElement(cx, array, index, rval);
    }
    else if (JSVAL_IS_STRING(id)) {
      const char * str = spidermonkey()->JS_GetStringBytes(JSVAL_TO_STRING(id));
      if (SbName("length") == str) {
        assert(array != NULL);
        uint32_t length;
        assert(spidermonkey()->JS_GetArrayLength(cx, array, &length));
        *rval = INT_TO_JSVAL(length);
        return JSVAL_TRUE;
      }
    }

    return JSVAL_FALSE;
  }

  static JSBool set(JSContext * cx, JSObject * obj, jsval id, jsval * val)
  {
    JSObject * array = (JSObject *)spidermonkey()->JS_GetPrivate(cx, obj);

    if (JSVAL_IS_INT(id)) {
      int index = JSVAL_TO_INT(id);

      // check for bounds
      if (index < 0) {
        return JSVAL_FALSE;
      }

      // resize if necessary
      uint32_t length;
      assert(spidermonkey()->JS_GetArrayLength(cx, array, &length));
      if (index >= (int)length) {
        resize(cx, array, index+1);
      }

      SFFieldClass * field = (SFFieldClass *)SFFieldClass::createInstance();
      // Check if val is not of wrong type
      if (SoJavaScriptEngine::jsval2field(cx, *val, field)) {
        // assign it
        assert(spidermonkey()->JS_SetElement(cx, array, index, val));
        return JSVAL_TRUE;
      }
      delete field;
    }
    else if (JSVAL_IS_STRING(id)) {
      const char * str = spidermonkey()->JS_GetStringBytes(JSVAL_TO_STRING(id));
      if (SbName("length") == str) {
        double number;
        spidermonkey()->JS_ValueToNumber(cx, *val, &number);
        if (number < 0) {
          spidermonkey()->JS_ReportError(cx, "RangeError: invalid array length");
        }
        else {
          resize(cx, array, (uint32_t)number);
        }
        return JSVAL_TRUE;
      }
    }

    return JSVAL_FALSE;
  }


  static SbBool jsval2field(JSContext * cx, const jsval v, SoField * f)
  {
    if (JSVAL_IS_OBJECT(v) && 
        spidermonkey()->JS_InstanceOf(cx, JSVAL_TO_OBJECT(v), &desc->cls, NULL)) {
      JSObject * obj = JSVAL_TO_OBJECT(v);
      JSObject * array = (JSObject *)spidermonkey()->JS_GetPrivate(cx, obj);
      assert(array != NULL);

      jsval element;
      uint32_t i;
      uint32_t num;
      JSBool ok = spidermonkey()->JS_GetArrayLength(cx, array, &num);

      SFFieldClass * field = (SFFieldClass *)SFFieldClass::createInstance();
      
      for (i=0; i<num; ++i) {
        ok = spidermonkey()->JS_GetElement(cx, obj, i, &element);
        assert(ok);

        assert(SoJavaScriptEngine::jsval2field(cx, element, field));
        ((MFFieldClass *)f)->set1Value(i, field->getValue());
      }
      delete field;
      return TRUE;
    }
    return FALSE;
  }
    
  static void field2jsval(JSContext * cx, const SoField * f, jsval * v)
  {
    JSObject * obj = spidermonkey()->JS_NewObject(cx, &desc->cls, NULL, NULL);
    spidermonkey()->JS_DefineFunctions(cx, obj, desc->functions);

    int num = ((SoMField *)f)->getNum();
    jsval * vals = new jsval[num];

    MFFieldClass & mf = *(MFFieldClass *)f;

    SFFieldClass * field = (SFFieldClass *)SFFieldClass::createInstance();
    for (int i=0; i<num; ++i) {
      field->setValue(mf[i]);
      assert(SoJavaScriptEngine::field2jsval(cx, field, &vals[i]));
    }

    jsval rval;
    constructor(cx, obj, num, vals, &rval);
    *v = OBJECT_TO_JSVAL(obj);
    delete field;
    delete [] vals;
  }
};

// *************************************************************************
// constructors

static JSBool SFRotationConstructor(JSContext * cx, JSObject * obj,
                                    uintN argc, jsval * argv, jsval * rval)
{
  if (argc == 2) {
    if (JSVAL_IS_SFVEC3F(cx, argv[0])) {
      SbVec3f & vec =
        *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));

      SbVec4f * data = new SbVec4f();
      spidermonkey()->JS_SetPrivate(cx, obj, data);
      *rval = OBJECT_TO_JSVAL(obj);
      // new SFRotation(SFVec3f fromVector, SFVec3f toVector)
      if (JSVAL_IS_SFVEC3F(cx, argv[1])) {
        SbVec3f & vec2 = 
          *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1]));

        SbRotation rot(vec, vec2);
        SbVec3f axis;
        float rad;
        rot.getValue(axis, rad);

        data->setValue(axis[0], axis[1], axis[2], rad);
        return JSVAL_TRUE;
      }
      // new SFRotation(SFVec3f axis, numeric angle)
      else {
        SbVec4f * data = new SbVec4f();
        spidermonkey()->JS_SetPrivate(cx, obj, data);
        *rval = OBJECT_TO_JSVAL(obj);

        double number = 0.0;
        spidermonkey()->JS_ValueToNumber(cx, argv[1], &number);

        data->setValue(vec[0], vec[1], vec[2], (float)number);
        return JSVAL_TRUE;
      }
    }
  }
  // new SFRotation(numeric x, numeric y, numeric z, numeric angle)
  // Missing values default to 0.0, except y, which defaults to 1.0.
  //
  // SbRotation will default to 0.0, 0.0, 1.0, when angle is 0.0
  // So we use SbVec4f to hold values for SFRotation, since we need to support
  // patterns like this:
  // var colors = new MFColor();
  // colors.length = 10;
  // colors[0].x = 1
  // colors[0].y = 0
  // colors[0].z = 0
  // colors[0].angle = 1.8
  // 
  // This will not work when SbRotation holds the values. 20050714 erikgors.

  return SFRotationHandler::constructor(cx, obj, argc, argv, rval);
} 

// *************************************************************************
// functions

static JSBool SFVec2f_add(JSContext * cx, JSObject * obj, uintN argc, 
                           jsval * argv, jsval * rval)
{
  SbVec2f & vec1 = *(SbVec2f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_SFVEC2F(cx, argv[0])) {
    SbVec2f & vec2 = *(SbVec2f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
    SbVec2f result = vec1 + vec2;
    *rval = OBJECT_TO_JSVAL(SFVec2fFactory(cx, result));
    return JSVAL_TRUE;
  }

  return JSVAL_FALSE;
}

static JSBool SFVec3f_add(JSContext * cx, JSObject * obj, uintN argc, 
                           jsval * argv, jsval * rval)
{
  SbVec3f & vec1 = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_SFVEC3F(cx, argv[0])) {
    SbVec3f & vec2 = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
    SbVec3f result = vec1 + vec2;
    *rval = OBJECT_TO_JSVAL(SFVec3fFactory(cx, result));
    return JSVAL_TRUE;
  }

  return JSVAL_FALSE;
}

static JSBool SFVec2f_divide(JSContext * cx, JSObject * obj, uintN argc, 
                                jsval * argv, jsval * rval)
{
  SbVec2f & vec = *(SbVec2f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_NUMBER(argv[0])) {
    double number;
    spidermonkey()->JS_ValueToNumber(cx, argv[0], &number);

    SbVec2f newVec = vec / (float)number;
    *rval = OBJECT_TO_JSVAL(SFVec2fFactory(cx, newVec));
    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSBool SFVec3f_divide(JSContext * cx, JSObject * obj, uintN argc, 
                                jsval * argv, jsval * rval)
{
  SbVec3f & vec = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_NUMBER(argv[0])) {
    double number;
    spidermonkey()->JS_ValueToNumber(cx, argv[0], &number);

    SbVec3f newVec = vec / (float)number;
    *rval = OBJECT_TO_JSVAL(SFVec3fFactory(cx, newVec));
    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSBool SFVec2f_dot(JSContext *cx, JSObject *obj, uintN argc, 
                                  jsval *argv, jsval *rval)
{
  SbVec2f & vec1 = *(SbVec2f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_SFVEC2F(cx, argv[0])) {
    SbVec2f & vec2 = *(SbVec2f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));

    float dot = vec1.dot(vec2);
    JSBool ok = spidermonkey()->JS_NewDoubleValue(cx, dot, rval);
    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSBool SFVec3f_dot(JSContext *cx, JSObject *obj, uintN argc, 
                                  jsval *argv, jsval *rval)
{
  SbVec3f & vec = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_SFVEC3F(cx, argv[0])) {

    SbVec3f & vec2 = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));

    float dot = vec.dot(vec2);
    JSBool ok = spidermonkey()->JS_NewDoubleValue(cx, dot, rval);
    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSBool SFVec2_length(JSContext * cx, JSObject * obj, uintN argc, 
                                  jsval * argv, jsval * rval)
{
  SbVec2f * vec = (SbVec2f *)spidermonkey()->JS_GetPrivate(cx, obj);
  JSBool ok = spidermonkey()->JS_NewDoubleValue(cx, vec->length(), rval);
  return JSVAL_TRUE;
}

static JSBool SFVec3_length(JSContext * cx, JSObject * obj, uintN argc, 
                                  jsval * argv, jsval * rval)
{
  SbVec3f * vec = (SbVec3f *)spidermonkey()->JS_GetPrivate(cx, obj);
  JSBool ok = spidermonkey()->JS_NewDoubleValue(cx, vec->length(), rval);
  return JSVAL_TRUE;
}

static JSBool SFVec2f_multiply(JSContext * cx, JSObject * obj, uintN argc, 
                                    jsval * argv, jsval * rval)
{
  
  SbVec2f & vec = *(SbVec2f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_NUMBER(argv[0])) {
    double number;
    spidermonkey()->JS_ValueToNumber(cx, argv[0], &number);

    SbVec2f newVec = vec * (float)number;

    *rval = OBJECT_TO_JSVAL(SFVec2fFactory(cx, newVec));

    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSBool SFVec3f_multiply(JSContext * cx, JSObject * obj, uintN argc, 
                                    jsval * argv, jsval * rval)
{
  SbVec3f & vec = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_NUMBER(argv[0])) {
    double number;
    spidermonkey()->JS_ValueToNumber(cx, argv[0], &number);

    SbVec3f newVec = vec * (float)number;
    *rval = OBJECT_TO_JSVAL(SFVec3fFactory(cx, newVec));
    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSBool SFVec2f_normalize(JSContext * cx, JSObject * obj, uintN argc, 
                                     jsval * argv, jsval * rval)
{
  SbVec2f vec = *(SbVec2f *)spidermonkey()->JS_GetPrivate(cx, obj);
  vec.normalize();
  *rval = OBJECT_TO_JSVAL(SFVec2fFactory(cx, vec));
  return JSVAL_TRUE;
}

static JSBool SFVec3f_normalize(JSContext * cx, JSObject * obj, uintN argc, 
                                     jsval * argv, jsval * rval)
{
  SbVec3f vec = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, obj);
  vec.normalize();
  *rval = OBJECT_TO_JSVAL(SFVec3fFactory(cx, vec));
  return JSVAL_TRUE;
}

static JSBool SFVec3f_negate(JSContext * cx, JSObject * obj, uintN argc, 
                                     jsval * argv, jsval * rval)
{
  SbVec3f vec = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, obj);
  vec.negate();
  *rval = OBJECT_TO_JSVAL(SFVec3fFactory(cx, vec));
  return JSVAL_TRUE;
}

static JSBool SFVec2f_substract(JSContext * cx, JSObject * obj, uintN argc, 
                                 jsval * argv, jsval * rval)
{
  SbVec2f & vec1 = *(SbVec2f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_SFVEC2F(cx, argv[0])) {
    SbVec2f & vec2 = *(SbVec2f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
    SbVec2f result = vec1 - vec2;
    *rval = OBJECT_TO_JSVAL(SFVec2fFactory(cx, result));
    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSBool SFVec3f_substract(JSContext * cx, JSObject * obj, uintN argc, 
                                   jsval * argv, jsval * rval)
{
  SbVec3f & vec1 = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_SFVEC3F(cx, argv[0])) {
    SbVec3f & vec2 = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
    SbVec3f result = vec1 - vec2;
    *rval = OBJECT_TO_JSVAL(SFVec3fFactory(cx, result));
    return JSVAL_TRUE;
  }

  return JSVAL_FALSE;
}

static JSBool SFColor_setHSV(JSContext * cx, JSObject * obj, uintN argc, 
                                   jsval * argv, jsval * rval)
{
  if (argc != 3) {
    return JSVAL_FALSE;
  }
  SbColor & color = *(SbColor *)spidermonkey()->JS_GetPrivate(cx, obj);

  float vals[3];
  int i;

  for (i=0; i<3; ++i) {
    double number;
    spidermonkey()->JS_ValueToNumber(cx, argv[i], &number);
    vals[i] = (float)number;
  }

  color.setHSVValue(vals);
  
  *rval = JSVAL_VOID;
  return JSVAL_TRUE;
}

static JSBool SFColor_getHSV(JSContext * cx, JSObject * obj, uintN argc, 
                                jsval * argv, jsval * rval)
{
  SbColor & color = *(SbColor *)spidermonkey()->JS_GetPrivate(cx, obj);

  float vals[3];
  color.getHSVValue(vals);

  jsval vector[3];
  for (int i=0; i<3; ++i) {
    spidermonkey()->JS_NewDoubleValue(cx, vals[i], &vector[i]);
  }

  *rval = OBJECT_TO_JSVAL(spidermonkey()->JS_NewArrayObject(cx, 3, vector));

  return JSVAL_TRUE;
}

static JSBool SFRotation_getAxis(JSContext * cx, JSObject * obj, uintN argc, 
                                    jsval * argv, jsval * rval)
{
  SbVec4f & rot = *(SbVec4f *)spidermonkey()->JS_GetPrivate(cx, obj);
  SbVec3f axis(rot[0], rot[1], rot[2]);
  *rval = OBJECT_TO_JSVAL(SFVec3fFactory(cx, axis));
  return JSVAL_TRUE;
}

static JSBool SFRotation_inverse(JSContext * cx, JSObject * obj, uintN argc, 
                                    jsval * argv, jsval * rval)
{
  SbVec4f & rot = *(SbVec4f *)spidermonkey()->JS_GetPrivate(cx, obj);

  SbVec3f axis(rot[0], rot[1], rot[2]);
  SbRotation newRot(axis, rot[3]);
  newRot.invert();

  *rval = OBJECT_TO_JSVAL(SFRotationFactory(cx, newRot));
  return JSVAL_TRUE;
}

static JSBool SFRotation_multiply(JSContext * cx, JSObject * obj, uintN argc, 
                                     jsval * argv, jsval * rval)
{
  SbVec4f & vec1 = *(SbVec4f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_SFROTATION(cx, argv[0])) {
    SbVec4f & vec2 = *(SbVec4f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
    SbVec3f axis1(vec1[0], vec1[1], vec1[2]);
    SbVec3f axis2(vec2[0], vec2[1], vec2[2]);
    SbRotation result = SbRotation(axis1, vec1[3]) * SbRotation(axis2, vec2[3]);

    *rval = OBJECT_TO_JSVAL(SFRotationFactory(cx, result));
    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSBool SFRotation_multVec(JSContext * cx, JSObject * obj, uintN argc, 
                                    jsval * argv, jsval * rval)
{
  SbVec4f & vec = *(SbVec4f *)spidermonkey()->JS_GetPrivate(cx, obj);
  SbVec3f axis(vec[0], vec[1], vec[2]);
  SbRotation rot(axis, vec[3]);

  if (argc >= 1 && JSVAL_IS_SFVEC3F(cx, argv[0])) {
    SbVec3f & src = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
    SbVec3f dst;
    rot.multVec(src, dst);

    *rval = OBJECT_TO_JSVAL(SFVec3fFactory(cx, dst));
    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSBool SFRotation_setAxis(JSContext * cx, JSObject * obj, uintN argc, 
                                    jsval * argv, jsval * rval)
{
  SbVec4f & rot = *(SbVec4f *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (argc >= 1 && JSVAL_IS_SFVEC3F(cx, argv[0])) {
    SbVec3f & axis = *(SbVec3f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
    rot[0] = axis[0];
    rot[1] = axis[1];
    rot[2] = axis[2];

    *rval = JSVAL_VOID;
    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSBool SFRotation_slerp(JSContext * cx, JSObject * obj, uintN argc, 
                                  jsval * argv, jsval * rval)
{
  SbVec4f & vec = *(SbVec4f *)spidermonkey()->JS_GetPrivate(cx, obj);
  SbVec3f axis(vec[0], vec[1], vec[2]);
  SbRotation rot(axis, vec[3]);

  if (argc >= 2 && JSVAL_IS_SFROTATION(cx, argv[0]) && JSVAL_IS_NUMBER(argv[1])) {
    SbVec4f & vec2 = *(SbVec4f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
    SbVec3f axis2(vec2[0], vec2[1], vec2[2]);
    SbRotation dest(axis2, vec2[3]);

    double number;
    spidermonkey()->JS_ValueToNumber(cx, argv[1], &number);

    SbRotation result = SbRotation::slerp(rot, dest, (float)number);

    *rval = OBJECT_TO_JSVAL(SFRotationFactory(cx, result));
    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSFunctionSpec SFVec2fFunctions[] = {
  {"add", SFVec2f_add, 1, 0, 0},
  {"divide", SFVec2f_divide, 1, 0, 0},
  {"dot", SFVec2f_dot, 1, 0, 0},
  {"length", SFVec2_length, 0, 0, 0},
  {"multiply", SFVec2f_multiply, 1, 0, 0},
  {"normalize", SFVec2f_normalize, 0, 0, 0},
  {"substract", SFVec2f_substract, 1, 0, 0},
  {NULL, NULL, 0, 0, 0}
};

static JSFunctionSpec SFVec3fFunctions[] = {
  {"add", SFVec3f_add, 1, 0, 0},
  {"divide", SFVec3f_divide, 1, 0, 0},
  {"dot", SFVec3f_dot, 1, 0, 0},
  {"length", SFVec3_length, 0, 0, 0},
  {"multiply", SFVec3f_multiply, 1, 0, 0},
  {"normalize", SFVec3f_normalize, 0, 0, 0},
  {"negate", SFVec3f_negate, 0, 0, 0},
  {"substract", SFVec3f_substract, 1, 0, 0},
  {NULL, NULL, 0, 0, 0}
};

static JSFunctionSpec SFColorFunctions[] = {
  {"setHSV", SFColor_setHSV, 3, 0, 0},
  {"getHSV", SFColor_getHSV, 0, 0, 0},
  {NULL, NULL, 0, 0, 0}
};

static JSFunctionSpec SFRotationFunctions[] = {
  {"getAxis", SFRotation_getAxis, 0, 0, 0},
  {"inverse", SFRotation_inverse, 0, 0, 0},
  {"multiply", SFRotation_multiply, 1, 0, 0},
  {"multVec", SFRotation_multVec, 1, 0, 0},
  {"setAxis", SFRotation_setAxis, 1, 0, 0},
  {"slerp", SFRotation_slerp, 2, 0, 0},
  {NULL, NULL, 0, 0, 0}
};

static JSFunctionSpec MFFunctions[] = {
//  {"toString", MF_toString, 0, 0, 0},
  {NULL, NULL, 0, 0, 0}
};    

// *************************************************************************
// factory

static JSObject * SFColorFactory(JSContext * cx, const SbColor & self)
{
  JSObject * obj =
    spidermonkey()->JS_NewObject(cx, &CoinVrmlJs::SFColor.cls, NULL, NULL);
  spidermonkey()->JS_DefineFunctions(cx, obj, SFColorFunctions);

  SbColor * color = new SbColor(self);
  spidermonkey()->JS_SetPrivate(cx, obj, color);

  return obj;
}

static JSObject * SFRotationFactory(JSContext * cx, const SbRotation & self)
{
  JSObject * obj =
    spidermonkey()->JS_NewObject(cx, &CoinVrmlJs::SFRotation.cls, NULL, NULL);
  spidermonkey()->JS_DefineFunctions(cx, obj, SFRotationFunctions);

  SbVec3f axis;
  float angle;
  self.getValue(axis, angle);
  SbVec4f * data = new SbVec4f(axis[0], axis[1], axis[2], angle);
  spidermonkey()->JS_SetPrivate(cx, obj, data);

  return obj;
}

static JSObject * SFVec2fFactory(JSContext * cx, const SbVec2f & self)
{
  JSObject * obj =
    spidermonkey()->JS_NewObject(cx, &CoinVrmlJs::SFVec2f.cls, NULL, NULL);
  spidermonkey()->JS_DefineFunctions(cx, obj, SFVec2fFunctions);

  SbVec2f * data = new SbVec2f(self);
  spidermonkey()->JS_SetPrivate(cx, obj, data);
  return obj;
}

static JSObject * SFVec3fFactory(JSContext * cx, const SbVec3f & self)
{
  JSObject * obj =
    spidermonkey()->JS_NewObject(cx, &CoinVrmlJs::SFVec3f.cls, NULL, NULL);
  spidermonkey()->JS_DefineFunctions(cx, obj, SFVec3fFunctions);

  SbVec3f * data = new SbVec3f(self);
  spidermonkey()->JS_SetPrivate(cx, obj, data);

  return obj;
}

static JSObject * SFVec2f_init(JSContext * cx, JSObject * obj)
{
  return spidermonkey()->JS_InitClass(cx, obj, NULL, &CoinVrmlJs::SFVec2f.cls, 
                                      SFVec2fHandler::constructor, 0,
                                      NULL, SFVec2fFunctions, NULL, NULL);
}

static JSObject * SFVec3f_init(JSContext * cx, JSObject * obj)
{
  return spidermonkey()->JS_InitClass(cx, obj, NULL, &CoinVrmlJs::SFVec3f.cls, 
                                      SFVec2fHandler::constructor, 0,
                                      NULL, SFVec3fFunctions, NULL, NULL);
}

static JSObject * SFColor_init(JSContext * cx, JSObject * obj)
{
  return spidermonkey()->JS_InitClass(cx, obj, NULL, &CoinVrmlJs::SFColor.cls, 
                                      SFColorHandler::constructor, 0,
                                      NULL, SFColorFunctions, NULL, NULL);
}

static JSObject * SFRotation_init(JSContext * cx, JSObject * obj)
{
  return spidermonkey()->JS_InitClass(cx, obj, NULL, &CoinVrmlJs::SFRotation.cls, 
                                      SFRotationConstructor, 0,
                                      NULL, SFRotationFunctions, NULL, NULL);
}

// *************************************************************************
// SFNode

static JSBool SFNode_get(JSContext * cx, JSObject * obj, jsval id, jsval * rval)
{ 
  SoNode * container = (SoNode *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (container == NULL) {
    // this will only happen when JS_NewObject calls "constructor" 
    // or the node is "undefined"
    
    if (JSVAL_IS_STRING(id)) {
      const char * str = spidermonkey()->JS_GetStringBytes(JSVAL_TO_STRING(id));
      if (SbName("constructor") == str) {
        return JSVAL_FALSE;
      }
    }
    spidermonkey()->JS_ReportError(cx, "node is undefined");
    return JSVAL_FALSE;
  }

  if (JSVAL_IS_STRING(id)) {
    SbString str(spidermonkey()->JS_GetStringBytes(JSVAL_TO_STRING(id)));

    SoField * out = container->getField(SbName(str));

    int len = str.getLength();
    static const char post[] = "_changed";
    static const size_t postLen = sizeof(post) - 1;

    if (out == NULL && len > (int)postLen &&
        str.getSubString(len - postLen) == post) {
      out = container->getField(SbName(str.getSubString(0, len - postLen - 1)));
    }

    if (out != NULL) {
      SoJavaScriptEngine::field2jsval(cx, out, rval);
      return JSVAL_TRUE;
    }
    else {
      spidermonkey()->JS_ReportError(cx, "field %s does not exists", str.getString());
    }
  }
  
  return JSVAL_FALSE;
}

static JSBool SFNode_set(JSContext * cx, JSObject * obj, jsval id, jsval * rval)
{ 
  SoNode * container = (SoNode *)spidermonkey()->JS_GetPrivate(cx, obj);

  if (container == NULL) {
    spidermonkey()->JS_ReportError(cx, "node is undefined");
    return JSVAL_FALSE;
  }

  if (JSVAL_IS_STRING(id)) {
    SbString str(spidermonkey()->JS_GetStringBytes(JSVAL_TO_STRING(id)));

    SoField * in = container->getField(SbName(str));

    int len = str.getLength();
    static const char pre[] = "set_";
    static const size_t preLen = sizeof(pre) - 1;

    if (in == NULL && len > (int)preLen &&
        str.getSubString(0, preLen - 1) == pre) {
      in = container->getField(SbName(str.getSubString(preLen)));
    }

    if (in != NULL) {
      SoJavaScriptEngine::jsval2field(cx, *rval, in);
      if (SoJavaScriptEngine::debug()) {
        SoDebugError::postInfo("SFNode_set", "setting field %s", str.getString());
      }
      return JSVAL_TRUE;
    }
    else {
      spidermonkey()->JS_ReportError(cx, "field %s does not exists", str.getString());
    }
  }
  
  return JSVAL_FALSE;
}

static void SFNodeDestructor(JSContext * cx, JSObject * obj)
{
  SoNode * container = (SoNode *)spidermonkey()->JS_GetPrivate(cx, obj);
  // FIXME: We cannot assume this since the class object itself is an
  // instance of this JSClass. kintel 20050804.
  //  assert(container != NULL);
  if (container) container->unref();
}

static JSObject * SFNodeFactory(JSContext * cx, SoNode * container)
{
  JSObject * obj = spidermonkey()->JS_NewObject(cx, &CoinVrmlJs::SFNode.cls, NULL, NULL);
  spidermonkey()->JS_SetPrivate(cx, obj, container);
  container->ref();
  return obj;
}

static JSBool SFNodeConstructor(JSContext * cx, JSObject * obj, 
                                    uintN argc, jsval * argv, jsval *rval)
{
  // spidermonkey ignores the return value

  if (argc >= 1 && JSVAL_IS_STRING(argv[0])) {
    JSString * js = JSVAL_TO_STRING(argv[0]);
    char * str = spidermonkey()->JS_GetStringBytes(js);
    size_t len = spidermonkey()->JS_GetStringLength(js);
    
    // FIXME: what about UTF8? 20050701 erikgors.

    if (SoJavaScriptEngine::debug()) {
      SoDebugError::postInfo("SFNodeConstructor",
                             "creating new node with str = '%s'", str);
    }
                             

    SoInput input = SoInput();
    const char * array[2];
    array[0] = str;
    array[1] = NULL;
    input.setStringArray(array);

    SoVRMLGroup * group = SoDB::readAllVRML(&input);

    if (group == NULL) {
      spidermonkey()->JS_ReportError(cx, "input is not legal VRML string");
      return JSVAL_FALSE;
    }
    if (group->getNumChildren() == 0) {
      spidermonkey()->JS_ReportError(cx, "no top-level node, result is undefined");
      *rval = JSVAL_VOID;
      return JSVAL_FALSE;
    }

    group->ref();
    spidermonkey()->JS_SetPrivate(cx, obj, group);

    return JSVAL_TRUE;
  }
  return JSVAL_FALSE;
}

static JSObject * SFNode_init(JSContext * cx, JSObject * obj)
{
  return spidermonkey()->JS_InitClass(cx, obj, NULL, &CoinVrmlJs::SFNode.cls,
                                      SFNodeConstructor, 0,
                                      NULL, NULL, NULL, NULL);
}


// *************************************************************************
// jsval2field

static SbBool SFBool_jsval2field(JSContext * cx, const jsval v, SoField * f)
{
  if (JSVAL_IS_BOOLEAN(v)) {
    const SbBool b = JSVAL_TO_BOOLEAN(v);
    ((SoSFBool *)f)->setValue(b);
    return TRUE;
  }
  else {
    JSBool b;
    if (spidermonkey()->JS_ValueToBoolean(cx, v, &b)) {
      ((SoSFBool *)f)->setValue(b);
      return TRUE;
    }
  }
  return FALSE;
}

static SbBool SFColor_jsval2field(JSContext * cx, const jsval v, SoField * f)
{
  if (JSVAL_IS_SFCOLOR(cx, v)) {
    SbColor * color = (SbColor *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
    assert(color != NULL);
    ((SoSFColor *)f)->setValue(*color);
    return TRUE;
  }
  return FALSE;
}

static SbBool SFFloat_jsval2field(JSContext * cx, const jsval v, SoField * f)
{
  if (JSVAL_IS_NUMBER(v)) {
    double number;
    spidermonkey()->JS_ValueToNumber(cx, v, &number);
    ((SoSFFloat *)f)->setValue((float)number);
    return TRUE;
  }
  return FALSE;
}

static SbBool SFInt32_jsval2field(JSContext * cx, const jsval v, SoField * f)
{
  if (JSVAL_IS_INT(v)) {
    const int32_t val = JSVAL_TO_INT(v);
    ((SoSFInt32 *)f)->setValue(val);
    return TRUE;
  }
  return FALSE;
}

static SbBool SFNode_jsval2field(JSContext * cx, const jsval v, SoField * f)
{
  if (JSVAL_IS_OBJECT(v) &&
      spidermonkey()->JS_InstanceOf(cx, JSVAL_TO_OBJECT(v), &CoinVrmlJs::SFNode.cls, NULL)) {
    SoNode * node = (SoNode *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
    ((SoSFNode *)f)->setValue(node);
    return TRUE;
  }
  return FALSE;
}

static SbBool SFRotation_jsval2field(JSContext * cx, const jsval v, SoField * f)
{
  if (JSVAL_IS_SFROTATION(cx, v)) {
    SbVec4f * rot = (SbVec4f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
    assert(rot != NULL);
    SbVec3f axis((*rot)[0], (*rot)[1], (*rot)[2]);
    ((SoSFRotation *)f)->setValue(SbRotation(axis, (*rot)[3]));
    return TRUE;
  }
  return FALSE;
}

static SbBool SFString_jsval2field(JSContext * cx, const jsval v, SoField * f)
{
  if (JSVAL_IS_STRING(v)) {
    const char * str = spidermonkey()->JS_GetStringBytes(JSVAL_TO_STRING(v));
    ((SoSFString *)f)->setValue(str);
    return TRUE;
  }
  return FALSE;
}

static SbBool SFTime_jsval2field(JSContext * cx, const jsval v, SoField * f)
{
  if (JSVAL_IS_NUMBER(v)) {
    double number;
    spidermonkey()->JS_ValueToNumber(cx, v, &number);
    ((SoSFTime*)f)->setValue(SbTime(number));
    return TRUE;
  }
  return FALSE;
}

static SbBool SFVec2f_jsval2field(JSContext * cx, const jsval v, SoField * f)
{
  if (JSVAL_IS_SFVEC2F(cx, v)) {
    SbVec2f * vec = (SbVec2f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
    assert(vec != NULL);
    ((SoSFVec2f *)f)->setValue(*vec);
    return TRUE;
  }
  return FALSE;
}

static SbBool SFVec3f_jsval2field(JSContext * cx, const jsval v, SoField * f)
{
  if (JSVAL_IS_SFVEC3F(cx, v)) {
    SbVec3f * vec = (SbVec3f *)spidermonkey()->JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
    assert(vec != NULL);
    ((SoSFVec3f *)f)->setValue(*vec);
    return TRUE;
  }
  return FALSE;
}

// *************************************************************************
// field2jsval

static void SFBool_field2jsval(JSContext * cx, const SoField * f, jsval * v)
{
  const SbBool val = ((SoSFBool *)f)->getValue();
  *v = BOOLEAN_TO_JSVAL(val);
}

static void SFColor_field2jsval(JSContext * cx, const SoField * f, jsval *v)
{
  const SbColor & val = ((SoSFColor *)f)->getValue();
  JSObject * obj = SFColorFactory(cx, val); 
  *v = OBJECT_TO_JSVAL(obj);
}

static void SFFloat_field2jsval(JSContext * cx, const SoField * f, jsval * v)
{
  const float val = ((SoSFFloat *)f)->getValue();
  JSBool ok = spidermonkey()->JS_NewDoubleValue(cx, val, v);
}

static void SFInt32_field2jsval(JSContext * cx, const SoField * f, jsval * v)
{
  const int32_t val = ((SoSFInt32 *)f)->getValue();
  *v = INT_TO_JSVAL(val);
}

static void SFNode_field2jsval(JSContext * cx, const SoField * f, jsval * v)
{
  SoNode * node = ((SoSFNode *)f)->getValue();
  JSObject * obj = SFNodeFactory(cx, node);
  *v = OBJECT_TO_JSVAL(obj);
}

static void SFRotation_field2jsval(JSContext * cx, const SoField * f, jsval *v)
{
  const SbRotation & val = ((SoSFRotation *)f)->getValue();
  JSObject * obj = SFRotationFactory(cx, val); 
  *v = OBJECT_TO_JSVAL(obj);
}

static void SFString_field2jsval(JSContext * cx, const SoField * f, jsval * v)
{
  const SbString & val = ((SoSFString *)f)->getValue();
  JSString * str = spidermonkey()->JS_NewStringCopyZ(cx, val.getString());
  *v = STRING_TO_JSVAL(str);
}

static void SFTime_field2jsval(JSContext * cx, const SoField * f, jsval * v)
{
  const SbTime & time = ((SoSFTime *)f)->getValue();
  JSBool ok = spidermonkey()->JS_NewDoubleValue(cx, time.getValue(), v);
}

static void SFVec2f_field2jsval(JSContext * cx, const SoField * f, jsval *v)
{
  const SbVec2f & val = ((SoSFVec2f *)f)->getValue();
  JSObject * obj = SFVec2fFactory(cx, val); 
  *v = OBJECT_TO_JSVAL(obj);
}

static void SFVec3f_field2jsval(JSContext * cx, const SoField * f, jsval *v)
{
  const SbVec3f & val = ((SoSFVec3f *)f)->getValue();
  JSObject * obj = SFVec3fFactory(cx, val); 
  *v = OBJECT_TO_JSVAL(obj);
}

// *************************************************************************
// classes

CoinVrmlJs::ClassDescriptor CoinVrmlJs::SFColor = {
  {
    "SFColor", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    SFColorHandler::get,
    SFColorHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    SFColorHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0 
  },
  SFColorFunctions
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::SFNode = {
  {
    "SFNode", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    SFNode_get,
    SFNode_set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    SFNodeDestructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0
  },
  NULL
};


CoinVrmlJs::ClassDescriptor CoinVrmlJs::SFRotation = {
  {
    "SFRotation", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    SFRotationHandler::get,
    SFRotationHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    SFRotationHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0 
  },
  SFRotationFunctions
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::SFVec2f = {
  {
    "SFVec2f", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    SFVec2fHandler::get,
    SFVec2fHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    SFVec2fHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0 
  },
  SFVec2fFunctions
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::SFVec3f = {
  {
    "SFVec3f", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    SFVec3fHandler::get,
    SFVec3fHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    SFVec3fHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0 
  },
  SFVec3fFunctions
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::MFColor = {
  {
    "MFColor", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    MFColorHandler::get,
    MFColorHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    MFColorHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0
  },
  MFFunctions,
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::MFFloat = {
  {
    "MFFloat", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    MFFloatHandler::get,
    MFFloatHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    MFFloatHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0
  },
  MFFunctions,
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::MFInt32 = {
  {
    "MFInt32", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    MFInt32Handler::get,
    MFInt32Handler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    MFInt32Handler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0
  },
  MFFunctions,
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::MFNode = {
  {
    "MFNode", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    MFNodeHandler::get,
    MFNodeHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    MFNodeHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0
  },
  MFFunctions,
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::MFRotation = {
  {
    "MFRotation", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    MFRotationHandler::get,
    MFRotationHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    MFRotationHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0
  },
  MFFunctions,
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::MFString = {
  {
    "MFString", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    MFStringHandler::get,
    MFStringHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    MFStringHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0
  },
  MFFunctions,
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::MFTime = {
  {
    "MFTime", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    MFTimeHandler::get,
    MFTimeHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    MFTimeHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0
  },
  MFFunctions,
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::MFVec2f = {
  {
    "MFVec2f", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    MFVec2fHandler::get,
    MFVec2fHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    MFVec2fHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0
  },
  MFFunctions,
};

CoinVrmlJs::ClassDescriptor CoinVrmlJs::MFVec3f = {
  {
    "MFVec3f", JSCLASS_HAS_PRIVATE,
    spidermonkey()->JS_PropertyStub,
    spidermonkey()->JS_PropertyStub,
    MFVec3fHandler::get,
    MFVec3fHandler::set,
    spidermonkey()->JS_EnumerateStub,
    spidermonkey()->JS_ResolveStub,
    spidermonkey()->JS_ConvertStub,
    MFVec3fHandler::destructor,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, 0
  },
  MFFunctions,
};


// *************************************************************************
// helper function to add all classes to engine

static void JS_addVRMLclasses(SoJavaScriptEngine * engine)
{
  // Bool
  engine->addHandler(
    SoSFBool::getClassTypeId(), NULL,
    SFBool_field2jsval, SFBool_jsval2field);

  // Color
  engine->addHandler(
    SoSFColor::getClassTypeId(), SFColor_init,
    SFColor_field2jsval, SFColor_jsval2field);
  engine->addHandler(
    SoMFColor::getClassTypeId(), 
    MFColorHandler::init,
    MFColorHandler::field2jsval,
    MFColorHandler::jsval2field);

  // Float
  engine->addHandler(
    SoSFFloat::getClassTypeId(), NULL,
    SFFloat_field2jsval, SFFloat_jsval2field);
  engine->addHandler(
    SoMFFloat::getClassTypeId(),
    MFFloatHandler::init,
    MFFloatHandler::field2jsval,
    MFFloatHandler::jsval2field);

  // Int32
  engine->addHandler(
    SoSFInt32::getClassTypeId(), NULL,
    SFInt32_field2jsval, SFInt32_jsval2field);
  engine->addHandler(
    SoMFInt32::getClassTypeId(),
    MFInt32Handler::init,
    MFInt32Handler::field2jsval,
    MFInt32Handler::jsval2field);

  // Node
  engine->addHandler(
    SoSFNode::getClassTypeId(), SFNode_init,
    SFNode_field2jsval, SFNode_jsval2field);
  engine->addHandler(
    SoMFNode::getClassTypeId(),
    MFNodeHandler::init,
    MFNodeHandler::field2jsval,
    MFNodeHandler::jsval2field);

  // Rotation
  engine->addHandler(
    SoSFRotation::getClassTypeId(), SFRotation_init,
    SFRotation_field2jsval, SFRotation_jsval2field);
  engine->addHandler(
    SoMFRotation::getClassTypeId(),
    MFRotationHandler::init,
    MFRotationHandler::field2jsval,
    MFRotationHandler::jsval2field);

  // String
  engine->addHandler(
    SoSFString::getClassTypeId(), NULL,
    SFString_field2jsval, SFString_jsval2field);
  engine->addHandler(
    SoMFString::getClassTypeId(),
    MFStringHandler::init,
    MFStringHandler::field2jsval,
    MFStringHandler::jsval2field);

  // Time
  engine->addHandler(
    SoSFTime::getClassTypeId(), NULL,
    SFTime_field2jsval, SFTime_jsval2field);
  engine->addHandler(
    SoMFTime::getClassTypeId(),
    MFTimeHandler::init,
    MFTimeHandler::field2jsval,
    MFTimeHandler::jsval2field);

  // Vec2f
  engine->addHandler(
    SoSFVec2f::getClassTypeId(), SFVec2f_init,
    SFVec2f_field2jsval, SFVec2f_jsval2field);
  engine->addHandler(
    SoMFVec2f::getClassTypeId(),
    MFVec2fHandler::init,
    MFVec2fHandler::field2jsval,
    MFVec2fHandler::jsval2field);

  // Vec3f
  engine->addHandler(
    SoSFVec3f::getClassTypeId(), SFVec3f_init,
    SFVec3f_field2jsval, SFVec3f_jsval2field);
  engine->addHandler(
    SoMFVec3f::getClassTypeId(),
    MFVec3fHandler::init,
    MFVec3fHandler::field2jsval,
    MFVec3fHandler::jsval2field);
}

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
  \class SoInteractionKit SoInteractionKit.h Inventor/nodekits/SoInteractionKit.h
  \brief The SoInteractionKit class is the base class for all draggers.
  \ingroup nodekits

  Ihis nodekits makes it possible to set surrogate paths for
  parts. Instead of creating new geometry for the dragger, it is
  possible to specify an existing path in your scene to be used for
  interaction. All picks on this path will be handled by the dragger.
*/

#include <Inventor/nodekits/SoInteractionKit.h>
#include <Inventor/nodekits/SoSubKitP.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/errors/SoReadError.h>
#include <coindefs.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <coindefs.h> // COIN_STUB()
#include <Inventor/lists/SoPathList.h>
#include <Inventor/lists/SbList.h>
#include <Inventor/misc/SoChildList.h>

#include <stdlib.h>

#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG


#ifndef DOXYGEN_SKIP_THIS

class SoInteractionKitP {
public:
  SoInteractionKitP(SoInteractionKit * kit) : kit(kit) { }

  SoInteractionKit * kit;
  SoFieldSensor * fieldsensor;
  SoSeparator * connectedseparator;

  void connectFields(const SbBool onoff);
  void attachSensor(const SbBool onoff);

  static void sensorCB(void *, SoSensor *);

  SoPathList surrogatepathlist;
  SbList <SbName> surrogatenamelist;

  void addSurrogatePath(SoPath * path, const SbName & name);
  void removeSurrogatePath(const SbName & partname);
  void removeSurrogatePath(const int idx);
  int findSurrogateIndex(const SbName & partname) const;
  int findSurrogateInPath(const SoPath * path);
};

#endif // DOXYGEN_SKIP_THIS


static SbList <SoNode*> * defaultdraggerparts = NULL;

//
// atexit callback. To delete default dragger parts files.
//
static void
interactionkit_cleanup(void)
{
  int n = defaultdraggerparts->getLength();
  for (int i = 0; i < n; i++) {
    (*defaultdraggerparts)[i]->unref();
  }
  
  delete defaultdraggerparts;
}

#undef THIS
#define THIS this->pimpl

SO_KIT_SOURCE(SoInteractionKit);


/*!
  Constructor.
*/
SoInteractionKit::SoInteractionKit(void)
{
  THIS = new SoInteractionKitP(this);
  SO_KIT_INTERNAL_CONSTRUCTOR(SoInteractionKit);

  SO_KIT_ADD_FIELD(renderCaching, (SoInteractionKit::AUTO));
  SO_KIT_ADD_FIELD(boundingBoxCaching, (SoInteractionKit::AUTO));
  SO_KIT_ADD_FIELD(renderCulling, (SoInteractionKit::AUTO));
  SO_KIT_ADD_FIELD(pickCulling, (SoInteractionKit::AUTO));

  SO_KIT_DEFINE_ENUM_VALUE(CacheEnabled, ON);
  SO_KIT_DEFINE_ENUM_VALUE(CacheEnabled, OFF);
  SO_KIT_DEFINE_ENUM_VALUE(CacheEnabled, AUTO);

  SO_KIT_SET_SF_ENUM_TYPE(renderCaching, CacheEnabled);
  SO_KIT_SET_SF_ENUM_TYPE(boundingBoxCaching, CacheEnabled);
  SO_KIT_SET_SF_ENUM_TYPE(renderCulling, CacheEnabled);
  SO_KIT_SET_SF_ENUM_TYPE(pickCulling, CacheEnabled);


  // Note: we must use "" instead of , , to humour MS VisualC++ 6.

  SO_KIT_ADD_CATALOG_ENTRY(topSeparator, SoSeparator, TRUE, this, "", FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(geomSeparator, SoSeparator, TRUE, topSeparator, "", FALSE);

  SO_KIT_INIT_INSTANCE();

  THIS->connectedseparator = NULL;
  THIS->fieldsensor = new SoFieldSensor(SoInteractionKitP::sensorCB, THIS);
  THIS->fieldsensor->setPriority(0);

  this->setUpConnections(TRUE, TRUE);
}

/*!
  Destructor.
*/
SoInteractionKit::~SoInteractionKit()
{
  THIS->connectFields(FALSE);
  delete THIS->fieldsensor;
  delete THIS;
}

/*!
  Does initialization common for all objects of the
  SoInteractionKit class. This includes setting up the
  type system, among other things.
*/
void
SoInteractionKit::initClass(void)
{
  defaultdraggerparts = new SbList <SoNode*>;
  atexit(interactionkit_cleanup);

  SO_KIT_INTERNAL_INIT_CLASS(SoInteractionKit);
}

/*!
  Sets a part in the kit as a surrogate path. The \a partname part is
  set to \c NULL, and the surrogate path is remembered. Following
  picks on the surrogate path will be regarded as a pick on \a
  partname.
*/
SbBool
SoInteractionKit::setPartAsPath(const SbName & partname,
                                SoPath * path)
{
  return this->setAnySurrogatePath(partname, path, TRUE, TRUE);
}

/*!
  Sets the value of \a partname to \a node, and sets the part's field
  to default (i.e. node will not be written on scene graph export).

  If \a onlyifdefault is \c TRUE, \a partname is only set if it is
  already in the default state.

  The reason for this method is to make it possible for dragger
  subclasses to avoid having their default parts written out on
  export.
*/
SbBool
SoInteractionKit::setPartAsDefault(const SbName & partname,
                                   SoNode * node,
                                   SbBool onlyifdefault)
{
  return this->setAnyPartAsDefault(partname, node, FALSE, onlyifdefault);
}

/*!
  Find node in the global dictionary, and set as default.

  \sa setPartAsDefault()
*/
SbBool
SoInteractionKit::setPartAsDefault(const SbName & partname,
                                   const SbName & nodename,
                                   SbBool onlyifdefault)
{
  return this->setAnyPartAsDefault(partname, nodename, FALSE, onlyifdefault);
}


/*!
  Checks if \a path is contained within any of the surrogate paths
  in any interaction kits from this node down. Returns information
  about the owner and the surrogate path if found, and \a fillargs is
  \e TRUE. The returned path (\a pathToOwner) is not ref'ed, It's the
  callers responsibility to ref and unref this path.
*/
SbBool
SoInteractionKit::isPathSurrogateInMySubgraph(const SoPath * path,
                                              SoPath *& pathToOwner,
                                              SbName  & surrogatename,
                                              SoPath *& surrogatepath,
                                              SbBool fillargs)
{
  int idx = THIS->findSurrogateInPath(path);
  if (idx >= 0) {
    if (fillargs) {
      pathToOwner = new SoPath(this); // a very short path
      surrogatename = THIS->surrogatenamelist[idx];
      surrogatepath = THIS->surrogatepathlist[idx];
    }
    return TRUE;
  }
  else {
    SoSearchAction sa;
    sa.setType(SoInteractionKit::getClassTypeId());
    sa.setFind(SoSearchAction::ALL);
    sa.setSearchingAll(TRUE);
    sa.apply(this);
    SoPathList & pathlist = sa.getPaths();
    for (int i = 0; i < pathlist.getLength(); i++) {
      SoInteractionKit * kit = (SoInteractionKit *)pathlist[i]->getTail();
      assert(kit->isOfType(SoInteractionKit::getClassTypeId()));
      int idx = kit->pimpl->findSurrogateInPath(path);
      if (idx >= 0) {
        if (fillargs) {
          pathToOwner = pathlist[i]->copy();
          surrogatename = kit->pimpl->surrogatenamelist[idx];
          surrogatepath = kit->pimpl->surrogatepathlist[idx];
        }
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*!
  \overload
*/
SbBool
SoInteractionKit::isPathSurrogateInMySubgraph(const SoPath * path)
{
  SoPath * dummypath, * dummypath2;
  SbName dummyname;

  return this->isPathSurrogateInMySubgraph(path, dummypath,
                                           dummyname, dummypath2, FALSE);
}

/*!
  Convenience method that sets the switch value for a switch node.
  Checks if node != 0, and only sets the switch value if value
  has changed.
*/
void
SoInteractionKit::setSwitchValue(SoNode * node, const int newVal)
{
  if (node == NULL) return;
  assert(node->isOfType(SoSwitch::getClassTypeId()));
  SoSwitch * mySwitch = (SoSwitch *)node;
  if (mySwitch->whichChild.getValue() != newVal) {
    mySwitch->whichChild = newVal;
  }
}

/*!
  Overloaded to copy the surrogate lists.
 */
void
SoInteractionKit::copyContents(const SoFieldContainer * fromFC,
                               SbBool copyConnections)
{
  int i;
  inherited::copyContents(fromFC, copyConnections);

  assert(fromFC->isOfType(SoInteractionKit::getClassTypeId()));
  SoInteractionKit * kit = (SoInteractionKit *) fromFC;
  
  THIS->surrogatenamelist.truncate(0);
  THIS->surrogatepathlist.truncate(0);
  
  const int n = kit->pimpl->surrogatenamelist.getLength();
  for (i = 0; i < n; i++) {
    THIS->surrogatenamelist.append(kit->pimpl->surrogatenamelist[i]);
    THIS->surrogatepathlist.append(kit->pimpl->surrogatepathlist[i]);
  }
}

/*!
  Overloaded to check topSeperator and fields after reading.
*/
SbBool
SoInteractionKit::readInstance(SoInput * in, unsigned short flags)
{
  SbBool ret = inherited::readInstance(in, flags); // will handle fields
  if (ret) {
    // remove surrogate paths where part != NULL and not an empty group or separator
    int n = THIS->surrogatenamelist.getLength();
    for (int i = 0; i < n; i++) {
      SbName name = THIS->surrogatenamelist[i];
      SoNode * node = this->getAnyPart(name, FALSE, FALSE, FALSE);
      if (node && ((node->getTypeId() != SoGroup::getClassTypeId() &&
                    node->getTypeId() != SoSeparator::getClassTypeId()) ||
                   node->getChildren()->getLength())) {
        n--; // don't forget this!
        THIS->surrogatenamelist.remove(i);
        THIS->surrogatepathlist.remove(i);
      }
    }
  }
  return ret;
}

/*!
  Reads default parts for a dragger. \a fileName is the user-changeable
  resource file (most often an ascii iv file), while \a defaultBuffer and
  \a defBufSize can point to the statically compiled default parts.

  This method is called from dragger constructors.
*/
void
SoInteractionKit::readDefaultParts(const char * fileName,
                                   const char defaultBuffer[],
                                   int defBufSize)
{
  SbBool foundsrc = FALSE;
  SoInput input;

  char * draggerdir = getenv("SO_DRAGGER_DIR");

  if (fileName) {
    if (draggerdir) {
      SbString fullname = draggerdir;
      // FIXME: use configure to check this. 20000129 mortene.
#ifdef _WIN32
      char dirsep = '\\';
#else // ! WIN32
      char dirsep = '/';
#endif // !WIN32

      if (fullname.getLength() && fullname[fullname.getLength()-1] != dirsep)
        fullname += dirsep;

      fullname += fileName;
      if (input.openFile(fullname.getString(), TRUE)) foundsrc = TRUE;
    }
    else if (input.openFile(fileName, TRUE)) foundsrc = TRUE;
  }

  if (!foundsrc && defaultBuffer) {
    input.setBuffer((void *)defaultBuffer, defBufSize);
    foundsrc = TRUE;
  }

  if (!foundsrc) {
#if COIN_DEBUG
    SoDebugError::post("SoInteractionKit::readDefaultParts",
                       "Could not find %s for the dragger "
                       "default parts.%s",
                       fileName,
                       draggerdir ? "" :
                       " (SO_DRAGGER_DIR environment variable is not set.)");
    exit(1);
#endif // COIN_DEBUG
    return;
  }

  SoNode * node = (SoNode *)SoDB::readAll(&input);
  if (node == NULL) {
#if COIN_DEBUG
    SoReadError::post(&input, "error reading dragger defaults");
    exit(1);
#endif // COIN_DEBUG
  }
  else {
    node->ref(); // this node is unref'ed at exit
    defaultdraggerparts->append(node);
  }
}

/*!
  Protected version of setPartAsDefault(), to make it possible to set
  non-leaf and private parts (if \a anypart is \c TRUE).

  \sa setPartAsDefault()
*/
SbBool
SoInteractionKit::setAnyPartAsDefault(const SbName & partname,
                                      SoNode * node,
                                      SbBool anypart,
                                      SbBool onlyifdefault)
{
  SoBaseKit * kit = this;
  int partNum;
  SbBool isList;
  int listIdx;
  if (SoBaseKit::findPart(SbString(partname.getString()), kit, partNum,
                          isList, listIdx, TRUE)) {
    SoSFNode * field = kit->getCatalogInstances()[partNum];
    // FIXME: default check not working properly. pederb, 2000-01-21
    if (1 || (!onlyifdefault || field->isDefault())) {
      kit->setPart(partNum, node);
      field->setDefault(TRUE);
    }
    else {
#if COIN_DEBUG
      SoDebugError::postInfo("SoInteractionKit::setAnyPartAsDefault",
                             "no permission to set");
#endif // COIN_DEBUG
    }
  }
#if COIN_DEBUG
  else {
    SoDebugError::postInfo("SoInteractionKit::setAnyPartAsDefault",
                           "part %s not found", partname.getString());
  }
#endif // COIN_DEBUG

  return FALSE;
}

/*!
  Protected version of setPartAsDefault(), to make it possible to set
  non-leaf and private parts (if anypart is \c TRUE).

  \sa setPartAsDefault()
*/
SbBool
SoInteractionKit::setAnyPartAsDefault(const SbName & partname,
                                      const SbName & nodename,
                                      SbBool anypart,
                                      SbBool onlyifdefault)
{
  SoNode * node = (SoNode *)
    SoBase::getNamedBase(nodename, SoNode::getClassTypeId());
  if (node) {
    return this->setAnyPartAsDefault(partname, node, anypart, onlyifdefault);
  }
#if COIN_DEBUG && 1 // debug
  else {
    SoDebugError::postInfo("SoInteractionKit::setAnyPartAsDefault",
                           "nodename %s not found", nodename.getString());

    // FIXME: temporary code, pederb 2000-01-21
    node = new SoText2();
    ((SoText2 *)node)->string = "Default dragger part not found";
    return this->setAnyPartAsDefault(partname, node, anypart, onlyifdefault);
  }
#endif // debug
  return FALSE;
}

/*!
  Protected version of setPartAsPath(), to make it possible to set non-leaf
  and private parts.
  \sa setPartAsPath()
*/
SbBool
SoInteractionKit::setAnySurrogatePath(const SbName & partname,
                                      SoPath * path,
                                      SbBool leafcheck,
                                      SbBool publiccheck)
{
  SoBaseKit * kit = this;
  int partNum;
  SbBool isList;
  int listIdx;
  if (SoBaseKit::findPart(SbString(partname.getString()), kit, partNum,
                          isList, listIdx, TRUE)) {
    assert(kit->isOfType(SoInteractionKit::getClassTypeId()));
    const SoNodekitCatalog * catalog = kit->getNodekitCatalog();
    if (leafcheck && !catalog->isLeaf(partNum)) {
#if COIN_DEBUG && 1 // debug
      SoDebugError::postInfo("SoInteractionKit::setAnySurrogatePath",
                             "part %s is not leaf", partname.getString());
#endif // debug
      return FALSE;
    }
    if (publiccheck && !catalog->isPublic(partNum)) {
#if COIN_DEBUG && 1 // debug
      SoDebugError::postInfo("SoInteractionKit::setAnySurrogatePath",
                             "part %s is not public", partname.getString());
#endif // debug
      return FALSE;
    }
    int parentIdx = catalog->getParentPartNumber(partNum);
    SoNode * parent = this->getCatalogInstances()[parentIdx]->getValue();
    if (parent->isOfType(SoSwitch::getClassTypeId())) {
      SoNode * node = this->getCatalogInstances()[partNum]->getValue();
      SoType type = node->getTypeId();
      if (type == SoGroup::getClassTypeId() ||
          type == SoSeparator::getClassTypeId()) {
        // replace with empty group to keep switch numbering
        kit->setPart(partNum, (SoNode *)type.createInstance());
      }
      else { // set to NULL and update switch numbering
        SoSwitch * sw = (SoSwitch *)parent;
        int whichChild = sw->whichChild.getValue();
        int partIdx = sw->findChild(node);
        if (partIdx == whichChild) {
          sw->whichChild.setValue(SO_SWITCH_NONE);
        }
        else if (partIdx < whichChild) {
          sw->whichChild.setValue(whichChild-1);
        }
        kit->setPart(partNum, NULL);
      }
    }
    else {
      // set part to NULL
      kit->setPart(partNum, NULL);
    }
    // add the path
    ((SoInteractionKit *)kit)->pimpl->addSurrogatePath(path, catalog->getName(partNum));
    return TRUE;
  }
#if COIN_DEBUG && 1 // debug
  else {
    SoDebugError::postInfo("SoInteractionKit::setAnyPartAsDefault",
                           "part %s not found", partname.getString());
  }
#endif // debug
  return FALSE;
}

/*!
  FIXME: doc
*/
SbBool
SoInteractionKit::setUpConnections(SbBool onoff, SbBool doitalways)
{
  if (onoff == this->connectionsSetUp && !doitalways)
    return onoff;

  if (onoff) {
    inherited::setUpConnections(onoff, FALSE);
    THIS->connectFields(TRUE);
    THIS->attachSensor(TRUE);
  }
  else {
    THIS->attachSensor(FALSE);
    THIS->connectFields(FALSE);
    inherited::setUpConnections(onoff, FALSE);
  }
  return !(this->connectionsSetUp = onoff);
}

/*!
  Overloaded to detect when part changes value. If a substitute
  path for that part exists, it must be cleared.
*/
SbBool
SoInteractionKit::setPart(const int partNum, SoNode * node)
{
  THIS->removeSurrogatePath(this->getNodekitCatalog()->getName(partNum));
  return inherited::setPart(partNum, node);
}

// test if ok to set default and then do it if test succeeds
static void
test_set_default(SoSFEnum * field, int value)
{
  if (!(field->isConnected() && field->isConnectionEnabled()) &&
      field->getValue() == value) field->setDefault(TRUE);
}

/*!
  FIXME: doc
*/
void
SoInteractionKit::setDefaultOnNonWritingFields(void)
{
  this->topSeparator.setDefault(TRUE);
  this->geomSeparator.setDefault(TRUE);

  test_set_default(&this->renderCaching, SoInteractionKit::AUTO);
  test_set_default(&this->boundingBoxCaching, SoInteractionKit::AUTO);
  test_set_default(&this->pickCulling, SoInteractionKit::AUTO);
  test_set_default(&this->renderCulling, SoInteractionKit::AUTO);

  const SoNodekitCatalog * catalog = this->getNodekitCatalog();

  for (int i = 0; i < this->getCatalogInstances().getLength(); i++) {
    if (!catalog->isLeaf(i)) {
      SoNode * node = this->getCatalogInstances()[i]->getValue();
      if (node && node->getTypeId() == SoSwitch::getClassTypeId()) {
        this->getCatalogInstances()[i]->setDefault(TRUE);
      }
    }
  }

  inherited::setDefaultOnNonWritingFields();
}

/*!
  Overloaded only to fool the incredible stupid gcc 2.95.2
  compiler, who couldn't figure out I wanted to call this function in
  SoBaseKit, but instead insisted that I tried to call
  SoInteractionKit::setPart(int, SoNode *). Cheeessss.
*/
SbBool
SoInteractionKit::setPart(const SbName & partname, SoNode * from)
{
  return inherited::setPart(partname, from);
}

#undef THIS
// methods for SoInteractionKitP are below

//
// checks if partname is in surrogate list. Returns index, -1 if not found.
//
int
SoInteractionKitP::findSurrogateIndex(const SbName & partname) const
{
  int i, n = this->surrogatenamelist.getLength();
  for (i = 0; i < n; i++) {
    if (this->surrogatenamelist[i] == partname) return i;
  }
  return -1;
}

//
// removes surrogate path with name 'partname'
//
void
SoInteractionKitP::removeSurrogatePath(const SbName & partname)
{
  int idx = this->findSurrogateIndex(partname);
  if (idx >= 0) this->removeSurrogatePath(idx);
}

//
// removes a specified surrogate path
//
void
SoInteractionKitP::removeSurrogatePath(const int idx)
{
  assert(idx >= 0 && idx < this->surrogatenamelist.getLength());
  this->surrogatenamelist.remove(idx);
  this->surrogatepathlist.remove(idx);
}

//
// return index of surrogate path that is contained within path,
// or -1 if none found.
//
int
SoInteractionKitP::findSurrogateInPath(const SoPath * path)
{
  int n = this->surrogatepathlist.getLength();
  for (int i = 0; i < n; i++) {
    if (path->containsPath(this->surrogatepathlist[i])) return i;
  }
  return -1;
}

//
// adds or replaces a surrogate path
//
void
SoInteractionKitP::addSurrogatePath(SoPath * path, const SbName & name)
{
  int idx = this->findSurrogateIndex(name);
  if (idx >= 0) {
    this->surrogatepathlist.remove(idx);
    this->surrogatenamelist.remove(idx);
  }
  this->surrogatepathlist.append(path);
  this->surrogatenamelist.append(name);
}


//
// connect fields in topSeparator to the fields in this node.
//
void
SoInteractionKitP::connectFields(const SbBool onoff)
{
  if (this->connectedseparator) { // always disconnect
    this->connectedseparator->renderCaching.disconnect();
    this->connectedseparator->boundingBoxCaching.disconnect();
    this->connectedseparator->renderCulling.disconnect();
    this->connectedseparator->pickCulling.disconnect();
    this->connectedseparator->unref();
    this->connectedseparator = NULL;
  }
  if (onoff) {
    SoSeparator * sep = (SoSeparator*) this->kit->topSeparator.getValue();
    if (sep) {
      this->connectedseparator = sep;
      this->connectedseparator->ref(); // ref to make sure pointer is legal
      sep->renderCaching.connectFrom(&this->kit->renderCaching);
      sep->boundingBoxCaching.connectFrom(&this->kit->boundingBoxCaching);
      sep->renderCulling.connectFrom(&this->kit->renderCulling);
      sep->pickCulling.connectFrom(&this->kit->pickCulling);
    }
  }
}

//
// attach sensor to topSeparator if onoff, detach otherwise
//
void
SoInteractionKitP::attachSensor(const SbBool onoff)
{
  if (onoff) {
    if (this->fieldsensor->getAttachedField() != &this->kit->topSeparator) {
      this->fieldsensor->attach(&this->kit->topSeparator);
    }
  }
  else {
    if (this->fieldsensor->getAttachedField()) this->fieldsensor->detach();
  }
}

//
// callback from field sensor connected to topSeparator
//
void
SoInteractionKitP::sensorCB(void * data, SoSensor *)
{
  SoInteractionKitP * thisp = (SoInteractionKitP*) data;
  if (thisp->connectedseparator != thisp->kit->topSeparator.getValue()) {
    thisp->connectFields(TRUE);
  }
}

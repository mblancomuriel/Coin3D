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
  \class SoGroup SoGroup.h Inventor/nodes/SoGroup.h
  \brief The SoGroup class is a node which managed other node instances.
  \ingroup nodes

  The internal scene datastructures in Coin is managed as directed
  graphs. The graphs are built by setting up a hierarchy through the
  use of group nodes (either of this type, or from subclasses like
  SoSeparator) which is then traversed when applying actions (like
  SoGLRenderAction) to it.
*/

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSubNodeP.h>
#include <assert.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/elements/SoCacheElement.h>

#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG


/*!
  \var SoChildList * SoGroup::children
  List of managed child nodes.
*/

// *************************************************************************

SO_NODE_SOURCE(SoGroup);

/*!
  Default constructor.
*/
SoGroup::SoGroup(void)
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoGroup);

  this->children = new SoChildList(this);
}

/*!
  Constructor.

  The \a nchildren argument is a hint to the group instance about how
  many children it is expected will be managed by this node. This
  makes it possible to do better resource allocation.
*/
SoGroup::SoGroup(int nchildren)
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoGroup);

  this->children = new SoChildList(this, nchildren);
}

/*!
  Destructor.
*/
SoGroup::~SoGroup()
{
  delete this->children;
}

/*!
  Returns pointer to child node at \a index.
*/
SoNode *
SoGroup::getChild(const int index) const
{
  assert((index >= 0) && (index < getNumChildren()));

  return (*this->children)[index];
}

/*!
  Returns number of child nodes managed by this group.
*/
int
SoGroup::getNumChildren(void) const
{
  return this->children->getLength();
}

// Doc from superclass.
SbBool
SoGroup::readInstance(SoInput * in, unsigned short flags)
{
  SbBool readfields = TRUE;

  // Make sure we're compatible with binary format Inventor 2.0 files.
  if (in->isBinary() && (in->getIVVersion() < 2.1f) &&
      this->getTypeId() == SoGroup::getClassTypeId())
    readfields = FALSE;

  // For nodes with fields inheriting SoGroup, the fields must come
  // before the children, according to the file format specification.
  if (readfields && !inherited::readInstance(in, flags)) return FALSE;

  return this->readChildren(in);
}

/*!
  Read all children of this node from \a in and attach them below this
  group in left-to-right order. Returns \c FALSE upon read error.
*/
SbBool
SoGroup::readChildren(SoInput * in)
{
  unsigned int numchildren = 0; // used by binary format import
  if (in->isBinary() && !in->read(numchildren)) {
    SoReadError::post(in, "Premature end of file");
    return FALSE;
  }

  for (unsigned int i=0; !in->isBinary() || (i < numchildren); i++) {
    SoBase * child;
    if (SoBase::read(in, child, SoNode::getClassTypeId())) {
      if (child == NULL) {
        if (in->eof()) {
          SoReadError::post(in, "Premature end of file");
          return FALSE;
        }
        else {
          if (in->isBinary()) {
            SoReadError::post(in, "Couldn't read valid identifier name");
            return FALSE;
          }

#if COIN_DEBUG && 0 // debug
          char m;
          if (in->read(m)) {
            SoDebugError::postInfo("SoGroup::readChildren",
                                   "next char: '%c'", m);
          }
#endif // debug
          // Completed reading of children for ASCII format import.
          return TRUE;
        }
      }
      else {
        this->addChild((SoNode *)child);
      }
    }
    else {
      // SoReadError::post() is called within the SoBase::read()
      // frame upon error conditions, so don't duplicate with
      // another error message here.  mortene.
      return FALSE;
    }
  }

  // A successful import operation for binary format reading of child
  // nodes will exit here.
  return TRUE;
}

// Overloaded from parent.
void
SoGroup::copyContents(const SoFieldContainer * from, SbBool copyconnections)
{
  this->removeAllChildren();

  inherited::copyContents(from, copyconnections);

  SoGroup * g = (SoGroup *)from;

  // Add children of "from" group node.
  for (int i=0 ; i < g->getNumChildren(); i++) {
    SoNode * cp = (SoNode *)
      SoFieldContainer::findCopy(g->getChild(i), copyconnections);
    this->addChild(cp);
  }
}

/*!
  Append a child \a node to the list of children nodes this group node
  is managing.
*/
void
SoGroup::addChild(SoNode * const node)
{
  assert(node != NULL);
  this->children->append(node);
}

/*!
  Insert a \a child node at position \a newchildindex.
*/
void
SoGroup::insertChild(SoNode * const child, const int newchildindex)
{
#if COIN_DEBUG
  if (newchildindex < 0 || newchildindex > this->getNumChildren()) {
    SoDebugError::post("SoGroup::insertChild",
                       "idx %d is out of bounds (groupnode # children == %d)",
                       newchildindex, this->getNumChildren());
    return;
  }
#endif // COIN_DEBUG
  this->children->insert(child, newchildindex);
}

/*!
  Remove node at \a childindex in our list of children.
*/
void
SoGroup::removeChild(const int childindex)
{
#if COIN_DEBUG
  if (childindex < 0 || childindex >= this->getNumChildren()) {
    SoDebugError::post("SoGroup::removeChild",
                       "idx %d is out of bounds (groupnode # children == %d)",
                       childindex, this->getNumChildren());
    return;
  }
#endif // COIN_DEBUG
  this->children->remove(childindex);
}

/*!
  Returns index in our list of children for child \a node, or -1 if \a
  node is not a child of this group node.
*/
int
SoGroup::findChild(const SoNode * const node) const
{
  return this->children->find((SoNode *) node);
}

// Doc from superclass.
void
SoGroup::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoGroup);
}


// Doc from superclass.
void
SoGroup::doAction(SoAction * action)
{
  int numindices;
  const int * indices;
  if (action->getPathCode(numindices, indices) == SoAction::IN_PATH) {
    this->children->traverseInPath(action, numindices, indices);
  }
  else {
    this->children->traverse(action); // traverse all children
  }
}

// Doc from superclass.
void
SoGroup::getBoundingBox(SoGetBoundingBoxAction * action)
{
  int numindices;
  const int * indices;
  int lastchildindex;

  if (action->getPathCode(numindices, indices) == SoAction::IN_PATH)
    lastchildindex = indices[numindices-1];
  else
    lastchildindex = this->getNumChildren() - 1;

  // Initialize accumulation variables.
  SbVec3f acccenter(0.0f, 0.0f, 0.0f);
  int numcenters = 0;

  for (int i = 0; i <= lastchildindex; i++) {
    this->children->traverse(action, i);

    // If center point is set, accumulate.
    if (action->isCenterSet()) {
      acccenter += action->getCenter();
        numcenters++;
        action->resetCenter();
    }
  }

  if (numcenters != 0)
    action->setCenter(acccenter / float(numcenters), FALSE);
}

// Doc from superclass.
void
SoGroup::GLRender(SoGLRenderAction * action)
{
  int numindices;
  const int * indices;
  SoAction::PathCode pathcode = action->getPathCode(numindices, indices);

  SoNode ** childarray = (SoNode**) this->children->getArrayPtr();
  SoState * state = action->getState();

  if (pathcode == SoAction::IN_PATH) {
    int lastchild = indices[numindices - 1];
    for (int i = 0; i <= lastchild && !action->hasTerminated(); i++) {
      SoNode * child = childarray[i];
      action->pushCurPath(i, child);
      if (action->getCurPathCode() != SoAction::OFF_PATH ||
          child->affectsState()) {
        if (!action->abortNow()) {
          child->GLRender(action);
        }
        else {
          SoCacheElement::invalidate(state);
        }
      }
      action->popCurPath(pathcode);
    }
  }
  else {
    action->pushCurPath();
    int n = this->children->getLength();
    for (int i = 0; i < n && !action->hasTerminated(); i++) {
      if (action->abortNow()) {
        // only cache if we do a full traversal
        SoCacheElement::invalidate(state);
        break;
      }
      action->popPushCurPath(i, childarray[i]);
      childarray[i]->GLRender(action);
    }
    action->popCurPath();
  }
}

// Doc from superclass.
void
SoGroup::callback(SoCallbackAction * action)
{
  SoGroup::doAction((SoAction *)action);
}

// Doc from superclass.
void
SoGroup::getMatrix(SoGetMatrixAction * action)
{
  switch (action->getCurPathCode()) {
  case SoAction::NO_PATH:
  case SoAction::BELOW_PATH:
    break;
  case SoAction::OFF_PATH:
  case SoAction::IN_PATH:
    SoGroup::doAction((SoAction *)action);
    break;
  }
}

// Doc from superclass.
void
SoGroup::pick(SoPickAction * action)
{
  SoGroup::doAction((SoAction *)action);
}

// Doc from superclass.
void
SoGroup::handleEvent(SoHandleEventAction * action)
{
  SoGroup::doAction((SoAction *)action);
}

// Doc from superclass.
void
SoGroup::write(SoWriteAction * action)
{
  SoOutput * out = action->getOutput();
  if (out->getStage() == SoOutput::COUNT_REFS) {
    inherited::write(action);
    // Only increase number of writereferences to the top level node
    // in a tree which is used multiple times.
    if (!this->hasMultipleWriteRefs()) SoGroup::doAction((SoAction *)action);
  }
  else if (out->getStage() == SoOutput::WRITE) {
    if (this->writeHeader(out, TRUE, FALSE)) return;
    this->writeInstance(out);
    if (out->isBinary()) out->write(this->getNumChildren());
    SoGroup::doAction((SoAction *)action);
    this->writeFooter(out);
  }
  else assert(0 && "unknown stage");
}

// Doc from superclass.
void
SoGroup::search(SoSearchAction * action)
{
  // Include this node in the search.
  inherited::search(action);
  if (action->isFound()) return;

  // If we're not the one being sought after, try child subgraphs.
  SoGroup::doAction((SoAction *)action);
}

/*!
  Returns list of children.
*/
SoChildList *
SoGroup::getChildren(void) const
{
  return ((SoGroup *)this)->children;
}

/*!
  Remove \a child from the set of children managed by this group node.
  Will decrease the reference count of \a child by 1.
*/
void
SoGroup::removeChild(SoNode * const child)
{
  int idx = this->findChild(child);

#if COIN_DEBUG
  if (idx < 0 || idx > this->getNumChildren()) {
    SoDebugError::post("SoGroup::removeChild",
                       "tried to remove non-existent child %p (%s)",
                       child,
                       child ? child->getTypeId().getName().getString() : "");
    return;
  }
#endif // COIN_DEBUG

  this->removeChild(idx);
}

/*!
  Do not manage the children anymore. Will dereference all children by
  1 as they are removed.
*/
void
SoGroup::removeAllChildren(void)
{
  this->children->truncate(0);
}

/*!
  Replace child at \a index with \a newChild.

  Dereferences the child previously at \a index, and increases the
  reference count of \a newChild by 1.
*/
void
SoGroup::replaceChild(const int index, SoNode * const newchild)
{
  // Note: its imperative that we use set() here, and not a
  // remove+insert pair of calls as that would puck up SoChildList
  // auditing from SoPath instances.
  this->children->set(index, newchild);
}

/*!
  Replace \a oldchild with \a newchild.

  Dereferences \a oldchild by 1, and increases the reference count of
  \a newchild by 1.
*/
void
SoGroup::replaceChild(SoNode * const oldchild, SoNode * const newchild)
{
#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoGroup::replaceChild",
                         "(%p) from %p (%s) to %p (%s)",
                         this,
                         oldchild,
                         oldchild->getTypeId().getName().getString(),
                         newchild,
                         newchild->getTypeId().getName().getString());
#endif // debug

  int idx = this->findChild(oldchild);

#if COIN_DEBUG
  if (idx < 0 || idx > this->getNumChildren()) {
    SoDebugError::post("SoGroup::replaceChild",
                       "(%p) Tried to remove non-existent child %p (%s)",
                       this,
                       oldchild,
                       oldchild->getTypeId().getName().getString());
    return;
  }
#endif // COIN_DEBUG

  this->replaceChild(idx, newchild);
}

// Doc from parent class.
void
SoGroup::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  SoGroup::doAction((SoAction *)action);
}

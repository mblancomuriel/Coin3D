#ifndef CC_RWMUTEXP_H
#define CC_RWMUTEXP_H

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

#ifndef COIN_INTERNAL
#error this is a private header file
#endif /* ! COIN_INTERNAL */

#include <Inventor/C/threads/common.h>
#include <Inventor/C/threads/mutexp.h>
#include <Inventor/C/threads/condvarp.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ********************************************************************** */

struct cc_rwmutex {
  cc_precedence policy;
  int readers;
  int writers;
  int readwaiters;
  int writewaiters;
  cc_mutex mutex;
  cc_condvar read;
  cc_condvar write;
};

/* ********************************************************************** */

void cc_rwmutex_struct_init(cc_rwmutex * rwmutex);
void cc_rwmutex_struct_clean(cc_rwmutex * rwmutex);

/* ********************************************************************** */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* ! CC_RWMUTEXP_H */

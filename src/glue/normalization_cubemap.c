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

#include <Inventor/system/gl.h>
#include <Inventor/C/glue/gl.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <Inventor/C/glue/glp.h>


#define COIN_CUBEMAP_SIZE 32

/*
  Based on code from "Simple Bump Mapping Tutorial"
  http://www.paulsprojects.net
*/

/*
  Convert a [-1,1] vector to a [0,1] vector.
*/
static void coin_pack_to_01(float * v)
{
  float len = (float) sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

  /* normalize */
  if (len != 0.0f) {
    len = 1.0f / len;
    v[0] *= len;
    v[1] *= len;
    v[2] *= len;
  }
  v[0] = 0.5f * v[0] + 0.5f;
  v[1] = 0.5f * v[1] + 0.5f;
  v[2] = 0.5f * v[2] + 0.5f;
}

static void
coin_create_normalization_cube_map(const int size)
{
  int i, j;

  float offset = 0.5f;
  float halfsize = size * 0.5f;
  float tmpvec[3];
  unsigned char * ptr;
  unsigned char * data = (unsigned char*) malloc(size*size*3);

  /* positive x */
  ptr = data;
  for(j = 0; j < size; j++) {
    for(i = 0; i < size; i++) {
      tmpvec[0] = halfsize;
      tmpvec[1] = -(((float)j)+offset-halfsize);
      tmpvec[2] = -(((float)i)+offset-halfsize);

      coin_pack_to_01(tmpvec);

      ptr[0]=(unsigned char)(tmpvec[0]*255);
      ptr[1]=(unsigned char)(tmpvec[1]*255);
      ptr[2]=(unsigned char)(tmpvec[2]*255);

      ptr += 3;
    }
  }
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
               0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  /* negative x */
  ptr = data;

  for(j = 0; j < size; j++) {
    for(i = 0; i < size; i++) {
      tmpvec[0] = -halfsize;
      tmpvec[1] = -(((float)j)+offset-halfsize);
      tmpvec[2] = (((float)i)+offset-halfsize);

      coin_pack_to_01(tmpvec);

      ptr[0]=(unsigned char)(tmpvec[0]*255);
      ptr[1]=(unsigned char)(tmpvec[1]*255);
      ptr[2]=(unsigned char)(tmpvec[2]*255);

      ptr += 3;
    }
  }
  glTexImage2D(	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  /* positive y */
  ptr=data;

  for(j = 0; j < size; j++) {
      for(i = 0; i < size; i++) {
        tmpvec[0] = ((float)i)+offset-halfsize;
        tmpvec[1] = halfsize;
        tmpvec[2] = ((j+offset-halfsize));

        coin_pack_to_01(tmpvec);

        ptr[0]=(unsigned char)(tmpvec[0]*255);
        ptr[1]=(unsigned char)(tmpvec[1]*255);
        ptr[2]=(unsigned char)(tmpvec[2]*255);

        ptr += 3;
      }
  }
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
               0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  /* negative y */
  ptr = data;

  for(j = 0; j < size; j++) {
    for(i = 0; i < size; i++) {
      tmpvec[0] = ((float)i)+offset-halfsize;
      tmpvec[1] = -halfsize;
      tmpvec[2] = -(((float)j)+offset-halfsize);

      coin_pack_to_01(tmpvec);

      ptr[0]=(unsigned char)(tmpvec[0]*255);
      ptr[1]=(unsigned char)(tmpvec[1]*255);
      ptr[2]=(unsigned char)(tmpvec[2]*255);

      ptr += 3;
    }
  }
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
               0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  /* positive z */
  ptr = data;

  for(j = 0; j < size; j++) {
    for(i = 0; i < size; i++) {
      tmpvec[0] = ((float)i)+offset-halfsize;
      tmpvec[1] = -(((float)j)+offset-halfsize);
      tmpvec[2] = halfsize;

      coin_pack_to_01(tmpvec);

      ptr[0]=(unsigned char)(tmpvec[0]*255);
      ptr[1]=(unsigned char)(tmpvec[1]*255);
      ptr[2]=(unsigned char)(tmpvec[2]*255);

      ptr += 3;
    }
  }
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
               0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  /* negative z */
  ptr=data;

  for(j = 0; j < size; j++) {
    for(i = 0; i < size; i++) {
      tmpvec[0] = -(((float)i)+offset-halfsize);
      tmpvec[1] = -(((float)j)+offset-halfsize);
      tmpvec[2] = -halfsize;

      coin_pack_to_01(tmpvec);

      ptr[0]=(unsigned char)(tmpvec[0]*255);
      ptr[1]=(unsigned char)(tmpvec[1]*255);
      ptr[2]=(unsigned char)(tmpvec[2]*255);

      ptr += 3;
    }
  }
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
               0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  free(data);
}

void
coin_apply_normalization_cube_map(const cc_glglue * glue)
{
  if (glue->normalizationcubemap == 0) {
    GLuint * dst = (GLuint*) &glue->normalizationcubemap;
    cc_glglue_glGenTextures(glue, 1, dst);
    cc_glglue_glBindTexture(glue, GL_TEXTURE_CUBE_MAP, *dst);
    coin_create_normalization_cube_map(COIN_CUBEMAP_SIZE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
  cc_glglue_glBindTexture(glue, GL_TEXTURE_CUBE_MAP,
                          glue->normalizationcubemap);
}

#undef COIN_CUBEMAP_SIZE

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <float.h> /* FLT_MIN */

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <Inventor/C/basic.h>
#include <Inventor/C/base/list.h>
#include <Inventor/C/base/hash.h>
#include <Inventor/C/base/hashp.h>
#include <Inventor/C/base/string.h>
#include <Inventor/C/threads/threadsutilp.h>

#include <Inventor/C/glue/freetype.h>
#include <Inventor/C/tidbitsp.h>

#include "fontlib_wrapper.h"
#include "freetype.h"
#include "glyph3d.h"
#include "defaultfonts.h"

static SbBool glyph3d_specmatch(const cc_font_specification * spec1, 
                                const cc_font_specification * spec2);
static void glyph3d_calcboundingbox(cc_glyph3d * g);

struct cc_glyph3d {
  int fontidx;
  int glyphidx;
  float width;
  float bbox[4];
  cc_flw_vector_glyph * vectorglyph;
  SbBool didallocvectorglyph;
  cc_font_specification * fontspec;
  uint32_t character;
  int refcount;
};

static cc_hash * glyph3d_fonthash = NULL;
static int glyph3d_spaceglyphindices[] = { -1, -1 };
static float glyph3d_spaceglyphvertices[] = { 0, 0 };
static SbBool glyph3d_initialized = FALSE;

/* Mutex lock for the static ang global font hash */
static void * glyph3d_fonthash_lock = NULL;

/* Because the 3D glyphs are normalized when generated, a standard
   fontsize is used for all glyphs. This also prevent Windows from
   quantizing advancement and kerning values for very small fontsizes
   (size<4).
*/
static unsigned int glyph3d_standardfontsize = 50;

/* Set '#if 1' to enable debug info to stderr when tracking mutex locking. */
#if 0
#define GLYPH3D_MUTEX_LOCK(m) \
  do { \
    (void)fprintf(stderr, "glyph3d mutex lock in %s\n", __func__); \
    CC_MUTEX_LOCK(m); \
  } while (0)
#define GLYPH3D_MUTEX_UNLOCK(m) \
  do { \
    (void)fprintf(stderr, "glyph3d mutex unlock in %s\n", __func__); \
    CC_MUTEX_UNLOCK(m); \
  } while (0)
#else
#define GLYPH3D_MUTEX_LOCK(m) CC_MUTEX_LOCK(m)
#define GLYPH3D_MUTEX_UNLOCK(m) CC_MUTEX_UNLOCK(m)
#endif

static void
cc_glyph3d_cleanup(void)
{
  CC_MUTEX_DESTRUCT(glyph3d_fonthash_lock);
  glyph3d_fonthash_lock = NULL;
  cc_hash_destruct(glyph3d_fonthash);
  glyph3d_fonthash = NULL;
}

static void
cc_glyph3d_initialize()
{

  static SbBool initialized = FALSE;
  CC_MUTEX_CONSTRUCT(glyph3d_fonthash_lock);
  GLYPH3D_MUTEX_LOCK(glyph3d_fonthash_lock);
  
  if (initialized) {
    GLYPH3D_MUTEX_UNLOCK(glyph3d_fonthash_lock);
    return;
  }
  initialized = TRUE;
  
  glyph3d_fonthash = cc_hash_construct(15, 0.75);

  coin_atexit((coin_atexit_f*) cc_glyph3d_cleanup, 0);
  GLYPH3D_MUTEX_UNLOCK(glyph3d_fonthash_lock);  
}

cc_glyph3d *
cc_glyph3d_ref(uint32_t character, const cc_font_specification * spec)
{

  cc_glyph3d * glyph;
  int glyphidx;
  int fontidx;
  int i;
  void * val;
  cc_font_specification * newspec;
  int namelen = 0;
  cc_string * fonttoload;
  cc_list * glyphlist;

  /* Beacuse this function is the entry point for glyph3d, the mutex
     is initialized here. */
  if (glyph3d_fonthash_lock == NULL) 
    cc_glyph3d_initialize();
  
  assert(spec);

  GLYPH3D_MUTEX_LOCK(glyph3d_fonthash_lock);

  /* Has the glyph been created before? */
  if (cc_hash_get(glyph3d_fonthash, (unsigned long) character, &val)) {
    glyphlist = (cc_list *) val;
    for (i=0;i<cc_list_get_length(glyphlist);++i) {
      glyph = (cc_glyph3d *) cc_list_get(glyphlist, i);
      if (glyph3d_specmatch(spec, glyph->fontspec)) {
        GLYPH3D_MUTEX_UNLOCK(glyph3d_fonthash_lock);
        glyph->refcount++;
        return glyph;
      }
    }    
  } else {
    /* No glyphlist for this character is found. Create one and
       add it to the hashtable. */
    glyphlist = cc_list_construct();
    cc_hash_put(glyph3d_fonthash, (unsigned long) character, glyphlist);
  }

  assert(glyphlist);

  /* build a new glyph struct */
  glyph = (cc_glyph3d *) malloc(sizeof(cc_glyph3d));

  glyph->character = character;
  glyph->refcount = 1;


  newspec = (cc_font_specification *) malloc(sizeof(cc_font_specification));
  assert(newspec);
  cc_fontspec_copy(spec, newspec);

  glyph->fontspec = newspec;

  /* FIXME: fonttoload variable should be allocated on the
     stack. 20030921 mortene. */
  fonttoload = cc_string_construct_new();
  cc_string_set_text(fonttoload, cc_string_get_text(&spec->name));
  if (cc_string_length(&spec->style) > 0) {
    cc_string_append_text(fonttoload, " ");
    cc_string_append_string(fonttoload, &spec->style);
  }
  
  fontidx = cc_flw_get_font_id(cc_string_get_text(fonttoload), 
                               glyph3d_standardfontsize,
                               glyph3d_standardfontsize,
                               0.0f,
                               newspec->complexity);

  /* fprintf(stderr,"new glyph: %c\n", glyph->character); */

  cc_string_destruct(fonttoload);
  assert(fontidx >= 0);

  cc_flw_ref_font(fontidx);

  /* Should _always_ be able to get hold of a glyph -- if no glyph is
     available for a specific character, a default empty rectangle
     should be used.  -mortene. */
  glyphidx = cc_flw_get_glyph(fontidx, character);
  assert(glyphidx >= 0);

  glyph->glyphidx = glyphidx;
  glyph->fontidx = fontidx;
  glyph->didallocvectorglyph = FALSE;

  if (fontidx != 0) 
    glyph->vectorglyph = cc_flw_get_vector_glyph(fontidx, glyphidx, newspec->complexity);
  else
    glyph->vectorglyph = NULL;

  /* Setup builtin default font if no character was found */
  if (glyph->vectorglyph == NULL) {
    glyph->vectorglyph = (struct cc_flw_vector_glyph *) malloc(sizeof(struct cc_flw_vector_glyph));
    glyph->didallocvectorglyph = TRUE;
  
    if (character <= 32 || character >= 127) {

      /* FIXME: Characters other than space, should be replaced with
         squares. (20030910 handegar) */

      /* treat all these characters as spaces*/
      glyph->vectorglyph->vertices = (float *) glyph3d_spaceglyphvertices;
      glyph->vectorglyph->faceindices = (int *) glyph3d_spaceglyphindices;
      glyph->vectorglyph->edgeindices = (int *) glyph3d_spaceglyphindices;     
    }
    else {
      glyph->vectorglyph->vertices = (float *) coin_default3dfont_get_coords()[character-33];
      glyph->vectorglyph->faceindices = (int *) coin_default3dfont_get_faceidx()[character-33];
      glyph->vectorglyph->edgeindices = (int *) coin_default3dfont_get_edgeidx()[character-33];
    }
  } 
  
  glyph3d_calcboundingbox(glyph);  
  glyph->width = glyph->bbox[2] - glyph->bbox[0];

  /* Store newly created glyph in the list for this character */
  cc_list_append(glyphlist, glyph);

  GLYPH3D_MUTEX_UNLOCK(glyph3d_fonthash_lock);
  return glyph;
}

void 
cc_glyph3d_unref(cc_glyph3d * glyph)
{
  glyph->refcount--;
  if (glyph->refcount == 0) {
    cc_list * glyphlist;
    int ret;
    void * tmp;
    int i;

    /* fprintf(stderr,"unref glyph: %c\n", glyph->character); */

    ret = cc_hash_get(glyph3d_fonthash, (unsigned long) glyph->character, &tmp);
    assert(ret);
    glyphlist = (cc_list*) tmp;
    
    for (i = 0; i < cc_list_get_length(glyphlist); i++) {
      if (glyph == (cc_glyph3d*) cc_list_get(glyphlist, i)) break;
    }    
    assert(i < cc_list_get_length(glyphlist));

    cc_list_remove_fast(glyphlist, i);
    if (cc_list_get_length(glyphlist) == 0) {
      (void) cc_hash_remove(glyph3d_fonthash, (unsigned long) glyph->character);
      cc_list_destruct(glyphlist);
    }

    cc_fontspec_clean(glyph->fontspec);
    free(glyph->fontspec);

    if (glyph->didallocvectorglyph) {
      free(glyph->vectorglyph);
    }

    cc_flw_done_glyph(glyph->fontidx, glyph->glyphidx);
    cc_flw_unref_font(glyph->fontidx);

    free(glyph);
  }
}

const float *
cc_glyph3d_getcoords(const cc_glyph3d * g)
{
  const float * ptr = cc_flw_get_vector_glyph_coords(g->vectorglyph);
  if (ptr == NULL) {    
    assert(g->vectorglyph->vertices && "Default vertices has not been initialized as expected!");
    /* default vertices are already initialized in 'cc_glyph3d_getglyph()' */
    return g->vectorglyph->vertices;
  } else return ptr;  
}

const int *
cc_glyph3d_getfaceindices(const cc_glyph3d * g)
{
  const int * ptr = cc_flw_get_vector_glyph_faceidx(g->vectorglyph);
  if (ptr == NULL) {      
    assert(g->vectorglyph->faceindices && "Default face indices has not been initialized as expected!");
    /* default face indices are already initialized in 'cc_glyph3d_getglyph()' */
    return g->vectorglyph->faceindices; 
  } else return ptr;
}

const int *
cc_glyph3d_getedgeindices(const cc_glyph3d * g)
{
  const int * ptr = cc_flw_get_vector_glyph_edgeidx(g->vectorglyph);
  if (ptr == NULL) {    
    assert(g->vectorglyph->edgeindices && "Default edge indices has not been initialized as expected!");
    /* default edge indices are already initialized in 'cc_glyph3d_getglyph()'*/
    return g->vectorglyph->edgeindices; 
  } else return ptr;
}

const int *
cc_glyph3d_getnextcwedge(const cc_glyph3d * g, int edgeidx)
{

  int findidx;
  const int * ptr;
  const int * edgeptr;
  int idx = edgeidx * 2;

  edgeptr = cc_glyph3d_getedgeindices(g);

  /* test for common case */
  if (edgeidx > 0) {
    if (edgeptr[idx] == edgeptr[idx-1])
      return &edgeptr[idx-2];
  }
  /* do a linear search */
  findidx = edgeptr[idx];
  ptr = edgeptr;
  while (*ptr >= 0) {
    if (ptr[1] == findidx) return ptr;
    ptr += 2;
  }

  return NULL;
}

const int *
cc_glyph3d_getnextccwedge(const cc_glyph3d * g, int edgeidx)
{
  int findidx;
  const int * ptr;
  const int * edgeptr;  
  int idx = edgeidx * 2;
  
  edgeptr = cc_glyph3d_getedgeindices(g);

  /* test for common case */
  if (edgeptr[idx+1] == edgeptr[idx+2])
    return &edgeptr[idx+2];

  /* do a linear search */
  findidx = edgeptr[idx+1];
  ptr = edgeptr;
  while (*ptr >= 0) {
    if (*ptr == findidx) 
      return ptr;
    ptr += 2;
  }

  return NULL;

}


float
cc_glyph3d_getwidth(const cc_glyph3d * g)
{
  assert(g && "Parameter cc_glyph3d == NULL!");
  return g->width;
}

static void
glyph3d_calcboundingbox(cc_glyph3d * g)
{
  float * coordptr;
  int * edgeptr;

  g->bbox[0] = 0.0f;
  g->bbox[1] = 0.0f;
  g->bbox[2] = 0.0f;
  g->bbox[3] = 0.0f;

  coordptr = (float *) cc_glyph3d_getcoords(g);
  edgeptr = (int *) cc_glyph3d_getedgeindices(g);

  while ((*edgeptr >= 0) && (*edgeptr != -1)) {

    g->bbox[0] = cc_min(coordptr[(*edgeptr)*2], g->bbox[0]);
    g->bbox[1] = cc_min(coordptr[(*edgeptr)*2 + 1], g->bbox[1]);
    g->bbox[2] = cc_max(coordptr[(*edgeptr)*2], g->bbox[2]);
    g->bbox[3] = cc_max(coordptr[(*edgeptr)*2 + 1], g->bbox[3]);

    *edgeptr++;
  }

}

const float *
cc_glyph3d_getboundingbox(const cc_glyph3d * g)
{
  return g->bbox;
}

void
cc_glyph3d_getadvance(const cc_glyph3d * g, float * x, float * y)
{
  cc_flw_get_vector_advance(g->fontidx, g->glyphidx, x, y);
}

void
cc_glyph3d_getkerning(const cc_glyph3d * left, const cc_glyph3d * right,
                      float * x, float * y)
{
  cc_flw_get_vector_kerning(right->fontidx, left->glyphidx, right->glyphidx, x, y);
}


static SbBool
glyph3d_specmatch(const cc_font_specification * spec1,
                  const cc_font_specification * spec2)
{
  
  float c1, c2;
  int temp1,temp2;
  
  assert(spec1);
  assert(spec2);
  
  /* Reducing precision of the complexity variable. This is done to
     prevent the user from flooding the memory with generated glyphs
     which might be more or less identical */
  c1 = spec1->complexity;
  c2 = spec2->complexity;
  
  /* Clamp values to [0...1] */
  if (c1 > 1.0f) c1 = 1.0f;
  if (c1 < 0.0f) c1 = 0.0f;
  if (c2 > 1.0f) c2 = 1.0f;
  if (c2 < 0.0f) c2 = 0.0f;
  
  temp1 = (int) (c1 * 10.0f);
  temp2 = (int) (c2 * 10.0f);
  
  if ((!cc_string_compare(&spec1->name, &spec2->name)) &&
      (!cc_string_compare(&spec1->style, &spec2->style)) &&
      (temp1 == temp2)) {
    /* No need to compare size for 3D fonts */
    return TRUE;
  }
  else return FALSE;
  
}



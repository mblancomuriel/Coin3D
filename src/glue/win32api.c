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

/* See comment block at top of win32api.h header file for the
   rationale behind this wrapper. */

/*************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_WIN32_API

/* ********************************************************************** */

#include <assert.h>
#include <stdlib.h>

#include <windows.h>

#include <Inventor/C/glue/win32api.h>
#include <Inventor/C/errors/debugerror.h>

/* ********************************************************************** */

/* internal helper function */
void
cc_win32_print_error(const char * callerfuncname, const char * apifuncname,
                     DWORD lasterror)
{
  char * outputstr = NULL;
  LPTSTR buffer = NULL;
  BOOL result = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                              FORMAT_MESSAGE_FROM_SYSTEM |
                              FORMAT_MESSAGE_IGNORE_INSERTS,
                              NULL,
                              lasterror,
                              0,
                              (LPTSTR)&buffer,
                              0,
                              NULL);
  if (!result) {
    cc_debugerror_post("cc_win32_print_error", "FormatMessage() failed!");
    return;
  }

#ifdef UNICODE
  { /* Narrow from wide characters to 8-bit characters. */
    size_t len;
    len = wcstombs(NULL, buffer, 0);
    outputstr = (char *)LocalAlloc(0, len + 1);
    if (!outputstr) {
      cc_debugerror_post("cc_win32_print_error",
                         "LocalAlloc() failed! (errorcode %d)",
                         GetLastError());
      goto exitfunc;
    }

    if (wcstombs(outputstr, buffer, len + 1) != len) {
      cc_debugerror_post("cc_win32_print_error",
                         "UNICODE to mbs conversion failed!");
      outputstr[0] ='\0';
      goto exitfunc;
    }
  }
#else /* !UNICODE */
  outputstr = (char *)buffer;
#endif /* !UNICODE */

  cc_debugerror_post(callerfuncname, "%s failed: '%s'", apifuncname, outputstr);

#ifdef UNICODE
exitfunc:
#endif /* UNICODE */

  /* Don't call coin_LocalFree() here, to make sure we don't get a
     recursive call back here again. */
  if (buffer && LocalFree(buffer) != NULL) {
    cc_debugerror_post("cc_win32_print_error",
                       "LocalFree() failed! (errorcode %d)",
                       GetLastError());
  }
  if (outputstr && (outputstr != (char *)buffer)) {
    if (LocalFree(outputstr) != NULL) {
      cc_debugerror_post("cc_win32_print_error",
                         "LocalFree() failed! (errorcode %d)",
                         GetLastError());
    }
  }
}

static BOOL WINAPI
coin_GetVersionEx(LPOSVERSIONINFO osvi) 
{

  /* Because one might want to fetch extended version-info using the
     extended version of the OSVERSIONINFO struct (service pack info
     etc.), errorhandeling will not be taken care of at this point. See
     <URL:http://msdn.microsoft.com/library/default.asp?url=/library/en-us/sysinfo/base/getting_the_system_version.asp>
     for an example of use. */

  return GetVersionEx(osvi);
}

/* ********************************************************************** */

static int WINAPI
coin_GetTextFace(HDC hdc, /* handle to device context */
                 int nCount, /* length of buffer receiving typeface name */
                 LPTSTR lpFaceName) /* pointer to buffer receiving typeface name */
{

  OSVERSIONINFO osvi;
  
  /* FIXME: Due to a bug in WIN95/98/ME, the GetTextFace(-,-,NULL)
     will allways return size=0. A workaround is to specify the
     expected size of the font name (in this case; 1024 chars). The
     fontname will be zero terminated, so name-lengths below 1024 will
     be OK. If size >= 1024, the system will crop the
     string. (20031011 handegar).*/
  
  int copied = GetTextFace(hdc, nCount, lpFaceName);
  if (copied == 0 && lpFaceName == NULL) {    
    /* NOTE: We could have used the 'VerifyVersionInfo()'
       function, but this is not supported on Win95/98/ME. */

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);  

    if (!coin_GetVersionEx(&osvi)) {      
      cc_win32_print_error("coin_GetTextFace", "GetVersionEx()", GetLastError());
      assert(FALSE && "unexpected error");      
    }
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {          
      /* This is a wellknown WIN95/98/Me bug. Return a default, large number. */
      copied = 1024;
    }
  }

  if (copied == 0) {
    cc_string apicall;
    DWORD err = GetLastError();

    /* This case has been seen to hit, so we try to provide more
       information to aid future debugging. */
    cc_string_construct(&apicall);
    cc_string_sprintf(&apicall,
                      "GetTextFace(hdc==%p, nCount==%d, lpFaceName==%p)",
                      hdc, nCount, lpFaceName);
    cc_win32_print_error("coin_GetTextFace", cc_string_get_text(&apicall), err);
    assert(FALSE && "unexpected error");
    cc_string_clean(&apicall);
  }

  return copied;
}

static void WINAPI
coin_LocalFree(HLOCAL hMem) /* handle to local memory object */
{
  const HLOCAL ptr = LocalFree(hMem);
  if (ptr != NULL) {
    cc_win32_print_error("coin_LocalFree", "LocalFree()", GetLastError());
    assert(FALSE && "unexpected error");
  }
}

/* ********************************************************************** */

/* singleton access to the structure, so we can initialize it at first
   use */
const struct cc_win32_api *
cc_win32(void)
{
  static struct cc_win32_api * instance = NULL;

  if (instance) { return instance; }

  /* FIXME: one-time mem leak, clean up at exit. 20030530 mortene. */
  instance = (struct cc_win32_api *)malloc(sizeof(struct cc_win32_api));
  assert(instance != NULL);

  /* set up all function pointers */
  instance->GetTextFace = coin_GetTextFace;
  instance->LocalFree = coin_LocalFree;
  instance->GetVersionEx = coin_GetVersionEx;

  return instance;
}

/* ********************************************************************** */

#endif /* HAVE_WIN32_API */

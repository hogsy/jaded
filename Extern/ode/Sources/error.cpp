/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/
#include "Precomp.h"

#include <ode/config.h>
#include <ode/error.h>


static dMessageFunction *error_function = 0;
static dMessageFunction *debug_function = 0;
static dMessageFunction *message_function = 0;

#ifdef JADEFUSION
extern void dSetErrorHandler (dMessageFunction *fn)
#else
extern "C" void dSetErrorHandler (dMessageFunction *fn)
#endif
{
  error_function = fn;
}

#ifdef JADEFUSION
extern void dSetDebugHandler (dMessageFunction *fn)
#else
extern "C" void dSetDebugHandler (dMessageFunction *fn)
#endif
{
  debug_function = fn;
}

#ifdef JADEFUSION
extern void dSetMessageHandler (dMessageFunction *fn)
#else
extern "C" void dSetMessageHandler (dMessageFunction *fn)
#endif
{
  message_function = fn;
}

#ifdef JADEFUSION
extern dMessageFunction *dGetErrorHandler()
#else
extern "C" dMessageFunction *dGetErrorHandler()
#endif
{
  return error_function;
}

#ifdef JADEFUSION
extern dMessageFunction *dGetDebugHandler()
#else
extern "C" dMessageFunction *dGetDebugHandler()
#endif
{
  return debug_function;
}

#ifdef JADEFUSION
extern dMessageFunction *dGetMessageHandler()
#else
extern "C" dMessageFunction *dGetMessageHandler()
#endif
{
  return message_function;
}


static void printMessage (int num, const char *msg1, const char *msg2,
			  va_list ap)
{
  fflush (stderr);
  fflush (stdout);
  if (num) fprintf (stderr,"\n%s %d: ",msg1,num);
  else fprintf (stderr,"\n%s: ",msg1);
  vfprintf (stderr,msg2,ap);
  fprintf (stderr,"\n");
  fflush (stderr);
}

//****************************************************************************
// unix

#ifndef WIN32

#ifdef JADEFUSION
extern void dError (int num, const char *msg, ...)
#else
extern "C" void dError (int num, const char *msg, ...)
#endif
{
  va_list ap;
  va_start (ap,msg);
  if (error_function) error_function (num,msg,ap);
  else printMessage (num,"ODE Error",msg,ap);
#ifndef PSX2_TARGET  
#ifndef _XBOX
#ifndef _XENON
  exit (1);
#endif  
#endif
#endif
}

#ifdef JADEFUSION
extern void dDebug (int num, const char *msg, ...)
#else
extern "C" void dDebug (int num, const char *msg, ...)
#endif
{
  va_list ap;
  va_start (ap,msg);
  if (debug_function) debug_function (num,msg,ap);
  else printMessage (num,"ODE INTERNAL ERROR",msg,ap);
  // *((char *)0) = 0;   ... commit SEGVicide
#ifndef PSX2_TARGET  
#ifndef _XBOX
#ifndef _XENON
  abort();
#endif
#endif
#endif
}

#ifdef JADEFUSION
extern void dMessage (int num, const char *msg, ...)
#else
extern "C" void dMessage (int num, const char *msg, ...)
#endif
{
  va_list ap;
  va_start (ap,msg);
  if (message_function) message_function (num,msg,ap);
  else printMessage (num,"ODE Message",msg,ap);
}

#endif

//****************************************************************************
// windows

#ifdef WIN32

#include "windows.h"

#ifdef JADEFUSION
extern void dError (int num, const char *msg, ...)
#else
extern "C" void dError (int num, const char *msg, ...)
#endif
{
  va_list ap;
  va_start (ap,msg);
  if (error_function) error_function (num,msg,ap);
  else {
    char s[1000],title[100];
    _snprintf (title,sizeof(title),"ODE Error %d",num);
    _vsnprintf (s,sizeof(s),msg,ap);
    s[sizeof(s)-1] = 0;
    MessageBox(0,s,title,MB_OK | MB_ICONWARNING);
  }
  exit (1);
}
#ifdef JADEFUSION
extern void dDebug (int num, const char *msg, ...)
#else
extern "C" void dDebug (int num, const char *msg, ...)
#endif
{
  va_list ap;
  va_start (ap,msg);
  if (debug_function) debug_function (num,msg,ap);
  else {
    char s[1000],title[100];
    _snprintf (title,sizeof(title),"ODE INTERNAL ERROR %d",num);
    _vsnprintf (s,sizeof(s),msg,ap);
    s[sizeof(s)-1] = 0;
    MessageBox(0,s,title,MB_OK | MB_ICONSTOP);
  }
  abort();
}


#ifdef JADEFUSION
extern void dMessage (int num, const char *msg, ...)
#else
extern "C" void dMessage (int num, const char *msg, ...)
#endif
{
  va_list ap;
  va_start (ap,msg);
  if (message_function) message_function (num,msg,ap);
  else printMessage (num,"ODE Message",msg,ap);
}


#endif

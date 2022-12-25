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

//#include <ode/config.h>
#ifndef _GAMECUBE
#include <malloc.h>
#endif // _GAMECUBE

#include <ode/memory.h>
//#include <ode/error.h>


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" {
#endif

#if 0 //ndef _GAMECUBE

static dAllocFunction *allocfn = 0;
static dReallocFunction *reallocfn = 0;
static dFreeFunction *freefn = 0;



void dSetAllocHandler (dAllocFunction *fn)
{
  allocfn = fn;
}


void dSetReallocHandler (dReallocFunction *fn)
{
  reallocfn = fn;
}


void dSetFreeHandler (dFreeFunction *fn)
{
  freefn = fn;
}


dAllocFunction *dGetAllocHandler()
{
  return allocfn;
}


dReallocFunction *dGetReallocHandler()
{
  return reallocfn;
}


dFreeFunction *dGetFreeHandler()
{
  return freefn;
}


void * dAlloc (int size)
{
  if (allocfn) return allocfn (size); else return malloc (size);
}


void * dRealloc (void *ptr, int oldsize, int newsize)
{
  if (reallocfn) return reallocfn (ptr,oldsize,newsize);
  else return realloc (ptr,newsize);
}


void dFree (void *ptr, int size)
{
  if (!ptr) return;
  if (freefn) freefn (ptr,size); else free (ptr);
}

#endif // #ifndef _GAMECUBE

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#include "BASe/MEMory/MEM.h"

/*
CAlloca::CAlloca(void)
{
#ifdef MEM_OPT
	mpv_FirstAlloc = NULL;
#else //MEM_OPT
	mui_PreviousSize = MEM_gst_MemoryInfo.ul_DynamicMaxSize;
	mpv_PreviousPtr = MEM_gst_MemoryInfo.pv_LastFree;
#endif //MEM_OPT
}

CAlloca::~CAlloca(void)
{
#ifdef MEM_OPT
	while (mpv_FirstAlloc)
	{
		void *pNextAddress = (void *)(*((ULONG *)mpv_FirstAlloc));
		MEM_FreeFromEnd(4 + ((unsigned char *)mpv_FirstAlloc));
		mpv_FirstAlloc = pNextAddress;
	}
#else // MEM_OPT
	MEM_gst_MemoryInfo.ul_DynamicMaxSize = mui_PreviousSize ;
	MEM_gst_MemoryInfo.pv_LastFree = mpv_PreviousPtr;
#endif // MEM_OPT
}


void * CAlloca::my_alloca(unsigned int uiSize)
{
#ifdef MEM_OPT
	void *pAddress = MEM_p_AllocFromEnd(uiSize+4);
	*((ULONG *)pAddress) = (ULONG)mpv_FirstAlloc;
	mpv_FirstAlloc = pAddress;
	return 4 + ((unsigned char *)pAddress);
#else // MEM_OPT
	return MEM_p_AllocFromEnd(uiSize);
#endif // MEM_OPT
}
*/

CAlloca::CAlloca(void)
{
#ifdef MEM_OPT
	mp_FirstAlloc = NULL;
#else //MEM_OPT
	mui_PreviousSize = MEM_gst_MemoryInfo.ul_DynamicMaxSize;
	mpv_PreviousPtr = MEM_gst_MemoryInfo.pv_LastFree;
#endif //MEM_OPT
}

CAlloca::~CAlloca(void)
{
#ifdef MEM_OPT
	while (mp_FirstAlloc)
	{
		COneAlloc *pNextOneAlloc = mp_FirstAlloc->mp_NextAlloc;
		MEM_FreeFromEnd(mp_FirstAlloc->m_pvAlloc);
		MEM_FreeFromEnd(mp_FirstAlloc);
		mp_FirstAlloc = pNextOneAlloc;
	}
#else // MEM_OPT
	MEM_gst_MemoryInfo.ul_DynamicMaxSize = mui_PreviousSize ;
	MEM_gst_MemoryInfo.pv_LastFree = mpv_PreviousPtr;
#endif // MEM_OPT
}


void * CAlloca::my_alloca(unsigned int uiSize)
{
#ifdef MEM_OPT
	if(!uiSize) return NULL;
	COneAlloc *pNewOneAlloc = (COneAlloc *)MEM_p_AllocFromEnd(sizeof(COneAlloc));
	pNewOneAlloc->mp_NextAlloc = mp_FirstAlloc;
	pNewOneAlloc->m_pvAlloc = MEM_p_AllocFromEnd(uiSize);
	mp_FirstAlloc = pNewOneAlloc;
	return pNewOneAlloc->m_pvAlloc;
#else // MEM_OPT
	return MEM_p_AllocFromEnd(uiSize);
#endif // MEM_OPT
}



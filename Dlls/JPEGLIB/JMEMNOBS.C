/*
 * jmemnobs.c
 *
 * Copyright (C) 1992-1994, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file provides a really simple implementation of the system-
 * dependent portion of the JPEG memory manager.  This implementation
 * assumes that no backing-store files are needed: all required space
 * can be obtained from malloc().
 * This is very portable in the sense that it'll compile on almost anything,
 * but you'd better have lots of main memory (or virtual memory) if you want
 * to process big images.
 * Note that the max_memory_to_use option is ignored by this implementation.
 */

#define JPEG_INTERNALS
#include "JINCLUDE.H"
#include "JPEGLIB.H"
#include "JMEMSYS.H"		/* import the system-dependent declarations */


#ifndef HAVE_STDLIB_H		/* <stdlib.h> should declare malloc(),free() */
extern void * malloc JPP((size_t size));
extern void free JPP((void *ptr));
#endif

#ifndef JADEFUSION
extern void * _MEM_p_Alloc(size_t sizeofobject);
#else //JADEFUSION
#ifdef _DEBUG
extern void *_MEM_p_Alloc(unsigned long _ul_BlockSize, char *_psz_File, int _i_Line);
#define MEM_p_Alloc_Custom(BlockSize) _MEM_p_Alloc(BlockSize, "", 0)
#else
extern void *_MEM_p_Alloc(unsigned long _ul_BlockSize);
#define MEM_p_Alloc_Custom(BlockSize) _MEM_p_Alloc(BlockSize)
#endif
#endif //JADEFUSION

extern void MEM_Free(void *object);
void *jpgHightPointer = NULL;
#define jpgHightPointer_BASE (1024*1024*100)

/*
 * Memory allocation and freeing are controlled by the regular library
 * routines malloc() and free().
 */

GLOBAL void *
Jade_jpeg_get_small (j_common_ptr cinfo, size_t sizeofobject)
{
#ifdef JADEFUSION
#ifndef _XENON_RENDER
  return (void *) MEM_p_Alloc_Custom(sizeofobject);
#else 
  return NULL;
#endif
#else
	return (void *) _MEM_p_Alloc(sizeofobject);
#endif
}

GLOBAL void
Jade_jpeg_free_small (j_common_ptr cinfo, void * object, size_t sizeofobject)
{
  MEM_Free(object);
}


/*
 * "Large" objects are treated the same as "small" ones.
 * NB: although we include FAR keywords in the routine declarations,
 * this file won't actually work in 80x86 small/medium model; at least,
 * you probably won't be able to process useful-size images in only 64KB.
 */

GLOBAL void FAR *
Jade_jpeg_get_large (j_common_ptr cinfo, size_t sizeofobject)
{
#if !defined(_FINAL_) && defined(PSX2_TARGET)

  if ((unsigned int)sizeofobject > 512*1024) 
  {
  	if ((unsigned int)jpgHightPointer < jpgHightPointer_BASE) jpgHightPointer = (void *)jpgHightPointer_BASE;
  	*(unsigned int *)&jpgHightPointer += sizeofobject;
  	return (void *)((unsigned int)jpgHightPointer - sizeofobject);
  }
  return (void FAR *) _MEM_p_Alloc(sizeofobject);
#else 

#ifdef JADEFUSION
#if defined (_XENON_RENDER)
	return NULL;
#else
	return (void FAR *) MEM_p_Alloc_Custom(sizeofobject);
#endif
#else
	return (void FAR *) _MEM_p_Alloc(sizeofobject);
#endif
#endif  
}

GLOBAL void
Jade_jpeg_free_large (j_common_ptr cinfo, void FAR * object, size_t sizeofobject)
{
#if !defined(_FINAL_) && defined(PSX2_TARGET)
  if (jpgHightPointer_BASE >= (unsigned int)object)
	  MEM_Free(object);
#else
    MEM_Free(object);	  
#endif  
}


/*
 * This routine computes the total memory space available for allocation.
 * Here we always say, "we got all you want bud!"
 */

GLOBAL LONG
Jade_jpeg_mem_available (j_common_ptr cinfo, LONG min_bytes_needed,
		    LONG max_bytes_needed, LONG already_allocated)
{
  return max_bytes_needed;
}


/*
 * Backing store (temporary file) management.
 * Since Jade_jpeg_mem_available always promised the moon,
 * this should never be called and we can just error out.
 */

GLOBAL void
Jade_jpeg_open_backing_store (j_common_ptr cinfo, backing_store_ptr info,
			 LONG total_bytes_needed)
{
  ERREXIT(cinfo, JERR_NO_BACKING_STORE);
}


/*
 * These routines take care of any system-dependent initialization and
 * cleanup required.  Here, there isn't any.
 */

GLOBAL LONG
Jade_jpeg_mem_init (j_common_ptr cinfo)
{
  return 0;			/* just set max_memory_to_use to 0 */
}

GLOBAL void
Jade_jpeg_mem_term (j_common_ptr cinfo)
{
  /* no work */
}

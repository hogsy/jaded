/*$T TEXmemory.h GC!1.40 09/29/99 10:14:37 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "TEXture/TEXfile.h"
#include "BASe/BAStypes.h"


#ifndef PSX2_TARGET
#pragma once
#endif



#ifndef __TEXMEMORY_H__
#define __TEXMEMORY_H__

//#define TEX_USEMEMORYCACHE



#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$3
 ===================================================================================================
    Structures
 ===================================================================================================
 */

typedef struct  TEX_tdst_OneTextureInMemory_
{
    float               f_Time;
    BIG_KEY             ul_Key;
    TEX_tdst_File_Desc  st_Texture;
} TEX_tdst_OneTextureInMemory;

typedef struct  TEX_tdst_Memory_
{
    ULONG               ul_MaxTextures;
    ULONG               ul_MemoryTaken;
    ULONG               ul_MaxMemory;
    TEX_tdst_OneTextureInMemory *dst_Texture;
} TEX_tdst_Memory;

/*
 ===================================================================================================
    Function header
 ===================================================================================================
 */

#ifdef TEX_USEMEMORYCACHE
void                TEX_Memory_Init(ULONG);
void                TEX_Memory_Close(void);
LONG                TEX_l_Memory_AddTexture(BIG_KEY, TEX_tdst_File_Desc *);
LONG                TEX_l_Memory_DeleteTexture(BIG_KEY);
TEX_tdst_File_Desc  *TEX_pst_Memory_FindTexture(BIG_KEY);
#else
#define             TEX_Memory_Init(a)
#define             TEX_Memory_Close()
#define             TEX_l_Memory_AddTexture(a, b) 0
#define             TEX_l_Memory_DeleteTexture(a) 1
#define             TEX_pst_Memory_FindTexture(a) NULL
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif



#endif /* __TEXMEMORY_H__ */
/*$T OBJsave.h GC!1.55 01/07/00 16:53:38 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __OBJSAVE_H__
#define __OBJSAVE_H__


#include "BASe/BAStypes.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

extern LONG                 OBJ_gl_SaveOnlyImportedDataFromMad;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

extern ULONG        OBJ_ul_GameObject_Save(WOR_tdst_World *, OBJ_tdst_GameObject *, char *);
extern OBJ_tdst_GameObject  *OBJ_pst_DuplicateGameObjectFile(WOR_tdst_World *pst_World, BIG_INDEX _ul_FatFile, ULONG, char * asz_NewName, MATH_tdst_Vector * _pst_Pos );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __OBJSAVE_H__ */
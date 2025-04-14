/*$T WORsave.h GC!1.55 01/20/00 12:13:10 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Initialisations du module World
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __WORSAVE_H__
#define __WORSAVE_H__

#include "BASe/BAStypes.h"

#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Flags for save
 ***********************************************************************************************************************
 */

enum
{
	BAS_BITFLAG( WOR_C_IsABank, 0 ),
	BAS_BITFLAG( WOR_C_DoNotSaveNetwork, 1 ),
	BAS_BITFLAG( WOR_C_SaveJustWolAndGol, 2 ),
	BAS_BITFLAG( WOR_C_SaveSelected, 3 ),
};

	/*$4
 ***********************************************************************************************************************
    Svae a world
 ***********************************************************************************************************************
 */

LONG    WOR_l_World_SaveWithFileName(WOR_tdst_World *, char *, char *, LONG);
LONG    WOR_l_World_Save(WOR_tdst_World *, unsigned int flags);

void    WOR_GetGroPath(WOR_tdst_World *, char *);
void    WOR_GetGroPathWithKey(BIG_KEY _ul_WorldKey, char *_sz_Path);
void    WOR_GetGrmPath(WOR_tdst_World *, char *);
void    WOR_GetGrmPathWithKey(BIG_KEY _ul_WorldKey, char *_sz_Path);
void    WOR_GetGaoPath(WOR_tdst_World *, char *);
void    WOR_GetGaoPathWithKey(BIG_KEY _ul_WorldKey, char *);
void    WOR_GetPath(WOR_tdst_World *, char *);
void    WOR_GetPathWithKey(BIG_KEY _ul_WorldKey, char *);
void    WOR_GetSubPath(WOR_tdst_World *, char *, char *);
void    WOR_GetSubPathWithKey(BIG_KEY _ul_WorldKey, char *_sz_SubPath, char *_sz_Path);void    WOR_CheckName(WOR_tdst_World *);
void    WOR_CheckName(WOR_tdst_World *);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __WORSAVE_H__ */
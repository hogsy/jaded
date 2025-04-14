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

#define WOR_C_IsABank           0x00000001
#define WOR_C_DoNotSaveNetwork  0x00000002
#define WOR_C_SaveJustWolAndGol	0x00000004

/*$4
 ***********************************************************************************************************************
    Svae a world
 ***********************************************************************************************************************
 */

LONG    WOR_l_World_SaveWithFileName(WOR_tdst_World *, char *, char *, LONG);
LONG    WOR_l_World_Save(WOR_tdst_World *);
LONG    WOR_l_World_Save_ONLYSELECTED( WOR_tdst_World * ); // showin added
LONG    WOR_l_World_SaveWithFileName_SELECTED_ONLY( WOR_tdst_World *, char *, char *, LONG ); // showin added

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
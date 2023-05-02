/*$T WORload.h GC!1.71 02/18/00 14:52:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __WORLOAD_H__
#define __WORLOAD_H__

#include "BASe/BAStypes.h"

#include "BIGfiles/BIGkey.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Loading a world
 ***********************************************************************************************************************
 */

void                    WOR_World_JustAfterLoadObject(WOR_tdst_World *, OBJ_tdst_GameObject *, BOOL, BOOL);
void                    WOR_vComputeDX8FriendlyData(WOR_tdst_World *_pst_Dest);
void                    WOR_World_ResolveAIRef(WOR_tdst_World *);
void                    WOR_World_AttachGameObject(WOR_tdst_World *, OBJ_tdst_GameObject *);
WOR_tdst_World          *WOR_pst_World_Load(WOR_tdst_World *, BIG_KEY, BOOL);

// Droolie start
void WOR_World_AddRefBeforeCheckGroup( WOR_tdst_World *_pst_World );
void WOR_World_CheckFathersOfWorld( WOR_tdst_World * );
void WOR_World_CheckGroupOfWorld( WOR_tdst_World * );
// Droolie end

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern BOOL             WOR_gb_CanResolve;
extern LONG             WOR_gl_LoadJustWorldStruct;
extern WOR_tdst_World   *WOR_gpst_WorldToLoadIn;
extern BOOL             WOR_gb_HasAddedMaterial;
extern int              WOR_gi_CurrentConsole;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WORLOAD_H__ */

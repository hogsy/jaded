/*$T OBJinit.h GC! 1.081 06/21/00 16:45:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __OBJINI_H__
#define __OBJINI_H__

#include "BASe/BAStypes.h"

#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
#define OBJ_GameObject_RegSetName	0x00000001
#define OBJ_GameObject_RegInBank	0x00000002
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

extern void					OBJ_ResolveAIRef(OBJ_tdst_GameObject *);
extern void					OBJ_ResolveAIRefSecondPass(OBJ_tdst_GameObject *);
extern void					OBJ_Reinit(OBJ_tdst_GameObject *);
extern void					OBJ_SetInactive(OBJ_tdst_GameObject *);
extern OBJ_tdst_GameObject	*OBJ_GameObject_Create(ULONG);
extern OBJ_tdst_GameObject	*OBJ_GameObject_Duplicate(WOR_tdst_World *, OBJ_tdst_GameObject *, BOOL, BOOL, MATH_tdst_Vector *, ULONG, char * asz_NewName);
extern void					OBJ_GameObject_Remove_actual( OBJ_tdst_GameObject *, char, const char *caller_file, int caller_line );
extern void					OBJ_GameObject_CreateExtendedIfNot(OBJ_tdst_GameObject *);
extern void					OBJ_GameObject_CreateExtended(OBJ_tdst_GameObject *);
extern void					OBJ_GameObject_AllocateGizmo(OBJ_tdst_GameObject *, ULONG, BOOL);

#define OBJ_GameObject_Remove( pst_Object, _c_DecGroRef, __FILE__, __LINE__ )

#ifndef NDEBUG
#define OBJ_GameObject_Check( pst_Object ) \
	assert( pst_Object->obj_FreeRecord.magic == OBJ_LIVE_MAGIC );
#else
#define OBJ_GameObject_Check( pst_Object )
#endif

#if defined(_XENON_RENDER)
void                        OBJ_UpdateXenonVisu(GRO_tdst_Visu* _pst_DestVisu, GRO_tdst_Visu* _pst_SrcVisu, BOOL _b_Copy);
#endif

#ifdef Active_CloneListe

extern void					OBJ_GameObject_UpdateCloneListe();

extern void					OBJ_GameObject_AddLightClone(LightCloneListeGao **List,OBJ_tdst_GameObject *);
extern void					OBJ_GameObject_RemoveAllLightClone(OBJ_tdst_GameObject *obj);

void StockCloneLight(OBJ_tdst_GameObject *Obj);
#endif

#ifdef ACTIVE_EDITORS
void						OBJ_GameObject_BuildName(char *, char *);
void						OBJ_GameObject_SetName(OBJ_tdst_GameObject *, char *);
void						OBJ_GameObject_RegisterWithName(OBJ_tdst_GameObject *, ULONG, char *, LONG);
void                        OBJ_GameObject_Rename(OBJ_tdst_GameObject *pst_GO, char *asz_Name);
void                        OBJ_GameObject_MoveToWorld(OBJ_tdst_GameObject *pst_GO, BIG_KEY _ul_WorldKey);

#else
#define OBJ_GameObject_SetName(a, b)
#define OBJ_GameObject_BuildName(a, b)
#define OBJ_GameObject_RegisterWithName(a, b, c, d)
#endif /* ACTIVE_EDITORS */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __OBJINI_H__ */

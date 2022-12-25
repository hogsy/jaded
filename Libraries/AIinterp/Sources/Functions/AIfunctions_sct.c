/*$T AIfunctions_sct.c GC 1.139 03/25/04 09:53:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/OBJects/OBJ.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJorient.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SCT_SetMode_C(int on)
{
	SCT_gul_Flags = on;
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_SetMode(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_SCT_SetMode_C(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SCT_SetFlags_C(OBJ_tdst_GameObject *pst_GO, int id, int on, int off)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return;
	pst_World->ast_AllSectos[id].ul_Flags &= ~off;
	pst_World->ast_AllSectos[id].ul_Flags |= on;
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_SetFlags(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id, on, off;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	off = AI_PopInt();
	on = AI_PopInt();
	id = AI_PopInt();
	AI_Check(id > 0 && id < WOR_C_MaxSecto, "Bad sector ID");
	AI_EvalFunc_SCT_SetFlags_C(pst_GO, id, on, off);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SCT_SetCurrent_C(OBJ_tdst_GameObject *pst_GO, int what)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
    WOR_tdst_View   *pst_View;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return;
    pst_View = (pst_World->pst_CurrentView ? pst_World->pst_CurrentView : pst_World->pst_View);
	if(!pst_View) return;
	if(!pst_View->pst_Father) return;
	WOR_SetCurrentSector(pst_World, what, TRUE);

	switch(SCT_gul_Flags)
	{
	case SCT_MODE_Cam:
	default:
		MATH_CopyVector(&pst_World->st_SectorLastPos, &pst_View->pst_Father->pst_GlobalMatrix->T);
		break;

	case SCT_MODE_Main0:
		pst_GO = AI_gpst_MainActors[0];
		if(pst_GO) MATH_CopyVector(&pst_World->st_SectorLastPos, OBJ_pst_GetAbsolutePosition(pst_GO));
		break;

	case SCT_MODE_Main1:
		pst_GO = AI_gpst_MainActors[1];
		if(pst_GO) MATH_CopyVector(&pst_World->st_SectorLastPos, OBJ_pst_GetAbsolutePosition(pst_GO));
		break;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_SetCurrent(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	id = AI_PopInt();
	AI_Check(id > 0 && id < WOR_C_MaxSecto, "Bad sector ID");
	AI_EvalFunc_SCT_SetCurrent_C(pst_GO, id);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SCT_GetCurrent_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
    WOR_tdst_View   *pst_View;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return -1;
    pst_View = (pst_World->pst_CurrentView ? pst_World->pst_CurrentView : pst_World->pst_View);
	if(!pst_View) return -1;
	if(!pst_View->pst_Father) return -1;
	return pst_World->ul_CurrentSector;
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_GetCurrent(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_SCT_GetCurrent_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SCT_ResetAll_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
    WOR_tdst_View   *pst_View;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return;
    pst_View = (pst_World->pst_CurrentView ? pst_World->pst_CurrentView : pst_World->pst_View);
	if(!pst_View) return;
	if(!pst_View->pst_Father) return;

	WOR_NoSectorVisAct(pst_World);

	/* Recompute active objects, SnP and Views next frame */
	pst_World->b_ForceActivationRefresh = TRUE;
	pst_World->b_ForceVisibilityRefresh = TRUE;
	pst_World->b_ForceBVRefresh = TRUE;
	pst_World->ul_CurrentSector = 0xFFFFFFFF;
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_ResetAll(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_SCT_ResetAll_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SCT_AddCurrent_C(OBJ_tdst_GameObject *pst_GO, int what)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
    WOR_tdst_View   *pst_View;
	int				val;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return;
    pst_View = (pst_World->pst_CurrentView ? pst_World->pst_CurrentView : pst_World->pst_View);
	if(!pst_View) return;
	if(!pst_View->pst_Father) return;

	/* Treat current */
	if(pst_World->ast_AllSectos[what].ul_Flags & WOR_CF_SectorVisible) return;

	pst_World->ast_AllSectos[what].ul_Flags |= WOR_CF_SectorVisible;
	pst_World->ast_AllSectos[what].ul_Flags |= WOR_CF_SectorActive;
	for(i = 0; i < WOR_C_MaxSectoRef; i++)
	{
		val = pst_World->ast_AllSectos[what].auc_RefVis[i];
		if(val) pst_World->ast_AllSectos[val].ul_Flags |= WOR_CF_SectorVisible;
		val = pst_World->ast_AllSectos[what].auc_RefAct[i];
		if(val) pst_World->ast_AllSectos[val].ul_Flags |= WOR_CF_SectorActive;
	}

	/* Recompute active objects, SnP and Views next frame */
	pst_World->b_ForceActivationRefresh = TRUE;
	pst_World->b_ForceVisibilityRefresh = TRUE;
	pst_World->b_ForceBVRefresh = TRUE;
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_AddCurrent(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_SCT_AddCurrent_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SCT_Add_C(OBJ_tdst_GameObject *pst_GO, int what, int _flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
    WOR_tdst_View   *pst_View;
	int				val;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return;
    pst_View = (pst_World->pst_CurrentView ? pst_World->pst_CurrentView : pst_World->pst_View);
	if(!pst_View) return;
	if(!pst_View->pst_Father) return;
    if(!_flags) return;

    // If added sector is already active and visible, return
    if ((pst_World->ast_AllSectos[what].ul_Flags & WOR_CF_SectorVisible) && 
        (pst_World->ast_AllSectos[what].ul_Flags & WOR_CF_SectorActive)) return;

	/* Treat current */
    if (_flags & WOR_CF_SectorVisible)
    	pst_World->ast_AllSectos[what].ul_Flags |= WOR_CF_SectorVisible;

    if (_flags & WOR_CF_SectorActive)
    	pst_World->ast_AllSectos[what].ul_Flags |= WOR_CF_SectorActive;

	for(i = 0; i < WOR_C_MaxSectoRef; i++)
	{
        if (_flags & WOR_CF_SectorVisible)
        {
    		val = pst_World->ast_AllSectos[what].auc_RefVis[i];
	    	if(val) pst_World->ast_AllSectos[val].ul_Flags |= WOR_CF_SectorVisible;
        }
        if (_flags & WOR_CF_SectorActive)
        {
    		val = pst_World->ast_AllSectos[what].auc_RefAct[i];
	    	if(val) pst_World->ast_AllSectos[val].ul_Flags |= WOR_CF_SectorActive;
        }
	}

	/* Recompute active objects, SnP and Views next frame */
	pst_World->b_ForceActivationRefresh = TRUE;
	pst_World->b_ForceVisibilityRefresh = TRUE;
	pst_World->b_ForceBVRefresh = TRUE;
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_Add(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int i1, i2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i2 = AI_PopInt();
	i1 = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_SCT_Add_C(pst_GO, i1, i2);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SCT_Remove_C(OBJ_tdst_GameObject *pst_GO, int what, int _flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
    WOR_tdst_View   *pst_View;
	int				val;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return;
    pst_View = (pst_World->pst_CurrentView ? pst_World->pst_CurrentView : pst_World->pst_View);
	if(!pst_View) return;
	if(!pst_View->pst_Father) return;
    if(!_flags) return;
    // If removed sector is already inactive and invisible, return
    if (!(pst_World->ast_AllSectos[what].ul_Flags & (WOR_CF_SectorVisible|WOR_CF_SectorActive))) return;

	/* Treat current */
    if (_flags & WOR_CF_SectorVisible)
    	pst_World->ast_AllSectos[what].ul_Flags &= ~WOR_CF_SectorVisible;

    if (_flags & WOR_CF_SectorActive)
    	pst_World->ast_AllSectos[what].ul_Flags &= ~WOR_CF_SectorActive;

	for(i = 0; i < WOR_C_MaxSectoRef; i++)
	{
        if (_flags & WOR_CF_SectorVisible)
        {
    		val = pst_World->ast_AllSectos[what].auc_RefVis[i];
	    	if(val) pst_World->ast_AllSectos[val].ul_Flags &= ~WOR_CF_SectorVisible;
        }
        if (_flags & WOR_CF_SectorActive)
        {
    		val = pst_World->ast_AllSectos[what].auc_RefAct[i];
	    	if(val) pst_World->ast_AllSectos[val].ul_Flags &= ~WOR_CF_SectorActive;
        }
	}

	/* Recompute active objects, SnP and Views next frame */
	pst_World->b_ForceActivationRefresh = TRUE;
	pst_World->b_ForceVisibilityRefresh = TRUE;
	pst_World->b_ForceBVRefresh = TRUE;
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_Remove(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int i1, i2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i2 = AI_PopInt();
	i1 = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_SCT_Remove_C(pst_GO, i1, i2);
	return ++_pst_Node;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SCT_GetOf_C(OBJ_tdst_GameObject *pst_GO, int *s1, int *s2, int *s3, int *s4)
{
	if(!pst_GO->pst_Extended) 
	{
		*s1 = *s2 = *s3 = *s4 = -1;
		return;
	}

	*s1 = pst_GO->pst_Extended->auc_Sectos[0];
	*s2 = pst_GO->pst_Extended->auc_Sectos[1];
	*s3 = pst_GO->pst_Extended->auc_Sectos[2];
	*s4 = pst_GO->pst_Extended->auc_Sectos[3];
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_GetOf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Arr1;
	AI_tdst_UnionVar	s_Val1;
	AI_tdst_PushVar		st_Arr2;
	AI_tdst_UnionVar	s_Val2;
	AI_tdst_PushVar		st_Arr3;
	AI_tdst_UnionVar	s_Val3;
	AI_tdst_PushVar		st_Arr4;
	AI_tdst_UnionVar	s_Val4;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVar(&s_Val4, &st_Arr4);
	AI_PopVar(&s_Val3, &st_Arr3);
	AI_PopVar(&s_Val2, &st_Arr2);
	AI_PopVar(&s_Val1, &st_Arr1);
	AI_EvalFunc_SCT_GetOf_C(pst_GO, (int *) st_Arr1.pv_Addr, (int *) st_Arr2.pv_Addr, (int *) st_Arr3.pv_Addr, (int *) st_Arr4.pv_Addr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SCT_SetOf_C(OBJ_tdst_GameObject *pst_GO, int i1, int i2, int i3, int i4)
{
	if (pst_GO && pst_GO->pst_Extended )
	{
		pst_GO->pst_Extended->auc_Sectos[0] = i1;
		pst_GO->pst_Extended->auc_Sectos[1] = i2;
		pst_GO->pst_Extended->auc_Sectos[2] = i3;
		pst_GO->pst_Extended->auc_Sectos[3] = i4;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_SetOf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int i1, i2, i3, i4;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i4 = AI_PopInt();
	i3 = AI_PopInt();
	i2 = AI_PopInt();
	i1 = AI_PopInt();
	AI_EvalFunc_SCT_SetOf_C(pst_GO, i1, i2, i3, i4);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SCT_ObjActive_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return 0;
	return WOR_SectoObjActive(pst_World, pst_GO);
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_ObjActive(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_SCT_ObjActive_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SCT_ObjVisible_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return 0;
	return WOR_SectoObjVisible(pst_World, pst_GO);
}
/**/
AI_tdst_Node *AI_EvalFunc_SCT_ObjVisible(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_SCT_ObjVisible_C(pst_GO));
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

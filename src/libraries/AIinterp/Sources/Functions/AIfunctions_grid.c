/*$T AIfunctions_grid.c GC! 1.100 04/02/01 14:45:10 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIstack.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_vars.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJmain.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef RASTERS_ON
PRO_tdst_TrameRaster	ENG_gpst_RasterEng_ShortWayGrid;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int						GRID_gi_Current = 0;
#define M_Grid()		GRID_gi_Current == 0 ? pst_World->pst_Grid : pst_World->pst_Grid1
float					GRI_gaf_Ponderations[16] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
float					GRI_gaf_PonderationsDyn[4] = { 1, 1, 1, 1 };

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRIDSetPond_C(float *val)
{
	L_memcpy(GRI_gaf_Ponderations, val, sizeof(GRI_gaf_Ponderations));
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDSetPond(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_GRIDSetPond_C((float *) st_Var.pv_Addr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRIDSetPondDyn_C(float *val)
{
	L_memcpy(GRI_gaf_PonderationsDyn, val, sizeof(GRI_gaf_PonderationsDyn));
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDSetPondDyn(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_GRIDSetPondDyn_C((float *) st_Var.pv_Addr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_GRIDGetPond_C(int index)
{
	return GRI_gaf_Ponderations[index];
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDGetPond(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_GRIDGetPond_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_GRIDGetPondDyn_C(int index)
{
	return GRI_gaf_PonderationsDyn[index];
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDGetPondDyn(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_GRIDGetPondDyn_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRIDCurrentSet_C(int cur)
{
	GRID_gi_Current = cur;
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDCurrentSet(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_GRIDCurrentSet_C(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GRIDHas_C(OBJ_tdst_GameObject *pst_GO, int cur)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GRID_tdst_World *pst_Grid;
	int mem;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	mem = GRID_gi_Current;
	GRID_gi_Current = cur;
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = M_Grid();
	GRID_gi_Current = mem;
	if(!pst_Grid) return 0;
	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDHas(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *pst_GO;
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_GRIDHas_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRIDSetPosAll_C(OBJ_tdst_GameObject *pst_GO, int grid, MATH_tdst_Vector *pst_Vec)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GRID_tdst_World *pst_Grid;
	int				save;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	save = GRID_gi_Current;
	GRID_gi_Current = grid;
	pst_Grid = M_Grid();
	pst_Grid->f_MinXTotal = pst_Vec->x;
	pst_Grid->f_MinYTotal = pst_Vec->y;
	GRID_gi_Current = save;
#ifdef ACTIVE_EDITORS
	if(pst_Grid->p_GameObject) OBJ_SetAbsolutePosition((OBJ_tdst_GameObject*)pst_Grid->p_GameObject, pst_Vec);
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDSetPosAll(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					grid;
	MATH_tdst_Vector	st_Vec;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Vec);
	grid = AI_PopInt();
	AI_EvalFunc_GRIDSetPosAll_C(pst_GO, grid, &st_Vec);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRIDSetCapa_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Vec, int i_Capa)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GRID_tdst_World *pst_Grid;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = M_Grid();
	AI_Check(pst_Grid, "Current world has no grid");
	GRI_SetCapa(pst_Grid, pst_Vec, (char) i_Capa);
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDSetCapa(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Capa;
	MATH_tdst_Vector	st_Vec;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Capa = AI_PopInt();
	AI_PopVector(&st_Vec);
	AI_EvalFunc_GRIDSetCapa_C(pst_GO, &st_Vec, i_Capa);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRIDAddCapa_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pvec, int i_Capa)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GRID_tdst_World *pst_Grid;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = M_Grid();
	AI_Check(pst_Grid, "Current world has no grid");
	GRI_AddCapa(pst_Grid, pvec, (char) i_Capa, NULL);
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDAddCapa(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Vec;
	int					i_Capa;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Capa = AI_PopInt();
	AI_PopVector(&st_Vec);
	AI_EvalFunc_GRIDAddCapa_C(pst_GO, &st_Vec, i_Capa);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GRIDGetCapa_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Vec)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GRID_tdst_World *pst_Grid;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = M_Grid();
	AI_Check(pst_Grid, "Current world has no grid");
	return GRI_i_GetCapa(pst_Grid, pst_Vec);
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDGetCapa(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Vec;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Vec);
	AI_PushInt(AI_EvalFunc_GRIDGetCapa_C(pst_GO, &st_Vec));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GRIDShortWay_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*p_Addr,
	MATH_tdst_Vector	*pst_Src,
	MATH_tdst_Vector	*pst_Dest,
	int					i_Born,
	float				f_Max,
	int					i_Par1,
	int					i_Par2
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static int		i_Raster = 0;
	WOR_tdst_World	*pst_World;
	GRID_tdst_World *pst_Grid;
	int				ires;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = M_Grid();
	AI_Check(pst_Grid, "Current world has no grid");

	if(!i_Raster)
	{
		PRO_FirstInitTrameRaster
		(
			&ENG_gpst_RasterEng_ShortWayGrid,
			"Trame Loop",
			"Game Engine",
			"GRID Short Way",
			PRO_E_Time,
			0
		);
		i_Raster = 1;
	}

	PRO_StartTrameRaster(&ENG_gpst_RasterEng_ShortWayGrid);
	ires = GRID_ShortestPath
		(
			pst_GO,
			pst_Grid,
			pst_Src,
			pst_Dest,
			p_Addr,
			i_Born,
			f_Max,
			i_Par1,
			i_Par2
		);
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_ShortWayGrid);
	return ires;
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDShortWay(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	float				f_Max;
	int					i_Born, i_Par1, i_Par2;
	MATH_tdst_Vector	st_Src, st_Dest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Par2 = AI_PopInt();
	i_Par1 = AI_PopInt();
	f_Max = AI_PopFloat();
	i_Born = AI_PopInt();
	AI_PopVector(&st_Dest);
	AI_PopVector(&st_Src);
	AI_PopVar(&Val, &st_Var);

	AI_PushInt
	(
		AI_EvalFunc_GRIDShortWay_C
			(
				pst_GO,
				(MATH_tdst_Vector *) st_Var.pv_Addr,
				&st_Src,
				&st_Dest,
				i_Born,
				f_Max,
				i_Par1,
				i_Par2
			)
	);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRIDPosGet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Src, MATH_tdst_Vector *pst_Dest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				x, y;
	WOR_tdst_World	*pst_World;
	GRID_tdst_World *pst_Grid;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = M_Grid();
	AI_Check(pst_Grid, "Current world has no grid");

	MATH_CopyVector(pst_Dest, pst_Src);
	GRI_3Dto2D(pst_Grid, pst_Dest, &x, &y);
	pst_Dest->x = (float) x + (GRID_CASE / 2.0f) + pst_Grid->f_MinXTotal;
	pst_Dest->y = (float) y + (GRID_CASE / 2.0f) + pst_Grid->f_MinYTotal;
	pst_Dest->z = 0.0f;
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDPosGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Src, st_Dest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Src);
	AI_EvalFunc_GRIDPosGet_C(pst_GO, &st_Src, &st_Dest);

	AI_PushVector(&st_Dest);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRIDSetCenter_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pvec)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				x, y;
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	AI_Check(pst_World->pst_Grid || pst_World->pst_Grid1, "Current world has no grid");

	if(pst_World->pst_Grid)
	{
		GRI_3Dto2D(pst_World->pst_Grid, pvec, &x, &y);
		GRID_SetRealCenter(pst_World->pst_Grid, x, y);
	}

	if(pst_World->pst_Grid1)
	{
		GRI_3Dto2D(pst_World->pst_Grid1, pvec, &x, &y);
		GRID_SetRealCenter(pst_World->pst_Grid1, x, y);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDSetCenter(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Src;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Src);
	AI_EvalFunc_GRIDSetCenter_C(pst_GO, &st_Src);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GRIDIsIn_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	int				x, y;
	GRID_tdst_World *pst_Grid;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = M_Grid();
	AI_Check(pst_Grid, "Current world has no grid");

	if(GRID_3Dto2D(pst_Grid, pst_Src, &x, &y)) return 1;
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDIsIn(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Src;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Src);
	AI_PushInt(AI_EvalFunc_GRIDIsIn_C(pst_GO, &st_Src));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GRIDTagBox_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pos, MATH_tdst_Vector *sight, MATH_tdst_Vector *pst_1, MATH_tdst_Vector *pst_2, int i_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GRID_tdst_World *pst_Grid;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = M_Grid();
	AI_Check(pst_Grid, "Current world has no grid");
	return GRI_TagBox(pst_Grid, pos, sight, pst_1, pst_2, i_Val);
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDTagBox(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Val;
	MATH_tdst_Vector	st_1, st_2;
	MATH_tdst_Vector	pos, sight;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Val = AI_PopInt();
	AI_PopVector(&st_2);
	AI_PopVector(&st_1);
	AI_PopVector(&sight);
	AI_PopVector(&pos);
	AI_PushInt(AI_EvalFunc_GRIDTagBox_C(pst_GO, &pos, &sight, &st_1, &st_2, i_Val));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GRIDSmoothWay_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *paddr, int i_Num, float maxpound)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GRID_tdst_World *pst_Grid;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = M_Grid();
	AI_Check(pst_Grid, "Current world has no grid");
	return GRI_SmoothWay(pst_Grid, paddr, i_Num, maxpound);
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDSmoothWay(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Num;
	float				maxp;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	maxp = AI_PopFloat();
	i_Num = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_PushInt(AI_EvalFunc_GRIDSmoothWay_C(pst_GO, (MATH_tdst_Vector *) st_Var.pv_Addr, i_Num, maxp));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRIDCptDest_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*psrc,
	float				f_Dist,
	MATH_tdst_Vector	*pdest
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
//	float			f_Dist1;
	GRID_tdst_World *pst_Grid;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = M_Grid();
	AI_Check(pst_Grid, "Current world has no grid");

//	MATH_SubVector(pdest, psrc, OBJ_pst_GetAbsolutePosition(pst_GO));
//	f_Dist1 = MATH_f_NormVector(pdest);
//	if(!f_Dist1) return;
//	if(f_Dist1 < f_Dist) f_Dist = f_Dist1;
//	if(!f_Dist) f_Dist = 1;
//	MATH_SetNormVector(pdest, pdest, f_Dist);
//	MATH_AddVector(pdest, pdest, OBJ_pst_GetAbsolutePosition(pst_GO));

	MATH_CopyVector(pdest, psrc);
	if(!GRI_i_ComputeDest(pst_Grid, pdest)) pdest->x = pdest->y = pdest->z = 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_GRIDCptDest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Vec;
	MATH_tdst_Vector	st_Src;
	OBJ_tdst_GameObject *pst_GO;
	float				f_Dist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Dist = AI_PopFloat();
	AI_PopVector(&st_Src);

	AI_EvalFunc_GRIDCptDest_C(pst_GO, &st_Src, f_Dist, &st_Vec);
	AI_PushVector(&st_Vec);
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

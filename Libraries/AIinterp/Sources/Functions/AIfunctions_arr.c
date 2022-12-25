/*$T AIfunctions_arr.c GC! 1.100 07/06/01 11:15:50 */


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
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "BASe/CLIbrary/CLIstr.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
int			*ARR_gpi_Global = NULL;

#ifdef ACTIVE_EDITORS
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGio.h"

extern char *BIG_pc_ReadFileTmp(ULONG, ULONG *);

BOOL		SpecialFlags_b_Init = TRUE;
char		SpecialFlags_Text[5000][500];

typedef struct
{
	OBJ_tdst_GameObject *pst_GO;
	int					i_Flag;
	BOOL				b_Status;
} SpecialFlags_LastFrameTests;

/* -- SpecialFlags TRACER: Variables -- */
SpecialFlags_LastFrameTests SpecialFlags_TestTable[1000];
int							SpecialFlags_NumTests = 0;

/* -- SpecialFlags TRACER: Variables -- */
ULONG						SpecialFlags_ul_CurrentLoop = -1;
ULONG						SpecialFlags_ul_LastInfoLoop = 0;

/* -- SpecialFlags TRACER: Prototypes -- */
void						SpecialFlags_Reset(void);
void						SpecialFlags_Init(void);

void						SpecialFlags_Test(int *, int, OBJ_tdst_GameObject *);
void						SpecialFlags_Set(int *, int, OBJ_tdst_GameObject *);
void						SpecialFlags_Clear(int *, int, OBJ_tdst_GameObject *);

BOOL						SpecialFlags_Skip(OBJ_tdst_GameObject *, int);
int							*SpecialFlags_Array = NULL;

/* -- SpecialFlags TRACER: STATIC -- */
int							SpecialFlags_FlagToTrace = 0;
int							SpecialFlags_b_Reset = 0;
int							SpecialFlags_b_ActiveTracer = 0;

extern ULONG				ENG_gul_Loop;

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRObjNearestOfPoint_C(OBJ_tdst_GameObject **pobj, int i_Size, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Pos, st_Dist;
	int					i_Best;
	OBJ_tdst_GameObject *pst_GO;
	float				fNorm, fBest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Best = 0;
    i_Size--;
	fBest = Cf_Infinit;
	for(; i_Size >= 0; i_Size--)
	{
		pst_GO = ((OBJ_tdst_GameObject **) pobj)[i_Size];
		if(pst_GO && ((int) pst_GO != -1))
		{
			pst_Pos = OBJ_pst_GetAbsolutePosition(pst_GO);
			MATH_SubVector(&st_Dist, v, pst_Pos);
			fNorm = MATH_f_NormVector(&st_Dist);
			if(fNorm < fBest)
			{
				fBest = fNorm;
				i_Best = i_Size;
			}
		}
	}

	return i_Best;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRObjNearestOfPoint(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v;
	int					i_Size;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	v = AI_PopVectorPtr();
	i_Size = AI_PopInt();
	AI_PopVar(&Val, &st_Var);

	AI_PushInt(AI_EvalFunc_ARRObjNearestOfPoint_C((OBJ_tdst_GameObject **) st_Var.pv_Addr, i_Size, v));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRObjBestSight_C
(
	OBJ_tdst_GameObject **pobj,
	int					i_Size,
	MATH_tdst_Vector	*vpos,
	MATH_tdst_Vector	*v1,
	float				fTol
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Sight;
	int					i_Best;
	OBJ_tdst_GameObject *pst_GO;
	float				fNorm, fBest, fDist;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_NormalizeVector(&v, v1);
	i_Size--;
	i_Best = i_Size;

	fBest = -Cf_Infinit;
	for(; i_Size >= 0; i_Size--)
	{
		pst_GO = pobj[i_Size];
		if(pst_GO && ((int) pst_GO != -1))
		{
			MATH_SubVector(&st_Sight, OBJ_pst_GetAbsolutePosition(pst_GO), vpos);
			if(MATH_b_NulVector(&st_Sight)) continue;
			if(fTol) fDist = MATH_f_NormVector(&st_Sight);
			MATH_NormalizeVector(&st_Sight, &st_Sight);
			fNorm = MATH_f_DotProduct(&st_Sight, &v);
			if(fTol) fNorm -= fDist / fTol;
			if(fNorm > fBest)
			{
				fBest = fNorm;
				i_Best = i_Size;
			}
		}
	}

	return i_Best;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRObjBestSight(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	float				fTol;
	MATH_tdst_Vector	v, vpos;
	int					i_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	fTol = AI_PopFloat();
	AI_PopVector(&v);
	AI_PopVector(&vpos);
	i_Size = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_PushInt(AI_EvalFunc_ARRObjBestSight_C((OBJ_tdst_GameObject **) st_Var.pv_Addr, i_Size, &vpos, &v, fTol));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRObjBestSightBVCenter_C
(
	OBJ_tdst_GameObject **paddr,
	int					i_Size,
	MATH_tdst_Vector	*vpos,
	MATH_tdst_Vector	*v,
	float				fTol
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Sight;
	int					i_Best;
	float				fNorm, fBest, fDist;
	MATH_tdst_Vector	st_Center;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_NormalizeVector(v, v);
	i_Size--;
	i_Best = i_Size;

	fBest = -Cf_Infinit;
	for(; i_Size >= 0; i_Size--)
	{
		pst_GO = (paddr)[i_Size];
		if(pst_GO && ((int) pst_GO != -1))
		{
			OBJ_BV_ComputeCenter(pst_GO, &st_Center);
			MATH_SubVector(&st_Sight, &st_Center, vpos);
			if(fTol) fDist = MATH_f_NormVector(&st_Sight);
			MATH_NormalizeVector(&st_Sight, &st_Sight);
			fNorm = MATH_f_DotProduct(&st_Sight, v);
			if(fTol) fNorm -= fDist / fTol;
			if(fNorm > fBest)
			{
				fBest = fNorm;
				i_Best = i_Size;
			}
		}
	}

	return i_Best;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRObjBestSightBVCenter(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	float				fTol;
	MATH_tdst_Vector	v, vpos;
	int					i_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	fTol = AI_PopFloat();
	AI_PopVector(&v);
	AI_PopVector(&vpos);
	i_Size = AI_PopInt();
	AI_PopVar(&Val, &st_Var);

	AI_PushInt(AI_EvalFunc_ARRObjBestSightBVCenter_C((OBJ_tdst_GameObject **) st_Var.pv_Addr, i_Size, &vpos, &v, fTol));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ARRObjCopy_C(OBJ_tdst_GameObject **pdest, OBJ_tdst_GameObject **psrc, int i_Num)
{
	L_memcpy(pdest, psrc, i_Num * sizeof(void *));
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRObjCopy(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_VarDest, st_VarSrc;
	AI_tdst_UnionVar	ValDest, ValSrc;
	int					i_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Num = AI_PopInt();
	AI_PopVar(&ValSrc, &st_VarSrc);
	AI_PopVar(&ValDest, &st_VarDest);
	AI_EvalFunc_ARRObjCopy_C
	(
		(OBJ_tdst_GameObject **) st_VarDest.pv_Addr,
		(OBJ_tdst_GameObject **) st_VarSrc.pv_Addr,
		i_Num
	);

	return ++_pst_Node;
}

extern void AI_EvalFunc_OBJ_InfoPhotoPivotGet_C(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRObjBestSightInfoPhoto_C
(
	OBJ_tdst_GameObject **paddr,
	int					i_Size,
	MATH_tdst_Vector	*vpos,
	MATH_tdst_Vector	*v,
	float				fTol
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Sight;
	int					i_Best;
	float				fDot, fDist;
	float				fBestCos, fBestDist;
	float				Angle, fCosThresHold;
	MATH_tdst_Vector	st_Center;
	OBJ_tdst_GameObject *pst_GO;
	int					i_SaveSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_SaveSize = i_Size;
	MATH_NormalizeVector(v, v);
	i_Size--;
	i_Best = i_Size;

	fBestCos = -Cf_Infinit;
	for(; i_Size >= 0; i_Size--)
	{
		pst_GO = (paddr)[i_Size];
		if(pst_GO && ((int) pst_GO != -1))
		{
			AI_EvalFunc_OBJ_InfoPhotoPivotGet_C(pst_GO, &st_Center);
			MATH_SubVector(&st_Sight, &st_Center, vpos);
			MATH_NormalizeVector(&st_Sight, &st_Sight);
			fDot = MATH_f_DotProduct(&st_Sight, v);
			
			if(fDot > fBestCos)
				fBestCos = fDot;
		}
	}


	Angle = fAcos(fBestCos);

	if(fTol)
		Angle += (fTol * Cf_Pi / 180.0f);
	else
		Angle += (5.0f * Cf_Pi / 180.0f);

	fCosThresHold = fCos(Angle);

	fBestDist = Cf_Infinit;

	i_Size = i_SaveSize;
	i_Size--;
	for(; i_Size >= 0; i_Size--)
	{
		pst_GO = (paddr)[i_Size];
		if(pst_GO && ((int) pst_GO != -1))
		{
			AI_EvalFunc_OBJ_InfoPhotoPivotGet_C(pst_GO, &st_Center);
			MATH_SubVector(&st_Sight, &st_Center, vpos);
			fDist = MATH_f_NormVector(&st_Sight);

			MATH_NormalizeVector(&st_Sight, &st_Sight);
			fDot = MATH_f_DotProduct(&st_Sight, v);
			
			if((fDot > fCosThresHold) && (fDist < fBestDist))
			{
				fBestDist = fDist;
				i_Best = i_Size;
			}
		}
	}

	return i_Best;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRObjBestSightInfoPhoto(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	float				fTol;
	MATH_tdst_Vector	v, vpos;
	int					i_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	fTol = AI_PopFloat();
	AI_PopVector(&v);
	AI_PopVector(&vpos);
	i_Size = AI_PopInt();
	AI_PopVar(&Val, &st_Var);

	AI_PushInt(AI_EvalFunc_ARRObjBestSightInfoPhoto_C((OBJ_tdst_GameObject **) st_Var.pv_Addr, i_Size, &vpos, &v, fTol));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRSortByFlags_C
(
	OBJ_tdst_GameObject **pdest,
	int					i_Num,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					end, i;
	OBJ_tdst_GameObject *p_Test, *p_Swap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	end = i_Num;
	i = 0;
	while(i < end)
	{
		p_Test = pdest[i];
		if(!p_Test) continue;
		if(!OBJ_b_TestFlag(p_Test, _ul_OnFlags, _ul_OffFlags, _ul_FlagID))
		{
			end--;
			p_Swap = pdest[i];
			pdest[i] = pdest[end];
			pdest[end] = p_Swap;
			continue;
		}

		i++;
	}

	return end;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRSortByFlags(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_VarSrc;
	AI_tdst_UnionVar	ValSrc;
	int					i_Num;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	i_Num = AI_PopInt();
	AI_PopVar(&ValSrc, &st_VarSrc);

	AI_PushInt
	(
		AI_EvalFunc_ARRSortByFlags_C
			(
				(OBJ_tdst_GameObject **) st_VarSrc.pv_Addr,
				i_Num,
				ul_OnFlags,
				ul_OffFlags,
				ul_FlagID
			)
	);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRObjSearch_C(OBJ_tdst_GameObject **ppst_GO, int i_Num, OBJ_tdst_GameObject *pst_GO)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < i_Num; i++)
	{
		if(ppst_GO[i] == pst_GO) return i;
	}

	return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRObjSearch(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_VarSrc;
	AI_tdst_UnionVar	ValSrc;
	int					i_Num;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = AI_PopGameObject();
	i_Num = AI_PopInt();
	AI_PopVar(&ValSrc, &st_VarSrc);

	AI_PushInt(AI_EvalFunc_ARRObjSearch_C((OBJ_tdst_GameObject **) st_VarSrc.pv_Addr, i_Num, pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_ARRObjCopyIdFlags(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_VarDest, st_VarSrc;
	AI_tdst_UnionVar	ValDest, ValSrc;
	int					i_Num, i_Set, i_NotSet;
	int					i_Src, i_Dst;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Set = AI_PopInt();
	i_NotSet = AI_PopInt();
	i_Num = AI_PopInt();
	AI_PopVar(&ValSrc, &st_VarSrc);
	AI_PopVar(&ValDest, &st_VarDest);

	for(i_Dst = 0, i_Src = 0; i_Src < i_Num; i_Src++)
	{
		pst_GO = ((OBJ_tdst_GameObject **) st_VarSrc.pv_Addr)[i_Src];
		if(!pst_GO) continue;
		if((pst_GO->ul_IdentityFlags & i_Set) && (!(pst_GO->ul_IdentityFlags & i_NotSet)))
		{
			((OBJ_tdst_GameObject **) st_VarSrc.pv_Addr)[i_Dst++] = pst_GO;
		}
	}

	AI_PushInt(i_Dst);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_ARRObjCopyCB(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_VarDest, st_VarSrc;
	AI_tdst_UnionVar	ValDest, ValSrc;
	int					i_Num, i_Set, i_NotSet;
	int					i_Src, i_Dst;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Set = AI_PopInt();
	i_NotSet = AI_PopInt();
	i_Num = AI_PopInt();
	AI_PopVar(&ValSrc, &st_VarSrc);
	AI_PopVar(&ValDest, &st_VarDest);

	for(i_Dst = 0, i_Src = 0; i_Src < i_Num; i_Src++)
	{
		pst_GO = ((OBJ_tdst_GameObject **) st_VarSrc.pv_Addr)[i_Src];
		if(!pst_GO) continue;
		if
		(
			((pst_GO->ul_StatusAndControlFlags & 0x0000FF00) & (i_Set & 0x0000FF00))
		&&	(!((pst_GO->ul_StatusAndControlFlags & 0x0000FF00) & (i_NotSet & 0x0000FF00)))
		)
		{
			((OBJ_tdst_GameObject **) st_VarSrc.pv_Addr)[i_Dst++] = pst_GO;
		}
	}

	AI_PushInt(i_Dst);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ARRBitChange_C(int *paddr, int i_Set)
{
	/*~~~*/
	int *p;
	/*~~~*/

	p = paddr + ((i_Set &~31) >> 5);
	if(*p & (1 << (i_Set & 31)))
		*p &= ~(1 << (i_Set & 31));
	else
		*p |= 1 << (i_Set & 31);
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRBitChange(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Arr;
	AI_tdst_UnionVar	s_Val;
	int					i_Set;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Set = AI_PopInt();
	AI_PopVar(&s_Val, &st_Arr);
	AI_EvalFunc_ARRBitChange_C((int *) st_Arr.pv_Addr, i_Set);

	return ++_pst_Node;
}

BOOL	AI_EvalFunc_ARRBitTest_C(int *, int i_Set);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ARRBitSet_C(int *paddr, int i_Set)
{
	/*~~~*/
	int *p;
	/*~~~*/

	p = paddr + ((i_Set &~31) >> 5);
	*p |= 1 << (i_Set & 31);
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRBitSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Arr;
	AI_tdst_UnionVar	s_Val;
	int					i_Set;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Set = AI_PopInt();
	AI_PopVar(&s_Val, &st_Arr);
#ifdef ACTIVE_EDITORS
	SpecialFlags_Array = (int *) st_Arr.pv_Addr;
	if(SpecialFlags_b_ActiveTracer)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_GO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		AI_M_GetCurrentObject(pst_GO);
		if(!SpecialFlags_Skip(pst_GO, i_Set)) SpecialFlags_Set((int *) st_Arr.pv_Addr, i_Set, pst_GO);
	}

#endif
	AI_EvalFunc_ARRBitSet_C((int *) st_Arr.pv_Addr, i_Set);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ARRBitClear_C(int *paddr, int i_Clear)
{
	/*~~~*/
	int *p;
	/*~~~*/

	p = paddr + ((i_Clear &~31) >> 5);
	*p &= ~(1 << (i_Clear & 31));
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRBitClear(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Arr;
	AI_tdst_UnionVar	s_Val;
	int					i_Clear;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Clear = AI_PopInt();
	AI_PopVar(&s_Val, &st_Arr);

#ifdef ACTIVE_EDITORS
	SpecialFlags_Array = (int *) st_Arr.pv_Addr;
	if(SpecialFlags_b_ActiveTracer)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_GO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		AI_M_GetCurrentObject(pst_GO);
		if(!SpecialFlags_Skip(pst_GO, i_Clear)) SpecialFlags_Clear((int *) st_Arr.pv_Addr, i_Clear, pst_GO);
	}

#endif
	AI_EvalFunc_ARRBitClear_C((int *) st_Arr.pv_Addr, i_Clear);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ARRGlobalIntFlagSet_C(int on)
{
	AI_Check(ARR_gpi_Global, "Global array not defined");
	AI_EvalFunc_ARRBitSet_C(ARR_gpi_Global, on);
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRGlobalIntFlagSet(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int on;
	/*~~~*/

	on = AI_PopInt();
#ifdef ACTIVE_EDITORS
	if(SpecialFlags_b_ActiveTracer)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_GO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		AI_M_GetCurrentObject(pst_GO);
		SpecialFlags_Set((int *) ARR_gpi_Global, on, pst_GO);
	}

#endif
	AI_EvalFunc_ARRGlobalIntFlagSet_C(on);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ARRGlobalIntFlagClear_C(int off)
{
	AI_Check(ARR_gpi_Global, "Global array not defined");
	AI_EvalFunc_ARRBitClear_C(ARR_gpi_Global, off);
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRGlobalIntFlagClear(AI_tdst_Node *_pst_Node)
{
	/*~~~~*/
	int off;
	/*~~~~*/

	off = AI_PopInt();
#ifdef ACTIVE_EDITORS
	if(SpecialFlags_b_ActiveTracer)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_GO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		AI_M_GetCurrentObject(pst_GO);
		SpecialFlags_Clear((int *) ARR_gpi_Global, off, pst_GO);
	}

#endif

	AI_EvalFunc_ARRGlobalIntFlagClear_C(off);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRBitTest_C(int *paddr, int i_Test)
{
	/*~~~*/
	int *p;
	/*~~~*/

	p = paddr + ((i_Test &~31) >> 5);
	if(*p & (1 << (i_Test & 31))) return 1;
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRBitTest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Arr;
	AI_tdst_UnionVar	s_Val;
	int					i_Test;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Test = AI_PopInt();
	AI_PopVar(&s_Val, &st_Arr);
	AI_PushInt(AI_EvalFunc_ARRBitTest_C((int *) st_Arr.pv_Addr, i_Test));

#ifdef ACTIVE_EDITORS
	SpecialFlags_Array = (int *) st_Arr.pv_Addr;
	if(SpecialFlags_b_ActiveTracer)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_GO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		AI_M_GetCurrentObject(pst_GO);
		if(!SpecialFlags_Skip(pst_GO, i_Test)) SpecialFlags_Test((int *) st_Arr.pv_Addr, i_Test, pst_GO);
	}

#endif
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRGlobalIntFlagTest_C(int on)
{
	AI_Check(ARR_gpi_Global, "Global array not defined");
	return AI_EvalFunc_ARRBitTest_C(ARR_gpi_Global, on);
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRGlobalIntFlagTest(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int on;
	/*~~~*/

	on = AI_PopInt();
#ifdef ACTIVE_EDITORS
	if(SpecialFlags_b_ActiveTracer)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_GO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		AI_M_GetCurrentObject(pst_GO);
		SpecialFlags_Test((int *) ARR_gpi_Global, on, pst_GO);
	}

#endif

	AI_PushInt(AI_EvalFunc_ARRGlobalIntFlagTest_C(on));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_ARRBitClearRange(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Arr;
	AI_tdst_UnionVar	s_Val;
	int					i_Set2, i_Set1;
	int					*p1, *p2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Set2 = AI_PopInt();
	i_Set1 = AI_PopInt();
	AI_PopVar(&s_Val, &st_Arr);

	p1 = (int *) st_Arr.pv_Addr + ((i_Set1 &~31) >> 5);
	p2 = (int *) st_Arr.pv_Addr + ((i_Set2 &~31) >> 5);
	if(p1 == p2)
	{
		*p1 &= ~((2 << (i_Set2 & 31)) - (1 << (i_Set1 & 31)));
	}
	else
	{
#ifdef JADEFUSION
		*(p1++) &= ~(0x100000000 - (__int64)(1 << (i_Set1 & 31)));
#else
		*(p1++) &= ~(0x100000000 - (1 << (i_Set1 & 31)));
#endif
		*p2 &= ~((2 << (i_Set2 & 31)) - 1);
		while(p1 != p2) *(p1++) = 0;
	}

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_ARRBitSetRange(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Arr;
	AI_tdst_UnionVar	s_Val;
	int					i_Set2, i_Set1;
	int					*p1, *p2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Set2 = AI_PopInt();
	i_Set1 = AI_PopInt();
	AI_PopVar(&s_Val, &st_Arr);

	p1 = (int *) st_Arr.pv_Addr + ((i_Set1 &~31) >> 5);
	p2 = (int *) st_Arr.pv_Addr + ((i_Set2 &~31) >> 5);
	if(p1 == p2)
	{
		*p1 |= (2 << (i_Set2 & 31)) - (1 << (i_Set1 & 31));
	}
	else
	{
#ifdef JADEFUSION
		*(p1++) |= 0x100000000 - (__int64)(1 << (i_Set1 & 31));
#else
		*(p1++) |= 0x100000000 - (1 << (i_Set1 & 31));
#endif
		*p2 |= (2 << (i_Set2 & 31)) - 1;
		while(p1 != p2) *(p1++) = 0xFFFFFFFF;
	}

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRBitAnd_C(int *paddr, int *pand, int *pinv, int i_Max, int i_Ignore)
{
	/*~~~~~~*/
	int *pbit;
	int i;
	int iok;
	/*~~~~~~*/

	iok = 0;

	for(i = 0; i < i_Max; i++)
	{
		if(pand[i] == i_Ignore) continue;
		iok = 1;
		pbit = paddr + ((pand[i] &~31) >> 5);
		if(*pbit & (1 << (pand[i] & 31)))
		{
			if(!pinv[i])
			{
				return 0;
			}
		}
		else
		{
			if(pinv[i])
			{
				return 0;
			}
		}
	}

	if(!iok) return 0;
	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRBitAnd(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Ignore;
	int					i_Max;
	AI_tdst_PushVar		st_ArrInv, st_ArrAnd, st_ArrBit;
	AI_tdst_UnionVar	s_ValInv, st_ValAnd, st_ValBit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Ignore = AI_PopInt();
	i_Max = AI_PopInt();
	AI_PopVar(&s_ValInv, &st_ArrInv);
	AI_PopVar(&st_ValAnd, &st_ArrAnd);
	AI_PopVar(&st_ValBit, &st_ArrBit);

	AI_PushInt
	(
		AI_EvalFunc_ARRBitAnd_C
			(
				(int *) st_ArrBit.pv_Addr,
				(int *) st_ArrAnd.pv_Addr,
				(int *) st_ArrInv.pv_Addr,
				i_Max,
				i_Ignore
			)
	);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRBitOr_C(int *paddr, int *pand, int *pinv, int i_Max, int i_Ignore)
{
	/*~~~~~~*/
	int *pbit;
	int i;
	/*~~~~~~*/

	for(i = 0; i < i_Max; i++)
	{
		if(pand[i] == i_Ignore) continue;
		pbit = paddr + ((pand[i] &~31) >> 5);
		if(*pbit & (1 << (pand[i] & 31)))
		{
			if(pinv[i])
			{
				return 1;
			}
		}
		else
		{
			if(!pinv[i])
			{
				return 1;
			}
		}
	}

	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRBitOr(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Ignore;
	int					i_Max;
	AI_tdst_PushVar		st_ArrInv, st_ArrAnd, st_ArrBit;
	AI_tdst_UnionVar	s_ValInv, st_ValAnd, st_ValBit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Ignore = AI_PopInt();
	i_Max = AI_PopInt();
	AI_PopVar(&s_ValInv, &st_ArrInv);
	AI_PopVar(&st_ValAnd, &st_ArrAnd);
	AI_PopVar(&st_ValBit, &st_ArrBit);

	AI_PushInt
	(
		AI_EvalFunc_ARRBitOr_C
			(
				(int *) st_ArrBit.pv_Addr,
				(int *) st_ArrAnd.pv_Addr,
				(int *) st_ArrInv.pv_Addr,
				i_Max,
				i_Ignore
			)
	);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ARRBit4Test_C(int *paddr, int i1, int i2, int i3, int i4)
{
	/*~~~~~~~*/
	int sf;
	int *pbit;
	int i, tst;
	int sf1[4];
	/*~~~~~~~*/

	sf1[0] = i1;
	sf1[1] = i2;
	sf1[2] = i3;
	sf1[3] = i4;

	for(i = 0; i < 4; i++)
	{
		if(sf1[i])
		{
			sf = sf1[i];
			if(sf > 10000) sf -= 10000;
			pbit = paddr + ((sf &~31) >> 5);
			tst = *pbit & (1 << (sf & 31));
			if((tst && sf1[i] > 10000) || (!tst && sf1[i] < 10000))
			{
				return 0;
			}
		}
	}

	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRBit4Test(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					sf1[4];
	AI_tdst_PushVar		st_ArrBit;
	AI_tdst_UnionVar	st_ValBit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sf1[3] = AI_PopInt();
	sf1[2] = AI_PopInt();
	sf1[1] = AI_PopInt();
	sf1[0] = AI_PopInt();
	AI_PopVar(&st_ValBit, &st_ArrBit);
	AI_PushInt(AI_EvalFunc_ARRBit4Test_C((int *) st_ArrBit.pv_Addr, sf1[0], sf1[1], sf1[2], sf1[3]));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ARRGlobalIntSet_C(int *p)
{
	ARR_gpi_Global = p;
}
/**/
AI_tdst_Node *AI_EvalFunc_ARRGlobalIntSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_ArrBit;
	AI_tdst_UnionVar	st_ValBit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&st_ValBit, &st_ArrBit);
	ARR_gpi_Global = (int *) st_ArrBit.pv_Addr;
	return ++_pst_Node;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SpecialFlags_Test(int *pi_Array, int i_Test, OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	asz_Log[500];
	BOOL	b_TestTRUE;
	int		i;
	/*~~~~~~~~~~~~~~~~~*/

	b_TestTRUE = (AI_EvalFunc_ARRBitTest_C(pi_Array, i_Test));

	if(!SpecialFlags_FlagToTrace || (SpecialFlags_FlagToTrace == i_Test))
	{
		if(SpecialFlags_b_Reset) SpecialFlags_Reset();
		if(SpecialFlags_b_Init) SpecialFlags_Init();

		if(SpecialFlags_ul_LastInfoLoop + 150 < ENG_gul_Loop)
		{
			SpecialFlags_ul_LastInfoLoop = ENG_gul_Loop;
			LINK_PrintStatusMsg(".");
		}

		if(SpecialFlags_ul_CurrentLoop != ENG_gul_Loop)
		{
			if(ENG_gb_FirstFrame) SpecialFlags_Reset();
			SpecialFlags_ul_CurrentLoop = ENG_gul_Loop;
		}

		for(i = 0; i < SpecialFlags_NumTests; i++)
		{
			if((SpecialFlags_TestTable[i].pst_GO == pst_GO) && (SpecialFlags_TestTable[i].i_Flag == i_Test))
			{
				if(SpecialFlags_TestTable[i].b_Status == b_TestTRUE)
				{
					return;
				}
				else
				{
					SpecialFlags_TestTable[i] = SpecialFlags_TestTable[SpecialFlags_NumTests];
					SpecialFlags_NumTests--;
				}
			}
		}

		if(b_TestTRUE)
		{
			sprintf
			(
				asz_Log,
				"[VRAI] %s teste le SpecialFlag %u <%s>",
				pst_GO ? pst_GO->sz_Name : "<Universe?>",
				i_Test,
				SpecialFlags_Text[i_Test]
			);
			SpecialFlags_ul_LastInfoLoop = ENG_gul_Loop;
		}
		else
		{
			sprintf
			(
				asz_Log,
				"[FAUX] %s teste le SpecialFlag %u <%s>",
				pst_GO ? pst_GO->sz_Name : "<Universe?>",
				i_Test,
				SpecialFlags_Text[i_Test]
			);
			SpecialFlags_ul_LastInfoLoop = ENG_gul_Loop;
		}

		LINK_PrintStatusMsg(asz_Log);

		SpecialFlags_TestTable[SpecialFlags_NumTests].i_Flag = i_Test;
		SpecialFlags_TestTable[SpecialFlags_NumTests].pst_GO = pst_GO;
		SpecialFlags_TestTable[SpecialFlags_NumTests].b_Status = b_TestTRUE;

		SpecialFlags_NumTests++;

		if(SpecialFlags_NumTests == 1000)
		{
			LINK_PrintStatusMsg("Rupture du continuum Espace-Temps ....");
			SpecialFlags_Reset();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SpecialFlags_Clear(int *pi_Array, int i_Clear, OBJ_tdst_GameObject *pst_GO)
{
	if
	(
		AI_EvalFunc_ARRBitTest_C(pi_Array, i_Clear)
	&&	(!SpecialFlags_FlagToTrace || (SpecialFlags_FlagToTrace == i_Clear))
	)
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[500];
		/*~~~~~~~~~~~~~~~~~*/

		if(SpecialFlags_b_Reset) SpecialFlags_Reset();
		if(SpecialFlags_b_Init) SpecialFlags_Init();

		sprintf
		(
			asz_Log,
			"[RESET] %s enleve le SpecialFlag %u <%s>",
			pst_GO ? pst_GO->sz_Name : "<Universe?>",
			i_Clear,
			SpecialFlags_Text[i_Clear]
		);
		LINK_PrintStatusMsg(asz_Log);
		SpecialFlags_ul_LastInfoLoop = ENG_gul_Loop;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SpecialFlags_Set(int *pi_Array, int i_Set, OBJ_tdst_GameObject *pst_GO)
{
	if(!AI_EvalFunc_ARRBitTest_C(pi_Array, i_Set) && (!SpecialFlags_FlagToTrace || (SpecialFlags_FlagToTrace == i_Set)))
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[500];
		/*~~~~~~~~~~~~~~~~~*/

		if(SpecialFlags_b_Reset) SpecialFlags_Reset();
		if(SpecialFlags_b_Init) SpecialFlags_Init();

        sprintf(asz_Log, "[SET] %s met le SpecialFlag %u <%s>", pst_GO ? pst_GO->sz_Name : "Universe?", i_Set, SpecialFlags_Text[i_Set]);
		LINK_PrintStatusMsg(asz_Log);
		SpecialFlags_ul_LastInfoLoop = ENG_gul_Loop;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SpecialFlags_Skip(OBJ_tdst_GameObject *_pst_GO, int _i_Flag)
{
    if ( !_pst_GO ) return FALSE;
    if ( !_pst_GO->sz_Name ) return FALSE;
	if(!L_strcmp(_pst_GO->sz_Name, "GST_Cheat_Manager.gao")) return TRUE;
	if(!L_strcmp(_pst_GO->sz_Name, "GST_Carte.gao")) return TRUE;
	if(!L_strcmp(_pst_GO->sz_Name, "WP_notes.gao")) return TRUE;

	/* Questions / Reponses */
//	if((_i_Flag > 125) && (_i_Flag < 315)) return TRUE;

	/* Map */
	if((_i_Flag > 1200) && (_i_Flag < 1250)) return TRUE;

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SpecialFlags_Reset(void)
{
	/*~~*/
	int i;
	/*~~*/

	SpecialFlags_b_Reset = 0;

	for(i = 0; i < 1000; i++) SpecialFlags_TestTable[i].pst_GO = NULL;

	SpecialFlags_NumTests = 0;
	LINK_PrintStatusMsg("---------------------------------------------------------------");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SpecialFlags_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	L_FILE	h_File;
	ULONG	ul_File;
	char	*pc_Buffer, *pc_Buf;
	char	pc_Info[500];
	LONG	l_Size;
	int		i, iSize, i_Flag;
	BOOL	b_FileInBigFile;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	SpecialFlags_Reset();

	SpecialFlags_b_Init = FALSE;

	for(i = 0; i < 5000; i++)
	{
		L_strcpy(SpecialFlags_Text[i], "???");
	}

	ul_File = BIG_ul_SearchFileExt("Root/EditorDatas", "Spécial flags.csv");

	if(ul_File == BIG_C_InvalidIndex)
	{
		h_File = L_fopen("Spécial flags.csv", "rt");
		if(!CLI_FileOpen(h_File)) return;
		L_fseek(h_File, 0, SEEK_END);
		l_Size = L_ftell(h_File);
		L_fseek(h_File, 0, SEEK_SET);
		pc_Buffer = (char*)L_malloc(l_Size);
		L_fread(pc_Buffer, 1, l_Size, h_File);
		L_fclose(h_File);
		b_FileInBigFile = FALSE;
	}
	else
	{
		pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(ul_File), NULL);
		b_FileInBigFile = TRUE;
	}

	pc_Buf = pc_Buffer;

	/* Skip 26 First line (Useless stuff in the csv file */
	for(i = 0; i < 26; i++)
	{
		while(*pc_Buffer != '\n') *pc_Buffer++;
		/* Skip '\n'. */
		*pc_Buffer++;
	}


	while((*pc_Buffer != ';') || (*(pc_Buffer + 1) != ';'))
	{
		/* Skip First column until ";" */
		while(*pc_Buffer != ';') pc_Buffer ++;
		/* Skip ';'. */
		*pc_Buffer++;

		sscanf(pc_Buffer, "%d", &i_Flag);

		while(*pc_Buffer != ';') *pc_Buffer++;

		/* Skip ';' after Flag ID */
		*pc_Buffer++;

		/* Skip Map string */
		while(*pc_Buffer != ';') *pc_Buffer++;

		/* Skip ';' after Map string */
		*pc_Buffer++;

		iSize = 0;
		while(*pc_Buffer != ';')
		{
			if((*pc_Buffer == '\n') || ((iSize > 2) && (*pc_Buffer == ' ') && (pc_Info[iSize - 1] == ' ')))
			{
				*pc_Buffer++;
				continue;
			}

			pc_Info[iSize++] = *pc_Buffer++;
		}

		if(iSize != 0)
		{
			pc_Info[iSize++] = '\0';
			L_strcpy(SpecialFlags_Text[i_Flag], pc_Info);
		}

		/* Skip End of Line. */
		while(*pc_Buffer != '\n') *pc_Buffer++;

		/* Skip '\n'. */
		*pc_Buffer++;
	}

	if(!b_FileInBigFile) L_free(pc_Buf);
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

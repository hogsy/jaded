/*$T AIfunctions_way.c GC! 1.100 03/30/01 14:09:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "CAMera/CAMera.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/WAYs/WAYstruct.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "ENGine/Sources/WAYs/WAY.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_vars.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "BASe/BENch/BENch.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

static OBJ_tdst_GameObject	*AI_WAYgpstBest = NULL;
static float				AI_WAYgfBest = 0.0f;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_WAYNetNextWP_C
(
	OBJ_tdst_GameObject *pst_GO,
	WAY_tdst_Network	*pst_Net,
	OBJ_tdst_GameObject *pst_Cur,
	int					iPar,
	ULONG				iCapas
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Root;
	OBJ_tdst_GameObject *pst_Next;
	ULONG				i, j, ulBestJ;
	WAY_tdst_Struct		*pst_Struct;
	MATH_tdst_Vector	*pst_Axis, *pst_Pos, st_Dir, st_Axis;
	MATH_tdst_Vector	st_Axis1;
	float				fAngle, fMaxAngle, fLowPound;
	OBJ_tdst_GameObject *pst_Ret;
	unsigned int		lrand;
	OBJ_tdst_GameObject *ap_Tmp[50];
	int					ai_Tmp;
	int					fullpar;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	fullpar = iPar;
	iPar &= 0xFF;
	ulBestJ = 0;
	pst_Ret = NULL;

	/* Get root node of network */
	pst_Root = pst_Net->pst_Root;
	if(pst_Cur)
	{
		ERR_X_Assert(pst_Cur->pst_Extended && pst_Cur->pst_Extended->pst_Links);
		pst_Struct = (WAY_tdst_Struct *) pst_Cur->pst_Extended->pst_Links;

		/* Search the given list of links */
		for(i = 0; i < pst_Struct->ul_Num; i++)
		{
			if(pst_Struct->pst_AllLinks[i].pst_Network == pst_Net) break;
		}

		/* No links */
		AI_Check(i < pst_Struct->ul_Num, "Object is not in the specified network");
		if(!pst_Struct->pst_AllLinks[i].pst_Links) return NULL;

		/* Choose a link */
		switch(iPar)
		{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Return first link
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case 0:
			if(pst_Struct->pst_AllLinks[i].ul_Num == 0)
				pst_Ret = NULL;
			else
				pst_Ret = pst_Struct->pst_AllLinks[i].pst_Links[0].pst_Next;
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Return random link
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case 1:
			if(pst_Struct->pst_AllLinks[i].ul_Num == 0)
				pst_Ret = NULL;
			else
			{
				lrand = lRand(0, pst_Struct->pst_AllLinks[i].ul_Num);
				ERR_X_Assert(lrand < pst_Struct->pst_AllLinks[i].ul_Num);
				pst_Ret = pst_Struct->pst_AllLinks[i].pst_Links[lrand].pst_Next;
			}
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Return link with lowest pound
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case 2:
			if(pst_Struct->pst_AllLinks[i].ul_Num == 0)
				pst_Ret = NULL;
			else
			{
				fLowPound = Cf_Infinit;
				for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
				{
					if(pst_Struct->pst_AllLinks[i].pst_Links[j].f_Pound < fLowPound)
					{
						fLowPound = pst_Struct->pst_AllLinks[i].pst_Links[j].f_Pound;
						ulBestJ = j;
					}
				}

				pst_Ret = pst_Struct->pst_AllLinks[i].pst_Links[ulBestJ].pst_Next;
			}
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Return the better axis depending on current object axis
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case 3:
		case 4:
		case 5:
			if(iPar == 3)
			{
				pst_Axis = MATH_pst_GetYAxis(pst_GO->pst_GlobalMatrix);
				MATH_CopyVector(&st_Axis1, pst_Axis);
				pst_Axis = &st_Axis1;
				MATH_NegVector(pst_Axis, pst_Axis);
			}
			else if(iPar == 4)
			{
				pst_Axis = MATH_pst_GetYAxis(pst_Net->pst_Root->pst_GlobalMatrix);
				MATH_CopyVector(&st_Axis1, pst_Axis);
				pst_Axis = &st_Axis1;
				MATH_NegVector(pst_Axis, pst_Axis);
			}
			else
			{
				AI_Check(pst_GO->pst_Base->pst_Dyna, "Perso has no dynamic");
				DYN_GetSpeedVector(pst_GO->pst_Base->pst_Dyna, &st_Axis);
				MATH_NormalizeVector(&st_Axis, &st_Axis);
				pst_Axis = &st_Axis;
			}

			fMaxAngle = -2.0f;
			ulBestJ = (ULONG) - 1;
			for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
			{
				pst_Next = pst_Struct->pst_AllLinks[i].pst_Links[j].pst_Next;
				if(!pst_Next) break;
				if((!iCapas) || (pst_Struct->pst_AllLinks[i].pst_Links[j].uw_Capacities & iCapas))
				{
					pst_Pos = OBJ_pst_GetAbsolutePosition(pst_Next);
					MATH_SubVector(&st_Dir, pst_Pos, OBJ_pst_GetAbsolutePosition(pst_GO));
					MATH_NormalizeVector(&st_Dir, &st_Dir);
					fAngle = MATH_f_DotProduct(&st_Dir, pst_Axis);
					if(fAngle > fMaxAngle)
					{
						fMaxAngle = fAngle;
						ulBestJ = j;
					}
				}
			}

			pst_Ret = NULL;
			if(ulBestJ != (ULONG) - 1)
			{
				pst_Ret = pst_Struct->pst_AllLinks[i].pst_Links[ulBestJ].pst_Next;
			}
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Return a link number
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case 6:
			if((ULONG) iCapas >= pst_Struct->pst_AllLinks[i].ul_Num)
				pst_Ret = NULL;
			else
				pst_Ret = pst_Struct->pst_AllLinks[i].pst_Links[iCapas].pst_Next;
			break;

		/* Trouve un wp qui repond au condition des capas. S'il y en a plusieurs, choisit au hasard parmis tous */
		case 7:
			ai_Tmp = 0;
			for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
			{
				if(pst_Struct->pst_AllLinks[i].pst_Links[j].uw_Capacities & iCapas)
				{
					pst_Next = pst_Struct->pst_AllLinks[i].pst_Links[j].pst_Next;
					ap_Tmp[ai_Tmp++] = pst_Next;
				}
			}

			if(!ai_Tmp) return NULL;

			/* fullparam & 0xFF00 => numéro de lien a prendre (hasard si 0) */
			if(fullpar & 0xFF00) return ap_Tmp[((fullpar >> 8) & 0xFF) % ai_Tmp];
			return ap_Tmp[lRand(0, ai_Tmp)];
		}
	}
	else
	{
		pst_Ret = pst_Root;
	}

	return pst_Ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WAYGetNumLinks_C(WAY_tdst_Network *pst_Net, OBJ_tdst_GameObject *pst_Cur)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			i;
	WAY_tdst_Struct *pst_Struct;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_Net, "Network not define");
	AI_Check(pst_Cur && pst_Cur->pst_Extended && pst_Cur->pst_Extended->pst_Links, "Bad object");

	pst_Struct = (WAY_tdst_Struct *) pst_Cur->pst_Extended->pst_Links;

	/* Search the given list of links */
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		if(pst_Struct->pst_AllLinks[i].pst_Network == pst_Net) break;
	}

	AI_Check(i < pst_Struct->ul_Num, "Object is not in the specified network");
	return pst_Struct->pst_AllLinks[i].ul_Num;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYGetNumLinks(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Cur;
	WAY_tdst_Network	*pst_Net;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cur = AI_PopGameObject();
	pst_Net = AI_PopNetwork();
	AI_PushInt(AI_EvalFunc_WAYGetNumLinks_C(pst_Net, pst_Cur));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_WAYNetNextWP(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Cur;
	WAY_tdst_Network	*pst_Net;
	int					iPar, iCapa;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	iCapa = AI_PopInt();
	iPar = AI_PopInt();
	pst_Cur = AI_PopGameObject();
	pst_Net = AI_PopNetwork();
	AI_Check(pst_Net, "Network not define");

	AI_PushGameObject(AI_EvalFunc_WAYNetNextWP_C(pst_GO, pst_Net, pst_Cur, iPar, iCapa));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_WAYRootGet_C(WAY_tdst_Network *pst_Net)
{
	AI_Check(pst_Net, "Network not define");
	return pst_Net->pst_Root;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYRootGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Network	*pst_Net;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Net = AI_PopNetwork();
	AI_PushGameObject(AI_EvalFunc_WAYRootGet_C(pst_Net));
	return ++_pst_Node;
}

int okok = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject **AI_WAYNetFillRec
(
	WAY_tdst_Network	*_pst_Net,
	OBJ_tdst_GameObject *_pst_Root,
	OBJ_tdst_GameObject **_pst_Buf,
	int on,
	int off,
	int what
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct *pst_Struct;
	ULONG			i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Root) return _pst_Buf;
	if(!_pst_Buf) return _pst_Buf;

	if(BAS_bsearch((ULONG) _pst_Root, &WAY_gst_Seen) != (ULONG) - 1) return _pst_Buf;
	BAS_binsert((ULONG) _pst_Root, (ULONG) _pst_Root, &WAY_gst_Seen);

	if(OBJ_b_TestFlag(_pst_Root, on, off, what)) 
	{
		okok++;
		*(_pst_Buf++) = _pst_Root;
	}

	if(!_pst_Root->pst_Extended) return _pst_Buf;
	if(!_pst_Root->pst_Extended->pst_Links) return _pst_Buf;

	pst_Struct = (WAY_tdst_Struct *) _pst_Root->pst_Extended->pst_Links;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		if(pst_Struct->pst_AllLinks[i].pst_Network != _pst_Net) continue;
		for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
		{
			_pst_Buf = AI_WAYNetFillRec(_pst_Net, pst_Struct->pst_AllLinks[i].pst_Links[j].pst_Next, _pst_Buf, on, off, what);
		}
	}

	return _pst_Buf;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WAYNetFill_C
(
	WAY_tdst_Network	*pst_Net, 
	OBJ_tdst_GameObject	*pst_Root, 
	OBJ_tdst_GameObject **paddr
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject **pst_Ret;
	int					ires;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_Root)
	{
		AI_Check(pst_Net != NULL, "Network not define");
		pst_Root = pst_Net->pst_Root;
	}

	WAY_gst_Seen.num = 0;
	pst_Ret = AI_WAYNetFillRec(pst_Net, pst_Root, paddr, -1, 0, 0);
	ires = WAY_gst_Seen.num;
	WAY_gst_Seen.num = 0;
	return ires;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYNetFill(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	WAY_tdst_Network	*pst_Net;
	OBJ_tdst_GameObject *pst_Root;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_Check(st_Var.pv_Addr, "Array not defined");
	pst_Root = AI_PopGameObject();
	pst_Net = AI_PopNetwork();
	AI_PushInt(AI_EvalFunc_WAYNetFill_C(pst_Net, pst_Root, (OBJ_tdst_GameObject **) st_Var.pv_Addr));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WAYGet_C(WAY_tdst_Network *pst_Net, OBJ_tdst_GameObject **parray, int on, int off, int what)
{
	int ires;

	okok = 0;
	WAY_gst_Seen.num = 0;
	AI_WAYNetFillRec(pst_Net, pst_Net->pst_Root, parray, on, off, what);
	ires = WAY_gst_Seen.num;
	WAY_gst_Seen.num = 0;
	return okok;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	int					on, off, what;
	int					ires;
	WAY_tdst_Network	*pst_Net;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	what = AI_PopInt();
	off = AI_PopInt();
	on = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	pst_Net = AI_PopNetwork();

	ires = AI_EvalFunc_WAYGet_C(pst_Net, (OBJ_tdst_GameObject **) st_Var.pv_Addr, on, off, what);
	AI_PushInt(ires);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_WAYNearestOfPosRec
(
	WAY_tdst_Network	*_pst_Net,
	OBJ_tdst_GameObject *_pst_Root,
	MATH_tdst_Vector	*_pst_Pos,
	ULONG				ul_OnFlags,
	ULONG				ul_OffFlags,
	ULONG				ul_FlagID,
	OBJ_tdst_GameObject **_ppst_Exclude,
	ULONG				_ul_NumExclude,
	BOOL				_b_TestGrid
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct		*pst_Struct;
	ULONG				i, j;
	MATH_tdst_Vector	st_Vec;
	float				f_Norm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(BAS_bsearch((ULONG) _pst_Root, &WAY_gst_Seen) != (ULONG) - 1) return;
	BAS_binsert((ULONG) _pst_Root, (ULONG) _pst_Root, &WAY_gst_Seen);

	/* Test exclusion */
	for(i = 0; i < _ul_NumExclude; i++)
	{
		if(_ppst_Exclude[i] == _pst_Root) break;
	}

	/* Test distance and flags */
	if((i == _ul_NumExclude) && _pst_Root && OBJ_b_TestFlag(_pst_Root, ul_OnFlags, ul_OffFlags, ul_FlagID))
	{
		if(!AI_WAYgpstBest)
		{
			AI_WAYgpstBest = _pst_Root;
			MATH_SubVector(&st_Vec, OBJ_pst_GetAbsolutePosition(_pst_Root), _pst_Pos);
//			if(_b_TestGrid)st_Vec.z = 0;
			AI_WAYgfBest = MATH_f_NormVector(&st_Vec);
			if(_b_TestGrid)BAS_binsert((ULONG) (AI_WAYgfBest * 10000.0f), (ULONG) _pst_Root, &WAY_gst_SeenDist);
		}
		else
		{
			MATH_SubVector(&st_Vec, OBJ_pst_GetAbsolutePosition(_pst_Root), _pst_Pos);
//			if(_b_TestGrid)st_Vec.z = 0;
			f_Norm = MATH_f_NormVector(&st_Vec);
			if(_b_TestGrid) BAS_binsert((ULONG) (f_Norm * 10000.0f), (ULONG) _pst_Root, &WAY_gst_SeenDist);
			if(f_Norm < AI_WAYgfBest)
			{
				AI_WAYgpstBest = _pst_Root;
				AI_WAYgfBest = f_Norm;
			}
		}
	}

	if(!_pst_Root) return;
	if(!_pst_Root->pst_Extended) return;
	if(!_pst_Root->pst_Extended->pst_Links) return;

	pst_Struct = (WAY_tdst_Struct *) _pst_Root->pst_Extended->pst_Links;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		if(pst_Struct->pst_AllLinks[i].pst_Network != _pst_Net) continue;
		for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
		{
			AI_WAYNearestOfPosRec
			(
				_pst_Net,
				pst_Struct->pst_AllLinks[i].pst_Links[j].pst_Next,
				_pst_Pos,
				ul_OnFlags,
				ul_OffFlags,
				ul_FlagID,
				_ppst_Exclude,
				_ul_NumExclude,
				_b_TestGrid
			);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_WAYNearestWPOfObj_C
(
	OBJ_tdst_GameObject *_pst_GO,
	WAY_tdst_Network	*_pst_Net,
	ULONG				ul_OnFlags,
	ULONG				ul_OffFlags,
	ULONG				ul_FlagID
)
{
	AI_WAYgpstBest = NULL;
	WAY_gst_Seen.num = 0;
	AI_WAYNearestOfPosRec
	(
		_pst_Net,
		_pst_Net->pst_Root,
		OBJ_pst_GetAbsolutePosition(_pst_GO),
		ul_OnFlags,
		ul_OffFlags,
		ul_FlagID,
		NULL,
		0,
		FALSE
	);
	return AI_WAYgpstBest;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYNearestWPOfObj(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	WAY_tdst_Network	*pst_Net;
	ULONG				ul_OnFlags;
	ULONG				ul_OffFlags;
	ULONG				ul_FlagID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	pst_Net = AI_PopNetwork();
	AI_Check(pst_Net, "Network is null");
	AI_PushGameObject(AI_EvalFunc_WAYNearestWPOfObj_C(pst_GO, pst_Net, ul_OnFlags, ul_OffFlags, ul_FlagID));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_WAYNearestWPOfPos_C
(
	MATH_tdst_Vector	*_pst_Pos,
	WAY_tdst_Network	*_pst_Net,
	ULONG				ul_OnFlags,
	ULONG				ul_OffFlags,
	ULONG				ul_FlagID
)
{
	AI_WAYgpstBest = NULL;
	WAY_gst_Seen.num = 0;
	AI_WAYNearestOfPosRec
	(
		_pst_Net,
		_pst_Net->pst_Root,
		_pst_Pos,
		ul_OnFlags,
		ul_OffFlags,
		ul_FlagID,
		NULL,
		0,
		FALSE
	);
	return AI_WAYgpstBest;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYNearestWPOfPos(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Network	*pst_Net;
	ULONG				ul_OnFlags;
	ULONG				ul_OffFlags;
	ULONG				ul_FlagID;
	MATH_tdst_Vector	*pst_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	pst_Net = AI_PopNetwork();
	pst_Pos = AI_PopVectorPtr();
	AI_Check(pst_Net, "Network is null");
	AI_PushGameObject(AI_EvalFunc_WAYNearestWPOfPos_C(pst_Pos, pst_Net, ul_OnFlags, ul_OffFlags, ul_FlagID));
	return ++_pst_Node;
}

extern BOOL TestRay(GRID_tdst_World *, MATH_tdst_Vector *, MATH_tdst_Vector *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_WAYNearestWPOfPosGrid_C
(
	OBJ_tdst_GameObject	*pst_GO,
	MATH_tdst_Vector	*_pst_Pos,
	WAY_tdst_Network	*_pst_Net,
	ULONG				ul_OnFlags,
	ULONG				ul_OffFlags,
	ULONG				ul_FlagID,
	int					*failed
)
{
	int						i;
	BAS_tdst_Key			*pst_Key;
	GRID_tdst_World			*pst_Grid;
	WOR_tdst_World			*pst_World;

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_Grid = pst_World->pst_Grid;

	AI_WAYgpstBest = NULL;
	WAY_gst_Seen.num = 0;
	WAY_gst_SeenDist.num = 0;
	AI_WAYNearestOfPosRec
	(
		_pst_Net,
		_pst_Net->pst_Root,
		_pst_Pos,
		ul_OnFlags,
		ul_OffFlags,
		ul_FlagID,
		NULL,
		0,
		TRUE
	);

	pst_Key = (BAS_tdst_Key *) WAY_gst_SeenDist.base;
	for(i = 0; i < 	WAY_gst_SeenDist.num; i++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_Key->ul_Val;
		if(!TestRay(pst_Grid, _pst_Pos, &pst_GO->pst_GlobalMatrix->T))
		{
			*failed = 0;
			return pst_GO;
		}
		pst_Key++;
	}

	*failed = 1;
	return AI_WAYgpstBest;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYNearestWPOfPosGrid(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Network	*pst_Net;
	ULONG				ul_OnFlags;
	ULONG				ul_OffFlags;
	ULONG				ul_FlagID;
	MATH_tdst_Vector	*pst_Pos;
	OBJ_tdst_GameObject	*pst_GO;
	int					failed;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVar(&Val, &st_Var);
	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	pst_Net = AI_PopNetwork();
	pst_Pos = AI_PopVectorPtr();
	AI_Check(pst_Net, "Network is null");
	AI_PushGameObject(AI_EvalFunc_WAYNearestWPOfPosGrid_C(pst_GO, pst_Pos, pst_Net, ul_OnFlags, ul_OffFlags, ul_FlagID, &failed));
	*(int *) st_Var.pv_Addr = failed;
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_WAYNearestWPOfObjExcl_C
(
	OBJ_tdst_GameObject *_pst_GO,
	WAY_tdst_Network	*_pst_Net,
	ULONG				ul_OnFlags,
	ULONG				ul_OffFlags,
	ULONG				ul_FlagID,
	OBJ_tdst_GameObject **_ppst_Exclude,
	int					_i_NumExclude
)
{
	AI_WAYgpstBest = NULL;
	WAY_gst_Seen.num = 0;
	AI_WAYNearestOfPosRec
	(
		_pst_Net,
		_pst_Net->pst_Root,
		OBJ_pst_GetAbsolutePosition(_pst_GO),
		ul_OnFlags,
		ul_OffFlags,
		ul_FlagID,
		_ppst_Exclude,
		_i_NumExclude,
		FALSE
	);
	return AI_WAYgpstBest;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYNearestWPOfObjExcl(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	WAY_tdst_Network	*pst_Net;
	ULONG				ul_OnFlags;
	ULONG				ul_OffFlags;
	ULONG				ul_FlagID;
	int					i_NumExcl;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_NumExcl = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	pst_Net = AI_PopNetwork();
	AI_Check(pst_Net, "Network is null");
	AI_PushGameObject
	(
		AI_EvalFunc_WAYNearestWPOfObjExcl_C
			(
				pst_GO,
				pst_Net,
				ul_OnFlags,
				ul_OffFlags,
				ul_FlagID,
				(OBJ_tdst_GameObject **) st_Var.pv_Addr,
				i_NumExcl
			)
	);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_Network *AI_EvalFunc_WAYNetOfObj_C(OBJ_tdst_GameObject *_pst_Root)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct *pst_Struct;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Root->pst_Extended || !_pst_Root->pst_Extended->pst_Links) return NULL;
	pst_Struct = (WAY_tdst_Struct *) _pst_Root->pst_Extended->pst_Links;
    if (pst_Struct->ul_Num>0)
    	return pst_Struct->pst_AllLinks[0].pst_Network;
    else
        return NULL;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYNetOfObj(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushNetWork(AI_EvalFunc_WAYNetOfObj_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int WAY_gi_ShortParams = 0;
#define WAY_C_TestGrid		0x00000001
#define WAY_C_FarFromMain	0x00000002
#define WAY_C_FarFromMain1	0x00000004
#define WAY_C_NoTestGridRed	0x00000008

#define WALL			0x20
#define OCCUP			0xC0
#define M_Grid()		GRID_gi_Current == 0 ? pst_World->pst_Grid : pst_World->pst_Grid1
extern int GRID_gi_Current;
extern int AI_EvalFunc_GRIDGetCapa_C(OBJ_tdst_GameObject *, MATH_tdst_Vector *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WAYShortParams_C(int par)
{
	WAY_gi_ShortParams = par;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYShortParams(AI_tdst_Node *_pst_Node)
{
	WAY_gi_ShortParams = AI_PopInt();
	return ++_pst_Node;
}

OBJ_tdst_GameObject	*WAY_Params[2];
float				WAY_Pound[2];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_WAYShortGet_C(int par)
{
	if(par >= 2) return NULL;
	return WAY_Params[par];
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYShortGet(AI_tdst_Node *_pst_Node)
{
	int what;
	what = AI_PopInt();
	AI_PushGameObject(AI_EvalFunc_WAYShortGet_C(what));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WAYShortSet_C(int what, float val)
{
	if(what >= 2) return;
	WAY_Pound[what] = val;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYShortSet(AI_tdst_Node *_pst_Node)
{
	int what;
	float val;
	val = AI_PopFloat();
	what = AI_PopInt();
	AI_EvalFunc_WAYShortSet_C(what, val);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WAYShortWay_C
(
	OBJ_tdst_GameObject *_pst_GO,
	WAY_tdst_Network	*_pst_Net,
	OBJ_tdst_GameObject **_pst_Arr,
	OBJ_tdst_GameObject *_pst_1,
	OBJ_tdst_GameObject *_pst_2,
	ULONG				capaon,
	ULONG				capaoff
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_LinkList	*pst_LinkList;
	WAY_tdst_Link		*pst_Link;
	OBJ_tdst_GameObject **pst_Explored;
	unsigned int		i_NbElemInExplored;
	float				*pf_EvalExplored;
	unsigned int		*pi_WayExplored;
	OBJ_tdst_GameObject **pst_Reached, *pst_Obj;
	unsigned int		i_CurElemInReached;
	unsigned int		i_NbElemInReached;
	float				*pf_EvalReached;
	unsigned int		*pi_WayReached;
	unsigned int		i_Index, i_Index1;
	float				f_Eval;
	BOOL				b_Done;
	float				pound;
	MATH_tdst_Vector	t_Tmp, t_Tmp1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_CurElemInReached = 0;

	pf_EvalExplored = (float *) MEM_p_Alloc(1000 * sizeof(float));
	pf_EvalReached = (float *) MEM_p_Alloc(1000 * sizeof(float));
	pi_WayExplored = (unsigned int *) MEM_p_Alloc(1000 * sizeof(int));
	pi_WayReached = (unsigned int *) MEM_p_Alloc(1000 * sizeof(int));
	pst_Explored = (OBJ_tdst_GameObject **) MEM_p_Alloc(1000 * sizeof(OBJ_tdst_GameObject *));
	pst_Reached = (OBJ_tdst_GameObject **) MEM_p_Alloc(1000 * sizeof(OBJ_tdst_GameObject *));
	b_Done = FALSE;

	/* First reached is first node */
	i_NbElemInExplored = 0;
	i_NbElemInReached = 1;
	pst_Reached[0] = _pst_1;
	pf_EvalReached[0] = 0;
	pi_WayReached[0] = (unsigned int) - 1;

	while(i_NbElemInReached && !b_Done)
	{
		/* Retreive the best reached node */
		f_Eval = Cf_Infinit;
		for(i_Index = 0; i_Index < i_NbElemInReached; i_Index++)
		{
			if(pf_EvalReached[i_Index] < f_Eval)
			{
				f_Eval = pf_EvalReached[i_Index];
				i_CurElemInReached = i_Index;
			}
		}

		/* Add node to explored */
		pst_Explored[i_NbElemInExplored] = pst_Reached[i_CurElemInReached];
		pi_WayExplored[i_NbElemInExplored] = pi_WayReached[i_CurElemInReached];
		pf_EvalExplored[i_NbElemInExplored++] = f_Eval;

		/* Remove node from reached */
		i_NbElemInReached--;
		pst_Reached[i_CurElemInReached] = pst_Reached[i_NbElemInReached];
		pf_EvalReached[i_CurElemInReached] = pf_EvalReached[i_NbElemInReached];
		pi_WayReached[i_CurElemInReached] = pi_WayReached[i_NbElemInReached];

		/* Does the destination is reached ? */
		if(pst_Explored[i_NbElemInExplored - 1] == _pst_2) b_Done = TRUE;

		pst_LinkList = WAY_pst_SearchLinkList(pst_Explored[i_NbElemInExplored - 1], _pst_Net);
		if(pst_LinkList)
		{
			for(i_Index = 0; i_Index < (unsigned int) pst_LinkList->ul_Num; i_Index++)
			{
				/* The link has the correct capabilities ? */
				pst_Link = &pst_LinkList->pst_Links[i_Index];
				if((capaon && capaon != 0xFFFFFFFF) || pst_Link->uw_Capacities)
				{
					if(!(pst_Link->uw_Capacities & capaon)) continue;
					if(pst_Link->uw_Capacities & capaoff) continue;
				}

				/* Is the node already in explored ? */
				for(i_Index1 = 0; i_Index1 < i_NbElemInExplored; i_Index1++)
				{
					if(pst_Explored[i_Index1] == pst_Link->pst_Next) goto l_NextLink;
				}

				/* Compute pound of link */
				if(WAY_gi_ShortParams & (WAY_C_FarFromMain | WAY_C_FarFromMain1))
				{
					if(WAY_gi_ShortParams & WAY_C_FarFromMain)
						pst_Obj = AI_gpst_MainActors[0];
					else
						pst_Obj = AI_gpst_MainActors[1];
					if(pst_Obj)
					{
						MATH_SubVector(&t_Tmp, &pst_Obj->pst_GlobalMatrix->T, &pst_Link->pst_Next->pst_GlobalMatrix->T);
						pound = 1.0f / MATH_f_NormVector(&t_Tmp);
					}
					else
					{
						pound = pst_Link->f_Pound;
					}
				}
				else
				{
					pound = pst_Link->f_Pound;
					MATH_SubVector(&t_Tmp1, OBJ_pst_GetAbsolutePosition(_pst_2), OBJ_pst_GetAbsolutePosition(pst_Link->pst_Next));
					pound += MATH_f_NormVector(&t_Tmp1);

					WAY_Params[0] = pst_Explored[i_NbElemInExplored - 1];
					WAY_Params[1] = pst_Link->pst_Next;
					WAY_Pound[0] = 0;
					AI_ExecCallback(_pst_GO, AI_C_Callback_ShortWay);
					if(WAY_Pound[0] == -1) continue;
					pound += WAY_Pound[0];
				}

				/* Is the node already in reached ? */
				for(i_Index1 = 0; i_Index1 < i_NbElemInReached; i_Index1++)
				{
					if(pst_Reached[i_Index1] == pst_Link->pst_Next)
					{
						if(pf_EvalReached[i_Index1] > pf_EvalExplored[i_NbElemInExplored - 1] + pound)
						{
							pf_EvalReached[i_Index1] = pf_EvalExplored[i_NbElemInExplored - 1] + pound;
							pi_WayReached[i_Index1] = i_NbElemInExplored - 1;
						}

						goto l_NextLink;
					}
				}

				/* Else add the node in reached */
				pst_Reached[i_NbElemInReached] = pst_Link->pst_Next;
				pf_EvalReached[i_NbElemInReached] = pf_EvalExplored[i_NbElemInExplored - 1] + pound;
				pi_WayReached[i_NbElemInReached] = i_NbElemInExplored - 1;
				i_NbElemInReached++;
l_NextLink: ;
			}
		}
	}

	if(b_Done)
	{
		/* Construct way */
		i_Index = i_NbElemInExplored - 1;
		i_Index1 = 0;
		while((int) i_Index >= 0)
		{
			_pst_Arr[i_Index1] = pst_Explored[i_Index];
			i_Index = pi_WayExplored[i_Index];
			i_Index1++;
		}

		i_NbElemInExplored = i_Index1;
	}
	else
		i_NbElemInExplored = 0;

	MEM_Free(pf_EvalExplored);
	MEM_Free(pf_EvalReached);
	MEM_Free(pi_WayExplored);
	MEM_Free(pi_WayReached);
	MEM_Free(pst_Reached);
	MEM_Free(pst_Explored);

	return i_NbElemInExplored;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYShortWay(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Network	*pst_Net;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_1, *pst_2;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	int					capaon, capaoff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	capaoff = AI_PopInt();
	capaon = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	pst_2 = AI_PopGameObject();
	AI_Check(pst_2, "Destination waypoint is invalid");
	pst_1 = AI_PopGameObject();
	AI_Check(pst_1, "Source waypoint is invalid");
	AI_PopVar(&Val, &st_Var);
	pst_Net = AI_PopNetwork();
	AI_Check(pst_Net, "Network is invalid");
	AI_PushInt(AI_EvalFunc_WAYShortWay_C(pst_GO, pst_Net, (OBJ_tdst_GameObject **) st_Var.pv_Addr, pst_1, pst_2, capaon, capaoff));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_Link *AI_WayGetLink(WAY_tdst_Network *_pst_Net, OBJ_tdst_GameObject *_pst_1, OBJ_tdst_GameObject *_pst_2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_LinkList	*pst_LinkList;
	WAY_tdst_Link		*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(_pst_1, "Source object for link is invalid");
	AI_Check(_pst_1->pst_Extended && _pst_1->pst_Extended->pst_Links, "Object has no link");
	pst_LinkList = WAY_pst_SearchLinkList(_pst_1, _pst_Net);
	AI_Check(pst_LinkList, "Object is not in the given network");
	pst_Link = WAY_pst_SearchOneLinkInList(pst_LinkList, _pst_2);
	AI_Check(pst_Link, "The link doesn't exist");
	return pst_Link;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_Link *AI_WayGetUnorientedLink(WAY_tdst_Network *_pst_Net, OBJ_tdst_GameObject *_pst_1, OBJ_tdst_GameObject *_pst_2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_LinkList	*pst_LinkList;
	WAY_tdst_Link		*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(_pst_1, "Source object for link is invalid");
    AI_Check(_pst_2, "Target object for link is invalid");

    if (_pst_1->pst_Extended && _pst_1->pst_Extended->pst_Links)
    {
        pst_LinkList = WAY_pst_SearchLinkList(_pst_1, _pst_Net);
        if (pst_LinkList)
        {
            pst_Link = WAY_pst_SearchOneLinkInList(pst_LinkList, _pst_2);
            if (pst_Link) return pst_Link;
        }
    }

	AI_Check(_pst_2->pst_Extended && _pst_2->pst_Extended->pst_Links, "Objects have no link");
	pst_LinkList = WAY_pst_SearchLinkList(_pst_2, _pst_Net);
	AI_Check(pst_LinkList, "Objects are not in the given network");
	pst_Link = WAY_pst_SearchOneLinkInList(pst_LinkList, _pst_1);
	AI_Check(pst_Link, "The link doesn't exist");
	return pst_Link;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WAYCapaSetLink_C
(
	WAY_tdst_Network	*_pst_Net,
	OBJ_tdst_GameObject *_pst_1,
	OBJ_tdst_GameObject *_pst_2,
	ULONG				_i_Add,
	ULONG				_i_Remove
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Link	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Link = AI_WayGetLink(_pst_Net, _pst_1, _pst_2);
	pst_Link->uw_Capacities &= ~_i_Remove;
	pst_Link->uw_Capacities |= _i_Add;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYCapaSetLink(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Network	*pst_Net;
	OBJ_tdst_GameObject *pst_1, *pst_2;
	int					i_Add, i_Remove;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Remove = AI_PopInt();
	i_Add = AI_PopInt();
	pst_2 = AI_PopGameObject();
	pst_1 = AI_PopGameObject();
	pst_Net = AI_PopNetwork();
	AI_EvalFunc_WAYCapaSetLink_C(pst_Net, pst_1, pst_2, i_Add, i_Remove);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_WAYCapaGetLink_C(WAY_tdst_Network *_pst_Net, OBJ_tdst_GameObject *_pst_1, OBJ_tdst_GameObject *_pst_2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Link	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Link = AI_WayGetLink(_pst_Net, _pst_1, _pst_2);
	return pst_Link->uw_Capacities;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYCapaGetLink(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Network	*pst_Net;
	OBJ_tdst_GameObject *pst_1, *pst_2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_2 = AI_PopGameObject();
	pst_1 = AI_PopGameObject();
	pst_Net = AI_PopNetwork();
	AI_PushInt(AI_EvalFunc_WAYCapaGetLink_C(pst_Net, pst_1, pst_2));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WAYDesignSetLink_C
(
	WAY_tdst_Network	*_pst_Net,
	OBJ_tdst_GameObject *_pst_1,
	OBJ_tdst_GameObject *_pst_2,
	int					_i_Add,
	int					_i_Remove
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Link	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Link = AI_WayGetLink(_pst_Net, _pst_1, _pst_2);
	pst_Link->c_Design &= ~_i_Remove;
	pst_Link->c_Design |= _i_Add;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYDesignSetLink(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Network	*pst_Net;
	OBJ_tdst_GameObject *pst_1, *pst_2;
	int					i_Add, i_Remove;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Remove = AI_PopInt();
	i_Add = AI_PopInt();
	pst_2 = AI_PopGameObject();
	pst_1 = AI_PopGameObject();
	pst_Net = AI_PopNetwork();
	AI_EvalFunc_WAYDesignSetLink_C(pst_Net, pst_1, pst_2, i_Add, i_Remove);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WAYDesignGetLink_C(WAY_tdst_Network *_pst_Net, OBJ_tdst_GameObject *_pst_1, OBJ_tdst_GameObject *_pst_2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Link	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Link = AI_WayGetLink(_pst_Net, _pst_1, _pst_2);
	return pst_Link->c_Design;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYDesignGetLink(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Network	*pst_Net;
	OBJ_tdst_GameObject *pst_1, *pst_2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_2 = AI_PopGameObject();
	pst_1 = AI_PopGameObject();
	pst_Net = AI_PopNetwork();
	AI_PushInt(AI_EvalFunc_WAYDesignGetLink_C(pst_Net, pst_1, pst_2));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WAYLinkSetWeight_C
(
	WAY_tdst_Network	*_pst_Net,
	OBJ_tdst_GameObject *_pst_1,
	OBJ_tdst_GameObject *_pst_2,
	float               f_Weight
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Link	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Link = AI_WayGetUnorientedLink(_pst_Net, _pst_1, _pst_2);
    pst_Link->f_Pound = f_Weight;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYLinkSetWeight(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Network	*pst_Net;
	OBJ_tdst_GameObject *pst_1, *pst_2;
	float               f_Weight;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	
	f_Weight = AI_PopFloat();
	pst_2 = AI_PopGameObject();
	pst_1 = AI_PopGameObject();
	pst_Net = AI_PopNetwork();
	AI_EvalFunc_WAYLinkSetWeight_C(pst_Net, pst_1, pst_2, f_Weight);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_WAYLinkGetWeight_C(WAY_tdst_Network *_pst_Net, OBJ_tdst_GameObject *_pst_1, OBJ_tdst_GameObject *_pst_2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Link	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Link = AI_WayGetUnorientedLink(_pst_Net, _pst_1, _pst_2);
	return pst_Link->f_Pound;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYLinkGetWeight(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Network	*pst_Net;
	OBJ_tdst_GameObject *pst_1, *pst_2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_2 = AI_PopGameObject();
	pst_1 = AI_PopGameObject();
	pst_Net = AI_PopNetwork();
	AI_PushFloat(AI_EvalFunc_WAYLinkGetWeight_C(pst_Net, pst_1, pst_2));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WAYHasLinks_C(OBJ_tdst_GameObject *pst_GO, WAY_tdst_Network *pst_Net, OBJ_tdst_GameObject *pst_1)
{
	WAY_tdst_Struct *pst_Struct;
	ULONG			i, j;

	if(!pst_GO->pst_Extended) return 0;
	if(!pst_GO->pst_Extended->pst_Links) return 0;

	pst_Struct = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		if(pst_Struct->pst_AllLinks[i].pst_Network == pst_Net) break;
	}

	if(i == pst_Struct->ul_Num) return 0;
	for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
	{
		if(pst_Struct->pst_AllLinks[i].pst_Links[j].pst_Next == pst_1) return 1;
	}

	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_WAYHasLinks(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *pst_GO, *pst_1;
	WAY_tdst_Network	*pst_Net;

	AI_M_GetCurrentObject(pst_GO);
	pst_1 = AI_PopGameObject();
	pst_Net = AI_PopNetwork();
	AI_PushInt(AI_EvalFunc_WAYHasLinks_C(pst_GO, pst_Net, pst_1));
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

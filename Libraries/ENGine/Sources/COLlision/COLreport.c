/*$T COLreport.c GC 1.138 05/02/03 15:32:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "MATHs/MATH.h"
#include "MATHs/MATHdebug.h"

#include "OBJects/OBJstruct.h"
#include "OBJects/OBJconst.h"
#include "OBJects/OBJaccess.h"
#include "OBJects/OBJorient.h"
#include "OBJects/OBJmain.h"

#include "ANImation/ANImain.h"
#include "ANImation/ANIstruct.h"
#include "ANImation/ANIaccess.h"

#include "COLlision/COLconst.h"
#include "COLlision/COLstruct.h"
#include "COLlision/COLvars.h"
#include "COLlision/COLaccess.h"
#include "COLlision/COLcob.h"

#include "WORld/WORaccess.h"

#include "WORld/WORstruct.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern BOOL				INT_PointInTriangle
						(
							MATH_tdst_Vector *,
							MATH_tdst_Vector *,
							MATH_tdst_Vector *,
							MATH_tdst_Vector *,
							MATH_tdst_Vector *,
							UCHAR *
						);

extern BOOL				COL_gb_LastLoop;
extern BOOL				COL_gb_AfterEngine;

BOOL		COL_gb_SkipODEReports = TRUE;

/*$F------------------------------- Collision Warning Messages static variable --------------------*/
#ifdef ACTIVE_EDITORS
UCHAR					COL_s_Log = 0;
static UCHAR			COL_s_Debug_Number = 0;
static char				*COL_sasz_Debug[8] =
{
	"_PJ_Kong.gao",
	"_PJ_Jack.gao",
	"_PJ_Ann.gao",
	"PFB_PNJ_Bat_Agressive_nid5.gao",
	"_PJ_Camera.gao",
	"PNJ_Raptor_01.gao",
	"Crawler_2.gao",
	"_PJ_Camera_Derriere.gao"
};
#endif
extern WOR_tdst_World	*WOR_gpst_CurrentWorld;

/*$F
 =======================================================================================================================
									--- DEFINES	---
 =======================================================================================================================
 */

#define COL_b_HierarchyLinkCreationTest(a, b) \
		( \
			!(a->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && (a->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) && !(((DYN_tdst_Dyna *) a->pst_Base->pst_Dyna)->ul_DynFlags & DYN_C_NeverDynamicHierarchy) \
			&&	(b->pst_Extended) && (b->pst_Extended->pst_Col) \
				&&	(((COL_tdst_Base *) b->pst_Extended->pst_Col)->pst_ColMap) \
					&&	(b->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) && ! \
								(((DYN_tdst_Dyna *) b->pst_Base->pst_Dyna)->ul_DynFlags & DYN_C_NeverDynamicFather) \
		)
#define COL_b_HierarchyLinkDestructionTest(a, b) \
		( \
			(a->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) \
		&&	(!(a->pst_Base->pst_Hierarchy->pst_FatherInit)) \
		&&	(!(b->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) || (b != a->pst_Base->pst_Hierarchy->pst_Father)) \
		)

/*
 =======================================================================================================================
    && (!(((DYN_tdst_Dyna *) a->pst_Base->pst_Dyna)->ul_DynFlags & DYN_C_ApplyRec)) \ £
    && (!(((DYN_tdst_Dyna *) b->pst_Base->pst_Dyna)->ul_DynFlags & DYN_C_ApplyRec)) \
 =======================================================================================================================
 */
#define COL_b_ActorVsActor(a, b) \
		( \
			( \
				(a->pst_Extended) \
			&&	(a->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_COLPriority) \
			&&	(b->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) \
			) || ((a->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) && (b->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)) \
		)

extern COL_tdst_GlobalVars	COL_gst_GlobalVars;
#define COL_ACCESS(a)	COL_gst_GlobalVars.a

#ifdef ACTIVE_EDITORS
#define COL4Edit_SetReportDebugFlag(a) 		MATH_InitVector(_pst_Global_Recal, (float)a , 0.0f, 0.0f)
#else
#define COL4Edit_SetReportDebugFlag(a)
#endif



/*$F
 =======================================================================================================================

												DEBUG FUNCTIONS

 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
extern UCHAR	COL_uc_RecomputingLoop;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_TraceCollisionReport
(
	COL_tdst_GlobalVars *_pst_GlobalVars,
	COL_tdst_Report		*_pst_Report,
	MATH_tdst_Vector	*_pst_Move,
	UCHAR				_uc_NumFrames
)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	asz_Log[1000];
	char	*psz_Name;
	float	f_Norm;
	float	f_Move;
	/*~~~~~~~~~~~~~~~~~~*/

	f_Move = MATH_f_NormVector(_pst_Move);
	f_Norm = MATH_f_NormVector(&_pst_Report->st_Recal);
	if(_pst_Report->pst_B)
		psz_Name = _pst_Report->pst_B->sz_Name;
	else
		psz_Name = "No Name";

	if((f_Norm - (float) ((int) f_Norm)) != 0.0f)
	{
		sprintf
		(
			asz_Log,
			"-> %s, (%s), (Vx,Vy,Vz)=(%0.3f, %0.3f, %0.3f), (GMat: %x) (F: %u) (M: %0.3f) (M: %s, %0.5f, %s) (N: %0.2f, %0.2f, %0.2f) (R: %0.2f, %0.2f, %0.2f) (Recal: %0.5f) (Elmt: %u, Face: %u)",
			psz_Name,
			(_pst_Report->ul_Flag & COL_Cul_ZDMBox) ? "Box" : "Sph",
			(
				_pst_Report->pst_A->pst_Base
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna->pst_Forces
			) ? _pst_Report->pst_A->pst_Base->pst_Dyna->st_SpeedVector.x : -6.66f,
			(
				_pst_Report->pst_A->pst_Base
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna->pst_Forces
			) ? _pst_Report->pst_A->pst_Base->pst_Dyna->st_SpeedVector.y : -6.66f,
			(
				_pst_Report->pst_A->pst_Base
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna->pst_Forces
			) ? _pst_Report->pst_A->pst_Base->pst_Dyna->st_SpeedVector.z : -6.66f,
			(_pst_Report->pst_GM) ? _pst_Report->pst_GM->ul_CustomBits : 0x11111111,
			_uc_NumFrames,
			f_Move,
			(_pst_Move->x == 0.0f) ? "=0" : ((_pst_Move->x > 0.0f) ? ">0" : "<0"),
			_pst_Move->y,
			(_pst_Move->z == 0.0f) ? "=0" : ((_pst_Move->z > 0.0f) ? ">0" : "<0"),
			_pst_Report->st_Normal.x,
			_pst_Report->st_Normal.y,
			_pst_Report->st_Normal.z,
			_pst_Report->st_Recal.x,
			_pst_Report->st_Recal.y,
			_pst_Report->st_Recal.z,
			f_Norm,
			_pst_Report->uw_Element,
			_pst_Report->ul_Triangle
		);
	}
	else
	{
		sprintf
		(
			asz_Log,
			"-> %s, (%s), (Vx,Vy,Vz)=(%0.3f, %0.3f, %0.3f), (GMat: %x) (F: %u) (M: %0.3f) (M: %s, %0.5f, %s) (N: %0.2f, %0.2f, %0.2f) (Elmt: %u, Face: %u)",
			psz_Name,
			(_pst_Report->ul_Flag & COL_Cul_ZDMBox) ? "Box" : "Sph",
			(
				_pst_Report->pst_A->pst_Base
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna->pst_Forces
			) ? _pst_Report->pst_A->pst_Base->pst_Dyna->st_SpeedVector.x : -6.66f,
			(
				_pst_Report->pst_A->pst_Base
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna->pst_Forces
			) ? _pst_Report->pst_A->pst_Base->pst_Dyna->st_SpeedVector.y : -6.66f,
			(
				_pst_Report->pst_A->pst_Base
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna
			&&	_pst_Report->pst_A->pst_Base->pst_Dyna->pst_Forces
			) ? _pst_Report->pst_A->pst_Base->pst_Dyna->st_SpeedVector.z : -6.66f,
			(_pst_Report->pst_GM) ? _pst_Report->pst_GM->ul_CustomBits : 0x11111111,
			_uc_NumFrames,
			f_Move,
			(_pst_Move->x == 0.0f) ? "=0" : ((_pst_Move->x > 0.0f) ? ">0" : "<0"),
			_pst_Move->y,
			(_pst_Move->z == 0.0f) ? "=0" : ((_pst_Move->z > 0.0f) ? ">0" : "<0"),
			_pst_Report->st_Normal.x,
			_pst_Report->st_Normal.y,
			_pst_Report->st_Normal.z,
			_pst_Report->uw_Element,
			_pst_Report->ul_Triangle
		);

		switch((int) f_Norm)
		{
		case 0: L_strcat(asz_Log, "(None) "); break;
		case 1: L_strcat(asz_Log, "(BackFace) "); break;
		case 4: L_strcat(asz_Log, "(Big Step) "); break;
		}
	}

	if(_pst_Report->ul_Flag & COL_Cul_Triangle)
		L_strcat(asz_Log, "Triangle, ");
	else
	{
		if(_pst_Report->ul_Flag & COL_Cul_Edge)
			L_strcat(asz_Log, "Edge, ");
		else
			L_strcat(asz_Log, "Mathematical, ");
	}

	if(_pst_Report->ul_Flag & COL_Cul_Ground)
		L_strcat(asz_Log, "Ground ");
	else
		L_strcat(asz_Log, "Wall ");
	if(_pst_Report->ul_Flag & COL_Cul_SlipperyEdge) L_strcat(asz_Log, "(Slippery) ");

	if(_pst_Report->ul_Flag & COL_Cul_Static)
		L_strcat(asz_Log, "(Static) ");
	else
		L_strcat(asz_Log, "(Dynamic) ");

	if(_pst_Report->ul_Flag & COL_Cul_Extra_SlipperyEdge) L_strcat(asz_Log, "(-Extra- SLIPPERY) ");
	if(_pst_Report->ul_Flag & COL_Cul_Extra_Corner) L_strcat(asz_Log, "(-Extra- CORNER) ");
	if(_pst_Report->ul_Flag & COL_Cul_Extra_Minor) L_strcat(asz_Log, "(-Extra- MINOR) ");
	if(_pst_Report->ul_Flag & COL_Cul_Extra_ODE) L_strcat(asz_Log, "(-Extra- ODE) ");

	if(_pst_Report->ul_Flag & COL_Cul_Crossable) L_strcat(asz_Log, "(CROSSABLE) ");
	if(_pst_Report->ul_Flag & COL_Cul_Inactive) L_strcat(asz_Log, "(INACTIVE) ");
	if(_pst_Report->ul_Flag & COL_Cul_Invalid) L_strcat(asz_Log, "(INVALID) ");

	if(((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance->uw_Flags & COL_Cul_Recompute_GroundAndWall)
	{
		L_strcat(asz_Log, "[->Ground+Wall] ");
	}

	if(((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance->uw_Flags & COL_Cul_Recompute_GroundEdge)
	{
		L_strcat(asz_Log, "[->Ground-Edge] ");
	}

	if(((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance->uw_Flags & COL_Cul_Recompute_Wall)
	{
		L_strcat(asz_Log, "[->Wall] ");
	}

	if(((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance->uw_Flags & COL_Cul_Recompute_Dynamic)
	{
		L_strcat(asz_Log, "[->Dynamic] ");
	}

	if(((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance->uw_Flags & COL_Cul_Recompute_SlowFast)
	{
		L_strcat(asz_Log, "[->Slow-Fast] ");
	}

	if(((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance->uw_Flags & COL_Cul_OnlyGravity)
	{
		L_strcat(asz_Log, "(->Optimisation Gravity) ");
	}

	LINK_PrintStatusMsg(asz_Log);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_Debug_Trace(COL_tdst_GlobalVars *_pst_GlobalVars, char *asz_DbgGO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
	BOOL			b_FirstCol;
	float			f_Move;
	UCHAR			uc_NumFrames;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GlobalVars->b_Recomputing)
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[100];;
		/*~~~~~~~~~~~~~~~~~*/

		if(COL_gb_AfterEngine)
		{
			sprintf(asz_Log, "- [After Engine] Loop n°%i -", COL_uc_RecomputingLoop);
			LINK_PrintStatusMsg(asz_Log);
		}
		else
		{
			sprintf(asz_Log, "- Loop n°%i -", COL_uc_RecomputingLoop);
			LINK_PrintStatusMsg(asz_Log);
		}
	}

	if(_pst_GlobalVars->b_Recomputing)
	{
		pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->ul_FirstRecomputingReport;
		pst_LastReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->pst_World->ul_NbReports;
	}
	else
	{
		pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports;
		pst_LastReport = pst_CurrentReport + _pst_GlobalVars->pst_World->ul_NbReports;
	}

	b_FirstCol = TRUE;
	for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
	{
		if(!L_strcmp(pst_CurrentReport->pst_A->sz_Name, asz_DbgGO))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Move;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(b_FirstCol)
			{
				if(pst_CurrentReport->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Matrix	st_Matrix;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_MulMatrixMatrix
					(
						&st_Matrix,
						((COL_tdst_Base *) pst_CurrentReport->pst_A->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
						pst_CurrentReport->pst_A->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
					);
					MATH_SubVector(&st_Move, &pst_CurrentReport->pst_A->pst_GlobalMatrix->T, &st_Matrix.T);
				}
				else
				{
					MATH_SubVector
					(
						&st_Move,
						&pst_CurrentReport->pst_A->pst_GlobalMatrix->T,
						&(((COL_tdst_Base *) pst_CurrentReport->pst_A->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix->T
						)
					);
				}

				f_Move = MATH_f_NormVector(&st_Move);
				uc_NumFrames = ((UCHAR) (((TIM_gf_dt) * 60.0f) + 0.5f));
				b_FirstCol = FALSE;
			}

			COL_TraceCollisionReport(_pst_GlobalVars, pst_CurrentReport, &st_Move, uc_NumFrames);
		}
	}

	if(0 && b_FirstCol)
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[500];
		/*~~~~~~~~~~~~~~~~~*/

		if(_pst_GlobalVars->b_Recomputing)
		{
			pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->ul_FirstRecomputingReport;
			pst_LastReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->pst_World->ul_NbReports;
		}
		else
		{
			pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports;
			pst_LastReport = pst_CurrentReport + _pst_GlobalVars->pst_World->ul_NbReports;
		}

		for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
		{
			sprintf(asz_Log, "### %s", pst_CurrentReport->pst_A->sz_Name);
			LINK_PrintStatusMsg(asz_Log);
		}
	}
}
#endif
BOOL	COL_NeedRecomputation(COL_tdst_GlobalVars *, OBJ_tdst_GameObject *, ULONG);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_b_IsARealTopologicEdge
(
	COL_tdst_GlobalVars *_pst_GlobalVars,
	MATH_tdst_Vector	*pst_Face1,
	ULONG				_ul_CollisionType
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_VerticalColMap;
	BOOL				b_Face1IsAWall, b_Face2IsAWall;
	MATH_tdst_Vector	*pst_Face2, st_BCS_InvUnitG;
	float				f_Dot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector(&_pst_GlobalVars->st_B_Edge_Normal, pst_Face1);

	b_VerticalColMap = (fEqWithEpsilon(_pst_GlobalVars->pst_A_GlobalMatrix->Kz, 1.0f, 1E-3f));

	if(_ul_CollisionType & COL_Cul_Edge12)
	{
		if(_pst_GlobalVars->pst_B_Triangle->auw_Prox[0] != 0xFFFF)
		{
			pst_Face2 = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_FaceNormal[_pst_GlobalVars->pst_B_Triangle->auw_Prox[0]];
		}
		else
		{
			MATH_TransformVector(&st_BCS_InvUnitG, &_pst_GlobalVars->st_B_InvGlobalMatrix, _pst_GlobalVars->pst_InvUnitG);
			b_Face1IsAWall = (MATH_f_DotProduct(&st_BCS_InvUnitG, pst_Face1) < _pst_GlobalVars->f_WallCosAngle);
			if(!b_Face1IsAWall)
				MATH_CopyVector(&_pst_GlobalVars->st_B_Edge_Normal, &st_BCS_InvUnitG);
			return TRUE;
		}
	}
	else
	{
		if(_ul_CollisionType & COL_Cul_Edge13)
		{
			if(_pst_GlobalVars->pst_B_Triangle->auw_Prox[1] != 0xFFFF)
			{
				pst_Face2 = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_FaceNormal[_pst_GlobalVars->pst_B_Triangle->auw_Prox[1]];
			}
			else
			{
				MATH_TransformVector(&st_BCS_InvUnitG, &_pst_GlobalVars->st_B_InvGlobalMatrix, _pst_GlobalVars->pst_InvUnitG);
				b_Face1IsAWall = (MATH_f_DotProduct(&st_BCS_InvUnitG, pst_Face1) < _pst_GlobalVars->f_WallCosAngle);
				if(!b_Face1IsAWall)
					MATH_CopyVector(&_pst_GlobalVars->st_B_Edge_Normal, &st_BCS_InvUnitG);

				return TRUE;
			}
		}
		else
		{
			if(_pst_GlobalVars->pst_B_Triangle->auw_Prox[2] != 0xFFFF)
			{
				pst_Face2 = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_FaceNormal[_pst_GlobalVars->pst_B_Triangle->auw_Prox[2]];
			}
			else
			{
				MATH_TransformVector(&st_BCS_InvUnitG, &_pst_GlobalVars->st_B_InvGlobalMatrix, _pst_GlobalVars->pst_InvUnitG);
				b_Face1IsAWall = (MATH_f_DotProduct(&st_BCS_InvUnitG, pst_Face1) < _pst_GlobalVars->f_WallCosAngle);
				if(!b_Face1IsAWall)
					MATH_CopyVector(&_pst_GlobalVars->st_B_Edge_Normal, &st_BCS_InvUnitG);

				return TRUE;
			}
		}
	}

	MATH_TransformVector(&st_BCS_InvUnitG, &_pst_GlobalVars->st_B_InvGlobalMatrix, _pst_GlobalVars->pst_InvUnitG);
	MATH_AddVector(&_pst_GlobalVars->st_B_Edge_Normal, pst_Face1, pst_Face2);

	if(b_VerticalColMap)
	{
		b_Face1IsAWall = (MATH_f_DotProduct(&st_BCS_InvUnitG, pst_Face1) < _pst_GlobalVars->f_WallCosAngle);
		b_Face2IsAWall = (MATH_f_DotProduct(&st_BCS_InvUnitG, pst_Face2) < _pst_GlobalVars->f_WallCosAngle);
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_GCS_Face1, st_GCS_Face2;
		float				f;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_TransformVectorNoScale(&st_GCS_Face1, _pst_GlobalVars->pst_B_GlobalMatrix, pst_Face1);
		MATH_TransformVectorNoScale(&st_GCS_Face2, _pst_GlobalVars->pst_B_GlobalMatrix, pst_Face2);

		f = MATH_f_DotProduct(_pst_GlobalVars->pst_InvUnitG, &st_GCS_Face1);
		b_Face1IsAWall = ((f >= 0.0f) && (f < _pst_GlobalVars->f_WallCosAngle)) || ((f < 0.0f) && (f > -0.80f));

		f = MATH_f_DotProduct(_pst_GlobalVars->pst_InvUnitG, &st_GCS_Face2);
		b_Face2IsAWall = ((f >= 0.0f) && (f < _pst_GlobalVars->f_WallCosAngle)) || ((f < 0.0f) && (f > -0.80f));
	}

	MATH_MulEqualVector(&_pst_GlobalVars->st_B_Edge_Normal, 0.5f);

	f_Dot = MATH_f_DotProduct(pst_Face1, pst_Face2);

 	return ((b_Face1IsAWall || b_Face2IsAWall) && (f_Dot < fCos(Cf_Pi / 10.0f)));

	/*

	if(b_Face1IsAWall &&   b_Face2IsAWall && (f_Dot < fCos(Cf_Pi / 10.0f)))
		return TRUE;
	else
		return FALSE;
	*/
}


extern BOOL	COL_gb_SpecialCamera;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_b_CurrentElementIsCrossable(COL_tdst_GlobalVars *_pst_GlobalVars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_GameMat	*pst_GMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GMat = COL_pst_GMat_Get(_pst_GlobalVars->pst_B_Cob, _pst_GlobalVars->pst_B_Element);

	if(pst_GMat && ( ((USHORT) pst_GMat->ul_CustomBits) & _pst_GlobalVars->pst_A_Instance->uw_Crossable))
	{
		return TRUE;
	}

	return FALSE;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_b_SkipWithMaxStep(COL_tdst_GlobalVars *_pst_GlobalVars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_GCS_Ground, st_GCS_Temp;
	MATH_tdst_Vector	st_GCS_Pt[3], st_GCS_GroundPt[3];
	MATH_tdst_Vector	*pst_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_MulVector(&st_GCS_Temp, &_pst_GlobalVars->st_InvUnitG, -_pst_GlobalVars->f_A_GCS_ZDxRadius);
	MATH_TransformVertexNoScale(&st_GCS_Ground, _pst_GlobalVars->pst_B_GlobalMatrix, &_pst_GlobalVars->st_A_BCS_OldZDxCenter);
	MATH_AddEqualVector(&st_GCS_Ground, &st_GCS_Temp);

	pst_Point = _pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point;

	MATH_TransformVertexNoScale(&st_GCS_Pt[0], _pst_GlobalVars->pst_B_GlobalMatrix, &pst_Point[_pst_GlobalVars->pst_B_Triangle->auw_Index[0]]);
	MATH_TransformVertexNoScale(&st_GCS_Pt[1], _pst_GlobalVars->pst_B_GlobalMatrix, &pst_Point[_pst_GlobalVars->pst_B_Triangle->auw_Index[1]]);
	MATH_TransformVertexNoScale(&st_GCS_Pt[2], _pst_GlobalVars->pst_B_GlobalMatrix, &pst_Point[_pst_GlobalVars->pst_B_Triangle->auw_Index[2]]);


	MATH_SubVector(&st_GCS_GroundPt[0], &st_GCS_Pt[0], &st_GCS_Ground);
	MATH_SubVector(&st_GCS_GroundPt[1], &st_GCS_Pt[1], &st_GCS_Ground);
	MATH_SubVector(&st_GCS_GroundPt[2], &st_GCS_Pt[2], &st_GCS_Ground);
	
	if
	(
		(MATH_f_DotProduct(&st_GCS_GroundPt[0], &_pst_GlobalVars->st_InvUnitG) > _pst_GlobalVars->f_MaxStepSize)
	&&	(MATH_f_DotProduct(&st_GCS_GroundPt[1], &_pst_GlobalVars->st_InvUnitG) > _pst_GlobalVars->f_MaxStepSize)
	&&	(MATH_f_DotProduct(&st_GCS_GroundPt[2], &_pst_GlobalVars->st_InvUnitG) > _pst_GlobalVars->f_MaxStepSize)
	)
	{
		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_UpdateRecalUnitForVerticalWalls(MATH_tdst_Vector *_pst_A_BCS_RecalUnit, MATH_tdst_Vector *_pst_Global_Normal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Global_RecalUnit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* No Vertical Walls for Camera */
	if(COL_ACCESS(pst_A) == WOR_gpst_CurrentWorld->pst_View->pst_Father) return;

	if(_pst_Global_Normal)
		MATH_CopyVector(&st_Global_RecalUnit, _pst_Global_Normal);
	else
	{
		MATH_TransformVectorNoScale
		(
			&st_Global_RecalUnit,
			COL_ACCESS(pst_B_GlobalMatrix),
			_pst_A_BCS_RecalUnit
		);
	}

	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_A_Global_Normal;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_CopyVector(&st_A_Global_Normal, &st_Global_RecalUnit);
		st_A_Global_Normal.z = 0.0f;
		MATH_NormalizeEqualVector(&st_A_Global_Normal);

		MATH_TransformVectorNoScale
		(
			_pst_A_BCS_RecalUnit,
			&(COL_ACCESS(st_B_InvGlobalMatrix)),
			&st_A_Global_Normal
		);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
extern unsigned int WOR_gul_WorldKey;
#else
extern ULONG WOR_gul_WorldKey;
#endif
/*$F
 =======================================================================================================================

											REPORT RECALAGE COMPUTATION

 =======================================================================================================================
 */
BOOL COL_ComputeReport
(
	MATH_tdst_Vector 	*_pst_BCS_Hit,
	MATH_tdst_Vector	*_pst_BCS_Normal,
	MATH_tdst_Vector	*_pst_Global_Hit,
	MATH_tdst_Vector	*_pst_Global_Normal,
	MATH_tdst_Vector	*_pst_Global_Recal,
	ULONG				*pul_CollisionType,
	ULONG				_ul_Triangle,
	ULONG				_ul_Element,
	COL_tdst_GlobalVars *_pst_GlobalVars
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_BCS_VectHitCenter, st_BCS_Recal, st_A_BCS_RecalUnit, st_BCS_Normal;
	MATH_tdst_Vector	st_BCS_HitOldCenter;
	float				f_NormHitCenter;
	float				f_Recalage, f;
	BOOL				b_DeepImpact, b_SlipperyEdge;
	MATH_tdst_Vector	st_Global_InvGravity;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_TransformVectorNoScale(_pst_Global_Normal, COL_ACCESS(pst_B_GlobalMatrix), _pst_BCS_Normal);
	MATH_TransformVertexNoScale(_pst_Global_Hit, COL_ACCESS(pst_B_GlobalMatrix), _pst_BCS_Hit);
		

	f = MATH_f_DotProduct(COL_ACCESS(pst_InvUnitG), _pst_Global_Normal);

	/* Is it a Wall or a Ground ? */
	if
	(
		((f < COL_ACCESS(f_WallCosAngle)) && (f > -0.8f))
	||	((*pul_CollisionType & COL_Cul_Cylinder) && ((_pst_Global_Normal->x != 0.0f) || (_pst_Global_Normal->y != 0.0f)))
	)
	{
		*pul_CollisionType |= COL_Cul_Wall;

		 /*	Ground and Wall Reports at a same Iteration. Skip Walls, only deal with Grounds and call for recomputation ... */
		if(COL_Instance_TestFlag(COL_ACCESS(pst_A_Instance), COL_Cul_StaticGround) && !COL_b_CurrentElementIsCrossable(_pst_GlobalVars))
		{
			COL_NeedRecomputation(_pst_GlobalVars, COL_ACCESS(pst_A), COL_Cul_Recompute_GroundAndWall);
			return FALSE;
		}
	}
	else
	{
		*pul_CollisionType |= COL_Cul_Ground;

		/* Gravity ground/ceiling */
		if(f < 0.0f)
		{
			MATH_NegVector(&st_Global_InvGravity, COL_ACCESS(pst_InvUnitG));
			((COL_tdst_Base *) _pst_GlobalVars->pst_A->pst_Extended->pst_Col)->pst_Instance->uw_Flags |= COL_Cul_ForceRecomputeWhenOnlyGround;
		}
		else
			MATH_CopyVector(&st_Global_InvGravity, COL_ACCESS(pst_InvUnitG));
	}

	MATH_SubVector(&st_BCS_HitOldCenter, &(COL_ACCESS(st_A_BCS_OldZDxCenter)), _pst_BCS_Hit);

	/* BackFace Test with the Old GlobalMatrix */
	if((COL_ACCESS(f_MaxStepSize) <= 0.0f) && (MATH_f_DotProduct(&st_BCS_HitOldCenter, _pst_BCS_Normal) <= 0.0f))
	{
		*pul_CollisionType |= COL_Cul_Invalid + COL_Cul_Inactive;
		COL4Edit_SetReportDebugFlag(1);
		return TRUE;
	}

	/* Max Step size */
	if
	(
		(COL_ACCESS(f_MaxStepSize) > 0.0f) 
	&&	(*pul_CollisionType & COL_Cul_Ground) 
	&&	(*pul_CollisionType & (COL_Cul_Triangle | COL_Cul_Edge))
	&&	(COL_b_SkipWithMaxStep(_pst_GlobalVars))
	)
	{
		*pul_CollisionType |= COL_Cul_Invalid + COL_Cul_Inactive;
		COL4Edit_SetReportDebugFlag(4);
		return TRUE;
	}

	MATH_SubVector(&st_BCS_VectHitCenter, &(COL_ACCESS(st_A_BCS_ZDxCenter)), _pst_BCS_Hit);

	/* Have we crossed more than half the ZDM diameter ? */
	b_DeepImpact = (MATH_f_DotProduct(&st_BCS_VectHitCenter, _pst_BCS_Normal) < 0.0f);

	/* Did we collide a Slippery Edge ? */
	b_SlipperyEdge =
		(
			((*pul_CollisionType) & COL_Cul_Edge)
//		&&	((*pul_CollisionType) & COL_Cul_Wall)
		&&	((*pul_CollisionType) & COL_Cul_Static)
		&&	(COL_b_IsARealTopologicEdge(_pst_GlobalVars, _pst_BCS_Normal, *pul_CollisionType))
		);

	/* Flag Dyna to prevent slippery edges on Grounds ? */
	if
	(
		b_SlipperyEdge
	&& ((*pul_CollisionType) & COL_Cul_Ground) 
	&& (_pst_GlobalVars->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) 
	&& !(_pst_GlobalVars->pst_A->pst_Base->pst_Dyna->ul_DynFlags & DYN_C_SlipOnGroundEdges)
	)
	{
		b_SlipperyEdge = FALSE;
	}

	if(b_SlipperyEdge)
		*pul_CollisionType |= COL_Cul_SlipperyEdge;

	f_Recalage = 0.0f;


	/*$F --------------------------- RECALAGE DIRECTION ------------------------------ */
	if((*pul_CollisionType) & COL_Cul_Ground)
	{
		if(b_SlipperyEdge)
		{
			MATH_NormalizeVector(&st_BCS_Normal, &(COL_ACCESS(st_B_Edge_Normal)));	// Slippery Edge
			MATH_CopyVector(&st_A_BCS_RecalUnit, &st_BCS_Normal);
		}
		else
		{
			MATH_TransformVectorNoScale(_pst_Global_Normal, COL_ACCESS(pst_B_GlobalMatrix), _pst_BCS_Normal);	

			if(!(*pul_CollisionType & COL_Cul_Dynamic) || (_pst_Global_Normal->z < 0.5f)) // || (WOR_gul_WorldKey != 0x180016af))
			{
				MATH_TransformVector(&st_A_BCS_RecalUnit, &(COL_ACCESS(st_B_InvGlobalMatrix)), &st_Global_InvGravity);
				MATH_NormalizeEqualVector(&st_A_BCS_RecalUnit);
			}
			else
			{
				//POPOWARNING CODE DIFFERENT XENON
				MATH_TransformVector(&st_A_BCS_RecalUnit, &(COL_ACCESS(st_B_InvGlobalMatrix)), _pst_Global_Normal);
				MATH_NormalizeEqualVector(&st_A_BCS_RecalUnit);
			}
		}
	}
	else
	{
		if(b_SlipperyEdge || (COL_gb_SpecialCamera && !((*pul_CollisionType) & COL_Cul_Static)))
		{
			if(b_SlipperyEdge)
				MATH_NormalizeVector(&st_BCS_Normal, &(COL_ACCESS(st_B_Edge_Normal)));	// Slippery Edge
			else
				MATH_NormalizeVector(&st_BCS_Normal, &st_BCS_HitOldCenter);				// Dynamic Camera

			MATH_TransformVectorNoScale(_pst_Global_Normal, COL_ACCESS(pst_B_GlobalMatrix), &st_BCS_Normal);
			COL_UpdateRecalUnitForVerticalWalls(&st_A_BCS_RecalUnit, _pst_Global_Normal);
		}
		else
		{
			MATH_CopyVector(&st_A_BCS_RecalUnit, _pst_BCS_Normal);
			if(!(*pul_CollisionType & COL_Cul_Dynamic))
				COL_UpdateRecalUnitForVerticalWalls(&st_A_BCS_RecalUnit, _pst_Global_Normal);

			/* For KONG ... Sometimes, Kong is just on a corner and shakes (rebound between one face to the other of the corner) */
			/* We modify the recal vector when we find this case to avoid shaking */
			if(((*pul_CollisionType) & COL_Cul_Triangle) && ((COL_ACCESS(pst_A_Instance))->uw_Flags & COL_Cul_CornerFound))
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				float					f_Dot;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				f_Dot = MATH_f_DotProduct(&(COL_ACCESS(pst_A_Instance))->st_LastWall_Normal, _pst_Global_Normal);

				if((f_Dot < 0.95f) && (f_Dot > -0.95f))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector		st_Cross, st_Temp;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_CrossProduct(&st_Temp, &(COL_ACCESS(pst_A_Instance))->st_LastWall_Normal, _pst_Global_Normal);
					MATH_CrossProduct(&st_Cross, &st_Temp, &(COL_ACCESS(pst_A_Instance))->st_LastWall_Normal);
					MATH_TransformVector(&st_A_BCS_RecalUnit, &(COL_ACCESS(st_B_InvGlobalMatrix)), &st_Cross);
					f_Dot = MATH_f_DotProduct(&st_A_BCS_RecalUnit, _pst_BCS_Normal);
					if(f_Dot > 0.0f)
					{
						MATH_NormalizeEqualVector(&st_A_BCS_RecalUnit);
						COL_UpdateRecalUnitForVerticalWalls(&st_A_BCS_RecalUnit, NULL);
					}
				}
			}
		}
	}

	/*$F --------------------------- RECALAGE VALUE ------------------------------ */
	if(COL_b_Zone_TestType(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_Sphere))
	{
		if((*pul_CollisionType) & COL_Cul_Static)
		{
			/*$F ----------------------- STATIC COLLISION ----------------------- */
			if
			(
				((*pul_CollisionType) & COL_Cul_Triangle)
			||	((*pul_CollisionType) & COL_Cul_Cylinder)
			||	((*pul_CollisionType) & COL_Cul_Box)
			||	((*pul_CollisionType) & COL_Cul_Sphere)
			)
			{
				f_NormHitCenter = MATH_f_NormVector(&st_BCS_VectHitCenter);

				if(b_DeepImpact)
					f_Recalage = fDiv(_pst_GlobalVars->f_A_GCS_ZDxRadius + f_NormHitCenter, MATH_f_DotProduct(&st_A_BCS_RecalUnit, _pst_BCS_Normal));
				else
					f_Recalage = fDiv(_pst_GlobalVars->f_A_GCS_ZDxRadius - f_NormHitCenter, MATH_f_DotProduct(&st_A_BCS_RecalUnit, _pst_BCS_Normal));

				/*$F
				* [Static Ground Triangle Case]
				* ----------------------------------
				* ---> VERY SLOPY TRIANGLE CASE <---
				* ----------------------------------
				*  NB: (It is the reason why we check the 0.95 z value of the normal to avoid this test when we deal with real horizontal triangles)
				*
				*  We check that the destination point is really ON the Triangle and not outside it. It can be outside cause we're using a vertical vector to move the sphere.
				*  If it is the case, we use the Edge Algorithm that will give a valid point on the triangle and call for recomputaion.
				*
				*/
				if
				(
					((*pul_CollisionType) & COL_Cul_Ground)
				&&	((*pul_CollisionType) & COL_Cul_Triangle)
				&&	(_pst_Global_Normal->z < 0.95f)
				)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	st_Point2check, st_Temp;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_MulVector(&st_Point2check, _pst_BCS_Normal, _pst_GlobalVars->f_A_GCS_ZDxRadius);
					MATH_NegEqualVector(&st_Point2check);
					MATH_AddEqualVector(&st_Point2check, &_pst_GlobalVars->st_A_BCS_ZDxCenter);
					MATH_MulVector(&st_Temp, &st_A_BCS_RecalUnit, f_Recalage);
					MATH_AddEqualVector(&st_Point2check, &st_Temp);

					if
					(
						!INT_PointInTriangle
						(
							&st_Point2check,
							&_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point[_pst_GlobalVars->pst_B_Element->dst_Triangle[_ul_Triangle].auw_Index[0]],
							&_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point[_pst_GlobalVars->pst_B_Element->dst_Triangle[_ul_Triangle].auw_Index[1]],
							&_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point[_pst_GlobalVars->pst_B_Element->dst_Triangle[_ul_Triangle].auw_Index[2]],
							_pst_BCS_Normal,
							NULL
						)
					)
					{
						*pul_CollisionType &= ~COL_Cul_Triangle;
						*pul_CollisionType |= COL_Cul_Edge;

						/* [Static Edge Algorithm used] */
						{
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
							float	f_SqrNormHitCenter, f_SqrRadius;
							float	f_Dot, f_SqrDot;
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

							f_SqrNormHitCenter = MATH_f_SqrNormVector(&st_BCS_VectHitCenter);
							f_SqrRadius = fSqr(_pst_GlobalVars->f_A_GCS_ZDxRadius);
							f_Dot = MATH_f_DotProduct(&st_BCS_VectHitCenter, &st_A_BCS_RecalUnit);
							f_SqrDot = fSqr(f_Dot);
							f_Recalage = -f_Dot + fSqrt(f_SqrDot + f_SqrRadius - f_SqrNormHitCenter);

							//POPPOWARNING XENON CODE DIFFERENT
							/* Force recomputation in that case but only once */
							if(!COL_Instance_TestFlag(COL_pst_GetInstance(_pst_GlobalVars->pst_A, TRUE), COL_Cul_Recompute_TriangleEdge))
								COL_NeedRecomputation(_pst_GlobalVars, _pst_GlobalVars->pst_A, COL_Cul_Recompute_TriangleEdge);
						}
					}
				}
			}
			else	/* [Static Edge Case] */
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				float	f_SqrNormHitCenter, f_SqrRadius;
				float	f_Dot, f_SqrDot;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				f_SqrNormHitCenter = MATH_f_SqrNormVector(&st_BCS_VectHitCenter);
				f_SqrRadius = fSqr(_pst_GlobalVars->f_A_GCS_ZDxRadius);
				f_Dot = MATH_f_DotProduct(&st_BCS_VectHitCenter, &st_A_BCS_RecalUnit);
				f_SqrDot = fSqr(f_Dot);
				f_Recalage = -f_Dot + fSqrt(f_SqrDot + f_SqrRadius - f_SqrNormHitCenter);
			}
		}
		else
		{
			/*$F ----------------------- DYNAMIC COLLISION ----------------------- */

			/*$F
			* We dont perform a "perfect" and complicated recalage because there will be a
			* collision recomputation where the computation will be more precise.
			*/
			f_Recalage = -MATH_f_DotProduct(&st_BCS_VectHitCenter, &st_A_BCS_RecalUnit);
		}
	}
#if 0		/* Box ZDM --- desactivated */
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	*pst_Min, *pst_Max;
		MATH_tdst_Vector	st_Temp, st_Global_RecalUnit;
		float				f_Recalage;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Max = COL_pst_Shape_GetMax(_pst_GlobalVars->pst_A_ZDx->p_Shape);
		pst_Min = COL_pst_Shape_GetMin(_pst_GlobalVars->pst_A_ZDx->p_Shape);

		MATH_TransformVectorNoScale(&st_Global_RecalUnit, _pst_GlobalVars->pst_B_GlobalMatrix, &st_A_BCS_RecalUnit);
		f_Recalage = -100000.0f;

		if(*pul_CollisionType & COL_Cul_Triangle)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_OneTriPoint;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
			MATH_TransformVertexNoScale
			(
				&st_OneTriPoint, 
				_pst_GlobalVars->pst_B_GlobalMatrix, 
				&_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point[_pst_GlobalVars->pst_B_Element->dst_Triangle[_ul_Triangle].auw_Index[0]]
			);
			MATH_SubVector(&st_Temp, &st_OneTriPoint, _pst_Global_Hit);
			f_Recalage = MATH_f_DotProduct(&st_Temp, _pst_Global_Normal);
		}
		else if (*pul_CollisionType & COL_Cul_Edge)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix	st_A_InvGlobalMatrix;
			MATH_tdst_Vector	st_ACS_BoxPoint;
			MATH_tdst_Vector	st_ACS_Hit, st_ACS_RecalUnit;
			MATH_tdst_Vector	st_ACS_HitBox;
			MATH_tdst_Vector	st_ACS_V1;
			float				Alpha;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_InvertMatrix(&st_A_InvGlobalMatrix, _pst_GlobalVars->pst_A_GlobalMatrix);
			MATH_TransformVertex(&st_ACS_Hit, &st_A_InvGlobalMatrix, _pst_Global_Hit);
			MATH_TransformVector(&st_ACS_RecalUnit, &st_A_InvGlobalMatrix, _pst_Global_Normal);


			// Plan Max X
			st_ACS_BoxPoint.x = pst_Max->x;
			st_ACS_BoxPoint.y = pst_Min->y;
			st_ACS_BoxPoint.z = pst_Min->z;

			MATH_SubVector(&st_ACS_HitBox, &st_ACS_BoxPoint, &st_ACS_Hit);

			Alpha = st_ACS_HitBox.x / (st_ACS_RecalUnit.x);

			f_Recalage = 1000000.0f;

			if((Alpha < 0.0f) && (- Alpha < f_Recalage))
			{
				MATH_MulVector(&st_ACS_V1, &st_ACS_RecalUnit, Alpha);
				MATH_AddEqualVector(&st_ACS_V1, &st_ACS_Hit);

				if
				(
					(st_ACS_V1.y < pst_Max->y)
				&&  (st_ACS_V1.y > pst_Min->y)
				&&  (st_ACS_V1.z < pst_Max->z)
				&&  (st_ACS_V1.z > pst_Min->z)
				)
				{
					f_Recalage = -Alpha;
					MATH_TransformVertex(_pst_Global_Hit, _pst_GlobalVars->pst_A_GlobalMatrix, &st_ACS_V1);
				}
			}

			// Plan Min X
			st_ACS_BoxPoint.x = pst_Min->x;
			st_ACS_BoxPoint.y = pst_Min->y;
			st_ACS_BoxPoint.z = pst_Min->z;

			MATH_SubVector(&st_ACS_HitBox, &st_ACS_BoxPoint, &st_ACS_Hit);

			Alpha = st_ACS_HitBox.x / st_ACS_RecalUnit.x;

			if(Alpha < 0.0f && (- Alpha < f_Recalage))
			{
				MATH_MulVector(&st_ACS_V1, &st_ACS_RecalUnit, Alpha);
				MATH_AddEqualVector(&st_ACS_V1, &st_ACS_Hit);

				if
				(
					(st_ACS_V1.y < pst_Max->y + 1E-3f)
				&&  (st_ACS_V1.y > pst_Min->y - 1E-3f)
				&&  (st_ACS_V1.z < pst_Max->z + 1E-3f)
				&&  (st_ACS_V1.z > pst_Min->z - 1E-3f)
				)
				{
					f_Recalage = -Alpha;
					MATH_TransformVertex(_pst_Global_Hit, _pst_GlobalVars->pst_A_GlobalMatrix, &st_ACS_V1);
				}

			}


			// Plan Max Y
			st_ACS_BoxPoint.x = pst_Min->x;
			st_ACS_BoxPoint.y = pst_Max->y;
			st_ACS_BoxPoint.z = pst_Min->z;

			MATH_SubVector(&st_ACS_HitBox, &st_ACS_BoxPoint, &st_ACS_Hit);

			Alpha = st_ACS_HitBox.y / st_ACS_RecalUnit.y;

			if((Alpha < 0.0f) && (- Alpha < f_Recalage))
			{
				MATH_MulVector(&st_ACS_V1, &st_ACS_RecalUnit, Alpha);
				MATH_AddEqualVector(&st_ACS_V1, &st_ACS_Hit);

				if
				(
					(st_ACS_V1.x < pst_Max->x + 1E-3f)
				&&  (st_ACS_V1.x > pst_Min->x - 1E-3f)
				&&  (st_ACS_V1.z < pst_Max->z + 1E-3f)
				&&  (st_ACS_V1.z > pst_Min->z - 1E-3f)
				)
				{
					f_Recalage = -Alpha;
					MATH_TransformVertex(_pst_Global_Hit, _pst_GlobalVars->pst_A_GlobalMatrix, &st_ACS_V1);
				}

			}

			// Plan Min Y
			st_ACS_BoxPoint.x = pst_Min->x;
			st_ACS_BoxPoint.y = pst_Min->y;
			st_ACS_BoxPoint.z = pst_Min->z;

			MATH_SubVector(&st_ACS_HitBox, &st_ACS_BoxPoint, &st_ACS_Hit);

			Alpha = st_ACS_HitBox.y / st_ACS_RecalUnit.y;

			if((Alpha < 0.0f) && (- Alpha < f_Recalage))
			{
				MATH_MulVector(&st_ACS_V1, &st_ACS_RecalUnit, Alpha);
				MATH_AddEqualVector(&st_ACS_V1, &st_ACS_Hit);

				if
				(
					(st_ACS_V1.x < pst_Max->x + 1E-3f)
				&&  (st_ACS_V1.x > pst_Min->x - 1E-3f)
				&&  (st_ACS_V1.z < pst_Max->z + 1E-3f)
				&&  (st_ACS_V1.z > pst_Min->z - 1E-3f)
				)
				{
					f_Recalage = -Alpha;
					MATH_TransformVertex(_pst_Global_Hit, _pst_GlobalVars->pst_A_GlobalMatrix, &st_ACS_V1);
				}
			}

			// Plan Max Z
			st_ACS_BoxPoint.x = pst_Min->x;
			st_ACS_BoxPoint.y = pst_Min->y;
			st_ACS_BoxPoint.z = pst_Max->z;

			MATH_SubVector(&st_ACS_HitBox, &st_ACS_BoxPoint, &st_ACS_Hit);

			Alpha = st_ACS_HitBox.z / st_ACS_RecalUnit.z;

			if((Alpha < 0.0f) && (- Alpha < f_Recalage))
			{
				MATH_MulVector(&st_ACS_V1, &st_ACS_RecalUnit, Alpha);
				MATH_AddEqualVector(&st_ACS_V1, &st_ACS_Hit);

				if
				(
					(st_ACS_V1.y < pst_Max->y + 1E-3f)
				&&  (st_ACS_V1.y > pst_Min->y - 1E-3f)
				&&  (st_ACS_V1.x < pst_Max->x + 1E-3f)
				&&  (st_ACS_V1.x > pst_Min->x - 1E-3f)
				)
				{
					f_Recalage = -Alpha;
					MATH_TransformVertex(_pst_Global_Hit, _pst_GlobalVars->pst_A_GlobalMatrix, &st_ACS_V1);
				}

			}

			// Plan Min Z
			st_ACS_BoxPoint.x = pst_Min->x;
			st_ACS_BoxPoint.y = pst_Min->y;
			st_ACS_BoxPoint.z = pst_Min->z;

			MATH_SubVector(&st_ACS_HitBox, &st_ACS_BoxPoint, &st_ACS_Hit);

			Alpha = st_ACS_HitBox.x / st_ACS_RecalUnit.x;

			if((Alpha < 0.0f) && (- Alpha < f_Recalage))
			{
				MATH_MulVector(&st_ACS_V1, &st_ACS_RecalUnit, Alpha);
				MATH_AddEqualVector(&st_ACS_V1, &st_ACS_Hit);


				if
				(
					(st_ACS_V1.y < pst_Max->y + 1E-3f)
				&&  (st_ACS_V1.y > pst_Min->y - 1E-3f)
				&&  (st_ACS_V1.x < pst_Max->x + 1E-3f)
				&&  (st_ACS_V1.x > pst_Min->x - 1E-3f)
				)
				{
					f_Recalage = -Alpha;
					MATH_TransformVertex(_pst_Global_Hit, _pst_GlobalVars->pst_A_GlobalMatrix, &st_ACS_V1);
				}
			}

			if(f_Recalage == 1000000.0f)
				f_Recalage *= -1.0f;

		}

		/* If the recalage is < 1 mm, we skip it, to avoid Collision loops. */
		if(f_Recalage < 1E-3f)
		{
			if(*pul_CollisionType & COL_Cul_Dynamic)
			{
				*pul_CollisionType |= COL_Cul_Dynamic;
			}

			*pul_CollisionType |= COL_Cul_Extra_Minor;
			MATH_InitVector(_pst_Global_Recal, 0.0f, 0.0f, 0.0f);
			return TRUE;
		}


		MATH_MulVector(_pst_Global_Recal, &st_Global_RecalUnit, f_Recalage);

#ifdef ACTIVE_EDITORS
	MATH_CheckVector(_pst_Global_Recal, "Collision recalage error.");
#endif

		return TRUE;
	}
#endif //0

	/*
	 * We have found a valid Slippery Edge but we have lost the real normal of its
	 * face (cause we have updated it).
	 * We create an Empty report to store the real face normal for corner
	 * computation. If Corner computation has already been done, it is useless.
	 */
	if
	(
		b_SlipperyEdge
	&&	!(_pst_GlobalVars->pst_A_Instance->uw_Flags & COL_Cul_CornerComputed)
	&&	(_pst_GlobalVars->ul_ReportIndex < COL_Cul_MaxNbOfCollision)
	)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World		*pst_World;
		ULONG				ul_ReportIndex;
		COL_tdst_GameMat	*pst_GMat;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_ReportIndex = _pst_GlobalVars->ul_ReportIndex;
		pst_World = _pst_GlobalVars->pst_World;
		pst_World->ast_Reports[ul_ReportIndex].st_CollidedPoint = *_pst_Global_Hit;
		pst_World->ast_Reports[ul_ReportIndex].st_Recal = MATH_gst_NulVector;
		MATH_TransformVectorNoScale
		(
			&pst_World->ast_Reports[ul_ReportIndex].st_Normal,
			_pst_GlobalVars->pst_B_GlobalMatrix,
			_pst_BCS_Normal
		);

		pst_World->ast_Reports[ul_ReportIndex].pst_A = _pst_GlobalVars->pst_A;
		pst_World->ast_Reports[ul_ReportIndex].pst_B = _pst_GlobalVars->pst_B;

		pst_World->ast_Reports[ul_ReportIndex].ul_Flag = (*pul_CollisionType & COL_Cul_Edge) | COL_Cul_Static | (*pul_CollisionType & COL_Cul_Wall) | (*pul_CollisionType & COL_Cul_Ground) | COL_Cul_Extra_SlipperyEdge;
		pst_World->ast_Reports[ul_ReportIndex].ul_Triangle = _ul_Triangle;
		pst_World->ast_Reports[ul_ReportIndex].uw_Element = (USHORT) _ul_Element;

		pst_GMat = COL_pst_GMat_Get(_pst_GlobalVars->pst_B_Cob, _pst_GlobalVars->pst_B_Element);
		pst_World->ast_Reports[ul_ReportIndex].pst_GM = pst_GMat;
		if(pst_GMat && (((USHORT) pst_GMat->ul_CustomBits) & _pst_GlobalVars->pst_A_Instance->uw_Crossable))
			COL_Report_SetFlag(&pst_World->ast_Reports[ul_ReportIndex], COL_Cul_Crossable);

		pst_World->ast_Reports[ul_ReportIndex].pst_GM = pst_GMat;

		_pst_GlobalVars->ul_ReportIndex++;
	}

	/* If the recalage is < 1 mm, we skip it, to avoid useless Collision loops. */
	if(f_Recalage < 1E-3f)
	{
			if(*pul_CollisionType & COL_Cul_Dynamic)
			{
				*pul_CollisionType |= COL_Cul_Dynamic;
			}

		*pul_CollisionType |= COL_Cul_Extra_Minor;
		MATH_InitVector(_pst_Global_Recal, 0.0f, 0.0f, 0.0f);
		return TRUE;
	}

	f_Recalage += 1E-5f;

	MATH_MulVector(&st_BCS_Recal, &st_A_BCS_RecalUnit, f_Recalage);


	if(*pul_CollisionType & COL_Cul_Ground)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_GameMat	*pst_GMat;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_GMat = COL_pst_GMat_Get(_pst_GlobalVars->pst_B_Cob, _pst_GlobalVars->pst_B_Element);

		if(!pst_GMat || !(((USHORT) pst_GMat->ul_CustomBits) & _pst_GlobalVars->pst_A_Instance->uw_Crossable))
		{
			/* Set the Flag to indicate we found a Ground collision ... That will be used to skip Wall collisions
			 * during this loop (never both valid ground AND wall collisions during a collision loop) */
			COL_Instance_SetFlag(_pst_GlobalVars->pst_A_Instance, COL_Cul_StaticGround);

			/*
			 * Real Topological Ground-Edges (breaks and cracks) needs recomputation to
			 * be sure of the recaled position. By the way, we dont need to recompute if it is
			 * a "classical" Ground-Edge, that is to say when the 2 faces touched by the edges
			 * have nearly the same normal. (~Optimisation)
			 */
			if(*pul_CollisionType & COL_Cul_Edge)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	*pst_Face = NULL;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(*pul_CollisionType & COL_Cul_Edge12)
				{
					USHORT uwProx = _pst_GlobalVars->pst_B_Triangle->auw_Prox[0];
					if(uwProx != 0xFFFF)
						pst_Face = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_FaceNormal[uwProx];
					else
						pst_Face = NULL;
				}

				if(*pul_CollisionType & COL_Cul_Edge13)
				{
					USHORT uwProx = _pst_GlobalVars->pst_B_Triangle->auw_Prox[1];
					if(uwProx != 0xFFFF)
						pst_Face = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_FaceNormal[uwProx];
					else
						pst_Face = NULL;
				}

				if(*pul_CollisionType & COL_Cul_Edge23)
				{
					USHORT uwProx = _pst_GlobalVars->pst_B_Triangle->auw_Prox[2];
					if(uwProx != 0xFFFF)
						pst_Face = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_FaceNormal[uwProx];
					else
						pst_Face = NULL;
				}

				if(pst_Face && (MATH_f_DotProduct(pst_Face, _pst_BCS_Normal) < 0.95f))
				{
					COL_NeedRecomputation(_pst_GlobalVars, _pst_GlobalVars->pst_A, COL_Cul_Recompute_GroundEdge);
				}
			}
		}
	}

#ifdef ACTIVE_EDITORS
	MATH_CheckVector(&st_BCS_Recal, "Collision recalage error.");
#endif
		

	/*
	 * If B has scale, we deal with a precomputed map where the points are not scaled
	 * any longer. Everything is computed with this new "no_scale" object and so, we
	 * have to use the "NoScale" Mathematical function to be coherent.
	 */
	MATH_TransformVectorNoScale(_pst_Global_Recal, _pst_GlobalVars->pst_B_GlobalMatrix, &st_BCS_Recal);	

	return TRUE;
}

/*$F
 =======================================================================================================================
    
									CREATES A NEW COLLISION REPORT

 =======================================================================================================================
 */

void COL_ReportCollision
(
	MATH_tdst_Vector	*_pst_Global_CollidedPoint,
	MATH_tdst_Vector	*_pst_Global_Normal,
	MATH_tdst_Vector	*_pst_Global_Recal,
	ULONG				_ul_Flag,
	ULONG				_ul_Triangle,
	ULONG				_ul_Element,
	COL_tdst_GlobalVars *_pst_GlobalVars
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	COL_tdst_GameMat	*pst_GMat;
	ULONG				ul_ReportIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	// - NOTE -
	// COL_Cul_MaxNbOfCollision has been incremented from 1000 to 1500. In 03C, the assert occured
	// a few times, without being able to reproduce it in debug. The ERR_X_Warning should be able to
	// warns when we get close to the new limit.
	
	ERR_X_Assert(_pst_GlobalVars->ul_ReportIndex < COL_Cul_MaxNbOfCollision);
	ERR_X_Warning(_pst_GlobalVars->ul_ReportIndex <= 0.8 * COL_Cul_MaxNbOfCollision, "Max number of collision reports (COL_tdst_GlobalVars::ul_ReportIndex) is nearly reached - Contact Programming Team.", NULL);
#endif
#ifdef _DEBUG
	ERR_X_Assert(_pst_GlobalVars->ul_ReportIndex < COL_Cul_MaxNbOfCollision);
	if(_pst_GlobalVars->ul_ReportIndex >= COL_Cul_MaxNbOfCollision) return;
#else
	if(_pst_GlobalVars->ul_ReportIndex >= COL_Cul_MaxNbOfCollision) return;
#endif
	ul_ReportIndex = _pst_GlobalVars->ul_ReportIndex;
	pst_World = _pst_GlobalVars->pst_World;
	pst_World->ast_Reports[ul_ReportIndex].st_CollidedPoint = *_pst_Global_CollidedPoint;
	pst_World->ast_Reports[ul_ReportIndex].st_Recal = *_pst_Global_Recal;
	pst_World->ast_Reports[ul_ReportIndex].st_Normal = *_pst_Global_Normal;

	pst_World->ast_Reports[ul_ReportIndex].pst_A = _pst_GlobalVars->pst_A;
	pst_World->ast_Reports[ul_ReportIndex].pst_B = _pst_GlobalVars->pst_B;
	pst_World->ast_Reports[ul_ReportIndex].ul_Flag = _ul_Flag;
	pst_World->ast_Reports[ul_ReportIndex].ul_Triangle = _ul_Triangle;
	pst_World->ast_Reports[ul_ReportIndex].uw_Element = (USHORT) _ul_Element;

	if(_ul_Flag & (COL_Cul_Triangle | COL_Cul_Edge))
	{
		pst_GMat = COL_pst_GMat_Get(_pst_GlobalVars->pst_B_Cob, _pst_GlobalVars->pst_B_Element);
		pst_World->ast_Reports[ul_ReportIndex].uc_Design = _pst_GlobalVars->pst_B_Element->uc_Design;
		pst_World->ast_Reports[ul_ReportIndex].pst_GM = pst_GMat;
		if(pst_GMat && (((USHORT) pst_GMat->ul_CustomBits) & _pst_GlobalVars->pst_A_Instance->uw_Crossable))
			COL_Report_SetFlag(&pst_World->ast_Reports[ul_ReportIndex], COL_Cul_Crossable);
	}
	else
	{
		pst_GMat = COL_pst_GMat_Get(_pst_GlobalVars->pst_B_Cob, NULL);
		pst_World->ast_Reports[ul_ReportIndex].uc_Design = 0;
		pst_World->ast_Reports[ul_ReportIndex].pst_GM = pst_GMat;
		if(pst_GMat && (((USHORT) pst_GMat->ul_CustomBits) & _pst_GlobalVars->pst_A_Instance->uw_Crossable))
			COL_Report_SetFlag(&pst_World->ast_Reports[ul_ReportIndex], COL_Cul_Crossable);
	}

	_pst_GlobalVars->ul_ReportIndex++;
	pst_World->ul_NbReports = _pst_GlobalVars->ul_ReportIndex;
}

/*$F
 =======================================================================================================================

									UPDATE DYNAMIC HIERARCHY LINK

 =======================================================================================================================
 */

void COL_UpdateDynamicHierarchyLink(OBJ_tdst_GameObject *_pst_Child, OBJ_tdst_GameObject *_pst_Father)
{
	if(COL_b_HierarchyLinkDestructionTest(_pst_Child, _pst_Father))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix st_Temp	ONLY_PSX2_ALIGNED(16);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		OBJ_ComputeGlobalWhenHie(_pst_Child);

		/*
		 * Old Global Matrix, which in in Local Coordinate System of the Father need to be
		 * recomputed in Global.
		 */
		MATH_MulMatrixMatrix
		(
			&st_Temp,
			((COL_tdst_Base *) _pst_Child->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
			_pst_Child->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
		);

		MATH_CopyMatrix
		(
			((COL_tdst_Base *) _pst_Child->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
			&st_Temp
		);

		if(OBJ_b_TestIdentityFlag(_pst_Child, OBJ_C_IdentityFlag_FlashMatrix)) ANI_RemoveHierarchyOnFlash(_pst_Child);

		_pst_Child->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Hierarchy;

		MEM_Free(_pst_Child->pst_Base->pst_Hierarchy);
		_pst_Child->pst_Base->pst_Hierarchy = NULL;
	}

	if(COL_b_HierarchyLinkCreationTest(_pst_Child, _pst_Father))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix st_InvMatrix	ONLY_PSX2_ALIGNED(16);
		MATH_tdst_Matrix st_Temp		ONLY_PSX2_ALIGNED(16);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Hierarchy Loop avoiding Test. */
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			OBJ_tdst_GameObject *pst_Father;
			UCHAR				uc_Loop, uc_MaxLoop;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			uc_MaxLoop = 10;
			uc_Loop = 0;
			pst_Father = _pst_Father;
			while(OBJ_pst_GetFather(pst_Father) && (uc_Loop < uc_MaxLoop))
			{
				if(OBJ_pst_GetFather(pst_Father) == _pst_Child) return;

				pst_Father = OBJ_pst_GetFather(pst_Father);
			}
		}

		OBJ_ChangeIdentityFlags
		(
			_pst_Child,
			OBJ_ul_FlagsIdentityGet(_pst_Child) | OBJ_C_IdentityFlag_Hierarchy,
			OBJ_ul_FlagsIdentityGet(_pst_Child)
		);

		if(OBJ_b_TestIdentityFlag(_pst_Father, OBJ_C_IdentityFlag_ZDM + OBJ_C_IdentityFlag_ZDE))
		{
			if(_pst_Father->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	st_Matrix;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_MulMatrixMatrix
				(
					&st_Matrix,
					((COL_tdst_Base *) _pst_Father->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					_pst_Father->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
				);
				MATH_InvertMatrix(&st_InvMatrix, &st_Matrix);
			}
			else
			{
				MATH_InvertMatrix
				(
					&st_InvMatrix,
//					((COL_tdst_Base *) _pst_Father->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix
					_pst_Father->pst_GlobalMatrix
				);
			}
		}
		else
		{
			MATH_InvertMatrix(&st_InvMatrix, _pst_Father->pst_GlobalMatrix);
		}

		/* Computes Local Matrix. */
		MATH_MulMatrixMatrix
		(
			&_pst_Child->pst_Base->pst_Hierarchy->st_LocalMatrix,
			_pst_Child->pst_GlobalMatrix,
			&st_InvMatrix
		);

		/*
		 * Old Global Matrix, which in in Global Coordinate System need to be recomputed
		 * in Local of the Father.
		 */
        {
            COL_tdst_Instance *pst_Instance = ((COL_tdst_Base *) _pst_Child->pst_Extended->pst_Col)->pst_Instance;

            // Antibug (for when instance has been destroyed last frame)
            if (pst_Instance)
            {
        		MATH_MulMatrixMatrix(&st_Temp,pst_Instance->pst_OldGlobalMatrix,&st_InvMatrix);
        		MATH_CopyMatrix(pst_Instance->pst_OldGlobalMatrix,&st_Temp);
            }
        }
		_pst_Child->pst_Base->pst_Hierarchy->pst_FatherInit = NULL;
		_pst_Child->pst_Base->pst_Hierarchy->pst_Father = _pst_Father;

		if(OBJ_b_TestIdentityFlag(_pst_Child, OBJ_C_IdentityFlag_FlashMatrix)) ANI_ApplyHierarchyOnFlash(_pst_Child);
	}
}

/*$F
 =======================================================================================================================

									SLOW ACTOR VS FAST ACTOR

 =======================================================================================================================
 */

BOOL COL_b_SlowActorVsFastActor(COL_tdst_Report *_pst_Report, COL_tdst_GlobalVars *_pst_GlobalVars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	f_FakeHorizSpeedA, f_FakeHorizSpeedB;
	BOOL	b_ColPriority;
	BOOL	b_DynRecCol;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Report) return FALSE;

	/* If A has the Extra Flag SlowFastOff, return */
	if
	(
		_pst_Report->pst_A->pst_Extended
	&&	(_pst_Report->pst_A->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_SlowFastOff)
	) return FALSE;

	b_ColPriority = FALSE;

	if
	(
		_pst_Report->pst_A->pst_Extended
	&&	(_pst_Report->pst_A->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_COLPriority)
	) b_ColPriority = TRUE;

	b_DynRecCol =
		(
			(_pst_Report->pst_A->ul_IdentityFlags & (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
		&&	(_pst_Report->pst_B->ul_IdentityFlags & (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
		&&	_pst_Report->pst_A->pst_Base
		&&	_pst_Report->pst_A->pst_Base->pst_Dyna
		&&	(_pst_Report->pst_A->pst_Base->pst_Dyna->ul_DynFlags & DYN_C_ApplyRec)
		&&	_pst_Report->pst_B->pst_Base
		&&	_pst_Report->pst_B->pst_Base->pst_Dyna
		&&	(_pst_Report->pst_B->pst_Base->pst_Dyna->ul_DynFlags & DYN_C_ApplyRec)
		&&	(
				((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance->c_Priority ==
					((COL_tdst_Base *) _pst_Report->pst_B->pst_Extended->pst_Col)->pst_Instance->c_Priority
			)
		);

	if(b_DynRecCol)
	{
		if
		(
			(_pst_Report->ul_Flag & COL_Cul_Wall)
		&&	(
				!(((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance->uw_Flags & COL_Cul_Recompute_SlowFast
				)
			)
		) b_ColPriority = TRUE;
		else
			return FALSE;
	}

	/* Classical Priority Management */
	if
	(
		!b_ColPriority
	&&	_pst_Report->pst_A
	&&	_pst_Report->pst_B
	&&	_pst_Report->pst_B->pst_Extended
	&&	_pst_Report->pst_B->pst_Extended->pst_Col
	&&	((COL_tdst_Base *) _pst_Report->pst_B->pst_Extended->pst_Col)->pst_Instance
	&&	_pst_Report->pst_A->pst_Extended
	&&	_pst_Report->pst_A->pst_Extended->pst_Col
	&&	((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance
	)
	{
		if
		(
			((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance->c_Priority >
				((COL_tdst_Base *) _pst_Report->pst_B->pst_Extended->pst_Col)->pst_Instance->c_Priority
		) b_ColPriority = TRUE;
		else
		{
			if
			(
				((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_Instance->c_Priority <
					((COL_tdst_Base *) _pst_Report->pst_B->pst_Extended->pst_Col)->pst_Instance->c_Priority
			) return FALSE;
		}
	}

	if(!b_ColPriority)
	{
		f_FakeHorizSpeedA = (float) (fAbs((float) _pst_Report->pst_A->pst_Base->pst_Dyna->st_P.x) + fAbs((float) _pst_Report->pst_A->pst_Base->pst_Dyna->st_P.y));
		f_FakeHorizSpeedB = (float) (fAbs((float) _pst_Report->pst_B->pst_Base->pst_Dyna->st_P.x) + fAbs((float) _pst_Report->pst_B->pst_Base->pst_Dyna->st_P.y));
	}

	if
	(
		(b_ColPriority || (f_FakeHorizSpeedA < f_FakeHorizSpeedB))
	&&	(_pst_GlobalVars->ul_ReportIndex < COL_Cul_MaxNbOfCollision)
	)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_NegRecal;
		MATH_tdst_Vector	st_NegNormal;
		WOR_tdst_World		*pst_World;
		ULONG				ul_ReportIndex;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_NegVector(&st_NegRecal, &_pst_Report->st_Recal);
		MATH_NegVector(&st_NegNormal, &_pst_Report->st_Normal);

		_pst_GlobalVars->pst_A = _pst_Report->pst_B;
		_pst_GlobalVars->pst_B = _pst_Report->pst_A;

		/* Add a new Report */
		ul_ReportIndex = _pst_GlobalVars->ul_ReportIndex;
		pst_World = _pst_GlobalVars->pst_World;
		pst_World->ast_Reports[ul_ReportIndex].st_CollidedPoint = _pst_Report->st_CollidedPoint;
		pst_World->ast_Reports[ul_ReportIndex].st_Recal = st_NegRecal;
		pst_World->ast_Reports[ul_ReportIndex].st_Normal = st_NegNormal;

		pst_World->ast_Reports[ul_ReportIndex].pst_A = _pst_Report->pst_B;
		pst_World->ast_Reports[ul_ReportIndex].pst_B = _pst_Report->pst_A;
		pst_World->ast_Reports[ul_ReportIndex].ul_Flag = _pst_Report->ul_Flag;

		pst_World->ast_Reports[ul_ReportIndex].ul_Triangle = _pst_Report->ul_Triangle;
		pst_World->ast_Reports[ul_ReportIndex].uw_Element = _pst_Report->uw_Element;
		if
		(
			_pst_Report->pst_A
		&&	_pst_Report->pst_A->pst_Extended
		&&	_pst_Report->pst_A->pst_Extended->pst_Col
		&&	((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_ColMap
		&&	((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]
		&&	((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->pst_GMatList
		&&	(((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles)
		)
		{
			pst_World->ast_Reports[ul_ReportIndex].pst_GM = COL_pst_GMat_Get
				(
					((COL_tdst_Base *) _pst_Report->pst_A->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0],
					NULL
				);
		}
		else
			pst_World->ast_Reports[ul_ReportIndex].pst_GM = NULL;

		_pst_GlobalVars->ul_ReportIndex++;
		pst_World->ul_NbReports = _pst_GlobalVars->ul_ReportIndex;

		COL_Report_SetFlag(_pst_Report, COL_Cul_Inactive);

		/*
		 * We found A Vs B and have created B Vs A because of speed or priority. But,
		 * there can already be a B Vs A report that we have to deal with to avoid double
		 * recalages.
		 */
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(_pst_GlobalVars->b_Recomputing)
			{
				pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->ul_FirstRecomputingReport;
				pst_LastReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->ul_ReportIndex - 1;
			}
			else
			{
				pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports;
				pst_LastReport = pst_CurrentReport + _pst_GlobalVars->ul_ReportIndex - 1;
			}

			for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
			{
				if
				(
					COL_b_Report_TestFlag
						(
							pst_CurrentReport,
							COL_Cul_Invalid | COL_Cul_Inactive | COL_Cul_Crossable | COL_Cul_Extra_Mask
						)
				) continue;

				if((pst_CurrentReport->pst_A == _pst_Report->pst_B) && (pst_CurrentReport->pst_B == _pst_Report->pst_A))
				{
					if(MATH_f_SqrNormVector(&pst_CurrentReport->st_Recal) > MATH_f_SqrNormVector(&_pst_Report->st_Recal))
					{
						COL_Report_SetFlag
						(
							_pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->ul_ReportIndex - 1,
							COL_Cul_Inactive
						);
					}
					else
						COL_Report_SetFlag(pst_CurrentReport, COL_Cul_Inactive);

					return TRUE;
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

/*$F
 =======================================================================================================================
 Aim:	Suspect Collision Case. We prefer call a Collision Recomputation for this Actor.

 Note:	Suspect cases are:
							- Dynamic Collision
							- Only Edge Ground Collision
							- Wall Collision.
 =======================================================================================================================
 */

BOOL COL_NeedRecomputation(COL_tdst_GlobalVars *_pst_GlobalVars, OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_AlreadyRecompute;
	COL_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Instance = COL_pst_GetInstance(_pst_GO, FALSE);

	/* the GO must have a detection List to be recomputed */
	if(!(_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Detection)) return 1;

	if(_pst_GlobalVars->uc_RecomputeGO < COL_Cuc_MaxRecomputeGO)
	{
		/*~~*/
		int i;
		/*~~*/

		b_AlreadyRecompute = FALSE;
		for(i = 0; i < _pst_GlobalVars->uc_RecomputeGO; i++)
		{
			if(_pst_GlobalVars->apst_RecomputeGO[i] == _pst_GO)
			{
				b_AlreadyRecompute = TRUE;
				COL_Instance_SetFlag(COL_pst_GetInstance(_pst_GO, TRUE), (USHORT) _ul_Flag);
				break;
			}
		}

		if(!b_AlreadyRecompute)
		{
			_pst_GlobalVars->apst_RecomputeGO[_pst_GlobalVars->uc_RecomputeGO++] = _pst_GO;
			COL_Instance_SetFlag(COL_pst_GetInstance(_pst_GO, TRUE), (USHORT) _ul_Flag);
		}
	}

#ifdef ACTIVE_EDITORS
	else
	{
		LINK_PrintStatusMsg("[Collision Warning] Collision Max Recomputable GameObjects reached");
	}
#endif
	return(!b_AlreadyRecompute);
}

/*$F
 =======================================================================================================================
    Aim:    This function finds all the collisions reported between the same 2 objects in order to find a correct and
            valid recalage for this particular collision.
 =======================================================================================================================
 */

void COL_PreComputeReports(COL_tdst_GlobalVars *_pst_GlobalVars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report		*pst_FirstReport, *pst_CurrentReport, *pst_Report, *pst_LastReport, st_TempReport;
	OBJ_tdst_GameObject *pst_A_GO, *pst_B_GO, *pst_GroundGO;
	BOOL				b_SlipperyEdge, b_Dynamic;
	float				f_MinWallRecal, f_MaxWallRecal, f_WallRecal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GlobalVars->b_Recomputing)
	{
		pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->ul_FirstRecomputingReport;
		pst_LastReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->ul_ReportIndex;
	}
	else
	{
		pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports;
		pst_LastReport = pst_CurrentReport + _pst_GlobalVars->ul_ReportIndex;
	}

	pst_FirstReport = pst_CurrentReport;

	/*$F----------------------------------------------------------------------------------------------------

											 GROUND Reports												

		-------------------------------------------------------------------------------------------------------*/
	for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
	{
		pst_GroundGO = NULL;

		if
		(
			COL_b_Report_TestFlag
				(
					pst_CurrentReport,
					COL_Cul_Wall | COL_Cul_Invalid | COL_Cul_Inactive | COL_Cul_Crossable | COL_Cul_Extra_Mask
				)
		) continue;

		pst_A_GO = pst_CurrentReport->pst_A;
		pst_GroundGO = pst_CurrentReport->pst_B;

		if(COL_Instance_TestFlag(COL_pst_GetInstance(pst_A_GO, TRUE), COL_Cul_OnlyGravity)) continue;

#ifdef ODE_INSIDE
		/* Dont want to precompute reports that were created only for ODE */
		if((pst_A_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && !(pst_A_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)) continue;
#endif


		/*$F
		 * --------------------------
		 * Fast Actor Vs Slow Actor.
		 * --------------------------
		 * Example: Sally has no ColMap and doesnt move. The companion has one and mo)ves.
		 * We dont want Sally to be recaled, so, we create a new Collision report to recal
		 * the Companion out of Sally. By default, the priority is given to the slowest actor.
		 */
		if
		(
			pst_GroundGO
		&&	COL_b_ActorVsActor(pst_A_GO, pst_GroundGO)
		&&	(COL_b_SlowActorVsFastActor(pst_CurrentReport, _pst_GlobalVars))
		)
		{
			/* Force recomputation of B. */
			COL_NeedRecomputation(_pst_GlobalVars, pst_GroundGO, COL_Cul_Recompute_SlowFast);

			pst_LastReport++;
			continue;
		}

		/* ONLY FOR CAMERA: We force the recomputation even if we have only one ground */
		if(pst_A_GO == WOR_gpst_CurrentWorld->pst_View->pst_Father)
		{
			((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags |= COL_Cul_ForceRecomputeWhenOnlyGround;
		}

		/*
		 * When Ground AND Wall reports, we ONLY use the Ground information to recal and
		 * we force the recomputation. All the Wall Reports are set Inactive for this
		 * Object. This, to avoid Wall edges detection under the ground for example.
		 */
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_Report *pst_TempReport;
			BOOL			b_NeedRecompute;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_TempReport = pst_FirstReport;

			b_NeedRecompute = FALSE;
			for(; pst_TempReport < pst_LastReport; pst_TempReport++)
			{
				if
				(
					(pst_TempReport != pst_CurrentReport)
				&&	(pst_TempReport->pst_A == pst_A_GO)
				&&	COL_b_Report_TestFlag(pst_TempReport, COL_Cul_Wall)
				&&	!COL_b_Report_TestFlag(pst_TempReport, COL_Cul_Extra_Mask)

				)
				{
					COL_Report_SetFlag(pst_TempReport, COL_Cul_Inactive);
					b_NeedRecompute = TRUE;
				}
			}

			if(b_NeedRecompute)
			{
				COL_NeedRecomputation(_pst_GlobalVars, pst_A_GO, COL_Cul_Recompute_GroundAndWall);
			}
		}

		b_Dynamic = !(pst_CurrentReport->ul_Flag & COL_Cul_Static);

		for(pst_Report = pst_CurrentReport + 1; pst_Report < pst_LastReport; pst_Report++)
		{
			if(pst_Report->pst_A == pst_A_GO)
			{
				if(COL_b_Report_TestFlag(pst_Report, COL_Cul_Wall | COL_Cul_Inactive | COL_Cul_Crossable | COL_Cul_Extra_Mask)) continue;

#ifdef ODE_INSIDE
				/* Dont want to precompute reports that were created only for ODE */
				if((pst_Report->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && !(pst_Report->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna))continue;
#endif




				/*$F	---------------------------------
						BUG DU COUDECUL DE PEY'J SUR ILOT
						---------------------------------

						On pourrait penser que si le main report concernant le sol n'est pas dynamique, on peut se passer de
						forcer la recomputation. Malheureusement, les recalages sur collision dynamiques ne sont pas "parfaits".
						La logique des collisions dynamiques est de placer l'objet "dans le decor" et de reappeler ensuite les
						collision statiques pour le replacer parfaitement en dehors du decor.
						On peut donc se retrouver avec une collision statique comme etant (apparemment)le plus gros recalage
						alors qu'au final c'est la collision dynamique qui doit l'etre.
						Tout cela vient du caractere imparfait des recalages dynamiques.
				*/
				if(!b_Dynamic) b_Dynamic = !(pst_Report->ul_Flag & COL_Cul_Static);

				/*
				 * We have found a Ground report that has a bigger vertical recalage. We use it
				 * instead of the previous one.
				 */
				if(fAbs(pst_Report->st_Recal.z) > fAbs(pst_CurrentReport->st_Recal.z))
				{
					st_TempReport = *pst_CurrentReport;
					*pst_CurrentReport = *pst_Report;
					*pst_Report = st_TempReport;

					pst_GroundGO = pst_Report->pst_B;
				}

				COL_Report_SetFlag(pst_Report, COL_Cul_Inactive);
			}
		}

		COL_UpdateDynamicHierarchyLink(pst_A_GO, pst_GroundGO);

		if(COL_Instance_TestFlag(COL_pst_GetInstance(pst_A_GO, TRUE), COL_Cul_ForceRecomputeWhenOnlyGround))
		{
			COL_NeedRecomputation(_pst_GlobalVars, pst_A_GO, COL_Cul_Recompute_Force);
		}
		else
		{
			if(b_Dynamic)
			{
				COL_NeedRecomputation(_pst_GlobalVars, pst_A_GO, COL_Cul_Recompute_Dynamic);
			}
		}
	}

	/*$F----------------------------------------------------------------------------------------------------

											 WALL Reports												

	-------------------------------------------------------------------------------------------------------*/
	pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports;
	if(_pst_GlobalVars->b_Recomputing) pst_CurrentReport += _pst_GlobalVars->ul_FirstRecomputingReport;

	for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
	{
		if
		(
			COL_b_Report_TestFlag
				(
					pst_CurrentReport,
					COL_Cul_Ground | COL_Cul_Inactive | COL_Cul_Invalid | COL_Cul_Crossable | COL_Cul_Extra_Mask
				)
		) continue;

		pst_A_GO = pst_CurrentReport->pst_A;
		pst_B_GO = pst_CurrentReport->pst_B;

		if(COL_Instance_TestFlag(COL_pst_GetInstance(pst_A_GO, TRUE), COL_Cul_OnlyGravity)) continue;

#ifdef ODE_INSIDE
		if((pst_A_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && !(pst_A_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)) continue;
#endif



		if(pst_A_GO->ul_IdentityFlags & (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
		{
			((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags |= COL_Cul_ForceRecomputeWhenOnlyGround;
		}

		/*$F
		 * --------------------------
		 * Fast Actor Vs Slow Actor.
		 * --------------------------
		 * Example: Sally has no ColMap and doesnt move. The companion has one and moves.
		 * We dont want Sally to be recaled, so, we create a new Collision report to recal
		 * the Companion out of Sally. By default, the priority is given to the slowest actor.
		 */
		if
		(
			pst_B_GO
		&&	COL_b_ActorVsActor(pst_A_GO, pst_B_GO)
		&&	(COL_b_SlowActorVsFastActor(pst_CurrentReport, _pst_GlobalVars))
		)
		{
			/* Force recomputation of B. */
			COL_NeedRecomputation(_pst_GlobalVars, pst_B_GO, COL_Cul_Recompute_SlowFast);

			pst_LastReport++;
			continue;
		}

		/*$F----------------------------------------------------------------------------------------------------
	
												 GAMEOBJECT CORNER COMPUTATION

		-------------------------------------------------------------------------------------------------------*/
		if
		(
			COL_pst_GetInstance(pst_A_GO, TRUE)
		&&	!(COL_Instance_TestFlag(COL_pst_GetInstance(pst_A_GO, FALSE), COL_Cul_CornerComputed))
		)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_CornerNormal;
			UCHAR				uc_WallCol;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/* Slippery Edges reports cannot be used for corner normal computation. */
			if(pst_CurrentReport->ul_Flag & COL_Cul_SlipperyEdge)
			{
				MATH_InitVector(&st_CornerNormal, 0.0f, 0.0f, 0.0f);
				uc_WallCol = 0;
			}
			else
			{
				MATH_CopyVector(&st_CornerNormal, &pst_CurrentReport->st_Normal);
				uc_WallCol = 1;
			}

			for(pst_Report = _pst_GlobalVars->pst_World->ast_Reports; pst_Report < pst_LastReport; pst_Report++)
			{
				if(pst_Report->pst_A != pst_A_GO) continue;
				if(pst_Report == pst_CurrentReport) continue;

				if
				(
					COL_b_Report_TestFlag
						(
							pst_Report,
							COL_Cul_Ground | COL_Cul_Crossable | COL_Cul_Inactive | COL_Cul_Invalid | COL_Cul_SlipperyEdge
						)
				) continue;

				if(MATH_f_DotProduct(&st_CornerNormal, &pst_Report->st_Normal) < _pst_GlobalVars->f_CornerCosAngle)
				{
					uc_WallCol++;

					MATH_AddEqualVector(&st_CornerNormal, &pst_Report->st_Normal);

					if(!MATH_b_NulVector(&st_CornerNormal)) MATH_NormalizeEqualVector(&st_CornerNormal);

//					COL_Report_SetFlag(pst_Report, COL_Cul_Inactive);
				}
			}

			/* Found a corner ? */
			if
			(
				(uc_WallCol >= 2)
			&&	!(MATH_b_NulVector(&st_CornerNormal))
			&&	(_pst_GlobalVars->ul_ReportIndex < COL_Cul_MaxNbOfCollision)
			)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				WOR_tdst_World	*pst_World;
				ULONG			ul_ReportIndex;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				ul_ReportIndex = _pst_GlobalVars->ul_ReportIndex;
				pst_World = _pst_GlobalVars->pst_World;
				pst_World->ast_Reports[ul_ReportIndex].st_CollidedPoint = MATH_gst_NulVector;
				pst_World->ast_Reports[ul_ReportIndex].st_Recal = MATH_gst_NulVector;

				st_CornerNormal.z = 0.0f;
				MATH_NormalizeEqualVector(&st_CornerNormal);
				pst_World->ast_Reports[ul_ReportIndex].st_Normal = st_CornerNormal;

				pst_World->ast_Reports[ul_ReportIndex].pst_A = pst_A_GO;
				pst_World->ast_Reports[ul_ReportIndex].pst_B = pst_CurrentReport->pst_B;
				pst_World->ast_Reports[ul_ReportIndex].ul_Flag = COL_Cul_Wall |
					COL_Cul_Triangle |
					COL_Cul_Static |
					COL_Cul_Extra_Corner;
				pst_World->ast_Reports[ul_ReportIndex].ul_Triangle = 0;
				pst_World->ast_Reports[ul_ReportIndex].uw_Element = 0;
				pst_World->ast_Reports[ul_ReportIndex].pst_GM = NULL;
				((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags |= COL_Cul_CornerFound;

				_pst_GlobalVars->ul_ReportIndex++;
				pst_LastReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->ul_ReportIndex;
				pst_World->ul_NbReports = _pst_GlobalVars->ul_ReportIndex;


				/* We compute corners only once per frame for one GO. Set Flag to indicate it */
				((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags |= COL_Cul_CornerComputed;

//				MATH_CopyVector( &((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance->st_LastWall_Normal, &st_CornerNormal);

//				COL_Report_SetFlag(pst_CurrentReport, COL_Cul_Inactive);
//				COL_NeedRecomputation(_pst_GlobalVars, pst_A_GO, COL_Cul_Recompute_Wall);

//				continue;
			}
		} 

		/*$F----------------------------------------------------------------------------------------------------
	
												 END CORNER COMPUTATION

		-------------------------------------------------------------------------------------------------------*/
		if(COL_b_Report_TestFlag(pst_CurrentReport, COL_Cul_Extra_SlipperyEdge)) continue;

		b_Dynamic = !(pst_CurrentReport->ul_Flag & COL_Cul_Static);
		b_SlipperyEdge = (pst_CurrentReport->ul_Flag & COL_Cul_SlipperyEdge);

		f_MinWallRecal = MATH_f_SqrNormVector(&pst_CurrentReport->st_Recal);
		f_MaxWallRecal = f_MinWallRecal;

		MATH_CopyVector( &((COL_tdst_Base *)pst_CurrentReport->pst_A->pst_Extended->pst_Col)->pst_Instance->st_LastWall_Normal,  &pst_CurrentReport->st_Normal);
		COL_Instance_SetFlag(_pst_GlobalVars->pst_A_Instance, COL_Cul_WallDetected);

		for(pst_Report = pst_CurrentReport + 1; pst_Report < pst_LastReport; pst_Report++)
		{
			if(pst_Report->pst_A != pst_A_GO) continue;

			if
			(
				COL_b_Report_TestFlag
					(
						pst_Report,
						COL_Cul_Ground | COL_Cul_Invalid | COL_Cul_Inactive | COL_Cul_Crossable | COL_Cul_Extra_Mask
					)
			) continue;

#ifdef ODE_INSIDE
			if((pst_Report->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && !(pst_Report->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)) continue;
#endif



			f_WallRecal = MATH_f_SqrNormVector(&pst_Report->st_Recal);

			/*
			 * The Main Wall Collision is a Dynamic one. Check if the current one is also
			 * dynamic and has a bigger recalage. If not, we inactivate it.
			 */
			if(b_Dynamic)
			{
				if(!(pst_Report->ul_Flag & COL_Cul_Static) && (f_WallRecal > f_MaxWallRecal))
				{
					f_MaxWallRecal = f_WallRecal;

					st_TempReport = *pst_CurrentReport;
					*pst_CurrentReport = *pst_Report;
					*pst_Report = st_TempReport;
					MATH_CopyVector( &((COL_tdst_Base *)pst_CurrentReport->pst_A->pst_Extended->pst_Col)->pst_Instance->st_LastWall_Normal,  &pst_CurrentReport->st_Normal);
					COL_Instance_SetFlag(_pst_GlobalVars->pst_A_Instance, COL_Cul_WallDetected);
				}

				COL_Report_SetFlag(pst_Report, COL_Cul_Inactive);
				continue;
			}

			if(b_SlipperyEdge)
			{
				if((pst_Report->ul_Flag & COL_Cul_SlipperyEdge) && (f_WallRecal > f_MaxWallRecal))
				{
					f_MaxWallRecal = f_WallRecal;

					st_TempReport = *pst_CurrentReport;
					*pst_CurrentReport = *pst_Report;
					*pst_Report = st_TempReport;

					MATH_CopyVector( &((COL_tdst_Base *)pst_CurrentReport->pst_A->pst_Extended->pst_Col)->pst_Instance->st_LastWall_Normal,  &pst_CurrentReport->st_Normal);
					COL_Instance_SetFlag(_pst_GlobalVars->pst_A_Instance, COL_Cul_WallDetected);
				}

				COL_Report_SetFlag(pst_Report, COL_Cul_Inactive);
				continue;
			}

			/*
			 * We havent found yet a Dynamic Collision and the current one is Dynamic. We
			 * inactivate the previous valid Wall report and sets this new one as the new Main
			 * Wall collision report.
			 */
			if(!(pst_Report->ul_Flag & COL_Cul_Static))
			{
				f_MaxWallRecal = f_WallRecal;

				st_TempReport = *pst_CurrentReport;
				*pst_CurrentReport = *pst_Report;
				*pst_Report = st_TempReport;

				COL_Report_SetFlag(pst_Report, COL_Cul_Inactive);
				b_Dynamic = TRUE;

				MATH_CopyVector( &((COL_tdst_Base *)pst_CurrentReport->pst_A->pst_Extended->pst_Col)->pst_Instance->st_LastWall_Normal,  &pst_CurrentReport->st_Normal);
				COL_Instance_SetFlag(_pst_GlobalVars->pst_A_Instance, COL_Cul_WallDetected);
				continue;
			}

			if(pst_Report->ul_Flag & COL_Cul_SlipperyEdge)
			{
				f_MaxWallRecal = f_WallRecal;

				st_TempReport = *pst_CurrentReport;
				*pst_CurrentReport = *pst_Report;
				*pst_Report = st_TempReport;

				COL_Report_SetFlag(pst_Report, COL_Cul_Inactive);
				b_SlipperyEdge = TRUE;
				continue;
			}

			if(f_WallRecal > f_MinWallRecal)
			{
				f_MinWallRecal = f_WallRecal;
				st_TempReport = *pst_CurrentReport;
				*pst_CurrentReport = *pst_Report;
				*pst_Report = st_TempReport;

				MATH_CopyVector( &((COL_tdst_Base *)pst_CurrentReport->pst_A->pst_Extended->pst_Col)->pst_Instance->st_LastWall_Normal,  &pst_CurrentReport->st_Normal);
				COL_Instance_SetFlag(_pst_GlobalVars->pst_A_Instance, COL_Cul_WallDetected);
			}

			COL_Report_SetFlag(pst_Report, COL_Cul_Inactive);
		}

		/* We have found a Wall Collision. We force the Recomputation for this GO. */
		COL_NeedRecomputation(_pst_GlobalVars, pst_A_GO, COL_Cul_Recompute_Wall);
	}
}

#ifdef ODE_INSIDE
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_AddContactJoint(COL_tdst_Report *pst_CurrentReport)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct dContact *pcontact;
	dJointID	contact_id;
	WOR_tdst_World	*pst_World;
	DYN_tdst_ODE	*pst_ODE_B;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_CurrentReport->pst_A || !pst_CurrentReport->pst_B)
		return;

	pst_World = WOR_World_GetWorldOfObject(pst_CurrentReport->pst_A);

	if(pst_World->ode_contacts_num_jade >= COL_Cul_MaxNbOfCollision)
	{
		ERR_X_Warning(0, "Max contacts reached for ODE", NULL);
		pst_World->ode_contacts_num_jade = 0;
	}


	if
	(
		!(pst_CurrentReport->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
	&&  !(pst_CurrentReport->pst_B->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
	)
		return;

	pst_ODE_B = (pst_CurrentReport->pst_B->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) ? pst_CurrentReport->pst_B->pst_Base->pst_ODE : pst_CurrentReport->pst_A->pst_Base->pst_ODE;


	if
	(
		(pst_CurrentReport->st_Recal.x == 0.0f)
	&&	(pst_CurrentReport->st_Recal.y == 0.0f)
	&&	(pst_CurrentReport->st_Recal.z == 0.0f)
	) 
		return;
	
	pcontact = &(pst_World->ode_contacts_jade[pst_World->ode_contacts_num_jade]);

	pcontact->surface.mode = pst_ODE_B->SurfaceMode;
	pcontact->surface.mu =  pst_ODE_B->mu;
	pcontact->surface.mu2 = pst_ODE_B->mu2;
	pcontact->surface.bounce = pst_ODE_B->bounce;
	pcontact->surface.bounce_vel = pst_ODE_B->bounce_vel;
	pcontact->surface.soft_erp = pst_ODE_B->soft_erp;
	pcontact->surface.soft_cfm = pst_ODE_B->soft_cfm;
	pcontact->surface.motion1 = pst_ODE_B->motion1;
	pcontact->surface.motion2 = pst_ODE_B->motion2;
	pcontact->surface.slip1 = pst_ODE_B->slip1;
	pcontact->surface.slip2 = pst_ODE_B->slip2;


	pcontact->geom.g1 = (pst_CurrentReport->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) ? pst_CurrentReport->pst_A->pst_Base->pst_ODE->ode_id_geom : 0;
	pcontact->geom.g2 = (pst_CurrentReport->pst_B->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) ? pst_CurrentReport->pst_B->pst_Base->pst_ODE->ode_id_geom : 0;

	pcontact->geom.normal[0] = pst_CurrentReport->st_Normal.x;
	pcontact->geom.normal[1] = pst_CurrentReport->st_Normal.y;
	pcontact->geom.normal[2] = pst_CurrentReport->st_Normal.z;

	pcontact->geom.depth = MATH_f_NormVector(&pst_CurrentReport->st_Recal);

	pcontact->geom.pos[0] = pst_CurrentReport->st_CollidedPoint.x;
	pcontact->geom.pos[1] = pst_CurrentReport->st_CollidedPoint.y;
	pcontact->geom.pos[2] = pst_CurrentReport->st_CollidedPoint.z;

	contact_id = dJointCreateContact (pst_World->ode_id_world, pst_World->ode_joint_col_jade, pcontact);
	dJointAttach(contact_id, (pst_CurrentReport->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) ? pst_CurrentReport->pst_A->pst_Base->pst_ODE->ode_id_body : 0, (pst_CurrentReport->pst_B->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) ? pst_CurrentReport->pst_B->pst_Base->pst_ODE->ode_id_body : 0);
	pst_World->ode_contacts_num_jade ++;
}

#endif
/*
 =======================================================================================================================
    Aim: Deals with all the reports and really recal the objects.
 =======================================================================================================================
 */
void COL_ComputeReports(COL_tdst_GlobalVars *_pst_GlobalVars, ULONG _ul_FirstReport)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report		*pst_CurrentReport, *pst_LastReport;
	COL_tdst_Instance	*pst_Instance;
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*pst_Pos;
	BOOL				b_Hierarchy, b_GravityOnly;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_GravityOnly = FALSE;
	
	/*
	 * If the last Collision Loop didnt create any new report, we just have to recal
	 * the Objects into the Walls.
	 */
	if(_pst_GlobalVars->ul_ReportIndex == _pst_GlobalVars->ul_FirstRecomputingReport)
	{
#ifdef ACTIVE_EDITORS
		if(COL_s_Log)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[100];;
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Log, "- Loop n°%i - No Collisions", COL_uc_RecomputingLoop);
			LINK_PrintStatusMsg(asz_Log);
		}
#endif
		pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports;
		pst_LastReport = pst_CurrentReport + _pst_GlobalVars->pst_World->ul_NbReports;

		goto AfterRecalage;
	}

	b_Hierarchy = FALSE;
	COL_PreComputeReports(_pst_GlobalVars);

#ifdef ACTIVE_EDITORS
	if(COL_s_Log) COL_Debug_Trace(_pst_GlobalVars, COL_sasz_Debug[COL_s_Debug_Number]);
#endif
	if(_pst_GlobalVars->b_Recomputing)
	{
		pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->ul_FirstRecomputingReport;
		pst_LastReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->pst_World->ul_NbReports;
	}
	else
	{
		pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports + _ul_FirstReport;
		pst_LastReport = pst_CurrentReport + _pst_GlobalVars->pst_World->ul_NbReports;
	}

	/*$F----------------------------------------------------------------------------------------------------

											 NO HIERARCHY REPORTS												

	-------------------------------------------------------------------------------------------------------*/
	for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
	{
		/* We have precomputed the reports and may have invalidated some reports. */
		if
		(
			COL_b_Report_TestFlag
				(
					pst_CurrentReport,
					COL_Cul_Invalid | COL_Cul_Inactive | COL_Cul_Crossable | COL_Cul_Extra_Mask
				)
		) continue;

		pst_GO = pst_CurrentReport->pst_A;
		pst_Instance = COL_pst_GetInstance(pst_GO, FALSE);

		/* Hierarchy GO found. We will handle it in the next loop. */
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		{
			b_Hierarchy = TRUE;
			continue;
		}

		/* The Object is on the Ground and not slippery. Collision Optimization ENABLE. */
		if((pst_CurrentReport->ul_Flag & COL_Cul_Ground) && !(pst_CurrentReport->ul_Flag & COL_Cul_SlipperyEdge) && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Dyna))
		{
			/* Ceiling Case */
			if(!pst_GO->pst_Base->pst_Dyna->pst_Forces || (MATH_f_DotProduct(&pst_CurrentReport->st_Normal, &pst_GO->pst_Base->pst_Dyna->pst_Forces->st_Gravity) < 0.0f))
				pst_GO->pst_Base->pst_Dyna->ul_DynFlags |= DYN_C_OptimizeColEnable;
		}

		if(COL_Instance_TestFlag(pst_Instance, COL_Cul_OnlyGravity)) continue;

		pst_Pos = &pst_GO->pst_GlobalMatrix->T;


#ifndef ODE_INSIDE
		MATH_AddVector(pst_Pos, pst_Pos, &pst_CurrentReport->st_Recal);

		/* Update Flash (and Reference Matrix if Blend) Matrix */
		ANI_UpdateFlashAfterMagicBoxTranslation(pst_GO, &pst_CurrentReport->st_Recal, ANI_C_UpdateAllAnims);

#else


		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			BOOL b_AHasEnabledODE, b_BHasEnabledODE, b_AHasDYN, b_BHasDYN;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			b_AHasEnabledODE = (pst_CurrentReport->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && (pst_CurrentReport->pst_A->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_ENABLE);
			b_AHasDYN = (pst_CurrentReport->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna);

			b_BHasEnabledODE = (pst_CurrentReport->pst_B->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && (pst_CurrentReport->pst_B->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_ENABLE);
			b_BHasDYN = (pst_CurrentReport->pst_B->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna);

			if(!b_AHasEnabledODE || (b_AHasDYN && (b_BHasDYN || !b_BHasEnabledODE || (b_BHasEnabledODE && !(pst_CurrentReport->pst_B->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)))))
			{
				MATH_AddVector(pst_Pos, pst_Pos, &pst_CurrentReport->st_Recal);
				ANI_UpdateFlashAfterMagicBoxTranslation(pst_GO, &pst_CurrentReport->st_Recal, ANI_C_UpdateAllAnims);
			}

			if(b_AHasEnabledODE || b_BHasEnabledODE)
				COL_AddContactJoint(pst_CurrentReport);
		}
#endif

	}

	/*$F----------------------------------------------------------------------------------------------------

											 HIERARCHY REPORTS												

	-------------------------------------------------------------------------------------------------------*/
	if(b_Hierarchy)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix st_InvFatherMatrix ONLY_PSX2_ALIGNED(16);
		MATH_tdst_Vector					st_Recal;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_pst_GlobalVars->b_Recomputing)
		{
			pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports + _pst_GlobalVars->ul_FirstRecomputingReport;
		}
		else
			pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports;

		for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
		{
			if
			(
				COL_b_Report_TestFlag
					(
						pst_CurrentReport,
						COL_Cul_Invalid | COL_Cul_Inactive | COL_Cul_Crossable | COL_Cul_Extra_Mask
					)
			) continue;

			pst_GO = pst_CurrentReport->pst_A;
			pst_Instance = COL_pst_GetInstance(pst_GO, FALSE);

			if
			(
				!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
			||	(COL_Instance_TestFlag(pst_Instance, COL_Cul_OnlyGravity))
			) continue;

			if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy)) continue;
			if(!pst_GO->pst_Base || !pst_GO->pst_Base->pst_Hierarchy  || !pst_GO->pst_Base->pst_Hierarchy->pst_Father) continue;

			/* The Object touches a Ground not slippery. Collision Optimization ENABLE. */
			if((pst_CurrentReport->ul_Flag & COL_Cul_Ground)  && !(pst_CurrentReport->ul_Flag & COL_Cul_SlipperyEdge) && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Dyna))
				pst_GO->pst_Base->pst_Dyna->ul_DynFlags |= DYN_C_OptimizeColEnable;

			pst_Pos = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T;

			MATH_InvertMatrix(&st_InvFatherMatrix, OBJ_pst_GetAbsoluteMatrix(OBJ_pst_GetFather(pst_GO)));
			MATH_TransformVector(&st_Recal, &st_InvFatherMatrix, &pst_CurrentReport->st_Recal);

#ifndef ODE_INSIDE
			MATH_AddEqualVector(pst_Pos, &st_Recal);

			OBJ_ComputeGlobalWhenHie(pst_GO);

			/* Update Flash (and Reference Matrix if Blend) Matrix */
			ANI_UpdateFlashAfterMagicBoxTranslation(pst_GO, &st_Recal, ANI_C_UpdateAllAnims);

#else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			BOOL b_AHasEnabledODE, b_BHasEnabledODE, b_AHasDYN, b_BHasDYN;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			b_AHasEnabledODE = (pst_CurrentReport->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && (pst_CurrentReport->pst_A->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_ENABLE);
			b_AHasDYN = (pst_CurrentReport->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna);

			b_BHasEnabledODE = (pst_CurrentReport->pst_B->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && (pst_CurrentReport->pst_B->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_ENABLE);
			b_BHasDYN = (pst_CurrentReport->pst_B->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna);

			if(!b_AHasEnabledODE || (b_AHasDYN && (b_BHasDYN || !b_BHasEnabledODE || (b_BHasEnabledODE && !(pst_CurrentReport->pst_B->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)))))
			{
				MATH_AddEqualVector(pst_Pos, &st_Recal);
				ANI_UpdateFlashAfterMagicBoxTranslation(pst_GO, &st_Recal, ANI_C_UpdateAllAnims);
			}

			if(b_AHasEnabledODE || b_BHasEnabledODE)
				COL_AddContactJoint(pst_CurrentReport);
		}
#endif



		}
	}

AfterRecalage:
	if(_pst_GlobalVars->uc_RecomputeGO && !COL_gb_LastLoop) return;

	/*$F----------------------------------------------------------------------------------------------------

											 RECALAGE IN THE WALL												

	-------------------------------------------------------------------------------------------------------*/
	pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports + _ul_FirstReport;
	for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_InTheWall;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* We have precomputed the reports and may have invalidated some reports. */
		if
		(
			COL_b_Report_TestFlag
				(
					pst_CurrentReport,
					COL_Cul_Invalid | COL_Cul_Inactive | COL_Cul_Crossable | COL_Cul_Extra_Minor
				)
		) continue;

		pst_GO = pst_CurrentReport->pst_A;
		pst_Instance = COL_pst_GetInstance(pst_GO, FALSE);

		if(COL_Instance_TestFlag(pst_Instance, COL_Cul_OnlyGravity))
		{
			b_GravityOnly = TRUE;
		}

		if(COL_b_Report_TestFlag(pst_CurrentReport, COL_Cul_Ground | COL_Cul_SlipperyEdge | COL_Cul_Dynamic))
			continue;

		/* WE DO NOT RECAL THE CAMERA INTO THE WALL */
		if(pst_GO == WOR_gpst_CurrentWorld->pst_View->pst_Father)
		{
			continue;
		}

		if(!pst_Instance || !pst_Instance->b_InTheWall) continue;

		/*
		 * If the Object has been replaced where he was at the previous frame, we do not
		 * recal it into the Wall.
		 */
		if(COL_Instance_TestFlag(pst_Instance, COL_Cul_OnlyGravity)) continue;

		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
			pst_Pos = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T;
		else
			pst_Pos = &pst_GO->pst_GlobalMatrix->T;

		/* If we have found a Corner for this GO. */
		if(pst_Instance->uw_Flags & COL_Cul_CornerFound)
		{
			if(!(COL_b_Report_TestFlag(pst_CurrentReport, COL_Cul_Extra_Corner))) continue;

			MATH_MulVector(&st_InTheWall, &pst_CurrentReport->st_Normal, (float) -10E-3);

			/*$F HORIZONTAL WALLS */
			st_InTheWall.z = 0.0f;

			MATH_AddVector(pst_Pos, pst_Pos, &st_InTheWall);

			/* Update Flash (and Reference Matrix if Blend) Matrix */
			ANI_UpdateFlashAfterMagicBoxTranslation(pst_GO, &st_InTheWall, ANI_C_UpdateAllAnims);


#ifdef ACTIVE_EDITORS
			if(COL_s_Log && !L_strcmp(COL_sasz_Debug[COL_s_Debug_Number], pst_GO->sz_Name))
			{
				/*~~~~~~~~~~~~~~~~~*/
				char	asz_Log[200];
				/*~~~~~~~~~~~~~~~~~*/

				sprintf
				(
					asz_Log,
					"[-> PUSH IN THE WALL] [%0.6f, %0.6f, %0.6f] (Corner)",
					st_InTheWall.x,
					st_InTheWall.y,
					st_InTheWall.z
				);
				LINK_PrintStatusMsg(asz_Log);
			}
#endif
		}
		else
		{
			/* Dont want to recal using a Slippery Edge Normal. */
			if(pst_CurrentReport->ul_Flag & COL_Cul_SlipperyEdge) continue;

			MATH_MulVector(&st_InTheWall, &pst_CurrentReport->st_Normal, (float) -2E-3);

			/* We do not recal the object in the ground if the Wall is a bit slanting. */

			/*$F HORIZONTAL WALLS */
			st_InTheWall.z = 0.0f;
			MATH_AddVector(pst_Pos, pst_Pos, &st_InTheWall);

			/* Update Flash (and Reference Matrix if Blend) Matrix */
			ANI_UpdateFlashAfterMagicBoxTranslation(pst_GO, &st_InTheWall, ANI_C_UpdateAllAnims);

#ifdef ACTIVE_EDITORS
			if(COL_s_Log && !L_strcmp(COL_sasz_Debug[COL_s_Debug_Number], pst_GO->sz_Name))
			{
				/*~~~~~~~~~~~~~~~~~*/
				char	asz_Log[200];
				/*~~~~~~~~~~~~~~~~~*/

				sprintf
				(
					asz_Log,
					"[-> PUSH IN THE WALL]  [%0.6f, %0.6f, %0.6f] (No Corner)",
					st_InTheWall.x,
					st_InTheWall.y,
					st_InTheWall.z
				);
				LINK_PrintStatusMsg(asz_Log);
			}
#endif
		}
	}

	/*$F----------------------------------------------------------------------------------------------------

											 ONLY GRAVITY												

	-------------------------------------------------------------------------------------------------------*/
	if(b_GravityOnly)
	{
		pst_CurrentReport = _pst_GlobalVars->pst_World->ast_Reports + _ul_FirstReport;
		for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Delta;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/* We have precomputed the reports and may have invalidated some reports. */
			if
			(
				COL_b_Report_TestFlag
					(
						pst_CurrentReport,
						COL_Cul_Invalid | COL_Cul_Inactive | COL_Cul_Crossable | COL_Cul_Extra_Minor
					)
			) continue;

			pst_GO = pst_CurrentReport->pst_A;
			pst_Instance = COL_pst_GetInstance(pst_GO, FALSE);

			if
			(
				!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
			||	!(COL_Instance_TestFlag(pst_Instance, COL_Cul_OnlyGravity))
			)
			{
				continue;
			}

			if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				pst_Pos = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T;
				MATH_SubVector
				(
					&st_Delta,
					&pst_Instance->pst_OldGlobalMatrix->T,
					&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T
				);
			}
			else
			{
				pst_Pos = &pst_GO->pst_GlobalMatrix->T;
				MATH_SubVector(&st_Delta, &pst_Instance->pst_OldGlobalMatrix->T, pst_Pos);
			}

			/* Replace the GameObejct where it was before the Collision Call */
			MATH_AddVector(pst_Pos, pst_Pos, &st_Delta);

			if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				OBJ_ComputeGlobalWhenHie(pst_GO);
			}

			/* Update Flash (and Reference Matrix if Blend) Matrix */
			ANI_UpdateFlashAfterMagicBoxTranslation(pst_GO, &st_Delta, ANI_C_UpdateAllAnims);

			/*
			 * Remove the Flag to avoid many old-frame repositionning (there may be several
			 * reports for this GO)
			 */
			COL_Instance_ResetFlag(pst_Instance, COL_Cul_OnlyGravity);

#ifdef ACTIVE_EDITORS
			if(COL_s_Log && !L_strcmp(COL_sasz_Debug[COL_s_Debug_Number], pst_GO->sz_Name))
			{
				/*~~~~~~~~~~~~~~~~~*/
				char	asz_Log[200];
				/*~~~~~~~~~~~~~~~~~*/

				sprintf
				(
					asz_Log,
					"[-> Only Gravity Optimisation] %s has been replaced where it was before Collision Call.",
					pst_GO->sz_Name
				);
				LINK_PrintStatusMsg(asz_Log);
			}
#endif
		}
	}
}

/*
 =======================================================================================================================
    Aim: Return wether the object collide an object that corresponds to the filter. Note: The filter can be a Wall, the
    Ground ...
 =======================================================================================================================
 */
BOOL COL_Report_CollideType(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_Filter)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
	BOOL			b_SkipInactive;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Filter & COL_Cul_Extra_Corner)
	{
		if
		(
			!_pst_GO
		||	!_pst_GO->pst_Extended
		||	!_pst_GO->pst_Extended->pst_Col
		||	!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance
		) return FALSE;

		return
			(
				COL_Instance_TestFlag
				(
					((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance,
					COL_Cul_CornerFound
				)
			);
	}

	b_SkipInactive = TRUE;

	if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;

	pst_CurrentReport = _pst_World->ast_Reports;
	pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

	for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
	{
		/* If the current report does NOT deal with our GameObject, we continue. */
		if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
		if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;
		if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;

		if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

		/* We found it !!! */
		if((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter) 
		{
			COL_gb_SkipODEReports = TRUE;
			return TRUE;
		}
	}

	COL_gb_SkipODEReports = TRUE;
	/* We didn't find a appropriate collision. */
	return FALSE;
}

/*
 =======================================================================================================================
    Aim: Return wether the object collide an object that corresponds to the filter. Note: The filter can be a Wall, the
    Ground ...
 =======================================================================================================================
 */
BOOL COL_Report_HasGMat(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_Filter)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
	BOOL			b_SkipInactive;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_SkipInactive = TRUE;

	if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;

	if(_ul_Filter & COL_Cul_ReportIndex)
	{
		pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
		ERR_X_Warning
		(
			(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
			"AI Report Index overpass max reports",
			NULL
		);
#endif
		return(pst_CurrentReport->pst_GM ? 1 : 0);
	}
	else
	{
		pst_CurrentReport = _pst_World->ast_Reports;
		pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

		for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
		{
			/* If the current report does NOT deal with our GameObject, we continue. */
			if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
			if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;
			if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
			if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

			/* We found it !!! */
			if((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter)
			{
				COL_gb_SkipODEReports = TRUE;
				return(pst_CurrentReport->pst_GM ? 1 : 0);
			}
		}
	}

	COL_gb_SkipODEReports = TRUE;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *COL_Report_GetBestWallGOInDirection
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	MATH_tdst_Vector	*_pst_Vector,
	float				_f_SkipCosAngle,
	ULONG				*_pul_Index
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report		*pst_CurrentReport, *pst_LastReport;
	OBJ_tdst_GameObject *pst_BestGO;
	MATH_tdst_Vector	st_Vector;
	ULONG				ul_Index;
	float				f_Min, f_Current;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Min = _f_SkipCosAngle ? -_f_SkipCosAngle + 1E-3f : 0.0f;
	ul_Index = 0;
	pst_BestGO = NULL;

	pst_CurrentReport = _pst_World->ast_Reports;
	pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

	if(_pul_Index) *_pul_Index = 0;

	for(; pst_CurrentReport < pst_LastReport; ul_Index++, pst_CurrentReport++)
	{
		/* If the current report does NOT deal with our GameObject, we continue. */
		if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
		if(!(pst_CurrentReport->ul_Flag & COL_Cul_Wall)) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Crossable) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Corner) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_SlipperyEdge) continue;
		if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

		if(pst_CurrentReport->pst_A == _pst_GO)
			MATH_CopyVector(&st_Vector, _pst_Vector);
		else
			MATH_NegVector(&st_Vector, _pst_Vector);

		/* We found it !!! */
		f_Current = MATH_f_DotProduct(&pst_CurrentReport->st_Normal, &st_Vector);

		if((f_Current < f_Min) && (pst_CurrentReport->pst_B))
		{
			f_Min = f_Current;
			*_pul_Index = ul_Index;
			pst_BestGO = (pst_CurrentReport->pst_A == _pst_GO) ? pst_CurrentReport->pst_B : pst_CurrentReport->pst_A;
		}
	}

	COL_gb_SkipODEReports = TRUE;
	return pst_BestGO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *COL_Report_GetBestWallGOInDirectionSpecial
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	MATH_tdst_Vector	*_pst_Vector,
	float				_f_SkipCosAngle,
	ULONG				*_pul_Index
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report		*pst_CurrentReport, *pst_LastReport;
	OBJ_tdst_GameObject *pst_BestGO;
	MATH_tdst_Vector	st_Vector;
	ULONG				ul_Index;
	float				f_Min, f_Current;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Min = _f_SkipCosAngle ? -_f_SkipCosAngle + 1E-3f : 0.0f;
	ul_Index = 0;
	pst_BestGO = NULL;

	pst_CurrentReport = _pst_World->ast_Reports;
	pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

	if(_pul_Index) *_pul_Index = 0;

	for(; pst_CurrentReport < pst_LastReport; ul_Index++, pst_CurrentReport++)
	{
		/* If the current report does NOT deal with our GameObject, we continue. */
		if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
		if(!(pst_CurrentReport->ul_Flag & COL_Cul_Wall)) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Crossable) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Corner) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_SlipperyEdge) continue;
		if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

		if(pst_CurrentReport->pst_A == _pst_GO)
			MATH_CopyVector(&st_Vector, _pst_Vector);
		else
			MATH_NegVector(&st_Vector, _pst_Vector);

		/* We found it !!! */
		f_Current = MATH_f_DotProduct(&pst_CurrentReport->st_Normal, &st_Vector);

		if((f_Current < f_Min) && (pst_CurrentReport->pst_B))
		{
			f_Min = f_Current;
			*_pul_Index = ul_Index;
			pst_BestGO = (pst_CurrentReport->pst_A == _pst_GO) ? pst_CurrentReport->pst_B : pst_CurrentReport->pst_A;
		}
	}

	COL_gb_SkipODEReports = TRUE;
	return pst_BestGO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Vector *COL_Report_GMat_GetCollidedPoint
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	ULONG				_ul_GMat,
	ULONG				*_pul_Index
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report		*pst_CurrentReport, *pst_LastReport;
	OBJ_tdst_GameObject *pst_BestGO;
	ULONG				ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = 0;
	pst_BestGO = NULL;

	pst_CurrentReport = _pst_World->ast_Reports;
	pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

	if(_pul_Index) *_pul_Index = 0;

	for(; pst_CurrentReport < pst_LastReport; ul_Index++, pst_CurrentReport++)
	{
		/* If the current report does NOT deal with our GameObject, we continue. */
		if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Mask) continue;

		/* Game Material Flag Test */
		if(pst_CurrentReport->pst_GM && (pst_CurrentReport->pst_GM->ul_CustomBits & _ul_GMat))
		{
			if(_pul_Index) *_pul_Index = ul_Index;
			return(&pst_CurrentReport->st_CollidedPoint);
		}
	}

	return &MATH_gst_NulVector;
}

/*
 =======================================================================================================================
    Aim: Find the first report normal that corresponds to the given filter. Note: The filter can be a Wall, the Ground
    ...
 =======================================================================================================================
 */
void COL_Report_GetNormal
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	MATH_tdst_Vector	*_pst_Normal,
	ULONG				_ul_Filter
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
	BOOL			b_SkipInactive, b_SkipEdgeWall;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Normal) return;

	MATH_InitVector(_pst_Normal, 0.0f, 0.0f, 0.0f);
	if(_ul_Filter & COL_Cul_ReportIndex)
	{
		pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
		ERR_X_Warning
		(
			(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
			"AI Report Index overpass max reports",
			NULL
		);
#endif
		MATH_CopyVector(_pst_Normal, &(pst_CurrentReport->st_Normal));
		return;
	}
	else
	{
		b_SkipInactive = TRUE;
		b_SkipEdgeWall = FALSE;

		if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;
		if(_ul_Filter & COL_Cul_IgnoreEdgeWall) b_SkipEdgeWall = TRUE;

		_ul_Filter &= ~COL_Cul_IgnoreEdgeWall;

		pst_CurrentReport = _pst_World->ast_Reports;
		pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;
		for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
		{
			if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;

			/* If the current report does NOT deal with our GameObject, we continue. */
			if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
			if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
			if(b_SkipEdgeWall && (pst_CurrentReport->ul_Flag & COL_Cul_SlipperyEdge)) continue;
			if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

			/* We found it !!! */
			if((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter)
			{
				if(pst_CurrentReport->pst_A == _pst_GO)
					MATH_CopyVector(_pst_Normal, &(pst_CurrentReport->st_Normal));
				else
					MATH_NegVector(_pst_Normal, &(pst_CurrentReport->st_Normal));

				COL_gb_SkipODEReports = TRUE;
				return;
			}
		}
	}

	COL_gb_SkipODEReports = TRUE;
	return;
}

/*
 =======================================================================================================================
    Aim: Find the first report normal that corresponds to the given filter. Note: The filter can be a Wall, the Ground
    ...
 =======================================================================================================================
 */
int COL_Report_GetFlags
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	ULONG				_ul_Filter
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
	BOOL			b_SkipInactive, b_SkipEdgeWall;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Filter & COL_Cul_ReportIndex)
	{
		pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
		ERR_X_Warning
		(
			(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
			"AI Report Index overpass max reports",
			NULL
		);
#endif
		return(pst_CurrentReport->ul_Flag);
	}
	else
	{
		b_SkipInactive = TRUE;
		b_SkipEdgeWall = FALSE;

		if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;
		if(_ul_Filter & COL_Cul_IgnoreEdgeWall) b_SkipEdgeWall = TRUE;

		_ul_Filter &= ~COL_Cul_IgnoreEdgeWall;

		pst_CurrentReport = _pst_World->ast_Reports;
		pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;
		for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
		{
			if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;

			/* If the current report does NOT deal with our GameObject, we continue. */
			if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
			if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
			if(b_SkipEdgeWall && (pst_CurrentReport->ul_Flag & COL_Cul_SlipperyEdge)) continue;
			if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

			/* We found it !!! */
			if((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter)
			{
				COL_gb_SkipODEReports = TRUE;
				return (pst_CurrentReport->ul_Flag);
			}
		}
	}

	COL_gb_SkipODEReports = TRUE;
	return 0;
}


/*
 =======================================================================================================================
    Aim: Find the first report Collision Point that corresponds to the given filter. Note: The filter can be a Wall,
    the Ground ...
 =======================================================================================================================
 */
MATH_tdst_Vector *COL_Report_GetCollidedPoint
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	ULONG				_ul_Filter
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
	BOOL			b_SkipInactive, b_SkipEdgeWall;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Filter & COL_Cul_ReportIndex)
	{
		pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
		ERR_X_Warning
		(
			(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
			"AI Report Index overpass max reports",
			NULL
		);
#endif
		return &(pst_CurrentReport->st_CollidedPoint);
	}
	else
	{
		b_SkipInactive = TRUE;
		b_SkipEdgeWall = FALSE;

		if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;
		if(_ul_Filter & COL_Cul_IgnoreEdgeWall) b_SkipEdgeWall = TRUE;

		_ul_Filter &= ~COL_Cul_IgnoreEdgeWall;

		pst_CurrentReport = _pst_World->ast_Reports;
		pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;
		for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
		{
			if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;

			/* If the current report does NOT deal with our GameObject, we continue. */
			if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
			if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
			if(b_SkipEdgeWall && (pst_CurrentReport->ul_Flag & COL_Cul_SlipperyEdge)) continue;
			if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

			/* We found it !!! */
			if
			(
				((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter)
			&&	!(MATH_b_NulVector(&pst_CurrentReport->st_CollidedPoint))
			) 
			{
				COL_gb_SkipODEReports = TRUE;
				return &(pst_CurrentReport->st_CollidedPoint);
			}
		}
	}

	COL_gb_SkipODEReports = TRUE;
	return NULL;
}

/*
 =======================================================================================================================
    Aim: Find the first report Game Object that corresponds to the given filter. Note: The filter can be a Wall, the
    Ground ...
 =======================================================================================================================
 */
OBJ_tdst_GameObject *COL_Report_GetCOBGameObject
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	ULONG				_ul_Filter
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
	BOOL			b_SkipInactive;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Filter & COL_Cul_ReportIndex)
	{
		pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
		ERR_X_Warning
		(
			(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
			"AI Report Index overpass max reports",
			NULL
		);
#endif
		if(pst_CurrentReport->pst_B == _pst_GO)
			return pst_CurrentReport->pst_A;
		else
			return pst_CurrentReport->pst_B;
	}
	else
	{
		b_SkipInactive = TRUE;

		if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;

		pst_CurrentReport = _pst_World->ast_Reports;
		pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

		for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
		{
			if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;
			if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
			if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

			if(pst_CurrentReport->pst_A == _pst_GO)
			{
				if(((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter) && pst_CurrentReport->pst_B)
				{
					COL_gb_SkipODEReports = TRUE;
					return pst_CurrentReport->pst_B;
				}
			}

			if(pst_CurrentReport->pst_B == _pst_GO)
			{
				if(((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter) && pst_CurrentReport->pst_A)
				{
					COL_gb_SkipODEReports = TRUE;
					return pst_CurrentReport->pst_A;
				}
			}
		}
	}

	COL_gb_SkipODEReports = TRUE;
	return NULL;
}

/*
 =======================================================================================================================
    Aim: Returns the first report Game Material Flags that corresponds to the given filter. Note: The filter can be a
    Wall, the Ground ...
 =======================================================================================================================
 */
ULONG COL_GameMaterial_GetFlags(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_Filter)
{
	if(!_pst_World) return 0;
	if(_ul_Filter == COL_Cul_Ray)
	{
		if(_pst_World->st_RayInfo.pst_GMat) return _pst_World->st_RayInfo.pst_GMat->ul_CustomBits;
		return 0;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
		BOOL			b_SkipInactive;
		ULONG			ul_Custom;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_ul_Filter & COL_Cul_ReportIndex)
		{
			pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
			ERR_X_Warning
			(
				(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
				"AI Report Index overpass max reports",
				NULL
			);
#endif
			ul_Custom = 0;
			if(pst_CurrentReport->pst_GM)
				return(pst_CurrentReport->pst_GM->ul_CustomBits | ul_Custom);
			else
				return ul_Custom;
		}
		else
		{
			b_SkipInactive = TRUE;

			if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;

			pst_CurrentReport = _pst_World->ast_Reports;
			pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

			for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
			{
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;

				/* If the current report does NOT deal with our GameObject, we continue. */
				if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Corner) continue;
				if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

				/* We found it !!! */
				if(((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter))
				{
					/*~~~~~~~~~~~~~~*/
					ULONG	ul_Custom;
					/*~~~~~~~~~~~~~~*/

					ul_Custom = 0;
					COL_gb_SkipODEReports = TRUE;
					if(pst_CurrentReport->pst_GM)
						return(pst_CurrentReport->pst_GM->ul_CustomBits | ul_Custom);
					else
						return ul_Custom;
				}
			}
		}

		COL_gb_SkipODEReports = TRUE;
		return 0;
	}
}

/*
 =======================================================================================================================
    Aim: Returns the first report Game Material Slide that corresponds to the given filter. Note: The filter can be a
    Wall, the Ground ...
 =======================================================================================================================
 */
float COL_GameMaterial_GetSlide(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_Filter)
{
	if(!_pst_World) return 0.0f;
	if(_ul_Filter == COL_Cul_Ray)
	{
		if(_pst_World->st_RayInfo.pst_GMat) return _pst_World->st_RayInfo.pst_GMat->f_Slide;
		return 0.0f;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
		BOOL			b_SkipInactive;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_ul_Filter & COL_Cul_ReportIndex)
		{
			pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
			ERR_X_Warning
			(
				(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
				"AI Report Index overpass max reports",
				NULL
			);
#endif
			if(pst_CurrentReport->pst_GM)
				return pst_CurrentReport->pst_GM->f_Slide;
			else
				return 0.0f;
		}
		else
		{
			b_SkipInactive = TRUE;

			if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;

			pst_CurrentReport = _pst_World->ast_Reports;
			pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

			for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
			{
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;

				/* If the current report does NOT deal with our GameObject, we continue. */
				if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Corner) continue;
				if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

				/* We found it !!! */
				if(((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter) && (pst_CurrentReport->pst_GM))
				{
					COL_gb_SkipODEReports = TRUE;
					return pst_CurrentReport->pst_GM->f_Slide;
				}
			}
		}

		COL_gb_SkipODEReports = TRUE;
		return 0.0f;
	}
}

/*
 =======================================================================================================================
    Aim: Returns the first report Game Material Rebound that corresponds to the given filter. Note: The filter can be a
    Wall, the Ground ...
 =======================================================================================================================
 */
float COL_GameMaterial_GetRebound(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_Filter)
{
	if(!_pst_World) return 0.0f;
	if(_ul_Filter == COL_Cul_Ray)
	{
		if(_pst_World->st_RayInfo.pst_GMat) return _pst_World->st_RayInfo.pst_GMat->f_Rebound;
		return 0.0f;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
		BOOL			b_SkipInactive;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_ul_Filter & COL_Cul_ReportIndex)
		{
			pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
			ERR_X_Warning
			(
				(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
				"AI Report Index overpass max reports",
				NULL
			);
#endif
			if(pst_CurrentReport->pst_GM)
				return pst_CurrentReport->pst_GM->f_Rebound;
			else
				return 0.0f;
		}
		else
		{
			b_SkipInactive = TRUE;

			if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;

			pst_CurrentReport = _pst_World->ast_Reports;
			pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

			for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
			{
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;

				/* If the current report does NOT deal with our GameObject, we continue. */
				if((pst_CurrentReport->pst_A != _pst_GO) && (pst_CurrentReport->pst_B != _pst_GO)) continue;
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Corner) continue;
				if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

				/* We found it !!! */
				if(((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter) && (pst_CurrentReport->pst_GM))
				{
					COL_gb_SkipODEReports = TRUE;
					return pst_CurrentReport->pst_GM->f_Rebound;
				}
			}
		}

		COL_gb_SkipODEReports = TRUE;
		return 0.0f;
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG COL_GameMaterial_GetODESound(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG *_aul_Priority, ULONG ul_NumPrio)
{

#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
	ULONG			ul_Sound, ul_SoundRank, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_World) return 0;

	ul_SoundRank = 0xFFFFFFFF;
	ul_Sound = 0;

	pst_CurrentReport = _pst_World->ast_Reports;
	pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;
	for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
	{
		/* If the current report does NOT deal with our GameObject, we continue. */
		if(pst_CurrentReport->ul_Flag & COL_Cul_Inactive) continue;
		if(pst_CurrentReport->pst_A != _pst_GO) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Crossable) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
		if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
		if(!(pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

		if(_aul_Priority && ul_NumPrio)
		{
			if(pst_CurrentReport->ul_Triangle == _aul_Priority[0])
				return pst_CurrentReport->ul_Triangle;
			else
			{
				i = 0;
				while((i < ul_SoundRank) && (i < ul_NumPrio))
				{
					if(_aul_Priority[i] == pst_CurrentReport->ul_Triangle)
					{
						ul_SoundRank = i;
						ul_Sound = pst_CurrentReport->ul_Triangle;
					}
					i++;
				}

				if(!ul_Sound)
					ul_Sound = pst_CurrentReport->ul_Triangle;
			}
		}
		else
			return pst_CurrentReport->ul_Triangle;
	}

	return ul_Sound;
#else

	return 0;

#endif
}

/*
 =======================================================================================================================
    Aim: Returns the first report Game Material Slide that corresponds to the given filter. Note: The filter can be a
    Wall, the Ground ...
 =======================================================================================================================
 */
UCHAR COL_GameMaterial_GetSound(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_Filter)
{
	if(!_pst_World) return 0;
	if(_ul_Filter == COL_Cul_Ray)
	{
		if(_pst_World->st_RayInfo.pst_GMat) return _pst_World->st_RayInfo.pst_GMat->uc_Sound;
		return 0;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
		BOOL			b_SkipInactive;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_ul_Filter & COL_Cul_ReportIndex)
		{
			pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
			ERR_X_Warning
			(
				(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
				"AI Report Index overpass max reports",
				NULL
			);
#endif
			if(pst_CurrentReport->pst_GM)
				return pst_CurrentReport->pst_GM->uc_Sound;
			else
				return 0;
		}
		else
		{
			b_SkipInactive = TRUE;

			if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;

			pst_CurrentReport = _pst_World->ast_Reports;
			pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

			for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
			{
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;

				/* If the current report does NOT deal with our GameObject, we continue. */
				if(pst_CurrentReport->pst_A != _pst_GO) continue;
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
				if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

				/* We found it !!! */
				if(((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter) && (pst_CurrentReport->pst_GM))
				{
					COL_gb_SkipODEReports = TRUE;
					return pst_CurrentReport->pst_GM->uc_Sound;
				}
			}
		}

		COL_gb_SkipODEReports = TRUE;
		return 0;
	}
}

/*
 =======================================================================================================================
    Aim: Returns the first report Game Material Slide that corresponds to the given filter. Note: The filter can be a
    Wall, the Ground ...
 =======================================================================================================================
 */
UCHAR COL_GetDesign(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_Filter)
{
	if(!_pst_World) return 0;
	if(_ul_Filter == COL_Cul_Ray)
	{
		return _pst_World->st_RayInfo.uc_Design;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
		BOOL			b_SkipInactive;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_ul_Filter & COL_Cul_ReportIndex)
		{
			pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
			ERR_X_Warning
			(
				(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
				"AI Report Index overpass max reports",
				NULL
			);
#endif
			return pst_CurrentReport->uc_Design;
		}
		else
		{
			b_SkipInactive = TRUE;

			if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;

			pst_CurrentReport = _pst_World->ast_Reports;
			pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

			for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
			{
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;

				/* If the current report does NOT deal with our GameObject, we continue. */
				if(pst_CurrentReport->pst_A != _pst_GO) continue;
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
				if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

				/* We found it !!! */
				if((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter)
				{
					COL_gb_SkipODEReports = TRUE;
					return pst_CurrentReport->uc_Design;
				}
			}
		}

		COL_gb_SkipODEReports = TRUE;
		return 0;
	}
}


/*
 =======================================================================================================================
    Aim: Returns the first report Game Material ID that corresponds to the given filter. Note: The filter can be a
    Wall, the Ground ...
 =======================================================================================================================
 */
USHORT COL_GameMaterial_GetID(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_Filter)
{
	if(!_pst_World) return -1;
	if(_ul_Filter == COL_Cul_Ray)
	{
		if(_pst_World->st_RayInfo.pst_GMat) return _pst_World->st_RayInfo.pst_GMat->uw_Dummy;
		return -1;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
		BOOL			b_SkipInactive;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_ul_Filter & COL_Cul_ReportIndex)
		{
			pst_CurrentReport = &_pst_World->ast_Reports[_ul_Filter & COL_Cul_ReportIndexMask];
#ifdef ACTIVE_EDITORS
			ERR_X_Warning
			(
				(_ul_Filter & COL_Cul_ReportIndexMask) < _pst_World->ul_NbReports,
				"AI Report Index overpass max reports",
				NULL
			);
#endif
			if(pst_CurrentReport->pst_GM)
				return pst_CurrentReport->pst_GM->uw_Dummy;
			else
				return -1;
		}
		else
		{
			b_SkipInactive = TRUE;

			if(_ul_Filter & COL_Cul_Crossable) b_SkipInactive = FALSE;

			pst_CurrentReport = _pst_World->ast_Reports;
			pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

			for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
			{
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Inactive)) continue;

				/* If the current report does NOT deal with our GameObject, we continue. */
				if(pst_CurrentReport->pst_A != _pst_GO) continue;
				if(b_SkipInactive && (pst_CurrentReport->ul_Flag & COL_Cul_Crossable)) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
				if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Minor) continue;
				if(COL_gb_SkipODEReports && (pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE)) continue;

				/* We found it !!! */
				if(((pst_CurrentReport->ul_Flag & _ul_Filter) == _ul_Filter) && (pst_CurrentReport->pst_GM))
				{
					COL_gb_SkipODEReports = TRUE;
					return pst_CurrentReport->pst_GM->uw_Dummy;
				}
			}
		}

		COL_gb_SkipODEReports = TRUE;
		return -1;
	}
}

/*
 =======================================================================================================================
    Aim: Finds the different GameObject whose ZDM are colliding one Cob of the current GO.
 =======================================================================================================================
 */
ULONG COL_Report_GetZDMGameObjectList
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	ULONG				*_pul_Array,
	MATH_tdst_Vector	*_pst_Direction,
	ULONG				_ul_Filter
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject **dpst_Result;
	ULONG				ul_NbInter, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dpst_Result = ((OBJ_tdst_GameObject **) _pul_Array);
	ul_NbInter = 0;

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap)) return 0;

	for(i = 0; i < _pst_World->ul_NbReports; i++)
	{
		if
		(
			(_pst_World->ast_Reports[i].pst_B != _pst_GO)
		||	((_pst_World->ast_Reports[i].ul_Flag & _ul_Filter) != _ul_Filter)
		) continue;

		if
		(
			!(MATH_b_NulVector(_pst_Direction))
		&&	(MATH_f_DotProduct(_pst_Direction, &_pst_World->ast_Reports[i].st_Normal) <= 0.0f)
		) continue;

		*(dpst_Result + ul_NbInter) = _pst_World->ast_Reports[i].pst_A;
		ul_NbInter++;
	}

	return ul_NbInter;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_Report_GetAllReportsNormal
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	ULONG				*_pul_Array,
	ULONG				_ul_Filter
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*dpst_Result;
	ULONG				ul_Reports, i;
	BOOL				b_SkipEdgeWall;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dpst_Result = ((MATH_tdst_Vector *) _pul_Array);
	ul_Reports = 0;

	b_SkipEdgeWall = FALSE;

	if(_ul_Filter & COL_Cul_IgnoreEdgeWall) b_SkipEdgeWall = TRUE;

	_ul_Filter &= ~COL_Cul_IgnoreEdgeWall;

	for(i = 0; i < _pst_World->ul_NbReports; i++)
	{
		if
		(
			((_pst_World->ast_Reports[i].pst_A != _pst_GO) && (_pst_World->ast_Reports[i].pst_B != _pst_GO))
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Inactive)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Invalid)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Extra_Minor)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Crossable)
		||	(b_SkipEdgeWall && (_pst_World->ast_Reports[i].ul_Flag & COL_Cul_SlipperyEdge))
		||	((_pst_World->ast_Reports[i].ul_Flag & _ul_Filter) != _ul_Filter)
		||  (COL_gb_SkipODEReports && (_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Extra_ODE))
		) continue;

		MATH_CopyVector(dpst_Result + ul_Reports, &(_pst_World->ast_Reports[i].st_Normal));
		ul_Reports++;
	}

	COL_gb_SkipODEReports = TRUE;
	return ul_Reports;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_Report_GetNumberOfReports(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_Filter)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Reports, i;
	BOOL	b_SkipEdgeWall;
	/*~~~~~~~~~~~~~~~~~~~*/

	ul_Reports = 0;

	b_SkipEdgeWall = FALSE;

	if(_ul_Filter & COL_Cul_IgnoreEdgeWall) b_SkipEdgeWall = TRUE;

	_ul_Filter &= ~COL_Cul_IgnoreEdgeWall;

	for(i = 0; i < _pst_World->ul_NbReports; i++)
	{
		if
		(
			((_pst_World->ast_Reports[i].pst_A != _pst_GO) && (_pst_World->ast_Reports[i].pst_B != _pst_GO))
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Invalid)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Inactive)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Crossable)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Extra_Minor)
		||	(b_SkipEdgeWall && (_pst_World->ast_Reports[i].ul_Flag & COL_Cul_SlipperyEdge))
		||	((_pst_World->ast_Reports[i].ul_Flag & _ul_Filter) != _ul_Filter)
		||  (COL_gb_SkipODEReports && (_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Extra_ODE))
		) continue;

		if((_ul_Filter & COL_Cul_Ground) && (_pst_World->ast_Reports[i].st_Normal.z < 0.0f))
			continue;
		else
			ul_Reports++;
	}

	COL_gb_SkipODEReports = TRUE;
	return ul_Reports;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_Report_GetNumberOfReportsAndRanks
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	ULONG				_ul_Filter,
	ULONG				_ul_MaxElemInArray,
	ULONG				*pul_Array
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Reports, i, j;
	BOOL	b_SkipEdgeWall;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	ul_Reports = 0;

	b_SkipEdgeWall = FALSE;

	if(_ul_Filter & COL_Cul_IgnoreEdgeWall) b_SkipEdgeWall = TRUE;

	_ul_Filter &= ~COL_Cul_IgnoreEdgeWall;

	for(i = 0; i < _pst_World->ul_NbReports; i++)
	{
		if
		(
			((_pst_World->ast_Reports[i].pst_A != _pst_GO) && (_pst_World->ast_Reports[i].pst_B != _pst_GO))
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Invalid)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Inactive)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Crossable)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Extra_Minor)
		||	(b_SkipEdgeWall && (_pst_World->ast_Reports[i].ul_Flag & COL_Cul_SlipperyEdge))
		||	((_pst_World->ast_Reports[i].ul_Flag & _ul_Filter) != _ul_Filter)
		||  (COL_gb_SkipODEReports && (_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Extra_ODE))
		) continue;

		/* Avoid redundancies due to Collision iteration */
		for(j = 0; j < ul_Reports; j++)
		{
			if
			(
				((_pst_World->ast_Reports[*(pul_Array + j)].uw_Element) || (_pst_World->ast_Reports[*(pul_Array + j)].ul_Triangle))
			&&	((_pst_World->ast_Reports[*(pul_Array + j)].uw_Element) == _pst_World->ast_Reports[i].uw_Element)
			&&	((_pst_World->ast_Reports[*(pul_Array + j)].ul_Triangle) == _pst_World->ast_Reports[i].ul_Triangle)
			&&	((_pst_World->ast_Reports[*(pul_Array + j)].ul_Flag & ~COL_Cul_Dynamic) == (_pst_World->ast_Reports[i].ul_Flag & ~COL_Cul_Dynamic))
			) break;
		}

		if(j != ul_Reports) continue;

		if((_ul_Filter & COL_Cul_Ground) && (_pst_World->ast_Reports[i].st_Normal.z < 0.0f))
			continue;
		else
		{
			if(ul_Reports < _ul_MaxElemInArray)
			{
				*(pul_Array + ul_Reports) = i;
			}

			ul_Reports++;
			
			ul_Reports = (ul_Reports >= _ul_MaxElemInArray) ? _ul_MaxElemInArray - 1: ul_Reports; 
			ul_Reports = (ul_Reports < 0) ? 0 : ul_Reports;
			
			
		}
	}

	COL_gb_SkipODEReports = TRUE;
	return ul_Reports;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_Report_ComputeResultingNormal
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	ULONG				_ul_Filter,
	MATH_tdst_Vector	*_pst_Normal
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Reports, i;
	BOOL	b_SkipEdgeWall;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Normal) return;

	b_SkipEdgeWall = FALSE;

	if(_ul_Filter & COL_Cul_IgnoreEdgeWall) b_SkipEdgeWall = TRUE;

	_ul_Filter &= ~COL_Cul_IgnoreEdgeWall;

	MATH_InitVector(_pst_Normal, 0.0f, 0.0f, 0.0f);

	ul_Reports = 0;
	for(i = 0; i < _pst_World->ul_NbReports; i++)
	{
		if
		(
			((_pst_World->ast_Reports[i].pst_A != _pst_GO) && (_pst_World->ast_Reports[i].pst_B != _pst_GO))
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Inactive)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Invalid)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Crossable)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Extra_Minor)
		||	(b_SkipEdgeWall && (_pst_World->ast_Reports[i].ul_Flag & COL_Cul_SlipperyEdge))
		||	((_pst_World->ast_Reports[i].ul_Flag & _ul_Filter) != _ul_Filter)
		||  (COL_gb_SkipODEReports && (_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Extra_ODE))
		) continue;

		if((_ul_Filter & COL_Cul_Ground) && (_pst_World->ast_Reports[i].st_Normal.z < 0.0f))
		{
			continue;
		}
		else
		{
			MATH_AddEqualVector(_pst_Normal, &(_pst_World->ast_Reports[i].st_Normal));
			ul_Reports++;
		}
	}

	COL_gb_SkipODEReports = TRUE;
	if(!MATH_b_NulVector(_pst_Normal)) MATH_NormalizeEqualVector(_pst_Normal);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_Report_b_ObjectsAreColliding
(
	OBJ_tdst_GameObject *_pst_A,
	OBJ_tdst_GameObject *_pst_B,
	WOR_tdst_World		*_pst_World
)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	for(i = 0; i < _pst_World->ul_NbReports; i++)
	{
		if
		(
			(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Inactive)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Invalid)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Crossable)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Extra_Minor)
		) continue;

		if
		(
			((_pst_World->ast_Reports[i].pst_A == _pst_A) && (_pst_World->ast_Reports[i].pst_B == _pst_B))
		||	((_pst_World->ast_Reports[i].pst_A == _pst_B) && (_pst_World->ast_Reports[i].pst_B == _pst_A))
		) return TRUE;
		else
			continue;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_Report_ul_BestReportGet
(
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World,
	ULONG				_ul_FlagsToTest,
	ULONG				_ul_FlagsToSkip
)
{
	/*~~~~~~~~~~~~~~~~~~*/
	ULONG	i, index;
	float	f_SqrRecal;
	float	f_MaxSqrRecal;
	/*~~~~~~~~~~~~~~~~~~*/

	index = 0xFFFFFFFF;
	f_MaxSqrRecal = -1.0f;

	for(i = 0; i < _pst_World->ul_NbReports; i++)
	{
		if
		(
			(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Inactive)
		||	(_pst_World->ast_Reports[i].ul_Flag & COL_Cul_Invalid)
		) continue;

		if
		(
			!(_pst_World->ast_Reports[i].ul_Flag & _ul_FlagsToTest)
		||	(_pst_World->ast_Reports[i].ul_Flag & _ul_FlagsToSkip)
		) continue;

		if((_pst_World->ast_Reports[i].pst_A == _pst_GO) || (_pst_World->ast_Reports[i].pst_B == _pst_GO))
		{
			f_SqrRecal = MATH_f_SqrNormVector(&_pst_World->ast_Reports[i].st_Recal);
			if(f_SqrRecal > f_MaxSqrRecal)
			{
				index = i;
				f_MaxSqrRecal = f_SqrRecal;
			}
		}
		else
			continue;
	}

	return index;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_Report_ul_GMatReportGet(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_GMat)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	for(i = 0; i < _pst_World->ul_NbReports; i++)
	{
		/*
		 * if(_pst_World->ast_Reports[i].ul_Flag & (COL_Cul_Invalid |
		 * COL_Cul_Extra_Minor)) continue;
		 */
		if(_pst_World->ast_Reports[i].ul_Flag & (COL_Cul_Extra_Minor)) continue;

		if((_pst_World->ast_Reports[i].pst_A == _pst_GO) || (_pst_World->ast_Reports[i].pst_B == _pst_GO))
		{
			if(_pst_World->ast_Reports[i].pst_GM && (_pst_World->ast_Reports[i].pst_GM->ul_CustomBits & _ul_GMat))
				return i;
		}
		else
			continue;
	}

	return((ULONG) - 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_Report_ul_SlipperyEdgeNormalsGet(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, MATH_tdst_Vector *apst_Array)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap				*pst_ColMap;
	COL_tdst_IndexedTriangle	*pst_Triangle;
	OBJ_tdst_GameObject			*pst_CobGO;
	MATH_tdst_Vector		*pst_Face, *pst_Face2;
	ULONG						i, El, Tri;
	ULONG						ul_CollisionType;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_InitVector(apst_Array, 0.0f, 0.0f, 0.0f);
	MATH_InitVector(apst_Array + 1, 0.0f, 0.0f, 0.0f);

	for(i = 0; i < _pst_World->ul_NbReports; i++)
	{
		if((_pst_World->ast_Reports[i].pst_A == _pst_GO) && (_pst_World->ast_Reports[i].ul_Flag & COL_Cul_SlipperyEdge))
			break;
	}

	if(i == _pst_World->ul_NbReports)
		return 0;

	ul_CollisionType = _pst_World->ast_Reports[i].ul_Flag;

	pst_CobGO = _pst_World->ast_Reports[i].pst_B;

	pst_ColMap = ((COL_tdst_Base *)pst_CobGO->pst_Extended->pst_Col)->pst_ColMap;

	if(!pst_ColMap || (pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles)) return 0;


	El = (ULONG) _pst_World->ast_Reports[i].uw_Element;
	Tri = (ULONG) _pst_World->ast_Reports[i].ul_Triangle;

	pst_Face = pst_ColMap->dpst_Cob[0]->pst_TriangleCob->dst_FaceNormal;
	for(i = 0; i < El; i++)
		pst_Face += pst_ColMap->dpst_Cob[0]->pst_TriangleCob->dst_Element[i].uw_NbTriangles;

	pst_Face += Tri;

	MATH_TransformVector(apst_Array, pst_CobGO->pst_GlobalMatrix, pst_Face);

	pst_Triangle = &(pst_ColMap->dpst_Cob[0]->pst_TriangleCob->dst_Element[El].dst_Triangle[Tri]);

	if(ul_CollisionType & COL_Cul_Edge12)
	{
		if(pst_Triangle->auw_Prox[0] != 0xFFFF)
		{
			pst_Face2 = &pst_ColMap->dpst_Cob[0]->pst_TriangleCob->dst_FaceNormal[pst_Triangle->auw_Prox[0]];
		}
		else
			return 1;
	}
	else
	{
		if(ul_CollisionType & COL_Cul_Edge13)
		{
			if(pst_Triangle->auw_Prox[1] != 0xFFFF)
			{
				pst_Face2 = &pst_ColMap->dpst_Cob[0]->pst_TriangleCob->dst_FaceNormal[pst_Triangle->auw_Prox[1]];
			}
			else
				return 1;
		}
		else
		{
			if(pst_Triangle->auw_Prox[2] != 0xFFFF)
			{
				pst_Face2 = &pst_ColMap->dpst_Cob[0]->pst_TriangleCob->dst_FaceNormal[pst_Triangle->auw_Prox[2]];
			}
			else
				return 1;
		}
	}

	MATH_TransformVector(apst_Array + 1, pst_CobGO->pst_GlobalMatrix, pst_Face2);
	return 2;
}



#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

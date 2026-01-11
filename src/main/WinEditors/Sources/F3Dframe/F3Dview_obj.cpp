/*$T F3Dview_obj.cpp GC 1.139 04/07/04 13:00:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "EDImsg.h"
#include "EDIpaths.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "SOFT/SOFThelper.h"
#include "GEOmetric/GEOstaticlod.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "ENGine/Sources/GRP/GRPsave.h"
#include "ENGine/Sources/GRP/GRPload.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "ENGine/Sources/MODifier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_FCLONE.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDItors/Sources/PreFaB/PFBframe.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "F3Dframe/F3Dstrings.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDbank.h"
#include "EDItors/Sources/EVEnts/EVEmsg.h"
#include "EDItors/Sources/PreFaB/PFBdata.h"
#include "DIAlogs/DIAvector_dlg.h"
#include "DIAlogs/DIAcopymatrixfrom_dlg.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
//#include "DIAlogs/DIASelectGroup_dlg.h"
#include "Res\res.h"

#ifdef JADEFUSION
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAreplace_dlg.h"
#include "DIAlogs/CCheckList.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SoftBody.h"
#include "Light/LIGHTrejection.h"
#endif

EDI_cl_BaseFrame	*gpo_AssociatedEditor = NULL;
#ifdef JADEFUSION
extern void		GRP_AfterLoaded(WOR_tdst_World *_pst_World, OBJ_tdst_Group *_pst_Group);
extern void		MATH_VecRotate(MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, float f_Angle);
extern ULONG	OBJ_ul_GameObjectRLICallback(ULONG _ul_PosFile);
#else
extern "C" void		GRP_AfterLoaded(WOR_tdst_World *_pst_World, OBJ_tdst_Group *_pst_Group);
extern "C" void		MATH_VecRotate(MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, float f_Angle);
extern "C" ULONG	OBJ_ul_GameObjectRLICallback(ULONG _ul_PosFile);
#endif
//extern "C" void FCLONE_Modifier_Update(MDF_tdst_Modifier *_pst_Mod);

/*$4
 ***********************************************************************************************************************
    Internal data structure
 ***********************************************************************************************************************
 */

typedef struct	F3D_tdst_DuplicateParams_
{
	F3D_cl_View			*po_View;
	OBJ_tdst_GameObject **pst_Src;
	OBJ_tdst_GameObject **pst_Dup;
	int					i_Count;
	int					i_Current;
} F3D_tdst_DuplicateParams;

/*$4
 ***********************************************************************************************************************
    Object Functions - Moving
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_SnapToGrid(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*pst_Vec;
	float				f_ModXY;
	float				f_ModZ;
	MAI_tdst_WinHandles *pst_WinHandles;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_WinHandles = (MAI_tdst_WinHandles *) _ul_Param1;

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	f_ModXY = pst_WinHandles->pst_DisplayData->pst_Helpers->f_GridSizeXY;
	f_ModZ = pst_WinHandles->pst_DisplayData->pst_Helpers->f_GridSizeZ;

	pst_Vec = OBJ_pst_GetAbsolutePosition(pst_GO);

	//    Objects are moved in local ref, but snaped in global ref -> snap all coordinates.
    if (f_ModXY)
    {
        pst_Vec->x = MATH_f_FloatModulo(pst_Vec->x, f_ModXY);
        pst_Vec->y = MATH_f_FloatModulo(pst_Vec->y, f_ModXY);
    }

    if (f_ModZ)
        pst_Vec->z = MATH_f_FloatModulo(pst_Vec->z, f_ModZ);

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG F3D_ul_GetKeyFromPrefabName(char *_pc_Name)
{
	CString key("0x");
	CString name(_pc_Name);

	int indexBracket1 = name.Find('[');
	int indexBracket2 = name.Find(']');
	if (indexBracket1 == -1 || indexBracket2 == -1)
		return BIG_C_InvalidKey;

	for (int i = indexBracket1 + 1 ; i < indexBracket2 ; ++i)
		key.AppendChar(name[i]);

	int iTemp;
	sscanf(key,"%x",&iTemp);

	return iTemp;
}

/*
 =======================================================================================================================
 	Aim : Recompute a non-hierarchised prefab's elements position according to prefab's global pos after snapping it to grid, 
	in order to prevent elements from snapping to grid (by keeping their relative location)
 =======================================================================================================================
 */
BOOL F3D_b_PrefabAfterSnap(SEL_tdst_Selection *_pst_Sel, LONG _l_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Prefab		*P;
	EPFB_cl_Frame		*po_PFBeditor;
	ULONG				ul_Index;
	int					i;
	OBJ_tdst_GameObject	*pst_GO, *pst_Father = NULL;
	SEL_tdst_SelectedItem	*pst_Item;
	MATH_tdst_Matrix	*pst_Mat;
	BOOL				b_DontDestroy = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mat = (MATH_tdst_Matrix *) MEM_p_Alloc(sizeof(MATH_tdst_Matrix));

	pst_Item = _pst_Sel->pst_FirstItem;
	while(pst_Item != NULL)
	{
	    SEL_tdst_SelectedItem *pst_NextItem = pst_Item->pst_Next;
		b_DontDestroy = FALSE;

		if(pst_Item->l_Flag & _l_Flag)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_Item->p_Content;
			/* Only do it for non hierarchised prefabs */
			if (pst_GO->ul_PrefabKey == BIG_C_InvalidKey || (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
			{
				pst_Item = pst_NextItem;
				continue;
			}

			ul_Index = BIG_ul_SearchKeyToFat(pst_GO->ul_PrefabKey);
			if (ul_Index == BIG_C_InvalidIndex)
			{
				pst_Item = pst_NextItem;
				continue;
			}

			/* try to get prefab from editor */
			po_PFBeditor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
			if((po_PFBeditor) && po_PFBeditor->mpst_Prefab && (po_PFBeditor->mpst_Prefab->ul_Index == ul_Index))
			{
				P = po_PFBeditor->mpst_Prefab;
				b_DontDestroy = TRUE;
			}
			else
				P = Prefab_pst_Load(ul_Index, NULL);
			
			if(!P) 
			{
				pst_Item = pst_NextItem;
				continue;
			}
			else if (!P->l_NbRef)
			{
				if (!b_DontDestroy)
					Prefab_Destroy(&P);
				pst_Item = pst_NextItem;
				continue;
			}

			/* Get prefab's main element */
			SEL_tdst_SelectedItem *pst_Item2 = _pst_Sel->pst_FirstItem;
			while(pst_Item2 != NULL)
			{
				SEL_tdst_SelectedItem *pst_NextItem2 = pst_Item2->pst_Next;
				pst_Father = (OBJ_tdst_GameObject *) pst_Item2->p_Content;
				int a = F3D_ul_GetKeyFromPrefabName(pst_Father->sz_Name);
				int b = F3D_ul_GetKeyFromPrefabName(pst_GO->sz_Name);
				if(pst_Father->ul_PrefabObjKey == P->dst_Ref[0].ul_Key 
					&& F3D_ul_GetKeyFromPrefabName(pst_Father->sz_Name) == F3D_ul_GetKeyFromPrefabName(pst_GO->sz_Name))
				{
					break;
				}
				pst_Item2 = pst_NextItem2;
			}

			if ((ULONG) pst_Father == BIG_C_InvalidIndex || pst_Father == NULL)
			{
				if (!b_DontDestroy)
					Prefab_Destroy(&P);
				pst_Item = pst_NextItem;
				continue;
			}

			/* Recompute subelement's position according to main element's global location */
			for (i = 1 ; i < P->l_NbRef ; i++)
			{
				if (P->dst_Ref[i].ul_Key == pst_GO->ul_PrefabObjKey)
				{
					MATH_MulMatrixMatrix(pst_Mat, &P->dst_Ref[i].st_Matrix, OBJ_pst_GetAbsoluteMatrix(pst_Father));
					MATH_SetTranslation(OBJ_pst_GetAbsoluteMatrix(pst_GO), MATH_pst_GetTranslation(pst_Mat));
					break;
				}
			}

			if (!b_DontDestroy)
				Prefab_Destroy(&P);
		}

		pst_Item = pst_NextItem;
	}

	MEM_Free(pst_Mat);

	return TRUE;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_Move(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Father;
	OBJ_tdst_GameObject *pst_GO1;
	MATH_tdst_Vector	st_Move, st_NegMove, *pst_Vector;
	CAM_tdst_Camera		*pst_Camera;
	MAI_tdst_WinHandles *pst_WinHandles;
	BOOL				childmode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;

	/* Don't move if father is selected (avoid dual move) */
	if
	(
		(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	&&	(pst_GO->pst_Base->pst_Hierarchy->pst_Father->ul_EditorFlags & OBJ_C_EditFlags_Selected)
	) return TRUE;

	if(ANI_b_IsGizmoAnim(pst_GO, &pst_Father))
	{
		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		{
			pst_GO1 = WOR_pst_GetGizmoFromBone(pst_GO->pst_Base->pst_Hierarchy->pst_Father);
			if(pst_GO1 && (pst_GO1->ul_EditorFlags & OBJ_C_EditFlags_Selected)) return TRUE;
		}
	}

	if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) && pst_GO->pst_Base && pst_GO->pst_Base->pst_Dyna)
	{
		pst_GO->pst_Base->pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColEnable;
		pst_GO->pst_Base->pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColDisable;
	}

	pst_Vector = (MATH_tdst_Vector *) _ul_Param1;
	pst_WinHandles = (MAI_tdst_WinHandles *) _ul_Param2;
	pst_Camera = (CAM_tdst_Camera *) (&pst_WinHandles->pst_DisplayData->st_Camera);

	MATH_TransformVector(&st_NegMove, &pst_Camera->st_Matrix, pst_Vector);
	MATH_NegVector(&st_Move, &st_NegMove);

	OBJ_AddAbsoluteVector(pst_GO, &st_Move);

	if(pst_WinHandles->pst_DisplayData->uc_StaticBV)
	{
		OBJ_BV_Move(pst_GO, &st_NegMove, TRUE);
	}

	/* Recompute HIE */
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		pst_Father = pst_GO->pst_Base->pst_Hierarchy->pst_Father;
		if(!(pst_Father->ul_EditorFlags & OBJ_C_EditFlags_Selected))
		{
			OBJ_ComputeLocalWhenHie(pst_GO);
		}
	}

	childmode = FALSE;
	if(GetAsyncKeyState('Q') < 0)
	{
		childmode = TRUE;
		WOR_RecomputeAllChilds(pst_GO);
	}

	/* Move a gizmo in anim mode ? */
	if(ANI_b_IsGizmoAnim(pst_GO, &pst_Father) && gpo_AssociatedEditor)
	{
		if((childmode && (pst_GO != pst_Father)) || ((pst_GO == pst_Father) && (!childmode)))
		{
			M_MF()->SendMessageToLinks
				(
					gpo_AssociatedEditor,
					EEVE_MESSAGE_RECOMPUTEHIE,
					(ULONG) pst_Father,
					(ULONG) pst_GO
				);
		}
		else
		{
			M_MF()->SendMessageToLinks
				(
					gpo_AssociatedEditor,
					EEVE_MESSAGE_RECOMPUTE,
					(ULONG) pst_Father,
					(ULONG) pst_GO
				);
		}
	}

	/* Magic box move */
	else
	{
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
			OBJ_SetFlashMatrix(pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO));
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_Move(MATH_tdst_Vector *_pst_Move)
{
	gpo_AssociatedEditor = mpo_AssociatedEditor;
	if(mst_WinHandles.pst_World == NULL) return;

	/* BV edition */
	if(mst_WinHandles.pst_DisplayData->uc_EditBounding)
	{
		Selection_BV_Move(_pst_Move);
		LINK_Refresh();
		return;
	}

	/* ODE edition */
	if(mst_WinHandles.pst_DisplayData->uc_EditODE)
	{
		Selection_ODE_Move(_pst_Move);
		LINK_Refresh();
		return;
	}

	/* Normal edition mode */
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_Move, (ULONG) _pst_Move, (ULONG) & mst_WinHandles);

	if(M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_SnapGrid)
	{
		SEL_EnumItem
		(
			M_F3D_Sel,
			SEL_C_SIF_Object,
			F3D_b_SnapToGrid,
			(ULONG) & mst_WinHandles,
            NULL
		);

		/* Recompute elements position for non-hierarchised prefabs */
		F3D_b_PrefabAfterSnap(M_F3D_Sel, SEL_C_SIF_Object);
	}

	/* Zone edition */
	Selection_Zone_Move(_pst_Move);

	LINK_Refresh();
}

/*$F
 =======================================================================================================================
    Aim:    Rotate all selected object

    In:     _pst_Angle      -> Rotation vector
            _pst_Center     -> Rotation Center (if NULL GameObject center is taken)
            _pst_Axis       -> Rotation Axis
 =======================================================================================================================
 */
static MATH_tdst_Vector F3D_sgst_RotationCenter, *F3D_sgpst_RotationCenter;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_Rotate(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_GO1, *pst_Father;
	MATH_tdst_Vector	*pst_Vector;
	MATH_tdst_Vector	*pst_Axis, st_Axis, v;
	MATH_tdst_Matrix	st_TempMatrix, st_Rotation;
	float				f_Angle;
	BOOL				childmode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;

	/* Don't move if father is selected (avoid dual move) */
	if
	(
		(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	&&	(pst_GO->pst_Base)
	&&	(pst_GO->pst_Base->pst_Hierarchy)
	&&	(pst_GO->pst_Base->pst_Hierarchy->pst_Father)
	&&	(pst_GO->pst_Base->pst_Hierarchy->pst_Father->ul_EditorFlags & OBJ_C_EditFlags_Selected)
	) return TRUE;

	if(ANI_b_IsGizmoAnim(pst_GO, &pst_Father))
	{
		if
		(
			(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		&&	(pst_GO->pst_Base)
		&&	(pst_GO->pst_Base->pst_Hierarchy)
		&&	(pst_GO->pst_Base->pst_Hierarchy->pst_Father)
		)
		{
			pst_GO1 = WOR_pst_GetGizmoFromBone(pst_GO->pst_Base->pst_Hierarchy->pst_Father);
			if(pst_GO1 && (pst_GO1->ul_EditorFlags & OBJ_C_EditFlags_Selected)) return TRUE;
		}
	}

	pst_Axis = (MATH_tdst_Vector *) _ul_Param1;
	pst_Vector = (MATH_tdst_Vector *) _ul_Param2;
	f_Angle = pst_Vector->x;
	if(f_Angle == 0)
	{
		f_Angle = pst_Vector->y;
		if(f_Angle == 0)
		{
			f_Angle = pst_Vector->z;
			if(f_Angle == 0) return TRUE;
		}
	}

	/* Transform axis into localaxis of object */
	MATH_SetIdentityMatrix(&st_TempMatrix);
	MATH_GetRotationMatrix(&st_TempMatrix, pst_GO->pst_GlobalMatrix);
	MATH_SetRotationType(&st_TempMatrix);
	MATH_TranspEq33Matrix(&st_TempMatrix);

	MATH_TransformVector(&st_Axis, &st_TempMatrix, pst_Axis);
	MATH_NormalizeVector(&st_Axis, &st_Axis);

	/* Compute the matrix for the rotation around that axis */
	MATH_MakeRotationMatrix_AxisAngle(&st_Rotation, &st_Axis, f_Angle, NULL, 1);

	/* Make the rotation */
	MATH_TranspEq33Matrix(&st_TempMatrix);
	MATH_GetScaleVector(&v, pst_GO->pst_GlobalMatrix);
	MATH_Mul33MatrixMatrix(pst_GO->pst_GlobalMatrix, &st_Rotation, &st_TempMatrix, 1);
	MATH_SetScale(pst_GO->pst_GlobalMatrix, &v);

	if(F3D_sgpst_RotationCenter)
	{
		MATH_SubVector(&v, MATH_pst_GetTranslation(pst_GO->pst_GlobalMatrix), F3D_sgpst_RotationCenter);

        // Avoid small translations due to floating point errors
        if (MATH_f_NormVector(&v) > 0.001f)
        {
    		MATH_MakeRotationMatrix_AxisAngle(&st_Rotation, pst_Axis, f_Angle, NULL, 1);
	    	MATH_TransformVector(&v, &st_Rotation, &v);
		    MATH_AddEqualVector(&v, F3D_sgpst_RotationCenter);
		    MATH_SetTranslation(pst_GO->pst_GlobalMatrix, &v);
        }
	}

	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		pst_Father = pst_GO->pst_Base->pst_Hierarchy->pst_Father;
		if(pst_Father && !(pst_Father->ul_EditorFlags & OBJ_C_EditFlags_Selected))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	SaveT;
			/*~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_CopyVector(&SaveT, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
			OBJ_ComputeLocalWhenHie(pst_GO);
			MATH_CopyVector(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &SaveT);
		}
	}

	childmode = FALSE;
	if(GetAsyncKeyState('Q') < 0)
	{
		childmode = TRUE;
		WOR_RecomputeAllChilds(pst_GO);
	}

	/* Move a gizmo in anim mode ? */
	if(ANI_b_IsGizmoAnim(pst_GO, &pst_Father) && gpo_AssociatedEditor)
	{
		if((childmode && (pst_GO != pst_Father)) || ((pst_GO == pst_Father) && (!childmode)))
		{
			M_MF()->SendMessageToLinks
				(
					gpo_AssociatedEditor,
					EEVE_MESSAGE_RECOMPUTEHIE,
					(ULONG) pst_Father,
					(ULONG) pst_GO
				);
		}
		else
		{
			M_MF()->SendMessageToLinks
				(
					gpo_AssociatedEditor,
					EEVE_MESSAGE_RECOMPUTE,
					(ULONG) pst_Father,
					(ULONG) pst_GO
				);
		}
	}
	else if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
		OBJ_SetFlashMatrix(pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO));

	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_Rotate
(
	MATH_tdst_Vector	*_pst_Center,
	MATH_tdst_Vector	*_pst_Axe,
	MATH_tdst_Vector	*_pst_Angle
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CAM_tdst_Camera		*pst_Camera;
	MATH_tdst_Vector	st_GlobalAxis;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gpo_AssociatedEditor = mpo_AssociatedEditor;
	if(mst_WinHandles.pst_World == NULL) return;
	if(mst_WinHandles.pst_DisplayData->uc_EditBounding) return;

	/*
	 * Axis and center are exprimed into camera system axis, compute in world system
	 * axis
	 */
	pst_Camera = (CAM_tdst_Camera *) (&mst_WinHandles.pst_DisplayData->st_Camera);

	MATH_TransformVector(&st_GlobalAxis, &pst_Camera->st_Matrix, _pst_Axe);
	MATH_NormalizeVector(&st_GlobalAxis, &st_GlobalAxis);

	if(_pst_Center)
	{
		MATH_TransformVertex(&F3D_sgst_RotationCenter, &pst_Camera->st_Matrix, _pst_Center);

        // Anti bug : if rotation center is small enough, don't use it (to avoid precision problems)
        if (MATH_f_NormVector(&F3D_sgst_RotationCenter) < 0.001f)
            F3D_sgpst_RotationCenter = NULL;
        else
    		F3D_sgpst_RotationCenter = &F3D_sgst_RotationCenter;
	}
	else
		F3D_sgpst_RotationCenter = NULL;

	/* ODE edition */
	if(mst_WinHandles.pst_DisplayData->uc_EditODE)
	{
		Selection_ODE_Rotate(&st_GlobalAxis, _pst_Angle, F3D_sgpst_RotationCenter);
		LINK_Refresh();
		return;
	}

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_Rotate, (ULONG) & st_GlobalAxis, (ULONG) _pst_Angle);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_ResetRotation(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	MATH_ClearRotation(OBJ_pst_GetAbsoluteMatrix(pst_GO));

	/* Move a gizmo in anim mode ? */
	if(ANI_b_IsGizmoAnim(pst_GO, &pst_Father) && gpo_AssociatedEditor)
	{
		M_MF()->SendMessageToLinks(gpo_AssociatedEditor, EEVE_MESSAGE_RECOMPUTE, (ULONG) pst_Father, (ULONG) pst_GO);
	}

	/* hierarchy */
	OBJ_ComputeLocalWhenHie(pst_GO);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_ResetRotation(void)
{
	gpo_AssociatedEditor = mpo_AssociatedEditor;
	if(mst_WinHandles.pst_World == NULL) return;

	mpo_CurrentModif = new F3D_cl_Undo_HelperModif(this, FALSE);
	mpo_CurrentModif->SetDesc("Reset rotation");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_ResetRotation, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_ScaleObject(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*pst_Vector, st_Scale;
	MATH_tdst_Matrix	*pst_M;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Vector = (MATH_tdst_Vector *) _ul_Param1;
	if(!pst_GO) return TRUE;

	pst_M = OBJ_pst_GetAbsoluteMatrix(pst_GO);

	if(MATH_b_TestScaleType(pst_M))
	{
		MATH_GetScale(&st_Scale, pst_M);
		st_Scale.x *= pst_Vector->x;
		st_Scale.y *= pst_Vector->y;
		st_Scale.z *= pst_Vector->z;
		pst_Vector = &st_Scale;
	}

	OBJ_ScaleSet(pst_GO, pst_Vector);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_Scale(MATH_tdst_Vector *_pst_Scale)
{
	if(mst_WinHandles.pst_World == NULL) return;

	/* BV edition */
	if(mst_WinHandles.pst_DisplayData->uc_EditBounding)
	{
		Selection_BV_Scale(_pst_Scale);
		LINK_Refresh();
		return;
	}

	/* ODE edition */
	if(mst_WinHandles.pst_DisplayData->uc_EditODE)
	{
		Selection_ODE_Scale(_pst_Scale);
		LINK_Refresh();
		return;
	}
	/* Normal edition */
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_ScaleObject, (ULONG) _pst_Scale, (ULONG) & mst_WinHandles);

	/* Zone edition */
	Selection_Zone_Scale(_pst_Scale);

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_ResetScale(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Matrix	st_Matrix;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	MATH_CopyMatrix(&st_Matrix, OBJ_pst_GetAbsoluteMatrix(pst_GO));
	MATH_CopyVector(&st_Pos, MATH_pst_GetTranslation(&st_Matrix));
	MATH_GetRotationMatrix(&st_Matrix, &st_Matrix);
	MATH_SetTranslation(&st_Matrix, &st_Pos);
	OBJ_SetAbsoluteMatrix(pst_GO, &st_Matrix);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_ResetScale(void)
{
	if(mst_WinHandles.pst_World == NULL) return;

	mpo_CurrentModif = new F3D_cl_Undo_HelperModif(this, FALSE);
	mpo_CurrentModif->SetDesc("Reset scale");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_ResetScale, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static MATH_tdst_Vector F3D_v_TeleportRef;
static int				F3D_i_TeleportRef_ok;
/**/
BOOL F3D_b_GAO_TeleportObject(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_Father;
	MATH_tdst_Vector	*pst_Pos;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(ANI_b_IsGizmoAnim(pst_GO, &pst_Father)) return TRUE;

	pst_Pos = (MATH_tdst_Vector *) _ul_Param1;

	if(!pst_GO) return TRUE;
	if(!pst_Pos) return TRUE;

	if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) && pst_GO->pst_Base && pst_GO->pst_Base->pst_Dyna)
	{
		pst_GO->pst_Base->pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColEnable;
		pst_GO->pst_Base->pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColDisable;
	}
	
	if ( F3D_i_TeleportRef_ok <= 0)
	{
		if ( F3D_i_TeleportRef_ok == 0 ) F3D_i_TeleportRef_ok = 1;
		MATH_CopyVector( &F3D_v_TeleportRef, OBJ_pst_GetAbsolutePosition( pst_GO ) );
		OBJ_SetAbsolutePosition(pst_GO, pst_Pos);
	}
	else
	{
		MATH_SubVector( &st_Pos, OBJ_pst_GetAbsolutePosition( pst_GO ), &F3D_v_TeleportRef );
		MATH_AddEqualVector( &st_Pos, pst_Pos );
		OBJ_SetAbsolutePosition(pst_GO, &st_Pos);
	}

	OBJ_ComputeLocalWhenHie(pst_GO);

	if
	(
		(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	&&	pst_GO->pst_Extended
	&&	pst_GO->pst_Extended->pst_Col
	&&	((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance
	)
	{
		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		{
			MATH_CopyMatrix
			(
				((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
				&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix
			);
		}
		else
		{
			MATH_CopyMatrix
			(
				((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
				pst_GO->pst_GlobalMatrix
			);
		}
	}

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
			OBJ_SetFlashMatrix(pst_GO, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
		else
			OBJ_SetFlashMatrix(pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO));
	}

	if(GetAsyncKeyState('Q') < 0)
	{
		WOR_RecomputeAllChilds(pst_GO);
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_Teleport(BOOL b_Center)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POINT				st_Pt;
	GDI_tdst_Device		*pst_Dev;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return;
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;

	if(b_Center)
	{
		v.x = v.y = 0.5f;
		st_Pt.x = pst_Dev->l_Width / 2;
		st_Pt.y = pst_Dev->l_Height / 2;
	}
	else
	{
		GetCursorPos(&st_Pt);
		ScreenToClient(&st_Pt);

		if((unsigned long) st_Pt.x > (unsigned long) pst_Dev->l_Width) return;
		if((unsigned long) st_Pt.y > (unsigned long) pst_Dev->l_Height) return;

		v.x = (float) st_Pt.x / (float) pst_Dev->l_Width;
		v.y = 1.0f - ((float) st_Pt.y / (float) pst_Dev->l_Height);
	}

	b_PickDepth(&v, &v.z);

	v.x = (float) st_Pt.x;
	v.y = (float) st_Pt.y;
	CAM_2Dto3D(&mst_WinHandles.pst_DisplayData->st_Camera, &v, &v);

	mpo_CurrentModif = new F3D_cl_Undo_HelperModif(this, FALSE);
	mpo_CurrentModif->SetDesc("Teleport");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;
	
	F3D_i_TeleportRef_ok = ( ((EOUT_cl_Frame *) mpo_AssociatedEditor)->mst_Ini.c_Teleport_KeepRelativePos ) ? 0 : -1;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_TeleportObject, (ULONG) & v, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_DropToFloor(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*pst_Dir, *pst_Pos;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;

	pst_Dir = (MATH_tdst_Vector *) _ul_Param1;
	if(!pst_Dir) return FALSE;

	if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) && pst_GO->pst_Base && pst_GO->pst_Base->pst_Dyna)
	{
		pst_GO->pst_Base->pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColEnable;
		pst_GO->pst_Base->pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColDisable;
	}

	pst_Pos = OBJ_pst_GetAbsolutePosition(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	if
	(
		COL_Visual_RayCast
			(
				pst_World,
				pst_GO,
				pst_Pos,
				pst_Dir,
				10000.0f,
				0xFFFFFFFF,
				0,
				OBJ_C_OR_OR_IdentityFlags,
				NULL,
				FALSE,
				FALSE
			)
	)
	{
		OBJ_SetAbsolutePosition(pst_GO, &pst_World->st_RayInfo.st_CollidedPoint);
		OBJ_ComputeLocalWhenHie(pst_GO);

		if(GetAsyncKeyState('Q') < 0)
		{
			WOR_RecomputeAllChilds(pst_GO);
		}

		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
		{
			if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
				OBJ_SetFlashMatrix(pst_GO, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
			else
				OBJ_SetFlashMatrix(pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO));
		}
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_DropToFloor(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return;

	mpo_CurrentModif = new F3D_cl_Undo_HelperModif(this, FALSE);
	mpo_CurrentModif->SetDesc("Drop To Floor");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	MATH_InitVector(&v, 0, 0, -1);
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_DropToFloor, (ULONG) & v, 0);
	LINK_Refresh();
}

static ULONG	F3D_ul_CopyMatrixFlags;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

BOOL F3D_b_GAO_CopyMatrix(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Matrix	*M, *Matrix;
	MATH_tdst_Matrix	*Inv, NewInv, Temp, Transform;
	GEO_tdst_Object		*pst_Geo;
	GEO_Vertex			*V, *VLast;
	MATH_tdst_Vector	st_Scale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	M = (MATH_tdst_Matrix *) _ul_Param1;
	Inv = (MATH_tdst_Matrix *) _ul_Param2;

	if(!pst_GO) return TRUE;
	if(!M) return FALSE;

	if(F3D_ul_CopyMatrixFlags == EDIA_CopyMatrixFlag_All)
	{
		/* pivot : compute transform matrix */
		if(Inv) MATH_MulMatrixMatrix(&Transform, OBJ_pst_GetAbsoluteMatrix(pst_GO), Inv);
		OBJ_SetAbsoluteMatrix(pst_GO, M);
	}
	else
	{
		if(Inv) MATH_CopyMatrix(&Temp, OBJ_pst_GetAbsoluteMatrix(pst_GO));

		Matrix = OBJ_pst_GetAbsoluteMatrix(pst_GO);
		if(F3D_ul_CopyMatrixFlags & EDIA_CopyMatrixFlag_Trans)
		{
			if(F3D_ul_CopyMatrixFlags & EDIA_CopyMatrixFlag_TransX) Matrix->T.x = M->T.x;
			if(F3D_ul_CopyMatrixFlags & EDIA_CopyMatrixFlag_TransY) Matrix->T.y = M->T.y;
			if(F3D_ul_CopyMatrixFlags & EDIA_CopyMatrixFlag_TransZ) Matrix->T.z = M->T.z;
		}

		if(F3D_ul_CopyMatrixFlags & EDIA_CopyMatrixFlag_Rotation)
		{
			if(!(F3D_ul_CopyMatrixFlags & EDIA_CopyMatrixFlag_Scale))
			{
				MATH_GetScale(&st_Scale, Matrix);
				MATH_Copy33Matrix(Matrix, M);
				MATH_SetScale(Matrix, &st_Scale);
			}
			else
				MATH_Copy33Matrix(Matrix, M);
		}
		else if(F3D_ul_CopyMatrixFlags & EDIA_CopyMatrixFlag_Scale)
		{
			MATH_GetScale(&st_Scale, M);
			MATH_SetScale(Matrix, &st_Scale);
		}

		MATH_SetCorrectType(Matrix);

		if(Inv)
		{
			MATH_SetIdentityMatrix(&NewInv);
			MATH_InvertMatrix(&NewInv, Matrix);
			Inv = &NewInv;
			MATH_MulMatrixMatrix(&Transform, &Temp, Inv);
		}
	}

	OBJ_ComputeLocalWhenHie(pst_GO);

	if(GetAsyncKeyState('Q') < 0)
	{
		WOR_RecomputeAllChilds(pst_GO);
	}

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
		OBJ_SetFlashMatrix(pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO));

	/* pivot : transform geometry points */
	if(Inv)
	{
		pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		V = pst_Geo->dst_Point;
		VLast = V + pst_Geo->l_NbPoints;
		while(V < VLast)
		{
			MATH_TransformVertex((MATH_tdst_Vector *) V, &Transform, (MATH_tdst_Vector *) V);
			V++;
		}
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_CopyMatrix(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POINT							st_Pt;
	GDI_tdst_Device					*pst_Dev;
	OBJ_tdst_GameObject				*pst_GO;
	MATH_tdst_Matrix				M, Inv, *pInv;
	SOFT_tdst_PickingBuffer_Pixel	*pst_Pixel;
	EDIA_cl_CopyMatrixFrom			*po_Dlg;
	ULONG							ul_Flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return;

	GetCursorPos(&st_Pt);
	ScreenToClient(&st_Pt);
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;

	if((unsigned long) st_Pt.x > (unsigned long) pst_Dev->l_Width) return;
	if((unsigned long) st_Pt.y > (unsigned long) pst_Dev->l_Height) return;
	if(!Pick_l_UnderPoint(&st_Pt, SOFT_Cuc_PBQF_GameObject, 0)) return;
	pst_Pixel = Pick_pst_GetFirst(SOFT_Cuc_PBQF_GameObject, -1);
	pst_GO = (OBJ_tdst_GameObject *) pst_Pixel->ul_Value;
	MATH_CopyMatrix(&M, OBJ_pst_GetAbsoluteMatrix(pst_GO));

	ul_Flags = ((EOUT_cl_Frame *) mpo_AssociatedEditor)->mst_Ini.uc_CopyMatrixFrom_Flags;
	po_Dlg = new EDIA_cl_CopyMatrixFrom(pst_GO, ul_Flags);
	if(po_Dlg->DoModal() != IDOK) return;
	ul_Flags = po_Dlg->mul_Flags;
	delete po_Dlg;
	((EOUT_cl_Frame *) this->mpo_AssociatedEditor)->mst_Ini.uc_CopyMatrixFrom_Flags = (UCHAR) ul_Flags;
	if(ul_Flags == 0) return;

	if((M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_MovePivot) && Selection_b_IsInSubObjectMode(FALSE))
	{
		mpo_CurrentModif = new F3D_cl_Undo_HelperModif(this, TRUE);
		mpo_CurrentModif->SetDesc("Copy pivot");
		MATH_SetIdentityMatrix(&Inv);
		MATH_InvertMatrix(&Inv, &M);
		pInv = &Inv;
	}
	else
	{
		mpo_CurrentModif = new F3D_cl_Undo_HelperModif(this, FALSE);
		mpo_CurrentModif->SetDesc("Copy matrix");
		pInv = NULL;
	}

	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	F3D_ul_CopyMatrixFlags = ul_Flags;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_CopyMatrix, (ULONG) & M, (ULONG) pInv);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_ResetXForm(SEL_tdst_SelectedItem *_pst_Sel, ULONG, ULONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Matrix	*M;
	GEO_tdst_Object		*pst_Geo;
	GEO_Vertex			*V, *VLast;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;

	if(!pst_GO) return TRUE;
	if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return TRUE;
	pst_Geo = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
	if(!pst_Geo) return TRUE;
	if(pst_Geo->st_Id.i->ul_Type != GRO_Geometric) return TRUE;

	if(OBJ_b_IsChild(pst_GO))
		M = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix;
	else
		M = OBJ_pst_GetAbsoluteMatrix(pst_GO);

	V = pst_Geo->dst_Point;
	VLast = V + pst_Geo->l_NbPoints;
	for(; V < VLast; V++) MATH_TransformVector(VCast(V), M, VCast(V));

	GEO_ComputeNormals(pst_Geo);

	MATH_SetIdentity33Matrix(M);
	MATH_ClearScale(M, 1);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_ResetXForm(void)
{
	if(mst_WinHandles.pst_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_ResetXForm, 0, 0);
#ifdef JADEFUSION
	Selection_XenonPostOperation(M_F3D_Sel, TRUE, FALSE);
#endif
	LINK_Refresh();
}
/**/
void F3D_cl_View::GAO_ResetXForm( OBJ_tdst_GameObject *_pst_GO )
{
	SEL_tdst_SelectedItem st_Sel;

	st_Sel.p_Content = (void *) _pst_GO;
	F3D_b_GAO_ResetXForm( &st_Sel, 0, 0 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_ComputeBonePivot(SEL_tdst_SelectedItem *_pst_Sel, ULONG, ULONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *apst_Child[32];
	MATH_tdst_Matrix	*M, Matrix, MTemp, MI;
	GEO_tdst_Object		*pst_Geo;
	MATH_tdst_Vector	C, D, E, F;
	int					i, j, k, l, m, i_NbChild;
	MATH_tdst_Vector	ast_Pt[8], *P;
	GEO_Vertex			*V, *VLast;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;

	if(!pst_GO) return TRUE;
	if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return TRUE;
	pst_Geo = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
	if(!pst_Geo) return TRUE;
	if(pst_Geo->st_Id.i->ul_Type != GRO_Geometric) return TRUE;
	if(pst_Geo->l_NbPoints != 8) return TRUE;

	if(OBJ_b_IsChild(pst_GO))
		M = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix;
	else
		M = OBJ_pst_GetAbsoluteMatrix(pst_GO);

	V = pst_Geo->dst_Point;

	MATH_SetIdentityMatrix(&MTemp);
	MATH_Copy33Matrix(&MTemp, M);
	for(i = 0; i < 8; i++) MATH_TransformVertex(ast_Pt + i, &MTemp, VCast(V + i));

	P = ast_Pt;

	/* find index of vertex of face where the pivot his (Sum of points have to be null) */
	for(i = 0; i < 5; i++)
	{
		for(j = i + 1; j < 6; j++)
		{
			for(k = j + 1; k < 7; k++)
			{
				for(l = k + 1; l < 8; l++)
				{
					MATH_AddVector(&C, P + i, P + j);
					MATH_AddEqualVector(&C, P + k);
					MATH_AddEqualVector(&C, P + l);

					if(MATH_b_NulVectorWithEpsilon(&C, Cf_Epsilon)) goto ComputeBonePivot_FindPivotFace;
				}
			}
		}
	}

	return TRUE;

ComputeBonePivot_FindPivotFace:
	for(m = 0; m < 8; m++) MATH_AddEqualVector(&C, P + m);

	MATH_NormalizeVector(MATH_pst_GetYAxis(&MTemp), &C);

	MATH_AddVector(&C, P + i, P + j);
	if(MATH_b_NulVectorWithEpsilon(&C, Cf_Epsilon))
	{
		MATH_AddVector(&C, P + i, P + k);
		MATH_AddVector(&D, P + i, P + l);
	}
	else
	{
		MATH_AddVector(&D, P + i, P + k);
		if(MATH_b_NulVectorWithEpsilon(&C, Cf_Epsilon)) MATH_AddVector(&D, P + i, P + l);
	}

	MATH_NormalizeEqualVector(&C);
	MATH_NormalizeEqualVector(&D);
	MATH_NegVector(&E, &C);
	MATH_NegVector(&F, &D);

	if(MATH_b_EqVectorWithEpsilon(MATH_pst_GetXAxis(M), &C, Cf_EpsilonBig))
		P = &D;
	else if(MATH_b_EqVectorWithEpsilon(MATH_pst_GetXAxis(M), &D, Cf_EpsilonBig))
		P = &E;
	else if(MATH_b_EqVectorWithEpsilon(MATH_pst_GetXAxis(M), &E, Cf_EpsilonBig))
		P = &F;
	else
		P = &C;

	MATH_CopyVector(MATH_pst_GetXAxis(&MTemp), P);

	MATH_CrossProduct(MATH_pst_GetZAxis(&MTemp), MATH_pst_GetXAxis(&MTemp), MATH_pst_GetYAxis(&MTemp));
	MATH_CrossProduct(MATH_pst_GetXAxis(&MTemp), MATH_pst_GetYAxis(&MTemp), MATH_pst_GetZAxis(&MTemp));

	MATH_NormalizeEqualVector(MATH_pst_GetXAxis(&MTemp));
	MATH_NormalizeEqualVector(MATH_pst_GetZAxis(&MTemp));

	MATH_CopyMatrix(&Matrix, M);
	Matrix.T.x = Matrix.T.y = Matrix.T.z = 0;

	i_NbChild = WOR_i_GetAllChildsOf(pst_GO->pst_World, pst_GO, apst_Child, 0);
	for(m = 0; m < i_NbChild; m++) OBJ_ComputeGlobalWhenHie(apst_Child[m]);

	MATH_Copy33Matrix(M, &MTemp);
	M->Sx = M->Sy = M->Sz = 0;

	OBJ_ComputeGlobalWhenHie(pst_GO);
	OBJ_SetInitialAbsoluteMatrix(pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO));
	for(m = 0; m < i_NbChild; m++)
	{
		OBJ_ComputeLocalWhenHie(apst_Child[m]);
		OBJ_SetInitialAbsoluteMatrix(apst_Child[m], OBJ_pst_GetAbsoluteMatrix(apst_Child[m]));
	}

	MATH_SetIdentity33Matrix(&MI);
	MATH_InvertMatrix(&MI, &MTemp);
	MATH_MulMatrixMatrix(&MTemp, &Matrix, &MI);

	V = pst_Geo->dst_Point;
	VLast = V + pst_Geo->l_NbPoints;
	for(; V < VLast; V++) MATH_TransformVertex(VCast(V), &MTemp, VCast(V));
	GEO_ComputeNormals(pst_Geo);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_ComputeBonePivot(void)
{
	if(mst_WinHandles.pst_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_ComputeBonePivot, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_SetInitialPosition(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				asz_Name[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	OBJ_SetInitialAbsoluteMatrix(pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO));

	sprintf(asz_Name, "%s%s", F3D_STR_Csz_SetAsInitial, pst_GO->sz_Name);
	LINK_PrintStatusMsg(asz_Name);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_SetInitialPosition(void)
{
	if(mst_WinHandles.pst_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_SetInitialPosition, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_RestorePosition(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				asz_Name[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	OBJ_RestoreInitialPos(pst_GO);

	sprintf(asz_Name, "%s%s", F3D_STR_Csz_RestoreInitial, pst_GO->sz_Name);
	LINK_PrintStatusMsg(asz_Name);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_RestorePosition(void)
{
	if(mst_WinHandles.pst_World == NULL) return;

	mpo_CurrentModif = new F3D_cl_Undo_HelperModif(this, FALSE);
	mpo_CurrentModif->SetDesc("Restore initial position");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_RestorePosition, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_GAO_SelectAll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return;
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		ForceSelectObject(pst_GO, FALSE);
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_ViewGizmo(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_ShowGizmo;
	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_ViewGizmo(void)
{
	if(mst_WinHandles.pst_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_ViewGizmo, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_HideGizmo(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
	{
		pst_Father = ANI_pst_GetReference(pst_GO);
		if(pst_Father) pst_Father->ul_EditorFlags &= ~OBJ_C_EditFlags_ShowGizmo;
	}
	else
		pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_ShowGizmo;
	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_HideGizmo(void)
{
	if(mst_WinHandles.pst_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_HideGizmo, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GAO_ToggleGizmo(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowGizmo)
		pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_ShowGizmo;
	else
		pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_ShowGizmo;
	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_ToggleGizmo(void)
{
	if(mst_WinHandles.pst_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_ToggleGizmo, 0, 0);
	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
    Object Functions - Create / duplicate / destroy
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *F3D_cl_View::GAO_CreateInvisible(CPoint point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Current;
	unsigned long		ul_Flags;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayInvisible;
	pst_Current = CreateWaypoint(point);
	ul_Flags = pst_Current->ul_IdentityFlags & (~OBJ_C_IdentityFlag_Waypoints);
	OBJ_ChangeIdentityFlags(pst_Current, ul_Flags, pst_Current->ul_IdentityFlags);
	MATH_InitVector(&v, .5f, .5f, .5f);
	MATH_CopyVector(OBJ_pst_BV_GetGMax(pst_Current->pst_BV), &v);
	MATH_InitVector(&v, -0.5f, -0.5f, -0.5f);
	MATH_CopyVector(OBJ_pst_BV_GetGMin(pst_Current->pst_BV), &v);
	MATH_CopyVector(OBJ_pst_BV_GetGMinInit(pst_Current->pst_BV), OBJ_pst_BV_GetGMin(pst_Current->pst_BV));
	MATH_CopyVector(OBJ_pst_BV_GetGMaxInit(pst_Current->pst_BV), OBJ_pst_BV_GetGMax(pst_Current->pst_BV));
	return pst_Current;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL Destroy_One_Clone(WOR_tdst_World* calbut )
{
/*	OBJ_tdst_GameObject				*pst_GO;
	MDF_tdst_Modifier				st_Mod, *pst_Mod;
	SEL_tdst_Selection		*pst_Sel;*/
	
	/*pst_Sel = calbut->pst_Selection;
	if (!(pst_Sel->pst_FirstItem->l_Flag & SEL_C_SIF_Object))
        return FALSE;

	pst_GO = (OBJ_tdst_GameObject *) pst_Sel->pst_FirstItem->p_Content;

	pst_Mod = MDF_pst_GetByType(pst_GO, MDF_C_Modifier_FClone);

	if(pst_Mod)
	{
		int slip,inc;
		CLONE_Data *v;
		
		FCLONE_tdst_Modifier   *p_FCLONE;
		p_FCLONE = (FCLONE_tdst_Modifier *) pst_Mod->p_Data;
		if ( p_FCLONE->NBR_Instances<2 ) return TRUE;

		// Destruction of the Lohgbow :)
		//slip = FCLONE_Modifier_CurrentSelection();
		v = p_FCLONE->cl_eData;
		for (inc=0;inc<p_FCLONE->NBR_Instances-1;inc++) 
			v++;
		{
			MATH_tdst_Matrix m_temp;
			MATH_ConvertQuaternionToMatrix(&m_temp, &v->qQ);
			m_temp.T = v->vPos;
			*pst_GO->pst_GlobalMatrix = m_temp;
			p_FCLONE->NBR_Instances--;
			FCLONE_Modifier_Update(pst_Mod);
		}
		return TRUE;
	}*/
	
	return FALSE;
}
void F3D_cl_View::Selection_GAO_Destroy(void)
{
	if(mst_WinHandles.pst_World == NULL) return;
	// -- Warning -- destruction du GAO si ya pas modifier Clone
	if ( Destroy_One_Clone(mst_WinHandles.pst_World) ) return;	
	// ---------------------------------------------------------

	if(Selection_b_IsInSubObjectMode())
	{
		if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Vertex)
			Selection_SubObject_DelVertex();
		else if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Face)
			Selection_SubObject_DelFace();
	}
	else
	{
		mpo_CurrentModif = new F3D_cl_Undo_Destruction(this);
		mpo_CurrentModif->SetDesc("Object destruction");
		mo_UndoManager.b_AskFor(mpo_CurrentModif);
		mpo_CurrentModif = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

BOOL DuplicateOn = FALSE;
#ifdef JADEFUSION
BOOL						b_IsCancel = FALSE;
SEL_tdst_Selection			*pst_DupSel;
SEL_tdst_SelectedItem		*pst_DupItem;
int							i_Count = 0;
int							i_TypeofDuplication = -1;

#define						FILEEXT_TO_CHECK	5
char						*asz_FileExt[FILEEXT_TO_CHECK] =
{
							EDI_Csz_ExtGameObject,
							EDI_Csz_ExtGameObjectRLI,
							EDI_Csz_ExtAIEngineVars,
							EDI_Csz_ExtAIEngineInstance,
							EDI_Csz_ExtCOLMap
};
#else
struct OBJ_tdst_GameObject_ *pst_Duplicate_GAO = NULL;
#endif
BOOL F3D_b_GAO_Duplicate(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
	OBJ_tdst_GameObject			*pst_GO, *pst_NewGO, *pst_TempGO;
	BIG_INDEX					ul_Index, ul_DirIndex, ul_ParentDirIndex;
#else
	OBJ_tdst_GameObject			*pst_GO, *pst_NewGO;
	BIG_INDEX					ul_Index;
#endif
	MATH_tdst_Vector			v, *pst_Move;
	SEL_tdst_Selection			*pst_Selection;
	F3D_cl_View					*po_View;
	F3D_tdst_DuplicateParams	*pst_DP;
	unsigned long				ul_Msg;
	int							i_Index;
	OBJ_tdst_GameObject			*pst_Father;
#ifdef JADEFUSION
	char						asz_NewName[BIG_C_MaxLenName];
	WOR_tdst_World				*pst_world;
	ULONG						ul_Res;
	CString						strName;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(ANI_b_IsGizmoAnim(pst_GO, &pst_Father)) return FALSE;
	pst_DP = (F3D_tdst_DuplicateParams *) _ul_Param2;
	i_Index = pst_DP->i_Current++;
	pst_DP->pst_Src[i_Index] = pst_GO;

	if(!pst_GO) return TRUE;

	pst_Move = (MATH_tdst_Vector *) _ul_Param1;
	po_View = pst_DP->po_View;

#ifdef JADEFUSION
	if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_DuplicateExt))
	{
		if(!DuplicateOn)
		{
			if(pst_DupSel)
				pst_DupItem = SEL_pst_GetFirst(pst_DupSel, SEL_C_SIF_Object);
			for(int i = 0; i < SEL_l_CountItem(pst_DupSel, SEL_C_SIF_Object) - i_Count; i++)
				pst_DupItem = SEL_pst_GetNext(pst_DupSel, pst_DupItem, SEL_C_SIF_Object);
		}
		ul_Index = LOA_ul_SearchKeyWithAddress( DuplicateOn ? (ULONG)pst_GO:(ULONG)pst_DupItem->p_Content);
	}
	else
#endif
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);

	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

#ifdef JADEFUSION
	if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_DuplicateExt))
#endif
	pst_NewGO = OBJ_pst_DuplicateGameObjectFile(pst_GO->pst_World, ul_Index, 0, NULL, NULL);
#ifdef JADEFUSION
	else
	{
	if(DuplicateOn)	//on duplique un GAO temporaire
	{
			//init la selection temp si pas déjà fait
			if(!pst_DupSel)	//si c'est vide
			{
				pst_DupSel = (SEL_tdst_Selection *) MEM_p_Alloc(sizeof(SEL_tdst_Selection));
				SEL_Duplicate(pst_DupSel, po_View->mst_WinHandles.pst_World->pst_Selection);
			}

		pst_TempGO = OBJ_pst_DuplicateGameObjectFile(pst_GO->pst_World, ul_Index, 0, "GAOtemp.gao",NULL);
		
		// Replace the parent GAO with the temp GAO in the selection structure
		// and unselect parent GAO
		pst_Selection = po_View->mst_WinHandles.pst_World->pst_Selection;
		SEL_ReplaceItemContent(pst_Selection, pst_GO, pst_TempGO);
		pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;
			
			//incéremente le counter
			i_Count++;
			return TRUE;
	}

		pst_TempGO = pst_GO;
		//doit retrouver le bon gao source dans la selection temp
		if(pst_DupSel)
			pst_DupItem = SEL_pst_GetFirst(pst_DupSel, SEL_C_SIF_Object);
		for(int i = 0; i < SEL_l_CountItem(pst_DupSel, SEL_C_SIF_Object) - i_Count; i++)
			pst_DupItem = SEL_pst_GetNext(pst_DupSel, pst_DupItem, SEL_C_SIF_Object);
		pst_GO = (OBJ_tdst_GameObject *)pst_DupItem->p_Content;
		//pst_GO = pst_Duplicate_GAO;
		if(!pst_GO) return FALSE;

		//décrémente le counter
		i_Count--;

		// Show the dialog
		EDIA_cl_NameDialogDuplicate o_Dialog(po_View, pst_GO);

		if(i_TypeofDuplication != -1)
			o_Dialog.m_iTypeOfDuplication = i_TypeofDuplication;

		ul_Res =  b_IsCancel ? IDCANCEL : o_Dialog.DoModal();
		if(ul_Res == IDOK)
		{
			//keep trace of type of duplication
			i_TypeofDuplication = o_Dialog.m_iTypeOfDuplication;

			//add GAO extension
			if(o_Dialog.m_strGaoName.Find(EDI_Csz_ExtGameObject) == -1) 
				sprintf(asz_NewName, "%s%s", (LPSTR)(LPCSTR) o_Dialog.m_strGaoName, EDI_Csz_ExtGameObject);
			else
				sprintf(asz_NewName, "%s", (LPSTR)(LPCSTR) o_Dialog.m_strGaoName);

			//Duplicate GAO
			pst_NewGO = OBJ_pst_DuplicateGameObjectFile(pst_GO->pst_World, ul_Index, 0, asz_NewName,NULL);

			// Replace the parent GAO with the new GAO in the selection structure
			// and unselect parent GAO
			pst_Selection = po_View->mst_WinHandles.pst_World->pst_Selection;
			SEL_ReplaceItemContent(pst_Selection, pst_TempGO, pst_NewGO);
			pst_TempGO->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;
			
			//************************
			//code provenant du dialog->OnNbClickedOk()

			// Verify format of GAO name 
			if(BIG_b_CheckName((LPSTR)(LPCSTR) o_Dialog.m_strGaoName))
			{
				// If COPY was asked, copy each element of GAO & propose new name (except for materials)
				if ( !o_Dialog.m_iTypeOfDuplication )	
				{
					// COPY lights
					if ( pst_NewGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)
						F3D_b_Selection_DuplicateGro( SEL_RetrieveItem(pst_Selection, (void *) pst_NewGO), F3D_Duplicate_Light | F3D_Duplicate_AllLocation, 0);

					if ( pst_NewGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu )
					{
						// COPY geometry
						if ( pst_NewGO->pst_Base->pst_Visu->pst_Object->i->ul_Type ==	GRO_Geometric )			
							F3D_b_Selection_DuplicateGro( SEL_RetrieveItem(pst_Selection, (void *) pst_NewGO), F3D_Duplicate_Geometry | F3D_Duplicate_AllLocation, 0);

						// COPY particle generator
						if ( pst_NewGO->pst_Base->pst_Visu->pst_Object->i->ul_Type ==	GRO_ParticleGenerator )			
							F3D_b_Selection_DuplicateGro( SEL_RetrieveItem(pst_Selection, (void *) pst_NewGO), F3D_Duplicate_Geometry | F3D_Duplicate_AllLocation, 0);
					}

					//Copy Cob
					if (pst_NewGO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap )
					{
						COL_tdst_ColMap *pst_ColMap = ((COL_tdst_Base *) pst_NewGO->pst_Extended->pst_Col)->pst_ColMap;
						if(pst_ColMap)
						{
							if(pst_ColMap->uc_NbOfCob)
							{
								for(int i = 0; i < pst_ColMap->uc_NbOfCob; i++)
								{
									if(!pst_ColMap->dpst_Cob[i]) continue;
									COL_tdst_Cob *pst_Cob = pst_ColMap->dpst_Cob[i];
									po_View->Selection_DuplicateCob(i, pst_NewGO);
								}
							}
						}
					}
				}

				// Rename GAO if different
				if ( stricmp(pst_NewGO->sz_Name, asz_NewName) != 0 )
					OBJ_GameObject_Rename(pst_NewGO, asz_NewName);
			}
			//code provenant du dialog
			//************************
#endif
	/* Move a little new object */
#ifdef JADEFUSION
		MATH_CopyVector(&v, OBJ_pst_GetAbsolutePosition(pst_TempGO));
#else
		MATH_CopyVector(&v, OBJ_pst_GetAbsolutePosition(pst_GO));
#endif
	if(pst_Move) MATH_AddEqualVector(&v, pst_Move);

	if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_HasInitialPos))
		MATH_SetTranslation(pst_NewGO->pst_GlobalMatrix + 1, &v);
	MATH_SetTranslation(pst_NewGO->pst_GlobalMatrix, &v);

#ifdef JADEFUSION
#if defined(_XENON_RENDER)
            if (pst_NewGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
            {
                if (pst_NewGO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
                {
                    GEO_ResetXenonMesh(pst_NewGO,
                                       pst_NewGO->pst_Base->pst_Visu,
                                       (GEO_tdst_Object*)pst_NewGO->pst_Base->pst_Visu->pst_Object,
                                       FALSE, TRUE, FALSE);
                }
            }
#endif
		}
		}

	if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_DuplicateExt))
	{
		//on efface le GAO temporaire
		pst_world = pst_TempGO->pst_World;
		if(pst_world)
		{
			OBJ_WhenDestroyAnObject(pst_world, pst_TempGO);
			WOR_World_DetachObject(pst_world, pst_TempGO);
			INT_SnP_DetachObject(pst_TempGO, pst_world);
			OBJ_ul_GameObject_Save(pst_world, pst_TempGO, NULL);
			OBJ_GameObject_Remove(pst_TempGO, 1);
			
			//find index of the base dir of the world
			ul_Index =  LOA_ul_SearchIndexWithAddress((ULONG) pst_world);
			if(ul_Index != BIG_C_InvalidIndex)
			{
				ul_DirIndex = BIG_ParentFile(ul_Index);
			}
			
			if(ul_DirIndex != BIG_C_InvalidIndex)
			{	
				for(int i = 0; i < FILEEXT_TO_CHECK; i++)
				{
					strName = "GAOtemp";
					strName += asz_FileExt[i];
					do
					{
						ul_Index = BIG_ul_SearchFileInDirRec(ul_DirIndex, (char *)(LPCSTR) strName);
						if(ul_Index != BIG_C_InvalidIndex)
						{
							//get index in fat of the parent dir
							ul_ParentDirIndex = BIG_ParentFile(ul_Index);
							if(ul_ParentDirIndex != BIG_C_InvalidIndex)
							{
								BIG_DeleteFile(ul_ParentDirIndex, ul_Index);
							}
						}
					}
					while(ul_Index != BIG_C_InvalidIndex);
				}
			}
		}

		if(ul_Res != IDOK)
		{
			b_IsCancel = !i_Count ? FALSE : TRUE;
			//swap des items des listes de sélection
			if(i_Count > 0)
			{
				pst_DupItem = SEL_pst_GetFirst(pst_DupSel, SEL_C_SIF_Object);
				for(int i = 0; i < SEL_l_CountItem(pst_DupSel, SEL_C_SIF_Object) - i_Count; i++)
					pst_DupItem = SEL_pst_GetNext(pst_DupSel, pst_DupItem, SEL_C_SIF_Object);

				pst_Selection = po_View->mst_WinHandles.pst_World->pst_Selection;
				SEL_ReplaceItemContent(pst_Selection, pst_TempGO, (OBJ_tdst_GameObject *)pst_DupItem->p_Content);
				pst_TempGO->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;
			}
			else
			{
				MEM_Free(pst_DupSel);
				pst_DupSel = NULL;
				pst_Selection = po_View->mst_WinHandles.pst_World->pst_Selection;
				SEL_Close(pst_Selection);
				return FALSE;
			}
			return TRUE;
		}

		if(!i_Count)	// terminer le travail sur la sélection, efface la liste temp
		{
			SEL_DelItems(pst_DupSel, (void *) BIG_C_InvalidIndex, SEL_C_SIF_Object);
			MEM_Free(pst_DupSel);
			pst_DupSel = NULL;
		}
	}
#endif

	/*
	 * OBJ_SetInitialAbsolutePosition(pst_NewGO, &v); £
	 * OBJ_RestoreInitialPos(pst_NewGO);
	 */
	pst_DP->pst_Dup[i_Index] = pst_NewGO;
	if(po_View)
	{
		/*~~~~~~~~~~~~~*/
		BOOL	bLastRef;
		/*~~~~~~~~~~~~~*/
#ifdef JADEFUSION
		if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_DuplicateExt))
#endif
		{
		pst_Selection = po_View->mst_WinHandles.pst_World->pst_Selection;
		SEL_ReplaceItemContent(pst_Selection, pst_GO, pst_NewGO);
		pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;
		}
		bLastRef = LINK_gb_AllRefreshEnable;
		LINK_gb_AllRefreshEnable = TRUE;
		M_MF()->SendMessageToLinks(po_View->mpo_AssociatedEditor, EDI_MESSAGE_DELSELDATA, 0, (ULONG) pst_GO);
		pst_NewGO->ul_EditorFlags |= OBJ_C_EditFlags_Selected;

		if(pst_NewGO == po_View->Selection_pst_GetFirstObject())
			ul_Msg = EDI_MESSAGE_SELDATA;
		else
			ul_Msg = EDI_MESSAGE_ADDSELDATA;
		M_MF()->SendMessageToLinks(po_View->mpo_AssociatedEditor, ul_Msg, 0, (ULONG) pst_NewGO);
		LINK_gb_AllRefreshEnable = bLastRef;

		if(po_View->mpo_ToolBoxDialog)
		{
			if(po_View->mpo_ToolBoxDialog->mpst_Gao == pst_GO)
				po_View->mpo_ToolBoxDialog->SelectGaoFromPtr(pst_NewGO);
		}
	}
	
	/* Show the dialog */
	if ( ((EOUT_cl_Frame *) pst_DP->po_View->mpo_AssociatedEditor)->mst_Ini.c_DuplicateExtended )
	{
		EDIA_cl_NameDialogDuplicate o_Dialog(po_View, pst_NewGO);
		if(o_Dialog.DoModal() == IDOK)
		{
		}
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_Duplicate(MATH_tdst_Vector *_pst_Move, BOOL _b_SwapSel,OBJ_tdst_GameObject ***_ppp_SrcNodes,OBJ_tdst_GameObject ***_ppp_DstNodes,int *_pi_LinkNb)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_tdst_DuplicateParams	st_DP;
	OBJ_tdst_GameObject			*pst_Father;
	int							i;
	int							j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//pour que la fonction F3D_GAO_Duplicate connaisse l'attribut membre de la View
	DuplicateOn = mb_DuplicateOn;

	if(mst_WinHandles.pst_World == NULL) return;

	st_DP.po_View = this;
	st_DP.i_Count = SEL_l_CountItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	st_DP.pst_Src = (OBJ_tdst_GameObject **) L_malloc(st_DP.i_Count * 4);
	st_DP.pst_Dup = (OBJ_tdst_GameObject **) L_malloc(st_DP.i_Count * 4);
	st_DP.i_Current = 0;

	LINK_gb_AllRefreshEnable = FALSE;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GAO_Duplicate, (ULONG) _pst_Move, (ULONG) & st_DP);
	LINK_gb_AllRefreshEnable = TRUE;

	/* Change hierarchy links */
	for(i = 0; i < st_DP.i_Current; i++)
	{
		if(st_DP.pst_Src[i]->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		{
			pst_Father = st_DP.pst_Src[i]->pst_Base->pst_Hierarchy->pst_Father;
			for(j = 0; j < st_DP.i_Current; j++)
			{
				if(st_DP.pst_Src[j] == pst_Father)
					st_DP.pst_Dup[i]->pst_Base->pst_Hierarchy->pst_Father = st_DP.pst_Dup[j];
			}
		}
	}

    if (_pi_LinkNb && _ppp_SrcNodes && _ppp_DstNodes)
    {
        // Keep pointers to selected and copied waypoints.
        *_ppp_SrcNodes = st_DP.pst_Src;
        *_ppp_DstNodes = st_DP.pst_Dup;
        *_pi_LinkNb = st_DP.i_Count;
    }
    else
    {
    	L_free(st_DP.pst_Src);
	    L_free(st_DP.pst_Dup);
    }

	/* LINK_FatHasChanged(); */
	LINK_Refresh();
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_CLONE_Duplicate()
{
/*	OBJ_tdst_GameObject				*pst_GO;
	MDF_tdst_Modifier				st_Mod, *pst_Mod;
	SEL_tdst_Selection		*pst_Sel;
	//SEL_tdst_SelectedItem	*pst_SelItem;
	//TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	
	pst_Sel = M_F3D_Sel;
	pst_GO = (OBJ_tdst_GameObject *) pst_Sel->pst_FirstItem->p_Content;

	pst_Mod = MDF_pst_GetByType(pst_GO, MDF_C_Modifier_FClone);
	
	if(!pst_Mod)
	{
		M_MF()->MessageBox("Object must have FClone modifier !!", "Error", MB_OK | MB_ICONSTOP);
		return;
	}
	else 
	{
		FCLONE_tdst_Modifier   *p_FCLONE;

		p_FCLONE = (FCLONE_tdst_Modifier *) pst_Mod->p_Data;
		p_FCLONE->NBR_Instances++;
		FCLONE_Modifier_Update(pst_Mod);

		{
			int inc;
			CLONE_Data *v;

			v = p_FCLONE->cl_eData;
			for (inc=0;inc<p_FCLONE->NBR_Instances-1;inc++) 
				v++;
			v->vPos = pst_GO->pst_GlobalMatrix->T;
			MATH_ConvertMatrixToQuaternion(&v->qQ,pst_GO->pst_GlobalMatrix);
		}
	}
	LINK_Refresh();*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Hide(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Mask, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
        pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
    else
        pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;

	if(!pst_GO) return TRUE;

	pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_Hidden;

	return TRUE;
}
/**/
void F3D_cl_View::Selection_Hide(void)
{
	if(mst_WinHandles.pst_World == NULL) return;
	SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object|SEL_C_SIF_Cob, F3D_b_Hide, 0, 0);

	LINK_Refresh();
}

MATH_tdst_Vector	gst_VecOffset;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_OffsetObj(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Mask, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	MATH_AddVector(OBJ_pst_GetAbsolutePosition(pst_GO), OBJ_pst_GetAbsolutePosition(pst_GO), &gst_VecOffset);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_GAO_OffsetObjects(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_VectorDialog	o_Dlg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_Dlg.mst_Vector.x = 0;
	o_Dlg.mst_Vector.y = 0;
	o_Dlg.mst_Vector.z = 0;
	o_Dlg.DoModal();
	MATH_CopyVector(&gst_VecOffset, &o_Dlg.mst_Vector);
	SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_OffsetObj, 0, 0);
	LINK_Refresh();
}
#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
typedef struct OBJ_tdst_GaoClipboard_
{
	OBJ_tdst_GameObject*	pst_Gao;
	GRO_tdst_Struct*		pst_Mat;
	ULONG					ul_GaoFlags;
	ULONG					ul_DrawMask;
	ULONG					ul_XMPFlags;
} 
OBJ_tdst_GaoClipboard;

OBJ_tdst_GaoClipboard st_GaoClipboard;

void F3D_ResetGaoClipboard(void)
{
	st_GaoClipboard.pst_Gao		= NULL;
	st_GaoClipboard.pst_Mat		= NULL;
	st_GaoClipboard.ul_GaoFlags = 0;
	st_GaoClipboard.ul_DrawMask	= 0;
	st_GaoClipboard.ul_XMPFlags = 0;
}

BOOL F3D_b_PropertiesObj(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GAO;
	BOOL				b_PasteGaoProperties;
	BOOL				*pb;
	char				sz_Msg[256];
	BIG_INDEX			ul_Index;
	F3D_cl_View			*po_View;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pb = (BOOL*) _ul_Param1;
	b_PasteGaoProperties = pb[F3D_UPDATE_NUM];

	po_View = (F3D_cl_View*) _ul_Param2;

	if(b_PasteGaoProperties)
	{
		pst_GAO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;

		//test for Gao
		if(!pst_GAO)
			return TRUE;
		
		if(pb[F3D_UPDATE_FLAGS])
		{
			pst_GAO->ul_StatusAndControlFlags = st_GaoClipboard.ul_GaoFlags;
		}

		//test for visu
		if(
			OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Visu)
			|| pst_GAO->pst_Base
			|| pst_GAO->pst_Base->pst_Visu
		  ) 
		{
			if(pb[F3D_UPDATE_FLAGS])
			{
				pst_GAO->pst_Base->pst_Visu->ul_DrawMask				 = st_GaoClipboard.ul_DrawMask;
				pst_GAO->pst_Base->pst_Visu->ul_XenonMeshProcessingFlags = st_GaoClipboard.ul_XMPFlags;
			}

			if(pb[F3D_UPDATE_LRL])
			{
				LRL_DuplicateList(st_GaoClipboard.pst_Gao, pst_GAO);
			}

			//test for Material
			if(pst_GAO->pst_Base->pst_Visu->pst_Material)
			{
				if(pb[F3D_UPDATE_MTL])
				{
					ul_Index = LOA_ul_SearchIndexWithAddress((ULONG)st_GaoClipboard.pst_Mat);

					if(ul_Index != BIG_C_InvalidIndex)
					{
						po_View->AssignMaterial(pst_GAO, ul_Index);
					}
				}
			}
			else
			{
				sprintf(sz_Msg, "[%08x] - %s has no material.\n", pst_GAO->ul_MyKey, pst_GAO->sz_Name);
				LINK_PrintStatusMsg(sz_Msg);
			}
		}
		else
		{
			sprintf(sz_Msg, "[%08x] - %s has no visu.\n", pst_GAO->ul_MyKey, pst_GAO->sz_Name);
			LINK_PrintStatusMsg(sz_Msg);
		}
	}
	else	//copy
	{
		pst_GAO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;

		//test for Gao
		if(!pst_GAO)
		{
			F3D_ResetGaoClipboard();

			ERR_X_Warning(0, "You must select a Gao first !", NULL);
			return FALSE;
		}

		//test for Visu
		if(
			OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Visu)
			|| pst_GAO->pst_Base
			|| pst_GAO->pst_Base->pst_Visu
		  ) 
		{
			st_GaoClipboard.pst_Gao	= pst_GAO;
			st_GaoClipboard.ul_GaoFlags = pst_GAO->ul_StatusAndControlFlags;

			st_GaoClipboard.ul_DrawMask = pst_GAO->pst_Base->pst_Visu->ul_DrawMask;
			st_GaoClipboard.ul_XMPFlags = pst_GAO->pst_Base->pst_Visu->ul_XenonMeshProcessingFlags;

			//test for Material
			if(pst_GAO->pst_Base->pst_Visu->pst_Material)
			{
				st_GaoClipboard.pst_Mat = pst_GAO->pst_Base->pst_Visu->pst_Material;
			}
			else
			{
				st_GaoClipboard.pst_Mat = NULL;

				sprintf(sz_Msg, "[%08x] - %s has no material, only LRL, Gao and Gro flags were copied.\n", pst_GAO->ul_MyKey, pst_GAO->sz_Name);
				LINK_PrintStatusMsg(sz_Msg);
			}
		}
		else
		{
			st_GaoClipboard.ul_DrawMask	= 0;
			st_GaoClipboard.ul_XMPFlags	= 0;
			st_GaoClipboard.pst_Mat		= NULL;

			sprintf(sz_Msg, "[%08x] - %s has no visu, only Gao flags were copied.\n", pst_GAO->ul_MyKey, pst_GAO->sz_Name);
			LINK_PrintStatusMsg(sz_Msg);
		}
	}

	return TRUE;
}
/**/
BOOL F3D_cl_View::Selection_GAO_CopyProperties(BOOL _b_PasteGaoProperties, CRect* _po_Rect)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CCheckList			o_Check;
	BOOL				pb[F3D_UPDATE_NUM + 1];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(int i = 0; i < F3D_UPDATE_NUM; ++i)
	{
		pb[i] = FALSE;
	}

	//add "_b_PasteGaoProperties" to the end of bool array to free the last param for f3dview pointer
	pb[F3D_UPDATE_NUM] = _b_PasteGaoProperties;

	if(_b_PasteGaoProperties)
	{
		//*************
		//update what ?
		o_Check.AddString("Materials");
		o_Check.AddString("Flags (DrawMask & XMP)");
		o_Check.AddString("Light Rejection List");

		_po_Rect->left += (_po_Rect->Width() / 2) - 100;
		_po_Rect->top += (_po_Rect->Height() / 2) - 100;	
		o_Check.SetArrayBool( pb );
		o_Check.Do( _po_Rect, 200, this, 20, "Update...");
		//*************

		if(!SEL_b_IsEmpty(mst_WinHandles.pst_World->pst_Selection))
			SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_PropertiesObj, (ULONG) pb, (ULONG) this);
		else
			return FALSE;
	}
	else
	{
		if(!SEL_b_IsEmpty(mst_WinHandles.pst_World->pst_Selection) && (SEL_l_CountItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object) == 1))
			SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_PropertiesObj, (ULONG) pb, (ULONG) this);
		else
			return FALSE;
	}

	LINK_Refresh();
	return TRUE;
}

#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_HideAll(BOOL _b_Refresh)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return;
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_Hidden;
		pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_HiddenCob;
	}

	if(_b_Refresh) LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_UnhideAll(BOOL _b_Refresh)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return;
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if (!(pst_GO->ul_AdditionalFlags & OBJ_C_EdAddFlags_HiddenByWowFilter))
		{
			pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Hidden;
			pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_HiddenCob;
		}
	}

	if(_b_Refresh) LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_HideUnsel(void)
{
	Selection_HideAll(FALSE);
	Selection_Display();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Display(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Mask, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
		}
	}

	if(!pst_GO) return TRUE;

	pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Hidden;
	pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_HiddenCob;

	return TRUE;
}
/**/
void F3D_cl_View::Selection_Display(void)
{
	if(mst_WinHandles.pst_World == NULL) return;
	SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Display, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::ChangeEditFlags(BOOL _b_OnlySel, ULONG _ul_Add, ULONG _ul_Remove)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return;
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	if(_b_OnlySel)
	{
		for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			if(!(pst_GO->ul_EditorFlags | OBJ_C_EditFlags_Selected)) continue;
			pst_GO->ul_EditorFlags |= _ul_Add;
			pst_GO->ul_EditorFlags &= ~_ul_Remove;
		}
	}
	else
	{
		for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			pst_GO->ul_EditorFlags |= _ul_Add;
			pst_GO->ul_EditorFlags &= ~_ul_Remove;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Dump(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Mask, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				sz_Text[256];
	OBJ_tdst_GameObject *pst_GO;
	GRO_tdst_Struct		*pst_Gro;
	GEO_tdst_Object		*pst_Geo;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;

	sprintf(sz_Text, "Name : %s", pst_GO->sz_Name);
	LINK_PrintStatusMsg(sz_Text);

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
	{
		pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Material;
		if(pst_Gro)
		{
			sprintf
			(
				sz_Text,
				". Mat-> %s / %s",
				GRO_gasz_InterfaceName[pst_Gro->i->ul_Type],
				GRO_sz_Struct_GetName(pst_Gro)
			);
			LINK_PrintStatusMsg(sz_Text);
		}

		pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
		if(pst_Gro)
		{
			sprintf
			(
				sz_Text,
				". Geo-> %s / %s",
				GRO_gasz_InterfaceName[pst_Gro->i->ul_Type],
				GRO_sz_Struct_GetName(pst_Gro)
			);
			LINK_PrintStatusMsg(sz_Text);
			if(pst_Gro->i->ul_Type == GRO_Geometric)
			{
				pst_Geo = (GEO_tdst_Object *) pst_Gro;

				sprintf(sz_Text, ".    NbPoints = %d", pst_Geo->l_NbPoints);
				LINK_PrintStatusMsg(sz_Text);
				sprintf(sz_Text, ".    NbUVs    = %d", pst_Geo->l_NbUVs);
				LINK_PrintStatusMsg(sz_Text);
				sprintf(sz_Text, ".    NbElems  = %d", pst_Geo->l_NbElements);
				LINK_PrintStatusMsg(sz_Text);

				sprintf(sz_Text, ".    <Points>");
				LINK_PrintStatusMsg(sz_Text);
				for(i = 0; i < pst_Geo->l_NbPoints; i++)
				{
					sprintf
					(
						sz_Text,
						".     {%.4f, %.4f, %.4f}",
						pst_Geo->dst_Point[i].x,
						pst_Geo->dst_Point[i].y,
						pst_Geo->dst_Point[i].z
					);
					LINK_PrintStatusMsg(sz_Text);
				}

				sprintf(sz_Text, ".    <UVs>");
				LINK_PrintStatusMsg(sz_Text);
				for(i = 0; i < pst_Geo->l_NbUVs; i++)
				{
					sprintf(sz_Text, ".     {%.4f, %.4f}", pst_Geo->dst_UV[i].fU, pst_Geo->dst_UV[i].fV);
					LINK_PrintStatusMsg(sz_Text);
				}

				for(i = 0; i < pst_Geo->l_NbElements; i++)
				{
					sprintf(sz_Text, ".    <Element %d> %d triangles", i, pst_Geo->dst_Element[i].l_NbTriangles);
					LINK_PrintStatusMsg(sz_Text);

					for(j = 0; j < pst_Geo->dst_Element[i].l_NbTriangles; j++)
					{
						sprintf
						(
							sz_Text,
							".     {%d, %d, %d} {%d, %d, %d}",
							pst_Geo->dst_Element[i].dst_Triangle[j].auw_Index[0],
							pst_Geo->dst_Element[i].dst_Triangle[j].auw_Index[1],
							pst_Geo->dst_Element[i].dst_Triangle[j].auw_Index[2],
							pst_Geo->dst_Element[i].dst_Triangle[j].auw_UV[0],
							pst_Geo->dst_Element[i].dst_Triangle[j].auw_UV[1],
							pst_Geo->dst_Element[i].dst_Triangle[j].auw_UV[2]
						);
						LINK_PrintStatusMsg(sz_Text);
					}
				}
			}
		}
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_Dump(void)
{
	if(mst_WinHandles.pst_World == NULL) return;
	LINK_PrintStatusMsg("<-- Dump selection information -->");
	SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_Dump, 0, 0);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::RenameObj(OBJ_tdst_GameObject *pst_GO, char *asz_Name)
{
	OBJ_GameObject_Rename(pst_GO, asz_Name);
	
#if old
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX			ul_Index;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Name1[BIG_C_MaxLenPath];
	LINK_tdst_Pointer	*p2;
	BOOL				err;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OBJ_GameObject_Rename(pst_GO, asz_Name);
	return;

	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

	/* Rename file */
	err = FALSE;
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	_Try_	BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(ul_Index));
	_Catch_ err = TRUE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_End_ if(err) return;

	/* Name of GO */
	MEM_Free(pst_GO->sz_Name);
	pst_GO->sz_Name = (char *) MEM_p_Alloc(L_strlen(asz_Name) + 1);
	L_strcpy(pst_GO->sz_Name, asz_Name);

	/* Update GO */
	OBJ_ul_GameObject_Save(mst_WinHandles.pst_World, pst_GO, NULL);
	p2 = LINK_p_SearchPointer(pst_GO);
	ERR_X_Assert(p2);
	L_strcpy(p2->asz_Name, asz_Name);
	LINK_UpdatePointer(pst_GO);

	/* Rename AI */
	if(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Ai)
	{
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO->pst_Extended->pst_Ai);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
		L_strcpy(asz_Name1, BIG_NameFile(ul_Index));

		/* AI instance */
		*L_strrchr(asz_Name, '.') = 0;
		L_strcat(asz_Name, EDI_Csz_ExtAIEngineInstance);
		BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		_Try_			BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(ul_Index));
		_Catch_ _End_	p2 = LINK_p_SearchPointer(pst_GO->pst_Extended->pst_Ai);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ERR_X_Assert(p2);
		L_strcpy(p2->asz_Name, asz_Name);
		LINK_UpdatePointer(pst_GO->pst_Extended->pst_Ai);

		/* AI vars */
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO->pst_Extended->pst_Ai);
		*L_strrchr(asz_Name1, '.') = 0;
		L_strcat(asz_Name1, EDI_Csz_ExtAIEngineVars);
		ul_Index = BIG_ul_SearchFileExt(asz_Path, asz_Name1);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			_Try_	*L_strrchr(asz_Name, '.') = 0;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			L_strcat(asz_Name, EDI_Csz_ExtAIEngineVars);
			BIG_RenFile(asz_Name, asz_Path, asz_Name1);
			_Catch_ _End_
		}
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Rename(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO;
	//EDIA_cl_NameDialog		o_Dialog(F3D_STR_Csz_NewName);
	BIG_INDEX				ul_Index;
	char					asz_Name[BIG_C_MaxLenPath];
	char					asz_Name1[BIG_C_MaxLenPath];
	char					*psz_Temp;
	SEL_tdst_SelectedItem	*pst_Item;
	int						i_Num;
#ifdef JADEFUSION
	int						i_Res;
	CString					po_Name;
	EDIA_cl_NameDialog		o_Dialog(F3D_STR_Csz_NewName);
#endif
	char					asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Item = mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);

#ifdef JADEFUSION
	if(pst_GO == NULL || !OBJ_IsValidGAO(pst_GO)) return;
#else
	if(!pst_GO) return;
#endif
	/* Get index of corresponding file */
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

	L_strcpy(asz_Name, BIG_NameFile(ul_Index));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;

#ifdef JADEFUSION
	EDIA_cl_NameDialogCompletion o_DialogCompletion(M_F3D_World, DLG_WAYPOINT, pst_GO);

	if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_AdvanceRename))
	{
		i_Res = o_DialogCompletion.DoModal();
	}
	else
		{
		o_Dialog.mo_Name = asz_Name;
		i_Res = o_Dialog.DoModal();
	}
	
	if(i_Res == IDOK)
#else
	EDIA_cl_NameDialogCompletion o_Dialog(M_F3D_World, DLG_WAYPOINT, pst_GO);
//	o_Dialog.mo_Name = asz_Name;
	if(o_Dialog.DoModal() == IDOK)
#endif
	{
#ifdef JADEFUSION
		if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_AdvanceRename))
			po_Name = o_DialogCompletion.m_strName;
		else
			po_Name = o_Dialog.mo_Name;
		if(L_strcmp(asz_Name, (char *) (LPCSTR) po_Name))
#else
		if(L_strcmp(asz_Name, (char *) (LPCSTR) o_Dialog.m_strName))
#endif
		{
#ifdef JADEFUSION
			if(BIG_b_CheckName((char *) (LPCSTR) po_Name) == FALSE) return;
			L_strcpy(asz_Name, (char *) (LPCSTR) po_Name);
#else
			if(BIG_b_CheckName((char *) (LPCSTR) o_Dialog.m_strName) == FALSE) return;
			L_strcpy(asz_Name, (char *) (LPCSTR) o_Dialog.m_strName);
#endif
			/* Rename a single item */
			if(!pst_Item->pst_Next)
			{
				sprintf(asz_Name1, "%s%s", asz_Name, EDI_Csz_ExtGameObject);
				RenameObj(pst_GO, asz_Name1);
			}

			/* Rename all items with a number */
			else
			{
#ifdef JADEFUSION
				i_Num = 1;
#else
				i_Num = 0;
#endif
				BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
				while(pst_Item)
				{
					sprintf(asz_Name1, "%s%02d%s", asz_Name, i_Num++, EDI_Csz_ExtGameObject);
					while(BIG_ul_SearchFileExt(asz_Path, asz_Name1) != BIG_C_InvalidIndex)
						sprintf(asz_Name1, "%s%02d%s", asz_Name, i_Num++, EDI_Csz_ExtGameObject);
					RenameObj((OBJ_tdst_GameObject *) pst_Item->p_Content, asz_Name1);
					pst_Item = pst_Item->pst_Next;
				}
			}

			/* Refresh */
			LINK_UpdatePointers();
			M_MF()->FatHasChanged();
			LINK_Refresh();
			if(mpo_AssociatedEditor) mpo_AssociatedEditor->RefreshMenu();
		}
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

OBJ_tdst_GameObject *gpo_DroppedGO = NULL;
ULONG				gul_PrefabKey = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropGameObject(EDI_tdst_DragDrop *_pst_DragDrop,BOOL _bDoPick)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v;
	GDI_tdst_Device		*pst_Dev;
	CAM_tdst_Camera		*pst_Cam;
	GEO_tdst_Object		*pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Pick to know pos */
	if (_bDoPick)
	{
		pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
		ScreenToClient(&_pst_DragDrop->o_Pt);
		v.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
		v.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
		b_PickDepth(&v, &v.z);
		pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
		v.x = (float) _pst_DragDrop->o_Pt.x;
		v.y = (float) _pst_DragDrop->o_Pt.y;
		CAM_2Dto3D(pst_Cam, &v, &v);
	}
	else
	{
		v.x = 0;
		v.y = 0;
		v.z = 0;
	}


	/* Create object */
#ifdef JADEFUSION
	if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_PrefabInstance))
#endif
	{
	mpst_LockPickObj = pst_GO = OBJ_pst_DuplicateGameObjectFile
		(
			mst_WinHandles.pst_World,
			_pst_DragDrop->ul_FatFile,
			gul_PrefabKey,
#ifdef JADEFUSION
			NULL,//_asz_NewName, //popoverif popowar popoajour
            NULL
		);
	}
	else
	{
	mpst_LockPickObj = pst_GO = OBJ_pst_DuplicateGameObjectFile
		(
			mst_WinHandles.pst_World,
			_pst_DragDrop->ul_FatFile,
			gul_PrefabKey,
#endif
			NULL,
			&v
		);
	}
	gpo_DroppedGO = pst_GO;

	if(WOR_gb_HasAddedMaterial)
	{
		GDI_l_AttachWorld(mst_WinHandles.pst_DisplayData, mst_WinHandles.pst_World);
		WOR_gb_HasAddedMaterial = FALSE;
	}

	if(! (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
	{
		OBJ_SetInitialAbsolutePosition(pst_GO, &v);
		OBJ_RestoreInitialPos(pst_GO);
	}


	/* Force display */
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)
		mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayLight;
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Waypoints)
	{
		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Links)
			mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayWaypointInNetwork;
		else
			mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayWaypoint;
	}

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
	{
		if(pst_GO->pst_Base && pst_GO->pst_Base->pst_Visu)
		{
			pst_Geo = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
			if(pst_Geo && (pst_Geo->st_Id.i->ul_Type == GRO_Camera))
				mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayCamera;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropPrefab(EDI_tdst_DragDrop *_pst_DragDrop)
{
	DropPrefabExt(_pst_DragDrop->ul_FatFile, _pst_DragDrop->o_Pt, TRUE, TRUE);
	DropPrefabExt(_pst_DragDrop->ul_FatFile, _pst_DragDrop->o_Pt, TRUE, FALSE);
}

/*
 =======================================================================================================================
 Aim : Build a group, when dropping a prefab, correctly sorted according to hierarchy, so that when
 computing elements position, father position is always computed before child position
 =======================================================================================================================
 */
void F3D_cl_View::BuildGroupFromPrefab(OBJ_tdst_Group *pst_Group, OBJ_tdst_Prefab *_pst_Prefab, int _i_Index, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_Index;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (_i_Index >= _pst_Prefab->l_NbRef)
		return;

	/* Add element in group */
	if(_pst_Prefab->dst_Ref[_i_Index].c_IsActive && _pst_Prefab->dst_Ref[_i_Index].ul_Index != BIG_C_InvalidIndex)
		OBJ_AddInGroup(pst_Group, _pst_GO);
	
	/* Add every child in group */
	for(i = 0 ; i < _pst_Prefab->l_NbRef ; i++)
	{
		ul_Index = _pst_Prefab->dst_Ref[i].ul_Index;
		if (ul_Index == BIG_C_InvalidIndex)
			continue;

		OBJ_tdst_GameObject *pst_Child = ( ( OBJ_tdst_GameObject * ) LOA_ul_SearchAddress( BIG_PosFile( ul_Index ) ) );
		if (pst_Child->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy
			&& pst_Child->pst_Base->pst_Hierarchy
			&& pst_Child->pst_Base->pst_Hierarchy->pst_FatherInit)
		{
			if (pst_Child->pst_Base->pst_Hierarchy->pst_FatherInit == _pst_GO)
			{
				BuildGroupFromPrefab(pst_Group, _pst_Prefab, i, pst_Child);
			}
		}
	}

	return;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
extern ULONG IsSubGAODirectoryPresent(WOR_tdst_World* pWorld, char* pstrSubDirName);
#endif

void F3D_cl_View::DropPrefabExt(ULONG _ul_File, CPoint _o_Pt, BOOL _b_First, BOOL _b_BuildGrp)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX				ul_Index;
	EOUT_cl_Frame			*po_Out;
	EPFB_cl_Frame			*po_PFBeditor;
	int						i, j, k, l_NbObj, l_Active;
	EDI_tdst_DragDrop		st_DD;
	OBJ_tdst_Prefab			*P;
	MATH_tdst_Matrix		st_Ref, st_Ref1, M, MI;
	static OBJ_tdst_Group	*pst_Group;
	static OBJ_tdst_Group	*pst_NewGroup;
	static int				i_RefCount;
	OBJ_tdst_GameObject		*pst_GO, *pst_GO1;
	int						i_First;
#ifdef JADEFUSION
	BIG_INDEX				ul_File;
	int						i_AutoInc, i_Str, i_Nb;
	CString					strNewName, strLocalFile;
	char					asz_Temp[BIG_C_MaxLenPath];
	char					asz_Temp2[BIG_C_MaxLenPath];
	BOOL					b_Find;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Out = (EOUT_cl_Frame *) mpo_AssociatedEditor;

	/* try to get prefab from editor */
	po_PFBeditor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
	if((po_PFBeditor) && po_PFBeditor->mpst_Prefab && (po_PFBeditor->mpst_Prefab->ul_Index == _ul_File))
		P = po_PFBeditor->mpst_Prefab;
	else
		P = Prefab_pst_Load(_ul_File, NULL);
	if(!P || !P->l_NbRef) return;

	/* Groups to update references */
	i_First = -1;
	if(_b_First) i_RefCount = 0;
	if(_b_First && _b_BuildGrp)
	{
		pst_Group = GRP_pst_CreateNewGroup();
		pst_NewGroup = GRP_pst_CreateNewGroup();
	}

	if(_b_First && !_b_BuildGrp)
	{
		GRP_AfterLoaded(mst_WinHandles.pst_World, pst_Group);
	}

	l_NbObj = 0;
	l_Active = (P->l_Type == OBJPREFAB_C_AllRef) ? 1 : 0;
	for(i = 0; i < P->l_NbRef; i++) P->dst_Ref[i].c_IsActive = l_Active;

	switch(P->l_Type)
	{
	case OBJPREFAB_C_Cycle:
	case OBJPREFAB_C_Random:
	case OBJPREFAB_C_RandomUser:
		P->dst_Ref[P->c_CurrentRef].c_IsActive = 1;
		break;
	}

	Prefab_b_ComputeBV(P, TRUE);

	/* get prefix key */
	gul_PrefabKey = BIG_ul_GetNewKey(BIG_C_InvalidIndex);

    /* Load all prefab elements */
	if(_b_BuildGrp)
	{
		for(i = 0; i < P->l_NbRef; i++)
		{
			if(!P->dst_Ref[i].c_IsActive) continue;
			ul_Index = P->dst_Ref[i].ul_Index;
			if(ul_Index == BIG_C_InvalidIndex) continue;
			PrefabRef_b_UpdateBV(&P->dst_Ref[i]);
			WOR_gpst_WorldToLoadIn = mst_WinHandles.pst_World;
			LOA_MakeFileRef(BIG_FileKey(ul_Index), (ULONG *) &pst_GO, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
			LOA_Resolve();
		}
	}


	/* Build group sorted from father to children (if hierarchy), to prevent from
	 * problems if hierarchy inside prefab is not correct */
	if(_b_BuildGrp)
	{
		for(i = 0; i < P->l_NbRef; i++)
		{
			if (P->dst_Ref[i].ul_Index == BIG_C_InvalidIndex)
				continue;

			pst_GO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(P->dst_Ref[i].ul_Index));

			if (!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy
				&& pst_GO->pst_Base
				&& pst_GO->pst_Base->pst_Hierarchy
				&& pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit))
			{
				BuildGroupFromPrefab(pst_Group, P, 0, (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(P->dst_Ref[i].ul_Index)));
			}
		}
	}






	i = 0;

	for(k = 0; k < P->l_NbRef; k++)
	{	
		/* get index of prefab element corresponding to group object
		 * so that it's treated in the right order */
		for(j = 0; j < P->l_NbRef; j++)
		{
			if (P->dst_Ref[j].ul_Key == ((OBJ_tdst_GameObject *) pst_Group->pst_AllObjects->p_Table[k].p_Pointer)->ul_MyKey)
			{
				i = j;
				break;
			}
		}

		if(!P->dst_Ref[i].c_IsActive) continue;
		ul_Index = P->dst_Ref[i].ul_Index;
		if(ul_Index == BIG_C_InvalidIndex) continue;

		if(P->dst_Ref[i].uc_Type == OBJPREFAB_C_RefIsGao)
		{
			if(_b_BuildGrp)
			{
				i_RefCount++;
			}
			else
			{
				st_DD.o_Pt = _o_Pt;
				st_DD.ul_FatFile = ul_Index;

#ifdef JADEFUSION
				if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_PrefabInstance))
				{
				i_AutoInc = 1;

				/*modify name with auto increment number for the map*/
				strNewName = BIG_NameFile(_ul_File);
				if(strNewName.Find(EDI_Csz_ExtPrefab) != -1)
					strNewName.Delete(strNewName.GetLength() - strlen(EDI_Csz_ExtPrefab), strlen(EDI_Csz_ExtPrefab));
				strNewName += "_";
				if(i_AutoInc < 10)
					strNewName += "0";
				strNewName += itoa(i_AutoInc, asz_Temp, 3);
				strNewName += "@";
				strNewName += BIG_NameFile(P->dst_Ref[i].ul_Index);

				b_Find = false;
				while(1)
				{
					ul_File = IsSubGAODirectoryPresent(mst_WinHandles.pst_World, asz_Temp);
					//compare with all files in the GAO folder
					while(ul_File != BIG_C_InvalidIndex)
					{
						strLocalFile = BIG_NameFile(ul_File);
						if(strNewName == strLocalFile)
						{
							b_Find = TRUE;
							break;
						}
						ul_File = BIG_NextFile(ul_File);
					}
					if(!b_Find)
						break;
					b_Find = FALSE;
					
					itoa(i_AutoInc, asz_Temp, 10);
					itoa(++i_AutoInc, asz_Temp2, 10);
					
					//i_Nb est le nombre de chiffre à modifier
					i_Nb = i_AutoInc/10 + 1;
					i_Str = strNewName.Find(_T('@')) ;
					if(i_Str != -1)
						strNewName.Delete(i_Str - i_Nb, i_Nb);
					strNewName.Insert(i_Str - i_Nb, _T(asz_Temp2));
				}
				/*modify name with auto increment number for the map*/

				//DropGameObject(&st_DD, (LPSTR)(LPCSTR) strNewName);
				DropGameObject(&st_DD,i_First == -1);//popoverif popowar
				}
				else
#endif

				{
					DropGameObject(&st_DD,i_First == -1);
				}
				OBJ_AddInGroup(pst_NewGroup, gpo_DroppedGO);
				i_RefCount++;

				if(P->l_Type == OBJPREFAB_C_AllRef)
				{
					if(i_First == -1)
					{
						MATH_tdst_Vector v;

						i_First = i_RefCount - 1;
						MATH_CopyMatrix(&st_Ref1, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
						MATH_AddVector(&v, &P->st_BVMax, &P->st_BVMin);
						MATH_ScaleEqualVector(&v, 0.5f);
						gpo_DroppedGO->pst_GlobalMatrix->T.x -= v.x;

						gpo_DroppedGO->pst_GlobalMatrix->T.y -= v.y;
						gpo_DroppedGO->pst_GlobalMatrix->T.z -=P->st_BVMin.z;
						MATH_CopyMatrix(&st_Ref, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
						if(! (gpo_DroppedGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
							OBJ_SetInitialAbsoluteMatrix(gpo_DroppedGO, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
					}
					else
					{
						/* Calcul de la matrice relative par rapport aux objets models */
						pst_GO = (OBJ_tdst_GameObject *) pst_Group->pst_AllObjects->p_Table[i_First].p_Pointer;
						pst_GO1 = (OBJ_tdst_GameObject *) pst_Group->pst_AllObjects->p_Table[i_RefCount - 1].p_Pointer;
						MATH_InvertMatrix(&MI, pst_GO->pst_GlobalMatrix);
						MATH_MulMatrixMatrix(&P->dst_Ref[i].st_Matrix, pst_GO1->pst_GlobalMatrix, &MI);

						MATH_SetIdentityMatrix(&M);
						MATH_MulMatrixMatrix(&M, &P->dst_Ref[i].st_Matrix, &st_Ref);
						OBJ_SetAbsoluteMatrix(gpo_DroppedGO, &M);
						if(! (gpo_DroppedGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
							OBJ_SetInitialAbsoluteMatrix(gpo_DroppedGO, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
					}
				}

				MATH_AddEqualVector(OBJ_pst_GetAbsolutePosition(gpo_DroppedGO), &P->st_Offset);
				gpo_DroppedGO->ul_EditorFlags &= ~OBJ_C_EditFlags_Hidden;
				gpo_DroppedGO->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;
				gpo_DroppedGO->ul_AdditionalFlags &= ~OBJ_C_EdAddFlags_HiddenByWowFilter;
				gpo_DroppedGO->ul_PrefabKey = BIG_FileKey(_ul_File);
				gpo_DroppedGO->ul_PrefabObjKey = BIG_FileKey(ul_Index);

				/* rotate eventually object */
				if(P->f_Rotate != 0)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Matrix	MCopy;
					MATH_tdst_Vector	Trans;
					MATH_tdst_Vector	Temp, Temp1, TempZ;
					/*~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_MakeRotationMatrix_ZAxis(&M, P->f_Rotate, TRUE);
					MATH_CopyVector(&Trans, OBJ_pst_GetAbsolutePosition(gpo_DroppedGO));
					MATH_CopyMatrix(&MCopy, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
					MATH_MulMatrixMatrix(OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO), &M, &MCopy);

					TempZ.x = TempZ.y = 0;
					TempZ.z = 1;
					MATH_SubVector(&Temp, &Trans, &st_Ref1.T);
					Temp.z = 0;
					MATH_VecRotate(&Temp1, &TempZ, &Temp, P->f_Rotate);
					MATH_AddVector(OBJ_pst_GetAbsolutePosition(gpo_DroppedGO), &st_Ref1.T, &Temp1);
					OBJ_pst_GetAbsolutePosition(gpo_DroppedGO)->z = Trans.z;
					if(! (gpo_DroppedGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
						OBJ_SetInitialAbsoluteMatrix(gpo_DroppedGO, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
				}

				/*
				 * change name of object £
				 * Force to lock picking with the first created object
				 */
				po_Out->mpo_EngineFrame->mpo_DisplayView->mb_LockPickObj = TRUE;
			}
		}
		else if(P->dst_Ref[i].uc_Type == OBJPREFAB_C_RefIsPrefab)
		{
			if(_b_First) DropPrefabExt(ul_Index, _o_Pt, FALSE, _b_BuildGrp);
		}
	}














#if 0
	/* Scan all group */
	for(i = 0; i < P->l_NbRef; i++)
	{
		if(!P->dst_Ref[i].c_IsActive) continue;
		ul_Index = P->dst_Ref[i].ul_Index;
		if(ul_Index == BIG_C_InvalidIndex) continue;

		if(P->dst_Ref[i].uc_Type == OBJPREFAB_C_RefIsGao)
		{
			if(_b_BuildGrp)
			{
				PrefabRef_b_UpdateBV(&P->dst_Ref[i]);
				WOR_gpst_WorldToLoadIn = mst_WinHandles.pst_World;
				LOA_MakeFileRef(BIG_FileKey(ul_Index), (ULONG *) &pst_GO, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
				LOA_Resolve();
				OBJ_AddInGroup(pst_Group, pst_GO);
				i_RefCount++;
			}
			else
			{
				st_DD.o_Pt = _o_Pt;
				st_DD.ul_FatFile = ul_Index;
#ifdef JADEFUSION
				if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_PrefabInstance))
				{
				i_AutoInc = 1;

				/*modify name with auto increment number for the map*/
				strNewName = BIG_NameFile(_ul_File);
				if(strNewName.Find(EDI_Csz_ExtPrefab) != -1)
					strNewName.Delete(strNewName.GetLength() - strlen(EDI_Csz_ExtPrefab), strlen(EDI_Csz_ExtPrefab));
				strNewName += "_";
				if(i_AutoInc < 10)
					strNewName += "0";
				strNewName += itoa(i_AutoInc, asz_Temp, 3);
				strNewName += "@";
				strNewName += BIG_NameFile(P->dst_Ref[i].ul_Index);

				b_Find = false;
				while(1)
				{
					ul_File = IsSubGAODirectoryPresent(mst_WinHandles.pst_World, asz_Temp);
					//compare with all files in the GAO folder
					while(ul_File != BIG_C_InvalidIndex)
					{
						strLocalFile = BIG_NameFile(ul_File);
						if(strNewName == strLocalFile)
						{
							b_Find = TRUE;
							break;
						}
						ul_File = BIG_NextFile(ul_File);
					}
					if(!b_Find)
						break;
					b_Find = FALSE;
					
					itoa(i_AutoInc, asz_Temp, 10);
					itoa(++i_AutoInc, asz_Temp2, 10);
					
					//i_Nb est le nombre de chiffre à modifier
					i_Nb = i_AutoInc/10 + 1;
					i_Str = strNewName.Find(_T('@')) ;
					if(i_Str != -1)
						strNewName.Delete(i_Str - i_Nb, i_Nb);
					strNewName.Insert(i_Str - i_Nb, _T(asz_Temp2));
				}
				/*modify name with auto increment number for the map*/

				//DropGameObject(&st_DD, (LPSTR)(LPCSTR) strNewName);
				DropGameObject(&st_DD,i_First == -1);//popoverif popowar
				}
				else
#endif

				{
					DropGameObject(&st_DD,i_First == -1);
				}
				OBJ_AddInGroup(pst_NewGroup, gpo_DroppedGO);
				i_RefCount++;

				if(P->l_Type == OBJPREFAB_C_AllRef)
				{
					if(i_First == -1)
					{
						MATH_tdst_Vector v;

						i_First = i_RefCount - 1;
						MATH_CopyMatrix(&st_Ref1, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
						MATH_AddVector(&v, &P->st_BVMax, &P->st_BVMin);
						MATH_ScaleEqualVector(&v, 0.5f);
						gpo_DroppedGO->pst_GlobalMatrix->T.x -= v.x;

						gpo_DroppedGO->pst_GlobalMatrix->T.y -= v.y;
						gpo_DroppedGO->pst_GlobalMatrix->T.z -=P->st_BVMin.z;
						MATH_CopyMatrix(&st_Ref, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
						if(! (gpo_DroppedGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
							OBJ_SetInitialAbsoluteMatrix(gpo_DroppedGO, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
					}
					else
					{
						/* Calcul de la matrice relative par rapport aux objets models */
						pst_GO = (OBJ_tdst_GameObject *) pst_Group->pst_AllObjects->p_Table[i_First].p_Pointer;
						pst_GO1 = (OBJ_tdst_GameObject *) pst_Group->pst_AllObjects->p_Table[i_RefCount - 1].p_Pointer;
						MATH_InvertMatrix(&MI, pst_GO->pst_GlobalMatrix);
						MATH_MulMatrixMatrix(&P->dst_Ref[i].st_Matrix, pst_GO1->pst_GlobalMatrix, &MI);

						MATH_SetIdentityMatrix(&M);
						MATH_MulMatrixMatrix(&M, &P->dst_Ref[i].st_Matrix, &st_Ref);
						OBJ_SetAbsoluteMatrix(gpo_DroppedGO, &M);
						if(! (gpo_DroppedGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
							OBJ_SetInitialAbsoluteMatrix(gpo_DroppedGO, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
					}
				}

				MATH_AddEqualVector(OBJ_pst_GetAbsolutePosition(gpo_DroppedGO), &P->st_Offset);
				gpo_DroppedGO->ul_EditorFlags &= ~OBJ_C_EditFlags_Hidden;
				gpo_DroppedGO->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;
				gpo_DroppedGO->ul_AdditionalFlags &= ~OBJ_C_EdAddFlags_HiddenByWowFilter;
				gpo_DroppedGO->ul_PrefabKey = BIG_FileKey(_ul_File);
				gpo_DroppedGO->ul_PrefabObjKey = BIG_FileKey(ul_Index);

				/* rotate eventually object */
				if(P->f_Rotate != 0)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Matrix	MCopy;
					MATH_tdst_Vector	Trans;
					MATH_tdst_Vector	Temp, Temp1, TempZ;
					/*~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_MakeRotationMatrix_ZAxis(&M, P->f_Rotate, TRUE);
					MATH_CopyVector(&Trans, OBJ_pst_GetAbsolutePosition(gpo_DroppedGO));
					MATH_CopyMatrix(&MCopy, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
					MATH_MulMatrixMatrix(OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO), &M, &MCopy);

					TempZ.x = TempZ.y = 0;
					TempZ.z = 1;
					MATH_SubVector(&Temp, &Trans, &st_Ref1.T);
					Temp.z = 0;
					MATH_VecRotate(&Temp1, &TempZ, &Temp, P->f_Rotate);
					MATH_AddVector(OBJ_pst_GetAbsolutePosition(gpo_DroppedGO), &st_Ref1.T, &Temp1);
					OBJ_pst_GetAbsolutePosition(gpo_DroppedGO)->z = Trans.z;
					if(! (gpo_DroppedGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
						OBJ_SetInitialAbsoluteMatrix(gpo_DroppedGO, OBJ_pst_GetAbsoluteMatrix(gpo_DroppedGO));
				}

				/*
				 * change name of object £
				 * Force to lock picking with the first created object
				 */
				po_Out->mpo_EngineFrame->mpo_DisplayView->mb_LockPickObj = TRUE;
			}
		}
		else if(P->dst_Ref[i].uc_Type == OBJPREFAB_C_RefIsPrefab)
		{
			if(_b_First) DropPrefabExt(ul_Index, _o_Pt, FALSE, _b_BuildGrp);
		}
	}
#endif





	if(_b_BuildGrp) 
	{
		Prefab_b_ComputeBV(P, FALSE);
		Prefab_ul_Save(P);
		return;
	}


	// Set correct fathers
	for(i = 0; i < P->l_NbRef; i++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_NewGroup->pst_AllObjects->p_Table[i].p_Pointer;
		if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
		{
			int j;
			for(j = 0; j < P->l_NbRef; j++)
			{
				if (pst_Group->pst_AllObjects->p_Table[j].p_Pointer == pst_GO->pst_Base->pst_Hierarchy->pst_Father)
				{
					pst_GO->pst_Base->pst_Hierarchy->pst_Father = (OBJ_tdst_GameObject *)pst_NewGroup->pst_AllObjects->p_Table[j].p_Pointer;
					pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit = pst_GO->pst_Base->pst_Hierarchy->pst_Father;
					break;
				}
			}
		}
	}

	// Set initial matrix for objects with hierarchy
	for(i = 0; i < P->l_NbRef; i++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_NewGroup->pst_AllObjects->p_Table[i].p_Pointer;
		if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
		{
			OBJ_SetInitialAbsoluteMatrix(pst_GO, pst_GO->pst_GlobalMatrix);
		}
	}

	/* Update all references */
	if(_b_First)
	{
		GRP_RepercuteHierarchy(pst_Group, pst_NewGroup);
		GRP_RepercuteAIRefs(pst_Group, pst_NewGroup);
		GRP_RepercuteEventsRefs(pst_Group, pst_NewGroup);
		GRP_RepercuteLinks(pst_Group, pst_NewGroup, mst_WinHandles.pst_World);
	
		GRP_ReinitAllGO(pst_NewGroup);
		GRP_DetachAndDestroyGAOs(mst_WinHandles.pst_World, pst_Group, TRUE );
		OBJ_FreeGroup(pst_Group);
		OBJ_FreeGroup(pst_NewGroup);
	}

	/* Parcequ'on a recalculé les matrices/BV */
	Prefab_b_ComputeBV(P, FALSE);
	Prefab_ul_Save(P);

	gul_PrefabKey = 0;
	po_Out->mpo_EngineFrame->mpo_DisplayView->mb_LockPickObj = FALSE;
	Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropObjectModel(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_Size, i;
	BIG_INDEX			ul_File;
	EOUT_cl_Frame		*po_Out;
	int					i_Want, i_Has, i_Once;
	int					i_HasB;
	EDI_tdst_DragDrop	st_DD;
	CPoint				o_Pt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Out = (EOUT_cl_Frame *) mpo_AssociatedEditor;
	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(_pst_DragDrop->ul_FatFile), &ul_Size);

	/* Scan all group */
	i_Want = 0;
	i_Once = 0;
	while(1)
	{
		i_Has = 0;
		i_HasB = 0;
		for(i = 0; i < (ul_Size / sizeof(BIG_tdst_GroupElem)); i++)
		{
			if(!pst_Buf[i].ul_Key) continue;
			ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
			if(ul_File == BIG_C_InvalidIndex) continue;

			i_Has = 1;
			st_DD.b_Copy = TRUE;
			switch(i_Want)
			{
			case 0:
				if(BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtGameObject))
				{
					st_DD.b_Copy = FALSE;
					break;
				}

				continue;

			case 1:
				if(BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtGameObject)) continue;
				if(BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtGraphicObject))
				{
					if(i_Once)
						st_DD.b_Copy = TRUE;
					else
						st_DD.b_Copy = FALSE;
					break;
				}

				continue;

			case 2:
				if(BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtGraphicObject)) continue;
				if(BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtGameObject)) continue;
				break;
			}

			/* Simulate a D & D */
			st_DD.ul_FatDir = BIG_C_InvalidIndex;
			st_DD.ul_FatFile = ul_File;
			st_DD.i_Type = EDI_DD_File;
			st_DD.o_Pt = _pst_DragDrop->o_Pt;
			if(po_Out->i_OnMessage(EDI_MESSAGE_CANDRAGDROP, (ULONG) & st_DD, 0))
			{
				pst_Buf[i].ul_Key = 0;
				mb_CanRefresh = FALSE;
				po_Out->i_OnMessage(EDI_MESSAGE_ENDDRAGDROP, (ULONG) & st_DD, 0);
				mb_CanRefresh = TRUE;
				i_Want++;
				i_Once = 1;
				i_HasB = 1;
				break;
			}
		}

		if(i_Has == 0) break;

		/* We have not found a visual. Create a dummy one */
		if(i_Want == 0)
			i_Want++;
		else if((i_Want == 1) && (!i_Once))
		{
			o_Pt = _pst_DragDrop->o_Pt;
			ScreenToClient(&o_Pt);
			GAO_CreateInvisible(o_Pt);
			i_Want++;
		}
		else if((i_Want == 1) && (!i_HasB))
			i_Want++;

		/* Force to lock picking with the first created object */
		po_Out->mpo_EngineFrame->mpo_DisplayView->mb_LockPickObj = TRUE;
	}

	po_Out->mpo_EngineFrame->mpo_DisplayView->mb_LockPickObj = FALSE;
	L_free(pst_Buf);
	Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::ForceSelectObject(void *_p_Obj, BOOL _b_Desel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !mst_WinHandles.pst_World ) return;
	
	if(_b_Desel)
	{
		SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_Undo_Unselect, (ULONG) this, 0);

		/* Inform linked editors about deselection */
		if(mpo_AssociatedEditor)
		{
			pst_Item = mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
			while(pst_Item != NULL)
			{
				M_MF()->SendMessageToLinks(mpo_AssociatedEditor, EDI_MESSAGE_DELSELDATA, 0, (ULONG) _p_Obj);
				pst_Item = pst_Item->pst_Next;
			}
		}

		SEL_Close(mst_WinHandles.pst_World->pst_Selection);
	}

	Selection_Object((OBJ_tdst_GameObject *) _p_Obj, SEL_C_SIF_Object);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropSoundBank(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Device					*pst_Dev;
	OBJ_tdst_GameObject				*pst_GO;
	BIG_KEY							ul_Key;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find the game object where to drop track list */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);

	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();

	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
	if(ul_Key == BIG_C_InvalidIndex) return;

	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Sound)
		SND_FreeMainStruct(pst_GO);
	else
	{
		OBJ_GameObject_CreateExtendedIfNot(pst_GO);
		pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_Sound;
	}

	ul_Key = LOA_ul_SearchAddress(BIG_PosFile(_pst_DragDrop->ul_FatFile));
	if(ul_Key == (ULONG) pst_GO->pst_Extended->pst_Sound) return;

	if(ul_Key == -1)
	{
		LOA_MakeFileRef
		(
			BIG_FileKey(_pst_DragDrop->ul_FatFile),
			(ULONG *) &pst_GO->pst_Extended->pst_Sound,
			SND_ul_CallbackLoadBank,
			LOA_C_MustExists | LOA_C_HasUserCounter
		);
		LOA_Resolve();
	}
	else
	{
		pst_GO->pst_Extended->pst_Sound = (SND_tdun_Main *) ul_Key;
        pst_GO->pst_Extended->pst_Sound->st_Bank.ul_UserCount++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropSoundMetaBank(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Device					*pst_Dev;
	OBJ_tdst_GameObject				*pst_GO;
	BIG_KEY							ul_Key;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find the game object where to drop track list */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);

	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();

	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
	if(ul_Key == BIG_C_InvalidIndex) return;

	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Sound)
		SND_FreeMainStruct(pst_GO);
	else
	{
		OBJ_GameObject_CreateExtendedIfNot(pst_GO);
		pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_Sound;
	}

	ul_Key = LOA_ul_SearchAddress(BIG_PosFile(_pst_DragDrop->ul_FatFile));
	if(ul_Key == (ULONG) pst_GO->pst_Extended->pst_Sound) return;

	if(ul_Key == -1)
	{
		LOA_MakeFileRef
		(
			BIG_FileKey(_pst_DragDrop->ul_FatFile),
			(ULONG *) &pst_GO->pst_Extended->pst_Sound,
			SND_ul_CallbackLoadMetabank,
			LOA_C_MustExists | LOA_C_HasUserCounter
		);
		LOA_Resolve();
	}
	else
	{
		pst_GO->pst_Extended->pst_Sound = (SND_tdun_Main *) ul_Key;
        pst_GO->pst_Extended->pst_Sound->st_Bank.ul_UserCount++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropMorphData(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Device					*pst_Dev;
	OBJ_tdst_GameObject				*pst_GO;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	GEO_tdst_Object					*pst_Geo;
	MDF_tdst_Modifier				st_Mod, *pst_Mod;
	GEO_tdst_ModifierMorphing		*pst_Morph;
	char							*pc_Buffer;
	char							sz_Msg[512];
	ULONG							ul_Length, ul_Flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find the game object where to drop track list */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);

	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();

	/* check GO */
	if(!pst_GO)
	{
		M_MF()->MessageBox("Morph data must be dropped on a Object", "Error", MB_OK | MB_ICONSTOP);
		return;
	}

	/* check geometry */
	pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(!pst_Geo)
	{
		M_MF()->MessageBox("Object must have graphic data to accept morph data", "Error", MB_OK | MB_ICONSTOP);
		return;
	}

	/* check if there's already a morphing modifier */
	pst_Mod = MDF_pst_GetByType(pst_GO, MDF_C_Modifier_Morphing);
	if(pst_Mod)
	{
		if
		(
			M_MF()->MessageBox
				(
					"Object has already morphing data\nreplace ?",
					"Confirm",
					MB_YESNO | MB_ICONQUESTION
				) == IDNO
		) return;
	}

	/* load data */
	pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(_pst_DragDrop->ul_FatFile), &ul_Length);
	GEO_ModifierMorphing_Create(pst_GO, &st_Mod, NULL);
	GEO_ul_ModifierMorphing_Load(&st_Mod, pc_Buffer);

	/* check number of points */
	pst_Morph = (GEO_tdst_ModifierMorphing *) st_Mod.p_Data;
	if(pst_Morph->l_NbPoints != pst_Geo->l_NbPoints)
	{
		sprintf
		(
			sz_Msg,
			"Graphic (%d) has not the same number of vertices than Morph data (%d)",
			pst_Geo->l_NbPoints,
			pst_Morph->l_NbPoints
		);
		M_MF()->MessageBox(sz_Msg, "Error", MB_OK | MB_ICONSTOP);
		GEO_ModifierMorphing_Destroy(&st_Mod);
		return;
	}

	/* add modifier to game object */
	if(!pst_Mod)
	{
		ul_Flags = pst_GO->ul_IdentityFlags;
		OBJ_ChangeIdentityFlags(pst_GO, ul_Flags | OBJ_C_IdentityFlag_ExtendedObject, ul_Flags);
		pst_Mod = MDF_pst_Modifier_Create(pst_GO, MDF_C_Modifier_Morphing, NULL);
		MDF_Modifier_AddToGameObject(pst_GO, pst_Mod);
	}

	GEO_ModifierMorphing_Destroy(pst_Mod);
	pst_Mod->p_Data = st_Mod.p_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropRLIData(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Device					*pst_Dev;
	OBJ_tdst_GameObject				*pst_GO;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	GEO_tdst_Object					*pst_Geo;
	ULONG							*pul_RLI, ul_Copy;
	GRO_tdst_Visu					*pst_Visu;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find the game object where to drop track list */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);

	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();

	/* check GO */
	if(!pst_GO)
	{
		M_MF()->MessageBox("RLI data must be dropped on a Object", "Error", MB_OK | MB_ICONSTOP);
		return;
	}
	
	/* check geometry */
	pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(!pst_Geo)
	{
		M_MF()->MessageBox("Object must have graphic data to accept RLI data", "Error", MB_OK | MB_ICONSTOP);
		return;
	}
	if ( !pst_Geo->l_NbPoints )
	{
		M_MF()->MessageBox("Geometry without point, cannot DnD RLI", "Error", MB_OK | MB_ICONSTOP);
		return;
	}
	
	/* LOAD RLI */
	pul_RLI = (ULONG *) OBJ_ul_GameObjectRLICallback( BIG_PosFile(_pst_DragDrop->ul_FatFile) );
	if ( !pul_RLI ) return;
	
	/* CHECK GO RLI buffer */
	pst_Visu = pst_GO->pst_Base->pst_Visu;
	if ( pst_Visu->dul_VertexColors )
	{
		if ( pst_Visu->dul_VertexColors[0] != pst_Geo->l_NbPoints)
		{
			OBJ_VertexColor_Realloc( pst_GO, pst_Geo->l_NbPoints );
			//ul_Index = LOA_ul_SearchIndexWithAddress( (ULONG) pst_Visu->dul_VertexColors );
			//LOA_DeleteAddress( pst_Visu->dul_VertexColors );
			//pst_Visu->dul_VertexColors = (ULONG *) MEM_p_Realloc( pst_Visu->dul_VertexColors, (pst_Geo->l_NbPoints + 1) * sizeof( ULONG ) );
			//if ( ul_Index != BIG_C_InvalidIndex ) 
			//	LOA_AddAddress( ul_Index, pst_Visu->dul_VertexColors );
		}
	}
	else
	{
		pst_Visu->dul_VertexColors = (ULONG *) MEM_p_Alloc( (pst_Geo->l_NbPoints + 1) * sizeof( ULONG ) );
		pst_Visu->dul_VertexColors[ 0 ] = pst_Geo->l_NbPoints;
	}
	
	/* COPY RLI into GO RLI buffer */
	if ( pul_RLI[ 0 ] < (ULONG) pst_Geo->l_NbPoints)
		ul_Copy = pul_RLI[ 0 ];
	else
		ul_Copy = pst_Geo->l_NbPoints;
	L_memcpy( &pst_GO->pst_Base->pst_Visu->dul_VertexColors[ 1 ], &pul_RLI[ 1 ], ul_Copy * sizeof(ULONG) );
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void F3D_cl_View::LogUnCollidable(void)
{
	OBJ_tdst_GameObject		*pst_GO;
	COL_tdst_DetectionList	*pst_List;
	char					asz_Log[1024];
	int						i;

	pst_GO = (OBJ_tdst_GameObject*) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);

	if
	(
		!pst_GO
	||	!(pst_GO->pst_Extended)
	||	!(pst_GO->pst_Extended->pst_Col)
	||	!(((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_UnCollidable)
	) 
	{
		sprintf(asz_Log, "No GameObject in [%s] Uncollidable List", pst_GO->sz_Name ? pst_GO->sz_Name : "Unknown");
		LINK_PrintStatusMsg(asz_Log);
		return;
	}


	pst_List = (((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_UnCollidable);

	sprintf(asz_Log, "List of GameObject(s) in [%s] Uncollidable List", pst_GO->sz_Name ? pst_GO->sz_Name : "Unknown");	
	LINK_PrintStatusMsg(asz_Log);
	LINK_PrintStatusMsg("----------------------------");

	for(i = 0; i < (int) pst_List->ul_NbUnCollidableObjects; i++)
	{
		sprintf(asz_Log, "-> %s", pst_List->dpst_UnCollidableObject[i]->sz_Name);
		LINK_PrintStatusMsg(asz_Log);
	}

	LINK_PrintStatusMsg("----------------------------");
}

#ifdef JADEFUSION
void F3D_cl_View::MDF_CreateSoftBody()
{
    OBJ_tdst_GameObject	* pst_Gao;

    pst_Gao = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
    if(!pst_Gao) return;

    GAO_ModifierSoftBody_CreateFromGro(pst_Gao);
}
#endif

#endif

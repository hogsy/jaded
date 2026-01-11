/*$T F3Dview_ode.cpp GC!1.71 03/09/00 10:34:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#ifdef ODE_INSIDE

#include "SELection/SELection.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJODE.h"
#include "F3Dframe/F3Dview.h"

/*$4
 ***********************************************************************************************************************
    ODE Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */

BOOL F3D_b_MoveODECameraAxis(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE		*pst_ODE;
    OBJ_tdst_GameObject *pst_GO;
    MATH_tdst_Vector    *pst_Vector, st_GCS_Move;
    CAM_tdst_Camera     *pst_Camera;
    MAI_tdst_WinHandles *pst_WinHandles;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	pst_Vector = (MATH_tdst_Vector *) _ul_Param1;
	pst_WinHandles = (MAI_tdst_WinHandles *) _ul_Param2;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
			LINK_PrintStatusMsg("Cannot edit ODE, you have a Cob selected");
		
		return FALSE;
	}

	if(pst_GO && pst_GO->pst_Base && pst_GO->pst_Base->pst_ODE)	//we have a ODE object
		pst_ODE = pst_GO->pst_Base->pst_ODE;
	else
	{
		LINK_PrintStatusMsg("Cannot edit ODE, you must select a Gao with a ODE Box");
		return FALSE;
	}


    if (pst_WinHandles) 
		pst_Camera = (CAM_tdst_Camera *) (&pst_WinHandles->pst_DisplayData->st_Camera);
  
    if( pst_WinHandles )
		MATH_TransformVector(&st_GCS_Move, &pst_Camera->st_Matrix, pst_Vector);
	else
		MATH_CopyVector(&st_GCS_Move, pst_Vector);

	MATH_tdst_Matrix st_TempMatrix;
	MATH_SetIdentityMatrix(&st_TempMatrix);
	st_TempMatrix.lType = MATH_Ci_NoRotationNoAnyFormOfScale;
		
	MATH_Invert33Matrix(&st_TempMatrix, pst_GO->pst_GlobalMatrix);
	MATH_TransformVector(&st_GCS_Move, &st_TempMatrix, &st_GCS_Move);

    MATH_NegEqualVector(&st_GCS_Move);
	OBJ_ODE_Move(pst_GO, &st_GCS_Move);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_ODE_Move(MATH_tdst_Vector *_pst_Move)
{
    SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_MoveODECameraAxis, (ULONG) _pst_Move, (ULONG) & mst_WinHandles);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL F3D_b_RotateODE(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MAI_tdst_WinHandles *pst_WinHandles;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_WinHandles = (MAI_tdst_WinHandles *) _ul_Param2;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
			LINK_PrintStatusMsg("Cannot edit ODE, you have a Cob selected");

		return FALSE;
	}

	if(pst_GO && pst_GO->pst_Base && pst_GO->pst_Base->pst_ODE)	//we have a ODE object
	{
		OBJ_ODE_Rotate(pst_GO, (MATH_tdst_Vector *) _ul_Param1, (MATH_tdst_Vector *) _ul_Param2);
		return TRUE;
	}
	else
	{
		LINK_PrintStatusMsg("Cannot edit ODE, you must select a Gao with a ODE Box");
		return FALSE;
	}
}
/**/
void F3D_cl_View::Selection_ODE_Rotate(MATH_tdst_Vector *_st_GlobalAxis, MATH_tdst_Vector *_pst_Angle, MATH_tdst_Vector *_pst_RotationCenter)
{
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_RotateODE, (ULONG) _st_GlobalAxis, (ULONG)_pst_Angle);
}
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_ScaleODE(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
	MAI_tdst_WinHandles *pst_WinHandles;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_WinHandles = (MAI_tdst_WinHandles *) _ul_Param2;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
			LINK_PrintStatusMsg("Cannot edit ODE, you have a Cob selected");

		return FALSE;
	}

	if(pst_GO && pst_GO->pst_Base && pst_GO->pst_Base->pst_ODE)	//we have a ODE object
	{
		OBJ_ODE_Scale(pst_GO, (MATH_tdst_Vector *) _ul_Param1);
		return TRUE;
	}
	else
	{
		LINK_PrintStatusMsg("Cannot edit ODE, you must select a Gao with a ODE Box");
		return FALSE;
	}
}
/**/
void F3D_cl_View::Selection_ODE_Scale(MATH_tdst_Vector *_pst_Scale)
{
    SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_ScaleODE, (ULONG) _pst_Scale, (ULONG) & mst_WinHandles);
}

#endif	//ODE_INSIDE
#endif	//ACTIVE_EDITORS

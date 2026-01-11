/*$T F3Dview_zonebv.cpp GC!1.71 03/09/00 10:34:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "SELection/SELection.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/COLlision/COLaccess.h"

#include "F3Dframe/F3Dview.h"



/*$4
 ***********************************************************************************************************************
    Zones Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_MoveZoneCameraAxis(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_Zone_GO;
    COL_tdst_ZDx        *pst_ZDx;
    COL_tdst_Cob        *pst_Cob;
    MATH_tdst_Vector    *pst_Vector;
    MATH_tdst_Matrix    st_Matrix;
    MATH_tdst_Vector    st_Move;
    CAM_tdst_Camera     *pst_Camera;
    MAI_tdst_WinHandles *pst_WinHandles;
    ULONG               ul_Type;
    void                *p_Shape;
    BOOL                b_LocalMode;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_WinHandles = (MAI_tdst_WinHandles *) _ul_Param2;
    pst_Zone_GO = (OBJ_tdst_GameObject *) _pst_Sel->ul_User;
    pst_Vector = (MATH_tdst_Vector *) _ul_Param1;
    pst_Camera = (CAM_tdst_Camera *) (&pst_WinHandles->pst_DisplayData->st_Camera);

    if(_pst_Sel->l_Flag & SEL_C_SIF_ZDx)
    {
        pst_ZDx = (COL_tdst_ZDx *) _pst_Sel->p_Content;
        p_Shape = pst_ZDx->p_Shape;
        pst_Cob = NULL;

        if
        (
            (!COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_Specific)) &&
            (pst_WinHandles->pst_World->pst_Selection->l_Flag != SEL_C_SF_Editable)
        ) return TRUE;
    }
    else
    {
        pst_ZDx = NULL;
        pst_Cob = (COL_tdst_Cob *) _pst_Sel->p_Content;
        if(!COL_b_Cob_TestType(pst_Cob, COL_C_Zone_Triangles))
            p_Shape = pst_Cob->pst_MathCob->p_Shape;
		else
			return TRUE;
    }

    MATH_TransformVector(&st_Move, &pst_Camera->st_Matrix, pst_Vector);

    b_LocalMode = TRUE;

    if(b_LocalMode)
    {
        MATH_InvertMatrix(&st_Matrix, OBJ_pst_GetAbsoluteMatrix(pst_Zone_GO));
        MATH_TransformVector(&st_Move, &st_Matrix, &st_Move);
    }

    ul_Type = pst_ZDx ? COL_Zone_GetType(pst_ZDx) : COL_Cob_GetType(pst_Cob);
    switch(ul_Type)
    {
    case COL_C_Zone_Sphere:
        MATH_SubEqualVector(COL_pst_Shape_GetCenter(p_Shape), &st_Move);
        break;

    case COL_C_Zone_Box:
        MATH_SubEqualVector(COL_pst_Shape_GetMin(p_Shape), &st_Move);
        MATH_SubEqualVector(COL_pst_Shape_GetMax(p_Shape), &st_Move);
        break;

    case COL_C_Zone_Cylinder:
        MATH_SubEqualVector(COL_pst_Shape_GetCenter(p_Shape), &st_Move);
        break;

    case COL_C_Zone_Cone:
        break;
    }

    return TRUE;
}
/**/
void F3D_cl_View::Selection_Zone_Move(MATH_tdst_Vector *_pst_Move)
{
    SEL_EnumItem
    (
        M_F3D_Sel,
        SEL_C_SIF_ZDx | SEL_C_SIF_Cob,
        F3D_b_MoveZoneCameraAxis,
        (ULONG) _pst_Move,
        (ULONG) & mst_WinHandles
    );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_ScaleZone(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float               f_Zoom;
    OBJ_tdst_GameObject *pst_Zone_GO;
    COL_tdst_ZDx        *pst_ZDx;
    COL_tdst_Cob        *pst_Cob;
    MATH_tdst_Vector    *pst_Scale;
    MAI_tdst_WinHandles *pst_WinHandles;
    ULONG               ul_Type;
    void                *p_Shape;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_WinHandles = (MAI_tdst_WinHandles *) _ul_Param2;
    pst_Zone_GO = (OBJ_tdst_GameObject *) _pst_Sel->ul_User;

    pst_Scale = (MATH_tdst_Vector *) _ul_Param1;

    if(_pst_Sel->l_Flag & SEL_C_SIF_ZDx)
    {
        pst_ZDx = (COL_tdst_ZDx *) _pst_Sel->p_Content;
        p_Shape = pst_ZDx->p_Shape;
        pst_Cob = NULL;

        if
        (
            (!COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_Specific)) &&
            (pst_WinHandles->pst_World->pst_Selection->l_Flag != SEL_C_SF_Editable)
        ) return TRUE;
    }
    else
    {
        pst_ZDx = NULL;
        pst_Cob = (COL_tdst_Cob *) _pst_Sel->p_Content;
        if(!COL_b_Cob_TestType(pst_Cob, COL_C_Zone_Triangles))
            p_Shape = pst_Cob->pst_MathCob->p_Shape;
		else
			return TRUE;
    }

    ul_Type = pst_ZDx ? COL_Zone_GetType(pst_ZDx) : COL_Cob_GetType(pst_Cob);
    switch(ul_Type)
    {
    case COL_C_Zone_Sphere:
        f_Zoom = fMax3(pst_Scale->x, pst_Scale->y, pst_Scale->z);
        if(f_Zoom == 1.0f) f_Zoom = fMin3(pst_Scale->x, pst_Scale->y, pst_Scale->z);
        COL_Shape_SetRadius(p_Shape, COL_f_Shape_GetRadius(p_Shape) * f_Zoom);
        break;

    case COL_C_Zone_Box:
        MATH_tdst_Vector * pst_Min, *pst_Max;

        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        MATH_tdst_Vector    st_Center, st_HalfDiag;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        pst_Min = COL_pst_Shape_GetMin(p_Shape);
        pst_Max = COL_pst_Shape_GetMax(p_Shape);

        MATH_SubVector(&st_HalfDiag, pst_Max, pst_Min);
        MATH_MulEqualVector(&st_HalfDiag, 0.5f);

        MATH_AddVector(&st_Center, pst_Min, &st_HalfDiag);

        st_HalfDiag.x = st_HalfDiag.x * pst_Scale->x;
        st_HalfDiag.y = st_HalfDiag.y * pst_Scale->y;
        st_HalfDiag.z = st_HalfDiag.z * pst_Scale->z;

        MATH_AddVector(pst_Max, &st_Center, &st_HalfDiag);
        MATH_SubVector(pst_Min, &st_Center, &st_HalfDiag);
        break;

    case COL_C_Zone_Cylinder:
        f_Zoom = fMax(pst_Scale->x, pst_Scale->y);
        if(f_Zoom <= 1.0f) f_Zoom = fMin(pst_Scale->x, pst_Scale->y);

        COL_Shape_SetRadius(p_Shape, COL_f_Shape_GetRadius(p_Shape) * f_Zoom);
        COL_Shape_SetHeight(p_Shape, COL_f_Shape_GetHeight(p_Shape) * pst_Scale->z);
        break;

    case COL_C_Zone_Cone:
        break;
    }

    return TRUE;
}
/**/
void F3D_cl_View::Selection_Zone_Scale(MATH_tdst_Vector *_pst_Move)
{
    SEL_EnumItem
    (
        M_F3D_Sel,
        SEL_C_SIF_ZDx | SEL_C_SIF_Cob,
        F3D_b_ScaleZone,
        (ULONG) _pst_Move,
        (ULONG) & mst_WinHandles
    );
}

/*$4
 ***********************************************************************************************************************
    Bounding volume Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_MoveBVCameraAxis(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    void                *pst_BV;
    OBJ_tdst_GameObject *pst_GO;
    MATH_tdst_Matrix    st_InvMatrix;
    MATH_tdst_Vector    *pst_Vector;
    MATH_tdst_Vector    st_GCS_Move, st_OCS_Move;
    CAM_tdst_Camera     *pst_Camera;
    MAI_tdst_WinHandles *pst_WinHandles;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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
		else
			return TRUE;
	}

    pst_BV = pst_GO->pst_BV;
    pst_Vector = (MATH_tdst_Vector *) _ul_Param1;
    pst_WinHandles = (MAI_tdst_WinHandles *) _ul_Param2;
    if (pst_WinHandles) 
		pst_Camera = (CAM_tdst_Camera *) (&pst_WinHandles->pst_DisplayData->st_Camera);

    OBJ_SetStatusFlag(pst_GO, OBJ_C_ControlFlag_EditableBV);
    
    if( pst_WinHandles )
		MATH_TransformVector(&st_GCS_Move, &pst_Camera->st_Matrix, pst_Vector);
	else
		MATH_CopyVector(&st_GCS_Move, pst_Vector);

    if(!(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV))
    {
        MATH_NegEqualVector(&st_GCS_Move);
        OBJ_BV_Move(pst_GO, &st_GCS_Move, TRUE);
    }
    else
    {
        MATH_InvertMatrix(&st_InvMatrix, OBJ_pst_GetAbsoluteMatrix(pst_GO));
        MATH_TransformVector(&st_OCS_Move, &st_InvMatrix, &st_GCS_Move);
        MATH_NegEqualVector(&st_OCS_Move);
        OBJ_BV_OBBoxMove(pst_GO, &st_OCS_Move);
    }

    return TRUE;
}
/**/
void F3D_cl_View::Selection_BV_Move(MATH_tdst_Vector *_pst_Move)
{
    SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_MoveBVCameraAxis, (ULONG) _pst_Move, (ULONG) & mst_WinHandles);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_ScaleBV(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float               f_Zoom, f_OldRadius;
    OBJ_tdst_GameObject *pst_GO;
    void                *pst_BV;
    MATH_tdst_Vector    *pst_Scale;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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
		else
			return TRUE;
	}

    pst_BV = pst_GO->pst_BV;
    pst_Scale = (MATH_tdst_Vector *) _ul_Param1;

    OBJ_SetStatusFlag(pst_GO, OBJ_C_ControlFlag_EditableBV);

    if(!(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV) || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_OBBox))
    {
        if(OBJ_BV_IsSphere(pst_BV))
        {
            f_Zoom = fMax3(pst_Scale->x, pst_Scale->y, pst_Scale->z);
            if(f_Zoom == 1.0f) f_Zoom = fMin3(pst_Scale->x, pst_Scale->y, pst_Scale->z);
            f_OldRadius = OBJ_f_BV_GetRadius(pst_BV);
            OBJ_v_BV_SetRadius(pst_BV, f_OldRadius * f_Zoom);
        }
        else
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            MATH_tdst_Vector    *pst_GMin, *pst_GMax;
            MATH_tdst_Vector    st_Center, st_HalfDiag;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

            pst_GMin = OBJ_pst_BV_GetGMinInit(pst_BV);
            pst_GMax = OBJ_pst_BV_GetGMaxInit(pst_BV);

            MATH_SubVector(&st_HalfDiag, pst_GMax, pst_GMin);
            MATH_MulEqualVector(&st_HalfDiag, 0.5f);

            MATH_AddVector(&st_Center, pst_GMin, &st_HalfDiag);

            st_HalfDiag.x = st_HalfDiag.x * pst_Scale->x;
            st_HalfDiag.y = st_HalfDiag.y * pst_Scale->y;
            st_HalfDiag.z = st_HalfDiag.z * pst_Scale->z;

            MATH_AddVector(pst_GMax, &st_Center, &st_HalfDiag);
            MATH_SubVector(pst_GMin, &st_Center, &st_HalfDiag);
			MATH_CopyVector(OBJ_pst_BV_GetGMin(pst_BV), pst_GMin);
			MATH_CopyVector(OBJ_pst_BV_GetGMax(pst_BV), pst_GMax);
        }
    }
    else
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        MATH_tdst_Vector    *pst_LMin, *pst_LMax;
        MATH_tdst_Vector    st_Center, st_HalfDiag;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        pst_LMin = OBJ_pst_BV_GetLMin(pst_BV);
        pst_LMax = OBJ_pst_BV_GetLMax(pst_BV);

        MATH_SubVector(&st_HalfDiag, pst_LMax, pst_LMin);
        MATH_MulEqualVector(&st_HalfDiag, 0.5f);

        MATH_AddVector(&st_Center, pst_LMin, &st_HalfDiag);

        st_HalfDiag.x = st_HalfDiag.x * pst_Scale->x;
        st_HalfDiag.y = st_HalfDiag.y * pst_Scale->y;
        st_HalfDiag.z = st_HalfDiag.z * pst_Scale->z;

        MATH_AddVector(pst_LMax, &st_Center, &st_HalfDiag);
        MATH_SubVector(pst_LMin, &st_Center, &st_HalfDiag);
    }

    return TRUE;
}
/**/
void F3D_cl_View::Selection_BV_Scale(MATH_tdst_Vector *_pst_Scale)
{
    SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_ScaleBV, (ULONG) _pst_Scale, (ULONG) & mst_WinHandles);
}

#endif

/*$T F3Dview_cam.cpp GC! 1.081 06/19/01 14:45:15 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIapp.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dstrings.h"
#include "F3Dframe/F3Dview_undo.h"
#include "CAMera/CAMera.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "INOut/INOkeyboard.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/ENGcall.h"
#include "ENGine/Sources/WORld/WORrender.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "GEOmetric/GEOdebugobject.h"
#include "LIGHT/LIGHTstruct.h"
#include "GraphicDK/Sources/CAMera/CAMstruct.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "SELection/SELection.h"
#include "Res/Res.h"
#include "BASe/MEMory/MEM.h"
#include "EDIpaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "MATHs/MATH.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#define EOUT_INFINITE	10.0e37f

/*$4
 ***********************************************************************************************************************
    Camera
 ***********************************************************************************************************************
 */

#define RECT(pDC, o) \
	{ \
		pDC->MoveTo(o.left, o.top); \
		pDC->LineTo(o.right, o.top); \
		pDC->LineTo(o.right, o.bottom); \
		pDC->LineTo(o.left, o.bottom); \
		pDC->LineTo(o.left, o.top); \
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::TestLockCam(void)
{
	if((mst_WinHandles.pst_DisplayData->uc_EngineCamera) || (mst_WinHandles.pst_DisplayData->uc_EditorCamera))
	{
		mst_WinHandles.pst_DisplayData->uc_EditorCamera = 0;
		mst_WinHandles.pst_DisplayData->uc_EngineCamera = 0;
		if(mpo_AssociatedEditor) mpo_AssociatedEditor->RefreshMenu();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::EndZoom(CPoint pt, BOOL _b_CanZoom)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC					*pDC;
	CPen				o_Pen;
	CPen				*poldpen;
	CRect				o_Rect;
	MATH_tdst_Vector	v, v1;
	float				fFactor, fX, fY, fZ;
	CAM_tdst_Camera		*pst_Cam;
	BOOL				bZoomOn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDC = GetDC();
	pDC->SetROP2(R2_XORPEN);

#ifdef JADEFUSION
	o_Pen.CreatePen(PS_SOLID, 1, GetSysColor(0x00ffffff));
#else
	o_Pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_ACTIVECAPTION));
#endif
	poldpen = pDC->SelectObject(&o_Pen);
	RECT(pDC, mo_ZoomRect);
	pDC->SelectObject(poldpen);
	DeleteObject(&o_Pen);
	ReleaseDC(pDC);
	mb_ZoomOn = FALSE;
	if(!_b_CanZoom) return;
	bZoomOn = TRUE;
	if(mo_ZoomRect.bottom <= mo_ZoomRect.top) bZoomOn = FALSE;
	mo_ZoomRect.NormalizeRect();
	GetClientRect(&o_Rect);
	pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;

	/* Rect */
	v.x = (float) mo_ZoomRect.left;
	v.x = (v.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width);
	v.y = (float) mo_ZoomRect.top;
	v.y = 1.0f - (v.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
	v1.x = (float) mo_ZoomRect.right;
	v1.x = (v1.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width);
	v1.y = (float) mo_ZoomRect.bottom;
	v1.y = 1.0f - (v1.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
	if(b_PickOozRect(&v, &v1, &v.z) && (v.z != 0))
		fFactor = 1.0f / v.z;
	else
		fFactor = MATH_f_Distance(MATH_pst_GetTranslation(&pst_Cam->st_Matrix), &pst_Cam->st_Target);

	/* Compute move point (center of rect) */
	v.x = (float) mo_ZoomRect.left + ((mo_ZoomRect.right - mo_ZoomRect.left) / 2);
	v.y = (float) mo_ZoomRect.top + ((mo_ZoomRect.bottom - mo_ZoomRect.top) / 2);
	v.x -= (o_Rect.right / 2);
	v.y -= (o_Rect.bottom / 2);

	if(pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
	{
		v.x *= fFactor / pst_Cam->f_FactorX;
		v.y *= fFactor / pst_Cam->f_FactorY;
	}
	else
	{
		v.x /= pst_Cam->f_FactorX;
		v.y /= pst_Cam->f_FactorY;
	}

	/* Compute scale factor */
	if(mo_ZoomRect.Width() > mo_ZoomRect.Height())
	{
		if(mo_ZoomRect.Width() < 20) mo_ZoomRect.right = mo_ZoomRect.left + 20;
		v.z = (float) (mo_ZoomRect.right - mo_ZoomRect.left) / (o_Rect.right - o_Rect.left);
	}
	else
	{
		if(mo_ZoomRect.Height() < 20) mo_ZoomRect.bottom = mo_ZoomRect.top + 20;
		v.z = (float) (mo_ZoomRect.bottom - mo_ZoomRect.top) / (o_Rect.bottom - o_Rect.top);
	}

	fZ = v.z;

	fFactor = (1.0f - v.z) * fFactor;
	v.z = 0;
	CAM_Move(pst_Cam, &v);
	fX = v.x, fY = v.y;

	v.x = v.y = 0;
	if(pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
	{
		v.z = fFactor;
		if(!bZoomOn) v.z = -v.z;
		CAM_Move(pst_Cam, &v);
	}
	else
	{
		v.z = fFactor;
		if(!bZoomOn) v.z = -v.z;
		if(bZoomOn)
			pst_Cam->f_IsoZoom *= fZ;
		else
			pst_Cam->f_IsoZoom /= fZ;
		CAM_UpdateIsometricFactor(pst_Cam, &v, 0);
		CAM_Move(pst_Cam, &v);
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::Viewpoint_b_MoveToCamera(long _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_First, *pst_Last;
	OBJ_tdst_GameObject		*pst_GO;
	GRO_tdst_Struct			*pst_GRO;
	MATH_tdst_Matrix		st_Matrix;
	CAM_tdst_CameraObject	*pst_CamObj;
	CAM_tdst_Camera			*pst_Cam;
	EOUT_cl_Frame			*po_Out;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return FALSE;
	TestLockCam();

	/* Get out editor */
	po_Out = (EOUT_cl_Frame *) mpo_AssociatedEditor;
	pst_First = TAB_pst_PFtable_GetFirstElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	pst_Last = TAB_pst_PFtable_GetLastElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	for(; pst_First <= pst_Last; pst_First++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_First->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
		{
			pst_GRO = pst_GO->pst_Base->pst_Visu->pst_Object;
			if((pst_GRO) && (pst_GRO->i->ul_Type == GRO_Camera) && (!_l_Index--)) break;
		}
	}

	if(_l_Index != -1) return FALSE;
	MATH_CopyMatrix(&st_Matrix, pst_GO->pst_GlobalMatrix);
	if(po_Out)
	{
		WOR_View_SetViewPoint
		(
			&mst_WinHandles.pst_World->pst_View[po_Out->mi_NumEdit],
			&st_Matrix,
			&MATH_gst_NulVector,
			TRUE,
			FALSE
		);
		mst_WinHandles.pst_World->pst_View[po_Out->mi_NumEdit].uc_Flags |= WOR_Cuc_View_RecomputeTables;
	}
	else
	{
		WOR_View_SetViewPoint(&mst_WinHandles.pst_World->pst_View[0], &st_Matrix, &MATH_gst_NulVector, TRUE, FALSE);
		mst_WinHandles.pst_World->pst_View[0].uc_Flags |= WOR_Cuc_View_RecomputeTables;
	}

	CAM_SetCameraMatrix(&mst_WinHandles.pst_DisplayData->st_Camera, &st_Matrix);
	pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
	pst_CamObj = (CAM_tdst_CameraObject *) pst_GRO;
	pst_Cam->f_FieldOfVision = pst_CamObj->f_FieldOfVision;
	if(po_Out)
	{
		mst_WinHandles.pst_World->pst_View[po_Out->mi_NumEdit].st_DisplayInfo.f_FieldOfVision = pst_CamObj->f_FieldOfVision;
	}
	else
		mst_WinHandles.pst_World->pst_View[0].st_DisplayInfo.f_FieldOfVision = pst_CamObj->f_FieldOfVision;
	pst_Cam->f_NearPlane = pst_CamObj->f_NearPlane;
	pst_Cam->f_FarPlane = pst_CamObj->f_FarPlane;
	LINK_Refresh();
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Viewpoint_SetStandard(long _l_Type)
{
/*$off*/
	static MATH_tdst_Matrix sgast_Mat[ 6 ] =
	{
		{-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, {0,0, 10}, 0 },
		{-1, 0, 0, 0, 0, 0,-1, 0, 0,-1, 0, 0, {0,0,-10}, 0 },
		{ 0, 1, 0, 0,-1, 0, 0, 0, 0, 0, 1, 0, {-10,0,0}, 0 },
		{ 0,-1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, { 10,0,0}, 0 },
		{-1, 0, 0, 0, 0,-1, 0, 0, 0, 0, 1, 0, {0,-10,0}, 0 },
		{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, {0, 10,0}, 0 }
	};
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/*$on*/
	MATH_tdst_Matrix		st_Matrix, *pst_CamMatrix;
	OBJ_tdst_GameObject			*pst_GO, *pst_Father;
	MATH_tdst_Vector			st_Pos;

	if((unsigned long) _l_Type >= 7) return;
	if(!mst_WinHandles.pst_World) return;
	TestLockCam();
	pst_CamMatrix = &mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix;
	if(_l_Type == F3D_StdView_Origin)
	{
		mst_WinHandles.pst_World->pst_View[0].uc_Flags |= WOR_Cuc_View_RecomputeTables;
		MATH_SetIdentityMatrix(pst_CamMatrix);
		MATH_InitVectorToZero(&pst_CamMatrix->T);
	}
	else
	{
		MATH_SetIdentityMatrix(&st_Matrix);
		MATH_Copy33Matrix(&st_Matrix, &sgast_Mat[_l_Type]);
		pst_GO = Selection_pst_GetFirstObject();
		if(pst_GO == NULL)
		{
			MATH_SetTranslation(&st_Matrix, MATH_pst_GetTranslation(pst_CamMatrix));
		}
		else
		{
			if(ANI_b_IsGizmoAnim(pst_GO, &pst_Father))
			{
				MATH_AddVector(&st_Pos, OBJ_pst_GetAbsolutePosition(pst_Father), &sgast_Mat[_l_Type].T);
			}
			else
			{
				MATH_AddVector(&st_Pos, OBJ_pst_GetAbsolutePosition(pst_GO), &sgast_Mat[_l_Type].T);
			}

			MATH_SetTranslation(&st_Matrix, &st_Pos);
		}

		CAM_SetCameraMatrix(&mst_WinHandles.pst_DisplayData->st_Camera, &st_Matrix);

		/* If an object is selected, zoom extent... */
		if(Selection_pst_GetFirstObject())
		{
			ZoomExtendSelected(&mst_WinHandles.pst_DisplayData->st_Camera, mst_WinHandles.pst_DisplayData->pst_World);
		}
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *F3D_cl_View::Viewpoint_sz_GetName(long _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_First, *pst_Last;
	OBJ_tdst_GameObject *pst_GO;
	GRO_tdst_Struct		*pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return NULL;
	if(mst_WinHandles.pst_World->st_AllWorldObjects.ul_NbElems == 0) return NULL;
	pst_First = TAB_pst_PFtable_GetFirstElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	if(!pst_First) return NULL;
	pst_Last = TAB_pst_PFtable_GetLastElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	for(; pst_First <= pst_Last; pst_First++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_First->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
		{
			pst_GRO = pst_GO->pst_Base->pst_Visu->pst_Object;
			if(pst_GRO && pst_GRO->i->ul_Type == GRO_Camera)
			{
				if(!_l_Index--) return pst_GO->sz_Name;
			}
		}
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Viewpoint_Next(void)
{
	mi_CurCamera++;
	if(!Viewpoint_b_MoveToCamera(mi_CurCamera))
	{
		mi_CurCamera = 0;
		Viewpoint_b_MoveToCamera(0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_BoundObject(void *_p_Node, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*pst_Min;
	MATH_tdst_Vector	*pst_Max;
	MATH_tdst_Vector	*pst_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _p_Node;
	pst_Min = (MATH_tdst_Vector *) _ul_Param1;
	pst_Max = (MATH_tdst_Vector *) _ul_Param2;
	pst_Pos = MATH_pst_GetTranslation(pst_GO->pst_GlobalMatrix);
	if(pst_Min->x > pst_Pos->x) pst_Min->x = pst_Pos->x;
	if(pst_Min->y > pst_Pos->y) pst_Min->y = pst_Pos->y;
	if(pst_Min->z > pst_Pos->z) pst_Min->z = pst_Pos->z;
	if(pst_Max->x < pst_Pos->x) pst_Max->x = pst_Pos->x;
	if(pst_Max->y < pst_Pos->y) pst_Max->y = pst_Pos->y;
	if(pst_Max->z < pst_Pos->z) pst_Max->z = pst_Pos->z;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::RotateCameraAroundTarget(MATH_tdst_Vector *_pst_Rotate)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CAM_tdst_Camera		*pst_Cam;
	MATH_tdst_Vector	v, v1;
	POINT				point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_ScaleVector(_pst_Rotate, _pst_Rotate, 0.01f);
	pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
	if(pst_Cam->ul_Flags & CAM_Cul_Flags_RotateXLocked) _pst_Rotate->x = 0;
	if(pst_Cam->ul_Flags & CAM_Cul_Flags_RotateYLocked) _pst_Rotate->y = 0;
	if(pst_Cam->ul_Flags & CAM_Cul_Flags_RotateZLocked) _pst_Rotate->z = 0;
	if(mst_WinHandles.pst_World != NULL)
	{
		if(!mb_BeginModeRotate)
		{
			if (
					(GetAsyncKeyState( VK_SHIFT ) < 0 )
					&& (M_F3D_Helpers->st_Center.z > 0.1)
					&& (M_F3D_Helpers->st_Center2D.x > 0)
					&& (M_F3D_Helpers->st_Center2D.x < mst_WinHandles.pst_DisplayData->st_Device.l_Width)
					&& (M_F3D_Helpers->st_Center2D.y > 0)
					&& (M_F3D_Helpers->st_Center2D.y < mst_WinHandles.pst_DisplayData->st_Device.l_Height)
				)
			{
				MATH_CopyVector( &v, &M_F3D_Helpers->st_Center2D );
				v.y = mst_WinHandles.pst_DisplayData->st_Device.l_Height - v.y;
				v.z = M_F3D_Helpers->st_Center.z;
				CAM_2Dto3D(pst_Cam, &v1, &v);
				MATH_CopyVector(&pst_Cam->st_Target, &v1);
				mb_BeginModeRotate = TRUE;
			}
			else
			{
				GetCursorPos(&point);
				ScreenToClient(&point);
				v.x = (float) point.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width;
				v.y = 1.0f - ((float) point.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
				b_PickDepth(&v, &v.z);
				v.x = (float) point.x;
				v.y = (float) point.y;
				CAM_2Dto3D(pst_Cam, &v1, &v);
				MATH_CopyVector(&pst_Cam->st_Target, &v1);
   				mb_BeginModeRotate = TRUE;
			}
		}
	}
	else
		MATH_InitVectorToZero(&pst_Cam->st_Target);
	CAM_RotateAroundTarget(pst_Cam, _pst_Rotate);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::MoveCamera(MATH_tdst_Vector *_pst_Move, char c_UseZ)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	CAM_tdst_Camera *pst_Cam;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
	if(_pst_Move->z == 0)
	{
		MATH_ScaleVector(_pst_Move, _pst_Move, 0.2f);
		CAM_Move(pst_Cam, _pst_Move);
	}
	else
	{
		if(pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
		{
			_pst_Move->x = _pst_Move->x * _pst_Move->z / pst_Cam->f_FactorX;
			_pst_Move->y = _pst_Move->y * _pst_Move->z / pst_Cam->f_FactorY;
			CAM_Move(pst_Cam, _pst_Move);
		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_V;
			/*~~~~~~~~~~~~~~~~~~~~~*/

            /*if (c_UseZ)
                MATH_CopyVector( &st_V, _pst_Move );
            else
            */
            {
    			st_V.x = _pst_Move->x / (pst_Cam->f_FactorX);
	    		st_V.y = _pst_Move->y / (pst_Cam->f_FactorY);
		    	st_V.z = _pst_Move->z;
            }
			CAM_Move(pst_Cam, &st_V);

			/* st_V.x = st_V.y = 0; st_V.z = _pst_Move->z; st_V.z = _pst_Move->z / 10.0f; */
			CAM_UpdateIsometricFactor(pst_Cam, &st_V, c_UseZ);

			_pst_Move->x = -_pst_Move->x / pst_Cam->f_FactorX;
			_pst_Move->y = -_pst_Move->y / pst_Cam->f_FactorY;
			_pst_Move->z = 0;
			CAM_Move(pst_Cam, _pst_Move);
		}
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Cam_FocalZoom(CPoint _o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_NumView;
	EOUT_cl_Frame		*po_Out;
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	TestLockCam();

	/* Get out editor */
	po_Out = (EOUT_cl_Frame *) mpo_AssociatedEditor;
	i_NumView = (po_Out) ? po_Out->mi_NumEdit : 0;
	pst_World = mst_WinHandles.pst_World;
	if(!mb_BeginModePane)
	{
		ScreenToClient(&_o_Pt);
		v.x = ((float) _o_Pt.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width);
		v.y = 1.0f - ((float) _o_Pt.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
		b_PickDepth(&v, &mf_PickedDepth);
		mf_Zintegration = mf_PickedDepth;
		mf_OriginalAlpha = pst_World->pst_View[i_NumView].st_DisplayInfo.f_FieldOfVision;
		mf_OriginalAlpha = fTan(mf_OriginalAlpha * 0.5f) * mf_Zintegration;
		mb_BeginModePane = TRUE;
		ClientToScreen(&_o_Pt);
	}

	::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PERSPECTIVE));
	v.x = 0.0f;
	v.y = 0.0f;
	v.z = (float) (mi_MousePosY - _o_Pt.y);
	v.z *= mf_PickedDepth / mst_WinHandles.pst_DisplayData->st_Camera.f_FactorX;
	mf_Zintegration -= v.z;
	if((mf_OriginalAlpha / mf_Zintegration) > 3.0f)
	{
		mf_Zintegration = mf_OriginalAlpha / 3.0f;
		v.z = 0.0f;
	}

	pst_World->pst_View[i_NumView].st_DisplayInfo.f_FieldOfVision = 2.0f * fAtan(mf_OriginalAlpha / mf_Zintegration);
//	CAM_Move(&mst_WinHandles.pst_DisplayData->st_Camera, &v);
	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::CameraAsObj(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = Selection_pst_GetFirstObject();
	if(!pst_GO) return;
	CAM_SetCameraMatrix(&mst_WinHandles.pst_DisplayData->st_Camera, pst_GO->pst_GlobalMatrix);

	mst_WinHandles.pst_DisplayData->pst_EditorCamObject = pst_GO;
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::ObjAsCamera(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = Selection_pst_GetFirstObject();
	if(!pst_GO) return;

	mpo_CurrentModif = new F3D_cl_Undo_HelperModif(this, FALSE);
	mpo_CurrentModif->SetDesc("Set object as camera");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	CAM_SetObjectMatrixFromCam(pst_GO->pst_GlobalMatrix, &mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix);
	OBJ_ComputeLocalWhenHie(pst_GO);
	mst_WinHandles.pst_DisplayData->pst_EditorCamObject = pst_GO;
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::CreateCamAtCam(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO;
	GRO_tdst_Visu			*pst_Visu;
	CAM_tdst_CameraObject	*pst_Cam;
	char					asz_Path[BIG_C_MaxLenPath];
	BIG_INDEX				ul_Index;
	MATH_tdst_Matrix		st_Matrix;
	unsigned long			ul_Flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = CreateWaypoint(CPoint(0, 0), "Camera" );
	if(!pst_GO) return;
	ul_Flags = OBJ_ul_FlagsIdentityGet(pst_GO);
	ul_Flags |= OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_HasInitialPos;
	ul_Flags &= ~OBJ_C_IdentityFlag_Waypoints;
	OBJ_ChangeIdentityFlags(pst_GO, ul_Flags, OBJ_ul_FlagsIdentityGet(pst_GO));
	CAM_SetObjectMatrixFromCam(&st_Matrix, &mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix);
	OBJ_SetInitialAbsoluteMatrix(pst_GO, &st_Matrix);
	OBJ_RestoreInitialPos(pst_GO);
	pst_Visu = (GRO_tdst_Visu *) pst_GO->pst_Base->pst_Visu;
	pst_Visu->pst_Object = (GRO_tdst_Struct *) CAM_pst_Create();
	pst_Visu->pst_Object->i->pfn_AddRef(pst_Visu->pst_Object, 1);
	pst_Cam = (CAM_tdst_CameraObject *) pst_Visu->pst_Object;
	pst_Cam->f_NearPlane = mst_WinHandles.pst_DisplayData->st_Camera.f_NearPlane;
	pst_Cam->f_FarPlane = mst_WinHandles.pst_DisplayData->st_Camera.f_FarPlane;
	pst_Cam->f_FieldOfVision = mst_WinHandles.pst_DisplayData->st_Camera.f_FieldOfVision;
	pst_Cam->ul_Flags = CAM_Cul_Flags_Perspective;

	/* Create a new file for graphic object */
	WOR_GetGroPath(pst_GO->pst_World, asz_Path);
	ul_Index = GRO_ul_Struct_FullSave(pst_Visu->pst_Object, asz_Path, NULL, NULL);
	LINK_Refresh();

	/* add new object into table */
	TAB_Ptable_AddElemAndResize(&M_F3D_World->st_GraphicObjectsTable, pst_Visu->pst_Object);
	pst_Visu->pst_Object->i->pfn_AddRef(pst_Visu->pst_Object, 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::ZoomOnObject(CAM_tdst_Camera *pst_Camera, MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max)
{
	if(!(pst_Camera->ul_Flags & CAM_Cul_Flags_Perspective))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Diag, st_Camera_Move, st_Global_Move;
		float				f_Max;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_InvertMatrix(&pst_Camera->st_InverseMatrix, &pst_Camera->st_Matrix);

		MATH_SubVector(&st_Diag, _pst_Max, _pst_Min);

		f_Max = (st_Diag.x > st_Diag.y) ? st_Diag.x : st_Diag.y;
		pst_Camera->f_IsoZoom = f_Max / (2.0f * pst_Camera->f_IsoFactor);

		MATH_AddVector(&st_Camera_Move, _pst_Max, _pst_Min);
		MATH_ScaleEqualVector(&st_Camera_Move, 0.5f);

		MATH_TransformVector(&st_Global_Move, &pst_Camera->st_Matrix, &st_Camera_Move);
		MATH_AddEqualVector(&pst_Camera->st_Matrix.T, &st_Global_Move);
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Move;
		float				Radius, Alpha2;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_AddVector(&st_Move, _pst_Max, _pst_Min);
		MATH_ScaleVector(&st_Move, &st_Move, 0.5f);
		Radius = MATH_f_Distance(_pst_Max, _pst_Min) * 0.5f;
		pst_Camera->st_Matrix.T = st_Move;
		st_Move.x = 0.0f;
		if((pst_Camera->f_FieldOfVision <= 0.0f) || (pst_Camera->f_FieldOfVision > 3.1415f))
			pst_Camera->f_FieldOfVision = 1.0f;
		Alpha2 = pst_Camera->f_CenterX / fTan(pst_Camera->f_FieldOfVision * 0.5f);
		Alpha2 = fAtan(pst_Camera->f_CenterY / Alpha2);
		st_Move.y = -Radius / fSin(Alpha2);
		st_Move.z = -Radius / fSin(pst_Camera->f_FieldOfVision * 0.5f);
		if(st_Move.y < st_Move.z) st_Move.z = st_Move.y;
		st_Move.y = 0.0f;
		CAM_Move(pst_Camera, &st_Move);
		CAM_UpdateIsometricFactor(pst_Camera, &st_Move, 0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OUT_b_ComputeSelectedBoudingVolume(void *_p_Node, long _l_Flag, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*pst_VectorMin;
	MATH_tdst_Vector	*pst_VectorMax;
	MATH_tdst_Vector	st_Box;
	unsigned long		Counter;
	OBJ_tdst_SingleBV	*pst_BV;
	float				f_Radius;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _p_Node;
	if(!pst_GO->pst_BV) return TRUE;
	pst_VectorMin = (MATH_tdst_Vector *) _ul_Param1;
	pst_VectorMax = (MATH_tdst_Vector *) _ul_Param2;
	if(OBJ_BV_IsAABBox(pst_GO->pst_BV))
	{
		pst_BV = (OBJ_tdst_SingleBV *) pst_GO->pst_BV;
		for(Counter = 0; Counter < 8; Counter++)
		{
			st_Box.x = (Counter & 1) ? pst_BV->st_GMin.x : pst_BV->st_GMax.x;
			st_Box.y = (Counter & 2) ? pst_BV->st_GMin.y : pst_BV->st_GMax.y;
			st_Box.z = (Counter & 4) ? pst_BV->st_GMin.z : pst_BV->st_GMax.z;
			MATH_AddVector(&st_Box, &st_Box, &pst_GO->pst_GlobalMatrix->T);
			if(st_Box.x > pst_VectorMax->x) pst_VectorMax->x = st_Box.x;
			if(st_Box.y > pst_VectorMax->y) pst_VectorMax->y = st_Box.y;
			if(st_Box.z > pst_VectorMax->z) pst_VectorMax->z = st_Box.z;
			if(st_Box.x < pst_VectorMin->x) pst_VectorMin->x = st_Box.x;
			if(st_Box.y < pst_VectorMin->y) pst_VectorMin->y = st_Box.y;
			if(st_Box.z < pst_VectorMin->z) pst_VectorMin->z = st_Box.z;
		}
	}
	else
	{
		pst_BV = (OBJ_tdst_SingleBV *) pst_GO->pst_BV;
		if(OBJ_f_BV_GetRadius(pst_GO) != 0.0f)
		{
			f_Radius = OBJ_f_BV_GetRadius(pst_BV);
			MATH_CopyVector(&st_Box, &pst_GO->pst_GlobalMatrix->T);
			pst_BV->st_GMax.x *= 0.5f;
			if(st_Box.x + f_Radius > pst_VectorMax->x) pst_VectorMax->x = st_Box.x + f_Radius;
			if(st_Box.y + f_Radius > pst_VectorMax->y) pst_VectorMax->y = st_Box.y + f_Radius;
			if(st_Box.z + f_Radius > pst_VectorMax->z) pst_VectorMax->z = st_Box.z + f_Radius;
			if(st_Box.x - f_Radius < pst_VectorMin->x) pst_VectorMin->x = st_Box.x - f_Radius;
			if(st_Box.y - f_Radius < pst_VectorMin->y) pst_VectorMin->y = st_Box.y - f_Radius;
			if(st_Box.z - f_Radius < pst_VectorMin->z) pst_VectorMin->z = st_Box.z - f_Radius;
			pst_BV->st_GMax.x *= 2.0f;
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_AddBoundingVolume(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ANI_b_IsGizmoAnim((OBJ_tdst_GameObject *) _pst_Sel->p_Content, &pst_Father))
		return OUT_b_ComputeSelectedBoudingVolume(pst_Father, 0, _ul_Param1, _ul_Param2);
	else
		return OUT_b_ComputeSelectedBoudingVolume(_pst_Sel->p_Content, 0, _ul_Param1, _ul_Param2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_Add2DBoundingVolume
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max,
	CAM_tdst_Camera		*_pst_Camera
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_SingleBV	*pst_BV;
	MATH_tdst_Vector	st_Camera_Point, st_Global_Point;
	int					Counter;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BV = (OBJ_tdst_SingleBV *) _pst_GO->pst_BV;

	for(Counter = 0; Counter < 8; Counter++)
	{
		st_Global_Point.x = (Counter & 1) ? pst_BV->st_GMin.x : pst_BV->st_GMax.x;
		st_Global_Point.y = (Counter & 2) ? pst_BV->st_GMin.y : pst_BV->st_GMax.y;
		st_Global_Point.z = (Counter & 4) ? pst_BV->st_GMin.z : pst_BV->st_GMax.z;
		MATH_AddVector(&st_Global_Point, &st_Global_Point, &_pst_GO->pst_GlobalMatrix->T);

		MATH_TransformVertex(&st_Camera_Point, &_pst_Camera->st_InverseMatrix, &st_Global_Point);

		_pst_Min->x = (st_Camera_Point.x < _pst_Min->x) ? st_Camera_Point.x : _pst_Min->x;
		_pst_Min->y = (st_Camera_Point.y < _pst_Min->y) ? st_Camera_Point.y : _pst_Min->y;

		_pst_Max->x = (st_Camera_Point.x > _pst_Max->x) ? st_Camera_Point.x : _pst_Max->x;
		_pst_Max->y = (st_Camera_Point.y > _pst_Max->y) ? st_Camera_Point.y : _pst_Max->y;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::ZoomOnObj(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Min, st_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st_Min.x = st_Min.y = st_Min.z = EOUT_INFINITE;
	st_Max.x = st_Max.y = st_Max.z = -EOUT_INFINITE;
	if(!(mst_WinHandles.pst_DisplayData->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective))
		F3D_Add2DBoundingVolume(_pst_GO, &st_Min, &st_Max, &mst_WinHandles.pst_DisplayData->st_Camera);
	else
		OUT_b_ComputeSelectedBoudingVolume((void *) _pst_GO, 0, (ULONG) & st_Min, (ULONG) & st_Max);
	ZoomOnObject(&mst_WinHandles.pst_DisplayData->st_Camera, &st_Min, &st_Max);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Selection_Add2DBoundingVolume(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	CAM_tdst_Camera		*pst_Camera;
	MATH_tdst_Vector	*pst_Min, *pst_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Camera = &((GDI_tdst_DisplayData *) GDI_gpst_CurDD)->st_Camera;

	pst_Min = (MATH_tdst_Vector *) _ul_Param1;
	pst_Max = (MATH_tdst_Vector *) _ul_Param2;

	F3D_Add2DBoundingVolume(pst_GO, pst_Min, pst_Max, pst_Camera);

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::ZoomExtendSelected(CAM_tdst_Camera *pst_Camera, WOR_tdst_World *pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Min, st_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !pst_World ) return;
	
	TestLockCam();
	st_Min.x = st_Min.y = st_Min.z = EOUT_INFINITE;
	st_Max.x = st_Max.y = st_Max.z = -EOUT_INFINITE;

	MATH_InvertMatrix(&pst_Camera->st_InverseMatrix, &pst_Camera->st_Matrix);

	if(!(pst_Camera->ul_Flags & CAM_Cul_Flags_Perspective))
	{
		SEL_EnumItem
		(
			pst_World->pst_Selection,
			SEL_C_SIF_Object,
			F3D_b_Selection_Add2DBoundingVolume,
			(ULONG) & st_Min,
			(ULONG) & st_Max
		);
	}
	else
	{
		SEL_EnumItem
		(
			pst_World->pst_Selection,
			SEL_C_SIF_Object,
			F3D_b_AddBoundingVolume,
			(ULONG) & st_Min,
			(ULONG) & st_Max
		);
	}

	if(st_Min.x != EOUT_INFINITE)
	{
		ZoomOnObject(pst_Camera, &st_Min, &st_Max);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::ZoomExtendAll(CAM_tdst_Camera *pst_Camera, WOR_tdst_World *pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	MATH_tdst_Vector	st_Min, st_Max;
	OBJ_tdst_GameObject *pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	TestLockCam();

	st_Min.x = st_Min.y = st_Min.z = EOUT_INFINITE;
	st_Max.x = st_Max.y = st_Max.z = -EOUT_INFINITE;
	
	MATH_InvertMatrix(&pst_Camera->st_InverseMatrix, &pst_Camera->st_Matrix);
	
	pst_Elem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_VisibleObjects);
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&pst_World->st_VisibleObjects);
	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
		pst_Obj = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_Obj)) continue;
		
		if(pst_Obj->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInvisibleInit) continue;
		if(pst_Obj->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInvisible) continue;
		if(pst_Obj->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims) continue;
		if(pst_Obj->c_FixFlags & OBJ_C_HasBeenMerge) continue;
		if(!pst_Obj->pst_Base) continue;
		if(!pst_Obj->pst_Base->pst_Visu) continue;
		
		if(!(pst_Camera->ul_Flags & CAM_Cul_Flags_Perspective))
			F3D_Add2DBoundingVolume(pst_Obj, &st_Min, &st_Max, pst_Camera);
		else
			OUT_b_ComputeSelectedBoudingVolume(pst_Elem->p_Pointer, 0, (ULONG) & st_Min, (ULONG) & st_Max);
	}
		
	if(st_Min.x != EOUT_INFINITE)
	{
		ZoomOnObject(pst_Camera, &st_Min, &st_Max);
	}

	LINK_Refresh();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
#ifdef JADEFUSION
void F3D_cl_View::SetCameraMatrix(MATH_tdst_Matrix* _pst_Matrix)
{
	//we don't want to call CAM_SetCameraMatrix since it swap Z and Y axes
	//our matrix come from a camera matrix
	if(_pst_Matrix)
		MATH_CopyMatrix(&mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix, _pst_Matrix);
}
#endif

#endif /* ACTIVE_EDITORS */

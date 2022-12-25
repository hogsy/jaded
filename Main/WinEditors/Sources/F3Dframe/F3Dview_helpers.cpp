/*$T F3Dview_helpers.cpp GC! 1.081 04/07/00 12:31:24 */


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
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/OBJects/OBJODE.h"

#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GAO.h"

#include "GEOmetric/GEOdebugobject.h"
#include "GraphicDK/Sources/CAMera/CAMstruct.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "Res/Res.h"
#include "BASe/MEMory/MEM.h"
#include "EDIpaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "MATHs/MATH.h"
#include "EDItors/Sources/OUTput/OUTframe.h"

extern void					EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);
/*$4
 ***********************************************************************************************************************
    Private function prototype
 ***********************************************************************************************************************
 */

void	BornMove(MATH_tdst_Vector *, float _f_Max);

/*$4
 ***********************************************************************************************************************
    Public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    To know if there's a helper under mouse
 =======================================================================================================================
 */
BOOL F3D_cl_View::Helper_b_IsOneUnderMouse(void)
{
	return M_F3D_Helpers->l_Pickable != -1;
}

/*
 =======================================================================================================================
    Aim:    To know if helper is being used
 =======================================================================================================================
 */
BOOL F3D_cl_View::Helper_b_IsInUse(void)
{
	return M_F3D_Helpers->l_Picked != -1;
}

/*
 =======================================================================================================================
    Aim:    To launch use of helper that is under mouse
 =======================================================================================================================
 */
void F3D_cl_View::Helper_Use(void)
{
	M_F3D_Helpers->l_Picked = M_F3D_Helpers->l_Pickable;
    
    MATH_CopyVector( &M_F3D_Helpers->st_StartPos, &M_F3D_Helpers->st_GlobalMatrix.T );

	/* curve */
	if((M_F3D_Helpers->l_Picked >= SOFT_Cl_Helpers_Curve1) && (M_F3D_Helpers->l_Picked <= SOFT_Cl_Helpers_Curve16))
	{
		mi_EditedCurve = M_F3D_Helpers->l_Picked - SOFT_Cl_Helpers_Curve1;
		Refresh();
		return;
	}
	
	/* portal */
	if((M_F3D_Helpers->l_Picked >= SOFT_Cl_Helpers_Portal) && (M_F3D_Helpers->l_Picked <= SOFT_Cl_Helpers_LastPortal))
	{
		if ( mpst_EditedPortal )
			mpst_EditedPortal->uw_Flags &= ~WOR_CF_PortalPicked;
		mpst_EditedPortal = M_F3D_Helpers->apst_Portal[ M_F3D_Helpers->l_Picked - SOFT_Cl_Helpers_Portal ];
		mpst_EditedPortal->uw_Flags |= WOR_CF_PortalPicked;
		Selection_b_Treat(NULL, 0, 0, FALSE );
		Refresh();
		return;
	}
	
	
}

/*
 =======================================================================================================================
    Aim:    To end use of helper
 =======================================================================================================================
 */
void F3D_cl_View::Helper_EndUse(void)
{
	M_F3D_Helpers->l_Picked = -1;
}

/*
 =======================================================================================================================
    Aim:    To know if helper is in translation mode
 =======================================================================================================================
 */
BOOL F3D_cl_View::Helper_b_IsInTranslateMode(void)
{
	return M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_Move;
}

/*
 =======================================================================================================================
    Aim:    Set helper axis (local global or camera)
 =======================================================================================================================
 */
void F3D_cl_View::Helper_SetAxis(long _l_Axis)
{
    if ( (M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_SAMask) == (ULONG) _l_Axis )
        return;

	M_F3D_Helpers->ul_Flags &= ~SOFT_Cul_HF_SAMask;
	M_F3D_Helpers->ul_Flags |= _l_Axis;
	switch(_l_Axis)
	{
	case SOFT_Cul_HF_SACamera:
		LINK_PrintStatusMsg(F3D_STR_Csz_CameraSystemAxis);
		break;
	case SOFT_Cul_HF_SAWorld:
		LINK_PrintStatusMsg(F3D_STR_Csz_GlobalSystemAxis);
		break;
	case SOFT_Cul_HF_SAObject:
		LINK_PrintStatusMsg(F3D_STR_Csz_LocalSystemAxis);
		break;
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    To know if given systemn axis is current one
 =======================================================================================================================
 */
BOOL F3D_cl_View::Helper_b_IsCurrentAxis(long _l_Axis)
{
	return M_F3D_Helpers->ul_Flags & _l_Axis;
}

/*
 =======================================================================================================================
    Aim:    To know if a system axis can be used with current helper mode
 =======================================================================================================================
 */
BOOL F3D_cl_View::Helper_b_AcceptAxis(long _l_Axis)
{
	if(M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_Scale) 
    {
        if(Selection_b_IsInSubObjectMode( TRUE ) )
            return TRUE;
        return(_l_Axis & SOFT_Cul_HF_SAObject);
    }
	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Set constraint plane (used for translate mode)
 =======================================================================================================================
 */
void F3D_cl_View::Helper_SetConstraint(long _l_Axis)
{
	if(M_F3D_Helpers->ul_Flags & _l_Axis) _l_Axis = 0;
	M_F3D_Helpers->ul_Flags &= ~SOFT_Cul_HF_ConstraintMask;
	M_F3D_Helpers->ul_Flags |= _l_Axis;
	switch(_l_Axis)
	{
	case 0:
		LINK_PrintStatusMsg(F3D_STR_Csz_ConstraintOther);
		break;
	case SOFT_Cul_HF_ConstraintYZ:
		LINK_PrintStatusMsg(F3D_STR_Csz_ConstraintX);
		break;
	case SOFT_Cul_HF_ConstraintXZ:
		LINK_PrintStatusMsg(F3D_STR_Csz_ConstraintY);
		break;
	case SOFT_Cul_HF_ConstraintXY:
		LINK_PrintStatusMsg(F3D_STR_Csz_ConstraintZ);
		break;
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    To know if given constraint is the current one
 =======================================================================================================================
 */
BOOL F3D_cl_View::Helper_b_IsCurrentConstraint(long _l_Axis)
{
	return M_F3D_Helpers->ul_Flags & _l_Axis;
}

/*
 =======================================================================================================================
    Aim:    Copy move description into given string (assume string is big enough)
 =======================================================================================================================
 */
long F3D_cl_View::Helper_l_GetDescription(char *_sz_Desc)
{
	return SOFT_l_Helpers_GetDescription(M_F3D_Helpers, M_F3D_Helpers->l_Pickable, _sz_Desc, &mst_Move);
}

/*
 =======================================================================================================================
    Aim:    Look under mouse to find an helper
 =======================================================================================================================
 */
int F3D_cl_View::Helper_i_Pick(CPoint _o_Point, BOOL _b_Start)
{
	/*~~~~~*/
	int x, y;
	/*~~~~~*/

	x = _o_Point.x;
	y = mst_WinHandles.pst_DisplayData->st_Device.l_Height - _o_Point.y;

	if(_b_Start)
		M_F3D_Helpers->l_Pickable = SOFT_l_Helpers_Pick(M_F3D_Helpers, x, y, -1);
	else
		SOFT_l_Helpers_Pick(M_F3D_Helpers, x, y, M_F3D_Helpers->l_Pickable);
	if(mi_FreezeHelper != -1) M_F3D_Helpers->l_Pickable = mi_FreezeHelper;

	return M_F3D_Helpers->l_Pickable;
}

/*
 =======================================================================================================================
    Aim:    Reset helpers
 =======================================================================================================================
 */
void F3D_cl_View::Helper_Reset(void)
{
	M_F3D_Helpers->l_Picked = M_F3D_Helpers->l_Pickable = -1;
	mi_FreezeHelper = -1;
}

/*
 =======================================================================================================================
    Aim:    Change helpers mode

    In:     _b_Up   TRUE to change to next mover, FALSE to change to previous mover
 =======================================================================================================================
 */
void F3D_cl_View::Helper_ChangeMode(BOOL _b_Up)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned long		ul_HF;
	char				*sz_Mode;
	OBJ_tdst_GameObject *pst_GO;
    BOOL                b_AllScale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_FreezeHelper = -1;

	/* Set possible mode mask */
	if
	(
		(mst_WinHandles.pst_DisplayData->uc_EditBounding)
	||	SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_ZDx)
	) ul_HF = SOFT_Cul_HF_MoveMask;
	else
	{
		if(SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob))
		{
			ul_HF = SOFT_Cul_HF_MoveMask | SOFT_Cul_HF_NoMovers;
		}
		else
		{
			pst_GO = Selection_pst_GetFirstObject();
			if(!pst_GO && !mpst_EditedPortal) return;
			ul_HF = SOFT_Cul_HF_MoveMask | SOFT_Cul_HF_NoMovers;
		}
	}

#ifdef JADEFUSION
	//disable scaling on lights
    pst_GO = (OBJ_tdst_GameObject*)SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
    
	if(pst_GO && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Lights))
	{
		ul_HF &= ~SOFT_Cul_HF_Scale;
	}
#endif

	/* Change mode */
    b_AllScale = Selection_b_IsInSubObjectMode( TRUE );
	SOFT_Helper_ChangeMoverMode(M_F3D_Helpers, _b_Up ? 1 : -1, ul_HF, b_AllScale );

	/* Write description */
	switch(M_F3D_Helpers->ul_Flags & (SOFT_Cul_HF_MoveMask | SOFT_Cul_HF_NoMovers))
	{
	case SOFT_Cul_HF_NoMovers:
		sz_Mode = F3D_STR_Csz_ObjectMode;
		break;
	case SOFT_Cul_HF_Rotate:
		sz_Mode = F3D_STR_Csz_RotationMode;
		break;
	case SOFT_Cul_HF_Scale:
		sz_Mode = F3D_STR_Csz_ScaleMode;
		break;
	case SOFT_Cul_HF_Move:
		sz_Mode = F3D_STR_Csz_TranslateMode;
		break;
	}

	LINK_PrintStatusMsg(sz_Mode);

	/* Refresh display and menu */
	LINK_Refresh();
	if(mpo_AssociatedEditor) mpo_AssociatedEditor->RefreshMenu();
}

/*
 =======================================================================================================================
    Aim:    Treat helper action
 =======================================================================================================================
 */
void F3D_cl_View::Helper_Treat(CPoint point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v, v1;
	char				sz_Text[100];
	POINT				pt;
	CRect				o_Rect;
	int					iCXScreen, iCYScreen;
	int					type, i;
	float				t;
	MATH_tdst_Vector	Axe;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(M_F3D_Helpers->l_Pickable == -1) return;

	Helper_i_Pick(point, 0);

	/* Save old move */
	MATH_CopyVector(&v, &mst_Move);

	/* Get new move */
	SOFT_Helpers_GetMove(M_F3D_Helpers, M_F3D_Helpers->l_Pickable, &mst_Move);

    if (MATH_b_NulVector( &mst_Move)) 
    {
        MATH_CopyVector( &mst_Move, &v);
        return;
    }
	type = M_F3D_Helpers->l_Pickable;

	if((type >= SOFT_Cl_Helpers_RotationLocalX) && (type <= SOFT_Cl_Helpers_RotationCameraZ))
	{
		MATH_SubVector(&v, &mst_Move, &v);

		SOFT_Helpers_GetAxe(M_F3D_Helpers, M_F3D_Helpers->l_Pickable, &Axe);

        if (SOFT_b_Helpers_UVGizmoIsOn( M_F3D_Helpers ) )
            SOFT_UVGizmo_Rotate( &M_F3D_Helpers->st_UVGizmo, &M_F3D_DD->st_Camera.st_Matrix, &Axe, &v );
		else if( mi_EditedCurve != -1)
			Curve_Rotate(&Axe, &v);
		else if( mpst_EditedPortal )
		{
			if ( M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_LocalMode )
				Portal_Rotate( &Axe, &mst_Move );
			else
				Portal_Rotate( &Axe, &v );
		}
		else
		{
            if ( (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_MovePivot) && Selection_b_IsInSubObjectMode(FALSE))
                Selection_SubObjectRotate(&v, !Selection_b_IsInSubObjectMode(TRUE) );
			else if(Selection_b_IsInSubObjectMode(TRUE))
				Selection_SubObjectRotate(&v, 0);
			else
			{
				if(GetAsyncKeyState(VK_CONTROL) < 0) MATH_ScaleEqualVector(&v, 0.005f);
				if(M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_LocalCenterForRotation)
					Selection_GAO_Rotate(NULL, &Axe, &v);
				else
					Selection_GAO_Rotate(&M_F3D_Helpers->st_Center, &Axe, &v);
			}
		}
	}
	else if((type >= SOFT_Cl_Helpers_MoveLocalX) && (type <= SOFT_Cl_Helpers_MoveCameraYZ))
	{
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		GetClientRect(&o_Rect);
		iCXScreen = GetSystemMetrics(SM_CXSCREEN);
		iCYScreen = GetSystemMetrics(SM_CYSCREEN);
		if(o_Rect.left < 2) o_Rect.left = 2;
		if(o_Rect.top < 2) o_Rect.top = 2;
		if(o_Rect.right >= iCXScreen - 1) o_Rect.right = iCXScreen - 2;
		if(o_Rect.bottom >= iCYScreen - 1) o_Rect.bottom = iCYScreen - 2;
		if(IsActionValidate(F3D_Action_CameraOn) || (!o_Rect.PtInRect(pt)))
		{
			if(pt.x < o_Rect.left) pt.x = o_Rect.left;
			if(pt.x >= o_Rect.right) pt.x = o_Rect.right - 1;
			if(pt.y < o_Rect.top) pt.y = o_Rect.top;
			if(pt.y >= o_Rect.bottom) pt.y = o_Rect.bottom - 1;
			ClientToScreen(&pt);
			SetCursorPos(pt.x, pt.y);
			MATH_CopyVector(&v1, &mst_Move);
			CAM_Move(&mst_WinHandles.pst_DisplayData->st_Camera, &v1);
		}

        if (SOFT_b_Helpers_UVGizmoIsOn( M_F3D_Helpers ) )
            SOFT_UVGizmo_Move( &M_F3D_Helpers->st_UVGizmo, &M_F3D_DD->st_Camera.st_Matrix, &mst_Move );
		else if(mi_EditedCurve != -1)
			Curve_Move(&mst_Move);
		else if ( mpst_EditedPortal )
			Portal_Move( &mst_Move );
		else
		{
            if ( (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_MovePivot) && Selection_b_IsInSubObjectMode(FALSE))
                Selection_SubObjectMove(&mst_Move, &point, !Selection_b_IsInSubObjectMode(TRUE), FALSE);
			else if(Selection_b_IsInSubObjectMode(TRUE))
                Selection_SubObjectMove(&mst_Move, &point, 0, FALSE );
			else
			{
				MATH_NegEqualVector(&mst_Move);
				MATH_AddVector(&v, &v, &mst_Move);
				if(GetAsyncKeyState(VK_CONTROL) < 0) MATH_ScaleEqualVector(&mst_Move, 0.005f);
				Selection_GAO_Move(&mst_Move);
				MATH_CopyVector(&mst_Move, &v);
			}
		}
	}
	else if
		(
			((type >= SOFT_Cl_Helpers_ScaleLocalX) && (type <= SOFT_Cl_Helpers_ScaleLocalZ))
		||	(type == SOFT_Cl_Helpers_ScaleLocalXYZ)
		)
	{
        if ( M_F3D_DD->uc_EditBounding || Helper_b_IsCurrentAxis( SOFT_Cul_HF_SAObject ) || (type == SOFT_Cl_Helpers_ScaleLocalXYZ) )
        {
		    v.x = mst_Move.x / ((v.x) ? v.x : 1.0f);
		    v.y = mst_Move.y / ((v.y) ? v.y : 1.0f);
		    v.z = mst_Move.z / ((v.z) ? v.z : 1.0f);

            if ( (M_F3D_Helpers->ul_Flags) & SOFT_Cul_HF_ConstraintYZ) v.x = 1;
            if ( (M_F3D_Helpers->ul_Flags) & SOFT_Cul_HF_ConstraintXZ) v.y = 1;
            if ( (M_F3D_Helpers->ul_Flags) & SOFT_Cul_HF_ConstraintXY) v.z = 1;
        }
        
        if (SOFT_b_Helpers_UVGizmoIsOn( M_F3D_Helpers ) )
            SOFT_UVGizmo_Scale( &M_F3D_Helpers->st_UVGizmo, &v );
        else if(mi_EditedCurve != -1)
			return;
		else if ( mpst_EditedPortal )
			Portal_Scale( &v );
		else if(Selection_b_IsInSubObjectMode(TRUE))
			Selection_SubObjectScale(&v);
		else
			Selection_Scale(&v);
	}
	else if((type >= SOFT_Cl_Helpers_OmniNear1) && (type <= SOFT_Cl_Helpers_OmniNear8))
	{
		i = type - SOFT_Cl_Helpers_OmniNear1;
		*(long *) &mst_Move.y = -1;
		LIGHT_ChangeOmniNearFar((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Omni[i], mst_Move.x, mst_Move.y);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_OmniFar1) && (type <= SOFT_Cl_Helpers_OmniFar8))
	{
		i = type - SOFT_Cl_Helpers_OmniFar1;
		*(long *) &mst_Move.y = -1;
		LIGHT_ChangeOmniNearFar((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Omni[i], mst_Move.y, mst_Move.x);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_SpotLittleAlpha1) && (type <= SOFT_Cl_Helpers_SpotLittleAlpha8))
	{
		i = type - SOFT_Cl_Helpers_SpotLittleAlpha1;
		*(long *) &mst_Move.y = -1;
		LIGHT_ChangeSpotAlpha((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.x, mst_Move.y, 1);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_SpotNear1) && (type <= SOFT_Cl_Helpers_SpotNear8))
	{
		i = type - SOFT_Cl_Helpers_SpotNear1;
		*(long *) &mst_Move.y = -1;
		t = ((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i])->st_Spot.f_Near;
		t *= mst_Move.x;

		LIGHT_ChangeSpotNearFar((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], t, mst_Move.y);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_SpotBigAlpha1) && (type <= SOFT_Cl_Helpers_SpotBigAlpha8))
	{
		i = type - SOFT_Cl_Helpers_SpotBigAlpha1;
		*(long *) &mst_Move.y = -1;
		LIGHT_ChangeSpotAlpha((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.y, mst_Move.x, 1);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_SpotFar1) && (type <= SOFT_Cl_Helpers_SpotFar8))
	{
		i = type - SOFT_Cl_Helpers_SpotFar1;
		*(long *) &mst_Move.y = -1;
		t = ((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i])->st_Spot.f_Far;
		t *= mst_Move.x;

		LIGHT_ChangeSpotNearFar((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.y, t);
		Refresh();
	}
#ifdef JADEFUSION
	else if((type >= SOFT_Cl_Helpers_CylSpotLittleAlpha1) && (type <= SOFT_Cl_Helpers_CylSpotLittleAlpha8))
	{
		i = type - SOFT_Cl_Helpers_CylSpotLittleAlpha1;
		*(long *) &mst_Move.y = -1;
		LIGHT_ChangeSpotAlpha((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.x, mst_Move.y, 1);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_CylSpotBigAlpha1) && (type <= SOFT_Cl_Helpers_CylSpotBigAlpha8))
	{
		i = type - SOFT_Cl_Helpers_CylSpotBigAlpha1;
		*(long *) &mst_Move.y = -1;
		LIGHT_ChangeSpotAlpha((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.y, mst_Move.x, 1);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_CylSpotLittleAlphaStart1) && (type <= SOFT_Cl_Helpers_CylSpotLittleAlphaStart8))
	{
		i = type - SOFT_Cl_Helpers_CylSpotLittleAlphaStart1;
		*(long *) &mst_Move.y = -1;
		LIGHT_ChangeSpotAlpha((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.x, mst_Move.y, 1);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_CylSpotBigAlphaStart1) && (type <= SOFT_Cl_Helpers_CylSpotBigAlphaStart8))
	{
		i = type - SOFT_Cl_Helpers_CylSpotBigAlphaStart1;
		*(long *) &mst_Move.y = -1;
		LIGHT_ChangeSpotAlpha((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.y, mst_Move.x, 1);
		Refresh();
	}
	/* Shadow helpers */
	else if((type >= SOFT_Cl_Helpers_SpotShadowAlpha1) && (type <= SOFT_Cl_Helpers_SpotShadowAlpha8))
	{
		i = type - SOFT_Cl_Helpers_SpotShadowAlpha1;
		*(long *) &mst_Move.y = -1;
		if(((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i])->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
			LIGHT_ChangeShadowAlpha((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.y, mst_Move.x, 1);
		else
			LIGHT_ChangeShadowAlpha((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.x, mst_Move.y, 1);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_SpotShadowFarAlpha1) && (type <= SOFT_Cl_Helpers_SpotShadowFarAlpha8))
	{
		i = type - SOFT_Cl_Helpers_SpotShadowFarAlpha1;
		*(long *) &mst_Move.y = -1;
		LIGHT_ChangeShadowAlpha((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.y, mst_Move.x, 1);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_CylSpotShadowAlpha1) && (type <= SOFT_Cl_Helpers_CylSpotShadowAlpha8))
	{
		i = type - SOFT_Cl_Helpers_CylSpotShadowAlpha1;
		*(long *) &mst_Move.y = -1;
		LIGHT_ChangeShadowAlpha((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], mst_Move.y, mst_Move.x, 1);
		Refresh();
	}
	else if((type >= SOFT_Cl_Helpers_SpotShadowNear1) && (type <= SOFT_Cl_Helpers_SpotShadowNear8))
	{
		i = type - SOFT_Cl_Helpers_SpotShadowNear1;
		*(long *) &mst_Move.y = -1;
		t = ((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i])->f_ShadowNear;
		t *= mst_Move.x;

		LIGHT_ChangeShadowNearFar((LIGHT_tdst_Light *) M_F3D_Helpers->ap_Spot[i], t, mst_Move.y);
		Refresh();
	}
#endif

	Helper_l_GetDescription(sz_Text);
	LINK_gb_UseSecond = TRUE;
	LINK_gb_CanLog = FALSE;
	//if(!Selection_b_IsInSubObjectMode()) 

	//Hide Helper Tool Tip
	if ( !(M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_HideHelperToolTip) )
	EDI_Tooltip_DisplayMessage(sz_Text, 0);

	LINK_gb_CanLog = TRUE;
	LINK_gb_UseSecond = FALSE;
}

/*
 =======================================================================================================================
    Aim:    Compute matrix used by helper
 =======================================================================================================================
 */
BOOL F3D_cl_View::Helper_b_ComputeMatrix(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LINK_tdst_Pointer		    *p2;
	SEL_tdst_SelectedItem	    *pst_Sel;
    SEL_tdst_SelectedItem       *pst_CurrentSel;
    MATH_tdst_Matrix		    *pst_Matrix;
	MATH_tdst_Vector		    st_Pos, st_Center;
	OBJ_tdst_GameObject		    *pst_GO;
	COL_tdst_ZDx			    *pst_ZDx;
	COL_tdst_Cob			    *pst_Cob;
    BOOL                        b_AllScaleOk = FALSE;
    static  MATH_tdst_Vector    st_ScaleCenter;    
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(M_F3D_World == NULL) return FALSE;
	
	pst_Matrix = &M_F3D_Helpers->st_GlobalMatrix;
	M_F3D_Helpers->ul_Flags &= ~SOFT_Cul_HF_SeparateCenterForRotation;
	
	if ( mpst_EditedPortal )
	{
		Portal_b_ComputeHelperMatrix(pst_Matrix );
		return TRUE;
	}
	
	if(SEL_b_IsEmpty(M_F3D_Sel)) return FALSE;

    if (SOFT_b_Helpers_UVGizmoIsOn( M_F3D_Helpers ) )
    {
        M_F3D_Helpers->ul_Flags &= ~SOFT_Cul_HF_LocalMode;
        MATH_CopyMatrix( pst_Matrix, &M_F3D_Helpers->st_UVGizmo.st_Matrix );
        return TRUE;
    }

	if(mi_EditedCurve != -1)
	{
		Curve_b_ComputeHelperMatrix(pst_Matrix);
		return TRUE;
	}
	
	pst_Sel = SEL_pst_GetFirst(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_ZDx | SEL_C_SIF_Cob);
	if(!pst_Sel) return FALSE;

	/* The first selected item may have been erased if generated. We check its validity. */
	p2 = LINK_p_SearchPointer(pst_Sel->p_Content);
	while(!p2)
	{
		SEL_DelItem(M_F3D_Sel, pst_Sel->p_Content);
		if(SEL_b_IsEmpty(M_F3D_Sel)) return FALSE;
		pst_Sel = SEL_pst_GetFirst(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_ZDx | SEL_C_SIF_Cob);
		if(!pst_Sel) return FALSE;
		p2 = LINK_p_SearchPointer(pst_Sel->p_Content);
	}

    // If another selected item has no father, it's better.
    pst_CurrentSel = pst_Sel;
    while (pst_CurrentSel)
    {
        if ((pst_CurrentSel->l_Flag == SEL_C_SIF_Object) && 
            ((!((OBJ_tdst_GameObject *) pst_CurrentSel->p_Content)->pst_Base) || 
            (!((OBJ_tdst_GameObject *) pst_CurrentSel->p_Content)->pst_Base->pst_Hierarchy)))
        {   
             pst_Sel = pst_CurrentSel;
             break;
        }

    	pst_CurrentSel = SEL_pst_GetNext(M_F3D_Sel, pst_CurrentSel,SEL_C_SIF_Object | SEL_C_SIF_ZDx | SEL_C_SIF_Cob);
    }

	if(pst_Sel->l_Flag == SEL_C_SIF_Object)
		pst_GO = (OBJ_tdst_GameObject *) pst_Sel->p_Content;
	else
		pst_GO = (OBJ_tdst_GameObject *) pst_Sel->ul_User;

	OBJ_MakeAbsoluteMatrix(pst_GO, pst_Matrix);
	if(MATH_b_TestScaleType(pst_Matrix))
	{
		MATH_CopyVector(&st_Pos, MATH_pst_GetTranslation(pst_Matrix));
		MATH_GetRotationMatrix(pst_Matrix, pst_Matrix);
		MATH_SetTranslation(pst_Matrix, &st_Pos);
	}
    MATH_CopyVector( &st_Center, MATH_pst_GetTranslation( pst_Matrix ) );

	M_F3D_Helpers->ul_Flags &= ~SOFT_Cul_HF_LocalMode;

	if(pst_Sel->l_Flag == SEL_C_SIF_Object)
	{
		if(M_F3D_DD->uc_EditBounding)
		{
			if(!(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV))
			{
				if(OBJ_BV_IsSphere(pst_GO->pst_BV))
				{
					MATH_TransformVertexNoScale
					(
						&st_Center,
						OBJ_pst_GetAbsoluteMatrix(pst_GO),
						OBJ_pst_BV_GetCenter(pst_GO->pst_BV)
					);
					MATH_SetTranslation(pst_Matrix, &st_Center);
				}
				else
				{
					MATH_SubVector(&st_Pos, OBJ_pst_BV_GetGMaxInit(pst_GO->pst_BV), OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));
					MATH_MulEqualVector(&st_Pos, 0.5f);
					MATH_AddEqualVector(&st_Pos, OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));

					/*
					 * After this, st_Pos will be the position of the center of the AABBox in the
					 * Global coordinate system.
					 */
					MATH_AddEqualVector(&st_Pos, OBJ_pst_GetAbsolutePosition(pst_GO));
					MATH_SetTranslation(pst_Matrix, &st_Pos);
				}
			}
			else
			{
				MATH_SubVector(&st_Pos, OBJ_pst_BV_GetLMax(pst_GO->pst_BV), OBJ_pst_BV_GetLMin(pst_GO->pst_BV));
				MATH_MulEqualVector(&st_Pos, 0.5f);
				MATH_AddEqualVector(&st_Pos, OBJ_pst_BV_GetLMin(pst_GO->pst_BV));
				MATH_TransformVertex(&st_Center, OBJ_pst_GetAbsoluteMatrix(pst_GO), &st_Pos);
				MATH_SetTranslation(pst_Matrix, &st_Center);
			}
		}
		else if(M_F3D_DD->uc_EditODE)
		{
			if(OBJ_ODE_IsBox(pst_GO))
			{
				MATH_tdst_Matrix ODEMatrix, GOMatrixNoScale;
				MATH_CopyMatrix(&ODEMatrix, &pst_GO->pst_Base->pst_ODE->st_RotMatrix);
				MATH_SetTranslation(&ODEMatrix, &pst_GO->pst_Base->pst_ODE->st_Offset);

				MATH_CopyMatrix(&GOMatrixNoScale, pst_GO->pst_GlobalMatrix);
				MATH_ClearScaleType(&GOMatrixNoScale);
				MATH_MulMatrixMatrix(pst_Matrix, &ODEMatrix, &GOMatrixNoScale);
			}
		}
		else
		{
			if(Selection_b_IsInSubObjectMode(TRUE))
			{
                if ( !Helper_b_IsInUse() || (!(M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_Scale)) )
    				Selection_SubObject_CenterOfGravity(&st_ScaleCenter);
	    		MATH_SetTranslation(pst_Matrix, &st_ScaleCenter);
				M_F3D_Helpers->ul_Flags |= SOFT_Cul_HF_LocalMode;
                b_AllScaleOk = TRUE;
			}
		}

        /* si pas de sous objet sel et si on était en mode scale et axe non local, retourne en local */
        if (!b_AllScaleOk && !(M_F3D_DD->uc_EditBounding) && (M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_Scale) )
            Helper_SetAxis( SOFT_Cul_HF_SAObject );

		return TRUE;
	}

	if(pst_Sel->l_Flag == SEL_C_SIF_ZDx)
	{
		pst_ZDx = (COL_tdst_ZDx *) pst_Sel->p_Content;
		if(!pst_ZDx) return FALSE;

		switch(COL_Zone_GetType(pst_ZDx))
		{
		case COL_C_Zone_Sphere:
			if(pst_ZDx->uc_Flag & COL_C_Zone_NoScale)
				MATH_TransformVertexNoScale(&st_Center, pst_GO->pst_GlobalMatrix, COL_pst_Shape_GetCenter(pst_ZDx->p_Shape));
			else
				MATH_TransformVertex(&st_Center, pst_GO->pst_GlobalMatrix, COL_pst_Shape_GetCenter(pst_ZDx->p_Shape));
			MATH_SetTranslation(pst_Matrix, &st_Center);
			break;

		case COL_C_Zone_Box:
			MATH_AddVector(&st_Pos, COL_pst_Shape_GetMax(pst_ZDx->p_Shape), COL_pst_Shape_GetMin(pst_ZDx->p_Shape));
			MATH_MulEqualVector(&st_Pos, 0.5f);
			if(pst_ZDx->uc_Flag & COL_C_Zone_NoScale)
				MATH_TransformVertexNoScale(&st_Center, OBJ_pst_GetAbsoluteMatrix(pst_GO), &st_Pos);
			else
				MATH_TransformVertex(&st_Center, OBJ_pst_GetAbsoluteMatrix(pst_GO), &st_Pos);
			MATH_SetTranslation(pst_Matrix, &st_Center);
			break;
		}

		return TRUE;
	}

	if(pst_Sel->l_Flag == SEL_C_SIF_Cob)
	{
		pst_Cob = (COL_tdst_Cob *) pst_Sel->p_Content;
		if(!pst_Cob) return FALSE;

		if(M_F3D_DD->uc_EditBounding)
		{
			if(!(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV))
			{
				if(OBJ_BV_IsSphere(pst_GO->pst_BV))
				{
					MATH_TransformVertexNoScale
					(
						&st_Center,
						OBJ_pst_GetAbsoluteMatrix(pst_GO),
						OBJ_pst_BV_GetCenter(pst_GO->pst_BV)
					);
					MATH_SetTranslation(pst_Matrix, &st_Center);
				}
				else
				{
					MATH_SubVector(&st_Pos, OBJ_pst_BV_GetGMaxInit(pst_GO->pst_BV), OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));
					MATH_MulEqualVector(&st_Pos, 0.5f);
					MATH_AddEqualVector(&st_Pos, OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));

					/*
					 * After this, st_Pos will be the position of the center of the AABBox in the
					 * Global coordinate system.
					 */
					MATH_AddEqualVector(&st_Pos, OBJ_pst_GetAbsolutePosition(pst_GO));
					MATH_SetTranslation(pst_Matrix, &st_Pos);
				}
			}
			else
			{
				MATH_SubVector(&st_Pos, OBJ_pst_BV_GetLMax(pst_GO->pst_BV), OBJ_pst_BV_GetLMin(pst_GO->pst_BV));
				MATH_MulEqualVector(&st_Pos, 0.5f);
				MATH_AddEqualVector(&st_Pos, OBJ_pst_BV_GetLMin(pst_GO->pst_BV));
				MATH_TransformVertex(&st_Center, OBJ_pst_GetAbsoluteMatrix(pst_GO), &st_Pos);
				MATH_SetTranslation(pst_Matrix, &st_Center);
			}
		}
		else
		{
			switch(COL_Cob_GetType(pst_Cob))
			{
			case COL_C_Zone_Cylinder:
			case COL_C_Zone_Sphere:
				MATH_CopyVector(&st_Center, COL_pst_Shape_GetCenter(pst_Cob->pst_MathCob->p_Shape));
				break;

			case COL_C_Zone_Box:
				MATH_CopyVector(&st_Center, COL_pst_Shape_GetMax(pst_Cob->pst_MathCob->p_Shape));
				MATH_AddEqualVector(&st_Center, COL_pst_Shape_GetMin(pst_Cob->pst_MathCob->p_Shape));
				MATH_MulEqualVector(&st_Center, 0.5f);
				break;

			default:
				MATH_InitVector(&st_Center, 0.0f, 0.0f, 0.0f);
				break;
			}

			MATH_TransformVertexNoScale(&st_Pos, OBJ_pst_GetAbsoluteMatrix(pst_GO), &st_Center);
			MATH_SetTranslation(pst_Matrix, &st_Pos);

		}
		
		if(Selection_b_IsInSubObjectMode(FALSE))
		{
			Selection_SubObject_CenterOfGravity(&st_Center);
			MATH_SetTranslation(pst_Matrix, &st_Center);
			M_F3D_Helpers->ul_Flags |= SOFT_Cul_HF_LocalMode;
		}


		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Render helper
 =======================================================================================================================
 */
void F3D_cl_View::Helper_Render(void)
{
	if (Helper_b_ComputeMatrix() || M_F3D_Helpers->pv_Prefab )
		SOFT_Helpers_Render(M_F3D_Helpers, mst_WinHandles.pst_DisplayData);
    else
        SOFT_Helpers_Reset(M_F3D_Helpers);
}

/*$4
 ***********************************************************************************************************************
    Curve helper functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Helper_AddCurve
(
	OBJ_tdst_GameObject			*_pst_GO,
    struct EVE_tdst_Data_       *_pst_Events,
	struct EVE_tdst_Track_		*_pst_Track
)
{
	SOFT_Helper_AddCurve(M_F3D_Helpers, _pst_GO, _pst_Events, _pst_Track);
	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DrawCurve;
	Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Helper_DelCurve(struct EVE_tdst_Data_ *_pst_Data, struct EVE_tdst_Track_ *_pst_Track)
{
	SOFT_Helper_DelCurve(M_F3D_Helpers, _pst_Data, _pst_Track);
	Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Helper_ReplaceCurve
(
	OBJ_tdst_GameObject			*_pst_GO,
    struct EVE_tdst_Data_       *_pst_Events,
	struct EVE_tdst_Data_		*_pst_NewEvents
)
{
	SOFT_Helpers_CurveReplaceData(M_F3D_Helpers, _pst_GO, _pst_Events, _pst_NewEvents);
	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DrawCurve;
	Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Helper_SetInterPerSeconds(float _f_IPS)
{
	M_F3D_Helpers->f_NbInterPerSecond = _f_IPS;
	Refresh();
}

/*$4
 ***********************************************************************************************************************
    Private functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Assume that norm of given vector is not superior to given max
 =======================================================================================================================
 */
void BornMove(MATH_tdst_Vector *_pst_Vec, float _f_Max)
{
	/*~~~~~~~~~~*/
	float	fNorm;
	/*~~~~~~~~~~*/

	fNorm = MATH_f_NormVector(_pst_Vec);
	if(fNorm > _f_Max)
	{
		fNorm = _f_Max / fNorm;
		MATH_ScaleEqualVector(_pst_Vec, fNorm);
	}
}

#endif /* ACTIVE_EDITORS */

/*$T SOFTHelpers.c GC! 1.081 06/18/01 11:27:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "MATHs/MATH.h"

#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTlinear.h"
#include "SOFT/SOFTstrings.h"

#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"
#include "ENGine/Sources/ANImation/ANIinit.h"

#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"

#include "GRObject/GROedit.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOsubobject.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "SELection/SELection.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "Selection/Selection.h"
#include "GraphicDK/Sources/GRObject/GROrender.h"

#include "EDItors/Sources/PreFaB/PFBdata.h"
// -- Bug: modif temporaire --
extern MATH_tdst_Vector mst_MoveBugLocal;
// ---------------------------
/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

/*$off*/
static LONG		aal_CenterId[3][4] =
{
	{ /* Local system axis */ SOFT_Cl_Helpers_MoveCameraXY, SOFT_Cl_Helpers_MoveLocalYZ,  SOFT_Cl_Helpers_MoveLocalXZ,	SOFT_Cl_Helpers_MoveLocalXY },
	{ /* Global system axis */SOFT_Cl_Helpers_MoveCameraXY, SOFT_Cl_Helpers_MoveGlobalYZ, SOFT_Cl_Helpers_MoveGlobalXZ, SOFT_Cl_Helpers_MoveGlobalXY },
	{ /* Camera system axis */SOFT_Cl_Helpers_MoveCameraXY, SOFT_Cl_Helpers_MoveCameraYZ, SOFT_Cl_Helpers_MoveCameraXZ, SOFT_Cl_Helpers_MoveCameraXY },
};

static LONG		aaal_Id[2][3][4] =
{
	{	/* Rotation */
		{ /* Local system axis */  SOFT_Cl_Helpers_RotationLocalX,	SOFT_Cl_Helpers_RotationLocalY,		SOFT_Cl_Helpers_RotationLocalZ,		SOFT_Cl_Helpers_RotationCameraZ },
		{ /* Global system axis */ SOFT_Cl_Helpers_RotationGlobalX, SOFT_Cl_Helpers_RotationGlobalY, SOFT_Cl_Helpers_RotationGlobalZ, SOFT_Cl_Helpers_RotationCameraZ },
		{ /* Camera system axis */ SOFT_Cl_Helpers_RotationCameraX, SOFT_Cl_Helpers_RotationCameraY, SOFT_Cl_Helpers_RotationCameraZ, -1 },
	},
	{ /* Translation */
		{ /* Local system axis */  SOFT_Cl_Helpers_MoveLocalX,	SOFT_Cl_Helpers_MoveLocalY,		SOFT_Cl_Helpers_MoveLocalZ, -1 },
		{ /* Global system axis */ SOFT_Cl_Helpers_MoveGlobalX, SOFT_Cl_Helpers_MoveGlobalY, SOFT_Cl_Helpers_MoveGlobalZ, -1 },
		{ /* Camera system axis */ SOFT_Cl_Helpers_MoveCameraX, SOFT_Cl_Helpers_MoveCameraY, SOFT_Cl_Helpers_MoveCameraZ, -1 },
	}
};
/*$on*/
SOFT_tdst_HelperInterface	SOFT_gast_HelperInterface[SOFT_Cl_HT_Number];

static char					SOFT_sgac_TrackTreated[256];
static char					SOFT_sgac_TrackStatus[256];
static char					SOFT_sgac_GizmoStatus[256];
char						SOFT_gac_BoneCurveStatus[256];
static char					SOFT_sgc_MBStatus;
static MATH_tdst_Vector		SOFT_sgast_PreviousPos[256];
static MATH_tdst_Vector		SOFT_sgst_MBPreviousPos;
BOOL						SOFT_gb_ForceStartPicking = FALSE;

/*$4
 ***********************************************************************************************************************
    Macro
 ***********************************************************************************************************************
 */

#define M_SetHelper(helper, interf, id, data) \
	{ \
		helper->l_Id = id; \
		helper->p_Data = (void *) (data); \
		helper->i = &SOFT_gast_HelperInterface[interf]; \
	}

#define M_VectorModulo( _VT_, _VS_, _f_ )\
{\
    (_VT_)->x = MATH_f_FloatModulo( (_VS_)->x, _f_ );\
	(_VT_)->y = MATH_f_FloatModulo( (_VS_)->y, _f_ );\
	(_VT_)->z = MATH_f_FloatModulo( (_VS_)->z, _f_ );\
}

/*$4
 ***********************************************************************************************************************
    Private function prototypes
 ***********************************************************************************************************************
 */

/*$off*/

void SOFT_Helpers_EllipseAdd	( SOFT_tdst_Helpers *, LONG, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, LONG, ULONG, ULONG );
LONG SOFT_l_Helpers_EllipsePick ( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *, MATH_tdst_Vector *, char);
void SOFT_Helpers_EllipseGetAxe ( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *);
void SOFT_Helpers_EllipseGetMove( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *);
void SOFT_Helpers_EllipseRender ( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, GDI_tdst_DisplayData *);

void SOFT_Helpers_ArrowAdd		( SOFT_tdst_Helpers *, LONG, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, LONG, ULONG, ULONG );
LONG SOFT_l_Helpers_ArrowPick	( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *, MATH_tdst_Vector *, char );
void SOFT_Helpers_ArrowGetMove	( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *);
void SOFT_Helpers_ArrowRender	( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, GDI_tdst_DisplayData *);

void SOFT_Helpers_SquareAdd		( SOFT_tdst_Helpers *, LONG, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, LONG, ULONG, ULONG );
LONG SOFT_l_Helpers_SquarePick	( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *, MATH_tdst_Vector *, char );
void SOFT_Helpers_SquareGetMove ( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *);
void SOFT_Helpers_SquareRender	( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, GDI_tdst_DisplayData *);

BOOL SOFT_b_Helpers_AddLight	( SEL_tdst_SelectedItem *, ULONG , ULONG );
LONG SOFT_l_Helpers_OmniPick	( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *, MATH_tdst_Vector *, char );
void SOFT_Helpers_OmniGetMove	( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *);

LONG SOFT_l_Helpers_CurvePick  ( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *, MATH_tdst_Vector *, char );
LONG SOFT_l_Helpers_CurvePick2 ( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *, MATH_tdst_Vector *, char );
void SOFT_Helpers_CurveGetMove ( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, MATH_tdst_Vector *);
void SOFT_Helpers_CurveRender  ( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, GDI_tdst_DisplayData *);
void SOFT_Helpers_CurveRender2 ( SOFT_tdst_Helpers *, SOFT_tdst_Helper *, GDI_tdst_DisplayData *);

LONG SOFT_l_SkeletonPick( SOFT_tdst_HelperSkeleton *, int , int );
SOFT_tdst_HelperSkeleton *SOFT_pst_SkeletonAdd( SOFT_tdst_Helpers *, OBJ_tdst_GameObject *, int );
void SOFT_SkeletonAddBone( SOFT_tdst_Helpers *, SOFT_tdst_HelperSkeleton *, int , MATH_tdst_Vector *, MATH_tdst_Vector *);

LONG SOFT_l_PortalPick( WOR_tdst_Portal *, MATH_tdst_Vector *, MATH_tdst_Vector *, float *);

LONG SOFT_l_NearestPointOf2Lines( MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, float *, float *, MATH_tdst_Vector *, float *, MATH_tdst_Vector * );
LONG SOFT_l_IntersectPlaneLine	( MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector * );
LONG SOFT_l_IntersectQuadLine( MATH_tdst_Vector	*, MATH_tdst_Vector	*, MATH_tdst_Vector	*, MATH_tdst_Vector	*, MATH_tdst_Vector	*, float * );


/*$on*/

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_Project(SOFT_tdst_Helpers *H, MATH_tdst_Vector *v)
{
	if(H->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
	{
		v->x /= v->z;
		v->y /= v->z;
		v->z = 1.0f;
	}
	else
	{
		/*
		 * v->x *= H->pst_DD->st_Camera.f_IsoFactor; v->y *=
		 * H->pst_DD->st_Camera.f_IsoFactor;
		 */

		/* v->z = H->pst_DD->st_Camera.f_IsoFactor + 20.0f; */
		v->z = 10;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_Init(SOFT_tdst_Helpers *_pst_Helpers, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~*/
	int i;
	/*~~*/

	L_memset(_pst_Helpers, 0, sizeof(SOFT_tdst_Helpers));
	_pst_Helpers->l_Picked = _pst_Helpers->l_Pickable = -1;
	_pst_Helpers->ul_Flags = SOFT_Cul_HF_SAObject | SOFT_Cul_HF_NoMovers;
	_pst_Helpers->ul_FlagsSave[0] = SOFT_Cul_HF_SAObject | SOFT_Cul_HF_NoMovers;
	_pst_Helpers->ul_FlagsSave[1] = SOFT_Cul_HF_SAObject | SOFT_Cul_HF_NoMovers;
	_pst_Helpers->ul_FlagsSave[2] = SOFT_Cul_HF_SAObject | SOFT_Cul_HF_NoMovers;
	_pst_Helpers->pst_DD = _pst_DD;

	SOFT_gast_HelperInterface[SOFT_Cl_HT_Ellipse].l_Type = SOFT_Cl_HT_Ellipse;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Ellipse].pfnl_Pick = SOFT_l_Helpers_EllipsePick;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Ellipse].pfnv_GetAxe = SOFT_Helpers_EllipseGetAxe;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Ellipse].pfnv_GetMove = SOFT_Helpers_EllipseGetMove;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Ellipse].pfnv_Render = SOFT_Helpers_EllipseRender;

	SOFT_gast_HelperInterface[SOFT_Cl_HT_Arrow].l_Type = SOFT_Cl_HT_Arrow;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Arrow].pfnl_Pick = SOFT_l_Helpers_ArrowPick;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Arrow].pfnv_GetAxe = NULL;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Arrow].pfnv_GetMove = SOFT_Helpers_ArrowGetMove;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Arrow].pfnv_Render = SOFT_Helpers_ArrowRender;

	SOFT_gast_HelperInterface[SOFT_Cl_HT_Square].l_Type = SOFT_Cl_HT_Square;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Square].pfnl_Pick = SOFT_l_Helpers_SquarePick;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Square].pfnv_GetAxe = NULL;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Square].pfnv_GetMove = SOFT_Helpers_SquareGetMove;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Square].pfnv_Render = SOFT_Helpers_SquareRender;

	SOFT_gast_HelperInterface[SOFT_Cl_HT_Omni].l_Type = SOFT_Cl_HT_Omni;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Omni].pfnl_Pick = SOFT_l_Helpers_OmniPick;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Omni].pfnv_GetAxe = NULL;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Omni].pfnv_GetMove = SOFT_Helpers_OmniGetMove;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Omni].pfnv_Render = SOFT_Helpers_EllipseRender;

	SOFT_gast_HelperInterface[SOFT_Cl_HT_Curve].l_Type = SOFT_Cl_HT_Curve;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Curve].pfnl_Pick = SOFT_l_Helpers_CurvePick;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Curve].pfnv_GetAxe = NULL;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Curve].pfnv_GetMove = SOFT_Helpers_CurveGetMove;
	SOFT_gast_HelperInterface[SOFT_Cl_HT_Curve].pfnv_Render = SOFT_Helpers_CurveRender;

	/* curve */
	for(i = 0; i < 16; i++)
	{
		_pst_Helpers->ast_HelperCurve[i].i = &SOFT_gast_HelperInterface[SOFT_Cl_HT_Curve];
		_pst_Helpers->ast_HelperCurve[i].l_Id = SOFT_Cl_Helpers_Curve1 + i;
		_pst_Helpers->ast_HelperCurve[i].p_Data = &_pst_Helpers->ast_Curve[i];
		_pst_Helpers->ast_Curve[i].l_PickCur = -1;
	}

	_pst_Helpers->l_NbFramesAfter = 20;
	_pst_Helpers->l_NbFramesBefore = 20;
	_pst_Helpers->f_NbInterPerSecond = 60.0f;
	_pst_Helpers->ul_GizmoDisplayFlag[0] = 0xFFFFFFFF;
	_pst_Helpers->ul_GizmoDisplayFlag[1] = 0xFFFFFFFF;
	L_memset(SOFT_gac_BoneCurveStatus, 0, sizeof(SOFT_gac_BoneCurveStatus));

	/* skeleton */
	for(i = 0; i < 4; i++)
	{
		_pst_Helpers->ast_Skeleton[i].l_NbBones = 0;
	}

    /* UV gizmo */
    SOFT_UVGizmo_Init( &_pst_Helpers->st_UVGizmo );
    
    _pst_Helpers->pv_Prefab = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_Reset(SOFT_tdst_Helpers *_pst_Helpers)
{
	/*~~*/
	int i;
	/*~~*/

	_pst_Helpers->l_CurrentEllipse = 0;
	_pst_Helpers->l_CurrentArrow = 0;
	_pst_Helpers->l_CurrentHelper = 0;
	_pst_Helpers->l_CurrentOmni = 0;
	_pst_Helpers->l_CurrentSpot = 0;
	_pst_Helpers->l_CurrentSkeleton = 0;
    
	for(i = 0; i < 4; i++)
	{
		if(_pst_Helpers->ast_Skeleton[i].l_NbBones)
		{
			L_free(_pst_Helpers->ast_Skeleton[i].dst_Bone);
			_pst_Helpers->ast_Skeleton[i].l_NbBones = 0;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_DetachObject(SOFT_tdst_Helpers *H, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~*/
	int i;
	/*~~*/

	if(!H) return;
	for(i = 0; i < 16; i++)
	{
		if ( (H->ast_Curve[i].p_OwnerGO == _pst_GO) || (H->ast_Curve[i].p_CurGO == _pst_GO) )
		{
			if(H->ast_Curve[i].dst_Pick)
			{
				L_free(H->ast_Curve[i].dst_Pick);
				H->ast_Curve[i].dst_Pick = NULL;
				H->ast_Curve[i].l_PickMax = H->ast_Curve[i].l_PickNumber = 0;
				H->ast_Curve[i].l_PickCur = -1;
			}

			H->ast_Curve[i].ul_Flags = 0;
		}
	}

    /* UV gizmo */
    if ( H->st_UVGizmo.pst_GO == _pst_GO )
        SOFT_UVGizmo_Reset( &H->st_UVGizmo );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_HideMovers(SOFT_tdst_Helpers *_pst_Helpers)
{
	_pst_Helpers->ul_Flags &= ~SOFT_Cul_HF_MoveMask;
	_pst_Helpers->ul_Flags |= SOFT_Cul_HF_NoMovers;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helpers_MoversAreVisible(SOFT_tdst_Helpers *_pst_Helpers)
{
	return(_pst_Helpers->ul_Flags & SOFT_Cul_HF_MoveMask);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helper_ChangeMoverMode(SOFT_tdst_Helpers *_pst_Helpers, LONG _l_Delta, ULONG _ul_Constraint, BOOL _b_AllScale )
{
	/*~~~~~~~~~~~~*/
	ULONG	n, o, m;
	BOOL	b_WorldAxis;
	/*~~~~~~~~~~~~*/


	b_WorldAxis = FALSE;
	if(_pst_Helpers->pst_DD->uc_EditBounding && SEL_pst_GetFirstItem(_pst_Helpers->pst_DD->pst_World->pst_Selection, SEL_C_SIF_Object))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject	*pst_GO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(_pst_Helpers->pst_DD->pst_World->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO && !(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV))
			b_WorldAxis = TRUE;
	}

	m = (SOFT_Cul_HF_MoveMask | SOFT_Cul_HF_NoMovers);
	n = o = _pst_Helpers->ul_Flags & m;
	if (!n)
		return;

	if(_l_Delta >= 0)
	{
		if(_l_Delta > 0) n = (n & SOFT_Cul_HF_Move) ? SOFT_Cul_HF_NoMovers : ((n << 1) & m);

		/* BV edition. We dont want the Rotation Helper */
		if(_pst_Helpers->pst_DD->uc_EditBounding)
		{
			if(n == SOFT_Cul_HF_NoMovers)
				n = SOFT_Cul_HF_Scale;
		}

		while(!(n & _ul_Constraint)) n = (n & SOFT_Cul_HF_Move) ? SOFT_Cul_HF_NoMovers : ((n << 1) & m);
	}
	else
	{
		n = (n & SOFT_Cul_HF_NoMovers) ? SOFT_Cul_HF_Move : (n >> 1);

		/* BV edition. We dont want the Rotation Helper */
		if(_pst_Helpers->pst_DD->uc_EditBounding)
		{
			if(n == SOFT_Cul_HF_Rotate)
				n = SOFT_Cul_HF_NoMovers;
		}

		while(!(n & _ul_Constraint)) n = (n & SOFT_Cul_HF_NoMovers) ? SOFT_Cul_HF_Move : (n >> 1);
	}

	_pst_Helpers->ul_Flags &= ~m;
	_pst_Helpers->ul_Flags |= n;

    if (!_b_AllScale)
    {
	    /* System axis constraint */
	    if(n & SOFT_Cul_HF_Scale)
	    {
		    _pst_Helpers->ul_FlagsSave[0] = _pst_Helpers->ul_Flags;
		    _pst_Helpers->ul_Flags &= ~SOFT_Cul_HF_SAMask;
		    if(b_WorldAxis)
			    _pst_Helpers->ul_Flags |= SOFT_Cul_HF_SAWorld;
		    else
			    _pst_Helpers->ul_Flags |= SOFT_Cul_HF_SAObject;
	    }

	    if(o & SOFT_Cul_HF_Scale)
	    {
		    _pst_Helpers->ul_Flags &= ~SOFT_Cul_HF_SAMask;
		    _pst_Helpers->ul_Flags |= _pst_Helpers->ul_FlagsSave[0] & SOFT_Cul_HF_SAMask;
	    }

	    if(n & SOFT_Cul_HF_Move)
		{
			_pst_Helpers->ul_Flags &= ~SOFT_Cul_HF_SAWorld;
			_pst_Helpers->ul_Flags &= ~SOFT_Cul_HF_SAObject;

		    if(b_WorldAxis)
			    _pst_Helpers->ul_Flags |= SOFT_Cul_HF_SAWorld;
		    else
			    _pst_Helpers->ul_Flags |= SOFT_Cul_HF_SAObject;
		}
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char SOFT_c_Helper_GetUsedAxis(SOFT_tdst_Helpers *H)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*$off*/
    static char sac_UsedAxis[] = {  0, 1, 2, 4, 1, 2, 4, 1, 2, 4, 7, 		                /* from RotationLocalX to RotationSpere */
		                            1, 2, 4, 1, 2, 4, 1, 2, 4, 3, 5, 6, 3, 5, 6, 3, 5, 6,	/* from MoveLocalX to MoveCameraYZ */
		                            1, 2, 4, 3, 5, 6	                                    /* scale */
	                                };
    /*$on*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(H->l_Picked > SOFT_Cl_Helpers_ScaleLocalXYZ) return 0;
	return sac_UsedAxis[H->l_Picked];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char SOFT_c_Helper_GetUsedSystemAxis(SOFT_tdst_Helpers *H)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*$off*/
    static char sac_UsedSystemAxis[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		/* from RotationLocalX to RotationSpere */
		                                 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x10, 0x10, 0x10,
		                                 0, 0, 0, 0, 0, 0	                    /* scale */
	                                    };
    /*$on*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(H->l_Picked > SOFT_Cl_Helpers_ScaleLocalXYZ) return 0;
	return sac_UsedSystemAxis[H->l_Picked];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_GetMove(SOFT_tdst_Helpers *_pst_Helpers, LONG _l_Helper, MATH_tdst_Vector *_pst_Move)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Helper	*pst_Helper, *pst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Helper = _pst_Helpers->ast_Helper;
	pst_Last = pst_Helper + _pst_Helpers->l_CurrentHelper;
	for(; pst_Helper < pst_Last; pst_Helper++)
	{
		if(_l_Helper == pst_Helper->l_Id)
		{
			pst_Helper->i->pfnv_GetMove(_pst_Helpers, pst_Helper, _pst_Move);
			return;
		}
	}

	MATH_InitVectorToZero(_pst_Move);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_GetAxe(SOFT_tdst_Helpers *_pst_Helpers, LONG _l_Helper, MATH_tdst_Vector *_pst_Axe)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Helper	*pst_Helper, *pst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Helper = _pst_Helpers->ast_Helper;
	pst_Last = pst_Helper + _pst_Helpers->l_CurrentHelper;
	for(; pst_Helper < pst_Last; pst_Helper++)
	{
		if(_l_Helper == pst_Helper->l_Id)
		{
			pst_Helper->i->pfnv_GetAxe(_pst_Helpers, pst_Helper, _pst_Axe);
			return;
		}
	}

	MATH_InitVectorToZero(_pst_Axe);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helpers_GetDescription
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	LONG				_l_Helper,
	char				*_sz_Desc,
	MATH_tdst_Vector	*_pst_Move
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				l_Result;
	MATH_tdst_Vector	st_Move;
	int					i_Angle;
	float				f_Angle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((_sz_Desc == NULL) || (_pst_Move == NULL)) return 0;

	l_Result = !MATH_b_NulVector(_pst_Move);
	MATH_CopyVector(&st_Move, _pst_Move);

	switch(_l_Helper)
	{
	case SOFT_Cl_Helpers_RotationLocalX:
	case SOFT_Cl_Helpers_RotationLocalY:
	case SOFT_Cl_Helpers_RotationLocalZ:
		strcpy(_sz_Desc, SOFT_STR_Csz_RL);
		break;

	case SOFT_Cl_Helpers_RotationGlobalX:
	case SOFT_Cl_Helpers_RotationGlobalY:
	case SOFT_Cl_Helpers_RotationGlobalZ:
		strcpy(_sz_Desc, SOFT_STR_Csz_RG);
		break;
	case SOFT_Cl_Helpers_RotationCameraX:
	case SOFT_Cl_Helpers_RotationCameraY:
	case SOFT_Cl_Helpers_RotationCameraZ:
		strcpy(_sz_Desc, SOFT_STR_Csz_RC);
		break;
	case SOFT_Cl_Helpers_RotationSphere:
		strcpy(_sz_Desc, SOFT_STR_Csz_RF);
		break;

	case SOFT_Cl_Helpers_MoveLocalX:
	case SOFT_Cl_Helpers_MoveLocalY:
	case SOFT_Cl_Helpers_MoveLocalZ:
	case SOFT_Cl_Helpers_MoveLocalXY:
	case SOFT_Cl_Helpers_MoveLocalXZ:
	case SOFT_Cl_Helpers_MoveLocalYZ:
		MATH_NegEqualVector(&st_Move);
		MATH_TransformVector(&st_Move, &_pst_Helpers->st_CamToLocalMatrix, &st_Move);
		strcpy(_sz_Desc, SOFT_STR_Csz_ML);
		break;

	case SOFT_Cl_Helpers_MoveGlobalX:
	case SOFT_Cl_Helpers_MoveGlobalY:
	case SOFT_Cl_Helpers_MoveGlobalZ:
	case SOFT_Cl_Helpers_MoveGlobalXY:
	case SOFT_Cl_Helpers_MoveGlobalXZ:
	case SOFT_Cl_Helpers_MoveGlobalYZ:
		MATH_NegEqualVector(&st_Move);
		MATH_TransformVector(&st_Move, &_pst_Helpers->st_CamToGlobalMatrix, &st_Move);
		strcpy(_sz_Desc, SOFT_STR_Csz_MG);
		break;

	case SOFT_Cl_Helpers_MoveCameraX:
	case SOFT_Cl_Helpers_MoveCameraY:
	case SOFT_Cl_Helpers_MoveCameraZ:
	case SOFT_Cl_Helpers_MoveCameraXY:
	case SOFT_Cl_Helpers_MoveCameraXZ:
	case SOFT_Cl_Helpers_MoveCameraYZ:
		MATH_NegEqualVector(&st_Move);
		strcpy(_sz_Desc, SOFT_STR_Csz_MC);
		break;

	case SOFT_Cl_Helpers_ScaleLocalX:
	case SOFT_Cl_Helpers_ScaleLocalY:
	case SOFT_Cl_Helpers_ScaleLocalZ:
	case SOFT_Cl_Helpers_ScaleLocalXY:
	case SOFT_Cl_Helpers_ScaleLocalXZ:
	case SOFT_Cl_Helpers_ScaleLocalYZ:
	case SOFT_Cl_Helpers_ScaleLocalXYZ:
		strcpy(_sz_Desc, SOFT_STR_Csz_SL);
		l_Result = !MATH_b_EqVector(_pst_Move, &MATH_gst_IdentityVector);
		break;
	default: 
        *_sz_Desc = 0;
        return 0;
	}

	if((_l_Helper >= SOFT_Cl_Helpers_RotationLocalX) && (_l_Helper <= SOFT_Cl_Helpers_RotationCameraZ))
	{
		i_Angle = (_l_Helper - 1) % 3;
		f_Angle = ((float *) _pst_Move)[i_Angle];
		sprintf(_sz_Desc + strlen(_sz_Desc), "\n%c : %.2f (%.3f rad)", 'X' + i_Angle, f_Angle * Cf_180ByPi, f_Angle);
	}
	else
	{
		sprintf(_sz_Desc + strlen(_sz_Desc), "\n%.3f, %.3f, %.3f", st_Move.x, st_Move.y, st_Move.z);
		// -- Bug: modif temporaire --
		if (mst_MoveBugLocal.x!=0 && mst_MoveBugLocal.y!=0 && mst_MoveBugLocal.z!=0)
		sprintf(_sz_Desc + strlen(_sz_Desc), "\n%.3f, %.3f, %.3f", mst_MoveBugLocal.x, mst_MoveBugLocal.y, mst_MoveBugLocal.z);
		// ---------------------------
	}

	return l_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helper_IsScale(SOFT_tdst_Helper *_pst_Helper)
{
	if(_pst_Helper->l_Id < SOFT_Cl_Helpers_ScaleLocalX) return FALSE;
	if(_pst_Helper->l_Id > SOFT_Cl_Helpers_ScaleLocalXYZ) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helper_IsMove(SOFT_tdst_Helper *_pst_Helper)
{
	if(_pst_Helper->l_Id < SOFT_Cl_Helpers_MoveLocalX) return FALSE;
	if(_pst_Helper->l_Id > SOFT_Cl_Helpers_MoveCameraYZ) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helper_IsRotate(SOFT_tdst_Helper *_pst_Helper)
{
	if(_pst_Helper->l_Id < SOFT_Cl_Helpers_RotationLocalX) return FALSE;
	if(_pst_Helper->l_Id > SOFT_Cl_Helpers_RotationSphere) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helpers_Pick(SOFT_tdst_Helpers *_pst_Helpers, LONG x, LONG y, LONG _l_Helper)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Helper	*pst_Helper, *pst_Last;
	MATH_tdst_Vector	A, B, AW, BW;
	int					i, i_Best;
	float				Dist, DistMin;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	A.x = (float) x;
	A.y = (float) y;
	A.z = 1.0f;
	CAM_2Dto3DCamera(&_pst_Helpers->pst_DD->st_Camera, &B, &A);

	if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
		MATH_InitVectorToZero(&A);
	else
	{
		A.x = B.x;
		A.y = B.y;
		A.z = 0;
	}
	
	MATH_TransformVertex( &AW, &_pst_Helpers->pst_DD->st_Camera.st_Matrix, &A );
	MATH_TransformVertex( &BW, &_pst_Helpers->pst_DD->st_Camera.st_Matrix, &B );

	pst_Helper = _pst_Helpers->ast_Helper;
	pst_Last = pst_Helper + _pst_Helpers->l_CurrentHelper;
	for(; pst_Helper < pst_Last; pst_Helper++)
	{
		if(_l_Helper == -1)
		{
			if(pst_Helper->i->pfnl_Pick(_pst_Helpers, pst_Helper, &A, &B, 1)) return pst_Helper->l_Id;
		}
		else if(_l_Helper == pst_Helper->l_Id)
		{
			if(SOFT_gb_ForceStartPicking)
				pst_Helper->i->pfnl_Pick(_pst_Helpers, pst_Helper, &A, &B, 1);
			else
				pst_Helper->i->pfnl_Pick(_pst_Helpers, pst_Helper, &A, &B, 0);
			return 0;
		}
	}

	/* curve helper */
	pst_Helper = _pst_Helpers->ast_HelperCurve;
	pst_Last = pst_Helper + 16;
	for(; pst_Helper < pst_Last; pst_Helper++)
	{
		if(_l_Helper == -1)
		{
			if(pst_Helper->i->pfnl_Pick(_pst_Helpers, pst_Helper, &A, &B, 1)) return pst_Helper->l_Id;
		}
		else if(_l_Helper == pst_Helper->l_Id)
		{
			pst_Helper->i->pfnl_Pick(_pst_Helpers, pst_Helper, &A, &B, 0);
			return 0;
		}
	}

	/* skeleton helper */
	for(i = 0; i < _pst_Helpers->l_CurrentSkeleton; i++)
	{
		if(SOFT_l_SkeletonPick(_pst_Helpers->ast_Skeleton + i, x, y)) return SOFT_Cl_Helpers_Skeleton1 + i;
	}
	
	/* portal helper */
	for (i = 0; i < _pst_Helpers->i_PortalNb; i++ )
		_pst_Helpers->apst_Portal[i]->uw_Flags &= ~WOR_CF_PortalPickable;
	
	DistMin = Cf_Infinit;
	i_Best = -1;
	for (i = 0; i < _pst_Helpers->i_PortalNb; i++ )
	{
		if (!SOFT_l_PortalPick( _pst_Helpers->apst_Portal[i], &AW, &BW, &Dist ))
			continue;
		if ( Dist < DistMin )
		{
			i_Best = i;
			DistMin = Dist;
		}
	}
	if ( i_Best != -1 )
	{
		_pst_Helpers->apst_Portal[ i_Best ]->uw_Flags|= WOR_CF_PortalPickable;
		return SOFT_Cl_Helpers_Portal + i_Best;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_RenderGrid( SOFT_tdst_Helpers *_pst_Helpers, GDI_tdst_DisplayData *_pst_DD ) 
{
    GDI_tdst_Request_DrawLineEx st_LineEx;
    MATH_tdst_Vector			X, Y, A, B, C;
    int                         i, j;
    float                       f;
    float                       fGridSize;

    if (_pst_Helpers->f_GridSizeXY && _pst_Helpers->f_GridSizeZ)
        fGridSize = (_pst_Helpers->f_GridSizeXY > _pst_Helpers->f_GridSizeZ ? _pst_Helpers->f_GridSizeZ : _pst_Helpers->f_GridSizeXY);
    else if (_pst_Helpers->f_GridSizeXY)
        fGridSize = _pst_Helpers->f_GridSizeXY ;
    else if (_pst_Helpers->f_GridSizeZ)
        fGridSize = _pst_Helpers->f_GridSizeZ ;
    else 
        fGridSize = 1.f;

    _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 0);

    st_LineEx.A = &B;
	st_LineEx.B = &C;
	st_LineEx.f_Width = 1;
	st_LineEx.ul_Flags = 0;
	st_LineEx.ul_Color = 0xFF7F7F7F;

    j = (int) (_pst_Helpers->f_GridWidth / fGridSize);
    if ( j < 3 ) return;

	GDI_SetViewMatrix( (*_pst_DD) , _pst_DD->st_MatrixStack.dst_Matrix );
    
    MATH_TransformVector( &X, &_pst_DD->st_Camera.st_Matrix, &_pst_Helpers->st_GridX );
    MATH_TransformVector( &Y, &_pst_DD->st_Camera.st_Matrix, &_pst_Helpers->st_GridY );
    MATH_NormalizeEqualVector( &X );
    MATH_NormalizeEqualVector( &Y );
    MATH_ScaleEqualVector( &X, fGridSize );
    MATH_ScaleEqualVector( &Y, fGridSize );

    MATH_CopyVector(&A, MATH_pst_GetTranslation(&_pst_Helpers->st_GlobalMatrix));
    MATH_AddVector( &B, &A, &X );
    M_VectorModulo( &B, &B, fGridSize );
    MATH_AddVector( &C, &A, &Y );
    M_VectorModulo( &C, &C, fGridSize );

    M_VectorModulo( &A, &A, fGridSize );
    
    MATH_SubVector( &X, &B, &A );
    MATH_SubVector( &Y, &C, &A );

    f = (float) (j >> 1);
    MATH_AddScaleVector( &B, &A, &X, -f);
    MATH_AddScaleVector( &B, &B, &Y, -f);
    MATH_AddScaleVector( &C, &A, &X, -f);
    MATH_AddScaleVector( &C, &C, &Y, f);

    for (i = 0; i <= j; i++)
    {
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) &st_LineEx);
        MATH_AddEqualVector( &B, &X );
        MATH_AddEqualVector( &C, &X );
    }

    f = (float) (j >> 1);
    MATH_AddScaleVector( &B, &A, &X, -f);
    MATH_AddScaleVector( &B, &B, &Y, -f);
    MATH_AddScaleVector( &C, &A, &X, f);
    MATH_AddScaleVector( &C, &C, &Y, -f);

    for (i = 0; i <= j; i++)
    {
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) &st_LineEx);
        MATH_AddEqualVector( &B, &Y );
        MATH_AddEqualVector( &C, &Y );
    }

    _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 1);
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthFunc, 1);

}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_Render(SOFT_tdst_Helpers *_pst_Helpers, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Pos, st_SepPos, *pst_Pos;
	MATH_tdst_Vector	st_X, st_Y, st_Z, st_A, st_B;
	static float		f_Scale = .1f;
	ULONG				HF;
	SOFT_tdst_Helper	*pst_Helper, *pst_First;
	LONG				centersquareflag, dataflag, dfbase, id;
	LONG				l_SystemAxis;
	MATH_tdst_Matrix	*pst_AxisMatrix;
	MATH_tdst_Matrix M	ONLY_PSX2_ALIGNED(16);
	ULONG				ulSaveDI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get flags */
	HF = _pst_Helpers->ul_Flags;
	dfbase = HF & (SOFT_Cul_HF_LocalMode | SOFT_Cul_HF_SAMask);

    _pst_Helpers->ul_Flags &= ~SOFT_Cul_HF_SnapGridDisplay;

	/* Compute position of helper in camera system axis */
	MATH_CopyVector(&st_A, MATH_pst_GetTranslation(&_pst_Helpers->st_GlobalMatrix));
	MATH_TransformVertex(&_pst_Helpers->st_Center, &_pst_DD->st_Camera.st_InverseMatrix, &st_A);

	MATH_CopyMatrix(&_pst_Helpers->st_CamToGlobalMatrix, &_pst_DD->st_Camera.st_Matrix);
	MATH_SetIdentityMatrix(&M);
	MATH_InvertMatrix(&M, &_pst_Helpers->st_GlobalMatrix);
	MATH_MulMatrixMatrix(&_pst_Helpers->st_CamToLocalMatrix, &_pst_Helpers->st_CamToGlobalMatrix, &M);

	/* Z clipping */
	if(_pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
		if(_pst_Helpers->st_Center.z < 0.01f) HF = 0;

	/* Projection */
	MATH_CopyVector(&st_Pos, &_pst_Helpers->st_Center);
	SOFT_Helpers_Project(_pst_Helpers, &st_Pos);
	SOFT_Project( &_pst_Helpers->st_Center2D, &st_Pos, 1, &_pst_DD->st_Camera );

	/* separate center */
	MATH_TransformVertex
	(
		&_pst_Helpers->st_SeparateCenter,
		&_pst_DD->st_Camera.st_InverseMatrix,
		&_pst_Helpers->st_SeparateCenter
	);
	if(_pst_Helpers->st_SeparateCenter.z < 0)
		*(LONG *) &st_SepPos.z = -1;
	else
	{
		MATH_CopyVector(&st_SepPos, &_pst_Helpers->st_SeparateCenter);
		SOFT_Helpers_Project(_pst_Helpers, &st_SepPos);
	}

	/* Get system axis */
	if(HF & SOFT_Cul_HF_SAObject)
	{
		l_SystemAxis = 0;
		SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &_pst_Helpers->st_GlobalMatrix);
		pst_AxisMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
		SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);
	}
	else if(HF & SOFT_Cul_HF_SAWorld)
	{
		l_SystemAxis = 1;
		pst_AxisMatrix = &_pst_DD->st_Camera.st_InverseMatrix;
	}
	else
	{
		/* Camera system axis */
		l_SystemAxis = 2;
		pst_AxisMatrix = &MATH_gst_IdentityMatrix;

		/* no constraints if in camera axis */
		HF &= ~SOFT_Cul_HF_ConstraintMask;
	}

	MATH_CopyMatrix(&_pst_Helpers->st_Matrix, pst_AxisMatrix);
//	MATH_MulMatrixMatrix( &_pst_Helpers->st_MatrixGlobalAxis, &_pst_Helpers->st_CamToGlobalMatrix, pst_AxisMatrix );
	MATH_MulMatrixMatrix( &_pst_Helpers->st_MatrixGlobalAxis, pst_AxisMatrix, &_pst_Helpers->st_CamToGlobalMatrix );

	/* Compute axis length (can't be constant cause of focale change) */
	MATH_InitVector(&st_X, 100, 0, 1);
	CAM_2Dto3D(&_pst_DD->st_Camera, &st_Y, &st_X);
	MATH_InitVector(&st_X, 0, 0, 1);
	CAM_2Dto3D(&_pst_DD->st_Camera, &st_Z, &st_X);
	MATH_SubEqualVector(&st_Y, &st_Z);
	f_Scale = MATH_f_NormVector(&st_Y);

	/* anti-bug */
	if(!f_Scale) f_Scale = .1f;

	/* Compute axis */
	MATH_ScaleVector(&st_X, MATH_pst_GetXAxis(pst_AxisMatrix), f_Scale);
	MATH_ScaleVector(&st_Y, MATH_pst_GetYAxis(pst_AxisMatrix), f_Scale);
	MATH_ScaleVector(&st_Z, MATH_pst_GetZAxis(pst_AxisMatrix), f_Scale);

    /* Center square */
	centersquareflag = 2;
	switch(HF & SOFT_Cul_HF_ConstraintMask)
	{
	case 0:
		id = aal_CenterId[l_SystemAxis][0];
		centersquareflag = 0;
		MATH_InitVector(&st_A, f_Scale, 0.0f, 0.0f);
		MATH_InitVector(&st_B, 0.0f, f_Scale, 0.0f);
		break;
	case SOFT_Cul_HF_ConstraintXY:
		id = aal_CenterId[l_SystemAxis][3];
		MATH_CopyVector(&st_A, &st_X);
		MATH_CopyVector(&st_B, &st_Y);
		break;
	case SOFT_Cul_HF_ConstraintYZ:
		id = aal_CenterId[l_SystemAxis][2];
		MATH_CopyVector(&st_A, &st_Y);
		MATH_CopyVector(&st_B, &st_Z);
		break;
	case SOFT_Cul_HF_ConstraintXZ:
		id = aal_CenterId[l_SystemAxis][1];
		MATH_CopyVector(&st_A, &st_X);
		MATH_CopyVector(&st_B, &st_Z);
		break;
	}

	if(HF & (SOFT_Cul_HF_Scale | SOFT_Cul_HF_AllTogether))
	{
        /*$off*/
        if ( !(HF & SOFT_Cul_HF_AllTogether) )
            SOFT_Helpers_SquareAdd(_pst_Helpers, SOFT_Cl_Helpers_ScaleLocalXYZ, &st_Pos, &st_A, &st_B, centersquareflag + 1, 0xFF00FFFF, 0xFFFF00FF );
		
        dataflag = SOFT_Cul_Arrow_Cube | SOFT_Cl_ArrowScale | SOFT_Cul_Arrow_UseFactor;
        //SOFT_Helpers_SquareAdd(_pst_Helpers, SOFT_Cl_Helpers_ScaleLocalXYZ, &st_Pos, &st_X, &st_Y, 1, 0xFF00FFFF, 0xFFFF00FF);
		SOFT_Helpers_ArrowAdd(_pst_Helpers, SOFT_Cl_Helpers_ScaleLocalX, &st_Pos, &st_X, &st_Y, dataflag, 0xFF00FFFF, 0xFF0000FF);
		SOFT_Helpers_ArrowAdd(_pst_Helpers, SOFT_Cl_Helpers_ScaleLocalY, &st_Pos, &st_Y, &st_Z, dataflag, 0xFF00FFFF, 0xFF00FF00);
		SOFT_Helpers_ArrowAdd(_pst_Helpers, SOFT_Cl_Helpers_ScaleLocalZ, &st_Pos, &st_Z, &st_X, dataflag, 0xFF00FFFF, 0xFFFF0000);
		/*$on*/
	}

	if(HF & (SOFT_Cul_HF_Move | SOFT_Cul_HF_AllTogether))
	{
		/*$off*/
		SOFT_Helpers_SquareAdd(_pst_Helpers, id, &st_Pos, &st_A, &st_B, centersquareflag, 0xFF00FFFF, 0xFFFF00FF );
		dataflag = SOFT_Cl_ArrowMove;
		SOFT_Helpers_ArrowAdd(_pst_Helpers, aaal_Id[1][l_SystemAxis][0], &st_Pos, &st_X, &st_Y, dataflag, 0xFF00FFFF, 0xFF0000FF);
		SOFT_Helpers_ArrowAdd(_pst_Helpers, aaal_Id[1][l_SystemAxis][1], &st_Pos, &st_Y, &st_Z, dataflag, 0xFF00FFFF, 0xFF00FF00);
		SOFT_Helpers_ArrowAdd(_pst_Helpers, aaal_Id[1][l_SystemAxis][2], &st_Pos, &st_Z, &st_X, dataflag, 0xFF00FFFF, 0xFFFF0000);
		/*$on*/

        /* init for grid display */
        if ( (_pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapGrid ) && (_pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapGridRender ) )
        {
            if (id == _pst_Helpers->l_Pickable)
            {
                _pst_Helpers->ul_Flags |= SOFT_Cul_HF_SnapGridDisplay;
                MATH_CopyVector( &_pst_Helpers->st_GridX, &st_A );
                MATH_CopyVector( &_pst_Helpers->st_GridY, &st_B );
            }
            else
            {
                if (_pst_Helpers->l_Pickable == aaal_Id[1][l_SystemAxis][0])
                {
                    _pst_Helpers->ul_Flags |= SOFT_Cul_HF_SnapGridDisplay;
                    MATH_CopyVector( &_pst_Helpers->st_GridX, &st_X );
                    MATH_CopyVector( &_pst_Helpers->st_GridY, (fAbs( st_Y.z) < fAbs( st_Z.z )) ? &st_Y : &st_Z );
                }
                else if (_pst_Helpers->l_Pickable == aaal_Id[1][l_SystemAxis][1])
                {
                    _pst_Helpers->ul_Flags |= SOFT_Cul_HF_SnapGridDisplay;
                    MATH_CopyVector( &_pst_Helpers->st_GridX, &st_Y );
                    MATH_CopyVector( &_pst_Helpers->st_GridY, (fAbs( st_X.z) < fAbs( st_Z.z )) ? &st_X : &st_Z );
                }
                else if (_pst_Helpers->l_Pickable == aaal_Id[1][l_SystemAxis][2])
                {
                    _pst_Helpers->ul_Flags |= SOFT_Cul_HF_SnapGridDisplay;
                    MATH_CopyVector( &_pst_Helpers->st_GridX, &st_Z );
                    MATH_CopyVector( &_pst_Helpers->st_GridY, (fAbs( st_X.z) < fAbs( st_Y.z )) ? &st_X : &st_Y );
                }
            }
        }
	}

	if(HF & (SOFT_Cul_HF_Rotate | SOFT_Cul_HF_AllTogether))
	{
		pst_Pos = (HF & SOFT_Cul_HF_SeparateCenterForRotation) ? &st_SepPos : &st_Pos;

		if(*(LONG *) &pst_Pos->z != -1)
		{
			if(_pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
				pst_Pos->z = 1.0f;
			else
				pst_Pos->z = 10;

			/*$off*/
			SOFT_Helpers_EllipseAdd(_pst_Helpers, aaal_Id[0][l_SystemAxis][0], pst_Pos, &st_Y, &st_Z, dfbase, 0xFF00FFFF, 0xFF0000FF);
			SOFT_Helpers_EllipseAdd(_pst_Helpers, aaal_Id[0][l_SystemAxis][1], pst_Pos, &st_Z, &st_X, dfbase, 0xFF00FFFF, 0xFF00FF00);
			SOFT_Helpers_EllipseAdd(_pst_Helpers, aaal_Id[0][l_SystemAxis][2], pst_Pos, &st_X, &st_Y, dfbase, 0xFF00FFFF, 0xFFFF0000);

			if((dfbase & SOFT_Cul_HF_SACamera) == 0)
			{
				MATH_InitVector(&st_A, f_Scale * 1.1f, 0.0f, 0.0f);
				MATH_InitVector(&st_B, 0.0f, f_Scale * 1.1f, 0.0f);
				SOFT_Helpers_EllipseAdd(_pst_Helpers, aaal_Id[0][l_SystemAxis][3], pst_Pos, &st_A, &st_B, SOFT_Cul_HF_SACamera, 0xFF00FFFF, 0xFFFFFF00);
			}
			/*$on*/
		}
	}

	/* Add eventually helper for light object */
    if (_pst_DD->ul_DisplayFlags & GDI_Cul_DF_DisplayLight)
	    SEL_EnumItem(_pst_DD->pst_World->pst_Selection, SEL_C_SIF_Object, SOFT_b_Helpers_AddLight, (ULONG) _pst_Helpers, 0);

    /* Disable test of Z-buffer */
	GDI_gpst_CurDD = _pst_DD;
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthFunc, 0);
	ulSaveDI = _pst_DD->ul_DisplayInfo;
	_pst_DD->ul_DisplayInfo |= GDI_Cul_DI_DontSortObject;

    /* rendering grid */
    if (_pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapGridDisplay )
        SOFT_Helpers_RenderGrid( _pst_Helpers, _pst_DD );

    /* Init matrix for helper rendering */
	SOFT_MatrixStack_Reset(&_pst_DD->st_MatrixStack, NULL);
	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_Camera.pst_ObjectToCameraMatrix);

    /* rendering helper */
	if((!(_pst_Helpers->ul_Flags & SOFT_Cul_HF_HideWhileInUse)) || (_pst_Helpers->l_Picked == -1))
	{
		pst_First = _pst_Helpers->ast_Helper;
		pst_Helper = pst_First + (_pst_Helpers->l_CurrentHelper - 1);
		for(; pst_Helper >= pst_First; pst_Helper--)
		{
			pst_Helper->i->pfnv_Render(_pst_Helpers, pst_Helper, _pst_DD);
		}
	}

	/* rendering curve helper */
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 0);
	pst_First = _pst_Helpers->ast_HelperCurve;
	pst_Helper = pst_First + 16;
	for(; pst_First < pst_Helper; pst_First++)
	{
		pst_First->i->pfnv_Render(_pst_Helpers, pst_First, _pst_DD);
	}

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 1);
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthFunc, 1);

    /* rendering gizmo helper */
    SOFT_UVGizmo_Draw( &_pst_Helpers->st_UVGizmo );
    
    /* rendering prefab BV */
    if ( _pst_Helpers->pv_Prefab )
    {
		GDI_tdst_Request_DrawPointEx	st_Point;
		GDI_tdst_Request_DrawBox		st_Box;
		OBJ_tdst_Prefab					*P;
		
		GDI_SetViewMatrix((*GDI_gpst_CurDD), &_pst_DD->st_Camera.st_InverseMatrix);
		P = (OBJ_tdst_Prefab *) _pst_Helpers->pv_Prefab;
		
		if ( P->c_Show & 1)
		{
			st_Point.A = &_pst_Helpers->st_PrefabPos;
			st_Point.f_Size = 10;
			st_Point.ul_Color = 0xFF00FF00;
			_pst_DD->st_GDI.pfnl_Request( GDI_Cul_Request_DrawPointEx, (ULONG) &st_Point );
		}

		if ( (P->c_Show & 2) && !MATH_b_EqVector( &P->st_BVMin, &P->st_BVMax ) )
		{
			MATH_tdst_Matrix	M;
			MATH_tdst_Vector	vC, v1, vMin, vMax;
			
			MATH_MakeRotationMatrix_ZAxis( &M, P->f_Rotate, TRUE );
			
			st_Box.f_Width = 1;
			st_Box.ul_EdgeColor = 0xFF00FF00;
			st_Box.ul_SolidColor = 0x4F00FF00;
			st_Box.pst_Center = &vC;
			st_Box.pst_M = &M;

			MATH_SubVector(&v1, &P->st_BVMax, &P->st_BVMin);
			MATH_ScaleEqualVector(&v1, 0.5f);
			vMin.x = -fAbs(v1.x);
			vMin.y = -fAbs(v1.y);
			vMin.z = -fAbs(v1.z);
			vMax.x = fAbs(v1.x);
			vMax.y = fAbs(v1.y);
			vMax.z = fAbs(v1.z);

			MATH_CopyVector(&vC, &_pst_Helpers->st_PrefabPos);
			vC.z += vMax.z;

			st_Box.pst_Min = &vMin;
			st_Box.pst_Max = &vMax;
			_pst_DD->st_GDI.pfnl_Request( GDI_Cul_Request_DrawBox, (ULONG) &st_Box );
		}
    }

    _pst_DD->ul_DisplayInfo = ulSaveDI;
}

/*$4
 ***********************************************************************************************************************
    Private Functions for ellipse
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_EllipseRender(SOFT_tdst_Helpers *H, SOFT_tdst_Helper *_pst_Helper, GDI_tdst_DisplayData *_pst_DD)
{
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) _pst_Helper->p_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helpers_EllipsePick
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	char				_c_Start
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Ellipse	*pst_Ellipse;
	float				a, b, r, t, t2;
	MATH_tdst_Vector	C, M, N, V0, AB, AC;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Ellipse = (SOFT_tdst_Ellipse *) _pst_Helper->p_Data;

	MATH_CopyVector(&C, (MATH_tdst_Vector *) &pst_Ellipse->st_Center);
	MATH_CrossProduct(&N, &pst_Ellipse->st_A, &pst_Ellipse->st_B);
	MATH_NormalizeVector(&N, &N);
	r = MATH_f_NormVector(&pst_Ellipse->st_A);

	MATH_SubVector(&AB, B, A);
	MATH_SubVector(&AC, &C, A);

	t = MATH_f_DotProduct(&AB, &N);
	t2 = MATH_f_DotProduct(&AC, &N);
	if((fAbs(t) <= 0.05f) || (fAbs(t2) <= 0.05f))
	{
		MATH_SubVector(&M, B, &C);
		b = MATH_f_DotProduct(&M, &N);

		if(_c_Start)
			if(fAbs(b) > 0.01f) return 0;

		MATH_ScaleVector(&V0, &N, -b);
		MATH_AddVector(&M, &M, &V0);

		t = MATH_f_SqrVector(&pst_Ellipse->st_A) - MATH_f_SqrVector(&M);
		if(t < 0)
		{
			if(_c_Start) return 0;
			MATH_f_NormVector(&M);
			MATH_ScaleEqualVector(&M, r);
			M.z = 0;
		}
		else
		{
			M.z = -fSqrt(t);
		}
	}
	else
	{
		t = t2 / t;
		MATH_ScaleVector(&M, &AB, t);
		MATH_SubEqualVector(&M, &AC);
	}

	/* M is the intersection of projection line and circle plane */
	MATH_NormalizeVector(&V0, &M);
	MATH_ScaleEqualVector(&V0, r);

	if(_c_Start && (V0.z > 0)) return 0;

	a = MATH_f_DotProduct(&V0, &pst_Ellipse->st_A) / (r * r);
	if(a > 1.0f) a = 0;
	if(a < -1.0f)
		a = Cf_Pi;
	else
	{
		a = fAcos(a);
		b = MATH_f_DotProduct(&V0, &pst_Ellipse->st_B);
		if(b < 0) a = -a;
	}

	if(!_c_Start)
	{
		if
		(
			!(pst_Ellipse->l_Flag & SOFT_Cul_HF_LocalMode)
		&&	(pst_Ellipse->l_Flag & SOFT_Cul_HF_SAObject)
		&&	!(_pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapAngle)
		) 
            pst_Ellipse->f_AlphaStart = (pst_Ellipse->f_AlphaEnd - a) + pst_Ellipse->f_AlphaStart;
		else
			pst_Ellipse->f_AlphaEnd = a;
		return 1;
	}

	pst_Ellipse->f_AlphaDelta = 0.0f;
	pst_Ellipse->f_AlphaStart = pst_Ellipse->f_AlphaEnd = a;

	MATH_AddEqualVector(&V0, &C);

	SOFT_Helpers_Project(_pst_Helpers, &V0);
	MATH_AddEqualVector(&M, &C);
	SOFT_Helpers_Project(_pst_Helpers, &M);

	t = MATH_f_Distance(&M, &V0);

	if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
		return(t < 0.01);
	else
		return(t < (r *0.05) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_EllipseGetMove
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*_pst_Move
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Ellipse	*pst_Ellipse;
	float				f_Delta;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Ellipse = (SOFT_tdst_Ellipse *) _pst_Helper->p_Data;

	f_Delta = pst_Ellipse->f_AlphaEnd - pst_Ellipse->f_AlphaStart;

	if(_pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapAngle)
	{
		f_Delta = MATH_f_FloatModulo(f_Delta, _pst_Helpers->f_AngleSnap);
		if(!(pst_Ellipse->l_Flag & SOFT_Cul_HF_LocalMode) && (pst_Ellipse->l_Flag & SOFT_Cul_HF_SAObject))
			pst_Ellipse->f_AlphaStart -= (f_Delta - pst_Ellipse->f_AlphaDelta);
		pst_Ellipse->f_AlphaDelta = f_Delta;
	}

	switch(_pst_Helper->l_Id)
	{
	case SOFT_Cl_Helpers_RotationLocalX:
	case SOFT_Cl_Helpers_RotationGlobalX:
	case SOFT_Cl_Helpers_RotationCameraX:
		MATH_InitVector(_pst_Move, f_Delta, 0, 0);
		break;
	case SOFT_Cl_Helpers_RotationLocalY:
	case SOFT_Cl_Helpers_RotationGlobalY:
	case SOFT_Cl_Helpers_RotationCameraY:
		MATH_InitVector(_pst_Move, 0, f_Delta, 0);
		break;
	case SOFT_Cl_Helpers_RotationLocalZ:
	case SOFT_Cl_Helpers_RotationGlobalZ:
	case SOFT_Cl_Helpers_RotationCameraZ:
		MATH_InitVector(_pst_Move, 0, 0, f_Delta);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_EllipseGetAxe
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*_pst_Axe
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Ellipse	*pst_Ellipse;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Ellipse = (SOFT_tdst_Ellipse *) _pst_Helper->p_Data;

	MATH_CrossProduct(_pst_Axe, &pst_Ellipse->st_A, &pst_Ellipse->st_B);
	MATH_NormalizeVector(_pst_Axe, _pst_Axe);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_EllipseAdd
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	LONG				_l_Id,
	MATH_tdst_Vector	*_pst_Center,
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	LONG				_l_DataFlag,
	ULONG				_ul_ColorSel,
	ULONG				_ul_ColorUnsel
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Ellipse	*pst_Ellipse;
	SOFT_tdst_Helper	*pst_Helper;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

 	pst_Ellipse = &_pst_Helpers->ast_Ellipse[_pst_Helpers->l_CurrentEllipse++];
	MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, _pst_Center);
	MATH_CopyVector(&pst_Ellipse->st_A, _pst_A);
	MATH_CopyVector(&pst_Ellipse->st_B, _pst_B);

	pst_Ellipse->l_Flag = _l_DataFlag;
	if(_l_Id == _pst_Helpers->l_Pickable)
	{
		pst_Ellipse->st_Center.color = _ul_ColorSel;
		pst_Ellipse->l_Flag |= 1;
		if(_l_Id == _pst_Helpers->l_Picked) pst_Ellipse->l_Flag |= 2;
	}
	else
	{
		pst_Ellipse->st_Center.color = _ul_ColorUnsel;
	}

	pst_Helper = &_pst_Helpers->ast_Helper[_pst_Helpers->l_CurrentHelper++];
	M_SetHelper(pst_Helper, SOFT_Cl_HT_Ellipse, _l_Id, pst_Ellipse)
}

/*$4
 ***********************************************************************************************************************
    Private Functions for arrow
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_ArrowRender(SOFT_tdst_Helpers *_pst_H, SOFT_tdst_Helper *_pst_Helper, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~*/
	LONG	l_Scale;
	/*~~~~~~~~~~~~*/

	l_Scale = 0;

	if(SOFT_l_Helper_IsScale(_pst_Helper))
	{
		if(_pst_H->l_Picked != _pst_Helper->l_Id) ((SOFT_tdst_Arrow *) _pst_Helper->p_Data)->f_Move = 1.0f;
	}
	else if(_pst_H->ul_Flags & SOFT_Cul_HF_AllTogether)
	{
		l_Scale = 1;
		MATH_ScaleEqualVector(&((SOFT_tdst_Arrow *) _pst_Helper->p_Data)->st_Axis, 0.8f);
	}

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftArrow, (ULONG) _pst_Helper->p_Data);

	if(l_Scale)
	{
		MATH_ScaleEqualVector(&((SOFT_tdst_Arrow *) _pst_Helper->p_Data)->st_Axis, 1 / 0.8f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helpers_ArrowPick
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	char				_c_Start
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Arrow		*pst_Arrow;
	MATH_tdst_Vector	C, D, H1, H2, X, Y, Z;
	float				d, t1, t2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Arrow = (SOFT_tdst_Arrow *) _pst_Helper->p_Data;

	if(_c_Start)
	{
		MATH_CopyVector(&C, (MATH_tdst_Vector *) &pst_Arrow->st_Start);
		MATH_AddVector(&D, &C, &pst_Arrow->st_Axis);

		if(pst_Arrow->l_Flag & (SOFT_Cl_ArrowScale | SOFT_Cl_ArrowSpot))
		{
			if(!(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)) D.z = 0.0f;
			if(pst_Arrow->l_Flag & SOFT_Cl_ArrowScale)
				MATH_InitVector(&X, 10, 0, D.z);
			else
				MATH_InitVector(&X, 1, 0, D.z);

			/* MATH_InitVector(&X, 0.1f, 0, D.z); */
			CAM_2Dto3D(&_pst_Helpers->pst_DD->st_Camera, &Y, &X);
			MATH_InitVector(&X, 0, 0, D.z);
			CAM_2Dto3D(&_pst_Helpers->pst_DD->st_Camera, &Z, &X);
			MATH_SubEqualVector(&Y, &Z);
			d = MATH_f_NormVector(&Y);

			if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
				MATH_ScaleEqualVector(&D, 1 / D.z);
			else
				D.z = 1.0f;

			pst_Arrow->st_Start.u = pst_Arrow->f_Move = 1.0f;
			return(MATH_f_Distance(&D, B) < d);
		}
		else
		{
			if(SOFT_l_NearestPointOf2Lines(&C, &D, A, B, &d, &t1, &H1, &t2, &H2))
			{
				/*~~~~~~~~~~*/
				float	fCoef;
				/*~~~~~~~~~~*/

				if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
					fCoef = 0.01f;
				else
				{
					fCoef = 0.01f * (_pst_Helpers->pst_DD->st_Camera.f_IsoFactor * _pst_Helpers->pst_DD->st_Camera.f_IsoZoom * 2);
				}

				if(SOFT_gb_ForceStartPicking || ((d < fCoef) && (t1 > 0) && (t1 <= 1.05f)))
				{
					if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
						pst_Arrow->st_Start.u = t1 * _pst_Helpers->st_Center.z;
					else
						pst_Arrow->st_Start.u = t1;

					pst_Arrow->f_Move = 0;
					return 1;
				}
			}
		}

		return 0;
	}

	if(pst_Arrow->l_Flag & (SOFT_Cl_ArrowScale | SOFT_Cl_ArrowSpot))
	{
		MATH_CopyVector(&C, (MATH_tdst_Vector *) &pst_Arrow->st_Start);
		MATH_AddVector(&D, &C, &pst_Arrow->st_Axis);
		SOFT_l_NearestPointOf2Lines(&C, &D, A, B, &d, &t1, &H1, &t2, &H2);
		pst_Arrow->f_Move = (t1 < 0.1f) ? 0.1f : t1;
	}
	else
	{
		MATH_CopyVector(&C, (MATH_tdst_Vector *) &_pst_Helpers->st_Center);
		MATH_AddVector(&D, &C, &pst_Arrow->st_Axis);
		SOFT_l_NearestPointOf2Lines(&C, &D, A, B, &d, &t1, &H1, &t2, &H2);
		pst_Arrow->f_Move = t1 - pst_Arrow->st_Start.u;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_ArrowGetMove
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*_pst_Move
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Arrow *pst_Arrow;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Arrow = (SOFT_tdst_Arrow *) _pst_Helper->p_Data;

	if(pst_Arrow->l_Flag & SOFT_Cl_ArrowMove)
		MATH_ScaleVector(_pst_Move, &pst_Arrow->st_Axis, pst_Arrow->f_Move);
	else if(pst_Arrow->l_Flag & SOFT_Cl_ArrowScale)
	{
        if (_pst_Helpers->ul_Flags & SOFT_Cul_HF_SAObject || GDI_gpst_CurDD->uc_EditBounding)
        {
            switch(_pst_Helper->l_Id)
		    {
		    case SOFT_Cl_Helpers_ScaleLocalX:
			    MATH_InitVector(_pst_Move, pst_Arrow->f_Move, 1.0f, 1.0f);
			    break;
		    case SOFT_Cl_Helpers_ScaleLocalY:
			    MATH_InitVector(_pst_Move, 1.0f, pst_Arrow->f_Move, 1.0f);
			    break;
		    case SOFT_Cl_Helpers_ScaleLocalZ:
			    MATH_InitVector(_pst_Move, 1.0f, 1.0f, pst_Arrow->f_Move);
			    break;
            }
        }
        else
        {
            MATH_tdst_Vector V;

            MATH_TransformVector( &V, &_pst_Helpers->st_CamToLocalMatrix, &pst_Arrow->st_Axis );
            MATH_NormalizeEqualVector( &V );
            MATH_ScaleVector( _pst_Move, &V, pst_Arrow->f_Move );
        }
	}
	else if(pst_Arrow->l_Flag & SOFT_Cl_ArrowSpot)
	{
		MATH_InitVector(_pst_Move, pst_Arrow->f_Move, 0, 0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_ArrowAdd
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	LONG				_l_Id,
	MATH_tdst_Vector	*_pst_Start,
	MATH_tdst_Vector	*_pst_Axis,
	MATH_tdst_Vector	*_pst_Up,
	LONG				_l_DataFlag,
	ULONG				_ul_ColorSel,
	ULONG				_ul_ColorUnsel
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Arrow		*pst_Arrow;
	SOFT_tdst_Helper	*pst_Helper;
	MATH_tdst_Vector	st_Axis;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Add arrow only if axis is not Z oriented */
	MATH_NormalizeVector(&st_Axis, _pst_Axis);
	if(fAbs(st_Axis.z) > 0.95f) return;

	pst_Arrow = &_pst_Helpers->ast_Arrow[_pst_Helpers->l_CurrentArrow++];
	MATH_CopyVector((MATH_tdst_Vector *) &pst_Arrow->st_Start, _pst_Start);
	MATH_CopyVector(&pst_Arrow->st_Axis, _pst_Axis);
	MATH_CopyVector(&pst_Arrow->st_Up, _pst_Up);
	pst_Arrow->st_Start.color = (_l_Id == _pst_Helpers->l_Pickable) ? _ul_ColorSel : _ul_ColorUnsel;
	pst_Arrow->l_Flag = _l_DataFlag;

	pst_Helper = &_pst_Helpers->ast_Helper[_pst_Helpers->l_CurrentHelper++];
	M_SetHelper(pst_Helper, SOFT_Cl_HT_Arrow, _l_Id, pst_Arrow)
}

/*$4
 ***********************************************************************************************************************
    Private Functions for square
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_SquareRender(SOFT_tdst_Helpers *H, SOFT_tdst_Helper *_pst_Helper, GDI_tdst_DisplayData *_pst_DD)
{
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftSquare, (ULONG) _pst_Helper->p_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helpers_SquarePick
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	char				_c_Start
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Square	*pst_Square;
	MATH_tdst_Vector	C, D, E, H;
	float				t;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Square = (SOFT_tdst_Square *) _pst_Helper->p_Data;

	if(_c_Start)
	{
		/*~~~~~~~~~~*/
		float	fCoef;
		/*~~~~~~~~~~*/

		MATH_CopyVector(&C, (MATH_tdst_Vector *) &pst_Square->st_Pos);

		if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
			fCoef = MATH_f_NormVector( &pst_Square->st_A ) * 0.15f;
		else
		{
			fCoef = 0.015f *
				_pst_Helpers->pst_DD->st_Camera.f_IsoFactor *
				_pst_Helpers->pst_DD->st_Camera.f_IsoZoom *
				2;
		}

        MATH_SubVector( &pst_Square->st_PosDecal, B, &C );
		return (fAbs (C.x -B->x) < fCoef) && (fAbs(C.y - B->y) < fCoef);
	}

    MATH_CopyVector( &C, &pst_Square->st_PosDecal );
    if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
        MATH_ScaleEqualVector( &C, _pst_Helpers->st_Center.z );

	//MATH_CopyVector(&C, (MATH_tdst_Vector *) &_pst_Helpers->st_Center);
    //MATH_AddVector(&C, (MATH_tdst_Vector *) &_pst_Helpers->st_Center, &pst_Square->st_PosDecal );
    MATH_AddEqualVector(&C, (MATH_tdst_Vector *) &_pst_Helpers->st_Center );
	MATH_CopyVector(&D, &pst_Square->st_A);
	MATH_CopyVector(&E, &pst_Square->st_B);

	if(pst_Square->l_Flag & 1)
	{
		if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
			t = 1.0f + (B->x - C.x / C.z);
		else
		{
			t = 1.0f + (B->x - C.x) / (_pst_Helpers->pst_DD->st_Camera.f_IsoFactor * _pst_Helpers->pst_DD->st_Camera.f_IsoZoom);
		}

		if(t < .1) t = .1f;
		MATH_InitVector(&pst_Square->st_Move, t, t, t);
		return 0;
	}

	if(SOFT_l_IntersectPlaneLine(A, B, &C, &D, &E, &H))
		MATH_SubVector(&pst_Square->st_Move, &H, &C);
	else
		MATH_InitVectorToZero(&pst_Square->st_Move);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_SquareGetMove
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*_pst_Move
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Square	*pst_Square;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Square = (SOFT_tdst_Square *) _pst_Helper->p_Data;
	MATH_CopyVector(_pst_Move, (MATH_tdst_Vector *) &pst_Square->st_Move);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_SquareAdd
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	LONG				_l_Id,
	MATH_tdst_Vector	*_pst_Start,
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	LONG				_l_DataFlag,
	ULONG				_ul_ColorSel,
	ULONG				_ul_ColorUnsel
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Square	*pst_Square;
	SOFT_tdst_Helper	*pst_Helper;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Square = &_pst_Helpers->st_Square;
	MATH_CopyVector((MATH_tdst_Vector *) &pst_Square->st_Pos, _pst_Start);
	MATH_CopyVector(&pst_Square->st_A, _pst_A);
	MATH_CopyVector(&pst_Square->st_B, _pst_B);
	pst_Square->st_Pos.color = (_l_Id == _pst_Helpers->l_Pickable) ? _ul_ColorSel : _ul_ColorUnsel;
	pst_Square->l_Flag = _l_DataFlag;

	pst_Helper = &_pst_Helpers->ast_Helper[_pst_Helpers->l_CurrentHelper++];
	M_SetHelper(pst_Helper, SOFT_Cl_HT_Square, _l_Id, pst_Square)
}

/*$4
 ***********************************************************************************************************************
    Private Functions for light helpers
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helpers_OmniPick
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	char				_c_Start
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Ellipse	*pst_Ellipse;
	MATH_tdst_Vector	C, D, E, H;
	float				n1, n2, t;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Ellipse = (SOFT_tdst_Ellipse *) _pst_Helper->p_Data;

	MATH_CopyVector(&C, (MATH_tdst_Vector *) &pst_Ellipse->st_Center);
	MATH_CopyVector(&D, &pst_Ellipse->st_A);
	MATH_CopyVector(&E, &pst_Ellipse->st_B);
	SOFT_l_IntersectPlaneLine(A, B, &C, &D, &E, &H);

	if(_c_Start)
	{
		n1 = MATH_f_Distance(&C, &H);
		n2 = MATH_f_NormVector(&pst_Ellipse->st_A);
		pst_Ellipse->f_AlphaStart = pst_Ellipse->f_AlphaEnd = n2;
		t = (float) fAbs(n2 - n1);
		t /= C.z;
		return(t < .01f);
	}

	MATH_SubEqualVector(&H, &C);
	pst_Ellipse->f_AlphaEnd = MATH_f_NormVector(&H);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_OmniGetMove
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*_pst_Move
)
{
	_pst_Move->x = ((SOFT_tdst_Ellipse *) _pst_Helper->p_Data)->f_AlphaEnd;
	_pst_Move->y = _pst_Move->z = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SOFT_b_Helpers_AddLight(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_Helpers	*pst_Helpers;
	OBJ_tdst_GameObject *pst_GO;
	LIGHT_tdst_Light	*pst_Light;
	SOFT_tdst_Ellipse	*pst_Ellipse;
	SOFT_tdst_Arrow		*pst_Arrow;
	SOFT_tdst_Helper	*pst_Helper;
	MATH_tdst_Vector	st_Center, A, B, C, D, E;
	MATH_tdst_Vector	*X, *Y, *Z;
	float				t;
#ifdef JADEFUSION
	MATH_tdst_Vector	st_Temp, st_Far;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Helpers = (SOFT_tdst_Helpers *) _ul_Param1;
	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;

    
    if (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden) 
        return TRUE;

	if(pst_GO && (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights) && (pst_GO->pst_Extended->pst_Light))
	{
		MATH_TransformVertex
		(
			&st_Center,
			pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix,
			OBJ_pst_GetAbsolutePosition(pst_GO)
		);

		if(st_Center.z < 0) return TRUE;

		pst_Light = (LIGHT_tdst_Light *) pst_GO->pst_Extended->pst_Light;
		switch(pst_Light->ul_Flags & LIGHT_Cul_LF_Type)
		{
		case LIGHT_Cul_LF_Omni:
			if(pst_Helpers->l_CurrentOmni >= 8) break;

			pst_Ellipse = &pst_Helpers->ast_OmniNear[pst_Helpers->l_CurrentOmni];

			pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
			M_SetHelper
			(
				pst_Helper,
				SOFT_Cl_HT_Omni,
				SOFT_Cl_Helpers_OmniNear1 + pst_Helpers->l_CurrentOmni,
				pst_Ellipse
			) MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &st_Center);
			MATH_InitVector(&pst_Ellipse->st_A, pst_Light->st_Omni.f_Near, 0, 0);
			MATH_InitVector(&pst_Ellipse->st_B, 0, pst_Light->st_Omni.f_Near, 0);
			if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
				pst_Ellipse->st_Center.color = 0xFF00FFFF;
			else
				pst_Ellipse->st_Center.color = 0xFFFF0000;
			pst_Ellipse->l_Flag = 4;

			pst_Ellipse = &pst_Helpers->ast_OmniFar[pst_Helpers->l_CurrentOmni];

			pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
			M_SetHelper
			(
				pst_Helper,
				SOFT_Cl_HT_Omni,
				SOFT_Cl_Helpers_OmniFar1 + pst_Helpers->l_CurrentOmni,
				pst_Ellipse
			) MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &st_Center);
			MATH_InitVector(&pst_Ellipse->st_A, pst_Light->st_Omni.f_Far, 0, 0);
			MATH_InitVector(&pst_Ellipse->st_B, 0, pst_Light->st_Omni.f_Far, 0);
			if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
				pst_Ellipse->st_Center.color = 0xFF00FFFF;
			else
				pst_Ellipse->st_Center.color = 0xFFFF0000;
			pst_Ellipse->l_Flag = 4;

			pst_Helpers->ap_Omni[pst_Helpers->l_CurrentOmni++] = pst_Light;
			break;

		case LIGHT_Cul_LF_Spot:
			if(pst_Helpers->l_CurrentSpot >= 8) break;

			X = MATH_pst_GetXAxis(OBJ_pst_GetAbsoluteMatrix(pst_GO));
			Y = MATH_pst_GetYAxis(OBJ_pst_GetAbsoluteMatrix(pst_GO));
			Z = MATH_pst_GetZAxis(OBJ_pst_GetAbsoluteMatrix(pst_GO));

			MATH_ScaleVector(&A, Y, -pst_Light->st_Spot.f_Near);
			MATH_AddVector(&B, OBJ_pst_GetAbsolutePosition(pst_GO), &A);
			MATH_TransformVertex(&C, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &B);

			pst_Ellipse = &pst_Helpers->ast_SpotLittleAlpha[pst_Helpers->l_CurrentSpot];

			pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
			M_SetHelper
			(
				pst_Helper,
				SOFT_Cl_HT_Omni,
				SOFT_Cl_Helpers_SpotLittleAlpha1 + pst_Helpers->l_CurrentSpot,
				pst_Ellipse
			) MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &C);
			t = pst_Light->st_Spot.f_Near * fTan(pst_Light->st_Spot.f_LittleAlpha);

			MATH_ScaleVector(&A, Z, t);
			MATH_AddEqualVector(&A, &B);
			MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
			MATH_SubVector(&pst_Ellipse->st_A, &D, &C);
			MATH_ScaleVector(&A, X, t);
			MATH_AddEqualVector(&A, &B);
			MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
			MATH_SubVector(&pst_Ellipse->st_B, &D, &C);

			if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
				pst_Ellipse->st_Center.color = 0xFF00FFFF;
			else
				pst_Ellipse->st_Center.color = 0xFFFF0000;
			pst_Ellipse->l_Flag = 5;

#ifdef JADEFUSION
			// modify gizmo if the spot light is cylindrical
			
			//additional helpers
			if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
			{
				// First little Alpha ellipse helper
				MATH_AddVector(&B, OBJ_pst_GetAbsolutePosition(pst_GO), Y);
				MATH_TransformVertex(&C, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &B);

				pst_Ellipse = &pst_Helpers->ast_Cyl_SpotLittleAlphaStart[pst_Helpers->l_CurrentSpot];

				pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
				M_SetHelper
					(
					pst_Helper,
					SOFT_Cl_HT_Omni,
					SOFT_Cl_Helpers_CylSpotLittleAlphaStart1 + pst_Helpers->l_CurrentSpot,
					pst_Ellipse
					);
				MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &st_Center);
				t = pst_Light->st_Spot.f_Near * fTan(pst_Light->st_Spot.f_LittleAlpha);

				MATH_ScaleVector(&A, Z, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_A, &D, &C);
				MATH_ScaleVector(&A, X, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_B, &D, &C);

				if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
					pst_Ellipse->st_Center.color = 0xFF00FFFF;
				else
					pst_Ellipse->st_Center.color = 0xFFFF0000;
				pst_Ellipse->l_Flag = 5;

				// First Big Alpha ellipse helper
				pst_Ellipse = &pst_Helpers->ast_Cyl_SpotBigAlphaStart[pst_Helpers->l_CurrentSpot];

				pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
				M_SetHelper
					(
					pst_Helper,
					SOFT_Cl_HT_Omni,
					SOFT_Cl_Helpers_CylSpotBigAlphaStart1 + pst_Helpers->l_CurrentSpot,
					pst_Ellipse
					);
				MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &st_Center);
				t = pst_Light->st_Spot.f_Far * fTan(pst_Light->st_Spot.f_BigAlpha);

				MATH_ScaleVector(&A, Z, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_A, &D, &C);
				MATH_ScaleVector(&A, X, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_B, &D, &C);

				if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
					pst_Ellipse->st_Center.color = 0xFF00FFFF;
				else
					pst_Ellipse->st_Center.color = 0xFFFF0000;
				pst_Ellipse->l_Flag = 5;	
			}

			// Additional far Little Alpha ellipse helper
			if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
			{
				MATH_ScaleVector(&A, Y, -pst_Light->st_Spot.f_Far);
				MATH_AddVector(&B, OBJ_pst_GetAbsolutePosition(pst_GO), &A);
				MATH_TransformVertex(&C, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &B);
				MATH_CopyVector(&st_Far, &C);

				pst_Ellipse = &pst_Helpers->ast_Cyl_SpotLittleAlpha[pst_Helpers->l_CurrentSpot];

#else//JADEFUSION
			MATH_SubVector(&A, &C, &st_Center);
			MATH_NormalizeVector(&E, &A);
			if(fAbs(E.z) <= 0.95f)
			{
				pst_Arrow = &pst_Helpers->ast_SpotNear[pst_Helpers->l_CurrentSpot];
#endif//JADEFUSION
				pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
#ifdef JADEFUSION
				M_SetHelper
				(
					pst_Helper,

					SOFT_Cl_HT_Omni,
					SOFT_Cl_Helpers_CylSpotLittleAlpha1 + pst_Helpers->l_CurrentSpot,
					pst_Ellipse
					);
				MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &C);
				t = pst_Light->st_Spot.f_Near * fTan(pst_Light->st_Spot.f_LittleAlpha);

				MATH_ScaleVector(&A, Z, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_A, &D, &C);
				MATH_ScaleVector(&A, X, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_B, &D, &C);
				if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
					pst_Ellipse->st_Center.color = 0xFF00FFFF;
				else
					pst_Ellipse->st_Center.color = 0xFFFF0000;
				pst_Ellipse->l_Flag = 5;
			}

			// Near arrow helper
			if(!(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical))
			{
				MATH_SubVector(&A, &C, &st_Center);
				MATH_NormalizeVector(&E, &A);
				if(fAbs(E.z) <= 0.95f)
				{
					pst_Arrow = &pst_Helpers->ast_SpotNear[pst_Helpers->l_CurrentSpot];

					pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
					M_SetHelper
						(
						pst_Helper,
						SOFT_Cl_HT_Arrow,
						SOFT_Cl_Helpers_SpotNear1 + pst_Helpers->l_CurrentSpot,
						pst_Arrow
						);
					MATH_CopyVector((MATH_tdst_Vector *) &pst_Arrow->st_Start, &st_Center);
					MATH_CopyVector(&pst_Arrow->st_Axis, &A);
					MATH_CopyVector(&pst_Arrow->st_Up, &pst_Ellipse->st_A);
					if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
						pst_Arrow->st_Start.color = 0xFF00FFFF;
					else
						pst_Arrow->st_Start.color = 0xFFFF0000;
					pst_Arrow->l_Flag = SOFT_Cl_ArrowSpot | SOFT_Cul_Arrow_Cube | SOFT_Cul_Arrow_ForSpot;
				}
			}
#else //JADE FUSION
				M_SetHelper
				(
					pst_Helper,
					SOFT_Cl_HT_Arrow,
					SOFT_Cl_Helpers_SpotNear1 + pst_Helpers->l_CurrentSpot,
					pst_Arrow
				);
				MATH_CopyVector((MATH_tdst_Vector *) &pst_Arrow->st_Start, &st_Center);
				MATH_CopyVector(&pst_Arrow->st_Axis, &A);
				MATH_CopyVector(&pst_Arrow->st_Up, &pst_Ellipse->st_A);
				if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
					pst_Arrow->st_Start.color = 0xFF00FFFF;
				else
					pst_Arrow->st_Start.color = 0xFFFF0000;
				pst_Arrow->l_Flag = SOFT_Cl_ArrowSpot | SOFT_Cul_Arrow_Cube | SOFT_Cul_Arrow_ForSpot;
			}
#endif
			MATH_ScaleVector(&A, Y, -pst_Light->st_Spot.f_Far);
			MATH_AddVector(&B, OBJ_pst_GetAbsolutePosition(pst_GO), &A);
			MATH_TransformVertex(&C, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &B);

			pst_Ellipse = &pst_Helpers->ast_SpotBigAlpha[pst_Helpers->l_CurrentSpot];

			pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
			M_SetHelper
			(
				pst_Helper,
				SOFT_Cl_HT_Omni,
				SOFT_Cl_Helpers_SpotBigAlpha1 + pst_Helpers->l_CurrentSpot,
				pst_Ellipse
			) MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &C);
			t = pst_Light->st_Spot.f_Far * fTan(pst_Light->st_Spot.f_BigAlpha);
			MATH_ScaleVector(&A, Z, t);
			MATH_AddEqualVector(&A, &B);
			MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
			MATH_SubVector(&pst_Ellipse->st_A, &D, &C);
			MATH_ScaleVector(&A, X, t);
			MATH_AddEqualVector(&A, &B);
			MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
			MATH_SubVector(&pst_Ellipse->st_B, &D, &C);
			if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
				pst_Ellipse->st_Center.color = 0xFF00FFFF;
			else
				pst_Ellipse->st_Center.color = 0xFFFF0000;
			pst_Ellipse->l_Flag = 5;

/*#ifdef JADEFUSION
			// Far arrow helper for cylindrical
			if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)

			{
				MATH_SubVector(&A, &C, &st_Center);
				MATH_NormalizeVector(&E, &A);
				if(fAbs(E.z) <= 0.95f)
				{
#else*/
			MATH_SubVector(&A, &C, &st_Center);
			MATH_NormalizeVector(&E, &A);
			if(fAbs(E.z) <= 0.95f)
			{
//#endif
				pst_Arrow = &pst_Helpers->ast_SpotFar[pst_Helpers->l_CurrentSpot];

				pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
				M_SetHelper
				(
					pst_Helper,
					SOFT_Cl_HT_Arrow,
					SOFT_Cl_Helpers_SpotFar1 + pst_Helpers->l_CurrentSpot,
					pst_Arrow
				) MATH_CopyVector((MATH_tdst_Vector *) &pst_Arrow->st_Start, &st_Center);
				MATH_CopyVector(&pst_Arrow->st_Axis, &A);
				MATH_CopyVector(&pst_Arrow->st_Up, &pst_Ellipse->st_A);
				if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
					pst_Arrow->st_Start.color = 0xFF00FFFF;
				else
					pst_Arrow->st_Start.color = 0xFFFF0000;
/*#ifdef JADEFUSION
				pst_Arrow->l_Flag = SOFT_Cl_ArrowSpot | SOFT_Cul_Arrow_Cube | SOFT_Cul_Arrow_ForSpot | SOFT_Cul_Arrow_DrawNotLine;
				}
			}

			// Additional near Big Alpha ellipse helper
			if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
			{
				MATH_ScaleVector(&A, Y, -pst_Light->st_Spot.f_Near);
				MATH_AddVector(&B, OBJ_pst_GetAbsolutePosition(pst_GO), &A);
				MATH_TransformVertex(&C, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &B);

				pst_Ellipse = &pst_Helpers->ast_Cyl_SpotBigAlpha[pst_Helpers->l_CurrentSpot];

				pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
				M_SetHelper
					(
					pst_Helper,
					SOFT_Cl_HT_Omni,
					SOFT_Cl_Helpers_CylSpotBigAlpha1 + pst_Helpers->l_CurrentSpot,
					pst_Ellipse
					);
				MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &C);
				t = pst_Light->st_Spot.f_Far * fTan(pst_Light->st_Spot.f_BigAlpha);
				MATH_ScaleVector(&A, Z, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_A, &D, &C);
				MATH_ScaleVector(&A, X, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_B, &D, &C);
				if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
					pst_Ellipse->st_Center.color = 0xFF00FFFF;
				else
					pst_Ellipse->st_Center.color = 0xFFFF0000;
				pst_Ellipse->l_Flag = 5;
			}

			// Near Arrow helper for cylindrical
			if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
			{
				MATH_SubVector(&A, &C, &st_Center);
				MATH_NormalizeVector(&E, &A);
				if(fAbs(E.z) <= 0.95f)
				{
					pst_Arrow = &pst_Helpers->ast_SpotNear[pst_Helpers->l_CurrentSpot];

					pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
					M_SetHelper
					(
						pst_Helper, 
						SOFT_Cl_HT_Arrow,
						SOFT_Cl_Helpers_SpotNear1 + pst_Helpers->l_CurrentSpot,
						pst_Arrow
					);
					MATH_CopyVector((MATH_tdst_Vector *) &pst_Arrow->st_Start, &st_Center);
					MATH_CopyVector(&pst_Arrow->st_Axis, &A);
					MATH_CopyVector(&pst_Arrow->st_Up, &pst_Ellipse->st_A);
					if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
						pst_Arrow->st_Start.color = 0xFF00FFFF;
					else
						pst_Arrow->st_Start.color = 0xFFFF0000;
					pst_Arrow->l_Flag = SOFT_Cl_ArrowSpot | SOFT_Cul_Arrow_Cube | SOFT_Cul_Arrow_ForSpot | SOFT_Cul_Arrow_DrawNotLine;
				}
			}
			// Far Arrow helper
			else
			{
				MATH_SubVector(&A, &C, &st_Center);
				MATH_NormalizeVector(&E, &A);
				if(fAbs(E.z) <= 0.95f)
				{
					pst_Arrow = &pst_Helpers->ast_SpotFar[pst_Helpers->l_CurrentSpot];

					pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
					M_SetHelper
						(
						pst_Helper,
						SOFT_Cl_HT_Arrow,
						SOFT_Cl_Helpers_SpotFar1 + pst_Helpers->l_CurrentSpot,
						pst_Arrow
						);
					MATH_CopyVector((MATH_tdst_Vector *) &pst_Arrow->st_Start, &st_Center);
					MATH_CopyVector(&pst_Arrow->st_Axis, &A);
					MATH_CopyVector(&pst_Arrow->st_Up, &pst_Ellipse->st_A);
					if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
						pst_Arrow->st_Start.color = 0xFF00FFFF;
					else
						pst_Arrow->st_Start.color = 0xFFFF0000;
					pst_Arrow->l_Flag = SOFT_Cl_ArrowSpot | SOFT_Cul_Arrow_Cube | SOFT_Cul_Arrow_ForSpot;
				}
			}

			// additional lines for cylindrical light
			if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
			{
				MATH_SubVector(&A, &C, &st_Center);
				MATH_NormalizeAnyVector(&E, &A);
				if(fAbs(E.z) <= 0.95f)
				{
					pst_Arrow = &pst_Helpers->ast_Cyl_SpotExtraLinesH[pst_Helpers->l_CurrentSpot];

					pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
					M_SetHelper
						(
						pst_Helper,
						SOFT_Cl_HT_Arrow,
						SOFT_Cl_Helpers_CylSpotExtraLinesH + pst_Helpers->l_CurrentSpot,
						pst_Arrow
						);
					// Start at camera pos
					MATH_CopyVector((MATH_tdst_Vector*)&pst_Arrow->st_Start, &st_Center);

					// End at the far plane
					MATH_SubVector(&pst_Arrow->st_Axis, &st_Far, &st_Center);

					MATH_CopyVector(&pst_Arrow->st_Up, &pst_Ellipse->st_A);
					if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
						pst_Arrow->st_Start.color = 0xFF00FFFF;
					else
						pst_Arrow->st_Start.color = 0xFFFF0000;
					pst_Arrow->l_Flag = SOFT_Cul_Arrow_DrawParallel | SOFT_Cul_Arrow_NoEndPoint;
					// -------------------------------------------------------------------------
					pst_Arrow = &pst_Helpers->ast_Cyl_SpotExtraLinesV[pst_Helpers->l_CurrentSpot];

					pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
					M_SetHelper
						(
						pst_Helper,
						SOFT_Cl_HT_Arrow,
						SOFT_Cl_Helpers_CylSpotExtraLinesV + pst_Helpers->l_CurrentSpot,
						pst_Arrow
						);
					//same as previous helper
					memcpy(pst_Arrow, &pst_Helpers->ast_Cyl_SpotExtraLinesH[pst_Helpers->l_CurrentSpot], sizeof(SOFT_tdst_Arrow));

					//rotation of 90 degres
					MATH_NormalizeAnyVector(&st_Temp, &pst_Arrow->st_Axis);
					MATH_CrossProduct(&E, &st_Temp, &pst_Arrow->st_Up);
					MATH_CopyVector(&pst_Arrow->st_Up, &E);
				}
			}
	
			// Shadow helpers
			if((pst_Light->ul_Flags & LIGHT_Cul_LF_ExtendedShadowLight)
                && pst_Light->b_UseHiResFOV
				&& !(GetAsyncKeyState(VK_CONTROL) < 0))
			{
				if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
				{
					// First Shadow Alpha ellipse helper
					MATH_AddVector(&B, OBJ_pst_GetAbsolutePosition(pst_GO), Y);
					MATH_TransformVertex(&C, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &B);

					pst_Ellipse = &pst_Helpers->ast_Cyl_ShadowAlpha[pst_Helpers->l_CurrentSpot];

					pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
					M_SetHelper
						(
						pst_Helper,
						SOFT_Cl_HT_Omni,
						SOFT_Cl_Helpers_CylSpotShadowAlpha1 + pst_Helpers->l_CurrentSpot,
						pst_Ellipse
						);
					MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &st_Center);
					t = pst_Light->st_Spot.f_Far * fTan(pst_Light->f_HiResFOV);

					MATH_ScaleVector(&A, Z, t);
					MATH_AddEqualVector(&A, &B);
					MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
					MATH_SubVector(&pst_Ellipse->st_A, &D, &C);
					MATH_ScaleVector(&A, X, t);
					MATH_AddEqualVector(&A, &B);
					MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
					MATH_SubVector(&pst_Ellipse->st_B, &D, &C);

					if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
						pst_Ellipse->st_Center.color = 0xFF00FFFF;
					else
						pst_Ellipse->st_Center.color = 0x00FFFF00;
					pst_Ellipse->l_Flag = 5;
				}

				// Shadow Alpha ellipse helper (NEAR)
				MATH_ScaleVector(&A, Y, -pst_Light->f_ShadowNear);
				MATH_AddVector(&B, OBJ_pst_GetAbsolutePosition(pst_GO), &A);
				MATH_TransformVertex(&C, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &B);

				pst_Ellipse = &pst_Helpers->ast_ShadowAlpha[pst_Helpers->l_CurrentSpot];

				pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
				M_SetHelper
					(
					pst_Helper,
					SOFT_Cl_HT_Omni,
					SOFT_Cl_Helpers_SpotShadowAlpha1 + pst_Helpers->l_CurrentSpot,
					pst_Ellipse
					);
				MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &C);
				
				if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
					t = pst_Light->st_Spot.f_Far * fTan(pst_Light->f_HiResFOV);
				else
					t = pst_Light->f_ShadowNear * fTan(pst_Light->f_HiResFOV);
				
				MATH_ScaleVector(&A, Z, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_A, &D, &C);
				MATH_ScaleVector(&A, X, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_B, &D, &C);
				if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
					pst_Ellipse->st_Center.color = 0xFF00FFFF;
				else
					pst_Ellipse->st_Center.color = 0x00FFFF00;
				pst_Ellipse->l_Flag = 5;			

				// Shadow Near Arrow helper
				MATH_SubVector(&A, &C, &st_Center);
				MATH_NormalizeVector(&E, &A);
				if(fAbs(E.z) <= 0.95f)
				{
					pst_Arrow = &pst_Helpers->ast_ShadowNear[pst_Helpers->l_CurrentSpot];

					pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
					M_SetHelper
						(
						pst_Helper, 
						SOFT_Cl_HT_Arrow,
						SOFT_Cl_Helpers_SpotShadowNear1 + pst_Helpers->l_CurrentSpot,
						pst_Arrow
						);
					MATH_CopyVector((MATH_tdst_Vector *) &pst_Arrow->st_Start, &st_Center);
					MATH_CopyVector(&pst_Arrow->st_Axis, &A);
					MATH_CopyVector(&pst_Arrow->st_Up, &pst_Ellipse->st_A);
					if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
						pst_Arrow->st_Start.color = 0xFF00FFFF;
					else
						pst_Arrow->st_Start.color = 0x00FFFF00;
					pst_Arrow->l_Flag = SOFT_Cl_ArrowSpot | SOFT_Cul_Arrow_Cube | SOFT_Cul_Arrow_ForSpot |SOFT_Cul_Arrow_DrawNotLine;
				}

				// Shadow Alpha ellipse helper (FAR)
				MATH_ScaleVector(&A, Y, -pst_Light->st_Spot.f_Far);
				MATH_AddVector(&B, OBJ_pst_GetAbsolutePosition(pst_GO), &A);
				MATH_TransformVertex(&C, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &B);

				pst_Ellipse = &pst_Helpers->ast_ShadowFarAlpha[pst_Helpers->l_CurrentSpot];

				pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
				M_SetHelper
					(
					pst_Helper,
					SOFT_Cl_HT_Omni,
					SOFT_Cl_Helpers_SpotShadowFarAlpha1 + pst_Helpers->l_CurrentSpot,
					pst_Ellipse
					);
				MATH_CopyVector((MATH_tdst_Vector *) &pst_Ellipse->st_Center, &C);
				t = pst_Light->st_Spot.f_Far * fTan(pst_Light->f_HiResFOV);
				MATH_ScaleVector(&A, Z, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_A, &D, &C);
				MATH_ScaleVector(&A, X, t);
				MATH_AddEqualVector(&A, &B);
				MATH_TransformVertex(&D, pst_Helpers->pst_DD->st_MatrixStack.pst_CurrentMatrix, &A);
				MATH_SubVector(&pst_Ellipse->st_B, &D, &C);
				if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
					pst_Ellipse->st_Center.color = 0xFF00FFFF;
				else
					pst_Ellipse->st_Center.color = 0x00FFFF00;
				pst_Ellipse->l_Flag = 5;

				// Shadow Far Arrow helper (Only for lines, not for manipulate the Far parameter)
				MATH_SubVector(&A, &C, &st_Center);
				MATH_NormalizeVector(&E, &A);
				if(fAbs(E.z) <= 0.95f)
				{
					pst_Arrow = &pst_Helpers->ast_ShadowFar[pst_Helpers->l_CurrentSpot];

					pst_Helper = &pst_Helpers->ast_Helper[pst_Helpers->l_CurrentHelper++];
					M_SetHelper
						(
						pst_Helper, 
						SOFT_Cl_HT_Arrow,
						SOFT_Cl_Helpers_SpotShadowFar1 + pst_Helpers->l_CurrentSpot,
						pst_Arrow
						);
					MATH_CopyVector((MATH_tdst_Vector *) &pst_Arrow->st_Start, &st_Center);
					MATH_CopyVector(&pst_Arrow->st_Axis, &A);
					MATH_CopyVector(&pst_Arrow->st_Up, &pst_Ellipse->st_A);

					if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
						pst_Arrow->l_Flag = SOFT_Cl_ArrowSpot | SOFT_Cul_Arrow_DrawParallel | SOFT_Cul_Arrow_NoEndPoint | SOFT_Cul_Arrow_ForShadow;
					else
						pst_Arrow->l_Flag = SOFT_Cl_ArrowSpot | SOFT_Cul_Arrow_ForSpot |SOFT_Cul_Arrow_NoEndPoint | SOFT_Cul_Arrow_ForShadow;

					if(pst_Helpers->l_Pickable == pst_Helper->l_Id)
					{
						pst_Arrow->st_Start.color = 0xFF00FFFF;
						pst_Arrow->l_Flag &= ~SOFT_Cul_Arrow_ForShadow;
					}
					else
						pst_Arrow->st_Start.color = 0x00FFFF00;
				}
#else //JADEFUSION*/

			pst_Arrow->l_Flag = SOFT_Cl_ArrowSpot | SOFT_Cul_Arrow_Cube | SOFT_Cul_Arrow_ForSpot;
//#endif
			}

			pst_Helpers->ap_Spot[pst_Helpers->l_CurrentSpot++] = pst_Light;

			break;
		}
	}

	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    Private Functions for curve
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_CurveRender(SOFT_tdst_Helpers *H, SOFT_tdst_Helper *_pst_Helper, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_HelperCurve	*pst_Curve;
	OBJ_tdst_GameObject		*pst_GO;
	EVE_tdst_ListTracks		*pst_Tracks;
	EVE_tdst_Track			*pst_TrackTrans, *pst_TrackRot;
	EVE_tdst_Event			*pst_Evt, *pst_NextEvt;
	MATH_tdst_Vector		M[2], *pM[2], v[4], *pV[2], *pst_Pos, *pst_PosNext;
	float					t, omt, dt, totaltime;
	ULONG					ul_SaveColor, ul_ColSeg, ul_ColInter;
	MATH_tdst_Matrix		st_Matrix, st_Temp, st_Rot;
	int						i, j, nbframes;
	short					w_Type;
	float					*pf_TimeKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Curve = (SOFT_tdst_HelperCurve *) _pst_Helper->p_Data;
	if(!(pst_Curve->ul_Flags & SOFT_Cl_CurveUsed)) return;
	if(pst_Curve->ul_Flags & SOFT_Cl_AnimCurve)
	{
		if(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_DrawCurve) SOFT_Helpers_CurveRender2(H, _pst_Helper, _pst_DD);
		return;
	}

	ul_SaveColor = _pst_DD->ul_ColorConstant;
	pM[0] = &M[0];
	pM[1] = &M[1];

	pst_GO = (OBJ_tdst_GameObject *) pst_Curve->p_CurGO;
	pst_TrackTrans = (EVE_tdst_Track *) pst_Curve->p_Track[SOFT_Cl_TrackTrans];
	pst_TrackRot = (EVE_tdst_Track *) pst_Curve->p_Track[SOFT_Cl_TrackRot];

	pst_Tracks = ((EVE_tdst_Data *) pst_Curve->p_EventData)->pst_ListTracks;

	EVE_Event_InterpolationKey_GetPathInitMatrix
	(
		pst_TrackTrans,
		pst_GO,
		&st_Temp
	);
	MATH_MulMatrixMatrix(&st_Matrix, &st_Temp, &_pst_DD->st_Camera.st_InverseMatrix);

	if(pst_TrackTrans)
	{
		pst_Evt = pst_TrackTrans->pst_AllEvents;
		for(i = 0; i < pst_TrackTrans->uw_NumEvents; i++, pst_Evt++)
		{
            if (!pst_Evt) continue;
			if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) continue;
			ul_ColSeg = 0xFFFF0000;
			ul_ColInter = 0xFF0000FF;
			if(pst_Evt->w_Flags & EVE_C_EventFlag_Selected) 
			{
				ul_ColSeg = 0xFFFFFFFF;
				ul_ColInter = 0xFFFFFFFF;
			}

			if(i == pst_TrackTrans->uw_NumEvents - 1)
				pst_NextEvt = pst_TrackTrans->pst_AllEvents;
			else
				pst_NextEvt = pst_Evt + 1;

			w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Evt);
			pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(pst_Evt);
			MATH_TransformVertex(&v[0], &st_Matrix, pst_Pos);
			MATH_CopyVector(M, v);

			if((pst_NextEvt->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
			{
				pst_PosNext = EVE_pst_Event_InterpolationKey_GetPos(pst_NextEvt);
				MATH_TransformVertex(&v[3], &st_Matrix, pst_PosNext);

				nbframes = (int) (EVE_FrameToTime(pst_Evt->uw_NumFrames) * H->f_NbInterPerSecond);
				dt = 1.0f / H->f_NbInterPerSecond;

				if((w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_0)
				{
					MATH_CopyVector(M + 1, v + 3);
					_pst_DD->ul_ColorConstant = ul_ColSeg;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pM);
				}
				else if((w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_1)
				{
					MATH_TransformVertex(&v[1], &st_Matrix, pst_Pos + 1);

					_pst_DD->ul_ColorConstant = 0xFF00FF00;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPoint, (ULONG) (v + 1));

					_pst_DD->ul_ColorConstant = 0xFF007F00;
					pV[0] = v + 1;
					pV[1] = v;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pV);
					pV[1] = v + 3;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pV);

					_pst_DD->ul_ColorConstant = ul_ColSeg;
					for(t = 0.01f; t < 1.001; t += 0.05f)
					{
						omt = 1 - t;
						MATH_ScaleVector(M + 1, v, omt * omt);
						MATH_AddScaleVector(M + 1, M + 1, v + 1, 2 * omt * t);
						MATH_AddScaleVector(M + 1, M + 1, v + 3, t * t);
						_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pM);
						MATH_CopyVector(M, M + 1);
					}
				}
				else if((w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_2)
				{
					MATH_TransformVertex(&v[1], &st_Matrix, pst_Pos + 1);
					MATH_TransformVertex(&v[2], &st_Matrix, pst_Pos + 2);

					_pst_DD->ul_ColorConstant = 0xFF00FF00;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPoint, (ULONG) (v + 1));
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPoint, (ULONG) (v + 2));
					_pst_DD->ul_ColorConstant = 0xFF007F00;
					pV[0] = v;
					pV[1] = v + 1;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pV);
					pV[0] = v + 2;
					pV[1] = v + 3;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pV);

					_pst_DD->ul_ColorConstant = ul_ColSeg;
					for(t = 0.01f; t < 1.001; t += 0.05f)
					{
						omt = 1 - t;
						MATH_ScaleVector(M + 1, v, omt * omt * omt);
						MATH_AddScaleVector(M + 1, M + 1, v + 1, 3 * omt * omt * t);
						MATH_AddScaleVector(M + 1, M + 1, v + 2, 3 * t * t * omt);
						MATH_AddScaleVector(M + 1, M + 1, v + 3, t * t * t);
						_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pM);
						MATH_CopyVector(M, M + 1);
					}
				}

				/* inter */
				_pst_DD->ul_ColorConstant = ul_ColInter;
				for(t = dt, j = 0; j < nbframes; j++, t += dt)
				{
					EVE_Event_InterpolationKey_BlendTranslation(pst_Evt, pst_NextEvt, t, M);
					MATH_TransformVertex(M, &st_Matrix, M);
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPoint, (ULONG) M);
				}

				/* time key */
				_pst_DD->ul_ColorConstant = 0xFFFFFF00;
				if(w_Type & EVE_InterKeyType_Time)
				{
					pf_TimeKey = EVE_pf_Event_InterpolationKey_GetTime(pst_Evt);
					for(j = 1; j < *(int *) pf_TimeKey; j++)
					{
						EVE_Event_InterpolationKey_BlendTranslation(pst_Evt, pst_NextEvt, pf_TimeKey[j * 2], M);
						MATH_TransformVertex(M, &st_Matrix, M);
						_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPoint, (ULONG) M);
					}
				}

				_pst_DD->ul_ColorConstant = 0xFF0000FF;
				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPoint, (ULONG) (v + 3));
			}

			_pst_DD->ul_ColorConstant = 0xFF0000FF;
			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPoint, (ULONG) v);

			if(i == pst_Curve->i_TransEvt)
			{
				_pst_DD->ul_ColorConstant = 0xFF00FFFF;
				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPoint, (ULONG) (v + pst_Curve->i_TransPos));
			}
		}

		/* last key is selected ? */
		if(i == pst_Curve->i_TransEvt)
		{
			_pst_DD->ul_ColorConstant = 0xFF00FFFF;
			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPoint, (ULONG) & v[3]);
		}
	}

	pst_TrackRot = (EVE_tdst_Track *) pst_Curve->p_Track[SOFT_Cl_TrackRot];
	if(pst_TrackRot)
	{
		totaltime = 0;
		for(i = 0, pst_Evt = pst_TrackRot->pst_AllEvents; i < pst_TrackRot->uw_NumEvents; i++, pst_Evt++)
		{
			if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey)
			{
				totaltime += EVE_FrameToTime(pst_Evt->uw_NumFrames);
				continue;
			}

			if(pst_TrackTrans) EVE_Event_InterpolationKey_GetTranslation(pst_TrackTrans, totaltime, v);
			totaltime += EVE_FrameToTime(pst_Evt->uw_NumFrames);

			EVE_Event_InterpolationKey_CopyMatrix(&st_Rot, pst_Evt, 0);

			/* pst_Rot = EVE_pst_Event_InterpolationKey_GetRotation(pst_Evt); */
			MATH_AddVector(v + 1, v, MATH_pst_GetZAxis(&st_Rot));

			MATH_TransformVertex(M, &st_Matrix, v);
			MATH_TransformVertex(M + 1, &st_Matrix, v + 1);

			_pst_DD->ul_ColorConstant = 0xFF7F7F7F;
			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pM);
			_pst_DD->ul_ColorConstant = (i == pst_Curve->i_RotEvt) ? 0xFF00FFFF : 0xFFBFBFBF;
			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPoint, (ULONG) (M + 1));
		}
	}

	_pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_CurveRenderBone(OBJ_tdst_GameObject *_pst_GO, struct GDI_tdst_DisplayData_ *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern BOOL			SOFT_gb_NoDepthTest;
	extern LONG			GRO_PushSpecialMatrixForProportionnalBone(OBJ_tdst_GameObject *);
	extern LONG			GRO_UnknowPushSpecialMatrix(void *);
	OBJ_tdst_GameObject *pst_Father;
	GRO_tdst_Visu		*pst_Visu;
	GRO_tdst_Struct		*pst_Obj;
	ULONG				ul_SaveDrawMask;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

 	if(!ANI_b_GizmoHasTrack(_pst_GO)) return;

	pst_Father = _pst_GO;

	while
	(
		pst_Father
	&&	pst_Father->pst_Base
	&&	pst_Father->pst_Base->pst_Hierarchy
	&&	pst_Father->pst_Base->pst_Hierarchy->pst_Father
	) pst_Father = pst_Father->pst_Base->pst_Hierarchy->pst_Father;
	if(pst_Father && pst_Father->ul_EditorFlags & OBJ_C_EditFlags_Hidden) return;

	/* Switch off Z Test */
	ul_SaveDrawMask = _pst_DD->ul_DrawMask;
	_pst_DD->ul_DrawMask &=  ~GDI_Cul_DM_ZTest;
	_pst_DD->ul_DrawMask &=  ~GDI_Cul_DM_DontForceColor;
	SOFT_gb_NoDepthTest = TRUE;

	if(SEL_RetrieveItem(_pst_DD->pst_World->pst_Selection, _pst_GO))
		_pst_DD->ul_ColorConstant = 0xFF00AA00;
	else
		_pst_DD->ul_ColorConstant = 0xFFAA00AA;

	pst_Visu = _pst_GO->pst_Base->pst_Visu;
	pst_Obj = pst_Visu->pst_Object;
#ifdef JADEFUSION
	pst_Obj->i->pfnl_PushSpecialMatrix = (LONG (__cdecl *)(void *))GRO_PushSpecialMatrixForProportionnalBone;
#else
	pst_Obj->i->pfnl_PushSpecialMatrix = GRO_PushSpecialMatrixForProportionnalBone;
#endif
	GRO_Render(_pst_GO);

	pst_Obj->i->pfnl_PushSpecialMatrix = GRO_UnknowPushSpecialMatrix;

	SOFT_gb_NoDepthTest = FALSE;
	_pst_DD->ul_DrawMask = ul_SaveDrawMask;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_CurveRenderSkel(OBJ_tdst_GameObject *_pst_GO, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim		*pst_GOAnim;
	TAB_tdst_PFelem				*pst_Bone;
	OBJ_tdst_GameObject			*pst_BoneGO, *pst_GizmoBone;
	OBJ_tdst_GameObject			*pst_Father, *pst_GizmoFather;
	MATH_tdst_Matrix			st_Matrix;
	MATH_tdst_Vector			M[2], *pM[2];
	int							i_Gizmo;
	ULONG						ul_SaveColor;
	SOFT_tdst_HelperSkeleton	*pst_Skel;
	EVE_tdst_Track				*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_SaveColor = _pst_DD->ul_ColorConstant;

	if(!_pst_GO->pst_Base) return;
	if(!_pst_GO->pst_Base->pst_GameObjectAnim) return;
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	if(!pst_GOAnim->pst_Skeleton) return;
	if(!pst_GOAnim->apst_Anim[0]) return;
	if(!pst_GOAnim->apst_Anim[0]->pst_Data) return;
	if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden) return;

	pM[0] = &M[0];
	pM[1] = &M[1];

	pst_Skel = SOFT_pst_SkeletonAdd
		(
			_pst_DD->pst_Helpers,
			_pst_GO,
			pst_GOAnim->pst_Skeleton->pst_AllObjects->ul_NbElems
		);

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 0);

	MATH_SetIdentityMatrix(&st_Matrix);
	SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_Matrix);
	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_Camera.pst_ObjectToCameraMatrix);

	for(i_Gizmo = 0; i_Gizmo < (int) pst_GOAnim->pst_Skeleton->pst_AllObjects->ul_NbElems; i_Gizmo++)
	{
		pst_Bone = pst_GOAnim->pst_Skeleton->pst_AllObjects->p_Table + i_Gizmo;
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_Bone->p_Pointer;

		pst_Father = OBJ_pst_GetFatherInit(pst_BoneGO);
		if(!pst_Father) continue;
		if(pst_Father == _pst_GO) continue;

		if(SEL_RetrieveItem(_pst_DD->pst_World->pst_Selection, WOR_pst_GetGizmoFromBone(pst_Father)))
			_pst_DD->ul_ColorConstant = 0xFF0000AA;
		else if(pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListParam[i_Gizmo * 2].f_Time < 10e-7)
			_pst_DD->ul_ColorConstant = 0xFFAA0000;
		else
			_pst_DD->ul_ColorConstant = 0xFF00AAAA;

		MATH_CopyVector(&M[0], &OBJ_pst_GetAbsoluteMatrix(pst_Father)->T);
		MATH_CopyVector(&M[1], &OBJ_pst_GetAbsoluteMatrix(pst_BoneGO)->T);

		pst_GizmoBone = WOR_pst_GetGizmoFromBone(pst_BoneGO);
		pst_GizmoFather = WOR_pst_GetGizmoFromBone(pst_Father);

		if
		(
			!pst_GizmoBone
		||	!pst_GizmoFather
		||	(pst_GizmoBone->ul_EditorFlags & OBJ_C_EditFlags_Hidden)
		||	(pst_GizmoFather->ul_EditorFlags & OBJ_C_EditFlags_Hidden)
		) continue;

		if(pst_Skel)
		{
			pst_Track = EVE_pst_GizmoToTrack(pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks, pst_GizmoFather);
			SOFT_SkeletonAddBone(_pst_DD->pst_Helpers, pst_Skel, pst_Track->uw_Gizmo, M, M + 1);
		}

		/* If both Current and Father GO have tracks, we draw a line between them. */
		if(ANI_b_GizmoHasTrack(pst_BoneGO) && ANI_b_GizmoHasTrack(pst_Father))
			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pM);
	}

	SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 1);
	_pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_CurveRender2(SOFT_tdst_Helpers *H, SOFT_tdst_Helper *_pst_Helper, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_HelperCurve	*pst_Curve;
	OBJ_tdst_GameObject		*pst_GO;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	EVE_tdst_Data			*pst_Events;
	EVE_tdst_ListTracks		*pst_Tracks;
	EVE_tdst_Track			*pst_Track;
	EVE_tdst_Params			*pst_Param;
	EVE_tdst_Event			*pst_Evt, *pst_Evt1;
	MATH_tdst_Matrix		*Gizmo, st_Matrix, st_Temp;
	MATH_tdst_Vector		M[2], *pM[2], *pst_Pos, *pst_PrevPos;
	float					f_Start, f_End, t, dt, f_Delay, f_DelaySum;
	ULONG					ul_SaveColor;
	int						i_Sel1, i_Sel2, i_Gizmo, i_Track, i_SelTrack, i_Evt, i_NextEvt, i_SelEvt, i_SelPos, i, nbframes;
	short					w_Type;
	ULONG					ul_Father, ul_Value;
	BOOL					bFirst;
	float					fBefore, fAfter;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* init */
	fBefore = fAfter = 0.0f;
	pst_Curve = (SOFT_tdst_HelperCurve *) _pst_Helper->p_Data;
	if(!(pst_Curve->ul_Flags & SOFT_Cl_CurveUsed)) return;

	L_memset(SOFT_sgac_TrackTreated, 0, 64);
	L_memset(SOFT_sgac_TrackStatus, 0, 64);
	L_memset(SOFT_sgac_GizmoStatus, 0, 64);
	SOFT_sgc_MBStatus = 0;

	ul_SaveColor = _pst_DD->ul_ColorConstant;
	pM[0] = &M[0];
	pM[1] = &M[1];

	pst_Events = (EVE_tdst_Data *) pst_Curve->p_EventData;
	pst_Tracks = pst_Events->pst_ListTracks;
	pst_Track = pst_Tracks->pst_AllTracks;
	pst_GO = (OBJ_tdst_GameObject *) pst_Curve->p_CurGO;
	pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;

	if(H->ul_CurveFlag & SOFT_Cl_UseGizmoFlag)
	{
		ul_Value = H->ul_GizmoDisplayFlag[0];
		for(i = 0; i < 32; i++) SOFT_sgac_GizmoStatus[i] = (char) ((ul_Value >> i) & 1) * 3;
		ul_Value = H->ul_GizmoDisplayFlag[1];
		for(i = 0; i < 32; i++) SOFT_sgac_GizmoStatus[i + 32] = (char) ((ul_Value >> i) & 1) * 3;
	}

	/* get time length and update status */
	f_Delay = t = 0;
	for(i_Track = 0; i_Track < (int) pst_Tracks->uw_NumTracks; i_Track++, pst_Track++)
	{
		pst_Evt = pst_Track->pst_AllEvents;
		if(!pst_Evt) continue;
		if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) continue;

		w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Evt);
		SOFT_sgac_TrackTreated[i_Track] = w_Type & (EVE_InterKeyType_Translation_Mask | EVE_InterKeyType_Rotation_Mask);
		if(!SOFT_sgac_TrackTreated[i_Track]) continue;

		if(pst_Track->uw_Gizmo == (unsigned short) - 1)
		{
			if(t == 0)
			{
				pst_Evt = pst_Track->pst_AllEvents;
				for(i_Evt = 0; i_Evt < pst_Events->pst_ListParam[i_Track].uw_CurrentEvent; i_Evt++, pst_Evt++)
					t += EVE_FrameToTime(pst_Evt->uw_NumFrames);
				t += pst_Events->pst_ListParam[i_Track].f_Time;
				fBefore = t;

				pst_Evt1 = pst_Evt;
				fAfter = EVE_FrameToTime(pst_Evt->uw_NumFrames) - pst_Events->pst_ListParam[i_Track].f_Time;
				i_Evt++;
				for(; i_Evt < pst_Track->uw_NumEvents; i_Evt++, pst_Evt1++)
					fAfter += EVE_FrameToTime(pst_Evt1->uw_NumFrames);
			}

			if(pst_Track->uw_Flags & EVE_C_Track_Curve)
			{
				SOFT_sgc_MBStatus = 3;
				SOFT_sgac_TrackStatus[i_Track] = 3;
			}

			continue;
		}

		if(pst_Track->uw_Flags & EVE_C_Track_Curve)
		{
			SOFT_sgac_GizmoStatus[pst_Track->uw_Gizmo] = 3;
			SOFT_sgac_TrackStatus[i_Track] = 3;
		}

		for(f_DelaySum = 0, i_Evt = 0; i_Evt < pst_Track->uw_NumEvents; i_Evt++, pst_Evt++)
			f_DelaySum += EVE_FrameToTime(pst_Evt->uw_NumFrames);
		if(f_DelaySum > f_Delay) f_Delay = f_DelaySum;
	}

	/*
	 * second pass on track to update status of unselect curve that has a
	 * correspondant curve selected
	 */
	pst_Track = pst_Tracks->pst_AllTracks;
	for(i_Track = 0; i_Track < (int) pst_Tracks->uw_NumTracks; i_Track++, pst_Track++)
	{
		if(!SOFT_sgac_TrackTreated[i_Track]) continue;

		if(pst_Track->uw_Gizmo == (unsigned short) - 1)
		{
			if(SOFT_sgc_MBStatus) SOFT_sgac_TrackStatus[i_Track] |= 1;
		}
		else
		{
			if(SOFT_sgac_GizmoStatus[pst_Track->uw_Gizmo]) SOFT_sgac_TrackStatus[i_Track] |= 1;
		}
	}

	EVE_Event_InterpolationKey_GetAbsoluteMatrix(pst_Events, NULL, pst_GO, 0, -1, &st_Temp);
	MATH_MulMatrixMatrix(&st_Matrix, &st_Temp, &_pst_DD->st_Camera.st_InverseMatrix);

	dt = 1.0f / H->f_NbInterPerSecond;
	t = MATH_f_FloatModulo(t, dt);
	nbframes = H->l_NbFramesAfter + H->l_NbFramesBefore;
	if(nbframes <= 0)
		nbframes = (int) (f_Delay * H->f_NbInterPerSecond);
	else
		t -= dt * H->l_NbFramesBefore;
	f_Start = t;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    draw curve and interpolation
	 -------------------------------------------------------------------------------------------------------------------
	 */

	bFirst = TRUE;
	for(i = 0; i <= nbframes; i++, t += dt)
	{
		/* Avoid cycling... */
		if(i < H->l_NbFramesBefore)
		{
			if(H->l_NbFramesBefore - i > (int) (fBefore * 60.5f)) continue;
		}
		else if((i - H->l_NbFramesBefore) + 1 >= (int) (fAfter * 60.5f))
		{
			break;
		}

		EVE_Event_InterpolationKey_SpecialPlay(pst_GO, pst_Events, t, -1, 0, &EVE_st_MagicBox, EVE_ast_Gizmo);
		EVE_Event_InterpolationKey_UpdateGizmos(pst_GO, pst_GOAnim->pst_Skeleton, &EVE_st_MagicBox, EVE_ast_Gizmo);

		for(i_Gizmo = -1; i_Gizmo < (int) pst_GOAnim->pst_Skeleton->pst_AllObjects->ul_NbElems; i_Gizmo++)
		{
			if(i_Gizmo == -1)
			{
				if(!SOFT_sgc_MBStatus) continue;
				Gizmo = &EVE_st_MagicBox;
				pst_PrevPos = &SOFT_sgst_MBPreviousPos;
			}
			else
			{
				if(!SOFT_sgac_GizmoStatus[i_Gizmo]) continue;
				Gizmo = EVE_ast_Gizmo + i_Gizmo;
				pst_PrevPos = SOFT_sgast_PreviousPos + i_Gizmo;
			}

			_pst_DD->ul_ColorConstant = 0;
			MATH_TransformVertex(M, &st_Matrix, MATH_pst_GetTranslation(Gizmo));
			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointMin, (ULONG) M);

			if(!bFirst)
			{
				_pst_DD->ul_ColorConstant = 0xFFFFFFFF;
				MATH_TransformVertex(M + 1, &st_Matrix, pst_PrevPos);
				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pM);
			}

			if(i == H->l_NbFramesBefore)
				_pst_DD->ul_ColorConstant = 0xFFFF0000;
			else
				_pst_DD->ul_ColorConstant = 0xFF00FFFF;
			MATH_TransformVertex(M, &st_Matrix, MATH_pst_GetTranslation(Gizmo));
			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointMin, (ULONG) M);

			MATH_CopyVector(pst_PrevPos, MATH_pst_GetTranslation(Gizmo));

			/* Draw bones */
			if(SOFT_gac_BoneCurveStatus[i_Gizmo])
			{
				if(i == H->l_NbFramesBefore)
					_pst_DD->ul_ColorConstant = 0xFF0000FF;
				else
					_pst_DD->ul_ColorConstant = 0xFF4F4F4F;
				MATH_AddScaleVector
				(
					M + 1,
					MATH_pst_GetTranslation(Gizmo),
					MATH_pst_GetZAxis(Gizmo),
					pst_Curve->f_BoneLength[i_Gizmo]
				);
				MATH_TransformVertex(M + 1, &st_Matrix, M + 1);
				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pM);
			}
		}

		bFirst = FALSE;
	}

	f_End = t;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Draw key
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(pst_Curve->l_PickCur != -1)
	{
		i_SelTrack = pst_Curve->dst_Pick[pst_Curve->l_PickCur].i_Track;
		i_SelEvt = pst_Curve->dst_Pick[pst_Curve->l_PickCur].i_Evt;
		i_SelPos = pst_Curve->dst_Pick[pst_Curve->l_PickCur].i_Translation;
		t = pst_Curve->dst_Pick[pst_Curve->l_PickCur].f_Time;
	}

	SOFT_Helper_CurvePickReset(pst_Curve);
	pst_Track = pst_Tracks->pst_AllTracks;
	for(i_Track = 0; i_Track < (int) pst_Tracks->uw_NumTracks; i_Track++, pst_Track++)
	{
		if(!(SOFT_sgac_TrackStatus[i_Track] & 2)) continue;

		pst_Param = pst_Events->pst_ListParam + i_Track;
		i_Sel1 = (i_Track == i_SelTrack);
		f_DelaySum = 0;
		i_Evt = 0;

		while(f_DelaySum < f_End)
		{
			pst_Evt = pst_Track->pst_AllEvents + i_Evt;

			if(f_DelaySum < f_Start) goto curverender2_continue;

			EVE_Event_InterpolationKey_SpecialPlay
			(
				pst_GO,
				pst_Events,
				f_DelaySum,
				i_Track,
				i_Evt,
				&EVE_st_MagicBox,
				EVE_ast_Gizmo
			);
			EVE_Event_InterpolationKey_UpdateGizmos(pst_GO, pst_GOAnim->pst_Skeleton, &EVE_st_MagicBox, EVE_ast_Gizmo);
			Gizmo = ((short) pst_Track->uw_Gizmo == -1) ? &EVE_st_MagicBox : EVE_ast_Gizmo + pst_Track->uw_Gizmo;
			i_Sel2 = i_Sel1 && (i_SelEvt == i_Evt) && (t == f_DelaySum);

			MATH_TransformVertex(M, &st_Matrix, MATH_pst_GetTranslation(Gizmo));

			/*$2------------------------------------------------------------------------------------------------------*/

			if(SOFT_sgac_TrackTreated[i_Track] & EVE_InterKeyType_Translation_Mask)
			{
				if(i_Evt == pst_Param->uw_CurrentEvent)
				{
					_pst_DD->ul_ColorConstant = ((i_Sel2) && (i_SelPos == 1)) ? 0xFFFF0040 : 0xFF0000FF;
				}
				else
				{
					_pst_DD->ul_ColorConstant = ((i_Sel2) && (i_SelPos == 1)) ? 0xFFFF0040 : 0xFF4040FF;
				}

				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointMin, (ULONG) M);
				SOFT_Helper_CurvePickAdd(pst_Curve, M, i_Track, i_Evt, f_DelaySum, ((i_Sel2) && (i_SelPos == 1)), 1);

				w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Evt) & EVE_InterKeyType_Translation_Mask;

				if(w_Type > 1)
				{
					ul_Father = ANI_ul_GetFatherIndex(pst_GOAnim->pst_Skeleton, pst_Track->uw_Gizmo);
					pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(pst_Evt) + 1;
					MATH_TransformVertex(M + 1, EVE_ast_Gizmo + ul_Father, pst_Pos);
					MATH_TransformVertex(M + 1, &st_Matrix, M + 1);
					_pst_DD->ul_ColorConstant = 0xFF00FF00;
					_pst_DD->ul_ColorConstant = ((i_Sel2) && (i_SelPos == 2)) ? 0xFF00FFFF : 0xFF00FF00;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointMin, (ULONG) (M + 1));
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pM);
					SOFT_Helper_CurvePickAdd
					(
						pst_Curve,
						M + 1,
						i_Track,
						i_Evt,
						f_DelaySum,
						((i_Sel2) && (i_SelPos == 2)),
						2
					);

					if(w_Type > 2)
					{
						MATH_TransformVertex(M + 1, EVE_ast_Gizmo + ul_Father, pst_Pos + 1);
						MATH_TransformVertex(M + 1, &st_Matrix, M + 1);
						_pst_DD->ul_ColorConstant = ((i_Sel2) && (i_SelPos == 3)) ? 0xFF0000FF : 0xFF00FF00;
						_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointMin, (ULONG) (M + 1));
						SOFT_Helper_CurvePickAdd
						(
							pst_Curve,
							M + 1,
							i_Track,
							i_Evt,
							f_DelaySum,
							((i_Sel2) && (i_SelPos == 3)),
							3
						);
					}

					i_NextEvt = (i_Evt + 1 == pst_Track->uw_NumEvents) ? 0 : i_Evt + 1;
					EVE_Event_InterpolationKey_SpecialPlay
					(
						pst_GO,
						pst_Events,
						f_DelaySum + EVE_FrameToTime(pst_Evt->uw_NumFrames),
						i_Track,
						i_NextEvt,
						&EVE_st_MagicBox,
						EVE_ast_Gizmo
					);
					EVE_Event_InterpolationKey_UpdateGizmos
					(
						pst_GO,
						pst_GOAnim->pst_Skeleton,
						&EVE_st_MagicBox,
						EVE_ast_Gizmo
					);
					MATH_TransformVertex(M, &st_Matrix, MATH_pst_GetTranslation(Gizmo));
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) pM);
				}
			}

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			else
			{
				if(!i_Sel2)
				{
					if(i_Evt == pst_Param->uw_CurrentEvent)
						_pst_DD->ul_ColorConstant = 0xFFFF0000;
					else
						_pst_DD->ul_ColorConstant = 0xFF0000FF;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointMin, (ULONG) M);
				}
			}

curverender2_continue:
			f_DelaySum += EVE_FrameToTime(pst_Evt->uw_NumFrames);
			i_Evt++;
			if(i_Evt == pst_Track->uw_NumEvents)
			{
				if(f_DelaySum == 0) break;
				i_Evt = 0;
			}
		}
	}

	_pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helpers_CurvePick
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	char				_c_Start
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		C, *pst_Pos, st_Pos;
	SOFT_tdst_HelperCurve	*pst_Curve;
	int						i, j;
	OBJ_tdst_GameObject		*pst_GO;
	MATH_tdst_Matrix		st_Matrix, st_Temp, st_Rot;
	EVE_tdst_ListTracks		*pst_Tracks;
	EVE_tdst_Track			*pst_Track, *pst_TrackRot;
	EVE_tdst_Event			*pst_Evt;
	short					w_Type;
	float					time;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Curve = (SOFT_tdst_HelperCurve *) _pst_Helper->p_Data;
	if(!(pst_Curve->ul_Flags & SOFT_Cl_CurveUsed)) return 0;
	if(pst_Curve->ul_Flags & SOFT_Cl_AnimCurve)
		return SOFT_l_Helpers_CurvePick2(_pst_Helpers, _pst_Helper, A, B, _c_Start);

	pst_GO = (OBJ_tdst_GameObject *) pst_Curve->p_CurGO;
	pst_Track = (EVE_tdst_Track *) pst_Curve->p_Track[SOFT_Cl_TrackTrans];
	pst_TrackRot = (EVE_tdst_Track *) pst_Curve->p_Track[SOFT_Cl_TrackRot];
	pst_Tracks = ((EVE_tdst_Data *) pst_Curve->p_EventData)->pst_ListTracks;

	EVE_Event_InterpolationKey_GetPathInitMatrix
	(
		pst_Track,
		pst_GO,
		&st_Temp
	);
	MATH_MulMatrixMatrix(&st_Matrix, &st_Temp, &_pst_Helpers->pst_DD->st_Camera.st_InverseMatrix);

	/* check translation key */
	if(pst_Track)
	{
		for(i = 0, pst_Evt = pst_Track->pst_AllEvents; i < pst_Track->uw_NumEvents; i++, pst_Evt++)
		{
			if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) continue;

			pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(pst_Evt);
			w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Evt);

			for(j = 0; j < (w_Type & EVE_InterKeyType_Translation_Mask); j++)
			{
				MATH_TransformVertex(&C, &st_Matrix, pst_Pos + j);
				if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
					MATH_ScaleEqualVector(&C, 1 / C.z);
				else
					C.z = 1;
				if(MATH_f_Distance(&C, B) < 0.01f)
				{
					pst_Curve->i_RotEvt = -1;
					pst_Curve->i_TransEvt = i;
					pst_Curve->i_TransPos = j;
					return 1;
				}
			}
		}
	}

	/* check rotation key */
	if(pst_TrackRot)
	{
		MATH_InitVectorToZero(&st_Pos);
		for(time = 0, i = 0, pst_Evt = pst_TrackRot->pst_AllEvents; i < pst_TrackRot->uw_NumEvents; i++, pst_Evt++)
		{
			if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) continue;

			if(pst_Track) EVE_Event_InterpolationKey_GetTranslation(pst_Track, time, &st_Pos);
			time += EVE_FrameToTime(pst_Evt->uw_NumFrames);

			/* pst_Rot = EVE_pst_Event_InterpolationKey_GetRotation(pst_Evt); */
			EVE_Event_InterpolationKey_CopyMatrix(&st_Temp, pst_Evt, 0);
			MATH_AddEqualVector(&st_Pos, MATH_pst_GetZAxis(&st_Rot));

			MATH_TransformVertex(&C, &st_Matrix, &st_Pos);
			if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
				MATH_ScaleEqualVector(&C, 1 / C.z);
			else
				C.z = 1;
			if(MATH_f_Distance(&C, B) < 0.01f)
			{
				pst_Curve->i_RotEvt = i;
				pst_Curve->i_TransEvt = -1;
				pst_Curve->i_TransPos = -1;
				return 1;
			}
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_Helpers_CurvePick2
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	char				_c_Start
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		C;
	SOFT_tdst_HelperCurve	*pst_Curve;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Curve = (SOFT_tdst_HelperCurve *) _pst_Helper->p_Data;
	if(!(pst_Curve->ul_Flags & SOFT_Cl_CurveUsed)) return 0;

	for(i = 0; i < pst_Curve->l_PickNumber; i++)
	{
		MATH_CopyVector(&C, &pst_Curve->dst_Pick[i].st_ViewPos);
		if(_pst_Helpers->pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
			MATH_ScaleEqualVector(&C, 1 / C.z);
		else
			C.z = 1;
		if(MATH_f_Distance(&C, B) < 0.01f)
		{
			pst_Curve->l_PickCur = i;
			return 1;
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_CurveGetMove
(
	SOFT_tdst_Helpers	*_pst_Helpers,
	SOFT_tdst_Helper	*_pst_Helper,
	MATH_tdst_Vector	*_pst_Move
)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SOFT_tdst_HelperCurve *SOFT_pst_Helper_GetCurve(SOFT_tdst_Helpers *H, EVE_tdst_Track *_pst_Track)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < 16; i++)
	{
		if(!(H->ast_Curve[i].ul_Flags & SOFT_Cl_CurveUsed)) continue;

		if(H->ast_Curve[i].p_Track[0] == _pst_Track) return &H->ast_Curve[i];
		if(H->ast_Curve[i].p_Track[1] == _pst_Track) return &H->ast_Curve[i];
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SOFT_tdst_HelperCurve *SOFT_pst_Helper_GetCurveByData(SOFT_tdst_Helpers *H, EVE_tdst_Data *_pst_Data)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < 16; i++)
	{
		if(!(H->ast_Curve[i].ul_Flags & SOFT_Cl_CurveUsed)) continue;
		if(H->ast_Curve[i].p_EventData == _pst_Data) return &H->ast_Curve[i];
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SOFT_tdst_HelperCurve *SOFT_pst_Helper_GetUnusedCurve(SOFT_tdst_Helpers *H)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < 16; i++)
	{
		if(H->ast_Curve[i].ul_Flags & SOFT_Cl_CurveUsed) continue;
		return &H->ast_Curve[i];
	}

	return NULL;
}

/* #define SOFT_HELPER_CURVE_DUMP */
#ifdef SOFT_HELPER_CURVE_DUMP

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helper_CurveDump(SOFT_tdst_Helpers *H)
{
	/*~~~~~~~~~~~~~~~~~*/
	int		i, l;
	char	sz_Text[256];
	/*~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg("<-- Curve data -->");
	for(l = 0, i = 0; i < 16; i++)
	{
		if(!(H->ast_Curve[i].ul_Flags & SOFT_Cl_CurveUsed)) continue;
		l++;
		sprintf
		(
			sz_Text,
			"%02d - %08X - %s",
			i,
			H->ast_Curve[i].ul_Flags,
			((OBJ_tdst_GameObject *) H->ast_Curve[i].p_GO)->sz_Name
		);
		LINK_PrintStatusMsg(sz_Text);
	}

	sprintf(sz_Text, "<-- %d curves data -->");
	LINK_PrintStatusMsg(sz_Text);
}

#else
#define SOFT_Helper_CurveDump(a)
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helper_AddCurve
(
	SOFT_tdst_Helpers	*H,
	OBJ_tdst_GameObject *_pst_GO,
	EVE_tdst_Data		*_pst_Events,
	EVE_tdst_Track		*_pst_Track
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event			*pst_Evt;
	short					w_Type;
	SOFT_tdst_HelperCurve	*pst_Curve;
	int						i;
	EVE_tdst_ListTracks		*pst_LT;
	EVE_tdst_Track			*pst_Track;
	OBJ_tdst_GameObject		/**pst_GO, */*pst_BoneGO;
	OBJ_tdst_GameObject		*pst_Father;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	OBJ_tdst_Group			*pst_Group;
	MATH_tdst_Vector		v;
#if 0
	MATH_tdst_Vector		*pst_Max, *pst_Min;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = _pst_Track->pst_AllEvents;
	if(!pst_Evt) return;

	i = 0;

	/* Loop while we only find Empty Events. */
	while(((pst_Evt->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_Empty) && (i + 1 < _pst_Track->uw_NumEvents))
		pst_Evt++;
	if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) return;
	w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Evt);
	if(w_Type == -1) return;

    if( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	{
		pst_Curve = SOFT_pst_Helper_GetCurveByData(H, _pst_Events);
		if(pst_Curve) return;
	}
	else
		pst_Curve = SOFT_pst_Helper_GetCurve(H, _pst_Track);

	if(!pst_Curve)
	{
		pst_Curve = SOFT_pst_Helper_GetUnusedCurve(H);
		if(!pst_Curve) return;
	}

	pst_Curve->ul_Flags |= SOFT_Cl_CurveUsed;
    pst_Curve->p_OwnerGO = _pst_GO;
    pst_Curve->p_CurGO = (_pst_Track->pst_GO) ? _pst_Track->pst_GO : _pst_GO;
	pst_Curve->p_EventData = _pst_Events;
	pst_LT = _pst_Events->pst_ListTracks;
	pst_Curve->p_Track[0] = pst_Curve->p_Track[1] = pst_Curve->p_Track[2] = pst_Curve->p_Track[3] = NULL;

	//if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
    if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	{
		pst_Curve->ul_Flags |= SOFT_Cl_AnimCurve;
		//pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;
        pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
		pst_Group = pst_GOAnim->pst_Skeleton;
		for(i = 0; i < (int) pst_Group->pst_AllObjects->ul_NbElems; i++)
		{
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_Group->pst_AllObjects->p_Table[i].p_Pointer;

			pst_Father = OBJ_pst_GetFatherInit(pst_BoneGO);
			if(!pst_Father) continue;
			if(pst_Father == _pst_GO) continue;

			MATH_CopyVector(&v, OBJ_pst_GetAbsolutePosition(pst_BoneGO));
			MATH_SubVector(&v, &v, OBJ_pst_GetAbsolutePosition(pst_Father));
			pst_Curve->f_BoneLength[i - 1] = MATH_f_NormVector(&v);
#if 0
			if(pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_OBBox)
			{
				pst_Max = OBJ_pst_BV_GetLMax(pst_BoneGO->pst_BV);
				pst_Min = OBJ_pst_BV_GetLMin(pst_BoneGO->pst_BV);
				pst_Curve->f_BoneLength[i] = pst_Max->z - pst_Min->z;
			}
			else
			{
				pst_Max = OBJ_pst_BV_GetGMax(pst_BoneGO->pst_BV);
				pst_Min = OBJ_pst_BV_GetGMin(pst_BoneGO->pst_BV);
				pst_Curve->f_BoneLength[i] = fMax3
					(
						pst_Max->x - pst_Min->x,
						pst_Max->y - pst_Min->y,
						pst_Max->z - pst_Min->z
					);
			}

#endif
		}
	}
	else
	{
		if(w_Type & EVE_InterKeyType_Translation_Mask)
			pst_Curve->p_Track[SOFT_Cl_TrackTrans] = _pst_Track;
		else if(w_Type & EVE_InterKeyType_Rotation_Matrix) pst_Curve->p_Track[SOFT_Cl_TrackRot] = _pst_Track;

		for(i = 0, pst_Track = pst_LT->pst_AllTracks; i < pst_LT->uw_NumTracks; i++, pst_Track++)
		{
			if(pst_Track == _pst_Track) continue;
			if(!pst_Track->uw_NumEvents) continue;
			pst_Evt = pst_Track->pst_AllEvents;
			if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) continue;

			if(pst_Track->pst_GO != _pst_Track->pst_GO) continue;
			if(pst_Track->uw_Gizmo != _pst_Track->uw_Gizmo) continue;
			w_Type = (int) EVE_w_Event_InterpolationKey_GetType(pst_Evt);

			if(w_Type & EVE_InterKeyType_Translation_Mask)
				pst_Curve->p_Track[SOFT_Cl_TrackTrans] = pst_Track;
			else if(w_Type & EVE_InterKeyType_Rotation_Matrix) pst_Curve->p_Track[SOFT_Cl_TrackRot] = pst_Track;
		}
	}

	SOFT_Helper_CurveDump(H);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helper_DelCurve(SOFT_tdst_Helpers *H, EVE_tdst_Data *_pst_Data, EVE_tdst_Track *_pst_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_HelperCurve	*pst_Curve;
	EVE_tdst_Track			*pst_Track;
	EVE_tdst_ListTracks		*pst_LT;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Curve = SOFT_pst_Helper_GetCurve(H, _pst_Track);
	if(!pst_Curve)
	{
		pst_Curve = SOFT_pst_Helper_GetCurveByData(H, _pst_Data);
		if(!pst_Curve) return;
	}

	if(!(pst_Curve->ul_Flags & SOFT_Cl_CurveUsed)) return;

	if(pst_Curve->ul_Flags & SOFT_Cl_AnimCurve)
	{
		pst_LT = _pst_Data->pst_ListTracks;
		for(i = 0, pst_Track = pst_LT->pst_AllTracks; i < pst_LT->uw_NumTracks; i++, pst_Track++)
		{
			if(pst_Track->uw_Flags & EVE_C_Track_Curve) return;
		}
	}
	else
	{
		pst_Track = (EVE_tdst_Track *) pst_Curve->p_Track[0];
		if((pst_Track) && (pst_Track != _pst_Track) && (pst_Track->uw_Flags & EVE_C_Track_Selected)) return;
		pst_Track = (EVE_tdst_Track *) pst_Curve->p_Track[1];
		if((pst_Track) && (pst_Track != _pst_Track) && (pst_Track->uw_Flags & EVE_C_Track_Selected)) return;
	}

	if(pst_Curve->dst_Pick)
	{
		L_free(pst_Curve->dst_Pick);
		pst_Curve->dst_Pick = NULL;
		pst_Curve->l_PickMax = pst_Curve->l_PickNumber = 0;
		pst_Curve->l_PickCur = -1;
	}

	pst_Curve->ul_Flags = 0;

	SOFT_Helper_CurveDump(H);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helper_CurvePickReset(SOFT_tdst_HelperCurve *_pst_Curve)
{
	_pst_Curve->l_PickNumber = 0;
	_pst_Curve->l_PickCur = -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helper_CurvePickAdd
(
	SOFT_tdst_HelperCurve	*_pst_Curve,
	MATH_tdst_Vector		*_pst_Pos,
	int						_i_Track,
	int						_i_Evt,
	float					_f_Time,
	int						_i_Sel,
	int						_i_Translation
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_HelperCurvePick	*pst_Pick;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Curve->l_PickNumber == _pst_Curve->l_PickMax)
	{
		_pst_Curve->l_PickMax += 16;
		if(_pst_Curve->l_PickMax == 16)
			_pst_Curve->dst_Pick = (SOFT_tdst_HelperCurvePick*)L_malloc(_pst_Curve->l_PickMax * sizeof(SOFT_tdst_HelperCurvePick));
		else
		{
			_pst_Curve->dst_Pick = (SOFT_tdst_HelperCurvePick*)L_realloc
				(
					_pst_Curve->dst_Pick,
					_pst_Curve->l_PickMax * sizeof(SOFT_tdst_HelperCurvePick)
				);
		}
	}

	pst_Pick = _pst_Curve->dst_Pick + _pst_Curve->l_PickNumber;
	pst_Pick->f_Time = _f_Time;
	pst_Pick->i_Evt = _i_Evt;
	pst_Pick->i_Track = _i_Track;
	pst_Pick->i_Translation = _i_Translation;
	MATH_CopyVector(&pst_Pick->st_ViewPos, _pst_Pos);

	if(_i_Sel) _pst_Curve->l_PickCur = _pst_Curve->l_PickNumber;

	_pst_Curve->l_PickNumber++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_CurveReplaceData( SOFT_tdst_Helpers *H, OBJ_tdst_GameObject *_pst_Gao, EVE_tdst_Data *_pst_EvtOld, EVE_tdst_Data *_pst_EvtNew )
{
    int i, j;

    for (i = 0; i < 16; i++)
    {
        if( !(H->ast_Curve[i].ul_Flags & SOFT_Cl_CurveUsed) ) continue;
        if (H->ast_Curve[i].p_OwnerGO != _pst_Gao ) continue;
        if (H->ast_Curve[i].p_EventData != _pst_EvtOld ) continue;

        H->ast_Curve[i].p_EventData = _pst_EvtNew;

        for (j = 0; j < _pst_EvtOld->pst_ListTracks->uw_NumTracks; j++)
        {
            if ( H->ast_Curve[i].p_Track[0] == &_pst_EvtOld->pst_ListTracks->pst_AllTracks[ j ] )
                H->ast_Curve[i].p_Track[0] = &_pst_EvtNew->pst_ListTracks->pst_AllTracks[ j ];
            if ( H->ast_Curve[i].p_Track[1] == &_pst_EvtOld->pst_ListTracks->pst_AllTracks[ j ] )
                H->ast_Curve[i].p_Track[1] = &_pst_EvtNew->pst_ListTracks->pst_AllTracks[ j ];
        }
    }
}

/*$4
 ***********************************************************************************************************************
    Skeleton
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_SkeletonPick(SOFT_tdst_HelperSkeleton *_pst_Skel, int x, int y)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_HelperSkeletonBone	*pst_Bone;
	int								i;
	MATH_tdst_Vector				C, D, AB, AC;
	float							t;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_InitVector(&C, (float) x, (float) y, 0);

	for(pst_Bone = _pst_Skel->dst_Bone, i = 0; i < _pst_Skel->l_NbBones; i++, pst_Bone++)
	{
		if(pst_Bone->l_Gizmo == 0xcdcdcdcd) continue;

		MATH_SubVector(&AB, &pst_Bone->B, &pst_Bone->A);
		MATH_SubVector(&AC, &C, &pst_Bone->A);
		t = MATH_f_DotProduct(&AB, &AB);
		if(t < 0.0001f) continue;
		t = MATH_f_DotProduct(&AB, &AC) / t;
		if((t < 0) || (t > .8f)) continue;

		MATH_ScaleVector(&D, &AB, -t);
		MATH_AddEqualVector(&D, &AC);
		t = MATH_f_SqrVector(&D);
		if(t < 25)
		{
			_pst_Skel->l_GizmoPicked = pst_Bone->l_Gizmo;
			return 1;
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SOFT_tdst_HelperSkeleton *SOFT_pst_SkeletonAdd(SOFT_tdst_Helpers *H, OBJ_tdst_GameObject *_pst_GO, int _i_NbBones)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_HelperSkeleton	*pst_Skel;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(H->l_CurrentSkeleton == 4) return NULL;

	pst_Skel = H->ast_Skeleton + H->l_CurrentSkeleton++;
	pst_Skel->l_NbBones = _i_NbBones;
	pst_Skel->pst_GO = _pst_GO;
	if(_i_NbBones)
	{
		pst_Skel->dst_Bone = (SOFT_tdst_HelperSkeletonBone *) L_malloc(_i_NbBones * sizeof(SOFT_tdst_HelperSkeletonBone));
		for(i = 0; i < _i_NbBones; i++)
		{
			pst_Skel->dst_Bone->l_Gizmo = 0xcdcdcdcd;
		}
	}

	return pst_Skel;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_SkeletonAddBone
(
	SOFT_tdst_Helpers			*H,
	SOFT_tdst_HelperSkeleton	*_pst_Skel,
	int							_i_Gizmo,
	MATH_tdst_Vector			*A,
	MATH_tdst_Vector			*B
)
{
	/*~~~~~~~*/
	int i_Bone;
	/*~~~~~~~*/

	for(i_Bone = 0; i_Bone < _pst_Skel->l_NbBones; i_Bone++)
	{
		if(_pst_Skel->dst_Bone[i_Bone].l_Gizmo == 0xcdcdcdcd) break;
	}

	if(i_Bone == _pst_Skel->l_NbBones) return;

	_pst_Skel->dst_Bone[i_Bone].l_Gizmo = _i_Gizmo;

	SOFT_TransformAndProject(&_pst_Skel->dst_Bone[i_Bone].A, A, 1, &H->pst_DD->st_Camera);
	_pst_Skel->dst_Bone[i_Bone].A.z = 0;
	SOFT_TransformAndProject(&_pst_Skel->dst_Bone[i_Bone].B, B, 1, &H->pst_DD->st_Camera);
	_pst_Skel->dst_Bone[i_Bone].B.z = 0;
}

/*$4
 ***********************************************************************************************************************
    portal
 ***********************************************************************************************************************
 */
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_PortalPick(WOR_tdst_Portal *_pst_Portal, MATH_tdst_Vector *A, MATH_tdst_Vector *B, float *_pf_Dist )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	AB, AD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	MATH_SubVector( &AB, &_pst_Portal->vB, &_pst_Portal->vA );
	MATH_SubVector( &AD, &_pst_Portal->vD, &_pst_Portal->vA );
	
	if ( SOFT_l_IntersectQuadLine( A, B, &_pst_Portal->vA, &AB, &AD, _pf_Dist ) )
		return 1;
		
	return 0;
}
 

/*$4
 ***********************************************************************************************************************
    UV gizmo functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_UVGizmoOn( SOFT_tdst_Helpers *H, OBJ_tdst_GameObject *_pst_Obj, int _c_Type, float _f_Depth )
{
    MATH_tdst_Matrix    *M0, *M1;
    MATH_tdst_Vector    pos, w, v0;

    SOFT_UVGizmo_Set( &H->st_UVGizmo, _pst_Obj, _c_Type );

    if (_c_Type == SOFT_Cl_UVGizmo_Planar)
    {
        M0 = &H->st_UVGizmo.st_Matrix;
        M1 = &H->pst_DD->st_Camera.st_Matrix;

        MATH_CopyVector( MATH_pst_GetXAxis( M0 ), MATH_pst_GetXAxis( M1 ) );
        MATH_NegVector( MATH_pst_GetYAxis( M0 ), MATH_pst_GetZAxis( M1 ) );
        MATH_CopyVector( MATH_pst_GetZAxis( M0 ), MATH_pst_GetYAxis( M1 ) );

        MATH_InitVector( &w, H->pst_DD->st_Camera.f_Width * 0.5f, H->pst_DD->st_Camera.f_Height * 0.5f, _f_Depth );
        CAM_2Dto3DCamera( &H->pst_DD->st_Camera ,&pos, &w );
        MATH_TransformVertex( &M0->T, M1, &pos );

        MATH_InitVector( &w, H->pst_DD->st_Camera.f_Width * 0.95f, H->pst_DD->st_Camera.f_Height * 0.5f, _f_Depth );
        CAM_2Dto3DCamera( &H->pst_DD->st_Camera ,&v0, &w );
        H->st_UVGizmo.x = MATH_f_Distance( &pos, &v0 );

        MATH_InitVector( &w, H->pst_DD->st_Camera.f_Width * 0.5f, H->pst_DD->st_Camera.f_Height * 0.95f, _f_Depth );
        CAM_2Dto3DCamera( &H->pst_DD->st_Camera ,&v0, &w );
        H->st_UVGizmo.z = MATH_f_Distance( &pos, &v0 );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Helpers_UVGizmoOff( SOFT_tdst_Helpers *H )
{
    SOFT_UVGizmo_Reset( &H->st_UVGizmo );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SOFT_b_Helpers_UVGizmoIsOn( SOFT_tdst_Helpers *H )
{
    return ( H->st_UVGizmo.c_On == 1 );
}

/*$4
 ***********************************************************************************************************************
    Intersection functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_NearestPointOf2Lines
(
	MATH_tdst_Vector	*a,
	MATH_tdst_Vector	*b,
	MATH_tdst_Vector	*c,
	MATH_tdst_Vector	*d,
	float				*_pf_Distance,
	float				*t1,
	MATH_tdst_Vector	*_pst_Pt1,
	float				*t2,
	MATH_tdst_Vector	*_pst_Pt2
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_H1, st_H2, *h1, *h2;
	float				t;
	MATH_tdst_Vector	N, N1, N2, V1, V2, V3;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&V1, b, a);
	MATH_SubVector(&V2, d, c);

	MATH_CrossProduct(&N, &V1, &V2);
	if(MATH_b_NulVector(&N)) return 0;

	MATH_NormalizeVector(&N, &N);

	MATH_SubVector(&V3, c, a);

	/* Point on line ab */
	h2 = (_pst_Pt2) ? _pst_Pt2 : &st_H2;
	MATH_CrossProduct(&N1, &V1, &N);

	t = -MATH_f_DotProduct(&V3, &N1);
	t /= MATH_f_DotProduct(&V2, &N1);
	if(t2) *t2 = t;
	MATH_ScaleVector(h2, &V2, t);
	MATH_AddEqualVector(h2, c);

	/* Point on line cd */
	h1 = (_pst_Pt1) ? _pst_Pt1 : &st_H1;
	MATH_CrossProduct(&N2, &V2, &N);

	t = MATH_f_DotProduct(&V3, &N2);
	t /= MATH_f_DotProduct(&V1, &N2);
	if(t1) *t1 = t;
	MATH_ScaleVector(h1, &V1, t);
	MATH_AddEqualVector(h1, a);

	if(_pf_Distance) *_pf_Distance = MATH_f_Distance(h1, h2);

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_IntersectPlaneLine
(
	MATH_tdst_Vector	*a,
	MATH_tdst_Vector	*b,
	MATH_tdst_Vector	*c,
	MATH_tdst_Vector	*d,
	MATH_tdst_Vector	*e,
	MATH_tdst_Vector	*_pst_Pt
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				t;
	MATH_tdst_Vector	AB, AC, N;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CrossProduct(&N, d, e);
	MATH_SubVector(&AB, b, a);
	MATH_SubVector(&AC, c, a);

	t = MATH_f_DotProduct(&AB, &N);
	if(fAbs(t) < Cf_EpsilonBig) return 0;

	t = MATH_f_DotProduct(&AC, &N) / t;
	MATH_ScaleVector(_pst_Pt, &AB, t);
	MATH_AddEqualVector(_pst_Pt, a);
	return 1;
}

/*
 =======================================================================================================================
	a is start of the line £
	b is end of the line £
	c is one a corner of the quad £
	d is one edge of quad starting from c £
	e is the other edge of quad starting from c £
	dist est la distance entre le point start et le point d'intersection (valable seulement si retour 1 )
 =======================================================================================================================
 */
LONG SOFT_l_IntersectQuadLine
(
	MATH_tdst_Vector	*a,
	MATH_tdst_Vector	*b,
	MATH_tdst_Vector	*c,
	MATH_tdst_Vector	*d,
	MATH_tdst_Vector	*e,
	float				*_pf_Dist
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				t, dist;
	MATH_tdst_Vector	AB, AC, N, Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CrossProduct(&N, d, e);
	MATH_SubVector(&AB, b, a);
	MATH_SubVector(&AC, c, a);

	t = MATH_f_DotProduct(&AB, &N);
	if(fAbs(t) < Cf_EpsilonBig) return 0;

	dist = MATH_f_DotProduct(&AC, &N) / t;
	MATH_AddScaleVector( &Temp, a, &AB, dist);
	MATH_SubEqualVector( &Temp, c );
	
	t = MATH_f_DotProduct( &Temp, d ) / MATH_f_SqrVector( d );
	if ( (t < 0) || ( t > 1 ) ) 
		return  0;
		
	t = MATH_f_DotProduct( &Temp, e ) / MATH_f_SqrVector( e );
	if ( (t < 0) || ( t > 1 ) ) 
		return  0;
		
	if ( _pf_Dist )
		*_pf_Dist = dist;

	
	return 1;
}


#endif

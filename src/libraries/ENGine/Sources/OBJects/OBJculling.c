/*$T OBJculling.c GC! 1.081 06/28/02 10:19:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"

#if defined(_XENON)
#include "ppcintrinsics.h"
#endif

#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
MATH_tdst_Vector		g_stGlobalNormPlaneUp;
MATH_tdst_Vector		g_stGlobalNormPlaneDown;
MATH_tdst_Vector		g_stGlobalNormPlaneLeft;
MATH_tdst_Vector		g_stGlobalNormPlaneRight;
MATH_tdst_Vector		g_stOverlayed_GlobalNormPlaneUp;
MATH_tdst_Vector		g_stOverlayed_GlobalNormPlaneDown;
MATH_tdst_Vector		g_stOverlayed_GlobalNormPlaneLeft;
MATH_tdst_Vector		g_stOverlayed_GlobalNormPlaneRight;
static MATH_tdst_Vector g_stGlobalCameraPos;
static MATH_tdst_Vector g_stGlobalCameraSight;
static float			g_fDistance;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char OBJ_c_GetCullingMask(OBJ_tdst_GameObject *pst_GO)
{
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_GreatFather;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(
				(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_AdditionalMatrix))
			&&	(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_AddMatArePointer))
			&&	pst_GO->pst_Base
			&&	pst_GO->pst_Base->pst_AddMatrix
			&&	pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr
		 )
		{
			pst_GreatFather = pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr->pst_GO;
		}
		else
		{
			pst_GreatFather = ANI_pst_GetReference(pst_GO);
		}

		if(pst_GreatFather->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
			return pst_GreatFather->pst_Base->pst_GameObjectAnim->c_CullingMask;
		else
			return OBJ_Culling_Z_OverLap + OBJ_Culling_XY_OverLap;
	}
	else
	{
		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
		{
			return pst_GO->pst_Base->pst_GameObjectAnim->c_CullingMask;
		}
		else
		{
			if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
				return pst_GO->pst_Base->pst_Visu->c_CullingMask;
			else
				return OBJ_Culling_Z_OverLap + OBJ_Culling_XY_OverLap;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
void OBJ_UpdateCullingVars(CAM_tdst_Camera *_pst_Cam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*pst_Matrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * The st_Matrix is to go from the Camera coordinate system to the Global world
	 * coordinate system.
	 */
	pst_Matrix = &_pst_Cam->st_Matrix;

	MATH_TransformVector(&g_stGlobalNormPlaneUp, pst_Matrix, &_pst_Cam->st_NormPlaneUp);
	MATH_TransformVector(&g_stGlobalNormPlaneDown, pst_Matrix, &_pst_Cam->st_NormPlaneDown);
	MATH_TransformVector(&g_stGlobalNormPlaneLeft, pst_Matrix, &_pst_Cam->st_NormPlaneLeft);
	MATH_TransformVector(&g_stGlobalNormPlaneRight, pst_Matrix, &_pst_Cam->st_NormPlaneRight);
	
	/* Compute Overlayed Vector For Clipping */ 
	MATH_BlendVector(&g_stOverlayed_GlobalNormPlaneUp 	, &g_stGlobalNormPlaneUp 	, &g_stGlobalNormPlaneDown , 0.25f);
	MATH_BlendVector(&g_stOverlayed_GlobalNormPlaneDown , &g_stGlobalNormPlaneUp 	, &g_stGlobalNormPlaneDown , 0.75f);
	MATH_BlendVector(&g_stOverlayed_GlobalNormPlaneLeft , &g_stGlobalNormPlaneLeft 	, &g_stGlobalNormPlaneRight , 0.25f);
	MATH_BlendVector(&g_stOverlayed_GlobalNormPlaneRight, &g_stGlobalNormPlaneLeft 	, &g_stGlobalNormPlaneRight , 0.75f);
	
	MATH_NormalizeEqualVector(&g_stOverlayed_GlobalNormPlaneUp);
	MATH_NormalizeEqualVector(&g_stOverlayed_GlobalNormPlaneDown);
	MATH_NormalizeEqualVector(&g_stOverlayed_GlobalNormPlaneLeft);
	MATH_NormalizeEqualVector(&g_stOverlayed_GlobalNormPlaneRight);

	g_stGlobalCameraPos = *(MATH_pst_GetTranslation(&_pst_Cam->st_Matrix));
	MATH_CopyVector(&g_stGlobalCameraSight, MATH_pst_GetZAxis(&_pst_Cam->st_Matrix));

	g_fDistance = (_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective) ? 0 : -(_pst_Cam->f_IsoFactor * _pst_Cam->f_IsoZoom);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_CullingOBBox
(
	MATH_tdst_Vector	*_pst_LMin,
	MATH_tdst_Vector	*_pst_LMax,
	MATH_tdst_Matrix	*_pst_GlobalObject,
	CAM_tdst_Camera		*_pst_Cam
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_LocalObjectToCamera;
	ULONG				l_Cpt;
	float				f_MaxZ, f_Dot, f_MaxDot;
	MATH_tdst_Vector	st_OCS_Point;		/* OCS: Object CoordSys. */
	MATH_tdst_Vector	st_CCS_Point[8];	/* CCS: Camera CoordSys */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_MaxZ = -10000.0f;

	/*
	 * We compute the matrix to go from the Local coordinate system of the object to
	 * the Camera one.
	 */
	MATH_MulMatrixMatrix(&st_LocalObjectToCamera, _pst_GlobalObject, &_pst_Cam->st_InverseMatrix);

	for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
	{
		st_OCS_Point.x = (l_Cpt & 1) ? _pst_LMin->x : _pst_LMax->x;
		st_OCS_Point.y = (l_Cpt & 2) ? _pst_LMin->y : _pst_LMax->y;
		st_OCS_Point.z = (l_Cpt & 4) ? _pst_LMin->z : _pst_LMax->z;

		MATH_TransformVertexNoScale(&st_CCS_Point[l_Cpt], &st_LocalObjectToCamera, &st_OCS_Point);
#if defined(_XENON)
		f_MaxZ = (float) fMax(st_CCS_Point[l_Cpt].z, f_MaxZ);
#else
		if(fSup(st_CCS_Point[l_Cpt].z, f_MaxZ)) f_MaxZ = st_CCS_Point[l_Cpt].z;
#endif
	}

	if(fInfZero(f_MaxZ)) return TRUE;

	f_MaxDot = -10000.0f;
	for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
	{
		f_Dot = MATH_f_DotProduct(&st_CCS_Point[l_Cpt], &_pst_Cam->st_NormPlaneLeft);
#if defined(_XENON)
		f_MaxDot = (float) fMax(f_Dot, f_MaxDot);
#else
		if(fSup(f_Dot, f_MaxDot)) f_MaxDot = f_Dot;
#endif
	}

	if(fInfZero(f_MaxDot)) return TRUE;

	f_MaxDot = -10000.0f;
	for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
	{
		f_Dot = MATH_f_DotProduct(&st_CCS_Point[l_Cpt], &_pst_Cam->st_NormPlaneUp);
#if defined(_XENON)
		f_MaxDot = (float) fMax(f_Dot, f_MaxDot);
#else
		if(fSup(f_Dot, f_MaxDot)) f_MaxDot = f_Dot;
#endif
	}

	if(fInfZero(f_MaxDot)) return TRUE;

	f_MaxDot = -10000.0f;
	for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
	{
		f_Dot = MATH_f_DotProduct(&st_CCS_Point[l_Cpt], &_pst_Cam->st_NormPlaneRight);
#if defined(_XENON)
		f_MaxDot = (float) fMax(f_Dot, f_MaxDot);
#else
		if(fSup(f_Dot, f_MaxDot)) f_MaxDot = f_Dot;
#endif
	}

	if(fInfZero(f_MaxDot)) return TRUE;

	f_MaxDot = -10000.0f;
	for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
	{
		f_Dot = MATH_f_DotProduct(&st_CCS_Point[l_Cpt], &_pst_Cam->st_NormPlaneDown);
		if(fSup(f_Dot, f_MaxDot)) f_MaxDot = f_Dot;
	}
#if defined(_XENON)
		f_MaxDot = (float) fMax(f_Dot, f_MaxDot);
#else
	if(fInfZero(f_MaxDot)) return TRUE;
#endif
	return FALSE;
}
#ifdef JADEFUSION
BOOL OBJ_CullingOBBoxView(MATH_tdst_Vector* _pst_LMin, 
                          MATH_tdst_Vector* _pst_LMax, 
                          MATH_tdst_Matrix* _pst_LocalToCam, 
                          CAM_tdst_Camera*  _pst_Cam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG				l_Cpt;
    float				f_MaxZ, f_Dot, f_MaxDot;
    MATH_tdst_Vector	st_OCS_Point;		/* OCS: Object CoordSys. */
    MATH_tdst_Vector	st_CCS_Point[8];	/* CCS: Camera CoordSys */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    f_MaxZ = -10000.0f;

    for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
    {
        st_OCS_Point.x = (l_Cpt & 1) ? _pst_LMin->x : _pst_LMax->x;
        st_OCS_Point.y = (l_Cpt & 2) ? _pst_LMin->y : _pst_LMax->y;
        st_OCS_Point.z = (l_Cpt & 4) ? _pst_LMin->z : _pst_LMax->z;

        MATH_TransformVertexNoScale(&st_CCS_Point[l_Cpt], _pst_LocalToCam, &st_OCS_Point);
#if defined(_XENON)
		f_MaxZ = (float) fMax(st_CCS_Point[l_Cpt].z, f_MaxZ);
#else
		if(fSup(st_CCS_Point[l_Cpt].z, f_MaxZ)) f_MaxZ = st_CCS_Point[l_Cpt].z;
#endif
    }

    if(fInfZero(f_MaxZ)) return TRUE;

    f_MaxDot = -10000.0f;
    for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
    {
        f_Dot = MATH_f_DotProduct(&st_CCS_Point[l_Cpt], &_pst_Cam->st_NormPlaneLeft);
#if defined(_XENON)
		f_MaxDot = (float) fMax(f_Dot, f_MaxDot);
#else
		if(fSup(f_Dot, f_MaxDot)) f_MaxDot = f_Dot;
#endif
    }

    if(fInfZero(f_MaxDot)) return TRUE;

    f_MaxDot = -10000.0f;
    for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
    {
        f_Dot = MATH_f_DotProduct(&st_CCS_Point[l_Cpt], &_pst_Cam->st_NormPlaneUp);
#if defined(_XENON)
		f_MaxDot = (float) fMax(f_Dot, f_MaxDot);
#else
		if(fSup(f_Dot, f_MaxDot)) f_MaxDot = f_Dot;
#endif
	}

    if(fInfZero(f_MaxDot)) return TRUE;

    f_MaxDot = -10000.0f;
    for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
    {
        f_Dot = MATH_f_DotProduct(&st_CCS_Point[l_Cpt], &_pst_Cam->st_NormPlaneRight);
#if defined(_XENON)
		f_MaxDot = (float) fMax(f_Dot, f_MaxDot);
#else
		if(fSup(f_Dot, f_MaxDot)) f_MaxDot = f_Dot;
#endif
    }

    if(fInfZero(f_MaxDot)) return TRUE;

    f_MaxDot = -10000.0f;
    for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
    {
        f_Dot = MATH_f_DotProduct(&st_CCS_Point[l_Cpt], &_pst_Cam->st_NormPlaneDown);
#if defined(_XENON)
		f_MaxDot = (float) fMax(f_Dot, f_MaxDot);
#else
		if(fSup(f_Dot, f_MaxDot)) f_MaxDot = f_Dot;
#endif
	}

    if(fInfZero(f_MaxDot)) return TRUE;

    return FALSE;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef PSX2_TARGET1
BOOL OBJ_CullingAABBox(MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max, char *_pc_Mask)
{
 asm __volatile__ ("
 vmulx $vf05,$vf05,$vf08x
 vadd $vf11,$vf10,$vf00
 vadd $vf21,$vf20,$vf00
 lqc2 $vf10,0(pst_Point)
 vsub $vf20,$vf10,$vf11
   ");
	
	return FALSE;
}
#else
#define MAC_Sup0_01(a) ((*(ULONG *)&a) >> 31)
BOOL OBJ_CullingAABBox(MATH_tdst_Vector **_pst_Box, char *_pc_Mask)
{
	MATH_tdst_Vector	st_VectCamPoint;

	/* Is the AABBox completely BEHIND the camera ?? */
	st_VectCamPoint.x = _pst_Box[MAC_Sup0_01(g_stGlobalCameraSight.x)]->x;
	st_VectCamPoint.y = _pst_Box[MAC_Sup0_01(g_stGlobalCameraSight.y)]->y;
	st_VectCamPoint.z = _pst_Box[MAC_Sup0_01(g_stGlobalCameraSight.z)]->z;
	if(MATH_f_DotProduct(&st_VectCamPoint, &g_stGlobalCameraSight) < g_fDistance) return TRUE;

	/* Plane Left */
	st_VectCamPoint.x = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneLeft.x)]->x;
	st_VectCamPoint.y = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneLeft.y)]->y;
	st_VectCamPoint.z = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneLeft.z)]->z;
	if(MATH_f_DotProduct(&st_VectCamPoint, &g_stGlobalNormPlaneLeft) < g_fDistance) return TRUE;

	/* Plane Right */
	st_VectCamPoint.x = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneRight.x)]->x;
	st_VectCamPoint.y = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneRight.y)]->y;
	st_VectCamPoint.z = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneRight.z)]->z;
	if(MATH_f_DotProduct(&st_VectCamPoint, &g_stGlobalNormPlaneRight) < g_fDistance) return TRUE;

	/* Plane Up */
	st_VectCamPoint.x = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneUp.x)]->x;
	st_VectCamPoint.y = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneUp.y)]->y;
	st_VectCamPoint.z = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneUp.z)]->z;
	if(MATH_f_DotProduct(&st_VectCamPoint, &g_stGlobalNormPlaneUp) < g_fDistance) return TRUE;

	/* Plane Down */
	st_VectCamPoint.x = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneDown.x)]->x;
	st_VectCamPoint.y = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneDown.y)]->y;
	st_VectCamPoint.z = _pst_Box[MAC_Sup0_01(g_stGlobalNormPlaneDown.z)]->z;
	if(MATH_f_DotProduct(&st_VectCamPoint, &g_stGlobalNormPlaneDown) < g_fDistance) return TRUE;

	if(_pc_Mask)
	{
		MATH_tdst_Vector *p_Swap;
		p_Swap = _pst_Box[0];
		_pst_Box[0] = _pst_Box[1];
		_pst_Box[1] = p_Swap;
		*_pc_Mask = 0;
		
		st_VectCamPoint.x = _pst_Box[MAC_Sup0_01(g_stGlobalCameraSight.x)]->x;
		st_VectCamPoint.y = _pst_Box[MAC_Sup0_01(g_stGlobalCameraSight.y)]->y;
		st_VectCamPoint.z = _pst_Box[MAC_Sup0_01(g_stGlobalCameraSight.z)]->z;
		if(MATH_f_DotProduct(&st_VectCamPoint, &g_stGlobalCameraSight) < g_fDistance)
			*_pc_Mask |= OBJ_Culling_Z_OverLap;

		st_VectCamPoint.x = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneLeft.x)]->x;
		st_VectCamPoint.y = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneLeft.y)]->y;
		st_VectCamPoint.z = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneLeft.z)]->z;
		if(MATH_f_DotProduct(&st_VectCamPoint, &g_stOverlayed_GlobalNormPlaneLeft) < g_fDistance)
		{
			*_pc_Mask |= OBJ_Culling_XY_OverLap;
			return FALSE;
		}

		st_VectCamPoint.x = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneRight.x)]->x;
		st_VectCamPoint.y = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneRight.y)]->y;
		st_VectCamPoint.z = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneRight.z)]->z;
		if(MATH_f_DotProduct(&st_VectCamPoint, &g_stOverlayed_GlobalNormPlaneRight) < g_fDistance)
		{
			*_pc_Mask |= OBJ_Culling_XY_OverLap;
			return FALSE;
		}

		st_VectCamPoint.x = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneUp.x)]->x;
		st_VectCamPoint.y = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneUp.y)]->y;
		st_VectCamPoint.z = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneUp.z)]->z;
		if(MATH_f_DotProduct(&st_VectCamPoint, &g_stOverlayed_GlobalNormPlaneUp) < g_fDistance)
		{
			*_pc_Mask |= OBJ_Culling_XY_OverLap;
			return FALSE;
		}

		st_VectCamPoint.x = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneDown.x)]->x;
		st_VectCamPoint.y = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneDown.y)]->y;
		st_VectCamPoint.z = _pst_Box[MAC_Sup0_01(g_stOverlayed_GlobalNormPlaneDown.z)]->z;
		if(MATH_f_DotProduct(&st_VectCamPoint, &g_stOverlayed_GlobalNormPlaneDown) < g_fDistance)
		{
			*_pc_Mask |= OBJ_Culling_XY_OverLap;
			return FALSE;
		}

	}
	return FALSE;
}
#endif
extern void SOFT_TransformAndProject(MATH_tdst_Vector *, MATH_tdst_Vector *, LONG, CAM_tdst_Camera *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_Frame_CullingAABBox
(
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max,
	CAM_tdst_Camera		*_pst_Cam,
	LONG				_l_Width,
	LONG				_l_Height,
	float				*_pf_PercentBV,
	float				*_pf_PercentScreen
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	ast_GlobalBox[8], ast_ScreenBox[8];
	UCHAR				uc_XFlag, uc_YFlag, i;
	float				f_HalfX, f_HalfY;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ast_GlobalBox[0].x = _pst_Max->x;
	ast_GlobalBox[0].y = _pst_Max->y;
	ast_GlobalBox[0].z = _pst_Max->z;

	ast_GlobalBox[1].x = _pst_Max->x;
	ast_GlobalBox[1].y = _pst_Max->y;
	ast_GlobalBox[1].z = _pst_Min->z;

	ast_GlobalBox[2].x = _pst_Max->x;
	ast_GlobalBox[2].y = _pst_Min->y;
	ast_GlobalBox[2].z = _pst_Max->z;

	ast_GlobalBox[3].x = _pst_Max->x;
	ast_GlobalBox[3].y = _pst_Min->y;
	ast_GlobalBox[3].z = _pst_Min->z;

	ast_GlobalBox[4].x = _pst_Min->x;
	ast_GlobalBox[4].y = _pst_Max->y;
	ast_GlobalBox[4].z = _pst_Max->z;

	ast_GlobalBox[5].x = _pst_Min->x;
	ast_GlobalBox[5].y = _pst_Max->y;
	ast_GlobalBox[5].z = _pst_Min->z;

	ast_GlobalBox[6].x = _pst_Min->x;
	ast_GlobalBox[6].y = _pst_Min->y;
	ast_GlobalBox[6].z = _pst_Max->z;

	ast_GlobalBox[7].x = _pst_Min->x;
	ast_GlobalBox[7].y = _pst_Min->y;
	ast_GlobalBox[7].z = _pst_Min->z;

	uc_XFlag = 0;
	uc_YFlag = 0;

	f_HalfX = ((float) _l_Width) / 2.0f;
	f_HalfY = ((float) _l_Height) / 2.0f;

	SOFT_TransformAndProject(ast_ScreenBox, ast_GlobalBox, 8, _pst_Cam);

	/* Cull test */
	for(i = 0; i < 8; i++)
	{
		if((ast_ScreenBox[i].x == 0x80000000) || (ast_ScreenBox[i].y == 0x80000000)) continue;
		if
		(
			((fAbs(ast_ScreenBox[i].x - _pst_Cam->f_CenterX)) < f_HalfX)
		&&	((fAbs(ast_ScreenBox[i].y - _pst_Cam->f_CenterY)) < f_HalfY)
		) goto Percent;

		if
		(
			(uc_XFlag == 1)
		&&	((fAbs(ast_ScreenBox[i].y - _pst_Cam->f_CenterY)) < f_HalfY)
		&&	(((ast_ScreenBox[i].x - _pst_Cam->f_CenterX) < -f_HalfX))
		) goto Percent;
		if
		(
			(uc_XFlag == 2)
		&&	((fAbs(ast_ScreenBox[i].y - _pst_Cam->f_CenterY)) < f_HalfY)
		&&	(((ast_ScreenBox[i].x - _pst_Cam->f_CenterX) > f_HalfX))
		) goto Percent;

		if
		(
			(uc_YFlag == 1)
		&&	((fAbs(ast_ScreenBox[i].x - _pst_Cam->f_CenterX)) < f_HalfX)
		&&	(((ast_ScreenBox[i].y - _pst_Cam->f_CenterY) < -f_HalfY))
		) goto Percent;
		if
		(
			(uc_YFlag == 2)
		&&	((fAbs(ast_ScreenBox[i].x - _pst_Cam->f_CenterX)) < f_HalfX)
		&&	(((ast_ScreenBox[i].y - _pst_Cam->f_CenterY) > f_HalfY))
		) goto Percent;

		if(!uc_XFlag)
		{
			uc_XFlag = ((ast_ScreenBox[i].x - _pst_Cam->f_CenterX) > f_HalfX) ? 1 : 2;
			uc_YFlag = ((ast_ScreenBox[i].y - _pst_Cam->f_CenterY) > f_HalfY) ? 1 : 2;
		}
	}

	if(_pf_PercentBV) *_pf_PercentBV = 0;
	if(_pf_PercentScreen) *_pf_PercentScreen = 0;
	return TRUE;

Percent:
	if(_pf_PercentBV || _pf_PercentScreen)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		float	MinSX, MaxSX, MinSY, MaxSY;
		float	MinTX, MaxTX, MinTY, MaxTY;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MinSX = f_HalfX;
		MaxSX = -f_HalfX;
		MinSY = f_HalfY;
		MaxSY = -f_HalfY;

		MinTX = f_HalfX;
		MaxTX = -f_HalfX;
		MinTY = f_HalfY;
		MaxTY = -f_HalfY;

		for(i = 0; i < 8; i++)
		{
			/*~~~~~~~~~*/
			float	X, Y;
			/*~~~~~~~~~*/

			if((ast_ScreenBox[i].x == 0x80000000) || (ast_ScreenBox[i].y == 0x80000000)) continue;

			X = (ast_ScreenBox[i].x - _pst_Cam->f_CenterX);
			Y = (ast_ScreenBox[i].y - _pst_Cam->f_CenterY);

			if(X < MinSX) MinSX = X > -f_HalfX ? X : -f_HalfX;
			if(X > MaxSX) MaxSX = X < f_HalfX ? X : f_HalfX;
			if(Y < MinSY) MinSY = Y > -f_HalfY ? Y : -f_HalfY;
			if(Y > MaxSY) MaxSY = Y < f_HalfY ? Y : f_HalfY;

			if(X < MinTX) MinTX = X;
			if(X > MaxTX) MaxTX = X;
			if(Y < MinTY) MinTY = Y;
			if(Y > MaxTY) MaxTY = Y;
		}

		if(_pf_PercentBV) *_pf_PercentBV = ((MaxSX - MinSX) * (MaxSY - MinSY)) / ((MaxTX - MinTX) * (MaxTY - MinTY));
		if(_pf_PercentScreen)
			*_pf_PercentScreen = ((MaxSX - MinSX) * (MaxSY - MinSY)) / ((float) (_l_Width * _l_Height));
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_Frame_CullingSphere
(
	MATH_tdst_Vector	*_pst_GCS_Center,
	float				_f_GCS_Radius,
	CAM_tdst_Camera		*_pst_Cam,
	LONG				_l_Width,
	LONG				_l_Height,
	float				*_pf_PercentBV,
	float				*_pf_PercentScreen
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_ScreenCenter, ast_ScreenBox[4];
	MATH_tdst_Vector	st_ScreenFakeCenter, st_ScreenFakeRadius;
	MATH_tdst_Vector	st_GCS_FakeRadius, st_GCS_FakeCenter;
	UCHAR				uc_XFlag, uc_YFlag, i;
	float				f_HalfX, f_HalfY;
	float				f_ScreenRadius;
	float				f_Distance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uc_XFlag = 0;
	uc_YFlag = 0;

	f_HalfX = ((float) _l_Width) / 2.0f;
	f_HalfY = ((float) _l_Height) / 2.0f;

	if(_pf_PercentBV) *_pf_PercentBV = 0;
	if(_pf_PercentScreen) *_pf_PercentScreen = 0;

	_pst_Cam->pst_ObjectToCameraMatrix = &_pst_Cam->st_InverseMatrix;

	f_Distance = MATH_f_Distance(&_pst_Cam->st_Matrix.T, _pst_GCS_Center);

	MATH_InitVector(&st_GCS_FakeCenter, _pst_Cam->st_Matrix.Kx, _pst_Cam->st_Matrix.Ky, _pst_Cam->st_Matrix.Kz);
	MATH_MulEqualVector(&st_GCS_FakeCenter, f_Distance);
	MATH_AddEqualVector(&st_GCS_FakeCenter, &_pst_Cam->st_Matrix.T);

	MATH_MulVector(&st_GCS_FakeRadius,  &((MATH_tdst_33Matrix *)&_pst_Cam->st_Matrix)->I, _f_GCS_Radius);
	MATH_AddEqualVector(&st_GCS_FakeRadius, &st_GCS_FakeCenter);

	SOFT_TransformAndProject(&st_ScreenFakeRadius, &st_GCS_FakeRadius, 1, _pst_Cam);
	SOFT_TransformAndProject(&st_ScreenFakeCenter, &st_GCS_FakeCenter, 1, _pst_Cam);
	SOFT_TransformAndProject(&st_ScreenCenter, _pst_GCS_Center, 1, _pst_Cam);

	if((st_ScreenCenter.x == 0x80000000) || (st_ScreenCenter.y == 0x80000000)) return TRUE;

	MATH_SubEqualVector(&st_ScreenFakeRadius, &st_ScreenFakeCenter);

	f_ScreenRadius = fSqrt(st_ScreenFakeRadius.x * st_ScreenFakeRadius.x + st_ScreenFakeRadius.y * st_ScreenFakeRadius.y);

	/* Culling Test */
	if
	(
		((fAbs(st_ScreenCenter.x - _pst_Cam->f_CenterX)) > f_HalfX + f_ScreenRadius)
	&&	((fAbs(st_ScreenCenter.y - _pst_Cam->f_CenterY)) > f_HalfY + f_ScreenRadius)
	) return TRUE;

	/* For BV % computing, use a fake box that englobes the screen circle */
	ast_ScreenBox[0].x = st_ScreenCenter.x + f_ScreenRadius;
	ast_ScreenBox[0].y = st_ScreenCenter.y + f_ScreenRadius;

	ast_ScreenBox[1].x = st_ScreenCenter.x + f_ScreenRadius;
	ast_ScreenBox[1].y = st_ScreenCenter.y - f_ScreenRadius;

	ast_ScreenBox[2].x = st_ScreenCenter.x - f_ScreenRadius;
	ast_ScreenBox[2].y = st_ScreenCenter.y - f_ScreenRadius;

	ast_ScreenBox[3].x = st_ScreenCenter.x - f_ScreenRadius;
	ast_ScreenBox[3].y = st_ScreenCenter.y + f_ScreenRadius;

	if(_pf_PercentBV || _pf_PercentScreen)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		float	MinSX, MaxSX, MinSY, MaxSY;
		float	MinSX2, MaxSX2, MinSY2, MaxSY2;
		float	MinTX, MaxTX, MinTY, MaxTY;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MinSX = f_HalfX;
		MaxSX = -f_HalfX;
		MinSY = f_HalfY;
		MaxSY = -f_HalfY;

		MinSX2 = f_HalfX;
		MaxSX2 = -f_HalfX;
		MinSY2 = f_HalfY;
		MaxSY2 = -f_HalfY;

		MinTX = f_HalfX;
		MaxTX = -f_HalfX;
		MinTY = f_HalfY;
		MaxTY = -f_HalfY;

		for(i = 0; i < 4; i++)
		{
			/*~~~~~~~~~*/
			float	X, Y;
			/*~~~~~~~~~*/

			X = (ast_ScreenBox[i].x - _pst_Cam->f_CenterX);
			Y = (ast_ScreenBox[i].y - _pst_Cam->f_CenterY);

			if(X < MinSX) MinSX = X > -f_HalfX ? X : -f_HalfX;
			if(X > MaxSX) MaxSX = X < f_HalfX ? X : f_HalfX;
			if(Y < MinSY) MinSY = Y > -f_HalfY ? Y : -f_HalfY;
			if(Y > MaxSY) MaxSY = Y < f_HalfY ? Y : f_HalfY;

			if(X < MinSX2) MinSX2 = X;
			if(X > MaxSX2) MaxSX2 = X;
			if(Y < MinSY2) MinSY2 = Y;
			if(Y > MaxSY2) MaxSY2 = Y;


			if(X < MinTX) MinTX = X;
			if(X > MaxTX) MaxTX = X;
			if(Y < MinTY) MinTY = Y;
			if(Y > MaxTY) MaxTY = Y;
		}

		if(_pf_PercentBV)  *_pf_PercentBV = ((MaxSX - MinSX) * (MaxSY - MinSY)) / ((MaxTX - MinTX) * (MaxTY - MinTY));
		if(_pf_PercentScreen) *_pf_PercentScreen = ((MaxSX2 - MinSX2) * (MaxSY2 - MinSY2)) / ((float) (_l_Width * _l_Height));
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_Frame_CullingPoint
(
	MATH_tdst_Vector	*_pst_GlobalPoint,
	CAM_tdst_Camera		*_pst_Cam,
	LONG				_l_Width,
	LONG				_l_Height
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_ScreenPoint;
	float				f_HalfX, f_HalfY;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_HalfX = ((float) _l_Width) / 2.0f;
	f_HalfY = ((float) _l_Height) / 2.0f;

	SOFT_TransformAndProject(&st_ScreenPoint, _pst_GlobalPoint, 1, _pst_Cam);

	if((st_ScreenPoint.x == 0x80000000) || (st_ScreenPoint.y == 0x80000000)) return TRUE;

	if((fAbs(st_ScreenPoint.x - _pst_Cam->f_CenterX)) > f_HalfX) return TRUE;
	if((fAbs(st_ScreenPoint.y - _pst_Cam->f_CenterY)) > f_HalfY) return TRUE;

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_CullingPointOutside
(
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_Left,
	MATH_tdst_Vector	*_pst_Up,
	MATH_tdst_Vector	*_pst_Right,
	MATH_tdst_Vector	*_pst_Down
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_VectCamPoint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Plane Left */
	MATH_SubVector(&st_VectCamPoint, _pst_Point, &g_stGlobalCameraPos);
	if(MATH_f_DotProduct(&st_VectCamPoint, _pst_Left) < g_fDistance) return TRUE;

	/* Plane Right */
	MATH_SubVector(&st_VectCamPoint, _pst_Point, &g_stGlobalCameraPos);
	if(MATH_f_DotProduct(&st_VectCamPoint, _pst_Right) < g_fDistance) return TRUE;

	/* Plane Up */
	MATH_SubVector(&st_VectCamPoint, _pst_Point, &g_stGlobalCameraPos);
	if(MATH_f_DotProduct(&st_VectCamPoint, _pst_Up) < g_fDistance) return TRUE;

	/* Plane Down */
	MATH_SubVector(&st_VectCamPoint, _pst_Point, &g_stGlobalCameraPos);
	if(MATH_f_DotProduct(&st_VectCamPoint, _pst_Down) < g_fDistance) return TRUE;

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_CullingPointInside
(
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_Left,
	MATH_tdst_Vector	*_pst_Up,
	MATH_tdst_Vector	*_pst_Right,
	MATH_tdst_Vector	*_pst_Down,
	int					cullcull
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_VectCamPoint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Plane Left */
	if(cullcull & 1)
	{
		MATH_SubVector(&st_VectCamPoint, _pst_Point, &g_stGlobalCameraPos);
		if(MATH_f_DotProduct(&st_VectCamPoint, _pst_Left) >= g_fDistance) return FALSE;
	}

	/* Plane Right */
	if(cullcull & 4)
	{
		MATH_SubVector(&st_VectCamPoint, _pst_Point, &g_stGlobalCameraPos);
		if(MATH_f_DotProduct(&st_VectCamPoint, _pst_Right) >= g_fDistance) return FALSE;
	}

	/* Plane Up */
	if(cullcull & 2)
	{
		MATH_SubVector(&st_VectCamPoint, _pst_Point, &g_stGlobalCameraPos);
		if(MATH_f_DotProduct(&st_VectCamPoint, _pst_Up) >= g_fDistance) return FALSE;
	}

	/* Plane Down */
	if(cullcull & 8)
	{
		MATH_SubVector(&st_VectCamPoint, _pst_Point, &g_stGlobalCameraPos);
		if(MATH_f_DotProduct(&st_VectCamPoint, _pst_Down) >= g_fDistance) return FALSE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_CullingSphere(MATH_tdst_Vector *_pst_Center, float f_Radius)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_SpherePoint;
	MATH_tdst_Vector	st_VectCamPoint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st_SpherePoint.x = fAdd(_pst_Center->x, fSupZero(g_stGlobalNormPlaneLeft.x) ? f_Radius : -f_Radius);
	st_SpherePoint.y = fAdd(_pst_Center->y, fSupZero(g_stGlobalNormPlaneLeft.y) ? f_Radius : -f_Radius);
	st_SpherePoint.z = fAdd(_pst_Center->z, fSupZero(g_stGlobalNormPlaneLeft.z) ? f_Radius : -f_Radius);
	MATH_SubVector(&st_VectCamPoint, &st_SpherePoint, &g_stGlobalCameraPos);

	if(fInfZero(MATH_f_DotProduct(&st_VectCamPoint, &g_stGlobalNormPlaneLeft))) return TRUE;

	st_SpherePoint.x = fAdd(_pst_Center->x, fSupZero(g_stGlobalNormPlaneRight.x) ? f_Radius : -f_Radius);
	st_SpherePoint.y = fAdd(_pst_Center->y, fSupZero(g_stGlobalNormPlaneRight.y) ? f_Radius : -f_Radius);
	st_SpherePoint.z = fAdd(_pst_Center->z, fSupZero(g_stGlobalNormPlaneRight.z) ? f_Radius : -f_Radius);
	MATH_SubVector(&st_VectCamPoint, &st_SpherePoint, &g_stGlobalCameraPos);

	if(fInfZero(MATH_f_DotProduct(&st_VectCamPoint, &g_stGlobalNormPlaneRight))) return TRUE;

	st_SpherePoint.x = fAdd(_pst_Center->x, fSupZero(g_stGlobalNormPlaneUp.x) ? f_Radius : -f_Radius);
	st_SpherePoint.y = fAdd(_pst_Center->y, fSupZero(g_stGlobalNormPlaneUp.y) ? f_Radius : -f_Radius);
	st_SpherePoint.z = fAdd(_pst_Center->z, fSupZero(g_stGlobalNormPlaneUp.z) ? f_Radius : -f_Radius);
	MATH_SubVector(&st_VectCamPoint, &st_SpherePoint, &g_stGlobalCameraPos);

	if(fInfZero(MATH_f_DotProduct(&st_VectCamPoint, &g_stGlobalNormPlaneUp))) return TRUE;

	st_SpherePoint.x = fAdd(_pst_Center->x, fSupZero(g_stGlobalNormPlaneDown.x) ? f_Radius : -f_Radius);
	st_SpherePoint.y = fAdd(_pst_Center->y, fSupZero(g_stGlobalNormPlaneDown.y) ? f_Radius : -f_Radius);
	st_SpherePoint.z = fAdd(_pst_Center->z, fSupZero(g_stGlobalNormPlaneDown.z) ? f_Radius : -f_Radius);
	MATH_SubVector(&st_VectCamPoint, &st_SpherePoint, &g_stGlobalCameraPos);

	if(fInfZero(MATH_f_DotProduct(&st_VectCamPoint, &g_stGlobalNormPlaneDown))) return TRUE;

	return FALSE;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_CullingZDx(OBJ_tdst_GameObject *_pst_GO, COL_tdst_ZDx *_pst_ZDx, CAM_tdst_Camera *_pst_Cam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Type;
	MATH_tdst_Matrix	*pst_Matrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Type = COL_Zone_GetType(_pst_ZDx);
	pst_Matrix = OBJ_pst_GetAbsoluteMatrix(_pst_GO);

	switch(ul_Type)
	{
	case COL_C_Zone_Box:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_GCS_Center, st_Center;
			float				f_FakeRadius, f_GCS_FakeRadius;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_SubVector
			(
				&st_Center,
				COL_pst_Shape_GetMax(_pst_ZDx->p_Shape),
				COL_pst_Shape_GetMin(_pst_ZDx->p_Shape)
			);
			MATH_MulEqualVector(&st_Center, 0.5f);
			MATH_AddEqualVector(&st_Center, COL_pst_Shape_GetMin(_pst_ZDx->p_Shape));
			f_FakeRadius = MATH_f_Distance(&st_Center, COL_pst_Shape_GetMin(_pst_ZDx->p_Shape));

			MATH_TransformVertex(&st_GCS_Center, pst_Matrix, &st_Center);
			if(MATH_b_TestScaleType(pst_Matrix))
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	st_Scale;
				float				f_Max;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_GetScale(&st_Scale, pst_Matrix);
				f_Max = fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
				f_GCS_FakeRadius = f_FakeRadius * f_Max;
			}
			else
				f_GCS_FakeRadius = f_FakeRadius;
			return OBJ_CullingSphere(&st_GCS_Center, f_GCS_FakeRadius);
		}
		break;

	case COL_C_Zone_Sphere:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_GCS_Center;
			float				f_GCS_Radius;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_TransformVertex(&st_GCS_Center, pst_Matrix, COL_pst_Shape_GetCenter(_pst_ZDx->p_Shape));
			if(MATH_b_TestScaleType(pst_Matrix))
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	st_Scale;
				float				f_Max;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_GetScale(&st_Scale, pst_Matrix);
				f_Max = fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
				f_GCS_Radius = COL_f_Shape_GetRadius(_pst_ZDx->p_Shape) * f_Max;
			}
			else
				f_GCS_Radius = COL_f_Shape_GetRadius(_pst_ZDx->p_Shape);
			return OBJ_CullingSphere(&st_GCS_Center, f_GCS_Radius);
		}
		break;
	}

	return FALSE;
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_CullingObject(OBJ_tdst_GameObject *_pst_GO, CAM_tdst_Camera *_pst_Cam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_BVCulled;
	void				*pst_BV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	pst_BV = _pst_GO->pst_BV;
	if(OBJ_HasNoBV(_pst_GO))
		return FALSE;
	else
	{
		if(OBJ_BV_IsAABBox(pst_BV))
		{
			MATH_tdst_Vector	st_Min ONLY_PSX2_ALIGNED(16); 
			MATH_tdst_Vector	st_Max ONLY_PSX2_ALIGNED(16);
			MATH_tdst_Vector	*pst_Box[2];
			MATH_tdst_Vector	st_Center;
			char c_CullingMAsk;
			MATH_SubVector(&st_Center, OBJ_pst_GetAbsolutePosition(_pst_GO) , &g_stGlobalCameraPos);
			MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_BV), &st_Center);
			MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_BV), &st_Center);
			pst_Box[0] = &st_Max;
			pst_Box[1] = &st_Min;
			b_BVCulled = OBJ_CullingAABBox(pst_Box, &c_CullingMAsk);
			
			if
			(
				OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)
			&&	_pst_GO->pst_Base
			&&	_pst_GO->pst_Base->pst_GameObjectAnim
			)
			{
				_pst_GO->pst_Base->pst_GameObjectAnim->c_CullingMask = c_CullingMAsk;
			}
			else
			{
				if
				(
					OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)
				&&	_pst_GO->pst_Base
				&&	_pst_GO->pst_Base->pst_Visu
				)
				{
					_pst_GO->pst_Base->pst_Visu->c_CullingMask = c_CullingMAsk;
				}
			}
		}
		else	/* Sphere */
		{
			MATH_tdst_Vector	st_Center;
			MATH_TransformVertexNoScale(&st_Center, OBJ_pst_GetAbsoluteMatrix(_pst_GO), OBJ_pst_BV_GetCenter(pst_BV));

			b_BVCulled = OBJ_CullingSphere(&st_Center, OBJ_f_BV_GetRadius(pst_BV));
		}

		if(b_BVCulled)
		{
#ifdef ACTIVE_EDITORS
			/* In ACTIVE_EDITORS, we must not cull the object if one of its zones is visible. */
			if(GDI_gpst_CurDD->ul_DisplayFlags & (GDI_Cul_DF_ShowZDM | GDI_Cul_DF_ShowZDE))
			{
				if(OBJ_b_TestIdentityFlag(_pst_GO, (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					COL_tdst_Instance	*pst_Instance;
					COL_tdst_ZDx		**dpst_ZDx, **dpst_LastZDx;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
					dpst_ZDx = pst_Instance->dpst_ZDx;
					dpst_LastZDx = dpst_ZDx +
					pst_Instance->uc_NbOfZDx;
					for(; dpst_ZDx < dpst_LastZDx; dpst_ZDx++)
					{
						if(!OBJ_CullingZDx(_pst_GO, *dpst_ZDx, _pst_Cam)) return FALSE;
					}
				}
			}

#endif
			return TRUE;
		}

		return FALSE;
	}
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}

#endif

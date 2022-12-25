/*$T STRrender.c GC! 1.081 03/03/04 09:56:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "GDInterface/GDInterface.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEODebugObject.h"
#include "STRing/STRstruct.h"
#include "STRing/STRdata.h"
#include "GFX/GFX.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "AIinterp/Sources/AIengine.h"
#include "LINKs/LINKtoed.h"
#ifdef JADEFUSION
#include "INOut/INO.h"
#endif
#ifdef PSX2_TARGET

/* mamagouille */
#include "PSX2debug.h"
#endif
#include "BASe/BENch/BENch.h"

#ifdef _GAMECUBE
#ifndef _FINAL_
#include "GXI_GC/GXI_dbg.h"
#endif
#endif
#ifdef _XBOX
#include "GX8/RASter/Gx8_CheatFlags.h"
#include "GX8/Gx8.h"
#endif /* _XBOX */

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/* Type of projection for text */
#define SPRITE_Scale	0.01f
#define SPRITE_Add		0.06f

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

static ULONG	sul_SaveDrawMask;
static ULONG	sul_SaveDisplayInfo;

#if !defined(ACTIVE_EDITORS)
float			STR_f_XOffset = 0.0f;
float			STR_f_XFactor = 1.0f;
float			STR_f_YOffset = 0.0f;
float			STR_f_YFactor = 1.0f;
#endif
extern ULONG	AI_C_Callback;
#ifdef JADEFUSION
extern int		INO_gai_PresentLanguage[INO_e_MaxLangNb];
#endif
/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_HasSomethingToRender(GRO_tdst_Visu *_pst_Visu, GEO_tdst_Object **ppst_PickableObject)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	i_Index;
	/*~~~~~~~~~~~~~~~~~~~~*/

	*ppst_PickableObject = NULL;

	if(!(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible)) return TRUE;

#ifdef ACTIVE_EDITORS
	i_Index = GDI_gpst_CurDD->pst_CurrentGameObject->ul_InvisibleObjectIndex;
	if(i_Index > GEO_DebugObject_LastInvisible - GEO_DebugObject_Invisible) i_Index = 0;

	if( !GDI_WPShowStatusOn( GDI_gpst_CurDD, i_Index ) ) 
		return FALSE;
#else
	i_Index = 0;
#endif
#ifdef ACTIVE_EDITORS
	* ppst_PickableObject = GEO_pst_DebugObject_Get(GEO_DebugObject_Invisible + i_Index);
#endif
	return TRUE;
}

#if defined(WIN32) || defined(_XENON)
#define STR_TransformPoints(a)
#define STR_TranformAllPoints(a, b)
#define STRDATA_TranformAllPoints(a, b)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_TransformPointsFloat(GEO_Vertex *V)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_2D;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef _XBOX//
	return;
#endif
	st_2D.z = SPRITE_Add + SPRITE_Scale * V->z; /* Z is constant */
	for(i = 0; i < 4; i++)
	{
		st_2D.x = V[i].x * GDI_gpst_CurDD->st_Camera.f_Width;
		st_2D.y = V[i].y * GDI_gpst_CurDD->st_Camera.f_Height;
	}
}

#else

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_TransformPointsFloat(GEO_Vertex *V)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_2D;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
/*#ifdef _XBOX//
	return;
#endif*/
	st_2D.z = SPRITE_Add + SPRITE_Scale * V->z; /* Z is constant */
	for(i = 0; i < 4; i++)
	{
		st_2D.x = V[i].x * GDI_gpst_CurDD->st_Camera.f_Width;
		st_2D.y = V[i].y * GDI_gpst_CurDD->st_Camera.f_Height;
		CAM_2Dto3DCamera2(&GDI_gpst_CurDD->st_Camera, VCast(&V[i]), &st_2D);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_TransformPoints(GEO_Vertex *V)
{
	/*~~~~~~*/
	float	z;
	int		i;
	/*~~~~~~*/

#ifdef _XBOX
	return;//-----------ok
#endif*/

	z = SPRITE_Add + SPRITE_Scale * V->z;
	for(i = 0; i < 4; i++)
	{
		V[i].z = z;
		CAM_2Dto3DCamera2(&GDI_gpst_CurDD->st_Camera, VCast(&V[i]), VCast(&V[i]));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_TranformAllPoints(GEO_Vertex *V, int n)
{
#ifdef _XBOX//
	return;
#endif
	while(n--)
	{
		V->x = (V->x * STR_f_XFactor) + STR_f_XOffset;
		V->y = (V->y * STR_f_YFactor) + STR_f_YOffset;
		V->z = SPRITE_Add;
		CAM_2Dto3DCamera2(&GDI_gpst_CurDD->st_Camera, VCast(V), VCast(V));
		V++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STRDATA_TranformAllPoints(GEO_Vertex *V, int n)
{
#ifdef _XBOX
	return;
#endif

	while(n--)
	{
		V->x *= 640;
		V->y = 480 * (1 - V->y);
		
		V->x = (V->x * STR_f_XFactor) + STR_f_XOffset;
		V->y = (V->y * STR_f_YFactor) + STR_f_YOffset;
		V->z = SPRITE_Add;
		CAM_2Dto3DCamera2(&GDI_gpst_CurDD->st_Camera, VCast(V), VCast(V));
		V++;
	}
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetDisplay(BOOL _b_InvertY)
{
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = &MATH_gst_IdentityMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD), GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);

	sul_SaveDrawMask = GDI_gpst_CurDD->ul_CurrentDrawMask;
	sul_SaveDisplayInfo = GDI_gpst_CurDD->ul_DisplayInfo;

	GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_DontSortObject;
	GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_Lighted | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_TestBackFace | GDI_Cul_DM_ReceiveDynSdw);
	GDI_gpst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_UseRLI;

#if defined(_XBOX) || defined(ACTIVE_EDITORS) || defined(_PC_RETAIL) || defined(PCWIN_TOOL) || defined(_XENON)

	/* camera iso taille en pixel */
	{
		/*~~~~~~~~~~~~~~~~~~~*/
		CAM_tdst_Camera st_Cam;
		/*~~~~~~~~~~~~~~~~~~~*/

		L_memcpy(&st_Cam, &GDI_gpst_CurDD->st_Camera, sizeof(CAM_tdst_Camera));
		st_Cam.ul_Flags = CAM_Cul_Flags_Perspective | CAM_Cul_Flags_Ortho;
		if(_b_InvertY) st_Cam.ul_Flags |= CAM_Cul_Flags_OrthoYInvert;
		GDI_gpst_CurDD->st_GDI.pfnv_SetProjectionMatrix(&st_Cam);
	}

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_RestoreDisplay(void)
{
	GDI_gpst_CurDD->ul_DisplayInfo = sul_SaveDisplayInfo;
	GDI_gpst_CurDD->ul_CurrentDrawMask = sul_SaveDrawMask;

#if defined(_XBOX) || defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL) || defined(_PC_RETAIL) || defined(_XENON)
	GDI_gpst_CurDD->st_GDI.pfnv_SetProjectionMatrix(&GDI_gpst_CurDD->st_Camera);
#endif
}

/* Editors : display invisible object */
#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_EditorsRender(OBJ_tdst_GameObject *_pst_Node)
{
	/*~~~~~~~*/
	ULONG	DM;
	/*~~~~~~~*/

	if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible)
	{
		/*~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	*M;
		ULONG				C;
		int					i;
		/*~~~~~~~~~~~~~~~~~~~*/

		i = _pst_Node->ul_InvisibleObjectIndex;
		if((i < 0) || (i > GEO_DebugObject_LastInvisible - GEO_DebugObject_Invisible)) i = 0;

		if( GDI_WPShowStatusOn( GDI_gpst_CurDD, i ) ) 
		{
			C = (_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected) ? 1 : 0;
			C = GEO_ul_DebugObject_GetColor(GEO_DebugObject_Invisible, C);
			GEO_DebugObject_SetLight(C);
			M = OBJ_pst_GetAbsoluteMatrix(_pst_Node);
			DM = GDI_Cul_DM_All - GDI_Cul_DM_UseAmbient - GDI_Cul_DM_Fogged - GDI_Cul_DM_MaterialColor;
            GEO_DebugObject_Draw(GDI_gpst_CurDD, GEO_DebugObject_Invisible + i, DM, _pst_Node->ul_EditorFlags, M);
		}
	}
}

#else
#define STR_EditorsRender(a)

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_ComputeFrame(STR_tdst_Struct *_pst_STR, int _i_Index, STR_tdst_String *_pst_S)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Min, st_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*(LONG *) &st_Min.x = -1;

	if(_pst_S->uw_Flags & STR_Cuw_SF_AdaptFrameToText)
	{
		STR_GetRectShort(_pst_STR, _i_Index, 0, -1, &st_Min, &st_Max);

		/*
		 * _pst_S->st_Frame.x[0] = st_Min.x / _pst_STR->uw_SW; _pst_S->st_Frame.y[0] =
		 * st_Min.y / _pst_STR->uw_SH; _pst_S->st_Frame.x[1] = st_Max.x / _pst_STR->uw_SW;
		 * _pst_S->st_Frame.y[1] = st_Max.y / _pst_STR->uw_SH;
		 */
		_pst_S->st_Frame.x[0] = (SHORT) st_Min.x;
		_pst_S->st_Frame.y[0] = (SHORT) st_Min.y;
		_pst_S->st_Frame.x[1] = (SHORT) st_Max.x;
		_pst_S->st_Frame.y[1] = (SHORT) st_Max.y;
	}

	if(_pst_S->uw_Flags & STR_Cuw_SF_ShowFrame)
	{
		if((_pst_S->uw_Flags & STR_Cuw_SF_XJustifyMask) == STR_Cuw_SF_CenterWidth)
			STR_w_XFramePivot -= (_pst_S->st_Frame.x[0] + _pst_S->st_Frame.x[1]) / 2;
		else if((_pst_S->uw_Flags & STR_Cuw_SF_XJustifyMask) == STR_Cuw_SF_Right)
			STR_w_XFramePivot -= (_pst_S->st_Frame.x[1] - _pst_S->st_Frame.x[0]);

		if((_pst_S->uw_Flags & STR_Cuw_SF_YJustifyMask) == STR_Cuw_SF_CenterHeight)
			STR_w_YFramePivot -= (_pst_S->st_Frame.y[0] + _pst_S->st_Frame.y[1]) / 2;
		else if((_pst_S->uw_Flags & STR_Cuw_SF_YJustifyMask) == STR_Cuw_SF_Bottom)
			STR_w_YFramePivot -= (_pst_S->st_Frame.y[1] - _pst_S->st_Frame.y[0]);
	}

	if(_pst_S->uw_Flags & (STR_Cuw_SF_XJustifyMask | STR_Cuw_SF_YJustifyMask))
	{
		if(*(LONG *) &st_Min.x == -1) STR_GetRectShort(_pst_STR, _i_Index, 0, -1, &st_Min, &st_Max);

		if((_pst_S->uw_Flags & STR_Cuw_SF_XJustifyMask) == STR_Cuw_SF_CenterWidth)
			STR_w_XPivot -= (short) (st_Min.x + st_Max.x) / 2;
		else if((_pst_S->uw_Flags & STR_Cuw_SF_XJustifyMask) == STR_Cuw_SF_Right)
			STR_w_XPivot -= (short) (st_Max.x - st_Min.x);

		if((_pst_S->uw_Flags & STR_Cuw_SF_YJustifyMask) == STR_Cuw_SF_CenterHeight)
			STR_w_YPivot -= (short) (st_Min.y + st_Max.y) / 2;
		else if((_pst_S->uw_Flags & STR_Cuw_SF_YJustifyMask) == STR_Cuw_SF_Bottom)
			STR_w_YPivot -= (short) (st_Max.y - st_Min.y);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_AddSprite
(
	int						i,
	short					X1,
	short					Y1,
	short					X2,
	short					Y2,
	float					z,
	ULONG					color,
	STR_tdst_FontLetterDesc *pst_FL,
	int						i_Elem
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									index;
	GEO_Vertex							*pst_Vertex;
	ULONG								*pul_RLI;
	GEO_tdst_UV							*pst_UV;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Triangle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	index = i * 4;
	pst_Vertex = GFX_gpst_Geo->dst_Point + index;

	STR_M_RecalageSprite(X1, Y1, X2, Y2);

	pst_Vertex[0].x = pst_Vertex[1].x = (float) X1;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y1;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X2;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y2;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = z;
	STR_TransformPoints(pst_Vertex);

	pul_RLI = GFX_gpst_Geo->dul_PointColors + index + 1;

	pul_RLI[0] = color;
	pul_RLI[1] = color;
	pul_RLI[2] = color;
	pul_RLI[3] = color;

	pst_UV = GFX_gpst_Geo->dst_UV + index;

	pst_UV[0].fU = pst_UV[1].fU = pst_FL->f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = pst_FL->f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = pst_FL->f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = pst_FL->f_V[1];

	pst_Element = GFX_gpst_Geo->dst_Element + i_Elem;
	pst_Triangle = pst_Element->dst_Triangle + pst_Element->l_NbTriangles;
	pst_Element->l_NbTriangles += 2;

	// rotate CCW
	if (pst_FL->f_U[0] > 2.0 )
	{
		pst_Triangle[0].auw_UV[0] = index + 3;
		pst_Triangle[0].auw_UV[1] = index + 0;
		pst_Triangle[0].auw_UV[2] = index + 1;
		pst_Triangle[1].auw_UV[0] = index + 3;
		pst_Triangle[1].auw_UV[1] = index + 1;
		pst_Triangle[1].auw_UV[2] = index + 2;
	}
	// rotate CW
	else if (pst_FL->f_U[0] > 1.0 )
	{
		pst_Triangle[0].auw_UV[0] = index + 1;
		pst_Triangle[0].auw_UV[1] = index + 2;
		pst_Triangle[0].auw_UV[2] = index + 3;
		pst_Triangle[1].auw_UV[0] = index + 1;
		pst_Triangle[1].auw_UV[1] = index + 3;
		pst_Triangle[1].auw_UV[2] = index + 0;
	}
	else
	{
		pst_Triangle[0].auw_UV[0] = index;
		pst_Triangle[0].auw_UV[1] = index + 1;
		pst_Triangle[0].auw_UV[2] = index + 2;
		pst_Triangle[1].auw_UV[0] = index;
		pst_Triangle[1].auw_UV[1] = index + 2;
		pst_Triangle[1].auw_UV[2] = index + 3;
	}
	pst_Triangle[0].auw_Index[0] = index;
	pst_Triangle[0].auw_Index[1] = index + 1;
	pst_Triangle[0].auw_Index[2] = index + 2;
	pst_Triangle[1].auw_Index[0] = index;
	pst_Triangle[1].auw_Index[1] = index + 2;
	pst_Triangle[1].auw_Index[2] = index + 3;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_AddFrame
(
	int						i,
	short					X0,
	short					Y0,
	short					X1,
	short					Y1,
	short					X2,
	short					Y2,
	short					X3,
	short					Y3,
	float					z,
	ULONG					color,
	STR_tdst_FontLetterDesc *pst_FL,
	int						i_Elem
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									index, loop;
	GEO_Vertex							*pst_Vertex;
	ULONG								*pul_RLI;
	GEO_tdst_UV							*pst_UV;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Triangle;
	STR_tdst_FontLetterDesc				st_FLFrame;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	STR_M_RecalageFrame(X0, Y0, X1, Y1, X2, Y2, X3, Y3);

	index = i * 4;
	pst_Vertex = GFX_gpst_Geo->dst_Point + index;

	pst_Vertex[0].x = pst_Vertex[1].x = (float) X0;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y0;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X1;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y1;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = z;
	STR_TransformPoints(pst_Vertex);
	pst_Vertex += 4;

	pst_Vertex[0].x = pst_Vertex[1].x = (float) X0;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y1;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X1;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y2;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = z;
	STR_TransformPoints(pst_Vertex);
	pst_Vertex += 4;

	pst_Vertex[0].x = pst_Vertex[1].x = (float) X0;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y2;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X1;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y3;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = z;
	STR_TransformPoints(pst_Vertex);
	pst_Vertex += 4;

	pst_Vertex[0].x = pst_Vertex[1].x = (float) X1;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y0;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X2;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y1;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = z;
	STR_TransformPoints(pst_Vertex);
	pst_Vertex += 4;

	pst_Vertex[0].x = pst_Vertex[1].x = (float) X1;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y1;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X2;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y2;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = z;
	STR_TransformPoints(pst_Vertex);
	pst_Vertex += 4;

	pst_Vertex[0].x = pst_Vertex[1].x = (float) X1;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y2;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X2;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y3;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = z;
	STR_TransformPoints(pst_Vertex);
	pst_Vertex += 4;

	pst_Vertex[0].x = pst_Vertex[1].x = (float) X2;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y0;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X3;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y1;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = z;
	STR_TransformPoints(pst_Vertex);
	pst_Vertex += 4;

	pst_Vertex[0].x = pst_Vertex[1].x = (float) X2;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y1;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X3;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y2;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = z;
	STR_TransformPoints(pst_Vertex);
	pst_Vertex += 4;

	pst_Vertex[0].x = pst_Vertex[1].x = (float) X2;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y2;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X3;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y3;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = z;
	STR_TransformPoints(pst_Vertex);
	pst_Vertex += 4;

	pul_RLI = GFX_gpst_Geo->dul_PointColors + index + 1;
	for(loop = 0; loop < 36; loop++) pul_RLI[loop] = color;

	pst_UV = GFX_gpst_Geo->dst_UV + index;

	st_FLFrame.f_U[0] = pst_FL->f_U[0];
	st_FLFrame.f_U[1] = (pst_FL->f_U[0] + pst_FL->f_U[1]) / 2;
	st_FLFrame.f_V[0] = (pst_FL->f_V[0] + pst_FL->f_V[1]) / 2;
	st_FLFrame.f_V[1] = pst_FL->f_V[1];
	pst_UV[0].fU = pst_UV[1].fU = st_FLFrame.f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = st_FLFrame.f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = st_FLFrame.f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = st_FLFrame.f_V[1];
	pst_UV += 4;

	st_FLFrame.f_V[1] = st_FLFrame.f_V[0];
	pst_UV[0].fU = pst_UV[1].fU = st_FLFrame.f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = st_FLFrame.f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = st_FLFrame.f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = st_FLFrame.f_V[1];
	pst_UV += 4;

	st_FLFrame.f_V[0] = pst_FL->f_V[0];
	pst_UV[0].fU = pst_UV[1].fU = st_FLFrame.f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = st_FLFrame.f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = st_FLFrame.f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = st_FLFrame.f_V[1];
	pst_UV += 4;

	st_FLFrame.f_U[0] = st_FLFrame.f_U[1];
	st_FLFrame.f_V[0] = st_FLFrame.f_V[1];
	st_FLFrame.f_V[1] = pst_FL->f_V[1];
	pst_UV[0].fU = pst_UV[1].fU = st_FLFrame.f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = st_FLFrame.f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = st_FLFrame.f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = st_FLFrame.f_V[1];
	pst_UV += 4;

	st_FLFrame.f_V[1] = st_FLFrame.f_V[0];
	pst_UV[0].fU = pst_UV[1].fU = st_FLFrame.f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = st_FLFrame.f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = st_FLFrame.f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = st_FLFrame.f_V[1];
	pst_UV += 4;

	st_FLFrame.f_V[0] = pst_FL->f_V[0];
	pst_UV[0].fU = pst_UV[1].fU = st_FLFrame.f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = st_FLFrame.f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = st_FLFrame.f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = st_FLFrame.f_V[1];
	pst_UV += 4;

	st_FLFrame.f_U[1] = pst_FL->f_U[1];
	st_FLFrame.f_V[0] = st_FLFrame.f_V[1];
	st_FLFrame.f_V[1] = pst_FL->f_V[1];
	pst_UV[0].fU = pst_UV[1].fU = st_FLFrame.f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = st_FLFrame.f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = st_FLFrame.f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = st_FLFrame.f_V[1];
	pst_UV += 4;

	st_FLFrame.f_V[1] = st_FLFrame.f_V[0];
	pst_UV[0].fU = pst_UV[1].fU = st_FLFrame.f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = st_FLFrame.f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = st_FLFrame.f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = st_FLFrame.f_V[1];
	pst_UV += 4;

	st_FLFrame.f_V[0] = pst_FL->f_V[0];
	pst_UV[0].fU = pst_UV[1].fU = st_FLFrame.f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = st_FLFrame.f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = st_FLFrame.f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = st_FLFrame.f_V[1];
	pst_UV += 4;

	pst_Element = GFX_gpst_Geo->dst_Element + i_Elem;
	pst_Triangle = pst_Element->dst_Triangle + pst_Element->l_NbTriangles;
	pst_Element->l_NbTriangles += 18;

	for(loop = 0; loop < 9; loop++)
	{
		pst_Triangle[0].auw_Index[0] = index;
		pst_Triangle[0].auw_Index[1] = index + 1;
		pst_Triangle[0].auw_Index[2] = index + 2;
		pst_Triangle[0].auw_UV[0] = index;
		pst_Triangle[0].auw_UV[1] = index + 1;
		pst_Triangle[0].auw_UV[2] = index + 2;
		pst_Triangle[1].auw_Index[0] = index;
		pst_Triangle[1].auw_Index[1] = index + 2;
		pst_Triangle[1].auw_Index[2] = index + 3;
		pst_Triangle[1].auw_UV[0] = index;
		pst_Triangle[1].auw_UV[1] = index + 2;
		pst_Triangle[1].auw_UV[2] = index + 3;
		index += 4;
		pst_Triangle += 2;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_AddSprite_1C(int i, short X1, short Y1, short X2, short Y2, ULONG Color, STR_tdst_FontLetterDesc *pst_FL)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int			index;
	GEO_Vertex	*pst_Vertex;
	ULONG		*pul_RLI;
	GEO_tdst_UV *pst_UV;
	/*~~~~~~~~~~~~~~~~~~~~*/

	index = i * 4;

	pst_Vertex = GFX_gpst_Geo->dst_Point + index;
	pst_Vertex[0].x = pst_Vertex[1].x = (float) X1;
	pst_Vertex[0].y = pst_Vertex[3].y = (float) Y1;
	pst_Vertex[2].x = pst_Vertex[3].x = (float) X2;
	pst_Vertex[1].y = pst_Vertex[2].y = (float) Y2;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = 0;

	pul_RLI = GFX_gpst_Geo->dul_PointColors + index + 1;
	pul_RLI[0] = pul_RLI[1] = pul_RLI[2] = pul_RLI[3] = Color;

	pst_UV = GFX_gpst_Geo->dst_UV + index;
	pst_UV[0].fU = pst_UV[1].fU = pst_FL->f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = pst_FL->f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = pst_FL->f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = pst_FL->f_V[1];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_AddSprite_F1C(int i, float x, float y, float w, float h, ULONG Color, STR_tdst_FontLetterDesc *pst_FL)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int			index;
	GEO_Vertex	*pst_Vertex;
	ULONG		*pul_RLI;
	GEO_tdst_UV *pst_UV;
	/*~~~~~~~~~~~~~~~~~~~~*/

	index = i * 4;

	pst_Vertex = GFX_gpst_Geo->dst_Point + index;
	pst_Vertex[0].x = pst_Vertex[1].x = x;
	pst_Vertex[0].y = pst_Vertex[3].y = y;
	pst_Vertex[2].x = pst_Vertex[3].x = x + w;
	pst_Vertex[1].y = pst_Vertex[2].y = y + h;
	pst_Vertex[0].z = pst_Vertex[1].z = pst_Vertex[2].z = pst_Vertex[3].z = 0;

	pul_RLI = GFX_gpst_Geo->dul_PointColors + index + 1;
	pul_RLI[0] = pul_RLI[1] = pul_RLI[2] = pul_RLI[3] = Color;

	pst_UV = GFX_gpst_Geo->dst_UV + index;
	pst_UV[0].fU = pst_UV[1].fU = pst_FL->f_U[0];
	pst_UV[2].fU = pst_UV[3].fU = pst_FL->f_U[1];
	pst_UV[1].fV = pst_UV[2].fV = pst_FL->f_V[0];
	pst_UV[0].fV = pst_UV[3].fV = pst_FL->f_V[1];
}

#ifdef GSP_PS2_BENCH
extern unsigned int NoSTR;
#endif

#ifdef ACTIVE_EDITORS 

#define M4Edit_STRRender_Vars\
	int	i_SaveSpeedDraw;

#define M4Edit_STRRender_BeforeDisplay\
	i_SaveSpeedDraw = LINK_gi_SpeedDraw;\
	LINK_gi_SpeedDraw = 0;

#define M4Edit_STRRender_AfterDisplay\
	LINK_gi_SpeedDraw = i_SaveSpeedDraw;

#else /* ACTIVE_EDITORS */

#define M4Edit_STRRender_Vars
#define M4Edit_STRRender_BeforeDisplay
#define M4Edit_STRRender_AfterDisplay

#endif /* ACTIVE_EDITORS */


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Render(OBJ_tdst_GameObject *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct			*pst_STR;
	STR_tdst_Letter			*pst_Letter, *pst_Last;
	int						i, i_Index, i_SubMat, i_Sprite, s;
	STR_tdst_String			*pst_S, *pst_SLast;
	short					X, Y, W, H, X0, Y0, X1, Y1, X2, Y2, X3, Y3;
	STR_tdst_FontLetterDesc *pst_FontLetter;
	ULONG					ul_SpriteFont[64], ul_Font[64], ul_Element[64], uc_FontToElement[64];
	MAT_tdst_Material		*pst_Mat;
#ifdef JADEFUSION
	static ULONG			ul_SubMatOrder[32] =
#else
	static ULONG			ul_SubMatOrder[24] =
#endif
	{
		8,
		9,
		13,
		16,
		10,
		11,
		12,
		14,
		15,
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		17,
		18,
		19,
		20,
		21,
		22,
		23
#ifdef JADEFUSION
		,24,
		25,
		26,
		27,
		28,
		29,
		30,
		31
#endif
	};
	M4Edit_STRRender_Vars
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if (!defined(ACTIVE_EDITORS) && !defined(PCWIN_TOOL) && !defined(_PC_RETAIL))
	{
#ifdef _XBOX
extern ULONG h_SaveWorldKey;
//		STR_f_YFactor = 1.0f / (1.0f + (Gx8Global_get(f2DFFY_A2D) - 1.0f) / 1.0f);
//		STR_f_YOffset = Gx8Global_get(Ysize) * (1.0f - STR_f_YFactor) / 1.8f;
// Exeption en carton pour le press start du menu a virer plus tard bien entendu
if (h_SaveWorldKey==0x3d00c456)
{
		STR_f_YFactor = 0.9f;//1.03//1.00f
		STR_f_YOffset = 26.0101f;//10.0101f
}
else
{
		STR_f_YFactor = 0.9f;//1.03//1.00f
		STR_f_YOffset = 28.0101f;//10.0101f
}
#endif
	}
#endif

#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoSTR) return;
#endif

#ifdef GX8_BENCH
	if(g_iNoSTR) return;
#endif /* GX8_BENCH */

	/*$2- Render an invisible object in editor mode ------------------------------------------------------------------*/

	STR_EditorsRender(_pst_Node);

	pst_STR = (STR_tdst_Struct *) _pst_Node->pst_Base->pst_Visu->pst_Object;
	if(pst_STR->ul_RenderingCounter == GDI_gpst_CurDD->ul_RenderingCounter) return;
	pst_STR->ul_RenderingCounter = GDI_gpst_CurDD->ul_RenderingCounter;

	pst_STR->uw_SW = (short) GDI_gpst_CurDD->st_Camera.f_Width;
	pst_STR->uw_SH = (short) GDI_gpst_CurDD->st_Camera.f_Height;

	if(!(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_RenderingInterface))
		GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_ForceSortTriangle;

	pst_Mat = (MAT_tdst_Material *) _pst_Node->pst_Base->pst_Visu->pst_Material;
	if(!pst_Mat) return;

	/* AI */
	if(_pst_Node->pst_Extended && _pst_Node->pst_Extended->pst_Ai)
	{
		AI_C_Callback = 1;
		AI_ExecCallback(_pst_Node, AI_C_Callback_BeforeDisplay);
		AI_C_Callback = 0;
	}

	STR_SetDisplay(FALSE);
	STR_M_ResetFont(pst_STR);
	STR_M_SetMaterial(_pst_Node);

	i_SubMat = (pst_Mat->st_Id.i->ul_Type == GRO_MaterialMulti) ? (((MAT_tdst_Multi *) pst_Mat)->l_NumberOfSubMaterials) : 1;
	if(i_SubMat > 64) i_SubMat = 64;
#ifdef JADEFUSION
	for(i = 0; i < 32; i++) ul_SpriteFont[i] = 0;
#else
	for(i = 0; i < 24; i++) ul_SpriteFont[i] = 0;
#endif
	/* numéro du sprite associé à une lettre */
	i_Sprite = 0;

	/*
	 * on va utiliser le champ specular color du computing buffer pour stocker le
	 * numéro de sprite correspondant à une lettre
	 */
	pst_STR->pul_Sprite = GDI_gpst_CurDD->pst_ComputingBuffers->aul_Specular;

	i_Sprite += STR_l_Effect_PreTreatAll(pst_STR);

	/* get used index to init geometric object */
	pst_S = pst_STR->dst_String;
	pst_SLast = pst_S + pst_STR->uc_NbMaxStrings;
	for(i_Index = 0; pst_S < pst_SLast; pst_S++, i_Index++)
	{
		if(pst_S->uw_Flags & STR_Cuw_SF_Hide) continue;

		pst_Letter = pst_STR->dst_Letter + pst_S->uw_First + pst_S->uw_SubFirst;
		pst_Last = pst_Letter + pst_S->uw_SubNumber;
		if(pst_Last <= pst_Letter) continue;

		if(pst_S->uw_Flags & STR_Cuw_SF_ShowFrame)
		{
			i = ((pst_S->st_Frame.ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift) % i_SubMat;
			ul_SpriteFont[i] += 9;
			i_Sprite += 9;
		}

		for(; pst_Letter < pst_Last; pst_Letter++)
		{
			/* i = ((pst_Letter->ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift); */
			i = ((pst_Letter->ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift) % i_SubMat;
			ul_SpriteFont[i] += 1 + pst_STR->pul_Sprite[pst_Letter - pst_STR->dst_Letter];
			i_Sprite++;
		}
	}

	i_Index = 0;
#ifdef JADEFUSION
	for(i = 0; i < 32; i++)
#else
	for(i = 0; i < 24; i++)
#endif
	{
		if(ul_SpriteFont[ul_SubMatOrder[i]])
		{
			STR_M_SetFont(pst_STR, ul_SubMatOrder[i]);
			if(pst_STR->pst_Font)
			{
				uc_FontToElement[ul_SubMatOrder[i]] = i_Index;
				ul_Element[i_Index++] = ul_SubMatOrder[i];
			}
			else
			{
				i_Sprite -= ul_SpriteFont[ul_SubMatOrder[i]];
			}

			ul_Font[ul_SubMatOrder[i]] = (ULONG) pst_STR->pst_Font;
		}
		else
			ul_Font[ul_SubMatOrder[i]] = 0;
	}

	if(i_Sprite)
	{
		GFX_NeedGeomEx(i_Sprite * 4, i_Sprite * 4, i_Index, 1, TRUE);

		for(i = 0; i < i_Index; i++)
		{
			GFX_NeedGeomExElem(i, ul_SpriteFont[ul_Element[i]] * 2);
			GFX_gpst_Geo->dst_Element[i].l_NbTriangles = 0;
#ifdef _XENON
			if (ul_Element[i] < 2 && XGetLanguage( ) == XC_LANGUAGE_JAPANESE && INO_gai_PresentLanguage[INO_e_Japanese] == 1)
			{
				// offset id by 22 for japanese as this is the position of the first japanese entry in the multi-material
				GFX_gpst_Geo->dst_Element[i].l_MaterialId = ul_Element[i] + 22;
			}
			else
#endif
			GFX_gpst_Geo->dst_Element[i].l_MaterialId = ul_Element[i];
		}

		i_Sprite = 0;
		pst_S = pst_STR->dst_String;
		pst_SLast = pst_S + pst_STR->uc_NbMaxStrings;
		for(i_Index = 0; pst_S < pst_SLast; pst_S++, i_Index++)
		{
			if(pst_S->uw_Flags & STR_Cuw_SF_Hide) continue;

			STR_w_XFramePivot = STR_w_XPivot = (short) (pst_S->st_Pivot.x * pst_STR->uw_SW);
			STR_w_YFramePivot = STR_w_YPivot = (short) (pst_S->st_Pivot.y * pst_STR->uw_SH);

			pst_Letter = pst_STR->dst_Letter + pst_S->uw_First + pst_S->uw_SubFirst;
			pst_Last = pst_Letter + pst_S->uw_SubNumber;
			if(pst_Last <= pst_Letter) continue;

			STR_ComputeFrame(pst_STR, i_Index, pst_S);

			if(pst_S->uw_Flags & STR_Cuw_SF_ShowFrame)
			{
				i = ((pst_S->st_Frame.ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift) % i_SubMat;

				pst_STR->pst_Font = (STR_tdst_FontDesc *) ul_Font[i];
				if(!pst_STR->pst_Font) continue;
				pst_FontLetter = pst_STR->pst_Font->pst_Letter + STR_M_A2I(pst_S->st_Frame.ul_Flags);

				pst_S->st_Frame.ul_Sprite = i_Sprite;

				X1 = pst_S->st_Frame.x[0] + STR_w_XFramePivot;
				Y1 = pst_STR->uw_SH - (pst_S->st_Frame.y[0] + STR_w_YFramePivot);
				X2 = pst_S->st_Frame.x[1] + STR_w_XFramePivot;
				Y2 = pst_STR->uw_SH - (pst_S->st_Frame.y[1] + STR_w_YFramePivot);

				X0 = X1 - pst_S->st_Frame.borderx;
				Y0 = Y1 + pst_S->st_Frame.bordery;
				X3 = X2 + pst_S->st_Frame.borderx;
				Y3 = Y2 - pst_S->st_Frame.bordery;

				STR_AddFrame
				(
					i_Sprite,
					X0,
					Y0,
					X1,
					Y1,
					X2,
					Y2,
					X3,
					Y3,
					0,
					pst_S->st_Frame.ul_Color,
					pst_FontLetter,
					uc_FontToElement[i]
				);
				i_Sprite += 9;
			}

			if(pst_S->uw_SubFirst != 0)
			{
				STR_w_XPivot += pst_STR->dst_Letter[pst_S->uw_First].x - pst_Letter->x;
				STR_w_YPivot += pst_STR->dst_Letter[pst_S->uw_First].y - pst_Letter->y;
			}

			for(; pst_Letter < pst_Last; pst_Letter++)
			{
				i = ((pst_Letter->ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift) % i_SubMat;

				pst_STR->pst_Font = (STR_tdst_FontDesc *) ul_Font[i];
				pst_STR->w_FontPage = i;
				if(!pst_STR->pst_Font) continue;

				pst_FontLetter = pst_STR->pst_Font->pst_Letter + STR_M_A2I(pst_Letter->ul_Flags);

				X = pst_Letter->x;
				Y = pst_Letter->y;
				W = pst_Letter->w;
				H = pst_Letter->h;

				if(pst_Letter->ul_Flags & STR_Cul_LF_CenterX)
					STR_w_LetterDX = W >> 1;
				else if(pst_Letter->ul_Flags & STR_Cul_LF_RightX)
					STR_w_LetterDX = W;
				else
					STR_w_LetterDX = 0;

				if(pst_Letter->ul_Flags & STR_Cul_LF_CenterY)
					STR_w_LetterDY = H >> 1;
				else if(pst_Letter->ul_Flags & STR_Cul_LF_BottomY)
					STR_w_LetterDY = H;
				else
					STR_w_LetterDY = 0;

				Y1 = pst_STR->uw_SH - (Y + STR_w_YPivot - STR_w_LetterDY);
				Y2 = Y1 - H;
				X1 = X + STR_w_XPivot - STR_w_LetterDX;
				X2 = X1 + W;
				STR_AddSprite(i_Sprite, X1, Y1, X2, Y2, 0, pst_Letter->ul_Color, pst_FontLetter, uc_FontToElement[i]);

				s = pst_STR->pul_Sprite[pst_Letter - pst_STR->dst_Letter];
				pst_STR->pul_Sprite[pst_Letter - pst_STR->dst_Letter] = i_Sprite++;
				while(s)
				{
					STR_AddSprite
					(
						i_Sprite,
						X1,
						Y1,
						X2,
						Y2,
						0,
						pst_Letter->ul_Color,
						pst_FontLetter,
						uc_FontToElement[i]
					);
					i_Sprite++;
					s--;
				}
			}
		}

		STR_Effect_TreatAll(pst_STR);

		M_GFX_CheckGeom();

		STR_sgpst_GO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) GFX_gpst_Geo;

		M4Edit_STRRender_BeforeDisplay
		GFX_gpst_Geo->st_Id.i->pfn_Render(STR_sgpst_GO);
		M4Edit_STRRender_AfterDisplay
	}

	/* AI */
	if(_pst_Node->pst_Extended && _pst_Node->pst_Extended->pst_Ai)
		AI_ExecCallback(_pst_Node, AI_C_Callback_AfterDisplay);

	STR_RestoreDisplay();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
 void STRDATA_ComputeRect( int i, GEO_Vertex *V, float *pf_XMin, float *pf_XMax, float *pf_YMin, float *pf_YMax )
 {
	*pf_XMin = *pf_YMin = 10000.0f;
	*pf_XMax = *pf_YMax = -10000.0f;
	while(i--)
	{
		if(V->x < *pf_XMin) *pf_XMin = V->x;
		if(V->x > *pf_XMax) *pf_XMax = V->x;
		if(V->y < *pf_YMin) *pf_YMin = V->y;
		if(V->y > *pf_YMax) *pf_YMax = V->y;
		V++;
	}
}
			
/**/
void STRDATA_Render(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define MAX_SPRITE	512
	STR_tdst_OneString			*S, *SLast;
	int							i, i_Index, i_SubMat, i_Sprite, s, i_Font, i_NextFont;
	STR_tdst_FontLetterDesc		*pst_FontLetter;
	ULONG						ul_SpriteFont[32], ul_Font[32];
	MAT_tdst_Material			*pst_Mat;
	GEO_tdst_IndexedTriangle	*T;
	unsigned char				*pc;
	STR_tdst_FontDesc			*pst_Font;
	float						f_PosX, f_PosY;
	float						f_SizeX, f_SizeY, fX, fY, fXMin, fXMax, fYMin, fYMax;
	ULONG						ul_Color;
	int							i_Ascii;
	int							i_StringFirstSprite, i_FixedWidth;
	GEO_Vertex					*V;
	static ULONG				FC = 0xFFFFFFFF, FI = 0x90022;
	ULONG						ul_FrameColor, ul_FrameIcon;
	float						f_FrameBorder;
	BOOL						b_HasFrame, b_RectComputed;
	int							i_Align;
	int							ai_SubFontElem[ 32 ] = {9,0,1,2,3,4,5,6,7,8,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
	float						f_XMul;
	int							i_Unicode;
#ifdef ACTIVE_EDITORS
	int							i_SaveSpeedDraw;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoSTR) return;
#endif
#ifdef GX8_BENCH
	if(g_iNoSTR) return;
#endif /* GX8_BENCH */

	/* update number of letters */
	STRDATA_DefragBuffer();
	if(STR_gst_Data.uw_NbString == 0) return;

	/* init mat */
	if(!STR_gst_Data.pst_Material) return;
	pst_Mat = (MAT_tdst_Material *) STR_gst_Data.pst_Material;
	STR_sgpst_GO->pst_Base->pst_Visu->pst_Material = (GRO_tdst_Struct *) pst_Mat;
	i_SubMat = (pst_Mat->st_Id.i->ul_Type == GRO_MaterialMulti) ? (((MAT_tdst_Multi *) pst_Mat)->l_NumberOfSubMaterials) : 1;
	if(i_SubMat > 32) return;
	for(i = 0; i < 32; i++)
	{
		ul_SpriteFont[i] = 0;
		ul_Font[i] = (ULONG) STR_pst_GetFontDesc(STR_gst_Data.pst_Material, i);
	}

	/* init display */
	STR_gst_Data.uw_SW = (short) GDI_gpst_CurDD->st_Camera.f_Width;
	STR_gst_Data.uw_SH = (short) GDI_gpst_CurDD->st_Camera.f_Height;
	
	if ( (GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst <= 0) || (GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst >= GDI_Cul_SRC_Number) ) 
		f_XMul = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
	else
		f_XMul = GDI_gaf_ScreenRation[ GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst ];
		
	f_XMul *= STR_gst_Data.f_XMul;
	STR_SetDisplay(TRUE);
	
	/*
	 * champ de sockage pour les infos des sprites£
	 * les 24 premiers bits indiquent numéro de lettre -> numéro du sprite correspondant£
	 * les 8 derniers bits indiquent numéro de sprite -> numéro de material £
	 */
	STR_gst_Data.pul_Sprite = GDI_gpst_CurDD->pst_ComputingBuffers->aul_Specular;

	/* alloue les données nécessaires */
	GFX_NeedGeomEx(4 * MAX_SPRITE, 4 * MAX_SPRITE, 32, 1, TRUE);

	/* Calcule les coordonnées de tous les sprites utilisés par les chaines */
	i_Sprite = 0;
	S = STR_gst_Data.pst_String;
	SLast = S + STR_gst_Data.uw_NbString;

	for(i_Index = 0; S < SLast; S++, i_Index++)
	{
STRRENDER_ContinueStringAfterDraw:
		if (S->uw_Nb <= 0) S->ul_Flags &= ~STR_String_Used;
		if (!(S->ul_Flags & STR_String_Used)) continue;
		if (S->ul_Flags & STR_String_Hide) continue;
		if (!((1 << S->c_Group) & STR_gst_Data.ul_Group_Display) ) continue;

		/* init char loop */
		pc = (unsigned char*)STR_gst_Data.pc_Letter + S->uw_First;
		i_Font = i_NextFont = 0;
		f_SizeX = STR_gst_Data.f_SizeX;
		f_SizeY = STR_gst_Data.f_SizeY;
		f_PosX = S->st_Pivot.x;
		f_PosY = S->st_Pivot.y;
		ul_Color = 0xFFFFFFFF;
		i_StringFirstSprite = i_Sprite;
		b_HasFrame = 0;
		i_Align = 0;
		ul_FrameColor = 0xFFFFFFFF;
		ul_FrameIcon = 0x90022;
		f_FrameBorder = 0.01f;
		i_FixedWidth = (S->ul_Flags & STR_String_FixedWidth) ? 1 : 0;
		i_Unicode = 0;
		
		while(*pc != 0)
		{
			i_Ascii = 0;
			if(*pc == '\\')
			{
				pc += STR_l_ReadFormat((char*)pc);
				switch(STR_sgst_Format.i_Format)
				{
				case STR_Cul_Format_EOL:
					f_PosY += f_SizeY;
					f_PosX = S->st_Pivot.x;
					break;
				case STR_Cul_Format_PageInst:
					i_Font = STR_sgst_Format.i_Param;
					break;
				case STR_Cul_Format_Page:
					i_Font = i_NextFont = STR_sgst_Format.i_Param;
					break;
				case STR_Cul_Format_W:
					f_SizeX = STR_sgst_Format.f_Param;
					if( f_SizeX )
						i_FixedWidth = 1;
					else
						i_FixedWidth = 0;
					break;
				case STR_Cul_Format_H:
					f_SizeY = STR_sgst_Format.f_Param;
					break;
				case STR_Cul_Format_X:
					f_PosX = STR_sgst_Format.f_Param;
					break;
				case STR_Cul_Format_Y:
					f_PosY = STR_sgst_Format.f_Param;
					break;
				case STR_Cul_Format_Color:
					ul_Color = STR_sgst_Format.i_Param;
					break;
				case STR_Cul_Format_Ascii:
					i_Ascii = STR_sgst_Format.i_Param;
					break;
				case STR_Cul_Format_Justify:
					S->ul_Flags &= ~STR_String_JustFlags;
					S->ul_Flags |= STR_sgst_Format.i_Param;
					break;
				case STR_Cul_Format_Tab:
					f_PosX -= STR_gst_Data.f_SizeX;
					f_PosX = ((float) ((int) ((f_PosX + 0.1f) * 10.0f))) / 10.0f;
					f_PosX += STR_gst_Data.f_SizeX;
					break;
				case STR_Cul_Format_TabGlobal:
					f_PosX = ((float) ((int) ((f_PosX + 0.1f) * 10.0f))) / 10.0f;
					break;
				case STR_Cul_Format_Frame:
					b_HasFrame = 1;
					break;
				case STR_Cul_Format_FrameColor:
					b_HasFrame = 1;
					ul_FrameColor = STR_sgst_Format.i_Param;
					break;
				case STR_Cul_Format_FrameIcon:
					b_HasFrame = 1;
					ul_FrameIcon = STR_sgst_Format.i_Param;
					break;
				case STR_Cul_Format_FrameBorder:
					b_HasFrame = 1;
					f_FrameBorder = STR_sgst_Format.f_Param;
					break;
				case STR_Cul_Format_Align:
					i_Align = STR_sgst_Format.i_Param;
					break;
				case STR_Cul_Format_Unicode:
					i_Unicode = 1;
				}
			}
			else if (*pc == '\n')
			{
				f_PosY += f_SizeY + STR_gst_Data.f_YEcart;
				f_PosX = S->st_Pivot.x;
				pc++;
			}
			else if (*pc >= 32)
			{
				if ( i_Unicode )
				{
					i_Ascii = ((*pc - 32) * 200) + (pc[ 1 ] - 32);
					pc += 2;
				}
				else
				{
					i_Ascii = *pc;
					pc++;
				}
			}
			else
				pc++;

			if(i_Ascii)
			{
				ul_SpriteFont[i_Font]++;
				pst_Font = (STR_tdst_FontDesc *) ul_Font[i_Font];
				pst_FontLetter = pst_Font->pst_Letter + (i_Ascii - 32);
				if( i_FixedWidth )
					fX = f_SizeX;
				else
				{
					fX = (pst_FontLetter->f_V[1] - pst_FontLetter->f_V[0]);
					if(fX == 0)
						fX = 0;
					else
					{
						fX = f_XMul * pst_Font->fWoH * f_SizeY * (pst_FontLetter->f_U[1] - pst_FontLetter->f_U[0]) / fX;
						(*(ULONG *) &fX) &= 0x7FFFFFFF;
					}
				}

				STR_AddSprite_F1C(i_Sprite, f_PosX, f_PosY, fX, f_SizeY, ul_Color, pst_FontLetter);
				f_PosX += fX + STR_gst_Data.f_XEcart;

				STR_gst_Data.pul_Sprite[pc - (unsigned char*)STR_gst_Data.pc_Letter] = (STR_gst_Data.pul_Sprite[pc - (unsigned char*)STR_gst_Data.pc_Letter] & 0xFF000000) | i_Sprite;
				STR_gst_Data.pul_Sprite[i_Sprite] = (STR_gst_Data.pul_Sprite[i_Sprite] & 0xFFFFFF) | (i_Font << 24);
				i_Sprite++;
				i_Font = i_NextFont;

				if(i_Sprite == MAX_SPRITE)
				{
					i_Sprite = i_StringFirstSprite;
					goto STRRENDER_DrawString;
				}
			}
		}
		
		/* manage display timer */
		if(S->f_Timer != -1)
		{
#ifdef ACTIVE_EDITORS
			if (ENG_gb_EngineRunning) 
#endif
				S->f_Timer -= TIM_gf_dt;
			if(S->f_Timer < 0) S->ul_Flags &= ~STR_String_Used;
		}

		/* justification */
		b_RectComputed = 0;
		if(S->ul_Flags & STR_String_JustFlags)
		{
			i = (i_Sprite - i_StringFirstSprite) << 2;
			V = GFX_gpst_Geo->dst_Point + (i_StringFirstSprite << 2);
			STRDATA_ComputeRect( i, V, &fXMin, &fXMax, &fYMin, &fYMax );
			b_RectComputed = 1;

			if(S->ul_Flags & STR_String_JustPivotXCenter)
				fX = (fXMax - fXMin) / 2;
			else if(S->ul_Flags & STR_String_JustPivotXRight)
				fX = fXMax - S->st_Pivot.x;
			else
				fX = 0;

			if(S->ul_Flags & STR_String_JustPivotYCenter)
				fY = (fYMax - fYMin) / 2;
			else if(S->ul_Flags & STR_String_JustPivotYBottom)
				fY = fYMax - S->st_Pivot.y;
			else
				fY = 0;

			i = (i_Sprite - i_StringFirstSprite) << 2;
			V = GFX_gpst_Geo->dst_Point + (i_StringFirstSprite << 2);
			while(i--)
			{
				V->x -= fX;
				V->y -= fY;
				V++;
			}
			
			fXMin -= fX;
			fXMax -= fX;
			fYMin -= fY;
			fYMax -= fY;
		}
		
		/* rajout d'une frame */
		if ( b_HasFrame )
		{
			if (!b_RectComputed)
			{
				i = (i_Sprite - i_StringFirstSprite) << 2;
				V = GFX_gpst_Geo->dst_Point + (i_StringFirstSprite << 2);
				STRDATA_ComputeRect( i, V, &fXMin, &fXMax, &fYMin, &fYMax );
				b_RectComputed = 1;
			}
			
			fXMin -= f_FrameBorder;
			fXMax += f_FrameBorder;
			fYMin -= f_FrameBorder;
			fYMax += f_FrameBorder;
			
			ul_SpriteFont[ ul_FrameIcon >> 16 ]++;
			pst_Font = (STR_tdst_FontDesc *) ul_Font[ ul_FrameIcon >> 16 ];
			pst_FontLetter = pst_Font->pst_Letter + ((ul_FrameIcon & 0xffff) - 32);
			STR_AddSprite_F1C(i_Sprite, fXMin, fYMin, fXMax - fXMin, fYMax - fYMin, ul_FrameColor, pst_FontLetter);
			STR_gst_Data.pul_Sprite[i_Sprite] = (STR_gst_Data.pul_Sprite[i_Sprite] & 0xFFFFFF) | ((ul_FrameIcon >> 16) << 24);
			i_Sprite++;
			
			if(i_Sprite == MAX_SPRITE)
			{
				i_Sprite = i_StringFirstSprite;
				goto STRRENDER_DrawString;
			}

		}
	}

STRRENDER_DrawString:

	/* construit les triangles */
	for(i = 0; i < 32; i++)
	{
		if(ul_SpriteFont[i]) GFX_NeedGeomExElem(ai_SubFontElem[ i ], ul_SpriteFont[i] * 2);
		GFX_gpst_Geo->dst_Element[ ai_SubFontElem[i] ].l_NbTriangles = 0;
		GFX_gpst_Geo->dst_Element[ ai_SubFontElem[i] ].l_MaterialId = i;
	}

	for(i = 0; i < i_Sprite; i++)
	{
		i_Font = STR_gst_Data.pul_Sprite[i] >> 24;
		T = GFX_gpst_Geo->dst_Element[ai_SubFontElem[i_Font]].dst_Triangle + GFX_gpst_Geo->dst_Element[ai_SubFontElem[i_Font]].l_NbTriangles;
		GFX_gpst_Geo->dst_Element[ai_SubFontElem[i_Font]].l_NbTriangles += 2;

		s = i << 2;
		T->auw_Index[0] = T->auw_UV[0] = s;
		T->auw_Index[1] = T->auw_UV[1] = s + 1;
		T->auw_Index[2] = T->auw_UV[2] = s + 2;
		T++;
		T->auw_Index[0] = T->auw_UV[0] = s;
		T->auw_Index[1] = T->auw_UV[1] = s + 2;
		T->auw_Index[2] = T->auw_UV[2] = s + 3;
	}

	STRDATA_TranformAllPoints(GFX_gpst_Geo->dst_Point, i_Sprite * 4);

	M_GFX_CheckGeom();
	
#ifdef ACTIVE_EDITORS 
	i_SaveSpeedDraw = LINK_gi_SpeedDraw;
	LINK_gi_SpeedDraw = 0;
#endif

	GDI_gpst_CurDD_SPR.ul_DisplayInfo |= GDI_Cul_DI_RenderingTransparency;
	STR_sgpst_GO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) GFX_gpst_Geo;
	GFX_gpst_Geo->st_Id.i->pfn_Render(STR_sgpst_GO);
	
#ifdef ACTIVE_EDITORS 
	LINK_gi_SpeedDraw = i_SaveSpeedDraw;
#endif 
	

	if(S < SLast)
	{
		i_Sprite = 0;
		goto STRRENDER_ContinueStringAfterDraw;
	}

	STR_RestoreDisplay();
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#if 0

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Render2(OBJ_tdst_GameObject *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct				*pst_STR;
	STR_tdst_Letter				*pst_Letter, *pst_Last;
	int							i, i_Index, i_SubMat, i_Sprite, s;
	STR_tdst_String				*pst_S, *pst_SLast;
	short						X, Y, W, H, X1, Y1, X2, Y2;
	STR_tdst_FontLetterDesc		*pst_FontLetter;
	ULONG						ul_SpriteFont[32], ul_Font[32];
	MAT_tdst_Material			*pst_Mat;
	static UCHAR				uc_SubMatOrder[32] =
	{
		9,
		13,
		16,
		10,
		11,
		12,
		14,
		15,
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8,
		17,
		18,
		19,
		20,
		21,
		22,
		23,
		24,
		25,
		26,
		27,
		28,
		29,
		30,
		31
	};
	static UCHAR				uc_SubMatInvOrder[32] =
	{
		8,
		9,
		10,
		11,
		12,
		13,
		14,
		15,
		16,
		0,
		3,
		4,
		5,
		1,
		6,
		7,
		2,
		17,
		18,
		19,
		20,
		21,
		22,
		23,
		24,
		25,
		26,
		27,
		28,
		29,
		30,
		31
	};
	GEO_tdst_IndexedTriangle	*T;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoSTR) return;
#endif
#ifdef GX8_BENCH
	if(g_iNoSTR) return;
#endif /* GX8_BENCH */

	/*$2- Render an invisible object in editor mode ------------------------------------------------------------------*/

	STR_EditorsRender(_pst_Node);

	pst_STR = (STR_tdst_Struct *) _pst_Node->pst_Base->pst_Visu->pst_Object;
	if(pst_STR->ul_RenderingCounter == GDI_gpst_CurDD->ul_RenderingCounter) return;
	pst_STR->ul_RenderingCounter = GDI_gpst_CurDD->ul_RenderingCounter;

	pst_STR->uw_SW = (short) GDI_gpst_CurDD->st_Camera.f_Width;
	pst_STR->uw_SH = (short) GDI_gpst_CurDD->st_Camera.f_Height;

	STR_SetDisplay(FALSE);
	STR_M_SetMaterial(_pst_Node);
	STR_M_ResetFont(pst_STR);

	if(!(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_RenderingInterface))
		GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_ForceSortTriangle;

	pst_Mat = (MAT_tdst_Material *) _pst_Node->pst_Base->pst_Visu->pst_Material;
	if(!pst_Mat) return;
	i_SubMat = (pst_Mat->st_Id.i->ul_Type == GRO_MaterialMulti) ? (((MAT_tdst_Multi *) pst_Mat)->l_NumberOfSubMaterials) : 1;
	if(i_SubMat > 32) return;
	for(i = 0; i < 32; i++)
	{
		ul_SpriteFont[i] = 0;
		STR_M_SetFont(pst_STR, i);
		ul_Font[i] = (ULONG) pst_STR->pst_Font;
	}

	/* champ de sockage pour les infos des sprites */

	/*
	 * les 24 premiers bits indiquent numéro de lettre -> numéro du sprite
	 * correspondant
	 */

	/* les 8 derniers bits indiquent numéro de sprite -> numéro de material */
	pst_STR->pul_Sprite = GDI_gpst_CurDD->pst_ComputingBuffers->aul_Specular;

	/* compte le nombre max de sprites */
	i_Sprite = 0;

	/* dans les chaines */
	pst_S = pst_STR->dst_String;
	pst_SLast = pst_S + pst_STR->uc_NbMaxStrings;
	for(; pst_S < pst_SLast; pst_S++)
	{
		if(pst_S->uw_Flags & STR_Cuw_SF_Hide) continue;

		if(pst_S->uw_SubNumber)
		{
			i_Sprite += pst_S->uw_SubNumber;
			if(pst_S->uw_Flags & STR_Cuw_SF_ShowFrame) i_Sprite++;
		}
	}

	/* avec les effets */
	i_Sprite += STR_l_Effect_CountAdditionalSprite(pst_STR);
	if(!i_Sprite)
	{
		STR_RestoreDisplay();
		return;
	}

	/* alloue les données nécessaires */
	GFX_NeedGeomEx(i_Sprite * 4, i_Sprite * 4, 32, 1, TRUE);

	/* Calcule les coordonnées de tous les sprites utilisés par les chaines */
	i_Sprite = 0;
	pst_S = pst_STR->dst_String;
	pst_SLast = pst_S + pst_STR->uc_NbMaxStrings;
	for(i_Index = 0; pst_S < pst_SLast; pst_S++, i_Index++)
	{
		if(pst_S->uw_Flags & STR_Cuw_SF_Hide) continue;

		STR_w_XFramePivot = STR_w_XPivot = (short) (pst_S->st_Pivot.x * pst_STR->uw_SW);
		STR_w_YFramePivot = STR_w_YPivot = (short) (pst_S->st_Pivot.y * pst_STR->uw_SH);

		pst_Letter = pst_STR->dst_Letter + pst_S->uw_First + pst_S->uw_SubFirst;
		pst_Last = pst_Letter + pst_S->uw_SubNumber;
		if(pst_Last <= pst_Letter) continue;

		STR_ComputeFrame(pst_STR, i_Index, pst_S);

		/* sprite pour frame */
		if(pst_S->uw_Flags & STR_Cuw_SF_ShowFrame)
		{
			i = ((pst_S->st_Frame.ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift) % i_SubMat;
			pst_STR->pst_Font = (STR_tdst_FontDesc *) ul_Font[i];
			if(pst_STR->pst_Font)
			{
				pst_STR->w_FontPage = i;
				ul_SpriteFont[i]++;

				pst_FontLetter = pst_STR->pst_Font->pst_Letter + STR_M_A2I(pst_S->st_Frame.ul_Flags);
				X1 = (pst_S->st_Frame.x[0] + STR_w_XFramePivot - pst_S->st_Frame.borderx);
				Y1 = (pst_STR->uw_SH - (pst_S->st_Frame.y[0] + STR_w_YFramePivot - pst_S->st_Frame.bordery));
				X2 = (pst_S->st_Frame.x[1] + STR_w_XFramePivot + pst_S->st_Frame.borderx);
				Y2 = (pst_STR->uw_SH - (pst_S->st_Frame.y[1] + STR_w_YFramePivot + pst_S->st_Frame.bordery));
				STR_AddSprite_1C(i_Sprite, X1, Y1, X2, Y2, pst_S->st_Frame.ul_Color, pst_FontLetter);

				pst_STR->pul_Sprite[i_Sprite] = (pst_STR->pul_Sprite[i_Sprite] & 0xFFFFFF) | (i << 24);
				i_Sprite++;
			}
		}

		if(pst_S->uw_SubFirst != 0)
		{
			STR_w_XPivot += pst_STR->dst_Letter[pst_S->uw_First].x - pst_Letter->x;
			STR_w_YPivot += pst_STR->dst_Letter[pst_S->uw_First].y - pst_Letter->y;
		}

		for(; pst_Letter < pst_Last; pst_Letter++)
		{
			i = ((pst_Letter->ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift) % i_SubMat;
			pst_STR->pst_Font = (STR_tdst_FontDesc *) ul_Font[i];
			pst_STR->w_FontPage = i;
			if(!pst_STR->pst_Font) continue;
			ul_SpriteFont[i]++;

			pst_FontLetter = pst_STR->pst_Font->pst_Letter + STR_M_A2I(pst_Letter->ul_Flags);
			X = pst_Letter->x;
			Y = pst_Letter->y;
			W = pst_Letter->w;
			H = pst_Letter->h;

			if(!(pst_Letter->ul_Flags & (STR_Cul_LF_CenterX | STR_Cul_LF_RightX)))
				STR_w_LetterDX = 0;
			else if(pst_Letter->ul_Flags & STR_Cul_LF_CenterX)
				STR_w_LetterDX = W >> 1;
			else
				STR_w_LetterDX = W;

			if(!(pst_Letter->ul_Flags & (STR_Cul_LF_CenterY | STR_Cul_LF_BottomY)))
				STR_w_LetterDY = 0;
			else if(pst_Letter->ul_Flags & STR_Cul_LF_CenterY)
				STR_w_LetterDY = H >> 1;
			else
				STR_w_LetterDY = H;

			Y1 = pst_STR->uw_SH - (Y + STR_w_YPivot - STR_w_LetterDY);
			Y2 = Y1 - H;
			X1 = X + STR_w_XPivot - STR_w_LetterDX;
			X2 = X1 + W;
			STR_AddSprite_1C(i_Sprite, X1, Y1, X2, Y2, pst_Letter->ul_Color, pst_FontLetter);

			pst_STR->pul_Sprite[pst_Letter - pst_STR->dst_Letter] = (pst_STR->pul_Sprite[pst_Letter - pst_STR->dst_Letter] & 0xFF000000) | i_Sprite;
			pst_STR->pul_Sprite[i_Sprite] = (pst_STR->pul_Sprite[i_Sprite] & 0xFFFFFF) | (i << 24);
			i_Sprite++;
		}
	}

	/* effets spéciaux */
	pst_STR->ul_NbSprite = i_Sprite;
	pst_STR->pul_SpriteFont = ul_SpriteFont;
	STR_Effect_TreatAll2(pst_STR);
	i_Sprite = pst_STR->ul_NbSprite;

	/* construit les triangles */
	for(i = 0; i < 32; i++)
	{
		i_SubMat = (int) uc_SubMatOrder[i];

		if(ul_SpriteFont[i_SubMat])
		{
			s = i + 1;
			GFX_NeedGeomExElem(i, ul_SpriteFont[i_SubMat] * 2);
		}

		GFX_gpst_Geo->dst_Element[i].l_NbTriangles = 0;
		GFX_gpst_Geo->dst_Element[i].l_MaterialId = i_SubMat;
	}

	GFX_gpst_Geo->l_NbElements = s;

	for(i = 0; i < i_Sprite; i++)
	{
		s = uc_SubMatInvOrder[pst_STR->pul_Sprite[i] >> 24];
		T = GFX_gpst_Geo->dst_Element[s].dst_Triangle + GFX_gpst_Geo->dst_Element[s].l_NbTriangles;
		GFX_gpst_Geo->dst_Element[s].l_NbTriangles += 2;

		s = i << 2;
		T->auw_Index[0] = T->auw_UV[0] = s;
		T->auw_Index[1] = T->auw_UV[1] = s + 1;
		T->auw_Index[2] = T->auw_UV[2] = s + 2;
		T++;
		T->auw_Index[0] = T->auw_UV[0] = s;
		T->auw_Index[1] = T->auw_UV[1] = s + 2;
		T->auw_Index[2] = T->auw_UV[2] = s + 3;
	}

	STR_TranformAllPoints(GFX_gpst_Geo->dst_Point, i_Sprite * 4);

	M_GFX_CheckGeom();

	STR_sgpst_GO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) GFX_gpst_Geo;
	GFX_gpst_Geo->st_Id.i->pfn_Render(STR_sgpst_GO);

	STR_RestoreDisplay();
}

#endif
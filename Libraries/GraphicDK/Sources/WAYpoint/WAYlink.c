/*$T WAYlink.c GC! 1.076 03/09/00 18:53:16 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#ifdef ACTIVE_EDITORS

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTlinear.h"
#include "SOFT/SOFTPickingBuffer.h"
#include "GRObject/GROrender.h"
#include "WAYpoint/WAYlink.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_Links_Init(WAY_tdst_GraphicLinks *_pst_GL)
{
	L_memset(_pst_GL, 0, sizeof(WAY_tdst_GraphicLinks));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_Links_Close(WAY_tdst_GraphicLinks *_pst_GL)
{
	if(_pst_GL->dst_GL) MEM_Free(_pst_GL->dst_GL);
	L_memset(_pst_GL, 0, sizeof(WAY_tdst_GraphicLinks));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_Links_Clear(WAY_tdst_GraphicLinks *_pst_GL)
{
	_pst_GL->l_Next = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_GraphicLink *WAY_Links_GetNextLink(WAY_tdst_GraphicLinks *_pst_GL)
{
	/*~~~~~~~~~~~~*/
	ULONG	ul_Size;
	/*~~~~~~~~~~~~*/

	if(_pst_GL->l_Max == _pst_GL->l_Next)
	{
		ul_Size = (_pst_GL->l_Max + 50) * sizeof(WAY_tdst_GraphicLink);
		if(_pst_GL->l_Max == 0)
			_pst_GL->dst_GL = (WAY_tdst_GraphicLink *) MEM_p_Alloc(ul_Size);
		else
			_pst_GL->dst_GL = (WAY_tdst_GraphicLink *) MEM_p_Realloc(_pst_GL->dst_GL, ul_Size);
		_pst_GL->l_Max += 50;
	}

	return(_pst_GL->dst_GL + _pst_GL->l_Next++);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_Link_Render
(
	OBJ_tdst_GameObject		*_pst_GO,
	WAY_tdst_Link			*_pst_Link,
	ULONG					_ul_EditorFlags,
	char					_c_Hierarchy,
	char					_c_Init
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix		st_Matrix;
	MATH_tdst_Vector		st_Z, st_X, st_Y;
	MATH_tdst_Vector		st_Pos, st_EndPos;
    MATH_tdst_Vector        st_Temp;
	ULONG					C, DM;
	WAY_tdst_GraphicLink	*pst_Link;
    float                   f_SizeFactor, f_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO) return;
	if(!_pst_Link) return;
	if(!_pst_Link->pst_Next) return;

	/* Compute object matrix */
	MATH_CopyVector(&st_Pos, OBJ_pst_GetAbsolutePosition(_pst_GO));
	MATH_CopyVector(&st_EndPos, OBJ_pst_GetAbsolutePosition(_pst_Link->pst_Next));
	MATH_SubVector(&st_Y, &st_EndPos, &st_Pos);
	if(MATH_f_NormVector(&st_Y)<0.000001f) return;
	MATH_InitVector(&st_Z, 0.0f, 0.0f, 1.0f);
	MATH_CrossProduct(&st_X, &st_Y, &st_Z);
	if(MATH_b_NulVector(&st_X))
	{
		MATH_InitVector(&st_Z, 1.0f, 0.0f, 0.0f);
		MATH_CrossProduct(&st_X, &st_Y, &st_Z);
	}

	MATH_NormalizeVector(&st_X, &st_X);
	MATH_CrossProduct(&st_Z, &st_X, &st_Y);
	MATH_NormalizeVector(&st_Z, &st_Z);

	MATH_SetIdentityMatrix(&st_Matrix);

    st_Matrix.Sy = MATH_f_NormVector(&st_Y);

    if (_c_Hierarchy )
    {
        f_SizeFactor = st_Matrix.Sy / 4;
        if (f_SizeFactor > 1.0f)
            f_SizeFactor = 1.0f;
    }
    else
    {
        if (GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_Proportionnal)
        {
            f_SizeFactor = 0.1f * fNormalTan( GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);
            
            SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix( _pst_GO ) );
            MATH_TransformVertex(&st_Temp, GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix, &MATH_gst_NulVector);
            SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);
            f_Size = f_SizeFactor * ((st_Temp.z > 0) ? st_Temp.z : 1.0f);
            
            SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix( _pst_Link->pst_Next ) );
            MATH_TransformVertex(&st_Temp, GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix, &MATH_gst_NulVector);
            SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);
            f_SizeFactor *= (st_Temp.z > 0) ? st_Temp.z : 1.0f;

            if (f_Size < f_SizeFactor) f_SizeFactor = f_Size;
            if (f_SizeFactor < 1.0f) f_SizeFactor = 1.0f;
        }
        else
            f_SizeFactor = 1.0f;
    }

    f_Size = 0.1f * f_SizeFactor;
	st_Matrix.Sx = st_Matrix.Sz = f_Size;
	MATH_DivEqualVector(&st_Y, st_Matrix.Sy);
	MATH_SetScaleType(&st_Matrix);

	MATH_CopyVector(MATH_pst_GetXAxis(&st_Matrix), &st_X);
	MATH_CopyVector(MATH_pst_GetYAxis(&st_Matrix), &st_Y);
	MATH_CopyVector(MATH_pst_GetZAxis(&st_Matrix), &st_Z);
	MATH_SetRotationType(&st_Matrix);

    MATH_ScaleEqualVector(&st_X, f_Size * .5f);
	MATH_AddEqualVector(&st_Pos, &st_X);
	MATH_ScaleEqualVector(&st_Z, f_Size * -.5f);
	MATH_AddEqualVector(&st_Pos, &st_Z);
    MATH_SetTranslation(&st_Matrix, &st_Pos);

	/* Display link */
	SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, &st_Matrix);

	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
	

	C = (_ul_EditorFlags & OBJ_C_EditFlags_Selected) ? 1 : 0;
	if(_c_Hierarchy)
	{
		if(_c_Init)
			C = C ? 0xFF00FFFF : 0xFF7f00FF;
		else
			C = C ? 0xFF00FFFF : 0xFF007FFF;
	}
	else
	{
		C = GEO_ul_DebugObject_GetColor(GEO_DebugObject_LinkArrow, C);
		if(!(_ul_EditorFlags & OBJ_C_EditFlags_Selected))
		{
			if(_pst_Link->uw_CapacitiesInit & 32)
				C = 0x00FFFFFF;
			else if(_pst_Link->uw_CapacitiesInit & 16)
				C = 0x0000FFFF;
			else if(_pst_Link->uw_CapacitiesInit & 8)
				C = 0x00FFFF00;
			else if(_pst_Link->uw_CapacitiesInit & 4)
				C = 0x00FF0000;
			else if(_pst_Link->uw_CapacitiesInit & 2)
				C = 0x0000FF00;
			else if(_pst_Link->uw_CapacitiesInit & 1)
				C = 0x000000FF;
		}
	}

	GEO_DebugObject_SetLight(C);
	DM = GDI_Cul_DM_All - GDI_Cul_DM_UseAmbient - GDI_Cul_DM_Fogged - GDI_Cul_DM_MaterialColor;
    GEO_DebugObject_Draw(GDI_gpst_CurDD, GEO_DebugObject_Box, DM, 0, &st_Matrix);

	if(_c_Hierarchy)
    {
        /* Display link in picking buffer */
		if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer)
		{
			GRO_RenderPickableObject
			(
				NULL,
				GEO_pst_DebugObject_Get(GEO_DebugObject_Box),
				(ULONG) _pst_Link->pst_Next,
				SOFT_Cuc_PBQF_HieLink
			);
		}
    }
    else
	{
		/* Add new link to list of links */
		pst_Link = WAY_Links_GetNextLink(&GDI_gpst_CurDD->st_DisplayedLinks);
		pst_Link->pst_Link = _pst_Link;
		pst_Link->pst_Origin = _pst_GO;

		/* Display link in picking buffer */
		if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer)
		{
			GRO_RenderPickableObject
			(
				NULL,
				GEO_pst_DebugObject_Get(GEO_DebugObject_Box),
				(ULONG) pst_Link,
				SOFT_Cuc_PBQF_Link
			);
		}
	}

	SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;

	/* Display link arrow to show link direction */
    MATH_ScaleVector(&st_EndPos, &st_Y, st_Matrix.Sy * (_c_Hierarchy ? .5f : .6f) );
	MATH_CopyVector(&st_Pos, OBJ_pst_GetAbsolutePosition(_pst_GO));
	MATH_AddEqualVector(&st_Pos, &st_EndPos);
	MATH_SetZoom( &st_Matrix, f_SizeFactor );
    MATH_AddScaleVector(&st_Pos, &st_Pos, &st_X, 2);
    MATH_SetTranslation(&st_Matrix, &st_Pos);

	SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, &st_Matrix);
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
	

	GEO_DebugObject_SetLight(C);
    GEO_DebugObject_Draw(GDI_gpst_CurDD, GEO_DebugObject_LinkArrow, DM, 0, &st_Matrix);

	SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
} 

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

#endif /*ACTIVE_EDITORS*/

/*$T GROrender.c GC!1.55 01/11/00 11:16:15 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDIrasters.h"
#include "GDInterface/GDIrequest.h"
#include "GRObject/GROstruct.h"
#include "GRObject/GROrender.h"
#include "GRObject/GROedit.h"
#include "SELection/SELection.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEO_SKIN.h"

#include "SOFT/SOFTlinear.h"
#include "SOFT/SOFTPickingBuffer.h"
#include "SOFT/SOFTZlist.h"

#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"

#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#endif
#include "ENGine/Sources/MoDiFier/MDFmodifier_FCLONE.h"

#include "BASe/BENch/BENch.h"

#ifdef _GAMECUBE
#ifndef _FINAL_
#include "GXI_GC/GXI_dbg.h"
#endif
#endif

/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */


//extern int NbrObjectClone ;//de base old
//extern BOOL b_CloneCulled[MAXCLONE]; old

#ifdef ACTIVE_EDITORS

#define M_4Edit_SpecialPush_Init()  l_SpecialPush = 0;
#define M_4Edit_SpecialPush_DoIt()  l_SpecialPush = pst_Obj->i->pfnl_PushSpecialMatrix(_pst_GO);
#define M_4Edit_SpecialPush_End()\
    while(l_SpecialPush--)\
        SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);

#define M_4Edit_BadVisu_Init() \
    if( ( pst_Obj == NULL ) && ( GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DisplayVisuWithoutGro) )\
    {\
        pst_Obj = pst_Visu->pst_Object = (GRO_tdst_Struct *) GEO_pst_DebugObject_Get(GEO_DebugObject_Question);\
        pst_Visu->pst_Material = NULL;\
        GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_DontSortObject;\
    }

#define M_4Edit_BadVisu_End() \
    if(pst_Obj == (GRO_tdst_Struct *) GEO_pst_DebugObject_Get(GEO_DebugObject_Question))\
    {\
        pst_Visu->pst_Object = NULL;\
        pst_Visu->pst_Material = pst_Mat;\
    }

#define M_4Edit_RenderPickable(a,b) GRO_RenderPickable(a,b)

#else

#define M_4Edit_SpecialPush_Init()
#define M_4Edit_SpecialPush_DoIt()
#define M_4Edit_SpecialPush_End()
#define M_4Edit_BadVisu_Init() 
#define M_4Edit_BadVisu_End()
#define M_4Edit_RenderPickable(a,b)

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
#ifdef ACTIVE_EDITORS
void GRO_RenderPickable(OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *pst_PickableObject)
{
    OBJ_tdst_GameObject	*pst_GO;
    BOOL				b_SelectBone;

    if(pst_PickableObject && (GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer))
	{
	    b_SelectBone = GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_SelectBone;
		if(GetAsyncKeyState('A') < 0)
		    b_SelectBone = b_SelectBone ? FALSE : TRUE;

        pst_GO = _pst_GO;
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Bone) && !b_SelectBone)
			pst_GO = ANI_pst_GetReference(_pst_GO);

        /*$2- MRM --------------------------------------------------------------------------------------------------------*/
	    GDI_gpst_CurDD->pus_ReorderBuffer = NULL;
        if (pst_PickableObject->p_MRM_Levels)
        {
#ifdef ACTIVE_EDITORS
            extern BOOL	OGL_gb_DispLOD;
            extern ULONG OGL_ulLODAmbient;
#endif //ACTIVE_EDITORS
            GEO_MRM_SetLevelFromQuality(pst_PickableObject,GEO_MRM_GetFloatFromUC((ULONG) _pst_GO->uc_LOD_Vis));
#ifdef ACTIVE_EDITORS
            if (OGL_gb_DispLOD)
            {
                OGL_ulLODAmbient = 0xFF000000 |
                    (pst_PickableObject->p_MRM_Levels->ul_CurrentLevel & 1 ? 0x000000A0 : 0x00000040) |
                    (pst_PickableObject->p_MRM_Levels->ul_CurrentLevel & 2 ? 0x0000A000 : 0x00004000) |
                    (pst_PickableObject->p_MRM_Levels->ul_CurrentLevel & 4 ? 0x00A00000 : 0x00400000);
            }
            else 
                OGL_ulLODAmbient = 0;
#endif //ACTIVE_EDITORS
        }
        else if(GEO_MRM_ul_IsMrmObject(pst_PickableObject))
            GEO_MRM_SetNumberOfPoints_Percent(pst_PickableObject, GEO_MRM_GetFloatFromUC((ULONG) _pst_GO->uc_LOD_Vis));

	    /*$2- Modifiers --------------------------------------------------------------------------------------------------*/
	    GDI_gpst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseSpecialVertexBuffer;
#ifdef JADEFUSION
		pst_PickableObject->m_pWYB1 = pst_PickableObject->m_pWYB2 = NULL;
#endif
		MDF_ApplyAll(_pst_GO, 0);

	    /*$2- SKIN -------------------------------------------------------------------------------------------------------*/
	    if(GEO_SKN_IsSkinned(pst_PickableObject))
		    GEO_SKN_Compute4Display(_pst_GO, pst_PickableObject);

        /*$2- recomputing normals ----------------------------------------------------------------------------------------*/
        if ( !(GDI_gpst_CurDD ->ul_CurrentDrawMask & GDI_Cul_DM_DontRecomputeNormales) && (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer) )
		    GEO_SKN_ComputeNormals(pst_PickableObject, GDI_gpst_CurDD->p_Current_Vertex_List);

		GRO_RenderPickableObject(_pst_GO, pst_PickableObject, (ULONG) pst_GO, SOFT_Cuc_PBQF_GameObject);

#ifdef RenderPickableClone//desactivé test

		if (NbrObjectClone)
		{
			FCLONE_tdst_Modifier * p_FCLONE = NULL;
			CLONE_Data *v;

			SOFT_tdst_PickingBuffer_Query	*pst_Query;
			u32 Counter;
			MATH_tdst_Matrix SaveCurrent;
			bool sort=0;
			int number;
			MDF_tdst_Modifier		*pst_Modifier;
			pst_Modifier = pst_GO->pst_Extended->pst_Modifiers;

			//CLONE_CurrentSelection = 0;
			while (pst_Modifier)
			{	
				if ( pst_Modifier->i->ul_Type == MDF_C_Modifier_FClone)
				{
					p_FCLONE = (FCLONE_tdst_Modifier *) pst_Modifier->p_Data;
					v = p_FCLONE->cl_eData;
					v+=NbrObjectClone-1;
					break;
				}
				pst_Modifier = pst_Modifier->pst_Next;
			}

			pst_Query = &GDI_gpst_CurDD->pst_PickingBuffer->st_Query;
			SaveCurrent = *pst_GO->pst_GlobalMatrix;

			if ( pst_Query->dst_List )
			{
				number = pst_Query->l_Number;
				if ( number>0 )
				{
					while (number--)
					{
						if ( (void *) pst_Query->dst_List[number].ul_Value == _pst_GO )
						sort=1;
						break;
					}
				}
			}

			if (!sort)
			{
				Counter = NbrObjectClone;
				while(Counter--)
				{
					if (!b_CloneCulled[Counter])
					{
						MATH_tdst_Matrix m_temp;
						MATH_ConvertQuaternionToMatrix(&m_temp, &v->qQ);
						m_temp.T = v->vPos;

						*pst_GO->pst_GlobalMatrix = m_temp;

						SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);
						SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, &m_temp);
						GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
						GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
						GRO_RenderPickableObject(_pst_GO, pst_PickableObject, (ULONG) pst_GO, SOFT_Cuc_PBQF_GameObject);
						//CLONE_CurrentSelection++;
						if ( pst_Query->dst_List )
						{
							number = pst_Query->l_Number;
							if ( number>0 )
							{
								sort=false;
								while (number--)
								{
									if ( (void *) pst_Query->dst_List[number].ul_Value == _pst_GO ) 
									{
										MATH_tdst_Matrix SaveCurrent2;
										SaveCurrent2 = SaveCurrent;
										SaveCurrent = m_temp;
										m_temp = SaveCurrent2;
										MATH_ConvertMatrixToQuaternion(&v->qQ,&m_temp);
										v->vPos = m_temp.T;
										sort=true;
										break;
									}

								}
								if (sort) break;
							}
						}
					}
					v--;
				}
			}//sort
			*pst_GO->pst_GlobalMatrix = SaveCurrent;
			SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);
			SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
			//sprintf(_pst_GO->sz_Name,savename);
		}
#endif

		/*$2- Modifiers --------------------------------------------------------------------------------------------------*/
		MDF_UnApplyAll(_pst_GO, 0);

		// End of MRM level rendering
		if (pst_PickableObject->p_MRM_Levels)
			GEO_MRM_SetAllLevels(pst_PickableObject);
	}
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
ULONG GRO_IsParentSelected(OBJ_tdst_GameObject *p_stSon )
{
	if (p_stSon->ul_EditorFlags & OBJ_C_EditFlags_Selected) return 1;
	if ((OBJ_ul_FlagsIdentityGet(p_stSon) & OBJ_C_IdentityFlag_Hierarchy) && p_stSon->pst_Base && p_stSon->pst_Base->pst_Hierarchy && (p_stSon->pst_Base->pst_Hierarchy->pst_Father))
	{
			return GRO_IsParentSelected(p_stSon->pst_Base->pst_Hierarchy->pst_Father);
	}
	return 0;
}

ULONG OGL_EditorInfoCorrectMul2X(ULONG Color)
{
	if (!GDI_gpst_CurDD->GlobalMul2X) return Color;
	Color &= 0xfFfefefe;
	Color >>= 1;
	Color += Color & (0xff000000 >> 1);
	return Color;
}

extern "C" BOOL GRO_gb_ResetGhost = FALSE;

void GRO_RenderWired(OBJ_tdst_GameObject *_pst_GO, GRO_tdst_Struct *pst_Obj , GDI_tdst_DisplayData   *_pst_CurDD)
{
	JADED_PROFILER_START();

	ULONG Mode;
	ULONG savesave, saveC;

	if(GRO_gb_ResetGhost) _pst_GO->ul_EditorFlags                    &= ~OBJ_C_EditFlags_Ghost;
	savesave = _pst_CurDD->ul_CurrentDrawMask;
	saveC = _pst_CurDD->ul_ColorConstant;
	_pst_CurDD->ul_ColorConstant = OGL_EditorInfoCorrectMul2X(_pst_CurDD->ul_ColorConstant);
	if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Ghost) 
	{
		_pst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_NotWired | GDI_Cul_DM_UseTexture | GDI_Cul_DM_DontForceColor );
		_pst_CurDD->ul_ColorConstant = OGL_EditorInfoCorrectMul2X(0x10F0F0F0) ;
	}

	if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GhostODE) 
	{
		_pst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_NotWired | GDI_Cul_DM_UseTexture | GDI_Cul_DM_DontForceColor );
		_pst_CurDD->ul_ColorConstant = OGL_EditorInfoCorrectMul2X(0x0099FFFF);
	}


	if (_pst_CurDD->ul_DisplayFlags & GDI_Cul_DF_ShadeSelected)
	{
		if (GRO_IsParentSelected(_pst_GO))
			Mode = (_pst_CurDD->ul_WiredMode & 3);
		else
		{
			Mode = (_pst_CurDD->ul_WiredMode & 3) + 2;
			if (_pst_CurDD->ul_WiredMode == 0x202)
				Mode = 0;
		}
		Mode %= 3;
	} else
	{
		if (GRO_IsParentSelected(_pst_GO))
			Mode = _pst_CurDD->ul_WiredMode >> 8;
		else
			Mode = _pst_CurDD->ul_WiredMode & 3;
	}
	
	if (_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowSlope )
	{
		_pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_ShowSlope;
	}
	
	if (Mode == 0)
	{
		pst_Obj->i->pfn_Render(_pst_GO);
	}
	else
	{
		ULONG			SavedDrawMask , SaveK, SaveEditOption;
		static float	f[2] = { 2, 2 };

		if ( Mode == 1)
		{
			SaveEditOption = _pst_CurDD->pst_EditOptions->ul_Flags;
			_pst_CurDD->pst_EditOptions->ul_Flags |= GRO_Cul_EOF_HideSubObject;
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Enable, 0x2A02 /* GL_POLYGON_OFFSET_LINE */ );
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Enable, 0x8037 /* GL_POLYGON_OFFSET_FILL */ );
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PolygonOffset, (ULONG) f);
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PolygonOffsetSave, 0); /* store polygon offset editor parameter */
			pst_Obj->i->pfn_Render(_pst_GO);
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Disable, 0x2A02 /* GL_POLYGON_OFFSET_LINE */ );
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Disable, 0x8037 /* GL_POLYGON_OFFSET_FILL */ );
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PolygonOffsetSave, 0); /* store polygon offset editor parameter */
			_pst_CurDD->pst_EditOptions->ul_Flags = SaveEditOption;
		}

		//GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Enable, 0x2A02 /* GL_POLYGON_OFFSET_LINE */ );
		//GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PolygonOffset, (ULONG) f);

		SavedDrawMask = _pst_CurDD->ul_CurrentDrawMask;
		_pst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_NotWired | GDI_Cul_DM_UseTexture | GDI_Cul_DM_DontForceColor );
		SaveK = _pst_CurDD->ul_ColorConstant;
		_pst_CurDD->ul_ColorConstant = OGL_EditorInfoCorrectMul2X(_pst_CurDD->pst_EditOptions->ul_WiredColor);
		if (GRO_IsParentSelected(_pst_GO))
		{
			_pst_CurDD->ul_ColorConstant = OGL_EditorInfoCorrectMul2X(_pst_CurDD->pst_EditOptions->ul_WiredColorSel);
			_pst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_Fogged| GDI_Cul_DM_TestBackFace);
		}
		
		pst_Obj->i->pfn_Render(_pst_GO);
		_pst_CurDD->ul_CurrentDrawMask = SavedDrawMask;
		_pst_CurDD->ul_ColorConstant = SaveK;
			
		//GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Disable, 0x2A02 /* GL_POLYGON_OFFSET_LINE */ );
	}

	_pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_ShowSlope;
	_pst_CurDD->ul_CurrentDrawMask = savesave;
	_pst_CurDD->ul_ColorConstant = saveC;

	JADED_PROFILER_END();
}
#else
#define GRO_RenderWired(_pst_GO, pst_Obj , p_stCurDD) pst_Obj->i->pfn_Render(_pst_GO);
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_PushLookAtMatrix(GDI_tdst_DisplayData    *p_stCurDD , OBJ_tdst_GameObject *_pst_GO)
{
	MATH_tdst_Vector    st_Sight;
	MATH_tdst_Vector    st_Up;
	MATH_tdst_Matrix    st_LookAtMatrix;

	PRO_StartTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_LookAt);

	MATH_SubVector
	(
		&st_Sight,
		OBJ_pst_GetAbsolutePosition(_pst_GO),
		MATH_pst_GetTranslation(&p_stCurDD->st_Camera.st_Matrix)
	);
	MATH_ScaleVector(&st_Up, MATH_pst_GetYAxis(&p_stCurDD->st_Camera.st_Matrix), -1.0f);

	MATH_MakeRotationMatrix_UsingSight(&st_LookAtMatrix, &st_Sight, &st_Up, 1);
	MATH_ScaleVector(&st_Up, MATH_pst_GetYAxis(&st_LookAtMatrix), -1.0f);
	MATH_CopyVector(MATH_pst_GetYAxis(&st_LookAtMatrix), MATH_pst_GetZAxis(&st_LookAtMatrix));
	MATH_CopyVector(MATH_pst_GetZAxis(&st_LookAtMatrix), &st_Up);
	MATH_SetTranslation(&st_LookAtMatrix, OBJ_pst_GetAbsolutePosition(_pst_GO));

	if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_GO)))
	{
		MATH_GetScaleVector(&st_Up, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
		MATH_SetScale(&st_LookAtMatrix, &st_Up);
	}

	PRO_StopTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_LookAt);
	PRO_StartTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_PushMatrix);
	SOFT_l_MatrixStack_Push(&p_stCurDD->st_MatrixStack, &st_LookAtMatrix);
	PRO_StopTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_PushMatrix);
}
#ifdef GSP_PS2
extern u_int NoGODRAW;
#endif
void GRO_Render(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
    LONG                l_SpecialPush;
#endif
    GRO_tdst_Visu       *pst_Visu;
    GEO_tdst_Object     *pst_PickableObject;
    GRO_tdst_Struct     *pst_Obj, *pst_Mat;
    register GDI_tdst_DisplayData    *p_stCurDD;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if defined(GSP_PS2) || (defined(_GAMECUBE) && !defined(_FINAL_))
   if (NoGODRAW) return;
#endif
	p_stCurDD = GDI_gpst_CurDD;

    M_4Edit_SpecialPush_Init();


    pst_Visu = _pst_GO->pst_Base->pst_Visu;
    pst_Obj = pst_Visu->pst_Object;
    pst_Mat = pst_Visu->pst_Material;
    
    M_4Edit_BadVisu_Init();

    if(!pst_Obj) return;
    if(!pst_Obj->i) return;
    if(!pst_Obj->i->pfn_Render) return;

	JADED_PROFILER_START();

	SOFT_ZList_SelectZListe(ZListesBase + pst_Visu->c_DisplayOrder);

    p_stCurDD->ul_DisplayInfo &= GDI_Cul_DI_ComputeShadowMap | GDI_Cul_DI_DoubleRendering_I | GDI_Cul_DI_DoubleRendering_K | GDI_Cul_DI_RenderingTransparency | GDI_Cul_DI_RenderingInterface;
    p_stCurDD->ul_CurrentDrawMask = p_stCurDD->ul_DrawMask & pst_Visu->ul_DrawMask;
	p_stCurDD->pst_CurrentGameObject = _pst_GO;
    p_stCurDD->pst_CurrentGeo = NULL;
    p_stCurDD->pst_ComputingBuffers->ul_Flags = 0;
    
    if(pst_Obj->i->pfnl_HasSomethingToRender(pst_Visu, &pst_PickableObject))
    {
        PRO_IncRasterLong(&p_stCurDD->pst_Raster->st_NbObjects, 1);

        if(!(p_stCurDD->ul_DisplayFlags & GDI_Cul_DF_DoNotRender))
        {
            if(OBJ_b_TestControlFlag(_pst_GO, OBJ_C_ControlFlag_LookAt))
            {
            	GRO_PushLookAtMatrix(p_stCurDD , _pst_GO);
            }
            else//*/
            {
                PRO_StartTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_PushMatrix);
                SOFT_l_MatrixStack_Push(&p_stCurDD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
                M_4Edit_SpecialPush_DoIt()
                PRO_StopTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_PushMatrix);
            }

            PRO_StartTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_SetViewMatrix);
			p_stCurDD->st_Camera.pst_ObjectToCameraMatrix = p_stCurDD->st_MatrixStack.pst_CurrentMatrix;
			GDI_SetViewMatrix((*p_stCurDD) , p_stCurDD->st_Camera.pst_ObjectToCameraMatrix);
            PRO_StopTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_SetViewMatrix);
            PRO_StartTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_Render);

			GRO_RenderWired(_pst_GO, pst_Obj , p_stCurDD);

            PRO_StopTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_Render);
	        M_4Edit_RenderPickable( _pst_GO, pst_PickableObject );
	        M_4Edit_SpecialPush_End();
	        SOFT_l_MatrixStack_Pop(&p_stCurDD->st_MatrixStack);
        }
#ifndef PSX2_TARGET
        else
        {
            PRO_StartTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_PushMatrix);
            SOFT_l_MatrixStack_Push(&p_stCurDD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
            M_4Edit_SpecialPush_DoIt();
            PRO_StopTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_PushMatrix);
            PRO_StartTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_SetViewMatrix);
            p_stCurDD->st_Camera.pst_ObjectToCameraMatrix = p_stCurDD->st_MatrixStack.pst_CurrentMatrix;
			GDI_SetViewMatrix((*p_stCurDD) , p_stCurDD->st_Camera.pst_ObjectToCameraMatrix);
            PRO_StopTrameRaster(&p_stCurDD->pst_Raster->st_GroRender_SetViewMatrix);
	        M_4Edit_RenderPickable( _pst_GO, pst_PickableObject );
	        M_4Edit_SpecialPush_End();
	        SOFT_l_MatrixStack_Pop(&p_stCurDD->st_MatrixStack);
        }//*/
#endif
    }

    M_4Edit_BadVisu_End();

    p_stCurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseSpecialVertexBuffer;
	SOFT_ZList_SelectZListe(ZListesBase);

	JADED_PROFILER_END();
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_RenderGro(OBJ_tdst_GameObject *_pst_GO, GRO_tdst_Struct *pst_Obj)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GRO_tdst_Visu   st_Visu;
    GEO_tdst_Object *pst_PickableObject;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(pst_Obj == NULL) return;

    st_Visu.pst_Object = pst_Obj;

    GDI_gpst_CurDD->ul_DisplayInfo &= GDI_Cul_DI_ComputeShadowMap | GDI_Cul_DI_DoubleRendering_I | GDI_Cul_DI_DoubleRendering_K | GDI_Cul_DI_RenderingTransparency | GDI_Cul_DI_RenderingInterface;
    GDI_gpst_CurDD->ul_CurrentDrawMask = GDI_gpst_CurDD->ul_DrawMask;
    GDI_gpst_CurDD->pst_CurrentGameObject = _pst_GO;
    GDI_gpst_CurDD->pst_CurrentGeo = NULL;
    GDI_gpst_CurDD->pst_ComputingBuffers->ul_Flags = 0;

    if(pst_Obj->i->pfnl_HasSomethingToRender(&st_Visu, &pst_PickableObject))
    {
#ifdef ACTIVE_EDITORS
		LONG l_SpecialPush;
#endif

        if(!(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DoNotRender))
        {
            PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GroRender_PushMatrix);
            SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
            M_4Edit_SpecialPush_DoIt();
            PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GroRender_PushMatrix);

            PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GroRender_SetViewMatrix);
            GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
			GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
            
            PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GroRender_SetViewMatrix);

            PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GroRender_Render);

			pst_Obj->i->pfn_Render(_pst_GO);

            PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GroRender_Render);
        }
        else
        {
            PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GroRender_PushMatrix);
            SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
            M_4Edit_SpecialPush_DoIt();
            PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GroRender_PushMatrix);

            PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GroRender_SetViewMatrix);
            GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
			GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
            
            PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GroRender_SetViewMatrix);
        }

#ifdef ACTIVE_EDITORS
        if(pst_PickableObject && (GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer))
            GRO_RenderPickableObject(_pst_GO, pst_PickableObject, (ULONG) _pst_GO, SOFT_Cuc_PBQF_GameObject);
#endif
        M_4Edit_SpecialPush_End();
        SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);
    }
}

void GAO_Render(OBJ_tdst_GameObject * _pst_GO )
{
    // Set up the GDI view matrix 
    GDI_tdst_DisplayData    *p_stCurDD = GDI_gpst_CurDD;

    SOFT_l_MatrixStack_Push(&p_stCurDD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix(_pst_GO));

    p_stCurDD->st_Camera.pst_ObjectToCameraMatrix = p_stCurDD->st_MatrixStack.pst_CurrentMatrix;
    GDI_SetViewMatrix((*p_stCurDD) , p_stCurDD->st_Camera.pst_ObjectToCameraMatrix);

    SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);
}

/*$4
 ***********************************************************************************************************************
    Private functions
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_RenderPickableObject
(
    OBJ_tdst_GameObject     *_pst_GO,
    GEO_tdst_Object         *_pst_PO,
    ULONG           _ul_Value,
    ULONG           _ul_Value2
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_Vertex                          *pst_3D;
    MATH_tdst_Vector                    *pst_VT[3];
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
    LONG                                l_Element, l_Triangle;
    ULONG								ul_Value2Element, ul_Value2Triangle;
	COL_tdst_Cob						*pst_Cob;
	COL_tdst_GameMat					*pst_GMat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	if(_pst_GO && _pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Ghost) return;
#endif

    if(_pst_PO->l_NbPoints < SOFT_Cul_ComputingBufferSize)
    {
        pst_3D = GDI_gpst_CurDD->pst_ComputingBuffers->ast_3D;

        SOFT_PickingBuffer_SetMatrix(GDI_gpst_CurDD->pst_PickingBuffer, &GDI_gpst_CurDD->st_Camera);
		if (GDI_gpst_CurDD ->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer)
			SOFT_TransformInComputingBuffer
			(
				GDI_gpst_CurDD->pst_ComputingBuffers,
				GDI_gpst_CurDD ->pst_ComputingBuffers->ast_SpecialVB,
				_pst_PO->l_NbPoints,
				&GDI_gpst_CurDD->st_Camera
			);
		else
			SOFT_TransformInComputingBuffer
			(
				GDI_gpst_CurDD->pst_ComputingBuffers,
				_pst_PO->dst_Point,
				_pst_PO->l_NbPoints,
				&GDI_gpst_CurDD->st_Camera
			);

		if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_ShowCurrentCOB)
			pst_Cob = *((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob;
		else
			pst_Cob = NULL;

        pst_Element = _pst_PO->dst_Element;
        pst_LastElement = pst_Element + _pst_PO->l_NbElements;
        l_Element = 0;

        if(GDI_gpst_CurDD->pst_PickingBuffer->ul_Flags & SOFT_Cul_PBF_PickWhileRender)
            SOFT_PickingBuffer_BeginPickingWhileRender(GDI_gpst_CurDD->pst_PickingBuffer, _ul_Value, 0);

        for(; pst_Element < pst_LastElement; pst_Element++)
        {
			pst_GMat = NULL;
			if(pst_Cob && pst_Cob->pst_TriangleCob)
			{
				pst_GMat = COL_pst_GMat_Get(pst_Cob, pst_Cob->pst_TriangleCob->dst_Element + (pst_Element - _pst_PO->dst_Element));
				
				if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera) && ((GDI_gpst_CurDD->uc_ColMapDisplayMode  & 0x7F) == 2))
				{
					l_Element++;
					continue;
				}

				if(pst_GMat && (pst_GMat->ul_CustomBits & (COL_Cul_GMat_FlagX | COL_Cul_GMat_Camera)) && ((GDI_gpst_CurDD->uc_ColMapDisplayMode  & 0x7F) == 3))
				{
					l_Element++;
					continue;
				}
				
				if(((GDI_gpst_CurDD->uc_ColMapDisplayMode  & 0x7F) == 1) && (!pst_GMat || (pst_GMat && !(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera))))
				{
  					l_Element++;
					continue;
				}
			}
			
            ul_Value2Element = _ul_Value2 | (((l_Element++) << SOFT_Cul_PBQF_ElementShift) & SOFT_Cul_PBQF_ElementMask);
            pst_Triangle = pst_Element->dst_Triangle;
            pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;
            l_Triangle = 0;

            for(; pst_Triangle < pst_LastTriangle; pst_Triangle++, l_Triangle++ )
            {
				if( pst_Triangle->ul_MaxFlags & 0x80000000 ) continue;
				
                ul_Value2Triangle = ul_Value2Element | (((l_Triangle) << SOFT_Cul_PBQF_TriangleShift) & SOFT_Cul_PBQF_TriangleMask);
				pst_VT[0] = VCast( pst_3D + pst_Triangle->auw_Index[0] );
				pst_VT[1] = VCast( pst_3D + pst_Triangle->auw_Index[1]);
				pst_VT[2] = VCast( pst_3D + pst_Triangle->auw_Index[2]);

				GDI_gpst_CurDD->pst_PickingBuffer->st_ClipInfo.ZMin = (GDI_gpst_CurDD->st_Camera.f_NearPlane * 0.05f);
				GDI_gpst_CurDD->pst_PickingBuffer->st_ClipInfo.ZMax = GDI_gpst_CurDD->st_Camera.f_FarPlane;
				SOFT_PickingBuffer_Render(GDI_gpst_CurDD->pst_PickingBuffer, pst_VT, _ul_Value, ul_Value2Triangle , GDI_gpst_CurDD ->ul_CurrentDrawMask );
            }
        }

        if(GDI_gpst_CurDD->pst_PickingBuffer->ul_Flags & SOFT_Cul_PBF_PickWhileRender)
            SOFT_PickingBuffer_EndPickingWhileRender(GDI_gpst_CurDD->pst_PickingBuffer);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_RenderTrace(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector				*pst_Pos, *pst_First, st_Pos;
    GDI_tdst_Request_DrawPointEx	st_Point;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Trace)
    {
        GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
		GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);

		pst_First = _pst_GO->pst_Trace->dst_Pos;

		// UPDATE
		if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_UpdateTrace)
		{
			_pst_GO->pst_Trace->l_Counter--;
			if( _pst_GO->pst_Trace->l_Counter <= 0 )
			{
				pst_Pos = pst_First + _pst_GO->pst_Trace->l_Number - 1;
				for(; pst_Pos >= pst_First; pst_Pos--)
					MATH_CopyVector(pst_Pos, pst_Pos - 1);
				MATH_CopyVector(pst_First, OBJ_pst_GetAbsolutePosition(_pst_GO));
				_pst_GO->pst_Trace->l_Counter = _pst_GO->pst_Trace->l_Delay;
			}
        }

		// RENDER
		if ( (_pst_GO->pst_Trace->l_NbDisplay < 0) || (_pst_GO->pst_Trace->l_NbDisplay > _pst_GO->pst_Trace->l_Number ) )
			pst_Pos = pst_First + _pst_GO->pst_Trace->l_Number - 1;
		else
			pst_Pos = pst_First + _pst_GO->pst_Trace->l_NbDisplay - 1;
			
		st_Point.A = &st_Pos;
		st_Point.f_Size = 5.0;
		st_Point.ul_Color = _pst_GO->pst_Trace->ul_Color;

		for(; pst_Pos >= pst_First; pst_Pos--)
		{
			MATH_CopyVector( &st_Pos, pst_Pos );
			st_Pos.z += _pst_GO->pst_Trace->f_ZOffset;
			
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointEx, (ULONG) &st_Point);
		}
    }
}

#endif

/*$T MATDraw.c GC! 1.081 04/13/00 10:47:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#define __MATDRAW_C__

#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"

#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"

#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"

#ifndef PSX2_TARGET
#ifndef _GAMECUBE
#ifdef ACTIVE_EDITORS
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#endif
#endif
#endif
#include "MATerial/MATSingle.h"
#include "MATerial/MATmulti.h"
#include "MATerial/MATSprite.h"
#include "MATerial/MATCompute.h"

#include "GRObject/GROstruct.h"
#include "GEOmetric/GEO_MRM.h"
#include "SOFT/SOFTlinear.h" 

#include "TIMer/TIMdefs.h"

#ifdef ACTIVE_EDITORS
#include "LINks/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "GEOmetric/GEOsubobject.h"
#include "GRObject/GROrender.h"
#include "GRObject/GROedit.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef PSX2_TARGET
#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>
#include "GS_PS2/Gsp.h"
#ifdef GSP_PS2_BENCH
extern u_int NoGEODRAW;
extern ULONG ShowNormals;
#endif
extern int renderState_Cloned;
#endif
#include "BASe/BENch/BENch.h"

#ifdef _GAMECUBE
#define USE_SOFT_UV_MATRICES
#include "GXI_GC/GXI_vertexspace.h"
#include "GXI_GC/GXI_def.h"
#endif
#ifdef _XBOX
#pragma message("Philippe -> Link resolve")
int g_MultipleVBIndex = 0;
#endif
ULONG OpenglCorrectBugMul2X = 0;
int renderState_Shfited = 0;
int NumberMat = 0;

#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/COLlision/COLvars.h"
extern COL_tdst_GlobalVars COL_gst_GlobalVars;
extern LONG OGL_l_DrawTriangle( GEO_tdst_ElementIndexedTriangles *, GEO_Vertex*, int);
extern LONG OGL_l_DrawSlopeTriangle(GEO_tdst_ElementIndexedTriangles*,GEO_Vertex*,int,MATH_tdst_Matrix*,GDI_tdst_DisplayData *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_DrawIndexedTriangle_ColMap
(
	GDI_tdst_DisplayData				*pst_CurDD,
	GEO_tdst_Object						*pst_Obj,
	MAT_tdst_Material					*_pst_Material,
	GEO_tdst_ElementIndexedTriangles	*pst_Element,
	MATH_tdst_Matrix					*pst_Matrix
)
{
	int				i;

	pst_CurDD->pst_ComputingBuffers->CurrentColorField = pst_CurDD->pst_ComputingBuffers->ComputedColors;
	pst_CurDD->pst_CurrentMat = (MAT_tdst_MultiTexture *) _pst_Material;
	MAT_SetRenderState(_pst_Material, pst_Element->l_MaterialId);

	pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseOneUVPerPoint;
	pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_FaceMap;
	pst_CurDD->pst_ComputingBuffers->Current = (SOFT_tdst_UV *) pst_Obj->dst_UV;
	if(GEO_MRM_ul_IsMrmObject(pst_Obj))
	{
		SOFT_tdst_UV	*SrcUV;
		unsigned short	*UsdIndx, *LastUsdIndx;

		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
		SrcUV = ((SOFT_tdst_UV *) pst_Obj->dst_UV) + pst_Element->p_MrmElementAdditionalInfo->ul_One_UV_Per_Point_Per_Element_Base;		
		
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)
		pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
		UsdIndx = pst_Element->pus_ListOfUsedIndex;
		LastUsdIndx = UsdIndx + pst_Element->ul_NumberOfUsedIndex;
		while(UsdIndx < LastUsdIndx) pst_CurDD->pst_ComputingBuffers->Current[*(UsdIndx++)] = *(SrcUV++);
#else
		{
			pst_CurDD->pst_ComputingBuffers->Current = (SOFT_tdst_UV*)GXI_GetVertexSpace(pst_Element->ul_NumberOfUsedIndex*sizeof(SOFT_tdst_UV));
			memcpy(pst_CurDD->pst_ComputingBuffers->Current, SrcUV, pst_Element->ul_NumberOfUsedIndex*sizeof(SOFT_tdst_UV));
			DCFlushRange(pst_CurDD->pst_ComputingBuffers->Current, GDI_gpst_CurDD->pst_CurrentGeo->l_NbUVs*2*4);
		}
#endif		
		
	}
	for(i = 0; i < pst_Element->l_NbTriangles; i++)
	{
		OGL_l_DrawSlopeTriangle(pst_Element, pst_CurDD->p_Current_Vertex_List, i, pst_Matrix, pst_CurDD);
	}
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_DrawIndexedTriangle_ST
(
	GDI_tdst_DisplayData				*pst_CurDD,
	GEO_tdst_Object						*pst_Obj,
	MAT_tdst_Material					*_pst_Material,
	GEO_tdst_ElementIndexedTriangles	*pst_Element
)
{

	PRO_StartTrameRaster(&pst_CurDD->pst_Raster->st_MatDIT_SingleMaterial);
	pst_CurDD->pst_ComputingBuffers->CurrentColorField = pst_CurDD->pst_ComputingBuffers->ComputedColors;
	pst_CurDD->pst_CurrentMat = (MAT_tdst_MultiTexture *) _pst_Material;
	PRO_StartTrameRaster(&pst_CurDD->pst_Raster->st_MatDIT_SetRenderState);
	MAT_SetRenderState(_pst_Material, pst_Element->l_MaterialId);
	PRO_StopTrameRaster(&pst_CurDD->pst_Raster->st_MatDIT_SetRenderState);

	pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseOneUVPerPoint;
	pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_FaceMap;
	pst_CurDD->pst_ComputingBuffers->Current = (SOFT_tdst_UV *) pst_Obj->dst_UV;
	if(GEO_MRM_ul_IsMrmObject(pst_Obj))
	{
		SOFT_tdst_UV	*SrcUV;
		unsigned short	*UsdIndx, *LastUsdIndx;

		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
		SrcUV = ((SOFT_tdst_UV *) pst_Obj->dst_UV) + pst_Element->p_MrmElementAdditionalInfo->ul_One_UV_Per_Point_Per_Element_Base;		
		
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)
		pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
		UsdIndx = pst_Element->pus_ListOfUsedIndex;
		LastUsdIndx = UsdIndx + pst_Element->ul_NumberOfUsedIndex;
		while(UsdIndx < LastUsdIndx) pst_CurDD->pst_ComputingBuffers->Current[*(UsdIndx++)] = *(SrcUV++);
#else
		{
			pst_CurDD->pst_ComputingBuffers->Current = (SOFT_tdst_UV*)GXI_GetVertexSpace(pst_Element->ul_NumberOfUsedIndex*sizeof(SOFT_tdst_UV));
			memcpy(pst_CurDD->pst_ComputingBuffers->Current, SrcUV, pst_Element->ul_NumberOfUsedIndex*sizeof(SOFT_tdst_UV));
			DCFlushRange(pst_CurDD->pst_ComputingBuffers->Current, GDI_gpst_CurDD->pst_CurrentGeo->l_NbUVs*2*4);
		}
#endif		
		
	}
	PRO_StartTrameRaster(&pst_CurDD->pst_Raster->st_MatDIT_DrawTriangles);
	GDI_DrawIndexedTriangles(
				GDI_gpst_CurDD_SPR,
				pst_Element,
				pst_CurDD->p_Current_Vertex_List,
				pst_Obj->dst_PointNormal,
				(GEO_tdst_UV *) pst_CurDD->pst_ComputingBuffers->Current,
				pst_Obj->l_NbPoints
			);
	PRO_StopTrameRaster(&pst_CurDD->pst_Raster->st_MatDIT_DrawTriangles);
	PRO_StopTrameRaster(&pst_CurDD->pst_Raster->st_MatDIT_SingleMaterial);//*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
extern unsigned int NoMATDRAW;
extern unsigned int NoMulti;
extern unsigned int NoSDW;
extern unsigned int NoSPR;
#endif
#ifdef _GAMECUBE
#define ShowNormals NoMulti
#endif


void MAT_DrawIndexedTriangle_MT
(
	GDI_tdst_DisplayData				*pst_CurDD,
	GEO_tdst_Object						*pst_Obj,
	MAT_tdst_Material					*_pst_Material,
	GEO_tdst_ElementIndexedTriangles	*pst_Element
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_MultiTexture	*pst_MLTTX;
	MAT_tdst_MTLevel		*pst_MLTTXLVL;
	register SOFT_tdst_ComputingBuffers	*p_CptBf;
	ULONG 					bIsSpriteGen;
	BOOL					bActiveLayer;	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	p_CptBf = pst_CurDD->pst_ComputingBuffers;
	pst_CurDD->pst_CurrentMat = pst_MLTTX = (MAT_tdst_MultiTexture *) _pst_Material;
	pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;
	NumberMat = 0;
#ifdef PSX2_TARGET
	if(pst_MLTTXLVL)
#else	
	if((pst_MLTTXLVL == NULL) || (!(pst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_UseTexture)))
	{
//#ifdef JADEFUSION
		pst_CurDD->pst_CurrentMLTTXLVL = pst_MLTTXLVL;
//#endif		
		p_CptBf->ulColorXor = 0;
		p_CptBf->Current = NULL;
		p_CptBf->ul_Ambient = 0;
		p_CptBf->CurrentAlphaField = NULL;

		p_CptBf->CurrentColorField = p_CptBf->ComputedColors;
		pst_CurDD->pst_CurrentMat = (MAT_tdst_MultiTexture *) _pst_Material;
		
#ifdef _GAMECUBE		
//		if(GXI_Global_ACCESS(bPutObjectInObjectList))
//			GXI_ObjectList_StartCurrent_DLStates();
#endif			
			
		pst_CurDD->st_GDI.pfnv_SetTextureBlending((ULONG) - 1, MAT_C_DefaultFlag , 0);
		PRO_IncRasterLong(&GDI_gpst_CurDD_SPR.pst_Raster->st_NbTriangles, pst_Element->l_NbTriangles);
		GDI_DrawIndexedTriangles((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,pst_Obj->dst_PointNormal,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints);
	}
	else//*/
#endif	
	{
#ifndef PSX2_TARGET
		pst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_DoNotSort;
#endif
/*#ifdef PSX2_TARGET
		{
			extern void Gsp_SetNormalsPrepare(ULONG *pNormals);
			Gsp_SetNormalsPrepare(pst_Obj -> p_CompressedNormals);
		}
#endif*/
		bActiveLayer = FALSE;
		NumberMat = 0;
		while (pst_MLTTXLVL && (LONG)pst_MLTTXLVL != -1)
		{
			if(!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_InActive))
			{
//#ifdef JADEFUSION
				pst_CurDD->pst_CurrentMLTTXLVL = pst_MLTTXLVL;
//#endif
				bActiveLayer = TRUE;
				PRO_IncRasterLong(&GDI_gpst_CurDD_SPR.pst_Raster->st_NbTriangles, pst_Element->l_NbTriangles);
				bIsSpriteGen = 0;
				if ((pst_MLTTXLVL->s_TextureId != -1) && (TEX_gst_GlobalList.dst_Texture[pst_MLTTXLVL->s_TextureId].uw_Flags & TEX_uw_IsSpriteGen))
					bIsSpriteGen = 1;

				p_CptBf->ulColorXor = 0;
				p_CptBf->Current = NULL;
				p_CptBf->CurrentColorField = NULL;
				p_CptBf->ul_Ambient = 0;
				p_CptBf->CurrentAlphaField = NULL;
				
				switch(MAT_GET_ColorOp(pst_MLTTXLVL->ul_Flags))
				{
				case MAT_Cc_ColorOp_Disable: // Constant 
					p_CptBf->ul_Ambient = pst_MLTTX->ul_Specular & 0xffffff;
 					break;

				case MAT_Cc_ColorOp_Specular:
					p_CptBf->CurrentColorField = p_CptBf->aul_Specular;
                    if (!(GDI_gpst_CurDD->pst_ComputingBuffers->ul_Flags & SOFT_Cul_CB_SpecularColorField))
					{
						p_CptBf->ul_Ambient = 0;
						p_CptBf->CurrentColorField = NULL;
					}
					break;

				case MAT_Cc_ColorOp_InvertDiffuse:
					p_CptBf->ulColorXor = 0xffffff;
				case MAT_Cc_ColorOp_RLI:
					p_CptBf->CurrentColorField = pst_CurDD->pul_RLI_Used;
					break;

				case MAT_Cc_ColorOp_FullLight:
					p_CptBf->ul_Ambient = 0xffffff;
					break;
				case MAT_Cc_ColorOp_Diffuse2X:
				case MAT_Cc_ColorOp_Diffuse:
#ifdef PSX2_TARGET				
					Gsp_SetDiffuse2XMode();
#endif					
					p_CptBf->CurrentColorField = p_CptBf->ComputedColors;
					break;

				}//*/
#ifdef GSP_PS2_BENCH
				if ((ShowNormals) && ((ShowNormals & 3) != 2))
				{
					if (pst_Obj -> p_CompressedNormals)
							p_CptBf->CurrentColorField = pst_Obj -> p_CompressedNormals;
					else return;
				}
#endif
				
				if(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_UseLocalAlpha)
				{
					p_CptBf->ulColorXor |= (((ULONG) (pst_MLTTXLVL->s_AditionalFlags)) << 16) & 0xff000000;
					p_CptBf->ulColorXor ^= 0xff000000;
				}//*/
				
#ifdef _GAMECUBE		
//				if(GXI_Global_ACCESS(bPutObjectInObjectList))
//					GXI_ObjectList_StartCurrent_DLStates();
#endif			

#ifdef PSX2_TARGET
				{ 
					extern LONG GSP_DrawIndexedTriangles_Clone(	GEO_tdst_Object						*pst_Obj, GEO_tdst_ElementIndexedTriangles	*_pst_Element,	GEO_Vertex				*_pst_Point,	GEO_tdst_UV				*_pst_UV,	ULONG 					ulNbPoints,	MAT_tdst_MTLevel		*pst_MLTTXLVL);
					GDI_SetTextureBlending((*pst_CurDD),pst_MLTTXLVL->s_TextureId, pst_MLTTXLVL->ul_Flags , 0 );
#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
					if (!NoGEODRAW)
#endif				
						MAT_ComputeUV_NoStore(pst_Obj, pst_Element, pst_MLTTXLVL,pst_CurDD);
					if (renderState_Cloned)
						GSP_DrawIndexedTriangles_Clone(pst_Obj,pst_Element,pst_CurDD->p_Current_Vertex_List,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints,pst_MLTTXLVL);
					else
					if (pst_MLTTX->ul_ValidateMask & MAT_ValidateMask_Fur)
					{
						if ( pst_MLTTXLVL->s_AditionalFlags & MAT_XYZ_Flag_ShiftUsingNormal )
						{
							GEO_UseNormals(pst_Obj); // 
							GSP_l_DrawElementIndexedTriangles_FUR(pst_Element,	pst_CurDD->p_Current_Vertex_List,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints , pst_Obj->p_CompressedNormals);
							pst_MLTTX->ul_ValidateMask |= MAT_ValidateMask_Fur;
						} else
						{
							GEO_UseNormals(pst_Obj); // 
							GSP_l_DrawElementIndexedTriangles_FUR_FAKE(pst_Element,	pst_CurDD->p_Current_Vertex_List,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints , pst_Obj->p_CompressedNormals);
						}
					} else//*/
						GDI_DrawIndexedTriangles((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,NULL,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints);
				}
#else					
				GDI_SetTextureBlending((*pst_CurDD),pst_MLTTXLVL->s_TextureId, pst_MLTTXLVL->ul_Flags , 0);
				MAT_ComputeUV(pst_Obj, pst_Element, pst_MLTTXLVL );
				/*
				if(MAT_GET_s_Flags(pst_MLTTXLVL->s_AditionalFlags) & MAT_Cul_sFlag_DeductAlpha) 
				{
					MAT_ComputeDeductedAlpha(pst_Obj, pst_MLTTXLVL);
				}
				*/
				if ( pst_MLTTXLVL->s_AditionalFlags & MAT_XYZ_Flag_ShiftUsingNormal )
					renderState_Shfited = 1;
				else
					renderState_Shfited = 0;

				if (bIsSpriteGen)
				{
					MAT_RasterizeUV(pst_CurDD, pst_Obj, pst_MLTTXLVL, pst_Element);
				}
				else//*/
				{
                    /*$F test bump
                    if ( pst_CurDD->ul_DisplayInfo & 0x80000000 )
                    {
                        GDI_DrawIndexedTriangles((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints);
                        
                        if ( !LIGHT_gpst_Bump )
                        {
                            GDI_SetTextureBlending((*pst_CurDD),pst_MLTTXLVL->s_TextureId, (pst_MLTTXLVL->ul_Flags & ~MAT_Cc_Blending_MASK) | (MAT_Cc_Op_Add << MAT_Cc_Blending_SHIFT));
                            GDI_DrawIndexedTriangles((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints);
                        }
                        else
                        {
                            static float f = 0;
                            float t;
                            t = (LIGHT_gpst_Bump ) ? 0.004f : 0; 
                            f += 0.05f;
                            MAT_SetScale( &pst_MLTTXLVL->ScaleSPeedPosU, 1.0f );
                            MAT_SetScale( &pst_MLTTXLVL->ScaleSPeedPosV, 1.0f );
                            MAT_SetPos( &pst_MLTTXLVL->ScaleSPeedPosU, t * fSin(f) );
                            MAT_SetPos( &pst_MLTTXLVL->ScaleSPeedPosV, t * fCos(f) );
                            MAT_ComputeUV(pst_Obj, pst_Element, pst_MLTTXLVL );
                            GDI_SetTextureBlending((*pst_CurDD),pst_MLTTXLVL->s_TextureId, (pst_MLTTXLVL->ul_Flags & ~MAT_Cc_Blending_MASK) | (MAT_Cc_Op_Add << MAT_Cc_Blending_SHIFT));
                            GDI_DrawIndexedTriangles((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints);
                            MAT_SetPos( &pst_MLTTXLVL->ScaleSPeedPosU, 0 ); 
                            MAT_SetPos( &pst_MLTTXLVL->ScaleSPeedPosV, 0 );
                        }
                        pst_CurDD->ul_DisplayInfo &= ~0x80000000;
                    }
                    else
                    */
                    {
						GEO_UseNormals(pst_Obj); // OK
						GDI_DrawIndexedTriangles((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,pst_Obj->dst_PointNormal, (GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints);
#ifndef _GAMECUBE
						if (((MAT_GET_ColorOp(pst_MLTTXLVL->ul_Flags)) == MAT_Cc_ColorOp_Diffuse2X) && (!GDI_gpst_CurDD->GlobalMul2X))
						{
#ifdef ACTIVE_EDITORS
//							if(LINK_gi_SpeedDraw != 1)								
#endif
							{

								PRO_IncRasterLong(&GDI_gpst_CurDD_SPR.pst_Raster->st_NbTriangles, pst_Element->l_NbTriangles);
								if (MAT_GET_Blending(pst_MLTTXLVL->ul_Flags) == MAT_Cc_Op_Alpha) OpenglCorrectBugMul2X = 1;
								GDI_SetTextureBlending((*pst_CurDD),pst_MLTTXLVL->s_TextureId, (pst_MLTTXLVL->ul_Flags & ~MAT_Cc_Blending_MASK) | (MAT_Cc_Op_Add << MAT_Cc_Blending_SHIFT) , 0) ;
								GDI_DrawIndexedTriangles((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,pst_Obj->dst_PointNormal,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints);
								OpenglCorrectBugMul2X = 0;
							}
						}
#endif					
					}
				}
#endif
			}

			pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
			NumberMat++;

/*#ifdef PSX2_TARGET
		{
			extern void Gsp_SetNormals(ULONG *pNormals);
			Gsp_SetNormals(pst_Obj -> p_CompressedNormals);
		}
#endif*/
			if (pst_CurDD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap) *(LONG *)&pst_MLTTXLVL = -1;
			
#if defined(GSP_PS2_BENCH)  || (defined(_GAMECUBE) && !defined(_FINAL_))
			if (NoMulti) *(LONG *)&pst_MLTTXLVL = -1;
#endif		
#ifdef ACTIVE_EDITORS
			if(bActiveLayer && (LINK_gi_SpeedDraw == 1)) break;
#endif
		}
	}
#ifdef PSX2_TARGET
		{
/*			if (renderState_Cloned)
						GSP_DrawIndexedTriangles_Clone(NULL,NULL,NULL, NULL, 1);*/
		}
#endif
	pst_CurDD->ul_DisplayInfo &= ~(GDI_Cul_DI_FaceMap|GDI_Cul_DI_UseOneUVPerPoint);
#ifndef PSX2_TARGET
	p_CptBf->Current = NULL;
	pst_CurDD->st_GDI.pfnv_SetTextureBlending((ULONG) - 1, MAT_C_DefaultFlag , 0);
#endif	

#if defined(_XENON_RENDER)
    pst_CurDD->pst_CurrentMat = NULL;
#endif
}


#ifdef ACTIVE_EDITORS
void MAT_DrawIndexedTriangleZone
(
	GEO_tdst_Object						*pst_Obj,
	GEO_tdst_ElementIndexedTriangles	*pst_Element,
    BOOL bHasTransparency
)
{
    MAT_tdst_Material	*pst_Material;
    
	_GSP_BeginRaster(12);

	pst_Material = (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

    {
        GDI_tdst_DisplayData				*pst_CurDD = &GDI_gpst_CurDD_SPR ;

        MAT_tdst_MultiTexture	*pst_MLTTX;
        MAT_tdst_MTLevel		*pst_MLTTXLVL;
        register SOFT_tdst_ComputingBuffers	*p_CptBf;
        ULONG 					bIsSpriteGen;
        BOOL					bActiveLayer;	

        p_CptBf = pst_CurDD->pst_ComputingBuffers;
        pst_CurDD->pst_CurrentMat = pst_MLTTX = (MAT_tdst_MultiTexture *) pst_Material;
        pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;
		NumberMat = 0;

        if((pst_MLTTXLVL == NULL) || (!(pst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_UseTexture)))
        {
            p_CptBf->ulColorXor = 0;
            p_CptBf->Current = NULL;
            p_CptBf->ul_Ambient = 0;
            p_CptBf->CurrentAlphaField = NULL;

            p_CptBf->CurrentColorField = p_CptBf->ComputedColors;
            pst_CurDD->pst_CurrentMat = (MAT_tdst_MultiTexture *) pst_Material;

            if (bHasTransparency)
                pst_CurDD->st_GDI.pfnv_SetTextureBlending((ULONG) - 1, MAT_C_DefaultFlag | (MAT_Cc_Blending_MASK & (MAT_Cc_Op_Alpha << MAT_Cc_Blending_SHIFT)) , 0);
            else
                pst_CurDD->st_GDI.pfnv_SetTextureBlending((ULONG) - 1, MAT_C_DefaultFlag , 0);

            PRO_IncRasterLong(&GDI_gpst_CurDD_SPR.pst_Raster->st_NbTriangles, pst_Element->l_NbTriangles);
            GDI_DrawIndexedTriangles((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,pst_Obj->dst_PointNormal,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints);
        }
        else
        {
            pst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_DoNotSort;
            bActiveLayer = FALSE;
			while (pst_MLTTXLVL && (LONG)pst_MLTTXLVL != -1)
            {
                if(!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_InActive))
                {
                    bActiveLayer = TRUE;
                    PRO_IncRasterLong(&GDI_gpst_CurDD_SPR.pst_Raster->st_NbTriangles, pst_Element->l_NbTriangles);
                    bIsSpriteGen = 0;
                    if ((pst_MLTTXLVL->s_TextureId != -1) && (TEX_gst_GlobalList.dst_Texture[pst_MLTTXLVL->s_TextureId].uw_Flags & TEX_uw_IsSpriteGen))
                        bIsSpriteGen = 1;

                    p_CptBf->ulColorXor = 0;
                    p_CptBf->Current = NULL;
                    p_CptBf->CurrentColorField = NULL;
                    p_CptBf->ul_Ambient = 0;
                    p_CptBf->CurrentAlphaField = NULL;

                    switch(MAT_GET_ColorOp(pst_MLTTXLVL->ul_Flags))
                    {
                    case MAT_Cc_ColorOp_Disable: // Constant 
                        p_CptBf->ul_Ambient = pst_MLTTX->ul_Specular & 0xffffff;
                        break;

                    case MAT_Cc_ColorOp_Specular:
                        p_CptBf->CurrentColorField = p_CptBf->aul_Specular;
                        if (!(GDI_gpst_CurDD->pst_ComputingBuffers->ul_Flags & SOFT_Cul_CB_SpecularColorField))
                        {
                            p_CptBf->ul_Ambient = 0;
                            p_CptBf->CurrentColorField = NULL;
                        }
                        break;

                    case MAT_Cc_ColorOp_InvertDiffuse:
                        p_CptBf->ulColorXor = 0xffffff;
                    case MAT_Cc_ColorOp_RLI:
                        p_CptBf->CurrentColorField = pst_CurDD->pul_RLI_Used;
                        break;

                    case MAT_Cc_ColorOp_FullLight:
                        p_CptBf->ul_Ambient = 0xffffff;
                        break;
                    case MAT_Cc_ColorOp_Diffuse2X:
                    case MAT_Cc_ColorOp_Diffuse:
                        p_CptBf->CurrentColorField = p_CptBf->ComputedColors;
                        break;

                    }

                    if(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_UseLocalAlpha)
                    {
                        p_CptBf->ulColorXor |= (((ULONG) (pst_MLTTXLVL->s_AditionalFlags)) << 16) & 0xff000000;
                        p_CptBf->ulColorXor ^= 0xff000000;
                    }				
                    GDI_SetTextureBlending((*pst_CurDD),pst_MLTTXLVL->s_TextureId, pst_MLTTXLVL->ul_Flags , 0);
                    MAT_ComputeUV(pst_Obj, pst_Element, pst_MLTTXLVL );
					if ( pst_MLTTXLVL->s_AditionalFlags & MAT_XYZ_Flag_ShiftUsingNormal )
                        renderState_Shfited = 1;
                    else
                        renderState_Shfited = 0;

                    if (bIsSpriteGen)
                    {
                        MAT_RasterizeUV(pst_CurDD, pst_Obj, pst_MLTTXLVL, pst_Element);
                    }
                    else
                    {
                        GEO_UseNormals(pst_Obj); // OK
                        GDI_DrawIndexedTriangles((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,pst_Obj->dst_PointNormal, (GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints);
                        if (((MAT_GET_ColorOp(pst_MLTTXLVL->ul_Flags)) == MAT_Cc_ColorOp_Diffuse2X) && (!GDI_gpst_CurDD->GlobalMul2X))
                        {

                            PRO_IncRasterLong(&GDI_gpst_CurDD_SPR.pst_Raster->st_NbTriangles, pst_Element->l_NbTriangles);
                            if (MAT_GET_Blending(pst_MLTTXLVL->ul_Flags) == MAT_Cc_Op_Alpha) OpenglCorrectBugMul2X = 1;
                            GDI_SetTextureBlending((*pst_CurDD),pst_MLTTXLVL->s_TextureId, (pst_MLTTXLVL->ul_Flags & ~MAT_Cc_Blending_MASK) | (MAT_Cc_Op_Add << MAT_Cc_Blending_SHIFT) , 0) ;
                            GDI_DrawIndexedTriangles((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,pst_Obj->dst_PointNormal,(GEO_tdst_UV *) p_CptBf->Current , pst_Obj->l_NbPoints);
                            OpenglCorrectBugMul2X = 0;
                        }
                    }
                }

                pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
				NumberMat++;

                if (pst_CurDD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap) *(LONG *)&pst_MLTTXLVL = -1;

                if(bActiveLayer && (LINK_gi_SpeedDraw == 1)) 
                    break;
            }
        }
        pst_CurDD->ul_DisplayInfo &= ~(GDI_Cul_DI_FaceMap|GDI_Cul_DI_UseOneUVPerPoint);
        p_CptBf->Current = NULL;
        pst_CurDD->st_GDI.pfnv_SetTextureBlending((ULONG) - 1, MAT_C_DefaultFlag , 0);

    }
    _GSP_EndRaster(12);

    GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ulColorXor = 0;

}

#endif //ACTIVE_EDITORS

/*
=======================================================================================================================
=======================================================================================================================
*/

void MAT_DrawIndexedTriangle
(
 GEO_tdst_Object						*pst_Obj,
 MAT_tdst_Material					*_pst_Material,
 GEO_tdst_ElementIndexedTriangles	*pst_Element
 )
{

#ifdef _GAMECUBE 
    if((!pst_Element->l_NbTriangles) && (!pst_Element->dl) && (!pst_Element->pst_StripData)) return;
#else	
#ifdef PSX2_TARGET 
    if((!pst_Element->l_NbTriangles) && (!pst_Element->pst_StripDataPS2)) return;
#else	
    if(!pst_Element->l_NbTriangles) return;
#endif	
#endif	




#if (defined(PSX2_TARGET) && defined(GSP_PS2_BENCH)) || (defined(_GAMECUBE) && !defined(_FINAL_))
    if (NoMATDRAW) return;
#endif	

    _GSP_BeginRaster(12);

    PRO_StartTrameRaster(&GDI_gpst_CurDD_SPR.pst_Raster->st_MatDIT_PrepareMaterial);

    if(_pst_Material  && (_pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti))
    {
        if(((MAT_tdst_Multi *)_pst_Material)->l_NumberOfSubMaterials == 0)
            _pst_Material = NULL;
        else
            _pst_Material = ((MAT_tdst_Multi *)_pst_Material)->dpst_SubMaterial[lMin(pst_Element->l_MaterialId , ((MAT_tdst_Multi *)_pst_Material)->l_NumberOfSubMaterials - 1)];
    }

    if(_pst_Material == NULL) _pst_Material = (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

    //#ifdef _GAMECUBE 
    // unconditionnal rendering 
    //#else
    if((!(GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency)))
    {
        if(MAT_IsMaterialTransparent(_pst_Material, -1, GDI_gpst_CurDD_SPR.ul_CurrentDrawMask, NULL)) 
        {
            PRO_StopTrameRaster(&GDI_gpst_CurDD_SPR.pst_Raster->st_MatDIT_PrepareMaterial);

            _GSP_EndRaster(12);

            return;
        }
    }
    else if(!MAT_IsMaterialTransparent(_pst_Material, -1, GDI_gpst_CurDD_SPR.ul_CurrentDrawMask, NULL)) 
    {
        PRO_StopTrameRaster(&GDI_gpst_CurDD_SPR.pst_Raster->st_MatDIT_PrepareMaterial);

        _GSP_EndRaster(12);

        return;
    }
    //#endif//*/

    PRO_StopTrameRaster(&GDI_gpst_CurDD_SPR.pst_Raster->st_MatDIT_PrepareMaterial);

#ifdef PSX2_TARGET
    /*	Gsp_SetUVMatrix_Identity();
    Gsp_SetPlanarProjectionMode(0);*/
    GDI_gpst_CurDD_SPR.pst_CurrentMat = (MAT_tdst_MultiTexture *) _pst_Material;
#endif		
    //*/

    //    PRO_IncRasterLong(&GDI_gpst_CurDD_SPR.pst_Raster->st_NbTriangles, pst_Element->l_NbTriangles);

#ifdef ACTIVE_EDITORS
    if	((GDI_gpst_CurDD_SPR.pst_CurrentGameObject) &&	(GDI_gpst_CurDD_SPR.pst_CurrentGameObject->ul_EditorFlags & OBJ_C_EditFlags_Selected)&&pst_Obj->pst_SubObject)
    {
        if(GDI_gpst_CurDD_SPR.pst_EditOptions->ul_Flags & GRO_Cul_EOF_SkinMode)
        {
            if((pst_Obj->p_SKN_Objectponderation) && (pst_Obj->p_SKN_Objectponderation->dul_PointColors) && (pst_Obj->p_SKN_Objectponderation->SelectionCLBK) )
            {
                GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentColorField = pst_Obj->p_SKN_Objectponderation->dul_PointColors;
                GDI_gpst_CurDD_SPR.st_GDI.pfnv_SetTextureBlending((ULONG) - 1, MAT_C_DefaultFlag , 0);
                GDI_DrawIndexedTriangles(GDI_gpst_CurDD_SPR,pst_Element,GDI_gpst_CurDD_SPR.p_Current_Vertex_List,pst_Obj->dst_PointNormal,(GEO_tdst_UV *) GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current , pst_Obj->l_NbPoints);
                return;
            }
        }
    }
#endif

#ifdef _GAMECUBE

    GDI_gpst_CurDD->pst_CurrentGeo = pst_Obj;

#endif

    if(_pst_Material->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
    {
        MAT_DrawIndexedTriangle_MT(&GDI_gpst_CurDD_SPR , pst_Obj,_pst_Material,pst_Element);
    } else
        if(_pst_Material->st_Id.i->ul_Type == GRO_MaterialSingle)
        {
            PRO_IncRasterLong(&GDI_gpst_CurDD_SPR.pst_Raster->st_NbTriangles, pst_Element->l_NbTriangles);
            MAT_DrawIndexedTriangle_ST(&GDI_gpst_CurDD_SPR , pst_Obj,_pst_Material,pst_Element);
        }

        _GSP_EndRaster(12);

        GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ulColorXor = 0;

}



void MAT_DrawIndexedSprites_MT
(
 GDI_tdst_DisplayData				*pst_CurDD,
 GEO_tdst_Object						*pst_Obj,
 MAT_tdst_Material					*_pst_Material,
 GEO_tdst_ElementIndexedSprite    	*pst_Element
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_MultiTexture	*pst_MLTTX;
    MAT_tdst_MTLevel		*pst_MLTTXLVL;
    SOFT_tdst_ComputingBuffers	*p_CptBf;
    ULONG 						bIsSpriteGen;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
    if(LINK_gi_SpeedDraw == 1) return;
#endif

    p_CptBf = pst_CurDD->pst_ComputingBuffers;
    pst_MLTTX = (MAT_tdst_MultiTexture *) _pst_Material;		

    pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;
	NumberMat = 0;
    if(pst_MLTTXLVL)
    {	
        p_CptBf->CurrentColorField = p_CptBf->ComputedColors;
        pst_CurDD->pst_CurrentMat = (MAT_tdst_MultiTexture *)_pst_Material;
        pst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_DoNotSort;
        PRO_StopTrameRaster(&pst_CurDD->pst_Raster->st_MatDIT_MultiTexture);
        PRO_StartTrameRaster(&pst_CurDD->pst_Raster->st_MatDIT_MultiTexture);

        p_CptBf->ul_Ambient = pst_MLTTX->ul_Specular;
        while (pst_MLTTXLVL && (LONG)pst_MLTTXLVL != -1)
        {
//#ifdef JADEFUSION
			pst_CurDD->pst_CurrentMLTTXLVL = pst_MLTTXLVL;
//#endif
			p_CptBf->CurrentAlphaField = NULL;
            if(!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_InActive))
            {
                p_CptBf->ulColorXor = 0;

                bIsSpriteGen = 0;
                if ((pst_MLTTXLVL->s_TextureId != -1) && (TEX_gst_GlobalList.dst_Texture[pst_MLTTXLVL->s_TextureId].uw_Flags & TEX_uw_IsSpriteGen))
#if defined(_GAMECUBE) || defined(PSX2_TARGET)
                    goto MAT_NstLvl;
#else
                    bIsSpriteGen = 1;
#endif					

                switch(MAT_GET_ColorOp(pst_MLTTXLVL->ul_Flags))
                {
                case MAT_Cc_ColorOp_Disable: // Constant 
                    p_CptBf->ul_Ambient = pst_MLTTX->ul_Specular & 0xffffff;
                    p_CptBf->CurrentColorField = NULL;
                    break;

                case MAT_Cc_ColorOp_Specular:
                    p_CptBf->CurrentColorField = p_CptBf->aul_Specular;
                    if (!(GDI_gpst_CurDD->pst_ComputingBuffers->ul_Flags & SOFT_Cul_CB_SpecularColorField))
                    {
                        p_CptBf->ul_Ambient = 0;
                        p_CptBf->CurrentColorField = NULL;
                    }
                    break;

                case MAT_Cc_ColorOp_InvertDiffuse:
                    p_CptBf->ulColorXor = 0xffffff;
                case MAT_Cc_ColorOp_RLI:
                    p_CptBf->CurrentColorField = pst_CurDD->pul_RLI_Used;
                    break;

                case MAT_Cc_ColorOp_FullLight:
                    p_CptBf->ul_Ambient = 0xffffff;
                    p_CptBf->CurrentColorField = NULL;
                    break;
                case MAT_Cc_ColorOp_Diffuse2X:
                case MAT_Cc_ColorOp_Diffuse:
#ifdef PSX2_TARGET
#ifdef USE_GO_DATA
                    // Delete Colors from cache in case of sprite.
                    if (pst_Obj->dst_Element)
                    {
                        if (pst_Obj->dst_Element->pst_StripDataPS2)
                        {
                            if (GDI_gpst_CurDD->pst_CurrentGameObject)
                            {
                                if (GDI_gpst_CurDD->pst_CurrentGameObject->DataCacheOptimization)
                                {
                                    extern void GSP_Free_DC_DATA(void*);
                                    GSP_Free_DC_DATA((void  *)GDI_gpst_CurDD->pst_CurrentGameObject->DataCacheOptimization);
                                    GDI_gpst_CurDD->pst_CurrentGameObject->DataCacheOptimization = NULL;
                                }
                            }
                        }
                    }
#endif
#endif				
                    p_CptBf->CurrentColorField = p_CptBf->ComputedColors;
                    break;

                }//*/
                p_CptBf->Current = NULL;


				GDI_SetTextureBlending((*pst_CurDD),pst_MLTTXLVL->s_TextureId, pst_MLTTXLVL->ul_Flags , 0);
				
				if(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_UseLocalAlpha)
				{
					p_CptBf->ulColorXor |= (((ULONG) (pst_MLTTXLVL->s_AditionalFlags)) << 16) & 0xff000000;
					p_CptBf->ulColorXor ^= 0xff000000;
				}//*/

				GDI_DrawIndexedSprites((*pst_CurDD),pst_Element,pst_CurDD->p_Current_Vertex_List,pst_Obj->l_NbPoints);
			}
#if defined(_GAMECUBE) || defined(PSX2_TARGET)
MAT_NstLvl:
#endif
			pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
			NumberMat++;
#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
			if (NoMulti) *(LONG *)&pst_MLTTXLVL = -1;
#endif			
		}
	}
	pst_CurDD->ul_DisplayInfo &= ~(GDI_Cul_DI_FaceMap|GDI_Cul_DI_UseOneUVPerPoint);
#ifndef PSX2_TARGET
	p_CptBf->Current = NULL;
	pst_CurDD->st_GDI.pfnv_SetTextureBlending((ULONG) - 1, MAT_C_DefaultFlag , 0);
	PRO_StopTrameRaster(&pst_CurDD->pst_Raster->st_MatDIT_MultiTexture);
#endif	
#if defined(_XENON_RENDER)
    pst_CurDD->pst_CurrentMat = NULL;
#endif
}

void MAT_DrawIndexedSprites(
	GEO_tdst_Object					*pst_Obj,
	MAT_tdst_Material				*_pst_Material,
	GEO_tdst_ElementIndexedSprite	*pst_Element)
{
#ifdef ACTIVE_EDITORS
	if(LINK_gi_SpeedDraw == 1) return;
#endif

	if(!pst_Element->l_NbSprites) return;
	
	_GSP_BeginRaster(12);

	PRO_StartTrameRaster(&GDI_gpst_CurDD_SPR.pst_Raster->st_MatDIT_PrepareMaterial);

	if(_pst_Material  && (_pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti))
	{
		_pst_Material = ((MAT_tdst_Multi *)_pst_Material)->dpst_SubMaterial[lMin(pst_Element->l_MaterialId , ((MAT_tdst_Multi *)_pst_Material)->l_NumberOfSubMaterials - 1)];
	}

	if(_pst_Material == NULL) _pst_Material = (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;


	PRO_StopTrameRaster(&GDI_gpst_CurDD_SPR.pst_Raster->st_MatDIT_PrepareMaterial);
/*
	if((!(GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency)))
	{
		if(MAT_IsMaterialTransparent(_pst_Material, -1, GDI_gpst_CurDD_SPR.ul_CurrentDrawMask, NULL)) 
		{
			_GSP_EndRaster(12);
			return;
		}
	}
	else if(!MAT_IsMaterialTransparent(_pst_Material, -1, GDI_gpst_CurDD_SPR.ul_CurrentDrawMask, NULL)) 
	{
		_GSP_EndRaster(12);
		return;
	}//*/

#ifdef PSX2_TARGET
	GDI_gpst_CurDD_SPR.pst_CurrentMat = (MAT_tdst_MultiTexture *) _pst_Material;
#endif		

	if(_pst_Material->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
	{
		MAT_DrawIndexedSprites_MT(&GDI_gpst_CurDD_SPR , pst_Obj,_pst_Material,pst_Element);
	} 
	GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ulColorXor = 0;

	_GSP_EndRaster(12);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

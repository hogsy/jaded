/*$T SOFTzlist.c GC! 1.081 01/29/02 10:43:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "SOFT/SOFTzlist.h"
#include "GDInterface/GDIrasters.h"

#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"
#include "GFX/GFX.h"

#ifdef _GAMECUBE
#include "GXI_GC/GXI_init.h"
#endif

#ifdef _GAMECUBE
#ifndef _FINAL_
#include "GXI_GC/GXI_dbg.h"
#endif
#endif

#ifdef _XBOX
#include "GX8/RASter/Gx8_CheatFlags.h"
extern int g_MultipleVBIndex;
#endif // _XBOX

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

////////////////////////////////////////
#ifdef _GAMECUBE
extern char *SOFT_gpc_BigBuffer;
extern SOFT_tdst_ComputingBuffers			*SOFT_gp_Compute;
extern SOFT_tdst_ZList						*SOFT_gst_ZList;
extern SOFT_tdst_ZList_CommonParrams		*SOFT_gst_ZList_CP;
extern void									*g_pstFrameBuffer1;
extern void									*g_pstFrameBuffer2;
extern void									*DefaultFifo;
#else
char SOFT_gpc_BigBuffer[SIZE_BIG_BUFFER] ONLY_PSX2_ALIGNED(64);
SOFT_tdst_ComputingBuffers			*SOFT_gp_Compute = (SOFT_tdst_ComputingBuffers *) (SOFT_gpc_BigBuffer);
SOFT_tdst_ZList						*SOFT_gst_ZList = (SOFT_tdst_ZList *) (SOFT_gpc_BigBuffer+size_SOFT_Compute);
SOFT_tdst_ZList_CommonParrams		*SOFT_gst_ZList_CP = (SOFT_tdst_ZList_CommonParrams *) (SOFT_gpc_BigBuffer +size_SOFT_Compute+ size_SOFT_gst_ZList);
#endif
////////////////////////////////////////

SOFT_tdst_ZList						*p_Current_SOFT_gst_ZList;
GEO_tdst_Object						ZList_Obj;
GEO_tdst_ElementIndexedTriangles	ZList_Element;
GEO_tdst_IndexedTriangle			ZList_Triangle;
GEO_Vertex							ZList_Points[3];
GEO_tdst_UV							ZList_UV[3];
ULONG								ZList_RLI[3];
ULONG								ZList_Locked;

GEO_tdst_IndexedTriangle			*ZList_T;
GEO_Vertex							*ZList_V;
GEO_tdst_UV							*ZList_UVs;
MAT_tdst_Material					*ZList_Mat;
ULONG								*ZList_RLIs;

#ifdef GSP_PS2
extern u_int						NoZLST;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_AddCurrentObjectInZList(GDI_tdst_DisplayData *pst_DD, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float					f_Z;
	SOFT_tdst_ZList_Node	*pst_Node;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_b_CheckPointer(_pst_GO);
	
	if(pst_DD->ul_DisplayInfo & GDI_Cul_DI_ObjectHasBeenZAdded) return;
	f_Z = 1.0f / pst_DD->st_MatrixStack.pst_CurrentMatrix->T.z;
	pst_Node = SOFT_pst_ZList_AddNode(f_Z, 1, SOFT_l_TON_ObjectContext);
	if(pst_Node)
	{
		pst_Node->l_DrawMask = pst_DD->ul_CurrentDrawMask;

		pst_Node->pst_OC->pst_GO = _pst_GO;
		MATH_CopyMatrix(&pst_Node->pst_OC->st_Matrix, pst_DD->st_MatrixStack.pst_CurrentMatrix);
		MATH_CopyMatrix(&pst_Node->pst_OC->st_MatrixGO, pst_DD->pst_CurrentGameObject->pst_GlobalMatrix);


#ifdef ACTIVE_EDITORS
		pst_Node->pst_OC->ulForcedColorSaved = pst_DD->ul_ColorConstant;
#endif
	}

	pst_DD->ul_DisplayInfo |= GDI_Cul_DI_ObjectHasBeenZAdded;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SOFT_tdst_ZList_Node *SOFT_pst_ZList_AddNode(float _f_Z, LONG _l_VerticesNumber, LONG TypeOfContain)
{
	/*~~~~~~~~~~~~~~~~~~*/
	unsigned short	index;
	/*~~~~~~~~~~~~~~~~~~*/

#if defined(GSP_PS2) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoZLST) return NULL;
#endif

#ifdef GX8_BENCH 
	if(g_iNoZLST) 
		return NULL; 
#endif 

//	if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_I) return NULL;
	
	if((ULONG) SOFT_gst_ZList_CP->pst_CurrentNode >= (ULONG) & SOFT_gst_ZList_CP->pst_CurrentNode - 16) return NULL;
	if(ZList_Locked) return NULL;

	if(_f_Z < 0.0f)
	{
		if(TypeOfContain == SOFT_l_TON_ObjectContext)
			_f_Z = 0.0f;
		else
			return NULL;
	}

	index = (unsigned short) ((*(ULONG *) &_f_Z >> (31 - SOFT_l_MaxEntry)));
	if(index >= (1 << SOFT_l_MaxEntry)) return NULL;

	if(p_Current_SOFT_gst_ZList->l_Min > index) p_Current_SOFT_gst_ZList->l_Min = index;
	if(p_Current_SOFT_gst_ZList->l_Max < index) p_Current_SOFT_gst_ZList->l_Max = index;

	switch(TypeOfContain)
	{
	case SOFT_l_TON_Vertices:
		if((ULONG *) SOFT_gst_ZList_CP->ast_OC <= (ULONG *) SOFT_gst_ZList_CP->pst_CurrentVertex + _l_VerticesNumber) 
			return NULL;
		SOFT_gst_ZList_CP->pst_CurrentNode->pst_Vertices = SOFT_gst_ZList_CP->pst_CurrentVertex;
		if ((SOFT_gst_ZList_CP->pst_CurrentVertex + _l_VerticesNumber) >= (SOFT_gst_ZList_CP->ast_Vertex + (1<<SOFT_l_MaxVertex)))
			return NULL;
		SOFT_gst_ZList_CP->pst_CurrentVertex += _l_VerticesNumber;
		SOFT_gst_ZList_CP->pst_CurrentNode->pst_NextNode = p_Current_SOFT_gst_ZList->apst_SortedNode[index];
		p_Current_SOFT_gst_ZList->apst_SortedNode[index] = SOFT_gst_ZList_CP->pst_CurrentNode;
		break;
	case SOFT_l_TON_ObjectContext:
		if(&SOFT_gst_ZList_CP->ulLastField <= (ULONG *) SOFT_gst_ZList_CP->pst_CurrentObjectContext) 
			return NULL;
		if(SOFT_gst_ZList_CP->pst_CurrentObjectContext+1 >= SOFT_gst_ZList_CP->ast_OC + (1<<SOFT_l_MaxOC))
			return NULL;
		SOFT_gst_ZList_CP->pst_CurrentNode->pst_OC = SOFT_gst_ZList_CP->pst_CurrentObjectContext++;
		SOFT_gst_ZList_CP->pst_CurrentNode->pst_NextNode = p_Current_SOFT_gst_ZList->apst_SortedNode[index];
		p_Current_SOFT_gst_ZList->apst_SortedNode[index] = SOFT_gst_ZList_CP->pst_CurrentNode;
		break;
	}

	if (SOFT_gst_ZList_CP->pst_CurrentNode + 1 >= SOFT_gst_ZList_CP->ast_Node + (1<<SOFT_l_MaxNode))
		return NULL;
	else	
		return SOFT_gst_ZList_CP->pst_CurrentNode++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_ZList_Lock(void)
{
	ZList_Locked = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_ZList_Unlock(void)
{
	ZList_Locked = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_ZList_Init(void)
{
	ULONG	ZListesCounter;

#ifndef _GAMECUBE 
	extern char *MEM_gp_AllocTmpNext;
	/* Memoire temporaire = computing buffer */
	MEM_gp_AllocTmpFirst = MEM_gp_AllocTmpNext = SOFT_gpc_BigBuffer + 60;
	MEM_gp_AllocTmpLast = MEM_gp_AllocTmpFirst + SIZE_BIG_BUFFER - 64;
#endif // _GAMECUBE 

	ZListesCounter = NumberOfZListes;
	while(ZListesCounter--)
	{
		p_Current_SOFT_gst_ZList = &SOFT_gst_ZList[ZListesCounter];
		L_memset(p_Current_SOFT_gst_ZList, 0, sizeof(SOFT_tdst_ZList));
	}

	SOFT_ZList_Clear();

	L_memset(&ZList_Obj, 0, sizeof(GEO_tdst_Object));
	ZList_Obj.dst_Element = &ZList_Element;
	ZList_Obj.l_NbElements = 1;
	ZList_Obj.l_NbPoints = 3;
#ifdef PSX2_TARGET
	ZList_Obj.p_CompressedNormals = NULL;
#endif	
	ZList_Obj.dst_Point = ZList_Points;
	ZList_Obj.l_NbUVs = 3;
	ZList_Obj.dst_UV = ZList_UV;
	ZList_Obj.dul_PointColors = ZList_RLI;
	ZList_Obj.st_Id.i = &GRO_gast_Interface[GRO_Geometric];

	ZList_Element.dst_Triangle = &ZList_Triangle;
	ZList_Element.l_MaterialId = 0;
	ZList_Element.l_NbTriangles = 1;
	ZList_Element.p_MrmElementAdditionalInfo = NULL;
	ZList_Element.pst_StripData = NULL;
#ifdef PSX2_TARGET
	ZList_Element.pst_StripDataPS2 = NULL;
	ZList_Element.p_ElementCache = NULL;
#endif
	ZList_Element.pus_ListOfUsedIndex = NULL;
	ZList_Element.ul_NumberOfUsedIndex = 0;

	ZList_Triangle.auw_Index[0] = 0;
	ZList_Triangle.auw_Index[1] = 1;
	ZList_Triangle.auw_Index[2] = 2;
	ZList_Triangle.auw_UV[0] = 0;
	ZList_Triangle.auw_UV[1] = 1;
	ZList_Triangle.auw_UV[2] = 2;

	ZList_Locked = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_ZList_Clear(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ZListesCounter;
	/*~~~~~~~~~~~~~~~~~~~*/

/*	if (GDI_gpst_CurDD->ul_DisplayInfo & (GDI_Cul_DI_DoubleRendering_I|GDI_Cul_DI_DoubleRendering_K))
		 return;*/
	if (SOFT_gst_ZList == NULL || SOFT_gst_ZList_CP == NULL)
	{
		ERR_X_Assert(SOFT_gst_ZList == NULL && SOFT_gst_ZList_CP == NULL);
		return;
	}

	ZListesCounter = NumberOfZListes;
	while(ZListesCounter--)
	{
		p_Current_SOFT_gst_ZList = &SOFT_gst_ZList[ZListesCounter];
//#ifndef PSX2_TARGET		
#if 0
		L_memset(p_Current_SOFT_gst_ZList, 0, sizeof(SOFT_tdst_ZList));
#endif		
		p_Current_SOFT_gst_ZList->l_Max = 0;
		p_Current_SOFT_gst_ZList->l_Min = 1 << SOFT_l_MaxEntry;
	}

	SOFT_gst_ZList_CP->pst_CurrentNode = SOFT_gst_ZList_CP->ast_Node;
	SOFT_gst_ZList_CP->pst_CurrentVertex = SOFT_gst_ZList_CP->ast_Vertex;
	SOFT_gst_ZList_CP->pst_CurrentObjectContext = SOFT_gst_ZList_CP->ast_OC;

	/*
	 * optim? L_memset(SOFT_gst_ZList_CP->ast_OC, 0,
	 * sizeof(SOFT_tdst_ZListObjectContext) << SOFT_l_MaxOC);
	 */
}

#include "BASe/BENch/BENch.h"

static int	i_Identity;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
void SOFT_ZList_Draw(CHAR _c_BaseDisplayOrder)
#else
void SOFT_ZList_Draw(void)
#endif
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_ZList_Node	*pst_CurrentNode, **pst_TableEntry, **pst_LastTableEntry;
	OBJ_tdst_GameObject		*pst_GO;
	GRO_tdst_Struct			*pst_Gro;
#ifdef JADEFUSION
#ifdef ACTIVE_EDITORS
	ULONG					ul_SaveForcedColor;
#endif
#else
	ULONG					ul_SaveForcedColor;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(GSP_PS2) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoZLST) return;
#endif
	_GSP_BeginRaster(13);

	GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_RenderingTransparency;

#ifdef _XBOX
#pragma message("TODO: handle lighting case when we treat transparent faces on a per-triangle basis (-> sorted)") // might be unnecessary
#endif // _XBOX

	if(p_Current_SOFT_gst_ZList->l_Max >= p_Current_SOFT_gst_ZList->l_Min)
	{
		pst_TableEntry = p_Current_SOFT_gst_ZList->apst_SortedNode;
		pst_LastTableEntry = pst_TableEntry + p_Current_SOFT_gst_ZList->l_Max;
		pst_TableEntry += p_Current_SOFT_gst_ZList->l_Min;

		while(pst_TableEntry <= pst_LastTableEntry)
		{
 			pst_CurrentNode = *(pst_TableEntry);
			while(pst_CurrentNode != NULL)
			{
				/* Detect object */
				if(pst_CurrentNode->pst_OC < SOFT_gst_ZList_CP->ast_OC)
				{
					PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_NbSortedTriangles, 1);

					if
					(
						(
							(pst_CurrentNode->l_DrawMask != GDI_gpst_CurDD->ul_CurrentDrawMask)
						||	(ZList_Mat != (MAT_tdst_Material *) pst_CurrentNode->pst_Material)
						||	(ZList_V - GFX_gpst_Geo->dst_Point >= 768)
						)
					&&	(ZList_V != GFX_gpst_Geo->dst_Point)
					)
					{
						GFX_gpst_Geo->l_NbPoints = ZList_V - GFX_gpst_Geo->dst_Point;
						GFX_gpst_Geo->l_NbUVs = ZList_UVs - GFX_gpst_Geo->dst_UV;
						GFX_gpst_Geo->dst_Element->l_NbTriangles = ZList_T - GFX_gpst_Geo->dst_Element->dst_Triangle;
						GDI_gpst_CurDD->p_Current_Vertex_List = GFX_gpst_Geo->dst_Point;
#ifdef _XBOX 
                        LIGHT_TurnOffObjectLighting();

						Gx8_SetMultipleVBIndexForUpdate( 0 );
#endif // _XBOX
						MAT_DrawIndexedTriangle
						(
							GFX_gpst_Geo,
							(MAT_tdst_Material *) ZList_Mat,
							GFX_gpst_Geo->dst_Element
						);
						ZList_T = GFX_gpst_Geo->dst_Element->dst_Triangle;
						ZList_V = GFX_gpst_Geo->dst_Point;
						ZList_UVs = GFX_gpst_Geo->dst_UV;
						ZList_RLIs = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
					}

					GDI_gpst_CurDD->ul_CurrentDrawMask = pst_CurrentNode->l_DrawMask;

					ZList_Mat = (MAT_tdst_Material *) pst_CurrentNode->pst_Material;

					if(!i_Identity)
					{
						GDI_SetViewMatrix((*GDI_gpst_CurDD), &MATH_gst_IdentityMatrix);

						GDI_gpst_CurDD->pul_RLI_Used = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
						GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField = GDI_gpst_CurDD->pul_RLI_Used;
						GDI_gpst_CurDD->pst_AdditionalMaterial = NULL;
						i_Identity = 1;
					}

					ZList_T->auw_Index[0] = ZList_T->auw_UV[0] = ZList_V - GFX_gpst_Geo->dst_Point;
					ZList_T->auw_Index[1] = ZList_T->auw_UV[1] = ZList_T->auw_Index[0] + 1;
					ZList_T->auw_Index[2] = ZList_T->auw_UV[2] = ZList_T->auw_Index[0] + 2;
					ZList_T++;
					MATH_CopyVector(VCast(ZList_V), (MATH_tdst_Vector *) pst_CurrentNode->pst_Vertices);
					ZList_V++;
					MATH_CopyVector(VCast(ZList_V), (MATH_tdst_Vector *) (pst_CurrentNode->pst_Vertices + 1));
					ZList_V++;
					MATH_CopyVector(VCast(ZList_V), (MATH_tdst_Vector *) (pst_CurrentNode->pst_Vertices + 2));
					ZList_V++;
					ZList_UVs->fU = pst_CurrentNode->pst_Vertices->u;
					(ZList_UVs++)->fV = pst_CurrentNode->pst_Vertices->v;
					ZList_UVs->fU = (pst_CurrentNode->pst_Vertices + 1)->u;
					(ZList_UVs++)->fV = (pst_CurrentNode->pst_Vertices + 1)->v;
					ZList_UVs->fU = (pst_CurrentNode->pst_Vertices + 2)->u;
					(ZList_UVs++)->fV = (pst_CurrentNode->pst_Vertices + 2)->v;
					*ZList_RLIs++ = pst_CurrentNode->pst_Vertices->color;
					*ZList_RLIs++ = (pst_CurrentNode->pst_Vertices + 1)->color;
					*ZList_RLIs++ = (pst_CurrentNode->pst_Vertices + 2)->color;
				}
				else
				{
#ifdef ACTIVE_EDITORS
					u32 CurrentNumberOfTRris;
					extern u32 Stats_ulNumberOfTRiangles;
#endif
					MATH_tdst_Matrix *p_SaveOriginalMTX;
					if(ZList_V != GFX_gpst_Geo->dst_Point)
					{
						GFX_gpst_Geo->l_NbPoints = ZList_V - GFX_gpst_Geo->dst_Point;
						GFX_gpst_Geo->l_NbUVs = ZList_UVs - GFX_gpst_Geo->dst_UV;
						GFX_gpst_Geo->dst_Element->l_NbTriangles = ZList_T - GFX_gpst_Geo->dst_Element->dst_Triangle;
						GDI_gpst_CurDD->p_Current_Vertex_List = GFX_gpst_Geo->dst_Point;
#ifdef _XBOX  
                        LIGHT_TurnOffObjectLighting();

						Gx8_SetMultipleVBIndexForUpdate( 0 );
#endif // _XBOX 
						MAT_DrawIndexedTriangle
						(
							GFX_gpst_Geo,
							(MAT_tdst_Material *) ZList_Mat,
							GFX_gpst_Geo->dst_Element
						);
						ZList_T = GFX_gpst_Geo->dst_Element->dst_Triangle;
						ZList_V = GFX_gpst_Geo->dst_Point;
						ZList_UVs = GFX_gpst_Geo->dst_UV;
						ZList_RLIs = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
					}

					PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_NbSortedObjects, 1);

					pst_GO = pst_CurrentNode->pst_OC->pst_GO;
#ifdef ACTIVE_EDITORS
					CurrentNumberOfTRris = Stats_ulNumberOfTRiangles;
#endif
	                GDI_gpst_CurDD->ul_CurrentDrawMask = pst_CurrentNode->l_DrawMask;

					/*$2- render state -------------------------------------------------------------------------------*/

					SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, &pst_CurrentNode->pst_OC->st_Matrix);
					GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
					GDI_gpst_CurDD->pst_ComputingBuffers->ul_Flags &= ~SOFT_Cul_CB_SpecularField;
					GDI_gpst_CurDD->pst_CurrentGameObject = pst_GO;
					p_SaveOriginalMTX = pst_GO -> pst_GlobalMatrix;
					pst_GO -> pst_GlobalMatrix = &pst_CurrentNode->pst_OC->st_MatrixGO;

					if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
					{
						GDI_gpst_CurDD->pst_AdditionalMaterial = pst_GO->pst_Base->pst_AddMaterial;
						pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
#ifdef ACTIVE_EDITORS
						ul_SaveForcedColor = GDI_gpst_CurDD->ul_ColorConstant;
						GDI_gpst_CurDD->ul_ColorConstant = pst_CurrentNode->pst_OC->ulForcedColorSaved;
#endif
#if defined(_XENON_RENDER)
                        GDI_gpst_CurDD->g_cZListCurrentDisplayOrder = _c_BaseDisplayOrder + pst_GO->pst_Base->pst_Visu->c_DisplayOrder;
#endif
						if((pst_Gro->i->ul_Type == GRO_GeoStaticLOD) || (pst_Gro->i->ul_Type == GRO_Geometric) || (pst_Gro->i->ul_Type == GRO_ParticleGenerator) )
						{
							GDI_SetViewMatrix((*GDI_gpst_CurDD), &pst_CurrentNode->pst_OC->st_Matrix);
							i_Identity = 0;
							pst_Gro->i->pfn_Render(pst_GO);
							
							ZList_T = GFX_gpst_Geo->dst_Element->dst_Triangle;
    						ZList_V = GFX_gpst_Geo->dst_Point;
	    					ZList_UVs = GFX_gpst_Geo->dst_UV;
		    				ZList_RLIs = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
						}
#ifdef ACTIVE_EDITORS
						GDI_gpst_CurDD->ul_ColorConstant = ul_SaveForcedColor;
#endif
					}

					GDI_gpst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseSpecialVertexBuffer;
					SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);

					pst_GO -> pst_GlobalMatrix = p_SaveOriginalMTX;			
#ifdef ACTIVE_EDITORS
					pst_GO ->NumberOfTris += Stats_ulNumberOfTRiangles - CurrentNumberOfTRris;
#endif
				}

				if(pst_CurrentNode == pst_CurrentNode->pst_NextNode) pst_CurrentNode->pst_NextNode = NULL;
				pst_CurrentNode = pst_CurrentNode->pst_NextNode;
			}
//			if (!(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_I))
				*(pst_TableEntry++) = NULL;
/*			else
				pst_TableEntry++;//*/
		}
	}

	if(ZList_V != GFX_gpst_Geo->dst_Point)
	{
		GFX_gpst_Geo->l_NbPoints = ZList_V - GFX_gpst_Geo->dst_Point;
		GFX_gpst_Geo->l_NbUVs = ZList_UVs - GFX_gpst_Geo->dst_UV;
		GFX_gpst_Geo->dst_Element->l_NbTriangles = ZList_T - GFX_gpst_Geo->dst_Element->dst_Triangle;
		GDI_gpst_CurDD->p_Current_Vertex_List = GFX_gpst_Geo->dst_Point;
#ifdef _XBOX   
        LIGHT_TurnOffObjectLighting();
#endif // _XBOX 
		MAT_DrawIndexedTriangle(GFX_gpst_Geo, (MAT_tdst_Material *) ZList_Mat, GFX_gpst_Geo->dst_Element);
		ZList_T = GFX_gpst_Geo->dst_Element->dst_Triangle;
		ZList_V = GFX_gpst_Geo->dst_Point;
		ZList_UVs = GFX_gpst_Geo->dst_UV;
		ZList_RLIs = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
	}

	GDI_gpst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_RenderingTransparency;

	_GSP_EndRaster(13);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

extern void GSP_AE_ZListHook(int ZlistNum );

extern void	GAO_ModifierXMEN_DRAW_ALL(void);

void SOFT_ZList_Send(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ZListesCounter;
	/*~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	extern void AFTEREFFX_GlowFromZList();
	AFTEREFFX_GlowFromZList();
#endif
#ifdef PSX2_TARGET
	extern void GSP_AE_ZListHookGlow();
	GSP_AE_ZListHookGlow();
#endif	

	GAO_ModifierXMEN_DRAW_ALL();
	PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_ZList);
	/*
	 * optim?
	 * GDI_gpst_CurDD->st_GDI.pfnv_SetProjectionMatrix(&GDI_gpst_CurDD->st_Camera);
	 */
	i_Identity = 0;
	SOFT_MatrixStack_Reset(&GDI_gpst_CurDD->st_MatrixStack, &MATH_gst_IdentityMatrix);
	GFX_NeedGeom(768, 768, 256, FALSE);
	ZList_T = GFX_gpst_Geo->dst_Element->dst_Triangle;
	ZList_V = GFX_gpst_Geo->dst_Point;
	ZList_UVs = GFX_gpst_Geo->dst_UV;
	ZList_RLIs = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;

	ZListesCounter = NumberOfZListes;
	while(ZListesCounter--)
	{
		GSP_AE_ZListHook(ZListesCounter - ZListesBase);
		p_Current_SOFT_gst_ZList = &SOFT_gst_ZList[ZListesCounter];
#ifdef JADEFUSION
		SOFT_ZList_Draw((CHAR)ZListesCounter - (CHAR)ZListesBase);
#else
		SOFT_ZList_Draw();
#endif
	}

	SOFT_ZList_Clear();

	PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_ZList);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

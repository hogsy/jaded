/*$T MDFmodifier_XMEN.c GC! */


/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_XMEN.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTlinear.h"

#ifdef _XENON_RENDER
/*#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeShader.h"
#include "XenonGraphics/XeSharedDefines.h"
#include "XenonGraphics/XeWaterManager.h"
#include "XenonGraphics/XeUtils.h"*/
#include "XenonGraphics/XeMesh.h"
#include "XenonGraphics/XeDynVertexBuffer.h"
#include "XenonGraphics/XeRenderer.h"
#endif

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#define MAX_XMEN_NODES 96
#define XMEN_SMOOTH_DEPTH 2

GAO_tdst_ModifierXMEN	*p_XMEN_STACK[MAX_XMEN_NODES];
ULONG					XMEN_STACK_PTR = 0;

static ULONG	bIsMaterialTransparent = 1;
static ULONG 	g_ulFrameCounter = 0;
extern BOOL     ENG_gb_ForcePauseEngine;

#ifdef JADEFUSION
class XeMesh            *g_pXmenMesh;
XeBuffer                *g_pXmenBuffer;
XeRenderer::XeVertexDyn *g_pXmenBufferArray=NULL;
void GAO_XMN_Render_Xenon( GAO_tdst_ModifierXMEN *p_XMEN,GEO_tdst_Object *pst_Obj, MAT_tdst_Material *p_MaterialUsed );
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

GEO_tdst_Object						XMEN_OBJECT;
GEO_Vertex							XMEN_OBJECT_Points_Smoothed[2<<(XMEN_NumberOfSegs_PO2 + XMEN_SMOOTH_DEPTH)];
GEO_tdst_UV							XMEN_OBJECT_UVS	  [2<<(XMEN_NumberOfSegs_PO2 + XMEN_SMOOTH_DEPTH)];
GEO_tdst_ElementIndexedTriangles	XMEN_ELEMENT;
GEO_tdst_IndexedTriangle			XMEN_OBJECT_TRIS  [2<<(XMEN_NumberOfSegs_PO2 + XMEN_SMOOTH_DEPTH)];
GAO_tdst_XMEN_Chhlaahhh_Memory		XMEN_OBJECT_Points;


void XMEN_FirstInit(void)
{
	static ULONG bFirst = 0;
	if (bFirst) return;
	bFirst = 1;
	memset(&XMEN_OBJECT , 0 , sizeof( GEO_tdst_Object ));
	GRO_Struct_Init(&XMEN_OBJECT.st_Id, GRO_Geometric);
	XMEN_OBJECT . l_NbPoints = (1<<XMEN_NumberOfSegs_PO2) * 2;
	XMEN_OBJECT . dst_Point	 = XMEN_OBJECT_Points_Smoothed;
	XMEN_OBJECT . dst_PointNormal = XMEN_OBJECT_Points_Smoothed;
	XMEN_OBJECT . l_NbUVs	 = (1<<XMEN_NumberOfSegs_PO2) * 2;
	XMEN_OBJECT . dst_UV	 = XMEN_OBJECT_UVS;
	XMEN_OBJECT .l_NbElements= 1;
	XMEN_OBJECT .dst_Element = &XMEN_ELEMENT;

	memset(&XMEN_ELEMENT, 0 , sizeof( GEO_tdst_ElementIndexedTriangles ));
	memset(XMEN_OBJECT_TRIS, 0 , sizeof( GEO_tdst_IndexedTriangle ) * (2<<(XMEN_NumberOfSegs_PO2 + XMEN_SMOOTH_DEPTH)));
	XMEN_ELEMENT .	dst_Triangle = XMEN_OBJECT_TRIS;
	XMEN_ELEMENT . l_NbTriangles = ((1<<XMEN_NumberOfSegs_PO2) - 1) * 2;
	XMEN_ELEMENT . l_MaterialId  = 0;
}

/*$4
***********************************************************************************************************************
XMEN modifier :
Modifier apply XMEN on world
***********************************************************************************************************************
*/
#ifdef USE_DOUBLE_RENDERING	
void 								GAO_ModifierXMEN_Interpolate
									(
										struct MDF_tdst_Modifier_ * p_Mod, 
										u_int mode , 
										float fInterpolatedValue
									)
{
}
#endif

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierXMEN_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	XMEN_FirstInit();
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierXMEN));

   if (p_Data == NULL)
   {
		/* First init */
	    L_memset(_pst_Mod->p_Data , 0 , sizeof(GAO_tdst_ModifierXMEN));
	    ((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) ->bk_MaterialUsed = (ULONG)-1;
		_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
//		((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) -> p_stXMEN_MEM = MEM_p_Alloc(sizeof(GAO_tdst_XMEN_Chhlaahhh_Memory) * );
   }
   else
   {
		GAO_tdst_ModifierXMEN *p_XMEN;
		_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
		p_XMEN = (GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data;
		L_memcpy( _pst_Mod->p_Data, p_Data, sizeof (GAO_tdst_ModifierXMEN) );
		if (p_XMEN->ulNumber_Of_Chhlaahhh)
		{
			p_XMEN->p_st_Chhlaahhh = (GAO_tdst_XMEN_Chhlaahhh*)MEM_p_Alloc(p_XMEN->ulNumber_Of_Chhlaahhh * sizeof(GAO_tdst_XMEN_Chhlaahhh));
			memcpy(p_XMEN->p_st_Chhlaahhh ,((GAO_tdst_ModifierXMEN *)p_Data)->p_st_Chhlaahhh, p_XMEN->ulNumber_Of_Chhlaahhh * sizeof(GAO_tdst_XMEN_Chhlaahhh));
			p_XMEN ->p_stXMEN_MEM = (GAO_tdst_XMEN_Chhlaahhh_Memory *)MEM_p_Alloc(sizeof(GAO_tdst_XMEN_Chhlaahhh_Memory) * (p_XMEN -> ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2));		
			memcpy(p_XMEN ->p_stXMEN_MEM , ((GAO_tdst_ModifierXMEN *)p_Data)->p_stXMEN_MEM , sizeof(GAO_tdst_XMEN_Chhlaahhh_Memory) * (p_XMEN -> ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2));

			p_XMEN ->p_XmenLocalLenght = (float *)MEM_p_Alloc(sizeof(float) * (p_XMEN -> ulNumber_Of_Chhlaahhh  << XMEN_NumberOfSegs_PO2));		
			memcpy(p_XMEN ->p_XmenLocalLenght , ((GAO_tdst_ModifierXMEN *)p_Data)->p_XmenLocalLenght , sizeof(float) * (p_XMEN -> ulNumber_Of_Chhlaahhh  << XMEN_NumberOfSegs_PO2));
			
		}

		if(p_XMEN->p_MaterialUsed)
		{
			p_XMEN->p_MaterialUsed->st_Id.i->pfn_AddRef(p_XMEN->p_MaterialUsed, 1);
		}
   }
#ifdef ACTIVE_EDITORS
   ((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) ->ulCodeKey = 0xC0DE2001;
#endif
   ((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) ->pst_GO = _pst_GO;

/*#ifdef JADEFUSION
   ((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) ->XmenMesh = new XeMesh;
#endif*/
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierXMEN_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	if (_pst_Mod->p_Data)
	{
		{
			GAO_tdst_ModifierXMEN *p_XMEN;
			ULONG XStackCounter;

			p_XMEN =  (GAO_tdst_ModifierXMEN*)_pst_Mod->p_Data;
			XStackCounter = XMEN_STACK_PTR;
			while (XStackCounter --)
			{
				if (p_XMEN_STACK[XStackCounter ] == p_XMEN) p_XMEN_STACK[XStackCounter ] = NULL;
			}

			if(p_XMEN->p_MaterialUsed)
			{
				p_XMEN->p_MaterialUsed->st_Id.i->pfn_AddRef(p_XMEN->p_MaterialUsed, -1);
				p_XMEN->p_MaterialUsed->st_Id.i->pfn_Destroy(p_XMEN->p_MaterialUsed);
			}
		}
		if (((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) -> ulNumber_Of_Chhlaahhh)
		{
			MEM_Free(((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) ->p_st_Chhlaahhh );
			MEM_Free(((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) ->p_stXMEN_MEM);
			MEM_Free(((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) ->p_XmenLocalLenght);
			
		}
/*#ifdef JADEFUSION
		SAFE_DELETE(((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) ->XmenMesh);
		((GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data) ->XmenMesh =NULL;
#endif*/
		MEM_Free(_pst_Mod->p_Data);
	}
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_XMN_Render_TransparentTriangles(GEO_tdst_Object *pst_Obj, MAT_tdst_Material		*p_MaterialUsed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector					*pst_V1, *pst_V2, *pst_V3;
	GEO_Vertex							*pst_VertexList;
	GEO_tdst_IndexedTriangle			*pst_Triangle, *pst_LastTriangle;
	float								f_Z;
	SOFT_tdst_ZList_Node				*pst_Node;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	ULONG								ul_ObjectMask;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Element = pst_Obj->dst_Element;
	pst_LastElement = pst_Element + pst_Obj->l_NbElements;
	ul_ObjectMask = GDI_gpst_CurDD->ul_CurrentDrawMask;

	for(; pst_Element < pst_LastElement; pst_Element++)
	{
		/* No multitexture & transparency -> Classic algorithm */
		SOFT_TransformInComputingBuffer
		(
			GDI_gpst_CurDD->pst_ComputingBuffers,
			GDI_gpst_CurDD->p_Current_Vertex_List,
			pst_Obj->l_NbPoints,
			&GDI_gpst_CurDD->st_Camera
		);
		pst_VertexList = GDI_gpst_CurDD->pst_ComputingBuffers->ast_3D;

		pst_Triangle = pst_Element->dst_Triangle;
		pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

		for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
		{
			pst_V1 = VCast(pst_VertexList + pst_Triangle->auw_Index[0]);
			pst_V2 = VCast(pst_VertexList + pst_Triangle->auw_Index[1]);
			pst_V3 = VCast(pst_VertexList + pst_Triangle->auw_Index[2]);

			f_Z = pst_V1->z;
			if(f_Z < pst_V2->z) f_Z = pst_V2->z;
			if(f_Z < pst_V3->z) f_Z = pst_V3->z;
			f_Z = fOptInv(f_Z);

			if((pst_Node = SOFT_pst_ZList_AddNode(f_Z, 3, SOFT_l_TON_Vertices)) != NULL)
			{
				*((MATH_tdst_Vector *) &pst_Node->pst_Vertices[0]) = *pst_V1;
				*((MATH_tdst_Vector *) &pst_Node->pst_Vertices[1]) = *pst_V2;
				*((MATH_tdst_Vector *) &pst_Node->pst_Vertices[2]) = *pst_V3;
				if(pst_Obj->dst_UV)
				{
					pst_Node->pst_Vertices[0].u = pst_Obj->dst_UV[pst_Triangle->auw_UV[0]].fU;
					pst_Node->pst_Vertices[0].v = pst_Obj->dst_UV[pst_Triangle->auw_UV[0]].fV;
					pst_Node->pst_Vertices[1].u = pst_Obj->dst_UV[pst_Triangle->auw_UV[1]].fU;
					pst_Node->pst_Vertices[1].v = pst_Obj->dst_UV[pst_Triangle->auw_UV[1]].fV;
					pst_Node->pst_Vertices[2].u = pst_Obj->dst_UV[pst_Triangle->auw_UV[2]].fU;
					pst_Node->pst_Vertices[2].v = pst_Obj->dst_UV[pst_Triangle->auw_UV[2]].fV;
				}

				/*
                pst_Node->pst_Vertices[0].color = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[pst_Triangle->auw_Index[0]];
				pst_Node->pst_Vertices[1].color = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[pst_Triangle->auw_Index[1]];
				pst_Node->pst_Vertices[2].color = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[pst_Triangle->auw_Index[2]];
                */
                pst_Node->pst_Vertices[0].color = 0x00FFFFFF;
				pst_Node->pst_Vertices[1].color = 0x00FFFFFF;
				pst_Node->pst_Vertices[2].color = 0x00FFFFFF;
                

				pst_Node->pst_Material = (MAT_tdst_MultiTexture *)p_MaterialUsed;
				pst_Node->l_DrawMask = GDI_gpst_CurDD->ul_CurrentDrawMask;
			}
		}

		GDI_gpst_CurDD->ul_CurrentDrawMask = ul_ObjectMask;
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierXMEN_GetBonePosFromNum(OBJ_tdst_GameObject		*_pst_GO, ULONG ul_MatrixNum , MATH_tdst_Vector *p_stBonePos , MATH_tdst_Vector *p_stBoneDir , ULONG ulProjMethod , MATH_tdst_Vector *p_OldPos)
{
	MATH_tdst_Matrix                        *p_UsedMatrix,st_Matrix2;
	MATH_tdst_Matrix                        st_Matrix3  ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Vector						stSpeedDir ;
	p_UsedMatrix = NULL;
	if (_pst_GO->pst_Base)
	{
		if (_pst_GO->pst_Base->pst_AddMatrix)
		{
			if ((ULONG)_pst_GO->pst_Base->pst_AddMatrix->l_Number > ul_MatrixNum) 
			{
				if (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
				{
					p_UsedMatrix = _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ul_MatrixNum].pst_Matrix;
					if((_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ul_MatrixNum].l_MatrixId == (LONG)0xffffffff) || (_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ul_MatrixNum].pst_GO && (_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ul_MatrixNum].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)))
					{
						if (_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ul_MatrixNum].pst_GO)
						{
							p_UsedMatrix = _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ul_MatrixNum].pst_GO->pst_GlobalMatrix;
							MATH_InvertMatrix(&st_Matrix3 , _pst_GO->pst_GlobalMatrix);
							MATH_MulMatrixMatrix(&st_Matrix2 , p_UsedMatrix, &st_Matrix3 );
						} else
						{
							MATH_SetIdentityMatrix(&st_Matrix2);
						}
						p_UsedMatrix = &st_Matrix2;
					}
				}
				else
					p_UsedMatrix = &_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ul_MatrixNum].st_Matrix;
			} 
		}
	}
	if (!p_UsedMatrix) p_UsedMatrix = &MATH_gst_IdentityMatrix;
	
	*p_stBonePos = p_UsedMatrix->T;

	switch(ulProjMethod)
	{
	case 4:// Speed X
	case 2://X
		*p_stBoneDir = *MATH_pst_GetXAxis(p_UsedMatrix);
		break;
	case 5:// Speed Y
	case 1://Y
		*p_stBoneDir = *MATH_pst_GetYAxis(p_UsedMatrix);
		break;
	case 6:// Speed Z
	case 0://Z
		*p_stBoneDir = *MATH_pst_GetZAxis(p_UsedMatrix);
		break;
	case 3:// Camera 
		p_stBoneDir -> x = GDI_gpst_CurDD_SPR.st_MatrixStack.pst_CurrentMatrix->Iz;
		p_stBoneDir -> y = GDI_gpst_CurDD_SPR.st_MatrixStack.pst_CurrentMatrix->Jz;
		p_stBoneDir -> z = GDI_gpst_CurDD_SPR.st_MatrixStack.pst_CurrentMatrix->Kz;
		break;
	}

	MATH_TransformVertex(p_stBonePos, _pst_GO->pst_GlobalMatrix, p_stBonePos);
	if (ulProjMethod != 3) MATH_TransformVector(p_stBoneDir, _pst_GO->pst_GlobalMatrix, p_stBoneDir);

	MATH_SubVector(&stSpeedDir , p_stBonePos , p_OldPos);
	switch(ulProjMethod)
	{
		MATH_tdst_Vector						stLocal;
	case 3:// Camera 
	case 4:// Speed X
	case 5:// Speed Y
	case 6:// Speed Z
		MATH_CrossProduct(&stLocal , p_stBoneDir , &stSpeedDir);
		MATH_NormalizeAnyVector(p_stBoneDir , &stLocal );
		break;
	}
}
/*
=======================================================================================================================
=======================================================================================================================
*/
_inline_ void GAO_ModifierXMEN_SMOOTH2COORDS(GAO_tdst_XMEN_Chhlaahhh_Memory	*PR1,GAO_tdst_XMEN_Chhlaahhh_Memory	*P1,GAO_tdst_XMEN_Chhlaahhh_Memory	*P2)
{
	MATH_BlendVector(VCast(&PR1->stOldPos1),VCast(&P1->stOldPos1),VCast(&P2->stOldPos1),0.5f);
	MATH_BlendVector(VCast(&PR1->stOldPos2),VCast(&P1->stOldPos2),VCast(&P2->stOldPos2),0.5f);
}
/*
=======================================================================================================================
=======================================================================================================================
*/
static float LastSmooth = 0.5f;
_inline_ void GAO_ModifierXMEN_SMOOTH3COORDS(GAO_tdst_XMEN_Chhlaahhh_Memory	*PR1,GAO_tdst_XMEN_Chhlaahhh_Memory	*P1,GAO_tdst_XMEN_Chhlaahhh_Memory	*P2)
{
	MATH_tdst_Vector stLocal;
	MATH_BlendVector(&stLocal,VCast(&P1->stOldPos1),VCast(&P2->stOldPos1),0.5f);
	MATH_BlendVector(VCast(&PR1->stOldPos1),VCast(&PR1->stOldPos1),&stLocal,LastSmooth);
	MATH_BlendVector(&stLocal,VCast(&P1->stOldPos2),VCast(&P2->stOldPos2),0.5f);
	MATH_BlendVector(VCast(&PR1->stOldPos2),VCast(&PR1->stOldPos2),&stLocal,LastSmooth);
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierXMEN_SMOOTH_BUFFER(GAO_tdst_XMEN_Chhlaahhh_Memory	*p_BUFFER,ULONG Step)
{
	GAO_tdst_XMEN_Chhlaahhh_Memory	*p_Base,*p_BaseLast;
	/* Compute Middles */
	p_Base = p_BUFFER;
	p_BaseLast = p_Base + (1 << (XMEN_NumberOfSegs_PO2 + XMEN_SMOOTH_DEPTH)) - Step;
	GAO_ModifierXMEN_SMOOTH2COORDS(p_BaseLast + (Step >> 1), p_Base , p_BaseLast );
	while (p_Base < p_BaseLast)
	{
		GAO_ModifierXMEN_SMOOTH2COORDS(p_Base + (Step >> 1), p_Base , p_Base + Step);
		p_Base+=Step;
	}
	/* Erode borders */
	p_Base = p_BUFFER;
	GAO_ModifierXMEN_SMOOTH3COORDS(p_Base,p_BaseLast + (Step >> 1),p_Base + (Step >> 1));
	p_Base += Step;
	while (p_Base <= p_BaseLast)
	{
		GAO_ModifierXMEN_SMOOTH3COORDS(p_Base,p_Base - (Step >> 1),p_Base + (Step >> 1));
		p_Base+=Step;
	}
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierXMEN_DRAW(GAO_tdst_ModifierXMEN *p_XMEN)
{
	ULONG							ulRingCounter,ulRingCounterM1;
	GEO_tdst_IndexedTriangle		*p_TRIS;
	GEO_tdst_UV						*p_UVS;
	GEO_Vertex						*p_XYZ,*p_Last_XYZ;
	float							fCurrentV,fAddV,fTotalLenght;
	ULONG							BigStep;
	ULONG							ChlCounter;
//	MATH_tdst_Matrix				stFUCK;
	ULONG							ulRingEntryUsed;

	if (!p_XMEN->ulNumber_Of_Chhlaahhh) return;

#ifdef ACTIVE_EDITORS
	if (ENG_gb_EngineRunning)
#endif
#ifdef PSX2_TARGET
	if (!ENG_gb_ForcePauseEngine)
#endif
	{
		GAO_tdst_XMEN_Chhlaahhh			*p_CurrentChla;
		GAO_tdst_XMEN_Chhlaahhh_Memory	*p_CurrentMem;
		GAO_tdst_XMEN_Chhlaahhh_Memory	*p_CurrentMemLast;
		MATH_tdst_Vector				stBoneDir;
		MATH_tdst_Vector				stBonePos ;
		MATH_tdst_Vector				stLastBonePos;
		
		if (p_XMEN -> ulFrameCounter != (g_ulFrameCounter - 1))
		{
			p_XMEN -> ulCurrentRINGentry = 0;
			p_XMEN -> fDTMinLocal = 10000000000000.0f;
		}
		if (p_XMEN -> fDTMin < 0.0005f) p_XMEN -> fDTMin = 0.0005f;
		p_XMEN -> fDTMinLocal += TIM_gf_dt;
		if ((p_XMEN -> fDTMinLocal < p_XMEN -> fDTMin )/* || 	(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_K)*/)
		{
			p_XMEN -> ulCurrentRINGentry+= (1 << XMEN_NumberOfSegs_PO2) - 1;
			p_XMEN -> ulCurrentRINGentry &= (1 << XMEN_NumberOfSegs_PO2) - 1;
		} else p_XMEN -> fDTMinLocal = 0.0f;


		p_CurrentChla = p_XMEN ->p_st_Chhlaahhh + p_XMEN -> ulNumber_Of_Chhlaahhh;
		p_CurrentMem  = p_XMEN ->p_stXMEN_MEM + (p_XMEN -> ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2) + p_XMEN -> ulCurrentRINGentry;
		p_CurrentMemLast = p_CurrentMem -1;
		if (p_XMEN -> ulCurrentRINGentry == 0) p_CurrentMemLast += 1 << XMEN_NumberOfSegs_PO2;
//		if (p_XMEN ->ulProjectionMethod != 3) 
		{
			while (p_CurrentChla > p_XMEN ->p_st_Chhlaahhh )
			{
				p_CurrentChla--;
				p_CurrentMem -= 1 << XMEN_NumberOfSegs_PO2;
				p_CurrentMemLast -= 1 << XMEN_NumberOfSegs_PO2;
				if (p_XMEN ->ulProjectionMethod == 3) 
				{
					MATH_BlendVector(&stLastBonePos,VCast(&p_CurrentMemLast->stOldPos1),VCast(&p_CurrentMemLast->stOldPos2) , 0.5f);
					GAO_ModifierXMEN_GetBonePosFromNum(p_XMEN ->pst_GO, p_CurrentChla->ulBonesNum , &stBonePos , &stBoneDir , p_XMEN ->ulProjectionMethod , &stLastBonePos );
					MATH_AddScaleVector(&p_CurrentMem->stOldPos1 , &stBonePos , &stBoneDir , 0.0f);
					MATH_AddScaleVector(&p_CurrentMem->stOldPos2 , &stBonePos , &stBoneDir , 0.0f);
				} else
				{
					if (p_XMEN ->ulFlags & MOD_XMEN_Centered)
					{
						MATH_BlendVector(&stLastBonePos,VCast(&p_CurrentMemLast->stOldPos1),VCast(&p_CurrentMemLast->stOldPos2) , 0.5f);
						GAO_ModifierXMEN_GetBonePosFromNum(p_XMEN ->pst_GO, p_CurrentChla->ulBonesNum , &stBonePos , &stBoneDir , p_XMEN ->ulProjectionMethod , &stLastBonePos );
						MATH_AddScaleVector(&p_CurrentMem->stOldPos1 , &stBonePos , &stBoneDir , -p_CurrentChla->fLenght * 0.5f);
						MATH_AddScaleVector(&p_CurrentMem->stOldPos2 , &stBonePos , &stBoneDir , p_CurrentChla->fLenght * 0.5f);
					}
					else
					{
						MATH_CopyVector(&stLastBonePos, VCast(&p_CurrentMemLast->stOldPos1));
						GAO_ModifierXMEN_GetBonePosFromNum(p_XMEN ->pst_GO, p_CurrentChla->ulBonesNum , &stBonePos , &stBoneDir , p_XMEN ->ulProjectionMethod , &stLastBonePos );
						MATH_AddScaleVector(&p_CurrentMem->stOldPos1 , &stBonePos , &stBoneDir , 0.0f);
						MATH_AddScaleVector(&p_CurrentMem->stOldPos2 , &stBonePos , &stBoneDir , p_CurrentChla->fLenght);
					}
				}
			}
		}
	}
#ifdef ACTIVE_EDITORS
	if (ENG_gb_EngineRunning)
#endif
#ifdef PSX2_TARGET
	if (!ENG_gb_ForcePauseEngine)	
#endif
	{
		GAO_tdst_XMEN_Chhlaahhh_Memory	*p_CurrentMem;
		GAO_tdst_XMEN_Chhlaahhh_Memory	*p_CurrentMemLast;
		p_XMEN -> ulCurrentRINGentry++;
		p_XMEN -> ulCurrentRINGentry &= (1 << XMEN_NumberOfSegs_PO2) - 1;
		if (p_XMEN -> ulFrameCounter != (g_ulFrameCounter - 1))
		{
			p_XMEN -> ulCurrentRINGentry = 0;
			p_XMEN -> fDTMinLocal = 10000000000000.0f;
			p_CurrentMem  = p_XMEN ->p_stXMEN_MEM;
			p_CurrentMemLast  = p_XMEN ->p_stXMEN_MEM + (p_XMEN -> ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2);
			while (p_CurrentMem < p_CurrentMemLast)
			{
				*p_CurrentMem = *p_XMEN ->p_stXMEN_MEM;
				p_CurrentMem++;
			}
		}
	}
	/* ------------------------------------ */
	/* If Xmen is in LookAt					*/
	/* Turn ALL the segs in front of Camera */
	/* Even if we are in editor mode		*/
	/* ------------------------------------ */
	if (p_XMEN ->ulProjectionMethod == 3) 
	{
		GAO_tdst_XMEN_Chhlaahhh			*p_CurrentChla;
		GAO_tdst_XMEN_Chhlaahhh_Memory	*p_CurrentMem;
		GAO_tdst_XMEN_Chhlaahhh_Memory	*p_CurrentMemEntry;
		GAO_tdst_XMEN_Chhlaahhh_Memory	*p_CurrentMemLast;
		MATH_tdst_Vector				stBoneDir;
		MATH_tdst_Vector				stBonePos , stCameraZ;
		ULONG Counter , Counter2;
		ulRingEntryUsed = p_XMEN -> ulCurrentRINGentry;
		Counter = p_XMEN -> ulNumber_Of_Chhlaahhh;
		while (Counter--)
		{
			p_CurrentChla = p_XMEN ->p_st_Chhlaahhh + Counter;
			Counter2 = 1 << XMEN_NumberOfSegs_PO2;
			p_CurrentMemEntry = p_XMEN ->p_stXMEN_MEM + (Counter << XMEN_NumberOfSegs_PO2) + ulRingEntryUsed;
			p_CurrentMem = p_XMEN ->p_stXMEN_MEM + (Counter << XMEN_NumberOfSegs_PO2);
			while (Counter2--)
			{
				if (p_XMEN ->ulFlags & MOD_XMEN_Centered)
				{
					MATH_BlendVector(VCast(&p_CurrentMem->stOldPos1),VCast(&p_CurrentMem->stOldPos1),VCast(&p_CurrentMem->stOldPos2) , 0.5f);
				}
				p_CurrentMem++;
			}
			Counter2 = 1 << XMEN_NumberOfSegs_PO2;
			p_CurrentMem = p_XMEN ->p_stXMEN_MEM + (Counter << XMEN_NumberOfSegs_PO2);
			p_CurrentMemLast = p_XMEN ->p_stXMEN_MEM + ((Counter + 1) << XMEN_NumberOfSegs_PO2) - 1;
			MATH_CopyVector(&stCameraZ , MATH_pst_GetZAxis(&GDI_gpst_CurDD_SPR.st_Camera.st_Matrix));
			while (Counter2--)
			{
				if (p_CurrentMemEntry == p_CurrentMem)
				{
					GAO_tdst_XMEN_Chhlaahhh_Memory	*p_CurrentMemPlusOne;
					if (Counter2) p_CurrentMemPlusOne = p_CurrentMem+1;
					else p_CurrentMemPlusOne = p_XMEN ->p_stXMEN_MEM + (Counter << XMEN_NumberOfSegs_PO2);
					MATH_SubVector(&stBoneDir , VCast(&p_CurrentMemPlusOne ->stOldPos1) , VCast(&p_CurrentMem->stOldPos1));
					MATH_CrossProduct(VCast(&p_CurrentMem->stOldPos2) , &stCameraZ , &stBoneDir); 
					MATH_NormalizeAnyVector(VCast(&p_CurrentMem->stOldPos2) , VCast(&p_CurrentMem->stOldPos2));
				} else
				{
					MATH_SubVector(&stBoneDir , VCast(&p_CurrentMem->stOldPos1) , VCast(&p_CurrentMemLast->stOldPos1));
					MATH_CrossProduct(VCast(&p_CurrentMem->stOldPos2) , &stCameraZ , &stBoneDir); 
					MATH_NormalizeAnyVector(VCast(&p_CurrentMem->stOldPos2) , VCast(&p_CurrentMem->stOldPos2));
				}
				p_CurrentMemLast = p_CurrentMem++;
			}

			Counter2 = 1 << XMEN_NumberOfSegs_PO2;
			p_CurrentMem = p_XMEN ->p_stXMEN_MEM + (Counter << XMEN_NumberOfSegs_PO2);
			while (Counter2--)
			{
				if (p_XMEN ->ulFlags & MOD_XMEN_Centered)
				{
					MATH_CopyVector(&stBonePos , VCast(&p_CurrentMem->stOldPos1));
					MATH_AddScaleVector(VCast(&p_CurrentMem->stOldPos1) , &stBonePos , VCast(&p_CurrentMem->stOldPos2) , -p_CurrentChla->fLenght * 0.5f);
					MATH_AddScaleVector(VCast(&p_CurrentMem->stOldPos2) , &stBonePos , VCast(&p_CurrentMem->stOldPos2) , p_CurrentChla->fLenght * 0.5f);
				} else
				{
					MATH_AddScaleVector(&p_CurrentMem->stOldPos2 , &p_CurrentMem->stOldPos1 , &p_CurrentMem->stOldPos2  , p_CurrentChla->fLenght);
				}
				p_CurrentMem++;
			}
		}
	}
	if (p_XMEN ->ulFlags & MOD_XMEN_GlobalMatrix)
	{
		SOFT_MatrixStack_Reset(&GDI_gpst_CurDD->st_MatrixStack, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);		
	} else
	{
		SOFT_MatrixStack_Reset(&GDI_gpst_CurDD->st_MatrixStack, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);		
		SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix(p_XMEN->pst_GO));
	}
	
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
	ChlCounter = p_XMEN->ulNumber_Of_Chhlaahhh;
	while (ChlCounter--)
	{
		memset(XMEN_OBJECT_TRIS, 0 , sizeof( GEO_tdst_IndexedTriangle ) * (2<<(XMEN_NumberOfSegs_PO2 + XMEN_SMOOTH_DEPTH)));
		memset(XMEN_OBJECT_Points_Smoothed, 0 , sizeof( GEO_Vertex ) * 2<<(XMEN_NumberOfSegs_PO2 + XMEN_SMOOTH_DEPTH));
		if (p_XMEN ->ulFlags & MOD_XMEN_Smooth)
		{
			ULONG Local1;
			GAO_tdst_XMEN_Chhlaahhh_Memory	*p_CurrentMem,*p_CurrentMemDest;
			GAO_tdst_XMEN_Chhlaahhh_Memory	*p_CurrentMemS1,*p_CurrentMemS2;
			/* First copy the points in computingbuffer */
			p_CurrentMemDest = (GAO_tdst_XMEN_Chhlaahhh_Memory	*)XMEN_OBJECT_Points_Smoothed ; 
			p_CurrentMem  = p_XMEN ->p_stXMEN_MEM + (ChlCounter << XMEN_NumberOfSegs_PO2);
			Local1 = 1<<XMEN_NumberOfSegs_PO2;
			while (Local1--)
			{
				*p_CurrentMemDest = *p_CurrentMem;
				p_CurrentMem ++;
				p_CurrentMemDest += 1<<XMEN_SMOOTH_DEPTH;
			}

			/* then Smooth */
			Local1 = XMEN_SMOOTH_DEPTH;

			p_CurrentMem = ((GAO_tdst_XMEN_Chhlaahhh_Memory	*)XMEN_OBJECT_Points_Smoothed) + (p_XMEN -> ulCurrentRINGentry << XMEN_SMOOTH_DEPTH);
			p_CurrentMemDest = p_XMEN ->p_stXMEN_MEM + (p_XMEN -> ulCurrentRINGentry)  + (ChlCounter << XMEN_NumberOfSegs_PO2);
			p_CurrentMemS1 = p_CurrentMem - (1<<XMEN_SMOOTH_DEPTH);
			p_CurrentMemS2 = p_CurrentMemDest - 1;
			if (!p_XMEN -> ulCurrentRINGentry)
			{
				p_CurrentMemS1 += 1<<(XMEN_NumberOfSegs_PO2 + XMEN_SMOOTH_DEPTH);
				p_CurrentMemS2 += 1<<(XMEN_NumberOfSegs_PO2);
			}

			while (Local1)
			{
				GAO_ModifierXMEN_SMOOTH_BUFFER((GAO_tdst_XMEN_Chhlaahhh_Memory	*)XMEN_OBJECT_Points_Smoothed,1<<(Local1--));
				/* Force ENd & Start TO keep Good position */
				*p_CurrentMem = *p_CurrentMemDest;
				*p_CurrentMemS1 = *p_CurrentMemS2;
			}//*/

			XMEN_OBJECT . dst_Point	 = (MATH_tdst_Vector	*)XMEN_OBJECT_Points_Smoothed;
			p_TRIS   = XMEN_OBJECT_TRIS;
			XMEN_ELEMENT . l_NbTriangles = ((1<<XMEN_NumberOfSegs_PO2) - 1) << (1 + XMEN_SMOOTH_DEPTH);
			BigStep = XMEN_SMOOTH_DEPTH;
			XMEN_OBJECT . l_NbPoints = ((1<<XMEN_NumberOfSegs_PO2) * 2) << XMEN_SMOOTH_DEPTH;
			XMEN_OBJECT . l_NbUVs	 = ((1<<XMEN_NumberOfSegs_PO2) * 2) << XMEN_SMOOTH_DEPTH;
			fAddV = 1.0f / (float)(((1<<XMEN_NumberOfSegs_PO2) - 1)<<XMEN_SMOOTH_DEPTH);
		} else
		{
			XMEN_OBJECT . dst_Point	 = (&p_XMEN ->p_stXMEN_MEM->stOldPos1)  + (ChlCounter << XMEN_NumberOfSegs_PO2);
			p_TRIS   = XMEN_OBJECT_TRIS;
			XMEN_ELEMENT . l_NbTriangles = ((1<<XMEN_NumberOfSegs_PO2) - 1) << 1;
			BigStep = 0;
			XMEN_OBJECT . l_NbPoints = (1<<XMEN_NumberOfSegs_PO2) * 2;
			XMEN_OBJECT . l_NbUVs	 = (1<<XMEN_NumberOfSegs_PO2) * 2;
			fAddV = 1.0f / (float)((1<<XMEN_NumberOfSegs_PO2) - 1);
		}

		GDI_gpst_CurDD_SPR.p_Current_Vertex_List = XMEN_OBJECT . dst_Point	;
		/* UVS */
		XMEN_ELEMENT . l_MaterialId  = 0;

		ulRingCounter = 0;
		ulRingCounterM1= ((1 << (XMEN_NumberOfSegs_PO2 + BigStep)) - 1); 
		/* Build the first part */
		while ((ulRingCounter + (1 << BigStep) - 1)< (p_XMEN -> ulCurrentRINGentry << BigStep))
		{
			p_TRIS -> auw_UV[0] = p_TRIS -> auw_Index[0] = (unsigned short)((ulRingCounter	<< 1));
			p_TRIS -> auw_UV[1] = p_TRIS -> auw_Index[1] = (unsigned short)((ulRingCounter	<< 1) + 1);
			p_TRIS -> auw_UV[2] = p_TRIS -> auw_Index[2] = (unsigned short)((ulRingCounterM1<< 1) + 1);
			p_TRIS ++;
			p_TRIS -> auw_UV[0] = p_TRIS -> auw_Index[0] = (unsigned short)((ulRingCounter	<< 1));
			p_TRIS -> auw_UV[1] = p_TRIS -> auw_Index[1] = (unsigned short)((ulRingCounterM1<< 1) + 1);
			p_TRIS -> auw_UV[2] = p_TRIS -> auw_Index[2] = (unsigned short)((ulRingCounterM1<< 1) + 0);
			p_TRIS ++;
			ulRingCounterM1 = ulRingCounter++;
		}
		/* Build the second  part */
		if (BigStep && (!p_XMEN -> ulCurrentRINGentry))
		{
			ulRingCounter = 1;
			ulRingCounterM1 = ulRingCounter-1;
		} else
		{
			ulRingCounter += 1 << BigStep;
			ulRingCounterM1 = ulRingCounter-1;
		}

		while (ulRingCounter < (ULONG) (1<<(XMEN_NumberOfSegs_PO2 + BigStep)))
		{
			p_TRIS ->auw_UV[0] = p_TRIS -> auw_Index[0] = (unsigned short)((ulRingCounter	<< 1));
			p_TRIS ->auw_UV[1] = p_TRIS -> auw_Index[1] = (unsigned short)((ulRingCounter	<< 1) + 1);
			p_TRIS ->auw_UV[2] = p_TRIS -> auw_Index[2] = (unsigned short)((ulRingCounterM1	<< 1) + 1);
			p_TRIS ++;
			p_TRIS ->auw_UV[0] = p_TRIS -> auw_Index[0] = (unsigned short)((ulRingCounter	<< 1));
			p_TRIS ->auw_UV[1] = p_TRIS -> auw_Index[1] = (unsigned short)((ulRingCounterM1	<< 1) + 1);
			p_TRIS ->auw_UV[2] = p_TRIS -> auw_Index[2] = (unsigned short)((ulRingCounterM1	<< 1) + 0);
			p_TRIS ++;
			ulRingCounterM1 = ulRingCounter++;
		}

		/* Compute TotalLenght */
		ulRingCounter = ((1<<XMEN_NumberOfSegs_PO2) - p_XMEN -> ulCurrentRINGentry) << BigStep;
		p_UVS	 = &XMEN_OBJECT_UVS[(p_XMEN -> ulCurrentRINGentry << 1) << BigStep];
		p_XYZ	 = &XMEN_OBJECT_Points_Smoothed[(p_XMEN -> ulCurrentRINGentry << 1) << BigStep];
		p_Last_XYZ = NULL;
		fTotalLenght = 0.0f;
		while (ulRingCounter--)
		{
			if (p_Last_XYZ) fTotalLenght += MATH_f_Distance(p_Last_XYZ,p_XYZ);
			p_Last_XYZ = p_XYZ;
			p_XYZ += 2;
		}
		ulRingCounter = p_XMEN -> ulCurrentRINGentry << BigStep;
		p_XYZ	 = XMEN_OBJECT_Points_Smoothed;
		while (ulRingCounter --)
		{
			if (p_Last_XYZ) fTotalLenght += MATH_f_Distance(p_Last_XYZ,p_XYZ);
			p_Last_XYZ = p_XYZ;
			p_XYZ += 2;
		}
		if (fTotalLenght != 0.0f) fTotalLenght = 1.f / fTotalLenght;
		
		
		ulRingCounter = ((1<<XMEN_NumberOfSegs_PO2) - p_XMEN -> ulCurrentRINGentry) << BigStep;
		p_UVS	 = &XMEN_OBJECT_UVS[(p_XMEN -> ulCurrentRINGentry << 1) << BigStep];
		p_XYZ	 = &XMEN_OBJECT_Points_Smoothed[(p_XMEN -> ulCurrentRINGentry << 1) << BigStep];
		fCurrentV = 0.0f;
		fAddV = 0.0f;
		p_Last_XYZ = NULL;
		if (p_XMEN ->ulFlags & MOD_XMEN_TurnText90)
		{
			while (ulRingCounter--)
			{
				if (p_Last_XYZ) fAddV = MATH_f_Distance(p_Last_XYZ,p_XYZ) * fTotalLenght;
				p_Last_XYZ = p_XYZ;
				fCurrentV += fAddV;
				p_UVS->fV = 0.0f;
				p_UVS->fU = fCurrentV;
				p_UVS++;
				p_UVS->fV = 1.0f;
				p_UVS->fU = fCurrentV;
				p_UVS++;
				p_XYZ+= 2;
			}
			ulRingCounter = p_XMEN -> ulCurrentRINGentry << BigStep;
			p_UVS	 = XMEN_OBJECT_UVS;
			p_XYZ	 = XMEN_OBJECT_Points_Smoothed;
			while (ulRingCounter --)
			{
				if (p_Last_XYZ) fAddV = MATH_f_Distance(p_Last_XYZ,p_XYZ) * fTotalLenght;
				p_Last_XYZ = p_XYZ;
				fCurrentV += fAddV;
				p_UVS->fV = 0.0f;
				p_UVS->fU = fCurrentV;
				p_UVS++;
				p_UVS->fV = 1.0f;
				p_UVS->fU = fCurrentV;
				p_UVS++;
				p_XYZ+= 2;
			}
		} else
		{
			while (ulRingCounter--)
			{
				if (p_Last_XYZ) fAddV = MATH_f_Distance(p_Last_XYZ,p_XYZ) * fTotalLenght;
				p_Last_XYZ = p_XYZ;
				fCurrentV += fAddV;
				p_UVS->fU = 0.0f;
				p_UVS->fV = fCurrentV;
				p_UVS++;
				p_UVS->fU = 1.0f;
				p_UVS->fV = fCurrentV;
				p_UVS++;
				p_XYZ+= 2;
			}
			ulRingCounter = p_XMEN -> ulCurrentRINGentry << BigStep;
			p_UVS	 = XMEN_OBJECT_UVS;
			p_XYZ	 = XMEN_OBJECT_Points_Smoothed;
			while (ulRingCounter --)
			{
				if (p_Last_XYZ) fAddV = MATH_f_Distance(p_Last_XYZ,p_XYZ) * fTotalLenght;
				p_Last_XYZ = p_XYZ;
				fCurrentV += fAddV;
				p_UVS->fU = 0.0f;
				p_UVS->fV = fCurrentV;
				p_UVS++;
				p_UVS->fU = 1.0f;
				p_UVS->fV = fCurrentV;
				p_UVS++;
				p_XYZ+= 2;
			}
		}

#ifndef JADEFUSION
		if (p_XMEN ->ulFlags & MOD_XMEN_TransMat)
		{
			GAO_XMN_Render_TransparentTriangles( &XMEN_OBJECT , p_XMEN->p_MaterialUsed );
		} 
		else
		{
			MAT_DrawIndexedTriangle(&XMEN_OBJECT,p_XMEN->p_MaterialUsed,&XMEN_ELEMENT);
		}
#else
		GAO_XMN_Render_Xenon( p_XMEN,&XMEN_OBJECT , p_XMEN->p_MaterialUsed );
#endif
	}
	SOFT_MatrixStack_Reset(&GDI_gpst_CurDD->st_MatrixStack, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);		
}
void GAO_ModifierXMEN_DRAW_ALL(void)
{

	GDI_gpst_CurDD_SPR.pus_ReorderBuffer = NULL;
	GDI_gpst_CurDD_SPR.ul_DisplayInfo &= ~GDI_Cul_DI_UseSpecialVertexBuffer;
	GDI_gpst_CurDD_SPR.ul_CurrentDrawMask &= ~GDI_Cul_DM_TestBackFace;
	GDI_gpst_CurDD_SPR.ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
	

	g_ulFrameCounter ++;
	while (XMEN_STACK_PTR--)
	{
		if (p_XMEN_STACK[XMEN_STACK_PTR])	
		{
			if (p_XMEN_STACK[XMEN_STACK_PTR]->ulFrameCounter != g_ulFrameCounter)
			{
				GAO_ModifierXMEN_DRAW(p_XMEN_STACK[XMEN_STACK_PTR]);//*/
			}
			p_XMEN_STACK[XMEN_STACK_PTR]->ulFrameCounter = g_ulFrameCounter;
		}
	}
	XMEN_STACK_PTR = 0;
}


void GAO_ModifierXMEN_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	GAO_tdst_ModifierXMEN *p_XMEN;

	p_XMEN = (GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data;

	/* Update Positions */
	p_XMEN_STACK[(XMEN_STACK_PTR++) & (MAX_XMEN_NODES - 1)] = p_XMEN;
}
/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierXMEN_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierXMEN_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierXMEN *p_XMEN;
	p_XMEN = (GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data;
	_pst_Mod ->ul_Flags |= MDF_C_Modifier_ApplyGao;
	if(!(p_XMEN ->ulFlags & MOD_XMEN_Validate))
	{
		if(p_XMEN ->p_MaterialUsed)
		{
			p_XMEN->p_MaterialUsed->st_Id.i->pfn_AddRef(p_XMEN->p_MaterialUsed, 1);
			p_XMEN ->ulFlags |= MOD_XMEN_Validate;
		}

		if(p_XMEN->ulNumber_Of_Chhlaahhh)
		{
			if(!p_XMEN ->p_stXMEN_MEM)
			{
				p_XMEN ->p_stXMEN_MEM = (GAO_tdst_XMEN_Chhlaahhh_Memory *)MEM_p_Alloc(sizeof(GAO_tdst_XMEN_Chhlaahhh_Memory) * (p_XMEN -> ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2));		
			}
			if(!p_XMEN ->p_XmenLocalLenght)
			{
				p_XMEN ->p_XmenLocalLenght = (float *)MEM_p_Alloc(sizeof(float) * (p_XMEN -> ulNumber_Of_Chhlaahhh  << XMEN_NumberOfSegs_PO2));		
			}
			
		}
		else
		{
			p_XMEN ->p_stXMEN_MEM = NULL;
			p_XMEN ->p_XmenLocalLenght = NULL;
		}
	}
	else
	{
		L_memset(p_XMEN ->p_stXMEN_MEM, 0, sizeof(GAO_tdst_XMEN_Chhlaahhh_Memory) * (p_XMEN -> ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2));
		L_memset(p_XMEN ->p_XmenLocalLenght, 0, sizeof(float) * (p_XMEN -> ulNumber_Of_Chhlaahhh  << XMEN_NumberOfSegs_PO2));
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG GAO_ModifierXMEN_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	GAO_tdst_ModifierXMEN *p_XMEN;
	char *_pc_BufferSave;
	ULONG ulVersion;
	XMEN_FirstInit();
	_pc_BufferSave = _pc_Buffer;
	p_XMEN = (GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data;
	/* size */
	LOA_ReadLong_Ed(&_pc_Buffer, NULL); // skip size

	/* version */
	ulVersion = LOA_ReadULong(&_pc_Buffer);

	/* flags */
	p_XMEN->ulFlags = LOA_ReadULong(&_pc_Buffer);
#if !defined(XML_CONV_TOOL)
	p_XMEN->ulFlags &= ~MOD_XMEN_Validate;
#endif
	/* Chlahhh */
	p_XMEN->ulNumber_Of_Chhlaahhh = LOA_ReadULong(&_pc_Buffer);
	p_XMEN->p_st_Chhlaahhh = NULL;
	if(p_XMEN->ulNumber_Of_Chhlaahhh) p_XMEN->p_st_Chhlaahhh = (GAO_tdst_XMEN_Chhlaahhh*)MEM_p_Alloc(p_XMEN->ulNumber_Of_Chhlaahhh * sizeof(GAO_tdst_XMEN_Chhlaahhh));
	{
		ULONG i;
		GAO_tdst_XMEN_Chhlaahhh * p;
		for(i = 0, p = p_XMEN->p_st_Chhlaahhh; i < p_XMEN->ulNumber_Of_Chhlaahhh; ++i, ++p)
		{
			p->ulBonesNum = LOA_ReadULong(&_pc_Buffer);
			p->fLenght = LOA_ReadFloat(&_pc_Buffer);
		}
	}

	p_XMEN ->p_stXMEN_MEM = NULL;
	p_XMEN ->p_XmenLocalLenght = NULL;
	if(p_XMEN -> ulNumber_Of_Chhlaahhh)
	{
		p_XMEN ->p_stXMEN_MEM = (GAO_tdst_XMEN_Chhlaahhh_Memory *)MEM_p_Alloc(sizeof(GAO_tdst_XMEN_Chhlaahhh_Memory) * (p_XMEN -> ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2));
		memset(p_XMEN ->p_stXMEN_MEM ,0,sizeof(GAO_tdst_XMEN_Chhlaahhh_Memory) * (p_XMEN -> ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2));
		p_XMEN ->p_XmenLocalLenght = (float *)MEM_p_Alloc(sizeof(float) * (p_XMEN -> ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2));
		memset(p_XMEN ->p_XmenLocalLenght ,0,sizeof(float) * (p_XMEN -> ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2));
	}

	/* Material */
	p_XMEN->bk_MaterialUsed = LOA_ReadULong(&_pc_Buffer);
	p_XMEN ->p_MaterialUsed = NULL;

	if (ulVersion > 0)
	{
		/* load projection method */
		p_XMEN->ulProjectionMethod = LOA_ReadULong(&_pc_Buffer);
	}

	if (ulVersion > 1)
	{
		/* load DT Min*/
		p_XMEN->fDTMin = LOA_ReadFloat(&_pc_Buffer);
	}

	if (ulVersion > 2)
	{
		/* load projection method */
		p_XMEN->ulUserID = LOA_ReadULong(&_pc_Buffer);
	}

#ifdef ACTIVE_EDITORS
	p_XMEN->ulCodeKey = 0xC0DE2001;
	p_XMEN->ulOLD_Number_Of_Chhlaahhh = p_XMEN->ulNumber_Of_Chhlaahhh;
	p_XMEN ->bSmoothed = 0;
	p_XMEN ->bGlobalMatrix = 0;
	p_XMEN ->bMaterialIsTransparent = 0;
	if (p_XMEN->ulFlags & MOD_XMEN_Smooth) p_XMEN ->bSmoothed = 1;
	if (p_XMEN->ulFlags & MOD_XMEN_GlobalMatrix) p_XMEN ->bGlobalMatrix = 1;
	if (p_XMEN->ulFlags & MOD_XMEN_TransMat) p_XMEN ->bMaterialIsTransparent = 1;
	if (p_XMEN->ulFlags & MOD_XMEN_Centered) p_XMEN ->bCentered = 1;
	if (p_XMEN->ulFlags & MOD_XMEN_TurnText90) p_XMEN ->bTurn90 = 1;
#endif



	p_XMEN -> pst_GO = _pst_Mod->pst_GO;
	p_XMEN -> ulFrameCounter = 0;
	p_XMEN -> ulCurrentRINGentry = 0;

	if (!(p_XMEN ->ulFlags & MOD_XMEN_Validate))
	{
		if (p_XMEN -> bk_MaterialUsed != 0)
		{
			if (p_XMEN -> bk_MaterialUsed != 0xffffffff)
				LOA_MakeFileRef(p_XMEN -> bk_MaterialUsed, (ULONG *) &p_XMEN ->p_MaterialUsed, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
		}//*/
	}

	return _pc_Buffer - _pc_BufferSave ;
}

#ifdef ACTIVE_EDITORS
/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierXMEN_Save(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierXMEN *p_XMEN;
	ULONG ulSize;

	p_XMEN = (GAO_tdst_ModifierXMEN *)_pst_Mod->p_Data;

   /* Save Size */
	ulSize = 4/* Version */ + 4/* Flags */ + 4/* Numch */ + p_XMEN->ulNumber_Of_Chhlaahhh * sizeof(GAO_tdst_XMEN_Chhlaahhh) + 
			4 /* Material BK */ + 4/* projection method */ + 4 /* XMEN ID*/;
			
	SAV_Buffer(&ulSize, 4); 
   /* Save version */
	ulSize = 3;
	SAV_Buffer(&ulSize, 4); 
	/* Save flags */
	SAV_Buffer(&p_XMEN->ulFlags, 4); 
	/* Save Chlahhh */
	SAV_Buffer(&p_XMEN->ulNumber_Of_Chhlaahhh, 4);
	SAV_Buffer(p_XMEN->p_st_Chhlaahhh, p_XMEN->ulNumber_Of_Chhlaahhh * sizeof(GAO_tdst_XMEN_Chhlaahhh));
	/* Save Material */
	SAV_Buffer(&p_XMEN->bk_MaterialUsed, 4);
	/* Save projection method */
	SAV_Buffer(&p_XMEN->ulProjectionMethod, 4);
	/* Save fDTMin */
	SAV_Buffer(&p_XMEN->fDTMin, 4);
	/* Save fDTMin */
	SAV_Buffer(&p_XMEN->ulUserID, 4);

	
}
	
#endif

#ifdef JADEFUSION
void GAO_XMN_Render_Xenon( GAO_tdst_ModifierXMEN *p_XMEN,GEO_tdst_Object *pst_Obj, MAT_tdst_Material *p_MaterialUsed )
{
		GEO_tdst_IndexedTriangle	*t, *tend;
		GEO_Vertex                  *_pst_Point;
		GEO_tdst_UV                 *_pst_UV;
		ULONG						*pst_Color;
		int	TNum,iSizeOfElements;

		TNum = pst_Obj->dst_Element->l_NbTriangles;
		t = pst_Obj->dst_Element->dst_Triangle;
		tend = pst_Obj->dst_Element->dst_Triangle + TNum;
		
		_pst_Point = XMEN_OBJECT.dst_Point;
		pst_Color = XMEN_OBJECT.dul_PointColors;
		_pst_UV = XMEN_OBJECT.dst_UV;
		
		g_pXmenMesh = g_oXeRenderer.RequestDynamicMesh();
		
		iSizeOfElements = sizeof(XeRenderer::XeVertexDyn);
		g_pXmenBuffer = (XeBuffer*)g_pXmenMesh->GetStream(0)->pBuffer;
		g_pXmenMesh->SetStreamComponents(0, XEVC_POSITION | XEVC_COLOR0 | XEVC_TEXCOORD0);

		g_pXmenBufferArray = (XeRenderer::XeVertexDyn *)g_pXmenBuffer->Lock(TNum * 3, iSizeOfElements);

		//-- FILL --
		while(t < tend)
		{
		    *(GEO_Vertex *) &g_pXmenBufferArray->vPos = _pst_Point[t->auw_Index[0]];
		    g_pXmenBufferArray->ulColor = 0xffffffff;//XeConvertColor(pst_Color[t->auw_Index[0]]);
			g_pXmenBufferArray->UV.fU = _pst_UV[t->auw_Index[0]].fU;
			g_pXmenBufferArray->UV.fV = _pst_UV[t->auw_Index[0]].fV;
			g_pXmenBufferArray++;

		    *(GEO_Vertex *) &g_pXmenBufferArray->vPos = _pst_Point[t->auw_Index[1]];
		    g_pXmenBufferArray->ulColor = 0xffffffff;//XeConvertColor(pst_Color[t->auw_Index[1]]);
			g_pXmenBufferArray->UV.fU = _pst_UV[t->auw_Index[1]].fU;
			g_pXmenBufferArray->UV.fV = _pst_UV[t->auw_Index[1]].fV;
			g_pXmenBufferArray++;
			
		    *(GEO_Vertex *) &g_pXmenBufferArray->vPos = _pst_Point[t->auw_Index[2]];
		    g_pXmenBufferArray->ulColor = 0xffffffff;//XeConvertColor(pst_Color[t->auw_Index[2]]);
			g_pXmenBufferArray->UV.fU = _pst_UV[t->auw_Index[2]].fU;
			g_pXmenBufferArray->UV.fV = _pst_UV[t->auw_Index[2]].fV;
			g_pXmenBufferArray++;
			
			t++;
		}
		//----------
		//pVB->Unlock();
		g_pXmenBuffer->Unlock();

        MAT_tdst_MultiTexture	*pst_MLTTX = (MAT_tdst_MultiTexture *)p_MaterialUsed;//(MAT_tdst_MultiTexture *) pst_Material;
        MAT_tdst_MTLevel		*pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;

		eXeRENDERLISTTYPE eRT = (p_XMEN ->ulFlags & MOD_XMEN_TransMat) ? XeRT_TRANSPARENT : XeRT_OPAQUE;
		pst_MLTTXLVL->pst_XeMaterial->SetTwoSided(TRUE);

		g_oXeRenderer.QueueMeshForRender(   p_XMEN->pst_GO->pst_GlobalMatrix,
                                                        g_pXmenMesh, // use any mesh, will be changed anyway
                                                        pst_MLTTXLVL->pst_XeMaterial,
                                                        GDI_gpst_CurDD->ul_CurrentDrawMask,
                                                        -1,
														eRT,
                                                        XeRenderObject::TriangleList,
														p_XMEN->pst_GO->uc_LOD_Vis,
                                                        GDI_gpst_CurDD->g_cZListCurrentDisplayOrder,
                                                        p_XMEN->pst_GO, 
                                                        XeRenderObject::Common,
                                                        0,
                                                        GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted ? QMFR_LIGHTED : 0);
		
 /*                       g_oXeRenderer.QueueMeshForRender(   _pst_GO->pst_GlobalMatrix,
                                                            pst_Visu->p_XeElements[iElem].pst_Mesh,
                                                            pst_MLTTXLVL->pst_XeMaterial,
                                                            GDI_gpst_CurDD->ul_CurrentDrawMask,
                                                            ulLMTexID,
                                                            eRT,
                                                            XeRenderObject::TriangleList,
                                                            _pst_GO->uc_LOD_Vis,
							                                GDI_gpst_CurDD->g_cZListCurrentDisplayOrder,
                                                            _pst_GO,
                                                            pst_Obj->b_Particles ? XeRenderObject::Particles : XeRenderObject::Common,
                                                            0,
                                                            pst_Visu->p_XeElements[iElem].pst_Mesh->IsDynamic() ? ulExtraFlags : (ulExtraFlags | QMFR_GAO),
                                                            ulLayerIndex);
*/			
			g_pXmenMesh = NULL;
}
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

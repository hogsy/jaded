/*$T MDFmodifier_SDW.c GC! */


/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
//#define USE_FULL_SCENE_SHADOWS

#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "MATHs/MATHmatrix.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SDW.h"
#include "GEOmetric/GEOobject.h"
#include "MATerial/MATShadow.h"
#include "GX8/Gx8renderstate.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
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
#endif
#endif

#include "BASe/BENch/BENch.h"

#ifndef __GX8LIGHT_H__
#include "GX8/Gx8Light.h"
#endif

#ifdef _XBOX
#include "GX8/RASter/Gx8_CheatFlags.h"
#endif // _XBOX

#define DBG_SHADOWS_SCREEN_CULLING_ON
#define DBG_SHADOWS_INTERSETCION_CULLING_ON
#define DBG_SHADOWS_UV_CULLING_ON
#define DBG_SHADOWS_UV_TILLING_OFF
#define DBG_SHADOWS_SHOWCLIPPED

void SDW_SmoothShadow(ULONG *pUsedShadows);


#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#if defined(__CW__) && defined(RELEASE)
#pragma always_inline off			/* can't pass with CW */
#endif
static ULONG SDW_TextNUm = 0;
#define DEF_fCoefAlpha 0.2f
	ULONG SDW_ul_Interpol2Colors(ULONG ulP2, float fZClipLocalCoef)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	RetValue, Interpoler /*, ulAlpha*/;
		float	TmpRetValue=0.f;
 		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
 		/*	*((float *) &ulAlpha) = ((DEF_fCoefAlpha * fZClipLocalCoef) + 0.5f) + 32768.0f + 16384.0f;
		ulAlpha <<= 24;*/
		
		if(fZClipLocalCoef >= 1.0f) 
			return 0x02000000 | ulP2;
		if(fZClipLocalCoef >= 0.68f) return ulP2 | 0x01000000;
		if(fZClipLocalCoef <= 0.02f) 
			return 0x01000000;
		*((float *) &Interpoler) = fZClipLocalCoef + 32768.0f + 16384.0f;
		RetValue = (Interpoler & 128) ? (ulP2 & 0xfefefefe) >> 1 : 0 ;
		RetValue += (Interpoler & 64) ? (ulP2 & 0xfcfcfcfc) >> 2 : 0 ;
		RetValue += (Interpoler & 32) ? (ulP2 & 0xf8f8f8f8) >> 3 : 0 ;
		RetValue += (Interpoler & 16) ? (ulP2 & 0xf0f0f0f0) >> 4 : 0 ;
		RetValue += (Interpoler & 8) ?  (ulP2 & 0xe0e0e0e0) >> 5 : 0 ;
		RetValue += (Interpoler & 4) ?  (ulP2 & 0xc0c0c0c0) >> 6 : 0 ;
		RetValue += (Interpoler & 2) ?  (ulP2 & 0x80808080) >> 7 : 0 ;

		return RetValue | 0x01000000;
	}
	
	/*
	=======================================================================================================================
	GLOBAL SHADOW MANAGEMENT
	=======================================================================================================================
	*/
	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
	
	DD_tdst_ShadowStack *SDW_Create(void)
	{
		DD_tdst_ShadowStack *p_ShadowStack;
		p_ShadowStack = (DD_tdst_ShadowStack*)MEM_p_Alloc(sizeof(DD_tdst_ShadowStack));
		p_ShadowStack->ulNumberOfNodes =  MDF_MemoryGranularity;
		p_ShadowStack->ulLastNode = 0;
		p_ShadowStack->pp_Nodes =  (DD_tdst_ShadowStackNode *)MEM_p_Alloc(sizeof(DD_tdst_ShadowStackNode) * p_ShadowStack->ulNumberOfNodes);
		L_memset(p_ShadowStack->pp_Nodes , 0 , sizeof(DD_tdst_ShadowStackNode *) * p_ShadowStack->ulNumberOfNodes);
		return p_ShadowStack; 
	}
	
	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
	
	void SDW_Destroy(void)
	{
		DD_tdst_ShadowStack *p_ShadowStack;

        SDW_Clear();
		p_ShadowStack = (DD_tdst_ShadowStack *) GDI_gpst_CurDD->pv_ShadowStack;
		if (!p_ShadowStack) return;
		MEM_Free(p_ShadowStack->pp_Nodes);
		MEM_Free(p_ShadowStack);
		GDI_gpst_CurDD->pv_ShadowStack = NULL;
	}
	
	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
static float SDW_RDR_XS = 64.0f;
static float SDW_RDR_YS = 64.0f;
static int SDW_RDR_SHIFT = 0x2;
static int SDW_RDR_SHIFTY = 0x2;
static int SDW_RDR_AND   = 0x3;

	void SDW_DispatchTextures(void)
	{
		ULONG TExtnmumLocal;
		if (!SDW_TextNUm) return;
		SDW_RDR_XS = 256.0f;
		SDW_RDR_YS = 128.0f;
		SDW_RDR_SHIFT = 0;
		SDW_RDR_SHIFTY = 0;
		TExtnmumLocal = SDW_TextNUm - 1;
//		if (TExtnmumLocal < 10) TExtnmumLocal = 65;
		while (TExtnmumLocal)
		{
			SDW_RDR_XS *= 0.5f;
			SDW_RDR_SHIFT++;
			TExtnmumLocal >>= 1;
			if (TExtnmumLocal)
			{
				SDW_RDR_YS *= 0.5f;
				SDW_RDR_SHIFTY ++;
				TExtnmumLocal >>= 1;
			}
		}
//		SDW_RDR_SHIFT = 0;
		SDW_RDR_AND = (1<<SDW_RDR_SHIFT) - 1;
	}
	
	void SDW_AddAShadow(DD_tdst_ShadowStackNode * p_NewShadow)
	{
		DD_tdst_ShadowStack *p_ShadowStack;
		p_ShadowStack = (DD_tdst_ShadowStack *) GDI_gpst_CurDD->pv_ShadowStack;
		if (!p_ShadowStack) 
			p_ShadowStack = SDW_Create();
		if (p_ShadowStack->ulLastNode == p_ShadowStack->ulNumberOfNodes )
		{
			p_ShadowStack->ulNumberOfNodes += MDF_MemoryGranularity;
			p_ShadowStack->pp_Nodes =  (DD_tdst_ShadowStackNode	*)MEM_p_Realloc(p_ShadowStack->pp_Nodes,sizeof(DD_tdst_ShadowStackNode) * p_ShadowStack->ulNumberOfNodes);
			L_memset(&p_ShadowStack->pp_Nodes[p_ShadowStack->ulLastNode], 0 , sizeof(DD_tdst_ShadowStackNode) * MDF_MemoryGranularity);
		}
		if (p_NewShadow -> p_FatherModifier->ulFlags & MDF_SDW_1RealTime)
		{
			p_NewShadow-> RDR_WindowAffectation = 0x80000000 | SDW_TextNUm++;
			SDW_DispatchTextures();
		}
		p_ShadowStack->pp_Nodes[p_ShadowStack->ulLastNode++] = *p_NewShadow;
		GDI_gpst_CurDD->pv_ShadowStack = (void *) p_ShadowStack ;
	}
	
	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
	
	void SDW_Clear()
	{
		DD_tdst_ShadowStack *p_ShadowStack;
		p_ShadowStack = (DD_tdst_ShadowStack *) GDI_gpst_CurDD->pv_ShadowStack;
		if (!p_ShadowStack) return;
		p_ShadowStack ->ulLastNode = 0;
	}
	
	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
	extern void GAO_ModifierShadow_Apply_All();
	void SDW_UpdatePositions()
	{
		DD_tdst_ShadowStack *p_ShadowStack;
		DD_tdst_ShadowStackNode *pp_Frst,*pp_Lst;
		MATH_tdst_Vector 	stLOCAL,stU1,stV1,stWorldVectV,stWorldVectU;
		MATH_tdst_Vector	stBBSize;
		MATH_tdst_Vector	stCullingPlane[5];
		float				fCullingDistances[5];
		ULONG				ulPlaneCounter , ulCullingResult;
		ULONG	ulBoxVertexCounter;
		
		SDW_TextNUm = 0;
		GAO_ModifierShadow_Apply_All();

#if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)	
		
		p_ShadowStack = (DD_tdst_ShadowStack *)GDI_gpst_CurDD -> pv_ShadowStack;
		if (!p_ShadowStack) return;
		
		
		MATH_TransformVector(&stCullingPlane[0], &GDI_gpst_CurDD ->st_Camera.st_Matrix, &GDI_gpst_CurDD ->st_Camera.st_NormPlaneUp);
		MATH_TransformVector(&stCullingPlane[1], &GDI_gpst_CurDD ->st_Camera.st_Matrix, &GDI_gpst_CurDD ->st_Camera.st_NormPlaneDown);
		MATH_TransformVector(&stCullingPlane[2], &GDI_gpst_CurDD ->st_Camera.st_Matrix, &GDI_gpst_CurDD ->st_Camera.st_NormPlaneLeft);
		MATH_TransformVector(&stCullingPlane[3], &GDI_gpst_CurDD ->st_Camera.st_Matrix, &GDI_gpst_CurDD ->st_Camera.st_NormPlaneRight);
		stLOCAL . x = stLOCAL . y = 0.0f;
		stLOCAL . z = 1.0f;
		MATH_TransformVector(&stCullingPlane[4], &GDI_gpst_CurDD ->st_Camera.st_Matrix, &stLOCAL);
		ulPlaneCounter =  5;
		while (ulPlaneCounter--)
		{
			MATH_NegVector(&stCullingPlane[ulPlaneCounter] , &stCullingPlane[ulPlaneCounter] );
			fCullingDistances[ulPlaneCounter] = MATH_f_DotProduct(&stCullingPlane[ulPlaneCounter],&GDI_gpst_CurDD ->st_Camera.st_Matrix.T);
		}
		
		pp_Frst = p_ShadowStack ->pp_Nodes;
		pp_Lst  = pp_Frst + p_ShadowStack -> ulLastNode;
		while (pp_Frst < pp_Lst )
		{
			float Norm,fAlpha,XCoef,YCoef,ZCoef;
			pp_Frst ->p_FatherModifier->ulFlags |= MDF_SDW_IsCulled;
			
			pp_Frst -> p_FatherModifier -> ulFlags |= MDF_SDW_IsUpdatedOnThisDisplayData;

			// Traffic pour pas sortir de la BV
			{
				/*float factor=0.5f;	        
				if (SDW_GET_FK(pp_Frst -> p_FatherModifier->pst_GO) == 0x72006cf9)//Kong key
	            {
					factor=0.8f;
				}*/
				float factor=1.2f;
				XCoef = (pp_Frst -> p_FatherModifier -> XSizeFactor *factor);// +1.0f * factor;
				YCoef = (pp_Frst -> p_FatherModifier -> YSizeFactor *factor);// +1.0f * factor;
			}

			ZCoef = pp_Frst -> p_FatherModifier -> ZSizeFactor;
			MATH_InitVector(&stBBSize,1.0f,1.0f,1.0f);
			if	(OBJ_b_TestIdentityFlag(pp_Frst -> p_FatherModifier->pst_GO, OBJ_C_IdentityFlag_OBBox))
			{
				MATH_tdst_Vector    *p_LMinSDW, *p_LMaxSDW;
				p_LMinSDW = OBJ_pst_BV_GetLMin(pp_Frst -> p_FatherModifier->pst_GO->pst_BV);
				p_LMaxSDW = OBJ_pst_BV_GetLMax(pp_Frst -> p_FatherModifier->pst_GO->pst_BV);
				stBBSize.x *= p_LMaxSDW->x - p_LMinSDW->x;
				stBBSize.y *= p_LMaxSDW->y - p_LMinSDW->y;
				stBBSize.z *= p_LMaxSDW->z - p_LMinSDW->z;
			} else
			if (OBJ_BV_IsAABBox(pp_Frst -> p_FatherModifier->pst_GO->pst_BV))
			{
				MATH_tdst_Vector    *p_LMinSDW, *p_LMaxSDW;
				p_LMinSDW = OBJ_pst_BV_GetGMin(pp_Frst -> p_FatherModifier->pst_GO->pst_BV);
				p_LMaxSDW = OBJ_pst_BV_GetGMax(pp_Frst -> p_FatherModifier->pst_GO->pst_BV);
				stBBSize.x *= p_LMaxSDW->x - p_LMinSDW->x;
				stBBSize.y *= p_LMaxSDW->y - p_LMinSDW->y;
				stBBSize.z *= p_LMaxSDW->z - p_LMinSDW->z;
			} 
			
			/* OK ============================= Shadow center position =========================== */
			if (OBJ_b_TestIdentityFlag(pp_Frst -> p_FatherModifier->pst_GO, OBJ_C_IdentityFlag_OBBox))
			{
				/* this solve the problem of exentred center */
				MATH_BlendVector(&pp_Frst -> stWorldCenter , OBJ_pst_BV_GetLMin(pp_Frst -> p_FatherModifier->pst_GO->pst_BV) , OBJ_pst_BV_GetLMax(pp_Frst -> p_FatherModifier->pst_GO->pst_BV) , 0.5f);
				MATH_TransformVertex(&pp_Frst -> stWorldCenter, pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix , &pp_Frst -> stWorldCenter);
			} else
			if (OBJ_BV_IsAABBox(pp_Frst -> p_FatherModifier->pst_GO->pst_BV))
			{
				MATH_BlendVector(&pp_Frst -> stWorldCenter , OBJ_pst_BV_GetGMin(pp_Frst -> p_FatherModifier->pst_GO->pst_BV) , OBJ_pst_BV_GetGMax(pp_Frst -> p_FatherModifier->pst_GO->pst_BV) , 0.5f);
				MATH_AddVector(&pp_Frst -> stWorldCenter,&pp_Frst -> stWorldCenter,OBJ_pst_GetAbsolutePosition(pp_Frst -> p_FatherModifier -> pst_GO));
			} else
				MATH_CopyVector(&pp_Frst -> stWorldCenter,&pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix -> T);
			MATH_AddScaleVector(&pp_Frst -> stWorldCenter , &pp_Frst -> stWorldCenter, &pp_Frst -> stWorldVectShadow , -pp_Frst -> p_FatherModifier ->ZStart);
			/* Delta X,Y & Z */
			MATH_AddScaleVector(&pp_Frst -> stWorldCenter,&pp_Frst -> stWorldCenter,MATH_pst_GetXAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix), pp_Frst -> p_FatherModifier -> stCenter . x);
			MATH_AddScaleVector(&pp_Frst -> stWorldCenter,&pp_Frst -> stWorldCenter,MATH_pst_GetYAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix), pp_Frst -> p_FatherModifier -> stCenter . y);
			MATH_AddScaleVector(&pp_Frst -> stWorldCenter,&pp_Frst -> stWorldCenter,MATH_pst_GetZAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix), pp_Frst -> p_FatherModifier -> stCenter . z);
			
			/* ? ===================================		XYZ		==================================== */
			switch (pp_Frst ->p_FatherModifier->ulProjectionMethod)
			{
			case MDF_SDW_Projection_PlaneXY:
				XCoef *= stBBSize.x;
				YCoef *= stBBSize.y;
				MATH_CopyVector(&stWorldVectV , MATH_pst_GetXAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				MATH_CopyVector(&stWorldVectU , MATH_pst_GetYAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				MATH_CopyVector(&pp_Frst -> stWorldVectZ , MATH_pst_GetZAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				break;
			case MDF_SDW_Projection_PlaneXZ:
				XCoef *= stBBSize.x;
				YCoef *= stBBSize.z;
				MATH_CopyVector(&stWorldVectV , MATH_pst_GetXAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				MATH_CopyVector(&stWorldVectU , MATH_pst_GetZAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				MATH_CopyVector(&pp_Frst -> stWorldVectZ , MATH_pst_GetYAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				break;
			case MDF_SDW_Projection_PlaneYZ:
				XCoef *= stBBSize.y;
				YCoef *= stBBSize.z;
				MATH_CopyVector(&stWorldVectV ,	MATH_pst_GetYAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				MATH_CopyVector(&stWorldVectU ,	MATH_pst_GetZAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				MATH_CopyVector(&pp_Frst -> stWorldVectZ , MATH_pst_GetXAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				break;
			case MDF_SDW_Projection_CylinderX:
				XCoef *= stBBSize.x;
				MATH_CopyVector(&stWorldVectV , MATH_pst_GetXAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				MATH_CrossProduct(&stWorldVectU , &stWorldVectV ,&pp_Frst -> stWorldVectShadow);
				MATH_NormalizeVector( &stWorldVectU , &stWorldVectU );
				MATH_CrossProduct(&pp_Frst -> stWorldVectZ , &stWorldVectV , &stWorldVectU);
				Norm = MATH_f_DotProduct(MATH_pst_GetYAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix),&stWorldVectU);
				if (Norm < 0.0f) Norm =- Norm;
				YCoef *= Norm * stBBSize.y + (1.0f - Norm) * stBBSize.z;
				break;
			case MDF_SDW_Projection_CylinderY:
				XCoef *= stBBSize.y;
				MATH_CopyVector(&stWorldVectV , MATH_pst_GetYAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				MATH_CrossProduct(&stWorldVectU , &stWorldVectV ,&pp_Frst -> stWorldVectShadow);
				MATH_NormalizeVector( &stWorldVectU , &stWorldVectU );
				MATH_CrossProduct(&pp_Frst -> stWorldVectZ , &stWorldVectV , &stWorldVectU);
				Norm = MATH_f_DotProduct(MATH_pst_GetXAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix),&stWorldVectU);
				if (Norm < 0.0f) Norm =- Norm;
				YCoef *= Norm * stBBSize.x + (1.0f - Norm) * stBBSize.z;
				break;
			case MDF_SDW_Projection_CylinderZ:
				XCoef *= stBBSize.z;
				MATH_CopyVector(&stWorldVectV , MATH_pst_GetZAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix));
				MATH_CrossProduct(&stWorldVectU , &stWorldVectV ,&pp_Frst -> stWorldVectShadow);
				MATH_NormalizeVector( &stWorldVectU , &stWorldVectU );
				MATH_CrossProduct(&pp_Frst -> stWorldVectZ , &stWorldVectV , &stWorldVectU);
				Norm = MATH_f_DotProduct(MATH_pst_GetXAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix),&stWorldVectU);
				if (Norm < 0.0f) Norm =- Norm;
				YCoef *= Norm * stBBSize.x + (1.0f - Norm) * stBBSize.y;
				break;
			case MDF_SDW_Projection_Spherical:
				{
					float XC/*,YC*/;
					stLOCAL = pp_Frst -> stWorldVectShadow;
					if (stLOCAL . z < 0.0f) stLOCAL . z = -stLOCAL . z;
	/*				XCoef *= (stBBSize.x + stBBSize.y + stBBSize.z) * 0.33333333f;
					YCoef = XCoef;*/
					XC = fSqrt(stBBSize.x * stBBSize.x + stBBSize.y * stBBSize.y);
					YCoef *= XC;
					XCoef *= stLOCAL . z * XC + fSqrt(1.0f - stLOCAL.z * stLOCAL.z) * stBBSize.z;
					Norm = MATH_f_DotProduct(MATH_pst_GetZAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix), &pp_Frst -> stWorldVectShadow);
					if ((Norm < 0.999f) && (Norm > -0.999f))
						MATH_CrossProduct(&stWorldVectU , MATH_pst_GetZAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix) ,&pp_Frst -> stWorldVectShadow);
					else 
						MATH_CrossProduct(&stWorldVectU , MATH_pst_GetXAxis(pp_Frst -> p_FatherModifier -> pst_GO -> pst_GlobalMatrix) ,&pp_Frst -> stWorldVectShadow);
					MATH_NormalizeVector( &stWorldVectU , &stWorldVectU );
					MATH_CrossProduct(&stWorldVectV , &stWorldVectU ,&pp_Frst -> stWorldVectShadow);
					MATH_CrossProduct(&pp_Frst -> stWorldVectZ  , &stWorldVectU , &stWorldVectV );
				}
				break;
			}
//			if (pp_Frst ->p_FatherModifier->ulProjectionMethod == MDF_SDW_Projection_Spherical)

			/* OK =================================       Rotations      ================================= */
			if (pp_Frst ->p_FatherModifier -> ulFlags & MDF_SDW_180Turn)
			{
				MATH_NegVector(&stWorldVectV , &stWorldVectV );
				MATH_NegVector(&stWorldVectU , &stWorldVectU );
			}
			
			if (pp_Frst ->p_FatherModifier -> ulFlags & MDF_SDW_90Turn)
			{
				MATH_CopyVector(&stLOCAL , &stWorldVectV);
				MATH_NegVector(&stWorldVectV , &stWorldVectU);
				MATH_CopyVector(&stWorldVectU , &stLOCAL);
				Norm = YCoef;
				YCoef = XCoef;
				XCoef = Norm;
			}
			
			/* OK =========================  Eliminate shadow plane XY ========================== */
			MATH_AddScaleVector(&stWorldVectV,&stWorldVectV , &pp_Frst -> stWorldVectShadow , -MATH_f_DotProduct(&stWorldVectV,&pp_Frst -> stWorldVectShadow) );
			MATH_AddScaleVector(&stWorldVectU,&stWorldVectU , &pp_Frst -> stWorldVectShadow , -MATH_f_DotProduct(&stWorldVectU,&pp_Frst -> stWorldVectShadow) );
			
			/* ? ==================== Scale UV to make it fit to parrams & BV ================== */
			MATH_ScaleVector(&stWorldVectV, &stWorldVectV , XCoef);
			MATH_ScaleVector(&stWorldVectU, &stWorldVectU , YCoef);
			
			/* OK =========================== Make Z Go in the Right way ======================== */
			if (MATH_f_DotProduct(&pp_Frst -> stWorldVectZ,&pp_Frst ->stWorldVectShadow) < 0.f)
				MATH_NegVector(&pp_Frst -> stWorldVectZ , &pp_Frst -> stWorldVectZ);
			
			/* ? ============== Solve the problem of non perpendicular U & V axis ============== */
			MATH_NormalizeVector(&stU1, &stWorldVectU);
			MATH_NormalizeVector(&stV1, &stWorldVectV);
			fAlpha = MATH_f_DotProduct(&stU1,&stV1);
			MATH_ScaleVector(&stLOCAL, &stV1 , fAlpha * MATH_f_NormVector(&stWorldVectU));
			MATH_ScaleVector(&stV1	 , &stU1 , fAlpha * MATH_f_NormVector(&stWorldVectV));
			MATH_SubVector	(&pp_Frst -> stWorldVectUOrtho , &stWorldVectU , &stLOCAL	); 
			MATH_SubVector	(&pp_Frst -> stWorldVectVOrtho , &stWorldVectV , &stV1		); //*/
			
			/* ? =========================== Normalize U & V axis ============================== */
			pp_Frst -> stVCoef = fOptInv(MATH_f_NormVector(&pp_Frst -> stWorldVectVOrtho));
			MATH_ScaleVector(&pp_Frst -> stWorldVectVOrtho, &pp_Frst -> stWorldVectVOrtho, pp_Frst -> stVCoef);
			pp_Frst -> stUCoef = fOptInv(MATH_f_NormVector(&pp_Frst -> stWorldVectUOrtho));
			MATH_ScaleVector(&pp_Frst -> stWorldVectUOrtho, &pp_Frst -> stWorldVectUOrtho, pp_Frst -> stUCoef);
			pp_Frst -> stZCoef = fOptInv(pp_Frst ->fZDepth);
			
			/* OK ================ Compute world Bounding volume (axe aligned) =================== */
			{
				MATH_tdst_Vector		stCurrentVrtx , stZPlus ;
				MATH_InitVector(&pp_Frst -> stGMin , Cf_Infinit , Cf_Infinit , Cf_Infinit );
				MATH_InitVector(&pp_Frst -> stGMax ,-Cf_Infinit ,-Cf_Infinit ,-Cf_Infinit );
				MATH_ScaleVector(&stZPlus , &pp_Frst -> stWorldVectShadow, pp_Frst -> fZDepth);
				ulBoxVertexCounter = 16 ;
				ulCullingResult  = 0x1f; /* 5 bits */
				while (ulBoxVertexCounter--)
				{
					stCurrentVrtx = pp_Frst -> stWorldCenter;
					if (ulBoxVertexCounter & 1)
						MATH_AddVector(&stCurrentVrtx,&stCurrentVrtx, &stWorldVectU);
					else
						MATH_SubVector(&stCurrentVrtx,&stCurrentVrtx, &stWorldVectU);
					if (ulBoxVertexCounter & 2)
						MATH_AddVector(&stCurrentVrtx,&stCurrentVrtx, &stWorldVectV);
					else
						MATH_SubVector(&stCurrentVrtx,&stCurrentVrtx, &stWorldVectV);
					if (ulBoxVertexCounter & 4)
						MATH_AddVector(&stCurrentVrtx,&stCurrentVrtx, &pp_Frst -> stWorldVectZ);
					else
						MATH_SubVector(&stCurrentVrtx,&stCurrentVrtx, &pp_Frst -> stWorldVectZ);
					if (ulBoxVertexCounter & 8)
						MATH_AddVector(&stCurrentVrtx,&stCurrentVrtx, &stZPlus);
					/* Camera culling should be done here with stCurrentVrtx */
					ulPlaneCounter = 5;
					while (ulPlaneCounter--)
					{
						Norm = MATH_f_DotProduct(&stCullingPlane[ulPlaneCounter] , &stCurrentVrtx) - fCullingDistances[ulPlaneCounter];
						if (Norm < 0.0f) 
							ulCullingResult &= ~((LONG)1 << ulPlaneCounter);
					}
					if (stCurrentVrtx.x < pp_Frst -> stGMin.x) pp_Frst -> stGMin.x = stCurrentVrtx.x;
					if (stCurrentVrtx.y < pp_Frst -> stGMin.y) pp_Frst -> stGMin.y = stCurrentVrtx.y;
					if (stCurrentVrtx.z < pp_Frst -> stGMin.z) pp_Frst -> stGMin.z = stCurrentVrtx.z;
					if (stCurrentVrtx.x > pp_Frst -> stGMax.x) pp_Frst -> stGMax.x = stCurrentVrtx.x;
					if (stCurrentVrtx.y > pp_Frst -> stGMax.y) pp_Frst -> stGMax.y = stCurrentVrtx.y;
					if (stCurrentVrtx.z > pp_Frst -> stGMax.z) pp_Frst -> stGMax.z = stCurrentVrtx.z;
				}
			}
#ifdef DBG_SHADOWS_SCREEN_CULLING_ON
			if (!ulCullingResult)
#endif
				pp_Frst ->p_FatherModifier->ulFlags &= ~MDF_SDW_IsCulled;
			
			pp_Frst++;
	}
	
#endif //#if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)	
}

/*
=======================================================================================================================
=======================================================================================================================
*/
#define TrianglesbufferSize 42 * 2

#ifdef PSX2_TARGET
void Gsp_SetUVMatrix(MAT_tdst_Decompressed_UVMatrix *TDST_ummAT);
void Gsp_SetPlanarProjectionMode(ULONG ProjMode);
void Gsp_SetPlanarProjectionUVMatrix(MATH_tdst_Vector *VU,MATH_tdst_Vector *VV,MATH_tdst_Vector *VT);
	static ULONG GGG;
void GSP_SDW_ComputeUV(GEO_Vertex *p_SourceXYZ , SOFT_tdst_UV *DestUV , ULONG *ZdistDest ,ULONG Number , MATH_tdst_Vector *VU,MATH_tdst_Vector *VV,MATH_tdst_Vector *VZ,MATH_tdst_Vector *VT)
{
	MATH_tdst_Vector *V1,*V2,*V3,*V4;
	V1 = VU;
	V2 = VV;
	V3 = VZ;
	V4 = VT;
	GGG <<= Number;
	asm  ("
		.set noreorder;
			lw		t7,V1
			lq		t7,0(t7)
			qmtc2	t7,$vf10
			lw		t7,V2
			lq		t7,0(t7)
			qmtc2	t7,$vf11
			lw		t7,V3
			lq		t7,0(t7)
			qmtc2	t7,$vf12
			lw		t7,V4
			lq		t7,0(t7)
			qmtc2	t7,$vf13
			
			srl 	a3,a3,2
			b		LoopEntry
			addiu	a3,a3,1
loop: // treat 4 points
			lq	t0,0(a0)																	// Load
			lq	t1,16(a0)
			lq	t2,32(a0)
			lq	t3,48(a0)
			pextlw     t4,t1,t0 															// Transpose
			pextuw     t5,t1,t0
			pextlw     t6,t3,t2
			pextuw     t7,t3,t2
			pcpyld     t0,t6,t4
			pcpyud     t1,t4,t6
			pcpyld     t2,t7,t5
			qmtc2	   t0,$vf01 															// Move to  v0 
			qmtc2	   t1,$vf02
			qmtc2	   t2,$vf03
			VMULAx   $ACC ,$vf01,$vf10x  				// ACC = V1 * V2  (X)         		// Compute U1 U2 U3 U4
			VMADDAy  $ACC ,$vf02,$vf10y  				// ACC += V1 * V2 (Y)
			VMADDz   $vf28,$vf03,$vf10z  			// VF30 = ACC + V1 * V2 (Z)
			VADDx	$vf28,$vf28,$vf13x				// REsult U
			VMULAx   $ACC ,$vf01,$vf11x  				// ACC = V1 * V2  (X) 				// Compute V1 V2 V3 V4
			VMADDAy  $ACC ,$vf02,$vf11y  				// ACC += V1 * V2 (Y)
			VMADDz   $vf29,$vf03,$vf11z 				// VF30 = ACC + V1 * V2 (Z)
			VADDy	$vf29,$vf29,$vf13y				// REsult V
			VMULAx   $ACC ,$vf01,$vf12x  				// ACC = V1 * V2  (X)				// Compute L1 L2 L3 L4
			VMADDAy  $ACC ,$vf02,$vf12y  				// ACC += V1 * V2 (Y)
			VMADDz   $vf30,$vf03,$vf12z  			// VF30 = ACC + V1 * V2 (Z)
			VSUBz	$vf30,$vf30,$vf13z				// REsult Z
			qmfc2.i	   t0,$vf28
			qmfc2	   t1,$vf29
			pextlw     t3,t1,t0						// t3 = u1 v1 u2  v2
			pextuw     t4,t1,t0						// t4 = u3 v3 u4  v4 
			sq t3,0(a1)
			sq t4,16(a1)
			qmfc2	   t2,$vf30
			sq t2,0(a2)
			addiu	a0,a0,64
			addiu	a1,a1,32
			addiu	a2,a2,16
LoopEntry:
			bne a3,$0,loop
			addi	a3,a3,-1
		.set reorder;
   ");//*/

};


#endif

void SDW_Display1ShadfowOn1Object(GEO_tdst_Object *pst_Obj , DD_tdst_ShadowStackNode *p_SdwNode)
{
#if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)
	GEO_tdst_IndexedTriangle				dst_Triangle		[TrianglesbufferSize];
	SOFT_tdst_UV							*DstUV , *pp_Cull[4];
 	GEO_Vertex                          	*VSrc, *VSrcLast;
	MATH_tdst_Vector						*VSrcNormales;
	GEO_Vertex								stfA ONLY_PSX2_ALIGNED(16); 
	MATH_tdst_Vector						stVZ ONLY_PSX2_ALIGNED(16); 
	MATH_tdst_Vector						stVU ONLY_PSX2_ALIGNED(16); 
	MATH_tdst_Vector						stVV ONLY_PSX2_ALIGNED(16); 
	MATH_tdst_Vector						stVS ONLY_PSX2_ALIGNED(16); 
	MATH_tdst_Vector						stStock; 
	float									LightFactor,LightFactor2;
	MATH_tdst_Matrix						stInvertedMatrix ONLY_PSX2_ALIGNED(16);
	GEO_tdst_ElementIndexedTriangles		stElement,*p_First, *p_Lst;
//	MATH_tdst_Vector						stLocalClipping;
	GEO_tdst_IndexedTriangle				*p_stTriangle , *p_stTriangleLast , *p_stTriangleDest;
	ULONG									ulCuller,ulCullerColor,*p_ulColorPtr,ulShadowColor;
#ifdef _GAMECUBE
	ULONG 									*destalpha;
#endif
	
	
	
	GDI_gpst_CurDD->pst_ComputingBuffers->Current = GDI_gpst_CurDD->pst_ComputingBuffers->ast_UV;
	GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
	
	MATH_SubVector(&stStock,&GDI_gpst_CurDD ->pst_CurrentGameObject -> pst_GlobalMatrix -> T, &p_SdwNode -> stWorldCenter); 
	stfA.x = p_SdwNode -> stUCoef * MATH_f_DotProduct(&p_SdwNode -> stWorldVectUOrtho , &stStock );
	stfA.y = p_SdwNode -> stVCoef * MATH_f_DotProduct(&p_SdwNode -> stWorldVectVOrtho , &stStock );
	stfA.z = p_SdwNode -> stZCoef * MATH_f_DotProduct(&p_SdwNode -> stWorldVectZ , &stStock );
	
	MATH_InvertMatrix(&stInvertedMatrix , GDI_gpst_CurDD ->pst_CurrentGameObject -> pst_GlobalMatrix );
	if (MATH_b_TestScaleType(&stInvertedMatrix))
	{
		stInvertedMatrix.Sx = fOptInv(stInvertedMatrix.Sx);
		stInvertedMatrix.Sy = fOptInv(stInvertedMatrix.Sy);
		stInvertedMatrix.Sz = fOptInv(stInvertedMatrix.Sz);
	}
	MATH_TransformVector(&stVU, &stInvertedMatrix , &p_SdwNode -> stWorldVectUOrtho);
	MATH_TransformVector(&stVV, &stInvertedMatrix , &p_SdwNode -> stWorldVectVOrtho);
	MATH_TransformVector(&stVZ, &stInvertedMatrix , &p_SdwNode -> stWorldVectZ);
	MATH_TransformVector(&stVS, &stInvertedMatrix , &p_SdwNode -> stWorldVectShadow);
	MATH_NormalizeVector(&stVS,&stVS);
	MATH_ScaleVector(&stVU,&stVU,p_SdwNode -> stUCoef); 
	MATH_ScaleVector(&stVV,&stVV,p_SdwNode -> stVCoef); 
	MATH_ScaleVector(&stVZ,&stVZ,-p_SdwNode -> stZCoef); 
	ulShadowColor = p_SdwNode->ulColor ^ 0xFFFFFF;
	ulShadowColor &= 0xffffff;
	ulShadowColor |= 0xfc000000;//fc
	stfA.x += 0.5f;
	stfA.y += 0.5f;
	stfA.z -= 1.0f;
	
	/* Compute UV */
	p_ulColorPtr = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
	VSrc = GDI_gpst_CurDD->p_Current_Vertex_List;
	VSrcLast = VSrc + pst_Obj->l_NbPoints;
	GEO_UseNormals(pst_Obj);
	VSrcNormales = pst_Obj->dst_PointNormal;
	DstUV = GDI_gpst_CurDD->pst_ComputingBuffers->ast_UV;
#ifdef PSX2_TARGET
	{
		ULONG SaveStatus;
		SaveStatus =GspGlobal_ACCESS(Status);
		GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_WATEREFFECT;
		if (!pst_Obj->p_CompressedNormals) return;
		p_First = pst_Obj ->dst_Element;
		p_Lst   = p_First + pst_Obj ->l_NbElements;
		while (p_First < p_Lst)
		{
			Gsp_SetPlanarProjectionUVMatrix_For_Shadows(&stVU,&stVV,&stfA,&stVZ,stfA.z + 1.0f);
			GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient = 		(ULONG)(p_SdwNode->fStartFactor * 255.0f) << 24;
			GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField = NULL;
			GspGlobal_ACCESS(ulColorOr) = 0xffffff;
			GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient |= 0xffffff;
			GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor = 0;
			GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),p_First,GDI_gpst_CurDD->p_Current_Vertex_List,NULL,pst_Obj->l_NbPoints);
			p_First++;
		}
		GspGlobal_ACCESS(Status) = SaveStatus;
	}
	return;
#elif defined(_GAMECUBE)
	{
	Mtx mtx;
	extern BOOL g_bUVGenFromPos;
		
	mtx[0][0] = stVU.x;
	mtx[0][1] = stVU.y;
	mtx[0][2] = stVU.z;
	mtx[0][3] = stfA.x;			
	mtx[1][0] = stVV.x;
	mtx[1][1] = stVV.y;
	mtx[1][2] = stVV.z;				
	mtx[1][3] = stfA.y;	
	GXLoadTexMtxImm(mtx, GX_TEXMTX0,  GX_MTX2x4);	
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX0);
	GXI_Global_ACCESS(current_object_list_item).bShadow = TRUE;
	
	g_bUVGenFromPos = TRUE;
	
	while(VSrc < VSrcLast)
	{
		DstUV->u = MATH_f_DotProduct(VCast( VSrc ),&stVU) + stfA.x;
		DstUV->v = MATH_f_DotProduct(VCast( VSrc ),&stVV) + stfA.y;
		LightFactor = MATH_f_DotProduct(VCast( VSrc ),&stVZ) - stfA.z;
		*(float *)(p_ulColorPtr++) = LightFactor;
		DstUV++;
		VSrc++;
	}
	
	p_ulColorPtr = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
	VSrc = GDI_gpst_CurDD->p_Current_Vertex_List;
	VSrcLast = VSrc + pst_Obj->l_NbPoints;
	DstUV = GDI_gpst_CurDD->pst_ComputingBuffers->ast_UV;
	}
#else
	GEO_UseNormals(pst_Obj);
	while(VSrc < VSrcLast)
	{
		DstUV->u = MATH_f_DotProduct(VCast( VSrc ),&stVU) + stfA.x;
		DstUV->v = MATH_f_DotProduct(VCast( VSrc ),&stVV) + stfA.y;
		LightFactor = MATH_f_DotProduct(VCast( VSrc ),&stVZ) - stfA.z;
		*(float *)(p_ulColorPtr++) = LightFactor * 0.5f;//0.9f
		DstUV++;
		VSrc++;
	} 
	p_ulColorPtr = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
	VSrc = GDI_gpst_CurDD->p_Current_Vertex_List;
	VSrcLast = VSrc + pst_Obj->l_NbPoints;
	DstUV = GDI_gpst_CurDD->pst_ComputingBuffers->ast_UV;
#endif	
//	*p_ulColorPtr = SDW_ul_Interpol2Colors(ulShadowColor, *(float *)p_ulColorPtr);	
	/* Compute final colors */
	if (p_SdwNode->p_FatherModifier->ulFlags & MDF_SDW_UseNormales)
	{
			if (!(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric)) stVS.x = -stVS.x;

			MATH_NegEqualVector(&stVS);
			VSrc = GDI_gpst_CurDD->p_Current_Vertex_List;
			VSrcNormales = pst_Obj->dst_PointNormal;
		
			while(VSrc < VSrcLast)
			{
				LightFactor2 = MATH_f_DotProduct(VSrcNormales,&stVS); 
				if (LightFactor2 > 0.0f)
				{ 
					//static DWORD dwColor = 0xff000000;
					/*float fCoef = 1.0f-*(float *)p_ulColorPtr;
					/*fCoef *= fCoef;
					fCoef = 1.0f-fCoef;
//					*p_ulColorPtr++ = ((ULONG)(255.f/ (fCoef*fCoef))<<24);
					*p_ulColorPtr++ = dwColor;*/
					
					LightFactor = *(float *)p_ulColorPtr;
					
						if (LightFactor >=1.0f )  // > 1.0f | < 0.0f 
						*(float *)(p_ulColorPtr++) = LightFactor;
					else
					{
							LightFactor2 = 1.0f - LightFactor2;
						LightFactor2 *= LightFactor2;
						LightFactor2 = 1.0f - LightFactor2;
						*(float *)(p_ulColorPtr++) = LightFactor * LightFactor2;
						//*(float *)(p_ulColorPtr++) = 0xffffffff;
					}
				}
				else
				{
					*(p_ulColorPtr++) = 0;
				}
				VSrc++;
				VSrcNormales++;
			} 
	}//*/
	
	/* Compute final colors */
	{
		ULONG *p_ulColorPtrLast;
		p_ulColorPtr 		= GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
		p_ulColorPtrLast 	= p_ulColorPtr + pst_Obj->l_NbPoints;

		while (p_ulColorPtr < p_ulColorPtrLast)
		{
			if (*p_ulColorPtr)
				 *(p_ulColorPtr++) = SDW_ul_Interpol2Colors(ulShadowColor, *(float *)p_ulColorPtr);
			 else 
			 	p_ulColorPtr++;
		}
	}
	/* Then for each element Cull & draw shadow */
	
	
	VSrc = GDI_gpst_CurDD->p_Current_Vertex_List;
	DstUV = GDI_gpst_CurDD->pst_ComputingBuffers->ast_UV;
	p_ulColorPtr = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
	VSrcNormales = pst_Obj->dst_PointNormal;
	
	p_First = pst_Obj ->dst_Element;
	p_Lst   = p_First + pst_Obj ->l_NbElements;
	stElement .pus_ListOfUsedIndex = NULL;
	
	stElement .ul_NumberOfUsedIndex = 0;
	stElement .p_MrmElementAdditionalInfo = NULL;
	stElement .pst_StripData = NULL;
#ifdef PSX2_TARGET	
		stElement .p_ElementCache = NULL;
#endif
	stElement .l_NbTriangles = 0;
	p_stTriangle = stElement .dst_Triangle = dst_Triangle;
	p_stTriangleDest = stElement.dst_Triangle;
	
	stElement.pst_Gx8Add = NULL;
	Gx8_SetUVStageOff();

	stfA.z += 1.0f;
	MATH_NegVector(&stVZ,&stVZ);
	stfA.z = -stfA.z;
#ifdef _GAMECUBE
	stElement.dl = (void*)0;
	stElement.dl_pos = (void*)0;
	
//	GX_GXSetAlphaCompare(GX_GREATER, 0x00, GX_AOP_OR, GX_NEVER, 0);
//	GX_GXSetZCompLoc(GX_FALSE);
	GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentAlphaField = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ComputedAlpha;
	destalpha = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ComputedAlpha;
#endif
	
	while (p_First < p_Lst)
	{
		p_stTriangle = p_First ->dst_Triangle;
		p_stTriangleLast = p_stTriangle + p_First ->l_NbTriangles;
		while (p_stTriangle < p_stTriangleLast)
		{
			/* CullingTest */
#define CAROTAX(a) (a | (a>>9) | (a>>18))
			ulCullerColor	=	GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[p_stTriangle->auw_Index[0]] |
				GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[p_stTriangle->auw_Index[1]] |
				GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[p_stTriangle->auw_Index[2]];
			if (!(ulCullerColor & 0x01000000)) goto LABEL_Culled; // back
//			if (!(ulCullerColor & 0x00ffffff)) goto LABEL_Culled; // black//
			pp_Cull[0] = DstUV + p_stTriangle->auw_Index[0];
			pp_Cull[1] = DstUV + p_stTriangle->auw_Index[1];
			pp_Cull[2] = DstUV + p_stTriangle->auw_Index[2];
			ulCuller =  (((*(ULONG *)&pp_Cull[0]->u) & 0xFF800000) | (((*(ULONG *)&pp_Cull[1]->u) & 0xFF800000) >> 9) | (((*(ULONG *)&pp_Cull[2]->u) ) >> 18));
			if ((ulCuller & CAROTAX(0x80000000)) == CAROTAX(0x80000000)) goto LABEL_Culled; // u < 0.0f
			ulCuller +=  CAROTAX(0x00800000);
			if ((ulCuller & CAROTAX(0xC0000000)) == CAROTAX(0x40000000)) goto LABEL_Culled; // u > 1.0f
			ulCuller =  (((*(ULONG *)&pp_Cull[0]->v) & 0xFF800000) | (((*(ULONG *)&pp_Cull[1]->v) & 0xFF800000) >> 9) | (((*(ULONG *)&pp_Cull[2]->v) ) >> 18));
			if ((ulCuller & CAROTAX(0x80000000)) == CAROTAX(0x80000000)) goto LABEL_Culled; // v < 0.0f
			ulCuller +=  CAROTAX(0x00800000);
			if ((ulCuller & CAROTAX(0xC0000000)) == CAROTAX(0x40000000)) goto LABEL_Culled; // v > 1.0f

#ifndef _GAMECUBE
			p_stTriangleDest ->auw_Index[0] = p_stTriangle->auw_Index[0] ;
			p_stTriangleDest ->auw_Index[1] = p_stTriangle->auw_Index[1] ;
			p_stTriangleDest ->auw_Index[2] = p_stTriangle->auw_Index[2] ;
			p_stTriangleDest ++;
			stElement.l_NbTriangles ++;

			if (stElement.l_NbTriangles == TrianglesbufferSize)
			{
#ifdef PSX2_TARGET
				Gsp_SetUVMatrix_Identity();
				Gsp_SetPlanarProjectionUVMatrix(&stVU,&stVV,&stfA);
#endif
#ifdef PSX2_TARGET
				GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),&stElement,VSrc,NULL,pst_Obj->l_NbPoints);
#else				
				GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),&stElement,VSrc,NULL,(GEO_tdst_UV *) GDI_gpst_CurDD->pst_ComputingBuffers->Current,pst_Obj->l_NbPoints);
#endif		
				stElement.l_NbTriangles = 0;

				p_stTriangleDest = stElement.dst_Triangle;
			}
#endif			
			goto LABEL_NotCulled;
LABEL_Culled:
#ifdef _GAMECUBE
			destalpha[p_stTriangle->auw_Index[0]] = 0x00000000;
			destalpha[p_stTriangle->auw_Index[1]] = 0x00000000;
			destalpha[p_stTriangle->auw_Index[2]] = 0x00000000;
#elif _XBOX
/*			GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[p_stTriangle->auw_Index[0]] &= 0x00ffffff;
			GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[p_stTriangle->auw_Index[1]] &= 0x00ffffff;
			GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[p_stTriangle->auw_Index[2]] &= 0x00ffffff;//*/
#endif
			goto LABEL_EndCulled;

LABEL_NotCulled:
#ifdef _GAMECUBE
			destalpha[p_stTriangle->auw_Index[0]] = 0xff000000;
			destalpha[p_stTriangle->auw_Index[1]] = 0xff000000;
			destalpha[p_stTriangle->auw_Index[2]] = 0xff000000;
#elif _XBOX
/*			GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[p_stTriangle->auw_Index[0]] |= 0xff000000;
			GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[p_stTriangle->auw_Index[1]] |= 0xff000000;
			GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[p_stTriangle->auw_Index[2]] |= 0xff000000;//*/
#endif
LABEL_EndCulled:
			p_stTriangle++;
		}
		
#ifdef _GAMECUBE		
		GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),p_First,GDI_gpst_CurDD->p_Current_Vertex_List,NULL,pst_Obj->l_NbPoints);
#endif		
		
		p_First++;
	}

#ifndef _GAMECUBE
	if (stElement.l_NbTriangles)
	{
	
#ifdef PSX2_TARGET
		Gsp_SetUVMatrix_Identity();
		Gsp_SetPlanarProjectionUVMatrix(&stVU,&stVV,&stfA);					
//				Gsp_SetPlanarProjectionMode(0);
#endif
#ifdef PSX2_TARGET
				GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),&stElement,VSrc,NULL,pst_Obj->l_NbPoints);
#else				
				GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),&stElement,VSrc, NULL,(GEO_tdst_UV *) GDI_gpst_CurDD->pst_ComputingBuffers->Current,pst_Obj->l_NbPoints);
//				GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),&stElement,VSrc,NULL,pst_Obj->l_NbPoints);
#endif		
	}
#endif

#ifdef _GAMECUBE
	GXI_Global_ACCESS(current_object_list_item).bShadow = FALSE;
#endif

#endif	// #if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)
}

/*
=======================================================================================================================
=======================================================================================================================
*/

//#ifdef PSX2_TARGET
/*extern void GSP_SetTextureBlending(ULONG _l_Texture, ULONG BM);
extern void GSP_SetTextureTarget(ULONG _l_Texture);*/
//extern void GSP_SetViewMatrix_SDW(MATH_tdst_Matrix *_pst_Matrix , float *Limits);
extern MATH_tdst_Matrix MATH_gst_IdentityMatrix;
static ULONG UsedShadowTexture = 0;
void SDW_RenderShadow(GDI_tdst_DisplayData *pst_DD, GEO_tdst_Object *pst_Obj , DD_tdst_ShadowStackNode *p_SdwNode)
{
#if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)
	GEO_tdst_ElementIndexedTriangles				*pst_Elem,*pst_LastElem;
	ULONG 								ulFlags , ulMustBeCleared;
	MATH_tdst_Vector						stfA,stVZ, stVU, stVV  ,stStock;
	MATH_tdst_Matrix						stInvertedMatrix ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Matrix						stMatrix;
	
	/* Save current Matrix 		*/
	L_memset(&stInvertedMatrix , 0 , sizeof(MATH_tdst_Matrix));
	L_memset(&stMatrix , 0 , sizeof(MATH_tdst_Matrix));

	ulMustBeCleared = p_SdwNode->RDR_WindowAffectation & 0x80000000;
	
	p_SdwNode->RDR_WindowAffectation &= ~0x80000000;

	//check if there's space for another texture
	if(p_SdwNode->RDR_WindowAffectation >= MaxShadowTexture-1)
		return;

	UsedShadowTexture |= 1<<p_SdwNode->RDR_WindowAffectation;
	
	/* BEGIN Compute & Set shadow Matrix ------------------------------------------------------------------------------	*/
	MATH_SubVector(&stStock,&pst_DD ->pst_CurrentGameObject -> pst_GlobalMatrix -> T, &p_SdwNode -> stWorldCenter); 
	stfA.x = 2.0F * p_SdwNode -> stUCoef * MATH_f_DotProduct(&p_SdwNode -> stWorldVectUOrtho , &stStock );
	stfA.y = -2.0F * p_SdwNode -> stVCoef * MATH_f_DotProduct(&p_SdwNode -> stWorldVectVOrtho , &stStock );
	stfA.z = 0.0f;

	MATH_InvertMatrix(&stInvertedMatrix , pst_DD ->pst_CurrentGameObject -> pst_GlobalMatrix );
	
	MATH_Transp33MatrixWithoutBuffer(&stInvertedMatrix, pst_DD ->pst_CurrentGameObject -> pst_GlobalMatrix)	;
	if (MATH_b_TestScaleType(&stInvertedMatrix))
	{
		stInvertedMatrix.Sx = 1.0f / (stInvertedMatrix.Sx);
		stInvertedMatrix.Sy = 1.0f / (stInvertedMatrix.Sy);
		stInvertedMatrix.Sz = 1.0f / (stInvertedMatrix.Sz);
	} else
	{
		stInvertedMatrix.Sx = 1.0f;
		stInvertedMatrix.Sy = 1.0f;
		stInvertedMatrix.Sz = 1.0f;
	}	
		
	MATH_TransformVector(&stVU, &stInvertedMatrix , &p_SdwNode -> stWorldVectUOrtho);
	MATH_TransformVector(&stVV, &stInvertedMatrix , &p_SdwNode -> stWorldVectVOrtho);
	//MATH_TransformVector(&stVZ, &stInvertedMatrix , &p_SdwNode->stWorldVectZ);
	MATH_InitVectorToZero(&stVZ);
	
	MATH_ScaleVector(&stVU,&stVU,p_SdwNode -> stUCoef * 2.0F); 
	MATH_ScaleVector(&stVV,&stVV,-p_SdwNode -> stVCoef * 2.0F); 
	*(MATH_tdst_Vector *)&stMatrix.Ix = stVU;
	*(MATH_tdst_Vector *)&stMatrix.Jx = stVV;
	*(MATH_tdst_Vector *)&stMatrix.Kx = stVZ;
	MATH_TranspEq33Matrix(&stMatrix);
	stMatrix.T = stfA; 
	stMatrix.Sx = stMatrix.Sy = stMatrix.Sz = 0.0f;
	stMatrix.w = 1.0f;// MATRIX W!
	MATH_MakeOGLMatrix(&stMatrix, &stMatrix);

	Gx8_SetViewMatrix_SDW(&stMatrix);
	/* END Compute & Set shadow Matrix ---------------------------------------------------------------------------------- */

	ulFlags = /*MAT_Cul_Flag_HideAlpha |*/ MAT_Cul_Flag_HideColor | MAT_Cul_Flag_NoZWrite;
	MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Disable);
	MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);

	pst_DD->pst_CurrentGameObject = p_SdwNode->p_FatherModifier->pst_GO;
	pst_DD->pst_ComputingBuffers->CurrentColorField = NULL;
	GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentAlphaField = NULL;
	pst_DD->pst_ComputingBuffers->ulColorXor = 0;
	pst_DD->pst_ComputingBuffers->ul_Ambient = 0xffffffff;
	pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_Fogged;

	GDI_SetTextureBlending((*pst_DD),0xffffffff, ulFlags, 0);


	pst_DD->st_GDI.pfnv_SetTextureTarget(p_SdwNode->RDR_WindowAffectation , ulMustBeCleared);
	/* Draw shadow 			*/

	{
/*		ULONG ulBlendingMode = 0;
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
		MAT_SET_ColorOp(ulFlags,MAT_Cc_ColorOp_Diffuse);
		Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode,0);
*/
/*		MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
		MAT_SET_ColorOp(ulFlags,MAT_Cc_ColorOp_Diffuse);
		Gx8_SetTextureBlending((ULONG) - 1, ulFlags, 0);
*/
	}
	//Gx8_vTest();

	/*TiZ: begin shadow dot smooth */
	{
/*		LONG	*pAlphaColor;
		MATH_tdst_Vector *pNormal, *pLastNormal, stVS;
		MATH_TransformVector(&stVS, &stInvertedMatrix , &p_SdwNode -> stWorldVectShadow);
		MATH_NormalizeVector(&stVS,&stVS);


		pAlphaColor = pst_DD->pst_ComputingBuffers->ComputedAlpha;
		pNormal = pst_Obj->dst_PointNormal;
		pLastNormal = pNormal + pst_Obj->l_NbPoints;
		while(pNormal < pLastNormal)
		{
			*pAlphaColor = (int)(MATH_f_DotProduct(pNormal,&stVS) * 255.f)<<24; 
			pAlphaColor++;
			pNormal++;
		}
		GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentAlphaField = pst_DD->pst_ComputingBuffers->ComputedAlpha;
		pst_DD->pst_ComputingBuffers->CurrentColorField = pst_DD->pst_ComputingBuffers->ComputedAlpha;
		*/
	}
	/*TiZ: end

	/* BEGIN Render shadow */
	pst_Elem = pst_Obj->dst_Element;
	pst_LastElem = pst_Elem + pst_Obj->l_NbElements;
	while (pst_Elem < pst_LastElem)
	{
/*	GRO_tdst_Visu						*pst_Visu;
	pst_Visu = p_SdwNode -> p_FatherModifier->pst_GO->pst_Base->pst_Visu;
	    MAT_DrawIndexedTriangle(pst_Obj, (MAT_tdst_Material *) pst_Visu->pst_Material, pst_Elem);*/

		GDI_DrawIndexedTriangles((*pst_DD),pst_Elem,pst_DD->p_Current_Vertex_List,NULL ,NULL, pst_Obj->l_NbPoints);
		//Gx8_l_DrawElementIndexedTrianglesForShadow(pst_Elem,pst_DD->p_Current_Vertex_List,NULL , pst_Obj->l_NbPoints);
		pst_Elem++;
	}
	/* END Render shadow */

	/* Restore frame buffer target 	*/
	pst_DD->st_GDI.pfnv_SetTextureTarget(0xffffffff , 0);
	pst_DD->pst_ComputingBuffers->ul_Ambient = 0;
	pst_DD->pst_ComputingBuffers->CurrentColorField = pst_DD->pst_ComputingBuffers->ComputedColors;
	/* Restore Matrix*/	
	GDI_SetViewMatrix((*pst_DD) , pst_DD->st_Camera.pst_ObjectToCameraMatrix);

	Gx8_SetProjectionMatrix(&pst_DD->st_Camera);

	Gx8_RS_Lighting(pst_DD->pv_SpecificData, 0);

	GDI_SetTextureBlending((*pst_DD),0xffffffff, 0, 0);
#endif	// #if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)
}
//#endif 
/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG SDW_GET_SF(OBJ_tdst_GameObject *p_stSon , OBJ_tdst_GameObject *p_stSsdw)
{
	if ((OBJ_ul_FlagsIdentityGet(p_stSon) & OBJ_C_IdentityFlag_Hierarchy) && p_stSon->pst_Base && p_stSon->pst_Base->pst_Hierarchy && (p_stSon->pst_Base->pst_Hierarchy->pst_Father))
	{
		if (p_stSsdw == p_stSon) 	
			return 1;
		else
			return SDW_GET_SF(p_stSon->pst_Base->pst_Hierarchy->pst_Father,p_stSsdw);
	}
	else
	{
		if (p_stSon == p_stSsdw)
			return 1;
		else
			return 0;
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void SDW_DisplayShadowsOnCurrentObject(GEO_tdst_Object *pst_Obj)
{
#if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)
	DD_tdst_ShadowStack *p_ShadowStack;
 	DD_tdst_ShadowStackNode *p_SdwNode;
	ULONG ulFlags , ulShadowCounter;

#ifdef _XBOX
	static bool NoShadows = false;

	if(NoShadows)
		return;
#endif

	p_ShadowStack = (DD_tdst_ShadowStack *) GDI_gpst_CurDD->pv_ShadowStack;
	if (!p_ShadowStack) return;
	if (!p_ShadowStack->ulNumberOfNodes) return;
	if (!(GDI_gpst_CurDD ->ul_CurrentDrawMask & GDI_Cul_DM_ReceiveDynSdw)) return;
	if (!GDI_gpst_CurDD->pst_CurrentGameObject) return;

#ifdef Gx8_BENCH
	if (g_iNoSDW) return;
#endif 


	_GSP_BeginRaster(3);

	
	
	ulShadowCounter = p_ShadowStack->ulLastNode;
	p_SdwNode = p_ShadowStack->pp_Nodes;
	while (ulShadowCounter--)
	{
	ulFlags = MAT_Cul_Flag_Bilinear ;
	ulFlags |= MAT_Cul_Flag_NoZWrite;
	//	ulFlags |= MAT_Cul_Flag_AlphaTest;
	//	ulFlags |= MAT_Cul_Flag_InvertAlpha;
	//ulFlags &= GDI_Cul_DM_Fogged;
	
#ifndef DBG_SHADOWS_UV_TILLING_OFF
	ulFlags |= MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV;
#endif
	
		/* Bounding volume culling is done here */
		if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_IsCulled) goto NEXT_SHADOW;
		if (SDW_GET_SF(GDI_gpst_CurDD->pst_CurrentGameObject,p_SdwNode -> p_FatherModifier->pst_GO))
		{
			/* Rendering shadow */
			if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_1RealTime) 
			{
				// turn all lights off, but keep D3D lighting on
				LIGHT_TurnOffAllLights();

				// change material
				Gx8_vChangeMaterialForShadow(p_SdwNode->fStartFactor);
				SDW_RenderShadow(GDI_gpst_CurDD, pst_Obj , p_SdwNode);
			}

			if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_EliminateSameFather)
			{
				goto NEXT_SHADOW;
			}
		}
		if (GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency) goto NEXT_SHADOW;

		if (!(p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_ApplyOnDynam))
		{
			if (OBJ_ul_FlagsIdentityGet(GDI_gpst_CurDD->pst_CurrentGameObject) & (OBJ_C_IdentityFlag_Dyna|OBJ_C_IdentityFlag_Bone))
				goto NEXT_SHADOW;
		}//*/
		if (!(p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_ApplyOnNotDynam))
		{
			if (!(OBJ_ul_FlagsIdentityGet(GDI_gpst_CurDD->pst_CurrentGameObject) & (OBJ_C_IdentityFlag_Dyna|OBJ_C_IdentityFlag_Bone)))
				goto NEXT_SHADOW;
		}

#ifdef DBG_SHADOWS_INTERSETCION_CULLING_ON
		{
			MATH_tdst_Vector *p_bMin,*p_bMax;
			p_bMin = OBJ_pst_BV_GetGMin(GDI_gpst_CurDD->pst_CurrentGameObject->pst_BV);
			p_bMax = OBJ_pst_BV_GetGMax(GDI_gpst_CurDD->pst_CurrentGameObject->pst_BV);
			if ( p_SdwNode -> stGMin.x > p_bMax -> x + GDI_gpst_CurDD -> pst_CurrentGameObject -> pst_GlobalMatrix -> T . x ) goto NEXT_SHADOW;
			if ( p_SdwNode -> stGMin.y > p_bMax -> y + GDI_gpst_CurDD -> pst_CurrentGameObject -> pst_GlobalMatrix -> T . y ) goto NEXT_SHADOW;
			if ( p_SdwNode -> stGMax.x < p_bMin -> x + GDI_gpst_CurDD -> pst_CurrentGameObject -> pst_GlobalMatrix -> T . x ) goto NEXT_SHADOW;
			if ( p_SdwNode -> stGMax.y < p_bMin -> y + GDI_gpst_CurDD -> pst_CurrentGameObject -> pst_GlobalMatrix -> T . y ) goto NEXT_SHADOW;
			if ( p_SdwNode -> stGMax.z < p_bMin -> z + GDI_gpst_CurDD -> pst_CurrentGameObject -> pst_GlobalMatrix -> T . z ) goto NEXT_SHADOW;
			if ( p_SdwNode -> stGMin.z > p_bMax -> z + GDI_gpst_CurDD -> pst_CurrentGameObject -> pst_GlobalMatrix -> T . z ) goto NEXT_SHADOW;
		}
#endif
		if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_1RealTime)
		{

			//If we don't have space for another shadow... don't draw it
			if(p_SdwNode->RDR_WindowAffectation>=MaxShadowTexture)
				goto NEXT_SHADOW;

			MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
			MAT_SET_Blending(ulFlags, MAT_Cc_Op_Add);
			MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Diffuse);
			GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient = 0;
			GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;

/*			ULONG X,Y;*/
			MAT_SET_Blending(ulFlags, MAT_Cc_Op_PSX2ShadowSpecific);

			/* 
				Texture is defined as follow:
				Size X Po2 = (_l_TSDW >> 26) & 0xf;
				Size Y Po2 = (_l_TSDW >> 22) & 0xf;
				Buffer X = (_l_TSDW >> 10) & 0x3ff;
				Buffer Y = (_l_TSDW >> 0) & 0x3ff; 
			*/

			GDI_SetTextureBlending((*GDI_gpst_CurDD),0x80000000 | p_SdwNode->RDR_WindowAffectation, ulFlags, 0);
			IDirect3DDevice8_SetRenderState(GDI_gpst_CurDD, D3DRS_FOGENABLE, 1/*TRUE*/ );
		} else
		{

			if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_IsAdditional)
			{
				MAT_SET_Blending(ulFlags, MAT_Cc_Op_Add);
				p_SdwNode -> ulColor ^= 0xffffff;
			} else
			{
				MAT_SET_Blending(ulFlags, MAT_Cc_Op_Sub);
			}
			GDI_SetTextureBlending((*GDI_gpst_CurDD),p_SdwNode->p_FatherModifier->TextureIndex , ulFlags, 0);
			IDirect3DDevice8_SetRenderState(GDI_gpst_CurDD, D3DRS_FOGENABLE, 1/*TRUE*/ );
			
			
			
		}

		LIGHT_SetLighting(FALSE);

		//TiZ: Smooth shadow. Too "blocky" for my eyes
		if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_1RealTime)
			SDW_SmoothShadow(&UsedShadowTexture);

		SDW_Display1ShadfowOn1Object(pst_Obj , p_SdwNode );

		if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_IsAdditional)
		{
			p_SdwNode -> ulColor ^= 0xffffff;
		} 

		GDI_SetTextureBlending((*GDI_gpst_CurDD), -1, 0, 0);

		// put back original material
		Gx8_vSetNormalMaterial();

NEXT_SHADOW:
		p_SdwNode++;
	}

	// put back original material
	Gx8_vSetNormalMaterial();

	_GSP_EndRaster(3);
	
#endif	//#if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)
}

extern Gx8_tdst_SpecificData   *p_gGx8SpecificData;
extern void Gx8_SmoothTexture(D3DTexture *pSourceTexture, D3DTexture *pDestTexture);
void SDW_SmoothShadow(ULONG *pUsedShadows)
{
	static bool	EnableSmoothShadow = 1;
	int FreeTexture = 0;
	int i=0, j=0;

	if((!(*pUsedShadows)) || EnableSmoothShadow==0)
		return;

	while(!((*pUsedShadows)&(1<<j)) && (j<MaxShadowTexture-1))
	{
		j++;
}

	i = MaxShadowTexture-1;

/*	i=j;
	while(!((*pUsedShadows)&(1<<i)) && i<MaxShadowTexture)
	{
		i++;
	}
*/
	(*pUsedShadows) &=~(1<<j);


	if(j==i)
		return;	//No space available... can't smooth

	Gx8_SmoothTexture(p_gGx8SpecificData->dul_SDW_Texture[j], p_gGx8SpecificData->dul_SDW_Texture[i]);
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif


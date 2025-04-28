/*$T MDFmodifier_SDW.c GC! */


/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


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

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"

#include "BASe/BENch/BENch.h"

#define DBG_SHADOWS_SCREEN_CULLING_ON
#define DBG_SHADOWS_INTERSETCION_CULLING_ON
#define DBG_SHADOWS_UV_CULLING_ON
#define DBG_SHADOWS_UV_TILLING_OFF
#define DBG_SHADOWS_SHOWCLIPPED

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
 		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
 		/*	*((float *) &ulAlpha) = ((DEF_fCoefAlpha * fZClipLocalCoef) + 0.5f) + 32768.0f + 16384.0f;
		ulAlpha <<= 24;*/
		
		if(fZClipLocalCoef >= 1.0f) return 0x02000000 | ulP2;
		if(fZClipLocalCoef >= 0.98f) return ulP2 | 0x01000000;
		if(fZClipLocalCoef <= 0.02f) return 0x01000000;
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
	
	DD_tdst_ShadowStack *SDW_Create()
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
	
	void SDW_Destroy()
	{
		DD_tdst_ShadowStack *p_ShadowStack;
		if(!GDI_gpst_CurDD) return;

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
	void SDW_DispatchTextures()
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
		if(!GDI_gpst_CurDD) return;
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
			XCoef = pp_Frst -> p_FatherModifier -> XSizeFactor;
			YCoef = pp_Frst -> p_FatherModifier -> YSizeFactor;
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
	ulShadowColor |= 0xfC000000;
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
		if (!pst_Obj->p_CompressedNormals) return;
		p_First = pst_Obj ->dst_Element;
		p_Lst   = p_First + pst_Obj ->l_NbElements;
		while (p_First < p_Lst)
		{
			Gsp_SetPlanarProjectionUVMatrix_For_Shadows(&stVU,&stVV,&stfA,&stVZ,stfA.z + 1.0f);
			GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient = 		(ULONG)(p_SdwNode->fStartFactor * 64.0f) << 24;
			GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField = NULL;
			GspGlobal_ACCESS(ulColorOr) = 0xffffff;
			GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient = 0x60404040;
			GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor = 0;
			GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),p_First,GDI_gpst_CurDD->p_Current_Vertex_List, NULL,NULL,pst_Obj->l_NbPoints);
			p_First++;
		}
	}
	return;
#elif defined(_GAMECUBE)
	{
		Mtx mtx;
		GXColor Color;
		static u32 bFirst = 1;
		extern BOOL g_bUVGenFromPos;
		MATH_tdst_Vector						stVC;


extern BOOL g_bUVGenFromNrm;
extern BOOL g_bLightOff;

		g_bLightOff = TRUE;
		g_bUVGenFromPos = FALSE;	
		g_bUVGenFromNrm = FALSE;
//		GXSetVerifyLevel(GX_WARN_SEVERE);
		if (bFirst)
		{
			GXInitTexObj(&g_SDWClipTextures, g_SDWClipTexturesMap, 4, 4, GX_TF_IA8, GX_CLAMP , GX_CLAMP,GX_FALSE);
//		    GXInitTexObjLOD(&g_SDWClipTextures,GX_NEAR_MIP_NEAR, GX_NEAR,0.0F, 0.0f,0.0F,GX_FALSE,GX_FALSE,GX_ANISO_1 );
		    GXInitTexObjLOD(&g_SDWClipTextures,GX_LINEAR, GX_LINEAR,0.0F, 0.0f,0.0F,GX_FALSE,GX_FALSE,GX_ANISO_1 );
		    GXInitTexObjUserData(&g_SDWClipTextures, (void*)0);
		    
		    bFirst = 4;
		    while (bFirst--) g_SDWClipTexturesMap[bFirst] = 0x0000;
		    bFirst = 12;
		    while (bFirst--) g_SDWClipTexturesMap[bFirst+4] = 0xffff;
			bFirst = 0;
		}
		GX_GXSetNumTevStages(2);
		GX_GXSetNumTexGens(2);
		GX_GXSetNumChans(0);     
		GXInvalidateTexAll(); 
		MATH_ScaleEqualVector(&stVZ , -1.f);
			GX_GXLoadTexObj(&g_SDWClipTextures, GX_TEXMAP2);
/*		{
			extern GXTexObj g_ShadowTextures[];
			GX_GXLoadTexObj(&g_ShadowTextures[0], GX_TEXMAP0);
			GX_GXLoadTexObj(&g_ShadowTextures[1], GX_TEXMAP2);
		}*/

		mtx[0][0] = stVU.x;
		mtx[0][1] = stVU.y;
		mtx[0][2] = stVU.z;
		mtx[0][3] = stfA.x;			
		mtx[1][0] = stVV.x;
		mtx[1][1] = stVV.y;
		mtx[1][2] = stVV.z;				
		mtx[1][3] = stfA.y;	
		GXLoadTexMtxImm(mtx, GX_TEXMTX0,  GX_MTX2x4);
		
		MATH_InvertMatrix(&stInvertedMatrix , GDI_gpst_CurDD ->pst_CurrentGameObject -> pst_GlobalMatrix );
		MATH_NormalizeVector(&stVZ , &p_SdwNode -> stWorldVectZ);
		MATH_AddScaleVector(&stVC , &p_SdwNode -> stWorldCenter ,  &stVZ , -1.0f - p_SdwNode -> p_FatherModifier -> ZAttenuationFactor * 0.25f);
		MATH_AddScaleVector(&stVU , &stVC ,  &stVZ , -p_SdwNode -> p_FatherModifier -> ZAttenuationFactor);
		MATH_AddScaleVector(&stVV , &stVC ,  &stVZ , 0.0f);
		MATH_TransformVertex(&stVU, &stInvertedMatrix , &stVU);
		MATH_TransformVertex(&stVV, &stInvertedMatrix , &stVV);
		MATH_SubVector(&stVZ , &stVV , &stVU);
		LightFactor = fOptInv(MATH_f_NormVector(&stVZ));
		MATH_ScaleEqualVector(&stVZ , LightFactor * LightFactor);//*/
		stfA.z = -MATH_f_DotProduct(&stVZ , &stVV);
		
		mtx[1][0] = stVZ.x;
		mtx[1][1] = stVZ.y;
		mtx[1][2] = stVZ.z;				
		mtx[1][3] = stfA.z;	
		GXLoadTexMtxImm(mtx, GX_TEXMTX1,  GX_MTX2x4);
		GXI_Global_ACCESS(current_object_list_item).bShadow = TRUE;
		g_bUVGenFromPos = TRUE;

#ifdef JADEFUSION
		Color.r = (u32)(p_SdwNode->fStartFactor * 100.0f) & 0xff;
		Color.g = (u32)(p_SdwNode->fStartFactor * 100.0f) & 0xff;
		Color.b = (u32)(p_SdwNode->fStartFactor * 100.0f) & 0xff;
#else
		Color.r = (u32)(p_SdwNode->fStartFactor * 130.0f) & 0xff;
		Color.g = (u32)(p_SdwNode->fStartFactor * 130.0f) & 0xff;
		Color.b = (u32)(p_SdwNode->fStartFactor * 130.0f) & 0xff;
#endif
		GXSetTevKColor( GX_KCOLOR0,  Color );
		GXSetTevKColorSel(GX_TEVSTAGE0,GX_TEV_KCSEL_K0);
		
		GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX0,0, GX_PTIDENTITY);
		GX_GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
		GX_GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX1,0, GX_PTIDENTITY);
		GXSetTevColorOp(GX_TEVSTAGE0,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_ENABLE,GX_TEVPREV); 
		GXSetTevColorIn(GX_TEVSTAGE0,GX_CC_ZERO,GX_CC_KONST,GX_CC_TEXC  ,GX_CC_ZERO);
		GXSetTevAlphaOp(GX_TEVSTAGE0,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_ENABLE, GX_TEVPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE0,GX_CA_ZERO,GX_CA_ZERO,GX_CA_ZERO   ,GX_CA_ZERO);//*/
		GXSetTevColorOp(GX_TEVSTAGE1,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_ENABLE,GX_TEVPREV); 
		GXSetTevColorIn(GX_TEVSTAGE1,GX_CC_ZERO,GX_CC_CPREV,GX_CC_TEXC,GX_CC_ZERO);
		GXSetTevAlphaOp(GX_TEVSTAGE1,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_ENABLE, GX_TEVPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE1,GX_CA_ZERO,GX_CA_ZERO,GX_CA_ZERO   ,GX_CA_TEXA);//*/
		GX_GXSetTevOpDirty();

/*		GXSetTevColorOp(GX_TEVSTAGE1,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_ENABLE,GX_TEVPREV); 
		GXSetTevColorIn(GX_TEVSTAGE1,GX_CC_ZERO,GX_CC_KONST,GX_CC_TEXC  ,GX_CC_ZERO);
		GXSetTevAlphaOp(GX_TEVSTAGE1,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_ENABLE, GX_TEVPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE1,GX_CA_ZERO,GX_CA_ZERO,GX_CA_ZERO   ,GX_CA_RASA);//*/
		
		
		g_ShadowPass = 1;
		{
			p_First = pst_Obj ->dst_Element;
			p_Lst   = p_First + pst_Obj ->l_NbElements;
			while (p_First < p_Lst)
			{
				if (p_First->dl)
					GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),p_First,GDI_gpst_CurDD->p_Current_Vertex_List,NULL,NULL,pst_Obj->l_NbPoints);
				p_First++;
			}
		}
//    GXSetVerifyLevel(GX_WARN_NONE);
		
		g_ShadowPass = 0;//*/
		GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE );
		GX_GXSetTevOp(GX_TEVSTAGE1, GX_MODULATE );
		GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE );
		GX_GXSetTevOp(GX_TEVSTAGE1, GX_MODULATE );
		GX_GXSetNumTevStages(1);
		GX_GXSetNumTexGens(1);
		
		GXI_Global_ACCESS(current_object_list_item).bShadow = FALSE;
		return;
	}
#else
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
#endif	
//	*p_ulColorPtr = SDW_ul_Interpol2Colors(ulShadowColor, *(float *)p_ulColorPtr);	
	/* Compute final colors */
	if (p_SdwNode->p_FatherModifier->ulFlags & MDF_SDW_UseNormales)
	{
			if (!(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric)) stVS.x = -stVS.x;

			MATH_NegEqualVector(&stVS);
			VSrc = GDI_gpst_CurDD->p_Current_Vertex_List;
			GEO_UseNormals(pst_Obj);
			VSrcNormales = pst_Obj->dst_PointNormal;
		
			while(VSrc < VSrcLast)
			{
				LightFactor2 = MATH_f_DotProduct(VSrcNormales,&stVS); 
				if (LightFactor2 > 0.0f)
				{
					LightFactor = *(float *)p_ulColorPtr;
					if (LightFactor >= 1.0f )  // > 1.0f | < 0.0f 
						*(float *)(p_ulColorPtr++) = LightFactor;
					else
					{
						LightFactor2 = 1.0f - LightFactor2;
						LightFactor2 *= LightFactor2;
						LightFactor2 = 1.0f - LightFactor2;
						*(float *)(p_ulColorPtr++) = LightFactor * LightFactor2;
					}
				}
				else
					*(p_ulColorPtr++) = 0;
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
			 	p_ulColorPtr++;//*/
		}
	}
	/* Then for each element Cull & draw shadow */
	
	
	VSrc = GDI_gpst_CurDD->p_Current_Vertex_List;
	DstUV = GDI_gpst_CurDD->pst_ComputingBuffers->ast_UV;
	p_ulColorPtr = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
	GEO_UseNormals(pst_Obj);
	VSrcNormales = pst_Obj->dst_PointNormal;
	
	p_First = pst_Obj ->dst_Element;
	p_Lst   = p_First + pst_Obj ->l_NbElements;
	stElement .pus_ListOfUsedIndex = NULL;
	
	stElement .ul_NumberOfUsedIndex = 0;
	stElement .p_MrmElementAdditionalInfo = NULL;
	stElement .pst_StripData = NULL;
#ifdef PSX2_TARGET	
	stElement .pst_StripDataPS2 = NULL;

	stElement .p_ElementCache = NULL;
#endif
	stElement .l_NbTriangles = 0;
	p_stTriangle = stElement .dst_Triangle = dst_Triangle;
	p_stTriangleDest = stElement.dst_Triangle;
	
	stfA.z += 1.0f;
	MATH_NegVector(&stVZ,&stVZ);
	stfA.z = -stfA.z;
#ifdef ACTIVE_EDITORS
	renderState_Shfited = 0;
	{
		p_First = pst_Obj ->dst_Element;
		p_Lst   = p_First + pst_Obj ->l_NbElements;
		while (p_First < p_Lst)
		{
			GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentColorField = NULL;
			GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentAlphaField = NULL;
			GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient = 0xffffff;
			GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor = 0;
			GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),p_First,GDI_gpst_CurDD->p_Current_Vertex_List,NULL,(GEO_tdst_UV *) GDI_gpst_CurDD->pst_ComputingBuffers->Current,pst_Obj->l_NbPoints);
			p_First++;
		}
	}
	return;
#endif
//*/
	
	while (p_First < p_Lst)
	{

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
			if (!(ulCullerColor & 0x00ffffff)) goto LABEL_Culled; // black//*/
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
				GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),&stElement,VSrc,VSrcNormales,NULL,pst_Obj->l_NbPoints);
#else			
				GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),&stElement,VSrc,VSrcNormales,(GEO_tdst_UV *) GDI_gpst_CurDD->pst_ComputingBuffers->Current,pst_Obj->l_NbPoints);
#endif		
				stElement.l_NbTriangles = 0;

				p_stTriangleDest = stElement.dst_Triangle;
			}
#endif			
			goto LABEL_NotCulled;
LABEL_Culled:
LABEL_NotCulled:
			p_stTriangle++;
		}
		
		}
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
				GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),&stElement,VSrc,VSrcNormales,NULL,pst_Obj->l_NbPoints);
#else				
				GDI_DrawIndexedTriangles((*GDI_gpst_CurDD),&stElement,VSrc,VSrcNormales,(GEO_tdst_UV *) GDI_gpst_CurDD->pst_ComputingBuffers->Current,pst_Obj->l_NbPoints);
#endif		
	}
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
static ULONG Ccccounnnnter = 0;
void SDW_RenderShadow(GDI_tdst_DisplayData *pst_DD, GEO_tdst_Object *pst_Obj , DD_tdst_ShadowStackNode *p_SdwNode)
{
#if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)
	GEO_tdst_ElementIndexedTriangles				*pst_Elem,*pst_LastElem;
	ULONG 								ulFlags , ulMustBeCleared;
	MATH_tdst_Vector						stfA,stVZ, stVU, stVV  ,stStock;
	MATH_tdst_Matrix						stInvertedMatrix ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Matrix						stMatrix;
	MATH_tdst_Vector						Delta;
	float								Limits[4];
#ifdef _GAMECUBE
	BOOL bInObjectList = GXI_Global_ACCESS(bPutObjectInObjectList);
	GXI_Global_ACCESS(bPutObjectInObjectList) = FALSE;
#endif
	
	/* Save current Matrix 		*/
	L_memset(&stInvertedMatrix , 0 , sizeof(MATH_tdst_Matrix));
	L_memset(&stMatrix , 0 , sizeof(MATH_tdst_Matrix));

	ulMustBeCleared = p_SdwNode->RDR_WindowAffectation & 0x80000000;
	
	p_SdwNode->RDR_WindowAffectation &= ~0x80000000;
	
	Delta.x = SDW_RDR_XS + (float)(p_SdwNode->RDR_WindowAffectation & SDW_RDR_AND) * SDW_RDR_XS * 2.0f;
	Delta.y = SDW_RDR_YS + (float)(p_SdwNode->RDR_WindowAffectation >> SDW_RDR_SHIFT) * SDW_RDR_YS * 2.0f;
	
	Limits[0] = Delta.x - SDW_RDR_XS;  	/*MIN X */
	Limits[1] = Delta.y - SDW_RDR_YS;   	/*MIN Y */
	Limits[2] = Delta.x + SDW_RDR_XS;   	/*MAX X */
	Limits[3] = Delta.y + SDW_RDR_YS + 1;   	/*MAX Y */

#ifdef _GAMECUBE 
	GXI_BeginShadowRendering(p_SdwNode->RDR_WindowAffectation, ulMustBeCleared);
#endif
	
	/* Clear Buffer	*/
#ifdef PSX2_TARGET
	if (ulMustBeCleared)
	{
		GEO_tdst_ElementIndexedTriangles				Element;
		GEO_tdst_IndexedTriangle					Triangle[2];
		GEO_Vertex							stClear[4];
		
		pst_DD->st_GDI.pfnv_SetViewMatrix_SDW(&MATH_gst_IdentityMatrix , Limits);
		
		/* Set render state 		*/
		ulFlags = MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_HideColor | MAT_Cul_Flag_NoZWrite;
		MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Disable);
		MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
		GDI_SetTextureBlending(&pst_DD->st_GDI,0xffffffff, ulFlags , 0);
		pst_DD->st_GDI.pfnv_SetTextureTarget(p_SdwNode->RDR_WindowAffectation , 0);
		Element.ul_NumberOfUsedIndex = 0;
		Element.pus_ListOfUsedIndex = NULL;
		
		Element.p_MrmElementAdditionalInfo = NULL;
		Element.pst_StripData = NULL;
		Element.pst_StripDataPS2 = NULL;
		Element.p_ElementCache = NULL;
		Element.dst_Triangle = Triangle;
		Triangle[0] . auw_Index[0] = 0;
		Triangle[0] . auw_Index[1] = 1;
		Triangle[0] . auw_Index[2] = 2;
		Triangle[1] . auw_Index[0] = 0;
		Triangle[1] . auw_Index[1] = 2;
		Triangle[1] . auw_Index[2] = 3;
		Element.l_NbTriangles = 2;
		MATH_InitVector((MATH_tdst_Vector *)&stClear[0] , Limits[2] , Limits[3] , 10000000000.0f);
		MATH_InitVector((MATH_tdst_Vector *)&stClear[1] , Limits[0] , Limits[3] , 10000000000.0f);
		MATH_InitVector((MATH_tdst_Vector *)&stClear[2] , Limits[0] , Limits[1] , 10000000000.0f);
		MATH_InitVector((MATH_tdst_Vector *)&stClear[3] , Limits[2] , Limits[1] , 10000000000.0f);
		pst_DD->pst_ComputingBuffers->CurrentColorField = NULL;
		GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentAlphaField = NULL;
		pst_DD->pst_ComputingBuffers->ulColorXor = 0;
		pst_DD->pst_ComputingBuffers->ul_Ambient = 0x00707070;
//		pst_DD->ulColorOr = 0;
		GDI_DrawIndexedTriangles((*pst_DD),&Element,stClear,NULL, NULL,pst_Obj->l_NbPoints);
	} 
	/* BEGIN Compute & Set shadow Matrix ------------------------------------------------------------------------------	*/
	MATH_SubVector(&stStock,&pst_DD ->pst_CurrentGameObject -> pst_GlobalMatrix -> T, &p_SdwNode -> stWorldCenter); 
	stfA.x = p_SdwNode -> stUCoef * MATH_f_DotProduct(&p_SdwNode -> stWorldVectUOrtho , &stStock );
	stfA.y = p_SdwNode -> stVCoef * MATH_f_DotProduct(&p_SdwNode -> stWorldVectVOrtho , &stStock );
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
	}//*/
	MATH_TransformVector(&stVU, &stInvertedMatrix , &p_SdwNode -> stWorldVectUOrtho);
	MATH_TransformVector(&stVV, &stInvertedMatrix , &p_SdwNode -> stWorldVectVOrtho);
	MATH_InitVectorToZero(&stVZ);
	MATH_ScaleVector(&stVU,&stVU,(SDW_RDR_XS * 2.0f) * p_SdwNode -> stUCoef); 
	MATH_ScaleVector(&stVV,&stVV,(SDW_RDR_YS * 2.0f) * p_SdwNode -> stVCoef); 
 	stfA.x *= SDW_RDR_XS * 2.0f;
	stfA.y *= SDW_RDR_YS * 2.0f;
	*(MATH_tdst_Vector *)&stMatrix.Ix = stVU;
	*(MATH_tdst_Vector *)&stMatrix.Jx = stVV;
	*(MATH_tdst_Vector *)&stMatrix.Kx = stVZ;
	MATH_TranspEq33Matrix(&stMatrix);
	stMatrix.T = 	stfA; 
	stMatrix.T.x += Delta.x;
	stMatrix.T.y += Delta.y;
	stMatrix.T.z = 	10000000000.0f;//*/
	Limits[0] += 1.5f;  	/*MIN X */
	Limits[1] += 1.5f;   	/*MIN Y */
	Limits[2] -= 1.5f;   	/*MAX X */
	Limits[3] -= 1.5f;   	/*MAX Y */
	pst_DD->st_GDI.pfnv_SetViewMatrix_SDW(&stMatrix , Limits);
	/* END Compute & Set shadow Matrix ---------------------------------------------------------------------------------- */
#else
	/* BEGIN Compute & Set shadow Matrix ------------------------------------------------------------------------------	*/
	MATH_SubVector(&stStock,&pst_DD ->pst_CurrentGameObject -> pst_GlobalMatrix -> T, &p_SdwNode -> stWorldCenter); 
	stfA.x = 2.0F * p_SdwNode -> stUCoef * MATH_f_DotProduct(&p_SdwNode -> stWorldVectUOrtho , &stStock );
	stfA.y = -2.0F * p_SdwNode -> stVCoef * MATH_f_DotProduct(&p_SdwNode -> stWorldVectVOrtho , &stStock );
#ifdef _GAMECUBE
	stfA.z = 0.0f;//-2.0f*MATH_f_DotProduct(&p_SdwNode->stWorldVectZ , &stStock );
#else
	stfA.z = 0.0f;
#endif
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
	pst_DD->st_GDI.pfnv_SetViewMatrix_SDW(&stMatrix, Limits);
	/* END Compute & Set shadow Matrix ---------------------------------------------------------------------------------- */
#endif
	ulFlags = MAT_Cul_Flag_HideAlpha | MAT_Cul_Flag_HideColor | MAT_Cul_Flag_NoZWrite;
	MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Disable);
	MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
	GDI_SetTextureBlending((*pst_DD),0xffffffff, ulFlags , 0);

	pst_DD->st_GDI.pfnv_SetTextureTarget(p_SdwNode->RDR_WindowAffectation , ulMustBeCleared);

	/* Draw shadow 			*/
	pst_DD->pst_ComputingBuffers->CurrentColorField = NULL;
	GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentAlphaField = NULL;
	pst_DD->pst_ComputingBuffers->ulColorXor = 0;
#ifdef GSP_USE_TRIPLE_BUFFERRING			
	pst_DD->pst_ComputingBuffers->ul_Ambient = 0xffffffff;
#else
	pst_DD->pst_ComputingBuffers->ul_Ambient = 0x70707070;
#endif
#if defined ACTIVE_EDITORS || defined(WIN32)
	renderState_Shfited = 0;
#endif
	/* BEGIN Render shadow */
	pst_Elem = pst_Obj->dst_Element;
	pst_LastElem = pst_Elem + pst_Obj->l_NbElements;
	while (pst_Elem < pst_LastElem)
	{
#ifdef PSX2_TARGET
		GspGlobal_ACCESS(Mode2X) |= 2;
#endif
		GDI_DrawIndexedTriangles((*pst_DD),pst_Elem,pst_DD->p_Current_Vertex_List,NULL,NULL , pst_Obj->l_NbPoints);
		pst_Elem++;
	}
	/* END Render shadow */
#ifdef PSX2_TARGET
	GspGlobal_ACCESS(Mode2X) &= ~2;
#endif

#ifdef _GAMECUBE 
	GXI_EndShadowRendering();
#endif

	/* Restore frame buffer target 	*/
	pst_DD->st_GDI.pfnv_SetTextureTarget(0xffffffff , 0);
	pst_DD->pst_ComputingBuffers->ul_Ambient = 0;
	pst_DD->pst_ComputingBuffers->CurrentColorField = pst_DD->pst_ComputingBuffers->ComputedColors;
	/* Restore Matrix*/	
	GDI_SetViewMatrix((*pst_DD) , pst_DD->st_Camera.pst_ObjectToCameraMatrix);

#ifdef _GAMECUBE
	GXI_Global_ACCESS(bPutObjectInObjectList) = bInObjectList;
#endif
	
#endif	// #if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)
}
//#endif 
/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG SDW_GET_SF(OBJ_tdst_GameObject *p_stSon , OBJ_tdst_GameObject *p_stSsdw)
{
	if ((OBJ_ul_FlagsIdentityGet(p_stSon) & OBJ_C_IdentityFlag_Hierarchy) && p_stSon->pst_Base && p_stSon->pst_Base->pst_Hierarchy && (p_stSon->pst_Base->pst_Hierarchy->pst_FatherInit))
	{
		if (p_stSsdw == p_stSon) 	
			return 1;
		else
			return SDW_GET_SF(p_stSon->pst_Base->pst_Hierarchy->pst_FatherInit,p_stSsdw);
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
#ifdef PSX2_TARGET
extern u_int NoMATDRAW;
extern u_int NoMulti;
extern u_int NoSDW;
extern u_int NoSPR;
#endif

void SDW_DisplayShadowsOnCurrentObject(GEO_tdst_Object *pst_Obj)
{
#if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)

#ifdef PSX2_TARGET
    ULONG X, Y;
#endif    
	DD_tdst_ShadowStack *p_ShadowStack;
 	DD_tdst_ShadowStackNode *p_SdwNode;
	ULONG ulFlags , ulShadowCounter;
	//    	DD_tdst_ShadowStackNode **pp_Frst,**pp_Lst;
	p_ShadowStack = (DD_tdst_ShadowStack *) GDI_gpst_CurDD->pv_ShadowStack;
	if (!p_ShadowStack) return;
	if (!p_ShadowStack->ulNumberOfNodes) return;
	if (!(GDI_gpst_CurDD ->ul_CurrentDrawMask & GDI_Cul_DM_ReceiveDynSdw)) return;


	if (!GDI_gpst_CurDD->pst_CurrentGameObject) return;
	if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap) return;
	if (GDI_gpst_CurDD->ul_DisplayInfo & (GDI_Cul_DI_RenderingTransparency|GDI_Cul_DI_Noshadows)) return;
	if (GDI_gpst_CurDD->ul_DisplayFlags & (GDI_Cul_DF_Noshadows|GDI_Cul_DF_DisplayWaypoint|GDI_Cul_DF_DisplayWaypointInNetwork|GDI_Cul_DF_DisplayInvisible)) return;
	

#if defined(PSX2_TARGET) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if (NoSDW) return;
#endif 

#if defined(_GAMECUBE)
	// no compute uv is called to set the matrix so set it to identity
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY ,GX_FALSE, GX_PTIDENTITY);
#endif 

	_GSP_BeginRaster(3);
	
	ulFlags = MAT_Cul_Flag_Bilinear ;
	ulFlags |= MAT_Cul_Flag_NoZWrite;
	//	ulFlags |= MAT_Cul_Flag_AlphaTest;
	//	ulFlags |= MAT_Cul_Flag_InvertAlpha;
#ifndef DBG_SHADOWS_UV_TILLING_OFF
	ulFlags |= MAT_Cul_Flag_TileU | MAT_Cul_Flag_TileV;
#endif

//#ifdef _GAMECUBE
//	GDI_gpst_CurDD->ul_CurrentDrawMask = ulFlags;
//#endif

	MAT_SET_Blending(ulFlags, MAT_Cc_Op_Copy);
	MAT_SET_ColorOp (ulFlags, MAT_Cc_ColorOp_Diffuse);
//	MAT_SET_AlphaTresh(ulFlags, 128);
	GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient = 0;
	GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
	
	
	ulShadowCounter = p_ShadowStack->ulLastNode;
	p_SdwNode = p_ShadowStack->pp_Nodes;
	while (ulShadowCounter--)
	{
		/* Bounding volume culling is done here */
		if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_IsCulled) goto NEXT_SHADOW;
#ifdef ACTIVE_EDITORS
		if (p_SdwNode -> p_FatherModifier->pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden) goto NEXT_SHADOW;
#endif
//*/
		if (p_SdwNode -> p_FatherModifier->pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled) goto NEXT_SHADOW;
		
		if (SDW_GET_SF(GDI_gpst_CurDD->pst_CurrentGameObject,p_SdwNode -> p_FatherModifier->pst_GO))
		{
//#ifdef PSX2_TARGET				
			/* Rendering shadow */
#if !defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_PC_RETAIL) && !defined(_XBOX) && !defined(_XENON_RENDER)
			//if (GDI_GetInterface(GDI_gpst_CurDD) != 0)
#endif
			if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_1RealTime) 
				SDW_RenderShadow(GDI_gpst_CurDD, pst_Obj , p_SdwNode);
//#endif				
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
#if defined(PSX2_TARGET) || defined(_GAMECUBE) || defined(_PC_RETAIL)  || defined(_XBOX) || defined(_XENON_RENDER)
		if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_1RealTime) 
#else
		if ((p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_1RealTime) /*&& (GDI_GetInterface(GDI_gpst_CurDD) != 0)*/)
#endif
		{
/*			ULONG X,Y;*/
			MAT_SET_Blending(ulFlags, MAT_Cc_Op_PSX2ShadowSpecific);
			/* 
				Texture is defined as follow:
				Size X Po2 = (_l_TSDW >> 26) & 0xf;
				Size Y Po2 = (_l_TSDW >> 22) & 0xf;
				Buffer X = (_l_TSDW >> 10) & 0x3ff;
				Buffer Y = (_l_TSDW >> 0) & 0x3ff; 
			*/
#ifdef PSX2_TARGET
			X = (ULONG)((float)(p_SdwNode->RDR_WindowAffectation & SDW_RDR_AND) * SDW_RDR_XS * 2.0f);
			Y = (ULONG)((float)((p_SdwNode->RDR_WindowAffectation & 0x7fffffff) >> SDW_RDR_SHIFT) * SDW_RDR_YS * 2.0f);
			GDI_SetTextureBlending(&GDI_gpst_CurDD->st_GDI , ((9 - SDW_RDR_SHIFT) << 26) | ((8 - SDW_RDR_SHIFTY) << 22) | (X << 10) | (Y), ulFlags , 0);
#else
			GDI_SetTextureBlending((*GDI_gpst_CurDD),0x80000000 | p_SdwNode->RDR_WindowAffectation, ulFlags , 0);
#endif

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
			GDI_SetTextureBlending((*GDI_gpst_CurDD),p_SdwNode->p_FatherModifier->TextureIndex , ulFlags , 0);
		}
		SDW_Display1ShadfowOn1Object(pst_Obj , p_SdwNode );
		if (p_SdwNode -> p_FatherModifier->ulFlags & MDF_SDW_IsAdditional)
		{
			p_SdwNode -> ulColor ^= 0xffffff;
		} 

NEXT_SHADOW:
		p_SdwNode++;
	}

	_GSP_EndRaster(3);
	
#endif	//#if !defined(_GAMECUBE) || !defined(USE_FULL_SCENE_SHADOWS)
}



#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif


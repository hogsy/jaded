#include "Precomp.h"


#include "TIMer/PROfiler/PROdefs.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/COLlision/COLmain.h"
#include "ENGine/Sources/INTersection/INTmain.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#ifdef JADEFUSION
#include "Light/LIGHTrejection.h"
#endif
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "SOFT/SOFTlinear.h"
#include "LIGHT/LIGHTstruct.h"

#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GRObject/GROedit.h"
#endif

#ifdef _GAMECUBE
#include "GXI_GC/GXI_def.h"
#include "GXI_GC/GXI_dbg.h"
#endif

#include "BASe/BENch/BENch.h"
#ifdef GSP_PS2_BENCH
extern unsigned int	n_Parralleles;
extern unsigned int	n_Omni;
extern unsigned int	n_Spot;
#endif

#ifdef _XBOX
#include "GX8/Gx8Light.h" 
#include "GX8/Gx8Color.h"
#endif 

#ifdef _XENON_RENDER
#include "XenonGraphics/XeGDInterface.h"
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeVertexShaderManager.h"
#include "XenonGraphics/XeMesh.h"
#include <algorithm>

#if defined(ACTIVE_EDITORS)
#include "XenonGraphics/XeUtils.h"
#endif
#endif

#if defined(_XENON_RENDER) && defined(ACTIVE_EDITORS)
BOOL LIGHT_gb_DisableXenonSend = FALSE;
#endif
//extern int NbrObjectClone;old clone

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
    Lumière omni
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 GC
 =======================================================================================================================
 */// Hardware omni light for GameCube --------------------------------------

#if (defined(_GAMECUBE) && defined(USE_HARDWARE_LIGHTS))
extern float AI_EvalFunc_BVSizeGet_C(OBJ_tdst_GameObject *_pst_GO);

void LIGHT_SendObjectToHWLight(BOOL _bDirect)
{
	static u8 			i = 0;
	
	MATH_tdst_Vector 	st_Point;
	MATH_tdst_Vector	st_ScaleVect;
	GXColor 			light_color;
	f32					f_Factor, f_Far, f_Near, f_Dist;
	u32 				ul_Color;

	if( (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Paint) || (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Absorb))
	{
//		OSReport("Unsupported light type.\n");
		return;
	}
	
	// omni light with soft attenuation (omni const)
	if( LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_OmniConst )
	{
		MATH_GetScaleVector(&st_ScaleVect, &LIGHT_gst_NIM);
		f_Factor = (st_ScaleVect.x + st_ScaleVect.y + st_ScaleVect.z) * .333f;
		f_Far = LIGHT_gpst_Cur->st_Omni.f_Far * f_Factor;
		f_Near = LIGHT_gpst_Cur->st_Omni.f_Near * f_Factor;
        f_Dist = MATH_f_NormVector(&LIGHT_gst_LP.T);
        
        if (f_Dist > f_Far)
        	return;
        
        if (f_Dist > f_Near)
        {
            f_Factor = (f_Far - f_Dist) / (f_Far - f_Near);
            ul_Color = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
        }
        else
            ul_Color = LIGHT_gul_Color;
     
		if(GXI_Global_ACCESS(LightMask)==GX_LIGHT_NULL)
		{
			GXI_Global_ACCESS(LightMask) = GX_LIGHT0;
			i = 0;
		}
		else
		{
			if(i>=MAX_HARDWARE_LIGHTS)
			{
//				OSReport("Too many lights\n");
				return;
			}
			GXI_Global_ACCESS(LightMask) = (GXLightID)(GXI_Global_ACCESS(LightMask) | (GX_LIGHT0<<i));
		}
		
		light_color.r = (ul_Color & 0x000000ff);
		light_color.g = ((ul_Color & 0x0000ff00)>>8);
		light_color.b = ((ul_Color & 0x00ff0000)>>16);
		light_color.a = 255;
					
		VECScale((Vec*)&LIGHT_gst_LP.T, (Vec*)&st_Point, 1000.f);
		MTXMultVec(*((Mtx*)&GXI_Global_ACCESS(current_modelview_matrix)), (Vec*)&st_Point, (Vec*)&st_Point);	
		GXInitLightPosv(&GXI_Global_ACCESS(LightObjects)[i], (Vec*)&st_Point);

#ifndef _FINAL_
		if(g_bShowLights)					
			GXI_DBG_DrawLight((Vec*)&st_Point);
#endif						

		// No attenuation
		GXInitLightAttn(&GXI_Global_ACCESS(LightObjects)[i], 1.0f, 0, 0, 1.0f, 0.0f, 0.0f );		    
		GXInitLightColor(&GXI_Global_ACCESS(LightObjects)[i], light_color);
		GXLoadLightObjImm(&GXI_Global_ACCESS(LightObjects)[i], (GXLightID)(GX_LIGHT0<<(i)));
		++i;
	}
	
	// omni with hardware attenuation or directional light
	
	else
	{
		MATH_tdst_Vector *pst_Dir;
				        
		if(GXI_Global_ACCESS(LightMask)==GX_LIGHT_NULL)
		{
			GXI_Global_ACCESS(LightMask) = GX_LIGHT0;
			i = 0;
		}
		else
		{
			if(i>=MAX_HARDWARE_LIGHTS)
			{
//				OSReport("Too many lights\n");
				return;
			}
			GXI_Global_ACCESS(LightMask) = (GXLightID)(GXI_Global_ACCESS(LightMask) | (GX_LIGHT0<<i));
		}
		
		light_color.r = (LIGHT_gul_Color & 0x000000ff);
		light_color.g = ((LIGHT_gul_Color & 0x0000ff00)>>8);
		light_color.b = ((LIGHT_gul_Color & 0x00ff0000)>>16);
		light_color.a = 255;
		
		GXInitLightColor(&GXI_Global_ACCESS(LightObjects)[i], light_color);


		if(_bDirect)
		{
			pst_Dir = MATH_pst_GetYAxis(&LIGHT_gst_LP);
			VECScale((Vec*)pst_Dir, (Vec*)pst_Dir, 100000);
			VECAdd((Vec*)&LIGHT_gst_LP.T, (Vec*)pst_Dir, (Vec*)&st_Point);
			MTXMultVec(*((Mtx*)&GXI_Global_ACCESS(current_modelview_matrix)), (Vec*)&st_Point, (Vec*)&st_Point);		
			// No attenuation (distance or angle)
			GXInitLightAttn(&GXI_Global_ACCESS(LightObjects)[i], 1.0f, 0.f, 0.f, 1.0f , 0.f, 0.f);
		}
		else // omni
		{
			// Compute coeffs that are roughly equivalent to near/far
			f32 k0,k1,k2; // These coefs must be positive so that the light function always decreases.
			f32 fInvDelta,fFar2,fNear2,fTempN,fTempF,fFDivN,fMinTempF;
			static f32 fL0 = 1.1f; // Light coef at distance 0
			static f32 fLN = 0.9f; // Light coef at distance near
			static f32 fLF = 0.2f; // Light coef at distance far
			
			MATH_GetScaleVector(&st_ScaleVect, &LIGHT_gst_NIM);
			f_Factor = (st_ScaleVect.x + st_ScaleVect.y + st_ScaleVect.z) * .333f;
			f_Far = LIGHT_gpst_Cur->st_Omni.f_Far * f_Factor;
			f_Near = LIGHT_gpst_Cur->st_Omni.f_Near * f_Factor;
			
			
			ERR_X_Assert(f_Near > 0);
			if (f_Far < f_Near)
			{
				f32 fTemp = f_Far;
				f_Far = f_Near;
				f_Near = fTemp;
				ERR_X_Assert(f_Near > 0);
			}
			ERR_X_Assert(f_Far > f_Near);
			
			k0 = 1.f/fL0;
			fFar2 = f_Far*f_Far;
			fNear2 = f_Near*f_Near;
			fInvDelta = 1.f / (f_Near*fFar2 - f_Far*fNear2);
			fTempN = (1.f/fLN) - k0;
			fTempF = (1.f/fLF) - k0;
			
			// We limit fTempF so that k1 and k2 are positive. 
			fFDivN = f_Far/f_Near;
			fMinTempF = fTempN * fFDivN;
			fTempF = MATH_f_FloatLimit(fTempF,fMinTempF,fMinTempF*fFDivN);
			
			k1 = fInvDelta * (fTempN * fFar2 - fTempF * fNear2);
			k2 = fInvDelta * (-fTempN * f_Far + fTempF * f_Near);
			
			MTXMultVec(*((Mtx*)&GXI_Global_ACCESS(current_modelview_matrix)), (Vec*)&LIGHT_gst_LP.T, (Vec*)&st_Point);	
			// Distance attenuation only
			GXInitLightAttn(&GXI_Global_ACCESS(LightObjects)[i], 1.0f, 0, 0, k0, k1, k2 );		    
		}
				
		GXInitLightPosv(&GXI_Global_ACCESS(LightObjects)[i], (Vec*)&st_Point);
		
#ifndef _FINAL_
		if(g_bShowLights)					
			GXI_DBG_DrawLight((Vec*)&st_Point);
#endif		

		GXLoadLightObjImm(&GXI_Global_ACCESS(LightObjects)[i], (GXLightID)(GX_LIGHT0<<(i)));
		++i;	
	}
}
										
#endif
																
/*
 =======================================================================================================================
 PS2
 =======================================================================================================================
 */
#ifdef PSX2_TARGET

extern void asm_MATH_SerialSphericalLight(ULONG *);
extern void asm_MATH_SerialSphericalLight_PAINT(ULONG *);
extern void asm_MATH_SerialSphericalLight_C(ULONG *);
extern void asm_MATH_SerialSphericalLight_PAINT_C(ULONG *);
extern void GSP_ASM_ComputeDirect(MATH_tdst_Vector *pst_N1,ULONG Number , MATH_tdst_Vector *pst_Dir , MATH_tdst_Vector *pu_ulColors , ULONG COLOR);
extern void GSP_ASM_ComputeDirect_C(ULONG *pst_N1,ULONG Number , MATH_tdst_Vector *pst_Dir , MATH_tdst_Vector *pu_ulColors , ULONG COLOR);

void LIGHT_SendObjectToSpherical(ULONG *pul_Diffuse, MATH_tdst_Vector *pst_Normal,ULONG *pCompressed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_ScaleVect;
    GEO_Vertex          V, *pst_Point, *pst_Last;
	float				f_Factor, f_Far, f_Near , f_Dist;
    ULONG               ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    

#if 1
    if ((LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_OmniConst | LIGHT_Cul_LF_Paint)) == LIGHT_Cul_LF_OmniConst)
    {
		MATH_GetScaleVector(&st_ScaleVect, &LIGHT_gst_NIM);
		f_Factor = (st_ScaleVect.x + st_ScaleVect.y + st_ScaleVect.z) * .333f;
        f_Far = LIGHT_gpst_Cur->st_Omni.f_Far * f_Factor;
	    f_Near = LIGHT_gpst_Cur->st_Omni.f_Near * f_Factor;
#ifdef GSP_PS2_BENCH
		n_Parralleles += LIGHT_gpst_CurObject->l_NbPoints;
#endif

        MATH_NegVector(&V, &LIGHT_gst_LP.T);
        f_Dist = V.x*V.x + V.y*V.y + V.z*V.z;
        if (f_Dist > f_Far*f_Far) return;
        MATH_ScaleEqualVector( &V, fInvSqrt( f_Dist) );
        if (f_Dist > f_Near * f_Near)
        {
	        f_Dist = fSqrt(f_Dist);
            f_Factor = (f_Far - f_Dist) / (f_Far - f_Near);
            ul_Color = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
        }
        else
            ul_Color = LIGHT_gul_Color;
        MATH_NegVector(&V, &V);
        if (pCompressed)
        {
        	GSP_ASM_ComputeDirect_C(pCompressed,LIGHT_gpst_CurObject->l_NbPoints , &V, pul_Diffuse , ul_Color);
        }
        else 
	        GSP_ASM_ComputeDirect(pst_Normal,LIGHT_gpst_CurObject->l_NbPoints , &V, pul_Diffuse , ul_Color);//*/
        return;
     }
#endif

#ifdef GSP_PS2_BENCH
	n_Omni += LIGHT_gpst_CurObject->l_NbPoints;
#endif

    pst_Point = LIGHT_gpst_CurPointsBuffer;
	pst_Last = pst_Point + LIGHT_gpst_CurObject->l_NbPoints;

    /* Scale Near & Far */
	MATH_GetScaleVector(&st_ScaleVect, &LIGHT_gst_NIM);
	f_Factor = (st_ScaleVect.x + st_ScaleVect.y + st_ScaleVect.z) * .333f;
	f_Far = LIGHT_gpst_Cur->st_Omni.f_Far * f_Factor;
	f_Near = LIGHT_gpst_Cur->st_Omni.f_Near * f_Factor;




	{
			ULONG 		AsmParrams[10];
			GEO_Vertex	LightCenter ONLY_PSX2_ALIGNED(16);
			LightCenter.x = LIGHT_gst_LP.T.x;
			LightCenter.y = LIGHT_gst_LP.T.y;
			LightCenter.z = LIGHT_gst_LP.T.z;
			AsmParrams[0] = (ULONG)pst_Point;
			AsmParrams[1] = (ULONG)pst_Normal;
			AsmParrams[2] = (ULONG)LIGHT_gpst_CurObject->l_NbPoints;
			AsmParrams[3] = (ULONG)&LightCenter;
			*(float *)&AsmParrams[4] = 1.0f / ((1.0f / f_Near) - (1.0f / f_Far));
			*(float *)&AsmParrams[5] = -1.0f / f_Far;
			AsmParrams[6] = LIGHT_gul_Color;
			AsmParrams[7] = (ULONG)pul_Diffuse;
			if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Paint)
				asm_MATH_SerialSphericalLight_PAINT(AsmParrams);
			else
			{
				if (pCompressed)
				{
					AsmParrams[1] = (ULONG)pCompressed;
					asm_MATH_SerialSphericalLight_C(AsmParrams);//*/
				} else
				{
					asm_MATH_SerialSphericalLight(AsmParrams);//*/
				}
			}
	}
}

	    
/*
 =======================================================================================================================
 Other platform
 =======================================================================================================================
 */
#else

void LIGHT_SendObjectToSpherical(ULONG *pul_Diffuse, MATH_tdst_Vector *pst_Normal , ULONG *pCompressedNormals)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_ScaleVect;
    GEO_Vertex          *pst_Point, *pst_Last;
	float				f_ConstFactor, f_Factor, f_Far, f_Near;
    float               f_Dist, f_Dist2, f_Far2, f_Near2, f_Div, f_Div2;
	MATH_tdst_Vector	st_LightToPoint;
	ULONG				ul_Diffuse;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_Omni);
    PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_Lights_OmniNbVertex, LIGHT_gpst_CurObject->l_NbPoints);

	pst_Point = LIGHT_gpst_CurPointsBuffer;
	pst_Last = pst_Point + LIGHT_gpst_CurObject->l_NbPoints;

	/* Scale Near & Far */
	MATH_GetScaleVector(&st_ScaleVect, &LIGHT_gst_NIM);
	f_Factor = (st_ScaleVect.x + st_ScaleVect.y + st_ScaleVect.z) * .333f;
	f_Far = LIGHT_gpst_Cur->st_Omni.f_Far * f_Factor;
	f_Near = LIGHT_gpst_Cur->st_Omni.f_Near * f_Factor;

    f_Div = (f_Far - f_Near);
	if(f_Div) f_Div = fOptInv(f_Div);

	f_Far2 = fNormalSqr(f_Far);
	f_Near2 = fNormalSqr(f_Near);
	f_Div2 = (f_Far2 - f_Near2);
	if(f_Div2) f_Div2 = fOptInv(f_Div2);

	if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Absorb)
	{
		if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Paint)
		{
			for(; pst_Point < pst_Last; pst_Point++, pul_Diffuse++)
			{
				MATH_SubVector(&st_LightToPoint, VCast(pst_Point), &LIGHT_gst_LP.T);
				f_Dist2 = MATH_f_SqrVector(&st_LightToPoint);
				if(f_Dist2 > f_Far2) continue;

				if(f_Dist2 > f_Near2)
				{
					f_Dist = fOptSqrt(f_Dist2);
					f_Factor = (f_Far - f_Dist) * f_Div;
				}
				else
					f_Factor = 1.0f;
				ul_Diffuse = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
				LIGHT_SubColor(pul_Diffuse, ul_Diffuse);
			}
		}
		else
		{
			for(; pst_Point < pst_Last; pst_Point++, pul_Diffuse++, pst_Normal++)
			{
				MATH_SubVector(&st_LightToPoint, VCast( pst_Point ), &LIGHT_gst_LP.T);
				f_Dist2 = MATH_f_SqrVector(&st_LightToPoint);
				if(f_Dist2 > f_Far2) continue;
				f_Factor = -MATH_f_DotProduct(pst_Normal, &st_LightToPoint);
				if(*(LONG *) &f_Factor & 0x80000000) continue;

				f_Dist = fOptSqrt(f_Dist2);
				f_Factor *= fOptInv(f_Dist);

				if(f_Dist2 > f_Near2) f_Factor *= (f_Far - f_Dist) * f_Div;

				ul_Diffuse = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
				LIGHT_SubColor(pul_Diffuse, ul_Diffuse);
			}
		}
	}
	else
	{
		if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Paint)
		{
			for(; pst_Point < pst_Last; pst_Point++, pul_Diffuse++)
			{
				MATH_SubVector(&st_LightToPoint, VCast(pst_Point), &LIGHT_gst_LP.T);
				f_Dist2 = MATH_f_SqrVector(&st_LightToPoint);
				if(f_Dist2 > f_Far2) continue;

				f_Dist = fOptSqrt(f_Dist2);
				f_Factor *= fOptInv(f_Dist);

				if(f_Dist2 > f_Near2)
					f_Factor = (f_Far - f_Dist) * f_Div;
				else
					f_Factor = 1.0f;

				ul_Diffuse = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
				LIGHT_AddColor(pul_Diffuse, ul_Diffuse);
			}
		}
		else
		{
            if ( LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_OmniConst )
            {
                MATH_NegVector(&st_LightToPoint, &LIGHT_gst_LP.T);
                f_Dist = MATH_f_NormVector( &st_LightToPoint );
                if (f_Dist > f_Far) goto LightOmni_End;
                MATH_DivEqualVector( &st_LightToPoint, f_Dist );
                if (f_Dist > f_Near)
                    f_ConstFactor = ( f_Far2 - (f_Dist * f_Dist)) * f_Div2;
                else
                    f_ConstFactor = 1;
        
                for(; pst_Point < pst_Last; pst_Point++, pul_Diffuse++, pst_Normal++)
			    {
				    f_Factor = -MATH_f_DotProduct(pst_Normal, &st_LightToPoint);
				    if(f_Factor <= 0.0f) continue;
				    LIGHT_AddColor(pul_Diffuse, LIGHT_ul_MulColor(LIGHT_gul_Color, (f_Factor * f_ConstFactor) ) );
                }
            }
            else
            {
                for(; pst_Point < pst_Last; pst_Point++, pul_Diffuse++, pst_Normal++)
			    {
                    st_LightToPoint.x = pst_Point->x - LIGHT_gst_LP.T.x;
                    f_Dist2 = st_LightToPoint.x * st_LightToPoint.x;
                    if(f_Dist2 > f_Far2) continue;
                    st_LightToPoint.y = pst_Point->y - LIGHT_gst_LP.T.y;
                    f_Dist2 += st_LightToPoint.y * st_LightToPoint.y;
                    if(f_Dist2 > f_Far2) continue;
                    st_LightToPoint.z = pst_Point->z - LIGHT_gst_LP.T.z;
                    f_Dist2 += st_LightToPoint.z * st_LightToPoint.z;
                    if(f_Dist2 > f_Far2) continue;

				    f_Factor = -MATH_f_DotProduct(pst_Normal, &st_LightToPoint);
				    if(f_Factor <= 0.0f) continue;
				    if(f_Dist2 > f_Near2) f_Factor *= (f_Far2 - f_Dist2) * f_Div2;
                    f_Factor *= fOptInvSqrt( f_Dist2 );
				    LIGHT_AddColor(pul_Diffuse, LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor) );
            
			    }
            }
		}
	}

LightOmni_End:
    PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_Omni);
}

#endif


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
    Lumière directes
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
 
/*
 =======================================================================================================================
 PS2
 =======================================================================================================================
 */
#ifdef PSX2_TARGET


void LIGHT_SendObjectToDirect(ULONG *pul_Diffuse, MATH_tdst_Vector *pst_Normal,ULONG *pCompressed)
{
	MATH_tdst_Vector	 *pst_Dir;
	pst_Dir = MATH_pst_GetYAxis(&LIGHT_gst_LP);

#ifdef GSP_PS2_BENCH
	n_Parralleles  += LIGHT_gpst_CurObject->l_NbPoints;
#endif
    if (pCompressed)
    {
    	GSP_ASM_ComputeDirect_C(pCompressed,LIGHT_gpst_CurObject->l_NbPoints , pst_Dir, pul_Diffuse , LIGHT_gul_Color);
    }
    else 
		GSP_ASM_ComputeDirect(pst_Normal,LIGHT_gpst_CurObject->l_NbPoints , pst_Dir , pul_Diffuse , LIGHT_gul_Color);
}

/*
 =======================================================================================================================
 Other platform
 =======================================================================================================================
 */
#else

void LIGHT_SendObjectToDirect(ULONG *pul_Diffuse, MATH_tdst_Vector *pst_Normal,ULONG *pCompressed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Diffuse;
	MATH_tdst_Vector	*pst_Last, *pst_Dir;
	float				f_Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_Direct);
    PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_Lights_DirectNbVertex, LIGHT_gpst_CurObject->l_NbPoints);

	pst_Dir = MATH_pst_GetYAxis(&LIGHT_gst_LP);

	MATH_NegEqualVector(pst_Dir);
	pst_Last = pst_Normal + LIGHT_gpst_CurObject->l_NbPoints;

	if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Absorb)
	{
		if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Paint)
		{
			for(; pst_Normal < pst_Last; pul_Diffuse++, pst_Normal++) LIGHT_SubColor(pul_Diffuse, LIGHT_gul_Color);
		}
		else
		{
			for(; pst_Normal < pst_Last; pul_Diffuse++, pst_Normal++)
			{
				f_Factor = fMax(-MATH_f_DotProduct(pst_Normal, pst_Dir), 0.0f);
				ul_Diffuse = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
				LIGHT_SubColor(pul_Diffuse, ul_Diffuse);
			}
		}
	}
	else
	{
		if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Paint)
		{
			for(; pst_Normal < pst_Last; pul_Diffuse++, pst_Normal++) LIGHT_AddColor(pul_Diffuse, LIGHT_gul_Color);
		}
		else
		{
			for(; pst_Normal < pst_Last; pul_Diffuse++, pst_Normal++)
			{
				f_Factor = fMax(-MATH_f_DotProduct(pst_Normal, pst_Dir), 0.0f);
				ul_Diffuse = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
				LIGHT_AddColor(pul_Diffuse, ul_Diffuse);
			}
		}
	}
	MATH_NegEqualVector(pst_Dir);
    PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_Direct);
}

#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
    Lumière spot
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 PS2
 =======================================================================================================================
 */
#ifdef PSX2_TARGET

extern void asm_MATH_SerialSpotLight(ULONG *);

void LIGHT_SendObjectToSpot(ULONG *pul_Diffuse, MATH_tdst_Vector *pst_Normal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex          *pst_Point, *pst_Last;
	float				 f_Factor;
	MATH_tdst_Vector	  *pst_Dir;
	float				f_Near2, f_Far2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dir = MATH_pst_GetYAxis(&LIGHT_gst_LP);
	
#ifdef GSP_PS2_BENCH
	n_Spot  += LIGHT_gpst_CurObject->l_NbPoints;
#endif
	{
			ULONG 		AsmParrams[10];
			MATH_tdst_Vector	st_ScaleVect;
			GEO_Vertex	LightDir ONLY_PSX2_ALIGNED(16);
			GEO_Vertex	LightFactors ONLY_PSX2_ALIGNED(16);
			GEO_Vertex	LightCenter ONLY_PSX2_ALIGNED(16);
			pst_Point = LIGHT_gpst_CurPointsBuffer;
			pst_Last = pst_Point + LIGHT_gpst_CurObject->l_NbPoints;

			/* Scale Near & Far */
			MATH_GetScaleVector(&st_ScaleVect, &LIGHT_gst_NIM);
			f_Factor = (st_ScaleVect.x + st_ScaleVect.y + st_ScaleVect.z) * .333f;
			f_Far2 = LIGHT_gpst_Cur->st_Spot.f_Far * f_Factor;
			f_Near2 = LIGHT_gpst_Cur->st_Spot.f_Near * f_Factor;
			LightCenter.x = LIGHT_gst_LP.T.x;
			LightCenter.y = LIGHT_gst_LP.T.y;
			LightCenter.z = LIGHT_gst_LP.T.z;
			AsmParrams[0] = (ULONG)pst_Point;
			AsmParrams[1] = (ULONG)pst_Normal;
			AsmParrams[2] = (ULONG)LIGHT_gpst_CurObject->l_NbPoints;
			AsmParrams[3] = (ULONG)&LightCenter;
			*(float *)&AsmParrams[4] = 1.0f / ((1.0f / f_Near2) - (1.0f / f_Far2));
			*(float *)&AsmParrams[5] = -1.0f / f_Far2;
			AsmParrams[6] = LIGHT_gul_Color;
			AsmParrams[7] = (ULONG)pul_Diffuse;
			
			LightDir.x = pst_Dir->x;
			LightDir.y = pst_Dir->y;
			LightDir.z = pst_Dir->z;
			AsmParrams[8] = (ULONG)&LightDir;
			
			LightFactors.x = -fOptCos(LIGHT_gpst_Cur->st_Spot.f_BigAlpha);
			LightFactors.y = fOptCos(LIGHT_gpst_Cur->st_Spot.f_LittleAlpha) + LightFactors.x;
			if (LightFactors.y != 0.0f) LightFactors.y = 1.0f / LightFactors.y;
			else LightFactors.y = Cf_Infinit;
			
			AsmParrams[9] = (ULONG)&LightFactors;
			
			asm_MATH_SerialSpotLight(AsmParrams);
	}
}


/*
 =======================================================================================================================
 Other platform
 =======================================================================================================================
 */
#else

void LIGHT_SendObjectToSpot(ULONG *pul_Diffuse, MATH_tdst_Vector *pst_Normal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Diffuse;
	GEO_Vertex          *pst_Point, *pst_Last;
	float				f_Dist, f_Dist2, f_Factor, f_Angle, f_Div, f_DivAngle;
	MATH_tdst_Vector	st_LightToPoint, st_Product, *pst_Dir;
	float				f_LittleSin, f_BigSin;
	float				f_Near2, f_Far2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_Spot);
    PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_Lights_SpotNbVertex, LIGHT_gpst_CurObject->l_NbPoints);

	pst_Dir = MATH_pst_GetYAxis(&LIGHT_gst_LP);
	
	f_Near2 = LIGHT_gpst_Cur->st_Spot.f_Near * LIGHT_gpst_Cur->st_Spot.f_Near;
	f_Far2 = LIGHT_gpst_Cur->st_Spot.f_Far * LIGHT_gpst_Cur->st_Spot.f_Far;
	

	f_Div = (f_Far2 - f_Near2);
	if(f_Div) f_Div = fOptInv(f_Div);

	f_LittleSin = fOptSin(LIGHT_gpst_Cur->st_Spot.f_LittleAlpha);
	f_BigSin = fOptSin(LIGHT_gpst_Cur->st_Spot.f_BigAlpha);

	f_DivAngle = f_BigSin - f_LittleSin;
	if(f_DivAngle) f_DivAngle = fOptInv(f_DivAngle);

	pst_Point = LIGHT_gpst_CurPointsBuffer;
	pst_Last = pst_Point + LIGHT_gpst_CurObject->l_NbPoints;

	if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Absorb)
	{
		if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Paint)
		{
			for(; pst_Point < pst_Last; pst_Point++, pul_Diffuse++)
			{
				MATH_SubVector(&st_LightToPoint, VCast( pst_Point ), &LIGHT_gst_LP.T);
				if(MATH_f_DotProduct(&st_LightToPoint, pst_Dir) > 0) continue;

				f_Dist2 = MATH_f_SqrVector(&st_LightToPoint);

				if(f_Dist2 > f_Far2) continue;

				f_Factor = 1.0;
				if(f_Dist2 > f_Near2) f_Factor -= ((f_Dist2 - f_Near2) * f_Div);

				MATH_CrossProduct(&st_Product, pst_Dir, &st_LightToPoint);
				f_Dist = fOptSqrt(f_Dist2);
				f_Angle = MATH_f_NormVector(&st_Product) / f_Dist;
				if(f_Angle > f_BigSin) continue;
				if(f_Angle > f_LittleSin) f_Factor *= (f_BigSin - f_Angle) * f_DivAngle;

				ul_Diffuse = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
				LIGHT_SubColor(pul_Diffuse, ul_Diffuse);
			}
		}
		else
		{
			for(; pst_Point < pst_Last; pst_Point++, pul_Diffuse++, pst_Normal++)
			{
				MATH_SubVector(&st_LightToPoint, VCast( pst_Point ), &LIGHT_gst_LP.T);
				if(MATH_f_DotProduct(&st_LightToPoint, pst_Dir) > 0) continue;

				f_Dist2 = MATH_f_SqrVector(&st_LightToPoint);

				if(f_Dist2 > f_Far2) continue;

				f_Factor = -MATH_f_DotProduct(pst_Normal, &st_LightToPoint);
				if(f_Factor <= 0.0f) continue;
				f_Dist = fOptSqrt(f_Dist2);
				f_Factor /= f_Dist;

				if(f_Dist2 > f_Near2) f_Factor *= 1.0f - ((f_Dist2 - f_Near2) * f_Div);

				MATH_CrossProduct(&st_Product, pst_Dir, &st_LightToPoint);
				f_Angle = MATH_f_NormVector(&st_Product) / f_Dist;
				if(f_Angle > f_BigSin) continue;
				if(f_Angle > f_LittleSin) f_Factor *= (f_BigSin - f_Angle) * f_DivAngle;

				ul_Diffuse = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
				LIGHT_SubColor(pul_Diffuse, ul_Diffuse);
			}
		}
	}
	else
	{
		if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Paint)
		{
			for(; pst_Point < pst_Last; pst_Point++, pul_Diffuse++)
			{
				MATH_SubVector(&st_LightToPoint, VCast(pst_Point), &LIGHT_gst_LP.T);
				if(MATH_f_DotProduct(&st_LightToPoint, pst_Dir) > 0) continue;

				f_Dist2 = MATH_f_SqrVector(&st_LightToPoint);

				if(f_Dist2 > f_Far2) continue;

				f_Factor = 1.0;
				if(f_Dist2 > f_Near2) f_Factor -= ((f_Dist2 - f_Near2) * f_Div);

				MATH_CrossProduct(&st_Product, pst_Dir, &st_LightToPoint);
				f_Dist = fOptSqrt(f_Dist2);
				f_Angle = MATH_f_NormVector(&st_Product) / f_Dist;
				if(f_Angle > f_BigSin) continue;
				if(f_Angle > f_LittleSin) f_Factor *= (f_BigSin - f_Angle) * f_DivAngle;

				ul_Diffuse = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
				LIGHT_AddColor(pul_Diffuse, ul_Diffuse);
			}
		}
		else
		{
            for(; pst_Point < pst_Last; pst_Point++, pul_Diffuse++, pst_Normal++)
            {
	            MATH_SubVector(&st_LightToPoint, VCast(pst_Point), &LIGHT_gst_LP.T);
	            if(MATH_f_DotProduct(&st_LightToPoint, pst_Dir) > 0) continue;

	            f_Dist2 = MATH_f_SqrVector(&st_LightToPoint);

	            if(f_Dist2 > f_Far2) continue;

	            f_Factor = -MATH_f_DotProduct(pst_Normal, &st_LightToPoint);
	            if(f_Factor <= 0.0f) continue;
	            f_Dist = fOptSqrt(f_Dist2);
	            f_Factor /= f_Dist;

	            if(f_Dist2 > f_Near2) f_Factor *= 1.0f - ((f_Dist2 - f_Near2) * f_Div);

	            MATH_CrossProduct(&st_Product, pst_Dir, &st_LightToPoint);
	            f_Angle = MATH_f_NormVector(&st_Product) / f_Dist;
	            if(f_Angle > f_BigSin) continue;
	            if(f_Angle > f_LittleSin) f_Factor *= (f_BigSin - f_Angle) * f_DivAngle;

	            ul_Diffuse = LIGHT_ul_MulColor(LIGHT_gul_Color, f_Factor);
	            LIGHT_AddColor(pul_Diffuse, ul_Diffuse);
            }
		}
	}
    
    PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_Spot);
}

#endif	

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
    calcul des lumières : fonction principale
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/* start computing RLI */
#define M_4Edit_ComputeRLIStart()\
 	if(LIGHT_gl_ComputeRLI)\
	{\
        pul_RLI = (ULONG *) 1;\
        if ( (LIGHT_gl_ComputeRLI & 4) && (!(_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected)) )\
            pul_RLI = NULL;\
        else if (_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_RLILocked)\
        {\
            char sz_Text[256];\
            pul_RLI = NULL;\
            sprintf( sz_Text, "RLI locked on (%s)", _pst_Node->sz_Name ? _pst_Node->sz_Name : "unknown" );\
            LINK_PrintStatusMsg( sz_Text );\
        }\
        if (pul_RLI)\
        {\
		    l_Size = sizeof(ULONG) * LIGHT_gpst_CurObject->l_NbPoints;\
	        pul_RLI = (ULONG*)L_malloc(l_Size);\
		    L_memset(pul_RLI, 0, l_Size);\
		    pul_Diffuse = pul_RLI;\
\
    		if(_pst_Node->pst_Base->pst_Visu->dul_VertexColors == NULL)\
	    	{\
		    	_pst_Node->pst_Base->pst_Visu->dul_VertexColors = (ULONG *) MEM_p_Alloc(l_Size + 4);\
			    _pst_Node->pst_Base->pst_Visu->dul_VertexColors[0] = LIGHT_gpst_CurObject->l_NbPoints;\
    			L_memset(_pst_Node->pst_Base->pst_Visu->dul_VertexColors + 1, 0, l_Size);\
	    	}\
            else if (LIGHT_gl_ComputeRLI & 2)\
            {\
                if ( _pst_Node->ul_EditorFlags & OBJ_C_EditFlags_RLINotReset )\
                {\
                    ULONG *rli1, *rli2;\
\
                    rli1 = _pst_Node->pst_Base->pst_Visu->dul_VertexColors;\
			        rli2 = rli1 + (rli1[0] + 1);\
			        rli1++;\
			        for(; rli1 < rli2; rli1++) \
                        *rli1 &= ~LIGHT_gul_ComputeRLIMask;\
                    _pst_Node->ul_EditorFlags &= ~OBJ_C_EditFlags_RLINotReset;\
                }\
            }\
        }\
	}

/* ajoute aux RLI les résultats d'une lumière */
#define M_4Edit_ComputeRLIAdd()\
    if(LIGHT_gl_ComputeRLI && pul_RLI)\
    {\
        if ( ((LIGHT_gl_ComputeRLI & 8) == 0) || ((*ppst_LightNode)->ul_EditorFlags & OBJ_C_EditFlags_Selected) )\
        {\
	        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/\
	        ULONG				*pul_CurRLI, *pul_CurVC;\
	        GEO_Vertex          *pst_Point, *pst_Last;\
	        WOR_tdst_World		*pst_World;\
	        MATH_tdst_Vector	*pst_LightPos, st_Point, st_Dir;\
	        MATH_tdst_Matrix	M;\
	        float				f_Norm;\
	        int					b_visible;\
	        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/\
        \
	        pul_CurRLI = pul_RLI;\
	        pul_CurVC = _pst_Node->pst_Base->pst_Visu->dul_VertexColors + 1;\
	        pst_Point = LIGHT_gpst_CurObject->dst_Point;\
	        pst_Last = pst_Point + LIGHT_gpst_CurObject->l_NbPoints;\
	        pst_World = WOR_World_GetWorldOfObject(_pst_Node);\
	        pst_LightPos = OBJ_pst_GetAbsolutePosition(*ppst_LightNode);\
	        OBJ_MakeAbsoluteMatrix(_pst_Node, &M);\
        \
	        for(; pst_Point < pst_Last; pst_Point++, pul_CurRLI++, pul_CurVC++)\
	        {\
		        if(*pul_CurRLI == 0) continue;\
        \
		        if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_RLICastRay)\
		        {\
			        MATH_TransformVertex(&st_Point, &M, VCast(pst_Point));\
			        MATH_SubVector(&st_Dir, &st_Point, pst_LightPos);\
			        f_Norm = MATH_f_NormVector(&st_Dir);\
			        st_Dir.x /= f_Norm;\
			        st_Dir.y /= f_Norm;\
			        st_Dir.z /= f_Norm;\
			        f_Norm -= 0.01f;\
			        COL_Visual_RayCast\
			        (\
				        pst_World,\
				        (*ppst_LightNode),\
				        pst_LightPos,\
				        &st_Dir,\
				        f_Norm,\
				        0xFFFFFFFF,\
				        0,\
				        OBJ_C_OR_OR_IdentityFlags,\
				        &b_visible,\
				        TRUE,\
						FALSE\
			        );\
			        if (!b_visible) continue;\
		        }\
        \
                LIGHT_AddColor(pul_CurVC, *pul_CurRLI & LIGHT_gul_ComputeRLIMask);\
	        }\
        }\
\
		l_Size = sizeof(ULONG) * LIGHT_gpst_CurObject->l_NbPoints;\
		L_memset(pul_RLI, 0, l_Size);\
	}

/* computing RLI end */
#define M_4Edit_ComputeRLIEnd()	if(LIGHT_gl_ComputeRLI && pul_RLI) L_free(pul_RLI);

/* is object lighted mode */
#define M_4Edit_ListOfLightOnObject()\
    if (_pst_Node == GDI_gpst_CurDD_SPR.pst_GetLightForGO)\
    {\
        LINK_PrintStatusMsg( (*ppst_LightNode)->sz_Name );\
    }

#else

#define M_4Edit_ComputeRLIStart()
#define M_4Edit_ComputeRLIAdd()
#define M_4Edit_ComputeRLIEnd()
#define M_4Edit_ListOfLightOnObject()

#endif /*ACTIVE_EDITORS*/

/*~1- macros pour la PS2 */
#ifdef GSP_PS2
extern unsigned int NoLIGH;
#endif

/*~1- macros pour la Gamacuba */
#if defined(_GAMECUBE) && defined(USE_HARDWARE_LIGHTS)

        #define M_TestGameCubeHardwareLights  if ( !g_bUseHardwareLights )

#else

    #define M_TestGameCubeHardwareLights

#endif

//------------------------------------------------
// BuildSpotLightFrustum
//------------------------------------------------
static void 
BuildSpotLightFrustum( LIGHT_tdst_SpotFrustum * _pst_Frustum,
                       LIGHT_tdst_Spot *        _pst_Spot,
                       OBJ_tdst_GameObject *    _pst_LightNode,  
                       MATH_tdst_Matrix *       _pmWorldToObject )
{
    MATH_tdst_Matrix * pMatrix = OBJ_pst_GetAbsoluteMatrix(_pst_LightNode);

    MATH_tdst_Matrix mLightToObject;
    if( _pmWorldToObject != NULL )
    {
        MATH_MulMatrixMatrix( &mLightToObject, OBJ_pst_GetAbsoluteMatrix(_pst_LightNode), _pmWorldToObject );
        pMatrix = &mLightToObject;
    }

    // Prepare the vertices needed to build the frustum planes
    MATH_tdst_Vector vOrigin, vFarCenter, vFarUp, vFarRight;
    MATH_CopyVector( &vOrigin, &pMatrix->T );
    MATH_InitVector( &vFarCenter, 0.0f,                     -_pst_Spot->f_Far,  0.0f );
#ifdef JADEFUSION
    MATH_InitVector( &vFarUp,     0.0f,                     -_pst_Spot->f_Far,  _pst_Spot->f_FarRadius );
    MATH_InitVector( &vFarRight,  _pst_Spot->f_FarRadius,   -_pst_Spot->f_Far,  0.0f );
#endif

    // Transform light space vertices
    MATH_TransformVertex( &vFarCenter, pMatrix, &vFarCenter );
    MATH_TransformVertex( &vFarUp,     pMatrix, &vFarUp );
    MATH_TransformVertex( &vFarRight,  pMatrix, &vFarRight );

    // Compute far plane frustum vectors
    MATH_SubEqualVector( &vFarUp, &vFarCenter );
    MATH_SubEqualVector( &vFarRight, &vFarCenter );

    // Compute the frustum edges 
    MATH_tdst_Vector vEdge[4];
    MATH_tdst_Vector vTemp;
    MATH_SubVector( &vTemp, &vFarCenter, &vOrigin );

    MATH_SubVector( &vEdge[0], &vTemp, &vFarUp );
    MATH_AddEqualVector( &vEdge[0], &vFarRight );

    MATH_AddVector( &vEdge[1], &vTemp, &vFarUp );
    MATH_AddEqualVector( &vEdge[1], &vFarRight );

    MATH_AddVector( &vEdge[2], &vTemp, &vFarUp );
    MATH_SubEqualVector( &vEdge[2], &vFarRight );

    MATH_SubVector( &vEdge[3], &vTemp, &vFarUp );
    MATH_SubEqualVector( &vEdge[3], &vFarRight );

    // Now build the 4 planes
    for( ULONG p=0; p<4; ++p )
    {
        MATH_CrossProduct( &_pst_Frustum->m_normal[p], &vEdge[(p+1)%4], &vEdge[p] );
        MATH_NormalizeEqualAnyVector( &_pst_Frustum->m_normal[p] );
        _pst_Frustum->m_distance[p] = MATH_f_DotProduct( &_pst_Frustum->m_normal[p], &vOrigin );
    }
}

static bool IsCulledByFrustum( LIGHT_tdst_SpotFrustum *    _pst_Frustum, 
                        MATH_tdst_Vector *          _pvPoints,
                        ULONG                       _ulNbrPoints )
{
    bool isCulled = false;
    for( ULONG p=0; p<4; ++p )
    {
        bool isCulledByPlane = true;
        for( ULONG b=0; b<_ulNbrPoints; ++b )
        {
            float fDist = MATH_f_DotProduct( &_pst_Frustum->m_normal[p], &_pvPoints[b] );
            if( fDist < _pst_Frustum->m_distance[p] )
            {
                isCulledByPlane = false;
                break;
            }
        }
        if( isCulledByPlane )
        {
            isCulled = true;
            break;
        }
    }

    return isCulled;
}

/*
 =======================================================================================================================
  L'objet est il éclairé ??
 =======================================================================================================================
 */
#define LIGHT_ObjSkinFlag   (OBJ_C_IdentityFlag_Bone | OBJ_C_IdentityFlag_Hierarchy | OBJ_C_IdentityFlag_AdditionalMatrix)

static LONG LIGHT_l_IsNodeLighted(OBJ_tdst_GameObject *_pst_Node, OBJ_tdst_GameObject *_pst_LightNode, LIGHT_tdst_Light *_pst_Light, BOOL _b_TestBV )
{
    OBJ_tdst_GameObject *pst_Father;
    MATH_tdst_Vector    v, *pv;

#ifdef ACTIVE_EDITORS
	if(LIGHT_gl_ComputeRLI)
	{
        if ( !(_pst_Node->ul_StatusAndControlFlags & OBJ_C_StatusFlag_RTL) )
        {
            if (! (_pst_Light->ul_Flags & LIGHT_Cul_LF_RLIOnNonDynam) )
                return FALSE;
        }
        else
        {
            if ( !(_pst_Light->ul_Flags & LIGHT_Cul_LF_RLIOnDynam) )
                return FALSE;
        }
	}
    else
    {

#endif

    if ( !(_pst_Node->ul_StatusAndControlFlags & OBJ_C_StatusFlag_RTL) )
    {
        if ( !(_pst_Light->ul_Flags & LIGHT_Cul_LF_RealTimeOnNonDynam) )
            return FALSE;
    }
    else
    {
        if ( !(_pst_Light->ul_Flags & LIGHT_Cul_LF_RealTimeOnDynam) )
            return FALSE;
    }

#ifdef ACTIVE_EDITORS
	}
#endif


    if (_pst_Light->pst_GO)
    {
        if ( _pst_Light->pst_GO != ANI_pst_GetReferenceInit(_pst_Node) )
            return FALSE;   
    }
#ifdef JADEFUSION
		ULONG ulType;
		ulType  =	_pst_Light->ul_Flags & LIGHT_Cul_LF_Type;

    if (_b_TestBV && (_pst_Node->pst_BV) && (ulType != LIGHT_Cul_LF_Direct) )
    {
        if( ulType == LIGHT_Cul_LF_Omni )
#else
    if (_b_TestBV && (_pst_Node->pst_BV) )
    {
		ULONG ulType;
		ulType  =	_pst_Light->ul_Flags & LIGHT_Cul_LF_Type;
        if ((ulType == LIGHT_Cul_LF_Omni) || (ulType == LIGHT_Cul_LF_Spot))
#endif
		{
			if (   
	                ((_pst_Node->ul_IdentityFlags & (LIGHT_ObjSkinFlag|OBJ_C_IdentityFlag_Hierarchy) ) == (LIGHT_ObjSkinFlag|OBJ_C_IdentityFlag_Hierarchy)) && 
                    (_pst_Node->pst_Base->pst_AddMatrix) &&
					(_pst_Node->pst_Base->pst_AddMatrix->l_Number > 1) && 
                    ( _pst_Node->pst_Base->pst_Hierarchy) &&
					(pst_Father = _pst_Node->pst_Base->pst_Hierarchy->pst_Father) 
	           )
			{
	            pv = &v;
	            MATH_TransformVertex( pv, OBJ_pst_GetAbsoluteMatrix(pst_Father), &_pst_Node->pst_Base->pst_Hierarchy->st_LocalMatrix.T );
			}
	        else
	            pv = OBJ_pst_GetAbsolutePosition(_pst_Node );
		    MATH_SubVector( &v, OBJ_pst_GetAbsolutePosition(_pst_LightNode), pv );
            if (!INT_SphereAABBox(&v, _pst_Light->st_Omni.f_Far, OBJ_pst_BV_GetGMin(_pst_Node->pst_BV ), OBJ_pst_BV_GetGMax(_pst_Node->pst_BV ) ))
        		return FALSE;
        }
#ifdef JADEFUSION
        else if( ulType == LIGHT_Cul_LF_Spot )
        {
            bool isCylindrical = (_pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical) != 0;
            bool isCullingGaoSpotFrustum = (GDI_gpst_CurDD->ul_LightCullingFlags & GDI_Cul_Light_Cull_Frustum_Gao) != 0;

            // Object local bounding volume (in world coords)
            MATH_tdst_Vector vGlobalAABVMin, vGlobalAABVMax, vGlobalAABVCenter;

            vGlobalAABVMin = *OBJ_pst_BV_GetGMin(_pst_Node->pst_BV );
            vGlobalAABVMax = *OBJ_pst_BV_GetGMax(_pst_Node->pst_BV );
            MATH_AddVector(&vGlobalAABVMax, OBJ_pst_BV_GetGMax(_pst_Node->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_Node));
            MATH_AddVector(&vGlobalAABVMin, OBJ_pst_BV_GetGMin(_pst_Node->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_Node));

            MATH_SubVector(&vGlobalAABVCenter, &vGlobalAABVMax, &vGlobalAABVMin);
            MATH_MulEqualVector(&vGlobalAABVCenter, 0.5f);
            MATH_AddEqualVector(&vGlobalAABVCenter, &vGlobalAABVMin);
 
            //
            // Prepare bounding volume in light space
            //
            LIGHT_tdst_Spot * pSpot = & _pst_Light->st_Spot;
            if( !pSpot->b_IsBVValid )
            {
                // Compute light bounding volume
                pSpot->f_FarRadius = pSpot->f_Far * tan( pSpot->f_BigAlpha );
                MATH_InitVector( &pSpot->st_LocalBVMax, pSpot->f_FarRadius, 0.0f, pSpot->f_FarRadius );
                MATH_InitVector( &pSpot->st_LocalBVMin, -pSpot->f_FarRadius, -pSpot->f_Far, -pSpot->f_FarRadius );

                // Compute light bounding volume center
                MATH_SubVector( &pSpot->st_GlobalBVCenter, &pSpot->st_LocalBVMax, &pSpot->st_LocalBVMin );
                MATH_MulEqualVector( &pSpot->st_GlobalBVCenter, 0.5f );
                MATH_AddEqualVector( &pSpot->st_GlobalBVCenter, &pSpot->st_LocalBVMin );
                MATH_TransformVertexNoScale( &pSpot->st_GlobalBVCenter, OBJ_pst_GetAbsoluteMatrix(_pst_LightNode), &pSpot->st_GlobalBVCenter );

                // Compute light inverse matrix
                MATH_InvertMatrix( &pSpot->st_Inverse, OBJ_pst_GetAbsoluteMatrix(_pst_LightNode) );
                MATH_ClearScale( &pSpot->st_Inverse, 1 );

                if( !isCylindrical && isCullingGaoSpotFrustum )
                {
                    BuildSpotLightFrustum( &pSpot->st_FrustumInWorld, pSpot, _pst_LightNode, NULL );
    }

                // Data will be reused as needed within this frame 
                pSpot->b_IsBVValid = TRUE;
            }
          
            // Distance between both centers in world space converted to local light system
            MATH_tdst_Vector st_Distance;
            MATH_SubVector(&st_Distance, &vGlobalAABVCenter, &pSpot->st_GlobalBVCenter );
            MATH_TransformVectorNoScale( &st_Distance, &pSpot->st_Inverse, &st_Distance );
           
            MATH_tdst_Matrix st_TransMatrix;
            MATH_CopyMatrix( &st_TransMatrix, &pSpot->st_Inverse );
            MATH_CopyVector( MATH_pst_GetTranslation(&st_TransMatrix), &st_Distance);
  
            //
            // Check to see if light bounding volume intersects the mesh
            //
            BOOL bIntersect =  INT_HighLevel_OBBoxOBBox( &pSpot->st_LocalBVMin, 
                                                         &pSpot->st_LocalBVMax, 
                                                         &vGlobalAABVMin,
                                                         &vGlobalAABVMax,
                                                         &st_TransMatrix,
                                                         NULL );

            // Check non-cylindrical spot light with side planes
            if( bIntersect && !isCylindrical && isCullingGaoSpotFrustum )
            {
                // Now we can do the frustum culling
                MATH_tdst_Vector vAABBox[8] = 
                {
                    { vGlobalAABVMin.x, vGlobalAABVMin.y, vGlobalAABVMin.z },
                    { vGlobalAABVMin.x, vGlobalAABVMin.y, vGlobalAABVMax.z },
                    { vGlobalAABVMin.x, vGlobalAABVMax.y, vGlobalAABVMin.z },
                    { vGlobalAABVMin.x, vGlobalAABVMax.y, vGlobalAABVMax.z },
                    { vGlobalAABVMax.x, vGlobalAABVMin.y, vGlobalAABVMin.z },
                    { vGlobalAABVMax.x, vGlobalAABVMin.y, vGlobalAABVMax.z },
                    { vGlobalAABVMax.x, vGlobalAABVMax.y, vGlobalAABVMin.z },
                    { vGlobalAABVMax.x, vGlobalAABVMax.y, vGlobalAABVMax.z }
                };

                bIntersect = !IsCulledByFrustum( &pSpot->st_FrustumInWorld, vAABBox, 8 );
            }
     
            return bIntersect;
        }
#endif
	}

    return TRUE;
}

/*
 =======================================================================================================================
 inversion des normales en cas de symétrie
 =======================================================================================================================
 */
static void LIGHT_Invert(MATH_tdst_Vector *pst_Normal,ULONG Number,ULONG InvertSym,ULONG InvertBF)
{
	MATH_tdst_Vector *pst_LastNormal;
	pst_LastNormal = pst_Normal + Number;
	if (InvertSym && InvertBF)
	{
		while (pst_Normal < pst_LastNormal)
		{
			*(LONG *) &pst_Normal ->y ^= 0x80000000;
			*(LONG *) &pst_Normal ->z ^= 0x80000000;
			pst_Normal++;
		}
			
	} 
    else if (InvertSym)
	{
		while (pst_Normal < pst_LastNormal)
		{
			*(LONG *) &pst_Normal ->x ^= 0x80000000;
			pst_Normal++;
		}
	} 
    else if  (InvertBF)
	{
		while (pst_Normal < pst_LastNormal)
		{
			*(LONG *) &pst_Normal ->x ^= 0x80000000;
			*(LONG *) &pst_Normal ->y ^= 0x80000000;
			*(LONG *) &pst_Normal ->z ^= 0x80000000;
			pst_Normal++;
		}
	} 	
}

/*
 =======================================================================================================================
 inversion des normales compressées en cas de symétrie
 =======================================================================================================================
 */
static void LIGHT_Invert_C(unsigned int  *pst_Normal,ULONG Number,ULONG InvertSym,ULONG InvertBF)
{
	unsigned int *pst_LastNormal;
	unsigned int XorFlag;
	
    XorFlag = 0;
	if  (InvertSym && InvertBF)
		XorFlag = 0xffff00; // YZ
    else if  (InvertSym)
		XorFlag = 0x0000ff; // X
    else if  (InvertBF)
		XorFlag = 0xffffff; // XYZ

    if (XorFlag)
	{
		pst_LastNormal = pst_Normal + Number;
		while (pst_Normal < pst_LastNormal)
		{
			*(pst_Normal++) ^= XorFlag;
		}
	}
	
}
/*
 =======================================================================================================================
  fct principale
 =======================================================================================================================
 */
OBJ_tdst_GameObject    *LIGHT_gpst_Bump;

#ifdef _XBOX
float LIGHT_ComputeExclusiveLightFactor(LIGHT_tdst_List *_pst_LightList, OBJ_tdst_GameObject *_pst_Node) ;
void LIGHT_SendObjectToLightXB( LIGHT_tdst_List *_pst_LightList, OBJ_tdst_GameObject *_pst_Node )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			**ppst_LightNode, **ppst_Last;
	MATH_tdst_Vector			*pst_Normal, V, W;
	LONG						l_FirstComputing;
	ULONG						*pul_Diffuse;
	SOFT_tdst_ComputingBuffers	*pst_CB;
   	int                         i_Mat;
   	BOOL                        b_BVRejection;
	ULONG						ulLIGHT_XOR;
	ULONG						ulLIGHT_NORMAL_XSYM_XOR;
	BOOL						b_AttenuateLight;
	float						f_AttenuateCoeff;

#ifdef ACTIVE_EDITORS
	LONG						l_Size;
	ULONG						*pul_RLI;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



    /* pas de lumière ? */
    LIGHT_gpst_Bump = NULL;
    if ( _pst_LightList->ul_Current == 0 ) return;


    PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_All);
	_GSP_BeginRaster(1);
		
	{
		/* symétrie ? */
        ulLIGHT_XOR = (GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF) ? 0 : 1;
		ulLIGHT_NORMAL_XSYM_XOR  = 0;

		if (!(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_Symetric))
		{
			ulLIGHT_NORMAL_XSYM_XOR  = 1;
			ulLIGHT_XOR ^= 1;
		}
	}
	
    /* init des variables */
    l_FirstComputing = 1;
	pst_CB = GDI_gpst_CurDD_SPR.pst_ComputingBuffers;
	pst_CB->ul_Ambient = 0;
	LIGHT_gpst_CurObject = ((GEO_tdst_Object *) _pst_Node->pst_Base->pst_Visu->pst_Object);
	LIGHT_gpst_CurPointsBuffer = GDI_gpst_CurDD_SPR.p_Current_Vertex_List;
    pul_Diffuse = pst_CB->aul_Diffuse;
	GEO_UseNormals(LIGHT_gpst_CurObject);
    pst_Normal = LIGHT_gpst_CurObject->dst_PointNormal;
    if(!pst_Normal) return;
    b_BVRejection = GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_UseBVForLightRejection;

	if ( b_AttenuateLight = !(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_DontAttenuateLight) )
	{
		f_AttenuateCoeff = ((float) 127 - _pst_Node->pst_Base->pst_Visu->c_LightAttenuation ) / 127.0f;
		f_AttenuateCoeff *= (1.0f - GDI_gpst_CurDD_SPR.f_LightAttenuation);
	}
    
    /* editor : start computing RLI */	
    M_4Edit_ComputeRLIStart();
    
    /* loop sur la liste des lumières */
    ppst_LightNode = _pst_LightList->dpst_Light;
	ppst_Last = ppst_LightNode + _pst_LightList->ul_Current;
	while(ppst_LightNode < ppst_Last)
	{
	    LIGHT_gpst_Cur = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;
		
		LIGHT_ConvertLightColor(LIGHT_gpst_Cur);

        /* l'objet est il éclairé ? */
	    if (!LIGHT_l_IsNodeLighted(_pst_Node, *ppst_LightNode, LIGHT_gpst_Cur, b_BVRejection)) 
	    {
	        ppst_LightNode++;
	        continue;
        }

        if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Bump)
            LIGHT_gpst_Bump = *ppst_LightNode;

        /* editor : liste des lumières qui éclaire un objet donné */
        M_4Edit_ListOfLightOnObject()

        if ( (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Type) != LIGHT_Cul_LF_AddMaterial )
        {
            /* Move light into node system axis */
		    if(l_FirstComputing)
		    {
			    l_FirstComputing = 0;
			    MATH_InvertMatrix(&LIGHT_gst_NIM, _pst_Node->pst_GlobalMatrix);

                /* calcul le déplacement du centre de la BV dans le repère de l'objet (necessaire pour les omni const) */
                if ( _pst_Node->pst_BV )
                {
                    MATH_AddVector( &V, OBJ_pst_BV_GetGMin(_pst_Node->pst_BV), OBJ_pst_BV_GetGMax(_pst_Node->pst_BV));
		            MATH_MulEqualVector(&V, 0.5f);
                    MATH_TransformVector( &W, &LIGHT_gst_NIM, &V ); 
                }
                else
                {
                    MATH_InitVectorToZero( &W );
                }
			    
    	        LIGHT_Invert(pst_Normal,LIGHT_gpst_CurObject->l_NbPoints,ulLIGHT_NORMAL_XSYM_XOR,ulLIGHT_XOR);
		    }

            /* calcul la matrice de la lumière dans le repère de l'objet */
            MATH_MulMatrixMatrix(&LIGHT_gst_LP, (*ppst_LightNode)->pst_GlobalMatrix, &LIGHT_gst_NIM);
            /* si la lumière est une omni const le centre du repère de l'objet est déplacé sur le centre de la BV */
            if ( (LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_Type | LIGHT_Cul_LF_OmniConst)) == (LIGHT_Cul_LF_Omni | LIGHT_Cul_LF_OmniConst) ) 
                MATH_SubEqualVector( &LIGHT_gst_LP.T, &W );

            /* couleur de la lumière */
		    LIGHT_gul_Color = LIGHT_gpst_Cur->ul_Color;
		    if(!(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_UseColor)) LIGHT_gul_Color &= 0xFF000000;
		    if(!(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_UseAlpha)) LIGHT_gul_Color &= 0x00FFFFFF;
			if (b_AttenuateLight )
			{
				LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
				LIGHT_gul_Color =  LIGHT_ul_MulColor( LIGHT_gul_Color, f_AttenuateCoeff );
			}
		    
            /* initialisation des couleurs spéculaires */
            M_TestGameCubeHardwareLights
			{		    
			    if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Specular)
			    {
				    if (!(GDI_gpst_CurDD->pst_ComputingBuffers->ul_Flags & SOFT_Cul_CB_SpecularColorField))
					    SOFT_pst_InitSpecularColors(LIGHT_gpst_CurObject->l_NbPoints);
			    }
			}
			
            /* calcul de la lumière selon son type */
		    switch(LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_Type | LIGHT_Cul_LF_Specular))
		    {
		    case LIGHT_Cul_LF_Omni | LIGHT_Cul_LF_Specular:
				{

			    /* Specular */
			    LIGHT_gpul_CurrentTable = LIGHT_gaul_ExponantTable;
			    LIGHT_gpst_Cur->st_Omni.f_Far *= LIGHT_C_fSpecularExp;
			    LIGHT_gpst_Cur->st_Omni.f_Near *= 0.25f;
			    LIGHT_SendObjectToSpherical(pst_CB->aul_Specular, SOFT_pst_GetSpecularVectors(),NULL);
			    LIGHT_gpst_Cur->st_Omni.f_Far *= LIGHT_C_OO_fSpecularExp;
			    LIGHT_gpst_Cur->st_Omni.f_Near *= 4.0f;

				LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
				LIGHT_SendObjectToSpherical(pul_Diffuse, pst_Normal,NULL);
				break;
		
				}

		    case LIGHT_Cul_LF_Omni:
			{
	
				ppst_LightNode++;
				continue;
			}
	

    		    LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
			    LIGHT_SendObjectToSpherical(pul_Diffuse, pst_Normal,NULL);
			    
				break;			    

		    case LIGHT_Cul_LF_Direct | LIGHT_Cul_LF_Specular:
				{

				//ppst_LightNode++;
				//continue;


		        /* Specular */
		        LIGHT_gpul_CurrentTable = LIGHT_gaul_ExponantTable;
		        LIGHT_SendObjectToDirect(pst_CB->aul_Specular, SOFT_pst_GetSpecularVectors(),NULL);

				LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
				LIGHT_SendObjectToDirect(pul_Diffuse, pst_Normal,NULL);

				break;
		        
				}



		    case LIGHT_Cul_LF_Direct:


				ppst_LightNode++;
				continue;

			    LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
			    
			    LIGHT_SendObjectToDirect(pul_Diffuse, pst_Normal,NULL);
			    break;
			    
		    case LIGHT_Cul_LF_Spot | LIGHT_Cul_LF_Specular:
				{

				//ppst_LightNode++;
				//continue;
		        
				/* Specular */
		        LIGHT_gpul_CurrentTable = LIGHT_gaul_ExponantTable;
		        LIGHT_gpst_Cur->st_Spot.f_Far *= LIGHT_C_fSpecularExp;
		        LIGHT_SendObjectToSpot(pst_CB->aul_Specular, SOFT_pst_GetSpecularVectors());
		        LIGHT_gpst_Cur->st_Spot.f_Far *= LIGHT_C_OO_fSpecularExp;

				LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
			    LIGHT_SendObjectToSpot(pul_Diffuse, pst_Normal);

				break;

				}



		    case LIGHT_Cul_LF_Spot:
				{

				ppst_LightNode++;
				continue;

			    LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
			    LIGHT_SendObjectToSpot(pul_Diffuse, pst_Normal);
	       	    break;
				}
            }

            M_4Edit_ComputeRLIAdd()
        }
        else
        {
	        if (GDI_gpst_CurDD_SPR.pst_AdditionalMaterial == NULL)
	        {
	            if(((*ppst_LightNode)->pst_Base) && ((*ppst_LightNode)->pst_Base->pst_Visu) && ((*ppst_LightNode)->pst_Base->pst_Visu->pst_Material) )
	            {
	                if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_AddMaterialBVMustBeInside)
	                    i_Mat = COL_b_BVIn( (*ppst_LightNode), _pst_Node );
	                else
	                    i_Mat = COL_b_BVOverlap( _pst_Node, (*ppst_LightNode), TRUE, TRUE, FALSE);
	                
	                if ( i_Mat )
	                {
	                    GDI_gpst_CurDD_SPR.pst_AdditionalMaterial = (*ppst_LightNode)->pst_Base->pst_Visu->pst_Material;
	                    if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_AddMaterialDontUseNormalMaterial)
                            GDI_gpst_CurDD_SPR.ul_CurrentDrawMask &= ~GDI_Cul_DM_UseNormalMaterial;
	                    if (GDI_gpst_CurDD_SPR.pst_AdditionalMaterial->i->ul_Type == GRO_MaterialMulti)
	                    {
	                        i_Mat = (int) LIGHT_gpst_Cur->st_AddMaterial.w_Id;
	                        i_Mat %= ((MAT_tdst_Multi *) GDI_gpst_CurDD_SPR.pst_AdditionalMaterial)->l_NumberOfSubMaterials;
	                        GDI_gpst_CurDD_SPR.pst_AdditionalMaterial = (GRO_tdst_Struct *) ((MAT_tdst_Multi *) GDI_gpst_CurDD_SPR.pst_AdditionalMaterial)->dpst_SubMaterial[ i_Mat ];
                        }
                    }
                }
            }
        }

		ppst_LightNode++;
	}

    M_TestGameCubeHardwareLights
	{
	    /* Restore eventually initial Normals */
	    if (!l_FirstComputing)
	    	LIGHT_Invert(pst_Normal,LIGHT_gpst_CurObject->l_NbPoints,ulLIGHT_NORMAL_XSYM_XOR,ulLIGHT_XOR);
	}

    M_4Edit_ComputeRLIEnd()
	
	_GSP_EndRaster(1);
    PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_All);
}
#elif defined(_XENON_RENDER)

#define MAX_POTENTIAL_LIGHTS 128

#ifdef ACTIVE_EDITORS
#define M_4Edit_If_Light_Logging( statement ) if( g_oXeLightUsageLogger.IsLogging() ) { statement ; }
#define M_4Edit_If_Light_Logging_Cond( cond, statement ) if( g_oXeLightUsageLogger.IsLogging() && (cond) ) { statement ; }
#else
#define M_4Edit_If_Light_Logging( statement )
#define M_4Edit_If_Light_Logging_Cond( cond, statement ) 
#endif

//----------------------------------
// STRUCT : PotentialLight
//----------------------------------
struct PotentialLight
{
    OBJ_tdst_GameObject *   m_pLightGameObject;
    LIGHT_tdst_Light *      m_pLight;
    float                   m_fInfluenceFactor;
    bool                    m_isCulled;
    float                   m_fLuminance;
    ULONG                   m_ulLightType;
    ULONG                   m_ulLightIndex;
    MATH_tdst_Vector        m_vLightCenterInObjectSpace;
    LIGHT_tdst_SpotFrustum  m_spotLightFrustum;
    BOOL                    m_bIsFrustumDirty;
};

PotentialLight  g_potentialLights[ MAX_POTENTIAL_LIGHTS ];

//----------------------------------
// STRUCT : LessByInfluence
//----------------------------------
struct LessByInfluence
{
    bool operator()( const PotentialLight & _lhs, const PotentialLight & _rhs )
    {
        // In a sorted container, a light with higher influence factor comes first
        return _lhs.m_fInfluenceFactor > _rhs.m_fInfluenceFactor;
    }
};

//------------------------------------------------
// InitElementsForLighting
//------------------------------------------------
ULONG 
InitElementsForLighting( OBJ_tdst_GameObject * _pst_Node, BOOL _isLightingPerElement, ULONG _ulRequestedMaxLight)
{
    GRO_tdst_XeElement* pElements = NULL;
    int iNbElementsMeshes = 0;
    int iMaxNbrBones      = 0;

    if( (_pst_Node->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu) != 0 &&
        (_pst_Node->pst_Base != NULL ) &&
        (_pst_Node->pst_Base->pst_Visu != NULL ) )
    {
        iNbElementsMeshes = _pst_Node->pst_Base->pst_Visu->l_NbXeElements;
        pElements         = _pst_Node->pst_Base->pst_Visu->p_XeElements;
    }

    for( int i=0; i<iNbElementsMeshes; ++i )
    {
        ERR_X_Assert( pElements != NULL );
        XeMesh * pMesh = pElements[i].pst_Mesh;

        if( pMesh != NULL )
        {
            pMesh->ClearLightSet();
            pMesh->UseLightSet( _isLightingPerElement && pMesh->HasBoundingVolume() );
            INT iNbrOfBones = pMesh->GetMaxBones();
            iMaxNbrBones = max( iNbrOfBones, iMaxNbrBones );
        }
    }
    
    return min(g_oVertexShaderMgr.GetMaxHWLights( iMaxNbrBones ), _ulRequestedMaxLight);
}

//------------------------------------------------
// ClearElementsForLighting
//------------------------------------------------
void 
ClearElementsForLighting( OBJ_tdst_GameObject * _pst_Node )
{
    GRO_tdst_XeElement* pElements = NULL;
    int iNbElementsMeshes = 0;

    if( (_pst_Node->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu) != 0 &&
        (_pst_Node->pst_Base != NULL ) &&
        (_pst_Node->pst_Base->pst_Visu != NULL ) )
    {
        iNbElementsMeshes = _pst_Node->pst_Base->pst_Visu->l_NbXeElements;
        pElements         = _pst_Node->pst_Base->pst_Visu->p_XeElements;
    }

    for( int i=0; i<iNbElementsMeshes; ++i )
    {
        ERR_X_Assert( pElements != NULL );
        XeMesh * pMesh = pElements[i].pst_Mesh;

        if( pMesh != NULL )
        {
            pMesh->ClearLightSet();
            pMesh->UseLightSet( false );
        }
    }
}

//------------------------------------------------
// BuildPotentialLightsList
//------------------------------------------------
ULONG 
BuildPotentialLightsList( PotentialLight*           _potentialLights, 
                          LIGHT_tdst_List *         _pst_LightList,
                          OBJ_tdst_GameObject *     _pst_Node, 
                          BOOL                      _b_BVRejection )
{
	OBJ_tdst_GameObject **ppst_LightNode = NULL;
    OBJ_tdst_GameObject **ppst_Last = NULL;
    ULONG ulNbrPotentialLights = 0;
    int i_Mat = 0;

    ERR_X_Assert( _pst_Node->pst_Base );
    GRO_tdst_Visu * pVisu = _pst_Node->pst_Base->pst_Visu;
    ERR_X_Assert( pVisu );

	// If GDI_Cul_DM_UseMapLightSettings is not set then GAO is an actor
	BOOL bGAOIsAnActor = ! (pVisu->ul_DrawMask & GDI_Cul_DM_UseMapLightSettings);

	RejectedLights * pRejectedLights = pVisu->pRejectedLights;
	
	RejectedLights::iterator itRejected;
	if( pRejectedLights )
	{
		itRejected = pRejectedLights->begin();
	}

	ppst_LightNode = _pst_LightList->dpst_Light;
	ppst_Last = ppst_LightNode + _pst_LightList->ul_Current;
	while( ppst_LightNode < ppst_Last )
	{
		LIGHT_tdst_Light* pLight = ((LIGHT_tdst_Light*)(*ppst_LightNode)->pst_Extended->pst_Light);
		if ( (bGAOIsAnActor && ! pLight->b_LightActor) || (! bGAOIsAnActor && ! pLight->b_LightScene) )
		{
			// Light is rejected
			ppst_LightNode++;
			continue;
		}

		if( pRejectedLights && ! bGAOIsAnActor )
		{
			//
			// Lights are sorted in ascending order and so is the rejection list
			//
			while( (itRejected != pRejectedLights->end()) && 
				(*ppst_LightNode > *itRejected ))
			{
				++itRejected;
			}

			if( (itRejected != pRejectedLights->end()) && 
				(*itRejected == *ppst_LightNode ) )
			{
				// Light is rejected
				ppst_LightNode++;
				continue;
			}
		}

		LIGHT_gpst_Cur = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;

		/* l'objet est il éclairé ? */
		if (!LIGHT_l_IsNodeLighted( _pst_Node, 
									*ppst_LightNode, 
									LIGHT_gpst_Cur, 
									_b_BVRejection ))
		{
			ppst_LightNode++;
			continue;
		}

		if( (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Type) != LIGHT_Cul_LF_AddMaterial )
		{
			// Add light to the potential lights list
			ERR_X_Assert( ulNbrPotentialLights < MAX_POTENTIAL_LIGHTS );
			_potentialLights[ulNbrPotentialLights].m_pLightGameObject = *ppst_LightNode;
			_potentialLights[ulNbrPotentialLights].m_pLight           = LIGHT_gpst_Cur;
	
			ulNbrPotentialLights++;
		}
		else if (GDI_gpst_CurDD_SPR.pst_AdditionalMaterial == NULL )
		{
			if(((*ppst_LightNode)->pst_Base) && ((*ppst_LightNode)->pst_Base->pst_Visu) && ((*ppst_LightNode)->pst_Base->pst_Visu->pst_Material) )
			{
				if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_AddMaterialBVMustBeInside)
					i_Mat = COL_b_BVIn( (*ppst_LightNode), _pst_Node );
				else
					i_Mat = COL_b_BVOverlap( _pst_Node, (*ppst_LightNode), TRUE, TRUE, FALSE);

				if ( i_Mat )
				{
					GDI_gpst_CurDD_SPR.pst_AdditionalMaterial = (*ppst_LightNode)->pst_Base->pst_Visu->pst_Material;
					if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_AddMaterialDontUseNormalMaterial)
						GDI_gpst_CurDD_SPR.ul_CurrentDrawMask &= ~GDI_Cul_DM_UseNormalMaterial;
					if (GDI_gpst_CurDD_SPR.pst_AdditionalMaterial->i->ul_Type == GRO_MaterialMulti)
					{
						i_Mat = (int) LIGHT_gpst_Cur->st_AddMaterial.w_Id;
						i_Mat %= ((MAT_tdst_Multi *) GDI_gpst_CurDD_SPR.pst_AdditionalMaterial)->l_NumberOfSubMaterials;
						GDI_gpst_CurDD_SPR.pst_AdditionalMaterial = (GRO_tdst_Struct *) ((MAT_tdst_Multi *) GDI_gpst_CurDD_SPR.pst_AdditionalMaterial)->dpst_SubMaterial[ i_Mat ];
					}
				}
			}
		}

		++ppst_LightNode;
	}
    return ulNbrPotentialLights;
}

//------------------------------------------------
// ComputeCurrentWorldToObject
//------------------------------------------------
void 
ComputeCurrentWorldToObject( MATH_tdst_Matrix & _mWorldToObject, 
                             MATH_tdst_Matrix & _mObjectToWorld,
                             float &            _fWorldToObjectMaxScaling )
{
    MATH_MulMatrixMatrix( &_mObjectToWorld, 
                          GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix,
                          &GDI_gpst_CurDD->st_Camera.st_Matrix );

    MATH_InvertMatrix( &_mWorldToObject, &_mObjectToWorld );

    // Compute max scaling of the world to object matrix
    MATH_tdst_Vector vScale;
    MATH_GetScale( &vScale, &_mWorldToObject );

    _fWorldToObjectMaxScaling = max( max( vScale.x, vScale.y ), vScale.z );
}

//------------------------------------------------
// SetupPotentialLightsLuminance
//------------------------------------------------
void
SetupPotentialLightsLuminance( PotentialLight * _potentialLights, ULONG _ulNbrPotentialLights )
{
    for( ULONG l=0; l<_ulNbrPotentialLights; ++l )
    {
        PotentialLight * pPotentialLight = &_potentialLights[l];
        
        //
        // Compute the luminance of every potential light
        // It will be used as an influence criteria to reject the least influencial
        //

        ERR_X_Assert( pPotentialLight->m_pLight );

        ULONG ul_Color = pPotentialLight->m_pLight->ul_Color;
        float r = ((float)(ul_Color & 0xFF)) / 255.0f;
        float g = ((float)((ul_Color>>8) & 0xFF)) /255.0f;
        float b = ((float)((ul_Color>>16) & 0xFF)) / 255.0f;

        pPotentialLight->m_fLuminance = (r * 0.3f) + (g * 0.59f) + (b * 0.11f);

        // Init culling flag
        pPotentialLight->m_isCulled = false;
    }
}

//------------------------------------------------
// SortPotentialLights
//------------------------------------------------
void 
SortPotentialLights( PotentialLight *           _potentialLights, 
                     ULONG                      _ulNbrPotentialLights,
                     const MATH_tdst_Matrix &   _mWorldToObject,
                     float                      _fWorldToObjectMaxScaling,
                     const MATH_tdst_Vector &   _vCenterOfBvInObjectSpace,
                     float                      _fObjectRadius )
{
    //
    // Compute the influence factor for every light
    //
    for( ULONG l=0; l<_ulNbrPotentialLights; ++l )
    {
        PotentialLight * pPotentialLight = &_potentialLights[l];

        if( pPotentialLight->m_isCulled )
        {
            pPotentialLight->m_fInfluenceFactor = 0.0f;
        }
        else
        {
            // Compute the distance between light and the center of bounding volume
            float fDistance = 0.0f;
            float fAttenuation = 1.0f;

            if( (pPotentialLight->m_pLight->ul_Flags & LIGHT_Cul_LF_Type) != LIGHT_Cul_LF_Direct )
            {
                fDistance = MATH_f_Distance( &pPotentialLight->m_vLightCenterInObjectSpace, (MATH_tdst_Vector*) &_vCenterOfBvInObjectSpace );
                fDistance -= _fObjectRadius;
            }

            if( fDistance < 0.0f )
            {
                fDistance = 0.0f;
            }

            LIGHT_tdst_Light * cur_light = pPotentialLight->m_pLight;

            switch(cur_light->ul_Flags & LIGHT_Cul_LF_Type)
            {
            case LIGHT_Cul_LF_Spot: 
                {
                    float fScaledNear = cur_light->st_Spot.f_Near * _fWorldToObjectMaxScaling;
                    float fScaledFar = cur_light->st_Spot.f_Far * _fWorldToObjectMaxScaling;

                    if( fDistance > fScaledFar )
                    {
                        fAttenuation = 0.0f;
                    }
                    else if (fScaledNear > fScaledNear)
                    {
                        fAttenuation = (fScaledFar - fDistance) / (fScaledFar - fScaledNear);
                    }
                }
                break;
            case LIGHT_Cul_LF_Omni:
                {
                    float fScaledNear = cur_light->st_Omni.f_Near * _fWorldToObjectMaxScaling;
                    float fScaledFar = cur_light->st_Omni.f_Far * _fWorldToObjectMaxScaling;

                    if( fDistance > fScaledFar )
                    {
                        fAttenuation = 0.0f;
                    }
                    else if( fDistance > fScaledNear )
                    {
                        fAttenuation = (fScaledFar - fDistance) / (fScaledFar - fScaledNear);
                    }
                }
                break;
            };

            pPotentialLight->m_fInfluenceFactor = cur_light->f_DiffuseMultiplier * pPotentialLight->m_fLuminance * fAttenuation;

            if(!(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_UseMapLightSettings))
                pPotentialLight->m_fInfluenceFactor *= cur_light->f_ActorDiffusePonderator;
        }
    }

    // Now sort by influence
    std::sort( _potentialLights, _potentialLights + _ulNbrPotentialLights, LessByInfluence() );
}

//------------------------------------------------
// ComputeLightSetsPerElement
//------------------------------------------------
void
ComputeLightSetsPerElement( OBJ_tdst_GameObject *   _pst_Node, 
                            PotentialLight *        _potentialLights,
                            ULONG                   _ulNbrPotentialLights,
                            MATH_tdst_Matrix &      _mWorldToObject,
                            MATH_tdst_Matrix &      _mObjectToWorld,
                            float                   _fWorldToObjectMaxScaling,
                            ULONG                   _ulMaxRequestedLights)
{
    GRO_tdst_XeElement* pElements = NULL;
    int iNbElementsMeshes = 0;

    if( (_pst_Node->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu) != 0 &&
        (_pst_Node->pst_Base != NULL ) &&
        (_pst_Node->pst_Base->pst_Visu != NULL ) )
    {
        iNbElementsMeshes = _pst_Node->pst_Base->pst_Visu->l_NbXeElements;
        pElements         = _pst_Node->pst_Base->pst_Visu->p_XeElements;
    }

    for( ULONG l=0; l<_ulNbrPotentialLights; ++l )
    {
        PotentialLight * pPotentialLight = &_potentialLights[l];
        pPotentialLight->m_bIsFrustumDirty = TRUE;
    }

    // Now check all elements against the potential lights for bounding volume rejection
    for( int e = 0; e < iNbElementsMeshes; ++e )
    {
        ERR_X_Assert( pElements );
        XeMesh * pMesh = pElements[e].pst_Mesh;

        if( pMesh == NULL || !pMesh->UseLightSet( ) )
        {
            // Element will use default gao light set
            continue;
        }

        ULONG ulMaxLightsForMesh = min(g_oVertexShaderMgr.GetMaxHWLights( pMesh->GetMaxBones() ), _ulMaxRequestedLights);
        ULONG ulNbLightsForMesh = 0;
        ULONG l = 0;
        for( l=0; l<_ulNbrPotentialLights; ++l )
        {
            PotentialLight * pPotentialLight = &_potentialLights[l];

            // Initialize all lights as not culled
            pPotentialLight->m_isCulled = false;

            LIGHT_gpst_Cur = pPotentialLight->m_pLight;
            ERR_X_Assert( LIGHT_gpst_Cur );

            ULONG ulType = LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Type;
            bool needToAddLight = true;

            if( ulType == LIGHT_Cul_LF_Omni )
            {
                //
                // Check to see if light bounding volume intersects the mesh
                //

                float fScaledFar = _fWorldToObjectMaxScaling * LIGHT_gpst_Cur->st_Omni.f_Far;

                MATH_tdst_Vector vAABVMin, vAABVMax;
                pMesh->GetAxisAlignedBoundingVolume( vAABVMin, vAABVMax );

                pPotentialLight->m_isCulled = !INT_SphereAABBox( &pPotentialLight->m_vLightCenterInObjectSpace,
                                                                 fScaledFar, 
                                                                 &vAABVMin,
                                                                 &vAABVMax );
            }
            else if( ulType == LIGHT_Cul_LF_Spot )
            {
                LIGHT_tdst_Spot * pSpot = &LIGHT_gpst_Cur->st_Spot;
                bool isCylindrical = (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical) != 0;
                bool isCullingElementFrustum = (GDI_gpst_CurDD->ul_LightCullingFlags & GDI_Cul_Light_Cull_Frustum_Element) != 0;

                if(!MATH_b_TestScaleType( &_mObjectToWorld ) )
                {
                    MATH_tdst_Vector vAABVMin, vAABVMax, vAABVCenter;
                    pMesh->GetAxisAlignedBoundingVolume( vAABVMin, vAABVMax );
                    MATH_SubVector( &vAABVCenter, &vAABVMax, &vAABVMin);
                    MATH_MulEqualVector( &vAABVCenter, 0.5f);
                    MATH_AddEqualVector( &vAABVCenter, &vAABVMin);

                    MATH_tdst_Vector vCenterInWorld;
                    MATH_TransformVertexNoScale( &vCenterInWorld, &_mObjectToWorld, &vAABVCenter );

                    // Distance between both centers in world space converted to local light system
                    MATH_tdst_Vector st_Distance;
                    MATH_SubVector(&st_Distance, &vCenterInWorld, &pSpot->st_GlobalBVCenter );
                    MATH_TransformVectorNoScale( &st_Distance, &pSpot->st_Inverse, &st_Distance );

                    MATH_tdst_Matrix st_TransMatrix;
                    MATH_SetIdentityMatrix( &st_TransMatrix );
                    MATH_Mul33MatrixMatrix( &st_TransMatrix, &_mObjectToWorld, &pSpot->st_Inverse, false  );
                    MATH_CopyVector( MATH_pst_GetTranslation(&st_TransMatrix), &st_Distance);

                    //
                    // Check to see if light bounding volume intersects the mesh
                    // There is no scaling to deal with because object bounding volume includes it
                    //
                    pPotentialLight->m_isCulled = !INT_HighLevel_OBBoxOBBox( &pSpot->st_LocalBVMin, 
                                                                             &pSpot->st_LocalBVMax, 
                                                                             &vAABVMin,
                                                                             &vAABVMax,
                                                                             &st_TransMatrix,
                                                                             NULL );
                }

                if( !pPotentialLight->m_isCulled && !isCylindrical && isCullingElementFrustum )
                {
                    MATH_tdst_Vector vAABVMin, vAABVMax;
                    pMesh->GetAxisAlignedBoundingVolume( vAABVMin, vAABVMax );

                    // Now we can do the frustum culling
                    MATH_tdst_Vector vAABBox[8] = 
                    {
                        { vAABVMin.x, vAABVMin.y, vAABVMin.z },
                        { vAABVMin.x, vAABVMin.y, vAABVMax.z },
                        { vAABVMin.x, vAABVMax.y, vAABVMin.z },
                        { vAABVMin.x, vAABVMax.y, vAABVMax.z },
                        { vAABVMax.x, vAABVMin.y, vAABVMin.z },
                        { vAABVMax.x, vAABVMin.y, vAABVMax.z },
                        { vAABVMax.x, vAABVMax.y, vAABVMin.z },
                        { vAABVMax.x, vAABVMax.y, vAABVMax.z }
                    };

                    if( pPotentialLight->m_bIsFrustumDirty )
                    {
                        BuildSpotLightFrustum( &pPotentialLight->m_spotLightFrustum, 
                                               pSpot,
                                               pPotentialLight->m_pLightGameObject, 
                                               &_mWorldToObject );
                         pPotentialLight->m_bIsFrustumDirty = FALSE;
                    }
                    pPotentialLight->m_isCulled = IsCulledByFrustum( &pPotentialLight->m_spotLightFrustum, vAABBox, 8 );
                }
            }

            // if this light do not affect actors and we are currently lighting an actor
            if(!(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_UseMapLightSettings))
            {
                if(pPotentialLight->m_pLight->f_ActorDiffusePonderator < 0.001f && pPotentialLight->m_pLight->f_ActorSpecularPonderator < 0.001f)
                    pPotentialLight->m_isCulled = true;
            }

            if( !pPotentialLight->m_isCulled )
            {
                ++ulNbLightsForMesh;
            }
        }
     
        // Keep only most influencial lights
        if( ulNbLightsForMesh > ulMaxLightsForMesh )
        {
			//set flag for the secto warning
#ifdef	ACTIVE_EDITORS
				_pst_Node->ul_AdditionalFlags |= OBJ_C_EdAddFlags_TooManyLights;
#endif	//ACTIVE_EDITORS

            // Sort by descending order of influence
            SortPotentialLights( _potentialLights, 
                                 _ulNbrPotentialLights,
                                 _mWorldToObject,
                                 _fWorldToObjectMaxScaling,
                                 pMesh->GetBoundingVolumeCenter(),
                                 pMesh->GetBoundingVolumeRadius() );

            ulNbLightsForMesh = ulMaxLightsForMesh;
        }

        //
        // Now add every light to the mesh light set
        //

        M_4Edit_If_Light_Logging_Cond( ulNbLightsForMesh > 0, g_oXeLightUsageLogger.BeginElement( e ) );

        l = 0;
        ULONG ulLightsLeftToAdd = ulNbLightsForMesh;
        while( ulLightsLeftToAdd > 0 )
        {
            ERR_X_Assert( l < _ulNbrPotentialLights );

            while( _potentialLights[l].m_isCulled ) 
            {
                l++;
            }

            pMesh->AddLight( _potentialLights[l].m_ulLightType, _potentialLights[l].m_ulLightIndex );
            M_4Edit_If_Light_Logging( g_oXeLightUsageLogger.AddLight( _potentialLights[l].m_pLightGameObject->sz_Name ) );

            --ulLightsLeftToAdd;
            l++;
        }
        M_4Edit_If_Light_Logging_Cond( ulNbLightsForMesh > 0, g_oXeLightUsageLogger.EndElement( ) );
    } 
}

void LIGHT_SendObjectToLightXenon( LIGHT_tdst_List *_pst_LightList, OBJ_tdst_GameObject *_pst_Node, ULONG _ulMaxRequestedLights )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector			W;
    MATH_tdst_Vector            vCenterOfGaoBvInObjectSpace;
    BOOL                        b_BVRejection;
	ULONG						ulLIGHT_XOR;
	ULONG						ulLIGHT_NORMAL_XSYM_XOR;
	BOOL						b_AttenuateLight;
	float						f_AttenuateCoeff;

#ifdef ACTIVE_EDITORS
    LONG                        l_Size = 0;
    ULONG	                    *pul_RLI = NULL;
#endif

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(ACTIVE_EDITORS)
    if (!GDI_b_IsXenonGraphics() || (LIGHT_gl_ComputeRLI != 0) || LIGHT_gb_DisableXenonSend)
    {
        LIGHT_SendObjectToLight(_pst_LightList, _pst_Node);
        return;
    }
#endif

    /* pas de lumière ? */
    if ( _pst_LightList->ul_Current == 0 )
    {
        ClearElementsForLighting(_pst_Node);
        return;
    }

    //
    // Get elements info. If those elements have a bounding volume, we need to check them
    // individually against the lights. Only those that intersect are added to the mesh light set.
    //
    int		 iNbElementsMeshes = 0;
    XeMesh   **pElementsMeshes = NULL;

    bool     isLightingPerElement = (GDI_gpst_CurDD->ul_LightCullingFlags & GDI_Cul_Light_Cull_Element) != 0;

#ifdef VS_FORCE_SW_LIGHTING
    isLightingPerElement = false;
#endif

    ULONG ulMaxLightsForGao = InitElementsForLighting( _pst_Node, isLightingPerElement, _ulMaxRequestedLights );
        
    PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_All);
	_GSP_BeginRaster(1);
		
    /* symétrie ? */
    ulLIGHT_XOR = (GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF) ? 0 : 1;
    ulLIGHT_NORMAL_XSYM_XOR  = 0;

    if (!(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_Symetric))
    {
        ulLIGHT_NORMAL_XSYM_XOR  = 1;
        ulLIGHT_XOR ^= 1;
    }
	
#if defined(ACTIVE_EDITORS) || defined(VS_FORCE_SW_LIGHTING)
    SOFT_tdst_ComputingBuffers *pst_CB = GDI_gpst_CurDD_SPR.pst_ComputingBuffers;
    pst_CB->ul_Ambient = 0;
    ULONG *pul_Diffuse = pst_CB->aul_Diffuse;
#endif

#ifdef VS_FORCE_SW_LIGHTING
    LIGHT_gpst_CurObject = ((GEO_tdst_Object *) _pst_Node->pst_Base->pst_Visu->pst_Object);
	LIGHT_gpst_CurPointsBuffer = GDI_gpst_CurDD_SPR.p_Current_Vertex_List;
    GEO_UseNormals(LIGHT_gpst_CurObject);
    MATH_tdst_Vector *pst_Normal = LIGHT_gpst_CurObject->dst_PointNormal;
    if(!pst_Normal) return;
#endif

    b_BVRejection = GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_UseBVForLightRejection;

	if ( b_AttenuateLight = !(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_DontAttenuateLight) )
	{
		f_AttenuateCoeff = ((float) 127 - _pst_Node->pst_Base->pst_Visu->c_LightAttenuation ) / 127.0f;
		f_AttenuateCoeff *= (1.0f - GDI_gpst_CurDD_SPR.f_LightAttenuation);
	}
    
    /* editor : start computing RLI */	
    M_4Edit_ComputeRLIStart();

    // Compute world to object matrix
    MATH_tdst_Matrix mWorldToObject;
    MATH_tdst_Matrix mObjectToWorld;
    float fWorldToObjectMaxScaling = 1.0f;

    //
    // From all the lights in the scene, build a list of those that potentially need to be
    // computed for this object
    //
    ULONG           ulNbrPotentialLights = 0;
    ulNbrPotentialLights = BuildPotentialLightsList( g_potentialLights,
                                                     _pst_LightList, 
                                                     _pst_Node, 
                                                     b_BVRejection );

    bool hasTooManyLights   = (ulNbrPotentialLights > ulMaxLightsForGao);
    bool hasLights          = (ulNbrPotentialLights > 0 );

    ULONG ulNbrOfLights = ulNbrPotentialLights;

    if( hasLights )
    {
        /* Move light into node system axis */
        MATH_InvertMatrix( &LIGHT_gst_NIM, _pst_Node->pst_GlobalMatrix );

        /* calcul le déplacement du centre de la BV dans le repère de l'objet (necessaire pour les omni const) */
        MATH_InitVector( &vCenterOfGaoBvInObjectSpace, 0.0f, 0.0f, 0.0f );
        if ( _pst_Node->pst_BV )
        {
            MATH_AddVector( &vCenterOfGaoBvInObjectSpace, OBJ_pst_BV_GetGMin(_pst_Node->pst_BV), OBJ_pst_BV_GetGMax(_pst_Node->pst_BV));
            MATH_MulEqualVector(&vCenterOfGaoBvInObjectSpace, 0.5f);
            MATH_TransformVector( &W, &LIGHT_gst_NIM, &vCenterOfGaoBvInObjectSpace ); 
        }
        else
        {
            MATH_InitVectorToZero( &W );
        }

#ifdef VS_FORCE_SW_LIGHTING
        LIGHT_Invert( pst_Normal, LIGHT_gpst_CurObject->l_NbPoints,ulLIGHT_NORMAL_XSYM_XOR, ulLIGHT_XOR );
#endif
        if( isLightingPerElement || hasTooManyLights )
        {
            ComputeCurrentWorldToObject( mWorldToObject, mObjectToWorld, fWorldToObjectMaxScaling );
          
            // For each light, find it's center in object space
            for( ULONG l=0; l<ulNbrPotentialLights; ++l )
            {
                PotentialLight * pPotentialLight = &g_potentialLights[l];

                // Directionnals have no position
                if( (pPotentialLight->m_pLight->ul_Flags & LIGHT_Cul_LF_Type) != LIGHT_Cul_LF_Direct )
                {
                    ERR_X_Assert( pPotentialLight->m_pLightGameObject );
                    MATH_TransformVertex( &pPotentialLight->m_vLightCenterInObjectSpace, 
                                          &mWorldToObject, 
                                          OBJ_pst_GetAbsolutePosition( pPotentialLight->m_pLightGameObject ) );
                }
            }
        }

        // Sort to keep the most influencial lights first
        if( hasTooManyLights )
        {
			//set flag for the secto warning
#ifdef	ACTIVE_EDITORS
			_pst_Node->ul_AdditionalFlags |= OBJ_C_EdAddFlags_TooManyLights;
#endif	//ACTIVE_EDITORS

            SetupPotentialLightsLuminance( g_potentialLights, ulNbrPotentialLights );

            MATH_tdst_Vector vRadius;
            MATH_SubVector( &vRadius, OBJ_pst_BV_GetGMax(_pst_Node->pst_BV), &vCenterOfGaoBvInObjectSpace );
            float fRadius = MATH_f_NormVector(& vRadius );

            // Sort by descending order of influence. This way all potential lights will be added
            // to the renderer stacks but only the most influencial will make it to the global light set
            SortPotentialLights( g_potentialLights, 
                                 ulNbrPotentialLights,
                                 mWorldToObject,
                                 fWorldToObjectMaxScaling,
                                 vCenterOfGaoBvInObjectSpace,
                                 fRadius);

            ulNbrOfLights = ulMaxLightsForGao;
        }
    }

    M_4Edit_If_Light_Logging_Cond( ulNbrOfLights > 0, g_oXeLightUsageLogger.BeginGAO( _pst_Node->sz_Name ) );

	//clear array of lights that's affect the actual GAO
#ifdef	ACTIVE_EDITORS
	memset(_pst_Node->apst_IsLightBy, 0, sizeof(_pst_Node->apst_IsLightBy));
#endif	//ACTIVE_EDITORS

    //
    // Add every potential light to the renderer
    //
    g_pXeContextManagerEngine->BeginAddLights();
    for( ULONG l=0; l<ulNbrPotentialLights; ++l )
    {
        PotentialLight * pPotentialLight = &g_potentialLights[l];
        ERR_X_Assert( pPotentialLight->m_pLight != NULL && pPotentialLight->m_pLightGameObject != NULL );

        LIGHT_gpst_Cur = pPotentialLight->m_pLight;

        OBJ_tdst_GameObject **ppst_LightNode = &pPotentialLight->m_pLightGameObject;
        M_4Edit_ComputeRLIAdd();

        /* editor : liste des lumières qui éclaire un objet donné */
        M_4Edit_ListOfLightOnObject()

		//add light pointer to the list
#ifdef	ACTIVE_EDITORS
			_pst_Node->apst_IsLightBy[l] = (*ppst_LightNode);
#endif	//ACTIVE_EDITORS


        /* calcul la matrice de la lumière dans le repère de l'objet */
        MATH_MulMatrixMatrix(&LIGHT_gst_LP, pPotentialLight->m_pLightGameObject->pst_GlobalMatrix, &LIGHT_gst_NIM);

        /* couleur de la lumière */
        LIGHT_gul_Color = LIGHT_gpst_Cur->ul_Color;
        if(!(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_UseColor)) LIGHT_gul_Color &= 0xFF000000;
        if(!(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_UseAlpha)) LIGHT_gul_Color &= 0x00FFFFFF;
        if (b_AttenuateLight && !(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_DontAttenuate))
        {
            LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
            LIGHT_gul_Color =  LIGHT_ul_MulColor( LIGHT_gul_Color, f_AttenuateCoeff );
        }

        ULONG ulLightIndex = 0;
        ULONG ulLightType = 0;

        /* calcul de la lumière selon son type */
        switch(LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_Type | LIGHT_Cul_LF_Specular))
        {
        case LIGHT_Cul_LF_Omni | LIGHT_Cul_LF_Specular:
        case LIGHT_Cul_LF_Omni:

            LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;

#ifdef VS_FORCE_SW_LIGHTING
            LIGHT_SendObjectToSpherical(pul_Diffuse, pst_Normal,NULL);
#else
            ulLightType   = LIGHT_TYPE_OMNI;
            ulLightIndex  = Xe_AddLight(LIGHT_gpst_Cur, (l<ulNbrOfLights) );
#endif
            break;			    

        case LIGHT_Cul_LF_Direct | LIGHT_Cul_LF_Specular:
        case LIGHT_Cul_LF_Direct:
#ifdef VS_FORCE_SW_LIGHTING
            LIGHT_SendObjectToDirect(pul_Diffuse, pst_Normal);
#else
            ulLightType   = LIGHT_TYPE_DIRECT;
            ulLightIndex  = Xe_AddLight(LIGHT_gpst_Cur, (l<ulNbrOfLights) );
#endif
            break;

        case LIGHT_Cul_LF_Spot | LIGHT_Cul_LF_Specular:
        case LIGHT_Cul_LF_Spot:

            LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;

#ifdef VS_FORCE_SW_LIGHTING
            LIGHT_SendObjectToSpot(pul_Diffuse, pst_Normal);
#else
            ulLightType = (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical) ? LIGHT_TYPE_CYLINDRICAL_SPOT : LIGHT_TYPE_SPOT;
            ulLightIndex  = Xe_AddLight(LIGHT_gpst_Cur, (l<ulNbrOfLights) );
#endif
            break;

        case LIGHT_Cul_LF_LightShaft | LIGHT_Cul_LF_Specular:
        case LIGHT_Cul_LF_LightShaft:
#ifndef VS_FORCE_SW_LIGHTING
            ulLightType   = LIGHT_TYPE_SPOT;
            ulLightIndex = Xe_AddLight(LIGHT_gpst_Cur, (l<ulNbrOfLights) );
#endif
            break;
        }

        M_4Edit_ComputeRLIAdd();

        // Keep handle to the added lights so we can reference them in the per-element light sets
        pPotentialLight->m_ulLightType  = ulLightType;
        pPotentialLight->m_ulLightIndex = ulLightIndex;

        M_4Edit_If_Light_Logging_Cond( (!isLightingPerElement && (l<ulNbrOfLights)), g_oXeLightUsageLogger.AddLight( pPotentialLight->m_pLightGameObject->sz_Name ) );
    }

    g_pXeContextManagerEngine->EndAddLights();

#ifndef VS_FORCE_SW_LIGHTING
    if( isLightingPerElement )
    {
        ComputeLightSetsPerElement( _pst_Node, 
                                    g_potentialLights,
                                    ulNbrPotentialLights,
                                    mWorldToObject,
                                    mObjectToWorld,
                                    fWorldToObjectMaxScaling,
                                    _ulMaxRequestedLights);
    }
#endif
        
    M_4Edit_If_Light_Logging_Cond( ulNbrOfLights > 0, g_oXeLightUsageLogger.EndGAO( ) );
  
#ifdef VS_FORCE_SW_LIGHTING
    /* Restore eventually initial Normals */
    if ( hasLights )
    {
        LIGHT_Invert(pst_Normal,LIGHT_gpst_CurObject->l_NbPoints,ulLIGHT_NORMAL_XSYM_XOR,ulLIGHT_XOR);
    }
#endif

    M_4Edit_ComputeRLIEnd()

	_GSP_EndRaster(1);
    PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_All);
}

#endif // _XBOX

float LIGHT_ComputeExclusiveLightFactor(LIGHT_tdst_List *_pst_LightList, OBJ_tdst_GameObject *_pst_Node)
{
	float returnValue;
	u32 Counter;
	OBJ_tdst_GameObject			**ppst_LightNode, **ppst_Last;
	returnValue = 1.0f;
	Counter = 0;
    ppst_LightNode = _pst_LightList->dpst_Light;
	ppst_Last = ppst_LightNode + _pst_LightList->ul_Current;
	while (ppst_LightNode < ppst_Last)
	{
		LIGHT_tdst_Light        *LIGHT_Local;
		LIGHT_Local = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;
        /* l'objet est il éclairé ? */
	    if (!LIGHT_l_IsNodeLighted(_pst_Node, *ppst_LightNode, LIGHT_Local, TRUE)) 
	    {
	        ppst_LightNode++;
	        continue;
        }
		if ((LIGHT_Local->ul_Flags & (LIGHT_Cul_LF_ExclusiveLight | LIGHT_Cul_LF_Type)) == (LIGHT_Cul_LF_Omni | LIGHT_Cul_LF_ExclusiveLight))
		{
			MATH_tdst_Vector Distance;
			float Norm;
			Counter++;	

			MATH_SubVector(&Distance,&(*ppst_LightNode)->pst_GlobalMatrix->T,&_pst_Node->pst_GlobalMatrix->T);
			Norm = MATH_f_SqrVector(&Distance);
			if (Norm < LIGHT_Local->st_Omni.f_Far * LIGHT_Local->st_Omni.f_Far)
			{
				if (Norm < LIGHT_Local->st_Omni.f_Near * LIGHT_Local->st_Omni.f_Near)
				{
					returnValue = 0.0f;
				} else
					returnValue *= (fOptSqrt(Norm) - LIGHT_Local->st_Omni.f_Near) / (LIGHT_Local->st_Omni.f_Far - LIGHT_Local->st_Omni.f_Near);
			}
		}
		if (returnValue == 0.0f)									ppst_LightNode = ppst_Last;
		if (Counter == _pst_LightList->ulContainExclusiveLights)	ppst_LightNode = ppst_Last;
		ppst_LightNode++;
	}

	return returnValue;

}


void LIGHT_SendObjectToLight( LIGHT_tdst_List *_pst_LightList, OBJ_tdst_GameObject *_pst_Node )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			**ppst_LightNode, **ppst_Last;
	MATH_tdst_Vector			*pst_Normal, V, W;
	LONG						l_FirstComputing;
	ULONG						*pul_Diffuse;
	SOFT_tdst_ComputingBuffers	*pst_CB;
   	int                         i_Mat;
   	BOOL                        b_BVRejection;
	ULONG						ulLIGHT_XOR;
	ULONG						ulLIGHT_NORMAL_XSYM_XOR;
	BOOL						b_AttenuateLight;
	float						f_AttenuateCoeff;
#ifdef ACTIVE_EDITORS
	LONG						l_Size;
	ULONG						*pul_RLI;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*#if defined(_GAMECUBE) && defined(USE_HARDWARE_LIGHTS)
	extern u32 THIS_IS_KONG(OBJ_tdst_GameObject	 *ss);
#endif */

#ifdef Active_CloneListe
//-- HARDWARELIGHT FOR CLONES --
if ( !(GDI_gpst_CurDD->ul_DrawMask & GDI_Cul_DM_NoAutoClone) )
if (_pst_Node->p_CloneNextGao) return;

#endif

#if defined(_GAMECUBE) && defined(USE_HARDWARE_LIGHTS)
	GXI_Global_ACCESS(LightMask) = GX_LIGHT_NULL;

/*	if (THIS_IS_KONG(_pst_Node)) // Kingkong ?)
	{
		g_bUseHardwareLights=FALSE;
	} else */
		g_bUseHardwareLights=TRUE;
	
#endif

    /* pas de lumière ? */
    LIGHT_gpst_Bump = NULL;
    if ( _pst_LightList->ul_Current == 0 ) return;

    /* bench , rasters, nolight */
#if defined(GSP_PS2) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if (NoLIGH) return;
#endif

    PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_All);
	
	JADED_PROFILER_START();
		
    M_TestGameCubeHardwareLights
	{
		/* symétrie ? */
        ulLIGHT_XOR = (GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF) ? 0 : 1;
		ulLIGHT_NORMAL_XSYM_XOR  = 0;

		if (!(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_Symetric))
		{
#ifdef PSX2_TARGET		
			extern u_int *p_OriginalPtr_CN;
			if (!p_OriginalPtr_CN) 
#endif			
				ulLIGHT_NORMAL_XSYM_XOR  = 1;
			ulLIGHT_XOR ^= 1;
		}
	}
	
    /* init des variables */
    l_FirstComputing = 1;
	pst_CB = GDI_gpst_CurDD_SPR.pst_ComputingBuffers;
	pst_CB->ul_Ambient = 0;
	LIGHT_gpst_CurObject = ((GEO_tdst_Object *) _pst_Node->pst_Base->pst_Visu->pst_Object);
	LIGHT_gpst_CurPointsBuffer = GDI_gpst_CurDD_SPR.p_Current_Vertex_List;
    pul_Diffuse = pst_CB->aul_Diffuse;
	GEO_UseNormals(LIGHT_gpst_CurObject);
    pst_Normal = LIGHT_gpst_CurObject->dst_PointNormal;
    if(!pst_Normal) return;
    b_BVRejection = GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_UseBVForLightRejection;

	/* Pliplo Exclusivelight */
	f_AttenuateCoeff = 1.0f;

	if ( b_AttenuateLight = !(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_DontAttenuateLight) )
	{
		f_AttenuateCoeff = ((float) 127 - _pst_Node->pst_Base->pst_Visu->c_LightAttenuation ) / 127.0f;
		f_AttenuateCoeff *= (1.0f - GDI_gpst_CurDD_SPR.f_LightAttenuation);
	}

	/* Pliplo Exclusivelight */
	if (_pst_LightList->ulContainExclusiveLights)
		f_AttenuateCoeff *= LIGHT_ComputeExclusiveLightFactor(_pst_LightList, _pst_Node);

    /* editor : start computing RLI */	
    M_4Edit_ComputeRLIStart();
    
    /* loop sur la liste des lumières */
    ppst_LightNode = _pst_LightList->dpst_Light;
	ppst_Last = ppst_LightNode + _pst_LightList->ul_Current;
	while(ppst_LightNode < ppst_Last)
	{
		LIGHT_gpst_Cur = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;

#ifdef _XBOX
		LIGHT_ConvertLightColor(LIGHT_gpst_Cur);
#endif

		/* l'objet est il éclairé ? */
	    if (!LIGHT_l_IsNodeLighted(_pst_Node, *ppst_LightNode, LIGHT_gpst_Cur, b_BVRejection)) 
	    {
	        ppst_LightNode++;
	        continue;
        }

#ifndef JADEFUSION
		if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Bump)
            LIGHT_gpst_Bump = *ppst_LightNode;
#endif

        /* editor : liste des lumières qui éclaire un objet donné */
        M_4Edit_ListOfLightOnObject()

        if ( (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Type) != LIGHT_Cul_LF_AddMaterial )
        {
            /* Move light into node system axis */
		    if(l_FirstComputing)
		    {
			    l_FirstComputing = 0;
			    MATH_InvertMatrix(&LIGHT_gst_NIM, _pst_Node->pst_GlobalMatrix);

                /* calcul le déplacement du centre de la BV dans le repère de l'objet (necessaire pour les omni const) */
                if ( _pst_Node->pst_BV )
                {
                    MATH_AddVector( &V, OBJ_pst_BV_GetGMin(_pst_Node->pst_BV), OBJ_pst_BV_GetGMax(_pst_Node->pst_BV));
		            MATH_MulEqualVector(&V, 0.5f);
                    MATH_TransformVector( &W, &LIGHT_gst_NIM, &V ); 
                }
                else
                {
                    MATH_InitVectorToZero( &W );
                }
			    
                M_TestGameCubeHardwareLights
				{	    
#ifdef PSX2_TARGET		    
					if (LIGHT_gpst_CurObject->p_CompressedNormals)
	                	LIGHT_Invert_C(LIGHT_gpst_CurObject->p_CompressedNormals,LIGHT_gpst_CurObject->l_NbPoints,ulLIGHT_NORMAL_XSYM_XOR,ulLIGHT_XOR);
                	else
#endif                	
    	            	LIGHT_Invert(pst_Normal,LIGHT_gpst_CurObject->l_NbPoints,ulLIGHT_NORMAL_XSYM_XOR,ulLIGHT_XOR);
                }
		    }

            /* calcul la matrice de la lumière dans le repère de l'objet */
            MATH_MulMatrixMatrix(&LIGHT_gst_LP, (*ppst_LightNode)->pst_GlobalMatrix, &LIGHT_gst_NIM);					
            
            /* si la lumière est une omni const le centre du repère de l'objet est déplacé sur le centre de la BV */
            if ( (LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_Type | LIGHT_Cul_LF_OmniConst)) == (LIGHT_Cul_LF_Omni | LIGHT_Cul_LF_OmniConst) ) 
                MATH_SubEqualVector( &LIGHT_gst_LP.T, &W );
                
            /* couleur de la lumière */
		    LIGHT_gul_Color = LIGHT_gpst_Cur->ul_Color;
		    if(!(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_UseColor)) LIGHT_gul_Color &= 0xFF000000;
		    if(!(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_UseAlpha)) LIGHT_gul_Color &= 0x00FFFFFF;

			/* Pliplo Exclusivelight */
			if ((LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_ExclusiveLight)) == 0)
			{
				LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
				LIGHT_gul_Color =  LIGHT_ul_MulColor( LIGHT_gul_Color, f_AttenuateCoeff );
			}
			else
			if (b_AttenuateLight && !(LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_DontAttenuate|LIGHT_Cul_LF_ExclusiveLight)))
			{
				LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
				LIGHT_gul_Color =  LIGHT_ul_MulColor( LIGHT_gul_Color, f_AttenuateCoeff );
			} 
		    
            /* initialisation des couleurs spéculaires */
            M_TestGameCubeHardwareLights
			{		    
			    if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Specular)
			    {
				    if (!(GDI_gpst_CurDD->pst_ComputingBuffers->ul_Flags & SOFT_Cul_CB_SpecularColorField))
					    SOFT_pst_InitSpecularColors(LIGHT_gpst_CurObject->l_NbPoints);
			    }
			}
			
            /* calcul de la lumière selon son type */
		    switch(LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_Type | LIGHT_Cul_LF_Specular))
		    {
		    case LIGHT_Cul_LF_Omni | LIGHT_Cul_LF_Specular:
#if !defined(_XENON) && !defined(_GX8)
				/* Specular */
			    LIGHT_gpul_CurrentTable = LIGHT_gaul_ExponantTable;
			    LIGHT_gpst_Cur->st_Omni.f_Far *= LIGHT_C_fSpecularExp;
			    LIGHT_gpst_Cur->st_Omni.f_Near *= 0.25f;
			    LIGHT_SendObjectToSpherical(pst_CB->aul_Specular, SOFT_pst_GetSpecularVectors(),NULL);
			    LIGHT_gpst_Cur->st_Omni.f_Far *= LIGHT_C_OO_fSpecularExp;
			    LIGHT_gpst_Cur->st_Omni.f_Near *= 4.0f;
#endif			    
			
			    
		    case LIGHT_Cul_LF_Omni:



    		    LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
#ifdef PSX2_TARGET		    
			    LIGHT_SendObjectToSpherical(pul_Diffuse, pst_Normal,LIGHT_gpst_CurObject->p_CompressedNormals);
#elif defined(_XENON) && !defined(_GX8)
              #ifdef VS_FORCE_SW_LIGHTING
			    LIGHT_SendObjectToSpherical(pul_Diffuse, pst_Normal,NULL);
              #else
                Xe_AddLight(LIGHT_gpst_Cur);
              #endif
#elif defined(_GAMECUBE) && defined(USE_HARDWARE_LIGHTS)
				if(g_bUseHardwareLights)
				{
					LIGHT_SendObjectToHWLight(0);
					break;
				}
				else
					LIGHT_SendObjectToSpherical(pul_Diffuse, pst_Normal,NULL);
#else
			    LIGHT_SendObjectToSpherical(pul_Diffuse, pst_Normal,NULL);
				
#endif
				break;			    

		    case LIGHT_Cul_LF_Direct | LIGHT_Cul_LF_Specular:
#if !defined(_XENON) && !defined(_GX8)
		        LIGHT_gpul_CurrentTable = LIGHT_gaul_ExponantTable;
		        LIGHT_SendObjectToDirect(pst_CB->aul_Specular, SOFT_pst_GetSpecularVectors(),NULL);
#endif		        
		     
		    case LIGHT_Cul_LF_Direct:
				

			    LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
			    
#if defined(_GAMECUBE) && defined(USE_HARDWARE_LIGHTS)
				if(g_bUseHardwareLights)
				{
					LIGHT_SendObjectToHWLight(1);
					break;
				}
				else
					LIGHT_SendObjectToDirect(pul_Diffuse, pst_Normal,NULL);
#elif defined(_XENON) && !defined(_GX8)
              #ifdef VS_FORCE_SW_LIGHTING
			    LIGHT_SendObjectToDirect(pul_Diffuse, pst_Normal);
              #else
                Xe_AddLight(LIGHT_gpst_Cur);
              #endif
#else
#ifdef PSX2_TARGET		    
				LIGHT_SendObjectToDirect(pul_Diffuse, pst_Normal,LIGHT_gpst_CurObject->p_CompressedNormals);
#else
				LIGHT_SendObjectToDirect(pul_Diffuse, pst_Normal,NULL);
#endif
			    
				
#endif			    
			    break;
			    
		    case LIGHT_Cul_LF_Spot | LIGHT_Cul_LF_Specular:
#if !defined(_XENON) && !defined(_GX8)
				/* Specular */
		        LIGHT_gpul_CurrentTable = LIGHT_gaul_ExponantTable;
		        LIGHT_gpst_Cur->st_Spot.f_Far *= LIGHT_C_fSpecularExp;
		        LIGHT_SendObjectToSpot(pst_CB->aul_Specular, SOFT_pst_GetSpecularVectors());
		        LIGHT_gpst_Cur->st_Spot.f_Far *= LIGHT_C_OO_fSpecularExp;
#endif				
		    case LIGHT_Cul_LF_Spot:




			    LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
#if defined(_XENON) && !defined(_GX8)
              #ifdef VS_FORCE_SW_LIGHTING
                LIGHT_SendObjectToSpot(pul_Diffuse, pst_Normal);
              #else
                Xe_AddLight(LIGHT_gpst_Cur);
              #endif
#else
				LIGHT_SendObjectToSpot(pul_Diffuse, pst_Normal);
#endif				
	       	    break;
            }

            M_4Edit_ComputeRLIAdd()
        }
        else
        {
	        if (GDI_gpst_CurDD_SPR.pst_AdditionalMaterial == NULL)
	        {
	            if(((*ppst_LightNode)->pst_Base) && ((*ppst_LightNode)->pst_Base->pst_Visu) && ((*ppst_LightNode)->pst_Base->pst_Visu->pst_Material) )
	            {
	                if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_AddMaterialBVMustBeInside)
	                    i_Mat = COL_b_BVIn( (*ppst_LightNode), _pst_Node );
	                else
	                    i_Mat = COL_b_BVOverlap( _pst_Node, (*ppst_LightNode), TRUE, TRUE, FALSE);
	                
	                if ( i_Mat )
	                {
	                    GDI_gpst_CurDD_SPR.pst_AdditionalMaterial = (*ppst_LightNode)->pst_Base->pst_Visu->pst_Material;
	                    if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_AddMaterialDontUseNormalMaterial)
                            GDI_gpst_CurDD_SPR.ul_CurrentDrawMask &= ~GDI_Cul_DM_UseNormalMaterial;
	                    if (GDI_gpst_CurDD_SPR.pst_AdditionalMaterial->i->ul_Type == GRO_MaterialMulti)
	                    {
	                        i_Mat = (int) LIGHT_gpst_Cur->st_AddMaterial.w_Id;
	                        i_Mat %= ((MAT_tdst_Multi *) GDI_gpst_CurDD_SPR.pst_AdditionalMaterial)->l_NumberOfSubMaterials;
	                        GDI_gpst_CurDD_SPR.pst_AdditionalMaterial = (GRO_tdst_Struct *) ((MAT_tdst_Multi *) GDI_gpst_CurDD_SPR.pst_AdditionalMaterial)->dpst_SubMaterial[ i_Mat ];
                        }
                    }
                }
            }
        }

		ppst_LightNode++;
	}

    M_TestGameCubeHardwareLights
	{
	    /* Restore eventually initial Normals */
	    if (!l_FirstComputing)
	    {
#ifdef PSX2_TARGET		    
			if (LIGHT_gpst_CurObject->p_CompressedNormals)
            	LIGHT_Invert_C(LIGHT_gpst_CurObject->p_CompressedNormals,LIGHT_gpst_CurObject->l_NbPoints,ulLIGHT_NORMAL_XSYM_XOR,ulLIGHT_XOR);
        	else
#endif        	
	     	   LIGHT_Invert(pst_Normal,LIGHT_gpst_CurObject->l_NbPoints,ulLIGHT_NORMAL_XSYM_XOR,ulLIGHT_XOR);
	     }
	}

    M_4Edit_ComputeRLIEnd()

#if defined(_XENON_RENDER) && defined(ACTIVE_EDITORS)
    if (GDI_b_IsXenonGraphics() && (LIGHT_gl_ComputeRLI != 0))
    {
        if (!(LIGHT_gl_ComputeRLI & 4) || ((LIGHT_gl_ComputeRLI & 4) && (_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected)))
        {
            if (_pst_Node->pst_Base &&
                _pst_Node->pst_Base->pst_Visu &&
                _pst_Node->pst_Base->pst_Visu->pst_Object &&
                _pst_Node->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
            {
                GEO_tdst_Object* pstGeo = (GEO_tdst_Object*)_pst_Node->pst_Base->pst_Visu->pst_Object;

                LIGHT_ComputeVertexColor(pstGeo->l_NbPoints);
                GEO_ResetXenonMesh(_pst_Node, _pst_Node->pst_Base->pst_Visu, (GEO_tdst_Object*)_pst_Node->pst_Base->pst_Visu->pst_Object, FALSE, FALSE, FALSE);
            }
        }
    }
#endif
	
    PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_All);

	JADED_PROFILER_END();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

#ifdef _XBOX
void LIGHT_ComputeVertexColorXB(LONG l_NbPoints,bool HWLIGHT)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	*pul_Diffuse, ul_Ambient;
	ULONG	*Colors;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_Lights_NbComputedVertexColor, l_NbPoints);
    PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_ComputeVertexColor);

	if(l_NbPoints > SOFT_Cul_ComputingBufferSize) 
        l_NbPoints = SOFT_Cul_ComputingBufferSize;

    pul_Diffuse = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->aul_Diffuse;
	Colors = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ComputedColors;
	GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentColorField = Colors;

    /* Forced color */
#ifdef ACTIVE_EDITORS
    {
        ULONG ul_Color, ul_MatAmbient, ul_MatDiffuse;
        ULONG *pul_Last;
        
		if(!(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_DontForceColor))
		{
			ul_Color = (GDI_gpst_CurDD_SPR.ul_ColorConstant & 0xFFFFFF) | 0xFF000000;
			pul_Last = pul_Diffuse + l_NbPoints;
			for(; pul_Diffuse < pul_Last; pul_Diffuse++, Colors++) *Colors = ul_Color;
			PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_ComputeVertexColor);
			return;
		}
		
		/* Do not use material color */
		if ((GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_MaterialColor) && (GDI_gpst_CurDD_SPR.pst_CurrentMat))
		{
			if(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_UseAmbient)
				ul_Ambient = GDI_gpst_CurDD_SPR.pst_World->ul_AmbientColor;
			else
				ul_Ambient = 0;
			ul_MatAmbient = GDI_gpst_CurDD_SPR.pst_CurrentMat->ul_Ambiant;
			ul_MatDiffuse = GDI_gpst_CurDD_SPR.pst_CurrentMat->ul_Diffuse;
			LIGHT_ColorMulAdd(ul_MatDiffuse,ul_MatAmbient,ul_Ambient,pul_Diffuse,l_NbPoints,Colors);
			return;
		}
	}
#endif

	if(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_UseAmbient)
	{
		if (GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_DontUseAmbient2)
			ul_Ambient = GDI_gpst_CurDD_SPR.pst_World->ul_AmbientColor;
		else
			ul_Ambient = GDI_gpst_CurDD_SPR.pst_World->ul_AmbientColor2;


		ul_Ambient=Gx8_ConvertChrominancePixel(Gx8_M_ConvertColor(ul_Ambient)); 
		//CARLONE ul_Ambient=Gx8_M_ConvertColor(ul_Ambient);

		//Set the ambient
		Gx8_SetGlobalAmbient(GX8_M_SD(GDI_gpst_CurDD),ul_Ambient);
		Gx8_SetMateriaAmbient(GX8_M_SD(GDI_gpst_CurDD), 0xffffffff);
		memcpy(Colors,pul_Diffuse,l_NbPoints*sizeof(DWORD));

		if(!HWLIGHT)
		{
			//TURN OFF ALL THE LIGHT AND ACTIVATE AMBIENT ONLY
			LIGHT_SetLighting(true);
		}


	}
	else
    {
		//Set the ambient to 0
		Gx8_SetGlobalAmbient(GX8_M_SD(GDI_gpst_CurDD), 0);

		memcpy(Colors,pul_Diffuse,l_NbPoints*sizeof(DWORD));
		PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_ComputeVertexColor);
        return;
    }



	//ul_Ambient = 0;
	//CARLONE LIGHT_ColorAdd(ul_Ambient & 0xffffff,pul_Diffuse,l_NbPoints,Colors);

    PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_ComputeVertexColor);

}
#endif // _XBOX



void LIGHT_ComputeVertexColor(LONG l_NbPoints)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	*pul_Diffuse, *pul_Last, ul_Ambient;
	ULONG	*Colors;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_Lights_NbComputedVertexColor, l_NbPoints);
    PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_ComputeVertexColor);

	if(l_NbPoints > SOFT_Cul_ComputingBufferSize) 
        l_NbPoints = SOFT_Cul_ComputingBufferSize;

    pul_Diffuse = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->aul_Diffuse;
	Colors = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ComputedColors;
	GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentColorField = Colors;

    /* Forced color */
#ifdef ACTIVE_EDITORS
    {
        ULONG ul_Color, ul_MatAmbient, ul_MatDiffuse;
        
        if ((GDI_gpst_CurDD_SPR.pst_EditOptions->ul_Flags & GRO_Cul_EOF_ColorModeMask) == GRO_Cul_EOF_ColorModeAlpha )
        {
			pul_Last = pul_Diffuse + l_NbPoints;
		    for(; pul_Diffuse < pul_Last; pul_Diffuse++, Colors++) 
		    {
				ul_Color = *pul_Diffuse & 0xFF000000;
				ul_Color |= (ul_Color >> 8) & 0xFF0000;
				ul_Color |= (ul_Color >> 16) & 0xFFFF;
				*Colors = ul_Color| 0xFF000000;
			}
			PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_ComputeVertexColor);
			return;
        }
	       
		if(!(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_DontForceColor))
		{
			ul_Color = (GDI_gpst_CurDD_SPR.ul_ColorConstant & 0xFFFFFF) | 0xFF000000;
			pul_Last = pul_Diffuse + l_NbPoints;
			for(; pul_Diffuse < pul_Last; pul_Diffuse++, Colors++) *Colors = ul_Color;
			PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_ComputeVertexColor);
			return;
		}
		    
		/* Do not use material color */
		if ((GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_MaterialColor) && (GDI_gpst_CurDD_SPR.pst_CurrentMat))
		{
			if(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_UseAmbient)
				ul_Ambient = GDI_gpst_CurDD_SPR.pst_World->ul_AmbientColor;
			else
				ul_Ambient = 0;
			ul_MatAmbient = GDI_gpst_CurDD_SPR.pst_CurrentMat->ul_Ambiant;
			ul_MatDiffuse = GDI_gpst_CurDD_SPR.pst_CurrentMat->ul_Diffuse;
			LIGHT_ColorMulAdd(ul_MatDiffuse,ul_MatAmbient,ul_Ambient,pul_Diffuse,l_NbPoints,Colors);
			return;
		}
    }
#endif

	if(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_UseAmbient)
	{
      #if defined(_XENON) && !defined(VS_FORCE_SW_LIGHTING)
        // force ambient to black since it will be added by the HW lighting.
        ul_Ambient = 0;
      #else
		if (GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_DontUseAmbient2)
			ul_Ambient = GDI_gpst_CurDD_SPR.pst_World->ul_AmbientColor;
		else
			ul_Ambient = GDI_gpst_CurDD_SPR.pst_World->ul_AmbientColor2;
	#endif
#ifdef _XBOX
		ul_Ambient=Gx8_ConvertChrominancePixel(Gx8_M_ConvertColor(ul_Ambient)); 
		ul_Ambient=Gx8_M_ConvertColor(ul_Ambient);
#endif

	}
	else
    {
	    pul_Last = pul_Diffuse + l_NbPoints;
        for(; pul_Diffuse < pul_Last; ) *Colors++ = *pul_Diffuse++;

        PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_ComputeVertexColor);
        return;
    }


//	ul_Ambient = 0;
	LIGHT_ColorAdd(ul_Ambient & 0xffffff,pul_Diffuse,l_NbPoints,Colors);
    PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_Light_ComputeVertexColor);
}
#ifdef JADEFUSION
void LIGHT_ResetAllSpotCullingBV( LIGHT_tdst_List *_pst_LightList )
{
    // Set to dirty all bounding voumes for spot lights in case parameters have changed
    OBJ_tdst_GameObject			**ppst_LightNode, **ppst_Last;
    ppst_LightNode = _pst_LightList->dpst_Light;
    ppst_Last = ppst_LightNode + _pst_LightList->ul_Current;
    while( ppst_LightNode < ppst_Last )
    {
        LIGHT_tdst_Light * pst_Light = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;

        ULONG ulType = pst_Light->ul_Flags & LIGHT_Cul_LF_Type;
        
        if( ulType == LIGHT_Cul_LF_Spot )
        {
            pst_Light->st_Spot.b_IsBVValid = FALSE;
        }
        ++ppst_LightNode;
    }
}
#endif
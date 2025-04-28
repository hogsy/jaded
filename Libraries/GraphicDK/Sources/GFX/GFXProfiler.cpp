/*$T GFXline.c GC! 1.081 09/19/00 09:14:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "GDInterface/GDInterface.h"
#include "GFX/GFX.h"
#include "GFX/GFXProfiler.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#ifdef PSX2_TARGET


/* mamagouille */
#include "PSX2debug.h"
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define ZeroMalloc(Ptr , Number)\
	Ptr = MEM_p_Alloc(sizeof(*Ptr) * Number);\
	L_memset( (void *)Ptr , 0 , sizeof(*Ptr) * Number);
#define ZeroMallocType(Ptr , Number, Type)\
	Ptr = (Type*)MEM_p_Alloc(sizeof(*Ptr) * Number);\
	L_memset( (void *)Ptr , 0 , sizeof(*Ptr) * Number);
#define VERIFY_PROFILER(Ptr)\
	if (!Ptr->Valid) return;

#ifdef ACTIVE_EDITOR
#define GFX_CRASH(a) *(ULONG *)0 = *(ULONG *)0;
#else
#define GFX_CRASH(a)
#endif

#ifndef PSX2_TARGET
#define powf pow
#endif

/*$4
 ***********************************************************************************************************************
    Internal Functions
 ***********************************************************************************************************************
 */

/*$3
 -----------------------------------------------------------------------------------------------------------------------
    This will move mesh points as particules
 -----------------------------------------------------------------------------------------------------------------------
 */
void GFX_i_Profiler_UpdateMecanic(GFX_tdst_Profiler *p_Prof,float fTimeCoef)
{
	ULONG *p_APS,*p_APSEnd;
	GFX_tdst_Profiler_Point	*p_PPoint,*p_PPointLast;
	MATH_tdst_Vector stFriction;

	VERIFY_PROFILER(p_Prof);

	p_APS = p_Prof->p_AllProfileStates;
	p_APSEnd = p_APS + p_Prof->ulNumberOfProfiles;
	
	{
		float fFactor;
		fFactor = fTimeCoef / (1.0f / 60.0f);
		stFriction.x = (float)powf(1.0f - p_Prof->FrictionXYZ.x , fFactor);
		stFriction.y = (float)powf(1.0f - p_Prof->FrictionXYZ.y , fFactor);
		stFriction.z = (float)powf(1.0f - p_Prof->FrictionXYZ.z , fFactor);
	}
	

	p_PPoint = p_Prof->p_AllPoints;
	while (p_APS < p_APSEnd)
	{
		if (*p_APS)
		{
			GFX_tdst_Profiler_Point_Generator *p_CurntPG;
			/* Update profile Points */
			p_PPointLast = p_PPoint+p_Prof->ulNumberOfPointsPerProfile;
			p_CurntPG = p_Prof->p_PointGenerator;
			while (p_PPoint < p_PPointLast)
			{
				p_PPoint->ActualSpeed.z += p_Prof->fGravity * fTimeCoef;
				MATH_AddScaleVector(&p_PPoint->ActualSpeed,&p_PPoint->ActualSpeed,&p_CurntPG->Force , fTimeCoef);
				MATH_AddScaleVector(&p_PPoint->ActualPos,&p_PPoint->ActualPos,&p_PPoint->ActualSpeed , fTimeCoef);
				if (p_PPoint->ActualPos.z < p_PPoint->fPointConstraintZMin) p_PPoint->ActualPos.z = p_PPoint->fPointConstraintZMin;
				p_PPoint->ActualSpeed.x *= stFriction.x;
				p_PPoint->ActualSpeed.y *= stFriction.y;
				p_PPoint->ActualSpeed.z *= stFriction.z;
				if (p_PPoint ->ActualColor & 0xFc000000) p_PPoint ->ActualColor-= 0x02000000;

				p_PPoint++;
				p_CurntPG++;
			}
		} else p_PPoint += p_Prof->ulNumberOfPointsPerProfile;
		p_APS++;
	}
}


/*$3
 -----------------------------------------------------------------------------------------------------------------------
    This will compute number of triangle & number of points in final mesh
 -----------------------------------------------------------------------------------------------------------------------
 */
void GFX_i_Profiler_ComputeMeshState(GFX_tdst_Profiler *p_Prof)
{
	ULONG ActualState , NumberOfTri , NumberOfPoints , NumberOfUV ;
	ULONG *p_APS,*p_APSEnd;

	VERIFY_PROFILER(p_Prof);

	p_APS = p_Prof->p_AllProfileStates;
	p_APSEnd = p_APS + p_Prof->ulNumberOfProfiles;
	ActualState = *(p_APSEnd - 1);
	NumberOfPoints = NumberOfTri = NumberOfUV = 0;
	while (p_APS < p_APSEnd)
	{
		if (*p_APS)
		{
			*p_APS = ((NumberOfPoints << 1) | 1);
			NumberOfPoints += p_Prof->ulNumberOfPointsPerProfile;
			if (ActualState)
			{
				NumberOfTri++;
			}
		}
		ActualState = *(p_APS++);
	}
	p_Prof->ulNumberOfTriangles = NumberOfTri * p_Prof->ulNumberOfLinksPerProfile * 2;
	p_Prof->ulNumberOfPoints = NumberOfPoints;
	p_Prof->ulNumberOfUV = p_Prof->ulNumberOfLinksPerProfile * 2 * (p_Prof->UTiler + 1);
}


/*$3
 -----------------------------------------------------------------------------------------------------------------------
    start & stop the profile (AI request)
 -----------------------------------------------------------------------------------------------------------------------
 */
void GFX_i_Profiler_StartStop(GFX_tdst_Profiler *p_Prof , ULONG StartStop)
{
	VERIFY_PROFILER(p_Prof);
	p_Prof->p_AllProfileStates[p_Prof->ulCurrentProfile] = StartStop;
	if (!StartStop)
	{
		p_Prof->ulCurrentProfile ++;
		if (p_Prof->ulCurrentProfile == p_Prof->ulNumberOfProfiles) p_Prof->ulCurrentProfile = 0;
	}
}

/*$3
 -----------------------------------------------------------------------------------------------------------------------
    Create the last profile base on GX (With) GY (Height) GZ (Depth)  
	(AI request)
 -----------------------------------------------------------------------------------------------------------------------
 */
void GFX_i_Profiler_Update_With_XYT(GFX_tdst_Profiler *p_Prof)
{
	ULONG NewProfile ;

	VERIFY_PROFILER(p_Prof);
	
	if (p_Prof->p_AllProfileStates[p_Prof->ulCurrentProfile] == 0) return;

	/* UpdateState */
	NewProfile = p_Prof->ulCurrentProfile + 1;
	if (NewProfile == p_Prof->ulNumberOfProfiles) NewProfile = 0;
	p_Prof->p_AllProfileStates[NewProfile] = p_Prof->p_AllProfileStates[p_Prof->ulCurrentProfile];
	p_Prof->ulCurrentProfile = NewProfile;

	/* Create New Profile */
	{
		GFX_tdst_Profiler_Point	*p_PPoint,*p_PPointLast;
		GFX_tdst_Profiler_Point_Generator	*p_PointG;
		if (p_Prof->bStateGoBack)
		{
			if (p_Prof->ulCurrentProfile == 0)
				p_Prof->ulCurrentProfile = p_Prof->ulNumberOfProfiles - 1;
			else
				p_Prof->ulCurrentProfile--;
		}


		p_PPoint = p_Prof->p_AllPoints + p_Prof->ulCurrentProfile * p_Prof->ulNumberOfPointsPerProfile;

		p_PPointLast = p_PPoint+p_Prof->ulNumberOfPointsPerProfile;

		p_PointG = p_Prof->p_PointGenerator;

		while (p_PPoint < p_PPointLast)
		{
			MATH_tdst_Vector	Pos;
			p_PPoint->ActualPos = p_Prof->GT;
			Pos = p_PointG->Pos;
			Pos.x += p_PointG->RandomPos.x * fRand(0.0f, 1.0f);
			Pos.y += p_PointG->RandomPos.y * fRand(0.0f, 1.0f);
			Pos.z += p_PointG->RandomPos.z * fRand(0.0f, 1.0f);
			MATH_AddScaleVector(&p_PPoint->ActualPos,&p_PPoint->ActualPos,&p_Prof->GX, Pos.x);
			MATH_AddScaleVector(&p_PPoint->ActualPos,&p_PPoint->ActualPos,&p_Prof->GY, Pos.y);
			MATH_AddScaleVector(&p_PPoint->ActualPos,&p_PPoint->ActualPos,&p_Prof->GZ, Pos.z);
			MATH_InitVectorToZero(&p_PPoint->ActualSpeed);
			Pos = p_PointG->initialSpeed;
			Pos.x += p_PointG->RandominitialSpeed.x * fRand(0.0f, 1.0f);
			Pos.y += p_PointG->RandominitialSpeed.y * fRand(0.0f, 1.0f);
			Pos.z += p_PointG->RandominitialSpeed.z * fRand(0.0f, 1.0f);
			MATH_AddScaleVector(&p_PPoint->ActualSpeed,&p_PPoint->ActualSpeed,&p_Prof->GX, Pos.x);
			MATH_AddScaleVector(&p_PPoint->ActualSpeed,&p_PPoint->ActualSpeed,&p_Prof->GY, Pos.y);
			MATH_AddScaleVector(&p_PPoint->ActualSpeed,&p_PPoint->ActualSpeed,&p_Prof->GZ, Pos.z);
			MATH_InitVector(&p_PPoint->ActualNormale , 0.0f , 0.0f , 1.0f);
			p_PPoint->ActualColor = p_PointG->ulColorStart & 0xe0ffffff;
			p_PPoint->fPointConstraintZMin = p_PointG->PointConstraintZMin;
			p_PPoint++;
			p_PointG++;
		}
	}

	/* Compute Delta - Distance */
	p_Prof->bStateGoBack = 0;
	{
		MATH_tdst_Vector	Posn , Posnm1 ;
		GFX_tdst_Profiler_Point	*p_PPoint,*p_PPointP;
		float SegmentLenght;
		/* Si pos nm1 is active */
		if ((p_Prof->ulCurrentProfile != 0) && (p_Prof->p_AllProfileStates[p_Prof->ulCurrentProfile-1]))
		{
			p_PPoint = p_Prof->p_AllPoints + p_Prof->ulCurrentProfile * p_Prof->ulNumberOfPointsPerProfile;
			MATH_AddVector(&Posn,&p_PPoint->ActualPos,&(p_PPoint + (p_Prof->ulNumberOfPointsPerProfile - 1))->ActualPos);
			if (p_Prof->ulCurrentProfile == 0)
				p_PPointP = p_Prof->p_AllPoints + (p_Prof->ulNumberOfProfiles - 1) * p_Prof->ulNumberOfPointsPerProfile;
			else
				p_PPointP = p_Prof->p_AllPoints + (p_Prof->ulCurrentProfile - 1) * p_Prof->ulNumberOfPointsPerProfile;
			MATH_AddVector(&Posnm1,&p_PPointP->ActualPos,&(p_PPointP + (p_Prof->ulNumberOfPointsPerProfile - 1))->ActualPos);
			SegmentLenght = 0.5f * MATH_f_Distance(&Posn,&Posnm1);
			if (SegmentLenght < p_Prof->fSegmentSizeMin)
			{
				/* Don't create the point */
				p_Prof->bStateGoBack = 1;
			} else
			{
				/* Create the point and update end alpha's */
				ULONG ulProfile,PrCounter;
				ulProfile = p_Prof->ulCurrentProfile;
				PrCounter= 0x9;
				while (PrCounter--)
				{
					GFX_tdst_Profiler_Point	*p_PPoint,*p_PPointLast;
					if (ulProfile >= p_Prof->ulNumberOfProfiles - 1) ulProfile = 0;
					else ulProfile++;

					p_PPoint = p_Prof->p_AllPoints + ulProfile * p_Prof->ulNumberOfPointsPerProfile;
					p_PPointLast = p_PPoint  + p_Prof->ulNumberOfPointsPerProfile;
					while (p_PPoint  < p_PPointLast )
					{
						if (p_PPoint ->ActualColor & 0xe0000000) p_PPoint ->ActualColor-= 0x20000000;
						p_PPoint ++;
					}
				}

			}
		}

	}

	/* Stop profile +1 because rotativ buffer */
	NewProfile = p_Prof->ulCurrentProfile + 1;
	if (NewProfile == p_Prof->ulNumberOfProfiles) NewProfile = 0;
	p_Prof->p_AllProfileStates[NewProfile] = 0;
}

/*$3
 -----------------------------------------------------------------------------------------------------------------------
    Validate the profile. 
 -----------------------------------------------------------------------------------------------------------------------
 */
void GFX_Profiler_Validate(GFX_tdst_Profiler *p_Prof)
{
	if (p_Prof->Valid) return;
	if (!p_Prof->ulNumberOfPointsPerProfile) return;
	if (!p_Prof->ulNumberOfLinksPerProfile) return;
	if (!p_Prof->ulNumberOfProfiles) return;

	ZeroMallocType(p_Prof->p_AllLinks , p_Prof->ulNumberOfLinksPerProfile, GFX_tdst_Profiler_Link);
	ZeroMallocType(p_Prof->p_PointGenerator , p_Prof->ulNumberOfPointsPerProfile, GFX_tdst_Profiler_Point_Generator);
	ZeroMallocType(p_Prof->p_AllProfileStates , p_Prof->ulNumberOfProfiles, ULONG);
	ZeroMallocType(p_Prof->p_AllPoints , p_Prof->ulNumberOfPointsPerProfile * p_Prof->ulNumberOfProfiles, GFX_tdst_Profiler_Point);

	p_Prof->Valid = 1;
	p_Prof->UTiler = 4;
}

/*$3
 -----------------------------------------------------------------------------------------------------------------------
    Set number of points per profile
	(AI request)
 -----------------------------------------------------------------------------------------------------------------------
 */
void GFX_SetNumberOfPPP(GFX_tdst_Profiler *p_Prof,ULONG NPPP)
{
	if (p_Prof->Valid) GFX_CRASH("Ne pas definir plusieurs fois par profile !!!");

	
	p_Prof->ulNumberOfPointsPerProfile = NPPP;

	GFX_Profiler_Validate(p_Prof);
}
/*$3
 -----------------------------------------------------------------------------------------------------------------------
    Set number of links per profile
	(AI request)
 -----------------------------------------------------------------------------------------------------------------------
 */
void GFX_SetNumberOfLPP(GFX_tdst_Profiler *p_Prof,ULONG NLPP)
{
	if (p_Prof->Valid) GFX_CRASH("Ne pas definir plusieurs fois par profile !!!");

	p_Prof->ulNumberOfLinksPerProfile = NLPP;
	GFX_Profiler_Validate(p_Prof);
}
/*$3
 -----------------------------------------------------------------------------------------------------------------------
    Set number of profiles in profiler
	(AI request)
 -----------------------------------------------------------------------------------------------------------------------
 */
void GFX_SetNumberOfP(GFX_tdst_Profiler *p_Prof , ULONG Np)
{
	if (p_Prof->Valid) GFX_CRASH("Ne pas definir plusieurs fois par profile !!!");

	p_Prof->ulNumberOfProfiles = Np;

	p_Prof->ulAlphaSub = 255 / Np;
	if (p_Prof->ulAlphaSub <= 0) p_Prof->ulAlphaSub = 1;
	if (p_Prof->ulAlphaSub >= 127) p_Prof->ulAlphaSub = 127;
	p_Prof->ulAlphaSub <<= 24;


	GFX_Profiler_Validate(p_Prof);
}



/*$4
 ***********************************************************************************************************************
    External Functions
 ***********************************************************************************************************************
 */
#ifdef USE_DOUBLE_RENDERING	
void  GFX_Profiler_Interpolate( void *p_Data , u_int Mode , float fInterpoler)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Profiler	*pst_AF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_AF = (GFX_tdst_Profiler *)p_Data;

	if (Mode < 100)
	// Render I
	{
			if (Mode == 0)
			{
				pst_AF->GX2 = pst_AF->GX1;
				pst_AF->GY2 = pst_AF->GY1;
				pst_AF->GZ2 = pst_AF->GZ1;
				pst_AF->GT2 = pst_AF->GT1;
				pst_AF->GX1 = pst_AF->GX;
				pst_AF->GY1 = pst_AF->GY;
				pst_AF->GZ1 = pst_AF->GZ;
				pst_AF->GT1 = pst_AF->GT;
			}
			MATH_BlendVector(&pst_AF->GX , &pst_AF->GX2 , &pst_AF->GX1 , fInterpoler);
			MATH_BlendVector(&pst_AF->GY , &pst_AF->GY2 , &pst_AF->GY1 , fInterpoler);
			MATH_BlendVector(&pst_AF->GZ , &pst_AF->GZ2 , &pst_AF->GZ1 , fInterpoler);
			MATH_BlendVector(&pst_AF->GT , &pst_AF->GT2 , &pst_AF->GT1 , fInterpoler);
		} else
	// Render K
	if (Mode == 101)
	{
		pst_AF->GX1  = pst_AF->GX;
		pst_AF->GY1  = pst_AF->GY;
		pst_AF->GZ1  = pst_AF->GZ;
		pst_AF->GT1  = pst_AF->GT;
	} else 
	{
		pst_AF->GX  = pst_AF->GX1;
		pst_AF->GY  = pst_AF->GY1;
		pst_AF->GZ  = pst_AF->GZ1;
		pst_AF->GT  = pst_AF->GT1;
	} 
    
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Profiler_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Profiler *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	ZeroMallocType(pst_Data , 1, GFX_tdst_Profiler);

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Profiler_Destroy(void *p_Data)
{
	GFX_tdst_Profiler *pProfiler;
	if (!p_Data) return;
	pProfiler = (GFX_tdst_Profiler *)p_Data;
	if (pProfiler->p_AllLinks)			MEM_Free(pProfiler->p_AllLinks);
	if (pProfiler->p_AllPoints)			MEM_Free(pProfiler->p_AllPoints);
	if (pProfiler->p_AllProfileStates)	MEM_Free(pProfiler->p_AllProfileStates);
	if (pProfiler->p_PointGenerator)	MEM_Free(pProfiler->p_PointGenerator);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define GFX_EXP_MEC_GRANUL 1.0f / 50.0f // 50 Hz maxi
int GFX_i_Profiler_Render(void *p_Data)
{
	GFX_tdst_Profiler *p_Prof;
	ULONG DM;

	p_Prof = (GFX_tdst_Profiler *)p_Data;
	if (!p_Prof->Valid) return 1;
#ifdef ACTIVE_EDITORS
	if (ENG_gb_EngineRunning)
#endif
	{
		float DT;
		DT = TIM_gf_dt;
		GFX_i_Profiler_UpdateMecanic(p_Prof,DT);
	}
	GFX_i_Profiler_Update_With_XYT(p_Prof);
	GFX_i_Profiler_ComputeMeshState(p_Prof);

	
	if (!p_Prof->ulNumberOfPoints) return 1;
	if (!p_Prof->ulNumberOfTriangles) return 1;

	GFX_NeedGeom(p_Prof->ulNumberOfPoints, p_Prof->ulNumberOfUV, p_Prof->ulNumberOfTriangles , 1);

	/* Compute Vertices */
	{
		GEO_Vertex                          *dst_Point;
		MATH_tdst_Vector					*dst_PointNormal;
		ULONG								*p_APS,*p_APSEnd;
		ULONG								*p_Color;
		GFX_tdst_Profiler_Point				*p_PPoint,*p_PPointLast;
 
		p_PPoint = p_Prof->p_AllPoints;

		p_Color = GFX_gpst_Geo->dul_PointColors + 1;

		dst_Point = GFX_gpst_Geo->dst_Point;
		dst_PointNormal = GFX_gpst_Geo->dst_PointNormal;

		p_APS = p_Prof->p_AllProfileStates;
		p_APSEnd = p_APS + p_Prof->ulNumberOfProfiles;

		while (p_APS < p_APSEnd)
		{
				if (*p_APS)
				{
					p_PPointLast = p_PPoint + p_Prof->ulNumberOfPointsPerProfile;
					while (p_PPoint < p_PPointLast)
					{
						*(dst_Point) = *AVCast(&p_PPoint->ActualPos);

						if(p_Prof->Flags & GFX_Profile_TextWater)
						{
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
							extern void WTR_Get_A_DifVector(MATH_tdst_Vector *pSrc, MATH_tdst_Vector *pDst, ULONG Converge);
							MATH_tdst_Vector tDest;
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

							WTR_Get_A_DifVector(dst_Point, &tDest, 1);
							dst_Point->z = tDest.z;
						}

						dst_Point++;
						*(dst_PointNormal++) = p_PPoint->ActualNormale;
						*(p_Color ++) = p_PPoint->ActualColor;
						p_PPoint++;
					}
				} else 
				{
					p_PPoint += p_Prof->ulNumberOfPointsPerProfile;
				}
				p_APS++;
			}
		}

	/* Compute UV */
	{
		GEO_tdst_UV							*dst_UV;
		GFX_tdst_Profiler_Link *p_tLink,*p_tLinkLast;
		ULONG UTileCounter;
		float UtileInc , CurrentU;
		UTileCounter = p_Prof->UTiler + 1;
 		p_tLink = p_Prof->p_AllLinks;
 		p_tLinkLast = p_Prof->p_AllLinks + p_Prof->ulNumberOfLinksPerProfile;
		UtileInc = 1.0f/ p_Prof->UTiler;
		CurrentU = 1.0f;

		dst_UV = GFX_gpst_Geo->dst_UV;
		while (UTileCounter--)
		{
			while (p_tLink < p_tLinkLast)
			{
				(dst_UV)->fU = CurrentU;
				(dst_UV+1)->fU = CurrentU;
				(dst_UV)->fV = p_tLink->fvA;
				(dst_UV+1)->fV = p_tLink->fvB;
				dst_UV+=2;
				p_tLink++;
			}
			p_tLink = p_Prof->p_AllLinks;
			CurrentU -= UtileInc;
		}
	}

	/* Compute triangles */
	{
		GEO_tdst_IndexedTriangle *p_Tri ;
		ULONG *p_APS,ulAPSMinusOne,*p_APSEnd;
		GFX_tdst_Profiler_Link *p_tLink,*p_tLinkLast;
			ULONG UVCounter;

		p_Tri = GFX_gpst_Geo->dst_Element->dst_Triangle;

		p_APS = p_Prof->p_AllProfileStates;
		p_APSEnd = p_APS + p_Prof->ulNumberOfProfiles;
		ulAPSMinusOne = *(p_APSEnd - 1);

 		p_tLinkLast = p_Prof->p_AllLinks + p_Prof->ulNumberOfLinksPerProfile;
		UVCounter = 0;

		while (p_APS < p_APSEnd)
		{
			if (*p_APS & ulAPSMinusOne) /* Only if profile n & n+1 are activated */
			{
				p_tLink = p_Prof->p_AllLinks;
				while (p_tLink < p_tLinkLast)
				{
					ULONG Base1,Base2;
					Base1 = (*p_APS >> 1) ;
					Base2 = (ulAPSMinusOne >> 1) ;
					p_Tri->auw_UV[0] = p_Tri->auw_UV[1] = p_Tri->auw_UV[2] = 0;
					p_Tri->auw_Index[0] = (unsigned short)(Base1 + p_tLink->A);
					p_Tri->auw_Index[1] = (unsigned short)(Base1 + p_tLink->B);
					p_Tri->auw_Index[2] = (unsigned short)(Base2 + p_tLink->B);
					p_Tri->auw_UV[0] = (unsigned short)(UVCounter + p_Prof->ulNumberOfLinksPerProfile * 2 );
					p_Tri->auw_UV[1] = (unsigned short)(UVCounter + p_Prof->ulNumberOfLinksPerProfile * 2  + 1);
					p_Tri->auw_UV[2] = (unsigned short)(UVCounter + 1);
					p_Tri++;
					p_Tri->auw_UV[0] = p_Tri->auw_UV[1] = p_Tri->auw_UV[2] = 0;
					p_Tri->auw_Index[0] = (unsigned short)(Base2 + p_tLink->B);
					p_Tri->auw_Index[1] = (unsigned short)(Base2 + p_tLink->A);
					p_Tri->auw_Index[2] = (unsigned short)(Base1 + p_tLink->A);
					p_Tri->auw_UV[0] = (unsigned short)(UVCounter + 1);
					p_Tri->auw_UV[1] = (unsigned short)(UVCounter );
					p_Tri->auw_UV[2] = (unsigned short)(UVCounter + p_Prof->ulNumberOfLinksPerProfile * 2 );
					p_Tri++;
					p_tLink++;
					UVCounter += 2;
				}
			} else UVCounter += p_Prof->ulNumberOfLinksPerProfile * 2;

			if (UVCounter == p_Prof->UTiler * p_Prof->ulNumberOfLinksPerProfile * 2) UVCounter = 0;
			ulAPSMinusOne = *(p_APS++);
		}

	}

    M_GFX_CheckGeom();

	/* REnder */
    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient /*| GDI_Cul_DM_NotWired*/);
	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;

	/* test si le flare est dans un secteur invisible */
	if(((OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner)->uc_LOD_Vis != 0)
	{
#if defined(ACTIVE_EDITORS)
		ULONG SaveK;

		switch (GDI_gpst_CurDD->ul_WiredMode & 3)
		{
		default:
		case 0:
			GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
			break;
		case 1:
			GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
		case 2:
			SaveK = GDI_gpst_CurDD->ul_ColorConstant;
			GDI_gpst_CurDD->ul_ColorConstant = 0xc0c0c0c0;
			GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_NotWired | GDI_Cul_DM_UseTexture | GDI_Cul_DM_DontForceColor);
			GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
			GDI_gpst_CurDD->ul_ColorConstant = SaveK;
			break;
		}
//#elif defined(_XBOX)
		// DO NOT RENDER TRAIL ON XBOX !!!
#else
		GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
#endif
	}
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;

    return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void	GFX_Profiler_Seti( void *p_Data, int i_Param, int i_Value )
{
	GFX_tdst_Profiler *p_Prof;
	p_Prof = (GFX_tdst_Profiler *)p_Data;
	i_Param -= GFX_ProfilerParamsi_First;
	switch (i_Param)
	{
	case 0:
		/* Set number of point per profile */
		GFX_SetNumberOfPPP(p_Prof,i_Value);
		GFX_Profiler_Validate(p_Prof);
		break;
	case 1:
		/* Set number of Link per profile */
		GFX_SetNumberOfLPP(p_Prof,i_Value);
		GFX_Profiler_Validate(p_Prof);
		break;
	case 2:
		/* Set number of profiles */
		GFX_SetNumberOfP(p_Prof , i_Value);
		GFX_Profiler_Validate(p_Prof);
		break;
	case 3:
		GFX_i_Profiler_StartStop(p_Prof , i_Value);
		break;
	case 4:
		p_Prof ->ulActualPointToSet = i_Value;
		break;
	case 5:
		p_Prof ->ulActualLinkToSet = i_Value;
		break;
	case 6:
#if defined( _MSC_VER )	
        ERR_X_Assert( i_Value >= 0 );
#ifdef JADEFUSION
        ERR_X_Assert( i_Value <= 65535u );
#else
		ERR_X_Assert( i_Value <= UINT16_MAX );
#endif
#endif        
		p_Prof->p_AllLinks[p_Prof ->ulActualLinkToSet].A = (unsigned short)i_Value;
		break;
	case 7:
#if defined( _MSC_VER )
        ERR_X_Assert( i_Value >= 0 );
 #ifdef JADEFUSION
		ERR_X_Assert( i_Value <= 65535u );
#else
		ERR_X_Assert( i_Value <= UINT16_MAX );
#endif
#endif        
		p_Prof->p_AllLinks[p_Prof ->ulActualLinkToSet].B = (unsigned short)i_Value;
		break;
	case 8:
#if defined( _MSC_VER )
        ERR_X_Assert( i_Value >= 0 );
#ifdef JADEFUSION
		ERR_X_Assert( i_Value <= 65535u );
#else
		ERR_X_Assert( i_Value <= UINT16_MAX );
#endif        
#endif
		p_Prof->p_AllLinks[p_Prof ->ulActualLinkToSet].MaterialID = (unsigned short)i_Value;
		break;
	case 9:
		p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].ulColorStart = i_Value;
		break;
	case 10:
		p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].ulColorEnd = i_Value;
		break;
	case 11:
		p_Prof->UTiler = i_Value;
		break;
	case 12:
		p_Prof->Flags = i_Value;
		break;
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void    GFX_Profiler_Setf( void *p_Data, int i_Param, float f_Value)
{
	GFX_tdst_Profiler *p_Prof;
	p_Prof = (GFX_tdst_Profiler *)p_Data;
	i_Param -= GFX_ProfilerParamsf_First;
	switch (i_Param)
	{
	case 0:
		p_Prof ->fGravity = f_Value;
		break;
	case 1:
		p_Prof->p_AllLinks[p_Prof ->ulActualLinkToSet].fvA = f_Value;
		break;
	case 2:
		p_Prof->p_AllLinks[p_Prof ->ulActualLinkToSet].fvB = f_Value;
		break;
	case 3:
		p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].PointConstraintZMin = f_Value;
		break;
	case 4:
		p_Prof->fSegmentSizeMin = f_Value;
		break;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void    GFX_Profiler_Setv( void *p_Data, int i_Param, MATH_tdst_Vector *V )
{
	GFX_tdst_Profiler *p_Prof;
	p_Prof = (GFX_tdst_Profiler *)p_Data;
	i_Param -= GFX_ProfilerParamsv_First;
	switch (i_Param)
	{
	case 0:
		p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].Pos = *V;
		break;
	case 1:
		p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].initialSpeed = *V;
		break;
	case 2:
		p_Prof->FrictionXYZ = *V;
		break;
	case 3: /* GX */
		p_Prof->GX = *V;
		break;
	case 4: /* GY */
		p_Prof->GY = *V;
		break;
	case 5: /* GZ */
		p_Prof->GZ = *V;
		break;
	case 6: /* GT */
		p_Prof->GT = *V;
		break;
	case 7:
		p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].RandomPos = *V;
		break;
	case 8:
		p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].RandominitialSpeed = *V;
		break;
	
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Vector *GFX_pst_Profiler_Getv( void *p_Data, int i_Param )
{
	GFX_tdst_Profiler *p_Prof;
	p_Prof = (GFX_tdst_Profiler *)p_Data;
	i_Param -= GFX_ProfilerParamsv_First;
	
	switch (i_Param)
	{
	case 0:
		return &p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].Pos;
	case 1:
		return &p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].initialSpeed;
	case 2:
		return &p_Prof->FrictionXYZ;
	case 3: /* GX */
		return &p_Prof->GX;
	case 4: /* GY */
		return &p_Prof->GY;
	case 5: /* GZ */
		return &p_Prof->GZ;
	case 6: /* GT */
		return &p_Prof->GT;
	case 7:
		return &p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].RandomPos;
	case 8:
		return &p_Prof->p_PointGenerator[p_Prof ->ulActualPointToSet].RandominitialSpeed;
	}
	return NULL;
}


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


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
#include "GFX/GFXExplode.h"
#include "GEOmetric/GEODebugObject.h"
#include "GRObject/GROrender.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "BASe/BENch/BENch.h"
#include "SOFT/SOFTzlist.h"

#define ZeroMalloc(Ptr , Number)\
	(void *)Ptr = MEM_p_Alloc(sizeof(*Ptr) * Number);\
	L_memset( (void *)Ptr , 0 , sizeof(*Ptr) * Number);

#define VERIFY_Explode(Ptr)\
	if (!Ptr->Valid) return;

#ifdef ACTIVE_EDITOR
#define GFX_CRASH(a) *(ULONG *)0 = *(ULONG *)0;
#else
#define GFX_CRASH(a)
#endif
static float fTimeRandomFactors[256];
_inline_ float GFX_Expl_Random(float f_Min,float f_Max,ULONG Base)
{
	return fRand(f_Min , f_Max);
//	return f_Min + (f_Max - f_Min) * fTimeRandomFactors[Base & 255];
}

#ifndef PSX2_TARGET
#define powf pow
#endif

/*$4
 ***********************************************************************************************************************
    Internal Functions
 ***********************************************************************************************************************
 */

#define IS_Finished(A) ((*(ULONG *)A) == 1)
void GFX_Explode_Generate(GFX_tdst_Explode *pExplode)
{
	float *p_FirstSPR,*p_LastSPR, norm;
	SOFT_tdst_AVertex *p_Pos , *p_Speed;
	ULONG RCount,*p_Colors;
    int i = 0;

	RCount = lFloatToLong(fRand(0.0,255.0));
	p_Colors = pExplode->p_Colors;
	pExplode->fTimeElapsed += TIM_gf_dt;
	p_Pos = pExplode->p_Pos___SZ;
	p_Speed = pExplode->p_Speed_GS;

	if ((pExplode->fLastTimeCreation + pExplode->fGenerationRate > pExplode->fTimeElapsed) || (!pExplode->ulNumberToGenerate)) return;

	p_FirstSPR = pExplode->p_TimeElapsed;
	p_LastSPR = p_FirstSPR + pExplode->ulNumberOfSprites;
	while (p_FirstSPR < p_LastSPR)
	{
		if (IS_Finished(p_FirstSPR))
		{
			MATH_tdst_Vector stRndStart;
			*(MATH_tdst_Vector *)p_Pos 	= pExplode->CreationPos;
			MATH_ScaleVector(&stRndStart  , &pExplode->CreationPosVolumeX , GFX_Expl_Random(-1.0f , 1.0f , RCount++));
			MATH_AddScaleVector(&stRndStart  , &stRndStart  , &pExplode->CreationPosVolumeY , GFX_Expl_Random(-1.0f , 1.0f , RCount++));
			MATH_AddScaleVector(&stRndStart  , &stRndStart  , &pExplode->CreationPosVolumeZ , GFX_Expl_Random(-1.0f , 1.0f , RCount++));
			MATH_AddVector((MATH_tdst_Vector *)p_Pos  , (MATH_tdst_Vector *)p_Pos , &stRndStart  );
			p_Pos->w			= GFX_Expl_Random(pExplode->CreationSizeMin,pExplode->CreationSizeMax,RCount++);
			*p_Colors			= pExplode->ColorFase1;
			p_Speed->x	= GFX_Expl_Random(pExplode->SpeedMin.x,pExplode->SpeedMax.x,RCount++);
			p_Speed->y	= GFX_Expl_Random(pExplode->SpeedMin.y,pExplode->SpeedMax.y,RCount++);
			p_Speed->z	= GFX_Expl_Random(pExplode->SpeedMin.z,pExplode->SpeedMax.z,RCount++);
			p_Speed->w	= GFX_Expl_Random(pExplode->GrowingSpeedMin,pExplode->GrowingSpeedMax,RCount++);
			MATH_SubVector((MATH_tdst_Vector *)p_Speed  , (MATH_tdst_Vector *) p_Speed  , &stRndStart  );
			MATH_AddVector((MATH_tdst_Vector *)p_Speed  , (MATH_tdst_Vector *) p_Speed  , &pExplode->MainPosSpeed);

            norm = MATH_f_SqrNormVector( (MATH_tdst_Vector *)p_Speed );
            if ( norm )
                MATH_ScaleEqualVector( (MATH_tdst_Vector *)p_Speed, GFX_Expl_Random(pExplode->NormSpeedMin,pExplode->NormSpeedMax,RCount++) * fOptInvSqrt( norm ) );
			//MATH_SetNormVector((MATH_tdst_Vector *)p_Speed ,(MATH_tdst_Vector *) p_Speed  ,GFX_Expl_Random(pExplode->NormSpeedMin,pExplode->NormSpeedMax,RCount++));
            if (pExplode->ulRotationEnable)
            {
                if (pExplode->fMaxRotation)
                {
                    float fCurrentRotation;

                    if (pExplode->fMinRotation < -1 || pExplode->fMinRotation > 1)
                    {
                        pExplode->fMinRotation = -1.0f;
                    }
                    if (pExplode->fMaxRotation < -1 || pExplode->fMaxRotation > 1)
                    {
                        pExplode->fMinRotation = 1.0f;
                    }

                    fCurrentRotation = GFX_Expl_Random(1024.0f * pExplode->fMinRotation,1024.0f * pExplode->fMaxRotation,RCount++);

                    if (fCurrentRotation < 0)
                    {
                        fCurrentRotation += 1024.0f;
                    }
                    
                    pExplode->p_usRotation[i] = (USHORT)(fCurrentRotation);
                }				
                else
                {
                    pExplode->p_usRotation[i] = 0;
                }

                if (pExplode->fAngularSpeedMax)
                {
                    pExplode->p_fAngularSpeed[i] = GFX_Expl_Random(pExplode->fAngularSpeedMin,pExplode->fAngularSpeedMax,RCount++);
                }
                else
                {
                    pExplode->p_fAngularSpeed[i] = 0;
                }
            }

			*p_FirstSPR		= 0.0f;
			pExplode->ulNumberToGenerate--;
			pExplode->ulNumberOfValidSprites++;
	        if((pExplode->ulNumberOfValidSprites*4)> SOFT_Cul_ComputingBufferSize) 
	        {
                pExplode->ulNumberOfValidSprites = SOFT_Cul_ComputingBufferSize/10;
#ifdef ACTIVE_EDITORS
                ERR_X_Warning(0, "The GFX \"explode\" size exceeds the RAM capacity", NULL);
#endif	
	        }

			if ((pExplode->fLastTimeCreation + pExplode->fGenerationRate > pExplode->fTimeElapsed) || (!pExplode->ulNumberToGenerate))
				p_FirstSPR = p_LastSPR;
			else					
				pExplode->fLastTimeCreation += pExplode->fGenerationRate;
		}
		RCount++;
		p_Pos++;
		p_Speed++;
		p_FirstSPR++;
		p_Colors++;
        i++;
	}
}
_inline_ void GFX_PreLoad(SOFT_tdst_AVertex *p_Fricion,SOFT_tdst_AVertex *p_Wind,float DT,float fGround,float fGravity,float OoDT)
{
#ifdef PSX2_TARGET
	ULONG tmp1,tmp2,tmp3,tmp4;
	asm __volatile__ {"
	.set noreorder
	mfc1    tmp1, DT
	mfc1    tmp2, fGround
	mfc1    tmp3, fGravity
	mfc1    tmp4, OoDT
	lqc2 	$vf18 , 0(p_Fricion)
	lqc2 	$vf19 , 0(p_Wind)
	qmtc2  	tmp1,$vf14
	qmtc2  	tmp2,$vf15
	qmtc2  	tmp3,$vf16
	qmtc2  	tmp4,$vf17
	.set reorder
	"}
#endif
}
#ifdef PSX2_TARGET
extern u_int 	VU0_Label_FirstLine         __attribute__((section(".vudata")));
extern u_int 	VU0_ComputeParticules		__attribute__((section(".vudata")));
#endif
_inline_ void GFX_FullMix(SOFT_tdst_AVertex *p_Pos,SOFT_tdst_AVertex *p_Speed,SOFT_tdst_AVertex *p_Fricion,SOFT_tdst_AVertex *p_Wind,ULONG Counter,float DT,float fGround,float fGravbity,float OoDT)
{
#ifdef PSX2_TARGET
	register u_long128 tmp8,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
	register u_int Addr;
	Addr = 	(u_int)&VU0_ComputeParticules;
	Addr -=	(u_int)&VU0_Label_FirstLine;
	Addr >>=3;
	asm __volatile__ {"
	.set noreorder
	beq		Counter, zero, end
	nop
	ctc2.ni	Addr,$vi27
	vnop
	vnop
	lqc2 	$vf01 , 0x00(p_Speed)
	lqc2 	$vf02 , 0x10(p_Speed)
	lqc2 	$vf03 , 0x20(p_Speed)
	lqc2 	$vf04 , 0x30(p_Speed)
	lqc2 	$vf05 , 0x00(p_Pos)
	lqc2 	$vf06 , 0x10(p_Pos)
	lqc2 	$vf07 , 0x20(p_Pos)
	lqc2 	$vf08 , 0x30(p_Pos)
	vcallmsr $vi27
	vnop
	vnop
loop:	
	addiu	Counter,Counter, -1
	pref	0,0x80(p_Pos)
	nop
	nop
	nop
	nop"}
asm __volatile__ {"
	.set noreorder
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	pref	0,0x80(p_Speed)
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	qmfc2.i	tmp1 , $vf01
	qmfc2	tmp2 , $vf02
	qmfc2	tmp3 , $vf03
	qmfc2	tmp4 , $vf04
	qmfc2	tmp5 , $vf05
	qmfc2	tmp6 , $vf06
	qmfc2	tmp7 , $vf07
	qmfc2	tmp8 , $vf08
	lqc2 	$vf01 , 0x40(p_Speed)
	lqc2 	$vf02 , 0x50(p_Speed)
	lqc2 	$vf03 , 0x60(p_Speed)
	lqc2 	$vf04 , 0x70(p_Speed)
	lqc2 	$vf05 , 0x40(p_Pos)
	lqc2 	$vf06 , 0x50(p_Pos)
	lqc2 	$vf07 , 0x60(p_Pos)
	lqc2 	$vf08 , 0x70(p_Pos)
	vcallmsr $vi27
	sq 		tmp5 , 0x00(p_Pos)
	beq		Counter, zero, end
	sq   	tmp1 , 0x00(p_Speed)
	addiu	Counter,Counter, -1
	sq   	tmp6 , 0x10(p_Pos)
	beq		Counter, zero, end
	sq   	tmp2 , 0x10(p_Speed)
	addiu	Counter,Counter, -1
	sq   	tmp7 , 0x20(p_Pos)
	beq		Counter, zero, end
	sq   	tmp3 , 0x20(p_Speed)
	addiu	Counter,Counter, -1
	sq   	tmp8 , 0x30(p_Pos)
	beq		Counter, zero, end
	sq   	tmp4 , 0x30(p_Speed)
	addiu	p_Speed,p_Speed,0x40
	bne		Counter, zero, loop
	addiu	p_Pos,p_Pos,0x40
end:
	.set reorder
	"}
#else
	float Sub0;
	while (Counter--)
	{
		p_Pos->x += DT * (p_Wind->x + p_Speed->x);
		p_Pos->y += DT * (p_Wind->y + p_Speed->y);
		p_Pos->z += DT * (p_Wind->z + p_Speed->z);
		p_Pos->w += DT * (p_Wind->w + p_Speed->w);
		p_Speed->x *= p_Fricion->x;
		p_Speed->y *= p_Fricion->y;
		p_Speed->z *= p_Fricion->z;
		p_Speed->w *= p_Fricion->w;
		Sub0 = fMax(0.0f , fGround - p_Pos->z);
		p_Pos->z += Sub0;
		p_Speed->z += fGravbity + Sub0 * OoDT;
		p_Speed++;
		p_Pos++;
	}
#endif
}
extern ULONG LIGHT_ul_Interpol2Colors(ULONG ulP1, ULONG ulP2, float fZClipLocalCoef);
void GFX_Explode_FastMeca(GFX_tdst_Explode *pExplode,SOFT_tdst_AVertex *p_Fricion,SOFT_tdst_AVertex *p_Wind,float DT,float OoDT,float fGravbity,float fGround)
{
	SOFT_tdst_AVertex *p_Pos , *p_PosLast , *p_Speed;
	p_Pos = pExplode->p_Pos___SZ;
	p_Speed = pExplode->p_Speed_GS;
	p_PosLast = p_Pos + pExplode->ulNumberOfSprites;
	GFX_PreLoad(p_Fricion,p_Wind,DT,fGround,fGravbity,OoDT);
	GFX_FullMix(p_Pos,p_Speed,p_Fricion,p_Wind,pExplode->ulNumberOfSprites,DT,fGround,fGravbity,OoDT);
}

void GFX_Explode_Mecanic(GFX_tdst_Explode *pExplode , float DT)
{
	float *p_FirstSPR,*p_LastSPR;
	SOFT_tdst_AVertex *p_Pos,*p_Speed;
	SOFT_tdst_AVertex stFrictionSpeed ONLY_PSX2_ALIGNED(16);
	ULONG	*p_Colors;
	ULONG RCount;
	register float fTimeCoef;
	ULONG TimeInFase2,TimeInFase1;
    int     nbFinalParticle = 0;
	RCount = 0;
    
	
	{
		float fFactor;
		fFactor = DT / (1.0f / 60.0f);
		stFrictionSpeed = pExplode->FrictionSpeed_FG;
		stFrictionSpeed.x = (float)powf(stFrictionSpeed.x , fFactor);
		stFrictionSpeed.y = (float)powf(stFrictionSpeed.y , fFactor);
		stFrictionSpeed.z = (float)powf(stFrictionSpeed.z , fFactor);
		stFrictionSpeed.w = (float)powf(stFrictionSpeed.w , fFactor);
	}
	
	p_Pos = pExplode->p_Pos___SZ;
	p_Speed = pExplode->p_Speed_GS;
	p_Colors = pExplode->p_Colors;

	*(float *)&TimeInFase2 = pExplode->TimeInFase1 + pExplode->TimeInFase2;
	*(float *)&TimeInFase1 = pExplode->TimeInFase1;
	fTimeCoef = DT;
	p_FirstSPR = pExplode->p_TimeElapsed;
	p_LastSPR = p_FirstSPR + pExplode->ulNumberOfSprites;
	GFX_Explode_FastMeca(pExplode,&stFrictionSpeed,&pExplode->Wind_0,DT,2.0f / DT,pExplode->Gravity * DT,pExplode->fGround);
	while (p_FirstSPR < p_LastSPR)
	{
		if (!IS_Finished(p_FirstSPR))
		{
			if (*(ULONG *)p_FirstSPR > TimeInFase2)
			{                
                if (pExplode->ulRotationEnable)
                {
                    nbFinalParticle++;
                }
                else
                {
                    *(ULONG *)p_FirstSPR = 1;
                }

				*(ULONG *)&p_Pos->w = 0; //Size = 0;
				*(ULONG *)&p_Speed->w = 0; //Size Speed = 0;
				*p_Colors = 0;
			} else
			{
				if (*(ULONG *)p_FirstSPR > TimeInFase1)
				{
					*p_Colors = pExplode->ulColorTable_012[256 + lMin((lFloatToLong((*p_FirstSPR - pExplode->TimeInFase1) * pExplode->OoTimeInFase2)),255)];//
				} else
				{
					*p_Colors =pExplode->ulColorTable_012[ lMin(lFloatToLong(*p_FirstSPR * pExplode->OoTimeInFase1),256)];//
				}
				*p_FirstSPR += fTimeCoef * fRand(1.0, pExplode->TimeRandomFactor);				
			}
		}
		p_Colors++;
		p_Pos++;
		p_Speed++;
		p_FirstSPR++;
	}//*/

    if (pExplode->ulRotationEnable && nbFinalParticle == pExplode->ulNumberOfValidSprites)
    {
        p_FirstSPR = pExplode->p_TimeElapsed;
        p_LastSPR = p_FirstSPR + pExplode->ulNumberOfSprites;

        while (p_FirstSPR < p_LastSPR)
        {
            if (!IS_Finished(p_FirstSPR))
            {
                *(ULONG *)p_FirstSPR = 1;
            }

            p_FirstSPR++;
        }
    }

	MATH_AddScaleVector(&pExplode->CreationPos , &pExplode->CreationPos , &pExplode->MainPosSpeed , fTimeCoef);
	pExplode->MainPosSpeed .x *= pExplode->MainPosFriction.x;
	pExplode->MainPosSpeed .y *= pExplode->MainPosFriction.y;
	pExplode->MainPosSpeed .z *= pExplode->MainPosFriction.z;
}




/*$4
 ***********************************************************************************************************************
    External Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Explode_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Explode *pst_Data;
	static int bFirst = 1;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	pst_Data = (GFX_tdst_Explode*)MEM_p_AllocAlign(sizeof(GFX_tdst_Explode) , 16);
	L_memset(pst_Data , 0 , sizeof(GFX_tdst_Explode));
	if (bFirst)
	{
		ULONG RCount;
		RCount = 256;
		while (RCount --)
		{
			fTimeRandomFactors [RCount] = fRand(0.0f,1.0f);
		}
		
	}

	return (void *) pst_Data;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Explode_Render(void *p_Data,GRO_tdst_Struct *p_Material)
{
	GEO_tdst_IndexedTriangle	*p_Tri ;
	GEO_tdst_IndexedTriangle	*p_Tri4[4] ;
	GFX_tdst_Explode			*pExplode;
	MATH_tdst_Vector			*dst_PointNormal,stScreenX,stScreenY;
	unsigned short				usCurrentIndex;
	GEO_Vertex					*dst_Point;
	ULONG						*p_Color ;


    ULONG TriCounter,ElementCounter;
	extern SOFT_tdst_ZList_CommonParrams *SOFT_gst_ZList_CP;

	pExplode = (GFX_tdst_Explode *)p_Data;

	GFX_Explode_Generate(pExplode);

/****************************/
/* First pass compute nspr	*/
/****************************/

	{
		float *p_Spr,*p_SprLast;
		ULONG *p_Colors;
		p_Colors = pExplode->p_Colors;
		p_Spr = pExplode->p_TimeElapsed;
		p_SprLast = p_Spr + pExplode->ulNumberOfSprites;
		pExplode->ulNumberOfValidSprites = 0;
		/* For each sprite */
		while (p_Spr < p_SprLast)
		{
			if (!IS_Finished(p_Spr))// No color mean invisible
			{
				pExplode->ulNumberOfValidSprites++;
	            if((pExplode->ulNumberOfValidSprites*4)> SOFT_Cul_ComputingBufferSize) 
	            {
                    pExplode->ulNumberOfValidSprites = SOFT_Cul_ComputingBufferSize/10;
#ifdef ACTIVE_EDITORS
                    ERR_X_Warning(0, "The GFX \"explode\" size exceeds the RAM capacity", NULL);
#endif	
	            }
			}	
			p_Spr++;
			p_Colors++;
		}
	}


/****************************/
/* Destroy if no sprites	*/
/****************************/

	if (!pExplode->ulNumberOfValidSprites) 
	{
		if (!pExplode->ulNumberToGenerate)
			return 0; /* Destroy mesh */
		else
			return 1; 
	}

    /*
    if(((OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner)->uc_LOD_Vis == 0)
    {
      	if (ENG_gb_EngineRunning) 
			GFX_Explode_Mecanic(pExplode , TIM_gf_dt);
        return 1;
    }
    */
        

/****************************/
/* Second pass compute MESH	*/
/****************************/

	if (!pExplode->ulSortEnable/* && !pExplode->ulRotationEnable*/)
	{
nosort:
		_GSP_EndRaster(7); 
		if ((pExplode->p_Owner) &&
			(pExplode->p_Owner->pst_Base) &&
			(pExplode->p_Owner->pst_Base->pst_Visu))
		{
			pExplode->p_Owner->pst_Base->pst_Visu->pst_Material = p_Material;
			pExplode->p_Owner->pst_Base->pst_Visu->ul_DrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_Lighted | GDI_Cul_DM_UseRLI);
			GRO_Render(pExplode->p_Owner);
		}
		_GSP_BeginRaster(7);
	}
//*/
	else
	{

		/* Do not sort if too many triangles */
		if (pExplode->ZOrder & 0x80000000)
			SOFT_ZList_SelectZListe(pExplode->ZOrder & 0xff);
		else
			SOFT_ZList_SelectZListe(0);
		if( ((pExplode->ulNumberOfValidSprites * 2) + 32) + (SOFT_gst_ZList_CP->pst_CurrentVertex - SOFT_gst_ZList_CP->ast_Vertex) > SOFT_l_MaxNode
            /*&& !pExplode->ulRotationEnable*/)
		{
			goto nosort;
		}

		/* Get the mesh */
		GFX_NeedGeomEx(pExplode->ulNumberOfValidSprites*4, 4,4, ((pExplode->ulNumberOfValidSprites>>1) + 8) , 1);
		GFX_NeedGeomExElem(1,((pExplode->ulNumberOfValidSprites>>1) + 8));
		GFX_NeedGeomExElem(2,((pExplode->ulNumberOfValidSprites>>1) + 8));
		GFX_NeedGeomExElem(3,((pExplode->ulNumberOfValidSprites>>1) + 8));

		/* UV */
		GFX_gpst_Geo->dst_UV[0].fU = 0.0f;
		GFX_gpst_Geo->dst_UV[0].fV = 0.0f;
		GFX_gpst_Geo->dst_UV[1].fU = 1.0f;
		GFX_gpst_Geo->dst_UV[1].fV = 0.0f;
		GFX_gpst_Geo->dst_UV[2].fU = 1.0f;
		GFX_gpst_Geo->dst_UV[2].fV = 1.0f;
		GFX_gpst_Geo->dst_UV[3].fU = 0.0f;
		GFX_gpst_Geo->dst_UV[3].fV = 1.0f;

		GFX_gpst_Geo->dst_Element[0].l_MaterialId = pExplode->SubMaterial1;
		GFX_gpst_Geo->dst_Element[1].l_MaterialId = pExplode->SubMaterial2;
		GFX_gpst_Geo->dst_Element[2].l_MaterialId = pExplode->SubMaterial3;
		GFX_gpst_Geo->dst_Element[3].l_MaterialId = pExplode->SubMaterial4;

		/* Point, colors & triangles */
		dst_Point		= GFX_gpst_Geo->dst_Point;
		dst_PointNormal = GFX_gpst_Geo->dst_PointNormal;
		p_Color			= GFX_gpst_Geo->dul_PointColors + 1;
		p_Tri			= GFX_gpst_Geo->dst_Element->dst_Triangle;
		usCurrentIndex	= 0;
		*GFX_gpst_Geo->dul_PointColors = pExplode->ulNumberOfValidSprites * 4;

		stScreenX = *MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix );
		stScreenY = *MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix );

		{
			float *p_Spr,*p_SprLast;
			ULONG *p_Colors;
            ULONG ulLastColor, ulLastDividedColor;
			SOFT_tdst_AVertex 			*p_Pos ;
            int i = 0;
			
            ulLastColor = 0;
			p_Pos = pExplode->p_Pos___SZ; 
			
			p_Spr = pExplode->p_TimeElapsed;
			p_SprLast = p_Spr + pExplode->ulNumberOfSprites;
			p_Colors = pExplode->p_Colors;

			TriCounter = 0;
			p_Tri4[0] = GFX_gpst_Geo->dst_Element[0].dst_Triangle;
			p_Tri4[1] = GFX_gpst_Geo->dst_Element[1].dst_Triangle;
			p_Tri4[2] = GFX_gpst_Geo->dst_Element[2].dst_Triangle;
			p_Tri4[3] = GFX_gpst_Geo->dst_Element[3].dst_Triangle;
			ElementCounter = 0;
			/* For each sprite */
			while (p_Spr < p_SprLast)
			{
				if (!IS_Finished(p_Spr))// || (*p_Colors))// No color mean invisible
				{
					MATH_tdst_Vector p_SX,p_SY;
                    USHORT           usAngle;
                    float            f_Sin, f_Cos;

                    usAngle = ((USHORT)(pExplode->p_usRotation[i] + (pExplode->p_fAngularSpeed[i] * 1024.0f * pExplode->fTimeElapsed)))%1024;
                    
                    f_Sin = MATH_gf_TableSin[usAngle];
                    f_Cos = MATH_gf_TableSin[(usAngle + 256) % 1023];
                    MATH_ScaleVector(&p_SX, &stScreenX, f_Cos);
                    MATH_AddScaleVector(&p_SX, &p_SX, &stScreenY, -f_Sin);
                    MATH_ScaleVector(&p_SY, &stScreenY, f_Cos);
                    MATH_AddScaleVector(&p_SY, &p_SY, &stScreenX, f_Sin);               

					MATH_AddScaleVector(dst_Point,(MATH_tdst_Vector *)(p_Pos),&p_SX, +p_Pos->w);
					MATH_AddScaleVector(dst_Point,dst_Point  ,&p_SY, +p_Pos->w);
					dst_Point++;

					MATH_AddScaleVector(dst_Point,(MATH_tdst_Vector *)(p_Pos),&p_SX, +p_Pos->w);
					MATH_AddScaleVector(dst_Point,dst_Point  ,&p_SY, -p_Pos->w);
					dst_Point++;

					MATH_AddScaleVector(dst_Point,(MATH_tdst_Vector *)(p_Pos),&p_SX, -p_Pos->w); 
					MATH_AddScaleVector(dst_Point,dst_Point  ,&p_SY, -p_Pos->w); 
					dst_Point++;

					MATH_AddScaleVector(dst_Point,(MATH_tdst_Vector *)(p_Pos),&p_SX, -p_Pos->w); 
					MATH_AddScaleVector(dst_Point,dst_Point  ,&p_SY, +p_Pos->w); 
					dst_Point++;

					p_Color[0] = p_Color[1] = p_Color[2] = p_Color[3] = *p_Colors;

                    if (GDI_gpst_CurDD->GlobalMul2X) 
                    {
                        if (ulLastColor != *p_Color)
                        {
                            ulLastDividedColor = (*p_Colors & 0xFF000000) | ((*p_Colors & 0x00FEFEFE)>>1);
                            p_Color[0] = p_Color[1] = p_Color[2] = p_Color[3] = ulLastDividedColor;
                            ulLastColor = *p_Color;
                        }
                        else
                        {
                            p_Color[0] = p_Color[1] = p_Color[2] = p_Color[3] = ulLastDividedColor;
                        }
                        
                    }
                    else
                    {
					    p_Color[0] = p_Color[1] = p_Color[2] = p_Color[3] = *p_Colors;
                    }

					p_Color+=4;
					p_Tri = p_Tri4[TriCounter & 3] ;

					p_Tri->auw_Index[0] = usCurrentIndex;
					p_Tri->auw_Index[1] = usCurrentIndex + 1;
					p_Tri->auw_Index[2] = usCurrentIndex + 2;
					p_Tri->auw_UV[0] = 0;
					p_Tri->auw_UV[1] = 1;
					p_Tri->auw_UV[2] = 2;
					p_Tri++;

					p_Tri->auw_Index[0] = usCurrentIndex;
					p_Tri->auw_Index[1] = usCurrentIndex + 2;
					p_Tri->auw_Index[2] = usCurrentIndex + 3;
					p_Tri->auw_UV[0] = 0;
					p_Tri->auw_UV[1] = 2;
					p_Tri->auw_UV[2] = 3;
					p_Tri++;
					
					p_Tri4[TriCounter & 3] = p_Tri;
					TriCounter++;
					usCurrentIndex += 4;
				}	

				p_Colors++;
				p_Spr++;
				p_Pos++;
                i++;
			}
			GFX_gpst_Geo->dst_Element[0].l_NbTriangles = (LONG) (p_Tri4[0] - GFX_gpst_Geo->dst_Element[0].dst_Triangle );
			GFX_gpst_Geo->dst_Element[1].l_NbTriangles = (LONG) (p_Tri4[1] - GFX_gpst_Geo->dst_Element[1].dst_Triangle );
			GFX_gpst_Geo->dst_Element[2].l_NbTriangles = (LONG) (p_Tri4[2] - GFX_gpst_Geo->dst_Element[2].dst_Triangle );
			GFX_gpst_Geo->dst_Element[3].l_NbTriangles = (LONG) (p_Tri4[3] - GFX_gpst_Geo->dst_Element[3].dst_Triangle );

		}

        M_GFX_CheckGeom();

	/**********/
	/* RENDER */
	/**********/
		{
			ULONG DM;
			DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
			GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient /*| GDI_Cul_DM_NotWired*/ | GDI_Cul_DM_Lighted);
            if (GFX_gpst_Current->c_Flags & GFX_Transparent)
            {                 
                GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_RenderingTransparency;
            }

			if(((OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner)->uc_LOD_Vis != 0)
            {
                GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
            }
			
			GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
		}
	}
/*******************/
/* Compute mecanic */
/*******************/
	if (ENG_gb_EngineRunning) 
	{
		float LocalDT;
		LocalDT = TIM_gf_dt;
		while (LocalDT > 1.0f / 30.0f)
		{
			GFX_Explode_Mecanic(pExplode , 1.0f / 30.0f);
			LocalDT -= 1.0f / 30.0f;
		}
		GFX_Explode_Mecanic(pExplode , LocalDT );
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Explode_Destroy(void *p_Data)
{
	GFX_tdst_Explode *pExplode;
	if (!p_Data) return;
	pExplode = (GFX_tdst_Explode *)p_Data;
	if (pExplode->p_TimeElapsed) MEM_FreeAlign(pExplode->p_TimeElapsed);
	if (pExplode->p_Pos___SZ) MEM_FreeAlign(pExplode->p_Pos___SZ);
	if (pExplode->p_Speed_GS) MEM_FreeAlign(pExplode->p_Speed_GS);
    if (pExplode->p_usRotation) MEM_FreeAlign(pExplode->p_usRotation);
    if (pExplode->p_fAngularSpeed) MEM_FreeAlign(pExplode->p_fAngularSpeed);
#ifdef JADEFUSION
	if (pExplode->p_Colors)   MEM_Free((void *)((ULONG)pExplode->p_Colors - 32));//popoverif
#else
	if (pExplode->p_Colors)   MEM_Free((void *)((ULONG)pExplode->p_Colors - 16));
#endif
	/*
    if
	(	
		(pExplode->p_Owner)
	&&	(pExplode->p_Owner->pst_Base) 
	&&	(pExplode->p_Owner->pst_Base->pst_Visu) 
	&&	(pExplode->p_Owner->pst_Base->pst_Visu->pst_Material) 
	)
	{
		pExplode->p_Owner->pst_Base->pst_Visu->pst_Material->i->pfn_AddRef(pExplode->p_Owner->pst_Base->pst_Visu->pst_Material, 1);
	}
    */

	if (pExplode->p_Owner)	  
    {
        if ( (pExplode->p_Owner->pst_Base) && (pExplode->p_Owner->pst_Base->pst_Visu) )
        {
            pExplode->p_Owner->pst_Base->pst_Visu->pst_Material = NULL;
            pExplode->p_Owner->pst_Base->pst_Visu->pst_Object = NULL;
        }
        OBJ_GameObject_Remove(pExplode->p_Owner, 1);
    }
	pExplode->p_TimeElapsed = NULL;
	pExplode->p_Pos___SZ = NULL;
	pExplode->p_Speed_GS = NULL;
	pExplode->p_Colors = NULL;
	pExplode->p_Owner = NULL;
	
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_SetNumberOfSprites(GFX_tdst_Explode *p_Expld , ULONG ulNumberOfSprites)
{
	ULONG Quatre;
	ulNumberOfSprites = (ulNumberOfSprites + 8) & ~7; 
	p_Expld->ulNumberOfSprites = ulNumberOfSprites;
	if (p_Expld->p_TimeElapsed) MEM_FreeAlign(p_Expld->p_TimeElapsed);
	p_Expld->p_TimeElapsed = (float*)MEM_p_AllocAlign(sizeof(float) * (p_Expld->ulNumberOfSprites + 8) , 16);
	L_memset(p_Expld->p_TimeElapsed,0,sizeof(float) * p_Expld->ulNumberOfSprites);
	
	if (p_Expld->p_Pos___SZ) MEM_FreeAlign(p_Expld->p_Pos___SZ);
	p_Expld->p_Pos___SZ = (SOFT_tdst_AVertex*)MEM_p_AllocAlign(sizeof(SOFT_tdst_AVertex) * (p_Expld->ulNumberOfSprites + 8) , 16);
	L_memset(p_Expld->p_Pos___SZ,0,sizeof(SOFT_tdst_AVertex) * p_Expld->ulNumberOfSprites);
	
	if (p_Expld->p_Speed_GS) MEM_FreeAlign(p_Expld->p_Speed_GS);
	p_Expld->p_Speed_GS = (SOFT_tdst_AVertex*)MEM_p_AllocAlign(sizeof(SOFT_tdst_AVertex) * (p_Expld->ulNumberOfSprites + 8) , 16);
	L_memset(p_Expld->p_Speed_GS,0,sizeof(SOFT_tdst_AVertex) * p_Expld->ulNumberOfSprites);
	
    if (p_Expld->p_usRotation) MEM_FreeAlign(p_Expld->p_usRotation);
    p_Expld->p_usRotation = (USHORT* )MEM_p_AllocAlign(sizeof(float) * (p_Expld->ulNumberOfSprites + 8) , 16);
    L_memset(p_Expld->p_usRotation,0,sizeof(float) * p_Expld->ulNumberOfSprites);

    if (p_Expld->p_fAngularSpeed) MEM_FreeAlign(p_Expld->p_fAngularSpeed);
    p_Expld->p_fAngularSpeed = (float* )MEM_p_AllocAlign(sizeof(float) * (p_Expld->ulNumberOfSprites + 8) , 16);
    L_memset(p_Expld->p_fAngularSpeed,0,sizeof(float) * p_Expld->ulNumberOfSprites);

#ifdef JADEFUSION
	if (p_Expld->p_Colors) MEM_Free((void *)((ULONG)p_Expld->p_Colors - 32));//popoverif
	p_Expld->p_Colors = (ULONG*)MEM_p_AllocAlign(sizeof(ULONG) * (p_Expld->ulNumberOfSprites + 8) + 32, 16);
    *p_Expld->p_Colors = ulNumberOfSprites;
	*(ULONG *)&p_Expld->p_Colors += 32;
#else
	if (p_Expld->p_Colors) MEM_Free((void *)((ULONG)p_Expld->p_Colors - 16));
	p_Expld->p_Colors = (ULONG*)MEM_p_AllocAlign(sizeof(ULONG) * (p_Expld->ulNumberOfSprites + 8) + 16, 16);
	*(ULONG *)&p_Expld->p_Colors += 16;
	p_Expld->p_Colors[-1] = ulNumberOfSprites;
#endif	
	L_memset(p_Expld->p_Colors,0,sizeof(ULONG) * p_Expld->ulNumberOfSprites);
	
	Quatre = ulNumberOfSprites;
	while (Quatre--) *(ULONG *)&p_Expld->p_TimeElapsed[Quatre] = 1;
	p_Expld->fGround = -100000000000.0f;
	p_Expld->fLastTimeCreation = 0.0f;
	p_Expld->fTimeElapsed = 0.0f;
	
	if (p_Expld->p_Owner)	  
    {
		LINK_PrintStatusMsg("GFX_SetNumberOfSprites appelé 2X de suite dans la même trame pour un GFX_Explode !!!!");

        if ( (p_Expld->p_Owner->pst_Base) && (p_Expld->p_Owner->pst_Base->pst_Visu) )
        {
            p_Expld->p_Owner->pst_Base->pst_Visu->pst_Material = NULL;
            p_Expld->p_Owner->pst_Base->pst_Visu->pst_Object = NULL;
        }
        OBJ_GameObject_Remove(p_Expld->p_Owner, 1);
		p_Expld->p_Owner = NULL;
    }

	p_Expld->p_Owner = OBJ_GameObject_Create(OBJ_C_IdentityFlag_BaseObject | OBJ_C_IdentityFlag_Visu);
	p_Expld->p_Owner->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) &p_Expld->stObjectForEffects;
	p_Expld->p_Owner->pst_Base->pst_Visu->c_DisplayOrder = -3;
	
	/* Init Object Fields */
	GRO_Struct_Init(&p_Expld->stObjectForEffects.st_Id, GRO_Geometric);
	p_Expld->stObjectForEffects.l_NbPoints = ulNumberOfSprites;
	p_Expld->stObjectForEffects.dst_Point = (MATH_tdst_Vector *)p_Expld->p_Pos___SZ;
	p_Expld->stObjectForEffects.dst_PointNormal = (MATH_tdst_Vector *)p_Expld->p_Speed_GS;
	p_Expld->stObjectForEffects.dul_PointColors = p_Expld->p_Colors - 1;
#ifdef PSX2_TARGET
	p_Expld->stObjectForEffects.p_CompressedNormals = NULL;
#endif	
	p_Expld->stObjectForEffects.l_NbUVs = 0;
	p_Expld->stObjectForEffects.dst_UV = NULL;
	p_Expld->stObjectForEffects.l_NbElements = 0;
	p_Expld->stObjectForEffects.dst_Element = NULL;
	p_Expld->stObjectForEffects.pst_BV = NULL;
	p_Expld->stObjectForEffects.p_MRM_ObjectAdditionalInfo = NULL;
	p_Expld->stObjectForEffects.p_SKN_Objectponderation = NULL;
#ifdef ACTIVE_EDITORS
	p_Expld->stObjectForEffects.pst_SubObject = NULL;
	p_Expld->stObjectForEffects.ul_EditorFlags = 0;
#endif /* ACTIVE_EDITORS */
	p_Expld->stObjectForEffects.ulStripFlag = 0;
	p_Expld->stObjectForEffects.l_NbSpritesElements = 4;
	p_Expld->stObjectForEffects.dst_SpritesElements = p_Expld->stSpritesElements;
	p_Expld->stObjectForEffects.pst_OK3 = NULL;
	Quatre = 4;
	while (Quatre --)
	{
		p_Expld->stSpritesElements[Quatre] . l_NbSprites = ulNumberOfSprites >>2;
		p_Expld->stSpritesElements[Quatre] . l_MaterialId= p_Expld->SubMaterial1;
		
		p_Expld->stSpritesElements[Quatre] . dst_Sprite = &p_Expld->stISpr [Quatre << 1] ;
		p_Expld->stSpritesElements[Quatre] . dst_Sprite[0] . auw_Index = 0xC0DE;
		p_Expld->stSpritesElements[Quatre] . dst_Sprite[1] . auw_Index = (USHORT)(Quatre * (ulNumberOfSprites >>2));
		p_Expld->stSpritesElements[Quatre] . ul_NumberOfUsedIndex= 0;
		p_Expld->stSpritesElements[Quatre] . pus_ListOfUsedIndex= NULL;
		p_Expld->stSpritesElements[Quatre] . fGlobalSize	= 1.0f;
		p_Expld->stSpritesElements[Quatre] . fGlobalRatio	= 1.0f;
		
	}
	p_Expld->stSpritesElements[0] . l_MaterialId = p_Expld->SubMaterial1;
	p_Expld->stSpritesElements[1] . l_MaterialId = p_Expld->SubMaterial2;
	p_Expld->stSpritesElements[2] . l_MaterialId = p_Expld->SubMaterial3;
	p_Expld->stSpritesElements[3] . l_MaterialId = p_Expld->SubMaterial4;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void	GFX_Explode_Seti( void *p_Data, int i_Param, int i_Value )
{
	GFX_tdst_Explode *p_Expld;
	int i;
	p_Expld = (GFX_tdst_Explode *)p_Data;
	i_Param -= GFX_ExplodeParamsi_First;
	switch (i_Param)
	{
	case 0:
	    if((i_Value*4)> SOFT_Cul_ComputingBufferSize) 
	    {
            i_Value = SOFT_Cul_ComputingBufferSize/10;
#ifdef ACTIVE_EDITORS
            ERR_X_Warning(0, "The GFX \"explode\" size exceeds the RAM capacity", NULL);
#endif	
        }
		GFX_SetNumberOfSprites(p_Expld , i_Value);
		break;
	case 1:
		p_Expld->SubMaterial4 = 
		p_Expld->SubMaterial3 = 
		p_Expld->SubMaterial2 = 
		p_Expld->SubMaterial1 = i_Value;
		{
			ULONG Quatre;
			Quatre = 4;
			while (Quatre --)
			{
				p_Expld->stSpritesElements[Quatre] . l_MaterialId = p_Expld->SubMaterial1;
			}
		}
		break;
	case 2:
		p_Expld->SubMaterial2 = i_Value;
		break;
	case 3:
#ifdef PSX2_TARGET
//		i_Value = (i_Value & 0xffffff) + ((i_Value & 0xfe000000) >> 1);
#endif		
		p_Expld->ColorFase1 = i_Value;
		for (i = 0 ; i < 256 ; i ++)
			p_Expld->ulColorTable_012[i] = LIGHT_ul_Interpol2Colors(p_Expld->ColorFase1,p_Expld->ColorFase2,(float)i / 256.0f);
		break;
	case 4:
#ifdef PSX2_TARGET
//		i_Value = (i_Value & 0xffffff) + ((i_Value & 0xfe000000) >> 1);
#endif		
		p_Expld->ColorFase2 = i_Value;
		for (i = 0 ; i < 256 ; i ++)
		{
			p_Expld->ulColorTable_012[i] = LIGHT_ul_Interpol2Colors(p_Expld->ColorFase1,p_Expld->ColorFase2,(float)i / 256.0f);
			p_Expld->ulColorTable_012[i + 256] = LIGHT_ul_Interpol2Colors(p_Expld->ColorFase2,p_Expld->ColorFase3,(float)i / 256.0f);
		}
		break;
	case 5:
#ifdef PSX2_TARGET
//		i_Value = (i_Value & 0xffffff) + ((i_Value & 0xfe000000) >> 1);
#endif
		p_Expld->ColorFase3 = i_Value;
		for (i = 0 ; i < 256 ; i ++)
			p_Expld->ulColorTable_012[i + 256] = LIGHT_ul_Interpol2Colors(p_Expld->ColorFase2,p_Expld->ColorFase3,(float)i / 256.0f);
		break;
	case 6:
		p_Expld->ulNumberToGenerate = i_Value;
		break;
	case 7:
		p_Expld->ulSortEnable = i_Value;
		break;
	case 10:
		p_Expld->SubMaterial1 = i_Value;
		p_Expld->stSpritesElements[0] . l_MaterialId = p_Expld->SubMaterial1;
		break;
	case 11:
		p_Expld->SubMaterial2 = i_Value;
		p_Expld->stSpritesElements[1] . l_MaterialId = p_Expld->SubMaterial2;
		break;
	case 12:
		p_Expld->SubMaterial3 = i_Value;
		p_Expld->stSpritesElements[2] . l_MaterialId = p_Expld->SubMaterial3;
		break;
	case 13:
		p_Expld->SubMaterial4 = i_Value;
		p_Expld->stSpritesElements[3] . l_MaterialId = p_Expld->SubMaterial4;
		break;
    case 14:
        p_Expld->ulRotationEnable = i_Value;  
        break;
    case 15:
		if (i_Value < -3) i_Value = -3;
		if (i_Value > 4) i_Value = 4;
        p_Expld->ZOrder = (i_Value + 3) | 0x80000000;  
        break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Explode_Geti( void *p_Data, int i_Param )
{
    GFX_tdst_Explode    *p_Expld;
	
    p_Expld = (GFX_tdst_Explode *)p_Data;
	i_Param -= GFX_ExplodeParamsi_First;
	switch (i_Param)
    {
	case 0:
        return p_Expld->ulNumberOfSprites;
	case 1:
		return p_Expld->SubMaterial4;
	case 2:
		return p_Expld->SubMaterial2;
	case 3:
#ifdef PSX2_TARGET
        return (p_Expld->ColorFase1 << 1);
#endif		
		return p_Expld->ColorFase1;
	case 4:
#ifdef PSX2_TARGET
		return (p_Expld->ColorFase2 << 1);
#endif		
		return p_Expld->ColorFase2;
    }
    return 0;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void    GFX_Explode_Setf( void *p_Data, int i_Param, float f_Value)
{
	GFX_tdst_Explode *p_Expld;
	p_Expld = (GFX_tdst_Explode *)p_Data;
	i_Param -= GFX_ExplodeParamsf_First;
	switch (i_Param)
	{
	case 0:
		p_Expld->GrowingSpeedMin = f_Value;
		break;
	case 1:
		p_Expld->GrowingSpeedMax = f_Value;
		break;
	case 2:
		p_Expld->FrictionSpeed_FG.w = f_Value;
		p_Expld->FrictionSpeed_FG.w = 1.0f - p_Expld->FrictionSpeed_FG.w ;
		break;
	case 3:
		p_Expld->TimeInFase1 = f_Value;
#ifdef JADEFUSION
        p_Expld->OoTimeInFase1 = 10000.0f;
        if(f_Value > 0.0f )
        {
            p_Expld->OoTimeInFase1 = 256.0f / f_Value;
        }
#else
		p_Expld->OoTimeInFase1 = 256.0f / f_Value;
#endif
		break;
	case 4:
		p_Expld->TimeInFase2 = f_Value;
#ifdef JADEFUSION
		p_Expld->OoTimeInFase2 = 10000.0f;
        if( f_Value > 0.0f )
        {
            p_Expld->OoTimeInFase2 = 256.0f / f_Value;
        }
#else
		p_Expld->OoTimeInFase2 = 256.0f / f_Value;
#endif
		break;
	case 5:
		p_Expld->CreationSizeMin = f_Value;
		break;
	case 6:
		p_Expld->CreationSizeMax = f_Value;
		break;
	case 7:
		p_Expld->Gravity = f_Value;
		break;
	case 8:
		p_Expld->fGenerationRate = f_Value;
		break;
	case 9:
		p_Expld->NormSpeedMin = f_Value;
		break;
	case 10:
		p_Expld->NormSpeedMax = f_Value;
		break;
	case 11:
		p_Expld->fGround = f_Value;
		break;
	case 12:
		p_Expld->TimeRandomFactor = f_Value;
		break;
    case 13:
        p_Expld->fMinRotation = f_Value;  
		break;
    case 14:
        p_Expld->fMaxRotation = f_Value;
        break;
    case 15:
        p_Expld->fAngularSpeedMin = f_Value;
        break;
    case 16:
        p_Expld->fAngularSpeedMax = f_Value; 
        break;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */

void    GFX_Explode_Setv( void *p_Data, int i_Param, MATH_tdst_Vector *V )
{
	GFX_tdst_Explode *pExplode;
	pExplode = (GFX_tdst_Explode *)p_Data;
	i_Param -= GFX_ExplodeParamsv_First;
	switch (i_Param)
	{
	case 0:
		pExplode->CreationPos = *V;
		break;
	case 1:
		pExplode->SpeedMin = *V;
		break;
	case 2:
		pExplode->SpeedMax = *V;
		break;
	case 3:
		*(MATH_tdst_Vector *)&pExplode->FrictionSpeed_FG = *V;
		pExplode->FrictionSpeed_FG.x = 1.0f - pExplode->FrictionSpeed_FG.x;
		pExplode->FrictionSpeed_FG.y = 1.0f - pExplode->FrictionSpeed_FG.y;
		pExplode->FrictionSpeed_FG.z = 1.0f - pExplode->FrictionSpeed_FG.z;
		break;
	case 4:
		*(MATH_tdst_Vector *)&pExplode->Wind_0 = *V;
		pExplode->Wind_0.w = 0.0f;
		break;
	case 5:
		pExplode->MainPosSpeed = *V;
		break;
	case 6:
		pExplode->MainPosFriction = *V;
		pExplode->MainPosFriction.x = 1.0f - pExplode->MainPosFriction.x;
		pExplode->MainPosFriction.y = 1.0f - pExplode->MainPosFriction.y;
		pExplode->MainPosFriction.z = 1.0f - pExplode->MainPosFriction.z;
		break;
	case 7:
		pExplode->CreationPosVolumeX = *V;
		break;
		
	case 8:
		pExplode->CreationPosVolumeY = *V;
		break;
		
	case 9:
		pExplode->CreationPosVolumeZ = *V;
		break;
		
	}
}

extern "C" void GFX_CreateExplosion( float sizecoef, float Speedcoef, MATH_tdst_Vector *Position, OBJ_tdst_GameObject *pst_GO, void *p_Material )
{
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector stLocal;
	USHORT GFX;
    int result;
	float OoSpeedcoef;
	OoSpeedcoef = 1.0f / Speedcoef;
	//	if ((counter & 3) == 0)
	Position->z+=0.1f;
	// FLAMES
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

    result = GFX_i_Add(&pst_World->pst_GFX , 13 , pst_GO); // Create the boum;
    ERR_X_Assert( result != -1 ); // the boum has not to boum itself -- Max 6 Feb 2003
	GFX = (USHORT) result;
	//GFX = pst_World->pst_GFX->uw_Id;
	GFX_SetMaterial(pst_World->pst_GFX, GFX, (void *)p_Material);
	GFX_Seti(pst_World->pst_GFX , GFX, 13100, 10); // // Buffer number of sprite
	GFX_Seti(pst_World->pst_GFX , GFX, 13106, 10); // // number of sprite to generate
	GFX_Seti(pst_World->pst_GFX , GFX, 13101, 1); // Material sub-mat num
	GFX_Seti(pst_World->pst_GFX , GFX, 13103, 0); // // Color fase 0
	GFX_Seti(pst_World->pst_GFX , GFX, 13104, 0xffffff); // // Color fase 1
	GFX_Seti(pst_World->pst_GFX , GFX, 13105, 0); // // Color fase 2
	GFX_Setf(pst_World->pst_GFX , GFX, 13000, 0.8f * sizecoef * OoSpeedcoef ); // // Grwing speed min
	GFX_Setf(pst_World->pst_GFX , GFX, 13001, 0.9f * sizecoef * OoSpeedcoef ); // // Grwing speed max
	GFX_Setf(pst_World->pst_GFX , GFX, 13002, OoSpeedcoef  * 0.1f  ); // // Friction Grow
	GFX_Setf(pst_World->pst_GFX , GFX, 13003, 0.03f * Speedcoef); // // Time fase 1
	GFX_Setf(pst_World->pst_GFX , GFX, 13004, 0.03f * Speedcoef); // // Time fase 2
	GFX_Setf(pst_World->pst_GFX , GFX, 13005, 0.04f * sizecoef); // // Creation size min
	GFX_Setf(pst_World->pst_GFX , GFX, 13006, 0.04f * sizecoef ); // // Creation size max
	GFX_Setf(pst_World->pst_GFX , GFX, 13007, 0.15f * OoSpeedcoef * OoSpeedcoef ); // // Gravity
	GFX_Setf(pst_World->pst_GFX , GFX, 13008, 0.00005f * Speedcoef); // // generation rate
	GFX_Setf(pst_World->pst_GFX , GFX, 13009, 0.50f * sizecoef * OoSpeedcoef ); // // Norm speed min
	GFX_Setf(pst_World->pst_GFX , GFX, 13010, 0.60f * sizecoef * OoSpeedcoef ); // // Norm speed max
	GFX_Setf(pst_World->pst_GFX , GFX, 13011, Position->z - 0.1f); // // Ground
	GFX_Setv(pst_World->pst_GFX , GFX, 13200, Position); // // Creation Pos
	MATH_InitVector(&stLocal ,1.0f , 1.0f , 1.0f);
	GFX_Setv(pst_World->pst_GFX , GFX, 13201, &stLocal); // // Speed min
	MATH_InitVector(&stLocal ,-1.0f , -1.0f , 0.0f);
	GFX_Setv(pst_World->pst_GFX , GFX, 13202, &stLocal); // // Speed max
	MATH_InitVector(&stLocal ,OoSpeedcoef * 0.1f , OoSpeedcoef * 0.1f , OoSpeedcoef * 0.5f);
	GFX_Setv(pst_World->pst_GFX , GFX, 13203, &stLocal); // // friction speed
	GFX_FlagSet(pst_World->pst_GFX , GFX, 0 , 1);
	GFX_FlagSet(pst_World->pst_GFX , GFX, 2 , 1);
	Position->z-=0.0;
	// Sparks
	result = GFX_i_Add(&pst_World->pst_GFX , 13 , pst_GO); // Create the boum;
    ERR_X_Assert( result != -1 ); // the boum has not to boum itself -- Max 6 Feb 2003
    GFX = (USHORT) result;
	//GFX = pst_World->pst_GFX->uw_Id;
	GFX_SetMaterial(pst_World->pst_GFX, GFX, (void *)p_Material);
	GFX_Seti(pst_World->pst_GFX , GFX, 13100, 80); // // Buffer number of sprite
	GFX_Seti(pst_World->pst_GFX , GFX, 13106, 80); // // number of sprite to generate
	GFX_Seti(pst_World->pst_GFX , GFX, 13101, 2); // Material sub-mat num
	GFX_Seti(pst_World->pst_GFX , GFX, 13103, 0xffffffff); // // Color fase 0
	GFX_Seti(pst_World->pst_GFX , GFX, 13104, 0xffffffff ); // // Color fase 1
	GFX_Seti(pst_World->pst_GFX , GFX, 13105, 0); // // Color fase 2
	GFX_Setf(pst_World->pst_GFX , GFX, 13000, 0.0f * sizecoef * OoSpeedcoef ); // // Grwing speed min
	GFX_Setf(pst_World->pst_GFX , GFX, 13001, 0.0f * sizecoef * OoSpeedcoef ); // // Grwing speed max
	GFX_Setf(pst_World->pst_GFX , GFX, 13002, OoSpeedcoef  * 0.1f  ); // // Friction Grow
	GFX_Setf(pst_World->pst_GFX , GFX, 13003, 2.0f * Speedcoef); // // Time fase 1
	GFX_Setf(pst_World->pst_GFX , GFX, 13004, 2.0f * Speedcoef); // // Time fase 2
	GFX_Setf(pst_World->pst_GFX , GFX, 13005, 0.002f * sizecoef); // // Creation size min
	GFX_Setf(pst_World->pst_GFX , GFX, 13006, 0.005f  * sizecoef); // // Creation size max
	GFX_Setf(pst_World->pst_GFX , GFX, 13007, -2.5f * OoSpeedcoef * OoSpeedcoef ); // // Gravity
	GFX_Setf(pst_World->pst_GFX , GFX, 13008, 0.001f  * Speedcoef); // // generation rate
	GFX_Setf(pst_World->pst_GFX , GFX, 13009, 0.2f * sizecoef* OoSpeedcoef ); // // Norm speed min
	GFX_Setf(pst_World->pst_GFX , GFX, 13010, 0.5f * sizecoef* OoSpeedcoef ); // // Norm speed max
	GFX_Setf(pst_World->pst_GFX , GFX, 13011, Position->z - 0.5f  ); // // Ground
	GFX_Setv(pst_World->pst_GFX , GFX, 13200, Position); // // Creation Pos
	MATH_InitVector(&stLocal ,-1,-1,0.0);
	GFX_Setv(pst_World->pst_GFX , GFX, 13201, &stLocal ); // // Speed min
	MATH_InitVector(&stLocal ,1.0f,1.0f,2.0f);
	GFX_Setv(pst_World->pst_GFX , GFX, 13202, &stLocal); // // Speed max
	MATH_InitVector(&stLocal ,OoSpeedcoef  * 0.01f,OoSpeedcoef  * 0.01f,OoSpeedcoef  * 0.0001f );
	GFX_Setv(pst_World->pst_GFX , GFX, 13203, &stLocal); // // friction speed
	GFX_FlagSet(pst_World->pst_GFX , GFX, 0 , 1);
	GFX_FlagSet(pst_World->pst_GFX , GFX, 2 , 1);
	// SMOKE
	result = GFX_i_Add(&pst_World->pst_GFX , 13 , pst_GO); // Create the boum;
    ERR_X_Assert( result != -1 ); // the boum has not to boum itself -- Max 6 Feb 2003
    GFX = (USHORT) result;
	//GFX = pst_World->pst_GFX->uw_Id;
	GFX_SetMaterial(pst_World->pst_GFX, GFX, (void *)p_Material);
	GFX_Seti(pst_World->pst_GFX , GFX, 13100, 30); // // Buffer number of sprite
	GFX_Seti(pst_World->pst_GFX , GFX, 13106, 30); // // number of sprite to generate
	GFX_Seti(pst_World->pst_GFX , GFX, 13101, 1); // Material sub-mat num
	GFX_Seti(pst_World->pst_GFX , GFX, 13103, 0x0); // // Color fase 0
	GFX_Seti(pst_World->pst_GFX , GFX, 13104, 0xff000000); // // Color fase 1
	GFX_Seti(pst_World->pst_GFX , GFX, 13105, 0); // // Color fase 2
	GFX_Setf(pst_World->pst_GFX , GFX, 13000, 0.1f* sizecoef * OoSpeedcoef ); // // Grwing speed min
	GFX_Setf(pst_World->pst_GFX , GFX, 13001, 0.13f* sizecoef* OoSpeedcoef ); // // Grwing speed max
	GFX_Setf(pst_World->pst_GFX , GFX, 13002, OoSpeedcoef  * 0.02f  ); // // Friction Grow
	GFX_Setf(pst_World->pst_GFX , GFX, 13003, 0.3f * Speedcoef); // // Time fase 1
	GFX_Setf(pst_World->pst_GFX , GFX, 13004, 1.4f * Speedcoef); // // Time fase 2
	GFX_Setf(pst_World->pst_GFX , GFX, 13005, 0.05f* sizecoef); // // Creation size min
	GFX_Setf(pst_World->pst_GFX , GFX, 13006, 0.06f * sizecoef); // // Creation size max
	GFX_Setf(pst_World->pst_GFX , GFX, 13007, 0.005f* OoSpeedcoef * OoSpeedcoef ); // // Gravity
	GFX_Setf(pst_World->pst_GFX , GFX, 13008, 0.005f* Speedcoef); // // generation rate
	GFX_Setv(pst_World->pst_GFX , GFX, 13200, Position); // // Creation Pos
	GFX_Setf(pst_World->pst_GFX , GFX, 13009, 0.30f* sizecoef* OoSpeedcoef ); // // Norm speed min
	GFX_Setf(pst_World->pst_GFX , GFX, 13010, 0.60f* sizecoef* OoSpeedcoef ); // // Norm speed max
	MATH_InitVector(&stLocal ,1,1,0);
	GFX_Setv(pst_World->pst_GFX , GFX, 13201, &stLocal ); // // Speed min
	MATH_InitVector(&stLocal ,-1,-1,-0);
	GFX_Setv(pst_World->pst_GFX , GFX, 13202, &stLocal); // // Speed max
	MATH_InitVector(&stLocal ,OoSpeedcoef  * 0.05f,OoSpeedcoef  * 0.05f,OoSpeedcoef  * 0.00f );
	GFX_Setv(pst_World->pst_GFX , GFX, 13203, &stLocal); // // friction speed
	GFX_FlagSet(pst_World->pst_GFX , GFX, 0 , 1);
	GFX_FlagSet(pst_World->pst_GFX , GFX, 2 , 1);
	// FIRE BALL
	result = GFX_i_Add(&pst_World->pst_GFX , 13 , pst_GO); // Create the boum;
    ERR_X_Assert( result != -1 ); // the boum has not to boum itself -- Max 6 Feb 2003
    GFX = (USHORT) result;
	//GFX = pst_World->pst_GFX->uw_Id;
	GFX_SetMaterial(pst_World->pst_GFX, GFX, (void *)p_Material);
	GFX_Seti(pst_World->pst_GFX , GFX, 13100, 50); // // Buffer number of sprite
	GFX_Seti(pst_World->pst_GFX , GFX, 13106, 50); // // number of sprite to generate
	GFX_Seti(pst_World->pst_GFX , GFX, 13101, 0); // Material sub-mat num
	GFX_Seti(pst_World->pst_GFX , GFX, 13103, 0xffffff ); // // Color fase 0
	GFX_Seti(pst_World->pst_GFX , GFX, 13104, 0xff080808 ); // // Color fase 1
	GFX_Seti(pst_World->pst_GFX , GFX, 13105, 0); // // Color fase 2
	GFX_Setf(pst_World->pst_GFX , GFX, 13000, 0.15f* sizecoef*OoSpeedcoef ); // // Grwing speed min
	GFX_Setf(pst_World->pst_GFX , GFX, 13001, 0.20f* sizecoef*OoSpeedcoef ); // // Grwing speed max
	GFX_Setf(pst_World->pst_GFX , GFX, 13002, OoSpeedcoef  * 0.03f ); // // Friction Grow
	GFX_Setf(pst_World->pst_GFX , GFX, 13003, 1.2f     * Speedcoef ); // // Time fase 1
	GFX_Setf(pst_World->pst_GFX , GFX, 13004, 2.2f * Speedcoef); // // Time fase 2
	GFX_Setf(pst_World->pst_GFX , GFX, 13005, 0.020f* sizecoef); // // Creation size min
	GFX_Setf(pst_World->pst_GFX , GFX, 13006, 0.022f * sizecoef); // // Creation size max
	GFX_Setf(pst_World->pst_GFX , GFX, 13007, 0.02f* OoSpeedcoef * OoSpeedcoef ); // // Gravity
	GFX_Setf(pst_World->pst_GFX , GFX, 13008, 0.004f  * Speedcoef); // // generation rate
	GFX_Setv(pst_World->pst_GFX , GFX, 13200, Position); // // Creation Pos
	GFX_Setf(pst_World->pst_GFX , GFX, 13009, 0.20f* sizecoef* OoSpeedcoef ); // // Norm speed min
	GFX_Setf(pst_World->pst_GFX , GFX, 13010, 0.5f  * sizecoef* OoSpeedcoef ); // // Norm speed max
	GFX_Setf(pst_World->pst_GFX , GFX, 13012, 2.0f * OoSpeedcoef ); // // dephasage
	MATH_InitVector(&stLocal ,1.0f,1.0f,1.2f);
	GFX_Setv(pst_World->pst_GFX , GFX, 13201, &stLocal); // // Speed min
	MATH_InitVector(&stLocal ,-1.0f,-1.0f,0.0f);
	GFX_Setv(pst_World->pst_GFX , GFX, 13202, &stLocal); // // Speed max
	MATH_InitVector(&stLocal ,OoSpeedcoef  * 0.05f,OoSpeedcoef  * 0.05f,OoSpeedcoef  * 0.03f );
	GFX_Setv(pst_World->pst_GFX , GFX, 13203, &stLocal); // // friction speed
	GFX_FlagSet(pst_World->pst_GFX , GFX, 0 , 1);
	GFX_FlagSet(pst_World->pst_GFX , GFX, 2 , 1);

}

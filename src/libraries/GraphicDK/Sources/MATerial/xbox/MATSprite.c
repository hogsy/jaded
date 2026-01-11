/*$T MATSprite.c GC!1.55 01/10/00 12:04:08 */

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include "Precomp.h"
#include "BASe/BAStypes.h"

/* FOR DEDUG */
//#define MAT_SPR_D_SL 
//#define MAT_SPR_D_SL_UC
/* #define MAT_SPR_D_SCALEPOINTS */
//#define MAT_SPR_D_TESTMAP

// GVW 21/05
#define MAXSPGENSPRITE 2000

#define MAT_SPR_D_TESTMAPSIZE 32
#define SPRT_TSIZE          12
#define MAT_SPR_RndTableSize 256

#define MAT_SPR_INFINITE    100000000000000000.0f
#define MAT_SPR_ZMin        0.01f
//#define MAT_SPR_PICTURE_ACTIVATED


#include "BASe/MEMory/MEM.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "TEXture/TEXstruct.h"
#include "MATerial/MATSprite.h"
#include "TIMer/TIMdefs.h"

// GVW 21/05
#if defined(_XBOX)
#include "GX8\Gx8init.h"
#include "GX8/RASter/Gx8_CheatFlags.h"
#endif

	
#include "BASe/BENch/BENch.h"	
	
#define MAT_C_f_0Bits   ((32768.0f + 16384.0f) * 256.0f) 
#define MAT_SPR_ColorReductionCoef 0.9f
#define MAT_C_f_14Bits  (512.0f + 256.0f)
#define IFAL            lInterpretFloatAsLong
#define ILAF            fInterpretLongAsFloat
	
	float           MAT_SPR_RandomTableU[MAT_SPR_RndTableSize];
	float           MAT_SPR_RandomTableV[MAT_SPR_RndTableSize];
	float           MAT_SPR_RandomTableS[MAT_SPR_RndTableSize];
	float			MAT_SPR_256_to_01[256];
#ifdef MAT_SPR_D_TESTMAP
	ULONG *p_IMGDEBUG;
#endif

typedef struct  FloatingColor_
{
	float b,g,r,a;
} FloatingColor;

typedef struct  UInc_
{
	MATH_tdst_Vector    BasePoint;	//3
	float               MipMapSize;	//1
	FloatingColor		stColor;	//4
	GEO_tdst_UV         BaseUV;		//2
	float               OoZ;		//1
	float               Fautquecafasse12poursuperalgoPS2doncjerajouteunflottantici;		//12!!
} UInc ONLY_PSX2_ALIGNED(16);

typedef struct  MAT_SPR_InternalStruct_
{
	GDI_tdst_DisplayData    *pst_DD;
	GEO_tdst_Object         *pst_Obj;
	ULONG           flags;
	float                   *MipMapValues;
	unsigned short          *p_UV_Index;
	unsigned short          *p_XYZ_Index;
	GEO_Vertex              *dst_Point;
	GEO_tdst_UV             *dst_UV;
	ULONG					SizeOfTexture;
	ULONG					UCounter;
	UInc                    *p_RInc;
	UInc                    *p_LInc;
	UInc                    **pp_ClippedUInc;
	ULONG					NumberOfPoints;
	ULONG					NumberOfPointsIn;
	ULONG					NumberOfPointsOut;
	ULONG					UIncCachePos;
	ULONG					NumberOfSpritesGenerated;
	UInc                    UIncCache[32] ONLY_PSX2_ALIGNED(16);
	float                   TexelsSurf;
	float                   NearestValue;
	float                   FarestValue;
	float                   CurrentDelta;
	float                   Focale;
	float                   CurrentU;
	float 					fCurrentSize;
	float 					fCurrentSize4All;
	MATH_tdst_Vector		UVector ONLY_PSX2_ALIGNED(16);
	UInc                    VVector ONLY_PSX2_ALIGNED(16);
	MAT_tdst_SpriteGen      *p_SpriteGen;
	MATH_tdst_Vector        XCam;
	MATH_tdst_Vector        YCam;
	MATH_tdst_Vector        ClippingVector[5];
	float					ClippingDistances[5];
	MAT_tdst_MTLevel        *pst_MttxtLvl;
	
	
	ULONG					*pimg_XYZS_Picture;
	ULONG					*pimg_XYZS_Picture_Line;
	
	MATH_tdst_Vector        stZtriangle;
	
	/* Clipping */
	float                   Thresh;
} MAT_SPR_InternalStruct;

/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_MUL_ADD_flt_tbl(float *Dst, float *Src , float *Src2 ,float Factor)
{
	*(Dst + 0) = *(Src2 + 0) + *(Src + 0) * Factor;
	*(Dst + 1) = *(Src2 + 1) + *(Src + 1) * Factor;
	*(Dst + 2) = *(Src2 + 2) + *(Src + 2) * Factor;
	*(Dst + 3) = *(Src2 + 3) + *(Src + 3) * Factor;
	*(Dst + 4) = *(Src2 + 4) + *(Src + 4) * Factor;
	*(Dst + 5) = *(Src2 + 5) + *(Src + 5) * Factor;
	*(Dst + 6) = *(Src2 + 6) + *(Src + 6) * Factor;
	*(Dst + 7) = *(Src2 + 7) + *(Src + 7) * Factor;
	*(Dst + 8) = *(Src2 + 8) + *(Src + 8) * Factor;
	*(Dst + 9) = *(Src2 + 9) + *(Src + 9) * Factor;
	*(Dst + 10) = *(Src2 + 10) + *(Src + 10)* Factor;
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_ADD_flt_tbl(float *Dst, float *Src)
{
	*(Dst + 0) += *(Src + 0);
	*(Dst + 1) += *(Src + 1);
	*(Dst + 2) += *(Src + 2);
	*(Dst + 3) += *(Src + 3);
	
	*(Dst + 4) += *(Src + 4);
	*(Dst + 5) += *(Src + 5);
	*(Dst + 6) += *(Src + 6);
	*(Dst + 7) += *(Src + 7);
	
	*(Dst + 8) += *(Src + 8);
	*(Dst + 9) += *(Src + 9);
	*(Dst + 10) += *(Src + 10);//*/
}
/*
=======================================================================================================================
=======================================================================================================================
*/
_inline_ void MAT_SPR_ADD_flt_tbl_8(float *Dst, float *Src)
{
	*(Dst + 0) += *(Src + 0);
	*(Dst + 1) += *(Src + 1);
	*(Dst + 2) += *(Src + 2);
	*(Dst + 3) += *(Src + 3);
	*(Dst + 4) += *(Src + 4);
	*(Dst + 5) += *(Src + 5);
	*(Dst + 6) += *(Src + 6);
	*(Dst + 7) += *(Src + 7);
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_SUB_MUL_flt_tbl	(float *Dst, float *A, float *B, float Factor)
{
	*(Dst + 0) = (*(B + 0) - *(A + 0)) * Factor;
	*(Dst + 1) = (*(B + 1) - *(A + 1)) * Factor;
	*(Dst + 2) = (*(B + 2) - *(A + 2)) * Factor;
	*(Dst + 3) = (*(B + 3) - *(A + 3)) * Factor;
	*(Dst + 4) = (*(B + 4) - *(A + 4)) * Factor;
	*(Dst + 5) = (*(B + 5) - *(A + 5)) * Factor;
	*(Dst + 6) = (*(B + 6) - *(A + 6)) * Factor;
	*(Dst + 7) = (*(B + 7) - *(A + 7)) * Factor;
	*(Dst + 8) = (*(B + 8) - *(A + 8)) * Factor;
	*(Dst + 9) = (*(B + 9) - *(A + 9)) * Factor;
	*(Dst + 10) = (*(B + 10) - *(A + 10)) * Factor;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_SUB_MUL_SUB_MUL_flt_tbl(float *Dst, float *A, float *B, float *C, float Factor,float Factor2)
{
	*(Dst + 0) = (*(A + 0) + (*(B + 0) - *(A + 0)) * Factor - *(C + 0)) * Factor2;
	*(Dst + 1) = (*(A + 1) + (*(B + 1) - *(A + 1)) * Factor - *(C + 1)) * Factor2;
	*(Dst + 2) = (*(A + 2) + (*(B + 2) - *(A + 2)) * Factor - *(C + 2)) * Factor2;
	*(Dst + 3) = (*(A + 3) + (*(B + 3) - *(A + 3)) * Factor - *(C + 3)) * Factor2;
	*(Dst + 4) = (*(A + 4) + (*(B + 4) - *(A + 4)) * Factor - *(C + 4)) * Factor2;
	*(Dst + 5) = (*(A + 5) + (*(B + 5) - *(A + 5)) * Factor - *(C + 5)) * Factor2;
	*(Dst + 6) = (*(A + 6) + (*(B + 6) - *(A + 6)) * Factor - *(C + 6)) * Factor2;
	*(Dst + 7) = (*(A + 7) + (*(B + 7) - *(A + 7)) * Factor - *(C + 7)) * Factor2;
	*(Dst + 8) = (*(A + 8) + (*(B + 8) - *(A + 8)) * Factor - *(C + 8)) * Factor2;
	*(Dst + 9) = (*(A + 9) + (*(B + 9) - *(A + 9)) * Factor - *(C + 9)) * Factor2;
	*(Dst + 10) = (*(A + 10) + (*(B + 10) - *(A + 10)) * Factor - *(C + 10)) * Factor2;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_Interpol_flt_tbl	(float *Dst, float *A, float *B, float Factor)
{
/*	    if ((isnanf(Factor)) || (Factor > 1.0f) || (Factor < 0.0f))
    	*(ULONG *)0 = *(ULONG *)0;//*/

	*(Dst + 0) = *(A + 0) + (*(B + 0) - *(A + 0)) * Factor;
	*(Dst + 1) = *(A + 1) + (*(B + 1) - *(A + 1)) * Factor;
	*(Dst + 2) = *(A + 2) + (*(B + 2) - *(A + 2)) * Factor;
	*(Dst + 3) = *(A + 3) + (*(B + 3) - *(A + 3)) * Factor;
	*(Dst + 4) = *(A + 4) + (*(B + 4) - *(A + 4)) * Factor;
	*(Dst + 5) = *(A + 5) + (*(B + 5) - *(A + 5)) * Factor;
	*(Dst + 6) = *(A + 6) + (*(B + 6) - *(A + 6)) * Factor;
	*(Dst + 7) = *(A + 7) + (*(B + 7) - *(A + 7)) * Factor;
	*(Dst + 8) = *(A + 8) + (*(B + 8) - *(A + 8)) * Factor;
	*(Dst + 9) = *(A + 9) + (*(B + 9) - *(A + 9)) * Factor;
	*(Dst + 10) = *(A + 10) + (*(B + 10) - *(A + 10)) * Factor;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_SUBMULSUBMUL_Vector(MATH_tdst_Vector		*Dst, MATH_tdst_Vector		*A, MATH_tdst_Vector		*B, MATH_tdst_Vector		*C, float Factor,float Factor2)
{
	Dst->x = (A->x + ((B->x - A->x) * Factor) - C->x) * Factor2;
	Dst->y = (A->y + ((B->y - A->y) * Factor) - C->y) * Factor2;
	Dst->z = (A->z + ((B->z - A->z) * Factor) - C->z) * Factor2;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_MULADD_Vector(MATH_tdst_Vector		*Dst, MATH_tdst_Vector		*A, float Factor)
{
	Dst->x += A->x * Factor;
	Dst->y += A->y * Factor;
	Dst->z += A->z * Factor;
}

_inline_ void MAT_SPR_AffectNoise1(MATH_tdst_Vector *Dst , MATH_tdst_Vector *A,MATH_tdst_Vector *B,float C)
{
	Dst->x = A->x + B->x * C;
	Dst->y = A->y + B->y * C;
	Dst->z = A->z + B->z * C;
}
_inline_ void MAT_SPR_AffectNoise2(MATH_tdst_Vector *Dst , MATH_tdst_Vector *A,float C)
{
	Dst->x += A->x * C;
	Dst->y += A->y * C;
	Dst->z += A->z * C;
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_RasterizeV_Line(MAT_SPR_InternalStruct *is)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UInc    			st_Crnt 		ONLY_PSX2_ALIGNED(16); 
	GEO_Vertex    		st_CrntNoise 	ONLY_PSX2_ALIGNED(16); 
	UInc    *p_A, *p_B;
	float   CurrentV, HighestV;
	LONG    RandPtr, RandPtrU;
	ULONG	*pimg_XYZS_Picture_Pixel,*pimg_XYZS_Picture_Pixel_Last;
	
	float   Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef MAT_SPR_D_SL
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector    *v[2];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		v[0] = &is->p_LInc->BasePoint;
		v[1] = &is->p_RInc->BasePoint;
		is ->pst_DD->st_GDI.pfnl_Request(is->pst_DD, GDI_Cul_Request_DrawLine, (ULONG) v);
	}
	
#endif
	if(is->p_LInc->BaseUV.fV < is->p_RInc->BaseUV.fV)
	{
		p_A = is->p_LInc;
		p_B = is->p_RInc;
	}
	else
	{
		p_A = is->p_RInc;
		p_B = is->p_LInc;
	}
	CurrentV = p_A->BaseUV.fV + MAT_C_f_0Bits/*  - 0.5f*/ ;
	IFAL(CurrentV)++;
	CurrentV -= MAT_C_f_0Bits ;
	
	if (CurrentV > p_A->BaseUV.fV) CurrentV--;
	if (CurrentV < p_A->BaseUV.fV) CurrentV++;
	
	HighestV = p_B->BaseUV.fV;
	if(!(is->flags & MAT_Cul_Flag_TileV))
	{
		if(HighestV < 0.0f) return;
		if(CurrentV > (float) is->SizeOfTexture) return;
		if(HighestV > (float) is->SizeOfTexture)
			HighestV = (float) is->SizeOfTexture;
		if(CurrentV < 0.0f) CurrentV = 0.0f;
	}//*/
	
	if(HighestV <= CurrentV) return;
	
	ILAF(RandPtr) = CurrentV + MAT_C_f_0Bits ;
	pimg_XYZS_Picture_Pixel = is -> pimg_XYZS_Picture_Line + (RandPtr & (is->SizeOfTexture - 1));
	pimg_XYZS_Picture_Pixel_Last = is -> pimg_XYZS_Picture_Line + is->SizeOfTexture;//*/
	
	ILAF(RandPtrU) = is->CurrentU + MAT_C_f_0Bits ;
	RandPtr += 1 << (RandPtrU & 15);
	RandPtr += ((RandPtrU >> 1) & 15);
	RandPtr += ((RandPtrU >> 2) & 7);
	RandPtrU &= MAT_SPR_RndTableSize-1;
	Size = fOptInv(p_B->BaseUV.fV - p_A->BaseUV.fV);
	is->VVector.MipMapSize = (p_B->MipMapSize - p_A->MipMapSize) * Size;
	
	if (is->p_SpriteGen->flags & MAT_SPR_DisearpearWthTransparency)
	{
		if (MAT_GET_Blending(is->pst_MttxtLvl->ul_Flags) >= MAT_Cc_Op_Add)
		{
			is->VVector.stColor . r = (p_B->stColor . r - p_A->stColor . r ) * Size;
			is->VVector.stColor . g = (p_B->stColor . g - p_A->stColor . g ) * Size;
			is->VVector.stColor . b = (p_B->stColor . b - p_A->stColor . b ) * Size;
		} else
			is->VVector.stColor . a = (p_B->stColor . a - p_A->stColor . a ) * Size;
	}//*/
	
	MAT_SPR_MUL_ADD_flt_tbl ((float *)&st_Crnt, (float *)&is->VVector,(float *)p_A,CurrentV - p_A->BaseUV.fV);
	st_Crnt . stColor .a += MAT_C_f_14Bits;
	st_Crnt . stColor .r += MAT_C_f_14Bits;
	st_Crnt . stColor .g += MAT_C_f_14Bits;
	st_Crnt . stColor .b += MAT_C_f_14Bits;
	
	
	while(CurrentV < HighestV)
	{
		RandPtr &= MAT_SPR_RndTableSize-1;
		RandPtr ^= RandPtrU;
		Size = MAT_SPR_RandomTableU[RandPtr] * is->p_SpriteGen->Noise;
#ifdef MAT_SPR_PICTURE_ACTIVATED		
		if ( is -> pimg_XYZS_Picture ) Size += MAT_SPR_256_to_01[(*pimg_XYZS_Picture_Pixel >> 8) & 0xff];
#endif		
		MAT_SPR_AffectNoise1(&st_CrntNoise , &st_Crnt.BasePoint,&is->UVector,Size);
		Size = MAT_SPR_RandomTableV[RandPtr] * is->p_SpriteGen->Noise;
#ifdef MAT_SPR_PICTURE_ACTIVATED		
		if ( is -> pimg_XYZS_Picture ) Size += MAT_SPR_256_to_01[(*pimg_XYZS_Picture_Pixel >> 16) & 0xff];
#endif		
		MAT_SPR_AffectNoise2(&st_CrntNoise , &is->VVector.BasePoint,Size);
#ifdef MAT_SPR_PICTURE_ACTIVATED		
		if ( is -> pimg_XYZS_Picture ) 
		{
			Size = MAT_SPR_256_to_01[(*pimg_XYZS_Picture_Pixel >> 24) & 0xff];
			st_CrntNoise.x += is->stZtriangle.x * Size;
			st_CrntNoise.y += is->stZtriangle.y * Size;
			st_CrntNoise.z += is->stZtriangle.z * Size;
		}//*/
#endif		
		
		if (is->p_SpriteGen->flags & MAT_SPR_DisearpearWthSize)
			Size = is->fCurrentSize * st_Crnt.MipMapSize;
		else
			Size = is->fCurrentSize;
		
		if (is->p_SpriteGen->flags & MAT_SPR_VertexAlphaIsSize)
			Size *= (st_Crnt.stColor.a - MAT_C_f_14Bits) / 255.0f;
		
		Size *= is->p_SpriteGen->SizeNoise * MAT_SPR_RandomTableS[RandPtr] + 1.0f;
		
#ifdef MAT_SPR_PICTURE_ACTIVATED		
		if ( is -> pimg_XYZS_Picture ) Size *= MAT_SPR_256_to_01[*pimg_XYZS_Picture_Pixel & 0xff];
#endif		

// GVW 21/05
// GVW 21/05
		if (Size)
		{
			if (++(is->NumberOfSpritesGenerated)<MAXSPGENSPRITE)
			{
				ULONG	ulColor;
				ulColor = (*(ULONG *)&st_Crnt.stColor.a << 10) & 0xff000000
										| (*(ULONG *)&st_Crnt.stColor.r << 2) & 0xff0000
										| (*(ULONG *)&st_Crnt.stColor.g >> 6) & 0xff00
										| (*(ULONG *)&st_Crnt.stColor.b >> 14) & 0xff;

				if (is->p_SpriteGen->flags & MAT_SPR_TableMapped)
				{
					GEO_Vertex stPoint;
					MATH_AddVector(&stPoint,&is->UVector,&is->VVector.BasePoint);
					MATH_ScaleVector(&stPoint,&stPoint,Size);
					MATH_AddVector(&stPoint,&stPoint,&st_CrntNoise);
					Gx8_fnvAddSprite(&stPoint,Size,ulColor);
				}
				else
				{
					Gx8_fnvAddSprite(&st_CrntNoise,Size,ulColor);
				}
			}
		}
		
		MAT_SPR_ADD_flt_tbl_8((float *)&st_Crnt, (float *)&is->VVector);
		RandPtr ^= RandPtrU;
		RandPtr ++;
		CurrentV ++;
#ifdef MAT_SPR_PICTURE_ACTIVATED		
		pimg_XYZS_Picture_Pixel++;
		if (pimg_XYZS_Picture_Pixel >= pimg_XYZS_Picture_Pixel_Last)
			pimg_XYZS_Picture_Pixel = is->pimg_XYZS_Picture_Line;//*/
#endif			
	}
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_RasterizeUV_nGone(MAT_SPR_InternalStruct *is)// 0.64 == 0.18
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    UInc            *sz_Table[SPRT_TSIZE],st_Rinc, st_RCrnt, st_Linc, st_LCrnt  ONLY_PSX2_ALIGNED(16);
    UInc            **p_Rpos, **p_Lpos;
    
    ULONG			HighestUIndex=0, Index=0, LastIndex, Counter;
    float           LowestU, HighestU;
	ULONG			ulCurrentU;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LowestU = MAT_SPR_INFINITE;
    HighestU = -MAT_SPR_INFINITE;
    Counter = is->NumberOfPoints;
    p_Rpos = is->pp_ClippedUInc;
    p_Lpos = p_Rpos + Counter - 1;
    
	//HACK TiZ
	memset(sz_Table,0,sizeof(sz_Table));
	//HACK end
	
    while(Counter--)
    {
        if((LowestU >= (*p_Lpos)->BaseUV.fU) && ((*p_Lpos)->BaseUV.fU < (*p_Rpos)->BaseUV.fU))
        {
            Index = Counter;
            LowestU = (*p_Lpos)->BaseUV.fU;
        }
        else if((HighestU <= (*p_Lpos)->BaseUV.fU) && ((*p_Lpos)->BaseUV.fU > (*p_Rpos)->BaseUV.fU))
        {
            HighestUIndex = Counter;
            HighestU = (*p_Lpos)->BaseUV.fU;
        }
        p_Rpos = p_Lpos--;
    }
	
    is->CurrentU = LowestU + MAT_C_f_0Bits/* - 0.5f */;
    IFAL(is->CurrentU)++;
    is->CurrentU -= MAT_C_f_0Bits ;
	if (is->CurrentU > LowestU) is->CurrentU--;
	if (is->CurrentU > LowestU) is->CurrentU--;
	if (is->CurrentU < LowestU) is->CurrentU++;
	if (is->CurrentU < LowestU) is->CurrentU++;
	if (is->CurrentU < LowestU) is->CurrentU++;
    
	
    if(!(is->flags & MAT_Cul_Flag_TileU))
    {
        if(HighestU < 0.0f) return;
        if(is->CurrentU > is->SizeOfTexture) return;
        if(HighestU > is->SizeOfTexture)
            HighestU = (float) is->SizeOfTexture;
        if(is->CurrentU < 0.0f) is->CurrentU = 0.0f;
    }
	
    if(HighestU < is->CurrentU) return;
	
    p_Rpos = sz_Table;
    p_Lpos = sz_Table + SPRT_TSIZE - 1;
    *(p_Rpos++) = is->pp_ClippedUInc[Index];
    *(p_Lpos) = is->pp_ClippedUInc[HighestUIndex];
	
    Counter = is->NumberOfPoints;
    while(Counter--)
    {
        LastIndex = Index++;
        if(Index == is->NumberOfPoints) Index = 0;
        if(is->pp_ClippedUInc[LastIndex]->BaseUV.fU < is->pp_ClippedUInc[Index]->BaseUV.fU)
            /* Right */
            *(p_Rpos++) = is->pp_ClippedUInc[Index];
        else if(is->pp_ClippedUInc[LastIndex]->BaseUV.fU > is->pp_ClippedUInc[Index]->BaseUV.fU)
            /* Left */
            *(--p_Lpos) = is->pp_ClippedUInc[Index];
    }
	
    p_Rpos = sz_Table;
    is->p_LInc = &st_LCrnt;
    is->p_RInc = &st_RCrnt;
    Index = 1;
    Counter = 1;
	MAT_SPR_SUB_MUL_flt_tbl((float *)&st_Rinc,(float *)*p_Rpos,(float *)*(p_Rpos + 1),fOptInv((*(p_Rpos + 1))->BaseUV.fU - (*(p_Rpos))->BaseUV.fU));
	MAT_SPR_MUL_ADD_flt_tbl((float *)&st_RCrnt,(float *)&st_Rinc,(float *)(*(p_Rpos)),(is->CurrentU - (*(p_Rpos))->BaseUV.fU));
	MAT_SPR_SUB_MUL_flt_tbl((float *)&st_Linc,(float *)(*p_Lpos),(float *)(*(p_Lpos + 1)),fOptInv((*(p_Lpos + 1))->BaseUV.fU - (*(p_Lpos))->BaseUV.fU));
	MAT_SPR_MUL_ADD_flt_tbl((float *)&st_LCrnt,(float *)&st_Linc,(float *)(*(p_Lpos)),(is->CurrentU - (*(p_Lpos))->BaseUV.fU));
	
	ulCurrentU = (((ULONG)is->CurrentU) & (is->SizeOfTexture - 1));
	
    while(is->CurrentU < HighestU)
    {
        /* Right */
        while((*(p_Rpos + 1))->BaseUV.fU < is->CurrentU)
        {
            p_Rpos++;
			MAT_SPR_SUB_MUL_flt_tbl((float *)&st_Rinc,(float *)*p_Rpos,(float *)*(p_Rpos + 1),fOptInv((*(p_Rpos + 1))->BaseUV.fU - (*(p_Rpos))->BaseUV.fU));
			MAT_SPR_MUL_ADD_flt_tbl((float *)&st_RCrnt,(float *)&st_Rinc,(float *)(*(p_Rpos)),(is->CurrentU - (*(p_Rpos))->BaseUV.fU));
        }
		
        /* Left */
        while((*(p_Lpos + 1))->BaseUV.fU < is->CurrentU)
        {
            p_Lpos++;
            MAT_SPR_SUB_MUL_flt_tbl((float *)&st_Linc,(float *)(*p_Lpos),(float *)(*(p_Lpos + 1)),fOptInv((*(p_Lpos + 1))->BaseUV.fU - (*(p_Lpos))->BaseUV.fU));
			MAT_SPR_MUL_ADD_flt_tbl((float *)&st_LCrnt,(float *)&st_Linc,(float *)(*(p_Lpos)),(is->CurrentU - (*(p_Lpos))->BaseUV.fU));
        }
#ifdef MAT_SPR_PICTURE_ACTIVATED		
		is->pimg_XYZS_Picture_Line = is->pimg_XYZS_Picture + (ulCurrentU & (is->SizeOfTexture - 1)) * is->SizeOfTexture;
#endif		
		ulCurrentU = (((ULONG)is->CurrentU) & (is->SizeOfTexture - 1));
		MAT_RasterizeV_Line(is);
		
        MAT_SPR_ADD_flt_tbl((float *)&st_RCrnt, (float *)&st_Rinc);
        MAT_SPR_ADD_flt_tbl((float *)&st_LCrnt, (float *)&st_Linc);
		
		ulCurrentU++;
        is->CurrentU ++;
    }
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_RasterizeUV_CLIP
(
 MAT_SPR_InternalStruct  *is,
 UInc                    **p_Near_e,
 UInc                    **p_Far_e,
 UInc                    **Source,
 ULONG           SourceNum
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    UInc            **LastSource,**p_Near,**p_Far;
    ULONG   State;
    float           OoZ;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LastSource = Source + SourceNum - 1;
	p_Near = p_Near_e;
	p_Far  = p_Far_e;
	OoZ = is->Thresh - (*LastSource)->OoZ;
	State = (*(ULONG *)&OoZ) >> 31;
    while(SourceNum--)
    {
		OoZ = is->Thresh - (*Source)->OoZ;
        State = ((*(ULONG *)&OoZ) >> 31) | (State << 1);
        switch(State & 3)
        {
        case 2: /* Come in */
            *p_Far = &is->UIncCache[is->UIncCachePos++];
            MAT_SPR_Interpol_flt_tbl((float *)*p_Far, (float *)*Source, (float *)*LastSource, OoZ * fOptInv((*LastSource)->OoZ - (*Source)->OoZ));
            (*p_Far)->MipMapSize = 1.0f;
            *(p_Near++) = *(p_Far++);
        case 0: /* Stay in */
            (*Source)->MipMapSize = 1.0f;
            *(p_Near++) = *(Source);
            break;
        case 1: /* Go out */
            *p_Far = &is->UIncCache[is->UIncCachePos++];
            MAT_SPR_Interpol_flt_tbl((float *)*p_Far, (float *)*Source, (float *)*LastSource, OoZ * fOptInv((*LastSource)->OoZ - (*Source)->OoZ));
            (*p_Far)->MipMapSize = 1.0f;
            *(p_Near++) = *(p_Far++);
        case 3: /* Stay out */
            (*Source)->MipMapSize = 1.0f - OoZ * is->CurrentDelta;
            *(p_Far++) = *(Source);
            break;
        }
        LastSource = Source++;
    }
	is->NumberOfPointsIn  = p_Near - p_Near_e;
	is->NumberOfPointsOut = p_Far  - p_Far_e;
    is->UIncCachePos = (is->UIncCachePos & 15) + 16;
    if(is->UIncCachePos == 32) is->UIncCachePos = 16;

}

/*
=======================================================================================================================
=======================================================================================================================
*/
float MAT_SPR_GetFarestValue(MAT_SPR_InternalStruct *is)
{
    is->FarestValue = 0.8f * is->TexelsSurf * is->Focale * is->p_SpriteGen->MipMapCoef;
	is->NearestValue = 0.12500f * is->TexelsSurf * is->Focale * is->p_SpriteGen->MipMapCoef;
    return is->FarestValue;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
float MAT_SPR_GetNearestValue(MAT_SPR_InternalStruct *is)
{
    return is->NearestValue;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
float MAT_SPR_GetNextValue(MAT_SPR_InternalStruct *is, float MMMax)
{
    if(MMMax <= is->NearestValue)
        return -MAT_SPR_INFINITE;
    return is->NearestValue;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
float MAT_SPR_AlignValue(MAT_SPR_InternalStruct *is, float MMMax)
{
    if(MMMax > is->NearestValue)
        MMMax = is->FarestValue;
    else
        MMMax = is->NearestValue;
    return MMMax;
}
/*
=======================================================================================================================
=======================================================================================================================
*/

/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_RasterizeUV
(
 GDI_tdst_DisplayData                *pst_DD,
 GEO_tdst_Object                     *pst_Obj,
 MAT_tdst_MTLevel                    *_pst_MttxtLvl,
 GEO_tdst_ElementIndexedTriangles    *pst_Element
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_SPR_InternalStruct      is;
    static ULONG                bFirst = 1;
    MATH_tdst_Matrix			Matrix ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Vector			Local,Local2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef Gx8_BENCH
	if (g_iNoSPR) return;
#endif	

	//HACK TiZ
	memset(&is, 0, sizeof(MAT_SPR_InternalStruct));
	//HACK end

    is.pst_DD = pst_DD;
    is.pst_Obj = pst_Obj;
    is.flags = _pst_MttxtLvl->ul_Flags;
	is.dst_Point = pst_DD->p_Current_Vertex_List;
    is.dst_UV = (GEO_tdst_UV *) pst_DD->pst_ComputingBuffers->Current;
    is.Focale = fOptCos(is.pst_DD->st_Camera.f_FieldOfVision * 0.5f) * fOptInv(fOptSin(is.pst_DD->st_Camera.f_FieldOfVision * 0.5f));
	is.p_SpriteGen = *(MAT_tdst_SpriteGen **)&TEX_gst_GlobalList.dst_Texture[_pst_MttxtLvl->s_TextureId].w_Width;
	is.fCurrentSize4All = is.p_SpriteGen->Size * 0.01f;

	is.SizeOfTexture = 32;
	is.pimg_XYZS_Picture = NULL;
#ifdef MAT_SPR_PICTURE_ACTIVATED		
	if (is.p_SpriteGen ->p_BMap)
	{
		is.SizeOfTexture = is.p_SpriteGen ->p_BMap->Size;
		is.pimg_XYZS_Picture = is.p_SpriteGen ->p_BMap->p_Pixels;
	}//*/
#endif	
	
	is.XCam . x = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Ix;
	is.XCam . y = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Jx;
	is.XCam . z = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Kx;
	is.YCam . x = pst_DD->st_MatrixStack.pst_CurrentMatrix->Iy;
	is.YCam . y = pst_DD->st_MatrixStack.pst_CurrentMatrix->Jy;
	is.YCam . z = pst_DD->st_MatrixStack.pst_CurrentMatrix->Ky;
	
    MATH_InvertMatrix(&Matrix, pst_DD->st_MatrixStack.pst_CurrentMatrix);
	Local .x = Local .y = Local . z = 0.0f;
	MATH_TransformVertex(&Local, &Matrix, &Local);
	
	Local2.x = 0.0f;
	Local2.y =-fOptCos(is.pst_DD->st_Camera.f_FieldOfVision * 0.5f);
	Local2.z =-fOptSin(is.pst_DD->st_Camera.f_FieldOfVision * 0.5f);
	MATH_TransformVertex(&is.ClippingVector[0], &Matrix, &Local2);
	MATH_SubVector(&is.ClippingVector[0], &is.ClippingVector[0] , &Local );
	is.ClippingDistances[0] = MATH_f_DotProduct(&is.ClippingVector[0], &Local);
	
	Local2.y =-Local2.y;
	MATH_TransformVertex(&is.ClippingVector[1], &Matrix, &Local2);
	MATH_SubVector(&is.ClippingVector[1], &is.ClippingVector[1] , &Local );
	is.ClippingDistances[1] = MATH_f_DotProduct(&is.ClippingVector[1], &Local);
	
	Local2.x =-Local2.y;
	Local2.y =0.0f;
	MATH_TransformVertex(&is.ClippingVector[2], &Matrix, &Local2);
	MATH_SubVector(&is.ClippingVector[2], &is.ClippingVector[2] , &Local );
	is.ClippingDistances[2] = MATH_f_DotProduct(&is.ClippingVector[2], &Local);
	
	Local2.x =-Local2.x;
	MATH_TransformVertex(&is.ClippingVector[3], &Matrix, &Local2);
	MATH_SubVector(&is.ClippingVector[3], &is.ClippingVector[3] , &Local );
	is.ClippingDistances[3] = MATH_f_DotProduct(&is.ClippingVector[3], &Local);
	
	Local2.x =0.0f;
	Local2.y =0.0f;
	Local2.z =1.0f;
	MATH_TransformVertex(&is.ClippingVector[4], &Matrix, &Local2);
	MATH_SubVector(&is.ClippingVector[4], &is.ClippingVector[4] , &Local );
	is.ClippingDistances[4] = MATH_f_DotProduct(&is.ClippingVector[4], &Local);
	
	
	is.pst_MttxtLvl = _pst_MttxtLvl;
	
	is.NumberOfSpritesGenerated = 0; 
	
	
	
	
	GDI_SetTextureBlending((*pst_DD),is.p_SpriteGen->s_TextureIndex, _pst_MttxtLvl->ul_Flags, _pst_MttxtLvl->s_AditionalFlags);
// GVW 21/05
	Gx8_fnvBeforeSprite(MAXSPGENSPRITE);
    if(bFirst)
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~*/
        ULONG   Counter;
        /*~~~~~~~~~~~~~~~~~~~~~~~~*/
		
        bFirst = 0;
        for(Counter = 0; Counter < MAT_SPR_RndTableSize; Counter++)
		{
            MAT_SPR_RandomTableU[Counter] = (((float) (rand() & 255) / 255.0f) - 0.5f) * 0.1f;
            MAT_SPR_RandomTableV[Counter] = (((float) (rand() & 255) / 255.0f) - 0.5f) * 0.1f;
            MAT_SPR_RandomTableS[Counter] = (((float) (rand() & 255) / 255.0f) - 0.5f) ;
		}
        for(Counter = 0; Counter < 256 ; Counter++)
		{
            MAT_SPR_256_to_01[Counter] = (float) Counter / 255.0f;
		}
        for(Counter = 0; Counter < 256 ; Counter++)
		{
            MAT_SPR_256_to_01[Counter] = (float) Counter / 255.0f;
		}
#ifdef MAT_SPR_D_TESTMAP
		p_IMGDEBUG = MEM_p_Alloc(MAT_SPR_D_TESTMAPSIZE * MAT_SPR_D_TESTMAPSIZE * 4L);
        for(Counter = 0; Counter < MAT_SPR_D_TESTMAPSIZE ; Counter++)
		{
			ULONG YCounter;
			for(YCounter = 0; YCounter < MAT_SPR_D_TESTMAPSIZE ; YCounter ++)
			{
				p_IMGDEBUG[YCounter + (Counter * MAT_SPR_D_TESTMAPSIZE)] = 0;
				if (((float)YCounter - (float)MAT_SPR_D_TESTMAPSIZE * 0.5f) * ((float)YCounter - (float)MAT_SPR_D_TESTMAPSIZE * 0.5f) + ((float)Counter - (float)MAT_SPR_D_TESTMAPSIZE * 0.5f) * ((float)Counter - (float)MAT_SPR_D_TESTMAPSIZE * 0.5f) < (float)MAT_SPR_D_TESTMAPSIZE * (float)MAT_SPR_D_TESTMAPSIZE * 0.25f)
				{
					p_IMGDEBUG[YCounter + (Counter * MAT_SPR_D_TESTMAPSIZE)] = (ULONG)((1.0f + cos(((float)Counter) * 1.0f)) * 128.0f);
				}
				
				p_IMGDEBUG[YCounter + (Counter * MAT_SPR_D_TESTMAPSIZE)] |= (ULONG)((1.0f + sin(((float)YCounter) * 1.5f)) * 128.0f) << 8L;
				p_IMGDEBUG[YCounter + (Counter * MAT_SPR_D_TESTMAPSIZE)] |= (ULONG)((1.0f + cos(((float)Counter) * 1.5f)) * 128.0f) << 16L;
				
				{
					float ZZZZ;
					ZZZZ = (((float)YCounter - (float)MAT_SPR_D_TESTMAPSIZE * 0.5f) * ((float)YCounter - (float)MAT_SPR_D_TESTMAPSIZE * 0.5f) + ((float)Counter - (float)MAT_SPR_D_TESTMAPSIZE * 0.5f) * ((float)Counter - (float)MAT_SPR_D_TESTMAPSIZE * 0.5f));
					ZZZZ = (float)sqrt(ZZZZ);
					ZZZZ = 1.0f + (float)cos(((float)ZZZZ) * 0.25f);
					p_IMGDEBUG[YCounter + (Counter * MAT_SPR_D_TESTMAPSIZE)] |= (ULONG)(ZZZZ * 128.0f) <<24L;
				}
			}
		}
#endif
    }
#ifdef MAT_SPR_D_TESTMAP
    is.SizeOfTexture = MAT_SPR_D_TESTMAPSIZE;
	is.pimg_XYZS_Picture = p_IMGDEBUG;
#endif
	
    //yo ERR_X_WarnOnce( false, "Attempting to call MAT_RasterizeUV_Triangle" );
// GVW 21/05
	Gx8_fnvEndSprite(is.NumberOfSpritesGenerated);
}


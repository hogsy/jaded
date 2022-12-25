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
#define MAT_SPR_D_TESTMAPSIZE 32
#define SPRT_TSIZE          12
#define MAT_SPR_RndTableSize 256

#define MAT_SPR_INFINITE    100000000000000000.0f
#define MAT_SPR_ZMin        0.01f
//#define MAT_SPR_PICTURE_ACTIVATED


#include "BASe/MEMory/MEM.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "GDInterface/GDIrequest.h"
#include "TEXture/TEXstruct.h"
#include "MATerial/MATSprite.h"
#include "TIMer/TIMdefs.h"

#ifdef _GAMECUBE
#ifndef _FINAL_
#include "GXI_GC/GXI_dbg.h"
#endif
#endif


#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
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
#endif
#endif

#include "BASe/BENch/BENch.h"	
	
#define MAT_C_f_0Bits   ((32768.0f + 16384.0f) * 256.0f) 
#ifdef PSX2_TARGET
#define MAT_SPR_ColorReductionCoef 0.9f
#define MAT_C_f_14Bits  (1024.0f + 512.0f)
#else
#define MAT_SPR_ColorReductionCoef 0.9f
#define MAT_C_f_14Bits  (512.0f + 256.0f)
#endif
#define IFAL            lInterpretFloatAsLong
#define ILAF            fInterpretLongAsFloat
	
	float           MAT_SPR_RandomTableU[MAT_SPR_RndTableSize];
	float           MAT_SPR_RandomTableV[MAT_SPR_RndTableSize];
	float           MAT_SPR_RandomTableS[MAT_SPR_RndTableSize];
	float			MAT_SPR_256_to_01[256];
#ifdef MAT_SPR_D_TESTMAP
	ULONG *p_IMGDEBUG;
#endif

#ifdef PSX2_TARGET
#pragma pack(1)
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

#ifdef PSX2_TARGET
#pragma pack(0)
#endif

/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_MUL_ADD_flt_tbl(float *Dst, float *Src , float *Src2 ,float Factor)
{
#ifdef PSX2_TARGET
	asm __volatile__ ("
		.set noreorder;
		mfc1		t2,$f12
		qmtc2    	t2 , $vf10

		lqc2          $vf01,0(a1)
		lqc2          $vf02,0(a2)
		lqc2          $vf03,16(a1)
		lqc2          $vf04,16(a2)
		lqc2          $vf05,32(a1)
		lqc2          $vf06,32(a2)
		vmulx.xyzw 	$vf01,$vf01,$vf10x
		vmulx.xyzw 	$vf03,$vf03,$vf10x
		vmulx.xyzw 	$vf05,$vf05,$vf10x
		vadd.xyzw 	$vf01,$vf01,$vf02
		vadd.xyzw 	$vf03,$vf03,$vf04
		vadd.xyzw 	$vf05,$vf05,$vf06
		qmfc2.i    	t0 , $vf01
		sq			t0 , 0(a0)
		sqc2          $vf03,16(a0)
		sqc2          $vf05,32(a0)

		.set reorder;
		
   ");//*/
#else   
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
#endif
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_ADD_flt_tbl(float *Dst, float *Src)
{
#ifdef PSX2_TARGET
	asm __volatile__ ("
		.set noreorder;
		lqc2        $vf01,0(a0)
		lqc2        $vf02,0(a1)
		lqc2        $vf03,16(a0)
		lqc2        $vf04,16(a1)
		lqc2        $vf05,32(a0)
		lqc2        $vf06,32(a1)
		vadd.xyzw 	$vf01,$vf01,$vf02
		vadd.xyzw 	$vf03,$vf03,$vf04
		vadd.xyzw 	$vf05,$vf05,$vf06
		qmfc2.i    	t0 , $vf01
		sq			t0 , 0(a0)
		sqc2    	$vf03 , 16(a0)
		sqc2    	$vf05 , 32(a0)
		.set reorder;
		
   ");//*/
#else   
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
	
#endif
}
/*
=======================================================================================================================
=======================================================================================================================
*/
_inline_ void MAT_SPR_ADD_flt_tbl_8(float *Dst, float *Src)
{
#ifdef PSX2_TARGET
	register ULONG TMP;
	asm __volatile__ ("
		.set noreorder;
		lqc2        $vf01,0(Dst)
		lqc2        $vf02,0(Src)
		lqc2        $vf03,16(Dst)
		lqc2        $vf04,16(Src)
		vadd.xyzw 	$vf01,$vf01,$vf02
		vadd.xyzw 	$vf03,$vf03,$vf04
		qmfc2.i    	TMP , $vf01
		sq			TMP , 0(Dst)
		sqc2	   	$vf03,16(Dst)
		.set reorder;
   ");
#else
	*(Dst + 0) += *(Src + 0);
	*(Dst + 1) += *(Src + 1);
	*(Dst + 2) += *(Src + 2);
	*(Dst + 3) += *(Src + 3);
	*(Dst + 4) += *(Src + 4);
	*(Dst + 5) += *(Src + 5);
	*(Dst + 6) += *(Src + 6);
	*(Dst + 7) += *(Src + 7);
#endif
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_SUB_MUL_flt_tbl	(float *Dst, float *A, float *B, float Factor)
{
#ifdef PSX2_TARGET
	asm __volatile__ ("
		.set noreorder;
		mfc1		t2,$f12
		qmtc2    	t2 , $vf10
		lqc2        $vf01,0(a1)
		lqc2        $vf02,0(a2)
		lqc2        $vf03,16(a1)
		lqc2        $vf04,16(a2)
		lqc2		$vf05,32(a1)
		lqc2		$vf06,32(a2)
		vsub.xyzw 	$vf01,$vf02,$vf01
		vsub.xyzw 	$vf03,$vf04,$vf03
		vsub.xyzw 	$vf05,$vf06,$vf05
		vmulx.xyzw 	$vf01,$vf01,$vf10x
		vmulx.xyzw 	$vf03,$vf03,$vf10x
		vmulx.xyzw 	$vf05,$vf05,$vf10x
		qmfc2.i    	t0 , $vf01
		sq			t0 , 0(a0)
		sqc2		$vf03 , 16(a0)
		sqc2		$vf05 , 32(a0)

		.set reorder;
		
   ");//*/
#else   
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
#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_SUB_MUL_SUB_MUL_flt_tbl(float *Dst, float *A, float *B, float *C, float Factor,float Factor2)
{
#ifdef PSX2_TARGET
	asm __volatile__ ("
		.set noreorder;
		mfc1		t2,$f12
		qmtc2    	t2 , $vf14
		mfc1		t2,$f13
		qmtc2    	t2 , $vf15
		lqc2        $vf01,0(a1)
		lqc2        $vf02,0(a2)
		lqc2        $vf03,0(a3)
		lqc2        $vf04,16(a1)
		lqc2        $vf05,16(a2)
		lqc2        $vf06,16(a3)
		lqc2        $vf07,32(a1)
		lqc2        $vf08,32(a2)
		lqc2        $vf09,32(a3)
		vsub.xyzw 	$vf02,$vf02,$vf01
		vsub.xyzw 	$vf05,$vf05,$vf04
		vsub.xyzw 	$vf08,$vf08,$vf07
		vmulx.xyzw 	$vf02,$vf02,$vf14x
		vmulx.xyzw 	$vf05,$vf05,$vf14x
		vmulx.xyzw 	$vf08,$vf08,$vf14x
		vadd.xyzw 	$vf01,$vf02,$vf01
		vadd.xyzw 	$vf04,$vf05,$vf04
		vadd.xyzw 	$vf07,$vf08,$vf07
		vsub.xyzw 	$vf01,$vf01,$vf03
		vsub.xyzw 	$vf04,$vf04,$vf06
		vsub.xyzw 	$vf07,$vf07,$vf09
		vmulx.xyzw 	$vf01,$vf01,$vf15x
		vmulx.xyzw 	$vf04,$vf04,$vf15x
		vmulx.xyzw 	$vf07,$vf07,$vf15x
		qmfc2.i    	t0 , $vf01
		sq			t0 , 0(a0)
		sqc2    	$vf04 , 16(a0)
		sqc2    	$vf07 , 32(a0)
		.set reorder;
		
   ");//*/
#else   
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
#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_SPR_Interpol_flt_tbl	(float *Dst, float *A, float *B, float Factor)
{
/*	    if ((isnanf(Factor)) || (Factor > 1.0f) || (Factor < 0.0f))
    	*(ULONG *)0 = *(ULONG *)0;//*/

#ifdef PSX2_TARGET
	asm __volatile__ ("
		.set noreorder;
		mfc1		t2,$f12
		qmtc2    	t2 , $vf10
		lqc2        $vf01,0(a1)
		lqc2        $vf02,0(a2)
		lqc2        $vf03,16(a1)
		lqc2        $vf04,16(a2)
		lqc2        $vf05,32(a1)
		lqc2        $vf06,32(a2)
		vsub.xyzw 	$vf02,$vf02,$vf01
		vsub.xyzw 	$vf04,$vf04,$vf03
		vsub.xyzw 	$vf06,$vf06,$vf05
		vmulx.xyzw 	$vf02,$vf02,$vf10x
		vmulx.xyzw 	$vf04,$vf04,$vf10x
		vmulx.xyzw 	$vf06,$vf06,$vf10x
		vadd.xyzw 	$vf01,$vf02,$vf01
		vadd.xyzw 	$vf03,$vf04,$vf03
		vadd.xyzw 	$vf05,$vf06,$vf05
		qmfc2.i    	t0 , $vf01
		sq			t0 , 0(a0)
		sqc2		$vf03 , 16(a0)
		sqc2		$vf05 , 32(a0)
		.set reorder;
		
   ");//*/
#else   
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
#endif	
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
#ifdef PSX2_TARGET
/*
=======================================================================================================================
=======================================================================================================================
*/
/*ULONG MAT_SPR_FloatColorToUint(float *ColorAligned16)
{
	asm __volatile__ ("
		.set noreorder;
		lq        	v0,0(a0)
		psrlw		v0,v0,14
		ppacb		v0, 	zero, v0		
		jr ra
		ppacb		v0, 	zero, v0		
		.set reorder;
   ");
}*/

_inline_ ULONG MAT_SPR_FloatColorToUint(float *ColorAligned16)
{
	register ULONG Ret;
	asm __volatile__ ("
		.set noreorder;
		lq        	Ret,0(ColorAligned16)
		psrlw		Ret,Ret,14
		ppacb		Ret, 	zero, Ret		/* Pack Color Result 			*/
		ppacb		Ret, 	zero, Ret		/* Pack Color Result 			*/
		.set reorder;
   ");
   return Ret;
}

_inline_ void MAT_SPR_StoreNoise(GEO_Vertex *Dst)
{
	asm __volatile__ ("
		sqc2        $vf01,0(Dst)
   ");
}
_inline_ void MAT_SPR_AffectNoise1(MATH_tdst_Vector *Dst , MATH_tdst_Vector *A,MATH_tdst_Vector *B,float C)
{
	register ULONG Tmp;
	asm __volatile__ ("
		lqc2        $vf01,0(A)
		lqc2        $vf02,0(B)
		mfc1		Tmp,C
		qmtc2		Tmp,$vf03
		VMULx.xyz	$vf02 , $vf02 , $vf03x
		VADD.xyz	$vf01 , $vf01 , $vf02
   ");
}
_inline_ void MAT_SPR_AffectNoise2(MATH_tdst_Vector *Dst , MATH_tdst_Vector *A,float C)
{
	register ULONG Tmp;
	asm __volatile__ ("
		lqc2        $vf02,0(A)
		mfc1		Tmp,C
		qmtc2		Tmp,$vf03
		VMULx.xyz	$vf02 , $vf02 , $vf03x
		VADD.xyz	$vf01 , $vf01 , $vf02
   ");
}
#else

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
#endif
/*
=======================================================================================================================
=======================================================================================================================
*/
#ifdef PSX2_TARGET		
void MAT_RasterizeV_Line(MAT_SPR_InternalStruct *is)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UInc    			st_Crnt 		ONLY_PSX2_ALIGNED(16); 
	GEO_Vertex    		st_CrntNoise 	ONLY_PSX2_ALIGNED(16); 
	UInc    *p_A, *p_B;
	SOFT_tdst_AVertex	st_Sprite;
	float   CurrentV, HighestV;
	LONG    RandPtr, RandPtrU;
	ULONG	*pimg_XYZS_Picture_Pixel,*pimg_XYZS_Picture_Pixel_Last;
	
	register float   Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
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
		MAT_SPR_AffectNoise1(&st_CrntNoise , &st_Crnt.BasePoint,&is->UVector,	MAT_SPR_RandomTableU[RandPtr] * is->p_SpriteGen->Noise);
		MAT_SPR_AffectNoise2(&st_CrntNoise , &is->VVector.BasePoint,			MAT_SPR_RandomTableV[RandPtr] * is->p_SpriteGen->Noise);
		Size = is->fCurrentSize * (is->p_SpriteGen->SizeNoise * MAT_SPR_RandomTableS[RandPtr] + 1.0f);
		if (is->p_SpriteGen->flags & MAT_SPR_DisearpearWthSize)
			Size *= st_Crnt.MipMapSize;
		if (is->p_SpriteGen->flags & MAT_SPR_VertexAlphaIsSize)
			Size *= (st_Crnt.stColor.a - MAT_C_f_14Bits) * 0.003921568627450f /* 1.0f / 255.0f */;
		
		/* BEGIN DRAW SPRITE */
		MAT_SPR_StoreNoise(&st_Sprite);
		st_Sprite.w = Size;
		GSP_DrawSpriteX(&st_Sprite , MAT_SPR_FloatColorToUint((float *)&st_Crnt.stColor));
		/* END DRAW SPRITE */
		
		MAT_SPR_ADD_flt_tbl_8((float *)&st_Crnt, (float *)&is->VVector);
		RandPtr ^= RandPtrU;
		RandPtr ++;
		CurrentV ++;
	}
}
#else
void MAT_RasterizeV_Line(MAT_SPR_InternalStruct *is)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UInc    			st_Crnt 		ONLY_PSX2_ALIGNED(16); 
	GEO_Vertex    		st_CrntNoise 	ONLY_PSX2_ALIGNED(16); 
	UInc    *p_A, *p_B;
	MATH_tdst_Vector        Sprite[5];
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
		/* BEGIN DRAW SPRITE */
		if (Size)
		{
			*(ULONG *)&Sprite[4].x =  (*(ULONG *)&st_Crnt.stColor.a << 10) & 0xff000000;
			*(ULONG *)&Sprite[4].x |= (*(ULONG *)&st_Crnt.stColor.r << 2) & 0xff0000;
			*(ULONG *)&Sprite[4].x |= (*(ULONG *)&st_Crnt.stColor.g >> 6) & 0xff00;
			*(ULONG *)&Sprite[4].x |= (*(ULONG *)&st_Crnt.stColor.b >> 14) & 0xff;
			if (is->p_SpriteGen->flags & MAT_SPR_TableMapped)
			{
				Sprite[0] . x = (+ is->UVector.x + is->VVector.BasePoint.x) * Size + st_CrntNoise.x;
				Sprite[0] . y = (+ is->UVector.y + is->VVector.BasePoint.y) * Size + st_CrntNoise.y;
				Sprite[0] . z = (+ is->UVector.z + is->VVector.BasePoint.z) * Size + st_CrntNoise.z;
				Sprite[1] . x = (- is->UVector.x + is->VVector.BasePoint.x) * Size + st_CrntNoise.x;
				Sprite[1] . y = (- is->UVector.y + is->VVector.BasePoint.y) * Size + st_CrntNoise.y;
				Sprite[1] . z = (- is->UVector.z + is->VVector.BasePoint.z) * Size + st_CrntNoise.z;
				Sprite[2] . x = (- is->UVector.x - is->VVector.BasePoint.x) * Size + st_CrntNoise.x;
				Sprite[2] . y = (- is->UVector.y - is->VVector.BasePoint.y) * Size + st_CrntNoise.y;
				Sprite[2] . z = (- is->UVector.z - is->VVector.BasePoint.z) * Size + st_CrntNoise.z;
				Sprite[3] . x = (+ is->UVector.x - is->VVector.BasePoint.x) * Size + st_CrntNoise.x;
				Sprite[3] . y = (+ is->UVector.y - is->VVector.BasePoint.y) * Size + st_CrntNoise.y;
				Sprite[3] . z = (+ is->UVector.z - is->VVector.BasePoint.z) * Size + st_CrntNoise.z;
			}
			else
			{
				Sprite[0] . x = st_CrntNoise.x  + (- is->XCam.x - is->YCam.x) * Size;
				Sprite[0] . y = st_CrntNoise.y  + (- is->XCam.y - is->YCam.y) * Size;
				Sprite[0] . z = st_CrntNoise.z  + (- is->XCam.z - is->YCam.z) * Size;
				Sprite[1] . x = st_CrntNoise.x  + (+ is->XCam.x - is->YCam.x) * Size;
				Sprite[1] . y = st_CrntNoise.y  + (+ is->XCam.y - is->YCam.y) * Size;
				Sprite[1] . z = st_CrntNoise.z  + (+ is->XCam.z - is->YCam.z) * Size;
				Sprite[2] . x = st_CrntNoise.x  + (+ is->XCam.x + is->YCam.x) * Size;
				Sprite[2] . y = st_CrntNoise.y  + (+ is->XCam.y + is->YCam.y) * Size;
				Sprite[2] . z = st_CrntNoise.z  + (+ is->XCam.z + is->YCam.z) * Size;
				Sprite[3] . x = st_CrntNoise.x  + (- is->XCam.x + is->YCam.x) * Size;
				Sprite[3] . y = st_CrntNoise.y  + (- is->XCam.y + is->YCam.y) * Size;
				Sprite[3] . z = st_CrntNoise.z  + (- is->XCam.z + is->YCam.z) * Size;
			}
			is ->pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSprite, (ULONG)Sprite);
			is->NumberOfSpritesGenerated++;
		}
		/* END DRAW SPRITE */
		
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
#endif
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
void MAT_SPR_ComputeScalarVector(MATH_tdst_Vector *PR,MATH_tdst_Vector *PA,MATH_tdst_Vector *PB,MATH_tdst_Vector *PC,float A,float B,float C)
{
}


/*
=======================================================================================================================
=======================================================================================================================
*/
void MAT_RasterizeUV_Triangle(MAT_SPR_InternalStruct *is)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG           Counter,Counter2,ulColor,ulColorOr;
    UInc            **p_SRC, **p_IN, **p_OUT, **p_SWP;
    ULONG           SRCNUM;
    UInc            LocalBuffer[3] ONLY_PSX2_ALIGNED(16); 
    UInc            *p_Local;
    UInc            *p_LocalBuffer[24];
    float           MMMin, MMMax,Alpha;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    p_SRC = p_LocalBuffer;
    p_OUT = p_LocalBuffer + 8;
    p_IN = p_LocalBuffer + 16;
	ulColorOr = 0;
	p_Local = &LocalBuffer[0];
    for(Counter = 0; Counter < 3; Counter++)
    {
        p_Local->BasePoint = *VCast( &is->dst_Point[is->p_XYZ_Index[Counter]] );
		GEO_UseNormals(is->pst_Obj);
		MAT_SPR_MULADD_Vector(&LocalBuffer[Counter].BasePoint, &is->pst_Obj->dst_PointNormal[is->p_XYZ_Index[Counter]], is->p_SpriteGen->ZExtraction * 0.1f);
        p_Local->BaseUV.fU = is->dst_UV[is->p_UV_Index[Counter]].fU * (float) is->SizeOfTexture;
        p_Local->BaseUV.fV = is->dst_UV[is->p_UV_Index[Counter]].fV * (float) is->SizeOfTexture;
        p_Local->MipMapSize = 1.0f;
		if (is->pst_DD->pst_ComputingBuffers->CurrentColorField) 
		{
			ulColor = is->pst_DD->pst_ComputingBuffers->CurrentColorField[is->p_XYZ_Index[Counter]] /*| ulOGLSetCol_Or*/; 
			ulColor ^= is->pst_DD->pst_ComputingBuffers->ulColorXor;
			if (is->pst_DD->pst_ComputingBuffers->CurrentAlphaField)
			{
				ulColor &= 0x00ffffff;
				ulColor |= is->pst_DD->pst_ComputingBuffers->CurrentAlphaField[is->p_XYZ_Index[Counter]];
			}
		} else 
		{
			ulColor = is -> pst_DD->pst_ComputingBuffers->ul_Ambient;
			if (is->pst_DD->pst_ComputingBuffers->CurrentAlphaField){ 
				ulColor &= 0x00ffffff;
				ulColor |= is->pst_DD->pst_ComputingBuffers->CurrentAlphaField[is->p_XYZ_Index[Counter]];
			}
		}
		ulColorOr |= ulColor;
		if ((is->p_SpriteGen->flags & MAT_SPR_VertexAlphaIsTransparency ) && (MAT_GET_Blending(is->pst_MttxtLvl->ul_Flags) >= MAT_Cc_Op_Add) )
		{
			p_Local->stColor.a = (float)((ulColor  >> 24) & 0xff) * MAT_SPR_ColorReductionCoef + 0.05f;
			Alpha = p_Local->stColor.a / 255.0f;
			p_Local->stColor.r = Alpha * (float)((ulColor  >> 16) & 0xff) * MAT_SPR_ColorReductionCoef + 0.05f;
			p_Local->stColor.g = Alpha * (float)((ulColor  >> 8) & 0xff)  * MAT_SPR_ColorReductionCoef + 0.05f;
			p_Local->stColor.b = Alpha * (float)((ulColor  >> 0) & 0xff)  * MAT_SPR_ColorReductionCoef + 0.05f;
		}else
		{
			p_Local->stColor.a = (float)((ulColor  >> 24) & 0xff) * MAT_SPR_ColorReductionCoef + 0.05f;
			p_Local->stColor.r = (float)((ulColor  >> 16) & 0xff) * MAT_SPR_ColorReductionCoef + 0.05f;
			p_Local->stColor.g = (float)((ulColor  >> 8) & 0xff)  * MAT_SPR_ColorReductionCoef + 0.05f;
			p_Local->stColor.b = (float)((ulColor  >> 0) & 0xff)  * MAT_SPR_ColorReductionCoef + 0.05f;
		}
        p_LocalBuffer[Counter] = p_Local++;
    }
	
	/* ColorCull */
	if ((ulColorOr & 0xfcfcfc) == 0)
	{
		if (MAT_GET_Blending(is->pst_MttxtLvl->ul_Flags) >= MAT_Cc_Op_Add)
			return;
	}
	if ((ulColorOr & 0xfc000000) == 0)
	{
		if (is->p_SpriteGen->flags & (MAT_SPR_VertexAlphaIsSize | MAT_SPR_VertexAlphaIsTransparency | MAT_SPR_VertexAlphaIsDensity))
			return;
	}
	// 0.08 return;
	
    is->NumberOfPoints = SRCNUM = 3;
	
	
    /* BEGIN COMPUTE UVECTOR & VVECTOR */
    {
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		float               Min,Max,Min0D,Max0D,Interp;
		ULONG				P1,P2,P3;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ILAF(P1) = (*p_SRC)[1].BaseUV.fU - (*p_SRC)[0].BaseUV.fU;
		P1 >>= 31;
		P2 = P1 ^ 1;
		if ((*p_SRC)[2].BaseUV.fU < (*p_SRC)[P1].BaseUV.fU) 
		P1 = 2;
		else
		if ((*p_SRC)[2].BaseUV.fU > (*p_SRC)[P2].BaseUV.fU) 
		P2 = 2;
		Min = (*p_SRC)[P1].BaseUV.fU;
		Max = (*p_SRC)[P2].BaseUV.fU;
		Min0D = Min/* - 0.5f*/ + MAT_C_f_0Bits;
		Max0D = Max/* - 0.5f*/ + MAT_C_f_0Bits;
		if (IFAL(Max0D) == IFAL(Min0D)) return;
		P3 = (P2 | P1) ^ 3;
		Interp = ((*p_SRC)[P3].BaseUV.fU - Min) * fOptInv(Max - Min) ;
		Min = (*p_SRC)[P1].BaseUV.fV + ((*p_SRC)[P2].BaseUV.fV - (*p_SRC)[P1].BaseUV.fV) * Interp;
		Min = fOptInv(Min - (*p_SRC)[P3].BaseUV.fV);
		MAT_SPR_SUB_MUL_SUB_MUL_flt_tbl((float *)&is->VVector, (float *)&(*p_SRC)[P1], (float *)&(*p_SRC)[P2], (float *)&(*p_SRC)[P3], Interp  , Min );

		ILAF(P1) = (*p_SRC)[1].BaseUV.fV - (*p_SRC)[0].BaseUV.fV;
		P1 >>= 31;
		P2 = P1 ^ 1;
		if ((*p_SRC)[2].BaseUV.fV < (*p_SRC)[P1].BaseUV.fV) 
		P1 = 2;
		else
		if ((*p_SRC)[2].BaseUV.fV > (*p_SRC)[P2].BaseUV.fV) 
		P2 = 2;
		Min = (*p_SRC)[P1].BaseUV.fV;
		Max = (*p_SRC)[P2].BaseUV.fV;
		Min0D = Min/* - 0.5f*/ + MAT_C_f_0Bits;
		Max0D = Max/* - 0.5f*/ + MAT_C_f_0Bits;
		if (IFAL(Max0D) == IFAL(Min0D)) return;
		P3 = (P2 | P1) ^ 3;
		Interp = ((*p_SRC)[P3].BaseUV.fV - Min) * fOptInv(Max - Min);
		Min = (*p_SRC)[P1].BaseUV.fU + ((*p_SRC)[P2].BaseUV.fU - (*p_SRC)[P1].BaseUV.fU) * Interp;
		Min = fOptInv(Min - (*p_SRC)[P3].BaseUV.fU);
		MAT_SPR_SUBMULSUBMUL_Vector(&is->UVector  , &(*p_SRC)[P1].BasePoint, &(*p_SRC)[P2].BasePoint, &(*p_SRC)[P3].BasePoint , Interp , Min);//*/
	}
	/* END COMPUTE UVECTOR & VVECTOR */
	/* BEGIN COMPUTE TEXTELSSURFACE & distortion MAX */
	{
		MATH_tdst_Vector    C;
		float fL1,fL2;
		MATH_CrossProduct(&C, &is->UVector, &is->VVector.BasePoint);
		is->TexelsSurf = fOptSqrt(MATH_f_NormVector(&C));
		fL1 = MATH_f_SqrNormVector(&is->UVector);
		fL2 = MATH_f_SqrNormVector(&is->VVector.BasePoint);
		if (fL1 > fL2)
		fL1 = fOptSqrt(fL1);
		else
		fL1 = fOptSqrt(fL2);
		if (fL1 > is->p_SpriteGen->DistortionMax * is->TexelsSurf) return;
		is->TexelsSurf *= 200.0f;
		is->fCurrentSize = is->fCurrentSize4All;
		if (!(is->p_SpriteGen->flags & MAT_SPR_SizeFactorIsRealSize))
			is->fCurrentSize *= is->TexelsSurf;
    }
    /* END COMPUTE TEXTELSSURFACE & distortion MAX */
	
#ifdef MAT_SPR_PICTURE_ACTIVATED		
	if ( is -> pimg_XYZS_Picture ) 
	{
		float fLoc;
		GEO_UseNormals(is->pst_Obj);
		MATH_AddVector(&is->stZtriangle, &is->pst_Obj->dst_PointNormal[is->p_XYZ_Index[0]] , &is->pst_Obj->dst_PointNormal[is->p_XYZ_Index[1]]);
		MATH_AddVector(&is->stZtriangle, &is->pst_Obj->dst_PointNormal[is->p_XYZ_Index[2]] , &is->stZtriangle);
		fLoc = MATH_f_NormVector(&is->UVector);
		MATH_ScaleEqualVector(&is->stZtriangle , 0.333333333f * fLoc * is->p_SpriteGen->fBumpFactor);
	}
#endif	
	// 0.4 == 0.32 return;
	
	is->UIncCachePos = 0;
	/* Screen Clipping */
	for (Counter = 0 ; Counter < 4 ; Counter ++)
	{
		if (is->p_SpriteGen->flags & MAT_SPR_SizeFactorIsRealSize)
			is->Thresh = is->ClippingDistances[Counter] + is->TexelsSurf * 0.03f / 200.0f + is->fCurrentSize4All * 0.1f;
		else
			is->Thresh = is->ClippingDistances[Counter] + is->fCurrentSize4All * is->TexelsSurf * 0.03f / 200.0f;
		for (Counter2 = 0 ; Counter2 < SRCNUM ; Counter2 ++)
		{
			/* Compute plane distances */
			p_SRC[Counter2]->OoZ =  is->ClippingVector[Counter] . x * p_SRC[Counter2]->BasePoint . x +
			is->ClippingVector[Counter] . y * p_SRC[Counter2]->BasePoint . y +
			is->ClippingVector[Counter] . z * p_SRC[Counter2]->BasePoint . z ;
		}
        MAT_RasterizeUV_CLIP(is, p_IN, p_OUT, p_SRC, SRCNUM);
        /* Src is p_in */
        SRCNUM = is->NumberOfPointsIn;
        if(!SRCNUM) return;
        p_SWP = p_SRC;p_SRC = p_IN;p_IN = p_SWP;
	}
	
	/* ZClipping */
	is->CurrentDelta = 0.0f;
	MMMin = MAT_SPR_INFINITE;
	MMMax = -MAT_SPR_INFINITE;
	
	for (Counter2 = 0 ; Counter2 < SRCNUM ; Counter2 ++)
	{
		p_SRC[Counter2]->OoZ =  MATH_f_DotProduct(&is->ClippingVector[4], &p_SRC[Counter2]->BasePoint) - is->ClippingDistances[4];
        MMMin = fMin(MMMin,p_SRC[Counter2]->OoZ);
        MMMax = fMax(MMMax,p_SRC[Counter2]->OoZ);
	}
	
    if(MMMax < MAT_SPR_ZMin) return;
    if(MMMin > MAT_SPR_GetFarestValue(is)) return;
	
	
    if(MMMin < MAT_SPR_ZMin)
    {
        MMMin = MAT_SPR_ZMin;
        is->Thresh = MMMin;
        MAT_RasterizeUV_CLIP(is, p_IN, p_OUT, p_SRC, SRCNUM);
        /* Src is pout */
        SRCNUM = is->NumberOfPointsOut;
        if(!SRCNUM) return;
		/* SWAP */
        p_SWP = p_SRC;p_SRC = p_OUT;p_OUT = p_SWP;
    }
	
    /* LowestLevel CLipping */
    if(MMMax > MAT_SPR_GetFarestValue(is))
    {
        MMMax = MAT_SPR_GetFarestValue(is);
        is->Thresh = MMMax;
        MAT_RasterizeUV_CLIP(is, p_IN, p_OUT, p_SRC, SRCNUM);
        /* Src is pin */
        SRCNUM = is->NumberOfPointsIn;
        if(!SRCNUM) return;
		/* SWAP */
        p_SWP = p_SRC;p_SRC = p_IN;p_IN = p_SWP;
    }
	
    MMMax = MAT_SPR_AlignValue(is, MMMax);
    is->pp_ClippedUInc = p_OUT;
	
	// 0.43 == 0.03 return;
	
	
    while((MMMin <= MMMax) && SRCNUM) // 0.46 == 0.03
    {
        is->CurrentDelta = MMMax;
        MMMax = MAT_SPR_GetNextValue(is, MMMax);
        is->CurrentDelta -= MMMax;
        is->CurrentDelta = -fOptInv(is->CurrentDelta);
        if(MMMax <= 0.0f) is->CurrentDelta = 0.0f;
        is->Thresh = MMMax;
		
		/* Mipmapping clipping */
        MAT_RasterizeUV_CLIP(is, p_IN, p_OUT, p_SRC, SRCNUM);
        is->NumberOfPoints = is->NumberOfPointsOut;
		p_SWP = p_OUT;
		
		if (is->p_SpriteGen->flags & MAT_SPR_DisearpearWthTransparency)
		{
			if (MAT_GET_Blending(is->pst_MttxtLvl->ul_Flags) >= MAT_Cc_Op_Add)
			{
				Counter2 = is->NumberOfPoints;
				while (Counter2--)
				{
					if (IFAL((*p_SWP)->MipMapSize) & 0x80000000) (*p_SWP)->MipMapSize = 0.0f;
					(*p_SWP)->stColor . r = (((*p_SWP)->stColor . r - 0.05f) * (*p_SWP)->MipMapSize) + 0.05f;
					(*p_SWP)->stColor . g = (((*p_SWP)->stColor . g - 0.05f) * (*p_SWP)->MipMapSize) + 0.05f;
					(*p_SWP)->stColor . b = (((*p_SWP)->stColor . b - 0.05f) * (*p_SWP)->MipMapSize) + 0.05f;
					p_SWP++;
				}
			} else
			{
				Counter2 = is->NumberOfPoints;
				while (Counter2--)
				{
					if (IFAL((*p_SWP)->MipMapSize) & 0x80000000) (*p_SWP)->MipMapSize = 0.0f;
					(*p_SWP)->stColor . a = (((*p_SWP)->stColor . a - 0.05f) * (*p_SWP)->MipMapSize) + 0.05f;
					p_SWP++;
				}
			} 		
		}
		
        
		MAT_RasterizeUV_nGone(is);
        /* Src is pin */
        SRCNUM = is->NumberOfPointsIn;
        p_SWP = p_SRC;p_SRC = p_IN;p_IN = p_SWP;
		
#ifdef MAT_SPR_D_SL_UC
        {
            /*~~~~~~~~~~~~~~~~*/
            void    *ppp[2];
            /*~~~~~~~~~~~~~~~~*/
            ppp[0] = (void *) &is->pp_ClippedUInc[is->NumberOfPoints - 1]->BasePoint;
            for(Counter = 0; Counter < is->NumberOfPoints; Counter++)
            {
                ppp[1] = (void *) &is->pp_ClippedUInc[Counter]->BasePoint;
                is ->pst_DD->st_GDI.pfnl_Request(is->pst_DD, GDI_Cul_Request_DrawLine, (ULONG) ppp);
                ppp[0] = ppp[1];
            }
        }
#endif
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
void MAT_RasterizeUV
(
 GDI_tdst_DisplayData                *pst_DD,
 GEO_tdst_Object                     *pst_Obj,
 MAT_tdst_MTLevel                    *_pst_MttxtLvl,
 GEO_tdst_ElementIndexedTriangles    *pst_Element
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_IndexedTriangle    *t, *tend;
    MAT_SPR_InternalStruct      is;
    static ULONG                bFirst = 1;
    MATH_tdst_Matrix			Matrix ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Vector			Local,Local2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if defined(PSX2_TARGET) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if (NoSPR) return;
#endif	
    is.pst_DD = pst_DD;
    is.pst_Obj = pst_Obj;
    is.flags = _pst_MttxtLvl->ul_Flags;
	is.dst_Point = pst_DD->p_Current_Vertex_List;
    is.dst_UV = (GEO_tdst_UV *) pst_DD->pst_ComputingBuffers->Current;
    is.Focale = fOptCos(is.pst_DD->st_Camera.f_FieldOfVision * 0.5f) * fOptInv(fOptSin(is.pst_DD->st_Camera.f_FieldOfVision * 0.5f));
	is.p_SpriteGen = *(MAT_tdst_SpriteGen **)&TEX_gst_GlobalList.dst_Texture[_pst_MttxtLvl->s_TextureId].w_Width;
#ifdef PSX2_TARGET
	if (is.p_SpriteGen->flags & MAT_SPR_TableMapped) return;
#endif		
	is.fCurrentSize4All = is.p_SpriteGen->Size * 0.01f;
#ifdef PSX2_TARGET	
	MATH_GetScale(&Local, &is.pst_DD->st_MatrixStack.pst_CurrentMatrix);
	is.fCurrentSize4All *= (Local.x + Local.y + Local.z) * 0.33333f;
#endif

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
	
	
	
	
	GDI_SetTextureBlending((*pst_DD),is.p_SpriteGen->s_TextureIndex, _pst_MttxtLvl->ul_Flags ,0 );
#ifdef PSX2_TARGET
	GSP_DrawSpriteBegin();
#else
	is .pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_BeforeDrawSprite, 0);
#endif
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
	
    t = pst_Element->dst_Triangle;
    tend = t + pst_Element->l_NbTriangles;
    if(pst_DD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
    {
        while(t < tend)
        {
            is.p_XYZ_Index = t->auw_Index;
            is.p_UV_Index = t->auw_Index;	
            MAT_RasterizeUV_Triangle(&is);
            t++;
        }
	}
#ifndef PSX2_TARGET	
#ifndef _GAMECUBE
    else if(pst_DD->ul_DisplayInfo & GDI_Cul_DI_FaceMap)
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~*/
        unsigned short  Lie[3];
        /*~~~~~~~~~~~~~~~~~~~~~~~*/
		
        while(t < tend)
        {
            is.p_XYZ_Index = t->auw_Index;
            is.p_UV_Index = Lie;
            Lie[0] = (unsigned short) ((t->ul_MaxFlags >> 7) & 3);
            Lie[1] = (unsigned short) ((t->ul_MaxFlags >> 9) & 3);
            Lie[2] = (unsigned short) ((t->ul_MaxFlags >> 11) & 3);
            MAT_RasterizeUV_Triangle(&is);
            t++;
        }
    }
#endif    
#endif    
    else
    {
        while(t < tend)
        {
            is.p_XYZ_Index = t->auw_Index;
            is.p_UV_Index = t->auw_UV;
            MAT_RasterizeUV_Triangle(&is);
            t++;
        }
    }
#ifdef PSX2_TARGET
	GSP_DrawSpriteEnd();
#else
	is .pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_AfterDrawSprite, 0);
    PRO_IncRasterLong(&pst_DD->pst_Raster->st_NbGenSprites, is.NumberOfSpritesGenerated);
#endif
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

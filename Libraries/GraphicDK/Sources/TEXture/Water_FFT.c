#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "Water_FFT.h"

#include "BASe/BENch/BENch.h"

#ifndef _M_IX86
#define __forceinline _inline_
#endif

//#define FFT_TEST   
//#undef PSX2_TARGET
/* 
---------------------------------------------------------------------------------------------------------------
---------------------------------------- WATER_FFT TOOLS ------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void*WATERFFT_Alloc		(unsigned int ulSize)			{ return 	MEM_p_AllocAlign(ulSize,32);}
void WATERFFT_Free		(void *ptr)						{ 			MEM_FreeAlign(ptr);}
void WATERFFT_Memset0	(void *ptr,unsigned int size)	{			L_memset(ptr,0,size);}

#define PSX2_ASM_PREFETCH(a) pref 0 , a //lq	$0 , a
/* 
---------------------------------------------------------------------------------------------------------------
-------------------------------------------- 2D FFT -----------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
static FFT_Complex FFT_FACTORS_C[] = {{-1.0f,0.0f},{0.0f,1.0f},{0.707106781f,0.707106781f},{0.923879533f,0.382683432f},{0.98078528f,0.195090322f},{0.995184727f,0.09801714f},{0.998795456f,0.049067674f},{0.999698819f,0.024541229f},{0.999924702f,0.012271538f},{0.999981175f,0.006135885f},{0.999995294f,0.003067957f},{0.999998823f,0.00153398f},{0.999999706f,0.00076699f}};
void WATERFFT_CorrectHeight(FFT_Complex *pSRC , unsigned int BorderSizePo2)
{
	register unsigned int W,H,Orig,*PTR ,BorderSize;
	PTR = (unsigned int *)pSRC ;
	BorderSize = 1 << BorderSizePo2;
	H = BorderSize;
	Orig = 2;
	while (H--)
	{
		W = BorderSize >> 1;
		PTR += Orig;
		while (W--) 
		{
			PTR[0] += 0x80000000;
			PTR[1] += 0x80000000;
#ifdef PSX2_TARGET			
			asm __volatile__ ("PSX2_ASM_PREFETCH(0x80(PTR))");
#endif			
			PTR+=4;
		}
		PTR -= Orig;
		Orig ^= 2;
	}
}
/* 
---------------------------------------------------------------------------------------------------------------
-------------------------------------------- 1D FFT REAL-TIME -------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
_inline_ void WATERFFT_ComplexMulSubAdd(FFT_Complex *p_xy00 , FFT_Complex *p_xy01 , FFT_ComplexCOS *p_CosTableX )
{
	float Tr0,Ti0;
	float Cxr,Cxi;

	Cxr = p_CosTableX->re1;
	Cxi = p_CosTableX->im1;

	Tr0	= Cxr * p_xy01->re - Cxi * p_xy01->im;
	Ti0	= Cxr * p_xy01->im + Cxi * p_xy01->re;

	p_xy01->re = p_xy00->re + Tr0;
	p_xy01->im = p_xy00->im + Ti0;
	p_xy00->re = p_xy00->re - Tr0;
	p_xy00->im = p_xy00->im - Ti0;

}
/* 
---------------------------------------------------------------------------------------------------------------
-------------------------------------------- 2D FFT REAL-TIME -------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
__forceinline void WATERFFT_ComplexMulSubAdd_SQR(register FFT_Complex *p_xy00 , register FFT_Complex *p_xy10 , register FFT_Complex *p_xy01 , register FFT_Complex *p_xy11 , register FFT_ComplexCOS *p_CosTableX , register FFT_ComplexCOS *p_CosTableY )
{
#ifdef _GAMECUBE
	register __vec2x32float__  Tr0,Tr1,Tr2,Tr3;
	register __vec2x32float__  Cxr,Cxi,Cyr,Cyi ;
	register __vec2x32float__  xy00,xy01,xy10,xy11,TMP0,TMP1;
	asm 
	{
		psq_l		xy00, 0(p_xy00), 0, 0
		psq_l		xy10, 0(p_xy10), 0, 0
		psq_l		xy01, 0(p_xy01), 0, 0
		psq_l		xy11, 0(p_xy11), 0, 0
		psq_l		Cxr, 0(p_CosTableX), 0, 0
		psq_l		Cxi, 8(p_CosTableX), 0, 0
		psq_l		Cyr, 0(p_CosTableY), 0, 0
		psq_l		Cyi, 8(p_CosTableY), 0, 0

		ps_muls0 	Tr0 , Cxr , xy01			//	Tr0	= Cxr * p_xy01->re - Cxi * p_xy01->im;
		ps_madds1 	Tr0 , Cxi , xy01 , Tr0		//	Ti0	= Cxi * p_xy01->re + Cxr * p_xy01->im;
		ps_muls0  	Tr1 , Cxr , xy11			//	Tr1	= Cxr * p_xy11->re - Cxi * p_xy11->im;
		ps_madds1 	Tr1 , Cxi , xy11 , Tr1		//	Ti1	= Cxi * p_xy11->re + Cxr * p_xy11->im;
		ps_sub 		TMP0 , xy10 , Tr1
		ps_add 		TMP1 , xy10 , Tr1
		ps_muls0  	Tr2 , Cyr , TMP0				//	Tr2	= Cyr * (p_xy10->re - Tr1) - Cyi * (p_xy10->im - Ti1);
		ps_madds1 	Tr2 , Cyi , TMP0 , Tr2			//	Ti2	= Cyi * (p_xy10->re - Tr1) + Cyr * (p_xy10->im - Ti1);
		ps_muls0  	Tr3 , Cyr , TMP1				//	Tr3	= Cyr * (p_xy10->re + Tr1) - Cyi * (p_xy10->im + Ti1);
		ps_madds1 	Tr3 , Cyi , TMP1 , Tr3			//	Ti3	= Cyi * (p_xy10->re + Tr1) + Cyr * (p_xy10->im + Ti1);
		
		ps_add		xy11 , xy00 , Tr0
		ps_add		xy11 , xy11 , Tr3
		
		ps_sub		xy10 , xy00 , Tr0
		ps_add		xy10 , xy10 , Tr2
		
		ps_add		xy01 , xy00 , Tr0
		ps_sub		xy01 , xy01 , Tr3
	
		ps_sub		xy00 , xy00 , Tr0
		ps_sub		xy00 , xy00 , Tr2

		psq_st		xy11, 0(p_xy11), 0, 0;
		psq_st		xy10, 0(p_xy10), 0, 0;
		psq_st		xy01, 0(p_xy01), 0, 0;
		psq_st		xy00, 0(p_xy00), 0, 0;
	}
#else	
#ifdef PSX2_TARGET
	register long  xy00,xy01,xy10,xy11;
	asm __volatile__ ("
	.set noreorder
		lqc2  $vf20, 0x00(p_CosTableX) // XIM  XRE  XIM  XRE 
		lqc2  $vf21, 0x10(p_CosTableX) // XRE -XIM  XRE -XIM
		lqc2  $vf22, 0x00(p_CosTableY) // YIM  YRE  YIM  YRE 
		lqc2  $vf23, 0x10(p_CosTableY) // YRE -YIM  YRE -YIM 
		ld  xy00, 0(p_xy00)
		ld  xy10, 0(p_xy10)
		ld  xy01, 0(p_xy01)
		ld  xy11, 0(p_xy11)
		PSX2_ASM_PREFETCH(0x40(p_xy01))
		qmtc2	xy00,$vf02
		qmtc2	xy10,$vf10
		qmtc2	xy01,$vf01
		qmtc2	xy11,$vf11
		vmulax	$acc , $vf20 , $vf01x	
		vmaddy	$vf15 , $vf21 , $vf01y
		vmulax	$acc , $vf20 , $vf11x	
		vmaddy	$vf16 , $vf21 , $vf11y
		vsub	$vf29 , $vf10 , $vf16
		vadd	$vf30 , $vf10 , $vf16
		vadd	$vf08 , $vf02 , $vf15
		vsub	$vf09 , $vf02 , $vf15
		PSX2_ASM_PREFETCH(0x40(p_xy11))
		vmulax	$acc  , $vf22 , $vf29x	
		vmaddy	$vf17 , $vf23 , $vf29y
		vmulax	$acc , $vf22 , $vf30x	
		vmaddy	$vf18 , $vf23 , $vf30y
		vadd	$vf10 , $vf09 , $vf17
		vadd	$vf11 , $vf08 , $vf18
		vsub	$vf02 , $vf09 , $vf17
		vsub	$vf01 , $vf08 , $vf18
		qmfc2	xy00,$vf02
		qmfc2	xy10,$vf10
		qmfc2	xy01,$vf01
		qmfc2	xy11,$vf11
		sd  xy00, 0(p_xy00)
		sd  xy10, 0(p_xy10)
		sd  xy01, 0(p_xy01)
		sd  xy11, 0(p_xy11)
		.set reorder
	");

#else
	float Tr0,Ti0;
	float Tr1,Ti1;
	float Tr2,Ti2;
	float Tr3,Ti3;
	float Cxr,Cxi,Cyr,Cyi;

	Cxr = p_CosTableX->re1;
	Cxi = p_CosTableX->im1;
	Cyr = p_CosTableY->re1;
	Cyi = p_CosTableY->im1;

	Tr0	= Cxr * p_xy01->re - Cxi * p_xy01->im;
	Ti0	= Cxr * p_xy01->im + Cxi * p_xy01->re;
	Tr1	= Cxr * p_xy11->re - Cxi * p_xy11->im;
	Ti1	= Cxr * p_xy11->im + Cxi * p_xy11->re;
	Tr2	= Cyr * (p_xy10->re - Tr1) - Cyi * (p_xy10->im - Ti1);
	Ti2	= Cyr * (p_xy10->im - Ti1) + Cyi * (p_xy10->re - Tr1);
	Tr3	= Cyr * (p_xy10->re + Tr1) - Cyi * (p_xy10->im + Ti1);
	Ti3	= Cyr * (p_xy10->im + Ti1) + Cyi * (p_xy10->re + Tr1);

	p_xy11->re = p_xy00->re + Tr0 + Tr3;
	p_xy11->im = p_xy00->im + Ti0 + Ti3;
	p_xy10->re = p_xy00->re - Tr0 + Tr2;
	p_xy10->im = p_xy00->im - Ti0 + Ti2;
	p_xy01->re = p_xy00->re + Tr0 - Tr3;
	p_xy01->im = p_xy00->im + Ti0 - Ti3;
	p_xy00->re = p_xy00->re - Tr0 - Tr2;
	p_xy00->im = p_xy00->im - Ti0 - Ti2;
#endif	
#endif
}
#ifdef PSX2_TARGET

#define VF_PCY1 $vf01
#define VF_PCY2 $vf02
#define VF_PCW1 $vf03
#define VF_PCW2 $vf04

_inline_ void WATERFFT_PreloadCosTableY(register FFT_ComplexCOS *p_CosTableY,register FFT_ComplexCOS *p_CosTableYP1 )
{
//	register long  xy01,xy11;
	asm __volatile__ ("
	.set noreorder
		lqc2   VF_PCY1, 0x00(p_CosTableY) // YIM  YRE  YIM  YRE 
		lqc2   VF_PCY2, 0x10(p_CosTableY) // YRE -YIM  YRE -YIM 
		lqc2   VF_PCW1, 0x00(p_CosTableYP1) // YIM  YRE  YIM  YRE 
		lqc2   VF_PCW2, 0x10(p_CosTableYP1) // YRE -YIM  YRE -YIM 
		.set reorder
		");
}
#endif
__forceinline void WATERFFT_ComplexMulSubAdd_SQR_DoubleXY(register FFT_Complex *p_xy00 , register FFT_Complex *p_xy10 , register FFT_Complex *p_xy01 , register FFT_Complex *p_xy11 , register FFT_ComplexCOS *p_CosTableX , register FFT_ComplexCOS *p_CosTableXP1 , register FFT_ComplexCOS *p_CosTableY , register FFT_ComplexCOS *p_CosTableYP1 , register ULONG Pitch)
{
#ifdef PSX2_TARGET
#define VF_PCX1 $vf05
#define VF_PCX2 $vf06
#define VF_PCZ1 $vf07
#define VF_PCZ2 $vf08

#define V_XY00a $vf09
#define V_XY01a $vf10
#define V_XY01ae(a) $vf10##a
#define V_XY10a $vf11
#define V_XY11a $vf12
#define V_XY11ae(a) $vf12##a

#define V_XY00b $vf13
#define V_XY01b $vf14
#define V_XY01be(a) $vf14##a
#define V_XY10b $vf15
#define V_XY11b $vf16
#define V_XY11be(a) $vf16##a

#define VF_TR0a	$vf17
#define VF_TR1a	$vf18
#define VF_TR2a	$vf19
#define VF_TR3a	$vf20

#define VF_TR0b	$vf21
#define VF_TR1b	$vf22
#define VF_TR2b	$vf23
#define VF_TR3b	$vf24

#define V_TMP0a	$vf25
#define V_TMP0ae(a)	$vf25##a
#define V_TMP1a	$vf26
#define V_TMP1ae(a)	$vf26##a
#define V_TMP2a	$vf27

#define V_TMP0b	$vf28
#define V_TMP0be(a)	$vf28##a
#define V_TMP1b	$vf29
#define V_TMP1be(a)	$vf29##a
#define V_TMP2b	$vf30

// !!! DO it 4 by 4 
	register long  p_xy00p,p_xy01p,p_xy10p,p_xy11p;

	asm __volatile__ ("
	.set noreorder
		add			p_xy01p , p_xy01 , Pitch
		lqc2 		V_XY01a , 0x000 ( p_xy01 )
		lqc2  		VF_PCX1 , 0x000 ( p_CosTableX  ) 	// XIM  XRE  XIM  XRE 
		lqc2  		VF_PCX2 , 0x010 ( p_CosTableX  ) 	// XRE -XIM  XRE -XIM
		add			p_xy11p , p_xy11 , Pitch
		lqc2 		V_XY01b , 0x000 ( p_xy01p )
		lqc2  		VF_PCZ1 , 0x000 ( p_CosTableXP1 ) 	// XIM  XRE  XIM  XRE 
		lqc2  		VF_PCZ2 , 0x010 ( p_CosTableXP1 ) 	// XRE -XIM  XRE -XIM
		add			p_xy00p , p_xy00 , Pitch
		lqc2 		V_XY11a , 0x000 ( p_xy11)
		vmulx.xy	VF_TR0a , VF_PCX1 , V_XY01ae(x)
		vmulx.xy	VF_TR0b , VF_PCX1 , V_XY01be(x)	
		vmuly.xy	V_TMP0a , VF_PCX2 , V_XY01ae(y)
		vmuly.xy	V_TMP0b , VF_PCX2 , V_XY01be(y)
		add			p_xy10p , p_xy10 , Pitch
		lqc2 		V_XY11b , 0x000 ( p_xy11p)
		vmulz.zw	VF_TR0a , VF_PCZ1 , V_XY01ae(z)	
		vmulz.zw	VF_TR0b , VF_PCZ1 , V_XY01be(z)	
		vmulw.zw	V_TMP0a , VF_PCZ2 , V_XY01ae(w)
		vmulw.zw	V_TMP0b , VF_PCZ2 , V_XY01be(w)
		lqc2 		V_XY10a , 0x000 ( p_xy10)
		vmulx.xy	VF_TR1a , VF_PCX1 , V_XY11ae(x)
		vmulx.xy	VF_TR1b , VF_PCX1 , V_XY11be(x)	
		vmuly.xy	V_TMP1a , VF_PCX2 , V_XY11ae(y)
		vmuly.xy	V_TMP1b , VF_PCX2 , V_XY11be(y)
		lqc2 		V_XY10b , 0x000 ( p_xy10p)
		vmulz.zw	VF_TR1a , VF_PCZ1 , V_XY11ae(z)
		vmulz.zw	VF_TR1b , VF_PCZ1 , V_XY11be(z)
		vmulw.zw	V_TMP1a , VF_PCZ2 , V_XY11ae(w)
		vmulw.zw	V_TMP1b , VF_PCZ2 , V_XY11be(w)
		lqc2 		V_XY00a , 0x000 ( p_xy00)
		vadd		VF_TR0a , VF_TR0a , V_TMP0a
		vadd		VF_TR0b , VF_TR0b , V_TMP0b
		vadd		VF_TR1a , VF_TR1a , V_TMP1a
		vadd		VF_TR1b , VF_TR1b , V_TMP1b
		lqc2 		V_XY00b , 0x000 ( p_xy00p)
		PSX2_ASM_PREFETCH(0x40(p_xy10p))
		vsub		V_TMP0a , V_XY10a , VF_TR1a
		vsub		V_TMP0b , V_XY10b , VF_TR1b
		vadd		V_TMP1a , V_XY10a , VF_TR1a
		vadd		V_TMP1b , V_XY10b , VF_TR1b
		vadd		V_TMP2a , V_XY00a , VF_TR0a
		vadd		V_TMP2b , V_XY00b , VF_TR0b
		vsub		V_XY00a , V_XY00a , VF_TR0a
		vsub		V_XY00b , V_XY00b , VF_TR0b
		PSX2_ASM_PREFETCH(0x40(p_xy01p))
		vmulx.xy	VF_TR2a , VF_PCY1 , V_TMP0ae(x)	
		vmulx.xy	VF_TR2b , VF_PCW1 , V_TMP0be(x)	
		vmulx.xy	VF_TR3a , VF_PCY1 , V_TMP1ae(x)	
		vmulx.xy	VF_TR3b , VF_PCW1 , V_TMP1be(x)	
		vmuly.xy	V_TMP0a , VF_PCY2 , V_TMP0ae(y)
		vmuly.xy	V_TMP0b , VF_PCW2 , V_TMP0be(y)
		vmuly.xy	V_TMP1a , VF_PCY2 , V_TMP1ae(y)
		vmuly.xy	V_TMP1b , VF_PCW2 , V_TMP1be(y)
		vmulz.zw	VF_TR2a , VF_PCY1 , V_TMP0ae(z)
		vmulz.zw	VF_TR2b , VF_PCW1 , V_TMP0be(z)
		PSX2_ASM_PREFETCH(0x40(p_xy11p))
		vmulz.zw	VF_TR3a , VF_PCY1 , V_TMP1ae(z)	
		vmulz.zw	VF_TR3b , VF_PCW1 , V_TMP1be(z)	
		vmulw.zw	V_TMP0a , VF_PCY2 , V_TMP0ae(w)
		vmulw.zw	V_TMP0b , VF_PCW2 , V_TMP0be(w)
		vmulw.zw	V_TMP1a , VF_PCY2 , V_TMP1ae(w)
		vmulw.zw	V_TMP1b , VF_PCW2 , V_TMP1be(w)
		vadd		VF_TR2a , VF_TR2a , V_TMP0a
		vadd		VF_TR2b , VF_TR2b , V_TMP0b
		vadd		VF_TR3a , VF_TR3a , V_TMP1a
		vadd		VF_TR3b , VF_TR3b , V_TMP1b
		PSX2_ASM_PREFETCH(0x40(p_xy10p))
		vadd		V_XY10a , V_XY00a , VF_TR2a
		vadd		V_XY10b , V_XY00b , VF_TR2b
		vadd		V_XY11a , V_TMP2a , VF_TR3a
		vadd		V_XY11b , V_TMP2b , VF_TR3b
		vsub		V_XY01a , V_TMP2a , VF_TR3a
		vsub		V_XY01b , V_TMP2b , VF_TR3b
		vsub		V_XY00a , V_XY00a , VF_TR2a
		vsub		V_XY00b , V_XY00b , VF_TR2b
		sqc2		V_XY10a , 0x000 (p_xy10)
		sqc2		V_XY10b , 0x000 (p_xy10p)
		sqc2		V_XY11a , 0x000 (p_xy11)
		sqc2		V_XY11b , 0x000 (p_xy11p)
		sqc2		V_XY01a , 0x000 (p_xy01)
		sqc2		V_XY01b , 0x000 (p_xy01p)
		sqc2		V_XY00a , 0x000 (p_xy00)
		sqc2		V_XY00b , 0x000 (p_xy00p)
		.set reorder
	");
#else
	WATERFFT_ComplexMulSubAdd_SQR(p_xy00 , p_xy10 , p_xy01 , p_xy11 , p_CosTableX , p_CosTableY );
	WATERFFT_ComplexMulSubAdd_SQR(p_xy00 + 1, p_xy10 + 1 , p_xy01 + 1, p_xy11 + 1 , p_CosTableXP1 , p_CosTableY );
	p_xy00 += Pitch;
	p_xy01 += Pitch;
	p_xy10 += Pitch;
	p_xy11 += Pitch;
	WATERFFT_ComplexMulSubAdd_SQR(p_xy00 , p_xy10 , p_xy01 , p_xy11 , p_CosTableX , p_CosTableYP1 );
	WATERFFT_ComplexMulSubAdd_SQR(p_xy00 + 1, p_xy10 + 1 , p_xy01 + 1, p_xy11 + 1 , p_CosTableXP1 , p_CosTableYP1 );
#endif	
}
/* 
---------------------------------------------------------------------------------------------------------------
-------------------------------------------- 2D FFT REAL-TIME -------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void WATERFFT_FFT2D(FFT_Complex *p_xy,int m,FFT_ComplexCOS *FFT_COSTABLE)
{
	FFT_ComplexCOS *p_CosTable;
	unsigned int SPo2;
	int Mpo2Counter;
	
	/******************************** TRANSFORM *********************************/
	p_CosTable = FFT_COSTABLE;
	SPo2 = 1;
	{
		unsigned int XCounter,YCounter,MASK,MASK_C,PITCH,TileCounter;
		TileCounter = (1 << m)>>1;
		MASK = ~(SPo2 - 1);
		MASK_C = SPo2 - 1;
		PITCH = (SPo2 << m);
		for (YCounter = 0 ; YCounter < TileCounter ; YCounter++ )
		{
			FFT_Complex *p_Y;
			FFT_ComplexCOS *p_CosTableY;
			p_CosTableY = p_CosTable + (YCounter & MASK_C);
			p_Y = p_xy + ((YCounter + (YCounter & MASK)) << m);
			for (XCounter = 0 ; XCounter < TileCounter ; XCounter++ )
			{
				FFT_Complex *p_X,*p_Xnl;
				p_X = p_Y + XCounter + (XCounter & MASK);
				p_Xnl = p_X + PITCH;
				WATERFFT_ComplexMulSubAdd_SQR(p_X , p_Xnl , p_X + SPo2 , p_Xnl + SPo2 , p_CosTable + (XCounter & MASK_C)  , p_CosTableY );
#ifdef PSX2_TARGET			
				asm __volatile__ ("PSX2_ASM_PREFETCH(0x80(p_Xnl))");
#endif			
			}
		}
		p_CosTable += SPo2;
		SPo2 <<= 1;
	}
	for (Mpo2Counter = 1 ; Mpo2Counter < m ; Mpo2Counter ++)
	{
		unsigned int XCounter,YCounter,MASK,MASK_C,PITCH,TileCounter,LinePitch;
		TileCounter = (1 << m)>>1;
		MASK = ~(SPo2 - 1);
		MASK_C = SPo2 - 1;
		PITCH = (SPo2 << m);
		LinePitch = 1 << m;
#ifdef PSX2_TARGET
		LinePitch *= sizeof(FFT_Complex);
#endif			
		for (YCounter = 0 ; YCounter < TileCounter ; YCounter+=2 )
		{
			FFT_Complex *p_Y;
			FFT_ComplexCOS *p_CosTableY,*p_CosTableYP1;
			p_CosTableY = p_CosTable + (YCounter & MASK_C);
			p_CosTableYP1 = p_CosTable + ((YCounter + 1) & MASK_C);
			p_Y = p_xy + ((YCounter + (YCounter & MASK)) << m);
#ifdef PSX2_TARGET
		
			WATERFFT_PreloadCosTableY(p_CosTableY,p_CosTableYP1);
#endif			
			for (XCounter = 0 ; XCounter < TileCounter ; XCounter+=2 )
			{
				FFT_Complex *p_X,*p_Xnl;
				p_X = p_Y + XCounter + (XCounter & MASK);
				p_Xnl = p_X + PITCH;
				WATERFFT_ComplexMulSubAdd_SQR_DoubleXY(p_X , p_Xnl , p_X + SPo2 , p_Xnl + SPo2 , p_CosTable + (XCounter & MASK_C) , p_CosTable + ((XCounter + 1) & MASK_C) , p_CosTableY , p_CosTableYP1 , LinePitch);
			}
		}
		p_CosTable += SPo2;
		SPo2 <<= 1;
	}
	/******************************** Last correction *********************************/
	WATERFFT_CorrectHeight(p_xy , m );
	/************************************* Finish *************************************/
}

/* 
---------------------------------------------------------------------------------------------------------------
-------------------------------------------- OCEAN -----------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
#define TWO_PI			6.283185307179586476925286766559f
#define INV_SQRT_TWO	0.70710678118654752440084436210485f
#define GRAVITY			9.81f
#define square(X) ((X)*(X))
float COS_TABLE[2048] ONLY_PSX2_ALIGNED(32);
#define MAT_C_f_0Bits   ((32768.0 + 16384.0) * 256.0 * 65536.0 * 4096.0) 
_inline_ float FAST_cos(unsigned int Alpha) {return COS_TABLE[(Alpha & 1023) << 1];}
_inline_ float FAST_sin(unsigned int Alpha) {return COS_TABLE[((Alpha & 1023) << 1) + 1];};
_inline_ float WATERFFT_frand(float min, float max) {return min+((float)rand()/(float)RAND_MAX)*(max-min);};

/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
/* This generate the ocean spectre from a horizontal vector */
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
/* This generate the ocean spectre from a horizontal vector */
float WATERFFT_philippe(Vector2 *k, Vector2 *wind , float Pow_factor , float Wind_factor)
{
    float k2 ,v2,ret;
	float WC;
	k2 = (float)(k->X * k->X + k->Y * k->Y);
	k2 = (float)sqrt(k2);
    if (k2 == 0.0f) return 0.0f;
    v2 = wind->X * wind->X + wind->Y * wind->Y;
	v2 = (float)sqrt(v2);

	WC = 0.5f + 0.5f * (k->X*wind->X + k->Y*wind->Y) / (k2*v2);
	WC = (float)pow(WC , Wind_factor);

    ret = (float)(WC / pow(k2 , Pow_factor));
    return ret;
}

/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
float WATERFFT_gaussian()
{
    static float y1, y2;
    static unsigned char calculate = 1;
    float x1, x2, w;
    if (calculate)
    {
        calculate = 0;
        do
        {
            x1 = 2.0f*WATERFFT_frand(0, 1) - 1.0f;
            x2 = 2.0f*WATERFFT_frand(0, 1) - 1.0f;
            w = x1*x1 + x2*x2;
        } while (w >= 1.0f);

        w = (float)sqrt((-2.0f*log(w))/w);
        y1 = x1*w;
        y2 = x2*w;
        return y1;
    }
    else
    {
        calculate = 1;
        return y2;
    }
}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void WATERFFT_SwapXY(FFT_Complex *c,int m,int i, int j)
{
	int Counter;FFT_Complex *pi,*pj,SWAP;
	/* 1 Swap Lines   */
	pi = c + (i<<m);pj = c + (j<<m);Counter = 1 << m;while (Counter--){SWAP = *pi;*pi = *pj;*pj = SWAP;pj++;pi++;}
	/* 2 Swap columns */
	pi = c + i;pj = c + j;Counter = 1 << m;while (Counter--){SWAP = *pi;*pi = *pj;*pj = SWAP;pj+=1<<m;pi+=1<<m;}
}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void WATERFFT_Pretranspose_Table(FFT_Complex *c,int m)
{
	int nn,i,j,k,i2;
	/* Calculate the number of points */
	nn = 1 << m;
	/* Do the bit reversal */
	i2 = nn >> 1;
	j = 0;
	for (i=0;i<nn-1;i++) 
	{
		if (i < j) WATERFFT_SwapXY(c,m,i,j);
		k = i2;
		while (k <= j) {j -= k;k >>= 1;}
		j += k;
	}
}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void WATERFFT_SWAP(int *Table , int i , int j)
{
	int SWAP;
	SWAP = Table[i] ;
	Table[i] = Table[j];
	Table[j] = SWAP;
}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void WATERFFT_Compute_Supershift(tdst_FFT *p_FFTSRC)
{
	int nn,i,j,k,i2;
	int TABLE_SRC[1 << SPECTRE_SIZE_MAX];
	int TABLE_DST[1 << SPECTRE_SIZE_MAX];
	p_FFTSRC->lSuperShift0 = 0;
	p_FFTSRC->lSuperShift1 = 1;
	p_FFTSRC->lSuperShift2 = 1;
	if (p_FFTSRC->DXDYBorderSizePo2 == p_FFTSRC->BorderSizePo2) return;
	if (p_FFTSRC->DXDYBorderSizePo2 > p_FFTSRC->BorderSizePo2) return; // BUG!!
	nn = 1 << p_FFTSRC->BorderSizePo2;
	for (i=0;i<nn-1;i++) 
	{
		TABLE_SRC[i] = i - ((1 << p_FFTSRC->BorderSizePo2) >> 1) + ((1 << p_FFTSRC->DXDYBorderSizePo2) >> 1);
		TABLE_DST[i] = i;
	};
	/* Do the bit reversal SRC */
	nn = 1 << p_FFTSRC->BorderSizePo2;
	i2 = nn >> 1;
	j = 0;
	for (i=0;i<nn-1;i++) 
	{
		if (i < j) WATERFFT_SWAP(TABLE_SRC, i , j);
		k = i2;
		while (k <= j) {j -= k;k >>= 1;}
		j += k;
	}
	/* Do the bit reversal DST */
	nn = 1 << p_FFTSRC->DXDYBorderSizePo2;
	i2 = nn >> 1;
	j = 0;
	for (i=0;i<nn-1;i++) 
	{
		if (i < j) WATERFFT_SWAP(TABLE_DST, i , j);
		k = i2;
		while (k <= j) {j -= k;k >>= 1;}
		j += k;
	}
	nn = 1 << p_FFTSRC->BorderSizePo2;
	for (i=0;i<nn-1;i++) 
	{
		if (TABLE_SRC[i] == TABLE_DST[0])
			p_FFTSRC->lSuperShift0 = i;
		if (TABLE_SRC[i] == TABLE_DST[1])
			p_FFTSRC->lSuperShift1 = i;
		if (TABLE_SRC[i] == TABLE_DST[2])
			p_FFTSRC->lSuperShift2 = i;
	}
	p_FFTSRC->lSuperShift2 -= p_FFTSRC->lSuperShift1;
	p_FFTSRC->lSuperShift1 -= p_FFTSRC->lSuperShift0;
}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
unsigned int WATERFFT_Compress_Spectromap(FFT_Complex *p_Src , unsigned int m)
{
	WATERFFT_Pretranspose_Table(p_Src,m);
	return (1 << (m << 1));
}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void WATERFFT_Precalc_DXDY_Repartition(tdst_FFT *p_FFTSRC  )
{
	int	X,Y,BS;
	if (!p_FFTSRC->DXDYBorderSizePo2) return;
	if (!p_FFTSRC->pDXDYMap) p_FFTSRC->pDXDYMap = (FFT_Complex*)WATERFFT_Alloc(sizeof(FFT_Complex) << (p_FFTSRC->DXDYBorderSizePo2 << 1));
	BS = 1 << (p_FFTSRC->DXDYBorderSizePo2);
	for (Y = 0; Y < BS ; Y++)
	{
		for ( X = 0 ; X < BS ; X++)
		{
			Vector2			k;
			float KL;
			k.X = (float)(X - (BS >> 1));
			k.Y = (float)(Y - (BS >> 1));
			KL = k.X*k.X + k.Y*k.Y;
			if (KL != 0.0f)
			{
				KL = 1.0f / (float)sqrt(KL);
#ifdef FFT_TEST   
				KL = 0.0f; 
#endif				
				p_FFTSRC->pDXDYMap[(Y << p_FFTSRC->DXDYBorderSizePo2)+X].re = k.X * KL;
				p_FFTSRC->pDXDYMap[(Y << p_FFTSRC->DXDYBorderSizePo2)+X].im = k.Y * KL;
			} else
			{
				p_FFTSRC->pDXDYMap[(Y << p_FFTSRC->DXDYBorderSizePo2)+X].re = 0.0f;
				p_FFTSRC->pDXDYMap[(Y << p_FFTSRC->DXDYBorderSizePo2)+X].im = 0.0f;
			}
		}
	}
	WATERFFT_Pretranspose_Table(p_FFTSRC->pDXDYMap,p_FFTSRC->DXDYBorderSizePo2);
	WATERFFT_Compute_Supershift(p_FFTSRC);
}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void WATERFFT_Destroy_Spectre(tdst_FFT *p_FFT)
{
	if (p_FFT->pSpectroMap) WATERFFT_Free(p_FFT->pSpectroMap);
	if (p_FFT->pDXDYMap) WATERFFT_Free(p_FFT->pDXDYMap);
	WATERFFT_Memset0(p_FFT , sizeof(tdst_FFT));
}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void WATERFFT_Normalize(FFT_Complex *p_Src , unsigned int Size)
{
	double Summ;
	unsigned long Counter;
	Summ = 0.0;
	for (Counter = 0 ; Counter < Size ; Counter ++)
		Summ += p_Src[Counter].re;
	Summ = 1.0f / Summ;
	for (Counter = 0 ; Counter < Size ; Counter ++)
		p_Src[Counter].re *= (float)Summ ;
}

/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
_inline_ void WATERFFT_ComplexMul(FFT_Complex *A,FFT_Complex *B,FFT_Complex *R)
{
	float z;
	z		= A->re * B->re - A->im * B->im;
	R->im	= A->re * B->im + A->im * B->re;
	R->re	= z;
}
void WATERFFT_FFT_COMPUTE_COST(int m,FFT_ComplexCOS *CosTable)
{
	int nn,l,l1,l2;
	FFT_Complex U,C;
	nn = 1 << m;
	l2 = 1;
	for (l=0;l<m;l++) 
	{
		l1 = l2;l2 <<= 1;U.re = 1.0f;U.im = 0.0f;
		C = FFT_FACTORS_C[l]; // c.im = (float)sqrt((1.0 - c.re) / 2.0); // c.re = (float)sqrt((1.0 + c.re) / 2.0);
		while (l1--) 
		{
#ifdef PSX2_TARGET
			CosTable->re1 = U.re;
			CosTable->im1 = U.im;
			CosTable->re2 = U.re;
			CosTable->im2 = U.im;
			CosTable->re3 = -U.im;
			CosTable->im3 = U.re;
			CosTable->re4 = -U.im;
			CosTable->im4 = U.re;
#else
			CosTable->re1 = U.re;
			CosTable->im1 = U.im;
			CosTable->re2 = -U.im;
			CosTable->im2 = U.re;
#endif			
			CosTable++;
			WATERFFT_ComplexMul(&U,&C,&U);
		}
	}
}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN PRECALC  ------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void WATERFFT_Init_Spectre(tdst_FFT *p_FFT)
{    
	int x,y;
	FFT_Complex *pLinePtr;
	if (!p_FFT->pSpectroMap)
		p_FFT->pSpectroMap = (FFT_Complex*)WATERFFT_Alloc(sizeof(FFT_Complex) << (p_FFT->BorderSizePo2 << 1));
		
	/* Philips Spectrum */
    for (x = 0; x < (1 << p_FFT->BorderSizePo2) ; x++)
    {
		pLinePtr = &p_FFT->pSpectroMap[x << p_FFT->BorderSizePo2];
	    for (y = 0; y < (1 << p_FFT->BorderSizePo2) ; y++)
        {
			Vector2 ITW;
			ITW.X = (((float)x) - ((float)(1 << p_FFT->BorderSizePo2))/2.0f)*(TWO_PI/p_FFT->WorldSizeInMeters);
			ITW.Y = (((float)y) - ((float)(1 << p_FFT->BorderSizePo2))/2.0f)*(TWO_PI/p_FFT->WorldSizeInMeters);
			pLinePtr->re = /*WATERFFT_gaussian() * */(float)WATERFFT_philippe(&ITW, &p_FFT->Wind , p_FFT->P_Factor , p_FFT->WindBalance);
            pLinePtr->im = /*WATERFFT_gaussian() * */(float)WATERFFT_philippe(&ITW, &p_FFT->Wind , p_FFT->P_Factor , p_FFT->WindBalance);
			pLinePtr->re = pLinePtr->re * pLinePtr->re + pLinePtr->im * pLinePtr->im ;
			pLinePtr->re = (float)sqrt(pLinePtr->re);
			pLinePtr++;
        }
    }
	if (p_FFT->BorderSizePo2 > 6) // Reset big waves.
	{
		long WaterWith , y ,x;
		int CounterX,CounterY;
		WaterWith = 1 << p_FFT->BorderSizePo2;
		y = WaterWith >> 1;
		x = WaterWith >> 1;
		for (CounterX = 0 ; CounterX < (int)(p_FFT->BorderSizePo2 - 6); CounterX ++)
			for (CounterY = 0 ; CounterY < (int)(p_FFT->BorderSizePo2 - 6); CounterY ++)
			{
				p_FFT->pSpectroMap[((y + CounterY) << p_FFT->BorderSizePo2) + (x + CounterX)] .re = 0.0f;
				p_FFT->pSpectroMap[((y + CounterY) << p_FFT->BorderSizePo2) + (x - CounterX)] .re = 0.0f;
				p_FFT->pSpectroMap[((y - CounterY) << p_FFT->BorderSizePo2) + (x + CounterX)] .re = 0.0f;
				p_FFT->pSpectroMap[((y - CounterY) << p_FFT->BorderSizePo2) + (x - CounterX)] .re = 0.0f;
			}
	} 
#ifdef FFT_TEST    
	if (1) // TEsT. Two waves.
	{
		long WaterWith , y ,x;
		WaterWith = 1 << p_FFT->BorderSizePo2;
		WATERFFT_Memset0(p_FFT->pSpectroMap , sizeof(FFT_Complex) << (p_FFT->BorderSizePo2 << 1));
		y = WaterWith >> 1;
		x = WaterWith >> 1;
		p_FFT->pSpectroMap[((y + 0) << p_FFT->BorderSizePo2) + (x + 1)] .re = 1.0f;
	//	p_FFT->pSpectroMap[((y - 6) << p_FFT->BorderSizePo2) + (x + 0)] .re = 1.0f;
	} 
#endif	
	WATERFFT_Normalize(p_FFT->pSpectroMap , 1 << (p_FFT->BorderSizePo2 << 1));
	{
		float kl;
		float wkt; // x(k)*t
		unsigned int x,y , ulWATERWIDTH , yLine;
		Vector2 k;
		ulWATERWIDTH = 1 << p_FFT->BorderSizePo2;
		for (y = 0 ; y < ulWATERWIDTH ; y++)
		{
			yLine = y*ulWATERWIDTH;
			for (x = 0; x < ulWATERWIDTH; x++)
			{
				k.X = (((float)x) - ((float)(ulWATERWIDTH))/2.0f)*(TWO_PI/p_FFT->WorldSizeInMeters);
				k.Y = (((float)y) - ((float)(ulWATERWIDTH))/2.0f)*(TWO_PI/p_FFT->WorldSizeInMeters);
				kl = (k.X*k.X + k.Y*k.Y);
				kl = (float)sqrt(kl);
				wkt = (float)sqrt(kl*GRAVITY);
				/* +- 2.5 % of error */
				wkt += WATERFFT_frand(-wkt , wkt) / 80.0f;
				p_FFT->pSpectroMap[yLine+x] .im = wkt;
			}
		}
	}
	for (y = 0 ; y < 1024 ; y++)
	{
		COS_TABLE[y << 1] = (float)cos (((float)y) * TWO_PI / 1024.0f);
		COS_TABLE[(y << 1) + 1] = (float)sin (((float)y) * TWO_PI / 1024.0f);
	}

	p_FFT->ulSpectromapCompressedSize = WATERFFT_Compress_Spectromap(p_FFT->pSpectroMap,p_FFT->BorderSizePo2);

	WATERFFT_Precalc_DXDY_Repartition(p_FFT);

	WATERFFT_FFT_COMPUTE_COST(p_FFT->BorderSizePo2,p_FFT->FFT_COSTABLE);
	WATERFFT_FFT_COMPUTE_COST(p_FFT->DXDYBorderSizePo2,p_FFT->FFT_COSTABLEDXDY);
}

/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN REAL-TIME -----------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
_inline_ void WATERFFT_Deduct_DXDY_Spectre_Unit(FFT_Complex *pCx , register FFT_Complex *pCy , FFT_Complex *pDXDY_REP,FFT_Complex *pSRCLocal,FFT_Complex *pSRCLocal2)
{
#ifdef PSX2_TARGET
	register ULONG TMP0 , TMP1 , TMP2 , TMP3;
	asm __volatile__ ("
	.set noreorder
		ld			TMP0 , 0x00(pSRCLocal)
		ld			TMP1 , 0x00(pDXDY_REP)
		ld			TMP2 , 0x00(pSRCLocal2)
		ld			TMP3 , 0x08(pDXDY_REP)
		prot3w		TMP0 , TMP0 
		prot3w		TMP2 , TMP2
		PSX2_ASM_PREFETCH(0x40(pDXDY_REP))
		pexcw		TMP0 , TMP0 
		pexcw		TMP2 , TMP2
		pcpyld		TMP0 , TMP2 , TMP0
		pcpyld		TMP1 , TMP3 , TMP1
		qmtc2		TMP0 , $vf10
		qmtc2		TMP1 , $vf11
		vmulx.xy 	$vf20 , $vf10 , $vf11
		vmuly.xy 	$vf21 , $vf10 , $vf11
		vmulz.zw 	$vf20 , $vf10 , $vf11
		vmulw.zw 	$vf21 , $vf10 , $vf11
		vsub.xz		$vf20 , $vf00 , $vf20	
		vsub.xz		$vf21 , $vf00 , $vf21	
		sqc2		$vf20 , 0(pCx)
		sqc2		$vf21 , 0(pCy)
		.set reorder
	");
#else
	pCx->re = -pDXDY_REP->re * pSRCLocal->im;
	pCx->im = pDXDY_REP->re * pSRCLocal->re;
	(pCx + 1)->re = -(pDXDY_REP + 1)->re * pSRCLocal2->im;
	(pCx + 1)->im = (pDXDY_REP + 1)->re * pSRCLocal2->re;
	
	pCy->re = -pDXDY_REP->im * pSRCLocal->im;
	pCy->im = pDXDY_REP->im * pSRCLocal->re;
	(pCy + 1)->re = -(pDXDY_REP + 1)->im * pSRCLocal2->im;
	(pCy + 1)->im = (pDXDY_REP + 1)->im * pSRCLocal2->re;
	
#endif
}
void WATERFFT_Deduct_DXDY_Spectre_Line(FFT_Complex *pCx , FFT_Complex *pCy, FFT_Complex *pCxLast ,FFT_Complex *pDXDY_REP,FFT_Complex *pSRCLocal, int lSuperShift1 , int lSuperShift2)
{
	LONG lSuperShift;
	FFT_Complex *pSRCLocal2;
	pSRCLocal2 = pSRCLocal + lSuperShift1;
	lSuperShift = lSuperShift1 + lSuperShift2;
	while (pCx < pCxLast)
	{
		WATERFFT_Deduct_DXDY_Spectre_Unit(pCx , pCy , pDXDY_REP,pSRCLocal,pSRCLocal2);
		pCx += 2;
		pCy += 2;
		pDXDY_REP+= 2;
		pSRCLocal += lSuperShift;
		pSRCLocal2 += lSuperShift;
	}
}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN REAL-TIME -----------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
void WATERFFT_Deduct_DXDY_Spectre(tdst_FFT *p_FFTSRC , FFT_Complex *pSRC , FFT_Complex *pDX, FFT_Complex *pDY )
{
    FFT_Complex *pCx , *pCy , *pDXDY_REP , *pCxLast , *pSRCLocal;
	unsigned int ulYCntr , ulYCntrLast , XOR;

	if (!p_FFTSRC->DXDYBorderSizePo2) return;

	ulYCntr = 0;
	ulYCntrLast = ulYCntr + (1 << (p_FFTSRC->DXDYBorderSizePo2 << 1));
	pCx = pDX;
	pCy = pDY;
	pDXDY_REP = p_FFTSRC->pDXDYMap;
	pSRC += p_FFTSRC->lSuperShift0;
	pSRC += (p_FFTSRC->lSuperShift0) << p_FFTSRC->BorderSizePo2;
	XOR = 0;
	while (ulYCntr < ulYCntrLast)
	{
		pCxLast = pCx + (1 << p_FFTSRC->DXDYBorderSizePo2);
		pSRCLocal = pSRC ;
		WATERFFT_Deduct_DXDY_Spectre_Line(pCx , pCy, pCxLast , pDXDY_REP, pSRCLocal, p_FFTSRC->lSuperShift1 , p_FFTSRC->lSuperShift2);
		pCx += 1 << p_FFTSRC->DXDYBorderSizePo2;
		pCy += 1 << p_FFTSRC->DXDYBorderSizePo2;
		pDXDY_REP += 1 << p_FFTSRC->DXDYBorderSizePo2;
		if (!XOR)
			pSRC += (p_FFTSRC->lSuperShift1) << p_FFTSRC->BorderSizePo2;
		else
			pSRC += (p_FFTSRC->lSuperShift2) << p_FFTSRC->BorderSizePo2;
		XOR ^= 1;
		ulYCntr += 1 << p_FFTSRC->DXDYBorderSizePo2;
	}
	/* Correct bug of size difference btween Spacemap & DXDY Map */
	if ((p_FFTSRC->BorderSizePo2 - p_FFTSRC->DXDYBorderSizePo2) & 1)
	{
		WATERFFT_CorrectHeight(pDX , p_FFTSRC->DXDYBorderSizePo2);
		WATERFFT_CorrectHeight(pDY , p_FFTSRC->DXDYBorderSizePo2);
	}

}
/* 
---------------------------------------------------------------------------------------------------------------
----------------------------------- OCEAN REAL-TIME -----------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
*/
#ifdef PSX2_TARGET // PSX Optimize tools
_inline_ void WATERFFT_PS2MUL_1(FFT_Complex *p_DstLocal , FFT_Complex *p_SRC , float *CosSinA , float *CosSinB, float *CosSinC , float *CosSinD)
{
#if 0
		(p_DstLocal + 0)->re = (p_SRC + 0)->re * *CosSinA;
		(p_DstLocal + 0)->im = (p_SRC + 0)->re * *(CosSinA + 1);
		(p_DstLocal + 1)->re = (p_SRC + 1)->re * *(CosSinB);
		(p_DstLocal + 1)->im = (p_SRC + 1)->re * *(CosSinB + 1);
		(p_DstLocal + 2)->re = (p_SRC + 1)->re * *(CosSinC);
		(p_DstLocal + 2)->im = (p_SRC + 1)->re * *(CosSinC + 1);
		(p_DstLocal + 3)->re = (p_SRC + 1)->re * *(CosSinD);
		(p_DstLocal + 3)->im = (p_SRC + 1)->re * *(CosSinD + 1);
#else
	register int TMP0,TMP1,TMP2;
	register int TMP0b,TMP1b,TMP2b;
	asm __volatile__ ("
.set noreorder
		ld 	TMP0 , 0(CosSinA)
		ld 	TMP1 , 0(CosSinB)
		lq	TMP2 , 0x00(p_SRC)			// IM2 RE2 IM1 RE1
		ld 	TMP0b , 0(CosSinC)
		ld 	TMP1b , 0(CosSinD)
		lq	TMP2b , 0x10(p_SRC)			// IM2 RE2 IM1 RE1

		pcpyld	TMP0 , TMP1 , TMP0 // SINB COSB SINA COSA
		ppacw	TMP2 , TMP2 , TMP2 	// RE2 RE1 RE2 RE1
		pextuw	TMP2 , TMP2 , TMP2 	// RE2 RE2 RE1 RE1
		pcpyld	TMP0b , TMP1b , TMP0b // SINB COSB SINA COSA
		ppacw	TMP2b , TMP2b , TMP2b 	// RE2 RE1 RE2 RE1
		pextuw	TMP2b , TMP2b , TMP2b 	// RE2 RE2 RE1 RE1
		
		qmtc2	TMP0 , $vf11	// SINB COSB SINA COSA
		qmtc2	TMP2 , $vf12	// RE2 RE2 RE1 RE1
		qmtc2	TMP0b , $vf21	// SINB COSB SINA COSA
		qmtc2	TMP2b , $vf22	// RE2 RE2 RE1 RE1
		
		vmul	$vf10 , $vf11 , $vf12
		vmul	$vf20 , $vf21 , $vf22
		
		sqc2	$vf10 , 0x00 (p_DstLocal)
		sqc2	$vf20 , 0x10 (p_DstLocal)
		.set reorder
	");
#endif		
}
#endif

void WATERFFT_Turn_Spectre(tdst_FFT *p_FFTSRC , FFT_Complex *pDST , FFT_Complex *pDX, FFT_Complex *pDY , float timeinsecond )
{
    register FFT_Complex *p_SRC, *p_LST,*p_DstLocal;
#ifdef PSX2_TARGET
	float *Loc[8] ONLY_PSX2_ALIGNED(32);
	register ULONG *LocAddress;
	register ULONG TMP0 , TMP1 , TMP2 , TMP3 , MASK, COSTABLADD;
#else
	unsigned int Loc[2] ;
#endif
#if defined(_GAMECUBE) || defined(_XENON)
#define LowByteAcces 1
#else
#define LowByteAcces 0
#endif
	timeinsecond *= 1024.0f  / TWO_PI ;
	p_SRC = p_FFTSRC->pSpectroMap;
	p_LST = p_SRC + p_FFTSRC->ulSpectromapCompressedSize;
	p_DstLocal = pDST;
#ifdef PSX2_TARGET
	COSTABLADD = (ULONG)&COS_TABLE;
	timeinsecond *= 4.0f;
	LocAddress = Loc;
   	asm __volatile__("
   	.set noreorder
   		mtsah	$0 , 2
   		mfc1 TMP0 , timeinsecond
   		ppacw TMP0 , TMP0 , TMP0
   		ppacw TMP0 , TMP0 , TMP0
   		qmtc2 TMP0 , $vf30
   		vadd $vf30 , $vf30 , $vf30 // *2.0f
   		addiu	MASK , $0 , 2046
   		ppacw MASK , MASK , MASK
   		ppacw MASK , MASK , MASK
   		ppacw COSTABLADD , COSTABLADD , COSTABLADD
   		ppacw COSTABLADD , COSTABLADD , COSTABLADD
   		.set reorder
   	");
    while (p_SRC < p_LST)
    {
    	asm __volatile__("
    	.set noreorder
    		lq TMP0 , 0x00(p_SRC)
    		lq TMP1 , 0x10(p_SRC)
    		lq TMP2 , 0x20(p_SRC)
    		lq TMP3 , 0x30(p_SRC)
    		qfsrv TMP0 , TMP0 , TMP0
    		qfsrv TMP1 , TMP1 , TMP1
    		qfsrv TMP2 , TMP2 , TMP2
    		qfsrv TMP3 , TMP3 , TMP3
			PSX2_ASM_PREFETCH(0x40(p_SRC))    		
    		ppacw	TMP0 , TMP1 , TMP0
    		ppacw	TMP2 , TMP3 , TMP2
    		qmtc2	TMP0 , $vf10
    		qmtc2	TMP2 , $vf11
    		vmul	$vf10 , $vf10 , $vf30
    		vmul	$vf11 , $vf11 , $vf30
    		vftoi0	$vf10 , $vf10
    		vftoi0	$vf11 , $vf11
    		qmfc2	TMP0 , $vf10 
    		qmfc2	TMP2 , $vf11
    		pand 	TMP0 , TMP0 , MASK
    		pand 	TMP2 , TMP2 , MASK
    		psllw	TMP0 , TMP0 , 2
    		psllw	TMP2 , TMP2 , 2
    		paddw	TMP0 , TMP0 , COSTABLADD
    		paddw	TMP2 , TMP2 , COSTABLADD
    		sq		TMP0 , 0x00(LocAddress)
    		sq		TMP2 , 0x10(LocAddress)
    		.set reorder
    	");
		WATERFFT_PS2MUL_1(p_DstLocal , p_SRC , Loc[0] , Loc[1], Loc[2], Loc[3]);
		WATERFFT_PS2MUL_1(p_DstLocal + 4 , p_SRC + 4 , Loc[4] , Loc[5], Loc[6], Loc[7]);
		p_DstLocal+=8;
		p_SRC+=8;//*/
    }
#else		
    while (p_SRC < p_LST)
    {
		*(double *)Loc = p_SRC->im * timeinsecond + MAT_C_f_0Bits;
		p_DstLocal->re = p_SRC->re * FAST_cos(Loc[LowByteAcces]);
		p_DstLocal->im = p_SRC->re * FAST_sin(Loc[LowByteAcces]);
		p_DstLocal++;
		p_SRC++;
    }
#endif
    
    if ((pDX != NULL) && (pDY != NULL) && (p_FFTSRC->DXDYBorderSizePo2))
    {
    	WATERFFT_Deduct_DXDY_Spectre(p_FFTSRC , pDST , pDX, pDY );
	    /* then transform it */
	    
	    WATERFFT_FFT2D(pDX,p_FFTSRC->DXDYBorderSizePo2 , p_FFTSRC->FFT_COSTABLEDXDY);
	    WATERFFT_FFT2D(pDY,p_FFTSRC->DXDYBorderSizePo2 , p_FFTSRC->FFT_COSTABLEDXDY);
	    WATERFFT_FFT2D(pDST,p_FFTSRC->BorderSizePo2	   , p_FFTSRC->FFT_COSTABLE);
	    
    } else
    	WATERFFT_FFT2D(pDST,p_FFTSRC->BorderSizePo2 , p_FFTSRC->FFT_COSTABLE);
}


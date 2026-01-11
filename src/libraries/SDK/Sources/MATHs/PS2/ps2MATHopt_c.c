/*$T ps2MATHopt.c GC! 1.081 07/11/00 10:09:33 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_TARGET
#define __ps2MATHOPT_C__
#include <math.h>
#include "MATHs/MATHstruct.h"
#include "MATHs/MATHconst.h"
#include "MATHs/PS2/ps2MATHfloat.h"
#include "MATHs/PS2/ps2MATHopt.h"
#include "BASe/BASsys.h"
#include <libdma.h>

extern sceDmaTag ps2MATH_VU0_MicroCode_DmaTag	__attribute__((section(".vudata")));
extern sceDmaTag ps2MATH_VU0_MicroData_DmaTag	__attribute__((section(".vudata")));

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void ps2MATH_VU0SendPackage(sceDmaTag *pTag)
{
	/* FlushCache(WRITEBACK_DCACHE); */
	*D0_QWC = 0x00;
	*D0_TADR = (u_int) pTag;
	*D_STAT = 1;
	*D0_CHCR = 1 | (1 << 2) | (0 << 4) | (1 << 6) | (0 << 7) | (1 << 8);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2MATH_InitModule(void)
{
	
    /* VU0 init : data + code */
	ps2MATH_VU0SendPackage(&ps2MATH_VU0_MicroData_DmaTag);
	ps2MATH_VU0SendPackage(&ps2MATH_VU0_MicroCode_DmaTag);
}

#define VU0_MappedMemoryBase 0x11004000
#define DL_SinBase  64
#define DL_CosBase  DL_SinBase + 2
#define DL_TanBase  DL_CosBase + 2
#define DL_ACosBase DL_TanBase + 2
#define DL_ASinBase DL_ACosBase + 4
#define DL_ATanBase DL_ASinBase + 4
asm void MATH_StoreKInVU0(float *Coefs , ULONG Adress)
{
	.set noreorder
		lqc2	$vf10,0(a0)
		ctc2	a1,$vi1
		vsqi	$vf10 , ($vi1++)
	.set reorder
}
/*
			 X^2   X^4   X^6   X^8
COS(X) = 1 - ___ + ___ - ___ + ___ - .....  For -inf < X < +inf
              2!    4!    6!    8!
*/
_inline_ float fCosPs2Dl_Asm(float Alpha)
{
    register float fReturnVal;
    register int Tmp1 , Tmp2 , Tmp3 ; 
	asm __volatile__ ("
	.set noreorder
		addiu       %1,$0,DL_CosBase
		lui         %3,16256			// 1.0f
		mfc1 		%2, %4
		ctc2		%1,$vi01
		ppacw 		%2,%2,%2
		vlqi		$vf20 , ($vi01++) // Load k0 K1 K2 K3
		ppacw 		%2,%2,%2
		vlqi		$vf21 , ($vi01++) // Load k4 K5 K6 K7
		dsll32		%2,%2,0
		daddu		%2,%2,%3
		qmtc2 		%2, $vf10
		vmul		$vf12 , $vf10 , $vf10   // -> 1   , x^2  , x^2  , x^2
		vmul.zw		$vf12 , $vf12 , $vf12   // -> 1   , x^2  , x^4  , x^4
		vmuly.w		$vf12 , $vf12 , $vf12y  // -> 1   , x^2  , x^4  , x^6
		vmuly.xyzw	$vf14 , $vf12 , $vf12y   // -> x^2 , x^4  , x^6  , x^8
		vmulw.xyzw	$vf14 , $vf12 , $vf14w   // -> x^10, x^12 , x^14 , x^16
		vmul 		$vf12 , $vf12 , $vf20
		vmul 		$vf14 , $vf14 , $vf21
		vadd		$vf12 , $vf12 , $vf14
		vaddy.x		$vf12 , $vf12 , $vf12y
		vaddz.x		$vf12 , $vf12 , $vf12z
		vaddw.x		$vf12 , $vf12 , $vf12w
		qmfc2.i		%2, $vf12
		mtc1		%2 , %0
	.set reorder
	" : "=f" (fReturnVal) , "=r" (Tmp1) , "=r" (Tmp2) , "=r" (Tmp3): "f" (Alpha) );
    return fReturnVal;
}
/*
			 X^3   X^5   X^7   X^9
SIN(X) = X - ___ + ___ - ___ + ___ - .....  For -inf < X < +inf
              3!    5!    7!    9!
*/
asm float fSinPs2Dl_Asm(float Alpha)
{
	.set noreorder
		addiu       a1,$0,DL_SinBase
		mfc1 		t0, $f12
		ctc2		a1,$vi1
		ppacw 		t0,t0,t0
		vlqi		$vf20 , ($vi01++) // Load k0 K1 K2 K3
		ppacw 		t0,t0,t0
		vlqi		$vf21 , ($vi01++) // Load k4 K5 K6 K7
		qmtc2 		t0, $vf10
		qmtc2 		t0, $vf12
		vmul		$vf11 , $vf10 , $vf10   // -> x^2 , x^2  , x^2  , x^2
		vmul.yzw	$vf12 , $vf10 , $vf11   // -> x   , x^3  , x^3  , x^3
		vmul.zw		$vf12 , $vf12 , $vf11   // -> x   , x^3  , x^5  , x^5
		vmul.w		$vf12 , $vf12 , $vf11   // -> x   , x^3  , x^5  , x^7
		vmul		$vf13 , $vf10 , $vf12   // -> x^2 , x^4  , x^6  , x^8
		vmulw		$vf14 , $vf13 , $vf12w  // -> x^9 , x^11 , x^13 , x^15
		vmul 		$vf12 , $vf12 , $vf20
		vmul 		$vf14 , $vf14 , $vf21
		vadd		$vf12 , $vf12 , $vf14
		vaddy.x		$vf12 , $vf12 , $vf12y
		vaddz.x		$vf12 , $vf12 , $vf12z
		vaddw.x		$vf12 , $vf12 , $vf12w
		qmfc2.i		t0, $vf12
		jr          ra
		mtc1		t0 , $f0
	.set reorder
}

/* 0.1 for sinuss*/
#define MATH_TRIG_SUPERTOLERANCE 	 0.1f
//#define MATH_TRIG_SUPERTOLERANCE_TOP (0.998749217771f /* 0.05f */) //== sqrt(1.0 - sqr(MATH_TRIG_SUPERTOLERANCE))
#define MATH_TRIG_SUPERTOLERANCE_TOP 	 (Cf_PiBy2 - MATH_TRIG_SUPERTOLERANCE)
#define MATH_TRIG_SUPERTOLERANCE_BOTTOM  (MATH_TRIG_SUPERTOLERANCE)




#define MATH_ARCTRIG_SUPERTOLERANCE 	 0.05f // <- modifiez ce coeff s'il y a un pb de vibration :-(
#define MATH_ARCTRIG_SUPERTOLERANCE_TOP ((1.0f - MATH_ARCTRIG_SUPERTOLERANCE )*(1.0f - MATH_ARCTRIG_SUPERTOLERANCE ))
#define MATH_ARCTRIG_SUPERTOLERANCE_BOTTOM  (MATH_ARCTRIG_SUPERTOLERANCE * MATH_ARCTRIG_SUPERTOLERANCE)

#define Cf_OneOver2Pi 	0.15915494309189533576888376337251f
#define Cf_OneOverPi 	0.31830988618379067153776752674503f
#define Cf_OneOverPiBy2 0.63661977236758134307553505349006f


float fCosPs2Dl(float Alpha)
{
	register  float Sign , fPiBy2;
	register   int MaGic , Store1;
	Store1 = 0x3f800000;									/* == 1.0f */
	fPiBy2 = Cf_PiBy2;
	Alpha = fAbs(Alpha);									/* Cos ( ALpha ) = Cos ( - Alpha ) */
	MaGic = iRoundf(Cf_OneOverPiBy2 * Alpha);			/* The two last bits of magic will contain the quad num (0,1,2 or 3) */
	Alpha -= (float)MaGic * fPiBy2;						/* Here Alpha is restricted to 0 - pi/2 */
	Sign = ps2_i2f_Special((MaGic << 31) | Store1);			/* if the quad num are 1 or 3, we must compute Cf_PiBy2 - Alpha */
	Sign = fMin(0.0f , Sign);								/* if the quad num are 1 or 3 Sign = -1.0f , else Sign = 0.0f */
	Alpha = fAbs(Sign * fPiBy2 + Alpha);					/* if the quad num are 1 or 3 Alpha = Cf_PiBy2 - alpha , else Alpha = | -0.0f * Cf_PiBy2 - alpha | = abs ( Alpha ) */
	Sign = ps2_i2f_Special((((MaGic + 1) >> 1) << 31) | Store1);/* if the quad num are 1 or 2, sign must be == to -1 else 1 */
	return Sign * fCosPs2Dl_Asm(Alpha);
}
float fSinPs2Dl(float Alpha)
{	
	return fCosPs2Dl(Alpha - Cf_PiBy2);
}

float fTanPs2Dl(float Alpha)
{
	return tanf(Alpha);
}
/*
			 	X^3   3.X^5   3.5.X^7 
ArcSin(X) = X + ___ + _____ + _______ + .....  For |X| < 1
                2.3   2.4.5   2.4.6.7 
*/
asm float fASinPs2Dl_Asm(float Alpha)
{
	.set noreorder
		addiu       a1,$0,DL_ASinBase
		mfc1 		t0, $f12
		ctc2		a1,$vi1
		ppacw 		t0,t0,t0
		vlqi		$vf20 , ($vi01++) // Load k0 K1 K2 K3
		ppacw 		t0,t0,t0
		vlqi		$vf21 , ($vi01++) // Load k4 K5 K6 K7
		qmtc2 		t0, $vf10
		vlqi		$vf22 , ($vi01++) // Load k8 K9 Ka Kb
		qmtc2 		t0, $vf12
		vlqi		$vf23 , ($vi01++) // Load kc Kd Ke Kf
		vmul		$vf11 , $vf10 , $vf10   // -> x^2 , x^2  , x^2  , x^2
		vmul.yzw	$vf12 , $vf10 , $vf11   // -> x   , x^3  , x^3  , x^3
		vmul.zw		$vf12 , $vf12 , $vf11   // -> x   , x^3  , x^5  , x^5
		vmul.w		$vf12 , $vf12 , $vf11   // -> x   , x^3  , x^5  , x^7
		vmul		$vf13 , $vf10 , $vf12   // -> x^2 , x^4  , x^6  , x^8
		vmulw		$vf14 , $vf13 , $vf12w  // -> x^9 , x^11 , x^13 , x^15
		vmulw		$vf15 , $vf14 , $vf13w  // -> x^17 , x^19  , x^21  , x^23
		vmul 		$vf12 , $vf12 , $vf20
		vmulw		$vf16 , $vf15 , $vf13w  // -> x^25 , x^27  , x^29  , x^31
		vmul 		$vf14 , $vf14 , $vf21
		vmul 		$vf15 , $vf15 , $vf22
		vmul 		$vf16 , $vf16 , $vf23
		vadd		$vf12 , $vf12 , $vf14
		vadd		$vf15 , $vf15 , $vf16
		vadd		$vf12 , $vf15 , $vf12
		vaddy.x		$vf12 , $vf12 , $vf12y
		vaddz.x		$vf12 , $vf12 , $vf12z
		vaddw.x		$vf12 , $vf12 , $vf12w
		qmfc2.i		t0, $vf12
		jr 			ra
		mtc1		t0 , $f0
	.set reorder
}
/*
			PI 	     X^3   3.X^5   3.5.X^7 	         PI
ArcCos(X) = __ - X - ___ - _____ - _______ - ..... = __ - ArcSin(X)   For |X| < 1
             2       2.3   2.4.5   2.4.6.7            2
*/

float fASinPs2Dl(float Alpha)
{

	register float fSquareAlpha;
	fSquareAlpha = Alpha * Alpha;
	if (fSquareAlpha < MATH_ARCTRIG_SUPERTOLERANCE_BOTTOM) return Alpha;
	if (fSquareAlpha > MATH_ARCTRIG_SUPERTOLERANCE_TOP)
	{
		float Sign;
		Sign = fSign(Alpha);
		return Sign * (Cf_PiBy2 - fSqrt(1.0f - fSquareAlpha));
	}
	return fASinPs2Dl_Asm(Alpha);
}
float fACosPs2Dl(float Alpha)
{

	register float fSquareAlpha;
	register float fSigne;
//	return acosf(Alpha);
	fSquareAlpha = Alpha * Alpha;
	if (fSquareAlpha < MATH_ARCTRIG_SUPERTOLERANCE_BOTTOM) return Cf_PiBy2 - Alpha;
	fSigne = fSign(Alpha);
	if (fSquareAlpha > MATH_ARCTRIG_SUPERTOLERANCE_TOP)
	{
		float Sign;
		Sign = fMin(fSigne , 0.0f );
		return fAbs((Sign * Cf_Pi) + fSqrt(1.0f - fSquareAlpha));
	}
	return Cf_PiBy2 - fSigne * fASinPs2Dl_Asm(fAbs(Alpha));
}
/*
if (|X| < 1.0)

			      X^3   X^5   X^7 	      
ArcTanDL(X) = X - ___ + ___ - ___ + ..... 
                   3     5     7          
                 
                 PI               1  
else ArcTan(X) = --- - ArcTanDL( --- )
				  2               X  
*/	

float fATanPs2Dl(float Alpha)
{
	return atanf(Alpha);
}
float fATan2Ps2Dl(float A , float B)
{
	return atan2f(A,B);
}
static float AWEAWREAWER;
static float AWEAWREAWER2;

void PS2_MATH_InitTrigo2()
{
	float FactTable[32];
	ULONG Counter;
	u_long128 Coefs ONLY_PSX2_ALIGNED(16);
	float *pT4;
	pT4 = (float *)&Coefs;
	FactTable[0] = 0.0f;
	FactTable[1] = 1.0f;
	for (Counter = 2 ; Counter < 32 ; Counter ++)
		FactTable[Counter] = (float)Counter * FactTable[Counter - 1];
		
/*
			 X^3   X^5   X^7   X^9
SIN(X) = X - ___ + ___ - ___ + ___ - .....  For -inf < X < +inf
              3!    5!    7!    9!
*/
	pT4[0] = 1.0f;
	pT4[1] = - 1.0f / FactTable[3];
	pT4[2] = 1.0f / FactTable[5];
	pT4[3] = -1.0f / FactTable[7];
	MATH_StoreKInVU0(pT4 , DL_SinBase)	;
	pT4[0] = 1.0f / FactTable[9];
	pT4[1] = - 1.0f / FactTable[11];
	pT4[2] = 1.0f / FactTable[13];
	pT4[3] = -1.0f / FactTable[15];
	MATH_StoreKInVU0(pT4 , DL_SinBase + 1);

/*
			 X^2   X^4   X^6   X^8
COS(X) = 1 - ___ + ___ - ___ + ___ - .....  For -inf < X < +inf
              2!    4!    6!    8!
*/
	pT4[0] = 1.0f;
	pT4[1] = - 1.0f / FactTable[2];
	pT4[2] = 1.0f / FactTable[4];
	pT4[3] = -1.0f / FactTable[6];
	MATH_StoreKInVU0(pT4 , DL_CosBase)	;
	pT4[0] = 1.0f / FactTable[8];
	pT4[1] = - 1.0f / FactTable[10];
	pT4[2] = 1.0f / FactTable[12];
	pT4[3] = -1.0f / FactTable[14];
	MATH_StoreKInVU0(pT4 , DL_CosBase + 1);
/*
			 	X^3   3.X^5   3.5.X^7 
ArcSin(X) = X + ___ + _____ + _______ + .....  For |X| < 1
                2.3   2.4.5   2.4.6.7 
*/
	{
		float UpperAcc , LowerAcc , Zorg4;
		UpperAcc = 1.0f;
		LowerAcc = 1.0f;
		Zorg4 = 1.0f;
		pT4[0] = UpperAcc;
		LowerAcc *= (++Zorg4);
		pT4[1] = UpperAcc / (LowerAcc * (Zorg4 + 1.0f));
		UpperAcc *= (++Zorg4);
		LowerAcc *= (++Zorg4);
		pT4[2] = UpperAcc / (LowerAcc * (Zorg4 + 1.0f));
		UpperAcc *= (++Zorg4);
		LowerAcc *= (++Zorg4);
		pT4[3] = UpperAcc / (LowerAcc * (Zorg4 + 1.0f));
		UpperAcc *= (++Zorg4);
		LowerAcc *= (++Zorg4);
		MATH_StoreKInVU0(pT4 , DL_ASinBase)	;
		for ( Counter = 1 ; Counter < 4 ; Counter++)
		{
			pT4[0] = UpperAcc / (LowerAcc * (Zorg4 + 1.0f));
			UpperAcc *= (++Zorg4);
			LowerAcc *= (++Zorg4);
			pT4[1] = UpperAcc / (LowerAcc * (Zorg4 + 1.0f));
			UpperAcc *= (++Zorg4);
			LowerAcc *= (++Zorg4);
			pT4[2] = UpperAcc / (LowerAcc * (Zorg4 + 1.0f));
			UpperAcc *= (++Zorg4);
			LowerAcc *= (++Zorg4);
			pT4[3] = UpperAcc / (LowerAcc * (Zorg4 + 1.0f));
			UpperAcc *= (++Zorg4);
			LowerAcc *= (++Zorg4);
			MATH_StoreKInVU0(pT4 , DL_ASinBase + Counter);
		}
	}
	// TESTS
	AWEAWREAWER2 = fCosPs2Dl(3.1415927f * 2.0f + 3.1415927f);
	AWEAWREAWER2 = fCosPs2Dl(3.1415927f * 20.0f + 3.1415927f);
	AWEAWREAWER2 = fCosPs2Dl(3.1415927f * 100.0f + 3.1415927f * 0.5f);
	AWEAWREAWER2 = fCosPs2Dl(3.1415927f * 100.0f + 3.1415927f * 1.5f);
	AWEAWREAWER2 = fCosPs2Dl(3.1415927f / 2.0f);
	AWEAWREAWER2 = fCosPs2Dl(3.1415927f / 2.0f);
	AWEAWREAWER2 = fCosPs2Dl(3.1415927f / 2.0f);
	AWEAWREAWER2 = fCosPs2Dl(3.1415927f / 2.0f);
	Counter = 100;
	while (Counter--)
	{
		float Alpha;
		Alpha = (float)Counter * 0.3f - 10.0f;
		AWEAWREAWER = sinf(Alpha);
		AWEAWREAWER2 = fSinPs2Dl(Alpha);
	}//*/
}

#endif /* PSX2_TARGET */
 
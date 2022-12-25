#include "Precomp.h"

#include "LIGHT/LIGHTstruct.h"

#if (defined(_XBOX) || defined(_M_X86)) && !defined(_XENON)
#define USE_MMX_C	1
#ifdef USE_MMX_C

#include "xmmintrin.h"
    #define OPT_ADD_LOOP
#endif
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG LIGHT_ul_Interpol2Colors(ULONG ulP1, ULONG ulP2, float fZClipLocalCoef)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	RetValue, Interpoler;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(fZClipLocalCoef >= 0.98f) return ulP2;
	if(fZClipLocalCoef <= 0.02f) return ulP1;
	*((float *) &Interpoler) = fZClipLocalCoef + 32768.0f + 16384.0f;
	RetValue = (Interpoler & 128) ? (ulP2 & 0xfefefefe) >> 1 : (ulP1 & 0xfefefefe) >> 1;
	RetValue += (Interpoler & 64) ? (ulP2 & 0xfcfcfcfc) >> 2 : (ulP1 & 0xfcfcfcfc) >> 2;
	RetValue += (Interpoler & 32) ? (ulP2 & 0xf8f8f8f8) >> 3 : (ulP1 & 0xf8f8f8f8) >> 3;
	RetValue += (Interpoler & 16) ? (ulP2 & 0xf0f0f0f0) >> 4 : (ulP1 & 0xf0f0f0f0) >> 4;
	RetValue += (Interpoler & 8) ? (ulP2 & 0xe0e0e0e0) >> 5 : (ulP1 & 0xe0e0e0e0) >> 5;
	RetValue += (Interpoler & 4) ? (ulP2 & 0xc0c0c0c0) >> 6 : (ulP1 & 0xc0c0c0c0) >> 6;
	RetValue += (Interpoler & 2) ? (ulP2 & 0x80808080) >> 7 : (ulP1 & 0x80808080) >> 7;
	return RetValue;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef PSX2_TARGET
ULONG LIGHT_ul_MulColor(ULONG ulP1, float fZClipLocalCoef)
{
	ULONG REturnValue;
	asm  ("
		lw			a0, ulP1
		mfc1  		t0,fZClipLocalCoef
		qmtc2 		t0,$vf01
		PEXTLB		a0, zero, a0	/* Extend Color */
		PEXTLB		a0, zero, a0	/* Extend Color */
		qmtc2    	a0,	$vf02		
		VITOF0		$vf02,$vf02		/* VF02 = Color */
		VMULx.xyzw 	$vf02,$vf02,$vf01x
		VFTOI0		$vf02,$vf02		/* VF02 = Color */
		qmfc2.i    	v0,	$vf02		
		PPACB		v0,	zero, v0	/* Pack Color Result 			*/
		PPACB		v0,	zero, v0	/* Pack Color Result 			*/
		sw			v0,REturnValue
   ");//*/
	return REturnValue;
}
#elif defined(OPT_MULCOLOR)
_inline ULONG LIGHT_ul_MulColor_OPT(ULONG ulP1, float fZClipLocalCoef)
{
#define BIT_SHIFT	8
	unsigned short iInterp_16_8;
	unsigned int intZero = 0;
	ULONG REturnValue;
	const	int ShiftValue = BIT_SHIFT;
	const	float FloatToFixed_16_8 = (	_mm_empty(), 256.0f);
#ifdef _DEBUG
	static bool	bOriginalAlgo = false;
#endif
	__m64	iRGBA;
	__m64	iZero;
	__m64	iInterpolator_16_8;
	__m64	iRGBA_Fixed_16_0;
	__m64	iFinalRGBA_Fixed_16_16;
	__m64	iFinalRGBA;

#ifdef _DEBUG
/*	if (bOriginalAlgo)
	{
		ULONG	RetValue, Interpoler;

		if(fZClipLocalCoef >= 0.98f) return ulP1;
		if(fZClipLocalCoef <= 0.02f) return 0;
		*((float *) &Interpoler) = fZClipLocalCoef + 32768.0f + 16384.0f;
		Interpoler = LIGHT_gpul_CurrentTable[Interpoler & 0xff];
		RetValue = 0;
		if(Interpoler & 128) RetValue += (ulP1 & 0xfefefefe) >> 1;
		if(Interpoler & 64) RetValue += (ulP1 & 0xfcfcfcfc) >> 2;
		if(Interpoler & 32) RetValue += (ulP1 & 0xf8f8f8f8) >> 3;
		if(Interpoler & 16) RetValue += (ulP1 & 0xf0f0f0f0) >> 4;
		if(Interpoler & 8) RetValue += (ulP1 & 0xe0e0e0e0) >> 5;
		return RetValue;
	}
	*/
#endif
	__asm
	{
		fld		fZClipLocalCoef   
		fmul	FloatToFixed_16_8
		fistp	iInterp_16_8
	}

	iRGBA = _mm_cvtsi32_si64(ulP1);
	iZero = _mm_setzero_si64();
	iInterpolator_16_8 = _mm_set1_pi16(iInterp_16_8);

	iRGBA_Fixed_16_0 = _mm_unpacklo_pi8 (iRGBA,iZero);

	iFinalRGBA_Fixed_16_16 = _mm_mullo_pi16 (iRGBA_Fixed_16_0, iInterpolator_16_8);

	iFinalRGBA_Fixed_16_16 = _mm_srli_pi16 (iFinalRGBA_Fixed_16_16, ShiftValue);
	iFinalRGBA = _mm_packs_pu16(iFinalRGBA_Fixed_16_16,iZero);

	REturnValue = _mm_cvtsi64_si32(iFinalRGBA);
	return REturnValue;
}

ULONG LIGHT_ul_MulColor(ULONG ulP1, float fZClipLocalCoef)
{
	ULONG RetValue = LIGHT_ul_MulColor_OPT(ulP1,fZClipLocalCoef);
	__asm
	{
		emms
	}
	return RetValue;
}


#else
ULONG LIGHT_ul_MulColor(ULONG ulP1, float fZClipLocalCoef)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	RetValue, Interpoler;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(fZClipLocalCoef >= 0.98f) return ulP1;
	if(fZClipLocalCoef <= 0.02f) return 0;
	*((float *) &Interpoler) = fZClipLocalCoef + 32768.0f + 16384.0f;
	Interpoler = LIGHT_gpul_CurrentTable[Interpoler & 0xff];
	RetValue = 0;
	if(Interpoler & 128) RetValue += (ulP1 & 0xfefefefe) >> 1;
	if(Interpoler & 64) RetValue += (ulP1 & 0xfcfcfcfc) >> 2;
	if(Interpoler & 32) RetValue += (ulP1 & 0xf8f8f8f8) >> 3;
	if(Interpoler & 16) RetValue += (ulP1 & 0xf0f0f0f0) >> 4;
	if(Interpoler & 8) RetValue += (ulP1 & 0xe0e0e0e0) >> 5;
	return RetValue;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef PSX2_TARGET
void LIGHT_AddColor(ULONG *const pul_Diffuse, ULONG ul_Diffuse)
{
	register ULONG MODIF;
	
	asm __volatile__("lw	%0, 0(%1)" : "=r"(MODIF) : "r"(pul_Diffuse));
	asm __volatile__("PADDUB	%0, %1, %2" : "=r"(MODIF) : "r"(ul_Diffuse), "r"(MODIF));
	asm __volatile__("sw %0, 0(%1)" :  : "r"(MODIF), "r"(pul_Diffuse) );
}//*/
#elif !defined(OPT_ADDCOLOR)
void LIGHT_AddColor(ULONG *pul_Diffuse, ULONG ul_Diffuse)
{
	ULONG	ul_Color,ulSatur;
	ul_Diffuse = (ul_Diffuse & 0xfefefefe) >> 1;
	ul_Color = ul_Diffuse + ((*pul_Diffuse & 0xfefefefe) >> 1);
	ulSatur = ul_Color & 0x80808080;
	ulSatur |= ulSatur >> 1;
	ulSatur |= ulSatur >> 2;
	ulSatur |= ulSatur >> 4;
	*pul_Diffuse = (ul_Color << 1) | ulSatur;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if !defined(OPT_SUBCOLOR)
void LIGHT_SubColor(ULONG *pul_Diffuse, ULONG ul_Diffuse)
{
	*pul_Diffuse ^= 0xffffffff;
	LIGHT_AddColor(pul_Diffuse, ul_Diffuse);
	*pul_Diffuse ^= 0xffffffff;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_ColorMulAdd(ULONG Mul,ULONG ul_Ambient,ULONG ul_MatAmbient,ULONG *Src,ULONG NUM,ULONG *Dst)
{
	ULONG	dar, dag, dab /* , daa */ ;
	ULONG	ar, ag, ab, ul_Color;
	ULONG 	*SrcLast;
	ar = ((ul_Ambient & 0xFF) * (ul_MatAmbient & 0xFF)) >> 8;
	ag = (((ul_Ambient >> 8) & 0xFF) * ((ul_MatAmbient >> 8) & 0xFF)) >> 8;
	ab = (((ul_Ambient >> 16) & 0xFF) * ((ul_MatAmbient >> 16) & 0xFF)) >> 8;
	dar = Mul & 0xFF;
	dag = (Mul >> 8) & 0xFF;
	dab = (Mul >> 16) & 0xFF;
	SrcLast = Src + NUM;
	for(; Src < SrcLast; Src++, Dst++)
	{
		ul_Color = (((*Src & 0xFF) * dar) >> 8) + ar;
		if(ul_Color & 0x100)
			*Dst = 0xFF;
		else
			*Dst = ul_Color;

		ul_Color = ((((*Src >> 8) & 0xFF) * dag) >> 8) + ag;
		if(ul_Color & 0x100)
			*Dst |= 0xFF00;
		else
			*Dst |= (ul_Color << 8);

		ul_Color = ((((*Src >> 16) & 0xFF) * dab) >> 8) + ab;
		if(ul_Color & 0x100)
			*Dst |= 0xFF0000;
		else
			*Dst |= (ul_Color << 16);

		*Dst |= (*Src & 0xFF000000);

		/*
		 * ul_Color = ( *pul_Diffuse >> 24 ) + aa; if(ul_Color & 0x100) Colors |=
		 * 0xFF000000; else Colors |= (ul_Color << 24);
		 */
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_ColorAdd(ULONG Add,ULONG *Src,ULONG NUM,ULONG *Dst)
{
#ifdef PSX2_TARGET
	asm __volatile__ ("
		.set noreorder;
		srl          a2,a2,4
		PEXTLW 	a0, a0, a0
		addiu        a2,a2,1
		b 		LCA_LOOPENTRY
		PCPYLD 	a0, a0, a0
LCA_LOOP:// TReat 16 colors 
		lq  t1,16(a1)
		lq  t2,32(a1)
		lq  t3,48(a1)
		addi a2,a2,-1
		addiu a1,a1,64
		PADDUB t0,a0,t0
		PADDUB t1,a0,t1
		PADDUB t2,a0,t2
		PADDUB t3,a0,t3
		sq	t0,0(a3)
		sq	t1,16(a3)
		sq	t2,32(a3)
		sq	t3,48(a3)
		addiu a3,a3,64
LCA_LOOPENTRY:		
		bne a2,$0,LCA_LOOP
		lq  t0,0(a1)
		.set reorder;
		
	   ");//*/
	   
#elif defined(OPT_ADD_LOOP)
//	ULONG	*pLastSrc;
	ULONG	DoubleAdd[2];
	__m64	*pSrc64 = (__m64*)Src;
	__m64	*pDst64 = (__m64*)Dst;
	__m64	*pLastSrc64;
//    for(; Src < pLastSrc; Src+=2, Dst+=2)
	__m64	*pAdd64;

//   	pLastSrc = Src + NUM;
	DoubleAdd[0]=Add;
	DoubleAdd[1]=Add;
	pAdd64 = (__m64*)&DoubleAdd[0];
	pLastSrc64 = pSrc64+(NUM-((NUM&1)/2));

/*	for(; pSrc64 < pLastSrc64; pSrc64++, pDst64++)
    {
//		*Dst = _mm_cvtsi64_si32(_mm_adds_pu8 (_mm_cvtsi32_si64(*Src) , _mm_cvtsi32_si64(Add)));	
		*pDst64 = _mm_adds_pu8 (*pSrc64 , *pAdd64);	
	}*/

	while(pSrc64!=pLastSrc64)
	{
		*(pDst64++) = _mm_adds_pu8 (*(pSrc64++) , *pAdd64);	
	}

	if(NUM&1)
	{
		int *pSrc = (int *)pSrc64;
		int *pDst = (int *)pDst64;
		*pDst = _mm_cvtsi64_si32(_mm_adds_pu8 (_mm_cvtsi32_si64((int)*pSrc) , _mm_cvtsi32_si64(Add)));	
	}

	_mm_empty();
#else
	ULONG	ul_Color,ulSatur,*pLastSrc;

   	pLastSrc = Src + NUM;
    Add = (Add & 0xfefefefe) >> 1;
    for(; Src < pLastSrc; Src++, Dst++)
    {
	    ul_Color = Add + ((*Src & 0xfefefefe) >> 1);
	    ulSatur = ul_Color & 0x80808080;
	    ulSatur |= ulSatur >> 1;
	    ulSatur |= ulSatur >> 2;
	    ulSatur |= ulSatur >> 4;
	    *Dst = (ul_Color << 1) | ulSatur;
    }
#endif
}

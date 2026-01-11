/*$T ps2MATHfloat.h GC! 1.081 07/07/00 15:16:14 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Definition of all the operations on the floats */
#if !defined(_ps2MATHFLOAT_H_) && defined(PSX2_TARGET)
#define _ps2MATHFLOAT_H_
#include "BASe/BAStypes.h"

#include "MATHs/MATHextern.h"/* For the rand */
#include "MATHs/MATHconst.h"
#include "BASe/BASsys.h"
#include "MATHs/MATHstruct.h"
#include "MATHs/MATHvars.h"
#include "MATHs/PS2/ps2MATHopt.h"

#if !defined(__CW__)
#include <math.h>
#endif
#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    conversions
 ***********************************************************************************************************************
 */
_inline_ float ps2_i2f_Special(int ff)
{
    register float ft;
    asm __volatile__ ("mtc1     %1, %0" : "=f" (ft): "r" (ff) );
    return ft;
}

#define fInterpretLongAsFloat(a)	(*((float *) &(a)))
#define lInterpretFloatAsLong(a)	(*((LONG *) &(a)))
#define fSetLongToFloat(a, b)		(*((LONG *) &(a)) = (b))
#define fSetLongToPFloat(a, b)		(*((LONG *) (a)) = (b))
#define lGetFloatMantisse(a)		(lInterpretFloatAsLong(a) & 0x007FFFFF)
#define lGetFloatExponent(a)		(lInterpretFloatAsLong(a) & 0x7F800000)
#define lGetFloatExponentAndSign(a) (lInterpretFloatAsLong(a) & 0xFF800000)
#define fLongToFloat(a)				((float) (a))

_inline_ int ps2_f2i(float ff)
{
    int tt;
    register int io;
    register float ft;
    
    tt = 0x3f000000 | (*(int*)&ff & 0x80000000);
    ff += *(float*)&tt;
    
    asm __volatile__ ("cvt.w.s  %0, %1" : "=f" (ft): "f" (ff) );
    asm __volatile__ ("mfc1     %0, %1" : "=r" (io): "f" (ft) );
    return io;
}

_inline_ int iRoundf(float ff)
{
    register int io;
    register float ft;
    
    asm __volatile__ ("cvt.w.s  %0, %1" : "=f" (ft): "f" (ff) );
    asm __volatile__ ("mfc1     %0, %1" : "=r" (io): "f" (ft) );
    return io;
}

#define lFloatToLong(a)     		ps2_f2i((float)(a))

/*$4
 ***********************************************************************************************************************
    comparisons
 ***********************************************************************************************************************
 */

#define fEq(a, b)				((a) == (b))
#define fEqWithEpsilon(a, b, e) fNulWithEpsilon((a) - (b), (e))
#define fDiff(a, b)				((a) != (b))
#define fSup(a, b)				((a) > (b))
#define fSupEq(a, b)			((a) >= (b))
#define fInf(a, b)				((a) < (b))
#define fInfEq(a, b)			((a) <= (b))

#define fNul(a)					((a) == Cf_Zero)
#define fNulWithEpsilon(a, e)	(fAbs(a) < (e))
#define fAlmostNul(a)			fNulWithEpsilon((a), Cf_Epsilon)
#define fInfZero(a)				((a) < Cf_Zero)
#define fEqZero					fNul
#define fInfEqZero(a)			((a) <= Cf_Zero)
#define fSupZero(a)				((a) > Cf_Zero)
#define fSupEqZero(a)			((a) >= Cf_Zero)

/*
 =======================================================================================================================
    Aim:    Comparisons between strictly POSITIVE floats

    Note:   These comparisons are faster than classical float comparisons £
            Same name as the classic float comparison functions, but with an I at the end
 =======================================================================================================================
 */
#define fEqI(a, b)		(lInterpretFloatAsLong(a) == lInterpretFloatAsLong(b))
#define fDiffI(a, b)	(lInterpretFloatAsLong(a) != lInterpretFloatAsLong(b))
#define fSupI(a, b)		(lInterpretFloatAsLong(a) > lInterpretFloatAsLong(b))
#define fSupEqI(a, b)	(lInterpretFloatAsLong(a) >= lInterpretFloatAsLong(b))
#define fInfI(a, b)		(lInterpretFloatAsLong(a) < lInterpretFloatAsLong(b))
#define fInfEqI(a, b)	(lInterpretFloatAsLong(a) <= lInterpretFloatAsLong(b))

/*
 * Comparaison entiere d'un floatant avec une constante floatante définie en
 * entier il faudra définir toutes les constantes floats en entier en plus de la
 * définition en floats!
 */

/*
 =======================================================================================================================
    Comparaisons entiere de deux floats. Dans les cas suivant, le LONG peut être positif ou negatif
 =======================================================================================================================
 */
#define fEqLong(a, l)	(lInterpretFloatAsLong(a) == (l))

/*$F In the next two cases, the reel value must be positive*/

#define fSupPositivLong(a, l)	(lInterpretFloatAsLong(a) > (l))
#define fSupEqPositivLong(a, l) (lInterpretFloatAsLong(a) >= (l))

/*$F In the next two cases, the reel value must be negative */

#define fInfNegativLong(a, l)	(lInterpretFloatAsLong(a) > (l))
#define fInfEqNegativLong(a, l) (lInterpretFloatAsLong(a) >= (l))

/*$4
 ***********************************************************************************************************************
    trigo functions (voir Vu pour optimisation)
 ***********************************************************************************************************************
 */

float fCosPs2Dl(float Alpha);
float fSinPs2Dl(float Alpha);
float fTanPs2Dl(float Alpha);
float fACosPs2Dl(float Alpha);
float fASinPs2Dl(float Alpha);
float fATanPs2Dl(float Alpha);
float fATan2Ps2Dl(float A,float B);


#define L_cos(_a_)		fCosPs2Dl((float) (_a_))
#define L_sin(_a_)		fSinPs2Dl((float) (_a_))
#define L_tan(_a_)		fTanPs2Dl((float) (_a_))

#define fSin(a)			fSinPs2Dl((float)(a))
#define fCos(a)			fCosPs2Dl((float)(a))
#define fTan(a)			fTanPs2Dl((float)(a))

#define fNormalSin(a)   fSinPs2Dl((float)(a))
#define fNormalCos(a)   fCosPs2Dl((float)(a))
#define fNormalTan(a)   fTanPs2Dl((float)(a))

#define fAtan(a)		fATanPs2Dl(a)
#define fAtan2(a, b)	fATan2Ps2Dl((a), (b))
#define fAcos(a)		fACosPs2Dl(a)
#define fAsin(a)		fASinPs2Dl(a)




/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define fOptSin fSinPs2Dl
#define fOptCos fCosPs2Dl


/*$4
 ***********************************************************************************************************************
    simple functions
 ***********************************************************************************************************************
 */
_inline_ float    ps2_fabs(float    ff)
{
    register float fo1;
    asm __volatile__ ("abs.s   %0, %1": "=f" (fo1) : "f" (ff) );
    return fo1;
}

_inline_ float    ps2_fsqrt(float    ff)
{
    register float fo2;
    asm __volatile__ ("
        nop
        nop
        sqrt.s   %0, %1
        ": "=f" (fo2): "f" (ff) );
    return fo2;
}

_inline_ float    ps2_frsqrt(float    ff)
{
    register float fo3, ft;
    register int tmp;
    
    asm __volatile__ ("lui      %0,16256"   : "=r" (tmp) );
    asm __volatile__ ("mtc1     %1, %0"     : "=f" (ft): "r" (tmp) );
    asm __volatile__ ("rsqrt.s  %0, %1, %2" : "=f" (fo3): "f" (ft), "f" (ff) );

    return fo3;
}



#define fAbs            ps2_fabs

#define L_sqrt  		ps2_fsqrt
#define fOptSqrt        ps2_fsqrt
#define fNormalSqrt     ps2_fsqrt
#define fSqrt		    ps2_fsqrt

#define fInvSqrt		ps2_frsqrt
#define fOptInvSqrt	    ps2_frsqrt
#define vOptInvSqrt(p)	(*(p) = 1.0f / ps2_fsqrt(*(p)))

#define L_pow(_a_, _b_) powf((float) (_a_), (float) (_b_))

#define fNormalInv(a)   (1.0f / (a))
#define fOptInv(a)		(1.0f / (a))
#define vOptInv(p)		(*(p) = 1.0f / (*(p)))

#define fOptDiv(a, b)	((a) / (b))
#define fSqr(f)			((f)*(f))
#define fNormalSqr(f)   ((f)*(f))
#define fCube(f)		((f)*(f)*(f))

#define fCbrt(a)		(fNormalCbrt(a))





/*$4
 ***********************************************************************************************************************
    Macros spéciales pour les valeurs absolues et négations de floatants Valeur absolue d'un float, renvoyée sous forme
    d'un LONG
 ***********************************************************************************************************************
 */

#define MATH_l_fAbsf(a) (lInterpretFloatAsLong(a) & 0x7FFFFFFF)

/*
 =======================================================================================================================
    Valeur absolue d'un float (dans un LONG) renvoyéé sous forme d'un LONG
 =======================================================================================================================
 */
#define MATH_l_fAbsl(a) ((a) & 0x7FFFFFFF)

/*
 =======================================================================================================================
    Négation d'un float, renvoyée sous forme d'un LONG
 =======================================================================================================================
 */
#define MATH_l_fOptNegf(a)	(lInterpretFloatAsLong(a) + 0x80000000)

/*
 =======================================================================================================================
    Négation d'un float (dans un LONG) renvoyée sous forme d'un LONG
 =======================================================================================================================
 */
#define MATH_l_fOptNegl(a)	((a) + 0x80000000)

/*
 =======================================================================================================================
    Aim:    Cubic root of a float
 =======================================================================================================================
 */
_inline_ float fNormalCbrt(float a)
{
	if(fSupZero(a))
		return((float) L_pow((a), Cf_Inv3));
	else if(fInfZero(a))
		return((float) - L_pow(-(a), Cf_Inv3));
	else
		return(Cf_Zero);
}

/*$5
 #######################################################################################################################
    Aim:    Commonly used math operations
 #######################################################################################################################
 */

/*
 =======================================================================================================================
    Classical operations
 =======================================================================================================================
 */
#define fAdd(a, b)	((a) + (b))
#define fSub(a, b)	((a) - (b))
#define fMul(a, b)	((a) * (b))
#define fDiv(a, b)	((a) / (b))
#define fInv(a)		(Cf_One / (a))
#define fHalf(a)	((a) * 0.5f)
#define fTwice(a)	((a) * 2.0f)
#define fMinus(a)	(-(a))
#define fNeg(a)		(-(a))
#define fExp(a)		expf(a)
#define fLn(a)		logf(a)
#define fLog10(a)	log10f(a)

/*$4
 ***********************************************************************************************************************
    Combined operations
 ***********************************************************************************************************************
 */
_inline_ float ps2_fMulAddMul(float f1, float f2, float f3, float f4)
{
    register float fo4;
    asm __volatile__ ("mula.s %0, %1" : : "f" (f1), "f" (f2) );
    asm __volatile__ ("madd.s %0, %1, %2" : "=f" (fo4) : "f" (f3), "f" (f4) );    
    return fo4;
}
#define fMulAddMul              ps2_fMulAddMul

_inline_ float ps2_fMulSubMul(float f1, float f2, float f3, float f4)
{
    register float fo5;
    asm __volatile__ ("mula.s %0, %1": : "f" (f1), "f" (f2) );
    asm __volatile__ ("msub.s %0, %1, %2": "=f" (fo5): "f" (f3), "f" (f4) );    
    return fo5;
}
#define fMulSubMul              ps2_fMulSubMul


#define fAdd3(a, b, c)			((a) + (b) + (c))
#define fAdd4(a, b, c, d)		((a) + (b) + (c) + (d))

#define fMul3(a, b, c)			((a) * (b) * (c))
#define fMul4(a, b, c, d)		((a) * (b) * (c) * (d))
#define fMiddle(a, b)			(fHalf(fAdd((a), (b))))
#define fGeoMiddle(a, b)		(fSqrt(fMul((a), (b))))

_inline_ float ps2_min(float f1, float f2)
{
    register float fo6;
    asm __volatile__ ("min.s    %0, %1, %2" : "=f" (fo6): "f" (f1), "f" (f2) );
    return fo6;
}
#define fMin(a, b)              ps2_min((a), (b))
#define fMin3(a, b, c)          ps2_min((a), ps2_min((b),(c)))

_inline_ float ps2_max(float f1, float f2)
{
    register float fo7;
    asm __volatile__ ("max.s    %0, %1, %2" : "=f" (fo7): "f" (f1), "f" (f2) );
    return fo7;
}
#define fMax(a, b)              ps2_max((a), (b))
#define fMax3(a, b, c)          ps2_max((a), ps2_max((b),(c)))

/*$4
 ***********************************************************************************************************************
    Min, Max, Signs, Limit
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Si les deux nombres sont de même signe (attention! pour ces fonctions 0 est positif, -0 est négatif), renvoie
    0x8000000, sinon renvoie 0
 =======================================================================================================================
 */
#define fSameSign(a, b)			((~(lInterpretFloatAsLong(a) ^ lInterpretFloatAsLong(b))) & 0x80000000)
#define fDifferentSign(a, b)	((lInterpretFloatAsLong(a) ^ lInterpretFloatAsLong(b)) & 0x80000000)

/*
 * Renvoie -1 en float si négatif, +1 en float si positif (attention +0 renvoie
 * +1, -0 renvoie -1)
 */

/*
 =======================================================================================================================
    Aim:    Returns the sign of a float

    Note:   Only +1 or -1 is returned (+0 returns +1 and -0 returns -1)
 =======================================================================================================================
 */
_inline_ float fSign(float a)
{
	/*~~~~~~~~~~*/
	LONG	*pl_a;
	LONG	b;
	/*~~~~~~~~~~*/

	pl_a = (LONG *) &a;

	b = (((*pl_a) & 0x80000000) | Cl_1f);
	return(*((float *) &b));
}

/*
 =======================================================================================================================
    Aim:    Changes a float to keep only its sign

    Note:   Only +1 or -1 are possible values (+0 returns +1 and -0 returns -1)
 =======================================================================================================================
 */
_inline_ void vSign(float *f)
{
	*((LONG *) f) &= 0x80000000;	/* Keep only sign */
	*((LONG *) f) |= Cl_1f;			/* Make the rest of the float be 1 */
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */

/*$5
 #######################################################################################################################
    Various functions (continued)
 #######################################################################################################################
 */

extern int	MATH_i_SolveCubicEquation(float f_A, float f_B, float f_C, MATH_tdst_Vector *);

/*
 =======================================================================================================================
    Aim:    Returns a random float between _f_min and _f_max
 =======================================================================================================================
 */
#ifndef f_Cte_invRAND_MAX
#define f_Cte_invRAND_MAX   (float)(1.0f/(float)RAND_MAX)
#endif
_inline_ float fRand(float _f_min, float _f_max)
{
    float f;

    f = fLongToFloat(rand()) * f_Cte_invRAND_MAX * (_f_max-_f_min);
    f +=  _f_min;
    return f;
}


_inline_ float MATH_f_FloatLimit(float f_In, float f_Min, float f_Max)
{
    float f;
    
    f = ps2_min(f_Max, ps2_max(f_Min, f_In));
    
	return f;
}

/*
 * Aim: Linear interpolation bewteen 2 floats Note: If _fCoeff=0 return _f1, if
 * _fCoeff=1 returns _f2
 */
 
_inline_ float MATH_f_FloatBlend(float _f1, float _f2, float _fCoeff)
{
    return(MATH_f_FloatLimit(_fCoeff, 0.0f, 1.0f)*( _f2 - _f1) +_f1);
}


/*
 =======================================================================================================================
    Aim:    Rounds a float with a given precision
 =======================================================================================================================
 */
_inline_ float MATH_f_FloatRound(float f, float f_precision)
{
	/*~~~~~~*/
	LONG	l;
	/*~~~~~~*/

	l = lFloatToLong(f * (fInv(f_precision)));
	return(fLongToFloat(l) * f_precision);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float MATH_f_FloatModulo(float _f_Val, float _f_Mod)
{
	/*~~~~~~~~~*/
	float	fVal;
	/*~~~~~~~~~*/

	fVal = (_f_Val + (_f_Mod / 2.0f)) / _f_Mod;
	fVal -= ((*(ULONG *) &fVal) & 0x80000000) ? 1.0f : 0.0f;
	fVal = _f_Mod * ((int) fVal);
	return fVal;
}

/*$4
 ***********************************************************************************************************************
    Operations using pointers
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Copy a float
 =======================================================================================================================
 */
_inline_ void vCopy(float *fDst, float *fSrc)
{
	*(LONG *) fDst = *(LONG *) fSrc;
}

/*
 =======================================================================================================================
    Aim:    Negate a float
 =======================================================================================================================
 */
_inline_ void vNeg(float *pf)
{
	(*(LONG *) pf) += 0x80000000;
}

/*
 =======================================================================================================================
    Aim:    Add two floats
 =======================================================================================================================
 */
_inline_ void vAdd(float *_pf_Dest, float *_pf_Source)
{
	*_pf_Dest += *_pf_Source;
}


#ifdef USE_ONLY_FOR_BENTCH

/*
 =======================================================================================================================
    fInvTab:: 35 / 35 / 70 (100)
 =======================================================================================================================
 */
_inline_ float fInvTab(float a)
{
	/*~~~~~~~~~~~~*/
	LONG	lResult;
	/*~~~~~~~~~~~~*/

	/* Le nouvel exposant est l'oppose de l'exposant actuel, mais on conserve le signe */
	lResult = 0x7E800000 - lGetFloatExponentAndSign(a);

	/* We get the new mantisse from the table */
	lResult += MATH_gl_TableInverse[(lInterpretFloatAsLong(a) & 0x007FE000) >> 13];

	return fInterpretLongAsFloat(lResult);
}

/*
 =======================================================================================================================
    vInvTab:: 30 / 30 / 106 (100)
 =======================================================================================================================
 */
_inline_ void vInvTab(float *pf)
{
#define F	(*(LONG *) pf)

	F = (0x7E800000 - (F & 0xFF800000)) + MATH_gl_TableInverse[(F & 0x007FE000) >> 13];

#undef F
}

/*
 =======================================================================================================================
    vTabSqrt:: 33 / 36 / 113 (100)
 =======================================================================================================================
 */
_inline_ void vTabSqrt(float *pf)
{
	(*((LONG *) pf)) = (((((*((LONG *) pf)) & 0x7F800000) + 0x3F800000) >> 1) & 0x7F800000) + MATH_gl_TableRacine[(((*((LONG *) pf)) & 0x00FFE000) >> 13)];
}

/* 
 =======================================================================================================================
    fTabSqrt:: 33 / 34 / 148 (100)
 =======================================================================================================================
 */
_inline_ float fTabSqrt(float a)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	LONG			lResult;
	register LONG	b;
	/*~~~~~~~~~~~~~~~~~~~~*/

	b = lInterpretFloatAsLong(a);

	lResult = ((((b & 0x7F800000) + 0x3F800000) >> 1) & 0x7F800000) + MATH_gl_TableRacine[((b & 0x00FFE000) >> 13)];

	return fInterpretLongAsFloat(lResult);
}

/*
 =======================================================================================================================
    fTabInvSqrt:: 37 / 38 / 154 (100)
 =======================================================================================================================
 */
_inline_ float fTabInvSqrt(float a)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	LONG			lResult;
	register LONG	b;
	/*~~~~~~~~~~~~~~~~~~~~*/

	b = lInterpretFloatAsLong(a);

	lResult = (((0xBD800000 - (b & 0x7F800000)) >> 1) & 0x7F800000) + MATH_gl_TableInverseRacine[((b & 0x00FFE000) >> 13)];

	return fInterpretLongAsFloat(lResult);
}

/*
 =======================================================================================================================
    vTabInvSqrt:: 35 / 39 / 114 (100)
 =======================================================================================================================
 */
_inline_ void vTabInvSqrt(float *_pf)
{
	(*((LONG *) _pf)) = (((0xBD800000 - ((*((LONG *) _pf)) & 0x7F800000)) >> 1) & 0x7F800000) + MATH_gl_TableInverseRacine[(((*((LONG *) _pf)) & 0x00FFE000) >> 13)];
}

#endif /* USE_ONLY_FOR_BENTCH */



#ifdef __cplusplus
}
#endif
#endif

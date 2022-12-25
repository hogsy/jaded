/*$T ix86MATHfloat.h */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Definition of all the operations on the floats */
#ifndef _ix86MATHFLOAT_H_
#define _ix86MATHFLOAT_H_
#include "BASe/BAStypes.h"

#include "MATHs/MATHextern.h"/* For the rand */
#include "MATHs/MATHconst.h"
#include "BASe/BASsys.h"
#include "MATHs/MATHstruct.h"
#include "MATHs/MATHvars.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    conversions
 ***********************************************************************************************************************
 */
#define fInterpretLongAsFloat(a)	(*((float *) &(a)))
#define lInterpretFloatAsLong(a)	(*((LONG *) &(a)))
#define fSetLongToFloat(a, b)		(*((LONG *) &(a)) = (b))
#define fSetLongToPFloat(a, b)		(*((LONG *) (a)) = (b))
#define lGetFloatMantisse(a)		(lInterpretFloatAsLong(a) & 0x007FFFFF)
#define lGetFloatExponent(a)		(lInterpretFloatAsLong(a) & 0x7F800000)
#define lGetFloatExponentAndSign(a) (lInterpretFloatAsLong(a) & 0xFF800000)
#define fLongToFloat(a)				((float) (a))

__inline int ix86_ftol(float ff)
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

__inline int iRoundf(float ff)
{
    register int io;
    register float ft;
    
    asm __volatile__ ("cvt.w.s  %0, %1" : "=f" (ft): "f" (ff) );
    asm __volatile__ ("mfc1     %0, %1" : "=r" (io): "f" (ft) );
    return io;
}

#define lFloatToLong(a)     		ix86_f2i((float)(a))

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
	Note from Alberto: I have removed this macros because it's not deterministically true that
			these comparison are faster than classical float as stated, for example if both
			operands are on the FPU stack, these comparisons require two memory writes and two memory reads
 =======================================================================================================================
#define fEqI(a, b)		(lInterpretFloatAsLong(a) == lInterpretFloatAsLong(b))
#define fDiffI(a, b)	(lInterpretFloatAsLong(a) != lInterpretFloatAsLong(b))
#define fSupI(a, b)		(lInterpretFloatAsLong(a) > lInterpretFloatAsLong(b))
#define fSupEqI(a, b)	(lInterpretFloatAsLong(a) >= lInterpretFloatAsLong(b))
#define fInfI(a, b)		(lInterpretFloatAsLong(a) < lInterpretFloatAsLong(b))
#define fInfEqI(a, b)	(lInterpretFloatAsLong(a) <= lInterpretFloatAsLong(b))
 */

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
//#define fEqLong(a, l)	(lInterpretFloatAsLong(a) == (l))

/*$F In the next two cases, the reel value must be positive*/

//#define fSupPositivLong(a, l)	(lInterpretFloatAsLong(a) > (l))
//#define fSupEqPositivLong(a, l) (lInterpretFloatAsLong(a) >= (l))

/*$F In the next two cases, the reel value must be negative */

//#define fInfNegativLong(a, l)	(lInterpretFloatAsLong(a) > (l))
//#define fInfEqNegativLong(a, l) (lInterpretFloatAsLong(a) >= (l))

/*$4
 ***********************************************************************************************************************
    trigo functions (voir Vu pour optimisation)
 ***********************************************************************************************************************
 */

#define L_sin(a)		((float)sin((a)))
#define L_cos(a)		((float)cos((a)))
#define L_tan(a)		((float)tan((a)))

#define fSin(a)			((float)sin((a)))
#define fCos(a)			((float)cos((a)))
#define fTan(a)			((float)tan((a)))

#define fNormalSin(a)   ((float)sin((a)))
#define fNormalCos(a)   ((float)cos((a)))
#define fNormalTan(a)   ((float)tan((a)))

#define fAtan(a)		((float)atan((a)))
#define fAtan2(a, b)	((float)atan2((a), (b)))
#define fAcos(a)		((float)acos((a)))
#define fAsin(a)		((float)asin((a)))




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
__inline float ix86_frsqrt(float    ff)
{
    register float fo3, ft;
    register int tmp;
    
    asm __volatile__ ("lui      %0,16256"   : "=r" (tmp) );
    asm __volatile__ ("mtc1     %1, %0"     : "=f" (ft): "r" (tmp) );
    asm __volatile__ ("rsqrt.s  %0, %1, %2" : "=f" (fo3): "f" (ft), "f" (ff) );

    return fo3;
}



#define fAbs(a)         ((float)fabs((a)))
#define L_sqrt(a)  		((float)sqrt((a)))
#define fOptSqrt(a)     ((float)sqrt((a)))
#define fNormalSqrt(a)  ((float)sqrt((a)))
#define fSqrt(a)		((float)sqrt((a)))

#define fInvSqrt(a)		(1.0f / sqrt((a))
#define fOptInvSqrt(a)  (1.0f / sqrt((a))
#define vOptInvSqrt(p)	(*(p) = 1.0f / sqrt(*(p)))

#define L_pow(a, b)		(float)pow((a), (b))

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
__inline float fNormalCbrt(float a)
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
#define fExp(a)		(float)exp(a)
#define fLn(a)		(float)log(a)
#define fLog10(a)	(float)log10(a)

/*$4
 ***********************************************************************************************************************
    Combined operations
 ***********************************************************************************************************************
 */

#define fMulSubMul(a, b, c, d)  (fMul(a, b) - fMul(c, d))
#define fMulAddMul(a, b, c, d)  (fMul(a, b) + fMul(c, d))

#define fAdd3(a, b, c)			((a) + (b) + (c))
#define fAdd4(a, b, c, d)		((a) + (b) + (c) + (d))

#define fMul3(a, b, c)			((a) * (b) * (c))
#define fMul4(a, b, c, d)		((a) * (b) * (c) * (d))
#define fMiddle(a, b)			(fHalf(fAdd((a), (b))))
#define fGeoMiddle(a, b)		(fSqrt(fMul((a), (b))))

__inline float ix86_min(float f1, float f2)
{
	return f1 < f2 ? f1 : f2;
}
#define fMin(a, b)              ix86_min((a), (b))
#define fMin3(a, b, c)          ix86_min((a), ix86_min((b),(c)))

__inline float ix86_max(float f1, float f2)
{
	return f1 < f2 ? f2 : f1;
}
#define fMax(a, b)              ix86_max((a), (b))
#define fMax3(a, b, c)          ix86_max((a), ix86_max((b),(c)))

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
__inline float fSign(float a)
{
	return a < 0 ? -1.0f : 1.0f;
}

/*
 =======================================================================================================================
    Aim:    Changes a float to keep only its sign

    Note:   Only +1 or -1 are possible values (+0 returns +1 and -0 returns -1)
 =======================================================================================================================
 */
__inline void vSign(float *f)
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
#define f_Cte_invRAND_MAX   (1.0f/(float)RAND_MAX)
#endif
__inline float fRand(float _f_min, float _f_max)
{
    return fLongToFloat(rand()) * f_Cte_invRAND_MAX * (_f_max-_f_min) + _f_min;
}


__inline float MATH_f_FloatLimit(float f_In, float f_Min, float f_Max)
{
    return ix86_min(f_Max, ix86_max(f_Min, f_In));
}

/*
 * Aim: Linear interpolation bewteen 2 floats Note: If _fCoeff=0 return _f1, if
 * _fCoeff=1 returns _f2
 */
 
__inline float MATH_f_FloatBlend(float _f1, float _f2, float _fCoeff)
{
	return	_fCoeff <= 0.0f ? _f1 :
			_fCoeff >= 1.0f ? _f2 : _fCoeff * (_f2 - _f1) + _f1;
}


/*
 =======================================================================================================================
    Aim:    Rounds a float with a given precision
 =======================================================================================================================
 */
__inline float MATH_f_FloatRound(float f, float f_precision)
{
	// TODO: optimize this
	return (float)floor(f / f_precision + 0.5f) * f_precision;
}

/*
 =======================================================================================================================
 [alb] What does this function do? It is *not* returning the modulo, really, so what it's use and
 why has it been named like that?
 =======================================================================================================================
 */
__inline float MATH_f_FloatModulo(float _f_Val, float _f_Mod)
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
__inline void vCopy(float *fDst, float *fSrc)
{
	*(LONG *) fDst = *(LONG *) fSrc;
}

/*
 =======================================================================================================================
    Aim:    Negate a float
 =======================================================================================================================
 */
__inline void vNeg(float *pf)
{
	(*(LONG *) pf) += 0x80000000;
}

/*
 =======================================================================================================================
    Aim:    Add two floats
 =======================================================================================================================
 */
__inline void vAdd(float *_pf_Dest, float *_pf_Source)
{
	*_pf_Dest += *_pf_Source;
}


#ifdef USE_ONLY_FOR_BENTCH

/*
 =======================================================================================================================
    fInvTab:: 35 / 35 / 70 (100)
 =======================================================================================================================
 */
__inline float fInvTab(float a)
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
__inline void vInvTab(float *pf)
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
__inline void vTabSqrt(float *pf)
{
	(*((LONG *) pf)) = (((((*((LONG *) pf)) & 0x7F800000) + 0x3F800000) >> 1) & 0x7F800000) + MATH_gl_TableRacine[(((*((LONG *) pf)) & 0x00FFE000) >> 13)];
}

/* 
 =======================================================================================================================
    fTabSqrt:: 33 / 34 / 148 (100)
 =======================================================================================================================
 */
__inline float fTabSqrt(float a)
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
__inline float fTabInvSqrt(float a)
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
__inline void vTabInvSqrt(float *_pf)
{
	(*((LONG *) _pf)) = (((0xBD800000 - ((*((LONG *) _pf)) & 0x7F800000)) >> 1) & 0x7F800000) + MATH_gl_TableInverseRacine[(((*((LONG *) _pf)) & 0x00FFE000) >> 13)];
}

#endif /* USE_ONLY_FOR_BENTCH */



#ifdef __cplusplus
}
#endif
#endif

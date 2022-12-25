/*$T MATHfloat.h GC!1.52 01/17/00 10:10:32 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Definition of all the operations on the floats
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef     _GCMATHFLOAT_H_
#define     _GCMATHFLOAT_H_


#include    <math.h>
#include    "BASe/BAStypes.h"
#include    "MATHs/MATHextern.h"
#include    "MATHs/MATHconst.h"
#include    "BASe/BASsys.h"
#include    "MATHs/MATHstruct.h"
#include    "MATHs/MATHvars.h"


#define L_cos(_a_)          cosf((float)_a_)
#define L_sin(_a_)          sinf((float)_a_)
#define L_tan(_a_)          tanf((float)_a_)
#define L_pow(_a_, _b_)     powf((float)_a_, (float)_b_)
#define L_sqrt(_a_)         sqrtf((float)_a_)

#ifdef __cplusplus
extern "C"
{
#endif

/*
 ===================================================================================================
    Interpretation de float en LONG et inversement
 ===================================================================================================
 */
#define fInterpretLongAsFloat(a)    (*((float *) &(a)))
#define lInterpretFloatAsLong(a)    (*((LONG *) &(a)))
#define fSetLongToFloat(a, b)       (*((LONG *) &(a)) = (b))
#define fSetLongToPFloat(a, b)      (*((LONG *) (a)) = (b))

/*
 ===================================================================================================
    Gestion des exposants et mantisses
 ===================================================================================================
 */
#define lGetFloatMantisse(a)        (lInterpretFloatAsLong(a) & 0x007FFFFF)
#define lGetFloatExponent(a)        (lInterpretFloatAsLong(a) & 0x7F800000)
#define lGetFloatExponentAndSign(a) (lInterpretFloatAsLong(a) & (LONG)0xFF800000)

/*$4
 ***************************************************************************************************
    Float to Loang and Long To Float conversion
 ***************************************************************************************************
 */

#define lFloatToLong(a) ((s32)((a) > 0.0f ? (a)+0.5f : (a)-0.5f))
#define fLongToFloat(a) ((float) (a))

/*$5
 ###################################################################################################
    COMPARISONS
 ###################################################################################################
 */

/*$4
 ***************************************************************************************************
    Comparisons between floats
 ***************************************************************************************************
 */

#define fEq(a, b)               ((a) == (b))
#define fEqWithEpsilon(a, b, e) fNulWithEpsilon((a) - (b), e)
#define fDiff(a, b)             ((a) != (b))
#define fSup(a, b)              ((a) > (b))
#define fSupEq(a, b)            ((a) >= (b))
#define fInf(a, b)              ((a) < (b))
#define fInfEq(a, b)            ((a) <= (b))

#define fNul(a)                 ((a) == Cf_Zero)
#define fNulWithEpsilon(a, e)   (fAbs(a) < (e))
#define fAlmostNul(a)           fNulWithEpsilon(a, Cf_Epsilon)
#define fInfZero(a)             ((a) < Cf_Zero)
#define fEqZero                 fNul
#define fInfEqZero(a)           ((a) <= Cf_Zero)
#define fSupZero(a)             ((a) > Cf_Zero)
#define fSupEqZero(a)           ((a) >= Cf_Zero)

/*$4
 ***************************************************************************************************
    Aim:    Comparisons between strictly POSITIVE floats

    Note:   These comparisons are faster than classical float comparisons £
            Same name as the classic float comparison functions, but with an I at the end
 ***************************************************************************************************
 */

#define fEqI(a, b)      (lInterpretFloatAsLong(a) == lInterpretFloatAsLong(b))
#define fDiffI(a, b)    (lInterpretFloatAsLong(a) != lInterpretFloatAsLong(b))
#define fSupI(a, b)     (lInterpretFloatAsLong(a) > lInterpretFloatAsLong(b))
#define fSupEqI(a, b)   (lInterpretFloatAsLong(a) >= lInterpretFloatAsLong(b))
#define fInfI(a, b)     (lInterpretFloatAsLong(a) < lInterpretFloatAsLong(b))
#define fInfEqI(a, b)   (lInterpretFloatAsLong(a) <= lInterpretFloatAsLong(b))

/*$4
 ***************************************************************************************************
    Comparaison entiere d'un floatant avec une constante floatante définie en entier il faudra
    définir toutes les constantes floats en entier en plus de la définition en floats!
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Comparaisons entiere de deux floats. Dans les cas suivant, le LONG peut être positif ou negatif
 ===================================================================================================
 */
#define fEqLong(a, l)   (lInterpretFloatAsLong(a) == (l))

/*$F In the next two cases, the reel value must be positive*/
#define fSupPositivLong(a, l)   (lInterpretFloatAsLong(a) > (l))
#define fSupEqPositivLong(a, l) (lInterpretFloatAsLong(a) >= (l))

/*$F In the next two cases, the reel value must be negative */
#define fInfNegativLong(a, l)   (lInterpretFloatAsLong(a) > (l))
#define fInfEqNegativLong(a, l) (lInterpretFloatAsLong(a) >= (l))

/*$5
 ###################################################################################################
    Various Functions
 ###################################################################################################
 */

/*$4
 ***************************************************************************************************
    Table Optimised functions
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Calcul de l'inverse par une table

    Time:   Testé sur K6/2-333 vOptInv 11 cycles (cache ok) fOptInv 17 cycles (cache ok)

    Note:   Entre 0 et 1: erreur moyenne de 0.003 £
            ATTENTION: erreur maximale> 1 pour les valeurs entre 0 et 1e-4 £
            entre 1000 et 10000: erreur maximale de 1e-6 £
            attention au cache qui peut faire perdre du temps si utilisé isolément
 ===================================================================================================
 */
#define fOptInv(a)  (1.0f/(float)(a)) 
/*
_inline_ float fOptInv(register float a)
{
	register float fReturnValue;
	asm {
		fres fReturnValue,a
	}
	return fReturnValue;
}
*/
/*
 ===================================================================================================
    Aim:    Same function as above but changes directly into memory
 ===================================================================================================
 */
_inline_ void vOptInv(float *pf)
{
	*pf = fOptInv(*pf);
}

/*
 ===================================================================================================
    Aim:    Divide 2 numbers using a table

    Time:   Tested on K6/2-333 21 cycles (cache ok)

    Note:   Cf. fOptInv
 ===================================================================================================
 */
_inline_ float fOptDiv(float a, float b)
{
    return ((a) * fOptInv(b));
}

/*
 ===================================================================================================
    Aim:    Calcul de le racine carre par une table
 ===================================================================================================
 */
_inline_ void vOptSqrt(float *pf)
{
    (*((LONG *) pf)) = (((((*((LONG *) pf)) & 0x7F800000) + 0x3F800000) >> 1) & 0x7F800000) +
        MATH_gl_TableRacine[(((*((LONG *) pf)) & 0x00FFE000) >> 13)];
}

/*
 ===================================================================================================
    Aim:    Same function as above but the rsult is returned in a float
 ===================================================================================================
 */
_inline_ float fOptSqrt(float a)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG            lResult;
    register LONG   b;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b = lInterpretFloatAsLong(a);

    lResult = ((((b & 0x7F800000) + 0x3F800000) >> 1) & 0x7F800000) + MATH_gl_TableRacine[
        ((b & 0x00FFE000) >> 13)];

    return fInterpretLongAsFloat(lResult);
}

/*
 ===================================================================================================
    Aim:    Calculate the sine using a table £

    Time:   Mesuré sur K6/2-333 20-22 cycles (cache ok)£
            détails: entre [0;2pi] : 20 cycles £
            en dehors de [0;2pi]: 22 cycles £

    Note:   Précision: £
            - erreur maximale possible de 0.00305 £
            - erreur moyenne de 0.00098 £
            attention au cache qui peut faire perdre du temps si utilisé isolément
 ===================================================================================================
 */
_inline_ float fOptSin(float a)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    register LONG   index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* On calcule l'index dans la table de sinus */
    index = lFloatToLong(a * MATH_gf_1024by2Pi);

    /* On ramene i dans l'intervale [0;1024], c'est a dire [0;2Pi] */
    if(index < 0)
        index = 1024 + (index | (LONG)0xfffffc00);
    else if(index > 1023) index &= 0x3ff;
    return MATH_gf_TableSin[index];
}

/*
 ===================================================================================================
    Aim:    Calculate the cosine using a table £

    Time:   Mesuré sur K6/2-333 20-22 cycles (cache ok)£
            détails: entre [0;2pi] : 20 cycles £
            en dehors de [0;2pi]: 22 cycles £

    Note:   Précision: £
            - erreur maximale possible de 0.00305 £
            - erreur moyenne de 0.00098 £
            attention au cache qui peut faire perdre du temps si utilisé isolément
 ===================================================================================================
 */
_inline_ float fOptCos(float a)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    register LONG   index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* On calcule l'index dans la table de sinus */

    /* Cos(a) = sin (a+pi/2) , 256 correspond a pi/2 comme decalage */
    index = lFloatToLong(a * MATH_gf_1024by2Pi) + 256;

    /* On ramene i dans l'intervale [0;1024], c'est a dire [0;2Pi] */
    if(index < 0)
        index = 1024 + (index | (LONG)0xfffffc00);
    else if(index > 1023) index &= 0x3ff;
    return MATH_gf_TableSin[index];
}

/*
 ===================================================================================================
    Aim:    Calculates (1 / quareroot) using a table £

    Time:   Tested on K6/2-333 £
            fOptInvSqrt: 11 (cache ok) £
            vOptInvSqrt 6-10 (cache ok, selon le contexte) £

    Note:   Précision: £
            entre 0 et 1: erreur maximale possible de 0.0003 erreur moyenne de 0.00007 £
            entre 1000 et 10000: erreur moyenne de 0.01 £
            attention au cache qui peut faire perdre du temps si utilisé isolément
 ===================================================================================================
 */
_inline_ float fOptInvSqrt(register float a)
{
    return 1.0f/sqrtf(a);
    /*
	register float fReturnValue;
	asm {
		frsqrte fReturnValue,a
	}
	return fReturnValue;
	*/
}

/*
 ===================================================================================================
    Aim:    Optimized C code. changes directly the float pointed by _pf
 ===================================================================================================
 */
_inline_ void vOptInvSqrt(float *_pf)
{
	*_pf = fOptInvSqrt(*_pf);
}

/*$4
 ***************************************************************************************************
    Macros spéciales pour les valeurs absolues et négations de floatants Valeur absolue d'un float,
    renvoyée sous forme d'un LONG
 ***************************************************************************************************
 */

#define MATH_l_fAbsf(a) (lInterpretFloatAsLong(a) & 0x7FFFFFFF)

/*
 ===================================================================================================
    Valeur absolue d'un float (dans un LONG) renvoyéé sous forme d'un LONG
 ===================================================================================================
 */
#define MATH_l_fAbsl(a) ((a) & 0x7FFFFFFF)

/*
 ===================================================================================================
    Négation d'un float, renvoyée sous forme d'un LONG
 ===================================================================================================
 */
#define MATH_l_fOptNegf(a)  (lInterpretFloatAsLong(a) + 0x80000000)

/*
 ===================================================================================================
    Négation d'un float (dans un LONG) renvoyée sous forme d'un LONG
 ===================================================================================================
 */
#define MATH_l_fOptNegl(a)  ((a) + 0x80000000)

/*$4
 ***************************************************************************************************
    Normal versions of some precedently optimised coded functions
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Square of a float
 ===================================================================================================
 */
_inline_ float fNormalSqr(float f)
{
    return(f * f);
}

/*
 ===================================================================================================
    Aim:    Cube of a float
 ===================================================================================================
 */
_inline_ float fNormalCube(float f)
{
    return(f * f * f);
}

/*
 ===================================================================================================
    Aim:    Normal (non optimised) quare root
 ===================================================================================================
 */
_inline_ float fNormalSqrt(float a)
{
    return((float) L_sqrt(a));
}

/*
 ===================================================================================================
    Aim:    Normal (non using table) sine
 ===================================================================================================
 */
_inline_ float fNormalSin(float a)
{
    return((float)L_sin(a));
}

/*
 ===================================================================================================
    Aim:    Normal (non using table) cosine
 ===================================================================================================
 */
_inline_ float fNormalCos(float a)
{
    return((float)L_cos(a));
}

/*
 ===================================================================================================
    Aim:    Normal (non using table) tangent
 ===================================================================================================
 */
_inline_ float fNormalTan(float a)
{
    return((float)L_tan(a));
}

/*
 ===================================================================================================
    Aim:    Normal (non using table) inverse squere root
 ===================================================================================================
 */
_inline_ float fNormalInvSqrt(float a)
{
    return((float) (Cf_One / fNormalSqrt(a)));
}

/*
 ===================================================================================================
    Aim:    Normal (non using table) inverse
 ===================================================================================================
 */
_inline_ float fNormalInv(float a)
{
    return((float) (Cf_One / a));
}

/*
 ===================================================================================================
    Aim:    Cubic root of a float
 ===================================================================================================
 */
_inline_ float fNormalCbrt(float a)
{
    if(fSupZero(a))
        return((float) L_pow( (a), Cf_Inv3));
    else if(fInfZero(a))
        return((float) - L_pow( - (a), Cf_Inv3));
    else
        return(Cf_Zero);
}

/*$5
 ###################################################################################################
    Aim:    Commonly used math operations
 ###################################################################################################
 */

/*
 ===================================================================================================
    Classical operations
 ===================================================================================================
 */
#define fAdd(a, b)      ((a) + (b))
#define fSub(a, b)      ((a) - (b))
#define fMul(a, b)      ((a) * (b))
#define fDiv(a, b)      ((a)/(b))
#define fInv(a)         (1.0f/(a))
#define fHalf(a)        ((a) * 0.5f)
#define fTwice(a)       ((a) * 2.0f)
#define fMinus(a)       (-(a))
#define fNeg(a)         (-(a))
#define fSqr(a)         ((a)*(a))
#define fCube(a)        ((a)*(a)*(a))
#define fSqrt(a)        (fOptSqrt(a))
#define fInvSqrt(a)     (fOptInvSqrt(a))
#define fCbrt(a)        (fNormalCbrt(a))
#define fSin(a)         (fNormalSin(a))
#define fCos(a)         (fNormalCos(a))
#define fTan(a)         (fNormalTan(a))
#define fAtan(a)        (atanf((float) (a)))
#define fAtan2(a, b)    (atan2f((float) (a), (float) (b)))
#define fAcos(a)        (acosf((float) (a)))
#define fAsin(a)        (asinf((float) (a)))
#define fExp(a)         (expf((float) (a)))
#define fLog10(a)       (log10f((float) (a)))
//#define fLn(a)          (logf((float) (a)))
_inline_ float fLn(float a) { return logf((float) (a));}


//#define fAbs(a)         __fabs(a)
inline float fAbs(register float _fValue)
{
	register float fResult;
	asm {
		ps_abs fResult,_fValue
	}
	return fResult;
}


/*$4
 ***************************************************************************************************
    Combined operations
 ***************************************************************************************************
 */

#define fAdd3(a, b, c)          ((a) + (b) + (c))
#define fAdd4(a, b, c, d)       ((a) + (b) + (c) + (d))
#define fMulSubMul(a, b, c, d)  (fMul(a, b) - fMul(c, d))
#define fMulAddMul(a, b, c, d)  (fMul(a, b) + fMul(c, d))
#define fMul3(a, b, c)          ((a) * (b) * (c))
#define fMul4(a, b, c, d)       ((a) * (b) * (c) * (d))
#define fMiddle(a, b)           (fHalf(fAdd(a, b)))
#define fGeoMiddle(a, b)        (fSqrt(fMul(a, b)))

/*$4
 ***************************************************************************************************
    Min, Max, Signs, Limit
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Si les deux nombres sont de même signe (attention! pour ces fonctions 0 est positif, -0 est
    négatif), renvoie 0x8000000, sinon renvoie 0
 ===================================================================================================
 */
#define fSameSign(a, b)         ((~(lInterpretFloatAsLong(a) ^ lInterpretFloatAsLong(b))) & 0x80000000)
#define fDifferentSign(a, b)    ((lInterpretFloatAsLong(a) ^ lInterpretFloatAsLong(b)) & 0x80000000)

/*
 * Renvoie -1 en float si négatif, +1 en float si positif (attention +0 renvoie +1, -0 renvoie
 * -1)
 */

/*
 ===================================================================================================
    Aim:    Returns the sign of a float

    Note:   Only +1 or -1 is returned (+0 returns +1 and -0 returns -1)
 ===================================================================================================
 */
_inline_ float fSign(float a)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *pl_a;
    LONG    b;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pl_a = (LONG *) &a;

    b = (((*pl_a) & (LONG)0x80000000) | Cl_1f);
    return(*((float *) &b));
}

/*
 ===================================================================================================
    Aim:    Changes a float to keep only its sign

    Note:   Only +1 or -1 are possible values (+0 returns +1 and -0 returns -1)
 ===================================================================================================
 */
_inline_ void vSign(float *f)
{
    *((LONG *) f) &= (LONG)0x80000000;    /* Keep only sign */
    *((LONG *) f) |= Cl_1f;			/* Make the rest of the float be 1 */
}

/*
 ===================================================================================================
    Aim:    Return the minimum of two or three floats
 ===================================================================================================
 */
_inline_ float fMin(register float a, register float b)
{
	register float fTemp;
	asm {
		fsubs fTemp, a, b
		ps_sel fTemp, fTemp, b, a
	}
	return fTemp;
    //return((a <= b) ? a : b);
}

extern float    fMin3(float a, float b, float c);

/*
 ===================================================================================================
    Aim:    Return the maximum of two or three floats
 ===================================================================================================
 */
_inline_ float fMax(register float a, register float b)
{
	register float fTemp;
	asm {
		fsubs fTemp, a, b
		ps_sel fTemp, fTemp, a, b
	}
	return fTemp;
    //return((a >= b) ? a : b);
}

extern float    fMax3(float a, float b, float c);

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ float MATH_f_FloatLimit(float f_In, float f_Min, float f_Max)
{
    if(f_In > f_Max)
        return f_Max;
    else 
    	return fMax(f_In,f_Min);
}

/*$5
 ###################################################################################################
    Various functions (continued)
 ###################################################################################################
 */

extern int  MATH_i_SolveCubicEquation(float f_A, float f_B, float f_C, MATH_tdst_Vector *pst_V);

/*
 ===================================================================================================
    Aim:    Returns a random float between _f_min and _f_max
 ===================================================================================================
 */
_inline_ float fRand(float _f_min, float _f_max)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Returns a random float between 0 and 1 */
    f = fDiv(fLongToFloat((LONG) rand()), fLongToFloat(RAND_MAX));

    /* Puts it between the min and the max range */
    return(f * (_f_max - _f_min) + _f_min);
}

/*
 ===================================================================================================
    Aim:    Linear interpolation bewteen 2 floats

    Note:   If _fCoeff=0 return _f1, if _fCoeff=1 returns _f2
 ===================================================================================================
 */
_inline_ float MATH_f_FloatBlend(float _f1, float _f2, float _fCoeff)
{
	if(_fCoeff < 0.0f) _fCoeff = 0.0f;
	if(_fCoeff > 1.0f) _fCoeff = 1.0f;
    return((Cf_One - _fCoeff) * _f1 + _f2 * _fCoeff);
}

/*
 ===================================================================================================
    Aim:    Rounds a float with a given precision
 ===================================================================================================
 */
_inline_ float MATH_f_FloatRound(float f, float f_precision)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    l;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    l = lFloatToLong(f * (fInv(f_precision)));
    return(fLongToFloat(l) * f_precision);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float MATH_f_FloatModulo(float _f_Val, float _f_Mod)
{
    /*~~~~~~~~~~~~~*/
    float   fVal;
    /*~~~~~~~~~~~~~*/

    fVal = (_f_Val + (_f_Mod / 2.0f)) / _f_Mod;
    fVal -= ((*(ULONG *) &fVal) & 0x80000000) ? 1.0f : 0.0f;
    fVal = _f_Mod * ((int) fVal);
    return fVal;
}


/*$4
 ***************************************************************************************************
    Operations using pointers
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Copy a float
 ===================================================================================================
 */
_inline_ void vCopy(float *fDst, float *fSrc)
{
    *(LONG *) fDst = *(LONG *) fSrc;
}

/*
 ===================================================================================================
    Aim:    Negate a float
 ===================================================================================================
 */
_inline_ void vNeg(float *pf)
{
    (*(LONG *) pf) += 0x80000000;
}

/*
 ===================================================================================================
    Aim:    Add two floats
 ===================================================================================================
 */
_inline_ void vAdd(float *_pf_Dest, float *_pf_Source)
{
    *_pf_Dest += *_pf_Source;
}

#ifdef __cplusplus
}
#endif
#endif /* _MATHFLOAT_H_ */

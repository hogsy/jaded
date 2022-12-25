/*$T MATHlong.h GC!1.5 10/06/99 17:32:21 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Definition of all the operations on the floats
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _MATHLONG_H_
#define _MATHLONG_H_

#include "BASe/BAStypes.h"
#include "BASe/BASsys.h" /* for _inline_ */
#include "MATHs/MATHextern.h" /* for the rand */
#include "MATHs/MATHfloat.h"

/*$5 
 ##################################################################################################
    Aim:    Commonly used math operations
 ###################################################################################################
 */

/*
 ===================================================================================================
    Classical operations
 ===================================================================================================
 */
#ifdef _GAMECUBE 
#define lAbs(a)     __abs(a)
#else
#define lAbs(a)     ((a >= 0) ? (a) : -(a))
#endif

#define lSqr(a)     ((a) * (a))
#define lCube(a)    ((a) * (a) * (a))
#define lSqrt(a)    (lFloatToLong(fSqrt(fLongToFloat(a))))
#define lHalf(a)    ((a) >> 1)
#define lTwice(a)   ((a) << 1)

/*$4
 ***************************************************************************************************
    Combined operations
 ***************************************************************************************************
 */

#define lAdd3(a, b, c)          ((a) + (b) + (c))
#define lAdd4(a, b, c, d)       ((a) + (b) + (c) + (d))
#define lMulSubMul(a, b, c, d)  ((a * b) - (c * d))
#define lMulAddMul(a, b, c, d)  ((a * b) + (c * d))
#define lMul3(a, b, c)          ((a) * (b) * (c))
#define lMul4(a, b, c, d)       ((a) * (b) * (c) * (d))
#define lMiddle(a, b)           (lHalf(a + b))
#define lGeoMiddle(a, b)        (lSqrt(a * b))

/*$4
 ***************************************************************************************************
    Min, Max, Signes
 ***************************************************************************************************
 */

#define lSameSign(a, b)         (!(((a) ^ (b)) & 0x80000000))
#define lDifferentSign(a, b)    (((a) ^ (b)) & 0x80000000)

/*
 ===================================================================================================
    Aim:    Returns the sign of an integer (-1 if negativ, 0 if null, +1 if positive)
 ===================================================================================================
 */
_inline_ LONG lSign(LONG i)
{
    if(i > 0)
        return(1);
    else if(i < 0)
        return(-1);
    else
        return(0);
}

/*
 ===================================================================================================
    Aim:    Return minimum of two integers
 ===================================================================================================
 */
_inline_ LONG lMin(LONG a, LONG b)
{
#ifdef PSX2_TARGET
	register ULONG ReturnValue;
	asm __volatile__ ("pminw %0 , %1 , %2  ": "=r" (ReturnValue): "r" (a)  , "r" (b) );
	return ReturnValue;
#else
    return((a <= b) ? a : b);
#endif    
}

/*
 ===================================================================================================
    Aim:    Return the maximum of two integers
 ===================================================================================================
 */
_inline_ LONG lMax(LONG a, LONG b)
{
#ifdef PSX2_TARGET
	register ULONG ReturnValue;
	asm __volatile__ ("pmaxw %0 , %1 , %2  ": "=r" (ReturnValue): "r" (a)  , "r" (b) );
	return ReturnValue;
#else
    return((a >= b) ? a : b);
#endif    
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ LONG lClamp(LONG val, LONG min, LONG max)
{
    if      (val < min) return min;
    else if (val > max) return max;
    else return val;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
#define rand JADErand
#if defined (__cplusplus) && !defined(JADEFUSION)
extern  "C"
{
#endif

extern int JADErand(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

/*$5
 ###################################################################################################
    Various functions
 ###################################################################################################
 */

/*
 ===================================================================================================
    Aim:    Return random LONG between a min and a max
 ===================================================================================================
 */
extern ULONG INO_n_FrameCounter;
extern int INO_b_RecordInput;
extern int INO_b_PlayInput;

#ifdef PSX2_TARGET
#ifndef f_Cte_invRAND_MAX
#define f_Cte_invRAND_MAX   (float)(1.0f/(float)RAND_MAX)
#endif

_inline_ LONG lRand(LONG _l_min, LONG _l_max)
{
    float f;
    int ij;

    if (INO_b_RecordInput || INO_b_PlayInput)
    {
        srand(INO_n_FrameCounter);
    }

    f = fLongToFloat(rand()) * f_Cte_invRAND_MAX * (fLongToFloat(_l_max) - fLongToFloat(_l_min));//ij) ;
    ij = iRoundf(f) + _l_min;

	return (ij == _l_max) ? ij - 1 : ij;
}
#else
_inline_ LONG lRand(LONG _l_min, LONG _l_max)
{
	LONG l;

    if (INO_b_RecordInput || INO_b_PlayInput)
    {
        srand(INO_n_FrameCounter);
    }
    /* Puts it between the min and the max range */
    l = (((rand() * (_l_max - _l_min)) / (RAND_MAX)) + _l_min);
	return (l == _l_max) ? l - 1 : l;
}
#endif
#endif

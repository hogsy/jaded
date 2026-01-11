/*$T GCMATHvector.h GC! 1.081 07/22/02 08:33:14 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* GCMATHVector.h: All vector operations */
#ifndef __GCMATHVECTOR_H__
#define __GCMATHVECTOR_H__

#include "MATHs/GC/GCMATHfloat.h"
#include "MATHs/GC/GCMATHgector.h"
#include "BASe/BAStypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    all functions are optimized for GameCube
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Tests if a vector is nul with an epsilon
 =======================================================================================================================
 */
inline int MATH_b_NulVectorWithEpsilon(const MATH_tdst_Vector *A, const float fEps)
{
	/*~~~~~~~~~*/
	LONG	*pA;
	LONG	lEps;
	/*~~~~~~~~~*/

	pA = (LONG *) A;
	lEps = *(LONG *) &fEps;

	/*
	 * On prend la valeur absolue des floatants et on fait au plus 3 comparaisons
	 * entieres
	 */
	if(MATH_l_fAbsl(*pA) > lEps) return 0;
	if(MATH_l_fAbsl(*(pA + 1)) > lEps) return 0;
	if(MATH_l_fAbsl(*(pA + 2)) > lEps) return 0;
	return 1;
}

/*
 =======================================================================================================================
    Aim:    Tests if a vector is exactly nul
 =======================================================================================================================
 */
inline int MATH_b_NulVector(const MATH_tdst_Vector *pst_Vector)
{
	/*~~~~~~~~*/
	LONG	*pl;
	/*~~~~~~~~*/

	pl = (LONG *) pst_Vector;

	if((MATH_l_fAbsl(*pl) != 0) || (MATH_l_fAbsl(*(pl + 1)) != 0) ||(MATH_l_fAbsl(*(pl + 2)) != 0))
	{
		return 0;
	}
	
	
	return 1;
}

/*
 =======================================================================================================================
    Aim:    Take the absolute value for each coordinate of a vector
 =======================================================================================================================
 */
inline void MATH_AbsVector(register MATH_tdst_Vector *_pst_Mdst, register MATH_tdst_Vector *_pst_Msrc)
{
	_pst_Mdst->x = fAbs(_pst_Msrc->x);
	_pst_Mdst->y = fAbs(_pst_Msrc->y);
	_pst_Mdst->z = fAbs(_pst_Msrc->z);
}

/*
 =======================================================================================================================
    Aim:    Initialize a vector to 0
 =======================================================================================================================
 */
inline void MATH_InitVectorToZero(register MATH_tdst_Vector *VDst)
{
	VDst->x = VDst->y = VDst->z = 0.0f;
}

#define MATH_SetNulVector	MATH_InitVectorToZero

/*
 =======================================================================================================================
    Aim:    Initialize a vector with 3 floats using values for the floats
 =======================================================================================================================
 */
inline void MATH_InitVector(register MATH_tdst_Vector *VDst, register float x, register float y, register float z)
{
	VDst->x = x;
	VDst->y = y;
	VDst->z = z;
}

#define MATH_SetVector	MATH_InitVector

/*
 =======================================================================================================================
    Aim:    Vdest=A+B;
 =======================================================================================================================
 */
inline void MATH_AddVector
(
	register MATH_tdst_Vector	*dst,
	register MATH_tdst_Vector	*vec1,
	register MATH_tdst_Vector	*vec2
)
{
#ifdef _C_MATH
	dst->x = fAdd(vec1->x, vec2->x);
	dst->y = fAdd(vec1->y, vec2->y);
	dst->z = fAdd(vec1->z, vec2->z);
#else

	register float V1_XY, V2_XY, V1_Z, V2_Z;
	register float D1_XY, D1_Z;

	asm
	{
		/* load vectors XY */
		psq_l	V1_XY, 0(vec1), 0, 0;
		psq_l	V2_XY, 0(vec2), 0, 0;

		/* add vectors XY */
		ps_add	D1_XY, V1_XY, V2_XY;

		/* store result XY */
		psq_st	D1_XY, 0(dst), 0, 0;

		/* load vectors Z */
		psq_l	V1_Z, 8(vec1), 1, 0;
		psq_l	V2_Z, 8(vec2), 1, 0;

		/* add vectors Z */
		ps_add	D1_Z, V1_Z, V2_Z;

		/* store result Z */
		psq_st	D1_Z, 8(dst), 1, 0;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}

#endif
}

/*
 =======================================================================================================================
    Aim:    Add two vectors
 =======================================================================================================================
 */
#define MATH_AddEqualVector(A, B)	MATH_AddVector(A, A, B)

/*
 =======================================================================================================================
    Aim:    Substract two vectors
 =======================================================================================================================
 */
inline void MATH_SubVector
(
	register MATH_tdst_Vector	*dst,
	const register MATH_tdst_Vector	*vec1,
	const register MATH_tdst_Vector	*vec2
)
{
#ifdef _C_MATH
	dst->x = fSub(vec1->x, vec2->x);
	dst->y = fSub(vec1->y, vec2->y);
	dst->z = fSub(vec1->z, vec2->z);
#else
    register float V1_XY, V2_XY, V1_Z, V2_Z;
    register float D1_XY, D1_Z;

	asm
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* load vectors XY */
		psq_l	V1_XY, 0(vec1), 0, 0;
		psq_l	V2_XY, 0(vec2), 0, 0;

		/* subtract vectors XY */
		ps_sub	D1_XY, V1_XY, V2_XY;

		/* store vectors XY */
		psq_st	D1_XY, 0(dst), 0, 0;

		/* load vectors Z */
		psq_l	V1_Z, 8(vec1), 1, 0;
		psq_l	V2_Z, 8(vec2), 1, 0;

		/* subtract vectors Z */
		ps_sub	D1_Z, V1_Z, V2_Z;

		/* store vectors Z */
		psq_st	D1_Z, 8(dst), 1, 0;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}

#endif
}

/*
 =======================================================================================================================
    Aim:    Sub two vectors
 =======================================================================================================================
 */
#define MATH_SubEqualVector(A, B)	MATH_SubVector(A, A, B)

/*
	dst = a*S+b
*/
inline void MATH_MulAddScalarVector
(
          register MATH_tdst_Vector *dst,
    const register MATH_tdst_Vector *a,
    const register MATH_tdst_Vector *b,
	const register f32 S
)
{
#ifdef _C_MATH

	int i;
	for( i = 0; i < 3; ++i )
		dst[i] = a[i]*S+b[i];

#else /* GEKKO */
    register f32 vxy1, vz1, vxy2, vz2,rxy, rz;

    asm
    {
		// load vectors XY
        psq_l     vxy1, 0(a), 0, 0
        psq_l     vz1,  8(a), 1, 0
        psq_l     vxy2, 0(b), 0, 0
        psq_l     vz2,  8(b), 1, 0

		// muladd XY
        ps_madds0    rxy, vxy1, S, vxy2

		// store res XY
        psq_st    rxy, 0(dst), 0, 0

		// muladd Z
        ps_madds0    rz,  vz1,  S, vz2

		// store res Z
        psq_st    rz,  8(dst), 1, 0
    }
#endif /* GEKKO */
}


/*
 =======================================================================================================================
    Aim:    Copy a vector
 =======================================================================================================================
 */
inline void MATH_CopyVector(register MATH_tdst_Vector *A, const register MATH_tdst_Vector *B)
{
	A->x = B->x;
	A->y = B->y;
	A->z = B->z;
}

/*
 =======================================================================================================================
    Aim:    Negate a vector
 =======================================================================================================================
 */
inline void MATH_NegEqualVector(register MATH_tdst_Vector *_pst_Vector)
{
	/*~~~~~~~~~*/
	LONG	*Src;
	/*~~~~~~~~~*/

	Src = (LONG *) _pst_Vector;

	*Src += 0x80000000;
	*(Src + 1) += 0x80000000;
	*(Src + 2) += 0x80000000;
}

/*
 =======================================================================================================================
    Aim:    Negate a vector
 =======================================================================================================================
 */
inline void MATH_NegVector(register MATH_tdst_Vector *_pst_VDst, register MATH_tdst_Vector *_pst_VSrc)
{
	/*~~~~~~~~~~~~~~~*/
	LONG	*Src, *Dst;
	/*~~~~~~~~~~~~~~~*/

	Src = (LONG *) _pst_VSrc;
	Dst = (LONG *) _pst_VDst;

	*Dst = *Src + 0x80000000;
	*(Dst + 1) = *(Src + 1) + 0x80000000;
	*(Dst + 2) = *(Src + 2) + 0x80000000;
}

/*
 =======================================================================================================================
    Aim:    Invert a vector

    Note:   (1/x,1/y,1/z) is performed, NOT (-x,-y,-z)...
 =======================================================================================================================
 */
inline void MATH_InvVector(MATH_tdst_Vector *_pst_Vdst, MATH_tdst_Vector *_pst_VSrc)
{
	_pst_Vdst->x = fOptInv(_pst_VSrc->x);
	_pst_Vdst->y = fOptInv(_pst_VSrc->y);
	_pst_Vdst->z = fOptInv(_pst_VSrc->z);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MATH_InvEqualVector(MATH_tdst_Vector *_pst_VSrc)
{
	_pst_VSrc->x = fOptInv(_pst_VSrc->x);
	_pst_VSrc->y = fOptInv(_pst_VSrc->y);
	_pst_VSrc->z = fOptInv(_pst_VSrc->z);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MATH_OptInvEqualVector(MATH_tdst_Vector *pst_Vector)
{
	vOptInv(&(pst_Vector->x));
	vOptInv(&(pst_Vector->y));
	vOptInv(&(pst_Vector->z));
}

/*
 =======================================================================================================================
    Aim:    Scale a vector (multiplies each coordinate with same value)
 =======================================================================================================================
 */
inline void MATH_ScaleVector(register MATH_tdst_Vector *dst, register MATH_tdst_Vector *src, register float mult)
{
	/*~~~~~~~~~~~~~*/
	Gector	*pS, *pD;
	/*~~~~~~~~~~~~~*/

	pS = (Gector *) src;
	pD = (Gector *) dst;

	*pD = *pS * mult;
	dst->z = src->z * mult;
}

#define MATH_MulVector(a, b, c)				MATH_ScaleVector(a, b, c)
#define MATH_ScaleEqualVector(A, fFactor)	MATH_ScaleVector(A, A, fFactor)
#define MATH_MulEqualVector(A, fFactor)		MATH_ScaleVector(A, A, fFactor)

#define MATH_DivVector(Dest, A, f)			MATH_ScaleVector(Dest, A, fOptInv(f))
#define MATH_DivEqualVector(A, f)			MATH_ScaleVector(A, A, fOptInv(f))

/*
 =======================================================================================================================
    Aim:    Dot product between two vectors
 =======================================================================================================================
 */
inline float MATH_f_DotProduct(register MATH_tdst_Vector *A, register MATH_tdst_Vector *B)
{
	/*~~~~~~*/
	float	f;
	/*~~~~~~*/

	f = 0.0f;

	f = f + A->x * B->x;
	f = f + A->y * B->y;
	f = f + A->z * B->z;
	return f;
}

/*
 =======================================================================================================================
    Aim:    Cross product between two vectors

    Time:   K6/2-333 50 clocks
 =======================================================================================================================
 */
inline void MATH_CrossProduct
(
	register MATH_tdst_Vector	*dst,
	const register MATH_tdst_Vector	*vec1,
	const register MATH_tdst_Vector	*vec2
)
{
#ifdef _C_MATH
	dst->x = fMul(vec1->y, vec2->z) - fMul(vec1->z, vec2->y);
	dst->y = fMul(vec1->z, vec2->x) - fMul(vec1->x, vec2->z);
	dst->z = fMul(vec1->x, vec2->y) - fMul(vec1->y, vec2->x);
#else
    register float ff0, ff1, ff2, ff3;
    register float ff4, ff5, ff6, ff7;
    register float ff8, ff9, ff10;

	asm
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* BX | BY */
		psq_l		ff1, 0(vec2), 0, 0;

		/* AZ | AZ */
		lfs			ff2, 8(vec1);

		/* AX | AY */
		psq_l		ff0, 0(vec1), 0, 0;

		/* BY | BX */
		ps_merge10	ff6, ff1, ff1;

		/* BZ | BZ */
		lfs			ff3, 8(vec2);

		/* BX*AZ | BY*AZ */
		ps_mul		ff4, ff1, ff2;

		/* BX*AX | BY*AX */
		ps_muls0	ff7, ff1, ff0;

		/* AX*BZ-BX*AZ | AY*BZ-BY*AZ */
		ps_msub		ff5, ff0, ff3, ff4;

		/* AX*BY-BX*AX | AY*BX-BY*AX */
		ps_msub		ff8, ff0, ff6, ff7;

		/* AY*BZ-AZ*BY | AY*BZ-AZ*BY */
		ps_merge11	ff9, ff5, ff5;

		/* AX*BZ-AZ*BX | AY*BX-AX*BY */
		ps_merge01	ff10, ff5, ff8;
		psq_st		ff9, 0(dst), 1, 0;

		/* AZ*BX-AX*BZ | AX*BY-AY*BX */
		ps_neg		ff10, ff10;
		psq_st		ff10, 4(dst), 0, 0;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}

#endif
}

/*
 =======================================================================================================================
    Aim:    Returns the square of the norm of a vector

    Time:   Not tested
 =======================================================================================================================
 */
inline float MATH_f_SqrNormVector(register MATH_tdst_Vector *vec1)
{
#ifdef _C_MATH
	return(vec1->x * vec1->x + vec1->y * vec1->y + vec1->z * vec1->z);

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#else
	register float	vxy, vzz, sqmag;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	asm
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* load X | Y */
		psq_l	vxy, 0(vec1), 0, 0;

		/* XX | YY */
		ps_mul	vxy, vxy, vxy;

		/* load Z | Z */
		lfs		vzz, 8(vec1);

		/* XX + ZZ | YY + ZZ */
		ps_madd sqmag, vzz, vzz, vxy;
		ps_sum0 sqmag, sqmag, vxy, vxy;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}

	return sqmag;
#endif
}

/*
 =======================================================================================================================
    Aim:    Returns the norm of a vector
 =======================================================================================================================
 */
inline float MATH_f_NormVector(register MATH_tdst_Vector *v)
{
#ifdef _C_MATH
	return(fSqrt(MATH_f_SqrNormVector(v)));

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#else
	register f32	vxy, vzz, sqmag, mag;
	register f32	rmag, nwork0, nwork1;
	register f32	c_three, c_half;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	asm
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Square mag calculation */
		psq_l	vxy, 0(v), 0, 0;
		ps_mul	vxy, vxy, vxy;
		lfs		vzz, 8(v);
		ps_madd sqmag, vzz, vzz, vxy;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}

	c_half = 0.5F;

	asm
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Square mag */
		ps_sum0 sqmag, sqmag, vxy, vxy;

		/* 1.0/sqrt : estimation[E] */
		frsqrte rmag, sqmag;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}

	c_three = 3.0F;

	asm
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Refinement x 1 : E' = (E/2)(3 - X*E*E) */
		fmuls	nwork0, rmag, rmag;
		fmuls	nwork1, rmag, c_half;
		fnmsubs nwork0, nwork0, sqmag, c_three;
		fmuls	rmag, nwork0, nwork1;

		/* NaN check (if sqmag == 0 ) */
		fsel	rmag, rmag, rmag, sqmag;

		/* 1/sqrt(X) * X = sqrt(X) */
		fmuls	mag, sqmag, rmag;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}

	return mag;
#endif
}

/*
 =======================================================================================================================
    Aim:    Returns the distance between two points

    Time:   Not tested
 =======================================================================================================================
 */
inline float MATH_f_Distance(register MATH_tdst_Vector *a, register MATH_tdst_Vector *b)
{
	/*~~~~~~~~~~~~~~~~~~*/
#ifdef _C_MATH
	MATH_tdst_Vector	V;
	/*~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&V, a, b);
	return(MATH_f_NormVector(&V));

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#else
	register f32	v0yz, v1yz, v0xy, v1xy;
	register f32	dyz, dxy, sqdist, rdist, dist;
	register f32	nwork0, nwork1;
	register f32	c_half, c_three;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	asm
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		psq_l	v0yz, 4(a), 0, 0;			/* [Y0][Z0] */
		psq_l	v1yz, 4(b), 0, 0;			/* [Y1][Z1] */
		ps_sub	dyz, v0yz, v1yz;			/* [Y0-Y1][Z0-Z1] */

		psq_l	v0xy, 0(a), 0, 0;			/* [X0][Y0] */
		psq_l	v1xy, 0(b), 0, 0;			/* [X1][Y1] */
		ps_mul	dyz, dyz, dyz;				/* [dYdY][dZdZ] */
		ps_sub	dxy, v0xy, v1xy;			/* [X0-X1][Y0-Y1] */
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
	}

	c_half = 0.5F;

	asm
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ps_madd sqdist, dxy, dxy, dyz;		/* [dXdX+dYdY][dYdY+dZdZ] */
		ps_sum0 sqdist, sqdist, dyz, dyz;	/* [dXdX+dYdY+dZdZ][N/A] */
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}

	c_three = 3.0F;

	asm
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* 1.0/sqrt : estimation[E] */
		frsqrte rdist, sqdist;

		/* Refinement x 1 : E' = (E/2)(3 - X*E*E) */
		fmuls	nwork0, rdist, rdist;
		fmuls	nwork1, rdist, c_half;
		fnmsubs nwork0, nwork0, sqdist, c_three;
		fmuls	rdist, nwork0, nwork1;

		/* NaN check (if sqmag == 0 ) */
		fsel	rdist, rdist, rdist, sqdist;

		/* 1/sqrt(X) * X = sqrt(X) */
		fmuls	dist, sqdist, rdist;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}

	return dist;
#endif
}

/* Aim: Return the square of the vector */
#define MATH_f_SqrVector	MATH_f_SqrNormVector

/*
 =======================================================================================================================
    Aim:    Returns the invert of the norm of a vector
 =======================================================================================================================
 */
inline float MATH_f_InvNormVector(MATH_tdst_Vector *A)
{
	return fOptInvSqrt(MATH_f_SqrNormVector(A));
}

/*
 =======================================================================================================================
    Aim:    Normalize a non nul vector
 =======================================================================================================================
 */
inline void MATH_NormalizeVector(register MATH_tdst_Vector *dst, const register MATH_tdst_Vector *vec1)
{
	/*~~~~~~~~~~~~~*/
#ifdef _C_MATH
	float	fInvNorm;
	/*~~~~~~~~~~~~~*/

	fInvNorm = MATH_f_InvNormVector(vec1);
	dst->x = fMul(vec1->x, fInvNorm);
	dst->y = fMul(vec1->y, fInvNorm);
	dst->z = fMul(vec1->z, fInvNorm);

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#else
	
	register f32	c_half;
	register f32	c_three;
	register f32	v1_xy, v1_z;
	register f32	xx_zz, xx_yy;
	register f32	sqsum;
	register f32	rsqrt;
	register f32	nwork0, nwork1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	c_half = 0.5F;
	c_three = 3.0F;

	if(MATH_b_NulVector(vec1)) 
	{
		return;
	}
	
	asm
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* X | Y */
		psq_l		v1_xy, 0(vec1), 0, 0;

		/* X*X | Y*Y */
		ps_mul		xx_yy, v1_xy, v1_xy;

		/* Z | 1 */
		psq_l		v1_z, 8(vec1), 1, 0;

		/* X*X+Z*Z | Y*Y+1 */
		ps_madd		xx_zz, v1_z, v1_z, xx_yy;

		/* X*X+Z*Z+Y*Y | Z */
		ps_sum0		sqsum, xx_zz, v1_z, xx_yy;

		/* 1.0/sqrt : estimation[E] */
		frsqrte		rsqrt, sqsum;

		/* Newton's refinement x 1 E' = (E/2)(3 - sqsum * E * E) */
		fmuls		nwork0, rsqrt, rsqrt;
		fmuls		nwork1, rsqrt, c_half;
		fnmsubs		nwork0, nwork0, sqsum, c_three;
		fmuls		rsqrt, nwork0, nwork1;

		/* X * mag | Y * mag */
		ps_muls0	v1_xy, v1_xy, rsqrt;
		psq_st		v1_xy, 0(dst), 0, 0;

		/* Z * mag */
		ps_muls0	v1_z, v1_z, rsqrt;
		psq_st		v1_z, 8(dst), 1, 0;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}
	
#ifdef _DEBUG
	{
		static BOOL bNaNErrorAlreadyDisplayed = FALSE;
		if (!bNaNErrorAlreadyDisplayed && (
				((*(int *) &dst->x) == (int) 0xFFC00000)
			||  ((*(int *) &dst->y) == (int) 0xFFC00000)
			||  ((*(int *) &dst->z) == (int) 0xFFC00000)
			||	((*(int *) &dst->x) == (int) 0x7FC00000)
			||  ((*(int *) &dst->y) == (int) 0x7FC00000)
			||  ((*(int *) &dst->z) == (int) 0x7FC00000)))
		{
			bNaNErrorAlreadyDisplayed = TRUE;
			OSReport("[NORMALIZE] Vector return NaN (error displayed only once)\n");
		}
	}
#endif

#endif
}

inline void MATH_NormalizeEqualVector(register MATH_tdst_Vector *dst)
{
	MATH_NormalizeVector(dst, dst);
}
/*
 =======================================================================================================================
    Aim:    Normalize and round a non nul vector (to avoid precision drift)
 =======================================================================================================================
 */
inline void MATH_NormalizeAndRoundVector(MATH_tdst_Vector *_pst_DstVector, MATH_tdst_Vector *_pst_SrcVector)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	float	f10KInvNorm;
	LONG	l_Sx, l_Sy, l_Sz;	/* Coordinates converted to LONG */
	/*~~~~~~~~~~~~~~~~~~~~~*/

	f10KInvNorm = MATH_f_InvNormVector(_pst_SrcVector) * Cf_10K;
	l_Sx = lFloatToLong(fMul(_pst_SrcVector->x, f10KInvNorm));
	l_Sy = lFloatToLong(fMul(_pst_SrcVector->y, f10KInvNorm));
	l_Sz = lFloatToLong(fMul(_pst_SrcVector->z, f10KInvNorm));
	_pst_DstVector->x = fLongToFloat(l_Sx) * Cf_EpsilonBig;
	_pst_DstVector->y = fLongToFloat(l_Sy) * Cf_EpsilonBig;
	_pst_DstVector->z = fLongToFloat(l_Sz) * Cf_EpsilonBig;
}

/*
 =======================================================================================================================
    Aim:    Set the norm of a vector
 =======================================================================================================================
 */
inline void MATH_SetNormVector(MATH_tdst_Vector *Dest, const MATH_tdst_Vector *Src, const float _f_Norm)
{
	MATH_NormalizeVector(Dest, Src);
	MATH_MulEqualVector(Dest, _f_Norm);
}

/*
 =======================================================================================================================
    Aim:    Round a vector with a precision of 0.0001
 =======================================================================================================================
 */
inline void MATH_RoundVector(MATH_tdst_Vector *_pst_Vector)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	LONG	l_Sx, l_Sy, l_Sz;	/* Coordinates converted to LONG */
	/*~~~~~~~~~~~~~~~~~~~~~*/

	l_Sx = lFloatToLong(_pst_Vector->x * Cf_10K);
	l_Sy = lFloatToLong(_pst_Vector->y * Cf_10K);
	l_Sz = lFloatToLong(_pst_Vector->z * Cf_10K);
	_pst_Vector->x = fLongToFloat(l_Sx) * Cf_EpsilonBig;
	_pst_Vector->y = fLongToFloat(l_Sy) * Cf_EpsilonBig;
	_pst_Vector->z = fLongToFloat(l_Sz) * Cf_EpsilonBig;
}

/*
 =======================================================================================================================
    Aim:    Round a vector with a given precision
 =======================================================================================================================
 */
inline void MATH_RoundVectorWithPrecision(MATH_tdst_Vector *_pst_Vector, const float f_precision)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	l_Sx, l_Sy, l_Sz;	/* Coordinates converted to LONG */
	float	f_InvPrec;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	f_InvPrec = (fInv(f_precision));

	l_Sx = lFloatToLong(_pst_Vector->x * f_InvPrec);
	l_Sy = lFloatToLong(_pst_Vector->y * f_InvPrec);
	l_Sz = lFloatToLong(_pst_Vector->z * f_InvPrec);
	_pst_Vector->x = fLongToFloat(l_Sx) * f_precision;
	_pst_Vector->y = fLongToFloat(l_Sy) * f_precision;
	_pst_Vector->z = fLongToFloat(l_Sz) * f_precision;
}

/*$4
 ***********************************************************************************************************************
    VECTOR TESTS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Tests if two vectors are exactly identical
 =======================================================================================================================
 */
inline char MATH_b_EqVector(const MATH_tdst_Vector *A, const MATH_tdst_Vector *B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	stTmp;

	LONG				*pA;
	LONG				*pB;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pA = (LONG *) A;
	pB = (LONG *) B;

	/* Pour assurer l'egalité */
	MATH_SubVector(&stTmp, A, B);
	return(MATH_b_NulVector(&stTmp));
}

/*
 =======================================================================================================================
    Aim:    Tests if two vectors are exactly identical, with memory comparison
 =======================================================================================================================
 */
inline char MATH_b_EqVector2(const MATH_tdst_Vector *A, const MATH_tdst_Vector *B)
{
	/*~~~~~~~~*/
	LONG	*pA;
	LONG	*pB;
	/*~~~~~~~~*/

	pA = (LONG *) A;
	pB = (LONG *) B;

	return((*pA == *pB) && (*(pA + 1) == *(pB + 1)) && (*(pA + 2) == *(pB + 2)));
}

/*
 =======================================================================================================================
    Aim:    Tests if two vectors are identical (with an epsilon)
 =======================================================================================================================
 */
inline int MATH_b_EqVectorWithEpsilon(const MATH_tdst_Vector *A, const MATH_tdst_Vector *B, float fEps)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	stTmp;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&stTmp, A, B);
	return(MATH_b_NulVectorWithEpsilon(&stTmp, fEps));
}

/*
 =======================================================================================================================
    Aim:    Returns true if 2 vectors are colinear
 =======================================================================================================================
 */
inline char MATH_b_VecColinear(const MATH_tdst_Vector *A, const MATH_tdst_Vector *B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_TmpVec;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CrossProduct(&st_TmpVec, A, B);
	return(MATH_b_NulVectorWithEpsilon(&st_TmpVec, Cf_Epsilon));
}

/*$4
 ***********************************************************************************************************************
    ADVANCED VECTOR OPERATIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Normalize a vector, even if nul
 =======================================================================================================================
 */
inline void MATH_NormalizeAnyVector(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A)
{
	if(MATH_b_NulVector(A))
		MATH_InitVectorToZero(Dest);
	else
		MATH_NormalizeVector(Dest, A);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MATH_NormalizeEqualAnyVector(MATH_tdst_Vector *_pst_V)
{
	if(!MATH_b_NulVector(_pst_V)) MATH_NormalizeEqualVector(_pst_V);
}

/*
 =======================================================================================================================
    Aim:    Calculates the distance from point "_pst_Point" to line defined by the two points "_pst_LineA" and
            "_pst_LineB"
 =======================================================================================================================
 */
inline float MATH_f_PointLineDistance
(
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_LineA,
	MATH_tdst_Vector	*_pst_LineB
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_u;
	MATH_tdst_Vector	st_AM;
	MATH_tdst_Vector	st_Tmp;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_u, _pst_LineB, _pst_LineA);
	MATH_SubVector(&st_AM, _pst_Point, _pst_LineA);
	MATH_NormalizeEqualVector(&st_u);
	MATH_CrossProduct(&st_Tmp, &st_u, &st_AM);
	return(MATH_f_NormVector(&st_Tmp));
}

/*
 =======================================================================================================================
    Aim:    Calculates the coordinates of the projection "_pst_Proj" of the point "_pst_Point" on the line defined by
            the two points "_pst_LineA" and "_pst_LineB"
 =======================================================================================================================
 */
inline void MATH_PointLineProjection
(
	MATH_tdst_Vector	*_pst_Proj,
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_LineA,
	MATH_tdst_Vector	*_pst_LineB
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_u;
	MATH_tdst_Vector	st_AM;
	float				f_AH;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_u, _pst_LineB, _pst_LineA);
	MATH_SubVector(&st_AM, _pst_Point, _pst_LineA);
	MATH_NormalizeEqualVector(&st_u);
	f_AH = MATH_f_DotProduct(&st_u, &st_AM);
	MATH_MulEqualVector(&st_u, f_AH);
	MATH_AddVector(_pst_Proj, &st_u, _pst_LineA);
}

extern void MATH_PointParabolProjection2D(MATH_tdst_Vector *, MATH_tdst_Vector *, float a, float b);
extern void MATH_PointParabolProjection3D
			(
				MATH_tdst_Vector *, /* Projection global coordinates */
				MATH_tdst_Vector *, /* Tangent global coordinates */
				MATH_tdst_Vector *, /* Point global coordinates (before projection) */
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				float _f_r
			);
extern void MATH_PointSegmentProjection
			(
				MATH_tdst_Vector *, /* Projection global coordinates */
				MATH_tdst_Vector *, /* Point global coordinates (before projection) */
				MATH_tdst_Vector *,
				MATH_tdst_Vector *
			);

/*
 =======================================================================================================================
    Aim:    Calculates the cos between two vectors
 =======================================================================================================================
 */
inline float MATH_f_VecCos(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_uV1;
	MATH_tdst_Vector	st_uV2;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_NormalizeVector(&st_uV1, _pst_V1);
	MATH_NormalizeVector(&st_uV2, _pst_V2);
	return(MATH_f_DotProduct(&st_uV1, &st_uV2));
}

/*
 =======================================================================================================================
    Aim:    Calculates the sin between two vectors
 =======================================================================================================================
 */
inline float MATH_f_VecSin(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2, MATH_tdst_Vector *_pst_Axis)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_CrossProduct;
	float				f_Sin;
	MATH_tdst_Vector	st_uV1;
	MATH_tdst_Vector	st_uV2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_NormalizeVector(&st_uV1, _pst_V1);
	MATH_NormalizeVector(&st_uV2, _pst_V2);

	MATH_CrossProduct(&st_CrossProduct, &st_uV1, &st_uV2);
	f_Sin = MATH_f_NormVector(&st_CrossProduct);

	/*
	 * If an axis is given, than we use it to calculate the sign of the sinus, else it
	 * is positive
	 */
	if(_pst_Axis != 0)
	{
		if(MATH_f_DotProduct(&st_CrossProduct, _pst_Axis) < 0.0f) vNeg(&f_Sin);
	}

	return(f_Sin);
}

/*
 =======================================================================================================================
    Aim:    Calculates the angle between two vectors of the space

    Note:   The axis defines the normal of the plan for the sign of the angle. If no axis is given (nul pointer), then
            the axis is calculated using the cross product Clocks: To be optimized, (too many normalizations are done)
 =======================================================================================================================
 */
inline float MATH_f_VecAngle(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2, MATH_tdst_Vector *_pst_Axis)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	f_Cos, f_Sin, f_Angle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Cos = MATH_f_VecCos(_pst_V1, _pst_V2);
	if(f_Cos >= 1.0f) return 0;
	if(f_Cos <= -1.0f)
		f_Angle = Cf_Pi;
	else
		f_Angle = fAcos(f_Cos);

	f_Sin = MATH_f_VecSin(_pst_V1, _pst_V2, _pst_Axis);
	if(f_Sin < 0) f_Angle = -f_Angle;

	return(f_Angle);
}

/*
 =======================================================================================================================
    Aim:    Calculates the absolute value of the angle between two vectors of the space

    Note:   Faster than MATH_f_VecAngle, no cross product to find the sign of the angle
 =======================================================================================================================
 */
inline float MATH_f_VecAbsAngle(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2)
{
	/*~~~~~~~~~~*/
	float	f_Cos;
	/*~~~~~~~~~~*/

	f_Cos = MATH_f_VecCos(_pst_V1, _pst_V2);
	if(f_Cos >= 1.0f) return 0;
	if(f_Cos <= -1.0f) return Cf_Pi;

	return fAcos(f_Cos);
}

/*
 =======================================================================================================================
    Aim:    Scale a vector and add another one

    Note:   Dst=A+B*f
 =======================================================================================================================
 */
inline void MATH_AddScaleVector(MATH_tdst_Vector *Dst, MATH_tdst_Vector *A, MATH_tdst_Vector *B, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tmp;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_ScaleVector(&st_Tmp, B, f);
	MATH_AddVector(Dst, A, &st_Tmp);
}

/*
 =======================================================================================================================
    Aim Scale a vector and subs the result to another one

    Note:   Makes:: Dst = A - f * B;
 =======================================================================================================================
 */
inline void MATH_SubScaleVector(MATH_tdst_Vector *Dst, MATH_tdst_Vector *A, MATH_tdst_Vector *B, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tmp;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_ScaleVector(&st_Tmp, B, f);
	MATH_SubVector(Dst, A, &st_Tmp);
}

/*
 =======================================================================================================================
    Aim:    Makes (A - B).(C - D)
 =======================================================================================================================
 */
inline float MATH_f_SubSubDotProduct(MATH_tdst_Vector *A, MATH_tdst_Vector *B, MATH_tdst_Vector *C, MATH_tdst_Vector *D)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tmp1;
	MATH_tdst_Vector	st_Tmp2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Tmp1, A, B);
	MATH_SubVector(&st_Tmp2, C, D);

	return(MATH_f_DotProduct(&st_Tmp1, &st_Tmp2));
}

/*
 =======================================================================================================================
    Aim:    Makes (A - B)^(C - D)
 =======================================================================================================================
 */
inline void MATH_f_SubSubCrossProduct
(
	MATH_tdst_Vector	*Dst,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	MATH_tdst_Vector	*C,
	MATH_tdst_Vector	*D
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Tmp1;
	MATH_tdst_Vector	st_Tmp2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Tmp1, A, B);
	MATH_SubVector(&st_Tmp2, C, D);

	MATH_CrossProduct(Dst, &st_Tmp1, &st_Tmp2);
}

/*
 =======================================================================================================================
    Aim:    Combine two vectors with the following formula: VDst = r * V1 + (1-r) * V2

    Note:   We use r * V1 + (1-r) * V2 = r * (V1 - V2) + V2, faster way for calculation
 =======================================================================================================================
 */
inline void MATH_VectorCombine
(
	MATH_tdst_Vector	*_pst_VDst,
	MATH_tdst_Vector	*_pst_V1,
	MATH_tdst_Vector	*_pst_V2,
	float				_f_r
)
{
	MATH_SubVector(_pst_VDst, _pst_V1, _pst_V2);
	MATH_AddScaleVector(_pst_VDst, _pst_V2, _pst_VDst, _f_r);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MATH_UncompressAbsoluteVector(MATH_tdst_Vector *Dst, MATH_tdst_CompressedVector *C)
{
	Dst->x = (C->x / 1024.0f);
	Dst->y = (C->y / 1024.0f);
	Dst->z = (C->z / 1024.0f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MATH_UncompressRelativeVector(MATH_tdst_Vector *Dst, MATH_tdst_CompressedVector *C)
{
	Dst->x += (C->x / 1024.0f);
	Dst->y += (C->y / 1024.0f);
	Dst->z += (C->z / 1024.0f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MATH_UltraUncompressXVector(MATH_tdst_Vector *Dst, MATH_tdst_UltraCompressedVector *C)
{
	Dst->x += (C->s / 1024.0f);
	Dst->y += (C->c1 / 1024.0f);
	Dst->z += (C->c2 / 1024.0f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MATH_UltraUncompressYVector(MATH_tdst_Vector *Dst, MATH_tdst_UltraCompressedVector *C)
{
	Dst->x += (C->c1 / 1024.0f);
	Dst->y += (C->s / 1024.0f);
	Dst->z += (C->c2 / 1024.0f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MATH_UltraUncompressZVector(MATH_tdst_Vector *Dst, MATH_tdst_UltraCompressedVector *C)
{
	Dst->x += (C->c1 / 1024.0f);
	Dst->y += (C->c2 / 1024.0f);
	Dst->z += (C->s / 1024.0f);
}

/*
 =======================================================================================================================
    Aim:    Multiplies each coordinate of a vector by the same coordinate of another vector
 =======================================================================================================================
 */
inline void MATH_MulTwoVectors(MATH_tdst_Vector *_pst_Dst, MATH_tdst_Vector *_pst_Src, MATH_tdst_Vector *_pst_Mul)
{
	_pst_Dst->x = fMul(_pst_Src->x, _pst_Mul->x);
	_pst_Dst->y = fMul(_pst_Src->y, _pst_Mul->y);
	_pst_Dst->z = fMul(_pst_Src->z, _pst_Mul->z);
}

/*
 =======================================================================================================================
    Aim:    Multiplies each coordinate of a vector by the same coordinate of another vector
 =======================================================================================================================
 */
inline void MATH_MulEqualTwoVectors(MATH_tdst_Vector *_pst_Dst, MATH_tdst_Vector *_pst_Mul)
{
	_pst_Dst->x = fMul(_pst_Dst->x, _pst_Mul->x);
	_pst_Dst->y = fMul(_pst_Dst->y, _pst_Mul->y);
	_pst_Dst->z = fMul(_pst_Dst->z, _pst_Mul->z);
}

/*
 =======================================================================================================================
    Aim:    Calculates the exponential of each coordinate of a vector
 =======================================================================================================================
 */
inline void MATH_ExpEqualVector(MATH_tdst_Vector *_pst_Dst)
{
	_pst_Dst->x = fExp(_pst_Dst->x);
	_pst_Dst->y = fExp(_pst_Dst->y);
	_pst_Dst->z = fExp(_pst_Dst->z);
}

/*
 =======================================================================================================================
    Aim:    Linear blends of two vector

    Note:   If factor == 0, destination is the vector 1
 =======================================================================================================================
 */
inline void MATH_BlendVector
(
	MATH_tdst_Vector	*_pst_Dest,
	MATH_tdst_Vector	*_pst_1,
	MATH_tdst_Vector	*_pst_2,
	float				_f_Factor
)
{
	/*~~~~~~~~*/
	float	f_b;
	/*~~~~~~~~*/

	if(_f_Factor < 0) _f_Factor = 0;
	if(_f_Factor > 1.0) _f_Factor = 1.0;
	f_b = Cf_One - _f_Factor;
	_pst_Dest->x = (_pst_2->x * _f_Factor) + (_pst_1->x * f_b);
	_pst_Dest->y = (_pst_2->y * _f_Factor) + (_pst_1->y * f_b);
	_pst_Dest->z = (_pst_2->z * _f_Factor) + (_pst_1->z * f_b);
}

#ifdef __cplusplus
}
#endif
#endif /* #ifndef __GXMATHVECTOR_H__ */


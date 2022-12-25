#define MATH_MulEqualVector_ALIGNED MATH_ScaleEqualVector_ALIGNED

_inline_ void MATH_SubVector_ALIGNED(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	asm __volatile__("
	lqc2    vf4,0x0(A)
	lqc2    vf5,0x0(B)
	vsub.xyzw vf6,vf4,vf5
	sqc2    vf6,0x0(VDst)");
}

_inline_ void MATH_AddVector_ALIGNED(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	asm __volatile__("
	lqc2    vf4,0x0(A)
	lqc2    vf5,0x0(B)
	vadd.xyzw vf6,vf4,vf5
	sqc2    vf6,0x0(VDst)");
}

_inline_ void MATH_AddScaleVector_ALIGNED(MATH_tdst_Vector *VDst,MATH_tdst_Vector *A, MATH_tdst_Vector *B, float fValue)
{
	register unsigned int  TEMP;
	asm __volatile__("
	lqc2    vf4,0x0(A)
	lqc2    vf5,0x0(B)
	mfc1    TEMP,fValue
	qmtc2    TEMP,vf10
	vmulx.xyzw	vf5,vf5,vf10
	vadd.xyzw vf6,vf4,vf5
	sqc2    vf6,0x0(VDst)");
}


_inline_ void MATH_CopyVector_ALIGNED(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A)
{
	register u_long128 Tmp1;
	asm __volatile__("
	lq    Tmp1,0x0(A)
	sq    Tmp1,0x0(VDst)");
}
_inline_ float MATH_f_DotProduct_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	register float fRet;
	register unsigned int Tmp1;
	asm __volatile__("
	lqc2    vf4,0x0(A)
	lqc2    vf5,0x0(B)
	vmul.xyzw vf6,vf4,vf5
	vaddz.x vf6,vf6,vf6
	vaddy.x vf6,vf6,vf6
	qmfc2	Tmp1 , vf6
	mtc1	Tmp1 , fRet");
    return	fRet;
}
_inline_ void MATH_NegVector_ALIGNED(MATH_tdst_Vector *_pst_VDst, MATH_tdst_Vector *_pst_VSrc)
{
	asm __volatile__("
	lqc2    vf4,0x0(_pst_VSrc)
	vsub.xyz vf4 , vf00 , vf4
	sqc2    vf4,0x0(_pst_VDst)");
}
_inline_ void MATH_AddEqualVector_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	asm __volatile__("
	lqc2    vf4,0x0(A)
	lqc2    vf5,0x0(B)
	vadd.xyzw vf6,vf4,vf5
	sqc2    vf6,0x0(A)");
}

_inline_ void MATH_SubEqualVector_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	asm __volatile__("
	lqc2    vf4,0x0(A)
	lqc2    vf5,0x0(B)
	vsub.xyzw vf6,vf4,vf5
	sqc2    vf6,0x0(A)");
}

_inline_ float MATH_f_NormVector_ALIGNED(MATH_tdst_Vector *_pst_V)
{
    return(fSqrt(MATH_f_DotProduct_ALIGNED(_pst_V , _pst_V)));
}


_inline_ void MATH_ScaleEqualVector_ALIGNED(MATH_tdst_Vector *A, float f)
{
	asm __volatile__("
	lqc2    vf4,0x0(%0)
	mfc1    $8,%1
	qmtc2    $8,vf5
	vmulx.xyz	vf6,vf4,vf5
	sqc2    vf6,0x0(%0)
	": : "r" (A) , "f" (f):"$8");
}

_inline_ void MATH_NormalizeVector_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A)
{
        asm __volatile__("
        lqc2    vf4,0x0(%1)
        vmul.xyz vf5,vf4,vf4
        vaddy.x vf5,vf5,vf5
        vaddz.x vf5,vf5,vf5

        vsqrt Q,vf5x 
        vwaitq
        vaddq.x vf5x,vf0x,Q
        vdiv    Q,vf0w,vf5x
        vsub.xyzw vf6,vf0,vf0           #vf6.xyzw=0;
        vwaitq

        vmulq.xyz  vf6,vf4,Q
        sqc2    vf6,0x0(%0)
        ": : "r" (Dest) , "r" (A));
}

_inline_ void MATH_AbsVector_ALIGNED(MATH_tdst_Vector *_pst_Mdst, MATH_tdst_Vector *_pst_Msrc)
{
	asm __volatile__("
	lqc2    	vf4,0x0(_pst_Msrc)
	vabs.xyz    vf4, vf4
	sqc2    	vf4,0x0(_pst_Mdst)
	");
}

#define MATH_SetNulVector_ALIGNED	MATH_InitVectorToZero_ALIGNED
_inline_ void MATH_InitVectorToZero_ALIGNED(MATH_tdst_Vector *VDst)
{
    asm __volatile__("
    sq  $0, 0(VDst)
    ");
}

#define MATH_InitVector_ALIGNED MATH_InitVector
#define MATH_SetVector_ALIGNED	MATH_InitVector
#define MATH_InvVector_ALIGNED  MATH_InvVector
#define MATH_InvEqualVector_ALIGNED MATH_InvEqualVector

#define MATH_MulVector_ALIGNED	MATH_ScaleVector_ALIGNED
_inline_ void MATH_ScaleVector_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, float f)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	mfc1    $8,%2
	qmtc2    $8,vf5
	vmulx.xyz	vf6,vf4,vf5
	sqc2    vf6,0x0(%0)
	": : "r" (Dest) , "r" (A), "f" (f):"$8");
}

_inline_ void MATH_DivVector_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, float f)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	mfc1    $8,%2
	qmtc2    $8,vf5
	vdiv    Q,vf0w,vf5x
	vwaitq
	vmulq.xyzw      vf4,vf4,Q
	sqc2    vf4,0x0(%0)
	": : "r" (Dest) , "r" (A), "f" (f):"$8");
}

_inline_ void MATH_DivEqualVector_ALIGNED(MATH_tdst_Vector *A, float f)
{
	asm __volatile__("
	lqc2    vf4,0x0(%0)
	mfc1    $8,%1
	qmtc2    $8,vf5
	vdiv    Q,vf0w,vf5x
	vwaitq
	vmulq.xyzw      vf4,vf4,Q
	sqc2    vf4,0x0(%0)
	": : "r" (A) , "f" (f):"$8");
}

_inline_ void MATH_CrossProduct_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	asm __volatile__
	("
	lqc2    vf4,0x0(%1)
	lqc2    vf5,0x0(%2)
	vopmula.xyz	ACC,vf4,vf5
	vopmsub.xyz	vf6,vf5,vf4
	vsub.w vf6,vf6,vf6		#vf6.xyz=0;
	sqc2    vf6,0x0(%0)
	": : "r" (Dest) , "r" (A) ,"r" (B));
}

_inline_ float MATH_f_InvNormVector_ALIGNED(MATH_tdst_Vector *A)
{
    register float ff;
    
        asm __volatile__("
        lqc2    vf4,0x0(%1)
        vmul.xyz vf5,vf4,vf4
        vaddy.x vf5,vf5,vf5
        vaddz.x vf5,vf5,vf5

        vsqrt Q,vf5x 
        vwaitq
        vaddq.x vf5x,vf0x,Q       
        vdiv    Q,vf0w,vf5x
        vwaitq
        vaddq.x vf5x,vf0x,Q       
        qmfc2.i   $8, vf5
        mtc1      $8, %0
        ": "=f"(ff) : "r" (A) : "$8");
        return ff;
}

_inline_ void MATH_NormalizeEqualVector_ALIGNED(MATH_tdst_Vector *_pst_V)
{
        asm __volatile__("
        lqc2    vf4,0x0(%0)
        vmul.xyz vf5,vf4,vf4
        vaddy.x vf5,vf5,vf5
        vaddz.x vf5,vf5,vf5

        vsqrt Q,vf5x 
        vwaitq
        vaddq.x vf5x,vf0x,Q
        vdiv    Q,vf0w,vf5x
        vsub.xyzw vf6,vf0,vf0           #vf6.xyzw=0;
        vwaitq

        vmulq.xyz  vf6,vf4,Q
        sqc2    vf6,0x0(%0)
        ": : "r" (_pst_V));
}

_inline_ void MATH_SetNormVector_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *Src, float _f_Norm)
{
        asm __volatile__("
    	mfc1    $8,%2
	    qmtc2    $8,vf6        
        lqc2    vf4,0x0(%1)
        vmul.xyz vf5,vf4,vf4
        vaddy.x vf5,vf5,vf5
        vaddz.x vf5,vf5,vf5

        vsqrt Q,vf5x 
        vwaitq
        vaddq.x vf5x,vf0x,Q
        vdiv    Q,vf6x,vf5x
        vsub.xyzw vf6,vf0,vf0           #vf6.xyzw=0;
        vwaitq

        vmulq.xyz  vf6,vf4,Q
        sqc2    vf6,0x0(%0)
        ": : "r" (Dest) , "r" (Src), "f"(_f_Norm));
}

_inline_ void MATH_RoundVector_ALIGNED(MATH_tdst_Vector *_pst_Vector)
{
	asm __volatile__("
	lqc2    vf4,0x0(%0)
	vftoi4.xyz	vf5,vf4
	vitof4.xyz	vf4,vf5
	sqc2    vf4,0x0(%0)
	": : "r" (_pst_Vector));
}

_inline_ void MATH_NormalizeAndRoundVector_ALIGNED(MATH_tdst_Vector *_pst_DstVector, MATH_tdst_Vector *_pst_SrcVector)
{
    MATH_NormalizeVector_ALIGNED(_pst_DstVector, _pst_SrcVector);
    MATH_RoundVector_ALIGNED(_pst_DstVector);
}

#define MATH_RoundVectorWithPrecision_ALIGNED   MATH_RoundVectorWithPrecision
#define MATH_b_NulVector_ALIGNED MATH_b_NulVector
#define MATH_b_NulVectorWithEpsilon_ALIGNED MATH_b_NulVectorWithEpsilon

#if 0
//AFAIRE
_inline_ char MATH_b_EqVector_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    register long128 t0, t1, t2, t3;
    
	asm __volatile__("lq      %0, 0(%1)" : "=r"(t0) : "r"(A));
	asm __volatile__("lq      %0, 0(%1)" : "=r"(t1) : "r"(B));
	asm __volatile__("psubw   %0, %1, %2" : "=r"(t2) : "r"(t0), "r"(t1));
	asm __volatile__("li      %0, 1" : "=r"(t1) :);
	asm __volatile__("dsll32  %0, %1, 0" : "=r"(t0) : "r"(t2));
	asm __volatile__("dsrl32  %0, %1, 0" : "=r"(t2) : "r"(t0));
	asm __volatile__("movz    %0, %1, $0" : "=r"(t1) : "r"(t2) : "$0");
	" : "=r"(
}
char MATH_b_EqVector2(MATH_tdst_Vector *A, MATH_tdst_Vector *B);
char MATH_b_EqVectorWithEpsilon(MATH_tdst_Vector *A, MATH_tdst_Vector *B, float fEps);


/*
 =======================================================================================================================
    Aim:    Returns true if 2 vectors are colinear
 =======================================================================================================================
 */
char MATH_b_VecColinear(MATH_tdst_Vector *A, MATH_tdst_Vector *B);


/*$4
 ***********************************************************************************************************************
    ADVANCED VECTOR OPERATIONS £
    £
    MATH_NormalizeAnyVector (vector*,vector*); £
    MATH_NormalizeEqualAnyVector (vector*); £
    MATH_VectorCombine(vector*,vector*,vector*,float); £
    MATH_f_PointLineDistance(vector*,vector*,vector*); £
    MATH_PointLineProjection(vector*,vector*,vector*,vector*); £
    MATH_PointSegmentProjection £
    MATH_PointParabolProjection2D £
    MATH_PointParabolProjection3D £
    MATH_f_VecCos(vector*,vector*); £
    MATH_f_VecSin(vector*,vector*,vector*); £
    MATH_f_VecAngle(vector*,vector*,vector*); £
    MATH_f_VecAbsAngle(vector*,vector*); £
    MATH_f_VectorAbsAngle £
    MATH_AddScaleVector(Dst, A, B, f); : Dst = A + f * B £
    MATH_SubScaleVector(Dst, A, B, f); : Dst = A - f * B £
    MATH_f_SubSubDotProduct(A, B, C, D); : (A - B); . (C - D); £
    MATH_f_SubSubCrossProduct(Dst, A, B, C, D); : Dst = (A - B); ^ (C - D); £
    MATH_MulTwoVectors(vector*,vector*,vector*); £
    MATH_MulEqualTwoVectors(vector* vector*); £
    MATH_ExpEqualVector (vector*); £
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Normalize a vector, even if nul

    Note:   If the vector is nul, a nul vector is returned
 =======================================================================================================================
 */
void MATH_NormalizeAnyVector(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A);


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_NormalizeEqualAnyVector(MATH_tdst_Vector *_pst_V);


/*
 =======================================================================================================================
    Aim:    Calculates the distance from point "_pst_Point" to line defined by the two points "_pst_LineA" and
            "_pst_LineB"
 =======================================================================================================================
 */
float MATH_f_PointLineDistance
(
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_LineA,
	MATH_tdst_Vector	*_pst_LineB
);


/*
 =======================================================================================================================
    Aim:    Calculates the coordinates of the projection "_pst_Proj" of the point "_pst_Point" on the line defined by
            the two points "_pst_LineA" and "_pst_LineB"
 =======================================================================================================================
 */
void MATH_PointLineProjection
(
	MATH_tdst_Vector	*_pst_Proj,
	MATH_tdst_Vector	*_pst_Point,
	MATH_tdst_Vector	*_pst_LineA,
	MATH_tdst_Vector	*_pst_LineB
);


extern void MATH_PointParabolProjection2D(MATH_tdst_Vector *, MATH_tdst_Vector *, float a, float b);
extern void MATH_PointParabolProjection3D
			(
				MATH_tdst_Vector *, /* Projection global coordinates */
				MATH_tdst_Vector *, /* Tangent global coordinates */
				MATH_tdst_Vector *, /* Point global coordinates (before projection); */
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				float _f_r
			);
extern void MATH_PointSegmentProjection
			(
				MATH_tdst_Vector *, /* Projection global coordinates */
				MATH_tdst_Vector *, /* Point global coordinates (before projection); */
				MATH_tdst_Vector *,
				MATH_tdst_Vector *
			);

/*
 =======================================================================================================================
    Aim:    Calculates the cos between two vectors
 =======================================================================================================================
 */
float MATH_f_VecCos(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2);


/*
 =======================================================================================================================
    Aim:    Calculates the sin between two vectors
 =======================================================================================================================
 */
float MATH_f_VecSin(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2, MATH_tdst_Vector *_pst_Axis);


/*
 =======================================================================================================================
    Aim:    Calculates the angle between two vectors of the space

    Note:   The axis defines the normal of the plan for the sign of the angle. If no axis is given (nul pointer);, then
            the axis is calculated using the cross product Clocks: To be optimized, (too many normalizations are done);
 =======================================================================================================================
 */
float MATH_f_VecAngle(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2, MATH_tdst_Vector *_pst_Axis);

/*
 =======================================================================================================================
    Aim:    Calculates the absolute value of the angle between two vectors of the space

    Note:   Faster than MATH_f_VecAngle, no cross product to find the sign of the angle
 =======================================================================================================================
 */
float MATH_f_VecAbsAngle(MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2);


/*
 =======================================================================================================================
    Aim:    Scale a vector and add another one

    Note:   Dst=A+B*f
 =======================================================================================================================
 */
void MATH_AddScaleVector(MATH_tdst_Vector *Dst, MATH_tdst_Vector *A, MATH_tdst_Vector *B, float f);


/*
 =======================================================================================================================
    Aim Scale a vector and subs the result to another one

    Note:   Makes:: Dst = A - f * B;
 =======================================================================================================================
 */
void MATH_SubScaleVector(MATH_tdst_Vector *Dst, MATH_tdst_Vector *A, MATH_tdst_Vector *B, float f);


/*
 =======================================================================================================================
    Aim:    Makes (A - B);.(C - D);
 =======================================================================================================================
 */
float MATH_f_SubSubDotProduct
(
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	MATH_tdst_Vector	*C,
	MATH_tdst_Vector	*D
);


/*
 =======================================================================================================================
    Aim:    Makes (A - B);^(C - D);
 =======================================================================================================================
 */
void MATH_f_SubSubCrossProduct
(
	MATH_tdst_Vector	*Dst,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	MATH_tdst_Vector	*C,
	MATH_tdst_Vector	*D
);

/*
 =======================================================================================================================
    Aim:    Combine two vectors with the following formula: VDst = r * V1 + (1-r); * V2

    Note:   We use r * V1 + (1-r); * V2 = r * (V1 - V2); + V2, faster way for calculation
 =======================================================================================================================
 */
void MATH_VectorCombine
(
	MATH_tdst_Vector	*_pst_VDst,
	MATH_tdst_Vector	*_pst_V1,
	MATH_tdst_Vector	*_pst_V2,
	float				_f_r
);


/*
 =======================================================================================================================
    Aim:    Linear blends of two vector

    Note:   If factor == 0, destination is the vector 1
 =======================================================================================================================
 */
void MATH_BlendVector
(
	MATH_tdst_Vector	*_pst_Dest,
	MATH_tdst_Vector	*_pst_1,
	MATH_tdst_Vector	*_pst_2,
	float				_f_Factor
);

/*
 ===================================================================================================
    Aim:    Take the absolute value for each coordinate of a vector
 ===================================================================================================
 */
_inline_ void MATH_AbsVector(MATH_tdst_Vector *_pst_Mdst, MATH_tdst_Vector *_pst_Msrc)
{
    _pst_Mdst->x = fAbs(_pst_Msrc->x);
    _pst_Mdst->y = fAbs(_pst_Msrc->y);
    _pst_Mdst->z = fAbs(_pst_Msrc->z);
}

/*
 ===================================================================================================
    Aim:    Initialize a vector to 0
 ===================================================================================================
 */
_inline_ void MATH_InitVectorToZero(MATH_tdst_Vector *VDst)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *Dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Dst = (LONG *) VDst;

    Dst[2] = Dst[1] = *Dst = Cl_0f;
}

#define MATH_SetNulVector   MATH_InitVectorToZero

/*
 ===================================================================================================
    Aim:    Initialize a vector with 3 floats using values for the floats

    Time:   K6/2-333 15 clocks
 ===================================================================================================
 */
_inline_ void MATH_InitVector(MATH_tdst_Vector *VDst, float x, float y, float z)
{
    VDst->x = x;
    VDst->y = y;
    VDst->z = z;
}

#define MATH_SetVector  MATH_InitVector

/*
 ===================================================================================================
    Aim:    Add two vectors

    Time:   K6/2-333 26 clocks

    Note:   Makes Vdest=A+B;
 ===================================================================================================
 */
_inline_ void MATH_AddVector(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    VDst->x = fAdd(A->x, B->x);
    VDst->y = fAdd(A->y, B->y);
    VDst->z = fAdd(A->z, B->z);
}

/*
 ===================================================================================================
    Aim:    Add two vectors

    Note:   Makes A+=B
 ===================================================================================================
 */
_inline_ void MATH_AddEqualVector(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    A->x += B->x;
    A->y += B->y;
    A->z += B->z;
}

/*
 ===================================================================================================
    Aim:    Substract two vectors

    Time:   K6/2-333 26 clocks

    Note:   Vdst = A-B
 ===================================================================================================
 */
_inline_ void MATH_SubVector(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    VDst->x = fSub(A->x, B->x);
    VDst->y = fSub(A->y, B->y);
    VDst->z = fSub(A->z, B->z);
}

/*
 ===================================================================================================
    Aim:    Sub two vectors

    Note:   Makes A-=B
 ===================================================================================================
 */
_inline_ void MATH_SubEqualVector(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    A->x -= B->x;
    A->y -= B->y;
    A->z -= B->z;
}

/*
 ===================================================================================================
    Aim:    Copy a vector

    Time:   K6/2-333 9 clocks
 ===================================================================================================
 */
_inline_ void MATH_CopyVector(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *Src;
    LONG    *Dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Dst = (LONG *) VDst;
    Src = (LONG *) A;

    *Dst = *Src;
    *(Dst + 1) = *(Src + 1);
    *(Dst + 2) = *(Src + 2);
}

/*
 ===================================================================================================
    Aim:    Negate a vector

    Time:   K6/2-333 6 clocks

    Note:   Warning, with this function the vector is directly negated.
 ===================================================================================================
 */
_inline_ void MATH_NegEqualVector(MATH_tdst_Vector *_pst_Vector)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *Src;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Src = (LONG *) _pst_Vector;

    *Src += 0x80000000;
    *(Src + 1) += 0x80000000;
    *(Src + 2) += 0x80000000;
}

/*
 ===================================================================================================
    Aim:    Negate a vector

    Time:   K6/2-333 6 clocks

    Note:   Warning, with this function the vector is directly negated.
 ===================================================================================================
 */
_inline_ void MATH_NegVector(MATH_tdst_Vector *_pst_VDst, MATH_tdst_Vector *_pst_VSrc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    *Src, *Dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Src = (LONG *) _pst_VSrc;
    Dst = (LONG *) _pst_VDst;

    *Dst = *Src + 0x80000000;
    *(Dst + 1) = *(Src + 1) + 0x80000000;
    *(Dst + 2) = *(Src + 2) + 0x80000000;
}


/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_OptInvEqualVector(MATH_tdst_Vector *pst_Vector)
{
    vOptInv(&(pst_Vector->x));
    vOptInv(&(pst_Vector->y));
    vOptInv(&(pst_Vector->z));
}



/*
 ===================================================================================================
    Aim:    Dot product between two vectors

    Time:   K6/2-333 26 clocks
 ===================================================================================================
 */
_inline_ float MATH_f_DotProduct(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    return((A->x) * (B->x) + (A->y) * (B->y) + (A->z) * (B->z));
}



/*
 ===================================================================================================
    Aim:    Returns the square of the norm of a vector

    Time:   Not tested
 ===================================================================================================
 */
_inline_ float MATH_f_SqrNormVector(MATH_tdst_Vector *A)
{
    return(A->x * A->x + A->y * A->y + A->z * A->z);
}

/*
 ===================================================================================================
    Aim:    Returns the norm of a vector
 ===================================================================================================
 */
_inline_ float MATH_f_NormVector(MATH_tdst_Vector *_pst_V)
{
    return(fSqrt(MATH_f_SqrNormVector(_pst_V)));
}

/*
 ===================================================================================================
    Aim:    Returns the distance between two points

    Time:   Not tested
 ===================================================================================================
 */
_inline_ float MATH_f_Distance(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    V;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_SubVector(&V, A, B);
    return(MATH_f_NormVector(&V));
}

/*
 ===================================================================================================
    Aim:    Return the square of the vector

    Time:   Not tested
 ===================================================================================================
 */
_inline_ float MATH_f_SqrVector(MATH_tdst_Vector *A)
{
    return(fSqr(A->x) + fSqr(A->y) + fSqr(A->z));
}



/*
 ===================================================================================================
    Aim:    Normalize a vector, even if nul

    Note:   If the vector is nul, a nul vector is returned
 ===================================================================================================
 */
_inline_ void MATH_NormalizeAnyVector(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A)
{
    if(MATH_b_NulVector(A))
        MATH_InitVectorToZero(Dest);
    else
        MATH_NormalizeVector(Dest, A);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_NormalizeEqualAnyVector(MATH_tdst_Vector *_pst_V)
{
    if(!MATH_b_NulVector(_pst_V))
        MATH_NormalizeEqualVector(_pst_V);
}




/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UncompressAbsoluteVector(MATH_tdst_Vector *Dst, MATH_tdst_CompressedVector *C)
{
    Dst->x = (C->x / 1024.0f);
    Dst->y = (C->y / 1024.0f);
    Dst->z = (C->z / 1024.0f);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UncompressRelativeVector(MATH_tdst_Vector *Dst, MATH_tdst_CompressedVector *C)
{
    Dst->x += (C->x / 1024.0f);
    Dst->y += (C->y / 1024.0f);
    Dst->z += (C->z / 1024.0f);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UltraUncompressXVector(MATH_tdst_Vector *Dst, MATH_tdst_UltraCompressedVector *C)
{
    Dst->x += (C->s / 1024.0f);
    Dst->y += (C->c1 / 1024.0f);
    Dst->z += (C->c2 / 1024.0f);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UltraUncompressYVector(MATH_tdst_Vector *Dst, MATH_tdst_UltraCompressedVector *C)
{
    Dst->x += (C->c1 / 1024.0f);
    Dst->y += (C->s / 1024.0f);
    Dst->z += (C->c2 / 1024.0f);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void MATH_UltraUncompressZVector(MATH_tdst_Vector *Dst, MATH_tdst_UltraCompressedVector *C)
{
    Dst->x += (C->c1 / 1024.0f);
    Dst->y += (C->c2 / 1024.0f);
    Dst->z += (C->s / 1024.0f);
}

/*
 ===================================================================================================
    Aim:    Multiplies each coordinate of a vector by the same coordinate of another vector
 ===================================================================================================
 */
_inline_ void MATH_MulTwoVectors
(
    MATH_tdst_Vector    *_pst_Dst,
    MATH_tdst_Vector    *_pst_Src,
    MATH_tdst_Vector    *_pst_Mul
)
{
    _pst_Dst->x = fMul(_pst_Src->x, _pst_Mul->x);
    _pst_Dst->y = fMul(_pst_Src->y, _pst_Mul->y);
    _pst_Dst->z = fMul(_pst_Src->z, _pst_Mul->z);
}

/*
 ===================================================================================================
    Aim:    Multiplies each coordinate of a vector by the same coordinate of another vector
 ===================================================================================================
 */
_inline_ void MATH_MulEqualTwoVectors(MATH_tdst_Vector *_pst_Dst, MATH_tdst_Vector *_pst_Mul)
{
    _pst_Dst->x = fMul(_pst_Dst->x, _pst_Mul->x);
    _pst_Dst->y = fMul(_pst_Dst->y, _pst_Mul->y);
    _pst_Dst->z = fMul(_pst_Dst->z, _pst_Mul->z);
}

/*
 ===================================================================================================
    Aim:    Calculates the exponential of each coordinate of a vector
 ===================================================================================================
 */
_inline_ void MATH_ExpEqualVector(MATH_tdst_Vector *_pst_Dst)
{
    _pst_Dst->x = fExp(_pst_Dst->x);
    _pst_Dst->y = fExp(_pst_Dst->y);
    _pst_Dst->z = fExp(_pst_Dst->z);
}





_inline_ void MATH_AbsVector_ALIGNED(MATH_tdst_Vector *_pst_Mdst, MATH_tdst_Vector *_pst_Msrc)
{
	MTH_TestAlign16(_pst_Mdst);
	MTH_TestAlign16(_pst_Msrc);
    	MATH_AbsVector(_pst_Mdst, _pst_Msrc);
}
_inline_ void MATH_InitVectorToZero_ALIGNED(MATH_tdst_Vector *VDst)
{
	MTH_TestAlign16(VDst);
	MATH_InitVectorToZero(VDst);
}
#define MATH_SetNulVector_ALIGNED   MATH_InitVectorToZero_ALIGNED
_inline_ void MATH_AddVector_ALIGNED(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	MTH_TestAlign16(VDst);
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	MATH_AddVector(VDst, A, B);
}
_inline_ void MATH_AddEqualVector_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	MATH_AddEqualVector(A, B);
}
_inline_ void MATH_SubVector_ALIGNED(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	MTH_TestAlign16(VDst);
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	MATH_SubVector(VDst, A, B);
}
_inline_ void MATH_SubEqualVector_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	MATH_SubEqualVector(A, B);
}
_inline_ void MATH_CopyVector_ALIGNED(MATH_tdst_Vector *VDst, MATH_tdst_Vector *A)
{
	MTH_TestAlign16(VDst);
	MTH_TestAlign16(A);
	MATH_CopyVector(VDst, A);
}
_inline_ void MATH_NegEqualVector_ALIGNED(MATH_tdst_Vector *_pst_Vector)
{
	MTH_TestAlign16(_pst_Vector);
	MATH_NegEqualVector(_pst_Vector);
}
_inline_ void MATH_NegVector_ALIGNED(MATH_tdst_Vector *_pst_VDst, MATH_tdst_Vector *_pst_VSrc)
{
	MTH_TestAlign16(_pst_VDst);
	MTH_TestAlign16(_pst_VSrc);
	MATH_NegVector(_pst_VDst, _pst_VSrc);
}
_inline_ void MATH_InvVector_ALIGNED(MATH_tdst_Vector *_pst_VDst, MATH_tdst_Vector *_pst_VSrc)
{
	MTH_TestAlign16(_pst_VDst);
	MTH_TestAlign16(_pst_VSrc);
	MATH_InvVector(_pst_VDst, _pst_VSrc);
}
_inline_ void MATH_InvEqualVector_ALIGNED(MATH_tdst_Vector *_pst_VSrc)
{
	MTH_TestAlign16(_pst_VSrc);
	MATH_InvEqualVector(_pst_VSrc);
}
_inline_ void MATH_OptInvEqualVector_ALIGNED(MATH_tdst_Vector *pst_Vector)
{
	MTH_TestAlign16(pst_Vector);
	MATH_OptInvEqualVector(pst_Vector);
}
_inline_ void MATH_ScaleVector_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, float f)
{
	MTH_TestAlign16(Dest);
	MTH_TestAlign16(A);
	MATH_ScaleVector(Dest, A, f);
}
#define MATH_MulVector_ALIGNED  MATH_ScaleVector_ALIGNED
_inline_ void MATH_ScaleEqualVector_ALIGNED(MATH_tdst_Vector *A, float f)
{
	MTH_TestAlign16(A);
	MATH_ScaleEqualVector(A, f);
}
#define MATH_MulEqualVector_ALIGNED MATH_ScaleEqualVector_ALIGNED
_inline_ void MATH_DivVector_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, float f)
{
	MTH_TestAlign16(Dest);
	MTH_TestAlign16(A);
	MATH_DivVector(Dest, A, f);
}
_inline_ void MATH_DivEqualVector_ALIGNED(MATH_tdst_Vector *A, float f)
{
	MTH_TestAlign16(A);
	MATH_DivEqualVector(A, f);
}
_inline_ float MATH_f_DotProduct_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	return MATH_f_DotProduct( A, B);
}
_inline_ void MATH_CrossProduct_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	MTH_TestAlign16(Dest);
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	MATH_CrossProduct(Dest, A, B);
}
_inline_ float MATH_f_SqrNormVector_ALIGNED(MATH_tdst_Vector *A)
{
	MTH_TestAlign16(A);
	return MATH_f_SqrNormVector(A);
}
_inline_ float MATH_f_NormVector_ALIGNED(MATH_tdst_Vector *_pst_V)
{
	MTH_TestAlign16(_pst_V);
	return MATH_f_NormVector(_pst_V);
}
_inline_ float MATH_f_Distance_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	return MATH_f_Distance(A, B);
}
_inline_ float MATH_f_SqrVector_ALIGNED(MATH_tdst_Vector *A)
{
	MTH_TestAlign16(A);
	return MATH_f_SqrVector(A);
}
_inline_ float MATH_f_InvNormVector_ALIGNED(MATH_tdst_Vector *A)
{
	MTH_TestAlign16(A);
	return MATH_f_InvNormVector(A);
}
_inline_ void MATH_NormalizeVector_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A)
{
	MTH_TestAlign16(Dest);
	MTH_TestAlign16(A);
	MATH_NormalizeVector(Dest, A);
}
_inline_ void MATH_NormalizeEqualVector_ALIGNED(MATH_tdst_Vector *_pst_V)
{
	MTH_TestAlign16(_pst_V);
	MATH_NormalizeEqualVector(_pst_V);
}
_inline_ void MATH_NormalizeAndRoundVector_ALIGNED
(
    MATH_tdst_Vector    *_pst_DstVector,
    MATH_tdst_Vector    *_pst_SrcVector
)
{
	MTH_TestAlign16(_pst_DstVector);
	MTH_TestAlign16(_pst_SrcVector);
	MATH_NormalizeAndRoundVector(_pst_DstVector,_pst_SrcVector);
}
_inline_ void MATH_SetNormVector_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *Src, float _f_Norm)
{
	MTH_TestAlign16(Dest);
	MTH_TestAlign16(Src);
	MATH_SetNormVector(Dest, Src, _f_Norm);
}
_inline_ void MATH_RoundVector_ALIGNED(MATH_tdst_Vector *_pst_Vector)
{
	MTH_TestAlign16(_pst_Vector);
	MATH_RoundVector(_pst_Vector);
}
_inline_ void MATH_RoundVectorWithPrecision_ALIGNED(MATH_tdst_Vector *_pst_Vector, float f_precision)
{
	MTH_TestAlign16(_pst_Vector);
	MATH_RoundVectorWithPrecision(_pst_Vector, f_precision);
}
_inline_ char MATH_b_NulVector_ALIGNED(MATH_tdst_Vector *pst_Vector)
{
	MTH_TestAlign16(pst_Vector);
	return MATH_b_NulVector(pst_Vector);
}

_inline_ char MATH_b_NulVectorWithEpsilon_ALIGNED(MATH_tdst_Vector *A, float fEps)
{
	MTH_TestAlign16(A);
	return MATH_b_NulVectorWithEpsilon(A, fEps);
}
_inline_ char MATH_b_EqVector_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	return MATH_b_EqVector(A, B);
}

_inline_ char MATH_b_EqVector2_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	return MATH_b_EqVector2(A, B);
}

_inline_ char MATH_b_EqVectorWithEpsilon_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B, float fEps)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	return MATH_b_EqVectorWithEpsilon(A, B, fEps);
}

_inline_ char MATH_b_VecColinear_ALIGNED(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	return MATH_b_VecColinear(A, B);
}
_inline_ void MATH_NormalizeAnyVector_ALIGNED(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(Dest);
	MATH_NormalizeAnyVector(Dest, A);
}
_inline_ void MATH_NormalizeEqualAnyVector_ALIGNED(MATH_tdst_Vector *_pst_V)
{
	MTH_TestAlign16(_pst_V);
	MATH_NormalizeEqualAnyVector(_pst_V);
}
_inline_ float MATH_f_PointLineDistance_ALIGNED
(
    MATH_tdst_Vector    *_pst_Point,
    MATH_tdst_Vector    *_pst_LineA,
    MATH_tdst_Vector    *_pst_LineB
)
{
	MTH_TestAlign16(_pst_Point);
	MTH_TestAlign16(_pst_LineA);
	MTH_TestAlign16(_pst_LineB);
	return MATH_f_PointLineDistance(_pst_Point,_pst_LineA,_pst_LineB);
}
_inline_ void MATH_PointLineProjection_ALIGNED
(
    MATH_tdst_Vector    *_pst_Proj,
    MATH_tdst_Vector    *_pst_Point,
    MATH_tdst_Vector    *_pst_LineA,
    MATH_tdst_Vector    *_pst_LineB
)
{
	MTH_TestAlign16(_pst_Proj);
	MTH_TestAlign16(_pst_Point);
	MTH_TestAlign16(_pst_LineA);
	MTH_TestAlign16(_pst_LineB);
	MATH_PointLineProjection(_pst_Proj,_pst_Point,_pst_LineA,_pst_LineB);
}

_inline_ void MATH_AddScaleVector_ALIGNED
(
    MATH_tdst_Vector    *Dst,
    MATH_tdst_Vector    *A,
    MATH_tdst_Vector    *B,
    float               f
)
{
	MTH_TestAlign16(Dst);
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	MATH_AddScaleVector(Dst,A,B,f);
}

_inline_ void MATH_SubScaleVector_ALIGNED
(
    MATH_tdst_Vector    *Dst,
    MATH_tdst_Vector    *A,
    MATH_tdst_Vector    *B,
    float               f
)
{
	MTH_TestAlign16(Dst);
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	MATH_SubScaleVector(Dst,A,B,f);
}
_inline_ float MATH_f_SubSubDotProduct_ALIGNED
(
    MATH_tdst_Vector    *A,
    MATH_tdst_Vector    *B,
    MATH_tdst_Vector    *C,
    MATH_tdst_Vector    *D
)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	MTH_TestAlign16(C);
	MTH_TestAlign16(D);
	return MATH_f_SubSubDotProduct(A,B,C,D);
}
_inline_ void MATH_f_SubSubCrossProduct_ALIGNED
(
    MATH_tdst_Vector    *Dst,
    MATH_tdst_Vector    *A,
    MATH_tdst_Vector    *B,
    MATH_tdst_Vector    *C,
    MATH_tdst_Vector    *D
)
{
	MTH_TestAlign16(Dst);
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	MTH_TestAlign16(C);
	MTH_TestAlign16(D);
	MATH_f_SubSubCrossProduct(Dst,A,B,C,D);
}
_inline_ void MATH_VectorCombine_ALIGNED
(
    MATH_tdst_Vector    *_pst_VDst,
    MATH_tdst_Vector    *_pst_V1,
    MATH_tdst_Vector    *_pst_V2,
    float               _f_r
)
{
	MTH_TestAlign16(_pst_VDst);
	MTH_TestAlign16(_pst_V1);
	MTH_TestAlign16(_pst_V2);
	MATH_VectorCombine(_pst_VDst,_pst_V1,_pst_V2,_f_r);
}
_inline_ void MATH_MulTwoVectors_ALIGNED
(
    MATH_tdst_Vector    *_pst_Dst,
    MATH_tdst_Vector    *_pst_Src,
    MATH_tdst_Vector    *_pst_Mul
)
{
	MTH_TestAlign16(_pst_Dst);
	MTH_TestAlign16(_pst_Src);
	MTH_TestAlign16(_pst_Mul);
	MATH_MulTwoVectors(_pst_Dst,_pst_Src,_pst_Mul);
}
_inline_ void MATH_MulEqualTwoVectors_ALIGNED(MATH_tdst_Vector *_pst_Dst, MATH_tdst_Vector *_pst_Mul)
{
	MTH_TestAlign16(_pst_Dst);
	MTH_TestAlign16(_pst_Mul);
	MATH_MulEqualTwoVectors(_pst_Dst, _pst_Mul);
}
_inline_ void MATH_ExpEqualVector_ALIGNED(MATH_tdst_Vector *_pst_Dst)
{
	MTH_TestAlign16(_pst_Dst);
	MATH_ExpEqualVector(_pst_Dst);
}
_inline_ void MATH_BlendVector_ALIGNED
(
    MATH_tdst_Vector    *_pst_Dest,
    MATH_tdst_Vector    *_pst_1,
    MATH_tdst_Vector    *_pst_2,
    float               _f_Factor
)
{
	MTH_TestAlign16(_pst_Dest);
	MTH_TestAlign16(_pst_1);
	MTH_TestAlign16(_pst_2);
	MATH_BlendVector(_pst_Dest,_pst_1,_pst_2,_f_Factor);
}
#endif
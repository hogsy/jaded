#ifndef __MATHvector_aligned_h__
#define __MATHvector_aligned_h__

//#define MTH_TestAlign16(a) if (((ULONG)a) & 0xf) *(ULONG *)0 = *(ULONG *)0;
#define MTH_TestAlign16(a) 

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
_inline_ int MATH_b_NulVector_ALIGNED(const MATH_tdst_Vector *pst_Vector)
{
	MTH_TestAlign16(pst_Vector);
	return MATH_b_NulVector(pst_Vector);
}

_inline_ int MATH_b_NulVectorWithEpsilon_ALIGNED( const MATH_tdst_Vector *A, float fEps)
{
	MTH_TestAlign16(A);
	return MATH_b_NulVectorWithEpsilon(A, fEps);
}

_inline_ int MATH_b_EqVector_ALIGNED( const MATH_tdst_Vector *A, const MATH_tdst_Vector *B )
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	return MATH_b_EqVector(A, B);
}

_inline_ int MATH_b_EqVector2_ALIGNED( const MATH_tdst_Vector *A, const MATH_tdst_Vector *B )
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	return MATH_b_EqVector2(A, B);
}

_inline_ int MATH_b_EqVectorWithEpsilon_ALIGNED(const MATH_tdst_Vector *A, const MATH_tdst_Vector *B, float fEps)
{
	MTH_TestAlign16(A);
	MTH_TestAlign16(B);
	return MATH_b_EqVectorWithEpsilon(A, B, fEps);
}

_inline_ int MATH_b_VecColinear_ALIGNED(const MATH_tdst_Vector *A, const MATH_tdst_Vector *B)
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


#endif /* __MATHvector_aligned_h__ */
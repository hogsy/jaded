/*$T MATHmatrixAdvanced.c GC! 1.081 05/03/00 14:27:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

/*
 * Aim: Matrix operation functions Note: Most functions are declared inline in the
 * Matrix.h file. Only the function sthat need no particular speed are here
 */
#include "MATHs/MATHmatrixAdvanced.h"
#include "MATHs/MATHquat.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
    Aim:    Calculates the determinant of the 3x3 part of the matrix

    Time:   K6/2-333 59 clocks
 =======================================================================================================================
 */
float MATH_f_Det33Matrix(MATH_tdst_Matrix *M)
{
	/*~~~~~~*/
	float	f;
	/*~~~~~~*/

	MATH_b_CheckMatrixValidity(M);
	f = fMul(M->Ix, fMulSubMul(M->Jy, M->Kz, M->Jz, M->Ky));
	f += fMul(M->Iy, fMulSubMul(M->Jz, M->Kx, M->Jx, M->Kz));
	f += fMul(M->Iz, fMulSubMul(M->Jx, M->Ky, M->Jy, M->Kx));
	return f;
}

/*
 =======================================================================================================================
    Aim:    Inverts the 3x3 part of a 4x4 matrix without changing the translation

    Time:   K6/2-333 196 clocks (really optimized)

    Note:   The destination matrix MUST be different from the source matrix
 =======================================================================================================================
 */
void MATH_Invert33Matrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M)
{
	/*~~~~~~~~~~~~*/
	float	fInvDet;
	/*~~~~~~~~~~~~*/

	MDst->Ix = fMul(M->Jy, M->Kz) - fMul(M->Ky, M->Jz);
	fInvDet = fMul(M->Ix, MDst->Ix);

	MDst->Iy = fMul(M->Ky, M->Iz) - fMul(M->Iy, M->Kz);
	fInvDet += fMul(M->Jx, MDst->Iy);

	MDst->Iz = fMul(M->Iy, M->Jz) - fMul(M->Jy, M->Iz);
	fInvDet += fMul(M->Kx, MDst->Iz);
	fInvDet = fInv(fInvDet);

	MDst->Jx = (fMul(M->Kx, M->Jz) - fMul(M->Jx, M->Kz)) * fInvDet;
	MDst->Jy = (fMul(M->Ix, M->Kz) - fMul(M->Kx, M->Iz)) * fInvDet;
	MDst->Jz = (fMul(M->Jx, M->Iz) - fMul(M->Ix, M->Jz)) * fInvDet;

	MDst->Kx = (fMul(M->Jx, M->Ky) - fMul(M->Kx, M->Jy)) * fInvDet;
	MDst->Ky = (fMul(M->Kx, M->Iy) - fMul(M->Ix, M->Ky)) * fInvDet;
	MDst->Kz = (fMul(M->Ix, M->Jy) - fMul(M->Jx, M->Iy)) * fInvDet;

	MDst->Ix *= fInvDet;
	MDst->Iy *= fInvDet;
	MDst->Iz *= fInvDet;
	MATH_b_CheckMatrixValidity(MDst);
}

/*
 =======================================================================================================================
    Aim:    Transpose the 3x3 part of a 4x4 matrix

    Time:   K6/2-333 18 clocks

    Note:   This function works only if source is different from destination use MATH_TranspEq33Matrix if source =
            destination
 =======================================================================================================================
 */
void MATH_Transp33MatrixWithoutBuffer(MATH_tdst_Matrix *Dest, MATH_tdst_Matrix *M)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	LONG	*pl_Src, *pl_Dst;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	MATH_b_CheckMatrixValidity(M);
	pl_Src = (LONG *) &(M->Ix);
	pl_Dst = (LONG *) &(Dest->Ix);

	*(pl_Dst) = *(pl_Src);
	*(pl_Dst + 4) = *(pl_Src + 1);
	*(pl_Dst + 8) = *(pl_Src + 2);

	*(pl_Dst + 1) = *(pl_Src + 4);
	*(pl_Dst + 5) = *(pl_Src + 5);
	*(pl_Dst + 9) = *(pl_Src + 6);

	*(pl_Dst + 10) = *(pl_Src + 10);
	*(pl_Dst + 2) = *(pl_Src + 8);
	*(pl_Dst + 6) = *(pl_Src + 9);
}

/*
 =======================================================================================================================
    Aim:    Transpose the 3x3 part of a 4x4 matrix

    Time:   K6/2-333 14 clocks

    Note:   This function transposes directly the Matrix use MATH_Transp33MatrixWithoutBuffer if source is different
            from destination
 =======================================================================================================================
 */
void MATH_TranspEq33Matrix(MATH_tdst_Matrix *M)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	register LONG	Buf;
	LONG			*pl_Src;
	/*~~~~~~~~~~~~~~~~~~~~*/

	pl_Src = (LONG *) &(M->Ix);

	Buf = *(pl_Src + 4);			/* Iy dans buf0 */
	*(pl_Src + 4) = *(pl_Src + 1);	/* Iy=Jx */
	*(pl_Src + 1) = Buf;

	Buf = *(pl_Src + 8);			/* Iz dans buf1 */
	*(pl_Src + 8) = *(pl_Src + 2);	/* Iz=Kx */
	*(pl_Src + 2) = Buf;

	Buf = *(pl_Src + 9);			/* Jz dans buf2 */
	*(pl_Src + 9) = *(pl_Src + 6);	/* Jz=Ky */
	*(pl_Src + 6) = Buf;
	MATH_b_CheckMatrixValidity(M);
}

/*
 =======================================================================================================================
    Aim:    Orthonormalize a rotation matrix
 =======================================================================================================================
 */
#ifdef PSX2_TARGET
asm void MATH_Orthonormalize(MATH_tdst_Matrix *M)
{
.set noreorder
/* ---------------------------------------------------------------------- */
/* Load src unaligned matrix  											  */
/* ---------------------------------------------------------------------- */
		mtsab 		a0,0
		lq			t0,0(a0)
		lq			t1,16(a0)
		lq			t2,32(a0)
		lq			t3,48(a0)
		qfsrv 		t0 , t1 , t0
		qfsrv 		t1 , t2 , t1
		qfsrv 		t2 , t3 , t2
		qmtc2		t0 , $vf10
		qmtc2		t1 , $vf11
		qmtc2		t2 , $vf12
/* Normalize X -----------------------------------------------------------*/
	    vmul.xyz     	$vf20,$vf10,$vf10
	    vaddy.x      	$vf20,$vf20,$vf20y
	    vaddz.x      	$vf20,$vf20,$vf20z
	    vrsqrt       	Q,vf0w,$vf20x
	    vwaitq       
	    vmulq.xyz    	$vf10,$vf10,Q
/* CrossProduct Z X -> Y -------------------------------------------------*/
	    vopmula.xyz 	ACCxyz, $vf12, $vf10
	    vopmsub.xyz 	$vf11, $vf10, $vf12
/* Normalize Y -----------------------------------------------------------*/
	    vmul.xyz     	$vf20,$vf11,$vf11
	    vaddy.x      	$vf20,$vf20,$vf20y
	    vaddz.x      	$vf20,$vf20,$vf20z
	    vrsqrt       	Q,vf0w,$vf20x
	    vwaitq       
	    vmulq.xyz    	$vf11,$vf11,Q
/* CrossProduct X Y -> Z -------------------------------------------------*/
	    vopmula.xyz 	ACCxyz, $vf10, $vf11
	    vopmsub.xyz 	$vf12, $vf11, $vf10
		qmfc2.i		 	t0,$vf10
		qmfc2		 	t1,$vf11
		qmfc2			t2,$vf12
/* ---------------------------------------------------------------------- */
/* store result 														  */
/* ---------------------------------------------------------------------- */
	    sdl		t0,0x07(a0)
	    sdl		t1,0x17(a0)
	    sdl		t2,0x27(a0)
	    sdr		t0,0x00(a0)
	    sdr		t1,0x10(a0)
	    sdr		t2,0x20(a0)
	    pcpyud 	t0, t0, t0
	    pcpyud 	t1, t1, t1
	    pcpyud 	t2, t2, t2
	    sw		t0,0x08(a0)	
	    sw		t1,0x18(a0)	
	    jr 				ra
	    sw		t2,0x28(a0)	
.set reorder
}
#else		
void MATH_Orthonormalize(MATH_tdst_Matrix *M)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_XAxis, *pst_YAxis, *pst_ZAxis;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_XAxis = MATH_pst_GetXAxis(M);
	pst_YAxis = MATH_pst_GetYAxis(M);
	pst_ZAxis = MATH_pst_GetZAxis(M);

	/* Normalize X axis */
	MATH_NormalizeEqualVector(pst_XAxis);

	/* Compute Y axis and normalize it */
	MATH_CrossProduct(pst_YAxis, pst_ZAxis, pst_XAxis);
	MATH_NormalizeEqualVector(pst_YAxis);

	/* Compute Z axis and normalize it */
	MATH_CrossProduct(pst_ZAxis, pst_XAxis, pst_YAxis);
	MATH_NormalizeEqualVector(pst_ZAxis);
	
	MATH_b_CheckMatrixValidity(M);
}
#endif	


/*
 =======================================================================================================================
    Aim:    Calculate the eigenvalues (valeurs propres) of the matrix

    Out:    V: This vector holds the 3 eigenvalues of the Matrix
 =======================================================================================================================
 */
void MATH_CalculateEigenValues(MATH_tdst_Matrix *M, MATH_tdst_Vector *pst_V)
{
	/*~~~~~~~~~~~~*/
	float	A, B, C;
	/*~~~~~~~~~~~~*/

	/* Degree 2 term = -Trace(M) */
	A = -MATH_f_Trace(M);

	/* Degree 1 term = (Ix*Jy - IyJx) + (Ix*Kz - Iz*Kx) + (Jy*Kz-Jz*Ky) */
	B = (M->Ix * M->Jy - M->Iy * M->Jx) + (M->Ix * M->Kz - M->Iz * M->Kx) + (M->Jy * M->Kz - M->Jz * M->Ky);

	/* Degree 0 term = -Det(M) */
	C = -MATH_f_Det(M);

	/* We solve the Cubic equation x^3+A*x^2+B*x+C=0 */
	MATH_i_SolveCubicEquation(A, B, C, pst_V);
	
	MATH_b_CheckMatrixValidity(M);
}

/*
 =======================================================================================================================
    Aim:    Real math function for blending between two matrixes (M1= start matrix, M2= end matrix)

    Note:   Makes: a*M2 + (1-a)*M1 (using slerp between quaternions) £
            set _b_InitBlend to 1 to recalculate the stored parameters
 =======================================================================================================================
 */
void MATH_MatrixBlend
(
	MATH_tdst_Matrix	*_pst_MResult,
	MATH_tdst_Matrix	*_pst_M1,
	MATH_tdst_Matrix	*_pst_M2,
	float				f_Coeff,
	char				_b_InitBlend
)
{
#if 1
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* For speed reasons, we store those parameters from one frame to another */
	static MATH_tdst_Quaternion Q0;
	static MATH_tdst_Quaternion Q1;
	static float				fTheta;
	static float				fInvSinTheta;

	float						fCosTheta;
	MATH_tdst_Quaternion		Q;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_InitBlend)
	{
		MATH_ConvertMatrixToQuaternion(&Q0, _pst_M1);
		MATH_ConvertMatrixToQuaternion(&Q1, _pst_M2);
		fCosTheta = MATH_f_QuaternionDotProduct(&Q0, &Q1);
		if(fCosTheta < 0.0f)
		{
			MATH_NegQuaternion(&Q1);
			fCosTheta = -fCosTheta;
		}
		
		if(fCosTheta > 1.0f)  fCosTheta = 1.0f;		

		//if(fAbs(fCosTheta - 1.0f) > 1E-7f)
		if(fAbs(fCosTheta) < 1.0f - 1E-7f)
		{
			fTheta = fAcos(fCosTheta);
			fInvSinTheta = fInvSqrt(1.0f - fCosTheta * fCosTheta);
			}
		else
		{
			fTheta = 0.0f;
			fInvSinTheta = 0.0f;
		}
	}

	MATH_SetRotationType(_pst_MResult);
	MATH_Slerp(&Q, f_Coeff, &Q0, &Q1, fTheta, fInvSinTheta);
	MATH_ConvertQuaternionToMatrix(_pst_MResult, &Q);
#else
	MATH_CopyMatrix(_pst_MResult,_pst_M1);
#endif	
	MATH_b_CheckMatrixValidity(_pst_MResult);
}
/*
 ===================================================================================================
    Aim:    Spherical lineaire interpolation between two quaternions

    In:     Q0              = Start quaternion
            Q1              = End quaternion
            T               = Time between 0 and 1
            Theta           = Angle between the two quaternions 

    Out:    QDst = [ Q0 . sin ((1-T).Theta) + Q1 . sin (T.Theta) ] / sin Theta
 ===================================================================================================
 */
#ifdef PSX2_TARGET 
asm void MATH_BlendQuatFast(MATH_tdst_Quaternion    *QDST , MATH_tdst_Quaternion    *Q0 , MATH_tdst_Quaternion    *Q1 , float Coef0 , float Coef1)
{	
	.set noreorder
		mtsab 		a1,0
		lq			t0,0(a1)
		lq			t1,16(a1)
		qfsrv 		t0 , t1 , t0
		mfc1		t4,$f12
		mfc1		t5,$f13
		qmtc2		t4,$vf20
		mtsab 		a2,0
		qmtc2		t5,$vf21
		lq			t2,0(a2)
		lq			t3,16(a2)
		qfsrv 		t2 , t3 , t2
		qmtc2		t0,$vf10
		qmtc2		t2,$vf11
		vmulx.xyzw	$vf10 , $vf10 , $vf20x
		vmulx.xyzw	$vf11 , $vf11 , $vf21x
		vadd.xyzw	$vf10 , $vf10 , $vf11
		vmul.xyzw	$vf14 , $vf10 , $vf10
		vaddy.x		$vf12 , $vf14 , $vf14y
		vaddz.x		$vf12 , $vf12 , $vf14z
		vaddw.x		$vf12 , $vf12 , $vf14w
		vrsqrt		Q,vf0w,$vf12x
        vwaitq       
        vmulq.xyzw  $vf10,$vf10,Q
		qmfc2.i		t0,$vf10     
        pcpyud      t1,t0,t0
        sdl         t0,0x00 + 7(a0)
        sdl         t1,0x08 + 7(a0)
        sdr         t0,0x00 + 0(a0)
        jr			ra
        sdr         t1,0x08 + 0(a0)
	.set reorder
}
#endif
void MATH_Slerp
(
    MATH_tdst_Quaternion    *QDst,
    float                   T,
    MATH_tdst_Quaternion    *Q0,
    MATH_tdst_Quaternion    *Q1,
    float                   Theta,
    float                   InvSinTheta
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float                   Coef0, Coef1;
#ifndef PSX2_TARGET 
    MATH_tdst_Quaternion    QTemp;
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!fNul(Theta))
    {
        Coef0 = fSin((1.0f - T) * Theta) * InvSinTheta;
        Coef1 = fSin(T * Theta) * InvSinTheta;
#ifdef PSX2_TARGET 
		MATH_BlendQuatFast(QDst , Q0 , Q1 , Coef0 ,  Coef1);
#else		
        MATH_ScaleQuaternion(&QTemp, Q0, Coef0);
        MATH_ScaleQuaternion(QDst, Q1, Coef1);
        MATH_AddQuaternion(QDst, QDst, &QTemp);
       	MATH_NormalizeQuaternion(QDst, QDst);
#endif       	
    }
    else
    {
        MATH_CopyQuaternion(QDst, Q0);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_QuaternionBlend
(
	MATH_tdst_Matrix		*_pst_MResult,
	MATH_tdst_Quaternion	*_pst_Q1,
	MATH_tdst_Quaternion	*_pst_Q2,
	float					f_Coeff,
	char					_b_InitBlend
)
{
#if 1
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float					fTheta;
	float					fInvSinTheta;
	float					fCosTheta;
	MATH_tdst_Quaternion	Q;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	fTheta = 1.57f;
	fInvSinTheta = 1.0f;
	if(_b_InitBlend)
	{
		fCosTheta = MATH_f_QuaternionDotProduct(_pst_Q1, _pst_Q2);

		if(fCosTheta < 0.0f)
		{
			MATH_NegQuaternion(_pst_Q2);
			fCosTheta = -fCosTheta;
		}
		
		if(fCosTheta > 1.0f)  fCosTheta = 1.0f;
				
		if(fAbs(fCosTheta - 1.0f) > 1E-7f)
		{
			fTheta = fAcos(fCosTheta);
			fInvSinTheta = fInvSqrt(1.0f - fCosTheta * fCosTheta);
		}
		else
		{
			fTheta = 0.0f;
			fInvSinTheta = 0.0f;
		}
	}

	MATH_SetRotationType(_pst_MResult);
	MATH_Slerp(&Q, f_Coeff, _pst_Q1, _pst_Q2, fTheta, fInvSinTheta);
	MATH_ConvertQuaternionToMatrix(_pst_MResult, &Q);
//	MATH_SetCorrectType(_pst_MResult);
#else
	MATH_tdst_Matrix		stM1;
	MATH_tdst_Matrix		stM2;
	MATH_ConvertQuaternionToMatrix(&stM1, _pst_Q1);
	MATH_ConvertQuaternionToMatrix(&stM2, _pst_Q2);
	MATH_MatrixBlend(_pst_MResult,&stM1,&stM2,f_Coeff,_b_InitBlend);
	MATH_SetRotationType(_pst_MResult);
#endif
	MATH_b_CheckMatrixValidity(_pst_MResult);
}
#ifdef JADEFUSION
void	MATH_QuaternionBlend
(
 MATH_tdst_Quaternion * _pst_QResult,
 MATH_tdst_Quaternion * _pst_Q1,
 MATH_tdst_Quaternion * _pst_Q2,
 float                  f_Coeff,
 char                   _b_InitBlend
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float					fTheta;
    float					fInvSinTheta;
    float					fCosTheta;
    //MATH_tdst_Quaternion	Q;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    fTheta = 1.57f;
    fInvSinTheta = 1.0f;
    if(_b_InitBlend)
    {
        fCosTheta = MATH_f_QuaternionDotProduct(_pst_Q1, _pst_Q2);

        if(fCosTheta < 0.0f)
        {
            MATH_NegQuaternion(_pst_Q2);
            fCosTheta = -fCosTheta;
        }

        if(fCosTheta > 1.0f)  fCosTheta = 1.0f;

        if(fAbs(fCosTheta - 1.0f) > 1E-7f)
        {
            fTheta = fAcos(fCosTheta);
            fInvSinTheta = fInvSqrt(1.0f - fCosTheta * fCosTheta);
        }
        else
        {
            fTheta = 0.0f;
            fInvSinTheta = 0.0f;
        }
    }

    MATH_Slerp(_pst_QResult, f_Coeff, _pst_Q1, _pst_Q2, fTheta, fInvSinTheta);
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_ConvertQuaternionToMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Quaternion *Q)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   xx, xy, xz, xw, yy, yz, yw, zz, zw;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*    
ORIGINAL :
    xx = fSqr(Q->x);
    xy = Q->x * Q->y;
    xz = Q->x * Q->z;
    xw = Q->x * Q->w;

    yy = fSqr(Q->y);
    yz = Q->y * Q->z;
    yw = Q->y * Q->w;

    zz = fSqr(Q->z);
    zw = Q->z * Q->w;

    MDst->Ix = 1 - 2 * (yy + zz);
    MDst->Jx = 2 * (xy - zw);
    MDst->Kx = 2 * (xz + yw);

    MDst->Iy = 2 * (xy + zw);
    MDst->Jy = 1 - 2 * (xx + zz);
    MDst->Ky = 2 * (yz - xw);

    MDst->Iz = 2 * (xz - yw);
    MDst->Jz = 2 * (yz + xw);
    MDst->Kz = 1 - 2 * (xx + yy);

	MDst->Sx = 0.0f;
	MDst->Sy = 0.0f;
	MDst->Sz = 0.0f;

	MATH_SetRotationType(MDst);
*/
    xx = 0.5f - 2.0f * Q->x * Q->x;
    xy = 2.0f * Q->x * Q->y;
    xz = 2.0f * Q->x * Q->z;
    xw = 2.0f * Q->x * Q->w;

    yy = 0.5f - 2.0f * Q->y * Q->y;
    yz = 2.0f * Q->y * Q->z;
    yw = 2.0f * Q->y * Q->w;

    zz = 0.5f - 2.0f * Q->z * Q->z;
    zw = 2.0f * Q->z * Q->w;
    
    MDst->Ix = yy + zz;
    MDst->Jx = xy - zw;
    MDst->Kx = xz + yw;

    MDst->Iy = xy + zw;
    MDst->Jy = xx + zz;
    MDst->Ky = yz - xw;

    MDst->Iz = xz - yw;
    MDst->Jz = yz + xw;
    MDst->Kz = xx + yy;

	MDst->Sx = 0.0f;
	MDst->Sy = 0.0f;
	MDst->Sz = 0.0f;

	MATH_SetRotationType(MDst);
    MATH_ClearScaleType(MDst);
	MATH_b_CheckMatrixValidity(MDst);
}


void MATH_ConvertMatrixToQuaternion(MATH_tdst_Quaternion *QDst, MATH_tdst_Matrix *M)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   T, S;
    int     iBestColumn;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_b_CheckMatrixValidity(M);
	
    /* Compute the trace of the matrix */
    T = fAdd3(M->Ix, M->Jy, M->Kz);

    if(fSupZero(T))
    {
        S = fNormalSqrt(T + 1.0f);
        QDst->w = fHalf(S);
        S = fDiv(0.5f, S);
        QDst->x = (M->Jz - M->Ky) * S;
        QDst->y = (M->Kx - M->Iz) * S;
        QDst->z = (M->Iy - M->Jx) * S;
    }
    else
    {
        /* Find the greatest diagonal element */
        if(fSupEq(M->Ix, M->Jy))
            if(fSupEq(M->Ix, M->Kz))
                iBestColumn = 1;
            else
                iBestColumn = 3;
        else if(fSupEq(M->Jy, M->Kz))
            iBestColumn = 2;
        else
            iBestColumn = 3;

        switch(iBestColumn)
        {
        case 1:
            S = fNormalSqrt(1.0f + M->Ix - M->Jy - M->Kz);
            QDst->x = fHalf(S);
            S = fDiv(0.5f, S);
            QDst->y = (M->Jx + M->Iy) * S;
            QDst->z = (M->Kx + M->Iz) * S;
            QDst->w = (M->Jz - M->Ky) * S;
            break;

        case 2:
            S = fNormalSqrt(1.0f - M->Ix + M->Jy - M->Kz);
            QDst->y = fHalf(S);
            S = fDiv(0.5f, S);
            QDst->z = (M->Ky + M->Jz) * S;
            QDst->x = (M->Iy + M->Jx) * S;
            QDst->w = (M->Kx - M->Iz) * S;
            break;

        case 3:
            S = fNormalSqrt(1.0f - M->Ix - M->Jy + M->Kz);
            QDst->z = fHalf(S);
            S = fDiv(0.5f, S);
            QDst->x = (M->Iz + M->Kx) * S;
            QDst->y = (M->Jz + M->Ky) * S;
            QDst->w = (M->Iy - M->Jx) * S;
            break;
        }
    }

    MATH_NormalizeQuaternion(QDst, QDst);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

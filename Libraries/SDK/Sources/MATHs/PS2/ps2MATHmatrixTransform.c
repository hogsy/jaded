/*$T MATHmatrixTransform.c GC! 1.081 04/12/00 11:22:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifdef PSX2_TARGET

#include "MATHs/MATHmatrixAdvanced.h"
#include "MATHs/MATHmatrixScale.h"
#include "BASe/ERRors/ERRasser.h"
#include "MATHs/MATHmatrixTransform.h"/* avoid C++ errors */

/*
 =======================================================================================================================
    Aim:    Copy the 3x3 matrix

    Note:   Copy the rotation type
 =======================================================================================================================
 */
void MATH_Copy33Matrix(MATH_tdst_Matrix *_pst_MDst, MATH_tdst_Matrix *_pst_MSrc)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	LONG	*pl_Src, *pl_Dst;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pl_Src = (LONG *) &(_pst_MSrc->Ix);
	pl_Dst = (LONG *) &(_pst_MDst->Ix);

	*pl_Dst = *pl_Src;
	*(pl_Dst + 1) = *(pl_Src + 1);
	*(pl_Dst + 2) = *(pl_Src + 2);

	*(pl_Dst + 4) = *(pl_Src + 4);
	*(pl_Dst + 5) = *(pl_Src + 5);
	*(pl_Dst + 6) = *(pl_Src + 6);

	*(pl_Dst + 8) = *(pl_Src + 8);
	*(pl_Dst + 9) = *(pl_Src + 9);
	*(pl_Dst + 10) = *(pl_Src + 10);

	if(MATH_b_TestRotationType(_pst_MSrc)) MATH_SetRotationType(_pst_MDst);
}

/*
 =======================================================================================================================
    Aim:    Makes a pure rotation matrix (without translation or scale) from a 4x4 matrix
 =======================================================================================================================
 */
void MATH_GetRotationMatrix(MATH_tdst_Matrix *_pst_MDst, MATH_tdst_Matrix *_pst_MSrc)
{

	if(!((int) _pst_MDst & 0xf) && !((int) _pst_MSrc & 0xf))
		asm_MATH_GetRotationMatrix(_pst_MDst, _pst_MSrc);
	else
	{
		MATH_Copy33Matrix(_pst_MDst, _pst_MSrc);
		MATH_ClearScale(_pst_MDst, 1);
		MATH_ClearTranslation(_pst_MDst);
		MATH_SetRotationType(_pst_MDst);
	}

}

/*
 =======================================================================================================================
    Aim:    Set matrix to identity and clear the translation & projection

    Time:   K6/2-333 20 clocks
 =======================================================================================================================
 */
void MATH_SetIdentityMatrix(MATH_tdst_Matrix *_pst_MSrc)
{
	/*~~~~~~~~~~~~~~*/
	LONG	*lPointer;
	/*~~~~~~~~~~~~~~*/

	lPointer = (LONG *) &(_pst_MSrc->Ix);

	*lPointer = Cl_1f;
	*(lPointer + 1) = Cl_0f;
	*(lPointer + 2) = Cl_0f;
	*(lPointer + 3) = Cl_0f;

	*(lPointer + 4) = Cl_0f;
	*(lPointer + 5) = Cl_1f;
	*(lPointer + 6) = Cl_0f;
	*(lPointer + 7) = Cl_0f;

	*(lPointer + 8) = Cl_0f;
	*(lPointer + 9) = Cl_0f;
	*(lPointer + 10) = Cl_1f;
	*(lPointer + 11) = Cl_0f;

	*(lPointer + 12) = Cl_0f;
	*(lPointer + 13) = Cl_0f;
	*(lPointer + 14) = Cl_0f;
	*(lPointer + 15) = Cl_1f;

	MATH_SetIdentityType(_pst_MSrc);
}

/*
 =======================================================================================================================
    Aim:    Set the 3x3 matrix to identity

    Note:   The rotation flag is cleared, but the scale and the translation don't change
 =======================================================================================================================
 */
void MATH_SetIdentity33Matrix(MATH_tdst_Matrix *_pst_M)
{
	/*~~~~~~~~~~~~*/
	LONG	*pl_Dst;
	/*~~~~~~~~~~~~*/

	pl_Dst = (LONG *) &(_pst_M->Ix);

	*pl_Dst = Cl_1f;
	*(pl_Dst + 1) = Cl_0f;
	*(pl_Dst + 2) = Cl_0f;

	*(pl_Dst + 4) = Cl_0f;
	*(pl_Dst + 5) = Cl_1f;
	*(pl_Dst + 6) = Cl_0f;

	*(pl_Dst + 8) = Cl_0f;
	*(pl_Dst + 9) = Cl_0f;
	*(pl_Dst + 10) = Cl_1f;

	MATH_ClearRotationType(_pst_M);
}


/*$4
 ***********************************************************************************************************************
    Test between matrixes £
    £
    MATH_b_EqMatrixWithEpsilon(matrix*,matrix*,epsilon)
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Test if two matrixes are equal
 =======================================================================================================================
 */
char MATH_b_EqMatrixWithEpsilon(MATH_tdst_Matrix *_pst_M1, MATH_tdst_Matrix *_pst_M2, float f_epsilon)
{
	/*~~~~~~~~~~*/
	char	c_Ret;
	/*~~~~~~~~~~*/

	c_Ret = MATH_b_EqVectorWithEpsilon(MATH_pst_GetXAxis(_pst_M1), MATH_pst_GetXAxis(_pst_M2), f_epsilon);
	c_Ret &= MATH_b_EqVectorWithEpsilon(MATH_pst_GetYAxis(_pst_M1), MATH_pst_GetYAxis(_pst_M2), f_epsilon);
	c_Ret &= MATH_b_EqVectorWithEpsilon(MATH_pst_GetZAxis(_pst_M1), MATH_pst_GetZAxis(_pst_M2), f_epsilon);

	return(c_Ret);
}

#ifdef PSX2_TARGET
asm void MATH_Mul33MatrixMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2, char _bUpdateType)
{
	.set noreorder
		beq          a3,$0,NoUpdateType
		lw           t0,64(a1)
		lw           t1,64(a2)
		lw           t2,64(a0)
		or           t0,t0,t1
		ori          t2,t2,0x4
		andi         t0,t0,0x4
		beq          t0,$0,NoUpdateType
		mtsab 		 a1,0
		sw           t2,64(a0)
NoUpdateType:		
		lq			t0,0(a1)
		lq			t1,16(a1)
		lq			t2,32(a1)
		lq			t3,48(a1)
		qfsrv 		t0 , t1 , t0
		qfsrv 		t1 , t2 , t1
		qfsrv 		t2 , t3 , t2
		qmtc2		t0 , $vf01
		qmtc2		t1 , $vf02
		qmtc2		t2 , $vf03
		mtsab 		a2,0
		lq			t0,0(a2)
		lq			t1,16(a2)
		lq			t2,32(a2)
		lq			t3,48(a2)
		qfsrv 		t0 , t1 , t0
		qfsrv 		t1 , t2 , t1
		qfsrv 		t2 , t3 , t2
		qmtc2		t0 , $vf05
		qmtc2		t1 , $vf06
		qmtc2		t2 , $vf07
		vmulax.xyz   ACC, 	$vf05 , $vf01x
		vmadday.xyz  ACC, 	$vf06 , $vf01y
		vmaddz.xyz   $vf10, $vf07 , $vf01z
		vmulax.xyz   ACC, 	$vf05 , $vf02x
		vmadday.xyz  ACC, 	$vf06 , $vf02y
		vmaddz.xyz   $vf11, $vf07 , $vf02z
		vmulax.xyz   ACC, 	$vf05 , $vf03x
		vmadday.xyz  ACC, 	$vf06 , $vf03y
		vmaddz.xyz   $vf12, $vf07 , $vf03z
		qmfc2		t0 , $vf10
		qmfc2		t1 , $vf11
		qmfc2		t2 , $vf12
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
void MATH_Mul33MatrixMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2, char _bUpdateType)
{
	if(MATH_b_TestIdentityType(M1))
		MATH_Copy33Matrix(MDst, M2);
	else if(MATH_b_TestIdentityType(M2))
		MATH_Copy33Matrix(MDst, M1);
	else
	{
		MDst->Ix = fAdd3(fMul(M1->Ix, M2->Ix), fMul(M1->Iy, M2->Jx), fMul(M1->Iz, M2->Kx));
		MDst->Iy = fAdd3(fMul(M1->Ix, M2->Iy), fMul(M1->Iy, M2->Jy), fMul(M1->Iz, M2->Ky));
		MDst->Iz = fAdd3(fMul(M1->Ix, M2->Iz), fMul(M1->Iy, M2->Jz), fMul(M1->Iz, M2->Kz));

		MDst->Jx = fAdd3(fMul(M1->Jx, M2->Ix), fMul(M1->Jy, M2->Jx), fMul(M1->Jz, M2->Kx));
		MDst->Jy = fAdd3(fMul(M1->Jx, M2->Iy), fMul(M1->Jy, M2->Jy), fMul(M1->Jz, M2->Ky));
		MDst->Jz = fAdd3(fMul(M1->Jx, M2->Iz), fMul(M1->Jy, M2->Jz), fMul(M1->Jz, M2->Kz));

		MDst->Kx = fAdd3(fMul(M1->Kx, M2->Ix), fMul(M1->Ky, M2->Jx), fMul(M1->Kz, M2->Kx));
		MDst->Ky = fAdd3(fMul(M1->Kx, M2->Iy), fMul(M1->Ky, M2->Jy), fMul(M1->Kz, M2->Ky));
		MDst->Kz = fAdd3(fMul(M1->Kx, M2->Iz), fMul(M1->Ky, M2->Jz), fMul(M1->Kz, M2->Kz));

		/* Update the type if asked */
		if(_bUpdateType)
		{
			if(MATH_b_TestRotationType(M1) | MATH_b_TestRotationType(M2)) MATH_SetRotationType(MDst);
		}
	}
}
#endif


/*
 =======================================================================================================================
    Aim:    4*4 Matrix inversion

    Note:   The destination matrix MUST be different from the source matrix £
            1. transposes the 3x3 part £
            2. inverts the scale vector £
            3. neg the trabslation and trabsform it £
 =======================================================================================================================
 */
#ifdef PSX2_TARGET
asm void MATH_InvertMatrix(MATH_tdst_Matrix *_pst_Dst, MATH_tdst_Matrix *_pst_Src)
{
.set noreorder
/* ---------------------------------------------------------------------- */
/* Load src unaligned matrix  											  */
/* ---------------------------------------------------------------------- */
		mtsab 		a1,0
		andi		a2,a0,0xf
		lw  		t7,64(a1) /* Load Flags */
		lq			t0,0(a1)
		lq			t1,16(a1)
		lq			t2,32(a1)
		lq			t3,48(a1)
		lq			t4,64(a1)
		qfsrv 		t0 , t1 , t0
		qfsrv 		t1 , t2 , t1
		qfsrv 		t2 , t3 , t2
		qfsrv 		t3 , t4 , t3
		vsubw.x		$vf31,$vf00,$vf00w		// VF31.x = -1.0f
		qmtc2		t3 , $vf20 				//$vf20 = TRanslation
/* ---------------------------------------------------------------------- */
/* transpose src matrix  												  */
/* ---------------------------------------------------------------------- */
		pextlw     t4,t1,t0
		pextuw     t5,t1,t0
		pextlw     t6,t3,t2
		pextuw     t3,t3,t2
		pcpyld     t0,t6,t4
		pcpyud     t1,t4,t6
		pcpyld     t2,t3,t5
		pcpyud     t3,t5,t3
		andi		t6 , t7 , 0x8
		vmulx.xyz	$vf20,$vf20,$vf31x		// MATH_NegVector
		qmtc2		t0 , $vf10
		qmtc2		t1 , $vf11
		bne			t6 , zero , SCALE
		qmtc2		t2 , $vf12
/* ---------------------------------------------------------------------- */
/* solve no scale  														  */
/* ---------------------------------------------------------------------- */
		vmulax.xyz  ACC, $vf10 , $vf20x
     	vmadday.xyz ACC, $vf11 , $vf20y
	    vmaddz.xyz  $vf13, $vf12, $vf20z
/* ---------------------------------------------------------------------- */
/* store result  														  */
/* ---------------------------------------------------------------------- */
FINISH:	bne		a2,zero,STORE_NON_ALIGNED
		sw  	t7,64(a0) /* save Flags */
		sqc2	$vf10,0x00(a0)
	    sqc2	$vf11,0x10(a0)
	    sqc2	$vf12,0x20(a0)
	    jr 		ra
	    sqc2	$vf13,0x30(a0)
/* ---------------------------------------------------------------------- */
/* solve scale  														  */
/* ---------------------------------------------------------------------- */
SCALE:		
		qmtc2		t3 , $vf13				// = Scale
		vdiv		Q,$vf00w,$vf13x
		vWaitq
		vmulq.w		$vf10 , $vf00 , Q
		vdiv		Q,$vf00w,$vf13y
		vWaitq
		vmulq.w		$vf11 , $vf00 , Q
		vdiv		Q,$vf00w,$vf13z
		vWaitq
		vmulq.w		$vf12 , $vf00 , Q
		vmulw.xyz 	$vf15 , $vf10 , $vf10w
		vmulw.xyz 	$vf16 , $vf11 , $vf11w
		vmulw.xyz 	$vf17 , $vf12 , $vf12w
		vmulax.xyz  ACC, $vf15 , $vf20x
	    vmadday.xyz ACC, $vf16 , $vf20y
		b FINISH
	    vmaddz.xyz  $vf13, $vf17, $vf20z
/* ---------------------------------------------------------------------- */
/* store result non aligned												  */
/* ---------------------------------------------------------------------- */
STORE_NON_ALIGNED:	    
		qmfc2.i		 t0,$vf10
		qmfc2		 t1,$vf11
		qmfc2		 t2,$vf12
		qmfc2		 t3,$vf13
        sdl          t0,0x00 + 7(a0)
        sdl          t1,0x10 + 7(a0)
        sdl          t2,0x20 + 7(a0)
        sdl          t3,0x30 + 7(a0)
        sdr          t0,0x00 + 0(a0)
        sdr          t1,0x10 + 0(a0)
        sdr          t2,0x20 + 0(a0)
        sdr          t3,0x30 + 0(a0)
        pcpyud        t0,t0,t0
        pcpyud        t1,t1,t1
        pcpyud        t2,t2,t2
        pcpyud        t3,t3,t3
        sdl          t0,0x08 + 7(a0)
        sdl          t1,0x18 + 7(a0)
        sdl          t2,0x28 + 7(a0)
        sdl          t3,0x38 + 7(a0)
        sdr          t0,0x08 + 0(a0)
        sdr          t1,0x18 + 0(a0)
        sdr          t2,0x28 + 0(a0)
	    jr 			 ra
        sdr          t3,0x38 + 0(a0)
.set reorder
}
#else
void MATH_InvertMatrix(MATH_tdst_Matrix *_pst_Dst, MATH_tdst_Matrix *_pst_Src)
{
	ERR_X_Assert(_pst_Dst != _pst_Src);
	MATH_Transp33MatrixWithoutBuffer(_pst_Dst, _pst_Src);
	MATH_SetType(_pst_Dst, MATH_l_GetType(_pst_Src));

	if(MATH_b_TestScaleType(_pst_Src))
	{
		_pst_Dst->Sx = fInv(_pst_Src->Sx);
		_pst_Dst->Sy = fInv(_pst_Src->Sy);
		_pst_Dst->Sz = fInv(_pst_Src->Sz);
	}

	MATH_NegVector(MATH_pst_GetTranslation(_pst_Dst), MATH_pst_GetTranslation(_pst_Src));
	MATH_TransformVector(&_pst_Dst->T, _pst_Dst, &_pst_Dst->T);
}
#endif


/*
 =======================================================================================================================
    Aim:    Transform a vector (not a vertex!) from a global to a local coodinates
 =======================================================================================================================
 */
void MATH_VectorGlobalToLocal(MATH_tdst_Vector *_pst_VDst, MATH_tdst_Matrix *_pst_MSrc, MATH_tdst_Vector *_pst_VSrc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_Inv ONLY_PSX2_ALIGNED(16);
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_InvertMatrix(&st_Inv, _pst_MSrc);
	MATH_TransformVectorNoScale(_pst_VDst, &st_Inv, _pst_VSrc);
}


/*
 =======================================================================================================================
    Aim:    Transform a vertex from a global to a local coodinate
 =======================================================================================================================
 */
void MATH_VertexGlobalToLocal(MATH_tdst_Vector *_pst_VDst, MATH_tdst_Matrix *_pst_MSrc, MATH_tdst_Vector *_pst_VSrc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_Inv ONLY_PSX2_ALIGNED(16);
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_InvertMatrix(&st_Inv, _pst_MSrc);
	MATH_TransformVertex(_pst_VDst, &st_Inv, _pst_VSrc);
}

#endif /* PSX2_TARGET */
/*$T MATHmatrix.c GC!1.41 09/28/99 10:08:28 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Matrix operation functions

    Note:   Most functions are declared inline in the Matrix.h file. Only the function sthat need
            no particular speed are here
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#include "MATHs/MATHmatrixScale.h"
#include "MATHs/MATHmatrixAdvanced.h"

/*
 ===================================================================================================
    Aim:    copy a matrix
 ===================================================================================================
 */
#ifdef PSX2_TARGET
asm void MATH_CopyMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M)
{
.set noreorder
		andi		a3,a1,0xf
		mtsab 		a1,0
		andi		a2,a0,0xf
		lw  		t7,0x40(a1) /* Load Flags */
		lq			t0,0x00(a1)
		lq			t1,0x10(a1)
		lq			t2,0x20(a1)
		beq			a3,zero,END_LOAD_ALIGNED
		lq			t3,0x30(a1)
		lq			t4,0x40(a1)
		qfsrv 		t0 , t1 , t0
		qfsrv 		t1 , t2 , t1
		qfsrv 		t2 , t3 , t2
		qfsrv 		t3 , t4 , t3
END_LOAD_ALIGNED:		
		bne			a2,zero,STORE_NON_ALIGNED
		sw  		t7,0x40(a0) /* save Flags */
STORE_ALIGNED:
		sq			t0,0x00(a0)
		sq			t1,0x10(a0)
		sq			t2,0x20(a0)
		jr			ra
		sq			t3,0x30(a0)
STORE_NON_ALIGNED:
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
#endif
/*
 ===================================================================================================
    Aim:    Initialize a 4*4 matrix
 ===================================================================================================
 */
void MATH_SetMatrixWithType
(
    MATH_tdst_Matrix    *M, /* Destination matrix */
    MATH_tdst_Vector    *I, /* 1st Column vector */
    MATH_tdst_Vector    *J, /* 2nd column vector */
    MATH_tdst_Vector    *K, /* 3rd column vector */
    MATH_tdst_Vector    *T, /* Translation vector */
    MATH_tdst_Vector    *S, /* Scale vector */
    float               w,  /* W */
    LONG                _lType
)                           /* Matrix type (see MathStruct.h) */
{
    M->lType = _lType;
    lInterpretFloatAsLong(M->Ix) = lInterpretFloatAsLong(I->x);
    lInterpretFloatAsLong(M->Iy) = lInterpretFloatAsLong(I->y);
    lInterpretFloatAsLong(M->Iz) = lInterpretFloatAsLong(I->z);
    lInterpretFloatAsLong(M->Jx) = lInterpretFloatAsLong(J->x);
    lInterpretFloatAsLong(M->Jy) = lInterpretFloatAsLong(J->y);
    lInterpretFloatAsLong(M->Jz) = lInterpretFloatAsLong(J->z);
    lInterpretFloatAsLong(M->Kx) = lInterpretFloatAsLong(K->x);
    lInterpretFloatAsLong(M->Ky) = lInterpretFloatAsLong(K->y);
    lInterpretFloatAsLong(M->Kz) = lInterpretFloatAsLong(K->z);
    lInterpretFloatAsLong(M->Sx) = lInterpretFloatAsLong(S->x);
    lInterpretFloatAsLong(M->Sy) = lInterpretFloatAsLong(S->y);
    lInterpretFloatAsLong(M->Sz) = lInterpretFloatAsLong(S->z);
    MATH_CopyVector(&M->T, T);
    lInterpretFloatAsLong(M->w) = Cl_1f;// MATRIX W!
    
	MATH_b_CheckMatrixValidity(M);    
}

/*
 ===================================================================================================
    Aim:    Initialise the 3*3 part of a 4*4 matrix
 ===================================================================================================
 */
void MATH_Set33Matrix
(
    MATH_tdst_Matrix    *M,
    MATH_tdst_Vector    *I,
    MATH_tdst_Vector    *J,
    MATH_tdst_Vector    *K
)
{
    lInterpretFloatAsLong(M->Ix) = lInterpretFloatAsLong(I->x);
    lInterpretFloatAsLong(M->Iy) = lInterpretFloatAsLong(I->y);
    lInterpretFloatAsLong(M->Iz) = lInterpretFloatAsLong(I->z);
    lInterpretFloatAsLong(M->Jx) = lInterpretFloatAsLong(J->x);
    lInterpretFloatAsLong(M->Jy) = lInterpretFloatAsLong(J->y);
    lInterpretFloatAsLong(M->Jz) = lInterpretFloatAsLong(J->z);
    lInterpretFloatAsLong(M->Kx) = lInterpretFloatAsLong(K->x);
    lInterpretFloatAsLong(M->Ky) = lInterpretFloatAsLong(K->y);
    lInterpretFloatAsLong(M->Kz) = lInterpretFloatAsLong(K->z);
    
	MATH_b_CheckMatrixValidity(M);    
};

/*
 ===================================================================================================
    Aim:    Calculate the correct type for one matrix
 ===================================================================================================
 */
LONG MATH_l_CalculateCorrectType(MATH_tdst_Matrix *_pst_M)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    pst_Scale;
    MATH_tdst_Matrix    st_Rotation;
    LONG                l_OldType;
    LONG                l_CorrectType;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_b_CheckMatrixValidity(_pst_M);    

    /* We save the old type */
    l_OldType = MATH_l_GetType(_pst_M);

    /* We clear the type, before starting rebuilding it */
    MATH_SetType(_pst_M, 0);

    /* 1. we get the scale (last colum) vector. If it is near from (1,1,1) or (0,0,0) there is no scale */
    MATH_SetScaleType(_pst_M);
    MATH_GetScale(&pst_Scale, _pst_M);
    if(MATH_b_EqVectorWithEpsilon(&pst_Scale, &MATH_gst_IdentityVector, Cf_EpsilonBig))
    {
        MATH_ClearScaleType(_pst_M);
    }
	if(MATH_b_EqVectorWithEpsilon(&pst_Scale, &MATH_gst_NulVector, Cf_EpsilonBig))
    {
        MATH_ClearScaleType(_pst_M);
    }

    /* 2. we get the rotation matrix */
    MATH_GetRotationMatrix(&st_Rotation, _pst_M);

    /* 3. we compare to identity matrix. If near, there is no rotation */
    if(MATH_b_EqMatrixWithEpsilon(&st_Rotation, &MATH_gst_IdentityMatrix, Cf_EpsilonBig))
    {
        MATH_ClearRotation( _pst_M );
		MATH_ClearRotationType( _pst_M );
    }
    else
    {
        MATH_Orthonormalize( _pst_M );
        MATH_SetRotationType(_pst_M);
    }

    /* 4. we compare the translation vector to 0. if near, there is no translation */
    if
    (
        !MATH_b_EqVectorWithEpsilon
        (
            MATH_pst_GetTranslation(_pst_M),
            &MATH_gst_NulVector,
            Cf_EpsilonLow
        )
    )
        MATH_SetTranslationType(_pst_M);

    /* 5: we compare the oldtype with the current type. If equal we return 1 */
    l_CorrectType = MATH_l_GetType(_pst_M);
    MATH_SetType(_pst_M, l_OldType);

    return(l_CorrectType);
}

/*
 ===================================================================================================
    Aim:    Check the matrix type. Return 1 if the type is correct, 0 if the type is incorrect
 ===================================================================================================
 */
BOOL MATH_b_TypeIsCorrect(MATH_tdst_Matrix *_pst_M)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    l_CorrectType;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_b_CheckMatrixValidity(_pst_M);    
	
    l_CorrectType = MATH_l_CalculateCorrectType(_pst_M);
    return(l_CorrectType == MATH_l_GetType(_pst_M));
}

/*
 ===================================================================================================
    Aim:    Sets a correct type to a matrix, whatever the type was before...
 ===================================================================================================
 */
void MATH_SetCorrectType(MATH_tdst_Matrix *_pst_M)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    l_CorrectType;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_b_CheckMatrixValidity(_pst_M);    
	
    l_CorrectType = MATH_l_CalculateCorrectType(_pst_M);
    MATH_SetType(_pst_M, l_CorrectType);
	/* when there is no scale, the last column = (0,0,0,1) */
	if (!MATH_b_TestScaleType(_pst_M)) MATH_ClearScale(_pst_M,0);
}

#ifdef _DEBUG
/*
#ifdef PSX2_TARGET
#define _isnan	isnanf
#endif

#ifdef _GAMECUBE
#define _isnan	isnan
#endif

/*
 ===================================================================================================
    Aim:    Test if a matrix has valid values (no NaN)
 ===================================================================================================
 */
/*int MATH_b_CheckMatrixValidity( MATH_tdst_Matrix    *_pst_M)
{
	if ((MATH_b_TestRotationType(_pst_M) &&
		(_isnan(_pst_M->Ix) || _isnan(_pst_M->Iy) || _isnan(_pst_M->Iz) || 
		_isnan(_pst_M->Jx) || _isnan(_pst_M->Jy) ||	_isnan(_pst_M->Jz) ||
		_isnan(_pst_M->Kx) || _isnan(_pst_M->Ky) ||	_isnan(_pst_M->Kz))) ||
		(MATH_b_TestTranslationType(_pst_M) && 
		(_isnan(_pst_M->T.x) ||_isnan(_pst_M->T.y) ||_isnan(_pst_M->T.z))) ||
		(MATH_b_TestAnyFormOfScaleType(_pst_M) && 
		(_isnan(_pst_M->Sx) || _isnan(_pst_M->Sy) || _isnan(_pst_M->Sz) || _isnan(_pst_M->w))))
	{
#ifdef _GAMECUBE
		OSReport("Invalid Matrix (NaN) \n");
#else
#if !(defined PSX2_TARGET) && !(defined _XBOX)
		OutputDebugString("Invalid Matrix (NaN) \n");
#endif // _PC_RETAIL
#endif // _GAMECUBE
		return 0;
	}
	else
		return 1;
}*/
#endif // _DEBUG

	

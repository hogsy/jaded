/*$T MATHmatrixScale.h GC!1.52 10/11/99 10:10:23 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MATHMatrixscale.h: All scale matrix operations £
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#include "MATHs/MATHmatrixScale.h"
#if defined(_XBOX) && !defined(_XENON) 
	#define USE_SSE	1	//this will enable SSE optimizations

	#ifdef USE_SSE
		#define OPT_MAKEOGLMATRIX	1
	#endif

#include "xmmintrin.h"
#endif
/*$4
 ***************************************************************************************************
    HANDLE THE SCALE PART OF THE 4x4 MATRIX (last colum is used for scale)£
    £
    MATH_GetScale £
    MATH_SetScale £
    MATH_SetZoomMatrix £
    MATH_MakeOGLMatrix £
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Returns the scale of a 4x4 matrix (from the last column)
 ===================================================================================================
 */
void MATH_GetScale(MATH_tdst_Vector *_pst_Scale, MATH_tdst_Matrix *_pst_M)
{
    MATH_b_CheckMatrixValidity(_pst_M);
    if(MATH_b_TestScaleType(_pst_M))
    {
        *(int*)&_pst_Scale->x = *(int*)&_pst_M->Sx;
        *(int*)&_pst_Scale->y = *(int*)&_pst_M->Sy;
        *(int*)&_pst_Scale->z = *(int*)&_pst_M->Sz;
    }
    else
    {
        /* No scale */
        fSetLongToFloat(_pst_Scale->x, Cl_1f);
        fSetLongToFloat(_pst_Scale->y, Cl_1f);
        fSetLongToFloat(_pst_Scale->z, Cl_1f);
    }
}

/*
 ===================================================================================================
    Aim:    Set the scale of a 4x4 matrix
 ===================================================================================================
 */
void MATH_SetScale(MATH_tdst_Matrix *_pst_M, MATH_tdst_Vector *_pst_Scale)
{
    if(!MATH_b_EqVectorWithEpsilon(&MATH_gst_IdentityVector, _pst_Scale, Cf_EpsilonBig) &&
    	!MATH_b_EqVectorWithEpsilon(&MATH_gst_NulVector, _pst_Scale, Cf_EpsilonBig))
    {
        *(int*)&_pst_M->Sx = *(int*)&_pst_Scale->x;
        *(int*)&_pst_M->Sy = *(int*)&_pst_Scale->y;
        *(int*)&_pst_M->Sz = *(int*)&_pst_Scale->z;
        MATH_SetScaleType(_pst_M);
    }
    else
        MATH_ClearScale(_pst_M,1);
    
    MATH_b_CheckMatrixValidity(_pst_M);
}

/*
 ===================================================================================================
    Aim:    Set the zoom of a 4x4 matrix
 ===================================================================================================
 */
void MATH_SetZoom(MATH_tdst_Matrix *_pst_M, float _f_Zoom)
{
    if(!fEqWithEpsilon(Cf_One, _f_Zoom, Cf_EpsilonBig) && !fEqWithEpsilon(0.0f, _f_Zoom, Cf_EpsilonBig))
    {
        *(int*)&_pst_M->Sx = *(int*)&_f_Zoom;
        *(int*)&_pst_M->Sy = *(int*)&_f_Zoom;
        *(int*)&_pst_M->Sz = *(int*)&_f_Zoom;
        MATH_SetScaleType(_pst_M);
    }
    else
        MATH_ClearScale(_pst_M,1);
    MATH_b_CheckMatrixValidity(_pst_M);
}

/*
 ===================================================================================================
 Aim: transforms a game engine matrix into a OpenGl matrix 
 Note: this is done by putting the scale (last column in the engine) into the transform matrix 
 ===================================================================================================
 */
#ifdef PSX2_TARGET
asm void MATH_MakeOGLMatrix(MATH_tdst_Matrix *_pst_MDst, MATH_tdst_Matrix *_pst_MSrc)
{
.set noreorder
		and			a3,a1,0xf
		mtsab 		a1,0
		and			a2,a0,0xf
		lw  		t7,64(a1) /* Load Flags */
		lq			t0,0(a1)
		and			t6,t7,0x8
		lq			t1,16(a1)
		lq			t2,32(a1)
		beq			a3,zero,END_LOAD
		lq			t3,48(a1)
		lq			t4,64(a1)
		qfsrv 		t0 , t1 , t0
		qfsrv 		t1 , t2 , t1
		qfsrv 		t2 , t3 , t2
		qfsrv 		t3 , t4 , t3
END_LOAD:
		beq			t6,zero,NO_SCALE
		qmtc2		t0,$vf10
		qmtc2		t1,$vf11
		qmtc2		t2,$vf12
		vmulw.xyz	$vf10,$vf10,$vf10w
		vmulw.xyz	$vf11,$vf11,$vf11w
		vmulw.xyz	$vf12,$vf12,$vf12w
		qmfc2.i		t0,$vf10
		qmfc2		t1,$vf11
		qmfc2		t2,$vf12
NO_SCALE:
		bne			a2,zero,STORE_NON_ALIGNED
		sw  		t7,64(a0) /* save Flags */
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
#else
void MATH_MakeOGLMatrix(register MATH_tdst_Matrix *_pst_MDst, register MATH_tdst_Matrix *_pst_MSrc)
{
    if (MATH_b_TestScaleType( _pst_MSrc ))
    {
#ifdef _GAMECUBE
		register float fM1I1,fM1I2,fM1J1,fM1J2,fM1K1,fM1K2;
		register float fMDI1,fMDI2,fMDJ1,fMDJ2,fMDK1,fMDK2;
		asm {
		    psq_l    	fM1I1, 0x00(_pst_MSrc), 0, 0;
		    psq_l    	fM1I2, 0x08(_pst_MSrc), 0, 0;
		    psq_l    	fM1J1, 0x10(_pst_MSrc), 0, 0;
		    psq_l    	fM1J2, 0x18(_pst_MSrc), 0, 0;
		    psq_l    	fM1K1, 0x20(_pst_MSrc), 0, 0;
		    psq_l    	fM1K2, 0x28(_pst_MSrc), 0, 0;
		    ps_muls1	fMDI1, fM1I1 , fM1I2
		    ps_muls1	fMDI2, fM1I2 , fM1I2
		    ps_muls1	fMDJ1, fM1J1 , fM1J2
		    ps_muls1	fMDJ2, fM1J2 , fM1J2
		    ps_muls1	fMDK1, fM1K1 , fM1K2
		    ps_muls1	fMDK2, fM1K2 , fM1K2
			psq_st      fMDI1, 0x00(_pst_MDst), 0, 0    	// save xy
			psq_st      fMDI2, 0x08(_pst_MDst), 1, 0    	// save z
			psq_st      fMDJ1, 0x10(_pst_MDst), 0, 0    	// save xy
			psq_st      fMDJ2, 0x18(_pst_MDst), 1, 0    	// save z
			psq_st      fMDK1, 0x20(_pst_MDst), 0, 0    	// save xy
			psq_st      fMDK2, 0x28(_pst_MDst), 1, 0    	// save z
		}
#else
#if defined(OPT_MAKEOGLMATRIX)
		//TiZ
		//SSE optimizations
		__m128 Row0;
		__m128 Row1;
		__m128 Row2;
		__m128 ScaleX;
		__m128 ScaleY;
		__m128 ScaleZ;
		__m128 DstRow0;
		__m128 DstRow1;
		__m128 DstRow2;

		Row0 = _mm_load_ps(&_pst_MSrc->Ix);	//load 4 floats Ix Iy Iz Sx
		ScaleX = _mm_shuffle_ps(Row0,Row0,_MM_SHUFFLE(3,3,3,3));	//Scale X factor Sx Sx Sx Sx
		DstRow0 = _mm_mul_ps(Row0 , ScaleX );	//Ix*Sx Iy*Sx Iz*Sx Sx*Sx
		_mm_stream_ps(&_pst_MDst->Ix, DstRow0);	//back to the original matrices format

		Row1 = _mm_load_ps(&_pst_MSrc->Jx);	//load 4 floats Jx Jy Jz Sy
		ScaleY = _mm_shuffle_ps(Row1,Row1,_MM_SHUFFLE(3,3,3,3));	//Scale Y factor Sy Sy Sy Sy
		DstRow1 = _mm_mul_ps(Row1 , ScaleY );	//Jx*Sx Jy*Sx Jz*Sx Sy*Sy
		_mm_stream_ps(&_pst_MDst->Jx, DstRow1);

		Row2 = _mm_load_ps(&_pst_MSrc->Kx);	//load 4 floats Kx Ky Kz Sz
		ScaleZ = _mm_shuffle_ps(Row2,Row2,_MM_SHUFFLE(3,3,3,3));	//Scale Z factor Sz Sz Sz Sz
		//Note: the scale will be cleared by MATH_ClearScale 
		//		and the translation with MATH_ClearScale
		DstRow2 = _mm_mul_ps(Row2 , ScaleZ );	//Kx*Sx Ky*Sx Kz*Sx Sz*Sz
		_mm_stream_ps(&_pst_MDst->Kx, DstRow2);



#ifdef MakeOGLMatrix_TEST_SUITE
		{
			MATH_tdst_Matrix TiZMDst;

			//Original
			TiZMDst.Ix = _pst_MSrc->Sx * _pst_MSrc->Ix;
			TiZMDst.Iy = _pst_MSrc->Sx * _pst_MSrc->Iy;
			TiZMDst.Iz = _pst_MSrc->Sx * _pst_MSrc->Iz;

			TiZMDst.Jx = _pst_MSrc->Sy * _pst_MSrc->Jx;
			TiZMDst.Jy = _pst_MSrc->Sy * _pst_MSrc->Jy;
			TiZMDst.Jz = _pst_MSrc->Sy * _pst_MSrc->Jz;

			TiZMDst.Kx = _pst_MSrc->Sz * _pst_MSrc->Kx;
			TiZMDst.Ky = _pst_MSrc->Sz * _pst_MSrc->Ky;
			TiZMDst.Kz = _pst_MSrc->Sz * _pst_MSrc->Kz;

			if(	(_pst_MDst->Ix != TiZMDst.Ix) ||
				(_pst_MDst->Iy != TiZMDst.Iy) ||
				(_pst_MDst->Iz != TiZMDst.Iz) ||
				(_pst_MDst->Jx != TiZMDst.Jx) ||
				(_pst_MDst->Jy != TiZMDst.Jy) ||
				(_pst_MDst->Jz != TiZMDst.Jz) ||
				(_pst_MDst->Kx != TiZMDst.Kx) ||
				(_pst_MDst->Ky != TiZMDst.Ky) ||
				(_pst_MDst->Kz != TiZMDst.Kz))
			{
				int boom;
				boom = 1000;
			}
		}
#endif //MakeOGLMatrix_TEST_SUITE
#else 
        _pst_MDst->Ix = _pst_MSrc->Sx * _pst_MSrc->Ix;
        _pst_MDst->Iy = _pst_MSrc->Sx * _pst_MSrc->Iy;
        _pst_MDst->Iz = _pst_MSrc->Sx * _pst_MSrc->Iz;

        _pst_MDst->Jx = _pst_MSrc->Sy * _pst_MSrc->Jx;
        _pst_MDst->Jy = _pst_MSrc->Sy * _pst_MSrc->Jy;
        _pst_MDst->Jz = _pst_MSrc->Sy * _pst_MSrc->Jz;

        _pst_MDst->Kx = _pst_MSrc->Sz * _pst_MSrc->Kx;
        _pst_MDst->Ky = _pst_MSrc->Sz * _pst_MSrc->Ky;
        _pst_MDst->Kz = _pst_MSrc->Sz * _pst_MSrc->Kz;
#endif        
#endif
        MATH_CopyTranslationNoType(_pst_MDst, _pst_MSrc);
        MATH_ClearScale(_pst_MDst,1);
    }
    else
        MATH_CopyMatrix( _pst_MDst, _pst_MSrc );
	_pst_MDst->w = 1.0f;
	
    MATH_b_CheckMatrixValidity(_pst_MDst);
}
#endif	


/*$4
 ***************************************************************************************************
    Handle Scale/Zoom of the 3x3 part of a 4x4 matrix £
    £
    Functions: £
    MATH_Zoom33Matrix £
    MATH_Scale33Matrix £
    MATH_GetScaleFrom33Matrix £
    MATH_GetRotationFromScaled33Matrix £

    Note:   These functions should normally only be used by the display, since in the rest of the
            engine, the scale is stored in the last column of the 4x4 matrix
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Multiplies the 3x3 part of a matrix with a float
 ===================================================================================================
 */
void MATH_Mul33Matrix(MATH_tdst_Matrix *_pst_Mdst, MATH_tdst_Matrix *_pst_Msrc, float f)
{
    MATH_MulVector(MATH_pst_GetXAxis(_pst_Mdst), MATH_pst_GetXAxis(_pst_Msrc), f);
    MATH_MulVector(MATH_pst_GetYAxis(_pst_Mdst), MATH_pst_GetYAxis(_pst_Msrc), f);
    MATH_MulVector(MATH_pst_GetZAxis(_pst_Mdst), MATH_pst_GetZAxis(_pst_Msrc), f);
    MATH_b_CheckMatrixValidity(_pst_Mdst);
}

/*
 ===================================================================================================
    Aim:    Scale the 33 matrix part with a scale vector

    Note:   MDst = S.M (where S is a diagonal Scale matrix) £
 ===================================================================================================
 */
void MATH_Scale33Matrix
(
    MATH_tdst_Matrix    *_pst_Mdst,
    MATH_tdst_Matrix    *_pst_Msrc,
    MATH_tdst_Vector    *_pst_Scale
)
{
    _pst_Mdst->Ix = _pst_Scale->x * _pst_Msrc->Ix;
    _pst_Mdst->Iy = _pst_Scale->x * _pst_Msrc->Iy;
    _pst_Mdst->Iz = _pst_Scale->x * _pst_Msrc->Iz;

    _pst_Mdst->Jx = _pst_Scale->y * _pst_Msrc->Jx;
    _pst_Mdst->Jy = _pst_Scale->y * _pst_Msrc->Jy;
    _pst_Mdst->Jz = _pst_Scale->y * _pst_Msrc->Jz;

    _pst_Mdst->Kx = _pst_Scale->z * _pst_Msrc->Kx;
    _pst_Mdst->Ky = _pst_Scale->z * _pst_Msrc->Ky;
    _pst_Mdst->Kz = _pst_Scale->z * _pst_Msrc->Kz;
    
    MATH_b_CheckMatrixValidity(_pst_Mdst);
}

/*
 ===================================================================================================
    Aim:    Gets the scale from the 3x3 transformation matrix into a vector

    Note:   Only the absolute value of the scale is extracted
 ===================================================================================================
 */
#ifdef _DEBUG
//#define MATH_GetScaleFrom33Matrix_TEST_SUITE

#ifdef MATH_GetScaleFrom33Matrix_TEST_SUITE
MATH_tdst_Vector ScaleVec;
#endif

#endif
void MATH_GetScaleFrom33Matrix(MATH_tdst_Vector *_pst_ScaleVec, MATH_tdst_Matrix *M)
{
    MATH_b_CheckMatrixValidity(M);
    
    /* We calculate the scale */
    _pst_ScaleVec->x = MATH_f_NormVector((MATH_tdst_Vector *) &(M->Ix));
    _pst_ScaleVec->y = MATH_f_NormVector((MATH_tdst_Vector *) &(M->Jx));
    _pst_ScaleVec->z = MATH_f_NormVector((MATH_tdst_Vector *) &(M->Kx));

    /* And round it to 0.0001 to avoid precision drift */
    MATH_RoundVector(_pst_ScaleVec);
}

/*
 ===================================================================================================
    Aim:    Extracts the rotation from the 3x3 transformation matrix

    Note:   The rotation matrix is found by normalizing the 3 axis (if there is scale in the 3x3
            matrix)
 ===================================================================================================
 */
void MATH_GetRotationFromScaled33Matrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M)
{
    /* We normalize and round the 3 axis (to avoid precision drift) */
    MATH_NormalizeAndRoundVector(MATH_pst_GetXAxis(MDst), MATH_pst_GetXAxis(M));
    MATH_NormalizeAndRoundVector(MATH_pst_GetYAxis(MDst), MATH_pst_GetYAxis(M));
    MATH_NormalizeAndRoundVector(MATH_pst_GetZAxis(MDst), MATH_pst_GetZAxis(M));
    
    MATH_b_CheckMatrixValidity(MDst);
}
#ifdef JADEFUSION
float MATH_GetMaxScale(MATH_tdst_Matrix * _pst_M)
{
    if(MATH_b_TestScaleType(_pst_M))
    {
        return fMax3(_pst_M->Sx, _pst_M->Sy, _pst_M->Sz);
    }
    else
    {
        /* No scale */
        return 1.0f;
    }
}
#endif

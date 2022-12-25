/*$T MATHmatrixTransform.c GC! 1.081 04/12/00 11:22:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#if !defined(PSX2_TARGET) && !defined(_GAMECUBE)

#include "MATHs/MATHmatrixAdvanced.h"
#include "MATHs/MATHmatrixScale.h"
#include "BASe/ERRors/ERRasser.h"

#if !defined(_XENON)
#if defined(_XBOX) || defined(_M_X86)
#define	USE_SSE	1
	#if USE_SSE == 1
		#define OPT_MULMATRIXMATRIX_SIMPLE	1	//Set when you want the math function to use SSE optimization.
												//NOTE: works only in conjunction of macro _M_IX86 or _XBOX
//		#define OPT_TRANSFORMVECTOR
		#include "xmmintrin.h"
	#endif//Use_sse
#endif
#endif

#ifdef _XENON_RENDER
#include "d3dx9math.h"
#endif
/*$4
 ***********************************************************************************************************************
    TRANSFORMATIONS (for 4*4 matrix): £
    £
    MATH_TransformVector £
    MATH_TransformVertex £
    MATH_TransformHomVector £
    MATH_MulMatrixMatrix £
 ***********************************************************************************************************************
 */

#ifdef _DEBUG
static BOOL sb_CheckVector = FALSE;
#endif

/*
 =======================================================================================================================
    Aim:    Transforms a 3 coordinates vector of the space (tranformation only, no translation)

    Time:   K6/2-333 79 - 108 (79 if destination vector is different from source vector) (108 if source = destination)

    Note:   The transformation is done by the operation V*M we are in a left-handed system where the vector is
            multiplied left to the matrix. So, the transformation matrix is: M = S.R.T (S=scale, R=rotation, T=
            tanslation)
 =======================================================================================================================
 */
#ifdef OPT_TRANSFORMVECTOR
extern __m128 _MASK_0FFF_;

void MATH_TransformVector(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
	/*~~~~~~~~~~~*/
	float	fx, fy;
	__m128 IJKx;
	__m128 IJKy;
	__m128 IJKz;
	__m128 Sxyz;
	__m128 m1;
	__m128 Vx;
	__m128 Vy;
	__m128 Vz;
	__m128 add1;
	__m128 VSrcxyz;
	__m128 Ixyzw;
	__m128 Jxyzw;
	__m128 Kxyzw; 
	/*~~~~~~~~~~~*/
	
    MATH_b_CheckMatrixValidity(M);    
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckVector(VSrc, "");
		MATH_CheckMatrix(M, "");
	}
#endif

	if(VDst == VSrc)
	{
//		vCopy(&fx, &(VSrc->x));
//		vCopy(&fy, &(VSrc->y));

		if(MATH_b_TestScaleType(M))
		{

			VSrcxyz = _mm_loadu_ps(&VSrc->x);
			Ixyzw = _mm_loadu_ps(&M->Ix);
			Jxyzw = _mm_loadu_ps(&M->Jx);
			Kxyzw = _mm_loadu_ps(&M->Kx);

			Sxyz = _mm_setr_ps(M->Sx, M->Sy, M->Sz, 0.0f);

			IJKx = _mm_shuffle_ps(_mm_unpacklo_ps(Ixyzw,Jxyzw),Kxyzw,_MM_SHUFFLE(3,0,1,0));
			m1 = _mm_mul_ps(_mm_mul_ps(IJKx,Sxyz),VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vx = _mm_add_ps(add1,m1);
			_mm_store_ss(&VSrc->x,_mm_add_ss(Vx,_mm_shuffle_ps(Vx,Vx,_MM_SHUFFLE(0,3,2,1))));

			IJKy = _mm_unpacklo_ps(Ixyzw,Jxyzw);
			IJKy = _mm_shuffle_ps(_mm_shuffle_ps(IJKy,IJKy,_MM_SHUFFLE(0,0,3,2)),Kxyzw,_MM_SHUFFLE(3,1,1,0));
			m1 = _mm_mul_ps(_mm_mul_ps(IJKy,Sxyz),VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vy = _mm_add_ps(add1,m1);
			_mm_store_ss(&VSrc->y, _mm_add_ss(Vy,_mm_shuffle_ps(Vy,Vy,_MM_SHUFFLE(0,3,2,1))));

			IJKz = _mm_shuffle_ps(_mm_unpackhi_ps(Ixyzw,Jxyzw),Kxyzw,_MM_SHUFFLE(3,2,1,0));
			m1 = _mm_mul_ps(_mm_mul_ps(IJKz,Sxyz),VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vz = _mm_add_ps(add1,m1);
			_mm_store_ss(&VSrc->z,_mm_add_ss(Vz,_mm_shuffle_ps(Vz,Vz,_MM_SHUFFLE(0,3,2,1))));
			

/*			VSrc->x = fAdd3
				(
					fMul(fMul(M->Ix, M->Sx), VSrc->x),
					fMul(fMul(M->Jx, M->Sy), VSrc->y),
					fMul(fMul(M->Kx, M->Sz), VSrc->z)
				);
			VSrc->y = fAdd3
				(
					fMul(fMul(M->Iy, M->Sx), fx),
					fMul(fMul(M->Jy, M->Sy), VSrc->y),
					fMul(fMul(M->Ky, M->Sz), VSrc->z)
				);
			VSrc->z = fAdd3
				(
					fMul(fMul(M->Iz, M->Sx), fx),
					fMul(fMul(M->Jz, M->Sy), fy),
					fMul(fMul(M->Kz, M->Sz), VSrc->z)
				);*/
		}
		else
		{
			
			VSrcxyz = _mm_and_ps(_mm_loadu_ps(&VSrc->x),_MASK_0FFF_);

			Ixyzw = _mm_loadu_ps(&M->Ix);
			Jxyzw = _mm_loadu_ps(&M->Jx);
			Kxyzw = _mm_loadu_ps(&M->Kx);

			IJKx = _mm_shuffle_ps(_mm_unpacklo_ps(Ixyzw,Jxyzw),Kxyzw,_MM_SHUFFLE(3,0,1,0));
			m1 = _mm_mul_ps(IJKx,VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vx = _mm_add_ps(add1,m1);
			_mm_store_ss(&VSrc->x,_mm_add_ss(Vx,_mm_shuffle_ps(Vx,Vx,_MM_SHUFFLE(0,3,2,1))));

			IJKy = _mm_unpacklo_ps(Ixyzw,Jxyzw);
			IJKy = _mm_shuffle_ps(_mm_shuffle_ps(IJKy,IJKy,_MM_SHUFFLE(0,0,3,2)),Kxyzw,_MM_SHUFFLE(3,1,1,0));
			m1 = _mm_mul_ps(IJKy,VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vy = _mm_add_ps(add1,m1);
			_mm_store_ss(&VSrc->y, _mm_add_ss(Vy,_mm_shuffle_ps(Vy,Vy,_MM_SHUFFLE(0,3,2,1))));

			IJKz = _mm_shuffle_ps(_mm_unpackhi_ps(Ixyzw,Jxyzw),Kxyzw,_MM_SHUFFLE(3,2,1,0));
			m1 = _mm_mul_ps(IJKz,VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vz = _mm_add_ps(add1,m1);
			_mm_store_ss(&VSrc->z,_mm_add_ss(Vz,_mm_shuffle_ps(Vz,Vz,_MM_SHUFFLE(0,3,2,1))));
			
/*			VSrc->x =	fAdd3(	fMul(M->Ix, VSrc->x), 
								fMul(M->Jx, VSrc->y), 
								fMul(M->Kx, VSrc->z));
			VSrc->y = fAdd3(fMul(M->Iy, fx), fMul(M->Jy, VSrc->y), fMul(M->Ky, VSrc->z));
			VSrc->z = fAdd3(fMul(M->Iz, fx), fMul(M->Jz, fy), fMul(M->Kz, VSrc->z));
*/			
		}
	}
	else
	{
		/*$F
        VDst->x = fAdd3(fMul(M->Ix, VSrc->x), fMul(M->Jx, VSrc->y), fMul(M->Kx, VSrc->z));
        VDst->y = fAdd3(fMul(M->Iy, VSrc->x), fMul(M->Jy, VSrc->y), fMul(M->Ky, VSrc->z));
        VDst->z = fAdd3(fMul(M->Iz, VSrc->x), fMul(M->Jz, VSrc->y), fMul(M->Kz, VSrc->z));
        */
		if(MATH_b_TestScaleType(M))
		{
			VSrcxyz = _mm_loadu_ps(&VSrc->x);
			Ixyzw = _mm_loadu_ps(&M->Ix);
			Jxyzw = _mm_loadu_ps(&M->Jx);
			Kxyzw = _mm_loadu_ps(&M->Kx);

			Sxyz = _mm_setr_ps(M->Sx, M->Sy, M->Sz, 0.0f);

			IJKx = _mm_shuffle_ps(_mm_unpacklo_ps(Ixyzw,Jxyzw),Kxyzw,_MM_SHUFFLE(3,0,1,0));
			m1 = _mm_mul_ps(_mm_mul_ps(IJKx,Sxyz),VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vx = _mm_add_ps(add1,m1);
			_mm_store_ss(&VDst->x,_mm_add_ss(Vx,_mm_shuffle_ps(Vx,Vx,_MM_SHUFFLE(0,3,2,1))));

			IJKy = _mm_unpacklo_ps(Ixyzw,Jxyzw);
			IJKy = _mm_shuffle_ps(_mm_shuffle_ps(IJKy,IJKy,_MM_SHUFFLE(0,0,3,2)),Kxyzw,_MM_SHUFFLE(3,1,1,0));
			m1 = _mm_mul_ps(_mm_mul_ps(IJKy,Sxyz),VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vy = _mm_add_ps(add1,m1);
			_mm_store_ss(&VDst->y, _mm_add_ss(Vy,_mm_shuffle_ps(Vy,Vy,_MM_SHUFFLE(0,3,2,1))));

			IJKz = _mm_shuffle_ps(_mm_unpackhi_ps(Ixyzw,Jxyzw),Kxyzw,_MM_SHUFFLE(3,2,1,0));
			m1 = _mm_mul_ps(_mm_mul_ps(IJKz,Sxyz),VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vz = _mm_add_ps(add1,m1);
			_mm_store_ss(&VDst->z,_mm_add_ss(Vz,_mm_shuffle_ps(Vz,Vz,_MM_SHUFFLE(0,3,2,1))));
			
/*
			VDst->x = fAdd3
				(
					fMul(fMul(M->Ix, M->Sx), VSrc->x),
					fMul(fMul(M->Jx, M->Sy), VSrc->y),
					fMul(fMul(M->Kx, M->Sz), VSrc->z)
				);
			VDst->y = fAdd3
				(
					fMul(fMul(M->Iy, M->Sx), VSrc->x),
					fMul(fMul(M->Jy, M->Sy), VSrc->y),
					fMul(fMul(M->Ky, M->Sz), VSrc->z)
				);
			VDst->z = fAdd3
				(
					fMul(fMul(M->Iz, M->Sx), VSrc->x),
					fMul(fMul(M->Jz, M->Sy), VSrc->y),
					fMul(fMul(M->Kz, M->Sz), VSrc->z)
				);
*/				
		}
		else
		{
			
			VSrcxyz = _mm_and_ps(_mm_loadu_ps(&VSrc->x),_MASK_0FFF_);

			Ixyzw = _mm_loadu_ps(&M->Ix);
			Jxyzw = _mm_loadu_ps(&M->Jx);
			Kxyzw = _mm_loadu_ps(&M->Kx);

			IJKx = _mm_shuffle_ps(_mm_unpacklo_ps(Ixyzw,Jxyzw),Kxyzw,_MM_SHUFFLE(3,0,1,0));
			m1 = _mm_mul_ps(IJKx,VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vx = _mm_add_ps(add1,m1);
			_mm_store_ss(&VDst->x,_mm_add_ss(Vx,_mm_shuffle_ps(Vx,Vx,_MM_SHUFFLE(0,3,2,1))));

			IJKy = _mm_unpacklo_ps(Ixyzw,Jxyzw);
			IJKy = _mm_shuffle_ps(_mm_shuffle_ps(IJKy,IJKy,_MM_SHUFFLE(0,0,3,2)),Kxyzw,_MM_SHUFFLE(3,1,1,0));
			m1 = _mm_mul_ps(IJKy,VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vy = _mm_add_ps(add1,m1);
			_mm_store_ss(&VDst->y, _mm_add_ss(Vy,_mm_shuffle_ps(Vy,Vy,_MM_SHUFFLE(0,3,2,1))));

			IJKz = _mm_shuffle_ps(_mm_unpackhi_ps(Ixyzw,Jxyzw),Kxyzw,_MM_SHUFFLE(3,2,1,0));
			m1 = _mm_mul_ps(IJKz,VSrcxyz);
			add1 = _mm_shuffle_ps(m1,m1,_MM_SHUFFLE(1,0,3,2));
			Vz = _mm_add_ps(add1,m1);
			_mm_store_ss(&VDst->z,_mm_add_ss(Vz,_mm_shuffle_ps(Vz,Vz,_MM_SHUFFLE(0,3,2,1))));

/*
			VDst->x = fAdd3(fMul(M->Ix, VSrc->x), fMul(M->Jx, VSrc->y), fMul(M->Kx, VSrc->z));
			VDst->y = fAdd3(fMul(M->Iy, VSrc->x), fMul(M->Jy, VSrc->y), fMul(M->Ky, VSrc->z));
			VDst->z = fAdd3(fMul(M->Iz, VSrc->x), fMul(M->Jz, VSrc->y), fMul(M->Kz, VSrc->z));
 */
		}
	}
}

#else
void MATH_TransformVector(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
	/*~~~~~~~~~~~*/
	float	fx, fy;
	/*~~~~~~~~~~~*/
    MATH_b_CheckMatrixValidity(M);    
    
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckVector(VSrc, "");
		MATH_CheckMatrix(M, "");
	}
#endif

	if(VDst == VSrc)
	{
		vCopy(&fx, &(VSrc->x));
		vCopy(&fy, &(VSrc->y));
		if(MATH_b_TestScaleType(M))
		{
			VSrc->x = fAdd3
				(
					fMul(fMul(M->Ix, M->Sx), VSrc->x),
					fMul(fMul(M->Jx, M->Sy), VSrc->y),
					fMul(fMul(M->Kx, M->Sz), VSrc->z)
				);
			VSrc->y = fAdd3
				(
					fMul(fMul(M->Iy, M->Sx), fx),
					fMul(fMul(M->Jy, M->Sy), VSrc->y),
					fMul(fMul(M->Ky, M->Sz), VSrc->z)
				);
			VSrc->z = fAdd3
				(
					fMul(fMul(M->Iz, M->Sx), fx),
					fMul(fMul(M->Jz, M->Sy), fy),
					fMul(fMul(M->Kz, M->Sz), VSrc->z)
				);
		}
		else
		{
			VSrc->x = fAdd3(fMul(M->Ix, VSrc->x), fMul(M->Jx, VSrc->y), fMul(M->Kx, VSrc->z));
			VSrc->y = fAdd3(fMul(M->Iy, fx), fMul(M->Jy, VSrc->y), fMul(M->Ky, VSrc->z));
			VSrc->z = fAdd3(fMul(M->Iz, fx), fMul(M->Jz, fy), fMul(M->Kz, VSrc->z));
		}
	}
	else
	{
		/*$F
        VDst->x = fAdd3(fMul(M->Ix, VSrc->x), fMul(M->Jx, VSrc->y), fMul(M->Kx, VSrc->z));
        VDst->y = fAdd3(fMul(M->Iy, VSrc->x), fMul(M->Jy, VSrc->y), fMul(M->Ky, VSrc->z));
        VDst->z = fAdd3(fMul(M->Iz, VSrc->x), fMul(M->Jz, VSrc->y), fMul(M->Kz, VSrc->z));
        */
		if(MATH_b_TestScaleType(M))
		{
			VDst->x = fAdd3
				(
					fMul(fMul(M->Ix, M->Sx), VSrc->x),
					fMul(fMul(M->Jx, M->Sy), VSrc->y),
					fMul(fMul(M->Kx, M->Sz), VSrc->z)
				);
			VDst->y = fAdd3
				(
					fMul(fMul(M->Iy, M->Sx), VSrc->x),
					fMul(fMul(M->Jy, M->Sy), VSrc->y),
					fMul(fMul(M->Ky, M->Sz), VSrc->z)
				);
			VDst->z = fAdd3
				(
					fMul(fMul(M->Iz, M->Sx), VSrc->x),
					fMul(fMul(M->Jz, M->Sy), VSrc->y),
					fMul(fMul(M->Kz, M->Sz), VSrc->z)
				);
		}
		else
		{
			VDst->x = fAdd3(fMul(M->Ix, VSrc->x), fMul(M->Jx, VSrc->y), fMul(M->Kx, VSrc->z));
			VDst->y = fAdd3(fMul(M->Iy, VSrc->x), fMul(M->Jy, VSrc->y), fMul(M->Ky, VSrc->z));
			VDst->z = fAdd3(fMul(M->Iz, VSrc->x), fMul(M->Jz, VSrc->y), fMul(M->Kz, VSrc->z));
		}
	}
}
#endif
/*
 =======================================================================================================================
    Aim:    Transforms a 3 coordinates vector of the space (tranformation only, no translation and no scale)
 =======================================================================================================================
 */
void MATH_TransformVectorNoScale(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
	/*~~~~~~~~~~~*/
	float	fx, fy;
	/*~~~~~~~~~~~*/

    MATH_b_CheckMatrixValidity(M);    
    
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckVector(VSrc, "");
		MATH_CheckMatrix(M, "");
	}
#endif
	if(VDst == VSrc)
	{
		vCopy(&fx, &(VSrc->x));
		vCopy(&fy, &(VSrc->y));
		VSrc->x = fAdd3(fMul(M->Ix, VSrc->x), fMul(M->Jx, VSrc->y), fMul(M->Kx, VSrc->z));
		VSrc->y = fAdd3(fMul(M->Iy, fx), fMul(M->Jy, VSrc->y), fMul(M->Ky, VSrc->z));
		VSrc->z = fAdd3(fMul(M->Iz, fx), fMul(M->Jz, fy), fMul(M->Kz, VSrc->z));
	}
	else
	{
		VDst->x = fAdd3(fMul(M->Ix, VSrc->x), fMul(M->Jx, VSrc->y), fMul(M->Kx, VSrc->z));
		VDst->y = fAdd3(fMul(M->Iy, VSrc->x), fMul(M->Jy, VSrc->y), fMul(M->Ky, VSrc->z));
		VDst->z = fAdd3(fMul(M->Iz, VSrc->x), fMul(M->Jz, VSrc->y), fMul(M->Kz, VSrc->z));
	}
}

/*
 =======================================================================================================================
    Aim:    Transforms a 3 coordinates point of the space (tranformation + translation)

    Time:   K6/2-333 95 - 135 (95 if destination vector is different from source vector) (135 if source = destination)

    Note:   The transformation is done by the operation V*M we are in a left-handed system where the vector is
            multiplied left to the matrix. So, the transformation matrix is: M = S.R.T (S=scale, R=rotation, T=
            tanslation)
 =======================================================================================================================
 */
void MATH_TransformVertex(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
    MATH_b_CheckMatrixValidity(M);    
    
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckVector(VSrc, "");
		MATH_CheckMatrix(M, "");
	}
#endif

	MATH_TransformVector(VDst, M, VSrc);
	MATH_AddVector(VDst, VDst, &(M->T));
}

/*
 =======================================================================================================================
    Aim:    Transforms a 3 coordinates point of the space (tranformation + translation but NO scale)
 =======================================================================================================================
 */
void MATH_TransformVertexNoScale(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
    MATH_b_CheckMatrixValidity(M);    
    
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckVector(VSrc, "");
		MATH_CheckMatrix(M, "");
	}
#endif

	MATH_TransformVectorNoScale(VDst, M, VSrc);
	MATH_AddVector(VDst, VDst, &(M->T));
}

/*
 =======================================================================================================================
    Aim:    Transforms a 4 coordinates point or vector of the space

    Note:   - The 4th coordinate always indicates if it's a point or a vector £
            (x,y,z,0) ==> vector (x,y,z,1) ==> point £
            this function works only if the 4th coordinate is 0 or 1 and if the last column of the 4x4 matrix is
            (0,0,0,1) if not, use MATH_TransformHomVector2 £
            - the transformation is done by the operation V*M we are in a left-handed system where the vector is
            multiplied left to the matrix. So, the transformation matrix is: M = S.R.T (S=scale, R=rotation, T=
            tanslation)
 =======================================================================================================================
 */
void MATH_TransformHomVector(MATH_tdst_HomVector *VDst, MATH_tdst_Matrix *M, MATH_tdst_HomVector *VSrc)
{
    MATH_b_CheckMatrixValidity(M);    
    
	if(fEqLong(VSrc->w, Cl_1f))

	/* It's a point, not a vector */
	{
		MATH_TransformVector((MATH_tdst_Vector *) VDst, M, (MATH_tdst_Vector *) VSrc);
		MATH_AddVector((MATH_tdst_Vector *) VDst, (MATH_tdst_Vector *) VDst, &(M->T));
		VDst->w = 1;
	}
	else
	{
		/* It's a vector */
		MATH_TransformVector((MATH_tdst_Vector *) VDst, M, (MATH_tdst_Vector *) VSrc);
		VDst->w = 0;
	}
}

/*
 =======================================================================================================================
    Aim:    4x4 Matrix multiplication (without taking care of the last column)

    Time:   K6/2-333 37-291 clocks

    Note:   - The destination matrix MUST be different from both source matix £
            - Precisions about timing: £
            37 clocks if M2 is an identity matrix £
            105 clocks if M1 is identity £
            291 clocks if no identity matrix £
            - The type is always updated (it's different from MATH_Mul33MatrixMatrix)
 =======================================================================================================================
 */
#if defined(OPT_MULMATRIXMATRIX_SIMPLE)

#ifdef _DEBUG
//#define MATH_MulMatrixMatrix_Simple_TEST_SUITE
#endif

#ifdef MATH_MulMatrixMatrix_Simple_TEST_SUITE
bool bQPFTicksPerSec = false;
LARGE_INTEGER qwTicksPerSec;
float gQPFTicksPerSec = 733.f;
LONGLONG fTimeStart_OPT;
LONGLONG fTimeEnd_OPT;
LONGLONG fTimeStart_ORIGINAL;
LONGLONG fTimeEnd_ORIGINAL;
float Average_ORIGINAL = 0.0;
float Average_OPT = 0.0;
LARGE_INTEGER qwTime;

MATH_tdst_Matrix Test;
MATH_tdst_Matrix Test2;

#endif //MATH_MulMatrixMatrix_Simple_TEST_SUITE
#endif

//void MATH_MulMatrixMatrix_Simple(register MATH_tdst_Matrix *MDst, register MATH_tdst_Matrix *M1, register MATH_tdst_Matrix *M2)
void MATH_MulMatrixMatrix_Simple(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2)
{ 
#if defined(OPT_MULMATRIXMATRIX_SIMPLE)
	__m128 row0;	
	__m128 row1;
	__m128 row2;
	__m128 row3;

	__m128 ai0;
	__m128 ai1;
	__m128 ai2;
	__m128 mm0;
	__m128 mm1;
	__m128 mm2;
	__m128 PartDst0;
	__m128 PartDst1;
	__m128 Dst1;
	__m128 Dst2;
	__m128 Dst3;
	__m128 Dst4;
	__m128 vec0;
	__m128 vec_tras;

	float	Sx = MDst->Sx;
	float	Sy = MDst->Sy;
	float	Sz = MDst->Sz;
	float	w = MDst->w;
	LONG	lType = MDst->lType;

#ifdef MATH_MulMatrixMatrix_Simple_TEST_SUITE
	QueryPerformanceCounter( &qwTime );
	fTimeStart_OPT = qwTime.QuadPart;
#endif

	



	row0 = _mm_load_ps(&M2->Ix);
	row1 = _mm_load_ps(&M2->Jx);
	row2 = _mm_load_ps(&M2->Kx);
//	row3 = _mm_loadu_ps(&M2->T.x);

	vec0 = _mm_load_ps(&M1->Ix);
	mm0 = _mm_mul_ps(row0,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(0,0,0,0)));
	mm1 = _mm_mul_ps(row1,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(1,1,1,1)));
	mm2 = _mm_mul_ps(row2,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(2,2,2,2)));
	PartDst0 = _mm_add_ps(mm0,mm1);
	Dst1 = _mm_add_ps(PartDst0,mm2);


	vec0 = _mm_load_ps(&M1->Jx);
	mm0 = _mm_mul_ps(row0,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(0,0,0,0)));
	mm1 = _mm_mul_ps(row1,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(1,1,1,1)));
	mm2 = _mm_mul_ps(row2,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(2,2,2,2)));
	PartDst0 = _mm_add_ps(mm0,mm1);
	Dst2 = _mm_add_ps(PartDst0,mm2);

	vec0 = _mm_load_ps(&M1->Kx);
	mm0 = _mm_mul_ps(row0,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(0,0,0,0)));
	mm1 = _mm_mul_ps(row1,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(1,1,1,1)));
	mm2 = _mm_mul_ps(row2,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(2,2,2,2)));
	PartDst0 = _mm_add_ps(mm0,mm1);
	Dst3 = _mm_add_ps(PartDst0,mm2);

	vec0 = _mm_load_ps(&M1->T.x);
	vec_tras = _mm_load_ps(&M2->T.x);

	mm0 = _mm_mul_ps(row0,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(0,0,0,0)));
	mm1 = _mm_mul_ps(row1,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(1,1,1,1)));
	mm2 = _mm_mul_ps(row2,_mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(2,2,2,2)));
	PartDst0 = _mm_add_ps(mm0,mm1);
	Dst4 = _mm_add_ps(vec_tras,mm2);
	Dst4 = _mm_add_ps(Dst4,PartDst0);


	_mm_stream_ps(&MDst->Ix,Dst1);
	_mm_stream_ps(&MDst->Jx,Dst2);
	_mm_stream_ps(&MDst->Kx,Dst3);
	_mm_stream_ps(&MDst->T.x,Dst4);


	MDst->Sx = Sx;
	MDst->Sy = Sy;
	MDst->Sz = Sz;
	MDst->w = w;
	MDst->lType = lType;

#ifdef MATH_MulMatrixMatrix_Simple_TEST_SUITE
	QueryPerformanceCounter( &qwTime );
	fTimeEnd_OPT = qwTime.QuadPart;
#endif

#ifdef MATH_MulMatrixMatrix_Simple_TEST_SUITE
	{
		QueryPerformanceCounter( &qwTime );
		fTimeStart_ORIGINAL = qwTime.QuadPart;

		Test.Ix = fAdd3(fMul(M1->Ix, M2->Ix), fMul(M1->Iy, M2->Jx), fMul(M1->Iz, M2->Kx));
		Test.Iy = fAdd3(fMul(M1->Ix, M2->Iy), fMul(M1->Iy, M2->Jy), fMul(M1->Iz, M2->Ky));
		Test.Iz = fAdd3(fMul(M1->Ix, M2->Iz), fMul(M1->Iy, M2->Jz), fMul(M1->Iz, M2->Kz));


		Test.Jx = fAdd3(fMul(M1->Jx, M2->Ix), fMul(M1->Jy, M2->Jx), fMul(M1->Jz, M2->Kx));
		Test.Jy = fAdd3(fMul(M1->Jx, M2->Iy), fMul(M1->Jy, M2->Jy), fMul(M1->Jz, M2->Ky));
		Test.Jz = fAdd3(fMul(M1->Jx, M2->Iz), fMul(M1->Jy, M2->Jz), fMul(M1->Jz, M2->Kz));


		Test.Kx = fAdd3(fMul(M1->Kx, M2->Ix), fMul(M1->Ky, M2->Jx), fMul(M1->Kz, M2->Kx));
		Test.Ky = fAdd3(fMul(M1->Kx, M2->Iy), fMul(M1->Ky, M2->Jy), fMul(M1->Kz, M2->Ky));
		Test.Kz = fAdd3(fMul(M1->Kx, M2->Iz), fMul(M1->Ky, M2->Jz), fMul(M1->Kz, M2->Kz));


		Test.T.x = fAdd3(fMul(M1->T.x, M2->Ix), fMul(M1->T.y, M2->Jx), fMul(M1->T.z, M2->Kx)) + M2->T.x;
		Test.T.y = fAdd3(fMul(M1->T.x, M2->Iy), fMul(M1->T.y, M2->Jy), fMul(M1->T.z, M2->Ky)) + M2->T.y;
		Test.T.z = fAdd3(fMul(M1->T.x, M2->Iz), fMul(M1->T.y, M2->Jz), fMul(M1->T.z, M2->Kz)) + M2->T.z;

		QueryPerformanceCounter( &qwTime );
		fTimeEnd_ORIGINAL = qwTime.QuadPart;

		memcpy(&Test2,&Test,sizeof(MATH_tdst_Matrix));

		if(	(Test.Ix!=MDst->Ix))
		{
			int	Boom;
			Boom = 0;
			MDst->Ix=Test.Ix;
		}

		if(	(Test.Iy!=MDst->Iy) )
		{
			int	Boom;
			Boom = 0;
			MDst->Iy=Test.Iy;
		}

		if(	(Test.Iz!=MDst->Iz) )
		{
			int	Boom;
			Boom = 0;
			MDst->Iz=Test.Iz;
		}



		if(	(Test.Jx!=MDst->Jx) ||
			(Test.Jy!=MDst->Jy) ||
			(Test.Jz!=MDst->Jz) )
		{
			int	Boom;
			Boom = 0;
			MDst->Jx=Test.Jx;
			MDst->Jy=Test.Jy;
			MDst->Jz=Test.Jz;
		}

		if(	(Test.Kx!=MDst->Kx) ||
			(Test.Ky!=MDst->Ky) ||
			(Test.Kz!=MDst->Kz) )
		{
			int	Boom;
			Boom = 0;
			MDst->Kx=Test.Kx;
			MDst->Ky=Test.Ky;
			MDst->Kz=Test.Kz;
		}

		if(	(Test.T.x!=MDst->T.x) ||
			(Test.T.y!=MDst->T.y) ||
			(Test.T.z!=MDst->T.z) )
		{
			int	Boom;
			Boom = 0;
			MDst->T.x=Test.T.x;
			MDst->T.y=Test.T.y;
			MDst->T.z=Test.T.z;
		}
		
	}

	
	//Results
	Average_ORIGINAL = (Average_ORIGINAL + ((float)(fTimeEnd_ORIGINAL-fTimeStart_ORIGINAL) / gQPFTicksPerSec)) / 2.0f;
	Average_OPT = (Average_OPT + ((float)(fTimeEnd_OPT-fTimeStart_OPT) / gQPFTicksPerSec)) / 2.0f;

#endif	//test_SUITE

#else
	MDst->Ix = fAdd3(fMul(M1->Ix, M2->Ix), fMul(M1->Iy, M2->Jx), fMul(M1->Iz, M2->Kx));
	MDst->Iy = fAdd3(fMul(M1->Ix, M2->Iy), fMul(M1->Iy, M2->Jy), fMul(M1->Iz, M2->Ky));
	MDst->Iz = fAdd3(fMul(M1->Ix, M2->Iz), fMul(M1->Iy, M2->Jz), fMul(M1->Iz, M2->Kz));

	MDst->Jx = fAdd3(fMul(M1->Jx, M2->Ix), fMul(M1->Jy, M2->Jx), fMul(M1->Jz, M2->Kx));
	MDst->Jy = fAdd3(fMul(M1->Jx, M2->Iy), fMul(M1->Jy, M2->Jy), fMul(M1->Jz, M2->Ky));
	MDst->Jz = fAdd3(fMul(M1->Jx, M2->Iz), fMul(M1->Jy, M2->Jz), fMul(M1->Jz, M2->Kz));

	MDst->Kx = fAdd3(fMul(M1->Kx, M2->Ix), fMul(M1->Ky, M2->Jx), fMul(M1->Kz, M2->Kx));
	MDst->Ky = fAdd3(fMul(M1->Kx, M2->Iy), fMul(M1->Ky, M2->Jy), fMul(M1->Kz, M2->Ky));
	MDst->Kz = fAdd3(fMul(M1->Kx, M2->Iz), fMul(M1->Ky, M2->Jz), fMul(M1->Kz, M2->Kz));

	MDst->T.x = fAdd3(fMul(M1->T.x, M2->Ix), fMul(M1->T.y, M2->Jx), fMul(M1->T.z, M2->Kx)) + M2->T.x;
	MDst->T.y = fAdd3(fMul(M1->T.x, M2->Iy), fMul(M1->T.y, M2->Jy), fMul(M1->T.z, M2->Ky)) + M2->T.y;
	MDst->T.z = fAdd3(fMul(M1->T.x, M2->Iz), fMul(M1->T.y, M2->Jz), fMul(M1->T.z, M2->Kz)) + M2->T.z;
#endif

    MATH_b_CheckMatrixValidity(MDst);        
}

#ifdef JADEFUSION
void MATH_MulMatrixMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Matrix	A, B;
    MATH_tdst_Vector	st_Scale;	
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(_DEBUG) 
    if(sb_CheckVector)
    {
        MATH_CheckMatrix(M1, "");
        MATH_CheckMatrix(M2, "");
    }
#endif










    ERR_X_Warning(((MDst!=M1) && (MDst!=M2)), "Dest matrix must nor be one of the to operand\n", "MulMatrixMatrix");

    MATH_SetType(MDst, 0);

    if ( !(MATH_b_TestScaleType(M1) || MATH_b_TestScaleType(M2)) )
    {
#ifdef _GAMECUBE
        M1->Sx = M1->Sy = M1->Sz = 0; M1->w = 1;
        M2->Sx = M2->Sy = M2->Sz = 0; M2->w = 1;
        MTX44Concat(*(Mtx44*)M1, *(Mtx44*)M2, *(Mtx44*)MDst);
#elif defined(_XBOX) || defined(PC_TARGET) || defined(_XENON_RENDER)
        M1->Sx = M1->Sy = M1->Sz = 0; M1->w = 1;
        M2->Sx = M2->Sy = M2->Sz = 0; M2->w = 1;
        D3DXMatrixMultiply((D3DXMATRIX*)MDst, (D3DXMATRIX*)M1, (D3DXMATRIX*)M2);
#else    	
        MDst->Ix = fAdd3(fMul(M1->Ix, M2->Ix), fMul(M1->Iy, M2->Jx), fMul(M1->Iz, M2->Kx));
        MDst->Iy = fAdd3(fMul(M1->Ix, M2->Iy), fMul(M1->Iy, M2->Jy), fMul(M1->Iz, M2->Ky));
        MDst->Iz = fAdd3(fMul(M1->Ix, M2->Iz), fMul(M1->Iy, M2->Jz), fMul(M1->Iz, M2->Kz));
        MDst->T.x = fAdd3(fMul(M1->T.x, M2->Ix), fMul(M1->T.y, M2->Jx), fMul(M1->T.z, M2->Kx)) + M2->T.x;

        MDst->Jx = fAdd3(fMul(M1->Jx, M2->Ix), fMul(M1->Jy, M2->Jx), fMul(M1->Jz, M2->Kx));
        MDst->Jy = fAdd3(fMul(M1->Jx, M2->Iy), fMul(M1->Jy, M2->Jy), fMul(M1->Jz, M2->Ky));
        MDst->Jz = fAdd3(fMul(M1->Jx, M2->Iz), fMul(M1->Jy, M2->Jz), fMul(M1->Jz, M2->Kz));
        MDst->T.y = fAdd3(fMul(M1->T.x, M2->Iy), fMul(M1->T.y, M2->Jy), fMul(M1->T.z, M2->Ky)) + M2->T.y;

        MDst->Kx = fAdd3(fMul(M1->Kx, M2->Ix), fMul(M1->Ky, M2->Jx), fMul(M1->Kz, M2->Kx));
        MDst->Ky = fAdd3(fMul(M1->Kx, M2->Iy), fMul(M1->Ky, M2->Jy), fMul(M1->Kz, M2->Ky));
        MDst->Kz = fAdd3(fMul(M1->Kx, M2->Iz), fMul(M1->Ky, M2->Jz), fMul(M1->Kz, M2->Kz));
        MDst->T.z = fAdd3(fMul(M1->T.x, M2->Iz), fMul(M1->T.y, M2->Jz), fMul(M1->T.z, M2->Kz)) + M2->T.z;

        MDst->Sx = MDst->Sy = MDst->Sz = 0;
        MDst->w = 1;// MATRIX W!
#endif        
    }
    else
    {
        MATH_MakeOGLMatrix(&A, M1);
        MATH_MakeOGLMatrix(&B, M2);

#ifdef _GAMECUBE
        MTX44Concat(*(Mtx44*)&A, *(Mtx44*)&B, *(Mtx44*)MDst);
#elif defined (_XBOX) || defined(PC_TARGET)
        D3DXMatrixMultiply((D3DXMATRIX*)MDst, (D3DXMATRIX*)&A, (D3DXMATRIX*)&B);
#else   
        MDst->Ix = fAdd3(fMul(A.Ix, B.Ix), fMul(A.Iy, B.Jx), fMul(A.Iz, B.Kx));
        MDst->Iy = fAdd3(fMul(A.Ix, B.Iy), fMul(A.Iy, B.Jy), fMul(A.Iz, B.Ky));
        MDst->Iz = fAdd3(fMul(A.Ix, B.Iz), fMul(A.Iy, B.Jz), fMul(A.Iz, B.Kz));
        MDst->T.x = fAdd3(fMul(A.T.x, B.Ix), fMul(A.T.y, B.Jx), fMul(A.T.z, B.Kx)) + B.T.x;

        MDst->Jx = fAdd3(fMul(A.Jx, B.Ix), fMul(A.Jy, B.Jx), fMul(A.Jz, B.Kx));
        MDst->Jy = fAdd3(fMul(A.Jx, B.Iy), fMul(A.Jy, B.Jy), fMul(A.Jz, B.Ky));
        MDst->Jz = fAdd3(fMul(A.Jx, B.Iz), fMul(A.Jy, B.Jz), fMul(A.Jz, B.Kz));
        MDst->T.y = fAdd3(fMul(A.T.x, B.Iy), fMul(A.T.y, B.Jy), fMul(A.T.z, B.Ky)) + B.T.y;

        MDst->Kx = fAdd3(fMul(A.Kx, B.Ix), fMul(A.Ky, B.Jx), fMul(A.Kz, B.Kx));
        MDst->Ky = fAdd3(fMul(A.Kx, B.Iy), fMul(A.Ky, B.Jy), fMul(A.Kz, B.Ky));
        MDst->Kz = fAdd3(fMul(A.Kx, B.Iz), fMul(A.Ky, B.Jz), fMul(A.Kz, B.Kz));
        MDst->T.z = fAdd3(fMul(A.T.x, B.Iz), fMul(A.T.y, B.Jz), fMul(A.T.z, B.Kz)) + B.T.z;
#endif

        MATH_GetScaleFrom33Matrix(&st_Scale, MDst);
        MATH_SetScale(MDst, &st_Scale);

        // Remove the scale from the 3x3 matrix 
        if(MATH_b_TestScaleType(MDst))
        {
            MATH_InvEqualVector(&st_Scale);
            MATH_Scale33Matrix(MDst, MDst, &st_Scale);
        }
    }

    // Update the type 
    if(MATH_b_TestRotationType(M1) || MATH_b_TestRotationType(M2)) 
        MATH_SetRotationType(MDst);


    MATH_UpdateTranslationType(MDst);

#if defined(_DEBUG)
    if(sb_CheckVector)
    {
        MATH_CheckMatrix(MDst, "");
    }
#endif
}
#else //JADEFUSION
void MATH_MulMatrixMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if defined(_XBOX) || defined(_M_X86)
	__declspec(align(16)) MATH_tdst_Matrix	A, B;
#else
	MATH_tdst_Matrix	A, B;
#endif
	MATH_tdst_Vector	st_Scale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckMatrix(M1, "");
		MATH_CheckMatrix(M2, "");
	}
#endif


	MATH_SetType(MDst, 0);

    if ( !(MATH_b_TestScaleType(M1) || MATH_b_TestScaleType(M2)) )
    {
	    MATH_MulMatrixMatrix_Simple(MDst, M1, M2);
        MDst->Sx = MDst->Sy = MDst->Sz = 0;
        MDst->w = 1;// MATRIX W!
    }
	else
    {
	    MATH_MakeOGLMatrix(&A, M1);
	    MATH_MakeOGLMatrix(&B, M2);

	    MATH_MulMatrixMatrix_Simple(MDst, &A, &B);

	    MATH_GetScaleFrom33Matrix(&st_Scale, MDst);
	    MATH_SetScale(MDst, &st_Scale);

	    /* Remove the scale from the 3x3 matrix */
	    if(MATH_b_TestScaleType(MDst))
	    {
		    MATH_InvEqualVector(&st_Scale);
		    MATH_Scale33Matrix(MDst, MDst, &st_Scale);
	    }
    }

	/* Update the type */
	if(MATH_b_TestRotationType(M1) || MATH_b_TestRotationType(M2)) 
        MATH_SetRotationType(MDst);

	
    MATH_UpdateTranslationType(MDst);

#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckMatrix(MDst, "");
	}
#endif

    MATH_b_CheckMatrixValidity(MDst);        
}
#endif //jadefusion
/*$4
 ***********************************************************************************************************************
    MATH_Mul33MatrixMatrix £
    MATH_InvertMatrix £
    MATH_VertexGlobalToLocal (vectorDst*,matrix*,vectorSrc*)£
    MATH_VertexLocalToGlobal (vectorDst*,matrix*,vectorSrc*)£
    MATH_VectorLocalToGlobal (vectorDst*,matrix*,vectorSrc*)£
    MATH_VectorGlobalToLocal (vectorDst*,matrix*,vectorSrc*)£
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Multiply the 3x3 part of the matrix without changing the translation

    Time:   K6/2-333 25-221 clocks

    Note:   Destination matrix must be different from both source matrixes the type of the matrix is updated if
            _bUpdateType is true - £
            timing precisions: £
            25 clocks if M1 is identity £
            27 clocks if M2 is identity £
            214 clocks without update £
            221 clocks with update
 =======================================================================================================================
 */
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
    MATH_b_CheckMatrixValidity(MDst);        
}

/*
 =======================================================================================================================
    Aim:    4*4 Matrix inversion

    Note:   The destination matrix MUST be different from the source matrix £
            1. transposes the 3x3 part £
            2. inverts the scale vector £
            3. neg the trabslation and trabsform it £
 =======================================================================================================================
 */
void MATH_InvertMatrix(MATH_tdst_Matrix *_pst_Dst, MATH_tdst_Matrix *_pst_Src)
{
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckMatrix(_pst_Src, "");
	}
#endif

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
    MATH_b_CheckMatrixValidity(_pst_Dst);        
}

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
	
    MATH_b_CheckMatrixValidity(_pst_MSrc);        
    
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckMatrix(_pst_MSrc, "");
		MATH_CheckVector(_pst_VSrc, "");
	}
#endif

	MATH_InvertMatrix(&st_Inv, _pst_MSrc);
	MATH_TransformVectorNoScale(_pst_VDst, &st_Inv, _pst_VSrc);
}

/*
 =======================================================================================================================
    Aim:    Transform a vector (not a vertex!) from a local to a global coodinates
 =======================================================================================================================
 */
void MATH_VectorLocalToGlobal(MATH_tdst_Vector *_pst_VDst, MATH_tdst_Matrix *_pst_MSrc, MATH_tdst_Vector *_pst_VSrc)
{
    MATH_b_CheckMatrixValidity(_pst_MSrc);        
    
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckMatrix(_pst_MSrc, "");
		MATH_CheckVector(_pst_VSrc, "");
	}
#endif

	MATH_TransformVectorNoScale(_pst_VDst, _pst_MSrc, _pst_VSrc);
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

    MATH_b_CheckMatrixValidity(_pst_MSrc);        
    
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckMatrix(_pst_MSrc, "");
		MATH_CheckVector(_pst_VSrc, "");
	}
#endif

	MATH_InvertMatrix(&st_Inv, _pst_MSrc);
	MATH_TransformVertex(_pst_VDst, &st_Inv, _pst_VSrc);
}

/*
 =======================================================================================================================
    Aim:    Transform a vertex from a local to a global coodinate
 =======================================================================================================================
 */
void MATH_VertexLocalToGlobal(MATH_tdst_Vector *_pst_VDst, MATH_tdst_Matrix *_pst_MSrc, MATH_tdst_Vector *_pst_VSrc)
{
    MATH_b_CheckMatrixValidity(_pst_MSrc);        
    
#ifdef _DEBUG
	if(sb_CheckVector)
	{
		MATH_CheckMatrix(_pst_MSrc, "");
		MATH_CheckVector(_pst_VSrc, "");
	}
#endif

	MATH_TransformVertex(_pst_VDst, _pst_MSrc, _pst_VSrc);
}

#endif /* ! PSX2_TARGET */

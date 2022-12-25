/*$T MATHmatrixTransform.c GC! 1.081 04/12/00 11:22:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#ifdef _GAMECUBE

#include "MATHs/GC/GCMATHgector.h"
#include "MATHs/MATHmatrixAdvanced.h"
#include "MATHs/MATHmatrixScale.h"
#include "MATHs/MATHdebug.h"

#include "BASe/ERRors/ERRasser.h"

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
void MATH_TransformVector(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
	/*~~~~~~~~~~~*/
	float	fx, fy, fz;
	Gector  *pGI, *pGJ, *pGK;
	Gector*pDst;
	Gector Ga;
	/*~~~~~~~~~~~*/
	
	if(MATH_b_TestScaleType(M))
	{
	    pGI = (Gector*)VSrc;
	    Ga = init_ps (M->Sx, M->Sy);
	    Ga = Ga * *pGI;
	    
	    fx = get_ps0(Ga);
	    fy = get_ps1(Ga);
	    fz = VSrc->z * M->Sz;
	}
	else
	{
    	fx= VSrc->x;
    	fy= VSrc->y;
    	fz= VSrc->z;
	}
	
	pGI = (Gector*)&(M->Ix);
	pGJ = (Gector*)&(M->Jx);
	pGK = (Gector*)&(M->Kx);
	pDst = (Gector*)VDst;
	
	*pDst = *pGI*fx + *pGJ*fy + *pGK*fz;
	VDst->z = (M->Iz*fx)+(M->Jz*fy)+(M->Kz*fz);
}

/*
 =======================================================================================================================
    Aim:    Transforms a 3 coordinates vector of the space (tranformation only, no translation and no scale)
 =======================================================================================================================
 */
void MATH_TransformVectorNoScale(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
	/*~~~~~~~~~~~*/
	register float Ix,Iy, Iz;
	register float Jx,Jy,Jz;
	register float Kx,Ky,Kz;
	register float x,y,z;
	/*~~~~~~~~~~~*/

    x=VSrc->x; 
    y=VSrc->y; 
    z=VSrc->z; 

    Ix=M->Ix; 
    Iy=M->Iy; 
    Iz=M->Iz; 
    Jx=M->Jx; 
    Jy=M->Jy; 
    Jz=M->Jz; 
    Kx=M->Kx; 
    Ky=M->Ky; 
    Kz=M->Kz; 
    
	VDst->x = Ix*x+ Jx*y+ Kx*z;
	VDst->y = Iy*x+ Jy*y+ Ky*z;
	VDst->z = Iz*x+ Jz*y+ Kz*z;
}


void MATH_InvTransformVectorNoScale(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
	/*~~~~~~~~~~~*/
	register float Ix,Iy, Iz;
	register float Jx,Jy,Jz;
	register float Kx,Ky,Kz;
	register float x,y,z;
	/*~~~~~~~~~~~*/

    x=VSrc->x; 
    y=VSrc->y; 
    z=VSrc->z; 

    Ix=M->Ix; 
    Iy=M->Iy; 
    Iz=M->Iz; 
    Jx=M->Jx; 
    Jy=M->Jy; 
    Jz=M->Jz; 
    Kx=M->Kx; 
    Ky=M->Ky; 
    Kz=M->Kz; 
    
	VDst->x = Ix*x+ Iy*y+ Iz*z;
	VDst->y = Jx*x+ Jy*y+ Jz*z;
	VDst->z = Kx*x+ Ky*y+ Kz*z;
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
	/*~~~~~~~~~~~*/
	float	fx, fy, fz;
	Gector  *pGI, *pGJ, *pGK;
	Gector*pDst;
	Gector Ga;
	Gector*pT;
	/*~~~~~~~~~~~*/
	
	if(MATH_b_TestScaleType(M))
	{
	    pGI = (Gector*)VSrc;
	    Ga = init_ps (M->Sx, M->Sy);
	    Ga = Ga * *pGI;
	    
	    fx = get_ps0(Ga);
	    fy = get_ps1(Ga);
	    fz = VSrc->z * M->Sz;
	}
	else
	{
    	fx= VSrc->x;
    	fy= VSrc->y;
    	fz= VSrc->z;
	}
	
	pGI = (Gector*)&(M->Ix);
	pGJ = (Gector*)&(M->Jx);
	pGK = (Gector*)&(M->Kx);
	pT = (Gector*)&(M->T);
	pDst = (Gector*)VDst;
	
	*pDst = *pGI*fx + *pGJ*fy + *pGK*fz +*pT;
	VDst->z = (M->Iz*fx)+(M->Jz*fy)+(M->Kz*fz) + M->T.z;
}

/*
 =======================================================================================================================
    Aim:    Transforms a 3 coordinates point of the space (tranformation + translation but NO scale)
 =======================================================================================================================
 */
void MATH_TransformVertexNoScale(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
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
void MATH_MulMatrixMatrix_Simple(register MATH_tdst_Matrix *MDst, register MATH_tdst_Matrix *M1, register MATH_tdst_Matrix *M2)
{
    register float I1x, I1y, I1z;
    register float J1x, J1y, J1z;
    register float K1x, K1y, K1z;

    register float I2x, I2y, I2z;
    register float J2x, J2y, J2z;
    register float K2x, K2y, K2z;

    I1x = M1->Ix;
    I1y = M1->Iy;
    I1z = M1->Iz;
    J1x = M1->Jx;
    J1y = M1->Jy;
    J1z = M1->Jz;
    K1x = M1->Kx;
    K1y = M1->Ky;
    K1z = M1->Kz;

    I2x = M2->Ix;
    I2y = M2->Iy;
    I2z = M2->Iz;
    J2x = M2->Jx;
    J2y = M2->Jy;
    J2z = M2->Jz;
    K2x = M2->Kx;
    K2y = M2->Ky;
    K2z = M2->Kz;

	MDst->Ix = I1x*I2x + I1y*J2x + I1z*K2x;
	MDst->Iy = I1x*I2y + I1y*J2y + I1z*K2y;
	MDst->Iz = I1x*I2z + I1y*J2z + I1z*K2z;

	MDst->Jx = J1x*I2x + J1y*J2x + J1z*K2x;
	MDst->Jy = J1x*I2y + J1y*J2y + J1z*K2y;
	MDst->Jz = J1x*I2z + J1y*J2z + J1z*K2z;

	MDst->Kx =K1x*I2x + K1y*J2x + K1z*K2x;
	MDst->Ky =K1x*I2y + K1y*J2y + K1z*K2y;
	MDst->Kz =K1x*I2z + K1y*J2z + K1z*K2z;


    I1x = M1->T.x;
    I1y = M1->T.y;
    I1z = M1->T.z;

	MDst->T.x = I1x*I2x + I1y*J2x + I1z*K2x + M2->T.x;
	MDst->T.y = I1x*I2y + I1y*J2y + I1z*K2y + M2->T.y;
	MDst->T.z = I1x*I2z + I1y*J2z + I1z*K2z + M2->T.z;
}
void MATH_MulMatrixMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	A, B;
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

}

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
        register float I1x, I1y, I1z;
        register float J1x, J1y, J1z;
        register float K1x, K1y, K1z;

        register float I2x, I2y, I2z;
        register float J2x, J2y, J2z;
        register float K2x, K2y, K2z;

        I1x = M1->Ix;
        I1y = M1->Iy;
        I1z = M1->Iz;
        J1x = M1->Jx;
        J1y = M1->Jy;
        J1z = M1->Jz;
        K1x = M1->Kx;
        K1y = M1->Ky;
        K1z = M1->Kz;

        I2x = M2->Ix;
        I2y = M2->Iy;
        I2z = M2->Iz;
        J2x = M2->Jx;
        J2y = M2->Jy;
        J2z = M2->Jz;
        K2x = M2->Kx;
        K2y = M2->Ky;
        K2z = M2->Kz;

	if(MATH_b_TestIdentityType(M1))
	{
       	MDst->Ix = I2x ;
    	MDst->Iy = I2y ;
    	MDst->Iz = I2z ;

    	MDst->Jx = J2x;
    	MDst->Jy = J2y;
    	MDst->Jz = J2z;

    	MDst->Kx =K2x;
    	MDst->Ky =K2y;
    	MDst->Kz =K2z;

	}
	else if(MATH_b_TestIdentityType(M2))
	{
    	MDst->Ix = I1x;
    	MDst->Iy = I1y;
    	MDst->Iz = I1z;

    	MDst->Jx = J1x;
    	MDst->Jy = J1y;
    	MDst->Jz = J1z;

    	MDst->Kx =K1x;
    	MDst->Ky =K1y;
    	MDst->Kz =K1z;
	}
	else
	{

    	MDst->Ix = I1x*I2x + I1y*J2x + I1z*K2x;
    	MDst->Iy = I1x*I2y + I1y*J2y + I1z*K2y;
    	MDst->Iz = I1x*I2z + I1y*J2z + I1z*K2z;

    	MDst->Jx = J1x*I2x + J1y*J2x + J1z*K2x;
    	MDst->Jy = J1x*I2y + J1y*J2y + J1z*K2y;
    	MDst->Jz = J1x*I2z + J1y*J2z + J1z*K2z;

    	MDst->Kx =K1x*I2x + K1y*J2x + K1z*K2x;
    	MDst->Ky =K1x*I2y + K1y*J2y + K1z*K2y;
    	MDst->Kz =K1x*I2z + K1y*J2z + K1z*K2z;


		/* Update the type if asked */
		if(_bUpdateType)
		{
			if(MATH_b_TestRotationType(M1) | MATH_b_TestRotationType(M2)) MATH_SetRotationType(MDst);
		}
	}
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
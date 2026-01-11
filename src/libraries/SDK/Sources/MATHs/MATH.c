/*$T MATH.c GC!1.40 09/24/99 10:03:34 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Initialisation du module de Maths (essentielement initialisation des tables)
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "MATHs/MATHmatrixTransform.h"
#include "MATHs/MATHinit.h"
#include "random/perlinnoise.h"

void PS2_MATH_InitTrigo2(void);
/*
 ===================================================================================================
    Aim:    Main initialisation fonction
 ===================================================================================================
 */
void MATH_InitModule(void)
{
    /* Initialise the pseudo random numbers generator */
    srand(1);
    MATH_gf_1024by2Pi = 162.9746617261f;
#ifndef PSX2_TARGET
#ifdef JADEFUSION
    MATH_gd_Decal = 3.0F * pow(2.0f, 51.0f);
#else
	MATH_gd_Decal = 3.0F * pow(2, 51);
#endif
#endif
    MATH_CreateInvSqrtTable();
    MATH_CreateInvTable();
    MATH_CreateSqrtTable();
    MATH_CreateTrigTables();
    MATH_SetIdentityMatrix(&MATH_gst_IdentityMatrix);
	MATH_SetIdentityMatrix(&MATH_gst_TmpIdentityMatrix);
    MATH_InitVector(&MATH_gst_IdentityVector, 1, 1, 1);
    MATH_InitVector(&MATH_gst_BaseVectorI, 1, 0, 0);
    MATH_InitVector(&MATH_gst_BaseVectorJ, 0, 1, 0);
    MATH_InitVector(&MATH_gst_BaseVectorK, 0, 0, 1);
    MATH_InitVector(&MATH_gst_NulVector, 0, 0, 0);
#ifdef PSX2_TARGET
    ps2MATH_InitModule();
    PS2_MATH_InitTrigo2();
#endif
    MATH_CreateTrigTables();
#ifdef JADEFUSION
    g_PerlinNoiseMaker.init();
#else
	PerlinInit();
#endif
}

#if!defined(PSX2_TARGET) && !defined(_XENON)
/* for PS2, max.s and min.s instructions are used inline, see ps2MATHfloat.h */
/*
 ===================================================================================================
 ===================================================================================================
 */
float fMin3(float a, float b, float c)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   m;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    m = a;
    if(b < m) m = b;
    if(c < m) m = c;
    return(m);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
float fMax3(float a, float b, float c)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   M;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    M = a;
    if(b > M) M = b;
    if(c > M) M = c;
    return(M);
}
#endif

/*
 ===================================================================================================
    Aim:    Solve an order 3 equation

    In:     A,B,C the 3 coeffs of the normal form x^3+Ax^2+Bx+C=0 

    Out:    V, vector holding the 3 solutions

    Note:   The value returned by the function is the number of different solutions
 ===================================================================================================
 */
int MATH_i_SolveCubicEquation(float f_A, float f_B, float f_C, MATH_tdst_Vector *pst_V)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int     num;        /* Number of solutions (value returned) */
    float   f_D;        /* Determinant */
    float   f_p, f_q;   /* P and q of the form y^3+3py+2q=0 */
    float   f_SqrA;
    float   f_CubeP;
    float   f_u;
    float   f_phi;
    float   f_t;
    float   f_SqrtD;
    float   f_Sub;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Substitute x = y - A/3 to eliminate quadric term: x^3 +px + q = 0 */
    f_SqrA = fSqr(f_A);

    f_p = Cf_Inv3 * (-Cf_Inv3 * f_SqrA + f_B);
    f_q = Cf_Inv2 * (2.0f / 27.0f * f_A * f_SqrA - Cf_Inv3 * f_A * f_B + f_C);

    /* Calculate determinant using Cardano's formula */
    f_CubeP = fCube(f_p);
    f_D = fSqr(f_q) + f_CubeP;

    if(fNulWithEpsilon(f_D, (float)1e-5))
    {
        if(fNulWithEpsilon(f_q, (float)1e-5))                      /* One triple Null solution */
        {
            MATH_SetNulVector(pst_V);
            num = 1;
        }
        else                                                /* One single and one double solution */
        {
            f_u = fCbrt(-f_q);

            pst_V->x = fTwice(f_u);
            pst_V->y = -f_u;
            pst_V->z = -f_u;
            num = 2;
        }
    }
    else if(f_D < 0.0f)                                        /* Casus irreducibilis: three real
                                                             * solutions */
    {
        f_phi = Cf_Inv3 * fAcos(-f_q / fSqrt(-f_CubeP));
        f_t = fTwice(fSqrt(-f_p));

        pst_V->x = f_t * fCos(f_phi);
        pst_V->y = -f_t * fCos(f_phi + Cf_PiBy3);
        pst_V->z = -f_t * fCos(f_phi - Cf_PiBy3);
        num = 3;
    }
    else                                                    /* One real solution */
    {
        f_SqrtD = fSqrt(f_D);
        f_u = fCbrt(f_SqrtD - f_q) - fCbrt(f_SqrtD + f_q);

        pst_V->x = f_u;
        pst_V->y = f_u;
        pst_V->z = f_u;
        num = 1;
    }

    /* Find x from y (x=y-A/3) */
    f_Sub = Cf_Inv3 * f_A;

    pst_V->x -= f_Sub;
    pst_V->y -= f_Sub;
    pst_V->z -= f_Sub;

    return num;
}

/*
 ===================================================================================================
 Aim: project a 2D-point on a parabol of equation y=ax^2+b
 ===================================================================================================
 */
void MATH_PointParabolProjection2D
(
    MATH_tdst_Vector    *_pst_Proj,
    MATH_tdst_Vector    *_pst_M,
    float               a,
    float               b
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float               A, B, C;
    float               K, HalfK;
    float               X, Y;                               /* Coordinates of the projection */
    MATH_tdst_Vector    st_Roots;
    int                 i_NbSolutions;
    float               d0, d1, d2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    
    if(!fNulWithEpsilon(a, Cf_EpsilonLow))
    {
        /* Solve the cubic equation to find the X of the projection */
        K = fInv(a);
        HalfK = fHalf(K);
        A = Cf_Zero;
        B = (HalfK + b - _pst_M->y) * K;
        C = -HalfK * K * _pst_M->x;
        i_NbSolutions = MATH_i_SolveCubicEquation(A, B, C, &st_Roots);
        if(i_NbSolutions > 1)
        {
            /*
             * If more than one solution, we are inside the parabol, we take the nearest point
             */
			X=st_Roots.x;
			Y = fSqr(X) * a + b;
			X-= _pst_M->x;
			Y-=_pst_M->y;
            d0 = fSqr(X)+fSqr(Y);

			X=st_Roots.y;
			Y = fSqr(X) * a + b;
			X-= _pst_M->x;
			Y-=_pst_M->y;
            d1 = fSqr(X)+fSqr(Y);

			X=st_Roots.z;
			Y = fSqr(X) * a + b;
			X-= _pst_M->x;
			Y-=_pst_M->y;
            d2 = fSqr(X)+fSqr(Y);
			
			/*
             * If more than one solution, we are inside the parabol, we take the nearest x from
             * _pst_M->x
             */
			//d0 = fAbs((st_Roots.x - _pst_M->x));
            //d1 = fAbs((st_Roots.y - _pst_M->x));
            //d2 = fAbs((st_Roots.z - _pst_M->x));
			if (d0<d1)
			{
			/* min= d0 ou d2 */
				if(d0<d2)
				{
				 /* min = d0 */
					X=st_Roots.x;
				}
				else
				{
				 /* min = d2 */
					X=st_Roots.z;
				}
			}
			else
			{
			/* min= d1 ou d2 */
				if(d1<d2)
				{
				 /* min = d1 */
					X=st_Roots.y;
				}
				else
				{
				 /* min = d2 */
					X=st_Roots.z;
				}
			}
        }
        else
            /* One solution only */
            X = st_Roots.x;

        /* Y of projection is given simply by Y=a*X^2+b */
        Y = fSqr(X) * a + b;
        _pst_Proj->x = X;
        _pst_Proj->y = Y;
        _pst_Proj->z = 0;
    }
    else
    {
        /* Project to line y=b */
        _pst_Proj->y = b;
        _pst_Proj->x = _pst_M->x;
        _pst_Proj->z = 0;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MATH_PointParabolProjection3D
(
    MATH_tdst_Vector    *_pst_X1,                           /* Projection global coordinates */
	MATH_tdst_Vector    *_pst_Tangent,                           /* Tangent global coordinates */
    MATH_tdst_Vector    *_pst_X0,                           /* Point global coordinates (before
                                                             * projection) */
    MATH_tdst_Vector    *_pst_A,
    MATH_tdst_Vector    *_pst_B,
    MATH_tdst_Vector    *_pst_O,
    float               _f_r
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_OA, st_OB;                       /* Global vectors */

    MATH_tdst_Vector    A, B, M;                            /* Local coordinates of A B and M */
    MATH_tdst_Matrix    st_NewBasis;

    MATH_tdst_Vector    *pst_i;                             /* Local i vector of the new basis */
    MATH_tdst_Vector    *pst_j;                             /* Local j vector of the new basis */
    MATH_tdst_Vector    *pst_k;                             /* Local k vector of the new basis */

    MATH_tdst_Vector    st_LocalProj;                       /* Projection of M on the parabol in
                                                             * the local basis */
	MATH_tdst_Vector	st_LocalTangent;					/* Tangent of the parabol at the projection point */
    float               a, b;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*$2
     -----------------------------------------------------------------------------------------------
        Calculate the basis in which the plane of the parabol is (Ox,Oy) and the Axis of the parabol is
        Oy
     -----------------------------------------------------------------------------------------------
     */

    
    MATH_SubVector(&st_OA, _pst_A, _pst_O);
    MATH_SubVector(&st_OB, _pst_B, _pst_O);
    MATH_NormalizeEqualVector(&st_OA);
    MATH_NormalizeEqualVector(&st_OB);
    pst_i = MATH_pst_GetXAxis(&st_NewBasis);
    pst_j = MATH_pst_GetYAxis(&st_NewBasis);
    pst_k = MATH_pst_GetZAxis(&st_NewBasis);
    MATH_SubVector(pst_i, &st_OB, &st_OA);
    MATH_AddVector(pst_j, &st_OB, &st_OA);
    MATH_NormalizeEqualVector(pst_i);
    MATH_NormalizeEqualVector(pst_j);
    MATH_CrossProduct(pst_k, pst_i, pst_j);
    MATH_SetTranslation(&st_NewBasis, _pst_O);
    MATH_ClearScale(&st_NewBasis,1);

    /*$2
     -----------------------------------------------------------------------------------------------
        Change the points A and B into new basis
     -----------------------------------------------------------------------------------------------
     */

    /* A and B are on the sphere of center O and Radius "_f_r" */
    MATH_MulEqualVector(&st_OA, _f_r);
    MATH_MulEqualVector(&st_OB, _f_r);
    MATH_AddEqualVector(&st_OA, _pst_O);
    MATH_AddEqualVector(&st_OB, _pst_O);

    /* Change coordinates into new basis */
    MATH_VertexGlobalToLocal(&A, &st_NewBasis, &st_OA);
    MATH_VertexGlobalToLocal(&B, &st_NewBasis, &st_OB);
    MATH_VertexGlobalToLocal(&M, &st_NewBasis, _pst_X0);    /* Point to constraint on the parabol */

    /*$2
     -----------------------------------------------------------------------------------------------
        Calculate parabol equation in the plane 0x,0y and project the point on the found parabol
     -----------------------------------------------------------------------------------------------
     */

    a = fDiv(A.y, fTwice(fSqr(A.x)));
    b = fHalf(A.y);
    MATH_PointParabolProjection2D(&st_LocalProj, &M, a, b);
    MATH_VertexLocalToGlobal(_pst_X1, &st_NewBasis, &st_LocalProj);

	/*$2 calculate the tangent vector of the parabol at the projection point */
	st_LocalTangent.x=1.0f;
	st_LocalTangent.y=2*a*st_LocalProj.x;
	st_LocalTangent.z=Cf_Zero;
	MATH_VectorLocalToGlobal(_pst_Tangent, &st_NewBasis, &st_LocalTangent);
}



/*
 ===================================================================================================
 ===================================================================================================
 */
void MATH_PointSegmentProjection
(
	MATH_tdst_Vector    *_pst_X1,	/* Projection */
	MATH_tdst_Vector    *_pst_X0,	/* Initial position */
    MATH_tdst_Vector    *_pst_A,
    MATH_tdst_Vector    *_pst_B
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_AM, st_AB, st_MB;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    MATH_PointLineProjection(_pst_X1, _pst_X0, _pst_A, _pst_B);

    /* Then, if we are ouside the AB segment, we Snap on the good extremity */
    MATH_SubVector(&st_AM, _pst_X1, _pst_A);
    MATH_SubVector(&st_MB, _pst_B, _pst_X1);
    if(MATH_f_DotProduct(&st_AM, &st_MB) < Cf_Zero)
    {
        MATH_SubVector(&st_AB, _pst_B, _pst_A);

        /* We are ouside */
        if(MATH_f_DotProduct(&st_AM, &st_AB) < Cf_Zero)
        {
            /* Snap to A */
            MATH_CopyVector(_pst_X1, _pst_A);
        }
        else
        {
            /* Snap to B */
            MATH_CopyVector(_pst_X1, _pst_B);
        }
    }
}

s32 GetUpperPowerOf2(u32 Value)
{
	s32 SACA;
	*(f32 *)&SACA = (f32)Value ;
	SACA --;
	SACA >>= 23;
	SACA &= 0xff;
	SACA -= 127;
	return 1<<(SACA + 1);
}

s32 GetExpOfUpperPowerOf2(f32 Value)
{
	s32 SACA;
	*(f32 *)&SACA = Value ;
	SACA --;
	SACA >>= 23;
	SACA &= 0xff;
	SACA -= 127;
	return SACA + 1;
}
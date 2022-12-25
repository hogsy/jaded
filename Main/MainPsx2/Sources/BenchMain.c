/*$T BenchMain.c GC! 1.081 07/13/00 12:14:09 */

#ifdef PSX2_BENCH
/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <stdio.h>
#include <libpc.h>
#include <string.h>
#define USE_ONLY_FOR_BENTCH
#include "MATH.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"

#ifdef __cplusplus
extern "C"
{
#endif



/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define M_CountConfig	(int) (SCE_PC_U1 | SCE_PC_U0 | SCE_PC_CTE | SCE_PC1_CPU_CYCLE | SCE_PC0_NO_EVENT)


/*
 =======================================================================================================================
    (SCE_PC_EXL1 | SCE_PC_K1 | SCE_PC_S1 | SCE_PC_U1 | SCE_PC_U0 | SCE_PC_CTE | SCE_PC1_CPU_CYCLE | SCE_PC0_NO_EVENT)
 =======================================================================================================================
 */
#define M_Start()		scePcStart(M_CountConfig, 0, 0)
#define M_Stop()		scePcStop()
#define M_Read()		scePcGetCounter1()
#define M_ActivationNb	500
#define M_Bentch(_fct, _ret, _arg, ) \
	{ \
		int pc, pcMin, pcMax, pcTotal, nb; \
		int i; \
		pc = pcMax = pcTotal = nb = 0; \
		pcMin = 0x7fffffff; \
		for(i = 0; i < M_ActivationNb; i++) \
		{ \
			M_Start(); \
			_ret = _fct(_arg); \
			M_Stop(); \
			pc = M_Read(); \
			pcMin = min(pcMin, pc); \
			pcMax = max(pcMax, pc); \
			pcTotal += pc; \
			nb++; \
		} \
		printf("-- %s : %d / %d / %d (%d)\n", #_fct, pcMin, pcTotal / nb, pcMax, nb); \
	}

/*$off*/

#define M_BeginBentch(_fct) \
	{ \
		int pc, pcMin, pcMax, pcTotal, nb; \
		int i; \
		pc = pcMax = pcTotal = nb = 0; \
		pcMin = 0x7fffffff; \
		for(i = 0; i < M_ActivationNb; i++) \
		{ \
			M_Start()

#define M_EndBentch(_fct) \
			M_Stop(); \
			pc = M_Read(); \
			pcMin = min(pcMin, pc); \
			pcMax = max(pcMax, pc); \
			pcTotal += pc; \
			nb++; \
		} \
		printf("-- %s : %d / %d / %d (%d)\n", #_fct, pcMin, pcTotal / nb, pcMax, nb); \
	}

/*$on*/

/*$4
 ***********************************************************************************************************************
    static prototypes
 ***********************************************************************************************************************
 */

static void					s_tst_MATHfloat(void);
static void					s_tst_MATHvector(void);
static void					s_tst_MATHmatrix(void);
static void                 s_tst_Others(void);

/*$4
 ***********************************************************************************************************************
    static variables
 ***********************************************************************************************************************
 */

static float				sf;
float						a, b, c;
float						*psf;
int							j, gk, gl;
MATH_tdst_Vector			va, vb, vc;
MATH_tdst_Vector align_va	__attribute__((aligned(16)));
MATH_tdst_Vector align_vb	__attribute__((aligned(16)));
MATH_tdst_Vector align_vc	__attribute__((aligned(16)));

char						hole[7];
MATH_tdst_Matrix			ma =
{
	21.0f,
	34.2f,
	843.9f,
	2344343.0f,
	151.0f,
	613.0f,
	651.0f,
	65.0f,
	616.0f,
	61.0f,
	161.0f,
	1.0f,
	6.0f,
	1514543.235f,
	0.21f,
	515.0f,
	151648
};
MATH_tdst_Matrix			mb =
{
	616.0f,
	61.0f,
	161.0f,
	1.0f,
	21.0f,
	34.2f,
	843.9f,
	2344343.0f,
	151.0f,
	613.0f,
	651.0f,
	65.0f,
	6165.0f,
	1.0f,
	651.0f,
	614.0f,
	8
};

MATH_tdst_Matrix			mc =
{
	616.0f,
	61.0f,
	161.0f,
	1.0f,
	21.0f,
	34.2f,
	843.9f,
	2344343.0f,
	151.0f,
	613.0f,
	651.0f,
	65.0f,
	6165.0f,
	1.0f,
	614.0f,
	23.0f,
	-1
};

MATH_tdst_Matrix align_ma __attribute__ ((aligned(16))) =
{
	616.0f,
	61.0f,
	161.0f,
	1.0f,
	21.0f,
	34.2f,
	844.9f,
	2343.0f,
	1.0f,
	413.0f,
	651.0f,
	65.0f,
	6165.0f,
	654.0f,
	614.0f,
	23.0f,
	8
};

MATH_tdst_Matrix align_mb __attribute__ ((aligned(16))) =
{
	616.0f,
	61.0f,
	161.0f,
	1.0f,
	21.0f,
	34.2f,
	843.9f,
	2344343.0f,
	151.0f,
	613.0f,
	651.0f,
	65.0f,
	1.0f,
	651.0f,
	614.0f,
	23.0f,
	8
};

MATH_tdst_Matrix align_mc __attribute__ ((aligned(16))) =
{
	616.0f,
	61.0f,
	161.0f,
	1.0f,
	21.0f,
	34.2f,
	843.9f,
	2344343.0f,
	151.0f,
	613.0f,
	651.0f,
	65.0f,
	6165.0f,
	1.0f,
	651.0f,
	614.0f,
	8
};

/*$4
 ***********************************************************************************************************************
    main
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */

inline void MoveIf(int Cnd, int * dst, int if_val, int else_val)
{
    asm __volatile__ ("
        movn    t0, %2, %0
        movz    t0, %3, %0
        sw      t0, 0(%1)
        ":: "r" (Cnd), "r" (dst), "r" (if_val), "r" (else_val) );
}





int main(int argc, char *argv[])
{

	printf("-- Bienvenu au bench PS2\n");
	MATH_InitModule();


	s_tst_MATHmatrix();
	s_tst_MATHfloat();
	s_tst_MATHvector();
	s_tst_Others();
	return 0;
}

/*$4
 ***********************************************************************************************************************
    static functions
 ***********************************************************************************************************************
 */


typedef struct  INT_tdst_Box_
{
    MATH_tdst_Vector    st_Min;
    MATH_tdst_Vector    st_Max;
} INT_tdst_Box;


inline MATH_tdst_Vector *OBJ_pst_BV_GetGMax(void *_pst_BV)
{
    return(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax);
}

inline MATH_tdst_Vector *OBJ_pst_BV_GetGMin(void *_pst_BV)
{
    return(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMin);
}

inline MATH_tdst_Vector *OBJ_pst_GetAbsolutePosition(OBJ_tdst_GameObject *_pst_Object)
{
	return(MATH_pst_GetTranslation(_pst_Object->pst_GlobalMatrix));
}


BOOL COL_RayAABBox
(
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	OBJ_tdst_GameObject *_pst_GO,
	float				*_pf_Dist,
	float				_f_MinDist
);
BOOL test(float fdir, float fmax, float fmin, float fend, float forigin);

BOOL COL_RayAABBox
(
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	OBJ_tdst_GameObject *_pst_GO,
	float				*_pf_Dist,
	float				_f_MinDist
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_Box		st_Box;
	MATH_tdst_Vector	st_End;
	int tt;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_AddVector(&st_Box.st_Max, OBJ_pst_BV_GetGMax(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
	MATH_AddVector(&st_Box.st_Min, OBJ_pst_BV_GetGMin(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));

	MATH_MulVector(&st_End, _pst_Direction, _f_MinDist);
	MATH_AddEqualVector(&st_End, _pst_Origin);

	tt = test(_pst_Direction->x, st_Box.st_Max.x, st_Box.st_Min.x, st_End.x, _pst_Origin->x   );
	tt &= test(_pst_Direction->y, st_Box.st_Max.y, st_Box.st_Min.y, st_End.y, _pst_Origin->y   );
	tt &= test(_pst_Direction->z, st_Box.st_Max.z, st_Box.st_Min.z, st_End.z, _pst_Origin->z   );
    
    if(!tt) return FALSE;
	/* The box must be tested */
	return TRUE;
	//return(INT_FullRayAABBox(_pst_Origin, _pst_Direction, &st_Box, NULL, _f_MinDist, _pf_Dist));
}

#define ifTestSup(fa, fb, ftempo)    ftempo =  fa-fb; if( ! (*(int*)&ftempo & 0x80000000) )

BOOL test(float fdir, float fmax, float fmin, float fend, float forigin)
{
    float ft;
    
    ifTestSup(fdir, 0, ft)
	{
		ifTestSup(forigin, fmax, ft) return FALSE;
		ifTestSup(fmin, fend, ft) return FALSE;
	}
	else
	{
		ifTestSup(fmin, forigin, ft) return FALSE;
		ifTestSup(fend, fmax, ft) return FALSE;
	}
    return TRUE;
}

static void s_tst_Others(void)
{
    BOOL bb;
    
	float fff = 10.0f;
	OBJ_tdst_GameObject GO;

	printf("***************\n");
	printf("-- others:\n");
	
	GO.pst_BV = malloc(sizeof(OBJ_tdst_SingleBV));
	((OBJ_tdst_SingleBV *)GO.pst_BV)->st_GMin = va;
    ((OBJ_tdst_SingleBV *)GO.pst_BV)->st_GMax = vb;
	
	GO.pst_GlobalMatrix = malloc(sizeof(MATH_tdst_Matrix));
	*GO.pst_GlobalMatrix = ma;
	
	M_BeginBentch(COL_RayAABBox);
    bb = COL_RayAABBox
    (
	&va,
	&vb,
	&GO,
	&fff,
	2.0f
    );
	M_EndBentch(COL_RayAABBox);
	printf("---------------\n");
	
    free(GO.pst_BV);
    free(GO.pst_GlobalMatrix);
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline LONG lRandok(LONG _l_min, LONG _l_max)
{
    float f;
    unsigned int i;
    
    i = (unsigned int)rand();
    f = (float)i / (float)RAND_MAX;
    f = f * (_l_max - _l_min) + _l_min;
 
    return lFloatToLong(f);
}

inline LONG lRandok1(LONG _l_min, LONG _l_max)
{
    float f;
    
    f = (float)rand();
    f = f / (float)RAND_MAX;
    f = f * (_l_max - _l_min) + _l_min;
 
    return lFloatToLong(f);
}

#define coefff  (float)(1.0f/ (float)RAND_MAX)
inline LONG lRandok2(LONG _l_min, LONG _l_max)
{
    float f;
    
    f = (float)rand();
    f = f * coefff;
    f = f * (_l_max - _l_min) + _l_min;
 
    return lFloatToLong(f);
}

inline LONG lRandok3(LONG _l_min, LONG _l_max)
{
    float f;
    int ij;

//    ij = _l_max - _l_min;
    f = fLongToFloat(rand()) * coefff * (fLongToFloat(_l_max) - fLongToFloat(_l_min));//ij) ;
    ij = lFloatToLong(f) + _l_min;
    return ij;
}

LONG lRandok4(LONG _l_min, LONG _l_max)
{
    float f;

    f = (float)rand()* coefff * (_l_max - _l_min) + _l_min;
 
    return lFloatToLong(f);
}

inline LONG lRand2(LONG _l_min, LONG _l_max)
{
    unsigned long i, lj;
    int jj;
    
    i = (unsigned long)rand();
    lj = (unsigned long)(_l_max - _l_min);
    i = i*lj;
    i = i/(unsigned long)RAND_MAX;
    jj = (int)i + _l_min;
 
    return jj;
}

inline LONG lRand22(LONG _l_min, LONG _l_max)
{
    unsigned int i, lj;
    int jj;
    float f;
    
    i = (unsigned int)rand();
    lj = (unsigned int)(_l_max - _l_min);
    i = i*lj;
    f = fLongToFloat(i) * coefff;
    jj = lFloatToLong(f) + _l_min;
 
    return jj;
}

inline float fRandd(float _f_min, float _f_max)
{
	/*~~~~~~*/
	float	f;
	/*~~~~~~*/

	/* Returns a random float between 0 and 1 */
//	f = fLongToFloat(rand()) / fLongToFloat(RAND_MAX);
	f = (fLongToFloat(rand()) * coefff* (_f_max - _f_min))+ _f_min;

	/* Puts it between the min and the max range */
	return(f  );
}
asm void init_Rand(float ff)
{
    .set noreorder
    mfc1        t0, $f12
    qmtc2       t0 ,$vf01
    vrinit      R, $vf01x
    jr          ra
    nop
    .set reorder
}

asm float v0_rand(void)
{
    .set noreorder
    vrnext.x    $vf01x, R
    qmfc2.i 	t0, $vf01
    mtc1        t0, $f0
    lui         t1,16256
    mtc1        t1,$f1
    nop
    sub.s    $f0,$f0,$f1
    jr          ra
    nop
    .set reorder
}

inline int lv0Rand(int _i_min, int _i_max)
{
	/*~~~~~~*/
	float	f;
	int i;
	/*~~~~~~*/

	f = (v0_rand() * fLongToFloat(_i_max - _i_min));
	i = lFloatToLong(f) + _i_min;
	return i;
}

_inline_ float ps2_i2f(int in)
{
    register float ft, out;
    asm __volatile__ ("mtc1     %1, %0" : "=f" (ft): "r" (in) );
    asm __volatile__ ("cvt.s.w  %0, %1" : "=f" (out): "f" (ft) );
    return out;
}

static void s_tst_MATHfloat(void)
{
    int kk;
    
	printf("***************\n");
	printf("-- MATHfloat:\n");
	a = 144.0f;
	b = 10.0f;
	sf = 15.0f;
	psf = &sf;

    /* Random */
    init_Rand(5.0f);

	M_BeginBentch(lRandok31);
	kk = lFloatToLong(fRandd(fLongToFloat(0), fLongToFloat(5)));
	M_EndBentch(lRandok31);

	M_BeginBentch(lv0Rand);
	kk = lv0Rand(0, 5);
	M_EndBentch(lv0Rand);

	M_BeginBentch(lRand2);
	kk = lRand2(0, 5);
	M_EndBentch(lRand2);

	M_BeginBentch(lRand22);
	kk = lRand22(0, 5);
	M_EndBentch(lRand22);

	M_BeginBentch(lRandok);
	kk = lRandok(0, 5);
	M_EndBentch(lRandok);
    
	M_BeginBentch(lRandok1);
	kk = lRandok1(0, 5);
	M_EndBentch(lRandok1);
    
	M_BeginBentch(lRandok3);
	kk = lRandok3(0, 5);
	M_EndBentch(lRandok3);

	M_BeginBentch(lRandok2);
	kk = lRandok2(0, 5);
	M_EndBentch(lRandok2);
    
	M_BeginBentch(lRandok4);
	kk = lRandok4(0, 5);
	M_EndBentch(lRandok4);

	M_BeginBentch(fRand);
	b = fRandd(0.0f, 5.0f);
	M_EndBentch(fRand);

    j = 123456;
    gk = 46465143;
    gl = 51614;
    
	/* ABS */
	M_Bentch(fabsf, c, a);
	M_Bentch(asm_fabs, c, a);
	M_Bentch(ps2_fabs, c, a);
	printf("---------------\n");

	/* CAST -> INT */
	M_Bentch(asm_f2i, j, a);
	M_Bentch(ps2_f2i, j, a);
	M_BeginBentch(natural_cast);
	j = (LONG) a;
	M_EndBentch(natural_cast);
	printf("---------------\n");

	/* CAST -> FLOAT */
	M_Bentch(ps2_i2f, a, j);
	M_BeginBentch(natural_castf);
	a = (float)( j+gk*gl);
	M_EndBentch(natural_castf);
	printf("---------------\n");

	/* DIVISION */
	M_BeginBentch(fDivTab);
	c = a * fInvTab(b);
	M_EndBentch(fDivTab);

	M_BeginBentch(natural_div);
	c = a / b;
	M_EndBentch(natural_div);
	printf("---------------\n");

	/* INVERSE */
	M_Bentch(fInvTab, c, b);
	M_BeginBentch(natural_inv);
	c = 1.0f / b;
	M_EndBentch(natural_inv);
	printf("---------------\n");

	/* SELF INVERSE */
	M_BeginBentch(vInvTab);
	vInvTab(psf);
	M_EndBentch(vInvTab);

	M_BeginBentch(natural_vInv);
	sf = 1.0f / sf;
	M_EndBentch(natural_vInv);
	printf("---------------\n");

	/* SQUARE ROOT */
	M_Bentch(sqrtf, c, a);
	M_Bentch(asm_fsqrt, c, a);
	M_Bentch(ps2_fsqrt, c, a);
	M_Bentch(fTabSqrt, c, a);
	printf("---------------\n");

	/* SELF SQUARE ROOT */
	sf = 15.0f;
	M_BeginBentch(vTabSqrt);
	vTabSqrt(psf);
	M_EndBentch(vTabSqrt);

	sf = 15.0f;
	M_BeginBentch(asm_vSqrt);
	asm_vsqrt(psf);
	M_EndBentch(asm_vSqrt);

	sf = 15.0f;
	M_BeginBentch(nat_vSqrt);
	*psf = asm_fsqrt(*psf);
	M_EndBentch(nat_vSqrt);
	printf("---------------\n");

	/* SELF REVERSE SQUARE ROOT */
	sf = 12345.045f;
	M_BeginBentch(asm_vrsqrt);
	asm_vrsqrt(psf, 1.0f);
	M_EndBentch(asm_vrsqrt);

	sf = 12345.045f;
	M_BeginBentch(nat_vrsqrt);
	*psf = 1.0f / asm_fsqrt(*psf);
	M_EndBentch(nat_vrsqrt);

	sf = 12345.045f;
	M_BeginBentch(nat2_vrsqrt);
	*psf = asm_frsqrt(1.0f, *psf);
	M_EndBentch(nat2_vrsqrt);

	sf = 12345.045f;
	M_BeginBentch(ps2_frsqrt);
	*psf = ps2_frsqrt(*psf);
	M_EndBentch(ps2_frsqrt);

	sf = 12345.045f;
	M_BeginBentch(vTabInvSqrt);
	vTabInvSqrt(psf);
	M_EndBentch(vTabInvSqrt);
	printf("---------------\n");

	/* RECIPROCAL SQUARE ROOT */
	sf = 12345.045f;
	M_BeginBentch(asm_frsqrt);
	c = asm_frsqrt(1.0f, sf);
	M_EndBentch(asm_frsqrt);

	sf = 12345.045f;
	M_BeginBentch(nat_frsqrt);
	c = 1.0f / asm_fsqrt(sf);
	M_EndBentch(nat_frsqrt);

	sf = 12345.045f;
	M_BeginBentch(ps2_frsqrt);
	c = ps2_fsqrt(sf);
	M_EndBentch(ps2_frsqrt);

	sf = 12345.045f;
	M_BeginBentch(fTabInvSqrt);
	c = fTabInvSqrt(sf);
	M_EndBentch(fTabInvSqrt);
	printf("---------------\n");

	/* MIN, MAX */
	M_BeginBentch(nat_min);
	c = (a <= b) ? a : b;
	M_EndBentch(nat_min);

	M_BeginBentch(asm_Min);
	c = asm_Min(a, b);
	M_EndBentch(asm_Min);

	M_BeginBentch(ps2_Min);
	c = ps2_min(a, b);
	M_EndBentch(ps2_Min);
	printf("---------------\n");

	M_BeginBentch(nat_max);
	c = (a >= b) ? a : b;
	M_EndBentch(nat_max);

	M_BeginBentch(asm_Max);
	c = asm_Max(a, b);
	M_EndBentch(asm_Max);

	M_BeginBentch(ps2_Max);
	c = ps2_max(a, b);
	M_EndBentch(ps2_Max);
	printf("---------------\n");

	/* BLEND */
	c = 0.17f;
	M_BeginBentch(SimpleBlend);
	sf = SimpleBlend(a, b, c);
	M_EndBentch(SimpleBlend);

	c = 0.17f;
	sf = asm_Blend(a, b, c);
	M_BeginBentch(asm_Blend);
	sf = asm_Blend(a, b, c);
	M_EndBentch(asm_Blend);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void MATH_AbsVector2(MATH_tdst_Vector *_pst_Mdst, MATH_tdst_Vector *_pst_Msrc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector tmp	__attribute__((aligned(16)));
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	tmp = *_pst_Msrc;

	*(long *) &tmp = *(long *) &tmp & 0x7fffffff7fffffff;
	*(int *) &tmp.z &= 0x7fffffff;

	*_pst_Mdst = tmp;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_CrossProductOld(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, MATH_tdst_Vector *B);
void MATH_TransformVectorOld(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc);
void MATH_TransformVectorNoScaleOld(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc);
void MATH_TransformVertexOld(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc);
 
static void s_tst_MATHvector(void)
{
	float ff;
	
	printf("***************\n");
	printf("-- MATHvector:\n");

	va.x = 12.0f;
	va.y = 34.0f;
	va.z = -56.7f;
	vb.x = 90.0f;
	vb.y = -87.0f;
	vb.z = 65.7f;
	vc.x = 19.0f;
	vc.y = -38.0f;
	vc.z = 6789.7f;

	align_va = va;
	align_vb = vb;
	align_vc = vc;

	/* ABS */

	M_BeginBentch(MATH_AbsVector2);
	MATH_AbsVector2(&vc, &va);
	M_EndBentch(MATH_AbsVector2);

	M_BeginBentch(asm_AbsAlignedVector);
	asm_AbsAlignedVector(&align_vc, &align_va);
	M_EndBentch(asm_AbsAlignedVector);
	printf("---------------\n");

	M_BeginBentch(MATH_TransformVectorOld);
	MATH_TransformVectorOld(&vb, &va, &ma);
	M_EndBentch(MATH_TransformVectorOld);

	M_BeginBentch(MATH_TransformVector);
	MATH_TransformVector(&vb, &ma, &va);
	M_EndBentch(MATH_TransformVector);
	printf("---------------\n");

	M_BeginBentch(MATH_TransformVertexOld);
	MATH_TransformVertexOld(&vb, &va, &ma);
	M_EndBentch(MATH_TransformVertexOld);

	M_BeginBentch(MATH_TransformVertex);
	MATH_TransformVertex(&vb, &ma, &va);
	M_EndBentch(MATH_TransformVertex);
	printf("---------------\n");

	M_BeginBentch(MATH_TransformVectorNoScaleOld);
	MATH_TransformVectorNoScaleOld(&vb, &ma, &va);
	M_EndBentch(MATH_TransformVectorNoScaleOld);

	M_BeginBentch(MATH_TransformVectorNoScale);
	MATH_TransformVectorNoScale(&vb, &ma, &va);
	M_EndBentch(MATH_TransformVectorNoScale);

	M_BeginBentch(MATH_TransformVertexNoScale);
	MATH_TransformVertexNoScale(&vb, &ma, &va);
	M_EndBentch(MATH_TransformVertexNoScale);
	printf("---------------\n");

	M_BeginBentch(MATH_TransformHomVector);
	MATH_TransformHomVector(&vb, &ma, &va);
	M_EndBentch(MATH_TransformHomVector);
	printf("---------------\n");

	M_BeginBentch(MATH_CrossProduct);
	MATH_CrossProduct(&vc, &vb, &va);
	M_EndBentch(MATH_CrossProduct);

	M_BeginBentch(MATH_CrossProductOld);
	MATH_CrossProductOld(&vc, &vb, &va);
	M_EndBentch(MATH_CrossProductOld);
	printf("---------------\n");	

	M_BeginBentch(MATH_f_SqrNormVector);
	ff = MATH_f_SqrNormVector(&va);
	M_EndBentch(MATH_f_SqrNormVector);
	printf("---------------\n");	

	M_BeginBentch(MATH_VectorGlobalToLocal);
	MATH_VectorGlobalToLocal(&va, &ma, &vc);
	M_EndBentch(MATH_VectorGlobalToLocal);
	printf("---------------\n");	

	M_BeginBentch(MATH_VertexGlobalToLocal);
	MATH_VertexGlobalToLocal(&va, &ma, &vc);
	M_EndBentch(MATH_VertexGlobalToLocal);
	printf("---------------\n");	

}

typedef struct	t44Matrix_
{
	long128 V1, V2, V3, V4;
	int		i;
} t44Matrix;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_MulMatrixMatrixOld(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2);
void MATH_MulMatrixMatrix2(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2);
 
void mMATH_CopyMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M)
{
	/*~~~~~~~~~~~~~~~~~*/
	LONG	*lSrc, *lDst;
	/*~~~~~~~~~~~~~~~~~*/


	if(MDst != M)
	{
		if(!((int) MDst % 16) && !((int) M % 16))
			*MDst = *M;
		else
		{
			lSrc = (LONG *) M;
			lDst = (LONG *) MDst;
			*lDst = *lSrc;
			*(lDst + 1) = *(lSrc + 1);
			*(lDst + 2) = *(lSrc + 2);
			*(lDst + 3) = *(lSrc + 3);
			*(lDst + 4) = *(lSrc + 4);
			*(lDst + 5) = *(lSrc + 5);
			*(lDst + 6) = *(lSrc + 6);
			*(lDst + 7) = *(lSrc + 7);
			*(lDst + 8) = *(lSrc + 8);
			*(lDst + 9) = *(lSrc + 9);
			*(lDst + 10) = *(lSrc + 10);
			*(lDst + 11) = *(lSrc + 11);
			*(lDst + 12) = *(lSrc + 12);
			*(lDst + 13) = *(lSrc + 13);
			*(lDst + 14) = *(lSrc + 14);
			*(lDst + 15) = *(lSrc + 15);
			*(lDst + 16) = *(lSrc + 16);
		}
	}

}

asm void mmMATH_CopyMatrix(MATH_tdst_Matrix *dst, MATH_tdst_Matrix *src)
{
    .set noreorder
    lw  t0, 0(a1);  lw   t1, 4(a1)
    sw  t0, 0(a0);  sw   t1, 4(a0)

    lw  t0, 8(a1);  lw   t1, 12(a1)
    sw  t0, 8(a0);  sw   t1, 12(a0)

    lw  t0, 16(a1);  lw   t1, 20(a1)
    sw  t0, 16(a0);  sw   t1, 20(a0)

    lw  t0, 24(a1);  lw   t1, 28(a1)
    sw  t0, 24(a0);  sw   t1, 28(a0)
    
    lw  t0, 32(a1);  lw   t1, 36(a1)
    sw  t0, 32(a0);  sw   t1, 36(a0)

    lw  t0, 40(a1);  lw   t1, 44(a1)
    sw  t0, 40(a0);  sw   t1, 44(a0)

    lw  t0, 48(a1);  lw   t1, 52(a1)
    sw  t0, 48(a0);  sw   t1, 52(a0)

    lw  t0, 56(a1);  lw   t1, 60(a1)
    sw  t0, 56(a0);  sw   t1, 60(a0)

    lw  t0, 64(a1);  jr     ra
    sw  t0, 64(a0)
    .set reorder
    
/*    register    int i0, i1;

    asm __volatile__ ("lw   %0, 0(%1)" : "=r" (i0) : "r" (src));
    asm __volatile__ ("lw   %0, 4(%1)" : "=r" (i1) : "r" (src));
    asm __volatile__ ("sw   %0, 0(%1)" : : "r" (i0), "r" (dst));
    asm __volatile__ ("sw   %0, 4(%1)" : : "r" (i1), "r" (dst));

    asm __volatile__ ("lw   %0, 8(%1)" : "=r" (i0) : "r" (src));
    asm __volatile__ ("lw   %0, 12(%1)" : "=r" (i1) : "r" (src));
    asm __volatile__ ("sw   %0, 8(%1)" : : "r" (i0), "r" (dst));
    asm __volatile__ ("sw   %0, 12(%1)" : : "r" (i1), "r" (dst));

    asm __volatile__ ("lw   %0, 16(%1)" : "=r" (i0) : "r" (src));
    asm __volatile__ ("lw   %0, 20(%1)" : "=r" (i1) : "r" (src));
    asm __volatile__ ("sw   %0, 16(%1)" : : "r" (i0), "r" (dst));
    asm __volatile__ ("sw   %0, 20(%1)" : : "r" (i1), "r" (dst));

    asm __volatile__ ("lw   %0, 24(%1)" : "=r" (i0) : "r" (src));
    asm __volatile__ ("lw   %0, 28(%1)" : "=r" (i1) : "r" (src));
    asm __volatile__ ("sw   %0, 24(%1)" : : "r" (i0), "r" (dst));
    asm __volatile__ ("sw   %0, 28(%1)" : : "r" (i1), "r" (dst));

    asm __volatile__ ("lw   %0, 32(%1)" : "=r" (i0) : "r" (src));
    asm __volatile__ ("lw   %0, 36(%1)" : "=r" (i1) : "r" (src));
    asm __volatile__ ("sw   %0, 32(%1)" : : "r" (i0), "r" (dst));
    asm __volatile__ ("sw   %0, 36(%1)" : : "r" (i1), "r" (dst));

    asm __volatile__ ("lw   %0, 40(%1)" : "=r" (i0) : "r" (src));
    asm __volatile__ ("lw   %0, 44(%1)" : "=r" (i1) : "r" (src));
    asm __volatile__ ("sw   %0, 40(%1)" : : "r" (i0), "r" (dst));
    asm __volatile__ ("sw   %0, 44(%1)" : : "r" (i1), "r" (dst));

    asm __volatile__ ("lw   %0, 48(%1)" : "=r" (i0) : "r" (src));
    asm __volatile__ ("lw   %0, 52(%1)" : "=r" (i1) : "r" (src));
    asm __volatile__ ("sw   %0, 48(%1)" : : "r" (i0), "r" (dst));
    asm __volatile__ ("sw   %0, 52(%1)" : : "r" (i1), "r" (dst));

    asm __volatile__ ("lw   %0, 56(%1)" : "=r" (i0) : "r" (src));
    asm __volatile__ ("lw   %0, 60(%1)" : "=r" (i1) : "r" (src));
    asm __volatile__ ("sw   %0, 56(%1)" : : "r" (i0), "r" (dst));
    asm __volatile__ ("sw   %0, 60(%1)" : : "r" (i1), "r" (dst));

    asm __volatile__ ("lw   %0, 64(%1)" : "=r" (i0) : "r" (src));
    asm __volatile__ ("sw   %0, 64(%1)" : : "r" (i0), "r" (dst));
*/}

_inline_ void mmmMATH_CopyMatrix(MATH_tdst_Matrix *dst, MATH_tdst_Matrix *src)
{
    L_memcpy(dst, src, sizeof(MATH_tdst_Matrix));
}
_inline_ void m2MATH_CopyMatrix(MATH_tdst_Matrix *dst, MATH_tdst_Matrix *src)
{
    int pp;
    for(pp=0; pp<17; pp++)
        ((int*)dst)[pp] = ((int*)src)[pp];
}

static void s_tst_MATHmatrix(void)
{
	printf("***************\n");
	printf("-- MATHmatrix:\n");

	/* COPY MATRIX */
	M_BeginBentch(mMATH_CopyMatrix);
	mMATH_CopyMatrix(&mc, &ma);
	M_EndBentch(mMATH_CopyMatrix);

	M_BeginBentch(m2MATH_CopyMatrix);
	m2MATH_CopyMatrix(&mc, &ma);
	M_EndBentch(m2MATH_CopyMatrix);

	M_BeginBentch(mmMATH_CopyMatrix);
	mmMATH_CopyMatrix(&mc, &ma);
	M_EndBentch(mmMATH_CopyMatrix);

	M_BeginBentch(mmmMATH_CopyMatrix);
	mmmMATH_CopyMatrix(&mc, &ma);
	M_EndBentch(mmmMATH_CopyMatrix);

	M_BeginBentch(MATH_CopyMatrix);
	MATH_CopyMatrix(&mc, &ma);
	M_EndBentch(MATH_CopyMatrix);
	printf("---------------\n");

	M_BeginBentch(MATH_MulMatrixMatrixOld);
	MATH_MulMatrixMatrixOld(&mc, &mb, &ma);
	M_EndBentch(MATH_MulMatrixMatrixOld);

	M_BeginBentch(MATH_MulMatrixMatrix);
	MATH_MulMatrixMatrix(&mc, &mb, &ma);
	M_EndBentch(MATH_MulMatrixMatrix);
	printf("---------------\n");

	M_BeginBentch(MATH_Copy33Matrix);
	MATH_Copy33Matrix(&mc, &ma);
	M_EndBentch(MATH_Copy33Matrix);
	printf("---------------\n");

	M_BeginBentch(MATH_GetRotationMatrix);
	MATH_GetRotationMatrix(&mc, &ma);
	M_EndBentch(MATH_GetRotationMatrix);

	M_BeginBentch(asm_MATH_GetRotationMatrix);
	asm_MATH_GetRotationMatrix(&mc, &ma);
	M_EndBentch(asm_MATH_GetRotationMatrix);

	M_BeginBentch(asm_MATH_GetRotationMatrix2);
	asm_MATH_GetRotationMatrix2(&mc, &ma);
	M_EndBentch(asm_MATH_GetRotationMatrix2);
	printf("---------------\n");

	M_BeginBentch(MATH_Orthonormalize);
	MATH_Orthonormalize(&ma);
	M_EndBentch(MATH_Orthonormalize);
	printf("---------------\n");
	
	M_BeginBentch(MATH_MatrixBlend);
	MATH_MatrixBlend(&mc, &mb, &ma, 0.7f, 1);
	M_EndBentch(MATH_MatrixBlend);

	M_BeginBentch(MATH_MatrixBlend);
	MATH_MatrixBlend(&mc, &mb, &ma, 0.7f, 0);
	M_EndBentch(MATH_MatrixBlend);
	printf("---------------\n");
	
	M_BeginBentch(MATH_AddTranslation);
	MATH_AddTranslation(&mc, &mb, &ma);
	M_EndBentch(MATH_AddTranslation);
	printf("---------------\n");
	
}

void MATH_MulMatrixMatrixOld(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2)
{

	MATH_tdst_Matrix	A, B;
	MATH_tdst_Vector	st_Scale;


	MATH_SetType(MDst, 0);

    if ( !(MATH_b_TestScaleType(M1) || MATH_b_TestScaleType(M2)) )
    {
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
        MDst->w = 1;
    }
    else
    {
	    MATH_MakeOGLMatrix(&A, M1);
	    MATH_MakeOGLMatrix(&B, M2);

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

	    MATH_GetScaleFrom33Matrix(&st_Scale, MDst);
	    MATH_SetScale(MDst, &st_Scale);


	    if(MATH_b_TestScaleType(MDst))
	    {
		    MATH_InvEqualVector(&st_Scale);
		    MATH_Scale33Matrix(MDst, MDst, &st_Scale);
	    }
    }


	if(MATH_b_TestRotationType(M1) || MATH_b_TestRotationType(M2)) 
        MATH_SetRotationType(MDst);

	
    MATH_UpdateTranslationType(MDst);
} //*/

void MATH_TransformVectorOld(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
	float	fx, fy;

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
	//*/
}

void MATH_CrossProductOld(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{

	Dest->x = fMul(A->y, B->z) - fMul(A->z, B->y);
	Dest->y = fMul(A->z, B->x) - fMul(A->x, B->z);
	Dest->z = fMul(A->x, B->y) - fMul(A->y, B->x);
}

void MATH_TransformVectorNoScaleOld(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
	float	fx, fy;

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

void MATH_TransformVertexOld(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
{
	MATH_TransformVector(VDst, M, VSrc);
	MATH_AddVector(VDst, VDst, &(M->T));
}


#ifdef __cplusplus
}
#endif
#endif

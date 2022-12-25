
//#if defined (JADEFUSION)
//#include "Precomp.h"
//#endif
#ifdef JADEFUSION
#include "projectconfig.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIxxx.h"
#endif

#include "BASe/BAStypes.h"
#include "lcpmath.h"
#include "lcperr.h"

#include <assert.h>
#include <string.h> // memmove()
#ifdef PSX2_TARGET
#include <libvu0.h> 


//#ifdef PS2
  //  #include "lcp_vu0.h"
//#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" {
#endif
asm void *ps2memcpy(char *dst, char*src, int size);
asm void *ps2memset(char *dst, unsigned char c, int size);
#if defined (__cplusplus) && !defined(JADEFUSION)
}

#endif

#endif


#ifdef PS2_DEBUG
    #include "debugvariables.h"
#endif

#define OPTIM_WHEN_N_INFERIOR_8

/* constants */

/* pi and 1/sqrt(2) are defined here if necessary because they don't get
 * defined in <math.h> on some platforms (like MS-Windows)
 */

#ifndef M_PI
#define M_PI REAL(3.1415926535897932384626433832795029)
#endif

#define M_Sqrt1_2 0.70710678118654752440084436210485f

#ifndef M_SQRT1_2
#define M_SQRT1_2 REAL(0.7071067811865475244008443621048490)
#endif

#ifdef JADEFUSION
#if !defined(dAASSERT)
#define dAASSERT assert
#endif
#if !defined(dIASSERT)
#define dIASSERT assert 
#endif.
#else
#define dAASSERT assert
#define dIASSERT assert 
#endif

#define _FASTSOLVE_

/*
// sv. (debug)
#include "PointArray.h"
extern PointArrayOrd oSerie;
*/

#ifdef PS2
	#define PS2MACRO
#endif

//#include "MTH_CPoint4.h"

#ifdef GCN
	#define SolveL1(x,y,n,z)     if(n==3) LCPMath::dSolveL13(x,y,z); \
								 else if(n==4) LCPMath::dSolveL14(x,y,z); \
								 else if(n==5) LCPMath::dSolveL15(x,y,z); \
								 else if(n==6) LCPMath::dSolveL16(x,y,z); \
								 else LCPMath::dSolveL1(x,y,n,z);
#else
	#define SolveL1(x,y,n,z)	 LCPMath::dSolveL1(x,y,n,z);
#endif

#ifdef GCN
	#define SolveL1T(x,y,n,z)    if(n==3) LCPMath::dSolveL1T3(x,y,z); \
								 else if(n==4) LCPMath::dSolveL1T4(x,y,z); \
								 else if(n==5) LCPMath::dSolveL1T5(x,y,z); \
								 else if(n==6) LCPMath::dSolveL1T6(x,y,z); \
								 else LCPMath::dSolveL1T(x,y,n,z);
#else
	#define SolveL1T(x,y,n,z)	 LCPMath::dSolveL1T(x,y,n,z);
#endif


dReal LCPMath::dDot (const dReal *a, const dReal *b, int n)
{  
// sv. (debug)
//	oSerie.Add( (float) n , 1.0f);
	
	/*
#ifndef PS2MACRO

	float sum =0.0f;
	MTH_CPoint4 pa, pb;
	int s, steps = n / 4, remain = n % 4;
	for (s =0; s <steps; s++)
	{
		pa.Set(a[0], a[1], a[2], a[3]);
		pb.Set(b[0], b[1], b[2], b[3]);
		sum += pa.fDot(pb);
		a+=4;
		b+=4;
	}	
	if (remain > 0)
	{
		pa.Set(a[0],0,0,0);
		pb.Set(b[0],0,0,0);		
		if (remain > 1)
		{
			pa.SetY(a[1]);
			pb.SetY(b[1]);
			if (remain > 2)
			{
				pa.SetZ(a[2]);
				pb.SetZ(b[2]);								
			}
		}
		sum += pa.fDot(pb);
	}	

#else	
	int iqwords, ii, j;
	sceVu0FVECTOR p1;			
	sceVu0FVECTOR p2;
	sceVu0FVECTOR dest;
	float sum =0.0f;
	iqwords = n / 4;
	if (iqwords > 0)					
		for (ii=0, j=0; j< iqwords; j++, ii+=4)
		{					
			p1[0] = a[ii];
			p1[1] = a[ii+1];
			p1[2] = a[ii+2];
			p1[3] = a[ii+3];
			p2[0] = b[ii];
			p2[1] = b[ii+1];
			p2[2] = b[ii+2];
			p2[3] = b[ii+3];
			asm __volatile__("
    			lqc2        vf1,p1
    			lqc2        vf2,p2
				vmul.xyzw	vf3, vf1,vf2								
				vaddy.x 	vf3, vf3, vf3	# vf3.x = vf3.x + vf3.y
				vaddz.x 	vf3, vf3, vf3	# vf3.x = vf3.x + vf3.z
				vaddw.x 	vf3, vf3, vf3	# vf3.x = vf3.x + vf3.w				
				
				sqc2        vf3,dest
			": : "r" (dest) , "r" (p1), "r" (p2));
			
//			sum += ( dest[0] + dest[1] + dest[2] + dest[3]);
			sum += dest[0];
		}		

	//	EPILOGUE
	switch ( n % 4)
	{
	case 3:
		sum += a[n-3]*b[n-3];
	case 2:
		sum += a[n-2]*b[n-2];
	case 1:
		sum += a[n-1]*b[n-1];
		break;
	}	

#endif
*/
		
	// CHECK CODE: limited 10-digit precision
	float sum  =0.0f;
	int nn= n;
	while (nn > 0)
	{
		sum += a[0] * b[0];
		a++;
		b++;
		nn--;
	}


/*	if (fSum2!=sum)
	{
		printf("dotc = %.16f dotm = %.16f\n", fSum2, sum);
		assert(0);
	}
	*/

	return sum;	
}

void LCPMath::dSetZero (dReal *a, int n)
{
  dAASSERT (a && n >= 0);
  
#ifdef PS2MACRO
	int iqwords = n/4;
	if ( iqwords > 0 )
		memset128 ((u_long128 *)a, 0, iqwords);	
	switch (n % 4)
	{
	case 3:
		a[n-3] = 0;
	case 2:		
		a[n-2] = 0;
	case 1:
		a[n-1] = 0;
		break;
	}
#else
  while (n > 0) 
  {
    *a = 0;
	a++;
    n--;
  }
#endif 
}

#ifdef _FASTSOLVE_

void LCPMath::dSolveL1_1 (const dReal *L, dReal *B, int n, int lskip1)
{  
  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,Z21, *ex;
  const dReal *ell;
  int i,j;
#ifdef GCN
	register const float* rel0;
	register const float* rels;
	register const float* rex;
#endif
  /* compute all 2 x 1 blocks of X */
  for (i=0; i < n; i+=2) {
    /* compute all 2 x 1 block of X, from rows i..i+2-1 */
    /* set the Z matrix to 0 */
    Z11 = Z21 = 0;
    ell = L + i * lskip1;
    ex  = B;
    /* the inner loop that computes outer products and adds them to Z */
#ifdef GCN
	static float a2fZero[2] = {0.0f, 0.0f};
	register const float* zero = a2fZero;
	register const float* z11 = &Z11;
	register const float* z21 = &Z21;
	asm
	{
		psq_l	fp15, 0(zero), 0, 0;
		psq_l	fp16, 0(zero), 0, 0;
	}
	// Z11 will be fp15
	// Z21 will be fp16
#endif
    for (j=i-2; j >= 0; j -= 2) 
    {
	#ifdef GCN
		rel0 = &ell[0];
		rex = &ex[0];
		rels = &ell[lskip1];
		asm
		{
			psq_l	fp0, 0(rel0), 0, 0;		// fp0 <- e0,e1
			psq_l	fp1, 0(rels), 0, 0;		// fp1 <- es0,es1
			psq_l	fp2, 0(rex), 0, 0;		// fp2 <- ex0,ex1
			ps_mul	fp3, fp0, fp2;			// fp3 <- e0ex0,e1ex1
			ps_mul	fp4, fp1, fp2;			// fp4 <- es0ex0,es1ex1
			ps_add	fp15, fp15, fp3;
			ps_add	fp16, fp16, fp4;
		}
      ell += 2;
      ex  += 2;
	#else
      /* compute outer product and add it to the Z matrix */
      Z11 += ell[0] * ex[0];
      Z21 += ell[lskip1] * ex[0];
      /* compute outer product and add it to the Z matrix */
      Z11 += ell[1] * ex[1];
      Z21 += ell[1 + lskip1] * ex[1];
      /* advance pointers */
      ell += 2;
      ex  += 2;
      /* end of inner loop */
	#endif
    }
#ifdef GCN
	asm
	{
		ps_sum0	fp15, fp15, fp15, fp15;
		ps_sum0	fp16, fp16, fp16, fp16;
		psq_st	fp15, 0(z11), 1, 0;
		psq_st	fp16, 0(z21), 1, 0;
	}
#endif
    /* compute left-over iterations */
    j += 2;
    for (; j > 0; j--) {
      /* compute outer product and add it to the Z matrix */
      Z11 += ell[0] * ex[0];
      Z21 += ell[lskip1] * ex[0];
      /* advance pointers */
      ell++;
      ex++;
    }
    /* finish computing the X(i) block */
    ex[0] -= Z11;
    ex[1] -= (Z21 + ell[lskip1] * ex[0]);
    /* end of outer loop */
  }
}

/* solve L*X=B, with B containing 2 right hand sides.
 * L is an n*n lower triangular matrix with ones on the diagonal.
 * L is stored by rows and its leading dimension is lskip.
 * B is an n*2 matrix that contains the right hand sides.
 * B is stored by columns and its leading dimension is also lskip.
 * B is overwritten with X.
 * this processes blocks of 2*2.
 * if this is in the factorizer source file, n must be a multiple of 2.
 */

void LCPMath::dSolveL1_2 (const dReal *L, dReal *B, int n, int lskip1)
{  
  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,Z12,Z21,Z22,*ex;
  const dReal *ell;
  int i,j;
  /* compute all 2 x 2 blocks of X */
  for (i=0; i < n; i+=2) {
    /* compute all 2 x 2 block of X, from rows i..i+2-1 */
    /* set the Z matrix to 0 */
    Z11 = Z12 = Z21 = Z22 = 0;
    ell = L + i*lskip1;
    ex  = B;
    /* the inner loop that computes outer products and adds them to Z */
    for (j=i-2; j >= 0; j -= 2) {
      /* compute outer product and add it to the Z matrix */
      Z11 += ell[0] * ex[0];
      Z12 += ell[0] * ex[lskip1];
      Z21 += ell[lskip1] * ex[0];
      Z22 += ell[lskip1] * ex[lskip1];
      /* compute outer product and add it to the Z matrix */
      Z11 += ell[1] * ex[1];
      Z12 += ell[1] * ex[1 + lskip1];
      Z21 += ell[1 + lskip1] * ex[1];
      Z22 += ell[1 + lskip1] * ex[1 + lskip1];
      /* advance pointers */
      ell += 2;
      ex  += 2;
      /* end of inner loop */
    }
    /* compute left-over iterations */
    j += 2;
    for (; j > 0; j--) {
      /* compute outer product and add it to the Z matrix */
      Z11 += ell[0] * ex[0];
      Z12 += ell[0] * ex[lskip1];
      Z21 += ell[lskip1] * ex[0];
      Z22 += ell[lskip1] * ex[lskip1];
      /* advance pointers */
      ell++;
      ex++;
    }
    /* finish computing the X(i) block */
    ex[0] -= Z11;
    ex[lskip1] -= Z12;
    ex[1] -= (Z21 + ell[lskip1] * ex[0]);
    ex[1 + lskip1] -= (Z22 + ell[lskip1] * ex[lskip1]);
    /* end of outer loop */
  }
}

#ifdef GCN
void LCPMath::dFactorLDLT3 (dReal *A, dReal *d, int nskip1)
{
  dReal sum,*ell,*dee,q1,q2,Z11,Z21,Z22;

	// Z11 <- fp15

	register const float* el0;
	register const float* de;
	register const float* z11 = &Z11;

	// i = 0
	/* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
	dSolveL1_2 (A, A, 0, nskip1);
	/* scale the elements in a 2 x i block at A(i,0), and also */
	/* compute Z = the outer product matrix that we'll need. */
	Z11 = Z21 = Z22 = 0;
	ell = A;
	dee = d;
	// Nothing to do

    /* compute left-over iterations */
	// Nothing to do

	/* solve for diagonal 2 x 2 block at A(i,i) */
	Z11 = ell[0] - Z11;
	Z21 = ell[nskip1] - Z21;
	Z22 = ell[1+nskip1] - Z22;
	dee = d;
	/* factorize 2 x 2 block Z,dee */
	/* factorize row 1 */
	dee[0] = dRecip(Z11);
	/* factorize row 2 */
	sum = 0;
	q1 = Z21;
	q2 = q1 * dee[0];
	Z21 = q2;
	sum += q1 * q2;
	dee[1] = dRecip(Z22 - sum);
	/* done factorizing 2 x 2 block */
	ell[nskip1] = Z21;

	// switch code
  // At the end of the first for, when n=3 i=2
  /* compute the (less than 2) rows at the bottom */
    dSolveL1_1 (A, A + (nskip1<<1), 2, nskip1);
    /* scale the elements in a 1 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = 0;
    ell = A + (nskip1<<1);
    dee = d;
    /* compute left-over iterations */
	el0 = &ell[0];
	de = &dee[0];
	asm
	{
		psq_l	fp0, 0(el0), 0 , 0;
		psq_l	fp1, 0(de), 0, 0;
		ps_mul	fp2, fp0, fp1;
		ps_mul	fp3, fp0, fp2;
		psq_st	fp2, 0(el0), 0, 0;
		ps_sum0 fp15, fp3, fp3, fp3;
		psq_st	fp15, 0(z11), 1, 0;
	}

    /* solve for diagonal 1 x 1 block at A(i,i) */
    Z11 = ell[2] - Z11;
    dee = d + 2;
    /* factorize 1 x 1 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* done factorizing 1 x 1 block */
}
#else
void LCPMath::dFactorLDLT3 (dReal *A, dReal *d, int nskip1)
{
  dReal sum,*ell,*dee,q1,q2,Z11,Z21,Z22;

	// i = 0
	/* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
	dSolveL1_2 (A, A, 0, nskip1);
	/* scale the elements in a 2 x i block at A(i,0), and also */
	/* compute Z = the outer product matrix that we'll need. */
	Z11 = Z21 = Z22 = 0;
	ell = A;
	dee = d;
	// Nothing to do

    /* compute left-over iterations */
	// Nothing to do

	/* solve for diagonal 2 x 2 block at A(i,i) */
	Z11 = ell[0] - Z11;
	Z21 = ell[nskip1] - Z21;
	Z22 = ell[1+nskip1] - Z22;
	dee = d;
	/* factorize 2 x 2 block Z,dee */
	/* factorize row 1 */
	dee[0] = dRecip(Z11);
	/* factorize row 2 */
	sum = 0;
	q1 = Z21;
	q2 = q1 * dee[0];
	Z21 = q2;
	sum += q1 * q2;
	dee[1] = dRecip(Z22 - sum);
	/* done factorizing 2 x 2 block */
	ell[nskip1] = Z21;

	// switch code
  // At the end of the first for, when n=3 i=2
  /* compute the (less than 2) rows at the bottom */
    dSolveL1_1 (A, A + (nskip1<<1), 2, nskip1);
    /* scale the elements in a 1 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = 0;
    ell = A + (nskip1<<1);
    dee = d;
    /* compute left-over iterations */
    q1 = ell[0] * dee[0];
    Z11 += ell[0] * q1;
    ell[0] = q1;
    q1 = ell[1] * dee[1];
    Z11 += ell[1] * q1;
    ell[1] = q1;

    /* solve for diagonal 1 x 1 block at A(i,i) */
    Z11 = ell[2] - Z11;
    dee = d + 2;
    /* factorize 1 x 1 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* done factorizing 1 x 1 block */
}
#endif

void LCPMath::dFactorLDLT4 (dReal *A, dReal *d, int nskip1)
{
  dReal sum,*ell,*dee,q1,q2,Z11,Z21,Z22;

	// i = 0
	/* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
	dSolveL1_2 (A, A, 0, nskip1);
	/* scale the elements in a 2 x i block at A(i,0), and also */
	/* compute Z = the outer product matrix that we'll need. */
	Z11 = Z21 = Z22 = 0;
	ell = A;
	dee = d;
	// Nothing to do

    /* compute left-over iterations */
	// Nothing to do

	/* solve for diagonal 2 x 2 block at A(i,i) */
	Z11 = ell[0] - Z11;
	Z21 = ell[nskip1] - Z21;
	Z22 = ell[1+nskip1] - Z22;
	dee = d;
	/* factorize 2 x 2 block Z,dee */
	/* factorize row 1 */
	dee[0] = dRecip(Z11);
	/* factorize row 2 */
	sum = 0;
	q1 = Z21;
	q2 = q1 * dee[0];
	Z21 = q2;
	sum += q1 * q2;
	dee[1] = dRecip(Z22 - sum);
	/* done factorizing 2 x 2 block */
	ell[nskip1] = Z21;

	// i=2
    /* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
    dSolveL1_2 (A, A + (nskip1<<1), 2, nskip1);
    /* scale the elements in a 2 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = Z21 = Z22 = 0;
    ell = A + (nskip1<<1);
    dee = d;
    /* compute left-over iterations */
	// for with j=2, so 2 "iteraciones desenrolladas"
	q1 = ell[0] * dee[0];
	q2 = ell[nskip1] * dee[0];
	Z11 += ell[0] * q1;
	Z21 += ell[nskip1] * q1;
	Z22 += ell[nskip1] * q2;
	ell[0] = q1;
	ell[nskip1] = q2;
	q1 = ell[1] * dee[1];
	q2 = ell[nskip1+1] * dee[1];
	Z11 += ell[1] * q1;
	Z21 += ell[nskip1+1] * q1;
	Z22 += ell[nskip1+1] * q2;
	ell[1] = q1;
	ell[nskip1+1] = q2;
	ell+=2;
	dee+=2;
    /* solve for diagonal 2 x 2 block at A(i,i) */
    Z11 = ell[0] - Z11;
    Z21 = ell[nskip1] - Z21;
    Z22 = ell[1+nskip1] - Z22;
    dee = d + 2;
    /* factorize 2 x 2 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* factorize row 2 */
    sum = 0;
    q1 = Z21;
    q2 = q1 * dee[0];
    Z21 = q2;
    sum += q1 * q2;
    dee[1] = dRecip(Z22 - sum);
    /* done factorizing 2 x 2 block */
    ell[nskip1] = Z21;
	
	// switch code
	// nothing to do
}

#ifdef GCN
// <JSPG> GameCube Assembler Version
void LCPMath::dFactorLDLT5 (dReal *A, dReal *d, int nskip1)
{
  dReal sum,*ell,*dee,q1,q2,Z11,Z21,Z22;

	// Z11 <- fp15
	// Z21 <- fp16
	// Z22 <- fp17

	register const float* el0;
	register const float* eln;
	register const float* de;
	register const float* z11 = &Z11;
	register const float* z21 = &Z21;
	register const float* z22 = &Z22;

	// i = 0
	/* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
	dSolveL1_2 (A, A, 0, nskip1);
	/* scale the elements in a 2 x i block at A(i,0), and also */
	/* compute Z = the outer product matrix that we'll need. */
	Z11 = Z21 = Z22 = 0;
	ell = A;
	dee = d;
	// Nothing to do

    /* compute left-over iterations */
	// Nothing to do

	/* solve for diagonal 2 x 2 block at A(i,i) */
	Z11 = ell[0] - Z11;
	Z21 = ell[nskip1] - Z21;
	Z22 = ell[1+nskip1] - Z22;
	dee = d;
	/* factorize 2 x 2 block Z,dee */
	/* factorize row 1 */
	dee[0] = dRecip(Z11);
	/* factorize row 2 */
	sum = 0;
	q1 = Z21;
	q2 = q1 * dee[0];
	Z21 = q2;
	sum += q1 * q2;
	dee[1] = dRecip(Z22 - sum);
	/* done factorizing 2 x 2 block */
	ell[nskip1] = Z21;

	// i=2
    /* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
    dSolveL1_2 (A, A + (nskip1<<1), 2, nskip1);
    /* scale the elements in a 2 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = Z21 = Z22 = 0;
    ell = A + (nskip1<<1);
    dee = d;
    /* compute left-over iterations */
	// for with j=2, so 2 "iteraciones desenrolladas"
	el0 = &ell[0];
	eln = &ell[nskip1];
	de = &dee[0];
	asm
	{
		psq_l	fp0, 0(el0), 0, 0;	// fp0 <- e00,e01
		psq_l	fp1, 0(eln), 0, 0;	// fp1 <- en0,en1
		psq_l	fp2, 0(de), 0, 0;	// fp2 <- de0,de1
		ps_mul	fp3, fp0, fp2;		// fp3 <- e00de0,e01de1 (q1)
		ps_mul	fp4, fp1, fp2;		// fp4 <- en0de0,en1de1 (q2)
		ps_mul	fp15, fp0, fp3;		// fp15 <- e00q1,e01q1
		ps_mul	fp16, fp1, fp3;		// fp16 <- en0q1,en1q1
		ps_mul	fp17, fp1, fp4;		// fp17 <- en0q2,en1q2
		ps_sum0 fp15, fp15, fp15, fp15 // fp15 <- Z11,x
		ps_sum0 fp16, fp16, fp16, fp16 // fp16 <- Z21,x
		ps_sum0 fp17, fp17, fp17, fp17 // fp17 <- Z22,x
		psq_st	fp3, 0(el0), 0, 0;	// ell[0]=q1,ell[1]=q1
		psq_st	fp4, 0(eln), 0, 0;	// ell[nskip1]=q2,ell[nskip1+1]=q2
		psq_st	fp15, 0(z11), 1, 0;	// store Z11
		psq_st	fp16, 0(z21), 1, 0;	// store Z21
		psq_st	fp17, 0(z22), 1, 0;	// store Z22
	}
	ell+=2;
	dee+=2;
    /* solve for diagonal 2 x 2 block at A(i,i) */
    Z11 = ell[0] - Z11;
    Z21 = ell[nskip1] - Z21;
    Z22 = ell[1+nskip1] - Z22;
    dee = d + 2;
    /* factorize 2 x 2 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* factorize row 2 */
    sum = 0;
    q1 = Z21;
    q2 = q1 * dee[0];
    Z21 = q2;
    sum += q1 * q2;
    dee[1] = dRecip(Z22 - sum);
    /* done factorizing 2 x 2 block */
    ell[nskip1] = Z21;
    
    
    // switch code -> at this point i=4
    dSolveL1_1 (A, A + (nskip1<<2), 4, nskip1);
    /* scale the elements in a 1 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = 0;
    ell = A + (nskip1<<2);
    dee = d;
    /* compute left-over iterations */ // j = 4 -> 4 iterations
	el0 = &ell[0];
	de = &dee[0];
	asm
	{
		psq_l	fp0, 0(el0), 0, 0;
		psq_l	fp1, 0(de), 0 , 0;
		ps_mul	fp2, fp0, fp1;
		ps_mul	fp3, fp0, fp2;
		psq_st	fp2, 0(el0), 0, 0;
		psq_l	fp0, 8(el0), 0, 0;
		psq_l	fp1, 8(de), 0, 0;
		ps_mul	fp2, fp0, fp1;
		ps_madd	fp4, fp0, fp2, fp3;
		psq_st	fp2, 8(el0), 0, 0;
		ps_sum0	fp15, fp4, fp4, fp4;
		psq_st	fp15, 0(z11), 1, 0;
	}

    /* solve for diagonal 1 x 1 block at A(i,i) */
    Z11 = ell[4] - Z11;
    dee = d + 4;
    /* factorize 1 x 1 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* done factorizing 1 x 1 block */
    
}
#else
// PS2/WIN32 Version 
void LCPMath::dFactorLDLT5 (dReal *A, dReal *d, int nskip1)
{
  dReal sum,*ell,*dee,q1,q2,Z11,Z21,Z22;

	// i = 0
	/* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
	dSolveL1_2 (A, A, 0, nskip1);
	/* scale the elements in a 2 x i block at A(i,0), and also */
	/* compute Z = the outer product matrix that we'll need. */
	Z11 = Z21 = Z22 = 0;
	ell = A;
	dee = d;
	// Nothing to do

    /* compute left-over iterations */
	// Nothing to do

	/* solve for diagonal 2 x 2 block at A(i,i) */
	Z11 = ell[0] - Z11;
	Z21 = ell[nskip1] - Z21;
	Z22 = ell[1+nskip1] - Z22;
	dee = d;
	/* factorize 2 x 2 block Z,dee */
	/* factorize row 1 */
	dee[0] = dRecip(Z11);
	/* factorize row 2 */
	sum = 0;
	q1 = Z21;
	q2 = q1 * dee[0];
	Z21 = q2;
	sum += q1 * q2;
	dee[1] = dRecip(Z22 - sum);
	/* done factorizing 2 x 2 block */
	ell[nskip1] = Z21;

	// i=2
    /* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
    dSolveL1_2 (A, A + (nskip1<<1), 2, nskip1);
    /* scale the elements in a 2 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = Z21 = Z22 = 0;
    ell = A + (nskip1<<1);
    dee = d;
    /* compute left-over iterations */
	// for with j=2, so 2 "iteraciones desenrolladas"
	q1 = ell[0] * dee[0];
	q2 = ell[nskip1] * dee[0];
	Z11 += ell[0] * q1;
	Z21 += ell[nskip1] * q1;
	Z22 += ell[nskip1] * q2;
	ell[0] = q1;
	ell[nskip1] = q2;
	q1 = ell[1] * dee[1];
	q2 = ell[nskip1+1] * dee[1];
	Z11 += ell[1] * q1;
	Z21 += ell[nskip1+1] * q1;
	Z22 += ell[nskip1+1] * q2;
	ell[1] = q1;
	ell[nskip1+1] = q2;
	ell+=2;
	dee+=2;
    /* solve for diagonal 2 x 2 block at A(i,i) */
    Z11 = ell[0] - Z11;
    Z21 = ell[nskip1] - Z21;
    Z22 = ell[1+nskip1] - Z22;
    dee = d + 2;
    /* factorize 2 x 2 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* factorize row 2 */
    sum = 0;
    q1 = Z21;
    q2 = q1 * dee[0];
    Z21 = q2;
    sum += q1 * q2;
    dee[1] = dRecip(Z22 - sum);
    /* done factorizing 2 x 2 block */
    ell[nskip1] = Z21;
    
    
    // switch code -> at this point i=4
    dSolveL1_1 (A, A + (nskip1<<2), 4, nskip1);
    /* scale the elements in a 1 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = 0;
    ell = A + (nskip1<<2);
    dee = d;
    /* compute left-over iterations */ // j = 4 -> 4 iterations
	q1 = ell[0] * dee[0];
	Z11 += ell[0] * q1;
	ell[0] = q1;
	q1 = ell[1] * dee[1];
	Z11 += ell[1] * q1;
	ell[1] = q1;
	q1 = ell[2] * dee[2];
	Z11 += ell[2] * q1;
	ell[2] = q1;
	q1 = ell[3] * dee[3];
	Z11 += ell[3] * q1;
	ell[3] = q1;

    /* solve for diagonal 1 x 1 block at A(i,i) */
    Z11 = ell[4] - Z11;
    dee = d + 4;
    /* factorize 1 x 1 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* done factorizing 1 x 1 block */
    
}
#endif

#ifdef GCN
// <JSPG> GameCube Assembler Version
void LCPMath::dFactorLDLT6 (dReal *A, dReal *d, int nskip1)
{
	dReal sum,*ell,*dee,q1,q2,Z11,Z21,Z22;

	// Z11 <- fp15
	// Z21 <- fp16
	// Z22 <- fp17

	register const float* el0;
	register const float* eln;
	register const float* de;
	register const float* z11 = &Z11;
	register const float* z21 = &Z21;
	register const float* z22 = &Z22;
	
	// i = 0
	/* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
	dSolveL1_2 (A, A, 0, nskip1);
	/* scale the elements in a 2 x i block at A(i,0), and also */
	/* compute Z = the outer product matrix that we'll need. */
	Z11 = Z21 = Z22 = 0;
	ell = A;
	dee = d;
	// Nothing to do

    /* compute left-over iterations */
	// Nothing to do

	/* solve for diagonal 2 x 2 block at A(i,i) */
	Z11 = ell[0] - Z11;
	Z21 = ell[nskip1] - Z21;
	Z22 = ell[1+nskip1] - Z22;
	dee = d;
	/* factorize 2 x 2 block Z,dee */
	/* factorize row 1 */
	dee[0] = dRecip(Z11);
	/* factorize row 2 */
	sum = 0;
	q1 = Z21;
	q2 = q1 * dee[0];
	Z21 = q2;
	sum += q1 * q2;
	dee[1] = dRecip(Z22 - sum);
	/* done factorizing 2 x 2 block */
	ell[nskip1] = Z21;

	// i=2
    /* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
    dSolveL1_2 (A, A + (nskip1<<1), 2, nskip1);
    /* scale the elements in a 2 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = Z21 = Z22 = 0;
    ell = A + (nskip1<<1);
    dee = d;
    /* compute left-over iterations */
	// for with j=2, so 2 "iteraciones desenrolladas"
	el0 = &ell[0];
	eln = &ell[nskip1];
	de = &dee[0];
	asm
	{
		psq_l	fp0, 0(el0), 0, 0;	// fp0 <- e00,e01
		psq_l	fp1, 0(eln), 0, 0;	// fp1 <- en0,en1
		psq_l	fp2, 0(de), 0, 0;	// fp2 <- de0,de1
		ps_mul	fp3, fp0, fp2;		// fp3 <- e00de0,e01de1 (q1)
		ps_mul	fp4, fp1, fp2;		// fp4 <- en0de0,en1de1 (q2)
		ps_mul	fp15, fp0, fp3;		// fp15 <- e00q1,e01q1
		ps_mul	fp16, fp1, fp3;		// fp16 <- en0q1,en1q1
		ps_mul	fp17, fp1, fp4;		// fp17 <- en0q2,en1q2
		ps_sum0 fp15, fp15, fp15, fp15 // fp15 <- Z11,x
		ps_sum0 fp16, fp16, fp16, fp16 // fp16 <- Z21,x
		ps_sum0 fp17, fp17, fp17, fp17 // fp17 <- Z22,x
		psq_st	fp3, 0(el0), 0, 0;	// ell[0]=q1,ell[1]=q1
		psq_st	fp4, 0(eln), 0, 0;	// ell[nskip1]=q2,ell[nskip1+1]=q2
		psq_st	fp15, 0(z11), 1, 0;	// store Z11
		psq_st	fp16, 0(z21), 1, 0;	// store Z21
		psq_st	fp17, 0(z22), 1, 0;	// store Z22
	}
	ell+=2;
	dee+=2;
    /* solve for diagonal 2 x 2 block at A(i,i) */
    Z11 = ell[0] - Z11;
    Z21 = ell[nskip1] - Z21;
    Z22 = ell[1+nskip1] - Z22;
    dee = d + 2;
    /* factorize 2 x 2 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* factorize row 2 */
    sum = 0;
    q1 = Z21;
    q2 = q1 * dee[0];
    Z21 = q2;
    sum += q1 * q2;
    dee[1] = dRecip(Z22 - sum);
    /* done factorizing 2 x 2 block */
    ell[nskip1] = Z21;
    
	// i=4
    /* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
    dSolveL1_2 (A, A + (nskip1<<2), 4, nskip1);
    /* scale the elements in a 2 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = Z21 = Z22 = 0;
    ell = A + (nskip1<<2);
    dee = d;
    /* compute left-over iterations */
	// j=4	
	el0 = &ell[0];
	eln = &ell[nskip1];
	de = &dee[0];
	asm
	{
		psq_l	fp0, 0(el0), 0, 0;	// fp0 <- e00,e01
		psq_l	fp1, 0(eln), 0, 0;	// fp1 <- en0,en1
		psq_l	fp2, 0(de), 0, 0;	// fp2 <- de0,de1
		ps_mul	fp3, fp0, fp2;		// fp3 <- e00de0,e01de1 (q1)
		ps_mul	fp4, fp1, fp2;		// fp4 <- en0de0,en1de1 (q2)
		ps_mul	fp5, fp0, fp3;		// fp5 <- e00q1,e01q1 (Z11)
		ps_mul	fp6, fp1, fp3;		// fp6 <- en0q1,en1q1 (Z21)
		ps_mul	fp7, fp1, fp4;		// fp7 <- en0q2,en1q2 (Z22)
		psq_st	fp3, 0(el0), 0, 0;	// ell[0]=q1,ell[1]=q1
		psq_st	fp4, 0(eln), 0, 0;	// ell[nskip1]=q2,ell[nskip1+1]=q2

		psq_l	fp0, 8(el0), 0, 0;	// fp0 <- e02,e03
		psq_l	fp1, 8(eln), 0, 0;	// fp1 <- en2,en3
		psq_l	fp2, 8(de), 0, 0;	// fp2 <- de2,de3
		ps_mul	fp3, fp0, fp2;		// fp3 <- e02de2,e03de3 (q1)
		ps_mul	fp4, fp1, fp2;		// fp4 <- en2de2,en3de3 (q2)
		ps_madd	fp8, fp0, fp3, fp5; // fp8 <- e02q1+Z11,e03q1+Z11
		ps_madd	fp9, fp1, fp3, fp6;	// fp9 <- en2q1+Z21,en3q1+Z21
		ps_madd	fp10, fp1, fp4, fp7;// fp10 <- en2q2+Z22,en3q2+Z22
		
		psq_st	fp3, 8(el0), 0, 0;	// ell[2]=q1,ell[3]=q1
		psq_st	fp4, 8(eln), 0, 0;	// ell[nskip+2]=q2,ell[nskip+3]=q2
		ps_sum0	fp15, fp8, fp8, fp8;	// fp15 <- Z11
		ps_sum0	fp16, fp9, fp9, fp9;	// fp16 <- Z21
		ps_sum0	fp17, fp10, fp10, fp10;	// fp17 <- Z22
		psq_st	fp15, 0(z11), 1, 0;	// store Z11
		psq_st	fp16, 0(z21), 1, 0;	// store Z21
		psq_st	fp17, 0(z22), 1, 0;	// store Z22
	}
	ell+=4;
	dee+=4;
    /* solve for diagonal 2 x 2 block at A(i,i) */
    Z11 = ell[0] - Z11;
    Z21 = ell[nskip1] - Z21;
    Z22 = ell[1+nskip1] - Z22;
    dee = d + 4;
    /* factorize 2 x 2 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* factorize row 2 */
    sum = 0;
    q1 = Z21;
    q2 = q1 * dee[0];
    Z21 = q2;
    sum += q1 * q2;
    dee[1] = dRecip(Z22 - sum);
    /* done factorizing 2 x 2 block */
    ell[nskip1] = Z21;	
	    
    // switch code -> at this point i=6, n=6 -> nothing to do
}
#else
// PS2/WIN32 Version
void LCPMath::dFactorLDLT6 (dReal *A, dReal *d, int nskip1)
{
  dReal sum,*ell,*dee,q1,q2,Z11,Z21,Z22;

	// i = 0
	/* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
	dSolveL1_2 (A, A, 0, nskip1);
	/* scale the elements in a 2 x i block at A(i,0), and also */
	/* compute Z = the outer product matrix that we'll need. */
	Z11 = Z21 = Z22 = 0;
	ell = A;
	dee = d;
	// Nothing to do

    /* compute left-over iterations */
	// Nothing to do

	/* solve for diagonal 2 x 2 block at A(i,i) */
	Z11 = ell[0] - Z11;
	Z21 = ell[nskip1] - Z21;
	Z22 = ell[1+nskip1] - Z22;
	dee = d;
	/* factorize 2 x 2 block Z,dee */
	/* factorize row 1 */
	dee[0] = dRecip(Z11);
	/* factorize row 2 */
	sum = 0;
	q1 = Z21;
	q2 = q1 * dee[0];
	Z21 = q2;
	sum += q1 * q2;
	dee[1] = dRecip(Z22 - sum);
	/* done factorizing 2 x 2 block */
	ell[nskip1] = Z21;

	// i=2
    /* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
    dSolveL1_2 (A, A + (nskip1<<1), 2, nskip1);
    /* scale the elements in a 2 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = Z21 = Z22 = 0;
    ell = A + (nskip1<<1);
    dee = d;
    /* compute left-over iterations */
	// for with j=2, so 2 "iteraciones desenrolladas"
	q1 = ell[0] * dee[0];
	q2 = ell[nskip1] * dee[0];
	Z11 += ell[0] * q1;
	Z21 += ell[nskip1] * q1;
	Z22 += ell[nskip1] * q2;
	ell[0] = q1;
	ell[nskip1] = q2;
	q1 = ell[1] * dee[1];
	q2 = ell[nskip1+1] * dee[1];
	Z11 += ell[1] * q1;
	Z21 += ell[nskip1+1] * q1;
	Z22 += ell[nskip1+1] * q2;
	ell[1] = q1;
	ell[nskip1+1] = q2;
	ell+=2;
	dee+=2;
    /* solve for diagonal 2 x 2 block at A(i,i) */
    Z11 = ell[0] - Z11;
    Z21 = ell[nskip1] - Z21;
    Z22 = ell[1+nskip1] - Z22;
    dee = d + 2;
    /* factorize 2 x 2 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* factorize row 2 */
    sum = 0;
    q1 = Z21;
    q2 = q1 * dee[0];
    Z21 = q2;
    sum += q1 * q2;
    dee[1] = dRecip(Z22 - sum);
    /* done factorizing 2 x 2 block */
    ell[nskip1] = Z21;
    
	// i=4
    /* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
    dSolveL1_2 (A, A + (nskip1<<2), 4, nskip1);
    /* scale the elements in a 2 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = Z21 = Z22 = 0;
    ell = A + (nskip1<<2);
    dee = d;
    /* compute left-over iterations */
	// j=4	
	q1 = ell[0] * dee[0];
	q2 = ell[nskip1] * dee[0];
	Z11 += ell[0] * q1;
	Z21 += ell[nskip1] * q1;
	Z22 += ell[nskip1] * q2;
	ell[0] = q1;
	ell[nskip1] = q2;
	q1 = ell[1] * dee[1];
	q2 = ell[nskip1+1] * dee[1];
	Z11 += ell[1] * q1;
	Z21 += ell[nskip1+1] * q1;
	Z22 += ell[nskip1+1] * q2;
	ell[1] = q1;
	ell[nskip1+1] = q2;
	q1 = ell[2] * dee[2];
	q2 = ell[nskip1+2] * dee[2];
	Z11 += ell[2] * q1;
	Z21 += ell[nskip1+2] * q1;
	Z22 += ell[nskip1+2] * q2;
	ell[2] = q1;
	ell[nskip1+2] = q2;
	q1 = ell[3] * dee[3];
	q2 = ell[nskip1+3] * dee[3];
	Z11 += ell[3] * q1;
	Z21 += ell[nskip1+3] * q1;
	Z22 += ell[nskip1+3] * q2;
	ell[3] = q1;
	ell[nskip1+3] = q2;
	ell+=4;
	dee+=4;
    /* solve for diagonal 2 x 2 block at A(i,i) */
    Z11 = ell[0] - Z11;
    Z21 = ell[nskip1] - Z21;
    Z22 = ell[1+nskip1] - Z22;
    dee = d + 4;
    /* factorize 2 x 2 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* factorize row 2 */
    sum = 0;
    q1 = Z21;
    q2 = q1 * dee[0];
    Z21 = q2;
    sum += q1 * q2;
    dee[1] = dRecip(Z22 - sum);
    /* done factorizing 2 x 2 block */
    ell[nskip1] = Z21;	
	    
    // switch code -> at this point i=6, n=6 -> nothing to do
}
#endif



void LCPMath::dFactorLDLT (dReal *A, dReal *d, int n, int nskip1)
{  
  int i,j;
  dReal sum,*ell,*dee,q1,q2,Z11,Z21,Z22;
  if (n < 1) return;
  
  for (i=0; i <= n - 2; i += 2) {
    /* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
    dSolveL1_2 (A, A + i * nskip1, i, nskip1);
    /* scale the elements in a 2 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = Z21 = Z22 = 0.0f;
    ell = A + i * nskip1;
    dee = d;

#ifndef OPTIM_WHEN_N_INFERIOR_8
    for (j=i-6; j >= 0; j -= 6) {
			q1 = ell[0] * dee[0];
			q2 = ell[nskip1] * dee[0];
			Z11 += ell[0] * q1;
			Z21 += ell[nskip1] * q1;
			Z22 += ell[nskip1] * q2;
			ell[0] = q1;
			ell[nskip1] = q2;

			q1 = ell[1] * dee[1];
			q2 = ell[1 + nskip1] * dee[1];
			Z11 += ell[1] * q1;
			Z21 += ell[1 + nskip1] * q1;
			Z22 += ell[1 + nskip1] * q2;
			ell[1] = q1;
			ell[1 + nskip1] = q2;

			q1 = ell[2] * dee[2];
			q2 = ell[2 + nskip1] * dee[2];
			Z11 += ell[2] * q1;
			Z21 += ell[2 + nskip1] * q1;
			Z22 += ell[2 + nskip1] * q2;
			ell[2] = q1;
			ell[2 + nskip1] = q2;

			q1 = ell[3] * dee[3];
			q2 = ell[3 + nskip1] * dee[3];
			Z11 += ell[3] * q1;
			Z21 += ell[3 + nskip1] * q1;
			Z22 += ell[3 + nskip1] * q2;
			ell[3] = q1;
			ell[3 + nskip1] = q2;

			q1 = ell[4] * dee[4];
			q2 = ell[4 + nskip1] * dee[4];
			Z11 += ell[4] * q1;
			Z21 += ell[4 + nskip1] * q1;
			Z22 += ell[4 + nskip1] * q2;
			ell[4] = q1;
			ell[4 + nskip1] = q2;

			q1 = ell[5] * dee[5];
			q2 = ell[5 + nskip1] * dee[5];
			Z11 += ell[5] * q1;
			Z21 += ell[5 + nskip1] * q1;
			Z22 += ell[5 + nskip1] * q2;
			ell[5] = q1;
			ell[5 + nskip1] = q2;

			ell += 6;
			dee += 6;
    }
  
    /* compute left-over iterations */
    j += 6;
#else
    assert( n < 8 );
    j=i;
#endif

    for (; j > 0; j--) 
    {
		q1 = ell[0] * dee[0];
		q2 = ell[nskip1] * dee[0];
		Z11 += ell[0] * q1;
		Z21 += ell[nskip1] * q1;
		Z22 += ell[nskip1] * q2;
		ell[0] = q1;
		ell[nskip1] = q2;
		ell++;
		dee++;
    }
    /* solve for diagonal 2 x 2 block at A(i,i) */
    Z11 = ell[0] - Z11;
    Z21 = ell[nskip1] - Z21;
    Z22 = ell[1+nskip1] - Z22;
    dee = d + i;
    /* factorize 2 x 2 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* factorize row 2 */
    sum = 0.0f;
    q1 = Z21;
    q2 = q1 * dee[0];
    Z21 = q2;
    sum += q1 * q2;
    dee[1] = dRecip(Z22 - sum);
    /* done factorizing 2 x 2 block */
    ell[nskip1] = Z21;
  }
  /* compute the (less than 2) rows at the bottom */
  switch (n-i) {
    case 0:
    break;
    
    case 1:
    dSolveL1_1 (A, A + i * nskip1, i, nskip1);
    /* scale the elements in a 1 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = 0.0f;
    ell = A + i * nskip1;
    dee = d;

#ifndef OPTIM_WHEN_N_INFERIOR_8
	for (j=i-6; j >= 0; j -= 6) 
	{
		q1 = ell[0] * dee[0];
		Z11 += ell[0] * q1;
		ell[0] = q1;
		q1 = ell[1] * dee[1];
		Z11 += ell[1] * q1;
		ell[1] = q1;
		q1 = ell[2] * dee[2];
		Z11 += ell[2] * q1;
		ell[2] = q1;
		q1 = ell[3] * dee[3];
		Z11 += ell[3] * q1;
		ell[3] = q1;
		q1 = ell[4] * dee[4];
		Z11 += ell[4] * q1;
		ell[4] = q1;
		q1 = ell[5] * dee[5];
		Z11 += ell[5] * q1;
		ell[5] = q1;
		ell += 6;
		dee += 6;
    }
    /* compute left-over iterations */
    j += 6;
#else
    j=i;
#endif

    for (; j > 0; j--) {
      q1 = ell[0] * dee[0];
      Z11 += ell[0] * q1;
      ell[0] = q1;
      ell++;
      dee++;
    }
    /* solve for diagonal 1 x 1 block at A(i,i) */
    Z11 = ell[0] - Z11;
    dee = d + i;
    /* factorize 1 x 1 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* done factorizing 1 x 1 block */
    break;
    
    default: *((char*)0)=0;  /* this should never happen! */
  }
}

#else // _FASTSOLVE_

/* solve L*X=B, with B containing 1 right hand sides.
 * L is an n*n lower triangular matrix with ones on the diagonal.
 * L is stored by rows and its leading dimension is lskip.
 * B is an n*1 matrix that contains the right hand sides.
 * B is stored by columns and its leading dimension is also lskip.
 * B is overwritten with X.
 * this processes blocks of 2*2.
 * if this is in the factorizer source file, n must be a multiple of 2.
 */

static void LCPMath::dSolveL1_1 (const dReal *L, dReal *B, int n, int lskip1)
{  
  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,m11,Z21,m21,p1,q1,p2,*ex;
  const dReal *ell;
  int i,j;
  /* compute all 2 x 1 blocks of X */
  for (i=0; i < n; i+=2) {
    /* compute all 2 x 1 block of X, from rows i..i+2-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    Z21=0;
    ell = L + i*lskip1;
    ex = B;
    /* the inner loop that computes outer products and adds them to Z */
    for (j=i-2; j >= 0; j -= 2) {
      /* compute outer product and add it to the Z matrix */
      p1=ell[0];
      q1=ex[0];
      m11 = p1 * q1;
      p2=ell[lskip1];
      m21 = p2 * q1;
      Z11 += m11;
      Z21 += m21;
      /* compute outer product and add it to the Z matrix */
      p1=ell[1];
      q1=ex[1];
      m11 = p1 * q1;
      p2=ell[1+lskip1];
      m21 = p2 * q1;
      /* advance pointers */
      ell += 2;
      ex += 2;
      Z11 += m11;
      Z21 += m21;
      /* end of inner loop */
    }
    /* compute left-over iterations */
    j += 2;
    for (; j > 0; j--) {
      /* compute outer product and add it to the Z matrix */
      p1=ell[0];
      q1=ex[0];
      m11 = p1 * q1;
      p2=ell[lskip1];
      m21 = p2 * q1;
      /* advance pointers */
      ell += 1;
      ex += 1;
      Z11 += m11;
      Z21 += m21;
    }
    /* finish computing the X(i) block */
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
    p1 = ell[lskip1];
    Z21 = ex[1] - Z21 - p1*Z11;
    ex[1] = Z21;
    /* end of outer loop */
  }
}

/* solve L*X=B, with B containing 2 right hand sides.
 * L is an n*n lower triangular matrix with ones on the diagonal.
 * L is stored by rows and its leading dimension is lskip.
 * B is an n*2 matrix that contains the right hand sides.
 * B is stored by columns and its leading dimension is also lskip.
 * B is overwritten with X.
 * this processes blocks of 2*2.
 * if this is in the factorizer source file, n must be a multiple of 2.
 */

static void LCPMath::dSolveL1_2 (const dReal *L, dReal *B, int n, int lskip1)
{  
  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,m11,Z12,m12,Z21,m21,Z22,m22,p1,q1,p2,q2,*ex;
  const dReal *ell;
  int i,j;
  /* compute all 2 x 2 blocks of X */
  for (i=0; i < n; i+=2) {
    /* compute all 2 x 2 block of X, from rows i..i+2-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    Z12=0;
    Z21=0;
    Z22=0;
    ell = L + i*lskip1;
    ex = B;
    /* the inner loop that computes outer products and adds them to Z */
    for (j=i-2; j >= 0; j -= 2) {
      /* compute outer product and add it to the Z matrix */
      p1=ell[0];
      q1=ex[0];
      m11 = p1 * q1;
      q2=ex[lskip1];
      m12 = p1 * q2;
      p2=ell[lskip1];
      m21 = p2 * q1;
      m22 = p2 * q2;
      Z11 += m11;
      Z12 += m12;
      Z21 += m21;
      Z22 += m22;
      /* compute outer product and add it to the Z matrix */
      p1=ell[1];
      q1=ex[1];
      m11 = p1 * q1;
      q2=ex[1+lskip1];
      m12 = p1 * q2;
      p2=ell[1+lskip1];
      m21 = p2 * q1;
      m22 = p2 * q2;
      /* advance pointers */
      ell += 2;
      ex += 2;
      Z11 += m11;
      Z12 += m12;
      Z21 += m21;
      Z22 += m22;
      /* end of inner loop */
    }
    /* compute left-over iterations */
    j += 2;
    for (; j > 0; j--) {
      /* compute outer product and add it to the Z matrix */
      p1=ell[0];
      q1=ex[0];
      m11 = p1 * q1;
      q2=ex[lskip1];
      m12 = p1 * q2;
      p2=ell[lskip1];
      m21 = p2 * q1;
      m22 = p2 * q2;
      /* advance pointers */
      ell += 1;
      ex += 1;
      Z11 += m11;
      Z12 += m12;
      Z21 += m21;
      Z22 += m22;
    }
    /* finish computing the X(i) block */
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
    Z12 = ex[lskip1] - Z12;
    ex[lskip1] = Z12;
    p1 = ell[lskip1];
    Z21 = ex[1] - Z21 - p1*Z11;
    ex[1] = Z21;
    Z22 = ex[1+lskip1] - Z22 - p1*Z12;
    ex[1+lskip1] = Z22;
    /* end of outer loop */
  }
}


void LCPMath::dFactorLDLT (dReal *A, dReal *d, int n, int nskip1)
{  
  int i,j;
  dReal sum,*ell,*dee,dd,p1,p2,q1,q2,Z11,m11,Z21,m21,Z22,m22;
  if (n < 1) return;
  
  for (i=0; i<=n-2; i += 2) 
  {
    /* solve L*(D*l)=a, l is scaled elements in 2 x i block at A(i,0) */
    dSolveL1_2 (A,A+i*nskip1,i,nskip1);
    /* scale the elements in a 2 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = 0;
    Z21 = 0;
    Z22 = 0;
    ell = A+i*nskip1;
    dee = d;
    for (j=i-6; j >= 0; j -= 6) {
      p1 = ell[0];
      p2 = ell[nskip1];
      dd = dee[0];
      q1 = p1*dd;
      q2 = p2*dd;
      ell[0] = q1;
      ell[nskip1] = q2;
      m11 = p1*q1;
      m21 = p2*q1;
      m22 = p2*q2;
      Z11 += m11;
      Z21 += m21;
      Z22 += m22;
      p1 = ell[1];
      p2 = ell[1+nskip1];
      dd = dee[1];
      q1 = p1*dd;
      q2 = p2*dd;
      ell[1] = q1;
      ell[1+nskip1] = q2;
      m11 = p1*q1;
      m21 = p2*q1;
      m22 = p2*q2;
      Z11 += m11;
      Z21 += m21;
      Z22 += m22;
      p1 = ell[2];
      p2 = ell[2+nskip1];
      dd = dee[2];
      q1 = p1*dd;
      q2 = p2*dd;
      ell[2] = q1;
      ell[2+nskip1] = q2;
      m11 = p1*q1;
      m21 = p2*q1;
      m22 = p2*q2;
      Z11 += m11;
      Z21 += m21;
      Z22 += m22;
      p1 = ell[3];
      p2 = ell[3+nskip1];
      dd = dee[3];
      q1 = p1*dd;
      q2 = p2*dd;
      ell[3] = q1;
      ell[3+nskip1] = q2;
      m11 = p1*q1;
      m21 = p2*q1;
      m22 = p2*q2;
      Z11 += m11;
      Z21 += m21;
      Z22 += m22;
      p1 = ell[4];
      p2 = ell[4+nskip1];
      dd = dee[4];
      q1 = p1*dd;
      q2 = p2*dd;
      ell[4] = q1;
      ell[4+nskip1] = q2;
      m11 = p1*q1;
      m21 = p2*q1;
      m22 = p2*q2;
      Z11 += m11;
      Z21 += m21;
      Z22 += m22;
      p1 = ell[5];
      p2 = ell[5+nskip1];
      dd = dee[5];
      q1 = p1*dd;
      q2 = p2*dd;
      ell[5] = q1;
      ell[5+nskip1] = q2;
      m11 = p1*q1;
      m21 = p2*q1;
      m22 = p2*q2;
      Z11 += m11;
      Z21 += m21;
      Z22 += m22;
      ell += 6;
      dee += 6;
    }
    /* compute left-over iterations */
    j += 6;
    for (; j > 0; j--) {
      p1 = ell[0];
      p2 = ell[nskip1];
      dd = dee[0];
      q1 = p1*dd;
      q2 = p2*dd;
      ell[0] = q1;
      ell[nskip1] = q2;
      m11 = p1*q1;
      m21 = p2*q1;
      m22 = p2*q2;
      Z11 += m11;
      Z21 += m21;
      Z22 += m22;
      ell++;
      dee++;
    }
    /* solve for diagonal 2 x 2 block at A(i,i) */
    Z11 = ell[0] - Z11;
    Z21 = ell[nskip1] - Z21;
    Z22 = ell[1+nskip1] - Z22;
    dee = d + i;
    /* factorize 2 x 2 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* factorize row 2 */
    sum = 0;
    q1 = Z21;
    q2 = q1 * dee[0];
    Z21 = q2;
    sum += q1*q2;
    dee[1] = dRecip(Z22 - sum);
    /* done factorizing 2 x 2 block */
    ell[nskip1] = Z21;
  }
  /* compute the (less than 2) rows at the bottom */
  switch (n-i) 
  {
    case 0:
    break;
    
    case 1:
    dSolveL1_1 (A,A+i*nskip1,i,nskip1);
    /* scale the elements in a 1 x i block at A(i,0), and also */
    /* compute Z = the outer product matrix that we'll need. */
    Z11 = 0;
    ell = A+i*nskip1;
    dee = d;
    for (j=i-6; j >= 0; j -= 6) {
      p1 = ell[0];
      dd = dee[0];
      q1 = p1*dd;
      ell[0] = q1;
      m11 = p1*q1;
      Z11 += m11;
      p1 = ell[1];
      dd = dee[1];
      q1 = p1*dd;
      ell[1] = q1;
      m11 = p1*q1;
      Z11 += m11;
      p1 = ell[2];
      dd = dee[2];
      q1 = p1*dd;
      ell[2] = q1;
      m11 = p1*q1;
      Z11 += m11;
      p1 = ell[3];
      dd = dee[3];
      q1 = p1*dd;
      ell[3] = q1;
      m11 = p1*q1;
      Z11 += m11;
      p1 = ell[4];
      dd = dee[4];
      q1 = p1*dd;
      ell[4] = q1;
      m11 = p1*q1;
      Z11 += m11;
      p1 = ell[5];
      dd = dee[5];
      q1 = p1*dd;
      ell[5] = q1;
      m11 = p1*q1;
      Z11 += m11;
      ell += 6;
      dee += 6;
    }
    /* compute left-over iterations */
    j += 6;
    for (; j > 0; j--) {
      p1 = ell[0];
      dd = dee[0];
      q1 = p1*dd;
      ell[0] = q1;
      m11 = p1*q1;
      Z11 += m11;
      ell++;
      dee++;
    }
    /* solve for diagonal 1 x 1 block at A(i,i) */
    Z11 = ell[0] - Z11;
    dee = d + i;
    /* factorize 1 x 1 block Z,dee */
    /* factorize row 1 */
    dee[0] = dRecip(Z11);
    /* done factorizing 1 x 1 block */
    break;
    
    default: *((char*)0)=0;  /* this should never happen! */
  }
}

#endif

void LCPMath::dVectorScale (dReal *a, const dReal *d, int n)
{
  dAASSERT (a && d && n >= 0);
  for (int i=0; i<n; i++) a[i] *= d[i];
}

/* solve L*X=B, with B containing 1 right hand sides.
 * L is an n*n lower triangular matrix with ones on the diagonal.
 * L is stored by rows and its leading dimension is lskip.
 * B is an n*1 matrix that contains the right hand sides.
 * B is stored by columns and its leading dimension is also lskip.
 * B is overwritten with X.
 * this processes blocks of 4*4.
 * if this is in the factorizer source file, n must be a multiple of 4.
 */
//#ifndef PS2 
 
#ifdef GCN
// <JSPG> GameCube Assembler Version
void LCPMath::dSolveL13(const dReal *L, dReal *B, int lskip1)
{	
  dReal Z11,p1,q1,*ex;
  const dReal *ell;
  register const float* el;
  register const float* exr;

  /* compute rows at end that are not a multiple of block size */
	// i=0 -> nothing to do

	// i=1
	  Z11=0;
	  ell = L + lskip1;
	  ex = B;

      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* advance pointers */
      ell += 1;
      ex += 1;

	  Z11 = ex[0] - Z11;
	  ex[0] = Z11;

	// i=2

	  el = L + (lskip1<<1);
	  exr = B;	  

	  // PS2: 6 MEM + 2 MUL + 2 ADD + 1 SUB
	  // GCN: 4 MEM + 1 MUL + 1 ADD + 1 SUB

    /* compute left-over iterations */
	  asm
	  {
	  		psq_l	fp0, 0(el), 0 , 0;
	  		psq_l	fp1, 0(exr), 0 , 0;
	  		ps_mul	fp2, fp0, fp1;
	  		ps_sum0	fp3, fp2, fp2, fp2;
			psq_l	fp4, 8(exr), 1, 0;
			ps_sub	fp5, fp4, fp3;
			psq_st	fp5, 8(exr), 1, 0;
	  }
}
#else
// PS2/WIN32 Version
void LCPMath::dSolveL13(const dReal *L, dReal *B, int lskip1)
{	
  dReal Z11,p1,q1,*ex;
  const dReal *ell;

  /* compute rows at end that are not a multiple of block size */
	// i=0 -> nothing to do

	// i=1
	  Z11=0;
	  ell = L + lskip1;
	  ex = B;

      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* advance pointers */
      ell += 1;
      ex += 1;

	  Z11 = ex[0] - Z11;
	  ex[0] = Z11;

	// i=2
      Z11=0;
      ell = L + (lskip1<<1);
      ex = B;

      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* advance pointers */

      p1=ell[1];
      q1=ex[1];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* advance pointers */

	  Z11 = ex[2] - Z11;
	  ex[2] = Z11;		
}
#endif

void LCPMath::dSolveL14(const dReal *L, dReal *B, int lskip1)
{	
// sv. (debug)
//oSerie.Add( (float) n , 1.0f);

  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z21,Z31,Z41,p1,p2,p3,*ex;
  dReal aux0, aux1;
  const dReal *ell;
  int lskip2,lskip3;

  /* compute lskip values */
  lskip2 = (lskip1<<1);
  lskip3 = lskip2+lskip1;

  /* compute all 4 x 1 blocks of X */

	// i=0

    ell = L;
    ex = B;
	
    /* finish computing the X(i) block */
    p1 = ell[lskip1];
	aux0 = p1*ex[0];
    Z21 = ex[1] - aux0;
	aux1 = p1*Z21;
    ex[1] = Z21;
    p1 = ell[lskip2];
    p2 = ell[1+lskip2];
    Z31 = ex[2] - aux0 - aux1;
    ex[2] = Z31;
    p1 = ell[lskip3];
    p2 = ell[1+lskip3];
    p3 = ell[2+lskip3];
    Z41 = ex[3] - aux0 - aux1 - p3*Z31;
    ex[3] = Z41;
    
    // al final del bucle i=4 -> nothing more to do    
}

#ifdef GCN
void LCPMath::dSolveL15 (const dReal *L, dReal *B, int lskip1)
{  
// sv. (debug)
//oSerie.Add( (float) n , 1.0f);

  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,Z21,Z31,Z41,p1,p2,p3,*ex;
  dReal aux0, aux1;
  const dReal *ell;
  int lskip2,lskip3;
  register const float* el;
  register const float* exr;

  /* compute lskip values */
  lskip2 = (lskip1<<1);
  lskip3 = lskip2+lskip1;

  /* compute all 4 x 1 blocks of X */

	// i=0

    ell = L;
    ex = B;
	
    /* finish computing the X(i) block */
    Z11 = ex[0];
    p1 = ell[lskip1];
	aux0 = p1*Z11;
    Z21 = ex[1] - aux0;
	aux1 = p1*Z21;
    ex[1] = Z21;
    p1 = ell[lskip2];
    p2 = ell[1+lskip2];
    Z31 = ex[2] - aux0 - aux1;
    ex[2] = Z31;
    p1 = ell[lskip3];
    p2 = ell[1+lskip3];
    p3 = ell[2+lskip3];
    Z41 = ex[3] - aux0 - aux1 - p3*Z31;
    ex[3] = Z41;
    
    // al final del bucle i=4
  /* compute rows at end that are not a multiple of block size */
	
	// i=4
	
	el = L + (lskip1<<2);
	exr = B;
	
	// PS2: 10 MEM + 4 MUL + 4 ADD + 1 SUB
	// GCN: 6 MEM  + 2 MUL + 2 ADD + 1 SUB

    /* compute left-over iterations */
	asm
	{
		psq_l	fp0, 0(el), 0, 0;
		psq_l	fp1, 0(exr), 0, 0;
		ps_mul	fp2, fp0, fp1;
		psq_l	fp0, 8(el), 0, 0;
		psq_l	fp1, 8(exr), 0, 0;
		ps_mul	fp3, fp0, fp1;
		ps_add	fp4, fp2, fp3;
		ps_sum0	fp5, fp4, fp4, fp4;
		psq_l	fp6, 16(exr), 1, 0;
		ps_sub	fp7, fp6, fp5;
		psq_st	fp7, 16(exr), 1, 0;
	}
}
#else
void LCPMath::dSolveL15 (const dReal *L, dReal *B, int lskip1)
{  
// sv. (debug)
//oSerie.Add( (float) n , 1.0f);

  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,Z21,Z31,Z41,p1,p2,p3,q1,*ex;
  dReal aux0, aux1;
  const dReal *ell;
  int lskip2,lskip3;

  /* compute lskip values */
  lskip2 = (lskip1<<1);
  lskip3 = lskip2+lskip1;

  /* compute all 4 x 1 blocks of X */

	// i=0

    ell = L;
    ex = B;
	
    /* finish computing the X(i) block */
    Z11 = ex[0];
    p1 = ell[lskip1];
	aux0 = p1*Z11;
    Z21 = ex[1] - aux0;
	aux1 = p1*Z21;
    ex[1] = Z21;
    p1 = ell[lskip2];
    p2 = ell[1+lskip2];
    Z31 = ex[2] - aux0 - aux1;
    ex[2] = Z31;
    p1 = ell[lskip3];
    p2 = ell[1+lskip3];
    p3 = ell[2+lskip3];
    Z41 = ex[3] - aux0 - aux1 - p3*Z31;
    ex[3] = Z41;
    
    // al final del bucle i=4
  /* compute rows at end that are not a multiple of block size */
	
	// i=4
    Z11=0;
    ell = L + (lskip1<<2);
    ex = B;
	
    /* compute left-over iterations */
	// j=4
	
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
		
      /* load p and q values */
      p1=ell[1];
      q1=ex[1];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;

      /* load p and q values */
      p1=ell[2];
      q1=ex[2];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;

      /* load p and q values */
      p1=ell[3];
      q1=ex[3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;

    /* finish computing the X(i) block */
    Z11 = ex[4] - Z11;
    ex[4] = Z11;
}
#endif

#ifdef GCN
void LCPMath::dSolveL16 (const dReal *L, dReal *B, int lskip1)
{  
// sv. (debug)
//oSerie.Add( (float) n , 1.0f);

  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,Z21,Z31,Z41,p1,p2,p3,*ex;
  dReal aux0, aux1;
  const dReal *ell;
  int lskip2,lskip3;
  register const float* el;
  register const float* exr;

  /* compute lskip values */
  lskip2 = (lskip1<<1);
  lskip3 = lskip2+lskip1;

  /* compute all 4 x 1 blocks of X */

	// i=0

    ell = L;
    ex = B;
	
    /* finish computing the X(i) block */
    Z11 = ex[0];
    p1 = ell[lskip1];
	aux0 = p1*Z11;
    Z21 = ex[1] - aux0;
	aux1 = p1*Z21;
    ex[1] = Z21;
    p1 = ell[lskip2];
    p2 = ell[1+lskip2];
    Z31 = ex[2] - aux0 - aux1;
    ex[2] = Z31;
    p1 = ell[lskip3];
    p2 = ell[1+lskip3];
    p3 = ell[2+lskip3];
    Z41 = ex[3] - aux0 - aux1 - p3*Z31;
    ex[3] = Z41;
    
    // al final del bucle i=4
  /* compute rows at end that are not a multiple of block size */

	// i=4
		
	el = L + (lskip1<<2);

	// PS2: 10 MEM + 4 MUL + 4 ADD + 1 SUB
	// GCN: 6 MEM  + 2 MUL + 2 ADD + 1 SUB

    /* compute left-over iterations */
	asm
	{
		psq_l	fp0, 0(el), 0, 0;
		psq_l	fp1, 0(exr), 0, 0;
		ps_mul	fp2, fp0, fp1;
		psq_l	fp0, 8(el), 0, 0;
		psq_l	fp1, 8(exr), 0, 0;
		ps_mul	fp3, fp0, fp1;
		ps_add	fp4, fp2, fp3;
		ps_sum0	fp5, fp4, fp4, fp4;
		psq_l	fp6, 16(exr), 1, 0;
		ps_sub	fp7, fp6, fp5;
		psq_st	fp7, 16(exr), 1, 0;
	}
	
	// i=5

    el = L + 5*lskip1;

	// PS2: 12 MEM + 5 MUL + 5 ADD + 1 SUB
	// GCN: 7 MEM  + 2 MUL + 2 ADD + 1 SUB + 1 MULADD

    /* compute left-over iterations */
	asm
	{
		psq_l	fp0, 0(el), 0, 0;
		psq_l	fp1, 0(exr), 0, 0;
		ps_mul	fp2, fp0, fp1;
		psq_l	fp0, 8(el), 0, 0;
		psq_l	fp1, 8(exr), 0, 0;
		ps_mul	fp3, fp0, fp1;
		psq_l	fp0, 16(el), 1, 0;
		psq_l	fp1, 16(exr), 0, 0;
		ps_add	fp4, fp2, fp3;
		ps_sum0	fp5, fp4, fp4, fp4;
		ps_madd	fp6, fp0, fp1, fp5;
		ps_merge11 fp7, fp1, fp1;
		ps_sub	fp8, fp7, fp6;
		psq_st	fp8, 20(exr), 1, 0;
	}
}
#else
// PS2/WIN32 Version
void LCPMath::dSolveL16 (const dReal *L, dReal *B, int lskip1)
{  
// sv. (debug)
//oSerie.Add( (float) n , 1.0f);

  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,Z21,Z31,Z41,p1,p2,p3,q1,*ex;
  dReal aux0, aux1;
  const dReal *ell;
  int lskip2,lskip3;

  /* compute lskip values */
  lskip2 = (lskip1<<1);
  lskip3 = lskip2+lskip1;

  /* compute all 4 x 1 blocks of X */

	// i=0

    ell = L;
    ex = B;
	
    /* finish computing the X(i) block */
    Z11 = ex[0];
    p1 = ell[lskip1];
	aux0 = p1*Z11;
    Z21 = ex[1] - aux0;
	aux1 = p1*Z21;
    ex[1] = Z21;
    p1 = ell[lskip2];
    p2 = ell[1+lskip2];
    Z31 = ex[2] - aux0 - aux1;
    ex[2] = Z31;
    p1 = ell[lskip3];
    p2 = ell[1+lskip3];
    p3 = ell[2+lskip3];
    Z41 = ex[3] - aux0 - aux1 - p3*Z31;
    ex[3] = Z41;
    
    // al final del bucle i=4
  /* compute rows at end that are not a multiple of block size */

	// i=4
    Z11=0;
    ell = L + (lskip1<<2);
    ex = B;
	
    /* compute left-over iterations */
	// j=4
	
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
		
      /* load p and q values */
      p1=ell[1];
      q1=ex[1];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;

      /* load p and q values */
      p1=ell[2];
      q1=ex[2];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;

      /* load p and q values */
      p1=ell[3];
      q1=ex[3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;

    /* finish computing the X(i) block */
    Z11 = ex[4] - Z11;
    ex[4] = Z11;
	
	// i=5
    Z11=0;
    ell = L + 5*lskip1;
    ex = B;

    /* compute left-over iterations */
	// j=5
		
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
		
      /* load p and q values */
      p1=ell[1];
      q1=ex[1];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;

      /* load p and q values */
      p1=ell[2];
      q1=ex[2];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;

      /* load p and q values */
      p1=ell[3];
      q1=ex[3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;

      /* load p and q values */
      p1=ell[4];
      q1=ex[4];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;

    /* finish computing the X(i) block */
    Z11 = ex[5] - Z11;
    ex[5] = Z11;
}
#endif

void LCPMath::dSolveL1 (const dReal *L, dReal *B, int n, int lskip1)
{  
// sv. (debug)
//oSerie.Add( (float) n , 1.0f);

  /* declare variables - Z matrix, p and q vectors, etc */

#ifdef PS2
    union {
        u_long128 ul128Zi1; 
        sceVu0FVECTOR sceZi1;
        struct {
            dReal Z11;
            dReal Z21;
            dReal Z31;
            dReal Z41;
        };
    } __attribute__ ((aligned(16))); 
    union {
        u_long128 ul128Pi; 
        sceVu0FVECTOR scePi;
        struct {
            dReal p1;
            dReal p2;
            dReal p3;
            dReal p4;
        };
    } __attribute__ ((aligned(16))); 
#else
  dReal Z11,Z21,Z31,Z41;
  dReal p1,p2,p3,p4;
#endif

  dReal q1,*ex;
  const dReal *ell;
  int lskip2,lskip3,i,j;
  /* compute lskip values */
  lskip2 = 2*lskip1;
  lskip3 = 3*lskip1;
  /* compute all 4 x 1 blocks of X */
  for (i=0; i <= n-4; i+=4) {
    /* compute all 4 x 1 block of X, from rows i..i+4-1 */
    /* set the Z matrix to 0 */
#ifdef PS2
      ul128Zi1=0;
#else
    Z11=0.0f;
    Z21=0.0f;
    Z31=0.0f;
    Z41=0.0f;
#endif

    ell = L + i*lskip1;
    ex = B;

    /* the inner loop that computes outer products and adds them to Z */
#ifndef OPTIM_WHEN_N_INFERIOR_8

    for (j=i-12; j >= 0; j -= 12) {
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      p2=ell[lskip1];
      p3=ell[lskip2];
      p4=ell[lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[1];
      q1=ex[1];
      p2=ell[1+lskip1];
      p3=ell[1+lskip2];
      p4=ell[1+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[2];
      q1=ex[2];
      p2=ell[2+lskip1];
      p3=ell[2+lskip2];
      p4=ell[2+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[3];
      q1=ex[3];
      p2=ell[3+lskip1];
      p3=ell[3+lskip2];
      p4=ell[3+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[4];
      q1=ex[4];
      p2=ell[4+lskip1];
      p3=ell[4+lskip2];
      p4=ell[4+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[5];
      q1=ex[5];
      p2=ell[5+lskip1];
      p3=ell[5+lskip2];
      p4=ell[5+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[6];
      q1=ex[6];
      p2=ell[6+lskip1];
      p3=ell[6+lskip2];
      p4=ell[6+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[7];
      q1=ex[7];
      p2=ell[7+lskip1];
      p3=ell[7+lskip2];
      p4=ell[7+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[8];
      q1=ex[8];
      p2=ell[8+lskip1];
      p3=ell[8+lskip2];
      p4=ell[8+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[9];
      q1=ex[9];
      p2=ell[9+lskip1];
      p3=ell[9+lskip2];
      p4=ell[9+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[10];
      q1=ex[10];
      p2=ell[10+lskip1];
      p3=ell[10+lskip2];
      p4=ell[10+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* load p and q values */
      p1=ell[11];
      q1=ex[11];
      p2=ell[11+lskip1];
      p3=ell[11+lskip2];
      p4=ell[11+lskip3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      /* advance pointers */
      ell += 12;
      ex += 12;
      /* end of inner loop */
    }

    /* compute left-over iterations */
    j += 12;
#else
    assert( n < 16 );
    j=i;
#endif
    for (; j > 0; j--) {
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      p2=ell[lskip1];
      p3=ell[lskip2];
      p4=ell[lskip3];
      /* compute outer product and add it to the Z matrix */
#ifdef PS2
    sceVu0FVECTOR sceTmp;
    MysceVu0ScaleVector((float *)sceTmp, (float *)scePi, q1);      
    MysceVu0AddVector((float *)sceZi1, (float *)sceZi1, (float *)sceTmp);      
#else
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
#endif
      /* advance pointers */
      ell += 1;
      ex += 1;
    }
    /* finish computing the X(i) block */
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
    p1 = ell[lskip1];
    Z21 = ex[1] - Z21 - p1*Z11;
    ex[1] = Z21;
    p1 = ell[lskip2];
    p2 = ell[1+lskip2];
    Z31 = ex[2] - Z31 - p1*Z11 - p2*Z21;
    ex[2] = Z31;
    p1 = ell[lskip3];
    p2 = ell[1+lskip3];
    p3 = ell[2+lskip3];
    Z41 = ex[3] - Z41 - p1*Z11 - p2*Z21 - p3*Z31;
    ex[3] = Z41;
    /* end of outer loop */
  }
  /* compute rows at end that are not a multiple of block size */
  for (; i < n; i++) {
    /* compute all 1 x 1 block of X, from rows i..i+1-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    ell = L + i*lskip1;
    ex = B;
    /* the inner loop that computes outer products and adds them to Z */
#ifndef OPTIM_WHEN_N_INFERIOR_8
    for (j=i-12; j >= 0; j -= 12) {
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[1];
      q1=ex[1];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[2];
      q1=ex[2];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[3];
      q1=ex[3];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[4];
      q1=ex[4];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[5];
      q1=ex[5];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[6];
      q1=ex[6];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[7];
      q1=ex[7];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[8];
      q1=ex[8];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[9];
      q1=ex[9];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[10];
      q1=ex[10];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* load p and q values */
      p1=ell[11];
      q1=ex[11];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* advance pointers */
      ell += 12;
      ex += 12;
      /* end of inner loop */
    }
    /* compute left-over iterations */
    j += 12;
#else
    assert ( n < 12 );
    j = i;
#endif

    for (; j > 0; j--) {
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      Z11 += p1 * q1;
      /* advance pointers */
      ell += 1;
      ex += 1;
    }
    /* finish computing the X(i) block */
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
  }
}


/* solve L^T * x=b, with b containing 1 right hand side.
 * L is an n*n lower triangular matrix with ones on the diagonal.
 * L is stored by rows and its leading dimension is lskip.
 * b is an n*1 matrix that contains the right hand side.
 * b is overwritten with x.
 * this processes blocks of 4.
 */

void LCPMath::dSolveL1T3 (const dReal *L, dReal *B, int lskip1)
{
	// sv. (debug)
	// oSerie.Add( (float) n , 1.0f);

  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,m11,p1,q1,*ex;
  const dReal *ell;
  /* special handling for L and B because we're solving L1 *transpose* */
  L = L + ((lskip1+1)<<1);
  B = B + 2;
  lskip1 = -lskip1;

  /* compute rows at end that are not a multiple of block size */

	// i=0
	// Nothing to do
	
	// i=1
    /* compute all 1 x 1 block of X, from rows i..i+1-1 */
    /* set the Z matrix to 0 */
    ell = L - 1;
    ex = B;
    /* compute left-over iterations */
	// J=1
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
    /* finish computing the X(i) block */
    ex[-1] = ex[-1] - m11;

	// i=2
    /* compute all 1 x 1 block of X, from rows i..i+1-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    ell = L - 2;
    ex = B;
    /* compute left-over iterations */
	// J=2
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[lskip1];
      q1=ex[-1];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      Z11 += m11;
    /* finish computing the X(i) block */
    Z11 = ex[-2] - Z11;
    ex[-2] = Z11;
}

void LCPMath::dSolveL1T4 (const dReal *L, dReal *B, int lskip1)
{
	// sv. (debug)
	// oSerie.Add( (float) n , 1.0f);

  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,Z21,Z31,Z41,p1,p2,p3,*ex;
  const dReal *ell;
  dReal aux0, aux1;
  int lskip2;
  /* special handling for L and B because we're solving L1 *transpose* */
  L = L + 3*(lskip1+1);
  B = B + 3;
  lskip1 = -lskip1;
  /* compute lskip values */
  lskip2 = (lskip1<<1);

  /* compute all 4 x 1 blocks of X */

	// i=0
    /* compute all 4 x 1 block of X, from rows i..i+4-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    Z21=0;
    Z31=0;
    Z41=0;
    ell = L;
    ex = B;
    /* finish computing the X(i) block */
    p1 = ell[-1];
    aux0 = p1*ex[0];
    Z21 = ex[-1] - aux0;
    ex[-1] = Z21;
    p1 = ell[-2];
    p2 = ell[-2+lskip1];
	aux1 = p2*Z21;
    Z31 = ex[-2] - aux0 - aux1;
    ex[-2] = Z31;
    p1 = ell[-3];
    p2 = ell[-3+lskip1];
    p3 = ell[-3+lskip2];
    Z41 = ex[-3] - aux0 - aux1 - p3*Z31;
    ex[-3] = Z41;
    /* end of outer loop */	
}

void LCPMath::dSolveL1T5 (const dReal *L, dReal *B, int lskip1)
{
	// sv. (debug)
	// oSerie.Add( (float) n , 1.0f);

  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,m11,Z21,Z31,Z41,p1,q1,p2,p3,*ex;
  dReal aux0, aux1;
  const dReal *ell;
  int lskip2,lskip3;
  /* special handling for L and B because we're solving L1 *transpose* */
  L = L + ((lskip1+1)<<2);
  B = B + 4;
  lskip1 = -lskip1;
  /* compute lskip values */
  lskip2 = (lskip1<<1);
  lskip3 = lskip2+lskip1;

  /* compute all 4 x 1 blocks of X */

	// i=0
    /* compute all 4 x 1 block of X, from rows i..i+4-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    Z21=0;
    Z31=0;
    Z41=0;
    ell = L;
    ex = B;
    /* finish computing the X(i) block */
    p1 = ell[-1];
    aux0 = p1*ex[0];
    Z21 = ex[-1] - aux0;
    ex[-1] = Z21;
    p1 = ell[-2];
    p2 = ell[-2+lskip1];
	aux1 = p2*Z21;
    Z31 = ex[-2] - aux0 - aux1;
    ex[-2] = Z31;
    p1 = ell[-3];
    p2 = ell[-3+lskip1];
    p3 = ell[-3+lskip2];
    Z41 = ex[-3] - aux0 - aux1 - p3*Z31;
    ex[-3] = Z41;
    /* end of outer loop */	

  /* compute rows at end that are not a multiple of block size */
	
	// i=4
    /* compute all 1 x 1 block of X, from rows i..i+1-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    ell = L - 4;
    ex = B;
    /* the inner loop that computes outer products and adds them to Z */
	// j=0
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-1];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-2];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-3];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      ex -= 4;
      Z11 += m11;
      /* end of inner loop */
    /* finish computing the X(i) block */
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
}

void LCPMath::dSolveL1T6 (const dReal *L, dReal *B, int lskip1)
{
	// sv. (debug)
	// oSerie.Add( (float) n , 1.0f);

  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,m11,Z21,Z31,Z41,p1,q1,p2,p3,*ex;
  const dReal *ell;
  dReal aux0, aux1;
  int lskip2,lskip3;
  /* special handling for L and B because we're solving L1 *transpose* */
  L = L + 5*(lskip1+1);
  B = B + 5;
  lskip1 = -lskip1;
  /* compute lskip values */
  lskip2 = (lskip1<<1);
  lskip3 = lskip2+lskip1;

  /* compute all 4 x 1 blocks of X */
	// i=0
    /* compute all 4 x 1 block of X, from rows i..i+4-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    Z21=0;
    Z31=0;
    Z41=0;
    ell = L;
    ex = B;
    /* finish computing the X(i) block */
    p1 = ell[-1];
    aux0 = p1*ex[0];
    Z21 = ex[-1] - aux0;
    ex[-1] = Z21;
    p1 = ell[-2];
    p2 = ell[-2+lskip1];
	aux1 = p2*Z21;
    Z31 = ex[-2] - aux0 - aux1;
    ex[-2] = Z31;
    p1 = ell[-3];
    p2 = ell[-3+lskip1];
    p3 = ell[-3+lskip2];
    Z41 = ex[-3] - aux0 - aux1 - p3*Z31;
    ex[-3] = Z41;
    /* end of outer loop */	
	
  /* compute rows at end that are not a multiple of block size */

	// i=4
    /* compute all 1 x 1 block of X, from rows i..i+1-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    ell = L - 4;
    ex = B;
    /* the inner loop that computes outer products and adds them to Z */
	// j=0
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-1];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-2];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-3];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      ex -= 4;
      Z11 += m11;
      /* end of inner loop */
    /* finish computing the X(i) block */
    Z11 = ex[0] - Z11;
    ex[0] = Z11;

	// i=5
    /* compute all 1 x 1 block of X, from rows i..i+1-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    ell = L - 5;
    ex = B;
    /* the inner loop that computes outer products and adds them to Z */
	// j=1
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-1];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-2];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-3];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      ex -= 4;
      Z11 += m11;
      /* end of inner loop */
    /* finish computing the X(i) block */
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
}

void LCPMath::dSolveL1T (const dReal *L, dReal *B, int n, int lskip1)
{  
	// sv. (debug)
	// oSerie.Add( (float) n , 1.0f);


  /* declare variables - Z matrix, p and q vectors, etc */
  dReal Z11,m11,Z21,m21,Z31,m31,Z41,m41,p1,q1,p2,p3,p4,*ex;
  const dReal *ell;
  int lskip2,lskip3,i,j;
  /* special handling for L and B because we're solving L1 *transpose* */
  L = L + (n-1)*(lskip1+1);
  B = B + n-1;
  lskip1 = -lskip1;
  /* compute lskip values */
  lskip2 = 2*lskip1;
  lskip3 = 3*lskip1;
  /* compute all 4 x 1 blocks of X */
  for (i=0; i <= n-4; i+=4) {
    /* compute all 4 x 1 block of X, from rows i..i+4-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    Z21=0;
    Z31=0;
    Z41=0;
    ell = L - i;
    ex = B;
    /* the inner loop that computes outer products and adds them to Z */
#ifndef OPTIM_WHEN_N_INFERIOR_8
    for (j=i-4; j >= 0; j -= 4) {
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      p2=ell[-1];
      p3=ell[-2];
      p4=ell[-3];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      m21 = p2 * q1;
      m31 = p3 * q1;
      m41 = p4 * q1;
      ell += lskip1;
      Z11 += m11;
      Z21 += m21;
      Z31 += m31;
      Z41 += m41;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-1];
      p2=ell[-1];
      p3=ell[-2];
      p4=ell[-3];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      m21 = p2 * q1;
      m31 = p3 * q1;
      m41 = p4 * q1;
      ell += lskip1;
      Z11 += m11;
      Z21 += m21;
      Z31 += m31;
      Z41 += m41;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-2];
      p2=ell[-1];
      p3=ell[-2];
      p4=ell[-3];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      m21 = p2 * q1;
      m31 = p3 * q1;
      m41 = p4 * q1;
      ell += lskip1;
      Z11 += m11;
      Z21 += m21;
      Z31 += m31;
      Z41 += m41;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-3];
      p2=ell[-1];
      p3=ell[-2];
      p4=ell[-3];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      m21 = p2 * q1;
      m31 = p3 * q1;
      m41 = p4 * q1;
      ell += lskip1;
      ex -= 4;
      Z11 += m11;
      Z21 += m21;
      Z31 += m31;
      Z41 += m41;
      /* end of inner loop */
    }
    /* compute left-over iterations */
    j += 4;
#else
    assert( n < 8);
    j=i;
#endif
    for (; j > 0; j--) {
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      p2=ell[-1];
      p3=ell[-2];
      p4=ell[-3];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      m21 = p2 * q1;
      m31 = p3 * q1;
      m41 = p4 * q1;
      ell += lskip1;
      ex -= 1;
      Z11 += m11;
      Z21 += m21;
      Z31 += m31;
      Z41 += m41;
    }
    /* finish computing the X(i) block */
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
    p1 = ell[-1];
    Z21 = ex[-1] - Z21 - p1*Z11;
    ex[-1] = Z21;
    p1 = ell[-2];
    p2 = ell[-2+lskip1];
    Z31 = ex[-2] - Z31 - p1*Z11 - p2*Z21;
    ex[-2] = Z31;
    p1 = ell[-3];
    p2 = ell[-3+lskip1];
    p3 = ell[-3+lskip2];
    Z41 = ex[-3] - Z41 - p1*Z11 - p2*Z21 - p3*Z31;
    ex[-3] = Z41;
    /* end of outer loop */
  }
  /* compute rows at end that are not a multiple of block size */
  for (; i < n; i++) {
    /* compute all 1 x 1 block of X, from rows i..i+1-1 */
    /* set the Z matrix to 0 */
    Z11=0;
    ell = L - i;
    ex = B;
    /* the inner loop that computes outer products and adds them to Z */

    for (j=i-4; j >= 0; j -= 4) {
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-1];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-2];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      /* load p and q values */
      p1=ell[0];
      q1=ex[-3];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      ex -= 4;
      Z11 += m11;
      /* end of inner loop */
    }
    /* compute left-over iterations */
    j += 4;
    for (; j > 0; j--) {
      /* load p and q values */
      p1=ell[0];
      q1=ex[0];
      /* compute outer product and add it to the Z matrix */
      m11 = p1 * q1;
      ell += lskip1;
      ex -= 1;
      Z11 += m11;
    }
    /* finish computing the X(i) block */
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
  }
}

//#endif // WIN32

void LCPMath::dSolveLDLT (const dReal *L, const dReal *d, dReal *b, int n, int nskip)
{
  dAASSERT (L && d && b && n > 0 && nskip >= n);
  SolveL1 (L,b,n,nskip);
  dVectorScale (b,d,n);
  SolveL1T (L,b,n,nskip);
}

// macros for dLDLTRemove() for accessing A - either access the matrix
// directly or access it via row pointers. we are only supposed to reference
// the lower triangle of A (it is symmetric), but indexes i and j come from
// permutation vectors so they are not predictable. so do a test on the
// indexes - this should not slow things down too much, as we don't do this
// in an inner loop.

#define _GETA(i,j) (A[i][j])
//#define _GETA(i,j) (A[(i)*nskip+(j)])
#define GETA(i,j) ((i > j) ? _GETA(i,j) : _GETA(j,i))

extern dReal *dLDLTRemove_a;
extern dReal *dLDLTRemove_t;

void LCPMath::dLDLTRemove (dReal **A, const int *p, dReal *L, dReal *d,
		  int n1, int n2, int r, int nskip)
{
  int i;
  dAASSERT(A && p && L && d && n1 > 0 && n2 > 0 && r >= 0 && r < n2 &&
	   n1 >= n2 && nskip >= n1);
  #ifndef dNODEBUG
  for (i=0; i<n2; i++) dIASSERT(p[i] >= 0 && p[i] < n1);
  #endif

  if (r==n2-1) {
    return;		// deleting last row/col is easy
  }
  else if (r==0) {    
	//dReal *a = (dReal*) ALLOCA (n2 * sizeof(dReal));
	  //static alloc:

	dIASSERT( n2 < LCP_MAX_N );
	dReal * a = dLDLTRemove_a;

    for (i=0; i<n2; i++) a[i] = -GETA(p[i],p[0]);
    a[0] += REAL(1.0);
    dLDLTAddTL (L,d,a,n2,nskip);
  }
  else {
	// static alloc
	  dIASSERT( r < LCP_MAX_N );
	  dReal * t = dLDLTRemove_t;

	  dIASSERT( (n2-r) < LCP_MAX_N );
	  dReal * a = dLDLTRemove_a;

	  /*
    dReal *t = (dReal*) ALLOCA (r * sizeof(dReal));
    dReal *a = (dReal*) ALLOCA ((n2-r) * sizeof(dReal));
	*/

	

    for (i=0; i<r; i++) t[i] = L[r*nskip+i] / d[i];
    for (i=0; i<(n2-r); i++)
      a[i] = dDot(L+(r+i)*nskip,t,r) - GETA(p[r+i],p[r]);
    a[0] += REAL(1.0);
    dLDLTAddTL (L + r*nskip+r, d+r, a, n2-r, nskip);
  }

  // snip out row/column r from L and d
  dRemoveRowCol (L,n2,nskip,r);
  if (r < (n2-1)) memmove (d+r,d+r+1,(n2-r-1)*sizeof(dReal));
}


extern dReal *dLDLTAddTL_W1;
extern dReal *dLDLTAddTL_W2;

void LCPMath::dLDLTAddTL (dReal *L, dReal *d, const dReal *a, int n, int nskip)
{
  int j,p;
  dReal *W1,*W2,W11,W21,alpha1,alpha2,alphanew,gamma1,gamma2,k1,k2,Wp,ell,dee;
  dAASSERT (L && d && a && n > 0 && nskip >= n);

  if (n < 2) return;
  /*
  W1 = (dReal*) ALLOCA (n*sizeof(dReal));
  W2 = (dReal*) ALLOCA (n*sizeof(dReal));
  */
  // static alloc
  	dIASSERT( n < LCP_MAX_N );
	W1 = dLDLTAddTL_W1;
	W2 = dLDLTAddTL_W2;

  W1[0] = 0;
  W2[0] = 0;
  for (j=1; j<n; j++) W1[j] = W2[j] = a[j] * M_Sqrt1_2;
  W11 = (REAL(0.5)*a[0]+1)*M_Sqrt1_2;
  W21 = (REAL(0.5)*a[0]-1)*M_Sqrt1_2;

  alpha1=1;
  alpha2=1;

  dee = d[0];
  alphanew = alpha1 + (W11*W11)*dee;
  dee /= alphanew;
  gamma1 = W11 * dee;
  dee *= alpha1;
  alpha1 = alphanew;
  alphanew = alpha2 - (W21*W21)*dee;
  dee /= alphanew;
  gamma2 = W21 * dee;
  alpha2 = alphanew;
  k1 = REAL(1.0) - W21*gamma1;
  k2 = W21*gamma1*W11 - W21;
  for (p=1; p<n; p++) {
    Wp = W1[p];
    ell = L[p*nskip];
    W1[p] =    Wp - W11*ell;
    W2[p] = k1*Wp +  k2*ell;
  }

  for (j=1; j<n; j++) {
    dee = d[j];
    alphanew = alpha1 + (W1[j]*W1[j])*dee;
    dee /= alphanew;
    gamma1 = W1[j] * dee;
    dee *= alpha1;
    alpha1 = alphanew;
    alphanew = alpha2 - (W2[j]*W2[j])*dee;
    dee /= alphanew;
    gamma2 = W2[j] * dee;
    dee *= alpha2;
    d[j] = dee;
    alpha2 = alphanew;

    k1 = W1[j];
    k2 = W2[j];
    for (p=j+1; p<n; p++) {
      ell = L[p*nskip+j];
      Wp = W1[p] - k1 * ell;
      ell += gamma1 * Wp;
      W1[p] = Wp;
      Wp = W2[p] - k2 * ell;
      ell -= gamma2 * Wp;
      W2[p] = Wp;
      L[p*nskip+j] = ell;
    }
  }
}

void LCPMath::dRemoveRowCol (dReal *A, int n, int nskip, int r)
{
  int i;
  dAASSERT(A && n > 0 && nskip >= n && r >= 0 && r < n);
  if (r >= n-1) return;
  if (r > 0) {
    for (i=0; i<r; i++)
      memmove (A+i*nskip+r,A+i*nskip+r+1,(n-r-1)*sizeof(dReal));
    for (i=r; i<(n-1); i++)
      memcpy (A+i*nskip,A+i*nskip+nskip,r*sizeof(dReal));
  }
  for (i=r; i<(n-1); i++)
    memcpy (A+i*nskip+r,A+i*nskip+nskip+r+1,(n-r-1)*sizeof(dReal));
}

void LCPMath::dMakeRandomMatrix (dReal *A, int n, int m, dReal range)
{
  int i,j;
  int skip = dPAD(m);
  dSetZero (A,n*skip);
  for (i=0; i<n; i++) {
    for (j=0; j<m; j++) A[i*skip+j] = (dRandReal()*REAL(2.0)-REAL(1.0))*range;
  }
}

void LCPMath::dClearUpperTriangle (dReal *A, int n)
{
  int i,j;
  int skip = dPAD(n);
  for (i=0; i<n; i++) {
    for (j=i+1; j<n; j++) A[i*skip+j] = 0;
  }
}

void LCPMath::dMultiply0 (dReal *A, const dReal *B, const dReal *C, int p, int q, int r)
{
  int i,j,k,qskip,rskip,rpad;
  dAASSERT (A && B && C && p>0 && q>0 && r>0);
  qskip = dPAD(q);
  rskip = dPAD(r);
  rpad = rskip - r;
  dReal sum;
  const dReal *b,*c,*bb;
  bb = B;
  for (i=p; i; i--) {
    for (j=0 ; j<r; j++) {
      c = C + j;
      b = bb;
      sum = 0;
      for (k=q; k; k--, c+=rskip) sum += (*(b++))*(*c);
      *(A++) = sum; 
    }
    A += rpad;
    bb += qskip;
  }
}


void LCPMath::dMultiply1 (dReal *A, const dReal *B, const dReal *C, int p, int q, int r)
{
  int i,j,k,pskip,rskip;
  dReal sum;
  dAASSERT (A && B && C && p>0 && q>0 && r>0);
  pskip = dPAD(p);
  rskip = dPAD(r);
  for (i=0; i<p; i++) {
    for (j=0; j<r; j++) {
      sum = 0;
      for (k=0; k<q; k++) sum += B[i+k*pskip] * C[j+k*rskip];
      A[i*rskip+j] = sum;
    }
  }
}

void LCPMath::dMultiply2 (dReal *A, const dReal *B, const dReal *C, int p, int q, int r)
{
  int i,j,k,z,rpad,qskip;
  dReal sum;
  const dReal *bb,*cc;
  dAASSERT (A && B && C && p>0 && q>0 && r>0);
  rpad = dPAD(r) - r;
  qskip = dPAD(q);
  bb = B;
  for (i=p; i; i--) {
    cc = C;
    for (j=r; j; j--) {
      z = 0;
      sum = 0;
      for (k=q; k; k--,z++) sum += bb[z] * cc[z];
      *(A++) = sum; 
      cc += qskip;
    }
    A += rpad;
    bb += qskip;
  }
}

//****************************************************************************
// random numbers

static ULONG seed = 0;

ULONG LCPMath::dRand()
{
  seed = (1664525L*seed + 1013904223L) & 0xffffffff;
  return seed;
}

ULONG  LCPMath::dRandGetSeed()
{
  return seed;
}


void LCPMath::dRandSetSeed (ULONG s)
{
  seed = s;
}

#ifdef PSX2_TARGET
int LCPMath::dRandInt (int n)
{
  dReal a = dReal(n) / 4294967296.0f;
  return (int) ((dReal)dRand() * a);
}
#else
int LCPMath::dRandInt (int n)
{
  double a = double(n) / 4294967296.0;
  return (int) (double(dRand()) * a);
}
#endif


dReal LCPMath::dRandReal()
{
  return ((dReal) dRand()) / ((dReal) 0xffffffff);
}

dReal LCPMath::dMaxDifference (const dReal *A, const dReal *B, int n, int m)
{
  int i,j;
  int skip = dPAD(m);
  dReal diff,max;
  max = 0;
  for (i=0; i<n; i++) 
  {
    for (j=0; j<m; j++) 
	{
      diff = fabsf (A[i*skip+j] - B[i*skip+j]);
      if (diff > max) max = diff;
    }
  }
  return max;
}

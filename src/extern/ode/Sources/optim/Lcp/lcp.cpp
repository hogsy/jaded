/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001 Russell L. Smith.            *
 *   Email: russ@q12.org   Web: www.q12.org                              *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU Lesser General Public            *
 * License as published by the Free Software Foundation; either          *
 * version 2.1 of the License, or (at your option) any later version.    *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 * Lesser General Public License for more details.                       *
 *                                                                       *
 * You should have received a copy of the GNU Lesser General Public      *
 * License along with this library (see the file LICENSE.TXT); if not,   *
 * write to the Free Software Foundation, Inc., 59 Temple Place,         *
 * Suite 330, Boston, MA 02111-1307 USA.                                 *
 *                                                                       *
 *************************************************************************/

/*


THE ALGORITHM
-------------

solve A*x = b+w, with x and w subject to certain LCP conditions.
each x(i),w(i) must lie on one of the three line segments in the following
diagram. each line segment corresponds to one index set :

     w(i)
     /|\      |           :
      |       |           :
      |       |i in N     :
  w>0 |       |state[i]=0 :
      |       |           :
      |       |           :  i in C
  w=0 +       +-----------------------+
      |                   :           |
      |                   :           |
  w<0 |                   :           |i in N
      |                   :           |state[i]=1
      |                   :           |
      |                   :           |
      +-------|-----------|-----------|----------> x(i)
             lo           0           hi

the Dantzig algorithm proceeds as follows:
  for i=1:n
    * if (x(i),w(i)) is not on the line, push x(i) and w(i) positive or
      negative towards the line. as this is done, the other (x(j),w(j))
      for j<i are constrained to be on the line. if any (x,w) reaches the
      end of a line segment then it is switched between index sets.
    * i is added to the appropriate index set depending on what line segment
      it hits.

we restrict lo(i) <= 0 and hi(i) >= 0. this makes the algorithm a bit
simpler, because the starting point for x(i),w(i) is always on the dotted
line x=0 and x will only ever increase in one direction, so it can only hit
two out of the three line segments.


NOTES
-----

this is an implementation of "lcp_dantzig2_ldlt.m" and "lcp_dantzig_lohi.m".
the implementation is split into an LCP problem object (dLCP) and an LCP
driver function. most optimization occurs in the dLCP object.

a naive implementation of the algorithm requires either a lot of data motion
or a lot of permutation-array lookup, because we are constantly re-ordering
rows and columns. to avoid this and make a more optimized algorithm, a
non-trivial data structure is used to represent the matrix A (this is
implemented in the fast version of the dLCP object).

during execution of this algorithm, some indexes in A are clamped (set C),
some are non-clamped (set N), and some are "don't care" (where x=0).
A,x,b,w (and other problem vectors) are permuted such that the clamped
indexes are first, the unclamped indexes are next, and the don't-care
indexes are last. this permutation is recorded in the array `p'.
initially p = 0..n-1, and as the rows and columns of A,x,b,w are swapped,
the corresponding elements of p are swapped.

because the C and N elements are grouped together in the rows of A, we can do
lots of work with a fast dot product function. if A,x,etc were not permuted
and we only had a permutation array, then those dot products would be much
slower as we would have a permutation array lookup in some inner loops.

A is accessed through an array of row pointers, so that element (i,j) of the
permuted matrix is A[i][j]. this makes row swapping fast. for column swapping
we still have to actually move the data.

during execution of this algorithm we maintain an L*D*L' factorization of
the clamped submatrix of A (call it `AC') which is the top left nC*nC
submatrix of A. there are two ways we could arrange the rows/columns in AC.

(1) AC is always permuted such that L*D*L' = AC. this causes a problem
    when a row/column is removed from C, because then all the rows/columns of A
    between the deleted index and the end of C need to be rotated downward.
    this results in a lot of data motion and slows things down.
(2) L*D*L' is actually a factorization of a *permutation* of AC (which is
    itself a permutation of the underlying A). this is what we do - the
    permutation is recorded in the vector C. call this permutation A[C,C].
    when a row/column is removed from C, all we have to do is swap two
    rows/columns and manipulate C.

*/

//#include "Precomp.h"
#ifdef JADEFUSION
#include "projectconfig.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIxxx.h"
#endif

//#endif
//#include <ode/common.h>

#include "lcp.h"
#include "lcperr.h" // dMessage()
#include <stdio.h>

#ifdef PSX2_TARGET
#ifdef __cplusplus
extern "C" {
#endif
asm void *ps2memcpy(char *dst, char*src, int size);
asm void *ps2memset(char *dst, unsigned char c, int size);
#ifdef __cplusplus
}
#endif

#endif

#ifdef PS2
    // jm. Vu0
    #include "lcp_vu0.h"
	#define PS2MACRO       
#endif


#ifdef GCN
	#define FactorLDLT(x,y,n,z)  if(n==3) LCPMath::dFactorLDLT3(x,y,z); \
								 else if(n==4) LCPMath::dFactorLDLT4(x,y,z); \
								 else if(n==5) LCPMath::dFactorLDLT5(x,y,z); \
								 else if(n==6) LCPMath::dFactorLDLT6(x,y,z); \
								 else LCPMath::dFactorLDLT(x,y,n,z);
#else
	#define FactorLDLT(x,y,n,z)	LCPMath::dFactorLDLT(x,y,n,z);
#endif

#ifdef GCN
	#define SolveL1(x,y,n,z)  	 if(n==3) LCPMath::dSolveL13(x,y,z); \
								 else if(n==4) LCPMath::dSolveL14(x,y,z); \
								 else if(n==5) LCPMath::dSolveL15(x,y,z); \
								 else if(n==6) LCPMath::dSolveL16(x,y,z); \
								 else LCPMath::dSolveL1(x,y,n,z);							 
#else
	#define SolveL1(x,y,n,z)	LCPMath::dSolveL1(x,y,n,z);
#endif

#ifdef GCN
	#define SolveL1T(x,y,n,z)  	 if(n==3) LCPMath::dSolveL1T3(x,y,z); \
								 else if(n==4) LCPMath::dSolveL1T4(x,y,z); \
								 else if(n==5) LCPMath::dSolveL1T5(x,y,z); \
								 else if(n==6) LCPMath::dSolveL1T6(x,y,z); \
								 else LCPMath::dSolveL1T(x,y,n,z);							 
#else
	#define SolveL1T(x,y,n,z)	LCPMath::dSolveL1T(x,y,n,z);
#endif

//#include <ode/matrix.h>
//#include <ode/misc.h>
//#include "mat.h"		// for testing
//#include <ode/timer.h>		// for testing

#include <assert.h>
#ifdef JADEFUSION
#if !defined(dAASSERT)
#define dAASSERT assert
#endif
#if !defined(dIASSERT)
#define dIASSERT assert 
#endif
#else
#define dIASSERT assert
#define	dAASSERT assert
#endif
//***************************************************************************
// code generation parameters

// LCP debugging (mosty for fast dLCP) - this slows things down a lot
//#define DEBUG_LCP

// option 1 : matrix row pointers (less data copying) 
// ROWPTRS + dLCP_FAST + NUB_OPTIMIZATIONS


//**************************************************************************


static dReal *g_pfUnpermuteTmp;
static dReal *g_pfdSolveLCP_L;
static dReal *g_pfdSolveLCP_d;
static dReal *g_pfdSolveLCP_deltax;
static dReal *g_pfdSolveLCP_deltaw;
static dReal *g_pfdSolveLCP_Dell;
static dReal *g_pfdSolveLCP_ell;
static dReal *g_pfdSolveLCP_tmp;
static dReal **g_pfdSolveLCP_Arows;
static int *g_pidSolveLCP_p;
static int *g_pidSolveLCP_C;

//static  int *g_pidSolveLCP_dummy;
static int *g_pidSolveLCP_state;

// Allocations for LCPMATH : dLDLTAddTL()  
dReal *dLDLTAddTL_W1;
dReal *dLDLTAddTL_W2;

// Allocations for LCPMATH : dLDLTRemove()  
dReal *dLDLTRemove_a;
dReal *dLDLTRemove_t;


void LCP_StaticInit()
{

// Allocations for this module
	g_pfUnpermuteTmp = (dReal*) __ALLOCA ( LCP_MAX_N * sizeof(dReal));
	g_pfdSolveLCP_L= (dReal*) __ALLOCA( LCP_MAX_N * (dPAD(LCP_MAX_N)) * sizeof(dReal));
	g_pfdSolveLCP_d = (dReal*) __ALLOCA( LCP_MAX_N * sizeof( dReal));
	g_pfdSolveLCP_deltax = (dReal*) __ALLOCA( LCP_MAX_N * sizeof( dReal));
	g_pfdSolveLCP_deltaw = (dReal*) __ALLOCA( LCP_MAX_N * sizeof( dReal));
	g_pfdSolveLCP_Dell = (dReal*) __ALLOCA( LCP_MAX_N * sizeof( dReal));
	g_pfdSolveLCP_ell = (dReal*) __ALLOCA( LCP_MAX_N * sizeof( dReal));
	g_pfdSolveLCP_tmp = (dReal*) __ALLOCA( LCP_MAX_N * sizeof( dReal));
	g_pfdSolveLCP_Arows = (dReal**) __ALLOCA( LCP_MAX_N * sizeof( dReal*));
	g_pidSolveLCP_p = (int*) __ALLOCA( LCP_MAX_N * sizeof(int));
	g_pidSolveLCP_C = (int*) __ALLOCA( LCP_MAX_N * sizeof(int));
//	g_pidSolveLCP_dummy = (int*) __ALLOCA( LCP_MAX_N * sizeof(int));
	g_pidSolveLCP_state= (int*) __ALLOCA ( LCP_MAX_N *sizeof(int));
  
// Allocations for LCPMATH : dLDLTAddTL()  
	dLDLTAddTL_W1 = (dReal*) __ALLOCA ( LCP_MAX_N * sizeof(dReal));
	dLDLTAddTL_W2 = (dReal*) __ALLOCA ( LCP_MAX_N * sizeof(dReal));

// Allocations for LCPMATH : dLDLTRemove()  
	dLDLTRemove_a = (dReal*) __ALLOCA ( LCP_MAX_N * sizeof(dReal));
	dLDLTRemove_t = (dReal*) __ALLOCA ( LCP_MAX_N * sizeof(dReal));
}

inline dReal * pfGetUnpermuteTmp(int _n)
{
	assert (_n < LCP_MAX_N);
	return g_pfUnpermuteTmp;
}

void LCP_StaticDone()
{
	/*
	FREE(g_pfUnpermuteTmp);
	FREE( g_pfdSolveLCP_L);
	FREE( g_pfdSolveLCP_d);
	FREE( g_pfdSolveLCP_deltax);
	FREE( g_pfdSolveLCP_deltaw);
	FREE( g_pfdSolveLCP_Dell);
	FREE( g_pfdSolveLCP_ell);
	FREE( g_pfdSolveLCP_tmp);
	FREE( g_pfdSolveLCP_Arows);
	FREE( g_pidSolveLCP_p);
	FREE( g_pidSolveLCP_C);
	// FREE( g_pidSolveLCP_dummy);
	FREE( g_pidSolveLCP_state);
	*/
}


//***************************************************************************

#ifdef PS2MACRO
#include <libvu0.h>
inline void MulVectorMacro(dReal *dst, dReal *vec1, dReal *vec2, int n)
{
	int iqwords, ii, j;
	sceVu0FVECTOR p1;			
	sceVu0FVECTOR p2;
	sceVu0FVECTOR dest;
	iqwords = n / 4;
	if (iqwords > 0)
	{						
		for (ii=0, j=0; j< iqwords; j++, ii+=4)
		{					
			p1[0] = vec1[ii];
			p1[1] = vec1[ii+1];
			p1[2] = vec1[ii+2];
			p1[3] = vec1[ii+3];
			p2[0] = vec2[ii];
			p2[1] = vec2[ii+1];
			p2[2] = vec2[ii+2];
			p2[3] = vec2[ii+3];
			asm __volatile__("
    			lqc2        vf1,p1
    			lqc2        vf2,p2
				vmul.xyzw	vf3,vf1,vf2
				sqc2        vf3,dest
			": : "r" (dest) , "r" (p1), "r" (p2));			
			dst[ii] = dest[0];
			dst[ii+1] = dest[1];
			dst[ii+2] = dest[2];
			dst[ii+3] = dest[3];					
		}		
	}
	//	EPILOGUE
	switch ( n % 4)
	{
	case 3:
		dst[n-3] = vec1[n-3]*vec2[n-3];
	case 2:
		dst[n-2] = vec1[n-2]*vec2[n-2];
	case 1:
		dst[n-1] = vec1[n-1]*vec2[n-1];
		break;
	}
}
#endif

// an alternative inline dot product, for speed comparisons

static inline dReal myDot (dReal *a, dReal *b, int n)
{
  dReal sum=0;
  while (n > 0) 
  {
    sum += (*a) * (*b);
    a++;
    b++;
    n--;
  }
  return sum;
}


// swap row/column i1 with i2 in the n*n matrix A. the leading dimension of
// A is nskip. this only references and swaps the lower triangle.
// if `do_fast_row_swaps' is nonzero and row pointers are being used, then
// rows will be swapped by exchanging row pointers. otherwise the data will
// be copied.

static void swapRowsAndCols (dReal** A, int n, int i1, int i2, int nskip,
			     int do_fast_row_swaps)
{
  int i;
  dIASSERT (A && n > 0 && i1 >= 0 && i2 >= 0 && i1 < n && i2 < n &&
	    nskip >= n && i1 < i2);

  for (i=i1+1; i<i2; i++) 
	  A[i1][i] = A[i][i1];
  
  for (i=i1+1; i<i2; i++) 
	  A[i][i1] = A[i2][i];

  A[i1][i2] = A[i1][i1];
  A[i1][i1] = A[i2][i1];
  A[i2][i1] = A[i2][i2];

  // swap rows, by swapping row pointers  
  dReal *tmpp;
  dReal tmp;
  if (do_fast_row_swaps) 
  {
    
    tmpp = A[i1];
    A[i1] = A[i2];
    A[i2] = tmpp;
  }
  else 
  {
    dReal *tmprow = (dReal*) __ALLOCA (n * sizeof(dReal));
    memcpy (tmprow,A[i1],n * sizeof(dReal));
    memcpy (A[i1],A[i2],n * sizeof(dReal));
    memcpy (A[i2],tmprow,n * sizeof(dReal));
	FREE( tmprow);
  }

  // swap columns the hard way  
  for (i=i2+1; i<n; i++) 
  {
    tmp = A[i][i1];
    A[i][i1] = A[i][i2];
    A[i][i2] = tmp;
  }
}


// swap two indexes in the n*n LCP problem. i1 must be <= i2.

inline static void swapProblem (dReal** A, dReal *x, dReal *b, dReal *w, dReal *lo,
			 dReal *hi, int *p, int *state, int *findex,
			 int n, int i1, int i2, int nskip,
			 int do_fast_row_swaps)
{
  dReal tmp;
  int tmpi;
  dIASSERT (n>0 && i1 >=0 && i2 >= 0 && i1 < n && i2 < n && nskip >= n &&
	    i1 <= i2);

  if (i1==i2) 
	  return;

  swapRowsAndCols (A,n,i1,i2,nskip,do_fast_row_swaps);
  
  // swap (x[i1], x[i2])
  tmp = x[i1];  
  x[i1] = x[i2];
  x[i2] = tmp;

  // swap (b[i1], b[i2])
  tmp = b[i1];
  b[i1] = b[i2];
  b[i2] = tmp;

  // swap (w[i1], w[i2])
  tmp = w[i1];
  w[i1] = w[i2];
  w[i2] = tmp;

  // swap (lo[i1], lo[i2])
  tmp = lo[i1];
  lo[i1] = lo[i2];
  lo[i2] = tmp;

  // swap (hi[i1], hi[i2])
  tmp = hi[i1];
  hi[i1] = hi[i2];
  hi[i2] = tmp;

  // swap (p[i1], p[i2])
  tmpi = p[i1];
  p[i1] = p[i2];
  p[i2] = tmpi;

  // swap (state[i1], state[i2])
  tmpi = state[i1];
  state[i1] = state[i2];
  state[i2] = tmpi;

  if (findex) 
  {
	  // swap (findex[i1], findex[i2])
    tmpi = findex[i1];
    findex[i1] = findex[i2];
    findex[i2] = tmpi;
  }
}


// for debugging - check that L,d is the factorization of A[C,C].
// A[C,C] has size nC*nC and leading dimension nskip.
// L has size nC*nC and leading dimension nskip.
// d has size nC.

#ifdef DEBUG_LCP

static void checkFactorization (dReal** A, dReal *_L, dReal *_d,
				int nC, int *C, int nskip)
{
  int i,j;
  if (nC==0) return;

  // get A1=A, copy the lower triangle to the upper triangle, get A2=A[C,C]
  dMatrix A1 (nC,nC);
  for (i=0; i<nC; i++) {
    for (j=0; j<=i; j++) A1(i,j) = A1(j,i) = A[i][j];
  }
  dMatrix A2 = A1.select (nC,C,nC,C);

  // printf ("A1=\n"); A1.print(); printf ("\n");
  // printf ("A2=\n"); A2.print(); printf ("\n");

  // compute A3 = L*D*L'
  dMatrix L (nC,nC,_L,nskip,1);
  dMatrix D (nC,nC);
  
  for (i=0; i<nC; i++) 
	  D(i,i) = 1/_d[i];

  L.clearUpperTriangle();
  for (i=0; i<nC; i++) L(i,i) = 1;
  dMatrix A3 = L * D * L.transpose();

  // printf ("L=\n"); L.print(); printf ("\n");
  // printf ("D=\n"); D.print(); printf ("\n");
  // printf ("A3=\n"); A2.print(); printf ("\n");

  // compare A2 and A3
  dReal diff = A2.maxDifference (A3);
  if (diff > 1e-8)
    dDebug (0,"L*D*L' check, maximum difference = %.6e\n",diff);
}

#endif


// for debugging

#ifdef DEBUG_LCP

static void checkPermutations (int i, int n, int nC, int nN, int *p, int *C)
{
  int j,k;
  dIASSERT (nC>=0 && nN>=0 && (nC+nN)==i && i < n);
  for (k=0; k<i; k++) dIASSERT (p[k] >= 0 && p[k] < i);
  for (k=i; k<n; k++) dIASSERT (p[k] == k);
  for (j=0; j<nC; j++) {
    int C_is_bad = 1;
    for (k=0; k<nC; k++) if (C[k]==j) C_is_bad = 0;
    dIASSERT (C_is_bad==0);
  }
}

#endif


//#define _DEBUG_SPARSE_

#ifdef _DEBUG_SPARSE_

	// sv. debug
	static bool bDumpA = false;
	static int inumA = 0;
	#include "PointArray.h"
	static PointArrayOrd oSparseDistrib;

#endif




void dLCP::Init (int _n, int _nub, dReal *_Adata, dReal *_x, dReal *_b, dReal *_w,
	    dReal *_lo, dReal *_hi, dReal *_L, dReal *_d,
	    dReal *_Dell, dReal *_ell, dReal *_tmp,
	    int *_state, int *_findex, int *_p, int *_C, dReal **Arows)
{
  

	
  n = _n;
  nub = _nub;
  Adata = _Adata;
  A = 0;
  x = _x;
  b = _b;
  w = _w;
  lo = _lo;
  hi = _hi;
  L = _L;
  d = _d;
  Dell = _Dell;
  ell = _ell;
  tmp = _tmp;
  state = _state;
  findex = _findex;
  p = _p;
  C = _C;
  nskip = dPAD(n);
  LCPMath::dSetZero (x,n);
  
  int k;

  // make matrix row pointers
  A = Arows;
  for (k=0; k<n; k++) 
	  A[k] = Adata + k*nskip;

  nC = 0;
  nN = 0;
  for (k=0; k<n; k++) 
	  p[k]=k;		// initially unpermuted

  // permute the problem so that *all* the unbounded variables are at the
  // start, i.e. look for unbounded variables not included in `nub'. we can
  // potentially push up `nub' this way and get a bigger initial factorization.
  // note that when we swap rows/cols here we must not just swap row pointers,
  // as the initial factorization relies on the data being all in one chunk.
  // variables that have findex >= 0 are *not* considered to be unbounded even
  // if lo=-inf and hi=inf - this is because these limits may change during the
  // solution process.
  for (k=nub; k<n; k++) 
  {
	if (findex && findex[k] >= 0) continue;
    if (lo[k]==-dInfinity && hi[k]==dInfinity) 
	{
      swapProblem (A,x,b,w,lo,hi,p,state,findex,n,nub,k,nskip,0);
      nub++;
    }
  }

  // if there are unbounded variables at the start, factorize A up to that
  // point and solve for x. this puts all indexes 0..nub-1 into C.
  if (nub > 0) 
  {	
    for (k=0; k<nub; k++) 
		memcpy (L+k*nskip,A[k],(k+1)*sizeof(dReal));

	FactorLDLT (L,d,nub,nskip);
    memcpy (x,b,nub*sizeof(dReal));
	LCPMath::dSolveLDLT (L,d,x,nub,nskip);
	LCPMath::dSetZero (w,nub);
    	
	for (k=0; k<nub; k++) 
		C[k] = k;

    nC = nub;
  }

  // permute the indexes > nub such that all findex variables are at the end
  if (findex) 
  {
    int num_at_end = 0;
    for (k=n-1; k >= nub; k--) 
	{
      if (findex[k] >= 0) 
	  {
		swapProblem (A,x,b,w,lo,hi,p,state,findex,n,k,n-1-num_at_end,nskip,1);
		num_at_end++;
      }
    }
  }
}


void dLCP::transfer_i_to_C (int i)
{  
  if (nC > 0) 
  {
    // ell,Dell were computed by solve1(). note, ell = D \ L1solve (L,A(i,C))    
	/*
	for (j=0; j<nC; j++) 
		L[nC*nskip+j] = ell[j];
	*/

	// nC*nskip is constant
	dReal *pfL = L + nC*nskip;		
	memcpy(pfL, ell, sizeof(dReal) * nC);
	/*
	for (j=0; j<nC; j++) 
		pfL[j] = ell[j];
	*/
	
    d[nC] = dRecip (A[i][i] - LCPMath::dDot(ell,Dell,nC));
  }
  else 
  {
    d[0] = dRecip (A[i][i]);
  }
  swapProblem (A,x,b,w,lo,hi,p,state,findex,n,nC,i,nskip,1);
  C[nC] = nC;
  nC++;

#ifdef DEBUG_LCP
  checkFactorization (A,L,d,nC,C,nskip);
  if (i < (n-1)) 
	  checkPermutations (i+1,n,nC,nN,p,C);
#endif
}


void dLCP::transfer_i_from_N_to_C (int i)
{
  int j;
  if (nC > 0) 
  {
    dReal *aptr = A[i];

	// if nub>0, initial part of aptr unpermuted
	if (nub > 0)
		//	for (j=0; j<nub; j++) 
		//		Dell[j] = aptr[j];		
		memcpy(Dell, aptr, sizeof(dReal)* nub);
    
	for (j=nub; j<nC; j++) 
		Dell[j] = aptr[C[j]];

    SolveL1 (L,Dell,nC,nskip);

#ifndef PS2MACRO
	for (j=0; j<nC; j++) 
		ell[j] = Dell[j] * d[j];
#else
	MulVectorMacro(ell, Dell, d, nC);
#endif
    
	float *pfL = L + nC*nskip;

	/*for (j=0; j<nC; j++) 
	//	L[nC*nskip+j] = ell[j];
		pfL[j] = ell[j];
		*/
	
	// nC > 0 
	memcpy(pfL, ell, sizeof(dReal)*nC);

    d[nC] = dRecip (A[i][i] - LCPMath::dDot(ell,Dell,nC));
  }
  else 
  {
    d[0] = dRecip (A[i][i]);
  }
  swapProblem (A,x,b,w,lo,hi,p,state,findex,n,nC,i,nskip,1);
  C[nC] = nC;
  nN--;
  nC++;

  // @@@ TO DO LATER
  // if we just finish here then we'll go back and re-solve for
  // delta_x. but actually we can be more efficient and incrementally
  // update delta_x here. but if we do this, we wont have ell and Dell
  // to use in updating the factorization later.

# ifdef DEBUG_LCP
  checkFactorization (A,L,d,nC,C,nskip);
# endif
}


void dLCP::transfer_i_from_C_to_N (int i)
{
  // remove a row/column from the factorization, and adjust the
  // indexes (black magic!)
  int j,k;
  for (j=0; j<nC; j++) if (C[j]==i) 
  {
    LCPMath::dLDLTRemove (A,C,L,d,n,nC,j,nskip);
    for (k=0; k<nC; k++) 
		if (C[k]==nC-1) 
		{
			C[k] = C[j];
			if (j < (nC-1)) 
				memmove (C+j,C+j+1,(nC-j-1)*sizeof(int));
			break;
		}
    dIASSERT (k < nC);
    break;
  }
  dIASSERT (j < nC);
  swapProblem (A,x,b,w,lo,hi,p,state,findex,n,i,nC-1,nskip,1);
  nC--;
  nN++;

# ifdef DEBUG_LCP
  checkFactorization (A,L,d,nC,C,nskip);
# endif
}


void dLCP::pN_equals_ANC_times_qC (dReal *p, dReal *q)
{
  // we could try to make this matrix-vector multiplication faster using
  // outer product matrix tricks, e.g. with the dMultidotX() functions.
  // but i tried it and it actually made things slower on random 100x100
  // problems because of the overhead involved. so we'll stick with the
  // simple method for now.
  
	dReal *pfp = p + nC;
	dReal **pfA = A + nC;

 	for (int i=0; i<nN; i++) 
	  //p[i+nC] = dDot (A[i+nC],q,nC);
 	  pfp[i] = LCPMath::dDot (pfA[i],q,nC);
}


void dLCP::pN_plusequals_ANi (dReal *p, int i, int sign)
{
  dReal *aptr = A[i] + nC;
  dReal *pfp = p + nC;

  if (sign > 0) 
  {
    for (int i=0; i<nN; i++) 
		//p[i+nC] += aptr[i];
		pfp[i] += aptr[i];
  }
  else 
  {
    for (int i=0; i<nN; i++) 
//		p[i+nC] -= aptr[i];
		pfp[i] -= aptr[i];
  }
}




void dLCP::solve1 (dReal *a, int i, int dir, int only_transfer)
{
  // the `Dell' and `ell' that are computed here are saved. if index i is
  // later added to the factorization then they can be reused.
  //
  // @@@ question: do we need to solve for entire delta_x??? yes, but
  //     only if an x goes below 0 during the step.



  int j;
  if (nC <= 0) 
	  return;

   dReal *aptr = A[i];

   // if nub>0, initial part of aptr[] is guaranteed unpermuted
   /*
   for (j=0; j<nub; j++) 
		Dell[j] = aptr[j];
		*/
   if (nub >0)
	memcpy(Dell, aptr, sizeof(dReal) * nub);

   for (j=nub; j<nC; j++) 
		Dell[j] = aptr[C[j]];

   SolveL1 (L,Dell,nC,nskip);
    
#ifndef PS2MACRO
	for (j=0; j<nC; j++) 
		ell[j] = Dell[j] * d[j];
#else
	// PS2 MACROMODE		
	MulVectorMacro(ell, Dell, d, nC);
#endif

    if (!only_transfer) 
	{
      /*for (j=0; j<nC; j++) 
		  tmp[j] = ell[j];
		  */
		// Here nC > 0 allways
		memcpy(tmp, ell, sizeof(dReal)* nC);

		SolveL1T (L,tmp,nC,nskip);

		if (dir > 0) 
		{
			for (j=0; j<nC; j++) 
				a[C[j]] = -tmp[j];
		}
		else 
		{
			for (j=0; j<nC; j++) 
				a[C[j]] = tmp[j];
		}
	}
}


void dLCP::unpermute()
{
  // now we have to un-permute x and w
	int j;

	dReal *tmp = pfGetUnpermuteTmp(n);	

	memcpy (tmp,x,n*sizeof(dReal));

	for (j=0; j<n; j++) 
		x[p[j]] = tmp[j];

	memcpy (tmp,w,n*sizeof(dReal));

	for (j=0; j<n; j++) 
		w[p[j]] = tmp[j];
	
}

#ifdef PS2
static inline void memcpy_word_precision(u_long128 *_pDst,u_long128 *_pSrc,int _iWords)
{
	int iNbQW=(_iWords>>2);
	int	iRemainingWords=_iWords - (iNbQW<<2);
	memcpy128(_pDst,_pSrc,iNbQW); 
			
	if (iRemainingWords>0)
	{
		int *piDst=(int*)(_pDst + iNbQW);
		int *piSrc=(int*)(_pSrc + iNbQW);
		int i;
		for (i=0;i<iRemainingWords;i++)
			piDst[i] = piSrc[i];
	}
}
#endif


void dLCP::dSolveLCP_Fast (int n, dReal *A, dReal *x, dReal *rhs,
		dReal *w, int nub, dReal *lo, dReal *hi, int *findex)
{


	// an optimized Dantzig LCP driver routine for the lo-hi LCP problem.
	
	// LCP Solver replacement:
	// This algorithm goes like this:
	// Do a straightforward LDLT factorization of the matrix A, solving for
	// A*x = rhs
	// For each x[i] that is outside of the bounds of lo[i] and hi[i],
	//    clamp x[i] into that range.
	//    Substitute into A the now known x's
	//    subtract the residual away from the rhs.
	//    Remove row and column i from L, updating the factorization
	//    place the known x's at the end of the array, keeping up with location in p
	// Repeat until all constraints have been clamped or all are within bounds
	//
	// This is probably only faster in the single joint case where only one repeat is
	// the norm.

	// factorize A (L*D*L'=A)

	int mskip = dPAD(n);
	int m = n;
	int i, j;

	dReal t[6];
	dReal a[6];
	dReal d[6];
	dReal L[6*8];
	
	int iCpSz=m*mskip;
#ifdef PS2	
	if (iCpSz&0x3 == 0)
		memcpy128((u_long128*)L,(u_long128*)A,iCpSz>>2);
	else
		memcpy_word_precision((u_long128*)L,(u_long128*)A,iCpSz); // * sizeof(dReal));
#else
    memcpy(L,A,iCpSz* sizeof(dReal));
#endif
    
	FactorLDLT(L,d,m,mskip);

	// compute lambda
	
	int left = m;	//constraints left to solve.
	bool remove[8]//bool remove[6];
#ifdef PS2
__attribute__ ((aligned(16)))
#endif
;
	dReal lambda[6];
	
    int p[6] = {0,1,2,3,4,5};
	/*for (i = 0; i < 6; i++)
		p[i] = i;*/
	while (true)
	{
#ifdef PS2	
		memcpy_word_precision ((u_long128*)x,(u_long128*)rhs,left); // * sizeof(dReal));
#else
        memcpy(x,rhs,left* sizeof(dReal));
#endif

		LCPMath::dSolveLDLT (L,d,x,left,mskip);	
		int fixed = 0;		
#ifdef PS2	
        (*(u_long128*)remove)=0;  (*(((u_long128*)remove)+1))=0;
#else
		for (i = 0; i < left; i++)
			remove[i] = false;
#endif        
		for (i = 0; i < left; i++)
		{
			j = p[i];
			// This isn't the exact same use of findex as dSolveLCP.... since x[findex]
			// may change after I've already clamped x[i], but it should be close
			if (findex[j] > -1)
			{
				dReal f = fabsf(hi[j] * x[p[findex[j]]]);
				if (x[i] > f)
					x[i] = f;
				else if (x[i] < -f)
					x[i] = -f;
				else
					continue;
			}
			else
			{
				if (x[i] > hi[j])
					x[i] = hi[j];
				else if (x[i] < lo[j])
					x[i] = lo[j];
				else
					continue;
			}			
			remove[i] = true;
			fixed++;
		}
		if (fixed == 0 || fixed == left)	//no change or all constraints solved
			break;
		
		for (i = 0; i < left; i++)			//sub in to right hand side.
			if (remove[i])
				for (j = 0; j < left; j++)				
					if (!remove[j])					
						rhs[j] -= (A[j*mskip + i] * x[i]);
		
		// deleting last row/col is easy
		if (remove[left-1])
			left --;
		for (int r = left-1; r>0 /* r >= 0*/; r--)	//eliminate row/col for fixed variables
		{			
			if (remove[r])
			{				
				// dRemoveLDLT adapted for use without row pointers.
				for (i=0; i<r; i++) 
					t[i] = L[r*mskip+i] / d[i];					
				for (i=0; i<left-r; i++)
					a[i] = LCPMath::dDot(L+(r+i)*mskip,t,r) - A[(r+i)*mskip+r];
				a[0] += REAL(1.0);
				LCPMath::dLDLTAddTL (L + r*mskip+r, d+r, a, left-r, mskip);				
				LCPMath::dRemoveRowCol (L,left,mskip,r);
				left--;
				if (r < (left-1))
				{
					dReal tx = x[r];
					memmove (d+r,d+r+1,(left-r)*sizeof(dReal));
					memmove (rhs+r,rhs+r+1,(left-r)*sizeof(dReal));
					//x will get written over by rhs anyway, no need to move it around
					//just store the fixed value we just discovered in it.
					x[left] = tx;
					for (i=0; i < m; i++)
						if (p[i] > r && p[i] <= left)				
							p[i]--;
					p[r] = left;
				}
			}			
		}				
		if (remove[0])
		{
			// Pre: { r=0 }
			for (i=0; i<left; i++) 
				a[i] = -A[i*mskip];
			a[0] += REAL(1.0);
			LCPMath::dLDLTAddTL (L,d,a,left,mskip);
			LCPMath::dRemoveRowCol (L,left,mskip,0);
			left--;
			if (left > 1)
			{
				dReal tx = x[0];
				memmove (d,d+1,left*sizeof(dReal));
				memmove (rhs,rhs+1,left*sizeof(dReal));	
				x[left] = tx;
				for (i=0; i < m; i++)
					if (p[i] > 0 && p[i] <= left)				
						p[i]--;
				p[0] = left;
			}
		}		
	}
	for (i = 0; i < m; i++)
		lambda[i] = x[p[i]];
	for (i =0; i< m ; i++)	
		x[i] = lambda[i];	
}


void dLCP::dSolveLCP (int n, dReal *A, dReal *x, dReal *b,
		dReal *w, int nub, dReal *lo, dReal *hi, int *findex)
{
  dAASSERT (n>0 && A && x && b && w && lo && hi && nub >= 0 && nub <= n);
  int i,k,hit_first_friction_index = 0;
  int nskip = dPAD(n);

  // if all the variables are unbounded then we can 
  // just factor, solve, and return

  if (nub >= n) 
  {
    FactorLDLT (A,w,n,nskip);		// use w for d
    LCPMath::dSolveLDLT (A,w,b,n,nskip);
    memcpy (x,b,n*sizeof(dReal));
    LCPMath::dSetZero (w,n);
    return;
  }

# ifndef dNODEBUG
  // check restrictions on lo and hi
  for (k=0; k<n; k++) dIASSERT (lo[k] <= 0 && hi[k] >= 0);
# endif

  // sv. static allocation
	dAASSERT(n < LCP_MAX_N);
	dReal *L = g_pfdSolveLCP_L;
	dReal *d = g_pfdSolveLCP_d;
	dReal *delta_x = g_pfdSolveLCP_deltax;
	dReal *delta_w = g_pfdSolveLCP_deltaw;
	dReal *Dell = g_pfdSolveLCP_Dell;
	dReal *ell = g_pfdSolveLCP_ell;
	dReal **Arows = g_pfdSolveLCP_Arows;
	int *p = g_pidSolveLCP_p;
	int *C = g_pidSolveLCP_C;

  int dir;
  dReal dirf;

  // for i in N, state[i] is 0 if x(i)==lo(i) or 1 if x(i)==hi(i)  
  int *state = g_pidSolveLCP_state;

  // create LCP object. note that tmp is set to delta_w to save space, this
  // optimization relies on knowledge of how tmp is used, so be careful!

  Init(n,nub,A,x,b,w,lo,hi,L,d,Dell,ell,delta_w,state,findex,p,C,Arows);
  nub = getNub();

  // loop over all indexes nub..n-1. for index i, if x(i),w(i) satisfy the
  // LCP conditions then i is added to the appropriate index set. otherwise
  // x(i),w(i) is driven either +ve or -ve to force it to the valid region.
  // as we drive x(i), x(C) is also adjusted to keep w(C) at zero.
  // while driving x(i) we maintain the LCP conditions on the other variables
  // 0..i-1. we do this by watching out for other x(i),w(i) values going
  // outside the valid region, and then switching them between index sets
  // when that happens.

  for (i=nub; i<n; i++) 
  {
    // the index i is the driving index and indexes i+1..n-1 are "dont care",
    // i.e. when we make changes to the system those x's will be zero and we
    // don't care what happens to those w's. in other words, we only consider
    // an (i+1)*(i+1) sub-problem of A*x=b+w.

    // if we've hit the first friction index, we have to compute the lo and
    // hi values based on the values of x already computed. we have been
    // permuting the indexes, so the values stored in the findex vector are
    // no longer valid. thus we have to temporarily unpermute the x vector.
	// for the purposes of this computation, 0*infinity = 0 ... so if the
    // contact constraint's normal force is 0, there should be no tangential
    // force applied.

    if (hit_first_friction_index == 0 && findex && findex[i] >= 0) 
	{
      // un-permute x into delta_w, which is not being used at the moment
      for (k=0; k<n; k++) 
		  delta_w[p[k]] = x[k];
     
      // set lo and hi values
      for (k=i; k<n; k++) 
	  {
			dReal wfk = delta_w[findex[k]];
			if (wfk == 0) 
			{
				hi[k] = 0;
				lo[k] = 0;
			}
			else 
			{
				hi[k] = fabsf (hi[k] * wfk);
				lo[k] = -hi[k];
			}
      }
      hit_first_friction_index = 1;
    }

    // thus far we have not even been computing the w values for indexes
    // greater than i, so compute w[i] now.
    w[i] = AiC_times_qC (i,x) + AiN_times_qN (i,x) - b[i];

    // if lo=hi=0 (which can happen for tangential friction when normals are
    // 0) then the index will be assigned to set N with some state. however,
    // set C's line has zero size, so the index will always remain in set N.
    // with the "normal" switching logic, if w changed sign then the index
    // would have to switch to set C and then back to set N with an inverted
    // state. this is pointless, and also computationally expensive. to
    // prevent this from happening, we use the rule that indexes with lo=hi=0
    // will never be checked for set changes. this means that the state for
    // these indexes may be incorrect, but that doesn't matter.

    // see if x(i),w(i) is in a valid region
    if (lo[i]==0 && w[i] >= 0) 
	{
      transfer_i_to_N (i);
      state[i] = 0;
    }
    else if (hi[i]==0 && w[i] <= 0) 
	{
      transfer_i_to_N (i);
      state[i] = 1;
    }
    else if (w[i]==0) 
	{
      // this is a degenerate case. by the time we get to this test we know
      // that lo != 0, which means that lo < 0 as lo is not allowed to be +ve,
      // and similarly that hi > 0. this means that the line segment
      // corresponding to set C is at least finite in extent, and we are on it.
      // NOTE: we must call lcp.solve1() before lcp.transfer_i_to_C()
      solve1 (delta_x,i,0,1);
      transfer_i_to_C (i);
    }
    else 
	{
      // we must push x(i) and w(i)
      for (;;) 
	  {
		// find direction to push on x(i)
		if (w[i] <= 0) 
		{
			dir = 1;
			dirf = 1.0f;
		}
		else 
		{
			dir = -1;
			dirf = -1.0f;
		}

	// compute: delta_x(C) = -dir*A(C,C)\A(C,i)
	solve1 (delta_x,i,dir);
	// note that delta_x[i] = dirf, but we wont bother to set it

	// compute: delta_w = A*delta_x ... note we only care about
    // delta_w(N) and delta_w(i), the rest is ignored
	pN_equals_ANC_times_qC (delta_w,delta_x);
	pN_plusequals_ANi (delta_w,i,dir);
        delta_w[i] = AiC_times_qC (i,delta_x) + Aii(i)*dirf;

	// find largest step we can take (size=s), either to drive x(i),w(i)
	// to the valid LCP region or to drive an already-valid variable
	// outside the valid region.

	int cmd = 1;		// index switching command
	int si = 0;		// si = index to switch if cmd>3
	dReal s = -w[i]/delta_w[i];
	if (dir > 0) 
	{
	  if (hi[i] < dInfinity) 
	  {
	    dReal s2 = (hi[i]-x[i])/dirf;		// step to x(i)=hi(i)
	    if (s2 < s) 
		{
	      s = s2;
	      cmd = 3;
	    }
	  }
	}
	else 
	{
	  if (lo[i] > -dInfinity) 
	  {
	    dReal s2 = (lo[i]-x[i])/dirf;		// step to x(i)=lo(i)
	    if (s2 < s) 
		{
	      s = s2;
	      cmd = 2;
	    }
	  }
	}

    int nN=numN();
	for (k=0; k < nN; k++) 
	{
      int iN=indexN(k);
	  if ((state[iN]==0 && delta_w[iN] < 0) ||
	      (state[iN]!=0 && delta_w[iN] > 0)) 
	  {
	    // don't bother checking if lo=hi=0
	    if (lo[iN] == 0 && hi[iN] == 0) 
			continue;

	    dReal s2 = -w[iN] / delta_w[iN];
	    if (s2 < s) 
		{
	      s = s2;
	      cmd = 4;
	      si = iN;
	    }
	  }
	}

	dReal s2;
    int nC = numC();
	for (k=nub; k < nC; k++) 
	{
		int iC=indexC(k);
		if (delta_x[iC] < 0 && lo[iC] > -dInfinity) 
		{
			s2 = (lo[iC]-x[iC]) / delta_x[iC];
			if (s2 < s) 
			{
				s = s2;
				cmd = 5;
				si = iC;
			}
		}
		if (delta_x[iC] > 0 && hi[iC] < dInfinity) 
		{
			s2 = (hi[iC]-x[iC]) / delta_x[iC];
			if (s2 < s) 
			{
				s = s2;
				cmd = 6;
				si = iC;
			}
		}
	}

	//static char* cmdstring[8] = {0,"->C","->NL","->NH","N->C",
	//			     "C->NL","C->NH"};
	//printf ("cmd=%d (%s), si=%d\n",cmd,cmdstring[cmd],(cmd>3) ? si : i);

	// if s <= 0 then we've got a problem. if we just keep going then
	// we're going to get stuck in an infinite loop. instead, just cross
	// our fingers and exit with the current solution.
	if (s <= 0) 
	{
	  //dMessage (d_ERR_LCP, "LCP internal error, s <= 0 (s=%.4e)\n",s);
 	  #ifdef _DEBUG
	 	  printf("LCP internal error, s <= 0 (s=%.4e)\n",s);
	  #endif
	  if (i < (n-1)) 
	  {
	    LCPMath::dSetZero (x+i,n-i);
	    LCPMath::dSetZero (w+i,n-i);
	  }
	  goto done;
	}

	// apply x = x + s * delta_x
	pC_plusequals_s_times_qC (x,s,delta_x);
	x[i] += s * dirf;	

	// apply w = w + s * delta_w
	pN_plusequals_s_times_qN (w,s,delta_w);
	w[i] += s * delta_w[i];

	// switch indexes between sets if necessary
	switch (cmd) {
	case 1:		// done
	  w[i] = 0;
	  transfer_i_to_C (i);
	  break;
	case 2:		// done
	  x[i] = lo[i];
	  state[i] = 0;
	  transfer_i_to_N (i);
	  break;
	case 3:		// done
	  x[i] = hi[i];
	  state[i] = 1;
	  transfer_i_to_N (i);
	  break;
	case 4:		// keep going
	  w[si] = 0;
	  transfer_i_from_N_to_C (si);
	  break;
	case 5:		// keep going
	  x[si] = lo[si];
	  state[si] = 0;
	  transfer_i_from_C_to_N (si);
	  break;
	case 6:		// keep going
	  x[si] = hi[si];
	  state[si] = 1;
	  transfer_i_from_C_to_N (si);
	  break;
	}

	if (cmd <= 3) 
		break;
      }
    }
  }

 done:
  unpermute();
}

//***************************************************************************
// accuracy and timing test

#include "lcptimer.h"

void dTestSolveLCP()
{
  int n = 100;
  int i,nskip = dPAD(n);
  //const dReal tol = REAL(1e-9);
	const dReal tol = REAL(1e-4);
  #ifdef _DEBUG
	  printf ("dTestSolveLCP()\n");
  #endif

  dReal *A = (dReal*) __ALLOCA (n*nskip*sizeof(dReal));
  dReal *x = (dReal*) __ALLOCA (n*sizeof(dReal));
  dReal *b = (dReal*) __ALLOCA (n*sizeof(dReal));
  dReal *w = (dReal*) __ALLOCA (n*sizeof(dReal));
  dReal *lo = (dReal*) __ALLOCA (n*sizeof(dReal));
  dReal *hi = (dReal*) __ALLOCA (n*sizeof(dReal));

  dReal *A2 = (dReal*) __ALLOCA (n*nskip*sizeof(dReal));
  dReal *b2 = (dReal*) __ALLOCA (n*sizeof(dReal));
  dReal *lo2 = (dReal*) __ALLOCA (n*sizeof(dReal));
  dReal *hi2 = (dReal*) __ALLOCA (n*sizeof(dReal));
  dReal *tmp1 = (dReal*) __ALLOCA (n*sizeof(dReal));
  dReal *tmp2 = (dReal*) __ALLOCA (n*sizeof(dReal));

  double total_time = 0;
  for (int count=0; count < 1000; count++) 
  {

    // form (A,b) = a random positive definite LCP problem
    LCPMath::dMakeRandomMatrix (A2,n,n,1.0);
    LCPMath::dMultiply2 (A,A2,A2,n,n,n);
    LCPMath::dMakeRandomMatrix (x,n,1,1.0);
    LCPMath::dMultiply0 (b,A,x,n,n,1);
    for (i=0; i<n; i++) 
		b[i] += (LCPMath::dRandReal()*REAL(0.2))-REAL(0.1);

    // choose `nub' in the range 0..n-1
    int nub = 50; //dRandInt (n);

    // make limits
    for (i=0; i<nub; i++) lo[i] = -dInfinity;
    for (i=0; i<nub; i++) hi[i] = dInfinity;
    //for (i=nub; i<n; i++) lo[i] = 0;
    //for (i=nub; i<n; i++) hi[i] = dInfinity;
    //for (i=nub; i<n; i++) lo[i] = -dInfinity;
    //for (i=nub; i<n; i++) hi[i] = 0;
    for (i=nub; i<n; i++) lo[i] = -(LCPMath::dRandReal()*REAL(1.0))-REAL(0.01);
    for (i=nub; i<n; i++) hi[i] =  (LCPMath::dRandReal()*REAL(1.0))+REAL(0.01);

    // set a few limits to lo=hi=0
    /*
    for (i=0; i<10; i++) {
      int j = dRandInt (n-nub) + nub;
      lo[j] = 0;
      hi[j] = 0;
    }
    */

    // solve the LCP. we must make copy of A,b,lo,hi (A2,b2,lo2,hi2) for
    // SolveLCP() to permute. also, we'll clear the upper triangle of A2 to
    // ensure that it doesn't get referenced (if it does, the answer will be
    // wrong).

    memcpy (A2,A,n*nskip*sizeof(dReal));
    LCPMath::dClearUpperTriangle (A2,n);
    memcpy (b2,b,n*sizeof(dReal));
    memcpy (lo2,lo,n*sizeof(dReal));
    memcpy (hi2,hi,n*sizeof(dReal));
    LCPMath::dSetZero (x,n);
    LCPMath::dSetZero (w,n);

    dStopwatch sw;
    dStopwatchReset (&sw);
    dStopwatchStart (&sw);

	dLCP oLCP;
    oLCP.dSolveLCP (n,A2,x,b2,w,nub,lo2,hi2,0);

    dStopwatchStop (&sw);
    double time = dStopwatchTime(&sw);
    total_time += time;
    double average = total_time / double(count+1) * 1000.0;

    // check the solution

    LCPMath::dMultiply0 (tmp1,A,x,n,n,1);
    for (i=0; i<n; i++) tmp2[i] = b[i] + w[i];
    dReal diff = LCPMath::dMaxDifference (tmp1,tmp2,n,1);
    // printf ("\tA*x = b+w, maximum difference = %.6e - %s (1)\n",diff,
    //	    diff > tol ? "FAILED" : "passed");
    if (diff > tol) dDebug (0,"A*x = b+w, maximum difference = %.6e",diff);
    int n1=0,n2=0,n3=0;
    for (i=0; i<n; i++) {
      if (x[i]==lo[i] && w[i] >= 0) {
	n1++;	// ok
      }
      else if (x[i]==hi[i] && w[i] <= 0) {
	n2++;	// ok
      }
      else if (x[i] >= lo[i] && x[i] <= hi[i] && w[i] == 0) {
	n3++;	// ok
      }
      else {
	dDebug (0,"FAILED: i=%d x=%.4e w=%.4e lo=%.4e hi=%.4e",i,
		x[i],w[i],lo[i],hi[i]);
      }
    }

    // pacifier
    #ifdef _DEBUG
	    printf ("passed: NL=%3d NH=%3d C=%3d   ",n1,n2,n3);
	    printf ("time=%10.3f ms  avg=%10.4f\n",time * 1000.0,average);
    #endif
  }
}

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

given (A,b,lo,hi), solve the LCP problem: A*x = b+w, where each x(i),w(i)
satisfies one of
	(1) x = lo, w >= 0
	(2) x = hi, w <= 0
	(3) lo < x < hi, w = 0
A is a matrix of dimension n*n, everything else is a vector of size n*1.
lo and hi can be +/- dInfinity as needed. the first `nub' variables are
unbounded, i.e. hi and lo are assumed to be +/- dInfinity.

we restrict lo(i) <= 0 and hi(i) >= 0.

the original data (A,b) may be modified by this function.

if the `findex' (friction index) parameter is nonzero, it points to an array
of index values. in this case constraints that have findex[i] >= 0 are
special. all non-special constraints are solved for, then the lo and hi values
for the special constraints are set:
  hi[i] = abs( hi[i] * x[findex[i]] )
  lo[i] = -hi[i]
and the solution continues. this mechanism allows a friction approximation
to be implemented.

*/


#ifndef _ODE_LCP_H_
#define _ODE_LCP_H_

#include "lcpmath.h"



//***************************************************************************
// dLCP manipulator object. this represents an n*n LCP problem.
//
// two index sets C and N are kept. each set holds a subset of
// the variable indexes 0..n-1. an index can only be in one set.
// initially both sets are empty.
//
// the index set C is special: solutions to A(C,C)\A(C,i) can be generated.

//***************************************************************************
// fast implementation of dLCP
//
// `p' records the permutation of A,x,b,w,etc. p is initially 1:n and is
// permuted as the other vectors/matrices are permuted.
//
// A,x,b,w,lo,hi,state,findex,p,c are permuted such that sets C,N have
// contiguous indexes. the don't-care indexes follow N.
//
// an L*D*L' factorization is maintained of A(C,C), and whenever indexes are
// added or removed from the set C the factorization is updated.
// thus L*D*L'=A[C,C], i.e. a permuted top left nC*nC submatrix of A.
// the leading dimension of the matrix L is always `nskip'.
//
// at the start there may be other indexes that are unbounded but are not
// included in `nub'. dLCP will permute the matrix so that absolutely all
// unbounded vectors are at the start. thus there may be some initial
// permutation.
//
// the algorithms here assume certain patterns, particularly with respect to
// index transfer.



struct dLCP 
{

protected:
  int n,nskip,nub;
  dReal** A;				// A rows
  dReal *Adata,*x,*b,*w,*lo,*hi;	// permuted LCP problem data
  dReal *L,*d;				// L*D*L' factorization of set C
  dReal *Dell,*ell,*tmp;
  int *state,*findex,*p,*C;
  int nC,nN;				// size of each index set


  void Init (int _n, int _nub, dReal *_Adata, dReal *_x, dReal *_b, dReal *_w,
	dReal *_lo, dReal *_hi, dReal *_L, dReal *_d,
	dReal *_Dell, dReal *_ell, dReal *_tmp,
	int *_state, int *_findex, int *_p, int *_C, dReal **Arows);
 
  // the constructor is given an initial problem description (A,x,b,w) and
  // space for other working data (which the caller may allocate on the stack).
  // some of this data is specific to the fast dLCP implementation.
  // the matrices A and L have size n*n, vectors have size n*1.
  // A represents a symmetric matrix but only the lower triangle is valid.
  // `nub' is the number of unbounded indexes at the start. all the indexes
  // 0..nub-1 will be put into C.

  int getNub() { return nub; }

  // return the value of `nub'. the constructor may want to change it,
  // so the caller should find out its new value.

  // ------------------------------------------------------------------------
  // transfer functions: transfer index i to the given set (C or N). indexes
  // less than `nub' can never be given. A,x,b,w,etc may be permuted by these
  // functions, the caller must be robust to this.
  
  void transfer_i_to_C (int i);

   // this assumes C and N span 1:i-1. this also assumes that solve1() has
    // been recently called for the same i without any other transfer
    // functions in between (thereby allowing some data reuse for the fast
    // implementation).

  void transfer_i_to_N (int i)
    { nN++; }			// because we can assume C and N span 1:i-1

      // this assumes C and N span 1:i-1.

  void transfer_i_from_N_to_C (int i);
  void transfer_i_from_C_to_N (int i);
  
  inline int numC() { return nC; }
    
	// return the number of indexes in set C

  inline int numN() { return nN; }

    // return the number of indexes in set N

  inline int indexC (int i) { return i; }

    // return index i in set C.

  inline int indexN (int i) { return i+nC; }

    // return index i in set N.

  // -----------------------------------------------------------------
  // accessor and arithmetic functions. Aij translates as A(i,j), etc.
  // make sure that only the lower triangle of A is ever referenced.

  dReal Aii (int i) { return A[i][i]; }
  
  dReal AiC_times_qC (int i, dReal *q) 
  { 
	  return LCPMath::dDot (A[i],q,nC); 
  }
  
  dReal AiN_times_qN (int i, dReal *q) 
  { 
	  // for all Nj
	  return LCPMath::dDot (A[i]+nC,q+nC,nN); 
  }

  void pN_equals_ANC_times_qC (dReal *p, dReal *q);
  
		// for all Nj

  void pN_plusequals_ANi (dReal *p, int i, int sign=1);
      
  // for all Nj. sign = +1,-1. assumes i > maximum index in N.

  void pC_plusequals_s_times_qC (dReal *p, dReal s, dReal *q)
  { 	  
	  for (int i=0; i<nC; i++) 
		  p[i] += s*q[i]; 
  }

  void pN_plusequals_s_times_qN (dReal *p, dReal s, dReal *q)
  { 
	   // for all Nj
	  for (int i=0; i<nN; i++) 
		  p[i+nC] += s*q[i+nC]; 
  }
  void solve1 (dReal *a, int i, int dir=1, int only_transfer=0);
  
	// get a(C) = - dir * A(C,C) \ A(C,i). dir must be +/- 1.
    // the fast version of this function computes some data that is needed by
    // transfer_i_to_C(). if only_transfer is nonzero then this function
    // *only* computes that data, it does not set a(C).

  void unpermute();
	
  // call this at the end of the LCP function. if the x/w values have been
  // permuted then this will unscramble them.


public:
  void dSolveLCP (int n, dReal *A, dReal *x, dReal *b,
		dReal *w, int nub, dReal *lo, dReal *hi, int *findex);

  void dSolveLCP_Fast (int n, dReal *A, dReal *x, dReal *b,
		dReal *w, int nub, dReal *lo, dReal *hi, int *findex);
};

#ifdef JADEFUSION
extern void LCP_StaticInit(void);
#else
extern "C" void LCP_StaticInit(void);
#endif
void LCP_StaticDone();

#endif

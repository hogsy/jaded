#ifndef _LCP_AUX_MATH_H_
#define _LCP_AUX_MATH_H_

#include <math.h> // fabs
#include "lcpmem.h"

#define dReal float

//double dInfinityValue = 1e20f;
const float dInfinityValue = 1e12f;
#ifndef dInfinity
#define dInfinity dInfinityValue
#endif

#define REAL(x) ((float)(x))			/* form a constant */

#ifndef dRecip
#define dRecip(x) ((float)(1.0f/(x)))		/* reciprocal */
#endif

#ifndef dRecip
#define dFabs(x) ((float)fabsf(x))		/* absolute value */
#endif

#ifndef dRecip
#define dAtan2(x,y) ((float) atan2f(x,y)) 
#endif

#define dPAD(a) (((a) > 1) ? ((((a)-1)|3)+1) : (a))

class LCPMath
{
public:

/* get the dot product of two n*1 vectors. if n <= 0 then
 * zero will be returned (in which case a and b need not be valid).
 */

static dReal dDot (const dReal *a, const dReal *b, int n);

/* round an integer up to a multiple of 4, except that 0 and 1 are unmodified
 * (used to compute matrix leading dimensions)
 */


/* set a vector/matrix of size n to all zeros, or to a specific value. */
static void dSetZero (dReal *a, int n);

/* factorize a matrix A into L*D*L', where L is lower triangular with ones on
 * the diagonal, and D is diagonal.
 * A is an n*n matrix stored by rows, with a leading dimension of n rounded
 * up to 4. L is written into the strict lower triangle of A (the ones are not
 * written) and the reciprocal of the diagonal elements of D are written into
 * d.
 */
static void dFactorLDLT3 (dReal *A, dReal *d, int nskip);
static void dFactorLDLT4 (dReal *A, dReal *d, int nskip);
static void dFactorLDLT5 (dReal *A, dReal *d, int nskip);
static void dFactorLDLT6 (dReal *A, dReal *d, int nskip);
static void dFactorLDLT (dReal *A, dReal *d, int n, int nskip);

// in matlab syntax: a(1:n) = a(1:n) .* d(1:n) 
static void dVectorScale (dReal *a, const dReal *d, int n);

/* solve L*x=b, where L is n*n lower triangular with ones on the diagonal,
 * and x,b are n*1. b is overwritten with x.
 * the leading dimension of L is `nskip'.
 */
static void dSolveL13 (const dReal *L, dReal *b, int nskip);
static void dSolveL14 (const dReal *L, dReal *b, int nskip);
static void dSolveL15 (const dReal *L, dReal *b, int nskip);
static void dSolveL16 (const dReal *L, dReal *b, int nskip);
static void dSolveL1 (const dReal *L, dReal *b, int n, int nskip);

/* solve L^T * x=b, with b containing 1 right hand side.
 * L is an n*n lower triangular matrix with ones on the diagonal.
 * L is stored by rows and its leading dimension is lskip.
 * b is an n*1 matrix that contains the right hand side.
 * b is overwritten with x.
 * this processes blocks of 4.
 */

static void dSolveL1_1 (const dReal *L, dReal *B, int n, int lskip1);

static void dSolveL1_2 (const dReal *L, dReal *B, int n, int lskip1);

static void dSolveL1T3 (const dReal *L, dReal *B, int lskip1);
static void dSolveL1T4 (const dReal *L, dReal *B, int lskip1);
static void dSolveL1T5 (const dReal *L, dReal *B, int lskip1);
static void dSolveL1T6 (const dReal *L, dReal *B, int lskip1);
static void dSolveL1T (const dReal *L, dReal *B, int n, int lskip1);

/* given `L', a n*n lower triangular matrix with ones on the diagonal,
 * and `d', a n*1 vector of the reciprocal diagonal elements of an n*n matrix
 * D, solve L*D*L'*x=b where x,b are n*1. x overwrites b.
 * the leading dimension of L is `nskip'.
 */
static void dSolveLDLT (const dReal *L, const dReal *d, dReal *b, int n, int nskip);

/* given an L*D*L' factorization of a permuted matrix A, produce a new
 * factorization for row and column `r' removed.
 *   - A has size n1*n1, its leading dimension in nskip. A is symmetric and
 *     positive definite. only the lower triangle of A is referenced.
 *     A itself may actually be an array of row pointers.
 *   - L has size n2*n2, its leading dimension in nskip. L is lower triangular
 *     with ones on the diagonal. only the lower triangle of L is referenced.
 *   - d has size n2. d contains the reciprocal diagonal elements of D.
 *   - p is a permutation vector. it contains n2 indexes into A. each index
 *     must be in the range 0..n1-1.
 *   - r is the row/column of L to remove.
 * the new L will be written within the old L, i.e. will have the same leading
 * dimension. the last row and column of L, and the last element of d, are
 * undefined on exit.
 *
 * a fast O(n^2) algorithm is used. see ldltremove.m for further comments.
 */
static void dLDLTRemove (dReal **A, const int *p, dReal *L, dReal *d,
		  int n1, int n2, int r, int nskip);

/* given an L*D*L' factorization of an n*n matrix A, return the updated
 * factorization L2*D2*L2' of A plus the following "top left" matrix:
 *
 *    [ b a' ]     <-- b is a[0]
 *    [ a 0  ]     <-- a is a[1..n-1]
 *
 *   - L has size n*n, its leading dimension is nskip. L is lower triangular
 *     with ones on the diagonal. only the lower triangle of L is referenced.
 *   - d has size n. d contains the reciprocal diagonal elements of D.
 *   - a has size n.
 * the result is written into L, except that the left column of L and d[0]
 * are not actually modified. see ldltaddTL.m for further comments. 
 */
static void dLDLTAddTL (dReal *L, dReal *d, const dReal *a, int n, int nskip);

/* given an n*n matrix A (with leading dimension nskip), remove the r'th row
 * and column by moving elements. the new matrix will have the same leading
 * dimension. the last row and column of A are untouched on exit.
 */
static void dRemoveRowCol (dReal *A, int n, int nskip, int r);

// make a random matrix with entries between +/- range. A has size n*m. 
static void dMakeRandomMatrix (dReal *A, int n, int m, dReal range);

// clear the upper triangle of a square matrix 
static void dClearUpperTriangle (dReal *A, int n);

/* matrix multiplication. all matrices are stored in standard row format.
 * the digit refers to the argument that is transposed:
 *   0:   A = B  * C   (sizes: A:p*r B:p*q C:q*r)
 *   1:   A = B' * C   (sizes: A:p*r B:q*p C:q*r)
 *   2:   A = B  * C'  (sizes: A:p*r B:p*q C:r*q)
 * case 1,2 are equivalent to saying that the operation is A=B*C but
 * B or C are stored in standard column format.
 */
static void dMultiply0 (dReal *A, const dReal *B, const dReal *C, int p,int q,int r);
static void dMultiply1 (dReal *A, const dReal *B, const dReal *C, int p,int q,int r);
static void dMultiply2 (dReal *A, const dReal *B, const dReal *C, int p,int q,int r);

// return a random real number between 0..1 
static dReal dRandReal();
static ULONG dRandGetSeed();
static void dRandSetSeed (ULONG s);
static ULONG dRand();
static int dRandInt (int n);

// return the maximum element difference between the two n*m matrices 
static dReal dMaxDifference (const dReal *A, const dReal *B, int n, int m);
};

#endif
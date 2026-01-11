#ifndef LCP_VU0_H
#define LCP_VU0_H 1

void dSolveL1 (const float *L, float *B, int n, int lskip1);
void dSolveL1T (const float *L, float *B, int n, int lskip1);
void TransferdSolveL1ProgramToVu0 ();

#endif
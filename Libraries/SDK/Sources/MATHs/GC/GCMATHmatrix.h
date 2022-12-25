
#ifndef __GCMATHMATRIX_H__
#define __GCMATHMATRIX_H__
#pragma once

#include "MATHs/GC/GCMATHfloat.h"
#include "MATHs/GC/GCMATHgector.h"
#include "BASe/BAStypes.h"
#include <string.h>

#ifdef _RVL
#include <revolution/mtx/GeoTypes.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif


inline void MATH_CopyMatrixToGCMatrix(Mtx *mDst, MATH_tdst_Matrix *mSrc)
{
	memcpy(mDst,mSrc,12*sizeof(float));
}


#ifdef __cplusplus
}
#endif


#endif //__GCMATHMATRIX_H__
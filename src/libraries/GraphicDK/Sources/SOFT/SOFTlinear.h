/*$T SOFTlinear.h GC!1.71 01/26/00 16:49:08 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __SOFTLINEAR_H__
#define __SOFTLINEAR_H__
#include "BASe/BAStypes.h"

#include "MATHs/MATH.h"
#include "CAMera/CAMera.h"
#include "SOFT/SOFTstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void                SOFT_Project(MATH_tdst_Vector *, MATH_tdst_Vector *, LONG, CAM_tdst_Camera *);
void                SOFT_TransformAndProject(MATH_tdst_Vector *, MATH_tdst_Vector *, LONG, CAM_tdst_Camera *);

void                SOFT_TransformAndProjectIn2Buffers(MATH_tdst_Vector *, MATH_tdst_Vector *, GEO_Vertex *, LONG, CAM_tdst_Camera *);

void                SOFT_TransformAndProjectInComputingBuffer
                    (
                        SOFT_tdst_ComputingBuffers *,
                        GEO_Vertex *,
                        LONG,
                        CAM_tdst_Camera *
                    );
void                SOFT_TransformInComputingBuffer
                    (
                        SOFT_tdst_ComputingBuffers *,
                        GEO_Vertex *,
                        LONG,
                        CAM_tdst_Camera *
                    );
void                SOFT_Compute_Z
                    (
                        float *DstZ,
                        GEO_Vertex *Src,
                        LONG Number,
                        MATH_tdst_Matrix *Matrix
                    );
void                SOFT_ProjectInComputingBuffer
                    (
                        SOFT_tdst_ComputingBuffers *,
                        MATH_tdst_Vector *,
                        LONG,
                        CAM_tdst_Camera *
                    );

void                SOFT_ComputeSpecularVectors(struct GEO_tdst_Object_ *);
MATH_tdst_Vector    *SOFT_pst_GetSpecularVectors(void);
void SOFT_pst_InitSpecularColors(ULONG NumPoints);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SOFTLINEAR_H */

/*$T CAMstruct.h GC!1.39 06/30/99 08:52:37 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __CAMSTRUCT_H__
#define __CAMSTRUCT_H__
#include "BASe/BAStypes.h"

#include "GRObject/GROstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Structures
 ***************************************************************************************************
 */

typedef struct  CAM_tdst_CameraObject_
{
    GRO_tdst_Struct st_Id;
    ULONG   ul_Flags;
    float           f_NearPlane;
    float           f_FarPlane;
    float           f_FieldOfVision;
} CAM_tdst_CameraObject;

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

void                    CAM_Init(void);
CAM_tdst_CameraObject   *CAM_pst_Create(void);
void                    CAM_Free(CAM_tdst_CameraObject *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif /* __CAMSTRUCT_H__ */
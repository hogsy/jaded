/*$T CAMera.h GC!1.52 10/25/99 10:37:51 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __CAM_CAMERA__
#define __CAM_CAMERA__
#include "BASe/BAStypes.h"


#include "MATHs/MATH.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Constant
 ***************************************************************************************************
 */

#define CAM_Cul_Flags_Perspective   0x00000001
#define CAM_Cul_Flags_Isometric     0x00000002
#define CAM_Cul_Flags_Targeted      0x00000004
#define CAM_Cul_Flags_TargetPoint   0x00000008
#define CAM_Cul_Flags_RotateXLocked 0x00000010
#define CAM_Cul_Flags_RotateYLocked 0x00000020
#define CAM_Cul_Flags_RotateZLocked 0x00000040
#define CAM_Cul_Flags_CropToWindow  0x00000080

#define CAM_Cul_Flags_Iso2          0x00000100
#define CAM_Cul_Flags_Iso2Old       0x00000200
#define CAM_Cul_Flags_Ortho			0x00000400
#define CAM_Cul_Flags_OrthoYInvert	0x00000800

/*$4
 ***************************************************************************************************
    Structure
 ***************************************************************************************************
 */

typedef struct  CAM_tdst_Camera_
{
    ULONG               ul_Flags;

    float               f_NearPlane;
    float               f_FarPlane;

    float               f_FieldOfVision;
    float               f_YoverX;
    float               f_FactorX;
    float               f_FactorY;

    float               f_CenterX;
    float               f_CenterY;

    float               f_Width;
    float               f_Height;

    float               f_ViewportLeft;
    float               f_ViewportTop;
    float               f_ViewportWidth;
    float               f_ViewportHeight;

    LONG                l_ViewportRealLeft;
    LONG                l_ViewportRealTop;

    float               f_IsoFactor;
    float               f_IsoZoom;

    MATH_tdst_Vector    st_NormPlaneUp;
    MATH_tdst_Vector    st_NormPlaneDown;
    MATH_tdst_Vector    st_NormPlaneRight;
    MATH_tdst_Vector    st_NormPlaneLeft;
    MATH_tdst_Vector    st_NormPlaneNear;

    MATH_tdst_Matrix    st_Matrix				ONLY_PSX2_ALIGNED(16);
    MATH_tdst_Matrix    st_InverseMatrix		ONLY_PSX2_ALIGNED(16);
    MATH_tdst_Matrix    st_ProjectionMatrix		ONLY_PSX2_ALIGNED(16);

    MATH_tdst_Matrix    *pst_ObjectToCameraMatrix;

    MATH_tdst_Vector    st_Target;
} CAM_tdst_Camera;

/*$4
 ***************************************************************************************************
    Globals
 ***************************************************************************************************
 */

extern CAM_tdst_Camera  CAM_gst_IsoNoClip;

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */
struct GDI_tdst_ScreenFormat_;
void                    CAM_Engine_ComputePlans(CAM_tdst_Camera *);
void                    CAM_Engine_Init(CAM_tdst_Camera *);
void                    CAM_AssignCameraToDevice(struct GDI_tdst_ScreenFormat_ *, CAM_tdst_Camera *, LONG, LONG);

void                    CAM_Move(CAM_tdst_Camera *, MATH_tdst_Vector *);
void                    CAM_Rotate(CAM_tdst_Camera *, MATH_tdst_Vector *);
void                    CAM_RotateAroundTarget(CAM_tdst_Camera *, MATH_tdst_Vector *);
void                    CAM_Inverse(CAM_tdst_Camera *);

void                    CAM_SetObjectMatrixFromCam(MATH_tdst_Matrix *, MATH_tdst_Matrix *);
#ifdef ACTIVE_EDITORS
void                    CAM_SetCamMatrixFromObject(MATH_tdst_Matrix *, MATH_tdst_Matrix *);
#endif
void                    CAM_SetCameraMatrix(CAM_tdst_Camera *, MATH_tdst_Matrix *);
void                    CAM_SetCameraPos(CAM_tdst_Camera *, MATH_tdst_Vector *);

void                    CAM_2Dto3D(CAM_tdst_Camera *, MATH_tdst_Vector *, MATH_tdst_Vector *);
void                    CAM_2Dto3DCamera(CAM_tdst_Camera *, MATH_tdst_Vector *, MATH_tdst_Vector *);
void                    CAM_2Dto3DCamera2(CAM_tdst_Camera *, MATH_tdst_Vector *, const MATH_tdst_Vector *);

void					CAM_UpdateIsometricFactor(CAM_tdst_Camera *, MATH_tdst_Vector *, char);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __CAM_CAMERA__ */

/*$T OBJculling.h GC!1.41 09/09/99 11:29:54 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __OBJ_CULLING__
#define __OBJ_CULLING__


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* __cplusplus */

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "GraphicDK/Sources/CAMera/CAMera.h"

void    OBJ_UpdateCullingVars(CAM_tdst_Camera *);
BOOL    OBJ_CullingObject(OBJ_tdst_GameObject *, CAM_tdst_Camera *);
char	OBJ_c_GetCullingMask(OBJ_tdst_GameObject *);

#ifdef JADEFUSION
BOOL OBJ_CullingOBBoxView(MATH_tdst_Vector* _pst_LMin, 
                          MATH_tdst_Vector* _pst_LMax, 
                          MATH_tdst_Matrix* _pst_LocalToCam, 
                          CAM_tdst_Camera*  _pst_Cam);
#endif

#define OBJ_Culling_Z_OverLap	1
#define OBJ_Culling_XY_OverLap	2


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* __cplusplus */

#endif /* __OBJ_CULLING__ */

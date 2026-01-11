/*$T GEODebugObject.h GC!1.55 01/19/00 10:55:21 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GEODEBUGOBJECT_H__
#define __GEODEBUGOBJECT_H__



#include "BASe/BAStypes.h"
#include "GDInterface/GDInterface.h"
#include "SOFT/SOFTMatrixStack.h"
#include "GEOmetric/GEOobject.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Debug object constant
 ***********************************************************************************************************************
 */

#define GEO_DebugObject_Sphere          0
#define GEO_DebugObject_Box             1
#define GEO_DebugObject_LightOmni       2
#define GEO_DebugObject_LightSpot       3
#define GEO_DebugObject_Camera          4
#define GEO_DebugObject_Arrow           5
#define GEO_DebugObject_Torus           6
#define GEO_DebugObject_Waypoint        7
#define GEO_DebugObject_Question        8
#define GEO_DebugObject_SystemAxis      9
#define GEO_DebugObject_LinkArrow       10
#define GEO_DebugObject_Cylinder        11
#define GEO_DebugObject_Cone            12
#define GEO_DebugObject_Gizmo			13
#ifdef JADEFUSION
#define GEO_DebugObject_Portal			14//popowarning
#define GEO_DebugObject_Invisible       15
#define GEO_DebugObject_Number          79
#else
#define GEO_DebugObject_Invisible       14
#define GEO_DebugObject_Number          78
#endif
#define GEO_DebugObject_LastInvisible   77


/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */
#ifdef ACTIVE_EDITORS
extern short GEO_gw_DebugObject_Texture;
#endif


/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
void            GEO_DebugObject_Create(void);
void            GEO_DebugObject_Destroy(void);
void            GEO_DebugObject_AddTexture(void);
void            GEO_DebugObject_UsedTexture(char *);
GEO_tdst_Object *GEO_pst_DebugObject_Get(LONG);
void            GEO_DebugObject_PushSphereMatrix(SOFT_tdst_MatrixStack *, MATH_tdst_Vector *, float);
void            GEO_DebugObject_PushBoxMatrix(SOFT_tdst_MatrixStack *, MATH_tdst_Vector *, MATH_tdst_Vector *);
void            GEO_DebugObject_PushCylinderMatrix(SOFT_tdst_MatrixStack *, MATH_tdst_Vector *, float, float);
void            GEO_DebugObject_PushConeMatrix(SOFT_tdst_MatrixStack *,MATH_tdst_Vector *, float,  float);
void            GEO_DebugObject_Draw(GDI_tdst_DisplayData *, LONG, ULONG, ULONG, MATH_tdst_Matrix *);
void            GEO_DebugObject_PopMatrix(SOFT_tdst_MatrixStack *);
void            GEO_DebugObject_SetLight(ULONG);
void            GEO_DebugObject_SetLightMatrix(GDI_tdst_DisplayData *);
ULONG           GEO_ul_DebugObject_GetColor(LONG, LONG);

void            GEO_Dump( struct GEO_tdst_Object_ *);


/*$4
 ***********************************************************************************************************************
    Macros for non editors versions
 ***********************************************************************************************************************
 */

#else /* Without editors */

#define GEO_DebugObject_Create()
#define GEO_DebugObject_Destroy()
#define GEO_DebugObject_AddTexture(a)
#define GEO_DebugObject_UsedTexture(a)
#define GEO_pst_DebugObject_Get(a)  NULL
#define GEO_DebugObject_PushSphereMatrix(a, b, c)
#define GEO_DebugObject_PushBoxMatrix(a, b, c)
#define GEO_DebugObject_PushCylinderMatrix(a,b,c,d)
#define GEO_DebugObject_PushConeMatrix(a,b,c,d)
#define GEO_DebugObject_Draw(a, b, c, d, e)
#define GEO_DebugObject_PopMatrix(a)
#define GEO_ul_DebugObject_GetColor(a, b)   0
#define GEO_DebugObject_SetLight(a)
#define GEO_DebugObject_SetLightMatrix(a)
#define GEO_ul_DebugObject_GetColor(a, b) 0
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif /* __GEODEBUGOBJECT_H__ */
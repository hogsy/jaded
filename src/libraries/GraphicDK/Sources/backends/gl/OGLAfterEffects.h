/*$T OGLrenderstate.h GC!1.52 11/23/99 12:22:14 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __OGLAFTEREFFECTS_H__
#define __OGLAFTEREFFECTS_H__

#ifndef PSX2_TARGET
#pragma once
#endif



#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Structure
 ***************************************************************************************************
 */

/*
 ---------------------------------------------------------------------------------------------------
    Specific data for OpenGL Device
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  OGL_tdst_RenderState_
{
    LONG    l_LastTexture;
    LONG    l_DepthFunc;
    float   f_LineWidth;
    float   f_PointSize;
    char    c_Wired;
    char    c_CullFace;
    char    c_DepthMask;
    char    c_CullFaceInverted;
    char    c_Fogged;

} OGL_tdst_RenderState;

/*$4
 ***************************************************************************************************
    Function
 ***************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __OGLRENDERSTATE_H */

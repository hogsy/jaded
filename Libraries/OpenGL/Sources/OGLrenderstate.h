/*$T OGLrenderstate.h GC!1.52 11/23/99 12:22:14 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __OGLRENDERSTATE_H__
#define __OGLRENDERSTATE_H__

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

void    OGL_RS_Init(OGL_tdst_RenderState *);
void    OGL_RS_DrawWired(OGL_tdst_RenderState *, LONG);
void    OGL_RS_CullFace(OGL_tdst_RenderState *, LONG);
void    OGL_RS_CullFaceInverted(OGL_tdst_RenderState *, LONG);
void    OGL_RS_Fogged(OGL_tdst_RenderState *, LONG);
void    OGL_RS_DepthFunc( OGL_tdst_RenderState *, LONG);
void    OGL_RS_DepthMask(OGL_tdst_RenderState *, LONG);
void    OGL_RS_DepthTest(OGL_tdst_RenderState *, LONG);

LONG    OGL_RS_UseTexture(struct OGL_tdst_SpecificData_ *, LONG);

void    OGL_RS_LineWidth(OGL_tdst_RenderState *, float);
void    OGL_RS_PointSize(OGL_tdst_RenderState *, float);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __OGLRENDERSTATE_H */

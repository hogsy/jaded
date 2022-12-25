/*$T DX8renderstate.h GC!1.52 11/23/99 12:22:14 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __DX8RENDERSTATE_H__
#define __DX8RENDERSTATE_H__

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
typedef struct  DX8_tdst_RenderState_
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

} DX8_tdst_RenderState;

/*$4
 ***************************************************************************************************
    Function
 ***************************************************************************************************
 */

void    DX8_RS_Init(DX8_tdst_RenderState *);

void    DX8_RS_DrawWired(struct DX8_tdst_SpecificData_ *, LONG);
void    DX8_RS_CullFace(struct DX8_tdst_SpecificData_ *, LONG);
void    DX8_RS_CullFaceInverted(struct DX8_tdst_SpecificData_ *, LONG);
void    DX8_RS_Fogged(struct DX8_tdst_SpecificData_ *, LONG);
void    DX8_RS_DepthFunc( struct DX8_tdst_SpecificData_ *, LONG);
void    DX8_RS_DepthTest(struct DX8_tdst_SpecificData_ *, LONG);
void    DX8_RS_DepthMask(struct DX8_tdst_SpecificData_ *, LONG);
LONG    DX8_RS_UseTexture(struct DX8_tdst_SpecificData_ *, LONG);

void    DX8_RS_LineWidth(DX8_tdst_RenderState *, float);
void    DX8_RS_PointSize(DX8_tdst_RenderState *, float);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __DX8RENDERSTATE_H */

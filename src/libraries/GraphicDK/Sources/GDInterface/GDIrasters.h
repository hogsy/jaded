/*$T GDIrasters.h GC!1.55 01/20/00 17:11:45 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GDIRASTERS_H__
#define __GDIRASTERS_H__

#include "TIMer/PROfiler/PROdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

#ifdef RASTERS_ON
typedef struct  GDI_tdst_Rasters_
{
    PRO_tdst_TrameRaster    st_ViewportWidth;
    PRO_tdst_TrameRaster    st_ViewportHeight;
    PRO_tdst_TrameRaster    st_RenderingCount;
    PRO_tdst_TrameRaster    st_NbObjects;
    PRO_tdst_TrameRaster    st_NbLights;
    PRO_tdst_TrameRaster    st_Lights_NbComputedVertexColor;
    PRO_tdst_TrameRaster    st_Lights_OmniNbVertex;
    PRO_tdst_TrameRaster    st_Lights_SpotNbVertex;
    PRO_tdst_TrameRaster    st_Lights_DirectNbVertex;
    PRO_tdst_TrameRaster    st_NbTriangles;
    PRO_tdst_TrameRaster    st_NbGenSprites;
    PRO_tdst_TrameRaster    st_NbSortedObjects;
    PRO_tdst_TrameRaster    st_NbSortedTriangles;
    PRO_tdst_TrameRaster    st_NbGFX;
    PRO_tdst_TrameRaster    st_NbGFXrendered;

    PRO_tdst_TrameRaster    st_BeforeDisplay;
    PRO_tdst_TrameRaster    st_AfterDisplay;
    PRO_tdst_TrameRaster    st_BuildLightList;
    PRO_tdst_TrameRaster    st_Light_ComputeVertexColor;
    PRO_tdst_TrameRaster    st_Light_All;
    PRO_tdst_TrameRaster    st_Light_Omni;
    PRO_tdst_TrameRaster    st_Light_Spot;
    PRO_tdst_TrameRaster    st_Light_Direct;
    PRO_tdst_TrameRaster    st_GroRender_LookAt;
    PRO_tdst_TrameRaster    st_GroRender_PushMatrix;
    PRO_tdst_TrameRaster    st_GroRender_SetViewMatrix;
    PRO_tdst_TrameRaster    st_GroRender_Render;
    PRO_tdst_TrameRaster    st_GeoRender_DrawIndexedTriangles;
    PRO_tdst_TrameRaster    st_MatDIT_PrepareMaterial;
    PRO_tdst_TrameRaster    st_MatDIT_SingleMaterial;
    PRO_tdst_TrameRaster    st_MatDIT_SetRenderState;
    PRO_tdst_TrameRaster    st_MatDIT_DrawTriangles;
    PRO_tdst_TrameRaster    st_MatDIT_MultiTexture;
    PRO_tdst_TrameRaster    st_MatDIT_MultiTexture_ComputeUV;
    PRO_tdst_TrameRaster    st_GL_Begin;
    PRO_tdst_TrameRaster    st_GL_End;
    PRO_tdst_TrameRaster    st_GL_Flip;
    PRO_tdst_TrameRaster    st_GL_Clear;
    PRO_tdst_TrameRaster    st_Temp1;
    PRO_tdst_TrameRaster    st_Temp2;
    PRO_tdst_TrameRaster    st_Temp3;
    PRO_tdst_TrameRaster    st_Temp4;
    PRO_tdst_TrameRaster    st_ComputeMRM;
    PRO_tdst_TrameRaster    st_SpriteGeneration;
    PRO_tdst_TrameRaster    st_RTShadowGeneration;
    PRO_tdst_TrameRaster    st_GFX;
    PRO_tdst_TrameRaster    st_ZList;
} GDI_tdst_Rasters;
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

#ifdef RASTERS_ON
void    GDI_Rasters_Init(GDI_tdst_Rasters *, char *);
void    GDI_Rasters_Close(GDI_tdst_Rasters *);
void    GDI_Rasters_Reset(GDI_tdst_Rasters *);
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */
/*$F*/


#define BENCH_TEXTURE_CREATION

/**/
#ifdef BENCH_TEXTURE_CREATION
extern float    GDI_f_Delay_AttachWorld_TextureUsed;
extern float    GDI_f_Delay_AttachWorld_TextureInfos;
extern float    GDI_f_Delay_AttachWorld_TextureManager;
extern float    GDI_f_Delay_AttachWorld_TextureCreate;
extern float    GDI_f_Delay_AttachWorld_TextureCreate_GetContent;
extern float    GDI_f_Delay_AttachWorld_TextureCreate_Compress;
extern float    GDI_f_Delay_AttachWorld_TextureCreate_Convert24To32;
extern float    GDI_f_Delay_AttachWorld_TextureCreate_Convert32Colors;
extern float    GDI_f_Delay_AttachWorld_TextureCreate_LoadHard;
extern float    GDI_f_Delay_AttachWorld_TextureCreate_Mipmap;
#define GDI_M_TimerReset(a) a=0;
#define GDI_M_TimerStart(a) a-=TIM_f_Clock_TrueRead();
#define GDI_M_TimerStop(a) a+=TIM_f_Clock_TrueRead();
#define GDI_M_TimerPrint(a,b) \
{\
    sprintf( sz_Text, "%s : %f", a,b);\
    LINK_PrintStatusMsg(sz_Text);\
}
#else

#define GDI_M_TimerReset(a)
#define GDI_M_TimerStart(a)
#define GDI_M_TimerStop(a)
#define GDI_M_TimerPrint(a,b)

#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GDIRASTERS_H */


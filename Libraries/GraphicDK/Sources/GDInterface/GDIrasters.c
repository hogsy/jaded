/*$T GDIrasters.c GC!1.71 01/26/00 15:17:49 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "GDInterface/GDIrasters.h"

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#ifdef BENCH_TEXTURE_CREATION
float   GDI_f_Delay_AttachWorld_TextureUsed;
float   GDI_f_Delay_AttachWorld_TextureInfos;
float   GDI_f_Delay_AttachWorld_TextureManager;
float   GDI_f_Delay_AttachWorld_TextureCreate;
float   GDI_f_Delay_AttachWorld_TextureCreate_GetContent;
float   GDI_f_Delay_AttachWorld_TextureCreate_Compress;
float   GDI_f_Delay_AttachWorld_TextureCreate_Convert24To32;
float   GDI_f_Delay_AttachWorld_TextureCreate_Convert32Colors;
float   GDI_f_Delay_AttachWorld_TextureCreate_LoadHard;
float   GDI_f_Delay_AttachWorld_TextureCreate_Mipmap;
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

#ifdef RASTERS_ON

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GDI_Rasters_Init(GDI_tdst_Rasters *R, char *N)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *C[2] = { "Display Bench", "Display Rasters" };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    PRO_FirstInitTrameRaster(&R->st_NbTriangles, C[0], N, "Number of triangles", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_NbGenSprites, C[0], N, "Number of generated sprites", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_NbLights, C[0], N, "Number of lights", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_Lights_NbComputedVertexColor, C[0], N, "Light : Nb computed color vertices", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_Lights_OmniNbVertex, C[0], N, "Light : Nb vertex lighted by omni", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_Lights_SpotNbVertex, C[0], N, "Light : Nb vertex lighted by spot", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_Lights_DirectNbVertex, C[0], N, "Light : Nb vertex lighted by direct", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_NbObjects, C[0], N, "Number of objects", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_RenderingCount, C[0], N, "Rendering counter", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_ViewportHeight, C[0], N, "Viewport height", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_ViewportWidth, C[0], N, "Viewport width", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_NbSortedObjects, C[0], N, "Number of sorted objects", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_NbSortedTriangles, C[0], N, "Number of sorted triangles", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_NbGFX, C[0], N, "Total Nb of GFX", PRO_E_Long, 0);
    PRO_FirstInitTrameRaster(&R->st_NbGFXrendered, C[0], N, "Nb of GFX displayed", PRO_E_Long, 0);

    PRO_FirstInitTrameRaster(&R->st_Temp4, C[1], N, "Temp4", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_Temp3, C[1], N, "Temp3", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_Temp2, C[1], N, "Temp2", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_Temp1, C[1], N, "Temp1", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_GL_End, C[1], N, "GL : End", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_GL_Begin, C[1], N, "GL : Begin", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_GL_Flip, C[1], N, "GL : Flip", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_GL_Clear, C[1], N, "GL : Clear", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&R->st_MatDIT_MultiTexture_ComputeUV, C[1], N, "MatDIT : multi texture UV's", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_MatDIT_MultiTexture, C[1], N, "MatDIT : multi texture", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_MatDIT_DrawTriangles, C[1], N, "MatDIT : draw triangles", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_MatDIT_SetRenderState, C[1], N, "MatDIT : set render state", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_MatDIT_SingleMaterial, C[1], N, "MatDIT : single material", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_MatDIT_PrepareMaterial, C[1], N, "MatDIT : Prepare mat", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&R->st_ComputeMRM, C[1], N, "GeoRender : Compute MRM", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&R->st_SpriteGeneration, C[1], N, "GeoRender : Sprite generation", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&R->st_RTShadowGeneration, C[1], N, "GeoRender : Shadow generation", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_GeoRender_DrawIndexedTriangles, C[1], N, "GeoRender : Draw IT", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_GroRender_Render, C[1], N, "GroRender : render", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_GroRender_SetViewMatrix, C[1], N, "GroRender : Set view matrix", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_GroRender_PushMatrix, C[1], N, "GroRender : Push matrix", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_GroRender_LookAt, C[1], N, "GroRender : Look At", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_BuildLightList, C[1], N, "Build light list", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_Light_ComputeVertexColor, C[1], N, "Compute vertex color", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_Light_All, C[1], N, "Compute all light", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_Light_Omni, C[1], N, "Compute omni light", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_Light_Spot, C[1], N, "Compute spot light", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_Light_Direct, C[1], N, "Compute direct light", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_AfterDisplay, C[1], N, "After display", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_BeforeDisplay, C[1], N, "Before display", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_GFX, C[1], N, "GFX", PRO_E_Time, 0);
    PRO_FirstInitTrameRaster(&R->st_ZList, C[1], N, "ZList", PRO_E_Time, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GDI_Rasters_Close(GDI_tdst_Rasters *_pst_Rasters)
{
    PRO_KillRaster(&_pst_Rasters->st_ViewportWidth);
    PRO_KillRaster(&_pst_Rasters->st_ViewportHeight);
    PRO_KillRaster(&_pst_Rasters->st_RenderingCount);
    PRO_KillRaster(&_pst_Rasters->st_NbObjects);
    PRO_KillRaster(&_pst_Rasters->st_NbLights);
    PRO_KillRaster(&_pst_Rasters->st_Lights_NbComputedVertexColor);
    PRO_KillRaster(&_pst_Rasters->st_Lights_OmniNbVertex);
    PRO_KillRaster(&_pst_Rasters->st_Lights_SpotNbVertex);
    PRO_KillRaster(&_pst_Rasters->st_Lights_DirectNbVertex);
    PRO_KillRaster(&_pst_Rasters->st_NbTriangles);
    PRO_KillRaster(&_pst_Rasters->st_NbGenSprites);
    PRO_KillRaster(&_pst_Rasters->st_NbSortedObjects);
    PRO_KillRaster(&_pst_Rasters->st_NbSortedTriangles);
    PRO_KillRaster(&_pst_Rasters->st_NbGFX);
    PRO_KillRaster(&_pst_Rasters->st_NbGFXrendered);

    PRO_KillRaster(&_pst_Rasters->st_BeforeDisplay);
    PRO_KillRaster(&_pst_Rasters->st_AfterDisplay);
    PRO_KillRaster(&_pst_Rasters->st_BuildLightList);
    PRO_KillRaster(&_pst_Rasters->st_Light_ComputeVertexColor);
    PRO_KillRaster(&_pst_Rasters->st_Light_All);
    PRO_KillRaster(&_pst_Rasters->st_Light_Omni);
    PRO_KillRaster(&_pst_Rasters->st_Light_Spot);
    PRO_KillRaster(&_pst_Rasters->st_Light_Direct);
    PRO_KillRaster(&_pst_Rasters->st_GroRender_LookAt);
    PRO_KillRaster(&_pst_Rasters->st_GroRender_PushMatrix);
    PRO_KillRaster(&_pst_Rasters->st_GroRender_SetViewMatrix);
    PRO_KillRaster(&_pst_Rasters->st_GroRender_Render);
    PRO_KillRaster(&_pst_Rasters->st_GeoRender_DrawIndexedTriangles);
    PRO_KillRaster(&_pst_Rasters->st_MatDIT_PrepareMaterial);
    PRO_KillRaster(&_pst_Rasters->st_MatDIT_SingleMaterial);
    PRO_KillRaster(&_pst_Rasters->st_MatDIT_SetRenderState);
    PRO_KillRaster(&_pst_Rasters->st_MatDIT_DrawTriangles);
    PRO_KillRaster(&_pst_Rasters->st_MatDIT_MultiTexture);
	PRO_KillRaster(&_pst_Rasters->st_MatDIT_MultiTexture_ComputeUV);
    PRO_KillRaster(&_pst_Rasters->st_GL_Begin);
    PRO_KillRaster(&_pst_Rasters->st_GL_End);
    PRO_KillRaster(&_pst_Rasters->st_GL_Flip);
    PRO_KillRaster(&_pst_Rasters->st_GL_Clear);
    PRO_KillRaster(&_pst_Rasters->st_Temp1);
    PRO_KillRaster(&_pst_Rasters->st_Temp2);
    PRO_KillRaster(&_pst_Rasters->st_Temp3);
    PRO_KillRaster(&_pst_Rasters->st_Temp4);
	PRO_KillRaster(&_pst_Rasters->st_ComputeMRM);
	PRO_KillRaster(&_pst_Rasters->st_SpriteGeneration);
	PRO_KillRaster(&_pst_Rasters->st_RTShadowGeneration);
    PRO_KillRaster(&_pst_Rasters->st_GFX);
    PRO_KillRaster(&_pst_Rasters->st_ZList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GDI_Rasters_Reset(GDI_tdst_Rasters *_pst_Rasters)
{
    PRO_SetRasterLong(&_pst_Rasters->st_NbObjects, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_NbLights, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_Lights_NbComputedVertexColor, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_Lights_OmniNbVertex, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_Lights_SpotNbVertex, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_Lights_DirectNbVertex, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_NbTriangles, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_NbGenSprites, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_NbSortedObjects, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_NbSortedTriangles, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_NbGFX, 0);
    PRO_SetRasterLong(&_pst_Rasters->st_NbGFXrendered, 0);
} 

#endif /* RASTERS_ON */
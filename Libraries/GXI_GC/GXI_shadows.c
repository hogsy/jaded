
#include "GDInterface/GDInterface.h"
#include "GXI_shadows.h"
#include "GXI_bench.h"

#ifndef USE_FULL_SCENE_SHADOWS

GXTexObj g_ShadowTextures[MAX_SHADOW];
static u8	g_ShadowTextureData[SHADOW_RESOLUTION*SHADOW_RESOLUTION*MAX_SHADOW] ATTRIBUTE_ALIGN(32);

u32 	 g_CurrentBufferState = 0;
BOOL 	 g_bRenderingShadows;
u32 	 g_currentShadowNum2Set=-1;
BOOL	 g_bIMustGetBack = FALSE;
BOOL	 g_bIMustCLear = FALSE;
u32 	 g_AlreadySet = 0;
#endif


void GXI_InitShadowTextures()
{
	u32 i;
	u8* pos = 0;
	
	L_memset(g_ShadowTextureData, 255, sizeof(g_ShadowTextureData));
	pos = g_ShadowTextureData;
	
	for(i=0; i<MAX_SHADOW; i++)
	{
		GXInitTexObj(&g_ShadowTextures[i], (void*)pos, SHADOW_RESOLUTION, SHADOW_RESOLUTION, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE); 
    	GXInitTexObjLOD(&g_ShadowTextures[i],GX_LINEAR,GX_LINEAR,0,0,0,0,0,GX_ANISO_1);
    	pos += (SHADOW_RESOLUTION*SHADOW_RESOLUTION);
    	GXInitTexObjUserData(&g_ShadowTextures[i], (void*)0);
	} 
}

void GXI_PutIntoShadowTexture2(u32 ShadowNum)
{
	//GXColor    clear_clr;

	GX_GXSetColorUpdate(GX_TRUE);
	GX_GXSetAlphaUpdate(GX_TRUE);

	GX_GXSetZMode(GX_ENABLE, GX_ALWAYS, GX_ENABLE);
	GXSetTexCopySrc(512.0f, (float)(FrameBufferHeight - SHADOW_RESOLUTION * 2), SHADOW_RESOLUTION * 2, SHADOW_RESOLUTION * 2);
	GXSetTexCopyDst(SHADOW_RESOLUTION, SHADOW_RESOLUTION, GX_CTF_R8, GX_TRUE);
	GXCopyTex(GXGetTexObjData(&g_ShadowTextures[ShadowNum]), GX_FALSE);
	GXPixModeSync();
	g_AlreadySet |= 1 << ShadowNum;
}

void GXI_BeginShadowRendering(u32 _iShadowNum, BOOL _bClear)
{
	if (_iShadowNum >= MAX_SHADOW) _iShadowNum = MAX_SHADOW - 1;
	
	if (g_currentShadowNum2Set == _iShadowNum)
	{
		g_bIMustGetBack = FALSE;
		g_bIMustCLear = FALSE;
	} else
	{
		if (g_currentShadowNum2Set != -1) GXI_PutIntoShadowTexture2(g_currentShadowNum2Set);
		if  (g_AlreadySet & (1 << _iShadowNum))
		{
			g_bIMustGetBack = TRUE;
			g_bIMustCLear = FALSE;
		} else
		{
			g_bIMustGetBack = FALSE;
			g_bIMustCLear = TRUE;
		} 
	}	
	
	g_currentShadowNum2Set = _iShadowNum;
	g_bRenderingShadows = TRUE;
}
// copy the last shadow into the shadow texture 
// (there is no way to know when the shadow rendering is over so we do it just before the 2d rendering)
void GXI_CompleteShadowRenderingB2D()
{
	if (g_currentShadowNum2Set != -1) GXI_PutIntoShadowTexture2(g_currentShadowNum2Set);
	g_currentShadowNum2Set = -1;
	g_AlreadySet = 0;
}
void GXI_EndShadowRendering()
{
	g_bIMustGetBack = FALSE;
	g_bIMustCLear = FALSE;
	g_bRenderingShadows = FALSE;
}



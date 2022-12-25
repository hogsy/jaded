#ifndef __GXI_SHADOWS_H__
#define __GXI_SHADOWS_H__

#undef USE_FULL_SCENE_SHADOWS

#ifndef USE_FULL_SCENE_SHADOWS

#define MAX_SHADOW 16
#define MAX_SHADOW_ELEM 64
#define SHADOW_DL_SIZE_TOT 256*1024
#define SHADOW_DL_SIZE 48*1024
#define SHADOW_RESOLUTION 64

#endif // USE_FULL_SCENE_SHADOWS

/*$4
 ***************************************************************************************************
    Function
 ***************************************************************************************************
 */
void GXI_CompleteShadowRendering();
void GXI_InitShadowTextures();
void GXI_BeginShadowRendering(u32 _iShadowNum, BOOL _bClear);
void GXI_EndShadowRendering();
void GXI_PutIntoShadowTexture();

#ifdef USE_SELF_SHADOWS
void GXI_SS_SetupTevAndTexture(GXTexObj *pShadowTex);
GXTexObj* GXI_SS_pGetObjectHaveASelfShadowTexture(u32 obj);
#endif


#endif /* __GXI_INIT_H__ */
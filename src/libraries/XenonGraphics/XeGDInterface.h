#pragma once

#include "MATHs/MATH.h"
#include "GEOmetric/GEOobject.h"
#include "BASe/BENch/BENch.h"

//#ifdef __cplusplus
//extern "C"
//{
//#endif

void * Xe_pst_CreateDevice(void);
void Xe_DestroyDevice(void *);
LONG Xe_l_Init(HWND _hWnd, struct GDI_tdst_DisplayData_ *);
LONG Xe_l_Close(void *);
LONG Xe_l_ReadaptDisplay(HWND _hWnd, struct GDI_tdst_DisplayData_ *);
void Xe_Clear(LONG, ULONG);
void Xe_RequestFlip(void);
#if defined(_XENON_RENDERER_USETHREAD)
void Xe_ThreadCallback_DynamicVB_InitialLock(int _iEventID = 0, void* _pParameter = NULL);
void Xe_ThreadCallback_DynamicVB_FinalUnlock(int _iEventID = 0, void* _pParameter = NULL);
void Xe_ThreadCallback_Flip(int _iEventID = 0, void* _pParameter = NULL);
#endif 
void Xe_Flip(void);
void Xe_DebugInfo(void);
void Xe_SetViewMatrix(MATH_tdst_Matrix *);
void Xe_SetProjectionMatrix(struct CAM_tdst_Camera_ *);
LONG Xe_l_DrawElementIndexedTriangles(GEO_tdst_ElementIndexedTriangles  *_pst_Element,
                                      GEO_Vertex                        *_pst_Point,
                                      GEO_tdst_UV                       *_pst_UV,
                                      ULONG                             ulnumberOfPoints);
LONG Xe_l_DrawElementIndexedSprites(GEO_tdst_ElementIndexedSprite   *_pst_Element,
                                    GEO_Vertex                      *_pst_Point,
                                    ULONG                           ulnumberOfPoints);
LONG Xe_l_Request(ULONG _ulRequest, ULONG _ulData);
void Xe_SetTextureBlending( ULONG _l_Texture, ULONG BM, unsigned short s_AditionalFlags );
LONG Xe_l_Texture_Init(struct GDI_tdst_DisplayData_ *, ULONG);
void Xe_Texture_Resize(ULONG);
void Xe_Texture_Load(struct GDI_tdst_DisplayData_ *, struct TEX_tdst_Data_ *, struct TEX_tdst_File_Desc_ *, ULONG);
LONG Xe_l_Texture_Store(struct GDI_tdst_DisplayData_ *, struct TEX_tdst_Data_ *, struct TEX_tdst_File_Desc_ *, ULONG);
void Xe_Palette_Load(struct GDI_tdst_DisplayData_ *, struct TEX_tdst_Palette_ *, ULONG);
void Xe_Set_Texture_Palette(struct GDI_tdst_DisplayData_ *, ULONG _ulTexNum, ULONG IT, ULONG IP);
void Xe_Texture_Unload(struct GDI_tdst_DisplayData_ *);
void Xe_SetTextureTarget(ULONG NumShadowTex , ULONG CLEAR);
void Xe_SetViewMatrix_SDW(struct MATH_tdst_Matrix_ *_pst_Matrix , float *Limits);

void Xe_InvalidateRenderLists(void);
ULONG Xe_AddLight(LIGHT_tdst_Light* _poLight, BOOL bAddToLightToSet = TRUE );
void Xe_AddShadowLight(OBJ_tdst_GameObject * _poLightGAO);

void Xe_ResetAllShaders(void);
void Xe_BeforeDisplay(void);
void Xe_AfterDisplay(void);

void Xe_SetAnimatedTexture(LONG _l_TextureId, LONG _l_CurrentAnimTextureId);

//#ifdef __cplusplus
//}
//#endif
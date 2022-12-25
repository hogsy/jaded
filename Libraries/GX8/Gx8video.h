#if !defined( GX8_VIDEO_H )
#  define GX8_VIDEO_H
#pragma once


#include <xtl.h>
#include <d3d8.h>
#include "Gx8Init.h"

bool Gx8_InitVideo(Gx8_tdst_SpecificData *_pst_SD, CHAR* strFilename);
void Gx8_CloseVideo(void);
void Gx8_PlayVideoInTexture(Gx8_tdst_SpecificData *pst_SD, LPDIRECT3DTEXTURE8 destTexture);
void Gx8_SetNoVideo(void);
void Gx8_CloseVideoFullScreen(void);
void Gx8_PlayVideoFullScreen(ULONG ulBK);
void Gx8_PlayVideoFullscreen(Gx8_tdst_SpecificData *pst_SD);
bool Gx8_InitVideoFullScreen(Gx8_tdst_SpecificData *_pst_SD, CHAR* strOriginalFilename);


#endif

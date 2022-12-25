#ifndef __GX8PIXELSHADERS_H__
#define __GX8PIXELSHADERS_H__

#include <xtl.h>

// Creation/release of all vertex shaders
void Gx8_vCreatePixelShaders(void);
void Gx8_vReleasePixelShaders(void);
/*
// In-game use of vertex shaders
void Gx8_vUseSpriteVertexShader(float fGlobalSize, float fGlobalRatio);
void Gx8_vUseSPG2VertexShader();

//New faster version of vertex shader driven particles
void Gx8_vUseSpriteVertexShaderNew(float fGlobalSize, float fGlobalRatio,DWORD vsHandle);

// same structure as in the constant shaders structure...
typedef struct Gx8_tdstLightForShaders_
{
    D3DVECTOR4      stParallelLightDirection; // must be normalized (w ignored)
    D3DCOLORVALUE   stParallelLightColor;
    D3DVECTOR4      stSphericalLightPosition; // (w ignored)
    D3DVECTOR4      stSphericalLightAttenuation; // in the form (A0, A1, A2, ignored)
    D3DCOLORVALUE   stSphericalLightColor;
} Gx8_tdstLightForShaders;

// specify which lights the LitAlpha VS will use :
void Gx8_vSetLightsForShaderConstants(Gx8_tdstLightForShaders *_p_stLightForShaders);
void Gx8_vSetTransformMatrixForShaders(void);
void Gx8_vSetUVTransformationForShaders(D3DMATRIX *pstMatrix);
void Gx8_vUseLitAlphaVertexShader(void);
// Normal Map
void  Gx8_vUseNormalMapVertexShader(D3DXMATRIX* objmatrix);
*/
void Gx8_vUseNormalMapPixelShader();

#endif __VERTEXSHADERS_H__

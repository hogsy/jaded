

#include "Gx8PixelShaders.h"
#include <d3dx8.h>
#include "Gx8init.h"
#include "Gx8renderstate.h"
#include <GDInterface\GDInterface.h>

#ifdef _XBOX
// Include files generated from .vsh :
#include "PShaders\specular.h"
#endif

// WARNING !
// The handle for an invalid shader is 0xFFFFFFFF, not NULL or 0x0 !!!

/*
static struct tdstShaderConstants_
{
    D3DXMATRIX  matWVP;
    union
    {
        struct
        {
            FLOAT       fGlobalSize;
            FLOAT       fGlobalRatio;
        } stSpriteConstants;
        struct
        {
            Gx8_tdstLightForShaders stLights;
            struct
            {
                D3DVECTOR4 stRow0;
                D3DVECTOR4 stRow1;
            } a4_stUVTransformationMatrix[4];
        } stLitAlphaConstants;
    };
} gs_stShaderConstants;
// Shader constants are set in a global variable because otherwise we seem to have stack overflow or something like that...

// Various sizes, depending on what part of the constant array is used
const DWORD C_dwShaderConstantSizeForSprites = 4 + 1;
#define M_dwShaderConstantSizeForLitAlpha(UVLayersCount) (4 + 5 + (UVLayersCount) * 2)
*/
typedef enum
{
	Gx8_eNormalMapSpecPShader,

    Gx8_ePShadersCount  // keep this one at the end ot the enum
} Gx8_tdePixelShadersID;

static DWORD Gx8_gs_a_dwPShaderHandles[Gx8_ePShadersCount] = { 0xFFFFFFFF };

void Gx8_vCreatePixelShaders(void)
{
	
	LPDIRECT3DDEVICE8 mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;

	// Normalmap Specular
	IDirect3DDevice8_CreatePixelShader(mp_D3DDevice,
                                            (D3DPIXELSHADERDEF*)dwSpecularPixelShader,
                                            &Gx8_gs_a_dwPShaderHandles[Gx8_eNormalMapSpecPShader]
											);
}




void Gx8_vReleasePixelShaders(void)
{
    LPDIRECT3DDEVICE8 mp_D3DDevice;
	int i;
	if (!GDI_gpst_CurDD)
		return;
	mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;

    for (i = 0; i < Gx8_ePShadersCount; ++i)
	{
        if (Gx8_gs_a_dwPShaderHandles[i] != 0xFFFFFFFF)
        {
            IDirect3DDevice8_DeletePixelShader(mp_D3DDevice, Gx8_gs_a_dwPShaderHandles[i]);
            Gx8_gs_a_dwPShaderHandles[i] = 0xFFFFFFFF;
        }
    }
}
/*
_inline_ void Gx8_vSetCommonConstants(LPDIRECT3DDEVICE8 mp_D3DDevice)
{
    MATH_tdst_Matrix stOGLMatrix;

    // Setup the world-view-projection matrix.
    MATH_MakeOGLMatrix(&stOGLMatrix, GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
    D3DXMatrixMultiply(&gs_stShaderConstants.matWVP, (struct _D3DMATRIX *)&stOGLMatrix, (const struct _D3DMATRIX *) GX8_M_SD(GDI_gpst_CurDD)->pst_ProjMatrix);

    // Pass the transform matrix into the shader.
    D3DXMatrixTranspose(&gs_stShaderConstants.matWVP, &gs_stShaderConstants.matWVP);
}
*/
void Gx8_vUseNormalMapPixelShader()
{
	D3DXVECTOR4 colorA= {0.4f,0.4f,0.4f,  1.f}; //RGB A
	D3DXVECTOR4 colorD= {1.f,1.0f,1.0f,  1.f}; //RGB A
	D3DXVECTOR4 colorS= {0.0f,0.0f,0.0f,  0.2f}; //RGB A
	D3DXVECTOR4 colorS2= {1.0f,0.8f,0.5f,  1.0f}; //RGB A

	LPDIRECT3DDEVICE8 mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;

//	IDirect3DDevice8_SetPixelShaderConstant(mp_D3DDevice,0, &colorA, 1);
//	IDirect3DDevice8_SetPixelShaderConstant(mp_D3DDevice,1, &colorD, 1);
//	IDirect3DDevice8_SetPixelShaderConstant(mp_D3DDevice,2, &colorS, 1);
//	IDirect3DDevice8_SetPixelShaderConstant(mp_D3DDevice,4, &colorS2, 1);
	Gx8_vSetPixelShader(Gx8_gs_a_dwPShaderHandles[Gx8_eNormalMapSpecPShader]);
//	IDirect3DDevice8_SetPixelShader(mp_D3DDevice, Gx8_gs_a_dwPShaderHandles[Gx8_eNormalMapSpecPShader]);
}
/*

void Gx8_vUseFurVertexShader()
{
   LPDIRECT3DDEVICE8 mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;

    MATH_tdst_Matrix stOGLMatrix;
	D3DXMATRIX transfo;
    // Setup the world-view-projection matrix.
    MATH_MakeOGLMatrix(&stOGLMatrix, GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
    D3DXMatrixMultiply(&transfo, (struct _D3DMATRIX *)&stOGLMatrix, (const struct _D3DMATRIX *) GX8_M_SD(GDI_gpst_CurDD)->pst_ProjMatrix);

    // Pass the transform matrix into the shader.
    D3DXMatrixTranspose(&transfo, &transfo);
   
   
   /*
	gs_stShaderConstants.stSpriteConstants.fGlobalSize = 1.333333f*fGlobalSize;//correction par rapport au ratio de l'écran et à l'inversion w/h
    gs_stShaderConstants.stSpriteConstants.fGlobalRatio = fGlobalRatio;
*/
/*desac
	IDirect3DDevice8_SetVertexShaderConstant(mp_D3DDevice, 0, &transfo, 4);

    Gx8_vSetVertexShader(Gx8_gs_a_dwVShaderHandles[Gx8_eFurVShader]);
}

void Gx8_vUseSpriteVertexShader(float fGlobalSize, float fGlobalRatio)
{
    LPDIRECT3DDEVICE8 mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;
    Gx8_vSetCommonConstants(mp_D3DDevice);
    gs_stShaderConstants.stSpriteConstants.fGlobalSize = 1.333333f*fGlobalSize;//correction par rapport au ratio de l'écran et à l'inversion w/h
    gs_stShaderConstants.stSpriteConstants.fGlobalRatio = fGlobalRatio;

    IDirect3DDevice8_SetVertexShaderConstant(mp_D3DDevice, 0, &gs_stShaderConstants, C_dwShaderConstantSizeForSprites);

    Gx8_vSetVertexShader(Gx8_gs_a_dwVShaderHandles[Gx8_eSpriteVShader]);
}

void Gx8_vSetLightsForShaderConstants(Gx8_tdstLightForShaders *_p_stLightForShaders)
{
    gs_stShaderConstants.stLitAlphaConstants.stLights = *_p_stLightForShaders;
}

void Gx8_vSetTransformMatrixForShaders(void)
{
    Gx8_vSetCommonConstants(GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice);
}

void Gx8_vSetUVTransformationForShaders(D3DMATRIX *pstMatrix)
{
    gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow0.x = pstMatrix->_11;
    gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow0.y = pstMatrix->_21;
    gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow0.z = pstMatrix->_31;
    gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow0.w = pstMatrix->_41;
    gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow1.x = pstMatrix->_12;
    gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow1.y = pstMatrix->_22;
    gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow1.z = pstMatrix->_32;
    gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow1.w = pstMatrix->_42;
}

void Gx8_vUseLitAlphaVertexShader(void)
{
    Gx8_tdeUVGenerationType eUVType;
    LPDIRECT3DDEVICE8 mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;
    Gx8_tdeVertexShadersID eShaderID;

    // Transform matrix was set by a call to Gx8_vSetTransformMatrixForShaders

    // Lights
    // light constants where set by Gx8_vApplyLightSetForLitAlphaShaders, called by LIGHT_SendObjectToLight_HW
    // it is an object-level setting, so no need to change them for each element/layer

    // UV coordinates
    eUVType = Gx8_fneGetCurrentUVState();
    if (eUVType != Gx8_eUVOff)
    {
        if (eUVType == Gx8_eUVRotation)
        {
            eShaderID = Gx8_eLitAlpha_SP_O;
        }
        else if (eUVType == Gx8_eUVNormal)
        {
            eShaderID = Gx8_eLitAlpha_SP_P;
        }
        else //Gx8_eUVChrome
        {
            eShaderID = Gx8_eLitAlpha_SP_N;
        }

        // gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0] was set by calling Gx8_vSetUVTransformationForShaders
    }
    else
    {
        // Set the matrix to identity
        gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow0.x = 1.0f;
        gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow0.y = 0.0f;
        gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow0.z = 0.0f;
        gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow0.w = 0.0f; //?
        gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow1.x = 0.0f;
        gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow1.y = 1.0f;
        gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow1.z = 0.0f;
        gs_stShaderConstants.stLitAlphaConstants.a4_stUVTransformationMatrix[0].stRow1.w = 0.0f; //?
        eShaderID = Gx8_eLitAlpha_SP_O;
    }

    IDirect3DDevice8_SetVertexShaderConstant(mp_D3DDevice, 0, &gs_stShaderConstants, M_dwShaderConstantSizeForLitAlpha(1));

    Gx8_vSetVertexShader(Gx8_gs_a_dwVShaderHandles[eShaderID]);
}



void Gx8_vUseSpriteVertexShaderNew(float fGlobalSize, float fGlobalRatio,DWORD vsHandle)
{

	LPDIRECT3DDEVICE8 mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;
    Gx8_vSetCommonConstants(mp_D3DDevice);
    gs_stShaderConstants.stSpriteConstants.fGlobalSize = 1.333333f*fGlobalSize;//correction par rapport au ratio de l'écran et à l'inversion w/h
    gs_stShaderConstants.stSpriteConstants.fGlobalRatio = fGlobalRatio;
    IDirect3DDevice8_SetVertexShaderConstant(mp_D3DDevice, 0, &gs_stShaderConstants, C_dwShaderConstantSizeForSprites);
    Gx8_vSetVertexShader(vsHandle);


}
*/


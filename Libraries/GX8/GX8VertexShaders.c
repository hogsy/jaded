

#include "Gx8VertexShaders.h"
#include <d3dx8.h>
#include "Gx8init.h"
#include "Gx8renderstate.h"
#include <GDInterface\GDInterface.h>

#ifdef _XBOX
// Include files generated from .vsh :
#include "VShaders\sprite_xbox.h"
#include "VShaders\fur.h"
#include "VShaders\NormalMap.h"
#include "VShaders\NormalMapSpec.h"
#include "VShaders\lit_alpha_PS_O.h"
#include "VShaders\lit_alpha_PS_P.h"
#include "VShaders\lit_alpha_PS_N.h"
#include "VShaders\lit_alpha_PS_OO.h"
#include "VShaders\lit_alpha_PS_OP.h"
#include "VShaders\lit_alpha_PS_PO.h"
#include "VShaders\lit_alpha_PS_PP.h"
#endif

#define NORMALMAP

// WARNING !
// The handle for an invalid shader is 0xFFFFFFFF, not NULL or 0x0 !!!

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

typedef enum
{
    Gx8_eSpriteVShader,
    Gx8_eFurVShader,//Yoann Fur
	Gx8_eNormalMapVShader,
	Gx8_eNormalMapSpecVShader,

	// The following shaders (for lit alpha) must stay in that order
    Gx8_eLitAlpha_SP_O,
    Gx8_eLitAlpha_SP_P,
    Gx8_eLitAlpha_SP_N,
    Gx8_eLitAlpha_SP_OO,
    Gx8_eLitAlpha_SP_OP,
    Gx8_eLitAlpha_SP_PO,
    Gx8_eLitAlpha_SP_PP,

    Gx8_eVShadersCount  // keep this one at the end ot the enum
} Gx8_tdeVertexShadersID;

static DWORD Gx8_gs_a_dwVShaderHandles[Gx8_eVShadersCount] = { 0xFFFFFFFF };

void Gx8_vCreateVertexShaders(void)
{
    LPDIRECT3DDEVICE8 mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;

    // Sprite vertex shader
    {
        // Sprite shader vertex format :
        // struct {
        //    D3DVECTOR stCenterPostion;
        //    FLOAT     f2DOffsetX;
        //    FLOAT     f2DOffsetY;
        //    FLOAT     fU;
        //    FLOAT     fV;
        // }
        
		DWORD dwSpriteVertexDecl[] =
        {
            D3DVSD_STREAM(0),
            D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3 ),  // v0 : 3D position of the sprite center
            D3DVSD_REG( D3DVSDE_NORMAL,  D3DVSDT_FLOAT2 ),    // v2 : 2 offsets to get 2D corner from 2D center position
            D3DVSD_REG( D3DVSDE_DIFFUSE,  D3DVSDT_D3DCOLOR ), // v3 : vertex color
            D3DVSD_REG( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2 ),  // v9 : corner UVs
            D3DVSD_END()
        };

        IDirect3DDevice8_CreateVertexShader(mp_D3DDevice,
                                            dwSpriteVertexDecl,
                                            dwSprite_xboxVertexShader,
                                            &Gx8_gs_a_dwVShaderHandles[Gx8_eSpriteVShader],
                                            0);
    }
	
	// Yoann Fur vertex shader
	{
		DWORD dwFurVertexDecl[] =
        {
            D3DVSD_STREAM(0),
            D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),
			D3DVSD_STREAM(1),
            D3DVSD_REG( 1, D3DVSDT_FLOAT3 ),
            D3DVSD_STREAM(2),
            D3DVSD_REG( 2, D3DVSDT_D3DCOLOR ),
            D3DVSD_STREAM(3),
            D3DVSD_REG( 3, D3DVSDT_FLOAT2 ),    
            D3DVSD_END()
        };
		
		IDirect3DDevice8_CreateVertexShader(mp_D3DDevice,
                                            dwFurVertexDecl,
                                            dwFurVertexShader,
                                            
											&Gx8_gs_a_dwVShaderHandles[Gx8_eFurVShader],
                                            0);

	}
#ifdef NORMALMAP
	// Normal Map vertex shader
	{
		DWORD dwNormalMapVertexDecl[] =
        {
            D3DVSD_STREAM(0),
            D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),
			D3DVSD_STREAM(1),
            D3DVSD_REG( 1, D3DVSDT_FLOAT3 ),
            D3DVSD_STREAM(2),
            D3DVSD_REG( 2, D3DVSDT_D3DCOLOR ),
            D3DVSD_STREAM(3),
            D3DVSD_REG( 3, D3DVSDT_FLOAT2 ),    
            D3DVSD_STREAM(5),
            D3DVSD_REG( 5, D3DVSDT_FLOAT4 ),    //codage W pour inversion binormale... :-)
            D3DVSD_STREAM(6),
 //           D3DVSD_REG( 6, D3DVSDT_FLOAT3 ),    
 //           D3DVSD_END()
        };

// 1er sans specular
		IDirect3DDevice8_CreateVertexShader(mp_D3DDevice,
                                            dwNormalMapVertexDecl,
                                            dwNormalMapVertexShader,
                                            
											&Gx8_gs_a_dwVShaderHandles[Gx8_eNormalMapVShader],
                                            0);
// 2eme avec Specular
		IDirect3DDevice8_CreateVertexShader(mp_D3DDevice,
                                            dwNormalMapVertexDecl,
                                            dwNormalMapSpecVertexShader,
                                            
											&Gx8_gs_a_dwVShaderHandles[Gx8_eNormalMapSpecVShader],
                                            0);

	}
#endif

   // Lit alpha vertex shaders
    {




	/*OLD..CARLONE 

        DWORD dwLitAlphaVertexDecl[] =
        {
            D3DVSD_STREAM(0),
            D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3 ),  // v0 : vertex position
            D3DVSD_REG( D3DVSDE_NORMAL,  D3DVSDT_FLOAT3 ),    // v2 : vertex normal
            D3DVSD_REG( D3DVSDE_DIFFUSE,  D3DVSDT_D3DCOLOR ), // v3 : vertex color
            D3DVSD_REG( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2 ),  // v9 : vertex UVs
            D3DVSD_END()
        };
	*/


		DWORD dwLitAlphaVertexDecl[] =
        {
            D3DVSD_STREAM(0),
            D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3 ),  // v0 : vertex position
			D3DVSD_STREAM(1),
            D3DVSD_REG( D3DVSDE_NORMAL,  D3DVSDT_FLOAT3 ),    // v2 : vertex normal
			D3DVSD_STREAM(2),
			D3DVSD_REG( D3DVSDE_DIFFUSE,  D3DVSDT_D3DCOLOR ), // v3 : vertex color
            D3DVSD_STREAM(3),
			D3DVSD_REG( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2 ),  // v9 : vertex UVs
            D3DVSD_END()
        };





#define GX8_M_CreateVertexShader_1UV(lights,uv_s0) \
        IDirect3DDevice8_CreateVertexShader(mp_D3DDevice, \
                                            dwLitAlphaVertexDecl, \
                                            dwLit_alpha_PS_##uv_s0##VertexShader, \
                                            &Gx8_gs_a_dwVShaderHandles[Gx8_eLitAlpha_SP_##uv_s0], \
                                            0);
        GX8_M_CreateVertexShader_1UV(PS,O);
        GX8_M_CreateVertexShader_1UV(PS,P);
        GX8_M_CreateVertexShader_1UV(PS,N);
#undef GX8_M_CreateVertexShader_1UV
#define GX8_M_CreateVertexShader_2UV(lights,uv_s0, uv_s1) \
        IDirect3DDevice8_CreateVertexShader(mp_D3DDevice, \
                                            dwLitAlphaVertexDecl, \
                                            dwLit_alpha_PS_##uv_s0##uv_s1##VertexShader, \
                                            &Gx8_gs_a_dwVShaderHandles[Gx8_eLitAlpha_SP_##uv_s0##uv_s1], \
                                            0);
        GX8_M_CreateVertexShader_2UV(PS,O,O);
        GX8_M_CreateVertexShader_2UV(PS,O,P);
        GX8_M_CreateVertexShader_2UV(PS,P,O);
        GX8_M_CreateVertexShader_2UV(PS,P,P);
#undef GX8_M_CreateVertexShader_2UV
    }
}

void Gx8_vReleaseVertexShaders(void)
{
    LPDIRECT3DDEVICE8 mp_D3DDevice;
	int i;
	if (!GDI_gpst_CurDD)
		return;
	mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;

    for (i = 0; i < Gx8_eVShadersCount; ++i)
	{
        if (Gx8_gs_a_dwVShaderHandles[i] != 0xFFFFFFFF)
        {
            IDirect3DDevice8_DeleteVertexShader(mp_D3DDevice, Gx8_gs_a_dwVShaderHandles[i]);
            Gx8_gs_a_dwVShaderHandles[i] = 0xFFFFFFFF;
        }
    }

    // ensure that attenuation is non-zero else we have divisions by zero in the shaders, and that causes bugs (even if spherical light color is black)
    gs_stShaderConstants.stLitAlphaConstants.stLights.stSphericalLightAttenuation.x = 1.0f;
    gs_stShaderConstants.stLitAlphaConstants.stLights.stSphericalLightAttenuation.x = 0.0f;
    gs_stShaderConstants.stLitAlphaConstants.stLights.stSphericalLightAttenuation.x = 0.0f;
    gs_stShaderConstants.stLitAlphaConstants.stLights.stSphericalLightAttenuation.x = 0.0f;
}

_inline_ void Gx8_vSetCommonConstants(LPDIRECT3DDEVICE8 mp_D3DDevice)
{
    MATH_tdst_Matrix stOGLMatrix;

    // Setup the world-view-projection matrix.
    MATH_MakeOGLMatrix(&stOGLMatrix, GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
    D3DXMatrixMultiply(&gs_stShaderConstants.matWVP, (struct _D3DMATRIX *)&stOGLMatrix, (const struct _D3DMATRIX *) GX8_M_SD(GDI_gpst_CurDD)->pst_ProjMatrix);

    // Pass the transform matrix into the shader.
    D3DXMatrixTranspose(&gs_stShaderConstants.matWVP, &gs_stShaderConstants.matWVP);
}

void Gx8_vUseNormalMapVertexShader(D3DXMATRIX* objmatrix,D3DXVECTOR3 pos)
{
	static float popo=0;
	D3DXVECTOR4 animationConstants = {0.5f, 0, 0, 0.0f};
	D3DXVECTOR3 lightPos;
	D3DXVECTOR3 lightDir;// = {1.0f, -1.0f, 1.0f, 0.0f};
	D3DXVECTOR3 vEyePt;
	D3DXVECTOR3 zero = {0,0,0};

	LPDIRECT3DDEVICE8 mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;

    MATH_tdst_Matrix stOGLMatrix;
	D3DXMATRIX transfo,matProj,matWorld,pipo;//,matView;
    
	
	IDirect3DDevice8_GetTransform( mp_D3DDevice,D3DTS_WORLD,      &matWorld );
    //IDirect3DDevice8_GetTransform( mp_D3DDevice,D3DTS_VIEW,       &matView );
    IDirect3DDevice8_GetTransform( mp_D3DDevice,D3DTS_PROJECTION, &matProj );

	//D3DXMatrixMultiply(&pipo,&matWorld,&matView);
	D3DXMatrixMultiply(&pipo,&matProj,&pipo);
	D3DXMatrixMultiply(&pipo,&matWorld,&matProj);
	//popo = matWorld*matView*matProj;
	pipo=matWorld;
	D3DXMatrixTranspose(&pipo, &pipo);
	IDirect3DDevice8_SetVertexShaderConstant(mp_D3DDevice, 8, &pipo, 4);

	//D3DXMatrixMultiply(&transfo, &matView,(const struct _D3DMATRIX *) GX8_M_SD(GDI_gpst_CurDD)->pst_ProjMatrix);
	//D3DXMatrixMultiply(&transfo, objmatrix,&transfo);

	//transfo = objmatrix*matView*matProj;
	// Setup the world-view-projection matrix.
    MATH_MakeOGLMatrix(&stOGLMatrix, GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
	D3DXMatrixMultiply(&transfo, (struct _D3DMATRIX *)&stOGLMatrix, (const struct _D3DMATRIX *) GX8_M_SD(GDI_gpst_CurDD)->pst_ProjMatrix);

	//D3DXMatrixMultiply(&transfo, &matWorld, &matProj);
 	//D3DXMatrixMultiply(&transfo, &matView, &transfo);
    

    // Pass the transform matrix into the shader.
    D3DXMatrixTranspose(&transfo, &transfo);
   
   
   /*
	gs_stShaderConstants.stSpriteConstants.fGlobalSize = 1.333333f*fGlobalSize;//correction par rapport au ratio de l'écran et à l'inversion w/h
    gs_stShaderConstants.stSpriteConstants.fGlobalRatio = fGlobalRatio;
*/

	IDirect3DDevice8_SetVertexShaderConstant(mp_D3DDevice, 0, &transfo, 4);
	
	vEyePt.x = GDI_gpst_CurDD->st_Camera.st_Matrix.T.x;
	vEyePt.y = GDI_gpst_CurDD->st_Camera.st_Matrix.T.y;
	vEyePt.z = GDI_gpst_CurDD->st_Camera.st_Matrix.T.z;

	popo+=1;
	/*lightPos.x = 0.0f+fSin(popo)*10;
	lightPos.y = 10.0f;
	lightPos.z = 0.0f;*/
	lightPos = pos;

	lightDir.x = -1;//0.3f;//0.5f;//+fSin(popo/1000)*1;
	lightDir.y = 0;//0.3f;//0.4f;
	lightDir.z = 0;//-0.8f;

	animationConstants.x =0.5f;
	animationConstants.y =0;
	animationConstants.z =0;
	animationConstants.w =0;

	//D3DXVec3Subtract(&lightDir,&zero,&lightPos);//lightDir = zero-lightPos;
	D3DXVec3Normalize(&lightDir, &lightDir);
	
	IDirect3DDevice8_SetVertexShaderConstant(mp_D3DDevice,4, &lightPos, 1);
	IDirect3DDevice8_SetVertexShaderConstant(mp_D3DDevice,5, &lightDir, 1);
	IDirect3DDevice8_SetVertexShaderConstant(mp_D3DDevice,6, &vEyePt, 1); 
	IDirect3DDevice8_SetVertexShaderConstant(mp_D3DDevice,7, &animationConstants, 1);


    //Gx8_vSetVertexShader(Gx8_gs_a_dwVShaderHandles[Gx8_eNormalMapVShader]);
    Gx8_vSetVertexShader(Gx8_gs_a_dwVShaderHandles[Gx8_eNormalMapSpecVShader]);

}


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



// Dx9texturestagestate.h

#ifndef __DX9TEXTURESTAGESTATE_H__
#define __DX9TEXTURESTAGESTATE_H__

#include "Dx9struct.h"


#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
    Function
 ****************************************************************************************************/

void	Dx9_InitTextureStageStates( void );

LONG	Dx9_TSS_UseTexture( DWORD stage, LONG textureId );


#define DEFINE_TSS_HANDLER(StateType, StateName, StateConstant) \
	__inline void Dx9_TSS_##StateName(DWORD stage, StateType value) \
	{ \
		if (gDx9SpecificData.TextureStageState[stage].StateName != value) \
		{ \
			gDx9SpecificData.TextureStageState[stage].StateName = value; \
			IDirect3DDevice9_SetTextureStageState(gDx9SpecificData.pD3DDevice, stage, StateConstant, \
				*(DWORD*)&gDx9SpecificData.TextureStageState[stage].StateName); \
		} \
	} \
	__inline void Dx9_TSS_Get##StateName(DWORD stage, StateType* value) \
	{ \
		IDirect3DDevice9_GetTextureStageState(gDx9SpecificData.pD3DDevice, stage, StateConstant, (DWORD*)value); \
	}


DEFINE_TSS_HANDLER(D3DTEXTUREOP,	ColorOp,		D3DTSS_COLOROP)
DEFINE_TSS_HANDLER(DWORD,			ColorArg1,		D3DTSS_COLORARG1) // D3DTA_*
DEFINE_TSS_HANDLER(DWORD,			ColorArg2,		D3DTSS_COLORARG2) // D3DTA_*
DEFINE_TSS_HANDLER(D3DTEXTUREOP,	AlphaOp,		D3DTSS_ALPHAOP)
DEFINE_TSS_HANDLER(DWORD,			AlphaArg1,		D3DTSS_ALPHAARG1) // D3DTA_*
DEFINE_TSS_HANDLER(DWORD,			AlphaArg2,		D3DTSS_ALPHAARG2) // D3DTA_*
DEFINE_TSS_HANDLER(float,			BumpEnvMat00,	D3DTSS_BUMPENVMAT00)
DEFINE_TSS_HANDLER(float,			BumpEnvMat01,	D3DTSS_BUMPENVMAT01)
DEFINE_TSS_HANDLER(float,			BumpEnvMat10,	D3DTSS_BUMPENVMAT10)
DEFINE_TSS_HANDLER(float,			BumpEnvMat11,	D3DTSS_BUMPENVMAT11)
DEFINE_TSS_HANDLER(DWORD,			TexCoordIndex,	D3DTSS_TEXCOORDINDEX)
DEFINE_TSS_HANDLER(float,			BumpEnvLScale,	D3DTSS_BUMPENVLSCALE)
DEFINE_TSS_HANDLER(float,			BumpEnvLOffset,	D3DTSS_BUMPENVLOFFSET)
DEFINE_TSS_HANDLER(D3DTEXTURETRANSFORMFLAGS, TextureTransformFlags, D3DTSS_TEXTURETRANSFORMFLAGS)
DEFINE_TSS_HANDLER(DWORD,			ColorArg0,		D3DTSS_COLORARG0) // D3DTA_*
DEFINE_TSS_HANDLER(DWORD,			AlphaArg0,		D3DTSS_ALPHAARG0) // D3DTA_*
DEFINE_TSS_HANDLER(DWORD,			ResultArg,		D3DTSS_RESULTARG) // D3DTA_*
DEFINE_TSS_HANDLER(DWORD,			Constant,		D3DTSS_CONSTANT)

#ifdef __cplusplus
}
#endif

#endif /* __DX9TEXTURESTAGESTATE_H__ */

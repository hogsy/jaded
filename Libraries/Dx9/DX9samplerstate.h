// Dx9samplerstate.h

#ifndef __DX9SAMPLERSTATE_H__
#define __DX9SAMPLERSTATE_H__

#include "Dx9struct.h"


#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
    Function
 ****************************************************************************************************/

void	Dx9_InitSamplerStates( void );

#define DEFINE_SS_HANDLER(StateType, StateName, StateConstant) \
	__inline void Dx9_SS_##StateName(DWORD sampler, StateType value) \
	{ \
		if (gDx9SpecificData.SamplerState[sampler].StateName != value) \
		{ \
			gDx9SpecificData.SamplerState[sampler].StateName = value; \
			IDirect3DDevice9_SetSamplerState(gDx9SpecificData.pD3DDevice, sampler, StateConstant, \
				gDx9SpecificData.SamplerState[sampler].StateName); \
		} \
	} \
	__inline void Dx9_SS_Get##StateName(DWORD stage, StateType* value) \
	{ \
		IDirect3DDevice9_GetSamplerState(gDx9SpecificData.pD3DDevice, stage, StateConstant, (DWORD*)value); \
	}

DEFINE_SS_HANDLER(D3DSAMPLERSTATETYPE,	AddressU,	D3DSAMP_ADDRESSU)
DEFINE_SS_HANDLER(D3DSAMPLERSTATETYPE,	AddressV,	D3DSAMP_ADDRESSV)
DEFINE_SS_HANDLER(D3DTEXTUREFILTERTYPE, MinFilter,	D3DSAMP_MINFILTER)
DEFINE_SS_HANDLER(D3DTEXTUREFILTERTYPE, MagFilter,	D3DSAMP_MAGFILTER)
DEFINE_SS_HANDLER(D3DTEXTUREFILTERTYPE, MipFilter,	D3DSAMP_MIPFILTER)

#ifdef __cplusplus
}
#endif

#endif /* __DX9SAMPLERSTATE_H__ */

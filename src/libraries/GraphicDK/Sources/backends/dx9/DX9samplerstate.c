// Dx9samplerstate.c


#include "Dx9samplerstate.h"

/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

#define INIT_SS_HANDLER(StateName) \
	Dx9_SS_Get##StateName(sampler, &gDx9SpecificData.SamplerState[sampler].StateName)

///////////////
void	Dx9_InitSamplerStates( void )
{
	DWORD sampler;

	for ( sampler = 0; sampler < gDx9SpecificData.d3dCaps.MaxTextureBlendStages; sampler ++ )
	{
		INIT_SS_HANDLER(AddressU);
		INIT_SS_HANDLER(AddressV);
		INIT_SS_HANDLER(MinFilter);
		INIT_SS_HANDLER(MagFilter);
		INIT_SS_HANDLER(MipFilter);
	}
}
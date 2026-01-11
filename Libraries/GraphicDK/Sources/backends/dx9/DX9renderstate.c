// Dx9renderstate.c


#include "Dx9renderstate.h"

/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

///////////////
void	Dx9_InitRenderStates( void )
{

    gDx9SpecificData.RenderState.ZEnable = D3DZB_FALSE;
    gDx9SpecificData.RenderState.ZFunc = D3DCMP_LESSEQUAL;
    gDx9SpecificData.RenderState.ZWriteEnable = TRUE;

    gDx9SpecificData.RenderState.FogEnable = TRUE;
    gDx9SpecificData.RenderState.FogColor = 0xFFFFFFFF;

    gDx9SpecificData.RenderState.AlphaTestEnable = FALSE;
    gDx9SpecificData.RenderState.AlphaFunc = D3DCMP_ALWAYS;
    gDx9SpecificData.RenderState.AlphaRef = 80;

	gDx9SpecificData.RenderState.AlphaBlendEnable = FALSE;
	gDx9SpecificData.RenderState.SrcBlend = D3DBLEND_ONE;
	gDx9SpecificData.RenderState.DestBlend = D3DBLEND_ZERO;

	gDx9SpecificData.RenderState.Lighting = FALSE;
	gDx9SpecificData.RenderState.FillMode = D3DFILL_SOLID;
    gDx9SpecificData.RenderState.CullMode = D3DCULL_CW;
	gDx9SpecificData.RenderState.ColorWriteEnable = D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED;

    //gDx9SpecificData.RenderState.currentVertexShader = 0xFFFFFFFF;
    gDx9SpecificData.RenderState.CurrentVertexShader = NULL; //VM
    gDx9SpecificData.RenderState.CurrentVertexFormat = 0; //VM

    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_ZENABLE, gDx9SpecificData.RenderState.ZEnable );
    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_ZFUNC, gDx9SpecificData.RenderState.ZFunc );
    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_ZWRITEENABLE, gDx9SpecificData.RenderState.ZWriteEnable );

    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_FOGENABLE, gDx9SpecificData.RenderState.FogEnable );
    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_FOGCOLOR, gDx9SpecificData.RenderState.FogColor );

    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_ALPHATESTENABLE, gDx9SpecificData.RenderState.AlphaTestEnable );
    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_ALPHAFUNC, gDx9SpecificData.RenderState.AlphaFunc );
    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_ALPHAREF, gDx9SpecificData.RenderState.AlphaRef );

    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_ALPHABLENDENABLE, gDx9SpecificData.RenderState.AlphaBlendEnable );
    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_SRCBLEND, gDx9SpecificData.RenderState.SrcBlend );
    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_DESTBLEND, gDx9SpecificData.RenderState.DestBlend );

    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_LIGHTING, gDx9SpecificData.RenderState.Lighting );
	IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_FILLMODE, gDx9SpecificData.RenderState.FillMode );
    IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_CULLMODE, gDx9SpecificData.RenderState.CullMode );
	IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_COLORWRITEENABLE, gDx9SpecificData.RenderState.ColorWriteEnable );


	IDirect3DDevice9_SetRenderState( gDx9SpecificData.pD3DDevice, D3DRS_NORMALIZENORMALS, TRUE );

/*
    // default values
    // take the per-vertex diffuse (RLI) as an emmissive color, so that we can use RLI's and HW light at the same time
    // This does not work on all PC cards, so in that case we have to switch to SW vertex processing...
#if (!defined(_XBOX)) && defined(_DEBUG)
    if (!(gDx9SpecificData.m_d3dCaps.VertexProcessingCaps & D3DVTXPCAPS_MATERIALSOURCE7))
    {
        OutputDebugString("TODO: RLI+lights management when device doesn't support selectable vertex color sources.\n"); // see comments above
    }
#endif (!defined(_XBOX)) && defined(_DEBUG)
    IDirect3DDevice9_SetRenderState(gDx9SpecificData.pD3DDevice, D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL); // default was D3DMCS_COLOR1
    IDirect3DDevice9_SetRenderState(gDx9SpecificData.pD3DDevice, D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
    IDirect3DDevice9_SetRenderState(gDx9SpecificData.pD3DDevice, D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL); // beware of the DX8 doc bug...
    IDirect3DDevice9_SetRenderState(gDx9SpecificData.pD3DDevice, D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1); // default was D3DMCS_MATERIAL

	Gx8_vSetNormalMaterial();
*/
}


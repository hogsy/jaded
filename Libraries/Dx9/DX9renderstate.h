// Dx9renderstate.h

#ifndef __DX9RENDERSTATE_H__
#define __DX9RENDERSTATE_H__

#include "Dx9struct.h"


#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
    Function
 ****************************************************************************************************/

void	Dx9_InitRenderStates( void );


/****************************************************************************************************
    Inline Function
 ****************************************************************************************************/

///////////////////
__inline void	Dx9_BeginScene( void )
{
	if ( !gDx9SpecificData.RenderScene ) 
	{
		IDirect3DDevice9_BeginScene( gDx9SpecificData.pD3DDevice );
		gDx9SpecificData.RenderScene = TRUE;
	}
}

///////////////////
__inline void	Dx9_EndScene( void )
{
	if ( gDx9SpecificData.RenderScene ) 
	{
		IDirect3DDevice9_EndScene( gDx9SpecificData.pD3DDevice );
		gDx9SpecificData.RenderScene = FALSE;
	}
}

///////////////////

#define DEFINE_RS_HANDLER(StateType, StateName, StateConstant) \
	__inline void Dx9_RS_##StateName(StateType value) \
	{ \
		if (gDx9SpecificData.RenderState.StateName != value) \
		{ \
			gDx9SpecificData.RenderState.StateName = value; \
			IDirect3DDevice9_SetRenderState(gDx9SpecificData.pD3DDevice, StateConstant, \
				*(DWORD*)&gDx9SpecificData.RenderState.StateName); \
		} \
	}

DEFINE_RS_HANDLER(D3DZBUFFERTYPE,	ZEnable,			D3DRS_ZENABLE)
DEFINE_RS_HANDLER(D3DCMPFUNC,		ZFunc,				D3DRS_ZFUNC)
DEFINE_RS_HANDLER(BOOL,				ZWriteEnable,		D3DRS_ZWRITEENABLE)
DEFINE_RS_HANDLER(BOOL,				FogEnable,			D3DRS_FOGENABLE)
DEFINE_RS_HANDLER(DWORD,			FogColor,			D3DRS_FOGCOLOR)
DEFINE_RS_HANDLER(D3DFILLMODE,		FillMode,			D3DRS_FILLMODE)
DEFINE_RS_HANDLER(D3DCULL,			CullMode,			D3DRS_CULLMODE)
DEFINE_RS_HANDLER(DWORD,			ColorWriteEnable,	D3DRS_COLORWRITEENABLE)
DEFINE_RS_HANDLER(BOOL,				AlphaTestEnable,	D3DRS_ALPHATESTENABLE)
DEFINE_RS_HANDLER(D3DCMPFUNC,		AlphaFunc,			D3DRS_ALPHAFUNC)
DEFINE_RS_HANDLER(DWORD,			AlphaRef,			D3DRS_ALPHAREF)
DEFINE_RS_HANDLER(BOOL,				AlphaBlendEnable,	D3DRS_ALPHABLENDENABLE)
DEFINE_RS_HANDLER(D3DBLEND,			SrcBlend,			D3DRS_SRCBLEND)
DEFINE_RS_HANDLER(D3DBLEND,			DestBlend,			D3DRS_DESTBLEND)
DEFINE_RS_HANDLER(BOOL,				Lighting,			D3DRS_LIGHTING)
DEFINE_RS_HANDLER(BOOL,				NormalizeNormals,	D3DRS_NORMALIZENORMALS)
DEFINE_RS_HANDLER(DWORD,			TextureFactor,		D3DRS_TEXTUREFACTOR)

///////////////////
__inline void    Dx9_RS_SetVertexShader( IDirect3DVertexShader9* pVertexShader )
{
	if ( gDx9SpecificData.RenderState.CurrentVertexShader != pVertexShader )
	{
		IDirect3DDevice9_SetVertexShader( gDx9SpecificData.pD3DDevice, pVertexShader );
		gDx9SpecificData.RenderState.CurrentVertexShader = pVertexShader;
	}
}

///////////////////
//VM
__inline void	Dx9_RS_SetVertexFormat( DWORD vertexFormat, bool isForced )
{
	if(
		( gDx9SpecificData.RenderState.CurrentVertexFormat != vertexFormat )
		|| isForced
	)
	{
		IDirect3DDevice9_SetFVF( gDx9SpecificData.pD3DDevice, vertexFormat );
		gDx9SpecificData.RenderState.CurrentVertexFormat = vertexFormat;
	}
}

///////////////////
__inline void	Dx9_RS_SetVertexDeclaration( IDirect3DVertexDeclaration9* vertexDecl )
{
	if ( gDx9SpecificData.RenderState.CurrentVertexDeclaration != vertexDecl )
	{
		IDirect3DDevice9_SetVertexDeclaration( gDx9SpecificData.pD3DDevice, vertexDecl );
		gDx9SpecificData.RenderState.CurrentVertexDeclaration = vertexDecl;
	}
}


#ifdef __cplusplus
}
#endif

#endif /* __DX9RENDERSTATE_H__ */

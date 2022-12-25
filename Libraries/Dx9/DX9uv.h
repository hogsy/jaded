// Dx9uv.h

#ifndef __DX9UV_H__
#define __DX9UV_H__

#include "Dx9struct.h"

#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
    Function
 ****************************************************************************************************/

void                    Dx9_SetUVStageMatrix(D3DMATRIX *pstMatrix);
void                    Dx9_SetUVChromeStage(void);
void                    Dx9_SetUVNormalStage(void);
void                    Dx9_SetUVStageOff(void);
void                    Dx9_SetUVRotationStage(void);
Dx9_tdeUVGenerationType Dx9_GetCurrentUVState(void);


/****************************************************************************************************
    Inline Function
 ****************************************************************************************************/

///////////////////
__forceinline void Dx9_SetUVStageMatrix( D3DMATRIX *pstMatrix )
{
	IDirect3DDevice9_SetTransform( gDx9SpecificData.pD3DDevice, D3DTS_TEXTURE0, pstMatrix );
}

///////////////////
__forceinline void Dx9_SetUVChromeStage( void )
{
	
	if ( gDx9SpecificData.UVType != Dx9_eUVChrome )
	{
		gDx9SpecificData.UVType = Dx9_eUVChrome;
		IDirect3DDevice9_SetTextureStageState( gDx9SpecificData.pD3DDevice, 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
		IDirect3DDevice9_SetTextureStageState( gDx9SpecificData.pD3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	}
	
}

///////////////////
__forceinline void Dx9_SetUVNormalStage( void )
{
	
	if ( gDx9SpecificData.UVType != Dx9_eUVNormal )
	{
		gDx9SpecificData.UVType = Dx9_eUVNormal;
		IDirect3DDevice9_SetTextureStageState( gDx9SpecificData.pD3DDevice, 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
		IDirect3DDevice9_SetTextureStageState( gDx9SpecificData.pD3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	}
	
}

///////////////////
__forceinline void Dx9_SetUVStageOff( void )
{
	
	if ( gDx9SpecificData.UVType != Dx9_eUVOff )
	{
		gDx9SpecificData.UVType = Dx9_eUVOff;
		IDirect3DDevice9_SetTextureStageState( gDx9SpecificData.pD3DDevice, 0, D3DTSS_TEXCOORDINDEX, 0 );
		IDirect3DDevice9_SetTextureStageState( gDx9SpecificData.pD3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	}
	
}

///////////////////
__forceinline void Dx9_SetUVRotationStage( void )
{
	
	if ( gDx9SpecificData.UVType != Dx9_eUVRotation )
	{
		gDx9SpecificData.UVType = Dx9_eUVRotation;
		IDirect3DDevice9_SetTextureStageState( gDx9SpecificData.pD3DDevice, 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
		IDirect3DDevice9_SetTextureStageState( gDx9SpecificData.pD3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	}
	
}

///////////////////
__forceinline Dx9_tdeUVGenerationType Dx9_GetCurrentUVState( void )
{
	return gDx9SpecificData.UVType;
}



#ifdef __cplusplus
}
#endif

#endif /* __DX9UV_H__ */

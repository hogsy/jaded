// Dx9matrix.h

#ifndef __DX9MATRIX_H__
#define __DX9MATRIX_H__

#include "Dx9struct.h"
#include "CAMera/CAMera.h"


#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
    Function
 ****************************************************************************************************/

void	Dx9_SetViewMatrix( MATH_tdst_Matrix *pMatrix);
void	Dx9_SetWorldMatrix( MATH_tdst_Matrix *pMatrix);
void	Dx9_SetProjectionMatrix( CAM_tdst_Camera *pCamera);
void	Dx9_SetViewMatrix_SDW( MATH_tdst_Matrix *pMatrix, float *limits );
void	Dx9_SetTextureTarget( ULONG textureNumber, ULONG clear);
void	Dx9_Ortho( float Left, float Right, float Bottom, float Top, float Near, float Far );

void	Dx9_SetViewport( LONG x, LONG y, LONG w, LONG h );

__inline void Dx9_SetProjectionMatrixX( const D3DMATRIX* pMatrix )
{
	IDirect3DDevice9_SetTransform(gDx9SpecificData.pD3DDevice, D3DTS_PROJECTION, pMatrix);
}

__inline void Dx9_SetViewMatrixX( const D3DMATRIX* pMatrix )
{
	IDirect3DDevice9_SetTransform(gDx9SpecificData.pD3DDevice, D3DTS_VIEW, pMatrix);
}

__inline void Dx9_SetWorldMatrixX( const D3DMATRIX* pMatrix )
{
	IDirect3DDevice9_SetTransform(gDx9SpecificData.pD3DDevice, D3DTS_WORLD, pMatrix);
}

__inline void Dx9_SetTextureMatrixX( int stage, const D3DMATRIX* pMatrix )
{
	IDirect3DDevice9_SetTransform(gDx9SpecificData.pD3DDevice, D3DTS_TEXTURE0 + stage, pMatrix);
}

__inline void Dx9_GetProjectionMatrixX( D3DMATRIX* pMatrix )
{
	IDirect3DDevice9_GetTransform(gDx9SpecificData.pD3DDevice, D3DTS_PROJECTION, pMatrix);
}

__inline void Dx9_GetViewMatrixX( D3DMATRIX* pMatrix )
{
	IDirect3DDevice9_GetTransform(gDx9SpecificData.pD3DDevice, D3DTS_VIEW, pMatrix);
}

__inline void Dx9_GetWorldMatrixX( D3DMATRIX* pMatrix )
{
	IDirect3DDevice9_GetTransform(gDx9SpecificData.pD3DDevice, D3DTS_WORLD, pMatrix);
}

__inline void Dx9_GetTextureMatrixX( int stage, D3DMATRIX* pMatrix )
{
	IDirect3DDevice9_GetTransform(gDx9SpecificData.pD3DDevice, D3DTS_TEXTURE0 + stage, pMatrix);
}

#ifdef __cplusplus
}
#endif

#endif /* __DX9MATRIX_H__ */

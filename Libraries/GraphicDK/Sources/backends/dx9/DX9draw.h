// Dx9draw.h

#ifndef __DX9DRAW_H__
#define __DX9DRAW_H__

#include "Dx9struct.h"
#include "GEOmetric/GEOobject.h"


#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
    Function
 ****************************************************************************************************/

LONG	Dx9_l_DrawElementIndexedTriangles( GEO_tdst_ElementIndexedTriangles	* _pst_Element,
										  GEO_Vertex						* _pst_Point,
										  GEO_tdst_UV						* _pst_UV,
										  ULONG								ulnumberOfPoints );

LONG	Dx9_l_DrawElementIndexedSprites( GEO_tdst_ElementIndexedSprite	*_pst_Element,
										GEO_Vertex						*_pst_Point,
										ULONG							ulnumberOfPoints );



__inline void Dx9_DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,
								  UINT PrimitiveCount,
								  const void *pVertexStreamZeroData,
								  UINT VertexStreamZeroStride)
{
	IDirect3DDevice9_DrawPrimitiveUP(
		gDx9SpecificData.pD3DDevice, PrimitiveType, PrimitiveCount,
		pVertexStreamZeroData, VertexStreamZeroStride);
}

#ifdef __cplusplus
}
#endif

#endif /* __DX9DRAW_H__ */

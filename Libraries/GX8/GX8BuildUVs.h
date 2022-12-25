/*=========================================================================
*
* BuildUVs.h	- UV functions for new object format
*
* Version			1.0
* Revision date
*
*=======================================================================*/
#ifndef __BUILDUV_H__
#define __BUILDUV_H__


//--- Includes --------------------------------------------------------

#include "GEOmetric/GEOobject.h"

//----------------------------------------------------------------------

/********************************************************/
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus 
/********************************************************/

int Gx8_ComputeVertexBufferFriendlyDataForGeometricObject(GEO_tdst_Object *p_stObject,
                                                          ULONG           *dul_VertexColors);

void Gx8_UpdateModifiedObject(GEO_tdst_Object *p_stObject,
			                  GEO_tdst_ElementIndexedTriangles *p_stElement,
                              ULONG           *dul_VertexColors,
                              int			   iIndex,
                              ULONG            ulDisplayInfo);

void Gx8_CreateAddInfo(GEO_tdst_Object	*p_stObject);
void Gx8_CreateSpriteVBData(GEO_tdst_Object	*p_stObject);



void Gx8_SetCurrentGameObject(void *);
//void Gx8_CopyVertexBuffer(LPDIRECT3DVERTEXBUFFER8 pDest,LPDIRECT3DVERTEXBUFFER8 pSource,int Size);

/********************************************************/
#ifdef __cplusplus
}
#endif // __cplusplus 
/********************************************************/

#endif // __BUILDUV_H__ 

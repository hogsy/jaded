#ifndef _GX8VERTEXBUFFER_H_
#define _GX8VERTEXBUFFER_H_

#pragma once

#include <Gx8/Gx8Init.h>
#include <GEOmetric/GEOobject.h>

//struct Gx8_stAddInfo *Gx8_fnpstAllocateElement(struct Gx8_tdst_SpecificData_ *p_SD,GEO_tdst_ElementIndexedTriangles *pstEIT,
//											   int iDynamic);
//void Gx8_fnvAddFree(struct Gx8_stAddInfo *pstAdd);
void Gx8_FreeAddInfo(GEO_tdst_Object *p_stObject);
//void Gx8_fnvReallocVertexBuffer(struct Gx8_stAddInfo *pstAdd,ULONG _ul_NbTriangles);

/**
 * Creates the singleton VertexBuffer. This has to be called once before
 * start dealing with graphics.
 *
 * @param device the DirectX device.
 */

void Gx8_VertexBuffer_Create( IDirect3DDevice8* device );

/**
 * Destroys the vertexbuffer singleton. Ideally you would call it after
 * having dealt with graphics. Likely you never need to call it - just
 * switch off the XBox.
 */
void Gx8_VertexBuffer_Destroy(void);

/**
 * Resets the VertexBuffer content. This may be called to avoid sticky
 * allocation to survive between game sections.
 */
void Gx8_VertexBuffer_Clean(void);

/**
 * Allocates a portion of the vertex buffer suitable for storing
 * geometry information.
 * All allocated data has to be freed via a call to #Gx8_VertexBuffer_Free.
 *
 * @param vertexCount the number of needed vertices.
 * @param stride the size, in bytes, of the information related to a
 *               single vertex.
 * @return a pointer to a #Gx8_VertexBuffer_Chunk. When you have done with
 *         the vertex buffer, pass this pointer to #Gx8_VertexBuffer_Free.
 *
 */
IDirect3DVertexBuffer8* 
Gx8_VertexBuffer_Allocate( unsigned int vertexCount, unsigned int stride );

/**
 * Releases a portion of the vertex buffer.
 *
 * @param chunk a pointer to a chunk as returned by 
 *              #Gx8_VertexBuffer_Allocate.
 */
void Gx8_VertexBuffer_Free( IDirect3DVertexBuffer8* vertexBuffer );


#endif

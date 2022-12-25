#include "Gx8VertexBuffer.h"

#include <xtl.h>
#include <D3D8.h>
#include <XGraphics.h>
#include <assert.h>

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "GX8init.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "GEOmetric/GEO_STRIP.h"

#ifndef _GX8ADDINFO_H_
#include "Gx8AddInfo.h"
#endif

extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;


void Gx8_FreeAddInfo(GEO_tdst_Object *p_stObject)
{
	GEO_tdst_ElementIndexedTriangles	* pst_Element;
	Gx8_tdstMultipleVB					* pMultipleVB;
	Gx8_tdstMultipleVBList				* pMultipleVBList;
	Gx8_tdstSpriteVBData				* pSpriteVBData;
	int				xElem;
	unsigned int	iMVB, iVB;
	HRESULT			hr;


	if ( ( !p_stObject ) || ( !p_stObject->dst_Element ) || ( !p_stObject->dst_Element[0].pst_Gx8Add ) )
		return;

    for (xElem=0; xElem<p_stObject->l_NbElements; xElem++)
	{	// for each element
        pst_Element = &p_stObject->dst_Element[xElem];

		// get pMultipleVBList
		pMultipleVBList = pst_Element->pst_Gx8Add->pMultipleVBList;
		if ( pMultipleVBList )
		{
			// for each MVB
			for (iMVB = 0; iMVB < pMultipleVBList->iMVBNumber; iMVB++)
			{	
				// get pMultipleVB
				pMultipleVB = &pMultipleVBList->pMultipleVB[ iMVB ];

				// for each VB
				for (iVB = 0; iVB < pMultipleVB->iVBNumber; iVB++)
				{	
					// release VB if needed
					if ( pMultipleVB->pVBData[ iVB ].pVB )
					{
						hr = IDirect3DVertexBuffer8_Release( pMultipleVB->pVBData[ iVB ].pVB );
						assert (hr==0);
					}
				}

				// free VB list
				MEM_Free( pMultipleVB->pVBData );
			}

			// free Multiple VB
			MEM_Free( pMultipleVBList->pMultipleVB );

			// free Multiple VB List
			MEM_Free( pMultipleVBList );
		}	

		// release Index Buffer
		if ( pst_Element->pst_Gx8Add->lpd3dibIndexBuffer )
		{
			hr = IDirect3DIndexBuffer8_Release( pst_Element->pst_Gx8Add->lpd3dibIndexBuffer );
			assert (hr==0);
		}
	}

	// release Sprite VB Data
	pSpriteVBData = p_stObject->dst_Element[0].pst_Gx8Add->pSpriteVBData;
	if ( pSpriteVBData )
	{
		// for each level
		for ( iVB = 0; iVB < GX8_SPG_LEVEL_NUMBER; iVB++)
		{
				// release Sprite VB
			hr = IDirect3DVertexBuffer8_Release( pSpriteVBData[iVB].pSPG_VB );
			assert (hr==0);
		}

		// free VB list
		MEM_Free( pSpriteVBData );
	}

	// free Gx8AddInfo 
    for (xElem=0; xElem<p_stObject->l_NbElements; xElem++)
	{	// for each element
        pst_Element = &p_stObject->dst_Element[xElem];

		if ( pst_Element->pst_Gx8Add )
		{
			// free Construction List 
			if ( pst_Element->pst_Gx8Add->d_stVBConstructionList )
			{
				MEM_Free( pst_Element->pst_Gx8Add->d_stVBConstructionList );
			}

			// free add structure
			MEM_Free( pst_Element->pst_Gx8Add );
			pst_Element->pst_Gx8Add = NULL;
		}
	}

}


#define Gx8_VertexBuffer_C_Size (5*1024*1024 + (512+256+64+128+64)*1024)//<------------ TEST+64newyork

IDirect3DVertexBuffer8* Gx8_VertexBuffer_g_VertexBuffer;
IDirect3DDevice8* Gx8_VertexBuffer_g_Device;

static BYTE* _bufferBase;
static BYTE* _nextFree;
static BYTE* _bufferEnd;
static BYTE* _fixTop;

#if defined( _DEBUG )
size_t usedMemory;
size_t maxUsedMemory;
size_t allocCount;
size_t wastedMemory;
size_t fixUsedMemory;
size_t fixAllocCount;
size_t fixWastedMemory;
#endif

void 
Gx8_VertexBuffer_Create( IDirect3DDevice8* device )
{
    HRESULT hr;

    ERR_X_Assert( Gx8_VertexBuffer_g_VertexBuffer == NULL );
    hr = IDirect3DDevice8_CreateVertexBuffer( device,
                                              Gx8_VertexBuffer_C_Size,
                                              0,
                                              0,
                                              0,
                                              &Gx8_VertexBuffer_g_VertexBuffer );
    ERR_X_Assert( SUCCEEDED( hr ));
    Gx8_VertexBuffer_g_Device = device;
    hr = IDirect3DVertexBuffer8_Lock( Gx8_VertexBuffer_g_VertexBuffer,
                                      0,
                                      0,
                                      &_bufferBase,
                                      D3DLOCK_NOOVERWRITE );
    ERR_X_Assert( SUCCEEDED( hr ));
    _nextFree = _bufferBase;
    _fixTop = NULL;
    _bufferEnd = _bufferBase+Gx8_VertexBuffer_C_Size;
#if defined( _DEBUG )
    usedMemory = 0;
    maxUsedMemory = 0;
    allocCount = 0;
    wastedMemory = 0;
#endif
}


void Gx8_VertexBuffer_Destroy( void )
{
    ULONG refCount;

    ERR_X_Assert( Gx8_VertexBuffer_g_VertexBuffer != NULL );
    refCount = IDirect3DVertexBuffer8_Release( Gx8_VertexBuffer_g_VertexBuffer );
    Gx8_VertexBuffer_g_VertexBuffer = NULL;
    Gx8_VertexBuffer_g_Device = NULL;
}

typedef enum
{
    CS_RESET,
    CS_FIRST_CLEAN,
    CS_FIX_CLEAN,
    CS_STANDARD_CLEAN
} CleanState;

CleanState _cleanState = CS_RESET;

void Gx8_VertexBuffer_Clean( void )
{
    switch( _cleanState )
    {
        case CS_RESET:
            _cleanState = CS_FIX_CLEAN;

#if defined( _DEBUG )
           assert( usedMemory == 0 );
            usedMemory = 0;
            allocCount = 0;
            wastedMemory = 0;
#endif
            break;
        case CS_FIX_CLEAN:
            _fixTop = _nextFree;
            _cleanState = CS_STANDARD_CLEAN;
#if defined( _DEBUG )
            fixUsedMemory = usedMemory;
            fixAllocCount = allocCount;
            fixWastedMemory = wastedMemory;
#endif
            break;
        case CS_STANDARD_CLEAN:
            _nextFree = _fixTop;    // preserve the fix
#if defined( _DEBUG )
            usedMemory = fixUsedMemory;
            allocCount = fixAllocCount;
            wastedMemory = fixWastedMemory;
#endif
            break;
        case CS_FIRST_CLEAN:
            // fall through
        default:
            assert( false && "something weird happened" );
            break;
    }


}


static BYTE*
allocateTheNeededSpace( unsigned int vertexCount, unsigned int stride )
{
    ULONG requiredSize = vertexCount * stride;
    UINT fakeCount;
    BYTE* memoryPointer;

//    requiredSize = (requiredSize + 0x0000001F) & 0xFFFFFFE0;       // round to 32 bytes page.
    ERR_X_Assert( _nextFree + requiredSize <= _bufferEnd );

    fakeCount = (_nextFree - _bufferBase) / stride;
    if( (_nextFree - _bufferBase) % stride != 0 )
    {
        _nextFree = _bufferBase + (fakeCount+1)*stride;
    }
    // This is an A-BUG. When updating duplicated object two extra vertices
    // are updated. In order to prevent that this bug cause slight geometry
    // corruption, 2 extra unused vertices are allocated.
    _nextFree += 0*stride;

    memoryPointer = _nextFree;

    _nextFree += requiredSize;

#if defined( _DEBUG )
    usedMemory += requiredSize;
    allocCount++;
    wastedMemory += (requiredSize + 0x00000FFF) & 0xFFFFF000;
    if( usedMemory > maxUsedMemory )
    {
        maxUsedMemory = usedMemory;
    }
#endif

    return memoryPointer;
}

IDirect3DVertexBuffer8* 
Gx8_VertexBuffer_Allocate( unsigned int vertexCount, unsigned int stride )
{
    IDirect3DVertexBuffer8* vertexBuffer;
    BYTE* pointer; 

    pointer = allocateTheNeededSpace( vertexCount, stride );
    if( pointer == NULL )
    {
        return NULL;
    }
    vertexBuffer = MEM_p_Alloc( sizeof( *vertexBuffer ));
    if( vertexBuffer != NULL )
    {
        memset( vertexBuffer, 0, sizeof( *vertexBuffer ));
        XGSetVertexBufferHeader( 0, 0, 0, 0, vertexBuffer, 0 );
        IDirect3DResource8_Register( (D3DResource*)vertexBuffer, pointer );
    }
    return vertexBuffer;
}

void Gx8_VertexBuffer_Free( IDirect3DVertexBuffer8* buffer )
{
    IDirect3DResource8_BlockUntilNotBusy( (D3DResource*)buffer );
    MEM_Free( buffer );
}

//-----------------------------------------------------------------------------
// File: XBResource.cpp
//
// Desc: Loads resources from an XPR (Xbox Packed Resource) file.  
//
// Hist: 03.12.01 - New for April XDK release
//       10.15.02 - Modifed to register resources in a more flexible way
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>
#include <stdio.h>
#include "XBUtil.h"
#include "XBResource.h"




//-----------------------------------------------------------------------------
// Magic values to identify XPR files
//-----------------------------------------------------------------------------
#define XPR0_MAGIC_VALUE 0x30525058
#define XPR1_MAGIC_VALUE 0x31525058




//-----------------------------------------------------------------------------
// Name: XBResource_SizeOf()
// Desc: Determines the byte size of a D3DResource
//-----------------------------------------------------------------------------
DWORD XBResource_SizeOf( LPDIRECT3DRESOURCE8 pResource )
{
    switch( pResource->GetType() )
    {
        case D3DRTYPE_TEXTURE:       return sizeof(D3DTexture);
        case D3DRTYPE_VOLUMETEXTURE: return sizeof(D3DVolumeTexture);
        case D3DRTYPE_CUBETEXTURE:   return sizeof(D3DCubeTexture);
        case D3DRTYPE_VERTEXBUFFER:  return sizeof(D3DVertexBuffer);
        case D3DRTYPE_INDEXBUFFER:   return sizeof(D3DIndexBuffer);
        case D3DRTYPE_PALETTE:       return sizeof(D3DPalette);
    }
    return 0;
}




//-----------------------------------------------------------------------------
// Name: CXBPackedResource()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBPackedResource::CXBPackedResource()
{
    m_pSysMemData       = NULL;
    m_dwSysMemDataSize  = 0L;
    m_pVidMemData       = NULL;
    m_dwVidMemDataSize  = 0L;
    m_pResourceTags     = NULL;
    m_dwNumResourceTags = 0L;
}




//-----------------------------------------------------------------------------
// Name: ~CXBPackedResource()
// Desc: Destructor
//-----------------------------------------------------------------------------
CXBPackedResource::~CXBPackedResource()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name: GetData()
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
VOID* CXBPackedResource::GetData( const CHAR* strName ) const
{
    if( NULL==m_pResourceTags || NULL==strName )
        return NULL;

    for( DWORD i=0; m_pResourceTags[i].strName; i++ )
    {
        if( !_stricmp( strName, m_pResourceTags[i].strName ) )
            return &m_pSysMemData[m_pResourceTags[i].dwOffset];
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
HRESULT CXBPackedResource::Create( const CHAR* strFilename, DWORD dwNumResourceTags,
                                   XBRESOURCE* pResourceTags )
{
    BOOL bHasResourceOffsetsTable = FALSE;

    // Find the media file
    CHAR strResourcePath[512];
    if( FAILED( XBUtil_FindMediaFile( strResourcePath, strFilename ) ) )
        return E_FAIL;

    // Open the file
    HANDLE hFile;
    DWORD dwNumBytesRead;
    hFile = CreateFile( strResourcePath, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL );
    if( hFile == INVALID_HANDLE_VALUE )
    {
//        OUTPUT_DEBUG_STRING( "CXBPackedResource::Create(): ERROR: File not found!\n" );
        return E_FAIL;
    }
   
    // Read in and verify the XPR magic header
    XPR_HEADER xprh;
    ReadFile( hFile, &xprh, sizeof(XPR_HEADER), &dwNumBytesRead, NULL );
    
    if( xprh.dwMagic == XPR0_MAGIC_VALUE )
    {
        bHasResourceOffsetsTable = FALSE;
    }
    else if( xprh.dwMagic == XPR1_MAGIC_VALUE )
    {
        bHasResourceOffsetsTable = TRUE;
    }
    else
    {
        CloseHandle( hFile );
//        OUTPUT_DEBUG_STRING( "Invalid Xbox Packed Resource (.xpr) file" );
        return E_INVALIDARG;
    }

    // Compute memory requirements
    m_dwSysMemDataSize = xprh.dwHeaderSize - sizeof(XPR_HEADER);
    m_dwVidMemDataSize = xprh.dwTotalSize - xprh.dwHeaderSize;

    // Allocate memory
    m_pSysMemData = new BYTE[m_dwSysMemDataSize];
    m_pVidMemData = (BYTE*)D3D_AllocContiguousMemory( m_dwVidMemDataSize, D3DTEXTURE_ALIGNMENT );

    // Read in the data from the file
    ReadFile( hFile, m_pSysMemData, m_dwSysMemDataSize, &dwNumBytesRead, NULL );
    ReadFile( hFile, m_pVidMemData, m_dwVidMemDataSize, &dwNumBytesRead, NULL );

    // Done with the file
    CloseHandle( hFile );
    
    // Extract resource table from the header data
    if( bHasResourceOffsetsTable )
    {
        m_dwNumResourceTags = *(DWORD*)(m_pSysMemData+0);
        m_pResourceTags     = (XBRESOURCE*)(m_pSysMemData+4);

        // Patch up the resource strings
        for( DWORD i=0; i<m_dwNumResourceTags; i++ )
            m_pResourceTags[i].strName = (CHAR*)( m_pSysMemData + (DWORD)m_pResourceTags[i].strName );
    }

    // Use user-supplied number of resources and the resource tags
    if( dwNumResourceTags !=0 || pResourceTags != NULL )
    {
        m_pResourceTags     = pResourceTags;
        m_dwNumResourceTags = dwNumResourceTags;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetResourceTags()
// Desc: Retrieves the resource tags
//-----------------------------------------------------------------------------
HRESULT CXBPackedResource::GetResourceTags( DWORD* pdwNumResourceTags,
                                            XBRESOURCE** ppResourceTags )
{
    if( pdwNumResourceTags )
        (*pdwNumResourceTags) = m_dwNumResourceTags;

    if( ppResourceTags )
        (*ppResourceTags) = m_pResourceTags;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Cleans up the packed resource data
//-----------------------------------------------------------------------------
VOID CXBPackedResource::Destroy() 
{
    if( m_pSysMemData != NULL )
        delete[] m_pSysMemData;
    m_pSysMemData      = NULL;
    m_dwSysMemDataSize = 0L;
    
    if( m_pVidMemData != NULL )
        D3D_FreeContiguousMemory( m_pVidMemData );
    m_pVidMemData      = NULL;
    m_dwVidMemDataSize = 0L;
    
    m_pResourceTags     = NULL;
    m_dwNumResourceTags = 0L;
}




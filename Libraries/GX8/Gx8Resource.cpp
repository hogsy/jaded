#include <xtl.h>
#include <xgraphics.h>
#include <stdio.h>
#include "Gx8Util.h"
#include "Gx8resource.h"




//-----------------------------------------------------------------------------
// Magic values to identify XPR files
//-----------------------------------------------------------------------------
#define XPR0_MAGIC_VALUE 0x30525058
#define XPR1_MAGIC_VALUE 0x31525058

#define FONT_FROM_SECTION	1 
 

//-----------------------------------------------------------------------------
// Name: XBResource_SizeOf()
// Desc: Determines the byte size of a D3DResource
//-----------------------------------------------------------------------------
/*DWORD XBResource_SizeOf( LPDIRECT3DRESOURCE8 pResource )
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
*/




//-----------------------------------------------------------------------------
// Name: Gx8Resource()
// Desc: Constructor
//-----------------------------------------------------------------------------
Gx8Resource::Gx8Resource()
{
    m_pSysMemData       = NULL;
    m_dwSysMemDataSize  = 0L;
    m_pVidMemData       = NULL;
    m_dwVidMemDataSize  = 0L;
    m_pResourceTags     = NULL;
    m_dwNumResourceTags = 0L;
}




//-----------------------------------------------------------------------------
// Name: ~Gx8Resource()
// Desc: Destructor
//-----------------------------------------------------------------------------
Gx8Resource::~Gx8Resource()
{
	//release the font section
    Destroy();
}




//-----------------------------------------------------------------------------
// Name: GetData()
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
VOID* Gx8Resource::GetData( const CHAR* strName ) const
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
HRESULT Gx8Resource::Create( const CHAR* strFilename, DWORD dwNumResourceTags,
                                   XBRESOURCE* pResourceTags )
{
    BOOL bHasResourceOffsetsTable = FALSE;

    XPR_HEADER xprh;
#if !defined( FONT_FROM_SECTION )
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
        OUTPUT_DEBUG_STRING( "Gx8Resource::Create(): ERROR: File not found!\n" );
        return E_FAIL;
    }
   
    // Read in and verify the XPR magic header
    ReadFile( hFile, &xprh, sizeof(XPR_HEADER), &dwNumBytesRead, NULL );
#else
	//Load the section and "simulate" file read from now on...
	char*	pFontSection;
	if((pFontSection = (char *)XLoadSection("FONTSECT"))==NULL)
		return FALSE;

	memcpy(&xprh,(char *)pFontSection,sizeof(XPR_HEADER));
	pFontSection += sizeof(XPR_HEADER);
#endif
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
#if !defined( FONT_FROM_SECTION )
        CloseHandle( hFile );
#endif
        OUTPUT_DEBUG_STRING( "Invalid Xbox Packed Resource (.xpr) file" );
        return E_INVALIDARG;
    }

    // Compute memory requirements
    m_dwSysMemDataSize = xprh.dwHeaderSize - sizeof(XPR_HEADER);
    m_dwVidMemDataSize = xprh.dwTotalSize - xprh.dwHeaderSize;

    // Allocate memory
    m_pSysMemData = new BYTE[m_dwSysMemDataSize];
    m_pVidMemData = (BYTE*)D3D_AllocContiguousMemory( m_dwVidMemDataSize, D3DTEXTURE_ALIGNMENT );

#if !defined( FONT_FROM_SECTION )
    // Read in the data from the file
    ReadFile( hFile, m_pSysMemData, m_dwSysMemDataSize, &dwNumBytesRead, NULL );
    ReadFile( hFile, m_pVidMemData, m_dwVidMemDataSize, &dwNumBytesRead, NULL );

	// Done with the file
    CloseHandle( hFile );
#else
	//fill the memory with the content
    memcpy( m_pSysMemData, pFontSection, (m_dwSysMemDataSize));
	pFontSection += (m_dwSysMemDataSize);
    memcpy( m_pVidMemData, pFontSection, (m_dwVidMemDataSize));
	pFontSection += (m_dwVidMemDataSize);

	//release the section
	XFreeSection("FONTSECT");

#endif
    
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
HRESULT Gx8Resource::GetResourceTags( DWORD* pdwNumResourceTags,
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
VOID Gx8Resource::Destroy() 
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




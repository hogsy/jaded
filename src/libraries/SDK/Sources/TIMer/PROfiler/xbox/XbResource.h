//-----------------------------------------------------------------------------
// File: XBResource.h
//
// Desc: Loads resources from an XPR (Xbox Packed Resource) file.  
//
// Hist: 03.12.01 - New for April XDK release
//       10.15.02 - Modifed to register resources in a more flexible way
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBRESOURCE_H
#define XBRESOURCE_H



//-----------------------------------------------------------------------------
// Name: XBResource_SizeOf()
// Desc: Determines the byte size of a D3DResource
//-----------------------------------------------------------------------------
DWORD XBResource_SizeOf( LPDIRECT3DRESOURCE8 pResource );





//-----------------------------------------------------------------------------
// Name: struct XBRESOURCE
// Desc: Name tag for resources. An app may initialize this structure, and pass
//       it to the resource's Create() function. From then on, the app may call
//       GetResource() to retrieve a resource using an ascii name.
//-----------------------------------------------------------------------------
struct XBRESOURCE
{
    CHAR* strName;
    DWORD dwOffset;
};





//-----------------------------------------------------------------------------
// Name: class CXBPackedResource
// Desc: 
//-----------------------------------------------------------------------------
class CXBPackedResource
{
protected:
    BYTE*       m_pSysMemData;        // Alloc'ed memory for resource headers etc.
    DWORD       m_dwSysMemDataSize;

    BYTE*       m_pVidMemData;        // Alloc'ed memory for resource data, etc.
    DWORD       m_dwVidMemDataSize;
 
    XBRESOURCE* m_pResourceTags;     // Tags to associate names with the resources
    DWORD       m_dwNumResourceTags; // Number of resource tags

public:
    // Loads the resources out of the specified bundle
    HRESULT Create( const CHAR* strFilename, DWORD dwNumResourceTags = 0L, 
                    XBRESOURCE* pResourceTags = NULL );

    VOID Destroy();

    // Retrieves the resource tags
    HRESULT GetResourceTags( DWORD* pdwNumResourceTags, XBRESOURCE** ppResourceTags );

    // Helper function to make sure a resource is registered
    LPDIRECT3DRESOURCE8 RegisterResource( LPDIRECT3DRESOURCE8 pResource ) const
    {
        // Register the resource, if it has not yet been registered. We mark
        // a resource as registered by upping it's reference count.
        if( pResource && ( pResource->Common & D3DCOMMON_REFCOUNT_MASK ) == 1 )
        {
            // Special case CPU-copy push buffers (which live in system memory)
            if( ( pResource->Common & D3DCOMMON_TYPE_PUSHBUFFER ) &&
                ( pResource->Common & D3DPUSHBUFFER_RUN_USING_CPU_COPY ) )
                pResource->Data += (DWORD)m_pSysMemData;
            else
                pResource->Register( m_pVidMemData );

            pResource->AddRef();
        }
        return pResource;
    }

    // Functions to retrieve resources by their offset
    VOID* GetData( DWORD dwOffset ) const
    { return &m_pSysMemData[dwOffset]; }

    LPDIRECT3DRESOURCE8 GetResource( DWORD dwOffset ) const
    { return RegisterResource( (LPDIRECT3DRESOURCE8)GetData(dwOffset) ); }

    LPDIRECT3DTEXTURE8 GetTexture( DWORD dwOffset ) const
    { return (LPDIRECT3DTEXTURE8)GetResource( dwOffset ); }

    LPDIRECT3DCUBETEXTURE8 GetCubemap( DWORD dwOffset ) const
    { return (LPDIRECT3DCUBETEXTURE8)GetResource( dwOffset ); }

    LPDIRECT3DVOLUMETEXTURE8 GetVolumeTexture( DWORD dwOffset ) const
    { return (LPDIRECT3DVOLUMETEXTURE8)GetResource( dwOffset ); }

    LPDIRECT3DVERTEXBUFFER8 GetVertexBuffer( DWORD dwOffset ) const
    { return (LPDIRECT3DVERTEXBUFFER8)GetResource( dwOffset ); }

    LPDIRECT3DPUSHBUFFER8 GetPushBuffer( DWORD dwOffset ) const
    { return (LPDIRECT3DPUSHBUFFER8)GetResource( dwOffset ); }

    // Functions to retrieve resources by their name
    VOID* GetData( const CHAR* strName ) const;

    LPDIRECT3DRESOURCE8 GetResource( const CHAR* strName ) const
    { return RegisterResource( (LPDIRECT3DRESOURCE8)GetData( strName ) ); }

    LPDIRECT3DTEXTURE8 GetTexture( const CHAR* strName ) const
    { return (LPDIRECT3DTEXTURE8)GetResource( strName ); }

    LPDIRECT3DCUBETEXTURE8 GetCubemap( const CHAR* strName ) const
    { return (LPDIRECT3DCUBETEXTURE8)GetResource( strName ); }

    LPDIRECT3DVOLUMETEXTURE8 GetVolumeTexture( const CHAR* strName ) const
    { return (LPDIRECT3DVOLUMETEXTURE8)GetResource( strName ); }

    LPDIRECT3DVERTEXBUFFER8 GetVertexBuffer( const CHAR* strName ) const
    { return (LPDIRECT3DVERTEXBUFFER8)GetResource( strName ); }

    LPDIRECT3DPUSHBUFFER8 GetPushBuffer( const CHAR* strName ) const
    { return (LPDIRECT3DPUSHBUFFER8)GetResource( strName ); }

    // Constructor/destructor
    CXBPackedResource();
    ~CXBPackedResource();
};




#endif XBRESOURCE_H

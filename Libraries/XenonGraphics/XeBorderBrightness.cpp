#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeBorderBrightness.h"
#include "XeRenderStateManager.h"
#include "XeRenderTargetManager.h"
#include "XeShader.h"
#include "XeSharedDefines.h"

struct BorderVertex
{
    BorderVertex( float _x, float _y, float _z ) : x(_x), y(_y), z(_z){} 
    float x,y,z;
};

#define BORDER_VERTEX_STRIDE (sizeof(BorderVertex))
#define BORDER_ATTENUATION_SEGMENTS 16

// ------------------------------------------------------------------------------------------------
// Name   : XeBorderBrightness
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeBorderBrightness::XeBorderBrightness() :
XeAfterEffect           ( ),
m_poBorderRenderObject  ( NULL ),
m_poBorderMesh          ( NULL ),
m_poBorderIB            ( NULL ),
m_poMaterial            ( NULL )
{
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeBorderBrightness
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeBorderBrightness::~XeBorderBrightness()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBorderBrightness::ReInit()
{
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ulContext++)
    {
        m_fBorderBrightness[ulContext] = 0.0f;
        m_ulBorderColor[ulContext] = 0;
    }
 
    m_poBorderRenderObject    = new XeRenderObject();
    m_poBorderMesh            = new XeMesh();
    m_poMaterial              = new XeMaterial;
    m_poBorderRenderObject->SetMesh(m_poBorderMesh);
    m_poBorderRenderObject->SetMaterial(m_poMaterial);

    //
    // Build mesh 
    //
    ULONG ulNbVertex = (BORDER_ATTENUATION_SEGMENTS+1) * (BORDER_ATTENUATION_SEGMENTS+1);
    m_poBorderMesh->ClearAllStreams();
    m_poBorderMesh->AddStream( XEVC_POSITION, 0, NULL, ulNbVertex );

    XeBuffer * pBufferObject = m_poBorderMesh->GetStream(0)->pBuffer;
    BorderVertex * pBorderVertexBuffer = (BorderVertex*) pBufferObject->Lock( ulNbVertex, BORDER_VERTEX_STRIDE );

    float fY = fTop;
    float fIncrement = 2.0f / ((float)BORDER_ATTENUATION_SEGMENTS);
    for( ULONG i=0; i<BORDER_ATTENUATION_SEGMENTS+1; ++i )
    {
        float fX = fLeft;
        for( ULONG j=0; j<BORDER_ATTENUATION_SEGMENTS+1; ++j )
        {
            *(pBorderVertexBuffer++) = BorderVertex( fX, fY, 0 );
            fX += fIncrement;
        }
        fY += fIncrement;
    }

    pBufferObject->Unlock(TRUE);

    // Build index buffer
    ULONG ulNbrTri = BORDER_ATTENUATION_SEGMENTS * BORDER_ATTENUATION_SEGMENTS * 2;
    ULONG ulNbrIndex = ulNbrTri * 3;
    m_poBorderIB = g_XeBufferMgr.CreateIndexBuffer( ulNbrIndex );

    unsigned short* pIndices = ( unsigned short* )m_poBorderIB->Lock( ulNbrIndex );

    int CurrentIndex = 0;

    for(USHORT y = 0; y < BORDER_ATTENUATION_SEGMENTS; ++y)
    {
        for(USHORT x = 0; x < BORDER_ATTENUATION_SEGMENTS; ++x)
        {
            pIndices[CurrentIndex++] = y * (BORDER_ATTENUATION_SEGMENTS+1) + x;
            pIndices[CurrentIndex++] = (y+1) * (BORDER_ATTENUATION_SEGMENTS+1) + x;
            pIndices[CurrentIndex++] = (y+1) * (BORDER_ATTENUATION_SEGMENTS+1) + x + 1;

            pIndices[CurrentIndex++] = y * (BORDER_ATTENUATION_SEGMENTS+1) + x;
            pIndices[CurrentIndex++] = (y+1) * (BORDER_ATTENUATION_SEGMENTS+1) + x +1;
            pIndices[CurrentIndex++] = y * (BORDER_ATTENUATION_SEGMENTS+1) + x + 1;
        }
    }

    m_poBorderIB->Unlock();
    m_poBorderMesh->SetIndices( m_poBorderIB );
    m_poBorderRenderObject->SetPrimType(XeRenderObject::TriangleList);
    m_poBorderRenderObject->SetFaceCount(ulNbrTri);
    m_poBorderRenderObject->SetDrawMask( GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);

    // Create material
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetCustomVSFeature(9, 1 );
    m_poMaterial->SetCustomPSFeature(12, 1);
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA );
    
    m_poBorderRenderObject->SetMaterial( m_poMaterial );
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBorderBrightness::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBorderBrightness::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBorderBrightness::Shutdown()
{
    SAFE_DELETE(m_poBorderRenderObject);
    SAFE_DELETE(m_poBorderMesh);
    SAFE_DELETE(m_poMaterial);
    SAFE_RELEASE(m_poBorderIB);
}

// ------------------------------------------------------------------------------------------------
// Name   : Apply
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBorderBrightness::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    VECTOR4FLOAT vBorderBrightness;
    
    lTextureOut = lTextureIn; 
    if( m_fBorderBrightness[ulContext] <= 0.01f )
    {
        return;
    }

    float fBorderBrightness = m_fBorderBrightness[ulContext];
    fBorderBrightness = min( fBorderBrightness, 1.0f );

    XeJadeColorToV4F( &vBorderBrightness, m_ulBorderColor[ulContext] );
    vBorderBrightness.w = fBorderBrightness;
    g_oVertexShaderMgr.SetBorderBrightness( vBorderBrightness );
  
    g_oXeRenderer.RenderObject(m_poBorderRenderObject, XeFXManager::RP_DEFAULT);
 
    lTextureOut = XEBACKBUFFER_ID;
}

#include "precomp.h"

#include "XeHeatManager.h"
#include "XeTextureManager.h"
#include "XeRenderer.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"
#include "XeBufferMgr.h"
#include "XeIndexBuffer.h"
#include "XeShader.h"
#include "XePixelShaderManager.h"
#include <algorithm>

// ----- Global variables -----
HeatEffectManager g_oHeatManager;

#define HEAT_MAXDU  0.018f
#define HEAT_MAXDV  0.018f


// ----- Vertex structures -----

struct HeatVertex
{
    D3DXVECTOR3 v;
};

struct LessCenterCameraSpace
{
    bool operator()( const HeatEffectManager::ObjectInfo & _lhs, const HeatEffectManager::ObjectInfo & _rhs )
    {
        return (_lhs.m_vCenterInCameraSpace.z < _rhs.m_vCenterInCameraSpace.z );
    }
};

extern float TIM_gf_realdt;

//----------------------------------------------------
// HeatEffectManager::HeatEffectManager
//----------------------------------------------------
HeatEffectManager::HeatEffectManager() :
m_fAlphaScale           ( 0.8f ),
m_poOffsetRenderObject  ( NULL ),
m_poOffsetMesh          ( NULL ),
m_poOffsetMaterial      ( NULL ),
m_pOffsetIB             ( NULL ),
m_poMaskMaterial        ( NULL ),
m_poMaskIB              ( NULL ),
m_poBltRenderObject     ( NULL ),
m_poBltMaterial         ( NULL )
{
    memset( &m_oStateRender, 0, sizeof(HotAirState) );
    memset( &m_oStateEngine, 0, sizeof(HotAirState) );
}

//----------------------------------------------------
// HeatEffectManager::~HeatEffectManager
//----------------------------------------------------
HeatEffectManager::~HeatEffectManager()
{
	Shutdown();
}

//----------------------------------------------------
// HeatEffectManager::OnDeviceLost
//----------------------------------------------------
void 
HeatEffectManager::OnDeviceLost()
{
    Shutdown();
}

//----------------------------------------------------
// HeatEffectManager::OnDeviceReset
//----------------------------------------------------
void 
HeatEffectManager::OnDeviceReset()
{
    Init();
}

//----------------------------------------------------
// HeatEffectManager::Init
//----------------------------------------------------
void HeatEffectManager::Init( )
{
    InitMaskRenderTask();
    InitOffsetsRenderTask();
}

//----------------------------------------------------
// HeatEffectManager::Shutdown
//----------------------------------------------------
void HeatEffectManager::Shutdown( )
{
    // Clear masks
    SAFE_RELEASE(m_poMaskIB);
    SAFE_DELETE(m_poMaskMaterial);
    SAFE_DELETE(m_poMaskMesh);

    // Clear offsets
    SAFE_RELEASE(m_pOffsetIB );
    SAFE_DELETE(m_poOffsetRenderObject);
    SAFE_DELETE(m_poOffsetMesh);
    SAFE_DELETE(m_poOffsetMaterial);

    // Blit
    SAFE_DELETE(m_poBltRenderObject);
    SAFE_DELETE(m_poBltMaterial);
}

//----------------------------------------------------
// HeatEffectManager::AddHotAirPoint
//----------------------------------------------------
void HeatEffectManager::AddHotAirPoint( HotAirObject * pHotAirObject )
{
    if( m_oStateEngine.m_NumHotAirObjects < MAX_HOTAIROBJECTS )
    {
        ObjectInfo * pObjectInfo = SetupRender( pHotAirObject );
        if( pObjectInfo != NULL )
        {
            // Render the mask of the newly added object
            RenderMaskQuad( pObjectInfo );
        }
    }
}

//----------------------------------------------------
// HeatEffectManager::AddHotAirPointToCache
//----------------------------------------------------
void
HeatEffectManager::AddHotAirPointToCache( HotAirObject * pHotAirObject, LONG ulProjectionMatrixIndex )
{
    if( m_oStateRender.m_NumHotAirObjects < MAX_HOTAIROBJECTS )
    {
        ObjectInfo * pObjectInfo = SetupRender( pHotAirObject, TRUE, ulProjectionMatrixIndex );
        if( pObjectInfo != NULL )
        {
            // Queue the mask of the newly added object
            RenderMaskQuad( pObjectInfo, TRUE );
        }
    }
}

//----------------------------------------------------
// HeatEffectManager::RenderHotAirObjects
//----------------------------------------------------
void HeatEffectManager::RenderHotAirObjects()
{
    // Check if there are any hot air objects to render
    if( m_oStateRender.m_NumHotAirObjects == 0  || 
        g_pXeContextManagerRender->GetCurrentProjMatrixIndex() == 0 )
    {
        m_oStateRender.m_NumHotAirObjects = 0;
        return;
    }

    CXBBeginEventObject oEvent("HeatEffectManager::RenderHotAirObjects");

    // Render offsets
    RenderOffsetPass();
 
    // Done, reset counters
    m_oStateRender.m_NumHotAirObjects = 0;
    m_oStateRender.m_NumHotAirObjectsFromGrid = 0;
}

//----------------------------------------------------
// HeatEffectManager::InitOffsetsRenderTask
//----------------------------------------------------
void 
HeatEffectManager::InitOffsetsRenderTask( )
{
    ULONG ulNbSegments   = NUMHEATGRIDSEGMENTS;
    ULONG ulNumVertices  = (ulNbSegments + 1) * (ulNbSegments + 1);
    ULONG ulNumTriangles = ulNbSegments * ulNbSegments * 2;

    m_poOffsetRenderObject = new XeRenderObject();
    m_poOffsetMesh = new XeMesh();
    m_poOffsetMaterial = new XeMaterial();
    m_pOffsetIB = g_XeBufferMgr.CreateIndexBuffer( ulNumTriangles * 3 );

    m_poOffsetRenderObject->SetMesh( m_poOffsetMesh );
    m_poOffsetRenderObject->SetMaterial( m_poOffsetMaterial );
    m_poOffsetRenderObject->SetDrawMask( GDI_Cul_DM_ZTest | GDI_Cul_DM_UseTexture | GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotWired );
    m_poOffsetRenderObject->SetPrimType( XeRenderObject::TriangleList );
    
    // Build mesh
    m_poOffsetMesh->ClearAllStreams();
    m_poOffsetMesh->SetIndices( m_pOffsetIB );
    m_poOffsetMesh->AddStream( XEVC_POSITION, FALSE, NULL, ulNumVertices );

    //
    // Set the vertex data
    //
    ERR_X_Assert( m_poOffsetMesh != NULL );
    XeVertexStream * pStream = m_poOffsetMesh->GetStream(0);
    ERR_X_Assert( pStream != NULL && pStream->pBuffer != NULL );

    int iNumVertices = (NUMHEATGRIDSEGMENTS+1) * (NUMHEATGRIDSEGMENTS+1);

    HeatVertex* pVertices = (HeatVertex*) pStream->pBuffer->Lock( iNumVertices, sizeof( HeatVertex ) );

    int iCurrentvtx = 0;
    float f_CurY = -0.5f;
    float fSegmentSize = 1.0f / ((float)(NUMHEATGRIDSEGMENTS));
    for( int y = 0; y < NUMHEATGRIDSEGMENTS + 1; ++y )
    {
        float f_CurX  = -0.5f;
        for( int x = 0; x < NUMHEATGRIDSEGMENTS + 1; ++x )
        {
            pVertices[iCurrentvtx].v.x = f_CurX;
            pVertices[iCurrentvtx].v.y = f_CurY;

            // Compute alpha value
            float fAlpha1 = 1.0f - (2.0f*fabs(f_CurX));
            float fAlpha2 = 1.0f - (2.0f*fabs(f_CurY));
            pVertices[iCurrentvtx].v.z = fAlpha1 * fAlpha2 * m_fAlphaScale;

            f_CurX += fSegmentSize;
            ++iCurrentvtx;
        }

        f_CurY += fSegmentSize;
    }

    pStream->pBuffer->Unlock( true );

    // Calculate indices for grid mesh
    InitIndices( m_pOffsetIB, (USHORT)ulNbSegments );

    // Build material
    m_poOffsetMaterial->SetCustomPS( CUSTOM_PS_HEATSHIMMERING );
    m_poOffsetMaterial->SetCustomVS( CUSTOM_VS_HEATSHIMMERING );
    m_poOffsetMaterial->SetZState( TRUE, FALSE );
    m_poOffsetMaterial->SetAlphaBlend( TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
    m_poOffsetMaterial->SetCustomVSFeature( 1, HEAT_SHIMMER_OFFSET_PASS );
    m_poOffsetMaterial->SetCustomPSFeature( 1, HEAT_SHIMMER_OFFSET_PASS );
    
    // Noise stage
    m_poOffsetMaterial->AddTextureStage( );
    m_poOffsetMaterial->SetTextureId( 0, XEHEATSHIMMERNOISE_ID );
    m_poOffsetMaterial->SetAddressMode( 0, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP );
    m_poOffsetMaterial->SetFilterMode( 0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_NONE );

    // Back buffer stage
    m_poOffsetMaterial->AddTextureStage( );
    m_poOffsetMaterial->SetTextureId( 1, XESCRATCHBUFFER0_ID );
    m_poOffsetMaterial->SetAddressMode( 1, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP );
    m_poOffsetMaterial->SetFilterMode( 1, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_NONE );

    // Mask stage
    m_poOffsetMaterial->AddTextureStage( );
    m_poOffsetMaterial->SetTextureId( 2, XEHEATSHIMMERSCRATCH_ID );
    m_poOffsetMaterial->SetAddressMode( 2, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP );
    m_poOffsetMaterial->SetFilterMode( 2, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_NONE );

    // Alpha stage
    m_poOffsetMaterial->AddTextureStage( );
    m_poOffsetMaterial->SetTextureId( 3, XESCRATCHBUFFER1_ID );
    m_poOffsetMaterial->SetAddressMode( 3, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP );
    m_poOffsetMaterial->SetFilterMode( 3, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_NONE );

    // SPG2 Alpha
    m_poOffsetMaterial->AddTextureStage( );
    m_poOffsetMaterial->SetTextureId( 4,  XESPG2ALPHASCRATCH_ID );
    m_poOffsetMaterial->SetAddressMode( 4, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP );
    m_poOffsetMaterial->SetFilterMode( 4, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_NONE );

    //
    // Blit
    //
    m_poBltRenderObject = new XeRenderObject();
    m_poBltMaterial     = new XeMaterial();

    m_poBltRenderObject->SetMesh( g_oXeRenderer.GetQuadMesh() );
    m_poBltRenderObject->SetMaterial(m_poBltMaterial);

    // build material
    m_poBltMaterial->SetZState(FALSE, FALSE);
    m_poBltMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO);
    m_poBltMaterial->SetCustomPS(CUSTOM_PS_HEATSHIMMERING);
    m_poBltMaterial->SetCustomPSFeature(1, 3);
 
    m_poBltMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poBltMaterial->SetCustomVSFeature(3, 2);

    m_poBltMaterial->AddTextureStage( );
    m_poBltMaterial->SetFilterMode( 0, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT );
    m_poBltMaterial->SetTextureId( 0, XESCRATCHBUFFER0_ID );

    //m_poBltMaterial->AddTextureStage( );
    //m_poBltMaterial->SetFilterMode( 1, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT );
    //m_poBltMaterial->SetTextureId( 1, XESPG2ALPHASCRATCH_ID );

    // render flags
    m_poBltRenderObject->SetDrawMask(GDI_Cul_DM_UseTexture | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poBltRenderObject->SetPrimType(XeRenderObject::TriangleStrip);
}

//----------------------------------------------------
// HeatEffectManager::InitMaskRenderTask
//----------------------------------------------------
void    
HeatEffectManager::InitMaskRenderTask( )
{
    ULONG ulNbSegments   = 1;
    ULONG ulNumVertices  = (ulNbSegments + 1) * (ulNbSegments + 1);
    ULONG ulNumTriangles = ulNbSegments * ulNbSegments * 2;

    // Create indices
    m_poMaskIB = g_XeBufferMgr.CreateIndexBuffer( ulNumTriangles * 3 );
    InitIndices( m_poMaskIB, (USHORT)ulNbSegments );

    //
    // Create the shared mask mesh
    //
    m_poMaskMesh = new XeMesh;
    ERR_X_Assert( m_poMaskMesh != NULL );

    m_poMaskMesh->ClearAllStreams();
    m_poMaskMesh->SetIndices( m_poMaskIB );
    m_poMaskMesh->AddStream( XEVC_POSITION, FALSE, NULL, ulNumVertices );

    XeVertexStream * pStream = m_poMaskMesh->GetStream(0);
    ERR_X_Assert( pStream != NULL && pStream->pBuffer != NULL );

    HeatVertex * pVertices = (HeatVertex*) pStream->pBuffer->Lock( 4, sizeof( HeatVertex ) );

    int iCurrentvtx = 0;
    float f_CurY =  -0.5f;
    for( int y = 0; y < 2; ++y )
    {
        float f_CurX  = -0.5f;
        for( int x = 0; x < 2; ++x )
        {
            pVertices[iCurrentvtx].v.x = f_CurX;
            pVertices[iCurrentvtx].v.y = f_CurY;
            pVertices[iCurrentvtx].v.z = 0.0f;

            f_CurX += 1.0f;
            ++iCurrentvtx;
        }

        f_CurY += 1.0f;
    }

    pStream->pBuffer->Unlock( );

    // Create the mask material
    m_poMaskMaterial = new XeMaterial();
    m_poMaskMaterial->SetCustomPS( CUSTOM_PS_HEATSHIMMERING );
    m_poMaskMaterial->SetCustomVS( CUSTOM_VS_HEATSHIMMERING );
    m_poMaskMaterial->SetZState( TRUE, FALSE );
    m_poMaskMaterial->SetAlphaBlend( FALSE, D3DBLEND_ONE, D3DBLEND_ZERO );
    m_poMaskMaterial->SetColorWrite( FALSE );
    m_poMaskMaterial->SetAlphaWrite( TRUE );
    m_poMaskMaterial->SetCustomVSFeature( 1, 3 );
    m_poMaskMaterial->SetCustomPSFeature( 1, HEAT_SHIMMER_MASK_PASS );
}

//----------------------------------------------------
// HeatEffectManager::InitIndices
//----------------------------------------------------
void
HeatEffectManager::InitIndices( XeIndexBuffer * pIndexBuffer, USHORT usSegments )
{
    ERR_X_Assert( pIndexBuffer != NULL );

    int ulNumTriangles = 2 * usSegments * usSegments;

    unsigned short* pIndices = ( unsigned short* )pIndexBuffer->Lock( ulNumTriangles * 3 );

    int CurrentIndex = 0;

    for(USHORT y = 0; y < usSegments; ++y)
    {
        for(USHORT x = 0; x < usSegments; ++x)
        {
            pIndices[CurrentIndex++] = y * (usSegments+1) + x;
            pIndices[CurrentIndex++] = (y+1) * (usSegments+1) + x;
            pIndices[CurrentIndex++] = (y+1) * (usSegments+1) + x + 1;

            pIndices[CurrentIndex++] = y * (usSegments+1) + x;
            pIndices[CurrentIndex++] = (y+1) * (usSegments+1) + x +1;
            pIndices[CurrentIndex++] = y * (usSegments+1) + x + 1;
        }
    }

    pIndexBuffer->Unlock();
}

//----------------------------------------------------
// HeatEffectManager::SetupRender
//----------------------------------------------------
HeatEffectManager::ObjectInfo *
HeatEffectManager::SetupRender( HotAirObject * pHotAirObject, BOOL bFromGrid, LONG ulProjectionMatrixIndex )
{
    ObjectInfo * pCurrentObject = NULL;
    HotAirState * pState = &m_oStateEngine;

    if( bFromGrid )
    {
        pState = &m_oStateRender;
    }
    pCurrentObject = &pState->m_objectInfos[ pState->m_NumHotAirObjects ];
 
    // Check to see if too insignificant to be drawn
    if( (fabs(pHotAirObject->Height) < 0.000001f) ||
        (fabs(pHotAirObject->Width) < 0.000001f) ||
        (fabs(pHotAirObject->NoisePixelSize) < 0.000001f) )
    {
        return NULL;
    }

    // Compute scaling of effect due to distance of object from camera
    if( !ComputeDistanceScaling( pCurrentObject, pHotAirObject, bFromGrid ) )
    {
        return NULL;
    }

    if( !bFromGrid )
    {
        // Compute center of quad in camera space
        MATH_TransformVertex( &pCurrentObject->m_vCenterInCameraSpace,
                              &GDI_gpst_CurDD->st_Camera.st_InverseMatrix, 
                              &pHotAirObject->Position );
    }
    else
    {
        MATH_CopyVector( &pCurrentObject->m_vCenterInCameraSpace, &pHotAirObject->Position );
    }
     
    if( ulProjectionMatrixIndex == - 1 )
    {
        pCurrentObject->m_ulProjectionMatrixIndex = g_pXeContextManagerEngine->GetCurrentProjMatrixIndex();
    }
    else
    {
        pCurrentObject->m_ulProjectionMatrixIndex = ulProjectionMatrixIndex;
    }

    // Do the scrolling for original object
    ComputeNoiseUVParams( &pCurrentObject->m_fNoiseSizeU, 
                          &pCurrentObject->m_fNoiseSizeV, 
                          pHotAirObject );

    // Cache object
    pCurrentObject->m_object = *pHotAirObject;

    ++pState->m_NumHotAirObjects;
    if( bFromGrid )
    {
        ++pState->m_NumHotAirObjectsFromGrid;
    }

    return pCurrentObject;
}

//----------------------------------------------------
// HeatEffectManager::RenderMaskQuad
//----------------------------------------------------
void
HeatEffectManager::RenderMaskQuad( ObjectInfo * pObjectInfo, BOOL bFromGrid )
{
    ERR_X_Assert( pObjectInfo );

    HotAirObject * pHotAirObject = &pObjectInfo->m_object;
    ERR_X_Assert( pHotAirObject );

    tdstHeatShimmerVSConsts stHeatShimmerConsts;

    stHeatShimmerConsts.m_fWidth        = pHotAirObject->Width;
    stHeatShimmerConsts.m_fHeight       = pHotAirObject->Height;
    stHeatShimmerConsts.m_vCenter.x     = pObjectInfo->m_vCenterInCameraSpace.x;
    stHeatShimmerConsts.m_vCenter.y     = pObjectInfo->m_vCenterInCameraSpace.y;
    stHeatShimmerConsts.m_vCenter.z     = pObjectInfo->m_vCenterInCameraSpace.z;
    stHeatShimmerConsts.m_vCenter.w     = 1.0f;
    stHeatShimmerConsts.m_fBottomScale  = pHotAirObject->BottomScale;
    stHeatShimmerConsts.m_ulProjectionMatrixIndex = pObjectInfo->m_ulProjectionMatrixIndex;

    if( bFromGrid )
    {
        g_pXeContextManagerRender->SetHeatShimmerConsts( stHeatShimmerConsts );
    }
    else
    {
        g_pXeContextManagerEngine->SetHeatShimmerConsts( stHeatShimmerConsts );
    }

    ULONG ulExtraFlags = 0;
    if( bFromGrid )
    {
        ulExtraFlags |= QMFR_HEATSHIMMER_GRID;
    }
    
    // Queue it
    g_oXeRenderer.QueueMeshForRender( NULL,
                                      m_poMaskMesh,
                                      m_poMaskMaterial,
                                      (GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotWired | GDI_Cul_DM_ZTest),
                                      -1,
                                      XeRT_HEAT_SHIMMER_QUADS,
                                      XeRenderObject::TriangleList,
                                      0, 0, NULL,
                                      XeRenderObject::Common, 0, ulExtraFlags );
}


//----------------------------------------------------
// HeatEffectManager::RenderOffsetPass
//----------------------------------------------------
void
HeatEffectManager::RenderOffsetPass( )
{
     // Heat shimmer planes might overlap
    BOOL bUseAlphaTexure = (m_oStateRender.m_NumHotAirObjectsFromGrid > 0) && g_oXeRenderer.HasSPG2WriteAlpha( );

    if( bUseAlphaTexure )
    {
        SortPlanes( );
        g_oXeRenderer.ClearAlphaOnly( 0 );
      
        RenderOffsetsHelper( TRUE, FALSE );
    }
  
    g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID, NULL, FALSE, 0, bUseAlphaTexure, 1.0f );

    // Render heat shimmering planes
    RenderOffsetsHelper( FALSE, bUseAlphaTexure );
}

//----------------------------------------------------
// HeatEffectManager::RenderOffsetsHelper
//----------------------------------------------------
void    
HeatEffectManager::RenderOffsetsHelper( BOOL bAlphaOnly, BOOL bUseAlphaTexture )
{
#ifdef ACTIVE_EDITORS
    float fVPX, fVPY, fVPWidth, fVPHeight;
    g_oXeRenderer.GetNormalizedViewport( &fVPX, &fVPY, &fVPWidth, &fVPHeight );
#endif

    m_poOffsetMaterial->SetCustomPSFeature( 3, 0 );
    if( bAlphaOnly )
    {
        m_poOffsetMaterial->SetAlphaBlend( TRUE, D3DBLEND_ONE, D3DBLEND_ZERO );
        m_poOffsetMaterial->SetCustomVSFeature( 1, 2 );
        m_poOffsetMaterial->SetCustomPSFeature( 1, 3 );
        m_poOffsetMaterial->SetCustomPSFeature( 2, 0 );
        m_poOffsetMaterial->SetColorWrite( FALSE );
        m_poOffsetMaterial->SetAlphaWrite( TRUE );
        m_poOffsetMaterial->SetZState( TRUE, TRUE, D3DCMP_GREATER );
        g_oXeRenderer.ClearDepthStencil( 0.0f, 0 );
    }
    else
    {
        m_poOffsetMaterial->SetAlphaBlend( TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
        m_poOffsetMaterial->SetCustomVSFeature( 1, HEAT_SHIMMER_OFFSET_PASS );
        m_poOffsetMaterial->SetCustomPSFeature( 1, HEAT_SHIMMER_OFFSET_PASS );
        m_poOffsetMaterial->SetCustomPSFeature( 2, bUseAlphaTexture ? HEAT_SHIMMER_ALPHA_FROM_TEXTURE : 0 );

        static BOOL bDebug1 = FALSE;
        static BOOL bDebug2 = FALSE;
        if( bDebug1 )
        {
            m_poOffsetMaterial->SetCustomPSFeature( 3, 1 );
        }
        if( bDebug2 )
        {
            m_poOffsetMaterial->SetCustomPSFeature( 3, 2 );
        }

        m_poOffsetMaterial->SetColorWrite( TRUE );
        m_poOffsetMaterial->SetZState( TRUE, TRUE, D3DCMP_LESSEQUAL );
    }

    tdstHeatShimmerPSConsts heatPSConsts;
    heatPSConsts.m_vScale.z = 0.0f;
    heatPSConsts.m_vScale.w = 0.0f;

    tdstHeatShimmerVSConsts heatVSConsts;

    // Go through all hot air objects
    for(UINT iObjIdx = 0; iObjIdx < m_oStateRender.m_NumHotAirObjects; iObjIdx++)
    {
        HotAirObject * pHotAirObject = NULL;
        ObjectInfo * pObjectInfo = NULL;
   
        // front to back
        pObjectInfo = &m_oStateRender.m_objectInfos[iObjIdx];

        ERR_X_Assert( pObjectInfo );
        pHotAirObject = &pObjectInfo->m_object;

        //
        // Vertex shader consts
        //
        heatVSConsts.m_vCenter.x                = pObjectInfo->m_vCenterInCameraSpace.x;
        heatVSConsts.m_vCenter.y                = pObjectInfo->m_vCenterInCameraSpace.y;
        heatVSConsts.m_vCenter.z                = pObjectInfo->m_vCenterInCameraSpace.z;
        heatVSConsts.m_vCenter.w                = 1.0f;
        heatVSConsts.m_fWidth                   = pHotAirObject->Width;
        heatVSConsts.m_fHeight                  = pHotAirObject->Height;
        heatVSConsts.m_fBottomScale             = pHotAirObject->BottomScale;
        heatVSConsts.m_fIntensity               = pHotAirObject->Intensity;
        heatVSConsts.m_ulProjectionMatrixIndex  = pObjectInfo->m_ulProjectionMatrixIndex;
        heatVSConsts.m_fSizeU                   = pObjectInfo->m_fNoiseSizeU;
        heatVSConsts.m_fSizeV                   = pObjectInfo->m_fNoiseSizeV;
        heatVSConsts.m_fScrollV                 = pHotAirObject->Scroll;

        g_pXeContextManagerRender->SetHeatShimmerConsts( heatVSConsts );

        //
        // Pixel shader consts
        //
        heatPSConsts.m_vScale.x = heatPSConsts.m_vScale.y = pObjectInfo->m_fDistanceScaling;

#ifdef ACTIVE_EDITORS
        heatPSConsts.m_vScale.x *= fVPWidth;
        heatPSConsts.m_vScale.y *= fVPHeight;
#endif
        g_oPixelShaderMgr.SetHeatShimmerConsts( heatPSConsts );

        // Copy the projection matrix
        m_poOffsetRenderObject->SetProjMatrixIndex( pObjectInfo->m_ulProjectionMatrixIndex );

        // Draw the grid
        g_oXeRenderer.RenderObject( m_poOffsetRenderObject, XeFXManager::RP_DEFAULT );  
    }
}

//----------------------------------------------------
// HeatEffectManager::ComputeDistanceScaling
//----------------------------------------------------
bool
HeatEffectManager::ComputeDistanceScaling( ObjectInfo * pObjectInfo, HotAirObject * pHotAirObject, BOOL bPosInCameraSpace )
{
    ERR_X_Assert( pObjectInfo != NULL && pHotAirObject != NULL );

    // If distance from hot air object to camera is over half the range,
    // Do a linear attenuation on the displacement;
    float fDistanceToCamera = pHotAirObject->Position.z;

    if( !bPosInCameraSpace )
    {
        MATH_tdst_Vector CamSpaceHotAirPos;
        MATH_tdst_Vector CamPos = GDI_gpst_CurDD->st_Camera.st_Matrix.T;
        MATH_SubVector(&CamSpaceHotAirPos, &pHotAirObject->Position, &CamPos);

        fDistanceToCamera = MATH_f_NormVector( &CamSpaceHotAirPos );
    }

    // Skip hot air object if its distance to camera is greater than its range
    if( fDistanceToCamera >= pHotAirObject->Range )
    {
        return false;
    }

    float t = (pHotAirObject->Range - fDistanceToCamera) / pHotAirObject->Range;

    // Clamp 
    if(t < 0.0f)
    {
        t = 0.0f;
    }
    else if(t > 1.0f)
    {
        t = 1.0f;
    }

    pObjectInfo->m_fDistanceScaling = HEAT_MAXDU * t;
    return true;
}

//----------------------------------------------------
// HeatEffectManager::ComputeNoiseUVParams
//----------------------------------------------------
void
HeatEffectManager::ComputeNoiseUVParams( float *            pfSizeU, 
                                         float *            pfSizeV,
                                         HotAirObject *     pHotAirObject )
{
    ERR_X_Assert( pfSizeU != NULL && pfSizeV != NULL );

    *pfSizeU = pHotAirObject->Width / pHotAirObject->NoisePixelSize;
    *pfSizeV = pHotAirObject->Height / pHotAirObject->NoisePixelSize;

    // Compute offset
    float vOffset = (pHotAirObject->ScrollSpeed * TIM_gf_realdt * (*pfSizeV)) / pHotAirObject->Height;
    pHotAirObject->Scroll += vOffset;
}

//----------------------------------------------------
// HeatEffectManager::SortPlanes
//----------------------------------------------------
void  
HeatEffectManager::SortPlanes( )
{
    std::sort( m_oStateRender.m_objectInfos, m_oStateRender.m_objectInfos + m_oStateRender.m_NumHotAirObjects, LessCenterCameraSpace() );
}

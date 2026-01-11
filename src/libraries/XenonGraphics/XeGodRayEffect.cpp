#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeGodRayEffect.h"
#include "XeRenderStateManager.h"
#include "XeRenderTargetManager.h"
#include "XeShader.h"
#include "XeSharedDefines.h"
#include "XeContextManager.h"

struct GodRayVertex
{
    GodRayVertex( float _x, float _y, float _z, float _u, float _v ) : x(_x), y(_y), z(_z), u(_u), v(_v) {} 
    float x,y,z;
    float u,v;
};

struct BorderVertex
{
    BorderVertex( float _x, float _y, float _z, ULONG _color, float _u, float _v ) : x(_x), y(_y), z(_z), color(_color), u(_u), v(_v) {} 
    float x,y,z;
    ULONG color;
    float u,v;
};

#define GODRAY_VERTEX_STRIDE (sizeof(GodRayVertex))
#define BORDER_VERTEX_STRIDE (sizeof(BorderVertex))
#define GODRAY_ZOOM_RATIO    0.4f
#define GODRAY_ZOOM_STEP     0.5f
#define GODRAY_NEAR_ANGLE    60.0f
#define GODRAY_FAR_ANGLE     90.0f
#define GODRAY_HD_RATIO      0.50f
#define BORDER_ATTENUATION_SEGMENTS 16

// ------------------------------------------------------------------------------------------------
// Name   : XeGodRayEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeGodRayEffect::XeGodRayEffect() :
XeAfterEffect           ( ),
m_poRenderObject        ( NULL ),
m_poMesh                ( NULL ),
m_poMaterial            ( NULL ),
m_poBorderRenderObject  ( NULL ),
m_poBorderMesh          ( NULL ),
m_poBorderIB            ( NULL )
{
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeGodRayEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeGodRayEffect::~XeGodRayEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeGodRayEffect::ReInit()
{
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ulContext++)
    {
        m_fFactor[ulContext] = 0.0f;
        m_oLightDir[ulContext].x = m_oLightDir[ulContext].y = m_oLightDir[ulContext].z = 0.0f;
        m_oLightDir[ulContext].w = 1.0f;
    }

    // allocate containers
    m_poMaterial            = new XeMaterial();

    m_poRenderObject        = new XeRenderObject();
    m_poMesh                = new XeMesh();
    m_poRenderObject->SetMesh(m_poMesh);
    m_poRenderObject->SetMaterial(m_poMaterial);

    m_poBorderRenderObject    = new XeRenderObject();
    m_poBorderMesh            = new XeMesh();
    m_poBorderRenderObject->SetMesh(m_poBorderMesh);
    m_poBorderRenderObject->SetMaterial(m_poMaterial);

    //
    // Build mesh for godray blur
    //
    m_poMesh->ClearAllStreams();
    m_poMesh->AddStream(XEVC_POSITION|XEVC_TEXCOORD0, 0, NULL, 4);

    XeBuffer     *pBufferObject = m_poMesh->GetStream(0)->pBuffer;
    GodRayVertex *pVertexBuffer = (GodRayVertex*) pBufferObject->Lock(4, GODRAY_VERTEX_STRIDE);

    // fullscreen quad
    *(pVertexBuffer++) = GodRayVertex( fLeft,  fBottom, 0.0f, 0.0f, 0.0f );
    *(pVertexBuffer++) = GodRayVertex( fLeft,  fTop,    0.0f, 0.0f, 1.0f );
    *(pVertexBuffer++) = GodRayVertex( fRight, fBottom, 0.0f, 1.0f, 0.0f );
    *(pVertexBuffer++) = GodRayVertex( fRight, fTop,    0.0f, 1.0f, 1.0f );
    pBufferObject->Unlock(TRUE);

    //
    // Build mesh for border attenuation
    //
    ULONG ulNbVertex = (BORDER_ATTENUATION_SEGMENTS + 1) * (BORDER_ATTENUATION_SEGMENTS+1);
    m_poBorderMesh->ClearAllStreams();
    m_poBorderMesh->AddStream( XEVC_POSITION|XEVC_COLOR0|XEVC_TEXCOORD0, 0, NULL, ulNbVertex );

    pBufferObject = m_poBorderMesh->GetStream(0)->pBuffer;
    BorderVertex *pBorderVertexBuffer = (BorderVertex*) pBufferObject->Lock( ulNbVertex, BORDER_VERTEX_STRIDE );

    float fY = fTop;
    float fV = 1.0f;
    float fIncrement = 2.0f / 16.0f;
    for( ULONG i=0; i<BORDER_ATTENUATION_SEGMENTS+1; ++i )
    {
        float fX = fLeft;
        float fU = 0.0f;
        for( ULONG j=0; j<BORDER_ATTENUATION_SEGMENTS+1; ++j )
        {
            float fAlpha = 1.0f - (fX*fX + fY*fY);
            if( fAlpha < 0.0f )
            {
                fAlpha = 0.0f;
            }
            ULONG ulAlpha = (ULONG)(fAlpha * 255.0f);
            ULONG ulColor = ulAlpha | ulAlpha<<8 | ulAlpha<<16 | ulAlpha<<24;
            *(pBorderVertexBuffer++) = BorderVertex( fX, fY, 0.0f, ulColor, fU, fV );
            fX += fIncrement;
            fU += (fIncrement/2.0f);
        }
        fY += fIncrement;
        fV -= (fIncrement/2.0f);
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

    ///////////////////////////////// Zoom smooth ////////////////////////////////
    FLOAT fConvoleRevolution = 0.666666f;
    for (ULONG ulCurPass = 0; ulCurPass < GODRAY_STEP_COUNT; ulCurPass++)
    {
        m_passAlpha[ulCurPass] = 1.0f - fConvoleRevolution;
        fConvoleRevolution = (fConvoleRevolution - 0.5f) * 0.5f + 0.5f;
    }

    // build material
    m_poMaterial->AddTextureStage();
    m_poMaterial->AddTextureStage();

    m_poRenderObject->SetPrimType(XeRenderObject::TriangleStrip);
    m_poMesh->SetIndices(NULL);
    m_poRenderObject->SetFaceCount(2);
 
    m_poBorderRenderObject->SetPrimType(XeRenderObject::TriangleList);
    m_poBorderRenderObject->SetFaceCount(ulNbrTri);

    for( ULONG i=0; i<AE_CONTEXT_COUNT; ++i )
    {
        m_vAdjust[i].x = m_vAdjust[i].y = m_vAdjust[i].z = m_vAdjust[i].w = 1.0f;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeGodRayEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeGodRayEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeGodRayEffect::Shutdown()
{
    SAFE_DELETE(m_poRenderObject);
    SAFE_DELETE(m_poMesh);

    SAFE_DELETE(m_poBorderRenderObject);
    SAFE_DELETE(m_poBorderMesh);
    SAFE_RELEASE(m_poBorderIB);

    SAFE_DELETE(m_poMaterial);
}

// ------------------------------------------------------------------------------------------------
// Name   : Apply
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeGodRayEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    float               fX, fY, fCurrentFocale;
    MATH_tdst_Vector    oMotionSmoothPoint;
    XeBuffer            *pBufferObject = m_poMesh->GetStream(0)->pBuffer;
    FLOAT               fAttenuation, fAngle;

    // figure out the screen space position of the ray source
    fCurrentFocale = 1.0f / fNormalTan(g_pXeContextManagerRender->f_FieldOfVision / 2);		
    MATH_TransformVector(&oMotionSmoothPoint, &g_pXeContextManagerRender->st_InverseCameraMatrix , (MATH_tdst_Vector*)&m_oLightDir[ulContext]);
    fX = ((fCurrentFocale) * oMotionSmoothPoint.x) / oMotionSmoothPoint.z;
    fY = -((fCurrentFocale) * oMotionSmoothPoint.y) / oMotionSmoothPoint.z;

    // calculate angle in degrees between light direction and camera direction
    fAngle = fAcos(-oMotionSmoothPoint.z) / 3.1416f * 180.0f;
    fAngle = fAbs(fAngle);

    // calculate attenuation
    fAttenuation = 1.0f;
    if( fAngle >= GODRAY_NEAR_ANGLE && fAngle <= GODRAY_FAR_ANGLE )
    {
        fAttenuation = fCos((fAngle - GODRAY_NEAR_ANGLE) / (GODRAY_FAR_ANGLE - GODRAY_NEAR_ANGLE));
    }

    // smooth point is in out back, no rays
    if ( fAngle > GODRAY_FAR_ANGLE || (fAttenuation * m_fFactor[ulContext] <= 0.0f) ) 
    {
        if( g_pXeContextManagerRender->b_AntiAliasingBlur )
        {
            ApplyBlur(lTextureIn, lTextureOut);
        }
        else
        {
            lTextureOut = lTextureIn;
        }
        return;
    }
 
    // if the backbuffer is not already resolved, resolve it now
    if (lTextureIn != XESCRATCHBUFFER0_ID)
    {
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID );
        lTextureIn = XESCRATCHBUFFER0_ID;
    }

    // unbind stream 0
    g_oRenderStateMgr.UnbindStream(0);
     
    // Get the base UV (with no offset)
    float fBaseLeftU   = 0.0f;
    float fBaseRightU  = 1.0f;
    float fBaseBottomV = 0.0f;
    float fBaseTopV    = 1.0f;

#ifdef ACTIVE_EDITORS
    g_oXeRenderer.AdjustUVToViewport( fBaseLeftU, fBaseTopV ); 
    g_oXeRenderer.AdjustUVToViewport( fBaseRightU, fBaseBottomV ); 
#endif

    g_oXeRenderer.AddHalfViewportPixelOffset( fBaseLeftU, fBaseTopV );  
    g_oXeRenderer.AddHalfViewportPixelOffset( fBaseRightU, fBaseBottomV );  

    float fBaseWidthU   = fBaseRightU - fBaseLeftU;
    float fBaseHeightV  = fBaseTopV - fBaseBottomV;

    float fPassLeftU   = 0.0f;
    float fPassRightU  = 1.0f;   
    float fPassBottomV = 0.0f;
    float fPassTopV    = 1.0f;
      
    ///////////////////////////////// Zoom smooth ////////////////////////////////
    FLOAT fFactor = (oMotionSmoothPoint.z < 0.0f) ? GODRAY_ZOOM_RATIO : -GODRAY_ZOOM_RATIO;
    for (ULONG ulCurPass = 0; ulCurPass < GODRAY_STEP_COUNT; ulCurPass++)
    {
        // Compute the positions of vertices if we were drawing a quad for this pass
        float fPassLeft    = (fLeft - fX)   * (fFactor + 1.0f) + fX;
        float fPassRight   = (fRight - fX)  * (fFactor + 1.0f) + fX;
        float fPassTop     = (fTop - fY)    * (fFactor + 1.0f) + fY;
        float fPassBottom  = (fBottom - fY) * (fFactor + 1.0f) + fY;

        float fPassWidth  = fPassRight - fPassLeft;
        float fPassHeight = fPassBottom - fPassTop;

        // Compute the uvs for this pass
        fPassLeftU    = 0.0f + (fLeft - fPassLeft) / fPassWidth;
        fPassRightU   = 1.0f + (fRight - fPassRight) / fPassWidth;
        fPassTopV     = 1.0f - (fTop - fPassTop) / fPassHeight;
        fPassBottomV  = 0.0f - (fBottom - fPassBottom) / fPassHeight;

        // Adjust pass coords
#ifdef ACTIVE_EDITORS
        g_oXeRenderer.AdjustUVToViewport( fPassLeftU, fPassTopV ); 
        g_oXeRenderer.AdjustUVToViewport( fPassRightU, fPassBottomV ); 
#endif

        // Compute the transformation matrix from quad uv to this pass uv
        float fPassWidthU   = fPassRightU - fPassLeftU;
        float fPassHeightV  = fPassTopV - fPassBottomV;

        // Pass uv transform M = [pass translate] * [scale] * [translate to origin]
        m_passTransform[ulCurPass].m_fOffsetU = ((fPassWidthU/fBaseWidthU) * (-fBaseLeftU)) + fPassLeftU ;
        m_passTransform[ulCurPass].m_fScaleU  = (fPassWidthU/fBaseWidthU);

        m_passTransform[ulCurPass].m_fOffsetV = ((fPassHeightV/ fBaseHeightV) * (-fBaseBottomV)) + fPassBottomV;
        m_passTransform[ulCurPass].m_fScaleV  = (fPassHeightV/fBaseHeightV);

        fFactor *= GODRAY_ZOOM_STEP;
    }

    //
    // Downscale sky mask if necessary
    //
    BOOL b_ScaleToBlur = g_oXeRenderer.IsHiDef();

#ifdef VIDEOCONSOLE_ENABLE
    if(NoGodRayLowRes)
    {
        b_ScaleToBlur = FALSE;
    }
#endif

    float fInputRatio  = 1.0f;
    float fOutputRatio = b_ScaleToBlur ? GODRAY_HD_RATIO : 1.0f;

    D3DRECT rect;
    if( b_ScaleToBlur )
    {
        // Build the rect that represents the desired portion of backbuffer to resolve
        rect.x1 =  rect.y1 = 0;
        UINT uiWidth, uiHeight;
        g_oXeRenderer.GetBackbufferResolution( &uiWidth, &uiHeight );
        uiWidth = (UINT) (((float)uiWidth) * fOutputRatio);
        uiHeight= (UINT) (((float)uiHeight) * fOutputRatio);

        // Make multiple of 8 
        if( uiWidth & 7 )
        {
            uiWidth = (uiWidth & ~7) + 8;
        }
        if( uiHeight & 7 )
        {
            uiHeight = (uiHeight & ~7) + 8;
        }
        rect.x2 = uiWidth;
        rect.y2 = uiHeight;
    }


//#ifdef VIDEOCONSOLE_ENABLE
//    if(b_ScaleToBlur)
//    {
//        fOutputRatio = GodRayRatio;
//    }
//#endif

    //
    // Mask sky 
    //
    g_oVertexShaderMgr.SetZoomBlurRatios( 1.0f, 1.0f );
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetCustomVSFeature(1, GODRAY_SHADER_1 );
    m_poMaterial->SetCustomPSFeature(1, GODRAY_SHADER_1);
    m_poMaterial->SetZState(TRUE, FALSE, D3DCMP_GREATER);
    m_poMaterial->SetAlphaBlend(FALSE);
    m_poMaterial->SetAddressMode( 0, D3DTADDRESS_BORDER, D3DTADDRESS_BORDER, D3DTADDRESS_CLAMP );
    m_poMaterial->SetBorderColor(0,0);
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_ZTest | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
 
    g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER1_ID, NULL, b_ScaleToBlur );

    //
    // Blur sky mask
    //
    g_oVertexShaderMgr.SetZoomBlurRatios( fInputRatio, fOutputRatio );
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetCustomVSFeature(1, GODRAY_SHADER_2);
    m_poMaterial->SetCustomPSFeature(1, GODRAY_SHADER_2);
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetTextureId(0, XESCRATCHBUFFER1_ID);
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_UseTexture | GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poBorderRenderObject->SetDrawMask( GDI_Cul_DM_UseTexture | GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor );

    for( int ulCurPass = 0; ulCurPass < 3; ulCurPass++)
    {
        if( ulCurPass == 0 )
        {
            g_oVertexShaderMgr.SetGodRayAdjust( m_vAdjust[ulContext] );
            m_poMaterial->SetCustomVSFeature(1, 6);
            m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO);
            m_poMaterial->SetCustomPSFeature(1, GODRAY_SHADER_5);
        }
        else
        {
            m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_SRCALPHA);
            m_poMaterial->SetCustomPSFeature(1, GODRAY_SHADER_2 );
            m_poMaterial->SetCustomVSFeature(1, GODRAY_SHADER_2 );
        }

        // Setup the pixel shader constants
        for( int i=0; i<2; ++i )
        {
            int iStep = ulCurPass*2+i; 

            g_oVertexShaderMgr.SetZoomBlurStepConsts( i, m_passAlpha[iStep],
                                                      m_passTransform[iStep].m_fScaleU,
                                                      m_passTransform[iStep].m_fOffsetU,
                                                      m_passTransform[iStep].m_fScaleV,
                                                      m_passTransform[iStep].m_fOffsetV );
        }

        g_oXeRenderer.RenderObject( (ulCurPass == 0) ? m_poBorderRenderObject : m_poRenderObject, XeFXManager::RP_DEFAULT ); 
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER1_ID, (ulCurPass == 0 || !b_ScaleToBlur) ? NULL : &rect );

        fInputRatio = fOutputRatio;
        g_oVertexShaderMgr.SetZoomBlurRatios( fInputRatio, fOutputRatio );
   }

    // Setup last blur pass 
    g_oVertexShaderMgr.SetZoomBlurStepConsts( 0, m_passAlpha[6],
                                              m_passTransform[6].m_fScaleU,
                                              m_passTransform[6].m_fOffsetU,
                                              m_passTransform[6].m_fScaleV,
                                              m_passTransform[6].m_fOffsetV );

    if( b_ScaleToBlur )
    {
        //
        // Combine scaled blurred mask with original frame
        //
        g_oVertexShaderMgr.SetZoomBlurRatios( fInputRatio, 1.0f );

        float fResultAlpha = m_fFactor[ulContext] * fAttenuation;
        g_oVertexShaderMgr.SetZoomBlurStepConsts( 1, fResultAlpha, 0.0f, 0.0f, 0.0f, 0.0f );

        m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
        m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
        m_poMaterial->SetCustomVSFeature( 1, GODRAY_SHADER_5 );
        m_poMaterial->SetCustomPSFeature( 1, GODRAY_SHADER_4 );
        m_poMaterial->SetTextureId( 1, XESCRATCHBUFFER0_ID );
        m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO );

        g_oVertexShaderMgr.IgnoreHalfViewportTexel(g_pXeContextManagerRender->b_AntiAliasingBlur);
        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT); 
        g_oVertexShaderMgr.IgnoreHalfViewportTexel(FALSE);
    }
    else
    {
        float fResultAlpha = m_fFactor[ulContext] * fAttenuation;
        g_oVertexShaderMgr.SetZoomBlurStepConsts( 1, fResultAlpha, 0.0f, 0.0f, 0.0f, 0.0f );

        // update data offset for final pass
        g_oVertexShaderMgr.SetZoomBlurRatios( 1.0f, 1.0f );

        m_poMaterial->SetCustomVSFeature(1, GODRAY_SHADER_3);
        m_poMaterial->SetCustomPSFeature(1, GODRAY_SHADER_3);
        m_poMaterial->SetTextureId(0, XESCRATCHBUFFER1_ID);
        m_poMaterial->SetTextureId(1, XESCRATCHBUFFER0_ID);
        m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_SRCALPHA);
 
        // blend result and original frame buffer (blur if needed)
        g_oVertexShaderMgr.IgnoreHalfViewportTexel(g_pXeContextManagerRender->b_AntiAliasingBlur);
        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
        g_oVertexShaderMgr.IgnoreHalfViewportTexel(FALSE);
    }

    // result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}

void XeGodRayEffect::ApplyBlur( LONG lTextureIn, LONG &lTextureOut )
{
    if (lTextureIn != XESCRATCHBUFFER0_ID)
    {
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID );
        lTextureIn = XESCRATCHBUFFER0_ID;
    }
    
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);

    // setup blur pass
    m_poMaterial->SetTextureId(0, XESCRATCHBUFFER0_ID);
    m_poMaterial->SetCustomVSFeature(3, BLIT_SHADER_1 );
    m_poMaterial->SetCustomPSFeature(6, BLIT_SHADER_1 );
    m_poMaterial->SetZState(FALSE, FALSE, D3DCMP_GREATEREQUAL);
    m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO );
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_UseTexture | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poMaterial->SetAddressMode( 0, D3DTADDRESS_BORDER, D3DTADDRESS_BORDER, D3DTADDRESS_CLAMP );
    g_oVertexShaderMgr.IgnoreHalfViewportTexel(TRUE);

    // render blur
    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
    g_oVertexShaderMgr.IgnoreHalfViewportTexel(FALSE);

    m_poMaterial->SetCustomVSFeature(3, 0);
    m_poMaterial->SetCustomPSFeature(6, 0);

    lTextureOut = XEBACKBUFFER_ID;
}

void 
XeGodRayEffect::SetIntensity( ULONG _ulContext, float _fIntensity, ULONG _color )
{
    XeJadeColorToV4F( &m_vAdjust[_ulContext], _color );
    m_vAdjust[_ulContext].x *= _fIntensity;
    m_vAdjust[_ulContext].y *= _fIntensity;
    m_vAdjust[_ulContext].z *= _fIntensity;
}


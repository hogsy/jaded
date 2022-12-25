#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeZoomSmoothCenterEffect.h"
#include "XeRenderStateManager.h"
#include "XeRenderTargetManager.h"
#include "XeShader.h"
#include "XeSharedDefines.h"
#include "XeContextManager.h"

struct ZoomSmoothVertex
{
    float x,y,z;
    float u,v;
};

#define ZSC_VERTEX_STRIDE (sizeof(ZoomSmoothVertex))
#define ZSC_ZOOM_STEP     0.5f
#define ZSC_AMPLITUDE     0.2f*0.1f
#define ZSC_HD_RATIO      0.7f

// ------------------------------------------------------------------------------------------------
// Name   : XeZoomSmoothCenterEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeZoomSmoothCenterEffect::XeZoomSmoothCenterEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMesh            = NULL;
    m_poMaterial        = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeZoomSmoothCenterEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeZoomSmoothCenterEffect::~XeZoomSmoothCenterEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeZoomSmoothCenterEffect::ReInit()
{ 
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ulContext++)
    {
        m_fFactor[ulContext] = 0.0f;
        m_oSmoothDir[ulContext].x = m_oSmoothDir[ulContext].y = m_oSmoothDir[ulContext].z = 0.0f;
        m_oSmoothDir[ulContext].w = 1.0f;
    }

    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMesh         = new XeMesh();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh(m_poMesh);
    m_poRenderObject->SetMaterial(m_poMaterial);

    // build mesh
    m_poMesh->ClearAllStreams();
    m_poMesh->AddStream(XEVC_POSITION|XEVC_TEXCOORD0, 0, NULL, 4 );

    XeBuffer     *pBufferObject = m_poMesh->GetStream(0)->pBuffer;
    ZoomSmoothVertex *pVertexBuffer = (ZoomSmoothVertex*) pBufferObject->Lock(4, ZSC_VERTEX_STRIDE);

    // fullscreen quad
    pVertexBuffer->x = fLeft;
    pVertexBuffer->y = fBottom;
    pVertexBuffer->z = 0.999f;
    pVertexBuffer->u = 0.0f;
    pVertexBuffer->v = 0.0f;
    pVertexBuffer++;

    pVertexBuffer->x = fLeft;
    pVertexBuffer->y = fTop;
    pVertexBuffer->z = 0.999f;
    pVertexBuffer->u = 0.0f;
    pVertexBuffer->v = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x = fRight;
    pVertexBuffer->y = fBottom;
    pVertexBuffer->z = 0.999f;
    pVertexBuffer->u = 1.0f;
    pVertexBuffer->v = 0.0f;
    pVertexBuffer++;

    pVertexBuffer->x = fRight;
    pVertexBuffer->y = fTop;
    pVertexBuffer->z = 0.999f;
    pVertexBuffer->u = 1.0f;
    pVertexBuffer->v = 1.0f;
    pVertexBuffer++;
    pBufferObject->Unlock(TRUE);

    ///////////////////////////////// Zoom smooth ////////////////////////////////
    FLOAT fConvoleRevolution = 0.666666f;
    for (ULONG ulCurPass = 0; ulCurPass < ZSC_STEP_COUNT; ulCurPass++)
    {
        m_passAlpha[ulCurPass] = 1.0f - fConvoleRevolution;
        fConvoleRevolution = (fConvoleRevolution - 0.5f) * 0.5f + 0.5f;
    }

    // setup ZoomSmooth passes

    // build material
    m_poMaterial->AddTextureStage();
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetTextureId(0, XESCRATCHBUFFER0_ID);

    // setup mesh
    m_poMesh->SetIndices(NULL);
  
    // setup render object
    m_poRenderObject->SetPrimType(XeRenderObject::TriangleStrip);
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_UseTexture | GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeZoomSmoothCenterEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeZoomSmoothCenterEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeZoomSmoothCenterEffect::Shutdown()
{
    SAFE_DELETE(m_poRenderObject);
    SAFE_DELETE(m_poMesh);
    SAFE_DELETE(m_poMaterial);
}

// ------------------------------------------------------------------------------------------------
// Name   : Apply
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeZoomSmoothCenterEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    FLOAT               fX, fY, fCurrentFocale, fFactor;
    MATH_tdst_Vector    oMotionSmoothPoint, oNormalizedSmoothDir;
    XeBuffer            *pBufferObject = m_poMesh->GetStream(0)->pBuffer;

    if( m_fFactor[ulContext] <= 0.0f )
    {
        lTextureOut = lTextureIn;
        return;
    }
 
    BOOL bExecute = TRUE;

    if( m_abDirValidated[ulContext] )
    {
        if( MATH_b_NulVectorWithEpsilon( (MATH_tdst_Vector*)&m_oSmoothDir[ulContext], Cf_Epsilon) )
        {
            bExecute = FALSE;
        }
        else
        {
            // figure out the screen space position of the ray source
            fCurrentFocale = 1.0f / fNormalTan(g_pXeContextManagerRender->f_FieldOfVision / 2);	

            float fDirectionNorm = MATH_f_NormVector( (MATH_tdst_Vector*)&m_oSmoothDir[ulContext] );

            MATH_NormalizeVector(&oNormalizedSmoothDir, (MATH_tdst_Vector*)&m_oSmoothDir[ulContext]);
            MATH_TransformVector(&oMotionSmoothPoint, &g_pXeContextManagerRender->st_InverseCameraMatrix , &oNormalizedSmoothDir);

            if (oMotionSmoothPoint.z <= 0.0f) 
            {
                bExecute = FALSE;
            }
            else
            {
                fX = ((fCurrentFocale) * oMotionSmoothPoint.x) / oMotionSmoothPoint.z;
                fY = -((fCurrentFocale) * oMotionSmoothPoint.y) / oMotionSmoothPoint.z;

                // calculate zoom factor
                fFactor = m_fFactor[ulContext] * ZSC_AMPLITUDE * oMotionSmoothPoint.z * oMotionSmoothPoint.z;
            }
        }
    }
    else
    {
        fX = 0.0f;
        fY = 0.0f;
        fFactor = m_fFactor[ulContext] * ZSC_AMPLITUDE;
    }
 
    if( !bExecute )
    {
        lTextureOut = lTextureIn;
        return;
    }

    // unbind stream 0
    g_oRenderStateMgr.UnbindStream(0);

    // Get the base UV (with no offset)
    float fBaseLeftU     = 0.0f;
    float fBaseRightU    = 1.0f;
    float fBaseBottomV   = 0.0f;
    float fBaseTopV      = 1.0f;

#ifdef ACTIVE_EDITORS
    g_oXeRenderer.AdjustUVToViewport( fBaseLeftU, fBaseTopV ); 
    g_oXeRenderer.AdjustUVToViewport( fBaseRightU, fBaseBottomV ); 
#endif

    g_oXeRenderer.AddHalfViewportPixelOffset( fBaseLeftU, fBaseTopV );  
    g_oXeRenderer.AddHalfViewportPixelOffset( fBaseRightU, fBaseBottomV );  

    float fBaseWidthU   = fBaseRightU - fBaseLeftU;
    float fBaseHeightV  = fBaseTopV - fBaseBottomV;

    float fPassLeftU    = 0.0f;
    float fPassRightU   = 1.0f;
    float fPassTopV     = 0.0f;
    float fPassBottomV  = 1.0f;

    ///////////////////////////////// Zoom smooth ////////////////////////////////
    for (ULONG ulCurPass = 0; ulCurPass < ZSC_STEP_COUNT; ulCurPass++)
    {
        // Compute the positions of vertices if we were drawing a quad for this pass
        float fPassLeft    = (fLeft - fX) * (fFactor + 1.0f) + fX;
        float fPassRight   = (fRight - fX) * (fFactor + 1.0f) + fX;
        float fPassTop     = (fTop - fY) * (fFactor + 1.0f) + fY;
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

        fFactor *= ZSC_ZOOM_STEP;
    }

    BOOL b_ScaleToBlur = g_oXeRenderer.IsHiDef();

    float fInputRatio  = 1.0f;
    float fOutputRatio = b_ScaleToBlur ? ZSC_HD_RATIO : 1.0f;

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

    
    if( lTextureIn != XESCRATCHBUFFER0_ID )
    {
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID, NULL, b_ScaleToBlur );
        lTextureIn = XESCRATCHBUFFER0_ID;
    }

    //
    // Now do the blur
    //
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetCustomVSFeature(1, GODRAY_SHADER_2);

    g_oVertexShaderMgr.SetZoomBlurRatios( fInputRatio, fOutputRatio );

    for( int ulCurPass = 0; ulCurPass < 3; ulCurPass++)
    {
        if( ulCurPass == 0 && b_ScaleToBlur )
        {
            m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO);
            m_poMaterial->SetCustomPSFeature(1, GODRAY_SHADER_5);
        }
        else
        {
            m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_SRCALPHA);
            m_poMaterial->SetCustomPSFeature(1, GODRAY_SHADER_2);
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

        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);  
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID, (ulCurPass == 0 || !b_ScaleToBlur) ? NULL : &rect );
        lTextureOut = XEBACKBUFFER_ID;
        fInputRatio = fOutputRatio;
        g_oVertexShaderMgr.SetZoomBlurRatios( fInputRatio, fOutputRatio );
    }
   

    // Final blur pass
    g_oVertexShaderMgr.SetZoomBlurStepConsts( 0, m_passAlpha[6],
                                              m_passTransform[6].m_fScaleU,
                                              m_passTransform[6].m_fOffsetU,
                                              m_passTransform[6].m_fScaleV,
                                              m_passTransform[6].m_fOffsetV );
    g_oVertexShaderMgr.SetZoomBlurRatios( fInputRatio, 1.0f );


    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);

    if( b_ScaleToBlur )
    {
        m_poMaterial->SetCustomVSFeature( 1, GODRAY_SHADER_3 );
        m_poMaterial->SetCustomPSFeature( 3, ZOOM_SMOOTH_SHADER_2);
        m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO );
    }
    else
    {
        m_poMaterial->SetCustomVSFeature( 1, GODRAY_SHADER_3 );
        m_poMaterial->SetCustomPSFeature( 3, ZOOM_SMOOTH_SHADER_1);
        m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_SRCALPHA );
    }

    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT); 

    // result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}
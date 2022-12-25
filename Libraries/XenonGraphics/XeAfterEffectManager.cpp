#include "Precomp.h"
#include "XeAfterEffectManager.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeGodRayEffect.h"
#include "XeMotionBlurEffect.h"
#include "XeZoomSmoothCenterEffect.h"
#include "XeColorBalanceEffect.h"
#include "XeBrightnessContrastEffect.h"
#include "XeRemananceEffect.h"
#include "XeBlackWhiteEffect.h"
#include "XeFogEffect.h"
#include "XeRenderStateManager.h"
#include "XePixelShaderManager.h"
#include "XeWideScreenEffect.h"
#include "XeColorDiffusion.h"
#include "XeBigBlurEffect.h"
#include "XeLightningAE.h"
#include "XeSafeFrameEffect.h"
#include "XeBKQuadAE.h"
#include "XeXInvertEffect.h"
#include "XeOldMovieEffect.h"
#include "XeContextManager.h"
#include "XeBorderBrightness.h"
#include "XeSpinSmoothEffect.h"
#include "XeColorCorrectionEffect.h"

// single instance of the XeAfterEffectManager
XeAfterEffectManager g_oAfterEffectManager;
XeAfterEffectManager* XeAfterEffect::m_poAfterEffectManager;

// ------------------------------------------------------------------------------------------------
// Name   : XeAfterEffectManager
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeAfterEffect::XeAfterEffect()
{ 
    m_pD3DDevice = g_oXeRenderer.GetDevice();
}


// ------------------------------------------------------------------------------------------------
// Name   : Enable
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeAfterEffect::Enable(ULONG ulContext, BOOL bEnable) 
{
    ERR_X_Assert( ulContext < AE_CONTEXT_COUNT );
    m_bEnabled[ulContext] = bEnable; 
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------

void XeAfterEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext) 
{
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------

void XeAfterEffect::DoCopyContext(ULONG _ulSrcContext, ULONG _ulDstContext)
{
    m_bEnabled[_ulDstContext] = m_bEnabled[_ulSrcContext]; 

    CopyContext(_ulSrcContext, _ulDstContext);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeAfterEffectManager
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeAfterEffectManager::XeAfterEffectManager()
{
    L_memset(m_apoEffects, 0, MAX_AFTER_EFFECTS * sizeof(XeAfterEffect*));
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeAfterEffectManager
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeAfterEffectManager::~XeAfterEffectManager()
{
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeAfterEffectManager::ReInit()
{
    m_pD3DDevice = g_oXeRenderer.GetDevice();
    XeAfterEffect::m_poAfterEffectManager = this;
    L_memset(m_apoEffects, 0, MAX_AFTER_EFFECTS * sizeof(XeAfterEffect*));

    m_ulWriteContext    = 0;
    m_ulReadContext     = 1;

    // init effects
    
    m_apoEffects[AE_BRIGHTNESS] = new XeBrightnessContrastEffect();
    m_apoEffects[AE_CONTRAST]   = m_apoEffects[AE_BRIGHTNESS];
    m_apoEffects[AE_BRIGHTNESS]->ReInit();

    // Brightness/Contrast used by scripted events only on Xenon.
    // An other instance of the AE is used so there is no confusion with the global  
    // contrast / brightness settings used on ps2 which we discard
    m_apoEffects[AE_BRIGHTNESS_XE] = new XeBrightnessContrastEffect();
    m_apoEffects[AE_CONTRAST_XE]   = m_apoEffects[AE_BRIGHTNESS_XE];
    m_apoEffects[AE_BRIGHTNESS_XE]->ReInit();

    m_apoEffects[AE_MOTIONBLUR] = new XeMotionBlurEffect();
    m_apoEffects[AE_MOTIONBLUR]->ReInit();

    m_apoEffects[AE_COLORBALANCE] = new XeColorBalanceEffect();
    m_apoEffects[AE_COLORBALANCE]->ReInit();

    m_apoEffects[AE_GODRAY] = new XeGodRayEffect();
    m_apoEffects[AE_GODRAY]->ReInit();

    m_apoEffects[AE_REMANANCE] = new XeRemananceEffect();
    m_apoEffects[AE_REMANANCE]->ReInit();

    m_apoEffects[AE_ZOOMSMOOTHCENTER] = new XeZoomSmoothCenterEffect();
    m_apoEffects[AE_ZOOMSMOOTHCENTER]->ReInit();


    m_apoEffects[AE_SPINSMOOTH] = new XeSpinSmoothEffect();
    m_apoEffects[AE_SPINSMOOTH]->ReInit();

    m_apoEffects[AE_BLACKWHITE] = new XeBlackWhiteEffect();
    m_apoEffects[AE_BLACKWHITE]->ReInit();

  #if defined(_XENON) && !defined(_FINAL_)
    m_apoEffects[AE_SAFEFRAME] = new XeSafeFrameEffect();
    m_apoEffects[AE_SAFEFRAME]->ReInit();
  #endif

    m_apoEffects[AE_FOG] = new XeFogEffect();
    m_apoEffects[AE_FOG]->ReInit();

    m_apoEffects[AE_WIDESCREEN] = new XeWideScreenEffect();
    m_apoEffects[AE_WIDESCREEN]->ReInit();

    m_apoEffects[AE_COLORDIFFUSION] = new XeColorDiffusionEffect();
    m_apoEffects[AE_COLORDIFFUSION]->ReInit();

    m_apoEffects[AE_BIGBLUR] = new XeBigBlurEffect();
    m_apoEffects[AE_BIGBLUR]->ReInit();

    m_apoEffects[AE_LIGHTNING_XE] = new XeLightningAE();
    m_apoEffects[AE_LIGHTNING_XE]->ReInit();

    m_apoEffects[AE_BKQUAD_XE] = new XeBKQuadAE();
    m_apoEffects[AE_BKQUAD_XE]->ReInit();

    m_apoEffects[AE_XINVERT] = new XeXInvertEffect();
    m_apoEffects[AE_XINVERT]->ReInit();

    m_apoEffects[AE_BORDERBRIGHTNESS] = new XeBorderBrightness();
    m_apoEffects[AE_BORDERBRIGHTNESS]->ReInit();

    m_apoEffects[AE_OLDMOVIE] = new XeOldMovieEffect();
    m_apoEffects[AE_OLDMOVIE]->ReInit();

    m_apoEffects[AE_COLORCORRECTION] = new XeColorCorrectionEffect();
    m_apoEffects[AE_COLORCORRECTION]->ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeAfterEffectManager::OnDeviceLost()
{
    for (ULONG i = 0; i < MAX_AFTER_EFFECTS; ++i)
    {
        if ((i == AE_CONTRAST) || (i == AE_CONTRAST_XE))
            continue;

        if (m_apoEffects[i] != NULL)
        {
            m_apoEffects[i]->OnDeviceLost();
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeAfterEffectManager::OnDeviceReset()
{
    for (ULONG i = 0; i < MAX_AFTER_EFFECTS; ++i)
    {
        if ((i == AE_CONTRAST) || (i == AE_CONTRAST_XE))
            continue;

        if (m_apoEffects[i] != NULL)
        {
            m_apoEffects[i]->OnDeviceReset();
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeAfterEffectManager::Shutdown()
{
    for (ULONG i = 0; i < MAX_AFTER_EFFECTS; ++i)
    {
        if ((i == AE_CONTRAST) || (i == AE_CONTRAST_XE))
        {
            m_apoEffects[i] = NULL;
            continue;
        }

        SAFE_DELETE(m_apoEffects[i]);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : ApplyAfterEffects
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeAfterEffectManager::ApplyAfterEffects()
{
    LONG lTextureIn, lTextureOut;
   
#ifdef VIDEOCONSOLE_ENABLE
    if(NoAfterFX)
        return;
#endif

#ifdef ACTIVE_EDITORS
    if ((ENG_gb_EngineRunning) && (!GDI_gpst_CurDD->ShowAEInEngine)) return;
    if ((!ENG_gb_EngineRunning) && (!GDI_gpst_CurDD->ShowAEEditor)) return;
#endif

    g_pXeContextManagerRender->b_AntiAliasingBlur = FALSE;

    CXBBeginEventObject oEvent("XeAfterEffectManager::ApplyAfterEffects");

    // init pTextureIn to backbuffer texture
    lTextureIn = XESCRATCHBUFFER0_ID;
    lTextureOut = XEBACKBUFFER_ID;
    g_oXeTextureMgr.ResolveToScratchBuffer( lTextureIn );

    if (IsAfterEffectEnabled(AE_MOTIONBLUR, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("MotionBlur");

        m_apoEffects[AE_MOTIONBLUR]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    if (IsAfterEffectEnabled(AE_BLACKWHITE, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("BlackAndWhite");

        m_apoEffects[AE_BLACKWHITE]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    if (IsAfterEffectEnabled(AE_COLORBALANCE, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("ColorBalance");

        m_apoEffects[AE_COLORBALANCE]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    if (IsAfterEffectEnabled(AE_LIGHTNING_XE, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("Lightning");

        m_apoEffects[AE_LIGHTNING_XE]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    /* off for now : we don't want the ps2 settings 
    if (IsAfterEffectEnabled(AE_BRIGHTNESS, m_ulReadContext))
    {
        m_apoEffects[AE_BRIGHTNESS]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }
    */

    if (IsAfterEffectEnabled(AE_BRIGHTNESS_XE, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("Brightness");

        m_apoEffects[AE_BRIGHTNESS_XE]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    {
        CXBBeginEventObject oAEEvent("Color Correction");

        m_apoEffects[AE_COLORCORRECTION]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }
    
    if (IsAfterEffectEnabled(AE_GODRAY, m_ulReadContext) || g_pXeContextManagerRender->b_AntiAliasingBlur )
    {
        CXBBeginEventObject oAEEvent("GodRay");

        m_apoEffects[AE_GODRAY]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    if (IsAfterEffectEnabled(AE_REMANANCE, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("Remanance");

        m_apoEffects[AE_REMANANCE]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    if (IsAfterEffectEnabled(AE_BORDERBRIGHTNESS, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("Border Brightness");

        m_apoEffects[AE_BORDERBRIGHTNESS]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    if (IsAfterEffectEnabled(AE_ZOOMSMOOTHCENTER, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("ZoomSmoothCenter");

        m_apoEffects[AE_ZOOMSMOOTHCENTER]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    if (IsAfterEffectEnabled(AE_BIGBLUR, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("BigBlur");

        m_apoEffects[AE_BIGBLUR]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    if (IsAfterEffectEnabled(AE_SPINSMOOTH, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("SpinSmooth");

        m_apoEffects[AE_SPINSMOOTH]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    if (IsAfterEffectEnabled(AE_OLDMOVIE, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("OldMovie");
        m_apoEffects[AE_OLDMOVIE]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    if (IsAfterEffectEnabled(AE_BKQUAD_XE, m_ulReadContext))
    {
        CXBBeginEventObject oAEEvent("BKQuad");

        m_apoEffects[AE_BKQUAD_XE]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
        lTextureIn = lTextureOut;
    }

    // unbind all streams
    g_oRenderStateMgr.UnbindAllStreams();
}


// ------------------------------------------------------------------------------------------------
// Name   : ApplyGlow
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------

void  XeAfterEffectManager::ApplyGlow()
{
#ifdef VIDEOCONSOLE_ENABLE
    if(NoAfterFX)
        return;
#endif

#ifdef ACTIVE_EDITORS
    if ((ENG_gb_EngineRunning) && (!GDI_gpst_CurDD->ShowAEInEngine)) return;
    if ((!ENG_gb_EngineRunning) && (!GDI_gpst_CurDD->ShowAEEditor)) return;
#endif

    CXBBeginEventObject oEvent("XeAfterEffectManager::ApplyGlow");

    LONG lTextureIn(0), lTextureOut(0);

    // always apply glow
    if (IsAfterEffectEnabled(AE_COLORDIFFUSION, m_ulReadContext))
    {
        m_apoEffects[AE_COLORDIFFUSION]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : ApplyWideScreen
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeAfterEffectManager::ApplyWideScreen()
{
    LONG lTextureIn(0), lTextureOut(0);

    bool bDoDrawWideScreenBands = true;
#ifdef _XENON
  #if (_XDK_VER >= 1838)
    bDoDrawWideScreenBands = !g_oXeRenderer.GetVideoMode()->fIsWideScreen;
  #else
    bDoDrawWideScreenBands = ((XGetVideoFlags() & XC_VIDEO_FLAGS_WIDESCREEN) == 0);
  #endif
#endif

    if( bDoDrawWideScreenBands )
    {
        CXBBeginEventObject oEvent("XeAfterEffectManager::ApplyWideScreen");
        m_apoEffects[AE_WIDESCREEN]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : ApplyWideScreen
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void  XeAfterEffectManager::SetGodRayIntensity( float _fIntensity, ULONG _color )
{
    XeGodRayEffect * pEffect = (XeGodRayEffect *) m_apoEffects[AE_GODRAY];
    if( pEffect )
    {
        pEffect->SetIntensity( m_ulWriteContext, _fIntensity, _color );
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : SetParams
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeAfterEffectManager::SetParams(ULONG AENum, ULONG OnOff, ULONG P1, float Pf1)
{
    ERR_X_Assert(AENum < MAX_AFTER_EFFECTS);

#ifdef _XENON_RENDER_PC
    if (!GDI_b_IsXenonGraphics())
    {
        Gsp_AE_MASTER_OGL(AENum, OnOff, P1, Pf1);
        return;
    }
#endif

    // WARNING : we are assuming that only the writing thread will call this function

    if( AENum == AE_BORDERCOLOR )
    {
        ((XeBorderBrightness*) m_apoEffects[AE_BORDERBRIGHTNESS])->SetBorderColor(m_ulWriteContext, P1);
    }
    else if (m_apoEffects[AENum] != NULL)
    {
        // special case for brightness/contrast which are combined
        if (AENum == AE_BRIGHTNESS || AENum == AE_BRIGHTNESS_XE ) 
            P1 = 0;
        else if (AENum == AE_CONTRAST || AENum == AE_CONTRAST_XE )
            P1 = 1;

        m_apoEffects[AENum]->SetParam(m_ulWriteContext, P1, Pf1);

        if (AENum == AE_BRIGHTNESS || AENum == AE_BRIGHTNESS_XE || AENum == AE_CONTRAST || AENum == AE_CONTRAST_XE ) 
        {
           ((XeBrightnessContrastEffect*) m_apoEffects[AENum])->Enable( m_ulWriteContext, P1, OnOff != 0 );
        }
        else
        {
            m_apoEffects[AENum]->Enable(m_ulWriteContext, OnOff != 0);
        }
    }
}

void XeAfterEffectManager::ApplyXInvert()
{
    LONG lTextureIn(0), lTextureOut(0);

    if (IsAfterEffectEnabled(AE_XINVERT, m_ulReadContext))
    {
        m_apoEffects[AE_XINVERT]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
    }
}

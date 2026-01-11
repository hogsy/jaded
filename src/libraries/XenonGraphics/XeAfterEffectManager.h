#ifndef XEAFTEREFFECTMANAGER_HEADER
#define XEAFTEREFFECTMANAGER_HEADER

#include "XeProfiling.h"

#define AE_CONTEXT_COUNT                2

#define fTop    -1.0f
#define fBottom  1.0f
#define fLeft   -1.0f
#define fRight   1.0f

enum {
    AE_MOTIONBLUR       = 1,
    AE_BIGBLUR          = 2,
    AE_ZOOMSMOOTHCENTER = 5,
    AE_SPINSMOOTH       = 6,
    AE_REMANANCE        = 7,
    AE_BRIGHTNESS       = 8,
    AE_CONTRAST         = 9,
    AE_BLACKWHITE       = 11,
    AE_COLORBALANCE     = 12,
    AE_GODRAY           = 18,
    AE_BORDERBRIGHTNESS = 20,
    AE_BORDERCOLOR      = 23,
    AE_SAFEFRAME        = 30,
    AE_FOG              = 31,
    AE_WIDESCREEN       = 32,
    AE_COLORDIFFUSION   = 33,
    AE_BRIGHTNESS_XE    = 34, 
    AE_CONTRAST_XE      = 35,
    AE_LIGHTNING_XE     = 36, // SC: Used in AI, do NOT change the value
    AE_BKQUAD_XE        = 37, // SC: Used in AI too, do NOT change the value
    AE_XINVERT          = 38, 
    AE_OLDMOVIE         = 39,
    AE_COLORCORRECTION  = 40,
    
    MAX_AFTER_EFFECTS,
};

class XeAfterEffectManager;
class XeRenderObject;
class XeMesh;

#ifdef _XENON_RENDER_PC
extern void		Gsp_AE_MASTER_OGL(ULONG AENum, ULONG OnOff, ULONG P1, float Pf1);
extern ULONG	Gsp_AE_MASTER_GET_ONOFF_OGL(ULONG AENum);
extern float	Gsp_AE_MASTER_GET_P1_OGL(ULONG AENum, ULONG P1);
#endif //_XENON_RENDER_PC


#ifdef ACTIVE_EDITORS
#define M_AdjustUVToViewport( vertex, u_orig, v_orig ) (vertex)->u1 = (u_orig); (vertex)->v1 = (v_orig);                \
                                                       g_oXeRenderer.AdjustUVToViewport( (vertex)->u1, (vertex)->v1 );  \
                                                       g_oXeRenderer.AddHalfViewportPixelOffset( (vertex)->u1, (vertex)->v1 );  
#else
#define M_AdjustUVToViewport( vertex, u_orig, v_orig )
#endif

class XeAfterEffect
{
public:

M_DeclareOperatorNewAndDelete();
    
    XeAfterEffect();
    virtual ~XeAfterEffect() {}

    virtual void ReInit() = 0;
    virtual void OnDeviceLost() = 0;
    virtual void OnDeviceReset() = 0;
    virtual void Shutdown() = 0;
    virtual void Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut) = 0;

    virtual void  SetParam(ULONG ulContext, ULONG ulParam, float fValue) = 0;
    virtual float GetParam(ULONG ulContext, ULONG ulParam) = 0;
    virtual void  Enable(ULONG ulContext, BOOL bEnable);
    virtual void  CopyContext(ULONG ulSrcContext, ULONG ulDstContext);
    virtual BOOL  IsEnabled(ULONG ulContext) { return m_bEnabled[ulContext]; }

    void          DoCopyContext(ULONG _ulSrcContext, ULONG _ulDstContext);

    LPDIRECT3DDEVICE9           m_pD3DDevice;
    BOOL                        m_bEnabled[AE_CONTEXT_COUNT];
    static XeAfterEffectManager *m_poAfterEffectManager;
};

class XeAfterEffectManager 
{
public:
    XeAfterEffectManager();
    ~XeAfterEffectManager();

    // init + release
    void ReInit();
    void OnDeviceLost();
    void OnDeviceReset();
    void Shutdown();

    // rendering
    void ApplyAfterEffects();

    // after effect params access
    inline ULONG IsAfterEffectEnabled(ULONG AENum, ULONG ulContext);
           void  SetParams(ULONG AENum, ULONG OnOff, ULONG P1, float Pf1);
    inline FLOAT GetParam(ULONG AENum, ULONG P1);
    inline ULONG GetWriteContext() { return m_ulWriteContext; }
    inline ULONG GetReadContext() { return m_ulReadContext; }
    inline void  SwitchContexts();

    inline void  ApplyFog();
           void  ApplyWideScreen();
           void  ApplyGlow();
           void  SetGodRayIntensity( float _fIntensity, ULONG _color );
    inline void  ApplySafeFrame();
           void  ApplyXInvert();

private:
    ULONG               m_ulWriteContext;
    ULONG               m_ulReadContext;
    IDirect3DDevice9    *m_pD3DDevice;
    XeAfterEffect       *m_apoEffects[MAX_AFTER_EFFECTS];
};

extern __declspec(align(32)) XeAfterEffectManager g_oAfterEffectManager;

// ------------------------------------------------------------------------------------------------
// Name   : Gsp_AE_MASTER_GET_ONOFF
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
ULONG XeAfterEffectManager::IsAfterEffectEnabled(ULONG AENum, ULONG ulContext)
{
    ERR_X_Assert(AENum < MAX_AFTER_EFFECTS);

  #ifdef _XENON_RENDER_PC
    if (!GDI_b_IsXenonGraphics())
    {
        return Gsp_AE_MASTER_GET_ONOFF_OGL(AENum);
    }
  #endif

   
    return ((m_apoEffects[AENum] != NULL) && (m_apoEffects[AENum]->IsEnabled(ulContext)));
}

// ------------------------------------------------------------------------------------------------
// Name   : Gsp_AE_MASTER_GET_P1
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeAfterEffectManager::GetParam(ULONG AENum, ULONG P1)
{
    ERR_X_Assert(AENum < MAX_AFTER_EFFECTS);

  #ifdef _XENON_RENDER_PC
    if (!GDI_b_IsXenonGraphics())
    {
        return Gsp_AE_MASTER_GET_P1_OGL(AENum, P1);
    }
  #endif

    // WARNING : we are assuming that only the writing thread will call this function

    if (m_apoEffects[AENum] != NULL)
    {
        // special case for brightness/contrast which are combined
        if (AENum == AE_BRIGHTNESS) 
            P1 = 0;
        else if (AENum == AE_CONTRAST)
            P1 = 1;

        return m_apoEffects[AENum]->GetParam(m_ulWriteContext, P1);
    }

    return 0.0f;
}

// ------------------------------------------------------------------------------------------------
// Name   : SwitchContexts
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeAfterEffectManager::SwitchContexts()
{
    // actually switch the read and write context
    if (m_ulWriteContext == 0)
    {
        m_ulWriteContext    = 1;
        m_ulReadContext     = 0;
    }
    else
    {
        m_ulWriteContext    = 0;
        m_ulReadContext     = 1;
    }

    // update the new write context with the values of the pewvious write context
    for (ULONG ulAfterEffect = 0; ulAfterEffect < MAX_AFTER_EFFECTS; ulAfterEffect++)
    {
        if (m_apoEffects[ulAfterEffect] != NULL)
        {
            m_apoEffects[ulAfterEffect]->DoCopyContext(m_ulReadContext, m_ulWriteContext);
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : ApplyFog
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeAfterEffectManager::ApplyFog()
{
    LONG lTextureIn(0), lTextureOut(0);

    // always apply fog
    CXBBeginEventObject oEvent("XeAfterEffectManager::ApplyFog");
    m_apoEffects[AE_FOG]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
}

// ------------------------------------------------------------------------------------------------
// Name   : ApplySafeFrame
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void  XeAfterEffectManager::ApplySafeFrame()
{
    LONG lTextureIn(0), lTextureOut(0);
    m_apoEffects[AE_SAFEFRAME]->Apply(m_ulReadContext, lTextureIn, lTextureOut);
}


#endif // XEAFTEREFFECTMANAGER_HEADER

#ifndef XEMOTIONBLUR_HEADER
#define XEMOTIONBLUR_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"

class XeMotionBlurEffect : public XeAfterEffect
{
public:

    M_DeclareOperatorNewAndDelete();

    XeMotionBlurEffect();
    ~XeMotionBlurEffect();

    virtual void ReInit();
    virtual void OnDeviceLost();
    virtual void OnDeviceReset();
    virtual void Shutdown();
    virtual void Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut);

    inline virtual void  Enable(ULONG ulContext, BOOL bEnable);
    inline virtual void  SetParam(ULONG ulContext, ULONG ulParam, float fValue);
    inline virtual float GetParam(ULONG ulContext, ULONG ulParam);
    inline void  CopyContext(ULONG ulSrcContext, ULONG ulDstContext);

private:
    BOOL            m_bFirstFrame;
    FLOAT           m_fMotionBlurFactor[AE_CONTEXT_COUNT];
    XeRenderObject  *m_poRenderObject;
    XeMaterial      *m_poMaterial;
};

// ------------------------------------------------------------------------------------------------
// Name   : Enable
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeMotionBlurEffect::Enable(ULONG ulContext, BOOL bEnable)
{
    XeAfterEffect::Enable(ulContext, bEnable);
    if (!bEnable) 
        m_bFirstFrame = TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeMotionBlurEffect::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
    ERR_X_Assert(ulParam <= 1);

    if (fValue == 0.0f) 
        m_bFirstFrame = TRUE;

    m_fMotionBlurFactor[ulContext] = fValue;
}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeMotionBlurEffect::GetParam(ULONG ulContext, ULONG ulParam)
{
    ERR_X_Assert(ulParam <= 1);

    return m_fMotionBlurFactor[ulContext];
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeMotionBlurEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
    m_fMotionBlurFactor[ulDstContext]   = m_fMotionBlurFactor[ulSrcContext];
}

#endif // XEMOTIONBLUR_HEADER
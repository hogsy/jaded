#ifndef XEBRIGHTNESSCONTRAST_HEADER
#define XEBRIGHTNESSCONTRAST_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"

class XeBrightnessContrastEffect : public XeAfterEffect
{
public:
    XeBrightnessContrastEffect();
    ~XeBrightnessContrastEffect();

    virtual void ReInit();
    virtual void OnDeviceLost();
    virtual void OnDeviceReset();
    virtual void Shutdown();
    virtual void Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut);
    virtual void Enable(ULONG ulContext, BOOL bEnable) {}

    void Enable(ULONG ulContext, ULONG ulParam, BOOL bEnable);
    virtual BOOL  IsEnabled(ULONG ulContext) { return m_bEnableBrightness[ulContext] || m_bEnableContraste[ulContext]; }

    inline virtual void  SetParam(ULONG ulContext, ULONG ulParam, float fValue);
    inline virtual float GetParam(ULONG ulContext, ULONG ulParam);
    inline void  CopyContext(ULONG ulSrcContext, ULONG ulDstContext);

private:
    FLOAT           m_afBrightness[AE_CONTEXT_COUNT];
    FLOAT           m_afContrast[AE_CONTEXT_COUNT];
    XeRenderObject  *m_poRenderObject;
    XeMaterial      *m_poMaterial;
    BOOL            m_bEnableContraste[AE_CONTEXT_COUNT];
    BOOL            m_bEnableBrightness[AE_CONTEXT_COUNT];
};

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBrightnessContrastEffect::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
    ERR_X_Assert(ulParam <= 1);

    switch(ulParam)
    {
    case 0:
        m_afBrightness[ulContext] = fValue;
        break;
    case 1:
        m_afContrast[ulContext] = fValue;
        break;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeBrightnessContrastEffect::GetParam(ULONG ulContext, ULONG ulParam)
{
    FLOAT fValue;
    ERR_X_Assert(ulParam <= 1);

    switch(ulParam)
    {
    case 0:
        fValue = m_afBrightness[ulContext];
        break;
    case 1:
        fValue = m_afContrast[ulContext];
        break;
    default:
        fValue = 0.0f;
        break;
    }

    return fValue;
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBrightnessContrastEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
    m_afBrightness[ulDstContext]    = m_afBrightness[ulSrcContext];
    m_afContrast[ulDstContext]      = m_afContrast[ulSrcContext];

    m_bEnableContraste[ulDstContext] = m_bEnableContraste[ulSrcContext];
    m_bEnableBrightness[ulDstContext] = m_bEnableBrightness[ulSrcContext];

}

#endif // XEBRIGHTNESSCONTRAST_HEADER
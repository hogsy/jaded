#ifndef XECOLORCORRECTION_HEADER
#define XECOLORCORRECTION_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMaterial.h"

class XeColorCorrectionEffect : public XeAfterEffect
{
public:
    XeColorCorrectionEffect();
    ~XeColorCorrectionEffect();

    virtual void ReInit();
    virtual void OnDeviceLost();
    virtual void OnDeviceReset();
    virtual void Shutdown();
    virtual void Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut);

    inline virtual void  SetParam(ULONG ulContext, ULONG ulParam, float fValue);
    inline virtual float GetParam(ULONG ulContext, ULONG ulParam);
    inline void  CopyContext(ULONG ulSrcContext, ULONG ulDstContext);

private:
    XeRenderObject  *m_poRenderObject;
    XeMaterial      *m_poMaterial;
};

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorCorrectionEffect::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeColorCorrectionEffect::GetParam(ULONG ulContext, ULONG ulParam)
{
    return 0.0f;
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorCorrectionEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
}

#endif // XECOLORCORRECTION_HEADER
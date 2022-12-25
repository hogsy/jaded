#ifndef XEBIGBLUR_HEADER
#define XEBIGBLUR_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMaterial.h"

class XeBigBlurEffect : public XeAfterEffect
{
public:

    M_DeclareOperatorNewAndDelete();

    XeBigBlurEffect();
    ~XeBigBlurEffect();

    virtual void    ReInit();
    virtual void    OnDeviceLost();
    virtual void    OnDeviceReset();
    virtual void    Shutdown();
    virtual void    Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut);

    virtual void    SetParam(ULONG ulContext, ULONG ulParam, float fValue);
    virtual float   GetParam(ULONG ulContext, ULONG ulParam);
    void            CopyContext(ULONG ulSrcContext, ULONG ulDstContext);

private:
    FLOAT           m_fBlurFactor[AE_CONTEXT_COUNT];
    XeRenderObject  *m_poRenderObject;
    XeMaterial      *m_poMaterial;
};

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline
void XeBigBlurEffect::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
    ERR_X_Assert(ulParam <= 1);

    m_fBlurFactor[ulContext] = fValue;
}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline
float XeBigBlurEffect::GetParam(ULONG ulContext, ULONG ulParam)
{
    ERR_X_Assert(ulParam <= 1);

    return m_fBlurFactor[ulContext];
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline
void XeBigBlurEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
    m_fBlurFactor[ulDstContext]   = m_fBlurFactor[ulSrcContext];
}

#endif // XEBIGBLUR_HEADER
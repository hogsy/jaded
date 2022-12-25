#ifndef XECOLORBALANCE_HEADER
#define XECOLORBALANCE_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"

class XeColorBalanceEffect : public XeAfterEffect
{
public:

    M_DeclareOperatorNewAndDelete();

    XeColorBalanceEffect();
    ~XeColorBalanceEffect();

    virtual void ReInit();
    virtual void OnDeviceLost();
    virtual void OnDeviceReset();
    virtual void Shutdown();
    virtual void Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut);

    inline virtual void  SetParam(ULONG ulContext, ULONG ulParam, float fValue);
    inline virtual float GetParam(ULONG ulContext, ULONG ulParam);
    inline void  CopyContext(ULONG ulSrcContext, ULONG ulDstContext);

private:
    FLOAT        GetPhase(FLOAT fColorSpctr);

    FLOAT           m_afIntensity[AE_CONTEXT_COUNT];
    FLOAT           m_afSpectre[AE_CONTEXT_COUNT];
    XeRenderObject  *m_poRenderObject;
    XeMaterial      *m_poMaterial;
};

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorBalanceEffect::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
    ERR_X_Assert(ulParam <= 2);

    switch(ulParam)
    {
    case 0:
        m_afIntensity[ulContext] = fValue;
        break;
    case 1:
        m_afSpectre[ulContext] = fValue;
        break;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeColorBalanceEffect::GetParam(ULONG ulContext, ULONG ulParam)
{
    FLOAT fValue;
    ERR_X_Assert(ulParam <= 2);

    switch(ulParam)
    {
    case 0:
        fValue = m_afIntensity[ulContext];
        break;
    case 1:
        fValue = m_afSpectre[ulContext];
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
void XeColorBalanceEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
    m_afIntensity[ulDstContext]         = m_afIntensity[ulSrcContext];
    m_afSpectre[ulDstContext]           = m_afSpectre[ulSrcContext];
}

#endif // XECOLORBALANCE_HEADER
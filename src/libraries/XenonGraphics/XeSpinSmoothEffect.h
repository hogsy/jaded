#ifndef XESPINSMOOTHEFFECT_HEADER
#define XESPINSMOOTHEFFECT_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMaterial.h"

class XeSpinSmoothEffect : public XeAfterEffect
{

public:

    M_DeclareOperatorNewAndDelete();

    XeSpinSmoothEffect( );
    virtual ~XeSpinSmoothEffect( );

    virtual void ReInit();
    virtual void OnDeviceLost();
    virtual void OnDeviceReset();
    virtual void Shutdown();
    virtual void Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut);

    inline virtual void  SetParam(ULONG ulContext, ULONG ulParam, float fValue);
    inline virtual float GetParam(ULONG ulContext, ULONG ulParam);
    inline void CopyContext(ULONG ulSrcContext, ULONG ulDstContext);

private:
    float           m_fFactor[AE_CONTEXT_COUNT];
  
    XeRenderObject  *m_poRenderObject;
    XeMaterial      *m_poMaterial;
};

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeSpinSmoothEffect::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
    ERR_X_Assert(ulParam < 1);
    if (ulParam >= 1) return;

    if (ulParam == 0)
    {
        m_fFactor[ulContext] = fValue;
    }
 
}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeSpinSmoothEffect::GetParam(ULONG ulContext, ULONG ulParam)
{
    ERR_X_Assert(ulParam < 1);
    if (ulParam >= 1) return 0.0f;

    if (ulParam == 0)
    {
        return m_fFactor[ulContext];
    }
    return 0.0f;
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeSpinSmoothEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
    m_fFactor[ulDstContext] = m_fFactor[ulSrcContext];
}

#endif // XEZOOMSMOOTHCENTEREFFECT_HEADER
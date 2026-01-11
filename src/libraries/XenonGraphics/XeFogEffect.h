#ifndef XEFOG_HEADER
#define XEFOG_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"

class XeFogEffect : public XeAfterEffect
{
public:
    XeFogEffect();
    ~XeFogEffect();

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
void XeFogEffect::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{

}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeFogEffect::GetParam(ULONG ulContext, ULONG ulParam)
{
    return 0.0f;
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFogEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{

}

#endif // XEFOG_HEADER
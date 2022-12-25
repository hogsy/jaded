#ifndef XEZOOMSMOOTHCENTEREFFECT_HEADER
#define XEZOOMSMOOTHCENTEREFFECT_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"

#define ZSC_STEP_COUNT    7

class XeZoomSmoothCenterEffect : public XeAfterEffect
{
    struct PassTransform
    {
        float m_fScaleU, m_fOffsetU;
        float m_fScaleV, m_fOffsetV;
    };

public:

    M_DeclareOperatorNewAndDelete();

    XeZoomSmoothCenterEffect();
    ~XeZoomSmoothCenterEffect();

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
    D3DXVECTOR4     m_oSmoothDir[AE_CONTEXT_COUNT];
    BOOL            m_abDirValidated[AE_CONTEXT_COUNT];
    XeRenderObject  *m_poRenderObject;
    XeMesh          *m_poMesh;
    XeMaterial      *m_poMaterial;
    float           m_passAlpha[ZSC_STEP_COUNT];
    PassTransform   m_passTransform[ZSC_STEP_COUNT];
};

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeZoomSmoothCenterEffect::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
    ERR_X_Assert(ulParam < 4);
    if (ulParam >= 4) return;

    if (ulParam == 0)
    {
        m_fFactor[ulContext] = fValue;
    }
    else
    {
        m_oSmoothDir[ulContext][ulParam-1] = fValue;
        m_abDirValidated[ulContext] = TRUE;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeZoomSmoothCenterEffect::GetParam(ULONG ulContext, ULONG ulParam)
{
    ERR_X_Assert(ulParam < 4);
    if (ulParam >= 4) return 0.0f;

    if (ulParam == 0)
    {
        return m_fFactor[ulContext];
    }
    else
    {
        return m_oSmoothDir[ulContext][ulParam-1];
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeZoomSmoothCenterEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
    m_fFactor[ulDstContext]        = m_fFactor[ulSrcContext];
    m_oSmoothDir[ulDstContext]     = m_oSmoothDir[ulSrcContext];
    m_abDirValidated[ulDstContext] = m_abDirValidated[ulSrcContext];

    m_abDirValidated[ulDstContext] = FALSE;
}
#endif // XEZOOMSMOOTHCENTEREFFECT_HEADER
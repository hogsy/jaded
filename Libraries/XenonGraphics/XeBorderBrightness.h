#ifndef XEBORDERBRIGHTNESS_HEADER
#define XEBORDERBRIGHTNESS_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"

class XeBorderBrightness : public XeAfterEffect
{
public:
    M_DeclareOperatorNewAndDelete();

    XeBorderBrightness();
    ~XeBorderBrightness();

    virtual void ReInit();
    virtual void OnDeviceLost();
    virtual void OnDeviceReset();
    virtual void Shutdown();
    virtual void Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut);

    inline virtual void  SetParam(ULONG ulContext, ULONG ulParam, float fValue);
    inline virtual float GetParam(ULONG ulContext, ULONG ulParam);
    inline void  CopyContext(ULONG ulSrcContext, ULONG ulDstContext);
    void         SetBorderColor(ULONG ulContext, ULONG _color );

private:
    void ApplyBlur( LONG lTextureIn, LONG &lTextureOut );

    float           m_fBorderBrightness[AE_CONTEXT_COUNT];
    ULONG           m_ulBorderColor[AE_CONTEXT_COUNT];


    XeRenderObject  *m_poBorderRenderObject;
    XeMesh          *m_poBorderMesh;
    XeIndexBuffer   *m_poBorderIB;
    XeMaterial      *m_poMaterial;
};

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline 
void XeBorderBrightness::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
    ERR_X_Assert(ulParam < 1);
    if (ulParam >= 1) return;

    if (ulParam == 0)
    {
        m_fBorderBrightness[ulContext] = fValue;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline
void XeBorderBrightness::SetBorderColor(ULONG ulContext, ULONG _color )
{
    m_ulBorderColor[ulContext] = _color & 0xFFFFFF;
}


// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline
float XeBorderBrightness::GetParam(ULONG ulContext, ULONG ulParam)
{
    ERR_X_Assert(ulParam < 4);
    if (ulParam >= 1) return 0.0f;

    if (ulParam == 0)
    {
        return m_fBorderBrightness[ulContext];
    }

    return 0.0f;
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline 
void XeBorderBrightness::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
    m_fBorderBrightness[ulDstContext] = m_fBorderBrightness[ulSrcContext];
    m_ulBorderColor[ulDstContext] = m_ulBorderColor[ulSrcContext];
}

#endif // XEBORDERBRIGHTNESS_HEADER
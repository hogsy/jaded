#ifndef XEGODRAYEFFECT_HEADER
#define XEGODRAYEFFECT_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"

#define GODRAY_STEP_COUNT    7

class XeGodRayEffect : public XeAfterEffect
{
    struct PassTransform
    {
        float m_fScaleU, m_fOffsetU;
        float m_fScaleV, m_fOffsetV;
    };

    public:

        M_DeclareOperatorNewAndDelete();

        XeGodRayEffect();
        ~XeGodRayEffect();

        virtual void ReInit();
        virtual void OnDeviceLost();
        virtual void OnDeviceReset();
        virtual void Shutdown();
        virtual void Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut);
                
        inline virtual void  SetParam(ULONG ulContext, ULONG ulParam, float fValue);
        inline virtual float GetParam(ULONG ulContext, ULONG ulParam);
        inline void  CopyContext(ULONG ulSrcContext, ULONG ulDstContext);
        void         SetIntensity(ULONG ulContext, float _fIntensity, ULONG _color );

    private:
        void ApplyBlur( LONG lTextureIn, LONG &lTextureOut );

        float           m_fFactor[AE_CONTEXT_COUNT];
        D3DXVECTOR4     m_oLightDir[AE_CONTEXT_COUNT];
        VECTOR4FLOAT    m_vAdjust[AE_CONTEXT_COUNT];
        
        XeRenderObject  *m_poRenderObject;
        XeMesh          *m_poMesh;

        XeRenderObject  *m_poBorderRenderObject;
        XeMesh          *m_poBorderMesh;
        XeIndexBuffer   *m_poBorderIB;

        XeMaterial      *m_poMaterial;
        float           m_passAlpha[GODRAY_STEP_COUNT];
        PassTransform   m_passTransform[GODRAY_STEP_COUNT];
};

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeGodRayEffect::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
    ERR_X_Assert(ulParam < 4);
    if (ulParam >= 4) return;

    if (ulParam == 0)
    {
        m_fFactor[ulContext] = fValue;
    }
    else
    {
        m_oLightDir[ulContext][ulParam-1] = fValue;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeGodRayEffect::GetParam(ULONG ulContext, ULONG ulParam)
{
    ERR_X_Assert(ulParam < 4);
    if (ulParam >= 4) return 0.0f;

    if (ulParam == 0)
    {
        return m_fFactor[ulContext];
    }
    else
    {
        return m_oLightDir[ulContext][ulParam-1];
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeGodRayEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
    m_fFactor[ulDstContext]     = m_fFactor[ulSrcContext];
    m_oLightDir[ulDstContext]   = m_oLightDir[ulSrcContext];
    m_vAdjust[ulDstContext]     = m_vAdjust[ulSrcContext];
}

#endif // XEGODRAYEFFECT_HEADER
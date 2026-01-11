// ------------------------------------------------------------------------------------------------
// File   : XeLightningAE.h
// Date   : 2005-08-30
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XELIGHTNINGAE_H
#define GUARD_XELIGHTNINGAE_H

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"

// ------------------------------------------------------------------------------------------------
// CLASSES
// ------------------------------------------------------------------------------------------------

class XeLightningAE : public XeAfterEffect
{
public:

    XeLightningAE();
    ~XeLightningAE();

    virtual void ReInit();
    virtual void OnDeviceLost();
    virtual void OnDeviceReset();
    virtual void Shutdown();
    virtual void Apply(ULONG _ulContext, LONG _lTextureIn, LONG& _rlTextureOut);

    inline virtual void  SetParam(ULONG _ulContext, ULONG _ulParam, FLOAT _fValue);
    inline virtual FLOAT GetParam(ULONG _ulContext, ULONG _ulParam);

    inline void CopyContext(ULONG _ulSrcContext, ULONG _ulDstContext);

private:

    FLOAT           m_afBrightness[AE_CONTEXT_COUNT];
    FLOAT           m_afContrast[AE_CONTEXT_COUNT];
    XeRenderObject* m_poRenderObject;
    XeMesh*         m_poMesh;
    XeMaterial*     m_poMaterial;
};

// ------------------------------------------------------------------------------------------------
// INLINE IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

inline void XeLightningAE::SetParam(ULONG _ulContext, ULONG _ulParam, FLOAT _fValue)
{
    switch (_ulParam)
    {
        case 0:
            m_afBrightness[_ulContext] = 0.5f * _fValue + 0.5f;
            break;

        case 1:
            m_afContrast[_ulContext] = 0.5f * _fValue + 0.5f;
            break;
    }
}

inline FLOAT XeLightningAE::GetParam(ULONG _ulContext, ULONG _ulParam)
{
    switch (_ulParam)
    {
        case 0:  return m_afBrightness[_ulContext];
        case 1:  return m_afContrast[_ulContext];
        default: return 0.0f;
    }
}

inline void XeLightningAE::CopyContext(ULONG _ulSrcContext, ULONG _ulDstContext)
{
    m_afBrightness[_ulDstContext] = m_afBrightness[_ulSrcContext];
    m_afContrast[_ulDstContext]   = m_afContrast[_ulSrcContext];
}

#endif // #ifdef GUARD_XELIGHTNINGAE_H

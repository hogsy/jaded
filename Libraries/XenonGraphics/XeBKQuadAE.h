// ------------------------------------------------------------------------------------------------
// File   : XeBKQuadAE.h
// Date   : 2005-04-08
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XEBKQUADAE_H
#define GUARD_XEBKQUADAE_H

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

class XeBKQuadAE : public XeAfterEffect
{
public:

    M_DeclareOperatorNewAndDelete();

    XeBKQuadAE();
    ~XeBKQuadAE();

    virtual void ReInit();
    virtual void OnDeviceLost();
    virtual void OnDeviceReset();
    virtual void Shutdown();
    virtual void Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut);

    inline virtual void  SetParam(ULONG ulContext, ULONG ulParam, float fValue);
    inline virtual float GetParam(ULONG ulContext, ULONG ulParam);
    inline void  CopyContext(ULONG ulSrcContext, ULONG ulDstContext);

private:

    FLOAT           m_afFactor[AE_CONTEXT_COUNT];
    XeRenderObject* m_poRenderObject;
    XeMesh*         m_poMesh;
    XeMaterial*     m_poMaterial;
};

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBKQuadAE::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
    m_afFactor[ulContext] = fValue;
}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeBKQuadAE::GetParam(ULONG ulContext, ULONG ulParam)
{
    return m_afFactor[ulContext];
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBKQuadAE::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
    m_afFactor[ulDstContext] = m_afFactor[ulSrcContext];
}

#endif // #ifdef GUARD_XEBKQUADAE_H

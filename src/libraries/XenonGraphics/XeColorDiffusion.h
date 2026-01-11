// ------------------------------------------------------------------------------------------------
// File   : XeColorDiffusion.h
// Date   : 
// Author : 
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XECOLORDIFFUSION_H
#define GUARD_XECOLORDIFFUSION_H

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "XeAfterEffectManager.h"

class XeRenderObject;
class XeMesh; 
class XeMaterial;

// ------------------------------------------------------------------------------------------------
// CLASSES
// ------------------------------------------------------------------------------------------------

class XeColorDiffusionEffect : public XeAfterEffect
{
public:
    enum
    {
        ReductionFactor  = 6,
    };

    M_DeclareOperatorNewAndDelete();

    XeColorDiffusionEffect                          ( );
    ~XeColorDiffusionEffect                         ( );

    virtual void            ReInit                  ( );
    virtual void            OnDeviceLost            ( );
    virtual void            OnDeviceReset           ( );
    virtual void            Shutdown                ( );
    virtual void            Apply                   ( ULONG ulContext, LONG lTextureIn, LONG &lTextureOut );

    inline virtual void     SetParam                ( ULONG ulContext, ULONG ulParam, float fValue );
    inline virtual float    GetParam                ( ULONG ulContext, ULONG ulParam );
    inline void             CopyContext             ( ULONG ulSrcContext, ULONG ulDstContext );
  
private:

    void                    PrepareForBlur          ( ULONG ulContext );
    void                    RegionToRect            ( D3DRECT & _oRect, LONG iRegion );
    float                   GetGlowRegionScale      ( LONG iRegion );
    float                   GetGlowRegionOffset     ( LONG iRegion );

    XeRenderObject*         m_poRenderObject;
    XeMesh*                 m_poMesh;
    XeMaterial*             m_poMaterial;

    float                   m_fLuminosityMin[AE_CONTEXT_COUNT];
    float                   m_fLuminosityMax[AE_CONTEXT_COUNT];
    float                   m_fIntensity[AE_CONTEXT_COUNT];
    ULONG                   m_ulGlowColor[AE_CONTEXT_COUNT];
    float                   m_fZNear[AE_CONTEXT_COUNT];
    float                   m_fZFar[AE_CONTEXT_COUNT];
};

// ------------------------------------------------------------------------------------------------
// Name   : SetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorDiffusionEffect::SetParam(ULONG ulContext, ULONG ulParam, float fValue)
{
    if( ulParam == 0 )
    {
        m_fLuminosityMin[ulContext] = fValue;
    }
    else if( ulParam == 1 )
    {
        m_fLuminosityMax[ulContext] = fValue;
    }
    else if( ulParam == 2 )
    {
        m_fIntensity[ulContext] = fValue;
    }
    else if( ulParam == 3 )
    {
        m_ulGlowColor[ulContext] = *((ULONG*) &fValue);
    }
    else if( ulParam == 4 )
    {
        m_fZNear[ulContext] = fValue;
    }
    else if( ulParam == 5 )
    {
        m_fZFar[ulContext] = fValue;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : GetParam
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float XeColorDiffusionEffect::GetParam(ULONG ulContext, ULONG ulParam)
{
    if( ulParam == 0 )
    {
        return m_fLuminosityMin[ulContext];
    }
    else if( ulParam == 1 )
    {
        return m_fLuminosityMax[ulContext];
    }
    else if( ulParam == 2 )
    {
        return m_fIntensity[ulContext];
    }
    else if( ulParam == 3 )
    {
        return *((float*)&m_ulGlowColor[ulContext]);
    }
    else if( ulParam == 4 )
    {
        return m_fZNear[ulContext];
    }
    else if( ulParam == 5 )
    {
        return m_fZFar[ulContext];
    }
        
    return 0.0f;
}

// ------------------------------------------------------------------------------------------------
// Name   : CopyContext
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorDiffusionEffect::CopyContext(ULONG ulSrcContext, ULONG ulDstContext)
{
    m_fLuminosityMin[ulDstContext]      = m_fLuminosityMin[ulSrcContext];
    m_fLuminosityMax[ulDstContext]      = m_fLuminosityMax[ulSrcContext];
    m_fIntensity[ulDstContext]          = m_fIntensity[ulSrcContext];
    m_ulGlowColor[ulDstContext]         = m_ulGlowColor[ulSrcContext];
    m_fZNear[ulDstContext]              = m_fZNear[ulSrcContext];
    m_fZFar[ulDstContext]               = m_fZFar[ulSrcContext];
}

#endif // #ifdef GUARD_XECOLORDIFFUSION_H

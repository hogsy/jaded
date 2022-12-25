#ifndef XEWIDESCREEN_HEADER
#define XEWIDESCREEN_HEADER

#include "XeAfterEffectManager.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"

class XeWideScreenEffect : public XeAfterEffect
{
public:
    XeWideScreenEffect();
    ~XeWideScreenEffect();

    virtual void ReInit();
    virtual void OnDeviceLost();
    virtual void OnDeviceReset();
    virtual void Shutdown();
    virtual void Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut);

    inline virtual void  SetParam(ULONG ulContext, ULONG ulParam, float fValue) {}
    inline virtual float GetParam(ULONG ulContext, ULONG ulParam) { return 0.0f;}
    inline void  CopyContext(ULONG ulSrcContext, ULONG ulDstContext) {}

private:

    XeRenderObject  *m_poRenderObject;
    XeMaterial      *m_poMaterial;
    XeMesh          *m_poMesh;
    XeIndexBuffer   *m_poIB;

};

#endif // XEWIDESCREEN_HEADER
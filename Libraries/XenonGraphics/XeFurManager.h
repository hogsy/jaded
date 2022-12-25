#ifndef XE_FUR_MANAGER
#define XE_FUR_MANAGER

#include "XeVertexShaderManager.h"
#include "XePixelShaderManager.h"

class XeRenderObject;

#define FUR_TECHNIQUE_PS2       0
#define FUR_TECHNIQUE_XENON1    1
#define FUR_MAX_LIT_LAYERS      100

// FUR
struct XeFurInfo
{
    ULONG               m_ulType;
    ULONG               m_ulMaxLitLayers;

    XeVertexShaderManager::XeVSFeature         m_oVSFeatures;
    XePixelShaderManager::XePSFeature          m_oPSFeatures;
    ULONG               m_ulLightSet;
    ULONG               m_ulLightCount;

    VECTOR4FLOAT        m_vFURParams;
	BOOL FirstPass;
};

// Simple utility class
class XeFurManager
{
public:
    XeFurManager() {};
    ~XeFurManager() {};

    void UpdateObjectFurInfo(XeRenderObject *_pObject);
    void UpdateFurPrimitive(XeRenderObject *_pObject, INT iPrimitive);

private:

};

extern XeFurManager g_oFurManager;

#endif // XE_FUR_MANAGER
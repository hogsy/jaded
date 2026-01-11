#include "Precomp.h"
#include "XeRenderer.h"
#include "XeFurManager.h"
#include "XeRenderObject.h"

XeFurManager                g_oFurManager;

extern int					NbrFurLayers;
extern float				OffsetNormal;
extern float				OffsetU;
extern float				OffsetV;
extern int					FurInvertAlpha;
extern float                FurLength;
extern ULONG                FurMaxLitLayers;
extern int					DontDrawFirstLayer;
// ------------------------------------------------------------------------------------------------
// Name   : XeFurManager::SetFurInfo
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFurManager::UpdateObjectFurInfo(XeRenderObject *_pObject)
{
    XeFurInfo oFURInfo;
    oFURInfo.m_vFURParams.x = (FLOAT)NbrFurLayers;

    if (FurLength < 0)
    {
        oFURInfo.m_ulType = FUR_TECHNIQUE_PS2;
        oFURInfo.m_vFURParams.y = (FLOAT)OffsetNormal;
    }
    else
    {
        oFURInfo.m_ulType = FUR_TECHNIQUE_XENON1;
        oFURInfo.m_ulMaxLitLayers = (ULONG)FurMaxLitLayers;
        oFURInfo.m_ulLightSet = 0;
        oFURInfo.m_ulLightCount = 0;
        oFURInfo.m_oVSFeatures.Clear();
        oFURInfo.m_oPSFeatures.Clear();
        oFURInfo.m_vFURParams.y = (FLOAT)FurLength / (FLOAT)NbrFurLayers;
    }


    oFURInfo.m_vFURParams.z = (FLOAT)OffsetU;
    oFURInfo.m_vFURParams.w = (FLOAT)OffsetV;
	oFURInfo.FirstPass		= (BOOL)DontDrawFirstLayer;


    _pObject->SetFURShaderConsts(&oFURInfo);    
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFurManager::UpdateFurPrimitive
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFurManager::UpdateFurPrimitive(XeRenderObject* _pObject, INT iPrimitive)
{
    XeFurInfo       *poFurInfo    = _pObject->GetFURShaderConsts();
    VECTOR4FLOAT    *pFurParams   = &poFurInfo->m_vFURParams;
    VECTOR4FLOAT    oInstanceParams;

    if (poFurInfo->m_ulType == FUR_TECHNIQUE_XENON1)
    {
        // set fur offset scale
        FLOAT fLayer  = (FLOAT)(iPrimitive+1) / pFurParams->x;
        FLOAT fScale  = -((iPrimitive+1) * pFurParams->y) * (1.0f*fLayer*fLayer + 0.4f*fLayer);
        oInstanceParams.x = fScale;
    }
    else
    {
        oInstanceParams.x = 0.0f;
    }
    
    oInstanceParams.y = pFurParams->y * iPrimitive;
    oInstanceParams.z = pFurParams->z * iPrimitive;
    oInstanceParams.w = pFurParams->w * iPrimitive;
    g_pXeContextManagerRender->SetFURShaderConsts(&oInstanceParams);

    g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAREF, (DWORD) ((iPrimitive*255)/pFurParams->x));
    g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    g_oRenderStateMgr.Update(_pObject, FALSE, FALSE);

    g_oVertexShaderMgr.UpdateShaderConstantsFur(g_oRenderStateMgr.GetCurrentVertexShader());
}
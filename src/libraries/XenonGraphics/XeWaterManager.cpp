#include "Precomp.h"
#include "XeRenderer.h"
#include "XeWaterManager.h"
#include "XeTextureManager.h"
#include "XeMaterial.h"
#include "XeShader.h"
#include "XeRenderStateManager.h"
#include "XeRenderTargetManager.h"

#ifdef _XENON_RENDER_PC
#define WATER_EFFECT_MAX_DISTANCE 150.0f
#else
#define WATER_EFFECT_MAX_DISTANCE 75.0f
#endif

#define REFLECTION_BLUR_PASSES 3
#define REFLECTION_FINAL_BLUR_BUFFER    XEREFLECTIONSCRATCH1_ID // NOTE: Must be scratch 0 if REFLECTION_BLUR_PASSES is even
 
FLOAT g_fWaterBlurFactor = 0.5f; // set default blur factor here.

// single instance of the water manager
XeWaterManager g_oWaterManager;

// ------------------------------------------------------------------------------------------------
// Name   : XeWaterManager
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeWaterManager::XeWaterManager()
{
    L_memset(m_aRegisteredWaterPatches, 0, XE_WATERPATCH_REGISTER_MAX*sizeof(WATER3D_tdst_Patches*));
    m_pBlurObject       = NULL;
    m_pBlurMaterial     = NULL;
    m_pReflectionBuffers[0] = NULL;
    m_pReflectionBuffers[1] = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeWaterManager
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeWaterManager::~XeWaterManager()
{

}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::ReInit( )
{
    m_pD3DDevice = g_oXeRenderer.GetDevice();

    m_bUsingReflectionBuffer = FALSE;
    m_oStateEngine.m_ulCurrentPatchSet = 0;
    m_oStateEngine.m_ulNbPatchSets     = 1;
    m_oStateEngine.m_ulCurrentPatchInfoIdIndex = 0;
    L_memset(m_oStateEngine.m_aulPatchInfoIdList, 0, XE_MAX_PATCH_ID_LIST * sizeof(ULONG));

    // init blur render targets
    m_pReflectionBuffers[0] = (IDirect3DTexture9 *) g_oXeTextureMgr.GetScratchBuffer(XEREFLECTIONSCRATCH0_ID);
    m_pReflectionBuffers[1] = (IDirect3DTexture9 *) g_oXeTextureMgr.GetScratchBuffer(XEREFLECTIONSCRATCH1_ID);

    D3DSURFACE_DESC oDesc;
    m_pReflectionBuffers[0]->GetLevelDesc(0, &oDesc);
    m_fBlurTexelWidth = 1.0f / (FLOAT) oDesc.Width;
    m_fBlurTexelHeight = 1.0f / (FLOAT) oDesc.Height;

    // Blur material
    SAFE_DELETE(m_pBlurMaterial);
    m_pBlurMaterial = new XeMaterial();
    m_pBlurMaterial->SetCustomVS(CUSTOM_VS_REFLECTION);
    m_pBlurMaterial->SetCustomVSFeature(1, 1); // Blur shader id
    m_pBlurMaterial->SetCustomPS(CUSTOM_PS_REFLECTION);
    m_pBlurMaterial->SetCustomPSFeature(1, 1); // Blur shader id
    m_pBlurMaterial->SetZState(FALSE, FALSE);
    m_pBlurMaterial->SetAlphaBlend(FALSE);
    m_pBlurMaterial->AddTextureStage();
    m_pBlurMaterial->SetFilterMode(0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR);
    m_pBlurMaterial->SetAddressMode(0, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);

    SAFE_DELETE(m_pBlurObject);
    m_pBlurObject = new XeRenderObject();
    m_pBlurObject->SetMesh(g_oXeRenderer.GetQuadMesh());
    m_pBlurObject->SetMaterial(m_pBlurMaterial);
    m_pBlurObject->SetPrimType(XeRenderObject::TriangleStrip);
    m_pBlurObject->SetDrawMask(0xffffffff & ~(GDI_Cul_DM_Lighted | GDI_Cul_DM_Fogged | GDI_Cul_DM_TestBackFace));

    // Water material
    SAFE_DELETE(m_pWaterMaterial);
    m_pWaterMaterial = new XeMaterial();
    m_pWaterMaterial->SetCustomVS(CUSTOM_VS_WATER);
    m_pWaterMaterial->SetCustomPS(CUSTOM_PS_WATER);
    m_pWaterMaterial->AddTextureStage();
    m_pWaterMaterial->SetTextureId(0, XESCRATCHBUFFER0_ID); // scratch 0 contains current back buffer for refraction
    m_pWaterMaterial->AddTextureStage();
    m_pWaterMaterial->SetTextureId(1, REFLECTION_FINAL_BLUR_BUFFER); // reflection buffer
    m_pWaterMaterial->AddTextureStage(); // environnment map
    m_pWaterMaterial->SetAddressMode(2, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP);
    m_pWaterMaterial->AddTextureStage(); // normal map
    m_pWaterMaterial->SetAddressMode(3, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP);
    m_pWaterMaterial->AddTextureStage(); // detail map
    m_pWaterMaterial->SetAddressMode(4, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP);
    m_pWaterMaterial->AddTextureStage(); // Depth buffer
    m_pWaterMaterial->SetFilterMode(5, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE);
    m_pWaterMaterial->AddTextureStage(); // base map
    m_pWaterMaterial->SetAddressMode(6, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP);
    m_pWaterMaterial->SetTwoSided(TRUE);
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::Shutdown( )
{
    SAFE_DELETE(m_pWaterMaterial);
    SAFE_DELETE(m_pBlurObject);
    SAFE_DELETE(m_pBlurMaterial);
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::OnDeviceLost( )
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::OnDeviceReset( )
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : RegisterWaterPatch
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
ULONG XeWaterManager::RegisterWaterPatch(WATER3D_tdst_Modifier* _pst_Water, WATER3D_tdst_Patches* p_Patch)
{
    ULONG i;

    for (i = 0; i < XE_WATERPATCH_REGISTER_MAX; ++i)
    {
        if ((m_aRegisteredWaterPatches[i] == NULL) ||
            (m_aRegisteredWaterPatches[i] == p_Patch))
        {
            m_aRegisteredWaterPatches[i] = p_Patch;

            XeWaterPatchInfo *poExtraPatchInfo = &m_oStateEngine.m_aWaterPatches[i];
            poExtraPatchInfo->oPlaneOrigin.x = (p_Patch->fMinX + p_Patch->fMaxX) * 0.5f;
            poExtraPatchInfo->oPlaneOrigin.y = (p_Patch->fMinY + p_Patch->fMaxY) * 0.5f;
            poExtraPatchInfo->oPlaneOrigin.z = (p_Patch->fZMinMin + p_Patch->fZMaxMin + p_Patch->fZMinMax + p_Patch->fZMaxMax) * 0.25f;
            poExtraPatchInfo->oPlaneOrigin.w = 1.0f;
            poExtraPatchInfo->oPlaneEquation.x = 0.0f;
            poExtraPatchInfo->oPlaneEquation.y = 0.0f;
            poExtraPatchInfo->oPlaneEquation.z = 1.0f;
            poExtraPatchInfo->oPlaneEquation.w = -poExtraPatchInfo->oPlaneOrigin.z;

            UpdateModifierParams(i, _pst_Water);

            poExtraPatchInfo->bUpdated = FALSE;

            return i;
        }
    }

    return -1;
}

// ------------------------------------------------------------------------------------------------
// Name   : UnregisterWaterPatch
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::UnregisterWaterPatch(WATER3D_tdst_Patches* p_Patch)
{
    ULONG i;

    for (i = 0; i < XE_WATERPATCH_REGISTER_MAX; ++i)
    {
        if (m_aRegisteredWaterPatches[i] == p_Patch)
        {
            m_aRegisteredWaterPatches[i] = NULL;
            break;
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : UpdateModifierParams
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::UpdateModifierParams(ULONG _ulPatchId, WATER3D_tdst_Modifier* _pst_Water)
{
    if (_ulPatchId != -1)
    {
        XeWaterPatchInfo *poExtraPatchInfo  = &m_oStateEngine.m_aWaterPatches[_ulPatchId];

        poExtraPatchInfo->fReflectionFactor = _pst_Water->fReflectionIntensity;
        poExtraPatchInfo->fRefractionFactor = _pst_Water->fRefractionIntensity;
        poExtraPatchInfo->fWaterDensity     = _pst_Water->fWaterDensity;
        poExtraPatchInfo->fBaseMapOpacity   = _pst_Water->fBaseMapOpacity;
        poExtraPatchInfo->fMossMapOpacity   = _pst_Water->fMossMapOpacity;
        poExtraPatchInfo->fFogIntensity     = _pst_Water->fFogIntensity;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : BeginReflection
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
BOOL XeWaterManager::BeginReflection(int _nbElem)
{
    m_bUsingReflectionBuffer = FALSE;
    m_ulReflectedObjects = 0;

    if (_nbElem > 0)
    {
        if (g_oXeRenderTargetMgr.BeginRenderTarget( 0, 
                                                    m_pReflectionBuffers[0], 
                                                    NULL, 
                                                    TRUE, 
                                                    TRUE, 
                                                    0xFF000000,
                                                    TRUE, 
                                                    1.0f, 
                                                  #ifdef _XENON
                                                    TRUE, 
                                                  #else
                                                    FALSE,
                                                  #endif
                                                    D3DFMT_A8R8G8B8, 
                                                    D3DFMT_D24S8, 
                                                    g_oXeRenderer.GetMultiSampleType()))
        {
            m_bUsingReflectionBuffer = TRUE;
            return TRUE;
        }
    }

    return FALSE;
}

// ------------------------------------------------------------------------------------------------
// Name   : EndReflection
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::EndReflection()
{
    if (!m_bUsingReflectionBuffer)
        return;

    DWORD dwColor = g_oXeRenderer.GetBackgroundColor();
    dwColor = XeConvertColor(dwColor);
    g_oXeRenderTargetMgr.EndRenderTarget(0, FALSE, FALSE, dwColor, FALSE);

    CXBBeginEventObject oEvent("BlurReflection");

    // Blur reflection buffer
    ULONG ulCurReflectionBuffer = 1;
    for (int j = 0; j < REFLECTION_BLUR_PASSES; ++j)
    {
        // SC: Keep the current depth buffer even if the size don't match since we are not using
        //     it. This allows for the HiZ information to be preserved.
        if (g_oXeRenderTargetMgr.BeginRenderTarget( 0, 
                                                    m_pReflectionBuffers[ulCurReflectionBuffer], 
                                                    NULL, 
                                                    TRUE, 
                                                    FALSE, 
                                                    0, 
                                                    FALSE, 
                                                    0.0f, 
                                                  #ifdef _XENON
                                                    TRUE, 
                                                  #else
                                                    FALSE,
                                                  #endif
                                                    D3DFMT_A8R8G8B8, 
                                                    D3DFMT_D24S8, 
                                                    g_oXeRenderer.GetMultiSampleType()))
        {
            // Apply blur pass
            ulCurReflectionBuffer = (ulCurReflectionBuffer + 1) % 2;

            // Update the material
            m_pBlurMaterial->SetTextureId(0, XEREFLECTIONSCRATCH0_ID + ulCurReflectionBuffer);

            FLOAT fOffsetX = g_fWaterBlurFactor * ((FLOAT)j + 1.0f) * m_fBlurTexelWidth;
            FLOAT fOffsetY = g_fWaterBlurFactor * ((FLOAT)j + 1.0f) * m_fBlurTexelHeight;
            m_avTexOffsets[0].x = -fOffsetX; m_avTexOffsets[0].y = -fOffsetY;
            m_avTexOffsets[0].z =  fOffsetX; m_avTexOffsets[0].w = -fOffsetY;
            m_avTexOffsets[1].x =  fOffsetX; m_avTexOffsets[1].y =  fOffsetY;
            m_avTexOffsets[1].z = -fOffsetX; m_avTexOffsets[1].w =  fOffsetY;

            g_oXeRenderer.RenderObject(m_pBlurObject, XeFXManager::RP_DEFAULT);

            g_oXeRenderTargetMgr.EndRenderTarget(0, FALSE, j == (REFLECTION_BLUR_PASSES-1), dwColor, j == (REFLECTION_BLUR_PASSES-1));
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : RenderWater
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
ULONG XeWaterManager::RenderReflection(ULONG ulNbWaterElements, XeRenderObject **_aoWaterList, ULONG ulNbReflectedElements, XeRenderObject **_aoReflectedList)
{
    ULONG           ulElementIdx, ulWaterElement, ulObjectsRendered = 0;
    FLOAT           fMinDistance, fDistance;
    XeRenderObject  *pReflectedObj(NULL), *pObj(NULL);
    XeMaterial      *poMaterial(NULL);
    XeWaterPatchInfo *poPatchInfo(NULL), *poClosestPatchInfo(NULL);
    XeWaterPatchSet *poPatchSet;

    CXBBeginEventObject oEvent("RenderReflection");

    if ((ulNbWaterElements == 0) || (ulNbReflectedElements == 0)) 
        return 0;

    fMinDistance = FLT_MAX;

    for (ulWaterElement = 0; ulWaterElement < ulNbWaterElements; ulWaterElement++)
    {
        // get current water patch information
        pObj        = _aoWaterList[ulWaterElement];
        poPatchSet  = &m_oStateRender.m_aoPatchSets[pObj->GetWaterPatchSetIndex()];

        for (ULONG ulCurPatch = 0; ulCurPatch < poPatchSet->ulPatchCount; ulCurPatch++)
        {
            poPatchInfo = UpdateWaterPatchInfo(pObj, m_oStateRender.m_aulPatchInfoIdList[poPatchSet->ulFirstPatchIndex + ulCurPatch]);

#if !defined(_XENON)
            if (poPatchInfo != NULL)
#endif
            {
                // check for min distance with the viewer
                D3DXVECTOR3 oDirectionVector;
                fDistance = D3DXVec3Length((D3DXVECTOR3*)&poPatchInfo->oViewSpacePlaneOrigin);
                if (fDistance < fMinDistance)
                {
                    // select this patch since it has the minimum distance between it and the reflected object
                    poClosestPatchInfo = poPatchInfo;
                    fMinDistance = fDistance;
                }
            }
        }
    }

    if (poClosestPatchInfo == NULL) return 0;

    // set common render states
    g_oVertexShaderMgr.EnableReflection(TRUE);
    g_oRenderStateMgr.SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

    // setup reflection and clipping planes
    g_oVertexShaderMgr.SetReflectionPlane((D3DXVECTOR3*)&poClosestPatchInfo->oViewSpacePlaneEquation, (D3DXVECTOR3*)&poClosestPatchInfo->oViewSpacePlaneOrigin);
    g_oRenderStateMgr.SetClipPlane(0, (FLOAT*) poClosestPatchInfo->oClipSpacePlaneEquation);

    for(ulElementIdx = 0; ulElementIdx < (ULONG) ulNbReflectedElements; ++ulElementIdx)
    {
        pReflectedObj   = _aoReflectedList[ulElementIdx];

        // calculate viewspace origin of the current reflected object
        D3DXMATRIX* poWorldView = g_pXeContextManagerRender->GetWorldViewMatrixByIndex(pReflectedObj->GetWorldViewMatrixIndex());

        if (poWorldView->_43 > WATER_EFFECT_MAX_DISTANCE) 
            continue;

        // render reflected object
        g_oXeRenderer.RenderObject(pReflectedObj, XeFXManager::RP_APPLY_REFLECTION);

        m_ulReflectedObjects++;
        ulObjectsRendered++;
    }

    // restore states
    g_oRenderStateMgr.SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
    g_oVertexShaderMgr.EnableReflection(FALSE);

    //DrawRectangleEx(0.0f, 0.0f, 0.25f, 0.25f, 0.0f, 0.0f, 1.0f, 1.0f, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f, MAT_Cc_Op_Copy, REFLECTION_FINAL_BLUR_BUFFER);

    return ulObjectsRendered;
}

// ------------------------------------------------------------------------------------------------
// Name   : RenderWater
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::RenderWater(ULONG ulNbWaterElements, XeRenderObject **_aoWaterList, int _nbReflectionObjects, XeRenderObject **_aoReflectedList)
{
    if (ulNbWaterElements == 0) return;

    CXBBeginEventObject oEvent("XeWaterManager::RenderWater");

    ULONG           ulElementIdx, ulWorldMatrixIndex, ulDrawMask;
    XeRenderObject  *pObj;
    XeMaterial      *pOriginalMaterial;
    XeWaterPatchInfo *poPatchInfo;
    ULONG           ulMaxLightsPerPass = min(WM_MAX_LIGHTS_PER_PASS, g_oXeRenderer.GetMaxLightsPerPass(FALSE));
    XeWaterPatchSet *poPatchSet;
    UINT            uiNumTriangles = 0, uiNumDrawCalls = 0;
    BOOL            bFog;

    // Clear alpha channel
    g_oXeRenderer.ClearAlphaOnly(0x00000000);

    // Render alpha mask of water

    // setup material
    m_pWaterMaterial->SetZState(TRUE, TRUE);
    m_pWaterMaterial->SetAlphaBlend(FALSE);
    m_pWaterMaterial->SetColorWrite(FALSE);

    // setup custon VS features
    ULONG ulCurFeature;
    for (ulCurFeature = 1; ulCurFeature <= 10; ulCurFeature++)
    {
        m_pWaterMaterial->SetCustomVSFeature(ulCurFeature, 0);
    }
    m_pWaterMaterial->SetCustomVSFeature(11, 1); // ShaderId 1

    // setup custon PS features
    for (ulCurFeature = 1; ulCurFeature <= 10; ulCurFeature++)
    {
        m_pWaterMaterial->SetCustomPSFeature(ulCurFeature, 0);
    }
    m_pWaterMaterial->SetCustomPSFeature(11, 1); // ShaderId 1

    for(ulElementIdx = 0; ulElementIdx < ulNbWaterElements; ++ulElementIdx)
    {
        pObj                = _aoWaterList[ulElementIdx];
        poPatchSet          = &m_oStateRender.m_aoPatchSets[pObj->GetWaterPatchSetIndex()];
        ulWorldMatrixIndex  = pObj->GetWorldMatrixIndex();

        // save original material
        pOriginalMaterial   = pObj->GetMaterial();

        // get patch info
        ULONG ulPatchInfoId = m_oStateRender.m_aulPatchInfoIdList[poPatchSet->ulFirstPatchIndex];
        poPatchInfo         = UpdateWaterPatchInfo(pObj, ulPatchInfoId);

        // setup mesh
        pObj->SetMesh(poPatchInfo->poMesh);
        pObj->SetExtraDataIndex(ulPatchInfoId);
        pObj->SetMaterial(m_pWaterMaterial);

        // render first water patch
        g_oXeRenderer.RenderObject(pObj, XeFXManager::RP_DEFAULT);

        // render all other patches in a fast path
        for (ULONG ulCurPatch = 1; ulCurPatch < poPatchSet->ulPatchCount; ulCurPatch++)
        {
            // get patch info
            ulPatchInfoId = m_oStateRender.m_aulPatchInfoIdList[poPatchSet->ulFirstPatchIndex + ulCurPatch];
            poPatchInfo   = UpdateWaterPatchInfo(pObj, ulPatchInfoId);

            // setup mesh
            XeMesh *poMesh = poPatchInfo->poMesh;
            pObj->SetMesh(poMesh);
            pObj->SetExtraDataIndex(ulPatchInfoId);
            XeVertexShaderManager::XeVertexDeclaration *pCurrentDeclaration = g_oVertexShaderMgr.GetVertexDeclaration(poMesh);
            g_oRenderStateMgr.SetVertexDeclaration(pCurrentDeclaration->pDeclaration);
            g_oRenderStateMgr.SetStreamSource(0, *poMesh->GetStream(0));

            BOOL bStaticMesh;
            if (poMesh->GetStreamCount() > 1)
            {
                // dynamic mesh
                g_oRenderStateMgr.SetIndices(poMesh->GetIndices());
                g_oRenderStateMgr.SetStreamSource(1, *poMesh->GetStream(1));
                bStaticMesh = FALSE;
            }
            else
            {
                // static mesh
                g_oRenderStateMgr.SetIndices(NULL);
                g_oRenderStateMgr.SetStreamSource(1, NULL);
                bStaticMesh = TRUE;
            }

            // update managers
            g_oPixelShaderMgr.Update(pObj);
            g_oVertexShaderMgr.Update(pObj);

            g_oRenderStateMgr.Update(pObj, FALSE, TRUE);

            // draw primitive
            UINT             uiNumFaces       = pObj->GetPrimitiveCount();
            ULONG            ulVertexCount    = 0;
            D3DPRIMITIVETYPE ePrimitiveType   = pObj->GetPrimType();

#if defined(_XENON_RENDER_PC)
            ulVertexCount = poMesh->GetStream(0)->pBuffer->GetVertexCount();
#endif

            HRESULT hr;
            if (!bStaticMesh)
            {
                hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType, // prim type
                    0,              // base index
                    0,              // MinIndex - unused
                    ulVertexCount,  // NumVertices - unused
                    0,              // start index
                    uiNumFaces);
            }
            else 
            {
                hr = m_pD3DDevice->DrawPrimitive(   ePrimitiveType,            // prim type
                    0,                         // start vertex
                    uiNumFaces);
            }

            uiNumTriangles += uiNumFaces;
            uiNumDrawCalls++;
        }

        // restore original material
        pObj->SetMaterial(pOriginalMaterial);
    }

    // resolve back buffer in a scratch buffer to be used as a texture
    g_oXeTextureMgr.ResolveToScratchBuffer(XESCRATCHBUFFER0_ID);

    // Render the actual water

    m_pWaterMaterial->SetTextureId(5, g_oXeTextureMgr.GetDepthBufferID()); // Z texture

    m_pWaterMaterial->SetColorWrite(TRUE);

    // Multi-pass on each object
    for(ulElementIdx = 0; ulElementIdx < ulNbWaterElements; ++ulElementIdx)
    {
        pObj                = _aoWaterList[ulElementIdx];
        poPatchSet          = &m_oStateRender.m_aoPatchSets[pObj->GetWaterPatchSetIndex()];
        ulWorldMatrixIndex  = pObj->GetWorldMatrixIndex();
        ulDrawMask          = pObj->GetDrawMask();
        bFog                = g_pXeContextManagerRender->IsFogEnabled() && ((ulDrawMask & GDI_Cul_DM_Fogged) !=0 );
        
        // save original material
        pOriginalMaterial   = pObj->GetMaterial();
        
        // find light count for this pass
        ULONG ulLightSetIndex = pObj->GetLightSetIndex();
        ULONG ulTotalLights = min(g_pXeContextManagerRender->GetLightSetLightCount(ulLightSetIndex), g_oXeRenderer.GetMaxTotalLightCount());
        ULONG ulRemainingLights = ulTotalLights;
        ULONG ulCurPassLightCount = min(ulRemainingLights, ulMaxLightsPerPass);

        // setup material
        m_pWaterMaterial->SetZState(TRUE, FALSE);
        m_pWaterMaterial->SetAlphaBlend(FALSE);
        m_pWaterMaterial->SetConstantColor(pOriginalMaterial->GetConstantColor());
        m_pWaterMaterial->SetDiffuseColor(pOriginalMaterial->GetDiffuseColor());
        m_pWaterMaterial->SetSpecularColor(pOriginalMaterial->GetSpecularColor());
        m_pWaterMaterial->SetSpecularExponent(pOriginalMaterial->GetSpecularExponent());
        
        // setup custon VS features
        m_pWaterMaterial->SetCustomVSFeature(1, FALSE);
        ULONG ulColorSource = ConvertUVSource(pOriginalMaterial->GetUVSource());
        m_pWaterMaterial->SetCustomVSFeature(2, ulColorSource);

        ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
        pOriginalMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
        m_pWaterMaterial->SetCustomVSFeature(3, ul_BaseTC);
        m_pWaterMaterial->SetCustomVSFeature(4, ul_NormalTC);
        m_pWaterMaterial->SetCustomVSFeature(5, ul_DetailNMapTC);
        m_pWaterMaterial->SetCustomVSFeature(10, bFog ? 1 : 0);
        m_pWaterMaterial->SetCustomVSFeature(11, 0); // ShaderId 0

        // setup custom PS features
        m_pWaterMaterial->SetCustomPSFeature(1, FALSE);
        
        // use normal map if present
        LONG lMapId = pOriginalMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL);
        if ((lMapId != -1) && (ul_NormalTC == TEXTRANSFORM_NORMAL))
        {
            D3DXMATRIX stTexMatrix;
            pOriginalMaterial->GetTransform(XeMaterial::TEXTURE_NORMAL, &stTexMatrix, ulWorldMatrixIndex);
            m_pWaterMaterial->SetTransform(XeMaterial::TEXTURE_NORMAL, TRUE, &stTexMatrix);
        }
        m_pWaterMaterial->SetTextureId(3, lMapId);
        m_pWaterMaterial->SetCustomPSFeature(6, lMapId != -1 ? 1 : 0);
        
        // use detail map if present
        if (lMapId != -1)
        {
            lMapId = pOriginalMaterial->GetTextureId(XeMaterial::TEXTURE_DETAILNMAP);
            if ((lMapId != -1) && (ul_DetailNMapTC == TEXTRANSFORM_NORMAL))
            {
                D3DXMATRIX stTexMatrix;
                pOriginalMaterial->GetTransform(XeMaterial::TEXTURE_DETAILNMAP, &stTexMatrix, ulWorldMatrixIndex);
                m_pWaterMaterial->SetTransform(XeMaterial::TEXTURE_DETAILNMAP, TRUE, &stTexMatrix);
            }
            m_pWaterMaterial->SetTextureId(4, lMapId);
            m_pWaterMaterial->SetCustomPSFeature(7, lMapId != -1 ? 1 : 0);
        }
        else
        {
            // no detail map if no normal map
            m_pWaterMaterial->SetTextureId(4, -1);
            m_pWaterMaterial->SetCustomPSFeature(7, 0);
        }

        m_pWaterMaterial->SetCustomPSFeature(8, g_oPixelShaderMgr.IsGlobalMul2XEnabled() ? TRUE : (ulColorSource == COLORSOURCE_DIFFUSE2X));

        // setup base map
        lMapId = pOriginalMaterial->GetTextureId(XeMaterial::TEXTURE_BASE);
        if ((ulDrawMask & GDI_Cul_DM_UseTexture) && (lMapId != -1))
        {
            D3DXMATRIX stTexMatrix;
			pOriginalMaterial->GetTransform(XeMaterial::TEXTURE_BASE, &stTexMatrix, ulWorldMatrixIndex);
            m_pWaterMaterial->SetTransform(XeMaterial::TEXTURE_BASE, TRUE, &stTexMatrix);

            m_pWaterMaterial->SetTextureId(6, lMapId);
            m_pWaterMaterial->SetCustomPSFeature(9, 1);
        }
        else
        {
            m_pWaterMaterial->SetTextureId(6, -1);
            m_pWaterMaterial->SetCustomPSFeature(9, 0);
        }

        // fog
        m_pWaterMaterial->SetCustomPSFeature(10, bFog ? 1 : 0);
        m_pWaterMaterial->SetCustomPSFeature(11, 0); // ShaderId 0

        lMapId = pOriginalMaterial->GetTextureId(XeMaterial::TEXTURE_ENVIRONMENT);
        m_pWaterMaterial->SetTextureId(2, lMapId);

        pObj->SetMaterial(m_pWaterMaterial);
        g_pXeContextManagerRender->SetCurrentLightSet(ulLightSetIndex);

        for(ULONG uiLightIdx = 0; ((uiLightIdx < ulTotalLights) && (ulRemainingLights > 0)) || (ulTotalLights == 0); )
        { 
            // get patch info
            ULONG ulPatchInfoId = m_oStateRender.m_aulPatchInfoIdList[poPatchSet->ulFirstPatchIndex];
            poPatchInfo         = UpdateWaterPatchInfo(pObj, ulPatchInfoId);

            // setup mesh
            pObj->SetMesh(poPatchInfo->poMesh);
            pObj->SetExtraDataIndex(ulPatchInfoId);

            // set ps features according to patch info
            m_pWaterMaterial->SetCustomPSFeature(3, (lMapId != -1) && poPatchInfo->bReflectionEnabled ? 1 : 0);
            m_pWaterMaterial->SetCustomPSFeature(4, poPatchInfo->bRefractionEnabled);
            m_pWaterMaterial->SetCustomPSFeature(5, poPatchInfo->bReflectionEnabled && (m_ulReflectedObjects > 0));

            // setup VS for multi-light rendering
            m_pWaterMaterial->SetCustomPSFeature(2, ulCurPassLightCount);
            g_oPixelShaderMgr.SetPPLightCount(ulCurPassLightCount);
            g_oVertexShaderMgr.SetBaseLightIndex(uiLightIdx);

            for (ULONG ulCurLight = 0; ulCurLight < VS_MAX_LIGHTS_PER_PASS; ulCurLight++)
            {
                if (ulCurLight < ulCurPassLightCount)
                {
                    g_pXeContextManagerRender->SetPerPixelLightIndex(ulCurLight, uiLightIdx + ulCurLight);
                }
                else
                {
                    g_pXeContextManagerRender->SetPerPixelLightIndex(ulCurLight, -1);
                }
            }

            // Set light counts
            ULONG ulDirLightCount, ulOmniLightCount, ulSpotLightCount, ulCylSpotLightCount;
            g_oVertexShaderMgr.GetLightCountByType(g_pXeContextManagerRender->GetCurrentLightSet(),
                                                   uiLightIdx,
                                                   ulCurPassLightCount, 
                                                   ulDirLightCount, 
                                                   ulOmniLightCount, 
                                                   ulSpotLightCount, 
                                                   ulCylSpotLightCount);
            
            m_pWaterMaterial->SetCustomVSFeature(6, ulDirLightCount);
            m_pWaterMaterial->SetCustomVSFeature(7, ulOmniLightCount);
            m_pWaterMaterial->SetCustomVSFeature(8, ulSpotLightCount);
            m_pWaterMaterial->SetCustomVSFeature(9, ulCylSpotLightCount);

            // render first water patch
            g_oXeRenderer.RenderObject(pObj, XeFXManager::RP_APPLY_WATER);

            // render all other patches in a fast path
            for (ULONG ulCurPatch = 1; ulCurPatch < poPatchSet->ulPatchCount; ulCurPatch++)
            {
                // get patch info
                ulPatchInfoId = m_oStateRender.m_aulPatchInfoIdList[poPatchSet->ulFirstPatchIndex + ulCurPatch];
                poPatchInfo   = UpdateWaterPatchInfo(pObj, ulPatchInfoId);

                // check if we need/want reflection and refraction
                g_oPixelShaderMgr.SetFeatureCustom(3, (lMapId != -1) && poPatchInfo->bReflectionEnabled ? 1 : 0);
                g_oPixelShaderMgr.SetFeatureCustom(4, poPatchInfo->bRefractionEnabled);
                g_oPixelShaderMgr.SetFeatureCustom(5, poPatchInfo->bReflectionEnabled && (m_ulReflectedObjects > 0));                

                // setup mesh
                XeMesh *poMesh = poPatchInfo->poMesh;
                pObj->SetMesh(poMesh);
                pObj->SetExtraDataIndex(ulPatchInfoId);
                XeVertexShaderManager::XeVertexDeclaration *pCurrentDeclaration = g_oVertexShaderMgr.GetVertexDeclaration(poMesh);
                g_oRenderStateMgr.SetVertexDeclaration(pCurrentDeclaration->pDeclaration);
                g_oRenderStateMgr.SetStreamSource(0, *poMesh->GetStream(0));
                
                BOOL bStaticMesh;
                if (poMesh->GetStreamCount() > 1)
                {
                    // dynamic mesh
                    g_oRenderStateMgr.SetIndices(poMesh->GetIndices());
                    g_oRenderStateMgr.SetStreamSource(1, *poMesh->GetStream(1));
                    bStaticMesh = FALSE;
                }
                else
                {
                    // static mesh
                    g_oRenderStateMgr.SetIndices(NULL);
                    g_oRenderStateMgr.SetStreamSource(1, NULL);
                    bStaticMesh = TRUE;
                }

                // update managers
                g_oPixelShaderMgr.Update(pObj);
                g_oVertexShaderMgr.Update(pObj);

                g_oRenderStateMgr.Update(pObj, FALSE, TRUE);

                // draw primitive
                UINT             uiNumFaces       = pObj->GetPrimitiveCount();
                ULONG            ulVertexCount    = 0;
                D3DPRIMITIVETYPE ePrimitiveType   = pObj->GetPrimType();

              #if defined(_XENON_RENDER_PC)
                ulVertexCount = poMesh->GetStream(0)->pBuffer->GetVertexCount();
              #endif

                HRESULT hr;
                if (!bStaticMesh)
                {
                    hr = m_pD3DDevice->DrawIndexedPrimitive(ePrimitiveType, // prim type
                                                            0,              // base index
                                                            0,              // MinIndex - unused
                                                            ulVertexCount,  // NumVertices - unused
                                                            0,              // start index
                                                            uiNumFaces);
                }
                else 
                {
                    hr = m_pD3DDevice->DrawPrimitive(   ePrimitiveType,            // prim type
                                                        0,                         // start vertex
                                                        uiNumFaces);
                }

                uiNumTriangles += uiNumFaces;
                uiNumDrawCalls++;
            }

            // exit for loop if no lighting is done
            if (ulTotalLights == 0) break;

            ulRemainingLights -= ulCurPassLightCount;
            if (ulRemainingLights > 0)
            {                
                uiLightIdx += ulCurPassLightCount;
                ulCurPassLightCount = min(ulRemainingLights, ulMaxLightsPerPass);

                m_pWaterMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
                m_pWaterMaterial->SetCustomVSFeature(1, TRUE);
                m_pWaterMaterial->SetCustomPSFeature(1, TRUE);
            }
        }
        
        // restore original material
        pObj->SetMaterial(pOriginalMaterial);
    }

    g_oXeRenderer.AddToDrawCallCount(uiNumDrawCalls);
    g_oXeRenderer.AddToTriangleCount(uiNumTriangles);
}

// ------------------------------------------------------------------------------------------------
// Name   : UpdateWaterPathInfo
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeWaterManager::XeWaterPatchInfo* XeWaterManager::UpdateWaterPatchInfo(XeRenderObject *_pWaterObject, ULONG _ulPatchInfoId)
{
#if !defined(_XENON)
	if (_pWaterObject == NULL)
	{
		return NULL;
	}
#else
	ERR_X_Assert(_pWaterObject != NULL);
#endif

	// Fetch Patch Data

    XeWaterPatchInfo* poPatchInfo = GetWaterPatchInfoByIndex(_ulPatchInfoId);

#if !defined(_XENON)
	if (poPatchInfo == NULL)
	{
		return NULL;
	}
#else
	ERR_X_Assert(poPatchInfo != NULL);
#endif

    if (!poPatchInfo->bUpdated)
    {
        // transform reflection plane to view space
        D3DXMATRIX oMatrix, oInvTransMatrix;
        D3DXMATRIX *pWorldViewMatrix = g_pXeContextManagerRender->GetWorldViewMatrixByIndex(_pWaterObject->GetWorldViewMatrixIndex());
        D3DXVec3Transform(&poPatchInfo->oViewSpacePlaneOrigin, (D3DXVECTOR3*)&poPatchInfo->oPlaneOrigin, pWorldViewMatrix);
        D3DXMatrixInverse(&oInvTransMatrix, NULL, pWorldViewMatrix);
        D3DXMatrixTranspose(&oInvTransMatrix, &oInvTransMatrix);
        D3DXVec3TransformNormal((D3DXVECTOR3*)&poPatchInfo->oViewSpacePlaneEquation, (D3DXVECTOR3*)&poPatchInfo->oPlaneEquation, &oInvTransMatrix);

        // setup D3D clip plane (clip space) to keep only what's under the water plane
        D3DXVECTOR4 oOrigin;
        D3DXMATRIX *pProjectionMatrix = g_pXeContextManagerRender->GetProjectionMatrixByIndex(_pWaterObject->GetProjMatrixIndex());
        D3DXMatrixMultiply(&oMatrix, pWorldViewMatrix, pProjectionMatrix);
        D3DXMatrixInverse(&oInvTransMatrix, NULL, &oMatrix); 
        D3DXMatrixTranspose(&oInvTransMatrix, &oInvTransMatrix);
        D3DXVec3TransformCoord((D3DXVECTOR3*)&oOrigin, (D3DXVECTOR3*)&poPatchInfo->oPlaneOrigin, &oInvTransMatrix);
        D3DXVec3TransformNormal((D3DXVECTOR3*)&poPatchInfo->oClipSpacePlaneEquation, (D3DXVECTOR3*)&poPatchInfo->oPlaneEquation, &oInvTransMatrix);

        // inverse plane normal to keep what's under it
        poPatchInfo->oClipSpacePlaneEquation = -poPatchInfo->oClipSpacePlaneEquation;
        poPatchInfo->oClipSpacePlaneEquation.w = -poPatchInfo->oClipSpacePlaneEquation.x * oOrigin.x
                                                 -poPatchInfo->oClipSpacePlaneEquation.y * oOrigin.y
                                                 -poPatchInfo->oClipSpacePlaneEquation.z * oOrigin.z;

        // check if we need/want reflection and refraction
        FLOAT fAttenuation;
        fAttenuation  = fMax(WATER_EFFECT_MAX_DISTANCE - poPatchInfo->oViewSpacePlaneOrigin.z, 0.0f);
        fAttenuation  = fMin(fAttenuation, WATER_EFFECT_MAX_DISTANCE);
        fAttenuation /= WATER_EFFECT_MAX_DISTANCE;
        poPatchInfo->fRefractionIntensity = poPatchInfo->fRefractionFactor * fAttenuation;
        poPatchInfo->fReflectionIntensity = poPatchInfo->fReflectionFactor * fAttenuation;
        poPatchInfo->bRefractionEnabled   = poPatchInfo->fRefractionIntensity > 0.0f;
        poPatchInfo->bReflectionEnabled   = poPatchInfo->fReflectionIntensity > 0.0f;

        poPatchInfo->bUpdated = TRUE;
    }
 
    return poPatchInfo;
}
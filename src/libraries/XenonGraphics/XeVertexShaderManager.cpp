#include "precomp.h"

#include "XeVertexShaderManager.h"
#include "XePixelShaderManager.h"
#include "XeRenderStateManager.h"
#include "XeRenderer.h"
#include "XeShader.h"
#include "XeLightShaftManager.h"
#include "XeWeatherManager.h"
#include "XeShadowManager.h"
#include "XeShaderDatabase.h"
#include "XeContextManager.h"
#include "XeWaterManager.h"

#include "DevHelpers/XeBench.h"
#include "DevHelpers/XePerf.h"

#if defined(_DEBUG)
#define XE_VS_VERBOSE
#endif
 
// Unique instance of the vertex shader manager
XeVertexShaderManager g_oVertexShaderMgr;

LPDIRECT3DDEVICE9 XeShader<LPDIRECT3DVERTEXSHADER9, VS_GLOBAL_CONSTANT_COUNT>::m_pD3DDevice;

const CHAR XEVS_SHADER_ENTRY[]  = "VSMain";
const CHAR XEVS_SHADER_TARGET_2_0[] = "vs_2_0";
const CHAR XEVS_SHADER_TARGET_2_a[] = "vs_2_a";
const CHAR XEVS_SHADER_TARGET_3_0[] = "vs_3_0";
const CHAR XEVS_SHADER_FORMAT[] = 
    "#include \"ShaderCommon.hlsl\"\n"                                      \
    "#include \"VsGeneric.hlsl\"\n"                                         \
    "VSOUT VSMain( VSIN Input )\n"                                          \
    "{\n"                                                                   \
    "    return ProcessVertex( Input, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);\n"  \
    "}";

const CHAR XEVS_CUSTOM_SHADER_FORMAT[] = 
    "#include \"%s\"\n"                                                     \
    "VSOUT VSMain( VSIN Input )\n"                                          \
    "{\n"                                                                   \
    "    return ProcessVertex( Input, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d );\n"  \
    "}";

const CHAR XEVS_CUSTOM_SHADER_FORMAT_WITH_MATERIAL[] = 
"#include \"%s\"\n"                                                     \
"VSOUT VSMain( VSIN Input )\n"                                          \
"{\n"                                                                   \
"    return ProcessCustomVertexMAT( Input, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);\n"  \
"}";

const CHAR XEVS_WAVEYOURBODY_SHADER_FORMAT[] =
"#include \"%s\"\n"                                                     \
"VSOUT VSMain( VSIN Input )\n"                                          \
"{\n"                                                                   \
"    return ProcessCustomVertexWYB( Input, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);\n"  \
"}";

const CHAR XEVS_SYMMETRY_SHADER_FORMAT[] =
"#include \"%s\"\n"                                                     \
"VSOUT VSMain( VSIN Input )\n"                                          \
"{\n"                                                                   \
"    return ProcessCustomVertexSYM( Input, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);\n"  \
"}";

///// IMPORTANT : After adding a new custom shader, update the custom shader is defines in XeShader.h
const XeVSCustomShaderInfo XEVS_CUSTOM_SHADERS[] = 
{
    { NULL },                       // 0 NOT used (Generic shader)
    { "VSSPG22.hlsl" },             // 1 SPG2
    { "VsAfterEffects.hlsl" },      // 2 After Effect shader
    { "VsShadow.hlsl" },            // 3 Shadows
    { "VsLightShaft.hlsl" },        // 4 Light Shaft
    { "VsWater.hlsl" },             // 5 Water Effect
    { "VsHeatShimmer.hlsl" },       // 6 Heat shimmering
    { "VsFur.hlsl" },               // 7 Fur Effect
    { "VsRain.hlsl" },              // 8 Rain
    { "VsShadowQuad.hlsl" },        // 9 Shadow quad
    { "VsBlurShadow.hlsl" },        // 10 Shadow blurring
    { "VsWYB.hlsl" },               // 11 Wave Your Body
    { "VsSymmetry.hlsl" },          // 12 Symmetry modifier
    { "VsSprite.hlsl" },            // 13 Sprites
    { "VsReflection.hlsl" },        // 14 Reflection
};

const ULONG XEVS_NB_CUSTOM_SHADERS = sizeof(XEVS_CUSTOM_SHADERS) / sizeof(XeVSCustomShaderInfo);


// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeVertexShaderManager::XeVertexShaderManager() :
m_oCurrentKey           ( (ULONG128)-1, -1 )
{ 
#if !defined(XML_CONV_TOOL)
	InitShadersTable();
#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeVertexShaderManager::~XeVertexShaderManager()
{
#if !defined(XML_CONV_TOOL)
    UnloadAll();
#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::UnloadAll(void)
{
    UnloadAllShaders();

    // Vertex declarations
    for (ULONG i = 0; i < m_aoVertexDeclarations.size(); ++i)
    {
        SAFE_RELEASE(m_aoVertexDeclarations[i].pDeclaration);
    }
    m_aoVertexDeclarations.resize(0);
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::UnloadAllShaders(void)
{
    // Shaders
    MapVertexShader::iterator it = m_mapLoadedShaders.begin();
    while (it != m_mapLoadedShaders.end())
    {
        SAFE_DELETE(it->second);
        it++;
    }
    m_mapLoadedShaders.clear();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::ReInit()
{
    m_pD3DDevice                    = g_oXeRenderer.GetDevice();
    XeVertexShader::m_pD3DDevice    = m_pD3DDevice;

    ResetAllFeatures();

    // load all vertex shaders
    UnloadAll();

    D3DXMatrixIdentity(&m_oWorldViewProj);

    m_iMaxWeight = 4;

    m_iLightBase = 0;
 
    g_pXeContextManagerEngine->ResetFogParams();

    m_ulActiveAmbient = 0;

    m_bReflectionEnabled = FALSE;

    m_ignoreHalfViewportTexel = FALSE;
    m_isRimLightEnabled = FALSE;

    g_pXeContextManagerEngine->SetGlobalRLIScaleAndOffset(1.0f, 0.0f);
    
    m_vZoomBlurRatios.x = m_vZoomBlurRatios.y = 1.0f;

    m_fBigBlurFactor = 0.0f;

    m_oDirtyFlags.DirtyAll();

    InvalidateFeatureKey();
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::OnDeviceLost
// Params : None
// RetVal : Success
// Descr. : Before IDirect3DDevice::Reset()
// ------------------------------------------------------------------------------------------------
BOOL XeVertexShaderManager::OnDeviceLost(void)
{
    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::OnDeviceReset
// Params : None
// RetVal : Success
// Descr. : After IDirect3DDevice::Reset()
// ------------------------------------------------------------------------------------------------
BOOL XeVertexShaderManager::OnDeviceReset(void)
{
    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::Shutdown
// Params : None
// RetVal : None
// Descr. : Shutdown the pixel shader manager
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::Shutdown(void)
{
    UnloadAll();

    m_pD3DDevice = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
BOOL XeVertexShaderManager::GenerateShader(XeRenderObject *_poObject,  XeVSFeature _oFeatures, ULONG _ulVertexFormatHandle, ULONG64 _ulOutputSemantics )
{
    XeShaderMacro oMacros;
    ULONG         ulFlags         = 0;
    CHAR          szShaderEntry[512];
    ULONG         ulExtraFlags = _poObject->GetExtraFlags();

    // Compilation flags
#if defined(_XENON)
  #if (_XDK_VER >= 1640)
    // SDALLAIRE : use old back-end since the new one sucks for now
    ulFlags |= D3DXSHADER_SKIPVALIDATION | D3DXSHADER_MICROCODE_BACKEND_OLD;
  #endif
#endif

    // Macro settings
    oMacros.Clear();

#if defined(_XENON_RENDER_PC)
    if (!HasBaseTexCoordInFormat(_ulVertexFormatHandle))
    {
        oMacros.ulVSNoColor = 1;
    }
    if (!HasNormalInFormat(_ulVertexFormatHandle))
    {
        oMacros.ulVSNoNormal = 1;
    }
    if (!HasColorInFormat(_ulVertexFormatHandle))
    {
        oMacros.ulVSNoTexCoord = 1;
    }
#endif

    oMacros.ulVSBonesConfig  = _oFeatures.ulBonesConfig;
    oMacros.ulVSLightsConfig = _oFeatures.ulLightsConfig;
    oMacros.ulVSOutputSemantics = _ulOutputSemantics;

    // Allow flow control usage if we are doing vertex lighting
    if (((_oFeatures.ulCustomShader == 0) || (_oFeatures.ulCustomShader == CUSTOM_VS_FUR)) && 
         (_oFeatures.ulLighting == LIGHTING_VERTEX))
    {
        ulFlags &= ~D3DXSHADER_AVOID_FLOW_CONTROL;
    }


    if ((_oFeatures.ulCustomShader == 0) && 
        (_oFeatures.iSPG2Mode      == 0) && // SPG2 require a vertex preprocess
        (_oFeatures.bWaveYourBody  == 0) &&
        ((ulExtraFlags & QMFR_SYMMETRY) == 0))
    {
        sprintf(szShaderEntry, XEVS_SHADER_FORMAT, 
                (ULONG)_oFeatures.ulTransform, 
                (ULONG)_oFeatures.ulLighting, 
                (ULONG)_oFeatures.ulDirLightCount,
                (ULONG)_oFeatures.ulOmniLightCount,
                (ULONG)_oFeatures.ulSpotLightCount,
                (ULONG)_oFeatures.ulCylSpotLightCount,
                (ULONG)_oFeatures.ulColorSource, 
                (ULONG)_oFeatures.ulBaseMapTransform, 
                (ULONG)_oFeatures.ulUVSource, 
                (ULONG)_oFeatures.bFogEnable, 
                (ULONG)_oFeatures.ulNormalMapTransform, 
                (ULONG)_oFeatures.ulSpecularMapTransform,
                (ULONG)_oFeatures.ulDetailNMapTransform,
                (ULONG)_oFeatures.bAddAmbient,
                (ULONG)_oFeatures.bAddRLI,
                (ULONG)_oFeatures.bRLIScaleAndOffset,
                (ULONG)_oFeatures.ulReflectionType,
                (ULONG)_oFeatures.bRimLightEnable,
                (ULONG)_oFeatures.bRimLightHeightAttenuation,
                (ULONG)_oFeatures.bDepthToColorEnable,
                (ULONG)_oFeatures.bInvertMossMode,
				(ULONG)_oFeatures.bOffsetMap,
                (ULONG)_oFeatures.iSPG2Mode,
                (ULONG)_oFeatures.bShadows,
                (ULONG)_oFeatures.bVertexSpecular);
    }
    else
    {
        XeValidateRet((_oFeatures.ulCustomShader < XEVS_NB_CUSTOM_SHADERS), FALSE, "Invalid custom shader index");

        if (_oFeatures.ulCustomShader == CUSTOM_VS_FUR)
        {
             // use regular features
            sprintf(szShaderEntry, XEVS_CUSTOM_SHADER_FORMAT_WITH_MATERIAL,
                    XEVS_CUSTOM_SHADERS[_oFeatures.ulCustomShader].szFileName,
                    (ULONG)_oFeatures.ulTransform, 
                    (ULONG)_oFeatures.ulLighting, 
                    (ULONG)_oFeatures.ulDirLightCount,
                    (ULONG)_oFeatures.ulOmniLightCount,
                    (ULONG)_oFeatures.ulSpotLightCount,
                    (ULONG)_oFeatures.ulCylSpotLightCount,
                    (ULONG)_oFeatures.ulColorSource, 
                    (ULONG)_oFeatures.ulBaseMapTransform, 
                    (ULONG)_oFeatures.ulUVSource, 
                    (ULONG)_oFeatures.bFogEnable, 
                    (ULONG)_oFeatures.ulNormalMapTransform, 
                    (ULONG)_oFeatures.ulSpecularMapTransform,
                    (ULONG)_oFeatures.ulDetailNMapTransform,
                    (ULONG)_oFeatures.bAddAmbient,
                    (ULONG)_oFeatures.bAddRLI,
                    (ULONG)_oFeatures.bRLIScaleAndOffset,
                    (ULONG)_oFeatures.ulReflectionType,
                    (ULONG)_oFeatures.bRimLightEnable,
                    (ULONG)_oFeatures.bRimLightHeightAttenuation,
                    (ULONG)_oFeatures.bDepthToColorEnable,
                    (ULONG)_oFeatures.bInvertMossMode,
                    (ULONG)_oFeatures.iSPG2Mode,
                    (ULONG)_oFeatures.bShadows,
                    (ULONG)_oFeatures.bVertexSpecular);
        }
        else if (_oFeatures.iSPG2Mode > 0)
        {
            // Always avoid flow control on spg2 so z-prepass and lighting pass generate same vertex output
            /*if (_oFeatures.ulLighting != LIGHTING_VERTEX)
            {
              //ulFlags |= D3DXSHADER_AVOID_FLOW_CONTROL;
            }*/

            sprintf(szShaderEntry, XEVS_CUSTOM_SHADER_FORMAT_WITH_MATERIAL,
                    XEVS_CUSTOM_SHADERS[1].szFileName,
                    (ULONG)_oFeatures.ulTransform, 
                    (ULONG)_oFeatures.ulLighting, 
                    (ULONG)_oFeatures.ulDirLightCount,
                    (ULONG)_oFeatures.ulOmniLightCount,
                    (ULONG)_oFeatures.ulSpotLightCount,
                    (ULONG)_oFeatures.ulCylSpotLightCount,
                    (ULONG)_oFeatures.ulColorSource, 
                    (ULONG)_oFeatures.ulBaseMapTransform, 
                    (ULONG)_oFeatures.ulUVSource, 
                    (ULONG)_oFeatures.bFogEnable, 
                    (ULONG)_oFeatures.ulNormalMapTransform, 
                    (ULONG)_oFeatures.ulSpecularMapTransform,
                    (ULONG)_oFeatures.ulDetailNMapTransform,
                    (ULONG)_oFeatures.bAddAmbient,
                    (ULONG)_oFeatures.bAddRLI,
                    (ULONG)_oFeatures.bRLIScaleAndOffset,
                    (ULONG)_oFeatures.ulReflectionType,
                    (ULONG)_oFeatures.bRimLightEnable,
                    (ULONG)_oFeatures.bRimLightHeightAttenuation,
                    (ULONG)_oFeatures.bDepthToColorEnable,
                    (ULONG)_oFeatures.bInvertMossMode,
                    (ULONG)_oFeatures.iSPG2Mode,
                    (ULONG)_oFeatures.bShadows,
                    (ULONG)_oFeatures.bVertexSpecular);
        }
        else if (_oFeatures.bWaveYourBody)
        {
            sprintf(szShaderEntry, XEVS_WAVEYOURBODY_SHADER_FORMAT,
                    XEVS_CUSTOM_SHADERS[CUSTOM_VS_WYB].szFileName,
                    (ULONG)_oFeatures.ulNumWYB,
                    (ULONG)_oFeatures.ulTransform, 
                    (ULONG)_oFeatures.ulLighting, 
                    (ULONG)_oFeatures.ulDirLightCount,
                    (ULONG)_oFeatures.ulOmniLightCount,
                    (ULONG)_oFeatures.ulSpotLightCount,
                    (ULONG)_oFeatures.ulCylSpotLightCount,
                    (ULONG)_oFeatures.ulColorSource, 
                    (ULONG)_oFeatures.ulBaseMapTransform, 
                    (ULONG)_oFeatures.ulUVSource, 
                    (ULONG)_oFeatures.bFogEnable, 
                    (ULONG)_oFeatures.ulNormalMapTransform, 
                    (ULONG)_oFeatures.ulSpecularMapTransform,
                    (ULONG)_oFeatures.ulDetailNMapTransform,
                    (ULONG)_oFeatures.bAddAmbient,
                    (ULONG)_oFeatures.bAddRLI,
                    (ULONG)_oFeatures.bRLIScaleAndOffset,
                    (ULONG)_oFeatures.ulReflectionType,
                    (ULONG)_oFeatures.bRimLightEnable,
                    (ULONG)_oFeatures.bRimLightHeightAttenuation,
                    (ULONG)_oFeatures.bDepthToColorEnable,
                    (ULONG)_oFeatures.bInvertMossMode,
                    (ULONG)_oFeatures.iSPG2Mode,
                    (ULONG)_oFeatures.bShadows,
                    (ULONG)_oFeatures.bVertexSpecular);
        }
        else if (_oFeatures.ulCustomShader == CUSTOM_VS_SYMMETRY)
        {
            sprintf(szShaderEntry, XEVS_SYMMETRY_SHADER_FORMAT,
                    XEVS_CUSTOM_SHADERS[CUSTOM_VS_SYMMETRY].szFileName,
                    (ULONG)_oFeatures.ulTransform, 
                    (ULONG)_oFeatures.ulLighting, 
                    (ULONG)_oFeatures.ulDirLightCount,
                    (ULONG)_oFeatures.ulOmniLightCount,
                    (ULONG)_oFeatures.ulSpotLightCount,
                    (ULONG)_oFeatures.ulCylSpotLightCount,
                    (ULONG)_oFeatures.ulColorSource, 
                    (ULONG)_oFeatures.ulBaseMapTransform, 
                    (ULONG)_oFeatures.ulUVSource, 
                    (ULONG)_oFeatures.bFogEnable, 
                    (ULONG)_oFeatures.ulNormalMapTransform, 
                    (ULONG)_oFeatures.ulSpecularMapTransform,
                    (ULONG)_oFeatures.ulDetailNMapTransform,
                    (ULONG)_oFeatures.bAddAmbient,
                    (ULONG)_oFeatures.bAddRLI,
                    (ULONG)_oFeatures.bRLIScaleAndOffset,
                    (ULONG)_oFeatures.ulReflectionType,
                    (ULONG)_oFeatures.bRimLightEnable,
                    (ULONG)_oFeatures.bRimLightHeightAttenuation,
                    (ULONG)_oFeatures.bDepthToColorEnable,
                    (ULONG)_oFeatures.bInvertMossMode,
                    (ULONG)_oFeatures.iSPG2Mode,
                    (ULONG)_oFeatures.bShadows,
                    (ULONG)_oFeatures.bVertexSpecular,
                    (ULONG)_oFeatures.ulSymmetryMode);
        }
        else
        {
            sprintf(szShaderEntry, XEVS_CUSTOM_SHADER_FORMAT,
                    XEVS_CUSTOM_SHADERS[_oFeatures.ulCustomShader].szFileName,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature1,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature2,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature3,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature4,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature5,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature6,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature7,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature8,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature9,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature10,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature11,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature12,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature13,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature14,
                    (ULONG)_oFeatures.stCustomShaderFeatures.ulFeature15);
					//(ULONG)_oFeatures.stCustomShaderFeatures.ulFeature16);
        }
    }

    // Request compilation of that shader
    g_oXeShaderDatabase.RequestVertexShader(*(const ULONG128*)&_oFeatures.ulKey, szShaderEntry, 
                                            XEVS_SHADER_ENTRY, ulFlags, oMacros);

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeVertexShader* XeVertexShaderManager::CreateShader(const VertexShaderKey& _ulKey, const void* _pShaderCode, ID3DXConstantTable* _pConstantTable, CHAR* _szName)
{
    ERR_X_Assert(_pShaderCode != NULL);

    XeVertexShader* pShader = NULL;

    pShader = new XeVertexShader();
    if (pShader == NULL)
        return NULL;

    if (_pConstantTable)
    {
        pShader->SetConstantTable(_pConstantTable);
    }

    if( (pShader == NULL) || (pShader->ReInit((DWORD *)_pShaderCode) == FALSE))
    {
        SAFE_DELETE(pShader);
        return NULL;
    }

#ifdef _DEBUG
    pShader->SetFilename(_szName);
#endif

    return pShader;
}

void XeVertexShaderManager::AddShader(const VertexShaderKey& _ulKey, XeVertexShader* _pShader)
{
    MapVertexShader::iterator it = m_mapLoadedShaders.find(_ulKey);
    _pShader->SetOutputSemantics( _ulKey.ulVSOuputSemantics );

    if (it != m_mapLoadedShaders.end())
    {
        SAFE_DELETE(it->second);

        it->second = _pShader;
    }
    else
    {
        m_mapLoadedShaders[_ulKey] = _pShader;
    }
}

void XeVertexShaderManager::AddCreateShader(const VertexShaderKey& _ulKey, const void* _pCode, ID3DXConstantTable* _pConstantTable)
{
    XeVertexShader* pVertexShader;
    CHAR            szShaderName[64];

    sprintf(szShaderName, "0x%016I64x%016I64x.hlsl", _ulKey.ulFeatureKey.part1, _ulKey.ulFeatureKey.part2);

    pVertexShader = CreateShader(_ulKey, _pCode, _pConstantTable, szShaderName);

    AddShader(_ulKey, pVertexShader);
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::Update(XeRenderObject *_pObject)
{
    XeMesh *poMesh = _pObject->GetMesh();

    // send vertex stream states
#if defined(_XENON_RENDER_PC)
    // Quad list are not supported on PC, we'll use a special index buffer instead
    if (_pObject->GetPrimType() == D3DPT_QUADLIST)
    {
        g_oRenderStateMgr.SetIndices(g_XeBufferMgr.GetSpriteIB());
    }
    else
#endif
    {
        g_oRenderStateMgr.SetIndices(poMesh->GetIndices());
    }

    // send stream info
    for (ULONG ulCurStream = 0; ulCurStream < poMesh->GetStreamCount(); ulCurStream++)
    {
        g_oRenderStateMgr.SetStreamSource(ulCurStream, *poMesh->GetStream(ulCurStream));
    }

    // WYB
    INT iWYB1, iWYB2;
    _pObject->GetWYBIndices(iWYB1, iWYB2);
    m_oFeatures.bWaveYourBody = (iWYB1 != -1);
    if(m_oFeatures.bWaveYourBody)
    {
        m_oFeatures.ulCustomShader = CUSTOM_VS_WYB;
        m_oFeatures.ulNumWYB = (iWYB2 == -1) ? 1 : 2;
    }
    else
    {
        m_oFeatures.bWaveYourBody = 0;
    }

    // Set the custom shader to symmetry if needed
    if (_pObject->GetExtraFlags() & QMFR_SYMMETRY)
    {
        m_oFeatures.ulCustomShader = CUSTOM_VS_SYMMETRY;
        m_oFeatures.ulSymmetryMode = g_pXeContextManagerRender->GetSymmetryParams(_pObject->GetSymmetryIndex())->ulAxis;
    }

#if defined(_XENON_RENDER_PC)
    XeVertexShaderManager::XeVertexDeclaration *pDeclarationObject = GetVertexDeclaration(poMesh);
    m_oFeatures.bVertexHasColor        = pDeclarationObject->eAttributes.bHasColor;
    m_oFeatures.bVertexHasNormal       = pDeclarationObject->eAttributes.bHasNormal;
    m_oFeatures.bVertexHasBaseTexCoord = pDeclarationObject->eAttributes.bHasBaseTexCoord;
#endif

    // Pixel shader must be set before vertex shader
    XePixelShader * pCurrentPixelShader = g_oRenderStateMgr.GetCurrentPixelShader();

    // select and send vertex shader    
    // look for cached vertex shader for current key
    VertexShaderKey key( m_oFeatures.ulKey, (ULONG64)-1);
    BOOL b_PixelShaderValid = FALSE;
    if( pCurrentPixelShader == NULL )
    {
        key.ulVSOuputSemantics = 0;
        b_PixelShaderValid = TRUE;
    }
    else if( !pCurrentPixelShader->IsDefault() )
    {
        key.ulVSOuputSemantics = pCurrentPixelShader->GetInputSemantics();
        b_PixelShaderValid = TRUE;
    }

    // Vertex shader is already set
    if( key == m_oCurrentKey )
    {
        return;
    }

    BOOL b_ShaderFound = FALSE;

    MapVertexShader::iterator it = m_mapLoadedShaders.find( key );
    if( it != m_mapLoadedShaders.end() )
    {
        if( !it->second->IsDefault() || 
            it->second->IsRequestQueued() ||
            !b_PixelShaderValid )
        {
            // use cached vertex shader
            g_oRenderStateMgr.SetVertexShader(it->second);
            m_oCurrentKey = key;
            b_ShaderFound = TRUE;
        }
    }

    if( !b_ShaderFound )
    {
        if( !b_PixelShaderValid )
        {
            // Wait until we have an associated pixel shader before generating this vertex shader.
            // The reason is that we need the input semantics of the pixel shader to generate the vertex 
            // shader output struct. Use a duplicated default until then. 
            XeVertexShader* pDupVS = g_oXeShaderDatabase.GetDefaultVertexShader()->Duplicate();

            g_oVertexShaderMgr.AddShader(key, pDupVS);
            g_oRenderStateMgr.SetVertexShader(pDupVS);
            m_oCurrentKey = key;
        }
        else
        {
            if (GenerateShader(_pObject, m_oFeatures, poMesh->GetVertexFormatHandle(), key.ulVSOuputSemantics))
            {
                it = m_mapLoadedShaders.find( key );
                ERR_X_Assert( it != m_mapLoadedShaders.end() );

                g_oRenderStateMgr.SetVertexShader(it->second);
                m_oCurrentKey = key;
            }
            else
            {
                _breakpoint_;
            }
        }
    }
    
    g_pXeContextManagerRender->DirtyAllConstants();
    DirtyAllConstants();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::UpdateShaderConstantsSPG2(XeVertexShader *_pShader, XeRenderObject* _pObject, BOOL bSet)
{
    //static tdstSPG2ShaderConsts stSPG2ShaderConsts;
    
    tdstSPG2ShaderConsts *pSPG2Const = _pObject->GetSPG2ShadersConstsPointer();

    /*if(bSet)
    {
        L_memset(&stSPG2ShaderConsts, 0xff, sizeof(tdstSPG2ShaderConsts));
    }*/

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_UVMANIP))
    {
        //if(stSPG2ShaderConsts.m_vSPG2UVManip.component != pSPG2Const->m_vSPG2UVManip.component)
        {
            _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_UVMANIP, &pSPG2Const->m_vSPG2UVManip);
            //stSPG2ShaderConsts.m_vSPG2UVManip = pSPG2Const->m_vSPG2UVManip;
        }
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_GLOBALPOS))
    {
        //if(stSPG2ShaderConsts.m_vSPG2GlobalPos.component != pSPG2Const->m_vSPG2GlobalPos.component)
        {
            _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_GLOBALPOS, &pSPG2Const->m_vSPG2GlobalPos);
            //stSPG2ShaderConsts.m_vSPG2GlobalPos = pSPG2Const->m_vSPG2GlobalPos;
        }
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_GLOBALZADD))
    {
        //if(stSPG2ShaderConsts.m_vSPG2GlobalZAdd.component != pSPG2Const->m_vSPG2GlobalZAdd.component)
        {
            _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_GLOBALZADD, &pSPG2Const->m_vSPG2GlobalZAdd);
            //stSPG2ShaderConsts.m_vSPG2GlobalZAdd = pSPG2Const->m_vSPG2GlobalZAdd;
        }
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_WIND))
    {
        //if(bSet)
        {
            _pShader->SetGlobalVectorArray(VS_GLOBAL_CONSTANT_SPG2_WIND, pSPG2Const->m_avSPG2Wind, 8);
        }
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_XCAM))
    {
        //if(stSPG2ShaderConsts.m_vSPG2XCam.component != pSPG2Const->m_vSPG2XCam.component)
        {
            _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_XCAM, &pSPG2Const->m_vSPG2XCam);
            //stSPG2ShaderConsts.m_vSPG2XCam = pSPG2Const->m_vSPG2XCam;
        }
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_YCAM))
    {
        //if(stSPG2ShaderConsts.m_vSPG2YCam.component != pSPG2Const->m_vSPG2YCam.component)
        {
            _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_YCAM, &pSPG2Const->m_vSPG2YCam);
            //stSPG2ShaderConsts.m_vSPG2YCam = pSPG2Const->m_vSPG2YCam;
        }
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_GLOBALCOLORWIND))
    {
        VECTOR4FLOAT vColorWind;
        vColorWind.w = 0.0f; // tell shader to skip this parameter.. only used for grid

        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_GLOBALCOLORWIND, &vColorWind);
    }

    /*
    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_MIXED1))
    {
        VECTOR4FLOAT vMixed1;
        
        //vMixed1.x = pSPG2Const->m_fSPG2Ratio;
        vMixed1.y = pSPG2Const->m_fSPG2OoNumOfSeg;
        //vMixed1.z = pSPG2Const->m_fSPG2GlobalScale;
        vMixed1.w = pSPG2Const->m_fSPG2Trapeze;

        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_MIXED1, &vMixed1);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_MIXED2))
    {
        VECTOR4FLOAT vMixed2;

        vMixed2.x = pSPG2Const->m_fSPG2TrapezeDelta;
        vMixed2.y = pSPG2Const->m_fSPG2EOHP;
        vMixed2.z = pSPG2Const->m_fSPG2SpriteRadius;
        vMixed2.w = 0.0f; // unused

        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SPG2_MIXED2, &vMixed2);
    }*/

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_RATIO))
    {
        _pShader->SetGlobalFloat(VS_GLOBAL_CONSTANT_SPG2_RATIO, pSPG2Const->m_fSPG2Ratio);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_GLOBALSCALE))
    {
        _pShader->SetGlobalFloat(VS_GLOBAL_CONSTANT_SPG2_GLOBALSCALE, pSPG2Const->m_fSPG2GlobalScale);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_NUMSEG))
    {
        _pShader->SetGlobalFloat(VS_GLOBAL_CONSTANT_SPG2_NUMSEG, pSPG2Const->m_fSPG2OoNumOfSeg);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_TRAPEZE))
    {
        _pShader->SetGlobalFloat(VS_GLOBAL_CONSTANT_SPG2_TRAPEZE, pSPG2Const->m_fSPG2Trapeze);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_TRAPEZEDELTA))
    {
        _pShader->SetGlobalFloat(VS_GLOBAL_CONSTANT_SPG2_TRAPEZEDELTA, pSPG2Const->m_fSPG2TrapezeDelta);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_EOHP))
    {
        _pShader->SetGlobalFloat(VS_GLOBAL_CONSTANT_SPG2_EOHP, pSPG2Const->m_fSPG2EOHP);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SPG2_SPRITERADIUS))
    {
        _pShader->SetGlobalFloat(VS_GLOBAL_CONSTANT_SPG2_SPRITERADIUS, pSPG2Const->m_fSPG2SpriteRadius);
    }

   /* if(bSet)
    {
        L_memcpy(&stSPG2ShaderConsts, pSPG2Const, sizeof(tdstSPG2ShaderConsts));
    }*/
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::UpdateShaderConstants(XeVertexShader *_pShader, XeRenderObject* _pObject)
{
    ERR_X_Assert(_pShader != NULL);
    ERR_X_Assert(_pObject != NULL);

    XeContextManager::DirtyFlags _oDirtyFlags;
    g_pXeContextManagerRender->GetDirtyFlags(_oDirtyFlags);

    UINT       iMaxBones            = _pObject->GetMesh()->GetMaxBones();
    XeMaterial *pMaterial           = _pObject->GetMaterial();
    D3DXMATRIX *pProjMatrix         = g_pXeContextManagerRender->GetProjectionMatrixByIndex(g_pXeContextManagerRender->GetCurrentProjMatrixActiveIndex());
    D3DXMATRIX *pWorldViewMatrix    = g_pXeContextManagerRender->GetWorldViewMatrixByIndex(g_pXeContextManagerRender->GetCurrentWorldViewMatrixActiveIndex());

    // update matrices... to be optimized    
    D3DXMatrixMultiply(&m_oWorldViewProj, pWorldViewMatrix, pProjMatrix);

    // update all constants
    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_WORLDVIEWPROJ) && _oDirtyFlags.bWorldViewProj)
    {
        _pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_WORLDVIEWPROJ, &m_oWorldViewProj);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_WORLDVIEW) && _oDirtyFlags.bWorldView )
    {
        _pShader->SetGlobalMatrixAsFloat4x3(VS_GLOBAL_CONSTANT_WORLDVIEW, pWorldViewMatrix);

        if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_INVTRANSWORLDVIEW))
        {
            D3DXMATRIX oInvTransWorldView;

            D3DXMatrixInverse(&oInvTransWorldView, NULL, pWorldViewMatrix);
            // Already transposed for column-major
            _pShader->SetGlobalRawData(VS_GLOBAL_CONSTANT_INVTRANSWORLDVIEW, (VECTOR4FLOAT*)&oInvTransWorldView, 3);
        }
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_INVTRANSWORLD) && _oDirtyFlags.bWorldView)
    {
        D3DXMATRIX oInvTransWorld;

        D3DXMatrixInverse(&oInvTransWorld, NULL, _pObject->GetWorldMatrix());
        // Already transposed for column-major
        _pShader->SetGlobalRawData(VS_GLOBAL_CONSTANT_INVTRANSWORLD, (VECTOR4FLOAT*)&oInvTransWorld, 3);

        if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_WORLDSPACE_PLANE_NORMAL))
        {
            VECTOR4FLOAT oPlaneNormal;

            oPlaneNormal.x = oInvTransWorld._13;
            oPlaneNormal.y = oInvTransWorld._23;
            oPlaneNormal.z = oInvTransWorld._33;

            _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_WORLDSPACE_PLANE_NORMAL, &oPlaneNormal);
        }
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_PROJECTION) && _oDirtyFlags.bProjection )
    {
        _pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_PROJECTION, pProjMatrix);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SKINNING) && _oDirtyFlags.bSkinning)
    {
        if (g_pXeContextManagerRender->GetCurrentSkinningSet()->ulSkinningMatrixCount > 0)
        {
            Float4x3 * pSkinningMatrix= g_pXeContextManagerRender->GetSkinningMatrixByIndex( g_pXeContextManagerRender->GetCurrentSkinningSet()->ulSkinningMatrixIndex );
            _pShader->SetGlobalRawData(VS_GLOBAL_CONSTANT_SKINNING, (VECTOR4FLOAT*)pSkinningMatrix, g_pXeContextManagerRender->GetCurrentSkinningSet()->ulSkinningMatrixCount * 3);
        }
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_MAXWEIGHT) && m_oDirtyFlags.bMaxWeight)
    {
        _pShader->SetGlobalInt(VS_GLOBAL_CONSTANT_MAXWEIGHT,  m_iMaxWeight);
        //ERR_OutputDebugString("Xenon --- Rendering skinned mesh with %i weights.\n", m_iMaxWeight);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SCALING))
    {
        D3DXMATRIX *pWorldMatrix = g_pXeContextManagerRender->GetWorldMatrixByIndex(_pObject->GetWorldMatrixIndex());
        FLOAT afScaling[4];

        afScaling[0] = sqrt(pWorldMatrix->_11*pWorldMatrix->_11 + pWorldMatrix->_21*pWorldMatrix->_21 + pWorldMatrix->_31*pWorldMatrix->_31);
        afScaling[1] = sqrt(pWorldMatrix->_12*pWorldMatrix->_12 + pWorldMatrix->_22*pWorldMatrix->_22 + pWorldMatrix->_32*pWorldMatrix->_32);
        afScaling[2] = sqrt(pWorldMatrix->_11*pWorldMatrix->_13 + pWorldMatrix->_23*pWorldMatrix->_23 + pWorldMatrix->_33*pWorldMatrix->_33);
        afScaling[3] = 1.0f;

        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_SCALING, (VECTOR4FLOAT*) &afScaling[0]);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_FOG))
    {
        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_FOG, g_pXeContextManagerRender->GetVertexFogParams() );

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_FOG_PITCH_ATTENUATION))
		{
	        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_FOG_PITCH_ATTENUATION, g_pXeContextManagerRender->GetVertexFogPitchAttenuation());
	    }
    }
    
    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_FOG_PROJECTION) )
    {
        VECTOR4FLOAT oFogProjection;

        float fNear, fFar, fFOVX, fFOVY;
        g_pXeContextManagerRender->GetCameraParams( &fNear, &fFar, &fFOVX, &fFOVY );

        oFogProjection.x = 2.0f * tan( fFOVX / 2.0f );
        oFogProjection.y = 2.0f * tan( fFOVY / 2.0f );
        oFogProjection.z = 1.0f;
        oFogProjection.w = 0.0f;

        _pShader->SetGlobalVector( VS_GLOBAL_CONSTANT_FOG_PROJECTION, &oFogProjection );
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_CAMERA) )
    {
        // Camera transformation matrix
        _pShader->SetGlobalMatrixAsFloat4x3( VS_GLOBAL_CONSTANT_CAMERA, (D3DXMATRIX *) &g_pXeContextManagerRender->st_CameraMatrix );
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_TEXTUREMATRIX0))
    {
        D3DXMATRIX stTexMatrix;

        pMaterial->GetTransform(0, &stTexMatrix);

        _pShader->SetGlobalMatrixAsFloat4x2(VS_GLOBAL_CONSTANT_TEXTUREMATRIX0, &stTexMatrix);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_TANGENTTRANSFORM))
    {
        D3DXMATRIX stTexMatrix;

        pMaterial->GetTangentTransform(&stTexMatrix);
        _pShader->SetGlobalMatrixAsFloat4x3(VS_GLOBAL_CONSTANT_TANGENTTRANSFORM, &stTexMatrix);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_TEXTUREMATRIX1))
    {
        D3DXMATRIX stTexMatrix;

        pMaterial->GetTransform(1, &stTexMatrix);

        _pShader->SetGlobalMatrixAsFloat4x2(VS_GLOBAL_CONSTANT_TEXTUREMATRIX1, &stTexMatrix);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_TEXTUREMATRIX2))
    {
        D3DXMATRIX stTexMatrix;

        pMaterial->GetTransform(2, &stTexMatrix);

        _pShader->SetGlobalMatrixAsFloat4x2(VS_GLOBAL_CONSTANT_TEXTUREMATRIX2, &stTexMatrix);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_TEXTUREMATRIX3))
    {
        D3DXMATRIX stTexMatrix;

        pMaterial->GetTransform(3, &stTexMatrix);

        _pShader->SetGlobalMatrixAsFloat4x2(VS_GLOBAL_CONSTANT_TEXTUREMATRIX3, &stTexMatrix);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_MAT_DIFFUSE))
    {
        VECTOR4FLOAT oDiffuseColor;

        XeJadeColorToV4F(&oDiffuseColor, pMaterial->GetDiffuseColor());

        if(pMaterial->GetDiffuseSelection()==MAT_Xe_DiffuseSel_World)
        {
            VECTOR4FLOAT oMatDiffuseMult;
            XeJadeColorToV4F(&oMatDiffuseMult, g_pXeContextManagerRender->ul_XeDiffuseColor);

            oDiffuseColor.x *= oMatDiffuseMult.x * 2.0f;
            oDiffuseColor.y *= oMatDiffuseMult.y * 2.0f;
            oDiffuseColor.z *= oMatDiffuseMult.z * 2.0f;

            oDiffuseColor.x = fMin(oDiffuseColor.x, 1.0f);
            oDiffuseColor.y = fMin(oDiffuseColor.y, 1.0f);
            oDiffuseColor.z = fMin(oDiffuseColor.z, 1.0f);
        }

#if defined(ACTIVE_EDITORS)
		XeEdModifyColor(&oDiffuseColor, _pObject->GetExtraFlags());
#endif
        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_MAT_DIFFUSE, &oDiffuseColor);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_MAT_SPECULAR))
    {
        VECTOR4FLOAT oSpecularColor;

        XeJadeColorToV4F(&oSpecularColor, pMaterial->GetSpecularColor());

        if(pMaterial->GetSpecularSelection()==MAT_Xe_SpecularSel_World)
        {
            VECTOR4FLOAT oMatSpecularMult;
            XeJadeColorToV4F(&oMatSpecularMult, g_pXeContextManagerRender->ul_XeSpecularColor);

            oSpecularColor.x *= oMatSpecularMult.x * 2.0f;
            oSpecularColor.y *= oMatSpecularMult.y * 2.0f;
            oSpecularColor.z *= oMatSpecularMult.z * 2.0f;

            oSpecularColor.x = fMin(oSpecularColor.x, 1.0f);
            oSpecularColor.y = fMin(oSpecularColor.y, 1.0f);
            oSpecularColor.z = fMin(oSpecularColor.z, 1.0f);
        }

#if defined(ACTIVE_EDITORS)
		XeEdModifyColor(&oSpecularColor, _pObject->GetExtraFlags());
#endif
        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_MAT_SPECULAR, &oSpecularColor);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_MAT_CONSTANT))
    {
        ULONG        ulColor;
        VECTOR4FLOAT oColor;

        switch (g_oFXManager.GetCurrentPass())
        {
            case XeFXManager::RP_APPLY_DEPTH_ENV_RLI:
                ulColor = 0x00000000;
                break;

            case XeFXManager::RP_APPLY_AMBIENT:
                ulColor = g_pXeContextManagerRender->ul_AmbientColor;
                break;

            case XeFXManager::RP_DEFAULT:
            case XeFXManager::RP_APPLY_REFLECTION:
                ulColor = pMaterial->GetConstantColor();
                break;

            default:
                ulColor = pMaterial->GetDiffuseColor();
                break;
        }

        XeJadeColorToV4F(&oColor, ulColor);
#if defined(ACTIVE_EDITORS)
		XeEdModifyColor(&oColor, _pObject->GetExtraFlags());
#endif
        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_MAT_CONSTANT, &oColor);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHTS_ARRAY) && (_oDirtyFlags.bDirLights || _oDirtyFlags.bOmniLights || _oDirtyFlags.bSpotLights || _oDirtyFlags.bCylSpotLights))
    {
        if (g_pXeContextManagerRender->GetCurrentLightSetLightCount() > 0)
        {
            if ((m_oFeatures.ulCustomShader != CUSTOM_VS_WATER) && (m_oFeatures.ulLighting == LIGHTING_VERTEX))
                UpdateLightConstantsForVertexLighting(_pShader, _pObject);
            else
                UpdateLightConstantsForPixelLighting(_pShader, _pObject);

            g_pXeContextManagerRender->ResetLastActiveLightSet();
        }

	    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHT_COUNTS) &&
            ( _oDirtyFlags.bDirLights ||  _oDirtyFlags.bOmniLights || _oDirtyFlags.bSpotLights || _oDirtyFlags.bCylSpotLights ))
	    {
            _pShader->SetGlobalIntArray( VS_GLOBAL_CONSTANT_LIGHT_COUNTS,  g_pXeContextManagerRender->GetCurrentLightSetLightCountArray(), 4 );
        }
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_AMBIENT_COLOR) && m_oDirtyFlags.bAmbientColor)
    {
        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_AMBIENT_COLOR, &m_oAmbientColor[m_ulActiveAmbient]);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_PER_PIXEL_LIGHT) && _oDirtyFlags.bPerPixelLightIndex)
    {
        _pShader->SetGlobalIntArray(VS_GLOBAL_CONSTANT_PER_PIXEL_LIGHT, g_pXeContextManagerRender->GetCurrentPerPixelLightArray(), VS_MAX_LIGHTS_PER_PASS);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_MIXED2))
    {
        VECTOR4FLOAT oMixed2 = {1.0f, 0.0f, pMaterial->GetSpecularExponent(), 0.0f};
      
        if (_pObject->GetDrawMask() & GDI_Cul_DM_DynamicRLI)
        {
            g_pXeContextManagerRender->GetGlobalRLIScaleAndOffset(&oMixed2.x, &oMixed2.y);
        }

        if(pMaterial->GetSpecularSelection()==MAT_Xe_SpecularSel_World)
        {
            oMixed2.z = g_pXeContextManagerRender->f_XeSpecularShiny*(oMixed2.z - MAT_Xe_Specular_Exponent_Min)+MAT_Xe_Specular_Exponent_Min;
            MATH_f_FloatLimit(oMixed2.z, MAT_Xe_Specular_Exponent_Min, MAT_Xe_Specular_Exponent_Max);
        }
      
        _pShader->SetGlobalVector( VS_GLOBAL_CONSTANT_MIXED2, &oMixed2 );
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_VIEWER_VECTOR))
    {
	    D3DXMATRIX oInvWorldView;
        D3DXMatrixInverse(&oInvWorldView, NULL, pWorldViewMatrix);
        
        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_VIEWER_VECTOR, (VECTOR4FLOAT*)(&oInvWorldView.m[3][0]));
    }

    // Light shaft constants

	if (_pObject->GetObjectType() == XeRenderObject::LightShaft)
	{
		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHTSHAFT_PLANEPARAMS))
		{
	        VECTOR4FLOAT stParams;

			g_oXeLightShaftManager.GetLightShaftPlaneParams(&stParams);

			_pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_LIGHTSHAFT_PLANEPARAMS, &stParams);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHTSHAFT_VOLUME))
		{
	        VECTOR4FLOAT stVolume;

			g_oXeLightShaftManager.GetLightShaftVolume(&stVolume);

			_pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_LIGHTSHAFT_VOLUME, &stVolume);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHTSHAFT_MATRIX))
		{
	        D3DXMATRIX stMatrix;

			g_oXeLightShaftManager.GetLightShaftMatrix(&stMatrix);

			_pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_LIGHTSHAFT_MATRIX, &stMatrix);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHTSHAFT_NOISEMATRIX1))
		{
	        D3DXMATRIX stMatrix;

			g_oXeLightShaftManager.GetLightShaftNoiseMatrix(&stMatrix, 0);

			_pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_LIGHTSHAFT_NOISEMATRIX1, &stMatrix);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHTSHAFT_NOISEMATRIX2))
		{
	        D3DXMATRIX stMatrix;

			g_oXeLightShaftManager.GetLightShaftNoiseMatrix(&stMatrix, 1);

			_pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_LIGHTSHAFT_NOISEMATRIX2, &stMatrix);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHTSHAFT_INVWORLDVIEW))
		{
	        D3DXMATRIX stMatrix;

			g_oXeLightShaftManager.GetLightShaftInvWorldViewMatrix(&stMatrix);

			_pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_LIGHTSHAFT_INVWORLDVIEW, &stMatrix);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHTSHAFT_SHADOWMATRIX))
		{
	        D3DXMATRIX stMatrix;

			g_oXeLightShaftManager.GetLightShaftShadowMatrix(&stMatrix);

			_pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_LIGHTSHAFT_SHADOWMATRIX, &stMatrix);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHTSHAFT_TEXOFFSET0_1))
		{
	        VECTOR4FLOAT stOffsets;

			g_oXeLightShaftManager.GetLightShaftTexOffset(&stOffsets, 0);

			_pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_LIGHTSHAFT_TEXOFFSET0_1, &stOffsets);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHTSHAFT_TEXOFFSET2_3))
		{
	        VECTOR4FLOAT stOffsets;

			g_oXeLightShaftManager.GetLightShaftTexOffset(&stOffsets, 1);

			_pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_LIGHTSHAFT_TEXOFFSET2_3, &stOffsets);
		}
	}

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_TEXOFFSET01))
    {
        VECTOR4FLOAT *pOffsets;

        pOffsets = g_oWaterManager.GetTexOffset(0);

        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_TEXOFFSET01, pOffsets);

        if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_TEXOFFSET23))
        {
            pOffsets = g_oWaterManager.GetTexOffset(1);

            _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_TEXOFFSET23, pOffsets);
        }
    }
   
    // Time
    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_TIME))
    {
        VECTOR4FLOAT stTime;

        stTime.x = TIM_gf_MainClock;
        stTime.y = fmodf(TIM_gf_MainClock, Cf_2Pi);
        stTime.z = 0.0f; // Free
        stTime.w = 0.0f; // Free

        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_TIME, &stTime);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_REFLECTION_PLANE))
    {
        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_REFLECTION_PLANE, &m_oReflectionPlane);
        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_REFLECTION_PLANE_ORIGIN, &m_oReflectionPlaneOrigin);
    }

    // Viewport UVs
    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_VIEWPORTUV))
    {
        VECTOR4FLOAT stViewportUV;

        stViewportUV.x = 0.0f;
        stViewportUV.y = 0.0f;
        stViewportUV.z = 1.0f;
        stViewportUV.w = 1.0f;

#if defined(ACTIVE_EDITORS)
        g_oXeRenderer.AdjustUVToViewport(stViewportUV.x, stViewportUV.y);
        g_oXeRenderer.AdjustUVToViewport(stViewportUV.z, stViewportUV.w);
#endif
        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_VIEWPORTUV, &stViewportUV);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_RESOLUTION))
    {
        VECTOR4FLOAT Resolution;
        UINT uiWidth, uiHeight;

        g_oXeRenderer.GetBackbufferResolution(&uiWidth, &uiHeight);

        Resolution.x = (FLOAT)uiWidth;
        Resolution.y = (FLOAT)uiHeight;
        Resolution.z = 0.0f;
        Resolution.w = 0.0f;

        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_RESOLUTION, &Resolution);
    }

    // Rain

	if (_pObject->GetObjectType() == XeRenderObject::Rain)
	{
	    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_RAIN_TEXCOORDMDF))
	    {
			VECTOR4FLOAT vTemp;

			g_oXeWeatherManager.GetRainTexCoordModifiers(&vTemp.component[0]);

			_pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_RAIN_TEXCOORDMDF, &vTemp);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_RAIN_TEXCOORDPOS))
		{
	        VECTOR4FLOAT vTemp;

			g_oXeWeatherManager.GetRainTexCoordPosition(&vTemp.component[0]);

			_pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_RAIN_TEXCOORDPOS, &vTemp);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_RAIN_WINDVECTOR))
		{
			VECTOR4FLOAT vTemp;

			g_oXeWeatherManager.GetRainWindVector(&vTemp.component[0]);

			_pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_RAIN_WINDVECTOR, &vTemp);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_RAIN_BLURTEXCOORDOFS0_1))
		{
	        VECTOR4FLOAT vTemp;

			g_oXeWeatherManager.GetRainBlurTexOffset(&vTemp.component[0], 0);

			_pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_RAIN_BLURTEXCOORDOFS0_1, &vTemp);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_RAIN_BLURTEXCOORDOFS2_3))
		{
	        VECTOR4FLOAT vTemp;

			g_oXeWeatherManager.GetRainBlurTexOffset(&vTemp.component[0], 1);

	        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_RAIN_BLURTEXCOORDOFS2_3, &vTemp);
	    }

	    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_RAIN_BLURPOSOFS))
	    {
			VECTOR4FLOAT vTemp;

			g_oXeWeatherManager.GetRainBlurPosOffset(&vTemp.component[0]);

			_pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_RAIN_BLURPOSOFS, &vTemp);
		}

		if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_RAIN_TEXCOORDXFORM))
		{
	        _pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_RAIN_TEXCOORDXFORM, g_oXeWeatherManager.GetRainTexCoordTransform());
	    }
	}

    // Rim Light
    if( m_oFeatures.bRimLightEnable > RIMLIGHT_OFF )
    {
        if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_RIMLIGHT_HEIGHT_ATTENUATION ) )
        {
            _pShader->SetGlobalVector( VS_GLOBAL_CONSTANT_RIMLIGHT_HEIGHT_ATTENUATION, &m_stRimLightShaderConsts.m_vHeightAttenuation );
        }
    }

    if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_WORLD ) )
    {
        D3DXMATRIX *pWorldMatrix = g_pXeContextManagerRender->GetWorldMatrixByIndex(_pObject->GetWorldMatrixIndex());
        _pShader->SetGlobalMatrix(VS_GLOBAL_CONSTANT_WORLD, pWorldMatrix);
    }

    // Heat shimmer
    if ( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_HEAT_SHIMMER_CENTER ) )
    {
        _pShader->SetGlobalVector( VS_GLOBAL_CONSTANT_HEAT_SHIMMER_CENTER, &(g_pXeContextManagerRender->GetHeatShimmerConstsPointer()->m_vCenter) );

	    if ( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_HEAT_SHIMMER_SHAPE ) )
	    {
			_pShader->SetGlobalVector( VS_GLOBAL_CONSTANT_HEAT_SHIMMER_SHAPE, &(g_pXeContextManagerRender->GetHeatShimmerConstsPointer()->m_vShape) );
		}

		if ( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_HEAT_SHIMMER_PARAMS ) )
	    {
			_pShader->SetGlobalVector( VS_GLOBAL_CONSTANT_HEAT_SHIMMER_PARAMS, &(g_pXeContextManagerRender->GetHeatShimmerConstsPointer()->m_vParams) );
		}

        if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_HEAT_SHIMMER_PROJ) )
        {
            _pShader->SetGlobalMatrix ( VS_GLOBAL_CONSTANT_HEAT_SHIMMER_PROJ, g_pXeContextManagerRender->GetProjectionMatrixByIndex(g_pXeContextManagerRender->GetHeatShimmerConstsPointer()->m_ulProjectionMatrixIndex) );
        }
    }

    if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_HALF_VIEWPORT_TEXEL ))
    {
        VECTOR4FLOAT vHalfTexel = {0.0f, 0.0f, 0.0f, 0.0f};

        if( !m_ignoreHalfViewportTexel )
        {
            g_oXeRenderer.AddHalfViewportPixelOffset(vHalfTexel.x, vHalfTexel.y );
        }

        _pShader->SetGlobalVector( VS_GLOBAL_CONSTANT_HALF_VIEWPORT_TEXEL, &vHalfTexel );
    }

    if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_SHADOWBLUROFFSETS ))
    {
        // only need 4 entries since 7x7 gaussian is in mirror
        _pShader->SetGlobalVectorArray(VS_GLOBAL_CONSTANT_SHADOWBLUROFFSETS, g_oXeShadowManager.GetBlurOffsetsAndWeights(), 4);
    }

    if(_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_WYB_AXES))
    {
        INT iIdx1, iIdx2;
        _pObject->GetWYBIndices(iIdx1, iIdx2);

        INT iNumWYB = (iIdx2 == -1) ? 1 : 2;

        _pShader->SetGlobalVectorArray(VS_GLOBAL_CONSTANT_WYB_AXES, (g_pXeContextManagerRender->GetWYBParams(iIdx1))->Axes, iNumWYB);
        _pShader->SetGlobalVectorArray(VS_GLOBAL_CONSTANT_WYB_VALUES, (g_pXeContextManagerRender->GetWYBParams(iIdx1))->Values, iNumWYB);
        _pShader->SetGlobalVectorArray(VS_GLOBAL_CONSTANT_WYB_CONDITIONS, (g_pXeContextManagerRender->GetWYBParams(iIdx1))->Conditions, iNumWYB);
    }

    // Symmetry offset
    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_SYMMETRY_OFFSET))
    {
        FLOAT fOffset = 2.0f * g_pXeContextManagerRender->GetSymmetryParams(_pObject->GetSymmetryIndex())->fOffset;

        _pShader->SetGlobalFloat(VS_GLOBAL_CONSTANT_SYMMETRY_OFFSET, fOffset);
    }

    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_LIGHT_BASE))
    {
        _pShader->SetGlobalInt(VS_GLOBAL_CONSTANT_LIGHT_BASE, m_iLightBase);
    }

    if( m_oFeatures.ulCustomShader == CUSTOM_VS_AFTEREFFECTS )
    {
        if(  _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_ZOOM_BLUR_RATIOS ) )
        {
            _pShader->SetGlobalVector( VS_GLOBAL_CONSTANT_ZOOM_BLUR_RATIOS, &m_vZoomBlurRatios );

            if(_pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_ZOOM_BLUR_SCALE ))
            {
                _pShader->SetGlobalVectorArray( VS_GLOBAL_CONSTANT_ZOOM_BLUR_SCALE, m_vZoomBlurUVScale, ZOOM_BLUR_MAX_STEPS_PER_PASS );
            }

            if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_ZOOM_BLUR_TRANSLATE ))
            {
                _pShader->SetGlobalVectorArray( VS_GLOBAL_CONSTANT_ZOOM_BLUR_TRANSLATE, m_vZoomBlurUVTranslation, ZOOM_BLUR_MAX_STEPS_PER_PASS );
            }

            if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_ZOOM_BLUR_TRANSLATE ))
            {
                _pShader->SetGlobalVectorArray( VS_GLOBAL_CONSTANT_ZOOM_BLUR_TRANSLATE, m_vZoomBlurUVTranslation, ZOOM_BLUR_MAX_STEPS_PER_PASS );
            }

            if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_ZOOM_BLUR_ALPHA ))
            {
                _pShader->SetGlobalFloatArray ( VS_GLOBAL_CONSTANT_ZOOM_BLUR_ALPHA, m_fZoomBlurPassAlpha, ZOOM_BLUR_MAX_STEPS_PER_PASS );
            }

            if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_GODRAY_ADJUST) )
            {
                _pShader->SetGlobalVector ( VS_GLOBAL_CONSTANT_GODRAY_ADJUST, &m_vGodRayAdjust );
            }
        }

        if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_AE_GLOW_INPUT_REGION ) )
        {
            _pShader->SetGlobalInt( VS_GLOBAL_CONSTANT_AE_GLOW_INPUT_REGION, (INT)m_stAEGlowConsts.m_iInputRegion );

            if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_AE_GLOW_OUTPUT_REGION ) )
            {
                _pShader->SetGlobalInt( VS_GLOBAL_CONSTANT_AE_GLOW_OUTPUT_REGION, (INT)m_stAEGlowConsts.m_iOutputRegion );
            }

            if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_AE_BLURDIRECTION ) )
            {
                VECTOR4FLOAT vBlurDirection;

                vBlurDirection.z = 0.0f;
                vBlurDirection.w = 0.0f;

                if( m_stAEGlowConsts.m_bBlurHorizontal )
                {
                    vBlurDirection.x = 1.0f;
                    vBlurDirection.y = 0.0f;
                }
                else
                {
                    vBlurDirection.x = 0.0f;
                    vBlurDirection.y = 1.0f;
                }

                _pShader->SetGlobalVector( VS_GLOBAL_CONSTANT_AE_BLURDIRECTION, &vBlurDirection );
            }
        }

        if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_AE_BIG_BLUR_FACTOR ) )
        {
            _pShader->SetGlobalFloat( VS_GLOBAL_CONSTANT_AE_BIG_BLUR_FACTOR, m_fBigBlurFactor );
        }

        if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_BORDER_BRIGHTNESS ) )
        {
            _pShader->SetGlobalVector( VS_GLOBAL_CONSTANT_BORDER_BRIGHTNESS, &m_vBorderBrightness );
        }

        if( _pShader->IsGlobalConstPresent( VS_GLOBAL_CONSTANT_SPINSMOOTH_TRANSFORM ) )
        {
            _pShader->SetGlobalMatrix( VS_GLOBAL_CONSTANT_SPINSMOOTH_TRANSFORM, &m_mSpinSmoothTransform );
        }
    }

    m_oDirtyFlags.Clear();
    g_pXeContextManagerRender->ClearDirtyFlags();
}


// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::UpdateLightConstantsForVertexLighting(XeVertexShader *_pShader, XeRenderObject *_pObject)
{
    VECTOR4FLOAT * aVectors = NULL;
    ULONG ulCurIndex = 0, ulCurLight;
    ULONG ulRemainingLights;

    // only one pass supported for spg2 per pixel lighting
    ulRemainingLights = VS_MAX_LIGHTS;

  #ifdef _XENON
    _pShader->FastConstBegin(VS_GLOBAL_CONSTANT_LIGHTS_ARRAY, &aVectors, XELIGHT_VECTOR_COUNT * min(ulRemainingLights, g_pXeContextManagerRender->GetCurrentLightSetLightCount() ));
  #else
    static VECTOR4FLOAT aVectorsBuffer[VS_MAX_LIGHTS * XELIGHT_VECTOR_COUNT];
    aVectors = aVectorsBuffer;
  #endif

    XeFXManager::RenderPass ePass = g_oFXManager.GetCurrentPass();

    for (ulCurLight = 0; (ulCurLight < g_pXeContextManagerRender->GetCurrentLightSetDirLightCount()) && (ulRemainingLights > 0); ulCurLight++, ulCurIndex += XELIGHT_VECTOR_COUNT)
    {
        XeLight* pLight = g_pXeContextManagerRender->GetCurrentLightSetDirLight(ulCurLight);

        *((XeLightConsts *)&aVectors[ulCurIndex]) = *((XeLightConsts *)pLight);

        // Include the light multipliers in the color
        if (m_oFeatures.ulLighting == LIGHTING_VERTEX )
        {
            FLOAT fDiffuseMul  = pLight->DiffuseMultiplier;
            FLOAT fSpecularMul = pLight->SpecularMultiplier;

            if(!(_pObject->GetDrawMask() & GDI_Cul_DM_UseMapLightSettings))
            {
                fDiffuseMul  *= pLight->ActorDiffusePonderator;
                fSpecularMul *= pLight->ActorSpecularPonderator;
            }

            if(fDiffuseMul<0.0001f)
                fDiffuseMul = 0.0001f;

            fSpecularMul /= fDiffuseMul;

            VECTOR4FLOAT* pVector = &aVectors[ulCurIndex + 2];

            pVector->x *= fDiffuseMul;
            pVector->y *= fDiffuseMul;
            pVector->z *= fDiffuseMul;
            pVector->w  = fSpecularMul;
        }

        --ulRemainingLights;
    }

    for (ulCurLight = 0; ulCurLight < (g_pXeContextManagerRender->GetCurrentLightSetOmniLightCount()) && (ulRemainingLights > 0); ulCurLight++, ulCurIndex += XELIGHT_VECTOR_COUNT)
    {
        XeLight* pLight = g_pXeContextManagerRender->GetCurrentLightSetOmniLight(ulCurLight);

        *((XeLightConsts *)&aVectors[ulCurIndex]) = *((XeLightConsts *)pLight);

        // Include the light multipliers in the color
        if (m_oFeatures.ulLighting == LIGHTING_VERTEX)
        {
            FLOAT fDiffuseMul  = pLight->DiffuseMultiplier;
            FLOAT fSpecularMul = pLight->SpecularMultiplier;

            if(!(_pObject->GetDrawMask() & GDI_Cul_DM_UseMapLightSettings))
            {
                fDiffuseMul  *= pLight->ActorDiffusePonderator;
                fSpecularMul *= pLight->ActorSpecularPonderator;
            }

            if(fDiffuseMul<0.0001f)
                fDiffuseMul = 0.0001f;

            fSpecularMul /= fDiffuseMul;

            VECTOR4FLOAT* pVector = &aVectors[ulCurIndex + 2];

            pVector->x *= fDiffuseMul;
            pVector->y *= fDiffuseMul;
            pVector->z *= fDiffuseMul;
            pVector->w  = fSpecularMul;
        }

        --ulRemainingLights;
    }

    for (ulCurLight = 0; ulCurLight < (g_pXeContextManagerRender->GetCurrentLightSetSpotLightCount()) && (ulRemainingLights > 0); ulCurLight++, ulCurIndex += XELIGHT_VECTOR_COUNT)
    {
        XeLight* pLight = g_pXeContextManagerRender->GetCurrentLightSetSpotLight(ulCurLight);

        *((XeLightConsts *)&aVectors[ulCurIndex]) = *((XeLightConsts *)pLight);

        // Include the light multipliers in the color
        if (m_oFeatures.ulLighting == LIGHTING_VERTEX)
        {
            FLOAT fDiffuseMul  = pLight->DiffuseMultiplier;
            FLOAT fSpecularMul = pLight->SpecularMultiplier;

            if(!(_pObject->GetDrawMask() & GDI_Cul_DM_UseMapLightSettings))
            {
                fDiffuseMul  *= pLight->ActorDiffusePonderator;
                fSpecularMul *= pLight->ActorSpecularPonderator;
            }

            if(fDiffuseMul<0.0001f)
                fDiffuseMul = 0.0001f;

            fSpecularMul /= fDiffuseMul;

            VECTOR4FLOAT* pVector = &aVectors[ulCurIndex + 2];

            pVector->x *= fDiffuseMul;
            pVector->y *= fDiffuseMul;
            pVector->z *= fDiffuseMul;
            pVector->w  = fSpecularMul;
        }

        --ulRemainingLights;
    }

    for (ulCurLight = 0; ulCurLight < (g_pXeContextManagerRender->GetCurrentLightSetCylSpotLightCount()) && (ulRemainingLights > 0); ulCurLight++, ulCurIndex += XELIGHT_VECTOR_COUNT)
    {
        XeLight* pLight = g_pXeContextManagerRender->GetCurrentLightSetCylSpotLight(ulCurLight);

        *((XeLightConsts *)&aVectors[ulCurIndex]) = *((XeLightConsts *)pLight);

        // Include the light multipliers in the color
        if (m_oFeatures.ulLighting == LIGHTING_VERTEX)
        {
            FLOAT fDiffuseMul  = pLight->DiffuseMultiplier;
            FLOAT fSpecularMul = pLight->SpecularMultiplier;

            if(!(_pObject->GetDrawMask() & GDI_Cul_DM_UseMapLightSettings))
            {
                fDiffuseMul  *= pLight->ActorDiffusePonderator;
                fSpecularMul *= pLight->ActorSpecularPonderator;
            }

            if(fDiffuseMul<0.0001f)
                fDiffuseMul = 0.0001f;

            fSpecularMul /= fDiffuseMul;

            VECTOR4FLOAT* pVector = &aVectors[ulCurIndex + 2];

            pVector->x *= fDiffuseMul;
            pVector->y *= fDiffuseMul;
            pVector->z *= fDiffuseMul;
            pVector->w  = fSpecularMul;
        }

        --ulRemainingLights;
    }

  #ifdef _XENON
    _pShader->FastConstEnd();
  #else
    _pShader->SetGlobalRawData(VS_GLOBAL_CONSTANT_LIGHTS_ARRAY, aVectors, ulCurIndex);
  #endif
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::UpdateLightConstantsForPixelLighting(XeVertexShader *_pShader, XeRenderObject *_pObject)
{
    VECTOR4FLOAT * aVectors = NULL; 
    ULONG ulCurIndex = 0, ulType;

    ULONG ulRemainingLights = min(VS_MAX_LIGHTS_PER_PASS, g_pXeContextManagerRender->GetCurrentLightSetLightCount() - m_iLightBase);
  #ifdef _XENON
    ULONG ulVectorCount = ulRemainingLights * XELIGHT_VECTOR_COUNT;
    
    // SDALLAIRE : if the last light is a directionnal, the compiler will upload
    // just the necessary number of vectors instead of the full structure.
    if (ulRemainingLights > 0)
    {
        g_pXeContextManagerRender->GetCurrentPerPixelLight(ulRemainingLights-1, &ulType);
        
        if (ulType == LIGHT_TYPE_DIRECT)
            ulVectorCount -= 2;
    }

    _pShader->FastConstBegin(VS_GLOBAL_CONSTANT_LIGHTS_ARRAY, &aVectors, ulVectorCount);
  #else
    static VECTOR4FLOAT aVectorsBuffer[VS_MAX_LIGHTS_PER_PASS * XELIGHT_VECTOR_COUNT];
    aVectors = aVectorsBuffer;
  #endif

    for (ULONG ulCurLight = 0; ulCurLight < ulRemainingLights; ulCurLight++)
    {
        XeLight *poLight = g_pXeContextManagerRender->GetCurrentPerPixelLight(ulCurLight, &ulType);
        if (poLight != NULL)
        {
            if ((ulCurLight == ulRemainingLights-1) && (ulType == LIGHT_TYPE_DIRECT))
            {
                // SDALLAIRE : if the last light is a directionnal, the compiler will upload
                // just the necessary number of vectors instead of the full structure.
                L_memcpy(&aVectors[ulCurIndex], poLight, 2*sizeof(VECTOR4FLOAT));
                ulCurIndex += 2;
            }
            else
            {
                L_memcpy(&aVectors[ulCurIndex], poLight, 4*sizeof(VECTOR4FLOAT));
                ulCurIndex += 4;
            }
        }
    }

  #ifdef _XENON
    _pShader->FastConstEnd();
  #else
    _pShader->SetGlobalRawData(VS_GLOBAL_CONSTANT_LIGHTS_ARRAY, aVectors, ulCurIndex);
  #endif
}


// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::UpdateShaderConstantsFur(XeVertexShader *_pShader)
{
    if (_pShader->IsGlobalConstPresent(VS_GLOBAL_CONSTANT_FUR_PARAMS))
    {
        _pShader->SetGlobalVector(VS_GLOBAL_CONSTANT_FUR_PARAMS, g_pXeContextManagerRender->GetFURShaderConsts());
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::InitShadersTable
// Params : None
// RetVal : None
// Descr. : Initialize the shaders table
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::InitShadersTable(void)
{
/*
    XeVertexShaderEntry entry;

    sprintf(entry.FileName, "d:\\Shaders\\PositionDiffuseTexCoord.xvu");
    entry.Key.ulKey = BuildFeatureKey(CUSTOMSHADER_DISABLE, TRANSFORM_NORMAL, COLORSOURCE_VERTEX_COLOR, LIGHTING_USE_COLOR, UVSOURCE_OBJECT, TEXTRANSFORM_NONE, FOG_OFF, ENVMAP_NONE, FALSE);
    m_aVertexShaderEntries.push_back(entry);

    sprintf(entry.FileName, "d:\\Shaders\\ScreenspaceDiffuseTexcoord.xvu");
    entry.Key.ulKey = BuildFeatureKey(CUSTOMSHADER_DISABLE, TRANSFORM_NONE, COLORSOURCE_VERTEX_COLOR, LIGHTING_USE_COLOR, UVSOURCE_OBJECT, TEXTRANSFORM_NONE, FOG_OFF, ENVMAP_NONE, FALSE);
    m_aVertexShaderEntries.push_back(entry);

    sprintf(entry.FileName, "d:\\Shaders\\PositionDiffuseTexCoordFog.xvu");
    entry.Key.ulKey = BuildFeatureKey(CUSTOMSHADER_DISABLE, TRANSFORM_NORMAL, COLORSOURCE_VERTEX_COLOR, LIGHTING_USE_COLOR, UVSOURCE_OBJECT, TEXTRANSFORM_NONE, FOG_ON, ENVMAP_NONE, FALSE);
    m_aVertexShaderEntries.push_back(entry);

    sprintf(entry.FileName, "d:\\Shaders\\ScreenspaceDiffuseTexcoordFog.xvu");
    entry.Key.ulKey = BuildFeatureKey(CUSTOMSHADER_DISABLE, TRANSFORM_NONE, COLORSOURCE_VERTEX_COLOR, LIGHTING_USE_COLOR, UVSOURCE_OBJECT, TEXTRANSFORM_NONE, FOG_ON, ENVMAP_NONE, FALSE);
    m_aVertexShaderEntries.push_back(entry);

    // Output shader information
#if defined(XE_VS_VERBOSE)

    ERR_OutputDebugString("[Xenon] XeVertexShaderManager - Shader Table\n");
    ERR_OutputDebugString("        ------------------------------------\n");
    for (ULONG i = 0; i < m_aVertexShaderEntries.size(); ++i)
    {
        ERR_OutputDebugString("        [0x%08x] %s\n", m_aVertexShaderEntries[i].Key, m_aVertexShaderEntries[i].FileName);
    }
    ERR_OutputDebugString("        ------------------------------------\n");
#endif // defined(XE_VS_VERBOSE)
*/
}

// ------------------------------------------------------------------------------------------------
// Name   : GetD3DElementSize
// Params : ..
// RetVal : Size of the element (in bytes)
// Descr. : Returns the size of an element
// ------------------------------------------------------------------------------------------------
inline ULONG GetD3DElementSize(DWORD _dwType)
{
    switch (_dwType)
    {
        case D3DDECLTYPE_FLOAT1:
        case D3DDECLTYPE_D3DCOLOR:
        case D3DDECLTYPE_UBYTE4:
        case D3DDECLTYPE_DEC3N:
            return 4;

        case D3DDECLTYPE_FLOAT2:
        case D3DDECLTYPE_SHORT4:
        case D3DDECLTYPE_SHORT4N:
            return 8;

        case D3DDECLTYPE_FLOAT3:
            return 12;

        case D3DDECLTYPE_FLOAT4:
            return 16;

        default:
            ERR_X_Assert(FALSE && "Invalid element type");
            return 0;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : SetD3DElement
// Params : ..
// RetVal : None
// Descr. : Fill a D3D vertex element and increment the offset accordingly
// ------------------------------------------------------------------------------------------------
inline void SetD3DElement(D3DVERTEXELEMENT9& _stElement, ULONG _ulStream, ULONG& _ulOffset, DWORD _dwType, BYTE _byUsage, BYTE _byIndex)
{
    _stElement.Stream     = (WORD)_ulStream;
    _stElement.Offset     = (WORD)_ulOffset;
    _stElement.Type       = _dwType;
    _stElement.Method     = D3DDECLMETHOD_DEFAULT;
    _stElement.Usage      = _byUsage;
    _stElement.UsageIndex = _byIndex;

    _ulOffset += GetD3DElementSize(_dwType);
}

#if defined(XE_VS_VERBOSE)
const CHAR* GetD3DTypeString(DWORD _dwType)
{
    switch (_dwType)
    {
        case D3DDECLTYPE_FLOAT1:        return "FLOAT1";
        case D3DDECLTYPE_FLOAT2:        return "FLOAT2";
        case D3DDECLTYPE_FLOAT3:        return "FLOAT3";
        case D3DDECLTYPE_FLOAT4:        return "FLOAT4";
        case D3DDECLTYPE_D3DCOLOR:      return "D3DCOLOR";
        case D3DDECLTYPE_UBYTE4:        return "UBYTE4";
        case D3DDECLTYPE_SHORT4:        return "SHORT4";
        case D3DDECLTYPE_SHORT4N:       return "SHORT4N";
        case D3DDECLTYPE_DEC3N:         return "DEC3N";
    }

    return "Unknown";
}

const CHAR* GetD3DUsageString(DWORD _dwUsage)
{
    switch (_dwUsage)
    {
        case D3DDECLUSAGE_POSITION:     return "Position";
        case D3DDECLUSAGE_BLENDWEIGHT:  return "BlendWeights";
        case D3DDECLUSAGE_BLENDINDICES: return "BlendIndices";
        case D3DDECLUSAGE_NORMAL:       return "Normal";
        case D3DDECLUSAGE_TANGENT:      return "Tangent";
        case D3DDECLUSAGE_COLOR:        return "Color";
        case D3DDECLUSAGE_TEXCOORD:     return "TexCoord";
    }

    return "Unknown";
}

const CHAR* GetTransformString(ULONG _ulTransform)
{
    switch (_ulTransform)
    {
        case TRANSFORM_NONE:     return "None";
        case TRANSFORM_NORMAL:   return "Normal";
        case TRANSFORM_SKINNING: return "Skinning";
        case TRANSFORM_MORPHING: return "Morphing";
    }

    return "Unknown";
}
#endif // defined(XE_VS_VERBOSE)

ULONG XeVertexShaderManager::CreateDeclaration(ULONG  _ulNbStreams, 
                                               ULONG* _pulComponents,
                                               ULONG  _ulIndex)
{
    XeVertexDeclaration stMeshDecl;
    std::vector<D3DVERTEXELEMENT9> aElements;
    D3DVERTEXELEMENT9 stElement;
    ULONG ulOffset;
    ULONG ulComponents;
    ULONG ulStream;

    if (_ulIndex >= m_aoVertexDeclarations.size())
    {
        ULONG ulOldSize = m_aoVertexDeclarations.size();
        m_aoVertexDeclarations.resize(_ulIndex + 1);
        L_memset(&m_aoVertexDeclarations[ulOldSize], 0, ((_ulIndex + 1) - ulOldSize) * sizeof(XeVertexDeclaration));
    }
    else
    {
        XeValidateRet(m_aoVertexDeclarations[_ulIndex].pDeclaration == NULL, _ulIndex, "Vertex declaration already exists");
    }

    L_memset(&stMeshDecl, 0, sizeof(XeVertexDeclaration));

    for (ulStream = 0; ulStream < _ulNbStreams; ++ulStream)
    {
        stMeshDecl.aStreamComponents[ulStream] = _pulComponents[ulStream];
    }

    stMeshDecl.eAttributes.ulTransformType  = TRANSFORM_NORMAL;
    stMeshDecl.eAttributes.bHasColor        = 0;
    stMeshDecl.eAttributes.bHasNormal       = 0;
    stMeshDecl.eAttributes.bHasBaseTexCoord = 0;

    for (ulStream = 0; ulStream < _ulNbStreams; ++ulStream)
    {
        ulComponents = stMeshDecl.aStreamComponents[ulStream];
        ulOffset     = 0;

        if (ulComponents & XEVC_POSITIONT)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_POSITION, 0);
            aElements.push_back(stElement);
            stMeshDecl.eAttributes.ulTransformType = TRANSFORM_NONE;
        }
        else if (ulComponents & XEVC_POSITION)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_POSITION, 0);
            aElements.push_back(stElement);
        }

        if (ulComponents & XEVC_COLOR0)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_COLOR, 0);
            aElements.push_back(stElement);
            stMeshDecl.eAttributes.bHasColor = 1;
        }

        if (ulComponents & XEVC_TEXCOORD0_FLOAT4)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_TEXCOORD, 0);
            aElements.push_back(stElement);
            stMeshDecl.eAttributes.bHasBaseTexCoord = 1;
        }
        else if (ulComponents & XEVC_TEXCOORD0)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD, 0);
            aElements.push_back(stElement);
            stMeshDecl.eAttributes.bHasBaseTexCoord = 1;
        }

        if (ulComponents & XEVC_BLENDWEIGHT4)
        {
            if ((ulComponents & XEVC_BLENDWEIGHT4_SHORT4N) == XEVC_BLENDWEIGHT4_SHORT4N)
            {
                SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_SHORT4N, D3DDECLUSAGE_BLENDWEIGHT, 0);
            }
            else if ((ulComponents & XEVC_BLENDWEIGHT3_DEC3N) == XEVC_BLENDWEIGHT3_DEC3N)
            {
                SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_DEC3N, D3DDECLUSAGE_BLENDWEIGHT, 0);
            }
            else
            {
                SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_BLENDWEIGHT, 0);
            }

            aElements.push_back(stElement);
            stMeshDecl.eAttributes.ulTransformType = TRANSFORM_SKINNING;
        }

        if (ulComponents & XEVC_BLENDINDICES)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_UBYTE4, D3DDECLUSAGE_BLENDINDICES, 0);
            aElements.push_back(stElement);
        }

        if (ulComponents & XEVC_NORMAL)
        {
            if ((ulComponents & XEVC_NORMAL_DEC3N) == XEVC_NORMAL_DEC3N)
            {
                SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_DEC3N, D3DDECLUSAGE_NORMAL, 0);
            }
            else
            {
                SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_NORMAL, 0);
            }
            aElements.push_back(stElement);
            stMeshDecl.eAttributes.bHasNormal = 1;
        }

        if (ulComponents & XEVC_TANGENT)
        {
            if ((ulComponents & XEVC_TANGENT_SHORT4N) == XEVC_TANGENT_SHORT4N)
            {
                SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_SHORT4N, D3DDECLUSAGE_TANGENT, 0);
            }
            else if ((ulComponents & XEVC_TANGENT3_DEC3N) == XEVC_TANGENT3_DEC3N)
            {
                SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_DEC3N, D3DDECLUSAGE_TANGENT, 0);
            }
            else
            {
                SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_TANGENT, 0);
            }
            aElements.push_back(stElement);
        }

        if (ulComponents & XEVC_COLOR1)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_COLOR, 1);
            aElements.push_back(stElement);
        }

        if (ulComponents & XEVC_COLOR2)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_COLOR, 2);
            aElements.push_back(stElement);
        }

        if (ulComponents & XEVC_COLOR3)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_COLOR, 3);
            aElements.push_back(stElement);
        }

        if (ulComponents & XEVC_TEXCOORD1_FLOAT4)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_TEXCOORD, 1);
            aElements.push_back(stElement);
            stMeshDecl.eAttributes.bHasBaseTexCoord = 1;
        }
        else if (ulComponents & XEVC_TEXCOORD1)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD, 1);
            aElements.push_back(stElement);
            stMeshDecl.eAttributes.bHasBaseTexCoord = 1;
        }

        if (ulComponents & XEVC_TEXCOORD2_FLOAT4)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_TEXCOORD, 2);
            aElements.push_back(stElement);
        }
        else if (ulComponents & XEVC_TEXCOORD2)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD, 2);
            aElements.push_back(stElement);
        }

        if (ulComponents & XEVC_TEXCOORD3_FLOAT4)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_TEXCOORD, 3);
            aElements.push_back(stElement);
        }
        else if (ulComponents & XEVC_TEXCOORD3)
        {
            SetD3DElement(stElement, ulStream, ulOffset, D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD, 3);
            aElements.push_back(stElement);
        }
    }

    D3DVERTEXELEMENT9 stEndElement = D3DDECL_END();
    aElements.push_back(stEndElement);

    // Create a new declaration
    HRESULT hr = m_pD3DDevice->CreateVertexDeclaration(&aElements[0], &stMeshDecl.pDeclaration);
    ERR_X_Assert(SUCCEEDED(hr));

    // Add to the declaration array
    m_aoVertexDeclarations[_ulIndex] = stMeshDecl;

#if defined(XE_VS_VERBOSE)
    ERR_OutputDebugString("[Xenon] XeVertexShaderManager - New Vertex Declaration [%u] - Transform: %s\n", _ulIndex, GetTransformString(stMeshDecl.eAttributes.ulTransformType));
    ERR_OutputDebugString("        %-6s %-6s %-12s %-15s %s\n", "Stream", "Offset", "Type", "Usage", "Index");
    for (ULONG i = 0; i < aElements.size() - 1; ++i)
    {
        ERR_OutputDebugString("        %-6u %-6u %-12s %-15s %u\n", 
                              (ULONG)aElements[i].Stream,
                              (ULONG)aElements[i].Offset,
                              GetD3DTypeString(aElements[i].Type),
                              GetD3DUsageString(aElements[i].Usage),
                              (ULONG)aElements[i].UsageIndex);
    }
#endif

    return _ulIndex;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::GetVertexDeclaration
// Params : _pstMesh : Mesh
// RetVal : Vertex declaration
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeVertexShaderManager::XeVertexDeclaration* XeVertexShaderManager::GetVertexDeclaration(XeMesh* _pstMesh)
{
    ULONG ulHandle = _pstMesh->GetVertexFormatHandle();

    if (ulHandle == VS_INVALID_HANDLE)
    {
        XeVertexDeclaration* pstDecl;
        XeVertexDeclaration  stMeshDecl;
        ULONG ulNbDecl = m_aoVertexDeclarations.size();
        ULONG ulDecl;
        ULONG ulStream;
        BOOL  bValid;

        memset(stMeshDecl.aStreamComponents, 0, VS_MAX_STREAM * sizeof(ULONG));
        for (ulStream = 0; ulStream < _pstMesh->GetStreamCount(); ++ulStream)
        {
            stMeshDecl.aStreamComponents[ulStream] = _pstMesh->GetStream(ulStream)->ulComponents;
        }

        for (ulDecl = 0; ulDecl < ulNbDecl; ++ulDecl)
        {
            pstDecl = &m_aoVertexDeclarations[ulDecl];
            bValid  = TRUE;

            for (ulStream = 0; ulStream < VS_MAX_STREAM; ++ulStream)
            {
                if (pstDecl->aStreamComponents[ulStream] != stMeshDecl.aStreamComponents[ulStream])
                {
                    bValid = FALSE;
                    break;
                }
            }

            // Found it?
            if (bValid)
            {
                ulHandle = ulDecl;
                break;
            }
        }

        if (ulHandle != VS_INVALID_HANDLE)
        {
            // Declaration was already created, associate it with the mesh
            _pstMesh->SetVertexFormatHandle(ulHandle);

            return pstDecl;
        }
        else
        {
            ulHandle = CreateDeclaration(_pstMesh->GetStreamCount(), 
                                         stMeshDecl.aStreamComponents, 
                                         m_aoVertexDeclarations.size());

            // Add the declaration to the shader database
            ULONG aulDeclBuffer[VS_MAX_STREAM + 1];
            aulDeclBuffer[0] = _pstMesh->GetStreamCount();
            for (ULONG i = 0; i < VS_MAX_STREAM; ++i)
            {
                aulDeclBuffer[i + 1] = stMeshDecl.aStreamComponents[i];
            }
            g_oXeShaderDatabase.AddElement(XeShaderDatabase::ItemVertexDeclaration, ULONG128(ulHandle),
                                           aulDeclBuffer, (VS_MAX_STREAM + 1) * sizeof(ULONG));

            // Associate the declaration with the mesh
            _pstMesh->SetVertexFormatHandle(ulHandle);

        }
    }

    return &m_aoVertexDeclarations[ulHandle];
}

void XeVertexShaderManager::PreLoadDeclaration(ULONG _ulIndex, void* _pContent)
{
    ERR_X_Assert(_pContent != NULL);

    ULONG* pulDecl = (ULONG*)_pContent;

    CreateDeclaration(pulDecl[0], &pulDecl[1], _ulIndex);
}

void XeVertexShaderManager::PreLoadShader(const VertexShaderKey & _ulKey, void* _pCode)
{
    AddCreateShader(_ulKey, _pCode, NULL);
}

void     
XeVertexShaderManager::SetZoomBlurStepConsts( INT iStepNbr,
                                              float fAlpha, 
                                              float fUScale,
                                              float fUOffset,
                                              float fVScale,
                                              float fVOffset )
{
    ERR_X_Assert( iStepNbr < ZOOM_BLUR_MAX_STEPS_PER_PASS );

    m_fZoomBlurPassAlpha[iStepNbr] = fAlpha;

    m_vZoomBlurUVScale[iStepNbr].x = fUScale;
    m_vZoomBlurUVScale[iStepNbr].y = fVScale;
    m_vZoomBlurUVScale[iStepNbr].z = 1.0f;
    m_vZoomBlurUVScale[iStepNbr].w = 1.0f;

    m_vZoomBlurUVTranslation[iStepNbr].x = fUOffset;
    m_vZoomBlurUVTranslation[iStepNbr].y = fVOffset;
    m_vZoomBlurUVTranslation[iStepNbr].z = 0.0f;
    m_vZoomBlurUVTranslation[iStepNbr].w = 0.0f;
}


void   
XeVertexShaderManager::SetZoomBlurRatios( float fInputRatio, float fOutputRatio )
{
    m_vZoomBlurRatios.x = fInputRatio;
    m_vZoomBlurRatios.y = fOutputRatio;
}

BOOL    
XeVertexShaderManager::IsLightCreatingRim( ULONG _ulCurRim )
{
    ERR_X_Assert( _ulCurRim < VS_MAX_LIGHTS_PER_PASS );

    ULONG ulPerPixelLightType = 0;
    XeLight * pLight = g_pXeContextManagerRender->GetLight(g_pXeContextManagerRender->GetCurrentLightSetIndex(), g_pXeContextManagerRender->GetCurrentPerPixelLightIndex(_ulCurRim), &ulPerPixelLightType);
    ERR_X_Assert( pLight != NULL );

    return pLight->IsRimLight;
}

void XeVertexShaderManager::SetFeaturePPLightCountByType(ULONG _ulRemainingLights)
{
    ULONG ulDirLightCount, ulOmniLightCount, ulSpotLightCount, ulCylSpotLightCount;
    
    GetLightCountByType(g_pXeContextManagerRender->GetCurrentLightSet(),
                        m_iLightBase,
                        _ulRemainingLights,
                        ulDirLightCount,
                        ulOmniLightCount,
                        ulSpotLightCount,
                        ulCylSpotLightCount);

    m_oFeatures.ulDirLightCount     = ulDirLightCount;
    m_oFeatures.ulOmniLightCount    = ulOmniLightCount;
    m_oFeatures.ulSpotLightCount    = ulSpotLightCount;
    m_oFeatures.ulCylSpotLightCount = ulCylSpotLightCount;
}

void XeVertexShaderManager::GetLightCountByType(XeLightSet *_pLightSet, 
                                                ULONG _ulBaseLightIndex,
                                                ULONG _ulRemainingLights, 
                                                ULONG &_ulDirLightCount, 
                                                ULONG &_ulOmniLightCount, 
                                                ULONG &_ulSpotLightCount, 
                                                ULONG &_ulCylSpotLightCount)
{
    if ((_pLightSet != NULL) && (_ulRemainingLights > 0))
    {
        ULONG   ulRemDirLightCount, ulRemOmniLightCount, ulRemSpotLightCount, ulRemCylSpotLightCount;
        INT     iCurLight = (INT)_pLightSet->ulDirLightCount - _ulBaseLightIndex;

        // figure out all the lights left for this pass according to m_iLightBase
        ulRemDirLightCount     = (ULONG) max(iCurLight, 0);
        iCurLight += (INT)_pLightSet->ulOmniLightCount - (INT)ulRemDirLightCount;
        ulRemOmniLightCount    = (ULONG) max(iCurLight, 0);
        iCurLight += (INT)_pLightSet->ulSpotLightCount - (INT)ulRemOmniLightCount;
        ulRemSpotLightCount    = (ULONG) max(iCurLight, 0);
        iCurLight += (INT)_pLightSet->ulCylSpotLightCount - (INT)ulRemSpotLightCount;
        ulRemCylSpotLightCount = (ULONG) max(iCurLight, 0);

        // select remaining lights according to the number of lights for this pass
        _ulDirLightCount     = min(ulRemDirLightCount, _ulRemainingLights);
        _ulRemainingLights -= _ulDirLightCount;
        _ulOmniLightCount    = min(ulRemOmniLightCount, _ulRemainingLights);
        _ulRemainingLights -= _ulOmniLightCount;
        _ulSpotLightCount    = min(ulRemSpotLightCount, _ulRemainingLights);
        _ulRemainingLights -= _ulSpotLightCount;
        _ulCylSpotLightCount = min(ulRemCylSpotLightCount, _ulRemainingLights);
    }
    else
    {
        // invalid call
        _ulDirLightCount        = 0;
        _ulOmniLightCount       = 0;
        _ulSpotLightCount       = 0;
        _ulCylSpotLightCount    = 0;
    }
}

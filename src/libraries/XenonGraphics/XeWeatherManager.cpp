// ------------------------------------------------------------------------------------------------
// File   : XeWeatherManager.cpp
// Date   : 2005-04-14
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"

#include "ENGine/Sources/MoDiFier/MDFmodifier_Weather.h"
#include "SDK/Sources/Random/PerlinNoise.h"

#include "XeMesh.h"
#include "XeMaterial.h"
#include "XeRenderer.h"
#include "XeShader.h"
#include "XeVertexShaderManager.h"
#include "XeTextureManager.h"
#include "XeRenderTargetManager.h"
#include "XeWeatherManager.h"

//#define SC_DEV

// ------------------------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------------------------
XeWeatherManager g_oXeWeatherManager;

static MATH_tdst_Matrix s_IdentityMatrix;

// ------------------------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------------------------

const ULONG XE_WM_RAIN_VERTEX_FORMAT   = XEVC_POSITION | XEVC_COLOR0 | XEVC_TEXCOORD0 | XEVC_TEXCOORD1;
const ULONG XE_WM_FILTER_VERTEX_FORMAT = XEVC_POSITION | XEVC_TEXCOORD0;

#if defined(_XENON)
#define XE_WM_USE_BACKBUFFER TRUE
#else
#define XE_WM_USE_BACKBUFFER FALSE
#endif

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

XeWeatherManager::XeWeatherManager(void)
: m_pRainMesh(NULL), m_pRainRenderable(NULL), 
  m_pRainMaterial(NULL), m_pRainBlurMaterial(NULL), m_pRainApplyMaterial(NULL), 
  m_pFilterMesh(NULL), m_pFilterRenderable(NULL), 
  m_bInitialized(FALSE)
{
    for (ULONG i = 0; i < 2; ++i)
    {
        m_pBlurBuffers[i]    = NULL;
        m_uiBlurBufferIds[i] = (UINT)MAT_Xe_InvalidTextureId;
    }

    m_pRainParamsEngine = &m_stRainParams[0];

#ifdef _XENON
    m_pRainParamsRender = &m_stRainParams[1];
#else
    m_pRainParamsRender = m_pRainParamsEngine;
#endif
}

XeWeatherManager::~XeWeatherManager(void)
{
}

void XeWeatherManager::Initialize(void)
{
    InitializeRain();
    InitializeFiltering();
    InitializeBlurBuffers();

    m_pRainApplyMaterial->SetTextureId(0, m_uiBlurBufferIds[0]);
    m_pRainApplyMaterial->SetTextureId(1, m_uiBlurBufferIds[1]);
    m_pRainApplyMaterial->SetTextureId(2, XESCRATCHBUFFER0_ID);

    ResetParameters();

    MATH_SetIdentityMatrix(&s_IdentityMatrix);

    Reset();

    m_bInitialized = TRUE;
}

void XeWeatherManager::OnDeviceLost(void)
{
    for (ULONG i = 0; i < 2; ++i)
    {
        SAFE_RELEASE(m_pBlurBuffers[i]);
    }

    Reset();
}

void XeWeatherManager::OnDeviceReset(void)
{
    InitializeBlurBuffers();

    Reset();
}

void XeWeatherManager::Shutdown(void)
{
    ShutdownBlurBuffers();
    ShutdownFiltering();
    ShutdownRain();

    Reset();

    m_bInitialized = FALSE;
}

void XeWeatherManager::Reset(void)
{
    m_pRainParamsEngine->m_ulNbRainEntries = 0;
}

void XeWeatherManager::ReInit()
{
	// ReInit Rain Effect when F6 is pressed
	m_pRainParamsEngine->m_fRainSpecularFactor = 0.0f;
	m_pRainParamsEngine->m_fRainDiffuseFactor  = 0.0f;
	m_pRainParamsEngine->m_fRainEffectLastUpdateTime = TIM_f_Clock_TrueRead();
}

void XeWeatherManager::ApplyRain(void)
{
    ULONG ulLastTextureId;

    if (m_pRainParamsRender->m_ulNbRainEntries == 0)
        return;

    if (m_pRainParamsRender->m_fRainIntensity < 0.01f)
        return;

    CXBBeginEventObject oEvent("XeWeatherManager::ApplyRain");

    // Clear alpha so we can render the rain mask
    g_oXeRenderer.ClearAlphaOnly(0);

    // Render the rain mask (alpha only)
    for (m_pRainParamsRender->m_ulCurrentRainEntry = 0; m_pRainParamsRender->m_ulCurrentRainEntry < m_pRainParamsRender->m_ulNbRainEntries; ++m_pRainParamsRender->m_ulCurrentRainEntry)
    {
        XeMesh* pMesh = NULL;

        // get transform
        m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pMaterial->GetTransform(
            0, (D3DXMATRIX*)&m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].mTexCoordTransform, 
            m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].ulWorldIndex
            );

        if (m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].bDynamic)
        {
            pMesh = m_pRainMesh;
        }
        else
        {
            OBJ_tdst_GameObject* pstGO = m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pGO;
            if (OBJ_b_TestIdentityFlag(pstGO, OBJ_C_IdentityFlag_Visu))
            {
                GRO_tdst_Visu* pstVisu = pstGO->pst_Base->pst_Visu;

                if (pstVisu->l_NbXeElements > 0)
                {
                    pMesh = pstVisu->p_XeElements[0].pst_Mesh;
                }
            }
        }

        if (pMesh != NULL)
        {
            m_pRainRenderable->SetMesh(pMesh);
            m_pRainRenderable->SetMaterial(m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pMaterial);
            m_pRainRenderable->SetWorldMatrixIndex(m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].ulWorldIndex);
            m_pRainRenderable->SetWorldViewMatrixIndex(m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].ulWorldViewIndex);
            m_pRainRenderable->SetProjMatrixIndex(m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].ulProjectionIndex);

            if (!m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].bDynamic)
            {
                m_pRainRenderable->SetBlendingMode(m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pMaterial->GetJadeFlags());

                m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pMaterial->SetCustomVS(CUSTOM_VS_RAIN);
                m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pMaterial->SetCustomVSFeature(Rain_Feature_Mode, Rain_VSMode_Normal);
                m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pMaterial->SetCustomVSFeature(Rain_Feature_Dynamic, 0);
                m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pMaterial->SetCustomPS(CUSTOM_PS_RAIN);
                m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pMaterial->SetCustomPSFeature(Rain_Feature_Mode, Rain_PSMode_Normal);
            }

            g_oXeRenderer.RenderObject(m_pRainRenderable, XeFXManager::RP_DEFAULT);

            if (!m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].bDynamic)
            {
                m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pMaterial->SetCustomVS(0);
                m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].pMaterial->SetCustomPS(0);
            }
        }
    }

    g_oXeTextureMgr.ResolveToScratchBuffer(0);
    ulLastTextureId = XESCRATCHBUFFER0_ID;

    FLOAT fTexelWidth  = 1.0f / (FLOAT)m_dwBlurBufferWidth;
    FLOAT fTexelHeight = 1.0f / (FLOAT)m_dwBlurBufferHeight;
    FLOAT fBlurFactor  = 0.5f;

    for (ULONG i = 0; i < 2; ++i)
    {
        if (g_oXeRenderTargetMgr.BeginRenderTarget(0, m_pBlurBuffers[i], NULL, FALSE, TRUE, 0, 
                                                   FALSE, 0.0f, XE_WM_USE_BACKBUFFER && TRUE, D3DFMT_A8R8G8B8, D3DFMT_D16, g_oXeRenderer.GetMultiSampleType()))
        {
            // Update the material
            m_pRainBlurMaterial->SetTextureId(0, ulLastTextureId);
            m_pRainBlurMaterial->SetCustomVSFeature(Rain_Feature_Mode, Rain_VSMode_Blur);
            m_pRainBlurMaterial->SetCustomPSFeature(Rain_Feature_Mode, Rain_PSMode_Blur);

            m_avTexOffsets[0][0] = fBlurFactor * ((FLOAT)i + 1.0f) * -fTexelWidth; m_avTexOffsets[0][1] = fBlurFactor * ((FLOAT)i + 1.0f) * -fTexelHeight;
            m_avTexOffsets[0][2] = fBlurFactor * ((FLOAT)i + 1.0f) *  fTexelWidth; m_avTexOffsets[0][3] = fBlurFactor * ((FLOAT)i + 1.0f) * -fTexelHeight;
            m_avTexOffsets[1][0] = fBlurFactor * ((FLOAT)i + 1.0f) *  fTexelWidth; m_avTexOffsets[1][1] = fBlurFactor * ((FLOAT)i + 1.0f) *  fTexelHeight;
            m_avTexOffsets[1][2] = fBlurFactor * ((FLOAT)i + 1.0f) * -fTexelWidth; m_avTexOffsets[1][3] = fBlurFactor * ((FLOAT)i + 1.0f) *  fTexelHeight;

            m_pFilterRenderable->SetMaterial(m_pRainBlurMaterial);
            m_pFilterRenderable->SetPrimType(XeRenderObject::TriangleStrip);

            // Apply blur
            g_oXeRenderer.RenderObject(m_pFilterRenderable, XeFXManager::RP_DEFAULT);

            g_oXeRenderTargetMgr.EndRenderTarget(0, FALSE, FALSE, NULL, FALSE);

            ulLastTextureId = m_uiBlurBufferIds[i];
        }
    }

    // Apply the rain!
    m_pFilterRenderable->SetMaterial(m_pRainApplyMaterial);
    m_pFilterRenderable->SetPrimType(XeRenderObject::TriangleStrip);
    g_oXeRenderer.RenderObject(m_pFilterRenderable, XeFXManager::RP_DEFAULT);

#if defined(SC_DEV)
    static bool bDrawTextures = true;
    static bool bDumpTextures = false;

    if (bDrawTextures)
    {
        DrawRectangleEx(0.75f, 0.00f, 1.00f, 0.25f, 0.0f, 0.0f, 1.0f, 1.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Copy,  m_uiBlurBufferIds[0]);
        DrawRectangleEx(0.75f, 0.25f, 1.00f, 0.50f, 0.0f, 0.0f, 1.0f, 1.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Copy,  m_uiBlurBufferIds[1]);
        DrawRectangleEx(0.75f, 0.50f, 1.00f, 0.75f, 0.0f, 0.0f, 1.0f, 1.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Alpha, XESCRATCHBUFFER0_ID);
    }

    if (bDumpTextures)
    {
        D3DXSaveTextureToFile("Y:\\ScratchBuffer0.TGA", D3DXIFF_TGA, g_oXeTextureMgr.GetTextureFromID(XESCRATCHBUFFER0_ID),  NULL);
        D3DXSaveTextureToFile("Y:\\BlurBuffer0.TGA",    D3DXIFF_TGA, g_oXeTextureMgr.GetTextureFromID(m_uiBlurBufferIds[0]), NULL);
        D3DXSaveTextureToFile("Y:\\BlurBuffer1.TGA",    D3DXIFF_TGA, g_oXeTextureMgr.GetTextureFromID(m_uiBlurBufferIds[1]), NULL);
    }
#endif

    m_pRainParamsRender->m_ulNbRainEntries = 0;
}

void XeWeatherManager::SetRainIntensity(FLOAT _fIntensity)
{
	if (_fIntensity > 0.0f)
		m_pRainParamsEngine->m_bIsRaining = TRUE;
	else 
		m_pRainParamsEngine->m_bIsRaining = FALSE;

    m_pRainParamsEngine->m_fRainIntensity = MATH_f_FloatLimit(_fIntensity, 0.0f, 1.0f);
}

void XeWeatherManager::SetRainMode(BOOL _bDynamicOnly)
{
    m_pRainParamsEngine->m_bRainDynamicOnly = _bDynamicOnly;
}

void XeWeatherManager::SetLightningIntensity(FLOAT _fIntensity)
{
    m_pRainParamsEngine->m_fLightningIntensity = MATH_f_FloatLimit(_fIntensity, 0.0f, 1.0f);
}

void XeWeatherManager::SetWindParameters(MATH_tdst_Vector* _pDirection, FLOAT _fValue, FLOAT _fDev)
{
    MATH_NormalizeAnyVector(&m_pRainParamsEngine->m_vWindDirection, _pDirection);
    m_pRainParamsEngine->m_fWindValue = _fValue;
    m_pRainParamsEngine->m_fWindDev   = _fDev;
}

void XeWeatherManager::GetRainEffectFactors(
	float _fSpecDry, float _fSpecWet, 
	float _fDiffDry, float _fDiffWet,
	float _fDelay, float _fDecay,
	float* _specBoost,
	float* _diffFactor)
{
	float fCurrentTime = TIM_f_Clock_TrueRead();

	// If rain has started, ramp up specular factor
	if (m_pRainParamsEngine->m_bIsRaining)
	{
		if (_fDelay < 0.1f)
			_fDelay = 0.1f;
		float fSpecIncRate = (_fSpecWet - _fSpecDry)/_fDelay;
		float fDiffIncRate = (_fDiffWet - _fDiffDry)/_fDelay;

		m_pRainParamsEngine->m_fRainSpecularFactor += fSpecIncRate*(fCurrentTime-m_pRainParamsEngine->m_fRainEffectLastUpdateTime);
		m_pRainParamsEngine->m_fRainDiffuseFactor  += fDiffIncRate*(fCurrentTime-m_pRainParamsEngine->m_fRainEffectLastUpdateTime);
	}
	else 
	{
		if (_fDecay < 0.1f)
			_fDecay = 0.1f;
		float fSpecDecRate = (_fSpecWet - _fSpecDry)/_fDecay;
		float fDiffDecRate = (_fDiffWet - _fDiffDry)/_fDecay;

		m_pRainParamsEngine->m_fRainSpecularFactor -= fSpecDecRate*(fCurrentTime-m_pRainParamsEngine->m_fRainEffectLastUpdateTime);
		m_pRainParamsEngine->m_fRainDiffuseFactor  -= fDiffDecRate*(fCurrentTime-m_pRainParamsEngine->m_fRainEffectLastUpdateTime);
	}

	if (_fSpecWet > _fSpecDry)
		m_pRainParamsEngine->m_fRainSpecularFactor = MATH_f_FloatLimit(m_pRainParamsEngine->m_fRainSpecularFactor, _fSpecDry, _fSpecWet);
	else
		m_pRainParamsEngine->m_fRainSpecularFactor = MATH_f_FloatLimit(m_pRainParamsEngine->m_fRainSpecularFactor, _fSpecWet, _fSpecDry);

	if (_fDiffWet > _fDiffDry)
		m_pRainParamsEngine->m_fRainDiffuseFactor  = MATH_f_FloatLimit(m_pRainParamsEngine->m_fRainDiffuseFactor, _fDiffDry, _fDiffWet);
	else
		m_pRainParamsEngine->m_fRainDiffuseFactor  = MATH_f_FloatLimit(m_pRainParamsEngine->m_fRainDiffuseFactor, _fDiffWet, _fDiffDry);

	m_pRainParamsEngine->m_fRainEffectLastUpdateTime = fCurrentTime;

	*_specBoost  = m_pRainParamsEngine->m_fRainSpecularFactor;
	*_diffFactor = m_pRainParamsEngine->m_fRainDiffuseFactor;
}

void XeWeatherManager::ResetParameters(void)
{
    // Rain
    m_pRainParamsEngine->m_fRainIntensity      = 0.0f;
    m_pRainParamsEngine->m_fRainAlphaBoost     = 16.0f;
    m_pRainParamsEngine->m_fRainAlphaIntensity = 0.25f;
    m_pRainParamsEngine->m_bRainApplyWind      = FALSE;
    m_pRainParamsEngine->m_bRainDynamicOnly    = FALSE;
	m_pRainParamsEngine->m_bIsRaining		  = FALSE;
	m_pRainParamsEngine->m_fRainSpecularFactor = 0.0f;
	m_pRainParamsEngine->m_fRainDiffuseFactor  = 0.0f;
	m_pRainParamsEngine->m_fRainEffectLastUpdateTime = TIM_f_Clock_TrueRead();

    // Lightnings
    m_pRainParamsEngine->m_fLightningIntensity = 0.0f;

    // Wind
    m_pRainParamsEngine->m_fWindValue       = 1.0f;
    m_pRainParamsEngine->m_fWindDev         = 0.4f;
    m_pRainParamsEngine->m_vWindDirection.x = 1.0f;
    m_pRainParamsEngine->m_vWindDirection.y = m_pRainParamsEngine->m_vWindDirection.z = 0.0f;
}

void XeWeatherManager::InitializeRain(void)
{
    const FLOAT fStartPlaneRadius[Rain_MaxPlanes] = { 1.0f, 3.0f, 5.0f, 7.5f, 10.0f, 0.1f };
    const FLOAT fEndPlaneRadius[Rain_MaxPlanes]   = { 1.0f, 3.0f, 5.0f, 7.5f, 10.0f, 1.0f };
    const FLOAT fHalfHeight       = 10.0f;
    const ULONG ulNbVertsPerPlane = (Rain_MaxFaces + 1) * 2;
    const FLOAT fAngleStep        = Cf_2Pi / (FLOAT)Rain_MaxFaces;
    const FLOAT fUMaxDistance     = 1.0f / (FLOAT)Rain_MaxFaces;
    XeIndexBuffer* pstIndices  = NULL;
    USHORT*        pwIndices   = NULL;
    RainVertex*    pstVertices = NULL;
    ULONG ulNbVertices = (Rain_MaxFaces + 1) * Rain_MaxPlanes * 2;
    ULONG ulNbIndices  = Rain_MaxFaces * Rain_MaxPlanes * 2 * 3;
    ULONG ulIndex      = 0;
    ULONG ulBaseVertex = 0;
    FLOAT fAngle;
    FLOAT fUOffset;
    FLOAT fVOffset;
    ULONG i;
    ULONG j;

    // Create the mesh
    m_pRainMesh = new XeMesh();

    // Indices
    pstIndices   = g_XeBufferMgr.CreateIndexBuffer(ulNbIndices);
    pwIndices    = (USHORT*)pstIndices->Lock(ulNbIndices);
    ulIndex      = 0;
    ulBaseVertex = 0;
    for (j = 0; j < Rain_MaxPlanes; ++j)
    {
        for (i = 0; i < Rain_MaxFaces; ++i)
        {
            pwIndices[ulIndex++] = (USHORT)(((i + 0) * 2) + 0 + ulBaseVertex);
            pwIndices[ulIndex++] = (USHORT)(((i + 1) * 2) + 0 + ulBaseVertex);
            pwIndices[ulIndex++] = (USHORT)(((i + 0) * 2) + 1 + ulBaseVertex);

            pwIndices[ulIndex++] = (USHORT)(((i + 1) * 2) + 0 + ulBaseVertex);
            pwIndices[ulIndex++] = (USHORT)(((i + 1) * 2) + 1 + ulBaseVertex);
            pwIndices[ulIndex++] = (USHORT)(((i + 0) * 2) + 1 + ulBaseVertex);
        }

        ulBaseVertex += ulNbVertsPerPlane;
    }
    pstIndices->Unlock();
    m_pRainMesh->SetIndices(pstIndices);
 
    // Vertices
    m_pRainMesh->AddStream(XE_WM_RAIN_VERTEX_FORMAT, FALSE, NULL, ulNbVertices);
    pstVertices = (RainVertex*)m_pRainMesh->GetStream(0)->pBuffer->Lock(ulNbVertices, sizeof(RainVertex));
    ulIndex     = 0;
    for (j = 0; j < Rain_MaxPlanes; ++j)
    {
        fUOffset = fRand(0.0f, 0.3f);
        fVOffset = fRand(0.0f, 0.5f);
        fAngle   = 0.0f;

        for (i = 0; i < Rain_MaxFaces + 1; ++i)
        {
            FLOAT fDispl  = fRand(0.0f, 0.4f);
            FLOAT fRadius = fStartPlaneRadius[j] + fStartPlaneRadius[j] * fDispl;

            pstVertices[ulIndex].fX = fRadius * fCos(fAngle);
            pstVertices[ulIndex].fY = fRadius * fSin(fAngle);
            pstVertices[ulIndex].fZ = fHalfHeight;
            pstVertices[ulIndex].ulColor = 0xffffffff;
            pstVertices[ulIndex].fU = fUOffset + ((FLOAT)i / (FLOAT)Rain_MaxFaces);
            pstVertices[ulIndex].fV = 1.0f + fVOffset;
            pstVertices[ulIndex].fWindFactor = -1.0f * ((FLOAT)j * 4.0f + 1.0f);
            pstVertices[ulIndex].fAlphaBoost = 1.0f + ((FLOAT)j / (FLOAT)Rain_MaxPlanes);
            ++ulIndex;

            fRadius = fEndPlaneRadius[j] + fEndPlaneRadius[j] * fDispl;

            // Add a random offset in the texture coordinates to break the linear pattern
            fUOffset += 0.5f * fRand(-fUMaxDistance, fUMaxDistance);

            pstVertices[ulIndex].fX =  fRadius * fCos(fAngle);
            pstVertices[ulIndex].fY =  fRadius * fSin(fAngle);
            pstVertices[ulIndex].fZ = -fHalfHeight;
            pstVertices[ulIndex].ulColor = 0xffffffff;
            pstVertices[ulIndex].fU = fUOffset + ((FLOAT)i / (FLOAT)Rain_MaxFaces);
            pstVertices[ulIndex].fV = 0.0f + fVOffset;
            pstVertices[ulIndex].fWindFactor = 1.0f * ((FLOAT)j * 4.0f + 1.0f);
            pstVertices[ulIndex].fAlphaBoost = 1.0f + ((FLOAT)j / (FLOAT)Rain_MaxPlanes);
            ++ulIndex;

            fAngle += fAngleStep;
        }
    }
    m_pRainMesh->GetStream(0)->pBuffer->Unlock();

    // Create the material
    m_pRainMaterial = new XeMaterial();
    m_pRainMaterial->SetZState(TRUE, FALSE);
    m_pRainMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
    m_pRainMaterial->SetColorSource(MAT_Cc_ColorOp_ConstantColor);
    m_pRainMaterial->SetConstantColor(0x20202020);
    m_pRainMaterial->SetCustomVS(CUSTOM_VS_RAIN);
    m_pRainMaterial->SetCustomVSFeature(Rain_Feature_Mode,    Rain_VSMode_Normal);
    m_pRainMaterial->SetCustomVSFeature(Rain_Feature_Dynamic, 1);
    m_pRainMaterial->SetCustomPS(CUSTOM_PS_RAIN);
    m_pRainMaterial->SetCustomPSFeature(Rain_Feature_Mode, Rain_PSMode_Normal);
    m_pRainMaterial->AddTextureStage();
    m_pRainMaterial->SetAddressMode(0, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP);
    m_pRainMaterial->SetFilterMode(0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR);

    // Rain blur material
    m_pRainBlurMaterial = new XeMaterial();
    m_pRainBlurMaterial->SetCustomVS(CUSTOM_VS_RAIN);
    m_pRainBlurMaterial->SetCustomVSFeature(Rain_Feature_Mode, Rain_VSMode_Blur);
    m_pRainBlurMaterial->SetCustomPS(CUSTOM_PS_RAIN);
    m_pRainBlurMaterial->SetCustomPSFeature(Rain_Feature_Mode, Rain_PSMode_Blur);
    m_pRainBlurMaterial->SetZState(FALSE, FALSE);
    m_pRainBlurMaterial->SetAlphaBlend(FALSE);
    m_pRainBlurMaterial->AddTextureStage();
    m_pRainBlurMaterial->SetFilterMode(0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR);
    m_pRainBlurMaterial->SetAddressMode(0, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);

    // Material to apply the rain over the scene
    m_pRainApplyMaterial = new XeMaterial();
    m_pRainApplyMaterial->SetCustomVS(CUSTOM_VS_RAIN);
    m_pRainApplyMaterial->SetCustomVSFeature(Rain_Feature_Mode, Rain_VSMode_Apply);
    m_pRainApplyMaterial->SetCustomPS(CUSTOM_PS_RAIN);
    m_pRainApplyMaterial->SetCustomPSFeature(Rain_Feature_Mode, Rain_PSMode_Apply);
    m_pRainApplyMaterial->SetZState(FALSE, FALSE);
    m_pRainApplyMaterial->SetAlphaBlend(FALSE);
    for (i = 0; i < 3; ++i)
    {
        // Linear interpolation for the blur buffers (2:1), nearest for the back buffer copy (1:1)
        ULONG ulFilter = i < 2 ? D3DTEXF_LINEAR : D3DTEXF_POINT;

        m_pRainApplyMaterial->AddTextureStage();
        m_pRainApplyMaterial->SetFilterMode(i, ulFilter, ulFilter, ulFilter);
        m_pRainApplyMaterial->SetAddressMode(i, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
    }

    // Rain renderable
    m_pRainRenderable = new XeRenderObject();
    m_pRainRenderable->SetDrawMask(0xffffffff & ~(GDI_Cul_DM_Lighted | GDI_Cul_DM_Fogged | GDI_Cul_DM_TestBackFace | GDI_Cul_DM_EmitShadowBuffer | GDI_Cul_DM_ReceiveShadowBuffer));
    m_pRainRenderable->SetMesh(m_pRainMesh);
    m_pRainRenderable->SetMaterial(m_pRainMaterial);
    m_pRainRenderable->SetPrimType(XeRenderObject::TriangleList);
    m_pRainRenderable->SetObjectType(XeRenderObject::Rain);
    m_pRainRenderable->SetExtraDataIndex(0);

    // Pseudo-random offset to break the pattern
    for (i = 0; i < Rain_MaxRandOffsets; ++i)
    {
        m_afRandOffsets[i] = fRand(-1.0f, 1.0f);
    }
}

void XeWeatherManager::ShutdownRain(void)
{
    SAFE_DELETE(m_pRainMesh);
    SAFE_DELETE(m_pRainMaterial);
    SAFE_DELETE(m_pRainBlurMaterial);
    SAFE_DELETE(m_pRainApplyMaterial);
    SAFE_DELETE(m_pRainRenderable);

    m_pRainParamsEngine->m_ulNbRainEntries = 0;
}

void XeWeatherManager::InitializeFiltering(void)
{
    FilterVertex* pstVertices = NULL;
    ULONG ulNbVertices = 4;
    ULONG ulIndex      = 0;

    // Create the mesh
    m_pFilterMesh = new XeMesh();

    // Vertices
    m_pFilterMesh->AddStream(XE_WM_FILTER_VERTEX_FORMAT, FALSE, NULL, ulNbVertices);
    pstVertices = (FilterVertex*)m_pFilterMesh->GetStream(0)->pBuffer->Lock(ulNbVertices, sizeof(FilterVertex));

    ulIndex     = 0;
    pstVertices[ulIndex].fX = -1.0f;
    pstVertices[ulIndex].fY =  1.0f;
    pstVertices[ulIndex].fZ =  0.0f;
    pstVertices[ulIndex].fU =  0.0f;
    pstVertices[ulIndex].fV =  0.0f;
    ++ulIndex; 

    pstVertices[ulIndex].fX =  1.0f;
    pstVertices[ulIndex].fY =  1.0f;
    pstVertices[ulIndex].fZ =  0.0f;
    pstVertices[ulIndex].fU =  1.0f;
    pstVertices[ulIndex].fV =  0.0f;
    ++ulIndex;

    pstVertices[ulIndex].fX = -1.0f;
    pstVertices[ulIndex].fY = -1.0f;
    pstVertices[ulIndex].fZ =  0.0f;
    pstVertices[ulIndex].fU =  0.0f;
    pstVertices[ulIndex].fV =  1.0f;
    ++ulIndex;

    pstVertices[ulIndex].fX =  1.0f;
    pstVertices[ulIndex].fY = -1.0f;
    pstVertices[ulIndex].fZ =  0.0f;
    pstVertices[ulIndex].fU =  1.0f;
    pstVertices[ulIndex].fV =  1.0f;
    ++ulIndex;

    m_pFilterMesh->GetStream(0)->pBuffer->Unlock();

    // Create the renderable
    m_pFilterRenderable = new XeRenderObject();
    m_pFilterRenderable->SetDrawMask(0xffffffff & ~(GDI_Cul_DM_Lighted | GDI_Cul_DM_Fogged | GDI_Cul_DM_TestBackFace));
    m_pFilterRenderable->SetMesh(m_pFilterMesh);
    m_pFilterRenderable->SetPrimType(XeRenderObject::TriangleStrip);
    m_pFilterRenderable->SetObjectType(XeRenderObject::Rain);
    m_pFilterRenderable->SetExtraDataIndex(0xffffffff);
}

void XeWeatherManager::ShutdownFiltering(void)
{
    SAFE_DELETE(m_pFilterMesh);
    SAFE_DELETE(m_pFilterRenderable);
}

void XeWeatherManager::InitializeBlurBuffers(void)
{
    HRESULT hr;
    UINT    uiBackBufferWidth;
    UINT    uiBackBufferHeight;
    ULONG   i;

    // Blur buffers are always 1/4 of the screen size (min of 8x8)
    g_oXeRenderer.GetBackbufferResolution(&uiBackBufferWidth, &uiBackBufferHeight);
    m_dwBlurBufferWidth  = (uiBackBufferWidth  >> 1);
    m_dwBlurBufferHeight = (uiBackBufferHeight >> 1);
    if (m_dwBlurBufferWidth < 8)
        m_dwBlurBufferWidth = 8;
    if (m_dwBlurBufferHeight < 8)
        m_dwBlurBufferHeight = 8;

    for (i = 0; i < 2; ++i)
    {
        hr = g_oXeRenderer.GetDevice()->CreateTexture(m_dwBlurBufferWidth, m_dwBlurBufferHeight, 
                                                      1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
                                                      D3DPOOL_DEFAULT, &m_pBlurBuffers[i], NULL);
        XeValidateRet(SUCCEEDED(hr) && (m_pBlurBuffers[i] != NULL), , "Failed to create a rain blur buffer");

        if (m_uiBlurBufferIds[i] == (UINT)MAT_Xe_InvalidTextureId)
        {
            m_uiBlurBufferIds[i] = g_oXeTextureMgr.RegisterUserTexture(m_pBlurBuffers[i]);
        }
        else
        {
            g_oXeTextureMgr.UpdateUserTexture(m_uiBlurBufferIds[i], m_pBlurBuffers[i]);
        }
    }
}

void XeWeatherManager::ShutdownBlurBuffers(void)
{
    for (ULONG i = 0; i < 2; ++i)
    {
        SAFE_RELEASE(m_pBlurBuffers[i]);
        m_uiBlurBufferIds[i] = (UINT)MAT_Xe_InvalidTextureId;
    }
}

void XeWeatherManager::AddRainFX(OBJ_tdst_GameObject* _pst_GO, MDF_tdst_Weather_* _pst_Weather)
{
    BOOL             bIsDynamic  = FALSE;
    ULONG            ulColor     = 0x0a0a0a0a;
    XeMaterial*      pXeMaterial = NULL;
    MATH_tdst_Matrix mOGLMatrix;

    if (!m_bInitialized)
        return;

    if (!m_pRainParamsEngine->m_bIsRaining)
        return;

    if (m_pRainParamsEngine->m_bRainDynamicOnly && !(_pst_Weather->st_RainFX.ul_Flags & MDF_Weather_RainFX_Flag_Dynamic))
        return;

    if (m_pRainParamsEngine->m_ulNbRainEntries == Rain_MaxEntries)
    {
#if defined(SC_DEV)
        ERR_X_Assert(FALSE && "Too many rain instances");
#else
        ERR_OutputDebugString("[XeWM] AddRainFX() - Too many rain instances\n");
#endif
        return;
    }

    if (_pst_Weather->st_RainFX.ul_Flags & MDF_Weather_RainFX_Flag_Dynamic)
        bIsDynamic = TRUE;

    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        MAT_tdst_Material* pMaterial = (MAT_tdst_Material*)_pst_GO->pst_Base->pst_Visu->pst_Material;
        if (pMaterial->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
        {
            MAT_tdst_MultiTexture* pMultiTex = (MAT_tdst_MultiTexture*)pMaterial;
            if (pMultiTex->pst_FirstLevel)
            {
                if (bIsDynamic)
                {
                    pXeMaterial = m_pRainMaterial;

                    // Use the same texture
                    m_pRainMaterial->SetTextureId(0, pMultiTex->pst_FirstLevel->s_TextureId);
                }
                else
                {
                    pXeMaterial = pMultiTex->pst_FirstLevel->pst_XeMaterial;
                }

                // Specular (constant) color contains the color to use when rendering
                ULONG ulAlpha = ((pMultiTex->ul_Specular >> 16) & 0xff) + 
                                ((pMultiTex->ul_Specular >>  8) & 0xff) + 
                                 (pMultiTex->ul_Specular        & 0xff);
                if (ulAlpha > 255) ulAlpha = 255;
                ulColor = (pMultiTex->ul_Specular & 0x00ffffff) | (ulAlpha << 24);
                ulColor = XeConvertColor(ulColor);
            }
        }
    }

    MATH_MakeOGLMatrix(&mOGLMatrix, _pst_GO->pst_GlobalMatrix);
    g_pXeContextManagerEngine->PushWorldMatrix((D3DXMATRIX*)&mOGLMatrix);

    m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].pGO               = _pst_GO;
    m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].pMaterial         = pXeMaterial;
    m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].bDynamic          = bIsDynamic;
    m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].ulWorldIndex      = g_pXeContextManagerEngine->GetCurrentWorldMatrixIndex();
    m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].ulWorldViewIndex  = g_pXeContextManagerEngine->GetCurrentWorldViewMatrixIndex();
    m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].ulProjectionIndex = g_pXeContextManagerEngine->GetCurrentProjMatrixIndex();
    m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].fScaleU           = _pst_Weather->st_RainFX.f_RainScaleU;
    m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].fScaleV           = _pst_Weather->st_RainFX.f_RainScaleV;
    m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].ulColor           = ulColor;

//    pXeMaterial->GetTransform(0, (D3DXMATRIX*)&m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].mTexCoordTransform, 
//                              m_pRainParamsEngine->m_RainEntries[m_pRainParamsEngine->m_ulNbRainEntries].ulWorldIndex);

    if (bIsDynamic || (m_pRainParamsEngine->m_ulNbRainEntries == 0))
    {
        m_pRainParamsEngine->m_fRainAlphaBoost     = _pst_Weather->st_RainFX.f_AlphaBoost;
        m_pRainParamsEngine->m_fRainAlphaIntensity = _pst_Weather->st_RainFX.f_AlphaIntensity;
        m_pRainParamsEngine->m_bRainApplyWind      = (_pst_Weather->st_RainFX.ul_Flags & MDF_Weather_RainFX_Flag_EnableWind) != 0;
    }

    ++m_pRainParamsEngine->m_ulNbRainEntries;
}

void XeWeatherManager::GetRainTexCoordModifiers(FLOAT* _pVec)
{
    ULONG ulOffset = (ULONG)(TIM_f_Clock_Read() * 10.0f) % Rain_MaxRandOffsets;

    // xy : Scale
    // zw : Offset

    _pVec[0] = m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].fScaleU;
    _pVec[1] = m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].fScaleV;
    _pVec[2] = m_afRandOffsets[ulOffset];
    _pVec[3] = fmodf(2.0f * m_pRainParamsRender->m_fRainIntensity * TIM_f_Clock_Read(), 1.0f);
}

void XeWeatherManager::GetRainTexCoordPosition(FLOAT* _pVec)
{
    D3DXMATRIX* pWorldMat = g_pXeContextManagerRender->GetWorldMatrixByIndex(m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].ulWorldIndex);

    // xy: Position offset
    // zw: FREE

    _pVec[0] = 0.0f;
    _pVec[1] = fAbs(pWorldMat->_41 + pWorldMat->_42) * 0.1f;
    _pVec[2] = 0.0f;
    _pVec[3] = 0.0f;
}

void XeWeatherManager::GetRainWindVector(FLOAT* _pVec)
{
    FLOAT fWindStrength = 0.0f;

    if (m_pRainParamsRender->m_bRainApplyWind)
    {
        fWindStrength = m_pRainParamsRender->m_fWindValue;

        m_pRainParamsRender->m_fWindDev = 0.4f;
        FLOAT fNoise = TIM_f_Clock_TrueRead() * 1000.0f;
        FLOAT ret = -m_pRainParamsRender->m_fWindDev + (g_PerlinNoiseMaker.noise1(fNoise) * 0.5f + 0.5f) * (m_pRainParamsRender->m_fWindDev+m_pRainParamsRender->m_fWindDev);
        ret = MATH_f_FloatLimit(ret, -m_pRainParamsRender->m_fWindDev, m_pRainParamsRender->m_fWindDev);
        fWindStrength = ret;
    }

    // xyz: Wind vector (on the XY plane)
    // w  : 0
    _pVec[0] = m_pRainParamsRender->m_vWindDirection.x * fWindStrength;
    _pVec[1] = m_pRainParamsRender->m_vWindDirection.y * fWindStrength;
    _pVec[2] = 0.0f;
    _pVec[3] = 0.0f;
}

void XeWeatherManager::GetRainBlurTexOffset(FLOAT* _pVec, ULONG _ulIndex)
{
    _pVec[0] = m_avTexOffsets[_ulIndex][0];
    _pVec[1] = m_avTexOffsets[_ulIndex][1];
    _pVec[2] = m_avTexOffsets[_ulIndex][2];
    _pVec[3] = m_avTexOffsets[_ulIndex][3];
}

void XeWeatherManager::GetRainColor(FLOAT* _pVec)
{
    XeJadeColorToV4F((VECTOR4FLOAT*)_pVec, (m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].ulColor & 0xff000000));
}

void XeWeatherManager::GetRainBlurPosOffset(FLOAT* _pVec)
{
    UINT uiWidth;
    UINT uiHeight;

    g_oXeRenderer.GetBackbufferResolution(&uiWidth, &uiHeight);

    _pVec[0] = -1.0f / (FLOAT)uiWidth;
    _pVec[1] =  1.0f / (FLOAT)uiHeight;
    _pVec[2] =  0.0f;
    _pVec[3] =  0.0f;
}

D3DXMATRIX* XeWeatherManager::GetRainTexCoordTransform(void)
{
    return (D3DXMATRIX*)&m_pRainParamsRender->m_RainEntries[m_pRainParamsRender->m_ulCurrentRainEntry].mTexCoordTransform;
}

FLOAT XeWeatherManager::GetRainAlphaBoost(void)
{
    return m_pRainParamsRender->m_fRainAlphaBoost;
}

FLOAT XeWeatherManager::GetRainAlphaIntensity(void)
{
    return m_pRainParamsRender->m_fRainAlphaIntensity;
}

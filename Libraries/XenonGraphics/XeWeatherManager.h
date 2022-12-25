// ------------------------------------------------------------------------------------------------
// File   : XeWeatherManager.h
// Date   : 2005-04-14
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XEWEATHERMANAGER_H
#define GUARD_XEWEATHERMANAGER_H

// ------------------------------------------------------------------------------------------------
// FORWARD DECLARATIONS
// ------------------------------------------------------------------------------------------------
class XeMesh;
class XeMaterial;
class XeRenderObject;

struct MDF_tdst_Weather_;

struct D3DXMATRIX;
struct IDirect3DTexture9;

// ------------------------------------------------------------------------------------------------
// CLASSES
// ------------------------------------------------------------------------------------------------
class XeWeatherManager
{
private:

	enum
	{
		Rain_MaxFaces       = 16,
		Rain_MaxPlanes      = 6,
		Rain_MaxEntries     = 32,
		Rain_MaxRandOffsets = 16,

		Rain_Feature_Mode    = 1,
		Rain_Feature_Dynamic = 2,

		Rain_VSMode_Normal = 0,
		Rain_VSMode_Blur   = 1,
		Rain_VSMode_Apply  = 2,

		Rain_PSMode_Normal = 0,
		Rain_PSMode_Blur   = 1,
		Rain_PSMode_Apply  = 2,
	};

	struct RainVertex
	{
		FLOAT fX;               // XEVC_POSITION
		FLOAT fY;
		FLOAT fZ;
		ULONG ulColor;          // XEVC_COLOR0
		FLOAT fU;               // XEVC_TEXCOORD0
		FLOAT fV;
		FLOAT fWindFactor;      // XEVC_TEXCOORD1
		FLOAT fAlphaBoost;
	};

	struct FilterVertex
	{
		FLOAT fX;               // XEVC_POSITION
		FLOAT fY;
		FLOAT fZ;
		FLOAT fU;               // XEVC_TEXCOORD0
		FLOAT fV;
	};

	struct RainEntry
	{
		OBJ_tdst_GameObject*    pGO;
		XeMaterial*             pMaterial;
		FLOAT                   mTexCoordTransform[16];
		BOOL                    bDynamic;
		ULONG                   ulWorldIndex;
		ULONG                   ulWorldViewIndex;
		ULONG                   ulProjectionIndex;
		FLOAT                   fScaleU;
		FLOAT                   fScaleV;
		ULONG                   ulColor;
	};

    struct RainParams
    {
        // Rain
        FLOAT m_fRainIntensity;
        FLOAT m_fRainAlphaBoost;
        FLOAT m_fRainAlphaIntensity;
        BOOL  m_bRainApplyWind;
        BOOL  m_bRainDynamicOnly;
        BOOL  m_bIsRaining;
        FLOAT m_fRainSpecularFactor;
        FLOAT m_fRainDiffuseFactor;
        FLOAT m_fRainEffectLastUpdateTime;

        RainEntry m_RainEntries[Rain_MaxEntries];
        ULONG     m_ulNbRainEntries;
        ULONG     m_ulCurrentRainEntry;

        // Lightning
        FLOAT m_fLightningIntensity;

        // Wind
        MATH_tdst_Vector m_vWindDirection;
        FLOAT            m_fWindValue;
        FLOAT            m_fWindDev;
    };

public:

	XeWeatherManager(void);
	~XeWeatherManager(void);

	void Initialize(void);
	void OnDeviceLost(void);
	void OnDeviceReset(void);
	void Shutdown(void);

	void Reset(void);
	void ApplyRain(void);

	void SetRainIntensity(FLOAT _fIntensity);
	void SetRainMode(BOOL _bDynamicOnly);
	void SetLightningIntensity(FLOAT _fIntensity);
	void SetWindParameters(MATH_tdst_Vector* _pDirection, FLOAT _fValue, FLOAT _fDev);

	inline FLOAT GetRainIntensity(void) const      { return m_pRainParamsEngine->m_fRainIntensity; }
	inline FLOAT GetLightningIntensity(void) const { return m_pRainParamsEngine->m_fLightningIntensity; }
	void GetRainEffectFactors(float _fSpecDry, float _fSpecWet, 
		float _fDiffDry, float _fDiffWet,
		float _fDelay, float _fDecay,
		float* _specBoost,
		float* _diffFactor);

	void AddRainFX(OBJ_tdst_GameObject* _pst_GO, MDF_tdst_Weather_* _pst_Weather);

	void GetRainTexCoordModifiers(FLOAT* _pVec);
	void GetRainTexCoordPosition(FLOAT* _pVec);
	void GetRainWindVector(FLOAT* _pVec);
	void GetRainBlurTexOffset(FLOAT* _pVec, ULONG _ulIndex);
	void GetRainBlurPosOffset(FLOAT* _pVec);
	void GetRainColor(FLOAT* _pVec);
	D3DXMATRIX* GetRainTexCoordTransform(void);
	FLOAT GetRainAlphaBoost(void);
	FLOAT GetRainAlphaIntensity(void);

	void ReInit(void); 
    void Swap() { RainParams *pTmp = m_pRainParamsEngine; m_pRainParamsEngine = m_pRainParamsRender; m_pRainParamsRender = pTmp; }

private:

	void ResetParameters(void);

	void InitializeRain(void);
	void ShutdownRain(void);

	void InitializeFiltering(void);
	void ShutdownFiltering(void);

	void InitializeBlurBuffers(void);
	void ShutdownBlurBuffers(void);

private:

	BOOL m_bInitialized;

    RainParams      m_stRainParams[2];
    RainParams     *m_pRainParamsEngine;
    RainParams     *m_pRainParamsRender;

	XeMesh*         m_pRainMesh;
	XeMaterial*     m_pRainMaterial;
	XeMaterial*     m_pRainBlurMaterial;
	XeMaterial*     m_pRainApplyMaterial;
	XeRenderObject* m_pRainRenderable;

	XeRenderObject* m_pFilterRenderable;
	XeMesh*         m_pFilterMesh;

	// Blur working buffers
	IDirect3DTexture9* m_pBlurBuffers[2];
	UINT               m_uiBlurBufferIds[2];
	DWORD              m_dwBlurBufferWidth;
	DWORD              m_dwBlurBufferHeight;

	FLOAT m_avTexOffsets[2][4];

	FLOAT m_afRandOffsets[Rain_MaxRandOffsets];
};

// ------------------------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------------------------
extern __declspec(align(32)) XeWeatherManager g_oXeWeatherManager;

#endif // #ifdef GUARD_XEWEATHERMANAGER_H

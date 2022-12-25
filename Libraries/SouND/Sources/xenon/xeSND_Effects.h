#ifndef __xeSND_Effects_h__
#define __xeSND_Effects_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

//#include "assert.h"

#include "SouND/Sources/SNDfx.h"

#include "BASe/MEMory/MEM.h"

// ***********************************************************************************************************************
//    Defines
// ***********************************************************************************************************************

//#ifndef ASSERT
//#define ASSERT assert // For XAUDIO_USE_BATCHALLOC_NEWDELETE macro
//#endif

// ***********************************************************************************************************************

//#define DELAYEFFECT_ID (XAUDIOFXID_FIRSTCLIENT + 0)

// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************
/*
typedef enum
{
	eDelayAlgorithm_None,
	eDelayAlgorithm_AllPass,
	eDelayAlgorithm_Comb,

	eDelayAlgorithm_Count
}
eDelayAlgorithm;
*/
// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************
/*
typedef struct
{
	float			fTime;
	float			fGain;
	eDelayAlgorithm	eAlgorithm;
}
stFXParameters_Delay;
*/
// ***********************************************************************************************************************
//    Effect Construction Functions
// ***********************************************************************************************************************

//HRESULT Effect_Delay_QuerySize(const XAUDIOFXINIT* Param_pstInit, unsigned long* Param_pulEffectSize);
//HRESULT Effect_Delay_Create(const XAUDIOFXINIT* Param_pstInit, IXAudioBatchAllocator* Param_pAllocator, IXAudioEffect** Param_ppEffect);

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

//static const XAUDIOFXINIT		kstEffects_Init_Delay			= {DELAYEFFECT_ID, NULL};
static const XAUDIOFXINIT		kstEffects_Init_Reverb			= {XAUDIOFXID_REVERB, NULL};
//static const XAUDIOFXINIT*		kastEffects_VoiceInit_Delay[]	= {&kstEffects_Init_Delay};
static const XAUDIOFXINIT*		kastEffects_VoiceInit_Reverb[]	= {&kstEffects_Init_Reverb};
//static const XAUDIOVOICEFXCHAIN	kstEffects_VoiceChain_Delay		= {1, kastEffects_VoiceInit_Delay};
static const XAUDIOVOICEFXCHAIN	kstEffects_VoiceChain_Reverb	= {1, kastEffects_VoiceInit_Reverb};

static const XAUDIOFXTABLEENTRY kastEffects_Table[] =
{
	{XAUDIOFXID_SOURCE, XAudioQuerySourceEffectSize, XAudioCreateSourceEffect},
	{XAUDIOFXID_ROUTER, XAudioQueryRouterEffectSize, XAudioCreateRouterEffect},
	{XAUDIOFXID_RENDER, XAudioQueryRenderEffectSize, XAudioCreateRenderEffect},
	//{DELAYEFFECT_ID   , Effect_Delay_QuerySize     , Effect_Delay_Create     },
	{XAUDIOFXID_REVERB, XAudioQueryReverbEffectSize, XAudioCreateReverbEffect}
};

// ***********************************************************************************************************************
/*
const XAUDIOFXPARAMID	kiDelay_ParamID_Time		= 0;
const XAUDIOFXPARAMID	kiDelay_ParamID_Gain		= 1;
const XAUDIOFXPARAMID	kiDelay_ParamID_Algorithm	= 2;

const float				kfDelay_Gain_Min			= 0.00f;
const float				kfDelay_Gain_Max			= 0.65f; // 65%

const float				kfDelay_Time_Min			= 50.0f;
const float				kfDelay_Time_Max			= 1000.0f; // 1 second
const float				kfDelay_Feedback_Damping	= 0.25f;
const float				kfDelay_Time_Scale			= 0.50f;

const float				kfDelay_SamplesPerMS		= (1024.0f * 48.0f) / 1000.0f;

const int				kiDelay_CircularBuffer_Size	= 65536;
const int				kiDelay_CircularBuffer_Mask	= kiDelay_CircularBuffer_Size - 1;

const float				kfDelay_Preset_Free			= -1.0f;
*/
// ***********************************************************************************************************************
//    Effect - Reverb
// ***********************************************************************************************************************

// lRoom               [-10000,       0] in milliBels
// lRoomHF             [-10000,       0] in milliBels
// flRoomRolloffFactor [   0.0,    10.0]
// flDecayTime         [   0.1,    20.0] in seconds
// flDecayHFRatio      [   0.1,     2.0]
// lReflections        [-10000,    1000] in milliBels
// flReflectionsDelay  [   0.0,     0.3] in seconds
// lReverb             [-10000,    2000] in milliBels
// flReverbDelay       [   0.0,     0.1] in seconds
// flDiffusion         [   0.0,   100.0] in percents
// flDensity           [   0.0,   100.0] in percents
// flHFReference       [  20.0, 20000.0] in Hertz

static const XAUDIOREVERBI3DL2SETTINGS kstEffects_Reverb_ModePresets[SND_Cte_FxMode_Nb] =
{
	{-1000, -6000, 0.0f,  0.17f, 0.10f,  -1204, 0.001f,   207, 0.002f, 100.0f, 100.0f, 5000.0f}, // PS2 Mode : Off			(Preset : Padded Cell)
	{-1000,  -454, 0.0f,  0.40f, 0.83f,  -1646, 0.002f,    53, 0.003f, 100.0f, 100.0f, 5000.0f}, // PS2 Mode : Room			(Preset : Room)
	{-1000,  -600, 0.0f,  1.10f, 0.83f,   -400, 0.005f,   500, 0.010f, 100.0f, 100.0f, 5000.0f}, // PS2 Mode : Studio A		(Preset : Small Room)
	{-1000,  -600, 0.0f,  1.30f, 0.83f,  -1000, 0.010f,  -200, 0.020f, 100.0f, 100.0f, 5000.0f}, // PS2 Mode : Studio B		(Preset : Medium Room)
	{-1000,  -600, 0.0f,  1.50f, 0.83f,  -1600, 0.020f, -1000, 0.040f, 100.0f, 100.0f, 5000.0f}, // PS2 Mode : Studio C		(Preset : Large Room)
	{-1000,  -237, 0.0f,  2.70f, 0.79f,  -1214, 0.013f,   395, 0.020f, 100.0f, 100.0f, 5000.0f}, // PS2 Mode : Hall			(Preset : Stone Corridor)
	{-1000,  -698, 0.0f,  7.24f, 0.33f,  -1166, 0.020f,    16, 0.030f, 100.0f, 100.0f, 5000.0f}, // PS2 Mode : Space		(Preset : Arena)
	{-1000, -3300, 0.0f,  1.49f, 0.54f,  -2560, 0.162f,  -613, 0.088f,  79.0f, 100.0f, 5000.0f}, // PS2 Mode : Echo			(Preset : Forest)
	{-1000, -3300, 0.0f,  1.49f, 0.54f,  -2560, 0.162f,  -613, 0.088f,  79.0f, 100.0f, 5000.0f}, // PS2 Mode : Delay		(Preset : Forest)
	{-1000, -1000, 0.0f,  2.81f, 0.14f,    429, 0.014f,   648, 0.021f,  80.0f,  60.0f, 5000.0f}, // PS2 Mode : Pipe			(Preset : SewerPipe)
	{-1000, -2500, 0.0f,  1.49f, 0.21f,  -2780, 0.300f, -2014, 0.100f,  27.0f, 100.0f, 5000.0f}, // PS2 Mode : Mountains	(Preset : Mountains)
	{-1000,  -800, 0.0f,  1.49f, 0.67f,  -2273, 0.007f, -2217, 0.011f,  50.0f, 100.0f, 5000.0f}	 // PS2 Mode : City			(Preset : City)
};

// ***********************************************************************************************************************
//    Effect - Delay
// ***********************************************************************************************************************
/*
#define FREE kfDelay_Preset_Free

static const stFXParameters_Delay kstEffect_Delay_ModePresets[SND_Cte_FxMode_Nb] =
{
	{000.0f, 0.00f, eDelayAlgorithm_None},		// Off
	{000.0f, 0.00f, eDelayAlgorithm_None},		// Room
	{000.0f, 0.00f, eDelayAlgorithm_None},		// Studio A
	{000.0f, 0.00f, eDelayAlgorithm_None},		// Studio B
	{000.0f, 0.00f, eDelayAlgorithm_None},		// Studio C
	{000.0f, 0.00f, eDelayAlgorithm_None},		// Hall
	{000.0f, 0.00f, eDelayAlgorithm_None},		// Space
	{  FREE,  FREE, eDelayAlgorithm_Comb},		// Echo
	{  FREE,  FREE, eDelayAlgorithm_Comb},		// Delay
	{000.0f, 0.00f, eDelayAlgorithm_None},		// Pipe
	{259.0f, 0.20f, eDelayAlgorithm_Comb},		// Mountains
	{251.0f, 0.12f, eDelayAlgorithm_Comb}		// City
};

#undef FREE
*/
// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CFXCore
{
private:
	int					m_iCoreID;
	BOOL				m_bIsEnabled;
	SND_tdst_FxParam	m_stEngineSettings;
	IXAudioSourceVoice*	m_pSourceVoice_EmptyLoop;
	//IXAudioSubmixVoice*	m_pSubmixVoice_Delay;
	IXAudioSubmixVoice*	m_pSubmixVoice_Reverb;
	short				m_aEmptyBuffer[256];
	//BOOL				m_bForcedFeedback;
	//BOOL				m_bForcedDelay;
	//BOOL				m_bForcedDelayAlgorithm;
	//eDelayAlgorithm		m_eDelayAlgorithm;

public:
    M_DeclareOperatorNewAndDelete();

	CFXCore();
	~CFXCore();

	void				Initialize(int Param_iCoreID);
	void				Uninit();
	//IXAudioSubmixVoice*	GetVoice_Delay();
	IXAudioSubmixVoice*	GetVoice_Reverb();
	void				UpdateEffect();
	BOOL				GetEnabled();
	int					GetMode();
	//int					GetDelay();
	//int					GetFeedback();
	float				GetWetVolume();
	int					GetWetPan();
	//eDelayAlgorithm		GetDelayAlgorithm();
	void				SetEnabled(BOOL Param_bIsEnabled);
	void				SetMode(int Param_iMode);
	//void				SetDelay(int Param_iDelay);
	//void				SetFeedback(int Param_iFeedback);
	void				SetWetVolume(float Param_fWetVolume);
	void				SetWetPan(int Param_iWetPan);
	//void				SetDelayAlgorithm(eDelayAlgorithm Param_eDelayAlgorithm);
};

// ***********************************************************************************************************************
/*
class CEffect_Delay : public IXAudioEffect
{
	XAUDIO_USE_BATCHALLOC_NEWDELETE();

private:
	void*					m_pContext;
	int						m_iReferenceCount;

	stFXParameters_Delay	m_stParameters;
	int						m_iDelaySample;

	float*					m_apfCircularBuffer[XAUDIOSPEAKER_COUNT];
	int						m_iCircularOffset;

public:
	CEffect_Delay(void* Param_pContext);

	HRESULT			Initialize(const XAUDIOFXINIT* Param_pstInit, IXAudioBatchAllocator* Param_pAllocator);

	unsigned long	AddRef();
	unsigned long	Release();

	HRESULT			GetInfo(XAUDIOFXINFO* Param_pstInfo);
	HRESULT			GetParam(XAUDIOFXPARAMID Param_iID, XAUDIOFXPARAMTYPE Param_iType, XAUDIOFXPARAM* Param_pstValue);
	HRESULT			SetParam(XAUDIOFXPARAMID Param_iID, XAUDIOFXPARAMTYPE Param_iType, const XAUDIOFXPARAM* Param_pstValue);
	HRESULT			GetContext(void** Param_ppContext);
	HRESULT			Process(IXAudioFrameBuffer* Param_pInputBuffer, IXAudioFrameBuffer* Param_pOutputBuffer);
};
*/
// ***********************************************************************************************************************

#endif // __xeSND_Effects_h__

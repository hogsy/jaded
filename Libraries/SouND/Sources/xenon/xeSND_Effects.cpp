#include "Precomp.h"

#ifdef _XENON

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "SNDvolume.h"

#include "xeSND_Effects.h"

#include "xeSND_Engine.h"

// ***********************************************************************************************************************
//    Class Methods : CFXCore
// ***********************************************************************************************************************

CFXCore::CFXCore()
{
	// Initialize Fields

	m_iCoreID						= -1;
	m_bIsEnabled					= FALSE;
	m_stEngineSettings.i_Mode		= SND_Cte_FxMode_Off;
	//m_pSubmixVoice_Delay			= NULL;
	m_pSubmixVoice_Reverb			= NULL;
	//m_bForcedFeedback				= FALSE;
	//m_bForcedDelay					= FALSE;
	//m_bForcedDelayAlgorithm			= FALSE;
	//m_eDelayAlgorithm				= eDelayAlgorithm_None;

	m_stEngineSettings.ui_FxFlag	= 0;
	m_stEngineSettings.i_Mode		= -1;
	m_stEngineSettings.i_Delay		= 0;
	m_stEngineSettings.i_Feedback	= 0;
	m_stEngineSettings.f_WetVol		= 0.0f;
	m_stEngineSettings.i_WetPan		= 0;
}

// ***********************************************************************************************************************

CFXCore::~CFXCore()
{
	Uninit();
}

// ***********************************************************************************************************************

void CFXCore::Initialize(int Param_iCoreID)
{
#if _XENON_SOUND_FX
	// Update Fields

	m_iCoreID = Param_iCoreID;

	// Set Up Channel Map

	XAUDIOVOICEOUTPUT		VoiceOutput;
	XAUDIOVOICEOUTPUTENTRY	VoiceOutputEntry;
	XAUDIOCHANNELMAP		ChannelMap;
	XAUDIOCHANNELMAPENTRY	ChannelMapEntry[XAUDIOSPEAKER_COUNT];

	ChannelMap.EntryCount			= XAUDIOSPEAKER_COUNT;
	ChannelMap.paEntries			= ChannelMapEntry;

	VoiceOutputEntry.pDestVoice		= NULL;
	VoiceOutputEntry.pChannelMap	= &ChannelMap;

	VoiceOutput.EntryCount			= 1;
	VoiceOutput.paEntries			= &VoiceOutputEntry;

	// Configure Default Outputs and Volumes

	for (int Loop = 0; Loop < sizeof(ChannelMapEntry) / sizeof(XAUDIOCHANNELMAPENTRY); Loop++)
	{
		ChannelMapEntry[Loop].OutputChannel	= (XAUDIOCHANNEL)Loop;
		ChannelMapEntry[Loop].Volume		= XAUDIOVOLUME_MAX;
	}

	// Create XAudio Submix Voices

	XAUDIOSUBMIXVOICEINIT SubmixVoiceInit = {0};

	SubmixVoiceInit.pVoiceOutput = &VoiceOutput;

	// - Delay
/*
	ChannelMapEntry[XAUDIOSPEAKER_FRONTLEFT   ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
	ChannelMapEntry[XAUDIOSPEAKER_FRONTRIGHT  ].InputChannel = XAUDIOSPEAKER_FRONTRIGHT;
	ChannelMapEntry[XAUDIOSPEAKER_FRONTCENTER ].InputChannel = XAUDIOSPEAKER_FRONTCENTER;
	ChannelMapEntry[XAUDIOSPEAKER_LOWFREQUENCY].InputChannel = XAUDIOSPEAKER_LOWFREQUENCY;
	ChannelMapEntry[XAUDIOSPEAKER_BACKLEFT    ].InputChannel = XAUDIOSPEAKER_BACKLEFT;
	ChannelMapEntry[XAUDIOSPEAKER_BACKRIGHT   ].InputChannel = XAUDIOSPEAKER_BACKRIGHT;

	SubmixVoiceInit.Format.ChannelCount	= XAUDIOSPEAKER_COUNT;
	SubmixVoiceInit.pEffectChain		= &kstEffects_VoiceChain_Delay;

	XAudioCreateSubmixVoice(&SubmixVoiceInit, &m_pSubmixVoice_Delay);
*/
	// - Reverb

	ChannelMapEntry[XAUDIOSPEAKER_FRONTLEFT   ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
	ChannelMapEntry[XAUDIOSPEAKER_FRONTRIGHT  ].InputChannel = XAUDIOSPEAKER_FRONTRIGHT;
	ChannelMapEntry[XAUDIOSPEAKER_FRONTCENTER ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
	ChannelMapEntry[XAUDIOSPEAKER_LOWFREQUENCY].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
	ChannelMapEntry[XAUDIOSPEAKER_BACKLEFT    ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
	ChannelMapEntry[XAUDIOSPEAKER_BACKRIGHT   ].InputChannel = XAUDIOSPEAKER_FRONTRIGHT;

	SubmixVoiceInit.Format.ChannelCount	= 2;
	SubmixVoiceInit.pEffectChain		= &kstEffects_VoiceChain_Reverb;

	XAudioCreateSubmixVoice(&SubmixVoiceInit, &m_pSubmixVoice_Reverb);

	// Create Empty Source Voice (To Keep Reverb Effect Running At All Times)
	// Note: This is a temporary patch to counter the Reverb "Trail Frames too low" Microsoft Bug

	// - Configure Output

	XAUDIOVOICEOUTPUT		stVoiceOutput;
	XAUDIOVOICEOUTPUTENTRY	astVoiceOutputEntry[1];
	XAUDIOCHANNELMAP		stChannelMap;
	XAUDIOCHANNELMAPENTRY	astChannelMapEntry[1];

	astChannelMapEntry[XAUDIOSPEAKER_FRONTLEFT].InputChannel	= XAUDIOSPEAKER_FRONTLEFT;
	astChannelMapEntry[XAUDIOSPEAKER_FRONTLEFT].OutputChannel	= XAUDIOSPEAKER_FRONTLEFT;
	astChannelMapEntry[XAUDIOSPEAKER_FRONTLEFT].Volume			= XAUDIOVOLUME_MAX;

	stChannelMap.EntryCount		= 1;
	stChannelMap.paEntries		= astChannelMapEntry;

	stVoiceOutput.EntryCount	= 1;
	stVoiceOutput.paEntries		= astVoiceOutputEntry;

	astVoiceOutputEntry[0].pDestVoice	= m_pSubmixVoice_Reverb;
	astVoiceOutputEntry[0].pChannelMap	= &stChannelMap;

	// - Initialize Voice

	XAUDIOSOURCEVOICEINIT stSourceVoiceInit;

	L_memset(&stSourceVoiceInit, 0, sizeof(stSourceVoiceInit));

	stSourceVoiceInit.MaxPacketCount			= 1;
	stSourceVoiceInit.Format.ChannelCount		= 1;
	stSourceVoiceInit.Format.SampleRate			= 64;
	stSourceVoiceInit.Format.SampleType			= XAUDIOSAMPLETYPE_16BITPCM;
	stSourceVoiceInit.pVoiceOutput				= &stVoiceOutput;

	XAudioCreateSourceVoice(&stSourceVoiceInit, &m_pSourceVoice_EmptyLoop);

	// - Submit Looping Empty Packet

	L_memset(m_aEmptyBuffer, 0, sizeof(m_aEmptyBuffer));

	XAUDIOPACKET stPacket;

	stPacket.pBuffer	= m_aEmptyBuffer;
	stPacket.BufferSize	= sizeof(m_aEmptyBuffer);
	stPacket.LoopCount	= XAUDIOLOOPCOUNT_INFINITE;
	stPacket.LoopStart	= 0;
	stPacket.LoopLength	= stPacket.BufferSize;

	m_pSourceVoice_EmptyLoop->SubmitPacket(&stPacket, XAUDIOSUBMITPACKET_DISCONTINUITY);
	m_pSourceVoice_EmptyLoop->Start(0);
#endif
}

// ***********************************************************************************************************************

void CFXCore::Uninit()
{
#if _XENON_SOUND_FX
	SetEnabled(FALSE);

	// Destroy Voices

	//if (m_pSubmixVoice_Delay != NULL)
	//{
		//m_pSubmixVoice_Delay->Release();
		//m_pSubmixVoice_Delay = NULL;
	//}

	if (m_pSubmixVoice_Reverb != NULL)
	{
		m_pSubmixVoice_Reverb->Release();
		m_pSubmixVoice_Reverb = NULL;
	}

	if (m_pSourceVoice_EmptyLoop != NULL)
	{
		m_pSourceVoice_EmptyLoop->FlushPackets();
		m_pSourceVoice_EmptyLoop->Release();
		m_pSourceVoice_EmptyLoop = NULL;
	}
#endif
}

// ***********************************************************************************************************************
/*
IXAudioSubmixVoice* CFXCore::GetVoice_Delay()
{
#if !_XENON_SOUND_FX
	return NULL;
#else
	return m_pSubmixVoice_Delay;
#endif
}
*/
// ***********************************************************************************************************************

IXAudioSubmixVoice* CFXCore::GetVoice_Reverb()
{
#if !_XENON_SOUND_FX
	return NULL;
#else
	return m_pSubmixVoice_Reverb;
#endif
}

// ***********************************************************************************************************************

void CFXCore::UpdateEffect()
{
#if _XENON_SOUND_FX
	// Error Checking

	if ((m_stEngineSettings.i_Mode < SND_Cte_FxMode_Off) || (m_stEngineSettings.i_Mode >= SND_Cte_FxMode_Nb))
	{
		return;
	}

	// Setup Voice Output Structures

	XAUDIOVOICEOUTPUTVOLUME			VoiceVolume;
	XAUDIOVOICEOUTPUTVOLUMEENTRY	VoiceEntries[1];
	XAUDIOCHANNELVOLUME				ChannelVolume;
	XAUDIOCHANNELVOLUMEENTRY		ChannelVolumeEntries[XAUDIOSPEAKER_COUNT];

	ChannelVolume.EntryCount			= XAUDIOSPEAKER_COUNT;
	ChannelVolume.paEntries				= ChannelVolumeEntries;

	VoiceEntries[0].OutputVoiceIndex	= 0;
	VoiceEntries[0].pVolume				= &ChannelVolume;

	VoiceVolume.EntryCount				= sizeof(VoiceEntries) / sizeof(XAUDIOVOICEOUTPUTVOLUMEENTRY);
	VoiceVolume.paEntries				= VoiceEntries;

	for (int Loop = 0; Loop < sizeof(ChannelVolumeEntries) / sizeof(XAUDIOCHANNELVOLUMEENTRY); Loop++)
	{
		ChannelVolumeEntries[Loop].EntryIndex = (XAUDIOCHANNEL)Loop;
	}

	// Compute Speaker Factors (Wet Panning)

	float Volume_LeftFactor		= m_stEngineSettings.f_WetVol * SND_f_GetVolFromAtt(-m_stEngineSettings.i_WetPan);
	float Volume_RightFactor	= m_stEngineSettings.f_WetVol * SND_f_GetVolFromAtt(m_stEngineSettings.i_WetPan);

	// Clamp Values (In Case Some Bizarre Overwrite Happens Elsewhere)

	if (Volume_LeftFactor < 0.0f)
	{
		Volume_LeftFactor = 0.0f;
	}

	if (Volume_LeftFactor > 1.0f)
	{
		Volume_LeftFactor = 1.0f;
	}

	if (Volume_RightFactor < 0.0f)
	{
		Volume_RightFactor = 0.0f;
	}

	if (Volume_RightFactor > 1.0f)
	{
		Volume_RightFactor = 1.0f;
	}

	if (m_stEngineSettings.f_WetVol < 0.0f)
	{
		m_stEngineSettings.f_WetVol = 0.0f;
	}

	if (m_stEngineSettings.f_WetVol > 1.0f)
	{
		m_stEngineSettings.f_WetVol = 1.0f;
	}

	// Configure Volume Entry Table

	ChannelVolumeEntries[XAUDIOSPEAKER_FRONTLEFT   ].Volume = Volume_LeftFactor;
	ChannelVolumeEntries[XAUDIOSPEAKER_FRONTRIGHT  ].Volume = Volume_RightFactor;
	ChannelVolumeEntries[XAUDIOSPEAKER_FRONTCENTER ].Volume = m_stEngineSettings.f_WetVol;
	ChannelVolumeEntries[XAUDIOSPEAKER_LOWFREQUENCY].Volume = m_stEngineSettings.f_WetVol;
	ChannelVolumeEntries[XAUDIOSPEAKER_BACKLEFT    ].Volume = Volume_LeftFactor;
	ChannelVolumeEntries[XAUDIOSPEAKER_BACKRIGHT   ].Volume = Volume_RightFactor;

	// Update Volumes

	//m_pSubmixVoice_Delay->SetVoiceOutputVolume(&VoiceVolume);
	m_pSubmixVoice_Reverb->SetVoiceOutputVolume(&VoiceVolume);

	// Update Effect Parameters

	XAUDIOFXPARAM stFXParam = {0};
/*
	// - Delay Feedback Gain

	if ((kstEffect_Delay_ModePresets[m_stEngineSettings.i_Mode].fGain == kfDelay_Preset_Free) || m_bForcedFeedback)
	{
		// Use Value Specified By AI / Forced

		stFXParam.Value = (float)m_stEngineSettings.i_Feedback / 100.0f;
	}
	else
	{
		// Use Preset Value

		stFXParam.Value = kstEffect_Delay_ModePresets[m_stEngineSettings.i_Mode].fGain;
	}

	m_pSubmixVoice_Delay->SetEffectParam(0, kiDelay_ParamID_Gain, XAUDIOFXPARAMTYPE_NUMERIC, &stFXParam);

	// - Delay Time

	if ((kstEffect_Delay_ModePresets[m_stEngineSettings.i_Mode].fTime == kfDelay_Preset_Free) || m_bForcedDelay)
	{
		// Use Value Specified By AI / Forced

		stFXParam.Value = (float)m_stEngineSettings.i_Delay;
	}
	else
	{
		// Use Preset Value

		stFXParam.Value = kstEffect_Delay_ModePresets[m_stEngineSettings.i_Mode].fTime;
	}

	m_pSubmixVoice_Delay->SetEffectParam(0, kiDelay_ParamID_Time, XAUDIOFXPARAMTYPE_NUMERIC, &stFXParam);

	// - Delay Algorithm

	if (m_bForcedDelayAlgorithm)
	{
		// Use Forced Specified

		stFXParam.Value = (float)m_eDelayAlgorithm;
	}
	else
	{
		// Use Preset Value

		m_eDelayAlgorithm	= kstEffect_Delay_ModePresets[m_stEngineSettings.i_Mode].eAlgorithm;
		stFXParam.Value		= (float)m_eDelayAlgorithm;
	}

	m_pSubmixVoice_Delay->SetEffectParam(0, kiDelay_ParamID_Algorithm, XAUDIOFXPARAMTYPE_NUMERIC, &stFXParam);
*/
	// - Reverb Preset

	stFXParam.Data.BufferSize	= sizeof(XAUDIOREVERBI3DL2SETTINGS);
	stFXParam.Data.pBuffer		= (LPVOID)&kstEffects_Reverb_ModePresets[m_stEngineSettings.i_Mode];

	m_pSubmixVoice_Reverb->SetEffectParam(0, XAUDIOFXPARAMID_REVERB_I3DL2SETTINGS, XAUDIOFXPARAMTYPE_DATA, &stFXParam);
#endif
}

// ***********************************************************************************************************************

BOOL CFXCore::GetEnabled()
{
#if _XENON_SOUND_FX
	return m_bIsEnabled;
#else
	return FALSE;
#endif
}

// ***********************************************************************************************************************

int CFXCore::GetMode()
{
#if _XENON_SOUND_FX
	return m_stEngineSettings.i_Mode;
#else
	return SND_Cte_FxMode_Off;
#endif
}

// ***********************************************************************************************************************
/*
int CFXCore::GetDelay()
{
#if _XENON_SOUND_FX
	return m_stEngineSettings.i_Delay;
#else
	return 0;
#endif
}
*/
// ***********************************************************************************************************************
/*
int CFXCore::GetFeedback()
{
#if _XENON_SOUND_FX
	return m_stEngineSettings.i_Feedback;
#else
	return 0;
#endif
}
*/
// ***********************************************************************************************************************

float CFXCore::GetWetVolume()
{
#if _XENON_SOUND_FX
	return m_stEngineSettings.f_WetVol;
#else
	return 0.0f;
#endif
}

// ***********************************************************************************************************************

int CFXCore::GetWetPan()
{
#if _XENON_SOUND_FX
	return m_stEngineSettings.i_WetPan;
#else
	return 0;
#endif
}

// ***********************************************************************************************************************
/*
eDelayAlgorithm CFXCore::GetDelayAlgorithm()
{
#if _XENON_SOUND_FX
	return m_eDelayAlgorithm;
#else
	return eDelayAlgorithm_None;
#endif
}
*/
// ***********************************************************************************************************************

void CFXCore::SetEnabled(BOOL Param_bIsEnabled)
{
#if _XENON_SOUND_FX
	m_bIsEnabled = Param_bIsEnabled;
#endif
}

// ***********************************************************************************************************************

void CFXCore::SetMode(int Param_iMode)
{
#if _XENON_SOUND_FX
	// Forced Mode For Core A

	if ((m_iCoreID == SND_Cte_FxCoreA) && (GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedMode > SND_Cte_FxMode_Off))
	{
		Param_iMode = GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedMode;
	}

	// Forced Mode For Core B

	if ((m_iCoreID == SND_Cte_FxCoreB) && (GetAudioEngine()->GetRuntimeSettings()->iFXCoreB_ForcedMode > SND_Cte_FxMode_Off))
	{
		Param_iMode = GetAudioEngine()->GetRuntimeSettings()->iFXCoreB_ForcedMode;
	}

	// Update Core Settings (If Applicable)

	if (m_stEngineSettings.i_Mode != Param_iMode)
	{
		m_stEngineSettings.i_Mode = Param_iMode;
		UpdateEffect();
	}
#endif
}

// ***********************************************************************************************************************
/*
void CFXCore::SetDelay(int Param_iDelay)
{
#if _XENON_SOUND_FX
	m_bForcedDelay = FALSE;

	// Forced Delay For Core A

	if ((m_iCoreID == SND_Cte_FxCoreA) && (GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedDelay > 0))
	{
		Param_iDelay	= GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedDelay;
		m_bForcedDelay	= TRUE;
	}

	// Forced Delay For Core B

	if ((m_iCoreID == SND_Cte_FxCoreB) && (GetAudioEngine()->GetRuntimeSettings()->iFXCoreB_ForcedDelay > 0))
	{
		Param_iDelay	= GetAudioEngine()->GetRuntimeSettings()->iFXCoreB_ForcedDelay;
		m_bForcedDelay	= TRUE;
	}

	// Update Core Settings (If Applicable)

	if (m_stEngineSettings.i_Delay != Param_iDelay)
	{
		m_stEngineSettings.i_Delay = Param_iDelay;
		UpdateEffect();
	}
#endif
}
*/
// ***********************************************************************************************************************
/*
void CFXCore::SetFeedback(int Param_iFeedback)
{
#if _XENON_SOUND_FX
	m_bForcedFeedback = FALSE;

	// Forced Feedback For Core A

	if ((m_iCoreID == SND_Cte_FxCoreA) && (GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedFeedback > 0))
	{
		Param_iFeedback		= GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedFeedback;
		m_bForcedFeedback	= TRUE;
	}

	// Forced Feedback For Core B

	if ((m_iCoreID == SND_Cte_FxCoreB) && (GetAudioEngine()->GetRuntimeSettings()->iFXCoreB_ForcedFeedback > 0))
	{
		Param_iFeedback		= GetAudioEngine()->GetRuntimeSettings()->iFXCoreB_ForcedFeedback;
		m_bForcedFeedback	= TRUE;
	}

	// Update Core Settings (If Applicable)

	if (m_stEngineSettings.i_Feedback != Param_iFeedback)
	{
		m_stEngineSettings.i_Feedback = Param_iFeedback;
		UpdateEffect();
	}
#endif
}
*/
// ***********************************************************************************************************************

void CFXCore::SetWetVolume(float Param_fWetVolume)
{
#if _XENON_SOUND_FX
	// Forced WetVolume For Core A

	if ((m_iCoreID == SND_Cte_FxCoreA) && (GetAudioEngine()->GetRuntimeSettings()->fFXCoreA_ForcedWetVolume > 0.0f))
	{
		Param_fWetVolume = GetAudioEngine()->GetRuntimeSettings()->fFXCoreA_ForcedWetVolume;
	}

	// Forced WetVolume For Core B

	if ((m_iCoreID == SND_Cte_FxCoreB) && (GetAudioEngine()->GetRuntimeSettings()->fFXCoreB_ForcedWetVolume > 0.0f))
	{
		Param_fWetVolume = GetAudioEngine()->GetRuntimeSettings()->fFXCoreB_ForcedWetVolume;
	}

	// Update Core Settings (If Applicable)

	if (m_stEngineSettings.f_WetVol != Param_fWetVolume)
	{
		m_stEngineSettings.f_WetVol = Param_fWetVolume;
		UpdateEffect();
	}
#endif
}

// ***********************************************************************************************************************

void CFXCore::SetWetPan(int Param_iWetPan)
{
#if _XENON_SOUND_FX
	// Forced WetPan For Core A

	if ((m_iCoreID == SND_Cte_FxCoreA) && (GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedWetPan != 0))
	{
		Param_iWetPan = GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedWetPan;
	}

	// Forced WetVolume For Core B

	if ((m_iCoreID == SND_Cte_FxCoreB) && (GetAudioEngine()->GetRuntimeSettings()->iFXCoreB_ForcedWetPan != 0))
	{
		Param_iWetPan = GetAudioEngine()->GetRuntimeSettings()->iFXCoreB_ForcedWetPan;
	}

	// Update Core Settings (If Applicable)

	if (m_stEngineSettings.i_WetPan != Param_iWetPan)
	{
		m_stEngineSettings.i_WetPan = Param_iWetPan;
		UpdateEffect();
	}
#endif
}

// ***********************************************************************************************************************
/*
void CFXCore::SetDelayAlgorithm(eDelayAlgorithm Param_eDelayAlgorithm)
{
#if _XENON_SOUND_FX
	m_bForcedDelayAlgorithm = FALSE;

	// Forced Delay Algorithm For Core A

	if ((m_iCoreID == SND_Cte_FxCoreA) && (GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedDelayAlgorithm != eDelayAlgorithm_None))
	{
		Param_eDelayAlgorithm	= (eDelayAlgorithm)GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedDelayAlgorithm;
		m_bForcedDelayAlgorithm	= TRUE;
	}

	// Forced Delay Algorithm For Core B

	if ((m_iCoreID == SND_Cte_FxCoreB) && (GetAudioEngine()->GetRuntimeSettings()->iFXCoreB_ForcedDelayAlgorithm != eDelayAlgorithm_None))
	{
		Param_eDelayAlgorithm	= (eDelayAlgorithm)GetAudioEngine()->GetRuntimeSettings()->iFXCoreB_ForcedDelayAlgorithm;
		m_bForcedDelayAlgorithm	= TRUE;
	}

	// Update Core Settings (If Applicable)

	if (m_eDelayAlgorithm != Param_eDelayAlgorithm)
	{
		m_eDelayAlgorithm = Param_eDelayAlgorithm;
		UpdateEffect();
	}
#endif
}
*/
// ***********************************************************************************************************************
//    Class Methods : CEffect_Delay
// ***********************************************************************************************************************
/*
CEffect_Delay::CEffect_Delay(void* Param_pContext)
{
	m_pContext					= Param_pContext;
	m_iReferenceCount			= 1;
	m_iCircularOffset			= 0;
	m_iDelaySample				= 0;

	m_stParameters.eAlgorithm	= eDelayAlgorithm_None;
	m_stParameters.fTime		= kfDelay_Time_Min;
	m_stParameters.fGain		= kfDelay_Gain_Min;
}

// ***********************************************************************************************************************

HRESULT CEffect_Delay::Initialize(const XAUDIOFXINIT* Param_pstInit, IXAudioBatchAllocator* Param_pAllocator)
{
	for (int iLoop = 0; iLoop < XAUDIOSPEAKER_COUNT; iLoop++)
	{
		m_apfCircularBuffer[iLoop] = XAUDIO_BATCHALLOC_ALLOC(float, kiDelay_CircularBuffer_Size, Param_pAllocator);
		ZeroMemory(m_apfCircularBuffer[iLoop], kiDelay_CircularBuffer_Size * sizeof(float));
	}

	return S_OK;
}

// ***********************************************************************************************************************

HRESULT CEffect_Delay::Process(IXAudioFrameBuffer* Param_pInputBuffer, IXAudioFrameBuffer* Param_pOutputBuffer)
{
	// Fetch Frame Buffer

	XAUDIOFRAMEBUFDATA stFrameBufferData;

	Param_pInputBuffer->GetProcessingData(&stFrameBufferData);

	XAUDIOSAMPLE* pfVoiceSampleBuffer = stFrameBufferData.pSampleBuffer;

	// Update Voice Samples & Circular Buffers

	for (int iLoop_Channel = 0; iLoop_Channel < stFrameBufferData.Format.ChannelCount; iLoop_Channel++)
	{
		int		iOffset					= m_iCircularOffset;
		float*	pfCircularSampleBuffer	= m_apfCircularBuffer[iLoop_Channel];

		switch (m_stParameters.eAlgorithm)
		{
			case eDelayAlgorithm_None:
			{
				break;
			}

			case eDelayAlgorithm_AllPass:
			{
				for (int iLoop_Sample = 0; iLoop_Sample < XAUDIOFRAMESIZE_NATIVE; iLoop_Sample++)
				{
					// Caculate Output Sample (Delayed + Inverse Gained FeedForward Input)

					float Sample_Out = kfDelay_Feedback_Damping * pfCircularSampleBuffer[(iOffset - m_iDelaySample) & kiDelay_CircularBuffer_Mask] - (*pfVoiceSampleBuffer * (1.0f - m_stParameters.fGain));

					// Write New Delayed Sample (Input + Gained FeedBack Output)

					pfCircularSampleBuffer[iOffset & kiDelay_CircularBuffer_Mask] = *pfVoiceSampleBuffer + (Sample_Out * (1.0f - m_stParameters.fGain) * kfDelay_Feedback_Damping);

					// Write Output Sample

					*pfVoiceSampleBuffer = Sample_Out;

					// Update Local Buffer Offsets

					iOffset++;
					pfVoiceSampleBuffer++;
				}
				break;
			}

			case eDelayAlgorithm_Comb:
			{
				for (int iLoop_Sample = 0; iLoop_Sample < XAUDIOFRAMESIZE_NATIVE; iLoop_Sample++)
				{
					// Apply To Voice Previous Sample From Circular Buffer

					*pfVoiceSampleBuffer += (pfCircularSampleBuffer[(iOffset - m_iDelaySample) & kiDelay_CircularBuffer_Mask] * m_stParameters.fGain);

					// Update Circular Buffer Sample

					pfCircularSampleBuffer[iOffset & kiDelay_CircularBuffer_Mask] = *pfVoiceSampleBuffer;

					// Update Local Buffer Offsets

					iOffset++;
					pfVoiceSampleBuffer++;
				}
				break;
			}
		}
	}

	// Update Global Circular Buffer Offset

	m_iCircularOffset += XAUDIOFRAMESIZE_NATIVE;
	m_iCircularOffset &= kiDelay_CircularBuffer_Mask;

	return S_OK;
}

// ***********************************************************************************************************************

HRESULT CEffect_Delay::GetInfo(XAUDIOFXINFO* Param_pstInfo)
{
	Param_pstInfo->DataFlow		= XAUDIODATAFLOW_INPLACE;
	Param_pstInfo->TrailFrames	= 255;
	Param_pstInfo->PrimeFrames	= 0;

	return S_OK;
}

// ***********************************************************************************************************************

HRESULT CEffect_Delay::GetParam(XAUDIOFXPARAMID Param_iID, XAUDIOFXPARAMTYPE Param_iType, XAUDIOFXPARAM* Param_pstValue)
{
	XAUDIOFXPARAM stValue = {0};

	switch (Param_iID)
	{
		case kiDelay_ParamID_Time:
		{
			stValue.Value = m_stParameters.fTime;
			break;
		}

		case kiDelay_ParamID_Gain:
		{
			stValue.Value = m_stParameters.fGain;
			break;
		}

		case kiDelay_ParamID_Algorithm:
		{
			stValue.Value = (float)m_stParameters.eAlgorithm;
			break;
		}

		default:
		{
			return E_FAIL;
		}
	}

	*Param_pstValue = stValue;

	return S_OK;
}

// ***********************************************************************************************************************

HRESULT CEffect_Delay::SetParam(XAUDIOFXPARAMID Param_iID, XAUDIOFXPARAMTYPE Param_iType, const XAUDIOFXPARAM* Param_pstValue)
{
	switch (Param_iID)
	{
		case kiDelay_ParamID_Time:
		{
			float fValue = Param_pstValue->Value;

			if (fValue > kfDelay_Time_Max)
			{
				fValue = kfDelay_Time_Max;
			}

			if (fValue < kfDelay_Time_Min)
			{
				fValue = kfDelay_Time_Min;
			}

			m_stParameters.fTime	= fValue * kfDelay_Time_Scale;
			m_iDelaySample			= (int)(m_stParameters.fTime * kfDelay_SamplesPerMS);
			break;
		}

		case kiDelay_ParamID_Gain:
		{
			float fValue = Param_pstValue->Value;

			if (fValue > kfDelay_Gain_Max)
			{
				fValue = kfDelay_Gain_Max;
			}

			if (fValue < kfDelay_Gain_Min)
			{
				fValue = kfDelay_Gain_Min;
			}

			m_stParameters.fGain = fValue;
			break;
		}

		case kiDelay_ParamID_Algorithm:
		{
			if ((Param_pstValue->Value >= eDelayAlgorithm_Count) || (Param_pstValue->Value < eDelayAlgorithm_None))
			{
				return E_FAIL;
			}

			m_stParameters.eAlgorithm = (eDelayAlgorithm)((int)Param_pstValue->Value);
			break;
		}

		default:
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

// ***********************************************************************************************************************

HRESULT CEffect_Delay::GetContext(void** Param_ppContext)
{
	*Param_ppContext = m_pContext;

	return S_OK;
}

// ***********************************************************************************************************************

unsigned long CEffect_Delay::AddRef()
{
	m_iReferenceCount++;

	return m_iReferenceCount;
}

// ***********************************************************************************************************************

unsigned long CEffect_Delay::Release()
{
	// Decrement Reference Counter

	m_iReferenceCount--;

	// Return Reference Counter (If > 0)

	if (m_iReferenceCount > 0)
	{
		return m_iReferenceCount;
	}

	// Destroy Effect Class

	for (int iLoop = 0; iLoop < XAUDIOSPEAKER_COUNT; iLoop++)
	{
		XAUDIO_BATCHALLOC_FREE(m_apfCircularBuffer[iLoop]);
	}

	delete this;

	return 0;
}

// ***********************************************************************************************************************

HRESULT Effect_Delay_QuerySize(const XAUDIOFXINIT* Param_pstInit, unsigned long* Param_pulEffectSize)
{
	// Error Checking

	if (Param_pulEffectSize == NULL)
	{
		return E_FAIL;
	}

	// Output Effect Size

	*Param_pulEffectSize = sizeof(CEffect_Delay) + (unsigned long)(sizeof(float) * XAUDIOSPEAKER_COUNT * kiDelay_CircularBuffer_Size);

	return S_OK;
}

// ***********************************************************************************************************************

HRESULT Effect_Delay_Create(const XAUDIOFXINIT* Param_pstInit, IXAudioBatchAllocator* Param_pAllocator, IXAudioEffect** Param_ppEffect)
{
	// Error Checking

	if ((Param_pstInit == NULL) || (Param_pAllocator == NULL) || (Param_ppEffect == NULL))
	{
		return E_FAIL;
	}

	// Allocate Effect Class

	CEffect_Delay* pUserEffect = XAUDIO_BATCHALLOC_NEW(CEffect_Delay(Param_pstInit->pContext), Param_pAllocator);

	if (pUserEffect == NULL)
	{
		return E_FAIL;
	}

	// Initialize Effect Class

	if (pUserEffect->Initialize(Param_pstInit, Param_pAllocator) != S_OK)
	{
		return E_FAIL;
	}

	// Output Newly Allocated Class

	*Param_ppEffect = pUserEffect;

	return S_OK;
}

// ***********************************************************************************************************************
*/
#endif // _XENON

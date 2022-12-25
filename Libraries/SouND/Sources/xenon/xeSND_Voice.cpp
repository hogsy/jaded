#include "Precomp.h"

#ifdef _XENON

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "SouND/Sources/SNDvolume.h"

#include "xeSND_Engine.h"
#include "xeSND_ADPCM.h"

// ***********************************************************************************************************************
//    Class Methods : CVoice
// ***********************************************************************************************************************

CVoice::CVoice(SND_tdst_WaveData* Param_pWaveData, unsigned int Param_uiFilePositionInBF, int Param_iStreamID, CBuffer* Param_pBuffer_Playback) : CListItem()
{
	// Safe Init

	m_pSourceVoice		= NULL;
	m_pBuffer_Playback	= NULL;
	m_pBuffer_Loading	= NULL;

	// Error Checking

	if (Param_pWaveData == NULL)
	{
		return;
	}

	// Initialize Early Fields

	m_bIsMusic = FALSE;

	// Prepare Source Voice Init Structure

	L_memset(&m_stSourceVoiceInit, 0, sizeof(m_stSourceVoiceInit));

	m_stSourceVoiceInit.MaxOutputVoiceCount			= 3;
	m_stSourceVoiceInit.Category					= IsMusic() ? XAUDIOVOICECATEGORY_BGMUSIC : XAUDIOVOICECATEGORY_NONE;
	m_stSourceVoiceInit.Format.ChannelCount			= (XAUDIOCHANNEL)Param_pWaveData->st_WaveFmtx.nChannels;
	m_stSourceVoiceInit.Format.SampleRate			= (XAUDIOSAMPLERATE)Param_pWaveData->st_WaveFmtx.nSamplesPerSec;
	m_stSourceVoiceInit.pContext					= this;
	m_stSourceVoiceInit.MaxPacketCount				= kiStreamPacketCount;
	m_stSourceVoiceInit.pfnPacketCompletionCallback	= fnXAudio_PacketCallback;
	m_stSourceVoiceInit.pfnPacketLoopCallback		= NULL;

	// - Sample Format

	switch (Param_pWaveData->st_WaveFmtx.wBitsPerSample)
	{
		case 16:
		{
			m_stSourceVoiceInit.Format.SampleType = XAUDIOSAMPLETYPE_16BITPCM;
			break;
		}
		case 8:
		{
			m_stSourceVoiceInit.Format.SampleType = XAUDIOSAMPLETYPE_8BITPCM;
			break;
		}
		case 4:
		{
			if (Param_pWaveData->st_WaveFmtx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
			{
				m_stSourceVoiceInit.Format.SampleType = XAUDIOSAMPLETYPE_16BITPCM;
				break;
			}
		}
		default:
		{
#if defined(AUDIOCONSOLE_ENABLE)
			AudioConsole_WaitForMessageExclusion();
			AudioConsole_AddMessageParameter(Param_pWaveData->st_WaveFmtx.wBitsPerSample, L"Bits");
			AudioConsole_AddMessage(L"SoundBuffer Create - Invalid Format", kszModuleName_XenonSound, kstMessageType_Error);
#endif
			return;
		}
	}

	// Create XAudio Source Voice

	XAudioCreateSourceVoice(&m_stSourceVoiceInit, &m_pSourceVoice);

	// Initialize Fields

	m_eState							= eState_None;
	m_stWaveFormat						= Param_pWaveData->st_WaveFmtx;
	m_iFrequency						= -1;
	m_iPan								= -1;
	m_iSurroundPan						= -1;
	m_iVolume							= -1;
	m_fVolumeAddGain					= 0.0f;
	m_iStreamID							= Param_iStreamID;
	m_iKey								= GetAudioEngine()->GetVoiceInit_Key();
	m_eSoundBFIndex						= GetAudioEngine()->GetVoiceInit_SoundBF();

	m_iFlags							= 0;
	m_iLoopCount						= 0;
	m_iSize_File_SoundData				= 0;

	m_iPosition_Buffer_ToLoad			= 0;
	m_iPosition_Buffer_ToDecode			= 0;
	m_iPosition_Buffer_ToPlay			= 0;

	m_fWetMixFactor						= 0.0f;

	// Clear Stream Voice Init Settings

	L_memset(&m_stInitSettings, 0, sizeof(m_stInitSettings));

	// Initial Channel Setup

	Setup_Channels();

	// Convert Size From Native Format

	int iSize_PCM = SoundPosition_NativeToPCM(Param_pWaveData->i_Size, Param_pWaveData->st_WaveFmtx.wFormatTag);

	// Create Packet Tracker

	m_pPacketTracker = new CPacketTracker();

	// Assign / Allocate Buffers (If Applicable)

	m_pBuffer_Playback	= new CBuffer();
	m_pBuffer_Loading	= new CBuffer();

	if (Param_pBuffer_Playback != NULL)
	{
		m_pBuffer_Playback->Acquire(Param_pBuffer_Playback);
	}
	else
	{
		if (IsAStream())
		{
			m_pBuffer_Playback->SetTracker(eMemoryTracker_Buffer_Stream);
			m_pBuffer_Playback->RequestStorage(SND_Cte_MaxBufferSize);
		}
		else
		{
			m_pBuffer_Playback->SetTracker(eMemoryTracker_Buffer_Static);
			m_pBuffer_Playback->RequestStorage(iSize_PCM);
		}
	}

	// Schedule Data Load (If Applicable)

	if (!IsAStream() && (Param_pBuffer_Playback == NULL))
	{
		Setup_Static(Param_pWaveData, Param_uiFilePositionInBF);
	}
}

// ***********************************************************************************************************************

CVoice::~CVoice()
{
	// Destroy Source Voice

	if (m_pSourceVoice != NULL)
	{
		m_pSourceVoice->FlushPackets();
		m_pSourceVoice->Release();
	}

	// Destroy Buffers

	if (m_pBuffer_Playback != NULL)
	{
		delete m_pBuffer_Playback;
	}

	if (m_pBuffer_Loading != NULL)
	{
		delete m_pBuffer_Loading;
	}

	// Destroy Packet Tracker

	if (m_pPacketTracker != NULL)
	{
		delete m_pPacketTracker;
	}
}

// ***********************************************************************************************************************

void CVoice::SetListIndex(int Param_iListIndex)
{
	m_iListIndex = Param_iListIndex;
}

// ***********************************************************************************************************************

int CVoice::GetListIndex()
{
	return m_iListIndex;
}

// ***********************************************************************************************************************

void CVoice::SetKey(int Param_iKey, eSoundBFIndex Param_eSoundBFIndex)
{
	m_iKey			= Param_iKey;
	m_eSoundBFIndex	= Param_eSoundBFIndex;
}

// ***********************************************************************************************************************

int CVoice::GetKey()
{
	return m_iKey;
}

// ***********************************************************************************************************************

int CVoice::GetStreamID()
{
	return m_iStreamID;
}

// ***********************************************************************************************************************

eState CVoice::GetState()
{
	return m_eState;
}

// ***********************************************************************************************************************

int CVoice::GetFXCoreID()
{
	if (m_iFlags & SND_Cul_SBC_PlayFxA)
	{
		return SND_Cte_FxCoreA;
	}

	if (m_iFlags & SND_Cul_SBC_PlayFxB)
	{
		return SND_Cte_FxCoreA;
	}

	return -1;
}

// ***********************************************************************************************************************

CBuffer* CVoice::GetPlaybackBuffer()
{
	return m_pBuffer_Playback;
}

// ***********************************************************************************************************************

char* CVoice::RequestLoadingBuffer(int Param_iSize)
{
	// Error Checking

	if (m_pBuffer_Loading == NULL)
	{
		return NULL;
	}

	// Fetch Appropriate Buffer

	switch (GetEncoding())
	{
		case eEncoding_PCM:
		{
			// Use Playback Buffer For Loading

			if (m_pBuffer_Loading->IsEmpty())
			{
				m_pBuffer_Loading->Acquire(m_pBuffer_Playback);
			}

			// Request Block

			return m_pBuffer_Loading->RequestStorage(Param_iSize, m_iPosition_Buffer_ToLoad);
		}

		case eEncoding_ADPCM:
		{
			// Reset Loading Offset

			m_iPosition_Buffer_ToLoad = 0;

			// Request Block

			return m_pBuffer_Loading->RequestStorage(SoundPosition_PCMToNative(Param_iSize), eMemoryTracker_General);
		}

		case eEncoding_XMA:
		{
			return NULL;
		}

		default:
		case eEncoding_Unknown:
		{
			return NULL;
		}
	}
}

// ***********************************************************************************************************************

unsigned int CVoice::GetFileStartPosition()
{
	return m_uiPosition_File_SoundStartInBF;
}

// ***********************************************************************************************************************

WAVEFORMATEX* CVoice::GetWaveFormat()
{
	return &m_stWaveFormat;
}

// ***********************************************************************************************************************

eEncoding CVoice::GetEncoding(int Param_iFormat)
{
	// Fetch Current Voice Encoding Format (If Applicable)

	if (Param_iFormat <= 0)
	{
		Param_iFormat = m_stWaveFormat.wFormatTag;
	}

	// Convert To Format Enum

	switch (Param_iFormat)
	{
		case WAVE_FORMAT_PCM		: return eEncoding_PCM;
		case WAVE_FORMAT_XBOX_ADPCM	: return eEncoding_ADPCM;
		case WAVE_FORMAT_XMA		: return eEncoding_XMA;
		default:
		{
#if defined(AUDIOCONSOLE_ENABLE)
			AudioConsole_WaitForMessageExclusion();
			AudioConsole_AddMessageParameter(m_stWaveFormat.wFormatTag, L"Tag");
			AudioConsole_AddMessage(L"GetEncoding - Invalid Format", kszModuleName_XenonSound, kstMessageType_Error);
#endif
			return eEncoding_Unknown;
		}
	}
}

// ***********************************************************************************************************************

int CVoice::SoundPosition_ClampDownToFrame(int Param_iFilePosition, int Param_iFormat)
{
	// Error Checking

	if (Param_iFilePosition <= 0)
	{
		return Param_iFilePosition;
	}

	// Clamp Down And Convert By Encoding Format

	switch (GetEncoding(Param_iFormat))
	{
		case eEncoding_PCM:
		{
			return Param_iFilePosition;
		}

		case eEncoding_ADPCM:
		{
			Param_iFilePosition = Param_iFilePosition / (36 * m_stWaveFormat.nChannels);
			Param_iFilePosition = Param_iFilePosition * (36 * m_stWaveFormat.nChannels);

			return Param_iFilePosition;
		}

		case eEncoding_XMA:
		{
			return 0;
		}

		default:
		case eEncoding_Unknown:
		{
			return 0;
		}
	}
}

// ***********************************************************************************************************************

int CVoice::SoundPosition_NativeToPCM(int Param_iFilePosition, int Param_iFormat)
{
	// Error Checking

	if (Param_iFilePosition <= 0)
	{
		return Param_iFilePosition;
	}

	// Clamp Down And Convert By Encoding Format

	switch (GetEncoding(Param_iFormat))
	{
		case eEncoding_PCM:
		{
			return Param_iFilePosition;
		}

		case eEncoding_ADPCM:
		{
			return xeSND_ADPCM_GetDecompressedSize(Param_iFilePosition);
		}

		case eEncoding_XMA:
		{
			return 0;
		}

		default:
		case eEncoding_Unknown:
		{
			return 0;
		}
	}
}

// ***********************************************************************************************************************

int CVoice::SoundPosition_PCMToNative(int Param_iFilePosition, int Param_iFormat)
{
	// Error Checking

	if (Param_iFilePosition <= 0)
	{
		return Param_iFilePosition;
	}

	// Convert Encoding Format

	switch (GetEncoding(Param_iFormat))
	{
		case eEncoding_PCM:
		{
			return Param_iFilePosition;
		}

		case eEncoding_ADPCM:
		{
			return xeSND_ADPCM_GetCompressedSize(Param_iFilePosition);
		}

		case eEncoding_XMA:
		{
			return 0;
		}

		default:
		case eEncoding_Unknown:
		{
			return 0;
		}
	}
}

// ***********************************************************************************************************************

BOOL CVoice::IsMusic()
{
	return m_bIsMusic;
}

// ***********************************************************************************************************************

BOOL CVoice::IsLooping()
{
	return (m_iLoopCount != 0);
}

// ***********************************************************************************************************************

BOOL CVoice::IsSetupping()
{
	return ((m_eState == eState_Setup) || m_stInitSettings.bSetupRequested);
}

// ***********************************************************************************************************************

BOOL CVoice::IsPlaying()
{
	return (m_eState == eState_Playing);
}

// ***********************************************************************************************************************

BOOL CVoice::IsPaused()
{
	return (m_eState == eState_Paused);
}

// ***********************************************************************************************************************

BOOL CVoice::IsStopped()
{
	return (m_eState == eState_Stopped);
}

// ***********************************************************************************************************************

BOOL CVoice::IsAStream()
{
	return (m_iStreamID != kiStreamID_None);
}

// ***********************************************************************************************************************

BOOL CVoice::IsSinglePacket()
{
	return (!IsAStream() || (m_iSize_File_SoundData <= SND_Cte_MaxBufferSize));
}

// ***********************************************************************************************************************

void CVoice::Play(int Param_iFlags, int Param_iFxVolume_Left, int Param_iFxVolume_Right)
{
	// Ensure Voice Has Associated Sound

	if (m_iKey == 0)
	{
		return;
	}

	// Branch Playback For Static / Stream

	if (!IsAStream())
	{
		// Forced Stop Playback

		Stop();

		// Fetch New Playback Settings (If Applicable)

		if (Param_iFlags != -1)
		{
			m_iFlags		= Param_iFlags;
			m_iLoopCount	= (m_iFlags & SND_Cul_SBC_PlayLooping) ? -1 : 0;
		}

		if ((Param_iFxVolume_Left <= 0) && (Param_iFxVolume_Right <= 0))
		{
			m_fWetMixFactor	= 0.5f * (SND_f_GetVolFromAtt(Param_iFxVolume_Left) + SND_f_GetVolFromAtt(Param_iFxVolume_Right));
		}

		// Setup Channel Map

		Setup_Channels();

		// Start Playback

		m_pPacketTracker->Reset();

		Packet_Submit(m_iSize_LoadedData_Decoded);
	}
	else
	{
		// Check If a Packet is Loading

		if (m_pPacketTracker->GetLoadingStateWithTimeout())
		{
			// Re-schedule play after load

			GetAudioEngine()->RequestEvent(eEventID_StreamPlay, this);
			return;
		}

		// Forced Stop Playback

		m_eState = eState_Setup;

		Stop();

		// Fetch New Playback Settings

		m_iSize_File_SoundData						= m_stInitSettings.iFileSize;
		m_uiPosition_File_SoundStartInBF			= m_stInitSettings.uiFilePosition;
		m_iPosition_File_CurrentOffsetInSoundData	= m_stInitSettings.iPositionStart;
		m_iPosition_File_StopOffsetInSoundData		= m_stInitSettings.iPositionStop;
		m_iPosition_File_LoopBeginInSoundData		= m_stInitSettings.iLoopBegin;
		m_iPosition_File_LoopEndInSoundData			= m_stInitSettings.iLoopEnd;
		m_iFlags									= m_stInitSettings.iFlags;
		m_stWaveFormat.wFormatTag					= m_stInitSettings.iFormat;
		m_iLoopCount								= m_stInitSettings.iLoopCount;
		m_fWetMixFactor								= m_stInitSettings.fWetMixFactor;

		m_stInitSettings.bSetupRequested = FALSE;

		// Setup Channel Map

		Setup_Channels();

		// Start Initial Read

		m_iPosition_Buffer_ToLoad	= 0;
		m_iPosition_Buffer_ToDecode	= 0;
		m_iPosition_Buffer_ToPlay	= 0;
		m_iSize_LoadedData_Decoded	= 0;

		m_bHasReachedEndOfFile = FALSE;

		// Reset Voice

		m_pPacketTracker->Reset();

		SetVolume(m_stInitSettings.iVolume);
		SetFrequency(m_stInitSettings.iFrequency);
		SetPan(m_stInitSettings.iPan, 0);

		// Load & Submit Packets

		int PacketCount = IsSinglePacket() ? 1 : kiStreamPacketCount;

		while (PacketCount > 0)
		{
			Packet_LoadStart();
			PacketCount--;
		}
	}
}

// ***********************************************************************************************************************

void CVoice::Pause()
{
	// Error Checking

	if (m_pSourceVoice == NULL) return;

	// Playing -> Paused

	if (IsPlaying())
	{
		m_eState = eState_Paused;
		m_pSourceVoice->Stop(0);
		return;
	}

	// Paused -> Playing

	if (IsPaused())
	{
		m_eState = eState_Playing;
		m_pSourceVoice->Start(0);
	}
}

// ***********************************************************************************************************************

void CVoice::Stop()
{
	// Error Checking

	if (m_pSourceVoice == NULL) return;

	// Update State

	if (IsPlaying() || IsPaused())
	{
		m_eState = eState_Stopped;
	}

	// Discard All Submitted Packets

	m_pSourceVoice->FlushPackets();
	m_pSourceVoice->Stop(XAUDIOSTOP_IMMEDIATE);
}

// ***********************************************************************************************************************

void CVoice::UpdateVolume()
{
	// Error Checking

	if (m_pSourceVoice == NULL) return;

	// Calculate Float Volume

	float fVolume = SND_f_GetVolFromAtt(m_iVolume) + m_fVolumeAddGain;

	// Clamp To [0.0, 1.0] Range

	if (fVolume < 0.0f)
	{
		fVolume = 0.0f;
	}

	if (fVolume > 1.0f)
	{
		fVolume = 1.0f;
	}

	// Update Source Voice

	m_pSourceVoice->SetVolume(fVolume * GetAudioEngine()->GetReinitVolumeFactor());
}

// ***********************************************************************************************************************

void CVoice::SetVolume(int Param_iVolume)
{
	// Error Checking

	if (m_pSourceVoice == NULL) return;

	// Calculate New Volume

	int iNewVolume = Param_iVolume + GetAudioEngine()->GetGlobalVolume();

	// Update Voice Volume (If Applicable)

	if (m_iVolume != iNewVolume)
	{
		m_iVolume = iNewVolume;

		UpdateVolume();
	}
}

// ***********************************************************************************************************************

void CVoice::SetVolumeAddGain(float Param_fVolumeAddGain)
{
	// Error Checking

	if (m_fVolumeAddGain == Param_fVolumeAddGain)
	{
		return;
	}

	// Clamp Value To [-1.0, 1.0] Range

	if (Param_fVolumeAddGain < -1.0f)
	{
		Param_fVolumeAddGain = -1.0f;
	}

	if (Param_fVolumeAddGain > 1.0f)
	{
		Param_fVolumeAddGain = 1.0f;
	}

	// Update Additive Gain & Volume

	m_fVolumeAddGain = Param_fVolumeAddGain;

	UpdateVolume();
}

// ***********************************************************************************************************************

void CVoice::SetFrequency(int Param_iFrequency)
{
	// Error Checking

	if ((m_pSourceVoice == NULL) || (m_iFrequency == Param_iFrequency) || (m_stSourceVoiceInit.Format.SampleRate <= 0)) return;

	// Update Voice Frequency

	m_iFrequency = Param_iFrequency;

	// Clamp Frequency To XAudio Limits

	if (Param_iFrequency < XAUDIOSAMPLERATE_MIN)
	{
		Param_iFrequency = XAUDIOSAMPLERATE_MIN;
	}

	if (Param_iFrequency > XAUDIOSAMPLERATE_MAX)
	{
		Param_iFrequency = XAUDIOSAMPLERATE_MAX;
	}

	// Calculate Pitch Factor

	float Pitch;
	float Factor = (float)Param_iFrequency / (float)m_stSourceVoiceInit.Format.SampleRate;

	if (Factor >= 1.0f)
	{
		Pitch = Factor - 1.0f;

		if (Pitch > kfPitch_Maximum)
		{
			Pitch = kfPitch_Maximum;
		}
	}
	else
	{
		if (Factor > 0.0f)
		{
			Pitch = (-1.0f / Factor) + 1.0f;

			if (Pitch < kfPitch_Minimum)
			{
				Pitch = kfPitch_Minimum;
			}
		}
		else
		{
			Pitch = kfPitch_Minimum;
		}
	}

	// Update Voice Pitch

	m_pSourceVoice->SetPitch(Pitch);
}

// ***********************************************************************************************************************

void CVoice::SetPan(int Param_iPan, int Param_iSurroundPan)
{
	// Error Checking

	if (m_pSourceVoice == NULL)
	{
		return;
	}

	// Ensure Different Values

	if ((m_iPan == Param_iPan) && (m_iSurroundPan == Param_iSurroundPan))
	{
		return;
	}

	// Update Pan Values

	m_iPan			= Param_iPan;
	m_iSurroundPan	= Param_iSurroundPan;

	// Setup Voice Output Structures

	XAUDIOVOICEOUTPUTVOLUME			VoiceVolume;
	XAUDIOVOICEOUTPUTVOLUMEENTRY	VoiceEntry[/*3*/2];
	XAUDIOCHANNELVOLUME				ChannelVolume_Panned;
	XAUDIOCHANNELVOLUME				ChannelVolume_Full;
	XAUDIOCHANNELVOLUME				ChannelVolume_Silent;
	XAUDIOCHANNELVOLUMEENTRY		ChannelVolumeEntries_Panned[XAUDIOSPEAKER_COUNT];
	XAUDIOCHANNELVOLUMEENTRY		ChannelVolumeEntries_Full[XAUDIOSPEAKER_COUNT];
	XAUDIOCHANNELVOLUMEENTRY		ChannelVolumeEntries_Silent[XAUDIOSPEAKER_COUNT];

	ChannelVolume_Panned.EntryCount	= XAUDIOSPEAKER_COUNT;
	ChannelVolume_Panned.paEntries	= ChannelVolumeEntries_Panned;

	ChannelVolume_Full.EntryCount	= XAUDIOSPEAKER_COUNT;
	ChannelVolume_Full.paEntries	= ChannelVolumeEntries_Full;

	ChannelVolume_Silent.EntryCount	= XAUDIOSPEAKER_COUNT;
	ChannelVolume_Silent.paEntries	= ChannelVolumeEntries_Silent;

	VoiceEntry[0].OutputVoiceIndex	= 0;
	VoiceEntry[0].pVolume			= &ChannelVolume_Panned;
	VoiceEntry[1].OutputVoiceIndex	= 1;
	VoiceEntry[1].pVolume			= &ChannelVolume_Full;//Panned;
	//VoiceEntry[2].OutputVoiceIndex	= 2;
	//VoiceEntry[2].pVolume			= &ChannelVolume_Full;

	VoiceVolume.EntryCount			= m_iVoiceOutputs;
	VoiceVolume.paEntries			= VoiceEntry;

	for (int Loop = 0; Loop < sizeof(ChannelVolumeEntries_Panned) / sizeof(XAUDIOCHANNELVOLUMEENTRY); Loop++)
	{
		ChannelVolumeEntries_Panned[Loop].EntryIndex	= (XAUDIOCHANNEL)Loop;
		ChannelVolumeEntries_Full[Loop].EntryIndex		= (XAUDIOCHANNEL)Loop;
		ChannelVolumeEntries_Full[Loop].Volume			= XAUDIOVOLUME_MAX;
		ChannelVolumeEntries_Silent[Loop].EntryIndex	= (XAUDIOCHANNEL)Loop;
		ChannelVolumeEntries_Silent[Loop].Volume		= 0.0f;
	}

	// Output Wet Channels Only (If Applicable)

	if ((m_iVoiceOutputs > 1) && (m_fWetMixFactor >= kfWetMixFactor_DryKickOut))
	{
		VoiceEntry[0].pVolume = &ChannelVolume_Silent;
	}

	// Compute Speaker Factors

	float Volume_LeftFactor;
	float Volume_RightFactor;

	float Volume_2ChannelLeftFactor;
	float Volume_2ChannelRightFactor;

	float Volume_FrontFactor;
	float Volume_BackFactor;

	Volume_2ChannelLeftFactor	= SND_f_GetVolFromAtt(-Param_iPan);
	Volume_2ChannelRightFactor	= SND_f_GetVolFromAtt(Param_iPan);

	Volume_FrontFactor	= SND_f_GetVolFromAtt(Param_iSurroundPan);
	Volume_BackFactor	= SND_f_GetVolFromAtt(-Param_iSurroundPan) * kfRearAmplitudeDampening;

	switch (m_stWaveFormat.nChannels)
	{
		// Mono

		case 1:
		{
			// Left & Right Silent When Sound Centered (Using Center Speaker)

			Volume_LeftFactor	= (1.0f - SND_f_GetVolFromAtt(Param_iPan));
			Volume_RightFactor	= (1.0f - SND_f_GetVolFromAtt(-Param_iPan));
			break;
		}

		// Stereo

		case 2:
		{
			// Left & Right At Maximum When Sound Centered (No Center Channel)

			Volume_LeftFactor	= Volume_2ChannelLeftFactor;
			Volume_RightFactor	= Volume_2ChannelRightFactor;
			break;
		}

		// Dolby Digital 5.1

		case 6:
		{
			// Left & Right Silent When Sound Centered (Using Center Channel)

			Volume_LeftFactor	= (1.0f - SND_f_GetVolFromAtt(Param_iPan));
			Volume_RightFactor	= (1.0f - SND_f_GetVolFromAtt(-Param_iPan));
			break;
		}

		// Unknown

		default:
		{
#if defined(AUDIOCONSOLE_ENABLE)
			AudioConsole_WaitForMessageExclusion();
			AudioConsole_AddMessageParameter(m_stWaveFormat.nChannels, L"Channels");
			AudioConsole_AddMessage(L"SetPan - Unknown Format", kszModuleName_XenonSound, kstMessageType_Error);
#endif
		}
	}

	ChannelVolumeEntries_Panned[XAUDIOSPEAKER_FRONTLEFT   ].Volume = Volume_FrontFactor * Volume_LeftFactor;
	ChannelVolumeEntries_Panned[XAUDIOSPEAKER_FRONTRIGHT  ].Volume = Volume_FrontFactor * Volume_RightFactor;
	ChannelVolumeEntries_Panned[XAUDIOSPEAKER_FRONTCENTER ].Volume = Volume_FrontFactor * (1.0f - max(Volume_LeftFactor, Volume_RightFactor));
	ChannelVolumeEntries_Panned[XAUDIOSPEAKER_LOWFREQUENCY].Volume = XAUDIOVOLUME_MAX;
	ChannelVolumeEntries_Panned[XAUDIOSPEAKER_BACKLEFT    ].Volume = Volume_BackFactor  * Volume_2ChannelLeftFactor;
	ChannelVolumeEntries_Panned[XAUDIOSPEAKER_BACKRIGHT   ].Volume = Volume_BackFactor  * Volume_2ChannelRightFactor;

	// Update Source Voice

	m_pSourceVoice->SetVoiceOutputVolume(&VoiceVolume);
}

// ***********************************************************************************************************************

int CVoice::GetVolume()
{
	return m_iVolume;
}

// ***********************************************************************************************************************

float CVoice::GetVolumeAddGain()
{
	return m_fVolumeAddGain;
}

// ***********************************************************************************************************************

int CVoice::GetFrequency()
{
	return m_iFrequency;
}

// ***********************************************************************************************************************

int CVoice::GetPan()
{
	return m_iPan;
}

// ***********************************************************************************************************************

int CVoice::GetSurroundPan()
{
	return m_iSurroundPan;
}

// ***********************************************************************************************************************

int CVoice::GetChannelCount()
{
	return m_stWaveFormat.nChannels;
}

// ***********************************************************************************************************************

int CVoice::GetLoopCount()
{
	return m_iLoopCount;
}

// ***********************************************************************************************************************

int CVoice::GetFileSize()
{
	return m_iSize_File_SoundData;
}

// ***********************************************************************************************************************

int CVoice::GetSamplesPerSecond()
{
	// Return Buffer Sample Consumption Per Second

	return (m_iFrequency * m_stSourceVoiceInit.Format.ChannelCount);
}

// ***********************************************************************************************************************

float CVoice::GetBufferDurationInSeconds()
{
	if ((GetPlaybackBuffer()->GetSize_Capacity() <= 0) || (GetSamplesPerSecond() <= 0))
	{
		return 0.0f;
	}

	return ((float)GetPlaybackBuffer()->GetSize_Capacity() / (float)GetSamplesPerSecond());
}

// ***********************************************************************************************************************

float CVoice::GetFileDurationInSeconds()
{
	if ((GetFileSize() <= 0) || (GetSamplesPerSecond() <= 0))
	{
		return 0.0f;
	}

	return ((float)GetFileSize() / (float)GetSamplesPerSecond());
}

// ***********************************************************************************************************************

float CVoice::GetChainDelay()
{
	// Error Checking

	if (!IsPlaying() || (m_stInitSettings.iPositionChain <= 0))
	{
		return 0.0f;
	}

	// Calculate Distance (In Samples)

	int iSampleDistance = m_stInitSettings.iPositionChain - GetCursorPosition_PCM();

	if ((iSampleDistance <= 0) || (GetSamplesPerSecond() <= 0))
	{
		return 0.0f;
	}

	// Calculate Time To Cover Distance

	return ((float)iSampleDistance / (float)GetSamplesPerSecond());
}

// ***********************************************************************************************************************

int CVoice::GetPosition_File_Current()
{
	return m_iPosition_File_CurrentOffsetInSoundData;
}

// ***********************************************************************************************************************

int CVoice::GetPosition_File_Stop()
{
	return m_iPosition_File_StopOffsetInSoundData;
}

// ***********************************************************************************************************************

int CVoice::GetPosition_File_LoopBegin()
{
	return m_iPosition_File_LoopBeginInSoundData;
}

// ***********************************************************************************************************************

int CVoice::GetPosition_File_LoopEnd()
{
	return m_iPosition_File_LoopEndInSoundData;
}

// ***********************************************************************************************************************

int CVoice::GetPosition_File_Chain()
{
	return m_stInitSettings.iPositionChain;
}

// ***********************************************************************************************************************

int CVoice::GetPosition_Buffer_Load()
{
	return m_iPosition_Buffer_ToLoad;
}

// ***********************************************************************************************************************

int CVoice::GetPosition_Buffer_Decode()
{
	return m_iPosition_Buffer_ToDecode;
}

// ***********************************************************************************************************************

int CVoice::GetPosition_Buffer_Play()
{
	return m_iPosition_Buffer_ToPlay;
}

// ***********************************************************************************************************************

int CVoice::GetCursorInBuffer_PCM()
{
	// Error Checking

	if (m_pSourceVoice == NULL)
	{
		return 0;
	}

	// Fetch Packet Position

	XAUDIOPACKETPOSITION iPosition;

	m_pSourceVoice->GetPacketPosition(&iPosition);

	// Add Playback Buffer Packet Offset (If Applicable)

	iPosition.Position += m_pPacketTracker->GetCurrentPosition_Buffer();

	// Return Offset Position In File

	return iPosition.Position;
}

// ***********************************************************************************************************************

int CVoice::GetCursorInBuffer_Native()
{
	return SoundPosition_PCMToNative(GetCursorInBuffer_PCM());
}

// ***********************************************************************************************************************

int CVoice::GetCursorPosition_PCM()
{
	// Error Checking

	if (m_pSourceVoice == NULL)
	{
		return 0;
	}

	// Fetch Packet Position

	XAUDIOPACKETPOSITION iPosition;

	m_pSourceVoice->GetPacketPosition(&iPosition);

	// Add Stream File Offset (If Applicable)

	if (IsAStream())
	{
		iPosition.Position += m_pPacketTracker->GetCurrentPosition_File();
	}

	// Return Offset Position In File

	return iPosition.Position;
}

// ***********************************************************************************************************************

int CVoice::GetCursorPosition_Native()
{
	return SoundPosition_PCMToNative(GetCursorPosition_PCM());
}

// ***********************************************************************************************************************

void CVoice::SetCursorPosition_PCM(int Param_iPosition)
{
	// Convert Position From Native Format

	Param_iPosition = SoundPosition_NativeToPCM(Param_iPosition);

	// Error Checking

	if ((m_pSourceVoice == NULL) || (Param_iPosition < 0) || (Param_iPosition >= m_pBuffer_Playback->GetSize_Capacity()))
	{
		return;
	}

	// Check For Stream Seeking

	if (IsAStream() && (Param_iPosition != 0))
	{
#if defined(AUDIOCONSOLE_ENABLE)
		AudioConsole_AddMessageParameter(Param_iPosition);
		AudioConsole_AddMessage(L"SetCursorPosition - Stream Seek Not Supported", kszModuleName_XenonSound, kstMessageType_Error);
#endif
		return;
	}

	// Set New Packet Position

	XAUDIOPACKETPOSITION iPosition;

	iPosition.Position = Param_iPosition;

	m_pSourceVoice->SetPacketPosition(&iPosition);
}

// ***********************************************************************************************************************

void CVoice::SetCursorPosition_Native(int Param_iPosition)
{
	SetCursorPosition_PCM(SoundPosition_NativeToPCM(Param_iPosition));
}

// ***********************************************************************************************************************

void CVoice::TransferToDuplicate(CVoice* Param_pSourceVoice)
{
	// Error Checking

	if (Param_pSourceVoice == NULL)
	{
		return;
	}

	// Transfer Loaded Static Buffer Size

	m_iSize_LoadedData_Decoded = Param_pSourceVoice->m_iSize_LoadedData_Decoded;
}

// ***********************************************************************************************************************

void CVoice::Setup_Channels()
{
	XAUDIOVOICEOUTPUT		VoiceOutput;
	XAUDIOVOICEOUTPUTENTRY	VoiceOutputEntry[2/*3*/];
	XAUDIOCHANNELMAP		ChannelMap;
	XAUDIOCHANNELMAPENTRY	ChannelMapEntry[XAUDIOSPEAKER_COUNT];

	// Error Checking

	if (m_pSourceVoice == NULL)
	{
		return;
	}

	// Configure Speaker Input

	switch (m_stWaveFormat.nChannels)
	{
		// Mono

		case 1:
		{
			ChannelMapEntry[XAUDIOSPEAKER_FRONTLEFT   ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
			ChannelMapEntry[XAUDIOSPEAKER_FRONTRIGHT  ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
			ChannelMapEntry[XAUDIOSPEAKER_FRONTCENTER ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
			ChannelMapEntry[XAUDIOSPEAKER_LOWFREQUENCY].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
			ChannelMapEntry[XAUDIOSPEAKER_BACKLEFT    ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
			ChannelMapEntry[XAUDIOSPEAKER_BACKRIGHT   ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
			break;
		}

		// Stereo

		case 2:
		{
			ChannelMapEntry[XAUDIOSPEAKER_FRONTLEFT   ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
			ChannelMapEntry[XAUDIOSPEAKER_FRONTRIGHT  ].InputChannel = XAUDIOSPEAKER_FRONTRIGHT;
			ChannelMapEntry[XAUDIOSPEAKER_FRONTCENTER ].InputChannel = XAUDIOSPEAKER_FRONTCENTER; // No feed
			ChannelMapEntry[XAUDIOSPEAKER_LOWFREQUENCY].InputChannel = XAUDIOSPEAKER_FRONTLEFT;   // Should work well enough
			ChannelMapEntry[XAUDIOSPEAKER_BACKLEFT    ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
			ChannelMapEntry[XAUDIOSPEAKER_BACKRIGHT   ].InputChannel = XAUDIOSPEAKER_FRONTRIGHT;
			break;
		}

		// Dolby Digital 5.1

		case 6:
		{
			ChannelMapEntry[XAUDIOSPEAKER_FRONTLEFT   ].InputChannel = XAUDIOSPEAKER_FRONTLEFT;
			ChannelMapEntry[XAUDIOSPEAKER_FRONTRIGHT  ].InputChannel = XAUDIOSPEAKER_FRONTRIGHT;
			ChannelMapEntry[XAUDIOSPEAKER_FRONTCENTER ].InputChannel = XAUDIOSPEAKER_FRONTCENTER;
			ChannelMapEntry[XAUDIOSPEAKER_LOWFREQUENCY].InputChannel = XAUDIOSPEAKER_LOWFREQUENCY;
			ChannelMapEntry[XAUDIOSPEAKER_BACKLEFT    ].InputChannel = XAUDIOSPEAKER_BACKLEFT;
			ChannelMapEntry[XAUDIOSPEAKER_BACKRIGHT   ].InputChannel = XAUDIOSPEAKER_BACKRIGHT;
			break;
		}

		// Unknown

		default:
		{
#if defined(AUDIOCONSOLE_ENABLE)
			AudioConsole_WaitForMessageExclusion();
			AudioConsole_AddMessageParameter(m_stWaveFormat.nChannels, L"Channels");
			AudioConsole_AddMessage(L"Setup Channels - Unknown Format", kszModuleName_XenonSound, kstMessageType_Error);
#endif
		}
	}

    // Set Up Channel Map

	ChannelMap.EntryCount			= XAUDIOSPEAKER_COUNT;
	ChannelMap.paEntries			= ChannelMapEntry;

	VoiceOutputEntry[0].pDestVoice	= NULL;
	VoiceOutputEntry[0].pChannelMap	= &ChannelMap;
	VoiceOutputEntry[1].pDestVoice	= NULL;
	VoiceOutputEntry[1].pChannelMap	= &ChannelMap;
	//VoiceOutputEntry[2].pDestVoice	= NULL;
	//VoiceOutputEntry[2].pChannelMap	= &ChannelMap;

	VoiceOutput.EntryCount			= 1;
	VoiceOutput.paEntries			= VoiceOutputEntry;

#if _XENON_SOUND_FX
	// Configure For Submix Output (If Applicable)

	if (GetAudioEngine()->GetRuntimeSettings()->bAllowFX)
	{
		// Fetch Core

		CFXCore* pCore = NULL;

		if (GetAudioEngine()->GetRuntimeSettings()->iForcePlayToFXCore >= 0)
		{
			// Force To FX Core (If Applicable)

			pCore = GetAudioEngine()->GetFXCore(GetAudioEngine()->GetRuntimeSettings()->iForcePlayToFXCore);
		}
		else
		{
			// Use Core Requested By Flags

			if ((m_iFlags & (SND_Cul_SBC_PlayFxA | SND_Cul_SBC_PlayFxB)) && (m_fWetMixFactor >= kfWetMixFactor_WetKickIn))
			{
				// Forcing Output To Core A (Hopefully sidestepping the audio corruption issue)

				//if (m_iFlags & SND_Cul_SBC_PlayFxA)
				//{
					pCore = GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA);
				//}
				//else
				//{
					//pCore = GetAudioEngine()->GetFXCore(SND_Cte_FxCoreB);
				//}
			}
		}

		// - Secondary / Tertiary Output To Appropriate Core (If Applicable)

		if (pCore != NULL)
		{
			VoiceOutput.EntryCount = 2;//3;

			//VoiceOutputEntry[1].pDestVoice = pCore->GetVoice_Delay();;
			VoiceOutputEntry[1/*2*/].pDestVoice = pCore->GetVoice_Reverb();
		}
	}
#endif

	// Configure Default Outputs and Volumes

	for (int Loop = 0; Loop < sizeof(ChannelMapEntry) / sizeof(XAUDIOCHANNELMAPENTRY); Loop++)
	{
		ChannelMapEntry[Loop].OutputChannel	= (XAUDIOCHANNEL)Loop;
		ChannelMapEntry[Loop].Volume		= XAUDIOVOLUME_MAX;
	}

	// Send Output Configuration To Source Voice

	m_iVoiceOutputs = VoiceOutput.EntryCount;

	m_pSourceVoice->SetVoiceOutput(&VoiceOutput);

	// Reinitialize Output Volumes & Set Default Panning To Center

	m_iPan = -1;

	SetPan(0, 0);
}

// ***********************************************************************************************************************

void CVoice::Setup_Static(SND_tdst_WaveData* Param_pWaveData, unsigned int Param_uiFilePositionInBF)
{
	// Error Checking

	if (Param_pWaveData == NULL)
	{
		return;
	}

	// Convert Size From Native Format

	int iSize_PCM = SoundPosition_NativeToPCM(Param_pWaveData->i_Size, Param_pWaveData->st_WaveFmtx.wFormatTag);

	// Reset Sound Buffer Settings

	m_stWaveFormat								= Param_pWaveData->st_WaveFmtx;
	m_iFrequency								= m_stWaveFormat.nSamplesPerSec;
	m_iPan										= -1;
	m_iSurroundPan								= -1;
	m_iVolume									= -1;
	m_uiPosition_File_SoundStartInBF			= Param_uiFilePositionInBF;
	m_iPosition_File_CurrentOffsetInSoundData	= 0;
	m_iPosition_File_StopOffsetInSoundData		= iSize_PCM;
	m_iPosition_File_LoopBeginInSoundData		= 0;
	m_iPosition_File_LoopEndInSoundData			= iSize_PCM;
	m_iPosition_Buffer_ToLoad					= 0;
	m_iPosition_Buffer_ToDecode					= 0;
	m_iPosition_Buffer_ToPlay					= 0;
	m_iSize_File_SoundData						= iSize_PCM;
	m_iSize_LoadedData_Decoded					= 0;
	m_bHasReachedEndOfFile						= FALSE;
	m_eState									= eState_Setup;

	// Load data for WAV (Leave initially empty for WAC)

	if (Param_uiFilePositionInBF > 0)
	{
		Packet_LoadStart();
	}
}

// ***********************************************************************************************************************

void CVoice::Setup_Stream_SetFormat(int Param_iFormat)
{
	m_stInitSettings.iFormat = Param_iFormat;
}

// ***********************************************************************************************************************

void CVoice::Setup_Stream_Play(int Param_iFlags, int Param_iLoopCount, int Param_iFileSize, unsigned int Param_uiFilePositionInBF, int Param_iLoopBeginPosition, int Param_iLoopEndPosition, int Param_iStartPositionInSoundData, int Param_iStopPositionInSoundData, int Param_iFrequency, int Param_iPan, int Param_iVolume, int Param_iFxVolume_Left, int Param_iFxVolume_Right)
{
	// Convert Positions & Clamp Down To Nearest Loop-Point

	Param_iFileSize					= SoundPosition_NativeToPCM(Param_iFileSize, m_stInitSettings.iFormat);
	Param_iStartPositionInSoundData	= SoundPosition_NativeToPCM(Param_iStartPositionInSoundData, m_stInitSettings.iFormat);
	Param_iStopPositionInSoundData	= SoundPosition_NativeToPCM(Param_iStopPositionInSoundData, m_stInitSettings.iFormat);
	Param_iLoopBeginPosition		= SoundPosition_NativeToPCM(Param_iLoopBeginPosition, m_stInitSettings.iFormat);
	Param_iLoopEndPosition			= SoundPosition_NativeToPCM(Param_iLoopEndPosition, m_stInitSettings.iFormat);

	// Error Checking

	if ((Param_iStartPositionInSoundData < 0) || (Param_iStartPositionInSoundData > Param_iFileSize))
	{
		Param_iStartPositionInSoundData = 0;
	}

	if ((Param_iStopPositionInSoundData <= Param_iStartPositionInSoundData) || (Param_iStopPositionInSoundData > Param_iFileSize))
	{
		Param_iStopPositionInSoundData = Param_iFileSize;
	}

	if ((Param_iLoopBeginPosition < 0) || (Param_iLoopBeginPosition > Param_iFileSize))
	{
		Param_iLoopBeginPosition = 0;
	}

	if ((Param_iLoopEndPosition <= Param_iLoopBeginPosition) || (Param_iLoopEndPosition > Param_iFileSize))
	{
		Param_iLoopEndPosition = Param_iFileSize;
	}

	// Ensure Exact Same Stream Not Already Playing

	if ((IsPlaying() || IsSetupping()) && (m_stInitSettings.uiFilePosition == Param_uiFilePositionInBF))
	{
		return;
	}

	// Populate Reinit Structure

	m_stInitSettings.bSetupRequested		= TRUE;
	m_stInitSettings.iFileSize				= Param_iFileSize;
	m_stInitSettings.uiFilePosition			= Param_uiFilePositionInBF;
	m_stInitSettings.iPositionStart			= Param_iStartPositionInSoundData;
	m_stInitSettings.iPositionStop			= Param_iStopPositionInSoundData;
	m_stInitSettings.iPositionChain			= 0;
	m_stInitSettings.iLoopBegin				= Param_iLoopBeginPosition;
	m_stInitSettings.iLoopEnd				= Param_iLoopEndPosition;
	m_stInitSettings.iFlags					= Param_iFlags;
	m_stInitSettings.iFrequency				= Param_iFrequency;
	m_stInitSettings.iPan					= Param_iPan;
	m_stInitSettings.iVolume				= Param_iVolume;
	m_stInitSettings.iLoopCount				= Param_iLoopCount;
	m_stInitSettings.fWetMixFactor			= 0.5f * (SND_f_GetVolFromAtt(Param_iFxVolume_Left) + SND_f_GetVolFromAtt(Param_iFxVolume_Right));

	// Request Reinit Event

	GetAudioEngine()->RequestEvent(eEventID_StreamPlay, this);
}

// ***********************************************************************************************************************

void CVoice::Setup_Stream_Chain(int Param_iFlags, int Param_iLoopCount, int Param_iExitPointInSoundData, int Param_iEnterPointInSoundData, int Param_iFileSize, unsigned int Param_uiFilePositionInBF, int Param_iLoopBeginPosition, int Param_iLoopEndPosition)
{
	// Convert Positions & Clamp Down To Nearest Loop-Point

	Param_iFileSize					= SoundPosition_NativeToPCM(Param_iFileSize, m_stInitSettings.iFormat);
	Param_iEnterPointInSoundData	= SoundPosition_NativeToPCM(Param_iEnterPointInSoundData, m_stInitSettings.iFormat);
	Param_iExitPointInSoundData		= SoundPosition_NativeToPCM(Param_iExitPointInSoundData, m_stInitSettings.iFormat);
	Param_iLoopBeginPosition		= SoundPosition_NativeToPCM(Param_iLoopBeginPosition, m_stInitSettings.iFormat);
	Param_iLoopEndPosition			= SoundPosition_NativeToPCM(Param_iLoopEndPosition, m_stInitSettings.iFormat);

	// Error Checking

	if ((Param_iEnterPointInSoundData < 0) || (Param_iEnterPointInSoundData > Param_iFileSize))
	{
		Param_iEnterPointInSoundData = 0;
	}

	if ((Param_iExitPointInSoundData < 0) || (Param_iExitPointInSoundData > m_iSize_File_SoundData))
	{
		Param_iExitPointInSoundData = m_iSize_File_SoundData;
	}

	if ((Param_iLoopBeginPosition < 0) || (Param_iLoopBeginPosition > Param_iFileSize))
	{
		Param_iLoopBeginPosition = 0;
	}

	if ((Param_iLoopEndPosition <= Param_iLoopBeginPosition) || (Param_iLoopEndPosition > Param_iFileSize))
	{
		Param_iLoopEndPosition = Param_iFileSize;
	}

	// Ensure Exact Same Stream Not Already Playing

	if ((IsPlaying() || IsSetupping()) && (m_stInitSettings.uiFilePosition == Param_uiFilePositionInBF))
	{
		return;
	}

	// Populate Reinit Structure

	m_stInitSettings.iFileSize		= Param_iFileSize;
	m_stInitSettings.uiFilePosition	= Param_uiFilePositionInBF;
	m_stInitSettings.iPositionStart	= Param_iEnterPointInSoundData;
	m_stInitSettings.iPositionChain	= Param_iExitPointInSoundData;
	m_stInitSettings.iLoopBegin		= Param_iLoopBeginPosition;
	m_stInitSettings.iLoopEnd		= Param_iLoopEndPosition;
	m_stInitSettings.iFlags			= Param_iFlags;
	m_stInitSettings.iLoopCount		= Param_iLoopCount;
}

// ***********************************************************************************************************************

void CVoice::Packet_LoadStart()
{
	// Check Format

	if (GetEncoding() == eEncoding_Unknown)
	{
#if defined(AUDIOCONSOLE_ENABLE)
		AudioConsole_AddMessage(L"Unsupported WAVE Format", kszModuleName_XenonSound, kstMessageType_Error);
#endif
		return;
	}

	// Determine Packet Size

	int iSize;

	if (IsSinglePacket())
	{
		iSize = m_iSize_File_SoundData;
	}
	else
	{
		iSize = kiStreamPacketSize;
	}

	// - Clamp : Loading Buffer Extends (If Applicable)

	if (!m_pBuffer_Loading->IsMemoryOwner() && ((m_iPosition_Buffer_ToLoad + iSize) > m_pBuffer_Loading->GetSize_Capacity()))
	{
		iSize = m_pBuffer_Loading->GetSize_Capacity() - m_iPosition_Buffer_ToLoad;
	}

	// - Clamp : Sound File Size (If Applicable)

	if ((m_iPosition_File_CurrentOffsetInSoundData + iSize) > m_iSize_File_SoundData)
	{
		iSize = m_iSize_File_SoundData - m_iPosition_File_CurrentOffsetInSoundData;
	}

	// - Clamp : Stop Position (If Applicable)

	if ((m_iPosition_File_CurrentOffsetInSoundData + iSize) > m_iPosition_File_StopOffsetInSoundData)
	{
		iSize = m_iPosition_File_StopOffsetInSoundData - m_iPosition_File_CurrentOffsetInSoundData;
	}

	// - Clamp : Loop End (If Applicable)

	if (IsLooping() && ((m_iPosition_File_CurrentOffsetInSoundData + iSize) > m_iPosition_File_LoopEndInSoundData))
	{
		iSize = m_iPosition_File_LoopEndInSoundData - m_iPosition_File_CurrentOffsetInSoundData;
	}

	// - Clamp : Chain Point (If Applicable)

	if ((m_stInitSettings.iPositionChain > 0) && ((m_iPosition_File_CurrentOffsetInSoundData + iSize) > m_stInitSettings.iPositionChain))
	{
		iSize = m_stInitSettings.iPositionChain - m_iPosition_File_CurrentOffsetInSoundData;
	}

	// Check If Already Prefetched

	BOOL bIsInstantLoad;

	CPrefetchList* pPrefetchList = GetAudioEngine()->GetPrefetchList();

	CPrefetchBuffer* pBuffer = pPrefetchList->Find(m_uiPosition_File_SoundStartInBF, m_eSoundBFIndex);

	if ((pBuffer != NULL) && (pBuffer->GetStatus() == ePrefetchStatus_Useable))
	{
		bIsInstantLoad = TRUE;

		// Fake Load Notify (Extra Safety For The Mythical AI-Prefetched Stream)

		m_pPacketTracker->Notify_LoadStart(m_iPosition_File_CurrentOffsetInSoundData);

		// Copy From Prefetch Buffer

		iSize = SoundPosition_NativeToPCM(pBuffer->GetSize());

		pBuffer->RecoverDataFromPrefetch(RequestLoadingBuffer(iSize));

		m_iSize_LastReadResult = pBuffer->GetSize();

		// Discard Prefetch Buffer

		pPrefetchList->Discard(pBuffer);
	}
	else
	{
		bIsInstantLoad = FALSE;

		// Trigger Async Read From Disk

		m_pPacketTracker->Notify_LoadStart(m_iPosition_File_CurrentOffsetInSoundData);

		GetAudioEngine()->SoundFile_Read(RequestLoadingBuffer(iSize), SoundPosition_ClampDownToFrame(SoundPosition_PCMToNative(iSize)), (int*)&m_iSize_LastReadResult, m_uiPosition_File_SoundStartInBF + SoundPosition_ClampDownToFrame(SoundPosition_PCMToNative(m_iPosition_File_CurrentOffsetInSoundData)), this, m_eSoundBFIndex, eEventID_PacketLoadDone);
	}

	// Wrap - File Data Load Offset

	m_iPosition_File_CurrentOffsetInSoundData += iSize;

	// - File Size (If Applicable)

	if (m_iPosition_File_CurrentOffsetInSoundData >= m_iSize_File_SoundData)
	{
		m_iPosition_File_CurrentOffsetInSoundData = m_iPosition_File_LoopBeginInSoundData;
		m_bHasReachedEndOfFile = TRUE;
	}

	// - Stop Position (If Applicable)

	if (m_iPosition_File_CurrentOffsetInSoundData >= m_iPosition_File_StopOffsetInSoundData)
	{
		m_iPosition_File_CurrentOffsetInSoundData = m_iPosition_File_LoopBeginInSoundData;
		m_bHasReachedEndOfFile = TRUE;
	}

	// - Loop End (If Applicable)

	if (IsLooping() && (m_iPosition_File_CurrentOffsetInSoundData >= m_iPosition_File_LoopEndInSoundData))
	{
		m_iPosition_File_CurrentOffsetInSoundData = m_iPosition_File_LoopBeginInSoundData;
		m_bHasReachedEndOfFile = TRUE;
	}

	// - Chain Point (If Applicable)

	if ((m_stInitSettings.iPositionChain > 0) && (m_iPosition_File_CurrentOffsetInSoundData >= m_stInitSettings.iPositionChain))
	{
		// Apply New Chained Stream Settings

		m_iSize_File_SoundData						= m_stInitSettings.iFileSize;
		m_uiPosition_File_SoundStartInBF			= m_stInitSettings.uiFilePosition;
		m_iPosition_File_CurrentOffsetInSoundData	= m_stInitSettings.iPositionStart;
		m_iPosition_File_LoopBeginInSoundData		= m_stInitSettings.iLoopBegin;
		m_iPosition_File_LoopEndInSoundData			= m_stInitSettings.iLoopEnd;
		m_iFlags									= m_stInitSettings.iFlags;
		m_iLoopCount								= m_stInitSettings.iLoopCount;
		m_stWaveFormat.wFormatTag					= m_stInitSettings.iFormat;

		// Clear Chaining Point

		m_stInitSettings.iPositionChain = 0;
	}

	// Wrap - Buffer Position (If Applicable)

	if (!m_pBuffer_Loading->IsMemoryOwner())
	{
		m_iPosition_Buffer_ToLoad += iSize;

		if (m_iPosition_Buffer_ToLoad >= m_pBuffer_Loading->GetSize_Capacity())
		{
			m_iPosition_Buffer_ToLoad = 0;
		}
	}

	// Trigger Load Done (If Instant)

	if (bIsInstantLoad)
	{
		Packet_LoadDone();
	}
}

// ***********************************************************************************************************************

void CVoice::Packet_LoadDone()
{
	// Skip If Already Loading Full Packet Contingent (On Stream Reinit)

	if (m_pPacketTracker->GetLoadingCounter() > kiStreamPacketCount)
	{
		m_pPacketTracker->Notify_LoadEnd(TRUE);
		return;
	}
	else
	{
		m_pPacketTracker->Notify_LoadEnd(FALSE);
	}

	// Add To MapFile (If Applicable)

	if (!IsAStream())
	{
		GetAudioEngine()->GetHeaderList()->AddToMapFile(m_uiPosition_File_SoundStartInBF, m_eSoundBFIndex, m_iSize_LastReadResult, m_pBuffer_Loading->GetMemoryPointer());
	}

	// Decode Buffer

	Packet_Decode(SoundPosition_NativeToPCM(m_iSize_LastReadResult));
}

// ***********************************************************************************************************************

void CVoice::Packet_Decode(int Param_iSize)
{
	// Determine Size

	if (!m_pBuffer_Playback->IsEmpty() && ((m_iPosition_Buffer_ToDecode + Param_iSize) > m_pBuffer_Playback->GetSize_Capacity()))
	{
		Param_iSize = m_pBuffer_Playback->GetSize_Capacity() - m_iPosition_Buffer_ToDecode;
	}

	// Decode By Format

	switch (GetEncoding())
	{
		case eEncoding_PCM:
		{
			// Convert Small -> Big Endian

			if (m_stWaveFormat.wBitsPerSample == 16)
			{
				FlipEndian_16(m_pBuffer_Loading->RequestStorage(Param_iSize, m_iPosition_Buffer_ToDecode), Param_iSize);
				m_pBuffer_Loading->Release();
			}
			break;
		}

		case eEncoding_ADPCM:
		{
			// Calculate Size

			int iSize_ADPCM = SoundPosition_PCMToNative(Param_iSize);

			// Convert ADPCM -> PCM

			if (m_stWaveFormat.nBlockAlign > 0)
			{
				xeSND_ADPCM_Decode(m_pBuffer_Loading->RequestStorage(iSize_ADPCM), m_pBuffer_Playback->RequestStorage(Param_iSize, m_iPosition_Buffer_ToDecode), iSize_ADPCM / (36 * m_stWaveFormat.nChannels), m_stWaveFormat.nChannels);
				m_pBuffer_Playback->Release();
				m_pBuffer_Loading->Release();
			}
			break;
		}
	}

	// Release Loading Buffer

	m_pBuffer_Loading->Release();

	// Store Decoded Size (For Static Buffers)

	m_iSize_LoadedData_Decoded = Param_iSize;

	// Wrap - Buffer Decode Position

	m_iPosition_Buffer_ToDecode += Param_iSize;

	if (m_iPosition_Buffer_ToDecode >= m_pBuffer_Playback->GetSize_Capacity())
	{
		m_iPosition_Buffer_ToDecode = 0;
	}

	// Play Packet (If A Stream)

	if (IsAStream() && !IsStopped())
	{
		Packet_Submit(Param_iSize);
	}
	else
	{
		m_eState = eState_Stopped;
	}
}

// ***********************************************************************************************************************

BOOL CVoice::Packet_Submit(int Param_iSize)
{
	// Error Checking

	if ((m_pSourceVoice == NULL) || (m_pBuffer_Playback == NULL))
	{
		return FALSE;
	}

	// Ensure Voice Can Receive More Packets

	XAUDIOSOURCESTATE State;

	m_pSourceVoice->GetVoiceState(&State);

	if (State & XAUDIOSOURCESTATE_READYPACKET)
	{
		XAUDIOPACKET Packet;

		// Determine Size & Offset

		int iOffset;
		int iLoopCount;

		if (IsSinglePacket())
		{
			// Playable In Single Chunk

			iOffset		= 0;
			iLoopCount	= m_iLoopCount;
		}
		else
		{
			// Streamed Packet Chain

			iOffset		= m_iPosition_Buffer_ToPlay;
			iLoopCount	= XAUDIOLOOPCOUNT_MIN;

			if ((iOffset + Param_iSize) > m_pBuffer_Playback->GetSize_Capacity())
			{
				Param_iSize = m_pBuffer_Playback->GetSize_Capacity() - iOffset;
			}
		}

		// Prepare Packet

		Packet.pBuffer		= m_pBuffer_Playback->GetMemoryPointer(iOffset);
		Packet.BufferSize	= Param_iSize & -4;
		Packet.LoopCount	= iLoopCount;
		Packet.LoopStart	= 0;
		Packet.LoopLength	= Packet.BufferSize;
		Packet.pContext		= NULL;

		// Submit packet (If Applicable)

		if (Packet.BufferSize > 0)
		{
			HRESULT Result = m_pSourceVoice->SubmitPacket(&Packet, XAUDIOSUBMITPACKET_DISCONTINUITY);

			if (Result != S_OK)
			{
#if defined(AUDIOCONSOLE_ENABLE)
				AudioConsole_WaitForMessageExclusion();
				AudioConsole_AddMessageParameter(Result);
				AudioConsole_AddMessage(L"SubmitPacket - Error", kszModuleName_XenonSound, kstMessageType_Error);
#endif
				return FALSE;
			}

			// Start Playback (If Applicable)

			if (!IsPlaying() && !IsPaused())
			{
				m_pSourceVoice->Start(0);
				m_eState = eState_Playing;
			}

			m_pPacketTracker->Notify_Submit(iOffset);
		}

		// Wrap - Buffer Play Position

		m_iPosition_Buffer_ToPlay += Param_iSize;

		if (m_iPosition_Buffer_ToPlay >= m_pBuffer_Playback->GetSize_Capacity())
		{
			m_iPosition_Buffer_ToPlay = 0;
		}

		return TRUE;
	}
	else
	{
#if defined(AUDIOCONSOLE_ENABLE)
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddMessage(L"SubmitPacket - Queue Full", kszModuleName_XenonSound, kstMessageType_Error);
#endif
		return FALSE;
	}
}

// ***********************************************************************************************************************

void CVoice::Packet_EndNotify()
{
	// Update Counters

	m_pPacketTracker->Notify_PlayEnd();

	// Update Status (If Applicable)

	if ((m_pPacketTracker->GetSubmitCounter() <= 0) && (IsPlaying() || IsPaused()))
	{
		m_eState = eState_Stopped;
	}

	// Check For Playing Stream

	if (!IsAStream() || !IsPlaying())
	{
		return;
	}

	// Check For End-of-File

	if (m_bHasReachedEndOfFile)
	{
		// Ensure Stream Is Looping

		if (!IsLooping())
		{
			return;
		}

		// Decrement Looping Counter (If Applicable)

		if (m_iLoopCount > 0)
		{
			m_iLoopCount--;
		}

		// Seek To Loop Start Point

		m_bHasReachedEndOfFile = FALSE;
	}

	// Schedule Next Packet Load

	Packet_LoadStart();
}

// ***********************************************************************************************************************

#endif // _XENON

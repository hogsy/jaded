#include "Precomp.h"

#ifdef _XENON

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "INOut/INO.h"

#include "xeSND_Engine.h"
#include "xeSND_Voice.h"

#include "SouND/Sources/SNDvolume.h"

#include "ENGine/Sources/ENGvars.h"

// ***********************************************************************************************************************
//    Externs
// ***********************************************************************************************************************

extern int TEXT_i_GetLang();

extern float TIM_gf_dt;

extern int MEM_i_GetInternalSize(void *_pAddress);

// ***********************************************************************************************************************
//    Globals
// ***********************************************************************************************************************

CAudioEngine* g_pAudioEngine;

// ***********************************************************************************************************************
//    Class Methods : CAudioEngine
// ***********************************************************************************************************************

CAudioEngine::CAudioEngine()
{
	m_bIsInited					= FALSE;
	m_pVoiceList				= NULL;
	m_pThread					= NULL;
	m_pFile_BF					= NULL;
	m_iGlobalVolume				= 0;
	m_fReinitVolumeFactor		= 1.0f;
	m_iStreamCount				= 0;
	m_iVoiceInit_Key			= 0x00000000;
	m_eVoiceInit_SoundBF		= eSoundBF_None;
	m_pStreamReader				= NULL;
	m_pWaveParser				= NULL;
	m_pPrefetchList				= NULL;
	m_pHeaderList				= NULL;

	memset(&m_stRuntimeSettings	, 0, sizeof(m_stRuntimeSettings	));
	memset(&m_aMemoryTrackers	, 0, sizeof(m_aMemoryTrackers	));
	memset(m_pSoundBF			, 0, sizeof(m_pSoundBF			));
#if _XENON_SOUND_FX
	memset(m_apFXCore			, 0, sizeof(m_apFXCore			));
#endif
}

// ***********************************************************************************************************************

CAudioEngine::~CAudioEngine()
{
	Shutdown();
}

// ***********************************************************************************************************************

BOOL CAudioEngine::IsInited()
{
	return m_bIsInited;
}

// ***********************************************************************************************************************

void CAudioEngine::Initialize()
{
	// Error Checking

	if (m_bIsInited) return;

	// Initialize Fields

	m_bIsInited				= TRUE;
	m_iVoiceInit_Key		= 0x00000000;
	m_eVoiceInit_SoundBF	= eSoundBF_None;

	// Clear Memory Trackers

	L_memset(m_aMemoryTrackers, 0, eMemoryTracker_Count * sizeof(stMemoryTracker));

	// XAudio Init

	XAUDIOENGINEINIT	EngineInit		= {0};
	XAUDIOFXTABLE		EffectsTable	= {0};

	EffectsTable.EffectCount	= sizeof(kastEffects_Table) / sizeof(XAUDIOFXTABLEENTRY);
	EffectsTable.paEffects		= kastEffects_Table;

	EngineInit.SubmixStageCount	= 1; // Allow Reverb Submix Voice
	EngineInit.pEffectTable		= &EffectsTable;

	XAudioInitialize(&EngineInit);

	// Create "Game" BigFile Handle

	m_pFile_BF = new CFile(MAI_gst_InitStruct.asz_ProjectName);

	// Open Header Database

	GetHeaderList()->OpenDatabase();

	// Create "Sound" BigFile Handles

	for (int iLoop = 0; iLoop < eSoundBF_Count; iLoop++)
	{
		char acName[128];

		sprintf(acName, "GAME:\\Sound\\Sound_%s.bf", GetSoundBFName((eSoundBFIndex)iLoop));

		m_pSoundBF[iLoop] = new CBigFileReader(acName, GetHeaderList()->IsEmpty());
	}

	// Create Full Database (If Empty)

	if (GetHeaderList()->IsEmpty())
	{
		GetHeaderList()->BuildEntireDatabase();
	}

	// Create Sound Thread

	m_pThread = new CEventThread(eEventID_Count, kiThread_StackSizeKB, eThreadID_SoundStreams);

	// Create Voice List

	m_pVoiceList = new CVoiceList();

	// Create FX Cores

#if _XENON_SOUND_FX
	for (int Loop = 0; Loop < SND_Cte_FxCoreNb; Loop++)
	{
		m_apFXCore[Loop] = new CFXCore();
	}
#endif

	// Initialize Callbacks

	m_pThread->SetEventCallback(eEventID_PacketLoadDone		, fnThreadEvent_PacketLoadDone);
	m_pThread->SetEventCallback(eEventID_StreamPlay			, fnThreadEvent_StreamPlay);
	m_pThread->SetEventCallback(eEventID_PrefetchLoadDone	, fnThreadEvent_PrefetchLoadDone);

	// Init Runtime Settings Default Values

	m_stRuntimeSettings.bAllowStaticPlay				= TRUE;
	m_stRuntimeSettings.bAllowStreamPlay				= TRUE;
	m_stRuntimeSettings.bAllowPitching					= TRUE;
	m_stRuntimeSettings.bAllowVolumeChange				= TRUE;
	m_stRuntimeSettings.bAllowFX						= TRUE;
	m_stRuntimeSettings.iForcePlayToFXCore				= -1;
	m_stRuntimeSettings.iFXCoreA_ForcedMode				= SND_Cte_FxMode_Off;
	//m_stRuntimeSettings.iFXCoreA_ForcedDelay			= 0;
	//m_stRuntimeSettings.iFXCoreA_ForcedFeedback			= 0;
	m_stRuntimeSettings.fFXCoreA_ForcedWetVolume		= 0.0f;
	m_stRuntimeSettings.iFXCoreA_ForcedWetPan			= 0;
	//m_stRuntimeSettings.iFXCoreA_ForcedDelayAlgorithm	= eDelayAlgorithm_None;
	m_stRuntimeSettings.iFXCoreB_ForcedMode				= SND_Cte_FxMode_Off;
	//m_stRuntimeSettings.iFXCoreB_ForcedDelay			= 0;
	//m_stRuntimeSettings.iFXCoreB_ForcedFeedback			= 0;
	m_stRuntimeSettings.fFXCoreB_ForcedWetVolume		= 0.0f;
	m_stRuntimeSettings.iFXCoreB_ForcedWetPan			= 0;
	//m_stRuntimeSettings.iFXCoreB_ForcedDelayAlgorithm	= eDelayAlgorithm_None;

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_AddSetting(L"Playback");

	AudioConsole_AddSetting(L"Allow Static Play"					, &m_stRuntimeSettings.bAllowStaticPlay);
	AudioConsole_AddSetting(L"Allow Stream Play"					, &m_stRuntimeSettings.bAllowStreamPlay);
	AudioConsole_AddSetting(L"Allow Pitching"						, &m_stRuntimeSettings.bAllowPitching);
	AudioConsole_AddSetting(L"Allow Volume Change"					, &m_stRuntimeSettings.bAllowVolumeChange);

	AudioConsole_AddSetting(L"Effects");

	AudioConsole_AddSetting(L"Allow FX"								, &m_stRuntimeSettings.bAllowFX);
	AudioConsole_AddSetting(L"Force Play To FX Core"				, &m_stRuntimeSettings.iForcePlayToFXCore			, kstTrackerType_Enum_FXCores);

	AudioConsole_AddSetting(L"FX Core A - Forced Mode"				, &m_stRuntimeSettings.iFXCoreA_ForcedMode			, kstTrackerType_Enum_FXModes);
	//AudioConsole_AddSetting(L"FX Core A - Forced Delay"				, &m_stRuntimeSettings.iFXCoreA_ForcedDelay			, 0, 1000, 5);
	//AudioConsole_AddSetting(L"FX Core A - Forced Feedback"			, &m_stRuntimeSettings.iFXCoreA_ForcedFeedback		, 0, 100, 5);
	AudioConsole_AddSetting(L"FX Core A - Forced Wet Volume"		, &m_stRuntimeSettings.fFXCoreA_ForcedWetVolume		, 0.0f, 1.0f , 0.1f, fnSettingsCallback_ForcedWetVolume);
	AudioConsole_AddSetting(L"FX Core A - Forced Wet Pan"			, &m_stRuntimeSettings.iFXCoreA_ForcedWetPan		, -10000, 10000 , 250);
	//AudioConsole_AddSetting(L"FX Core A - Forced Delay Algorithm"	, &m_stRuntimeSettings.iFXCoreA_ForcedDelayAlgorithm, kstTrackerType_Enum_FXDelayAlgorithms, fnSettingsCallback_ForcedDelayAlgorithm);

	AudioConsole_AddSetting(L"FX Core B - Forced Mode"				, &m_stRuntimeSettings.iFXCoreB_ForcedMode			, kstTrackerType_Enum_FXModes);
	//AudioConsole_AddSetting(L"FX Core B - Forced Delay"				, &m_stRuntimeSettings.iFXCoreB_ForcedDelay			, 0, 1000, 5);
	//AudioConsole_AddSetting(L"FX Core B - Forced Feedback"			, &m_stRuntimeSettings.iFXCoreB_ForcedFeedback		, 0, 100, 5);
	AudioConsole_AddSetting(L"FX Core B - Forced Wet Volume"		, &m_stRuntimeSettings.fFXCoreB_ForcedWetVolume		, 0.0f, 1.0f , 0.1f, fnSettingsCallback_ForcedWetVolume);
	AudioConsole_AddSetting(L"FX Core B - Forced Wet Pan"			, &m_stRuntimeSettings.iFXCoreB_ForcedWetPan		, -10000, 10000 , 250);
	//AudioConsole_AddSetting(L"FX Core B - Forced Delay Algorithm"	, &m_stRuntimeSettings.iFXCoreB_ForcedDelayAlgorithm, kstTrackerType_Enum_FXDelayAlgorithms, fnSettingsCallback_ForcedDelayAlgorithm);
#endif
}

// ***********************************************************************************************************************

void CAudioEngine::Shutdown()
{
	m_bIsInited = FALSE;

	// Stop Playback

	Streams(eStreamOperation_Stop);

	// Close Header Database (If Applicable)

	if (m_pHeaderList != NULL)
	{
		delete m_pHeaderList;
		m_pHeaderList = NULL;
	}

	// Destroy FX Cores

#if _XENON_SOUND_FX
	for (int Loop = 0; Loop < SND_Cte_FxCoreNb; Loop++)
	{
		if (m_apFXCore[Loop] != NULL)
		{
			delete m_apFXCore[Loop];
			m_apFXCore[Loop] = NULL;
		}
	}
#endif

	// Destroy Wave Parser (If Applicable)

	if (m_pWaveParser != NULL)
	{
		delete m_pWaveParser;
		m_pWaveParser = NULL;
	}

	// Destroy Stream Reader (If Applicable)

	if (m_pStreamReader != NULL)
	{
		delete m_pStreamReader;
		m_pStreamReader = NULL;
	}

	// Destroy Prefetch List (If Applicable)

	if (m_pPrefetchList != NULL)
	{
		delete m_pPrefetchList;
		m_pPrefetchList = NULL;
	}

	// Destroy Voice List

	if (m_pVoiceList != NULL)
	{
		delete m_pVoiceList;
		m_pVoiceList = NULL;
	}

	// Destroy Thread

	if (m_pThread != NULL)
	{
		delete m_pThread;
		m_pThread = NULL;
	}

	// Destroy "Game" BigFile Handle

	if (m_pFile_BF != NULL)
	{
		delete m_pFile_BF;
		m_pFile_BF = NULL;
	}

	// Destroy "Sound" BigFile Handles

	for (int iLoop = 0; iLoop < eSoundBF_Count; iLoop++)
	{
		if (m_pSoundBF[iLoop] != NULL)
		{
			delete m_pSoundBF[iLoop];
			m_pSoundBF[iLoop] = NULL;
		}
	}

	// Shutdown XAudio

	XAudioShutDown();

	// Scan For Memory Leaks or Inconsistencies

	for (int Loop = 0; Loop < eMemoryTracker_Count; Loop++)
	{
		if ((m_aMemoryTrackers[Loop].iBlockCount != 0) || (m_aMemoryTrackers[Loop].iAllocatedSize != 0))
		{
#if defined(AUDIOCONSOLE_ENABLE)
			AudioConsole_WaitForMessageExclusion();
			AudioConsole_AddMessageParameter(m_aMemoryTrackers[Loop].iAllocatedSize, L"Size");
			AudioConsole_AddMessageParameter(m_aMemoryTrackers[Loop].iBlockCount, L"Blocks");
			AudioConsole_AddMessage(L"Shutdown - Leak Detected", kszModuleName_XenonSound, kstMessageType_Error);
#endif
		}
	}
}

// ***********************************************************************************************************************

void CAudioEngine::Tick()
{
	if (m_fReinitVolumeFactor < 1.0f)
	{
		m_fReinitVolumeFactor += TIM_gf_dt;

		if (m_fReinitVolumeFactor > 1.0f)
		{
			m_fReinitVolumeFactor = 1.0f;
		}
	}
}

// ***********************************************************************************************************************

void* CAudioEngine::MemoryAllocate(int Param_iSize, eMemoryTrackers Param_eTracker)
{
#if !defined(_FINAL_)
	// Error Checking

	if ((Param_iSize <= 0) || (Param_eTracker < 0) || (Param_eTracker >= eMemoryTracker_Count))
	{
#if defined(AUDIOCONSOLE_ENABLE)
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddMessageParameter(Param_iSize, L"Size");
		AudioConsole_AddMessageParameter((int)Param_eTracker, L"Tracker");
		AudioConsole_AddMessage(L"Allocate - Invalid Parameters", kszModuleName_XenonSound, kstMessageType_Error);
#endif // AUDIOCONSOLE_ENABLE
		return NULL;
	}
#endif // _FINAL_

	// Allocate Memory Block

	void* pPointer = MEM_p_Alloc(Param_iSize);

#if !defined(_FINAL_)
	if (pPointer == NULL)
	{
#if defined(AUDIOCONSOLE_ENABLE)
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddMessage(L"Allocate - Failed", kszModuleName_XenonSound, kstMessageType_Error);
#endif // AUDIOCONSOLE_ENABLE
		return NULL;
	}

	// Clear Block

	L_memset(pPointer, 0, Param_iSize);

	// Increase Counters

	m_aMemoryTrackers[Param_eTracker].iAllocatedSize += MEM_i_GetInternalSize(pPointer);
	m_aMemoryTrackers[Param_eTracker].iBlockCount++;
#endif // _FINAL_

	// Return Block Pointer

	return pPointer;
}

// ***********************************************************************************************************************

void CAudioEngine::MemoryFree(void* Param_pPointer, eMemoryTrackers Param_eTracker)
{
#if !defined(_FINAL_)
	// Error Checking

	if ((Param_pPointer == NULL) || (Param_eTracker < 0) || (Param_eTracker >= eMemoryTracker_Count))
	{
#if defined(AUDIOCONSOLE_ENABLE)
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddMessageParameter((int)Param_pPointer, L"Ptr");
		AudioConsole_AddMessageParameter((int)Param_eTracker, L"Tracker");
		AudioConsole_AddMessage(L"Free - Invalid Parameters", kszModuleName_XenonSound, kstMessageType_Error);
#endif // AUDIOCONSOLE_ENABLE
		return;
	}

	// Decrease Counters

	m_aMemoryTrackers[Param_eTracker].iAllocatedSize -= MEM_i_GetInternalSize(Param_pPointer);
	m_aMemoryTrackers[Param_eTracker].iBlockCount--;

	// Check Consistency

	if ((m_aMemoryTrackers[Param_eTracker].iAllocatedSize < 0) || (m_aMemoryTrackers[Param_eTracker].iBlockCount < 0))
	{
#if defined(AUDIOCONSOLE_ENABLE)
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddMessageParameter(m_aMemoryTrackers[Param_eTracker].iAllocatedSize, L"Size");
		AudioConsole_AddMessageParameter(m_aMemoryTrackers[Param_eTracker].iBlockCount, L"BlockS");
		AudioConsole_AddMessage(L"Free - Empty Pool", kszModuleName_XenonSound, kstMessageType_Error);
#endif // AUDIOCONSOLE_ENABLE
	}

	// Leak Check

	if ((m_aMemoryTrackers[Param_eTracker].iBlockCount == 0) && (m_aMemoryTrackers[Param_eTracker].iAllocatedSize > 0))
	{
#if defined(AUDIOCONSOLE_ENABLE)
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddMessageParameter(m_aMemoryTrackers[Param_eTracker].iAllocatedSize, L"Size");
		AudioConsole_AddMessage(L"Free - Leak Detected", kszModuleName_XenonSound, kstMessageType_Error);
#endif // AUDIOCONSOLE_ENABLE
	}
#endif // _FINAL_

	// Free Memory Block

	MEM_Free(Param_pPointer);
}

// ***********************************************************************************************************************

SND_tdst_SoundBuffer* CAudioEngine::CreateSoundBuffer(SND_tdst_WaveData* Param_pWaveData, int Param_iFilePositionInBF, int Param_iStreamID, CBuffer* Param_pBuffer_Playback)
{
	// Allocate Memory For Structure

	SND_tdst_SoundBuffer* pSoundBuffer = (SND_tdst_SoundBuffer*)MemoryAllocate(sizeof(SND_tdst_SoundBuffer), eMemoryTracker_General);

	// Create Voice Class

	if ((pSoundBuffer != NULL) && (m_pVoiceList != NULL))
	{
		pSoundBuffer->pVoice = m_pVoiceList->RequestVoice(Param_pWaveData, Param_iFilePositionInBF, Param_iStreamID, Param_pBuffer_Playback);
	}

	// Return Newly Created Struct

	return pSoundBuffer;
}

// ***********************************************************************************************************************

SND_tdst_SoundBuffer* CAudioEngine::DuplicateSoundBuffer(SND_tdst_SoundBuffer* Param_pSoundBuffer)
{
	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return NULL;
	}

	// Prepare Empty WaveData

	SND_tdst_WaveData stWaveData;

	stWaveData.i_Size		= 0;
	stWaveData.pbData		= NULL;
	stWaveData.st_WaveFmtx	= *Param_pSoundBuffer->pVoice->GetWaveFormat();

	// Create New Buffer

	SND_tdst_SoundBuffer* pSoundBuffer = CreateSoundBuffer(&stWaveData, Param_pSoundBuffer->pVoice->GetFileStartPosition(), Param_pSoundBuffer->pVoice->GetStreamID(), Param_pSoundBuffer->pVoice->GetPlaybackBuffer());

	if ((pSoundBuffer == NULL) || (pSoundBuffer->pVoice == NULL))
	{
		return NULL;
	}

	// Transfer Necessary Fields

	pSoundBuffer->pVoice->TransferToDuplicate(Param_pSoundBuffer->pVoice);

	// Return Newly Created Struct

	return pSoundBuffer;
}

// ***********************************************************************************************************************

void CAudioEngine::DestroySoundBuffer(SND_tdst_SoundBuffer* Param_pSoundBuffer)
{
	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return;
	}

	// Destroy Voice

	if (m_pVoiceList != NULL)
	{
		m_pVoiceList->DestroyVoice(Param_pSoundBuffer->pVoice);
	}

	// Free SoundBuffer Struct

	MemoryFree(Param_pSoundBuffer, eMemoryTracker_General);
}

// ***********************************************************************************************************************

void CAudioEngine::SoundFile_Read(void* Param_pTargetBuffer, int Param_iNbBytesToRead, int* Param_pNbBytesRead, unsigned int Param_uiFileOffset, TEventParameter Param_pParameter, eSoundBFIndex Param_eBFIndex, eEvents_AudioEngine Param_eEvent)
{
	// Fetch Sound File

	CFile* pFile = SoundFile_Fetch(Param_eBFIndex);

	if (pFile == NULL)
	{
		return;
	}

	// Read From File

	if (Param_eEvent == eEventID_None)
	{
		// Error Checking

		if (Param_pNbBytesRead == NULL)
		{
			return;
		}

		// Seek To Proper Position

		unsigned int uiHigh64 = 0;

		SetFilePointer(pFile->GetHandle_Sync(), Param_uiFileOffset, (PLONG)&uiHigh64, FILE_BEGIN);

		// Synchronous Read

		ReadFile(pFile->GetHandle_Sync(), Param_pTargetBuffer, Param_iNbBytesToRead, (LPDWORD)Param_pNbBytesRead, NULL);
	}
	else
	{
		// Error Checking

		if (m_pThread == NULL)
		{
			return;
		}

		// Request Asynchronous Read From Thread

		m_pThread->FileRead(Param_eEvent, pFile->GetHandle_Async(), Param_pTargetBuffer, Param_iNbBytesToRead, Param_pNbBytesRead, Param_uiFileOffset, Param_pParameter, FALSE);
	}
}

// ***********************************************************************************************************************

CFile* CAudioEngine::SoundFile_Fetch(eSoundBFIndex Param_eBFIndex)
{
	// Error Checking

	if ((Param_eBFIndex < eSoundBF_None) || (Param_eBFIndex >= eSoundBF_Count))
	{
		return NULL;
	}

	// Return Game BF (If Applicable)

	if (Param_eBFIndex == eSoundBF_None)
	{
		if ((m_pFile_BF != NULL) && m_pFile_BF->IsOpen())
		{
			return m_pFile_BF;
		}
		else
		{
			return NULL;
		}
	}

	// Return Appropriate Sound BF (If Applicable)

	if ((m_pSoundBF[Param_eBFIndex] != NULL) && m_pSoundBF[Param_eBFIndex]->IsOpen())
	{
		return m_pSoundBF[Param_eBFIndex]->GetFile();
	}

	return NULL;
}

// ***********************************************************************************************************************

CBigFileReader* CAudioEngine::SoundFile_GetBF(eSoundBFIndex Param_eBFIndex)
{
	// Error Checking

	if ((Param_eBFIndex <= eSoundBF_None) || (Param_eBFIndex >= eSoundBF_Count))
	{
		return NULL;
	}

	// Return Appropriate Sound BF (If Applicable)

	if ((m_pSoundBF[Param_eBFIndex] != NULL) && m_pSoundBF[Param_eBFIndex]->IsOpen())
	{
		return m_pSoundBF[Param_eBFIndex];
	}

	return NULL;
}

// ***********************************************************************************************************************

unsigned int CAudioEngine::SoundFile_FindKey(unsigned int Param_uiKey, eSoundBFIndex* Param_peBFIndex)
{
	// Error Checking

	if (Param_peBFIndex == NULL)
	{
		*Param_peBFIndex = eSoundBF_None;
		return 0xFFFFFFFF;
	}

	// Scan In Sound BFs

	for (int iLoop = 0; iLoop < eSoundBF_Count; iLoop++)
	{
		if ((m_pSoundBF[iLoop] != NULL) && m_pSoundBF[iLoop]->IsOpen())
		{
			unsigned int uiPosition = m_pSoundBF[iLoop]->FindPositionFromKey(Param_uiKey);

			if (uiPosition != 0xFFFFFFFF)
			{
				*Param_peBFIndex = (eSoundBFIndex)iLoop;
				return uiPosition;
			}
		}
	}

	// Return "Game" BF

	*Param_peBFIndex = eSoundBF_None;
	return 0xFFFFFFFF;
}

// ***********************************************************************************************************************

void CAudioEngine::RequestEvent(eEvents_AudioEngine Param_eEventID, void* Param_pParameter)
{
	// Error Checking

	if (m_pThread == NULL) return;

	// Request Event From Thread

	m_pThread->RequestEvent(Param_eEventID, Param_pParameter, FALSE);
}

// ***********************************************************************************************************************

void CAudioEngine::SetGlobalVolume(int Param_iVolume)
{
	// Update Global Volume Value

	m_iGlobalVolume = Param_iVolume;

	if (m_iGlobalVolume < kiMinimumGlobalVolume)
	{
		m_iGlobalVolume = kiMinimumGlobalVolume;
	}

	// Update Volume For All Voices

	int ListIndex = m_pVoiceList->GetItemCount();

	while (ListIndex > 0)
	{
		ListIndex--;

		CVoice* pVoice = m_pVoiceList->GetVoice(ListIndex);

		if (pVoice != NULL)
		{
			pVoice->SetVolume(pVoice->GetVolume());
		}
	}
}

// ***********************************************************************************************************************

int CAudioEngine::GetGlobalVolume()
{
	return m_iGlobalVolume;
}

// ***********************************************************************************************************************

int CAudioEngine::RequestStreamID()
{
	return m_iStreamCount++;
}

// ***********************************************************************************************************************

stRuntimeSettings* CAudioEngine::GetRuntimeSettings()
{
	return &m_stRuntimeSettings;
}

// ***********************************************************************************************************************

stMemoryTracker* CAudioEngine::GetMemoryTracker(eMemoryTrackers Param_eTracker)
{
	// Error Checking

	if ((Param_eTracker < 0) || (Param_eTracker >= eMemoryTracker_Count))
	{
		return NULL;
	}

	return &m_aMemoryTrackers[Param_eTracker];
}

// ***********************************************************************************************************************

CFXCore* CAudioEngine::GetFXCore(int Param_iCoreID)
{
#if !_XENON_SOUND_FX
	return NULL;
#else
	// Error Checking

	if ((Param_iCoreID < 0) || (Param_iCoreID >= SND_Cte_FxCoreNb))
	{
		return NULL;
	}

	return m_apFXCore[Param_iCoreID];
#endif
}

// ***********************************************************************************************************************

void CAudioEngine::Streams(eStreamOperation Param_eOperation)
{
	// Error Checking

	if ((m_pVoiceList == NULL) || m_pVoiceList->IsEmpty())
	{
		return;
	}

	// Find All Stream Voices

	int ListIndex = m_pVoiceList->GetItemCount();

	while (ListIndex > 0)
	{
		ListIndex--;

		CVoice* pVoice = m_pVoiceList->GetVoice(ListIndex);

		if ((pVoice != NULL) && pVoice->IsAStream())
		{
			switch (Param_eOperation)
			{
				case eStreamOperation_Stop:
				{
					if (pVoice->IsPlaying())
					{
						pVoice->Stop();
					}
					break;
				}

				case eStreamOperation_Pause:
				{
					if (pVoice->IsPlaying())
					{
						pVoice->Pause();
					}
					break;
				}

				case eStreamOperation_Resume:
				{
					if (pVoice->IsPaused())
					{
						pVoice->Pause();
					}
					break;
				}
			}
		}
	}
}

// ***********************************************************************************************************************

CVoice* CAudioEngine::GetVoice(int Param_iListIndex)
{
	// Error Checking

	if (m_pVoiceList == NULL)
	{
		return NULL;
	}

	return m_pVoiceList->GetVoice(Param_iListIndex);
}

// ***********************************************************************************************************************

BOOL CAudioEngine::IsVoicePointerValid(CVoice* Param_pVoice)
{
	return (m_pVoiceList->GetItemIndex(Param_pVoice) >= 0);
}

// ***********************************************************************************************************************

int CAudioEngine::GetVoiceCount()
{
	// Error Checking

	if (m_pVoiceList == NULL)
	{
		return 0;
	}

	return m_pVoiceList->GetItemCount();
}

// ***********************************************************************************************************************

void CAudioEngine::SetVoiceInit_Key(int Param_iKey, char Param_cSoundBFIndex)
{
	m_iVoiceInit_Key		= Param_iKey;
	m_eVoiceInit_SoundBF	= (eSoundBFIndex)Param_cSoundBFIndex;
}

// ***********************************************************************************************************************

int CAudioEngine::GetVoiceInit_Key()
{
	return m_iVoiceInit_Key;
}

// ***********************************************************************************************************************

eSoundBFIndex CAudioEngine::GetVoiceInit_SoundBF()
{
	return m_eVoiceInit_SoundBF;
}

// ***********************************************************************************************************************

void CAudioEngine::Profile(const TPString Param_pString)
{
#if defined(PROFILE) || defined(_DEBUG)
	if (Param_pString != NULL)
	{
		PIXBeginNamedEvent(0xFFA040, "xeSND:%s", Param_pString);
	}
	else
	{
		PIXEndNamedEvent();
	}
#endif
}
// ***********************************************************************************************************************

CStreamReader* CAudioEngine::GetStreamReader()
{
	// Instantiate Stream Reader (If Applicable)

	if (m_pStreamReader == NULL)
	{
		m_pStreamReader = new CStreamReader(kiStreamBufferSizeKB);
	}

	return m_pStreamReader;
}

// ***********************************************************************************************************************

CWaveParser* CAudioEngine::GetWaveParser()
{
	// Instantiate Wave Parser (If Applicable)

	if (m_pWaveParser == NULL)
	{
		m_pWaveParser = new CWaveParser();
	}

	return m_pWaveParser;
}

// ***********************************************************************************************************************

CPrefetchList* CAudioEngine::GetPrefetchList()
{
	// Instantiate Prefetch List (If Applicable)

	if (m_pPrefetchList == NULL)
	{
		m_pPrefetchList = new CPrefetchList();
	}

	return m_pPrefetchList;
}

// ***********************************************************************************************************************

CHeaderList* CAudioEngine::GetHeaderList()
{
	// Instantiate Header List (If Applicable)

	if (m_pHeaderList == NULL)
	{
		m_pHeaderList = new CHeaderList();
	}

	return m_pHeaderList;
}

// ***********************************************************************************************************************

char* CAudioEngine::GetSoundBFName(eSoundBFIndex Param_eSoundBFIndex)
{
	// Determine Name By Index

	switch (Param_eSoundBFIndex)
	{
		default:
		case eSoundBF_None:
		{
			return "";
		}

		case eSoundBF_Extra:
		{
			return "Extra";
		}

		case eSoundBF_Common:
		{
			return "Common";
		}

		case eSoundBF_Localized:
		{
			switch (TEXT_i_GetLang())
			{
				default:
				case INO_e_English:
				{
					return "Local_English";
				}

				case INO_e_French:
				{
					return "Local_French";
				}

				case INO_e_Spanish:
				{
					return "Local_Spanish";
				}

				case INO_e_German:
				{
					return "Local_German";
				}

				case INO_e_Italian:
				{
					return "Local_Italian";
				}

				case INO_e_Dutch:
				{
					return "Local_Dutch";
				}

				case INO_e_Japanese:
				{
					return "Local_Japanese";
				}
			}
		}
	}
}

// ***********************************************************************************************************************

float CAudioEngine::GetReinitVolumeFactor()
{
	return m_fReinitVolumeFactor;
}

// ***********************************************************************************************************************

void CAudioEngine::ResetReinitVolume()
{
	m_fReinitVolumeFactor = 0.0f;
}

// ***********************************************************************************************************************
//    Thread Callbacks
// ***********************************************************************************************************************

void CALLBACK fnThreadEvent_PacketLoadDone(int Param_iEventIndex, void* Param_pParameter)
{
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter(Param_iEventIndex, L"EventID");
	AudioConsole_AddEventParameter((int)Param_pParameter, L"Ptr", TRUE);
	AudioConsole_AddEvent(L"Th - Packet Load Done", kszModuleName_XenonSound, kstEventType_Update);
#endif

	// Error Checking

	if ((Param_pParameter == NULL) || !GetAudioEngine()->IsInited())
	{
		return;
	}

	if (!GetAudioEngine()->IsVoicePointerValid((CVoice*)Param_pParameter))
	{
		return;
	}

	// Notify Voice

	((CVoice*)Param_pParameter)->Packet_LoadDone();
}

// ***********************************************************************************************************************

void CALLBACK fnThreadEvent_StreamPlay(int Param_iEventIndex, void* Param_pParameter)
{
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter(Param_iEventIndex, L"EventID");
	AudioConsole_AddEventParameter((int)Param_pParameter, L"Ptr", TRUE);
	AudioConsole_AddEvent(L"Th - Stream Play", kszModuleName_XenonSound, kstEventType_Start);
#endif

	// Error Checking

	if ((Param_pParameter == NULL) || !GetAudioEngine()->IsInited()) return;

	((CVoice*)Param_pParameter)->Play();
}

// ***********************************************************************************************************************

void CALLBACK fnThreadEvent_PrefetchLoadDone(int Param_iEventIndex, void* Param_pParameter)
{
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter(Param_iEventIndex, L"EventID");
	AudioConsole_AddEventParameter((int)Param_pParameter, L"Ptr", TRUE);
	AudioConsole_AddEvent(L"Th - PrefetchDone", kszModuleName_XenonSound, kstEventType_Update);
#endif

	// Error Checking

	if (!GetAudioEngine()->IsInited()) return;

	// Fetch List Index (Passed As Parameter)

	int iListIndex = (unsigned int)Param_pParameter;

	// Fetch Prefetch List

	CPrefetchList* pPrefetchList = GetAudioEngine()->GetPrefetchList();

	// Get Prefetch Buffer

	CPrefetchBuffer* pBuffer = (CPrefetchBuffer*)pPrefetchList->GetItem(iListIndex);

	// Notify Load Complete

	if (pBuffer != NULL)
	{
		pBuffer->EndLoad();
	}
}

// ***********************************************************************************************************************

void CALLBACK fnXAudio_PacketCallback(LPCXAUDIOVOICEPACKETCALLBACK Param_pCallbackData)
{
	// Error Checking

	if ((Param_pCallbackData == NULL) || (Param_pCallbackData->pVoiceContext == NULL))
	{
		return;
	}

	// Notify Voice

	((CVoice*)Param_pCallbackData->pVoiceContext)->Packet_EndNotify();
}

// ***********************************************************************************************************************

void CALLBACK fnSettingsCallback_ForcedWetVolume(TFloat* Param_pLinkedVariable, TFloat Param_fOldValue, TListID Param_iOptionID)
{
	// Detect & Update Core

	if (&(GetAudioEngine()->GetRuntimeSettings()->fFXCoreA_ForcedWetVolume) == Param_pLinkedVariable)
	{
		GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->SetWetVolume(*Param_pLinkedVariable);
	}
	else
	{
		GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->SetWetVolume(*Param_pLinkedVariable);
	}
}

// ***********************************************************************************************************************
/*
void CALLBACK fnSettingsCallback_ForcedDelayAlgorithm(void* Param_pLinkedVariable, TTrackerType Param_iType, TListID Param_iOptionID)
{
	// Detect & Update Core

	if (&(GetAudioEngine()->GetRuntimeSettings()->iFXCoreA_ForcedDelayAlgorithm) == Param_pLinkedVariable)
	{
		GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->SetDelayAlgorithm(*((eDelayAlgorithm*)Param_pLinkedVariable));
	}
	else
	{
		GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->SetDelayAlgorithm(*((eDelayAlgorithm*)Param_pLinkedVariable));
	}
}
*/
// ***********************************************************************************************************************

#endif // _XENON

// ToDo:

//Prefetch
//Support XMA

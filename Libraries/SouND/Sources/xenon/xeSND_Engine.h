#ifndef __xeSND_Engine_h__
#define __xeSND_Engine_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "SouND/Sources/SND.h"

#include "xeSND_Helpers.h"
#include "xeSND_Database.h"
#include "xeSND_Voice.h"
#include "xeSND_Effects.h"
#include "xeSND_WaveParser.h"
#include "xeSND_BigFile.h"

#include "Xenon/EventThread/EventThread.h"

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

#define kiMinimumGlobalVolume	-5000
#define kfPitch_Maximum			10.0f
#define kfPitch_Minimum			-10.0f
#define kiThread_StackSizeKB	128
#define kiStreamID_None			-1

// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************

typedef enum
{
	eEventID_None = -1,
	eEventID_PacketLoadDone,
	eEventID_StreamPlay,
	eEventID_PrefetchLoadDone,

	eEventID_Count
}
eEvents_AudioEngine;

// ***********************************************************************************************************************

typedef enum
{
	eStreamOperation_Stop,
	eStreamOperation_Pause,
	eStreamOperation_Resume,

	eStreamOperation_Count
}
eStreamOperation;

// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************

typedef struct
{
	TBool	bAllowStaticPlay;
	TBool	bAllowStreamPlay;
	TBool	bAllowPitching;
	TBool	bAllowVolumeChange;
	TBool	bAllowFX;
	TInt	iForcePlayToFXCore;
	TInt	iFXCoreA_ForcedMode;
	//TInt	iFXCoreA_ForcedDelay;
	//TInt	iFXCoreA_ForcedFeedback;
	TFloat	fFXCoreA_ForcedWetVolume;
	TInt	iFXCoreA_ForcedWetPan;
	//TInt	iFXCoreA_ForcedDelayAlgorithm;
	TInt	iFXCoreB_ForcedMode;
	//TInt	iFXCoreB_ForcedDelay;
	//TInt	iFXCoreB_ForcedFeedback;
	TFloat	fFXCoreB_ForcedWetVolume;
	TInt	iFXCoreB_ForcedWetPan;
	//TInt	iFXCoreB_ForcedDelayAlgorithm;
}
stRuntimeSettings;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CAudioEngine
{
private:
	BOOL					m_bIsInited;
	CVoiceList*				m_pVoiceList;
	stMemoryTracker			m_aMemoryTrackers[eMemoryTracker_Count];
	stRuntimeSettings		m_stRuntimeSettings;
	CEventThread*			m_pThread;
	CFile*					m_pFile_BF;
	CBigFileReader*			m_pSoundBF[eSoundBF_Count];
	int						m_iGlobalVolume;
	float					m_fReinitVolumeFactor;
	int						m_iStreamCount;
	int						m_iVoiceInit_Key;
	eSoundBFIndex			m_eVoiceInit_SoundBF;
	CStreamReader*			m_pStreamReader;
	CWaveParser*			m_pWaveParser;
	CPrefetchList*			m_pPrefetchList;
	CHeaderList*			m_pHeaderList;
#if _XENON_SOUND_FX
	CFXCore*				m_apFXCore[SND_Cte_FxCoreNb];
#endif

public:
    M_DeclareOperatorNewAndDelete();

	CAudioEngine::CAudioEngine();
	CAudioEngine::~CAudioEngine();

	BOOL					IsInited();
	void					Initialize();
	void					Shutdown();
	void					Tick();
	void*					MemoryAllocate(int Param_iSize, eMemoryTrackers Param_eTracker);
	void					MemoryFree(void* Param_pPointer, eMemoryTrackers Param_eTracker);
	SND_tdst_SoundBuffer*	CreateSoundBuffer(SND_tdst_WaveData* Param_pWaveData = NULL, int Param_iFilePositionInBF = 0, int Param_iStreamID = kiStreamID_None, CBuffer* Param_pBuffer_Playback = NULL);
	SND_tdst_SoundBuffer*	DuplicateSoundBuffer(SND_tdst_SoundBuffer* Param_pSoundBuffer);
	void					DestroySoundBuffer(SND_tdst_SoundBuffer* Param_pSoundBuffer);
	void					SoundFile_Read(void* Param_pTargetBuffer, int Param_iNbBytesToRead, int* Param_pNbBytesRead, unsigned int Param_uiFileOffset, TEventParameter Param_pParameter, eSoundBFIndex Param_eBFIndex, eEvents_AudioEngine Param_eEvent = eEventID_None);
	CFile*					SoundFile_Fetch(eSoundBFIndex Param_eBFIndex);
	CBigFileReader*			SoundFile_GetBF(eSoundBFIndex Param_eBFIndex);
	unsigned int			SoundFile_FindKey(unsigned int Param_uiKey, eSoundBFIndex* Param_peBFIndex);
	void					RequestEvent(eEvents_AudioEngine Param_eEventID, void* Param_pParameter);
	void					SetGlobalVolume(int Param_iVolume);
	int						GetGlobalVolume();
	int						RequestStreamID();
	stRuntimeSettings*		GetRuntimeSettings();
	stMemoryTracker*		GetMemoryTracker(eMemoryTrackers Param_eTracker);
	CFXCore*				GetFXCore(int Param_iCoreID);
	void					Streams(eStreamOperation Param_eOperation);
	CVoice*					GetVoice(int Param_iListIndex);
	BOOL					IsVoicePointerValid(CVoice* Param_pVoice);
	int						GetVoiceCount();
	void					SetVoiceInit_Key(int Param_iKey, char Param_cSoundBFIndex);
	int						GetVoiceInit_Key();
	eSoundBFIndex			GetVoiceInit_SoundBF();
	void					Profile(const TPString Param_pString = NULL);
	CStreamReader*			GetStreamReader();
	CWaveParser*			GetWaveParser();
	CPrefetchList*			GetPrefetchList();
	CHeaderList*			GetHeaderList();
	char*					GetSoundBFName(eSoundBFIndex Param_eSoundBFIndex);
	float					GetReinitVolumeFactor();
	void					ResetReinitVolume();
};

// ***********************************************************************************************************************
//    Thread Callbacks
// ***********************************************************************************************************************

void CALLBACK fnThreadEvent_PacketLoadDone(int Param_iEventIndex, void* Param_pParameter);
void CALLBACK fnThreadEvent_StreamPlay(int Param_iEventIndex, void* Param_pParameter);
void CALLBACK fnThreadEvent_PrefetchLoadDone(int Param_iEventIndex, void* Param_pParameter);

void CALLBACK fnXAudio_PacketCallback(LPCXAUDIOVOICEPACKETCALLBACK Param_pCallbackData);

void CALLBACK fnSettingsCallback_ForcedWetVolume(TFloat* Param_pLinkedVariable, TFloat Param_fOldValue, TListID Param_iOptionID);
//void CALLBACK fnSettingsCallback_ForcedDelayAlgorithm(void* Param_pLinkedVariable, TTrackerType Param_iType, TListID Param_iOptionID);

// ***********************************************************************************************************************
//    Globals
// ***********************************************************************************************************************

extern CAudioEngine* g_pAudioEngine;

// ***********************************************************************************************************************
//    Engine Accessor
// ***********************************************************************************************************************

#define GetAudioEngine() g_pAudioEngine

// ***********************************************************************************************************************

#endif // __xeSND_Engine_h__

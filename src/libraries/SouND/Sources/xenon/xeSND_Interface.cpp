#include "Precomp.h"

#ifdef _XENON

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDvolume.h"

#include "xeSND_Engine.h"
#include "xeSND_Voice.h"

// ***********************************************************************************************************************
//    External Interface - Standard
// ***********************************************************************************************************************

int xeSND_lInit(SND_tdst_TargetSpecificData* Param_pSpecificData)
{
#if !_XENON_SOUND_ENGINE
	return FALSE;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEvent(L"Init", kszModuleName_XenonSound, kstEventType_Init);
#endif

	// Initialize Audio Engine

	g_pAudioEngine = new CAudioEngine();

	GetAudioEngine()->Initialize();

	return TRUE;
#endif
}

// ***********************************************************************************************************************

void xeSND_Close(SND_tdst_TargetSpecificData* Param_pSpecificData)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEvent(L"Close", kszModuleName_XenonSound, kstEventType_Shutdown);
#endif

	// Shutdown Audio Engine

	GetAudioEngine()->Shutdown();

	delete g_pAudioEngine;
	g_pAudioEngine = NULL;
#endif
}

// ***********************************************************************************************************************

SND_tdst_SoundBuffer* xeSND_pSndBuffCreate(SND_tdst_TargetSpecificData* Param_pSpecificData, SND_tdst_WaveData* Param_pWaveData, unsigned int Param_uiFilePositionInBF)
{
#if !_XENON_SOUND_ENGINE
	return NULL;
#else
	// Error Checking

	if ((Param_pWaveData == NULL) || (Param_pWaveData->i_Size <= 0))
	{
		return NULL;
	}

	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((Param_pWaveData->st_WaveFmtx.nChannels == 1) ? L"Mono" : L"Stereo");
	AudioConsole_AddEventParameter(Param_pWaveData->st_WaveFmtx.wBitsPerSample);
	AudioConsole_AddEventParameter((int)Param_pWaveData->st_WaveFmtx.nSamplesPerSec);
	AudioConsole_AddEventParameter(Param_pWaveData->i_Size, L"Size");
	AudioConsole_AddEventParameter((int)Param_uiFilePositionInBF, L"Pos", TRUE);
	AudioConsole_AddEvent(L"Buf - Create", kszModuleName_XenonSound, kstEventType_Create);
#endif

	// Create Sound Buffer

	return GetAudioEngine()->CreateSoundBuffer(Param_pWaveData, Param_uiFilePositionInBF);
#endif
}

// ***********************************************************************************************************************

SND_tdst_SoundBuffer* xeSND_pSndBuffReLoad(SND_tdst_TargetSpecificData* Param_pSpecificData, SND_tdst_SoundBuffer* Param_pSoundBuffer, SND_tdst_WaveData* Param_pWaveData, unsigned int Param_uiFilePositionInBF)
{
#if !_XENON_SOUND_ENGINE
	return NULL;
#else
	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL) || (Param_pWaveData == NULL))
	{
		return NULL;
	}

	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
	AudioConsole_AddEventParameter((Param_pWaveData->st_WaveFmtx.nChannels == 1) ? L"Mono" : L"Stereo");
	AudioConsole_AddEventParameter(Param_pWaveData->st_WaveFmtx.wBitsPerSample);
	AudioConsole_AddEventParameter((int)Param_pWaveData->st_WaveFmtx.nSamplesPerSec);
	AudioConsole_AddEventParameter(Param_pWaveData->i_Size, L"Size");
	AudioConsole_AddEventParameter((int)Param_uiFilePositionInBF, L"\nPos", TRUE);
	AudioConsole_AddEvent(L"Buf - ReLoad", kszModuleName_XenonSound, kstEventType_Update);
#endif

	// Load New Static Sound Data

	Param_pSoundBuffer->pVoice->Setup_Static(Param_pWaveData, Param_uiFilePositionInBF);

	return Param_pSoundBuffer;
#endif
}

// ***********************************************************************************************************************

int xeSND_iSndBuffPlay(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iFlags, int Param_iFXVolume_Left, int Param_iFXVolume_Right)
{
#if !_XENON_SOUND_ENGINE
	return -1;
#else
	// Ensure Enabled

	if (!GetAudioEngine()->GetRuntimeSettings()->bAllowStaticPlay)
	{
		return -1;
	}

	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
	AudioConsole_AddEventParameter(Param_iFlags, L"Flag", TRUE);
	AudioConsole_AddEventParameter(Param_iFXVolume_Left, L"FxL");
	AudioConsole_AddEventParameter(Param_iFXVolume_Right, L"FxR");
	AudioConsole_AddEvent(L"Buf - Play", kszModuleName_XenonSound, kstEventType_Start);
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return -1;
	}

	// Start Static Playback

	Param_pSoundBuffer->pVoice->Play(Param_iFlags, Param_iFXVolume_Left, Param_iFXVolume_Right);

	return S_OK;
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffStop(SND_tdst_SoundBuffer* Param_pSoundBuffer)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	if (AudioConsole_GetConfiguration(eConfiguration_LogEvent_BufferStop))
	{
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
		AudioConsole_AddEvent(L"Buf - Stop", kszModuleName_XenonSound, kstEventType_Stop);
	}
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return;
	}

	// Stop Static Playback

	Param_pSoundBuffer->pVoice->Stop();
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffPause(SND_tdst_SoundBuffer* Param_pSoundBuffer)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
	AudioConsole_AddEvent(L"Buf - Pause", kszModuleName_XenonSound, kstEventType_Stop);
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return;
	}

	// Pause / Unpause

	Param_pSoundBuffer->pVoice->Pause();
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffRelease(SND_tdst_SoundBuffer* Param_pSoundBuffer)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
	AudioConsole_AddEvent(L"Buf - Release", kszModuleName_XenonSound, kstEventType_Destroy);
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return;
	}

	// Destroy Sound Buffer

	GetAudioEngine()->DestroySoundBuffer(Param_pSoundBuffer);
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffDuplicate(SND_tdst_TargetSpecificData* Param_pSpecificData, SND_tdst_SoundBuffer* Param_pSoundBuffer_Source, SND_tdst_SoundBuffer** Param_ppSoundBuffer_Dest)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Clear Output

	if (Param_ppSoundBuffer_Dest != NULL)
	{
		*Param_ppSoundBuffer_Dest = NULL;
	}

	// Ensure Enable

	if (!GetAudioEngine()->GetRuntimeSettings()->bAllowStaticPlay)
	{
		return;
	}

	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_pSoundBuffer_Source, L"Ptr", TRUE);
	AudioConsole_AddEvent(L"Buf - Duplicate", kszModuleName_XenonSound, kstEventType_Create);
#endif

	// Error Checking

	if ((Param_pSoundBuffer_Source == NULL) || (Param_pSoundBuffer_Source->pVoice == NULL) || (Param_ppSoundBuffer_Dest == NULL))
	{
		return;
	}

	// Create New Identical Sound Buffer (With Shared Memory Buffer)

	*Param_ppSoundBuffer_Dest = GetAudioEngine()->DuplicateSoundBuffer(Param_pSoundBuffer_Source);
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffSetPos(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iPosition)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	if (AudioConsole_GetConfiguration(eConfiguration_LogEvent_BufferSetCurrentPos))
	{
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
		AudioConsole_AddEventParameter(Param_iPosition, L"Pos");
		AudioConsole_AddEvent(L"Buf - SetCurrPos", kszModuleName_XenonSound, kstEventType_Update);
	}
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return;
	}

	// Force New Position

	Param_pSoundBuffer->pVoice->SetCursorPosition_Native(Param_iPosition);
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffSetVol(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iVolume)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Ensure Enabled

	if (!GetAudioEngine()->GetRuntimeSettings()->bAllowVolumeChange)
	{
		return;
	}

	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	if (AudioConsole_GetConfiguration(eConfiguration_LogEvent_BufferSetVolume))
	{
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
		AudioConsole_AddEventParameter(Param_iVolume);
		AudioConsole_AddEvent(L"Buf - SetVolume", kszModuleName_XenonSound, kstEventType_Update);
	}
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return;
	}

	// Update Local Volume

	Param_pSoundBuffer->pVoice->SetVolume(Param_iVolume);
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffSetFreq(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iFrequency)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Ensure Enabled

	if (!GetAudioEngine()->GetRuntimeSettings()->bAllowPitching)
	{
		return;
	}

	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	if (AudioConsole_GetConfiguration(eConfiguration_LogEvent_BufferSetFrequency))
	{
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
		AudioConsole_AddEventParameter(Param_iFrequency);
		AudioConsole_AddEvent(L"Buf - SetFrequency", kszModuleName_XenonSound, kstEventType_Update);
	}
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return;
	}

	// Update Frequency

	Param_pSoundBuffer->pVoice->SetFrequency(Param_iFrequency);
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffSetPan(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iPan, int Param_iSurroundPan)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	if (AudioConsole_GetConfiguration(eConfiguration_LogEvent_BufferSetPan))
	{
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
		AudioConsole_AddEventParameter(Param_iPan, L"Pan");
		AudioConsole_AddEventParameter(Param_iSurroundPan, L"SPan");
		AudioConsole_AddEvent(L"Buf - SetPan", kszModuleName_XenonSound, kstEventType_Update);
	}
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return;
	}

	// Fetch Pan & Surround Pan

	Param_pSoundBuffer->pVoice->SetPan(Param_iPan, Param_iSurroundPan);
#endif
}

// ***********************************************************************************************************************

int xeSND_iSndBuffGetStatus(SND_tdst_SoundBuffer* Param_pSoundBuffer, int* Param_pStatus)
{
#if !_XENON_SOUND_ENGINE
	return -1;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	if (AudioConsole_GetConfiguration(eConfiguration_LogEvent_BufferGetStatus))
	{
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
		AudioConsole_AddEvent(L"Buf - GetStatus", kszModuleName_XenonSound, kstEventType_Update);
	}
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL) || (Param_pStatus == NULL))
	{
		return -1;
	}

	// Compute Status Flags

	*Param_pStatus = 0;

	if (Param_pSoundBuffer->pVoice->IsPlaying() || Param_pSoundBuffer->pVoice->IsSetupping())
	{
		*Param_pStatus |= SND_Cul_SBS_Playing;

		if (Param_pSoundBuffer->pVoice->IsAStream())
		{
			*Param_pStatus |= SND_Cul_SF_StreamIsPlayingNow;
		}

		if (Param_pSoundBuffer->pVoice->IsLooping())
		{
			*Param_pStatus |= SND_Cul_SBS_PlayingLoop;
		}
	}

	return S_OK;
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffGetFreq(SND_tdst_SoundBuffer* Param_pSoundBuffer, int* Param_pFrequency)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL) || (Param_pFrequency == NULL))
	{
		return;
	}

	// Fetch Frequency

	*Param_pFrequency = Param_pSoundBuffer->pVoice->GetFrequency();
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffGetPan(SND_tdst_SoundBuffer* Param_pSoundBuffer, int* Param_pPan, int* Param_pSurroundPan)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return;
	}

	// Fetch Pan Value (If Applicable)

	if (Param_pPan != NULL)
	{
		*Param_pPan = Param_pSoundBuffer->pVoice->GetPan();
	}

	// Fetch Surround Pan Value (If Applicable)

	if (Param_pSurroundPan != NULL)
	{
		*Param_pSurroundPan = Param_pSoundBuffer->pVoice->GetSurroundPan();
	}
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffGetPos(SND_tdst_SoundBuffer* Param_pSoundBuffer, int* Param_pPosition_Read, int* Param_pPosition_Write)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL) || (Param_pPosition_Read == NULL))
	{
		return;
	}

	// Fetch Read Position

	*Param_pPosition_Read = Param_pSoundBuffer->pVoice->GetCursorPosition_Native();

	// Use Read Position For Write (If Applicable)

	if (Param_pPosition_Write != NULL)
	{
		*Param_pPosition_Write = *Param_pPosition_Read;
	}
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffCreateTrack(SND_tdst_SoundBuffer** Param_ppSoundBuffer_Mono, SND_tdst_SoundBuffer** Param_ppSoundBuffer_Stereo)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_ppSoundBuffer_Mono, L"pMonSB", TRUE);
	AudioConsole_AddEventParameter((int)Param_ppSoundBuffer_Stereo, L"pSteSB", TRUE);
	AudioConsole_AddEvent(L"CreateTrk", kszModuleName_XenonSound, kstEventType_Create);
#endif

	// Prepare Wave Data For Mono

	SND_tdst_WaveData stWaveData;

	stWaveData.i_Size = SND_Cte_MaxBufferSize;
	stWaveData.pbData = NULL;

	stWaveData.st_WaveFmtx.cbSize			= 0;
	stWaveData.st_WaveFmtx.wFormatTag		= WAVE_FORMAT_PCM;
	stWaveData.st_WaveFmtx.nSamplesPerSec	= 48000;
	stWaveData.st_WaveFmtx.wBitsPerSample	= 16;

	stWaveData.st_WaveFmtx.nChannels		= 1;
	stWaveData.st_WaveFmtx.nBlockAlign		= stWaveData.st_WaveFmtx.nChannels * sizeof(short);
	stWaveData.st_WaveFmtx.nAvgBytesPerSec	= stWaveData.st_WaveFmtx.nBlockAlign * stWaveData.st_WaveFmtx.nSamplesPerSec;

	// Create Mono Buffer

	if (Param_ppSoundBuffer_Mono != NULL)
	{
		*Param_ppSoundBuffer_Mono = GetAudioEngine()->CreateSoundBuffer(&stWaveData, 0, GetAudioEngine()->RequestStreamID());
	}

	// Update Wave Data For Stereo

	stWaveData.st_WaveFmtx.nChannels		= 2;
	stWaveData.st_WaveFmtx.nBlockAlign		= stWaveData.st_WaveFmtx.nChannels * sizeof(short);
	stWaveData.st_WaveFmtx.nAvgBytesPerSec	= stWaveData.st_WaveFmtx.nBlockAlign * stWaveData.st_WaveFmtx.nSamplesPerSec;

	// Create Stereo Buffer (Sharing Memory Buffer With Mono)

	if (Param_ppSoundBuffer_Stereo != NULL)
	{
		*Param_ppSoundBuffer_Stereo = GetAudioEngine()->CreateSoundBuffer(&stWaveData, 0, GetAudioEngine()->RequestStreamID(), (*Param_ppSoundBuffer_Mono)->pVoice->GetPlaybackBuffer());
	}
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffReleaseTrack(SND_tdst_SoundBuffer* Param_pSoundBuffer_Mono, SND_tdst_SoundBuffer* Param_pSoundBuffer_Stereo)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_pSoundBuffer_Mono, L"MonSB", TRUE);
	AudioConsole_AddEventParameter((int)Param_pSoundBuffer_Stereo, L"SteSB", TRUE);
	AudioConsole_AddEvent(L"ReleaseTrk", kszModuleName_XenonSound, kstEventType_Destroy);
#endif

	// Destroy Stereo Buffer (If Applicable)

	if (Param_pSoundBuffer_Stereo != Param_pSoundBuffer_Mono)
	{
		GetAudioEngine()->DestroySoundBuffer(Param_pSoundBuffer_Stereo);
	}

	// Destroy Mono Buffer

	GetAudioEngine()->DestroySoundBuffer(Param_pSoundBuffer_Mono);
#endif
}

// ***********************************************************************************************************************

void xeSND_StreamPrefetch(unsigned int Param_uiFilePosition, unsigned int Param_uiFileSize, char Param_cSoundBFIndex)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_uiFilePosition, L"Pos", TRUE);
	AudioConsole_AddEventParameter((int)Param_uiFileSize, L"Size");
	AudioConsole_AddEvent(L"Str - Prefetch", kszModuleName_XenonSound, kstEventType_Update);
#endif

	// Error Checking

	if (Param_uiFileSize == 0)
	{
		return;
	}

	// Get Prefetch List

	CPrefetchList* pPrefetchList = GetAudioEngine()->GetPrefetchList();

	// Create / Find Buffer

	CPrefetchBuffer* pBuffer = pPrefetchList->Request(Param_uiFilePosition, min(Param_uiFileSize, kiStreamPacketSize), (eSoundBFIndex)Param_cSoundBFIndex);

	// Trigger Loading

	if (pBuffer != NULL)
	{
		pBuffer->StartLoad();
	}
#endif
}

// ***********************************************************************************************************************

void xeSND_StreamPrefetchArray(unsigned int* Param_pFilePositionArray, unsigned int* Param_pFileSizeArray, char* Param_pSoundBFIndex)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_pFilePositionArray, L"PtrPos", TRUE);
	AudioConsole_AddEventParameter((int)Param_pFileSizeArray, L"PtrSize", TRUE);
	AudioConsole_AddEvent(L"Str - PrefetchArray", kszModuleName_XenonSound, kstEventType_Update);
#endif

	// Error Checking

	if ((Param_pFilePositionArray == NULL) || (Param_pFileSizeArray == NULL) || (Param_pSoundBFIndex == NULL))
	{
		return;
	}

	// Get Prefetch List

	CPrefetchList* pPrefetchList = GetAudioEngine()->GetPrefetchList();

	for (int iLoop = 0; iLoop < SND_Cte_StreamPrefetchMax; iLoop++)
	{
		if ((Param_pFilePositionArray[iLoop] != 0) && (Param_pFileSizeArray[iLoop] != 0))
		{
			// Create / Find Buffer

			CPrefetchBuffer* pBuffer = pPrefetchList->Request(Param_pFilePositionArray[iLoop], min(Param_pFileSizeArray[iLoop], kiStreamPacketSize), (eSoundBFIndex)Param_pSoundBFIndex[iLoop], iLoop);

			if (pBuffer != NULL)
			{
				// Trigger Loading

				pBuffer->StartLoad();
			}
		}

		Param_pFilePositionArray[iLoop] = 0;
	}
#endif
}

// ***********************************************************************************************************************

void xeSND_StreamFlush(unsigned int Param_uiFilePosition)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_uiFilePosition, L"Pos", TRUE);
	AudioConsole_AddEvent(L"Str - Flush", kszModuleName_XenonSound, kstEventType_Update);
#endif

	// Get Prefetch List

	CPrefetchList* pPrefetchList = GetAudioEngine()->GetPrefetchList();

	// Flush List

	pPrefetchList->Flush();
#endif
}

// ***********************************************************************************************************************

void xeSND_StreamGetPrefetchStatus(unsigned int* Param_pFilePositionArray)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	if (AudioConsole_GetConfiguration(eConfiguration_LogEvent_StreamPrefetchStatus))
	{
		AudioConsole_WaitForMessageExclusion();
		AudioConsole_AddEventParameter((int)Param_pFilePositionArray, L"Ptr", TRUE);
		AudioConsole_AddEvent(L"Str - GetPrefetchStatus", kszModuleName_XenonSound, kstEventType_Update);
	}
#endif

	// Get Prefetch List

	CPrefetchList* pPrefetchList = GetAudioEngine()->GetPrefetchList();

	// Scan Mapping Array

	for(int iLoop = 0; iLoop < SND_Cte_StreamPrefetchMax; iLoop++)
	{
		// Find Buffer

		CPrefetchBuffer* pBuffer = pPrefetchList->FindFromMap(iLoop);

		// Copy Position To Parameter Array If Useable

		if ((pBuffer != NULL) && (pBuffer->GetStatus() == ePrefetchStatus_Useable))
		{
			Param_pFilePositionArray[iLoop] = pBuffer->GetPosition();
		}
	}
#endif
}

// ***********************************************************************************************************************

void xeSND_StreamReinitAndPlay(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iFlags, int Param_iLoopCount, unsigned int Param_uiFileSize, unsigned int Param_uiFilePositionInBF, unsigned int Param_uiLoopBeginInSoundData, unsigned int Param_uiLoopEndInSoundData, int Param_iStartPositionInSoundData, int Param_iStopPositionInSoundData, int Param_iFrequency, int Param_iPan, int Param_iVolume, int Param_iFXVolume_Left, int Param_iFXVolume_Right)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Ensure Enabled

	if (!GetAudioEngine()->GetRuntimeSettings()->bAllowStreamPlay)
	{
		return;
	}

	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
	AudioConsole_AddEventParameter(Param_iFlags, L"Flag", TRUE);
	AudioConsole_AddEventParameter(Param_iLoopCount, L"LoopNb");
	AudioConsole_AddEventParameter((int)Param_uiFileSize, L"\nSize");
	AudioConsole_AddEventParameter((int)Param_uiFilePositionInBF, L"Pos", TRUE);
	AudioConsole_AddEventParameter((int)Param_uiLoopBeginInSoundData, L"LoopSt");
	AudioConsole_AddEventParameter((int)Param_uiLoopEndInSoundData, L"LoopEn");
	AudioConsole_AddEventParameter(Param_iStartPositionInSoundData, L"StPos");
	AudioConsole_AddEventParameter(Param_iStopPositionInSoundData, L"\nEnPos");
	AudioConsole_AddEventParameter(Param_iFrequency, L"Freq");
	AudioConsole_AddEventParameter(Param_iPan, L"Pan");
	AudioConsole_AddEventParameter(Param_iVolume, L"Vol");
	AudioConsole_AddEventParameter(Param_iFXVolume_Left, L"FxL");
	AudioConsole_AddEventParameter(Param_iFXVolume_Right, L"FxR");
	AudioConsole_AddEvent(L"ReInitPlay", kszModuleName_XenonSound, kstEventType_Start);
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL) || (Param_uiFileSize == 0))
	{
		return;
	}

	// Setup Stream For Playback

	Param_pSoundBuffer->pVoice->Setup_Stream_Play(Param_iFlags, Param_iLoopCount, Param_uiFileSize, Param_uiFilePositionInBF, Param_uiLoopBeginInSoundData, Param_uiLoopEndInSoundData, Param_iStartPositionInSoundData, Param_iStopPositionInSoundData, Param_iFrequency, Param_iPan, Param_iVolume, Param_iFXVolume_Left, Param_iFXVolume_Right);
#endif
}

// ***********************************************************************************************************************

void xeSND_StreamChain(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iFlags, int Param_iLoopCount, unsigned int Param_uiExitPointInSoundData, unsigned int Param_uiEnterPointInSoundData, unsigned int Param_uiFileSize, unsigned int Param_uiFilePositionInBF, unsigned int Param_uiLoopBeginInSoundData, unsigned int Param_uiLoopEndInSoundData)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter((int)Param_pSoundBuffer, L"Ptr", TRUE);
	AudioConsole_AddEventParameter(Param_iFlags, L"Flag", TRUE);
	AudioConsole_AddEventParameter(Param_iLoopCount, L"Loop");
	AudioConsole_AddEventParameter((int)Param_uiExitPointInSoundData, L"ExPt");
	AudioConsole_AddEventParameter((int)Param_uiEnterPointInSoundData, L"EnPt");
	AudioConsole_AddEventParameter((int)Param_uiFileSize, L"Size");
	AudioConsole_AddEventParameter((int)Param_uiFilePositionInBF, L"Pos", TRUE);
	AudioConsole_AddEventParameter((int)Param_uiLoopBeginInSoundData, L"LoopSt");
	AudioConsole_AddEventParameter((int)Param_uiLoopEndInSoundData, L"LoopEn");
	AudioConsole_AddEvent(L"Str - Chain", kszModuleName_XenonSound, kstEventType_Update);
#endif

	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL) || (Param_uiFileSize == 0))
	{
		return;
	}

	// Setup Stream For Chaining

	Param_pSoundBuffer->pVoice->Setup_Stream_Chain(Param_iFlags, Param_iLoopCount, Param_uiExitPointInSoundData, Param_uiEnterPointInSoundData, Param_uiFileSize, Param_uiFilePositionInBF, Param_uiLoopBeginInSoundData, Param_uiLoopEndInSoundData);
#endif
}

// ***********************************************************************************************************************

void xeSND_StreamLoopCountGet(SND_tdst_SoundBuffer* Param_pSoundBuffer, int* Param_pLoopCount)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL) || (Param_pLoopCount == NULL))
	{
		return;
	}

	// Fetch Loop Count

	*Param_pLoopCount = Param_pSoundBuffer->pVoice->GetLoopCount();
#endif
}

// ***********************************************************************************************************************

void xeSND_StreamChainDelayGet(SND_tdst_SoundBuffer* Param_pSoundBuffer, float* Param_pDelay)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL) || (Param_pDelay == NULL))
	{
		return;
	}

	// Fetch Chain Delay

	*Param_pDelay = Param_pSoundBuffer->pVoice->GetChainDelay();
#endif
}

// ***********************************************************************************************************************

void xeSND_StreamShutDownAll()
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEvent(L"Str - Shutdown", kszModuleName_XenonSound, kstEventType_Stop);
#endif

	// Stop All Streams

	GetAudioEngine()->Streams(eStreamOperation_Stop);
	GetAudioEngine()->ResetReinitVolume();
#endif
}

// ***********************************************************************************************************************

void xeSND_GlobalSetVol(LONG Param_lVolume)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Console Logging

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_WaitForMessageExclusion();
	AudioConsole_AddEventParameter(Param_lVolume);
	AudioConsole_AddEvent(L"SetGlobalVolume", kszModuleName_XenonSound, kstEventType_Update);
#endif

	// Update Global Volume

	GetAudioEngine()->SetGlobalVolume(Param_lVolume);
#endif
}

// ***********************************************************************************************************************

LONG xeSND_lGlobalGetVol()
{
#if !_XENON_SOUND_ENGINE
	return 0;
#else
	// Fetch Global Volume

	return GetAudioEngine()->GetGlobalVolume();
#endif
}

// ***********************************************************************************************************************

void xeSND_CommitChange(MATH_tdst_Vector* Param_pSpeed)
{
	// Currently Unused Function
}

// ***********************************************************************************************************************

int xeSND_iChangeRenderMode(int Param_iMode)
{
#if !_XENON_SOUND_ENGINE
	return -1;
#else
	// Force Dolby Digital

	SND_gst_Params.ul_RenderMode = SND_Cte_RenderDolbyDigital;

	return S_OK;
#endif
}

// ***********************************************************************************************************************

int xeSND_iFxInit()
{
#if !_XENON_SOUND_ENGINE
	return -1;
#else
	// Setup All Cores

	for (int iLoop = 0; iLoop < SND_Cte_FxCoreNb; iLoop++)
	{
		CFXCore* pCore = GetAudioEngine()->GetFXCore(iLoop);

		if (pCore != NULL)
		{
			pCore->Initialize(iLoop);
		}
	}

	return S_OK;
#endif
}

// ***********************************************************************************************************************

void xeSND_FxClose()
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Clear All Cores

	for (int iLoop = 0; iLoop < SND_Cte_FxCoreNb; iLoop++)
	{
		CFXCore* pCore = GetAudioEngine()->GetFXCore(iLoop);

		if (pCore != NULL)
		{
			pCore->Uninit();
		}
	}
#endif
}

// ***********************************************************************************************************************

int xeSND_iFxSetMode(int Param_iCoreID, int Param_iMode)
{
#if !_XENON_SOUND_ENGINE
	return -1;
#else
	// Fetch Core

	CFXCore* pCore = GetAudioEngine()->GetFXCore(Param_iCoreID);

	// Error Checking

	if (pCore == NULL)
	{
		return -1;
	}

	pCore->SetMode(Param_iMode);

	return S_OK;
#endif
}

// ***********************************************************************************************************************

int xeSND_iFxSetDelay(int Param_iCoreID, int Param_iDelay)
{
//#if !_XENON_SOUND_ENGINE
	return -1;
/*
#else
	// Fetch Core

	CFXCore* pCore = GetAudioEngine()->GetFXCore(Param_iCoreID);

	// Error Checking

	if (pCore == NULL)
	{
		return -1;
	}

	pCore->SetDelay(Param_iDelay);

	return S_OK;
#endif
*/
}

// ***********************************************************************************************************************

int xeSND_iFxSetFeedback(int Param_iCoreID, int Param_iFeedback)
{
//#if !_XENON_SOUND_ENGINE
	return -1;
/*
#else
	// Fetch Core

	CFXCore* pCore = GetAudioEngine()->GetFXCore(Param_iCoreID);

	// Error Checking

	if (pCore == NULL)
	{
		return -1;
	}

	pCore->SetFeedback(Param_iFeedback);

	return S_OK;
#endif
*/
}

// ***********************************************************************************************************************

int xeSND_iFxSetWetVolume(int Param_iCoreID, int Param_iWetVolume)
{
#if !_XENON_SOUND_ENGINE
	return -1;
#else
	// Fetch Core

	CFXCore* pCore = GetAudioEngine()->GetFXCore(Param_iCoreID);

	// Error Checking

	if (pCore == NULL)
	{
		return -1;
	}

	pCore->SetWetVolume(SND_f_GetVolFromAtt(Param_iWetVolume));

	return S_OK;
#endif
}

// ***********************************************************************************************************************

int xeSND_iFxSetWetPan(int Param_iCoreID, int Param_iWetPan)
{
#if !_XENON_SOUND_ENGINE
	return -1;
#else
	// Fetch Core

	CFXCore* pCore = GetAudioEngine()->GetFXCore(Param_iCoreID);

	// Error Checking

	if (pCore == NULL)
	{
		return -1;
	}

	pCore->SetWetPan(Param_iWetPan);

	return S_OK;
#endif
}

// ***********************************************************************************************************************

int xeSND_iFxEnable(int Param_iCoreID)
{
#if !_XENON_SOUND_ENGINE
	return -1;
#else
	// Fetch Core

	CFXCore* pCore = GetAudioEngine()->GetFXCore(Param_iCoreID);

	// Error Checking

	if (pCore == NULL)
	{
		return -1;
	}

	pCore->SetEnabled(TRUE);

	return S_OK;
#endif
}

// ***********************************************************************************************************************

int xeSND_iFxDisable(int Param_iCoreID)
{
#if !_XENON_SOUND_ENGINE
	return -1;
#else
	// Fetch Core

	CFXCore* pCore = GetAudioEngine()->GetFXCore(Param_iCoreID);

	// Error Checking

	if (pCore == NULL)
	{
		return -1;
	}

	pCore->SetEnabled(FALSE);

	return S_OK;
#endif
}

// ***********************************************************************************************************************
//    External Interface - Supplemental
// ***********************************************************************************************************************

void xeSND_StreamSetReinitFormat(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iFormat)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	// Error Checking

	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		return;
	}

	// Update Reinit Wave Format (For Decompression)

	Param_pSoundBuffer->pVoice->Setup_Stream_SetFormat(Param_iFormat);
#endif
}

// ***********************************************************************************************************************

void xeSND_SndBuffSetKey(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iKey, char Param_cSoundBFIndex)
{
#if !_XENON_SOUND_ENGINE
	return;
#else
	if ((Param_pSoundBuffer == NULL) || (Param_pSoundBuffer->pVoice == NULL))
	{
		// Update Future Voice Initialization Key

		GetAudioEngine()->SetVoiceInit_Key(Param_iKey, Param_cSoundBFIndex);
	}
	else
	{
		// Directly Update Voice Key

		Param_pSoundBuffer->pVoice->SetKey(Param_iKey, (eSoundBFIndex)Param_cSoundBFIndex);
	}
#endif
}

// ***********************************************************************************************************************

#endif // _XENON

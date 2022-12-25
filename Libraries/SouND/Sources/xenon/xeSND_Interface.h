#ifndef __xeSND_Interface_h__
#define __xeSND_Interface_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDfx.h"

// ***********************************************************************************************************************
//    External Interface - Standard
// ***********************************************************************************************************************

int						xeSND_lInit(SND_tdst_TargetSpecificData* Param_pSpecificData);
void					xeSND_Close(SND_tdst_TargetSpecificData* Param_pSpecificData);

SND_tdst_SoundBuffer*	xeSND_pSndBuffCreate(SND_tdst_TargetSpecificData* Param_pSpecificData, SND_tdst_WaveData* Param_pWaveData, unsigned int Param_uiFilePositionInBF);
SND_tdst_SoundBuffer*	xeSND_pSndBuffReLoad(SND_tdst_TargetSpecificData* Param_pSpecificData, SND_tdst_SoundBuffer* Param_pSoundBuffer, SND_tdst_WaveData* Param_pWaveData, unsigned int Param_uiFilePositionInBF);

int						xeSND_iSndBuffPlay(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iFlags, int Param_iFXVolume_Left, int Param_iFXVolume_Right);
void					xeSND_SndBuffStop(SND_tdst_SoundBuffer* Param_pSoundBuffer);
void					xeSND_SndBuffPause(SND_tdst_SoundBuffer* Param_pSoundBuffer);

void					xeSND_SndBuffRelease(SND_tdst_SoundBuffer* Param_pSoundBuffer);
void					xeSND_SndBuffDuplicate(SND_tdst_TargetSpecificData* Param_pSpecificData, SND_tdst_SoundBuffer* Param_pSoundBuffer_Source, SND_tdst_SoundBuffer** Param_ppSoundBuffer_Dest);

void					xeSND_SndBuffSetPos(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iPosition);
void					xeSND_SndBuffSetVol(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iVolume);
void					xeSND_SndBuffSetFreq(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iFrequency);
void					xeSND_SndBuffSetPan(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iPan, int Param_iSurroundPan);

int						xeSND_iSndBuffGetStatus(SND_tdst_SoundBuffer* Param_pSoundBuffer, int* Param_pStatus);
void					xeSND_SndBuffGetFreq(SND_tdst_SoundBuffer* Param_pSoundBuffer, int* Param_pFrequency);
void					xeSND_SndBuffGetPan(SND_tdst_SoundBuffer* Param_pSoundBuffer, int* Param_pPan, int* Param_pSurroundPan);
void					xeSND_SndBuffGetPos(SND_tdst_SoundBuffer* Param_pSoundBuffer, int* Param_pPosition_Read, int* Param_pPosition_Write);

void					xeSND_SndBuffCreateTrack(SND_tdst_SoundBuffer** Param_ppSoundBuffer_Mono, SND_tdst_SoundBuffer** Param_ppSoundBuffer_Stereo);
void					xeSND_SndBuffReleaseTrack(SND_tdst_SoundBuffer* Param_pSoundBuffer_Mono, SND_tdst_SoundBuffer* Param_pSoundBuffer_Stereo);

void					xeSND_StreamPrefetch(unsigned int Param_uiFilePosition, unsigned int Param_uiFileSize, char Param_cSoundBFIndex);
void					xeSND_StreamPrefetchArray(unsigned int* Param_pFilePositionArray, unsigned int* Param_pFileSizeArray, char* Param_pSoundBFIndex);
void					xeSND_StreamFlush(unsigned int Param_uiFilePosition);
void					xeSND_StreamGetPrefetchStatus(unsigned int* Param_pFilePositionArray);

void					xeSND_StreamReinitAndPlay(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iFlags, int Param_iLoopCount, unsigned int Param_uiFileSize, unsigned int Param_uiFilePositionInBF, unsigned int Param_uiLoopBeginInSoundData, unsigned int Param_uiLoopEndInSoundData, int Param_iStartPositionInSoundData, int Param_iStopPositionInSoundData, int Param_iFrequency, int Param_iPan, int Param_iVolume, int Param_iFXVolume_Left, int Param_iFXVolume_Right);
void					xeSND_StreamChain(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iFlags, int Param_iLoopCount, unsigned int Param_uiExitPointInSoundData, unsigned int Param_uiEnterPointInSoundData, unsigned int Param_uiFileSize, unsigned int Param_uiFilePositionInBF, unsigned int Param_uiLoopBeginInSoundData, unsigned int Param_uiLoopEndInSoundData);

void					xeSND_StreamLoopCountGet(SND_tdst_SoundBuffer* Param_pSoundBuffer, int* Param_pLoopCount);
void					xeSND_StreamChainDelayGet(SND_tdst_SoundBuffer* Param_pSoundBuffer, float* Param_pDelay);
void					xeSND_StreamShutDownAll();

void					xeSND_GlobalSetVol(LONG Param_lVolume);
LONG					xeSND_lGlobalGetVol();

void					xeSND_CommitChange(MATH_tdst_Vector* Param_pSpeed);
int						xeSND_iChangeRenderMode(int Param_iMode);

int						xeSND_iFxInit();
void					xeSND_FxClose();

int						xeSND_iFxSetMode(int Param_iCoreID, int Param_iMode);
int						xeSND_iFxSetDelay(int Param_iCoreID, int Param_iDelay);
int						xeSND_iFxSetFeedback(int Param_iCoreID, int Param_iFeedback);
int						xeSND_iFxSetWetVolume(int Param_iCoreID, int Param_iWetVolume);
int						xeSND_iFxSetWetPan(int Param_iCoreID, int Param_iWetPan);

int						xeSND_iFxEnable(int Param_iCoreID);
int						xeSND_iFxDisable(int Param_iCoreID);

// ***********************************************************************************************************************
//    External Interface - Supplemental
// ***********************************************************************************************************************

void					xeSND_StreamSetReinitFormat(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iFormat);
void					xeSND_SndBuffSetKey(SND_tdst_SoundBuffer* Param_pSoundBuffer, int Param_iKey, char Param_cSoundBFIndex);

// ***********************************************************************************************************************

#endif // __xeSND_Interface_h__

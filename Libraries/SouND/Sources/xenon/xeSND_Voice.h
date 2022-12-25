#ifndef __xeSND_Voice_h__
#define __xeSND_Voice_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "SouND/Sources/SNDwave.h"

#include "xeSND_Helpers.h"

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

#define kiStreamPacketSize			(SND_Cte_MaxBufferSize / kiStreamPacketCount)
#define kfRearAmplitudeDampening	0.50f
#define kfWetMixFactor_WetKickIn	0.50f
#define kfWetMixFactor_DryKickOut	0.70f

// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************

typedef enum
{
	eState_None,
	eState_Setup,
	eState_Playing,
	eState_Paused,
	eState_Stopped
}
eState;

// ***********************************************************************************************************************

typedef enum
{
	eEncoding_Unknown,
	eEncoding_PCM,
	eEncoding_ADPCM,
	eEncoding_XMA
}
eEncoding;

// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************

typedef struct
{
	BOOL			bSetupRequested;
	unsigned int	uiFilePosition;
	int				iFileSize;
	int				iPositionStart;
	int				iPositionStop;
	int				iPositionChain;
	int				iLoopBegin;
	int				iLoopEnd;
	int				iFlags;
	int				iFrequency;
	int				iPan;
	int				iVolume;
	int				iFormat;
	int				iLoopCount;
	float			fWetMixFactor;
}
stStreamInitSettings;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CVoice : public CListItem
{
private:
	CPacketTracker*			m_pPacketTracker;
	IXAudioSourceVoice*		m_pSourceVoice;
	XAUDIOSOURCEVOICEINIT	m_stSourceVoiceInit;
	WAVEFORMATEX			m_stWaveFormat;
	stStreamInitSettings	m_stInitSettings;

	eState volatile			m_eState;

	CBuffer*				m_pBuffer_Playback;
	CBuffer*				m_pBuffer_Loading;

	int volatile			m_iSize_LoadedData_Decoded;
	int volatile			m_iSize_LastReadResult;
	int volatile			m_iSize_File_SoundData;

	unsigned int volatile	m_uiPosition_File_SoundStartInBF;
	int volatile			m_iPosition_File_CurrentOffsetInSoundData;
	int volatile			m_iPosition_File_StopOffsetInSoundData;
	int volatile			m_iPosition_File_LoopBeginInSoundData;
	int volatile			m_iPosition_File_LoopEndInSoundData;
	int volatile			m_iPosition_Buffer_ToLoad;
	int volatile			m_iPosition_Buffer_ToDecode;
	int volatile			m_iPosition_Buffer_ToPlay;

	BOOL volatile			m_bHasReachedEndOfFile;
	BOOL					m_bIsMusic;

	int						m_iFlags;
	int						m_iStreamID;
	int						m_iKey;
	eSoundBFIndex			m_eSoundBFIndex;

	int						m_iLoopCount;
	int						m_iFrequency;
	int						m_iPan;
	int						m_iSurroundPan;
	int						m_iVolume;
	float					m_fVolumeAddGain;
	float					m_fWetMixFactor;
	int						m_iVoiceOutputs;

public:
    M_DeclareOperatorNewAndDelete();

	CVoice(SND_tdst_WaveData* Param_pWaveData, unsigned int Param_uiFilePositionInBF, int Param_iStreamID, CBuffer* Param_pBuffer_Playback);
	virtual ~CVoice();

	void					SetListIndex(int Param_iListIndex);
	int						GetListIndex();
	void					SetKey(int Param_iKey, eSoundBFIndex Param_eSoundBFIndex);
	int						GetKey();
	int						GetStreamID();
	eState					GetState();
	int						GetFXCoreID();
	CBuffer*				GetPlaybackBuffer();
	char*					RequestLoadingBuffer(int Param_iSize);
	unsigned int			GetFileStartPosition();
	WAVEFORMATEX*			GetWaveFormat();
	eEncoding				GetEncoding(int Param_iFormat = 0);
	int						SoundPosition_ClampDownToFrame(int Param_iFilePosition, int Param_iFormat = 0);
	int						SoundPosition_NativeToPCM(int Param_iFilePosition, int Param_iFormat = 0);
	int						SoundPosition_PCMToNative(int Param_iFilePosition, int Param_iFormat = 0);
	BOOL					IsMusic();
	BOOL					IsLooping();
	BOOL					IsSetupping();
	BOOL					IsPlaying();
	BOOL					IsPaused();
	BOOL					IsStopped();
	BOOL					IsAStream();
	BOOL					IsSinglePacket();
	void					Play(int Param_iFlags = -1, int Param_iFxVolume_Left = 0xFFFF, int Param_iFxVolume_Right = 0xFFFF);
	void					Pause();
	void					Stop();
	void					UpdateVolume();
	void					SetVolume(int Param_iVolume);
	void					SetVolumeAddGain(float Param_fVolumeAddGain);
	void					SetFrequency(int Param_iFrequency);
	void					SetPan(int Param_iPan, int Param_iSurroundPan);
	int						GetVolume();
	float					GetVolumeAddGain();
	int						GetFrequency();
	int						GetPan();
	int						GetSurroundPan();
	int						GetChannelCount();
	int						GetLoopCount();
	int						GetFileSize();
	int						GetSamplesPerSecond();
	float					GetBufferDurationInSeconds();
	float					GetFileDurationInSeconds();
	float					GetChainDelay();
	int						GetPosition_File_Current();
	int						GetPosition_File_Stop();
	int						GetPosition_File_LoopBegin();
	int						GetPosition_File_LoopEnd();
	int						GetPosition_File_Chain();
	int						GetPosition_Buffer_Load();
	int						GetPosition_Buffer_Decode();
	int						GetPosition_Buffer_Play();
	int						GetCursorInBuffer_PCM();
	int						GetCursorInBuffer_Native();
	int						GetCursorPosition_PCM();
	int						GetCursorPosition_Native();
	void					SetCursorPosition_PCM(int Param_iPosition);
	void					SetCursorPosition_Native(int Param_iPosition);
	void					TransferToDuplicate(CVoice* Param_pSourceVoice);
	void					Setup_Channels();
	void					Setup_Static(SND_tdst_WaveData* Param_pWaveData, unsigned int Param_uiFilePositionInBF);
	void					Setup_Stream_SetFormat(int Param_iFormat);
	void					Setup_Stream_Play(int Param_iFlags, int Param_iLoopCount, int Param_iFileSize, unsigned int Param_uiFilePositionInBF, int Param_iLoopBeginPosition, int Param_iLoopEndPosition, int Param_iStartPositionInSoundData, int Param_iStopPositionInSoundData, int Param_iFrequency, int Param_iPan, int Param_iVolume, int Param_iFxVolume_Left, int Param_iFxVolume_Right);
	void					Setup_Stream_Chain(int Param_iFlags, int Param_iLoopCount, int Param_iExitPointInSoundData, int Param_iEnterPointInSoundData, int Param_iFileSize, unsigned int Param_uiFilePositionInBF, int Param_iLoopBeginPosition, int Param_iLoopEndPosition);
	void					Packet_LoadStart();
	void					Packet_LoadDone();
	void					Packet_Decode(int Param_iSize);
	BOOL					Packet_Submit(int Param_iSize);
	void					Packet_EndNotify();
};

// ***********************************************************************************************************************

#endif // __xeSND_Voice_h__

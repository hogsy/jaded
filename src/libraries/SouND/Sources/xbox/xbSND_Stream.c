/*$T xbSND_Stream.c GC 1.138 01/21/05 11:31:38 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#if !defined(_XBOX)
#error this is an XBOX only file.
#endif

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
 
#include "SouND/Sources/xbox/xbSND_Stream.h"

#include "BASe/XBox/XBCompositeFile.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"

#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGio.h"
#include "ENGine/Sources/ENGvars.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDstream.h"

#include "SouND/Sources/xbox/xbSND_Thread.h"
#include "SouND/Sources/xbox/xbSND_RAM.h"
#include "SouND/Sources/xbox/xbSND_debug.h"

#include <dsound.h>

#include "Gx8/Gx8FileError.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */
#define xbSND_Cte_ReadRetryMax 3

#define xbSND_Cte_StreamBufferNb	(SND_Cte_MaxSimultaneousStream * 2)
#define xbSND_Cte_NotificationSize	(SND_Cte_MaxBufferSize / xbSND_Cte_NotifyNb)
#define M_RoundSize(_size, _channel) \
	do \
	{ \
		_size = (_size) / (36 * (_channel)); \
		_size = (_size) * (36 * (_channel)); \
	} while(0);

extern ULONG LOA_ul_BinKey;
/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

/*$2- streams management ---------------------------------------------------------------------------------------------*/

typedef enum	xbSND_tden_StreamState_
{
	xbStream_e_Reinit	= 0,
	xbStream_e_Playing,
	xbStream_e_Stopping,
	xbStream_e_Stopped,
	xbStream_e_Error
} xbSND_tden_StreamState;

typedef struct	xbSND_tdst_StreamSettings_
{
	volatile unsigned int	ui_Flags;
	volatile int			i_InitFreq;
	volatile int			i_InitPan;
	volatile int			i_InitVol;
	volatile int			i_InitFxVolLeft;
	volatile int			i_InitFxVolRight;

	volatile unsigned int	ui_FilePosition;
	volatile unsigned int	ui_FileSize;

	volatile unsigned int	ui_StartSeek;
	volatile unsigned int	ui_ExitSeek;
	volatile unsigned int	ui_LoopBeginSeek;
	volatile unsigned int	ui_LoopEndSeek;
	volatile int			i_LoopNb;
} xbSND_tdst_StreamSettings;

typedef struct	xbSND_tdst_SoundStream_
{
	int								i_StreamId;
	volatile xbSND_tden_StreamState en_State;
	XBCompositeFile					*h_File;
	volatile unsigned int			ui_FileCurrSeek;
	volatile unsigned int			ui_LastReadResult;
	volatile unsigned int			ui_ChainSize;
	volatile unsigned int			ui_LoopSize;
	volatile unsigned int			ui_EndSize;
	volatile unsigned int			ui_WriteCursor;
	volatile int					i_CurrLoadingBufferIdx;
	volatile float					f_ChainDelay;
    volatile int                   b_PendingPlay;
    
    volatile unsigned int         ui_LoopingNb;
    volatile unsigned int         ui_DebugPlayPos;

    volatile unsigned int         ui_StopPosition;
    volatile unsigned int         ui_StopBufferPosition;

    DSBPOSITIONNOTIFY		        ast_Notification[xbSND_Cte_NotifyNb];
    
	xbSND_tdst_StreamSettings		st_Current;
	xbSND_tdst_StreamSettings		st_Next;
} xbSND_tdst_SoundStream;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    prefetch
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef enum	xbSND_tden_PrefetchState_ { e_PrefetchFree = 0, e_PrefetchLoading, e_PrefetchWaitingForUse, e_PrefetchLost } xbSND_tden_PrefetchState;

typedef struct	xbSND_tdst_Prefetch_
{
	xbSND_tden_PrefetchState volatile	en_State;

	/* volatile MaxP */
	XBCompositeFile						*h_File;
	volatile unsigned int				ui_PrefetchPosition;
	volatile unsigned int				ui_PrefetchSize;
	volatile unsigned int				ui_ReadSizeResult;
	volatile char						*pc_RamBuffer;
    volatile unsigned int				ui_RetryCount;
}
xbSND_tdst_Prefetch;

/*$4
 ***********************************************************************************************************************
    Private Prototypes
 ***********************************************************************************************************************
 */

static SND_tdst_SoundBuffer *xbSND_pst_StreamCreate(SND_tdst_TargetSpecificData *, SND_tdst_WaveData *);
static unsigned int			xbSND_ui_IsStreamPrefetched(char *_pc_Buff, unsigned int _ui_Seek, unsigned int _ui_Size);
static void					xbSND_StreamStopPair(int id);
static void					xbSND_StreamReadFile(SND_tdst_SoundBuffer *, char *, unsigned int);
static BOOL					xbSND_Overloaded_XBCompositeFile_Read
							(
								XBCompositeFile *file,
								char			*buffer,
								DWORD			bytesToRead,
								DWORD			*bytesRead,
								OVERLAPPED		*overlapped
							);

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

SND_tdst_SoundBuffer	*xbSND_ga_StreamSoundBufferList[xbSND_Cte_StreamBufferNb];
CRITICAL_SECTION		xbSND_gx_InitSettingsSection;
xbSND_tdst_Prefetch		xbSND_gst_PrefetchList[SND_Cte_StreamPrefetchMax];
volatile int			xbSND_gi_PendingPrefetch;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_InitStreamModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	/*$2- reset all --------------------------------------------------------------------------------------------------*/

	L_memset(xbSND_ga_StreamSoundBufferList, 0, sizeof(SND_tdst_SoundBuffer *) * xbSND_Cte_StreamBufferNb);
	L_memset(xbSND_gst_PrefetchList, 0, SND_Cte_StreamPrefetchMax * sizeof(xbSND_tdst_Prefetch));

	/*$2- init settings ----------------------------------------------------------------------------------------------*/

	InitializeCriticalSection(&xbSND_gx_InitSettingsSection);

	/*$2- init prefetch array ----------------------------------------------------------------------------------------*/

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		xbSND_gst_PrefetchList[i].pc_RamBuffer = xbSND_AllocFix(SND_Cte_MaxBufferSize / 2);
		xbSND_M_Assert(xbSND_gst_PrefetchList[i].pc_RamBuffer);

		xbSND_gst_PrefetchList[i].h_File = XBCompositeFile_Open
			(
				MAI_gst_InitStruct.asz_ProjectName,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
				Gx8_FileErrorSound
			);
	}

	xbSND_gi_PendingPrefetch = 0;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_CloseStreamModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		if(xbSND_gst_PrefetchList[i].pc_RamBuffer != NULL)
		{
			xbSND_FreeFix(xbSND_gst_PrefetchList[i].pc_RamBuffer);
			xbSND_gst_PrefetchList[i].pc_RamBuffer = NULL;
		}

		if(xbSND_gst_PrefetchList[i].h_File != NULL)
		{
			XBCompositeFile_Close(xbSND_gst_PrefetchList[i].h_File);
			xbSND_gst_PrefetchList[i].h_File = NULL;
		}
	}

	L_memset(xbSND_ga_StreamSoundBufferList, 0, sizeof(SND_tdst_SoundBuffer *) * xbSND_Cte_StreamBufferNb);
	L_memset(xbSND_gst_PrefetchList, 0, SND_Cte_StreamPrefetchMax * sizeof(xbSND_tdst_Prefetch));

	DeleteCriticalSection(&xbSND_gx_InitSettingsSection);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_CreateTrackSB(SND_tdst_SoundBuffer **_ppst_MonoSB, SND_tdst_SoundBuffer **_ppst_StereoSB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_WaveData	st_WaveData;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st_WaveData.i_Size = 0;
	st_WaveData.pbData = NULL;

	st_WaveData.st_WaveFmtx.nChannels = 1;
	st_WaveData.st_WaveFmtx.cbSize = 0;
	st_WaveData.st_WaveFmtx.wFormatTag = SND_Cte_DefaultWaveFormat;
	st_WaveData.st_WaveFmtx.nSamplesPerSec = SND_Cte_StreamFrequency;
	st_WaveData.st_WaveFmtx.wBitsPerSample = 16;
	st_WaveData.st_WaveFmtx.nBlockAlign = 2;
	st_WaveData.st_WaveFmtx.nAvgBytesPerSec = st_WaveData.st_WaveFmtx.nBlockAlign * st_WaveData.st_WaveFmtx.nSamplesPerSec;

	*_ppst_MonoSB = xbSND_pst_StreamCreate(SND_gst_Params.pst_SpecificD, &st_WaveData);

	st_WaveData.st_WaveFmtx.nChannels = 2;
	st_WaveData.st_WaveFmtx.cbSize = 0;
	st_WaveData.st_WaveFmtx.wFormatTag = SND_Cte_DefaultWaveFormat;
	st_WaveData.st_WaveFmtx.nSamplesPerSec = SND_Cte_StreamFrequency;
	st_WaveData.st_WaveFmtx.wBitsPerSample = 16;
	st_WaveData.st_WaveFmtx.nBlockAlign = 4;
	st_WaveData.st_WaveFmtx.nAvgBytesPerSec = st_WaveData.st_WaveFmtx.nBlockAlign * st_WaveData.st_WaveFmtx.nSamplesPerSec;

	*_ppst_StereoSB = xbSND_pst_StreamCreate(SND_gst_Params.pst_SpecificD, &st_WaveData);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_ReleaseTrackSB(SND_tdst_SoundBuffer *_pst_MonoSB, SND_tdst_SoundBuffer *_pst_StereoSB)
{
	xbSND_StreamRelease(_pst_MonoSB);
	if(_pst_StereoSB != _pst_MonoSB) xbSND_StreamRelease(_pst_StereoSB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static SND_tdst_SoundBuffer *xbSND_pst_StreamCreate
(
	SND_tdst_TargetSpecificData *_pst_SpecificD,
	SND_tdst_WaveData			*_pst_WI
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DSBUFFERDESC			dsbd;
	SND_tdst_SoundBuffer	*pst_SB;
	char					*pc_Data;
	XBOXADPCMWAVEFORMAT		st_XBOXADPCMWAVEFORMAT;
	IDirectSoundBuffer		*pDirectSoundBuffer;
	static int				i = 0;
	DSBPOSITIONNOTIFY		astDSBPOSITIONNOTIFY[xbSND_Cte_NotifyNb];
	int						j;
	HRESULT					hr;
	DSMIXBINS				dsMixBins;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- check input ------------------------------------------------------------------------------------------------*/

	if(!_pst_SpecificD->pst_DS) return NULL;
	xbSND_M_Assert(_pst_WI->st_WaveFmtx.wFormatTag == SND_Cte_DefaultWaveFormat);

	/*$2- read data --------------------------------------------------------------------------------------------------*/

	if(i & 1)
		pc_Data = xbSND_ga_StreamSoundBufferList[i - 1]->pv_RamBuffer;
	else
		pc_Data = XPhysicalAlloc(SND_Cte_MaxBufferSize, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE);

	if(!pc_Data)
	{
		/*~~~~~~~*/
		DWORD	dw;
		/*~~~~~~~*/

		dw = GetLastError();

		xbSND_M_BeginTraceBlock((asz_Log, "Can not allocate memory, error code %d", dw));
		xbSND_M_EndTraceBlock();
	}

	if(!pc_Data) return NULL;
	L_memset(pc_Data, 0, SND_Cte_MaxBufferSize);

	/*$2- create a Direct Sound Buffer -------------------------------------------------------------------------------*/

	st_XBOXADPCMWAVEFORMAT.wfx = _pst_WI->st_WaveFmtx;
	st_XBOXADPCMWAVEFORMAT.wfx.nBlockAlign = 36 * st_XBOXADPCMWAVEFORMAT.wfx.nChannels;
	st_XBOXADPCMWAVEFORMAT.wfx.nAvgBytesPerSec =
		(
			st_XBOXADPCMWAVEFORMAT.wfx.nSamplesPerSec *
			st_XBOXADPCMWAVEFORMAT.wfx.nBlockAlign
		) >>
		6;
	st_XBOXADPCMWAVEFORMAT.wfx.wBitsPerSample = 4;
	st_XBOXADPCMWAVEFORMAT.wfx.cbSize = 2;
	st_XBOXADPCMWAVEFORMAT.wSamplesPerBlock = 64;

	L_memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.lpwfxFormat = &st_XBOXADPCMWAVEFORMAT.wfx;
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY;

	/*$2- init sound buffer ------------------------------------------------------------------------------------------*/

	pst_SB = MEM_p_Alloc(sizeof(SND_tdst_SoundBuffer));
	L_memset(pst_SB, 0, sizeof(SND_tdst_SoundBuffer));

	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontLeft].dwMixBin = xbSND_Cte_SendFrontLeft;
	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontLeft].lVolume = -600;
	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontRight].dwMixBin = xbSND_Cte_SendFrontRight;
	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontRight].lVolume = -600;

	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_CenterLeft].dwMixBin = xbSND_Cte_SendCenterLeft;
	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_CenterLeft].lVolume = DSBVOLUME_MIN;
	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_CenterRight].dwMixBin = xbSND_Cte_SendCenterRight;
	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_CenterRight].lVolume = DSBVOLUME_MIN;

	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackLeft].dwMixBin = xbSND_Cte_SendBackLeft;
	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackLeft].lVolume = -600;
	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackRight].dwMixBin = xbSND_Cte_SendBackRight;
	pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackRight].lVolume = -600;

	pst_SB->i_MixBinNb = xbSND_Idx_NoFxNb;

	dsMixBins.dwMixBinCount = pst_SB->i_MixBinNb;
	dsMixBins.lpMixBinVolumePairs = pst_SB->dst_MixBinVolumePairs;
	dsbd.lpMixBins = &dsMixBins;

	hr = DirectSoundCreateBuffer(&dsbd, &pDirectSoundBuffer);
	if(hr != DS_OK)
	{
		if(pc_Data) xbSND_FreeFix(pc_Data);
		return NULL;
	}

	pst_SB->st_XBOXADPCMWAVEFORMAT = st_XBOXADPCMWAVEFORMAT;
	pst_SB->pst_DSB = pDirectSoundBuffer;
	pst_SB->pv_RamBuffer = (void *) pc_Data;
	pst_SB->ui_BufferSize = SND_Cte_MaxBufferSize;

	pst_SB->i_CurFrequency = -1;
	pst_SB->i_CurPan = -1;
	pst_SB->i_CurSPan = -1;
	pst_SB->i_CurVol = -1;

	hr = IDirectSoundBuffer_SetMixBins(pDirectSoundBuffer, &dsMixBins);
	xbSND_M_Assert(hr == DS_OK);

	/*$2- fill in the StreamBuffer -----------------------------------------------------------------------------------*/

	pst_SB->pst_SS = MEM_p_Alloc(sizeof(xbSND_tdst_SoundStream));
	L_memset(pst_SB->pst_SS, 0, sizeof(xbSND_tdst_SoundStream));
	pst_SB->pst_SS->i_StreamId = i;
	xbSND_M_Assert(i < xbSND_Cte_StreamBufferNb);
	xbSND_ga_StreamSoundBufferList[i++] = pst_SB;
	pst_SB->pst_SS->h_File = XBCompositeFile_Open
		(
			MAI_gst_InitStruct.asz_ProjectName,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			Gx8_FileErrorSound
		);
	xbSND_M_Assert(pst_SB->pst_SS->h_File != NULL);

	/*$2- setup notification positions -------------------------------------------------------------------------------*/

	hr = IDirectSoundBuffer_SetBufferData(pst_SB->pst_DSB, pst_SB->pv_RamBuffer, pst_SB->ui_BufferSize);
	xbSND_M_Assert(hr == DS_OK);

	for(j = 0; j < xbSND_Cte_NotifyNb; j++)
	{
		pst_SB->pst_SS->ast_Notification[j].dwOffset = astDSBPOSITIONNOTIFY[j].dwOffset = j * xbSND_Cte_NotificationSize;
		astDSBPOSITIONNOTIFY[j].hEventNotify = xbSND_gah_Event[xbSND_e_EventNotifyS0_A + j + (xbSND_Cte_NotifyNb * pst_SB->pst_SS->i_StreamId)];
	}

	hr = IDirectSoundBuffer_SetNotificationPositions(pst_SB->pst_DSB, xbSND_Cte_NotifyNb, astDSBPOSITIONNOTIFY);
	xbSND_M_Assert(hr == DS_OK);

	xbSND_SetFilter(pst_SB);

	return pst_SB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_StreamRelease(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~*/
	DWORD	dwStatus;
	HRESULT hr;
	/*~~~~~~~~~~~~~*/

	if(_pst_SB == NULL) return;
	if(!_pst_SB->pst_DSB) return;

	/*$2- delete reference -------------------------------------------------------------------------------------------*/

	xbSND_ga_StreamSoundBufferList[_pst_SB->pst_SS->i_StreamId] = NULL;

	/*$2- destroy the DirectSound object -----------------------------------------------------------------------------*/

	hr = IDirectSoundBuffer8_Stop(_pst_SB->pst_DSB);
	xbSND_M_Assert(hr == DS_OK);
	do
	{
		hr = IDirectSoundBuffer8_GetStatus(_pst_SB->pst_DSB, &dwStatus);
		xbSND_M_Assert(hr == DS_OK);
	} while(dwStatus & DSBSTATUS_PLAYING);
	hr = IDirectSoundBuffer8_Release(_pst_SB->pst_DSB);
	xbSND_M_Assert(hr == DS_OK);

	/*$2- free memory ------------------------------------------------------------------------------------------------*/

	if(!(_pst_SB->pst_SS->i_StreamId & 1) && _pst_SB->pv_RamBuffer) XPhysicalFree(_pst_SB->pv_RamBuffer);
	_pst_SB->pv_RamBuffer = NULL;

	if(_pst_SB->pst_SS) MEM_Free(_pst_SB->pst_SS);
	_pst_SB->pst_SS = NULL;

	MEM_Free(_pst_SB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_StreamReinitAndPlay
(
	SND_tdst_SoundBuffer	*_pSB,			/* SB */
	int						_i_Flag,		/* flag */
	int						_i_Loop,		/* loop nb */
	unsigned int			_ui_Size,		/* DataSize, */
	unsigned int			_ui_Position,	/* DataPosition, */
	unsigned int			_ui_LoopBegin,	/* LoopBeginOffset, */
	unsigned int			_ui_LoopEnd,	/* LoopEndOffset, */
	int						_i_StartPos,	/* StartPos, */
	int						_i_StopPos,		/* StopPos, */
	int						_i_Freq,		/* BaseFrequency, */
	int						_i_Pan,			/* Pan, */
	int						_i_Vol,			/* volume */
	int						fxleft,			/* fx left vol */
	int						fxright			/* fx right vol */
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	xbSND_tdst_StreamSettings	*pNext;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pSB) return;
	if(!_pSB->pst_SS) return;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    round size
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	M_RoundSize(_i_StartPos, _pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels);

	if(_i_StopPos != -1)
	{
		M_RoundSize(_i_StopPos, _pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels);
	}

	M_RoundSize(_ui_LoopBegin, _pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels);
	M_RoundSize(_ui_LoopEnd, _pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels);
	M_RoundSize(_ui_Size, _pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels);

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    register the order
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	pNext = &_pSB->pst_SS->st_Next;
	if(pNext->ui_FilePosition == _ui_Position) return;

	xbSND_StreamStopPair(_pSB->pst_SS->i_StreamId);

	EnterCriticalSection(&xbSND_gx_InitSettingsSection);
	pNext->ui_Flags = *(unsigned int *) &_i_Flag;
	pNext->i_InitFreq = _i_Freq;
	pNext->i_InitPan = _i_Pan;
	pNext->i_InitVol = _i_Vol;
	pNext->i_InitFxVolLeft = fxleft;
	pNext->i_InitFxVolRight = fxright;
	pNext->ui_FilePosition = _ui_Position;
	pNext->ui_FileSize = _ui_Size;
	pNext->ui_StartSeek = _i_StartPos;
	pNext->ui_LoopBeginSeek = _ui_LoopBegin;
	pNext->ui_LoopEndSeek = _ui_LoopEnd;
	pNext->ui_ExitSeek = 0;
	pNext->i_LoopNb = _i_Loop;
	LeaveCriticalSection(&xbSND_gx_InitSettingsSection);

	xbSND_M_BeginTraceBlock
	(
		(
			asz_Log, "push on S%d pos %d size %d startpos %d", _pSB->pst_SS->i_StreamId, _ui_Position, _ui_Size,
				_i_StartPos
		)
	);
	xbSND_M_EndTraceBlock();

	SetEvent(xbSND_gah_Event[xbSND_e_EventReinitS0 + _pSB->pst_SS->i_StreamId]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_ThStreamReinit(int _i_ID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundBuffer		*pSB;
	xbSND_tdst_SoundStream		*pSS;
	BOOL						b_Result;
	DWORD						dwStatus;
	HRESULT						hr;
	DSBPOSITIONNOTIFY			ast_DsbPositionNotify[xbSND_Cte_NotifyNb];
	int							i, j;
	unsigned int				ui_ReadSize;
	BOOL						b_Prefetched;
	xbSND_tdst_StreamSettings	*pReinit;
    float                      fFx, fFxR, fFxL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_ID < 0) return;
	if(_i_ID > xbSND_Cte_StreamBufferNb) return;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pSB = xbSND_ga_StreamSoundBufferList[_i_ID];
	pSS = xbSND_ga_StreamSoundBufferList[_i_ID]->pst_SS;
	if(pSS->st_Next.ui_FilePosition == 0) return;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(j = 0, i = (pSS->i_StreamId & 0xFFFFFFFE); j < xbSND_Cte_NotifyNb; j++, i++)
	{
		ResetEvent(xbSND_gah_Event[xbSND_e_EventReinitS0 + i]);
		ResetEvent(xbSND_gah_Event[xbSND_e_EventReadDoneS0 + i]);
		ResetEvent(xbSND_gah_Event[xbSND_e_EventNotifyS0_A + (xbSND_Cte_NotifyNb * i)]);
		ResetEvent(xbSND_gah_Event[xbSND_e_EventNotifyS0_B + (xbSND_Cte_NotifyNb * i)]);
		ResetEvent(xbSND_gah_Event[xbSND_e_EventEndHitS0 + i]);
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EnterCriticalSection(&xbSND_gx_InitSettingsSection);
	L_memcpy(&pSS->st_Current, &pSS->st_Next, sizeof(xbSND_tdst_StreamSettings));
	L_memset(&pSS->st_Next, 0, sizeof(xbSND_tdst_StreamSettings));


    fFxL = SND_f_GetVolFromAtt(pSS->st_Current.i_InitFxVolLeft);
    fFxR = SND_f_GetVolFromAtt(pSS->st_Current.i_InitFxVolRight);
	fFx = 0.5f*(fFxL + fFxR);
    if(fFx < 0.5f)
	{
		// 0% wet + 100% dry
        pSB->i_CurFxVol = -10000;
		pSB->i_CurNoFxVol = 0;
	}
	else if(fFx < 0.7f)
	{
		// 100% wet + 100% dry
        pSB->i_CurFxVol = 0;
		pSB->i_CurNoFxVol = 0; 
	}
	else
	{
		// 100% wet + 0% dry
        pSB->i_CurFxVol = 0;
		pSB->i_CurNoFxVol = 0;
	}

    pSS->en_State = xbStream_e_Reinit;
    pSS->b_PendingPlay = 1;
	pSS->ui_FileCurrSeek = pSS->st_Current.ui_StartSeek;
	pSS->ui_ChainSize = 0;
	pSS->ui_LoopSize = 0;
	pSS->ui_EndSize = 0;
	pSS->ui_LastReadResult = 0;
	pSS->i_CurrLoadingBufferIdx = 0;
	pSS->ui_WriteCursor = 0;
	pSS->f_ChainDelay = 0.0f;
    pSS->ui_StopPosition = -1;
    pSS->ui_StopBufferPosition = -1;
    
    
    pSS->ui_LoopingNb = 0;
    pSS->ui_DebugPlayPos = 0;

	LeaveCriticalSection(&xbSND_gx_InitSettingsSection);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hr = IDirectSoundBuffer_SetBufferData(pSB->pst_DSB, pSB->pv_RamBuffer, pSB->ui_BufferSize);
	xbSND_M_Assert(hr == DS_OK);

	for(j = 0; j < xbSND_Cte_NotifyNb; j++)
	{
		pSS->ast_Notification[j].dwOffset = ast_DsbPositionNotify[j].dwOffset = j * xbSND_Cte_NotificationSize;
		ast_DsbPositionNotify[j].hEventNotify = xbSND_gah_Event[xbSND_e_EventNotifyS0_A + j + (xbSND_Cte_NotifyNb * pSS->i_StreamId)];
		
        xbSND_M_BeginTraceBlock((asz_Log, "S%d notifypos %d", pSS->i_StreamId, ast_DsbPositionNotify[j].dwOffset));
		xbSND_M_EndTraceBlock();
	}

	hr = IDirectSoundBuffer_SetNotificationPositions(pSB->pst_DSB, xbSND_Cte_NotifyNb, ast_DsbPositionNotify);
	xbSND_M_Assert(hr == DS_OK);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	xbSND_M_BeginTraceBlock
	(
		(
			asz_Log, "pop on S%d pos %d size %d startpos %d - %d", pSS->i_StreamId, pSS->st_Current.ui_FilePosition, pSS->
				st_Current.ui_FileSize, pSS->st_Current.ui_StartSeek, pSB->i_CurVol 
		)
	);
	xbSND_M_EndTraceBlock();

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	xbSND_StreamReadFile(pSB, pSB->pv_RamBuffer, xbSND_Cte_NotificationSize);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void xbSND_StreamReadFile(SND_tdst_SoundBuffer *pSB, char *pc_Buff, unsigned int ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	xbSND_tdst_SoundStream	*pSS;
	OVERLAPPED				st_Overlapped;
	BOOL					b_Result;
	unsigned int			ui_ReadSize;
    int                     i_Retry;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pSS = pSB->pst_SS;

	

	if
	(
		pSS->st_Next.ui_ExitSeek
	&&	(
			(pSS->ui_FileCurrSeek <= pSS->st_Next.ui_ExitSeek)
		&&	(pSS->st_Next.ui_ExitSeek <= (pSS->ui_FileCurrSeek + ui_Size))
		)
	)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		unsigned int	ui_PlayPos;
		HRESULT			hr;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		/* chain now */
		ui_ReadSize = pSS->st_Next.ui_ExitSeek - pSS->ui_FileCurrSeek;
		pSS->ui_ChainSize = ui_Size - ui_ReadSize;
		pSS->ui_LoopSize = 0;
		pSS->ui_EndSize = 0;
        pSS->ui_StopPosition = -1;
        pSS->ui_StopBufferPosition = -1;


		xbSND_M_BeginTraceBlock((asz_Log, "S%d chaining - %d", pSS->i_StreamId, pSB->i_CurVol));
		xbSND_M_EndTraceBlock();

		hr = IDirectSoundBuffer8_GetCurrentPosition(pSB->pst_DSB, &ui_PlayPos, NULL);
		xbSND_M_Assert(hr == DS_OK);

		if(pSS->ui_WriteCursor > ui_PlayPos)
			pSS->f_ChainDelay = fLongToFloat(pSS->ui_WriteCursor - ui_PlayPos + ui_ReadSize);
		else
			pSS->f_ChainDelay = fLongToFloat(pSB->ui_BufferSize + pSS->ui_WriteCursor - ui_PlayPos + ui_ReadSize);

		pSS->f_ChainDelay = SND_ui_SizeToSample
			(
				SND_Cte_DefaultWaveFormat,
				pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels,
				pSS->f_ChainDelay
			);

		pSS->f_ChainDelay /= fLongToFloat(pSB->i_CurFrequency);
	}
	else
	{
		/* no chaining */
		if(pSS->st_Current.i_LoopNb)
		{
			if(pSS->st_Current.ui_LoopEndSeek > (pSS->ui_FileCurrSeek + ui_Size))
			{
				ui_ReadSize = ui_Size;
				pSS->ui_ChainSize = 0;
				pSS->ui_LoopSize = 0;
				pSS->ui_EndSize = 0;
                pSS->ui_StopPosition = -1;
                pSS->ui_StopBufferPosition = -1;
			}
			else
			{
				if(pSS->st_Current.i_LoopNb != -1) pSS->st_Current.i_LoopNb--;

				ui_ReadSize = pSS->st_Current.ui_LoopEndSeek - pSS->ui_FileCurrSeek;
				pSS->ui_LoopSize = ui_Size - ui_ReadSize;
				pSS->ui_ChainSize = 0;
				pSS->ui_EndSize = 0;
                pSS->ui_StopPosition = -1;
                pSS->ui_StopBufferPosition = -1;

				xbSND_M_BeginTraceBlock((asz_Log, "S%d looping -%d", pSS->i_StreamId, pSB->i_CurVol));
				xbSND_M_EndTraceBlock();
			}
		}
		else
		{
			if(pSS->st_Current.ui_FileSize > (pSS->ui_FileCurrSeek + ui_Size))
			{
				ui_ReadSize = ui_Size;
				pSS->ui_LoopSize = 0;
				pSS->ui_EndSize = 0;
				pSS->ui_ChainSize = 0;
			}
			else
			{
				ui_ReadSize = pSS->st_Current.ui_FileSize - pSS->ui_FileCurrSeek;
				pSS->ui_EndSize = ui_Size - ui_ReadSize;
				pSS->ui_LoopSize = 0;
				pSS->ui_ChainSize = 0;
                pSS->ui_StopPosition = -1;
                pSS->ui_StopBufferPosition = -1;

				xbSND_M_BeginTraceBlock((asz_Log, "S%d ending - %d", pSS->i_StreamId, pSB->i_CurVol));
				xbSND_M_EndTraceBlock();

				xbSND_M_BeginTraceBlock((asz_Log, "endsize %d ui_ReadSize %d", pSS->ui_EndSize, ui_ReadSize));
				xbSND_M_EndTraceBlock();
			}
		}
	}

	if(ui_ReadSize)
	{
        L_memset(&st_Overlapped, 0, sizeof(OVERLAPPED));
		st_Overlapped.hEvent = xbSND_gah_Event[xbSND_e_EventReadDoneS0 + pSS->i_StreamId];
		st_Overlapped.Offset = pSS->ui_FileCurrSeek + pSS->st_Current.ui_FilePosition;
		pSS->ui_LastReadResult = 0;
		
        i_Retry = 0;
        do
        {
            b_Result = xbSND_Overloaded_XBCompositeFile_Read
			    (
				    pSS->h_File,
				    pc_Buff, 
				    ui_ReadSize,
				    (LPDWORD) & pSS->ui_LastReadResult,
				    &st_Overlapped
			    );

            if(++i_Retry > xbSND_Cte_ReadRetryMax)
            {
                i_Retry = -1;
                break;
            }

        } while(b_Result && (pSS->ui_LastReadResult != ui_ReadSize));

		xbSND_M_Assert((!b_Result) || (pSS->ui_LastReadResult == ui_ReadSize));
		if(!b_Result)
		{
			xbSND_M_Assert(GetLastError() == ERROR_IO_PENDING);
		}
        
		xbSND_M_BeginTraceBlock((asz_Log, "S%d read %d oct - %d", pSS->i_StreamId, ui_ReadSize, pSB->i_CurVol));
		xbSND_M_EndTraceBlock();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_ThStreamReadDone(int _i_ID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundBuffer	*pSB;
	xbSND_tdst_SoundStream	*pSS;
	HRESULT					hr;
	OVERLAPPED				st_Overlapped;
	BOOL					b_Result;
	char					*pc_ReadBuffer;
	unsigned int			ui_ReadSize;
	DWORD					dwMode;
	DSBPOSITIONNOTIFY		ast_DsbPositionNotify[xbSND_Cte_NotifyNb];
    int                     i_Retry;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_ID < 0) return;
	if(_i_ID > xbSND_Cte_StreamBufferNb) return;

    

	pSB = xbSND_ga_StreamSoundBufferList[_i_ID];
	pSS = xbSND_ga_StreamSoundBufferList[_i_ID]->pst_SS;

	pSS->ui_WriteCursor += pSS->ui_LastReadResult;
	if(pSS->ui_WriteCursor >= pSB->ui_BufferSize) pSS->ui_WriteCursor -= pSB->ui_BufferSize;

	if(pSS->ui_ChainSize)
	{

		/*$1- chain ? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ui_ReadSize = pSS->ui_ChainSize;

        if(ui_ReadSize > (pSS->st_Next.ui_FileSize - pSS->st_Next.ui_StartSeek))
            ui_ReadSize = pSS->st_Next.ui_FileSize - pSS->st_Next.ui_StartSeek;

		EnterCriticalSection(&xbSND_gx_InitSettingsSection);
		L_memcpy(&pSS->st_Current, &pSS->st_Next, sizeof(xbSND_tdst_StreamSettings));
		L_memset(&pSS->st_Next, 0, sizeof(xbSND_tdst_StreamSettings));
		pSS->ui_FileCurrSeek = pSS->st_Current.ui_StartSeek;
		pSS->ui_ChainSize = 0;
		pSS->ui_LoopSize = 0;
		pSS->ui_EndSize = 0;
		pSS->ui_LastReadResult = 0;
        pSS->ui_StopPosition = -1;
        pSS->ui_StopBufferPosition = -1;
		LeaveCriticalSection(&xbSND_gx_InitSettingsSection);

		pc_ReadBuffer = pSB->pv_RamBuffer;
		pc_ReadBuffer += pSS->i_CurrLoadingBufferIdx * xbSND_Cte_NotificationSize;
		pc_ReadBuffer += xbSND_Cte_NotificationSize - ui_ReadSize;

        L_memset(&st_Overlapped, 0, sizeof(OVERLAPPED));
		st_Overlapped.hEvent = xbSND_gah_Event[xbSND_e_EventReadDoneS0 + _i_ID];
		st_Overlapped.Offset = pSS->ui_FileCurrSeek + pSS->st_Current.ui_FilePosition;
		pSS->ui_LastReadResult = 0;

        i_Retry = 0;
        do
        {
		    b_Result = xbSND_Overloaded_XBCompositeFile_Read
			    (
				    pSS->h_File,
				    pc_ReadBuffer,
				    ui_ReadSize,
				    (LPDWORD) & pSS->ui_LastReadResult,
				    &st_Overlapped
			    );
            if(++i_Retry > xbSND_Cte_ReadRetryMax)
            {
                i_Retry = -1;
                break;
            }

        } while(b_Result && (pSS->ui_LastReadResult != ui_ReadSize));

        xbSND_M_Assert((!b_Result) || (pSS->ui_LastReadResult == ui_ReadSize));
		if(!b_Result)
		{
			xbSND_M_Assert(GetLastError() == ERROR_IO_PENDING);
		}

		xbSND_M_BeginTraceBlock((asz_Log, "S%d chain pop - %d", pSS->i_StreamId, pSB->i_CurVol));
		xbSND_M_EndTraceBlock();

		return;
	}
	else if(pSS->st_Next.ui_FilePosition && !pSS->st_Next.ui_ExitSeek)
	{

		/*$1- new position + no pending chain cmd => canceling ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		xbSND_M_BeginTraceBlock((asz_Log, "S%d cancel from readdone - %d", pSS->i_StreamId, pSB->i_CurVol));
		xbSND_M_EndTraceBlock();
		return;
	}
	else if(pSS->ui_LoopSize)
	{

		/*$1- loop ? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ui_ReadSize = pSS->ui_LoopSize;
		pSS->ui_EndSize = 0;
		pSS->ui_LoopSize = 0;
		pSS->ui_ChainSize = 0;
        pSS->ui_StopPosition = -1;
        pSS->ui_StopBufferPosition = -1;


		pSS->ui_FileCurrSeek = pSS->st_Current.ui_LoopBeginSeek;

        L_memset(&st_Overlapped, 0, sizeof(OVERLAPPED));
		st_Overlapped.hEvent = xbSND_gah_Event[xbSND_e_EventReadDoneS0 + _i_ID];
		st_Overlapped.Offset = pSS->ui_FileCurrSeek + pSS->st_Current.ui_FilePosition;

		pSS->ui_LastReadResult = 0;

		pc_ReadBuffer = pSB->pv_RamBuffer;
		pc_ReadBuffer += pSS->i_CurrLoadingBufferIdx * xbSND_Cte_NotificationSize;
		pc_ReadBuffer += xbSND_Cte_NotificationSize - ui_ReadSize;

		b_Result = XBCompositeFile_Read
			(
				pSS->h_File,
				pc_ReadBuffer,
				ui_ReadSize,
				(LPDWORD) & pSS->ui_LastReadResult,
				&st_Overlapped
			);
		xbSND_M_Assert((!b_Result) || (pSS->ui_LastReadResult == ui_ReadSize));
		if(!b_Result)
		{
			xbSND_M_Assert(GetLastError() == ERROR_IO_PENDING);
		}

		xbSND_M_BeginTraceBlock((asz_Log, "S%d end looping %d oct - %d", pSS->i_StreamId, ui_ReadSize, pSB->i_CurVol));
		xbSND_M_EndTraceBlock();
		return;
	}
	else if(pSS->ui_EndSize)
	{

		/*$1- end hit ? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ui_ReadSize = pSS->ui_EndSize;
		pSS->ui_EndSize = 0;
		pSS->ui_LoopSize = 0;
		pSS->ui_ChainSize = 0;

		/* rewind */
		pSS->ui_FileCurrSeek = pSS->st_Current.ui_FileSize;

		/* clean buffer last part */
		pc_ReadBuffer = pSB->pv_RamBuffer;
		pc_ReadBuffer += pSS->i_CurrLoadingBufferIdx * xbSND_Cte_NotificationSize;
		pc_ReadBuffer += xbSND_Cte_NotificationSize - ui_ReadSize;

		L_memset(pc_ReadBuffer, 0, ui_ReadSize);

		xbSND_M_BeginTraceBlock((asz_Log, "S%d endending %d - %d", pSS->i_StreamId, ui_ReadSize, pSB->i_CurVol));
		xbSND_M_EndTraceBlock();

		pSS->ui_WriteCursor += ui_ReadSize;
		if(pSS->ui_WriteCursor >= pSB->ui_BufferSize) pSS->ui_WriteCursor -= pSB->ui_BufferSize;

		/* resize the buffer/notification positions */
		ui_ReadSize = xbSND_Cte_NotificationSize - ui_ReadSize;
		ui_ReadSize += (pSS->i_CurrLoadingBufferIdx * xbSND_Cte_NotificationSize);

		xbSND_M_BeginTraceBlock((asz_Log, "S%d resizing to %d oct - %d", pSS->i_StreamId, ui_ReadSize, pSB->i_CurVol));
		xbSND_M_EndTraceBlock();

		pSS->ast_Notification[0].dwOffset = ast_DsbPositionNotify[0].dwOffset = ui_ReadSize;	/* not used, in fact the buffer will stop itself */
		ast_DsbPositionNotify[0].hEventNotify = xbSND_gah_Event[xbSND_e_EventEndHitS0 + pSS->i_StreamId];
        pSS->ast_Notification[1].dwOffset = -1;

        pSS->ui_StopPosition = ast_DsbPositionNotify[0].dwOffset;
        pSS->ui_StopBufferPosition = pSS->i_CurrLoadingBufferIdx * xbSND_Cte_NotificationSize;


        xbSND_M_BeginTraceBlock((asz_Log, "S%d notifypos %d - %d", pSS->i_StreamId, ast_DsbPositionNotify[0].dwOffset, pSB->i_CurVol ));
		xbSND_M_EndTraceBlock();

        //if((LOA_ul_BinKey & 0x0000FFFF) != 0x0000eabc)
        {
            unsigned int ui_PlayPos;
		hr = IDirectSoundBuffer8_GetCurrentPosition(pSB->pst_DSB, &ui_PlayPos, NULL);
		xbSND_M_Assert(hr == DS_OK);

        xbSND_M_BeginTraceBlock((asz_Log, "S%d currpos %d", pSS->i_StreamId, ui_PlayPos));
		xbSND_M_EndTraceBlock();

        hr = IDirectSoundBuffer_SetNotificationPositions(pSB->pst_DSB, 1, ast_DsbPositionNotify);
		xbSND_M_Assert(hr == DS_OK);
        }

		pSS->en_State = xbStream_e_Stopping;
	}
	else
	{

		/*$1- normal playing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pSS->ui_FileCurrSeek += pSS->ui_LastReadResult;
	}

	if(pSS->b_PendingPlay)
	{
		xbSND_M_TraceS(_i_ID, "Init->Play");

        pSS->b_PendingPlay = 0;

		/* new settings */
		//*
        xbSND_SB_SetVolume(pSB, pSS->st_Current.i_InitVol);
        //IDirectSoundBuffer8_SetFrequency(pSB->pst_DSB, pSS->st_Current.i_InitFreq);
		//xbSND_SB_SetFrequency(pSB, pSS->st_Current.i_InitFreq);
		xbSND_SB_SetPan(pSB, pSS->st_Current.i_InitPan, 0);
        //*/

		if(pSS->st_Current.ui_Flags & SND_Cul_SBC_PlayFxA)
			xbSND_i_FxAddSB(SND_Cte_FxCoreA, pSB);
		else if(pSS->st_Current.ui_Flags & SND_Cul_SBC_PlayFxB)
			xbSND_i_FxAddSB(SND_Cte_FxCoreB, pSB);
		else
			xbSND_i_FxDelSB(pSB);

		/* rewind */
		pSS->ui_WriteCursor = 0;
        
        pSS->ui_LoopingNb = 0;
        pSS->ui_DebugPlayPos = 0;

		hr = IDirectSoundBuffer8_SetCurrentPosition(pSB->pst_DSB, 0);
		xbSND_M_Assert(hr == DS_OK);

		/* go ahead */
		if(pSS->st_Current.i_LoopNb || (pSS->st_Current.ui_FileSize > pSB->ui_BufferSize))
        {
			dwMode = DSBPLAY_LOOPING;
        }
        else
        {
			dwMode = DSBPLAY_FROMSTART;
        }

		hr = IDirectSoundBuffer8_Play(pSB->pst_DSB, 0, 0, dwMode);
		xbSND_M_Assert(hr == DS_OK);
		if(hr != DS_OK)
			pSS->en_State = xbStream_e_Error;
		else
			pSS->en_State = xbStream_e_Playing;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_ThStreamNotify(int _i_StreamId, int _i_BufferId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundBuffer	*pSB;
	xbSND_tdst_SoundStream	*pSS;
	BOOL					b_Result;
	HRESULT					hr;
	unsigned int			ui_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- get input --------------------------------------------------------------------------------------------------*/

	if(_i_StreamId < 0) return;
	if(_i_StreamId > xbSND_Cte_StreamBufferNb) return;
	if(_i_BufferId < 0) return;
	if(_i_BufferId > xbSND_Cte_NotifyNb) return;

	pSB = xbSND_ga_StreamSoundBufferList[_i_StreamId];
	pSS = xbSND_ga_StreamSoundBufferList[_i_StreamId]->pst_SS;

	/* if reinit without chain => cancel */
	if(pSS->st_Next.ui_FilePosition && !pSS->st_Next.ui_ExitSeek) return;

	/* if stopping => cancel */
	if(pSS->en_State == xbStream_e_Stopping) return;

{
unsigned int ui_PlayPos;
hr = IDirectSoundBuffer8_GetCurrentPosition(pSB->pst_DSB, &ui_PlayPos, NULL);
xbSND_M_Assert(hr == DS_OK);

if(ui_PlayPos < pSS->ast_Notification[_i_BufferId].dwOffset) return;
if((ui_PlayPos - pSS->ast_Notification[_i_BufferId].dwOffset) >= 10240) return;

xbSND_M_BeginTraceBlock((asz_Log, "S%d Notify%d - pos %d / %d", _i_StreamId, _i_BufferId, ui_PlayPos, pSS->ast_Notification[_i_BufferId].dwOffset));
xbSND_M_EndTraceBlock();
}

	/* if buffer error => cancel */
	if(pSS->i_CurrLoadingBufferIdx != _i_BufferId) return;

    pSS->ui_LoopingNb++;

//	xbSND_M_BeginTraceBlock((asz_Log, "S%d Notify%d - %d", _i_StreamId, _i_BufferId, pSB->i_CurVol));
//	xbSND_M_EndTraceBlock();
	xbSND_M_Assert(pSS->i_CurrLoadingBufferIdx == _i_BufferId);

	/*$2- update -----------------------------------------------------------------------------------------------------*/

	pSS->i_CurrLoadingBufferIdx = 1 - _i_BufferId;
	if(pSS->en_State == xbStream_e_Playing)
	{
		xbSND_StreamReadFile
		(
			pSB,
			(char *) pSB->pv_RamBuffer + (pSS->i_CurrLoadingBufferIdx * xbSND_Cte_NotificationSize),
			xbSND_Cte_NotificationSize
		);
	}
	else
	{
		xbSND_M_BeginTraceBlock((asz_Log, "S%d bad state %d - %d", pSS->i_StreamId, pSS->en_State, pSB->i_CurVol));
		xbSND_M_EndTraceBlock();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_ThStreamEndHit(int _i_StreamId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundBuffer	*pSB;
	xbSND_tdst_SoundStream	*pSS;
    unsigned int           ui_PlayPos;
    HRESULT                 hr;
    //DSBPOSITIONNOTIFY		ast_DsbPositionNotify[xbSND_Cte_NotifyNb];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_StreamId < 0) return;
	if(_i_StreamId > xbSND_Cte_StreamBufferNb) return;

	pSB = xbSND_ga_StreamSoundBufferList[_i_StreamId];
	pSS = xbSND_ga_StreamSoundBufferList[_i_StreamId]->pst_SS;


    xbSND_M_BeginTraceBlock((asz_Log, "S%d HitEnd - %d", _i_StreamId, pSB->i_CurVol));
	xbSND_M_EndTraceBlock();



    if(pSS->ui_StopPosition != -1)
    {
		hr = IDirectSoundBuffer8_GetCurrentPosition(pSB->pst_DSB, &ui_PlayPos, NULL);
		xbSND_M_Assert(hr == DS_OK);

        if(pSS->ui_StopBufferPosition != -1)
        {

            if(
                (ui_PlayPos >= pSS->ui_StopBufferPosition) && 
                (ui_PlayPos <= (pSS->ui_StopBufferPosition + xbSND_Cte_NotificationSize))
                )
            {
                xbSND_M_BeginTraceBlock((asz_Log, "S%d HitEnd -> reaches the half buffer", _i_StreamId));
	            xbSND_M_EndTraceBlock();

                pSS->ui_StopBufferPosition = -1; // hit the stop half-buffer
            }
            else
            {
                xbSND_M_BeginTraceBlock((asz_Log, "S%d HitEnd -> canceled, doesnt reach the half buffer", _i_StreamId));
	            xbSND_M_EndTraceBlock();

		        /*ast_DsbPositionNotify[0].dwOffset = pSS->ui_StopPosition;
		        ast_DsbPositionNotify[0].hEventNotify = xbSND_gah_Event[xbSND_e_EventEndHitS0 + pSS->i_StreamId];
                hr = IDirectSoundBuffer_SetNotificationPositions(pSB->pst_DSB, 1, ast_DsbPositionNotify);
		        xbSND_M_Assert(hr == DS_OK);*/
                
                return ; // doesnt reach the stop half buffer, cancel the stop order
            }
        } 

        if(ui_PlayPos < pSS->ui_StopPosition)
        {
            xbSND_M_BeginTraceBlock((asz_Log, "S%d HitEnd -> canceled, doesnt reach the stop position", _i_StreamId));
	        xbSND_M_EndTraceBlock();

		    /*ast_DsbPositionNotify[0].dwOffset = pSS->ui_StopPosition;
		    ast_DsbPositionNotify[0].hEventNotify = xbSND_gah_Event[xbSND_e_EventEndHitS0 + pSS->i_StreamId];
            hr = IDirectSoundBuffer_SetNotificationPositions(pSB->pst_DSB, 1, ast_DsbPositionNotify);
		    xbSND_M_Assert(hr == DS_OK);*/
            
            return; // stop position not reached
        }
    }

    
	IDirectSoundBuffer8_Stop(pSB->pst_DSB);
	pSS->en_State = xbStream_e_Stopped;
    pSS->b_PendingPlay = 0;
}

void xbSND_StreamStop(SND_tdst_SoundBuffer *_pst_SB)
{
    xbSND_ThStreamEndHit(_pst_SB->pst_SS->i_StreamId);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_ShutDownStreams(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	SND_tdst_SoundBuffer	*pSB;
	HRESULT					hr;
	DWORD					dwStatus;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < xbSND_Cte_StreamBufferNb; i++)
	{
		pSB = xbSND_ga_StreamSoundBufferList[i];
		hr = IDirectSoundBuffer8_Stop(pSB->pst_DSB);
		xbSND_M_Assert(hr == DS_OK);

		do
		{
			hr = IDirectSoundBuffer8_GetStatus(pSB->pst_DSB, &dwStatus);
			xbSND_M_Assert(hr == DS_OK);
		} while(dwStatus & DSBSTATUS_PLAYING);

		L_memset(pSB->pv_RamBuffer, 0, pSB->ui_BufferSize);

		L_memset(&pSB->pst_SS->st_Next, 0, sizeof(xbSND_tdst_StreamSettings));
		L_memset(&pSB->pst_SS->st_Next, 0, sizeof(xbSND_tdst_StreamSettings));
		pSB->pst_SS->en_State = xbStream_e_Stopped;
	    pSB->pst_SS->b_PendingPlay = 0;

		pSB->pst_SS->ui_FileCurrSeek = 0;
		pSB->pst_SS->ui_LastReadResult = 0;
		pSB->pst_SS->ui_ChainSize = 0;
		pSB->pst_SS->ui_LoopSize = 0;
		pSB->pst_SS->ui_EndSize = 0;
		pSB->pst_SS->i_CurrLoadingBufferIdx = 0;
		pSB->pst_SS->ui_WriteCursor = 0;
        pSB->pst_SS->ui_LoopingNb = 0;
        pSB->pst_SS->ui_DebugPlayPos = 0;
        pSB->pst_SS->ui_StopPosition = -1;
        pSB->pst_SS->ui_StopBufferPosition = -1;

	    pSB->i_CurFrequency = SND_Cte_StreamFrequency;
		hr = IDirectSoundBuffer8_SetFrequency(pSB->pst_DSB, SND_Cte_StreamFrequency);
		xbSND_M_Assert(hr == DS_OK);

    }

    xbSND_FxReinit();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_StreamGetStatus(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Status)
{
	/*~~~~~~~*/
	HRESULT hr;
    DWORD   dwStatus;
	/*~~~~~~~*/

	*_pi_Status = 0;

	if(_pst_SB->pst_SS->st_Next.ui_FilePosition)
	{
		*_pi_Status |= SND_Cul_SBS_Playing;
		if(_pst_SB->pst_SS->st_Next.i_LoopNb) *_pi_Status |= SND_Cul_SBC_PlayLooping;
		return;
	}

	switch(_pst_SB->pst_SS->en_State)
	{
	case xbStream_e_Stopping:
        if(!_pst_SB->pst_SS->b_PendingPlay)
        {
	        hr = IDirectSoundBuffer8_GetStatus(_pst_SB->pst_DSB, &dwStatus);
	        if((dwStatus & DSBSTATUS_PLAYING) == 0)
            {
                //if((LOA_ul_BinKey & 0x0000FFFF) != 0x0000eabc)
                {
                xbSND_ThStreamEndHit(_pst_SB->pst_SS->i_StreamId);        
                }
                break;
            }
        }

    case xbStream_e_Playing:
	case xbStream_e_Reinit:
		*_pi_Status |= SND_Cul_SBS_Playing;
		if(_pst_SB->pst_SS->st_Current.ui_Flags & SND_Cul_SBC_PlayLooping) *_pi_Status |= SND_Cul_SBC_PlayLooping;
		break;

	default:
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_Stream_GetCurrPos(SND_tdst_SoundBuffer *pSB, int *_pi_Pos, int *_pi_Write)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			ui_PlayPos, ui_WritePos;
	int						hr;
	xbSND_tdst_SoundStream	*pSS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_pi_Write = 0;
	*_pi_Pos = 0;

	if(!pSB) return;
	if(!pSB->pst_SS) return;
	pSS = pSB->pst_SS;

	hr = IDirectSoundBuffer8_GetCurrentPosition(pSB->pst_DSB, &ui_PlayPos, &ui_WritePos);
	xbSND_M_Assert(hr == DS_OK);


    ui_PlayPos += (pSS->ui_LoopingNb * SND_Cte_MaxBufferSize); // BUG : on devrait avoir 1/2 buffer en taille ici
     while(ui_PlayPos > pSS->st_Current.ui_LoopEndSeek)
    {
        ui_PlayPos -= (pSS->st_Current.ui_LoopEndSeek - pSS->st_Current.ui_LoopBeginSeek);
    }
    
    // BUG : le debugplaypos n'a pas de sens si on change de stream
    // en mode chain
    if(pSS->ui_DebugPlayPos > ui_PlayPos)
        *(unsigned int *) _pi_Pos = pSS->ui_DebugPlayPos;
    else
        *(unsigned int *) _pi_Pos =ui_PlayPos ;

        pSS->ui_DebugPlayPos = ui_PlayPos ;
    

	*_pi_Write = *(int *) &pSS->ui_FileCurrSeek;

	*(unsigned int *) _pi_Pos = SND_ui_SizeToSample
		(
			SND_Cte_DefaultWaveFormat,
			pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels,
			*(unsigned int *) _pi_Pos
		);
	*(unsigned int *) _pi_Write = SND_ui_SizeToSample
		(
			SND_Cte_DefaultWaveFormat,
			pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels,
			*(unsigned int *) _pi_Write
		);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void xbSND_StreamStopPair(int id)
{
	/*~~~~~~~~~*/
	int fellowid;
	/*~~~~~~~~~*/

	fellowid = id & 0xFFFFFFFE;
	IDirectSoundBuffer8_Stop(xbSND_ga_StreamSoundBufferList[fellowid]->pst_DSB);
	xbSND_ga_StreamSoundBufferList[fellowid]->pst_SS->en_State = xbStream_e_Stopped;
	xbSND_ga_StreamSoundBufferList[fellowid]->pst_SS->b_PendingPlay = 0;
	xbSND_i_FxDelSB(xbSND_ga_StreamSoundBufferList[fellowid]);

	fellowid = id | 0x00000001;
	IDirectSoundBuffer8_Stop(xbSND_ga_StreamSoundBufferList[fellowid]->pst_DSB);
	xbSND_ga_StreamSoundBufferList[fellowid]->pst_SS->en_State = xbStream_e_Stopped;
	xbSND_ga_StreamSoundBufferList[fellowid]->pst_SS->b_PendingPlay = 0;
	xbSND_i_FxDelSB(xbSND_ga_StreamSoundBufferList[fellowid]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_StreamChainDelayGet(SND_tdst_SoundBuffer *pSB, float *ptime)
{
	*ptime = pSB->pst_SS->f_ChainDelay;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_StreamLoopCountGet(SND_tdst_SoundBuffer *pSB, int *pLoop)
{
	*pLoop = pSB->pst_SS->st_Current.i_LoopNb;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_StreamChain
(
	SND_tdst_SoundBuffer	*_pSB,			/* SB */
	int						_i_Flag,		/* flag */
	int						_i_Loop,		/* loop number */
	unsigned int			_ui_Exit,		/* exit point (oct) */
	unsigned int			_ui_StartPos,	/* StartOffset (oct) */
	unsigned int			_ui_Size,		/* DataSize (oct) */
	unsigned int			_ui_Position,	/* DataPosition (oct) */
	unsigned int			_ui_LoopBegin,	/* LoopBeginOffset (oct) */
	unsigned int			_ui_LoopEnd		/* LoopEndOffset (oct) */
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	xbSND_tdst_StreamSettings	*pNext;
	xbSND_tdst_StreamSettings	*pCurr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    if(!_pSB) return;
	if(!_pSB->pst_SS) return;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    round size
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	M_RoundSize(_ui_Exit, _pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels);
	M_RoundSize(_ui_StartPos, _pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels);
	M_RoundSize(_ui_Size, _pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels);
	M_RoundSize(_ui_LoopBegin, _pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels);
	M_RoundSize(_ui_LoopEnd, _pSB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels);
    
    if(_ui_StartPos>=_ui_Size)
        _ui_StartPos = 0;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    register the order
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	pNext = &_pSB->pst_SS->st_Next;
	pCurr = &_pSB->pst_SS->st_Current;
	if(pNext->ui_FilePosition == _ui_Position) return;

	EnterCriticalSection(&xbSND_gx_InitSettingsSection);
	L_memcpy(pNext, pCurr, sizeof(xbSND_tdst_StreamSettings));

	pNext->ui_Flags = *(unsigned int *) &_i_Flag;
	pNext->ui_FilePosition = _ui_Position;
	pNext->ui_FileSize = _ui_Size;
	pNext->ui_StartSeek = _ui_StartPos;
	pNext->ui_LoopBeginSeek = _ui_LoopBegin;
	pNext->ui_LoopEndSeek = _ui_LoopEnd;
	pNext->ui_ExitSeek = _ui_Exit;
	pNext->i_LoopNb = _i_Loop;
	LeaveCriticalSection(&xbSND_gx_InitSettingsSection);

	xbSND_M_BeginTraceBlock
	(
		(
			asz_Log, "chain on S%d pos %d size %d startpos %d - %d", _pSB->pst_SS->i_StreamId, _ui_Position, _ui_Size,
				_ui_StartPos, _pSB->i_CurVol
		)
	);
	xbSND_M_EndTraceBlock();
}

/*$4
 ***********************************************************************************************************************
    PREFETCH
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_StreamGetPrefetchStatus(unsigned int *a)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	xbSND_tdst_Prefetch *pst_Prefetch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	memset(a, 0, sizeof(unsigned int) * SND_Cte_StreamPrefetchMax);
	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = xbSND_gst_PrefetchList + i;
		if(pst_Prefetch->en_State != e_PrefetchWaitingForUse) continue;
		a[i] = pst_Prefetch->ui_PrefetchPosition;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_StreamPrefetch(unsigned int _ui_Position, unsigned int _ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	xbSND_tdst_Prefetch *pst_Prefetch;
	OVERLAPPED			stOVERLAPPED;
	BOOL				b_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_ui_Position) return;
	if(!_ui_Size) return;

	
	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = xbSND_gst_PrefetchList + i;
		if(pst_Prefetch->en_State != e_PrefetchFree) continue;

		xbSND_M_Assert(pst_Prefetch->h_File != INVALID_HANDLE_VALUE);
		xbSND_M_Assert(pst_Prefetch->pc_RamBuffer);
		if((pst_Prefetch->h_File == INVALID_HANDLE_VALUE) || !pst_Prefetch->pc_RamBuffer) return;

		pst_Prefetch->ui_PrefetchPosition = _ui_Position;
		pst_Prefetch->ui_PrefetchSize = min(_ui_Size, SND_Cte_MaxBufferSize / 2);
		pst_Prefetch->en_State = e_PrefetchLoading;
        pst_Prefetch->ui_RetryCount = 0;

		xbSND_M_BeginTraceBlock
		(
			(
				asz_Log, "prefetching pos %d size %d handler %x", pst_Prefetch->ui_PrefetchPosition, pst_Prefetch->
					ui_PrefetchSize, (int) pst_Prefetch->h_File
			)
		);
		xbSND_M_EndTraceBlock();

		xbSND_gi_PendingPrefetch++;
        L_memset(&stOVERLAPPED, 0, sizeof(OVERLAPPED));
		stOVERLAPPED.hEvent = xbSND_gah_Event[xbSND_e_EventPrefetch0Done + i];
		stOVERLAPPED.Offset = pst_Prefetch->ui_PrefetchPosition;

		L_memset(pst_Prefetch->pc_RamBuffer, 0, SND_Cte_MaxBufferSize / 2);
		pst_Prefetch->ui_ReadSizeResult = 0;

		b_Result = XBCompositeFile_Read
			(
				pst_Prefetch->h_File,
				pst_Prefetch->pc_RamBuffer,
				pst_Prefetch->ui_PrefetchSize,
				(LPDWORD) & pst_Prefetch->ui_ReadSizeResult,
				&stOVERLAPPED
			);

		xbSND_M_Assert((!b_Result) || (pst_Prefetch->ui_ReadSizeResult == pst_Prefetch->ui_PrefetchSize));
		if(!b_Result)
		{
			xbSND_M_Assert(GetLastError() == ERROR_IO_PENDING);
		}
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_ThPrefetchDone(int _i_PrefetchId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	xbSND_tdst_Prefetch *pst_Prefetch;
	OVERLAPPED			stOVERLAPPED;
	BOOL				b_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_PrefetchId > SND_Cte_StreamPrefetchMax) return;
	if(_i_PrefetchId < 0) return;

	pst_Prefetch = xbSND_gst_PrefetchList + _i_PrefetchId;
	xbSND_M_Assert((pst_Prefetch->en_State == e_PrefetchLoading) || (pst_Prefetch->en_State == e_PrefetchLost));

	xbSND_M_BeginTraceBlock((asz_Log, "prefetch done pos %d size %d ", pst_Prefetch->ui_PrefetchPosition, pst_Prefetch->ui_PrefetchSize));
	xbSND_M_EndTraceBlock();

	switch(pst_Prefetch->en_State)
	{
	case e_PrefetchLoading:
		xbSND_M_Assert(pst_Prefetch->ui_ReadSizeResult == pst_Prefetch->ui_PrefetchSize);
        if(pst_Prefetch->ui_ReadSizeResult == pst_Prefetch->ui_PrefetchSize)
        {
            // read done : sucess
		    pst_Prefetch->en_State = e_PrefetchWaitingForUse;
        }
        else if(pst_Prefetch->ui_RetryCount < 2)
        {
            // read failed : retry
            pst_Prefetch->ui_RetryCount++;

		    xbSND_gi_PendingPrefetch++;
            L_memset(&stOVERLAPPED, 0, sizeof(OVERLAPPED));
		    stOVERLAPPED.hEvent = xbSND_gah_Event[xbSND_e_EventPrefetch0Done + _i_PrefetchId];
		    stOVERLAPPED.Offset = pst_Prefetch->ui_PrefetchPosition;

		    L_memset(pst_Prefetch->pc_RamBuffer, 0, SND_Cte_MaxBufferSize / 2);
		    pst_Prefetch->ui_ReadSizeResult = 0;

		    b_Result = XBCompositeFile_Read
			    (
				    pst_Prefetch->h_File,
				    pst_Prefetch->pc_RamBuffer,
				    pst_Prefetch->ui_PrefetchSize,
				    (LPDWORD) & pst_Prefetch->ui_ReadSizeResult,
				    &stOVERLAPPED
			    );

		    xbSND_M_Assert((!b_Result) || (pst_Prefetch->ui_ReadSizeResult == pst_Prefetch->ui_PrefetchSize));
		    if(!b_Result)
		    {
			    xbSND_M_Assert(GetLastError() == ERROR_IO_PENDING);
		    }
        }
        else
        {
            // read failed : too many retrying
            pst_Prefetch->en_State = e_PrefetchFree;
        }
		break;

	case e_PrefetchLost:
		pst_Prefetch->en_State = e_PrefetchFree;
		break;

	default:
		pst_Prefetch->en_State = e_PrefetchFree;
		xbSND_M_Assert(0);
		break;
	}

	xbSND_gi_PendingPrefetch--;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_StreamPrefetchArray(unsigned int *_pui_Position, unsigned int *_pui_Size)
{
	/*~~*/
	int i;
	/*~~*/

	xbSND_gi_PendingPrefetch = 0;
	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		if(_pui_Position[i])
		{
			xbSND_StreamPrefetch(_pui_Position[i], _pui_Size[i]);
		}

		_pui_Position[i] = 0;
	}

	while(xbSND_gi_PendingPrefetch)
	{
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static unsigned int xbSND_ui_IsStreamPrefetched(char *_pc_Buff, unsigned int _ui_Seek, unsigned int _ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	xbSND_tdst_Prefetch *pst_Prefetch;
	unsigned int		uiOffset, uiSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = xbSND_gst_PrefetchList + i;
		if(pst_Prefetch->en_State == e_PrefetchFree) continue;
		if(_ui_Seek  < pst_Prefetch->ui_PrefetchPosition) continue;
		if((pst_Prefetch->ui_PrefetchPosition + pst_Prefetch->ui_PrefetchSize) < _ui_Seek) continue;


		switch(pst_Prefetch->en_State)
		{
		case e_PrefetchLoading:
			pst_Prefetch->en_State = e_PrefetchLost;
			return 0;

		case e_PrefetchWaitingForUse:
			uiOffset = (_ui_Seek - pst_Prefetch->ui_PrefetchPosition);
			uiSize = min(pst_Prefetch->ui_PrefetchSize - uiOffset, _ui_Size);
			L_memcpy(_pc_Buff, pst_Prefetch->pc_RamBuffer + uiOffset, uiSize);
			pst_Prefetch->en_State = e_PrefetchFree;
			xbSND_M_BeginTraceBlock
			(
				(
					asz_Log, "use prefetch pos %d size %d", pst_Prefetch->ui_PrefetchPosition, uiSize
				)
			);
			xbSND_M_EndTraceBlock();
			return uiSize;

		default:
			pst_Prefetch->en_State = e_PrefetchFree;
			xbSND_M_Assert(0);
			return 0;
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_StreamFlush(unsigned int _ui_Position)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	xbSND_tdst_Prefetch *pst_Prefetch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = xbSND_gst_PrefetchList + i;
		if(pst_Prefetch->en_State == e_PrefetchFree) continue;
		if(pst_Prefetch->ui_PrefetchPosition != _ui_Position) continue;

		xbSND_M_BeginTraceBlock((asz_Log, "flush pos %d size %d", pst_Prefetch->ui_PrefetchPosition, pst_Prefetch->ui_PrefetchSize));
		xbSND_M_EndTraceBlock();

		switch(pst_Prefetch->en_State)
		{
		case e_PrefetchLoading:

			/*$1- prefetch is lost cause it is not finished ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Prefetch->en_State = e_PrefetchLost;
			break;

		case e_PrefetchWaitingForUse:
			pst_Prefetch->en_State = e_PrefetchFree;
			break;

		default:
			pst_Prefetch->en_State = e_PrefetchFree;
			xbSND_M_Assert(0);
			break;
		}
	}
}

DWORD	xbSND_gui_PrefetchedDummySize;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL xbSND_Overloaded_XBCompositeFile_Read
(
	XBCompositeFile *file,
	char			*buffer,
	DWORD			bytesToRead,
	DWORD			*bytesRead,
	OVERLAPPED		*overlapped
)
{
	*bytesRead = xbSND_ui_IsStreamPrefetched(buffer, overlapped->Offset, bytesToRead);
	bytesToRead -= (*bytesRead);
	overlapped->Offset += (*bytesRead);
	buffer += (*bytesRead);

	if(bytesToRead)
	{
		*bytesRead = *bytesRead + bytesToRead;	/* assume we will read all data needed */
		return XBCompositeFile_Read(file, buffer, bytesToRead, &xbSND_gui_PrefetchedDummySize, overlapped);
	}
	else
	{
		if(overlapped && overlapped->hEvent)
			SetEvent(overlapped->hEvent);
		return TRUE;
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

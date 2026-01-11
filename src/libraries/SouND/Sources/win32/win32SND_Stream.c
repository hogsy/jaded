/*$T win32SND_Stream.c GC 1.138 06/29/04 08:40:45 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef PCWIN_TOOL

/*$4
 ***********************************************************************************************************************
    HEADERS
 ***********************************************************************************************************************
 */

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/win32/win32SNDdebug.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDconv_xboxadpcm.h"
#include "SouND/Sources/SNDconv_pcretailadpcm.h"

#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGio.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "LINks/LINKmsg.h"


/*$4
 ***********************************************************************************************************************
    MACROS
 ***********************************************************************************************************************
 */

#define win32SND_Cte_NotifyNb		4
#define win32SND_Cte_StreamIdNb	(2 * SND_Cte_MaxSimultaneousStream)
#define win32SND_Cte_TimeOut		5000

#define M_CheckTimeOut(_a, _str) \
	do \
	{ \
		if(_a != WAIT_OBJECT_0) LINK_PrintStatusMsg("[ERROR] timeout overlaps (sound module), action :"_str); \
	} while(0);

#define M_IsLooping(loop)	(((loop) > 0) || ((loop) == -1))
#define M_InitLock() \
	do \
	{ \
		sh_Lock = (HANDLE) 0; \
		sh_Lock = CreateEvent(NULL, FALSE, FALSE, NULL); \
		ERR_X_Assert(sh_Lock); \
		SetEvent(sh_Lock); \
	} while(0)
#define M_BeginCriticalSection() \
	do \
	{ \
		WaitForSingleObject(sh_Lock, INFINITE); \
	} while(0)
#define M_EndCriticalSection() \
	do \
	{ \
		SetEvent(sh_Lock); \
	} while(0)
#define M_DestroyLock() \
	do \
	{ \
		CloseHandle(sh_Lock); \
		sh_Lock = (HANDLE) 0; \
	} while(0)
				HANDLE	sh_Lock;
unsigned int			gui_Size = 0;

/*$4
 ***********************************************************************************************************************
    TYPES
 ***********************************************************************************************************************
 */

typedef enum	win32SND_tden_EvtIdentifier_
{
	evt_Add				= 0,
	evt_Sub,
	evt_KillThread,
	evt_Stop,
	evt_SpecialActionsNb,
	evt_ReceiveAck,
	evt_RefreshS0,
	evt_RefreshS1,
	evt_RefreshS2,
	evt_RefreshS3,
	evt_RefreshS4,
	evt_RefreshS5,
	evt_RefreshS6,
	evt_RefreshS7,
	evt_RefreshS8,
	evt_AutostopS0,
	evt_AutostopS1,
	evt_AutostopS2,
	evt_AutostopS3,
	evt_AutostopS4,
	evt_AutostopS5,
	evt_AutostopS6,
	evt_AutostopS7,
	evt_AllActionsNb
} win32SND_tden_EvtIdentifier;

typedef struct	win32SND_tdst_SoundStream_
{
	IDirectSoundNotify8		*pst_DSN;
	/**/
	HANDLE					x_File;
	volatile unsigned int	ui_FilePosition;
	volatile unsigned int	ui_FileSize;
	volatile unsigned int	ui_FileCurrSeek;
	/**/
	volatile HANDLE			h_NotifyStopEvent;
	volatile HANDLE			h_NotifyRefreshEvent;
	volatile int			i_StreamId;
	/**/
	volatile unsigned int	ui_StartOffset;
	volatile unsigned int	ui_LoopBegin;
	volatile unsigned int	ui_LoopEnd;
	/**/
	volatile unsigned int	ui_BufferSize;
	volatile unsigned int	ui_NotifySize;
	/**/
	volatile unsigned int	ui_WriteCursor;
	volatile unsigned int	ui_PlayCursor;
	volatile unsigned int	ui_Progress;
	/**/
	volatile unsigned int	ui_EndCursor;
	volatile unsigned int	ui_WaitPlayLoopBack;
	volatile int			b_DonePlaying;
	volatile int			i_LoopFile;
	volatile int			b_FoundEnd;
	volatile int			b_Pause;
	/**/
	volatile int			i_NewFlag;
	volatile int			i_NewLoop;
	volatile unsigned int	ui_ExitPoint;
	volatile unsigned int	ui_EnterPoint;
	volatile unsigned int	ui_NewSize;
	volatile unsigned int	ui_NewPosition;
	volatile unsigned int	ui_NewLoopBegin;
	volatile unsigned int	ui_NewLoopEnd;
	/**/
	volatile unsigned int	ui_WriteOffset;
	/**/
	volatile float			f_ChainDelay;
} win32SND_tdst_SoundStream;

typedef struct	win32SND_tdst_NotifyRef_
{
	SND_tdst_SoundBuffer	*pst_SB;
	BOOL					b_Busy;
} win32SND_tdst_NotifyRef;

typedef struct	win32SND_tdst_NotifyEventList_
{
	volatile HANDLE * volatile ah_EventList;
	volatile unsigned int	ui_EventNb;
	volatile win32SND_tdst_NotifyRef * volatile apst_RefList;
	volatile unsigned int	ui_RefListSize;
} win32SND_tdst_NotifyEventList;

/*$4
 ***********************************************************************************************************************
    PROTOTYPES
 ***********************************************************************************************************************
 */

static SND_tdst_SoundBuffer *win32SND_pst_StreamCreate(SND_tdst_TargetSpecificData *, SND_tdst_WaveData *, unsigned int);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void win32SND_StreamReadFile(SND_tdst_SoundBuffer *, char *, unsigned int _ui_size, unsigned int *, BOOL);
static void win32SND_StreamFullfillBuffer(SND_tdst_SoundBuffer *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static int	win32SND_i_SetupNotifications(SND_tdst_SoundBuffer *);
static void win32SND_NotifyEventAdd(SND_tdst_SoundBuffer *);
static void win32SND_NotifyEventSub(SND_tdst_SoundBuffer *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

DWORD WINAPI	__Th_win32SND_ui_StreamThread(LPVOID lpParameter);
void			__Th_win32SND_NotifyEventAdd(int);
void			__Th_win32SND_NotifyEventSub(SND_tdst_SoundBuffer *, int);
void			__Th_win32SND_StreamStop(SND_tdst_SoundBuffer *, int);
void			__Th_win32SND_StreamUpdateProgress(SND_tdst_SoundBuffer *);

/*$4
 ***********************************************************************************************************************
    VARIABLES
 ***********************************************************************************************************************
 */

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

SND_tdst_SoundBuffer * volatile win32SND_gst_RequestBuffer;
win32SND_tdst_NotifyEventList win32SND_gst_NotifyManager;
unsigned int				win32SND_gui_ThreadId;
HANDLE						win32SND_gh_ThreadHandle;

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

HANDLE				win32SND_ga_Event[evt_AllActionsNb];
static int			win32SND_gi_StreamId;
static char			*win32SND_pc_TempBuffer;
static char			*win32SND_pc_TempBufferBis;
static unsigned int aui_PrefetchedPosition[SND_Cte_StreamPrefetchMax];

/*$4
 ***********************************************************************************************************************
    MODULE FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamInitModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	M_InitLock();

	L_memset(aui_PrefetchedPosition, 0, sizeof(unsigned int) * SND_Cte_StreamPrefetchMax);

	if(SND_gc_NoSound) return;

	/*$1- reset globals ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_gui_ThreadId = 0;
	win32SND_gh_ThreadHandle = NULL;
	win32SND_gst_RequestBuffer = NULL;
	win32SND_gi_StreamId = 0;

	win32SND_pc_TempBuffer = L_malloc(SND_Cte_MaxBufferSize);
	win32SND_M_Assert(win32SND_pc_TempBuffer);

	win32SND_pc_TempBufferBis = L_malloc(SND_Cte_MaxBufferSize);
	win32SND_M_Assert(win32SND_pc_TempBufferBis);

	/*$1- create events ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < evt_AllActionsNb; i++)
	{
		win32SND_ga_Event[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		win32SND_M_Assert(win32SND_ga_Event[i]);
	}

	/*$1- alloc event list ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_gst_NotifyManager.ui_RefListSize = evt_AllActionsNb;
	win32SND_gst_NotifyManager.apst_RefList = (win32SND_tdst_NotifyRef *) MEM_p_Alloc(win32SND_gst_NotifyManager.ui_RefListSize * sizeof(win32SND_tdst_NotifyRef));
	win32SND_M_Assert(win32SND_gst_NotifyManager.apst_RefList);
	L_memset
	(
		(char *) win32SND_gst_NotifyManager.apst_RefList,
		0,
		win32SND_gst_NotifyManager.ui_RefListSize * sizeof(win32SND_tdst_NotifyRef)
	);

	win32SND_gst_NotifyManager.ah_EventList = (HANDLE *) MEM_p_Alloc(win32SND_gst_NotifyManager.ui_RefListSize * 2 * sizeof(HANDLE));
	win32SND_M_Assert(win32SND_gst_NotifyManager.ah_EventList);
	L_memset(win32SND_gst_NotifyManager.ah_EventList, 0, win32SND_gst_NotifyManager.ui_RefListSize * 2 * sizeof(HANDLE));

	/*$1- register special actions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_gst_NotifyManager.ui_EventNb = evt_SpecialActionsNb;
	for(i = 0; i < (evt_SpecialActionsNb >> 1); i++)
	{
		win32SND_gst_NotifyManager.ah_EventList[2 * i] = win32SND_ga_Event[2 * i];
		win32SND_gst_NotifyManager.ah_EventList[2 * i + 1] = win32SND_ga_Event[2 * i + 1];
		win32SND_gst_NotifyManager.apst_RefList[i].b_Busy = TRUE;
		win32SND_gst_NotifyManager.apst_RefList[i].pst_SB = NULL;
	}

	/*$1- create the thread ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_gh_ThreadHandle = CreateThread(NULL, 0, __Th_win32SND_ui_StreamThread, NULL, 0, &win32SND_gui_ThreadId);
	win32SND_M_Assert(win32SND_gh_ThreadHandle);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamCloseModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	M_DestroyLock();

	if(SND_gc_NoSound) return;

	/*$1- send exit to thread ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_gst_RequestBuffer = (SND_tdst_SoundBuffer *) -1;
	SetEvent(win32SND_ga_Event[evt_KillThread]);
	WaitForSingleObject(win32SND_ga_Event[evt_ReceiveAck], win32SND_Cte_TimeOut);

	/*$1- kill thread ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	WaitForSingleObject(win32SND_gh_ThreadHandle, win32SND_Cte_TimeOut);
	CloseHandle(win32SND_gh_ThreadHandle);
	win32SND_gh_ThreadHandle = NULL;
	win32SND_gui_ThreadId = 0;

	/*$1- free all others ressources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_gst_RequestBuffer = NULL;

	if(win32SND_gst_NotifyManager.ah_EventList) MEM_Free((void *) win32SND_gst_NotifyManager.ah_EventList);
	win32SND_gst_NotifyManager.ah_EventList = NULL;

	if(win32SND_gst_NotifyManager.apst_RefList) MEM_Free((void *) win32SND_gst_NotifyManager.apst_RefList);
	win32SND_gst_NotifyManager.apst_RefList = NULL;

	win32SND_gst_NotifyManager.ui_EventNb = 0;
	win32SND_gst_NotifyManager.ui_RefListSize = 0;

	win32SND_gi_StreamId = 0;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < evt_AllActionsNb; i++)
	{
		if(win32SND_ga_Event[i]) CloseHandle(win32SND_ga_Event[i]);
		win32SND_ga_Event[i] = (HANDLE) NULL;
	}

	if(win32SND_pc_TempBuffer) L_free(win32SND_pc_TempBuffer);
	win32SND_pc_TempBuffer = NULL;

	if(win32SND_pc_TempBufferBis) L_free(win32SND_pc_TempBufferBis);
	win32SND_pc_TempBufferBis = NULL;
}

/*$4
 ***********************************************************************************************************************
    CREATION/DESTRUCTION
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_CreateTrackSB(SND_tdst_SoundBuffer **_ppst_MonoSB, SND_tdst_SoundBuffer **_ppst_StereoSB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_WaveData	stWave;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	stWave.i_Size = 0;
	stWave.pbData = NULL;
	stWave.st_WaveFmtx.cbSize = 0;
	stWave.st_WaveFmtx.nChannels = 1;
	stWave.st_WaveFmtx.wFormatTag = WAVE_FORMAT_PCM;
	stWave.st_WaveFmtx.wBitsPerSample = 16;
	stWave.st_WaveFmtx.nSamplesPerSec = 48000;
	stWave.st_WaveFmtx.nBlockAlign = sizeof(short) * stWave.st_WaveFmtx.nChannels;
	stWave.st_WaveFmtx.nAvgBytesPerSec = stWave.st_WaveFmtx.nSamplesPerSec * stWave.st_WaveFmtx.nBlockAlign;

	/* alloc mono stream SB */
	*_ppst_MonoSB = win32SND_pst_StreamCreate(SND_gst_Params.pst_SpecificD, &stWave, SND_Cte_StreamedFile);

	/* alloc stereo stream SB */
	stWave.st_WaveFmtx.nChannels = 2;
	stWave.st_WaveFmtx.nBlockAlign <<= 1;
	stWave.st_WaveFmtx.nAvgBytesPerSec <<= 1;
	*_ppst_StereoSB = win32SND_pst_StreamCreate(SND_gst_Params.pst_SpecificD, &stWave, SND_Cte_StreamedFile);

	/* this SB must not be counted in raster (win32 specific, only one mono SB on PSX2) */
	SND_RamRastersDel((int) (*_ppst_StereoSB)->pst_DSB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_ReleaseTrackSB(SND_tdst_SoundBuffer *_pst_MonoSB, SND_tdst_SoundBuffer *_pst_StereoSB)
{
	win32SND_SB_Release(_pst_MonoSB);
	win32SND_SB_Release(_pst_StereoSB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static SND_tdst_SoundBuffer *win32SND_pst_StreamCreate
(
	SND_tdst_TargetSpecificData *_pst_SpecificD,
	SND_tdst_WaveData			*_pst_WI,
	unsigned int				_ui_PositionOfWaveData
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DSBUFFERDESC			dsbd;
	SND_tdst_SoundBuffer	*pst_SB;
	HRESULT					hr;
	win32SND_tdst_SoundStream *pst_SS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_WI->st_WaveFmtx.wFormatTag = WAVE_FORMAT_PCM;
	if(!_pst_SpecificD->pst_DS) return NULL;

	/* init of SounbBuffer info */
	pst_SB = (SND_tdst_SoundBuffer *) MEM_p_Alloc(sizeof(SND_tdst_SoundBuffer));
	L_memset(pst_SB, 0, sizeof(SND_tdst_SoundBuffer));

	pst_SS = (win32SND_tdst_SoundStream *) MEM_p_Alloc(sizeof(win32SND_tdst_SoundStream));
	L_memset(pst_SS, 0, sizeof(win32SND_tdst_SoundStream));

	pst_SS->ui_NotifySize = SND_Cte_MaxBufferSize / _pst_WI->st_WaveFmtx.nBlockAlign / win32SND_Cte_NotifyNb;
	pst_SS->ui_NotifySize *= _pst_WI->st_WaveFmtx.nBlockAlign;
	pst_SS->ui_BufferSize = win32SND_Cte_NotifyNb * pst_SS->ui_NotifySize;

	/* init of DirectSoundBuffer descriptor */
	L_memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwBufferBytes = pst_SS->ui_BufferSize;
	dsbd.lpwfxFormat = &_pst_WI->st_WaveFmtx;
	dsbd.dwFlags =
		(
			DSBCAPS_CTRLPOSITIONNOTIFY |
			DSBCAPS_CTRLFREQUENCY |
			DSBCAPS_CTRLPAN |
			DSBCAPS_CTRLVOLUME |
			DSBCAPS_GETCURRENTPOSITION2 |
			DSBCAPS_LOCSOFTWARE
		);
	dsbd.guid3DAlgorithm = GUID_NULL;

	/*$2- create a DirectSoundBuffer ---------------------------------------------------------------------------------*/

	hr = IDirectSound8_CreateSoundBuffer(_pst_SpecificD->pst_DS, &dsbd, (IDirectSoundBuffer **) &pst_SB->pst_DSB, NULL);
	win32SND_M_Assert(hr == DS_OK);
	if(hr != DS_OK) return NULL;

	/*$2- get a DirectSoundNotify interface --------------------------------------------------------------------------*/
#ifdef JADEFUSION
	hr = pst_SB->pst_DSB->QueryInterface(IID_IDirectSoundNotify, (LPVOID*) &pst_SS->pst_DSN);
#else
	hr = IDirectSoundBuffer8_QueryInterface(pst_SB->pst_DSB, &IID_IDirectSoundNotify, (void *) &pst_SS->pst_DSN);
#endif
	win32SND_M_Assert(hr == DS_OK);
	if(hr != DS_OK) return NULL;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	SND_RamRasterAdd((int) pst_SB->pst_DSB, pst_SS->ui_BufferSize);

	pst_SB->i_Channel = dsbd.lpwfxFormat->nChannels;
	pst_SB->i_PanIdx = 64;
	pst_SB->pst_SS = pst_SS;

	win32SND_M_Assert(win32SND_gi_StreamId < win32SND_Cte_StreamIdNb);
	pst_SS->h_NotifyRefreshEvent = (HANDLE) - 1;
	pst_SS->h_NotifyStopEvent = (HANDLE) - 1;
	pst_SS->i_StreamId = win32SND_gi_StreamId++;

	pst_SS->x_File = CreateFile
		(
			BIG_gst.asz_Name,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
			NULL
		);

	win32SND_M_Assert(pst_SS->x_File != INVALID_HANDLE_VALUE);


	return(pst_SB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamRelease(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	win32SND_tdst_SoundStream *pst_SS;
	HRESULT					hr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SB) return;
	if(!_pst_SB->pst_SS) return;
	pst_SS = _pst_SB->pst_SS;


	/*$1- stop the stream ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_StreamStop(_pst_SB);

	/*$1- release resources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_SS->x_File != INVALID_HANDLE_VALUE) CloseHandle(pst_SS->x_File);
	pst_SS->x_File = INVALID_HANDLE_VALUE;

	if(pst_SS->pst_DSN)
	{
		hr = IDirectSoundNotify_Release(pst_SS->pst_DSN);
		win32SND_M_Assert(hr == DS_OK);
	}

	pst_SS->pst_DSN = NULL;

	if(_pst_SB->pst_DSB)
	{
		SND_RamRastersDel((int) _pst_SB->pst_DSB);
		hr = IDirectSoundBuffer8_Release(_pst_SB->pst_DSB);
		win32SND_M_Assert(hr == DS_OK);
	}

	_pst_SB->pst_DSB = NULL;

	if((pst_SS->h_NotifyRefreshEvent != (HANDLE) 0) && (pst_SS->h_NotifyRefreshEvent != (HANDLE) - 1))
	{
		LINK_PrintStatusMsg("[WARNING] We are releasing one streamed Sound Buffer with allocated refresh event handler");
	}

	if((pst_SS->h_NotifyStopEvent != (HANDLE) 0) && (pst_SS->h_NotifyStopEvent != (HANDLE) - 1))
	{
		LINK_PrintStatusMsg("[WARNING] We are releasing one streamed Sound Buffer with allocated stop event handler");
	}

	MEM_Free(_pst_SB->pst_SS);
	_pst_SB->pst_SS = NULL;
}

/*$4
 ***********************************************************************************************************************
    PUBLIC FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamStop(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	/* if never played */
    if((int)_pst_SB->pst_SS == -1) return ;
	if(_pst_SB->pst_SS->h_NotifyStopEvent == (HANDLE) - 1) return;

	win32SND_gst_RequestBuffer = _pst_SB;
	SetEvent(win32SND_ga_Event[evt_Stop]);
	hr = WaitForSingleObject(win32SND_ga_Event[evt_ReceiveAck], win32SND_Cte_TimeOut);
	M_CheckTimeOut(hr, "win32SND_StreamStop");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamPause(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~*/
	DWORD	dwStatus;
	HRESULT hr;
	/*~~~~~~~~~~~~~*/

	hr = IDirectSoundBuffer8_GetStatus(_pst_SB->pst_DSB, &dwStatus);
	win32SND_M_Assert(hr == DS_OK);

	if(dwStatus & DSBSTATUS_PLAYING)
	{
		_pst_SB->pst_SS->b_Pause = TRUE;
		hr = IDirectSoundBuffer8_Stop(_pst_SB->pst_DSB);
		win32SND_M_Assert(hr == DS_OK);

		do
		{
			hr = IDirectSoundBuffer8_GetStatus(_pst_SB->pst_DSB, &dwStatus);
			win32SND_M_Assert(hr == DS_OK);
		} while(dwStatus & DSBSTATUS_PLAYING);
	}
	else
	{
		_pst_SB->pst_SS->b_Pause = FALSE;
		hr = IDirectSoundBuffer8_Play(_pst_SB->pst_DSB, 0, 0, DSBPLAY_LOOPING);
		win32SND_M_Assert(hr == DS_OK);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamReinitAndPlay
(
	SND_tdst_SoundBuffer	*_pst_SB,
	int						_i_Flag,
	int						_i_LoopNb,
	unsigned int			_ui_Size,
	unsigned int			_ui_Position,
	unsigned int			_ui_LoopBegin,
	unsigned int			_ui_LoopEnd,
	int						_i_StartPos,
	int						_i_StopPos,
	int						_i_Freq,
	int						_i_Pan,
	int						_i_Vol,
	int						_i_FxL,
	int						_i_FxR
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	win32SND_tdst_SoundStream *pst_SS;
	HRESULT					hr;
	extern BAS_tdst_barray	win32SND_gst_PlayingSB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SB) return;
	pst_SS = _pst_SB->pst_SS;

	win32SND_FreeCoreBuffer(_pst_SB);
	if((_i_Flag & SND_Cul_SBC_PlayFxA) && (win32SND_AllocCoreBuffer(_pst_SB, SND_Cte_FxCoreA) < 0))
		return;
	else if((_i_Flag & SND_Cul_SBC_PlayFxB) && (win32SND_AllocCoreBuffer(_pst_SB, SND_Cte_FxCoreB) < 0))
		return;
	else if(win32SND_AllocCoreBuffer(_pst_SB, -1) < 0)
		return;

	if(1)//SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
	{
		_ui_Size = _ui_Size / (36 * _pst_SB->i_Channel);
		_ui_Size = _ui_Size * (36 * _pst_SB->i_Channel);
		_ui_Size = SND_ui_GetDecompressedSize(_ui_Size);
        
		_i_StartPos = _i_StartPos / (36 * _pst_SB->i_Channel);
		_i_StartPos = _i_StartPos * (36 * _pst_SB->i_Channel);
        _i_StartPos = SND_ui_GetDecompressedSize(_i_StartPos);
		  		
		_ui_LoopBegin = _ui_LoopBegin / (36 * _pst_SB->i_Channel);
		_ui_LoopBegin = _ui_LoopBegin * (36 * _pst_SB->i_Channel);
        _ui_LoopBegin = SND_ui_GetDecompressedSize(_ui_LoopBegin);
		
		_ui_LoopEnd = _ui_LoopEnd / (36 * _pst_SB->i_Channel);
		_ui_LoopEnd = _ui_LoopEnd * (36 * _pst_SB->i_Channel);		
        _ui_LoopEnd = SND_ui_GetDecompressedSize(_ui_LoopEnd);
        
        if(_i_StopPos  != -1)
        {
            _i_StopPos = _i_StopPos / (36 * _pst_SB->i_Channel);
		    _i_StopPos = _i_StopPos * (36 * _pst_SB->i_Channel);		
            _i_StopPos = SND_ui_GetDecompressedSize(_i_StopPos);
        }
	}

	/*$1- stop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_StreamStop(_pst_SB);

	/*$1- check settings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_i_StartPos < 0) _i_StartPos = 0;
	if((unsigned int) _i_StartPos > _ui_Size) _i_StartPos = 0;
	if((unsigned int) _i_StopPos > _ui_Size) _i_StopPos = -1;
	if(_i_StopPos <= _i_StartPos) _i_StopPos = -1;

	/*$1- reset parameters ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SS->b_FoundEnd = 0;
	pst_SS->b_DonePlaying = FALSE;
	pst_SS->b_Pause = FALSE;
	pst_SS->i_LoopFile = _i_LoopNb;
	if(pst_SS->i_LoopFile > 0) pst_SS->i_LoopFile--;
	if(!pst_SS->i_LoopFile) _i_Flag &= ~SND_Cul_SBC_PlayLooping;

	pst_SS->ui_ExitPoint = 0;
	pst_SS->ui_EnterPoint = 0;

	pst_SS->ui_WriteCursor = 0;
	pst_SS->ui_EndCursor = -1;
	pst_SS->ui_WaitPlayLoopBack = 0;
	pst_SS->ui_WriteOffset = 0;
	pst_SS->f_ChainDelay = 0.0f;

	pst_SS->ui_PlayCursor = 0;
	pst_SS->ui_Progress = 0;
	pst_SS->ui_StartOffset = (unsigned int) _i_StartPos;
	pst_SS->ui_LoopBegin = _ui_LoopBegin;
	pst_SS->ui_LoopEnd = _ui_LoopEnd;

	if(pst_SS->i_LoopFile && (pst_SS->ui_StartOffset > pst_SS->ui_LoopEnd)) pst_SS->ui_StartOffset = 0;

	pst_SS->ui_FileSize = _ui_Size;
	pst_SS->ui_FilePosition = _ui_Position;
	pst_SS->ui_FileCurrSeek = 0;

	/*$1- register the stream ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BAS_binsert((ULONG) _pst_SB, (ULONG) _pst_SB, &win32SND_gst_PlayingSB);

	/*$1- fill buffer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_StreamFullfillBuffer(_pst_SB);

	/*$1- prepare sound buffer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_SB_SetFrequency(_pst_SB, _i_Freq);
	win32SND_SB_SetPan(_pst_SB, _i_Pan, 0);
	win32SND_SB_SetVolume(_pst_SB, _i_Vol);

	/*$1- go ahead ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hr = win32SND_i_SetupNotifications(_pst_SB);
	win32SND_M_Assert(hr == DS_OK);

	hr = IDirectSoundBuffer8_SetCurrentPosition(_pst_SB->pst_DSB, 0);
	win32SND_M_Assert(hr == DS_OK);

	hr = IDirectSoundBuffer8_Play(_pst_SB->pst_DSB, 0, 0, DSBPLAY_LOOPING);
	win32SND_M_Assert(hr == DS_OK);
}

/*$4
 ***********************************************************************************************************************
    PRIVATE FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int win32SND_i_SetupNotifications(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						hr;
	DSBPOSITIONNOTIFY		ast_PosNotify[win32SND_Cte_NotifyNb + 1];
	unsigned int			ui_Size;
	int						i;
	win32SND_tdst_SoundStream *pst_SS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hr = DS_OK;
	ui_Size = _pst_SB->pst_SS->ui_NotifySize;
	pst_SS = _pst_SB->pst_SS;

	/* Create the 2 events. One for Play one for stop. */
	win32SND_NotifyEventAdd(_pst_SB);

	/* setup the first one. */
	ast_PosNotify[0].dwOffset = ui_Size;
	ast_PosNotify[0].hEventNotify = pst_SS->h_NotifyRefreshEvent;

	for(i = 1; i < win32SND_Cte_NotifyNb; i++)
	{
		ast_PosNotify[i].dwOffset = ast_PosNotify[i - 1].dwOffset + ui_Size;
		ast_PosNotify[i].hEventNotify = pst_SS->h_NotifyRefreshEvent;
	}

	ast_PosNotify[i - 1].dwOffset -= 1;

	/* set the stop notification. */
	ast_PosNotify[i].dwOffset = DSBPN_OFFSETSTOP;
	ast_PosNotify[i].hEventNotify = pst_SS->h_NotifyStopEvent;

	/* setup notification */
	hr = IDirectSoundNotify_SetNotificationPositions(pst_SS->pst_DSN, win32SND_Cte_NotifyNb + 1, ast_PosNotify);
	win32SND_M_Assert(hr == DS_OK);

	return(hr);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32SND_StreamFullfillBuffer(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*pc_Write1, *pc_Write2;
	unsigned int			ui_Len1, ui_Len2;
	unsigned int			ui_ActualBytesWritten;
	HRESULT					hr;
	win32SND_tdst_SoundStream *p_stream;
	LONG					dwPos = 0;
	unsigned int			ui_Written,ui_PlayPos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(p_stream = _pst_SB->pst_SS)) return;

	/*$1- require the write buffer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
	hr = _pst_SB->pst_DSB->Lock
		(

			0,
			p_stream->ui_BufferSize,
			(LPVOID*)&pc_Write1,
			(LPDWORD)&ui_Len1,
			(LPVOID*)&pc_Write2,
			(LPDWORD)&ui_Len2,
			DSBLOCK_ENTIREBUFFER
		);
#else
	hr = IDirectSoundBuffer8_Lock
		(
			_pst_SB->pst_DSB,
			0,
			p_stream->ui_BufferSize,
			&pc_Write1,
			&ui_Len1,
			&pc_Write2,
			&ui_Len2,
			DSBLOCK_ENTIREBUFFER
		);
#endif
	win32SND_M_Assert(hr == DS_OK);
	win32SND_M_Assert(pc_Write2 == NULL);
	win32SND_M_Assert(ui_Len2 == 0);
	win32SND_M_Assert(ui_Len1 == p_stream->ui_BufferSize);
	win32SND_M_Assert(ui_Len1);
	win32SND_M_Assert(NULL != pc_Write1);
	win32SND_M_Assert(p_stream->ui_WriteCursor < p_stream->ui_BufferSize);

	/*$1- rewind the file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(1)//SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
	{
		SetFilePointer
		(
			p_stream->x_File,
			p_stream->ui_FilePosition + SND_ui_GetCompressedSize(p_stream->ui_StartOffset),
			&dwPos,
			FILE_BEGIN
		);
	}
	else
	{
		SetFilePointer(p_stream->x_File, p_stream->ui_FilePosition + p_stream->ui_StartOffset, &dwPos, FILE_BEGIN);
	}

	p_stream->ui_FileCurrSeek = p_stream->ui_StartOffset;

	/*$1- fill in the buffer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	win32SND_StreamReadFile(_pst_SB, pc_Write1, ui_Len1, &ui_ActualBytesWritten, FALSE);

	/*$1- check completion ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(p_stream->i_LoopFile && (ui_ActualBytesWritten < ui_Len1))
	{
		if(p_stream->i_LoopFile > 0) p_stream->i_LoopFile--;

		/* rewind the file */
		p_stream->ui_FileCurrSeek = p_stream->ui_LoopBegin;

		if(1)//SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
            SetFilePointer(p_stream->x_File, p_stream->ui_FilePosition + SND_ui_GetCompressedSize(p_stream->ui_FileCurrSeek), &dwPos, FILE_BEGIN);
        else
            SetFilePointer(p_stream->x_File, p_stream->ui_FilePosition + p_stream->ui_FileCurrSeek, &dwPos, FILE_BEGIN);

		/* read data */
		win32SND_StreamReadFile
		(
			_pst_SB,
			pc_Write1 + ui_ActualBytesWritten,
			ui_Len1 - ui_ActualBytesWritten,
			&ui_Written,
			FALSE
		);

		ui_ActualBytesWritten += ui_Written;
	}

	if(ui_ActualBytesWritten < ui_Len1)
	{
		p_stream->ui_EndCursor = p_stream->ui_WriteCursor + ui_ActualBytesWritten;
		hr = IDirectSoundBuffer8_GetCurrentPosition(_pst_SB->pst_DSB, &ui_PlayPos, NULL);
		win32SND_M_Assert(hr == DS_OK);
		if(ui_PlayPos >= p_stream->ui_EndCursor)
			p_stream->ui_WaitPlayLoopBack = 1;
		
		if(ui_ActualBytesWritten < p_stream->ui_NotifySize) p_stream->b_FoundEnd = 1;
		L_memset(pc_Write1 + ui_ActualBytesWritten, 0, ui_Len1 - ui_ActualBytesWritten);
	}

	/*$1- send data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hr = IDirectSoundBuffer8_Unlock(_pst_SB->pst_DSB, pc_Write1, ui_Len1, NULL, 0);
	win32SND_M_Assert(hr == DS_OK);

	/*$1- update the write pointer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_stream->ui_WriteCursor += ui_Len1;
	if(p_stream->ui_WriteCursor >= p_stream->ui_BufferSize) p_stream->ui_WriteCursor -= p_stream->ui_BufferSize;

	/*$1- reset progress / position ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_stream->ui_Progress = p_stream->ui_StartOffset;
	p_stream->ui_PlayCursor = 0;
    p_stream->ui_StartOffset = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32SND_NotifyEventAdd(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	/* don't overwrite */
	if((_pst_SB->pst_SS->h_NotifyRefreshEvent != (HANDLE) - 1) && (_pst_SB->pst_SS->h_NotifyRefreshEvent != (HANDLE) 0))
		return;

	if((_pst_SB->pst_SS->h_NotifyStopEvent != (HANDLE) - 1) && (_pst_SB->pst_SS->h_NotifyStopEvent != (HANDLE) 0))
		return;

	/* assigns it in SB */
	_pst_SB->pst_SS->h_NotifyRefreshEvent = win32SND_ga_Event[evt_RefreshS0 + _pst_SB->pst_SS->i_StreamId];
	_pst_SB->pst_SS->h_NotifyStopEvent = win32SND_ga_Event[evt_AutostopS0 + _pst_SB->pst_SS->i_StreamId];
	win32SND_gst_RequestBuffer = _pst_SB;

	SetEvent(win32SND_ga_Event[evt_Add]);
	hr = WaitForSingleObject(win32SND_ga_Event[evt_ReceiveAck], win32SND_Cte_TimeOut);
	M_CheckTimeOut(hr, "win32SND_NotifyEventAdd");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32SND_NotifyEventSub(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	/* if(!win32SND_ga_Event[evt_ReceiveAck]) return; */
	win32SND_gst_RequestBuffer = _pst_SB;

	SetEvent(win32SND_ga_Event[evt_Sub]);
	hr = WaitForSingleObject(win32SND_ga_Event[evt_ReceiveAck], win32SND_Cte_TimeOut);
	M_CheckTimeOut(hr, "win32SND_NotifyEventSub");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamLoopCountGet(SND_tdst_SoundBuffer *_pSB, int * pi)
{
	*pi = _pSB->pst_SS->i_LoopFile;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamChainDelayGet(SND_tdst_SoundBuffer *_pSB, float *pf)
{
	*pf = _pSB->pst_SS->f_ChainDelay;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32SND_StreamReadFile
(
	SND_tdst_SoundBuffer	*_pSB,
	char					*_pc_buffer,
	unsigned int			_ui_size,
	unsigned int			*_pui_actual_size,
	BOOL					b_ThreadCall
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*pBuff;
	win32SND_tdst_SoundStream *pSS;
	BOOL					bChain;
	DWORD					dwPos = 0;
	unsigned int			uiChainSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* M_BeginCriticalSection(); */
    *_pui_actual_size = 0;
	pSS = _pSB->pst_SS;
	bChain = FALSE;
	pBuff = b_ThreadCall ? win32SND_pc_TempBuffer : win32SND_pc_TempBufferBis;

	if(pSS->i_LoopFile)
    {
		_ui_size = min(pSS->ui_LoopEnd - pSS->ui_FileCurrSeek, _ui_size);
        if(pSS->ui_LoopEnd < pSS->ui_FileCurrSeek)
            return;
    }
	else
    {
		_ui_size = min(pSS->ui_FileSize - pSS->ui_FileCurrSeek, _ui_size);
        if(pSS->ui_FileSize < pSS->ui_FileCurrSeek) 
            return;
    }

	if(pSS->ui_ExitPoint)
	{
		if((pSS->ui_FileCurrSeek <= pSS->ui_ExitPoint) && (pSS->ui_ExitPoint <= (pSS->ui_FileCurrSeek + _ui_size)))
		{
			uiChainSize = _ui_size;
			_ui_size = pSS->ui_ExitPoint - pSS->ui_FileCurrSeek;
			bChain = TRUE;
		}
	}

	if(1)//SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
	{
		/*~~~~~~~~~~~~*/
		int		newsize;
		BOOL	ok;
		/*~~~~~~~~~~~~*/
	
        newsize = SND_ui_GetCompressedSize(_ui_size);
		win32SND_M_Assert(!(newsize % (36 * _pSB->i_Channel)));

		L_memset(pBuff, 0, SND_Cte_MaxBufferSize);

		ReadFile(pSS->x_File, pBuff, newsize, _pui_actual_size, NULL);
		ok = SND_b_Decode(pBuff, _pc_buffer, newsize / (36 * _pSB->i_Channel), _pSB->i_Channel);
		win32SND_M_Assert(ok);

		*_pui_actual_size = SND_ui_GetDecompressedSize(*_pui_actual_size);
	}
	else
	{
		ReadFile(pSS->x_File, _pc_buffer, _ui_size, _pui_actual_size, NULL);
	}

	pSS->ui_FileCurrSeek += *_pui_actual_size;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(bChain)
	{
		pSS->ui_FilePosition = pSS->ui_NewPosition;
		pSS->ui_FileSize = pSS->ui_NewSize;
		pSS->i_LoopFile = pSS->i_NewLoop;
		pSS->ui_StartOffset = pSS->ui_EnterPoint;
		pSS->ui_LoopBegin = pSS->ui_NewLoopBegin;
		pSS->ui_LoopEnd = pSS->ui_NewLoopEnd;

		pSS->ui_WriteOffset = _ui_size;
		pSS->i_NewFlag = 0;
		pSS->i_NewLoop = 0;
		pSS->ui_ExitPoint = 0;
		pSS->ui_EnterPoint = 0;
		pSS->ui_NewSize = 0;
		pSS->ui_NewPosition = 0;

		pSS->b_FoundEnd = 0;
		pSS->b_DonePlaying = FALSE;

		if(1)//SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
		{
			SetFilePointer
			(
				pSS->x_File,
				pSS->ui_FilePosition + SND_ui_GetCompressedSize(pSS->ui_StartOffset),
				&dwPos,
				FILE_BEGIN
			);
		}
		else
		{
			SetFilePointer(pSS->x_File, pSS->ui_FilePosition + pSS->ui_StartOffset, &dwPos, FILE_BEGIN);
		}

		pSS->ui_FileCurrSeek = pSS->ui_StartOffset;
		pSS->ui_Progress = pSS->ui_StartOffset;
        pSS->ui_StartOffset = 0;
		pSS->ui_PlayCursor = 0;
		pSS->ui_EndCursor = -1;
		pSS->ui_WaitPlayLoopBack = 0;

		uiChainSize = uiChainSize - _ui_size;
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~*/
			unsigned int	ui_PlayPos;
			HRESULT			hr;
			/*~~~~~~~~~~~~~~~~~~~~~~~*/

			hr = IDirectSoundBuffer8_GetCurrentPosition(_pSB->pst_DSB, &ui_PlayPos, NULL);
			win32SND_M_Assert(hr == DS_OK);
			if(pSS->ui_WriteCursor > ui_PlayPos)
				pSS->f_ChainDelay = fLongToFloat(pSS->ui_WriteCursor - ui_PlayPos + _ui_size);
			else
				pSS->f_ChainDelay = fLongToFloat(pSS->ui_BufferSize + pSS->ui_WriteCursor - ui_PlayPos + _ui_size);
			pSS->f_ChainDelay /= (fLongToFloat(_pSB->i_Channel * sizeof(short)) * fLongToFloat(_pSB->l_Frequency));
		}

        if(uiChainSize <= (pSS->ui_FileSize-pSS->ui_FileCurrSeek) )
		{
			/*~~~~~~~~~~~~~~~~~~~~*/
			unsigned int	ui_Read;
			/*~~~~~~~~~~~~~~~~~~~~*/

			if(1)//SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
			{
				/*~~~~~~~~~~~~*/
				int		newsize;
				BOOL	ok;
				/*~~~~~~~~~~~~*/

				newsize = SND_ui_GetCompressedSize(uiChainSize);
                win32SND_M_Assert(!(newsize % (36 * _pSB->i_Channel)));

				ReadFile(pSS->x_File, pBuff, newsize, &ui_Read, NULL);
				ok = SND_b_Decode(pBuff, _pc_buffer + _ui_size, newsize / (36 * _pSB->i_Channel), _pSB->i_Channel);
				win32SND_M_Assert(ok);

				ui_Read = SND_ui_GetDecompressedSize(ui_Read);
			}
			else
			{
				ReadFile(pSS->x_File, _pc_buffer + _ui_size, uiChainSize, &ui_Read, NULL);
			}

			pSS->ui_FileCurrSeek += ui_Read;
			*_pui_actual_size += ui_Read;
            uiChainSize -= ui_Read;
		}
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    merge the 2 channels if needed
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if((_pSB->i_Channel == 2) && (SND_gst_Params.ul_RenderMode == SND_Cte_RenderMono))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		short			*pin, *pout;
		float			fl, fr;
		unsigned int	ui;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		pin = (short *) _pc_buffer;
		pout = (short *) _pc_buffer;
		for(ui = 0; ui < (*_pui_actual_size); ui += 4)
		{
			fl = (float) *pin++;
			fr = (float) *pin++;
			fl = 0.5f * (fl + fr);
			*pout++ = (short) fl;
			*pout++ = (short) fl;
		}
	}

	/* M_EndCriticalSection(); */
}

/*$4
 ***********************************************************************************************************************
    THREADED FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void __Th_win32SND_StreamStop(SND_tdst_SoundBuffer *pst_SB, int b_event)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	hr = IDirectSoundBuffer8_Stop(pst_SB->pst_DSB);
	win32SND_M_Assert(hr == DS_OK);

	__Th_win32SND_NotifyEventSub(pst_SB, b_event);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void __Th_win32SND_NotifyEventAdd(int b_event)
{
	/*~~~~~~~~~~~~~~*/
	unsigned int	i;
	/*~~~~~~~~~~~~~~*/

	win32SND_M_Assert(win32SND_gst_NotifyManager.apst_RefList);
	win32SND_M_Assert(win32SND_gst_NotifyManager.ah_EventList);

	for(i = 0; i < win32SND_gst_NotifyManager.ui_RefListSize; i++)
	{
		if(!win32SND_gst_NotifyManager.apst_RefList[i].b_Busy) break;
	}

	if(i >= win32SND_gst_NotifyManager.ui_RefListSize)
	{
		/*~~~~~*/
		int size;
		/*~~~~~*/

		size = win32SND_gst_NotifyManager.ui_RefListSize + SND_Cte_MinAllocInst;
		if(win32SND_gst_NotifyManager.ui_RefListSize)
		{
			win32SND_M_Assert(win32SND_gst_NotifyManager.ah_EventList);
			win32SND_gst_NotifyManager.apst_RefList = (win32SND_tdst_NotifyRef *) MEM_p_Realloc
				(
					(void *) win32SND_gst_NotifyManager.apst_RefList,
					size * sizeof(win32SND_tdst_NotifyRef)
				);
			win32SND_gst_NotifyManager.ah_EventList = (HANDLE *) MEM_p_Realloc
				(
					(void *) win32SND_gst_NotifyManager.apst_RefList,
					2 * size * sizeof(HANDLE)
				);
		}
		else
		{
			win32SND_M_Assert(!win32SND_gst_NotifyManager.ah_EventList);
			win32SND_gst_NotifyManager.apst_RefList = (win32SND_tdst_NotifyRef *) MEM_p_Alloc(size * sizeof(win32SND_tdst_NotifyRef));
			win32SND_gst_NotifyManager.ah_EventList = (HANDLE *) MEM_p_Alloc(2 * size * sizeof(HANDLE));
		}

		L_memset
		(
			(void *) &win32SND_gst_NotifyManager.apst_RefList[win32SND_gst_NotifyManager.ui_RefListSize],
			0,
			SND_Cte_MinAllocInst * sizeof(win32SND_tdst_NotifyRef)
		);
		L_memset
		(
			(void *) &win32SND_gst_NotifyManager.ah_EventList[2 * win32SND_gst_NotifyManager.ui_RefListSize],
			0,
			2 * SND_Cte_MinAllocInst * sizeof(win32SND_tdst_NotifyRef)
		);
		i = win32SND_gst_NotifyManager.ui_RefListSize;
		win32SND_gst_NotifyManager.ui_RefListSize += SND_Cte_MinAllocInst;
	}

	win32SND_M_Assert(win32SND_gst_NotifyManager.ui_EventNb == (2 * i));
	win32SND_gst_NotifyManager.apst_RefList[i].b_Busy = TRUE;
	win32SND_gst_NotifyManager.apst_RefList[i].pst_SB = win32SND_gst_RequestBuffer;
	win32SND_gst_NotifyManager.ah_EventList[2 * i] = win32SND_gst_RequestBuffer->pst_SS->h_NotifyRefreshEvent;
	win32SND_gst_NotifyManager.ah_EventList[2 * i + 1] = win32SND_gst_RequestBuffer->pst_SS->h_NotifyStopEvent;
	win32SND_gst_NotifyManager.ui_EventNb += 2;

	if(b_event)
	{
		SetEvent(win32SND_ga_Event[evt_ReceiveAck]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void __Th_win32SND_NotifyEventSub(SND_tdst_SoundBuffer *pst_SB, int b_event)
{
	/*~~~~~~~~~~~~~~~~~*/
	unsigned int	i, j;
	/*~~~~~~~~~~~~~~~~~*/

	win32SND_M_Assert(win32SND_gst_NotifyManager.ah_EventList);
	win32SND_M_Assert(win32SND_gst_NotifyManager.apst_RefList);

	if(pst_SB == (SND_tdst_SoundBuffer *) -1)
	{
		win32SND_gst_NotifyManager.ui_EventNb = 0;
		if(b_event)
		{
			SetEvent(win32SND_ga_Event[evt_ReceiveAck]);
		}

		return;
	}
	else
	{
		for(i = j = 0; i < (win32SND_gst_NotifyManager.ui_EventNb >> 1); i++)
		{
			if(win32SND_gst_NotifyManager.apst_RefList[i].pst_SB == pst_SB)
			{
				j = i + 1;
				break;
			}
		}

		if(j)
		{
			for(; j < (win32SND_gst_NotifyManager.ui_EventNb >> 1); i++, j++)
			{
				win32SND_gst_NotifyManager.apst_RefList[i].pst_SB = win32SND_gst_NotifyManager.apst_RefList[j].pst_SB;
				win32SND_gst_NotifyManager.ah_EventList[2 * i] = win32SND_gst_NotifyManager.ah_EventList[2 * j];
				win32SND_gst_NotifyManager.ah_EventList[2 * i + 1] = win32SND_gst_NotifyManager.ah_EventList[2 * j + 1];
			}

			win32SND_gst_NotifyManager.apst_RefList[i].pst_SB = NULL;
			win32SND_gst_NotifyManager.apst_RefList[i].b_Busy = FALSE;
			win32SND_gst_NotifyManager.ah_EventList[2 * i] = (HANDLE) 0;
			win32SND_gst_NotifyManager.ah_EventList[2 * i + 1] = (HANDLE) 0;
			win32SND_gst_NotifyManager.ui_EventNb -= 2;
		}
	}

	ResetEvent(pst_SB->pst_SS->h_NotifyRefreshEvent);
	pst_SB->pst_SS->h_NotifyRefreshEvent = NULL;

	ResetEvent(pst_SB->pst_SS->h_NotifyStopEvent);
	pst_SB->pst_SS->h_NotifyStopEvent = NULL;

	if(b_event)
	{
		SetEvent(win32SND_ga_Event[evt_ReceiveAck]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void __Th_win32SND_StreamUpdateProgress(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			ui_PlayPos;
	int						hr;
	unsigned int			ui_Played;
	win32SND_tdst_SoundStream *pst_SS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hr = IDirectSoundBuffer8_GetCurrentPosition(_pst_SB->pst_DSB, &ui_PlayPos, NULL);
	win32SND_M_Assert(hr == DS_OK);

	pst_SS = _pst_SB->pst_SS;

	if(pst_SS->ui_WaitPlayLoopBack && (ui_PlayPos <= pst_SS->ui_EndCursor))
		pst_SS->ui_WaitPlayLoopBack = 0;

	if(ui_PlayPos < pst_SS->ui_PlayCursor)
		ui_Played = pst_SS->ui_BufferSize + (ui_PlayPos - pst_SS->ui_PlayCursor);
	else
		ui_Played = ui_PlayPos - pst_SS->ui_PlayCursor;

	pst_SS->ui_Progress += ui_Played;
	pst_SS->ui_PlayCursor = ui_PlayPos;

	if(pst_SS->i_LoopFile)
	{
		while(pst_SS->ui_Progress > _pst_SB->pst_SS->ui_LoopEnd)
		{
			pst_SS->ui_Progress = pst_SS->ui_Progress - (_pst_SB->pst_SS->ui_LoopEnd - _pst_SB->pst_SS->ui_LoopBegin);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamUpdateStatus(SND_tdst_SoundBuffer *_pst_SB)
{
	if(_pst_SB->pst_SS)
	{
		if(_pst_SB->pst_SS->i_LoopFile) return;

		
        if(_pst_SB->pst_SS->ui_Progress >= _pst_SB->pst_SS->ui_FileSize) 
        {
	        win32SND_SB_Stop(_pst_SB); 
        }
        
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
DWORD WINAPI __Th_win32SND_ui_StreamThread(LPVOID lpParameter)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			hr;
	unsigned int			hRet;
	SND_tdst_SoundBuffer	*pst_SB;
	win32SND_tdst_SoundStream *pst_SS;
	LONG					dwPos = 0;
	char					*pc_Write1;
	unsigned int			ui_Write1;
	unsigned int			ui_Actual;
	unsigned int			ui_PlayPos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hr = DS_OK;
	hRet = 0;

	while(1)
	{
		hRet = WaitForMultipleObjects
			(
				win32SND_gst_NotifyManager.ui_EventNb,
				(HANDLE *) win32SND_gst_NotifyManager.ah_EventList,
				FALSE,
				INFINITE
			);

		if(hRet == WAIT_FAILED) continue;

		hRet = hRet - WAIT_OBJECT_0;
		switch(hRet)
		{
		case evt_Add:
			__Th_win32SND_NotifyEventAdd(1);
			break;

		case evt_KillThread:
			SetEvent(win32SND_ga_Event[evt_ReceiveAck]);
			ExitThread(0);
			break;

		case evt_Sub:
			__Th_win32SND_NotifyEventSub(win32SND_gst_RequestBuffer, 1);
			break;

		case evt_Stop:
			if(win32SND_gst_RequestBuffer) pst_SS = win32SND_gst_RequestBuffer->pst_SS;
			__Th_win32SND_StreamStop(win32SND_gst_RequestBuffer, 1);
			break;

		default:
			win32SND_M_Assert(hRet < win32SND_gst_NotifyManager.ui_EventNb);
			pst_SB = (SND_tdst_SoundBuffer *) win32SND_gst_NotifyManager.apst_RefList[hRet >> 1].pst_SB;
			win32SND_M_Assert(pst_SB);
			pst_SS = pst_SB->pst_SS;
			win32SND_M_Assert(pst_SS);

			if(pst_SS->b_Pause == TRUE) break;

			__Th_win32SND_StreamUpdateProgress(pst_SB);

			if(pst_SS->ui_EndCursor != -1) 
			{
				hr = IDirectSoundBuffer8_GetCurrentPosition(pst_SB->pst_DSB, &ui_PlayPos, NULL);
				win32SND_M_Assert(hr == DS_OK);

				if((ui_PlayPos > pst_SS->ui_EndCursor) && !pst_SS->ui_WaitPlayLoopBack)
				{
					__Th_win32SND_StreamStop(pst_SB, 0);
					break;
				}
			}

			if(pst_SS->b_DonePlaying == TRUE)
			{
				__Th_win32SND_StreamStop(pst_SB, 0);
				break;
			}

			if(hRet & 1)
			{
				__Th_win32SND_StreamStop(pst_SB, 0);
				break;
			}

			if(!pst_SS->b_FoundEnd)
			{
				M_BeginCriticalSection();
#ifdef JADEFUSION
				hr = pst_SB->pst_DSB->Lock
					(

						pst_SS->ui_WriteCursor,
						pst_SS->ui_NotifySize,
						(LPVOID*)&pc_Write1,
						(LPDWORD)&ui_Write1,
						NULL,
						NULL,
						0
					);
#else
				hr = IDirectSoundBuffer8_Lock
					(
						pst_SB->pst_DSB,
						pst_SS->ui_WriteCursor,
						pst_SS->ui_NotifySize,
						&pc_Write1,
						&ui_Write1,
						NULL,
						NULL,
						0
					);
#endif
				win32SND_M_Assert(hr == DS_OK);
				win32SND_M_Assert(ui_Write1 == pst_SS->ui_NotifySize);

				ui_Actual = 0;
				win32SND_StreamReadFile(pst_SB, pc_Write1, ui_Write1, &ui_Actual, TRUE);

				while(pst_SS->i_LoopFile && (ui_Actual < ui_Write1))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~*/
					unsigned int	cbWritten;
					/*~~~~~~~~~~~~~~~~~~~~~~*/

					if(pst_SS->i_LoopFile > 0) pst_SS->i_LoopFile--;

                    pst_SS->ui_FileCurrSeek = pst_SS->ui_LoopBegin;

					
                    if(1)//SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
                        SetFilePointer
					    (
						    pst_SS->x_File,
						    pst_SS->ui_FilePosition + SND_ui_GetCompressedSize(pst_SS->ui_FileCurrSeek),
						    &dwPos,
						    FILE_BEGIN
					    );
                    else
                        SetFilePointer
					    (
						    pst_SS->x_File,
						    pst_SS->ui_FilePosition + pst_SS->ui_FileCurrSeek,
						    &dwPos,
						    FILE_BEGIN
					    );


					win32SND_StreamReadFile(pst_SB, pc_Write1 + ui_Actual, ui_Write1 - ui_Actual, &cbWritten, TRUE);

					ui_Actual += cbWritten;
				}

				if(ui_Actual < ui_Write1)
				{
					pst_SS->ui_EndCursor = pst_SS->ui_WriteCursor+ui_Actual;
					
					hr = IDirectSoundBuffer8_GetCurrentPosition(pst_SB->pst_DSB, &ui_PlayPos, NULL);
					win32SND_M_Assert(hr == DS_OK);
					if(ui_PlayPos >=pst_SS->ui_EndCursor)
						pst_SS->ui_WaitPlayLoopBack = 1;
					
					pst_SS->b_FoundEnd = 1;
					L_memset(pc_Write1 + ui_Actual, 0, ui_Write1 - ui_Actual);
				}

				hr = IDirectSoundBuffer8_Unlock(pst_SB->pst_DSB, (void *) pc_Write1, ui_Write1, NULL, 0);
				win32SND_M_Assert(hr == DS_OK);

				/* setup the nextwrite offset. */
				pst_SS->ui_WriteCursor += ui_Write1;
				if(pst_SS->ui_WriteCursor >= pst_SS->ui_BufferSize) pst_SS->ui_WriteCursor -= pst_SS->ui_BufferSize;
				M_EndCriticalSection();
			}
			else
			{
				win32SND_M_Assert(!pst_SS->i_LoopFile);
#ifdef JADEFUSION
				hr = pst_SB->pst_DSB->Lock
					(

						pst_SS->ui_WriteCursor,
						pst_SS->ui_NotifySize,
						(LPVOID*)&pc_Write1,
						(LPDWORD)&ui_Write1,
						NULL,
						NULL,
						0
					);
#else
				hr = IDirectSoundBuffer8_Lock
					(
						pst_SB->pst_DSB,
						pst_SS->ui_WriteCursor,
						pst_SS->ui_NotifySize,
						&pc_Write1,
						&ui_Write1,
						NULL,
						NULL,
						0
					);
#endif
				win32SND_M_Assert(hr == DS_OK);

				L_memset(pc_Write1, 0, ui_Write1);

				hr = IDirectSoundBuffer8_Unlock(pst_SB->pst_DSB, (void *) pc_Write1, ui_Write1, NULL, 0);
				win32SND_M_Assert(hr == DS_OK);

				if(pst_SS->b_FoundEnd > 2) pst_SS->b_DonePlaying = TRUE;
				pst_SS->b_FoundEnd++;
			}
		}
	}

	return TRUE;
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
void win32SND_StreamPrefetch(unsigned int uiPos, unsigned int uiSize)
{
	/*~~*/
	int i;
	/*~~*/

	if(!uiPos) return;

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		if(aui_PrefetchedPosition[i] == 0)
		{
			aui_PrefetchedPosition[i] = uiPos;
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamPrefetchArray(unsigned int *ui, unsigned int *pp)
{
	/*~~*/
	int i;
	/*~~*/

	if(ui)
	{
		L_memset(ui, 0, SND_Cte_StreamPrefetchMax * sizeof(unsigned int));
	}

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		if(ui[i]) win32SND_StreamPrefetch(ui[i], pp[i]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamFlush(unsigned int ui)
{
	/*~~*/
	int i;
	/*~~*/

	if(!ui) return;

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		if(aui_PrefetchedPosition[i] == ui)
		{
			aui_PrefetchedPosition[i] = 0;
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamGetPrefetchStatus(unsigned int *ui)
{
	L_memcpy(ui, aui_PrefetchedPosition, SND_Cte_StreamPrefetchMax * sizeof(unsigned int));
}

/*$4
 ***********************************************************************************************************************
    SPECIAL FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_ShutDownStreams(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					i;
	ULONG					ul_StreamFilter;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_StreamFilter = (SND_Cul_SF_Dialog | SND_Cul_SF_Music | SND_Cul_SF_Ambience);

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;
		if(!(pst_SI->ul_Flags & ul_StreamFilter)) continue;
		if(!SND_i_IsPlaying(i)) continue;
		pst_SI->pst_LI->pfv_SndBuffStop(pst_SI->pst_DSB);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamGetCurrPos(SND_tdst_SoundBuffer *pSB, int *_pi_Pos, int *_pi_Write)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			ui_PlayPos, ui_WritePos;
	int						hr;
	win32SND_tdst_SoundStream *pSS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_pi_Write = 0;
	*_pi_Pos = 0;

	if(!pSB) return;
	if(!pSB->pst_SS) return;
	pSS = pSB->pst_SS;

	M_BeginCriticalSection();
#ifdef JADEFUSION
	hr = pSB->pst_DSB->GetCurrentPosition((LPDWORD)&ui_PlayPos, (LPDWORD)&ui_WritePos);
#else
	hr = IDirectSoundBuffer8_GetCurrentPosition(pSB->pst_DSB, &ui_PlayPos, &ui_WritePos);
#endif
	win32SND_M_Assert(hr == DS_OK);

	if(pSS->ui_WaitPlayLoopBack && (ui_PlayPos <= pSS->ui_EndCursor))
		pSS->ui_WaitPlayLoopBack = 0;

	__Th_win32SND_StreamUpdateProgress(pSB);
	*_pi_Pos = *(int *) &pSS->ui_Progress;
	*_pi_Write = *(int *) &pSS->ui_FileCurrSeek;

	
    if(1)//SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
	{
		*_pi_Write = SND_ui_GetCompressedSize(*_pi_Write);
		*_pi_Pos = SND_ui_GetCompressedSize(*_pi_Pos);
    }
	
    M_EndCriticalSection();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_StreamChain
(
	SND_tdst_SoundBuffer	*pSB,
	int						iNewFlag,
	int						iLoop,
	unsigned int			uiExitPoint,
	unsigned int			uiEnterPoint,
	unsigned int			uiNewSize,
	unsigned int			uiNewPosition,
	unsigned int			uiLoopBegin,
	unsigned int			uiLoopEnd
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	win32SND_tdst_SoundStream *pSS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pSB) return;
	if(!pSB->pst_SS) return;

	if(1)//SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
	{
		uiExitPoint = uiExitPoint / (36 * pSB->i_Channel);
		uiExitPoint = uiExitPoint * (36 * pSB->i_Channel);
		uiExitPoint = SND_ui_GetDecompressedSize(uiExitPoint);

		uiEnterPoint = uiEnterPoint / (36 * pSB->i_Channel);
		uiEnterPoint = uiEnterPoint * (36 * pSB->i_Channel);
		uiEnterPoint = SND_ui_GetDecompressedSize(uiEnterPoint);

		uiNewSize = uiNewSize / (36 * pSB->i_Channel);
		uiNewSize = uiNewSize * (36 * pSB->i_Channel);
		uiNewSize = SND_ui_GetDecompressedSize(uiNewSize);

		uiLoopBegin = uiLoopBegin / (36 * pSB->i_Channel);
		uiLoopBegin = uiLoopBegin * (36 * pSB->i_Channel);
		uiLoopBegin = SND_ui_GetDecompressedSize(uiLoopBegin);

		uiLoopEnd = uiLoopEnd / (36 * pSB->i_Channel);
		uiLoopEnd = uiLoopEnd * (36 * pSB->i_Channel);
		uiLoopEnd = SND_ui_GetDecompressedSize(uiLoopEnd);
    }

	pSS = pSB->pst_SS;
	pSS->i_NewFlag = iNewFlag;
	pSS->i_NewLoop = iLoop;
	pSS->ui_EnterPoint = uiEnterPoint;
	pSS->ui_NewSize = uiNewSize;
	pSS->ui_NewPosition = uiNewPosition;
	pSS->ui_NewLoopBegin = uiLoopBegin;
	pSS->ui_NewLoopEnd = uiLoopEnd;
	pSS->ui_ExitPoint = uiExitPoint;
	pSS->f_ChainDelay = 0.0f;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#endif

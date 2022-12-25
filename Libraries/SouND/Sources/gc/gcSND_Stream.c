/*$T gcSND_Stream.c GC 1.138 12/03/04 16:18:49 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <dolphin/mix.h>

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGopen.h"

#include "GameCube/GC_File.h"
#include "GameCube/GC_Stream.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/gc/gcSND.h"
#include "SouND/Sources/gc/gcSND_Stream.h"
#include "SouND/Sources/gc/gcSND_Debug.h"
#include "SouND/Sources/gc/gcSND_Soft.h"
#include "SouND/Sources/gc/gcSND_ARAM.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/gc/gcSND_Prefetch.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDfx.h"

/*$4
 ***********************************************************************************************************************
    macros + constants
 ***********************************************************************************************************************
 */

#define M_RoundSize(size, channel) \
	do \
	{ \
		(size) = (size) / (SND_Cte_OneGamecubeFrameSize * (channel)); \
		(size) = (size) * (SND_Cte_OneGamecubeFrameSize * (channel)); \
	} while(0);


#define Cte_RamBufferSize			(SND_Cte_MaxBufferSize / 2)
#define M_FlushRamBuffer(pc)		DCFlushRange(pc, Cte_RamBufferSize);

#define Cte_MuxOneChannelFrameSize	(SND_Cte_MaxBufferSize / 4)
#define C_Buffer0					0
#define C_Buffer1					1
#define C_BufferZero				2
#define C_BufferUnknown				(-1)

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

static gcSND_tdst_Stream	*gcSND_pst_AllocStream(void);
static void					gcSND_FreeStream(volatile gcSND_tdst_Stream *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

struct SND_tdst_SoundBuffer_	*gcSND_pst_SB_CreateStream(void);
gcSND_tdst_Stream				*gcSND_p_GetStreamFromSoftBuffer(gcSND_tdst_SoftBuffer *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void gcSND_StreamGetNewSettings(gcSND_tdst_Stream *);
static void gcSND_StreamGetChainSettings(gcSND_tdst_Stream *);
static int	gcSND_FillRamBuffer(gcSND_tdst_Stream *);
static void gcSND_StreamStopHardBuffer(volatile gcSND_tdst_Stream *);
static void gcSND_CancelAramLoad(volatile gcSND_tdst_Stream *);
int			gcSND_i_StreamPlay(volatile gcSND_tdst_Stream *);
static int	gcSND_i_GetCurrentBufferId(volatile gcSND_tdst_Stream *);
static void gcSND_i_StreamRead
			(
				GC_tdst_StreamHandler	*_p_FileHandler,
				char					*_pc_Buffer,
				unsigned int			_ui_Size,
				unsigned int			_ui_Position,
				GC_tdpfv_StreamCallBack _pfv_Callback,
				gcSND_tdst_Stream		*_pst_Stream
			);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void gcSND_StreamStop(volatile gcSND_tdst_Stream *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* ARAM */
static void gcSND_StrAramCallbackB1stLoaded(u32 task);
static void gcSND_StrAramCallbackB1Loaded(u32 task);
static void gcSND_StrAramCallbackB0Loaded(u32 task);
static void gcSND_StrAramCallbackStereoB1stLoaded(u32 task);
static void gcSND_StrAramCallbackStereoB0Loaded(u32 task);
static void gcSND_StrAramCallbackStereoB1Loaded(u32 task);

/* DVD - data */
static void gcSND_StrDvdCallbackBufferFilled(s32 result, DVDFileInfo *, unsigned int context);
static void gcSND_StrDvdCallbackBufferHalfFilled(s32 result, DVDFileInfo *, unsigned int context);
static void gcSND_StrDvdCallbackBufferLooped(s32 result, DVDFileInfo *, unsigned int context);
/**/
static void gcSND_StrDvdCallbackStreamRead(s32 result, DVDFileInfo *fileInfo, unsigned int context);
static void gcSND_StrDvdCallback(s32 result, DVDFileInfo *fileInfo, unsigned int context);

/* AX */
static void gcSND_StrAxCallbackChannelDropped(void *);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

gcSND_tdst_Stream volatile	gcSND_gax_StreamList[SND_Cte_MaxSimultaneousStream];
volatile gcSND_tdst_Stream	*gcSND_gpst_CurrentStream;
gcSND_tdst_StereoFile		gcSND_gast_StereoFile[SND_Cte_MaxSimultaneousStereoStream];

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */

inline
unsigned int gcSND_GetCurrentPosition(gcSND_tdst_SoftBuffer	*_pst_Soft)
{
#ifdef _RVL
	return (unsigned int) _pst_Soft->pc_AramBuffer - 10
		 + _pst_Soft->ast_Hard[0].pst_AxBuffer->pb.addr.currentAddressLo;
#else // _RVL
	return uiGetAddrFromHiLo
		(
			_pst_Soft->ast_Hard[0].pst_AxBuffer->pb.addr.currentAddressHi,
			_pst_Soft->ast_Hard[0].pst_AxBuffer->pb.addr.currentAddressLo
		);		
#endif // _RVL
}

void gcSND_StreamInitModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	L_memset(gcSND_gst_PrefetchList, 0, SND_Cte_StreamPrefetchMax * sizeof(gcSND_tdst_Prefetch));
	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		gcSND_gst_PrefetchList[i].pc_AramBuffer = gcSND_pv_StreamAllocARAM(SND_Cte_MaxBufferSize);
		gcSND_M_Assert(gcSND_gst_PrefetchList[i].pc_AramBuffer);
	}

	gcSND_gi_PendingPrefetch = 0;

	L_memset((void *) gcSND_gax_StreamList, 0, SND_Cte_MaxSimultaneousStream * sizeof(gcSND_tdst_Stream));
	L_memset((void *) gcSND_gast_StereoFile, 0, SND_Cte_MaxSimultaneousStereoStream * sizeof(gcSND_tdst_StereoFile));

	for(i = 0; i < SND_Cte_MaxSimultaneousStereoStream; i++)
	{
		gcSND_gast_StereoFile[i].pc_StereoFrameBuffer = MEM_p_Alloc(Cte_RamBufferSize);
		gcSND_M_Assert(gcSND_gast_StereoFile[i].pc_StereoFrameBuffer);

		L_memset(gcSND_gast_StereoFile[i].pc_StereoFrameBuffer, 0, Cte_RamBufferSize);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StreamCloseModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		if(gcSND_gst_PrefetchList[i].pc_AramBuffer) gcSND_StreamFreeARAM(gcSND_gst_PrefetchList[i].pc_AramBuffer);
	}

	L_memset(gcSND_gst_PrefetchList, 0, SND_Cte_StreamPrefetchMax * sizeof(gcSND_tdst_Prefetch));

	for(i = 0; i < SND_Cte_MaxSimultaneousStereoStream; i++)
	{
		MEM_Free(gcSND_gast_StereoFile[i].pc_StereoFrameBuffer);
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrAxCallbackChannelDropped(void *pv)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AXVPB					*pst_AXVPB;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_AXVPB = (AXVPB *) pv;
	pst_Soft = (gcSND_tdst_SoftBuffer *) pst_AXVPB->userContext;
	gcSND_M_ErrX(gcSND_StrAxCallbackChannelDropped, "One stream is dropped : %x !!!", (int) pst_Soft);
	pst_Soft->ast_Hard[0].pst_AxBuffer = NULL;
	pst_Soft->ast_Hard[1].pst_AxBuffer = NULL;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_CreateTrackSB(SND_tdst_SoundBuffer **_ppst_Mono, SND_tdst_SoundBuffer **_ppst_Stereo)
{
	*_ppst_Mono = gcSND_pst_SB_CreateStream();
	*(int *) _ppst_Stereo = gcSND_Cte_StereoFlag | * (int *) _ppst_Mono;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_ReleaseTrackSB(SND_tdst_SoundBuffer *_pst_MonoSB, SND_tdst_SoundBuffer *_pst_StereoSB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				old;
	gcSND_tdst_Stream	*pStream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gcSND_M_CheckSoundBuffPtrOrReturn(_pst_MonoSB, ;);

	old = OSDisableInterrupts();

	pStream = gcSND_p_GetStreamFromSoftBuffer((gcSND_tdst_SoftBuffer *) _pst_MonoSB);
	gcSND_SoftStreamRelease((gcSND_tdst_SoftBuffer *) _pst_MonoSB);

	OSRestoreInterrupts(old);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *gcSND_pst_SB_CreateStream(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_Soft;
	gcSND_tdst_Stream		*pst_Stream;
	AXPBSRC					st_SRC;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    get ressources
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for
	(
		i = 0, pst_Stream = (gcSND_tdst_SoftBuffer *) gcSND_gax_StreamList;
		i < SND_Cte_MaxSimultaneousStream;
		i++, pst_Stream++
	)
	{
		if(pst_Stream->en_State != e_StreamFree) continue;
		pst_Stream->en_State = e_StreamInit;
		break;
	}

	gcSND_M_Assert(i < SND_Cte_MaxSimultaneousStream);	/* no more stream !! */

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    init of soft buffer
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_Soft = gcSND_pst_AllocSoftBuffer();
	gcSND_M_Assert(pst_Soft);	/* no more soft buffer */

	pst_Soft->ui_Flags = SND_Cul_DSF_Used | SND_Cte_StreamedFile;

	/* hight level command */
	pst_Soft->i_Pan = 0;
	pst_Soft->i_SPan = 0;
	pst_Soft->i_Volume = SND_Cte_MinAtt;
	pst_Soft->i_FxVol = SND_Cte_MinAtt;
	pst_Soft->i_Frequency = 0;
	pst_Soft->i_StereoPanIdx = 64;

	/* low level command */
	for(i = 0; i < SND_Cte_FxCoreNb;++i)
		pst_Soft->i_MIXaux[i] = gcSND_Cte_MinVol;
	pst_Soft->i_MIXspan = gcSND_Cte_DefaultSPan;
	pst_Soft->f_SRCratio = 0.0f;
	for(i = 0; i < 2; i++)
	{
		L_memset(&pst_Soft->ast_Hard[i].st_AxAddrSettings, 0, sizeof(AXPBADDR));
		pst_Soft->ast_Hard[i].pst_AxBuffer = AXAcquireVoice
			(
				gcSND_Cte_StreamPriority,
				gcSND_StrAxCallbackChannelDropped,
				(u32) pst_Soft
			);
		gcSND_M_Assert(pst_Soft->ast_Hard[i].pst_AxBuffer); /* no more hard buffer for streaming !!!! */
		pst_Soft->ast_Hard[i].i_MIXinput = gcSND_Cte_DefaultInput;
		pst_Soft->ast_Hard[i].i_MIXpan = gcSND_Cte_DefaultPan;
		MIXInitChannel
		(
			pst_Soft->ast_Hard[i].pst_AxBuffer,
			gcSND_Cte_DefaultMode,
			pst_Soft->ast_Hard[i].i_MIXinput,
			pst_Soft->i_MIXaux[SND_Cte_FxCoreA],
			pst_Soft->i_MIXaux[SND_Cte_FxCoreB],
			#ifdef _RVL
			pst_Soft->i_MIXaux[SND_Cte_FxCoreC],			
			#endif
			pst_Soft->ast_Hard[i].i_MIXpan,
			pst_Soft->i_MIXspan,
			gcSND_Cte_DefaultFader
		);

		st_SRC.ratioHi = 1;
		st_SRC.ratioLo = 0;
		st_SRC.currentAddressFrac = 0;
		st_SRC.last_samples[0] = 0;
		st_SRC.last_samples[1] = 0;
		st_SRC.last_samples[2] = 0;
		st_SRC.last_samples[3] = 0;

		AXSetVoiceSrcType(pst_Soft->ast_Hard[i].pst_AxBuffer, AX_SRC_TYPE_4TAP_8K);
		AXSetVoiceSrc(pst_Soft->ast_Hard[i].pst_AxBuffer, &st_SRC);
	}

	pst_Soft->pst_SndFileHandler = GC_pst_StreamOpen(BIG_gst.h_CLibFileHandle, 2);
	gcSND_M_Assert(pst_Soft->pst_SndFileHandler);

	pst_Soft->pc_RamBuffer = MEM_p_AllocAlign(Cte_RamBufferSize, 64);
	gcSND_M_Assert(pst_Soft->pc_RamBuffer);					/* no more RAM !!! */

	L_memset(&pst_Soft->st_ARQRequest, 0, sizeof(ARQRequest));

	pst_Soft->pc_AramBuffer = (char *) gcSND_pv_StreamAllocARAM(SND_Cte_MaxBufferSize);
	gcSND_M_Assert(pst_Soft->pc_AramBuffer);				/* no more ARAM !!! */

	/* data config */
	pst_Soft->ui_DataPosition = 0;
	pst_Soft->ui_DataSize = 0;

	/* duplication data */
	pst_Soft->pst_DuplicateSrc = NULL;
	pst_Soft->ui_UserNb = 1;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    init stream struct
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_Stream->i_LastCommand = -1;
	pst_Stream->i_LastArg = -1;
	pst_Stream->en_State = e_StreamInit;
	pst_Stream->pst_SoftBuffer = pst_Soft;
	pst_Stream->ui_CurrentDataRead = 0;
	pst_Stream->ui_LastDvdReadSize = 0;
	pst_Stream->p_StereoFileCache = NULL;
	pst_Stream->ui_AramBufferHalfSize = SND_Cte_MaxBufferSize / 2;

	pst_Stream->b_EndReached = 0;
	pst_Stream->ui_UnloopB0 = 0;
	pst_Stream->b_PendingDvdAction = 0;
	pst_Stream->b_PendingAramAction = 0;

	pst_Stream->ui_LastPlayingCursor = 0;
	pst_Stream->i_LastBufferId = 0;
	pst_Stream->i_Loop = 0;
	pst_Stream->ui_ChainSize = 0;

	/* new params */
	L_memset(&pst_Stream->st_NewSettings, 0, sizeof(SND_tdst_StreamSettings));

	return (SND_tdst_SoundBuffer *) pst_Soft;
}

/*
 =======================================================================================================================
    all param in are in oct.
 =======================================================================================================================
 */
void gcSND_StreamReinitAndPlay
(
	SND_tdst_SoundBuffer	*_pSB,
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
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL					old;
	gcSND_tdst_Stream		*pst_Stream;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	int						channel = 1;
	float 					fFxL, fFxR, fFx;
	int 					iFxVol ;
	int 					fader;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pSB) return;

	pst_Soft = gcSND_M_GetBuffer(_pSB);
	pst_Stream = gcSND_p_GetStreamFromSoftBuffer(pst_Soft);
	gcSND_M_Assert(pst_Stream);

	/* dont repeat command */
	//if((pst_Stream->i_LastCommand == gsSND_Cmd_StreamPlay) && (pst_Stream->i_LastArg == _ui_Position)) return;

	/* round sizes */
	if(gcSND_M_IsStereo(_pSB)) channel = 2;
	M_RoundSize(_i_StartPos, channel);
	if(_i_StopPos != -1) M_RoundSize(_i_StopPos, channel);
	M_RoundSize(_ui_LoopBegin, channel);
	M_RoundSize(_ui_LoopEnd, channel);
	M_RoundSize(_ui_Size, channel);

	if(_i_StartPos < 0) _i_StartPos = 0;
	if((unsigned int) _i_StartPos > _ui_Size) _i_StartPos = 0;
	if(_i_LoopNb) _i_LoopNb--;

	if(_i_LoopNb == 0)
	{
		/* if not looping => there is no loop */
		_ui_LoopEnd = _ui_Size;
		_ui_LoopBegin = 0;
	}

	/*$1- now treat the command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	old = OSDisableInterrupts();
	switch(pst_Stream->en_State)
	{
	case e_StreamRamLoadingB1:
	case e_StreamRamLoadingB0:
	case e_StreamRamLoading1st:
		GC_StreamCancel(pst_Soft->pst_SndFileHandler);
		break;

	default:
		break;
	}

	/* reinit order */
	pst_Stream->ui_ChainSize = 0;
	pst_Stream->st_NewSettings.uc_IsOneChainOrder = 0;

	/* fx */
	pst_Stream->st_NewSettings.uc_SendToFxA = 0;
	pst_Stream->st_NewSettings.uc_SendToFxB = 0;
	if(_i_Flag & SND_Cul_SBC_PlayFxA)
		pst_Stream->st_NewSettings.uc_SendToFxA = 1;
	else if(_i_Flag & SND_Cul_SBC_PlayFxB)
		pst_Stream->st_NewSettings.uc_SendToFxB = 1;

	pst_Stream->st_NewSettings.uc_Stereo = gcSND_M_IsStereo(_pSB);
	pst_Stream->st_NewSettings.i_LoopNb = _i_LoopNb;

	pst_Stream->st_NewSettings.ui_DataSize = _ui_Size >> (channel - 1);
	pst_Stream->st_NewSettings.ui_DataPosition = _ui_Position;
	pst_Stream->st_NewSettings.ui_LoopBeginOffset = _ui_LoopBegin >> (channel - 1);
	pst_Stream->st_NewSettings.ui_LoopEndOffset = _ui_LoopEnd >> (channel - 1);
	pst_Stream->st_NewSettings.ui_StartOffset = (*(unsigned int *) &_i_StartPos) >> (channel - 1);
	pst_Stream->st_NewSettings.ui_ExitOffset = 0;
	pst_Stream->st_NewSettings.i_Freq = _i_Freq;
	pst_Stream->st_NewSettings.i_Pan = _i_Pan;
	pst_Stream->st_NewSettings.i_Vol = (int)((float)_i_Vol * 0.1f);
	pst_Stream->st_NewSettings.i_FxL = _i_FxL;
	pst_Stream->st_NewSettings.i_FxR = _i_FxR;

	pst_Stream->i_LastCommand = gsSND_Cmd_StreamPlay;
	pst_Stream->i_LastArg = _ui_Position;

	//$1-
	fFxL = SND_f_GetVolFromAtt(_i_FxL);
	fFxR = SND_f_GetVolFromAtt(_i_FxR);
	fFx = 0.5f*(fFxL + fFxR);
	if(fFx < 0.5f)
	{
		// 0% wet + 100% dry
		fFx = 0.0f;
		fader = gcSND_Cte_MaxVol;
	}
	else if(fFx < 0.7f)
	{
		// 100% wet + 100% dry
		fFx = 1.0f;
		fader = gcSND_Cte_MaxVol;
	}
	else
	{
		// 100% wet + 0% dry
		fFx = 1.0f;
		fader = gcSND_Cte_MinVol; 
	}
	iFxVol = SND_l_GetAttFromVol(fFx);

	pst_Soft->ui_Flags &= ~(SND_Cul_SF_FxA | SND_Cul_SF_FxB);
	pst_Soft->i_FxVol = SND_Cte_MinAtt;
	if(_i_Flag & SND_Cul_SBC_PlayFxA)
	{
		pst_Soft->i_FxVol = iFxVol;
		pst_Soft->ui_Flags |= SND_Cul_SF_FxA;
		gcSND_FxAddSB(pst_Soft);
	}
	else if(_i_Flag & SND_Cul_SBC_PlayFxB)
	{
		pst_Soft->i_FxVol = iFxVol;
		pst_Soft->ui_Flags |= SND_Cul_SF_FxB;
		gcSND_FxAddSB(pst_Soft);
	}
	else
	{
		gcSND_FxDelSB(pst_Soft);
	}

	if(pst_Soft->ast_Hard[0].pst_AxBuffer) 
		MIXSetFader(pst_Soft->ast_Hard[0].pst_AxBuffer, fader);
	if(pst_Soft->ast_Hard[1].pst_AxBuffer) 
		MIXSetFader(pst_Soft->ast_Hard[1].pst_AxBuffer, fader);

	//$1-
	
	// reset play flags
	pst_Soft->ui_Flags &= ~(SND_Cul_SF_StreamIsPlayingNow|SND_Cul_DSF_StartedOnce);
	
	// set for emulating playing state
	pst_Soft->ui_Flags |= SND_Cul_SBS_Playing | (_i_LoopNb ? SND_Cul_SBC_PlayLooping : 0);
	
	OSRestoreInterrupts(old);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_UpdateStream(void)
{
	gcSND_M_Assert(gcSND_gpst_CurrentStream->b_PendingDvdAction >= 0);
	gcSND_M_Assert(gcSND_gpst_CurrentStream->b_PendingAramAction >= 0);

	if(gcSND_gpst_CurrentStream->b_PendingDvdAction) return;
	if(gcSND_gpst_CurrentStream->b_PendingAramAction) return;

	if
	(
		gcSND_gpst_CurrentStream->st_NewSettings.ui_DataPosition
	&&	!gcSND_gpst_CurrentStream->st_NewSettings.uc_IsOneChainOrder
	)
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    new settings
		 ---------------------------------------------------------------------------------------------------------------
		 */

		gcSND_StreamStopHardBuffer(gcSND_gpst_CurrentStream);
		gcSND_gpst_CurrentStream->en_State = e_StreamInit;

		gcSND_StreamGetNewSettings((gcSND_tdst_Stream *) gcSND_gpst_CurrentStream);
		gcSND_FillRamBuffer((gcSND_tdst_Stream *) gcSND_gpst_CurrentStream);
	}
	else
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    normal update of streams
		 ---------------------------------------------------------------------------------------------------------------
		 */

		switch(gcSND_gpst_CurrentStream->en_State)
		{
		case e_StreamInit:
			if(gcSND_gpst_CurrentStream->p_StereoFileCache)
				gcSND_gpst_CurrentStream->p_StereoFileCache->p_Owner = NULL;
			gcSND_gpst_CurrentStream->p_StereoFileCache = NULL;
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case e_StreamAramLoaded1st:
			gcSND_i_StreamPlay(gcSND_gpst_CurrentStream);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case e_StreamAramLoadedB0:
			gcSND_gpst_CurrentStream->en_State = e_StreamWaitingPlayB0;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case e_StreamWaitingPlayB0:
			switch(gcSND_i_GetCurrentBufferId(gcSND_gpst_CurrentStream))
			{
			case C_Buffer0:
				SPY(gcSND_gpst_CurrentStream, update0);
				gcSND_FillRamBuffer((gcSND_tdst_Stream *) gcSND_gpst_CurrentStream);
				break;

			case C_Buffer1:
				break;

			case C_BufferZero:
				SPY(gcSND_gpst_CurrentStream, autostop);
				gcSND_StreamStop(gcSND_gpst_CurrentStream);
				break;

			default:
				gcSND_M_Assert(0);
				break;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case e_StreamAramLoadedB1:
			gcSND_gpst_CurrentStream->en_State = e_StreamWaitingPlayB1;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case e_StreamWaitingPlayB1:
			switch(gcSND_i_GetCurrentBufferId(gcSND_gpst_CurrentStream))
			{
			case C_Buffer0:
				break;

			case C_Buffer1:
				SPY(gcSND_gpst_CurrentStream, update1);
				gcSND_FillRamBuffer((gcSND_tdst_Stream *) gcSND_gpst_CurrentStream);
				break;

			case C_BufferZero:
				SPY(gcSND_gpst_CurrentStream, autostop);
				gcSND_StreamStop(gcSND_gpst_CurrentStream);
				break;

			default:
				gcSND_M_Assert(0);
				break;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case e_StreamStopRequest:
		case e_StreamAutoStopRequest:
			gcSND_StreamStop(gcSND_gpst_CurrentStream);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		default:
			gcSND_M_ErrX(gcSND_UpdateStream, "bad stream state %d", gcSND_gpst_CurrentStream->en_State);
			break;
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	gcSND_gpst_CurrentStream++;
	if(gcSND_gpst_CurrentStream == &gcSND_gax_StreamList[SND_Cte_MaxSimultaneousStream])
		gcSND_gpst_CurrentStream = gcSND_gax_StreamList;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StreamGetChainSettings(gcSND_tdst_Stream *pst_Stream)
{
	/*~~*/
	int i;
	/*~~*/

	SPY(pst_Stream, Get_new_param_chain);

	gcSND_M_Assert(pst_Stream->b_PendingDvdAction == 0);
	gcSND_M_Assert(pst_Stream->b_PendingAramAction == 0);

	if(pst_Stream->st_NewSettings.uc_Stereo)
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    STEREO
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		/* assign one stereo cache buffer */
		if(!pst_Stream->p_StereoFileCache)
		{
			for(i = 0; i < SND_Cte_MaxSimultaneousStereoStream; i++)
			{
				if(gcSND_gast_StereoFile[i].p_Owner) continue;
				pst_Stream->p_StereoFileCache = &gcSND_gast_StereoFile[i];
				break;
			}

			gcSND_M_Assert(i < SND_Cte_MaxSimultaneousStereoStream);
			gcSND_M_Assert(pst_Stream->p_StereoFileCache);
		}

		/* initialize the stereo cache */
		pst_Stream->p_StereoFileCache->p_Owner = pst_Stream;
		pst_Stream->p_StereoFileCache->ui_FilePosition = pst_Stream->st_NewSettings.ui_DataPosition;
		pst_Stream->p_StereoFileCache->ui_FileSize = (pst_Stream->st_NewSettings.ui_LoopEndOffset << 1);
		pst_Stream->p_StereoFileCache->ui_BufferPosition = -1;

		L_memset(pst_Stream->p_StereoFileCache->pc_StereoFrameBuffer, 0, Cte_RamBufferSize);

		/* the stereo aram buffer size is the half of mono one. */
		pst_Stream->ui_AramBufferHalfSize = SND_Cte_MaxBufferSize >> 2;
	}
	else
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    MONO
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		/* free any stereo cache buffer */
		if(pst_Stream->p_StereoFileCache) pst_Stream->p_StereoFileCache->p_Owner = NULL;
		pst_Stream->p_StereoFileCache = NULL;

		/* reinit of the aram buffer size */
		pst_Stream->ui_AramBufferHalfSize = SND_Cte_MaxBufferSize >> 1;
	}

	/* data settings */
	pst_Stream->pst_SoftBuffer->ui_DataSize = pst_Stream->st_NewSettings.ui_DataSize;
	pst_Stream->pst_SoftBuffer->ui_DataPosition = pst_Stream->st_NewSettings.ui_DataPosition;
	pst_Stream->ui_LoopBeginOffset = pst_Stream->st_NewSettings.ui_LoopBeginOffset;
	pst_Stream->ui_LoopEndOffset = pst_Stream->st_NewSettings.ui_LoopEndOffset;

	/* sound settings */
	pst_Stream->pst_SoftBuffer->i_Frequency = 0;//pst_Stream->st_NewSettings.i_Freq;
	pst_Stream->pst_SoftBuffer->f_SRCratio = fGetSRCRatioFromFreq(pst_Stream->st_NewSettings.i_Freq);
	pst_Stream->pst_SoftBuffer->i_Pan = pst_Stream->st_NewSettings.i_Pan;
	pst_Stream->pst_SoftBuffer->i_Volume = pst_Stream->st_NewSettings.i_Vol;
	pst_Stream->i_Loop = pst_Stream->st_NewSettings.i_LoopNb;

	/* state settings */
	pst_Stream->b_EndReached = 0;
	pst_Stream->ui_UnloopB0 = 0;
	pst_Stream->ui_CurrentDataRead = pst_Stream->st_NewSettings.ui_StartOffset;
	pst_Stream->ui_LastDvdReadSize = 0;

	pst_Stream->ui_LastPlayingCursor = 0;

	/* reset the new event */
	L_memset(&pst_Stream->st_NewSettings, 0, sizeof(SND_tdst_StreamSettings));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StreamGetNewSettings(gcSND_tdst_Stream *pst_Stream)
{
	/*~~*/
	int i;
	/*~~*/

	SPY(pst_Stream, Get_new_param);

	gcSND_M_Assert(pst_Stream->b_PendingDvdAction == 0);
	gcSND_M_Assert(pst_Stream->b_PendingAramAction == 0);

	if(pst_Stream->st_NewSettings.uc_Stereo)
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    STEREO
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		/* assign one stereo cache buffer */
		if(!pst_Stream->p_StereoFileCache)
		{
			for(i = 0; i < SND_Cte_MaxSimultaneousStereoStream; i++)
			{
				if(gcSND_gast_StereoFile[i].p_Owner) continue;
				pst_Stream->p_StereoFileCache = &gcSND_gast_StereoFile[i];
				break;
			}

			gcSND_M_Assert(i < SND_Cte_MaxSimultaneousStereoStream);
			gcSND_M_Assert(pst_Stream->p_StereoFileCache);
		}

		/* initialize the stereo cache */
		pst_Stream->p_StereoFileCache->p_Owner = pst_Stream;
		pst_Stream->p_StereoFileCache->ui_FilePosition = pst_Stream->st_NewSettings.ui_DataPosition;
		pst_Stream->p_StereoFileCache->ui_FileSize = (pst_Stream->st_NewSettings.ui_LoopEndOffset << 1);
		pst_Stream->p_StereoFileCache->ui_BufferPosition = -1;

		pst_Stream->p_StereoFileCache->ui_Block = 0;
		pst_Stream->p_StereoFileCache->ui_Read = 0;
		pst_Stream->p_StereoFileCache->ui_Offset = 0;

		pst_Stream->p_StereoFileCache->pc_UserBuffer = NULL;
		pst_Stream->p_StereoFileCache->ui_UserBufferSize = 0;
		pst_Stream->p_StereoFileCache->pfv_UserCallback = NULL;
		pst_Stream->p_StereoFileCache->ui_UserContext = 0;

		L_memset(pst_Stream->p_StereoFileCache->pc_StereoFrameBuffer, 0, Cte_RamBufferSize);

		/* the stereo aram buffer size is the half of mono one. */
		pst_Stream->ui_AramBufferHalfSize = SND_Cte_MaxBufferSize >> 2;
	}
	else
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    MONO
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		/* free any stereo cache buffer */
		if(pst_Stream->p_StereoFileCache) pst_Stream->p_StereoFileCache->p_Owner = NULL;
		pst_Stream->p_StereoFileCache = NULL;

		/* reinit of the aram buffer size */
		pst_Stream->ui_AramBufferHalfSize = SND_Cte_MaxBufferSize >> 1;
	}

	/* data settings */
	pst_Stream->pst_SoftBuffer->ui_DataSize = pst_Stream->st_NewSettings.ui_DataSize;
	pst_Stream->pst_SoftBuffer->ui_DataPosition = pst_Stream->st_NewSettings.ui_DataPosition;
	pst_Stream->ui_LoopBeginOffset = pst_Stream->st_NewSettings.ui_LoopBeginOffset;
	pst_Stream->ui_LoopEndOffset = pst_Stream->st_NewSettings.ui_LoopEndOffset;

	/* sound settings */
	pst_Stream->pst_SoftBuffer->i_Frequency = 0;//pst_Stream->st_NewSettings.i_Freq-1000;
	pst_Stream->pst_SoftBuffer->f_SRCratio = fGetSRCRatioFromFreq(pst_Stream->st_NewSettings.i_Freq);
	pst_Stream->pst_SoftBuffer->i_Pan = pst_Stream->st_NewSettings.i_Pan;
	pst_Stream->pst_SoftBuffer->i_Volume = pst_Stream->st_NewSettings.i_Vol;
	pst_Stream->i_Loop = pst_Stream->st_NewSettings.i_LoopNb;

	/* state settings */
	pst_Stream->b_EndReached = 0;
	pst_Stream->ui_UnloopB0 = 0;
	pst_Stream->ui_CurrentDataRead = pst_Stream->st_NewSettings.ui_StartOffset;
	pst_Stream->ui_LastDvdReadSize = 0;
	pst_Stream->ui_LastPlayingCursor = 0;

	/* reset the new event */
	pst_Stream->ui_ChainSize = 0;
	L_memset(&pst_Stream->st_NewSettings, 0, sizeof(SND_tdst_StreamSettings));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int gcSND_FillRamBuffer(gcSND_tdst_Stream *pst_Stream)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_Soft;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Soft = pst_Stream->pst_SoftBuffer;

	if
	(
		pst_Stream->st_NewSettings.uc_IsOneChainOrder
	&&	(
			(pst_Stream->ui_CurrentDataRead <= pst_Stream->st_NewSettings.ui_ExitOffset)
		&&	(
				pst_Stream->st_NewSettings.ui_ExitOffset <=
					(pst_Stream->ui_CurrentDataRead + pst_Stream->ui_AramBufferHalfSize)
			)
		)
	)
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    chainning
		 ---------------------------------------------------------------------------------------------------------------
		 */

		pst_Stream->ui_LastDvdReadSize = pst_Stream->st_NewSettings.ui_ExitOffset - pst_Stream->ui_CurrentDataRead;

		pst_Stream->ui_ChainSize = pst_Stream->ui_LastDvdReadSize;
		if(pst_Stream->p_StereoFileCache) pst_Stream->ui_ChainSize <<= 1;

		pst_Stream->st_NewSettings.uc_IsOneChainOrder++;

		switch(pst_Stream->en_State)
		{
		case e_StreamInit:
		case e_StreamRamLoading1st:
			pst_Stream->en_State = e_StreamRamLoading1st;
			break;

		case e_StreamAramLoadedB1:
		case e_StreamWaitingPlayB1:
			pst_Stream->en_State = e_StreamRamLoadingB0;
			break;

		case e_StreamAramLoadedB0:
		case e_StreamWaitingPlayB0:
			pst_Stream->en_State = e_StreamRamLoadingB1;
			break;

		case e_StreamAramLoaded1st:
			break;

		default:
			gcSND_M_Assert(0);
			break;
		}

		gcSND_i_StreamRead
		(
			pst_Soft->pst_SndFileHandler,
			pst_Soft->pc_RamBuffer,
			pst_Stream->ui_LastDvdReadSize,
			pst_Soft->ui_DataPosition + pst_Stream->ui_CurrentDataRead,
			gcSND_StrDvdCallbackBufferHalfFilled,
			pst_Stream
		);
	}
	else if(pst_Stream->b_EndReached == 1)
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    1st time end of file detected
		 ---------------------------------------------------------------------------------------------------------------
		 */

        SPY(pst_Stream, b_EndReached_1);
        
		pst_Stream->b_EndReached++;
		L_memset(pst_Soft->pc_RamBuffer, 0, Cte_RamBufferSize);
		M_FlushRamBuffer(pst_Soft->pc_RamBuffer);

		switch(pst_Stream->en_State)
		{
		case e_StreamWaitingPlayB1:
			pst_Stream->b_PendingAramAction++;
			pst_Stream->en_State = e_StreamAramLoadingB0;

			M_FlushRamBuffer(pst_Soft->pc_RamBuffer);
			ARQPostRequest
			(
				&pst_Soft->st_ARQRequest,
				(u32) pst_Stream,
				ARQ_TYPE_MRAM_TO_ARAM,
				ARQ_PRIORITY_HIGH,
				(u32) pst_Soft->pc_RamBuffer,
				(u32) pst_Soft->pc_AramBuffer,
				pst_Stream->ui_AramBufferHalfSize,
				pst_Stream->p_StereoFileCache ? gcSND_StrAramCallbackStereoB0Loaded : gcSND_StrAramCallbackB0Loaded
			);

			break;

		case e_StreamWaitingPlayB0:
			pst_Stream->b_PendingAramAction++;
			pst_Stream->en_State = e_StreamAramLoadingB1;

			M_FlushRamBuffer(pst_Soft->pc_RamBuffer);
			ARQPostRequest
			(
				&pst_Soft->st_ARQRequest,
				(u32) pst_Stream,
				ARQ_TYPE_MRAM_TO_ARAM,
				ARQ_PRIORITY_HIGH,
				(u32) pst_Soft->pc_RamBuffer,
				(u32) pst_Soft->pc_AramBuffer + (pst_Stream->ui_AramBufferHalfSize),
				pst_Stream->ui_AramBufferHalfSize,
				pst_Stream->p_StereoFileCache ? gcSND_StrAramCallbackStereoB1Loaded : gcSND_StrAramCallbackB1Loaded
			);
			break;

		default:
			gcSND_M_ErrX(gcSND_FillRamBuffer, "Bad stream state for stop checking %d", pst_Stream->en_State);
			break;
		}
	}
	else if(pst_Stream->b_EndReached == 2)
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    2nd time end of file detected
		 ---------------------------------------------------------------------------------------------------------------
		 */
        
        SPY(pst_Stream, b_EndReached_2);
        
		pst_Stream->en_State = e_StreamAutoStopRequest;
	}
	else
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    normal refresh
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(pst_Stream->ui_LoopEndOffset > (pst_Stream->ui_CurrentDataRead + pst_Stream->ui_AramBufferHalfSize))
		{

			/*$1- remain data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Stream->ui_LastDvdReadSize = pst_Stream->ui_AramBufferHalfSize;

			switch(pst_Stream->en_State)
			{
			case e_StreamInit:
			case e_StreamRamLoading1st:
				pst_Stream->en_State = e_StreamRamLoading1st;
				break;

			case e_StreamAramLoadedB1:
			case e_StreamWaitingPlayB1:
				pst_Stream->en_State = e_StreamRamLoadingB0;
				break;

			case e_StreamAramLoadedB0:
			case e_StreamWaitingPlayB0:
				pst_Stream->en_State = e_StreamRamLoadingB1;
				break;

			case e_StreamAramLoaded1st:
				break;

			default:
				gcSND_M_Assert(0);
				break;
			}

			gcSND_i_StreamRead
			(
				pst_Soft->pst_SndFileHandler,
				pst_Soft->pc_RamBuffer,
				pst_Stream->ui_LastDvdReadSize,
				pst_Soft->ui_DataPosition + pst_Stream->ui_CurrentDataRead,
				gcSND_StrDvdCallbackBufferFilled,
				pst_Stream
			);
		}
		else
		{

			/*$1- end of file reached ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

            SPY(pst_Stream, b_EndReached_0);
            
			if(pst_Stream->i_Loop == 0) pst_Stream->b_EndReached = 1;
			pst_Stream->ui_LastDvdReadSize = pst_Stream->ui_LoopEndOffset - pst_Stream->ui_CurrentDataRead;

			switch(pst_Stream->en_State)
			{
			case e_StreamInit:
			case e_StreamRamLoading1st:
				pst_Stream->en_State = e_StreamRamLoading1st;
				break;

			case e_StreamAramLoadedB1:
			case e_StreamWaitingPlayB1:
				pst_Stream->en_State = e_StreamRamLoadingB0;
				break;

			case e_StreamAramLoadedB0:
			case e_StreamWaitingPlayB0:
				pst_Stream->en_State = e_StreamRamLoadingB1;
				break;

			case e_StreamAramLoaded1st:
				break;

			default:
				gcSND_M_Assert(0);
				break;
			}

			gcSND_i_StreamRead
			(
				pst_Soft->pst_SndFileHandler,
				pst_Soft->pc_RamBuffer,
				pst_Stream->ui_LastDvdReadSize,
				pst_Soft->ui_DataPosition + pst_Stream->ui_CurrentDataRead,
				gcSND_StrDvdCallbackBufferHalfFilled,
				pst_Stream
			);
		}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    update stream state
	 -------------------------------------------------------------------------------------------------------------------
	 */

	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrDvdCallbackBufferLooped(s32 result, DVDFileInfo *fileInfo, unsigned int context)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_Stream		*pst_Stream;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	char					*pc_Aram;
	void					*pfv_Callback;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Stream = (gcSND_tdst_Stream *) context;
	pst_Soft = pst_Stream->pst_SoftBuffer;

	pst_Stream->b_PendingDvdAction--;

	/* check state */
	switch(pst_Stream->en_State)
	{
	case e_StreamRamLoading1st:
	case e_StreamRamLoadingB0:
	case e_StreamRamLoadingB1:
		break;

	default:
		return;
	}

	if(result == DVD_RESULT_GOOD)
	{
		pst_Stream->ui_CurrentDataRead += pst_Stream->ui_LastDvdReadSize;
		M_FlushRamBuffer(pst_Soft->pc_RamBuffer);

		pc_Aram = pst_Soft->pc_AramBuffer;

		switch(pst_Stream->en_State)
		{
		case e_StreamRamLoading1st:
			if(pst_Stream->p_StereoFileCache)
			{
				pfv_Callback = gcSND_StrAramCallbackStereoB1stLoaded;
				pst_Stream->pst_SoftBuffer->ast_Hard[0].pred_scale = (unsigned short) (*pst_Soft->pc_RamBuffer);
				pst_Stream->pst_SoftBuffer->ast_Hard[1].pred_scale = (unsigned short) (*(pst_Soft->pc_RamBuffer + pst_Stream->ui_AramBufferHalfSize));
			}
			else
			{
				pfv_Callback = gcSND_StrAramCallbackB1stLoaded;
				pst_Stream->pst_SoftBuffer->ast_Hard[0].pred_scale = (unsigned short) (*pst_Soft->pc_RamBuffer);
			}

			pst_Stream->en_State = e_StreamAramLoading1st;
			break;

		case e_StreamRamLoadingB0:
			pfv_Callback = pst_Stream->p_StereoFileCache ? gcSND_StrAramCallbackStereoB0Loaded : gcSND_StrAramCallbackB0Loaded;
			pst_Stream->en_State = e_StreamAramLoadingB0;
			break;

		case e_StreamRamLoadingB1:
			pc_Aram += pst_Stream->ui_AramBufferHalfSize;
			pfv_Callback = pst_Stream->p_StereoFileCache ? gcSND_StrAramCallbackStereoB1Loaded : gcSND_StrAramCallbackB1Loaded;
			pst_Stream->en_State = e_StreamAramLoadingB1;
			break;

		default:
			pfv_Callback = NULL;
			gcSND_M_ErrX(gcSND_StrDvdCallbackBufferLooped, "Bad state in Ram transfert(2) %d", pst_Stream->en_State);
			break;
		}

		if(pfv_Callback)
		{
			pst_Stream->b_PendingAramAction++;

			M_FlushRamBuffer(pst_Soft->pc_RamBuffer);
			ARQPostRequest
			(
				&pst_Soft->st_ARQRequest,
				(u32) pst_Stream,
				ARQ_TYPE_MRAM_TO_ARAM,
				ARQ_PRIORITY_HIGH,
				(u32) pst_Soft->pc_RamBuffer,
				(u32) pc_Aram,
				pst_Stream->ui_AramBufferHalfSize,
				pfv_Callback
			);
		}

		pst_Stream->b_EndReached = 0;
	}
	else
	{
		gcSND_M_ErrX(gcSND_StrDvdCallbackBufferLooped, "Bad state in Ram transfert %d", pst_Stream->en_State);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrDvdCallbackBufferHalfFilled(s32 result, DVDFileInfo *fileInfo, unsigned int context)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_Stream		*pst_Stream;
	unsigned int			ui_SizeYetDone;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	void					*pfv_Callback;
	char					*pc_Aram;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	pst_Stream = (gcSND_tdst_Stream *) context;
	pst_Soft = pst_Stream->pst_SoftBuffer;

	pst_Stream->b_PendingDvdAction--;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	switch(pst_Stream->en_State)
	{
	case e_StreamRamLoading1st:
	case e_StreamRamLoadingB0:
	case e_StreamRamLoadingB1:
		break;

	default:
		return;
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(result == DVD_RESULT_GOOD)
	{
		ui_SizeYetDone = pst_Stream->ui_LastDvdReadSize;
		pst_Stream->ui_CurrentDataRead = pst_Stream->ui_LoopEndOffset;

		if(pst_Stream->st_NewSettings.uc_IsOneChainOrder > 1)
		{
			/* chainning operation */
			gcSND_StreamGetChainSettings(pst_Stream);

			pst_Stream->ui_LastDvdReadSize = pst_Stream->ui_AramBufferHalfSize - ui_SizeYetDone;

			gcSND_i_StreamRead
			(
				pst_Soft->pst_SndFileHandler,
				pst_Soft->pc_RamBuffer + ui_SizeYetDone,
				pst_Stream->ui_LastDvdReadSize,
				pst_Soft->ui_DataPosition + pst_Stream->ui_CurrentDataRead,
				gcSND_StrDvdCallbackBufferLooped,
				pst_Stream
			);
		}
		else if(pst_Stream->i_Loop)
		{
			/* stream is playing loop -> complete buffer with part of the beginning */
            if(pst_Stream->i_Loop != -1) pst_Stream->i_Loop--;

			if(pst_Stream->i_Loop == 0)
			{
				pst_Stream->ui_LoopEndOffset = pst_Soft->ui_DataPosition;
				pst_Stream->p_StereoFileCache->ui_FileSize = (pst_Stream->ui_LoopEndOffset << 1);
			}

			pst_Stream->ui_CurrentDataRead = pst_Stream->ui_LoopBeginOffset;
			pst_Stream->ui_LastDvdReadSize = pst_Stream->ui_AramBufferHalfSize - ui_SizeYetDone;

			gcSND_i_StreamRead
			(
				pst_Soft->pst_SndFileHandler,
				pst_Soft->pc_RamBuffer + ui_SizeYetDone,
				pst_Stream->ui_LastDvdReadSize,
				pst_Soft->ui_DataPosition + pst_Stream->ui_LoopBeginOffset,
				gcSND_StrDvdCallbackBufferLooped,
				pst_Stream
			);
		}
		else
		{

			/*$2
			 -----------------------------------------------------------------------------------------------------------
			    the stream is not looping -> complete buffer with 0
			 -----------------------------------------------------------------------------------------------------------
			 */

			L_memset(pst_Soft->pc_RamBuffer + ui_SizeYetDone, 0, pst_Stream->ui_AramBufferHalfSize - ui_SizeYetDone);
			if(pst_Stream->p_StereoFileCache)
			{
				L_memset
				(
					pst_Soft->pc_RamBuffer + (Cte_RamBufferSize / 2) + ui_SizeYetDone,
					0,
					(pst_Stream->ui_AramBufferHalfSize) - ui_SizeYetDone
				);
			}

			switch(pst_Stream->en_State)
			{
			case e_StreamRamLoading1st:
			case e_StreamRamLoadingB0:
				/* update the state + callback */
				if(pst_Stream->en_State == e_StreamRamLoading1st)
				{
					pst_Stream->en_State = e_StreamAramLoading1st;

					if(pst_Stream->p_StereoFileCache)
					{
						pfv_Callback = gcSND_StrAramCallbackStereoB1stLoaded;
						pst_Stream->pst_SoftBuffer->ast_Hard[0].pred_scale = (unsigned short) (*pst_Soft->pc_RamBuffer);
						pst_Stream->pst_SoftBuffer->ast_Hard[1].pred_scale = (unsigned short) (*(pst_Soft->pc_RamBuffer + pst_Stream->ui_AramBufferHalfSize));
					}
					else
					{
						pfv_Callback = gcSND_StrAramCallbackB1stLoaded;
						pst_Stream->pst_SoftBuffer->ast_Hard[0].pred_scale = (unsigned short) (*pst_Soft->pc_RamBuffer);
					}
				}
				else
				{
					pst_Stream->en_State = e_StreamAramLoadingB0;
					pfv_Callback = pst_Stream->p_StereoFileCache ? gcSND_StrAramCallbackStereoB0Loaded : gcSND_StrAramCallbackB0Loaded;
				}

				/*
				 * set the new end address for auto-stopping £
				 * we cannot do it right now else the player will bug. We have to wait B0 is
				 * playing.
				 */
				pst_Stream->ui_UnloopB0 = ui_SizeYetDone;
				pc_Aram = pst_Soft->pc_AramBuffer;
				break;

			case e_StreamRamLoadingB1:
				/* normal case it is B1 loading */
				pfv_Callback = pst_Stream->p_StereoFileCache ? gcSND_StrAramCallbackStereoB1Loaded : gcSND_StrAramCallbackB1Loaded;
				pst_Stream->en_State = e_StreamAramLoadingB1;

				/* set the new end address for auto-stopping */
				AXSetVoiceLoopAddr(pst_Soft->ast_Hard[0].pst_AxBuffer, sGetStartAddr(gcSND_pv_GetZeroBuffer()));
				AXSetVoiceEndAddr
				(
					pst_Soft->ast_Hard[0].pst_AxBuffer,
					sGetEndAddr(pst_Soft->pc_AramBuffer + pst_Stream->ui_AramBufferHalfSize, ui_SizeYetDone)
				);
				AXSetVoiceLoop(pst_Soft->ast_Hard[0].pst_AxBuffer, AXPBADDR_LOOP_OFF);

				if(pst_Stream->p_StereoFileCache)
				{
					AXSetVoiceLoopAddr(pst_Soft->ast_Hard[1].pst_AxBuffer, sGetStartAddr(gcSND_pv_GetZeroBuffer()));
					AXSetVoiceEndAddr
					(
						pst_Soft->ast_Hard[1].pst_AxBuffer,
						sGetEndAddr
							(
								pst_Soft->pc_AramBuffer + (3 * pst_Stream->ui_AramBufferHalfSize),
								ui_SizeYetDone
							)
					);
					AXSetVoiceLoop(pst_Soft->ast_Hard[1].pst_AxBuffer, AXPBADDR_LOOP_OFF);
				}

				pc_Aram = pst_Soft->pc_AramBuffer + (pst_Stream->ui_AramBufferHalfSize);
				break;

			default:
				pfv_Callback = NULL;
				gcSND_M_ErrX
				(
					gcSND_StrDvdCallbackBufferHalfFilled,
					"Bad state in Ram transfert(2) %d",
					pst_Stream->en_State
				);
				break;
			}

			if(pfv_Callback)
			{
				pst_Stream->b_PendingAramAction++;

				M_FlushRamBuffer(pst_Soft->pc_RamBuffer);
				ARQPostRequest
				(
					&pst_Soft->st_ARQRequest,
					(u32) pst_Stream,
					ARQ_TYPE_MRAM_TO_ARAM,
					ARQ_PRIORITY_HIGH,
					(u32) pst_Soft->pc_RamBuffer,
					(u32) pc_Aram,
					pst_Stream->ui_AramBufferHalfSize,
					pfv_Callback
				);
			}
		}
	}
	else
	{
		gcSND_M_ErrX(gcSND_StrDvdCallbackBufferHalfFilled, "Bad state in Ram transfert %d", pst_Stream->en_State);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrDvdCallbackBufferFilled(s32 result, DVDFileInfo *fileInfo, unsigned int context)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_Stream		*pst_Stream;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	void					*pfv_Callback;
	char					*pc_Aram;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Stream = (gcSND_tdst_Stream *) context;
	pst_Stream->b_PendingDvdAction--;

	switch(pst_Stream->en_State)
	{
	case e_StreamRamLoading1st:
	case e_StreamRamLoadingB0:
	case e_StreamRamLoadingB1:
		break;

	default:
		return;
	}

	if(result == DVD_RESULT_GOOD)
	{
		pst_Soft = pst_Stream->pst_SoftBuffer;
		pst_Stream->ui_CurrentDataRead += pst_Stream->ui_LastDvdReadSize;
		M_FlushRamBuffer(pst_Soft->pc_RamBuffer);

		pc_Aram = pst_Soft->pc_AramBuffer;
		switch(pst_Stream->en_State)
		{
		case e_StreamRamLoading1st:
		case e_StreamInit:
			if(pst_Stream->p_StereoFileCache)
			{
				pfv_Callback = gcSND_StrAramCallbackStereoB1stLoaded;
				pst_Stream->pst_SoftBuffer->ast_Hard[0].pred_scale = (unsigned short) (*pst_Soft->pc_RamBuffer);
				pst_Stream->pst_SoftBuffer->ast_Hard[1].pred_scale = (unsigned short) (*(pst_Soft->pc_RamBuffer + pst_Stream->ui_AramBufferHalfSize));
			}
			else
			{
				pfv_Callback = gcSND_StrAramCallbackB1stLoaded;
				pst_Stream->pst_SoftBuffer->ast_Hard[0].pred_scale = (unsigned short) (*pst_Soft->pc_RamBuffer);
			}

			pst_Stream->en_State = e_StreamAramLoading1st;
			break;

		case e_StreamRamLoadingB0:
			pfv_Callback = pst_Stream->p_StereoFileCache ? gcSND_StrAramCallbackStereoB0Loaded : gcSND_StrAramCallbackB0Loaded;
			pst_Stream->en_State = e_StreamAramLoadingB0;
			break;

		case e_StreamRamLoadingB1:
			pfv_Callback = pst_Stream->p_StereoFileCache ? gcSND_StrAramCallbackStereoB1Loaded : gcSND_StrAramCallbackB1Loaded;
			pst_Stream->en_State = e_StreamAramLoadingB1;
			pc_Aram += (pst_Stream->ui_AramBufferHalfSize);
			break;

		default:
			gcSND_M_ErrX(gcSND_StrDvdCallbackBufferFilled, "Bad state in Ram transfert(2) %d", pst_Stream->en_State);
			return;
		}

		pst_Stream->b_PendingAramAction++;

		M_FlushRamBuffer(pst_Soft->pc_RamBuffer);
		ARQPostRequest
		(
			&pst_Soft->st_ARQRequest,
			(u32) pst_Stream,
			ARQ_TYPE_MRAM_TO_ARAM,
			ARQ_PRIORITY_HIGH,
			(u32) pst_Soft->pc_RamBuffer,
			(u32) pc_Aram,
			pst_Stream->ui_AramBufferHalfSize,
			pfv_Callback
		);
	}
	else
	{
		gcSND_M_ErrX(gcSND_StrDvdCallbackBufferFilled, "Bad state in Ram transfert %d", pst_Stream->en_State);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrAramCallbackStereoB1stLoaded(u32 task)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ARQRequest			*p;
	gcSND_tdst_Stream	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p = (ARQRequest *) task;
	pst_Stream = (gcSND_tdst_Stream *) p->owner;

	pst_Stream->b_PendingAramAction--;
	pst_Stream->b_PendingAramAction++;

	M_FlushRamBuffer(pst_Stream->pst_SoftBuffer->pc_RamBuffer);
	ARQPostRequest
	(
		&pst_Stream->pst_SoftBuffer->st_ARQRequest,
		(u32) pst_Stream,
		ARQ_TYPE_MRAM_TO_ARAM,
		ARQ_PRIORITY_HIGH,
		(u32) pst_Stream->pst_SoftBuffer->pc_RamBuffer + (Cte_RamBufferSize / 2),
		(u32) pst_Stream->pst_SoftBuffer->pc_AramBuffer + (2 * pst_Stream->ui_AramBufferHalfSize),
		pst_Stream->ui_AramBufferHalfSize,
		gcSND_StrAramCallbackB1stLoaded
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrAramCallbackStereoB0Loaded(u32 task)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ARQRequest			*p;
	gcSND_tdst_Stream	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p = (ARQRequest *) task;
	pst_Stream = (gcSND_tdst_Stream *) p->owner;

	pst_Stream->b_PendingAramAction--;
	pst_Stream->b_PendingAramAction++;

	M_FlushRamBuffer(pst_Stream->pst_SoftBuffer->pc_RamBuffer);
	ARQPostRequest
	(
		&pst_Stream->pst_SoftBuffer->st_ARQRequest,
		(u32) pst_Stream,
		ARQ_TYPE_MRAM_TO_ARAM,
		ARQ_PRIORITY_HIGH,
		(u32) pst_Stream->pst_SoftBuffer->pc_RamBuffer + (Cte_RamBufferSize / 2),
		(u32) pst_Stream->pst_SoftBuffer->pc_AramBuffer + (2 * pst_Stream->ui_AramBufferHalfSize),
		pst_Stream->ui_AramBufferHalfSize,
		gcSND_StrAramCallbackB0Loaded
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrAramCallbackStereoB1Loaded(u32 task)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ARQRequest			*p;
	gcSND_tdst_Stream	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p = (ARQRequest *) task;
	pst_Stream = (gcSND_tdst_Stream *) p->owner;

	pst_Stream->b_PendingAramAction--;
	pst_Stream->b_PendingAramAction++;

	M_FlushRamBuffer(pst_Stream->pst_SoftBuffer->pc_RamBuffer);

	ARQPostRequest
	(
		&pst_Stream->pst_SoftBuffer->st_ARQRequest,
		(u32) pst_Stream,
		ARQ_TYPE_MRAM_TO_ARAM,
		ARQ_PRIORITY_HIGH,
		(u32) pst_Stream->pst_SoftBuffer->pc_RamBuffer + (Cte_RamBufferSize / 2),
		(u32) pst_Stream->pst_SoftBuffer->pc_AramBuffer + (2 * pst_Stream->ui_AramBufferHalfSize) + (pst_Stream->ui_AramBufferHalfSize),
		pst_Stream->ui_AramBufferHalfSize,
		gcSND_StrAramCallbackB1Loaded
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrAramCallbackB1stLoaded(u32 task)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ARQRequest			*p;
	gcSND_tdst_Stream	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p = (ARQRequest *) task;
	pst_Stream = (gcSND_tdst_Stream *) p->owner;
	pst_Stream->b_PendingAramAction--;

	/* check state */
	if(pst_Stream->en_State != e_StreamAramLoading1st) return;
	pst_Stream->en_State = e_StreamAramLoaded1st;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrAramCallbackB0Loaded(u32 task)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ARQRequest			*p;
	gcSND_tdst_Stream	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p = (ARQRequest *) task;
	pst_Stream = (gcSND_tdst_Stream *) p->owner;
	pst_Stream->b_PendingAramAction--;

	/* check current state */
	if(pst_Stream->en_State != e_StreamAramLoadingB0) return;
	pst_Stream->en_State = e_StreamAramLoadedB0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrAramCallbackB1Loaded(u32 task)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ARQRequest			*p;
	gcSND_tdst_Stream	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p = (ARQRequest *) task;
	pst_Stream = (gcSND_tdst_Stream *) p->owner;
	pst_Stream->b_PendingAramAction--;

	/* check state */
	if(pst_Stream->en_State != e_StreamAramLoadingB1) return;
	pst_Stream->en_State = e_StreamAramLoadedB1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StreamStopHardBuffer(volatile gcSND_tdst_Stream *pst_Stream)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_Soft;
	int						cc;
	volatile u16			*pstate;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Soft = (gcSND_tdst_SoftBuffer *) pst_Stream->pst_SoftBuffer;

	for(cc = 0; cc < 2; cc++)
	{
		if(pst_Soft->ast_Hard[cc].pst_AxBuffer == NULL) continue;

		if(pst_Soft->ast_Hard[cc].pst_AxBuffer->pb.state != AX_PB_STATE_STOP)
		{
		    pst_Soft->ui_Flags &= ~(SND_Cul_SBS_Playing|SND_Cul_SF_StreamIsPlayingNow|SND_Cul_DSF_StartedOnce);
		    	
			AXSetVoiceState(pst_Soft->ast_Hard[cc].pst_AxBuffer, AX_PB_STATE_STOP);
			pstate = &pst_Soft->ast_Hard[cc].pst_AxBuffer->pb.state;
			gcSND_M_WaitWhile(*pstate != AX_PB_STATE_STOP);
		}
	}

	SPY(pst_Stream, quick_stop);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_CancelAramLoad(volatile gcSND_tdst_Stream *pst_Stream)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_Soft;
	BOOL					old;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	old = OSDisableInterrupts();
	pst_Soft = pst_Stream->pst_SoftBuffer;
	ARQRemoveOwnerRequest((u32) pst_Stream);
	OSRestoreInterrupts(old);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_StreamPlay(volatile gcSND_tdst_Stream *pst_Stream)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	BOOL					old;
	AXPBSRC					st_SRC;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Soft = pst_Stream->pst_SoftBuffer;

	old = OSDisableInterrupts();

	pst_Stream->i_LastBufferId = 0;

	/*$1- reinit of params ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gcSND_SB_SetFrequency
	(
		(gcSND_tdst_SoftBuffer *) pst_Stream->pst_SoftBuffer,
		24000
	);
	gcSND_SB_SetPan
	(
		(gcSND_tdst_SoftBuffer *) pst_Stream->pst_SoftBuffer,
		pst_Stream->pst_SoftBuffer->i_Pan,
		pst_Stream->pst_SoftBuffer->i_SPan
	);
	
	// ne sert pas, est meme nuisible pour prendre en compte la valeur du volume en temps reel
	// gcSND_SB_SetVolume((gcSND_tdst_SoftBuffer *) pst_Stream->pst_SoftBuffer, pst_Stream->pst_SoftBuffer->i_Volume);
	//

	/*$1- reinit addr ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < 2; i++)
	{
		if(!pst_Soft->ast_Hard[i].pst_AxBuffer)
    	{
    		L_memset(&pst_Soft->ast_Hard[i].st_AxAddrSettings, 0, sizeof(AXPBADDR));
    		pst_Soft->ast_Hard[i].pst_AxBuffer = AXAcquireVoice
    			(
    				gcSND_Cte_StreamPriority,
    				gcSND_StrAxCallbackChannelDropped,
    				(u32) pst_Soft
    			);
    			
    		gcSND_M_Assert(pst_Soft->ast_Hard[i].pst_AxBuffer);

    		if(pst_Soft->ast_Hard[i].pst_AxBuffer)
    		{

        		MIXInitChannel
        		(
        			pst_Soft->ast_Hard[i].pst_AxBuffer,
        			gcSND_Cte_DefaultMode,
        			pst_Soft->ast_Hard[i].i_MIXinput,
        			pst_Soft->i_MIXaux[SND_Cte_FxCoreA],
        			pst_Soft->i_MIXaux[SND_Cte_FxCoreB],
        			#ifdef _RVL
        			pst_Soft->i_MIXaux[SND_Cte_FxCoreC],        			
        			#endif
        			pst_Soft->ast_Hard[i].i_MIXpan,
        			pst_Soft->i_MIXspan,
        			gcSND_Cte_DefaultFader
        		);

        		st_SRC.ratioHi = 1;
        		st_SRC.ratioLo = 0;
        		st_SRC.currentAddressFrac = 0;
        		st_SRC.last_samples[0] = 0;
        		st_SRC.last_samples[1] = 0;
        		st_SRC.last_samples[2] = 0;
        		st_SRC.last_samples[3] = 0;

        		AXSetVoiceSrcType(pst_Soft->ast_Hard[i].pst_AxBuffer, AX_SRC_TYPE_4TAP_8K);
        		AXSetVoiceSrc(pst_Soft->ast_Hard[i].pst_AxBuffer, &st_SRC);

            	gcSND_SB_SetFrequency
            	(
            		(gcSND_tdst_SoftBuffer *) pst_Stream->pst_SoftBuffer,
            		24000
            	);
            	gcSND_SB_SetPan
            	(
            		(gcSND_tdst_SoftBuffer *) pst_Stream->pst_SoftBuffer,
            		pst_Stream->pst_SoftBuffer->i_Pan,
            		pst_Stream->pst_SoftBuffer->i_SPan
            	);
    		}
    	}

		if(!pst_Soft->ast_Hard[i].pst_AxBuffer)
		{
			gcSND_M_Err(gcSND_i_StreamPlay, "Play failed, can not alloc one voice");
			OSRestoreInterrupts(old);
			return -1;
		}

		gcSND_gst_GameCubeCoefficientsTable.pred_scale = pst_Soft->ast_Hard[i].pred_scale;
		gcSND_gst_GameCubeCoefficientsTable.loop_pred_scale = 0;

		AXSetVoiceType(pst_Soft->ast_Hard[i].pst_AxBuffer, AX_PB_TYPE_STREAM);
		AXSetVoiceAdpcm(pst_Soft->ast_Hard[i].pst_AxBuffer, &gcSND_gst_GameCubeCoefficientsTable);
		AXSetVoiceAdpcmLoop(pst_Soft->ast_Hard[i].pst_AxBuffer, &gcSND_gst_GameCubeCoefficientsTable.loop_pred_scale);

		if(pst_Stream->b_EndReached)
		{
		    SPY(pst_Stream, b_EndReached_au_play);
		    
			AXSetVoiceCurrentAddr
			(
				pst_Soft->ast_Hard[i].pst_AxBuffer,
				sGetStartAddr(pst_Soft->pc_AramBuffer + (i * 2 * pst_Stream->ui_AramBufferHalfSize))
			);

    		if(pst_Stream->ui_UnloopB0)
    		{
    		    SPY(pst_Stream, use_ui_UnloopB0);
    		    
    			AXSetVoiceLoopAddr(pst_Soft->ast_Hard[0].pst_AxBuffer, sGetStartAddr(gcSND_pv_GetZeroBuffer()));
    			AXSetVoiceEndAddr
    			(
    				pst_Soft->ast_Hard[0].pst_AxBuffer,
    				sGetEndAddr(pst_Soft->pc_AramBuffer, pst_Stream->ui_UnloopB0)
    			);
    			AXSetVoiceLoop(pst_Soft->ast_Hard[0].pst_AxBuffer, AXPBADDR_LOOP_OFF);

    			if(pst_Stream->p_StereoFileCache)
    			{
    				AXSetVoiceLoopAddr(pst_Soft->ast_Hard[1].pst_AxBuffer, sGetStartAddr(gcSND_pv_GetZeroBuffer()));
    				AXSetVoiceEndAddr
    				(
    					pst_Soft->ast_Hard[1].pst_AxBuffer,
    					sGetEndAddr
    						(
    							pst_Soft->pc_AramBuffer + (2 * pst_Stream->ui_AramBufferHalfSize),
    							pst_Stream->ui_UnloopB0
    						)
    				);
    				AXSetVoiceLoop(pst_Soft->ast_Hard[1].pst_AxBuffer, AXPBADDR_LOOP_OFF);
    			}

    			pst_Stream->ui_UnloopB0 = 0;
    		}
			/* the other address has been changed by the fill-in function */
		}
		else
		{
			AXSetVoiceLoop(pst_Soft->ast_Hard[i].pst_AxBuffer, AXPBADDR_LOOP_ON);
			AXSetVoiceLoopAddr
			(
				pst_Soft->ast_Hard[i].pst_AxBuffer,
				sGetStartAddr(pst_Soft->pc_AramBuffer + (i * 2 * pst_Stream->ui_AramBufferHalfSize))
			);
			AXSetVoiceEndAddr
			(
				pst_Soft->ast_Hard[i].pst_AxBuffer,
				sGetEndAddr
					(
						pst_Soft->pc_AramBuffer + (i * 2 * pst_Stream->ui_AramBufferHalfSize),
						2 * pst_Stream->ui_AramBufferHalfSize
					)
			);
			AXSetVoiceCurrentAddr
			(
				pst_Soft->ast_Hard[i].pst_AxBuffer,
				sGetStartAddr(pst_Soft->pc_AramBuffer + (i * 2 * pst_Stream->ui_AramBufferHalfSize))
			);
		}

		AXSetVoiceState(pst_Soft->ast_Hard[i].pst_AxBuffer, AX_PB_STATE_RUN);
		pst_Soft->ui_Flags |= SND_Cul_SF_StreamIsPlayingNow;

		if(!pst_Stream->p_StereoFileCache) break;
	}

	pst_Stream->en_State = e_StreamWaitingPlayB0;
	SPY(pst_Stream, play);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OSRestoreInterrupts(old);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int gcSND_i_GetCurrentBufferId(volatile gcSND_tdst_Stream *pst_Stream)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			ui_CurrentPosition;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	unsigned int			ui_AramZero;
	unsigned int			ui_AramZeroEnd;
	unsigned int			ui_AramStart;
	unsigned int			ui_AramMiddle;
	unsigned int			ui_AramEnd;
	int						i_B;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Soft = pst_Stream->pst_SoftBuffer;
	if(!pst_Soft->ast_Hard[0].pst_AxBuffer) return -1;
	if(pst_Soft->ast_Hard[0].pst_AxBuffer->pb.state == AX_PB_STATE_STOP) return 0;

	ui_AramZero = (unsigned int) gcSND_pv_GetZeroBuffer() - 10;
	ui_AramZeroEnd = ui_AramZero + gcSND_Cte_ZeroBufferSize + 10;

	ui_AramStart = (unsigned int) pst_Soft->pc_AramBuffer - 10;
	ui_AramMiddle = ui_AramStart + pst_Stream->ui_AramBufferHalfSize;
	ui_AramEnd = ui_AramStart + (2 * pst_Stream->ui_AramBufferHalfSize) + 10;

	ui_CurrentPosition = gcSND_GetCurrentPosition(pst_Soft);

	if((ui_AramStart <= ui_CurrentPosition) && (ui_CurrentPosition < ui_AramMiddle))
	{
		i_B = C_Buffer0;
	}
	else if((ui_AramMiddle <= ui_CurrentPosition) && (ui_CurrentPosition <= ui_AramEnd))
	{
		i_B = C_Buffer1;
	}
	else if((ui_AramZero <= ui_CurrentPosition) && (ui_CurrentPosition <= ui_AramZeroEnd))
	{
		i_B = C_BufferZero;
	}
	else
	{
		//i_B = C_BufferUnknown;
		i_B = C_Buffer0;
#ifdef _DEBUG
		OSReport
		(
			"[SND:%s %d] *** bad buffer id %x: %x Buffer %x-%x-%x, zero %x-%x\n",
			__FILE__,__LINE__,
			pst_Soft->ast_Hard[0].pst_AxBuffer,
			ui_CurrentPosition,
			ui_AramStart,
			ui_AramMiddle,
			ui_AramEnd,
			ui_AramZero,
			ui_AramZeroEnd
		);
#endif
	}

	if((i_B != C_BufferUnknown) && (pst_Stream->i_LastBufferId != i_B))
	{
		pst_Stream->i_LastBufferId = i_B;
		pst_Stream->ui_LastPlayingCursor += (pst_Stream->ui_AramBufferHalfSize);
	}

	if(i_B == C_Buffer0)
	{
		if(pst_Stream->ui_UnloopB0)
		{
			AXSetVoiceLoopAddr(pst_Soft->ast_Hard[0].pst_AxBuffer, sGetStartAddr(gcSND_pv_GetZeroBuffer()));
			AXSetVoiceEndAddr
			(
				pst_Soft->ast_Hard[0].pst_AxBuffer,
				sGetEndAddr(pst_Soft->pc_AramBuffer, pst_Stream->ui_UnloopB0)
			);
			AXSetVoiceLoop(pst_Soft->ast_Hard[0].pst_AxBuffer, AXPBADDR_LOOP_OFF);

			if(pst_Stream->p_StereoFileCache)
			{
				AXSetVoiceLoopAddr(pst_Soft->ast_Hard[1].pst_AxBuffer, sGetStartAddr(gcSND_pv_GetZeroBuffer()));
				AXSetVoiceEndAddr
				(
					pst_Soft->ast_Hard[1].pst_AxBuffer,
					sGetEndAddr
						(
							pst_Soft->pc_AramBuffer + (2 * pst_Stream->ui_AramBufferHalfSize),
							pst_Stream->ui_UnloopB0
						)
				);
				AXSetVoiceLoop(pst_Soft->ast_Hard[1].pst_AxBuffer, AXPBADDR_LOOP_OFF);
			}

			pst_Stream->ui_UnloopB0 = 0;
		}
	}

	return i_B;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_Stream_GetCurrPos(struct SND_tdst_SoundBuffer_ *_pst_SB, int *_pi_Pos, int *_pi_Write)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			ui_CurrentPosition;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	unsigned int			ui_AramStart;
	unsigned int			ui_AramEnd;
	float					f;
	gcSND_tdst_Stream		*pst_Stream;
	int						channel;
	unsigned int			ui_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_pi_Pos = *_pi_Write = 0;
	pst_Soft = gcSND_M_GetBuffer(_pst_SB);
	if(!pst_Soft->ast_Hard[0].pst_AxBuffer) return;
	if(pst_Soft->ast_Hard[0].pst_AxBuffer->pb.state == AX_PB_STATE_STOP) return;

	/* init */
	pst_Stream = gcSND_p_GetStreamFromSoftBuffer(pst_Soft);
	channel = 1;
	if(pst_Stream->p_StereoFileCache) channel = 2;

	/* get curr pos from the buffer */
	ui_CurrentPosition = gcSND_GetCurrentPosition(pst_Soft);

	/* normalize the position */
	ui_AramStart = (unsigned int) pst_Soft->pc_AramBuffer;
	ui_AramEnd = ui_AramStart + (2 * pst_Stream->ui_AramBufferHalfSize);
	f = fLongToFloat(ui_CurrentPosition - ui_AramStart) / fLongToFloat(2 * pst_Stream->ui_AramBufferHalfSize);

	/* scale the position with the buffer size */
	f *= SND_Cte_MaxBufferSize;				/* => size in oct (same for stereo and mono) */

	/* get only the remainder */
	ui_Pos = lFloatToLong(f);
	ui_Pos = ui_Pos % (SND_Cte_MaxBufferSize / 2);

	/* add the modulo */
	gcSND_i_GetCurrentBufferId(pst_Stream); /* force the update of the PlayingCursor */
	ui_Pos += (pst_Stream->ui_LastPlayingCursor << (channel - 1));
	*(unsigned int *) _pi_Pos = SND_ui_SizeToSample(SND_Cte_DefaultWaveFormat, channel, ui_Pos);

	/* get the write pointer */
	ui_Pos = pst_Stream->ui_CurrentDataRead << (channel - 1);
	*(unsigned int *) _pi_Write = SND_ui_SizeToSample(SND_Cte_DefaultWaveFormat, channel, ui_Pos);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StreamStop(volatile gcSND_tdst_Stream *pst_Stream)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_Soft;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gcSND_SB_SetVolume(pst_Stream->pst_SoftBuffer, -10000);
	gcSND_StreamStopHardBuffer(pst_Stream);
	pst_Soft = pst_Stream->pst_SoftBuffer;
	pst_Soft->ui_Flags &= ~(SND_Cul_SBS_Playing|SND_Cul_SF_StreamIsPlayingNow|SND_Cul_DSF_StartedOnce);

	pst_Stream->ui_CurrentDataRead = pst_Stream->ui_LoopBeginOffset;
	pst_Stream->ui_LastDvdReadSize = 0;
	pst_Stream->ui_LastPlayingCursor = 0;
	pst_Stream->i_LastBufferId = 0;
	pst_Stream->b_EndReached = 0;
	pst_Stream->ui_UnloopB0 = 0;
	pst_Stream->en_State = e_StreamInit;
	pst_Stream->i_LastCommand = gsSND_Cmd_StreamStop;
	pst_Stream->i_LastArg = 0;

	if(pst_Stream->p_StereoFileCache) pst_Stream->p_StereoFileCache->p_Owner = NULL;
	pst_Stream->p_StereoFileCache = NULL;

	SPY(pst_Stream, stop);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
gcSND_tdst_Stream *gcSND_p_GetStreamFromSoftBuffer(gcSND_tdst_SoftBuffer *_pst_Soft)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	gcSND_tdst_Stream	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Soft) return NULL;

	for
	(
		i = 0, pst_Stream = (gcSND_tdst_Stream *) gcSND_gax_StreamList;
		i < SND_Cte_MaxSimultaneousStream;
		i++, pst_Stream++
	)
	{
		if(pst_Stream->pst_SoftBuffer == _pst_Soft) return pst_Stream;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StopStreamedSB(SND_tdst_SoundBuffer *pst_Sound)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	volatile gcSND_tdst_Stream	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Stream = gcSND_p_GetStreamFromSoftBuffer(gcSND_M_GetBuffer(pst_Sound));
	if(pst_Stream) gcSND_StreamStop(pst_Stream);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_ShutDownStreams(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	volatile gcSND_tdst_Stream	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	for(i = 0; i < SND_Cte_MaxSimultaneousStream; i++)
	{
		pst_Stream = gcSND_gax_StreamList + i;

		/* if(pst_Stream->en_State == e_StreamInit) continue; */
		gcSND_StreamStop(pst_Stream);
		gcSND_FxDelSB((SND_tdst_SoundBuffer *) pst_Stream->pst_SoftBuffer);

		if(pst_Stream->b_PendingDvdAction)
		{
			GC_StreamCancel(pst_Stream->pst_SoftBuffer->pst_SndFileHandler);
			gcSND_M_WaitWhile(pst_Stream->b_PendingDvdAction);
		}

		pst_Stream->ui_ChainSize = 0;
		L_memset((void *) &pst_Stream->st_NewSettings, 0, sizeof(SND_tdst_StreamSettings));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StreamLoopCountGet(SND_tdst_SoundBuffer *_pst_SoundBuffer, int *_pi_LoopCount)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	volatile gcSND_tdst_Stream	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_pi_LoopCount = 0;
	pst_Stream = gcSND_p_GetStreamFromSoftBuffer(gcSND_M_GetBuffer(_pst_SoundBuffer));
	if(pst_Stream) *_pi_LoopCount = pst_Stream->i_Loop;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StreamChainDelayGet(SND_tdst_SoundBuffer *_pst_SoundBuffer, float *_pf_Delay)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	volatile gcSND_tdst_Stream	*pst_Stream;
	unsigned int				uiSample;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_pf_Delay = 0;
	pst_Stream = gcSND_p_GetStreamFromSoftBuffer(gcSND_M_GetBuffer(_pst_SoundBuffer));
	if(!pst_Stream) return;
	if(!pst_Stream->pst_SoftBuffer->i_Frequency) return;

	uiSample = SND_ui_SizeToSample
		(
			SND_Cte_DefaultWaveFormat,
			(pst_Stream->p_StereoFileCache ? 2 : 1),
			pst_Stream->ui_ChainSize
		);

	*_pf_Delay = fLongToFloat(uiSample) / fLongToFloat(pst_Stream->pst_SoftBuffer->i_Frequency);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_i_StreamRead
(
	GC_tdst_StreamHandler	*_p_FileHandler,
	char					*_pc_Buffer,
	unsigned int			_ui_Size,
	unsigned int			_ui_Seek,
	GC_tdpfv_StreamCallBack _pfv_Callback,
	gcSND_tdst_Stream		*_pst_Stream
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_StereoFile	*p_Stereo;
	unsigned int			uiRead, uiFrame, uiBlock;
	unsigned int			uiSizeByChannel;
	unsigned int			i;
	unsigned int			ui_FileCurrSeek, ui_BufferCurrSeek;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Stream->p_StereoFileCache)
	{
		L_memset(_pc_Buffer, 0, _ui_Size);

		/* no dvd action now */
		p_Stereo = _pst_Stream->p_StereoFileCache;
		gcSND_M_Assert(_ui_Seek >= p_Stereo->ui_FilePosition);

		uiSizeByChannel = _ui_Size;

		/* relative seeking */
		_ui_Seek -= p_Stereo->ui_FilePosition;

		/* eval the frame seek */
		uiFrame = _ui_Seek / Cte_MuxOneChannelFrameSize;
		uiFrame *= (2 * Cte_MuxOneChannelFrameSize);

		/* get the remainder */
		while(_ui_Seek >= Cte_MuxOneChannelFrameSize) _ui_Seek -= Cte_MuxOneChannelFrameSize;

		/* get seek */
		ui_FileCurrSeek = p_Stereo->ui_FilePosition + uiFrame;
		ui_BufferCurrSeek = _ui_Seek;

		/* get the max size we can read (by channel) */
		uiBlock = min((p_Stereo->ui_FileSize - uiFrame) / 2, Cte_MuxOneChannelFrameSize);

		/* read data (?) */
		uiRead = min(uiSizeByChannel, uiBlock - ui_BufferCurrSeek);

		if(ui_FileCurrSeek != p_Stereo->ui_BufferPosition)
		{
			p_Stereo->pc_UserBuffer = _pc_Buffer;
			p_Stereo->ui_UserBufferSize = _ui_Size;
			p_Stereo->pfv_UserCallback = _pfv_Callback;
			p_Stereo->ui_UserContext = (unsigned int) _pst_Stream;

			p_Stereo->ui_Block = uiBlock;
			p_Stereo->ui_Read = uiRead;
			p_Stereo->ui_Offset = ui_BufferCurrSeek;
			p_Stereo->ui_BufferPosition = ui_FileCurrSeek;

			_pst_Stream->b_PendingDvdAction++;
			Overloaded_GC_i_StreamRead
			(
				_p_FileHandler,
				p_Stereo->pc_StereoFrameBuffer,
				2 * uiBlock,
				p_Stereo->ui_BufferPosition,
				gcSND_StrDvdCallbackStreamRead,
				(unsigned int) _pst_Stream
			);
			return;
		}

		/* copy to user buffer */
		for(i = 0; i < 2; i++)
		{
			L_memcpy
			(
				_pc_Buffer + (i * Cte_MuxOneChannelFrameSize),
				p_Stereo->pc_StereoFrameBuffer + ui_BufferCurrSeek + (i * uiBlock),
				uiRead
			);
		}

		/* update size+pointer */
		_pc_Buffer += uiRead;
		uiSizeByChannel -= uiRead;

		/* any more ? */
		if(uiSizeByChannel)
		{
			/* goto next frame */
			p_Stereo->ui_BufferPosition += 2 * Cte_MuxOneChannelFrameSize;

			if(p_Stereo->ui_BufferPosition >= (p_Stereo->ui_FilePosition + p_Stereo->ui_FileSize))
			{
				/* end-of-file : no more data */
				if(_pfv_Callback)
				{
					_pst_Stream->b_PendingDvdAction++;
					_pfv_Callback(DVD_RESULT_GOOD, &_p_FileHandler->st_FileInfo, (unsigned int) _pst_Stream);
				}

				return;
			}

			/* read data */
			uiBlock = min
				(
					(p_Stereo->ui_FileSize - (p_Stereo->ui_BufferPosition - p_Stereo->ui_FilePosition)) / 2,
					Cte_MuxOneChannelFrameSize
				);
			uiRead = min(uiSizeByChannel, uiBlock);

			p_Stereo->ui_Block = uiBlock;
			p_Stereo->ui_Read = uiRead;
			p_Stereo->ui_Offset = 0;
			p_Stereo->pc_UserBuffer = _pc_Buffer;
			p_Stereo->ui_UserBufferSize = uiSizeByChannel;
			p_Stereo->pfv_UserCallback = _pfv_Callback;
			p_Stereo->ui_UserContext = (unsigned int) _pst_Stream;

			_pst_Stream->b_PendingDvdAction++;
			Overloaded_GC_i_StreamRead
			(
				_p_FileHandler,
				p_Stereo->pc_StereoFrameBuffer,
				2 * uiBlock,
				p_Stereo->ui_BufferPosition,
				gcSND_StrDvdCallbackStreamRead,
				(unsigned int) _pst_Stream
			);
		}
		else
		{
			if(_pfv_Callback)
			{
				_pst_Stream->b_PendingDvdAction++;
				_pfv_Callback(DVD_RESULT_GOOD, &_p_FileHandler->st_FileInfo, (unsigned int) _pst_Stream);
			}
		}
	}
	else
	{
		/* the fle is mono, we can lauch the dvd access */
		_pst_Stream->b_PendingDvdAction++;
		Overloaded_GC_i_StreamRead(_p_FileHandler, _pc_Buffer, _ui_Size, _ui_Seek, _pfv_Callback, (unsigned int) _pst_Stream);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_StrDvdCallbackStreamRead(s32 result, DVDFileInfo *fileInfo, unsigned int context)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_StereoFile	*p_Stereo;
	gcSND_tdst_Stream		*pst_Stream;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Stream = (gcSND_tdst_Stream *) context;
	pst_Soft = pst_Stream->pst_SoftBuffer;
	p_Stereo = pst_Stream->p_StereoFileCache;

	pst_Stream->b_PendingDvdAction--;

	gcSND_M_Assert(pst_Stream->b_PendingDvdAction == 0);
	if(!p_Stereo) return;

	if(result == DVD_RESULT_GOOD)
	{
		/* copy to user buffer */
		for(i = 0; i < 2; i++)
		{
			L_memcpy
			(
				p_Stereo->pc_UserBuffer + (i * Cte_MuxOneChannelFrameSize),
				p_Stereo->pc_StereoFrameBuffer + p_Stereo->ui_Offset + (i * p_Stereo->ui_Block),
				p_Stereo->ui_Read
			);
		}

		p_Stereo->ui_UserBufferSize -= p_Stereo->ui_Read;
		p_Stereo->pc_UserBuffer += p_Stereo->ui_Read;

		if(p_Stereo->ui_UserBufferSize)
		{
			/* goto next frame */
			p_Stereo->ui_BufferPosition += 2 * Cte_MuxOneChannelFrameSize;

			if(p_Stereo->ui_BufferPosition >= (p_Stereo->ui_FilePosition + p_Stereo->ui_FileSize))
			{
				/* end-of-file : no more data */
				if(p_Stereo->pfv_UserCallback)
				{
					pst_Stream->b_PendingDvdAction++;
					((GC_tdpfv_StreamCallBack) p_Stereo->pfv_UserCallback) (result, fileInfo, context);
				}

				return;
			}

			/* read data */
			p_Stereo->ui_Block = min
				(
					(p_Stereo->ui_FileSize - (p_Stereo->ui_BufferPosition - p_Stereo->ui_FilePosition)) / 2,
					Cte_MuxOneChannelFrameSize
				);
			p_Stereo->ui_Read = min(p_Stereo->ui_UserBufferSize, p_Stereo->ui_Block);
			p_Stereo->ui_Offset = 0;

			pst_Stream->b_PendingDvdAction++;
			Overloaded_GC_i_StreamRead
			(
				pst_Soft->pst_SndFileHandler,
				p_Stereo->pc_StereoFrameBuffer,
				2 * p_Stereo->ui_Block,
				p_Stereo->ui_BufferPosition,
				gcSND_StrDvdCallbackStreamRead,
				(unsigned int) pst_Stream
			);
		}
		else
		{
			if(p_Stereo->pfv_UserCallback)
			{
				pst_Stream->b_PendingDvdAction++;
				((GC_tdpfv_StreamCallBack) p_Stereo->pfv_UserCallback) (result, fileInfo, context);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StreamChain
(
	SND_tdst_SoundBuffer	*_pSB,
	int						_i_Flags,			/* flag */
	int						_i_Loop,			/* loop number */
	unsigned int			_ui_Exit,			/* exit point (oct) */
	unsigned int			_ui_Enter,			/* StartOffset (oct) */
	unsigned int			_ui_DataSize,		/* DataSize (oct) */
	unsigned int			_ui_DataPosition,	/* DataPosition (oct) */
	unsigned int			_ui_LoopBegin,		/* LoopBeginOffset (oct) */
	unsigned int			_ui_LoopEnd			/* LoopEndOffset (oct) */
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	volatile gcSND_tdst_Stream	*pst_Stream;
	int							channel = 1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Stream = gcSND_p_GetStreamFromSoftBuffer(gcSND_M_GetBuffer(_pSB));
	if(!pst_Stream) return;
	if(gcSND_M_IsStereo(_pSB)) channel = 2;

	M_RoundSize(_ui_Exit, channel);
	M_RoundSize(_ui_Enter, channel);
	M_RoundSize(_ui_DataSize, channel);
	M_RoundSize(_ui_LoopBegin, channel);
	M_RoundSize(_ui_LoopEnd, channel);

	if(_i_Loop) _i_Loop--;

	if(_i_Loop == 0)
	{
		_ui_LoopBegin = 0;
		_ui_LoopEnd = _ui_DataSize;
	}

	pst_Stream->st_NewSettings.uc_IsOneChainOrder = 1;

	/* fx */
	pst_Stream->st_NewSettings.uc_SendToFxA = 0;
	pst_Stream->st_NewSettings.uc_SendToFxB = 0;
	if(_i_Flags & SND_Cul_SBC_PlayFxA)
		pst_Stream->st_NewSettings.uc_SendToFxA = 1;
	else if(_i_Flags & SND_Cul_SBC_PlayFxB)
		pst_Stream->st_NewSettings.uc_SendToFxB = 1;

	pst_Stream->st_NewSettings.uc_Stereo = gcSND_M_IsStereo(_pSB);
	pst_Stream->st_NewSettings.i_LoopNb = _i_Loop;

	pst_Stream->st_NewSettings.ui_DataSize = _ui_DataSize >> (channel - 1);
	pst_Stream->st_NewSettings.ui_DataPosition = _ui_DataPosition;
	pst_Stream->st_NewSettings.ui_LoopBeginOffset = _ui_LoopBegin >> (channel - 1);
	pst_Stream->st_NewSettings.ui_LoopEndOffset = _ui_LoopEnd >> (channel - 1);
	pst_Stream->st_NewSettings.ui_StartOffset = _ui_Enter >> (channel - 1);
	pst_Stream->st_NewSettings.ui_ExitOffset = _ui_Exit >> (channel - 1);

	pst_Stream->i_LastCommand = gsSND_Cmd_StreamChain;
	pst_Stream->i_LastArg = _ui_DataPosition + _ui_Enter;

	gcSND_M_GetBuffer(_pSB)->ui_Flags |= SND_Cul_SBS_Playing | (_i_Loop ? SND_Cul_SBC_PlayLooping : 0);
}



/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* _GAMECUBE */

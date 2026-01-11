/*$T ps2SNDstream.c GC 1.138 06/30/04 09:10:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_TARGET

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "Sound/Sources/SNDstruct.h"
#include "Sound/Sources/SNDconst.h"
#include "Sound/Sources/SNDwave.h"
#include "Sound/Sources/SND.h"
#include "Sound/Sources/SNDstream.h"

#include "Sound/Sources/ps2/ps2SND.h"
#include "Sound/Sources/ps2/SPU.h"

#include "SDK/Sources/IOP/RPC_Manager.h"
#include "SDK/Sources/IOP/ee/eeDebug.h"

static int	ps2SND_i_CreateStream(void);

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int ps2SND_i_CreateStream(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ps2SND_tdst_Snd *pS;
	int				id;
	int				tmp[2];
	/*~~~~~~~~~~~~~~~~~~~*/

	id = ps2SND_i_AllocSB();
	if(id > 0)
	{
		pS = &ps2SND_sax_Snd[id];
		pS->ui_Flags = SND_Cul_DSF_Used | SND_Cte_StreamedFile;
		pS->i_Pan = 0;
		pS->f_Volume = 1.0f;
		pS->f_LeftCoeff = 1.0f;
		pS->f_RightCoeff = 1.0f;
		pS->s_LeftVol = 0;
		pS->s_RightVol = 0;
		pS->i_LastCmd = RPC_Cmd_SndStopStream;
		pS->i_LastArg = 0;
		pS->i_Frequency = 0;
		pS->ui_DataPosition = 0;
		pS->ui_DataSize = 0;
		pS->ui_CurrentStatus = 0;
		pS->pst_DuplicateSrc = NULL;
		pS->ui_UserNb = 1;
		pS->pc_SpuBuffer = SPU_pv_StreamAlloc(SND_Cte_MaxBufferSize);

		/*
		 * ## necessaire ?? £
		 * seulement pour la reserve en SPU ??
		 */
		tmp[0] = id;
		tmp[1] = (int) SPU_M_GetAddr(pS->pc_SpuBuffer);
		eeRPC_i_PushCommand(RPC_Cmd_SndCreateBufferForceStream, tmp, 2 * sizeof(int), NULL, 0);
		return id;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_CreateTrackSB(SND_tdst_SoundBuffer **_ppst_Mono, SND_tdst_SoundBuffer **_ppst_Stereo)
{
	/*~~~*/
	int id;
	/*~~~*/

	id = ps2SND_i_CreateStream();

	*_ppst_Mono = (SND_tdst_SoundBuffer *) (ps2SND_Cte_MonoIdMask | id);
	*_ppst_Stereo = (SND_tdst_SoundBuffer *) (ps2SND_Cte_StereoIdMask | id);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_ReleaseTrackSB(SND_tdst_SoundBuffer *_pst_MonoSB, SND_tdst_SoundBuffer *_pst_StereoSB)
{
	ps2SND_SB_Release(_pst_StereoSB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
iopSND_ReinitAndPlayStreamLong_VArgs g_RecordFirstCall;
int g_RecordFirstCallIsUsed = 0;
 
void ps2SND_StreamReinitAndPlay
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
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_ReinitAndPlayStreamLong_VArgs	stVArgs;
	int										idR;
//	float									f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	idR = ps2SND_M_GetIdxFromSB(_pSB);
	eeSND_M_CheckSoftBuffIdxOrReturn(idR, ;);

	if(ps2SND_M_IdIsStereo(_pSB))
	{
		_i_StartPos >>= 1; 
		
		_ui_Size >>= 1;
		_ui_LoopBegin >>= 1;
		_ui_LoopEnd >>= 1;
		
		_i_StartPos = _i_StartPos / (16);
		_i_StartPos = _i_StartPos * (16);
		
		if(_i_StopPos != -1)
		{
    		_i_StopPos >>= 1;  
			_i_StopPos = _i_StopPos / (16);
			_i_StopPos = _i_StopPos * (16);
		}
		
		_ui_LoopBegin = _ui_LoopBegin / (16);
		_ui_LoopBegin = _ui_LoopBegin * (16);
		
		_ui_LoopEnd = _ui_LoopEnd / (16);
		_ui_LoopEnd = _ui_LoopEnd * (16);
		
		_ui_Size = _ui_Size / (16);
		_ui_Size = _ui_Size * (16);

        
	}
	else
	{
		_i_StartPos = _i_StartPos / (16);
		_i_StartPos = _i_StartPos * (16);
		
		if(_i_StopPos != -1)
		{
			_i_StopPos = _i_StopPos / (16);
			_i_StopPos = _i_StopPos * (16);
		}
		
		_ui_LoopBegin = _ui_LoopBegin / (16);
		_ui_LoopBegin = _ui_LoopBegin * (16);
		
		_ui_LoopEnd = _ui_LoopEnd / (16);
		_ui_LoopEnd = _ui_LoopEnd * (16);
		
		_ui_Size = _ui_Size / (16);
		_ui_Size = _ui_Size * (16);
	}

	if
	(
		(ps2SND_sax_Snd[idR].i_LastCmd == RPC_Cmd_SndStreamReinitAndPlayLong)
	&&	(ps2SND_sax_Snd[idR].i_LastArg == _ui_Position)
	) return;

	ps2SND_sax_Snd[idR].i_LastCmd = RPC_Cmd_SndStreamReinitAndPlayLong;
	ps2SND_sax_Snd[idR].i_LastArg = _ui_Position;
	ps2SND_sax_Snd[idR].i_Frequency = _i_Freq;
	ps2SND_sax_Snd[idR].ui_CurrentStatus = SND_Cul_SBS_Playing;
	ps2SND_sax_Snd[idR].ui_DataSize = _ui_Size;
	ps2SND_sax_Snd[idR].ui_Flags &= ~SND_Cul_SF_StreamIsPlayingNow;
	

	if(_i_StartPos < 0) _i_StartPos = 0;
	if((unsigned int) _i_StartPos > _ui_Size) _i_StartPos = 0;

	if(_i_StopPos < 0) _i_StopPos = 0;
	if((unsigned int) _i_StopPos > _ui_Size) _i_StopPos = 0;

	ps2SND_SB_SetVolume(_pSB, _i_Vol);
	ps2SND_SB_SetPan(_pSB, _i_Pan, 0);

	stVArgs.i_ID = (int) _pSB;
	stVArgs.i_Flag = _i_Flag;
	stVArgs.i_LoopNb = _i_LoopNb;
	stVArgs.ui_Frequency = _i_Freq;
	stVArgs.ui_LVol = ps2SND_sax_Snd[idR].s_LeftVol;;
	stVArgs.ui_RVol = ps2SND_sax_Snd[idR].s_RightVol;
	stVArgs.ui_RPosition = _ui_Position;
	stVArgs.ui_RSize = _ui_Size;
	stVArgs.ui_LPosition = _ui_Position + _ui_Size;
	stVArgs.ui_LSize = _ui_Size;

	stVArgs.ui_RStartPos = *(unsigned int *) &_i_StartPos;
	stVArgs.ui_RStopPos = *(unsigned int *) &_i_StopPos;
	stVArgs.ui_LStartPos = *(unsigned int *) &_i_StartPos;
	stVArgs.ui_LStopPos = *(unsigned int *) &_i_StopPos;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_FxL <= -700)
	{
		stVArgs.c_DryMixL = 1;
		stVArgs.c_WetMixL = 0;
	}
	else if(_i_FxL >= -500)
	{
		stVArgs.c_DryMixL = 0;
		stVArgs.c_WetMixL = 1;
	}
	else
	{
		stVArgs.c_DryMixL = 1;
		stVArgs.c_WetMixL = 1;
	}

	if(_i_FxR <= -700)
	{
		stVArgs.c_DryMixR = 1;
		stVArgs.c_WetMixR = 0;
	}
	else if(_i_FxR >= -500)
	{
		stVArgs.c_DryMixR = 0;
		stVArgs.c_WetMixR = 1;
	}
	else
	{
		stVArgs.c_DryMixR = 1;
		stVArgs.c_WetMixR = 1;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	eeDbg_M_Assert((_ui_LoopBegin % 16) == 0, eeDbg_Err_0033);
	eeDbg_M_Assert((_ui_LoopEnd % 16) == 0, eeDbg_Err_0033);

	stVArgs.ui_RLoopStart = _ui_LoopBegin;
	stVArgs.ui_RLoopStop = _ui_LoopEnd;
	stVArgs.ui_LLoopStart = _ui_LoopBegin;
	stVArgs.ui_LLoopStop = _ui_LoopEnd;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*	
	if(!g_RecordFirstCallIsUsed)
	{
		g_RecordFirstCallIsUsed = 1;
		memcpy(&g_RecordFirstCall, &stVArgs, sizeof(iopSND_ReinitAndPlayStreamLong_VArgs));
	}//*/
		
	eeRPC_i_PushCommand
	(
		RPC_Cmd_SndStreamReinitAndPlayLong,
		&stVArgs,
		sizeof(iopSND_ReinitAndPlayStreamLong_VArgs),
		NULL,
		0
	);
	ps2SND_gi_StreamPlayOnce = 1;
	ps2SND_sax_Snd[idR].f_PlayTimeOut = 0.0f;
}

void ps2SND_RecallFirstStream(void)
{
/*
	if(g_RecordFirstCallIsUsed == 1)
	{
		g_RecordFirstCallIsUsed ++;
		eeRPC_i_PushCommand
		(
			RPC_Cmd_SndStreamReinitAndPlayLong,
			&g_RecordFirstCall,
			sizeof(iopSND_ReinitAndPlayStreamLong_VArgs),
			NULL,
			0
		);
		ps2SND_gi_StreamPlayOnce = 1;
	}//*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_StreamChain
(
	SND_tdst_SoundBuffer	*pSB,
	int						iNewFlag,
	int						iLoopNb,
	unsigned int			uiExitPosition,
	unsigned int			uiEnterPosition,
	unsigned int			uiNewSize,
	unsigned int			uiNewPosition,
	unsigned int			uiLoopBegin,
	unsigned int			uiLoopEnd
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RPC_tdst_SndStreamChain stArg;
	int						idR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	stArg.i_ID = (int) pSB;

	eeSND_M_CheckSoftBuffIdxOrReturn(stArg.i_ID, ;);
	idR = ps2SND_M_GetIdxFromSB(stArg.i_ID);

	if(ps2SND_M_IdIsStereo(stArg.i_ID))
	{
		uiExitPosition >>= 1;
		uiEnterPosition >>= 1;
		uiNewSize >>= 1;
		uiLoopBegin >>= 1;
		uiLoopEnd >>= 1;
	}
	
	uiExitPosition >>= 4;
	uiEnterPosition >>= 4;
	uiNewSize >>= 4;
	uiLoopBegin >>= 4;
	uiLoopEnd >>= 4;

	uiExitPosition <<= 4;
	uiEnterPosition <<= 4;
	uiNewSize <<= 4;
	uiLoopBegin <<= 4;
	uiLoopEnd <<= 4;

	if
	(
		(ps2SND_sax_Snd[idR].i_LastCmd == RPC_Cmd_SndStreamReinitAndPlayLong)
	&&	(ps2SND_sax_Snd[idR].i_LastArg == uiEnterPosition)
	) return;

	ps2SND_sax_Snd[idR].i_LastCmd = RPC_Cmd_SndStreamReinitAndPlayLong;
	ps2SND_sax_Snd[idR].i_LastArg = uiEnterPosition;
	ps2SND_sax_Snd[idR].ui_CurrentStatus = SND_Cul_SBS_Playing;
	ps2SND_sax_Snd[idR].ui_DataSize = uiNewSize;
	ps2SND_sax_Snd[idR].ui_Flags &= ~SND_Cul_SF_StreamIsPlayingNow;

	stArg.i_NewFlag = iNewFlag;
	stArg.i_LoopNb = iLoopNb;

	stArg.ui_ExitPosition = uiExitPosition;
	stArg.ui_EnterPosition = uiEnterPosition;

	stArg.ui_NewSize = uiNewSize;
	stArg.ui_NewPosition = uiNewPosition;

	stArg.ui_NewLoopBegin = uiLoopBegin;
	stArg.ui_NewLoopEnd = uiLoopEnd;


	eeRPC_i_PushCommand(RPC_Cmd_SndStreamChain, &stArg, sizeof(RPC_tdst_SndStreamChain), NULL, 0);
	ps2SND_gi_StreamPlayOnce = 1;
	ps2SND_sax_Snd[idR].f_PlayTimeOut = 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_ShutDownStreams(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(SND_gc_NoSound) return;
	if(ps2SND_gi_StreamPlayOnce)
	{
		for(i = 0; i < SND_Cte_MaxSoftBufferNb; i++)
		{
			if((ps2SND_sax_Snd[i].ui_Flags & SND_Cte_StreamedFile) == 0) continue;
			ps2SND_sax_Snd[i].i_LastCmd = RPC_Cmd_SndStopStream;
			ps2SND_sax_Snd[i].i_LastArg = 0;
			ps2SND_sax_Snd[i].ui_CurrentStatus = 0;
			ps2SND_sax_Snd[i].ui_Flags &= ~SND_Cul_SF_StreamIsPlayingNow;
		}

		eeRPC_i_PushCommand(RPC_Cmd_SndStopAllStream, NULL, 0, NULL, 0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_StreamPrefetch(unsigned int _ui_Pos, unsigned int _ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RPC_tdst_VArg_SndStreamPrefetch st;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st.ui_Position = _ui_Pos;
	st.ui_Size = min(SND_Cte_MaxBufferSize / 2, _ui_Size);
	eeRPC_i_PushCommand(RPC_Cmd_SndStreamPrefetch, &st, sizeof(RPC_tdst_VArg_SndStreamPrefetch), NULL, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_StreamPrefetchArray(unsigned int *_pui_Pos, unsigned int *_pui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RPC_tdst_VArg_SndStreamPrefetch st[SND_Cte_StreamPrefetchMax];
	int								i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* TODO checker la size */
	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		if(_pui_Pos[i])
		{
			/* printf("[ee] prefetch %d\n", _pui_Pos[i]); */
			st[i].ui_Position = _pui_Pos[i];
			st[i].ui_Size = min(SND_Cte_MaxBufferSize / 2, _pui_Size[i]);
		}
		else
		{
			st[i].ui_Position = 0;
			st[i].ui_Size = 0;
		}
	}

	eeRPC_i_PushCommand
	(
		RPC_Cmd_SndStreamPrefetchArray,
		&st,
		SND_Cte_StreamPrefetchMax * sizeof(RPC_tdst_VArg_SndStreamPrefetch),
		_pui_Pos,
		SND_Cte_StreamPrefetchMax * sizeof(unsigned int)
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_StreamFlush(unsigned int _ui_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RPC_tdst_VArg_SndStreamFlush	st;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* printf("[ee] flush %d\n", _ui_Pos); */
	st.ui_Position = _ui_Pos;
	eeRPC_i_PushCommand(RPC_Cmd_SndStreamFlush, &st, sizeof(RPC_tdst_VArg_SndStreamFlush), NULL, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_StreamGetPrefetchStatus(unsigned int *auiPos)
{
	eeRPC_i_PushCommand
	(
		RPC_Cmd_SndStreamPrefetchStatus,
		NULL,
		0,
		auiPos,
		SND_Cte_StreamPrefetchMax * sizeof(unsigned int)
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_StreamLoopCountGet(SND_tdst_SoundBuffer *pSB, int *piLoop)
{
	int id;
	id = ps2SND_M_GetIdxFromSB(pSB);
	eeSND_M_CheckSoftBuffIdxOrReturn(id, ;);
	eeRPC_i_PushCommand
	(
		RPC_Cmd_SndStreamLoopCountGet,
		&id,
		sizeof(int),
		piLoop,
		sizeof(int)
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_StreamChainDelayGet(SND_tdst_SoundBuffer *pSB, float *pfDelay)
{
	int id;
	unsigned int uiSize, uiSample;
	
	id = ps2SND_M_GetIdxFromSB(pSB);
	eeSND_M_CheckSoftBuffIdxOrReturn(id, ;);

	eeRPC_i_PushCommand
	(
		RPC_Cmd_SndStreamChainDelayGet,
		&id,
		sizeof(int),
		&uiSize,
		sizeof(float)
	);
	
	uiSample = SND_ui_SizeToSample
	(
		SND_Cte_DefaultWaveFormat,
		(ps2SND_M_IdIsStereo(pSB) ? 2 : 1),
		uiSize
	);

	*pfDelay = fLongToFloat(uiSample) /  fLongToFloat(ps2SND_sax_Snd[id].i_Frequency);	
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* PSX2_TARGET */

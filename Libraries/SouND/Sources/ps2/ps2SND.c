/*$T ps2SND.c GC 1.138 05/27/04 11:55:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_TARGET
#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Headers
 ***********************************************************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <libsd.h>
#include <libdev.h>

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGkey.h"

#include "SNDstruct.h"
#include "SNDconst.h"
#include "SND.h"
#include "SNDwave.h"
#include "SNDtrack.h"
#include "SNDvolume.h"

#include "IOP/ee/eeDebug.h"
#include "IOP/RPC_Manager.h"
#include "IOP/iop/iopMain.h"
#include "IOP/ee/eeIOP_RamManager.h"

#include "ps2/SPU.h"
#include "ps2/ps2SND.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2------------------------------------------------------------------------------------------------------------------*/

#define ps2SND_Cte_Status_Stop			0x00000000
#define ps2SND_Cte_Status_Playing		0x00000001
#define ps2SND_Cte_Status_PlayingLoop	0x00000002
#define ps2SND_Cte_Status_PlayingFx		0x00000003
#define ps2SND_Cte_Status_PlayingFxLoop 0x00000004
#define ps2SND_Cte_Status_Pause			0x00000005

/*$2------------------------------------------------------------------------------------------------------------------*/

#define SND_Cte_DeltaVolMin		0
#define SND_Cte_DeltaFreqMin	0

/*$2------------------------------------------------------------------------------------------------------------------*/

/*$2------------------------------------------------------------------------------------------------------------------*/

/*$4
 ***********************************************************************************************************************
    Private Prototypes
 ***********************************************************************************************************************
 */

int ps2SND_i_GetiVolumeFromfVolume(float _fVolume);
int ps2SND_i_ChangePriority(int id);

/*$4
 ***********************************************************************************************************************
    Extern Variables
 ***********************************************************************************************************************
 */

extern char PSX2_gasz_BigFile[L_MAX_PATH] __attribute__((aligned(64)));
extern int	gi_LowPriority;
extern int	gi_HightPriority;

/*$4
 ***********************************************************************************************************************
    Private Variables
 ***********************************************************************************************************************
 */

int						gi_CurrentPriority;
ps2SND_tdst_Snd			ps2SND_sax_Snd[SND_Cte_MaxSoftBufferNb];
ps2SND_tdst_Snd	        ps2SND_sx_MasterSnd;
BOOL					ps2SND_gb_ReloadingDialogBank = FALSE;
int						ps2SND_gi_Loading;

int						ps2SND_gi_StreamPlayOnce;

/*$off*/
/*$F
int						SNDPanTableL[128] =
{
	0,	0,	-10,	-10,	-10,	-20,	-20,	-20,
	-30,	-30,	-40,	-40,	-40,	-50,	-50,	-50,
	-60,	-60,	-70,	-70,	-70,	-80,	-80,	-90,
	-90,	-100,	-100,	-100,	-110,	-110,	-120,	-120,
	-130,	-130,	-140,	-140,	-140,	-150,	-150,	-160,
	-160,	-170,	-170,	-180,	-180,	-190,	-200,	-200,
	-210,	-210,	-220,	-220,	-230,	-230,	-240,	-250,
	-250,	-260,	-260,	-270,	-280,	-280,	-290,	-300,
	-300,	-310,	-320,	-330,	-330,	-340,	-350,	-360,
	-360,	-370,	-380,	-390,	-400,	-400,	-410,	-420,
	-430,	-440,	-450,	-460,	-470,	-480,	-490,	-500,
	-510,	-520,	-540,	-550,	-560,	-570,	-590,	-600,
	-610,	-630,	-640,	-660,	-670,	-690,	-710,	-720,
	-740,	-760,	-780,	-800,	-830,	-850,	-870,	-900,
	-930,	-960,	-990,	-1020,	-1060,	-1100,	-1150,	-1200,
	-1260,	-1330,	-1400,	-1500,	-1630,	-1800,	-2100,	-9040
};
*/

float ps2SND_gaf_PanTableL[128] =
{
1.0000000000f, 1.0000000000f, 0.9886900783f, 0.9886900783f, 0.9886900783f, 0.9773801565f, 0.9773801565f, 0.9773801565f, //0
0.9660702348f, 0.9660702348f, 0.9551228285f, 0.9551228285f, 0.9551228285f, 0.9442157149f, 0.9442157149f, 0.9442157149f, //8
0.9333086014f, 0.9333086014f, 0.9226734042f, 0.9226734042f, 0.9226734042f, 0.9121547937f, 0.9121547937f, 0.9016361833f, //16
0.9016361833f, 0.8913388252f, 0.8913388252f, 0.8913388252f, 0.8811889887f, 0.8811889887f, 0.8710391521f, 0.8710391521f, //24
0.8610700965f, 0.8610700965f, 0.8512817621f, 0.8512817621f, 0.8512817621f, 0.8414933681f, 0.8414933681f, 0.8318096399f, //32
0.8318096399f, 0.8223699331f, 0.8223699331f, 0.8129301667f, 0.8129301667f, 0.8035576940f, 0.7944541574f, 0.7944541574f, //40
0.7853506207f, 0.7853506207f, 0.7762470245f, 0.7762470245f, 0.7674677968f, 0.7674677968f, 0.7586885095f, 0.7499092817f, //48
0.7499092817f, 0.7414070368f, 0.7414070368f, 0.7329355478f, 0.7244640589f, 0.7244640589f, 0.7162339091f, 0.7080641389f, //56
0.7080641389f, 0.6998943090f, 0.6918991208f, 0.6840203404f, 0.6840203404f, 0.6761415601f, 0.6684009433f, 0.6607983708f, //64
0.6607983708f, 0.6531958580f, 0.6457016468f, 0.6383697987f, 0.6310380101f, 0.6310380101f, 0.6237584352f, 0.6166878343f, //72
0.6096171737f, 0.6025717258f, 0.5957528949f, 0.5889340639f, 0.5821152329f, 0.5755149722f, 0.5689390302f, 0.5623630881f, //80
0.5559715629f, 0.5496261716f, 0.5370935798f, 0.5309741497f, 0.5248547196f, 0.5188442469f, 0.5070413351f, 0.5012226701f, //88
0.4955281913f, 0.4842000008f, 0.4787082672f, 0.4677443504f, 0.4624482095f, 0.4518559575f, 0.4416409135f, 0.4365333915f, //96
0.4266458154f, 0.4169155061f, 0.4074096978f, 0.3981747031f, 0.3846523762f, 0.3758581281f, 0.3673273623f, 0.3548653424f, //104
0.3428179026f, 0.3311854303f, 0.3199400008f, 0.3090786934f, 0.2951553464f, 0.2818489969f, 0.2661133409f, 0.2512203753f, //112
0.2344429344f, 0.2163044661f, 0.1995580792f, 0.1778457463f, 0.1531118304f, 0.1259125024f, 0.0891285539f, 0.0000000000f //120
};

/*$on*/

/*$4
 ***********************************************************************************************************************
    Public Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG ps2SND_l_Init(SND_tdst_TargetSpecificData *_pst_SP)
{
	L_memset(ps2SND_sax_Snd, 0, SND_Cte_MaxSoftBufferNb * sizeof(ps2SND_tdst_Snd));
	L_memset(&ps2SND_sx_MasterSnd, 0, sizeof(ps2SND_tdst_Snd));
	ps2SND_sx_MasterSnd.f_Volume = ps2SND_Cte_MaxVol;
	ps2SND_sx_MasterSnd.s_LeftVol = ps2SND_Cte_MaxVol;
	ps2SND_sx_MasterSnd.s_RightVol = ps2SND_Cte_MaxVol;
	ps2SND_sx_MasterSnd.f_LeftCoeff = 1.0f;
	ps2SND_sx_MasterSnd.f_RightCoeff = 1.0f;
	ps2SND_sx_MasterSnd.i_Pan = 0;

	gi_CurrentPriority = gi_HightPriority;
	ps2SND_gi_Loading = 0;
	ps2SND_gb_ReloadingDialogBank = FALSE;
	ps2SND_gi_StreamPlayOnce = 0;

	SPU_i_InitModule();

	if(eeRPC_i_PushCommand(RPC_Cmd_SndInit, NULL, 0, NULL, 0) < 0)
	{
		eeDbg_M_Err(eeDbg_Err_0004 "-1- Init snd lib");
		return 0;
	}

	if
	(
		eeRPC_i_PushCommand
			(
				RPC_Cmd_SndBigOpen,
				PSX2_gasz_BigFile,
				((L_strlen(PSX2_gasz_BigFile) + 1) + 4) &~3,
				NULL,
				0
			) < 0
	)
	{
		eeDbg_M_Err(eeDbg_Err_0004 "-2- open bigfile");
		return 0;
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_Close(SND_tdst_TargetSpecificData *_pst_SP)
{
	eeRPC_i_PushCommand(RPC_Cmd_SndClose, NULL, 0, NULL, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeSND_StartScheduler(int id)
{
	/*~~~~~~~~~~~~~~~~~*/
	int			data;
	static int	doit = 1;
	/*~~~~~~~~~~~~~~~~~*/

	/* printf("end speed %08X\n", id); */
	data = id;

	if(!SND_gc_NoSound)
	{
		if(ps2SND_gb_ReloadingDialogBank) data = 0xFF800000;
		if(doit) eeRPC_i_PushCommand(RPC_Cmd_SndStartScheduler, &data, sizeof(int), NULL, 0);
		doit = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeSND_StopScheduler(int id)
{
	/*~~~~~*/
	int data;
	/*~~~~~*/

	/* int send; */
	data = id;

	/*
	 * send = 0; £
	 * printf("begin speed %08X\n", id);
	 */
	if(!SND_gc_NoSound)
	{
		/* fade (not currently used) */
		if
		(
			((id & 0xFF000000) != 0xFE000000)
		&&	((id & 0xFF000000) != 0xFD000000)
		&&	((id & 0xFFF00000) != 0xFF400000)
		&&	((id & 0xFFF00000) != 0xFF800000)
		)
		{
			ps2SND_gi_Loading = 70;
		}

		/* defrag IOP RAM Manager */
		if(id == 0xFF800000)
		{
			/*~~~~~~~~~~~~~~~~*/
			int				i;
			ps2SND_tdst_Snd *pS;
			/*~~~~~~~~~~~~~~~~*/

			pS = ps2SND_sax_Snd;
			for(i = 0; i < SND_Cte_MaxSoftBufferNb; i++, pS++)
			{
				if((pS->ui_Flags & SND_Cul_DSF_Used) == 0) continue;
				if(pS->ui_Flags & SND_Cte_StreamedFile) continue;
				if(pS->ui_Flags & SND_Cul_SF_LoadingSound) continue;
				if(pS->pc_SpuBuffer) pS->pc_SpuBuffer = NULL;
			}

			SPU_FreeAll();
		}
	}
}

extern BOOL				Demo_gb_IsActive;
extern unsigned short	Demo_gus_masterVolumeScale;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void ps2SND_SetGlobalVol(LONG _i_Level)
{
	/*~~~~~~~~~~~*/
	int		tmp[2];
	short	L, R;
	float	fmax;
	/*~~~~~~~~~~~*/

	if(Demo_gb_IsActive)
	{
		fmax = (float) ps2SND_Cte_MaxVol * (float) Demo_gus_masterVolumeScale / 10.0f;
	}
	else
	{
		fmax = (float) ps2SND_Cte_MaxVol;
	}

	if(_i_Level < 0)
		ps2SND_sx_MasterSnd.f_Volume = fmax * SND_f_GetVolFromAtt((unsigned int) _i_Level);
	else
		ps2SND_sx_MasterSnd.f_Volume = fmax;

	L = ps2SND_sx_MasterSnd.s_LeftVol;
	R = ps2SND_sx_MasterSnd.s_RightVol;
	ps2SND_sx_MasterSnd.s_LeftVol = (short) iRoundf(ps2SND_sx_MasterSnd.f_Volume * ps2SND_sx_MasterSnd.f_LeftCoeff);
	ps2SND_sx_MasterSnd.s_RightVol = (short) iRoundf(ps2SND_sx_MasterSnd.f_Volume * ps2SND_sx_MasterSnd.f_RightCoeff);

	tmp[0] = ps2SND_sx_MasterSnd.s_LeftVol;
	tmp[1] = ps2SND_sx_MasterSnd.s_RightVol;

	if((L != ps2SND_sx_MasterSnd.s_LeftVol) || (R != ps2SND_sx_MasterSnd.s_RightVol))
		eeRPC_i_PushCommand(RPC_Cmd_SndSetMasterVol, tmp, 2 * sizeof(int), NULL, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG ps2SND_l_GetGlobalVol(void)
{
	return SND_l_GetAttFromVol(ps2SND_sx_MasterSnd.f_Volume * ps2SND_Cte_InvMaxVol);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *ps2SND_pst_SB_Create
(
	SND_tdst_TargetSpecificData *_p,
	SND_tdst_WaveData			*_p_Wave,
	unsigned int				_ui_Position
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundBuffer		*pst_Retcode;
	ps2SND_tdst_Snd				*pS;
	int							idR;
	RPC_tdst_SndCreateBuffer	stCmd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Retcode = NULL;

	/*$2- check file format ------------------------------------------------------------------------------------------*/

	if((int) _p_Wave->pbData == -1) return NULL;
	if(_ui_Position != (unsigned int) _p_Wave->pbData) return NULL;
	if(_p_Wave->st_WaveFmtx.wFormatTag != (u_long16) SND_Cte_DefaultWaveFormat) return NULL;
	if(_p_Wave->st_WaveFmtx.nChannels != 1) return NULL;

	/*$2- right voice ------------------------------------------------------------------------------------------------*/

	idR = ps2SND_i_AllocSB();
	
#ifndef _FINAL_
    if(idR<=0)
    {
        printf("[EE][SND]*** no more sound soft buffer\n");
    }
#endif    
    
	if(idR > 0)
	{
		pS = &ps2SND_sax_Snd[idR];
		pS->ui_Flags = SND_Cul_DSF_Used;
		pS->i_Pan = 0;
		pS->f_Volume = 1.0f;
		pS->f_LeftCoeff = 1.0f;
		pS->f_RightCoeff = 1.0f;
		pS->s_LeftVol = 0;
		pS->s_RightVol = 0;
		pS->i_LastCmd = 0;
		pS->i_LastArg = 0;
		pS->i_Frequency = _p_Wave->st_WaveFmtx.nSamplesPerSec;
		pS->ui_DataPosition = (unsigned int) _ui_Position;
		pS->ui_DataSize = _p_Wave->i_Size;
		pS->ui_CurrentStatus = 0;
		pS->pst_DuplicateSrc = NULL;
		pS->ui_UserNb = 1;
		if(_ui_Position)
		{
			pS->pc_SpuBuffer = SPU_pv_Alloc(pS->ui_DataSize);
		}
		else
		{
			pS->pc_SpuBuffer = SPU_pv_LoadingSoundAlloc(pS->ui_DataSize);
			pS->ui_Flags = SND_Cul_DSF_Used | SND_Cul_SF_LoadingSound;
		}

		pS->pc_IopBuffer = IOP_pv_AllocAlign(pS->ui_DataSize, 64);


		*(int *) &pst_Retcode = idR;

		if(_ui_Position)
		{
			stCmd.ui_ID = (unsigned int) pst_Retcode;
			stCmd.ui_Frequency = pS->i_Frequency;
			stCmd.pc_RSpuBuffer = SPU_M_GetAddr(pS->pc_SpuBuffer);
			stCmd.pc_RIopBuffer = IOP_M_GetAddr(pS->pc_IopBuffer);
			stCmd.ui_RPosition = pS->ui_DataPosition;
			stCmd.ui_RSize = pS->ui_DataSize;

			eeRPC_i_PushCommand(RPC_Cmd_SndCreateBuffer, (char *) &stCmd, sizeof(RPC_tdst_SndCreateBuffer), NULL, 0);
		}
	}

	return pst_Retcode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *ps2SND_SB_Reload
(
	SND_tdst_TargetSpecificData *_p,
	SND_tdst_SoundBuffer		*_pst_SB,
	SND_tdst_WaveData			*_p_Wave,
	unsigned int				_ui_DataPos
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ps2SND_tdst_Snd				*pS;
	int							idR;
	int							_i_Freq;
	unsigned int				_ui_DataSize;
	RPC_tdst_SndCreateBuffer	stCmd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if !defined(_FINAL_)
	if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoWav) return NULL;
#endif
    eeSND_M_CheckSoftBuffIdxOrReturn((int) _pst_SB, NULL);

	/*$2- check file format ------------------------------------------------------------------------------------------*/

	_ui_DataSize = _p_Wave->i_Size;
	_i_Freq = _p_Wave->st_WaveFmtx.nSamplesPerSec;

	if(_p_Wave->st_WaveFmtx.nChannels != 1) return NULL;
	if(!_ui_DataSize) return NULL;
	if(!_ui_DataPos) return NULL;

	/*$2- right voice ------------------------------------------------------------------------------------------------*/

	idR = (int) _pst_SB;
	pS = &ps2SND_sax_Snd[idR];
	pS->ui_Flags = SND_Cul_DSF_Used | SND_Cul_SF_LoadingSound;
	pS->i_Pan = 0;
	pS->f_Volume = 1.0f;
	pS->f_LeftCoeff = 1.0f;
	pS->f_RightCoeff = 1.0f;
	pS->s_LeftVol = 0;
	pS->s_RightVol = 0;
	pS->i_LastCmd = 0;
	pS->i_LastArg = 0;
	pS->ui_CurrentStatus = 0;
	pS->pst_DuplicateSrc = NULL;
	pS->ui_UserNb = 1;
	pS->i_Frequency = _i_Freq;
	pS->ui_DataPosition = _ui_DataPos;
	pS->ui_DataSize = _ui_DataSize;

	stCmd.ui_ID = (unsigned int) _pst_SB;
	stCmd.ui_Frequency = pS->i_Frequency;
	stCmd.pc_RSpuBuffer = SPU_M_GetAddr(pS->pc_SpuBuffer);
	stCmd.pc_RIopBuffer = IOP_M_GetAddr(pS->pc_IopBuffer);
	stCmd.ui_RPosition = pS->ui_DataPosition;
	stCmd.ui_RSize = pS->ui_DataSize;
	eeRPC_i_PushCommand(RPC_Cmd_SndReloadBuffer, (char *) &stCmd, sizeof(RPC_tdst_SndCreateBuffer), NULL, 0);

	return _pst_SB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_SB_Play(SND_tdst_SoundBuffer *_pst_SB, int _i_Flag, int _iFxL, int _iFxR)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				idR, cmd, docmd;
	RPC_tdst_Play	stPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	eeSND_M_CheckSoftBuffIdxOrReturn((int) _pst_SB, -1);

	idR = (int) _pst_SB;

	if(_i_Flag & SND_Cul_SBC_PlayLooping)
	{
		if(_i_Flag & SND_Cul_SBC_PlayFxA)
			cmd = RPC_Cmd_SndPlayLoopFxA;
		else if(_i_Flag & SND_Cul_SBC_PlayFxB)
			cmd = RPC_Cmd_SndPlayLoopFxB;
		else
			cmd = RPC_Cmd_SndPlayLoop;
	}
	else
	{
		if(_i_Flag & SND_Cul_SBC_PlayFxA)
			cmd = RPC_Cmd_SndPlayFxA;
		else if(_i_Flag & SND_Cul_SBC_PlayFxB)
			cmd = RPC_Cmd_SndPlayFxB;
		else
			cmd = RPC_Cmd_SndPlay;
	}

	if(ps2SND_sax_Snd[idR].i_LastCmd == cmd)
		docmd = 0;
	else
		docmd = 1;

	ps2SND_sax_Snd[idR].i_LastCmd = cmd;
	ps2SND_sax_Snd[idR].i_LastArg = 0;
	ps2SND_sax_Snd[idR].ui_CurrentStatus = SND_Cul_SBS_Playing;
	ps2SND_sax_Snd[idR].ui_Flags &= ~SND_Cul_SF_StreamIsPlayingNow;

	ps2SND_sax_Snd[idR].ui_ModifyFlags |= ps2SND_Cte_ModifyFreq;
	ps2SND_sax_Snd[idR].ui_ModifyFlags |= ps2SND_Cte_ModifyVol;

	if(gi_CurrentPriority == gi_LowPriority)
	{
		docmd = 0;
		ps2SND_sax_Snd[idR].ui_CurrentStatus = 0;
	}

	stPlay.i_ID = idR;
	stPlay.i_Flag = _i_Flag;

	if(_iFxL <= -700)
	{
		stPlay.c_DryMixL = 1;
		stPlay.c_WetMixL = 0;
	}
	else if(_iFxL >= -500)
	{
		stPlay.c_DryMixL = 0;
		stPlay.c_WetMixL = 1;
	}
	else
	{
		stPlay.c_DryMixL = 1;
		stPlay.c_WetMixL = 1;
	}
/**/
	if(_iFxR <= -700)
	{
		stPlay.c_DryMixR = 1;
		stPlay.c_WetMixR = 0;
	}
	else if(_iFxR >= -500)
	{
		stPlay.c_DryMixR = 0;
		stPlay.c_WetMixR = 1;
	}
	else
	{
		stPlay.c_DryMixR = 1;
		stPlay.c_WetMixR = 1;
	}

	if(docmd) eeRPC_i_PushCommand(cmd, &stPlay, sizeof(RPC_tdst_Play), NULL, 0);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_Stop(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~*/
	int idR;
	int cmd;
	int docmd;
	/*~~~~~~*/

	idR = ps2SND_M_GetIdxFromSB(_pst_SB);
	eeSND_M_CheckSoftBuffIdxOrReturn(idR, ;);

	cmd = (ps2SND_sax_Snd[idR].ui_Flags & SND_Cte_StreamedFile) ? RPC_Cmd_SndStopStream : RPC_Cmd_SndStop;
	if(ps2SND_sax_Snd[idR].i_LastCmd == cmd)
		docmd = 0;
	else
		docmd = 1;

	ps2SND_sax_Snd[idR].i_LastCmd = cmd;
	ps2SND_sax_Snd[idR].i_LastArg = 0;
	ps2SND_sax_Snd[idR].ui_CurrentStatus = 0;
	ps2SND_sax_Snd[idR].ui_Flags &= ~SND_Cul_SF_StreamIsPlayingNow;

	/* force refresh of volume at next play */
	ps2SND_sax_Snd[idR].s_RightVol = 0;
	ps2SND_sax_Snd[idR].s_LeftVol = 0;

	if(docmd) eeRPC_i_PushCommand(cmd, &_pst_SB, sizeof(int), NULL, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_Release(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~*/
	ps2SND_tdst_Snd *pS;
	int				id1;
	/*~~~~~~~~~~~~~~~~*/

	id1 = ps2SND_M_GetIdxFromSB(_pst_SB);
	eeSND_M_CheckSoftBuffIdxOrReturn(id1, ;);

	ps2SND_SB_Stop(_pst_SB);


	pS = &ps2SND_sax_Snd[id1];
	pS->ui_UserNb--;
	if(pS->pst_DuplicateSrc)
	{
		pS->pst_DuplicateSrc->ui_UserNb--;
	}
	else
	{
		if(pS->ui_UserNb)
		{
			eeDbg_M_ErrX(eeDbg_Err_002D " S%08X", id1);
		}
		else
		{
			if(pS->pc_SpuBuffer) SPU_Free(pS->pc_SpuBuffer);
			if(pS->pc_IopBuffer) IOP_FreeAlign(pS->pc_IopBuffer);
		}
	}

	ps2SND_FreeSB((int) _pst_SB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_Duplicate
(
	SND_tdst_TargetSpecificData *_pst_SpeData,
	SND_tdst_SoundBuffer		*_pst_SrcSB,
	SND_tdst_SoundBuffer		**_ppst_DstSB
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				idR;
	int				odR;
	ps2SND_tdst_Snd *pSi, *pSo;
	int				tmp[2];
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	*_ppst_DstSB = NULL;
	eeSND_M_CheckSoftBuffIdxOrReturn(((int) _pst_SrcSB), ;);
	idR = ((int) _pst_SrcSB);

	odR = ps2SND_i_AllocSB();
	if(odR > 0)
	{
		pSo = &ps2SND_sax_Snd[odR];
		pSi = &ps2SND_sax_Snd[idR];

		pSo->ui_Flags = pSi->ui_Flags;
		pSo->i_Frequency = pSi->i_Frequency;
		pSo->ui_DataPosition = pSi->ui_DataPosition;
		pSo->ui_DataSize = pSi->ui_DataSize;
		pSo->pc_SpuBuffer = pSi->pc_SpuBuffer;
		pSo->pc_IopBuffer = NULL;

		pSo->i_Pan = 0;
		pSo->f_Volume = 1.0f;
		pSo->f_LeftCoeff = 1.0f;
		pSo->f_RightCoeff = 1.0f;
		pSo->s_LeftVol = 0;
		pSo->s_RightVol = 0;
		pSo->i_LastCmd = 0;
		pSo->i_LastArg = 0;
		pSo->ui_CurrentStatus = 0;

		pSo->pst_DuplicateSrc = pSi;
		pSo->ui_UserNb = 1;
		pSi->ui_UserNb++;

		/*$2- left voice ? -------------------------------------------------------------------------------------------*/

		(*_ppst_DstSB) = (SND_tdst_SoundBuffer *) (odR);

		eeDbg_M_Assert(!(ps2SND_sax_Snd[idR].ui_Flags & SND_Cte_StreamedFile), eeDbg_Err_0030);

		tmp[0] = (int) _pst_SrcSB;
		tmp[1] = (int) (*_ppst_DstSB);
		eeRPC_i_PushCommand(RPC_Cmd_SndDuplicateBuffer, (char *) tmp, 2 * sizeof(int), NULL, 0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_GetiVolumeFromfVolume(float _fVolume)
{
	/*~~~~~~~~*/
	int iVolume;
	/*~~~~~~~~*/

	if(_fVolume >= 1.0f) return ps2SND_Cte_MaxVol;
	if(_fVolume <= 0.0f) return 0;

	iVolume = iRoundf(_fVolume * (float) ps2SND_Cte_MaxVol);
	return iVolume;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_SetVolume(SND_tdst_SoundBuffer *_pst_SB, int _i_Level)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		_i_SndId;
	short	s_LastRight, s_LastLeft;
	short	s_PrevRight, s_PrevLeft;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_i_SndId = ps2SND_M_GetIdxFromSB(_pst_SB);
	eeSND_M_CheckSoftBuffIdxOrReturn(_i_SndId, ;);

	if(_i_Level < 0)
	{
		ps2SND_sax_Snd[_i_SndId].f_Volume = (float) ps2SND_Cte_MaxVol * SND_f_GetVolFromAtt((unsigned int) _i_Level);
	}
	else
		ps2SND_sax_Snd[_i_SndId].f_Volume = (float) ps2SND_Cte_MaxVol;

	/* save the phase */
	s_LastRight = (ps2SND_sax_Snd[_i_SndId].s_RightVol < 0) ? -1 : 1;
	s_LastLeft = (ps2SND_sax_Snd[_i_SndId].s_LeftVol < 0) ? -1 : 1;

	/* save prev values */
	s_PrevRight = ps2SND_sax_Snd[_i_SndId].s_RightVol;
	s_PrevLeft = ps2SND_sax_Snd[_i_SndId].s_LeftVol;

	ps2SND_sax_Snd[_i_SndId].s_LeftVol = (short) iRoundf(ps2SND_sax_Snd[_i_SndId].f_Volume * ps2SND_sax_Snd[_i_SndId].f_LeftCoeff);
	ps2SND_sax_Snd[_i_SndId].s_RightVol = (short) iRoundf(ps2SND_sax_Snd[_i_SndId].f_Volume * ps2SND_sax_Snd[_i_SndId].f_RightCoeff);

	/* restore the phase */
	ps2SND_sax_Snd[_i_SndId].s_RightVol *= s_LastRight;
	ps2SND_sax_Snd[_i_SndId].s_LeftVol *= s_LastLeft;

	if
	(
		(lAbs(s_PrevRight - ps2SND_sax_Snd[_i_SndId].s_RightVol) > SND_Cte_DeltaVolMin)
	||	(lAbs(s_PrevLeft - ps2SND_sax_Snd[_i_SndId].s_LeftVol) > SND_Cte_DeltaVolMin)
	)
	{
		ps2SND_sax_Snd[_i_SndId].ui_ModifyFlags |= ps2SND_Cte_ModifyVol;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_SetFrequency(SND_tdst_SoundBuffer *_pst_SB, int _i_Freq)
{
	/*~~~*/
	int iR;
	/*~~~*/

#ifndef _FINAL_
	if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoFreq) return;
#endif

	iR = ps2SND_M_GetIdxFromSB(_pst_SB);
	eeSND_M_CheckSoftBuffIdxOrReturn(iR, ;);


	if(lAbs(ps2SND_sax_Snd[iR].i_Frequency - _i_Freq) >= 100) 
	{
		ps2SND_sax_Snd[iR].ui_ModifyFlags |= ps2SND_Cte_ModifyFreq;
		ps2SND_sax_Snd[iR].i_Frequency = _i_Freq;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_SetPan(SND_tdst_SoundBuffer *_pst_SB, int _i_Pan, int _i_FrontRear)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		_i_SndId;
	short	s_PrevRight, s_PrevLeft;
	int		idx;
	float	f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_i_SndId = ps2SND_M_GetIdxFromSB(_pst_SB) ;
	eeSND_M_CheckSoftBuffIdxOrReturn(_i_SndId, ;);

	ps2SND_sax_Snd[_i_SndId].i_Pan = _i_Pan;

	if(SND_gst_Params.ul_RenderMode == SND_Cte_RenderMono)
	{
		idx = 64;
	}
	else if(_i_Pan > 0)
	{
		f = SND_f_GetVolFromAtt(-_i_Pan);
		idx = (int) (127.0f - (63.0f * f));
	}
	else if(_i_Pan < 0)
	{
		f = SND_f_GetVolFromAtt(_i_Pan);
		idx = (int) (64.0f * f);
	}
	else
	{
		idx = 64;
	}

	if(idx < 0) idx = 0;
	if(idx > 127) idx = 127;

	ps2SND_sax_Snd[_i_SndId].f_LeftCoeff = ps2SND_gaf_PanTableL[idx];
	ps2SND_sax_Snd[_i_SndId].f_RightCoeff = ps2SND_gaf_PanTableL[127 - idx];

	/* save previous */
	s_PrevLeft = ps2SND_sax_Snd[_i_SndId].s_LeftVol;
	s_PrevRight = ps2SND_sax_Snd[_i_SndId].s_RightVol;

	/* update value */
	ps2SND_sax_Snd[_i_SndId].s_LeftVol = (short) iRoundf(ps2SND_sax_Snd[_i_SndId].f_Volume * ps2SND_sax_Snd[_i_SndId].f_LeftCoeff);
	ps2SND_sax_Snd[_i_SndId].s_RightVol = (short) iRoundf(ps2SND_sax_Snd[_i_SndId].f_Volume * ps2SND_sax_Snd[_i_SndId].f_RightCoeff);

	ps2SND_sax_Snd[_i_SndId].s_LeftVol &= 0x7fff;
	ps2SND_sax_Snd[_i_SndId].s_RightVol &= 0x7fff;

	if
	(
		(lAbs(s_PrevRight - ps2SND_sax_Snd[_i_SndId].s_RightVol) > SND_Cte_DeltaVolMin)
	||	(lAbs(s_PrevLeft - ps2SND_sax_Snd[_i_SndId].s_LeftVol) > SND_Cte_DeltaVolMin)
	)
	{
		ps2SND_sax_Snd[_i_SndId].ui_ModifyFlags |= ps2SND_Cte_ModifyVol;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_RefreshStatus(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			ref[SND_Cte_MaxSoftBufferNb];
	int			state[SND_Cte_MaxSoftBufferNb];
	int			nb;
	int			i;
	static int	activ = 0;
	extern float       TIM_gf_dt;
	extern BOOL			SND_gb_PauseAll ;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	if(activ++ == 1)
	{
		activ = 0;
		return;
	}


	nb = 0;
	for(i = 1; i < SND_Cte_MaxSoftBufferNb; i++)
	{
		if(!(ps2SND_sax_Snd[i].ui_Flags & SND_Cul_DSF_Used)) continue;
		if(!(ps2SND_sax_Snd[i].ui_CurrentStatus & SND_Cul_SBS_Playing)) continue;
		ref[1 + nb++] = i;
	}


	ref[0] = nb;
	eeRPC_i_PushCommand(RPC_Cmd_SndGetStatus, ref, (nb + 1) * sizeof(int), state, nb * sizeof(int));


	for(i = 0; i < nb; i++)
	{
		if(ps2SND_sax_Snd[ref[i + 1]].ui_CurrentStatus & SND_Cul_SBS_Playing)
		{
			if(SND_Cul_DSF_StartedOnce & ps2SND_sax_Snd[ref[i + 1]].ui_CurrentStatus)
			{
				if((state[i] & SND_Cul_SBS_Playing) == 0)
				{
					ps2SND_sax_Snd[ref[i + 1]].ui_CurrentStatus = 0;
					ps2SND_sax_Snd[ref[i + 1]].ui_Flags &= ~SND_Cul_SF_StreamIsPlayingNow;
				}

				continue;
			}
			else if(!SND_gb_PauseAll)
			{
    		    if(ps2SND_sax_Snd[ref[i + 1]].ui_Flags & SND_Cte_StreamedFile)
    			{
    			    ps2SND_sax_Snd[ref[i + 1]].f_PlayTimeOut += TIM_gf_dt;
    			    if(ps2SND_sax_Snd[ref[i + 1]].f_PlayTimeOut > 1.5f)
    			    {
    			        ps2SND_sax_Snd[ref[i + 1]].ui_CurrentStatus |= SND_Cul_DSF_StartedOnce;
    			        ps2SND_sax_Snd[ref[i + 1]].ui_Flags |= SND_Cul_SF_StreamIsPlayingNow;
    			    }
    			}
			}


			if(state[i] & SND_Cul_SBS_Playing)
			{
				ps2SND_sax_Snd[ref[i + 1]].ui_CurrentStatus |= SND_Cul_DSF_StartedOnce;

				if(state[i] & SND_Cul_SF_StreamIsPlayingNow)
				{
					ps2SND_sax_Snd[ref[i + 1]].ui_Flags |= SND_Cul_SF_StreamIsPlayingNow;
				}
			}
			
		}
	}

	if(ps2SND_gi_Loading)
	{
		ps2SND_gi_Loading--;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_SB_GetStatus(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Status)
{
	/*~~~~~~~~*/
	int i_SndId;
	/*~~~~~~~~*/

	i_SndId = ps2SND_M_GetIdxFromSB(_pst_SB );
	eeSND_M_CheckSoftBuffIdxOrReturn(i_SndId, 0);
	*_pi_Status = ps2SND_sax_Snd[i_SndId].ui_CurrentStatus & SND_Cul_SBS_Playing;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_GetFrequency(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Freq)
{
	/*~~~~*/
	int ret;
	/*~~~~*/

	ret = ps2SND_M_GetIdxFromSB( _pst_SB);
	eeSND_M_CheckSoftBuffIdxOrReturn(ret, ;);

	*_pi_Freq = ps2SND_sax_Snd[ret].i_Frequency;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_GetPan(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pan, int *_pi_FrontRear)
{
	/*~~~~*/
	int idR;
	/*~~~~*/

	idR = ps2SND_M_GetIdxFromSB(_pst_SB);
	eeSND_M_CheckSoftBuffIdxOrReturn((int) _pst_SB, ;);

	*_pi_Pan = ps2SND_sax_Snd[idR].i_Pan;

	if((ps2SND_sax_Snd[idR].s_LeftVol * ps2SND_sax_Snd[idR].s_RightVol) < 0)
		*_pi_FrontRear = SND_Cte_MinAtt;
	else
		*_pi_FrontRear = -SND_Cte_MinAtt;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_GetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pos, int *_pi_Write)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float						f;
	RPC_tdst_SndStreamSetPos	stArg;
	int							stereo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	stereo = ps2SND_M_IdIsStereo(_pst_SB) ? 1 : 0;

	eeRPC_i_PushCommand(RPC_Cmd_SndStreamGetPos, &_pst_SB, sizeof(int), &stArg, sizeof(RPC_tdst_SndStreamSetPos));

	f = fLongToFloat(stArg.ui_PlayRpos + stArg.ui_PlayLpos);
	f = (f * 28.0f) / 16.0f;	/* 16oct per block, 28 samples in one block */
	*_pi_Pos = lFloatToLong(f) >> stereo;

	f = fLongToFloat(stArg.ui_WriteRpos + stArg.ui_WriteLpos);
	f = (f * 28.0f) / 16.0f;	/* 16oct per block, 28 samples in one block */
	*_pi_Write = lFloatToLong(f) >> stereo;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_Pause(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~*/
	int idR;
	int cmd;
	/*~~~~*/

	idR =ps2SND_M_GetIdxFromSB (_pst_SB);
	eeSND_M_CheckSoftBuffIdxOrReturn(idR, ;);


	cmd = (ps2SND_sax_Snd[idR].ui_Flags & SND_Cte_StreamedFile) ? RPC_Cmd_SndPauseStream : RPC_Cmd_SndPause;
	eeRPC_i_PushCommand(cmd, &_pst_SB, sizeof(int), NULL, 0);

	ps2SND_sax_Snd[idR].i_LastCmd = cmd;
	ps2SND_sax_Snd[idR].i_LastArg = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_FlushModif(void)
{
	/*~~~~~~~~~~*/
	int i;
	int i_voiceID;
	int iR;
	int cmd;
	/*~~~~~~~~~~*/

	for(i = 1; i < SND_Cte_MaxSoftBufferNb; i++)
	{
		if(!(ps2SND_sax_Snd[i].ui_Flags & SND_Cul_DSF_Used)) continue;
		if(!ps2SND_sax_Snd[i].ui_ModifyFlags) continue;

		/* treat stereo */
			i_voiceID = i;
			iR = i;

		/* volume */
		if(ps2SND_sax_Snd[i].ui_ModifyFlags & ps2SND_Cte_ModifyVol)
		{
			/*~~~~~~~*/
			int tmp[3];
			/*~~~~~~~*/

			cmd = (ps2SND_sax_Snd[i].ui_Flags & SND_Cte_StreamedFile) ? RPC_Cmd_SndSetVolStream : RPC_Cmd_SndSetVol;
			tmp[0] = i_voiceID;
			tmp[1] = (int) ps2SND_sax_Snd[iR].s_LeftVol;
			tmp[2] = (int) ps2SND_sax_Snd[iR].s_RightVol;
#ifndef _FINAL_
			if((SND_gst_Params.ul_Flags & SND_Cte_ForceNoRqVol) == 0)
#endif
				eeRPC_i_PushCommand(cmd, tmp, 3 * sizeof(int), NULL, 0);
		}

		/* frequency */
		if(ps2SND_sax_Snd[i].ui_ModifyFlags & ps2SND_Cte_ModifyFreq)
		{
			/*~~~~~~~*/
			int tmp[2];
			/*~~~~~~~*/

			cmd = (ps2SND_sax_Snd[i].ui_Flags & SND_Cte_StreamedFile) ? RPC_Cmd_SndSetFreqStream : RPC_Cmd_SndSetFreq;
			tmp[0] = i_voiceID;
			tmp[1] = ps2SND_sax_Snd[i].i_Frequency;
			eeRPC_i_PushCommand(cmd, tmp, 2 * sizeof(int), NULL, 0);
		}

		/* reset */
		ps2SND_sax_Snd[iR].ui_ModifyFlags = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_SB_SetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int _i_Pos)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_AllocSB(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 1; i < SND_Cte_MaxSoftBufferNb; i++)
	{
		if(!(ps2SND_sax_Snd[i].ui_Flags & SND_Cul_DSF_Used)) return i;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_FreeSB(int _i_ID)
{
	/*~~*/
	int R;
	/*~~*/

	R = ps2SND_M_GetIdxFromSB(_i_ID);
	eeSND_M_CheckSoftBuffIdxOrReturn(_i_ID, ;);
	L_memset(&ps2SND_sax_Snd[R], 0, sizeof(ps2SND_tdst_Snd));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ps2SND_b_IsLoading(void)
{
	if(SND_gc_NoSound) return FALSE;
	return(ps2SND_gi_Loading != 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_ChangeRenderMode(int _i_Mode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	i;
	int		iR, i_voiceID;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_i_Mode)
	{
	case SND_Cte_RenderMono:
		SND_gst_Params.ul_RenderMode = SND_Cte_RenderMono;
		break;

	case SND_Cte_RenderUseConsoleSettings:
	case SND_Cte_RenderStereo:
		SND_gst_Params.ul_RenderMode = SND_Cte_RenderStereo;
		break;

	default:
		return -1;
	}

	eeRPC_i_PushCommand(RPC_Cmd_SndRenderMode, &SND_gst_Params.ul_RenderMode, sizeof(LONG), NULL, 0);
	for(i = 1; i < SND_Cte_MaxSoftBufferNb; i++)
	{
		if(!(ps2SND_sax_Snd[i].ui_Flags & SND_Cul_DSF_Used)) continue;

		/* get SB id */
			i_voiceID = i;
			iR = i;

		ps2SND_SB_SetPan((SND_tdst_SoundBuffer *) i_voiceID, ps2SND_sax_Snd[iR].i_Pan, 0);
		ps2SND_sax_Snd[ps2SND_M_GetIdxFromSB(i_voiceID)].ui_ModifyFlags |= ps2SND_Cte_ModifyVol;
	}

	return 0;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_ChangePriority(int id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static int	last = 0;
	extern BOOL ps2INO_gb_EnableAutoCheck;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(id)
	{
	case -1:
		if(last != -1)
		{
			ps2INO_gb_EnableAutoCheck = TRUE;
			eeRPC_i_PushCommand(RPC_Cmd_SndPriority, &gi_LowPriority, sizeof(int), NULL, 0);
			gi_CurrentPriority = gi_LowPriority;
		}

		last = -1;
		break;

	case -2:
		if(last != -2)
		{
			ps2INO_gb_EnableAutoCheck = FALSE;
			eeRPC_i_PushCommand(RPC_Cmd_SndPriority, &gi_HightPriority, sizeof(int), NULL, 0);
			gi_CurrentPriority = gi_HightPriority;
		}

		last = -2;
		break;

	case -3:
		if(last != -3)
		{
			eeRPC_i_PushCommand(RPC_Cmd_SndUnlockThread, &gi_HightPriority, sizeof(int), NULL, 0);
			gi_CurrentPriority = gi_HightPriority;
		}

		last = -3;
		break;

	case -4:
		if(last != id)
		{
			ps2INO_gb_EnableAutoCheck = FALSE;
			gi_HightPriority |= 0x80000000;
			eeRPC_i_PushCommand(RPC_Cmd_SndPriority, &gi_HightPriority, sizeof(int), NULL, 0);
			gi_HightPriority &= 0x7FFFFFFF;
			gi_CurrentPriority = gi_HightPriority;
		}

		last = id;
	    break;
	    
	case -5:
		if(last != id)
		{
			eeRPC_i_PushCommand(RPC_Cmd_SndPriority, &id, sizeof(int), NULL, 0);
		}

		last = id;
	    break;

	default:
		break;
	}

	return 0;
}

void ps2SND_SetBinSeek(ULONG ulSeek)
{
	eeRPC_i_PushCommand(RPC_Cmd_BeginSpeedMode, &ulSeek, sizeof(int), NULL, 0);	
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* PSX2_TARGET */

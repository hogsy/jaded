/*$T iopSND.c GC! 1.097 05/02/02 23:23:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP

/*$T iopSND.c GC! 1.097 05/02/02 23:23:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- sce ------------------------------------------------------------------------------------------------------------*/

#include <libsd.h>
#include <stdio.h>
#include <sys/file.h>
#include <kernel.h>
#include <string.h>
#include <libcdvd.h>

#define __iopSND_C__

/*$2- SDK ------------------------------------------------------------------------------------------------------------*/

#include "CDV_Manager.h"
#include "RPC_Manager.h"
#include "iop/iopDebug.h"
#include "iop/iopCLI.h"
#include "iop/iopMEM.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "Sound/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/ps2/ps2SND.h"
#include "iop/iopSND.h"
#include "iop/iopSND_Voice.h"
#include "iop/iopSND_libsd.h"
#include "iop/iopSND_Hard.h"
#include "iop/iopSND_Fx.h"
#include "iop/iopSND_Stream.h"
#include "iop/iopSND_DmaScheduler.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

volatile int			iopSND_vi_BigFd;
volatile int			iopSND_vi_BigFd_Snd;
char					asz_BigFile[1024];
volatile int			iopSND_gi_SndThreadVoice;
volatile int			iopSND_gi_SndThreadVoicePrio;
volatile int			iopSND_gi_SeekLock;
volatile int			iopSND_gb_UseBinSeek = FALSE;
volatile unsigned int	iopSND_gui_BinSeek = 0;
int						iopSND_gb_EnableSound = FALSE;
int                     iopSND_gi_RenderMode=SND_Cte_RenderStereo;

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_InitModule(void)
{
	iopDbg_M_Msg(iopDbg_Msg_0004);

	/* libsd init */
	L_sceSdInit(SD_INIT_COLD);
	iopSND_gi_SeekLock = 0;

    if(iopCDV_gi_DiscType == SCECdCD)
    	L_sceSdSetCoreAttr(SD_C_SPDIF_MODE, (SD_SPDIF_MEDIA_CD | SD_SPDIF_OUT_PCM | SD_SPDIF_COPY_NORMAL));
    else if(iopCDV_gi_DiscType == SCECdDVD)
	    L_sceSdSetCoreAttr(SD_C_SPDIF_MODE, (SD_SPDIF_MEDIA_DVD | SD_SPDIF_OUT_PCM | SD_SPDIF_COPY_NORMAL));
    else
	    L_sceSdSetCoreAttr(SD_C_SPDIF_MODE, (SD_SPDIF_MEDIA_CD | SD_SPDIF_OUT_PCM | SD_SPDIF_COPY_NORMAL));

	/* Snd file manager init */
	iopSND_vi_BigFd_Snd = iopSND_vi_BigFd = -1;
	iopSND_gb_UseBinSeek = FALSE;
	iopSND_gui_BinSeek = 0;

	/*
	 * reset SPU memory DON'T MOVE THIS CALL £
	 * a faire passer l'init en back ground car reset trop de tps
	 */
	iopMEM_i_spuResetMemory();

	iopSND_i_InitVoiceModule();
	iopSND_i_InitStreamModule();

	iopSND_gb_EnableSound = TRUE;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_CloseModule(void)
{
	iopDbg_M_Msg(iopDbg_Msg_0017);

	iopSND_i_CloseStreamModule();
	iopSND_i_CloseVoiceModule();
	iopSND_FxCloseModule();
	L_fclose(iopSND_vi_BigFd);
	L_fclose(iopSND_vi_BigFd_Snd);
	iopSND_vi_BigFd_Snd = iopSND_vi_BigFd = -1;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_BigOpen(char *_str_name)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < 1024; i++)
	{
		asz_BigFile[i] = _str_name[i];
		if(_str_name[i] == '\0') break;
	}

	asz_BigFile[i] = '\0';

	iopSND_vi_BigFd = L_fopen(asz_BigFile);
	iopSND_vi_BigFd_Snd = L_fopen(asz_BigFile);

#ifdef PSX2_USE_iopCDV
	if(iopSND_vi_BigFd >= 0)
	{
		iopCDV_i_DirectSetSize(iopSND_vi_BigFd, (SND_Cte_MaxBufferSize / 2) + (2 * CDV_Cte_SectorSize));
	}

#endif
	iopDbg_M_Msg(iopDbg_Msg_0016);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int KKui_seek=0;
unsigned int KKui_globalseek=0;
int iopSND_i_LoadData(unsigned int *_pui_Pos, unsigned int _ui_Size, char **_ppc_Buff, int i_LoadNow)
{
	*_ppc_Buff = NULL;
	if(iopSND_vi_BigFd_Snd < 0) return -1;
	if(!i_LoadNow) return 0;

	if(iopSND_gb_UseBinSeek)
	{
		if(iopSND_gui_BinSeek)
		{
		    KKui_seek = iopSND_gui_BinSeek;
		    KKui_globalseek = iopSND_gui_BinSeek;
			iopSND_gui_BinSeek = 0;
			iopSND_TransferingVoice[0] = iopSND_TransferingVoice[1] = -1;
			KKui_seek += _ui_Size;
		}
		else if(KKui_seek)
		{
		    KKui_globalseek = KKui_seek;
		    KKui_seek += _ui_Size;
		}
	}
	else
	{
	    KKui_globalseek = *_pui_Pos;
	}

	
	*_pui_Pos = KKui_globalseek;
	KKui_globalseek += _ui_Size;
	iopSND_gi_SeekLock = 0;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_SetMasterVol(int _i_Left, int _i_Right)
{
	_i_Left &= 0x3FFF;
	_i_Right &= 0x3FFF;

	L_sceSdSetParam(0 | SD_P_MVOLL, (unsigned short) (_i_Left));
	L_sceSdSetParam(0 | SD_P_MVOLR, (unsigned short) (_i_Right));
	L_sceSdSetParam(1 | SD_P_MVOLL, (unsigned short) (_i_Left));
	L_sceSdSetParam(1 | SD_P_MVOLR, (unsigned short) (_i_Right));
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_GetMasterVol(int *_pi_Left, int *_pi_Right)
{
	*_pi_Right = 0xFFFF & L_sceSdGetParam(0 | SD_P_MVOLXR);
	*_pi_Left = 0xFFFF & L_sceSdGetParam(0 | SD_P_MVOLXR);
	return 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif

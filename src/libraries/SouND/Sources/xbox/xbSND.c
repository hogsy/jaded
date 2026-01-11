/*$T xbSND.c GC 1.138 04/28/03 14:36:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#if !defined( _XBOX )
#  error This file is XBox only
#endif

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/XBox/XBCompositeFile.h"

#include "BIGfiles/BIGio.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/ENGvars.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDloadingsound.h"

#include "SouND/Sources/xbox/xbSND_RAM.h"
#include "SouND/Sources/xbox/xbSND_Stream.h"
#include "SouND/Sources/xbox/xbSND_Thread.h"
#include "SouND/Sources/xbox/xbSND_Debug.h"

#include <dsound.h>
#include <xtl.h>

#include "Gx8/Gx8FileError.h"

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

extern void BIG_ReadNoSeek(ULONG, void *, ULONG);

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

/*$2- debug ----------------------------------------------------------------------------------------------------------*/

#ifdef _DEBUG
extern DWORD	g_dwDirectSoundDebugBreakLevel;
extern DWORD	g_dwDirectSoundDebugLevel;
DWORD			xbSND_gi_LastErrorCode;
#endif

/*$2- bin mode -------------------------------------------------------------------------------------------------------*/

extern BOOL		LOA_gb_SpeedMode;

/*$2- global settings ------------------------------------------------------------------------------------------------*/

LONG	xbSND_gl_GlobalVol;
extern CRITICAL_SECTION		xbSND_gx_InitSettingsSection;

/*$2- async read/event -----------------------------------------------------------------------------------------------*/

SND_tdst_SoundBuffer * volatile xbSND_gp_ReloadingBuffer;
volatile unsigned int	xbSND_gui_ReloadPosition;
volatile unsigned int	xbSND_gui_ReloadSize;
volatile unsigned int	xbSND_gui_ReloadResult;

/*$off*/
int xbSND_ai_RearLeftPan[128] =
{
	0,		0,		-10,	-10,	-10,	-20,	-20,	-20,	-30,	-30,
	-40,	-40,	-40,	-50,	-50,	-50,	-60,	-60,	-70,	-70,
	-70,	-80,	-80,	-90,	-90,	-100,	-100,	-100,	-110,	-110,
	-120,	-120,	-130,	-130,	-140,	-140,	-140,	-150,	-150,	-160,
	-160,	-170,	-170,	-180,	-180,	-190,	-200,	-200,	-210,	-210,
	-220,	-220,	-230,	-230,	-240,	-250,	-250,	-260,	-260,	-270,
	-280,	-280,	-290,	-300,	-300,	-310,	-320,	-330,	-330,	-340,
	-350,	-360,	-360,	-370,	-380,	-390,	-400,	-400,	-410,	-420,
	-430,	-440,	-450,	-460,	-470,	-480,	-490,	-500,	-510,	-520,
	-540,	-550,	-560,	-570,	-590,	-600,	-610,	-630,	-640,	-660,
	-670,	-690,	-710,	-720,	-740,	-760,	-780,	-800,	-830,	-850,
	-870,	-900,	-930,	-960,	-990,	-1020,	-1060,	-1100,	-1150,	-1200,
	-1260,	-1330,	-1400,	-1500,	-1630,	-1800,	-2100,	-9040
};

int xbSND_ai_RearSurroundPan[128] =
{
	0,		0,		-10,	-10,	-10,	-20,	-20,	-20,	-30,	-30,
	-40,	-40,	-40,	-50,	-50,	-50,	-60,	-60,	-70,	-70,
	-70,	-80,	-80,	-90,	-90,	-100,	-100,	-100,	-110,	-110,
	-120,	-120,	-130,	-130,	-140,	-140,	-140,	-150,	-150,	-160,
	-160,	-170,	-170,	-180,	-180,	-190,	-200,	-200,	-210,	-210,
	-220,	-220,	-230,	-230,	-240,	-250,	-250,	-260,	-260,	-270,
	-280,	-280,	-290,	-300,	-300,	-310,	-320,	-330,	-330,	-340,
	-350,	-360,	-360,	-370,	-380,	-390,	-400,	-400,	-410,	-420,
	-430,	-440,	-450,	-460,	-470,	-480,	-490,	-500,	-510,	-520,
	-540,	-550,	-560,	-570,	-590,	-600,	-610,	-630,	-640,	-660,
	-670,	-690,	-710,	-720,	-740,	-760,	-780,	-800,	-830,	-850,
	-870,	-900,	-930,	-960,	-990,	-1020,	-1060,	-1100,	-1150,	-1200,
	-1260,	-1330,	-1400,	-1500,	-1630,	-1800,	-2100,	-9040
};

int xbSND_ai_FrontLeftPan[128] =
{
	0,		-10,	-10,	-20,	-30,	-40,	-40,	-50,	-60,	-70,
	-80,	-80,	-90,	-100,	-110,	-120,	-130,	-140,	-150,	-160,
	-170,	-180,	-190,	-200,	-210,	-220,	-230,	-240,	-260,	-270,
	-280,	-290,	-310,	-320,	-340,	-350,	-370,	-380,	-400,	-420,
	-440,	-460,	-480,	-500,	-520,	-540,	-570,	-600,	-620,	-650,
	-690,	-720,	-760,	-800,	-850,	-900,	-950,	-1020,	-1100,	-1200,
	-1320,	-1500,	-1800,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040
};
int xbSND_ai_FrontCenterPan[128] =
{
	-9040,	-1800,	-1500,	-1320,	-1200,	-1100,	-1020,	-950,	-900,	-850,
	-800,	-760,	-720,	-690,	-650,	-620,	-600,	-570,	-540,	-520,
	-500,	-480,	-460,	-440,	-420,	-400,	-380,	-370,	-350,	-340,
	-320,	-310,	-290,	-280,	-270,	-260,	-240,	-230,	-220,	-210,
	-200,	-190,	-180,	-170,	-160,	-150,	-140,	-130,	-120,	-110,
	-100,	-90,	-80,	-80,	-70,	-60,	-50,	-40,	-40,	-30,
	-20,	-10,	-10,	0,		0,		-10,	-10,	-20,	-30,	-40,
	-40,	-50,	-60,	-70,	-80,	-80,	-90,	-100,	-110,	-120,
	-130,	-140,	-150,	-160,	-170,	-180,	-190,	-200,	-210,	-220,
	-230,	-240,	-260,	-270,	-280,	-290,	-310,	-320,	-340,	-350,
	-370,	-380,	-400,	-420,	-440,	-460,	-480,	-500,	-520,	-540,
	-570,	-600,	-620,	-650,	-690,	-720,	-760,	-800,	-850,	-900,
	-950,	-1020,	-1100,	-1200,	-1320,	-1500,	-1800,	-9040
};
int xbSND_ai_FrontRightPan[128] =
{
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,	-9040,
	-9040,	-9040,	-9040,	-9040,	-9040,	-1800,	-1500,	-1320,	-1200,	-1100,
	-1020,	-950,	-900,	-850,	-800,	-760,	-720,	-690,	-650,	-620,
	-600,	-570,	-540,	-520,	-500,	-480,	-460,	-440,	-420,	-400,
	-380,	-370,	-350,	-340,	-320,	-310,	-290,	-280,	-270,	-260,
	-240,	-230,	-220,	-210,	-200,	-190,	-180,	-170,	-160,	-150,
	-140,	-130,	-120,	-110,	-100,	-90,	-80,	-80,	-70,	-60,
	-50,	-40,	-40,	-30,	-20,	-10,	-10,	0
};

/*$on*/
static unsigned int		xbSND_ui_FreqToHardwareCoeff(float fFreq);

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_l_Init(SND_tdst_TargetSpecificData *_pst_SpecificD)
{
	/*~~*/
	int i;
	HRESULT hr;
	DWORD	wBits;
	/*~~*/

#ifdef _DEBUG

	/*$2- only for debugging -----------------------------------------------------------------------------------------*/

	g_dwDirectSoundDebugBreakLevel = 3;
	g_dwDirectSoundDebugLevel = 3;
	xbSND_gi_LastErrorCode = ERROR_SUCCESS;
#endif

	/*$2- reset all globals ------------------------------------------------------------------------------------------*/

	L_memset(_pst_SpecificD, 0, sizeof(SND_tdst_TargetSpecificData));
	L_memset(xbSND_gah_Event, -1, xbSND_e_EventNb * sizeof(HANDLE));
	xbSND_gh_ThreadHandler = INVALID_HANDLE_VALUE;
	xbSND_gl_GlobalVol = 0;
	xbSND_gh_BigfileHandler = INVALID_HANDLE_VALUE;
	xbSND_gp_ReloadingBuffer = NULL;

	/*$2- sub module init --------------------------------------------------------------------------------------------*/

	if(xbSND_i_InitRAMModule()) return 0;
	if(xbSND_i_InitStreamModule()) return 0;

	/*$2- init of DirectSound ----------------------------------------------------------------------------------------*/

	if((hr = DirectSoundCreate(NULL, &_pst_SpecificD->pst_DS, NULL)) != DS_OK)
	{
		xbSND_M_Assert(hr==DS_OK);
		return 0;
	}

	IDirectSound_EnableHeadphones(_pst_SpecificD->pst_DS, (SND_gst_Params.ul_RenderMode == SND_Cte_RenderHeadPhone));

	/*$2- getting one bigfile handler for async read -----------------------------------------------------------------*/

	xbSND_gh_BigfileHandler = XBCompositeFile_Open
		(
			MAI_gst_InitStruct.asz_ProjectName,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,	/* | FILE_FLAG_NO_BUFFERING, */
            Gx8_FileErrorSound
		);
    xbSND_M_Assert( xbSND_gh_BigfileHandler != NULL );

	/*$2- create all event handler we need ---------------------------------------------------------------------------*/

	for(i = 0; i < xbSND_e_EventNb; i++)
	{
		xbSND_gah_Event[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		if(!xbSND_gah_Event[i])
		{
			xbSND_M_GetLastError();
			xbSND_M_Assert(0);
			return 0;
		}
	}


	/*$2- create the sound thread ------------------------------------------------------------------------------------*/

	xbSND_gh_ThreadHandler = CreateThread(NULL, 500 * 1024, xbSND_Thread, NULL, 0, NULL);
	if(!xbSND_gh_ThreadHandler)
	{
		xbSND_M_GetLastError();
		xbSND_M_Assert(0);
		return 0;
	}

	/*$2- init of global volume --------------------------------------------------------------------------------------*/

	//xbSND_SetGlobalVol(xbSND_gl_GlobalVol);

	wBits = 1;
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFrontLeft, wBits);	
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFrontRight, wBits);
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendBackLeft, wBits);
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendBackRight, wBits);	
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendCenterLeft, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendCenterRight, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxReverbALeft, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxReverbARight, wBits);	
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxEchoALeft, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxEchoARight, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxReverbBLeft, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxReverbBRight, wBits);	
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxEchoBLeft, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxEchoBRight, wBits);		

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_Close(SND_tdst_TargetSpecificData *_pst_SD)
{
	/*~~~~~~~~~~~~~~~*/
	DWORD	dwExitCode;
	BOOL	b_Succeed;
	int		i;
	/*~~~~~~~~~~~~~~~*/

	/*$2- destroy the thread -----------------------------------------------------------------------------------------*/

	if(xbSND_gh_ThreadHandler)
	{
		SetEvent(xbSND_gah_Event[xbSND_e_EventExitThread]);
		do
		{
			b_Succeed = GetExitCodeThread(xbSND_gh_ThreadHandler, &dwExitCode);
		} while((dwExitCode == STILL_ACTIVE) || !b_Succeed);
	}

	/*$2- release all events -----------------------------------------------------------------------------------------*/

	for(i = 0; i < xbSND_e_EventNb; i++)
	{
		if(xbSND_gah_Event[i]) CloseHandle(xbSND_gah_Event[i]);
	}

	if(xbSND_gh_BigfileHandler != NULL)
    {
        XBCompositeFile_Close(xbSND_gh_BigfileHandler);
	    xbSND_gh_BigfileHandler = NULL;
    }

	/*$2- release DirectSound module ---------------------------------------------------------------------------------*/

	IDirectSound8_Release(_pst_SD->pst_DS);

	/*$2- close sub module -------------------------------------------------------------------------------------------*/

	xbSND_FxClose();
	xbSND_CloseStreamModule();
	xbSND_CloseRAMModule();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *xbSND_pst_SB_Create
(
	SND_tdst_TargetSpecificData *_pst_SpecificD,
	SND_tdst_WaveData			*_pst_WI,
	unsigned int				_ui_Pos
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DSBUFFERDESC			dsbd;
	SND_tdst_SoundBuffer	*pst_SB;
	HRESULT					hr;
	char					*pc_Data;
	XBOXADPCMWAVEFORMAT		st_XBOXADPCMWAVEFORMAT;
	IDirectSoundBuffer		*pDirectSoundBuffer;
	DSMIXBINS				dsMixBins;
	DSENVELOPEDESC			stDSENVELOPEDESC;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Data = NULL;

	/*$2- check input ------------------------------------------------------------------------------------------------*/

	if(!_pst_SpecificD->pst_DS) return NULL;
	xbSND_M_Assert(_pst_WI->st_WaveFmtx.wFormatTag == SND_Cte_DefaultWaveFormat)

	/*$2- read data --------------------------------------------------------------------------------------------------*/

	if(_ui_Pos)
	{
		/* this is a WAV */
		pc_Data = xbSND_Alloc(_pst_WI->i_Size);
		if(!pc_Data) return NULL;
		L_memset(pc_Data, 0, _pst_WI->i_Size);

		if(LOA_gb_SpeedMode)
		{
			/*~~~~~~~~~~~~*/
			CHAR	*pc_Buf;
			/*~~~~~~~~~~~~*/

			pc_Buf = LOA_FetchBuffer(_pst_WI->i_Size);
			LOA_ReadCharArray(&pc_Buf, pc_Data, _pst_WI->i_Size);
		}
		else
		{
			BIG_ReadNoSeek(_ui_Pos, pc_Data, _pst_WI->i_Size);
		}
	}
	else
	{
		/* This is a WAC */
		pc_Data = XPhysicalAlloc(_pst_WI->i_Size, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE);
		if(!pc_Data) return NULL;
		L_memset(pc_Data, 0, _pst_WI->i_Size);
	}

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

	//HERE DSOUNDBUFFER creation

	/*$2- init sound buffer ------------------------------------------------------------------------------------------*/

	pst_SB = MEM_p_Alloc(sizeof(SND_tdst_SoundBuffer));
	L_memset(pst_SB, 0, sizeof(SND_tdst_SoundBuffer));

	pst_SB->st_XBOXADPCMWAVEFORMAT = st_XBOXADPCMWAVEFORMAT;

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

    dsbd.dwFlags = DSBCAPS_LOCDEFER; //No hardware lock on creation of buffer

	hr = DirectSoundCreateBuffer(&dsbd, &pDirectSoundBuffer);
	if(hr != DS_OK)
	{
		if(pc_Data) xbSND_Free(pc_Data);
		return NULL;
	}

	L_memset(&stDSENVELOPEDESC, 0, sizeof(stDSENVELOPEDESC));
	stDSENVELOPEDESC.dwEG = DSEG_AMPLITUDE;
	stDSENVELOPEDESC.dwMode = DSEG_MODE_DISABLE;
	hr = IDirectSoundBuffer_SetEG(pDirectSoundBuffer, &stDSENVELOPEDESC);
	xbSND_M_Assert(hr == DS_OK);





	hr = IDirectSoundBuffer_SetMixBins(pDirectSoundBuffer, &dsMixBins);
	xbSND_M_Assert(hr == DS_OK);

	pst_SB->pst_DSB = pDirectSoundBuffer;
	pst_SB->pv_RamBuffer = (void *) pc_Data;
	pst_SB->ui_BufferSize = _pst_WI->i_Size;
	pst_SB->pst_SS = NULL;

	pst_SB->i_CurFrequency = -1;
	pst_SB->i_CurPan = -1;
	pst_SB->i_CurSPan = -1;
	pst_SB->i_CurVol = -1;

	/*$2- fill in the DirectSound buffer -----------------------------------------------------------------------------*/

	if(pc_Data)
	{
		hr = IDirectSoundBuffer_SetBufferData(pst_SB->pst_DSB, pst_SB->pv_RamBuffer, pst_SB->ui_BufferSize);
		xbSND_M_Assert(hr == DS_OK);
	}

	xbSND_SetFilter(pst_SB);

	return pst_SB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *xbSND_SB_ReLoad
(
	SND_tdst_TargetSpecificData *_pst_TargetSpeData,
	SND_tdst_SoundBuffer		*_pst_SB,
	SND_tdst_WaveData			*_pst_WaveData,
	unsigned int				_ui_DataPos
)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	if(!_pst_SB) return NULL;

	/*$2- load new data ----------------------------------------------------------------------------------------------*/

	/* reset the previous contents */
	L_memset(_pst_SB->pv_RamBuffer, 0, _pst_SB->ui_BufferSize);

	if(_ui_DataPos)
	{
		/* surround the size */
		_pst_SB->ui_BufferSize = _pst_WaveData->i_Size;

		/* read data */
		L_memset(_pst_SB->pv_RamBuffer, 0, SND_Cte_LoadingSoundMaxSize);

		xbSND_gui_ReloadPosition = _ui_DataPos;
		xbSND_gui_ReloadSize = _pst_WaveData->i_Size;
		xbSND_gp_ReloadingBuffer = _pst_SB;

		xbSND_M_BeginTraceBlock
		(
			(
				asz_Log, "Reload size %d, pos %d, total size %d", _pst_WaveData->i_Size, xbSND_gui_ReloadPosition,
					SND_Cte_LoadingSoundMaxSize
			)
		);
		xbSND_M_EndTraceBlock();

		SetEvent(xbSND_gah_Event[xbSND_e_EventDoReload]);
	}

	/*$2- reset the SB settings --------------------------------------------------------------------------------------*/

	_pst_SB->st_XBOXADPCMWAVEFORMAT.wfx = _pst_WaveData->st_WaveFmtx;
	_pst_SB->st_XBOXADPCMWAVEFORMAT.wfx.nBlockAlign = 36 * _pst_SB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels;
	_pst_SB->st_XBOXADPCMWAVEFORMAT.wfx.nAvgBytesPerSec =
		(
			_pst_SB->st_XBOXADPCMWAVEFORMAT.wfx.nSamplesPerSec *
			_pst_SB->st_XBOXADPCMWAVEFORMAT.wfx.nBlockAlign
		) >>
		6;
	_pst_SB->st_XBOXADPCMWAVEFORMAT.wfx.wBitsPerSample = 4;
	_pst_SB->st_XBOXADPCMWAVEFORMAT.wfx.cbSize = 2;
	_pst_SB->st_XBOXADPCMWAVEFORMAT.wSamplesPerBlock = 64;

	_pst_SB->i_CurFrequency = -1;

	_pst_SB->i_CurPan = -1;
	_pst_SB->i_CurSPan = -1;
	_pst_SB->i_CurVol = -1;

	hr = IDirectSoundBuffer_SetBufferData(_pst_SB->pst_DSB, _pst_SB->pv_RamBuffer, _pst_SB->ui_BufferSize);
	xbSND_M_Assert(hr == DS_OK);
	return _pst_SB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_EventDoReload(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	OVERLAPPED	stOVERLAPPED;
	BOOL		b_Result;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(xbSND_gp_ReloadingBuffer && xbSND_gui_ReloadPosition && xbSND_gui_ReloadSize)
	{
		L_memset(&stOVERLAPPED, 0, sizeof(OVERLAPPED));
		stOVERLAPPED.hEvent = xbSND_gah_Event[xbSND_e_EventReloadDone];
		stOVERLAPPED.Offset = xbSND_gui_ReloadPosition;

		L_memset(xbSND_gp_ReloadingBuffer->pv_RamBuffer, 0, SND_Cte_LoadingSoundMaxSize);
		xbSND_gui_ReloadResult = 0;

		b_Result = XBCompositeFile_Read
			(
				xbSND_gh_BigfileHandler,
				xbSND_gp_ReloadingBuffer->pv_RamBuffer,
				xbSND_gui_ReloadSize,
				(LPDWORD) & xbSND_gui_ReloadResult,
				&stOVERLAPPED
			);
		xbSND_M_Assert((!b_Result) || (xbSND_gui_ReloadSize == xbSND_gui_ReloadResult));
		if(!b_Result)
		{
			xbSND_M_Assert(GetLastError() == ERROR_IO_PENDING);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_EventReloadDone(void)
{
	if(xbSND_gp_ReloadingBuffer)
	{
		IDirectSoundBuffer_SetBufferData
		(
			xbSND_gp_ReloadingBuffer->pst_DSB,
			xbSND_gp_ReloadingBuffer->pv_RamBuffer,
			xbSND_gp_ReloadingBuffer->ui_BufferSize
		);
	}

	xbSND_gp_ReloadingBuffer = NULL;
	xbSND_gui_ReloadPosition = 0;
	xbSND_gui_ReloadSize = 0;
	xbSND_gui_ReloadResult = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_SB_Play(SND_tdst_SoundBuffer *_pst_SB, int _i_Flag, int _i_FxVolLeft, int _i_FxVolRight)
{
	/*~~~~~~~~~~~~~~~~~~*/
	DWORD	dwPlayLooping;
	HRESULT hr;
    float fFxL, fFxR, fFx;
	/*~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SB) return -1;
 

    fFxL = SND_f_GetVolFromAtt(_i_FxVolLeft);
	fFxR = SND_f_GetVolFromAtt(_i_FxVolRight);
	fFx = 0.5f*(fFxL + fFxR);
 
    if(fFx < 0.5f)
	{
		// 0% wet + 100% dry
        _pst_SB->i_CurFxVol = -10000;
		_pst_SB->i_CurNoFxVol = 0;
	}
	else if(fFx < 0.7f)
	{
		// 100% wet + 100% dry
        _pst_SB->i_CurFxVol = 0;
		_pst_SB->i_CurNoFxVol = 0; 
	}
	else
	{
		// 100% wet + 0% dry
        _pst_SB->i_CurFxVol = 0;
		_pst_SB->i_CurNoFxVol = -10000;
	}

    if(_i_Flag & SND_Cul_SBC_PlayFxA) 
        xbSND_i_FxAddSB(SND_Cte_FxCoreA, _pst_SB);
    else if(_i_Flag & SND_Cul_SBC_PlayFxB) 
        xbSND_i_FxAddSB(SND_Cte_FxCoreB, _pst_SB);
    else 
        xbSND_i_FxDelSB(_pst_SB);
    
	dwPlayLooping = (_i_Flag & SND_Cul_SBC_PlayLooping) ? DSBPLAY_LOOPING : DSBPLAY_FROMSTART;
	hr = IDirectSoundBuffer8_Play(_pst_SB->pst_DSB, 0, 0, dwPlayLooping);
	xbSND_M_Assert(hr == DS_OK);
	return(hr == DS_OK ? 0 : -1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_Pause(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~*/
	int		i_status;
	HRESULT hr;
	/*~~~~~~~~~~~~~*/

	if(!_pst_SB) return;
	if(!_pst_SB->pst_DSB) return;

	hr = IDirectSoundBuffer8_GetStatus(_pst_SB->pst_DSB, &i_status);
	xbSND_M_Assert(hr == DS_OK);
	i_status = (i_status & SND_Cul_SBS_Playing) ? DSBPAUSE_PAUSE : DSBPAUSE_RESUME;
	hr = IDirectSoundBuffer_Pause(_pst_SB->pst_DSB, i_status);
	xbSND_M_Assert(hr == DS_OK);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_Stop(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	if(!_pst_SB) return;
	if(!_pst_SB->pst_DSB) return;

    if(_pst_SB->pst_SS) 
        xbSND_StreamStop(_pst_SB);
    else 
    {
	    xbSND_i_FxDelSB(_pst_SB);
	    hr = IDirectSoundBuffer8_Stop(_pst_SB->pst_DSB);
	    xbSND_M_Assert(hr == DS_OK);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_Release(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~*/
	DWORD	dwStatus;
	HRESULT hr;
	/*~~~~~~~~~~~~~*/

	if(_pst_SB == NULL) return;
	if(!_pst_SB->pst_DSB) return;

    xbSND_i_FxDelSB(_pst_SB);

	hr = IDirectSoundBuffer8_Stop(_pst_SB->pst_DSB);
	xbSND_M_Assert(hr == DS_OK);
	do
	{
		hr = IDirectSoundBuffer8_GetStatus(_pst_SB->pst_DSB, &dwStatus);
		xbSND_M_Assert(hr == DS_OK);
	} while(dwStatus & DSBSTATUS_PLAYING);
	hr = IDirectSoundBuffer8_Release(_pst_SB->pst_DSB);
	xbSND_M_Assert(hr == DS_OK);

	if(_pst_SB->pv_RamBuffer) xbSND_Free(_pst_SB->pv_RamBuffer);

	MEM_Free(_pst_SB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_Duplicate
(
	SND_tdst_TargetSpecificData *_pst_SpeData,
	SND_tdst_SoundBuffer		*_pst_SrcSB,
	SND_tdst_SoundBuffer		**_ppst_DstSB
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DSBUFFERDESC			dsbd;
	SND_tdst_SoundBuffer	*pst_SB;
	HRESULT					hr;
	XBOXADPCMWAVEFORMAT		st_XBOXADPCMWAVEFORMAT;
	IDirectSoundBuffer		*pDirectSoundBuffer;
	DSMIXBINS				stDSMIXBINS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- init ouput -------------------------------------------------------------------------------------------------*/

	*_ppst_DstSB = NULL;

	/*$2- check input ------------------------------------------------------------------------------------------------*/

	if(!_pst_SpeData->pst_DS) return;
	if(!_pst_SrcSB) return;

	/*$2- create a Direct Sound Buffer -------------------------------------------------------------------------------*/

	st_XBOXADPCMWAVEFORMAT = _pst_SrcSB->st_XBOXADPCMWAVEFORMAT;

	L_memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.lpwfxFormat = &st_XBOXADPCMWAVEFORMAT.wfx;
	dsbd.dwSize = sizeof(DSBUFFERDESC);

    dsbd.dwFlags = DSBCAPS_LOCDEFER; //No hardware lock on creation of buffer

	hr = DirectSoundCreateBuffer(&dsbd, &pDirectSoundBuffer);
	if(hr != DS_OK) return;

	/*$2- init sound buffer ------------------------------------------------------------------------------------------*/

	pst_SB = MEM_p_Alloc(sizeof(SND_tdst_SoundBuffer));
	L_memset(pst_SB, 0, sizeof(SND_tdst_SoundBuffer));

	pst_SB->st_XBOXADPCMWAVEFORMAT = st_XBOXADPCMWAVEFORMAT;
	pst_SB->pst_DSB = pDirectSoundBuffer;
	pst_SB->pv_RamBuffer = NULL;
	pst_SB->ui_BufferSize = _pst_SrcSB->ui_BufferSize;
	pst_SB->pst_SS = NULL;
	pst_SB->i_CurFrequency = st_XBOXADPCMWAVEFORMAT.wfx.nSamplesPerSec;

	pst_SB->i_CurPan = -1;
	pst_SB->i_CurSPan = -1;
	pst_SB->i_CurVol = -1;

	pst_SB->i_CurFxVol = _pst_SrcSB->i_CurFxVol;
	pst_SB->i_CurNoFxVol = _pst_SrcSB->i_CurNoFxVol;
    pst_SB->i_MixBinNb = _pst_SrcSB->i_MixBinNb;
	L_memcpy(pst_SB->dst_MixBinVolumePairs, _pst_SrcSB->dst_MixBinVolumePairs, 8 * sizeof(DSMIXBINVOLUMEPAIR));

	stDSMIXBINS.dwMixBinCount = pst_SB->i_MixBinNb ;
	stDSMIXBINS.lpMixBinVolumePairs = pst_SB->dst_MixBinVolumePairs;

	hr = IDirectSoundBuffer_SetMixBins(pDirectSoundBuffer, &stDSMIXBINS);
	xbSND_M_Assert(hr == DS_OK);

	/*$2- fill in the DirectSound buffer -----------------------------------------------------------------------------*/

	hr = IDirectSoundBuffer_SetBufferData(pst_SB->pst_DSB, _pst_SrcSB->pv_RamBuffer, pst_SB->ui_BufferSize);
	xbSND_M_Assert(hr == DS_OK);

	xbSND_SetFilter(pst_SB);

	/*$2- assign the new buffer --------------------------------------------------------------------------------------*/

	*_ppst_DstSB = pst_SB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_SetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int _i_Pos)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	if(!_pst_SB) return;
	if(!_pst_SB->pst_DSB) return;

	hr = IDirectSoundBuffer8_SetCurrentPosition(_pst_SB->pst_DSB, _i_Pos);
	xbSND_M_Assert(hr == DS_OK);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_GetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pos, int *_pi_Write)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	*_pi_Pos = 0;
	if(_pst_SB->pst_SS)
	{
		xbSND_Stream_GetCurrPos(_pst_SB, _pi_Pos, _pi_Write);
	}
	else
	{
		hr = IDirectSoundBuffer8_GetCurrentPosition(_pst_SB->pst_DSB, _pi_Pos, _pi_Write);
		xbSND_M_Assert(hr == DS_OK);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_SetVolume(SND_tdst_SoundBuffer *_pst_SB, int _i_Vol)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HRESULT		hr;
	extern int	pouet(SND_tdst_SoundBuffer *, int);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SB) return;
	if(!_pst_SB->pst_DSB) return;
    if(!_pst_SB->pst_SS && (_pst_SB->i_CurVol == _i_Vol)) return;

    if(_pst_SB->pst_SS)
        EnterCriticalSection(&xbSND_gx_InitSettingsSection);
	
	_pst_SB->i_CurVol = _i_Vol;

	_i_Vol += (300 + _pst_SB->i_CurNoFxVol);
	if(_i_Vol > DSBVOLUME_MAX) _i_Vol = DSBVOLUME_MAX;
	if(_i_Vol < DSBVOLUME_MIN) _i_Vol = DSBVOLUME_MIN;

	hr = IDirectSoundBuffer8_SetVolume(_pst_SB->pst_DSB, _i_Vol);
	xbSND_M_Assert(hr == DS_OK);

    if(_pst_SB->pst_SS)
        LeaveCriticalSection(&xbSND_gx_InitSettingsSection);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_SetFrequency(SND_tdst_SoundBuffer *_pst_SB, int _i_Freq)
{
	/*~~~~~~~*/
	HRESULT hr;
	extern volatile BOOL	SND_gb_PausingAll ;
	/*~~~~~~~*/

	if(!_pst_SB) return;
	if(!_pst_SB->pst_DSB) return;

	if(SND_gb_PausingAll)
	{
		if(_i_Freq)
		{
			hr = IDirectSoundBuffer_Pause(_pst_SB->pst_DSB,DSBPAUSE_RESUME );
			xbSND_M_Assert(hr == DS_OK);
		}
		else
		{
			hr = IDirectSoundBuffer_Pause(_pst_SB->pst_DSB, DSBPAUSE_PAUSE);
			xbSND_M_Assert(hr == DS_OK);
		}
	}
	else
	{
		if(!_pst_SB->pst_SS && (_pst_SB->i_CurFrequency == _i_Freq)) return;
		if(_pst_SB->pst_SS)
			EnterCriticalSection(&xbSND_gx_InitSettingsSection);

		_pst_SB->i_CurFrequency = _i_Freq;


		if(_i_Freq < DSBFREQUENCY_MIN) _i_Freq = DSBFREQUENCY_MIN;
		if(_i_Freq > DSBFREQUENCY_MAX) _i_Freq = DSBFREQUENCY_MAX;

		hr = IDirectSoundBuffer8_SetFrequency(_pst_SB->pst_DSB, _i_Freq);
		xbSND_M_Assert(hr == DS_OK);

		if(_pst_SB->pst_SS)
			LeaveCriticalSection(&xbSND_gx_InitSettingsSection);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_GetFrequency(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Freq)
{
	*_pi_Freq = _pst_SB->i_CurFrequency;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_SetPan(SND_tdst_SoundBuffer *_pst_SB, int _i_Pan, int _i_SPan)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int			idx, Sidx;
	float		f;
	DSMIXBINS	MixBins;
	HRESULT		hr;
	/*~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SB) return;

	/*$1- check differences ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    
	if(!_pst_SB->pst_SS && (_pst_SB->i_CurPan == _i_Pan) && (_pst_SB->i_CurSPan == _i_SPan)) return;

    if(_pst_SB->pst_SS)
        EnterCriticalSection(&xbSND_gx_InitSettingsSection);

	_pst_SB->i_CurPan = _i_Pan;
	_pst_SB->i_CurSPan = _i_SPan;

/**/
	if(_i_Pan > 0)
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

/**/
	if(_i_SPan > 0)
	{
		f = SND_f_GetVolFromAtt(-_i_SPan);
		Sidx = (int) (127.0f - (63.0f * f));
	}
	else if(_i_SPan < 0)
	{
		f = SND_f_GetVolFromAtt(_i_SPan);
		Sidx = (int) (64.0f * f);
	}
	else
	{
		Sidx = 64;
	}

	if(Sidx < 0) idx = 0;
	if(Sidx > 127) idx = 127;

	/*$1- set values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if 1
    if(_pst_SB->st_XBOXADPCMWAVEFORMAT.wfx.nChannels > 1)
    {
        // treat stereo file as 4 voices sounds
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontLeft].lVolume = -500;
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontRight].lVolume = -500;
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackLeft].lVolume = -700;
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackRight].lVolume = -700;

		/*
		_pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontLeft].lVolume = -300;
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontRight].lVolume = -300;
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackLeft].lVolume = -700;
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackRight].lVolume = -700;
		*/

        // and nothing to center
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_CenterLeft].lVolume = DSBVOLUME_MIN;
		_pst_SB->dst_MixBinVolumePairs[xbSND_Idx_CenterRight].lVolume = DSBVOLUME_MIN;
    }
    else 
#endif
    if(_i_Pan || _i_SPan)
	{
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontLeft].lVolume = xbSND_M_GetVolume(xbSND_ai_FrontLeftPan[idx] + xbSND_ai_RearSurroundPan[127 - Sidx]);
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontRight].lVolume = xbSND_M_GetVolume(xbSND_ai_FrontRightPan[idx] + xbSND_ai_RearSurroundPan[127 - Sidx]);
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_CenterLeft].lVolume = xbSND_M_GetVolume(xbSND_ai_FrontCenterPan[idx] + xbSND_ai_RearSurroundPan[127 - Sidx]);
		_pst_SB->dst_MixBinVolumePairs[xbSND_Idx_CenterRight].lVolume = xbSND_M_GetVolume(xbSND_ai_FrontCenterPan[idx] + xbSND_ai_RearSurroundPan[127 - Sidx]);
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackLeft].lVolume = xbSND_M_GetVolume(xbSND_ai_RearLeftPan[idx] + xbSND_ai_RearSurroundPan[Sidx]);
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackRight].lVolume = xbSND_M_GetVolume(xbSND_ai_RearLeftPan[127 - idx] + xbSND_ai_RearSurroundPan[Sidx]);
	}
    else
    {
        Sidx = 106; // not 3D sound, force to be front
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontLeft].lVolume = xbSND_M_GetVolume(xbSND_ai_FrontLeftPan[idx] + xbSND_ai_RearSurroundPan[127 - Sidx]);
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FrontRight].lVolume = xbSND_M_GetVolume(xbSND_ai_FrontRightPan[idx] + xbSND_ai_RearSurroundPan[127 - Sidx]);
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_CenterLeft].lVolume = xbSND_M_GetVolume(xbSND_ai_FrontCenterPan[idx] + xbSND_ai_RearSurroundPan[127 - Sidx]);
		_pst_SB->dst_MixBinVolumePairs[xbSND_Idx_CenterRight].lVolume = xbSND_M_GetVolume(xbSND_ai_FrontCenterPan[idx] + xbSND_ai_RearSurroundPan[127 - Sidx]);
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackLeft].lVolume = xbSND_M_GetVolume(xbSND_ai_RearLeftPan[idx] + xbSND_ai_RearSurroundPan[Sidx]);
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_BackRight].lVolume = xbSND_M_GetVolume(xbSND_ai_RearLeftPan[127 - idx] + xbSND_ai_RearSurroundPan[Sidx]);
    }

    MixBins.dwMixBinCount = _pst_SB->i_MixBinNb;
	MixBins.lpMixBinVolumePairs = _pst_SB->dst_MixBinVolumePairs;

	hr = IDirectSoundBuffer_SetMixBinVolumes(_pst_SB->pst_DSB, &MixBins);
	xbSND_M_Assert(hr == DS_OK);

    if(_pst_SB->pst_SS)
        LeaveCriticalSection(&xbSND_gx_InitSettingsSection);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SB_GetPan(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pan, int *_pi_SPan)
{
	*_pi_Pan = _pst_SB->i_CurPan;
	*_pi_SPan = _pst_SB->i_CurSPan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SetGlobalVol(LONG _l_Volume)
{
	/*~~~~~~~~~~*/
	DWORD	wBits;
#define M_MinGlobalVol	-5000
	/*~~~~~~~~~~*/

	/* record volume */
	xbSND_gl_GlobalVol = _l_Volume;
	if(xbSND_gl_GlobalVol < M_MinGlobalVol) xbSND_gl_GlobalVol = M_MinGlobalVol;

	/* del offset 0->-10000 B => 10000->0 */
	wBits = 10000 + xbSND_gl_GlobalVol;

	/* scale 10000->0 => 0->7 */
	wBits = (7 * wBits) / 10000;

	/* add offset */
	wBits = 7 - wBits;

	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFrontLeft, wBits);	
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFrontRight, wBits);
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendBackLeft, wBits);
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendBackRight, wBits);	
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendCenterLeft, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendCenterRight, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxReverbALeft, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxReverbARight, wBits);	
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxEchoALeft, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxEchoARight, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxReverbBLeft, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxReverbBRight, wBits);	
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxEchoBLeft, wBits);		
	IDirectSound_SetMixBinHeadroom(SND_gst_Params.pst_SpecificD->pst_DS, xbSND_Cte_SendFxEchoBRight, wBits);		
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG xbSND_l_GetGlobalVol(void)
{
	return xbSND_gl_GlobalVol;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_SetGlobalPan(LONG _l_Pan)
{
	/* Empty function */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG xbSND_l_GetGlobalPan(void)
{
	/* Empty function */
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_SB_GetStatus(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Status)
{
	/*~~~~~~~~*/
	HRESULT ret;
	/*~~~~~~~~*/

	if(!_pst_SB) return 0;
	if(!_pst_SB->pst_DSB) return 0;

	if(_pst_SB->pst_SS)
	{
		xbSND_StreamGetStatus(_pst_SB, _pi_Status);
		ret = DS_OK;
	}
	else
	{
		ret = IDirectSoundBuffer8_GetStatus(_pst_SB->pst_DSB, _pi_Status);
		xbSND_M_Assert(ret == DS_OK);
	}

	if((_pst_SB->pst_SS) && (*_pi_Status & SND_Cul_SBS_Playing)) *_pi_Status |= SND_Cul_SF_StreamIsPlayingNow;

	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_CommitChange(MATH_tdst_Vector *pv)
{
	DirectSoundDoWork();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_ChangeRenderMode(int _i_Mode)
{
	/*~~~~~~~~~~~~*/
	DWORD	dwFlags;
	/*~~~~~~~~~~~~*/

	/* get the current capacity */
	dwFlags = XGetAudioFlags();

	/* translate the config to */
	if(_i_Mode == SND_Cte_RenderUseConsoleSettings)
	{
		switch(XC_AUDIO_FLAGS_BASIC(dwFlags))
		{
		case XC_AUDIO_FLAGS_MONO:
			_i_Mode = SND_Cte_RenderMono;
			break;

		case XC_AUDIO_FLAGS_STEREO:
			_i_Mode = SND_Cte_RenderStereo;
			break;

		case XC_AUDIO_FLAGS_SURROUND:
			if(XC_AUDIO_FLAGS_ENCODED(dwFlags) & XC_AUDIO_FLAGS_ENABLE_AC3)
				_i_Mode = SND_Cte_RenderDolbyDigital;
			else
				_i_Mode = SND_Cte_RenderDolbyPrologic;
			break;

		default:
			xbSND_M_Assert(0);
			return -1;
		}
	}

	/* configure the device */
	switch(_i_Mode)
	{
	case SND_Cte_RenderMono:
		SND_gst_Params.ul_RenderMode = SND_Cte_RenderMono;
		DirectSoundOverrideSpeakerConfig(DSSPEAKER_MONO);
		break;

	case SND_Cte_RenderStereo:
		if(XC_AUDIO_FLAGS_BASIC(dwFlags) == XC_AUDIO_FLAGS_STEREO)
		{
			SND_gst_Params.ul_RenderMode = SND_Cte_RenderStereo;
			DirectSoundOverrideSpeakerConfig(DSSPEAKER_STEREO);
		}
		else
		{
			xbSND_i_ChangeRenderMode(SND_Cte_RenderMono);
			return -1;
		}
		break;

	case SND_Cte_RenderHeadPhone:
		if(XC_AUDIO_FLAGS_BASIC(dwFlags) == XC_AUDIO_FLAGS_STEREO)
		{
			SND_gst_Params.ul_RenderMode = SND_Cte_RenderHeadPhone;
			DirectSoundOverrideSpeakerConfig(DSSPEAKER_STEREO);
		}
		else
		{
			xbSND_i_ChangeRenderMode(SND_Cte_RenderMono);
			return -1;
		}
		break;

	case SND_Cte_RenderDolbyPrologic:
		if(XC_AUDIO_FLAGS_BASIC(dwFlags) == XC_AUDIO_FLAGS_SURROUND)
		{
			SND_gst_Params.ul_RenderMode = SND_Cte_RenderDolbyPrologic;
			DirectSoundOverrideSpeakerConfig(DSSPEAKER_SURROUND);
		}
		else
		{
			xbSND_i_ChangeRenderMode(SND_Cte_RenderStereo);
			return -1;
		}
		break;

	case SND_Cte_RenderDolbyDigital:
		if
		(
			(XC_AUDIO_FLAGS_BASIC(dwFlags) == XC_AUDIO_FLAGS_SURROUND)
		&&	(XC_AUDIO_FLAGS_ENCODED(dwFlags) & XC_AUDIO_FLAGS_ENABLE_AC3)
		)
		{
			SND_gst_Params.ul_RenderMode = SND_Cte_RenderDolbyDigital;
		}
		else
		{
			xbSND_i_ChangeRenderMode(SND_Cte_RenderDolbyPrologic);
			return -1;
		}
		break;

	default:
		xbSND_M_Assert(0);
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static unsigned int xbSND_ui_FreqToHardwareCoeff(float fFreq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		lOctaves;
	float	fFC;
	float	fNormCutoff = fFreq / 48000.0f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * Filter is ineffective out of these ranges, so why £
	 * bother even trying?
	 */
	if(fFreq < 30.0f) return 0x8000;
	if(fFreq > 8000.0f) return 0x0;

	fFC = (float) (2.0f * fSin(D3DX_PI * fNormCutoff));

	/* log(fFC)/log(2) */
	lOctaves = (int) (4096.0f * log(fFC) / (0.6931));

	return (unsigned int) lOctaves & 0xFFFF;
}

/*
 =======================================================================================================================
	used for improving the xbox sound rendering (suppress hight freq noise)
 =======================================================================================================================
 */
void xbSND_SetFilter(SND_tdst_SoundBuffer *pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	HRESULT			hr;
	DSFILTERDESC	st_Filter;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	st_Filter.dwMode = DSFILTER_MODE_PARAMEQ;
	st_Filter.dwQCoefficient = 0;

	st_Filter.adwCoefficients[0] = xbSND_ui_FreqToHardwareCoeff(8000.0f);
	st_Filter.adwCoefficients[1] = 0xc000 - -0x1000;
	st_Filter.adwCoefficients[2] = xbSND_ui_FreqToHardwareCoeff(8000.0f);
	st_Filter.adwCoefficients[3] = 0xc000 - -0x1000;

	hr = IDirectSoundBuffer_SetFilter(pst_SB->pst_DSB, &st_Filter);
	xbSND_M_Assert(hr == DS_OK);
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

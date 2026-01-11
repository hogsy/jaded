/*$T xbSNDfx.c GC 1.138 01/14/05 11:31:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/xbox/xbSND_debug.h"
#include "SouND/Sources/xbox/xbSNDfx.h"
#include "ENGine/Sources/DEModisk/DEModisk.h"

#include <stddef.h>
#include <dsound.h>
#include "jadedsp.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define xbSND_Csz_DefautlImage	"jadedsp.bin"

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	xbSND_tdst_FxManager_
{
	BOOL				b_FxIsEnabled;
	SND_tdst_FxParam	st_CurrentFxParam;
	short				s_LeftSend;
	short				s_RightSend;
} xbSND_tdst_FxManager;

typedef struct	xbSND_tdst_FxImage_
{
	BOOL				b_FxIsUsable;
	unsigned int		ui_ImageSize;
	char				*pc_ImageBuffer;
	DSEFFECTIMAGEDESC	*po_EffectImageDesc;
} xbSND_tdst_FxImage;

/*$4
 ***********************************************************************************************************************
    private functions
 ***********************************************************************************************************************
 */

static int	xbSND_i_LoadDspImage(char *, char **, unsigned int *);
extern CRITICAL_SECTION		xbSND_gx_InitSettingsSection;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

#define YoYo	400

/*$1- PRESET ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*$off*/
static DSI3DL2LISTENER xbSND_gast_FxPreset[] =
{
	/*					lRoom		lRoomHF		fRoomRolloffFactor 
														fDecayTime fDecayHFRatio 
																				lReflections 
																							fReflectionsDelay 
																										lReverb		fReverbDelay 
																																fDiffusion fDensity	fHFReference	 */

	/* Off */			{ -10000,	-10000,		0.0f,  1.00f,		1.00f,		-10000,		0.0f,		-10000,		0.000f,		0.0f,		0.0f,		5000.0f },
	/* Room */			{ -900,		-454,		0.0f,  0.80f,		1.03f,		-1646,		0.02f,		253,		0.0f,		100.0f,		100.0f,		5000.0f },
	/* StdA */			{ -900,		-1200,		0.0f,  0.9f,		1.54f,		-370,		0.04f,		200,		0.01f,		100.0f,		60.0f,		5000.0f },
	/* StdB */			{ -900,		-500,		0.0f,  1.1f,		1.5f,		-1076,		0.0f,		-104,		0.04f,		100.0f,		100.0f,		5000.0f },
	/* StdC */			{ -1000,	-300,		0.0f,  1.5f,		0.64f,		-711,		0.08f,		83,			0.02f,		100.0f,		100.0f,		5000.0f },
	/* Hall */			{ -900,		-200,		0.0f,  2.2f,		0.89f,		-1219,		0.1f,		141,		0.01f,		85.0f,		100.0f,		5000.0f },
	/* Space */		{ -1000,	-600,		0.0f,  4.0f,		0.99f,		-1214,		0.3f,		595,		0.1f,		100.0f,		100.0f,		5000.0f },
	/* Echo */			{ -1000,	-1000,		0.0f,  1.1f,		0.21f,		-1800,		0.3f,		595,		0.1f,		50.0f,		100.0f,		5000.0f },
    /* Delay */		{ -1000,	-1000,		0.0f,  1.1f,		0.21f,		-1800,		0.3f,		595,		0.1f,		50.0f,		100.0f,		5000.0f },
    /* Pipe */			{ -1400,	-3000,		0.0f,  2.21f,		0.24f,		429,		0.01f,		2000,		0.02f,		50.0f,		100.0f,		10000.0f },
	/* Mountains */   { -1000,	-2500,		0.0f,  1.49f,		0.31f,		-2780,		0.30f,		-1614,		0.10f,		30.0f,		100.0f,		5000.0f },
	/* City */			{ -1000,	-800,		0.0f,  1.29f,		0.67f,		-2273,		0.06f,		-1517,		0.05f,		60.0f,		73.0f,		5000.0f }
};
/*$on*/

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct xbSND_tdst_FxCollection_
{
    ULONG   ul_Size;
    ULONG   ul_Element;
    ULONG   *dul_Table;
} xbSND_tdst_FxCollection;

xbSND_tdst_FxManager	xbSND_gdst_FxManager[SND_Cte_FxCoreNb];
xbSND_tdst_FxImage		xbSND_gst_FxImage;
xbSND_tdst_FxCollection xbSND_gax_FxBufferList[SND_Cte_FxCoreNb];

/*$2- DEBUG + TEST ---------------------------------------------------------------------------------------------------*/

int		glFx_iMode = SND_Cte_FxMode_Off;
int		glFx_iModeNew = 0;
int		glFx_lRoom = -10000;
int		glFx_lRoomHF = 0;
float	glFx_flRoomRolloffFactor = 0.0f;
float	glFx_flDecayTime = 1.0f;
float	glFx_flDecayHFRatio = 0.5f;
int		glFx_lReflections = -10000;
float	glFx_flReflectionsDelay = 0.02f;
int		glFx_lReverb = -10000;
float	glFx_flReverbDelay = 0.04f;
float	glFx_flDiffusion = 100.0f;
float	glFx_flDensity = 100.0f;
float	glFx_flHFReference = 5000.0f;
int		glFx_Wet = -300;
int		glFx_iGain = 1;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */
void xbSND_InsertToCollection(ULONG ulValue, xbSND_tdst_FxCollection * pCollection)
{
    if(pCollection->ul_Size == 0)
    {
	    pCollection->ul_Size = 100;
	    pCollection->dul_Table = MEM_p_Alloc(pCollection->ul_Size * sizeof(ULONG));
	    L_memset(pCollection->dul_Table, 0, pCollection->ul_Size * sizeof(ULONG));
    }
    else if(pCollection->ul_Element >= pCollection->ul_Size)
    {
	    pCollection->ul_Size += 100;
	    pCollection->dul_Table = MEM_p_Realloc
		    (
			    pCollection->dul_Table,
			    pCollection->ul_Size * sizeof(ULONG)
		    );
        L_memset(&pCollection->dul_Table[pCollection->ul_Element], 0, 100 * sizeof(ULONG));
    }

    pCollection->dul_Table[pCollection->ul_Element] = ulValue;
    pCollection->ul_Element++;
}

void xbSND_DeleteFromCollection(ULONG ulValue, xbSND_tdst_FxCollection * pCollection)
{
    ULONG i;

    for(i=0; i<pCollection->ul_Element; i++)
    {
        if(pCollection->dul_Table[i] != ulValue) continue;

        pCollection->dul_Table[i] = pCollection->dul_Table[pCollection->ul_Element-1];
        pCollection->dul_Table[pCollection->ul_Element-1] = 0;
        pCollection->ul_Element--;        
        break;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxInit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HRESULT				hr;
	DSEFFECTIMAGELOC	EffectLoc;
	char				TempString[256];
	char				*p = DEM_GetLaunchPath();
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(p == NULL)
		strcpy(TempString, "D:\\");
	else
		strcpy(TempString, p);
	strcat(TempString, xbSND_Csz_DefautlImage);

	/* reset */
	L_memset(&xbSND_gdst_FxManager[0], 0, SND_Cte_FxCoreNb * sizeof(xbSND_tdst_FxManager));
	L_memset(&xbSND_gst_FxImage, 0, sizeof(xbSND_tdst_FxImage));

	/* load image */
	if(xbSND_i_LoadDspImage(TempString, &xbSND_gst_FxImage.pc_ImageBuffer, &xbSND_gst_FxImage.ui_ImageSize))
		return -1;

	/* enable image */
	xbSND_gst_FxImage.b_FxIsUsable = TRUE;

	EffectLoc.dwI3DL2ReverbIndex = GraphI3DL2_I3DL2Reverb;
	EffectLoc.dwCrosstalkIndex = DSFX_IMAGELOC_UNUSED;	/* no Xtalk (HRTF only) */
	hr = IDirectSound_DownloadEffectsImage
		(
			SND_gst_Params.pst_SpecificD->pst_DS,
			xbSND_gst_FxImage.pc_ImageBuffer,
			xbSND_gst_FxImage.ui_ImageSize,
			&EffectLoc,
			&xbSND_gst_FxImage.po_EffectImageDesc
		);
	xbSND_M_Assert(hr == DS_OK);



    L_memset(xbSND_gax_FxBufferList, 0, SND_Cte_FxCoreNb * sizeof(xbSND_tdst_FxCollection));

    xbSND_gax_FxBufferList[SND_Cte_FxCoreA].ul_Size = 100;
	xbSND_gax_FxBufferList[SND_Cte_FxCoreA].dul_Table = MEM_p_Alloc(xbSND_gax_FxBufferList[SND_Cte_FxCoreA].ul_Size * sizeof(ULONG));
    L_memset(xbSND_gax_FxBufferList[SND_Cte_FxCoreA].dul_Table, 0, xbSND_gax_FxBufferList[SND_Cte_FxCoreA].ul_Size * sizeof(ULONG));

    xbSND_gax_FxBufferList[SND_Cte_FxCoreB].ul_Size = 100;
	xbSND_gax_FxBufferList[SND_Cte_FxCoreB].dul_Table = MEM_p_Alloc(xbSND_gax_FxBufferList[SND_Cte_FxCoreB].ul_Size * sizeof(ULONG));
    L_memset(xbSND_gax_FxBufferList[SND_Cte_FxCoreB].dul_Table, 0, xbSND_gax_FxBufferList[SND_Cte_FxCoreB].ul_Size * sizeof(ULONG));

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int xbSND_i_LoadDspImage(char *_asz_ImageName, char **_ppc_ResultBuffer, unsigned int *_pui_ResultSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HANDLE			hFile;
	unsigned int	ui_FileSize, ui_ReadSize;
	char			*pc_Buffer;
	BOOL			bResult;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- reset output ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_ppc_ResultBuffer = NULL;
	*_pui_ResultSize = 0;

	/*$1- open the file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	do{
		hFile = CreateFile(_asz_ImageName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

		if(hFile == INVALID_HANDLE_VALUE)
		{
			/*~~~~~~~~*/
			DWORD	err;
			/*~~~~~~~~*/

			err = GetLastError();
			Gx8_FileError();
		}
	} while(hFile == INVALID_HANDLE_VALUE);

	/*$1- get file size ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ui_FileSize = SetFilePointer(hFile, 0, NULL, FILE_END);
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	/*$1- alloc buffer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Buffer = MEM_p_Alloc(ui_FileSize);
	if(!pc_Buffer)
	{
		CloseHandle(hFile);
		xbSND_M_Assert(0);
		return -1;
	}

	/*$1- read file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	do
	{
		L_memset(pc_Buffer, 0, ui_FileSize);
		ui_ReadSize = 0;
		bResult = ReadFile(hFile, pc_Buffer, ui_FileSize, &ui_ReadSize, 0);
		if(!bResult || (ui_ReadSize != ui_FileSize))
		{
			Gx8_FileError();
		}
	}
	while(!bResult || (ui_ReadSize != ui_FileSize));

	/*$1- set output ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_ppc_ResultBuffer = pc_Buffer;
	*_pui_ResultSize = ui_ReadSize;

	CloseHandle(hFile);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_FxClose(void)
{
    int i;
	if(xbSND_gst_FxImage.pc_ImageBuffer) MEM_Free(xbSND_gst_FxImage.pc_ImageBuffer);
	xbSND_gst_FxImage.pc_ImageBuffer = NULL;

    for(i=0; i<SND_Cte_FxCoreNb; i++)
    {
        if(xbSND_gax_FxBufferList[i].dul_Table) MEM_Free(xbSND_gax_FxBufferList[i].dul_Table);
        xbSND_gax_FxBufferList[i].dul_Table = NULL;
        xbSND_gax_FxBufferList[i].ul_Element = 0;
        xbSND_gax_FxBufferList[i].ul_Size = 0;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxSetMode(int core, int mode)
{
	switch(mode)
	{
	case SND_Cte_FxMode_Off:
		/* we assign mode off to echo-A, with -100dB wet volume */
		xbSND_gdst_FxManager[core].s_LeftSend = xbSND_Cte_SendFxEchoALeft;
		xbSND_gdst_FxManager[core].s_RightSend = xbSND_Cte_SendFxEchoARight;
		xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Mode = mode;
		break;

	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_Mountains:
	case SND_Cte_FxMode_City:
	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_Delay:
		if(core == SND_Cte_FxCoreA)
		{
			xbSND_gdst_FxManager[core].s_LeftSend = xbSND_Cte_SendFxReverbALeft;
			xbSND_gdst_FxManager[core].s_RightSend = xbSND_Cte_SendFxReverbARight;
		}
		else
		{
			xbSND_gdst_FxManager[core].s_LeftSend = xbSND_Cte_SendFxReverbBLeft;
			xbSND_gdst_FxManager[core].s_RightSend = xbSND_Cte_SendFxReverbBRight;
		}

		xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Mode = mode;
		break;
/*
	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_Delay:
		if(core == SND_Cte_FxCoreA)
		{
			xbSND_gdst_FxManager[core].s_LeftSend = xbSND_Cte_SendFxEchoALeft;
			xbSND_gdst_FxManager[core].s_RightSend = xbSND_Cte_SendFxEchoARight;
		}
		else
		{
			xbSND_gdst_FxManager[core].s_LeftSend = xbSND_Cte_SendFxEchoBLeft;
			xbSND_gdst_FxManager[core].s_RightSend = xbSND_Cte_SendFxEchoBRight;
		}

		xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Mode = mode;
		break;*/

	default:
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxGetMode(int core)
{
	return xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Mode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxSetFeedback(int core, int d)
{
	xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Feedback = d;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxGetDelay(int core)
{
	return xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Delay;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxSetDelay(int core, int d)
{
	xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Delay = d;
/*
    if(d > 100) d = 100;
    
    xbSND_gast_FxPreset[SND_Cte_FxMode_Echo].flReverbDelay = (float)d / 1000.0f;
    xbSND_gast_FxPreset[SND_Cte_FxMode_Delay].flReverbDelay = xbSND_gast_FxPreset[SND_Cte_FxMode_Echo].flReverbDelay;
*/
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxGetFeedback(int core)
{
	return xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Feedback;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxSetWetVolume(int core, int d)
{
	xbSND_gdst_FxManager[core].st_CurrentFxParam.f_WetVol = SND_f_GetVolFromAtt(d);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxGetWetPan(int core)
{
	return xbSND_gdst_FxManager[core].st_CurrentFxParam.i_WetPan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxGetWetVolume(int core)
{
	return SND_l_GetAttFromVol(xbSND_gdst_FxManager[core].st_CurrentFxParam.f_WetVol);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxSetWetPan(int core, int d)
{
	xbSND_gdst_FxManager[core].st_CurrentFxParam.i_WetPan = d;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxEnable(int core)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									i;
	SND_tdst_SoundBuffer				*pSB;
	int									lWet;
	HRESULT								hr;
	DSMIXBINS							dsMixBins;
	DWORD								dwGain;
	DSFX_HIGH_LEVEL_EFFECT_DESCRIPTION	stEffectDescription;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!xbSND_gst_FxImage.b_FxIsUsable) return -1;
	xbSND_gdst_FxManager[core].b_FxIsEnabled = TRUE;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

    lWet = SND_l_GetAttFromVol(xbSND_gdst_FxManager[core].st_CurrentFxParam.f_WetVol);
	switch(xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Mode)
	{
	case SND_Cte_FxMode_Off:
		lWet = DSBVOLUME_MIN;
		xbSND_gdst_FxManager[core].b_FxIsEnabled = FALSE;
		break;

	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_City:
	case SND_Cte_FxMode_Mountains:
case SND_Cte_FxMode_Echo:
case SND_Cte_FxMode_Delay:
		stEffectDescription.effectType = DSFX_EFFECT_TYPE_I3DL2REVERB;
		L_memcpy
		(
			&stEffectDescription.I3DL2Reverb,
			&xbSND_gast_FxPreset[xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Mode],
			sizeof(DSI3DL2LISTENER)
		);

		hr = XAudioSetEffectData
			(
				(core == SND_Cte_FxCoreA) ? GraphI3DL2_I3DL2Reverb : UserI3DL224KReverbB_I3DL224KReverbB,
				&stEffectDescription,
				NULL
			);
		xbSND_M_Assert(hr == DS_OK);
		break;

	/*case SND_Cte_FxMode_Echo:
        /*
		dwGain = 8388;		
		IDirectSound_SetEffectData
		(
			SND_gst_Params.pst_SpecificD->pst_DS,
			(core == SND_Cte_FxCoreA) ? UserStereoEchoA_StereoEchoA : UserStereoEchoB_StereoEchoB,
			offsetof(DSFX_ECHO_STEREO_PARAMS, dwGain),
			(DWORD *) &dwGain,
			sizeof(DWORD),
			DSFX_IMMEDIATE
		);
        stEffectDescription.effectType = DSFX_EFFECT_TYPE_I3DL2REVERB;
		L_memcpy
		(
			&stEffectDescription.I3DL2Reverb,
			&xbSND_gast_FxPreset[xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Mode],
			sizeof(DSI3DL2LISTENER)
		);

		hr = XAudioSetEffectData
			(
				(core == SND_Cte_FxCoreA) ? GraphI3DL2_I3DL2Reverb : UserI3DL224KReverbB_I3DL224KReverbB,
				&stEffectDescription,
				NULL
			);
		xbSND_M_Assert(hr == DS_OK);
		break;


	case SND_Cte_FxMode_Delay:
        
		dwGain = 1 << 23;	
		IDirectSound_SetEffectData
		(
			SND_gst_Params.pst_SpecificD->pst_DS,
			(core == SND_Cte_FxCoreA) ? UserStereoEchoA_StereoEchoA : UserStereoEchoB_StereoEchoB,
			offsetof(DSFX_ECHO_STEREO_PARAMS, dwGain),
			(DWORD *) &dwGain,
			sizeof(DWORD),
			DSFX_IMMEDIATE
		);
        
		stEffectDescription.effectType = DSFX_EFFECT_TYPE_I3DL2REVERB;
		L_memcpy
		(
			&stEffectDescription.I3DL2Reverb,
			&xbSND_gast_FxPreset[xbSND_gdst_FxManager[core].st_CurrentFxParam.i_Mode],
			sizeof(DSI3DL2LISTENER)
		);


		hr = XAudioSetEffectData
			(
				(core == SND_Cte_FxCoreA) ? GraphI3DL2_I3DL2Reverb : UserI3DL224KReverbB_I3DL224KReverbB,
				&stEffectDescription,
				NULL
			);
		xbSND_M_Assert(hr == DS_OK);
		break;*/

	default:
		return -1;
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

    
    for(i = 0; i < xbSND_gax_FxBufferList[core].ul_Element; i++)
	{
        if(xbSND_gax_FxBufferList[core].dul_Table[i] == 0) continue;
        pSB = (SND_tdst_SoundBuffer *) xbSND_gax_FxBufferList[core].dul_Table[i];

        if(pSB->pst_SS)
            EnterCriticalSection(&xbSND_gx_InitSettingsSection);

        pSB->dst_MixBinVolumePairs[xbSND_Idx_FxLeft].dwMixBin = xbSND_gdst_FxManager[core].s_LeftSend;
		pSB->dst_MixBinVolumePairs[xbSND_Idx_FxLeft].lVolume = xbSND_M_GetVolume(lWet - YoYo) + pSB->i_CurFxVol;
		pSB->dst_MixBinVolumePairs[xbSND_Idx_FxRight].dwMixBin = xbSND_gdst_FxManager[core].s_RightSend;
		pSB->dst_MixBinVolumePairs[xbSND_Idx_FxRight].lVolume = xbSND_M_GetVolume(lWet - YoYo) + pSB->i_CurFxVol;
		pSB->i_MixBinNb = xbSND_Idx_WithFxNb;    

		dsMixBins.lpMixBinVolumePairs = pSB->dst_MixBinVolumePairs;
        dsMixBins.dwMixBinCount = pSB->i_MixBinNb;
		hr = IDirectSoundBuffer_SetMixBins(pSB->pst_DSB, &dsMixBins);
		xbSND_M_Assert(hr == DS_OK);

        if(pSB->pst_SS)
            LeaveCriticalSection(&xbSND_gx_InitSettingsSection);
	}
    

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxDisable(int core)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	SND_tdst_SoundBuffer	*pSB;
	HRESULT					hr;
	DSMIXBINS				dsMixBins;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!xbSND_gst_FxImage.b_FxIsUsable) return -1;

	
    
    for(i = 0; i < xbSND_gax_FxBufferList[core].ul_Element; i++)
	{
        if(xbSND_gax_FxBufferList[core].dul_Table[i] == 0) continue;
        pSB = (SND_tdst_SoundBuffer *) xbSND_gax_FxBufferList[core].dul_Table[i];

        if(pSB->pst_SS)
            EnterCriticalSection(&xbSND_gx_InitSettingsSection);

        pSB->dst_MixBinVolumePairs[xbSND_Idx_FxLeft].dwMixBin = xbSND_gdst_FxManager[core].s_LeftSend;
		pSB->dst_MixBinVolumePairs[xbSND_Idx_FxLeft].lVolume = DSBVOLUME_MIN;
		pSB->dst_MixBinVolumePairs[xbSND_Idx_FxRight].dwMixBin = xbSND_gdst_FxManager[core].s_RightSend;
		pSB->dst_MixBinVolumePairs[xbSND_Idx_FxRight].lVolume = DSBVOLUME_MIN;
		pSB->i_MixBinNb = xbSND_Idx_WithFxNb;   
        
        dsMixBins.dwMixBinCount = pSB->i_MixBinNb;
		dsMixBins.lpMixBinVolumePairs = pSB->dst_MixBinVolumePairs;
		hr = IDirectSoundBuffer_SetMixBins(pSB->pst_DSB, &dsMixBins);
		xbSND_M_Assert(hr == DS_OK);

        if(pSB->pst_SS)
            LeaveCriticalSection(&xbSND_gx_InitSettingsSection);
    }
    

	xbSND_gdst_FxManager[core].b_FxIsEnabled = FALSE;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxAddSB(int core, SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	HRESULT		hr;
	DSMIXBINS	dsMixBins;
	LONG		lWet;
	/*~~~~~~~~~~~~~~~~~~~*/

	if(!xbSND_gst_FxImage.b_FxIsUsable) return -1;

    EnterCriticalSection(&xbSND_gx_InitSettingsSection);

    xbSND_InsertToCollection((ULONG) _pst_SB, &xbSND_gax_FxBufferList[core]);

    if(xbSND_gdst_FxManager[core].b_FxIsEnabled) 
    {
	    lWet = SND_l_GetAttFromVol(xbSND_gdst_FxManager[core].st_CurrentFxParam.f_WetVol);
    		
        _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FxLeft].dwMixBin = xbSND_gdst_FxManager[core].s_LeftSend;
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FxLeft].lVolume = xbSND_M_GetVolume(lWet - YoYo) + _pst_SB->i_CurFxVol;
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FxRight].dwMixBin = xbSND_gdst_FxManager[core].s_RightSend;
	    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FxRight].lVolume = xbSND_M_GetVolume(lWet - YoYo) + _pst_SB->i_CurFxVol;
	    _pst_SB->i_MixBinNb = xbSND_Idx_WithFxNb;   

        dsMixBins.dwMixBinCount = _pst_SB->i_MixBinNb ;
	    dsMixBins.lpMixBinVolumePairs = _pst_SB->dst_MixBinVolumePairs;

	    hr = IDirectSoundBuffer_SetMixBins(_pst_SB->pst_DSB, &dsMixBins);
	    xbSND_M_Assert(hr == DS_OK);
    }

    LeaveCriticalSection(&xbSND_gx_InitSettingsSection);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_FxDelSB(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~*/
	HRESULT		hr;
	DSMIXBINS	dsMixBins;
	/*~~~~~~~~~~~~~~~~~~*/

	if(!xbSND_gst_FxImage.b_FxIsUsable) return -1;
    
    EnterCriticalSection(&xbSND_gx_InitSettingsSection);

    xbSND_DeleteFromCollection((ULONG) _pst_SB, &xbSND_gax_FxBufferList[SND_Cte_FxCoreA]);
	xbSND_DeleteFromCollection((ULONG) _pst_SB, &xbSND_gax_FxBufferList[SND_Cte_FxCoreB]);


    _pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FxLeft].dwMixBin = xbSND_gdst_FxManager[SND_Cte_FxCoreA].s_LeftSend;
	_pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FxLeft].lVolume = DSBVOLUME_MIN;
	_pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FxRight].dwMixBin = xbSND_gdst_FxManager[SND_Cte_FxCoreA].s_RightSend;
	_pst_SB->dst_MixBinVolumePairs[xbSND_Idx_FxRight].lVolume = DSBVOLUME_MIN;
	_pst_SB->i_MixBinNb = xbSND_Idx_WithFxNb;   

    dsMixBins.dwMixBinCount = _pst_SB->i_MixBinNb ;
	dsMixBins.lpMixBinVolumePairs = _pst_SB->dst_MixBinVolumePairs;

	hr = IDirectSoundBuffer_SetMixBins(_pst_SB->pst_DSB, &dsMixBins);
	xbSND_M_Assert(hr == DS_OK);
    

    LeaveCriticalSection(&xbSND_gx_InitSettingsSection);

    return 0;
}

void xbSND_FxReinit(void)
{
    int core;

    for(core = 0; core < SND_Cte_FxCoreNb; core++)
    {
        xbSND_gax_FxBufferList[core].ul_Element = 0;

        if(xbSND_gax_FxBufferList[core].dul_Table)
            L_memset(xbSND_gax_FxBufferList[core].dul_Table, 0, xbSND_gax_FxBufferList[core].ul_Size * sizeof(ULONG));
    }
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

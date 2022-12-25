/*$T gcSNDfx.c GC 1.138 12/07/04 12:53:23 */

/*$F
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Memory allocated by AXFXReverbStdInit()
=======================================
if (preDelay value passed to AXFXReverbStdInit() != 0)
  53088 + ( ( ( (s32)((preDelay value) * 32000) * 4) + 32) * 3) bytes
else
  53088 bytes


Memory allocated by AXFXReverbHiInit()
======================================
if (preDelay value passed to AXFXReverbHiInit() != 0)
  82144 + ( ( ( (s32)((preDelay value) * 32000) * 4) + 32) * 3) bytes
else
  82144 bytes


Memory allocated by AXFXReverbHiInitDpl2()
==========================================
if (preDelay value passed to AXFXReverbHiInitDpl2() != 0)
  109568 + ( ( ( (s32)((preDelay value) * 32000) * 4) + 32) * 4) bytes
//Note: 4 at end of expression! => 160 Ko max
else
  109568 bytes

 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifdef _GAMECUBE

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <dolphin/axfx.h>
#include <dolphin/mix.h>

#include "CLIbrary/CLIstr.h"
#include "MEMory/MEM.h"

#include "SND.h"
#include "SNDstruct.h"
#include "SNDconst.h"
#include "SNDfx.h"
#include "SNDvolume.h"
#include "gc/gcSND.h"
#include "gc/gcSND_Soft.h"
#include "gc/gcSNDfx.h"
#include "gc/gcSND_Debug.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define __ReverbHi		0
#define __ReverbDPL2	1
#define __Delay			2
#define __FxNbMax		3

typedef struct	gcSND_tdst_FxPreset_
{
	float	coloration;
	float	mix;
	float	time;
	float	damping;
	float	preDelay;
} gcSND_tdst_FxPreset;

/*$off*/
gcSND_tdst_FxPreset			gcSND_fa_FxPreset[] =
{
//  { coloration,   mix,    time,   damping,    predelay } name
	{ 0.5f,			0.5f,	1.1f,	0.3f,		0.02f	}, /* room */
	{ 0.7f,			0.5f,	1.0f,	0.2f,		0.02f	}, /* stdA */
	{ 0.7f,			0.5f,	1.2f,	0.3f,		0.02f	}, /* stdB */
	{ 0.7f,			0.5f,	2.0f,	0.7f,		0.03f	}, /* stdC */
	{ 0.8f,			0.5f,	2.6f,	0.4f,		0.03f	}, /* Hall */
	{ 0.7f,			0.5f,	6.0f,	0.8f,		0.099f	}, /* space */
	{ 0.7f,			0.5f,	6.0f,	1.0f,		0.1f	}, /* *delay* */
	{ 0.7f,			0.5f,	6.0f,	1.0f,		0.1f	}, /* *echo* */
	{ 0.7f,			0.5f,	2.0f,	0.9f,		0.02f	}, /* pipe */
	{ 0.7f,			0.5f,	6.0f,	1.0f,		0.1f	}, /* mountains */
	{ 0.7f,			0.5f,	1.0f,	1.0f,		0.1f	}  /* city */
};
/*$on*/

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	gcSND_tdst_FxKit_
{
	int (*pfi_FxInit) (void *);
	int (*pfi_FxShutdown) (void *);
	int (*pfi_FxSettings) (void *);
	void (*pfv_FxCallback) (void *, void *);
}
gcSND_tdst_FxKit;

#define gcSND_Cte_FxHasChanged_Mode		0x00000001
#define gcSND_Cte_FxHasChanged_Delay	0x00000002
#define gcSND_Cte_FxHasChanged_Feedback 0x00000004

typedef struct	gcSND_tdst_FxParams_
{
	unsigned int		ui_HasChanged;

	/* classic set */
	int					i_FxMode;
	int					i_WetVolume;
	int					i_WetPan;

	/* only delay+echo */
	int					i_Delay;
	int					i_Feedback;

	gcSND_tdst_FxKit	*i;
	void 				(*pfv_FxRegisterCallback)(AXAuxCallback callback, void *context);
	void				*pv_FxBuffer;
	unsigned int		ui_FxBufferSize;

	int					i_Aux;
	BOOL				b_FxIsActive;
} gcSND_tdst_FxParams;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

int		mAXFXReverbHiInit_A(AXFX_REVERBSTD *rev);
int		mAXFXReverbHiShutdown_A(AXFX_REVERBSTD *rev);
int		mAXFXReverbHiSettings_A(AXFX_REVERBSTD *rev);
void	mAXFXReverbHiCallback_A(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev);
int		mAXFXReverbHiInitDpl2_A(AXFX_REVERBSTD *rev);
int		mAXFXReverbHiShutdownDpl2_A(AXFX_REVERBSTD *rev);
int		mAXFXReverbHiSettingsDpl2_A(AXFX_REVERBSTD *rev);
void	mAXFXReverbHiCallbackDpl2_A(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev);
int		mAXFXDelayInit_A(AXFX_REVERBSTD *rev);
int		mAXFXDelayShutdown_A(AXFX_REVERBSTD *rev);
int		mAXFXDelaySettings_A(AXFX_REVERBSTD *rev);
void	mAXFXDelayCallback_A(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev);

/*
 * £
 */
int		mAXFXReverbHiInit_B(AXFX_REVERBSTD *rev);
int		mAXFXReverbHiShutdown_B(AXFX_REVERBSTD *rev);
int		mAXFXReverbHiSettings_B(AXFX_REVERBSTD *rev);
void	mAXFXReverbHiCallback_B(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev);
int		mAXFXReverbHiInitDpl2_B(AXFX_REVERBSTD *rev);
int		mAXFXReverbHiShutdownDpl2_B(AXFX_REVERBSTD *rev);
int		mAXFXReverbHiSettingsDpl2_B(AXFX_REVERBSTD *rev);
void	mAXFXReverbHiCallbackDpl2_B(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev);
int		mAXFXDelayInit_B(AXFX_REVERBSTD *rev);
int		mAXFXDelayShutdown_B(AXFX_REVERBSTD *rev);
int		mAXFXDelaySettings_B(AXFX_REVERBSTD *rev);
void	mAXFXDelayCallback_B(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

int					gcSND_gi_FxShutDown_A = 0;
int					gcSND_gi_FxShutDown_B = 0;

gcSND_tdst_FxParams gcSND_gst_FxParamsAux[SND_Cte_FxCoreNb];

/*$off*/
gcSND_tdst_FxKit	gcSND_gax_FxBasicInterface[SND_Cte_FxCoreNb][__FxNbMax] =
{
	{ /* coreA */
		{
			// __ReverbHi,
			mAXFXReverbHiInit_A,
			mAXFXReverbHiShutdown_A,
			mAXFXReverbHiSettings_A,
			mAXFXReverbHiCallback_A
		},
		{
			// __ReverbDPL2,
			mAXFXReverbHiInitDpl2_A,
			mAXFXReverbHiShutdownDpl2_A,
			mAXFXReverbHiSettingsDpl2_A,
			mAXFXReverbHiCallbackDpl2_A
		},
		{
			// __Delay,
			mAXFXDelayInit_A,
			mAXFXDelayShutdown_A,
			mAXFXDelaySettings_A,
			mAXFXDelayCallback_A
		}
	},
	{ /* coreB */
		{
			// __ReverbHi,
			mAXFXReverbHiInit_B,
			mAXFXReverbHiShutdown_B,
			mAXFXReverbHiSettings_B,
			mAXFXReverbHiCallback_B
		},
		{
			// __ReverbDPL2,
			mAXFXReverbHiInitDpl2_B,
			mAXFXReverbHiShutdownDpl2_B,
			mAXFXReverbHiSettingsDpl2_B,
			mAXFXReverbHiCallbackDpl2_B
		},
		{
			// __Delay,
			mAXFXDelayInit_B,
			mAXFXDelayShutdown_B,
			mAXFXDelaySettings_B,
			mAXFXDelayCallback_B
		}
	}
};
/*$on*/

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    alloc in special heap in order to fragment less.
 =======================================================================================================================
 */

void *gcSND_MEM_p_Alloc(ULONG _ul_Size)
{
	return MEM_p_AllocMem(_ul_Size,&MEM_gst_SoundMemoryInfo);
}

void gcSND_MEM_Free(void *_pAddress)
{
	MEM_Free(_pAddress); 
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_FxInit(void)
{
	/*~~~~~~~*/
	int j;
	unsigned int uiAlloc;
	/*~~~~~~~*/

	L_memset(gcSND_gst_FxParamsAux, 0, SND_Cte_FxCoreNb * sizeof(gcSND_tdst_FxParams));

	AXFXSetHooks(gcSND_MEM_p_Alloc, gcSND_MEM_Free );

	uiAlloc = max(sizeof(AXFX_REVERBHI), max(sizeof(AXFX_REVERBHI_DPL2), sizeof(AXFX_DELAY)));
	for(j = 0; j < SND_Cte_FxCoreNb; j++) 
	{
		gcSND_gst_FxParamsAux[j].pv_FxBuffer = MEM_p_Alloc(uiAlloc);
		gcSND_gst_FxParamsAux[j].ui_FxBufferSize = uiAlloc;
	}
	
	gcSND_gst_FxParamsAux[SND_Cte_FxCoreA].pfv_FxRegisterCallback = AXRegisterAuxACallback;
	gcSND_gst_FxParamsAux[SND_Cte_FxCoreB].pfv_FxRegisterCallback = AXRegisterAuxBCallback;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_FxClose(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < SND_Cte_FxCoreNb; i++)
	{
		if(gcSND_gst_FxParamsAux[i].i)
		{
			gcSND_gst_FxParamsAux[i].i->pfi_FxShutdown(gcSND_gst_FxParamsAux[i].pv_FxBuffer);
			gcSND_gst_FxParamsAux[i].i = NULL;
		}

		if(gcSND_gst_FxParamsAux[i].pv_FxBuffer) MEM_Free(gcSND_gst_FxParamsAux[i].pv_FxBuffer);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_FxSetMode(int _i_Core, int _i_Value)
{
	switch(_i_Value)
	{
	case SND_Cte_FxMode_Off:
	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_Mountains:
	case SND_Cte_FxMode_City:
		break;

	default:
		_i_Value = SND_Cte_FxMode_Off;
		break;
	}

	if(gcSND_gst_FxParamsAux[_i_Core].i_FxMode != _i_Value)
	{
		gcSND_gst_FxParamsAux[_i_Core].i_FxMode = _i_Value;
		gcSND_gst_FxParamsAux[_i_Core].ui_HasChanged |= gcSND_Cte_FxHasChanged_Mode;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_FxGetMode(int _i_Core)
{
	return gcSND_gst_FxParamsAux[_i_Core].i_FxMode;
}

/*
 =======================================================================================================================
    val:: 10 -> 5000ms
 =======================================================================================================================
 */
int gcSND_i_FxSetDelay(int _i_Core, int _i_Value)
{
	if(_i_Value > AXFX_DELAY_MAX_DELAY) _i_Value = AXFX_DELAY_MAX_DELAY;
	if(_i_Value < AXFX_DELAY_MIN_DELAY) _i_Value = AXFX_DELAY_MIN_DELAY;

	if(gcSND_gst_FxParamsAux[_i_Core].i_Delay != _i_Value)
	{
		gcSND_gst_FxParamsAux[_i_Core].i_Delay = _i_Value;
		gcSND_gst_FxParamsAux[_i_Core].ui_HasChanged |= gcSND_Cte_FxHasChanged_Delay;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_FxGetDelay(int _i_Core)
{
	return gcSND_gst_FxParamsAux[_i_Core].i_Delay;
}

/*
 =======================================================================================================================
    0->100 %
 =======================================================================================================================
 */
int gcSND_i_FxSetFeedback(int _i_Core, int _i_Value)
{
	if(_i_Value > AXFX_DELAY_MAX_FEEDBACK) _i_Value = AXFX_DELAY_MAX_FEEDBACK;
	if(_i_Value < AXFX_DELAY_MIN_FEEDBACK) _i_Value = AXFX_DELAY_MIN_FEEDBACK;

	if(gcSND_gst_FxParamsAux[_i_Core].i_Feedback != _i_Value)
	{
		gcSND_gst_FxParamsAux[_i_Core].i_Feedback = _i_Value;
		gcSND_gst_FxParamsAux[_i_Core].ui_HasChanged |= gcSND_Cte_FxHasChanged_Feedback;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_FxGetFeedback(int _i_Core)
{
	return gcSND_gst_FxParamsAux[_i_Core].i_Feedback;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_i_FxSetWetVolume(int _i_Core, int _i_Level)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float					wet;
	int						i,j;
	gcSND_tdst_SoftBuffer	*pSB;
	unsigned int			ui_Mask;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* att[-10 000 ; 0] => vol[0.0 ; 1.0] */
	wet = SND_f_GetVolFromAtt(_i_Level);

	/* vol => att */
	gcSND_gst_FxParamsAux[_i_Core].i_WetVolume = SND_l_GetAttFromVol(wet);

	/* aux = 2 x wet_vol <=> wetvol + 6dB */
	gcSND_gst_FxParamsAux[_i_Core].i_Aux = (gcSND_gst_FxParamsAux[_i_Core].i_WetVolume / 10) + 60;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    update all aux send
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ui_Mask = (_i_Core == SND_Cte_FxCoreA) ? SND_Cul_SF_FxA : SND_Cul_SF_FxB;
	ui_Mask |= SND_Cul_DSF_Used;

	for(i = 0, pSB = gcSND_gax_SoftBuffer; i < SND_Cte_MaxSoftBufferNb; i++, pSB++)
	{
		/* skip unused or bad core */
		if((pSB->ui_Flags & ui_Mask) != ui_Mask) continue;

		pSB->i_MIXaux[_i_Core] = pSB->i_FxVol + gcSND_gst_FxParamsAux[_i_Core].i_Aux;
		pSB->i_MIXaux[1 - _i_Core] = gcSND_Cte_MinVol;
		
		#if gcSND_Cte_NbAUX>2
		for(j = 2; j < gcSND_Cte_NbAUX;++j)
			pSB->i_MIXaux[j] = gcSND_Cte_MinVol;
		#endif

		if(pSB->ast_Hard[0].pst_AxBuffer)
		{
			MIXSetAuxA(pSB->ast_Hard[0].pst_AxBuffer, pSB->i_MIXaux[SND_Cte_FxCoreA]);
			MIXSetAuxB(pSB->ast_Hard[0].pst_AxBuffer, pSB->i_MIXaux[SND_Cte_FxCoreB]);
		}

		if(pSB->ast_Hard[1].pst_AxBuffer)
		{
			MIXSetAuxA(pSB->ast_Hard[1].pst_AxBuffer, pSB->i_MIXaux[SND_Cte_FxCoreA]);
			MIXSetAuxB(pSB->ast_Hard[1].pst_AxBuffer, pSB->i_MIXaux[SND_Cte_FxCoreB]);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_FxGetWetVolume(int _i_Core)
{
	return gcSND_gst_FxParamsAux[_i_Core].i_WetVolume;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_i_FxSetWetPan(int _i_Core, int _i_Pan)
{
	gcSND_gst_FxParamsAux[_i_Core].i_WetPan = _i_Pan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_FxGetWetPan(int _i_Core)
{
	return gcSND_gst_FxParamsAux[_i_Core].i_WetPan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_FxEnable(int _i_Core)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AXFX_REVERBHI			*pReverbHi;
	AXFX_REVERBHI_DPL2		*pReverbDPL2;
	AXFX_DELAY				*pDelay;
	BOOL					old;
	int						i;
	gcSND_tdst_SoftBuffer	*pSB;
	unsigned int			ui_Mask;
	gcSND_tdst_FxPreset		*pPreset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(gcSND_gst_FxParamsAux[_i_Core].ui_HasChanged == 0) return 0;
	gcSND_gst_FxParamsAux[_i_Core].ui_HasChanged = 0;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ui_Mask = (_i_Core == SND_Cte_FxCoreA) ? SND_Cul_SF_FxA : SND_Cul_SF_FxB;
	ui_Mask |= SND_Cul_DSF_Used;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    kill previous Fx
	 -------------------------------------------------------------------------------------------------------------------
	 */

	old = OSDisableInterrupts();
	if(gcSND_gst_FxParamsAux[_i_Core].i)
	{
		gcSND_gst_FxParamsAux[_i_Core].i->pfi_FxShutdown(gcSND_gst_FxParamsAux[_i_Core].pv_FxBuffer);
		gcSND_gst_FxParamsAux[_i_Core].i = NULL;
		gcSND_gst_FxParamsAux[_i_Core].pfv_FxRegisterCallback(NULL, NULL);
	}
	OSRestoreInterrupts(old);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    get new inteerface
	 -------------------------------------------------------------------------------------------------------------------
	 */

	switch(gcSND_gst_FxParamsAux[_i_Core].i_FxMode)
	{
	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Mountains:
	case SND_Cte_FxMode_City:
	case SND_Cte_FxMode_Pipe:
		if(SND_gst_Params.ul_RenderMode == SND_Cte_RenderDolbyPrologicII)
			gcSND_gst_FxParamsAux[_i_Core].i = &gcSND_gax_FxBasicInterface[_i_Core][__ReverbDPL2];
		else
			gcSND_gst_FxParamsAux[_i_Core].i = &gcSND_gax_FxBasicInterface[_i_Core][__ReverbHi];
		break;

	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_Delay:
		gcSND_gst_FxParamsAux[_i_Core].i = &gcSND_gax_FxBasicInterface[_i_Core][__Delay];
		break;

	default:
		gcSND_gst_FxParamsAux[_i_Core].b_FxIsActive = FALSE;
		for(i = 0, pSB = gcSND_gax_SoftBuffer; i < SND_Cte_MaxSoftBufferNb; i++, pSB++)
		{
			if((pSB->ui_Flags & ui_Mask) != ui_Mask) continue;
			gcSND_FxDelSB(pSB);
			pSB->ui_Flags |= ui_Mask;
		}

		return 0;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    init new Fx
	 -------------------------------------------------------------------------------------------------------------------
	 */

	L_memset(gcSND_gst_FxParamsAux[_i_Core].pv_FxBuffer, 0, gcSND_gst_FxParamsAux[_i_Core].ui_FxBufferSize);

	/* specific settings */
	switch(gcSND_gst_FxParamsAux[_i_Core].i_FxMode)
	{
	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_City:
	case SND_Cte_FxMode_Mountains:
	case SND_Cte_FxMode_Pipe:
		pPreset = gcSND_fa_FxPreset + gcSND_gst_FxParamsAux[_i_Core].i_FxMode - 1;

		if(SND_gst_Params.ul_RenderMode == SND_Cte_RenderDolbyPrologicII)
		{
			pReverbDPL2 = (AXFX_REVERBHI_DPL2 *) gcSND_gst_FxParamsAux[_i_Core].pv_FxBuffer;
			pReverbDPL2->mix = pPreset->mix;
			pReverbDPL2->coloration = pPreset->coloration;
			pReverbDPL2->time = pPreset->time;
			pReverbDPL2->damping = pPreset->damping;
			pReverbDPL2->preDelay = pPreset->preDelay;
		}
		else
		{
			pReverbHi = (AXFX_REVERBHI *) gcSND_gst_FxParamsAux[_i_Core].pv_FxBuffer;
			pReverbHi->mix = pPreset->mix;
			pReverbHi->coloration = pPreset->coloration;
			pReverbHi->time = pPreset->time;
			pReverbHi->damping = pPreset->damping;
			pReverbHi->preDelay = pPreset->preDelay;
			pReverbHi->crosstalk = 1.0f;
		}
		break;

	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_Delay:
		pDelay = (AXFX_DELAY *) gcSND_gst_FxParamsAux[_i_Core].pv_FxBuffer;
		pDelay->delay[0] = gcSND_gst_FxParamsAux[_i_Core].i_Delay;
		pDelay->delay[1] = gcSND_gst_FxParamsAux[_i_Core].i_Delay;
		pDelay->delay[2] = gcSND_gst_FxParamsAux[_i_Core].i_Delay;
		pDelay->feedback[0] = gcSND_gst_FxParamsAux[_i_Core].i_Feedback;
		pDelay->feedback[1] = gcSND_gst_FxParamsAux[_i_Core].i_Feedback;
		pDelay->feedback[2] = gcSND_gst_FxParamsAux[_i_Core].i_Feedback;
		pDelay->output[0] = 30;//50;			/* output = 50% dry + 50% wet */
		pDelay->output[1] = pDelay->output[0];
		pDelay->output[2] = pDelay->output[0];
		break;

	default:
		gcSND_M_Assert(0);
		break;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    activate the hardware
	 -------------------------------------------------------------------------------------------------------------------
	 */

	old = OSDisableInterrupts();
	gcSND_gst_FxParamsAux[_i_Core].i->pfi_FxInit(gcSND_gst_FxParamsAux[_i_Core].pv_FxBuffer);
	gcSND_gst_FxParamsAux[_i_Core].pfv_FxRegisterCallback
		(
			gcSND_gst_FxParamsAux[_i_Core].i->pfv_FxCallback,
			gcSND_gst_FxParamsAux[_i_Core].pv_FxBuffer
		);
	OSRestoreInterrupts(old);

	gcSND_gst_FxParamsAux[_i_Core].b_FxIsActive = TRUE;
	return 0;
}

static BOOL sb_FxPause[SND_Cte_FxCoreNb] = { FALSE, FALSE };

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_FxPause(void)
{
	/*~~~~~~*/
	int iCore;
	/*~~~~~~*/

	for(iCore = 0; iCore < SND_Cte_FxCoreNb; iCore++)
	{
		if(!gcSND_gst_FxParamsAux[iCore].b_FxIsActive) continue;
		sb_FxPause[iCore] = TRUE;
		gcSND_i_FxDisable(iCore);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_FxResume(void)
{
	/*~~~~~~*/
	int iCore;
	/*~~~~~~*/

	if(!sb_FxPause) return;

	for(iCore = 0; iCore < SND_Cte_FxCoreNb; iCore++)
	{
		if(!sb_FxPause[iCore]) continue;
		sb_FxPause[iCore] = FALSE;
		gcSND_i_FxEnable(iCore);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_FxDisable(int _i_Core)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	gcSND_tdst_SoftBuffer	*pSB;
	BOOL					old;
	unsigned int			ui_Mask;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    kill previous Fx
	 -------------------------------------------------------------------------------------------------------------------
	 */

	old = OSDisableInterrupts();
	if(gcSND_gst_FxParamsAux[_i_Core].i)
	{
		gcSND_gst_FxParamsAux[_i_Core].i->pfi_FxShutdown(gcSND_gst_FxParamsAux[_i_Core].pv_FxBuffer);
		gcSND_gst_FxParamsAux[_i_Core].i = NULL;
		gcSND_gst_FxParamsAux[_i_Core].pfv_FxRegisterCallback(NULL, NULL);
	}

	OSRestoreInterrupts(old);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    mute all Fx channels
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(gcSND_gst_FxParamsAux[_i_Core].b_FxIsActive)
	{
		ui_Mask = (_i_Core == SND_Cte_FxCoreA) ? SND_Cul_SF_FxA : SND_Cul_SF_FxB;
		ui_Mask |= SND_Cul_DSF_Used;

		pSB = gcSND_gax_SoftBuffer;
		for(i = 0; i < SND_Cte_MaxSoftBufferNb; i++, pSB++)
		{
			if((pSB->ui_Flags & ui_Mask) != ui_Mask) continue;
			gcSND_FxDelSB(pSB);
			pSB->ui_Flags |= ui_Mask;
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    record the state
	 -------------------------------------------------------------------------------------------------------------------
	 */

	gcSND_gst_FxParamsAux[_i_Core].b_FxIsActive = FALSE;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_FxAddSB(gcSND_tdst_SoftBuffer *pSoft)
{
	/*~~~~~~~~~~~*/
	int _i_Core, i, j;
	/*~~~~~~~~~~~*/

	if(pSoft->ui_Flags & SND_Cul_SF_FxA)
		_i_Core = SND_Cte_FxCoreA;
	else if(pSoft->ui_Flags & SND_Cul_SF_FxB)
		_i_Core = SND_Cte_FxCoreB;
	else
		return;


	pSoft->i_MIXaux[_i_Core] = pSoft->i_FxVol + gcSND_gst_FxParamsAux[_i_Core].i_Aux;
	pSoft->i_MIXaux[1 - _i_Core] = gcSND_Cte_MinVol;
	
	#if gcSND_Cte_NbAUX>2
	for(i = 2; i < gcSND_Cte_NbAUX;++i)
		pSoft->i_MIXaux[i] = gcSND_Cte_MinVol;
	#endif	

	for(i = 0; i < 2; i++)
	{
		if(pSoft->ast_Hard[i].pst_AxBuffer == NULL) continue;

		MIXSetAuxA(pSoft->ast_Hard[i].pst_AxBuffer, pSoft->i_MIXaux[SND_Cte_FxCoreA]);
		MIXSetAuxB(pSoft->ast_Hard[i].pst_AxBuffer, pSoft->i_MIXaux[SND_Cte_FxCoreB]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_FxDelSB(gcSND_tdst_SoftBuffer *pst_Soft)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < SND_Cte_FxCoreNb;++i)
		pst_Soft->i_MIXaux[i] = gcSND_Cte_MinVol;

	for(i = 0; i < 2; i++)
	{
		if(pst_Soft->ast_Hard[i].pst_AxBuffer == NULL) continue;

		MIXSetAuxA(pst_Soft->ast_Hard[i].pst_AxBuffer, pst_Soft->i_MIXaux[SND_Cte_FxCoreA]);
		MIXSetAuxB(pst_Soft->ast_Hard[i].pst_AxBuffer, pst_Soft->i_MIXaux[SND_Cte_FxCoreB]);
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
int mAXFXReverbHiInit_A(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_A = 0;
	return AXFXReverbHiInit(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiShutdown_A(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_A = 1;
	return AXFXReverbHiShutdown(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiSettings_A(AXFX_REVERBSTD *rev)
{
	return AXFXReverbHiSettings(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void mAXFXReverbHiCallback_A(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev)
{
	if(gcSND_gi_FxShutDown_A || !update || !rev) return;
	AXFXReverbHiCallback(update, rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiInitDpl2_A(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_A = 0;
	return AXFXReverbHiInitDpl2(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiShutdownDpl2_A(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_A = 1;
	return AXFXReverbHiShutdownDpl2(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiSettingsDpl2_A(AXFX_REVERBSTD *rev)
{
	return AXFXReverbHiSettingsDpl2(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void mAXFXReverbHiCallbackDpl2_A(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev)
{
	if(gcSND_gi_FxShutDown_A || !update || !rev) return;
	AXFXReverbHiCallbackDpl2(update, rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXDelayInit_A(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_A = 0;
	return AXFXDelayInit(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXDelayShutdown_A(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_A = 1;
	return AXFXDelayShutdown(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXDelaySettings_A(AXFX_REVERBSTD *rev)
{
	return AXFXDelaySettings(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void mAXFXDelayCallback_A(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev)
{
	if(gcSND_gi_FxShutDown_A || !update || !rev) return;
	AXFXDelayCallback(update, rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiInit_B(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_B = 0;
	return AXFXReverbHiInit(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiShutdown_B(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_B = 1;
	return AXFXReverbHiShutdown(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiSettings_B(AXFX_REVERBSTD *rev)
{
	return AXFXReverbHiSettings(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void mAXFXReverbHiCallback_B(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev)
{
	if(gcSND_gi_FxShutDown_B || !update || !rev) return;
	AXFXReverbHiCallback(update, rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiInitDpl2_B(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_B = 0;
	return AXFXReverbHiInitDpl2(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiShutdownDpl2_B(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_B = 1;
	return AXFXReverbHiShutdownDpl2(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXReverbHiSettingsDpl2_B(AXFX_REVERBSTD *rev)
{
	return AXFXReverbHiSettingsDpl2(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void mAXFXReverbHiCallbackDpl2_B(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev)
{
	if(gcSND_gi_FxShutDown_B || !update || !rev) return;
	AXFXReverbHiCallbackDpl2(update, rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXDelayInit_B(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_B = 0;
	return AXFXDelayInit(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXDelayShutdown_B(AXFX_REVERBSTD *rev)
{
	gcSND_gi_FxShutDown_B = 1;
	return AXFXDelayShutdown(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int mAXFXDelaySettings_B(AXFX_REVERBSTD *rev)
{
	return AXFXDelaySettings(rev);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void mAXFXDelayCallback_B(AXFX_BUFFERUPDATE *update, AXFX_REVERBSTD *rev)
{
	if(gcSND_gi_FxShutDown_B || !update || !rev) return;
	AXFXDelayCallback(update, rev);
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif

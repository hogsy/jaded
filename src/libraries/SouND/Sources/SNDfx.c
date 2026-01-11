/*$T SNDfx.c GC 1.138 03/09/05 15:59:48 */


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

#define __SNDfx_c__
#include "BASe/BASsys.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"

#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDspecific.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef struct	SND_tdst_FxCrossFade_
{
	int		i_ContextId;
	int		i_Mode;
	float	f_FadeFactor;
	float	f_FadeVol;
} SND_tdst_FxCrossFade;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

SND_tdst_FxParam		SND_dst_FxParam[SND_Cte_FxCoreNb][SND_Cte_FxContextNb];
int						SND_gai_ActiveContext[SND_Cte_FxCoreNb];

int						SND_gi_FxCurrCore;
int						SND_gi_FxCurrContext;

SND_tdst_FxCrossFade	SND_dst_FxCrossFade[SND_Cte_FxCoreNb];

SND_tdst_FxParam		SND_dst_FxLocalParam[SND_Cte_FxCoreNb];

/*$4
 ***********************************************************************************************************************
    open/close module
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_FxInitModule(void)
{
	/*~~~~~*/
	int i, j;
	/*~~~~~*/

	SND_gi_FxCurrCore = SND_gi_FxCurrContext = -1;
	L_memset(&SND_dst_FxParam[0][0], 0, SND_Cte_FxCoreNb * SND_Cte_FxContextNb * sizeof(SND_tdst_FxParam));
	L_memset(SND_gai_ActiveContext, -1, SND_Cte_FxContextNb * sizeof(int));
	L_memset(SND_dst_FxLocalParam, 0, SND_Cte_FxCoreNb * sizeof(SND_tdst_FxParam));

	if(!SND_gst_Params.l_Available) return -1;

	for(i = 0; i < SND_Cte_FxCoreNb; i++)
	{
		SND_dst_FxCrossFade[i].f_FadeFactor = 0.0f;
		SND_dst_FxCrossFade[i].f_FadeVol = 0.0f;
		SND_dst_FxCrossFade[i].i_ContextId = -1;
		SND_dst_FxCrossFade[i].i_Mode = -1;

		for(j = 0; j < SND_Cte_FxContextNb; j++)
		{
			SND_dst_FxParam[i][j].i_Mode = SND_Cte_FxMode_Off;
		}
	}

	SND_gpst_Interface[ISound].pfi_FxInit();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_FxCloseModule(void)
{
	if(!SND_gst_Params.l_Available) return;

	SND_gpst_Interface[ISound].pfv_FxClose();

	SND_gi_FxCurrCore = SND_gi_FxCurrContext = -1;
	L_memset(&SND_dst_FxParam[0][0], 0, SND_Cte_FxCoreNb * SND_Cte_FxContextNb * sizeof(SND_tdst_FxParam));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_FxLocalRequest(int _i_CoreId, float _f_WetVol, int _i_Mode, int _i_Delay, int _i_FeedBack)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_FxParam	*pLocal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pLocal = SND_dst_FxLocalParam + _i_CoreId;

	if(((pLocal->ui_FxFlag & SND_FxFlg_RqValidate) == 0) || (pLocal->f_WetVol < _f_WetVol))
	{
		pLocal->ui_FxFlag |= SND_FxFlg_RqValidate;
		pLocal->f_WetVol = _f_WetVol;
		pLocal->i_Delay = _i_Delay;
		pLocal->i_Feedback = _i_FeedBack;
		pLocal->i_Mode = _i_Mode;
		pLocal->i_WetPan = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_FxValidate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					iCore;
	SND_tdst_FxParam	*pLocal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(iCore = 0; iCore < SND_Cte_FxCoreNb; iCore++)
	{
		pLocal = SND_dst_FxLocalParam + iCore;
		SND_i_FxSeti(SND_Cte_Fx_iCoreId, iCore);

		if(pLocal->ui_FxFlag & SND_FxFlg_RqValidate)
		{
			SND_i_FxSeti(SND_Cte_Fx_iContext, SND_Cte_FxContextLocal);
			SND_i_FxSeti(SND_Cte_Fx_iMode, pLocal->i_Mode);
			SND_i_FxSeti(SND_Cte_Fx_iDelay, pLocal->i_Delay);
			SND_i_FxSeti(SND_Cte_Fx_iFeedback, pLocal->i_Feedback);
			SND_i_FxSetf(SND_Cte_Fx_fWetVol, pLocal->f_WetVol);
		}
		else
		{
			SND_i_FxSeti(SND_Cte_Fx_iContext, SND_Cte_FxContextGlobal);
		}

		pLocal->ui_FxFlag = 0;

		SND_i_FxSeti(SND_Cte_Fx_iWetPan, 0);
		SND_i_FxSeti(SND_Cte_Fx_iEnable, 1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_FxUpdate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				core, ctxt;
	extern float	TIM_gf_dt;
	float			vol;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_FxValidate();

	for(core = 0; core < SND_Cte_FxCoreNb; core++)
	{
		if(!SND_dst_FxCrossFade[core].f_FadeFactor) continue;

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    fading-out
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		if(SND_dst_FxCrossFade[core].f_FadeFactor < 0.0f)
		{
			SND_dst_FxCrossFade[core].f_FadeVol += SND_dst_FxCrossFade[core].f_FadeFactor * TIM_gf_dt;
			if(SND_dst_FxCrossFade[core].f_FadeVol <= 0.0f)
			{
				/* fade is finished */
				SND_dst_FxCrossFade[core].f_FadeVol = 0.0f;
				SND_dst_FxCrossFade[core].f_FadeFactor = 0.0f;

				if(SND_dst_FxCrossFade[core].i_ContextId == -1)
				{
					/* no more fx after the fade-out */
					SND_gpst_Interface[ISound].pfi_FxDisable(core);
					vol = 0.0f;
				}
				else
				{
					/* set the new fx */
					ctxt = SND_dst_FxCrossFade[core].i_ContextId;
					SND_gpst_Interface[ISound].pfi_FxSetMode(core, SND_dst_FxParam[core][ctxt].i_Mode);
					SND_gpst_Interface[ISound].pfi_FxSetDelay(core, SND_dst_FxParam[core][ctxt].i_Delay);
					SND_gpst_Interface[ISound].pfi_FxSetFeedback(core, SND_dst_FxParam[core][ctxt].i_Feedback);
					SND_gpst_Interface[ISound].pfi_FxSetWetPan(core, SND_dst_FxParam[core][ctxt].i_WetPan);
					SND_gpst_Interface[ISound].pfi_FxEnable(core);
					vol = SND_dst_FxParam[core][ctxt].f_WetVol;

					/* force fade-in */
					SND_dst_FxCrossFade[core].f_FadeFactor = 4.0f;
				}
			}

		//	a = SND_dst_FxCrossFade[core].f_FadeVol * SND_dst_FxParam[core][ctxt].f_WetVol;
			//b = SND_l_GetAttFromVol(a);

			SND_gpst_Interface[ISound].pfi_FxSetWetVolume
				(
					core,
					SND_l_GetAttFromVol(SND_dst_FxCrossFade[core].f_FadeVol * vol
				));
			SND_gpst_Interface[ISound].pfi_FxEnable(core);
		}

		/*$1- fading-in ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		else if(SND_dst_FxCrossFade[core].f_FadeFactor > 0.0f)
		{
			SND_dst_FxCrossFade[core].f_FadeVol += SND_dst_FxCrossFade[core].f_FadeFactor * TIM_gf_dt;

			ctxt = SND_dst_FxCrossFade[core].i_ContextId;
			if(SND_dst_FxCrossFade[core].f_FadeVol >= 1.0f)
			{
				/* fade is finished */
				SND_dst_FxCrossFade[core].f_FadeVol = 1.0f;
				SND_dst_FxCrossFade[core].f_FadeFactor = 0.0f;
				SND_dst_FxCrossFade[core].i_ContextId = -1;
			}
			
			//a = SND_dst_FxCrossFade[core].f_FadeVol * SND_dst_FxParam[core][ctxt].f_WetVol;
			//b = SND_l_GetAttFromVol(a);

			SND_gpst_Interface[ISound].pfi_FxSetWetVolume
				(
					core,
					SND_l_GetAttFromVol(SND_dst_FxCrossFade[core].f_FadeVol * SND_dst_FxParam[core][ctxt].f_WetVol
				));
			SND_gpst_Interface[ISound].pfi_FxEnable(core);
		}
	}
}

/*$4
 ***********************************************************************************************************************
    interface
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_FxGeti(int _i_RqId)
{
	if(!SND_gst_Params.l_Available) return -1;
	switch(_i_RqId)
	{
	case SND_Cte_Fx_iCoreId:
		return SND_gi_FxCurrCore;

	case SND_Cte_Fx_iContext:
		return SND_gi_FxCurrContext;

	case SND_Cte_Fx_iActiveContext:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		return SND_gai_ActiveContext[SND_gi_FxCurrCore];

	case SND_Cte_Fx_iMode:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		return SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_Mode;

	case SND_Cte_Fx_iDelay:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		return SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_Delay;

	case SND_Cte_Fx_iFeedback:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		return SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_Feedback;

	case SND_Cte_Fx_iWetPan:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		return SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_WetPan;

	case SND_Cte_Fx_iFlags:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		return * (int *) &SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].ui_FxFlag;

	default:
		ERR_X_Warning(0, "[SND][SND_i_FxSeti] bad request id", NULL);
		return -1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_FxSeti(int _i_RqId, int _i_Value)
{
	/*~~~~~*/
	int i, j;
	/*~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;
	switch(_i_RqId)
	{
	case SND_Cte_Fx_iReinit:
		L_memset(&SND_dst_FxParam[0][0], 0, SND_Cte_FxCoreNb * SND_Cte_FxContextNb * sizeof(SND_tdst_FxParam));
		L_memset(SND_gai_ActiveContext, -1, SND_Cte_FxContextNb * sizeof(int));

		for(i = 0; i < SND_Cte_FxCoreNb; i++)
		{
			SND_dst_FxCrossFade[i].f_FadeFactor = 0.0f;
			SND_dst_FxCrossFade[i].f_FadeVol = 0.0f;
			SND_dst_FxCrossFade[i].i_ContextId = -1;
			SND_dst_FxCrossFade[i].i_Mode = -1;

			for(j = 0; j < SND_Cte_FxContextNb; j++) SND_dst_FxParam[i][j].i_Mode = SND_Cte_FxMode_Off;

			SND_gpst_Interface[ISound].pfi_FxSetWetVolume(i, SND_l_GetAttFromVol(0.0f));
			SND_gpst_Interface[ISound].pfi_FxDisable(i);
		}
		break;

	case SND_Cte_Fx_iCoreId:
		ERR_X_Warning(_i_Value >= 0, "[SND][SND_i_FxSeti] bad core id", NULL);
		ERR_X_Warning(_i_Value < SND_Cte_FxCoreNb, "[SND][SND_i_FxSeti] bad core id", NULL);
		SND_gi_FxCurrCore = _i_Value;
		break;

	case SND_Cte_Fx_iContext:
		ERR_X_Warning(_i_Value >= 0, "[SND][SND_i_FxSeti] bad context id", NULL);
		ERR_X_Warning(_i_Value < SND_Cte_FxContextNb, "[SND][SND_i_FxSeti] bad context id", NULL);
		SND_gi_FxCurrContext = _i_Value;
		break;

	case SND_Cte_Fx_iMode:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		ERR_X_Warning(_i_Value >= SND_Cte_FxMode_Off, "[SND][SND_i_FxSeti] bad mode id", NULL);
		ERR_X_Warning(_i_Value < SND_Cte_FxMode_Nb, "[SND][SND_i_FxSeti] bad mode id", NULL);
		if(SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].ui_FxFlag & SND_FxFlg_Locked) break;
		SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_Mode = _i_Value;
		break;

	case SND_Cte_Fx_iDelay:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		ERR_X_Warning(_i_Value >= 0, "[SND][SND_i_FxSeti] bad delay value", NULL);
		ERR_X_Warning(_i_Value <= 1000, "[SND][SND_i_FxSeti] bad delay value", NULL);
		if(SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].ui_FxFlag & SND_FxFlg_Locked) break;
		SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_Delay = _i_Value;
		break;

	case SND_Cte_Fx_iFeedback:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		ERR_X_Warning(_i_Value >= 0, "[SND][SND_i_FxSeti] bad feedback value", NULL);
		ERR_X_Warning(_i_Value <= 100, "[SND][SND_i_FxSeti] bad feedback value", NULL);
		if(SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].ui_FxFlag & SND_FxFlg_Locked) break;
		SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_Feedback = _i_Value;
		break;

	case SND_Cte_Fx_iWetPan:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		ERR_X_Warning(_i_Value >= -10000, "[SND][SND_i_FxSeti] bad wet pan value", NULL);
		ERR_X_Warning(_i_Value <= 10000, "[SND][SND_i_FxSeti] bad wet pan value", NULL);
		if(SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].ui_FxFlag & SND_FxFlg_Locked) break;
		SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_WetPan = _i_Value;
		break;

	case SND_Cte_Fx_iEnable:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		switch(_i_Value)
		{
		case SND_Cte_FxState_Lock:
			/* lock */
			ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
			for(i = 0; i < SND_Cte_FxContextNb; i++)
				SND_dst_FxParam[SND_gi_FxCurrCore][i].ui_FxFlag |= SND_FxFlg_Locked;
			break;

		case SND_Cte_FxState_Unlock:
			/* unlock */
			ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
			for(i = 0; i < SND_Cte_FxContextNb; i++)
				SND_dst_FxParam[SND_gi_FxCurrCore][i].ui_FxFlag &= ~SND_FxFlg_Locked;
			break;

		case SND_Cte_FxState_Active:
			/* active */
			if(SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].ui_FxFlag & SND_FxFlg_Locked) break;

			if
			(
				SND_dst_FxCrossFade[SND_gi_FxCurrCore].i_Mode != SND_dst_FxParam[SND_gi_FxCurrCore][
					SND_gi_FxCurrContext].i_Mode
			)
			{
				/* change off mode => force a fade-out */
				SND_dst_FxCrossFade[SND_gi_FxCurrCore].i_Mode = SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_Mode;
				SND_dst_FxCrossFade[SND_gi_FxCurrCore].i_ContextId = SND_gi_FxCurrContext;
				SND_dst_FxCrossFade[SND_gi_FxCurrCore].f_FadeFactor = -4.0f;
			}
			else
			{
				SND_gpst_Interface[ISound].pfi_FxSetMode
					(
						SND_gi_FxCurrCore,
						SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_Mode
					);
				SND_gpst_Interface[ISound].pfi_FxSetDelay
					(
						SND_gi_FxCurrCore,
						SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_Delay
					);
				SND_gpst_Interface[ISound].pfi_FxSetFeedback
					(
						SND_gi_FxCurrCore,
						SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_Feedback
					);
				SND_gpst_Interface[ISound].pfi_FxSetWetPan
					(
						SND_gi_FxCurrCore,
						SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].i_WetPan
					);
				SND_gpst_Interface[ISound].pfi_FxEnable(SND_gi_FxCurrCore);
			}

			SND_gai_ActiveContext[SND_gi_FxCurrCore] = SND_gi_FxCurrContext;
			break;

		case SND_Cte_FxState_Inactive:
			/* inactive */
			if(SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].ui_FxFlag & SND_FxFlg_Locked) break;

			/* force a fade-out */
			SND_dst_FxCrossFade[SND_gi_FxCurrCore].f_FadeFactor = -4.0f;
			SND_dst_FxCrossFade[SND_gi_FxCurrCore].i_ContextId = -1;
			SND_dst_FxCrossFade[SND_gi_FxCurrCore].i_Mode = -1;
			SND_gai_ActiveContext[SND_gi_FxCurrCore] = -1;
			break;
		}
		break;

	default:
		ERR_X_Warning(0, "[SND][SND_i_FxSeti] bad request id", NULL);
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_FxSetf(int _i_RqId, float _f_Value)
{
	if(!SND_gst_Params.l_Available) return -1;
	switch(_i_RqId)
	{
	case SND_Cte_Fx_fWetVol:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		ERR_X_Warning(_f_Value >= 0.0f, "[SND][SND_i_FxSeti] bad wet volume value", NULL);
		ERR_X_Warning(_f_Value <= 1.0f, "[SND][SND_i_FxSeti] bad wet volume value", NULL);

		SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].f_WetVol = _f_Value;

		SND_gpst_Interface[ISound].pfi_FxSetWetVolume
			(
				SND_gi_FxCurrCore,
				SND_l_GetAttFromVol
				(
					SND_dst_FxCrossFade[SND_gi_FxCurrCore].f_FadeVol *
						SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].f_WetVol
				)
			);
		return 0;

	default:
		ERR_X_Warning(0, "Bad Argument ID (SND_i_FxSetf)", NULL);
		return -1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_FxGetf(int _i_RqId)
{
	if(!SND_gst_Params.l_Available) return -1.0f;
	switch(_i_RqId)
	{
	case SND_Cte_Fx_fWetVol:
		ERR_X_Warning(SND_gi_FxCurrCore != -1, "[SND][SND_i_FxSeti] core id not assigned", NULL);
		ERR_X_Warning(SND_gi_FxCurrContext != -1, "[SND][SND_i_FxSeti] context id not assigned", NULL);
		return SND_dst_FxParam[SND_gi_FxCurrCore][SND_gi_FxCurrContext].f_WetVol;

	default:
		ERR_X_Warning(0, "Bad Argument ID (SND_f_FxGetf)", NULL);
		return -1.0f;
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

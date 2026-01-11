/*$T ps2SNDfx.c GC 1.138 04/08/04 16:31:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <libsd.h>
#include "BASe/BASsys.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "SNDstruct.h"
#include "SNDconst.h"
#include "SNDfx.h"
#include "SND.h"
#include "SNDvolume.h"
#include "SNDspecific.h"
#include "IOP/RPC_Manager.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */



typedef struct	ps2SND_tdst_FxSettings_
{
	SND_tdst_FxParam		st_CurrFx;
	int		i_WetVol;
	float	f_WetVolLeftFactor;
	float	f_WetVolRightFactor;
	/**/
	RPC_tdst_FxSettings	st_Iop;
} ps2SND_tdst_FxSettings;

/*$4
 ***********************************************************************************************************************
    private functions
 ***********************************************************************************************************************
 */

void	ps2SND_FxFlushSettings(void);

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

ps2SND_tdst_FxSettings	ps2SND_sx_FxSettings[SND_Cte_FxCoreNb];
extern float ps2SND_gaf_PanTableL[128] ;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_FxInit(void)
{
	/*~~~~~*/
	int core;
	/*~~~~~*/

	L_memset(ps2SND_sx_FxSettings, 0, SND_Cte_FxCoreNb * sizeof(ps2SND_tdst_Snd));
	for(core = 0; core < SND_Cte_FxCoreNb; core++)
	{
		ps2SND_sx_FxSettings[SND_Cte_FxCoreA].f_WetVolLeftFactor = ps2SND_gaf_PanTableL[64];
		ps2SND_sx_FxSettings[SND_Cte_FxCoreA].f_WetVolRightFactor = ps2SND_gaf_PanTableL[127-64];
				
		ps2SND_i_FxSetMode(core, SND_Cte_FxMode_Off);
		ps2SND_i_FxSetDelay(core, 0);
		ps2SND_i_FxSetFeedback(core, 0);
		ps2SND_i_FxSetWetPan(core, 0);
		ps2SND_i_FxSetWetVolume(core, -10000);
	}
	
	ps2SND_sx_FxSettings[SND_Cte_FxCoreA].st_Iop.core = SD_CORE_0;
	ps2SND_sx_FxSettings[SND_Cte_FxCoreB].st_Iop.core = SD_CORE_1;

	ps2SND_FxFlushSettings();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_FxClose(void)
{
	/*~~~~~~~~~~~~~~*/
	int core;
	/*~~~~~~~~~~~~~~*/

	L_memset(ps2SND_sx_FxSettings, 0, SND_Cte_FxCoreNb * sizeof(ps2SND_tdst_Snd));
	for(core = 0; core < SND_Cte_FxCoreNb; core++)
	{
		ps2SND_i_FxSetMode(core, SND_Cte_FxMode_Off);
		ps2SND_i_FxSetDelay(core, 0);
		ps2SND_i_FxSetFeedback(core, 0);
		ps2SND_i_FxSetWetPan(core, 0);
		ps2SND_i_FxSetWetVolume(core, -10000);
	}

	ps2SND_FxFlushSettings();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_FxFlushSettings(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						core;
	int						cmd;
	RPC_tdst_FxSettings	astIopFx[SND_Cte_FxCoreNb];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(cmd = core = 0; core < SND_Cte_FxCoreNb; core++)
	{
		L_memcpy(&astIopFx[core], &ps2SND_sx_FxSettings[core].st_Iop, sizeof(RPC_tdst_FxSettings));
		
		cmd |= ps2SND_sx_FxSettings[core].st_Iop.ul_Modif;
		ps2SND_sx_FxSettings[core].st_Iop.ul_Modif = 0;
	}

	if(!cmd) return;

	eeRPC_i_PushCommand(RPC_Cmd_SndSetFx, (char *) astIopFx, SND_Cte_FxCoreNb * sizeof(RPC_tdst_FxSettings), NULL, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_FxSetMode(int core, int _i_Value)
{
	if(core<0) return -1;
	if(core>=SND_Cte_FxCoreNb) return -1;
	
	if(ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode != _i_Value)
		ps2SND_sx_FxSettings[core].st_Iop.ul_Modif |= ps2SND_Cte_FxModifyMode;
	
	switch(_i_Value)
	{
	case SND_Cte_FxMode_Off:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_OFF;
		break;

	case SND_Cte_FxMode_Room:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_ROOM;
		break;

	case SND_Cte_FxMode_StudioA:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_STUDIO_A;
		break;

	case SND_Cte_FxMode_StudioB:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_STUDIO_B;
		break;

	case SND_Cte_FxMode_StudioC:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_STUDIO_C;
		break;

	case SND_Cte_FxMode_Hall:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_HALL;
		break;

	case SND_Cte_FxMode_Space:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_SPACE;
		break;

	case SND_Cte_FxMode_Echo:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_ECHO;
		break;

	case SND_Cte_FxMode_Delay:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_DELAY;
		break;

	case SND_Cte_FxMode_Pipe:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_PIPE;
		break;

	case SND_Cte_FxMode_Mountains:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_DELAY;
		ps2SND_sx_FxSettings[core].st_Iop.delay = iRoundf((259.0f * 127.0f) / 1000.0f);
		ps2SND_sx_FxSettings[core].st_Iop.feedback = iRoundf((20.0f * 127.0f) / 100.0f);
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Delay = ps2SND_sx_FxSettings[core].st_Iop.delay;
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Feedback = ps2SND_sx_FxSettings[core].st_Iop.feedback;
		ps2SND_sx_FxSettings[core].st_Iop.ul_Modif |= ps2SND_Cte_FxModifyDelay|ps2SND_Cte_FxModifyFeedback;
		break;

	case SND_Cte_FxMode_City:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_DELAY;
		ps2SND_sx_FxSettings[core].st_Iop.delay = iRoundf((251.0f * 127.0f) / 1000.0f);
		ps2SND_sx_FxSettings[core].st_Iop.feedback = iRoundf((12.0f * 127.0f) / 100.0f);
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Delay = ps2SND_sx_FxSettings[core].st_Iop.delay;
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Feedback = ps2SND_sx_FxSettings[core].st_Iop.feedback;
		ps2SND_sx_FxSettings[core].st_Iop.ul_Modif |= ps2SND_Cte_FxModifyDelay|ps2SND_Cte_FxModifyFeedback;
		break;

	default:
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = SND_Cte_FxMode_Off;
		ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_OFF;
		break;
	}


	/* printf("(FX) mode %d\n", ps2SND_sx_FxSettings[ICurrent].i_Mode); */
	return 0;
}

/*
 =======================================================================================================================
    delay 127 i sabout 1 seconde. _i_Value is in ms...
 =======================================================================================================================
 */
int ps2SND_i_FxSetDelay(int core, int _i_Value)
{
	int delay = _i_Value;
	
	if(core<0) return -1;
	if(core>=SND_Cte_FxCoreNb) return -1;

	if(ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode == SND_Cte_FxMode_Mountains) return 0;
	if(ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode == SND_Cte_FxMode_City) return 0;

	if(delay < 0)
		delay = 0;
	else if(_i_Value > 1000)
		delay = 127;
	else
		delay = iRoundf((fLongToFloat(_i_Value) * 127.0f) / 1450.0f);

	if(ps2SND_sx_FxSettings[core].st_CurrFx.i_Delay != _i_Value)
	{
		ps2SND_sx_FxSettings[core].st_Iop.ul_Modif |= ps2SND_Cte_FxModifyDelay;
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Delay = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.delay = delay;
	}

	return 0;
}

/*
 =======================================================================================================================
    seems to be the feedback ratio. _i_Value 0->100
 =======================================================================================================================
 */
int ps2SND_i_FxSetFeedback(int core, int _i_Value)
{
	int feedback = _i_Value;
	
	if(core<0) return -1;
	if(core>=SND_Cte_FxCoreNb) return -1;

	if(ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode == SND_Cte_FxMode_Mountains) return 0;
	if(ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode == SND_Cte_FxMode_City) return 0;

	if(feedback < 0)
		feedback = 0;
	else if(_i_Value > 100)
		feedback = 127;
	else
		feedback = iRoundf((fLongToFloat(_i_Value) * 127.0f) / 100.0f);

	if(ps2SND_sx_FxSettings[core].st_CurrFx.i_Feedback != _i_Value)
	{
		ps2SND_sx_FxSettings[core].st_Iop.ul_Modif |= ps2SND_Cte_FxModifyFeedback;
		ps2SND_sx_FxSettings[core].st_CurrFx.i_Feedback = _i_Value;
		ps2SND_sx_FxSettings[core].st_Iop.feedback = feedback;
	}

	/* printf("(FX) feedback %d\n", ps2SND_sx_FxSettings[ICurrent].i_Feedback); */

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_i_FxSetWetVolume(int core, int _i_Level)
{
	if(core<0) return ;
	if(core>=SND_Cte_FxCoreNb) return ;

	if(ps2SND_sx_FxSettings[core].i_WetVol != _i_Level)
		ps2SND_sx_FxSettings[core].st_Iop.ul_Modif |= ps2SND_Cte_FxModifyWetVol;
		
	ps2SND_sx_FxSettings[core].i_WetVol = _i_Level		;
	
	if(_i_Level < 0)
		ps2SND_sx_FxSettings[core].st_CurrFx.f_WetVol = (float) ps2SND_Cte_MaxVol * SND_f_GetVolFromAtt((unsigned int) _i_Level);
	
	else
		ps2SND_sx_FxSettings[core].st_CurrFx.f_WetVol = (float) ps2SND_Cte_MaxVol;


	ps2SND_sx_FxSettings[core].st_Iop.depth_L = (short) iRoundf(ps2SND_sx_FxSettings[core].st_CurrFx.f_WetVol * ps2SND_sx_FxSettings[core].f_WetVolLeftFactor);
	ps2SND_sx_FxSettings[core].st_Iop.depth_R = (short) iRoundf(ps2SND_sx_FxSettings[core].st_CurrFx.f_WetVol * ps2SND_sx_FxSettings[core].f_WetVolRightFactor);

	/* printf("(FX) vol %g\n", ps2SND_sx_FxSettings[ICurrent].f_WetVol); */
}

int ps2SND_i_FxGetWetVolume(int core)
{
	if(core<0) return 0;
	if(core>=SND_Cte_FxCoreNb) return 0;

	return ps2SND_sx_FxSettings[core].i_WetVol;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2SND_i_FxSetWetPan(int core, int _i_Pan)
{
	int idx;
	float f;
	
	if(core<0) return ;
	if(core>=SND_Cte_FxCoreNb) return ;


	if(ps2SND_sx_FxSettings[core].st_CurrFx.i_WetPan != _i_Pan)
		ps2SND_sx_FxSettings[core].st_Iop.ul_Modif |= ps2SND_Cte_FxModifyWetVol;
		
	ps2SND_sx_FxSettings[core].st_CurrFx.i_WetPan = _i_Pan		;

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
		
	ps2SND_sx_FxSettings[core].f_WetVolLeftFactor = ps2SND_gaf_PanTableL[idx];
	ps2SND_sx_FxSettings[core].f_WetVolRightFactor = ps2SND_gaf_PanTableL[127-idx];

	ps2SND_sx_FxSettings[core].st_Iop.depth_L = (short) iRoundf(ps2SND_sx_FxSettings[core].st_CurrFx.f_WetVol * ps2SND_sx_FxSettings[core].f_WetVolLeftFactor);
	ps2SND_sx_FxSettings[core].st_Iop.depth_R = (short) iRoundf(ps2SND_sx_FxSettings[core].st_CurrFx.f_WetVol * ps2SND_sx_FxSettings[core].f_WetVolRightFactor);

	/* printf("(FX) pan %d\n", ps2SND_sx_FxSettings[ICurrent].i_WetPan); */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_FxEnable(int core)
{
	if(core<0) return -1;
	if(core>=SND_Cte_FxCoreNb) return -1;
	ps2SND_FxFlushSettings();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_FxDisable(int core)
{
	if(core<0) return -1;
	if(core>=SND_Cte_FxCoreNb) return -1;
	ps2SND_sx_FxSettings[core].st_Iop.ul_Modif |= ps2SND_Cte_FxModifyMode;
	ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode = SND_Cte_FxMode_Off;
	ps2SND_sx_FxSettings[core].st_Iop.mode = SD_REV_MODE_OFF;
	ps2SND_FxFlushSettings();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_FxGetDelay(int core)
{
	if(core<0) return -1;
	if(core>=SND_Cte_FxCoreNb) return -1;
	return ps2SND_sx_FxSettings[core].st_CurrFx.i_Delay;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_FxGetFeedback(int core)
{
	if(core<0) return -1;
	if(core>=SND_Cte_FxCoreNb) return -1;
	return ps2SND_sx_FxSettings[core].st_CurrFx.i_Feedback;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_FxGetWetPan(int core)
{
	if(core<0) return -1;
	if(core>=SND_Cte_FxCoreNb) return -1;
	return ps2SND_sx_FxSettings[core].st_CurrFx.i_WetPan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2SND_i_FxGetMode(int core)
{
	if(core<0) return -1;
	if(core>=SND_Cte_FxCoreNb) return -1;
	return ps2SND_sx_FxSettings[core].st_CurrFx.i_Mode;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

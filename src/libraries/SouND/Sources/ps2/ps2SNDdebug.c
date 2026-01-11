/*$T ps2SNDdebug.c GC! 1.097 11/26/01 17:35:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_TARGET
#ifndef _FINAL_

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- basic ----------------------------------------------------------------------------------------------------------*/

#include "BASe/BAStypes.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SNDstruct.h"
#include "SNDconst.h"
#include "SND.h"
#include "SNDtrack.h"
#include "SNDspecific.h"
#include "SNDfx.h"
#include "SNDvolume.h"

/*$2- IOP/RPC --------------------------------------------------------------------------------------------------------*/

#include "IOP/RPC_Manager.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2- GSP menu -------------------------------------------------------------------------------------------------------*/

#define ID_MenuDebug		0x000
#define ID_MenuFx			0x100
#define ID_MenuFxEnable0	0x200
#define ID_MenuFxMode0		0x300
#define ID_MenuNoRPCVol		0x400
#define ID_MenuNoPlay		0x800
#define ID_MenuNoRq		    0x900
#define ID_MenuNoVol		0xA00
#define ID_MenuNoFreq		0xB00
#define ID_MenuFxEnable1	0xC00
#define ID_MenuFxMode1		0xD00

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef enum	tden_Menu_
{

	/*$1- menu debug ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	e_DbgNodo			= ID_MenuDebug + 0,
	e_DbgHardBuffer		= ID_MenuDebug + 1,
	e_DbgFileBuffer		= ID_MenuDebug + 2,
	e_DbgSoundBuffer	= ID_MenuDebug + 3,
	e_DbgAll			= ID_MenuDebug + 4,
	e_DbgRPC			= ID_MenuDebug + 5,
	e_DbgTest			= ID_MenuDebug + 6,

	/*$1- menu fx ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	e_FxEnable0			= ID_MenuFx + 0,
	e_FxMode0			= ID_MenuFx + 1,
	e_FxDelay0			= ID_MenuFx + 2,
	e_FxFeedback0		= ID_MenuFx + 3,
	e_FxWetPan0			= ID_MenuFx + 4,
	e_FxWetVol0			= ID_MenuFx + 5,
	//
	e_FxEnable1			= ID_MenuFx + 6,
	e_FxMode1			= ID_MenuFx + 7,
	e_FxDelay1			= ID_MenuFx + 8,
	e_FxFeedback1		= ID_MenuFx + 9,
	e_FxWetPan1			= ID_MenuFx + 10,
	e_FxWetVol1			= ID_MenuFx + 11,

	/*$1- sub menu fx/enable0 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	e_FxEnable0_Off		= ID_MenuFxEnable0 + 0,
	e_FxEnable0_On		= ID_MenuFxEnable0 + 1,
	//
	e_FxEnable1_Off		= ID_MenuFxEnable1 + 0,
	e_FxEnable1_On		= ID_MenuFxEnable1 + 1,

	/*$1- submenu fx/mode0 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	e_FxMode0_Off		= ID_MenuFxMode0 + 0,
	e_FxMode0_Room		= ID_MenuFxMode0 + 1,
	e_FxMode0_StdA		= ID_MenuFxMode0 + 2,
	e_FxMode0_StdB		= ID_MenuFxMode0 + 3,
	e_FxMode0_StdC		= ID_MenuFxMode0 + 4,
	e_FxMode0_Hall		= ID_MenuFxMode0 + 5,
	e_FxMode0_Space		= ID_MenuFxMode0 + 6,
	e_FxMode0_Echo		= ID_MenuFxMode0 + 7,
	e_FxMode0_Delay		= ID_MenuFxMode0 + 8,
	e_FxMode0_Pipe		= ID_MenuFxMode0 + 9,
	e_FxMode0_Max		= ID_MenuFxMode0 + 10,
	//
	e_FxMode1_Off		= ID_MenuFxMode1 + 0,
	e_FxMode1_Room		= ID_MenuFxMode1 + 1,
	e_FxMode1_StdA		= ID_MenuFxMode1 + 2,
	e_FxMode1_StdB		= ID_MenuFxMode1 + 3,
	e_FxMode1_StdC		= ID_MenuFxMode1 + 4,
	e_FxMode1_Hall		= ID_MenuFxMode1 + 5,
	e_FxMode1_Space		= ID_MenuFxMode1 + 6,
	e_FxMode1_Echo		= ID_MenuFxMode1 + 7,
	e_FxMode1_Delay		= ID_MenuFxMode1 + 8,
	e_FxMode1_Pipe		= ID_MenuFxMode1 + 9,
	e_FxMode1_Max		= ID_MenuFxMode1 + 10,
	

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	e_NoPlay_Off		= ID_MenuNoPlay + 0,
	e_NoPlay_On			= ID_MenuNoPlay + 1,

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	e_NoRq_Off			= ID_MenuNoRq + 0,
	e_NoRq_On			= ID_MenuNoRq + 1,

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	e_NoVol_Off			= ID_MenuNoVol + 0,
	e_NoVol_On			= ID_MenuNoVol + 1,

/*$1- */
	e_NoRPCVol_Off			= ID_MenuNoRPCVol + 0,
	e_NoRPCVol_On			= ID_MenuNoRPCVol + 1,
    
	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	e_NoFreq_Off		= ID_MenuNoFreq + 0,
	e_NoFreq_On			= ID_MenuNoFreq + 1,

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

}
tden_Menu;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

int			SND_gai_CurrentDebugRq[] =
{
	ID_MenuDebug,
	ID_MenuFx,
	ID_MenuFxEnable0,
	ID_MenuFxMode0,
	ID_MenuNoRPCVol,
	ID_MenuNoPlay,
	ID_MenuNoRq,
	ID_MenuNoVol,
	ID_MenuNoFreq
};

int ps2SND_i_FxDelay=0;
int ps2SND_f_FxWetVol=0.0f;
int ps2SND_i_FxFeedback=0;
int ps2SND_i_FxWetPan=0;
/*$4
 ***********************************************************************************************************************
    external prototypes
 ***********************************************************************************************************************
 */

extern void GSP_DrawGlobalRasters(char *Text, float FrameNumb, float YPos, float ARS, u_int TheColor);
extern void    ps2SND_FxFlushSettings(void);

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_DebugSetRequest(int _i_rq)
{
    int data;
	switch((tden_Menu) _i_rq)
	{

	/*$1- menu debug ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case e_DbgNodo:
		SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] = _i_rq;
		break;

	case e_DbgHardBuffer:
		if(SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] != _i_rq)
		{
		    data = 4;
			eeRPC_i_PushCommand(RPC_Cmd_SndDebugInfos, &data, sizeof(int), NULL, 0);
			SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] = e_DbgNodo;
		}
		break;

	case e_DbgFileBuffer:
		if(SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] != _i_rq)
		{
		    data = 1;
			eeRPC_i_PushCommand(RPC_Cmd_SndDebugInfos, &data, sizeof(int), NULL, 0);
			SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] = e_DbgNodo;
		}
		break;

	case e_DbgSoundBuffer:
		if(SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] != _i_rq)
		{
		    data = 2;
			eeRPC_i_PushCommand(RPC_Cmd_SndDebugInfos, &data, sizeof(int), NULL, 0);
			SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] = e_DbgNodo;
		}
		break;

	case e_DbgAll:
		if(SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] != _i_rq)
		{
		    data = 7;
			eeRPC_i_PushCommand(RPC_Cmd_SndDebugInfos, &data, sizeof(int), NULL, 0);
			SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] = e_DbgNodo;
		}
		break;

	case e_DbgRPC:
		SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] = _i_rq;
		break;

	case e_DbgTest:
		{
			if(SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] != _i_rq)
			{
				SND_gai_CurrentDebugRq[ID_MenuDebug >> 8] = _i_rq;
			}
		}
		break;

	/*$1- menu fx ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case e_FxEnable0:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	case e_FxMode0:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	case e_FxDelay0:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	case e_FxFeedback0:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	case e_FxWetPan0:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	case e_FxWetVol0:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	//
	case e_FxEnable1:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	case e_FxMode1:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	case e_FxDelay1:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	case e_FxFeedback1:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	case e_FxWetPan1:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;
	case e_FxWetVol1:
		SND_gai_CurrentDebugRq[ID_MenuFx >> 8] = _i_rq;
		break;

	/*$1- sub menu fx/enable0 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case e_FxEnable0_Off:
	    if(SND_gai_CurrentDebugRq[ID_MenuFxEnable0 >> 8] != e_FxEnable0_Off)
	    {
		    ps2SND_i_FxDisable(SND_Cte_FxCoreA);
		    SND_gai_CurrentDebugRq[ID_MenuFxEnable0 >> 8] = _i_rq;
		}
		break;
	case e_FxEnable0_On:
	    if(SND_gai_CurrentDebugRq[ID_MenuFxEnable0 >> 8] != e_FxEnable0_On)
	    {
    		ps2SND_i_FxEnable(SND_Cte_FxCoreA);
    		SND_gai_CurrentDebugRq[ID_MenuFxEnable0 >> 8] = _i_rq;
    	}
		break;
		//
	case e_FxEnable1_Off:
	    if(SND_gai_CurrentDebugRq[ID_MenuFxEnable1 >> 8] != e_FxEnable0_Off)
	    {
		    ps2SND_i_FxDisable(SND_Cte_FxCoreB);
		    SND_gai_CurrentDebugRq[ID_MenuFxEnable1 >> 8] = _i_rq;
		}
		break;
	case e_FxEnable1_On:
	    if(SND_gai_CurrentDebugRq[ID_MenuFxEnable1 >> 8] != e_FxEnable0_On)
	    {
    		ps2SND_i_FxEnable(SND_Cte_FxCoreB);
    		SND_gai_CurrentDebugRq[ID_MenuFxEnable1 >> 8] = _i_rq;
    	}
		break;

	/*$1- submenu fx/mode0 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case e_FxMode0_Off:
	    ps2SND_i_FxSetMode(SND_Cte_FxCoreA, SND_Cte_FxMode_Off);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreA);
		SND_gai_CurrentDebugRq[ID_MenuFxMode0 >> 8] = _i_rq;
		break;
	case e_FxMode0_Room:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreA, SND_Cte_FxMode_Room);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreA);
		SND_gai_CurrentDebugRq[ID_MenuFxMode0 >> 8] = _i_rq;
		break;
	case e_FxMode0_StdA:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreA, SND_Cte_FxMode_StudioA);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreA);
		SND_gai_CurrentDebugRq[ID_MenuFxMode0 >> 8] = _i_rq;
		break;
	case e_FxMode0_StdB:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreA, SND_Cte_FxMode_StudioB);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreA);
		SND_gai_CurrentDebugRq[ID_MenuFxMode0 >> 8] = _i_rq;
		break;
	case e_FxMode0_StdC:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreA, SND_Cte_FxMode_StudioC);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreA);
		SND_gai_CurrentDebugRq[ID_MenuFxMode0 >> 8] = _i_rq;
		break;
	case e_FxMode0_Hall:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreA, SND_Cte_FxMode_Hall);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreA);
		SND_gai_CurrentDebugRq[ID_MenuFxMode0 >> 8] = _i_rq;
		break;
	case e_FxMode0_Space:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreA, SND_Cte_FxMode_Space);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreA);
		SND_gai_CurrentDebugRq[ID_MenuFxMode0 >> 8] = _i_rq;
		break;
	case e_FxMode0_Echo:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreA, SND_Cte_FxMode_Echo);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreA);
		SND_gai_CurrentDebugRq[ID_MenuFxMode0 >> 8] = _i_rq;
		break;
	case e_FxMode0_Delay:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreA, SND_Cte_FxMode_Delay);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreA);
		SND_gai_CurrentDebugRq[ID_MenuFxMode0 >> 8] = _i_rq;
		break;
	case e_FxMode0_Pipe:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreA, SND_Cte_FxMode_Pipe);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreA);
		SND_gai_CurrentDebugRq[ID_MenuFxMode0 >> 8] = _i_rq;
		break;
	///
	case e_FxMode1_Off:
	    ps2SND_i_FxSetMode(SND_Cte_FxCoreB, SND_Cte_FxMode_Off);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreB);
		SND_gai_CurrentDebugRq[ID_MenuFxMode1 >> 8] = _i_rq;
		break;
	case e_FxMode1_Room:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreB, SND_Cte_FxMode_Room);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreB);
		SND_gai_CurrentDebugRq[ID_MenuFxMode1 >> 8] = _i_rq;
		break;
	case e_FxMode1_StdA:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreB, SND_Cte_FxMode_StudioA);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreB);
		SND_gai_CurrentDebugRq[ID_MenuFxMode1 >> 8] = _i_rq;
		break;
	case e_FxMode1_StdB:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreB, SND_Cte_FxMode_StudioB);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreB);
		SND_gai_CurrentDebugRq[ID_MenuFxMode1 >> 8] = _i_rq;
		break;
	case e_FxMode1_StdC:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreB, SND_Cte_FxMode_StudioC);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreB);
		SND_gai_CurrentDebugRq[ID_MenuFxMode1 >> 8] = _i_rq;
		break;
	case e_FxMode1_Hall:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreB, SND_Cte_FxMode_Hall);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreB);
		SND_gai_CurrentDebugRq[ID_MenuFxMode1 >> 8] = _i_rq;
		break;
	case e_FxMode1_Space:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreB, SND_Cte_FxMode_Space);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreB);
		SND_gai_CurrentDebugRq[ID_MenuFxMode1 >> 8] = _i_rq;
		break;
	case e_FxMode1_Echo:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreB, SND_Cte_FxMode_Echo);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreB);
		SND_gai_CurrentDebugRq[ID_MenuFxMode1 >> 8] = _i_rq;
		break;
	case e_FxMode1_Delay:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreB, SND_Cte_FxMode_Delay);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreB);
		SND_gai_CurrentDebugRq[ID_MenuFxMode1 >> 8] = _i_rq;
		break;
	case e_FxMode1_Pipe:
		ps2SND_i_FxSetMode(SND_Cte_FxCoreB, SND_Cte_FxMode_Pipe);
	    ps2SND_i_FxEnable(SND_Cte_FxCoreB);
		SND_gai_CurrentDebugRq[ID_MenuFxMode1 >> 8] = _i_rq;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case e_NoPlay_Off:
	    SND_gst_Params.ul_Flags &= ~SND_Cte_ForceNoPlay;
		SND_gai_CurrentDebugRq[ID_MenuNoPlay >> 8] = _i_rq;
		break;
	case e_NoPlay_On:
	    SND_gst_Params.ul_Flags |= SND_Cte_ForceNoPlay;
		SND_gai_CurrentDebugRq[ID_MenuNoPlay >> 8] = _i_rq;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case e_NoRq_Off:
	    SND_gst_Params.ul_Flags &= ~SND_Cte_ForceNoRq;
		SND_gai_CurrentDebugRq[ID_MenuNoRq >> 8] = _i_rq;
		break;
	case e_NoRq_On:
	    SND_gst_Params.ul_Flags |= SND_Cte_ForceNoRq;
		SND_gai_CurrentDebugRq[ID_MenuNoRq >> 8] = _i_rq;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case e_NoVol_Off:
	    SND_gst_Params.ul_Flags &= ~SND_Cte_ForceNoFloatVol;
		SND_gai_CurrentDebugRq[ID_MenuNoVol >> 8] = _i_rq;
		break;
	case e_NoVol_On:
	    SND_gst_Params.ul_Flags |= SND_Cte_ForceNoFloatVol;
		SND_gai_CurrentDebugRq[ID_MenuNoVol >> 8] = _i_rq;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case e_NoRPCVol_Off:
	    SND_gst_Params.ul_Flags &= ~SND_Cte_ForceNoRqVol;
		SND_gai_CurrentDebugRq[ID_MenuNoRPCVol >> 8] = _i_rq;
		break;
	case e_NoRPCVol_On:
	    SND_gst_Params.ul_Flags |= SND_Cte_ForceNoRqVol;
		SND_gai_CurrentDebugRq[ID_MenuNoRPCVol >> 8] = _i_rq;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case e_NoFreq_Off:
	    SND_gst_Params.ul_Flags &= ~SND_Cte_ForceNoFreq;
		SND_gai_CurrentDebugRq[ID_MenuNoFreq >> 8] = _i_rq;
		break;
	case e_NoFreq_On:
	    SND_gst_Params.ul_Flags |= SND_Cte_ForceNoFreq;
		SND_gai_CurrentDebugRq[ID_MenuNoFreq >> 8] = _i_rq;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	default:
		break;
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_DebugGetStatus(int _i_MenuId)
{
	/*~~~~~~~~~*/
	int		code;
	extern int ps2SND_i_FxGetMode(int);
	/*~~~~~~~~~*/

	switch(_i_MenuId)
	{
	case ID_MenuFxMode0:
	    code = ps2SND_i_FxGetMode(SND_Cte_FxCoreA);
	    SND_gai_CurrentDebugRq[_i_MenuId >> 8] = code + _i_MenuId;
	    break;
	
	case ID_MenuFxEnable0:
		code = ps2SND_i_FxGetMode(SND_Cte_FxCoreA);
        if(code == SND_Cte_FxMode_Off)
            code = 0;
        else 
            code = 1;
        SND_gai_CurrentDebugRq[_i_MenuId >> 8] = code + _i_MenuId;
        break;

	case ID_MenuFxMode1:
	    code = ps2SND_i_FxGetMode(SND_Cte_FxCoreB);
	    SND_gai_CurrentDebugRq[_i_MenuId >> 8] = code + _i_MenuId;
	    break;
	
	case ID_MenuFxEnable1:
		code = ps2SND_i_FxGetMode(SND_Cte_FxCoreB);
        if(code == SND_Cte_FxMode_Off)
            code = 0;
        else 
            code = 1;
        SND_gai_CurrentDebugRq[_i_MenuId >> 8] = code + _i_MenuId;
        break;

	case ID_MenuNoVol:
	     if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoFloatVol) return 1;
	     return 0;	
	case ID_MenuNoRPCVol:
	     if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoRqVol) return 1;
	     return 0;	
	case ID_MenuNoPlay:
	     if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoPlay) return 1;
	     return 0;
	case ID_MenuNoRq:
	     if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoRq) return 1;
	     return 0;
	case ID_MenuNoFreq:
	     if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoFreq) return 1;
	     return 0;

	case ID_MenuDebug:
	case ID_MenuFx:
		code = SND_gai_CurrentDebugRq[_i_MenuId >> 8] - _i_MenuId;
		break;
	default:
		code = 0;
		break;
	}

	switch((tden_Menu) SND_gai_CurrentDebugRq[ID_MenuDebug >> 8])
	{
	case e_DbgTest:
		break;
	default:
		break;
	}

	return code;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ResetForceNoSound(void)
{
	if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoSound)
	{
		SND_gst_Params.ul_Flags &= ~SND_Cte_ForceNoSound;
		SND_gst_Params.l_Available = 1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ForceNo(int _i_mask)
{
	if(!SND_gst_Params.l_Available) return;
	SND_gst_Params.ul_Flags |= _i_mask;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ResetForceNo(int _i_mask)
{
	if(!SND_gst_Params.l_Available) return;
	SND_gst_Params.ul_Flags &= ~_i_mask;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ForceNoSound(void)
{
	if(!SND_gst_Params.l_Available) return;

	SND_StopAll(0);
	SND_gst_Params.ul_Flags |= SND_Cte_ForceNoSound;
	SND_gst_Params.l_Available = 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* _FINAL_ */
#endif /* PSX2_TARGET */

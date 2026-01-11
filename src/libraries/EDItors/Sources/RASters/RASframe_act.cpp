/*$T RASframe_act.cpp GC!1.41 08/04/99 10:28:29 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "RASframe.h"
#include "RASframe_act.h"

#ifndef PSX2_TARGET
#include "TIMer/TIMdefs.h"
#endif

/*
 ===================================================================================================
 ===================================================================================================
 */
void ERAS_cl_Frame::OnAction(ULONG _ul_Action)
{
    switch(_ul_Action)
    {
	case ERAS_ACTION_AUTO:
		mst_Ini.ul_MonitorFreq = 0;
		break;
    case ERAS_ACTION_60HZ:
        mst_Ini.ul_MonitorFreq = 60;
        break;
    case ERAS_ACTION_65HZ:
        mst_Ini.ul_MonitorFreq = 65;
        break;
    case ERAS_ACTION_70HZ:
        mst_Ini.ul_MonitorFreq = 70;
        break;
    case ERAS_ACTION_75HZ:
        mst_Ini.ul_MonitorFreq = 75;
        break;
    case ERAS_ACTION_80HZ:
        mst_Ini.ul_MonitorFreq = 80;
        break;
    case ERAS_ACTION_85HZ:
        mst_Ini.ul_MonitorFreq = 85;
        break;
    case ERAS_ACTION_90HZ:
        mst_Ini.ul_MonitorFreq = 90;
        break;
    case ERAS_ACTION_95HZ:
        mst_Ini.ul_MonitorFreq = 95;
        break;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
UINT ERAS_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    UINT    ui_State;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_ul_Action)
    {
    case ERAS_ACTION_AUTO:
        ui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.ul_MonitorFreq == 0)
            ui_State |= DFCS_CHECKED;
        break;
    case ERAS_ACTION_60HZ:
        ui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.ul_MonitorFreq == 60)
            ui_State |= DFCS_CHECKED;
        break;
    case ERAS_ACTION_65HZ:
        ui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.ul_MonitorFreq == 65)
            ui_State |= DFCS_CHECKED;
        break;
    case ERAS_ACTION_70HZ:
        ui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.ul_MonitorFreq == 70)
            ui_State |= DFCS_CHECKED;
        break;
    case ERAS_ACTION_75HZ:
        ui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.ul_MonitorFreq == 75)
            ui_State |= DFCS_CHECKED;
        break;
    case ERAS_ACTION_80HZ:
        ui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.ul_MonitorFreq == 80)
            ui_State |= DFCS_CHECKED;
        break;
    case ERAS_ACTION_85HZ:
        ui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.ul_MonitorFreq == 85)
            ui_State |= DFCS_CHECKED;
        break;
    case ERAS_ACTION_90HZ:
        ui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.ul_MonitorFreq == 90)
            ui_State |= DFCS_CHECKED;
        break;
    case ERAS_ACTION_95HZ:
        ui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.ul_MonitorFreq == 95)
            ui_State |= DFCS_CHECKED;
        break;
    }

    return ui_State;
}

#endif

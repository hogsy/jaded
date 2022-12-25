/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "LOGframe.h"
#include "LOGframe_act.h"
#include "EDImainframe.h"
#include "Res/Res.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKmsg.h"

#ifdef JADEFUSION
extern ULONG LOA_ul_FileTypeSize[40];
extern void LINK_PrintStatusMsgEOL(char *);
extern float TIM_gf_SynchroFrequency;
extern BOOL LOA_bLogLoad;
#else // JADEFUSION
extern "C" ULONG LOA_ul_FileTypeSize[40];
extern "C" void LINK_PrintStatusMsgEOL(char *);
extern "C" float TIM_gf_SynchroFrequency;
extern "C" BOOL LOA_bLogLoad;
#endif // JADEFUSION


/*
 ===================================================================================================
 ===================================================================================================
 */
void ELOG_cl_Frame::OnAction(ULONG _ul_Action)
{
	char az[1024];
    switch(_ul_Action)
    {
    case ELOG_ACTION_CLEARLOG:
        mpo_Edit->SetWindowText("");
        mi_StringIndex = 0;
        mb_IsBufferFull = FALSE;
        break;
	case ELOG_ACTION_FILTER:
		OnFilter();
		break;
	case ELOG_ACTION_FREEZE:
		mst_Ini.b_Freeze = mst_Ini.b_Freeze ? FALSE : TRUE;
        ERR_gb_WarningBox = !mst_Ini.b_Freeze;
		break;
	case ELOG_ACTION_WRITETOFILE:
		mst_Ini.b_LogToFile = mst_Ini.b_LogToFile ? FALSE : TRUE;
		break;
	case ELOG_ACTION_LOGLOAD:
		LOA_bLogLoad = LOA_bLogLoad ? FALSE : TRUE;
		break;

	case ELOG_ACTION_RASTERS:
		LogRasters("AI Instances");
		LogRasters("AI Functions");
		LogRasters("AI Procedures");
		LogRasters("AI System");
		break;

	case ELOG_ACTION_MEMLOAD:
		LINK_PrintStatusMsg("========= Files =============");
		sprintf(az, ".gro %d", LOA_ul_FileTypeSize[0]);
		LINK_PrintStatusMsg(az);
		sprintf(az, ".grm %d", LOA_ul_FileTypeSize[1]);
		LINK_PrintStatusMsg(az);
		sprintf(az, ".gao %d", LOA_ul_FileTypeSize[2]);
		LINK_PrintStatusMsg(az);
		sprintf(az, ".trl %d", LOA_ul_FileTypeSize[3]);
		LINK_PrintStatusMsg(az);
		sprintf(az, ".ofc, .omd %d", LOA_ul_FileTypeSize[4]);
		LINK_PrintStatusMsg(az);
		sprintf(az, ".cob, .map, .cin %d", LOA_ul_FileTypeSize[5]);
		LINK_PrintStatusMsg(az);
		sprintf(az, ".act %d", LOA_ul_FileTypeSize[6]);
		LINK_PrintStatusMsg(az);
		sprintf(az, ".ova %d", LOA_ul_FileTypeSize[7]);
		LINK_PrintStatusMsg(az);
		LINK_PrintStatusMsg("========= GEO =============");
		sprintf(az, "GEO Vertex %d ko", LOA_ul_FileTypeSize[20] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GEO Normals %d ko", LOA_ul_FileTypeSize[21] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GEO Global RLI %d ko", LOA_ul_FileTypeSize[22] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GEO UVs %d ko", LOA_ul_FileTypeSize[23] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GEO Triangles %d ko", LOA_ul_FileTypeSize[24] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GEO Used Index %d ko", LOA_ul_FileTypeSize[25] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GEO Skin %d ko", LOA_ul_FileTypeSize[26] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GEO Local RLI %d ko", LOA_ul_FileTypeSize[27] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GEO Misc %d ko", LOA_ul_FileTypeSize[28] / 1024);
		LINK_PrintStatusMsg(az);
		LINK_PrintStatusMsg("========= TRL =============");
		sprintf(az, "Events Struct %d ko", LOA_ul_FileTypeSize[30] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "Datas %d ko", LOA_ul_FileTypeSize[31] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "ListTracks struct %d ko", LOA_ul_FileTypeSize[32] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "Tracks struct %d ko", LOA_ul_FileTypeSize[33] / 1024);
		LINK_PrintStatusMsg(az);
		LINK_PrintStatusMsg("========= Misc =============");
		sprintf(az, "AI Vars %d ko", LOA_ul_FileTypeSize[35] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "AI Misc Struct %d ko", LOA_ul_FileTypeSize[36] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GAO Misc %d ko", LOA_ul_FileTypeSize[37] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GAO Gizmo %d ko", LOA_ul_FileTypeSize[34] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "COL %d ko", LOA_ul_FileTypeSize[38] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "Misc Engine %d ko", LOA_ul_FileTypeSize[39] / 1024);
		LINK_PrintStatusMsg(az);
		sprintf(az, "GAO Matrix %d ko", LOA_ul_FileTypeSize[29] / 1024);
		LINK_PrintStatusMsg(az);
		L_memset(LOA_ul_FileTypeSize, 0, sizeof(LOA_ul_FileTypeSize));
		break;

    case ELOG_ACTION_CANAL0:
    case ELOG_ACTION_CANAL1:
    case ELOG_ACTION_CANAL2:
    case ELOG_ACTION_CANAL3:
    case ELOG_ACTION_CANAL4:
    case ELOG_ACTION_CANAL5:
        {
            // Which canal changes ?
            int iCanal = 1<<(_ul_Action - ELOG_ACTION_CANAL0);
            if (iCanal & mui_ActiveCanal)
                mui_ActiveCanal &= ~iCanal;
            else
                mui_ActiveCanal |= iCanal;
            
            // Clear log window
            mpo_Edit->SetWindowText("");

            // Fill log window with active canals
            int i,j;
            int iLoopNb = (mb_IsBufferFull ? 2 : 1);
            for (j=0; j<iLoopNb ; j++)
            {
                int iMin,iMax;
                if (mb_IsBufferFull && (j == 0))
                {
                    iMin = mi_StringIndex;
                    iMax = ELOG_MaxStringNb;
                }
                else
                {
                    iMin = 0;
                    iMax = mi_StringIndex;
                }

                for (i=iMin; i<iMax; i++)
                {
                    ELOGString *pString = ma_Strings+i;
                    if (1<<pString->mi_Canal & mui_ActiveCanal)
                        AddText(pString->ms_String.c_str(),FALSE,pString->mx_Color,pString->mi_Canal);
                }
            }
        }

        break;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
UINT ELOG_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    UINT    ui_State;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ui_State = (UINT) - 1;

    switch(_ul_Action)
    {
    case ELOG_ACTION_FREEZE:
        ui_State = DFCS_BUTTONCHECK;
        if(mst_Ini.b_Freeze)
            ui_State |= DFCS_CHECKED;
        break;
    case ELOG_ACTION_WRITETOFILE:
        ui_State = DFCS_BUTTONCHECK;
        if(mst_Ini.b_LogToFile)
            ui_State |= DFCS_CHECKED;
        break;
    case ELOG_ACTION_LOGLOAD:
        ui_State = DFCS_BUTTONCHECK;
        if(LOA_bLogLoad)
            ui_State |= DFCS_CHECKED;
        break;

    case ELOG_ACTION_CANAL0:
    case ELOG_ACTION_CANAL1:
    case ELOG_ACTION_CANAL2:
    case ELOG_ACTION_CANAL3:
    case ELOG_ACTION_CANAL4:
    case ELOG_ACTION_CANAL5:
        // Display active canals
        ui_State = DFCS_BUTTONCHECK;
        if (mui_ActiveCanal & (1<<(_ul_Action-ELOG_ACTION_CANAL0)))
            ui_State |= DFCS_CHECKED;
        break;
	}

	return ui_State;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL ELOG_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL ELOG_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	if(GetFocus() != mpo_Edit) return FALSE;
	return M_MF()->b_EditKey(_uw_Key);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void ELOG_cl_Frame::OnToolBarCommand(UINT nID)
{
    switch(nID)
    {
    case ELOG_ID_CLEAR:
        OnAction(ELOG_ACTION_CLEARLOG);
        break;
    case ELOG_ID_FILTER:
		OnAction(ELOG_ACTION_FILTER);
        break;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void ELOG_cl_Frame::LogRasters(char *categ)
{
	PRO_tdst_TrameRaster			*pras;
	PRO_tdst_TrameRaster			*ptras;
	CList<PRO_tdst_TrameRaster *>	mo_AllRas;
	POSITION						pos;
	char							az[150];
	float							f1, f2;

	LINK_PrintStatusMsg(categ);
	LINK_PrintStatusMsg("---------------------------------------");
	LINK_PrintStatusMsg("");

	/* Tri */
	pras = _PRO_gpst_FirstTrameRaster;
	while(pras)
	{
		if(!strcmpi(pras->psz_SubCategory, categ))
		{
			pos = mo_AllRas.GetHeadPosition();
			while(pos)
			{
				ptras = mo_AllRas.GetAt(pos);

				f1 = ptras->f_timeresframe;
				f2 = pras->f_timeresframe;
				if(f1 < f2) break;

				mo_AllRas.GetNext(pos);
			}

			if(!pos)
				mo_AllRas.AddTail(pras);
			else
				mo_AllRas.InsertBefore(pos, pras);
		}

		pras = pras->pst_NextRaster;
		if(pras == _PRO_gpst_FirstTrameRaster) break;
	}

	/* Affichage */
	pos = mo_AllRas.GetHeadPosition();
	while(pos)
	{
		ptras = mo_AllRas.GetAt(pos);
		if(ptras->c_timetot)
		{
			LINK_PrintStatusMsgEOL(ptras->psz_Name);
			LINK_PrintStatusMsgEOL("  =>  ");

			sprintf(az, "PerFrame: %.3f   Moy: %.3f   Hit: %d   Max: %.3f", ptras->f_timeresframe, ptras->f_timetot / ptras->c_timetot, ptras->c_timetot, ptras->f_Max);
			LINK_PrintStatusMsgEOL(az);
			ptras->f_timetot = 0;
			ptras->f_timetotframe = 0;
			ptras->c_timetot = 0;
			ptras->f_Max = 0;

			LINK_PrintStatusMsgEOL("\\n");
		}

		mo_AllRas.GetNext(pos);
	}

	LINK_PrintStatusMsg("");
	LINK_PrintStatusMsg("");
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void ELOG_cl_Frame::OnToolBarCommandUI(UINT nID, CCmdUI *pUI)
{
    switch(nID)
    {
    case ELOG_ID_CLEAR:
        if(!b_OnActionValidate(ELOG_ACTION_CLEARLOG))
        {
            pUI->Enable(FALSE);
            return;
        }
        break;

    case ELOG_ID_FILTER:
		pUI->SetCheck(mao_Filter[0].IsEmpty() ? 0 : 1);
		if(!b_OnActionValidate(ELOG_ACTION_FILTER))
        {
            pUI->Enable(FALSE);
            return;
        }
		break;
    }

    pUI->Enable(TRUE);
}

#endif


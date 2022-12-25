/*$T EDImainframe_universe.cpp GC!1.71 01/19/00 11:21:31 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BIGfiles/BIGfat.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/WORld/WORvars.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "AIinterp/Sources/AIdebug.h"
#include "AIinterp/Sources/AIengine.h"
#include "ENGine/Sources/GRId/GRI_save.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/GRId/GRI_display.h"
#include "ENGine/Sources/GRId/GRI_struct.h"

#ifdef JADEFUSION
extern void SOFT_ZList_Clear(void);
extern BOOL GRI_gb_Modified;
extern int GRI_UpdateCompress(GRID_tdst_World *);
extern void MSG_GlobalReinit(void);
#else
extern "C" void SOFT_ZList_Clear(void);
extern "C" BOOL GRI_gb_Modified;
extern "C" int GRI_UpdateCompress(GRID_tdst_World *);
extern "C" void MSG_GlobalReinit(void);
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::Universe_Close(void)
{
    WOR_Universe_Close(0);
    SendMessageToEditors(EDI_MESSAGE_REFRESH, 0, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::Universe_SetAIKey(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDIA_cl_NameDialog  cl_GetKey("Enter Universe IA Key", 11);
    char                sz_Key[10];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sprintf(sz_Key, "%X", BIG_UniverseKey());
    cl_GetKey.mo_Name = sz_Key;

    if(cl_GetKey.DoModal() == IDOK)
    {
        WOR_Universe_Close(0);
        sscanf(cl_GetKey.mo_Name, "%x", &BIG_UniverseKey());
        BIG_WriteHeader();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::Universe_ReinitEngine(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    WOR_tdst_World  *pst_World;
    TAB_tdst_PFelem *pst_CurrentElem;
    TAB_tdst_PFelem *pst_EndElem;
	BOOL			b_Grid;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Grid */
	b_Grid = FALSE;
	if(GRI_gb_Modified)
	{
		if(MessageBox("Grid(s) has been changed. Do you want to update them ?", "Please confirm", MB_YESNO) == IDYES)
			b_Grid = TRUE;
		GRI_gb_Modified = FALSE;
	}

	SOFT_ZList_Clear();
	MSG_GlobalReinit();
	AI_ReinitUniverse();
    pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
    pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
    for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
    {
        pst_World = (WOR_tdst_World *) pst_CurrentElem->p_Pointer;
        if(!TAB_b_IsAHole(pst_World))
		{
			if(b_Grid)
			{
				if(pst_World->pst_Grid) GRI_UpdateCompress(pst_World->pst_Grid);
				if(pst_World->pst_Grid1) GRI_UpdateCompress(pst_World->pst_Grid1);
			}
            ENG_ReinitOneWorld(pst_World, UNI_Cuc_TotalInit);
		}
    }

    /* Reset AI break */
    AI_ReinitBreakMode();

    LINK_UpdatePointers();
    LINK_PrintStatusMsg("Engine Reset");
	LINK_Refresh();
}

#endif

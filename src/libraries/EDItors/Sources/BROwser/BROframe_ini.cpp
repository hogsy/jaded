/*$T BROframe_ini.cpp GC!1.71 01/21/00 12:20:11 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGfat.h"
#include "BROframe.h"
#include "BROtreectrl.h"
#include "BROlistctrl.h"
#include "BROgrpctrl.h"
#include "EDIpaths.h"
#include "EDIapp.h"

extern BOOL EDI_gb_CheckInDel;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::ReinitIni(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int i;
    int ai_InitColsListCtrl[EBRO_C_ListCtrlMaxColumns] = { 100, 50, 50, 65, 65, 60, 60, 60, 60};
    int ai_InitColsGrpCtrl[EBRO_C_GrpCtrlMaxColumns] = { 100, 50, 50, 100 };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    EDI_cl_BaseFrame::ReinitIni();

    mst_Ini.i_NumFavorites = 0;
    L_memset(mst_Ini.ast_Favorites, 0, EBRO_C_MaxFavorites * sizeof(EBRO_tdst_Favorite));

    mst_Ini.i_HeightFirstPane = 400;
    mst_Ini.i_HeightSecondPane = 100;

    for(i = 0; i < EBRO_C_ListCtrlMaxColumns; i++)
    {
        mst_Ini.ai_WidthListCol[i] = ai_InitColsListCtrl[i];
        mst_Ini.ai_NumColListCtrl[i] = i;
    }

    for(i = 0; i < EBRO_C_GrpCtrlMaxColumns; i++)
    {
        mst_Ini.ai_WidthGrpCol[i] = ai_InitColsGrpCtrl[i];
        mst_Ini.ai_NumColGrpCtrl[i] = i;
    }

    mst_Ini.i_SortColumnListCtrl = 0;
    mst_Ini.i_SortColumnGrpCtrl = 0;

    mst_Ini.b_DUMMY = FALSE;
    mst_Ini.b_EngineMode = FALSE;
    mst_Ini.i_Filter = 0;

    mst_Ini.i_ListDispMode = EBRO_C_ListModeReport;
    mst_Ini.i_GrpDispMode = EBRO_C_ListModeReport;
    mst_Ini.i_GroupAutoHide = TRUE;

    mpo_ListCtrl->mi_SortColumn = 0;
    mpo_GrpCtrl->mi_SortColumn = 0;

    DeleteLinkedPath();

    /* Options */
    mst_Ini.mst_Options.b_RecImp = TRUE;
    mst_Ini.mst_Options.b_RecExp = TRUE;
    mst_Ini.mst_Options.b_RecIn = TRUE;
    mst_Ini.mst_Options.b_RecOut = TRUE;
    mst_Ini.mst_Options.b_RecUndoOut = TRUE;
    mst_Ini.mst_Options.b_RecGet = TRUE;
    mst_Ini.mst_Options.b_AutoOn = FALSE;
    mst_Ini.mst_Options.i_AutoFreq = 60;
    mst_Ini.mst_Options.b_AutoRecImp = TRUE;
    mst_Ini.mst_Options.b_AutoRecExp = TRUE;
    mst_Ini.mst_Options.b_LinkRefresh = TRUE;
	mst_Ini.mst_Options.b_MirrorGetLatest = FALSE;

    /* Mad import options */
    mst_Ini.mst_MadOptions.b_AutoImp = TRUE;
    mst_Ini.mst_MadOptions.b_ForceImp = FALSE;
    mst_Ini.mst_MadOptions.c_MaterialUpdate = 1;
    mst_Ini.mst_MadOptions.c_MaterialMerge = 0;
    mst_Ini.mst_MadOptions.c_MaterialNoSingle = 8;
    mst_Ini.mst_MadOptions.c_GraphicObjectUpdate = 1;
    mst_Ini.mst_MadOptions.c_GraphicObjectMerge = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::LoadIni(void)
{
_Try_
    EDI_cl_BaseFrame::BaseLoadIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
    TreatIni();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::TreatIni(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int         i, i_Res;
    CComboBox   *po_Combo;
    char        asz_Name[BIG_C_MaxLenName];
    ULONG       *pul_Buf, ul_Size;
    BIG_INDEX   ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Normal ini file
     -------------------------------------------------------------------------------------------------------------------
     */

    switch(mst_Ini.i_ListDispMode)
    {
    case EBRO_C_ListModeIcon:
        mpo_ListCtrl->ModifyStyle(LVS_REPORT | LVS_LIST, LVS_ICON);
        break;
    case EBRO_C_ListModeSmallIcon:
        mpo_ListCtrl->ModifyStyle(LVS_REPORT | LVS_ICON, LVS_LIST);
        break;
    case EBRO_C_ListModeReport:
        mpo_ListCtrl->ModifyStyle(LVS_LIST | LVS_ICON, LVS_REPORT);
        break;
    }

    switch(mst_Ini.i_GrpDispMode)
    {
    case EBRO_C_ListModeIcon:
        mpo_GrpCtrl->ModifyStyle(LVS_REPORT | LVS_LIST, LVS_ICON);
        break;
    case EBRO_C_ListModeSmallIcon:
        mpo_GrpCtrl->ModifyStyle(LVS_REPORT | LVS_ICON, LVS_LIST);
        break;
    case EBRO_C_ListModeReport:
        mpo_GrpCtrl->ModifyStyle(LVS_LIST | LVS_ICON, LVS_REPORT);
        break;
    }

    /* Inform list ctrl about sort column */
    mpo_ListCtrl->mi_SortColumn = mst_Ini.i_SortColumnListCtrl;
    mpo_GrpCtrl->mi_SortColumn = mst_Ini.i_SortColumnGrpCtrl;

    /* Add favorites */
    po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(BROWSER_IDC_COMBOFAVORITES);
    po_Combo->ResetContent();
    for(i = 0; i < mst_Ini.i_NumFavorites; i++)
    {
        if(mst_Ini.ast_Favorites[i].asz_DisplayName[0])
        {
            i_Res = po_Combo->AddString(mst_Ini.ast_Favorites[i].asz_DisplayName);
            po_Combo->SetItemData(i_Res, i);
        }
    }

	/* Set check for Refresh of ListCtrl with P4 */
	//mpo_DialogBar->CheckDlgButton(BROWSER_IDC_P4REFRESHLISTCTRL, mb_P4RefreshListCtrl);

    /* Size of pane */
    mpo_InsideSplitter->SetRowInfo(0, mst_Ini.i_HeightFirstPane, 20);
    mpo_InsideSplitter->RecalcLayout();
    mpo_InsideLstSplitter->SetRowInfo(0, mst_Ini.i_HeightSecondPane, 20);
    mpo_InsideLstSplitter->RecalcLayout();

    /* Reset columns */
    ResetAllColumnsListCtrl();
    ResetAllColumnsGrpCtrl();

    /* Linked paths */
    ReadLinkedPath();

    /* Load expanded infos */
    L_strcpy(asz_Name, mst_Def.asz_Name);
    L_strcat(asz_Name, EDI_Csz_ExtTree);
    ul_Index = BIG_ul_SearchFileExt(EDI_Csz_Ini, asz_Name);
    if(ul_Index != BIG_C_InvalidIndex)
    {
        pul_Buf = (ULONG *) BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), &ul_Size);
        while(ul_Size)
        {
			if(*pul_Buf < BIG_gst.st_ToSave.ul_MaxDir)
				BIG_gst.dst_DirTable[*pul_Buf].st_BRO.b_IsExpanded[mi_NumEdit] = TRUE;
            pul_Buf++;
            ul_Size -= 4;
        }
    }

    if((mst_Ini.mst_MadOptions.c_MaterialUpdate < 0) || (mst_Ini.mst_MadOptions.c_MaterialUpdate > 2))
        mst_Ini.mst_MadOptions.c_MaterialUpdate = 1;
    if((mst_Ini.mst_MadOptions.c_MaterialMerge) && (mst_Ini.mst_MadOptions.c_MaterialMerge != 4))
        mst_Ini.mst_MadOptions.c_MaterialMerge= 0;
    if((mst_Ini.mst_MadOptions.c_MaterialNoSingle) && (mst_Ini.mst_MadOptions.c_MaterialNoSingle != 8))
        mst_Ini.mst_MadOptions.c_MaterialNoSingle= 8;
    
    //if((mst_Ini.mst_MadOptions.c_GraphicObjectUpdate < 0) || (mst_Ini.mst_MadOptions.c_GraphicObjectUpdate > 2))
        mst_Ini.mst_MadOptions.c_GraphicObjectUpdate = 1;
    if((mst_Ini.mst_MadOptions.c_GraphicObjectMerge) && (mst_Ini.mst_MadOptions.c_GraphicObjectMerge != 4))
        mst_Ini.mst_MadOptions.c_GraphicObjectMerge= 0;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::SaveColSizeList(void)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    if(mst_Ini.i_ListDispMode == EBRO_C_ListModeReport)
    {
        mst_Ini.i_SortColumnListCtrl = mpo_ListCtrl->mi_SortColumn;
        for(i = 0; i < EBRO_C_ListCtrlMaxColumns; i++)
        {
            if(EBRO_M_EngineMode() && (i > 1) && (i < EBRO_C_ListCtrlSpecial))
                continue;
            if(mst_Ini.ai_NumColListCtrl[i] != -1) mst_Ini.ai_WidthListCol[i] = mpo_ListCtrl->GetColumnWidth(mst_Ini.ai_NumColListCtrl[i]);
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::SaveColSizeGrp(void)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    if(mst_Ini.i_GrpDispMode == EBRO_C_ListModeReport)
    {
        mst_Ini.i_SortColumnGrpCtrl = mpo_GrpCtrl->mi_SortColumn;
        for(i = 0; i < EBRO_C_GrpCtrlMaxColumns; i++)
        {
            if(EBRO_M_EngineMode() && (i > 1)) continue;
            if(mst_Ini.ai_NumColGrpCtrl[i] != -1) mst_Ini.ai_WidthGrpCol[i] = mpo_GrpCtrl->GetColumnWidth(mst_Ini.ai_NumColGrpCtrl[i]);
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::SaveIni(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int         i_YCur, i_Temp;
    BIG_INDEX   ul_Index;
    char        asz_Name[BIG_C_MaxLenName];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    mpo_InsideSplitter->GetRowInfo(0, i_YCur, i_Temp);
    mst_Ini.i_HeightFirstPane = i_YCur;
    mpo_InsideLstSplitter->GetRowInfo(0, i_YCur, i_Temp);
    mst_Ini.i_HeightSecondPane = i_YCur;
    SaveColSizeList();
    SaveColSizeGrp();

_Try_
    EDI_cl_BaseFrame::BaseSaveIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
    /* Linked paths */
    SaveLinkedPath();

    /* Save expanded infos */
    L_strcpy(asz_Name, mst_Def.asz_Name);
    L_strcat(asz_Name, EDI_Csz_ExtTree);
    SAV_Begin(EDI_Csz_Ini, asz_Name);
    for(ul_Index = 0; ul_Index < BIG_MaxDir(); ul_Index++)
    {
        if(BIG_gst.dst_DirTable[ul_Index].st_BRO.b_IsExpanded[mi_NumEdit])
            SAV_Buffer(&ul_Index, sizeof(BIG_INDEX));
    }

    SAV_ul_End();
}

#endif /* ACTIVE_EDITORS */

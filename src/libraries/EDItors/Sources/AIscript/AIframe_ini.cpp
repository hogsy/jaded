/*$T AIframe_ini.cpp GC!1.71 01/18/00 16:21:11 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIStrings.h"
#include "EditView/AIleftview.h"
#include "EditView/AIview.h"
#include "AIframe.h"
#include "AIleftframe.h"
#include "EditView/AIview.h"

#ifdef JADEFUSION
extern BOOL		EDI_gb_NoVerbose;
#else
extern "C" BOOL		EDI_gb_NoVerbose;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::ReinitIni(void)
{
    EDI_cl_BaseFrame::ReinitIni();

    /* Colors */
    mst_Ini.ax_Colors[EAI_C_ColorBkGnd] = RGB(0, 0, 0);
    mst_Ini.ax_Colors[EAI_C_ColorDefault] = RGB(0xFF, 0xFF, 0xFF);
    mst_Ini.ax_Colors[EAI_C_ColorType] = RGB(0xFF, 0x50, 0xFF);
    mst_Ini.ax_Colors[EAI_C_ColorKeyword] = RGB(0x50, 0xFF, 0xFF);
    mst_Ini.ax_Colors[EAI_C_ColorFunction] = RGB(0xFF, 0xFF, 0x50);
    mst_Ini.ax_Colors[EAI_C_ColorComment] = RGB(0x50, 0xFF, 0x50);
    mst_Ini.ax_Colors[EAI_C_ColorPP] = RGB(0xFF, 0x50, 0x50);
    mst_Ini.ax_Colors[EAI_C_ColorField] = RGB(0x80, 0xa0, 0xFF);
    mst_Ini.ax_Colors[EAI_C_ColorReserved] = RGB(0xFF, 0xF0, 0x50);
    mst_Ini.ax_Colors[EAI_C_ColorString] = RGB(0x50, 0xF0, 0xFF);
    mst_Ini.ax_Colors[EAI_C_ColorConstant] = RGB(0x00, 0xF0, 0xF0);
    mst_Ini.ax_Colors[EAI_C_ColorPPConstants] = RGB(0x00, 0x70, 0xF0);
    mst_Ini.ax_Colors[EAI_C_ColorPPMacros] = RGB(0xF0, 0x00, 0x70);
    mst_Ini.ax_Colors[EAI_C_ColorProcedures] = RGB(0xE1, 0x70, 0x10);

    L_strcpy(mst_Ini.asz_FaceName, EDI_STR_Csz_Font);
    mst_Ini.i_FontSize = 16;
    mst_Ini.i_TabStop = 4;
    mst_Ini.i_WidthSplitter = 100;
    mst_Ini.i_HeightSplitter1 = 200;
    mst_Ini.i_WidthCol1 = 100;
    mst_Ini.i_WidthCol2 = 40;
    mst_Ini.i_SortedCol = 0;
    mst_Ini.b_AutoSave = TRUE;
    mst_Ini.b_BreakEnable = TRUE;
	mst_Ini.b_WarningHigh = FALSE;

    /* Size of completion box */
    mst_Ini.i_XCompletion = 400;
    mst_Ini.i_YCompletion = 400;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::LoadIni(void)
{
_Try_
    EDI_cl_BaseFrame::BaseLoadIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
	if(EDI_gb_NoVerbose) mst_Ini.b_WarningHigh = FALSE;
    TreatIni();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::TreatIni(void)
{
    /* Colors */
    mpo_Edit->ResetDefaultFormat();
    mpo_Edit->mi_LastColLine = -1;
    mpo_Edit->mb_FirstColoring = TRUE;

    /* Splitter */
    mpo_Splitter->SetColumnInfo(0, mst_Ini.i_WidthSplitter, 0);

    /* Column */
    mpo_ListAI->SetColumnWidth(0, mst_Ini.i_WidthCol1);
    mpo_ListAI->SetColumnWidth(1, mst_Ini.i_WidthCol2);
    mpo_ListAI->SortList();

	/* Other */
	AI_gb_CanBreak = mst_Ini.b_BreakEnable ? TRUE : FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::SaveIni(void)
{
    /*~~~~~~~~~~~~~~~~~~~*/
    int i_YCur, i_Temp;
    /*~~~~~~~~~~~~~~~~~~~*/

    mpo_Splitter->GetColumnInfo(0, i_YCur, i_Temp);
    mst_Ini.i_WidthSplitter = i_YCur;
    mst_Ini.i_HeightSplitter1 = i_YCur;
    mst_Ini.i_WidthCol1 = mpo_ListAI->GetColumnWidth(0);
    mst_Ini.i_WidthCol2 = mpo_ListAI->GetColumnWidth(1);

_Try_
    EDI_cl_BaseFrame::BaseSaveIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
}

#endif /* ACTIVE_EDITORS */

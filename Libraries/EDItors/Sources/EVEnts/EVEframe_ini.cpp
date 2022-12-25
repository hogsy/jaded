/*$T EVEframe_ini.cpp GC! 1.078 03/15/00 17:20:12 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "EVEframe.h"
#include "EDImainframe.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::ReinitIni(void)
{
	int	i;

	EDI_cl_BaseFrame::ReinitIni();
	mst_Ini.i_SplitterWidth = 100;
	mst_Ini.f_DivFactor = 10.0f;
	mst_Ini.f_ValFactor = 1.0f;
	mst_Ini.i_Inter = 20;
	mst_Ini.i_YBar = 18;
	mst_Ini.f_Factor = mst_Ini.f_DivFactor * mst_Ini.i_Inter;
	mst_Ini.i_TimeDisplay = EEVE_C_OptionTimeSec;
	mst_Ini.i_ForceSnap = 0;
	mst_Ini.i_DisplayHidden = 1;
	mst_Ini.i_XLeft = 100;
	mst_Ini.f_RealUnit = 1.0f / 60.0f;
	mst_Ini.i_UnitMode = EEVE_C_UnitModeAuto;
	mst_Ini.i_AllEventsVis = 0;
	mst_Ini.i_AutoAlign = TRUE;
	mst_Ini.i_SnapMode = EEVE_C_SnapModeAuto;
	mst_Ini.f_SnapUnit = 1.0f / 60.0f;
	mst_Ini.i_DisplayTrackName = 0;
	mst_Ini.i_PlayHz = 60;
	mst_Ini.b_AnimMode = FALSE;
	mst_Ini.b_Lock = FALSE;
	mst_Ini.b_RunSel = FALSE;
	mst_Ini.f_WidthFixed = 50;
	L_memset(mst_Ini.a, 0, sizeof(mst_Ini.a));

	for(i = 0; i < sizeof(mst_Ini.ac_ColIA) / 4; i++)
		mst_Ini.ac_ColIA[i] = M_MF()->u4_Interpol2PackedColor(0x00FF8080, GetSysColor(COLOR_BTNFACE), 0.8f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::LoadIni(void)
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
void EEVE_cl_Frame::TreatIni(void)
{
	if(mst_Ini.i_SplitterWidth < 0) mst_Ini.i_SplitterWidth = 100;
	mpo_MainSplitter->SetColumnInfo(0, mst_Ini.i_SplitterWidth, 0);

    SetLinkedOutputEditors();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SaveIni(void)
{
	/*~~~~~~~~~~~~~~~*/
	int i_YCur, i_Temp;
	/*~~~~~~~~~~~~~~~*/

	mpo_MainSplitter->GetColumnInfo(0, i_YCur, i_Temp);
	mst_Ini.i_SplitterWidth = i_YCur;

_Try_
	EDI_cl_BaseFrame::BaseSaveIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
}

#endif /* ACTIVE_EDITORS */

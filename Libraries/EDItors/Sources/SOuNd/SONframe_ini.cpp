/*$T SONframe_ini.cpp GC! 1.081 02/07/03 11:38:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
#include "Res/Res.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDwave.h"

#include "SONframe.h"
#include "SONstrings.h"
#include "SONview.h"
#include "SONview_smd.h"
#include "SONview_group.h"
#include "SONmsg.h"
#include "SONvumeter.h"

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	ESON_tdst_IniOld_
{
	int				i_SplitterWidth0;
	char			sz_SoundEditor[256];
	char			sz_TempDir[256];
	LONG			l_ListSoundColumnWidth[10];
	LONG			l_ListInstanceColumnWidth[10];
	int				i_SplitterWidth1;
	LONG			l_ListBankColumnWidth[10];
	int				i_SplitterWidth2;
	int				i_Option;
	int				i_FiltersSound;

	unsigned int	ui_PlayerFreq;
	float			f_PlayerVol;
	int				i_PlayerPan;
	int				i_FilterInstance;
	int				i_Falloff;
	int				i_PeakTTL;
	int			l_Dummy[7];
} ESON_tdst_IniOld;

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define ESON_Csz_EdiVersion "0000-0000-0002"

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */
#ifdef JADEFUSION
extern float SND_gf_AutoVolumeOff;
#else
extern "C" float SND_gf_AutoVolumeOff;
#endif

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::ReinitIni(void)
{
	mst_BaseIni.i_TopInMenu = 0x1e6;
	EDI_cl_BaseFrame::ReinitIni();

	strcpy(mst_Ini.sz_IniVersion, ESON_Csz_EdiVersion);
	mst_Ini.i_SplitterWidth0 = 100;
	mst_Ini.i_SplitterWidth1 = 100;
	mst_Ini.i_SplitterWidth2 = 100;
	*mst_Ini.sz_TempDir = 0;
	*mst_Ini.sz_SoundEditor = 0;

	mst_Ini.l_ListBankColumnWidth[0] = 64;
	mst_Ini.l_ListBankColumnWidth[1] = 64;
	mst_Ini.l_ListBankColumnWidth[2] = 64;
	mst_Ini.l_ListBankColumnWidth[3] = 64;
	mst_Ini.l_ListBankColumnWidth[4] = 64;

	mst_Ini.l_ListSoundColumnWidth[0] = 64;
	mst_Ini.l_ListSoundColumnWidth[1] = 64;
	mst_Ini.l_ListSoundColumnWidth[2] = 64;
	mst_Ini.l_ListSoundColumnWidth[3] = 64;
	mst_Ini.l_ListSoundColumnWidth[4] = 64;

	mst_Ini.l_ListInstanceColumnWidth[0] = 64;
	mst_Ini.l_ListInstanceColumnWidth[1] = 64;
	mst_Ini.l_ListInstanceColumnWidth[2] = 64;
	mst_Ini.l_ListInstanceColumnWidth[3] = 64;
	mst_Ini.l_ListInstanceColumnWidth[4] = 64;
	mst_Ini.l_ListInstanceColumnWidth[5] = 64;
	mst_Ini.l_ListInstanceColumnWidth[6] = 64;
	mst_Ini.l_ListInstanceColumnWidth[7] = 64;
	mst_Ini.l_ListInstanceColumnWidth[8] = 64;
	mst_Ini.l_ListInstanceColumnWidth[9] = 64;

	mst_Ini.l_ListMuteColumnWidth[0] = 64;
	mst_Ini.l_ListMuteColumnWidth[1] = 64;
	mst_Ini.l_ListMuteColumnWidth[2] = 64;
	mst_Ini.l_ListMuteColumnWidth[3] = 64;
	mst_Ini.l_ListMuteColumnWidth[4] = 64;
	mst_Ini.l_ListMuteColumnWidth[5] = 64;
	mst_Ini.l_ListMuteColumnWidth[6] = 64;
	mst_Ini.l_ListMuteColumnWidth[7] = 64;
	mst_Ini.l_ListMuteColumnWidth[8] = 64;

	mst_Ini.i_FiltersSound = 63;
	mst_Ini.i_FilterInstance =
		(
			ESON_C_InstDisplaySound | ESON_C_InstDisplaySound | ESON_C_InstDisplayState | ESON_C_InstDisplayVol |
				ESON_C_InstDisplayFlags | ESON_C_InstDisplayExtFlags | ESON_C_InstDisplayTrack
		);

	mst_Ini.i_Option = 0;

	mst_Ini.f_SndPlayerVol = 1.0f;
	mst_Ini.i_SndPlayerPan = 0;
	mst_Ini.ui_SndPlayerFreq = 0;

	mst_Ini.f_SmdPlayerVol = 1.0f;
	mst_Ini.i_SmdPlayerPan = 0;
	mst_Ini.ui_SmdPlayerFreq = 0;

	mst_Ini.i_VuFalloff = 50;
	mst_Ini.i_VuPeakTTL = 50;

	if(mpo_View)
	{
		mpo_View->mi_FirstUpdate = 0;
		mpo_View->FirstUpdateControls();
	}

    mst_Ini.c_TabId	= 0;
    mst_Ini.i_LastSplitter	= 200;

	mst_BaseIni.i_TopInMenu = 0x1e6;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::LoadIni(void)
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
void ESON_cl_Frame::TreatIni(void)
{
	if(!strcmp(mst_Ini.sz_IniVersion, "0000-0000-0000"))
	{
		/*$F
		 *	update 0000 to 0001 : just save the new version
		 */
		strcpy(mst_Ini.sz_IniVersion, ESON_Csz_EdiVersion);
		this->SaveIni();
	}
	else if(!strcmp(mst_Ini.sz_IniVersion, "0000-0000-0001"))
	{
		/*$F
		 *	update 0001 to 0002 : just save the new version
		 */
		strcpy(mst_Ini.sz_IniVersion, ESON_Csz_EdiVersion);
		mst_Ini.l_ListMuteColumnWidth[0] = 64;
		mst_Ini.l_ListMuteColumnWidth[1] = 64;
		mst_Ini.l_ListMuteColumnWidth[2] = 64;
		this->SaveIni();
	}
	else if(strcmp(mst_Ini.sz_IniVersion, ESON_Csz_EdiVersion))
	{
		ReinitIni();
		
		::MessageBox
			(
				NULL,
				"Your Sound.ini file has been update.\nPlease check the sound editor configuration and save it (\"control+W\" or \"INI files->Save Current\")",
				"Sound Editor",
				MB_OK | MB_ICONEXCLAMATION
			);
	}

	if(mst_Ini.i_SplitterWidth0 < 0) mst_Ini.i_SplitterWidth0 = 100;
	if(mst_Ini.i_SplitterWidth1 < 0) mst_Ini.i_SplitterWidth1 = 100;
	if(mst_Ini.i_SplitterWidth2 < 0) mst_Ini.i_SplitterWidth2 = 100;
	if(mst_Ini.i_LastSplitter < 0) mst_Ini.i_LastSplitter = 200;
	if(mst_Ini.c_TabId < 0) mst_Ini.c_TabId = 0;
	if(mst_Ini.c_TabId > 3) mst_Ini.c_TabId = 0;

	if(SND_gst_Params.l_Available)
	{
		mpo_Splitter->SetColumnInfo(1, mst_Ini.i_SplitterWidth0, 20);
		mpo_Splitter->SetColumnInfo(2, mst_Ini.i_SplitterWidth1, 20);
		mpo_Splitter->SetColumnInfo(0, mst_Ini.i_SplitterWidth2, 20);
		mpo_Splitter2->SetRowInfo(0, mst_Ini.i_LastSplitter, 20);		
	}
	else
	{
		mpo_Splitter->SetColumnInfo(1, 1, 1);
		mpo_Splitter->SetColumnInfo(2, 1, 1);
		mpo_Splitter->SetColumnInfo(0, 1000, 500);
		mpo_DialogBar->GetDlgItem(IDC1)->SetWindowText("NO SOUND AVAILABLE");
		mpo_DialogBar->GetDlgItem(IDC2)->SetWindowText("NO SOUND AVAILABLE");
		mpo_DialogBar->GetDlgItem(IDC3)->SetWindowText("NO SOUND AVAILABLE");
		return;
	}

	/* test validity of path */
	if(mst_Ini.sz_SoundEditor[1] != ':') mst_Ini.sz_SoundEditor[0] = 0;
	if(mst_Ini.sz_TempDir[1] != ':') mst_Ini.sz_TempDir[0] = 0;

	if(mst_Ini.i_Option & ESON_Cte_AutoSave)
		mst_SModifierDesc.b_AutoSave = TRUE;
	else
		mst_SModifierDesc.b_AutoSave = FALSE;

	if(!(mst_Ini.i_Option & ESON_Cte_VolumeOff))
	{
		if(SND_gf_AutoVolumeOff == 0.0f) i_OnMessage(ESON_MESSAGE_AUTOVOLUMEOFF, 0, 0);
	}
	
	if(mst_Ini.i_Option & ESON_Cte_EnablePCM)
		SND_gst_Params.i_EdiWaveFormat = WAVE_FORMAT_PCM;
	else
		SND_gst_Params.i_EdiWaveFormat = WAVE_FORMAT_XBOX_ADPCM;

	
	if(mst_Ini.i_Option & ESON_Cte_NoSynchro)
		SND_gst_Params.ul_Flags |= SND_Cte_EdiNoSynchro;
	else
		SND_gst_Params.ul_Flags &= ~SND_Cte_EdiNoSynchro;


#ifdef SND_DEBUG
	SND_gst_Debug.b_EnableDebugLog = (mst_Ini.i_Option & ESON_Cte_EnableDebugLog) ? 1 : 0;
#endif

	if(mpo_View)
	{
		mpo_View->mi_FirstUpdate = 0;
		mpo_View->FirstUpdateControls();
        ((CTabCtrl *) mpo_View->GetDlgItem(IDC_TAB_VIEW))->SetCurFocus(mst_Ini.c_TabId);
	}

	mst_SDesc.f_PlayerVol = 1.0f;//mst_Ini.f_SndPlayerVol;
	mst_SDesc.i_PlayerPan = 0;//mst_Ini.i_SndPlayerPan;
	mst_SDesc.ui_PlayerFreq = 0;//mst_Ini.ui_SndPlayerFreq;
	mst_SDesc.f_PlayerStartTime = 0.0f;

	if(mst_SDesc.f_PlayerVol > 1.0f) mst_SDesc.f_PlayerVol = 1.0f;
	if(mst_SDesc.f_PlayerVol < 0.0f) mst_SDesc.f_PlayerVol = 0.0f;

	if(mst_SDesc.i_PlayerPan > 10000) mst_SDesc.i_PlayerPan = 10000;
	if(mst_SDesc.i_PlayerPan < -10000) mst_SDesc.i_PlayerPan = -10000;

	if(mst_SDesc.ui_PlayerFreq > 100000) mst_SDesc.ui_PlayerFreq = 100000;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SaveIni(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i_YCur, i_Temp;
	CListCtrl	*po_LC;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_Splitter->GetColumnInfo(1, i_YCur, i_Temp);
	mst_Ini.i_SplitterWidth0 = i_YCur;

	mpo_Splitter->GetColumnInfo(2, i_YCur, i_Temp);
	mst_Ini.i_SplitterWidth1 = i_YCur;

	mpo_Splitter->GetColumnInfo(0, i_YCur, i_Temp);
	mst_Ini.i_SplitterWidth2 = i_YCur;

    mpo_Splitter2->GetRowInfo(0, i_YCur, i_Temp);
    mst_Ini.i_LastSplitter = i_YCur;

	if(mpo_VUmeter)
	{
		mst_Ini.i_VuFalloff = mpo_VUmeter->mi_Falloff;
		mst_Ini.i_VuPeakTTL = mpo_VUmeter->mi_PeakTTL;
	}

	if(mpo_View)
	{
        mst_Ini.c_TabId = ((CTabCtrl *) mpo_View->GetDlgItem(IDC_TAB_VIEW))->GetCurFocus();

		po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_BANK);
		mst_Ini.l_ListBankColumnWidth[0] = po_LC->GetColumnWidth(0);
		mst_Ini.l_ListBankColumnWidth[1] = po_LC->GetColumnWidth(1);
		mst_Ini.l_ListBankColumnWidth[2] = po_LC->GetColumnWidth(2);
		mst_Ini.l_ListBankColumnWidth[3] = po_LC->GetColumnWidth(3);
		mst_Ini.l_ListBankColumnWidth[4] = po_LC->GetColumnWidth(4);

		po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_SOUND);
		mst_Ini.l_ListSoundColumnWidth[0] = po_LC->GetColumnWidth(0);
		mst_Ini.l_ListSoundColumnWidth[1] = po_LC->GetColumnWidth(1);
		mst_Ini.l_ListSoundColumnWidth[2] = po_LC->GetColumnWidth(2);
		mst_Ini.l_ListSoundColumnWidth[3] = po_LC->GetColumnWidth(3);
		mst_Ini.l_ListSoundColumnWidth[4] = po_LC->GetColumnWidth(4);

		po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
		mst_Ini.l_ListInstanceColumnWidth[0] = po_LC->GetColumnWidth(0);
		mst_Ini.l_ListInstanceColumnWidth[1] = po_LC->GetColumnWidth(1);
		mst_Ini.l_ListInstanceColumnWidth[2] = po_LC->GetColumnWidth(2);
		mst_Ini.l_ListInstanceColumnWidth[3] = po_LC->GetColumnWidth(3);
		mst_Ini.l_ListInstanceColumnWidth[4] = po_LC->GetColumnWidth(4);
		mst_Ini.l_ListInstanceColumnWidth[5] = po_LC->GetColumnWidth(5);
		mst_Ini.l_ListInstanceColumnWidth[6] = po_LC->GetColumnWidth(6);
		mst_Ini.l_ListInstanceColumnWidth[7] = po_LC->GetColumnWidth(7);
		mst_Ini.l_ListInstanceColumnWidth[8] = po_LC->GetColumnWidth(8);
		mst_Ini.l_ListInstanceColumnWidth[9] = po_LC->GetColumnWidth(9);
		
		po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_MUTE);
		mst_Ini.l_ListMuteColumnWidth[0] = po_LC->GetColumnWidth(0);
		mst_Ini.l_ListMuteColumnWidth[1] = po_LC->GetColumnWidth(1);
		mst_Ini.l_ListMuteColumnWidth[2] = po_LC->GetColumnWidth(2);
		mst_Ini.l_ListMuteColumnWidth[3] = po_LC->GetColumnWidth(3);
		mst_Ini.l_ListMuteColumnWidth[4] = po_LC->GetColumnWidth(4);
		mst_Ini.l_ListMuteColumnWidth[5] = po_LC->GetColumnWidth(5);
	}

	mst_Ini.f_SndPlayerVol = mst_SDesc.f_PlayerVol;
	mst_Ini.i_SndPlayerPan = mst_SDesc.i_PlayerPan;
	mst_Ini.ui_SndPlayerFreq = mst_SDesc.ui_PlayerFreq;

	mst_Ini.f_SmdPlayerVol = -1.0f;
	mst_Ini.i_SmdPlayerPan = 0;
	mst_Ini.ui_SmdPlayerFreq = 0;
_Try_
	EDI_cl_BaseFrame::BaseSaveIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
}

#endif /* ACTIVE_EDITORS */

/*$T SONframe.cpp GC 1.138 04/05/04 14:07:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "EDIpaths.h"
#include "EDItors/Sources/BROwser/BROframe.h"

/*$2- dialog ---------------------------------------------------------------------------------------------------------*/

#include "DIAlogs/DIAfile_dlg.h"

/*$2- bigfile --------------------------------------------------------------------------------------------------------*/

#include "BIGfiles/BIGexport.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/LOAding/LOAdefs.h"

/*$2- wor ------------------------------------------------------------------------------------------------------------*/

#include "WORld/WORvars.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDspecific.h"
#include "SouND/sources/SNDstream.h"
#include "SouND/sources/SNDconst.h"
#include "SouND/sources/SNDconv.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDbank.h"

/*$2- sound editor ---------------------------------------------------------------------------------------------------*/

#define ACTION_GLOBAL
#include "SONframe_act.h"
#include "SONframe.h"
#include "SONview.h"
#include "SONview_smd.h"
#include "SONview_group.h"
#include "SONpane.h"
#include "SONutil.h"
#include "SONvumeter.h"

/*$4
 ***********************************************************************************************************************
    VARIABLES
 ***********************************************************************************************************************
 */

/*$2- global ---------------------------------------------------------------------------------------------------------*/

/*$2- extern ---------------------------------------------------------------------------------------------------------*/

#ifdef JADEFUSION
extern BOOL		SND_gb_EdiPause;
extern float	SND_gf_AutoVolumeOff;
extern void		AI_AddWatch(void *p, int s);
#else
extern "C" BOOL		SND_gb_EdiPause;
extern "C" float	SND_gf_AutoVolumeOff;
extern "C" void		AI_AddWatch(void *p, int s);
#endif

/*$4
 ***********************************************************************************************************************
    MESSAGE MAP.
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNCREATE(ESON_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(ESON_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_Frame::ESON_cl_Frame(void)
{
	mi_FiltersGroup = 0x1FFF;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_Splitter = new CSplitterWnd;
	mpo_Splitter2 = new CSplitterWnd;
	mpo_VarsView0 = new EVAV_cl_View;
	mpo_VarsView1 = new EVAV_cl_View;
	mpo_ViewSmd = new ESON_cl_ViewSmd(this);
	mpo_ViewGroup = new ESON_cl_ViewGroup(this);
	mpo_View = new ESON_cl_View(this);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_SDesc.ul_Key = BIG_C_InvalidKey;
	mst_SDesc.pWave = NULL;
	mst_SDesc.ul_Fat = BIG_C_InvalidIndex;
	mst_SDesc.po_SoundBuffer = NULL;
	mst_SDesc.b_Paused = FALSE;
	mst_SDesc.b_PlayLoop = FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_SModifierDesc.ul_Key = BIG_C_InvalidKey;
	mst_SModifierDesc.ul_Fat = BIG_C_InvalidIndex;
	mst_SModifierDesc.i_Index = -1;
	mst_SModifierDesc.i_Instance = -1;
	mst_SModifierDesc.b_AutoSave = FALSE;
	mst_SModifierDesc.b_NeedSaving = FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_SndBankDesc.pst_CurrentBank = NULL;
	mst_SndBankDesc.ul_Fat = BIG_C_InvalidIndex;
	mst_SndBankDesc.ul_Key = BIG_C_InvalidKey;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_VUmeter = NULL;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mui_TimerId = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_Frame::~ESON_cl_Frame(void)
{
	if(mpo_VUmeter)
	{
		if(mpo_VUmeter->mb_IsRecording) this->OnVumeter();

		delete mpo_VUmeter;
		mpo_VUmeter = NULL;
	}

	if(mst_SndBankDesc.pst_CurrentBank) SND_MainFreeUnion(mst_SndBankDesc.pst_CurrentBank);
	if(mst_SDesc.pWave) SND_WaveUnload(mst_SDesc.pWave);

	SModifier_SaveCurrent(FALSE);
	SModifier_CloseCurrent(FALSE);

	mpo_Splitter2->DestroyWindow();
	mpo_Splitter->DestroyWindow();
	mst_SModifierDesc.b_NeedSaving = FALSE;

	delete mpo_Splitter;
	delete mpo_Splitter2;
	delete mpo_VarsView0;
	delete mpo_VarsView1;
	delete mpo_PaneList;
	delete mpo_PaneSmd;
	delete mpo_PaneGrp;
	delete mpo_PaneSound;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	EDI_cl_BaseFrame::OnCreate(lpcs);

    //0:list
    //1:smd
    //2:ram raster/grp
    //3:sound
	mpo_PaneList = new ESON_cl_Pane;
	mpo_PaneList->mi_NumPane = 0;
	mpo_PaneList->mpo_Frame = this;

	mpo_PaneSmd = new ESON_cl_Pane;
	mpo_PaneSmd->mi_NumPane = 1;
	mpo_PaneSmd->mpo_Frame = this;

	mpo_PaneGrp = new ESON_cl_Pane;
	mpo_PaneGrp->mi_NumPane = 2;
	mpo_PaneGrp->mpo_Frame = this;

	mpo_PaneSound = new ESON_cl_Pane;
	mpo_PaneSound->mi_NumPane = 3;
	mpo_PaneSound->mpo_Frame = this;

	/*$1- Create splitters ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_Splitter->CreateStatic(this, 1, 4);
	mpo_Splitter2->CreateStatic
		(
			mpo_Splitter,
			2,
			1,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			mpo_Splitter->IdFromRowCol(0, 1)
		);

	/*$1- var view for bank ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_VarsView0->mb_CanDragDrop = TRUE;
	mst_VarsViewStruct0.po_ListItems = &mo_ListItems0;
	mst_VarsViewStruct0.psz_NameCol1 = "Name";
	mst_VarsViewStruct0.i_WidthCol1 = 100;
	mst_VarsViewStruct0.psz_NameCol2 = "Value";
	mpo_VarsView0->MyCreate(mpo_Splitter2, &mst_VarsViewStruct0, this, mpo_Splitter2->IdFromRowCol(1, 0));

	/*$1- var view for Sound ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_SoundSplitterId = 1;
	mpo_VarsView1->mb_CanDragDrop = TRUE;
	mst_VarsViewStruct1.po_ListItems = &mo_ListItems1;
	mst_VarsViewStruct1.psz_NameCol1 = "Name";
	mst_VarsViewStruct1.i_WidthCol1 = 100;
	mst_VarsViewStruct1.psz_NameCol2 = "Value";
	mpo_VarsView1->MyCreate(mpo_Splitter2, &mst_VarsViewStruct1, this, mpo_Splitter2->IdFromRowCol(0, 0));

	/*$1- var view for SModifier ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_SModifierSplitterId = 2;
	mpo_ViewSmd->Create
		(
			"EditSmd",
			"EditSmd",
			0,
			CRect(0, 0, 0, 0),
			mpo_Splitter,
			mpo_Splitter->IdFromRowCol(0, mi_SModifierSplitterId)
		);
	mpo_ViewSmd->mpo_Editor = this;
	mpo_ViewSmd->UpdateSmdView();

	/*$1- group ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_BankSplitterId = 3;
	mpo_ViewGroup->Create
		(
			"EditGrp",
			"EditGrp",
			0,
			CRect(0, 0, 0, 0),
			mpo_Splitter,
			mpo_Splitter->IdFromRowCol(0, mi_BankSplitterId)
		);
	mpo_ViewGroup->mpo_Editor = this;
	mpo_ViewGroup->UpdateControls();

	/*$1- view for list ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_ListSplitterId = 0;
	mpo_View->Create
		(
			"Edit",
			"Edit",
			0,
			CRect(0, 0, 0, 0),
			mpo_Splitter,
			mpo_Splitter->IdFromRowCol(0, mi_ListSplitterId)
		);
	mpo_View->mpo_Editor = this;
	mpo_View->UpdateControls();

	mpo_PaneList->SubclassDlgItem(IDC0, mpo_DialogBar);
	mpo_PaneList->SetFont(&M_MF()->mo_Fnt);

	mpo_PaneSmd->SubclassDlgItem(IDC2, mpo_DialogBar);
	mpo_PaneSmd->SetFont(&M_MF()->mo_Fnt);
	
    mpo_PaneGrp->SubclassDlgItem(IDC3, mpo_DialogBar);
	mpo_PaneGrp->SetFont(&M_MF()->mo_Fnt);
	
    mpo_PaneSound->SubclassDlgItem(IDC1, mpo_DialogBar);
	mpo_PaneSound->SetFont(&M_MF()->mo_Fnt);
    
    mpo_PaneGrp->EnableWindow(FALSE);
        
	
    UpdateVarView_Sound();
    OnRefreshRaster();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);
	SizeDialogBar();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OneTrameEnding(void)
{
	if(!SND_gst_Params.l_Available) return;
	if((mpo_View->mi_Pane == 2) || (mpo_View->mi_Pane == 3) || (mpo_View->mi_Pane == 4)) mpo_View->UpdateControls();
	mpo_ViewGroup->UpdateControls();
	UpdateVarView_Bank();
	UpdateVarView_Sound();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_Frame::b_CanActivate(void)
{
	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    windows
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::RefreshDialogBar(void)
{
	mpo_DialogBar->Invalidate();
	mpo_PaneList->Invalidate();
	mpo_PaneSmd->Invalidate();
	mpo_PaneSound->Invalidate();
	mpo_PaneGrp->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SizeDialogBar(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		i_Temp;
	int		i_ColWidth0, i_ColWidth1, i_ColWidth2, i_ColWidth3;
	CRect	o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_Splitter->GetColumnInfo(0, i_ColWidth0, i_Temp);
	mpo_Splitter->GetColumnInfo(1, i_ColWidth1, i_Temp);
	mpo_Splitter->GetColumnInfo(2, i_ColWidth2, i_Temp);
	mpo_Splitter->GetColumnInfo(3, i_ColWidth3, i_Temp);

	mpo_DialogBar->GetDlgItem(IDC0)->GetWindowRect(&o_Rect);
	mpo_DialogBar->ScreenToClient(&o_Rect);

	o_Rect.left = 0;
	o_Rect.right = o_Rect.left + i_ColWidth0 + 4;
	mpo_DialogBar->GetDlgItem(IDC0)->MoveWindow(o_Rect);

	o_Rect.left = o_Rect.right + 3;
	o_Rect.right = o_Rect.left + i_ColWidth1 + 4;
	mpo_DialogBar->GetDlgItem(IDC1)->MoveWindow(o_Rect);

	o_Rect.left = o_Rect.right + 3;
	o_Rect.right = o_Rect.left + i_ColWidth2 + 4;
	mpo_DialogBar->GetDlgItem(IDC2)->MoveWindow(o_Rect);

	o_Rect.left = o_Rect.right + 3;
	o_Rect.right = o_Rect.left + i_ColWidth3 + 4;
	mpo_DialogBar->GetDlgItem(IDC3)->MoveWindow(o_Rect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnRealIdle(void)
{
	SizeDialogBar();
}

/*$4
 ***********************************************************************************************************************
    external editor
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::External_SetEditor(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*psz_EndPath;
	EDIA_cl_FileDialog	*po_File;
	CString				o_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	psz_EndPath = strrchr(mst_Ini.sz_SoundEditor, '/');
	if(psz_EndPath)
	{
		*psz_EndPath = 0;
		po_File = new EDIA_cl_FileDialog("Select sound editor", 0, 0, 0, mst_Ini.sz_SoundEditor, "*.exe");
		*psz_EndPath = '/';
	}
	else
		po_File = new EDIA_cl_FileDialog("Select sound editor", 0, 0, 0, NULL, "*.exe");

	if(po_File->DoModal() == IDOK)
	{
		po_File->GetItem(po_File->mo_File, 0, o_Temp);
		sprintf(mst_Ini.sz_SoundEditor, "%s/%s", po_File->masz_FullPath, (LPCTSTR) o_Temp);
	}

	delete po_File;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::External_SetTempDir(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	*po_File;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_Ini.sz_TempDir[1] == ':')
		po_File = new EDIA_cl_FileDialog("Select sound editor", 2, 0, 0, mst_Ini.sz_TempDir);
	else
		po_File = new EDIA_cl_FileDialog("Select sound editor", 2, 0);

	if(po_File->DoModal() == IDOK)
	{
		L_strcpy(mst_Ini.sz_TempDir, po_File->masz_FullPath);
	}

	delete po_File;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::External_Edit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	char	sz_OldDir[512];
	char	sz_TempFile[260];
	/*~~~~~~~~~~~~~~~~~~~~~*/

	GetCurrentDirectory(260, sz_OldDir);
	if(!SetCurrentDirectory(mst_Ini.sz_TempDir))
	{
		M_MF()->MessageBox("Can't access temp directory", "Error", MB_OK | MB_ICONSTOP);
		return;
	}

	SetCurrentDirectory(sz_OldDir);

	BIG_ComputeFullName(BIG_ParentFile(mst_SDesc.ul_Fat), sz_OldDir);
	sprintf(sz_TempFile, "%s/%s", mst_Ini.sz_TempDir, BIG_NameFile(mst_SDesc.ul_Fat));
	BIG_ExportFileToDisk(sz_TempFile, sz_OldDir, BIG_NameFile(mst_SDesc.ul_Fat));

	sprintf(sz_OldDir, "%s %s", mst_Ini.sz_SoundEditor, sz_TempFile);
	WinExec(sz_OldDir, SW_SHOW);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::External_Reload(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	char	sz_TempFile[260];
	char	sz_Path[260];
	/*~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(sz_TempFile, "%s/%s", mst_Ini.sz_TempDir, BIG_NameFile(mst_SDesc.ul_Fat));
	if(L_access(sz_TempFile, 0))
	{
		M_MF()->MessageBox("Can't access temp file", "Error", MB_OK | MB_ICONSTOP);
		return;
	}

	SND_Delete(mst_SDesc.ul_Key);
	LOA_DeleteAddress((void *) LOA_ul_SearchAddress(BIG_PosFile(mst_SDesc.ul_Fat)));
	BIG_ComputeFullName(BIG_ParentFile(mst_SDesc.ul_Fat), sz_Path);
	BIG_UpdateFileFromDisk(sz_TempFile, sz_Path, BIG_NameFile(mst_SDesc.ul_Fat));

	mst_SDesc.ul_Fat = BIG_ul_SearchKeyToFat(mst_SDesc.ul_Key);
	mst_SDesc.ul_Key = BIG_C_InvalidKey;
	Sound_Set(mst_SDesc.ul_Fat);
}

/*$4
 ***********************************************************************************************************************
    display
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Display_EqualSplit(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	int i_ColWidth, i_Temp;
	int i_Width;
	int i_ColNb, i;
    int aiCol[]={0, 200, 320, 205};
	/*~~~~~~~~~~~~~~~~~~~*/


	i_ColNb = mpo_Splitter->GetColumnCount();
	i_Width = 0;

	for(i = 0; i < i_ColNb; i++)
	{
		mpo_Splitter->GetColumnInfo(i, i_ColWidth, i_Temp);
		i_Width += i_ColWidth;
        i_Width -= aiCol[i];
	}
    
    aiCol[0] = i_Width>=0 ? i_Width : 0;

	for(i = 0; i < i_ColNb; i++)
	    mpo_Splitter->SetColumnInfo(i, aiCol[i], 20);		
	

    mpo_Splitter2->SetRowInfo(0, 200, 20);
	mpo_Splitter2->SetRowInfo(1, 200, 20);

	mpo_Splitter->RecalcLayout();
	mpo_Splitter2->RecalcLayout();
}

/*$4
 ***********************************************************************************************************************
    OnAction
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnWorldDestruction(void)
{
	SND_gb_EdiPause = FALSE;
	SND_StreamPrefetchFlushAll();
	SND_StopAll(0);
	ESON_SoloOff();
	ESON_MuteOff();
	if(mst_Ini.i_Option & ESON_Cte_CloseAllWhenDestroyWorld) OnAction(ESON_ACTION_CLOSEALL);
	mpo_View->mst_InstanceStat.i_MaxNumber = 0;
	mpo_View->mst_InstanceStat.i_MaxPlayingNumber = 0;
	mpo_View->mst_InstanceStat.i_MaxPlayingSNumber = 0;
	mpo_View->mst_InstanceStat.i_MaxSNumber = 0;

#ifdef SND_RASTER
	SND_gst_InstanceStat.ui_Current = 0;
	SND_gst_SInstanceStat.ui_Current = 0;
	SND_gst_InstanceStat.ui_Max = 0;
	SND_gst_SInstanceStat.ui_Max = 0;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnResetAutoOff(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(SND_gf_AutoVolumeOff) return;

	if(SND_gul_MainRefListSize)
	{
		LINK_PrintStatusMsg("One loaded sound bank is found, please close world and destroy all sound objects");
		return;
	}

	for(i = 0; i < SND_gst_Params.l_SoundNumber; i++)
	{
		if(SND_gst_Params.dst_Sound[i].ul_Flags & SND_Cul_DSF_Used)
		{
			LINK_PrintStatusMsg("One loaded sound is found, please close world and destroy all sound objects");
			return;
		}
	}

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		if(SND_gst_Params.dst_Instance[i].ul_Flags & SND_Cul_DSF_Used)
		{
			LINK_PrintStatusMsg("One sound instance is found, please close world and destroy all sound objects");
			return;
		}
	}

	SND_gf_AutoVolumeOff = 1.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnReinitEngine(void)
{
	if(mpo_View)
	{
		mpo_View->mst_InstanceStat.i_MaxNumber = 0;
		mpo_View->mst_InstanceStat.i_MaxPlayingNumber = 0;
		mpo_View->mst_InstanceStat.i_MaxPlayingSNumber = 0;
		mpo_View->mst_InstanceStat.i_MaxSNumber = 0;
	}

	SND_gb_EdiPause = FALSE;
	ediSND_gst_SoundManager.i_Playing = 0;
	ediSND_gst_SoundManager.i_PlayingMax = 0;
	ediSND_gst_SoundManager.i_SPlaying = 0;
	ediSND_gst_SoundManager.i_SPlayingMax = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnTimer(UINT ui)
{
    mpo_PaneSound->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnVolumeOff(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					l_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_Ini.i_Option ^= ESON_Cte_VolumeOff;

	if(mst_Ini.i_Option & ESON_Cte_VolumeOff)
	{
		SND_gf_AutoVolumeOff = 0.0f;
		mpo_DialogBar->GetDlgItem(IDC0)->SetWindowText("AutoVolume Off");
		ERR_X_Warning(0, "Your sound editor is in silent mode (see options).", NULL);
	}
	else
	{
		SND_gf_AutoVolumeOff = 1.0f;
		mpo_DialogBar->GetDlgItem(IDC0)->SetWindowText("");
	}

	for(l_Index = 0; l_Index < SND_gst_Params.l_InstanceNumber; l_Index++)
	{
		pst_SI = SND_gst_Params.dst_Instance + l_Index;
		if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		SND_SetInstVolume(pst_SI);
	}
}

#ifdef JADEFUSION
extern float	SND_f_RamRasterGetUsedRatio(void);
#else
extern "C" float	SND_f_RamRasterGetUsedRatio(void);
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnRefreshRaster(void)
{
/*
	CString o;


    o.Format("RAM fill rate : %.1f %%", 100.0f*SND_f_RamRasterGetUsedRatio());
    if(!mpo_DialogBar) return;
    if(!mpo_DialogBar->m_hWnd) return;

    mpo_PaneGrp->SetWindowText(o);
	mpo_PaneGrp->Invalidate();
	*/
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */

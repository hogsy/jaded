/*$T SONview.cpp GC 1.138 07/28/05 11:11:00 */


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

/*$2- base -----------------------------------------------------------------------------------------------------------*/

#include "BASe/BAStypes.h"

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "Res/Res.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDImsg.h"
#include "SELection/SELection.h"
#include "LINks/LINKmsg.h"
#include "Dialogs/DIAbase.h"
#include "Dialogs/DIAsndgroupwatcher_dlg.h"
#include "BROwser/BROframe.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/Sources/SND.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDspecific.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDconv_xboxadpcm.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDbank.h"

/*$2- sound edi ------------------------------------------------------------------------------------------------------*/

#include "SONframe.h"
#include "SONView.h"
#include "SONstrings.h"
#include "SONutil.h"
#include "SONpane.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    message map
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNCREATE(ESON_cl_View, CFormView)
BEGIN_MESSAGE_MAP(ESON_cl_View, CFormView)
	ON_WM_SIZE()
	ON_WM_DRAWITEM()
	ON_WM_PARENTNOTIFY()
	ON_COMMAND(IDC_BUTTON_SOLOOFF, OnSoloOffButton)
	ON_COMMAND(IDC_BUTTON_MUTEOFF, OnMuteOffButton)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_VIEW, OnChangeView)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_INSTANCE, OnSelInstance)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SOUND, OnSelSound)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BANK, OnSelBank)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_SOUND, OnColumnClickSound)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_BANK, OnColumnClickBank)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_INSTANCE, OnColumnClickInstance)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SOUND, OnRightClickSound)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_BANK, OnRightClickBank)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_INSTANCE, OnRightClickInstance)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_MUTE, OnRightClickInsert)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_GROUP, OnRightClickGroup)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    extern
 ***********************************************************************************************************************
 */

#define ESON_Cte_DisplayLineSize	11

#ifdef JADEFUSION
extern float	SND_gf_AutoVolumeOff;
extern void AI_AddWatch(void *p, int s);
extern void AI_DelAllWatch(void);
#else
extern "C" float	SND_gf_AutoVolumeOff;
extern "C" void AI_AddWatch(void *p, int s);
extern "C" void AI_DelAllWatch(void);
#endif

static int			i_SortList = 0;
static int			i_SortCol[4] = { 0, 0, 0 };
static CListCtrl	*po_SortList[5] = { NULL, NULL, NULL, NULL };

static char			*InstanceColumnName[] =
{
	"Owner",
	"Sound",
	"State",
	"Volume",
	"Flags",
	"Delay",
	"Track",
	"Channel",
	"Frequency",
	"FaderGroup",
	"Prefetch"
};

static int			InstanceFlags[] =
{
	-1,
	ESON_C_InstDisplaySound,
	ESON_C_InstDisplayState,
	ESON_C_InstDisplayVol,
	ESON_C_InstDisplayFlags,
	ESON_C_InstDisplayExtFlags,
	ESON_C_InstDisplayTrack,
	ESON_C_InstDisplayChannel,
	ESON_C_InstDisplayFrequency,
	ESON_C_InstDisplayFaderGroup,
	ESON_C_InstDisplayPrefetch
};

static char			*BankColumnName[] = { "Name", "Type", "Key", "Users", "SndNb", "*", "*", "*", "*", "*" };
static char			*SoundColumnName[] = { "Name", "Type", "Size", "Key", "Users", "*", "*", "*", "*", "*" };
static char			*InsertColumnName[] = { "Name", "Key", "Users", "Size", "*", "*", "*", "*", "*", "*" };

/*$4
 ***********************************************************************************************************************
    menbers
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_View::ESON_cl_View(ESON_cl_Frame *_po_Editor) :
	CFormView(ESON_IDD_LIBVIEW)
{
	mpo_Editor = _po_Editor;
	mi_FirstUpdate = 1;
	mi_Pane = 0;
	mst_InstanceStat.i_MaxNumber = 0;
	mst_InstanceStat.i_MaxPlayingNumber = 0;
	mst_InstanceStat.i_MaxPlayingSNumber = 0;
	mst_InstanceStat.i_MaxSNumber = 0;

	L_memset(&mst_CurrInstanceDisplay, 0, sizeof(ESON_tdst_ViewInstanceDisplay));
	L_memset(&mst_PrevInstanceDisplay, 0, sizeof(ESON_tdst_ViewInstanceDisplay));
	mb_ForceRefreshInstanceDisplay = FALSE;
	mb_RefreshInstanceDisplay = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_View::ESON_cl_View(void) :
	CFormView(ESON_IDD_LIBVIEW)
{
	ESON_cl_View(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_View::~ESON_cl_View(void)
{
	mo_ImageList.Detach();
	if(mst_CurrInstanceDisplay.ax_Data) delete mst_CurrInstanceDisplay.ax_Data;
	if(mst_PrevInstanceDisplay.ax_Data) delete mst_PrevInstanceDisplay.ax_Data;
	L_memset(&mst_CurrInstanceDisplay, 0, sizeof(ESON_tdst_ViewInstanceDisplay));
	L_memset(&mst_PrevInstanceDisplay, 0, sizeof(ESON_tdst_ViewInstanceDisplay));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_View::Create
(
	LPCTSTR		lpszClassName,
	LPCTSTR		lpszWindowName,
	DWORD		dwStyle,
	const RECT	&rect,
	CWnd		*pParentWnd,
	UINT		nID
)
{
	/*~~~~~~~~~~~~~~~*/
	CListCtrl	*po_LC;
	CBitmap		o_Bmp;
	/*~~~~~~~~~~~~~~~*/

	CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, NULL);

	mo_ImageList.Create(32, 16, ILC_COLOR16 | ILC_MASK, 0, 10);
	o_Bmp.LoadBitmap(MAKEINTRESOURCE(EDI_IDR_IMAGELIST2));
	mo_ImageList.Add(&o_Bmp, RGB(192, 192, 192));

	po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_INSTANCE);
	po_LC->SetImageList(&mo_ImageList, LVSIL_SMALL);
	EDIA_cl_BaseDialog::SetTheme(this);

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_View::b_InstanceIsFiltered(SND_tdst_SoundInstance *pst_SI, int *pi_playmode)
{
	/*~~~~~~~~~~~~~~*/
	int i_playingmode;
	/*~~~~~~~~~~~~~~*/

	if(pi_playmode) *pi_playmode = -1;
	if(!pst_SI) return TRUE;
	if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) return TRUE;

	if(pst_SI->pst_LI && pst_SI->pst_DSB)
	{
		if(pst_SI->ul_Flags & SND_Cul_SF_PlayingLoop)
		{
			i_playingmode = 2;
		}
		else if(pst_SI->ul_Flags & SND_Cul_SF_Playing)
		{
			i_playingmode = 1;
		}
		else
			i_playingmode = 0;
	}
	else
	{
		i_playingmode = -1;
	}

	if(pi_playmode) *pi_playmode = i_playingmode;

	if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyNonMdF)
	{
		if(pst_SI->ul_ExtFlags & SND_Cul_ESF_EdiMdFInstance) return TRUE;;
	}

	if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyMdF)
	{
		if(!(pst_SI->ul_ExtFlags & SND_Cul_ESF_EdiMdFInstance)) return TRUE;;
	}

	if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyPlaying)
	{
		if((i_playingmode == 0) || (i_playingmode == -1)) return TRUE;;
	}

	if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyPlayingLoop)
	{
		if(i_playingmode != 2) return TRUE;;
	}

	if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyStream)
	{
		if(!(pst_SI->ul_Flags & SND_Cte_StreamedFile)) return TRUE;;
	}

	if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyNonStream)
	{
		if(pst_SI->ul_Flags & SND_Cte_StreamedFile) return TRUE;;
	}

	if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyOnTrack)
	{
		if(pst_SI->i_InstTrack == -1) return TRUE;;
	}

	if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyNonOnTrack)
	{
		if(pst_SI->i_InstTrack != -1) return TRUE;;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::UpdateInstanceDisplay(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	SND_tdst_SoundInstance	*pSI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_CurrInstanceDisplay.i_Size >= SND_gst_Params.l_InstanceNumber)
	{
		for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
		{
			pSI = SND_gst_Params.dst_Instance + i;
			if(b_InstanceIsFiltered(pSI))
			{
				L_memset(&mst_CurrInstanceDisplay.ax_Data[i], 0, sizeof(SND_tdst_SoundInstance));
			}
			else
			{
				mst_CurrInstanceDisplay.ax_Data[i] = SND_gst_Params.dst_Instance[i];
			}
		}

		for(; i < mst_CurrInstanceDisplay.i_Size; i++)
		{
			L_memset(&mst_CurrInstanceDisplay.ax_Data[i], 0, sizeof(SND_tdst_SoundInstance));
		}

		if
		(
			memcmp
				(
					mst_CurrInstanceDisplay.ax_Data,
					mst_PrevInstanceDisplay.ax_Data,
					mst_CurrInstanceDisplay.i_Size * sizeof(SND_tdst_SoundInstance)
				)
		)
		{
			mb_RefreshInstanceDisplay = TRUE;
			L_memcpy
			(
				mst_PrevInstanceDisplay.ax_Data,
				mst_CurrInstanceDisplay.ax_Data,
				mst_CurrInstanceDisplay.i_Size * sizeof(SND_tdst_SoundInstance)
			);
		}
	}
	else
	{
		mb_RefreshInstanceDisplay = TRUE;

		if(mst_CurrInstanceDisplay.ax_Data) delete mst_CurrInstanceDisplay.ax_Data;
		mst_CurrInstanceDisplay.i_Size = SND_gst_Params.l_InstanceNumber;
		mst_CurrInstanceDisplay.ax_Data = new SND_tdst_SoundInstance[mst_CurrInstanceDisplay.i_Size];

		for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
		{
			pSI = SND_gst_Params.dst_Instance + i;
			if(b_InstanceIsFiltered(pSI))
			{
				L_memset(&mst_CurrInstanceDisplay.ax_Data[i], 0, sizeof(SND_tdst_SoundInstance));
			}
			else
			{
				mst_CurrInstanceDisplay.ax_Data[i] = SND_gst_Params.dst_Instance[i];
			}
		}

		if(mst_PrevInstanceDisplay.ax_Data) delete mst_PrevInstanceDisplay.ax_Data;
		mst_PrevInstanceDisplay.i_Size = mst_CurrInstanceDisplay.i_Size;
		mst_PrevInstanceDisplay.ax_Data = new SND_tdst_SoundInstance[mst_PrevInstanceDisplay.i_Size];
		L_memcpy
		(
			mst_PrevInstanceDisplay.ax_Data,
			mst_CurrInstanceDisplay.ax_Data,
			mst_PrevInstanceDisplay.i_Size * sizeof(SND_tdst_SoundInstance)
		);
	}

	if
	(
		(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayVol)
	&&	(mpo_Editor->mst_Ini.i_Option & ESON_Cte_DispEffVol)
	) mb_RefreshInstanceDisplay = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::FirstUpdateControls(void)
{
	/*~~~~~~~~~~~~~~~*/
	CListCtrl	*po_LC;
	CTabCtrl	*po_TC;
	int			i, j;
	char		az[64];
	/*~~~~~~~~~~~~~~~*/

	if(!mpo_Editor) return;
	if(mi_FirstUpdate) return;
	po_TC = (CTabCtrl *) GetDlgItem(IDC_TAB_VIEW);
	if(!po_TC) return;

	mi_FirstUpdate = 1;

	if(SND_gst_Params.l_Available)
	{
		GetDlgItem(IDC_STATIC_SOUNDSTATE)->ShowWindow(SW_HIDE);
		po_TC->ShowWindow(SW_SHOW);
		po_TC->DeleteAllItems();

		/*$2- bank ---------------------------------------------------------------------------------------------------*/

		po_TC->InsertItem(0, "Bank", 0);
		po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_BANK);
		po_LC->DeleteAllItems();

		for(i = 9; i >= 0; i--)
		{
			if(*BankColumnName[i] == '*') continue;
			po_LC->DeleteColumn(i);
		}

		for(j = i = 0; i < 10; i++)
		{
			if(*BankColumnName[i] == '*') continue;
			if(!mpo_Editor->mst_Ini.l_ListBankColumnWidth[i]) continue;

			po_LC->InsertColumn(j++, BankColumnName[i], LVCFMT_LEFT, mpo_Editor->mst_Ini.l_ListBankColumnWidth[i]);
		}

		po_SortList[0] = po_LC;

		/*$2- sound list ---------------------------------------------------------------------------------------------*/

		po_TC->InsertItem(1, "Sound", 0);
		po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_SOUND);
		po_LC->DeleteAllItems();

		for(i = 9; i >= 0; i--)
		{
			if(*SoundColumnName[i] == '*') continue;
			po_LC->DeleteColumn(i);
		}

		for(j = i = 0; i < 10; i++)
		{
			if(*SoundColumnName[i] == '*') continue;
			if(!mpo_Editor->mst_Ini.l_ListSoundColumnWidth[i]) continue;

			po_LC->InsertColumn(j++, SoundColumnName[i], LVCFMT_LEFT, mpo_Editor->mst_Ini.l_ListSoundColumnWidth[i]);
		}

		po_SortList[1] = po_LC;

		/*$2- instance list ------------------------------------------------------------------------------------------*/

		po_TC->InsertItem(2, "Instance", 0);
		po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_INSTANCE);

		for(i = 0; i < ESON_Cte_DisplayLineSize; i++)
		{
			po_LC->DeleteColumn(ESON_Cte_DisplayLineSize - 1 - i);
		}

		po_LC->InsertColumn(0, InstanceColumnName[0], LVCFMT_LEFT, mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[0]);
		for(j = i = 1; i < ESON_Cte_DisplayLineSize; i++)
		{
			if(mpo_Editor->mst_Ini.i_FilterInstance & InstanceFlags[i])
			{
				if(mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] <= 0)
					mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] = 64;
				if(mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] > 100000)
					mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] = 64;

				po_LC->InsertColumn
					(
						j++,
						InstanceColumnName[i],
						LVCFMT_LEFT,
						mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i]
					);
			}
			else
				mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] = 0;
		}

		po_SortList[2] = po_LC;

		/*$2- insert list --------------------------------------------------------------------------------------------*/

		po_TC->InsertItem(3, "Insert", 0);
		po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_MUTE);
		po_LC->DeleteAllItems();

		for(j = i = 0; i < 10; i++)
		{
			po_LC->DeleteColumn(9 - i);
		}

		for(j = i = 0; i < 10; i++)
		{
			if(*InsertColumnName[i] == '*') continue;
			if(!mpo_Editor->mst_Ini.l_ListMuteColumnWidth[i]) continue;
			po_LC->InsertColumn(j++, InsertColumnName[i], LVCFMT_LEFT, mpo_Editor->mst_Ini.l_ListMuteColumnWidth[i]);
		}

		po_SortList[3] = po_LC;

		/*$2- group --------------------------------------------------------------------------------------------*/

		po_TC->InsertItem(4, "Group", 0);
		po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_GROUP);
		po_LC->DeleteAllItems();

		for(j = i = 0; i < 34; i++)
		{
			po_LC->DeleteColumn(33 - i);
		}

		j=0;
		po_LC->InsertColumn(j++, "Name", LVCFMT_LEFT, 100);
		po_LC->InsertColumn(j++, "Vol", LVCFMT_LEFT, 50);
		for(i = 0; i < 32; i++)
		{
			sprintf(az, "send %d", i);
			po_LC->InsertColumn(j++, az, LVCFMT_LEFT, 50);
		}

		po_SortList[4] = po_LC;
	}
	else
	{
		if(SND_gc_NoSound)
		{
			((CStatic *) GetDlgItem(IDC_STATIC_SOUNDSTATE))->SetWindowText("\n\nNo Sound Mode");
		}
		else
		{
			((CStatic *) GetDlgItem(IDC_STATIC_SOUNDSTATE))->SetWindowText("No sound driver available\n\nReboot and play\nor close all your sound players\nand restart Jade");
		}

		GetDlgItem(IDC_STATIC_SOUNDSTATE)->ShowWindow(SW_SHOW);

		mpo_Editor->mpo_DialogBar->GetDlgItem(IDC0)->SetWindowText("No sound editor available");
		mpo_Editor->mpo_DialogBar->GetDlgItem(IDC1)->SetWindowText("No sound editor available");
		mpo_Editor->mpo_DialogBar->GetDlgItem(IDC2)->SetWindowText("No sound editor available");
		mpo_Editor->mpo_DialogBar->GetDlgItem(IDC3)->SetWindowText("No sound editor available");

		GetDlgItem(IDC_TAB_VIEW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_SOUND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_INSTANCE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_BANK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_MUTE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_GROUP)->ShowWindow(SW_HIDE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::UpdateControls(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	RECT		st_Rect, R;
	int			x, y, w, h;
	CTabCtrl	*po_TC;
	CButton		*po_Button;
	RECT		rec;
	int			wi, hi, wj, hj;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	GetClientRect(&st_Rect);

	po_TC = (CTabCtrl *) GetDlgItem(IDC_TAB_VIEW);
	if(!po_TC) return;

	//M_MF()->LockDisplay(this);

	mi_Pane = po_TC->GetCurFocus();
	FirstUpdateControls();

	if(SND_gst_Params.l_Available)
	{
	
			
			
			x = st_Rect.left + 2;
			y = st_Rect.top + 2;
			w = st_Rect.right - x - 2;
			h = st_Rect.bottom - y - 2;

			po_Button = (CButton *) GetDlgItem(IDC_BUTTON_MUTEOFF);
			po_Button->GetClientRect(&rec);
			wi = rec.right - rec.left;
			hi = rec.bottom - rec.top;
			if(w > (wi + 150))
				po_Button->SetWindowPos(NULL, x + w - wi, y, wi, hi, SWP_SHOWWINDOW | SWP_NOZORDER);
			else
				po_Button->SetWindowPos(NULL, x + w - wi, y, wi, hi, SWP_HIDEWINDOW | SWP_NOZORDER);

			po_Button = (CButton *) GetDlgItem(IDC_BUTTON_SOLOOFF);
			po_Button->GetClientRect(&rec);
			wj = rec.right - rec.left;
			hj = rec.bottom - rec.top;
			if(w > (wi + wj + 150))
				po_Button->SetWindowPos(NULL, x + w - wi - wj - 3, y, wj, hj, SWP_SHOWWINDOW | SWP_NOZORDER);
			else
				po_Button->SetWindowPos(NULL, x + w - wi - wj - 3, y, wj, hj, SWP_HIDEWINDOW | SWP_NOZORDER);

			po_TC->SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW | SWP_NOZORDER);
		
		

		po_Button = (CButton *) GetDlgItem(IDC_BUTTON_MUTEOFF);
		if(ESON_b_MuteIsActive() || ESON_b_MuteInstIsActive())
			po_Button->EnableWindow(TRUE);
		else
			po_Button->EnableWindow(FALSE);

		po_Button = (CButton *) GetDlgItem(IDC_BUTTON_SOLOOFF);
		if(ESON_b_SoloIsActive() || ESON_b_SoloInstIsActive())
			po_Button->EnableWindow(TRUE);
		else
			po_Button->EnableWindow(FALSE);


		x = st_Rect.left + 8;
		y = st_Rect.top + 30;
		w = st_Rect.right - x - 8;
		h = st_Rect.bottom - y - 8;

		switch(mi_Pane)
		{
		case 0:
			FillListBank();
			GetDlgItem(IDC_LIST_BANK)->SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER);
			GetDlgItem(IDC_LIST_BANK)->BringWindowToTop();
			break;

		case 1:
			FillListSound();
			GetDlgItem(IDC_LIST_SOUND)->SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER);
			GetDlgItem(IDC_LIST_SOUND)->BringWindowToTop();
			break;

		case 3:
			FillListInsert();
			GetDlgItem(IDC_LIST_MUTE)->SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER);
			GetDlgItem(IDC_LIST_MUTE)->BringWindowToTop();
			break;

		case 4:
			FillListGroup();
			GetDlgItem(IDC_LIST_GROUP)->SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER);
			GetDlgItem(IDC_LIST_GROUP)->BringWindowToTop();
			break;

		case 2:
		default:
			FillListInstance();
			GetDlgItem(IDC_LIST_INSTANCE)->SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER);
			GetDlgItem(IDC_LIST_INSTANCE)->BringWindowToTop();
			break;
		}

			GetDlgItem(IDC_LIST_BANK)->ShowWindow((mi_Pane == 0) ? SW_SHOW : SW_HIDE);
			GetDlgItem(IDC_LIST_SOUND)->ShowWindow((mi_Pane == 1) ? SW_SHOW : SW_HIDE);
			GetDlgItem(IDC_LIST_INSTANCE)->ShowWindow((mi_Pane == 2) ? SW_SHOW : SW_HIDE);
			GetDlgItem(IDC_LIST_MUTE)->ShowWindow((mi_Pane == 3) ? SW_SHOW : SW_HIDE);
			GetDlgItem(IDC_LIST_GROUP)->ShowWindow((mi_Pane == 4) ? SW_SHOW : SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_STATIC_SOUNDSTATE)->GetClientRect(&R);
		x = (st_Rect.right - R.right) / 2;
		y = (st_Rect.bottom - R.bottom) / 2;
		GetDlgItem(IDC_STATIC_SOUNDSTATE)->SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

//	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int CALLBACK si_CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char		az1[512];
	char		az2[512];
	int			i1, i2;
	float		f1, f2;
	LVFINDINFO	t;
	int			res1, res2;
	/*~~~~~~~~~~~~~~~~~~~*/

	t.flags = LVFI_PARAM;
	t.lParam = lParam1;
	res1 = po_SortList[i_SortList]->FindItem(&t);

	t.flags = LVFI_PARAM;
	t.lParam = lParam2;
	res2 = po_SortList[i_SortList]->FindItem(&t);

	po_SortList[i_SortList]->GetItemText(res1, i_SortCol[i_SortList], az1, 1024);
	po_SortList[i_SortList]->GetItemText(res2, i_SortCol[i_SortList], az2, 1024);

	if((az1[0] == '0') && (az1[1] == 'x'))
	{
		sscanf(az1, "0x%x", &i1);
		sscanf(az2, "0x%x", &i2);
		if(i1 < i2) return -1;
		if(i1 > i2) return 1;
		return 0;
	}

	if(L_isdigit(az1[0]) || az1[0] == '-')
	{
		sscanf(az1, "%f", &f1);
		sscanf(az2, "%f", &f2);
		if(f1 < f2) return -1;
		if(f1 > f2) return 1;
		return 0;
	}

	return L_strcmpi(az1, az2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::FillListSound(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl			*po_LC;
	SND_tdst_OneSound	*pst_Sound;
	char				sz_Item[BIG_C_MaxLenName];
	int					i, j, k;
	BIG_INDEX			ul_Fat;
	char				az[1024];
	char				az1[1024];
	char				*pz;
	int					scrollh, scrollv;
	BOOL				ok;
	CRect				rect;
	int					wav, wad, wam, waa, smd, wac;
	int					snd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Pane != 1) return;

	po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_SOUND);
	po_LC->SetRedraw(FALSE);
	scrollh = po_LC->GetScrollPos(SB_HORZ);
	scrollv = po_LC->GetScrollPos(SB_VERT);

	po_LC->DeleteAllItems();
	ok = FALSE;
	wac = wav = wad = wam = waa = smd = 0;
	snd = 0;

	for(i = 0, j = 0; i < SND_gst_Params.l_SoundNumber; i++)
	{
		pst_Sound = SND_gst_Params.dst_Sound + i;

		if(!(pst_Sound->ul_Flags & SND_Cul_DSF_Used)) continue;

		ul_Fat = BIG_ul_SearchKeyToFat(pst_Sound->ul_FileKey);
		if(ul_Fat == -1) continue;

		snd++;

		/* Filters */
		L_strcpy(az1, BIG_NameFile(ul_Fat));
		pz = L_strrchr(az1, '.');
		if(pz)
		{
			*pz = 0;
			pz++;

			if(pst_Sound->ul_Flags & SND_Cul_SF_Ambience)
			{
				waa++;
				if(mpo_Editor->mst_Ini.i_FiltersSound & 1) goto ok;
			}
			else if(pst_Sound->ul_Flags & SND_Cul_SF_Dialog)
			{
				wad++;
				if(mpo_Editor->mst_Ini.i_FiltersSound & 2) goto ok;
			}
			else if(pst_Sound->ul_Flags & SND_Cul_SF_Music)
			{
				wam++;
				if(mpo_Editor->mst_Ini.i_FiltersSound & 4) goto ok;
			}
			else if(pst_Sound->ul_Flags & SND_Cul_SF_LoadingSound)
			{
				wac++;
				if(mpo_Editor->mst_Ini.i_FiltersSound & 16) goto ok;
			}
			else if(pst_Sound->ul_Flags & SND_Cul_SF_SModifier)
			{
				smd++;
				if(mpo_Editor->mst_Ini.i_FiltersSound & 32) goto ok;
			}
			else
			{
				wav++;
				if(mpo_Editor->mst_Ini.i_FiltersSound & 8) goto ok;
			}

			j++;
			continue;
		}

ok:
		sprintf(az, "%4d  %s", i, az1);
		k = po_LC->InsertItem(j, az);
		po_LC->SetItemData(k, i);

		if(pz)
		{
			sprintf(az, "%s", pz);
			po_LC->SetItem(k, 1, LVIF_TEXT, az, 0, 0, 0, 0);
		}

		if(pst_Sound->pst_Wave == NULL)
		{
			po_LC->SetItem(k, 2, LVIF_TEXT, "- x -", 0, 0, 0, 0);
		}
		else if(pst_Sound->pst_Wave->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
		{
			sprintf(sz_Item, "%d decomp.", SND_ui_GetDecompressedSize(pst_Sound->pst_Wave->ul_DataSize));
			po_LC->SetItem(k, 2, LVIF_TEXT, sz_Item, 0, 0, 0, 0);
		}
		else
		{
			po_LC->SetItem(k, 2, LVIF_TEXT, _itoa(pst_Sound->pst_Wave->ul_DataSize, sz_Item, 10), 0, 0, 0, 0);
		}

		sprintf(az, "0x%x", pst_Sound->ul_FileKey);
		po_LC->SetItem(k, 3, LVIF_TEXT, az, 0, 0, 0, 0);

		po_LC->SetItem(k, 4, LVIF_TEXT, _itoa(pst_Sound->ul_CptUsed, sz_Item, 10), 0, 0, 0, 0);

		ok = TRUE;

		j++;
	}

	i_SortList = 1;
	po_SortList[1] = po_LC;
	po_LC->SortItems(si_CompareFunc, 0);
	po_LC->SetRedraw(TRUE);
	if(ok)
	{
		po_LC->GetItemRect(0, &rect, LVIR_BOUNDS);
		po_LC->Scroll(CSize(scrollh, scrollv * rect.Height()));
	}

	if(SND_gf_AutoVolumeOff)
	{
		sprintf
		(
			az,
			"Sound files %d - wav %d / wad %d / wam %d / waa %d / smd %d / wac %d",
			snd,
			wav,
			wad,
			wam,
			waa,
			smd,
			wac
		);
	}
	else
	{
		sprintf
		(
			az,
			"AutoVolume Off    - Sound files %d - wav %d / wad %d / wam %d / waa %d / smd %d / wac %d",
			snd,
			wav,
			wad,
			wam,
			waa,
			smd,
			wac
		);
	}

	mpo_Editor->mpo_PaneList->SetWindowText(az);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::FillListInsert(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl		*po_LC;
	SND_tdst_Insert *pst_Insert;
	char			sz_Item[BIG_C_MaxLenName];
	int				i, j, k;
	BIG_INDEX		ul_Fat;
	char			az[1024];
	char			az1[1024];
	int				scrollh, scrollv;
	BOOL			ok;
	CRect			rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Pane != 3) return;

	po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_MUTE);
	po_LC->SetRedraw(FALSE);
	scrollh = po_LC->GetScrollPos(SB_HORZ);
	scrollv = po_LC->GetScrollPos(SB_VERT);
	ok = FALSE;

	po_LC->DeleteAllItems();
	for(i = 0, j = 0; i < SND_gst_Params.l_InsertNumber; i++)
	{
		pst_Insert = SND_gst_Params.dst_Insert + i;
		if(!(pst_Insert->ul_Flags & SND_Cul_DSF_Used)) break;

		ul_Fat = BIG_ul_SearchKeyToFat(pst_Insert->ul_FileKey);
		if(ul_Fat == -1) continue;

		L_strcpy(az1, BIG_NameFile(ul_Fat));

		sprintf(az, "%4d  %s", i, az1);

		k = po_LC->InsertItem(j, az);
		po_LC->SetItemData(k, i);

		sprintf(az, "0x%x", pst_Insert->ul_FileKey);
		po_LC->SetItem(k, 1, LVIF_TEXT, az, 0, 0, 0, 0);
		po_LC->SetItem(k, 2, LVIF_TEXT, _itoa(pst_Insert->ul_UserCount, sz_Item, 10), 0, 0, 0, 0);
		po_LC->SetItem(k, 3, LVIF_TEXT, _itoa(BIG_LengthDiskFile(ul_Fat), sz_Item, 10), 0, 0, 0, 0);

		ok = TRUE;

		j++;
	}

	i_SortList = 0;
	po_SortList[0] = po_LC;
	po_LC->SortItems(si_CompareFunc, 0);
	po_LC->SetRedraw(TRUE);
	if(ok)
	{
		po_LC->GetItemRect(0, &rect, LVIR_BOUNDS);
		po_LC->Scroll(CSize(scrollh, scrollv * rect.Height()));
	}

	sprintf(az, "Insert files %d", j);
	mpo_Editor->mpo_PaneList->SetWindowText(az);
}

char* gaz_GroupName[SND_e_GrpNumber] = {
	"SpecialFX",
	"Music",
	"Ambience",
	"Dialog",
	"CutScene",
	"Interface",
	"A",
	"B",
	"Master",
	"UserMusic",
	"UserDialog",
	"UserSpecialFX",
	"UserMaster"
};

char gaz_LastGroupDisplay[SND_e_GrpNumber][32][8] = {0};
char gaz_CurrGroupDisplay[SND_e_GrpNumber][32][8] = {0};


void ESON_cl_View::FillListGroup(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl		*po_LC;
	SND_tdst_Group *pGroup;
	int				i, j, k;
	char			az[1024];
	int				scrollh, scrollv;
	CRect			rect;
	static int		first=1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Pane != 4) return;

	if(first)
		L_memset(gaz_LastGroupDisplay, 0, sizeof(gaz_LastGroupDisplay));
	first = 0;
	

	for(i = 0; i < SND_e_GrpNumber; i++)
	{
		pGroup = SND_gdst_Group + i;

		sprintf(gaz_CurrGroupDisplay[i][0], "%.2f", pGroup->af_VolRq[0]);
		for(j=1; j<32; j++)
		{
			if(pGroup->ul_VolRqFlags & (1<<j))
				sprintf(gaz_CurrGroupDisplay[i][j], "%.2f", pGroup->af_VolRq[j]);
			else
				sprintf(gaz_CurrGroupDisplay[i][j], "-");
		}
	}
	
	if(!memcmp(gaz_CurrGroupDisplay, gaz_LastGroupDisplay, sizeof(gaz_LastGroupDisplay))) 
		return;
	L_memcpy(gaz_LastGroupDisplay, gaz_CurrGroupDisplay, sizeof(gaz_LastGroupDisplay)); 


	po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_GROUP);
	po_LC->SetRedraw(FALSE);
	scrollh = po_LC->GetScrollPos(SB_HORZ);
	scrollv = po_LC->GetScrollPos(SB_VERT);
	po_LC->DeleteAllItems();

	for(i = 0; i < SND_e_GrpNumber; i++)
	{
		if((mpo_Editor->mi_FiltersGroup & (1<<i)) == 0) continue;
		pGroup = SND_gdst_Group + i;


		k = po_LC->InsertItem(j, gaz_GroupName[i]);
		po_LC->SetItemData(k, i);

		sprintf(az, "%.2f", pGroup->f_Volume);
		po_LC->SetItem(k, 1, LVIF_TEXT, az, 0, 0, 0, 0);

		for(j=0; j<32; j++)
			po_LC->SetItem(k, 2+j, LVIF_TEXT, gaz_CurrGroupDisplay[i][j], 0, 0, 0, 0);
	}

	po_LC->SetRedraw(TRUE);

	po_LC->GetItemRect(0, &rect, LVIR_BOUNDS);
	po_LC->Scroll(CSize(scrollh, scrollv * rect.Height()));

	sprintf(az, "-");
	mpo_Editor->mpo_PaneList->SetWindowText(az);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::FillListBank(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl		*po_LC;
	SND_tdun_Main	*pst_Sound;
	char			sz_Item[BIG_C_MaxLenName];
	int				i, j, k;
	BIG_INDEX		ul_Fat;
	char			az[1024];
	char			az1[1024];
	int				scrollh, scrollv;
	BOOL			ok;
	CRect			rect;
	int				snk, msk;
	char			*pz;
	SND_tdst_Bank	*pBank;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Pane != 0) return;

	po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_BANK);
	po_LC->SetRedraw(FALSE);
	scrollh = po_LC->GetScrollPos(SB_HORZ);
	scrollv = po_LC->GetScrollPos(SB_VERT);
	ok = FALSE;

	msk = snk = 0;
	po_LC->DeleteAllItems();
	for(i = 0, j = 0; i < (int) SND_gul_MainRefListSize; i++)
	{
		pst_Sound = SND_gap_MainRefList[i];
		pBank = SND_p_MainGetBank(pst_Sound);

		ul_Fat = BIG_ul_SearchKeyToFat(pst_Sound->st_Bank.ul_FileKey);
		if(ul_Fat == -1) continue;

		L_strcpy(az1, BIG_NameFile(ul_Fat));
		pz = L_strrchr(az1, '.');
		if(!L_strnicmp(pz, EDI_Csz_ExtSoundBank, 4))
		{
			snk++;
		}
		else
		{
			msk++;
		}

		if(pz) *pz = 0;

		sprintf(az, "%4d  %s", i, az1);

		k = po_LC->InsertItem(j, az);
		po_LC->SetItemData(k, i);

		sprintf(az, "0x%x", pst_Sound->st_Bank.ul_FileKey);

		if(pz) po_LC->SetItem(k, 1, LVIF_TEXT, pz + 1, 0, 0, 0, 0);
		po_LC->SetItem(k, 2, LVIF_TEXT, az, 0, 0, 0, 0);
		po_LC->SetItem(k, 3, LVIF_TEXT, _itoa(pst_Sound->st_Bank.ul_UserCount, sz_Item, 10), 0, 0, 0, 0);
		po_LC->SetItem(k, 4, LVIF_TEXT, _itoa(pBank->i_SoundNb, sz_Item, 10), 0, 0, 0, 0);

		ok = TRUE;

		j++;
	}

	i_SortList = 0;
	po_SortList[0] = po_LC;
	po_LC->SortItems(si_CompareFunc, 0);
	po_LC->SetRedraw(TRUE);
	if(ok)
	{
		po_LC->GetItemRect(0, &rect, LVIR_BOUNDS);
		po_LC->Scroll(CSize(scrollh, scrollv * rect.Height()));
	}

	if(SND_gf_AutoVolumeOff)
		sprintf(az, "Bank files %d / Meta bank files %d", snk, msk);
	else
		sprintf(az, "AutoVolume Off    - Bank files %d / Meta bank files %d", snk, msk);

	mpo_Editor->mpo_PaneList->SetWindowText(az);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::FillListInstance(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	char					sz_Item[BIG_C_MaxLenName];
	int						i, j, k;
	char					az[1024];
	int						scrollh, scrollv;
	BOOL					ok;
	CRect					rect;
	int						i_playing, i_Splaying;
	int						i_nb, i_Snb;
	int						i_playingmode;
	BOOL					b_FilterIsActive;
	int						col;
	int						iMage, channel;
	BOOL					b_FilterInst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Pane < 2) return;
	if(mpo_Editor->mst_Ini.i_Option & ESON_Cte_DispEffVol) mb_ForceRefreshInstanceDisplay = TRUE;

	UpdateInstanceDisplay();

	if(!mb_RefreshInstanceDisplay && !mb_ForceRefreshInstanceDisplay) return;
	mb_RefreshInstanceDisplay = FALSE;
	mb_ForceRefreshInstanceDisplay = FALSE;

	po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_INSTANCE);
	po_LC->SetRedraw(FALSE);
	scrollh = po_LC->GetScrollPos(SB_HORZ);
	scrollv = po_LC->GetScrollPos(SB_VERT);
	ok = FALSE;
	po_LC->DeleteAllItems();

	i_nb = i_Snb = 0;
	col = 0;
	i_playing = i_Splaying = 0;
	b_FilterIsActive = FALSE;

	for(i = 0, j = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;

		if(pst_SI->ul_Flags & SND_Cte_StreamedFile)
			i_Snb++;
		else
			i_nb++;

		b_FilterInst = b_InstanceIsFiltered(pst_SI, &i_playingmode);
		channel = SND_gst_Params.dst_Sound[pst_SI->l_Sound].pst_Wave->wChannels;
		switch(i_playingmode)
		{
		case 2:
			if(pst_SI->ul_Flags & SND_Cte_StreamedFile)
				i_Splaying += channel;
			else
				i_playing += channel;
			break;

		case 1:
			if(pst_SI->ul_Flags & SND_Cte_StreamedFile)
				i_Splaying += channel;
			else
				i_playing += channel;
			break;

		case 0:
			break;

		default:
			break;
		}

		/*$1- filters ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(b_FilterInst)
		{
			b_FilterIsActive = TRUE;
			continue;
		}

		/*$1- GAO name ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(pst_SI->p_GameObject)
		{
			if(((OBJ_tdst_GameObject *) pst_SI->p_GameObject)->sz_Name)
				sprintf(az, "%3d  %s", i, ((OBJ_tdst_GameObject *) pst_SI->p_GameObject)->sz_Name);
			else
				sprintf(az, "%3d  None", i);
		}
		else
		{
			sprintf(az, "%3d  None", i);
		}

		/*$1- choose the image ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		iMage = i_playingmode ? 0 : 1;

		if(pst_SI->ul_EdiFlags & SND_EdiFlg_Mute)
			iMage += 2;
		else if(pst_SI->ul_EdiFlags & SND_EdiFlg_Solo)
			iMage += 4;
		else if(GetAsyncKeyState(VK_SHIFT) < 0)
			iMage += 6;

		if(pst_SI->ul_EdiFlags & SND_EdiFlg_EdiColor1)
			iMage += 8;
		else if(pst_SI->ul_EdiFlags & SND_EdiFlg_EdiColor2)
			iMage += 16;
		else if(pst_SI->ul_EdiFlags & SND_EdiFlg_EdiColor3)
			iMage += 24;

		/*$1- insert the item ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		k = po_LC->InsertItem(j, az, iMage);
		po_LC->SetItemData(k, i);
		col = 1;

		/*$1- sound number ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplaySound)
		{
			/*~~~~~~~~~~*/
			ULONG	ulfat;
			/*~~~~~~~~~~*/

			ulfat = BIG_ul_SearchKeyToFat(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey);

			if(ulfat == BIG_C_InvalidIndex)
				po_LC->SetItem(k, col++, LVIF_TEXT, _itoa(pst_SI->l_Sound, sz_Item, 10), 0, 0, 0, 0);
			else
				po_LC->SetItem(k, col++, LVIF_TEXT, BIG_NameFile(ulfat), 0, 0, 0, 0);
		}

		/*$1- playing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayState)
		{
			switch(i_playingmode)
			{
			case 0:		sprintf(az, "stopped"); break;
			case 1:		sprintf(az, "playing"); break;
			case 2:		sprintf(az, "playing loop"); break;
			default:	sprintf(az, "released"); break;
			}

			po_LC->SetItem(k, col++, LVIF_TEXT, az, 0, 0, 0, 0);
		}

		/*$1- volume ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayVol)
		{
			if(mpo_Editor->mst_Ini.i_Option & ESON_Cte_DispEffVol)
			{
				sprintf(sz_Item, "[%1.2f]", SND_f_GetInstVolume(pst_SI));
			}
			else
				sprintf(sz_Item, "%1.2f", pst_SI->f_Volume);
			po_LC->SetItem(k, col++, LVIF_TEXT, sz_Item, 0, 0, 0, 0);
		}

		/*$1- flags ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayFlags)
		{
			sprintf(sz_Item, "0x%08X", pst_SI->ul_Flags);
			po_LC->SetItem(k, col++, LVIF_TEXT, sz_Item, 0, 0, 0, 0);
		}

		/*$1- ext flag ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayExtFlags)
		{
			sprintf(sz_Item, "%.3f", pst_SI->f_Delay);
			po_LC->SetItem(k, col++, LVIF_TEXT, sz_Item, 0, 0, 0, 0);
		}

		/*$1- track ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayTrack)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			char	*track[] = { "A", "B", "C", "D" };
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			sprintf(sz_Item, "%s", pst_SI->i_InstTrack == -1 ? "none" : track[pst_SI->i_InstTrack]);
			po_LC->SetItem(k, col++, LVIF_TEXT, sz_Item, 0, 0, 0, 0);
		}

		/*$1- channel ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayChannel)
		{
			sprintf(sz_Item, "%d", SND_gst_Params.dst_Sound[pst_SI->l_Sound].pst_Wave->wChannels);
			po_LC->SetItem(k, col++, LVIF_TEXT, sz_Item, 0, 0, 0, 0);
		}

		/*$1- frequency ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayFrequency)
		{
			if((int) SND_gst_Params.dst_Sound[pst_SI->l_Sound].pst_Wave->dwSamplesPerSec != pst_SI->i_BaseFrequency)
				sprintf(sz_Item, "%d*", pst_SI->i_BaseFrequency);
			else
				sprintf(sz_Item, "%d", pst_SI->i_BaseFrequency);

			po_LC->SetItem(k, col++, LVIF_TEXT, sz_Item, 0, 0, 0, 0);
		}

		/*$1- fadergroup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayFaderGroup)
		{
			switch(SND_M_GetGrp(pst_SI->i_GroupId))
			{
			case SND_Cte_GrpMusic:		sprintf(sz_Item, "music"); break;
			case SND_Cte_GrpDialog:		sprintf(sz_Item, "dialog"); break;
			case SND_Cte_GrpSpecialFX:	sprintf(sz_Item, "SFX"); break;
			case SND_Cte_GrpAmbience:	sprintf(sz_Item, "ambience"); break;
			case SND_Cte_GrpCutScene:	sprintf(sz_Item, "cut-scene"); break;
			case SND_Cte_GrpInterface:	sprintf(sz_Item, "interface"); break;
			default:					sprintf(sz_Item, "unknown"); break;
			}

			if(pst_SI->i_GroupId & SND_Cte_GrpA) strcat(sz_Item, " + A");
			if(pst_SI->i_GroupId & SND_Cte_GrpB) strcat(sz_Item, " + B");

			po_LC->SetItem(k, col++, LVIF_TEXT, sz_Item, 0, 0, 0, 0);
		}

		/*$1- prefetch ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayPrefetch)
		{
			if(pst_SI->ul_EdiFlags & SND_EdiFlg_EdiPrefetching)
				po_LC->SetItem(k, col++, LVIF_TEXT, "Prefetching", 0, 0, 0, 0);
			else if(pst_SI->ul_EdiFlags & SND_EdiFlg_EdiPrefetched)
				po_LC->SetItem(k, col++, LVIF_TEXT, "Prefetched", 0, 0, 0, 0);
			else if(pst_SI->ul_EdiFlags & SND_EdiFlg_EdiPrefetchedUsed)
				po_LC->SetItem(k, col++, LVIF_TEXT, "Used", 0, 0, 0, 0);
			else
				po_LC->SetItem(k, col++, LVIF_TEXT, "-", 0, 0, 0, 0);
		}

		j++;
		ok = TRUE;
	}

	if(mst_InstanceStat.i_MaxNumber < i_nb) mst_InstanceStat.i_MaxNumber = i_nb;
	if(mst_InstanceStat.i_MaxSNumber < i_Snb) mst_InstanceStat.i_MaxSNumber = i_Snb;

#ifdef SND_RASTER
	i_nb = SND_gst_InstanceStat.ui_Current;
	i_Snb = SND_gst_SInstanceStat.ui_Current;
	mst_InstanceStat.i_MaxNumber = SND_gst_InstanceStat.ui_Max;
	mst_InstanceStat.i_MaxSNumber = SND_gst_SInstanceStat.ui_Max;
#endif
	i_playing = ediSND_gst_SoundManager.i_Playing;
	i_Splaying = ediSND_gst_SoundManager.i_SPlaying;
	mst_InstanceStat.i_MaxPlayingNumber = ediSND_gst_SoundManager.i_PlayingMax;
	mst_InstanceStat.i_MaxPlayingSNumber = ediSND_gst_SoundManager.i_SPlayingMax;

	if(SND_gf_AutoVolumeOff)
	{
		sprintf
		(
			sz_Item,
			"Instances %d+S%d - Max %d+S%d - playing %d+S%d - Max playing %d+S%d",
			i_nb,
			i_Snb,
			mst_InstanceStat.i_MaxNumber,
			mst_InstanceStat.i_MaxSNumber,
			i_playing,
			i_Splaying,
			mst_InstanceStat.i_MaxPlayingNumber,
			mst_InstanceStat.i_MaxPlayingSNumber
		);
	}
	else
	{
		sprintf
		(
			sz_Item,
			"AutoVolume Off    - Instances %d+S%d - playing %d+S%d - Max %d+S%d - Max playing %d+S%d",
			i_nb,
			i_Snb,
			mst_InstanceStat.i_MaxNumber,
			mst_InstanceStat.i_MaxSNumber,
			i_playing,
			i_Splaying,
			mst_InstanceStat.i_MaxPlayingNumber,
			mst_InstanceStat.i_MaxPlayingSNumber
		);
	}

#ifndef SND_RASTER
	strcat(sz_Item, "  - Estimation");
#endif
	if(b_FilterIsActive) strcat(sz_Item, "  - Display filter");

	mpo_Editor->mpo_PaneList->SetWindowText(sz_Item);

	i_SortList = 2;
	po_SortList[2] = po_LC;

	po_LC->SortItems(si_CompareFunc, 0);
	po_LC->SetRedraw(TRUE);

	if(ok)
	{
		po_LC->GetItemRect(0, &rect, LVIR_BOUNDS);
		po_LC->Scroll(CSize(scrollh, scrollv * rect.Height()));
	}
}

/*$4
 ***********************************************************************************************************************
    Message functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_View::PreTranslateMessage(MSG *pMsg)
{
	/*~~~~~~~~~~~~~~~~*/
	CListCtrl	*po_LC;
	CPoint		o_Point;
	int			i_Item;
	CMenu		o_Menu;
	CPoint		pt;
	/*~~~~~~~~~~~~~~~~*/

	if(pMsg->hwnd == GetDlgItem(IDC_LIST_SOUND)->GetSafeHwnd())
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		 ---------------------------------------------------------------------------------------------------------------
		 */

		switch(pMsg->message)
		{
		case WM_LBUTTONDOWN:
			po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_SOUND);
			o_Point.x = 5;
			o_Point.y = HIWORD(pMsg->lParam);
			i_Item = po_LC->HitTest(o_Point);
			if(i_Item != -1) SetCurSel(IDC_LIST_SOUND, i_Item);
			return 1;

		case WM_RBUTTONDOWN:
			po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_SOUND);
			o_Point.x = 5;
			o_Point.y = HIWORD(pMsg->lParam);
			i_Item = po_LC->HitTest(o_Point);
			if(i_Item != -1) SetCurSel(IDC_LIST_SOUND, i_Item);
			break;

		default:
			break;
		}
	}

	if(pMsg->hwnd == GetDlgItem(IDC_LIST_BANK)->GetSafeHwnd())
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		 ---------------------------------------------------------------------------------------------------------------
		 */

		switch(pMsg->message)
		{
		case WM_LBUTTONDOWN:
			po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_BANK);
			o_Point.x = 5;
			o_Point.y = HIWORD(pMsg->lParam);
			i_Item = po_LC->HitTest(o_Point);
			if(i_Item != -1) SetCurSel(IDC_LIST_BANK, i_Item);
			return 1;

		case WM_RBUTTONDOWN:
			po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_BANK);
			o_Point.x = 5;
			o_Point.y = HIWORD(pMsg->lParam);
			i_Item = po_LC->HitTest(o_Point);
			if(i_Item != -1) SetCurSel(IDC_LIST_BANK, i_Item);
			break;

		default:
			break;
		}
	}

	if(pMsg->hwnd == GetDlgItem(IDC_LIST_INSTANCE)->GetSafeHwnd())
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		 ---------------------------------------------------------------------------------------------------------------
		 */

		switch(pMsg->message)
		{
		case WM_LBUTTONDOWN:
			po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_INSTANCE);
			o_Point.x = 5;
			o_Point.y = HIWORD(pMsg->lParam);
			i_Item = po_LC->HitTest(o_Point);
			if(i_Item != -1)
			{
				SetCurSel(IDC_LIST_INSTANCE, i_Item);

				if(LOWORD(pMsg->lParam) < 16)
				{
					if(GetAsyncKeyState(VK_SHIFT) < 0)
						mpo_Editor->Instance_OnSetMute();
					else
						mpo_Editor->Instance_OnSetSolo();
				}
				else if(LOWORD(pMsg->lParam) < 32)
				{
					if(SND_gst_Params.dst_Instance[po_LC->GetItemData(i_Item)].ul_EdiFlags & SND_EdiFlg_EdiColor1)
					{
						SND_gst_Params.dst_Instance[po_LC->GetItemData(i_Item)].ul_EdiFlags &= ~SND_EdiFlg_EdiColor1;
						SND_gst_Params.dst_Instance[po_LC->GetItemData(i_Item)].ul_EdiFlags |= SND_EdiFlg_EdiColor2;
					}
					else if(SND_gst_Params.dst_Instance[po_LC->GetItemData(i_Item)].ul_EdiFlags & SND_EdiFlg_EdiColor2)
					{
						SND_gst_Params.dst_Instance[po_LC->GetItemData(i_Item)].ul_EdiFlags &= ~SND_EdiFlg_EdiColor2;
						SND_gst_Params.dst_Instance[po_LC->GetItemData(i_Item)].ul_EdiFlags |= SND_EdiFlg_EdiColor3;
					}
					else if(SND_gst_Params.dst_Instance[po_LC->GetItemData(i_Item)].ul_EdiFlags & SND_EdiFlg_EdiColor3)
					{
						SND_gst_Params.dst_Instance[po_LC->GetItemData(i_Item)].ul_EdiFlags &= ~SND_EdiFlg_EdiColor3;
					}
					else
						SND_gst_Params.dst_Instance[po_LC->GetItemData(i_Item)].ul_EdiFlags |= SND_EdiFlg_EdiColor1;
					UpdateControls();
				}

				return 1;
			}
			break;

		case WM_LBUTTONDBLCLK:
			po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_INSTANCE);
			o_Point.x = 5;
			o_Point.y = HIWORD(pMsg->lParam);
			i_Item = po_LC->HitTest(o_Point);
			if(i_Item != -1)
			{
				SetCurSel(IDC_LIST_INSTANCE, i_Item);

				if(LOWORD(pMsg->lParam) > 32)
				{
					if(mpo_Editor->b_OnActionValidate(ESON_ACTION_INST_EDITSMD))
						mpo_Editor->OnAction(ESON_ACTION_INST_EDITSMD);
					else if(mpo_Editor->b_OnActionValidate(ESON_ACTION_INST_EDITSOUND))
						mpo_Editor->OnAction(ESON_ACTION_INST_EDITSOUND);
				}

				return 1;
			}
			break;

		case WM_RBUTTONDOWN:
			po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_INSTANCE);
			o_Point.x = 5;
			o_Point.y = HIWORD(pMsg->lParam);
			i_Item = po_LC->HitTest(o_Point);
			if(i_Item != -1) SetCurSel(IDC_LIST_INSTANCE, i_Item);
			break;

		default:
			break;
		}
	}

	if(pMsg->hwnd == GetDlgItem(IDC_LIST_MUTE)->GetSafeHwnd())
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		 ---------------------------------------------------------------------------------------------------------------
		 */

		switch(pMsg->message)
		{
		case WM_LBUTTONDBLCLK:
			po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_MUTE);
			o_Point.x = 5;
			o_Point.y = HIWORD(pMsg->lParam);
			i_Item = po_LC->HitTest(o_Point);
			if(i_Item != -1)
			{
				/*~~~~~~~~~~*/
				ULONG	ulFat;
				/*~~~~~~~~~~*/

				SetCurSel(IDC_LIST_MUTE, i_Item);
				ulFat = BIG_ul_SearchKeyToFat(SND_gst_Params.dst_Insert[po_LC->GetItemData(i_Item)].ul_FileKey);

				if(mpo_Editor->b_OnActionValidate(ESON_ACTION_INS_OPEN)) mpo_Editor->Insert_OnOpen(ulFat);

				return 1;
			}
			break;

		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
			po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_MUTE);
			o_Point.x = 5;
			o_Point.y = HIWORD(pMsg->lParam);
			i_Item = po_LC->HitTest(o_Point);
			if(i_Item != -1) SetCurSel(IDC_LIST_MUTE, i_Item);
			break;

		default:
			break;
		}
	}
	if(pMsg->hwnd == GetDlgItem(IDC_LIST_GROUP)->GetSafeHwnd())
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		 ---------------------------------------------------------------------------------------------------------------
		 */

		switch(pMsg->message)
		{
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
			po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_GROUP);
			o_Point.x = 5;
			o_Point.y = HIWORD(pMsg->lParam);
			i_Item = po_LC->HitTest(o_Point);
			if(i_Item != -1) SetCurSel(IDC_LIST_GROUP, i_Item);
			break;

		default:
			break;
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnSize(UINT nType, int cx, int cy)
{
	if(mpo_Editor)
	{
		UpdateControls();
		SetScrollSizes(MM_TEXT, CSize(0, 0));
	}

	CFormView::OnSize(nType, cx, cy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnChangeView(NMHDR *, LRESULT *)
{
	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_View::i_GetCurSel(UINT _ui_LC)
{
	/*~~~~~~~~~~~~~~~*/
	POSITION	pos;
	CListCtrl	*po_LC;
	/*~~~~~~~~~~~~~~~*/

	po_LC = (CListCtrl *) GetDlgItem(_ui_LC);
	pos = po_LC->GetFirstSelectedItemPosition();
	if(!pos) return -1;
	return po_LC->GetNextSelectedItem(pos);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::SetCurSel(UINT _ui_LC, int _i_Index, BOOL _b_Data)
{
	/*~~~~~~~~~~~~~~~*/
	CListCtrl	*po_LC;
	POSITION	pos;
	int			i_Item;
	int			i_Data;
	BOOL		b_Done;
	/*~~~~~~~~~~~~~~~*/

	b_Done = FALSE;

	po_LC = (CListCtrl *) GetDlgItem(_ui_LC);
	pos = po_LC->GetFirstSelectedItemPosition();
	while(pos)
	{
		i_Item = po_LC->GetNextSelectedItem(pos);
		if(_b_Data)
		{
			i_Data = po_LC->GetItemData(i_Item);
			if(_i_Index == i_Data)
				b_Done = TRUE;
			else
				po_LC->SetItemState(i_Item, LVIS_SELECTED | LVIS_ACTIVATING, 0);
		}
		else
		{
			if(i_Item == _i_Index)
				b_Done = TRUE;
			else
				po_LC->SetItemState(i_Item, LVIS_SELECTED | LVIS_ACTIVATING, 0);
		}
	}

	if((b_Done) || (_i_Index == -1)) return;

	if(_b_Data)
	{
		for(i_Item = 0; i_Item < po_LC->GetItemCount(); i_Item++)
		{
			if(_i_Index == (int) po_LC->GetItemData(i_Item))
			{
				po_LC->SetItemState(i_Item, 0xFFFF, LVIS_SELECTED | LVIS_ACTIVATING);
				break;
			}
		}
	}
	else
	{
		po_LC->SetItemState(_i_Index, 0xFFFF, LVIS_SELECTED | LVIS_ACTIVATING);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnSelSound(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_OneSound	*pst_Sound;
	int					i_Item, i_Sound;
	ULONG				ul_Fat;
	CListCtrl			*po_LC;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Item = i_GetCurSel(IDC_LIST_SOUND);
	if(i_Item == -1) return;

	po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_SOUND);
	i_Sound = po_LC->GetItemData(i_Item);
	pst_Sound = SND_gst_Params.dst_Sound + i_Sound;
	ul_Fat = BIG_ul_SearchKeyToFat(pst_Sound->ul_FileKey);
	mpo_Editor->Sound_Set(ul_Fat);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnSelInstance(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	int						i_Item, i_Idx;
	CListCtrl				*po_LC;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Item = i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_Item == -1) return;

	po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_INSTANCE);
	i_Idx = po_LC->GetItemData(i_Item);
	pst_SI = SND_gst_Params.dst_Instance + i_Idx;
	if(pst_SI->p_GameObject && (pst_SI->ul_Flags & SND_Cul_DSF_Used) && mpo_Editor)
	{
		M_MF()->SendMessageToLinks(mpo_Editor, EDI_MESSAGE_SELDATA, SEL_C_SIF_Object, (ULONG) pst_SI->p_GameObject);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnSelBank(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdun_Main	*pst_Sound;
	int				i_Item, i_Sound;
	ULONG			ul_Fat;
	CListCtrl		*po_LC;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Item = i_GetCurSel(IDC_LIST_BANK);
	if(i_Item == -1) return;

	po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_BANK);
	i_Sound = po_LC->GetItemData(i_Item);
	pst_Sound = SND_gap_MainRefList[i_Sound];
	ul_Fat = BIG_ul_SearchKeyToFat(pst_Sound->st_Bank.ul_FileKey);
	mpo_Editor->SoundBank_Set(ul_Fat, FALSE);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnColumnClickSound(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	NM_LISTVIEW *pNotifyStruct;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pNotifyStruct = (NM_LISTVIEW *) pNotifyStruct2;

	i_SortCol[1] = pNotifyStruct->iSubItem;
	FillListSound();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnColumnClickBank(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	NM_LISTVIEW *pNotifyStruct;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pNotifyStruct = (NM_LISTVIEW *) pNotifyStruct2;

	i_SortCol[0] = pNotifyStruct->iSubItem;
	FillListBank();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnColumnClickInstance(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	NM_LISTVIEW *pNotifyStruct;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pNotifyStruct = (NM_LISTVIEW *) pNotifyStruct2;

	i_SortCol[2] = pNotifyStruct->iSubItem;
	mb_ForceRefreshInstanceDisplay = TRUE;
	FillListInstance();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnRightClickInstance(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint			o_Point;
	EMEN_cl_SubMenu o_SubMenu(FALSE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&o_Point);
	mpo_Editor->InitPopupMenuAction(&o_SubMenu);

	M_MF()->AddPopupMenuAction(mpo_Editor, &o_SubMenu, 0, TRUE, "SModifier");
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_FINDSMD);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_EDITSMD);

	M_MF()->AddPopupMenuAction(mpo_Editor, &o_SubMenu, 0, TRUE, "Sound");
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_FINDSOUND);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_EDITSOUND);

	M_MF()->AddPopupMenuAction(mpo_Editor, &o_SubMenu, 0, TRUE, "Solo/Mute");
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_SOLOGAO);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_MUTEGAO);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_SOLOINST);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_MUTEINST);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_SOLOOFF);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_MUTEOFF);

	M_MF()->AddPopupMenuAction(mpo_Editor, &o_SubMenu, 0, TRUE, "Instance");
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_SET);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_SPY);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_ADDAIWATCH);

	M_MF()->AddPopupMenuAction(mpo_Editor, &o_SubMenu, 0, TRUE, "Gao");
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_INST_FINDGAO);

	mpo_Editor->TrackPopupMenuAction(o_Point, &o_SubMenu);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnRightClickSound(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu		o_SubMenu(FALSE);
	CPoint				o_Point;
	int					i_Item;
	CListCtrl			*po_LC;
	int					find;
	SND_tdst_OneSound	*pst_Sound;
	int					i_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	find = 0;

	i_Item = i_GetCurSel(IDC_LIST_SOUND);
	if(i_Item != -1)
	{
		po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_SOUND);
		i_Sound = po_LC->GetItemData(i_Item);
		pst_Sound = SND_gst_Params.dst_Sound + i_Sound;
		if(pst_Sound->ul_FileKey == mpo_Editor->mst_SDesc.ul_Key) find = 1;
		if(pst_Sound->ul_FileKey == mpo_Editor->mst_SModifierDesc.ul_Key) find = 2;
	}

	GetCursorPos(&o_Point);

	mpo_Editor->InitPopupMenuAction(&o_SubMenu);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_SOUND_REPORT);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_SubMenu, 0, TRUE, "Debug");
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_SOUNDSETSPY);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_SOUNDUNSPY);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_SubMenu, 0, TRUE, "Edit");
	if(find == 1)
	{
		mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_SOUND_FINDFILE);
		mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_SOUND_USER_REPORT);
		mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_CLOSESND);
	}

	if(find == 2)
	{
		mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_SMODIFIER_FINDFILE);
		mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_SMODIFIER_USER_REPORT);
		mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_SMODIFIER_CLOSE);
	}

	mpo_Editor->TrackPopupMenuAction(o_Point, &o_SubMenu);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnRightClickInsert(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint			o_Point;
	CMenu			o_Menu;
	CListCtrl		*po_LC;
	int				i_Item, res;
	ULONG			ulFat;
	EBRO_cl_Frame	*po_Browser;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Item = i_GetCurSel(IDC_LIST_MUTE);
	if(i_Item == -1) return;

	GetCursorPos(&o_Point);
	o_Menu.CreatePopupMenu();
	o_Menu.AppendMenu(MF_BYCOMMAND, 1, "Find");
	res = o_Menu.TrackPopupMenu
		(
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
			o_Point.x,
			o_Point.y,
			this
		);
	switch(res)
	{
	case 1:
		po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_MUTE);
		ulFat = BIG_ul_SearchKeyToFat(SND_gst_Params.dst_Insert[po_LC->GetItemData(i_Item)].ul_FileKey);
		if(ulFat != BIG_C_InvalidIndex)
		{
			po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
			po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
			po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ulFat), ulFat);
		}
		break;

	default:
		break;
	}
}


void ESON_cl_View::OnRightClickGroup(NMHDR *pNotifyStruct2, LRESULT *result)
{
#ifdef _DEBUG
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint			o_Point;
	CMenu			o_Menu;
	int				i_Item, res;
	EDIA_cl_SndGrpWatcher o_Dial;
	CListCtrl *po_LC ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Item = i_GetCurSel(IDC_LIST_GROUP);
	if(i_Item == -1) return;
	po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_GROUP);
	i_Item = po_LC->GetItemData(i_Item);

	GetCursorPos(&o_Point);
	o_Menu.CreatePopupMenu();
	o_Menu.AppendMenu(MF_BYCOMMAND, 1, "Watcher");
	o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
	o_Menu.AppendMenu(MF_BYCOMMAND, 2, "Delete All AI Watch");
	o_Menu.AppendMenu(MF_BYCOMMAND, 3, "Reset Watcher");
	res = o_Menu.TrackPopupMenu
		(
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
			o_Point.x,
			o_Point.y,
			this
		);
	switch(res)
	{
	case 1:
		o_Dial.mi_CurrentGrp = i_Item;
		if(o_Dial.DoModal()==IDOK)
		{
			AI_DelAllWatch();
			for(int i = 0; i < SND_e_GrpNumber; i++)
			{
				for(int j = 0; j < 32; j++) 
				{
					if(o_Dial.mai_AiWatch[i] && (SND_gdst_Group[i].ul_WatchRq & (1 << j)))
						AI_AddWatch(&SND_gdst_Group[i].af_VolRq[j], sizeof(float));
				}
				
				if(SND_gdst_Group[i].b_VolWatch && o_Dial.mai_AiWatch[i])
					AI_AddWatch(&SND_gdst_Group[i].f_Volume, sizeof(float));
			}
		}
		break;

	case 2:
		AI_DelAllWatch();
		for(int i = 0; i < SND_e_GrpNumber; i++)
		{
			o_Dial.mai_AiWatch[i] = 0;
		}
		break;

	case 3:
		AI_DelAllWatch();
		for(int i = 0; i < SND_e_GrpNumber; i++)
		{
			for(int j = 0; j < 32; j++) 
			{
				SND_gdst_Group[i].ul_WatchRq = 0;
			}
			o_Dial.mai_AiWatch[i] = 0;
			SND_gdst_Group[i].b_VolWatch = FALSE;
		}
		break;

	default:
		break;
	}
#endif
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnRightClickBank(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_SubMenu(FALSE);
	CPoint			o_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&o_Point);

	mpo_Editor->InitPopupMenuAction(&o_SubMenu);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_BANK_REPORT);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_SubMenu, 0, TRUE, "Edit");
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_BANK_FINDFILE);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_MODIFYBANK);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_BANK_USER_REPORT);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_BANK_CONTENTS_REPORT);
	mpo_Editor->AddPopupMenuAction(&o_SubMenu, ESON_ACTION_CLOSEBANK);
	mpo_Editor->TrackPopupMenuAction(o_Point, &o_SubMenu);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void M_AppendDisplayMenu(CMenu *_po_Menu, int _i_filter, char *_str, int _i_flag)
{
	if(_i_filter & _i_flag)
		_po_Menu->AppendMenu(MF_BYCOMMAND | MF_CHECKED, _i_flag, _str);
	else
		_po_Menu->AppendMenu(MF_BYCOMMAND, _i_flag, _str);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnParentNotify(UINT message, LONG lParam)
{
	/*~~~~~~~~~~~~~~~*/
	CMenu		o_Menu;
	CPoint		pt;
	int			res;
	CTabCtrl	*po_TC;
	/*~~~~~~~~~~~~~~~*/

	po_TC = (CTabCtrl *) GetDlgItem(IDC_TAB_VIEW);
	if(po_TC)
	{
		mi_Pane = po_TC->GetCurFocus();

		switch(mi_Pane)
		{
		case 0: /* bank */
			break;

		case 1: /* sound */
			if(message == WM_RBUTTONDOWN)
			{
				o_Menu.CreatePopupMenu();
				GetCursorPos(&pt);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mst_Ini.i_FiltersSound, ".waa", 1);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mst_Ini.i_FiltersSound, ".wad", 2);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mst_Ini.i_FiltersSound, ".wam", 4);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mst_Ini.i_FiltersSound, ".wav", 8);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mst_Ini.i_FiltersSound, ".wac", 16);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mst_Ini.i_FiltersSound, ".smd", 32);
				o_Menu.AppendMenu(MF_SEPARATOR, 0, ESON_STR_Csz_Display);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mst_Ini.i_FiltersSound, "None", 128);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mst_Ini.i_FiltersSound, "All", 256);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mst_Ini.i_FiltersSound, "Reverse", 64);
				res = o_Menu.TrackPopupMenu
					(
						TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
						pt.x,
						pt.y,
						this
					);
				if(res != -1)
				{
					if(res & 256)
					{
						mpo_Editor->mst_Ini.i_FiltersSound = 63;
					}
					else if(res & 128)
					{
						mpo_Editor->mst_Ini.i_FiltersSound = 0;
					}
					else if(res & 64)
					{
						mpo_Editor->mst_Ini.i_FiltersSound = ~mpo_Editor->mst_Ini.i_FiltersSound;
						mpo_Editor->mst_Ini.i_FiltersSound &= 63;
					}
					else if(mpo_Editor->mst_Ini.i_FiltersSound & res)
						mpo_Editor->mst_Ini.i_FiltersSound &= ~res;
					else
						mpo_Editor->mst_Ini.i_FiltersSound |= res;
					UpdateControls();
				}

				return;
			}
			break;

		case 2: /* instance */
			if(message == WM_RBUTTONDOWN)
			{
				o_Menu.CreatePopupMenu();
				GetCursorPos(&pt);

				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					ESON_STR_Csz_TitleSound,
					ESON_C_InstDisplaySound
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					ESON_STR_Csz_TitleState,
					ESON_C_InstDisplayState
				);

				if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayVol)
				{
					if(mpo_Editor->mst_Ini.i_Option & ESON_Cte_DispEffVol)
						o_Menu.AppendMenu(MF_BYCOMMAND | MF_CHECKED, 15, "Eff-" ESON_STR_Csz_TitleVolume);
					else
						o_Menu.AppendMenu(MF_BYCOMMAND | MF_CHECKED, 15, ESON_STR_Csz_TitleVolume);
				}
				else
				{
					o_Menu.AppendMenu(MF_BYCOMMAND, ESON_C_InstDisplayVol, ESON_STR_Csz_TitleVolume);
				}

				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					ESON_STR_Csz_TitleFlags,
					ESON_C_InstDisplayFlags
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					ESON_STR_Csz_TitleCol5,
					ESON_C_InstDisplayExtFlags
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					ESON_STR_Csz_TitleTrack,
					ESON_C_InstDisplayTrack
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					ESON_STR_Csz_TitleChannel,
					ESON_C_InstDisplayChannel
				);

				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					ESON_STR_Csz_TitleFrequency,
					ESON_C_InstDisplayFrequency
				);

				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					ESON_STR_Csz_TitleFaderGroup,
					ESON_C_InstDisplayFaderGroup
				);

				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					"Prefetch",
					ESON_C_InstDisplayPrefetch
				);
				o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
				o_Menu.AppendMenu(MF_BYCOMMAND, 13, "None");
				o_Menu.AppendMenu(MF_BYCOMMAND, 17, "All");
				o_Menu.AppendMenu(MF_BYCOMMAND, 11, "Reverse");

				o_Menu.AppendMenu(MF_MENUBARBREAK, 0, "Filter :");

				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					"Only MdF",
					ESON_C_InstDisplayOnlyMdF
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					"Only NonMdF",
					ESON_C_InstDisplayOnlyNonMdF
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					"Only Playing",
					ESON_C_InstDisplayOnlyPlaying
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					"Only PlayingLoop",
					ESON_C_InstDisplayOnlyPlayingLoop
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					"Only Stream",
					ESON_C_InstDisplayOnlyStream
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					"Only NonStream",
					ESON_C_InstDisplayOnlyNonStream
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					"Only OnTrack",
					ESON_C_InstDisplayOnlyOnTrack
				);
				M_AppendDisplayMenu
				(
					&o_Menu,
					mpo_Editor->mst_Ini.i_FilterInstance,
					"Only NonOnTrack",
					ESON_C_InstDisplayOnlyNonOnTrack
				);

				o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
				o_Menu.AppendMenu(MF_BYCOMMAND, 7, "None");
				o_Menu.AppendMenu(MF_BYCOMMAND, 9, "All");
				o_Menu.AppendMenu(MF_BYCOMMAND, 5, "Reverse");

				res = o_Menu.TrackPopupMenu
					(
						TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
						pt.x,
						pt.y,
						this
					);

				if(res != -1)
				{
					mb_ForceRefreshInstanceDisplay = TRUE;
					switch(res)
					{
					case 15:
						if(mpo_Editor->mst_Ini.i_Option & ESON_Cte_DispEffVol)
						{
							mpo_Editor->mst_Ini.i_Option &= ~ESON_Cte_DispEffVol;
							mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayVol;
						}
						else
						{
							mpo_Editor->mst_Ini.i_Option |= ESON_Cte_DispEffVol;
							mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayVol;
						}
						break;

					case 11:
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplaySound;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayState;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayVol;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayFlags;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayExtFlags;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayTrack;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayChannel;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayFrequency;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayFaderGroup;
						break;

					case 13:
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplaySound;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayState;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayVol;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayFlags;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayExtFlags;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayTrack;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayChannel;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayFrequency;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayFaderGroup;
						break;

					case 17:
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplaySound;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayState;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayVol;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayFlags;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayExtFlags;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayTrack;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayChannel;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayFrequency;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayFaderGroup;
						break;

					case 5:
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayOnlyMdF;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayOnlyNonMdF;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayOnlyPlaying;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayOnlyStream;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayOnlyNonStream;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayOnlyReset;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayOnlyOnTrack;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayOnlyNonOnTrack;
						mpo_Editor->mst_Ini.i_FilterInstance ^= ESON_C_InstDisplayOnlyPlayingLoop;
						break;

					case 7:
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyMdF;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyNonMdF;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyPlaying;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyStream;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyNonStream;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyReset;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyOnTrack;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyNonOnTrack;
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyPlayingLoop;
						break;

					case 9:
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayOnlyMdF;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayOnlyNonMdF;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayOnlyPlaying;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayOnlyStream;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayOnlyNonStream;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayOnlyReset;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayOnlyOnTrack;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayOnlyNonOnTrack;
						mpo_Editor->mst_Ini.i_FilterInstance |= ESON_C_InstDisplayOnlyPlayingLoop;
						break;

					default:
						mpo_Editor->mst_Ini.i_FilterInstance ^= res;
						break;
					}

					if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyMdF)
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyNonMdF;
					if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyNonMdF)
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyMdF;
					if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyStream)
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyNonStream;
					if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyNonStream)
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyStream;
					if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyOnTrack)
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyNonOnTrack;
					if(mpo_Editor->mst_Ini.i_FilterInstance & ESON_C_InstDisplayOnlyNonOnTrack)
						mpo_Editor->mst_Ini.i_FilterInstance &= ~ESON_C_InstDisplayOnlyOnTrack;
					{
						/*~~~~~~~~~~~~~~~*/
						CListCtrl	*po_LC;
						int			i, j;
						/*~~~~~~~~~~~~~~~*/

						po_LC = (CListCtrl *) GetDlgItem(IDC_LIST_INSTANCE);
						for(i = 0; i < ESON_Cte_DisplayLineSize; i++)
						{
							mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] = po_LC->GetColumnWidth(i);
						}

						for(i = 0; i < ESON_Cte_DisplayLineSize; i++)
						{
							po_LC->DeleteColumn(ESON_Cte_DisplayLineSize - 1 - i);
						}

						po_LC->InsertColumn
							(
								0,
								InstanceColumnName[0],
								LVCFMT_LEFT,
								mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[0]
							);
						for(j = i = 1; i < ESON_Cte_DisplayLineSize; i++)
						{
							if(mpo_Editor->mst_Ini.i_FilterInstance & InstanceFlags[i])
							{
								if(mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] <= 0)
									mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] = 64;
								if(mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] > 100000)
									mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] = 64;
								po_LC->InsertColumn
									(
										j++,
										InstanceColumnName[i],
										LVCFMT_LEFT,
										mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i]
									);
							}
							else
								mpo_Editor->mst_Ini.l_ListInstanceColumnWidth[i] = 0;
						}
					}

					UpdateControls();
				}

				return;
			}
			break;

		case 3:
			break;

		case 4:
			if(message == WM_RBUTTONDOWN)
			{
				o_Menu.CreatePopupMenu();
				GetCursorPos(&pt);
				for(int ii = 0; ii <SND_e_GrpNumber; ii++)
				{
					M_AppendDisplayMenu(&o_Menu, mpo_Editor->mi_FiltersGroup, gaz_GroupName[ii], 1<<ii);
				}

				o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mi_FiltersGroup, "None", 0x80000000);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mi_FiltersGroup, "All", 0x40000000);
				M_AppendDisplayMenu(&o_Menu, mpo_Editor->mi_FiltersGroup, "Reverse", 0x20000000);
				res = o_Menu.TrackPopupMenu
					(
						TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
						pt.x,
						pt.y,
						this
					);
				if(res != -1)
				{
					if(res == 0x80000000)
					{
						mpo_Editor->mi_FiltersGroup = 0;
					}
					else if(res == 0x40000000)
					{
						mpo_Editor->mi_FiltersGroup = 0x1FFF;
					}
					else if(res == 0x20000000)
					{
						mpo_Editor->mi_FiltersGroup = ~mpo_Editor->mi_FiltersGroup ;
						mpo_Editor->mi_FiltersGroup &= 0x1FFF;
					}
					else if(mpo_Editor->mi_FiltersGroup & res)
						mpo_Editor->mi_FiltersGroup &= ~res;
					else
						mpo_Editor->mi_FiltersGroup |= res;

					L_memset(gaz_LastGroupDisplay, 0, sizeof(gaz_LastGroupDisplay));
					UpdateControls();
				}

				return;
			}
			break;
		default:
			break;
		}
	}

	CFormView::OnParentNotify(message, lParam);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnSoloOffButton(void)
{
	mpo_Editor->OnAction(ESON_ACTION_INST_SOLOOFF);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_View::OnMuteOffButton(void)
{
	mpo_Editor->OnAction(ESON_ACTION_INST_MUTEOFF);
}
#endif /* ACTIVE_EDITORS */

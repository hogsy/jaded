/*$T SONview_smd.cpp GC 1.138 03/18/05 15:25:17 */


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
#include "BASe/MEMory/MEM.h"

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "Res/Res.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "BROwser/BROframe.h"
#include "EDImsg.h"
#include "LINks/LINKmsg.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAsmodifier_dlg.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDambience.h"
#include "SouND/Sources/SNDmusic.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDspecific.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDconv_xboxadpcm.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDloadingsound.h"

/*$2- sound edi ------------------------------------------------------------------------------------------------------*/

#include "SONframe.h"
#include "SONView_smd.h"
#include "SONstrings.h"
#include "SONutil.h"
#include "SONmsg.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define M_EnableCButton(__id, __check) \
	do \
	{ \
		GetDlgItem(__id)->EnableWindow(TRUE); \
		((CButton *) GetDlgItem(__id))->SetCheck((__check) ? 1 : 0); \
	} while(0);

#define M_EnableCEdit(__id, __init) \
	do \
	{ \
		GetDlgItem(__id)->EnableWindow(TRUE); \
		((CEdit *) GetDlgItem(__id))->SetWindowText(__init); \
	} while(0);

#define M_DisableCButton(__id) \
	do \
	{ \
		GetDlgItem(__id)->EnableWindow(FALSE); \
		((CButton *) GetDlgItem(__id))->SetCheck(0); \
	} while(0);

#define M_DisableCEdit(__id) \
	do \
	{ \
		GetDlgItem(__id)->EnableWindow(FALSE); \
		((CEdit *) GetDlgItem(__id))->SetWindowText(""); \
	} while(0);

extern void EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);

UINT		gui_BtnID[] =
{
	IDC_BTN_FADEI,
	IDC_BTN_FADEI2,
	IDC_BTN_FADEO,
	IDC_BTN_FADEO2,
	IDC_BTN_INS_ADD,
	IDC_BTN_INS_DEL,
	IDC_BTN_INS_MOVEDOWN,
	IDC_BTN_INS_MOVEUP,
	IDC_BTN_PLAY_ADD,
	IDC_BTN_INSERTHOLE,
	IDC_BTN_PLAY_DEL,
	IDC_BTN_PLAY_MOVEUP,
	IDC_BTN_PLAY_MOVEDOWN,
	IDC_BTN_FXAB,
	-1
};
/**/
UINT		gui_CheckID[] = { IDC_CHECK_DYNPAN, IDC_CHECK_GRPA, IDC_CHECK_GRPB, IDC_CHECKX, IDC_CHECKY,
		IDC_CHECKZ, -1 };
/**/
UINT		gui_EditID[] =
{
	IDC_EDIT_DOPPLER,
	IDC_EDIT_FREQ,
	IDC_EDIT_FREQMAX,
	IDC_EDIT_FREQMIN,
	IDC_EDIT_CYLINDER,
	IDC_EDIT_MFACT,
	IDC_EDIT_VOLMIN,
	IDC_EDIT_VOLMAX,
	IDC_EDIT_XF,
	IDC_EDIT_XM,
	IDC_EDIT_XN,
	IDC_EDIT_YF,
	IDC_EDIT_YM,
	IDC_EDIT_YN,
	IDC_EDIT_ZF,
	IDC_EDIT_ZM,
	IDC_EDIT_ZN,
	-1
};
/**/
UINT		gui_ListID[] = { IDC_LIST_PLAY, IDC_LIST_INSERT, -1 };
/**/
UINT		gui_RadioID[] =
{
	IDC_RADIOAXIAL,
	IDC_RADIOGRP1,
	IDC_RADIOGRP2,
	IDC_RADIOGRP3,
	IDC_RADIOGRP4,
	IDC_RADIOGRP5,
	IDC_RADIOGRP6,
	IDC_RADIO_PLAYSEQ,
	IDC_RADIO_PLAYRAN,
	IDC_RADIO_STOPONLASTHIT,
	IDC_RADIOSPHERIC,
	-1
};
/**/
UINT		gui_SliderID[] =
{
	IDC_SLIDER_FXVOLL,
	IDC_SLIDER_FXVOLR,
	IDC_SLIDER_MINPAN,
	IDC_SLIDER_MINPAN2,
	IDC_SLIDER_PAN,
	IDC_SLIDER_VOL,
	IDC_SLIDER_VOL2,
	-1
};

UINT		gui_3DGroupID[] =
{
	IDC_CHECKX,
	IDC_CHECKY,
	IDC_CHECKZ,
	IDC_EDIT_CYLINDER,
	IDC_EDIT_MFACT,
	IDC_EDIT_XF,
	IDC_EDIT_XM,
	IDC_EDIT_XN,
	IDC_EDIT_YF,
	IDC_EDIT_YM,
	IDC_EDIT_YN,
	IDC_EDIT_ZF,
	IDC_EDIT_ZM,
	IDC_EDIT_ZN,
	IDC_RADIOAXIAL,
	IDC_RADIOSPHERIC,
	IDC_STATIC1,
	IDC_STATIC2,
	IDC_STATIC3,
	IDC_STATIC4,
	IDC_STATICf,
	IDC_SLIDER_VOL2,
	IDC_CHECK_DYNPAN,
	IDC_SLIDER_MINPAN,
	IDC_SLIDER_MINPAN2,
	-1
};

/*$4
 ***********************************************************************************************************************
    message map
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNCREATE(ESON_cl_ViewSmd, CFormView)
BEGIN_MESSAGE_MAP(ESON_cl_ViewSmd, CFormView)
	ON_BN_CLICKED(IDC_BTN_NO3D, OnNo3D)
	ON_BN_CLICKED(IDC_RADIOGRP1, OnGrp1)
	ON_BN_CLICKED(IDC_RADIOGRP2, OnGrp2)
	ON_BN_CLICKED(IDC_RADIOGRP3, OnGrp3)
	ON_BN_CLICKED(IDC_RADIOGRP4, OnGrp4)
	ON_BN_CLICKED(IDC_RADIOGRP5, OnGrp5)
	ON_BN_CLICKED(IDC_RADIOGRP6, OnGrp6)
	ON_BN_CLICKED(IDC_CHECK_GRPA, OnGrpA)
	ON_BN_CLICKED(IDC_CHECK_GRPB, OnGrpB)
	ON_BN_CLICKED(IDC_BTN_FXAB, OnFxAB)
	ON_BN_CLICKED(IDC_RADIO_PLAYSEQ, OnPlaySequential)
	ON_BN_CLICKED(IDC_RADIO_PLAYRAN, OnPlayRandom)
	ON_BN_CLICKED(IDC_RADIO_STOPONLASTHIT, OnStopOnLastHit)
	ON_BN_CLICKED(IDC_CHECK_DYNPAN, OnDynPan)
	ON_BN_CLICKED(IDC_RADIOSPHERIC, OnSpheric)
	ON_BN_CLICKED(IDC_RADIOAXIAL, OnAxial)
	ON_BN_CLICKED(IDC_CHECKX, OnX)
	ON_BN_CLICKED(IDC_CHECKY, OnY)
	ON_BN_CLICKED(IDC_CHECKZ, OnZ)
	ON_BN_CLICKED(IDC_BTN_FADEI, OnFadeIn)
	ON_BN_CLICKED(IDC_BTN_FADEI2, OnFadeInChange)
	ON_BN_CLICKED(IDC_BTN_FADEO, OnFadeOut)
	ON_BN_CLICKED(IDC_BTN_FADEO2, OnFadeOutChange)
	ON_BN_CLICKED(IDC_BTN_PLAY_ADD, OnPlayListAdd)
	ON_BN_CLICKED(IDC_BTN_INSERTHOLE, OnPlayListInsertHole)
	ON_BN_CLICKED(IDC_BTN_PLAY_DEL, OnPlayListDel)
	ON_BN_CLICKED(IDC_BTN_PLAY_MOVEUP, OnPlayListMoveUp)
	ON_BN_CLICKED(IDC_BTN_PLAY_MOVEDOWN, OnPlayListMoveDown)
	ON_BN_CLICKED(IDC_BTN_INS_ADD, OnInsertListAdd)
	ON_BN_CLICKED(IDC_BTN_INS_DEL, OnInsertListDel)
	ON_BN_CLICKED(IDC_BTN_INS_MOVEUP, OnInsertListMoveUp)
	ON_BN_CLICKED(IDC_BTN_INS_MOVEDOWN, OnInsertListMoveDown)
	ON_EN_KILLFOCUS(IDC_EDIT_XN, OnChangeXNear)
	ON_EN_KILLFOCUS(IDC_EDIT_XM, OnChangeXMiddle)
	ON_EN_KILLFOCUS(IDC_EDIT_XF, OnChangeXFar)
	ON_EN_KILLFOCUS(IDC_EDIT_YN, OnChangeYNear)
	ON_EN_KILLFOCUS(IDC_EDIT_YM, OnChangeYMiddle)
	ON_EN_KILLFOCUS(IDC_EDIT_YF, OnChangeYFar)
	ON_EN_KILLFOCUS(IDC_EDIT_ZN, OnChangeZNear)
	ON_EN_KILLFOCUS(IDC_EDIT_ZM, OnChangeZMiddle)
	ON_EN_KILLFOCUS(IDC_EDIT_ZF, OnChangeZFar)
	ON_EN_KILLFOCUS(IDC_EDIT_MFACT, OnChangeMiddleFactor)
	ON_EN_KILLFOCUS(IDC_EDIT_CYLINDER, OnChangeCylinder)
	ON_EN_KILLFOCUS(IDC_EDIT_VOLMIN, OnChangeVolMin)
	ON_EN_KILLFOCUS(IDC_EDIT_VOLMAX, OnChangeVolMax)
	ON_EN_KILLFOCUS(IDC_EDIT_FREQ, OnChangeFreq)
	ON_EN_KILLFOCUS(IDC_EDIT_FREQMIN, OnChangeFreqMin)
	ON_EN_KILLFOCUS(IDC_EDIT_FREQMAX, OnChangeFreqMax)
	ON_EN_KILLFOCUS(IDC_EDIT_DOPPLER, OnChangeDoppler)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    menbers
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_ViewSmd::ESON_cl_ViewSmd(ESON_cl_Frame *_po_Editor) :
	CFormView(ESON_IDD_EDITSMD)
{
	mpo_Editor = _po_Editor;
	mb_InterfaceIsEnabled = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_ViewSmd::ESON_cl_ViewSmd(void) :
	CFormView(ESON_IDD_EDITSMD)
{
	ESON_cl_ViewSmd(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_ViewSmd::~ESON_cl_ViewSmd(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_ViewSmd::Create
(
	LPCTSTR		lpszClassName,
	LPCTSTR		lpszWindowName,
	DWORD		dwStyle,
	const RECT	&rect,
	CWnd		*pParentWnd,
	UINT		nID
)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl *po_Slider;
	/*~~~~~~~~~~~~~~~~~~~*/

	CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, NULL);

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_VOL);
	po_Slider->SetRange(0, 1000, TRUE);
	po_Slider->SetPos(0);

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_VOL2);
	po_Slider->SetRange(0, 1000, TRUE);
	po_Slider->SetPos(0);

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_FXVOLL);
	po_Slider->SetRange(0, 2, TRUE);
	po_Slider->SetPos(0);

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_FXVOLR);
	po_Slider->SetRange(0, 2, TRUE);
	po_Slider->SetPos(0);

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN);
	po_Slider->SetRange(0, 100, TRUE);
	po_Slider->SetPos(0);

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN2);
	po_Slider->SetRange(0, 100, TRUE);
	po_Slider->SetPos(100);

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_PAN);
	po_Slider->SetRange(-10000, 10000, TRUE);
	po_Slider->SetPos(0);

	mb_InterfaceIsEnabled = TRUE;
	DisableInterface();
	EDIA_cl_BaseDialog::SetTheme(this);

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::UpdateSmdView(void)
{
	if(SND_gst_Params.l_Available && GetExtPlayer())
	{
		EnableInterface();
		UpdateExtendedPlayer();
	}
	else
	{
		DisableInterface();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnRButtonDown(UINT nId, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	CMenu						o_Menu;
	CMenu						o_FadeInMenu;
	CMenu						o_FadeOutMenu;
	CMenu						o_PanMenu;
	CMenu						o_FreqMenu;
	CMenu						o_VolMenu;
	UINT						ui_FadeInMenu;
	UINT						ui_FadeOutMenu;
	CPoint						o_pt;
	int							res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	o_FadeInMenu.CreatePopupMenu();
	o_FadeInMenu.AppendMenu(MF_BYCOMMAND, 1, "Find");
	o_FadeInMenu.AppendMenu(MF_BYCOMMAND, 2, "Edit");
	o_FadeInMenu.AppendMenu(MF_BYCOMMAND, 3, "Delete");
	ui_FadeInMenu = (pst_ExtPlayer->ul_FadeInKey == BIG_C_InvalidKey) ? MF_POPUP | MF_DISABLED | MF_GRAYED : MF_POPUP;

	o_FadeOutMenu.CreatePopupMenu();
	o_FadeOutMenu.AppendMenu(MF_BYCOMMAND, 11, "Find");
	o_FadeOutMenu.AppendMenu(MF_BYCOMMAND, 12, "Edit");
	o_FadeOutMenu.AppendMenu(MF_BYCOMMAND, 13, "Delete");
	ui_FadeOutMenu = (pst_ExtPlayer->ul_FadeOutKey == BIG_C_InvalidKey) ? MF_POPUP | MF_DISABLED | MF_GRAYED : MF_POPUP;

	o_PanMenu.CreatePopupMenu();
	o_PanMenu.AppendMenu(MF_BYCOMMAND, 21, "Reset");
	o_PanMenu.AppendMenu(MF_BYCOMMAND, 22, "Reset min. pan");

	o_FreqMenu.CreatePopupMenu();
	o_FreqMenu.AppendMenu(MF_BYCOMMAND, 31, "Reset");
	o_FreqMenu.AppendMenu(MF_BYCOMMAND, 32, "Reset random freq.");
	o_FreqMenu.AppendMenu(MF_BYCOMMAND, 34, "Set default random freq.");
	o_FreqMenu.AppendMenu(MF_BYCOMMAND, 33, "Get Freq. from file");
	o_FreqMenu.AppendMenu(MF_BYCOMMAND, 35, "Reset doppler.");

	o_VolMenu.CreatePopupMenu();
	o_VolMenu.AppendMenu(MF_BYCOMMAND, 41, "Reset");
	o_VolMenu.AppendMenu(MF_BYCOMMAND, 42, "Reset random vol.");
	o_VolMenu.AppendMenu(MF_BYCOMMAND, 43, "Set default random vol.");

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&o_pt);
	o_Menu.CreatePopupMenu();
	o_Menu.AppendMenu(MF_BYCOMMAND, 51, " Find smd file");
	o_Menu.AppendMenu(MF_POPUP, (UINT) o_VolMenu.m_hMenu, "Volume");
	o_Menu.AppendMenu(MF_POPUP, (UINT) o_PanMenu.m_hMenu, "Pan");
	o_Menu.AppendMenu(MF_POPUP, (UINT) o_FreqMenu.m_hMenu, "Frequency");
	o_Menu.AppendMenu(ui_FadeInMenu, (UINT) o_FadeInMenu.m_hMenu, "Fade In");
	o_Menu.AppendMenu(ui_FadeOutMenu, (UINT) o_FadeOutMenu.m_hMenu, "Fade Out");
	res = o_Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, o_pt.x, o_pt.y, this);

	switch(res)
	{
	case 1:		OnFind2(pst_ExtPlayer->ul_FadeInKey); break;
	case 2:		mpo_Editor->Insert_OnOpen(BIG_ul_SearchKeyToFat(pst_ExtPlayer->ul_FadeInKey)); break;
	case 3:		InitFadeIn(); break;
	case 11:	OnFind2(pst_ExtPlayer->ul_FadeOutKey); break;
	case 12:	mpo_Editor->Insert_OnOpen(BIG_ul_SearchKeyToFat(pst_ExtPlayer->ul_FadeOutKey)); break;
	case 13:	InitFadeOut(); break;
	case 21:	InitPan(); break;
	case 22:	InitMinPan(); break;
	case 31:	InitFreqFromFile(-1); break;
	case 32:	InitRandomFreq(); break;
	case 33:	InitFreqFromFile(0); break;
	case 34:	DefaultRandomFreq(); break;
	case 35:	InitDoppler(); break;
	case 41:	InitVol(); break;
	case 42:	InitRandomVol(); break;
	case 43:	DefaultRandomVol(); break;
	case 51:	OnFind(); break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnRButtonDownPlayList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	CMenu						o_Menu;
	CPoint						o_pt;
	int							res, i_Item;
	CListBox					*po_LC;
	ULONG						ulFat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;

	GetCursorPos(&o_pt);
	o_Menu.CreatePopupMenu();

	if(pst_ExtPlay->st_PlayList.aul_KeyList)
	{
		o_Menu.AppendMenu(MF_BYCOMMAND, 1, "Find");
		o_Menu.AppendMenu(MF_BYCOMMAND, 2, "Edit");
	}

	o_Menu.AppendMenu(MF_BYCOMMAND, 3, "Add");
	o_Menu.AppendMenu(MF_BYCOMMAND, 7, "Insert Hole");

	if(pst_ExtPlay->st_PlayList.aul_KeyList)
	{
		o_Menu.AppendMenu(MF_BYCOMMAND, 4, "Delete");
		o_Menu.AppendMenu(MF_BYCOMMAND, 5, "Move Up");
		o_Menu.AppendMenu(MF_BYCOMMAND, 6, "Move Down");
	}

	res = o_Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, o_pt.x, o_pt.y, this);

	switch(res)
	{
	case 1:
		po_LC = (CListBox *) GetDlgItem(IDC_LIST_PLAY);
		i_Item = po_LC->GetCurSel();
		if(i_Item != LB_ERR)
		{
			OnFind2(pst_ExtPlay->st_PlayList.aul_KeyList[i_Item]);
		}
		break;

	case 2:
		po_LC = (CListBox *) GetDlgItem(IDC_LIST_PLAY);
		i_Item = po_LC->GetCurSel();
		if(i_Item != LB_ERR)
		{
			ulFat = BIG_ul_SearchKeyToFat(pst_ExtPlay->st_PlayList.aul_KeyList[i_Item]);
			mpo_Editor->Sound_Set(ulFat);
		}
		break;

	case 3:
		OnPlayListAdd();
		break;

	case 4:
		OnPlayListDel();
		break;

	case 5:
		OnPlayListMoveUp();
		break;

	case 6:
		OnPlayListMoveDown();
		break;

	case 7:
		OnPlayListInsertHole();
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnRButtonDownInsertList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	CMenu						o_Menu;
	CPoint						o_pt;
	int							res, i_Item;
	CListBox					*po_LC;
	ULONG						ulFat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;

	GetCursorPos(&o_pt);
	o_Menu.CreatePopupMenu();

	if(pst_ExtPlay->st_InsertList.aul_KeyList)
	{
		o_Menu.AppendMenu(MF_BYCOMMAND, 1, "Find");
		o_Menu.AppendMenu(MF_BYCOMMAND, 2, "Edit");
	}

	o_Menu.AppendMenu(MF_BYCOMMAND, 3, "Add");
	if(pst_ExtPlay->st_InsertList.aul_KeyList)
	{
		o_Menu.AppendMenu(MF_BYCOMMAND, 4, "Delete");
		o_Menu.AppendMenu(MF_BYCOMMAND, 5, "Move Up");
		o_Menu.AppendMenu(MF_BYCOMMAND, 6, "Move Down");
	}

	res = o_Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, o_pt.x, o_pt.y, this);

	switch(res)
	{
	case 1:
		po_LC = (CListBox *) GetDlgItem(IDC_LIST_INSERT);
		i_Item = po_LC->GetCurSel();
		if(i_Item != LB_ERR)
		{
			OnFind2(pst_ExtPlay->st_InsertList.aul_KeyList[i_Item]);
		}
		break;

	case 2:
		po_LC = (CListBox *) GetDlgItem(IDC_LIST_INSERT);
		i_Item = po_LC->GetCurSel();
		if(i_Item != LB_ERR)
		{
			ulFat = BIG_ul_SearchKeyToFat(pst_ExtPlay->st_InsertList.aul_KeyList[i_Item]);
			mpo_Editor->Insert_OnOpen(ulFat);
		}
		break;

	case 3:
		OnInsertListAdd();
		break;

	case 4:
		OnInsertListDel();
		break;

	case 5:
		OnInsertListMoveUp();
		break;

	case 6:
		OnInsertListMoveDown();
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::UpdateExtendedPlayer(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl					*po_Slider;
	char						asz[64];
	SND_tdst_SModifierExtPlayer *pst_Play;
	ULONG						ulFat;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Play = GetExtPlayer();
	if(!pst_Play) return;

	/* SetRedraw(FALSE); */

	/*$1- group ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_EnableCButton(IDC_RADIOGRP1, (SND_M_SModifierGetGroup(pst_Play) == SND_Cte_SModifierGroupSFX));
	M_EnableCButton(IDC_RADIOGRP2, (SND_M_SModifierGetGroup(pst_Play) == SND_Cte_SModifierGroupMusic));
	M_EnableCButton(IDC_RADIOGRP3, (SND_M_SModifierGetGroup(pst_Play) == SND_Cte_SModifierGroupAmbience));
	M_EnableCButton(IDC_RADIOGRP4, (SND_M_SModifierGetGroup(pst_Play) == SND_Cte_SModifierGroupDialog));
	M_EnableCButton(IDC_RADIOGRP5, (SND_M_SModifierGetGroup(pst_Play) == SND_Cte_SModifierGroupCutScene));
	M_EnableCButton(IDC_RADIOGRP6, (SND_M_SModifierGetGroup(pst_Play) == SND_Cte_SModifierGroupInterface));

	M_EnableCButton(IDC_CHECK_GRPA, (pst_Play->ui_PlayerFlag & SND_Cte_SModifierGrpA));
	M_EnableCButton(IDC_CHECK_GRPB, (pst_Play->ui_PlayerFlag & SND_Cte_SModifierGrpB));

	M_EnableCButton(IDC_BTN_FXAB, 0);

	if(pst_Play->ui_PlayerFlag & SND_Cte_SModifierFxA)
		((CButton *) GetDlgItem(IDC_BTN_FXAB))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_FXA));
	else if(pst_Play->ui_PlayerFlag & SND_Cte_SModifierFxB)
		((CButton *) GetDlgItem(IDC_BTN_FXAB))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_FXB));
	else
		((CButton *) GetDlgItem(IDC_BTN_FXAB))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_FXAOFF));

	/*$1- lists ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	InsertListInit();
	PlayListInit();
	M_EnableCButton(IDC_RADIO_PLAYSEQ, !(pst_Play->ui_PlayerFlag & SND_Cte_SModifierRandomPlayList));
	M_EnableCButton(IDC_RADIO_PLAYRAN, pst_Play->ui_PlayerFlag & SND_Cte_SModifierRandomPlayList);
	M_EnableCButton(IDC_RADIO_STOPONLASTHIT, pst_Play->ui_PlayerFlag & SND_Cte_SModifierStopOnLastHit);

	/*$1- fade in/out ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_Play->ul_FadeInKey == BIG_C_InvalidKey)
	{
		((CButton *) GetDlgItem(IDC_BTN_FADEI))->SetWindowText("x");
	}
	else
	{
		ulFat = BIG_ul_SearchKeyToFat(pst_Play->ul_FadeInKey);
		((CButton *) GetDlgItem(IDC_BTN_FADEI))->SetWindowText(BIG_NameFile(ulFat));
	}

	if(pst_Play->ul_FadeOutKey == BIG_C_InvalidKey)
	{
		((CButton *) GetDlgItem(IDC_BTN_FADEO))->SetWindowText("x");
	}
	else
	{
		ulFat = BIG_ul_SearchKeyToFat(pst_Play->ul_FadeOutKey);
		((CButton *) GetDlgItem(IDC_BTN_FADEO))->SetWindowText(BIG_NameFile(ulFat));
	}

	/*$1- Near Far ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_EnableCButton(IDC_RADIOSPHERIC, pst_Play->ul_SndFlags & SND_Cul_SF_DynVolSpheric);
	M_EnableCButton(IDC_RADIOAXIAL, !(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolSpheric));

	M_EnableCButton(IDC_CHECKX, pst_Play->ul_SndFlags & SND_Cul_SF_DynVolXaxis);
	M_EnableCButton(IDC_CHECKY, pst_Play->ul_SndFlags & SND_Cul_SF_DynVolYaxis);
	M_EnableCButton(IDC_CHECKZ, pst_Play->ul_SndFlags & SND_Cul_SF_DynVolZaxis);

	/* reinit edit x-y-z */
	M_DisableCEdit(IDC_EDIT_XN);
	M_DisableCEdit(IDC_EDIT_XM);
	M_DisableCEdit(IDC_EDIT_XF);
	M_DisableCEdit(IDC_EDIT_YN);
	M_DisableCEdit(IDC_EDIT_YM);
	M_DisableCEdit(IDC_EDIT_YF);
	M_DisableCEdit(IDC_EDIT_ZN);
	M_DisableCEdit(IDC_EDIT_ZM);
	M_DisableCEdit(IDC_EDIT_ZF);

	if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolSpheric)
	{
		/* only one x-y-z */
		if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolXaxis)
		{
			((CButton *) GetDlgItem(IDC_CHECKX))->SetCheck(1);
			((CButton *) GetDlgItem(IDC_CHECKY))->SetCheck(0);
			((CButton *) GetDlgItem(IDC_CHECKZ))->SetCheck(0);
		}
		else if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolYaxis)
		{
			((CButton *) GetDlgItem(IDC_CHECKX))->SetCheck(0);
			((CButton *) GetDlgItem(IDC_CHECKY))->SetCheck(1);
			((CButton *) GetDlgItem(IDC_CHECKZ))->SetCheck(0);
		}
		else if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolZaxis)
		{
			((CButton *) GetDlgItem(IDC_CHECKX))->SetCheck(0);
			((CButton *) GetDlgItem(IDC_CHECKY))->SetCheck(0);
			((CButton *) GetDlgItem(IDC_CHECKZ))->SetCheck(1);
		}
		else
		{
			((CButton *) GetDlgItem(IDC_CHECKX))->SetCheck(0);
			((CButton *) GetDlgItem(IDC_CHECKY))->SetCheck(0);
			((CButton *) GetDlgItem(IDC_CHECKZ))->SetCheck(0);
		}

		/* edit x axis */
		sprintf(asz, "%.2f", pst_Play->af_Near[0]);
		M_EnableCEdit(IDC_EDIT_XN, asz);

		sprintf(asz, "%.2f", pst_Play->af_MiddleBlend[0]);
		M_EnableCEdit(IDC_EDIT_XM, asz);

		sprintf(asz, "%.2f", pst_Play->af_Far[0]);
		M_EnableCEdit(IDC_EDIT_XF, asz);

		if(pst_Play->ul_SndFlags & (SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis))
		{
			sprintf(asz, "%.2f", pst_Play->f_CylinderHeight);
			M_EnableCEdit(IDC_EDIT_CYLINDER, asz);
		}
		else
		{
			GetDlgItem(IDC_EDIT_CYLINDER)->EnableWindow(FALSE);
		}
	}
	else
	{
		M_DisableCEdit(IDC_EDIT_CYLINDER);
		if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolXaxis)
		{
			sprintf(asz, "%.2f", pst_Play->af_Near[0]);
			M_EnableCEdit(IDC_EDIT_XN, asz);

			sprintf(asz, "%.2f", pst_Play->af_MiddleBlend[0]);
			M_EnableCEdit(IDC_EDIT_XM, asz);

			sprintf(asz, "%.2f", pst_Play->af_Far[0]);
			M_EnableCEdit(IDC_EDIT_XF, asz);
		}

		if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolYaxis)
		{
			sprintf(asz, "%.2f", pst_Play->af_Near[1]);
			M_EnableCEdit(IDC_EDIT_YN, asz);

			sprintf(asz, "%.2f", pst_Play->af_MiddleBlend[1]);
			M_EnableCEdit(IDC_EDIT_YM, asz);

			sprintf(asz, "%.2f", pst_Play->af_Far[1]);
			M_EnableCEdit(IDC_EDIT_YF, asz);
		}

		if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolZaxis)
		{
			sprintf(asz, "%.2f", pst_Play->af_Near[2]);
			M_EnableCEdit(IDC_EDIT_ZN, asz);

			sprintf(asz, "%.2f", pst_Play->af_MiddleBlend[2]);
			M_EnableCEdit(IDC_EDIT_ZM, asz);

			sprintf(asz, "%.2f", pst_Play->af_Far[2]);
			M_EnableCEdit(IDC_EDIT_ZF, asz);
		}
	}

	sprintf(asz, "%.2f", pst_Play->f_MiddleCoeff);
	M_EnableCEdit(IDC_EDIT_MFACT, asz);

	/*$1- freq ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(asz, "%1.2f", pst_Play->f_FreqCoef);
	M_EnableCEdit(IDC_EDIT_FREQ, asz);

	sprintf(asz, "%1.2f", pst_Play->f_Freq_FactMax);
	M_EnableCEdit(IDC_EDIT_FREQMAX, asz);

	sprintf(asz, "%1.2f", pst_Play->f_Freq_FactMin);
	M_EnableCEdit(IDC_EDIT_FREQMIN, asz);

	sprintf(asz, "%1.2f", pst_Play->f_Doppler);
	M_EnableCEdit(IDC_EDIT_DOPPLER, asz);

	/*$1- volume ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_VOL);
	po_Slider->EnableWindow(TRUE);
	po_Slider->SetPos(1000 - (int) (pst_Play->f_DryVol * 1000.0f));

	if(pst_Play->ui_PlayerFlag & (SND_Cte_SModifierFxA | SND_Cte_SModifierFxB))
	{
		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_FXVOLL);
		po_Slider->EnableWindow(TRUE);
		po_Slider->ShowWindow(SW_SHOW);
		po_Slider->SetPos(2 - (int) (pst_Play->f_FxVolLeft * 2.0f));

		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_FXVOLR);
		po_Slider->EnableWindow(TRUE);
		po_Slider->ShowWindow(SW_SHOW);
		po_Slider->SetPos(2 - (int) (pst_Play->f_FxVolRight * 2.0f));

		GetDlgItem(IDC_STATICl)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATICr)->ShowWindow(SW_SHOW);
	}
	else
	{
		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_FXVOLL);
		po_Slider->EnableWindow(FALSE);
		po_Slider->ShowWindow(SW_HIDE);

		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_FXVOLR);
		po_Slider->EnableWindow(FALSE);
		po_Slider->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATICl)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATICr)->ShowWindow(SW_HIDE);
	}

	sprintf(asz, "%1.2f", pst_Play->f_DryVol_FactMin);
	M_EnableCEdit(IDC_EDIT_VOLMIN, asz);

	sprintf(asz, "%1.2f", pst_Play->f_DryVol_FactMax);
	M_EnableCEdit(IDC_EDIT_VOLMAX, asz);

	/*$1- pan ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((pst_Play->ul_SndFlags & SND_Cul_SF_MaskDynVol) == SND_Cul_SF_DynPan)
		pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynPan;

	if(pst_Play->ul_SndFlags & SND_Cul_SF_MaskDynVol)
	{
		GetDlgItem(IDC_STATICf)->ShowWindow(SW_SHOW);
		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_VOL2);
		po_Slider->EnableWindow(TRUE);
		po_Slider->ShowWindow(SW_SHOW);
		po_Slider->SetPos(1000 - (int) (pst_Play->f_FarCoeff * pst_Play->f_DryVol * 1000.0f));

		M_EnableCButton(IDC_CHECK_DYNPAN, pst_Play->ul_SndFlags & SND_Cul_SF_DynPan);

		if(GetDlgItem(IDC_BTN_NO3D)->IsWindowEnabled())
		{
			for(i = 0; gui_3DGroupID[i] != -1; i++)
			{
				GetDlgItem(gui_3DGroupID[i])->EnableWindow(TRUE);
				GetDlgItem(gui_3DGroupID[i])->ShowWindow(SW_SHOW);
			}

			GetDlgItem(IDC_BTN_NO3D)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_NO3D)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATICnear)->SetWindowText("Near");
		}
	}
	else
	{
		pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynPan;

		GetDlgItem(IDC_STATICf)->ShowWindow(SW_HIDE);
		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_VOL2);
		po_Slider->EnableWindow(FALSE);
		po_Slider->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_CHECK_DYNPAN)->EnableWindow(FALSE);
		if(!GetDlgItem(IDC_BTN_NO3D)->IsWindowEnabled())
		{
			for(i = 0; gui_3DGroupID[i] != -1; i++)
			{
				GetDlgItem(gui_3DGroupID[i])->EnableWindow(FALSE);
				GetDlgItem(gui_3DGroupID[i])->ShowWindow(SW_HIDE);
			}

			GetDlgItem(IDC_BTN_NO3D)->EnableWindow(TRUE);
			GetDlgItem(IDC_BTN_NO3D)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATICnear)->SetWindowText("Volume");
		}
	}

	if(pst_Play->ul_SndFlags & SND_Cul_SF_DynPan)
	{
		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN);
		po_Slider->EnableWindow(TRUE);
		po_Slider->ShowWindow(SW_SHOW);
		po_Slider->SetPos((int) (pst_Play->f_MinPan * 100.0f));
		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN2);
		po_Slider->EnableWindow(TRUE);
		po_Slider->ShowWindow(SW_SHOW);
		po_Slider->SetPos(100 - (int) (pst_Play->f_MinPan * 100.0f));
	}
	else
	{
		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN);
		po_Slider->EnableWindow(FALSE);
		po_Slider->ShowWindow(SW_HIDE);

		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN2);
		po_Slider->EnableWindow(FALSE);
		po_Slider->ShowWindow(SW_HIDE);
	}

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_PAN);
	po_Slider->EnableWindow(TRUE);
	po_Slider->SetPos(pst_Play->i_Pan);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::DisableInterface(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(!mb_InterfaceIsEnabled) return;
	mb_InterfaceIsEnabled = FALSE;

	GetDlgItem(IDC_STATICl)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATICr)->ShowWindow(SW_HIDE);

	for(i = 0; gui_3DGroupID[i] != -1; i++)
	{
		GetDlgItem(gui_3DGroupID[i])->EnableWindow(FALSE);
		GetDlgItem(gui_3DGroupID[i])->ShowWindow(SW_HIDE);
	}

	GetDlgItem(IDC_BTN_NO3D)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_NO3D)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATICnear)->SetWindowText("Volume");
/**/
	for(i = 0; gui_BtnID[i] != -1; i++)
	{
		GetDlgItem(gui_BtnID[i])->EnableWindow(FALSE);
	}
/**/
	for(i = 0; gui_CheckID[i] != -1; i++)
	{
		GetDlgItem(gui_CheckID[i])->EnableWindow(FALSE);
		((CButton *) GetDlgItem(gui_CheckID[i]))->SetCheck(0);
	}
/**/
	for(i = 0; gui_EditID[i] != -1; i++)
	{
		GetDlgItem(gui_EditID[i])->EnableWindow(FALSE);
		GetDlgItem(gui_EditID[i])->SetWindowText("");
	}
/**/
	for(i = 0; gui_ListID[i] != -1; i++)
	{
		GetDlgItem(gui_ListID[i])->EnableWindow(FALSE);
		((CListBox *) GetDlgItem(gui_ListID[i]))->ResetContent();
		((CListBox *) GetDlgItem(gui_ListID[i]))->SetRedraw(TRUE);
	}
/**/
	for(i = 0; gui_RadioID[i] != -1; i++)
	{
		GetDlgItem(gui_RadioID[i])->EnableWindow(FALSE);
		((CButton *) GetDlgItem(gui_RadioID[i]))->SetCheck(0);
	}
/**/
	for(i = 0; gui_SliderID[i] != -1; i++)
	{
		GetDlgItem(gui_SliderID[i])->EnableWindow(FALSE);
	}

	GetDlgItem(IDC_BTN_FADEI)->SetWindowText("");
	GetDlgItem(IDC_BTN_FADEO)->SetWindowText("");

	GetDlgItem(IDC_SLIDER_MINPAN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SLIDER_MINPAN2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SLIDER_PAN)->ShowWindow(SW_HIDE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::EnableInterface(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(mb_InterfaceIsEnabled) return;
	mb_InterfaceIsEnabled = TRUE;

	GetDlgItem(IDC_BTN_NO3D)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_NO3D)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATICnear)->SetWindowText("Volume");

	for(i = 0; gui_BtnID[i] != -1; i++)
	{
		GetDlgItem(gui_BtnID[i])->EnableWindow(TRUE);
	}
/**/
	for(i = 0; gui_CheckID[i] != -1; i++)
	{
		GetDlgItem(gui_CheckID[i])->EnableWindow(TRUE);
	}
/**/
	for(i = 0; gui_EditID[i] != -1; i++)
	{
		GetDlgItem(gui_EditID[i])->EnableWindow(TRUE);
	}
/**/
	for(i = 0; gui_ListID[i] != -1; i++)
	{
		GetDlgItem(gui_ListID[i])->EnableWindow(TRUE);
	}
/**/
	for(i = 0; gui_RadioID[i] != -1; i++)
	{
		GetDlgItem(gui_RadioID[i])->EnableWindow(TRUE);
	}
/**/
	for(i = 0; gui_SliderID[i] != -1; i++)
	{
		GetDlgItem(gui_SliderID[i])->EnableWindow(TRUE);
	}

	GetDlgItem(IDC_BTN_FADEI)->SetWindowText("x");
	GetDlgItem(IDC_BTN_FADEO)->SetWindowText("x");

	GetDlgItem(IDC_SLIDER_MINPAN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SLIDER_MINPAN2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SLIDER_PAN)->ShowWindow(SW_SHOW);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl					*po_Slider;
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	float						*pf_OldVol;
	float						f_NewVol;
	char						tip[32];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CFormView::OnVScroll(nSBCode, nPos, pScrollBar);

	if(!SND_gst_Params.l_Available) return;
	if(mpo_Editor->mst_SModifierDesc.i_Index < 0) return;

	if(pScrollBar)
	{
		if(pScrollBar->m_hWnd == GetDlgItem(IDC_SLIDER_VOL)->GetSafeHwnd())
		{
			pst_ExtPlayer = GetExtPlayer();
			if(!pst_ExtPlayer) return;

			pf_OldVol = &pst_ExtPlayer->f_DryVol;

			po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_VOL);
			f_NewVol = 1.0f - (float) po_Slider->GetPos() / 1000.0f;

			sprintf(tip, "%.1f dB (%.2f)", (float) SND_l_GetAttFromVol(f_NewVol) / 100.0f, f_NewVol);
			EDI_Tooltip_DisplayMessage(tip, 0);

			if(f_NewVol != *pf_OldVol)
			{
				*pf_OldVol = f_NewVol;

				/* update the far coeff */
				if(pst_ExtPlayer->f_DryVol)
				{
					po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_VOL2);
					f_NewVol = 1.0f - (float) po_Slider->GetPos() / 1000.0f;
					f_NewVol = f_NewVol / pst_ExtPlayer->f_DryVol;
					pst_ExtPlayer->f_FarCoeff = f_NewVol;
				}

				SignalChangeToEditor();
			}
		}
		else if(pScrollBar->m_hWnd == GetDlgItem(IDC_SLIDER_VOL2)->GetSafeHwnd())
		{
			pst_ExtPlayer = GetExtPlayer();
			if(!pst_ExtPlayer) return;
			if(!pst_ExtPlayer->f_DryVol) return;

			pf_OldVol = &pst_ExtPlayer->f_FarCoeff;

			po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_VOL2);
			f_NewVol = 1.0f - (float) po_Slider->GetPos() / 1000.0f;

			sprintf(tip, "%.1f dB (%.2f)", (float) SND_l_GetAttFromVol(f_NewVol) / 100.0f, f_NewVol);
			EDI_Tooltip_DisplayMessage(tip, 0);

			f_NewVol = f_NewVol / pst_ExtPlayer->f_DryVol;
			if(f_NewVol != *pf_OldVol)
			{
				*pf_OldVol = f_NewVol;
				SignalChangeToEditor();
			}
		}
		else if(pScrollBar->m_hWnd == GetDlgItem(IDC_SLIDER_FXVOLL)->GetSafeHwnd())
		{
			pst_ExtPlayer = GetExtPlayer();
			if(!pst_ExtPlayer) return;

			pf_OldVol = &pst_ExtPlayer->f_FxVolLeft;

			po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_FXVOLL);
			f_NewVol = 1.0f - (float) po_Slider->GetPos() / 2.0f;

			sprintf(tip, "%.f %%", 100.0f * f_NewVol);
			EDI_Tooltip_DisplayMessage(tip, 0);

			if(f_NewVol != *pf_OldVol)
			{
				*pf_OldVol = f_NewVol;
				SignalChangeToEditor();
			}
		}
		else if(pScrollBar->m_hWnd == GetDlgItem(IDC_SLIDER_FXVOLR)->GetSafeHwnd())
		{
			pst_ExtPlayer = GetExtPlayer();
			if(!pst_ExtPlayer) return;

			pf_OldVol = &pst_ExtPlayer->f_FxVolRight;

			po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_FXVOLR);
			f_NewVol = 1.0f - (float) po_Slider->GetPos() / 2.0f;

			sprintf(tip, "%.f %%", 100.0f * f_NewVol);
			EDI_Tooltip_DisplayMessage(tip, 0);

			if(f_NewVol != *pf_OldVol)
			{
				*pf_OldVol = f_NewVol;
				SignalChangeToEditor();
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl					*po_Slider;
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	int							*pi_OldPan;
	int							i_NewPan;
	float						*pf_OldPan;
	float						f_NewPan;
	char						tip[32];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);

	if(!SND_gst_Params.l_Available) return;
	if(mpo_Editor->mst_SModifierDesc.i_Index < 0) return;

	if(pScrollBar)
	{
		if(pScrollBar->m_hWnd == GetDlgItem(IDC_SLIDER_PAN)->GetSafeHwnd())
		{
			pst_ExtPlayer = GetExtPlayer();
			if(pst_ExtPlayer)
			{
				pi_OldPan = &pst_ExtPlayer->i_Pan;

				po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_PAN);
				i_NewPan = po_Slider->GetPos();

				sprintf(tip, "Pan = %d", i_NewPan);
				EDI_Tooltip_DisplayMessage(tip, 0);

				if(i_NewPan != *pi_OldPan)
				{
					*pi_OldPan = i_NewPan;
					SignalChangeToEditor();
				}
			}
		}
		else if(pScrollBar->m_hWnd == GetDlgItem(IDC_SLIDER_MINPAN)->GetSafeHwnd())
		{
			pst_ExtPlayer = GetExtPlayer();
			if(pst_ExtPlayer)
			{
				pf_OldPan = &pst_ExtPlayer->f_MinPan;

				po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN);
				f_NewPan = (float) po_Slider->GetPos() / 100.0f;

				sprintf(tip, "MinPan = %.2f", f_NewPan);
				EDI_Tooltip_DisplayMessage(tip, 0);

				po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN2);
				po_Slider->SetPos(100 - (int) (f_NewPan * 100.0f));

				if(f_NewPan != *pf_OldPan)
				{
					*pf_OldPan = f_NewPan;
					SignalChangeToEditor();
				}
			}
		}
		else if(pScrollBar->m_hWnd == GetDlgItem(IDC_SLIDER_MINPAN2)->GetSafeHwnd())
		{
			pst_ExtPlayer = GetExtPlayer();
			if(pst_ExtPlayer)
			{
				pf_OldPan = &pst_ExtPlayer->f_MinPan;

				po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN2);
				f_NewPan = 1.0f - (float) po_Slider->GetPos() / 100.0f;

				sprintf(tip, "MinPan = %.2f", f_NewPan);
				EDI_Tooltip_DisplayMessage(tip, 0);

				po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN);
				po_Slider->SetPos((int) (f_NewPan * 100.0f));

				if(f_NewPan != *pf_OldPan)
				{
					*pf_OldPan = f_NewPan;
					SignalChangeToEditor();
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnFxAB(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(pst_ExtPlayer->ui_PlayerFlag & SND_Cte_SModifierFxA)
	{
		pst_ExtPlayer->ui_PlayerFlag &= ~SND_Cte_SModifierFxA;
		pst_ExtPlayer->ui_PlayerFlag |= SND_Cte_SModifierFxB;
	}
	else if(pst_ExtPlayer->ui_PlayerFlag & SND_Cte_SModifierFxB)
	{
		pst_ExtPlayer->ui_PlayerFlag &= ~SND_Cte_SModifierFxA;
		pst_ExtPlayer->ui_PlayerFlag &= ~SND_Cte_SModifierFxB;
	}
	else
	{
		pst_ExtPlayer->ui_PlayerFlag |= SND_Cte_SModifierFxA;
		pst_ExtPlayer->ui_PlayerFlag &= ~SND_Cte_SModifierFxB;
	}

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnGrpA(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CButton						*po_Btn;
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	po_Btn = (CButton *) GetDlgItem(IDC_CHECK_GRPA);
	if(po_Btn->GetCheck())
		pst_ExtPlayer->ui_PlayerFlag |= SND_Cte_SModifierGrpA;
	else
		pst_ExtPlayer->ui_PlayerFlag &= ~SND_Cte_SModifierGrpA;

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnGrpB(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CButton						*po_Btn;
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	po_Btn = (CButton *) GetDlgItem(IDC_CHECK_GRPB);
	if(po_Btn->GetCheck())
		pst_ExtPlayer->ui_PlayerFlag |= SND_Cte_SModifierGrpB;
	else
		pst_ExtPlayer->ui_PlayerFlag &= ~SND_Cte_SModifierGrpB;

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnGrp2(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(SND_M_SModifierGetGroup(pst_ExtPlayer) != SND_Cte_SModifierGroupMusic)
	{
		SND_M_SModifierSetGroup(pst_ExtPlayer, SND_Cte_SModifierGroupMusic);
		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnGrp1(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(SND_M_SModifierGetGroup(pst_ExtPlayer) != SND_Cte_SModifierGroupSFX)
	{
		SND_M_SModifierSetGroup(pst_ExtPlayer, SND_Cte_SModifierGroupSFX);
		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnGrp3(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(SND_M_SModifierGetGroup(pst_ExtPlayer) != SND_Cte_SModifierGroupAmbience)
	{
		SND_M_SModifierSetGroup(pst_ExtPlayer, SND_Cte_SModifierGroupAmbience);
		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnGrp4(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(SND_M_SModifierGetGroup(pst_ExtPlayer) != SND_Cte_SModifierGroupDialog)
	{
		SND_M_SModifierSetGroup(pst_ExtPlayer, SND_Cte_SModifierGroupDialog);
		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnGrp5(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(SND_M_SModifierGetGroup(pst_ExtPlayer) != SND_Cte_SModifierGroupCutScene)
	{
		SND_M_SModifierSetGroup(pst_ExtPlayer, SND_Cte_SModifierGroupCutScene);
		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnGrp6(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(SND_M_SModifierGetGroup(pst_ExtPlayer) != SND_Cte_SModifierGroupInterface)
	{
		SND_M_SModifierSetGroup(pst_ExtPlayer, SND_Cte_SModifierGroupInterface);
		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnPlaySequential(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(pst_ExtPlayer->ui_PlayerFlag & SND_Cte_SModifierRandomPlayList)
	{
		pst_ExtPlayer->ui_PlayerFlag &= ~SND_Cte_SModifierRandomPlayList;
		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnPlayRandom(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(!(pst_ExtPlayer->ui_PlayerFlag & SND_Cte_SModifierRandomPlayList))
	{
		pst_ExtPlayer->ui_PlayerFlag |= SND_Cte_SModifierRandomPlayList;
		SignalChangeToEditor();
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnStopOnLastHit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	pst_ExtPlayer->ui_PlayerFlag ^= SND_Cte_SModifierStopOnLastHit;
	SignalChangeToEditor();	
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnDynPan(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CButton						*po_Button;
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(pst_ExtPlayer)
	{
		po_Button = (CButton *) GetDlgItem(IDC_CHECK_DYNPAN);
		pst_ExtPlayer->ul_SndFlags ^= SND_Cul_SF_DynPan;

		if(pst_ExtPlayer->ul_SndFlags & SND_Cul_SF_DynPan)
			po_Button->SetCheck(1);
		else
			po_Button->SetCheck(0);
		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnNo3D(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(int i = 0; gui_3DGroupID[i] != -1; i++)
	{
		GetDlgItem(gui_3DGroupID[i])->EnableWindow(TRUE);
		GetDlgItem(gui_3DGroupID[i])->ShowWindow(SW_SHOW);
	}

	GetDlgItem(IDC_BTN_NO3D)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_NO3D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATICnear)->SetWindowText("Near");

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	pst_ExtPlayer->ul_SndFlags &= ~SND_Cul_SF_MaskDynVol;
	pst_ExtPlayer->ul_SndFlags |= SND_Cul_SF_DynVolSpheric;
	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnSpheric(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_Play;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* get smd */
	pst_Play = GetExtPlayer();
	if(!pst_Play) return;

	pst_Play->ul_SndFlags ^= SND_Cul_SF_DynVolSpheric;
	pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolXaxis;
	pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolYaxis;
	pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolZaxis;

	SignalChangeToEditor();

	/* LINK_UpdatePointer(po_Parent->mp_Data); */
	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnAxial(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_Play;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* get smd */
	pst_Play = GetExtPlayer();
	if(!pst_Play) return;

	pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolSpheric;
	pst_Play->ul_SndFlags ^= SND_Cul_SF_DynVolXaxis;
	pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolYaxis;
	pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolZaxis;

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnX(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_Play;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* get smd */
	pst_Play = GetExtPlayer();
	if(!pst_Play) return;

	pst_Play->ul_SndFlags ^= SND_Cul_SF_DynVolXaxis;
	if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolXaxis)
	{
		if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolSpheric)
		{
			pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolZaxis;
			pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolYaxis;
		}
	}

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnY(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_Play;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* get smd */
	pst_Play = GetExtPlayer();
	if(!pst_Play) return;

	pst_Play->ul_SndFlags ^= SND_Cul_SF_DynVolYaxis;
	if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolYaxis)
	{
		if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolSpheric)
		{
			pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolXaxis;
			pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolZaxis;
		}
	}

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnZ(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_Play;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* get smd */
	pst_Play = GetExtPlayer();
	if(!pst_Play) return;

	pst_Play->ul_SndFlags ^= SND_Cul_SF_DynVolZaxis;
	if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolZaxis)
	{
		if(pst_Play->ul_SndFlags & SND_Cul_SF_DynVolSpheric)
		{
			pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolXaxis;
			pst_Play->ul_SndFlags &= ~SND_Cul_SF_DynVolYaxis;
		}
	}

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnFadeInChange(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	EDIA_cl_FileDialog			o_File
								(
									"Choose File",
									0,
									0,
									1,
									NULL,
									"*"EDI_Csz_ExtSoundInsert ",*"EDI_Csz_ExtSoundFade
								);
	CString						o_Temp;
	ULONG						ul_Index;
	CButton						*po_Btn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_File.mb_UseFavorite = TRUE;
	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	/*$1- assign a fade in file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		InitFadeIn();
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(ul_Index == BIG_C_InvalidIndex) return;

		pst_ExtPlayer->pst_FadeIn = NULL;
		pst_ExtPlayer->ul_FadeInKey = BIG_FileKey(ul_Index);
		LOA_MakeFileRef
		(
			pst_ExtPlayer->ul_FadeInKey,
			(ULONG *) &pst_ExtPlayer->pst_FadeIn,
			SND_ul_CallbackInsertLoad,
			LOA_C_MustExists | LOA_C_HasUserCounter
		);
		LOA_Resolve();
		ESON_LoadAllSounds();

		po_Btn = (CButton *) GetDlgItem(IDC_BTN_FADEI);
		po_Btn->SetWindowText(o_Temp);

		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnFadeIn(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	EDIA_cl_FileDialog			o_File
								(
									"Choose File",
									0,
									0,
									1,
									NULL,
									"*"EDI_Csz_ExtSoundInsert ",*"EDI_Csz_ExtSoundFade
								);
	CString						o_Temp;
	ULONG						ul_Index;
	CButton						*po_Btn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_File.mb_UseFavorite = TRUE;
	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	/*$1- just find the file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_ExtPlayer->ul_FadeInKey == BIG_C_InvalidKey)
	{

		/*$1- assign a fade in file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(o_File.DoModal() == IDOK)
		{
			o_File.GetItem(o_File.mo_File, 0, o_Temp);
			ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
			if(ul_Index == BIG_C_InvalidIndex) return;

			pst_ExtPlayer->pst_FadeIn = NULL;
			pst_ExtPlayer->ul_FadeInKey = BIG_FileKey(ul_Index);
			LOA_MakeFileRef
			(
				pst_ExtPlayer->ul_FadeInKey,
				(ULONG *) &pst_ExtPlayer->pst_FadeIn,
				SND_ul_CallbackInsertLoad,
				LOA_C_MustExists | LOA_C_HasUserCounter
			);
			LOA_Resolve();
			ESON_LoadAllSounds();

			po_Btn = (CButton *) GetDlgItem(IDC_BTN_FADEI);
			po_Btn->SetWindowText(o_Temp);

			SignalChangeToEditor();
		}
	}
	else
	{

		/*$1- edit fade file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		mpo_Editor->Insert_OnOpen(BIG_ul_SearchKeyToFat(pst_ExtPlayer->ul_FadeInKey));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnFadeOutChange(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	EDIA_cl_FileDialog			o_File
								(
									"Choose File",
									0,
									0,
									1,
									NULL,
									"*"EDI_Csz_ExtSoundInsert ",*"EDI_Csz_ExtSoundFade
								);
	CString						o_Temp;
	ULONG						ul_Index;
	CButton						*po_Btn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_File.mb_UseFavorite = TRUE;
	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	/*$1- assign a fade in file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		InitFadeOut();
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(ul_Index == BIG_C_InvalidIndex) return;

		pst_ExtPlayer->pst_FadeOut = NULL;
		pst_ExtPlayer->ul_FadeOutKey = BIG_FileKey(ul_Index);
		LOA_MakeFileRef
		(
			pst_ExtPlayer->ul_FadeOutKey,
			(ULONG *) &pst_ExtPlayer->pst_FadeOut,
			SND_ul_CallbackInsertLoad,
			LOA_C_MustExists | LOA_C_HasUserCounter
		);
		LOA_Resolve();
		ESON_LoadAllSounds();

		po_Btn = (CButton *) GetDlgItem(IDC_BTN_FADEO);
		po_Btn->SetWindowText(o_Temp);

		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnFadeOut(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	EDIA_cl_FileDialog			o_File
								(
									"Choose File",
									0,
									0,
									1,
									NULL,
									"*"EDI_Csz_ExtSoundInsert ",*"EDI_Csz_ExtSoundFade
								);
	CString						o_Temp;
	ULONG						ul_Index;
	CButton						*po_Btn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_File.mb_UseFavorite = TRUE;
	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(pst_ExtPlayer->ul_FadeOutKey == BIG_C_InvalidKey)
	{

		/*$1- assign a fade in file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(o_File.DoModal() == IDOK)
		{
			o_File.GetItem(o_File.mo_File, 0, o_Temp);
			ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
			if(ul_Index == BIG_C_InvalidIndex) return;

			pst_ExtPlayer->pst_FadeOut = NULL;
			pst_ExtPlayer->ul_FadeOutKey = BIG_FileKey(ul_Index);
			LOA_MakeFileRef
			(
				pst_ExtPlayer->ul_FadeOutKey,
				(ULONG *) &pst_ExtPlayer->pst_FadeOut,
				SND_ul_CallbackInsertLoad,
				LOA_C_MustExists | LOA_C_HasUserCounter
			);
			LOA_Resolve();
			ESON_LoadAllSounds();

			po_Btn = (CButton *) GetDlgItem(IDC_BTN_FADEO);
			po_Btn->SetWindowText(o_Temp);

			SignalChangeToEditor();
		}
	}
	else
	{

		/*$1- edit fade file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		mpo_Editor->Insert_OnOpen(BIG_ul_SearchKeyToFat(pst_ExtPlayer->ul_FadeOutKey));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnFind(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EBRO_cl_Frame	*po_Browser;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpo_Editor->mst_SModifierDesc.ul_Fat == -1) return;

	po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
	po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
	po_Browser->i_OnMessage
		(
			EDI_MESSAGE_SELFILE,
			BIG_ParentFile(mpo_Editor->mst_SModifierDesc.ul_Fat),
			mpo_Editor->mst_SModifierDesc.ul_Fat
		);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnFind2(ULONG ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EBRO_cl_Frame	*po_Browser;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ul_Key == -1) return;

	ul_Key = BIG_ul_SearchKeyToFat(ul_Key);
	if(ul_Key == -1) return;

	po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
	po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
	po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ul_Key), ul_Key);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeXNear(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_XN, &pst_ExtPlay->af_Near[0], 0.0f, pst_ExtPlay->af_Far[0]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeXMiddle(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_XM, &pst_ExtPlay->af_MiddleBlend[0], 0.0f, 1.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeXFar(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_XF, &pst_ExtPlay->af_Far[0], pst_ExtPlay->af_Near[0], 10000000.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeYNear(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_YN, &pst_ExtPlay->af_Near[1], 0.0f, pst_ExtPlay->af_Far[1]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeYMiddle(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_YM, &pst_ExtPlay->af_MiddleBlend[1], 0.0f, 1.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeYFar(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_YF, &pst_ExtPlay->af_Far[1], pst_ExtPlay->af_Near[1], 10000000.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeZNear(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_ZN, &pst_ExtPlay->af_Near[2], 0.0f, pst_ExtPlay->af_Far[2]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeZMiddle(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_ZM, &pst_ExtPlay->af_MiddleBlend[2], 0.0f, 1.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeZFar(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_ZF, &pst_ExtPlay->af_Far[2], pst_ExtPlay->af_Near[2], 10000000.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeMiddleFactor(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_MFACT, &pst_ExtPlay->f_MiddleCoeff, 0.0f, 1.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeCylinder(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_CYLINDER, &pst_ExtPlay->f_CylinderHeight, 0.0f, 10000000.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeVolMin(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay && pst_ExtPlay->f_DryVol)
	{
		OnEditChange(IDC_EDIT_VOLMIN, &pst_ExtPlay->f_DryVol_FactMin, 0.0f, 10.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeVolMax(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(pst_ExtPlay && pst_ExtPlay->f_DryVol)
	{
		OnEditChange(IDC_EDIT_VOLMAX, &pst_ExtPlay->f_DryVol_FactMax, 0.0f, 10.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeFreq(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();

	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_FREQ, (float *) &pst_ExtPlay->f_FreqCoef, 0.f, 10.f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeFreqMin(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;

	OnEditChange(IDC_EDIT_FREQMIN, &pst_ExtPlay->f_Freq_FactMin, 0.0f, 1.0f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeFreqMax(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;

	OnEditChange(IDC_EDIT_FREQMAX, &pst_ExtPlay->f_Freq_FactMax, 0.0f, 100.0f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnChangeDoppler(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();

	if(pst_ExtPlay)
	{
		OnEditChange(IDC_EDIT_DOPPLER, &pst_ExtPlay->f_Doppler, 0.0f, 20.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::SignalChangeToEditor(void)
{
	mpo_Editor->mst_SModifierDesc.b_NeedSaving = TRUE;
	mpo_Editor->i_OnMessage(ESON_MESSAGE_REFRESH_SMODIFIER_TITLE, 0, 0);
	UpdateExtendedPlayer();

	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SModifierExtPlayer *ESON_cl_ViewSmd::GetExtPlayer(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifier	*pst_SModifier;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return NULL;
	if(!mpo_Editor) return NULL;
	if(mpo_Editor->mst_SModifierDesc.i_Index >= SND_gst_Params.l_SoundNumber) return NULL;
	if(mpo_Editor->mst_SModifierDesc.i_Index < 0) return NULL;

	if((SND_gst_Params.dst_Sound[mpo_Editor->mst_SModifierDesc.i_Index].ul_Flags & SND_Cul_DSF_Used) == 0)
		return NULL;

	pst_SModifier = SND_pst_SModifierGet
		(
			SND_gst_Params.dst_Sound[mpo_Editor->mst_SModifierDesc.i_Index].pst_SModifier,
			SND_Cte_SModifierExtPlayer,
			0
		);

	if(pst_SModifier) return (SND_tdst_SModifierExtPlayer *) pst_SModifier->pv_Data;

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnEditChange(int id, float *pf_Val, float f_Min, float f_Max)
{
	/*~~~~~~~~~~~~~~~~~*/
	CEdit	*po_Edit;
	char	asz_Temp[32];
	float	f;
	/*~~~~~~~~~~~~~~~~~*/

	po_Edit = (CEdit *) GetDlgItem(id);
	if(po_Edit->GetWindowText(asz_Temp, 32) < 1) return;
	sscanf(asz_Temp, "%f", &f);

	if(f < f_Min) f = f_Min;
	if(f > f_Max) f = f_Max;
	if(*pf_Val == f) return;
	*pf_Val = f;

	sprintf(asz_Temp, "%.2f", f);
	po_Edit->SetWindowText(asz_Temp);

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnEditChange(int id, int *pi_Val, int i_Min, int i_Max)
{
	/*~~~~~~~~~~~~~~~~~*/
	CEdit	*po_Edit;
	char	asz_Temp[32];
	int		i;
	/*~~~~~~~~~~~~~~~~~*/

	po_Edit = (CEdit *) GetDlgItem(id);
	if(po_Edit->GetWindowText(asz_Temp, 32) < 1) return;
	sscanf(asz_Temp, "%d", &i);

	if(i < i_Min) i = i_Min;
	if(i > i_Max) i = i_Max;
	if(*pi_Val == i) return;
	*pi_Val = i;

	sprintf(asz_Temp, "%d", i);
	po_Edit->SetWindowText(asz_Temp);

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_ViewSmd::PreTranslateMessage(MSG *msg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox					*po_LC;
	int							i_Item;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	ULONG						ulFat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(msg->message == WM_LBUTTONUP)
	{

		/*$1- play when left button up from vol slider ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(msg->hwnd == GetDlgItem(IDC_SLIDER_VOL)->GetSafeHwnd())
		{
			mpo_Editor->OnAction(ESON_ACTION_SMODIFIER_PLAY);
			return 0;
		}
	}
	else if(msg->message == WM_LBUTTONDOWN)
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    short cut
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		if(GetAsyncKeyState(VK_CONTROL) < 0)
		{
			if(msg->hwnd == GetDlgItem(IDC_EDIT_FREQMIN)->GetSafeHwnd())
			{
				InitRandomFreq();
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_FREQMAX)->GetSafeHwnd())
			{
				InitRandomFreq();
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_VOLMIN)->GetSafeHwnd())
			{
				InitRandomVol();
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_VOLMAX)->GetSafeHwnd())
			{
				InitRandomVol();
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_SLIDER_PAN)->GetSafeHwnd())
			{
				InitPan();
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_SLIDER_MINPAN)->GetSafeHwnd())
			{
				InitMinPan();
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_SLIDER_MINPAN2)->GetSafeHwnd())
			{
				InitMinPan();
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_FREQ)->GetSafeHwnd())
			{
				InitFreqFromFile(0);
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_DOPPLER)->GetSafeHwnd())
			{
				InitDoppler();
				return 0;
			}
		}
		else if(GetAsyncKeyState(VK_SHIFT) < 0)
		{
			if(msg->hwnd == GetDlgItem(IDC_EDIT_FREQMIN)->GetSafeHwnd())
			{
				DefaultRandomFreq();
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_FREQMAX)->GetSafeHwnd())
			{
				DefaultRandomFreq();
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_VOLMIN)->GetSafeHwnd())
			{
				DefaultRandomVol();
				return 0;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_VOLMAX)->GetSafeHwnd())
			{
				DefaultRandomVol();
				return 0;
			}
		}
	}
	else if(msg->message == WM_MOUSEMOVE)
	{

		/*$1- tool tips ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(msg->hwnd == GetDlgItem(IDC_RADIOGRP1)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("SFX", 500);
			return 0;
		}
		else if(msg->hwnd == GetDlgItem(IDC_RADIOGRP2)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("Music", 500);
			return 0;
		}
		else if(msg->hwnd == GetDlgItem(IDC_RADIOGRP3)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("Ambience", 500);
			return 0;
		}
		else if(msg->hwnd == GetDlgItem(IDC_RADIOGRP4)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("Dialog", 500);
			return 0;
		}
		else if(msg->hwnd == GetDlgItem(IDC_RADIOGRP5)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("Cut Scene", 500);
			return 0;
		}
		else if(msg->hwnd == GetDlgItem(IDC_RADIOGRP6)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("Interface", 500);
			return 0;
		}
		else if(msg->hwnd == GetDlgItem(IDC_CHECK_GRPA)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("Group A", 500);
			return 0;
		}
		else if(msg->hwnd == GetDlgItem(IDC_CHECK_GRPB)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("Group B", 500);
			return 0;
		}
	}
	else if(msg->message == WM_KEYDOWN)
	{

		/*$1- get edit value when return or tab pressed ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if((msg->wParam == VK_RETURN) || (msg->wParam == VK_TAB))
		{
			if(msg->hwnd == GetDlgItem(IDC_EDIT_XN)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_XF)->SetFocus();
				GetDlgItem(IDC_EDIT_XN)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_XN))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_XM)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_XF)->SetFocus();
				GetDlgItem(IDC_EDIT_XM)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_XM))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_XF)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_XN)->SetFocus();
				GetDlgItem(IDC_EDIT_XF)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_XF))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_YN)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_YM)->SetFocus();
				GetDlgItem(IDC_EDIT_YN)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_YN))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_YM)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_YF)->SetFocus();
				GetDlgItem(IDC_EDIT_YM)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_YM))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_YF)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_YN)->SetFocus();
				GetDlgItem(IDC_EDIT_YF)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_YF))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_ZN)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_ZM)->SetFocus();
				GetDlgItem(IDC_EDIT_ZN)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_ZN))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_ZM)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_ZF)->SetFocus();
				GetDlgItem(IDC_EDIT_ZM)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_ZM))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_ZF)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_ZN)->SetFocus();
				GetDlgItem(IDC_EDIT_ZF)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_ZF))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_MFACT)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_ZF)->SetFocus();
				GetDlgItem(IDC_EDIT_MFACT)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_MFACT))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_CYLINDER)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_DOPPLER)->SetFocus();
				GetDlgItem(IDC_EDIT_CYLINDER)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_CYLINDER))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_VOLMIN)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_DOPPLER)->SetFocus();
				GetDlgItem(IDC_EDIT_VOLMIN)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_VOLMIN))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_VOLMAX)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_DOPPLER)->SetFocus();
				GetDlgItem(IDC_EDIT_VOLMAX)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_VOLMAX))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_FREQ)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_MFACT)->SetFocus();
				GetDlgItem(IDC_EDIT_FREQ)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_FREQ))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_FREQMIN)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_MFACT)->SetFocus();
				GetDlgItem(IDC_EDIT_FREQMIN)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_FREQMIN))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_FREQMAX)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_MFACT)->SetFocus();
				GetDlgItem(IDC_EDIT_FREQMAX)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_FREQMAX))->SetSel(0, -1);
				return 1;
			}
			else if(msg->hwnd == GetDlgItem(IDC_EDIT_DOPPLER)->GetSafeHwnd())
			{
				GetDlgItem(IDC_EDIT_MFACT)->SetFocus();
				GetDlgItem(IDC_EDIT_DOPPLER)->SetFocus();
				((CEdit *) GetDlgItem(IDC_EDIT_DOPPLER))->SetSel(0, -1);
				return 1;
			}
		}
	}
	else if(msg->message == WM_RBUTTONDOWN)
	{
		if(msg->hwnd == GetDlgItem(IDC_LIST_PLAY)->GetSafeHwnd())
		{
			OnRButtonDownPlayList();
			return 1;
		}
		else if(msg->hwnd == GetDlgItem(IDC_LIST_INSERT)->GetSafeHwnd())
		{
			OnRButtonDownInsertList();
			return 1;
		}
	}
	else if(msg->message == WM_LBUTTONDBLCLK)
	{
		if(msg->hwnd == GetDlgItem(IDC_LIST_PLAY)->GetSafeHwnd())
		{
			pst_ExtPlay = GetExtPlayer();
			if(pst_ExtPlay && pst_ExtPlay->st_PlayList.aul_KeyList)
			{
				po_LC = (CListBox *) GetDlgItem(IDC_LIST_PLAY);
				i_Item = po_LC->GetCurSel();

				if(i_Item != LB_ERR)
				{
					ulFat = BIG_ul_SearchKeyToFat(pst_ExtPlay->st_PlayList.aul_KeyList[i_Item]);
					mpo_Editor->Sound_Set(ulFat);
					return 1;
				}
			}
		}
		else if(msg->hwnd == GetDlgItem(IDC_LIST_INSERT)->GetSafeHwnd())
		{
			pst_ExtPlay = GetExtPlayer();
			if(pst_ExtPlay && pst_ExtPlay->st_InsertList.aul_KeyList)
			{
				po_LC = (CListBox *) GetDlgItem(IDC_LIST_INSERT);
				i_Item = po_LC->GetCurSel();

				if(i_Item != LB_ERR)
				{
					ulFat = BIG_ul_SearchKeyToFat(pst_ExtPlay->st_InsertList.aul_KeyList[i_Item]);
					mpo_Editor->Insert_OnOpen(ulFat);
					return 1;
				}
			}
		}
	}

	return CFormView::PreTranslateMessage(msg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC			*pdc;
	RECT		rc1;
	COLORREF	color = ::GetSysColor(COLOR_3DSHADOW);
	int			iID[] = { IDC_BTN_TOP1, IDC_BTN_TOP3, IDC_BTN_TOP2, IDC_BTN_TOP6, -1 };
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CFormView::OnPaint();
	pdc = GetDC();

	for(i = 0; iID[i] != -1; i++)
	{
		GetDlgItem(iID[i])->GetClientRect(&rc1);
		GetDlgItem(iID[i])->ClientToScreen(&rc1);
		ScreenToClient(&rc1);
		pdc->Draw3dRect(&rc1, color, color);
	}

	ReleaseDC(pdc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InitPan(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl					*po_Slider;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_PAN);
	po_Slider->SetPos(0);
	pst_ExtPlay->i_Pan = 0;

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InitMinPan(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl					*po_Slider;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN);
	po_Slider->SetPos(0);
	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MINPAN2);
	po_Slider->SetPos(100);

	pst_ExtPlay->f_MinPan = 0.0f;

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InitDoppler(void)
{
	M_EnableCEdit(IDC_EDIT_DOPPLER, "1.00");
	OnChangeDoppler();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InitRandomFreq(void)
{
	M_EnableCEdit(IDC_EDIT_FREQMIN, "0.00");
	OnChangeFreqMin();
	M_EnableCEdit(IDC_EDIT_FREQMAX, "0.00");
	OnChangeFreqMax();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::DefaultRandomFreq(void)
{
	M_EnableCEdit(IDC_EDIT_FREQMIN, "0.95");
	OnChangeFreqMin();
	M_EnableCEdit(IDC_EDIT_FREQMAX, "1.00");
	OnChangeFreqMax();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InitVol(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl					*po_Slider;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_VOL);
	po_Slider->SetPos(500);
	pst_ExtPlay->f_DryVol = 0.5f;

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InitRandomVol(void)
{
	/*~~~~~~~~~~~~*/
	char	asz[32];
	/*~~~~~~~~~~~~*/

	sprintf(asz, "0.00");

	M_EnableCEdit(IDC_EDIT_VOLMIN, asz);
	OnChangeVolMin();
	M_EnableCEdit(IDC_EDIT_VOLMAX, asz);
	OnChangeVolMax();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::DefaultRandomVol(void)
{
	M_EnableCEdit(IDC_EDIT_VOLMIN, "0.95");
	OnChangeVolMin();
	M_EnableCEdit(IDC_EDIT_VOLMAX, "1.00");
	OnChangeVolMax();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InitFadeIn(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;
	if(pst_ExtPlay->ul_FadeInKey == BIG_C_InvalidKey) return;
	SND_InsertUnload(SND_i_InsertGetIndex(pst_ExtPlay->ul_FadeInKey));

	pst_ExtPlay->ul_FadeInKey = BIG_C_InvalidKey;
	pst_ExtPlay->pst_FadeIn = NULL;

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InitFadeOut(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;
	if(pst_ExtPlay->ul_FadeOutKey == BIG_C_InvalidKey) return;
	SND_InsertUnload(SND_i_InsertGetIndex(pst_ExtPlay->ul_FadeOutKey));

	pst_ExtPlay->ul_FadeOutKey = BIG_C_InvalidKey;
	pst_ExtPlay->pst_FadeOut = NULL;

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InitFreqFromFile(int cmd)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	SND_tdst_OneSound			*pst_Sound;
	char						asz[64];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;

	if(!cmd)
	{
		if(pst_ExtPlay->i_SndIndex < 0)
		{
			if(pst_ExtPlay->st_PlayList.us_Size)
			{
				pst_ExtPlay->i_SndIndex = SND_l_GetSoundIndex(pst_ExtPlay->st_PlayList.aul_KeyList[0]);
			}
		}

		if(pst_ExtPlay->i_SndIndex < 0) return;
		SND_M_GetSoundOrReturn(pst_ExtPlay->i_SndIndex, pst_Sound, ;);

		if(!pst_Sound->pst_Wave) return;
		pst_ExtPlay->f_FreqCoef = 1.f;

		sprintf(asz, "%1.2f", pst_ExtPlay->f_FreqCoef);
		M_EnableCEdit(IDC_EDIT_FREQ, asz);
	}
	else
	{
		sprintf(asz, "0");
		M_EnableCEdit(IDC_EDIT_FREQ, asz);
	}

	OnChangeFreq();
	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::PlayListInit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox					*po_List;
	ULONG						ul_Res;
	int							top;
	CString						o_line;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	unsigned short				i;
	ULONG						ulFat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();

	/* Take care of init list */
	po_List = (CListBox *) GetDlgItem(IDC_LIST_PLAY);
	top = po_List->GetTopIndex();

	po_List->SetRedraw(FALSE);
	po_List->ResetContent();

	if(pst_ExtPlay)
	{
		for(i = 0; i < pst_ExtPlay->st_PlayList.us_Size; i++)
		{
			ulFat = BIG_ul_SearchKeyToFat(pst_ExtPlay->st_PlayList.aul_KeyList[i]);
			if(ulFat == BIG_C_InvalidIndex)
				o_line.Format("[%d] - <hole>", i);
			else
				o_line.Format("[%d] - %s", i, BIG_NameFile(ulFat));

			ul_Res = po_List->AddString(o_line);
		}
	}

	po_List->SetTopIndex(top);
	po_List->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnPlayListAdd(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	BIG_KEY						ul_key;
	int							i_SModifierSoundIndex;
	EDIA_cl_FileDialog			o_File
								(
									"Choose File",
									0,
									0,
									1,
									NULL,
									"*"EDI_Csz_ExtSoundFile ",*"EDI_Csz_ExtLoadingSound ",*"EDI_Csz_ExtSoundMusic ",*"EDI_Csz_ExtSoundAmbience
										",*"EDI_Csz_ExtSoundDialog
								);
	CString						o_Temp;
	ULONG						ul_Index;
	ULONG						*pul_Flags;
	BIG_KEY						*pNewKey;
	int							type, previous_type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_File.mb_UseFavorite = TRUE;
	pst_ExtPlayer = GetExtPlayer();
	pul_Flags = NULL;
	if(!pst_ExtPlayer) return;

	if(o_File.DoModal() == IDOK)
	{

		/*$1- get file fat index ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(ul_Index == BIG_C_InvalidIndex) return;
		ul_key = BIG_FileKey(ul_Index);

		/*$1- get file type ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtSoundMusic))
			type = SND_Cul_SF_Music;
		else if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtSoundAmbience))
			type = SND_Cul_SF_Ambience;
		else if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtSoundDialog))
			type = SND_Cul_SF_Dialog;
		else if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtSModifier))
			type = SND_Cul_SF_SModifier;
		else if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtSoundFile))
			type = 0;
		else
			return;

		/*$1- check that it has the same type ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(pst_ExtPlayer->st_PlayList.us_Size)
			previous_type = pst_ExtPlayer->st_PlayList.aul_FlagList[0] & SND_Cul_SF_MaskFileType;
		else
			previous_type = type;

		if(previous_type != type)
		{
			MessageBox("All files in the play list must have the same extension.", "Error", MB_OK | MB_ICONSTOP);
			return;
		}

		/*$1- alloc space for storing data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_ExtPlayer->st_PlayList.us_Size++;
		if(pst_ExtPlayer->st_PlayList.al_IdxList)
		{
			pst_ExtPlayer->st_PlayList.al_IdxList = (LONG *) MEM_p_Realloc
				(
					pst_ExtPlayer->st_PlayList.al_IdxList,
					pst_ExtPlayer->st_PlayList.us_Size * sizeof(LONG)
				);
			pst_ExtPlayer->st_PlayList.aul_FlagList = (ULONG *) MEM_p_Realloc
				(
					pst_ExtPlayer->st_PlayList.aul_FlagList,
					pst_ExtPlayer->st_PlayList.us_Size * sizeof(ULONG)
				);
			pst_ExtPlayer->st_PlayList.aul_KeyList = (BIG_KEY *) MEM_p_Realloc
				(
					pst_ExtPlayer->st_PlayList.aul_KeyList,
					pst_ExtPlayer->st_PlayList.us_Size * sizeof(BIG_KEY)
				);
		}
		else
		{
			pst_ExtPlayer->st_PlayList.al_IdxList = (LONG *) MEM_p_Alloc(pst_ExtPlayer->st_PlayList.us_Size * sizeof(LONG));
			pst_ExtPlayer->st_PlayList.aul_FlagList = (ULONG *) MEM_p_Alloc(pst_ExtPlayer->st_PlayList.us_Size * sizeof(ULONG));
			pst_ExtPlayer->st_PlayList.aul_KeyList = (BIG_KEY *) MEM_p_Alloc(pst_ExtPlayer->st_PlayList.us_Size * sizeof(BIG_KEY));
		}

		pst_ExtPlayer->st_PlayList.aul_FlagList[pst_ExtPlayer->st_PlayList.us_Size - 1] = 0;
		pst_ExtPlayer->st_PlayList.aul_KeyList[pst_ExtPlayer->st_PlayList.us_Size - 1] = -1;
		pst_ExtPlayer->st_PlayList.al_IdxList[pst_ExtPlayer->st_PlayList.us_Size - 1] = -1;

		/*$1- assign new values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pul_Flags = &pst_ExtPlayer->st_PlayList.aul_FlagList[pst_ExtPlayer->st_PlayList.us_Size - 1];
		pNewKey = &pst_ExtPlayer->st_PlayList.aul_KeyList[pst_ExtPlayer->st_PlayList.us_Size - 1];

		*pul_Flags |= type;
		*pNewKey = ul_key;

		/*$1- get the new sound ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(type & SND_Cul_SF_Music)
			i_SModifierSoundIndex = SND_l_AddMusic(ul_key);
		else if(type & SND_Cul_SF_Ambience)
			i_SModifierSoundIndex = SND_l_AddAmbience(ul_key);
		else if(type & SND_Cul_SF_Dialog)
			i_SModifierSoundIndex = SND_l_AddDialog(ul_key);
		else if(type & SND_Cul_SF_LoadingSound)
			i_SModifierSoundIndex = SND_l_AddLoadingSound(ul_key);
		else
			i_SModifierSoundIndex = SND_l_AddSound(ul_key);

		if(i_SModifierSoundIndex < 0)
		{
			ERR_X_Warning(0, "[SND] Can not load the new sound, operation failed !", NULL);
			return;
		}
		else
		{
			LOA_Resolve();
			ESON_LoadAllSounds();
			pst_ExtPlayer->st_PlayList.al_IdxList[pst_ExtPlayer->st_PlayList.us_Size - 1] = i_SModifierSoundIndex;
		}

		/*$1- update display ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		PlayListInit();
		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnPlayListInsertHole(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	int							type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = GetExtPlayer();
	if(!pst_ExtPlayer) return;

	if(pst_ExtPlayer->st_PlayList.us_Size)
		type = pst_ExtPlayer->st_PlayList.aul_FlagList[0] & SND_Cul_SF_MaskFileType;
	else
		type = 0;

	/*$1- alloc space for storing data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer->st_PlayList.us_Size++;
	if(pst_ExtPlayer->st_PlayList.al_IdxList)
	{
		pst_ExtPlayer->st_PlayList.al_IdxList = (LONG *) MEM_p_Realloc
			(
				pst_ExtPlayer->st_PlayList.al_IdxList,
				pst_ExtPlayer->st_PlayList.us_Size * sizeof(LONG)
			);
		pst_ExtPlayer->st_PlayList.aul_FlagList = (ULONG *) MEM_p_Realloc
			(
				pst_ExtPlayer->st_PlayList.aul_FlagList,
				pst_ExtPlayer->st_PlayList.us_Size * sizeof(ULONG)
			);
		pst_ExtPlayer->st_PlayList.aul_KeyList = (BIG_KEY *) MEM_p_Realloc
			(
				pst_ExtPlayer->st_PlayList.aul_KeyList,
				pst_ExtPlayer->st_PlayList.us_Size * sizeof(BIG_KEY)
			);
	}
	else
	{
		pst_ExtPlayer->st_PlayList.al_IdxList = (LONG *) MEM_p_Alloc(pst_ExtPlayer->st_PlayList.us_Size * sizeof(LONG));
		pst_ExtPlayer->st_PlayList.aul_FlagList = (ULONG *) MEM_p_Alloc(pst_ExtPlayer->st_PlayList.us_Size * sizeof(ULONG));
		pst_ExtPlayer->st_PlayList.aul_KeyList = (BIG_KEY *) MEM_p_Alloc(pst_ExtPlayer->st_PlayList.us_Size * sizeof(BIG_KEY));
	}

	pst_ExtPlayer->st_PlayList.aul_FlagList[pst_ExtPlayer->st_PlayList.us_Size - 1] = type;
	pst_ExtPlayer->st_PlayList.aul_KeyList[pst_ExtPlayer->st_PlayList.us_Size - 1] = -1;
	pst_ExtPlayer->st_PlayList.al_IdxList[pst_ExtPlayer->st_PlayList.us_Size - 1] = -1;

	/*$1- update display ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	PlayListInit();
	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnPlayListDel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox					*po_List;
	int							iSel, iSrc, iDst, iSize, iDeleted;
	int							ai_Sel[10000];
	int							iNumSel;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;

	po_List = (CListBox *) GetDlgItem(IDC_LIST_PLAY);
	iNumSel = po_List->GetSelItems(10000, ai_Sel);

	iDeleted = 0;
	for(iSel = 0; iSel < iNumSel; iSel++)
	{
		iDst = ai_Sel[iSel] - iDeleted;
		SND_DeleteByIndex(pst_ExtPlay->st_PlayList.al_IdxList[iDst]);

		iSrc = iDst + 1;
		iSize = (int) pst_ExtPlay->st_PlayList.us_Size;

		if(iSrc < iSize)
		{
			L_memcpy
			(
				&pst_ExtPlay->st_PlayList.al_IdxList[iDst],
				&pst_ExtPlay->st_PlayList.al_IdxList[iSrc],
				(iSize - iSrc) * sizeof(LONG)
			);
			L_memcpy
			(
				&pst_ExtPlay->st_PlayList.aul_FlagList[iDst],
				&pst_ExtPlay->st_PlayList.aul_FlagList[iSrc],
				(iSize - iSrc) * sizeof(ULONG)
			);
			L_memcpy
			(
				&pst_ExtPlay->st_PlayList.aul_KeyList[iDst],
				&pst_ExtPlay->st_PlayList.aul_KeyList[iSrc],
				(iSize - iSrc) * sizeof(BIG_KEY)
			);
		}

		pst_ExtPlay->st_PlayList.us_Size--;
		iDeleted++;
	}

	PlayListInit();
	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::PlayListSwapFile(int i1, int i2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	LONG						lIdx;
	ULONG						ulFlag;
	ULONG						ulKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(i1 < 0) return;
	if(i2 < 0) return;

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;
	if((int) pst_ExtPlay->st_PlayList.us_Size <= i1) return;
	if((int) pst_ExtPlay->st_PlayList.us_Size <= i2) return;

	lIdx = pst_ExtPlay->st_PlayList.al_IdxList[i1];
	ulFlag = pst_ExtPlay->st_PlayList.aul_FlagList[i1];
	ulKey = pst_ExtPlay->st_PlayList.aul_KeyList[i1];

	pst_ExtPlay->st_PlayList.al_IdxList[i1] = pst_ExtPlay->st_PlayList.al_IdxList[i2];
	pst_ExtPlay->st_PlayList.aul_FlagList[i1] = pst_ExtPlay->st_PlayList.aul_FlagList[i2];
	pst_ExtPlay->st_PlayList.aul_KeyList[i1] = pst_ExtPlay->st_PlayList.aul_KeyList[i2];

	pst_ExtPlay->st_PlayList.al_IdxList[i2] = lIdx;
	pst_ExtPlay->st_PlayList.aul_FlagList[i2] = ulFlag;
	pst_ExtPlay->st_PlayList.aul_KeyList[i2] = ulKey;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnPlayListMoveUp(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_List;
	int			ai_Sel[10000];
	int			iNumSel;
	int			iSel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST_PLAY);
	if(po_List->GetSelCount() == 0) return;
	if(po_List->GetSel(0)) return;

	iNumSel = po_List->GetSelItems(10000, ai_Sel);
	for(iSel = 1; iSel < po_List->GetCount(); iSel++)
	{
		if(po_List->GetSel(iSel))
		{
			PlayListSwapFile(iSel, iSel - 1);
			po_List->SetSel(iSel - 1, TRUE);
			po_List->SetSel(iSel, FALSE);
		}
	}

	PlayListInit();

	for(iSel = 0; iSel < iNumSel; iSel++)
		if(ai_Sel[iSel]) po_List->SetSel(ai_Sel[iSel] - 1, TRUE);

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnPlayListMoveDown(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_List;
	int			ai_Sel[10000];
	int			iNumSel;
	int			iSel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST_PLAY);
	if(po_List->GetSelCount() == 0) return;
	if(po_List->GetSel(po_List->GetCount() - 1)) return;

	iNumSel = po_List->GetSelItems(10000, ai_Sel);
	for(iSel = po_List->GetCount() - 2; iSel >= 0; iSel--)
	{
		if(po_List->GetSel(iSel))
		{
			PlayListSwapFile(iSel, iSel + 1);
			po_List->SetSel(iSel + 1, TRUE);
			po_List->SetSel(iSel, FALSE);
		}
	}

	PlayListInit();

	for(iSel = 0; iSel < iNumSel; iSel++)
		if(ai_Sel[iSel] != po_List->GetSelCount() - 1) po_List->SetSel(ai_Sel[iSel] + 1, TRUE);
	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InsertListInit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox					*po_List;
	ULONG						ul_Res;
	int							top;
	CString						o_line;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	unsigned short				i;
	ULONG						ulFat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();

	/* Take care of init list */
	po_List = (CListBox *) GetDlgItem(IDC_LIST_INSERT);
	top = po_List->GetTopIndex();

	po_List->SetRedraw(FALSE);
	po_List->ResetContent();

	if(pst_ExtPlay)
	{
		for(i = 0; i < pst_ExtPlay->st_InsertList.us_Size; i++)
		{
			ulFat = BIG_ul_SearchKeyToFat(pst_ExtPlay->st_InsertList.aul_KeyList[i]);
			if(ulFat == BIG_C_InvalidIndex) continue;

			o_line.Format("[%d] - %s", i, BIG_NameFile(ulFat));
			ul_Res = po_List->AddString(o_line);
		}
	}

	po_List->SetTopIndex(top);
	po_List->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnInsertListAdd(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	BIG_KEY						ul_key;
	EDIA_cl_FileDialog			o_File
								(
									"Choose File",
									0,
									0,
									1,
									NULL,
									"*"EDI_Csz_ExtSoundInsert ",*"EDI_Csz_ExtSoundFade
								);
	CString						o_Temp;
	ULONG						ul_Index;
	ULONG						*pul_Flags;
	BIG_KEY						*pNewKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_File.mb_UseFavorite = TRUE;
	pst_ExtPlayer = GetExtPlayer();
	pul_Flags = NULL;
	if(!pst_ExtPlayer) return;

	if(o_File.DoModal() == IDOK)
	{
		pst_ExtPlayer->st_InsertList.us_Size++;
		if(pst_ExtPlayer->st_InsertList.app_PtrList)
		{
			pst_ExtPlayer->st_InsertList.app_PtrList = (SND_tdst_Insert **) MEM_p_Realloc
				(
					pst_ExtPlayer->st_InsertList.app_PtrList,
					pst_ExtPlayer->st_InsertList.us_Size * sizeof(SND_tdst_Insert *)
				);
			pst_ExtPlayer->st_InsertList.aul_KeyList = (BIG_KEY *) MEM_p_Realloc
				(
					pst_ExtPlayer->st_InsertList.aul_KeyList,
					pst_ExtPlayer->st_InsertList.us_Size * sizeof(BIG_KEY)
				);
		}
		else
		{
			pst_ExtPlayer->st_InsertList.app_PtrList = (SND_tdst_Insert **) MEM_p_Alloc(pst_ExtPlayer->st_InsertList.us_Size * sizeof(SND_tdst_Insert *));
			pst_ExtPlayer->st_InsertList.aul_KeyList = (BIG_KEY *) MEM_p_Alloc(pst_ExtPlayer->st_InsertList.us_Size * sizeof(BIG_KEY));
		}

		pNewKey = &pst_ExtPlayer->st_InsertList.aul_KeyList[pst_ExtPlayer->st_InsertList.us_Size - 1];

		pst_ExtPlayer->st_InsertList.app_PtrList[pst_ExtPlayer->st_InsertList.us_Size - 1] = NULL;
		pst_ExtPlayer->st_InsertList.aul_KeyList[pst_ExtPlayer->st_InsertList.us_Size - 1] = -1;

		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(ul_Index == BIG_C_InvalidIndex)
			ul_key = BIG_C_InvalidKey;
		else
			ul_key = BIG_FileKey(ul_Index);

		*pNewKey = ul_key;

		/*$1- get the new file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		LOA_MakeFileRef
		(
			pst_ExtPlayer->st_InsertList.aul_KeyList[pst_ExtPlayer->st_InsertList.us_Size - 1],
			(ULONG *) &pst_ExtPlayer->st_InsertList.app_PtrList[pst_ExtPlayer->st_InsertList.us_Size - 1],
			SND_ul_CallbackInsertLoad,
			LOA_C_MustExists | LOA_C_HasUserCounter
		);
		LOA_Resolve();
		ESON_LoadAllSounds();

		InsertListInit();
		SignalChangeToEditor();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnInsertListDel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox					*po_List;
	int							iSel, iSrc, iDst, iSize, iDeleted;
	int							ai_Sel[10000];
	int							iNumSel;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;

	po_List = (CListBox *) GetDlgItem(IDC_LIST_INSERT);
	iNumSel = po_List->GetSelItems(10000, ai_Sel);

	iDeleted = 0;
	for(iSel = 0; iSel < iNumSel; iSel++)
	{
		iDst = ai_Sel[iSel] - iDeleted;
		SND_InsertUnload(SND_i_InsertGetIndex(pst_ExtPlay->st_InsertList.aul_KeyList[iDst]));

		iSrc = iDst + 1;
		iSize = (int) pst_ExtPlay->st_InsertList.us_Size;

		if(iSrc < iSize)
		{
			L_memcpy
			(
				&pst_ExtPlay->st_InsertList.app_PtrList[iDst],
				&pst_ExtPlay->st_InsertList.app_PtrList[iSrc],
				(iSize - iSrc) * sizeof(SND_tdst_Insert *)
			);
			L_memcpy
			(
				&pst_ExtPlay->st_InsertList.aul_KeyList[iDst],
				&pst_ExtPlay->st_InsertList.aul_KeyList[iSrc],
				(iSize - iSrc) * sizeof(BIG_KEY)
			);
		}

		pst_ExtPlay->st_InsertList.us_Size--;
		iDeleted++;
	}

	InsertListInit();
	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::InsertListSwapFile(int i1, int i2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	SND_tdst_Insert				*pInsert;
	ULONG						ulKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(i1 < 0) return;
	if(i2 < 0) return;

	pst_ExtPlay = GetExtPlayer();
	if(!pst_ExtPlay) return;
	if((int) pst_ExtPlay->st_InsertList.us_Size <= i1) return;
	if((int) pst_ExtPlay->st_InsertList.us_Size <= i2) return;

	pInsert = pst_ExtPlay->st_InsertList.app_PtrList[i1];
	ulKey = pst_ExtPlay->st_InsertList.aul_KeyList[i1];

	pst_ExtPlay->st_InsertList.app_PtrList[i1] = pst_ExtPlay->st_InsertList.app_PtrList[i2];
	pst_ExtPlay->st_InsertList.aul_KeyList[i1] = pst_ExtPlay->st_InsertList.aul_KeyList[i2];

	pst_ExtPlay->st_InsertList.app_PtrList[i2] = pInsert;
	pst_ExtPlay->st_InsertList.aul_KeyList[i2] = ulKey;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnInsertListMoveUp(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_List;
	int			ai_Sel[10000];
	int			iNumSel;
	int			iSel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST_INSERT);
	if(po_List->GetSelCount() == 0) return;
	if(po_List->GetSel(0)) return;

	iNumSel = po_List->GetSelItems(10000, ai_Sel);
	for(iSel = 1; iSel < po_List->GetCount(); iSel++)
	{
		if(po_List->GetSel(iSel))
		{
			InsertListSwapFile(iSel, iSel - 1);
			po_List->SetSel(iSel - 1, TRUE);
			po_List->SetSel(iSel, FALSE);
		}
	}

	InsertListInit();

	for(iSel = 0; iSel < iNumSel; iSel++)
		if(ai_Sel[iSel]) po_List->SetSel(ai_Sel[iSel] - 1, TRUE);

	SignalChangeToEditor();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewSmd::OnInsertListMoveDown(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_List;
	int			ai_Sel[10000];
	int			iNumSel;
	int			iSel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST_INSERT);
	if(po_List->GetSelCount() == 0) return;
	if(po_List->GetSel(po_List->GetCount() - 1)) return;

	iNumSel = po_List->GetSelItems(10000, ai_Sel);
	for(iSel = po_List->GetCount() - 2; iSel >= 0; iSel--)
	{
		if(po_List->GetSel(iSel))
		{
			InsertListSwapFile(iSel, iSel + 1);
			po_List->SetSel(iSel + 1, TRUE);
			po_List->SetSel(iSel, FALSE);
		}
	}

	InsertListInit();

	for(iSel = 0; iSel < iNumSel; iSel++)
		if(ai_Sel[iSel] != po_List->GetSelCount() - 1) po_List->SetSel(ai_Sel[iSel] + 1, TRUE);

	SignalChangeToEditor();
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */

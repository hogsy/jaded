/*$T SONview_group.cpp GC 1.138 03/04/04 15:08:05 */


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

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "Res/Res.h"
#include "EDImainframe.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDvolume.h"

/*$2- sound edi ------------------------------------------------------------------------------------------------------*/

#include "SONframe.h"
#include "SONView_group.h"
#include "Dialogs/DIAbase.h"

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

typedef struct	ESON_tdst_ViewGroupSliderCtrl_
{
	UINT	uiID;
	float	*pf_Vol;
} ESON_tdst_ViewGroupSliderCtrl;

ESON_tdst_ViewGroupSliderCtrl	ESON_gdst_ViewGroupSliderCtrl[] =
{
	{ IDC_SLIDER_MASTER, &SND_gdst_Group[SND_e_MasterGrp].f_Volume },
	{ IDC_SLIDER_VOLGRP1, &SND_gdst_Group[SND_e_GrpSpecialFX].f_Volume },
	{ IDC_SLIDER_VOLGRP2, &SND_gdst_Group[SND_e_GrpMusic].f_Volume },
	{ IDC_SLIDER_VOLGRP3, &SND_gdst_Group[SND_e_GrpAmbience].f_Volume },
	{ IDC_SLIDER_VOLGRP4, &SND_gdst_Group[SND_e_GrpDialog].f_Volume },
	{ IDC_SLIDER_VOLGRP5, &SND_gdst_Group[SND_e_GrpCutScene].f_Volume },
	{ IDC_SLIDER_VOLGRP6, &SND_gdst_Group[SND_e_GrpInterface].f_Volume },
	{ IDC_SLIDER_VOLGRP7, &SND_gdst_Group[SND_e_GrpA].f_Volume },
	{ IDC_SLIDER_VOLGRP8, &SND_gdst_Group[SND_e_GrpB].f_Volume },
	{ IDC_SLIDER_FXVOLGRP1, &SND_gdst_Group[SND_e_GrpMusic].f_FxVolume },
	{ IDC_SLIDER_FXVOLGRP2, &SND_gdst_Group[SND_e_GrpSpecialFX].f_FxVolume },
	{ IDC_SLIDER_FXVOLGRP3, &SND_gdst_Group[SND_e_GrpAmbience].f_FxVolume },
	{ IDC_SLIDER_FXVOLGRP4, &SND_gdst_Group[SND_e_GrpDialog].f_FxVolume },
	{ IDC_SLIDER_FXVOLGRP5, &SND_gdst_Group[SND_e_GrpCutScene].f_FxVolume },
	{ IDC_SLIDER_FXVOLGRP6, &SND_gdst_Group[SND_e_GrpInterface].f_FxVolume },
	{ IDC_SLIDER_FXVOLGRP7, &SND_gdst_Group[SND_e_GrpA].f_FxVolume },
	{ IDC_SLIDER_FXVOLGRP8, &SND_gdst_Group[SND_e_GrpB].f_FxVolume },
	{ (UINT)-1, NULL }
};

typedef struct					ESON_tdst_ViewGroupBtnCtrl_
{
	UINT			uiBypass;
	UINT			uiMute;
	UINT			uiSolo;
	SND_tdst_Group	*pst_Group;
} ESON_tdst_ViewGroupBtnCtrl;

ESON_tdst_ViewGroupBtnCtrl	ESON_gdst_ViewGroupBtnCtrl[] =
{
	{ IDC_CHECK_BYPASSGRP9, IDC_CHECK_MUTEGRP9, IDC_CHECK_SOLOGRP9, &SND_gdst_Group[SND_e_MasterGrp] },
	{ IDC_CHECK_BYPASSGRP1, IDC_CHECK_MUTEGRP1, IDC_CHECK_SOLOGRP1, &SND_gdst_Group[SND_e_GrpSpecialFX] },
	{ IDC_CHECK_BYPASSGRP2, IDC_CHECK_MUTEGRP2, IDC_CHECK_SOLOGRP2, &SND_gdst_Group[SND_e_GrpMusic] },
	{ IDC_CHECK_BYPASSGRP3, IDC_CHECK_MUTEGRP3, IDC_CHECK_SOLOGRP3, &SND_gdst_Group[SND_e_GrpAmbience] },
	{ IDC_CHECK_BYPASSGRP4, IDC_CHECK_MUTEGRP4, IDC_CHECK_SOLOGRP4, &SND_gdst_Group[SND_e_GrpDialog] },
	{ IDC_CHECK_BYPASSGRP5, IDC_CHECK_MUTEGRP5, IDC_CHECK_SOLOGRP5, &SND_gdst_Group[SND_e_GrpCutScene] },
	{ IDC_CHECK_BYPASSGRP6, IDC_CHECK_MUTEGRP6, IDC_CHECK_SOLOGRP6, &SND_gdst_Group[SND_e_GrpInterface] },
	{ IDC_CHECK_BYPASSGRP7, IDC_CHECK_MUTEGRP7, IDC_CHECK_SOLOGRP7, &SND_gdst_Group[SND_e_GrpA] },
	{ IDC_CHECK_BYPASSGRP8, IDC_CHECK_MUTEGRP8, IDC_CHECK_SOLOGRP8, &SND_gdst_Group[SND_e_GrpB] },
	{ (UINT)-1, (UINT)-1, (UINT)-1, NULL }
};

ESON_tdst_ViewGroupBtnCtrl	ESON_gdst_ViewUsrGroupBtnCtrl[] =
{
	{ IDC_CHECK_BYPASSGRP10, IDC_CHECK_MUTEGRP10, IDC_CHECK_SOLOGRP10, &SND_gdst_Group[SND_e_UserGrpMaster] },
	{ IDC_CHECK_BYPASSGRP11, IDC_CHECK_MUTEGRP11, IDC_CHECK_SOLOGRP11, &SND_gdst_Group[SND_e_UserGrpMusic] },
	{ IDC_CHECK_BYPASSGRP12, IDC_CHECK_MUTEGRP12, IDC_CHECK_SOLOGRP12, &SND_gdst_Group[SND_e_UserGrpSpecialFX] },
	{ IDC_CHECK_BYPASSGRP13, IDC_CHECK_MUTEGRP13, IDC_CHECK_SOLOGRP13, &SND_gdst_Group[SND_e_UserGrpDialog] },
	{ (UINT)-1, (UINT)-1, (UINT)-1, NULL }
};

typedef struct				ESON_tdst_ViewGroupProgressCtrl_
{
	UINT	uiID;
	float	*pf_Vol;
} ESON_tdst_ViewGroupProgressCtrl;

ESON_tdst_ViewGroupProgressCtrl ESON_gdst_ViewGroupProgressCtrl[] =
{
	{ IDC_PROGRESS, &SND_gdst_Group[SND_e_UserGrpMaster].f_Volume },
	{ IDC_PROGRESS2, &SND_gdst_Group[SND_e_UserGrpMusic].f_Volume },
	{ IDC_PROGRESS3, &SND_gdst_Group[SND_e_UserGrpSpecialFX].f_Volume },
	{ IDC_PROGRESS4, &SND_gdst_Group[SND_e_UserGrpDialog].f_Volume },
	{ (UINT)-1, NULL }
};


UINT ESON_gui_SoloIcon[] = 
{
IDC_CHECK_SOLOGRP1,
IDC_CHECK_SOLOGRP2,
IDC_CHECK_SOLOGRP3,
IDC_CHECK_SOLOGRP4,
IDC_CHECK_SOLOGRP5,
IDC_CHECK_SOLOGRP6,
IDC_CHECK_SOLOGRP7,
IDC_CHECK_SOLOGRP8,
IDC_CHECK_SOLOGRP9,
IDC_CHECK_SOLOGRP10,
IDC_CHECK_SOLOGRP11,
IDC_CHECK_SOLOGRP12,
IDC_CHECK_SOLOGRP13,
(UINT)-1
};

UINT ESON_gui_MuteIcon[] = 
{
IDC_CHECK_MUTEGRP1,
IDC_CHECK_MUTEGRP2,
IDC_CHECK_MUTEGRP3,
IDC_CHECK_MUTEGRP4,
IDC_CHECK_MUTEGRP5,
IDC_CHECK_MUTEGRP6,
IDC_CHECK_MUTEGRP7,
IDC_CHECK_MUTEGRP8,
IDC_CHECK_MUTEGRP9,
IDC_CHECK_MUTEGRP10,
IDC_CHECK_MUTEGRP11,
IDC_CHECK_MUTEGRP12,
IDC_CHECK_MUTEGRP13,
(UINT)-1
};

UINT ESON_gui_BypassIcon[] = 
{
IDC_CHECK_BYPASSGRP1,
IDC_CHECK_BYPASSGRP2,
IDC_CHECK_BYPASSGRP3,
IDC_CHECK_BYPASSGRP4,
IDC_CHECK_BYPASSGRP5,
IDC_CHECK_BYPASSGRP6,
IDC_CHECK_BYPASSGRP7,
IDC_CHECK_BYPASSGRP8,
IDC_CHECK_BYPASSGRP9,
IDC_CHECK_BYPASSGRP10,
IDC_CHECK_BYPASSGRP11,
IDC_CHECK_BYPASSGRP12,
IDC_CHECK_BYPASSGRP13,
(UINT)-1
};
/*$4
 ***********************************************************************************************************************
    proto
 ***********************************************************************************************************************
 */

extern void EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);

/*$4
 ***********************************************************************************************************************
    message map
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNCREATE(ESON_cl_ViewGroup, CFormView)
BEGIN_MESSAGE_MAP(ESON_cl_ViewGroup, CFormView)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP1, OnBypassGrp1)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP2, OnBypassGrp2)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP3, OnBypassGrp3)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP4, OnBypassGrp4)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP5, OnBypassGrp5)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP6, OnBypassGrp6)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP7, OnBypassGrp7)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP8, OnBypassGrp8)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP10, OnBypassUsrMaster)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP11, OnBypassGrp11)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP12, OnBypassGrp12)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP13, OnBypassGrp13)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP1, OnMuteGrp1)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP2, OnMuteGrp2)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP3, OnMuteGrp3)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP4, OnMuteGrp4)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP5, OnMuteGrp5)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP6, OnMuteGrp6)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP7, OnMuteGrp7)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP8, OnMuteGrp8)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP10, OnMuteUsrMaster)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP11, OnMuteGrp11)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP12, OnMuteGrp12)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP13, OnMuteGrp13)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP1, OnSoloGrp1)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP2, OnSoloGrp2)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP3, OnSoloGrp3)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP4, OnSoloGrp4)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP5, OnSoloGrp5)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP6, OnSoloGrp6)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP7, OnSoloGrp7)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP8, OnSoloGrp8)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP10, OnSoloUsrMaster)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP11, OnSoloGrp11)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP12, OnSoloGrp12)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP13, OnSoloGrp13)
	ON_BN_CLICKED(IDC_CHECK_BYPASSGRP9, OnMasterBypass)
	ON_BN_CLICKED(IDC_CHECK_MUTEGRP9, OnMasterMute)
	ON_BN_CLICKED(IDC_CHECK_SOLOGRP9, OnMasterSolo)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
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
ESON_cl_ViewGroup::ESON_cl_ViewGroup(ESON_cl_Frame *_po_Editor) :
	CFormView(ESON_IDD_EDITGROUP)
{
	mpo_Editor = _po_Editor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_ViewGroup::ESON_cl_ViewGroup(void) :
	CFormView(ESON_IDD_EDITGROUP)
{
	ESON_cl_ViewGroup(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_ViewGroup::~ESON_cl_ViewGroup(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_ViewGroup::Create
(
	LPCTSTR		lpszClassName,
	LPCTSTR		lpszWindowName,
	DWORD		dwStyle,
	const RECT	&rect,
	CWnd		*pParentWnd,
	UINT		nID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl		*po_Slider;
	CProgressCtrl	*po_Progress;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, NULL);

	for(i = 0; ESON_gdst_ViewGroupSliderCtrl[i].uiID != -1; i++)
	{
		po_Slider = (CSliderCtrl *) GetDlgItem(ESON_gdst_ViewGroupSliderCtrl[i].uiID);
		po_Slider->SetRange(0, 1000, TRUE);
		po_Slider->SetPos(0);
	}

	for(i = 0; ESON_gdst_ViewGroupProgressCtrl[i].uiID != -1; i++)
	{
		po_Progress = (CProgressCtrl *) GetDlgItem(ESON_gdst_ViewGroupProgressCtrl[i].uiID);
		po_Progress->SetRange(0, 100);
		po_Progress->SetStep(10);
		po_Progress->SetPos(0);
	}

    m_hIconSolo = AfxGetApp()->LoadIcon(IDI_ICON_SOLO);
    m_hIconMute = AfxGetApp()->LoadIcon(IDI_ICON_MUTE);
    m_hIconBypass = AfxGetApp()->LoadIcon(IDI_ICON_BYPASS);
    m_hIconSoloG = AfxGetApp()->LoadIcon(IDI_ICON_SOLO_G);
    m_hIconMuteG = AfxGetApp()->LoadIcon(IDI_ICON_MUTE_G);
    m_hIconBypassG = AfxGetApp()->LoadIcon(IDI_ICON_BYPASS_G);

	for(i = 0; ESON_gui_SoloIcon[i] != -1; i++)
	{
        ((CButton *) GetDlgItem(ESON_gui_SoloIcon[i]))->SetIcon(m_hIconSoloG);
    }
	
    for(i = 0; ESON_gui_MuteIcon[i] != -1; i++)
	{
        ((CButton *) GetDlgItem(ESON_gui_MuteIcon[i]))->SetIcon(m_hIconMuteG);
    }
	
    for(i = 0; ESON_gui_BypassIcon[i] != -1; i++)
	{
        ((CButton *) GetDlgItem(ESON_gui_BypassIcon[i]))->SetIcon(m_hIconBypassG);
    }

	EDIA_cl_BaseDialog::SetTheme(this);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::UpdateControls(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl		*po_Slider;
	CProgressCtrl	*po_Progress;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    //this->SetRedraw(FALSE);
	for(i = 0; ESON_gdst_ViewGroupSliderCtrl[i].uiID != -1; i++)
	{
		po_Slider = (CSliderCtrl *) GetDlgItem(ESON_gdst_ViewGroupSliderCtrl[i].uiID);
		po_Slider->SetPos(1000 - (int) (*ESON_gdst_ViewGroupSliderCtrl[i].pf_Vol * 1000.0f));
	}

	for(i = 0; ESON_gdst_ViewGroupProgressCtrl[i].uiID != -1; i++)
	{
		po_Progress = (CProgressCtrl *) GetDlgItem(ESON_gdst_ViewGroupProgressCtrl[i].uiID);
		po_Progress->SetPos((int) (100.0f * (*ESON_gdst_ViewGroupProgressCtrl[i].pf_Vol)));
	}
    this->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl *po_Slider;
	float		f_NewVol;
	char		tip[64];
	/*~~~~~~~~~~~~~~~~~~~*/

	CFormView::OnVScroll(nSBCode, nPos, pScrollBar);

	if(!pScrollBar) return;
	if(!SND_gst_Params.l_Available) return;

	if(pScrollBar->m_hWnd == GetDlgItem(IDC_SLIDER_MASTER)->GetSafeHwnd())
	{
		po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MASTER);
		f_NewVol = 1.0f - (float) po_Slider->GetPos() / 1000.0f;

		sprintf(tip, "%.1f dB (%.2f)", (float) SND_l_GetAttFromVol(f_NewVol) / 100.0f, f_NewVol);
		EDI_Tooltip_DisplayMessage(tip, 0);

		SND_gdst_Group[SND_e_MasterGrp].f_Volume = f_NewVol;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	CSliderCtrl *po_Slider;
	float		f_NewVol;
	char		tip[64];
	int			i;
	/*~~~~~~~~~~~~~~~~~~~*/

	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);

	if(!pScrollBar) return;
	if(!SND_gst_Params.l_Available) return;

	for(i = 1; ESON_gdst_ViewGroupSliderCtrl[i].uiID != -1; i++)
	{
		if(pScrollBar->m_hWnd == GetDlgItem(ESON_gdst_ViewGroupSliderCtrl[i].uiID)->GetSafeHwnd())
		{
			po_Slider = (CSliderCtrl *) GetDlgItem(ESON_gdst_ViewGroupSliderCtrl[i].uiID);
			f_NewVol = 1.0f - (float) po_Slider->GetPos() / 1000.0f;

			sprintf(tip, "%.1f dB (%.2f)", (float) SND_l_GetAttFromVol(f_NewVol) / 100.0f, f_NewVol);
			EDI_Tooltip_DisplayMessage(tip, 0);

			*ESON_gdst_ViewGroupSliderCtrl[i].pf_Vol = f_NewVol;

			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnPaint(void)
{
	CFormView::OnPaint();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp(int grp)
{
	/*~~*/
	int i;
	/*~~*/

	/* update state */
	ESON_gdst_ViewGroupBtnCtrl[grp].pst_Group->b_ByPass = ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[grp].uiBypass))->GetCheck();
	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass = FALSE;
    ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[grp].uiBypass))->SetIcon(ESON_gdst_ViewGroupBtnCtrl[grp].pst_Group->b_ByPass ? m_hIconBypass : m_hIconBypassG);

	/* update master state */
	for(i = 1; ESON_gdst_ViewGroupBtnCtrl[i].uiBypass != -1; i++)
	{
		if(ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_ByPass)
		{
			ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass = TRUE;
			break;
		}
	}
	((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiBypass))->SetCheck(ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass ? 1 : 0);
    ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiBypass))->SetIcon(ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass ? m_hIconBypass : m_hIconBypassG);

	/* update usr group */
	if
	(
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);

			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::UpdateAllInstances(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;
		SND_SetInstVolume(pst_SI);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassUsrGrp(int grp)
{
	/*~~*/
	int i;
	/*~~*/

	/* update state */
	ESON_gdst_ViewUsrGroupBtnCtrl[grp].pst_Group->b_ByPass = ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[grp].uiBypass))->GetCheck();
	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass = FALSE;

    ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[grp].uiBypass))->SetIcon(ESON_gdst_ViewUsrGroupBtnCtrl[grp].pst_Group->b_ByPass ? m_hIconBypass : m_hIconBypassG);

	/* update master state */
	for(i = 1; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass != -1; i++)
	{
		if(ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_ByPass)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass = TRUE;
			break;
		}
	}
	((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiBypass))->SetCheck(ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass ? 1 : 0);
    ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiBypass))->SetIcon(ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass ? m_hIconBypass : m_hIconBypassG);

	/* update groups */
	if
	(
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);

			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp(int grp)
{
	/*~~*/
	int i;
	/*~~*/

	ESON_gdst_ViewGroupBtnCtrl[grp].pst_Group->b_Solo = ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[grp].uiSolo))->GetCheck();
	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo = FALSE;

    ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[grp].uiSolo))->SetIcon(ESON_gdst_ViewGroupBtnCtrl[grp].pst_Group->b_Solo ? m_hIconSolo: m_hIconSoloG);


	for(i = 1; ESON_gdst_ViewGroupBtnCtrl[i].uiSolo != -1; i++)
	{
		if(ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Solo)
		{
			ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo = TRUE;
			break;
		}
	}
	((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiSolo))->SetCheck(ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo ? 1 : 0);
	((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiSolo))->SetIcon(ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo ? m_hIconSolo : m_hIconSoloG);

	/* update usr group */
	if
	(
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);

			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloUsrGrp(int grp)
{
	/*~~*/
	int i;
	/*~~*/

	ESON_gdst_ViewUsrGroupBtnCtrl[grp].pst_Group->b_Solo = ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[grp].uiSolo))->GetCheck();
	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo = FALSE;

    ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[grp].uiSolo))->SetIcon(ESON_gdst_ViewUsrGroupBtnCtrl[grp].pst_Group->b_Solo ? m_hIconSolo: m_hIconSoloG);

	for(i = 1; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo != -1; i++)
	{
		if(ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Solo)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo = TRUE;
			break;
		}
	}
	((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiSolo))->SetCheck(ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo ? 1 : 0);
	((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiSolo))->SetIcon(ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo ? m_hIconSolo: m_hIconSoloG);

	/* update group */
	if
	(
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);

			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp(int grp)
{
	/*~~*/
	int i;
	/*~~*/

	ESON_gdst_ViewGroupBtnCtrl[grp].pst_Group->b_Mute = ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[grp].uiMute))->GetCheck();
	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute = FALSE;

    ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[grp].uiMute))->SetIcon(ESON_gdst_ViewGroupBtnCtrl[grp].pst_Group->b_Mute ? m_hIconMute: m_hIconMuteG);

	for(i = 1; ESON_gdst_ViewGroupBtnCtrl[i].uiMute != -1; i++)
	{
		if(ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Mute)
		{
			ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute = TRUE;
			break;
		}
	}
	((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiMute))->SetCheck(ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute ? 1 : 0);
	((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiMute))->SetIcon(ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute ? m_hIconMute : m_hIconMuteG);

	/* update usr group */
	if
	(
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);

			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteUsrGrp(int grp)
{
	/*~~*/
	int i;
	/*~~*/

	ESON_gdst_ViewUsrGroupBtnCtrl[grp].pst_Group->b_Mute = ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[grp].uiMute))->GetCheck();
	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute = FALSE;

    ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[grp].uiMute))->SetIcon(ESON_gdst_ViewUsrGroupBtnCtrl[grp].pst_Group->b_Mute ? m_hIconMute: m_hIconMuteG);

	for(i = 1; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute != -1; i++)
	{
		if(ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Mute)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute = TRUE;
			break;
		}
	}
	((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiMute))->SetCheck(ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute ? 1 : 0);
	((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiMute))->SetIcon(ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute ? m_hIconMute : m_hIconMuteG);

	/* update group */
	if
	(
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);

			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp1(void)
{
	OnBypassGrp(1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp2(void)
{
	OnBypassGrp(2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp3(void)
{
	OnBypassGrp(3);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp4(void)
{
	OnBypassGrp(4);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp5(void)
{
	OnBypassGrp(5);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp6(void)
{
	OnBypassGrp(6);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp7(void)
{
	OnBypassGrp(7);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp8(void)
{
	OnBypassGrp(8);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassUsrMaster(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiBypass))->GetCheck())
	{
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass = TRUE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiBypass))->SetIcon(m_hIconBypass);

		for(i = 1; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_ByPass = TRUE;
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetCheck(1);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypass);
		}
	}
	else
	{
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass = FALSE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiBypass))->SetIcon(m_hIconBypassG);

		for(i = 1; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetCheck(0);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
		}
	}

	if
	(
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp11(void)
{
	OnBypassUsrGrp(1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp12(void)
{
	OnBypassUsrGrp(2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnBypassGrp13(void)
{
	OnBypassUsrGrp(3);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp1(void)
{
	OnSoloGrp(1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp2(void)
{
	OnSoloGrp(2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp3(void)
{
	OnSoloGrp(3);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp4(void)
{
	OnSoloGrp(4);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp5(void)
{
	OnSoloGrp(5);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp6(void)
{
	OnSoloGrp(6);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp7(void)
{
	OnSoloGrp(7);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp8(void)
{
	OnSoloGrp(8);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloUsrMaster(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiSolo))->GetCheck())
	{
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo = TRUE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiSolo))->SetIcon(m_hIconSolo);

		for(i = 1; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Solo = TRUE;
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetCheck(1);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSolo);
		}
	}
	else
	{
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo = FALSE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiSolo))->SetIcon(m_hIconSoloG);

		for(i = 1; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetCheck(0);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
		}
	}

	if
	(
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp11(void)
{
	OnSoloUsrGrp(1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp12(void)
{
	OnSoloUsrGrp(2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnSoloGrp13(void)
{
	OnSoloUsrGrp(3);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp1(void)
{
	OnMuteGrp(1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp2(void)
{
	OnMuteGrp(2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp3(void)
{
	OnMuteGrp(3);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp4(void)
{
	OnMuteGrp(4);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp5(void)
{
	OnMuteGrp(5);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp6(void)
{
	OnMuteGrp(6);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp7(void)
{
	OnMuteGrp(7);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp8(void)
{
	OnMuteGrp(8);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteUsrMaster(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiMute))->GetCheck())
	{
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute = TRUE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiMute))->SetIcon(m_hIconMute);

		for(i = 1; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute != -1; i++)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Mute = TRUE;
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetCheck(1);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMute);
		}
	}
	else
	{
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute = FALSE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[0].uiMute))->SetIcon(m_hIconMuteG);

		for(i = 1; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute != -1; i++)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetCheck(0);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
		}
	}

	if
	(
		ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewUsrGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiMute != -1; i++)
		{
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewGroupBtnCtrl[i].uiMute != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp11(void)
{
	OnMuteUsrGrp(1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp12(void)
{
	OnMuteUsrGrp(2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMuteGrp13(void)
{
	OnMuteUsrGrp(3);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMasterBypass(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiBypass))->GetCheck())
	{
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass = TRUE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiBypass))->SetIcon(m_hIconBypass);

		for(i = 1; ESON_gdst_ViewGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_ByPass = TRUE;
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetCheck(1);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypass);
		}
	}
	else
	{
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass = FALSE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiBypass))->SetIcon(m_hIconBypassG);

		for(i = 1; ESON_gdst_ViewGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetCheck(0);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
		}
	}

	if
	(
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMasterMute(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiMute))->GetCheck())
	{
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute = TRUE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiMute))->SetIcon(m_hIconMute);

		for(i = 1; ESON_gdst_ViewGroupBtnCtrl[i].uiMute != -1; i++)
		{
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Mute = TRUE;
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetCheck(1);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMute);
		}
	}
	else
	{
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute = FALSE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiMute))->SetIcon(m_hIconMuteG);

		for(i = 1; ESON_gdst_ViewGroupBtnCtrl[i].uiMute != -1; i++)
		{
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetCheck(0);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
		}
	}

	if
	(
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute != -1; i++)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_ViewGroup::OnMasterSolo(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiSolo))->GetCheck())
	{
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo = TRUE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiSolo))->SetIcon(m_hIconSolo);

		for(i = 1; ESON_gdst_ViewGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Solo = TRUE;
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetCheck(1);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSolo);
		}
	}
	else
	{
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo = FALSE;
        ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[0].uiSolo))->SetIcon(m_hIconSoloG);

		for(i = 1; ESON_gdst_ViewGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			ESON_gdst_ViewGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetCheck(0);
            ((CButton *) GetDlgItem(ESON_gdst_ViewGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
		}
	}

	if
	(
		ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Mute
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_ByPass
	||	ESON_gdst_ViewGroupBtnCtrl[0].pst_Group->b_Solo
	)
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Solo = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_ByPass = FALSE;
			ESON_gdst_ViewUsrGroupBtnCtrl[i].pst_Group->b_Mute = FALSE;
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetCheck(0);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->SetIcon(m_hIconBypassG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->SetIcon(m_hIconMuteG);
            ((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->SetIcon(m_hIconSoloG);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_HIDE);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		for(i = 0; ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo != -1; i++)
		{
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiSolo))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiBypass))->ShowWindow(SW_SHOW);
			((CButton *) GetDlgItem(ESON_gdst_ViewUsrGroupBtnCtrl[i].uiMute))->ShowWindow(SW_SHOW);
		}
	}

	UpdateAllInstances();
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */

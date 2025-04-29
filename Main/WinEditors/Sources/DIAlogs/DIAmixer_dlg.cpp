/*$T DIAmixer_dlg.cpp GC! 1.081 11/04/02 13:39:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAmixer_dlg.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "LINks/LINKmsg.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define gszLineFormatList			"%-s%c\t%-s\t%.08lXh\t%.08lXh\t%lu\t%lu\t%-s"
#define gszLineControlsFormatList	"%u\t%-s\t%.08lXh\t%.08lXh\t%lu\t%-s"

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	tMACONTROLINSTANCE
{
	HMIXER			hmx;
	LPMIXERLINE		pmxl;
	LPMIXERCONTROL	pmxctrl;
}
MACONTROLINSTANCE, FAR *LPMACONTROLINSTANCE;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_MixerDialog, EDIA_cl_BaseDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_DEST, OnSelMixerLine)
	ON_CBN_SELCHANGE(IDC_COMBO_SRC, OnSelMixerConnection)
	ON_CBN_SELCHANGE(IDC_COMBO_CTRL, OnSelMixerControl)
    ON_CBN_SELCHANGE(IDC_MIXSLIDER1, OnPos1Change)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MixerDialog::EDIA_cl_MixerDialog(void) :
	EDIA_cl_BaseDialog(ESON_IDD_MIXER)
{
        
	mi_CurrMixerLineIndex = -1;
	mi_CurrMixerConnection = -1;
	mi_CurrMixerControl = -1;

	L_memset(&mst_MixerLine, 0, sizeof(MIXERLINE));
	L_memset(&mst_MixerConnection, 0, sizeof(MIXERLINE));
	L_memset(&mst_MixerCaps, 0, sizeof(MIXERCAPS));
	L_memset(&mst_MixerControl, 0, sizeof(MIXERCONTROL));

	mh_Mixer = MixAppNewDevice(NULL, 0);
	if(mh_Mixer) MixAppRefreshLineList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MixerDialog::~EDIA_cl_MixerDialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MixerDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CComboBox	*po_Combo;
	MMRESULT	mmr;
	UINT		u;
	MIXERLINE	mxl;
	/*~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnInitDialog();

	MixHideAllButtons();

	if(!mh_Mixer) return TRUE;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    draw mixer line
	 -------------------------------------------------------------------------------------------------------------------
	 */

	po_Combo = (CComboBox *) GetDlgItem(IDC_COMBO_DEST);
	po_Combo->ResetContent();

	for(u = 0; u < mst_MixerCaps.cDestinations; u++)
	{
		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwDestination = u;

		mmr = mixerGetLineInfo((HMIXEROBJ) mh_Mixer, &mxl, MIXER_GETLINEINFOF_DESTINATION);
		if(MMSYSERR_NOERROR != mmr)
		{
			ERR_X_Warning(0, "mixerGetLineInfo failed", NULL);
			continue;
		}

		po_Combo->InsertString(u, mxl.szName);
		po_Combo->SetItemData(u, mxl.dwLineID);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    select 1st item
	 -------------------------------------------------------------------------------------------------------------------
	 */

	po_Combo->SetTopIndex(0);
	po_Combo->SetCurSel(0);
	po_Combo->EnableWindow();
	OnSelMixerLine();

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HMIXER EDIA_cl_MixerDialog::MixAppNewDevice(HMIXER hmxCur, UINT uMxId)
{
	/*~~~~~~~~~~~~*/
	MMRESULT	mmr;
	HMIXER		hmx;
	int			i;
	/*~~~~~~~~~~~~*/

	L_memset(&mst_MixerCaps, 0, sizeof(MIXERCAPS));
	mmr = mixerGetDevCaps(uMxId, &mst_MixerCaps, sizeof(MIXERCAPS));
	if(MMSYSERR_NOERROR != mmr)
	{
		::MessageBox(NULL, "error", "error", MB_OK);
		return(hmxCur);
	}

	mmr = mixerOpen(&hmx, uMxId, (DWORD) (UINT) this->m_hWnd, 0L, CALLBACK_WINDOW);
	if(MMSYSERR_NOERROR != mmr)
	{
		switch(mmr)
		{
		case MMSYSERR_NOERROR:
			return NULL;
		case MMSYSERR_ALLOCATED:
			i = 1;
			break;
		case MMSYSERR_BADDEVICEID:
			i = 2;
			break;
		case MMSYSERR_INVALFLAG:
			i = 3;
			break;
		case MMSYSERR_INVALHANDLE:
			i = 4;
			break;
		case MMSYSERR_INVALPARAM:
			i = 5;
			break;
		case MMSYSERR_NODRIVER:
			i = 6;
			break;
		case MMSYSERR_NOMEM:
			i = 7;
			break;
		default:
			i = -1;
			break;
		}

		::MessageBox(NULL, "error", "error", MB_OK);
	}

	if(NULL != hmxCur)
	{
		mmr = mixerClose(hmxCur);
		if(MMSYSERR_NOERROR != mmr)
		{
			::MessageBox(NULL, "error", "error", MB_OK);
		}

		hmxCur = NULL;
	}

	return(hmx);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MixerDialog::MixAppRefreshLineList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static char			szLineTypeDst[] = TEXT("DST");
	static char			szLineTypeSrc[] = TEXT("  src");
	MMRESULT			mmr;
	UINT				u;
	UINT				v;
	UINT				cConnections;
	MIXERLINE			mxl;
	char				ach[1024];
	char				szComponent[128];
	char				szControlType[128];
	unsigned int		i;
	MIXERLINECONTROLS	st_MixLineControls;
	MIXERCONTROL		*dst_MixerControl;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mh_Mixer) return FALSE;

	dst_MixerControl = NULL;
	for(u = 0; u < mst_MixerCaps.cDestinations; u++)
	{
		if(dst_MixerControl) L_free(dst_MixerControl);
		dst_MixerControl = NULL;

		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwDestination = u;

		mmr = mixerGetLineInfo((HMIXEROBJ) mh_Mixer, &mxl, MIXER_GETLINEINFOF_DESTINATION);
		if(MMSYSERR_NOERROR != mmr)
		{
			::MessageBox(NULL, "mixerGetLineInfo failed", "Error", MB_OK | MB_ICONEXCLAMATION);
			continue;
		}

		MixAppGetComponentName(&mxl, szComponent);
		sprintf
		(
			ach,
			gszLineFormatList,
			(LPSTR) szLineTypeDst,
			(MIXERLINE_LINEF_ACTIVE & mxl.fdwLine) ? '*' : ' ',
			(LPTSTR) szComponent,
			mxl.dwLineID,
			mxl.fdwLine,
			mxl.cControls,
			mxl.cConnections,
			(LPSTR) mxl.szName
		);
		LINK_PrintStatusMsg(ach);

		dst_MixerControl = (MIXERCONTROL *) L_malloc(mxl.cControls * sizeof(MIXERCONTROL));
		L_memset(dst_MixerControl, 0, mxl.cControls * sizeof(MIXERCONTROL));

		st_MixLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
		st_MixLineControls.dwLineID = mxl.dwLineID;
		st_MixLineControls.cControls = mxl.cControls;
		st_MixLineControls.cbmxctrl = sizeof(MIXERCONTROL);
		st_MixLineControls.pamxctrl = dst_MixerControl;

		mmr = mixerGetLineControls((HMIXEROBJ) mh_Mixer, &st_MixLineControls, MIXER_GETLINECONTROLSF_ALL);
		if(MMSYSERR_NOERROR != mmr)
		{
			::MessageBox(NULL, "mixerGetLineInfo failed", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
		else
		{
			for(i = 0; i < mxl.cControls; i++)
			{
				MixAppGetControlTypeName(&dst_MixerControl[u], szControlType);
				sprintf
				(
					ach,
					gszLineControlsFormatList,
					i,
					szControlType,
					dst_MixerControl[i].dwControlID,
					dst_MixerControl[i].fdwControl,
					dst_MixerControl[i].cMultipleItems,
					(LPSTR) dst_MixerControl[i].szName
				);
				LINK_PrintStatusMsg(ach);
			}
		}

		cConnections = (UINT) mxl.cConnections;
		for(v = 0; v < cConnections; v++)
		{
			mxl.cbStruct = sizeof(mxl);
			mxl.dwDestination = u;
			mxl.dwSource = v;

			mmr = mixerGetLineInfo((HMIXEROBJ) mh_Mixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
			if(MMSYSERR_NOERROR != mmr)
			{
				::MessageBox(NULL, "mixerGetLineInfo failed", "Error", MB_OK | MB_ICONEXCLAMATION);
				continue;
			}

			MixAppGetComponentName(&mxl, szComponent);

			sprintf
			(
				ach,
				gszLineFormatList,
				(LPSTR) szLineTypeSrc,
				(MIXERLINE_LINEF_ACTIVE & mxl.fdwLine) ? '*' : ' ',
				(LPTSTR) szComponent,
				mxl.dwLineID,
				mxl.fdwLine,
				mxl.cControls,
				mxl.cConnections,
				(LPSTR) mxl.szName
			);
			LINK_PrintStatusMsg(ach);
		}
	}

	if(dst_MixerControl) free(dst_MixerControl);
	dst_MixerControl = NULL;

	return(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MixerDialog::MixAppGetComponentName(LPMIXERLINE pmxl, char *szComponent)
{
	if(0 == (MIXERLINE_LINEF_SOURCE & pmxl->fdwLine))
	{
		switch(pmxl->dwComponentType)
		{
		case MIXERLINE_COMPONENTTYPE_DST_UNDEFINED:
			L_strcpy(szComponent, "Undefined");
			break;

		case MIXERLINE_COMPONENTTYPE_DST_DIGITAL:
			L_strcpy(szComponent, "Digital");
			break;

		case MIXERLINE_COMPONENTTYPE_DST_LINE:
			L_strcpy(szComponent, "Line Level");
			break;

		case MIXERLINE_COMPONENTTYPE_DST_MONITOR:
			L_strcpy(szComponent, "Monitor");
			break;

		case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:
			L_strcpy(szComponent, "Speakers");
			break;

		case MIXERLINE_COMPONENTTYPE_DST_HEADPHONES:
			L_strcpy(szComponent, "Headphones");
			break;

		case MIXERLINE_COMPONENTTYPE_DST_TELEPHONE:
			L_strcpy(szComponent, "Telephone");
			break;

		case MIXERLINE_COMPONENTTYPE_DST_WAVEIN:
			L_strcpy(szComponent, "Wave Input");
			break;

		case MIXERLINE_COMPONENTTYPE_DST_VOICEIN:
			L_strcpy(szComponent, "Voice Recognition");
			break;

		default:
			L_strcpy(szComponent, "NOT VALID");
			break;
		}
	}
	else
	{
		switch(pmxl->dwComponentType)
		{
		case MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED:
			L_strcpy(szComponent, "Undefined");
			break;

		case MIXERLINE_COMPONENTTYPE_SRC_DIGITAL:
			L_strcpy(szComponent, "Digital");
			break;

		case MIXERLINE_COMPONENTTYPE_SRC_LINE:
			L_strcpy(szComponent, "Line Level");
			break;

		case MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE:
			L_strcpy(szComponent, "Microphone");
			break;

		case MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER:
			L_strcpy(szComponent, "Synthesizer");
			break;

		case MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC:
			L_strcpy(szComponent, "Compact Disc");
			break;

		case MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE:
			L_strcpy(szComponent, "Telephone");
			break;

		case MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER:
			L_strcpy(szComponent, "PC Speaker");
			break;

		case MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT:
			L_strcpy(szComponent, "Wave Out");
			break;

		case MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY:
			L_strcpy(szComponent, "Auxiliary");
			break;

		case MIXERLINE_COMPONENTTYPE_SRC_ANALOG:
			L_strcpy(szComponent, "Analog");
			break;

		default:
			L_strcpy(szComponent, "NOT VALID");
			break;
		}
	}

	return(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MixerDialog::MixAppGetControlTypeName(LPMIXERCONTROL pmxctrl, char *szControlType)
{
	switch(pmxctrl->dwControlType)
	{
	case MIXERCONTROL_CONTROLTYPE_CUSTOM:
		L_strcpy(szControlType, "Custom!");
		break;

	case MIXERCONTROL_CONTROLTYPE_BOOLEANMETER:
		L_strcpy(szControlType, "Boolean Meter");
		break;

	case MIXERCONTROL_CONTROLTYPE_SIGNEDMETER:
		L_strcpy(szControlType, "Signed Meter");
		break;

	case MIXERCONTROL_CONTROLTYPE_PEAKMETER:
		L_strcpy(szControlType, "Peak Meter");
		break;

	case MIXERCONTROL_CONTROLTYPE_UNSIGNEDMETER:
		L_strcpy(szControlType, "Unsigned Meter");
		break;

	case MIXERCONTROL_CONTROLTYPE_BOOLEAN:
		L_strcpy(szControlType, "Boolean Switch");
		break;

	case MIXERCONTROL_CONTROLTYPE_ONOFF:
		L_strcpy(szControlType, "On/Off Switch");
		break;

	case MIXERCONTROL_CONTROLTYPE_MUTE:
		L_strcpy(szControlType, "Mute Switch");
		break;

	case MIXERCONTROL_CONTROLTYPE_MONO:
		L_strcpy(szControlType, "Mono Switch");
		break;

	case MIXERCONTROL_CONTROLTYPE_LOUDNESS:
		L_strcpy(szControlType, "Loudness Switch");
		break;

	case MIXERCONTROL_CONTROLTYPE_STEREOENH:
		L_strcpy(szControlType, "Stereo Enh Switch");
		break;

	case MIXERCONTROL_CONTROLTYPE_BUTTON:
		L_strcpy(szControlType, "Button");
		break;

	case MIXERCONTROL_CONTROLTYPE_DECIBELS:
		L_strcpy(szControlType, "Decibels Number");
		break;

	case MIXERCONTROL_CONTROLTYPE_SIGNED:
		L_strcpy(szControlType, "Signed Number");
		break;

	case MIXERCONTROL_CONTROLTYPE_PERCENT:
		L_strcpy(szControlType, "Percent Number");
		break;

	case MIXERCONTROL_CONTROLTYPE_UNSIGNED:
		L_strcpy(szControlType, "Unsigned Number");
		break;

	case MIXERCONTROL_CONTROLTYPE_SLIDER:
		lstrcpy(szControlType, "Slider");
		break;

	case MIXERCONTROL_CONTROLTYPE_PAN:
		L_strcpy(szControlType, "Pan Slider");
		break;

	case MIXERCONTROL_CONTROLTYPE_QSOUNDPAN:
		L_strcpy(szControlType, "Q-Sound Pan Slider");
		break;

	case MIXERCONTROL_CONTROLTYPE_FADER:
		L_strcpy(szControlType, "Fader");
		break;

	case MIXERCONTROL_CONTROLTYPE_VOLUME:
		L_strcpy(szControlType, "Volume Fader");
		break;

	case MIXERCONTROL_CONTROLTYPE_BASS:
		L_strcpy(szControlType, "Bass Fader");
		break;

	case MIXERCONTROL_CONTROLTYPE_TREBLE:
		L_strcpy(szControlType, "Treble Fader");
		break;

	case MIXERCONTROL_CONTROLTYPE_EQUALIZER:
		L_strcpy(szControlType, "Equalizer Fader");
		break;

	case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
		L_strcpy(szControlType, "Single-Sel List");
		break;

	case MIXERCONTROL_CONTROLTYPE_MUX:
		L_strcpy(szControlType, "MUX List");
		break;

	case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
		L_strcpy(szControlType, "Multi-Sel List");
		break;

	case MIXERCONTROL_CONTROLTYPE_MIXER:
		L_strcpy(szControlType, "Mixer List");
		break;

	case MIXERCONTROL_CONTROLTYPE_MICROTIME:
		L_strcpy(szControlType, "Microsecond Time");
		break;

	case MIXERCONTROL_CONTROLTYPE_MILLITIME:
		L_strcpy(szControlType, "Millisecond Time");
		break;

	default:
		L_strcpy(szControlType, "** NOT VALID **");
		return(FALSE);
	}

	return(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MixerDialog::PreTranslateMessage(MSG *p_Msg)
{
	if(p_Msg->hwnd == this->m_hWnd)
	{
		switch(p_Msg->message)
		{
		case MM_MIXM_LINE_CHANGE:
			MixAppLineChange((HMIXER) p_Msg->wParam, p_Msg->lParam);
			break;

		case MM_MIXM_CONTROL_CHANGE:
			MixAppControlChange((HMIXER) p_Msg->wParam, p_Msg->lParam);
			break;
		default: break;
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LRESULT EDIA_cl_MixerDialog::MixAppLineChange(HMIXER hmx, DWORD dwLineID)
{
	MixAppRefreshLineList();
	return(0L);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LRESULT EDIA_cl_MixerDialog::MixAppControlChange(HMIXER hmx, DWORD dwControlID)
{
	return(0L);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::OnSelMixerLine(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox	*pcombo;
	int			iNew, i, iID;
	MMRESULT	mmr;

	MIXERLINE	mxl;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_DEST);
	iNew = pcombo->GetCurSel();
	iID = pcombo->GetItemData(iNew);

	if(iNew == -1) return;
	if(iNew == mi_CurrMixerLineIndex) return;
	if(iNew >= (int) mst_MixerCaps.cDestinations) return;
	mi_CurrMixerLineIndex = iNew;

	mst_MixerLine.cbStruct = sizeof(MIXERLINE);
	mst_MixerLine.dwDestination = mi_CurrMixerLineIndex;

	mmr = mixerGetLineInfo((HMIXEROBJ) mh_Mixer, &mst_MixerLine, MIXER_GETLINEINFOF_DESTINATION);
	if(MMSYSERR_NOERROR != mmr)
	{
		::MessageBox(NULL, "mixerGetLineInfo failed", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_SRC);
	pcombo->ResetContent();

	pcombo->InsertString(0, "*Line control*");
	pcombo->SetItemData(0, iID);
	L_memcpy(&mxl, &mst_MixerLine, sizeof(MIXERLINE));
	for(i = 0; i < (int) mst_MixerLine.cConnections; i++)
	{
		mxl.cbStruct = sizeof(mxl);
		mxl.dwDestination = mi_CurrMixerLineIndex;
		mxl.dwSource = i;

		mmr = mixerGetLineInfo((HMIXEROBJ) mh_Mixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if(MMSYSERR_NOERROR != mmr)
		{
			::MessageBox(NULL, "mixerGetLineInfo failed", "Error", MB_OK | MB_ICONEXCLAMATION);
			continue;
		}

		/* MixAppGetComponentName(&mxl, szComponent); */
		pcombo->InsertString(i + 1, mxl.szName);
		pcombo->SetItemData(i + 1, mxl.dwLineID);
	}

	mi_CurrMixerConnection = -1;
	pcombo->SetTopIndex(0);
	pcombo->SetCurSel(0);

	OnSelMixerConnection();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::OnSelMixerConnection(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox			*pcombo;
	int					iNew, iID, i;
	MMRESULT			mmr;
	MIXERCONTROL		*dst_MixerControl;
	MIXERLINECONTROLS	st_MixLineControls;

	MIXERLINE			*pmxl;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_SRC);
	iNew = pcombo->GetCurSel();
	iID = pcombo->GetItemData(iNew);

	if(iNew == -1) return;
	if(iNew == mi_CurrMixerConnection) return;
	if(iNew >= (int) (mst_MixerLine.cConnections + 1)) return;
	mi_CurrMixerConnection = iNew;

	/*
	 * if(mi_CurrMixerConnection<0) { ::MessageBox(NULL, "case we choose the mixer
	 * line control", "TODO", MB_OK | MB_ICONEXCLAMATION); return; }
	 */
	pmxl = &mst_MixerConnection;
	L_memcpy(pmxl, &mst_MixerLine, sizeof(MIXERLINE));
	pmxl->cbStruct = sizeof(MIXERLINE);
	pmxl->dwDestination = mi_CurrMixerLineIndex;

	pmxl->dwLineID = iID;
	mmr = mixerGetLineInfo((HMIXEROBJ) mh_Mixer, pmxl, MIXER_GETLINEINFOF_LINEID);
	if(MMSYSERR_NOERROR != mmr)
	{
		::MessageBox(NULL, "111mixerGetLineInfo failed", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_CTRL);
	pcombo->ResetContent();

	if(!pmxl->cControls)
	{
		::MessageBox(NULL, "no controls found", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	dst_MixerControl = (MIXERCONTROL *) L_malloc(pmxl->cControls * sizeof(MIXERCONTROL));
	L_memset(dst_MixerControl, 0, pmxl->cControls * sizeof(MIXERCONTROL));

	st_MixLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
	st_MixLineControls.dwLineID = pmxl->dwLineID;
	st_MixLineControls.cControls = pmxl->cControls;
	st_MixLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	st_MixLineControls.pamxctrl = dst_MixerControl;

	mmr = mixerGetLineControls((HMIXEROBJ) mh_Mixer, &st_MixLineControls, MIXER_GETLINECONTROLSF_ALL);
	if(MMSYSERR_NOERROR != mmr)
	{
		::MessageBox(NULL, "mixerGetLineInfo failed", "Error", MB_OK | MB_ICONEXCLAMATION);
	}
	else
	{
		for(i = 0; i < (int) pmxl->cControls; i++)
		{
			/* MixAppGetControlTypeName(&dst_MixerControl[i], szControlType); */
			pcombo->InsertString(i, dst_MixerControl[i].szName);
			pcombo->SetItemData(i, dst_MixerControl[i].dwControlID);
		}

		pcombo->SetTopIndex(0);
		pcombo->SetCurSel(0);
		mi_CurrMixerControl = -1;
		OnSelMixerControl();
	}

	L_free(dst_MixerControl);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::MixHideAllButtons(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	CEdit		*po_Edit;
	CSliderCtrl *po_Slider;
	CButton		*po_Button;
	/*~~~~~~~~~~~~~~~~~~~*/

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Edit = (CEdit *) GetDlgItem(IDC_MIXEDIT1);
	po_Edit->ShowWindow(SW_HIDE);

	po_Edit = (CEdit *) GetDlgItem(IDC_MIXEDIT2);
	po_Edit->ShowWindow(SW_HIDE);

	po_Edit = (CEdit *) GetDlgItem(IDC_MIXEDIT3);
	po_Edit->ShowWindow(SW_HIDE);

	po_Edit = (CEdit *) GetDlgItem(IDC_MIXEDIT4);
	po_Edit->ShowWindow(SW_HIDE);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_MIXSLIDER1);
	po_Slider->ShowWindow(SW_HIDE);

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_MIXSLIDER2);
	po_Slider->ShowWindow(SW_HIDE);

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_MIXSLIDER3);
	po_Slider->ShowWindow(SW_HIDE);

	po_Slider = (CSliderCtrl *) GetDlgItem(IDC_MIXSLIDER4);
	po_Slider->ShowWindow(SW_HIDE);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Button = (CButton *) GetDlgItem(IDC_MIXRADIO1);
	po_Button->ShowWindow(SW_HIDE);

	po_Button = (CButton *) GetDlgItem(IDC_MIXRADIO2);
	po_Button->ShowWindow(SW_HIDE);

	po_Button = (CButton *) GetDlgItem(IDC_MIXRADIO3);
	po_Button->ShowWindow(SW_HIDE);

	po_Button = (CButton *) GetDlgItem(IDC_MIXRADIO4);
	po_Button->ShowWindow(SW_HIDE);

	po_Button = (CButton *) GetDlgItem(IDC_MIXRADIO5);
	po_Button->ShowWindow(SW_HIDE);

	po_Button = (CButton *) GetDlgItem(IDC_MIXRADIO6);
	po_Button->ShowWindow(SW_HIDE);

	po_Button = (CButton *) GetDlgItem(IDC_MIXRADIO7);
	po_Button->ShowWindow(SW_HIDE);

	po_Button = (CButton *) GetDlgItem(IDC_MIXRADIO8);
	po_Button->ShowWindow(SW_HIDE);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Button = (CButton *) GetDlgItem(IDC_MIXCHECK1);
	po_Button->ShowWindow(SW_HIDE);

	po_Button = (CButton *) GetDlgItem(IDC_MIXCHECK2);
	po_Button->ShowWindow(SW_HIDE);

	po_Button = (CButton *) GetDlgItem(IDC_MIXCHECK3);
	po_Button->ShowWindow(SW_HIDE);

	po_Button = (CButton *) GetDlgItem(IDC_MIXCHECK4);
	po_Button->ShowWindow(SW_HIDE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::OnSelMixerControl(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox			*pcombo;
	int					iNew, iID;
	MMRESULT			mmr;

	MIXERLINECONTROLS	mxlc;

	MACONTROLINSTANCE	maci;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    get selection
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_CTRL);
	iNew = pcombo->GetCurSel();
	iID = pcombo->GetItemData(iNew);

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    check selection
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(iNew == -1) return;
	if(iNew == mi_CurrMixerControl) return;
	if(iNew >= (int) (mst_MixerConnection.cControls)) return;
	mi_CurrMixerControl = iNew;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    get control
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	mxlc.cbStruct = sizeof(mxlc);
	mxlc.dwControlID = iID;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mst_MixerControl;

	mmr = mixerGetLineControls((HMIXEROBJ) mh_Mixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYID);
	if(MMSYSERR_NOERROR != mmr)
	{
		::MessageBox(NULL, "get control has failed", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if(MIXERCONTROL_CONTROLF_DISABLED & mst_MixerControl.fdwControl)
	{
		::MessageBox(NULL, "control is disabled", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    draw control
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

    MixHideAllButtons();
	maci.hmx = mh_Mixer;
	maci.pmxl = &mst_MixerConnection;
	maci.pmxctrl = &mst_MixerControl;

	switch(MIXERCONTROL_CT_CLASS_MASK & mst_MixerControl.dwControlType)
	{
	case MIXERCONTROL_CT_CLASS_CUSTOM:
		DrawCustomControl(&maci);
		break;
	case MIXERCONTROL_CT_CLASS_METER:
		DrawMeterControl(&maci);
		break;
	case MIXERCONTROL_CT_CLASS_SWITCH:
		DrawSwitchControl(&maci);
		break;
	case MIXERCONTROL_CT_CLASS_NUMBER:
		DrawNumberControl(&maci);
		break;
	case MIXERCONTROL_CT_CLASS_SLIDER:
		DrawSliderControl(&maci);
		break;
	case MIXERCONTROL_CT_CLASS_FADER:
		DrawFaderControl(&maci);
		break;
	case MIXERCONTROL_CT_CLASS_LIST:
		DrawListControl(&maci);
		break;
	default:
		DrawUnknownControl(&maci);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::DrawCustomControl(void *pv_Data)
{
	::MessageBox(NULL, "Class Custom", "Message", MB_OK);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::DrawMeterControl(void *pv_Data)
{
	::MessageBox(NULL, "Class Meter", "Message", MB_OK);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::DrawSwitchControl(void *pv_Data)
{
	::MessageBox(NULL, "Class Switch", "Message", MB_OK);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::DrawNumberControl(void *pv_Data)
{
	::MessageBox(NULL, "Class Number", "Message", MB_OK);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::DrawSliderControl(void *pv_Data)
{
	::MessageBox(NULL, "Class Custom", "Message", MB_OK);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::DrawFaderControl(void *pv_Data)
{
	/*~~~~~~~~~~~~~*/
	CEdit	*apo_Edit[10];
    CSliderCtrl *apo_Slider[10];
    MACONTROLINSTANCE *p_FaderCtrl;
    UINT                            cChannels;
    UINT                            cMultipleItems;
    MIXERCONTROLDETAILS             mxcd;
    MIXERCONTROLDETAILS_UNSIGNED    *pmxcd_u;
    unsigned int i,j;
    UINT uIndex  ;
    int nRange,nValue;
    MMRESULT    mmr;
    char asz[128];
    
	/*~~~~~~~~~~~~~*/

    p_FaderCtrl = (MACONTROLINSTANCE*)pv_Data;

    cChannels = mst_MixerConnection.cChannels;
    if (MIXERCONTROL_CONTROLF_UNIFORM & mst_MixerControl.fdwControl)
        cChannels = 1;

    cMultipleItems = 1;
    if (MIXERCONTROL_CONTROLF_MULTIPLE & mst_MixerControl.fdwControl)
        cMultipleItems = mst_MixerControl.cMultipleItems;
    
    pmxcd_u = (MIXERCONTROLDETAILS_UNSIGNED*)L_malloc(cChannels*cMultipleItems*sizeof(MIXERCONTROLDETAILS_UNSIGNED));

    mxcd.cbStruct       = sizeof(mxcd);
    mxcd.dwControlID    = mst_MixerControl.dwControlID;
    mxcd.cChannels      = cChannels;
    mxcd.cMultipleItems = mst_MixerControl.cMultipleItems;
    mxcd.cbDetails      = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails      = pmxcd_u;

    mmr = mixerGetControlDetails((HMIXEROBJ)mh_Mixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
    if (MMSYSERR_NOERROR != mmr)
    {
	    ::MessageBox(NULL, "mixerGetControlDetails failure", "Message", MB_OK|MB_ICONEXCLAMATION);
        return ;
    }
 
    
    
    
    apo_Edit[0] = (CEdit *) GetDlgItem(IDC_MIXEDIT1);
    apo_Edit[1] = (CEdit *) GetDlgItem(IDC_MIXEDIT2);
    apo_Edit[2] = (CEdit *) GetDlgItem(IDC_MIXEDIT3);
    apo_Edit[3] = (CEdit *) GetDlgItem(IDC_MIXEDIT4);

    apo_Slider[0] = (CSliderCtrl*)GetDlgItem(IDC_MIXSLIDER1);
    apo_Slider[1] = (CSliderCtrl*)GetDlgItem(IDC_MIXSLIDER2);
    apo_Slider[2] = (CSliderCtrl*)GetDlgItem(IDC_MIXSLIDER3);
    apo_Slider[3] = (CSliderCtrl*)GetDlgItem(IDC_MIXSLIDER4);

    nRange   = (int)min(32767, mst_MixerControl.Metrics.cSteps - 1);    
    for(i=0; i<cChannels;i++)
    {
        for(j=0;j<cMultipleItems; j++)
        {
            uIndex = (i * cMultipleItems) + j;
            nValue = (int)MulDiv(pmxcd_u[uIndex].dwValue, nRange, 0xFFFF);

            sprintf(asz, "channel %d : %d \%",i, MulDiv(100,nValue,nRange));
            apo_Edit[uIndex]->SetWindowText(asz);
            apo_Edit[uIndex]->ShowWindow(SW_SHOW);

            apo_Slider[uIndex]->ShowWindow(SW_SHOW);
            apo_Slider[uIndex]->ClearTics();
            apo_Slider[uIndex]->SetTic(0);
            apo_Slider[uIndex]->SetTicFreq(10);//mst_MixerControl.Metrics.cSteps);
            apo_Slider[uIndex]->SetLineSize(1);
            apo_Slider[uIndex]->SetPageSize(10);
            apo_Slider[uIndex]->SetRange(0,nRange, TRUE);            
            apo_Slider[uIndex]->SetPos(nValue);
            
        }
    }
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::DrawListControl(void *pv_Data)
{
	::MessageBox(NULL, "Class List", "Message", MB_OK);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MixerDialog::DrawUnknownControl(void *pv_Data)
{
	::MessageBox(NULL, "Class Unknwon", "Message", MB_OK);
}

void EDIA_cl_MixerDialog::OnPos1Change(void)
{
    int nValue;
    CSliderCtrl*po_Slider;
    po_Slider = (CSliderCtrl*)GetDlgItem(IDC_MIXSLIDER1);
    nValue = po_Slider->GetPos();
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */

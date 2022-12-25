/*$T DIAsndinstance_dlg.cpp GC! 1.097 02/21/02 09:12:12 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAsndinstance_dlg.h"
#include "Res/Res.h"
#include "VAVview/VAVlist.h"
#include "LINks/LINKtoed.h"

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDtrack.h"

BEGIN_MESSAGE_MAP(EDIA_cl_SndInstanceDialog, EDIA_cl_BaseDialog)
	ON_COMMAND(ESON_IDC_CHECKDYNPAN, OnUseCheckDynPan)
	ON_COMMAND(ESON_IDC_CHECKX, OnUseCheckX)
	ON_COMMAND(ESON_IDC_CHECKY, OnUseCheckY)
	ON_COMMAND(ESON_IDC_CHECKZ, OnUseCheckZ)
	ON_COMMAND(ESON_IDC_RADIOSPHVOL, OnUseSphericVol)
	ON_COMMAND(ESON_IDC_RADIOAXISVOL, OnUseAxisVol)
	ON_COMMAND(ESON_IDC_CHECKDOPPLER, OnUseCheckDoppler)
	ON_COMMAND(ESON_IDC_CHECKFX, OnUseCheckFx)
	ON_COMMAND(ESON_IDC_RADIOGRP1, OnUseGroup1)
	ON_COMMAND(ESON_IDC_RADIOGRP2, OnUseGroup2)
	ON_COMMAND(ESON_IDC_RADIOGRP3, OnUseGroup3)
	ON_COMMAND(ESON_IDC_RADIOGRP4, OnUseGroup4)
	ON_COMMAND(IDAPPLY, OnApply)
	ON_COMMAND(IDOK, OnOk)
	ON_COMMAND(IDREFRESH, OnRefresh)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndInstanceDialog::EDIA_cl_SndInstanceDialog(int index) :
	EDIA_cl_BaseDialog(ESON_IDD_DIALOG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(index < 0)
		pst_SI = NULL;
	else if(index >= SND_gst_Params.l_InstanceNumber)
		pst_SI = NULL;
	else
	{
		pst_SI = SND_gst_Params.dst_Instance + index;
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) pst_SI = NULL;
	}

	if(pst_SI)
	{
		mi_Index = index;
		L_memcpy(&mst_SoundInstance, pst_SI, sizeof(SND_tdst_SoundInstance));
		for(i = 0; i < 3; i++)
		{
			if(pst_SI->af_Near[i] - pst_SI->af_Far[i])
			{
				mst_SoundInstance.af_Middle[i] = (pst_SI->af_Middle[i] - pst_SI->af_Far[i]) / (pst_SI->af_Near[i] - pst_SI->af_Far[i]);
			}
			else
				mst_SoundInstance.af_Middle[i] = 0.5f;
		}
	}
	else
	{
		mi_Index = -1;
		L_memset(&mst_SoundInstance, 0, sizeof(SND_tdst_SoundInstance));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp[512];
	CString o_String;
	CButton *po_Button;
	/*~~~~~~~~~~~~~~~~~~*/

	LINK_gb_CanRefresh = FALSE;
	CDialog::DoDataExchange(pDX);
	if(!pDX->m_bSaveAndValidate)
	{
		if(mi_Index == -1)
		{
			sprintf(asz_Temp, "Sound Instance Settings - Instance is unvailable !!");
		}
		else
		{
			if(mst_SoundInstance.p_GameObject &&
				((OBJ_tdst_GameObject *) mst_SoundInstance.p_GameObject)->sz_Name)
			{
				sprintf
				(
					asz_Temp,
					"%3d - %s",
					mi_Index,
					((OBJ_tdst_GameObject *) mst_SoundInstance.p_GameObject)->sz_Name
				);
			}
			else
				sprintf(asz_Temp, "%3d - None", mi_Index);
		}

		SetWindowText(asz_Temp);

		sprintf(asz_Temp, "%1.4f", mst_SoundInstance.f_Volume);
		DDX_Text(pDX, ESON_IDC_INSTVOL, CString(asz_Temp));

		sprintf(asz_Temp, "%d", mst_SoundInstance.i_BaseFrequency);
		DDX_Text(pDX, ESON_IDC_INSTFREQ, CString(asz_Temp));

		sprintf(asz_Temp, "%d", mst_SoundInstance.i_Pan);
		DDX_Text(pDX, ESON_IDC_INSTPAN, CString(asz_Temp));

		sprintf(asz_Temp, "%d", mst_SoundInstance.i_InstTrack);
		DDX_Text(pDX, ESON_IDC_INSTTRACK, CString(asz_Temp));

		sprintf(asz_Temp, "%1.4f", mst_SoundInstance.f_MinPan);
		DDX_Text(pDX, ESON_IDC_INSTMINPAN, CString(asz_Temp));

		sprintf(asz_Temp, "%.2f", mst_SoundInstance.af_Near[0]);
		DDX_Text(pDX, ESON_IDC_INSTNEARX, CString(asz_Temp));
		sprintf(asz_Temp, "%.2f", mst_SoundInstance.af_Near[1]);
		DDX_Text(pDX, ESON_IDC_INSTNEARY, CString(asz_Temp));
		sprintf(asz_Temp, "%.2f", mst_SoundInstance.af_Near[2]);
		DDX_Text(pDX, ESON_IDC_INSTNEARZ, CString(asz_Temp));

		sprintf(asz_Temp, "%.2f", mst_SoundInstance.af_Middle[0]);
		DDX_Text(pDX, ESON_IDC_INSTMIDX, CString(asz_Temp));
		sprintf(asz_Temp, "%.2f", mst_SoundInstance.af_Middle[1]);
		DDX_Text(pDX, ESON_IDC_INSTMIDY, CString(asz_Temp));
		sprintf(asz_Temp, "%.2f", mst_SoundInstance.af_Middle[2]);
		DDX_Text(pDX, ESON_IDC_INSTMIDZ, CString(asz_Temp));

		sprintf(asz_Temp, "%.2f", mst_SoundInstance.af_Far[0]);
		DDX_Text(pDX, ESON_IDC_INSTFARX, CString(asz_Temp));
		sprintf(asz_Temp, "%.2f", mst_SoundInstance.af_Far[1]);
		DDX_Text(pDX, ESON_IDC_INSTFARY, CString(asz_Temp));
		sprintf(asz_Temp, "%.2f", mst_SoundInstance.af_Far[2]);
		DDX_Text(pDX, ESON_IDC_INSTFARZ, CString(asz_Temp));

		sprintf(asz_Temp, "%1.4f", mst_SoundInstance.f_MiddleCoeff);
		DDX_Text(pDX, ESON_IDC_INSTMIDFACTOR, CString(asz_Temp));

		sprintf(asz_Temp, "%1.4f", mst_SoundInstance.f_FarCoeff);
		DDX_Text(pDX, ESON_IDC_INSTFARFACTOR, CString(asz_Temp));

		sprintf(asz_Temp, "%.2f", mst_SoundInstance.f_DeltaFar);
		DDX_Text(pDX, ESON_IDC_INSTDELTAFAR, CString(asz_Temp));

		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKDYNPAN);
		if(mst_SoundInstance.ul_Flags & SND_Cul_SF_DynPan)
			po_Button->SetCheck(1);
		else
			po_Button->SetCheck(0);

		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKDOPPLER);

		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKFX);
		

		CheckDynVol();
	}
	else
	{
		DDX_Text(pDX, ESON_IDC_INSTVOL, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.f_Volume);

		DDX_Text(pDX, ESON_IDC_INSTFREQ, o_String);
		sscanf((char *) (LPCSTR) o_String, "%d", &mst_SoundInstance.i_BaseFrequency);

		DDX_Text(pDX, ESON_IDC_INSTPAN, o_String);
		sscanf((char *) (LPCSTR) o_String, "%d", &mst_SoundInstance.i_Pan);

		DDX_Text(pDX, ESON_IDC_INSTTRACK, o_String);
		sscanf((char *) (LPCSTR) o_String, "%d", &mst_SoundInstance.i_InstTrack);

		DDX_Text(pDX, ESON_IDC_INSTMINPAN, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.f_MinPan);

		DDX_Text(pDX, ESON_IDC_INSTNEARX, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.af_Near[0]);
		DDX_Text(pDX, ESON_IDC_INSTNEARY, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.af_Near[1]);
		DDX_Text(pDX, ESON_IDC_INSTNEARZ, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.af_Near[2]);

		DDX_Text(pDX, ESON_IDC_INSTMIDX, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.af_Middle[0]);
		DDX_Text(pDX, ESON_IDC_INSTMIDY, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.af_Middle[1]);
		DDX_Text(pDX, ESON_IDC_INSTMIDZ, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.af_Middle[2]);

		DDX_Text(pDX, ESON_IDC_INSTFARX, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.af_Far[0]);
		DDX_Text(pDX, ESON_IDC_INSTFARY, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.af_Far[1]);
		DDX_Text(pDX, ESON_IDC_INSTFARZ, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.af_Far[2]);

		DDX_Text(pDX, ESON_IDC_INSTMIDFACTOR, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.f_MiddleCoeff);

		DDX_Text(pDX, ESON_IDC_INSTFARFACTOR, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.f_FarCoeff);

		DDX_Text(pDX, ESON_IDC_INSTDELTAFAR, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mst_SoundInstance.f_DeltaFar);
	}

	LINK_gb_CanRefresh = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseCheckDynPan(void)
{
	mst_SoundInstance.ul_Flags ^= SND_Cul_SF_DynPan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseCheckX(void)
{
	if(mst_SoundInstance.ul_Flags & SND_Cul_SF_DynVolSpheric)
	{
		mst_SoundInstance.ul_Flags &= ~SND_Cul_SF_DynVolSpheric;
	}
	else if(mst_SoundInstance.ul_Flags & SND_Cul_SF_DynVolXaxis)
	{
		mst_SoundInstance.ul_Flags &= ~SND_Cul_SF_DynVolXaxis;
	}
	else if(mst_SoundInstance.ul_Flags & (SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis))
	{
		mst_SoundInstance.ul_Flags |= SND_Cul_SF_DynVolXaxis;
	}
	else
	{
		mst_SoundInstance.ul_Flags |= SND_Cul_SF_DynVolSpheric;
	}

	CheckDynVol();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseCheckY(void)
{
	if(mst_SoundInstance.ul_Flags & SND_Cul_SF_DynVolSpheric)
	{
		mst_SoundInstance.ul_Flags &= ~SND_Cul_SF_DynVolSpheric;
		mst_SoundInstance.ul_Flags |= SND_Cul_SF_DynVolYaxis;
	}
	else
	{
		mst_SoundInstance.ul_Flags ^= SND_Cul_SF_DynVolYaxis;
	}

	CheckDynVol();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseCheckZ(void)
{
	if(mst_SoundInstance.ul_Flags & SND_Cul_SF_DynVolSpheric)
	{
		mst_SoundInstance.ul_Flags &= ~SND_Cul_SF_DynVolSpheric;
		mst_SoundInstance.ul_Flags |= SND_Cul_SF_DynVolZaxis;
	}
	else
	{
		mst_SoundInstance.ul_Flags ^= SND_Cul_SF_DynVolZaxis;
	}

	CheckDynVol();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseSphericVol(void)
{
	if(mst_SoundInstance.ul_Flags & SND_Cul_SF_DynVolSpheric)
	{
		mst_SoundInstance.ul_Flags &= ~SND_Cul_SF_DynVolSpheric;
	}
	else
	{
		mst_SoundInstance.ul_Flags |= SND_Cul_SF_DynVolSpheric;
		mst_SoundInstance.ul_Flags &= ~(SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis);
	}

	CheckDynVol();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseAxisVol(void)
{
	if(mst_SoundInstance.ul_Flags & (SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis))
	{
		mst_SoundInstance.ul_Flags &= ~(SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis);
	}
	else
	{
		mst_SoundInstance.ul_Flags &= ~SND_Cul_SF_DynVolSpheric;
		mst_SoundInstance.ul_Flags |= SND_Cul_SF_DynVolXaxis;
	}

	CheckDynVol();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseCheckDoppler(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseCheckFx(void)
{
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::CheckDynVol(void)
{
	/*~~~~~~~~~~~~~~~*/
	CButton *po_Button;
	CEdit	*po_Edit;
	/*~~~~~~~~~~~~~~~*/

	if(mst_SoundInstance.ul_Flags & SND_Cul_SF_DynVolSpheric)
	{
		mst_SoundInstance.ul_Flags &= ~(SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis);

		/* global radio */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_RADIOSPHVOL);
		po_Button->SetCheck(1);
		po_Button = (CButton *) GetDlgItem(ESON_IDC_RADIOAXISVOL);
		po_Button->SetCheck(0);

		/* 3D coeff */
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDFACTOR);
		po_Edit->SetReadOnly(FALSE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARFACTOR);
		po_Edit->SetReadOnly(FALSE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTDELTAFAR);
		po_Edit->SetReadOnly(FALSE);

		/* X */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKX);
		po_Button->SetCheck(1);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARX);
		po_Edit->SetReadOnly(FALSE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARX);
		po_Edit->SetReadOnly(FALSE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDX);
		po_Edit->SetReadOnly(FALSE);

		/* Y */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKY);
		po_Button->SetCheck(0);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARY);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARY);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDY);
		po_Edit->SetReadOnly(TRUE);

		/* Z */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKZ);
		po_Button->SetCheck(0);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARZ);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARZ);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDZ);
		po_Edit->SetReadOnly(TRUE);
	}
	else if(mst_SoundInstance.ul_Flags & (SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis))
	{
		mst_SoundInstance.ul_Flags &= ~(SND_Cul_SF_DynVolSpheric);

		/* global radio */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_RADIOSPHVOL);
		po_Button->SetCheck(0);
		po_Button = (CButton *) GetDlgItem(ESON_IDC_RADIOAXISVOL);
		po_Button->SetCheck(1);

		/* 3D coeff */
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDFACTOR);
		po_Edit->SetReadOnly(FALSE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARFACTOR);
		po_Edit->SetReadOnly(FALSE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTDELTAFAR);
		po_Edit->SetReadOnly(FALSE);

		/* X */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKX);
		if(mst_SoundInstance.ul_Flags & SND_Cul_SF_DynVolXaxis)
		{
			po_Button->SetCheck(1);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARX);
			po_Edit->SetReadOnly(FALSE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARX);
			po_Edit->SetReadOnly(FALSE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDX);
			po_Edit->SetReadOnly(FALSE);
		}
		else
		{
			po_Button->SetCheck(0);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARX);
			po_Edit->SetReadOnly(TRUE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARX);
			po_Edit->SetReadOnly(TRUE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDX);
			po_Edit->SetReadOnly(TRUE);
		}

		/* Y */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKY);
		if(mst_SoundInstance.ul_Flags & SND_Cul_SF_DynVolYaxis)
		{
			po_Button->SetCheck(1);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARY);
			po_Edit->SetReadOnly(FALSE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARY);
			po_Edit->SetReadOnly(FALSE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDY);
			po_Edit->SetReadOnly(FALSE);
		}
		else
		{
			po_Button->SetCheck(0);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARY);
			po_Edit->SetReadOnly(TRUE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARY);
			po_Edit->SetReadOnly(TRUE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDY);
			po_Edit->SetReadOnly(TRUE);
		}

		/* Z */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKZ);
		if(mst_SoundInstance.ul_Flags & SND_Cul_SF_DynVolZaxis)
		{
			po_Button->SetCheck(1);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARZ);
			po_Edit->SetReadOnly(FALSE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARZ);
			po_Edit->SetReadOnly(FALSE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDZ);
			po_Edit->SetReadOnly(FALSE);
		}
		else
		{
			po_Button->SetCheck(0);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARZ);
			po_Edit->SetReadOnly(TRUE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARZ);
			po_Edit->SetReadOnly(TRUE);
			po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDZ);
			po_Edit->SetReadOnly(TRUE);
		}
	}
	else
	{
		mst_SoundInstance.ul_Flags &= ~(SND_Cul_SF_DynVolSpheric | SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis);

		/* global radio */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_RADIOSPHVOL);
		po_Button->SetCheck(0);
		po_Button = (CButton *) GetDlgItem(ESON_IDC_RADIOAXISVOL);
		po_Button->SetCheck(0);

		/* 3D coeff */
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDFACTOR);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARFACTOR);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTDELTAFAR);
		po_Edit->SetReadOnly(TRUE);

		/* X */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKX);
		po_Button->SetCheck(0);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARX);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARX);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDX);
		po_Edit->SetReadOnly(TRUE);

		/* Y */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKY);
		po_Button->SetCheck(0);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARY);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARY);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDY);
		po_Edit->SetReadOnly(TRUE);

		/* Z */
		po_Button = (CButton *) GetDlgItem(ESON_IDC_CHECKZ);
		po_Button->SetCheck(0);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTNEARZ);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTFARZ);
		po_Edit->SetReadOnly(TRUE);
		po_Edit = (CEdit *) GetDlgItem(ESON_IDC_INSTMIDZ);
		po_Edit->SetReadOnly(TRUE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseGroup1(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseGroup2(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseGroup3(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnUseGroup4(void)
{	
}

void EDIA_cl_SndInstanceDialog::OnUseGroup5(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnApply(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(mi_Index, pst_SI, ;);

	UpdateData(TRUE);

	if(mst_SoundInstance.f_Volume != pst_SI->f_Volume)
	{
		SND_SetVol(mi_Index, mst_SoundInstance.f_Volume);
	}

	if(mst_SoundInstance.i_BaseFrequency != pst_SI->i_BaseFrequency)
	{
		SND_SetFreq(mi_Index, mst_SoundInstance.i_BaseFrequency);
	}

	if(mst_SoundInstance.i_Pan != pst_SI->i_Pan)
	{
		SND_SetPan(mi_Index, mst_SoundInstance.i_Pan, 0);
	}

	if(mst_SoundInstance.i_InstTrack != pst_SI->i_InstTrack)
	{
		SND_InstTrackSet(mi_Index, mst_SoundInstance.i_InstTrack);
	}

	if(mst_SoundInstance.f_MinPan != pst_SI->f_MinPan)
	{
		pst_SI->f_MinPan = mst_SoundInstance.f_MinPan;
	}
 
	if
	(
		mst_SoundInstance.ul_Flags &
			(SND_Cul_SF_DynVolSpheric | SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis)
	)
	{
		/*~~~~~~~~~~~*/
		int		i;
		float	middle;
		/*~~~~~~~~~~~*/

		for(i = 0; i < 3; i++)
		{
			if(mst_SoundInstance.af_Near[i] != pst_SI->af_Near[i])
			{
				pst_SI->af_Near[i] = mst_SoundInstance.af_Near[i];
			}

			if(mst_SoundInstance.af_Far[i] != pst_SI->af_Far[i])
			{
				pst_SI->af_Far[i] = mst_SoundInstance.af_Far[i];
			}

			middle = MATH_f_FloatBlend(pst_SI->af_Near[i], pst_SI->af_Far[i], mst_SoundInstance.af_Middle[i]);
			if(middle != pst_SI->af_Middle[i])
			{
				pst_SI->af_Middle[i] = middle;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnRefresh(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(mi_Index, pst_SI, ;);

	L_memcpy(&mst_SoundInstance, pst_SI, sizeof(SND_tdst_SoundInstance));
	for(i = 0; i < 3; i++)
	{
		if(pst_SI->af_Near[i] - pst_SI->af_Far[i])
		{
			mst_SoundInstance.af_Middle[i] = (pst_SI->af_Middle[i] - pst_SI->af_Far[i]) / (pst_SI->af_Near[i] - pst_SI->af_Far[i]);
		}
		else
			mst_SoundInstance.af_Middle[i] = 0.5f;
	}

	UpdateData(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInstanceDialog::OnOk(void)
{
	this->OnApply();
	EDIA_cl_BaseDialog::OnOK();
}

#endif /* ACTIVE_EDITORS */

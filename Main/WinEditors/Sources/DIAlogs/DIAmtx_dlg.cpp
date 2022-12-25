/*$T DIAmtx_dlg.cpp GC 1.138 09/18/03 12:04:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAmtx_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"

#include "Res/Res.h"
#include "VAVview/VAVlist.h"
#include "LINks/LINKtoed.h"

#include "BIGfiles/BIGfat.h"

#include "Sound/Sources/SND.h"
#include "IOP/RPC_Manager.h"

BEGIN_MESSAGE_MAP(EDIA_cl_MtxDialog, EDIA_cl_BaseDialog)
	ON_COMMAND(IDC_BUTTON_CHOOSEVIDEO, OnSelectVideoFile)
	ON_COMMAND(IDC_BUTTON_CHOOSESND, OnSelectSoundFile)
	ON_COMMAND(IDC_BUTTON_CHOOSEOUT, OnSelectOutputFile)
	ON_COMMAND(IDC_RADIO_NTSC, OnSelectNtsc)
	ON_COMMAND(IDC_RADIO_PAL, OnSelectPal)
	ON_EN_CHANGE(IDC_EDIT_VIDEOBUFFSIZE, OnEditChangeVideoBuff)
	ON_EN_CHANGE(IDC_EDIT_SNDBUFFSIZE, OnEditChangeSndBuff)
	ON_EN_CHANGE(IDC_EDIT_SNDFREQ, OnEditChangeSndFreq)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MtxDialog::EDIA_cl_MtxDialog(ULONG _ul_VideoFatIndex, ULONG _ul_SoundFatIndex) :
	EDIA_cl_BaseDialog(ESON_IDD_MTX)
{
	/* video */
	mul_VideoFatIndex = -1;
	mo_VideoFile = CString("");
	L_memset(&mst_VideoInfo, 0, sizeof(MTX_tdst_VideoInfo));
	mb_VideoIsNTSC = TRUE;
	mui_VideoBufferSize = 1 * 1024 * 1024;
	mf_VideoDuration = 0.0f;

	/* sound */
	mul_SoundFatIndex = -1;
	mo_SoundFile = CString("");
	mi_SoundPlayerFrequency = 0;
	mf_SoundPlayerVolume = 1.0f;
	L_memset(&mst_SoundInfo, 0, sizeof(MTX_tdst_SoundInfo));
	mui_SoundBufferSize = 50 * 1024;
	mui_SoundTotalSize = mui_SoundBufferSize;

	/* output */
	mul_OutputFatIndex = -1;
	mo_OutputPath = CString("");
	mo_OutputFile = CString("");
	mb_OutputFileExist = FALSE;

	mb_EnableOKButton = TRUE;

	if(_ul_VideoFatIndex != BIG_C_InvalidIndex)
	{
		if(MTX_i_ParseVideoFile(&mst_VideoInfo, BIG_FileKey(_ul_VideoFatIndex)))
		{
			mul_VideoFatIndex = -1;
		}
		else
		{
			mul_VideoFatIndex = _ul_VideoFatIndex;
			mo_VideoFile = CString(BIG_NameFile(mul_VideoFatIndex));
			SetOutputFileFromVideoFile(FALSE);
		}
	}

	if(_ul_SoundFatIndex != BIG_C_InvalidIndex)
	{
		MTX_i_ParseSoundFile(&mst_SoundInfo, BIG_FileKey(_ul_SoundFatIndex));
		mul_SoundFatIndex = _ul_SoundFatIndex;
		mo_SoundFile = CString(BIG_NameFile(mul_SoundFatIndex));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MtxDialog::~EDIA_cl_MtxDialog(void)
{
	MTX_FreeVideoFileInfo(&mst_VideoInfo);
	MTX_FreeSoundFileInfo(&mst_SoundInfo);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp[512];
	CString o_String;
	CEdit	*po_Edit;
	CButton *po_Button;
	/*~~~~~~~~~~~~~~~~~~*/

	LINK_gb_CanRefresh = FALSE;
	CDialog::DoDataExchange(pDX);
	if(!pDX->m_bSaveAndValidate)
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    open dialog
		 ---------------------------------------------------------------------------------------------------------------
		 */

		/*$1- video section ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		SetPath(mul_VideoFatIndex, IDC_EDIT_VIDEOPATH);
		po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_VIDEOFILE);
		po_Edit->SetWindowText(mo_VideoFile);

		/* duration */
		SetVideoDuration();

		/* NTSC / PAL */
		if(mb_VideoIsNTSC)
		{
			po_Button = (CButton *) GetDlgItem(IDC_RADIO_NTSC);
			po_Button->SetCheck(1);
			po_Button = (CButton *) GetDlgItem(IDC_RADIO_PAL);
			po_Button->SetCheck(0);
		}
		else
		{
			po_Button = (CButton *) GetDlgItem(IDC_RADIO_PAL);
			po_Button->SetCheck(1);
			po_Button = (CButton *) GetDlgItem(IDC_RADIO_NTSC);
			po_Button->SetCheck(0);
		}

		/* buffer */
		sprintf(asz_Temp, "%d", mui_VideoBufferSize);
		DDX_Text(pDX, IDC_EDIT_VIDEOBUFFSIZE, CString(asz_Temp));

		/*$1- sound section ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* path + file */
		SetPath(mul_SoundFatIndex, IDC_EDIT_SNDPATH);
		po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_SNDFILE);
		po_Edit->SetWindowText(mo_SoundFile);

		/* sound duration */
		SetSoundDuration();

		/* volume */
		sprintf(asz_Temp, "%1.4f", mf_SoundPlayerVolume);
		DDX_Text(pDX, IDC_EDIT_SNDVOL, CString(asz_Temp));

		/* freq */
		sprintf(asz_Temp, "%d", mi_SoundPlayerFrequency);
		DDX_Text(pDX, IDC_EDIT_SNDFREQ, CString(asz_Temp));

		/* buffer */
		sprintf(asz_Temp, "%d", mui_SoundBufferSize);
		DDX_Text(pDX, IDC_EDIT_SNDBUFFSIZE, CString(asz_Temp));

		/*$1- output section ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_OUTPATH);
		po_Edit->SetWindowText(mo_OutputPath);

		po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_OUTFILE);
		po_Edit->SetWindowText(mo_OutputFile);

		/*$1- OK Button ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		CheckCreationCondition();
	}
	else
	{

		/*$1- get video buffer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		DDX_Text(pDX, IDC_EDIT_VIDEOBUFFSIZE, o_String);
		sscanf((char *) (LPCSTR) o_String, "%d", &mui_VideoBufferSize);

		DDX_Text(pDX, IDC_EDIT_SNDBUFFSIZE, o_String);
		sscanf((char *) (LPCSTR) o_String, "%d", &mui_SoundBufferSize);

		/*$1- get volume ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		DDX_Text(pDX, IDC_EDIT_SNDVOL, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mf_SoundPlayerVolume);
		SND_M_VolumeSaturation(mf_SoundPlayerVolume);

		/*$1- get freq ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		DDX_Text(pDX, IDC_EDIT_SNDFREQ, o_String);
		sscanf((char *) (LPCSTR) o_String, "%d", &mi_SoundPlayerFrequency);
		if(mi_SoundPlayerFrequency < 0) mi_SoundPlayerFrequency = 48000;
		if(mi_SoundPlayerFrequency > 100000) mi_SoundPlayerFrequency = 48000;
	}

	LINK_gb_CanRefresh = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::OnSelectVideoFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString				o_Temp;
	CEdit				*po_Edit;
	EDIA_cl_FileDialog	o_File("Choose Video File", 0, 0, 1, EDI_Csz_Path_Textures, "*"EDI_Csz_ExtVideo1);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		mul_VideoFatIndex = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(mul_VideoFatIndex == BIG_C_InvalidIndex)
		{
			mul_VideoFatIndex = -1;
			mo_VideoFile = CString("");
		}
		else
		{
			if(MTX_i_ParseVideoFile(&mst_VideoInfo, BIG_FileKey(mul_VideoFatIndex)))
			{
				M_MF()->MessageBox("The video file is invalid", "Error", MB_OK | MB_ICONSTOP);
				mul_VideoFatIndex = -1;
				mo_VideoFile = CString("");
			}
			else
			{
				mo_VideoFile = o_Temp;

				SetPath(mul_VideoFatIndex, IDC_EDIT_VIDEOPATH);
				po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_VIDEOFILE);
				po_Edit->SetWindowText(mo_VideoFile);
			}
		}

		SetOutputFileFromVideoFile();
		SetVideoDuration();
		EvalVideoBufferSize();
	}

	CheckCreationCondition();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::OnSelectSoundFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString				o_Temp;
	CEdit				*po_Edit;
	EDIA_cl_FileDialog	o_File
						(
							"Choose File",
							0,
							0,
							1,
							EDI_Csz_Path_Audio,
							"*"EDI_Csz_ExtSoundFile ",*"EDI_Csz_ExtLoadingSound ",*"EDI_Csz_ExtSoundMusic ",*"EDI_Csz_ExtSoundAmbience
								",*"EDI_Csz_ExtSoundDialog
						);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		mul_SoundFatIndex = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(mul_SoundFatIndex == BIG_C_InvalidIndex)
		{
			mul_SoundFatIndex = -1;
			mo_SoundFile = CString("");
		}
		else
		{
			mo_SoundFile = o_Temp;

			SetPath(mul_SoundFatIndex, IDC_EDIT_SNDPATH);
			po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_SNDFILE);
			po_Edit->SetWindowText(mo_SoundFile);

			MTX_i_ParseSoundFile(&mst_SoundInfo, BIG_FileKey(mul_SoundFatIndex));
		}
	}

	SetSoundDuration();
	EvalVideoBufferSize();
	CheckCreationCondition();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::OnSelectPal(void)
{
	if(mb_VideoIsNTSC)
	{
		mb_VideoIsNTSC = FALSE;
		SetVideoDuration();
		EvalVideoBufferSize();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::OnSelectNtsc(void)
{
	if(!mb_VideoIsNTSC)
	{
		mb_VideoIsNTSC = TRUE;
		SetVideoDuration();
		EvalVideoBufferSize();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::OnSelectOutputFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CEdit				*po_Edit;
	EDIA_cl_NameDialog	o_Dialog("Enter a new file name");
	char				*psz_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_Dialog.DoModal() == IDOK)
	{
		psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
		if(BIG_b_CheckName(psz_Name) == FALSE)
		{
			M_MF()->MessageBox("The file name is invalid", "Error", MB_YESNO | MB_ICONQUESTION);
		}
		else
		{
			mo_OutputFile = CString(psz_Name) + CString(EDI_Csz_ExtVideo3);
			if
			(
				(
					mul_OutputFatIndex = BIG_ul_SearchFileExt
						(
							(char *) (LPCSTR) mo_OutputPath,
							(char *) (LPCSTR) mo_OutputFile
						)
				) != BIG_C_InvalidIndex
			)
			{
				mb_OutputFileExist = TRUE;
			}
			else
			{
				mb_OutputFileExist = FALSE;
			}
		}

		po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_OUTPATH);
		po_Edit->SetWindowText(mo_OutputPath);

		po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_OUTFILE);
		po_Edit->SetWindowText(mo_OutputFile);
	}

	CheckCreationCondition();
}

/*$4
 ***********************************************************************************************************************
    PRIVATE MEMBER
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::SetPath(ULONG _ul_Fat, int _i_ID)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char	asz_Path[1024];
	CEdit	*po_Edit;
	/*~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Fat == BIG_C_InvalidIndex)
	{
		asz_Path[0] = 0;
		asz_Path[1] = 0;
	}
	else
	{
		BIG_ComputeFullName(BIG_ParentFile(_ul_Fat), asz_Path);
	}

	po_Edit = (CEdit *) GetDlgItem(_i_ID);
	po_Edit->SetWindowText(CString(asz_Path));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::CheckCreationCondition(void)
{
	/*~~~~~~~~~~~~~~~*/
	CButton *po_Button;
	/*~~~~~~~~~~~~~~~*/

	if((mul_SoundFatIndex != BIG_C_InvalidIndex) && (mul_VideoFatIndex != BIG_C_InvalidIndex))
	{
		mb_EnableOKButton = TRUE;
	}

	po_Button = (CButton *) GetDlgItem(IDOK);
	po_Button->EnableWindow(mb_EnableOKButton);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::SetOutputFileFromVideoFile(BOOL b_Refresh)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Path[1024];
	char	asz_NewName[1024];
	char	*pz_Temp;
	int		i;
	CEdit	*po_Edit;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(mul_VideoFatIndex == BIG_C_InvalidIndex)
	{
		mul_OutputFatIndex = -1;
		mo_OutputFile = CString("");
		mo_OutputPath = CString("");
		mb_OutputFileExist = FALSE;
	}
	else
	{
		BIG_ComputeFullName(BIG_ParentFile(mul_VideoFatIndex), asz_Path);
		mo_OutputPath = CString(asz_Path);

		for(pz_Temp = BIG_NameFile(mul_VideoFatIndex), i = 0; *pz_Temp != '.'; pz_Temp++, i++)
		{
			asz_NewName[i] = *pz_Temp;
		}

		asz_NewName[i] = '\0';
		L_strcat(asz_NewName, EDI_Csz_ExtVideo3);

		mo_OutputFile = CString(asz_NewName);

		/* check existence */
		if((mul_OutputFatIndex = BIG_ul_SearchFileExt(asz_Path, asz_NewName)) != BIG_C_InvalidIndex)
		{
			mb_OutputFileExist = TRUE;
		}
	}

	if(b_Refresh)
	{
		po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_OUTPATH);
		po_Edit->SetWindowText(mo_OutputPath);

		po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_OUTFILE);
		po_Edit->SetWindowText(mo_OutputFile);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::SetVideoDuration(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CEdit	*po_Edit;
	char	asz_Temp[512];
	/*~~~~~~~~~~~~~~~~~~*/

	po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_VIDEODURATION);
	mf_VideoDuration = 0.0f;

	if(mul_VideoFatIndex != -1)
	{
		if(mb_VideoIsNTSC)
			mf_VideoDuration = (float) mst_VideoInfo.ui_ImageNb * 2.0f / 60.0f;
		else
			mf_VideoDuration = (float) mst_VideoInfo.ui_ImageNb * 2.0f / 50.0f;
	}

	sprintf(asz_Temp, "%.3f sec", mf_VideoDuration);
	po_Edit->SetWindowText(asz_Temp);

	po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_VIDEOIMAGENB);
	if(mul_VideoFatIndex != -1)
	{
		sprintf(asz_Temp, "%d frames", mst_VideoInfo.ui_ImageNb);
	}
	else
	{
		sprintf(asz_Temp, "0 frames");
	}

	po_Edit->SetWindowText(asz_Temp);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::SetSoundDuration(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CEdit	*po_Edit;
	char	asz_Temp[512];
	/*~~~~~~~~~~~~~~~~~~*/

	po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_SOUNDDURATION);

	if(mul_SoundFatIndex != -1)
	{
		if(mi_SoundPlayerFrequency)
			sprintf(asz_Temp, "%.3f sec", mst_SoundInfo.f_Duration * mi_SoundPlayerFrequency / mst_SoundInfo.i_Freq);
		else
			sprintf(asz_Temp, "%.3f sec", mst_SoundInfo.f_Duration);
	}
	else
	{
		sprintf(asz_Temp, "0.0 sec");
	}

	po_Edit->SetWindowText(asz_Temp);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::OnEditChangeVideoBuff(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp[512];
	/*~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_EDIT_VIDEOBUFFSIZE)->GetWindowText(asz_Temp, 512);
	sscanf(asz_Temp, "%d", &mui_VideoBufferSize);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::OnEditChangeSndBuff(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp[512];
	/*~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_EDIT_SNDBUFFSIZE)->GetWindowText(asz_Temp, 512);
	sscanf(asz_Temp, "%d", &mui_SoundBufferSize);

	mui_SoundTotalSize = mui_SoundBufferSize;
	EvalVideoBufferSize();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::OnEditChangeSndFreq(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CEdit	*po_Edit;
	char	asz_Temp[512];
	/*~~~~~~~~~~~~~~~~~~*/

	/* get new freq */
	po_Edit = (CEdit *) GetDlgItem(IDC_EDIT_SNDFREQ);

	po_Edit->GetWindowText(asz_Temp, 512);
	sscanf(asz_Temp, "%d", &mi_SoundPlayerFrequency);

	/* update duration */
	SetSoundDuration();
	EvalVideoBufferSize();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MtxDialog::EvalVideoBufferSize(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			asz_Temp[512];
	float			ff;
	unsigned int	ii;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mul_SoundFatIndex == -1 ) return;
	if(!mf_VideoDuration) return;

	ff = ((float) mui_SoundBufferSize * 3.5f) / 2.0f;

	if(mi_SoundPlayerFrequency)
	{
		ff = ff / (float) mi_SoundPlayerFrequency;
	}
	else
	{
		ff = ff / (float) mst_SoundInfo.i_Freq;
	}

	ff = (ff * mst_VideoInfo.ui_FileSize) / mf_VideoDuration;

	ii = (unsigned int) (ff / 38912.0f + 0.5f);
	ii *= 38912;

	mui_VideoBufferSize = ii;
	sprintf(asz_Temp, "%d", mui_VideoBufferSize);
	GetDlgItem(IDC_EDIT_VIDEOBUFFSIZE)->SetWindowText(asz_Temp);
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */

/*$T DIAlink_dlg.cpp GC! 1.100 08/27/01 10:28:10 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGkey.h"
#include "DIAlogs/DIAlink_dlg.h"
#include "EDIerrid.h"
#include "EDIstrings.h"
#include "EDIpaths.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"

#ifdef JADEFUSION
extern BOOL BIG_gb_CanOpenFats;
#else
extern "C" BOOL BIG_gb_CanOpenFats;
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_LinkControlPassWord::EDIA_cl_LinkControlPassWord(BOOL _b_CurrentEnable) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_PASSWORD)
{
	mb_CurrentEnable = _b_CurrentEnable;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlPassWord::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);

	SetWindowText(mo_Title);
	DDX_Text(pDX, IDC_EDIT_NEW, mo_PassWord);

	/* Is the current edit box enable ? */
	if(!mb_CurrentEnable) ((CEdit *) GetDlgItem(IDC_EDIT_CURRENT))->EnableWindow(FALSE);

	/* Limit length of texts */
	((CEdit *) GetDlgItem(IDC_EDIT_CURRENT))->LimitText(EDI_C_MaxLenPassWord - 1);
	((CEdit *) GetDlgItem(IDC_EDIT_NEW))->LimitText(EDI_C_MaxLenPassWord - 1);
	((CEdit *) GetDlgItem(IDC_EDIT_CONFIRM))->LimitText(EDI_C_MaxLenPassWord - 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlPassWord::OnOK(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CString o_1, o_2, o_3;
	/*~~~~~~~~~~~~~~~~~~*/
	((CEdit *) GetDlgItem(IDC_EDIT_CURRENT))->GetWindowText(o_1);
	((CEdit *) GetDlgItem(IDC_EDIT_NEW))->GetWindowText(o_2);
	((CEdit *) GetDlgItem(IDC_EDIT_CONFIRM))->GetWindowText(o_3);

	if((o_1 != mo_CurrentPassWord) || (o_2 != o_3))
	{
		ERR_X_ForceError(EDI_ERR_Csz_BadPassWord, NULL);
		return;
	}

	EDIA_cl_BaseDialog::OnOK();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define EDI_C_ExcedentSize	275

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_LinkControlDialog, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_BN_CLICKED(IDC_CHECKR, OnCheckR)
	ON_COMMAND(IDBROWSE, OnBrowse)
	ON_COMMAND(IDPASSWORD, OnPassWord)
	ON_COMMAND(IDADMIN, OnShare)
	ON_COMMAND(IDADDUSER, OnAddUser)
	ON_COMMAND(IDDELETEUSER, OnDeleteUser)
	ON_COMMAND(IDREFRESH, RefreshLockList)
	ON_COMMAND(IDERASE, OnEraseLock)
	ON_COMMAND(IDEXIT, OnExit)
	ON_COMMAND(IDC_BUTTON_KILLRIGHTS, OnKillRights)
END_MESSAGE_MAP()
/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_LinkControlDialog::EDIA_cl_LinkControlDialog(void) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_LINKCONTROL)
{
	mb_Open = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_LinkControlDialog::OnInitDialog(void)
{
	/* Update infos for dialog */
	mi_LinkControlON = M_MF()->mst_Ini.b_LinkControlON ? 1 : 0;
	mo_UserName = M_MF()->mst_Ini.asz_CurrentUserName;
	mo_UserPassWord = M_MF()->mst_Ini.asz_UserPassWord;
	L_strcpy(masz_LinkControlFileVSS, M_MF()->mst_Ini.asz_CurrentVSSFile);
	L_strcpy(masz_LinkControlFileREF, M_MF()->mst_Ini.asz_CurrentRefFile);

	/* Base class call */
	EDIA_cl_BaseDialog::OnInitDialog();

	CenterWindow();

	/* Create current VSS file */
	L_strcpy(masz_CurrentVSSFile, BIG_gst.asz_Name);
	L_strcat(masz_CurrentVSSFile, EDI_Csz_ExtVss);

	/* Test if vss files are already here... */
	CanSetUserName();

	/* Update validate/unvalidate states of controls */
	EnableContent(mi_LinkControlON != 0);
	EnableAdmin(FALSE);

	if(mb_Open)
		GetDlgItem(IDEXIT)->ShowWindow(SW_SHOW);
	else
		GetDlgItem(IDEXIT)->ShowWindow(SW_HIDE);

	/* Limit length of texts */
	((CEdit *) GetDlgItem(IDC_EDIT_PATH))->LimitText(L_MAX_PATH - 1);
	((CEdit *) GetDlgItem(IDC_EDIT_NAME))->LimitText(BIG_C_MaxLenUserName - 1);

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp[100];
	char	asz_Temp1[100];
	CString o_String;
	int		i_Val;
	/*~~~~~~~~~~~~~~~~~~~*/

	CDialog::DoDataExchange(pDX);

	/* Keys */
	if(!pDX->m_bSaveAndValidate)
	{
		sprintf(asz_Temp, "%x", BIG_uc_GetKeyID(asz_Temp1));
		o_String = asz_Temp;
		DDX_Text(pDX, IDC_EDITID, o_String);

		sprintf(asz_Temp, "%x", BIG_ul_GetKeyValue(asz_Temp1));
		o_String = asz_Temp;
		DDX_Text(pDX, IDC_EDITVALUE, o_String);
	}
	else
	{
		DDX_Text(pDX, IDC_EDITID, o_String);
		sscanf(o_String, "%x", &i_Val);
		BIG_SetKeyID(i_Val);

		DDX_Text(pDX, IDC_EDITVALUE, o_String);
		sscanf(o_String, "%x", &i_Val);
		BIG_SetKeyValue(i_Val);
	}

	DDX_Check(pDX, IDC_CHECK, mi_LinkControlON);
	o_String = masz_LinkControlFileREF;
	DDX_Text(pDX, IDC_EDIT_PATH, o_String);
	DDX_Text(pDX, IDC_EDIT_NAME, mo_UserName);
}

/*
 =======================================================================================================================
    Aim:    Determin if ref and vss files are correct.
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::CanSetUserName(void)
{
	/* No link control on ! So no need to check */
	if(!mi_LinkControlON)
	{
		mb_CanSetUserName = FALSE;
		return;
	}

	/* Check if there's a link project */
	if(!*masz_LinkControlFileREF) return;

	mb_CanSetUserName = TRUE;

	/* Check if vss file is present */
	if(!*masz_LinkControlFileVSS)
	{
		L_strcpy(masz_LinkControlFileVSS, masz_LinkControlFileREF);
		L_strcat(masz_LinkControlFileVSS, EDI_Csz_ExtVss);
	}

	if(L_access(masz_LinkControlFileVSS, 0))
	{
		ERR_X_ForceError(ERI_ERR_Csz_NoSharedFile, NULL);
		*masz_LinkControlFileVSS = 0;
		mb_CanSetUserName = FALSE;
		return;
	}

	/* Try to open the file to be sure it's correct */
	BIG_gb_CanOpenFats = FALSE;
	COPY_BIG();
_Try_
	BIG_Open(masz_LinkControlFileREF);
	ERR_X_Error(L_strcmpi(BIG_gst.asz_Name, BIG_gst1.asz_Name), EDI_ERR_Csz_SameFile, NULL);
	BIG_Close();
_Catch_
	if(BIG_Handle()) BIG_Close();
	mb_CanSetUserName = FALSE;
_End_
	RESTORE_BIG();
	BIG_gb_CanOpenFats = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::EnableContent(BOOL _b_Enable)
{
	((CStatic *) GetDlgItem(IDC_STATIC5))->EnableWindow(_b_Enable);
	((CEdit *) GetDlgItem(IDC_EDIT_PATH))->EnableWindow(_b_Enable);

	if(mb_CanSetUserName == FALSE)
	{
		((CStatic *) GetDlgItem(IDC_STATIC6))->EnableWindow(FALSE);
		((CEdit *) GetDlgItem(IDC_EDIT_NAME))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDPASSWORD))->EnableWindow(FALSE);
	}
	else
	{
		((CStatic *) GetDlgItem(IDC_STATIC6))->EnableWindow(_b_Enable);
		((CEdit *) GetDlgItem(IDC_EDIT_NAME))->EnableWindow(_b_Enable);
		((CButton *) GetDlgItem(IDPASSWORD))->EnableWindow(_b_Enable);
	}
	((CButton *) GetDlgItem(IDBROWSE))->EnableWindow(_b_Enable);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::EnableAdmin(BOOL _b_Enable)
{
	/*~~~~~~~~~~~~~~~~~*/
	CRect		o_Rect;
	CListBox	*po_List;
	/*~~~~~~~~~~~~~~~~~*/

	/* Change dialog size to display administrator infos */
	GetWindowRect(&o_Rect);
	if(_b_Enable)
	{
		o_Rect.bottom += EDI_C_ExcedentSize;
		GetDlgItem(IDADMIN)->EnableWindow(FALSE);
	}
	else
		o_Rect.bottom -= EDI_C_ExcedentSize;

	if(_b_Enable == FALSE) GetDlgItem(IDADMIN)->EnableWindow(TRUE);

	/* Move window. Force repainting parent too */
	MoveWindow(&o_Rect, FALSE);
	GetDesktopWindow()->RedrawWindow
		(
			NULL,
			NULL,
			RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN
		);

	/* Disable/Enable all admin controls */
	((CButton *) GetDlgItem(IDADDUSER))->EnableWindow(_b_Enable);
	((CButton *) GetDlgItem(IDDELETEUSER))->EnableWindow(_b_Enable);

	po_List = (CListBox *) GetDlgItem(IDC_LIST_USERS);
	po_List->EnableWindow(_b_Enable);

	po_List = (CListBox *) GetDlgItem(IDC_LIST_LOCK);
	po_List->EnableWindow(_b_Enable);

	if(_b_Enable) RefreshLockList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_LinkControlDialog::b_CheckPath(char *_psz_PathName)
{
	/* Verify that selected folder exists */
	if(mi_LinkControlON)
	{
		if(L_access(_psz_PathName, 0))
		{
			ERR_X_ForceError(EDI_ERR_Csz_UnknowPath, _psz_PathName);
			return FALSE;
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_LinkControlDialog::b_CheckUserName(char *_psz_Name)
{
	/* Check user name */
	if(L_strlen(_psz_Name) == 0)
	{
		ERR_X_ForceError(EDI_ERR_Csz_BadUserName, NULL);
		return FALSE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_LinkControlDialog::b_GetUser(char *_psz_UserName, char *_psz_PassWord, int *_pi_Write, BOOL _b_Cur)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char		*pc_Buffer;
	ULONG		ul_Size;
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~~~*/

	/* Open VSS file. Open current vss if _b_Cur, else open linked one */
	if(_b_Cur)
	{
		COPY_BIG();
		BIG_Open(masz_CurrentVSSFile);
	}
	else
	{
		M_MF()->VssFileChanged();
		SWAP_BIG();
	}

	/* Get user and password if file exists */
	if((ul_Index = BIG_ul_SearchFileExt(EDI_Csz_VSSPath, _psz_UserName)) == BIG_C_InvalidIndex)
	{
		if(_b_Cur)
		{
			BIG_Close();
			RESTORE_BIG();
		}
		else
		{
			SWAP_BIG();
			M_MF()->CloseVssFile();
		}
		return FALSE;
	}

	/* Read password */
	pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), &ul_Size);
	L_strcpy(_psz_PassWord, pc_Buffer);

	/* Right */
	if(_pi_Write)
	{
		pc_Buffer += L_strlen(pc_Buffer) + 1;
		*_pi_Write = *(int *) pc_Buffer;
	}

	/* Close VSS bigfile */
	if(_b_Cur)
	{
		BIG_Close();
		RESTORE_BIG();
	}
	else
	{
		SWAP_BIG();
		M_MF()->CloseVssFile();
	}

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Check if current user name if registered in share vss file, and if password is correct.
 =======================================================================================================================
 */
BOOL EDIA_cl_LinkControlDialog::b_CheckCurrentUser(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				asz_PassWord[EDI_C_MaxLenPassWord];
	EDIA_cl_NameDialog	o_Dialog(EDI_STR_Csz_TitleEnterPassword, EDI_C_MaxLenPassWord - 1);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Determin if the user is registered */
	if(b_GetUser((char *) (LPCSTR) mo_UserName, asz_PassWord, &mi_CanWrite, FALSE) == FALSE)
	{
		ERR_X_ForceError(EDI_ERR_Csz_BadUserName, (char *) (LPCSTR) mo_UserName);
		return FALSE;
	}

	/* If bad password, ask to enter a new one */
	/*
	if(L_strcmpi(asz_PassWord, (char *) (LPCSTR) mo_UserPassWord))
	{
		if(o_Dialog.DoModal() == IDCANCEL) return FALSE;

		mo_UserPassWord = o_Dialog.mo_Name;
		if(L_strcmpi(asz_PassWord, (char *) (LPCSTR) mo_UserPassWord))
		{
			ERR_X_ForceError(EDI_ERR_Csz_BadPassWord, (char *) (LPCSTR) mo_UserName);
			return FALSE;
		}
	}
	*/

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Check, uncheck link control status.
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnCheck(void)
{
	if(((CButton *) GetDlgItem(IDC_CHECK))->GetCheck())
	{
		mi_LinkControlON = 1;
		CanSetUserName();
		EnableContent(TRUE);
	}
	else
	{
		mi_LinkControlON = 0;
		CanSetUserName();
		EnableContent(FALSE);
	}
}

/*
 =======================================================================================================================
    Aim:    To browser for a link control file.
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnBrowse(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_Dialog("Choose File", 0, FALSE);
	CString				o_Temp, o_RefFile;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_Dialog.DoModal() == IDOK)
	{
		o_Dialog.GetItem(o_Dialog.mo_File, 1, o_Temp);
		if(BIG_b_CheckName((char *) (LPCSTR) o_Temp))
		{
			/* Save access path to link control file */
			o_RefFile = o_Dialog.masz_FullPath;

			/* Display file */
			((CEdit *) GetDlgItem(IDC_EDIT_PATH))->SetWindowText(CString(o_Dialog.masz_FullPath) + CString("\\") + o_Temp);

			/* Compute project name with link control path */
			L_strcpy(masz_LinkControlFileREF, (char *) (LPCSTR) o_RefFile);
			if(masz_LinkControlFileREF[L_strlen(masz_LinkControlFileREF) - 1] != '\\')
				L_strcat(masz_LinkControlFileREF, "\\");
			L_strcat(masz_LinkControlFileREF, (char *) (LPCSTR) o_Temp);

			/* Add extension for files */
			L_strcpy(masz_LinkControlFileVSS, masz_LinkControlFileREF);
			L_strcat(masz_LinkControlFileVSS, EDI_Csz_ExtVss);

			CanSetUserName();
			EnableContent(mi_LinkControlON != 0);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    To change the password associated with the current user name, and with the current linked file. Be careful
            cause this will change the user password in the linked vss file, and not in the vss file associated with
            current project (if no exists).
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnPassWord(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_LinkControlPassWord o_Dialog;
	char						asz_PassWord[EDI_C_MaxLenPassWord];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get current user name, and check it */
	((CEdit *) GetDlgItem(IDC_EDIT_NAME))->GetWindowText(mo_UserName);
	if(b_GetUser((char *) (LPCSTR) mo_UserName, asz_PassWord, NULL, FALSE) == FALSE)
	{
		ERR_X_ForceError(EDI_ERR_Csz_BadUserName, (char *) (LPCSTR) mo_UserName);
		return;
	}

	/* Copy password to current one */
	mo_UserPassWord = asz_PassWord;

	/* Change the password */
	o_Dialog.mo_Title = EDI_STR_Csz_TitleChangePass;
	o_Dialog.mo_CurrentPassWord = mo_UserPassWord;
	if(o_Dialog.DoModal() == IDOK)
	{
		/* Update new password */
		mo_UserPassWord = o_Dialog.mo_PassWord;

		/* Update user */
		DeleteUser((char *) (LPCSTR) mo_UserName, FALSE);
		b_AddUser((char *) (LPCSTR) mo_UserName, (char *) (LPCSTR) mo_UserPassWord, NULL, FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnOK(void)
{
	/* Update attributes */
	UpdateData();

	/* No more process if linkcontrol is disabled */
	if(mi_LinkControlON)
	{
		L_strcpy(M_MF()->mst_Ini.asz_CurrentVSSFile, masz_LinkControlFileVSS);
		L_strcpy(M_MF()->mst_Ini.asz_CurrentRefFile, masz_LinkControlFileREF);

		/* Check for linked file */
		if(*masz_LinkControlFileREF == 0)
		{
			ERR_X_ForceError(EDI_ERR_Csz_SpecifyLink, NULL);
			return;
		}

		/* If TRUE, linkcontrol is ok, so check for a valid user */
		CanSetUserName();
		if(!mb_CanSetUserName) return;
		if(b_CheckCurrentUser() == FALSE) return;
	}

	CDialog::OnOK();
}

/*
 =======================================================================================================================
    Aim:    Inhibit cancel.
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnCancel(void)
{
}

/*$4
 ***********************************************************************************************************************
    ADMIN
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_LinkControlDialog::b_CreateIfNotExists(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_LinkControlPassWord o_Dialog(FALSE);
	char						asz_PassWord[EDI_C_MaxLenPassWord];
	EDIA_cl_NameDialog			o_DialogPass(EDI_STR_Csz_TitleEnterPassword, EDI_C_MaxLenPassWord - 1, TRUE);
	CString						o_Pass;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* If vss file is here, request to admin password */
	if(masz_CurrentVSSFile && !L_access(masz_CurrentVSSFile, 0))
	{
		/* Get admin password */
		if(b_GetUser(EDI_Csz_AdminUserName, asz_PassWord, NULL, TRUE) == FALSE)
		{
			ERR_X_ForceError(EDI_ERR_Csz_BadUserName, EDI_Csz_AdminUserName);
			return FALSE;
		}

		/* Ask to verify password */
		if(o_DialogPass.DoModal() == IDCANCEL) return FALSE;

		o_Pass = o_DialogPass.mo_Name;
		if(L_strcmpi(asz_PassWord, (char *) (LPCSTR) o_Pass))
		{
			ERR_X_ForceError(EDI_ERR_Csz_BadPassWord, EDI_Csz_AdminUserName);
			return FALSE;
		}

		return TRUE;
	}

	/* Set a new admin password to project */
	o_Dialog.mo_Title = EDI_STR_Csz_TitleEnterAdminPass;
	if(o_Dialog.DoModal() == IDCANCEL) return FALSE;
	o_Pass = o_Dialog.mo_PassWord;

	/* Create the empty bigfile */
	COPY_BIG();
_Try_
	BIG_CreateEmpty(masz_CurrentVSSFile);
_Catch_
	L_unlink(masz_CurrentVSSFile);
_End_
	RESTORE_BIG();

	/* Create administrator user */
	b_AddUser(EDI_Csz_AdminUserName, (char *) (LPCSTR) o_Pass, NULL, TRUE);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnKillRights(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	BIG_INDEX	ul_NextIndex;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	COPY_BIG();
_Try_
	BIG_Open(masz_CurrentVSSFile);
	ul_Index = BIG_FirstFile(0);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		ul_NextIndex = BIG_NextFile(ul_Index);
		BIG_DeleteFile(0, ul_Index);
		ul_Index = ul_NextIndex;
	}

	BIG_Close();
_Catch_
	if(BIG_Handle()) BIG_Close();
_End_
	RESTORE_BIG();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_LinkControlDialog::b_AddUser(char *_psz_UserName, char *_psz_PassWord, int *_pi_Write, BOOL _b_Cur)
{
	/*~~~~~~~~~~~~~~~*/
	CButton *po_Check;
	int		i_CanWrite;
	/*~~~~~~~~~~~~~~~*/

	/* Open VSS file */
	COPY_BIG();
_Try_
	if(_b_Cur)
		BIG_Open(masz_CurrentVSSFile);
	else
		BIG_Open(masz_LinkControlFileVSS);

	/* Search if the user name is already present */
	if(BIG_ul_SearchFileExt(EDI_Csz_VSSPath, _psz_UserName) != BIG_C_InvalidIndex)
	{
		ERR_X_ForceError(EDI_ERR_Csz_UserAlreadyPresent, _psz_UserName);
		BIG_Close();
		RESTORE_BIG();
		return FALSE;
	}

	/* Create a dir to receive that password (in VSS bigfile) */
	BIG_ul_CreateDir(EDI_Csz_VSSPath);

	SAV_Begin(EDI_Csz_VSSPath, _psz_UserName);
	SAV_Buffer((UCHAR *) _psz_PassWord, L_strlen(_psz_PassWord) + 1);

	/* Read only ? */
	po_Check = (CButton *) GetDlgItem(IDC_CHECKR);
	i_CanWrite = 1;
	if(po_Check->GetCheck()) i_CanWrite = 0;
	SAV_Buffer(&i_CanWrite, 4);
	if(_pi_Write) *_pi_Write = i_CanWrite;

	SAV_ul_End();

	/* Close VSS bigfile */
	BIG_Close();
_Catch_
	if(BIG_Handle()) BIG_Close();
_End_
	RESTORE_BIG();

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::DeleteUser(char *_psz_UserName, BOOL _b_Cur)
{
	/* Open VSS file */
	COPY_BIG();
_Try_
	if(_b_Cur)
		BIG_Open(masz_CurrentVSSFile);
	else
		BIG_Open(masz_LinkControlFileVSS);

	/* Delete user if file exists */
	if(BIG_ul_SearchFileExt(EDI_Csz_VSSPath, _psz_UserName) != BIG_C_InvalidIndex)
		BIG_DelFile(EDI_Csz_VSSPath, _psz_UserName);

	/* Close VSS bigfile */
	BIG_Close();
_Catch_
	if(BIG_Handle()) BIG_Close();
_End_
	RESTORE_BIG();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnShare(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		ul_Dir, ul_File;
	CListBox	*po_List;
	char		*pc_Buffer;
	ULONG		ul_Size;
	char		asz_PassWord[EDI_C_MaxLenPassWord];
	int			i_CanWrite;
	char		asz_User[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Create vss file, check password etc... */
	if(!b_CreateIfNotExists()) return;

	/* Enable all admin controls */
	EnableAdmin(TRUE);

	/* Open VSS file to display all users */
_Try_
	COPY_BIG();
	BIG_Open(masz_CurrentVSSFile);

	ul_Dir = BIG_ul_SearchDir(EDI_Csz_VSSPath);
	if(ul_Dir != BIG_C_InvalidIndex)
	{
		po_List = (CListBox *) GetDlgItem(IDC_LIST_USERS);
		ul_File = BIG_FirstFile(ul_Dir);
		while(ul_File != BIG_C_InvalidIndex)
		{
			/* Read password */
			pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(ul_File), &ul_Size);
			L_strcpy(asz_PassWord, pc_Buffer);

			/* Can write */
			pc_Buffer += L_strlen(pc_Buffer) + 1;
			i_CanWrite = *(int *) pc_Buffer;
			pc_Buffer += 4;

			/* Add user in list */
			sprintf(asz_User, "%s (%s)", BIG_NameFile(ul_File), i_CanWrite ? "Can Write" : "Read Only");
			po_List->AddString(asz_User);
			ul_File = BIG_NextFile(ul_File);
		}
	}

	/* Close VSS bigfile */
	BIG_Close();
_Catch_
	if(BIG_Handle()) BIG_Close();
_End_
	RESTORE_BIG();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnAddUser(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EDI_STR_Csz_TitleEnterUserName, BIG_C_MaxLenUserName - 1);
	CListBox			*po_List;
	int					i_CanWrite;
	char				asz_User[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_Dialog.DoModal() == IDOK)
	{
		if(b_CheckUserName((char *) (LPCSTR) o_Dialog.mo_Name) == FALSE) return;
_Try_
		if(b_AddUser((char *) (LPCSTR) o_Dialog.mo_Name, "", &i_CanWrite, TRUE) == FALSE) _Return_(;);
_Catch_
_End_
		po_List = (CListBox *) GetDlgItem(IDC_LIST_USERS);
		sprintf(asz_User, "%s (%s)", (char *) (LPCSTR) o_Dialog.mo_Name, i_CanWrite ? "Can Write" : "Read Only");
		po_List->AddString(asz_User);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnDeleteUser(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_List;
	int			i_Sel;
	char		asz_String[512];
	char		asz_Message[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST_USERS);
	i_Sel = po_List->GetCurSel();
	if(i_Sel != LB_ERR)
	{
		/* Get user name */
		GetUserSelected(asz_String);

		/* Can delete admin */
		if(!L_strcmpi(asz_String, EDI_Csz_AdminUserName))
		{
			ERR_X_ForceError(EDI_ERR_Csz_CantDeleteAdmin, NULL);
			return;
		}

		/* Confirm deletion */
		L_strcpy(asz_Message, EDI_STR_Csz_ConfirmDeleteUser);
		L_strcat(asz_Message, asz_String);
		L_strcat(asz_Message, " ?");
		if(M_MF()->MessageBox(asz_Message, EDI_STR_Csz_TitleConfirm, MB_ICONQUESTION | MB_YESNO) == IDNO)
			return;

		/* Delete user */
		po_List->DeleteString(i_Sel);
		DeleteUser(asz_String, TRUE);

		/* Select next user */
		if(i_Sel >= po_List->GetCount()) i_Sel--;
		if(i_Sel >= 0) po_List->SetCurSel(i_Sel);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::RefreshLockList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_List;
	L_FILE		p_File;
	char		asz_NameW[L_MAX_PATH];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST_LOCK);
	po_List->ResetContent();

	/* Fill lock list with current lock users */
	M_MF()->ComputeLockedFileName(masz_CurrentVSSFile, asz_NameW);

	/* BigFile write lock */
	if(!L_access(asz_NameW, 0))
	{
		p_File = L_fopen(asz_NameW, L_fopen_RB);
		L_fseek(p_File, 1, L_SEEK_SET); /* Zap user count */
		L_fgets(asz_NameW, L_MAX_PATH, p_File);
		L_fclose(p_File);
		po_List->AddString(CString(asz_NameW) + CString(" (W)"));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnEraseLock(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_NameW[L_MAX_PATH];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(M_MF()->MessageBox(EDI_STR_Csz_ConfirmDeleteLock, EDI_STR_Csz_TitleConfirm, MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		/* Delete */
		M_MF()->ComputeLockedFileName(masz_CurrentVSSFile, asz_NameW);
		L_unlink(asz_NameW);

		/* Refresh list */
		RefreshLockList();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::GetUserSelected(char *_psz_User)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_List;
	int			i_Sel;
	char		*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST_USERS);
	i_Sel = po_List->GetCurSel();
	if(i_Sel == -1)
	{
		*_psz_User = 0;
		return;
	}

	po_List->GetText(i_Sel, _psz_User);
	psz_Temp = L_strchr(_psz_User, '(');
	if(psz_Temp)
	{
		*psz_Temp = 0;
		psz_Temp--;
		while(L_isspace(*psz_Temp))
		{
			*psz_Temp = 0;
			psz_Temp--;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnCheckR(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_List;
	char		asz_Name[512];
	char		asz_Password[512];
	int			i_CanWrite;
	int			i_Sel;
	char		asz_User[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST_USERS);
	i_Sel = po_List->GetCurSel();
	if(i_Sel == -1) return;

	GetUserSelected(asz_Name);

	po_List->DeleteString(i_Sel);

	b_GetUser(asz_Name, asz_Password, NULL, TRUE);
	DeleteUser(asz_Name, TRUE);
	b_AddUser(asz_Name, asz_Password, &i_CanWrite, TRUE);

	sprintf(asz_User, "%s (%s)", asz_Name, i_CanWrite ? "Can Write" : "Read Only");
	i_Sel = po_List->AddString(asz_User);
	po_List->SetCurSel(i_Sel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LinkControlDialog::OnExit(void)
{
	TerminateProcess(GetCurrentProcess(), 0);
}

#endif /* ACTIVE_EDITORS */

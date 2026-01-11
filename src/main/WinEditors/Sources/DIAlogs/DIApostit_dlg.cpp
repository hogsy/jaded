/*$T DIApostit_dlg.cpp GC 1.134 10/15/04 12:08:21 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIApostit_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "EDIpaths.h"
#include "CCheckList.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "LINKs/LINKtoed.h"

/*$2------------------------------------------------------------------------------------------------------------------*/

BEGIN_MESSAGE_MAP(EDIA_cl_PostItDialog, EDIA_cl_BaseDialog)
	ON_COMMAND(IDKILLREAD, OnKillRead)
	ON_COMMAND(IDC_BUTTON_TO, OnTo)
	ON_COMMAND(IDPICKPOS, OnPickPos)
	ON_COMMAND(IDZOOM, OnZoom)
	ON_COMMAND(IDC_CHECK_PUBLIC, OnPublic)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_PostItDialog::EDIA_cl_PostItDialog(WOR_tdst_World *pst_World, F3D_cl_View *po_View) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_POSTIT)
{
	L_memset(&mst_PostIt, 0, sizeof(mst_PostIt));
	mpt_Org = NULL;
	mpst_World = pst_World;
	mpo_View = po_View;
	mb_ReadOnly = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_PostItDialog::OnInitDialog(void)
{
	EDIA_cl_BaseDialog::OnInitDialog();
	GetDlgItem(IDC_EDIT_NAME)->SetFocus();
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_PostItDialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CString		o_Name;
	char		az_Time[64];
	struct L_tm *newtime;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(pDX->m_bSaveAndValidate) return;

	/* Nom */
	if(mst_PostIt.az_Name[0])
	{
		o_Name = mst_PostIt.az_Name;
		char* str = const_cast<char*>(L_strrchr(o_Name, '.'));
		*str = 0;
		GetDlgItem(IDC_EDIT_NAME)->SetWindowText(o_Name);
	}

	/* To */
	GetDlgItem(IDC_EDIT_TO)->SetWindowText(mst_PostIt.az_To);

	/* Read */
	GetDlgItem(IDC_STATIC_READ)->SetWindowText(mst_PostIt.az_Read);

	/* Public */
	if(mst_PostIt.ul_Flags & PIT_C_NoReadOnly)
		((CButton *) GetDlgItem(IDC_CHECK_PUBLIC))->SetCheck(TRUE);
	else
		((CButton *) GetDlgItem(IDC_CHECK_PUBLIC))->SetCheck(FALSE);

	/* Contenu */
	if(mst_PostIt.az_Note[0]) GetDlgItem(IDC_EDIT_CONTENT)->SetWindowText(mst_PostIt.az_Note);

	/* Owner */
	if(!mst_PostIt.az_Owner[0]) L_strcpy(mst_PostIt.az_Owner, M_MF()->mst_Ini.asz_CurrentUserName);
	GetDlgItem(IDC_STATIC_FROM)->SetWindowText(mst_PostIt.az_Owner);

	/* Date de creation */
	if(!mst_PostIt.aul_DateCreate[0] && !mst_PostIt.aul_DateCreate[1])
		L_time((time_t *) &mst_PostIt.aul_DateCreate[0]);

	newtime = L_localtime((time_t *) &mst_PostIt.aul_DateCreate[0]);
	L_strftime(az_Time, 255, "%m/%d/%y %H:%M", newtime);
	GetDlgItem(IDC_STATIC_DATE)->SetWindowText(az_Time);

	/* Lecture seule ? */
	if(L_strcmpi(mst_PostIt.az_Owner, M_MF()->mst_Ini.asz_CurrentUserName))
	{
		if(!(mst_PostIt.ul_Flags & PIT_C_NoReadOnly))
		{
			GetDlgItem(IDC_BUTTON_TO)->EnableWindow(FALSE);
			GetDlgItem(IDUPDATE)->EnableWindow(FALSE);
			GetDlgItem(IDPICKPOS)->EnableWindow(FALSE);
			((CEdit *) GetDlgItem(IDC_EDIT_CONTENT))->SetReadOnly(TRUE);
			((CEdit *) GetDlgItem(IDC_EDIT_TO))->SetReadOnly(TRUE);
			((CEdit *) GetDlgItem(IDC_EDIT_NAME))->SetReadOnly(TRUE);
			GetDlgItem(IDC_CHECK_PUBLIC)->EnableWindow(FALSE);
			mb_ReadOnly = TRUE;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_PostItDialog::PreTranslateMessage(MSG *pMsg)
{
	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_PostItDialog::OnKillRead(void)
{
	/* Plus de lecture */
	L_memset(mst_PostIt.az_Read, 0, sizeof(mst_PostIt.az_Read));
	L_time((time_t *) &mst_PostIt.aul_DateCreate[0]);
	GetDlgItem(IDC_STATIC_READ)->SetWindowText(mst_PostIt.az_Read);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_PostItDialog::OnCancel(void)
{
	OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_PostItDialog::OnOK(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString			o_Name, o_Content, o_Content1;
	char			az_Path[2048];
	FILE			*f;
	F3D_tdst_PostIt pit;
	int				cpt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Nom fichier */
	GetDlgItem(IDC_EDIT_NAME)->GetWindowText(o_Name);
	if(o_Name.IsEmpty())
	{
		M_MF()->MessageBox("Invalid PostIt name", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	/* Dir */
	mpo_View->PostItPath(az_Path, TRUE);

	/* Meme nom */
	o_Name += EDI_Csz_PostIt;
	if(mst_PostIt.az_Name[0] && L_strcmpi((char *) (LPCSTR) o_Name, mst_PostIt.az_Name))
	{
		o_Content = az_Path + CString("/");
		o_Content += o_Name;
		if(!L_access(o_Content, 0))
		{
			M_MF()->MessageBox("Name of PostIt already exists", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		/* Rename */
		o_Content1 = az_Path + CString("/");
		o_Content1 += mst_PostIt.az_Name;
		MoveFile(o_Content1, o_Content);
	}

	/* Date de creation */
	if(!mst_PostIt.az_Name[0]) L_time((time_t *) &mst_PostIt.aul_DateCreate[0]);
	L_strcpy(mst_PostIt.az_Name, (char *) (LPCSTR) o_Name);

	/* Contenu */
	GetDlgItem(IDC_EDIT_CONTENT)->GetWindowText(o_Content);
	strncpy(mst_PostIt.az_Note, (char *) (LPCSTR) o_Content, 1024);

	/* To */
	GetDlgItem(IDC_EDIT_TO)->GetWindowText(o_Content);
	strncpy(mst_PostIt.az_To, (char *) (LPCSTR) o_Content, 1024);

	/* Lecture existant, au cas ou il a déjà changé */
	L_strcat(az_Path, "/");
	L_strcat(az_Path, (char *) (LPCSTR) o_Name);
	cpt = 0;
	do
	{
		f = fopen(az_Path, "rb");
		cpt++;
	} while(!f && cpt < 100);
	if(f)
	{
		fread(&pit, sizeof(pit), 1, f);
		fclose(f);
		if(mb_ReadOnly) /* On conserve tout l'original */
			L_memcpy(&mst_PostIt, &pit, sizeof(pit));
		else			/* On récupert éventuellement les lectures */
			L_strcpy(mst_PostIt.az_Read, pit.az_Read);
	}

	/* Lecteur */
	if(!mpo_View->PostItFindName(mst_PostIt.az_Read, M_MF()->mst_Ini.asz_CurrentUserName))
	{
		L_strcat(mst_PostIt.az_Read, ";");
		L_strcat(mst_PostIt.az_Read, M_MF()->mst_Ini.asz_CurrentUserName);
	}

	/* Save */
	cpt = 0;
	do
	{
		f = fopen(az_Path, "wb");
		cpt++;
	} while(!f && cpt < 100);
	if(f)
	{
		fwrite(&mst_PostIt, sizeof(mst_PostIt), 1, f);
		fclose(f);
	}

	EDIA_cl_BaseDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_PostItDialog::OnPickPos(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	if(mpo_View->PickPoint3D(&v))
	{
		MATH_CopyVector(&mst_PostIt.st_Pos, &v);
		if(mpt_Org) MATH_CopyVector(&mpt_Org->st_Pos, &v);
		mpo_View->mst_WinHandles.pst_DisplayData->uc_DrawPostIt = 1;
		mpo_View->Invalidate();
		mpo_View->RedrawWindow();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_PostItDialog::OnPublic(void)
{
	/*~~~~~~*/
	int check;
	/*~~~~~~*/

	check = ((CButton *) GetDlgItem(IDC_CHECK_PUBLIC))->GetCheck();
	if(check)
		mst_PostIt.ul_Flags |= PIT_C_NoReadOnly;
	else
		mst_PostIt.ul_Flags &= ~PIT_C_NoReadOnly;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_PostItDialog::OnZoom(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	min, max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector(&min, &mst_PostIt.st_Pos);
	MATH_CopyVector(&max, &mst_PostIt.st_Pos);
	min.x -= 1;
	min.y -= 1;
	min.z -= 1;
	max.x += 1;
	max.y += 1;
	max.z += 1;
	mpo_View->ZoomOnObject(&mpo_View->mst_WinHandles.pst_DisplayData->st_Camera, &min, &max);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_PostItDialog::OnTo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					az_Path[1024];
	char					az_Line[1024];
	char					az_Categ[128];
	FILE					*f;
	char					*pz, *pz1;
	CList<CString, CString> o_LstCateg;
	CList<CString, CString> o_LstContent;
	EMEN_cl_SubMenu			o_SubMenu(FALSE);
	CPoint					o_Point;
	POSITION				pos, pos1;
	CString					categ, content;
	int						cpt;
	int						i_Res;
	CRect					rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_View->PostItPath(az_Path);
	*L_strrchr(az_Path, '\\') = 0;
	L_strcat(az_Path, "\\");
	L_strcat(az_Path, "PostItList.txt");
	f = fopen(az_Path, "rt");
	if(f)
	{
		while(fgets(az_Line, 1024, f))
		{
			pz = az_Line;
			while(*pz && *pz != ':') pz++;
			if(*pz == ':')
			{
				L_memcpy(az_Categ, az_Line, pz - az_Line);
				az_Categ[pz - az_Line] = 0;
				pz++;
				pz1 = pz;
				while(*pz1 && *pz1 != '\n') pz1++;
				*pz1 = 0;
				o_LstCateg.AddTail(CString(az_Categ));
				o_LstContent.AddTail(CString(pz));
			}
		}

		fclose(f);
	}

	GetDlgItem(IDC_BUTTON_TO)->GetWindowRect(&rect);
	o_Point.x = rect.right;
	o_Point.y = rect.top;
	pos = o_LstCateg.GetHeadPosition();
	pos1 = o_LstContent.GetHeadPosition();
	if(pos)
	{
		cpt = 1;
		M_MF()->InitPopupMenuAction(NULL, &o_SubMenu);
		while(pos)
		{
			categ = o_LstCateg.GetNext(pos);
			content = o_LstContent.GetNext(pos1);
			M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, cpt++, TRUE, (char *) (LPCSTR) categ);
		}

		i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Point, &o_SubMenu);
		if(i_Res && i_Res != -1)
		{
			content = o_LstContent.GetAt(o_LstContent.FindIndex(i_Res - 1));
			L_strcpy(mst_PostIt.az_To, (char *) (LPCSTR) content);
			GetDlgItem(IDC_EDIT_TO)->SetWindowText(mst_PostIt.az_To);
		}
	}
}
#endif /* ACTIVE_EDITORS */

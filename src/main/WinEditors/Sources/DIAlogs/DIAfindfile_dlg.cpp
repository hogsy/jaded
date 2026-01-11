/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Dialog to search a file or a folder in a browser hierarchy.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAfindfile_dlg.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "EDImainframe.h"
#include "EDIstrings.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDItors/Sources/BROwser/BROtreectrl.h"
#include "EDItors/Sources/BROwser/BROlistctrl.h"
#include "EDImsg.h"
#include "Res/Res.h"
#include "EDIpaths.h"
#include "LINKs/LINKmsg.h"

EDIA_cl_FindFileDialog  go_FindFileDialog;

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_FindFileDialog, EDIA_cl_BaseDialog)
    ON_COMMAND(IDFIND, OnFind)
    ON_COMMAND(IDFINDNEXT, OnFindNext)
    ON_COMMAND(IDFINDFILES, OnFindFiles)
END_MESSAGE_MAP()

/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_FindFileDialog::EDIA_cl_FindFileDialog(void) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_EBROFIND)
{
    mo_Name = "";
    mi_WholeWord = 0;
    mi_MatchCase = 0;
    mi_ForFolder = 1;
    mi_ForFile = 1;
    mul_Dir = BIG_C_InvalidIndex;
    mul_File = BIG_C_InvalidIndex;
    mb_FindDir = FALSE;
	mb_FindInFiles = FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_FindFileDialog::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);

	if(mb_FindInFiles)
	{
		GetDlgItem(IDFIND)->EnableWindow(FALSE);
		GetDlgItem(IDFINDNEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKFOLDER)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKFILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC1)->EnableWindow(FALSE);
		GetDlgItem(IDFINDFILES)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDFIND)->EnableWindow(TRUE);
		GetDlgItem(IDFINDNEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECKFOLDER)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECKFILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC1)->EnableWindow(TRUE);
		GetDlgItem(IDFINDFILES)->EnableWindow(FALSE);
	}

    DDX_Text(pDX, IDC_EDITNAME, mo_Name);
    DDX_Check(pDX, IDC_CHECKWORD, mi_WholeWord);
    DDX_Check(pDX, IDC_CHECKCASE, mi_MatchCase);
    DDX_Check(pDX, IDC_CHECKFOLDER, mi_ForFolder);
    DDX_Check(pDX, IDC_CHECKFILE, mi_ForFile);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_FindFileDialog::SearchInFile(char *az1, BIG_INDEX _h_File)
{
	char	*p_buf;
	ULONG	len;

	if(BIG_b_IsFileExtension(_h_File, EDI_Csz_ExtAIEditorFct)) goto ok;
	if(BIG_b_IsFileExtension(_h_File, EDI_Csz_ExtAIEditorVars)) goto ok;
	if(BIG_b_IsFileExtension(_h_File, EDI_Csz_ExtAIEditorFctLib)) goto ok;
	return;

ok:
	p_buf = (char *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(_h_File), &len);
	if (!p_buf) return;
	MatchWord1(az1, _h_File, p_buf, FALSE);
	L_free(p_buf);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_FindFileDialog::FindFile(BIG_INDEX _h_Dir)
{
	char		az1[4096];
	BIG_INDEX	h_File;

	LINK_gb_UseSecond = TRUE;
	LINK_gb_CanLog = FALSE;
	BIG_ComputeFullName(_h_Dir, az1);
	LINK_PrintStatusMsg(az1);
	LINK_gb_CanLog = TRUE;
	LINK_gb_UseSecond = FALSE;

	h_File = BIG_SubDir(_h_Dir);
	while(h_File != BIG_C_InvalidIndex)
	{
		FindFile(h_File);
		h_File = BIG_NextDir(h_File);
	}

	h_File = BIG_FirstFile(_h_Dir);
	while(h_File != BIG_C_InvalidIndex)
	{
		SearchInFile(az1, h_File);
		h_File = BIG_NextFile(h_File);
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_FindFileDialog::OnFindFiles(void)
{
	AfxGetApp()->DoWaitCursor(1);
	LINK_PrintStatusMsg("** Find in files BEGIN **");
    UpdateData();
	FindFile(BIG_ul_SearchDir(EDI_Csz_Path_Technical));
	LINK_PrintStatusMsg("** Find in files END **");
	AfxGetApp()->DoWaitCursor(-1);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_FindFileDialog::MatchWord1(char *az1, BIG_INDEX _h_File, char *_psz_Name, BOOL _b_ForDir)
{
	CString	o_Test;
	CString	o_Test1;
	int res;
	char *pz, *pzd, *pzd1;
	BOOL ok;
	int line;
	char az[2048];
	char az2[4096];

	o_Test = _psz_Name;
	o_Test1 = mo_Name;
	if(!mi_MatchCase)
	{
		o_Test.MakeUpper();
		o_Test1.MakeUpper();
	}

	if(mi_WholeWord)
	{
		if(!o_Test.Compare(o_Test1)) return TRUE;
		return FALSE;
	}

	ok = FALSE;
	line = 0;
	res = o_Test.Find(o_Test1);
	pz = (char *) (LPCSTR) o_Test;
	while(res != -1)
	{
		ok = TRUE;
		pzd = (char *) (LPCSTR) o_Test + res;
		while(pzd - pz)
		{
			if(*pz == '\n') line++;
			pz++;
		}

		pzd = (char *) (LPCSTR) o_Test + res;
		while(pzd != (char *) (LPCSTR) o_Test && *pzd != '\n') pzd--;
		if(*pzd == '\n') pzd++;

		pzd1 = pzd;
		while(*pzd1 && *pzd1 != '\n') pzd1++;
		L_memcpy(az2, pzd, pzd1 - pzd);
		az2[pzd1 - pzd] = 0;

		sprintf(az, "%s -- %s -- [%x], Line %d : %s", az1, BIG_NameFile(_h_File), BIG_FileKey(_h_File), line, az2);
		LINK_PrintStatusMsg(az);
		res = o_Test.Find(o_Test1, res + 1);
	}

	return ok;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_FindFileDialog::MatchWord(char *_psz_Name, BOOL _b_ForDir)
{
	CString	o_Test;
	CString	o_Test1;
	int res;

	o_Test = _psz_Name;
	o_Test1 = mo_Name;
	if(!mi_MatchCase)
	{
		o_Test.MakeUpper();
		o_Test1.MakeUpper();
	}

	if(mi_WholeWord)
	{
		if(!o_Test.Compare(o_Test1)) return TRUE;
		return FALSE;
	}

	res = o_Test.Find(o_Test1);
	if(res != -1) return TRUE;
	return FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_FindFileDialog::RecurseFind(BIG_INDEX _ul_Cur, int _i_Level)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Memo;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    mul_Dir = _ul_Cur;

    /* Select folder */
    while(mul_Dir != BIG_C_InvalidIndex)
    {
        /* Is the folder name correct ? */
        if(mi_ForFolder && mb_FindDir)
        {
            if(MatchWord(BIG_NameDir(mul_Dir), TRUE)) return TRUE;
        }

        mb_FindDir = FALSE;

        /* Search inside files */
        if(mi_ForFile)
        {
            while(mul_File != BIG_C_InvalidIndex)
            {
                if(MatchWord(BIG_NameFile(mul_File), FALSE)) return TRUE;
                mul_File = BIG_NextFile(mul_File);
            }
        }

        /* Now we can search for dir name */
        mb_FindDir = TRUE;

        /* Next dir. Try child */
        ul_Memo = mul_Dir;
        mul_Dir = BIG_SubDir(mul_Dir);
        if(mul_Dir != BIG_C_InvalidIndex)
        {
            mul_File = BIG_FirstFile(mul_Dir);
            if(RecurseFind(mul_Dir, _i_Level + 1)) return TRUE;
        }

        /* Next dir. Try brother */
rec:
        mul_Dir = BIG_NextDir(ul_Memo);
        if(mul_Dir == BIG_C_InvalidIndex)
        {
	        mul_Dir = BIG_ParentDir(ul_Memo);
			if(mul_Dir != BIG_C_InvalidIndex) 
			{
				ul_Memo = mul_Dir;
				goto rec;
			}
            mul_Dir = ul_Memo;
			if(!ul_Memo) return TRUE;
            break;
        }

        mul_File = BIG_FirstFile(mul_Dir);
        if((mul_Dir == BIG_C_InvalidIndex) && (_i_Level == 0))
        {
            /* If no brother, we search for brother of a valid parent */
            mul_Dir = BIG_ParentDir(mul_Dir);
            while(mul_Dir != BIG_C_InvalidIndex)
            {
                ul_Memo = BIG_NextDir(mul_Dir);
                if(ul_Memo != BIG_C_InvalidIndex)
                {
                    mul_Dir = ul_Memo;
                    mul_File = BIG_FirstFile(mul_Dir);
                    break;
                }

                mul_Dir = BIG_ParentDir(mul_Dir);
            }
        }
    }

    return FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_FindFileDialog::OnFind(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BOOL    b_Res;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    UpdateData();

    mul_Dir = BIG_Root();
    mul_File = BIG_FirstFile(mul_Dir);
    mb_FindDir = TRUE;

    b_Res = RecurseFind(mul_Dir, 0);
    if(!b_Res)
        M_MF()->MessageBox(EDI_STR_Csz_FindFailed, EDI_STR_Csz_Title, MB_ICONINFORMATION | MB_OK);

    /* Select dir and file */
    if(b_Res)
    {
        if(mb_FindDir)
            mul_File = BIG_C_InvalidIndex;
        mpo_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, mul_Dir, mul_File);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_FindFileDialog::FindNext(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BOOL    b_Res;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(mul_Dir == BIG_C_InvalidIndex)
    {
        mul_Dir = BIG_Root();
        mul_File = BIG_FirstFile(mul_Dir);
        mb_FindDir = TRUE;
    }
    else if(mb_FindDir)
    {
        mul_File = BIG_FirstFile(mul_Dir);
        mb_FindDir = FALSE;
    }
    else if(mul_File != BIG_C_InvalidIndex)
    {
        mul_File = BIG_NextFile(mul_File);
        mb_FindDir = FALSE;
    }

    b_Res = RecurseFind(mul_Dir, 0);
    if(!b_Res)
        M_MF()->MessageBox(EDI_STR_Csz_FindFailed, EDI_STR_Csz_Title, MB_ICONINFORMATION | MB_OK);

    /* Select dir and file */
    if(b_Res)
    {
        if(mb_FindDir)
            mul_File = BIG_C_InvalidIndex;
        mpo_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, mul_Dir, mul_File);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_FindFileDialog::OnFindNext(void)
{
    UpdateData();
    FindNext();
}
#endif /* ACTIVE_EDITORS */

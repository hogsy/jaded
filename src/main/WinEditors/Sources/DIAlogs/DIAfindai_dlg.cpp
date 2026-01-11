/*$T DIAfindai_dlg.cpp GC!1.71 01/12/00 14:04:12 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/* Aim: Dialog to search a file or a folder in a browser hierarchy. */
#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAfindai_dlg.h"
#include "EDImainframe.h"
#include "EDIstrings.h"
#include "EDItors/Sources/AIscript/AIframe.h"
#include "EDItors/Sources/AIscript/AIleftframe.h"
#include "EDItors/Sources/AIscript/EditView/AIview.h"
#include "EDItors/Sources/AIscript/EditView/AIleftview.h"
#include "EDImsg.h"
#include "Res/Res.h"
#include "BIGfiles/BIGfat.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_FindAIDialog, EDIA_cl_BaseDialog)
    ON_COMMAND(IDFIND, OnFind)
    ON_COMMAND(IDFINDNEXT, OnFindNext)
    ON_COMMAND(IDREPLACE, OnReplace)
    ON_COMMAND(IDREPLACEALL, OnReplaceAll)
    ON_COMMAND(IDCLOSE, OnClose)
    ON_COMMAND(IDCANCEL, OnCancel)
	ON_COMMAND(IDC_CHECK_ALLFCT, OnUpdateData)
	ON_COMMAND(IDC_CHECK_FCL, OnUpdateData)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_FindAIDialog::EDIA_cl_FindAIDialog(void) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_EAIFIND)
{
    mo_Name = "";
    mi_WholeWord = 0;
    mi_MatchCase = 0;
	mi_InFcl = 0;
	mb_Replace = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_FindAIDialog::OnInitDialog(void)
{
	UpdateData(FALSE);
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FindAIDialog::OnUpdateData(void)
{
	UpdateData();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FindAIDialog::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_EDITNAME, mo_Name);
    DDX_Text(pDX, IDC_EDITNAMER, mo_NameR);
    DDX_Check(pDX, IDC_CHECKWORD, mi_WholeWord);
    DDX_Check(pDX, IDC_CHECKCASE, mi_MatchCase);
    DDX_Check(pDX, IDC_CHECK_ALLFCT, mi_AllFct);
    DDX_Check(pDX, IDC_CHECK_FCL, mi_InFcl);

	if(!pDX->m_bSaveAndValidate)
	{
		GetDlgItem(IDC_EDITNAME)->SetFocus();
		((CEdit *) GetDlgItem(IDC_EDITNAME))->SetSel(0, -1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FindAIDialog::Find(BOOL _b_Find)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
    char        *psz_Name;
    DWORD       dwFlags;
    FINDTEXTEX  st_Find;
    CHARRANGE   cp, cp1;
    int         i_Line;
	int			iSel;
	int			i_Prev;
	BOOL		bcur;
	BOOL		ok;
	BIG_INDEX	file;
    /*~~~~~~~~~~~~~~~~~~~~~~*/

	ok = FALSE;
	bcur = FALSE;
	M_MF()->LockDisplay(mpo_AI);
search:
    psz_Name = (char *) (LPCSTR) mpo_AI->mo_SearchString;

    dwFlags = 0;
    if(mi_WholeWord) dwFlags |= FR_WHOLEWORD;
    if(mi_MatchCase) dwFlags |= FR_MATCHCASE;

    mpo_AI->mpo_Edit->GetSel(cp);
    if(_b_Find)
        st_Find.chrg.cpMin = 0;
    else
        st_Find.chrg.cpMin = cp.cpMax;
    st_Find.chrg.cpMax = mpo_AI->mpo_Edit->GetTextLength();
    st_Find.lpstrText = psz_Name;

    if(mpo_AI->mpo_Edit->FindText(dwFlags, &st_Find) != -1)
    {
        mpo_AI->mpo_Edit->SetSel(st_Find.chrgText);
        i_Line = mpo_AI->mpo_Edit->LineFromChar(st_Find.chrgText.cpMin);
        if(i_Line < mpo_AI->mpo_Edit->GetFirstVisibleLine() || i_Line > mpo_AI->mpo_Edit->l_LastVisibleLine())
            mpo_AI->mpo_Edit->LineScroll(i_Line - mpo_AI->mpo_Edit->GetFirstVisibleLine());

		if(mb_Replace) 
		{
			mpo_AI->mpo_Edit->GetSel(cp1);
			mpo_AI->mpo_Edit->ReplaceSel(mo_NameR);
	        mpo_AI->mpo_Edit->SetSel(cp1.cpMin, cp1.cpMin + mo_NameR.GetLength());
			_b_Find = FALSE;
			ok = TRUE;
			goto search;
		}
    }
	else
	{
		if(mi_AllFct)
		{
next:
			iSel = mpo_AI->mpo_ListAI->GetNextItem(-1, LVNI_SELECTED) + 1;
			if(iSel >= mpo_AI->mpo_ListAI->GetItemCount()) 
			{
				M_MF()->UnlockDisplay(mpo_AI);
				if(M_MF()->MessageBox("Search at the first function ?", "Please confirm", MB_YESNO) == IDYES)
				{
					M_MF()->LockDisplay(mpo_AI);
					iSel = 0;
				}
				else
					return;
			}

			if(bcur && iSel == i_Prev)
			{
				M_MF()->UnlockDisplay(mpo_AI);
				ERR_X_ForceError("No occurence found", NULL);
				return;
			}

			if(!bcur) 
			{
				i_Prev = iSel;
				bcur = TRUE;
			}

			file = mpo_AI->mpo_ListAI->GetItemData(iSel);
			mpo_AI->mpo_ListAI->SetItemState(iSel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			mpo_AI->mpo_ListAI->SelectItem(iSel);
			mpo_AI->mpo_Edit->SetSel(0, 0);
			if(!L_strcmpi(L_strrchr(BIG_NameFile(file), '.'), ".fcl") && !mi_InFcl) goto next;
			goto search;
		}
			
		M_MF()->UnlockDisplay(mpo_AI);
		if(!ok) ERR_X_ForceError("No occurence found", NULL);
	}

	M_MF()->UnlockDisplay(mpo_AI);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_FindAIDialog::PreTranslateMessage(MSG *pMsg)
{
    if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
    {
		UpdateData();
        OnFind();
        OnClose();
        return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FindAIDialog::OnFind(void)
{
    UpdateData();
    mpo_AI->mo_SearchString = mo_Name;

    Find(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FindAIDialog::OnFindNext(void)
{
    UpdateData();
    mpo_AI->mo_SearchString = mo_Name;
    Find(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FindAIDialog::OnReplace(void)
{
	CHARRANGE cp1;

    UpdateData();
    mpo_AI->mo_SearchString = mo_Name;

	mpo_AI->mpo_Edit->GetSel(cp1);
	mpo_AI->mpo_Edit->ReplaceSel(mo_NameR);
	mpo_AI->mpo_Edit->SetSel(cp1.cpMin, cp1.cpMin + mo_NameR.GetLength());
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FindAIDialog::OnReplaceAll(void)
{
    UpdateData();
    mpo_AI->mo_SearchString = mo_Name;
	mb_Replace = TRUE;
    Find(TRUE);
	mb_Replace = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FindAIDialog::OnClose(void)
{
    mpo_AI->mpo_Edit->SetFocus();
    ShowWindow(SW_HIDE);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_FindAIDialog::OnCancel(void)
{
    mpo_AI->mpo_Edit->SetFocus();
    ShowWindow(SW_HIDE);
}

#endif /* ACTIVE_EDITORS */

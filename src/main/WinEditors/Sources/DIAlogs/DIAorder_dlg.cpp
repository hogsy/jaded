/*$T DIAorder_dlg.cpp GC 1.134 02/27/02 12:57:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAorder_dlg.h"
#include "EDImainframe.h"
#include "Res/Res.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "BIGfiles/BIGcheck.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGexport.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"

#define UP	80

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_OrderDialog, EDIA_cl_BaseDialog)
	ON_COMMAND(IDUP, OnUp)
	ON_COMMAND(IDDOWN, OnDown)
	ON_COMMAND(IDC_PLUS, OnPlus)
	ON_COMMAND(IDC_LESS, OnLess)
	ON_COMMAND(IDC_SETCONTENT, OnSetContent)
	ON_COMMAND(IDC_RESETCONTENT, OnResetContent)
	ON_COMMAND(IDC_INSERTCONTENT, OnInsertContent)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_OrderDialog::EDIA_cl_OrderDialog(char *_psz_Title, BOOL _b_LessPlus, BOOL _b_SetContent) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_ORDER)
{
	mpz_Title = _psz_Title;
	mpfn_CB = NULL;
	mb_LessPlus = _b_LessPlus;
	mb_SetContent = _b_SetContent;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_OrderDialog::~EDIA_cl_OrderDialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::OnSize(UINT, int x, int y)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(GetDlgItem(IDC_LIST2))
	{
		GetClientRect(&o_Rect);
		o_Rect.top += UP;
		o_Rect.InflateRect(-5, -5);
		GetDlgItem(IDC_LIST2)->MoveWindow(&o_Rect);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::AddItem(char *_psz_Str, ULONG _ul_Data)
{
	mo_StrList.AddTail(CString(_psz_Str));
	mo_DataList.AddTail(_ul_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::SetCallback(EDIA_tdpfnv_OrderCB _pfn_CB)
{
	mpfn_CB = _pfn_CB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_OrderDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~*/
	CRect		o_Rect;
	CListBox	*po_List;
	/*~~~~~~~~~~~*/

	CDialog::OnInitDialog();
	InitList();

	if(!mb_LessPlus)
	{
		GetDlgItem(IDC_PLUS)->EnableWindow(FALSE);
		GetDlgItem(IDC_LESS)->EnableWindow(FALSE);
	}

	if(!mb_SetContent)
	{
		GetDlgItem(IDC_SETCONTENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RESETCONTENT)->EnableWindow(FALSE);
	}

	GetClientRect(&o_Rect);
	o_Rect.top += UP;
	o_Rect.InflateRect(-5, -5);
	GetDlgItem(IDC_LIST2)->MoveWindow(&o_Rect);

	SetWindowText(mpz_Title);

	po_List = (CListBox *) GetDlgItem(IDC_LIST2);
	po_List->SetCurSel(0);

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::InitList(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	POSITION	pos, pos1;
	CListBox	*po_List;
	CString		o_Str;
	ULONG		ul_Data;
	ULONG		ul_Res;
	char		asz_N[100];
	int			iNum, top;
	/*~~~~~~~~~~~~~~~~~~~*/

	/* Take care of init list */
	po_List = (CListBox *) GetDlgItem(IDC_LIST2);
	top = po_List->GetTopIndex();
	po_List->SetRedraw(FALSE);
	po_List->ResetContent();
	pos = mo_StrList.GetHeadPosition();
	pos1 = mo_DataList.GetHeadPosition();
	iNum = 0;
	while(pos)
	{
		o_Str = mo_StrList.GetNext(pos);
		ul_Data = mo_DataList.GetNext(pos1);
		if(o_Str.IsEmpty())
			sprintf(asz_N, "%-4d <Hole>", iNum);
		else
			sprintf(asz_N, "%-4d %s", iNum, (char *) (LPCSTR) o_Str);
		ul_Res = po_List->AddString(asz_N);
		po_List->SetItemData(ul_Res, ul_Data);
		iNum++;
	}

	po_List->SetTopIndex(top);
	po_List->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::Swap(ULONG _ul1, ULONG _ul2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString		o_Str1, o_Str2;
	ULONG		ul_Data1, ul_Data2;
	POSITION	pos1, pos2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pos1 = mo_StrList.FindIndex(_ul1);
	o_Str1 = mo_StrList.GetAt(pos1);
	pos2 = mo_StrList.FindIndex(_ul2);
	o_Str2 = mo_StrList.GetAt(pos2);
	mo_StrList.InsertAfter(pos2, o_Str1);
	mo_StrList.InsertAfter(pos1, o_Str2);
	mo_StrList.RemoveAt(pos1);
	mo_StrList.RemoveAt(pos2);

	pos1 = mo_DataList.FindIndex(_ul1);
	ul_Data1 = mo_DataList.GetAt(pos1);
	pos2 = mo_DataList.FindIndex(_ul2);
	ul_Data2 = mo_DataList.GetAt(pos2);
	mo_DataList.InsertAfter(pos2, ul_Data1);
	mo_DataList.InsertAfter(pos1, ul_Data2);
	mo_DataList.RemoveAt(pos1);
	mo_DataList.RemoveAt(pos2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::OnUp(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	int			iSel;
	CListBox	*po_List;
	int			ai_Sel[1000];
	int			iNumSel;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST2);
	if(po_List->GetSelCount() == 0) return;
	if(po_List->GetSel(0)) return;

	iNumSel = po_List->GetSelItems(1000, ai_Sel);

	M_MF()->LockDisplay(po_List);
	for(iSel = 1; iSel < po_List->GetCount(); iSel++)
	{
		if(po_List->GetSel(iSel))
		{
			if(mpfn_CB) mpfn_CB(po_List->GetItemData(iSel), po_List->GetItemData(iSel - 1));
			Swap(iSel, iSel - 1);
			po_List->SetSel(iSel - 1, TRUE);
			po_List->SetSel(iSel, FALSE);
		}
	}

	InitList();

	for(iSel = 0; iSel < iNumSel; iSel++)
		if(ai_Sel[iSel]) po_List->SetSel(ai_Sel[iSel] - 1, TRUE);

	M_MF()->UnlockDisplay(po_List);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::OnDown(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	int			iSel;
	CListBox	*po_List;
	int			ai_Sel[1000];
	int			iNumSel;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST2);
	if(po_List->GetSelCount() == 0) return;
	if(po_List->GetSel(po_List->GetCount() - 1)) return;

	iNumSel = po_List->GetSelItems(1000, ai_Sel);

	M_MF()->LockDisplay(po_List);
	for(iSel = po_List->GetCount() - 2; iSel >= 0; iSel--)
	{
		if(po_List->GetSel(iSel))
		{
			if(mpfn_CB) mpfn_CB(po_List->GetItemData(iSel), po_List->GetItemData(iSel + 1));
			Swap(iSel, iSel + 1);
			po_List->SetSel(iSel + 1, TRUE);
			po_List->SetSel(iSel, FALSE);
		}
	}

	InitList();

	for(iSel = 0; iSel < iNumSel; iSel++)
		if(ai_Sel[iSel] != po_List->GetSelCount() - 1) po_List->SetSel(ai_Sel[iSel] + 1, TRUE);

	M_MF()->UnlockDisplay(po_List);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::OnPlus(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int			iSel;
	CListBox	*po_List;
	POSITION	pos;
	/*~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST2);
	iSel = po_List->GetCurSel();
	if(iSel == -1) return;
	pos = mo_StrList.FindIndex(iSel);
	if(!pos)
	{
		mo_StrList.AddHead("");
		mo_DataList.AddHead((ULONG) 0);
	}
	else
	{
		mo_StrList.InsertBefore(pos, "");
		pos = mo_DataList.FindIndex(iSel);
		mo_DataList.InsertBefore(pos, 0);
	}

	InitList();
	po_List->SetSel(iSel + 1, TRUE);
	po_List->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::OnLess(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int			iSel;
	CListBox	*po_List;
	POSITION	pos;
	CString		o_Str;
	int			max;
	/*~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST2);
	max = -1;
	for(iSel = po_List->GetCount() - 1; iSel >= 0; iSel--)
	{
		if(po_List->GetSel(iSel))
		{
			if(max == -1) 
				max = iSel;
			else
				max--;
			pos = mo_StrList.FindIndex(iSel);
			mo_StrList.RemoveAt(pos);
			pos = mo_DataList.FindIndex(iSel);
			mo_DataList.RemoveAt(pos);
		}
	}

	iSel = max;
	InitList();
	if(iSel >= po_List->GetCount()) iSel--;
	if(iSel >= 0) po_List->SetSel(iSel, TRUE);
	po_List->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::OnSetContent(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File1("Choose file", 3, TRUE, TRUE);
	int					iSel;
	CListBox			*po_List;
	int					ai_Sel[1000];
	int					iNumSel;
	POSITION			pos;
	CString				o_File, o_Temp;
	BIG_INDEX			ul_Index;
	char				*psz_Temp;
	int					i;
	BIG_INDEX			ul_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST2);
	if(po_List->GetSelCount() == 0) return;
	if(o_File1.DoModal() != IDOK) return;

	iNumSel = po_List->GetSelItems(1000, ai_Sel);
	M_MF()->LockDisplay(po_List);
	ul_Last = -1;

	for(i = 0, iSel = 0; iSel < po_List->GetCount(); iSel++)
	{
		if(po_List->GetSel(iSel))
		{
			if(i >= o_File1.mi_NumFiles)
			{
				if(ul_Last != -1)
				{
					pos = mo_StrList.FindIndex(iSel);
					mo_StrList.SetAt(pos, BIG_NameFile(ul_Index));
					pos = mo_DataList.FindIndex(iSel);
					mo_DataList.SetAt(pos, BIG_FileKey(ul_Index));
				}
				else
				{
					pos = mo_StrList.FindIndex(iSel);
					mo_StrList.SetAt(pos, "");
					pos = mo_DataList.FindIndex(iSel);
					mo_DataList.SetAt(pos, 0);
				}
			}
			else
			{
				o_File1.GetItem(o_File1.mo_File, i + 1, o_Temp);
				i++;
				psz_Temp = (char *) (LPCSTR) o_Temp;
				ul_Index = BIG_ul_SearchDir(o_File1.masz_FullPath);
				if(ul_Index == BIG_C_InvalidIndex) continue;
				ul_Index = BIG_ul_SearchFile(ul_Index, psz_Temp);
				if(ul_Index == BIG_C_InvalidIndex) continue;

				pos = mo_StrList.FindIndex(iSel);
				mo_StrList.SetAt(pos, BIG_NameFile(ul_Index));
				pos = mo_DataList.FindIndex(iSel);
				mo_DataList.SetAt(pos, BIG_FileKey(ul_Index));
				ul_Last = ul_Index;
			}
		}
	}

	InitList();

	for(iSel = 0; iSel < iNumSel; iSel++)
		if(ai_Sel[iSel] != po_List->GetSelCount() - 1) po_List->SetSel(ai_Sel[iSel], TRUE);

	M_MF()->UnlockDisplay(po_List);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::OnInsertContent(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*po_List;
	EDIA_cl_FileDialog	o_File1("Choose file", 3, TRUE, TRUE);
	CString				o_Temp;
	char				*psz_Temp;
	BIG_INDEX			ul_Index;
	int					iSel;
	POSITION			pos;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST2);

	if(o_File1.DoModal() != IDOK) return;

	iSel = po_List->GetCurSel();
	for(i = 1; i <= o_File1.mi_NumFiles; i++)
	{
		o_File1.GetItem(o_File1.mo_File, i, o_Temp);
		psz_Temp = (char *) (LPCSTR) o_Temp;
		ul_Index = BIG_ul_SearchDir(o_File1.masz_FullPath);
		if(ul_Index == BIG_C_InvalidIndex) continue;
		ul_Index = BIG_ul_SearchFile(ul_Index, psz_Temp);
		if(ul_Index == BIG_C_InvalidIndex) continue;

		if(iSel == -1)
		{
			mo_StrList.AddTail(BIG_NameFile(ul_Index));
			mo_DataList.AddTail(BIG_FileKey(ul_Index));
			iSel = po_List->GetCount() - 1;
		}
		else
		{
			pos = mo_StrList.FindIndex(iSel);
			mo_StrList.InsertAfter(pos, BIG_NameFile(ul_Index));
			pos = mo_DataList.FindIndex(iSel);
			mo_DataList.InsertAfter(pos, BIG_FileKey(ul_Index));
			iSel++;
		}
	}

	InitList();
	po_List->SetSel(iSel, TRUE);
	po_List->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_OrderDialog::OnResetContent(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	int			iSel;
	CListBox	*po_List;
	int			ai_Sel[1000];
	int			iNumSel;
	POSITION	pos;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST2);
	if(po_List->GetSelCount() == 0) return;

	iNumSel = po_List->GetSelItems(1000, ai_Sel);

	M_MF()->LockDisplay(po_List);
	for(iSel = po_List->GetCount() - 1; iSel >= 0; iSel--)
	{
		if(po_List->GetSel(iSel))
		{
			pos = mo_StrList.FindIndex(iSel);
			mo_StrList.SetAt(pos, "");
		}
	}

	InitList();

	for(iSel = 0; iSel < iNumSel; iSel++)
		if(ai_Sel[iSel] != po_List->GetSelCount() - 1) po_List->SetSel(ai_Sel[iSel], TRUE);

	M_MF()->UnlockDisplay(po_List);
}
#endif /* ACTIVE_EDITORS */

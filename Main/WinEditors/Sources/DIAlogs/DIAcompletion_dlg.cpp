/*$T DIAcompletion_dlg.cpp GC!1.71 01/19/00 09:46:32 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAcompletion_dlg.h"
#include "Res/Res.h"
#include "EDItors/Sources/AIscript/AIframe.h"
#include "EDItors/Sources/AIscript/EditView/AIview.h"
#include "EDImainframe.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CCompListBox::DrawItem(DRAWITEMSTRUCT *lpDrawItemStruct)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CDC     *pdc, dc;
    char    *psz_Temp, *psz_Temp1;
    char    asz_Buf[128];
    CRect   o_Rect, o_Rect1;
    CSize   o_Size;
    BOOL    bHasParam;
    char    cMem;
    int     iMem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(lpDrawItemStruct->itemID == -1) return;

    pdc = dc.FromHandle(lpDrawItemStruct->hDC);
    o_Rect = lpDrawItemStruct->rcItem;
    GetClientRect(&o_Rect1);
    o_Rect.right = o_Rect1.right;
    pdc->FillSolidRect(o_Rect, GetSysColor(COLOR_WINDOW));

    /* Separator ? */
    if(lpDrawItemStruct->itemData == 0)
    {
        o_Rect.left += 4;
        o_Rect.right -= 4;
        iMem = o_Rect.Height() / 2;
        o_Rect.top += iMem;
        o_Rect.bottom -= iMem - 1;
        pdc->Draw3dRect(o_Rect, GetSysColor(COLOR_ACTIVECAPTION), GetSysColor(COLOR_ACTIVECAPTION));
        return;
    }

    GetText(lpDrawItemStruct->itemID, asz_Buf);

    /* Selected item ? */
    if(lpDrawItemStruct->itemState & ODS_SELECTED)
    {
        pdc->SetBkColor(GetSysColor(COLOR_ACTIVECAPTION));
        pdc->SetTextColor(GetSysColor(COLOR_WINDOW));
        pdc->ExtTextOut(o_Rect.left, o_Rect.top, ETO_CLIPPED, o_Rect, asz_Buf, NULL);
    }
    else
    {
        pdc->SetBkColor(GetSysColor(COLOR_WINDOW));

        /* Function return value */
        psz_Temp = asz_Buf;
        psz_Temp = L_strchr(psz_Temp, ' ');
        if(psz_Temp)
        {
            psz_Temp1 = L_strchr(asz_Buf, '(');
            if(psz_Temp < psz_Temp1)
            {
                cMem = psz_Temp[1];
                psz_Temp[1] = 0;
                pdc->SetTextColor(0x00FF0000);
                pdc->ExtTextOut(o_Rect.left, o_Rect.top, ETO_CLIPPED, o_Rect, asz_Buf, NULL);
                o_Size = pdc->GetTextExtent(asz_Buf);
                o_Rect.left += o_Size.cx;
                psz_Temp[1] = cMem;
                L_strcpy(asz_Buf, psz_Temp + 1);
            }
        }

        /* Parameters ? */
        pdc->SetTextColor(0x00);
        bHasParam = FALSE;
        psz_Temp = L_strchr(asz_Buf, '(');
        if(psz_Temp && psz_Temp[1] != ')')
        {
            bHasParam = TRUE;
            *psz_Temp = 0;
        }

        /* Normal name */
        pdc->ExtTextOut(o_Rect.left, o_Rect.top, ETO_CLIPPED, o_Rect, asz_Buf, NULL);
        o_Size = pdc->GetTextExtent(asz_Buf);
        o_Rect.left += o_Size.cx;

        if(bHasParam)
        {
            pdc->ExtTextOut(o_Rect.left, o_Rect.top, ETO_CLIPPED, o_Rect, "(", NULL);
            o_Size = pdc->GetTextExtent("(");
            o_Rect.left += o_Size.cx;

            psz_Temp++;
            psz_Temp1 = L_strchr(psz_Temp, ')');
            *psz_Temp1 = 0;
            pdc->SetTextColor(0x00006F40);
            pdc->ExtTextOut(o_Rect.left, o_Rect.top, ETO_CLIPPED, o_Rect, psz_Temp, NULL);
            o_Size = pdc->GetTextExtent(psz_Temp);
            o_Rect.left += o_Size.cx;

            pdc->SetTextColor(0x00);
            pdc->ExtTextOut(o_Rect.left, o_Rect.top, ETO_CLIPPED, o_Rect, ")", NULL);
        }
    }

    pdc->DeleteTempMap();
}

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

extern CMapStringToString   go_PPConstants;
extern CMapStringToPtr      go_PPMacros;
extern CMapStringToPtr      go_PPFunctions;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_Completion, EDIA_cl_BaseDialog)
    ON_WM_SIZE()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_Completion::EDIA_cl_Completion(int what) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_COMPLETION)
{
    mpo_List = NULL;
    mo_Word.Empty();
	mpo_Frame = NULL;
	mi_What = what;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_Completion::~EDIA_cl_Completion(void)
{
    mpo_List->DestroyWindow();
    delete mpo_List;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_Completion::OnInitDialog(void)
{
    CDialog::OnInitDialog();
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Completion::DoDataExchange(CDataExchange *pDX)
{
    /*~~~~~~~~~~~~~~*/
    long    l_Sel;
    CPoint  o_Pos;
    int     cy;
	CString	o_Name, o_Val;
	CMapStringToPtr	Temp;
	POSITION pos;
    /*~~~~~~~~~~~~~~*/

    if(pDX->m_bSaveAndValidate) return;

    mpo_List = new CCompListBox;
    mpo_List->mpo_Frame = mpo_Frame;
    mpo_List->Create
        (
            LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWFIXED | WS_BORDER | WS_VISIBLE | WS_CHILD,
            CRect(0, 0, 0, 0),
            this,
            0
        );

    /* Force moving window at the right place */
	mpo_Frame->mpo_Edit->GetSel(l_Sel, l_Sel);
	o_Pos = mpo_Frame->mpo_Edit->GetCharPos(l_Sel);
	mpo_Frame->mpo_Edit->ClientToScreen(&o_Pos);

	cy = GetSystemMetrics(SM_CXSCREEN);
	if(o_Pos.x + mpo_Frame->mst_Ini.i_XCompletion + 30 >= cy)
		o_Pos.x = cy - (mpo_Frame->mst_Ini.i_XCompletion + 30);
	cy = GetSystemMetrics(SM_CYSCREEN);
	if(o_Pos.y + mpo_Frame->mst_Ini.i_YCompletion + 30 >= cy)
		o_Pos.y = cy - (mpo_Frame->mst_Ini.i_YCompletion + 30);

	MoveWindow(o_Pos.x, o_Pos.y, mpo_Frame->mst_Ini.i_XCompletion, mpo_Frame->mst_Ini.i_YCompletion);

	/* Get word at left */
	mo_Word = mpo_Frame->mpo_Edit->o_GetWordLeft(l_Sel);

    /* Fill list */
	if(mi_What == AI_C_Completion_All)
	{
		AddList(&mpo_Frame->mo_TypeList);
		AddList(&mpo_Frame->mo_KeywordList);
		AddList(&mpo_Frame->mo_FunctionList);
		AddList(&mpo_Frame->mo_FieldList);
	    AddList(&go_PPFunctions);
	}

	pos = go_PPConstants.GetStartPosition();
    while(pos)
    {
        go_PPConstants.GetNextAssoc(pos, o_Name, o_Val);
		Temp.SetAt(o_Name, (void *&) o_Val);
	}
	
    if(mi_What == AI_C_Completion_All || mi_What == AI_C_Completion_ProcList) 
	{
		AddList(&mpo_Frame->mo_Compiler.mo_ProcList);
	}

	if(mi_What == AI_C_Completion_All || mi_What == AI_C_Completion_Macro) 
	{
	    AddList(&go_PPMacros);
		AddList(&Temp);
	}

	if(mi_What == AI_C_Completion_All || mi_What == AI_C_Completion_Variable) 
	{
		AddList(&mpo_Frame->mo_Compiler.mst_LocalVars.o_Vars);
		AddList(&mpo_Frame->mo_Compiler.mst_GlobalVars.o_Vars);
	}

    /* Is there only one word ? Select it now, and don't display box */
    if(mpo_List->GetCount() == 1)
    {
        mpo_List->GetText(0, mo_Word);
        EndDialog(IDOK);
    }

    mpo_List->SetFont(&M_MF()->mo_Fnt);
    mpo_List->SetCurSel(0);
    mpo_List->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Completion::OnOK(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    int     i_Item;
    CRect   o_Rect;
    char    asz_Tmp[512];
    char    *psz_Tmp;
    /*~~~~~~~~~~~~~~~~~~~~~*/

    /* Save current box size */
    GetWindowRect(&o_Rect);
	mpo_Frame->mst_Ini.i_XCompletion = o_Rect.Width();
	mpo_Frame->mst_Ini.i_YCompletion = o_Rect.Height();

    i_Item = mpo_List->GetCurSel();
    if(i_Item == LB_ERR)
        EndDialog(IDCANCEL);
    else
    {
        mpo_List->GetText(i_Item, mo_Word);
        L_strcpy(asz_Tmp, (char *) (LPCSTR) mo_Word);
        psz_Tmp = asz_Tmp;
        while(L_isspace(*psz_Tmp)) psz_Tmp++;
        mo_Word = psz_Tmp;
        EDIA_cl_BaseDialog::OnOK();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Completion::OnCancel(void)
{
    /*~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~*/

    /* Save current box size */
    GetWindowRect(&o_Rect);
	mpo_Frame->mst_Ini.i_XCompletion = o_Rect.Width();
	mpo_Frame->mst_Ini.i_YCompletion = o_Rect.Height();

    EDIA_cl_BaseDialog::OnCancel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EDIA_cl_Completion_MakeFctName(CDC *pdc, int p, BOOL align)
{
	CString o_Ret, o_Word;
	CSize	o_size;
	char	*psz_Temp;

    /* Add return type (except void) */
    o_Ret = EAI_gast_Functions[p].psz_Return;
	o_Word = EAI_gast_Functions[p].psz_Name;
    o_size.cx = 0;
    if(!L_strcmpi((char *) (LPCSTR) o_Ret, "TYPE_VOID"))
    {
        o_Ret.Empty();
		if(align)
		{
			while(o_size.cx < 100)
			{
				o_Ret += " ";
				o_size = pdc->GetTextExtent(o_Ret);
			}
		}
		else
		{
			o_Ret = " ";
		}
    }
    else
    {
        o_Ret = o_Ret.Right(o_Ret.GetLength() - 5);
        o_Ret.MakeLower();
        o_size = pdc->GetTextExtent(o_Ret);
		if(align)
		{
			while(o_size.cx < 100)
			{
				o_Ret += " ";
				o_size = pdc->GetTextExtent(o_Ret);
			}
		}
		else
		{
			o_Ret += " ";
		}
    }

    o_Word = o_Ret + o_Word;

    /* Add parameters for functions */
    o_Word += "(";
    psz_Temp = EAI_gast_Functions[p].psz_Types;
    if(EAI_gast_Functions[p].i_NumPars) o_Word += psz_Temp;
    o_Word += ")";

	return o_Word;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_Completion::AddList(CMapStringToPtr *_po_List)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION                pos;
    CString                 o_Word, o_Word1, o_Test, o_Ret;
    int                     p;
    int                     iSel, i;
    int                     iNum;
    BOOL                    bFirst;
    CDC                     *pdc;
    CSize                   o_size;
    CList<CString, CString> o_Sorted;
    CList<int, int>         o_SortedInt;
	CStringArray			o_Arr;
	char					*pz;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Just to make test without case */
    o_Test = mo_Word;
    o_Test.MakeUpper();

	pos = _po_List->GetStartPosition();
    while(pos)
    {
        _po_List->GetNextAssoc(pos, o_Word, (void * &) p);
		o_Word1.Format("%s@%d", o_Word, p);
		o_Arr.Add(o_Word1);
	}

	SortStringArray(o_Arr, FALSE);

    iNum = 0;
    bFirst = TRUE;
    pdc = mpo_List->GetDC();
	for(i = 0; i < o_Arr.GetSize(); i++)
    {
		o_Word = o_Arr.GetAt(i);
		pz = L_strrchr((char *) (LPCSTR) o_Word, '@');
		p = L_atoi(pz + 1);
		o_Word = o_Word.Left(pz - (char *) (LPCSTR) o_Word);

        o_Word1 = o_Word;
        o_Word1.MakeUpper();
        if(o_Word1.Find(o_Test) >= 0)
        {
            if(_po_List == &mpo_Frame->mo_FunctionList)
            {
				o_Word = EDIA_cl_Completion_MakeFctName(pdc, p, TRUE);
            }

            /* Add separator */
            if(bFirst)
            {
                bFirst = FALSE;
                if(mpo_List->GetCount())
                {
                    iSel = mpo_List->AddString("");
                    mpo_List->SetItemData(iSel, 0);
                }
            }

            iSel = mpo_List->AddString(o_Word);
            mpo_List->SetItemData(iSel, (int) _po_List);
            iNum++;
        }
    }

    ReleaseDC(pdc);
    return iNum;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Completion::OnSize(UINT, int cx, int cy)
{
    /*~~~~~~~~~~~~~~~~~~~*/
    CButton *po_Button;
    /*~~~~~~~~~~~~~~~~~~~*/

    if(mpo_List) mpo_List->MoveWindow(0, 10, cx, cy - 30);

    po_Button = (CButton *) GetDlgItem(IDOK);
    if(po_Button) po_Button->MoveWindow(0, cy - 18, (cx / 2) - 1, 16);
    po_Button = (CButton *) GetDlgItem(IDCANCEL);
    if(po_Button) po_Button->MoveWindow(cx / 2, cy - 18, cx / 2, 16);
}

#endif /* ACTIVE_EDITORS */

/*$T RASframe.cpp GC! 1.081 04/11/00 09:47:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "RASframe.h"
#include "EDIapp.h"
#include "EDIstrings.h"
#include "ENGine/Sources/ENGinit.h"
#include "TIMer/PROfiler/PROdisplay.h"
#include "ENGine/Sources/ENGvars.h"
#define ACTION_GLOBAL
#include "RASframe_act.h"
#include "EDImainframe.h"
#include "LINKs/LINKtoed.h"
#include "TIMer/TIMdefs.h"
#include "DIAlogs\DIAname_dlg.h"
#include "DIAlogs/CCheckList.h"
#include "AIinterp/Sources/AIengine.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(ERAS_cl_InsideScroll, CScrollView)
BEGIN_MESSAGE_MAP(ERAS_cl_InsideScroll, CScrollView)
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ERAS_cl_InsideScroll::ERAS_cl_InsideScroll(void)
{
	mpo_Parent = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_InsideScroll::OnVScroll(UINT a, UINT b, CScrollBar *c)
{
	CScrollView::OnVScroll(a, b, c);
	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_InsideScroll::OneTrameEnding(HDC _hdc)
{
#ifdef RASTERS_ON
	/*~~~~*/
	int i_Y;
	/*~~~~*/

	i_Y = 5 - GetScrollPosition().y;
	i_Y = PRO_OneTrameEnding(m_hWnd, _hdc, i_Y);
	if(i_Y > mo_Size.cy)
	{
		mo_Size.cy = i_Y;
		SetScrollSizes(MM_TEXT, mo_Size);
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ERAS_cl_InsideScroll::OnEraseBkgnd(CDC *pdc)
{
	CScrollView::OnEraseBkgnd(pdc);
#ifdef RASTERS_ON
	if(mpo_Parent)
	{
		CFont	*poldfnt;
		poldfnt = pdc->SelectObject(&mpo_Parent->mo_Fnt);
		PRO_ForceDisplayRasters();
		OneTrameEnding(pdc->m_hDC);
		pdc->SelectObject(poldfnt);
	}
#endif

	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(ERAS_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(ERAS_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_CBN_SELENDOK(OUTPUT_IDC_COMBOCATEGORY, OnCategoryChange)
	ON_CBN_SELENDOK(OUTPUT_IDC_COMBOSUBCATEGORY, OnSubCategoryChange)
	ON_CBN_SELENDOK(OUTPUT_IDC_COMBONAME, OnNameChange)
	ON_CBN_CLOSEUP(OUTPUT_IDC_COMBOCATEGORY, OnCategoryChange)
	ON_CBN_CLOSEUP(OUTPUT_IDC_COMBOSUBCATEGORY, OnSubCategoryChange)
	ON_CBN_CLOSEUP(OUTPUT_IDC_COMBONAME, OnNameChange)
	ON_CBN_DROPDOWN(OUTPUT_IDC_COMBOCATEGORY, FillComboCategory)
	ON_CBN_DROPDOWN(OUTPUT_IDC_COMBOSUBCATEGORY, FillComboSubCategory)
	ON_CBN_DROPDOWN(OUTPUT_IDC_COMBONAME, FillComboName)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ERAS_cl_Frame::ERAS_cl_Frame(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ERAS_cl_Frame::~ERAS_cl_Frame(void)
{
	mo_Fnt.DeleteObject();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
extern AI_tdst_GlobalMessageList	gast_GlobalMessages[C_GM_MaxTypes];
extern AI_tdst_GlobalMessage	*pst_BreakOnMessageDelete;
#else
extern "C" AI_tdst_GlobalMessageList	gast_GlobalMessages[C_GM_MaxTypes];
extern "C" AI_tdst_GlobalMessage	*pst_BreakOnMessageDelete;
#endif
void ERAS_cl_Frame::OnAction(ULONG _ul_Action)
{
	switch(_ul_Action)
	{
	case ERAS_ACTION_ADDWATCH:
        {
            // Ask for ID of message
            AI_tdst_GlobalMessage *pst_GlobalMsg;
            AI_tdst_Message	*pst_Msg;
            char sText[50][256];
            EDIA_cl_NameDialogCombo o_Dialog("Choose message ID from list");

            // Add possible message IDs to dialog

            /* No raster to display */
            if(mpst_CurrentCategory == NULL) 
                return ;

            AI_tdst_GlobalMessageList *pst_GlobalMsgList = gast_GlobalMessages+mpst_CurrentSubCategory->ul_UserData;
            int j,iMsgNb = 0;
            for(j = 0; j < pst_GlobalMsgList->num; j++)
            {
                pst_GlobalMsg = pst_GlobalMsgList->msg+j;
                pst_Msg = &pst_GlobalMsg->msg;
                if (pst_GlobalMsg->flags & C_GM_Deleted) continue;

                sprintf(sText[iMsgNb],"%d",pst_Msg->i_Id);
                o_Dialog.AddItem(sText[iMsgNb],iMsgNb);
                iMsgNb++;
            }

            if (!iMsgNb)
                return;

        	o_Dialog.SetDefault(sText[0]);

            // Display dialog to ask for ID of message
            while (o_Dialog.DoModal() == IDOK)
            {
                // Is ID correct ?
                ULONG ulID;
                if ((1 != sscanf((char *) (LPCSTR) o_Dialog.mo_Name, "%u", &ulID)))
                    continue;

                pst_Msg = NULL;
                for(j = 0; j < pst_GlobalMsgList->num; j++)
                {
                    pst_GlobalMsg = pst_GlobalMsgList->msg+j;
                    pst_Msg = &pst_GlobalMsg->msg;
                    if (pst_GlobalMsg->flags & C_GM_Deleted) continue;

                    if (ulID == pst_Msg->i_Id)
                    {
                        // Display dialog to change watchpoints for this message.
                        CCheckList o_Check;
                        CRect o_Rect;
                        mpo_MyView->GetWindowRect(&o_Rect);
                        o_Rect.left += (o_Rect.Width() / 2) - 100;
                        o_Rect.top += (o_Rect.Height() / 2) - 100;

                        o_Check.AddString( "Break on destroy" );
                        o_Check.AddString( "GAO 1" );
                        o_Check.AddString( "GAO 2" );
                        o_Check.AddString( "GAO 3" );
                        o_Check.AddString( "GAO 4" );
                        o_Check.AddString( "GAO 5" );
                        o_Check.AddString( "INT 1" );
                        o_Check.AddString( "INT 2" );
                        o_Check.AddString( "INT 3" );
                        o_Check.AddString( "INT 4" );
                        o_Check.AddString( "INT 5" );
                        o_Check.AddString( "VECT_X 1" );
                        o_Check.AddString( "VECT_Y 1" );
                        o_Check.AddString( "VECT_Z 1" );
                        o_Check.AddString( "VECT_X 2" );
                        o_Check.AddString( "VECT_Y 2" );
                        o_Check.AddString( "VECT_Z 2" );
                        o_Check.AddString( "VECT_X 3" );
                        o_Check.AddString( "VECT_Y 3" );
                        o_Check.AddString( "VECT_Z 3" );
                        o_Check.AddString( "VECT_X 4" );
                        o_Check.AddString( "VECT_Y 4" );
                        o_Check.AddString( "VECT_Z 4" );
                        o_Check.AddString( "VECT_X 5" );
                        o_Check.AddString( "VECT_Y 5" );
                        o_Check.AddString( "VECT_Z 5" );

                        int l_Count;
                        BOOL	ab_UpdateFlags[ 32 ];
                        BOOL	ab_FormerUpdateFlags[ 32 ];
                        void    *ap_Address[32];

                        L_memset(ab_UpdateFlags,0,32*sizeof(BOOL));

                        for (l_Count = 0; l_Count < 5; l_Count++ ) 
                        {
                            ap_Address[ l_Count ] = &pst_Msg->pst_GAO1 + l_Count;
                            ap_Address[ l_Count + 5 ] = &pst_Msg->i_Int1 + l_Count;
                            ap_Address[ l_Count * 3 + 10] = &(pst_Msg->st_Vec1.x) + l_Count * 3;
                            ap_Address[ l_Count * 3 + 11] = &(pst_Msg->st_Vec1.y) + l_Count * 3;
                            ap_Address[ l_Count * 3 + 12] = &(pst_Msg->st_Vec1.z) + l_Count * 3;
                        }

                        ab_UpdateFlags[ 0 ] = (pst_BreakOnMessageDelete == pst_GlobalMsg);

                        for (l_Count = 0; l_Count < 25; l_Count++ )
                            ab_UpdateFlags[ l_Count + 1 ] = AI_bIsWatched(ap_Address[l_Count]);

                        L_memcpy(ab_FormerUpdateFlags,ab_UpdateFlags,32*sizeof(BOOL));

                        o_Check.SetArrayBool( ab_UpdateFlags );
                       
                        if (o_Check.Do( &o_Rect, 200, this, 30, "Choose watch points"))
                        {
                            pst_BreakOnMessageDelete = ab_UpdateFlags[0] ? pst_GlobalMsg : NULL;

                            for (l_Count = 0; l_Count < 25; l_Count++ ) 
                            {
                                if (ab_FormerUpdateFlags[l_Count+1] != ab_UpdateFlags[l_Count+1])
                                {
                                    if (ab_UpdateFlags[l_Count+1])
                                    {
                                        AI_AddWatch(ap_Address[l_Count],4);
                                    }
                                    else
                                    {
                                        AI_RemoveWatch(ap_Address[l_Count]);
                                    }
                                }
                            }
                        }

                        goto end_add_watch;
                    }
                }
            }
            // End of add watch dialog (either because of cancel or dialog has been displayed).
end_add_watch:;
        }
        break;
    }
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ERAS_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CCreateContext	o_Context;
	LOGFONT			*plf;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_ScrollView = NULL;

	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1) return -1;

	/* Create font. */
	plf = (LOGFONT *) LocalAlloc(LPTR, sizeof(LOGFONT));
	lstrcpy(plf->lfFaceName, EDI_STR_Csz_Font);
	plf->lfHeight = 12;
	plf->lfEscapement = 0;
	mo_Fnt.CreateFontIndirect(plf);
	LocalFree((LOCALHANDLE) plf);

	/* Create scroll view. */
	o_Context.m_pNewViewClass = RUNTIME_CLASS(ERAS_cl_InsideScroll);
	mpo_ScrollView = (ERAS_cl_InsideScroll *) CreateView(&o_Context);
	mpo_ScrollView->mpo_Parent = this;
	mpo_ScrollView->mo_Size = CSize(1, 1);

	/* Fill combos */
	FillComboCategory();
	FillComboSubCategory();
	FillComboName();

	/* Redraw window */
	RecalcLayout();

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CRect		o_Rect;
	CComboBox	*po_Combo;
	/*~~~~~~~~~~~~~~~~~~*/

	/* Call parent function */
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(OUTPUT_IDC_COMBOCATEGORY);
	po_Combo->GetWindowRect(o_Rect);
	mpo_DialogBar->ScreenToClient(o_Rect);
	po_Combo->SetWindowPos(0, 0, 0, cx - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(OUTPUT_IDC_COMBOSUBCATEGORY);
	po_Combo->SetWindowPos(0, 0, 0, cx - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(OUTPUT_IDC_COMBONAME);
	po_Combo->SetWindowPos(0, 0, 0, cx - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);

	/* Change scroll view size. For update background */
	if(mpo_ScrollView)
	{
		mpo_ScrollView->SetScrollSizes(MM_TEXT, mpo_ScrollView->mo_Size);
		mpo_ScrollView->Invalidate();
	}
}

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_Frame::FillComboCategory(void)
{
#ifdef RASTERS_ON
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PRO_tdst_TrameRaster	*_pst_Current;
	CComboBox				*po_ComboCategory;
	int						i_Res, i_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!this->mb_IsActivate) return;

	po_ComboCategory = (CComboBox *) mpo_DialogBar->GetDlgItem(OUTPUT_IDC_COMBOCATEGORY);
	i_Sel = po_ComboCategory->GetCurSel();
	if(i_Sel == -1) i_Sel = 0;
	po_ComboCategory->ResetContent();

	_pst_Current = _PRO_gpst_FirstTrameRaster;
	while(_pst_Current)
	{
		if(po_ComboCategory->FindString(-1, (LPCSTR) _pst_Current->psz_Category) == LB_ERR)
		{
			i_Res = po_ComboCategory->AddString((LPCSTR) _pst_Current->psz_Category);
			po_ComboCategory->SetItemData(i_Res, (DWORD) _pst_Current);
		}

		if
		(
			_pst_Current->pst_NextRaster 
		&&	!_pst_Current->pst_NextRaster->pst_PrevRaster
		)
		{
			_pst_Current->pst_NextRaster = NULL;
		}

		_pst_Current = _pst_Current->pst_NextRaster;
	}

	po_ComboCategory->InsertString(0, "-- Display None --");
	po_ComboCategory->SetItemData(0, NULL);

	if(i_Sel >= po_ComboCategory->GetCount()) i_Sel = po_ComboCategory->GetCount() - 1;
	po_ComboCategory->SetCurSel(i_Sel);
	mpst_CurrentCategory = (PRO_tdst_TrameRaster *) po_ComboCategory->GetItemData(i_Sel);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_Frame::FillComboSubCategory(void)
{
#ifdef RASTERS_ON
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PRO_tdst_TrameRaster	*_pst_Current;
	CComboBox				*po_ComboSubCategory;
	int						i_Num, i_Res, i_Sel;
	int						i_Find;
	CList<char *, char *>	o_Lst;
	POSITION				pos;
	char					*pz;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Num = 0;

	po_ComboSubCategory = (CComboBox *) mpo_DialogBar->GetDlgItem(OUTPUT_IDC_COMBOSUBCATEGORY);
	if (!po_ComboSubCategory) 
		return;
	i_Sel = po_ComboSubCategory->GetCurSel();
	if(i_Sel == -1) i_Sel = 0;
	po_ComboSubCategory->ResetContent();
	mpst_CurrentSubCategory = NULL;

	if(mpst_CurrentCategory == NULL)
	{
		FillComboName();
		return;
	}

	_pst_Current = _PRO_gpst_FirstTrameRaster;
	while(_pst_Current)
	{
        if (_pst_Current->psz_SubCategory)
        {
            i_Find = 0;
            pos = o_Lst.GetHeadPosition();
            while(pos)
            {
                pz = o_Lst.GetNext(pos);
                if(!L_strcmpi(pz, _pst_Current->psz_SubCategory))
                {
                    i_Find = 1;
                    break;
                }
            }

            if(i_Find == 0)
            {
                if
                    (
                    (mpst_CurrentCategory == NULL)
                    ||	(!L_strcmpi(mpst_CurrentCategory->psz_Category, _pst_Current->psz_Category))
                    )
                {
                    o_Lst.AddTail(_pst_Current->psz_SubCategory);
                    i_Res = po_ComboSubCategory->AddString((LPCSTR) _pst_Current->psz_SubCategory);
                    po_ComboSubCategory->SetItemData(i_Res, (DWORD) _pst_Current);
                    i_Num++;
                }
            }
        }

		_pst_Current = _pst_Current->pst_NextRaster;
	}

	if(i_Num > 1)
	{
		po_ComboSubCategory->InsertString(0, "-- Display All --");
		po_ComboSubCategory->SetItemData(0, NULL);
	}

	if(po_ComboSubCategory->GetCount() == 0) return;
	if(i_Sel >= po_ComboSubCategory->GetCount()) i_Sel = po_ComboSubCategory->GetCount() - 1;
	po_ComboSubCategory->SetCurSel(i_Sel);
	mpst_CurrentSubCategory = (PRO_tdst_TrameRaster *) po_ComboSubCategory->GetItemData(i_Sel);

	/* Fill the lists of names */
	FillComboName();
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_Frame::FillComboName(void)
{
#ifdef RASTERS_ON
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PRO_tdst_TrameRaster	*_pst_Current;
	CComboBox				*po_ComboName;
	int						i_Num, i_Res, i_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Num = 0;

	po_ComboName = (CComboBox *) mpo_DialogBar->GetDlgItem(OUTPUT_IDC_COMBONAME);
	if (!po_ComboName) return;
	i_Sel = po_ComboName->GetCurSel();
	if(i_Sel == -1) i_Sel = 0;
	po_ComboName->ResetContent();
	mpst_CurrentName = NULL;

	if(mpst_CurrentCategory == NULL)
	{
		return;
	}

	_pst_Current = _PRO_gpst_FirstTrameRaster;
	while(_pst_Current)
	{
		if(po_ComboName->FindString(-1, (LPCSTR) _pst_Current->psz_Name) == LB_ERR)
		{
			if
			(
				(mpst_CurrentSubCategory == NULL)
			||	(!L_strcmpi(mpst_CurrentSubCategory->psz_SubCategory, _pst_Current->psz_SubCategory))
			)
			{
				i_Res = po_ComboName->AddString((LPCSTR) _pst_Current->psz_Name);
				po_ComboName->SetItemData(i_Res, (DWORD) _pst_Current);
				i_Num++;
			}
		}

		_pst_Current = _pst_Current->pst_NextRaster;
	}

	if(i_Num > 1)
	{
		po_ComboName->InsertString(0, "-- Display All --");
		po_ComboName->SetItemData(0, NULL);
	}

	if(po_ComboName->GetCount() == 0) return;
	if(i_Sel >= po_ComboName->GetCount()) i_Sel = po_ComboName->GetCount() - 1;
	po_ComboName->SetCurSel(i_Sel);
	mpst_CurrentName = (PRO_tdst_TrameRaster *) po_ComboName->GetItemData(i_Sel);
#endif
}

/*$4
 ***********************************************************************************************************************
    MESSAGES
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_Frame::OnCategoryChange(void)
{
#ifdef RASTERS_ON
	/*~~~~~~~~~~~~~~~~~~*/
	int			i_Res;
	CComboBox	*po_Combo;
	/*~~~~~~~~~~~~~~~~~~*/

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(OUTPUT_IDC_COMBOCATEGORY);
	i_Res = po_Combo->GetCurSel();
	if(i_Res == CB_ERR) return;

	mpst_CurrentCategory = (PRO_tdst_TrameRaster *) po_Combo->GetItemData(i_Res);
	FillComboSubCategory();
	mpo_ScrollView->RedrawWindow();
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_Frame::OnSubCategoryChange(void)
{
#ifdef RASTERS_ON
	/*~~~~~~~~~~~~~~~~~~*/
	int			i_Res;
	CComboBox	*po_Combo;
	/*~~~~~~~~~~~~~~~~~~*/

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(OUTPUT_IDC_COMBOSUBCATEGORY);
	i_Res = po_Combo->GetCurSel();
	if(i_Res == CB_ERR) return;

	mpst_CurrentSubCategory = (PRO_tdst_TrameRaster *) po_Combo->GetItemData(i_Res);
	FillComboName();
	mpo_ScrollView->RedrawWindow();
#endif
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_Frame::OnNameChange(void)
{
#ifdef RASTERS_ON
	/*~~~~~~~~~~~~~~~~~~*/
	int			i_Res;
	CComboBox	*po_Combo;
	/*~~~~~~~~~~~~~~~~~~*/

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(OUTPUT_IDC_COMBONAME);
	i_Res = po_Combo->GetCurSel();
	if(i_Res == CB_ERR) return;

	mpst_CurrentName = (PRO_tdst_TrameRaster *) po_Combo->GetItemData(i_Res);
	mpo_ScrollView->RedrawWindow();
#endif
}

/*$4
 ***********************************************************************************************************************
    EVENTS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ERAS_cl_Frame::b_CanActivate(void)
{
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_Frame::WhenAppClose(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_Frame::OneTrameEnding(void)
{
	/*~~~~~~~~~~~~~*/
	CDC		*pdc;
	CFont	*poldfnt;
	/*~~~~~~~~~~~~~*/

	if
	(
		M_MF()->mpo_MaxView
	&&	M_MF()->mpo_MaxView != mpo_MyView
	&&	!mpo_MyView->mb_Floating
	&&	M_MF()->mst_Desktop.b_VeryMaximized
	&&	mb_IsActivate
	) return;

	pdc = mpo_ScrollView->GetDC();
	poldfnt = pdc->SelectObject(&mo_Fnt);
	mpo_ScrollView->OneTrameEnding(pdc->m_hDC);
	pdc->SelectObject(poldfnt);
	ReleaseDC(pdc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERAS_cl_Frame::OnRefresh(void)
{
	OneTrameEnding();
}

#endif /* ACTIVE_EDITORS */

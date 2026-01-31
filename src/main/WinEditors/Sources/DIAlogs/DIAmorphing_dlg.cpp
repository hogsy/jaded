/*$T DIAmorphing_dlg.cpp GC! 1.081 06/14/00 10:03:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAmorphing_dlg.h"
#include "DIAlogs/DIAmorphingsliders_dlg.h"
//#include "DIAlogs/DIAmorphing3Dview.h"
#include "DIAlogs/DIAfile_dlg.h"

#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "Engine/Sources/MODifier/MDFmodifier_GEO.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dframe.h"

#include "EDIapp.h"
#include "EDIeditors_infos.h"
#include "Editors/Sources/OUTput/OUTframe.h"
#include "Editors/Sources/OUTput/OUTmsg.h"
#include "GEOmetric/GEOsubobject.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "LINKs/LINKtoed.h"
#include "LINks/LINKmsg.h"


#include "EDImsg.h"

BEGIN_MESSAGE_MAP(EDIA_cl_MorphingDialog, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_LBN_SELCHANGE(IDC_LIST_DATA, OnSelChange_DataList)
    ON_BN_CLICKED(IDC_CHECK_VEDIT, OnCheck_VEdit)
    ON_BN_CLICKED(IDC_BUTTON_VSEL, OnButton_VSel)
    ON_BN_CLICKED(IDC_BUTTON_VDELSEL, OnButton_VDel)
    ON_BN_CLICKED(IDC_CHECK_HIDEVECTOR, OnCheck_HideVector )
    ON_BN_CLICKED(IDC_CHECK_HIDESUBOBJECT, OnCheck_HideSubObject )
    ON_BN_CLICKED(IDC_BUTTON_SELTHRESH, OnButton_VSelThresh )
	ON_BN_CLICKED(IDC_BUTTON_DATARENAME, OnButton_DataRename)
	ON_BN_CLICKED(IDC_BUTTON_DATADELETE, OnButton_DataDelete)
	ON_BN_CLICKED(IDC_BUTTON_CHANNELNEW, OnButton_ChannelNew)
	ON_BN_CLICKED(IDC_BUTTON_CHANNELDEL, OnButton_ChannelDel)
	ON_LBN_SELCHANGE(IDC_LIST_CHANNEL, OnSelChange_ChannelList)
	ON_BN_CLICKED(IDC_BUTTON_CHANNELRENAME, OnButton_ChannelRename)
    ON_BN_CLICKED(IDC_BUTTON_INSERTHOLE, OnButton_InsertHole )
    ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButton_Load )
    ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButton_Save )
	ON_EN_KILLFOCUS(IDC_EDIT_BLEND, OnEdit_Blend)
	ON_EN_KILLFOCUS(IDC_EDIT_CHANNELBLEND, OnEdit_ChannelBlend)
	ON_WM_LBUTTONUP()
    ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_CHECK_EXTSLIDERS, OnCheck_ExtSliders)
	ON_BN_CLICKED(IDC_CHECK_DISPDUMMY, OnCheck_DisplayDummyChannels)
	ON_BN_CLICKED(IDC_BUTTON_DELETEDUMMY, OnButton_DeleteDummyChannels)
	ON_BN_CLICKED(IDC_BUTTON_CREATEDUMMY, OnButton_CreateDummyChannels)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MorphingDialog::EDIA_cl_MorphingDialog(MDF_tdst_Modifier *_pst_Modifier, OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *_pst_Obj ) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_MORPHING)
{
    WOR_tdst_World          *pst_World;
    GDI_tdst_DisplayData    *pst_DD;

    pst_World = WOR_World_GetWorldOfObject(_pst_GO);
    pst_DD = (GDI_tdst_DisplayData *) pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas;
    if ( !pst_DD ) return;
	mpst_Modifier = _pst_Modifier;
	mpst_Morph = (GEO_tdst_ModifierMorphing *) mpst_Modifier->p_Data;
	mi_DataPick = 0;
	mi_Capture = mi_DD = 0;
	mf_FactorOffset = mf_ProgOffset = 0;
	mb_LockRefresh = FALSE;
    mpst_Gao = _pst_GO;
    mpst_Geo = _pst_Obj;

    pst_World = WOR_World_GetWorldOfObject(_pst_GO);
    pst_DD = (GDI_tdst_DisplayData *) pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas;
    mpst_EditOptions = (pst_DD) ? pst_DD->pst_EditOptions : NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MorphingDialog::~EDIA_cl_MorphingDialog(void)
{
	if ( mpo_Sliders )
	{
		delete mpo_Sliders;
		mpo_Sliders = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MorphingDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	Data_FillList();
	Channel_FillList();

	// Disable these button. They will be re-enabled by the EDIA_cl_MorphEditDialog class if necessary.
	// Anyway, they are hidden by the global window if this object is a EDIA_cl_MorphingDialog and not
	// a EDIA_cl_MorphEditDialog.
	GetDlgItem(IDC_BUTTON_LOADMORPH)->EnableWindow( FALSE );
	GetDlgItem(IDC_BUTTON_SAVEMORPH)->EnableWindow( FALSE );

	GetWindowRect(&o_Rect);
	GetDlgItem(IDC_EDIT_BLEND)->GetWindowRect(&mo_FactorRect);
	mo_FactorRect.left = mo_FactorRect.right;
	mo_FactorRect.right = o_Rect.right;
	ScreenToClient(&mo_FactorRect);
	GetDlgItem(IDC_EDIT_CHANNELBLEND)->GetWindowRect(&mo_ProgRect);
	mo_ProgRect.left = mo_ProgRect.right;
	mo_ProgRect.right = o_Rect.right;
	ScreenToClient(&mo_ProgRect);

	mf_ScaleDiv = 4.0f;
	mf_Scale = 0; //(float) (mo_ProgRect.right - mo_ProgRect.left - 16) / mf_ScaleDiv;
	mf_Origine = (float) mo_ProgRect.left + 8;
	
	mpo_Sliders = NULL;
	mb_DispDummies = FALSE;

    GetDlgItem( IDC_CHECK_DATAPICKUPDATE )->EnableWindow( FALSE );
    GetDlgItem( IDC_CHECK_DATAPICKEXTUPDATE )->EnableWindow( FALSE );
    GetDlgItem( IDC_EDIT_SELTHRESH )->SetWindowText( "0.001" );
	GetDlgItem( IDC_CHECK_HIDEVECTOR )->EnableWindow( FALSE );
    GetDlgItem( IDC_CHECK_HIDESUBOBJECT )->EnableWindow( FALSE );

	((CButton *) GetDlgItem(IDC_CHECK_EXTSLIDERS))->SetCheck( mpo_Sliders != NULL );
	((CButton *) GetDlgItem(IDC_CHECK_DISPDUMMY))->SetCheck( mb_DispDummies );

    if ( mpst_EditOptions )
    {
        ((CButton *) GetDlgItem( IDC_CHECK_HIDEVECTOR ))->SetCheck( (mpst_EditOptions->ul_Flags & GRO_Cul_EOF_HideMorphVector) ? 1 : 0 );
        ((CButton *) GetDlgItem( IDC_CHECK_HIDESUBOBJECT ))->SetCheck( (mpst_EditOptions->ul_Flags & GRO_Cul_EOF_HideSubObject4Morph) ? 1 : 0 );
    }

    SetWindowPos( NULL, 0, 0, 560, 400, SWP_NOMOVE | SWP_NOZORDER );
    CenterWindow();

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_MorphingDialog::i_OnMessage(ULONG msg, ULONG, ULONG) 
{ 
	if(msg == EDI_MESSAGE_REFRESHDLG)
	{
		RefreshFromExt();
	}
	if (msg == EOUT_MESSAGE_DESTROYWORLD)
	{
		DestroyWindow();
	}
	return 1; 
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::RefreshFromExt(void)
{
	if(mb_LockRefresh) return;
	M_MF()->LockDisplay(this);
	Data_FillList();
	Channel_FillList();
	Channel_Display(-1);
	InvalidateRect(&mo_FactorRect);
	InvalidateRect(&mo_ProgRect);
	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC		*pDC;
	CRgn	o_Region, o_Region2;
	CPen	o_RedPen, o_BluePen, o_BlackPen;
	void	*p_OldPen;
	int		x, y, i, j, k;
	float	fx, f, offset;
	char	sz_Text[10];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnPaint();

	Channel_GetCur();
	if((mpst_CurChannel) && (mpst_CurChannel->l_NbData))
	{
		pDC = GetDC();

		o_Region.CreateRectRgn(mo_FactorRect.left + 8, mo_FactorRect.top, mo_FactorRect.right - 8, mo_ProgRect.bottom);
		o_Region2.CreateRectRgn(mo_FactorRect.left, mo_FactorRect.top, mo_FactorRect.right, mo_ProgRect.bottom);
		o_RedPen.CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
		o_BluePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
		o_BlackPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		p_OldPen = pDC->SelectObject(o_BluePen);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);

		for(k = 0; k < 2; k++)
		{
			if(k == 0) 
			{
				y = mo_FactorRect.bottom - 4;
				offset = mf_FactorOffset;
				j = (int) mf_ScaleDiv + 1;
			}
			else
			{
				y = mo_ProgRect.bottom - 4;
				offset = mf_ProgOffset;
				j = mpst_CurChannel->l_NbData;
				if(j > mf_ScaleDiv) j = (int) mf_ScaleDiv;
				if(j == 0) continue;
			}

			f = (float) floor(offset);
			fx = mf_Origine + (f - offset) * mf_Scale;
			i = (int) f;
			j += i;

			pDC->MoveTo((int) fx, y);
			for(;; i++)
			{
				pDC->SelectClipRgn(&o_Region2);
				pDC->SelectObject(&o_BlackPen);
				sprintf(sz_Text, "%d", i);
				pDC->TextOut((int) fx, y, sz_Text);
				if(i < j)
				{
					pDC->SelectClipRgn(&o_Region);
					fx += mf_Scale;
					pDC->SelectObject((i & 1) ? &o_RedPen : &o_BluePen);
					pDC->LineTo((int) fx, y);
				}
				else
					break;
			}

			pDC->SelectClipRgn(&o_Region2);
			if(k == 0)
				f = mpst_CurChannel->f_Blend;
			else
				f = mpst_CurChannel->f_ChannelBlend;
			fx = mf_Origine + (f - offset) * mf_Scale;
			x = (int) fx;

			pDC->SelectClipRgn(&o_Region2);
			pDC->SelectObject(&o_BlackPen);
			pDC->MoveTo(x, y + 2);
			pDC->LineTo(x, y - 3);
			pDC->MoveTo(x + 1, y + 1);
			pDC->LineTo(x + 1, y - 1);
			pDC->LineTo(x - 1, y - 1);
			pDC->LineTo(x - 1, y + 1);
		}

		pDC->SelectObject(p_OldPen);
		DeleteObject(o_RedPen);
		DeleteObject(o_BluePen);
		DeleteObject(o_BlackPen);
		pDC->SelectClipRgn(NULL);
		o_Region.DeleteObject();
		o_Region2.DeleteObject();
		ReleaseDC(pDC);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MorphingDialog::PreTranslateMessage(MSG *pMsg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	CPoint		o_Pt;
	int			i_Index, i_Top;
	long		l_Swap;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		if(pMsg->hwnd == GetDlgItem(IDC_EDIT_BLEND)->m_hWnd)
			GetDlgItem(IDC_EDIT_CHANNELBLEND)->SetFocus();
		else if(pMsg->hwnd == GetDlgItem(IDC_EDIT_CHANNELBLEND)->m_hWnd) 
            GetDlgItem(IDC_EDIT_BLEND)->SetFocus();
        else if (pMsg->hwnd == GetDlgItem(IDC_EDIT_DATANAME)->m_hWnd)
        {
            GetDlgItem( IDC_BUTTON_DATARENAME )->SetFocus();
            OnButton_DataRename();
        }
        else if (pMsg->hwnd == GetDlgItem(IDC_EDIT_CHANNELNAME)->m_hWnd)
        {
            GetDlgItem( IDC_BUTTON_CHANNELRENAME )->SetFocus();
            OnButton_ChannelRename();
        }
		return TRUE;
	}
	
	if((pMsg->message == WM_KEYDOWN) && GetAsyncKeyState( VK_MENU ) &&(pMsg->wParam == 'S'))
	{
		OnCheck_ExtSliders();
	}

	if(pMsg->hwnd == GetDlgItem(IDC_CHECK_DATAPICK)->GetSafeHwnd())
	{
		if(pMsg->message == WM_LBUTTONDOWN)
		{
			((CButton *) GetDlgItem(IDC_CHECK_DATAPICK))->SetCheck(1);
			mi_DataPick = 1;
			SetCapture();
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
		}
	}
    else if(pMsg->hwnd == GetDlgItem(IDC_CHECK_DATAPICKUPDATE)->GetSafeHwnd())
	{
		if(pMsg->message == WM_LBUTTONDOWN)
		{
			((CButton *) GetDlgItem(IDC_CHECK_DATAPICKUPDATE))->SetCheck(1);
			mi_DataPick = 2;
			SetCapture();
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
		}
	}
    else if(pMsg->hwnd == GetDlgItem(IDC_CHECK_DATAPICKEXT)->GetSafeHwnd())
	{
		if(pMsg->message == WM_LBUTTONDOWN)
		{
			((CButton *) GetDlgItem(IDC_CHECK_DATAPICKEXT))->SetCheck(1);
			mi_DataPick = 3;
			SetCapture();
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
		}
	}
    else if(pMsg->hwnd == GetDlgItem(IDC_CHECK_DATAPICKEXTUPDATE)->GetSafeHwnd())
	{
		if(pMsg->message == WM_LBUTTONDOWN)
		{
			((CButton *) GetDlgItem(IDC_CHECK_DATAPICKEXTUPDATE))->SetCheck(1);
			mi_DataPick = 4;
			SetCapture();
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
		}
	}
	else if(pMsg->hwnd == GetDlgItem(IDC_LIST_DATA)->GetSafeHwnd())
	{
		if(pMsg->message == WM_LBUTTONDOWN)
		{
			po_LB = (CListBox *) GetDlgItem(IDC_LIST_DATA);
			o_Pt.x = LOWORD(pMsg->lParam);
			o_Pt.y = HIWORD(pMsg->lParam);
			mi_DDIndex = LB_i_ItemFromPoint(po_LB, &o_Pt);
			if(mi_DDIndex != LB_ERR)
			{
				mi_DDIndex = po_LB->GetItemData(mi_DDIndex);
				Data_Select(mi_DDIndex);
				Data_Display(mi_DDIndex);
			}

			mo_DDPt = o_Pt;
		}
		else if(pMsg->message == WM_MOUSEMOVE)
		{
			if((pMsg->wParam & MK_LBUTTON) && (mi_DDIndex != -1))
			{
				if((abs(mo_DDPt.x - LOWORD(pMsg->lParam)) > 4) || (abs(mo_DDPt.y - HIWORD(pMsg->lParam)) > 4))
				{
					mi_DD = 1;
					SetCapture();
					::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
				}
			}

			return 1;
		}
	}
	else if(pMsg->hwnd == GetDlgItem(IDC_LIST_CHANNELDATA)->GetSafeHwnd())
	{
		if(pMsg->message == WM_LBUTTONDOWN)
		{
			po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNELDATA);
			o_Pt.x = LOWORD(pMsg->lParam);
			o_Pt.y = HIWORD(pMsg->lParam);
			mi_DDIndex = LB_i_ItemFromPoint(po_LB, &o_Pt);
			if(mi_DDIndex != LB_ERR) po_LB->SetCurSel(mi_DDIndex);
			mo_DDPt = o_Pt;
		}
		else if(pMsg->message == WM_MOUSEMOVE)
		{
			if((pMsg->wParam & MK_LBUTTON) && (mi_DDIndex != -1))
			{
				if((abs(mo_DDPt.x - LOWORD(pMsg->lParam)) > 4) || (abs(mo_DDPt.y - HIWORD(pMsg->lParam)) > 4))
				{
					mi_DD = 2;
					SetCapture();
					::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
				}
			}

			return 1;
		}
	}

	if(pMsg->message == WM_LBUTTONDOWN)
	{
		o_Pt.x = LOWORD(pMsg->lParam);
		o_Pt.y = HIWORD(pMsg->lParam);

		if(mo_FactorRect.PtInRect(o_Pt))
		{
			Factor_Set(o_Pt.x, 1);
			SetCapture();
			mi_Capture = 1;
		}
		else if(mo_ProgRect.PtInRect(o_Pt))
		{
			Factor_Set(o_Pt.x, 2);
			SetCapture();
			mi_Capture = 2;
		}
	}

	if(pMsg->message == WM_LBUTTONUP)
	{
		if(mi_Capture)
		{
			ReleaseCapture();
			mi_Capture = FALSE;
		}

		if(mi_DataPick)
		{
			Data_Pick();
			((CButton *) GetDlgItem(IDC_CHECK_DATAPICK))->SetCheck(0);
            ((CButton *) GetDlgItem(IDC_CHECK_DATAPICKUPDATE))->SetCheck(0);
            ((CButton *) GetDlgItem(IDC_CHECK_DATAPICKEXT))->SetCheck(0);
            ((CButton *) GetDlgItem(IDC_CHECK_DATAPICKEXTUPDATE))->SetCheck(0);
			mi_DataPick = 0;
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			ReleaseCapture();
		}

		if(mi_DD)
		{
			if(mi_DD == 1)
			{
				o_Pt.x = LOWORD(pMsg->lParam);
				o_Pt.y = HIWORD(pMsg->lParam);
                if(ChildWindowFromPoint(o_Pt) == GetDlgItem(IDC_LIST_DATA))
				{
                    po_LB = (CListBox *) GetDlgItem(IDC_LIST_DATA );
				    ClientToScreen(&o_Pt);
				    po_LB->ScreenToClient(&o_Pt);
				    i_Index = LB_i_ItemFromPoint(po_LB, &o_Pt);
				    //if((i_Index != -1) && (mi_DDIndex != i_Index))
                    if (i_Index != -1)
				    {
                        if (GetAsyncKeyState( VK_CONTROL ) < 0)
                        {
                            GEO_l_ModifierMorphing_AddData( mpst_Modifier, NULL, mi_DDIndex, 0, 0 );
                            if(i_Index >= mi_DDIndex) i_Index++;
                            GEO_ModifierMorphing_Move( mpst_Morph, mpst_Morph->l_NbMorphData - 1, i_Index, 0 );
                        }
                        else
                        {
                            GEO_ModifierMorphing_Swap( mpst_Morph, mi_DDIndex, i_Index, 0 );
                        }
                        Data_FillList();
	    		        Data_Display( i_Index );
				    }
				}
				if(ChildWindowFromPoint(o_Pt) == GetDlgItem(IDC_LIST_CHANNELDATA))
				{
					ChannelData_Add( FALSE );
					Channel_Display(-1);
				}
			}
			else if(mi_DD == 2)
			{
				o_Pt.x = LOWORD(pMsg->lParam);
				o_Pt.y = HIWORD(pMsg->lParam);
				if(ChildWindowFromPoint(o_Pt) != GetDlgItem(IDC_LIST_CHANNELDATA))
				{
					ChannelData_Del();
					Channel_Display(-1);
				}
			}

			mi_DD = 0;
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			ReleaseCapture();
		}
	}
	else if(pMsg->message == WM_MOUSEMOVE)
	{
		if(mi_Capture)
		{
			o_Pt.x = (short) LOWORD(pMsg->lParam);
			o_Pt.y = (short) HIWORD(pMsg->lParam);

			if(o_Pt.x > mo_FactorRect.right - 8)
			{
				if(mi_Capture == 1)
					mf_FactorOffset += 0.1f;
				else
					mf_ProgOffset += 0.1f;
				o_Pt.x = mo_FactorRect.right - 8;
			}
			else if(o_Pt.x < mo_FactorRect.left + 8)
			{
				if(mi_Capture == 1)
					mf_FactorOffset -= 0.1f;
				else
				{
					mf_ProgOffset -= 0.1f;
					if(mf_ProgOffset < 0) mf_ProgOffset = 0;
				}

				o_Pt.x = mo_FactorRect.left + 8;
			}

			Factor_Set(o_Pt.x, mi_Capture);
		}

		if(mi_DD == 1)
		{
            o_Pt.x = LOWORD(pMsg->lParam);
            o_Pt.y = HIWORD(pMsg->lParam);
            if(ChildWindowFromPoint(o_Pt) == GetDlgItem(IDC_LIST_DATA))
            {
                if ( GetAsyncKeyState( VK_CONTROL ) < 0 )
                    ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGCOPY));
                else
                    ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
            }
            else 
            {
                if(((CListBox *) GetDlgItem(IDC_LIST_CHANNEL))->GetCurSel() == LB_ERR)
				    ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
				else if(ChildWindowFromPoint(o_Pt) == GetDlgItem(IDC_LIST_CHANNELDATA))
					::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
				else
					::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
            }
		}
		else if(mi_DD == 2)
		{
			o_Pt.x = LOWORD(pMsg->lParam);
			o_Pt.y = HIWORD(pMsg->lParam);
			if(ChildWindowFromPoint(o_Pt) == GetDlgItem(IDC_LIST_CHANNELDATA))
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
				po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNELDATA);
				ClientToScreen(&o_Pt);
				po_LB->ScreenToClient(&o_Pt);
				i_Index = LB_i_ItemFromPoint(po_LB, &o_Pt);
				if((i_Index != -1) && (mi_DDIndex != i_Index))
				{
					Channel_GetCur();
					l_Swap = mpst_CurChannel->dl_DataIndex[mi_DDIndex];
					mpst_CurChannel->dl_DataIndex[mi_DDIndex] = mpst_CurChannel->dl_DataIndex[i_Index];
					mpst_CurChannel->dl_DataIndex[i_Index] = l_Swap;
					ChannelData_FillList();
					mi_DDIndex = i_Index;
					i_Top = po_LB->GetTopIndex();
					po_LB->SetCurSel(i_Index);
					po_LB->SetTopIndex(i_Top);
				}
			}
			else
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGDEL));
		}
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnSize(UINT n, int x, int y)
{
	/*~~~~~~~~~~~*/
    int h, w;
	/*~~~~~~~~~~~*/
    
    EDIA_cl_BaseDialog::OnSize(n, x, y);

    if ( !GetDlgItem( IDC_STATIC_DATA ) ) return;

    M_MF()->LockDisplay( this );

    h = (y - 82) / 2;
    w = (x - 20) / 2;

    GetDlgItem( IDC_STATIC_DATA )->MoveWindow( 104, 5, x - 109, 16 );
    GetDlgItem( IDC_LIST_DATA )->MoveWindow( 5, 25, w, h - 20 );
    GetDlgItem( IDC_STATIC_DATANAME )->MoveWindow( w + 15, 25, 32, 12 );
    GetDlgItem( IDC_BUTTON_DATARENAME )->MoveWindow( x - 69, 25, 64, 16 );
    GetDlgItem( IDC_EDIT_DATANAME )->MoveWindow( w + 15, 45, w, 19 );

    GetDlgItem( IDC_STATIC_NBVERTEX )->MoveWindow( w + 15, 70, 60, 12 );
    GetDlgItem( IDC_EDIT_DATANBPOINTS )->MoveWindow( w + 80, 70, 40, 16 );
    GetDlgItem( IDC_CHECK_VEDIT )->MoveWindow( w + 120, 70, 40, 16 );
    GetDlgItem( IDC_BUTTON_VSEL )->MoveWindow( w + 160, 70, 40, 16 );
    GetDlgItem( IDC_BUTTON_VDELSEL )->MoveWindow( w + 200, 70, 40, 16 );
    GetDlgItem( IDC_CHECK_HIDEVECTOR )->MoveWindow( w + 240, 70, 40, 16 );
    GetDlgItem( IDC_CHECK_HIDESUBOBJECT )->MoveWindow( w + 240, 86, 40, 16 );
    GetDlgItem( IDC_BUTTON_SELTHRESH )->MoveWindow( w + 120, 86, 40, 16 );
    GetDlgItem( IDC_EDIT_SELTHRESH )->MoveWindow( w + 160, 86, 80, 16 );
    
    GetDlgItem( IDC_BUTTON_DATADELETE )->MoveWindow( w + 15, 90, 64, 16 );
    
    GetDlgItem( IDC_STATIC_PICK )->MoveWindow( w + 15, 110, 24, 16 );
    GetDlgItem( IDC_CHECK_DATAPICK )->MoveWindow( w + 40, 110, 49, 16 );
    GetDlgItem( IDC_CHECK_DATAPICKUPDATE )->MoveWindow( w + 90, 110, 50, 16 );
    GetDlgItem( IDC_CHECK_DATAPICKEXT )->MoveWindow( w + 140, 110, 50, 16 );
    GetDlgItem( IDC_CHECK_DATAPICKEXTUPDATE )->MoveWindow( w + 190, 110, 50, 16 );
    GetDlgItem( IDC_CHECK_PICKONLYSEL )->MoveWindow( w + 40, 126, 99, 16 );
#ifdef JADEFUSION
	GetDlgItem( IDC_BUTTON_RECALC_TS )->MoveWindow( w + 140, 126, 99, 16 );
#endif
	GetDlgItem( IDC_STATIC_CHANNEL )->MoveWindow( 5, h + 10, x - 10, 16 );
    GetDlgItem( IDC_BUTTON_CHANNELNEW )->MoveWindow( 5, h + 30, w / 2, 16 );
    GetDlgItem( IDC_BUTTON_CHANNELDEL )->MoveWindow( 6 + w/2, h + 30, w / 2, 16 );
    GetDlgItem( IDC_LIST_CHANNEL )->MoveWindow( 5, h + 50, w, h - 45 );
    GetDlgItem( IDC_STATIC_CHANNELNAME )->MoveWindow( w + 15, h + 30, 80, 12 );
    GetDlgItem( IDC_BUTTON_CHANNELRENAME )->MoveWindow( x - 69, h + 30, 64, 16 );
    GetDlgItem( IDC_EDIT_CHANNELNAME )->MoveWindow( w + 15, h + 50, w, 16 );
    GetDlgItem( IDC_STATIC_DATACHAIN )->MoveWindow( w + 15, h + 70, 80, 12 );
    GetDlgItem( IDC_BUTTON_INSERTHOLE )->MoveWindow( x - 69, h + 70, 64, 16 );
    GetDlgItem( IDC_LIST_CHANNELDATA )->MoveWindow( w + 15, h + 90, w, h - 85 );

    /*POPOWARNING GetDlgItem( IDC_STATIC_FACTOR )->MoveWindow( 5, y - 49, 75, 12 );
    GetDlgItem( IDC_EDIT_BLEND )->MoveWindow( 80, y - 49, 50, 18 );
    GetDlgItem( IDC_STATIC_PROGRESSION )->MoveWindow( 5, y - 27,75, 12 );
    GetDlgItem( IDC_EDIT_CHANNELBLEND )->MoveWindow( 80, y - 27, 50, 18 );*/

    GetDlgItem( IDC_STATIC_FACTOR )->MoveWindow( 5, y - 71, 75, 12 );
    GetDlgItem( IDC_EDIT_BLEND )->MoveWindow( 80, y - 71, 50, 18 );
    GetDlgItem( IDC_STATIC_PROGRESSION )->MoveWindow( 5, y - 49,75, 12 );
    GetDlgItem( IDC_EDIT_CHANNELBLEND )->MoveWindow( 80, y - 49, 50, 18 );
	GetDlgItem( IDC_CHECK_EXTSLIDERS )->MoveWindow( 5, y - 27, 125, 18 );
	GetDlgItem( IDC_STATIC_DUMMY )->MoveWindow( 135, y - 27, 125, 18 );
	GetDlgItem( IDC_CHECK_DISPDUMMY )->MoveWindow( 260, y - 27, 80, 18 );
	GetDlgItem( IDC_BUTTON_DELETEDUMMY )->MoveWindow( 345, y - 27, 70, 18 );
	GetDlgItem( IDC_BUTTON_CREATEDUMMY )->MoveWindow( 420, y - 27, 70, 18 );

	// These button are moved outside the window so that the child class which wants to
	// display them will just have to resize the main window
	GetDlgItem( IDC_BUTTON_LOADMORPH )->MoveWindow( 5, y + 1, 150, 40 );
	GetDlgItem( IDC_BUTTON_SAVEMORPH )->MoveWindow( x - 150, y + 1, 150, 40 );

	GetDlgItem(IDC_EDIT_BLEND)->GetWindowRect(&mo_FactorRect);
	mo_FactorRect.left = mo_FactorRect.right;
	ScreenToClient(&mo_FactorRect);
    mo_FactorRect.right = x;
	GetDlgItem(IDC_EDIT_CHANNELBLEND)->GetWindowRect(&mo_ProgRect);
	mo_ProgRect.left = mo_ProgRect.right;
	ScreenToClient(&mo_ProgRect);
    mo_ProgRect.right = x;

	if (mf_Scale == 0 )
		mf_Scale = (float) (mo_ProgRect.right - mo_ProgRect.left - 16) / mf_ScaleDiv;
	else
		mf_ScaleDiv = (float) (mo_ProgRect.right - mo_ProgRect.left - 16) / mf_Scale;
		
	mf_Origine = (float) mo_ProgRect.left + 8;
	
    //Invalidate();
    M_MF()->UnlockDisplay( this );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 500;
	lpMMI->ptMinTrackSize.y = 360;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnLButtonUp(UINT , CPoint)
{
}

/*$4
 ***********************************************************************************************************************
    list box functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_MorphingDialog::LB_i_ItemFromPoint(CListBox *_po_LB, CPoint *_po_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int		i_Index, i_Last;
	BOOL	b_Outside;
	CRect	o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~*/

	i_Index = _po_LB->ItemFromPoint(*_po_Pt, b_Outside);
	if(b_Outside) return -1;
	i_Last = _po_LB->GetCount() - 1;
	if(i_Index < i_Last) return i_Index;
	_po_LB->GetItemRect(i_Last, &o_Rect);
	if(o_Rect.PtInRect(*_po_Pt)) return i_Last;
	return -1;
}


/*$4
 ***********************************************************************************************************************
    vertex functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnCheck_VEdit( void )
{
    GEO_tdst_ModifierMorphing_Data  *pst_CurData;
    int                             i_Cur;

    pst_CurData = ( (i_Cur = Data_GetCur()) == -1) ? NULL : (mpst_Morph->dst_MorphData + i_Cur) ;
    if ( (pst_CurData) && (mpst_Geo) && (mpst_Geo->pst_SubObject) && (mpst_Geo->pst_SubObject->pst_MorphData == NULL) )
    {
        mpst_Geo->pst_SubObject->pst_MorphData = (void *) pst_CurData;
        ((CButton *) GetDlgItem( IDC_CHECK_VEDIT ))->SetCheck( 1 );
        GetDlgItem( IDC_CHECK_HIDEVECTOR )->EnableWindow( TRUE );
        GetDlgItem( IDC_CHECK_HIDESUBOBJECT )->EnableWindow( TRUE );
    }
    else
    {
        if ((mpst_Geo) && (mpst_Geo->pst_SubObject))
            mpst_Geo->pst_SubObject->pst_MorphData = NULL;
        ((CButton *) GetDlgItem( IDC_CHECK_VEDIT ))->SetCheck( 0 );        
        GetDlgItem( IDC_CHECK_HIDEVECTOR )->EnableWindow( FALSE );
        GetDlgItem( IDC_CHECK_HIDESUBOBJECT )->EnableWindow( FALSE );
    }
    LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_VSel( void )
{
    GEO_tdst_ModifierMorphing_Data  *pst_CurData;
    int                             i, i_Cur;
    char                            *pc_Sel;
    
    pst_CurData = ( (i_Cur = Data_GetCur()) == -1) ? NULL : (mpst_Morph->dst_MorphData + i_Cur) ;
    if ( pst_CurData && (mpst_Geo) && (mpst_Geo->pst_SubObject) )
    {
        pc_Sel = mpst_Geo->pst_SubObject->dc_VSel;
        L_memset( pc_Sel, 0, mpst_Geo->l_NbPoints );
        for (i = 0; i < pst_CurData->l_NbVector; i++)
        {
            pc_Sel[ pst_CurData->dl_Index[ i ] ] = 1;
        }
        LINK_Refresh();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_VSelThresh( void )
{
    GEO_tdst_ModifierMorphing_Data  *pst_CurData;
    int                             i, i_Cur;
    char                            *pc_Sel;
    float                           f, f_Thresh;
    char                            sz_Text[16];

    GetDlgItem( IDC_EDIT_SELTHRESH )->GetWindowText( sz_Text, 15 );
    f_Thresh = (float) atof( sz_Text );
    sprintf( sz_Text, "%g", f_Thresh );
    GetDlgItem( IDC_EDIT_SELTHRESH )->SetWindowText( sz_Text );
    f_Thresh *= f_Thresh;

    pst_CurData = ( (i_Cur = Data_GetCur()) == -1) ? NULL : (mpst_Morph->dst_MorphData + i_Cur) ;
    if ( pst_CurData && (mpst_Geo) && (mpst_Geo->pst_SubObject) )
    {
        pc_Sel = mpst_Geo->pst_SubObject->dc_VSel;
        L_memset( pc_Sel, 0, mpst_Geo->l_NbPoints );
        for (i = 0; i < pst_CurData->l_NbVector; i++)
        {
            f = MATH_f_SqrVector( pst_CurData->dst_Vector + i );
            if (f < f_Thresh)
                pc_Sel[ pst_CurData->dl_Index[ i ] ] = 1;
        }
        LINK_Refresh();
    }
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_VDel( void )
{
    GEO_tdst_ModifierMorphing_Data  *pst_CurData;
    int                             i, j, i_Cur;
    char                            *pc_Sel;

    pst_CurData = ( (i_Cur = Data_GetCur()) == -1) ? NULL : (mpst_Morph->dst_MorphData + i_Cur) ;
    if ( !pst_CurData || !mpst_Geo || !mpst_Geo->pst_SubObject )
        return;

    pc_Sel = mpst_Geo->pst_SubObject->dc_VSel;
    
    for (i = 0, j = 0; i < pst_CurData->l_NbVector; i++)
    {
        if (pc_Sel[ pst_CurData->dl_Index[ i ] ] & 1) continue;
        
        MATH_CopyVector( pst_CurData->dst_Vector + j, pst_CurData->dst_Vector + i );
        pst_CurData->dl_Index[ j++ ] = pst_CurData->dl_Index[ i ];
    }
    pst_CurData->l_NbVector = j;
    Data_Display( i_Cur );
    LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnCheck_HideVector( void )
{
    if ( mpst_EditOptions->ul_Flags & GRO_Cul_EOF_HideMorphVector )
    {
        mpst_EditOptions->ul_Flags &= ~GRO_Cul_EOF_HideMorphVector;
        ((CButton *) GetDlgItem( IDC_CHECK_HIDEVECTOR ))->SetCheck( 0 );
    }
    else
    {
        mpst_EditOptions->ul_Flags |= GRO_Cul_EOF_HideMorphVector;
        ((CButton *) GetDlgItem( IDC_CHECK_HIDEVECTOR ))->SetCheck( 1 );
    }
    LINK_Refresh();
} 

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnCheck_HideSubObject( void )
{
    if ( mpst_EditOptions->ul_Flags & GRO_Cul_EOF_HideSubObject4Morph )
    {
        mpst_EditOptions->ul_Flags &= ~GRO_Cul_EOF_HideSubObject4Morph;
        ((CButton *) GetDlgItem( IDC_CHECK_HIDESUBOBJECT ))->SetCheck( 0 );
    }
    else
    {
        mpst_EditOptions->ul_Flags |= GRO_Cul_EOF_HideSubObject4Morph;
        ((CButton *) GetDlgItem( IDC_CHECK_HIDESUBOBJECT ))->SetCheck( 1 );
    }
    LINK_Refresh();
} 

/*$4
 ***********************************************************************************************************************
    data functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_MorphingDialog::Data_GetCur(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox						*po_LB;
	int								i_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_DATA);
	i_Sel = po_LB->GetCurSel();
	if(i_Sel == LB_ERR) return -1;
    return po_LB->GetItemData( i_Sel );
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::Data_FillList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox						*po_LB;
	GEO_tdst_ModifierMorphing_Data	*pst_Data;
	int								i_Data, i_Index, i_Sel, i_First;
	char							sz_Name[70];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_DATA);

	i_First = po_LB->GetTopIndex();
	i_Sel = po_LB->GetCurSel();
	if(i_Sel != LB_ERR) i_Sel = po_LB->GetItemData(i_Sel);

	po_LB->ResetContent();

	pst_Data = mpst_Morph->dst_MorphData;
	for(i_Data = 0; i_Data < mpst_Morph->l_NbMorphData; i_Data++, pst_Data++)
	{
		sprintf(sz_Name, "%02d - %s", i_Data, pst_Data->sz_Name);
		i_Index = po_LB->AddString(sz_Name);
		po_LB->SetItemData(i_Index, i_Data);
	}

	po_LB->SetTopIndex( i_First );

	if(i_Sel != LB_ERR) Data_Select(i_Sel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::Data_Select(int _i_Data)
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	int			i_Index;
    static int	rec = 0;
	/*~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_DATA);
	for(i_Index = 0; i_Index < po_LB->GetCount(); i_Index++)
	{
		if(po_LB->GetItemData(i_Index) == (ULONG) _i_Data) po_LB->SetCurSel(i_Index);
	}

    if (rec) return;
    rec = 1;

    if ( ((CButton *) GetDlgItem( IDC_CHECK_VEDIT))->GetCheck() )
    {
        if ( (mpst_Geo) && (mpst_Geo->pst_SubObject) )
        {
            mpst_Geo->pst_SubObject->pst_MorphData = mpst_Morph->dst_MorphData + _i_Data;
            LINK_Refresh();
        }
        else
            ((CButton *) GetDlgItem( IDC_CHECK_VEDIT ))->SetCheck(0);
    }
    rec = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::Data_Display(int _i_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox						*po_LB;
	GEO_tdst_ModifierMorphing_Data	*pst_Data;
	char							sz_Value[16];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_LB = (CListBox *) GetDlgItem(IDC_LIST_DATA);
	if(_i_Data == -1)
	{
		_i_Data = po_LB->GetCurSel();
		if(_i_Data != LB_ERR) _i_Data = po_LB->GetItemData(_i_Data);
	}

	if((_i_Data == -1) || (_i_Data >= mpst_Morph->l_NbMorphData))
	{
		GetDlgItem(IDC_EDIT_DATANAME)->SetWindowText("");
		GetDlgItem(IDC_EDIT_DATANBPOINTS)->SetWindowText("");
        GetDlgItem(IDC_CHECK_DATAPICKUPDATE)->EnableWindow( FALSE );
        GetDlgItem(IDC_CHECK_DATAPICKEXTUPDATE)->EnableWindow( FALSE );
        po_LB->SetCurSel( -1 );
	}
	else
	{
		pst_Data = mpst_Morph->dst_MorphData + _i_Data;
		GetDlgItem(IDC_EDIT_DATANAME)->SetWindowText(pst_Data->sz_Name);
		GetDlgItem(IDC_EDIT_DATANBPOINTS)->SetWindowText(_itoa(pst_Data->l_NbVector, sz_Value, 10));
        GetDlgItem(IDC_CHECK_DATAPICKUPDATE)->EnableWindow( TRUE );
        GetDlgItem(IDC_CHECK_DATAPICKEXTUPDATE)->EnableWindow( TRUE );
        po_LB->SetCurSel( _i_Data );
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::Data_Pick(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_cl_BaseFrame	*po_Editor;
	F3D_cl_View			*po_3DView;
	POINT				st_Pt;
	OBJ_tdst_GameObject *pst_GO;
    LONG                l_Data, l_CurData;
    BOOL                b_Sel, b_Ext;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&st_Pt);

	/* get editor under mouse */
	po_Editor = EDI_go_TheApp.po_EditorUnderMouse();
	if(!po_Editor) return;
	if(po_Editor->mst_Def.i_Type != EDI_IDEDIT_OUTPUT) return;
	po_3DView = ((EOUT_cl_Frame *) po_Editor)->mpo_EngineFrame->mpo_DisplayView;

	po_3DView->ScreenToClient(&st_Pt);
	if(!po_3DView->Pick_l_UnderPoint(&st_Pt, SOFT_Cuc_PBQF_GameObject, 0)) return;
	pst_GO = (OBJ_tdst_GameObject *) po_3DView->Pick_pst_GetFirst(SOFT_Cuc_PBQF_GameObject, -1)->ul_Value;

    l_CurData = Data_GetCur();
    l_Data = (mi_DataPick & 1) ? -1 : l_CurData;
    b_Sel = ((CButton *) GetDlgItem( IDC_CHECK_PICKONLYSEL ))->GetCheck() ? TRUE : FALSE;
    b_Ext = (mi_DataPick > 2) ? 1 : 0;
    
	GEO_l_ModifierMorphing_AddData(mpst_Modifier, pst_GO, l_Data, b_Sel, b_Ext );
    if (l_Data == -1) 
    {
        l_CurData++;
        GEO_ModifierMorphing_Move( mpst_Morph, mpst_Morph->l_NbMorphData - 1, l_CurData, 0 );
    }
	Data_FillList();
    Data_Select( l_CurData );
	Data_Display(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::Data_Delete(int _i_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	int									i, j;
	long								l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Data == -1) return;
	if(_i_Data >= mpst_Morph->l_NbMorphData) return;

	/* vire le pointeur utilisé par le mode sub object */
	if ( (mpst_Geo) && (mpst_Geo->pst_SubObject) && (mpst_Geo->pst_SubObject->pst_MorphData == ( mpst_Morph->dst_MorphData + _i_Data ) ) )
	{
		mpst_Geo->pst_SubObject->pst_MorphData = NULL;
		((CButton *) GetDlgItem( IDC_CHECK_VEDIT ))->SetCheck( 0 );
	}

	/* delete from list */
	if(_i_Data < mpst_Morph->l_NbMorphData - 1)
	{
		l_Size = (mpst_Morph->l_NbMorphData - 1 - _i_Data) * sizeof(GEO_tdst_ModifierMorphing_Data);
		L_memmove(mpst_Morph->dst_MorphData + _i_Data, mpst_Morph->dst_MorphData + _i_Data + 1, l_Size);
	}
	mpst_Morph->l_NbMorphData--;

	/* update channel */
	pst_Channel = mpst_Morph->dst_MorphChannel;
	for(i = 0; i < mpst_Morph->l_NbChannel; i++, pst_Channel++)
	{
		j = 0;
		while(j < pst_Channel->l_NbData)
		{
			if(pst_Channel->dl_DataIndex[j] == _i_Data)
			{
				if(j < pst_Channel->l_NbData - 1)
				{
					l_Size = (pst_Channel->l_NbData - 1 - j) * 4;
					L_memmove(pst_Channel->dl_DataIndex + j, pst_Channel->dl_DataIndex + j + 1, l_Size);
				}

				pst_Channel->l_NbData--;
				continue;
			}

			if(pst_Channel->dl_DataIndex[j] > _i_Data) pst_Channel->dl_DataIndex[j]--;
			j++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnSelChange_DataList(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_DataRename(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox						*po_LB;
	GEO_tdst_ModifierMorphing_Data	*pst_Data;
	int								i_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_DATA);
	i_Index = po_LB->GetCurSel();
	if(i_Index == LB_ERR) return;
	i_Index = po_LB->GetItemData(i_Index);
	pst_Data = mpst_Morph->dst_MorphData + i_Index;

	GetDlgItem(IDC_EDIT_DATANAME)->GetWindowText(pst_Data->sz_Name, 64);
	Data_FillList();
	Channel_Display(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_DataDelete(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox						*po_LB;
	int								i_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_DATA);
	i_Index = po_LB->GetCurSel();
	if(i_Index == LB_ERR) return;
	i_Index = po_LB->GetItemData(i_Index);

	Data_Delete(i_Index);

	Data_FillList();
	Data_Select(i_Index == mpst_Morph->l_NbMorphData ? i_Index - 1 : i_Index);
	Data_Display(-1);
	Channel_Display(-1);
}

/*$4
 ***********************************************************************************************************************
    channel functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::Channel_FillList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox							*po_LB;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	int									i_Channel, i_Index, i_Sel;
	char								sz_Name[70];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNEL);

	i_Sel = po_LB->GetCurSel();
	if(i_Sel != LB_ERR) i_Sel = po_LB->GetItemData(i_Sel);

	po_LB->ResetContent();

	pst_Channel = mpst_Morph->dst_MorphChannel;
	for(i_Channel = 0; i_Channel < mpst_Morph->l_NbChannel; i_Channel++, pst_Channel++)
	{
		if (mb_DispDummies || pst_Channel->sz_Name[0] != 'D' || pst_Channel->sz_Name[1] != '_')
		{
			sprintf(sz_Name, "%02d - %s", i_Channel, pst_Channel->sz_Name);
			i_Index = po_LB->AddString(sz_Name);
			po_LB->SetItemData(i_Index, i_Channel);
		}
	}

	if(i_Sel != LB_ERR) Channel_Select(i_Sel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::Channel_Select(int _i_Data)
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	int			i_Index;
	/*~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNEL);
	for(i_Index = 0; i_Index < po_LB->GetCount(); i_Index++)
	{
		if(po_LB->GetItemData(i_Index) == (ULONG) _i_Data) po_LB->SetCurSel(i_Index);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::Channel_Display(int _i_Channel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox							*po_LB;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	char								sz_Value[16];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Channel == -1)
	{
		po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNEL);
		_i_Channel = po_LB->GetCurSel();
		if(_i_Channel != LB_ERR) _i_Channel = po_LB->GetItemData(_i_Channel);
	}

	if((_i_Channel == -1) || (_i_Channel >= mpst_Morph->l_NbChannel))
	{
		GetDlgItem(IDC_EDIT_CHANNELNAME)->SetWindowText("");
		((CListBox *) GetDlgItem(IDC_LIST_CHANNELDATA))->ResetContent();
		GetDlgItem(IDC_EDIT_BLEND)->SetWindowText("");
		GetDlgItem(IDC_EDIT_CHANNELBLEND)->SetWindowText("");
	}
	else
	{
		pst_Channel = mpst_Morph->dst_MorphChannel + _i_Channel;
		GetDlgItem(IDC_EDIT_CHANNELNAME)->SetWindowText(pst_Channel->sz_Name);
		sprintf(sz_Value, "%.3f", pst_Channel->f_Blend);
		GetDlgItem(IDC_EDIT_BLEND)->SetWindowText(sz_Value);
		sprintf(sz_Value, "%.3f", pst_Channel->f_ChannelBlend);
		GetDlgItem(IDC_EDIT_CHANNELBLEND)->SetWindowText(sz_Value);
		ChannelData_FillList();

		if((mf_FactorOffset > pst_Channel->f_Blend) || (mf_FactorOffset + mf_ScaleDiv < pst_Channel->f_Blend))
			mf_FactorOffset = pst_Channel->f_Blend - (mf_ScaleDiv / 2);
			
		if ( pst_Channel->f_ChannelBlend > pst_Channel->l_NbData)
			pst_Channel->f_ChannelBlend = (float) pst_Channel->l_NbData;

		if(pst_Channel->l_NbData < mf_ScaleDiv)
			mf_ProgOffset = 0;
		else
		{
			if((mf_ProgOffset > pst_Channel->f_ChannelBlend) || (mf_ProgOffset + mf_ScaleDiv < pst_Channel->f_ChannelBlend))
				mf_ProgOffset = pst_Channel->f_ChannelBlend - (mf_ScaleDiv / 2);
			if(mf_ProgOffset + mf_ScaleDiv > (float) pst_Channel->l_NbData)
				mf_ProgOffset = (float) (pst_Channel->l_NbData - mf_ScaleDiv);
			if(mf_ProgOffset < 0)
				mf_ProgOffset = 0;
		}
	}

	InvalidateRect(&mo_FactorRect);
	InvalidateRect(&mo_ProgRect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_ChannelNew(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	int									i_Channel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GEO_l_ModifierMorphing_AddChannel(mpst_Modifier);

	i_Channel = mpst_Morph->l_NbChannel - 1;
	pst_Channel = &mpst_Morph->dst_MorphChannel[i_Channel];
	sprintf(pst_Channel->sz_Name, "Channel %d", i_Channel);
	Channel_FillList();
	Channel_Select(i_Channel);
	Channel_Display(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_ChannelDel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	int			i_Index, i_Channel, i_Sel;
	long		l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNEL);
	i_Index = po_LB->GetCurSel();
	if(i_Index == LB_ERR) return;

	i_Channel = po_LB->GetItemData(i_Index);

	if(i_Index == po_LB->GetCount() - 1)
		i_Sel = i_Channel - 1;
	else
		i_Sel = i_Channel;

	if(i_Channel < mpst_Morph->l_NbChannel - 1)
	{
		MEM_Free( mpst_Morph->dst_MorphChannel[i_Channel].dl_DataIndex );
		l_Size = (mpst_Morph->l_NbChannel - 1 - i_Channel) * sizeof(GEO_tdst_ModifierMorphing_Channel);
		memmove(mpst_Morph->dst_MorphChannel + i_Channel, mpst_Morph->dst_MorphChannel + i_Channel + 1, l_Size);
	}

	mpst_Morph->l_NbChannel--;

	Channel_FillList();
	if(i_Sel != -1) Channel_Select(i_Sel);
    Channel_Display( -1 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnSelChange_ChannelList(void)
{
	Channel_Display(-1);

	if (mpo_Sliders && mpo_Sliders->mi_Synchronise )
	{
		Channel_GetCur();
		mpo_Sliders->mast_Line[ 0 ].i_Index = mpst_CurChannel - mpst_Morph->dst_MorphChannel;
		mpo_Sliders->Invalidate();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_ChannelRename(void)
{
	Channel_GetCur();
	if(mpst_CurChannel == NULL) return;
	GetDlgItem(IDC_EDIT_CHANNELNAME)->GetWindowText(mpst_CurChannel->sz_Name, 64);
	Channel_FillList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::Channel_GetCur(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	int			i_Index;
	/*~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNEL);
	i_Index = po_LB->GetCurSel();
	if(i_Index == LB_ERR)
		mpst_CurChannel = NULL;
	else
	{
		i_Index = po_LB->GetItemData(i_Index);
		mpst_CurChannel = mpst_Morph->dst_MorphChannel + i_Index;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_InsertHole(void)
{
    Channel_GetCur();
	if(mpst_CurChannel == NULL) return;
    ChannelData_Add( TRUE );
    Channel_Display(-1);
}


/*$4
 ***********************************************************************************************************************
    Channel data functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::ChannelData_FillList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox							*po_LB;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	LONG								*pl_Data;
	int									i_Data, i_Sel, i_OldSel, i_Top;
    char                                sz_String[ 80 ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNELDATA);
	i_Top = po_LB->GetTopIndex();
    i_OldSel = po_LB->GetCurSel();
	po_LB->ResetContent();

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNEL);
	i_Sel = po_LB->GetCurSel();
	if(i_Sel == LB_ERR) return;
	i_Sel = po_LB->GetItemData(i_Sel);

	pst_Channel = mpst_Morph->dst_MorphChannel + i_Sel;

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNELDATA);

	pl_Data = pst_Channel->dl_DataIndex;
	for(i_Data = 0; i_Data < pst_Channel->l_NbData; i_Data++, pl_Data++)
	{
        if ( ( *pl_Data == -1) || (*pl_Data >= mpst_Morph->l_NbMorphData) )
            sprintf( sz_String, "%02d - <Hole>", i_Data + 1);
        else
            sprintf( sz_String, "%02d - [%02d] %s", i_Data + 1, *pl_Data, (mpst_Morph->dst_MorphData + *pl_Data)->sz_Name );
		po_LB->AddString( sz_String );
	}

	if(i_Top != -1)
	{
		if(i_Top >= i_Data) i_Top = i_Data - 1;
		po_LB->SetTopIndex(i_Top);
	}
    po_LB->SetCurSel( i_OldSel );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::ChannelData_Add( BOOL _b_Hole )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox							*po_LB;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	int									i_Channel, i_Data, i_Index;
	CPoint								o_Pt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNEL);
	i_Channel = po_LB->GetCurSel();
	if(i_Channel == LB_ERR) return;
	i_Channel = po_LB->GetItemData(i_Channel);
	pst_Channel = mpst_Morph->dst_MorphChannel + i_Channel;

    po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNELDATA);
    if (_b_Hole)
    {
        i_Index = po_LB->GetCurSel();
        i_Data = -1;
    }
    else
    {
	    GetCursorPos(&o_Pt);
	    po_LB->ScreenToClient(&o_Pt);
	    i_Index = LB_i_ItemFromPoint(po_LB, &o_Pt);
        po_LB = (CListBox *) GetDlgItem(IDC_LIST_DATA);
	    i_Data = po_LB->GetItemData(po_LB->GetCurSel());
    }

	GEO_l_ModifierMorphing_AddChannelData(mpst_Modifier, i_Channel);

	if(i_Index == -1)
		pst_Channel->dl_DataIndex[pst_Channel->l_NbData - 1] = i_Data;
	else
	{
		L_memmove
		(
			pst_Channel->dl_DataIndex + i_Index + 1,
			pst_Channel->dl_DataIndex + i_Index,
			(pst_Channel->l_NbData - 1 - i_Index) * 4
		);
		pst_Channel->dl_DataIndex[i_Index] = i_Data;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::ChannelData_Del(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	int			i_Data, i_Size;
	CPoint		o_Pt;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	Channel_GetCur();
	if(!mpst_CurChannel) return;

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_CHANNELDATA);
	i_Data = po_LB->GetCurSel();

	i_Size = 4 * ((mpst_CurChannel->l_NbData - 1) - i_Data);

	if(i_Size) L_memmove(mpst_CurChannel->dl_DataIndex + i_Data, mpst_CurChannel->dl_DataIndex + i_Data + 1, i_Size);

	mpst_CurChannel->l_NbData--;
}

/*$4
 ***********************************************************************************************************************
    factor functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::Factor_Set(int x, int i)
{
	/*~~~~~~~~~~~~~~~~~*/
	float	f;
	char	sz_Value[16];
	/*~~~~~~~~~~~~~~~~~*/

	Channel_GetCur();
	if((!mpst_CurChannel) || (!mpst_CurChannel->l_NbData)) return;

	f = x - mf_Origine;
	f /= mf_Scale;

	if(i == 1)
	{
		mpst_CurChannel->f_Blend = f + mf_FactorOffset;
		InvalidateRect(&mo_FactorRect);
		sprintf(sz_Value, "%.3f", mpst_CurChannel->f_Blend);
		GetDlgItem(IDC_EDIT_BLEND)->SetWindowText(sz_Value);
	}
	else
	{
		if(mpst_CurChannel->l_NbData == 1)
		{
			if(f + mf_ProgOffset > 1.0f) f = 1.0f;
			mf_ProgOffset = 0;
		}
		else if(mf_ProgOffset + mf_ScaleDiv > (float) mpst_CurChannel->l_NbData)
			mf_ProgOffset = (float) (mpst_CurChannel->l_NbData - mf_ScaleDiv);
		if (mf_ProgOffset < 0)
			mf_ProgOffset = 0;

		mpst_CurChannel->f_ChannelBlend = f + mf_ProgOffset;
		InvalidateRect(&mo_ProgRect);
		sprintf(sz_Value, "%.3f", mpst_CurChannel->f_ChannelBlend);
		GetDlgItem(IDC_EDIT_CHANNELBLEND)->SetWindowText(sz_Value);
	}

	mb_LockRefresh = TRUE;
	LINK_Refresh();
	mb_LockRefresh = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnEdit_Blend(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Value[16];
	/*~~~~~~~~~~~~~~~~~*/

	Channel_GetCur();
	if((!mpst_CurChannel) || (!mpst_CurChannel->l_NbData)) return;

	GetDlgItem(IDC_EDIT_BLEND)->GetWindowText(sz_Value, 15);
	mpst_CurChannel->f_Blend = (float) atof(sz_Value);
	Channel_Display(-1);

	mb_LockRefresh = TRUE;
	LINK_Refresh();
	mb_LockRefresh = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnEdit_ChannelBlend(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Value[16];
	/*~~~~~~~~~~~~~~~~~*/

	Channel_GetCur();
	if((!mpst_CurChannel) || (!mpst_CurChannel->l_NbData)) return;

	GetDlgItem(IDC_EDIT_CHANNELBLEND)->GetWindowText(sz_Value, 15);
	mpst_CurChannel->f_ChannelBlend = (float) atof(sz_Value);
	if(mpst_CurChannel->f_ChannelBlend < 0)
		mpst_CurChannel->f_ChannelBlend = 0;
	else if(mpst_CurChannel->f_ChannelBlend > mpst_CurChannel->l_NbData)
		mpst_CurChannel->f_ChannelBlend = (float) mpst_CurChannel->l_NbData;
	Channel_Display(-1);
	mb_LockRefresh = TRUE;
	LINK_Refresh();
	mb_LockRefresh = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnCheck_ExtSliders(void)
{
	if (!mpo_Sliders)
	{
		mpo_Sliders = new EDIA_cl_MorphingSlidersDialog( this, mpst_CurChannel ? mpst_CurChannel - mpst_Morph->dst_MorphChannel : 0 );
		mpo_Sliders->DoModeless();
	}
	else
	{
		delete mpo_Sliders;
		mpo_Sliders = NULL;
	}
	((CButton *) GetDlgItem(IDC_CHECK_EXTSLIDERS))->SetCheck( mpo_Sliders != NULL );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnCheck_DisplayDummyChannels(void)
{
	mb_DispDummies = !mb_DispDummies;
	((CButton *) GetDlgItem(IDC_CHECK_DISPDUMMY))->SetCheck( mb_DispDummies );
	Channel_FillList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_DeleteDummyChannels(void)
{
	GEO_ModifierMorphing_DeleteDummyChannels( mpst_Morph );

	Channel_FillList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_CreateDummyChannels(void)
{
	GEO_ModifierMorphing_CreateDummyChannels( mpst_Morph );

	Channel_FillList();
}

/*$4
 ***********************************************************************************************************************
    file functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_Save()
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDIA_cl_FileDialog	o_File("Choose file", 3, TRUE, TRUE, NULL, "*.mor" );
    CString				o_Temp;
    BIG_INDEX			ul_Index;
    char                *sz_Ext;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(o_File.DoModal() != IDOK) return;

    o_File.GetItem(o_File.mo_File, 0, o_Temp);

    sz_Ext = strrchr( (char *) (LPCSTR) o_Temp, '.' );
    if (sz_Ext == NULL)
        o_Temp += ".mor";
    else if (L_stricmp( sz_Ext, ".mor" ) != 0)
    {
        M_MF()->MessageBox( "Bad file extension\nUse .mor extension", "Error", MB_OK | MB_ICONSTOP );
        return;
    }
    

	ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);

	
    if(ul_Index != BIG_C_InvalidIndex)
	{
        if (M_MF()->MessageBox( "Erase file ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDNO )
            return;
    }

    SAV_Begin( o_File.masz_FullPath, (char *) (LPCSTR) o_Temp );
    GEO_ModifierMorphing_Save( mpst_Modifier );
    SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingDialog::OnButton_Load()
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDIA_cl_FileDialog	        o_File("Choose file", 3, TRUE, TRUE, NULL, "*.mor" );
    CString				        o_Temp;
    BIG_INDEX			        ul_Index;
    char                        *sz_Ext, *pc_Buffer;
    ULONG                       ul_Length;
    GEO_tdst_ModifierMorphing	*pst_Morph;
    GEO_tdst_Object             *pst_Geo;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // check data
    pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo( mpst_Modifier->pst_GO );
    if ( !pst_Geo )
    {
        M_MF()->MessageBox( "modifier is associated to a GameObject without visual object", "Error", MB_OK | MB_ICONSTOP );
        return;
    }

    if(o_File.DoModal() != IDOK) return;

    o_File.GetItem(o_File.mo_File, 0, o_Temp);

    sz_Ext = strrchr( (char *) (LPCSTR) o_Temp, '.' );
    if (sz_Ext == NULL)
        o_Temp += ".mor";
    else if (L_stricmp( sz_Ext, ".mor" ) != 0)
    {
        M_MF()->MessageBox( "Bad file extension\nUse .mor extension", "Error", MB_OK | MB_ICONSTOP );
        return;
    }
    

	ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
    if(ul_Index == BIG_C_InvalidIndex)
	{
        M_MF()->MessageBox( "file doesn't exist !!", "Error", MB_OK | MB_ICONSTOP );
        return;
    }

    pc_Buffer = BIG_pc_ReadFileTmp( BIG_PosFile( ul_Index ), &ul_Length );

    // create new morph data
    GEO_ModifierMorphing_Create( mpst_Modifier->pst_GO, mpst_Modifier, NULL );
    // load data
    GEO_ul_ModifierMorphing_Load( mpst_Modifier, pc_Buffer );

    /* cheack data */
    pst_Morph = (GEO_tdst_ModifierMorphing *) mpst_Modifier->p_Data;
    if (!pst_Morph)
    {
        M_MF()->MessageBox( "Loading failed", "Error", MB_OK | MB_ICONSTOP );
        mpst_Modifier->p_Data = mpst_Morph;
        return;
    }

    if (pst_Geo->l_NbPoints != pst_Morph->l_NbPoints)
    {
        M_MF()->MessageBox( "Morph and Geometry doesn't have same number of vertices", "Error", MB_OK | MB_ICONSTOP );
        GEO_ModifierMorphing_Destroy( mpst_Modifier );
        mpst_Modifier->p_Data = mpst_Morph;
        return;
    }

    // free old morph modifier
    mpst_Modifier->p_Data = mpst_Morph;
    GEO_ModifierMorphing_Destroy( mpst_Modifier );
    mpst_Modifier->p_Data = pst_Morph;

    // update display
    mpst_Morph = (GEO_tdst_ModifierMorphing *) mpst_Modifier->p_Data;
    Data_FillList();
	Channel_FillList();
}

#ifdef JADEFUSION
afx_msg void
EDIA_cl_MorphingDialog::OnButton_RecalcAllTangentSpaceData(void)
{
    GEO_ModifierMorphing_RecomputeAllTangentSpaceData( mpst_Modifier );
}
#endif

#endif /* ACTIVE_EDITORS */

/*$T PFBview.cpp GC! 1.081 02/18/04 11:52:15 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- base -----------------------------------------------------------------------------------------------------------*/

#include "BASe/BAStypes.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "Res/Res.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDIicons.h"
#include "EDImsg.h"
#include "GDInterface/GDInterface.h"
#include "SELection/SELection.h"
#include "DIAlogs/DIAvector_dlg.h"
#include "DIAlogs/DIAmatrix_dlg.h"
#include "LINKs/LINKmsg.h"
#include "LINks/LINKtoed.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDItors/Sources/TEXtures/TEXutil.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "F3Dframe\F3Dframe.h"
#include "F3Dframe\F3Dview.h"

/*$2- prefab ---------------------------------------------------------------------------------------------------------*/

#include "PFBframe.h"
#include "PFBView.h"
#include "PFBtreeView.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    EXTERNAL.
 -----------------------------------------------------------------------------------------------------------------------
 */
extern BOOL						EDI_gb_CheckInDel;


/*$4
 ***********************************************************************************************************************
    message map
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNCREATE(EPFB_cl_View, CFormView)
BEGIN_MESSAGE_MAP(EPFB_cl_View, CFormView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_EN_CHANGE(IDC_EDIT_COMMENT, OnCommentChange)
	ON_BN_CLICKED(IDC_BUTTON_TYPE, OnButtonType)
	ON_BN_CLICKED(IDC_BUTTON_OFFSET, OnButtonOffset)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_CHECK_COMMENT, OnButtonComment )
	ON_BN_CLICKED(IDC_CHECK_PREVIEW, OnButtonPreview )
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    extern
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    menbers
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EPFB_cl_View::EPFB_cl_View(EPFB_cl_Frame *_po_Editor) :
	CFormView(EPFB_IDD_DATAVIEW)
{
	mpo_Editor = _po_Editor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EPFB_cl_View::EPFB_cl_View(void) :
	CFormView(EPFB_IDD_DATAVIEW)
{
	mpo_Editor = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EPFB_cl_View::~EPFB_cl_View(void)
{
	if (mh_PreviewBitmap)
		DeleteObject(mh_PreviewBitmap);
	mh_PreviewBitmap = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPFB_cl_View::Create
(
	LPCTSTR		lpszClassName,
	LPCTSTR		lpszWindowName,
	DWORD		dwStyle,
	const RECT	&rect,
	CWnd		*pParentWnd,
	UINT		nID
)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl	*po_ListRef;
	/*~~~~~~~~~~~~~~~~~~~~*/

	CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, NULL);

	po_ListRef = (CListCtrl *) GetDlgItem(IDC_LISTCTRL_REF);
	if(po_ListRef)
	{
		po_ListRef->InsertColumn(0, "Ref", LVCFMT_LEFT, 200, 0);
		po_ListRef->InsertColumn(1, "Path", LVCFMT_LEFT, 300, 1);
		po_ListRef->InsertColumn(2, "BV", LVCFMT_LEFT, 100, 2);
		po_ListRef->SetImageList(&(M_MF()->mo_FileImageList), LVSIL_SMALL);
	}
	
	mh_PreviewBitmap = NULL;
	mi_Preview = 0;
	mi_DisplayComment = 1;
	mi_DisplayPreview = 1;
	
	((CButton *) GetDlgItem( IDC_CHECK_COMMENT ))->SetCheck( 1 );
	((CButton *) GetDlgItem( IDC_CHECK_PREVIEW ))->SetCheck( 1 );

	EDIA_cl_BaseDialog::SetTheme(this);
	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    Message functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPFB_cl_View::PreTranslateMessage(MSG *pMsg)
{
	if(pMsg->hwnd == GetDlgItem(IDC_LISTCTRL_REF)->GetSafeHwnd())
	{
		if(pMsg->message == WM_RBUTTONDOWN)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			CListCtrl		*po_LRef;
			CPoint			o_Pt;
			LVHITTESTINFO	st_HitTest;
			int				i, j, k, i_Res;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			o_Pt.x = LOWORD(pMsg->lParam);
			o_Pt.y = HIWORD(pMsg->lParam);
			po_LRef = (CListCtrl *) GetDlgItem(IDC_LISTCTRL_REF);

			st_HitTest.pt = o_Pt;
			st_HitTest.pt.x = 2;
			i = po_LRef->HitTest(&st_HitTest);
			if(i != -1)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				EMEN_cl_SubMenu o_Menu(FALSE);
				POINT			pt;
				EBRO_cl_Frame	*po_Browser;
				ULONG			ul_Index;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				k = -1;
				while((j = po_LRef->GetNextItem(k, LVNI_SELECTED)) != -1)
				{
					po_LRef->SetItemState(j, 0, 0);
					k = j;
				}

				po_LRef->SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				
				/*$off*/
				M_MF()->InitPopupMenuAction(NULL, &o_Menu);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, EPFB_asz_String[EPFB_STRING_RefMenuRemoveRef], -1);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, EPFB_asz_String[EPFB_STRING_MenuShowInBrowser], -1 );
				if(mpo_Editor->mpst_Prefab->dst_Ref[i].uc_Type == OBJPREFAB_C_RefIsPrefab)
					M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, EPFB_asz_String[EPFB_STRING_RefMenuOpenPrefab], -1 );
				
				po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
				if( po_Browser && !po_Browser->mst_Ini.b_EngineMode && M_MF()->mst_Ini.b_LinkControlON )
				{
					/*$off*/
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, 0, TRUE, "Data Control" );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuCheckout, TRUE, EPFB_asz_String[EPFB_STRING_MenuCheckout], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuLocalCheckout, TRUE, EPFB_asz_String[EPFB_STRING_MenuLocalCheckout], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuUndoCheckout, TRUE, EPFB_asz_String[EPFB_STRING_MenuUndoCheckout], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuGetLVersion, TRUE, EPFB_asz_String[EPFB_STRING_MenuGetLVersion], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuCheckIn, TRUE, EPFB_asz_String[EPFB_STRING_MenuCheckIn], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuCheckInOut, TRUE, EPFB_asz_String[EPFB_STRING_MenuCheckInOut], -1 );
					/*$on*/
				}
				/*$on*/

				GetCursorPos(&pt);
				i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
				switch(i_Res)
				{
				case 1:
					Prefab_RemoveRef(mpo_Editor->mpst_Prefab, i);
					mpo_Editor->mi_PrefabModif = 1;
					Display(mpo_Editor->mpst_Prefab);
					break;
				case 2:
					ul_Index = mpo_Editor->mpst_Prefab->dst_Ref[i].ul_Index;
					if(ul_Index != BIG_C_InvalidIndex)
						po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ul_Index), ul_Index);
					break;
				case 3:
					ul_Index = mpo_Editor->mpst_Prefab->dst_Ref[i].ul_Index;
					if(ul_Index != BIG_C_InvalidIndex) mpo_Editor->mpo_TreeView->SelFile(ul_Index);
					break;			
				case EPFB_STRING_MenuCheckout:
					mpo_Editor->CommonVss(2);
					break;
				case EPFB_STRING_MenuLocalCheckout:
					mpo_Editor->CommonVss(6);
					break;
				case EPFB_STRING_MenuUndoCheckout:
					mpo_Editor->CommonVss(3);
					break;
				case EPFB_STRING_MenuGetLVersion:
					mpo_Editor->CommonVss(4);
					break;
				case EPFB_STRING_MenuCheckIn:
						EDI_gb_CheckInDel = FALSE;
					mpo_Editor->CommonVss(1);
					break;
				case EPFB_STRING_MenuCheckInOut:
					EDI_gb_CheckInDel = FALSE;
					mpo_Editor->CommonVss(5);
					break;
				}

				return TRUE;
			}
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_View::OnSize(UINT nType, int cx, int cy)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int x1, x2, y1, y2, y3, y4, y5, y6;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!GetDlgItem(IDC_EDIT_COMMENT)) return;
	
	GetDlgItem(IDC_STATIC_IMAGEZONE)->ShowWindow( SW_HIDE );
	GetDlgItem(IDC_EDIT_COMMENT)->ShowWindow( mi_DisplayComment );
	
	mpo_Editor->mst_Ini.i_VerticalLayout = 1;
	if (mpo_Editor->mst_Ini.i_VerticalLayout )
	{
		if (cy < 300) 
			cy = 300;
		
		y1 = 5;
		y2 = 30;
		if (mi_DisplayComment)
		{
			if (mi_DisplayPreview)
			{
				y3 = 48;
				y4 = 105;
				y5 = 200;
			}
			else
			{
				y3 = 48;
				y5 = 105;
			}
		}
		else
		{
			if (mi_DisplayPreview)
			{
				y4 = 48;
				y5 = 150;
			}
			else
			{
				y5 = 50;
			}
		}
		
		y6 = cy - 5;
		
		GetDlgItem(IDC_BUTTON_SAVE)->MoveWindow(5, y1, 90, 20, TRUE);
		GetDlgItem(IDC_BUTTON_TYPE)->MoveWindow(100, y1, 90, 20, TRUE);
		GetDlgItem(IDC_BUTTON_OFFSET)->MoveWindow( 195, y1, cx - 200, 20, TRUE);
		
		GetDlgItem(IDC_CHECK_COMMENT )->MoveWindow( 5, y2, 70, 15, TRUE ); 
		GetDlgItem(IDC_CHECK_PREVIEW )->MoveWindow( 80, y2, 70, 15, TRUE );
		
		if (mi_DisplayComment)
			GetDlgItem(IDC_EDIT_COMMENT)->MoveWindow(5, y3, cx - 10, 60, TRUE ); 
		
		if (mi_DisplayPreview)
			GetDlgItem(IDC_STATIC_IMAGEZONE)->MoveWindow(5, y4, cx - 10, 90, TRUE ); 
		
		
		GetDlgItem(IDC_LISTCTRL_REF)->MoveWindow( 5, y5, cx - 10, y6 - y5, TRUE);
	}
	else
	{
		x1 = 5;
		x2 = (cx - 100);

		y1 = 5;
		y2 = 25;
		y3 = (cy - 10) / 3;
		y4 = y3 + 20;
		y5 = cy - 5;

		//GetDlgItem(IDC_STATIC_COMMENT)->MoveWindow(x1, y1 + 2, 46, 16, TRUE);
		GetDlgItem(IDC_EDIT_COMMENT)->MoveWindow(x1, y2, cx - 10, y3 - y2 - 5, TRUE);

		//GetDlgItem(IDC_STATIC_REF)->MoveWindow(x1, y3 + 2, 100, 16, TRUE);
		GetDlgItem(IDC_LISTCTRL_REF)->MoveWindow(x1, y4, (x2 - x1), y5 - y4, TRUE);

		GetDlgItem(IDC_BUTTON_SAVE)->MoveWindow(x2 + 5, y4, 90, 20, TRUE);
		GetDlgItem(IDC_BUTTON_TYPE)->MoveWindow(x2 + 5, y4 + 25, 90, 20, TRUE);
		GetDlgItem(IDC_BUTTON_OFFSET)->MoveWindow(x2 + 5, y4 + 50, 90, 70, TRUE);
	}

	CFormView::OnSize(nType, cx, cy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_View::OnPaint()
{
	CDC					*pDC, dc;
	CBitmap             *po_Tmp;
	CRect				o_Rect;
	float				dx, dy;
	int					x,y,cx,cy;
	
	CFormView::OnPaint();
	
	if (!mi_DisplayPreview)
		return;
	
	GetDlgItem( IDC_STATIC_IMAGEZONE )->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	if ( (o_Rect.Width() == 0) || (o_Rect.Height() == 0))
		return;
	
	pDC = GetDC();
	pDC->FillSolidRect(o_Rect, GetSysColor(COLOR_3DSHADOW) );
	
	if ( mi_Preview )
	{
		// added paranoid checks... ~hogsy

		int rw = o_Rect.Width();
		if ( rw <= 0 ) rw = 1;
		int rh = o_Rect.Height();
		if ( rh <= 0 ) rh = 1;

		dx = ((float) mi_PreviewWidth) / (float)rw;
		dy = ((float) mi_PreviewHeight) / (float)rh;
		
		if ( dx >= dy )
		{
			x = 0;
			cx = rw;
			cy = (int) (mi_PreviewHeight * (1/dx));
			y = (rh - cy) / 2;
		}
		else
		{
			y = 0;
			cy = rh;
			cx = (int) (mi_PreviewWidth * (1/dy));
			x = (rw - cx) / 2;
		}
		x += o_Rect.left;
		y += o_Rect.top;
		
		dc.CreateCompatibleDC(pDC);
		po_Tmp = (CBitmap *) dc.SelectObject( mh_PreviewBitmap );
		if ( po_Tmp != nullptr )
		{
			pDC->SetStretchBltMode( HALFTONE );
			pDC->StretchBlt( x, y, cx, cy, &dc, 0, 0, mi_PreviewWidth, mi_PreviewHeight, SRCCOPY );
			dc.SelectObject( po_Tmp );
		}
	}
	else
	{
		pDC->SetTextColor(0xFF);
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText("No image", -1, &o_Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	}
	
	ReleaseDC( pDC );
	
}


/*
 =======================================================================================================================
    called when comment change
 =======================================================================================================================
 */
void EPFB_cl_View::OnRButtonDown( UINT nFlags, CPoint o_Pt )
{
	CRect	o_Rect;
	POINT	pt;
	int		i_Res;
	char	sz_SaveSnapName[ 255 ];
	
	if (!mpo_Editor->mpst_Prefab)
		return;	
	if (!mi_DisplayPreview)
		return;
	
	GetDlgItem( IDC_STATIC_IMAGEZONE )->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	if (o_Rect.PtInRect( o_Pt ) )
	{
		EMEN_cl_SubMenu			o_Menu(FALSE);
		EOUT_cl_Frame			*po_3DV;
		GDI_tdst_DisplayData	*pst_DD;
		char					*sz_Ext;
		
		po_3DV = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		if (!po_3DV) return;
		
		pst_DD = po_3DV->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData;
		
		
		M_MF()->InitPopupMenuAction(NULL, &o_Menu);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Capture preview", -1);

		GetCursorPos(&pt);
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		if (i_Res == 1)
		{
			L_strcpy( sz_SaveSnapName, pst_DD->sz_SnapshotName );
			
			BIG_ComputeFullName(BIG_ParentFile( mpo_Editor->mpst_Prefab->ul_Index ), pst_DD->sz_SnapshotPath );
			L_strcpy( pst_DD->sz_SnapshotName, BIG_NameFile( mpo_Editor->mpst_Prefab->ul_Index) );
			sz_Ext = L_strrchr( pst_DD->sz_SnapshotName, '.' );
			if ( !sz_Ext )
				L_strcat( pst_DD->sz_SnapshotName, ".tga" );
			else
				L_strcpy( sz_Ext, ".tga" );
				
			pst_DD->uc_SnapshotFlag |= GDI_Cc_CaptureFlag_Running;
			pst_DD->uc_SnapshotFlag |= GDI_Cc_CaptureFlag_SaveInBf;
			LINK_Refresh();
			pst_DD->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_Running;
			pst_DD->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_SaveInBf;
			
			L_strcpy( pst_DD->sz_SnapshotName, sz_SaveSnapName );
			CreatePreview( mpo_Editor->mpst_Prefab, TRUE );
			Invalidate();
		}
	}
}
/*
 =======================================================================================================================
    called when comment change
 =======================================================================================================================
 */
void EPFB_cl_View::OnCommentChange(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Prefab *P;
	int				i_Size;
	/*~~~~~~~~~~~~~~~~~~~*/

	i_Size = GetDlgItem(IDC_EDIT_COMMENT)->GetWindowTextLength();

	if((P = mpo_Editor->mpst_Prefab) != NULL)
	{
		if(P->sz_Comment) L_free(P->sz_Comment);
		P->sz_Comment = NULL;
		if(i_Size)
		{
			P->sz_Comment = (char *) L_malloc(i_Size + 1);
			GetDlgItem(IDC_EDIT_COMMENT)->GetWindowText(P->sz_Comment, i_Size + 1);
		}

		if(!mpo_Editor->mi_PrefabModif)
		{
			mpo_Editor->mi_PrefabModif = 1;
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}
}

/*
 =======================================================================================================================
    called when user click on type button ( changing type )
 =======================================================================================================================
 */
void EPFB_cl_View::OnButtonType(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	POINT			pt;
	int				i_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Editor || !mpo_Editor->mpst_Prefab) return;

	M_MF()->InitPopupMenuAction(NULL, &o_Menu);

	for(i_Res = OBJPREFAB_C_AllRef; i_Res < OBJPREFAB_C_RandomUser; i_Res++)
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, i_Res + 1, TRUE, EPFB_asz_String[EPFB_STRING_TypeAll + i_Res], -1);

	GetCursorPos(&pt);
	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	i_Res--;
	if((i_Res >= OBJPREFAB_C_AllRef) && (i_Res <= OBJPREFAB_C_RandomUser))
	{
		if(mpo_Editor->mpst_Prefab->l_Type == i_Res) return;
		mpo_Editor->mpst_Prefab->l_Type = i_Res;
		mpo_Editor->mi_PrefabModif = 1;
		Display(mpo_Editor->mpst_Prefab);
	}
}

/*
 =======================================================================================================================
    called when user click on offset button
 =======================================================================================================================
 */
void EPFB_cl_View::OnButtonOffset(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_VectorDialog	o_Dlg(3);
	MATH_tdst_Vector		*V;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Editor || !mpo_Editor->mpst_Prefab) return;

	V = &mpo_Editor->mpst_Prefab->st_Offset;
	MATH_CopyVector(&o_Dlg.mst_Vector, (MATH_tdst_Vector *) V);
	if(o_Dlg.DoModal() == IDOK)
	{
		if(MATH_b_EqVector((MATH_tdst_Vector *) V, &o_Dlg.mst_Vector)) return;

		MATH_CopyVector((MATH_tdst_Vector *) V, &o_Dlg.mst_Vector);
		mpo_Editor->mi_PrefabModif = 1;
		Display(mpo_Editor->mpst_Prefab);
	}
}

/*
 =======================================================================================================================
    called when user click on offset button
 =======================================================================================================================
 */
void EPFB_cl_View::OnButtonSave(void)
{
	if(mpo_Editor->mpst_Prefab)
	{
		Prefab_ul_Save(mpo_Editor->mpst_Prefab);
		mpo_Editor->mi_PrefabModif = 0;
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	}
}

/*
 =======================================================================================================================
    called when user click on offset button
 =======================================================================================================================
 */
 void EPFB_cl_View::OnButtonComment()
 {
	CRect o_Rect;
	
	GetClientRect( &o_Rect );
	mi_DisplayComment = ((CButton *) GetDlgItem( IDC_CHECK_COMMENT ))->GetCheck();
	OnSize( SIZE_RESTORED, o_Rect.Width(), o_Rect.Height() );
	Invalidate();
 }
 
 /*
 =======================================================================================================================
    called when user click on offset button
 =======================================================================================================================
 */
 void EPFB_cl_View::OnButtonPreview()
 {
	CRect o_Rect;
	
	GetClientRect( &o_Rect );
	mi_DisplayPreview = ((CButton *) GetDlgItem( IDC_CHECK_PREVIEW ))->GetCheck();
	OnSize( SIZE_RESTORED, o_Rect.Width(), o_Rect.Height() );
 }

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
 
/*
 =======================================================================================================================
    display data of a prefab
 =======================================================================================================================
 */
void EPFB_cl_View::CreatePreview( OBJ_tdst_Prefab *_pst_Prefab, BOOL _b_Force )
{
	char						asz_Name[ BIG_C_MaxLenName ];
	char						*sz_Ext;
	MAIEDITEX_tdst_BitmapDes	*pst_Des; 
	ULONG						ulLength;
	void						*p_Buf;
	
	if (mh_PreviewBitmap)
	{
		DeleteObject(mh_PreviewBitmap);
		mh_PreviewBitmap = NULL;
	}
	mi_Preview = 0;
	
	if ( ( !_pst_Prefab ) || (_pst_Prefab->ul_Index == BIG_C_InvalidIndex) )return;
	
	L_strcpy( asz_Name, BIG_NameFile( _pst_Prefab->ul_Index) );
	sz_Ext = L_strrchr( asz_Name, '.' );
	if ( !sz_Ext )
		L_strcat( asz_Name, ".tga" );
	else
		L_strcpy( sz_Ext, ".tga" );
		
	mul_PreviewIndex = BIG_ul_SearchFile( BIG_ParentFile(_pst_Prefab->ul_Index), asz_Name );
	if (mul_PreviewIndex != BIG_C_InvalidIndex )
	{
		p_Buf = (void *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(mul_PreviewIndex), &ulLength);
		pst_Des = TEXUtil_ConvertTGA( this, mul_PreviewIndex, (UCHAR *) p_Buf);
		L_free(p_Buf);
		
		if ( pst_Des )
		{
			mi_Preview = 1;
			mh_PreviewBitmap = pst_Des->st_Bmp.h_Bitmap;
			mi_PreviewWidth = pst_Des->st_Bmp.st_Header.biWidth;
			mi_PreviewHeight = pst_Des->st_Bmp.st_Header.biHeight;
			if (pst_Des->st_Bmp.h_BitmapAlpha) 
				DeleteObject(pst_Des->st_Bmp.h_BitmapAlpha);
			delete pst_Des;
		}
	}
}

/*
 =======================================================================================================================
    display data of a prefab
 =======================================================================================================================
 */
void EPFB_cl_View::Display(OBJ_tdst_Prefab *_pst_Prefab)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i, j, i_Img;
	CListCtrl	*po_LRef;
	CEdit		*po_Comment;
	CButton		*po_Save, *po_Type, *po_Offset;
	char		sz_Text[260];
	MATH_tdst_Vector	*Min, *Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	/* recompute BV of prefab */
	Prefab_b_ComputeBV( _pst_Prefab, FALSE );

	/* init */
	po_Save = (CButton *) GetDlgItem(IDC_BUTTON_SAVE);
	po_LRef = (CListCtrl *) GetDlgItem(IDC_LISTCTRL_REF);
	po_Comment = (CEdit *) GetDlgItem(IDC_EDIT_COMMENT);
	po_Type = (CButton *) GetDlgItem(IDC_BUTTON_TYPE);
	po_Offset = (CButton *) GetDlgItem(IDC_BUTTON_OFFSET);

	po_LRef->DeleteAllItems();
	po_Comment->SetWindowText("");
	po_Type->SetWindowText("");
	po_Offset->SetWindowText("");
	po_Save->EnableWindow(FALSE);

	/* current prefab ? */
	if(_pst_Prefab)
	{
		/* add prefab to history */
		M_MF()->AddHistoryFile(mpo_Editor, BIG_FileKey(_pst_Prefab->ul_Index));

		/* comment */
		po_Comment->SetWindowText(_pst_Prefab->sz_Comment);

		/* list of ref */
		for(i = 0; i < _pst_Prefab->l_NbRef; i++)
		{
			if(_pst_Prefab->dst_Ref[i].uc_Type == OBJPREFAB_C_RefIsGao)
				i_Img = EDI_IMAGE_GAO;
			else if(_pst_Prefab->dst_Ref[i].uc_Type == OBJPREFAB_C_RefIsPrefab)
				i_Img = EDI_IMAGE_GRP;
			else
				i_Img = EDI_IMAGE_TRASHCAN;

			if ( _pst_Prefab->dst_Ref[i].ul_Index == BIG_C_InvalidIndex )
				j = po_LRef->InsertItem(i, "<ERROR>", i_Img);
			else
			{
				j = po_LRef->InsertItem(i, BIG_NameFile(_pst_Prefab->dst_Ref[i].ul_Index), i_Img);
				BIG_ComputeFullName(BIG_ParentFile(_pst_Prefab->dst_Ref[i].ul_Index), sz_Text);
				po_LRef->SetItemText(j, 1, sz_Text);
				
				Min = &_pst_Prefab->dst_Ref[ i ].st_BVMin;
				Max = &_pst_Prefab->dst_Ref[ i ].st_BVMax;
				sprintf( sz_Text, "(%.2f, %.2f, %.2f) - (%.2f, %.2f, %.2f)", Min->x, Min->y, Min->z, Max->x, Max->y, Max->z );
				po_LRef->SetItemText(j, 2, sz_Text);
			}
		}
		
		/* preview */
		CreatePreview( _pst_Prefab, FALSE );
		Invalidate();

		/* type */
		sprintf
		(
			sz_Text,
			"%s%s",
			EPFB_asz_String[EPFB_STRING_TypeTitle],
			EPFB_asz_String[EPFB_STRING_TypeAll + _pst_Prefab->l_Type]
		);
		po_Type->SetWindowText(sz_Text);

		/* offset */
		sprintf
		(
			sz_Text,
			"%s (%.5f %.5f %.5f)",
			EPFB_asz_String[EPFB_STRING_OffsetTitle],
			_pst_Prefab->st_Offset.x,
			_pst_Prefab->st_Offset.y,
			_pst_Prefab->st_Offset.z
		);
		po_Offset->SetWindowText(sz_Text);

		if(mpo_Editor->mi_PrefabModif) po_Save->EnableWindow();
	}
}

#endif /* ACTIVE_EDITORS */

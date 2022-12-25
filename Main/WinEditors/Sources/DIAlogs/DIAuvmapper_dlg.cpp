/*$T DIAuvmapper_dlg.cpp GC! 1.081 10/28/02 15:01:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "Res/Res.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGdefs.h"
#include "LINKs/LINKtoed.h"
#include "LINks/LINKmsg.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "EDIpaths.h"

#include "Engine/Sources/Engloop.h"
#include "Engine/Sources/MODifier/MDFmodifier_GEO.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "Engine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORmain.h"

#include "SOFT/SOFTpickingbuffer.h"
#include "SOFT/SOFTHelper.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXeditorfct.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEOobjectaccess.h"

#include "DIAlogs/DIAuvmapper_dlg.h"
#include "DIALOGS/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview_undo.h"
#include "EDImainframe.h"
#include "EDIapp.h"
#include "EDIeditors_infos.h"
#include "Editors/Sources/OUTput/OUTframe.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"

#if defined(_XENON_RENDER)
#include "GraphicDK/Sources/GEOmetric/GEOXenonPack.h"
#endif

/*$4
 ***********************************************************************************************************************
    C calling
 ***********************************************************************************************************************
 */

extern void EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);

// We save the dialog options to restore them when we reopen the dialog
static EDIA_cl_UVMapperDialog* EDIA_g_pst_SavedDialog = NULL;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog_Update(ULONG ul_Dlg)
{
	((EDIA_cl_UVMapperDialog *) ul_Dlg)->UpdateUV();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG EDIA_cl_UVMapperDialog_Request(ULONG ul_Dlg, ULONG _ul_Request)
{
	/*~~~~~~~~~~~~~~~*/
	CButton *po_Button;
	/*~~~~~~~~~~~~~~~*/

	if(_ul_Request == 1)
	{
		po_Button = (CButton *) ((EDIA_cl_UVMapperDialog *) ul_Dlg)->GetDlgItem(IDC_CHECK_HIDESUBOBJECT);
		return(po_Button->GetCheck() ? 1 : 0);
	}

	if(_ul_Request == 2)
	{
		((EDIA_cl_UVMapperDialog *) ul_Dlg)->DestroyWindow();
		return 0;
	}

	return 0;
}

/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

#define UV_itof(xi, yi, xf, yf) \
	{ \
		xf = mf_X0 + (float) (xi - mo_MapRect.left) * (mf_ScaleX / mo_MapRect.Width()); \
		yf = mf_Y1 + (float) (mo_MapRect.bottom - yi) * (mf_ScaleY / mo_MapRect.Height()); \
	}

#define UV_ftoi(xf, yf, xi, yi) \
	{ \
		xi = mo_MapRect.left + (int) ((xf - mf_X0) * (mo_MapRect.Width() / mf_ScaleX)); \
		yi = mo_MapRect.bottom - (int) ((yf - mf_Y1) * (mo_MapRect.Height() / mf_ScaleY)); \
	}

/*$4
 ***********************************************************************************************************************
    Message map / init
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_UVMapperDialog, EDIA_cl_BaseDialog)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_PAINT()
	//ON_WM_DESTROY()
	ON_WM_MBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_MAT, OnChangeTexture)
	ON_BN_CLICKED(IDC_CHECK_ALPHA, OnRedraw)
	ON_BN_CLICKED(IDC_CHECK_SHOWGRID, OnRedraw)
	ON_BN_CLICKED(IDC_BUTTON_ZOOMALL, OnZoomAll)
	ON_BN_CLICKED(IDC_BUTTON_UVGIZMO, OnUVGizmo)
	ON_BN_CLICKED(IDC_BUTTON_WELDTHRESH, OnWeldThresh)
	ON_BN_CLICKED(IDC_CHECK_UVTRANSFORM, OnUVTransform)
	ON_BN_CLICKED(IDC_CHECK_HIDESUBOBJECT, OnRefresh)
	ON_BN_CLICKED(IDC_BUTTON_SHOWDATAEX, OnShowDataEx )
	ON_BN_CLICKED(IDC_BUTTON_HIDEDATAEX, OnHideDataEx )
	ON_BN_CLICKED(IDC_CHECK_LOCKRATIO, OnKeepRatio )
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_UVMapperDialog::EDIA_cl_UVMapperDialog(EDIA_cl_ToolBox_GROView *_po_GROView) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_UVMAPPER)
{
	mpo_GroView = _po_GROView;

	for(mi_NbGO = 0; mi_NbGO < 4; mi_NbGO++)
	{
		mpst_GO[mi_NbGO] = NULL;
		mpst_Gro[mi_NbGO] = NULL;
		mpst_UV[mi_NbGO] = NULL;
		
		mpul_Lines[mi_NbGO] = NULL;
		mul_NbLines[mi_NbGO] = mul_NbLinesMax[mi_NbGO] = 0;
		ml_NbUV[mi_NbGO] = 0;
        mpst_SaveUV[mi_NbGO] = NULL;
	}

	mi_NbGO = 0;
	mpst_Mat = NULL;
	mpst_MatUsed = NULL;
	mpst_MTLused = NULL;

	ml_NbTotalUV = 0;

	mh_BitmapRGB = NULL;
	mh_BitmapA = NULL;
	mb_MapBmpInit = 0;

	mi_Capture = 0;
	mi_Pickable = -1;
	
	mb_Transform = FALSE;
	mb_UseTransform = FALSE;
	mb_KeepRatio = FALSE;
	mi_KeepRatioType = 0;
	mb_FeepRatio_XisRef = 1;
	
	mf_X0 = mf_Y0 = 0;
	mf_Scale = 1;
	UpdateScale();
	mf_Y1 = 1 - mf_Y0 - mf_ScaleY;

	mi_Helper = 0;
	mi_HelperVisible = 0;
	mi_HelperPick = 0;
	mf_HelperX = mf_HelperY = mf_HelperRot = 0.0f;

	mi_ColorConfig = 0;

	mpst_3DHelpers = _po_GROView->mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers;
	mpst_EditOptions = _po_GROView->mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_EditOptions;
	
	mi_GridWidth = 1;
	mi_DisplayGrid = 0;
    mi_DisplayOrigin = 0;
    
    mi_BtMax = -1;
    mi_BtOffset = 0;
    
    mi_Undo_StackType = -1;
    mpo_Undo_stack = NULL;

    // Restore dialog data
    if (EDIA_g_pst_SavedDialog)
    {
        mb_UseTransform = EDIA_g_pst_SavedDialog->mb_UseTransform;

        mb_KeepRatio = EDIA_g_pst_SavedDialog->mb_KeepRatio;
        mi_KeepRatioType = EDIA_g_pst_SavedDialog->mi_KeepRatioType;
        mf_RatioToKeep = EDIA_g_pst_SavedDialog->mf_RatioToKeep;
        mb_FeepRatio_XisRef = EDIA_g_pst_SavedDialog->mb_FeepRatio_XisRef;
	
    	mi_GridWidth = EDIA_g_pst_SavedDialog->mi_GridWidth;
	    mi_DisplayGrid = EDIA_g_pst_SavedDialog->mi_DisplayGrid;
        mi_DisplayOrigin = EDIA_g_pst_SavedDialog->mi_DisplayOrigin;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_UVMapperDialog::~EDIA_cl_UVMapperDialog(void)
{
	/*~~*/
	int i;
	/*~~*/

    // Save dialog data
    if (!EDIA_g_pst_SavedDialog)
        EDIA_g_pst_SavedDialog = (EDIA_cl_UVMapperDialog *)MEM_p_Alloc(sizeof(EDIA_cl_UVMapperDialog));
    L_memcpy(EDIA_g_pst_SavedDialog,this,sizeof(EDIA_cl_UVMapperDialog));

	if(mpo_GroView)
	{
		RECT	st_Rect;
		GetWindowRect( &st_Rect );
		mpo_GroView->mpst_EditOption->i_UVMapper_X = st_Rect.left;
		mpo_GroView->mpst_EditOption->i_UVMapper_Y = st_Rect.top;
		mpo_GroView->mpst_EditOption->i_UVMapper_W = st_Rect.right - st_Rect.left;
		mpo_GroView->mpst_EditOption->i_UVMapper_H = st_Rect.bottom - st_Rect.top;
		mpo_GroView->mpo_UVMapper = NULL;
		mpo_GroView->UpdateControls();
	}

	for(i = 0; i < mi_NbGO; i++)
	{
		if(mpst_Gro[i] && mpst_Gro[i]->pst_SubObject)
		{
			mpst_Gro[i]->pst_SubObject->pfn_UVMapper_Update = NULL;
			mpst_Gro[i]->pst_SubObject->pfnl_UVMapper_Request = NULL;
			mpst_Gro[i]->pst_SubObject->pc_UVMapper_Sel = NULL;
		}

		if(mpst_UV[i])
		{
			L_free(mpst_UV[i]);
			mpst_UV[i] = NULL;
		}
	}

	SOFT_Helpers_UVGizmoOff(mpst_3DHelpers);

	APP_go_ModelessNoRef.RemoveAt(APP_go_ModelessNoRef.Find(GetSafeHwnd()));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_UVMapperDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			sz_Value[16];
	CDC				*pDC;
	HICON			hIcon = NULL;
	int				i_GizmoType;	
	CRect			o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_STATIC_MAP)->ShowWindow(SW_HIDE);
	UpdateGao();

	GetDlgItem(IDC_STATIC_MAP)->GetWindowRect(&mo_MapRect);
	ScreenToClient(&mo_MapRect);

	// ----( set icon )----
	hIcon = (HICON) ::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UVMAPPER_GRID), IMAGE_ICON, 16, 16, 0);
	((CButton *) GetDlgItem( IDC_CHECK_SHOWGRID ))->SetIcon( hIcon );
    ((CButton *) GetDlgItem( IDC_CHECK_SHOWGRID ))->SetCheck(mi_DisplayGrid);
	hIcon = (HICON) ::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UVMAPPER_ALPHA), IMAGE_ICON, 16, 16, 0);
	((CButton *) GetDlgItem( IDC_CHECK_ALPHA ))->SetIcon( hIcon );
	hIcon = (HICON) ::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UVMAPPER_ZOOM), IMAGE_ICON, 16, 16, 0);
	((CButton *) GetDlgItem( IDC_BUTTON_ZOOMALL ))->SetIcon( hIcon );
	hIcon = (HICON) ::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UVMAPPER_LOCKRATIO), IMAGE_ICON, 16, 16, 0);
	((CButton *) GetDlgItem( IDC_CHECK_LOCKRATIO ))->SetIcon( hIcon );
	((CButton *) GetDlgItem( IDC_CHECK_LOCKRATIO ) )->SetCheck( mb_KeepRatio );
	hIcon = (HICON) ::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UVMAPPER_USEUVTRANSFORM), IMAGE_ICON, 16, 16, 0);
	((CButton *) GetDlgItem( IDC_CHECK_UVTRANSFORM ))->SetIcon( hIcon );
	hIcon = (HICON) ::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UVMAPPER_HIDESUBOBJECT), IMAGE_ICON, 16, 16, 0);
	((CButton *) GetDlgItem( IDC_CHECK_HIDESUBOBJECT ))->SetIcon( hIcon );

	sprintf(sz_Value, "%.4f", mpst_EditOptions->f_UVWeldThresh);
	GetDlgItem(IDC_EDIT_WELDTHRESH)->SetWindowText(sz_Value);

	APP_go_ModelessNoRef.AddTail(GetSafeHwnd());

	pDC = GetDC();
	mo_MapDC.CreateCompatibleDC(pDC);

	EnableToolTips(TRUE);

    // Use mb_UseTransform.
    ((CButton *) GetDlgItem(IDC_CHECK_UVTRANSFORM))->SetCheck(mb_UseTransform);
    SetUVTransform();
    UV_TransformIn();
    InvalidateRect(NULL);
	
	hIconGizmo[ GRO_Cul_EOUVF_GizmoPlanar ]		= (HICON) ::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UVMAPPER_GIZMOPLANAR ), IMAGE_ICON, 16, 16, 0);
	hIconGizmo[ GRO_Cul_EOUVF_GizmoCylindrical ]= (HICON) ::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UVMAPPER_GIZMOCYLINDRE ), IMAGE_ICON, 16, 16, 0);
	hIconGizmo[ GRO_Cul_EOUVF_GizmoBox ]		= (HICON) ::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UVMAPPER_GIZMOBOX ), IMAGE_ICON, 16, 16, 0);
	hIconGizmo[ GRO_Cul_EOUVF_GizmoSpherical ]	= (HICON) ::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UVMAPPER_GIZMOSPHERE ), IMAGE_ICON, 16, 16, 0);
	
	i_GizmoType = mpst_EditOptions->ul_UVMapperFlags & GRO_Cul_EOUVF_GizmoTypeMask;
	if ( (i_GizmoType < 0) || (i_GizmoType > GRO_Cul_EOUVF_GizmoSpherical) )
	{
		mpst_EditOptions->ul_UVMapperFlags &= ~GRO_Cul_EOUVF_GizmoTypeMask;
		mpst_EditOptions->ul_UVMapperFlags |= GRO_Cul_EOUVF_GizmoPlanar;
		i_GizmoType = 0;
	}
	((CButton *) GetDlgItem( IDC_BUTTON_UVGIZMO ))->SetIcon( hIconGizmo[ i_GizmoType ] );
	
	mb_ShowAdditionalData = TRUE;
	RecalcLayout( 0 );
	
	if (mpo_GroView && (mpo_GroView->mpst_EditOption->i_UVMapper_W != 0 ) )
		SetWindowPos( NULL, mpo_GroView->mpst_EditOption->i_UVMapper_X, mpo_GroView->mpst_EditOption->i_UVMapper_Y, mpo_GroView->mpst_EditOption->i_UVMapper_W, mpo_GroView->mpst_EditOption->i_UVMapper_H, SWP_NOZORDER );
	else
		CenterWindow();
	
    // Zoom on area of interest at init
    OnZoomAll();
	
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnDestroy(void)
{
	if (mpo_GroView)
	{
		RECT	st_Rect;
		GetWindowRect( &st_Rect );
		mpo_GroView->mpst_EditOption->i_UVMapper_X = st_Rect.left;
		mpo_GroView->mpst_EditOption->i_UVMapper_Y = st_Rect.top;
		mpo_GroView->mpst_EditOption->i_UVMapper_W = st_Rect.right - st_Rect.left;
		mpo_GroView->mpst_EditOption->i_UVMapper_H = st_Rect.bottom - st_Rect.top;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnWindowPosChanged(WINDOWPOS* lpwndpos )
{
	int	i_SaveOffset;
	
	EDIA_cl_BaseDialog::OnWindowPosChanged( lpwndpos );
	if (mi_BtMax == -1) return;
	
	if ( ! (lpwndpos->flags & SWP_NOMOVE ) )
	{
		i_SaveOffset = mi_BtOffset;
		mi_BtOffset = -(lpwndpos->x + mi_BtMin);
		if (mi_BtOffset < 0) mi_BtOffset = 0;
		if (mi_BtOffset > mi_BtMax - mi_BtMin) mi_BtOffset = mi_BtMax - mi_BtMin;
		if (mi_BtOffset != i_SaveOffset)
			RecalcLayout( 0 );
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_UVMapperDialog::PreTranslateMessage(MSG *pMsg)
{
	/*~~~~~~*/
	int			go, i;
	float		dx, dy;
	/*~~~~~~*/

	if(pMsg->message == WM_MOUSEWHEEL)
	{
		if(WheelZoom((short) HIWORD(pMsg->wParam))) return 1;
	}
	else if(pMsg->message == WM_MOUSEMOVE)
	{
		/*$1- tool tips ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_HIDEDATAEX)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("close left pannel", 500);
			return 0;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_SHOWDATAEX)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("open left pannel", 500);
			return 0;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_CHECK_ALPHA)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("display / hide alpha channel", 500);
			return 0;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_ZOOMALL)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("zoom", 500);
			return 0;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_CHECK_LOCKRATIO)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("lock texture ratio", 500);
			return 0;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_CHECK_SHOWGRID)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("grid", 500);
			return 0;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_UVGIZMO)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("geometric mapping", 500);
			return 0;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_CHECK_UVTRANSFORM)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("use material UV transformation", 500);
			return 0;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_CHECK_HIDESUBOBJECT)->GetSafeHwnd())
		{
			EDI_Tooltip_DisplayMessage("Hide / Show sub object in 3DView", 500);
			return 0;
		}
	}
	else if(pMsg->message == WM_KEYDOWN)
	{
		if((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_SPACE)) return 1;

		/* undo */
		if(
			(pMsg->wParam == 'Z') && (GetAsyncKeyState(VK_CONTROL) < 0) ||
			(pMsg->wParam == 'Z') && (GetAsyncKeyState(VK_SHIFT) < 0)
			)
		{
			mpo_GroView->mpo_ToolBox->mpo_View->mpo_AssociatedEditor->OnAction(EOUT_ACTION_UNDO);
			LINK_Refresh();
			return 1;
		}

		/* redo */
		if(
			(pMsg->wParam == 'Y') && (GetAsyncKeyState(VK_CONTROL) < 0) ||
			(pMsg->wParam == 'Y') && (GetAsyncKeyState(VK_SHIFT) < 0)
		)
		{
			mpo_GroView->mpo_ToolBox->mpo_View->mpo_AssociatedEditor->OnAction(EOUT_ACTION_REDO);
			LINK_Refresh();
			return 1;
		}

		/* select all */
		if(
			(pMsg->wParam == 'A') && (GetAsyncKeyState(VK_CONTROL) < 0) ||
			(pMsg->wParam == 'A') && (GetAsyncKeyState(VK_SHIFT) < 0)
		)
		{
			for(go = 0; go < mi_NbGO; go++)
			{
				for(i = 0; i < ml_NbUV[go]; i++)
					if(mpst_UV[go][i].c_Show) mpst_UV[go][i].c_Sel = 1;
			}

			InvalidateRect(NULL, FALSE);
			return 1;
		}

		/* detach */
		if(pMsg->wParam == 'D')
		{
			UV_Detach();
			return 1;
		}

		/* weld */
		if(pMsg->wParam == 'W')
		{
			if(GetAsyncKeyState(VK_SHIFT) < 0)
				OnWeldThresh();
			else
				UV_WeldThresh(1000);
			return 1;
		}

		/* color config */
		if(pMsg->wParam == '1')
		{
			mi_ColorConfig = 0;
			InvalidateRect(NULL, FALSE);
			return 1;
		}

		if(pMsg->wParam == '2')
		{
			mi_ColorConfig = 1;
			InvalidateRect(NULL, FALSE);
			return 1;
		}
		
		if ( (pMsg->wParam == VK_UP ) || (pMsg->wParam == VK_DOWN ) || (pMsg->wParam == VK_RIGHT ) || (pMsg->wParam == VK_LEFT ) )
		{
			dx = dy = 0.0f;
			i = 1;
			switch (pMsg->wParam)
			{
			case VK_UP: dy = -1.0f; break;
			case VK_DOWN: dy = 1.0f; break;
			case VK_RIGHT: dx = 1.0f; break;
			case VK_LEFT : dx = -1.0f; break;
			}
			if (GetAsyncKeyState( VK_CONTROL ) < 0 )
			{
				i = 0;
			}
			else if (GetAsyncKeyState( VK_SHIFT) < 0)
			{
				dx *= 10;
				dy *= 10;
			}
			UV_Move( i, dx, dy);
			return 1;
		}
		
		

		return 0;
	}
	else if(pMsg->message == WM_RBUTTONDOWN)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EMEN_cl_SubMenu o_Menu(FALSE);
		POINT			pt;
		char			sz_Text[ 256 ];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_UVGIZMO)->GetSafeHwnd())
		{
			M_MF()->InitPopupMenuAction(NULL, &o_Menu);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Planar", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Cylindrical", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Box", -1);
			//M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Spherical", -1);

			GetCursorPos(&pt);
			i = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
			if ((i> 0) && (i <= 4) )
			{
				i--;
				mpst_EditOptions->ul_UVMapperFlags &= ~GRO_Cul_EOUVF_GizmoTypeMask;
				mpst_EditOptions->ul_UVMapperFlags |= i;
				((CButton *) GetDlgItem( IDC_BUTTON_UVGIZMO ))->SetIcon( hIconGizmo[ i ] );
			}
		}
		else if (pMsg->hwnd == GetDlgItem( IDC_BUTTON_ZOOMALL )->GetSafeHwnd() )
		{
			M_MF()->InitPopupMenuAction(NULL, &o_Menu);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Zoom all", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Zoom all with origin", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Zoom to (0,0)-(1,1)", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Move to (0,0)", -1);

			GetCursorPos(&pt);
			i = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
			switch (i )
			{
			case 1: OnZoomAll();	break;
			case 2: DoZoom(1);		break;
			case 3: OnReset();		break;
			case 4: OnGotoOrigin(); break;
			}
		}
		else if (pMsg->hwnd == GetDlgItem( IDC_CHECK_UVTRANSFORM )->GetSafeHwnd() )
		{
			M_MF()->InitPopupMenuAction(NULL, &o_Menu);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Use UV Transform", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Update UV transform", -1);

			GetCursorPos(&pt);
			i = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
			switch (i )
			{
			case 1: OnUVTransform();		break;
			case 2: OnUpdateUVTransform();	break;
			}
		}
		else if (pMsg->hwnd == GetDlgItem( IDC_CHECK_SHOWGRID )->GetSafeHwnd() )
		{
			sprintf( sz_Text, "Set grid width %d", mi_GridWidth );
			M_MF()->InitPopupMenuAction(NULL, &o_Menu);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Show grid",  mi_DisplayGrid ? DFCS_CHECKED : 0);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Show origin", mi_DisplayOrigin ? DFCS_CHECKED : 0);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, sz_Text, -1);
			
			GetCursorPos(&pt);
			i = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
			switch ( i )
			{
			case 1: 
				mi_DisplayGrid = !mi_DisplayGrid; 
				((CButton *) GetDlgItem( IDC_CHECK_SHOWGRID ))->SetCheck(mi_DisplayGrid);
				OnRedraw();		
				break;
			case 2: mi_DisplayOrigin = !mi_DisplayOrigin; OnRedraw(); break;
			case 3: 
				{
					EDIA_cl_NameDialog	o_Dlg("Enter grid size");
					sprintf( sz_Text, "%d",  mi_GridWidth );
					o_Dlg.mo_Name = sz_Text;
					if(o_Dlg.DoModal() == IDOK)
					{
						sscanf((char *) (LPCSTR) o_Dlg.mo_Name, "%u", &mi_GridWidth);
						if (mi_GridWidth < 1) mi_GridWidth = 1;
						if (mi_GridWidth > 32) mi_GridWidth = 32;
					}
					OnRedraw();
				}
			}
			return 1;
		}
		else if (pMsg->hwnd == GetDlgItem( IDC_CHECK_LOCKRATIO )->GetSafeHwnd() )
		{
			M_MF()->InitPopupMenuAction(NULL, &o_Menu);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Keep ratio - texture",  DFCS_BUTTONRADIO | ((mb_KeepRatio && (mi_KeepRatioType == 0)) ? DFCS_CHECKED : 0) );
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Keep ratio - current",  DFCS_BUTTONRADIO | ((mb_KeepRatio && (mi_KeepRatioType == 1)) ? DFCS_CHECKED : 0) );
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Keep ratio - 1 / 1",  DFCS_BUTTONRADIO | ((mb_KeepRatio && (mi_KeepRatioType == 2)) ? DFCS_CHECKED : 0) );
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "");
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "X is ref", DFCS_BUTTONRADIO | (mb_FeepRatio_XisRef ? DFCS_CHECKED : 0) );
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 5, TRUE, "Y is ref", DFCS_BUTTONRADIO | (mb_FeepRatio_XisRef ? 0 : DFCS_CHECKED) );
			
			GetCursorPos(&pt);
			i = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
			switch ( i )
			{
			case 1: 
				if (mb_KeepRatio && (mi_KeepRatioType == 0))
					mb_KeepRatio = FALSE;
				else
				{
					mb_KeepRatio = TRUE;
					mi_KeepRatioType = 0;
					mf_RatioToKeep = ((float) mi_BmpHeight)/ ((float) mi_BmpWidth);
					UpdateScale();
					mf_Y1 = 1 - mf_Y0 - mf_ScaleY;
					OnRedraw();
				}
				break;
			case 2:
				if (mb_KeepRatio && (mi_KeepRatioType == 1))
					mb_KeepRatio = FALSE;
				else
				{
					mb_KeepRatio = TRUE;
					mi_KeepRatioType = 1;
					mf_RatioToKeep = ((float) mo_MapRect.Height() ) / ((float) mo_MapRect.Width() );
					UpdateScale();
					mf_Y1 = 1 - mf_Y0 - mf_ScaleY;
					OnRedraw();
				}
				break;
			case 3:
				if (mb_KeepRatio && (mi_KeepRatioType == 2))
					mb_KeepRatio = FALSE;
				else
				{
					mb_KeepRatio = TRUE;
					mi_KeepRatioType = 2;
					mf_RatioToKeep = 1.0f;
					UpdateScale();
					mf_Y1 = 1 - mf_Y0 - mf_ScaleY;
					OnRedraw();
				}
				break;
			case 4:
				mb_FeepRatio_XisRef = TRUE;
				break;
			case 5:
				mb_FeepRatio_XisRef = FALSE;
				break;
			}
			((CButton *) GetDlgItem( IDC_CHECK_LOCKRATIO ) )->SetCheck( mb_KeepRatio );
		}

	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*$4
 ***********************************************************************************************************************
    messages handles
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnSize(UINT n, int x, int y)
{
	if (!GetDlgItem(IDC_TREE_MAT)) return;
	EDIA_cl_BaseDialog::OnSize(n, x, y);
	RecalcLayout(0);
	UpdateScale();
	mf_Y1 = 1 - mf_Y0 - mf_ScaleY;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 600;
	lpMMI->ptMinTrackSize.y = 300;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UpdateMapBmp(void)
{
	/*~~~~~~~~~~~~~~*/
	CDC		*pDC, oDC;
	HBITMAP h_Bmp;
	CBitmap *oldBmp;
	float	w, h;
	float	x, y;
	int		i, j, n, nx, ny;
	int		*XYWH;
	/*~~~~~~~~~~~~~~*/

	if(mb_MapBmpInit)
	{
		mo_MapDC.SelectObject((CBitmap *) NULL);
		mo_MapBmp.DeleteObject();
		mb_MapBmpInit = 0;
	}

	if((((CButton *) GetDlgItem(IDC_CHECK_ALPHA))->GetCheck()) && mh_BitmapA)
		h_Bmp = mh_BitmapA;
	else
		h_Bmp = mh_BitmapRGB;

	if(!h_Bmp) return;

	if(mf_Scale < 1.5) return;
	
	nx = (int) (ceil(mf_ScaleX) + 1);
	ny = (int) (ceil(mf_ScaleY) + 1);
	w = ((float) mo_MapRect.Width() / mf_ScaleX );
	h = ((float) mo_MapRect.Height() / mf_ScaleY );
	x = nx * w;
	y = ny * h;

	pDC = GetDC();

	mo_MapBmp.CreateCompatibleBitmap(pDC, (int) x, (int) y);
	mo_MapDC.SelectObject(&mo_MapBmp);
	mo_MapDC.SetStretchBltMode(HALFTONE);

	oDC.CreateCompatibleDC(pDC);
	oldBmp = (CBitmap *) oDC.SelectObject(h_Bmp);

	n = ((nx > ny) ? nx : ny);
	XYWH = (int *) L_malloc( n * 16);
	XYWH[0] = 0;
	XYWH[1] = 0;
	for(i = 1; i < n; i++)
	{
		XYWH[i * 4 + 0] = (int) (i * w);
		XYWH[i * 4 + 1] = (int) (i * h);
		XYWH[(i - 1) * 4 + 2] = XYWH[i * 4 + 0] - XYWH[(i - 1) * 4 + 0];
		XYWH[(i - 1) * 4 + 3] = XYWH[i * 4 + 1] - XYWH[(i - 1) * 4 + 1];
		if(XYWH[(i - 1) * 4 + 2] > XYWH[2]) XYWH[2] = XYWH[(i - 1) * 4 + 2];
		if(XYWH[(i - 1) * 4 + 3] > XYWH[3]) XYWH[3] = XYWH[(i - 1) * 4 + 3];
	}

	XYWH[(n - 1) * 4 + 2] = (int) x - XYWH[(i - 1) * 4 + 0];
	XYWH[(n - 1) * 4 + 3] = (int) y - XYWH[(i - 1) * 4 + 1];

	for(i = 0; i < nx; i++)
	{
		for(j = 0; j < ny; j++)
		{
			if(i + j == 0)
			{
				mo_MapDC.StretchBlt(0, 0, XYWH[2], XYWH[3], &oDC, 0, 0, mi_BmpWidth, mi_BmpHeight, SRCCOPY);
			}
			else
			{
				mo_MapDC.BitBlt
					(
						XYWH[i * 4],
						XYWH[j * 4 + 1],
						XYWH[i * 4 + 2],
						XYWH[j * 4 + 3],
						&mo_MapDC,
						0,
						0,
						SRCCOPY
					);
			}
		}
	}

	L_free(XYWH);

	oDC.SelectObject(oldBmp);
	mb_MapBmpInit = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UpdateScale( void )
{
	float	sw, sh;
	
	mf_ScaleX = mf_ScaleY = mf_Scale;
	if ( mb_KeepRatio )
	{
		sw = (float) mo_MapRect.Width();
		sh = (float) mo_MapRect.Height();
		if (mb_FeepRatio_XisRef)
			mf_ScaleY = (sh / sw) * (mf_ScaleX / mf_RatioToKeep);
		else
			mf_ScaleX = (sw / sh) * (mf_ScaleY * mf_RatioToKeep);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC			*pDC, oDC, oDC2;
	CRgn		o_Region;
	CPen		o_PtBorder, o_Grid, o_Line, o_Helper1, o_Helper2, *po_OldPen;
	CBrush		o_Brush, o_Brush2, *po_OldBrush;
	LOGBRUSH	st_Brush;
	HBITMAP		h_Bmp;
	int			r[2], x, x2, y, y2;
	int			i, i2, j, go;
	CBitmap		o_Bitmap;
	float		xmin, xmax, ymin, ymax, w, h;
	float		fx, fy, flimitx, flimity;
	float		sx, sy, sw, sh;
	static int	test = 0;
	ULONG		ul_C0, ul_C1, ul_C2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnPaint();

	pDC = GetDC();
	po_OldPen = NULL;

	ul_C0 = mpst_EditOptions->ul_UVMapperColor[mi_ColorConfig][GRO_UVMapper_ColorPoint];
	ul_C1 = mpst_EditOptions->ul_UVMapperColor[mi_ColorConfig][GRO_UVMapper_ColorSelPoint];
	ul_C2 = (ul_C0 ^ 0xFFFFFF) & 0xFFFFFF;
	o_PtBorder.CreatePen(PS_SOLID, 1, ul_C2);
	o_Grid.CreatePen(PS_SOLID, 1, mpst_EditOptions->ul_UVMapperColor[mi_ColorConfig][GRO_UVMapper_ColorGrid]);
	if
	(
		mpst_EditOptions->ul_UVMapperColor[mi_ColorConfig][GRO_UVMapper_ColorLines1] != mpst_EditOptions->
			ul_UVMapperColor[mi_ColorConfig][GRO_UVMapper_ColorLines2]
	)
	{
		pDC->SetBkColor(mpst_EditOptions->ul_UVMapperColor[mi_ColorConfig][GRO_UVMapper_ColorLines1]);
		o_Line.CreatePen(PS_DOT, 1, mpst_EditOptions->ul_UVMapperColor[mi_ColorConfig][GRO_UVMapper_ColorLines2]);
	}
	else
	{
		o_Line.CreatePen(PS_SOLID, 1, mpst_EditOptions->ul_UVMapperColor[mi_ColorConfig][GRO_UVMapper_ColorLines1]);
	}

	o_Helper1.CreatePen(PS_SOLID, 1, mpst_EditOptions->ul_UVMapperColor[mi_ColorConfig][GRO_UVMapper_ColorHelper1]);
	o_Helper2.CreatePen(PS_SOLID, 1, mpst_EditOptions->ul_UVMapperColor[mi_ColorConfig][GRO_UVMapper_ColorHelper2]);
	po_OldPen = pDC->SelectObject(&o_Grid);

	o_Region.CreateRectRgn(mo_MapRect.left, mo_MapRect.top, mo_MapRect.right, mo_MapRect.bottom);
	pDC->SelectClipRgn(&o_Region);

	/* display texture */
	if((((CButton *) GetDlgItem(IDC_CHECK_ALPHA))->GetCheck()) && mh_BitmapA)
		h_Bmp = mh_BitmapA;
	else
		h_Bmp = mh_BitmapRGB;

	if(mb_MapBmpInit)
	{
		x = (int) ((mf_X0 - (float) floor(mf_X0)) * (mo_MapRect.Width() / mf_ScaleX));
		y = (int) ((mf_Y0 - (float) floor(mf_Y0)) * (mo_MapRect.Height() / mf_ScaleY));
		pDC->BitBlt(mo_MapRect.left, mo_MapRect.top, mo_MapRect.Width(), mo_MapRect.Height(), &mo_MapDC, x, y, SRCCOPY);
	}
	else if(h_Bmp)
	{
		oDC.CreateCompatibleDC(pDC);
		oDC.SelectObject(h_Bmp);
		pDC->SetStretchBltMode(HALFTONE);

		sw = mo_MapRect.Width() / mf_ScaleX;
		flimitx = mf_X0 + mf_ScaleX;
		sh = mo_MapRect.Height() / mf_ScaleY;
		flimity = mf_Y0 + mf_ScaleY;
		
		fy = (float) floor(mf_Y0);
		while(1)
		{
			sy = (sh * (fy - mf_Y0));
			ymin = 0;
			ymax = (float) mi_BmpHeight;
			h = sh;
			while(sy + (sh / 32.0f) < 0)
			{
				sy += sh / 32.0f;
				h -= sh / 32.0f;
				ymin += mi_BmpHeight / 32.0f;
			}

			while((sy + h - (sh / 32.0f)) > mo_MapRect.Height())
			{
				h -= sh / 32.0f;
				ymax -= mi_BmpHeight / 32.0f;
			}

			fx = (float) floor(mf_X0);
			while(1)
			{
				sx = (sw * (fx - mf_X0));
				xmin = 0;
				xmax = (float) mi_BmpWidth;
				w = sw;
				while(sx + (sw / 32.0f) < 0)
				{
					sx += sw / 32.0f;
					w -= sw / 32.0f;
					xmin += mi_BmpWidth / 32.0f;
				}

				while((sx + w - (sw / 32.0f)) > mo_MapRect.Width())
				{
					w -= sw / 32.0f;
					xmax -= mi_BmpWidth / 32.0f;
				}

				pDC->StretchBlt
					(
						(int) sx + mo_MapRect.left,
						(int) sy + mo_MapRect.top,
						(int) w,
						(int) h,
						&oDC,
						(int) xmin,
						(int) ymin,
						(int) (xmax - xmin),
						(int) (ymax - ymin),
						SRCCOPY
					);
				fx += 1;
				if(fx >= flimitx) break;
			}

			fy += 1;
			if(fy >= flimity) break;
		}
	}
	else
	{
		pDC->FillSolidRect(&mo_MapRect, 0x7F4F00);
	}

	/* display grid */
	if( mi_DisplayGrid )
	{
		i2 = mi_GridWidth;
		if(i2 == 0)
			i2 = 1;

		fy = (float) ceil(mf_Y0 * i2);
		while(fy < (mf_Y0 + mf_ScaleY) * i2)
		{
			y = mo_MapRect.top + (int) (((fy / i2) - mf_Y0) * (mo_MapRect.Height() / mf_ScaleY));
			pDC->MoveTo(mo_MapRect.left, y);
			pDC->LineTo(mo_MapRect.right, y);
			fy += 1;
		}

		fx = (float) ceil(mf_X0 * i2);
		while(fx < (mf_X0 + mf_ScaleX) * i2)
		{
			x = mo_MapRect.left + (int) (((fx / i2) - mf_X0) * (mo_MapRect.Width() / mf_ScaleX));
			pDC->MoveTo(x, mo_MapRect.top);
			pDC->LineTo(x, mo_MapRect.bottom);
			fx += 1;
		}
	}

	/* display origin */
	if( mi_DisplayOrigin )
	{
		UV_ftoi(0, 0, i, j);
		pDC->Ellipse(i - 3, j - 3, i + 4, j + 4);
	}

	for(go = 0; go < mi_NbGO; go++)
	{
		if( ml_NbUV[go] )
		{
			/* compute coords */
			for(i = 0; i < ml_NbUV[ go ]; i++)
			{
				if(!mpst_UV[ go ][i].c_Show) continue;
				UV_ftoi(mpst_UV[ go ][i].u, mpst_UV[ go ][i].v, mpst_UV[ go ][i].x, mpst_UV[ go ][i].y);
			}

			/* Display lines */
			pDC->SelectObject(&o_Line);
			for(i = 0; i < ml_NbUV[go]; i++)
			{
				if(mpul_Lines[go][i] == 0xFFFFFFFF) continue;
				j = i;
				while(1)
				{
					pDC->MoveTo(*(POINT *) &mpst_UV[go][i].x);
					pDC->LineTo(*(POINT *) &mpst_UV[go][mpul_Lines[go][j] & 0xFFFF].x);
					if((mpul_Lines[go][j] & 0xFFFF0000) == 0xFFFF0000) break;
					j = mpul_Lines[go][j] >> 16;
				}
			}

			pDC->SelectObject(&o_PtBorder);

			/* display UV point */
			for(i = 0; i < ml_NbUV[go]; i++)
			{
				mpst_UV[go][i].c_Visible = 0;
				if(!mpst_UV[go][i].c_Show) continue;
				if(!mo_MapRect.PtInRect(*((POINT *) &mpst_UV[go][i].x))) continue;
				pDC->MoveTo(mpst_UV[go][i].x - 2, mpst_UV[go][i].y - 2);
				pDC->LineTo(mpst_UV[go][i].x + 2, mpst_UV[go][i].y - 2);
				pDC->LineTo(mpst_UV[go][i].x + 2, mpst_UV[go][i].y + 2);
				pDC->LineTo(mpst_UV[go][i].x - 2, mpst_UV[go][i].y + 2);
				pDC->LineTo(mpst_UV[go][i].x - 2, mpst_UV[go][i].y - 2);
				pDC->FillSolidRect(mpst_UV[go][i].x - 1, mpst_UV[go][i].y - 1, 3, 3, mpst_UV[go][i].c_Sel ? ul_C1 : ul_C0);
				mpst_UV[go][i].c_Visible = 1;
			}
		}
	}

	/* display helper */
	if(mi_Helper != 0)
	{
		mi_HelperVisible = 0;
		UV_ftoi(mf_HelperX, mf_HelperY, mi_HelperX, mi_HelperY);

		st_Brush.lbColor = 0;
		st_Brush.lbHatch = 0;
		st_Brush.lbStyle = BS_NULL;
		o_Brush.CreateBrushIndirect(&st_Brush);
		po_OldBrush = pDC->SelectObject(&o_Brush);

		mo_MapRect.InflateRect(40, 40);
		if(mo_MapRect.PtInRect(*(POINT *) &mi_HelperX))
		{
			mi_HelperVisible = 1;

			/* center */
			if((mi_Capture != 4) || (mi_HelperPick == 1))
			{
				pDC->SelectObject(&o_Helper2);
				pDC->Ellipse(mi_HelperX - 1, mi_HelperY - 1, mi_HelperX + 2, mi_HelperY + 2);
				pDC->SelectObject(&o_Helper1);
				pDC->Ellipse(mi_HelperX - 2, mi_HelperY - 2, mi_HelperX + 3, mi_HelperY + 3);
				pDC->SelectObject(&o_Helper2);
				pDC->Ellipse(mi_HelperX - 3, mi_HelperY - 3, mi_HelperX + 4, mi_HelperY + 4);
			}

			if(mi_Helper == 1)
			{
				/* rotation */
				r[0] = 40;
				r[1] = 0;
			}
			else
			{
				/* scale */
				r[0] = 15;
				r[1] = 30;

				if(mi_Capture == 4)
				{
					if(mi_HelperPick == 3)
					{
						r[0] = (int) mf_HelperEnd;
						r[1] = 0;
					}
					else if(mi_HelperPick == 4)
					{
						r[0] = 0;
						r[1] = 0;

						x = mi_HelperX + (int) (mf_HelperStart * fOptCos(mf_HelperRot));
						y = mi_HelperY + (int) (mf_HelperStart * fOptSin(mf_HelperRot));
						pDC->SelectObject(&o_Grid);
						pDC->MoveTo(mi_HelperX, mi_HelperY);
						pDC->LineTo(x, y);
						pDC->SelectObject(&o_Helper1);
						pDC->Ellipse(x - 3, y - 3, x + 4, y + 4);

						x = mi_HelperX + (int) (mf_HelperEnd * fOptCos(mf_HelperRot));
						y = mi_HelperY + (int) (mf_HelperEnd * fOptSin(mf_HelperRot));
						pDC->SelectObject(&o_Grid);
						pDC->MoveTo(mi_HelperX, mi_HelperY);
						pDC->LineTo(x, y);
						pDC->SelectObject(&o_Helper2);
						pDC->Ellipse(x - 3, y - 3, x + 4, y + 4);
					}
				}
				else
				{
					x = mi_HelperX + 30;
					y = mi_HelperY;

					pDC->SelectObject(&o_Helper2);
					pDC->Ellipse(x - 3, y - 3, x + 4, y + 4);
					pDC->SelectObject(&o_Helper1);
					pDC->Ellipse(x - 2, y - 2, x + 5, y + 5);

					x = mi_HelperX;
					y = mi_HelperY + 30;

					pDC->SelectObject(&o_Helper2);
					pDC->Ellipse(x - 3, y - 3, x + 4, y + 4);
					pDC->SelectObject(&o_Helper1);
					pDC->Ellipse(x - 2, y - 2, x + 5, y + 5);
				}
			}

			/* display circles */
			for(i = 0; i < 2; i++)
			{
				if(r[i] == 0) break;

				pDC->SelectObject(&o_Helper2);
				pDC->Ellipse(mi_HelperX - r[i], mi_HelperY - r[i], mi_HelperX + r[i] + 1, mi_HelperY + r[i] + 1);
				pDC->SelectObject(&o_Helper1);
				pDC->Ellipse
					(
						mi_HelperX - r[i] + 1,
						mi_HelperY - r[i] + 1,
						mi_HelperX + r[i] + 2,
						mi_HelperY + r[i] + 2
					);
			}

			if((mi_Capture == 4) && (mi_HelperPick == 2))
			{
				x = (int) (r[0] * fOptCos(mf_HelperStart));
				y = (int) (r[0] * fOptSin(mf_HelperStart));
				x2 = (int) (r[0] * fOptCos(mf_HelperEnd));
				y2 = (int) (r[0] * fOptSin(mf_HelperEnd));

				i = x * y2 - y * x2;

				x += mi_HelperX;
				y += mi_HelperY;
				x2 += mi_HelperX;
				y2 += mi_HelperY;

				pDC->SelectObject(&o_Helper1);
				pDC->Ellipse(x - 3, y - 3, x + 4, y + 4);
				pDC->SelectObject(&o_Helper2);
				pDC->Ellipse(x2 - 3, y2 - 3, x2 + 4, y2 + 4);

				st_Brush.lbStyle = BS_HOLLOW;
				o_Brush2.CreateBrushIndirect(&st_Brush);
				pDC->SelectObject(&o_Brush2);

				if(i <= 0)
					pDC->Pie(mi_HelperX - r[0], mi_HelperY - r[0], mi_HelperX + r[0], mi_HelperY + r[0], x, y, x2, y2);
				else
					pDC->Pie(mi_HelperX - r[0], mi_HelperY - r[0], mi_HelperX + r[0], mi_HelperY + r[0], x2, y2, x, y);
			}
		}

		pDC->SelectObject(&o_Grid);
		pDC->SelectObject(po_OldBrush);
		mo_MapRect.DeflateRect(40, 40);
	}

	pDC->SelectObject(po_OldPen);
	pDC->SelectClipRgn(NULL);
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnLButtonDown(UINT n_Flags, CPoint o_Pt)
{
	if(!mo_MapRect.PtInRect(o_Pt)) return;

	if(GetAsyncKeyState(VK_SPACE) < 0)
	{
		mi_Capture = 1;
	}
	else
	{
		if(Helper_Pick(o_Pt))
		{
			if(mi_HelperPick > 1) /* scale & rotation */
				UV_Save();

			mi_Capture = 4;
		}
		else
		{
			if(mi_Pickable != -1)
			{
				mi_Capture = 5;
			}
			else
			{
				mi_Capture = 3;
				mo_SelRect.TopLeft() = o_Pt;
				mo_SelRect.BottomRight() = o_Pt;
				SelRect_Draw();
			}
		}
	}

	mo_CapturePt = o_Pt;
	SetCapture();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnLButtonUp(UINT n_Flags, CPoint o_Pt)
{
	if(mi_Capture == 5)
	{
		if(mi_Pickable != -1)
		{
			if(n_Flags & MK_CONTROL)
			{
				if(n_Flags & MK_SHIFT)
					mpst_UV[mi_PickableGO][mi_Pickable].c_Sel = 1 - mpst_UV[mi_PickableGO][mi_Pickable].c_Sel;
				else
					mpst_UV[mi_PickableGO][mi_Pickable].c_Sel = 1;
			}
			else if(n_Flags & MK_SHIFT)
			{
				mpst_UV[mi_PickableGO][mi_Pickable].c_Sel = 0;
			}
			else
			{
				UVSelNone();
				mpst_UV[mi_PickableGO][mi_Pickable].c_Sel = 1;
			}

			LINK_Refresh();
		}
	}

	if(mi_Capture == 3)
	{
		SelRect_Draw();

		if(!(n_Flags & MK_CONTROL) && !(n_Flags & MK_SHIFT)) UVSelNone();
		SelRect_Sel();
		LINK_Refresh();
	}

	UV_PushUndo( FALSE, 0 );

	if(mi_Capture)
	{
		ReleaseCapture();
		mi_Capture = 0;
	}

	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnLButtonDblClk(UINT n_Flags, CPoint o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Face;
	char								c_Loop;
	int									i, j, go;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Pickable == -1) return;
	mpst_UV[mi_PickableGO][mi_Pickable].c_Sel |= 2;

	c_Loop = 1;
	while(c_Loop)
	{
		c_Loop = 0;

		for(go = 0; go < mi_NbGO; go++)
		{
			for(i = 0; i < (int) mpst_Gro[go]->pst_SubObject->ul_NbFaces; i++)
			{
				if ( !(mpst_Gro[go]->pst_SubObject->dc_FSel[i] & 1) )
					continue;
				
				pst_Element = mpst_Gro[go]->dst_Element + mpst_Gro[go]->pst_SubObject->dst_Faces[i].uw_Element;
				pst_Face = pst_Element->dst_Triangle + mpst_Gro[go]->pst_SubObject->dst_Faces[i].uw_Index;
				
				j =  ( mpst_UV[ go ][pst_Face->auw_UV[0]].c_Sel & 2 ) ? 1 : 0;
				j += ( mpst_UV[ go ][pst_Face->auw_UV[1]].c_Sel & 2 ) ? 1 : 0;
				j += ( mpst_UV[ go ][pst_Face->auw_UV[2]].c_Sel & 2 ) ? 1 : 0;
				if ( j && (j!=3) )
				{
					c_Loop = 1;
					mpst_UV[go][pst_Face->auw_UV[0]].c_Sel |= 2;
					mpst_UV[go][pst_Face->auw_UV[1]].c_Sel |= 2;
					mpst_UV[go][pst_Face->auw_UV[2]].c_Sel |= 2;
				}
			}
		
			/*
			pst_Element = mpst_Gro[go]->dst_Element;
			for(i = 0; i < mpst_Gro[go]->l_NbElements; i++, pst_Element++)
			{
				pst_Face = pst_Element->dst_Triangle;
				for(j = 0; j < pst_Element->l_NbTriangles; j++, pst_Face++)
				{
					if(mpst_UV[ go ][pst_Face->auw_UV[0]].c_Sel & 2)
					{
						if(!(mpst_UV[ go ][pst_Face->auw_UV[1]].c_Sel & 2))
						{
							c_Loop = 1;
							mpst_UV[go][pst_Face->auw_UV[1]].c_Sel |= 2;
							mpst_UV[go][pst_Face->auw_UV[2]].c_Sel |= 2;
						}
						else if(!(mpst_UV[go][pst_Face->auw_UV[2]].c_Sel & 2))
						{
							c_Loop = 1;
							mpst_UV[go][pst_Face->auw_UV[2]].c_Sel |= 2;
						}
					}
					else if((mpst_UV[go][pst_Face->auw_UV[1]].c_Sel & 2) || (mpst_UV[go][pst_Face->auw_UV[2]].c_Sel & 2))
					{
						c_Loop = 1;
						mpst_UV[go][pst_Face->auw_UV[0]].c_Sel |= 2;
						mpst_UV[go][pst_Face->auw_UV[1]].c_Sel |= 2;
						mpst_UV[go][pst_Face->auw_UV[2]].c_Sel |= 2;
					}
				}
			}
			*/
		}
	}

	for (go = 0; go < mi_NbGO; go++)
	{
		for(i = 0; i < mpst_Gro[go]->l_NbUVs; i++)
		{
			if((mpst_UV[go][i].c_Sel & 2) && (mpst_UV[go][i].c_Show)) mpst_UV[go][i].c_Sel |= 1;
			mpst_UV[go][i].c_Sel &= ~2;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnRButtonDown(UINT n_Flags, CPoint o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	int				go, i_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Capture)
	{
		if(mi_Capture == 3) SelRect_Draw();

        for (go = 0; go < mi_NbGO; go++)
        {
		    if(mpst_SaveUV[go])
		    {
    			if((mi_Capture >= 2) && (mi_Capture <= 4))
		    		UV_Restore();
			    L_free(mpst_SaveUV[go]);
			    mpst_SaveUV[go] = NULL;
            }
		}

		ReleaseCapture();
		mi_Capture = 0;
		LINK_Refresh();
		InvalidateRect(NULL, FALSE);
	}
	else
	{
		if(!mo_MapRect.PtInRect(o_Pt)) return;

		ClientToScreen(&o_Pt);

		M_MF()->InitPopupMenuAction(NULL, &o_Menu);

		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Flip horizontal", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Flip vertical", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Rotate Pi/2 (90)", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Rotate Pi (180)", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 5, TRUE, "Rotate 3Pi/2 (270)", -1);

		if(UV_CanBeAligned()) M_MF()->AddPopupMenuAction(NULL, &o_Menu, 6, TRUE, "Band alignement", -1);

		i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Pt, &o_Menu);

		if((i_Res >= 1) && (i_Res <= 5))
		{
			UV_Save();
			switch(i_Res)
			{
			case 1:
				mi_HelperStartX = 30;
				mi_HelperStartY = 0;
				mf_HelperRot = 0;
				UV_Zoom(-1, 1);
				break;
			case 2:
				mi_HelperStartX = 0;
				mi_HelperStartY = 30;
				mf_HelperRot = Cf_PiBy2;
				UV_Zoom(-1, 1);
				break;
			case 3:
				UV_Rotate(Cf_PiBy2);
				break;
			case 4:
				UV_Rotate(Cf_Pi);
				break;
			case 5:
				UV_Rotate(Cf_3PiBy2);
				break;
			}

			UV_PushUndo( FALSE, 0 );
			LINK_Refresh();
			InvalidateRect(NULL, FALSE);
		}
		else if(i_Res == 6)
		{
			UV_BandAlign();
			LINK_Refresh();
			InvalidateRect(NULL, FALSE);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnMButtonDown(UINT n_Flags, CPoint o_Pt)
{
	if(!mo_MapRect.PtInRect(o_Pt)) return;

	UV_itof(o_Pt.x, o_Pt.y, mf_HelperX, mf_HelperY);
	if(mi_Helper == 0) mi_Helper = 1;

	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnMouseMove(UINT n_Flags, CPoint o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i, j, go;
	float			dx, dy, dz;
	EDIA_tdst_UV	*uv;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Capture == 5)
	{
		dx = (float) (o_Pt.x - mo_CapturePt.x);
		dy = (float) (o_Pt.y - mo_CapturePt.y);
		if(fabs(dx) + fabs(dy) > 2)
		{
			if(!mpst_UV[mi_PickableGO][mi_Pickable].c_Sel)
			{
				if(!(n_Flags & MK_CONTROL) && !(n_Flags & MK_SHIFT)) UVSelNone();
				mpst_UV[mi_PickableGO][mi_Pickable].c_Sel = 1;
			}

			mo_CapturePt.x = mpst_UV[mi_PickableGO][mi_Pickable].x;
			mo_CapturePt.y = mpst_UV[mi_PickableGO][mi_Pickable].y;
			mi_Capture = 2;
			UV_Save();
		}
		else
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor((mi_Pickable == -1) ? IDC_ARROW : IDC_SIZEALL));
			return;
		}
	}

	mi_Pickable = -1;

	if(mi_Capture == 1)
	{
		mf_X0 -= (float) (o_Pt.x - mo_CapturePt.x) * (mf_ScaleX / (float) mo_MapRect.Width());
		mf_Y0 -= (float) (o_Pt.y - mo_CapturePt.y) * (mf_ScaleY / (float) mo_MapRect.Height());
		mf_Y1 = 1 - mf_Y0 - mf_ScaleY;
		mo_CapturePt = o_Pt;
		InvalidateRect(&mo_MapRect, FALSE);
	}
	else if(mi_Capture == 2)
	{
		/* snap */
		if(GetAsyncKeyState(VK_SHIFT) < 0)
		{
			float	distx, disty;
			
			UV_itof(o_Pt.x, o_Pt.y, dx, dy);
			
			distx = 15 * (mf_ScaleX / mo_MapRect.Width());
			disty = 15 * (mf_ScaleY / mo_MapRect.Height());
			
			i = mi_GridWidth;

			dx *= i;
			dz = MATH_f_FloatRound(dx, 1);
			//if(fAbs(dx - dz) < 0.2f) dx = dz;
			if(fAbs(dx - dz) < distx) dx = dz;
			dx /= i;

			dy *= i;
			dz = MATH_f_FloatRound(dy, 1);
			//if(fAbs(dy - dz) < 0.2f) dy = dz;
			if(fAbs(dy - dz) < disty) dy = dz;
			dy /= i;

			UV_ftoi(dx, dy, o_Pt.x, o_Pt.y);
		}
		
		/* snap to point */
		else if (GetAsyncKeyState( VK_CONTROL ) < 0 )
		{
			int dist, best, bestx, besty;
			
			best = 101;
			
			for(go = 0; go < mi_NbGO; go++)
			{
				for(i = 0; i < ml_NbUV[go]; i++)
				{
					if ( ( mpst_UV[go][i].c_Show ) && (!mpst_UV[go][i].c_Sel ) )
					{
						dist = (mpst_UV[go][i].x - o_Pt.x) * (mpst_UV[go][i].x - o_Pt.x);
						dist += (mpst_UV[go][i].y - o_Pt.y) * (mpst_UV[go][i].y - o_Pt.y);
						if (dist < best)
						{
							best = dist;
							bestx = mpst_UV[go][i].x;
							besty = mpst_UV[go][i].y;
						}
					}
				}
			}
			if (best != 101 )
			{
				o_Pt.x = bestx;
				o_Pt.y = besty;
			}
		}

		dx = (float) (o_Pt.x - mo_CapturePt.x) * (mf_ScaleX / mo_MapRect.Width());
		dy = (float) (o_Pt.y - mo_CapturePt.y) * (mf_ScaleY / mo_MapRect.Height());
		for(go = 0; go < mi_NbGO; go++)
		{
			for(i = 0; i < ml_NbUV[go]; i++)
			{
				if(mpst_UV[go][i].c_Sel)
				{
					mpst_UV[go][i].u += dx;
					mpst_UV[go][i].v -= dy;
				}
			}
		}

		UV_TransformOut();

		mo_CapturePt = o_Pt;
		LINK_Refresh();
		OnPaint();
	}
	else if(mi_Capture == 3)
	{
		SelRect_Draw();
		mo_SelRect.BottomRight() = o_Pt;
		SelRect_Draw();
	}
	else if(mi_Capture == 4)
	{
		if(mi_HelperPick == 1)		/* move center */
		{
			mf_HelperX += (float) (o_Pt.x - mo_CapturePt.x) * (mf_ScaleX / mo_MapRect.Width());
			mf_HelperY -= (float) (o_Pt.y - mo_CapturePt.y) * (mf_ScaleY / mo_MapRect.Height());
			mo_CapturePt = o_Pt;
			InvalidateRect(&mo_MapRect, FALSE);
		}
		else if(mi_HelperPick == 2) /* rotation circle */
		{
			mf_HelperEnd = Helper_PickCircle(o_Pt);
			UV_Rotate(mf_HelperEnd - mf_HelperStart);
			LINK_Refresh();
			OnPaint();
		}
		else if(mi_HelperPick == 3) /* zoom */
		{
			i = o_Pt.x - mi_HelperX;
			j = o_Pt.y - mi_HelperY;
			mf_HelperEnd = fSqrt((float) (i * i + j * j));
			UV_Zoom(mf_HelperEnd / mf_HelperStart, 0);
			LINK_Refresh();
			OnPaint();
		}
		else if(mi_HelperPick == 4) /* axis scale */
		{
			i = o_Pt.x - mi_HelperX;
			j = o_Pt.y - mi_HelperY;
			mf_HelperEnd = (float) (i * mi_HelperStartX + j * mi_HelperStartY) / mf_HelperStart;
			if(!(n_Flags & MK_SHIFT))
				if(mf_HelperEnd < 0) mf_HelperEnd = 0;
			UV_Zoom(mf_HelperEnd / mf_HelperStart, 1);
			LINK_Refresh();
			OnPaint();
		}
	}
	else
	{
		if(Helper_Pick(o_Pt))
		{
			switch(mi_HelperPick)
			{
			case 1:
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
				break;
			case 2:
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_CAMTGT));
				break;
			case 3:
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_CAMZOOM));
				break;
			case 4:
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_CAMZOOM1));
				break;
			}
		}
		else if((ml_NbTotalUV) && (mo_MapRect.PtInRect(o_Pt)))
		{
			for(go = 0; go < mi_NbGO; go++)
			{
				uv = mpst_UV[go];
				for(i = 0; i < ml_NbUV[go]; i++, uv++)
				{
					if(uv->c_Visible)
					{
						if
						(
							(o_Pt.x >= uv->x - 5)
						&&	(o_Pt.x <= uv->x + 5)
						&&	(o_Pt.y >= uv->y - 5)
						&&	(o_Pt.y <= uv->y + 5)
						)
						{
							mi_PickableGO = go;
							mi_Pickable = i;
							break;
						}
					}
				}
			}

			::SetCursor(AfxGetApp()->LoadStandardCursor((mi_Pickable == -1) ? IDC_ARROW : IDC_SIZEALL));

			if(mi_Pickable != -1)
			{
				/*~~~~~~~~~~~~~~~~~*/
				char	sz_Text[100];
				/*~~~~~~~~~~~~~~~~~*/

				uv = mpst_UV [ mi_PickableGO] + mi_Pickable;
				sprintf(sz_Text, "[%d-%d] (%.4f, %.4f)", mi_PickableGO, mi_Pickable, uv->u, uv->v);
				EDI_Tooltip_DisplayMessage(sz_Text, 1);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnChangeTexture(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_TC;
	int			i_Data, i_Image, i_Texture;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NM_TREEVIEW *pNotifyStruct;
	pNotifyStruct = (NM_TREEVIEW *)pNotifyStruct2;
	po_TC = (CTreeCtrl *) GetDlgItem(IDC_TREE_MAT);

	i_Data = po_TC->GetItemData(pNotifyStruct->itemNew.hItem);
	if(i_Data == 0) return;

	po_TC->GetItemImage(pNotifyStruct->itemNew.hItem, i_Image, i_Texture);

	mpst_MatUsed = (MAT_tdst_Material *) i_Data;
	SetMaterial(i_Image);
	InvalidateRect(NULL);

	/* SetTexture(i_Texture); */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnRedraw(void)
{
	mi_DisplayGrid = ((CButton *) GetDlgItem( IDC_CHECK_SHOWGRID ))->GetCheck();
	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnReset(void)
{
	mf_X0 = mf_Y0 = 0;
	mf_Scale = 1;
	UpdateScale();
	mf_Y1 = 1 - mf_Y0 - mf_ScaleY;
	UpdateMapBmp();
	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnGotoOrigin(void)
{
	mf_X0 = mf_Y0 = 0;
	mf_Y1 = 1 - mf_Y0 - mf_ScaleY;
	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::DoZoom( int _i_WithOrigin )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL	b_Zoom;
	float	x, xmin, xmax, y, ymin, ymax;
	int		i, go;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_WithOrigin)
	{
		xmin = ymin = 0;
		xmax = ymax = 0;
	}
	else
	{
		xmin = ymin = Cf_Infinit;
		xmax = ymax = -Cf_Infinit;
	}

	b_Zoom = FALSE;

    for (go = 0; go < mi_NbGO; go++)
    {
	    for(i = 0; i < ml_NbUV[go]; i++)
	    {
		    if(mpst_UV[go][i].c_Show)
		    {
			    b_Zoom = TRUE;

                // On limite les UVs à 1000 pour éviter les plantons dûs à des valeurs délirantes.
                if (fabsf(mpst_UV[go][i].u) > 1000.f || fabsf(mpst_UV[go][i].v) > 1000.f)
                {
                    char sMsg[256];
                    sprintf(sMsg,"(u,v) of vertex %d of GAO %d is too big(%f,%f), limited to 1000.",i,go,mpst_UV[go][i].u,mpst_UV[go][i].v);
                    ERR_X_Warning(0,sMsg,NULL);
                    mpst_UV[go][i].u = MATH_f_FloatLimit(mpst_UV[go][i].u,-1000.f,1000.f);
                    mpst_UV[go][i].v = MATH_f_FloatLimit(mpst_UV[go][i].v,-1000.f,1000.f);
                }

                x = mpst_UV[go][i].u;
			    y = mpst_UV[go][i].v;

			    if(xmin > x) xmin = x;
			    if(xmax < x) xmax = x;
			    if(ymin > y) ymin = y;
			    if(ymax < y) ymax = y;
		    }
        }
	}

	if(b_Zoom)
	{
		if(xmin == xmax)
		{
			xmin -= 0.5f;
			xmax += 0.5f;
		}

		if(ymin == ymax)
		{
			ymin -= 0.5f;
			ymax += 0.5f;
		}

		x = 0.05f * (xmax - xmin);
		xmin -= x;
		xmax += x;

		y = 0.05f * (ymax - ymin);
		ymin -= y;
		ymax += y;

		mf_X0 = xmin;
		mf_Y1 = ymin;
		mf_Scale = ((xmax - xmin) < (ymax - ymin)) ? (ymax - ymin) : (xmax - xmin);
		UpdateScale();
		mf_Y0 = 1 - mf_Y1 - mf_ScaleY;
		UpdateMapBmp();
		InvalidateRect(NULL, FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnZoomAll( void )
{
	//mb_KeepRatio = !mb_KeepRatio;
	//mf_RatioToKeep = 1.0f;
	DoZoom(0);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnWeldThresh(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Value[16];
	/*~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_EDIT_WELDTHRESH)->GetWindowText(sz_Value, 16);
	mpst_EditOptions->f_UVWeldThresh = (float) atof(sz_Value);
	sprintf(sz_Value, "%.4f", mpst_EditOptions->f_UVWeldThresh);
	GetDlgItem(IDC_EDIT_WELDTHRESH)->SetWindowText(sz_Value);

	UV_WeldThresh(mpst_EditOptions->f_UVWeldThresh);

	InvalidateRect(NULL, FALSE);
}

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL UVMapper_b_BuildGOListe(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
    EDIA_cl_UVMapperDialog  *po_Dialog;
    OBJ_tdst_GameObject     *pst_GO;

    po_Dialog = (EDIA_cl_UVMapperDialog *) _ul_Param1;
    if (po_Dialog->mi_NbGO == 4) return FALSE;

    if (!(_pst_Sel->l_Flag & SEL_C_SIF_Object)) return TRUE;
    pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
    if (!pst_GO) return TRUE;

    po_Dialog->mpst_GO[ po_Dialog->mi_NbGO ] = pst_GO;
    po_Dialog->mpst_Gro[ po_Dialog->mi_NbGO ] = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo( pst_GO);
    po_Dialog->mi_NbGO++;
    return TRUE;
}
/**/
void EDIA_cl_UVMapperDialog::UpdateGao(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	char		sz_Name[128];
	char		sz_Value[32];
	CTreeCtrl	*po_TC;
	BOOL		b_Show;
    int         go;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	/* exit UV gizmo mode */
	if(SOFT_b_Helpers_UVGizmoIsOn(mpst_3DHelpers)) OnUVGizmo();


    for (go = 0; go < mi_NbGO; go++)
    {
        if((!mpst_Gro[go]) || (!mpst_Gro[go]->pst_SubObject))
	    {
		    if(ml_NbUV[go])
		    {
			    ml_NbUV[go] = 0;
			    L_free(mpst_UV[go]);
			    mpst_UV[go] = NULL;
		    }

		    if(mul_NbLinesMax[go])
		    {
			    mul_NbLinesMax[go] = 0;
			    mul_NbLines[go] = 0;
			    L_free(mpul_Lines[go]);
			    mpul_Lines[go] = NULL;
		    }
	    }
    }

	if((mpo_GroView) && (mpo_GroView->mpo_ToolBox) && (mpo_GroView->mpo_ToolBox->mpst_Gao))
	{
        if (mpo_GroView->mpo_ToolBox->mpst_Gao->pst_World)
        {
            mi_NbGO = 0;
            SEL_EnumItem( mpo_GroView->mpo_ToolBox->mpst_Gao->pst_World->pst_Selection, SEL_C_SIF_Object, UVMapper_b_BuildGOListe, (ULONG ) this, 0 );
        }
        else
        {
            mi_NbGO = 1;
		    mpst_GO[0] = mpo_GroView->mpo_ToolBox->mpst_Gao;
		    mpst_Gro[0] = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(mpst_GO[ 0 ]);
        }
		mpst_Mat = (MAT_tdst_Material *) OBJ_p_GetGrm( mpst_GO[0] );
	}
	else
	{
        mi_NbGO = 0;
		mpst_GO[0] = NULL;
		mpst_Gro[0] = NULL;
		mpst_Mat = NULL;
	}

	GetDlgItem(IDC_STATIC_GONAME)->SetWindowText(mpst_GO[0] ? mpst_GO[0]->sz_Name : "");
	GetDlgItem(IDC_STATIC_GRONAME)->SetWindowText(mpst_Gro[0] ? mpst_Gro[0]->st_Id.sz_Name : "");
	GetDlgItem(IDC_STATIC_NBELEMENTS)->SetWindowText(mpst_Gro[0] ? _itoa(mpst_Gro[0]->l_NbElements, sz_Value, 10) : "");

	mi_NbSubMat = 0;
	mpst_MatUsed = NULL;

	if(mpst_Mat)
	{
		GetDlgItem(IDC_STATIC_MATNAME)->SetWindowText(mpst_Mat->st_Id.sz_Name);
		if(mpst_Mat->st_Id.i->ul_Type == GRO_MaterialMulti)
		{
			mi_NbSubMat = ((MAT_tdst_Multi *) mpst_Mat)->l_NumberOfSubMaterials;
			GetDlgItem(IDC_STATIC_NBSUBMAT)->SetWindowText(_itoa(mi_NbSubMat, sz_Value, 10));
		}
		else
			GetDlgItem(IDC_STATIC_NBSUBMAT)->SetWindowText("");
	}
	else
	{
		GetDlgItem(IDC_STATIC_MATNAME)->SetWindowText("");
		GetDlgItem(IDC_STATIC_NBSUBMAT)->SetWindowText("");
	}

	po_TC = (CTreeCtrl *) GetDlgItem(IDC_TREE_MAT);
	po_TC->DeleteAllItems();

	if(mpst_Mat)
	{
		if(mpst_Mat->st_Id.i->ul_Type == GRO_MaterialSingle)
			Tree_AddSingle((MAT_tdst_Single *) mpst_Mat, po_TC, TVI_ROOT);
		else if(mpst_Mat->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
			Tree_AddMultiTexture((MAT_tdst_MultiTexture *) mpst_Mat, po_TC, TVI_ROOT);
		else if(mpst_Mat->st_Id.i->ul_Type == GRO_MaterialMulti)
			Tree_AddMulti((MAT_tdst_Multi *) mpst_Mat, po_TC, TVI_ROOT);
	}

    b_Show = FALSE;
    for (go = 0 ; go < mi_NbGO; go++)
    {
	    if(mpst_Gro[ go ])
	    {
		    if(mpst_Gro[go]->pst_SubObject && mpst_Gro[go]->dst_UV)
		    {
    			mpst_Gro[go]->pst_SubObject->pfn_UVMapper_Update = EDIA_cl_UVMapperDialog_Update;
	    		mpst_Gro[go]->pst_SubObject->pfnl_UVMapper_Request = EDIA_cl_UVMapperDialog_Request;
		    	mpst_Gro[go]->pst_SubObject->ul_UVMapper_Param = (ULONG) this;
    			b_Show = TRUE;
		    }
		    else
		    {
			    if(!mpst_Gro[go]->dst_UV)
			    {
				    sprintf(sz_Name, "%s [NO UV]", mpst_Gro[go]->st_Id.sz_Name);
				    GetDlgItem(IDC_STATIC_GRONAME)->SetWindowText(sz_Name);
			    }
	    		mpst_Gro[ go ] = NULL;
		    }
        }
    }

	GetDlgItem(IDC_BUTTON_ZOOMALL)->EnableWindow(b_Show);
	UpdateUV();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UpdateUV_AddLine(int go, USHORT uv0, USHORT uv1)
{
	/*~~*/
	int i;
	/*~~*/

	if(uv0 == uv1) return;
	if(uv0 > uv1)
	{
		UpdateUV_AddLine(go, uv1, uv0);
		return;
	}

	if((mpul_Lines[go][uv0] & 0xFFFF) == 0xFFFF)
		mpul_Lines[go][uv0] = 0xFFFF0000 | uv1;
	else
	{
		i = uv0;
		while(1)
		{
			if((mpul_Lines[go][i] & 0xFFFF) == uv1) return;
			if((mpul_Lines[go][i] >> 16) == 0xFFFF) break;
			i = mpul_Lines[go][i] >> 16;
		}

		mpul_Lines[go][i] &= 0xFFFF;
		mpul_Lines[go][i] |= mul_NbLines[go] << 16;
		mpul_Lines[go][mul_NbLines[go]] = 0xFFFF0000 | uv1;
		mul_NbLines[go]++;
		if(mul_NbLines[go] == mul_NbLinesMax[go])
		{
			mul_NbLinesMax[go] += ml_NbUV[go];
			mpul_Lines[go] = (ULONG *) L_realloc(mpul_Lines[go], mul_NbLinesMax[go] * 4);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UpdateUV(BOOL _b_ChangeMat)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									i, k, go;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Face;
	LONG								al_MatUsed[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ml_NbTotalUV = 0;
    for (go = 0; go < mi_NbGO; go++)
    {
	    if((!mpst_Gro[go]) || (!mpst_Gro[go]->pst_SubObject))
	    {
		    if(ml_NbUV[go])
		    {
			    ml_NbUV[go] = 0;
			    L_free(mpst_UV[go]);
			    mpst_UV[go] = NULL;
		    }

		    if(mul_NbLinesMax[go])
		    {
			    mul_NbLinesMax[go] = 0;
			    mul_NbLines[go] = 0;
			    L_free(mpul_Lines[go]);
			    mpul_Lines[go] = NULL;
		    }
		    continue;
	    }

        ml_NbTotalUV += mpst_Gro[go]->l_NbUVs;

	    if(ml_NbUV[go] != mpst_Gro[go]->l_NbUVs)
	    {
		    if(ml_NbUV[go])
			    mpst_UV[go] = (EDIA_tdst_UV *) L_realloc(mpst_UV[go], mpst_Gro[go]->l_NbUVs * sizeof(EDIA_tdst_UV));
		    else
			    mpst_UV[go] = (EDIA_tdst_UV *) L_malloc(mpst_Gro[go]->l_NbUVs * sizeof(EDIA_tdst_UV));

		    if(mpst_Gro[go]->l_NbUVs > ml_NbUV[go])
			    L_memset(mpst_UV[go] + ml_NbUV[go], 0, (mpst_Gro[go]->l_NbUVs - ml_NbUV[go]) * sizeof(EDIA_tdst_UV));
		    ml_NbUV[go] = mpst_Gro[go]->l_NbUVs;
	    }

	    for(i = 0; i < ml_NbUV[go]; i++)
	    {
		    mpst_UV[go][i].c_Show = 0;
		    mpst_UV[go][i].i_Ref = 0;
	    }

	    L_memset(al_MatUsed, 0, 256);

	    /* prepare pour la liste des lignes */
	    if(mul_NbLinesMax[go]) L_free(mpul_Lines[go]);
	    mul_NbLinesMax[go] = ml_NbUV[go] * 3;
	    mul_NbLines[go] = ml_NbUV[go];
	    mpul_Lines[go] = (ULONG *) L_malloc(4 * mul_NbLinesMax[go]);
	    L_memset(mpul_Lines[go], -1, ml_NbUV[go] * 4);

	    /* scan faces */
	    k = (mi_NbSubMat) ? mi_NbSubMat : 1;
	    for(i = 0; i < (int) mpst_Gro[go]->pst_SubObject->ul_NbFaces; i++)
	    {
		    pst_Element = mpst_Gro[go]->dst_Element + mpst_Gro[go]->pst_SubObject->dst_Faces[i].uw_Element;
		    pst_Face = pst_Element->dst_Triangle + mpst_Gro[go]->pst_SubObject->dst_Faces[i].uw_Index;

		    mpst_UV[go][pst_Face->auw_UV[0]].i_Ref++;
		    mpst_UV[go][pst_Face->auw_UV[1]].i_Ref++;
		    mpst_UV[go][pst_Face->auw_UV[2]].i_Ref++;

		    if(mpst_Gro[go]->pst_SubObject->dc_FSel[i] & 1)
		    {
			    mpst_UV[go][pst_Face->auw_UV[0]].c_Show = 1;
			    mpst_UV[go][pst_Face->auw_UV[1]].c_Show = 1;
			    mpst_UV[go][pst_Face->auw_UV[2]].c_Show = 1;

			    UpdateUV_AddLine(go, pst_Face->auw_UV[0], pst_Face->auw_UV[1]);
			    UpdateUV_AddLine(go, pst_Face->auw_UV[1], pst_Face->auw_UV[2]);
			    UpdateUV_AddLine(go, pst_Face->auw_UV[2], pst_Face->auw_UV[0]);

			    al_MatUsed[pst_Element->l_MaterialId % k]++;
		    }
	    }

	    /* deselect vertex that are no more selected */
	    for(i = 0; i < ml_NbUV[go]; i++)
	    {
		    if(!mpst_UV[go][i].c_Show) mpst_UV[go][i].c_Sel = 0;
	    }

        mpst_Gro[go]->pst_SubObject->pc_UVMapper_Sel = (char *) &mpst_UV[go]->c_Sel;
	    mpst_Gro[go]->pst_SubObject->c_UVMapper_SelInc = sizeof(EDIA_tdst_UV);
    }

	/* sélectionne un materiaux */
	if(_b_ChangeMat)
	{
		if(mi_NbSubMat == 0)
		{
			mpst_MatUsed = mpst_Mat;
			SetMaterial(-1);
		}
		else
		{
			/* regarde si le matériau précédent est toujours utilisé */
			for(i = 0; i < mi_NbSubMat; i++)
			{
				if(((MAT_tdst_Multi *) mpst_Mat)->dpst_SubMaterial[i] == mpst_MatUsed) break;
			}

			/* plus utilisé, trouves t'en un autre */
			if((i == mi_NbSubMat) || !al_MatUsed[i])
			{
				k = 0;
				for(i = 1; i < mi_NbSubMat; i++)
				{
					if(al_MatUsed[i] > al_MatUsed[k]) k = i;
				}

				mpst_MatUsed = ((MAT_tdst_Multi *) mpst_Mat)->dpst_SubMaterial[k];

				/* met à jour le materiau utilisé */
				SetMaterial(-1);
			}
		}
	}
#ifdef JADEFUSION
    UpdateForXenon();
#endif

	UV_TransformIn();
	InvalidateRect(NULL, FALSE);
	LINK_Refresh();
}

#ifdef JADEFUSION
void EDIA_cl_UVMapperDialog::UpdateForXenon(void)
{
#if defined(_XENON_RENDER)
    for (int i = 0; i < mi_NbGO; ++i)
    {
        GEO_PackGameObjectEd(mpst_GO[i], TRUE);
    }
#endif
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UVSelNone(void)
{
	/*~~*/
	int go, i;
	/*~~*/

    for (go = 0; go < mi_NbGO; go++)
	    for(i = 0; i < ml_NbUV[go]; i++) 
            mpst_UV[go][i].c_Sel = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_TransformIn(void)
{
	/*~~*/
	int i, go;
	/*~~*/

	if(mb_Transform && mb_UseTransform)
	{
        for (go = 0; go < mi_NbGO; go++)
		    for(i = 0; i < ml_NbUV[go]; i++)
		    {
			    mpst_UV[go][i].u = mst_UVTransform.UVMatrix[0] * mpst_Gro[go]->dst_UV[i].fU + mst_UVTransform.UVMatrix[2] * mpst_Gro[go]->dst_UV[i].fV + mst_UVTransform.AddU; 
                mpst_UV[go][i].v = mst_UVTransform.UVMatrix[1] * mpst_Gro[go]->dst_UV[i].fU + mst_UVTransform.UVMatrix[3] * mpst_Gro[go]->dst_UV[i].fV + mst_UVTransform.AddV;
		    }
	}
	else
	{
        for (go = 0; go < mi_NbGO; go++)
		    for(i = 0; i < ml_NbUV[go]; i++)
		    {
			    mpst_UV[go][i].u = mpst_Gro[go]->dst_UV[i].fU;
			    mpst_UV[go][i].v = mpst_Gro[go]->dst_UV[i].fV;
		    }
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_TransformOut(void)
{
	/*~~*/
	int go, i;
	/*~~*/

	if(mb_Transform && mb_UseTransform)
	{
        for (go = 0; go < mi_NbGO; go++)
            for(i = 0; i < ml_NbUV[go]; i++)
		    {
			    mpst_Gro[go]->dst_UV[i].fU = mst_UVTransformInv.UVMatrix[0] * mpst_UV[go][i].u + mst_UVTransformInv.UVMatrix[2] * mpst_UV[go][i].v + mst_UVTransformInv.AddU;
			    mpst_Gro[go]->dst_UV[i].fV = mst_UVTransformInv.UVMatrix[1] * mpst_UV[go][i].u + mst_UVTransformInv.UVMatrix[3] * mpst_UV[go][i].v + mst_UVTransformInv.AddV;
		    }
	}
	else
	{
        for (go = 0; go < mi_NbGO; go++)
		    for(i = 0; i < ml_NbUV[go]; i++)
		    {
			    mpst_Gro[go]->dst_UV[i].fU = mpst_UV[go][i].u;
			    mpst_Gro[go]->dst_UV[i].fV = mpst_UV[go][i].v;
		    }
	}
#ifdef JADEFUSION
    UpdateForXenon();
#endif

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::SetMaterial(int _i_Level)
{
	mpst_MTLused = NULL;
	if(mpst_MatUsed == NULL) return;

	if(mpst_MatUsed->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
	{
		mpst_MTLused = ((MAT_tdst_MultiTexture *) mpst_MatUsed)->pst_FirstLevel;

		/* prend le premier level avec une texture si -1 */
		if(_i_Level == -1)
		{
			while(mpst_MTLused)
			{
				if(mpst_MTLused->s_TextureId != -1) break;
				mpst_MTLused = mpst_MTLused->pst_NextLevel;
			}
		}
		else
		{
			while(_i_Level--)
			{
				mpst_MTLused = mpst_MTLused->pst_NextLevel;
				if(mpst_MTLused == NULL) break;
			}
		}

		if(mpst_MTLused)
		{
			mb_Transform = TRUE;
			SetUVTransform();
			SetTexture(mpst_MTLused->s_TextureId);
		}
		else
		{
			mb_Transform = FALSE;
		}
	}
	else if(mpst_MatUsed->st_Id.i->ul_Type == GRO_MaterialSingle)
	{
		mb_Transform = FALSE;
		SetTexture(((MAT_tdst_Single *) mpst_MatUsed)->l_TextureId);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::SetUVTransform(void)
{
	/*~~~~~~~~~*/
	float							f, g;
	/*~~~~~~~~~*/

	if(mpst_MTLused)
	{
		mb_Transform = TRUE;
		MAT_VUDecompress(mpst_MTLused, &mst_UVTransform);

		f = mst_UVTransform.UVMatrix[0] * mst_UVTransform.UVMatrix[3];
		f -= mst_UVTransform.UVMatrix[2] * mst_UVTransform.UVMatrix[1];
		if(f == 0)
		{
			mb_Transform = FALSE;
			return;
		}

		f = fInv(f);
		mst_UVTransformInv.UVMatrix[0] = mst_UVTransform.UVMatrix[3] * f;
		mst_UVTransformInv.UVMatrix[1] = -mst_UVTransform.UVMatrix[1] * f;
		mst_UVTransformInv.UVMatrix[2] = -mst_UVTransform.UVMatrix[2] * f;
		mst_UVTransformInv.UVMatrix[3] = mst_UVTransform.UVMatrix[0] * f;
		
		g = mst_UVTransform.UVMatrix[2] * mst_UVTransform.AddV - mst_UVTransform.UVMatrix[3] * mst_UVTransform.AddU;
		mst_UVTransformInv.AddU = g * f;
		g = mst_UVTransform.UVMatrix[1] * mst_UVTransform.AddU - mst_UVTransform.UVMatrix[0] * mst_UVTransform.AddV;
		mst_UVTransformInv.AddV = g * f;
	}
	else
	{
		mb_Transform = FALSE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::SetTexture(int tex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC									*pDC;
	ULONG								ul_Key, ul_Index;
	HBITMAP								h_RGB, h_A;
	int									W, H, result;
	TEX_tdst_4Edit_CreateBitmapResult	st_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((tex >= 0) && (tex < TEX_gst_GlobalList.l_NumberOfTextures))
	{
		ul_Key = TEX_gst_GlobalList.dst_Texture[tex].ul_Key;
		ul_Index = BIG_ul_SearchKeyToFat(ul_Key);

		st_Res.ppc_Raw = NULL;
		st_Res.ph_Alpha = &h_A;
		st_Res.ph_Colors = &h_RGB;
		st_Res.pi_Height = &H;
		st_Res.pi_Width = &W;
		st_Res.pst_Pal = NULL;
		st_Res.ph_PaletteAlpha = NULL;
		st_Res.ph_PaletteColors = NULL;
		st_Res.b_RawPalPrio = 1;

		pDC = GetDC();
		result = TEX_i_4Edit_CreateBitmap(ul_Index, 0, pDC->GetSafeHdc(), &st_Res);
		ReleaseDC(pDC);

		if(result)
		{
			DeleteBitmap();
			mh_BitmapRGB = h_RGB;
			mh_BitmapA = h_A;
			mi_BmpWidth = W;
			mi_BmpHeight = H;
			UpdateMapBmp();
			InvalidateRect(NULL, FALSE);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_UVMapperDialog::WheelZoom(int i_Delta)
{
	/*~~~~~~~~~~~~~~~~~*/
	CPoint	o_Pt;
	float	x, y;
	/*~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&o_Pt);
	ScreenToClient(&o_Pt);

	if(!mo_MapRect.PtInRect(o_Pt)) return 0;

	if(GetAsyncKeyState(VK_LMENU) < 0)
	{
		((CButton *) GetDlgItem(IDC_CHECK_SHOWGRID))->SetCheck(1);
		mi_GridWidth += (i_Delta < 0) ? 1 : -1;
		if(mi_GridWidth < 1) mi_GridWidth = 1;
		if(mi_GridWidth > 16) mi_GridWidth = 16;
		OnRedraw();
	}
	else if(GetAsyncKeyState(VK_SPACE) < 0)
	{
		if((mf_Scale >= 32) && (i_Delta < 0)) return 1;
		if((mf_Scale <= 0.1f) && (i_Delta > 0)) return 1;

   		x = mf_X0 + (o_Pt.x - mo_MapRect.left) * (mf_ScaleX / mo_MapRect.Width());
		y = mf_Y0 + (o_Pt.y - mo_MapRect.top) * (mf_ScaleY / mo_MapRect.Height());

		mf_Scale *= (i_Delta < 0) ? 1.1f : 0.9f;
		UpdateScale();

		mf_X0 = x - (o_Pt.x - mo_MapRect.left) * (mf_ScaleX / mo_MapRect.Width());
		mf_Y0 = y - (o_Pt.y - mo_MapRect.top) * (mf_ScaleY / mo_MapRect.Height());
		mf_Y1 = 1 - mf_Y0 - mf_ScaleY;

		UpdateMapBmp();
	}
	else
	{
		mi_Helper = (mi_Helper + ((i_Delta > 0) ? 1 : 2)) % 3;
	}

	InvalidateRect(NULL, FALSE);

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::Tree_AddSingle(MAT_tdst_Single *_pst_Mat, CTreeCtrl *_po_TC, HTREEITEM _h_Parent)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		sz_Name[256], *psz_Name;
	int			i_Texture;
	HTREEITEM	h_TI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	psz_Name = sz_Name + sprintf(sz_Name, "%s [", _pst_Mat->st_Id.sz_Name);

	i_Texture = _pst_Mat->l_TextureId;
	if(!TEX_b_File_GetName(i_Texture, psz_Name))
	{
		strcpy(psz_Name, "Invalid Texture");
		i_Texture = -1;
	}

	psz_Name += strlen(psz_Name);
	strcpy(psz_Name, "]");

	h_TI = _po_TC->InsertItem(sz_Name, _h_Parent);

	_po_TC->SetItemImage(h_TI, 0, 0);
	_po_TC->SetItemData(h_TI, (ULONG) _pst_Mat);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::Tree_AddMultiTexture
(
	MAT_tdst_MultiTexture	*_pst_Mat,
	CTreeCtrl				*_po_TC,
	HTREEITEM				_h_Parent
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				sz_Name[256];
	int					i_Texture, i_Layer;
	HTREEITEM			h_TI;
	MAT_tdst_MTLevel	*pst_MTL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_MTL = _pst_Mat->pst_FirstLevel;
	i_Layer = 0;
	while(pst_MTL)
	{
		i_Layer++;
		pst_MTL = pst_MTL->pst_NextLevel;
	}

	sprintf(sz_Name, "%s [%d Layers]", _pst_Mat->st_Id.sz_Name, i_Layer);
	h_TI = _po_TC->InsertItem(sz_Name, _h_Parent);

	_po_TC->SetItemData(h_TI, (ULONG) 0);
	_po_TC->SetItemImage(h_TI, -1, -1);
	_h_Parent = h_TI;

	pst_MTL = _pst_Mat->pst_FirstLevel;
	i_Layer = 0;
	while(pst_MTL)
	{
		i_Texture = pst_MTL->s_TextureId;
		if(!TEX_b_File_GetName(i_Texture, sz_Name))
		{
			strcpy(sz_Name, "Invalid Texture");
			i_Texture = -1;
		}

		h_TI = _po_TC->InsertItem(sz_Name, _h_Parent);

		_po_TC->SetItemData(h_TI, (ULONG) _pst_Mat);
		_po_TC->SetItemImage(h_TI, i_Layer, i_Layer);

		pst_MTL = pst_MTL->pst_NextLevel;
		i_Layer++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::Tree_AddMulti(MAT_tdst_Multi *_pst_Mat, CTreeCtrl *_po_TC, HTREEITEM _h_Parent)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				sz_Name[256];
	int					i_Sub;
	HTREEITEM			h_TI;
	MAT_tdst_Material	**ppst_Mat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(sz_Name, "%s [%d SubMats]", _pst_Mat->st_Id.sz_Name, _pst_Mat->l_NumberOfSubMaterials);
	h_TI = _po_TC->InsertItem(sz_Name, _h_Parent);
	_po_TC->SetItemData(h_TI, (ULONG) 0);
	_po_TC->SetItemImage(h_TI, -1, -1);

	/* _po_TC->SetItemData(h_TI, -2); */
	_h_Parent = h_TI;

	ppst_Mat = _pst_Mat->dpst_SubMaterial;

	for(i_Sub = 0; i_Sub < _pst_Mat->l_NumberOfSubMaterials; i_Sub++, ppst_Mat++)
	{
		if(*ppst_Mat)
		{
			if((*ppst_Mat)->st_Id.i->ul_Type == GRO_MaterialSingle)
			{
				Tree_AddSingle((MAT_tdst_Single *) * ppst_Mat, _po_TC, _h_Parent);
				continue;
			}

			if((*ppst_Mat)->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
			{
				Tree_AddMultiTexture((MAT_tdst_MultiTexture *) * ppst_Mat, _po_TC, _h_Parent);
				continue;
			}
		}

		h_TI = _po_TC->InsertItem("Invalid Reference", _h_Parent);
		_po_TC->SetItemData(h_TI, -1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_Rotate(float _f_Angle)
{
	/*~~~~~~~~~~~~~~~*/
	float	dx, dy;
	float	dx2, dy2;
	float	cosa, sina;
	int		go, i, count;
	/*~~~~~~~~~~~~~~~*/

	cosa = fCos(-_f_Angle);
	sina = fSin(-_f_Angle);

    for (go = 0; go < mi_NbGO; go++)
	{
		if(!mpst_SaveUV[go]) continue;
	    for(i = 0, count = 0; i < ml_NbUV[go]; i++)
	    {
		    if(mpst_UV[go][i].c_Sel)
		    {
			    dx = mpst_SaveUV[go][count].fU;
			    dy = mpst_SaveUV[go][count].fV;
			    count++;

			    dx2 = (cosa * dx - sina * dy);
			    dy2 = (sina * dx + cosa * dy);

			    mpst_UV[go][i].u = mf_HelperX + dx2;
			    mpst_UV[go][i].v = mf_HelperY + dy2;
		    }
	    }
	}
	UV_TransformOut();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_Move( int _i_Pixel, float dx, float dy)
{
	int i, go;
	
	if ( !UV_StackUndo( 1 ) )
	{
		UV_Save();
		UV_PushUndo( FALSE, 1);
	}
	
	if ( _i_Pixel )
	{
		dx = (float) dx * (mf_ScaleX / mo_MapRect.Width());
		dy = (float) dy * (mf_ScaleY / mo_MapRect.Height());
	}
	
	for(go = 0; go < mi_NbGO; go++)
	{
		for(i = 0; i < ml_NbUV[go]; i++)
		{
			if(mpst_UV[go][i].c_Sel)
			{
				mpst_UV[go][i].u += dx;
				mpst_UV[go][i].v -= dy;
			}
		}
	}

	UV_TransformOut();
	LINK_Refresh();
	OnPaint();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_Zoom(float _f_Scale, char _c_Axis)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	float	dx, dy, t, d;
	float	dx2, dy2, dx3, dy3;
	int		go, i, count;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_c_Axis)
	{
		dx = (float) mi_HelperStartX * (mf_ScaleX / mo_MapRect.Width());
		dy = (float) mi_HelperStartY * (mf_ScaleY / mo_MapRect.Height());
		d = 1 / (float) (dx * dx + dy * dy);

        for (go = 0; go < mi_NbGO; go++)
		{
			if(!mpst_SaveUV[go]) continue;
		    for(i = 0, count = 0; i < ml_NbUV[go]; i++)
		    {
			    if(mpst_UV[go][i].c_Sel)
			    {
				    dx2 = mpst_SaveUV[go][count].fU;
				    dy2 = mpst_SaveUV[go][count].fV;

				    t = (dx * dx2 + dy * dy2) * d;

				    dx3 = t * dx;
				    dy3 = t * dy;

				    dx2 = (dx2 - dx3) + (dx3 * _f_Scale);
				    dy2 = (dy2 - dy3) + (dy3 * _f_Scale);
				    mpst_UV[go][i].u = mf_HelperX + dx2;
				    mpst_UV[go][i].v = mf_HelperY + dy2;
				    count++;
			    }
		    }
		}
	}
	else
	{
        for (go = 0; go < mi_NbGO; go++)
		{
			if(!mpst_SaveUV[go]) continue;
		    for(i = 0, count = 0; i < ml_NbUV[go]; i++)
		    {
			    if(mpst_UV[go][i].c_Sel)
			    {
				    mpst_UV[go][i].u = mf_HelperX + _f_Scale * mpst_SaveUV[go][count].fU;
				    mpst_UV[go][i].v = mf_HelperY + _f_Scale * mpst_SaveUV[go][count].fV;
				    count++;
			    }
		    }
		}
	}

	UV_TransformOut();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_Save(void)
{
	/*~~~~~~~~~*/
	int go, i, count;
	/*~~~~~~~~~*/

    for (go = 0; go < mi_NbGO; go++)
    {
	    for(i = 0, count = 0; i < ml_NbUV[go]; i++)
	    {
		    if(mpst_UV[go][i].c_Sel) count++;
	    }

	    if(!count) continue;

	    mpst_SaveUV[go] = (GEO_tdst_UV *) L_malloc(count * sizeof(GEO_tdst_UV));

	    for(i = 0, count = 0; i < ml_NbUV[go]; i++)
	    {
		    if(mpst_UV[go][i].c_Sel)
		    {
			    mpst_SaveUV[go][count].fU = mpst_UV[go][i].u - mf_HelperX;
			    mpst_SaveUV[go][count++].fV = mpst_UV[go][i].v - mf_HelperY;
		    }
	    }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_Restore(void)
{
	/*~~~~~~~~~*/
	int go, i, count;
	/*~~~~~~~~~*/

    for (go = 0; go < mi_NbGO; go++)
    {
		if(!mpst_SaveUV[go]) continue;

	    for(i = 0, count = 0; i < ml_NbUV[go]; i++)
	    {
		    if(mpst_UV[go][i].c_Sel)
		    {
			    mpst_UV[go][i].u = mf_HelperX + mpst_SaveUV[go][count].fU;
			    mpst_UV[go][i].v = mf_HelperY + mpst_SaveUV[go][count].fV;
			    count++;
		    }
	    }
    }

	UV_TransformOut();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_UVMapperDialog::UV_StackUndo( int _i_StackModif )
{
	if ( _i_StackModif && ( _i_StackModif == mi_Undo_StackType ) && ( mpo_GroView->mpo_ToolBox->mpo_View->mo_UndoManager.mpo_PrevModif == mpo_Undo_stack ) )
		return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_PushUndo(BOOL _b_All, int _i_StackModif )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						go, i, count, undo;
	GEO_tdst_UV				*pst_UV[4];
	F3D_cl_Undo_UVMoving	*po_UVModif = NULL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	if ( UV_StackUndo( _i_StackModif ) )
		return;
	
    undo = 0;
    for ( go = 0; go < mi_NbGO; go++)
    {
        pst_UV[go] = NULL;
	    if(!mpst_SaveUV[go] && !_b_All ) continue;

	    pst_UV[go] = (GEO_tdst_UV *) MEM_p_Alloc(ml_NbUV[go] * sizeof(GEO_tdst_UV));
        undo = 1;

	    for(i = 0, count = 0; i < ml_NbUV[go]; i++)
	    {
		    if(mpst_UV[go][i].c_Sel && !_b_All )
		    {
			    pst_UV[go][i].fU = mf_HelperX + mpst_SaveUV[go][count].fU;
			    pst_UV[go][i].fV = mf_HelperY + mpst_SaveUV[go][count].fV;
			    count++;
		    }
		    else
		    {
			    pst_UV[go][i].fU = mpst_Gro[go]->dst_UV[i].fU;
			    pst_UV[go][i].fV = mpst_Gro[go]->dst_UV[i].fV;
		    }
	    }

		if (mpst_SaveUV[go])
		{
			L_free(mpst_SaveUV[go]);
    		mpst_SaveUV[go] = NULL;
    	}
    }

    if (undo)
    {
	    po_UVModif = new F3D_cl_Undo_UVMoving(mpo_GroView->mpo_ToolBox->mpo_View, mi_NbGO, mpst_Gro, pst_UV);
	    mpo_GroView->mpo_ToolBox->mpo_View->mo_UndoManager.b_AskFor(po_UVModif, FALSE);
    }
    mi_Undo_StackType = _i_StackModif;
    mpo_Undo_stack = po_UVModif;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_Detach(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									go, i, j, k, i_Last[4], update;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Face;
	F3D_cl_Undo_GeoModif				*po_Modif;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    update = 0;
    for (go = 0; go < mi_NbGO; go++)
    {
	    if(!mpst_Gro[go] || !mpst_Gro[go]->pst_SubObject) continue;

    	po_Modif = new F3D_cl_Undo_GeoModif(mpo_GroView->mpo_ToolBox->mpo_View, mpst_Gro[go]);
    	mpo_GroView->mpo_ToolBox->mpo_View->mo_UndoManager.b_AskFor(po_Modif, FALSE);

	    i_Last[go] = mpst_Gro[go]->l_NbUVs;

    	for(i = 0; i < (int) mpst_Gro[go]->pst_SubObject->ul_NbFaces; i++)
	    {
		    if(mpst_Gro[go]->pst_SubObject->dc_FSel[i] & 1)
		    {
			    pst_Element = mpst_Gro[go]->dst_Element + mpst_Gro[go]->pst_SubObject->dst_Faces[i].uw_Element;
			    pst_Face = pst_Element->dst_Triangle + mpst_Gro[go]->pst_SubObject->dst_Faces[i].uw_Index;
			    for(j = 0; j < 3; j++)
			    {
				    k = pst_Face->auw_UV[j];
				    if(mpst_UV[go][k].c_Sel)
				    {
					    if(mpst_UV[go][k].i_Ref > 1)
					    {
						    mpst_UV[go][k].i_Ref--;
						    GEO_AddUV(mpst_Gro[go], mpst_Gro[go]->dst_UV[k].fU, mpst_Gro[go]->dst_UV[k].fV);
						    pst_Face->auw_UV[j] = (unsigned short) (mpst_Gro[go]->l_NbUVs - 1);
                            update = 1;
					    }
				    }
			    }
		    }
	    }
    }

    if (update)
    {
        UpdateUV(FALSE);
        for (go = 0; go < mi_NbGO; go++)
            if(i_Last[go] != mpst_Gro[go]->l_NbUVs)
    	        while(i_Last[go] < mpst_Gro[go]->l_NbUVs) mpst_UV[go][i_Last[go]++].c_Sel = 1;
        InvalidateRect(NULL, FALSE);
    }


}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_WeldThresh(float _f_Thresh)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	typedef struct	tdst_WeldPoint_
	{
		float	fU, fV;
		int		i, n, to;
	} tdst_WeldPoint;

	tdst_WeldPoint						*W;
	int									NbW;
	float								f, x, dist;
	int									go, i, j, update;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LElement;
	GEO_tdst_IndexedTriangle			*pst_Face, *pst_LFace;
	F3D_cl_Undo_GeoModif				*po_Modif;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    update = 0;
    for (go = 0; go < mi_NbGO; go++)
    {
	    if(!mpst_Gro[go] || !mpst_Gro[go]->pst_SubObject) continue;

	    W = (tdst_WeldPoint *) L_malloc(mpst_Gro[go]->l_NbUVs * sizeof(tdst_WeldPoint));
	    L_memset(W, 0, mpst_Gro[go]->l_NbUVs * sizeof(tdst_WeldPoint));
	    NbW = 0;
	    f = _f_Thresh * _f_Thresh;

	    for(i = 0; i < ml_NbUV[go]; i++)
	    {
		    W[i].to = i;
		    if(mpst_UV[go][i].c_Sel)
		    {
			    for(j = 0; j < NbW; j++)
			    {
				    x = mpst_Gro[go]->dst_UV[i].fU - W[j].fU;
				    dist = x * x;
				    x = mpst_Gro[go]->dst_UV[i].fV - W[j].fV;
				    dist += x * x;

				    if(dist < f)
				    {
					    W[j].fU = (W[j].fU * W[j].n + mpst_Gro[go]->dst_UV[i].fU) / ((float) (W[j].n + 1));
					    W[j].fV = (W[j].fV * W[j].n + mpst_Gro[go]->dst_UV[i].fV) / ((float) (W[j].n + 1));
					    W[j].n++;
					    W[i].to = W[j].i;
					    break;
				    }
			    }

			    if(j == NbW)
			    {
				    W[NbW].fU = mpst_Gro[go]->dst_UV[i].fU;
				    W[NbW].fV = mpst_Gro[go]->dst_UV[i].fV;
				    W[NbW].i = i;
				    W[NbW].n = 1;
				    NbW++;
			    }
		    }
	    }

	    for(i = 0; i < NbW; i++)
	    {
		    if(W[i].n > 1) break;
	    }

	    if(i == NbW) continue;

	    po_Modif = new F3D_cl_Undo_GeoModif(mpo_GroView->mpo_ToolBox->mpo_View, mpst_Gro[go]);
	    mpo_GroView->mpo_ToolBox->mpo_View->mo_UndoManager.b_AskFor(po_Modif, FALSE);
        update = 1;

	    for(i = 0; i < NbW; i++)
	    {
		    mpst_Gro[go]->dst_UV[W[i].i].fU = W[i].fU;
		    mpst_Gro[go]->dst_UV[W[i].i].fV = W[i].fV;
	    }

	    pst_Element = mpst_Gro[go]->dst_Element;
	    pst_LElement = pst_Element + mpst_Gro[go]->l_NbElements;
	    for(; pst_Element < pst_LElement; pst_Element++)
	    {
		    pst_Face = pst_Element->dst_Triangle;
		    pst_LFace = pst_Face + pst_Element->l_NbTriangles;
		    for(; pst_Face < pst_LFace; pst_Face++)
		    {
			    for(i = 0; i < 3; i++) pst_Face->auw_UV[i] = W[pst_Face->auw_UV[i]].to;
		    }
	    }

	    for(i = 0; i < NbW; i++) mpst_UV[go][W[i].i].c_Sel = 1;
    }

    if (update )
    {
        UpdateUV(FALSE);
        InvalidateRect(NULL, FALSE);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_UVMapperDialog::UV_CanBeAligned(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									i, Count;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Face;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Gro[0] || !mpst_Gro[0]->pst_SubObject) return FALSE;

	for(i = 0, Count = 0; i < ml_NbUV[0]; i++)
	{
		if(mpst_UV[0][i].c_Sel) Count++;
	}

	if(Count != 4) return FALSE;

	for(i = 0, Count = 0; i < (int) mpst_Gro[0]->pst_SubObject->ul_NbFaces; i++)
	{
		if(mpst_Gro[0]->pst_SubObject->dc_FSel[i] & 1)
		{
			pst_Element = mpst_Gro[0]->dst_Element + mpst_Gro[0]->pst_SubObject->dst_Faces[i].uw_Element;
			pst_Face = pst_Element->dst_Triangle + mpst_Gro[0]->pst_SubObject->dst_Faces[i].uw_Index;

			if(!mpst_UV[0][pst_Face->auw_UV[0]].c_Sel) continue;
			if(!mpst_UV[0][pst_Face->auw_UV[1]].c_Sel) continue;
			if(!mpst_UV[0][pst_Face->auw_UV[2]].c_Sel) continue;
			if(Count == 2) return FALSE;
			mai_BandAlignFace[Count++] = i;
		}
	}

	if(Count != 2) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::UV_BandAlign(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									i_First, i_FirstQuad, i;
	int									i_SaveFace[2];
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Face;
	GEO_tdst_SubObject_Edge				*pst_Edge, *pst_Last;
	GEO_tdst_UV							*UV[6], UVDir[2];
	char								*pc_Sel, *pc_FaceUsed;
	int									i_VertexIndex[6], i_UVIndex[6];
	MATH_tdst_Vector					V, W;
	float								t0, t1, d0, d1, d2, d3, ratio;
	F3D_cl_Undo_GeoModif				*po_Modif;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Modif = new F3D_cl_Undo_GeoModif(mpo_GroView->mpo_ToolBox->mpo_View, mpst_Gro[0]);
	mpo_GroView->mpo_ToolBox->mpo_View->mo_UndoManager.b_AskFor(po_Modif, FALSE);

	i_First = 1;
	i_FirstQuad = 1;
	MATH_InitVectorToZero(&V);
	MATH_InitVectorToZero(&W);
	GEO_SubObject_VUpdateESel(mpst_Gro[0]);

	for(i = 0; i < ml_NbUV[0]; i++) mpst_UV[0][i].c_Used = 0;
    pc_FaceUsed = (char *) L_malloc( mpst_Gro[0]->pst_SubObject->ul_NbFaces );
    L_memset( pc_FaceUsed, 0, mpst_Gro[0]->pst_SubObject->ul_NbFaces );

	/* trouve deux faces sélectionnées qui continue la bande */
uvbandalign_tryagain:
	for(i = 0; i < 2; i++)
	{
		pst_Edge = mpst_Gro[0]->pst_SubObject->dst_Edge;
		pst_Last = pst_Edge + mpst_Gro[0]->pst_SubObject->ul_NbEdges;
		pc_Sel = mpst_Gro[0]->pst_SubObject->dc_ESel;
		for(; pst_Edge < pst_Last; pst_Edge++, pc_Sel++)
		{
			if(!(*pc_Sel & 1)) continue;

			if(pst_Edge->ul_Triangle[1] == -1) continue;
			if(!(mpst_Gro[0]->pst_SubObject->dc_FSel[pst_Edge->uw_Index[0]] & 1)) continue;
			if(!(mpst_Gro[0]->pst_SubObject->dc_FSel[pst_Edge->uw_Index[1]] & 1)) continue;

			if((pst_Edge->uw_Index[0] == mai_BandAlignFace[1 + i]) && (pst_Edge->uw_Index[1] != mai_BandAlignFace[i]))
			{
                if ( !pc_FaceUsed[ pst_Edge->uw_Index[1] ] )
                {
                    pc_FaceUsed[ pst_Edge->uw_Index[1] ] = 1;
				    mai_BandAlignFace[2 + i] = pst_Edge->uw_Index[1];
				    break;
                }
			}

			if((pst_Edge->uw_Index[1] == mai_BandAlignFace[1 + i]) && (pst_Edge->uw_Index[0] != mai_BandAlignFace[i]))
			{
                if ( !pc_FaceUsed[ pst_Edge->uw_Index[0] ] )
                {
				    mai_BandAlignFace[2 + i] = pst_Edge->uw_Index[0];
                    pc_FaceUsed[ pst_Edge->uw_Index[0] ] = 1;
				    break;
                }
			}
		}

		if(pst_Edge == pst_Last)
		{
			if(i_First)
			{
				i_First = 0;
				i = mai_BandAlignFace[0];
				mai_BandAlignFace[0] = mai_BandAlignFace[1];
				mai_BandAlignFace[1] = i;
				goto uvbandalign_tryagain;
			}

			UpdateUV(FALSE);
			goto uvbandalign_end;
		}

		i_First = 0;

		if(i_FirstQuad && (i == 0))
		{
			i_SaveFace[0] = mai_BandAlignFace[0];
			i_SaveFace[1] = mai_BandAlignFace[1];
			i_VertexIndex[1] = pst_Edge->ul_Point[0];
			i_VertexIndex[4] = pst_Edge->ul_Point[1];
		}
	}

	if((mai_BandAlignFace[2] == i_SaveFace[0]) && (mai_BandAlignFace[3] == i_SaveFace[1]))
	{
		UpdateUV(FALSE);
		goto uvbandalign_end;
	}

	if(i_FirstQuad)
	{
		i_FirstQuad = 0;

		i = mai_BandAlignFace[1];
		pst_Element = mpst_Gro[0]->dst_Element + mpst_Gro[0]->pst_SubObject->dst_Faces[i].uw_Element;
		pst_Face = pst_Element->dst_Triangle + mpst_Gro[0]->pst_SubObject->dst_Faces[i].uw_Index;
		for(i = 0; i < 3; i++)
		{
			if(pst_Face->auw_Index[i] == i_VertexIndex[1])
				i_UVIndex[1] = pst_Face->auw_UV[i];
			else if(pst_Face->auw_Index[i] == i_VertexIndex[4])
				i_UVIndex[4] = pst_Face->auw_UV[i];
			else
			{
				i_VertexIndex[0] = pst_Face->auw_Index[i];
				i_UVIndex[0] = pst_Face->auw_UV[i];
			}
		}

		i = mai_BandAlignFace[0];
		pst_Element = mpst_Gro[0]->dst_Element + mpst_Gro[0]->pst_SubObject->dst_Faces[i].uw_Element;
		pst_Face = pst_Element->dst_Triangle + mpst_Gro[0]->pst_SubObject->dst_Faces[i].uw_Index;
		for(i = 0; i < 3; i++)
		{
			if(pst_Face->auw_Index[i] == i_VertexIndex[0])
				i_UVIndex[0] = pst_Face->auw_UV[i];
			else if(pst_Face->auw_Index[i] == i_VertexIndex[1])
				i_UVIndex[1] = pst_Face->auw_UV[i];
			else if(pst_Face->auw_Index[i] == i_VertexIndex[4])
				i_UVIndex[4] = pst_Face->auw_UV[i];
			else
			{
				i_VertexIndex[3] = pst_Face->auw_Index[i];
				i_UVIndex[3] = pst_Face->auw_UV[i];
			}
		}

		for(i = 0; i < 6; i++) UV[i] = mpst_Gro[0]->dst_UV + i_UVIndex[i];

		/* quad de référence dans le bon sens ? */
		t0 = fSqr(UV[1]->fU - UV[0]->fU) + fSqr(UV[1]->fV - UV[0]->fV);
		t0 += fSqr(UV[4]->fU - UV[3]->fU) + fSqr(UV[4]->fV - UV[3]->fV);

		t1 = fSqr(UV[4]->fU - UV[0]->fU) + fSqr(UV[4]->fV - UV[0]->fV);
		t1 += fSqr(UV[1]->fU - UV[3]->fU) + fSqr(UV[1]->fV - UV[3]->fV);

		if(t1 < t0)
		{
			i = i_VertexIndex[0];
			i_VertexIndex[0] = i_VertexIndex[3];
			i_VertexIndex[3] = i;

			i = i_UVIndex[0];
			i_UVIndex[0] = i_UVIndex[3];
			i_UVIndex[3] = i;

			UV[0] = mpst_Gro[0]->dst_UV + i_UVIndex[0];
			UV[3] = mpst_Gro[0]->dst_UV + i_UVIndex[3];
		}

		/*
		 * premier quad : calcule le ratio entre distance entre point et distance entre UV
		 * et direction
		 */
		UVDir[0].fU = UV[1]->fU - UV[0]->fU;
		UVDir[0].fV = UV[1]->fV - UV[0]->fV;
		t0 = fSqrt((UVDir[0].fU * UVDir[0].fU) + (UVDir[0].fV * UVDir[0].fV));
		UVDir[0].fU /= t0;
		UVDir[0].fV /= t0;

		UVDir[1].fU = UV[4]->fU - UV[3]->fU;
		UVDir[1].fV = UV[4]->fV - UV[3]->fV;
		t1 = fSqrt((UVDir[1].fU * UVDir[1].fU) + (UVDir[1].fV * UVDir[1].fV));
		UVDir[1].fU /= t1;
		UVDir[1].fV /= t1;

		d0 = MATH_f_Distance(mpst_Gro[0]->dst_Point + i_VertexIndex[0], mpst_Gro[0]->dst_Point + i_VertexIndex[1]);
		d1 = MATH_f_Distance(mpst_Gro[0]->dst_Point + i_VertexIndex[3], mpst_Gro[0]->dst_Point + i_VertexIndex[4]);
		if(d0 < d1) d0 = d1;
		ratio = t0 / d0;

		mpst_UV[0][i_UVIndex[0]].c_Used = 1;
		mpst_UV[0][i_UVIndex[1]].c_Used = 1;
		mpst_UV[0][i_UVIndex[3]].c_Used = 1;
		mpst_UV[0][i_UVIndex[4]].c_Used = 1;
	}

	i = mai_BandAlignFace[2];
	pst_Element = mpst_Gro[0]->dst_Element + mpst_Gro[0]->pst_SubObject->dst_Faces[i].uw_Element;
	pst_Face = pst_Element->dst_Triangle + mpst_Gro[0]->pst_SubObject->dst_Faces[i].uw_Index;
	for(i = 0; i < 3; i++)
	{
		if(pst_Face->auw_Index[i] == i_VertexIndex[1])
			pst_Face->auw_UV[i] = i_UVIndex[1];
		else if(pst_Face->auw_Index[i] == i_VertexIndex[4])
			pst_Face->auw_UV[i] = i_UVIndex[4];
		else
		{
			i_UVIndex[2] = pst_Face->auw_UV[i];
			i_VertexIndex[2] = pst_Face->auw_Index[i];

			if(mpst_UV[0][i_UVIndex[2]].c_Used)
			{
				GEO_AddUV(mpst_Gro[0], 0, 0);
				i_UVIndex[2] = pst_Face->auw_UV[i] = (unsigned short) (mpst_Gro[0]->l_NbUVs - 1);
			}
			else
				mpst_UV[0][i_UVIndex[2]].c_Used = 1;
		}
	}

	i = mai_BandAlignFace[3];
	pst_Element = mpst_Gro[0]->dst_Element + mpst_Gro[0]->pst_SubObject->dst_Faces[i].uw_Element;
	pst_Face = pst_Element->dst_Triangle + mpst_Gro[0]->pst_SubObject->dst_Faces[i].uw_Index;
	for(i = 0; i < 3; i++)
	{
		if(pst_Face->auw_Index[i] == i_VertexIndex[1])
			pst_Face->auw_UV[i] = i_UVIndex[1];
		else if(pst_Face->auw_Index[i] == i_VertexIndex[4])
			pst_Face->auw_UV[i] = i_UVIndex[4];
		else if(pst_Face->auw_Index[i] == i_VertexIndex[2])
			pst_Face->auw_UV[i] = i_UVIndex[2];
		else
		{
			i_VertexIndex[5] = pst_Face->auw_Index[i];
			i_UVIndex[5] = pst_Face->auw_UV[i];

			if(mpst_UV[0][i_UVIndex[5]].c_Used)
			{
				GEO_AddUV(mpst_Gro[0], 0, 0);
				i_UVIndex[5] = pst_Face->auw_UV[i] = (unsigned short) (mpst_Gro[0]->l_NbUVs - 1);
			}
			else
				mpst_UV[0][i_UVIndex[5]].c_Used = 1;
		}
	}

	/* quad suivant dans le bon sens ? */
	t0 = (d0 = MATH_f_Distance(mpst_Gro[0]->dst_Point + i_VertexIndex[2], mpst_Gro[0]->dst_Point + i_VertexIndex[1]));
	t0 += (d1 = MATH_f_Distance(mpst_Gro[0]->dst_Point + i_VertexIndex[5], mpst_Gro[0]->dst_Point + i_VertexIndex[4]));

	t1 = (d2 = MATH_f_Distance(mpst_Gro[0]->dst_Point + i_VertexIndex[5], mpst_Gro[0]->dst_Point + i_VertexIndex[1]));
	t1 += (d3 = MATH_f_Distance(mpst_Gro[0]->dst_Point + i_VertexIndex[2], mpst_Gro[0]->dst_Point + i_VertexIndex[4]));

	if(t1 < t0)
	{
		i = i_UVIndex[2];
		i_UVIndex[2] = i_UVIndex[5];
		i_UVIndex[5] = i;

		i = i_VertexIndex[2];
		i_VertexIndex[2] = i_VertexIndex[5];
		i_VertexIndex[5] = i;

		d0 = d2;
		d1 = d3;
	}

	for(i = 0; i < 6; i++) UV[i] = mpst_Gro[0]->dst_UV + i_UVIndex[i];

	/* nouveau UVx */
	if(d0 < d1) d0 = d1;
	t0 = d0 * ratio;
	UV[5]->fU = UV[4]->fU + t0 * UVDir[0].fU;
	UV[5]->fV = UV[4]->fV + t0 * UVDir[0].fV;
	UV[2]->fU = UV[1]->fU + t0 * UVDir[0].fU;
	UV[2]->fV = UV[1]->fV + t0 * UVDir[0].fV;

	memmove(&mai_BandAlignFace[0], &mai_BandAlignFace[2], 2 * sizeof(int));
	memmove(&i_VertexIndex[0], &i_VertexIndex[1], 5 * sizeof(int));
	memmove(&i_UVIndex[0], &i_UVIndex[1], 5 * sizeof(int));

	goto uvbandalign_tryagain;

uvbandalign_end:
    L_free( pc_FaceUsed );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::SelRect_Draw(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CDC		*pDC;
	CPen	o_Pen, *poldpen;
	/*~~~~~~~~~~~~~~~~~~~~*/

	pDC = GetDC();
	pDC->SetROP2(R2_XORPEN);

	o_Pen.CreatePen(PS_SOLID, 1, 0xFFFFFF);
	poldpen = pDC->SelectObject(&o_Pen);

	pDC->MoveTo(mo_SelRect.TopLeft());
	pDC->LineTo(mo_SelRect.right, mo_SelRect.top);
	pDC->LineTo(mo_SelRect.BottomRight());
	pDC->LineTo(mo_SelRect.left, mo_SelRect.bottom);
	pDC->LineTo(mo_SelRect.TopLeft());

	pDC->SelectObject(poldpen);
	DeleteObject(&o_Pen);
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::SelRect_Sel(void)
{
	/*~~*/
	int go, i;
	/*~~*/

	mo_SelRect.NormalizeRect();

	if(GetAsyncKeyState(VK_SHIFT) < 0)
	{
		if(GetAsyncKeyState(VK_CONTROL) < 0)
		{
            for (go = 0; go < mi_NbGO; go++)
    			for(i = 0; i < ml_NbUV[go]; i++)
	    		{
		    		if(!mpst_UV[go][i].c_Visible) continue;
			    	if(mo_SelRect.PtInRect(*(POINT *) &mpst_UV[go][i].x)) mpst_UV[go][i].c_Sel = 1 - mpst_UV[go][i].c_Sel;
			    }
		}
		else
		{
            for (go = 0; go < mi_NbGO; go++)
			    for(i = 0; i < ml_NbUV[go]; i++)
			    {
				    if(!mpst_UV[go][i].c_Visible) continue;
				    if(mo_SelRect.PtInRect(*(POINT *) &mpst_UV[go][i].x)) mpst_UV[go][i].c_Sel = 0;
			    }
		}
	}
	else
	{
        for (go = 0; go < mi_NbGO; go++)
		    for(i = 0; i < ml_NbUV[go]; i++)
		    {
			    if(!mpst_UV[go][i].c_Visible) continue;
			    if(mo_SelRect.PtInRect(*(POINT *) &mpst_UV[go][i].x)) mpst_UV[go][i].c_Sel = 1;
		    }
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_UVMapperDialog::Helper_Pick(CPoint &o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CPoint	o_Tmp;
	int		i_Dist, i_Dist2;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Helper == 0) return FALSE;
	if(!mi_HelperVisible) return FALSE;

	o_Tmp = o_Pt - *(POINT *) &mi_HelperX;
	i_Dist = (o_Tmp.x * o_Tmp.x) + (o_Tmp.y * o_Tmp.y);
	if(i_Dist < 25)
	{
		mi_HelperPick = 1;	/* center */
		return TRUE;
	}
	else if(mi_Helper == 1) /* rotation helper */
	{
		if((i_Dist >= 1296) && (i_Dist <= 1936))
		{
			mf_HelperCur = mf_HelperEnd = mf_HelperStart = Helper_PickCircle(o_Pt);
			mi_HelperPick = 2;	/* rotation circle */
			return TRUE;
		}

		return FALSE;
	}
	else						/* scale helper */
	{
		if(i_Dist < 225)
		{
			mf_HelperCur = mf_HelperEnd = mf_HelperStart = fSqrt((float) i_Dist);
			mi_HelperPick = 3;	/* zoom */
			return TRUE;
		}

		/* horizontal scale */
		o_Tmp.x -= 30;
		i_Dist2 = (o_Tmp.x * o_Tmp.x) + (o_Tmp.y * o_Tmp.y);
		if(i_Dist2 < 25)
		{
			mi_HelperStartX = 30;
			mi_HelperStartY = 0;
			mf_HelperCur = mf_HelperEnd = mf_HelperStart = 30;
			mf_HelperRot = 0;
			mi_HelperPick = 4;	/* axis scale */
			return TRUE;
		}

		/* vertical scale */
		o_Tmp.x += 30;
		o_Tmp.y -= 30;
		i_Dist2 = (o_Tmp.x * o_Tmp.x) + (o_Tmp.y * o_Tmp.y);
		if(i_Dist2 < 25)
		{
			mi_HelperStartX = 0;
			mi_HelperStartY = 30;
			mf_HelperCur = mf_HelperEnd = mf_HelperStart = 30;
			mf_HelperRot = Cf_PiBy2;
			mi_HelperPick = 4;	/* axis scale */
			return TRUE;
		}

		o_Tmp.y += 30;

		/* other axis scale */
		if(i_Dist < 900)
		{
			mi_HelperStartX = o_Tmp.x;
			mi_HelperStartY = o_Tmp.y;
			mf_HelperCur = mf_HelperEnd = mf_HelperStart = fSqrt((float) i_Dist);
			mf_HelperRot = Helper_PickCircle(o_Pt);
			mi_HelperPick = 4;	/* axis scale */
			return TRUE;
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float EDIA_cl_UVMapperDialog::Helper_PickCircle(CPoint &o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	CPoint	o_Tmp;
	int		i_Dist;
	float	f_Cos, f_Angle;
	/*~~~~~~~~~~~~~~~~~~~*/

	o_Tmp = o_Pt - *(POINT *) &mi_HelperX;
	i_Dist = (o_Tmp.x * o_Tmp.x) + (o_Tmp.y * o_Tmp.y);

	f_Cos = (float) o_Tmp.x / fSqrt((float) i_Dist);
	f_Angle = fAcos(f_Cos);
	if(o_Tmp.y < 0) f_Angle = Cf_2Pi - f_Angle;

	return f_Angle;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::DeleteBitmap(void)
{
	if(mh_BitmapRGB)
	{
		DeleteObject(mh_BitmapRGB);
		mh_BitmapRGB = NULL;
	}

	if(mh_BitmapA)
	{
		DeleteObject(mh_BitmapA);
		mh_BitmapA = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnUVTransform(void)
{
	mb_UseTransform = ((CButton *) GetDlgItem(IDC_CHECK_UVTRANSFORM))->GetCheck() ? TRUE : FALSE;
	SetUVTransform();
	UV_TransformIn();
	InvalidateRect(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnUpdateUVTransform(void)
{
	SetUVTransform();
	UV_TransformIn();
	InvalidateRect(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnRefresh(void)
{
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnShowDataEx( void )
{
	mb_ShowAdditionalData = TRUE;
	RecalcLayout( 1 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnHideDataEx( void )
{
	mb_ShowAdditionalData = FALSE;
	RecalcLayout( 1 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnKeepRatio( void )
{
	mb_KeepRatio = !mb_KeepRatio;
	((CButton *) GetDlgItem( IDC_CHECK_LOCKRATIO ) )->SetCheck( mb_KeepRatio );
	if (!mb_KeepRatio )
		return;
	
	switch( mi_KeepRatioType )
	{
	case 0: 
		mf_RatioToKeep = ((float) mi_BmpHeight)/ ((float) mi_BmpWidth);
		break;
	case 1:
		mf_RatioToKeep = ((float) mo_MapRect.Height() ) / ((float) mo_MapRect.Width() );
		break;
	case 2:	
		mf_RatioToKeep = 1.0f; 
		break;
	}
	UpdateScale();
	mf_Y1 = 1 - mf_Y0 - mf_ScaleY;
	OnRedraw();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::RecalcLayout( BOOL _b_Erase )
{
	/*~~~~~~~~~~~~~*/
	CWnd	*po_List;
	RECT	st_Rect;
	int		x, y, X, XSave, W;
	int		i_Flags;
	/*~~~~~~~~~~~~~*/
	
	GetClientRect( &st_Rect );
	x = st_Rect.right - st_Rect.left + 1;
	y = st_Rect.bottom - st_Rect.top + 1;

	po_List = GetDlgItem(IDC_TREE_MAT);
	if (!po_List) return;
	
	// left pannel ( additionnal data )
	if ( mb_ShowAdditionalData )
	{
		po_List->ShowWindow( SW_SHOW );
		po_List->GetWindowRect(&st_Rect);
		ScreenToClient(&st_Rect);
		st_Rect.bottom = (y - 5);
		po_List->MoveWindow(&st_Rect);
		X = st_Rect.right;
	}
	else
	{
		po_List->ShowWindow( SW_HIDE );
		X = 0;
	}
	
	GetDlgItem( IDC_STATIC_1 )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_STATIC_2 )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_STATIC_3 )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_STATIC_4 )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_STATIC_5 )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_STATIC_GONAME )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_STATIC_GRONAME )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_STATIC_NBELEMENTS )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_STATIC_MATNAME )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_STATIC_NBSUBMAT )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_BUTTON_HIDEDATAEX )->ShowWindow( mb_ShowAdditionalData );
	GetDlgItem( IDC_BUTTON_SHOWDATAEX )->ShowWindow( !mb_ShowAdditionalData );
	
	W = 24;
	XSave = X;
	X += (( X == 0 ) ? 20 : 5);
	mi_BtMin = X;
	mi_BtMax = x - (7*W + 160 );
	X += mi_BtOffset;
	if (mi_BtMax < mi_BtMin) mi_BtMax = mi_BtMin;
	GetDlgItem( IDC_CHECK_ALPHA )->MoveWindow( X, 4, W, W );
	GetDlgItem( IDC_BUTTON_ZOOMALL )->MoveWindow( X += W, 4, W, W );
	GetDlgItem( IDC_CHECK_LOCKRATIO )->MoveWindow( X += W, 4, W, W );
	GetDlgItem( IDC_CHECK_SHOWGRID )->MoveWindow( X += W, 4, W, W );
//	GetDlgItem( IDC_BUTTON_UVGIZMO )->MoveWindow( X += W, 4, W, W );
	GetDlgItem( IDC_CHECK_UVTRANSFORM )->MoveWindow( X += W, 4, W, W );
	GetDlgItem( IDC_CHECK_HIDESUBOBJECT )->MoveWindow( X += W, 4, W, W );
	
	GetDlgItem( IDC_BUTTON_WELDTHRESH )->MoveWindow( X += W + 5, 4, 90, W );
	GetDlgItem( IDC_EDIT_WELDTHRESH )->MoveWindow( X += 90, 4, 60, W );
	GetDlgItem( IDC_BUTTON_UVGIZMO )->MoveWindow( X += 65, 4, W, W );

	mo_MapRect.left = XSave + 5;
	mo_MapRect.right = x - 5;
	mo_MapRect.top = W + 10;
	mo_MapRect.bottom = y - 5;
	UpdateMapBmp();
	
	i_Flags = RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_ERASENOW;
	if ( _b_Erase ) i_Flags |= RDW_ERASE;
	RedrawWindow( NULL, NULL, i_Flags );
}

/*$4
 ***********************************************************************************************************************
    UV gizmo
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UVMapperDialog::OnUVGizmo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector				v;
	F3D_cl_Undo_MultipleGeoModif	*po_UVModif;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(SOFT_b_Helpers_UVGizmoIsOn(mpst_3DHelpers))
		SOFT_Helpers_UVGizmoOff(mpst_3DHelpers);
	else
	{
        if ( !mi_NbGO ) return; 
 
    	MATH_InitVector(&v, 0.5f, 0.5f, 0);
	    mpo_GroView->mpo_ToolBox->mpo_View->b_PickDepth(&v, &v.z);

	    v.z -= (v.z < 10) ? (v.z * 0.1f) : 1;
	    switch(mpst_EditOptions->ul_UVMapperFlags & GRO_Cul_EOUVF_GizmoTypeMask)
	    {
	    case GRO_Cul_EOUVF_GizmoPlanar:
		    SOFT_Helpers_UVGizmoOn(mpst_3DHelpers, mpst_GO[0], SOFT_Cl_UVGizmo_Planar, v.z);
		    break;
	    case GRO_Cul_EOUVF_GizmoCylindrical:
		    SOFT_Helpers_UVGizmoOn(mpst_3DHelpers, mpst_GO[0], SOFT_Cl_UVGizmo_Cylindre, v.z);
		    break;
	    case GRO_Cul_EOUVF_GizmoBox:
		    SOFT_Helpers_UVGizmoOn(mpst_3DHelpers, mpst_GO[0], SOFT_Cl_UVGizmo_Box, v.z);
		    break;
		 /*
		 case GRO_Cul_EOUVF_GizmoSpherical:
		    SOFT_Helpers_UVGizmoOn(mpst_3DHelpers, mpst_GO[0], SOFT_Cl_UVGizmo_Spherical, v.z);
		    break;
		    */
	    default: return;
	    }
	    
	    po_UVModif = new F3D_cl_Undo_MultipleGeoModif(mpo_GroView->mpo_ToolBox->mpo_View );
		po_UVModif->SetDesc("UV Gizmo");
		mpo_GroView->mpo_ToolBox->mpo_View->mo_UndoManager.b_AskFor(po_UVModif, FALSE);
		
	    SOFT_UVGizmo_Apply(&mpst_3DHelpers->st_UVGizmo);
	}

	if(ENG_gb_ExitApplication) return;
	LINK_Refresh();
}

#endif /* ACTIVE_EDITORS */

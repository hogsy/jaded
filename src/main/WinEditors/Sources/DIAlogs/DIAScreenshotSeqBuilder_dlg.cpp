/*$T DIAreplace_dlg.cpp GC! 1.078 03/16/00 10:33:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAScreenshotSeqBuilder_dlg.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "edipaths.h"
#include "bigfiles/bigfat.h"
#include "f3dframe/f3dview.h"
#include "engine/sources/objects/objinit.h"
#include "BIGfiles\LOAding\LOAdefs.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "SELection/SELection.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "EDIstrings.h"
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "TEXture/TEXfile.h"
#include "DIAlogs/DIAname_dlg.h"

#include "EDItors/Sources/OUTput/OUTframe.h"
#include "F3Dframe\F3Dframe.h"
#include "F3Dframe\F3Dview.h"
#include "GDInterface/GDInterface.h"
#include "EDImsg.h"
#include "LINks/LINKtoed.h"


#include <string>
#include "XenonGraphics/XeScreenshotSeqBuilder.h"
#include ".\diascreenshotseqbuilder_dlg.h"

BEGIN_MESSAGE_MAP(EDIA_cl_ScreenshotSeqBuilderDlg, EDIA_cl_BaseDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BT_LOAD, OnBtLoad)
	ON_BN_CLICKED(IDC_BT_SAVE, OnBtSave)
	ON_BN_CLICKED(IDC_BT_START, OnBtStart)
	ON_BN_CLICKED(IDC_BT_UP, OnBtUp)
	ON_BN_CLICKED(IDC_BT_DOWN, OnBtDown)
	ON_BN_CLICKED(IDC_BT_DELETE, OnBtDelete)
	ON_BN_CLICKED(IDC_BT_CLEAR, OnBtClear)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SCREENSHOT, OnListDblClk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SCREENSHOT, OnListLButtonDown)
END_MESSAGE_MAP()

/*$4
***********************************************************************************************************************
Globals
***********************************************************************************************************************
*/

/*
=======================================================================================================================
=======================================================================================================================
*/
EDIA_cl_ScreenshotSeqBuilderDlg::EDIA_cl_ScreenshotSeqBuilderDlg() :
EDIA_cl_BaseDialog(DIALOGS_IDD_SCREENSHOTSEQBUILDER)
{

	m_iOldWidth = 0;
	m_iOldHeight = 0;
	m_bSequenceStarted = FALSE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
EDIA_cl_ScreenshotSeqBuilderDlg::~EDIA_cl_ScreenshotSeqBuilderDlg()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_DisplayData_	*pst_DD;
	EOUT_cl_Frame			*po_3DV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//get Display Data
	po_3DV = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	if (!po_3DV) return;

	pst_DD = po_3DV->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData;

	//update Capture flags
	if(pst_DD)
	{
		if(m_bSequenceStarted)
			pst_DD->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_ForSeqBuilder;
	}
	
	if(((F3D_cl_View *) mpo_View)->mpo_ScreenshotSeqBuilderDialog == this) 
		((F3D_cl_View *) mpo_View)->mpo_ScreenshotSeqBuilderDialog = NULL;
}

/*
=======================================================================================================================
=======================================================================================================================
*/void EDIA_cl_ScreenshotSeqBuilderDlg::DoDataExchange(CDataExchange* pDX)
{
	//CDialog::DoDataExchange(pDX);
	EDIA_cl_BaseDialog::DoDataExchange(pDX);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_ScreenshotSeqBuilderDlg::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl*	po_List;
	CRect		oRect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	EDIA_cl_BaseDialog::OnInitDialog();


	po_List = (CListCtrl*) GetDlgItem(IDC_LIST_SCREENSHOT);
	po_List->GetWindowRect(&oRect);

	po_List->InsertColumn(0, "#");
	po_List->SetColumnWidth(0, 20);
	
	po_List->InsertColumn(1, "Screenshot name");
	po_List->SetColumnWidth(1, (oRect.Width() - 20) / 2);

	po_List->InsertColumn(2, "Camera pos");
	po_List->SetColumnWidth(2, (oRect.Width() - 20) / 2);

	UpdateList();
	RefreshControls();

	return TRUE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_ScreenshotSeqBuilderDlg::PreTranslateMessage(MSG *p_Msg)
{
	if((p_Msg->message == WM_KEYDOWN) && (p_Msg->wParam == VK_ESCAPE))
	{
		OnCancel();
		return TRUE;
	}

	if((p_Msg->message == WM_RBUTTONDOWN) && (p_Msg->hwnd == GetDlgItem(IDC_LIST_SCREENSHOT)->GetSafeHwnd()))
	{
		OnMenuListCtrl();
		return TRUE;
	}

	return FALSE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnMenuListCtrl()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog("Enter screenshot name");
	MATH_tdst_Matrix*	pst_Matrix;
	CListCtrl*			po_List;
	EMEN_cl_SubMenu		o_SubMenu(FALSE);
	CPoint				o_Point;
	int					i_Res, j;
	CString				str;
	POSITION			pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListCtrl *) GetDlgItem(IDC_LIST_SCREENSHOT);

	pos = po_List->GetFirstSelectedItemPosition();
	if(pos)
	{
		j = po_List->GetNextSelectedItem(pos);

		if(j == -1)
			return;
	}
	else
		return;

	GetCursorPos(&o_Point);

	M_MF()->InitPopupMenuAction(NULL, &o_SubMenu);
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 1, TRUE, "Go to screenshot pos");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 2, TRUE, "Set screenshot name");
	i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Point, &o_SubMenu);
	switch(i_Res)
	{
	case 1:
		if(mpo_View)
		{
			pst_Matrix = g_oXeScreenshotSeqBuilder.GetScreenshotMatrix(j);
			mpo_View->SetCameraMatrix(pst_Matrix);
			LINK_Refresh();
		}
		break;

	case 2:
		//set existing name
		o_Dialog.mo_Name = g_oXeScreenshotSeqBuilder.GetScreenshotName(j);

		if(o_Dialog.DoModal() == IDOK && o_Dialog.mo_Name.GetLength() < XE_MAXNAMELENGTH)
		{
			g_oXeScreenshotSeqBuilder.SetScreenshotName(j, (char *) (LPCSTR) o_Dialog.mo_Name);
		}
		break;

	default:
		break;
	}

	UpdateList();
	RefreshControls();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 300;
	lpMMI->ptMinTrackSize.y = 275;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnSize(UINT a, int cx, int cy)
{
	/*~~~~~~~~~~~*/
	CRect oRect;
	int idiffX, idiffY;
	/*~~~~~~~~~~~*/

	idiffX = m_iOldWidth;
	idiffY = m_iOldHeight;

	m_iOldWidth = cx;
	m_iOldHeight = cy;

	EDIA_cl_BaseDialog::OnSize(a, cx, cy);

	if ( !GetDlgItem( IDC_BT_LOAD ) ) return;

	idiffX -= m_iOldWidth;
	idiffY -= m_iOldHeight;


	GetDlgItem(IDC_BT_LOAD)->GetWindowRect(&oRect);
	ScreenToClient(oRect);
	GetDlgItem(IDC_BT_LOAD)->MoveWindow( oRect.left - idiffX, oRect.top, oRect.Width(), oRect.Height() );

	GetDlgItem(IDC_BT_SAVE)->GetWindowRect(&oRect);
	ScreenToClient(oRect);
	GetDlgItem(IDC_BT_SAVE)->MoveWindow( oRect.left - idiffX, oRect.top, oRect.Width(), oRect.Height() );
	
	GetDlgItem(IDC_BT_DOWN)->GetWindowRect(&oRect);
	ScreenToClient(oRect);
	GetDlgItem(IDC_BT_DOWN)->MoveWindow( oRect.left - idiffX, (cy / 2), oRect.Width(), oRect.Height() );

	GetDlgItem(IDC_BT_UP)->GetWindowRect(&oRect);
	ScreenToClient(oRect);
	GetDlgItem(IDC_BT_UP)->MoveWindow( oRect.left - idiffX, (cy / 2) - oRect.Height(), oRect.Width(), oRect.Height() );
	
	GetDlgItem(IDC_BT_DELETE)->GetWindowRect(&oRect);
	ScreenToClient(oRect);
	GetDlgItem(IDC_BT_DELETE)->MoveWindow( oRect.left - idiffX, (cy / 2) + 25, oRect.Width(), oRect.Height() );

	GetDlgItem(IDC_BT_CLEAR)->GetWindowRect(&oRect);
	ScreenToClient(oRect);
	GetDlgItem(IDC_BT_CLEAR)->MoveWindow( oRect.left - idiffX, (cy / 2) + 43, oRect.Width(), oRect.Height() );

	GetDlgItem(IDC_BT_START)->GetWindowRect(&oRect);
	ScreenToClient(oRect);
	GetDlgItem(IDC_BT_START)->MoveWindow( oRect.left - idiffX, oRect.top - idiffY, oRect.Width(), oRect.Height() );

	GetDlgItem(IDC_LIST_SCREENSHOT)->GetWindowRect(&oRect);
	ScreenToClient(oRect);
	GetDlgItem(IDC_LIST_SCREENSHOT)->MoveWindow( oRect.left, oRect.top, oRect.Width() - idiffX, oRect.Height() - idiffY );
	
	Invalidate();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnBtLoad()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Choose the file to import", 3, 0, 0, NULL);//, "*.htm");
	CString				o_file;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		o_file = CString(o_File.masz_FullPath) + CString("/") + o_File.mo_File.MakeLower();
		if(o_file.ReverseFind('/') == o_file.GetLength() - 1)
			o_file.Delete(o_file.GetLength() - 1, 1);

		g_oXeScreenshotSeqBuilder.Load((char*) (LPCSTR)o_file);

		UpdateList();
		RefreshControls();
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnBtSave()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Enter the file name", 1);
	CString				strFileName;
	char				asz_FullPath[BIG_C_MaxLenPath];
	char				*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		/* Get file name */
		o_File.GetItem(o_File.mo_File, 1, strFileName);
		psz_Temp = (char *) (LPCSTR) strFileName;

		/* Compute full name on disk of new file */
		L_strcpy(asz_FullPath, o_File.masz_FullPath);
		if(asz_FullPath[L_strlen(asz_FullPath) - 1] != '\\') L_strcat(asz_FullPath, "\\");
		L_strcat(asz_FullPath, psz_Temp);

		/* Check if file already exists */
		if(!L_access(asz_FullPath, 0))
			if(MessageBox(EDI_STR_Csz_ConfirmOverwrite, EDI_STR_Csz_TitleConfirm, MB_YESNO | MB_ICONQUESTION) == IDNO)
				return;

		g_oXeScreenshotSeqBuilder.Save(asz_FullPath);
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnBtStart()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_DisplayData_	*pst_DD;
	EOUT_cl_Frame			*po_3DV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//get Display Data
	po_3DV = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	if (!po_3DV) return;

	pst_DD = po_3DV->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData;

	//update Capture flags
	if(pst_DD)
	{
		if(m_bSequenceStarted)
			pst_DD->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_ForSeqBuilder;
		else
			pst_DD->uc_SnapshotFlag |= GDI_Cc_CaptureFlag_ForSeqBuilder;
	}

	m_bSequenceStarted = !m_bSequenceStarted;

	GetDlgItem(IDC_BT_START)->SetWindowText(m_bSequenceStarted ? "Stop" : "Start");
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnBtUp()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl*	po_List;
	POSITION	pos;
	int			j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListCtrl *) GetDlgItem(IDC_LIST_SCREENSHOT);

	pos = po_List->GetFirstSelectedItemPosition();
	while(pos)
	{
		j = po_List->GetNextSelectedItem(pos);

		if(j == -1)
			break;

		g_oXeScreenshotSeqBuilder.Up(j);
	}

	if(j != -1 && j != 0)
	{
		UpdateList();
		po_List->SetItemState(j - 1, LVIS_SELECTED, LVIS_SELECTED);
	}

	RefreshControls();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnBtDown()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl*	po_List;
	POSITION	pos;
	int			j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListCtrl *) GetDlgItem(IDC_LIST_SCREENSHOT);

	pos = po_List->GetFirstSelectedItemPosition();
	while(pos)
	{
		j = po_List->GetNextSelectedItem(pos);

		if(j == -1)
			break;

		g_oXeScreenshotSeqBuilder.Down(j);
	}

	if(j != -1 && j != (po_List->GetItemCount() -1))
	{
		UpdateList();
		po_List->SetItemState(j + 1, LVIS_SELECTED, LVIS_SELECTED);
	}

	RefreshControls();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnBtDelete()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl*	po_List;
	POSITION	pos;
	int			j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListCtrl *) GetDlgItem(IDC_LIST_SCREENSHOT);

	pos = po_List->GetFirstSelectedItemPosition();
	while(pos)
	{
		j = po_List->GetNextSelectedItem(pos);

		if(j == -1)
			break;

		g_oXeScreenshotSeqBuilder.Delete(j);
	}

	UpdateList();

	if(j != -1)
	{
        if(j == 0)
			po_List->SetItemState(j, LVIS_SELECTED, LVIS_SELECTED);
		else
			po_List->SetItemState(j - 1, LVIS_SELECTED, LVIS_SELECTED);
	}

	RefreshControls();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnBtClear()
{
	if(MessageBox("Clear screenshot list ?", EDI_STR_Csz_TitleConfirm, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		g_oXeScreenshotSeqBuilder.Clear();

		UpdateList();
		RefreshControls();
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnListLButtonDown(NMHDR *pNMHDR, LRESULT *pResult)
{
	RefreshControls();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::OnListDblClk(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix* pst_Matrix;
	CListCtrl*	po_List;
	POSITION	pos;
	int			j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListCtrl *) GetDlgItem(IDC_LIST_SCREENSHOT);

	pos = po_List->GetFirstSelectedItemPosition();
	if(pos)
	{
		j = po_List->GetNextSelectedItem(pos);

		if(j!= -1)
		{
			if(mpo_View)
			{
				pst_Matrix = g_oXeScreenshotSeqBuilder.GetScreenshotMatrix(j);
				mpo_View->SetCameraMatrix(pst_Matrix);
				LINK_Refresh();
			}

			RefreshControls();
		}
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::UpdateList()
{
	CListCtrl*		  po_List;
	CString			  strTemp;
	MATH_tdst_Vector* pst_Pos;
	int				  index;

	po_List = (CListCtrl*)GetDlgItem(IDC_LIST_SCREENSHOT);

	if(!po_List)
		return;

	po_List->DeleteAllItems();

	for(int i = 0; i < g_oXeScreenshotSeqBuilder.GetNbScreenshot(); i++)
	{
		pst_Pos = &g_oXeScreenshotSeqBuilder.GetScreenshotMatrix(i)->T;

		strTemp.Format(_T("%d"), i + 1);
		index = po_List->InsertItem(i, strTemp, -1);

		//display screenshot name
		strTemp = g_oXeScreenshotSeqBuilder.GetScreenshotName(i);
		po_List->SetItemText(index, 1, strTemp);

		//display screenshot pos
		strTemp.Format(_T("x: %.2f, y: %.2f, z: %.2f"), pst_Pos->x, pst_Pos->y, pst_Pos->z);
		po_List->SetItemText(index, 2, strTemp);
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ScreenshotSeqBuilderDlg::RefreshControls()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl*	po_List;
	POSITION	pos;
	BOOL		bSelected;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListCtrl *) GetDlgItem(IDC_LIST_SCREENSHOT);

	if(po_List)
	{
		pos = po_List->GetFirstSelectedItemPosition();
		if(pos)
		{
			if(po_List->GetNextSelectedItem(pos) != -1)
				bSelected = TRUE;
			else
				bSelected = FALSE;
		}
		else
			bSelected = FALSE;

		//update buttons
		((CButton *) GetDlgItem(IDC_BT_UP)    )->EnableWindow(bSelected);
		((CButton *) GetDlgItem(IDC_BT_DOWN)  )->EnableWindow(bSelected);
		((CButton *) GetDlgItem(IDC_BT_DELETE))->EnableWindow(bSelected);
	}
}

#endif /* ACTIVE_EDITORS */

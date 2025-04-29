/*$T DIAcreategeometry_dlg.cpp GC! 1.081 03/26/01 09:21:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "F3DFrame/F3Dview.h"
#include "DIAlogs/DIAgamematerial_dlg.h"
#include "DIAlogs/DIAgamematerial_inside.h"
#include "EDIpaths.h"
#include "Res/Res.h"

extern BOOL	COL_gb_SoundColorDisplay;

BEGIN_MESSAGE_MAP(EDIA_cl_GameMaterial, EDIA_cl_BaseDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GameMaterial::EDIA_cl_GameMaterial(F3D_cl_View *_po_View, char *_sz_Path) : EDIA_cl_BaseDialog(DIALOGS_IDD_GAMEMATERIAL)
{
	msz_Path = _sz_Path;
	if(msz_Path == NULL) msz_Path = EDI_Csz_Path_COLGameMaterials;

	mpo_Frame = NULL;
	mpo_MainSplitter = NULL;
	mpo_DataView = NULL;

	mpo_3DView = _po_View;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GameMaterial::~EDIA_cl_GameMaterial(void)
{
	if(mpo_Frame)
	{
        /* destroy window */
		mpo_MainSplitter->DestroyWindow();
		delete mpo_MainSplitter;
		delete mpo_DataView;

        mpo_3DView->mpo_GameMaterialDialog = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GameMaterial::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RECT						st_Rect = { 10, 10, 1, 1 };
	CListCtrl					*pList;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_Frame = new CFrameWnd;
	mpo_MainSplitter = new CSplitterWnd;
	mpo_DataView = new EVAV_cl_View;

	mpo_Frame->Create(NULL, "", WS_CHILD , st_Rect, this);
	mpo_Frame->ShowWindow(SW_SHOW);

	mpo_MainSplitter->CreateStatic(mpo_Frame, 1, 2);
	mpo_MainSplitter->CreateView(0, 0, RUNTIME_CLASS(EDIA_cl_GameMaterialView), CSize(0, 0), NULL);
	mpo_View = (EDIA_cl_GameMaterialView *) mpo_MainSplitter->GetPane(0, 0);
	mpo_View->mpo_Dialog = this;

	L_strcpy(mpo_View->msz_Path, msz_Path);
	mpo_View->msz_Name[0] = 0; 

	pList = (CListCtrl *)mpo_View->GetDlgItem(IDC_LIST_GAMEMATERIAL);
	pList->InsertColumn(0, "ID", LVCFMT_LEFT, 55);
	pList->InsertColumn(1, "Name", LVCFMT_LEFT, 120);
	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT );


	mpo_DataView->mb_CanDragDrop = FALSE;
	mst_DataView.po_ListItems = &mo_ListItems;
	mst_DataView.psz_NameCol1 = "Name";
	mst_DataView.i_WidthCol1 = 230;
	mst_DataView.psz_NameCol2 = "Value";
	mpo_DataView->MyCreate(mpo_MainSplitter, &mst_DataView, mpo_Frame, mpo_MainSplitter->IdFromRowCol(0, 1));
	mpo_DataView->ShowWindow(SW_SHOW);

	CenterWindow();

	GetClientRect(&st_Rect);
	
	mpo_Frame->MoveWindow(&st_Rect);

	mpo_MainSplitter->SetColumnInfo(0, 315, 0);
	mpo_MainSplitter->RecalcLayout();
	mpo_MainSplitter->ShowWindow(SW_SHOW);

	mpo_View->GetDlgItem(IDC_CHECK_PAINT)->EnableWindow(FALSE);
	mpo_View->GetDlgItem(IDC_CHECK_PAINT_SOUND)->EnableWindow(FALSE);
	mpo_View->GetDlgItem(IDC_CHECK_SOUND)->EnableWindow(TRUE);
	mpo_View->GetDlgItem(IDC_CHECK_DISPLAY)->EnableWindow(FALSE);
	mpo_View->GetDlgItem(IDC_BUTTON_GOTO)->EnableWindow(FALSE);
	mpo_View->GetDlgItem(IDC_BUTTON_ELEMID)->EnableWindow(FALSE);

	((CButton *)mpo_View->GetDlgItem(IDC_CHECK_LINK))->SetCheck(BST_CHECKED);		
	((CButton *)mpo_View->GetDlgItem(IDC_CHECK_DISPLAY))->SetCheck(BST_UNCHECKED);		


	((CButton *)mpo_View->GetDlgItem(IDC_CHECK_PAINT))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_BRUSH));
	((CButton *)mpo_View->GetDlgItem(IDC_CHECK_PAINT_SOUND))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_BRUSH_SOUND));

	if(COL_gb_SoundColorDisplay)
		mpo_View->SetDlgItemText(IDC_CHECK_SOUND, "Sound Color");
	else
		mpo_View->SetDlgItemText(IDC_CHECK_SOUND, "ID Color");

	return TRUE;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterial::OnSize(UINT nType, int cx, int cy )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RECT						st_Rect = { 10, 10, 1, 1 };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnSize(nType, cx, cy);

	mpo_MainSplitter->ShowWindow(SW_HIDE);
	GetClientRect(&st_Rect);
	mpo_Frame->MoveWindow(&st_Rect);
	mpo_MainSplitter->RecalcLayout();

	mpo_View->GetDlgItem(IDC_LIST_GAMEMATERIAL)->MoveWindow(15, 70, 200, cy - 80);

	mpo_View->GetDlgItem(IDC_CHECK_LINK)->MoveWindow(225, cy -161, 80, 20);
	mpo_View->GetDlgItem(IDC_CHECK_SOUND)->MoveWindow(225, cy -119, 80, 20);

	mpo_View->GetDlgItem(IDC_BUTTON_GOTO)->MoveWindow(215, cy -51, 50, 20);
	mpo_View->GetDlgItem(IDC_BUTTON_ELEMID)->MoveWindow(266, cy -51, 49, 20);

	mpo_View->GetDlgItem(IDC_CHECK_PAINT)->MoveWindow(215, cy -30, 50, 20);
	mpo_View->GetDlgItem(IDC_CHECK_PAINT_SOUND)->MoveWindow(266, cy -30, 49, 20);

	mpo_View->GetDlgItem(IDC_CHECK_DISPLAY)->MoveWindow(215, cy -72, 49, 20);

	mpo_MainSplitter->ShowWindow(SW_SHOW);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GameMaterial::PreTranslateMessage(MSG *pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE) return 1;
        if(pMsg->wParam == VK_RETURN) return 1;
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}




#endif /* ACTIVE_EDITORS */

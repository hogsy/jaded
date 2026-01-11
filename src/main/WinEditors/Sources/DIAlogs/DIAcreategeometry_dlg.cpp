/*$T DIAcreategeometry_dlg.cpp GC! 1.081 03/26/01 09:21:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "DIAlogs/DIAcreategeometry_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "EDIpaths.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "F3DFrame/F3Dview.h"
#include "SOFT/SOFTbackgroundImage.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "INOut/INOjoystick.h"
#include "LINKs/LINKmsg.h"
#include "DIAlogs/DIAcreategeometry_inside.h"

BEGIN_MESSAGE_MAP(EDIA_cl_CreateGeometry, EDIA_cl_BaseDialog)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

#define CreateGeometry_IniVersion 6

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_CreateGeometry::EDIA_cl_CreateGeometry(F3D_cl_View *_po_View, char *_sz_Path, int _i_ColMap) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_CREATEGEOM_MAIN)
{
	msz_Path = _sz_Path;
	if(msz_Path == NULL) msz_Path = EDI_Csz_Path_ObjModels;

	mi_ColMap = _i_ColMap;
	mpo_Frame = NULL;
	mpo_MainSplitter = NULL;
	mpo_DataView = NULL;
	mpo_3DView = _po_View;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_CreateGeometry::~EDIA_cl_CreateGeometry(void)
{
    ULONG   ul_Version;
    RECT    st_Rect;

	if(mpo_Frame)
	{
        /* save ini file */
        SAV_Begin( EDI_Csz_Ini, "CreateGeometry.ini" );
        ul_Version = CreateGeometry_IniVersion;
        SAV_Buffer(&ul_Version, 4 );
        GetWindowRect( &st_Rect );
        SAV_Buffer( &st_Rect, sizeof( RECT ) );
        SAV_Buffer( &mpo_View->mst_TextData, sizeof( GEO_tdst_CO_Text ) );
        SAV_Buffer( &mpo_View->mst_PlaneData, sizeof( GEO_tdst_CO_Plane ) );
        SAV_Buffer( &mpo_View->mst_BoxData, sizeof( GEO_tdst_CO_Box ) );
        SAV_Buffer( &mpo_View->mst_SphereData, sizeof( GEO_tdst_CO_Sphere ) );
        SAV_Buffer( &mpo_View->mst_CylindreData, sizeof( GEO_tdst_CO_Cylindre ) );
        SAV_Buffer( &mpo_View->mst_GeosphereData, sizeof( GEO_tdst_CO_Geosphere ) );
        SAV_ul_End();

        /* destroy window */
        mpo_3DView->mpo_CreateGeometryDialog = NULL;
		mpo_MainSplitter->DestroyWindow();
		delete mpo_MainSplitter;
		delete mpo_DataView;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_CreateGeometry::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RECT						st_Rect = { 10, 10, 400, 300 };
    ULONG                       ul_Index;
    char                        *pc_Buf;
    BOOL                        b_Ini;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_Frame = new CFrameWnd;
	mpo_MainSplitter = new CSplitterWnd;
	mpo_DataView = new EVAV_cl_View;

	mpo_Frame->Create(NULL, "", WS_CHILD, st_Rect, this);
	mpo_Frame->ShowWindow(SW_SHOW);

	mpo_MainSplitter->CreateStatic(mpo_Frame, 1, 2);
	mpo_MainSplitter->CreateView(0, 0, RUNTIME_CLASS(EDIA_cl_CreateGeometryView), CSize(0, 0), NULL);
	mpo_View = (EDIA_cl_CreateGeometryView *) mpo_MainSplitter->GetPane(0, 0);
	mpo_View->mpo_Dialog = this;
	L_strcpy(mpo_View->msz_Path, msz_Path);
	mpo_View->msz_Name[0] = 0;
	mpo_View->Init();

    /* read init data */
    b_Ini = FALSE;
    ul_Index = BIG_ul_SearchFileExt( EDI_Csz_Ini, "CreateGeometry.ini" );
    if (ul_Index != BIG_C_InvalidIndex)
    {
        pc_Buf = BIG_pc_ReadFileTmp( BIG_PosFile( ul_Index ), &ul_Index);
        if ( *(ULONG *) pc_Buf == CreateGeometry_IniVersion )
        {
            pc_Buf += 4;
            L_memcpy( &st_Rect, pc_Buf, sizeof( RECT ) );
            pc_Buf += sizeof( RECT );
            L_memcpy( &mpo_View->mst_TextData, pc_Buf, sizeof( GEO_tdst_CO_Text ) );
            pc_Buf += sizeof( GEO_tdst_CO_Text );
            L_memcpy( &mpo_View->mst_PlaneData, pc_Buf, sizeof( GEO_tdst_CO_Plane ) );
            pc_Buf += sizeof( GEO_tdst_CO_Plane );
            L_memcpy( &mpo_View->mst_BoxData, pc_Buf, sizeof( GEO_tdst_CO_Box ) );
            pc_Buf += sizeof( GEO_tdst_CO_Box );
            L_memcpy( &mpo_View->mst_SphereData, pc_Buf, sizeof( GEO_tdst_CO_Sphere ) );
            pc_Buf += sizeof( GEO_tdst_CO_Sphere );
            L_memcpy( &mpo_View->mst_CylindreData, pc_Buf, sizeof( GEO_tdst_CO_Cylindre ) );
            pc_Buf += sizeof( GEO_tdst_CO_Cylindre );
            L_memcpy( &mpo_View->mst_SphereData, pc_Buf, sizeof( GEO_tdst_CO_Geosphere ) );
            pc_Buf += sizeof( GEO_tdst_CO_Geosphere );
            b_Ini = TRUE;
        }
    }
    

	mpo_DataView->mb_CanDragDrop = FALSE;
	mst_DataView.po_ListItems = &mo_ListItems;
	mst_DataView.psz_NameCol1 = "Name";
	mst_DataView.i_WidthCol1 = 100;
	mst_DataView.psz_NameCol2 = "Value";
	mpo_DataView->MyCreate(mpo_MainSplitter, &mst_DataView, mpo_Frame, mpo_MainSplitter->IdFromRowCol(0, 1));
	mpo_DataView->ShowWindow(SW_SHOW);

    if (b_Ini)
        MoveWindow( st_Rect.left, st_Rect.top, st_Rect.right - st_Rect.left, st_Rect.bottom - st_Rect.top );
    else
        CenterWindow();

	GetClientRect(&st_Rect);
	mpo_Frame->MoveWindow(&st_Rect);

	mpo_Frame->RecalcLayout();
	mpo_MainSplitter->SetColumnInfo(0, 260, 0);
	mpo_MainSplitter->ShowWindow(SW_SHOW);
	mpo_MainSplitter->RecalcLayout();


	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_CreateGeometry::PreTranslateMessage(MSG *pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE) return 1;
        if(pMsg->wParam == VK_RETURN) return 1;

		OnKeyDown(pMsg->wParam);
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometry::OnSize(UINT type, int cx, int cy)
{
	/*~~~~~~~~~~~~*/
	RECT	st_Rect;
	/*~~~~~~~~~~~~*/

	if(mpo_Frame)
	{
		st_Rect.left = 0;
		st_Rect.top = 0;
		st_Rect.bottom = cy;
		st_Rect.right = cx;
		mpo_Frame->MoveWindow(&st_Rect);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometry::OnOK(void)
{
	EDIA_cl_BaseDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometry::OnCancel(void)
{
	EDIA_cl_BaseDialog::OnCancel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometry::OnMouseMove(UINT ui_Flags, CPoint pt)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometry::OnLButtonDown(UINT ui, CPoint pt)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometry::OnLButtonUp(UINT ui, CPoint pt)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometry::OnRButtonDown(UINT ui, CPoint pt)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometry::OnKeyDown(UINT nChar)
{
}

#endif /* ACTIVE_EDITORS */

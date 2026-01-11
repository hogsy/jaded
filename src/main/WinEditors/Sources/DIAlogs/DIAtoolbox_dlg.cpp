/*$T DIAtoolbox_dlg.cpp GC! 1.100 06/15/01 12:26:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "LINKs/LINKtoed.h"
#include "BASe/CLIbrary/CLImem.h"
#include "Res/Res.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "SELection/SELection.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "SELection/SELection.h"
#include "DIAlogs/DIAtoolbox_groview.h"
#include "DIAlogs/DIAtoolbox_curveview.h"
#include "DIAlogs/DIAshape_dlg.h"
#include "DIAlogs/DIAuvmapper_dlg.h"
#include "DIAlogs/BtnST.h"
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEOsubobject.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "F3Dframe/F3Dframe.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "LINks\LINKstruct.h"

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    EDIA_cl_SelectionDialog Message map
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_ToolBoxDialog, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(IDC_CLOSESEL, OnCloseSel)
	ON_BN_CLICKED(IDC_MINIMIZE, OnMinimize)
	ON_BN_CLICKED(IDC_SELECTION, OnSelection)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_VIEW, OnSelchangeTabView)
	ON_BN_CLICKED(IDC_CHECK_SELECTIONTYPE, OnSelectionType)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    EDIA_cl_SelectionDialog implementation
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBoxDialog::EDIA_cl_ToolBoxDialog(F3D_cl_View *_po_View) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_TOOLBOX)
{
	mpo_View = _po_View;
	mpst_Gao = NULL;
	mpst_Gro = NULL;

	mao_SelectionButtonBmp[0].LoadBitmap(IDB_BITMAP_CROSSINGSELECTION);
	mao_SelectionButtonBmp[1].LoadBitmap(IDB_BITMAP_WINDOWSELECTION);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBoxDialog::~EDIA_cl_ToolBoxDialog(void)
{
	mpo_View->mpo_ToolBoxDialog = NULL;

	mao_SelectionButtonBmp[0].DeleteObject();
	mao_SelectionButtonBmp[1].DeleteObject();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_ToolBoxDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CRect		o_FrameRect;
	CTabCtrl	*po_TC;
	/*~~~~~~~~~~~~~~~~~~~~*/

	/* Base class call */
	EDIA_cl_BaseDialog::OnInitDialog();

	o_FrameRect = CRect(0, 0, 10, 10);

	po_TC = (CTabCtrl *) GetDlgItem(IDC_TAB_VIEW);
	po_TC->DeleteAllItems();
	po_TC->InsertItem(0, "Geometry");
	po_TC->InsertItem(1, "Curve");
	po_TC->InsertItem(2, "Shape");

	o_FrameRect.OffsetRect(-o_FrameRect.left - 2, -o_FrameRect.top - 2);

	mpo_ToolView[0] = new EDIA_cl_ToolBox_GROView(this);
	((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->Create("Geometry", "Geometry", 0, o_FrameRect, po_TC);
	mpo_ToolView[0]->ShowWindow(SW_HIDE);

	mpo_ToolView[1] = new EDIA_cl_ToolBox_CurveView(this);
	((EDIA_cl_ToolBox_CurveView *) mpo_ToolView[1])->Create("Curve", "Curve", 0, o_FrameRect, po_TC);
	mpo_ToolView[1]->ShowWindow(SW_HIDE);

	mpo_ToolView[2] = new EDIA_cl_ShapeDialog(this);
	((EDIA_cl_ShapeDialog *) mpo_ToolView[2])->Create("Shape", "Shape", 0, o_FrameRect, po_TC);
	((EDIA_cl_ShapeDialog *) mpo_ToolView[2])->OnInitDialog();
	mpo_ToolView[2]->ShowWindow(SW_HIDE);

	if(mpo_View->mpo_Frame->mb_SelMinimize)
		GetDlgItem(IDC_MINIMIZE)->SetWindowText("+");
	else
		GetDlgItem(IDC_MINIMIZE)->SetWindowText("-");

	UpdateSelectionButton();

	EDIA_cl_BaseDialog::SetTheme(this);
	EDIA_cl_BaseDialog::SetTheme(mpo_ToolView[0]);
	EDIA_cl_BaseDialog::SetTheme(mpo_ToolView[1]);
	EDIA_cl_BaseDialog::SetTheme(mpo_ToolView[2]);

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::OnSize(UINT, int cx, int cy)
{
	/*~~~~~~~~~~~~*/
	CWnd	*po_Wnd;
	int		i;
	/*~~~~~~~~~~~~*/

	po_Wnd = GetDlgItem(IDC_TAB_VIEW);
	if(!po_Wnd) return;
	po_Wnd->MoveWindow(CRect(2, 25, cx - 2, cy - 5));
	for(i = 0; i < DIA_ToolBox_NumberOfView; i++)
	{
		mpo_ToolView[i]->MoveWindow(CRect(2, 23, cx - 7, cy - 32));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::OnCloseSel(void)
{
	mpo_View->Selection_ToolBox();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::FirstDraw(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CTabCtrl	*ptab;
	int			i, i_Pane;
	/*~~~~~~~~~~~~~~~~~~*/

	ptab = (CTabCtrl *) GetDlgItem(IDC_TAB_VIEW);
	i_Pane = ptab->GetCurSel();
	for(i = 0; i < DIA_ToolBox_NumberOfView; i++)
	{
		mpo_ToolView[i]->ShowWindow((i == i_Pane) ? SW_SHOW : SW_HIDE);
		if(i == i_Pane) mpo_ToolView[i]->BringWindowToTop();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::OnSelchangeTabView(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CTabCtrl	*ptab;
	int			i, i_Pane;
	/*~~~~~~~~~~~~~~~~~~*/

	ptab = (CTabCtrl *) GetDlgItem(IDC_TAB_VIEW);
	i_Pane = ptab->GetCurFocus();

	for(i = 0; i < DIA_ToolBox_NumberOfView; i++)
	{
#ifdef JADEFUSION
        if (i == i_Pane)
        {
            mpo_ToolView[i]->ShowWindow(SW_SHOW);
            mpo_ToolView[i]->BringWindowToTop();
        }
        else
        {
            mpo_ToolView[i]->ShowWindow(SW_HIDE);
        }
#else
		mpo_ToolView[i]->ShowWindow((i == i_Pane) ? SW_SHOW : SW_HIDE);
		if(i == i_Pane) mpo_ToolView[i]->BringWindowToTop();
#endif
	}

	*pResult = 0;
}

/*$4
 ***********************************************************************************************************************
    Selection
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog_Update(ULONG ul_Dlg, GEO_tdst_Object *_pst_Obj )
{
    EDIA_cl_ToolBoxDialog *po_Tools;

    if (ul_Dlg == 0) return;
    po_Tools = (EDIA_cl_ToolBoxDialog *) ul_Dlg;

    if (!_pst_Obj) return;
    if (_pst_Obj->st_Id.i->ul_Type != GRO_Geometric ) return;
    if (!_pst_Obj->pst_SubObject ) return;

    if (_pst_Obj != (GEO_tdst_Object *) po_Tools->mpst_Gro)
    {
        _pst_Obj->pst_SubObject->pfn_Tool_Update = NULL;
        _pst_Obj->pst_SubObject->ul_Tool_Param = 0;
        return;
    }

    ((EDIA_cl_ToolBox_GROView *) po_Tools->mpo_ToolView[0])->UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::SetGro(GRO_tdst_Struct *_pst_Gro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL	b_SkinDialog = FALSE;
	int		p;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->mpst_EditOption->ul_Flags & GRO_Cul_EOF_SkinMode )
		b_SkinDialog = ((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->mpst_SkinDialog != NULL;
	else
		((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->mpst_SkinDialog = NULL;

	if(b_SkinDialog) ((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->On_EditSkin();

	if(LINK_gx_PointersJustDeleted.Lookup(mpst_Gro, (void * &) p)) 
		mpst_Gro = NULL;

    if ( (mpst_Gro) && (mpst_Gro->i->ul_Type == GRO_Geometric) && (((GEO_tdst_Object *) mpst_Gro)->pst_SubObject) )
    {
        ((GEO_tdst_Object *) mpst_Gro)->pst_SubObject->ul_Tool_Param = NULL;
        ((GEO_tdst_Object *) mpst_Gro)->pst_SubObject->pfn_Tool_Update = NULL;
    }

	mpst_Gro = _pst_Gro;

    if ( (mpst_Gro) && (mpst_Gro->i->ul_Type == GRO_Geometric) && (((GEO_tdst_Object *) mpst_Gro)->pst_SubObject) )
    {
        ((GEO_tdst_Object *) mpst_Gro)->pst_SubObject->ul_Tool_Param = (ULONG) this;
        ((GEO_tdst_Object *) mpst_Gro)->pst_SubObject->pfn_Tool_Update = EDIA_cl_ToolBoxDialog_Update;

        // Set editor data with object data
        /*if (((GEO_tdst_Object *) mpst_Gro)->p_MRM_Levels)
        {
            ULONG i;
            ((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->mpst_EditOption->ul_MRMLevelNb = ((GEO_tdst_Object *) mpst_Gro)->p_MRM_Levels->ul_LevelNb;

            for (i=0; i<((GEO_tdst_Object *) mpst_Gro)->p_MRM_Levels->ul_LevelNb; i++)
                ((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->mpst_EditOption->f_MRMLevels[i] = ((GEO_tdst_Object *) mpst_Gro)->p_MRM_Levels->f_Thresholds[i];
        }*/
    }

	for(int i = 0; i < DIA_ToolBox_NumberOfView; i++)
	{
		if(mpo_ToolView[i]) mpo_ToolView[i]->Invalidate();
	}

	if(b_SkinDialog)
		((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->On_EditSkin();
	else
		((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::SelectGaoFromPtr(OBJ_tdst_GameObject *pst_GaoLocal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Gao;
	GRO_tdst_Struct		*pst_Gro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Gao = pst_GaoLocal;
	if((ULONG) pst_Gao == -1) pst_Gao = NULL;

	mpst_Gao = pst_Gao;
	pst_Gro = (GRO_tdst_Struct *) OBJ_p_GetGro(pst_Gao);

	if(mpst_Gao && (pst_Gro == NULL))
	{
		if(pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights) pst_Gro = pst_Gao->pst_Extended->pst_Light;
	}

	SetGro(pst_Gro);

	if(((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->mpo_UVMapper)
		((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->mpo_UVMapper->UpdateGao();
	((EDIA_cl_ShapeDialog *) mpo_ToolView[2])->UpdateList(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::SelectGao(ULONG _ul_Index)
{
	SelectGaoFromPtr((OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(_ul_Index)));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::OneTrameEnding(void)
{
	((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->OneTrameEnding();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::Reset(void)
{
	SelectGaoFromPtr(mpo_View->Selection_pst_GetFirstObject());
	((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::OnMinimize(void)
{
	mpo_View->mpo_Frame->MinMaxSel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::OnSelection(void)
{
	mpo_View->Selection_WithDialog();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::UpdateSelectionButton(void)
{
	/*~~~~~~~~~~~~~~~*/
	int		i_Bmp;
	CButton *po_Button;
	/*~~~~~~~~~~~~~~~*/

	i_Bmp = mpo_View->mst_WinHandles.pst_DisplayData->pst_PickingBuffer->ul_UserFlags & SOFT_Cul_PBF_WindowSelection;
	i_Bmp = i_Bmp ? 1 : 0;
	po_Button = (CButton *) GetDlgItem(IDC_CHECK_SELECTIONTYPE);
	po_Button->SetBitmap((HBITMAP) mao_SelectionButtonBmp[i_Bmp].GetSafeHandle());
	po_Button->SetCheck(i_Bmp);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::OnSelectionType(void)
{
	/*~~~~~~~~~~~~~~~*/
	CButton *po_Button;
	/*~~~~~~~~~~~~~~~*/

	po_Button = (CButton *) GetDlgItem(IDC_CHECK_SELECTIONTYPE);
	if(po_Button->GetCheck())
	{
		mpo_View->mst_WinHandles.pst_DisplayData->pst_PickingBuffer->ul_UserFlags |= SOFT_Cul_PBF_WindowSelection;
	}
	else
	{
		mpo_View->mst_WinHandles.pst_DisplayData->pst_PickingBuffer->ul_UserFlags &= ~SOFT_Cul_PBF_WindowSelection;
	}

	UpdateSelectionButton();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::UpdateFaceId(int i)
{
	((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->UpdateFaceId(i);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_ToolBoxDialog::b_CanDrop(POINT pt)
{
    if (mpo_ToolView[0]->IsWindowVisible() )
        return ((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->b_CanDrop( pt );
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::DropGro( BIG_INDEX _ul_Index )
{
    if (mpo_ToolView[0]->IsWindowVisible() )
        ((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->DropGro( _ul_Index );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBoxDialog::DropGao( BIG_INDEX _ul_Index )
{
    if (mpo_ToolView[0]->IsWindowVisible() )
        ((EDIA_cl_ToolBox_GROView *) mpo_ToolView[0])->DropGao( _ul_Index );
}


#endif /* ACTIVE_EDITORS */

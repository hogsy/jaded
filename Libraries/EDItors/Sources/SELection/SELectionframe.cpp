//------------------------------------------------------------------------------
// Filename   SELectionframe.cpp
/// \author    Nbeaufils
/// \date      2005-01-17
/// \par       Description: Implementation of SELectionframe
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "SELectionframe.h"
#include "SELectionGridData.h"
#include "MENu/MENmenu.h"
#include "MENu/MENsubmenu.h"
#include "res/Res.h"
#include "SELection/SELection.h"
#include "WORld/WORstruct.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "Output/OUTframe.h"
#include "OBJects\OBJaccess.h"
#include "EDIpaths.h"
#include "EDIicons.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "EDIapp.h"
#include "BIGfiles\SAVing\SAVdefs.h"
#include "BIGfiles\LOAding\LOAdefs.h"
#include <set>

#define ACTION_GLOBAL
#include "SELectionframe_act.h"

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------
#define IDC_GRID_CTRL 333

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(ESELection_cl_Frame, EDI_cl_BaseFrame)

BEGIN_MESSAGE_MAP(ESELection_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()		
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

//------------------------------------------------------------
//   ESELection_cl_Frame::ESELection_cl_Frame(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ESELection_cl_Frame::ESELection_cl_Frame(void) : 
EDI_cl_BaseFrame(),
mp_GridCtrl(NULL)
{
	// Initialize GRID CTRL
	mp_GridCtrl = new SELectionGridCtrl;

	mb_ShowStandard = TRUE;
	mb_ShowShading = TRUE;
	mb_ShowNormalMap = TRUE;
	mb_ShowDetailNormalMap = TRUE;
	mb_ShowSpecular = TRUE;
	mb_ShowEnvironmentMap = TRUE;
	mb_ShowMossMap = TRUE;
    mb_ShowRimLight = TRUE;
	mb_ShowMeshProcessing = TRUE;
}

//------------------------------------------------------------
//   ESELection_cl_Frame::~ESELection_cl_Frame(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ESELection_cl_Frame::~ESELection_cl_Frame(void)
{
	// Delete GRID CTRL
	if ( mp_GridCtrl )
		delete mp_GridCtrl ;
}

//------------------------------------------------------------
//   int ESELection_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int ESELection_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1)
	{
		return -1;
	}
	return 0;
}

//------------------------------------------------------------
//   BOOL ESELection_cl_Frame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
/// \author    NBeaufils
/// \date      21-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
BOOL ESELection_cl_Frame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CRect rect;                  
	GetClientRect(rect);
	mp_GridCtrl->Create(rect, this, IDC_GRID_CTRL);

	mp_GridCtrl->InitializeGrid();

	return EDI_cl_BaseFrame::OnCreateClient(lpcs, pContext);
}

//------------------------------------------------------------
//   void ESELection_cl_Frame::OnSize(UINT nType, int cx, int cy)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void ESELection_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	if (mp_GridCtrl->GetSafeHwnd())     // Have the grid object and window 
	{                               // been created yet?
		CRect rect;
		GetClientRect(rect);        // Get the size of the view's client
		mp_GridCtrl->MoveWindow(rect);  // Resize the grid to take up that 
	}
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);
}

//------------------------------------------------------------
//   void ESELection_cl_Frame::OnClose(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void ESELection_cl_Frame::OnClose(void)
{
}

//------------------------------------------------------------
//   BOOL ESELection_cl_Frame::b_CanActivate(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL ESELection_cl_Frame::b_CanActivate(void)
{
	return TRUE;
}

//------------------------------------------------------------
//   void ESELection_cl_Frame::OnCtrlPopup(CPoint pt)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void ESELection_cl_Frame::OnCtrlPopup(CPoint pt)
{
	BOOL bSingleCellSelected = FALSE;
	CCellRange range = mp_GridCtrl->GetSelectedCellRange();
	if ( range.GetRowSpan() == 1 && range.GetColSpan() == 1 )
		bSingleCellSelected = TRUE;

	EMEN_cl_SubMenu o_Menu(FALSE);

	InitPopupMenuAction(&o_Menu);

	AddPopupMenuAction(&o_Menu, ESELECTION_ACTION_PERFORCE_EDIT);
	AddPopupMenuAction(&o_Menu, ESELECTION_ACTION_PERFORCE_REVERT);

	if ( bSingleCellSelected )
	{
		AddPopupMenuAction(&o_Menu, 0);
		AddPopupMenuAction(&o_Menu, ESELECTION_ACTION_PERFORCE_DIFF);
		AddPopupMenuAction(&o_Menu, ESELECTION_ACTION_PERFORCE_HISTORY);
	}
	AddPopupMenuAction(&o_Menu, 0);
	AddPopupMenuAction(&o_Menu, ESELECTION_ACTION_SHOW_INMATERIAL);
	AddPopupMenuAction(&o_Menu, ESELECTION_ACTION_SHOW_SELECTED);
	AddPopupMenuAction(&o_Menu, ESELECTION_ACTION_SHOW_ALL);
	
	TrackPopupMenuAction(pt, &o_Menu);
}

//------------------------------------------------------------
//   void ESELection_cl_Frame::OpenProject(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void ESELection_cl_Frame::OpenProject(void)
{
	EDI_cl_BaseFrame::OpenProject();
}

//------------------------------------------------------------
//   void ESELection_cl_Frame::CloseProject(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void ESELection_cl_Frame::CloseProject(void)
{

}

//------------------------------------------------------------
//   void InsertRowsGrid_CB(SEL_tdst_SelectedItem *_pst_Sel, ULONG ulTmp1, ULONG ulParam)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL InsertRowsGrid_CB(SEL_tdst_SelectedItem* _pst_Sel, ULONG ulGridCtrl, ULONG ulParam)
{
	OBJ_tdst_GameObject* pst_Gao = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	
	if ( OBJ_b_TestIdentityFlag(pst_Gao, OBJ_C_IdentityFlag_Visu) )
	{
		GRO_tdst_Struct* pst_Gmt = pst_Gao->pst_Base->pst_Visu->pst_Material;
		if ( pst_Gmt != NULL )
		{
			((SELectionGridCtrl*)ulGridCtrl)->InitializeData(pst_Gmt);
		}
	}
	return TRUE;
}

//------------------------------------------------------------
//   void ESELection_cl_Frame::InsertRowsGrid(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void ESELection_cl_Frame::InsertRowsGrid()
{
	EOUT_cl_Frame* po_Out = NULL;
	SEL_tdst_Selection* pst_Sel = NULL;
	
	// Get pointer to 3D View
	po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	
	// Get pointer to selection
	if ( po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World != NULL && 
			po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_Selection != NULL )
	{
		mp_GridCtrl->DeleteNonFixedRows();
		pst_Sel = po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_Selection ;
		SEL_EnumItem(pst_Sel, SEL_C_SIF_Object, InsertRowsGrid_CB, (ULONG)mp_GridCtrl, (ULONG)NULL);
		mp_GridCtrl->Refresh();
	}
}

//------------------------------------------------------------
//   void ESELection_cl_Frame::RemoveRowsGrid(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void ESELection_cl_Frame::RemoveRowsGrid()
{
	mp_GridCtrl->Refresh();
}

//------------------------------------------------------------
//   void ESELection_cl_Frame::RemoveRowsGrid(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void ESELection_cl_Frame::SaveMaterials(void)
{
	M_MF()->BeginWaitCursor();

	std::set<BIG_INDEX> setIndexSaved;
	
	// Save to big file each row in grid
	for ( int iRow=1 ; iRow < mp_GridCtrl->GetRowCount() ; iRow++ )
	{
		if ( mp_GridCtrl->GetRowHeight(iRow) > 0 )
		{
			SELectionGridCtrl_Textures* pst_SELTextures = (SELectionGridCtrl_Textures*)mp_GridCtrl->GetItemData(iRow, eCOL_MaterialName);
			BIG_INDEX ulIndex = LOA_ul_SearchIndexWithAddress((ULONG)pst_SELTextures->pst_ACTIVE_GRO);

			// Check in GRM has already been saved	
			if ( setIndexSaved.find(ulIndex) == setIndexSaved.end() )		
			{
				char sz_Path[BIG_C_MaxLenPath];
				BIG_ComputeFullName(BIG_ParentFile(ulIndex), sz_Path);

				SAV_Begin(sz_Path, BIG_NameFile(ulIndex));
				pst_SELTextures->pst_ACTIVE_GRO->st_Id.i->pfnl_SaveInBuffer(pst_SELTextures->pst_ACTIVE_GRO, &TEX_gst_GlobalList);
				SAV_ul_End();

				setIndexSaved.insert(ulIndex);
			}
		}
	}
	LINK_PrintStatusMsg("Saving material OK");

	M_MF()->EndWaitCursor();
}

#endif /* ACTIVE_EDITORS */

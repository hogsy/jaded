//------------------------------------------------------------------------------
// Filename   PERCheckframe.cpp
/// \author    Nbeaufils
/// \date      2005-01-17
/// \par       Description: Implementation of PERCheckframe
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "res/Res.h"

#include "EDIpaths.h"
#include "EDIicons.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "EDIapp.h"

#include "EDItors/Sources/MENu/MENmenu.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"

#include "DATaControl/DATCPerforce.h"

#include "PERforce/PERCframe.h"

#include "PERCheckframe.h"
#include "PERCheckTopView.h"
#include "PERCheckBottomView.h"

#ifdef JADEFUSION
#include "DIAlogs\DATCP4FileFilter.h"
#endif
#include "afxcview.h"

#define ACTION_GLOBAL
#include "PERCheckframe_act.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------
//#define ID_EDIT_FIELD   333
//#define ID_TREE_LIST_HEADER 337
//#define ID_TREE_LIST_CTRL   373
//#define ID_TREE_LIST_SCROLLBAR   377

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------


IMPLEMENT_DYNCREATE(EPERCheck_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EPERCheck_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()		
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDC_P4CHECK_FILTERSAME, OnCheckFilterSame)
	ON_COMMAND(IDC_P4CHECK_FILTERMODIFIED, OnCheckFilterModified)
	ON_COMMAND(IDC_P4CHECK_FILTERNEW, OnCheckFilterNew)
	ON_COMMAND(IDC_P4CHECK_FILTERRENAMED, OnCheckFilterRenamed)
	ON_COMMAND(IDC_P4CHECK_FILTERMOVED, OnCheckFilterMoved)
	ON_COMMAND(IDC_P4CHECK_FILTERDELETED, OnCheckFilterDeleted)
	ON_COMMAND(IDC_P4CHECK_FILTERNEWP4, OnCheckFilterNewP4)

#ifdef JADEFUSION
	ON_COMMAND(IDC_P4CHECK_FILTERSETTINGS, OnCheckFilterSettings)
	ON_COMMAND(IDC_P4BUTTON_FILTERSETTINGS, OnClickFilterSettings)
#endif
END_MESSAGE_MAP()


//------------------------------------------------------------
//   EPERCheck_cl_Frame::EPERCheck_cl_Frame(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
EPERCheck_cl_Frame::EPERCheck_cl_Frame(void) : EDI_cl_BaseFrame()
{
	mpo_Splitter = new CSplitterWnd;
	mpo_Splitter2 = new CSplitterWnd;

	m_bSplitterCreated = FALSE;
}

//------------------------------------------------------------
//   EPERCheck_cl_Frame::~EPERCheck_cl_Frame(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
EPERCheck_cl_Frame::~EPERCheck_cl_Frame(void)
{
	mpo_Splitter->DestroyWindow();
	delete mpo_Splitter;

	mpo_Splitter2->DestroyWindow();
	delete mpo_Splitter2;

	EPERCheck_cl_Manager::GetInstance()->ClearDiffStruct();
}

//------------------------------------------------------------
//   int EPERCheck_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int EPERCheck_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1)
	{
		return -1;
	}

	// Create splitter that will contain both Diff and Files not in common windows
	mpo_Splitter2->CreateStatic(this, 2, 1, WS_CHILD | WS_VISIBLE);
	mpo_Splitter2->CreateView(1,0, RUNTIME_CLASS(EPERCheckBottom_cl_View), CSize(200,100), NULL);

	// Create splitter that will contain Diff
	mpo_Splitter->CreateStatic(mpo_Splitter2, 1, 2, WS_CHILD | WS_VISIBLE, mpo_Splitter2->IdFromRowCol(0, 0));
	mpo_Splitter->CreateView(0,0, RUNTIME_CLASS(EPERCheckTop_cl_View), CSize(200,100), NULL);
	mpo_Splitter->CreateView(0,1, RUNTIME_CLASS(EPERCheckTop_cl_View), CSize(200,100), NULL);

	m_bSplitterCreated = TRUE;

	mpo_Splitter2->SetRowInfo(0, 300, 50);
	mpo_Splitter2->SetRowInfo(1, 300, 50);

	mpo_Splitter->SetColumnInfo(0, 200,100);
	mpo_Splitter->SetColumnInfo(1, 200,100);

	mpo_Splitter->RecalcLayout();
	mpo_Splitter2->RecalcLayout();

	RecalcLayout();

	mpo_DialogBar->CheckDlgButton(IDC_P4CHECK_FILTERSAME, FALSE);
	mpo_DialogBar->CheckDlgButton(IDC_P4CHECK_FILTERNEW, TRUE);
	mpo_DialogBar->CheckDlgButton(IDC_P4CHECK_FILTERMODIFIED, TRUE);
	mpo_DialogBar->CheckDlgButton(IDC_P4CHECK_FILTERMOVED, TRUE);
	mpo_DialogBar->CheckDlgButton(IDC_P4CHECK_FILTERRENAMED, TRUE);
#ifdef JADEFUSION
	mpo_DialogBar->CheckDlgButton(IDC_P4CHECK_FILTERSETTINGS, FALSE);
#endif
	return 0;
}

//------------------------------------------------------------
//   BOOL EPERCheck_cl_Frame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
/// \author    NBeaufils
/// \date      21-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
BOOL EPERCheck_cl_Frame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	return EDI_cl_BaseFrame::OnCreateClient(lpcs, pContext);
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OpenProject(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OpenProject(void)
{
	EDI_cl_BaseFrame::OpenProject();
#ifdef JADEFUSION
	// Set file filter for both windows
	GetTopLeftWindow()->SetFileFilter(&m_FileFilter);
	GetTopRightWindow()->SetFileFilter(&m_FileFilter);
#endif
	GetBottomWindow()->InitializeTreeCtrl();
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnSize(UINT nType, int cx, int cy)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	CRect rect;
	GetWindowRect( &rect );
	if( m_bSplitterCreated )  
	{
		mpo_Splitter->SetColumnInfo(0, rect.Width()/2, 10);
		mpo_Splitter->SetColumnInfo(1, rect.Width()/2, 10);
		mpo_Splitter->RecalcLayout();
	}
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnCtrlPopup(CPoint pt, ePERCheckViewType eViewType/*=0*/)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnCtrlPopup(CPoint pt, ePERCheckViewType eViewType/*=0*/)
{
	EMEN_cl_SubMenu o_Menu(FALSE);
	
	InitPopupMenuAction(&o_Menu);
	
	HTREEITEM hSelectedItem = NULL;

	switch ( eViewType )
	{
		case ePERCheckTopLeftView :
		{
			EPERCheckTop_cl_View* pTopLeftView = GetTopLeftWindow();			
			hSelectedItem = pTopLeftView->GetTree()->GetSelectedItem();

			if ( pTopLeftView->GetTree()->ItemIsFolder(hSelectedItem) )
			{
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_EXPAND_BRANCH);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_REFRESH_BRANCH);
				AddPopupMenuAction(&o_Menu, 0);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_SUBMITTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_EDITTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_SUBMITEDITTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_REVERTTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_SYNCTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_FORCESYNCTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_ADDTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_DELETETOP);
			}
			else
			{
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_SUBMITTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_EDITTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_SUBMITEDITTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_REVERTTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_SYNCTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_FORCESYNCTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_ADDTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_DELETETOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_DIFFTOP);
				AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_HISTORYTOP);
			}
			break;
		}
		case ePERCheckTopRightView :
			return;
		case ePERCheckBottomView :
		{
			EPERCheckBottom_cl_View* pBottomView = GetBottomWindow();
			hSelectedItem = pBottomView->GetTree()->GetSelectedItem();
			if ( pBottomView->GetTree()->ItemIsFolder(hSelectedItem) )
			{
				switch ( pBottomView->GetTree()->GetItemData(hSelectedItem) ) 
				{
					case EPERCheckBottom_cl_View::eRootNotInP4:
						AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_REFRESH_NOTINP4);
						AddPopupMenuAction(&o_Menu, 0);
						//AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_ADDBOTTOM);
						break;
					case EPERCheckBottom_cl_View::eRootNotInBF:
						AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_REFRESH_NOTINBF);
						AddPopupMenuAction(&o_Menu, 0);
						//AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_SYNCBOTTOM);
						break;
					case EPERCheckBottom_cl_View::eRootDeleted:
						AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_REFRESH_DELETED);
						AddPopupMenuAction(&o_Menu, 0);
						//AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_DELETEBOTTOM);
						break;
				}
			}
			else
			{
				HTREEITEM hParent = pBottomView->GetTree()->GetParentItem(hSelectedItem);
				switch ( pBottomView->GetTree()->GetItemData(hParent) ) 
				{
					case EPERCheckBottom_cl_View::eRootNotInP4:
						AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_ADDBOTTOM);
						break;
					case EPERCheckBottom_cl_View::eRootNotInBF:
						AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_SYNCBOTTOM);
						break;
					case EPERCheckBottom_cl_View::eRootDeleted:
						AddPopupMenuAction(&o_Menu, EPERCHECK_ACTION_PERFORCE_DELETEBOTTOM);
						break;
				}
			}
			break;
		}
		default:
			return;
	}
	TrackPopupMenuAction(pt, &o_Menu);
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnClose(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnClose(void)
{
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnCheckFilterSame(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnCheckFilterSame()
{
	BOOL bIsChecked = mpo_DialogBar->IsDlgButtonChecked(IDC_P4CHECK_FILTERSAME);

	if ( bIsChecked )
	{
		GetTopLeftWindow()->AddFilter(ePERCheckViewFilterSame);
		GetTopRightWindow()->AddFilter(ePERCheckViewFilterSame);		
	}
	else
	{
		GetTopLeftWindow()->RemFilter(ePERCheckViewFilterSame);
		GetTopRightWindow()->RemFilter(ePERCheckViewFilterSame);		
	}
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnCheckFilterModified(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnCheckFilterModified()
{
	BOOL bIsChecked = mpo_DialogBar->IsDlgButtonChecked(IDC_P4CHECK_FILTERMODIFIED);

	if ( bIsChecked )
	{
		GetTopLeftWindow()->AddFilter(ePERCheckViewFilterModify);
		GetTopRightWindow()->AddFilter(ePERCheckViewFilterModify);		
	}
	else
	{
		GetTopLeftWindow()->RemFilter(ePERCheckViewFilterModify);
		GetTopRightWindow()->RemFilter(ePERCheckViewFilterModify);		
	}
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnCheckFilterNew(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnCheckFilterNew()
{
	BOOL bIsChecked = mpo_DialogBar->IsDlgButtonChecked(IDC_P4CHECK_FILTERNEW);

	if ( bIsChecked )
	{
		GetTopLeftWindow()->AddFilter(ePERCheckViewFilterNew);
		GetTopRightWindow()->AddFilter(ePERCheckViewFilterNew);		
	}
	else
	{
		GetTopLeftWindow()->RemFilter(ePERCheckViewFilterNew);
		GetTopRightWindow()->RemFilter(ePERCheckViewFilterNew);		
	}
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnCheckFilterRenamed(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnCheckFilterRenamed()
{
	BOOL bIsChecked = mpo_DialogBar->IsDlgButtonChecked(IDC_P4CHECK_FILTERRENAMED);

	if ( bIsChecked )
	{
		GetTopLeftWindow()->AddFilter(ePERCheckViewFilterRename);
		GetTopRightWindow()->AddFilter(ePERCheckViewFilterRename);		
	}
	else
	{
		GetTopLeftWindow()->RemFilter(ePERCheckViewFilterRename);
		GetTopRightWindow()->RemFilter(ePERCheckViewFilterRename);		
	}
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnCheckFilterMoved(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnCheckFilterMoved()
{
	BOOL bIsChecked = mpo_DialogBar->IsDlgButtonChecked(IDC_P4CHECK_FILTERMOVED);

	if ( bIsChecked )
	{
		GetTopLeftWindow()->AddFilter(ePERCheckViewFilterRename);
		GetTopRightWindow()->AddFilter(ePERCheckViewFilterRename);		
	}
	else
	{
		GetTopLeftWindow()->RemFilter(ePERCheckViewFilterRename);
		GetTopRightWindow()->RemFilter(ePERCheckViewFilterRename);		
	}
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnCheckFilterDeleted(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnCheckFilterDeleted()
{
	BOOL bIsChecked = mpo_DialogBar->IsDlgButtonChecked(IDC_P4CHECK_FILTERDELETED);

	if ( bIsChecked )
	{
		GetTopLeftWindow()->AddFilter(ePERCheckViewFilterDelete);
		GetTopRightWindow()->AddFilter(ePERCheckViewFilterDelete);		
	}
	else
	{
		GetTopLeftWindow()->RemFilter(ePERCheckViewFilterDelete);
		GetTopRightWindow()->RemFilter(ePERCheckViewFilterDelete);		
	}
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnCheckFilterNewP4(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnCheckFilterNewP4()
{
	BOOL bIsChecked = mpo_DialogBar->IsDlgButtonChecked(IDC_P4CHECK_FILTERNEWP4);

	if ( bIsChecked )
	{
		GetTopLeftWindow()->AddFilter(ePERCheckViewFilterNewP4);
		GetTopRightWindow()->AddFilter(ePERCheckViewFilterNewP4);		
	}
	else
	{
		GetTopLeftWindow()->RemFilter(ePERCheckViewFilterNewP4);
		GetTopRightWindow()->RemFilter(ePERCheckViewFilterNewP4);		
	}
}
#ifdef JADEFUSION
//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnClickFilterSettings(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnClickFilterSettings()
{
	EDIA_cl_P4FileFilterDialog Dlg(&m_FileFilter);
	
	Dlg.DoModal();
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnCheckFilterSettings(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnCheckFilterSettings()
{
	BOOL bIsChecked = mpo_DialogBar->IsDlgButtonChecked(IDC_P4CHECK_FILTERSETTINGS);
	
	m_FileFilter.Enable(bIsChecked);
}
#endif
//------------------------------------------------------------
//   BOOL EPERCheck_cl_Frame::b_CanActivate(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPERCheck_cl_Frame::b_CanActivate(void)
{
	return TRUE;
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::CloseProject(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::CloseProject(void)
{
}

#endif /* ACTIVE_EDITORS */

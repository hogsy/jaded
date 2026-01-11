/*$T PERtreectrl.cpp GC!1.71 02/17/00 14:52:18 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "PERCFrame.h"
#include "PERtreectrl.h"
#include "PERmsg.h"
#include "EDImainframe.h"
#include "ENGine/Sources/ENGinit.h"
#include "EDIapp.h"
#include "LINKs/LINKstruct.h"
#include "EDIpaths.h"
#include "EDIicons.h"
#include "EDImsg.h"
#include "LINks\LINKtoed.h"

#include "Res/Res.h"

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

BEGIN_MESSAGE_MAP(EPER_cl_TreeCtrl, CTreeCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETFOCUS()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING,OnExpand)
	//ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_CLICK,OnClick)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    OVERWRITE
 ***********************************************************************************************************************
 */

int sgi_PERLockUpdate = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EPER_cl_TreeCtrl::EPER_cl_TreeCtrl(EPER_cl_Frame* _pFrame ) : 
CTreeCtrl(),
mpo_frame(_pFrame),
m_bDragging(FALSE),
m_pImageList(NULL),
m_hLastSelItem(NULL)
{
    mb_LockSel = FALSE;
}

/*$4
 ***********************************************************************************************************************
    MESSAGES:: COMMANDS
 ***********************************************************************************************************************
 */




/*
 =======================================================================================================================
 =======================================================================================================================
 */
//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnBeginDrag(NMHDR *pNotifyStruct2, LRESULT *result)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnBeginDrag(NMHDR *pNotifyStruct2, LRESULT *result)
{
	NM_TREEVIEW *pNotifyStruct;
	pNotifyStruct = ( NM_TREEVIEW *)pNotifyStruct2;

	// So user cant drag root node
	if (GetParentItem(pNotifyStruct->itemNew.hItem) == NULL) return ; 

	// Item user started dragging ...
	m_hitemDrag = pNotifyStruct->itemNew.hItem;
	m_hitemDrop = NULL;
}

void EPER_cl_TreeCtrl::OnExpand(NMHDR *pNotifyStruct2, LRESULT *result)
{
	NM_TREEVIEW *pNotifyStruct;
	pNotifyStruct = ( NM_TREEVIEW *)pNotifyStruct2;

	if (GetParentItem(pNotifyStruct->itemNew.hItem) == NULL && 
		!(pNotifyStruct->itemNew.state & TVIS_EXPANDED )) 
	{
		ULONG ulChangelist = GetItemData(pNotifyStruct->itemNew.hItem);
		AfxGetApp()->DoWaitCursor(1);
		mpo_frame->FillChangelistTree(ulChangelist,TRUE);
		AfxGetApp()->DoWaitCursor(-1);
	}
}


//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	POINT pt = point;
	HTREEITEM hItem = HitTest(point,&nFlags);

	// this is a file, not a changelist
	if ( GetParentItem(hItem) != NULL ) 
	{
		BIG_KEY ulKey = GetItemData(hItem);
		BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat(ulKey);

		if ( ulIndex != BIG_C_InvalidIndex ) 
		{
			LINK_SendMessageToEditors(EDI_MESSAGE_SELFILE, BIG_ParentFile(ulIndex),ulIndex);		
		}
		else 
		{
			M_MF()->MessageBox( "File not found in that bigfile", "[P4 - ERROR]", MB_OK | MB_ICONERROR );
		}
	}
	else 
	{
		CTreeCtrl::OnLButtonDblClk(nFlags,point);
	}
	
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	POINT pt = point;
	HTREEITEM hItem = HitTest(pt,&nFlags);
	
	if( nFlags & TVHT_ONITEMBUTTON )
	{	
		// Check if any child present
		CTreeCtrl::OnLButtonDown(nFlags,point);
		return;
	}

	if( hItem == NULL ) 
	{
		ClearSelection();
		return;
	}

	
	unsigned short shKeyState = GetKeyState(VK_CONTROL);
	shKeyState >>= 15;
	
	if( shKeyState == 1 )
	{
		OnControlKeyPress(hItem);
		return;
	} 
	else
	{
		if( m_vSelItem.size() == 0 )
		{
			CTreeCtrl::OnLButtonDown(nFlags,point);
			SelectItem(hItem,TVIS_SELECTED);
			return;
		}
		
		shKeyState = GetKeyState(VK_SHIFT);
		shKeyState >>= 15;
		if( shKeyState == 1 )
		{
			OnShiftKeyPress(hItem);
			return;
		}
	}
	
	m_hLastSelItem = hItem;

	UINT ui = 0;
	for ( ui ; ui < m_vSelItem.size(); ui ++ ) 
	{
		if ( m_vSelItem[ui] == hItem ) 
		{
			break;
		}
	}

	if ( ui == m_vSelItem.size() ) 
	{
		ClearSelection();
	}

	if( m_vSelItem.size() == 0 )
	{
		CTreeCtrl::OnLButtonDown(nFlags,point);
		SelectItem(m_hLastSelItem,TVIS_SELECTED);
	}
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::SelectItem(HTREEITEM hItem,int iState)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::SelectItem(HTREEITEM hItem,int iState)
{
	
	SetItemState(hItem,iState,TVIS_SELECTED);
	

	if( iState == 0 )
	{
		RemoveFromSelectionList(hItem);
	}
	else
	{	
		m_hLastSelItem = hItem;

		SEL_ITEM_LIST::iterator itr;
		for( itr = m_vSelItem.begin(); itr != m_vSelItem.end(); ++itr )
		{
			if( (*itr) == hItem )
			{
				break;
			}
		}
		if ( itr == m_vSelItem.end() && hItem != NULL ) 
		{
			m_vSelItem.push_back(hItem);
		}
	}
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	POINT pt = point;
	HTREEITEM hItem = HitTest(pt,&nFlags);
	
	UINT ui = 0;
	for ( ui ; ui < m_vSelItem.size(); ui ++ ) 
	{
		if ( m_vSelItem[ui] == hItem ) 
		{
			break;
		}
	}

	if ( ui == m_vSelItem.size() ) 
	{
		ClearSelection();
	}
	
	if( m_vSelItem.size() == 0 )
	{
		m_hLastSelItem = hItem;
		SelectItem(hItem,TVIS_SELECTED);
	}

	ClientToScreen(&point);
	mpo_frame->OnCtrlPopup(point);
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnShiftKeyPress(HTREEITEM hCurItem)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnShiftKeyPress(HTREEITEM hCurItem)
{
	
	HTREEITEM hItemFrom = m_vSelItem[0];
	SetItemState(hCurItem,TVIS_SELECTED,TVIS_SELECTED);
	SelectItems(hItemFrom,hCurItem);
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::SelectItems(HTREEITEM hItemFrom,HTREEITEM hItemTo)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::SelectItems(HTREEITEM hItemFrom,HTREEITEM hItemTo)
{
	RECT FromRect;
	GetItemRect(hItemFrom,&FromRect,FALSE);
	RECT ToRect;
	GetItemRect(hItemTo,&ToRect,FALSE);
	HTREEITEM hTemp;
	if( FromRect.top > ToRect.top )
	{
		hTemp = hItemFrom;
		hItemFrom = hItemTo;
		hItemTo = hTemp;
	}
	ClearSelection();
	hTemp = hItemFrom;
	while(1)
	{
		SelectItem(hTemp,TVIS_SELECTED);
		if( hTemp ==  hItemTo )
		{
			break;
		}
		hTemp = GetNextVisibleItem(hTemp);
	}
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::ClearSelection()
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::ClearSelection()
{
	int nSelItemCount = m_vSelItem.size();
	for( int nIdx = 0; nIdx < nSelItemCount ; ++nIdx )
	{
		SetItemState(m_vSelItem[nIdx],0,TVIS_SELECTED);
	}
	
	m_vSelItem.clear();
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	//if click is not on any item clear all the selection
	ClearSelection();    
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnControlKeyPress(HTREEITEM hCurItem)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnControlKeyPress(HTREEITEM hCurItem)
{
	int iState = (TVIS_SELECTED == GetItemState(hCurItem,TVIS_SELECTED))?0:TVIS_SELECTED;
	SelectItem(hCurItem,iState);
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::RemoveFromSelectionList(HTREEITEM hItem)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::RemoveFromSelectionList(HTREEITEM hItem)
{
	SEL_ITEM_LIST::iterator itr;
	for( itr = m_vSelItem.begin(); itr != m_vSelItem.end(); ++itr )
	{
		if( (*itr) == hItem )
		{
			m_vSelItem.erase(itr);
			break;
		}
	}  
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_bDragging )
	{
		POINT pt = point ;
		ClientToScreen ( &pt ) ;
		CImageList::DragMove ( pt ) ;

		HTREEITEM	hitem;
		UINT		flags;
		if ((hitem = HitTest(point, &flags)) != NULL)
		{
			CImageList::DragShowNolock(FALSE);

			// Tests if dragged item is over another child !
			if ( (GetParentItem(hitem) != NULL) )// && (cursor_no != ::GetCursor())) 
			{
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
				// Dont want last selected target highlighted after mouse
				// has moved off of it, do we now ?
				SelectDropTarget(NULL);
			}
			// Is item we're over a root node and not parent root node ?
			if ( (GetParentItem(hitem) == NULL) && (GetParentItem(m_hitemDrag) != hitem ) ) 
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
				SelectDropTarget(hitem);
			}
			
			HTREEITEM	hVisible = GetNextItem(hitem,TVGN_FIRSTVISIBLE );

			if ( hVisible == hitem ) 
			{
				hVisible = GetNextItem(hitem,TVGN_PREVIOUS );
				if ( !hVisible ) 
					hVisible = GetParentItem(hitem);
				EnsureVisible(hVisible);
			} 
			else
			{
				hVisible = GetNextItem(hitem,TVGN_LASTVISIBLE );
				if ( hVisible = hitem ) 
				{
					hVisible = GetNextItem(hitem,TVGN_NEXT );
					if ( !hVisible ) 
					{
						hVisible = GetParentItem(hitem);
						hVisible = GetNextItem(hVisible,TVGN_NEXT );
					}
					EnsureVisible(hVisible);
				}
			}

			m_hitemDrop = hitem;
			CImageList::DragShowNolock(TRUE);
		}
		else 
		{
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
		}
		return;
	}

	if( DragDetect(point) )
	{
		OnDrag(point);
	}

	CTreeCtrl::OnMouseMove(nFlags, point);

}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnDrag(CPoint point)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnDrag(CPoint point)
{
	if( m_vSelItem.size() ==  0 )
	{
		return;
	}

	// cannot drag a dir
	for ( UINT i = 0; i < m_vSelItem.size() ; i ++ ) 
	{
		if (GetParentItem(m_vSelItem[i]) == NULL)
		{
			return ;
		}
	}

	// begin drag 
	UINT nFlags;
	HTREEITEM hItem = HitTest(point,&nFlags);
	m_bDragging = TRUE;
	m_hitemDrag = m_vSelItem[0];
	m_pImageList = CreateDragImage(m_hitemDrag);

	ASSERT (m_pImageList != NULL);

	if (m_pImageList != NULL)
	{
		CRect rect;
		GetItemRect (hItem, rect, TRUE);
		POINT pt;
		pt.x = rect.left+5;
		pt.y = rect.top+5;

		ClientToScreen ( &pt ) ;
		m_pImageList->BeginDrag (0,CPoint(-5,-5));
		m_pImageList->DragEnter (NULL, pt);    
		SetCapture();
		
	}
}

//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{

	CTreeCtrl::OnLButtonUp(nFlags, point);

	if( !(MK_CONTROL&nFlags) && !(MK_SHIFT&nFlags) && !m_bDragging)
	{
		if( m_vSelItem.size() > 1 )
		{
			ClearSelection();
			SelectItem(m_hLastSelItem,TVIS_SELECTED);
		}    
	}

	if (m_bDragging)
	{
		m_bDragging = FALSE;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();

		if(m_pImageList != NULL) 
		{ 
			delete m_pImageList; 
			m_pImageList = NULL; 
		} 

		// Remove drop target highlighting
		SelectDropTarget(NULL);

		if( m_hLastSelItem == m_hitemDrop )
			return;

		ULONG ulTargetChangelist = GetItemData(m_hitemDrop);

		HTREEITEM	hitem;
		// Make sure pt is over some item
		if ( ((hitem = HitTest(point, &nFlags)) == NULL)  ) return ;
		// Make sure dropped item isnt a child
		if (GetParentItem(hitem) != NULL) return ;

		// If Drag item is an ancestor of Drop item then return
		HTREEITEM htiParent = m_hitemDrop;
		while( (htiParent = GetParentItem( htiParent )) != NULL )
		{
			if( htiParent == m_hitemDrag ) return;
		}

		Expand( m_hitemDrop, TVE_EXPAND ) ;

		htiParent = GetParentItem( m_hLastSelItem );
		m_hLastSelItem = NULL;


		HTREEITEM hChildItem = GetChildItem(htiParent);
		SEL_ITEM_LIST::iterator itr;
		std::vector<ULONG> vChangelist;
		std::vector<BIG_KEY> vFiles;
		for( itr = m_vSelItem.begin(); itr != m_vSelItem.end(); ++itr )
		{
			if (	GetParentItem((*itr)) != NULL && 
					GetItemState( (*itr),TVIS_SELECTED ) == TVIS_SELECTED ) 
			{
				UINT ui = 0;
				for  ( ui = 0 ; ui < vChangelist.size() ;ui ++  )
				{
					if ( vChangelist[ui] == GetItemData(GetParentItem((*itr))) )  
						break;
				}
				if ( ui == vChangelist.size() ) 
				{
					vChangelist.push_back(GetItemData(GetParentItem((*itr))));
				}
				vFiles.push_back(GetItemReferenceData(HDATACTRLITEM(*itr)));
				HTREEITEM htiNew = MoveChildItem( (*itr), m_hitemDrop, TVI_LAST );
				DeleteItem( (*itr) );
			}
		}
		mpo_frame->MoveItems(vFiles,ulTargetChangelist);
		
		vChangelist.push_back(ulTargetChangelist);
		for  ( UINT ui = 0 ; ui < vChangelist.size(); ui ++  )
		{
			mpo_frame->Refresh(vChangelist[ui]);
		}
		m_vSelItem.clear();
	}

}

//------------------------------------------------------------
//   HTREEITEM EPER_cl_TreeCtrl::MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
HTREEITEM EPER_cl_TreeCtrl::MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter)
{

	TV_INSERTSTRUCT tvstruct;
	HTREEITEM hNewItem;
	CString sText;

	// get information of the source item
	tvstruct.item.hItem = hItem;
	tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE |     TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	GetItem(&tvstruct.item);  
	sText = GetItemText( hItem );

	tvstruct.item.cchTextMax = sText.GetLength();
	tvstruct.item.pszText = sText.LockBuffer();

	//insert the item at proper location
	tvstruct.hParent = htiNewParent;
	tvstruct.hInsertAfter = htiAfter;
	tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	hNewItem = InsertItem(&tvstruct);
	sText.ReleaseBuffer();

	//now copy item data and item state.
	SetItemData(hNewItem,GetItemData(hItem));
	SetItemState(hNewItem,GetItemState(hItem,TVIS_STATEIMAGEMASK),TVIS_STATEIMAGEMASK);
	SetItemState(hNewItem,0,TVIS_SELECTED);

	//now delete the old item
	DeleteItem(hItem);

	return hNewItem;
}


//------------------------------------------------------------
//   void EPER_cl_TreeCtrl::OnSetFocus(CWnd *pold)
/// \author    YCharbonneau
/// \date      2005-02-08
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_TreeCtrl::OnSetFocus(CWnd *pold)
{
	CTreeCtrl::OnSetFocus(pold);
	mpo_frame->RefreshMenu();
}





#endif /* ACTIVE_EDITORS */

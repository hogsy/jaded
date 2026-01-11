/*$T TEXframe_msg.cpp GC!1.62 12/27/99 12:50:04 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "Res/Res.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "PFBframe.h"
#include "PFBview.h"
#include "PFBtreeview.h"
#include "EDImsg.h"
#include "EDIicons.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDItors/Sources/BROwser/BROmsg.h"
#include "ENGine/Sources/ENGinit.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EPFB_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_tdst_DragDrop           *pst_DragDrop;
    CPoint                      pt;
    POSITION                    pos;
    int                         i_Msg;
    BOOL						b_Refresh;
    ULONG						ul_Ref, ul_Dir;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_ul_Msg)
    {
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_FATHASCHANGED:
        //if((mul_DirIndex == BIG_C_InvalidIndex) && (mul_FileIndex == BIG_C_InvalidIndex))
        //    return 0;
            
        b_Refresh = FALSE;
        pos = EDI_go_FHCFile.GetStartPosition();
		while(pos)
		{
			EDI_go_FHCFile.GetNextAssoc(pos, ul_Ref, i_Msg);
			if ( (i_Msg == EDI_FHC_Delete ) || (i_Msg == EDI_FHC_Move ) )
			{
				if( BIG_b_IsFileExtension(ul_Ref, EDI_Csz_ExtPrefab) )
					b_Refresh = TRUE;
				if ( ul_Ref == mul_FileIndex )
				{
					mpst_Prefab = NULL;
					mul_FileIndex = BIG_C_InvalidIndex;
					mpo_DataView->Display( NULL );
				}
			}
		}
		
		ul_Dir = BIG_ul_SearchDir( EDI_Csz_Path_Prefab );
		pos = EDI_go_FHCDir.GetStartPosition();
		while(pos)
		{
			EDI_go_FHCDir.GetNextAssoc(pos, ul_Ref, i_Msg);
			if ( (i_Msg == EDI_FHC_Delete ) || (i_Msg == EDI_FHC_Move ) )
			{
				if (mpo_TreeView->FindDir( ul_Ref ) )
					b_Refresh = TRUE;
				if ( ul_Ref == mul_DirIndex )
				{
					mpst_Prefab = NULL;
					mul_FileIndex = BIG_C_InvalidIndex;
					mul_DirIndex = BIG_C_InvalidIndex;
					mpo_DataView->Display( NULL );
				}
			}
		}
		
		if (b_Refresh)
			mpo_TreeView->Refresh();

        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        if( (pst_DragDrop->po_SourceEditor == this) && (pst_DragDrop->i_Type == EDI_DD_User ) )
        {
			return i_CanDragDrop_User( pst_DragDrop );
        }
		/* another thing that file or directory ? */
        if(pst_DragDrop->i_Type != EDI_DD_File)
            return FALSE;
		/* directory ? */
		if ( pst_DragDrop->ul_FatFile == BIG_C_InvalidIndex ) 
			return FALSE;
        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_ENDDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        if( (pst_DragDrop->po_SourceEditor == this) && (pst_DragDrop->i_Type == EDI_DD_User ) )
			EndDragDrop_User( pst_DragDrop );
        break;


    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELDATA: return FALSE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELFILE:
		if ( BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtPrefab ) )
			mpo_TreeView->SelFile( _ul_Param2 );
		else
			return FALSE;		
        break;
    }

    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EPFB_cl_Frame::i_CanDragDrop_User( EDI_tdst_DragDrop *_pst_DD )
{
	CTreeCtrl	*po_Tree;
	POINT		st_Point;
	UINT		uFlags;
	HTREEITEM	hItem;
	int			iImg, iImgSel;
	CListCtrl	*po_ListRef;
			
	/* Drag'n drop a tree item into the tree */
	po_Tree = (CTreeCtrl *) mpo_TreeView->GetDlgItem( IDC_TREE_PREFAB );
	GetCursorPos( &st_Point );
	mpo_TreeView->ScreenToClient( &st_Point );
    if ( mpo_TreeView->ChildWindowFromPoint( st_Point ) == po_Tree )
    {
		GetCursorPos( &st_Point );
		po_Tree->ScreenToClient( &st_Point );
		hItem = po_Tree->HitTest( st_Point, &uFlags);
		if ( (hItem != NULL) && (TVHT_ONITEM & uFlags))
		{
			po_Tree->GetItemImage( hItem, iImg, iImgSel );
			if (iImg == EDI_IMAGE_FOLDER1 )
			{
				po_Tree->SelectDropTarget(hItem);
				return TRUE;
			}
		}
		return FALSE;
	}
	
	/* Drag'n drop a tree item into the list of ref */
	po_ListRef = (CListCtrl *) mpo_DataView->GetDlgItem( IDC_LISTCTRL_REF );
	GetCursorPos( &st_Point );
	mpo_DataView->ScreenToClient( &st_Point );
    if ( (mpo_DataView->ChildWindowFromPoint( st_Point ) == po_ListRef) && mpst_Prefab )
		return TRUE;
    
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::EndDragDrop_User( EDI_tdst_DragDrop *_pst_DD )
{
	CTreeCtrl	*po_Tree;
	POINT		st_Point;
	UINT		uFlags;
	int			iImg, iSelImg;
	ULONG		ul_Dir;
	HTREEITEM	hItem, hNew;
	char		sz_Tgt[ 260 ], sz_Src[ 260 ];
	CListCtrl	*po_ListRef;
	BOOL		bAdd;
	
	/* drop a tree item into the tree */
	po_Tree = (CTreeCtrl *) mpo_TreeView->GetDlgItem( IDC_TREE_PREFAB );
	po_Tree->GetItemImage( (HTREEITEM) _pst_DD->i_Param2, iImg, iSelImg );
	
	po_Tree->SelectDropTarget( NULL );
	GetCursorPos( &st_Point );
	mpo_TreeView->ScreenToClient( &st_Point );
    if ( mpo_TreeView->ChildWindowFromPoint( st_Point ) == po_Tree )
    {
		GetCursorPos( &st_Point );
		po_Tree->ScreenToClient( &st_Point );
		hItem = po_Tree->HitTest( st_Point, &uFlags);
		ul_Dir = po_Tree->GetItemData( hItem );
		BIG_ComputeFullName( ul_Dir, sz_Tgt );
		
		
		po_Tree->GetItemImage( (HTREEITEM) _pst_DD->i_Param2, iImg, iSelImg );
		
		if ( iImg == EDI_IMAGE_GRP )
		{
			BIG_ComputeFullName( BIG_ParentFile( _pst_DD->i_Param3 ), sz_Src );
			BIG_MoveFile( sz_Tgt, sz_Src, BIG_NameFile( _pst_DD->i_Param3) );
			hNew = po_Tree->InsertItem( BIG_NameFile( _pst_DD->i_Param3 ), iImg, iSelImg, hItem );
			po_Tree->SetItemData( hNew, _pst_DD->i_Param3 );
			po_Tree->SortChildren( hItem );
			po_Tree->SelectItem( hNew );
			po_Tree->DeleteItem( (HTREEITEM) _pst_DD->i_Param2 );
		}
		else if (iImg == EDI_IMAGE_FOLDER1 )
		{
			BIG_ComputeFullName( _pst_DD->i_Param3, sz_Src );
			BIG_MoveDir( sz_Tgt, sz_Src );
			mpo_TreeView->Refresh();
			mpo_TreeView->SelDir( _pst_DD->i_Param3 );
		}
		return;
	}
	
	po_ListRef = (CListCtrl *) mpo_DataView->GetDlgItem( IDC_LISTCTRL_REF );
	GetCursorPos( &st_Point );
	mpo_DataView->ScreenToClient( &st_Point );
    if ( ( mpo_DataView->ChildWindowFromPoint( st_Point ) == po_ListRef ) && mpst_Prefab)
    {
		bAdd = FALSE;
		if ( iImg == EDI_IMAGE_GRP )
			bAdd = Prefab_b_AddRefFromFile( mpst_Prefab, _pst_DD->i_Param3 );
		else if ( iImg == EDI_IMAGE_FOLDER1 )
			bAdd = Prefab_b_AddRefFromDir( mpst_Prefab, _pst_DD->i_Param3 );
			
		if (bAdd)
		{
			Prefab_ul_Save( mpst_Prefab );
			mpo_DataView->Display( mpst_Prefab );
		}
		else
		{
		}

	}
	
}


#endif /* ACTIVE_EDITORS */

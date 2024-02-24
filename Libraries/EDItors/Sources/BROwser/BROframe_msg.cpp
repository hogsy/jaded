/*$T BROframe_msg.cpp GC!1.71 01/13/00 17:37:49 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BROframe.h"
#include "BROtreectrl.h"
#include "BROlistctrl.h"
#include "BROgrpctrl.h"
#include "BROstrings.h"
#include "EDImsg.h"
#include "BROmsg.h"
#include "EDImainframe.h"
#include "ENGine/Sources/ENGinit.h"
#include "EDIapp.h"

BOOL EBRO_gb_CanRefresh = TRUE;

/*
 =======================================================================================================================
    Aim:    Determins if a given point is in dir or file pane (or none).

    Out:    Returns item if needed.
 =======================================================================================================================
 */
int EBRO_cl_Frame::i_NumPaneForPoint(CPoint _po_Pt, void **_p_Item)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int         i_YCur, i_YCur1, i_Temp;
    CRect       o_Rect;
    HTREEITEM   h_TreeItem;
    int         i_ListItem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    mpo_InsideSplitter->GetWindowRect(o_Rect);
    if(!o_Rect.PtInRect(_po_Pt)) return 0;

    mpo_InsideSplitter->GetRowInfo(0, i_YCur, i_Temp);

    /* Group Ctrl */
    if(_po_Pt.y > o_Rect.top + i_YCur)
    {
        mpo_GrpCtrl->ScreenToClient(&_po_Pt);
        i_ListItem = mpo_GrpCtrl->HitTest(_po_Pt);
        if(_p_Item) *((int *) _p_Item) = i_ListItem;

        return 3;
    }
    else
    {
        /* Tree ctrl */
        mpo_InsideLstSplitter->GetRowInfo(0, i_YCur1, i_Temp);
        if(_po_Pt.y < o_Rect.top + i_YCur1)
        {
            mpo_TreeCtrl->ScreenToClient(&_po_Pt);
            h_TreeItem = mpo_TreeCtrl->HitTest(_po_Pt);
            if(h_TreeItem == NULL) return 0;

            if(_p_Item) *((HTREEITEM *) _p_Item) = h_TreeItem;

            return 1;
        }

        /* File ctrl */
        else
        {
            mpo_ListCtrl->ScreenToClient(&_po_Pt);
            i_ListItem = mpo_ListCtrl->HitTest(_po_Pt);
            if(_p_Item) *((int *) _p_Item) = i_ListItem;

            return 2;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::CopyMove(EDI_tdst_DragDrop *pst_DragDrop)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG        l_Return;
    int         i_Item, i_Pane, i_Item1;
    BIG_INDEX   ul_Index;
    char        asz_SrcPath[BIG_C_MaxLenPath];
    char        asz_DestPath[BIG_C_MaxLenPath];
    CPoint      o_Pt;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    i_Pane = i_NumPaneForPoint(pst_DragDrop->o_Pt, (void **) &l_Return);
    if(!i_Pane) return;

    /* No d&d in list view for a directory */
    if((i_Pane != 1) && (pst_DragDrop->ul_FatFile == BIG_C_InvalidIndex))
        return;

    /* Get selected dest dir */
    if(i_Pane == 1)
        ul_Index = mpo_TreeCtrl->GetItemData((HTREEITEM) l_Return);
    else
        ul_Index = mpo_TreeCtrl->GetItemData(mpo_TreeCtrl->GetSelectedItem());

    /* Get src dir */
    BIG_ComputeFullName(ul_Index, asz_DestPath);
    BIG_ComputeFullName(pst_DragDrop->ul_FatDir, asz_SrcPath);

    /* Move/Copy a file */
    if(pst_DragDrop->ul_FatFile != BIG_C_InvalidIndex)
    {
        if((i_Pane == 1) || (i_Pane == 2))  /* In treectrl | listctrl */
        {
            if(i_Pane == 2)
            {
                o_Pt = pst_DragDrop->o_Pt;
                mpo_ListCtrl->ScreenToClient(&o_Pt);
                i_Item1 = mpo_ListCtrl->HitTest(o_Pt);
                if((i_Item1 != -1) && (BIG_b_IsGrpFile(mpo_ListCtrl->GetItemData(i_Item1))))
                {
                    BIG_AddRefInGroup
                    (
                        mpo_ListCtrl->GetItemData(i_Item1),
                        BIG_NameFile(pst_DragDrop->ul_FatFile),
                        BIG_FileKey(pst_DragDrop->ul_FatFile)
                    );
                    goto nocpy;
                }
            }

            if(pst_DragDrop->b_Copy)
                BIG_CopyFile(asz_DestPath, asz_SrcPath, BIG_NameFile(pst_DragDrop->ul_FatFile));
            else
                BIG_MoveFile(asz_DestPath, asz_SrcPath, BIG_NameFile(pst_DragDrop->ul_FatFile));
nocpy: ;
        }
        else    /* In grpctrl */
        {
            i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
            if(i_Item != -1)
            {
                BIG_AddRefInGroup
                (
                    mpo_ListCtrl->GetItemData(i_Item),
                    BIG_NameFile(pst_DragDrop->ul_FatFile),
                    BIG_FileKey(pst_DragDrop->ul_FatFile)
                );
            }
        }
    }

    /* Move/Copy a dir */
    else
    {
        if(pst_DragDrop->b_Copy)
            BIG_CopyDir(asz_DestPath, asz_SrcPath);
        else
            BIG_MoveDir(asz_DestPath, asz_SrcPath);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EBRO_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LV_FINDINFO         st_Find;
    int                 i_Item, i_Pane;
    HTREEITEM           h_Item, h_Item1;
    EDI_tdst_DragDrop   *pst_DragDrop;
    LONG                l_Return;
    CRect               o_Rect;
    CPoint              o_Pt;
    EBRO_cl_Frame       *pst_Src;
    BOOL                bRefresh;
	static DWORD		dw_TimeScroll = 0;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_ul_Msg)
    {
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_FATHASCHANGED:
        if(EBRO_M_EngineMode()) return FALSE;
		if(EBRO_gb_CanRefresh) RefreshAll(FALSE);
        Invalidate();
        break;

    case EDI_MESSAGE_DATAHASCHANGED:
        if(!EBRO_M_EngineMode()) return FALSE;
        RefreshAll(FALSE);
        Invalidate();
        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EDI_MESSAGE_REFRESH:
		RefreshMenu();
		break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANDRAGDROP:

        /* No d&d if engine mode */
        if(EBRO_M_EngineMode()) return FALSE;

        /* Only receive dirs and files */
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        if(pst_DragDrop->i_Type == EDI_DD_Long) return FALSE;
        if((pst_DragDrop->ul_FatDir == BIG_C_InvalidIndex) && (pst_DragDrop->ul_FatFile == BIG_C_InvalidIndex))
            return FALSE;

        if(pst_DragDrop->i_Type != EDI_DD_File) return FALSE;
//        if(pst_DragDrop->po_SourceEditor->mst_Def.i_Type != mst_Def.i_Type)
//            return FALSE;

        i_Pane = i_NumPaneForPoint(pst_DragDrop->o_Pt, (void **) &l_Return);
        if(!i_Pane) return FALSE;

        /* No d&d in list view for a directory */
        if((i_Pane != 1) && (pst_DragDrop->ul_FatFile == BIG_C_InvalidIndex))
        {
            mpo_TreeCtrl->SelectDropTarget(NULL);
            return FALSE;
        }

        /* No d&d if grpctrl if no selected file */
        if(i_Pane == 3)
        {
            i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
            if(i_Item == -1) return FALSE;
            if(!BIG_b_IsGrpFile(mpo_ListCtrl->GetItemData(i_Item)))
                return FALSE;
        }

        /* Auto scroll, and select dir as a drag&drop dest */
        if(i_Pane == 1)
        {
            mpo_TreeCtrl->GetWindowRect(&o_Rect);
			if ( L_timeGetTime() - dw_TimeScroll > 300 )
			{
				dw_TimeScroll = L_timeGetTime();
				h_Item = mpo_TreeCtrl->GetFirstVisibleItem();
				if(pst_DragDrop->o_Pt.y < o_Rect.top + 20)
				{
					h_Item1 = mpo_TreeCtrl->GetPrevSiblingItem(h_Item);
					if(h_Item1 == NULL) h_Item1 = mpo_TreeCtrl->GetParentItem(h_Item);
					if(h_Item1) mpo_TreeCtrl->SelectSetFirstVisible(h_Item1);
				}
				else if(pst_DragDrop->o_Pt.y > o_Rect.bottom - 20)
				{
					h_Item1 = mpo_TreeCtrl->GetNextSiblingItem(h_Item);
					if(h_Item1 == NULL) h_Item1 = mpo_TreeCtrl->GetChildItem(h_Item);
					if(h_Item1) mpo_TreeCtrl->SelectSetFirstVisible(h_Item1);
				}

				mpo_TreeCtrl->SelectDropTarget((HTREEITEM) l_Return);
			}
        }

        /* Auto scroll, and select file as a drag&drop dest */
        if(i_Pane == 2)
        {
            /* dd of a file in file list */
            if(pst_DragDrop->ul_FatFile != BIG_C_InvalidIndex)
                return TRUE;

			if ( L_timeGetTime() - dw_TimeScroll > 300 )
			{
				dw_TimeScroll = L_timeGetTime();
				mpo_ListCtrl->GetWindowRect(&o_Rect);
				i_Item = mpo_ListCtrl->GetTopIndex();
				if(pst_DragDrop->o_Pt.y < o_Rect.top + 20)
				{
					i_Item--;
					if(i_Item) mpo_ListCtrl->EnsureVisible(i_Item - 1, FALSE);
				}
				else if(pst_DragDrop->o_Pt.y > o_Rect.bottom - 20)
				{
					i_Item += mpo_ListCtrl->GetCountPerPage();
					if(i_Item < mpo_ListCtrl->GetItemCount())
						mpo_ListCtrl->EnsureVisible(i_Item, FALSE);
				}

				i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_FOCUSED);
				if(i_Item != -1) mpo_ListCtrl->SetItemState(i_Item, LVIS_FOCUSED, 0);
				mpo_ListCtrl->SetItemState(l_Return, LVIS_FOCUSED, LVIS_FOCUSED);
			}
        }

        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_ENDDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;

        /* Test for d&d of more than one file/dir */
        if
        (
            (pst_DragDrop->po_SourceEditor) &&
            (pst_DragDrop->po_SourceEditor->mst_Def.i_Type == mst_Def.i_Type) &&
            (pst_DragDrop->ul_FatFile != BIG_C_InvalidIndex)
        )
        {
            pst_Src = (EBRO_cl_Frame *) pst_DragDrop->po_SourceEditor;
			if(pst_DragDrop->i_Param3 == 2)
				i_Item = pst_Src->mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
			else
				i_Item = pst_Src->mpo_GrpCtrl->GetNextItem(-1, LVNI_SELECTED);
            while(i_Item != -1)
            {
				if(pst_DragDrop->i_Param3 == 2)
					pst_DragDrop->ul_FatFile = pst_Src->mpo_ListCtrl->GetItemData(i_Item);
				else
					pst_DragDrop->ul_FatFile = BIG_ul_SearchKeyToFat(pst_Src->mpo_GrpCtrl->GetItemData(i_Item));
                CopyMove(pst_DragDrop);
				if(pst_DragDrop->i_Param3 == 2)
					i_Item = pst_Src->mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED);
				else
					i_Item = pst_Src->mpo_GrpCtrl->GetNextItem(i_Item, LVNI_SELECTED);
            }
        }
        else
            CopyMove(pst_DragDrop);

        mpo_TreeCtrl->SelectDropTarget(NULL);
        i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_FOCUSED);
        if(i_Item != -1) mpo_ListCtrl->SetItemState(i_Item, LVIS_FOCUSED, 0);
        M_MF()->FatHasChanged();
		RefreshAll(TRUE);
        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANCELDRAGDROP:
    case EDI_MESSAGE_LOOSEFOCUSDRAGDROP:
        mpo_TreeCtrl->SelectDropTarget(NULL);
        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELDIR:

        /* No selection in engine mode */
        if(EBRO_M_EngineMode()) return TRUE;

        if((BIG_INDEX) _ul_Param1 != BIG_C_InvalidIndex)
            SelectDir((BIG_INDEX) _ul_Param1);

        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELDATA:

        /* No selection in engine mode */
        if(!EBRO_M_EngineMode()) return TRUE;

        _ul_Param1 = LOA_ul_SearchIndexWithAddress(_ul_Param2);
        ERR_X_Assert(_ul_Param2 != BIG_C_InvalidIndex);
        _ul_Param1 = BIG_ParentFile(_ul_Param1);
        goto lcom;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELFILE:

        /* No selection in engine mode */
        if(EBRO_M_EngineMode()) return TRUE;

lcom:

        /* Select dir */
        bRefresh = TRUE;
        h_Item = mpo_TreeCtrl->GetSelectedItem();
        if(h_Item && mpo_TreeCtrl->GetItemData(h_Item) == _ul_Param1)
            bRefresh = FALSE;
        else
        {
            M_MF()->LockDisplay(this);
            if((BIG_INDEX) _ul_Param1 != BIG_C_InvalidIndex)
                SelectDir((BIG_INDEX) _ul_Param1);
        }

        /* Deselect current file */
        i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
        if(i_Item != -1) mpo_ListCtrl->SetItemState(i_Item, 0, LVIS_FOCUSED | LVIS_SELECTED);

        /* Select new file */
        if((BIG_INDEX) _ul_Param2 != BIG_C_InvalidIndex)
        {
            mpo_ListCtrl->SetRedraw(FALSE);
            st_Find.flags = LVFI_PARAM;
            st_Find.lParam = (BIG_INDEX) _ul_Param2;
            i_Item = mpo_ListCtrl->FindItem(&st_Find);
            if(i_Item != -1)
            {
                mpo_ListCtrl->SetItemState(i_Item, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

                mpo_ListCtrl->EnsureVisible(i_Item, FALSE);
            }

            mpo_ListCtrl->SetRedraw(TRUE);
        }

        if(bRefresh) M_MF()->UnlockDisplay(this);
        break;

    case EBRO_MESSAGE_LINKTOPATH:
        OnLinkToExt((char*)_ul_Param1);
        break;

    case EBRO_MESSAGE_IMPORTFROMPATH:
        OnImpFromExt();
        break;
    }

    return TRUE;
}

#endif /* ACTIVE_EDITORS */

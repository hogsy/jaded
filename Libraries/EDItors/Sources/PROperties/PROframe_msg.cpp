 /*$T PROframe_msg.cpp GC!1.52 12/10/99 11:48:33 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "PROframe.h"
#include "LINKs/LINKstruct.h"
#include "EDIpaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "LINKs/LINKstruct_reg.h"
#include "Res/Res.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
EVAV_cl_ViewItem *EPRO_cl_Frame::pst_GetGroupByMouse(CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Item;
    BOOL                b;
    int                 i_Sel;
    CRect               o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(mpo_VarsView->mpo_ListBox->GetCount() == 0) return NULL;

    mpo_VarsView->mpo_ListBox->ScreenToClient(&pt);

    i_Sel = mpo_VarsView->mpo_ListBox->ItemFromPoint(pt, b);
    if((short) i_Sel == -1) return NULL;
    if(b) return NULL;

    if(i_Sel == mpo_VarsView->mpo_ListBox->GetCount() - 1)
    {
        mpo_VarsView->mpo_ListBox->GetItemRect(i_Sel, &o_Rect);
        if(!o_Rect.PtInRect(pt)) return FALSE;
    }

    while(i_Sel >= 0)
    {
        po_Item = (EVAV_cl_ViewItem *) mpo_VarsView->mpo_ListBox->GetItemData(i_Sel);
        if(po_Item->me_Type == EVAV_EVVIT_Separator) return po_Item;
        i_Sel--;
    }

    return NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
POSITION EPRO_cl_Frame::x_AddItemByCopy
(
    EVAV_cl_ViewItem    *_po_ToAdd,
    POSITION            _posref,
    int                 _i_Head,
    BOOL                _b_First
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Create the new item */
    po_Item = new EVAV_cl_ViewItem;
    po_Item->CopyFrom(_po_ToAdd);

    /* For the first item, force possible deletion */
    if(_b_First)
        po_Item->mul_Flags |= EVAV_CanDelete;

    /* Insert at a specified position */
    if(_posref)
        return mo_ListItems.InsertAfter(_posref, po_Item);

    /* Insert at head or tail */
    if(_i_Head)
        return mo_ListItems.AddHead(po_Item);
    else
        return mo_ListItems.AddTail(po_Item);
}

/*
 ===================================================================================================
    Aim:    Destroy all the children of a node
 ===================================================================================================
 */
void ExpandedItemNode::DestroyAllChildren()
{
	for (int i=0; i<mi_ChildrenNb; i++)
	{
		mast_Children[i].DestroyAllChildren();
	}
	mi_ChildrenNb = 0;
	MEM_Free(mast_Children);
	mast_Children = NULL;
}

/*
 ===================================================================================================
    Aim:		Destroy one child of a node and all the children of this child
	Required :	A child named _sz_Name MUST EXIST
 ===================================================================================================
 */
void ExpandedItemNode::DestroyChild(char *_sz_Name)
{
	// Move the children after the deleted child if necessary
	for (int i = 0; i < mi_ChildrenNb; i++)
	{
		if (!L_strcmp(mast_Children[i].msz_Name, _sz_Name))
		{
			mast_Children[i].DestroyAllChildren();
			if (i != mi_ChildrenNb - 1)
				L_memmove(mast_Children + i, mast_Children + i + 1, (mi_ChildrenNb - i - 1) * sizeof(ExpandedItemNode));
			break;
		}
	}
	// Realloc or destroy array of children
	mi_ChildrenNb--;
	if (mi_ChildrenNb)
		mast_Children = (ExpandedItemNode *)MEM_p_Realloc(mast_Children, mi_ChildrenNb * sizeof(ExpandedItemNode));
	else
	{
		MEM_Free(mast_Children);
		mast_Children = NULL;
	}
}

/*
 ===================================================================================================
    Aim:    Add a child and return a pointer to this child
 ===================================================================================================
 */
ExpandedItemNode *ExpandedItemNode::AddChild(char *_sz_Name, int _i_Indent)
{
	mi_ChildrenNb++;
	if (mi_ChildrenNb > 1)
		mast_Children = (ExpandedItemNode *)MEM_p_Realloc(mast_Children, mi_ChildrenNb * sizeof(ExpandedItemNode));
	else
		mast_Children = (ExpandedItemNode *) MEM_p_Alloc(sizeof(ExpandedItemNode));
	mast_Children[mi_ChildrenNb - 1].mi_ChildrenNb = 0;
	mast_Children[mi_ChildrenNb - 1].mast_Children = NULL;
	mast_Children[mi_ChildrenNb - 1].mi_Indent = _i_Indent;
	strcpy (mast_Children[mi_ChildrenNb - 1].msz_Name, _sz_Name);
	return mast_Children + mi_ChildrenNb - 1;
}

///************
// *  DEBUG
// ************/
//static inline void PrintTree(ExpandedItemNode *tree, int depth=0)
//{
//	FILE *f = fopen("DebugProp.txt", "a");
//	for (int i=0; i<depth; i++)
//		fprintf(f, "\t");
//	fprintf(f, "%s\n", tree->msz_Name);
//	fclose(f);
//	for (int i=0; i<tree->mi_ChildrenNb; i++)
//		PrintTree(tree->mast_Children + i, depth + 1);
//}

/*
 ===================================================================================================
    Aim:    find a child. Return a pointer to this child or NULL
 ===================================================================================================
 */
ExpandedItemNode *ExpandedItemNode::pst_FindChildNamed(char *_sz_Name)
{
	for (int i=0; i<mi_ChildrenNb; i++)
	{
		if (!L_strcmp(mast_Children[i].msz_Name, _sz_Name))
			return mast_Children + i;
	}
	return NULL;
}

/*
 ===================================================================================================
    Aim:    Search the father of a node in the tree "this" is the root node of.
			Return NULL if not found
 ===================================================================================================
 */
ExpandedItemNode *ExpandedItemNode::pst_FindFatherOf(ExpandedItemNode *_pst_Child)
{
	ExpandedItemNode *res;

	if (!_pst_Child) return NULL;

	if ((_pst_Child >= mast_Children) && (_pst_Child < mast_Children + mi_ChildrenNb))
		return this;
	for (int i=0; i<mi_ChildrenNb; i++)
	{
		res = mast_Children[i].pst_FindFatherOf(_pst_Child);
		if (res) return res;
	}
	return NULL;
}

/*
 ===================================================================================================
    Aim:    Expand tree according to old tree and item list
 ===================================================================================================
 */
void EPRO_cl_Frame::SynchronizeExpandTree()
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem    *po_Item;
	POSITION			pos;
	int					i_PrevIndent, i_LastDeletedItemIndent;
	ExpandedItemNode	*pst_CurrentNode, *pst_CurrentFatherNode, *pst_LastExpandedChildNode;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_LastDeletedItemIndent = 1000; // Indent can't be greater than 1000
	i_PrevIndent = -1;
	pst_CurrentNode = &mst_ExpandedItemTree;
	pst_CurrentFatherNode = NULL;
	pst_LastExpandedChildNode = pst_CurrentNode;
	pos = mo_ListItems.GetHeadPosition();
	// Go through all EXISTING item of the current selection to update their state.
	// Do not change state of non existing items which are stored in the tree.
	while(pos)
	{
       	po_Item = mo_ListItems.GetNext(pos);

		// DEBUG
		if (!strcmp(po_Item->masz_Name, "Modifiers") || !strcmp(po_Item->masz_Name, "Extended"))
			int i = 0;

		// Check that we are not considering items which are child of a deleted (un-expanded) item
		if (i_LastDeletedItemIndent < 1000)
		{
			if (po_Item->mi_Indent > i_LastDeletedItemIndent)
				// This is still a child of the last deleted item
				continue;
			else
				// OK, all children of last deleted item were seen
				i_LastDeletedItemIndent = 1000;
		}

		// Update depth and current father node
		if (po_Item->mi_Indent > i_PrevIndent)
		{
			pst_CurrentFatherNode = pst_LastExpandedChildNode;
		}
		else if (po_Item->mi_Indent < i_PrevIndent)
		{
			// We can go back through several depth levels
			for(int i=i_PrevIndent; i>po_Item->mi_Indent; i--)
			{
				pst_LastExpandedChildNode = pst_CurrentFatherNode;
				pst_CurrentFatherNode = mst_ExpandedItemTree.pst_FindFatherOf(pst_CurrentFatherNode);
			}
		}
		// Double check
		if (pst_CurrentFatherNode->mi_Indent > po_Item->mi_Indent - 1)
		{
			while (pst_CurrentFatherNode && (pst_CurrentFatherNode->mi_Indent > po_Item->mi_Indent - 1))
			{
				pst_LastExpandedChildNode = pst_CurrentFatherNode;
				pst_CurrentFatherNode = mst_ExpandedItemTree.pst_FindFatherOf(pst_CurrentFatherNode);
			}
		}
		else if (pst_CurrentFatherNode->mi_Indent < po_Item->mi_Indent - 1)
		{
			// Problem => don't try to remember that
			i_LastDeletedItemIndent = pst_CurrentFatherNode->mi_Indent + 1;
			i_PrevIndent = i_LastDeletedItemIndent;
			continue;
		}
		i_PrevIndent = po_Item->mi_Indent;

		// Was po_Item expanded last time ?
		if (pst_CurrentFatherNode)
			pst_CurrentNode = pst_CurrentFatherNode->pst_FindChildNamed(po_Item->masz_Name);
		else
			return;

		if (pst_CurrentNode)
		{
			// po_Item was expanded last time.
			// NB : Special case for indent 0.
			// NB 2 : Special case too for items that ca't be extended because they have
			//        no data. We only remember the state of the last non NULL item.
			if (po_Item->mb_Expand || !i_PrevIndent || !po_Item->mp_Data)
				pst_LastExpandedChildNode = pst_CurrentNode;
			else
			{
				// Remember it as unexpanded now.
				i_LastDeletedItemIndent = i_PrevIndent;
				pst_CurrentFatherNode->DestroyChild(po_Item->masz_Name);
			}
		}
		else
		{
			// po_Item wasn't expanded last time.
			if (po_Item->mb_Expand)
				// Remember it as expanded now
				pst_LastExpandedChildNode = pst_CurrentFatherNode->AddChild(po_Item->masz_Name, po_Item->mi_Indent);
			else
				// Remember not to consider children of this item
				i_LastDeletedItemIndent = i_PrevIndent;
		}
	}
}

/*
 ===================================================================================================
    Aim:    Expand item list according to tree (similar to SynchronizedExpandTree)
 ===================================================================================================
 */
void EPRO_cl_Frame::SynchronizeItemList()
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem    *po_Item;
	POSITION			pos;
	int					i_PrevIndent;
	ExpandedItemNode	*pst_CurrentNode, *pst_CurrentFatherNode, *pst_LastExpandedChildNode;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_PrevIndent = -1;
	pst_CurrentNode = &mst_ExpandedItemTree;
	pst_CurrentFatherNode = NULL;
	pst_LastExpandedChildNode = pst_CurrentNode;
	pos = mo_ListItems.GetHeadPosition();
	while(pos)
	{
		po_Item = mo_ListItems.GetNext(pos);

		if (po_Item->mi_Indent > i_PrevIndent)
		{
			pst_CurrentFatherNode = pst_LastExpandedChildNode;
		}
		else if (po_Item->mi_Indent < i_PrevIndent)
		{
			for(int i=i_PrevIndent; i>po_Item->mi_Indent; i--)
			{
				pst_LastExpandedChildNode = pst_CurrentFatherNode;
				pst_CurrentFatherNode = mst_ExpandedItemTree.pst_FindFatherOf(pst_CurrentFatherNode);
			}
		}
		i_PrevIndent = po_Item->mi_Indent;

		if (pst_CurrentFatherNode)
			pst_CurrentNode = pst_CurrentFatherNode->pst_FindChildNamed(po_Item->masz_Name);
		else
			return;

		if (i_PrevIndent==0 || pst_CurrentNode)
		{
			pst_LastExpandedChildNode = pst_CurrentNode;
			if(!po_Item->mb_ExpandOnce)
				mpo_VarsView->mpo_ListBox->OnExpand(po_Item->mi_ID + WM_USER);
			pos = mo_ListItems.Find(po_Item);
			mo_ListItems.GetNext(pos);
		}
	}
		
}

/*
 ===================================================================================================
    Aim:    Add a pointer in current list. This pointer must have been registered.
 ===================================================================================================
 */
EVAV_cl_ViewItem *EPRO_cl_Frame::ReplaceItemByPointer
(
    LINK_tdst_Pointer   *_pst_Pointer1,
    LINK_tdst_Pointer   *_pst_Pointer2,
    POSITION            posgrp,
    BOOL                _b_CanAdd,
 
	POSITION            *_px_RetPos
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Item, *po_First;
    POSITION            ppos, pos, posdel;
    BOOL                bOk;
    void                *_p_Pointer1, *_p_Pointer2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _p_Pointer1 = _pst_Pointer1->pv_Data;
    _p_Pointer2 = _pst_Pointer2->pv_Data;
    if(_px_RetPos) *_px_RetPos = NULL;

    /* Search the pointer in the list */
    bOk = FALSE;
    if(!posgrp)
        pos = mo_ListItems.GetHeadPosition();
    else
        pos = posgrp;
    while(pos)
    {
        ppos = pos;
        po_Item = mo_ListItems.GetNext(pos);
        if(po_Item->me_Type == EVAV_EVVIT_Separator) posgrp = ppos;
        if(po_Item->mp_Data == _p_Pointer1)
        {
            po_First = po_Item;
            bOk = TRUE;
            break;
        }
    }

    if(bOk)
    {
        /* Update expanded item tree */
		SynchronizeExpandTree();

        /* Delete previous pointer */
        posdel = DelItemByPointer(_pst_Pointer1->pv_Data, posgrp);
        po_Item = AddItemByPointer(_pst_Pointer2, posgrp, FALSE, TRUE, posdel);
        if(_px_RetPos)
        {
            *_px_RetPos = mo_ListItems.Find(po_Item);
            mo_ListItems.GetNext(*_px_RetPos);
        }

        mpo_VarsView->SetItemList(&mo_ListItems);

        /* Expand as previous */
		SynchronizeItemList();

        return po_Item;
    }

    /* The previous item was not here, simply add new one */
    po_Item = NULL;
    if(_b_CanAdd)
    {
        po_Item = AddItemByPointer(_pst_Pointer2, posgrp, FALSE, TRUE);
		mpo_VarsView->SetItemList(&mo_ListItems);
		mpo_VarsView->mpo_ListBox->OnExpand(po_Item->mi_ID + WM_USER);
    }

    return po_Item;
}

/*
 ===================================================================================================
    Aim:    Add a pointer in current list. This pointer must have been registered.
 ===================================================================================================
 */
EVAV_cl_ViewItem *EPRO_cl_Frame::AddItemByPointer
(
    LINK_tdst_Pointer   *_pst_Pointer,
    POSITION            posgrp,
    BOOL                _b_Forced,
    BOOL                _b_DD,
    POSITION            posbefore
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Item;
    POSITION            ppos, pos;
    BOOL                bOk;
    char                *psz_Temp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(posbefore == NULL)
    {
        /* Search if there's the same group */
        ppos = posgrp;
        bOk = TRUE;
        if(!ppos && !_b_Forced)
        {
            pos = mo_ListItems.GetHeadPosition();
            bOk = FALSE;
            while(pos)
            {
                ppos = pos;
                po_Item = mo_ListItems.GetNext(pos);
                if(po_Item->me_Type == EVAV_EVVIT_Separator)
                {
                    if(!L_strcmpi(po_Item->masz_Name, _pst_Pointer->asz_Path))
                    {
                        bOk = TRUE;
                        break;
                    }

                    break;
                }
            }
        }

        /* Add the group if not present */
        if(!bOk || _b_Forced)
        {
			psz_Temp = _pst_Pointer->asz_Path;
            po_Item = mpo_VarsView->AddItem(psz_Temp, EVAV_EVVIT_Separator, NULL, EVAV_CanDelete);
            if(_b_DD)
                po_Item->mb_Locked = TRUE;
            ppos = mo_ListItems.GetTailPosition();
        }
    }
    else
    {
        ppos = posbefore;
    }

    /* Add the item */
    po_Item = mpo_VarsView->AddItem
        (
            LINK_gast_StructTypes[_pst_Pointer->i_Type].psz_Name,
            EVAV_EVVIT_Pointer,
            _pst_Pointer->pv_Data,
            EVAV_CanDelete,
            _pst_Pointer->i_Type,
            0,
            0,
            ppos
        );
    return po_Item;
}

/*
 ===================================================================================================
    Aim:    Delete a pointer in current list. This pointer must have been registered.

    Note:   Pointer maybe invalid for engine, so do not use content !!!
 ===================================================================================================
 */
POSITION EPRO_cl_Frame::DelItemByPointer
(
    void        *_pst_Pointer,
    POSITION    posgrp,
    POSITION    *_px_RetPos,
    BOOL        _b_DelGrp
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Item, *po_Item1;
    POSITION            ppos, pos;
    BOOL                bOk;
    POSITION            posbefore;
    int                 iFirstIndent;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    posbefore = NULL;
    if(_px_RetPos) *_px_RetPos = NULL;

    /* Search the pointer in the list */
    bOk = FALSE;
    if(!posgrp)
        pos = mo_ListItems.GetHeadPosition();
    else
        pos = posgrp;
    while(pos)
    {
        ppos = pos;
        po_Item = mo_ListItems.GetNext(pos);
        if(po_Item->mp_Data == _pst_Pointer)
        {
            bOk = TRUE;
            break;
        }
    }

    if(bOk)
    {
        /* Get position before deletion */
        posbefore = ppos;
        mo_ListItems.GetPrev(posbefore);

        /* Remove item and content */
        iFirstIndent = po_Item->mi_Indent;
        mo_ListItems.RemoveAt(ppos);
        delete po_Item;
		po_Item = NULL;
        while(pos)
        {
            ppos = pos;
            po_Item = mo_ListItems.GetNext(pos);
            if(po_Item->mi_Indent <= iFirstIndent) break;
            delete po_Item;
			po_Item = NULL;
            mo_ListItems.RemoveAt(ppos);
        }

        /*
         * If item before is a separator, and item after is a separator (or none), delete the
         * separator
         */
        if(posbefore && _b_DelGrp)
        {
            po_Item1 = mo_ListItems.GetAt(posbefore);
            if(po_Item1->me_Type == EVAV_EVVIT_Separator)
            {
                if((po_Item && po_Item->me_Type == EVAV_EVVIT_Separator) || (!pos))
                {
                    ppos = posbefore;
                    mo_ListItems.GetPrev(posbefore);
                    delete po_Item1;
                    mo_ListItems.RemoveAt(ppos);
                }
            }
        }

        if(_px_RetPos) *_px_RetPos = pos;
    }

    return posbefore;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
POSITION EPRO_cl_Frame::DragDropPasteContent
(
    POSITION            pos,
    EPRO_cl_Frame       *_po_Src,
    EVAV_cl_ViewItem    *_po_Group,
    EVAV_cl_ViewItem    *_po_Item
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            posgrp, pos1;
    EVAV_cl_ViewItem    *po_Next, *po_Next1;
    int                 i_TstIndent, i_StopIndent;
    BOOL                b_Ok, b_JustAdded;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Get position of the group in current list */
    posgrp = mo_ListItems.Find(_po_Group);
    ERR_X_Assert(posgrp);
    mo_ListItems.GetNext(posgrp);

    /* Add pointers */
    i_StopIndent = 1;
    while(1)
    {
        i_TstIndent = _po_Item->mi_Indent - 1;
        pos1 = pos;
        _po_Src->mo_ListItems.GetPrev(pos1);
        b_JustAdded = FALSE;
        if(pos1)
        {
            /* Drag & drop of the pointer */
            if
            (
                (
                    (_po_Item->me_Type == EVAV_EVVIT_Pointer) ||
                    (_po_Item->me_Type == EVAV_EVVIT_SubStruct)
                ) &&
                (_po_Item->mi_Indent == i_StopIndent)
            )
            {
                po_Next = _po_Item;
                goto l_Zap;
            }

            /* Search the parent pointer with a specific indent (i_StopIndent) */
            while(1)
            {
                po_Next = _po_Src->mo_ListItems.GetAt(pos1);
                if
                (
                    (po_Next->mi_Indent == i_TstIndent) &&
                    (
                        (po_Next->me_Type == EVAV_EVVIT_Pointer) ||
                        (po_Next->me_Type == EVAV_EVVIT_SubStruct)
                    )
                )
                {
                    if(i_TstIndent == i_StopIndent) break;
                    i_TstIndent--;
                    if(!i_TstIndent) break;
                }

                if(po_Next->me_Type == EVAV_EVVIT_Separator) break;
                _po_Src->mo_ListItems.GetPrev(pos1);
                if(!pos1) break;
            }

            /*
             * We have found the current top level pointer. We must search in the current group
             * if the pointer already exists
             */
            b_Ok = FALSE;
            if(posgrp)
            {
                po_Next1 = mo_ListItems.GetAt(posgrp);
                while(posgrp && (po_Next1->mi_NumGroup == _po_Group->mi_NumGroup + 1))
                {
                    mo_ListItems.GetNext(posgrp);
                    if(po_Next1->mp_Data == po_Next->mp_Data)
                    {
                        b_Ok = TRUE;
                        break;
                    }

                    if(posgrp)
                        po_Next1 = mo_ListItems.GetAt(posgrp);
                }
            }

            if(!b_Ok)
            {
                posgrp = x_AddItemByCopy(po_Next, posgrp, 0);
                mo_ListItems.GetAt(posgrp)->mi_NumFields = 1;
                b_JustAdded = TRUE;
            }

            i_StopIndent++;

            /* The end */
            if(i_StopIndent >= _po_Item->mi_Indent)
            {
l_Zap:

                /* Now search if the item is already present */
                if(!posgrp)
                    return mo_ListItems.GetTailPosition();
                pos = posgrp;
                po_Next1 = mo_ListItems.GetAt(pos);
                while(pos)
                {
                    if
                    (
                        (po_Next1->mi_Indent == _po_Item->mi_Indent) &&
                        (po_Next1->mp_Data == _po_Item->mp_Data) &&
                        (!L_strcmpi(po_Next1->masz_Name, _po_Item->masz_Name))
                    ) return NULL;

                    if(po_Next1->mi_Indent < _po_Item->mi_Indent) break;
                    mo_ListItems.GetNext(pos);
                    if(pos)
                        po_Next1 = mo_ListItems.GetAt(pos);
                }

                if(!b_JustAdded)
                {
                    /* Increase number of fields of previous pointer or substruct */
                    mo_ListItems.GetPrev(posgrp);
                    po_Next1 = mo_ListItems.GetAt(posgrp);
                    po_Next1->mi_NumFields++;

                    /* Add the field after all the other ones */
                    mo_ListItems.GetNext(posgrp);
                    if(posgrp)
                    {
                        po_Next = mo_ListItems.GetAt(posgrp);
                        while(posgrp && (po_Next->mi_Indent > po_Next1->mi_Indent))
                        {
                            mo_ListItems.GetNext(posgrp);
                            if(posgrp)
                                po_Next = mo_ListItems.GetAt(posgrp);
                        }
                    }

                    if(!posgrp)
                        return mo_ListItems.GetTailPosition();
                    mo_ListItems.GetPrev(posgrp);
                }

                break;
            }
        }
    }

    return posgrp;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
POSITION EPRO_cl_Frame::DragDropAddContent
(
    POSITION            pos,
    EPRO_cl_Frame       *po_Src,
    EVAV_cl_ViewItem    *po_Item
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Next;
    POSITION            pos1;
    POSITION            poscur, posref;
    int                 i_TstIndent, i_Head;
    int                 i_Num;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Search pointers before */
    posref = NULL;
    poscur = NULL;
    i_TstIndent = po_Item->mi_Indent - 1;
    pos1 = pos;
    po_Src->mo_ListItems.GetPrev(pos1);
    if(pos1)
    {
        while(1)
        {
            po_Next = po_Src->mo_ListItems.GetAt(pos1);
            if
            (
                (po_Next->mi_Indent == i_TstIndent) &&
                (
                    (po_Next->me_Type == EVAV_EVVIT_Pointer) ||
                    (po_Next->me_Type == EVAV_EVVIT_SubStruct)
                )
            )
            {
                i_TstIndent--;

                i_Head = 0;
                if(poscur)  /* To insert before */
                {
                    mo_ListItems.GetPrev(poscur);
                    if(!poscur) i_Head = 1;
                }

                poscur = x_AddItemByCopy(po_Next, poscur, i_Head);
                mo_ListItems.GetAt(poscur)->mi_NumFields = 1;
                if(posref == NULL) posref = poscur;

                if(!i_TstIndent) break;
            }

            po_Src->mo_ListItems.GetPrev(pos1);
            if(!pos1) break;
        }
    }

    /* Search separator before item to add it */
    pos1 = pos;
    po_Src->mo_ListItems.GetPrev(pos1);
    if(pos1)
    {
        po_Next = po_Src->mo_ListItems.GetAt(pos1);
        while(po_Next->mi_NumGroup != po_Item->mi_NumGroup - 1)
        {
            po_Src->mo_ListItems.GetPrev(pos1);
            if(!pos)
            {
                po_Next = NULL;
                break;
            }

            po_Next = po_Src->mo_ListItems.GetAt(pos1);
        }

        /* Add separator */
        if(po_Next)
        {
            i_Head = 0;
            if(poscur)  /* To insert before */
            {
                mo_ListItems.GetPrev(poscur);
                if(!poscur) i_Head = 1;
            }

            poscur = x_AddItemByCopy(po_Next, poscur, i_Head);
            mo_ListItems.GetAt(poscur)->mb_Locked = TRUE;
            if(posref == NULL) posref = poscur;
        }
    }

    /* Add item itself */
    pos1 = pos;
    poscur = x_AddItemByCopy(po_Item, NULL);
    if((po_Item->me_Type == EVAV_EVVIT_Pointer) || (po_Item->me_Type == EVAV_EVVIT_SubStruct))
    {
        i_Num = po_Item->mi_NumFields;
        po_Src->mo_ListItems.GetNext(pos1);
        if(pos1)
        {
            po_Next = po_Src->mo_ListItems.GetAt(pos1);
            while(i_Num)
            {
                if
                (
                    (po_Next->me_Type == EVAV_EVVIT_Pointer) ||
                    (po_Next->me_Type == EVAV_EVVIT_SubStruct)
                )
                    i_Num += po_Next->mi_NumFields;
                poscur = x_AddItemByCopy(po_Next, poscur, 0);
                po_Src->mo_ListItems.GetNext(pos1);
                if(!pos1) break;
                po_Next = po_Src->mo_ListItems.GetAt(pos1);
                i_Num--;
            }
        }
    }

    return NULL;
}

/*
 ===================================================================================================
    Aim:    Drag&Drop a propertie from another editor.
 ===================================================================================================
 */
void EPRO_cl_Frame::DragDropContent(CPoint pt, EPRO_cl_Frame *po_Src, EVAV_cl_ViewItem *po_Item)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Next, *po_Group;
    POSITION            pos, pos1;
    POSITION            poscur;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Has we drag & drop in an existing group ? */
    po_Group = pst_GetGroupByMouse(pt);

    /* Retreive source item */
    pos = po_Src->mo_ListItems.Find(po_Item);

    /*$2
     -----------------------------------------------------------------------------------------------
        Drag & drop of a separator. Drag all the group.
     -----------------------------------------------------------------------------------------------
     */

    if(po_Item->me_Type == EVAV_EVVIT_Separator)
    {
        pos1 = x_AddItemByCopy(po_Item, NULL, 0, TRUE);
        mo_ListItems.GetAt(pos1)->mb_Locked = TRUE;
        po_Src->mo_ListItems.GetNext(pos);
        if(pos)
        {
            po_Next = po_Src->mo_ListItems.GetAt(pos);
            while(po_Next->mi_NumGroup == po_Item->mi_NumGroup + 1)
            {
                x_AddItemByCopy(po_Next, NULL, 0);
                po_Src->mo_ListItems.GetNext(pos);
                if(!pos) break;

                po_Next = po_Src->mo_ListItems.GetAt(pos);
            }
        }
    }

    /*$2
     -----------------------------------------------------------------------------------------------
        Drag & drop of a single value
     -----------------------------------------------------------------------------------------------
     */
    else
    {
        /*$2
         -------------------------------------------------------------------------------------------
            Add at the end of the view
         -------------------------------------------------------------------------------------------
         */

        if(!po_Group)
        {
            DragDropAddContent(pos, po_Src, po_Item);
        }

        /*$2
         -------------------------------------------------------------------------------------------
            Search to insert at the right place
         -------------------------------------------------------------------------------------------
         */
        else
        {
            /* Expand group if necessary */
            if(!po_Group->mb_Expand)
                mpo_VarsView->mpo_ListBox->OnExpand(po_Group->mi_ID + WM_USER);

            /*
             * For a pointer or a substruct, we must paste each field cause some fields could
             * always be present
             */
            if
            (
                (po_Item->me_Type == EVAV_EVVIT_Pointer) ||
                (po_Item->me_Type == EVAV_EVVIT_SubStruct)
            )
            {
                poscur = DragDropPasteContent(pos, po_Src, po_Group, po_Item);
                if(poscur)
                    poscur = x_AddItemByCopy(po_Item, poscur);
                pos1 = pos;
                po_Src->mo_ListItems.GetNext(pos1);
                if(pos1)
                {
                    po_Next = po_Src->mo_ListItems.GetAt(pos1);
                    while(pos1 && (po_Next->mi_Indent > po_Item->mi_Indent))
                    {
                        poscur = DragDropPasteContent(pos1, po_Src, po_Group, po_Next);
                        if(poscur)
                            poscur = x_AddItemByCopy(po_Next, poscur);
                        po_Src->mo_ListItems.GetNext(pos1);
                        if(pos1)
                            po_Next = po_Src->mo_ListItems.GetAt(pos1);
                    }
                }
            }
            else
            {
                poscur = DragDropPasteContent(pos, po_Src, po_Group, po_Item);
                if(poscur)
                    x_AddItemByCopy(po_Item, poscur);
            }
        }
    }

    /* Reset list */
    mpo_VarsView->SetItemList(&mo_ListItems);
}

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Force to refresh properties view.
 ===================================================================================================
 */
void EPRO_cl_Frame::Refresh(void)
{
    mpo_VarsView->SetItemList(&mo_ListItems);
    mpo_VarsView->Invalidate();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int P2TypeToVavType(int i_Type)
{
    switch(i_Type)
    {
    case LINK_C_ENG_GameObjectOriented:
        return EVAV_EVVIT_GO;
    case LINK_C_ENG_AnimForAction:
		return EVAV_EVVIT_Anim;
    case LINK_C_ENG_Skeleton:
        return EVAV_EVVIT_Skeleton;
    case LINK_C_ENG_Skin:
        return EVAV_EVVIT_Skin;
    }

    return -1;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int EPRO_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_tdst_DragDrop   *pst_DragDrop;
    CRect               o_Rect;
    EVAV_cl_ViewItem    *po_Item, *po_Item1, *po_Group;
    LINK_tdst_Pointer   *p1, *p2;
    EPRO_cl_Frame       *po_Src;
    POSITION            ppos, posgrp, pos, pos1;
    ULONG               ul_Address;
    int                 p, i_Type;
    void                *ptr;
    int                 i_Sel, i_ScrollV, i_ScrollH;
    BIG_INDEX           ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_ul_Msg)
    {
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_DATAHASCHANGED:
        M_MF()->LockDisplay(mpo_VarsView);
        M_MF()->LockDisplay(mpo_VarsView->mpo_ListBox);

        /* Save scroll position to restore after change */
        i_Sel = mpo_VarsView->mpo_ListBox->GetCurSel();
        i_ScrollV = mpo_VarsView->mpo_ListBox->GetScrollPos(SB_VERT);
        i_ScrollH = mpo_VarsView->mpo_ListBox->GetScrollPos(SB_HORZ);

        /* A pointer have been change */
        pos = LINK_gx_PointersJustUpdated.GetStartPosition();
        while(pos)
        {
            LINK_gx_PointersJustUpdated.GetNextAssoc(pos, (void * &) ptr, (void * &) p);
            p2 = LINK_p_SearchPointer(ptr);
            if(p2)
            {
                M_MF()->LockDisplay(mpo_VarsView);
                posgrp = NULL;
                ReplaceItemByPointer(p2, p2, NULL, FALSE, &posgrp);
                while(posgrp)
                    ReplaceItemByPointer(p2, p2, posgrp, FALSE, &posgrp);
                M_MF()->UnlockDisplay(mpo_VarsView);
            }
        }

        /* A pointer have been deleted */
        pos = LINK_gx_PointersJustDeleted.GetStartPosition();
        while(pos)
        {
            LINK_gx_PointersJustDeleted.GetNextAssoc(pos, (void * &) ptr, (void * &) p);
            M_MF()->LockDisplay(mpo_VarsView);
            posgrp = NULL;
            DelItemByPointer(ptr, NULL, &posgrp, TRUE);
            while(posgrp)
                DelItemByPointer(ptr, NULL, &posgrp, TRUE);
            M_MF()->UnlockDisplay(mpo_VarsView);
        }

        mpo_VarsView->SetItemList(&mo_ListItems);

        /* Restore scoll position */
		mpo_VarsView->mpo_ListBox->EnableScroll();
        mpo_VarsView->mpo_ListBox->SetCurSel(i_Sel);

        if(i_ScrollV)
        {
            mpo_VarsView->mpo_ListBox->SetScrollPos(SB_VERT, i_ScrollV);
            mpo_VarsView->mpo_ListBox->SendMessage
                (
                    WM_VSCROLL,
                    SB_THUMBPOSITION + (i_ScrollV << 16),
                    0
                );
        }

        if(i_ScrollH)
        {
            mpo_VarsView->mpo_ListBox->SetScrollPos(SB_HORZ, i_ScrollH);
            mpo_VarsView->mpo_ListBox->SendMessage
                (
                    WM_HSCROLL,
                    SB_THUMBPOSITION + (i_ScrollH << 16),
                    0
                );
        }

        /* Force resize list box */
        M_MF()->UnlockDisplay(mpo_VarsView->mpo_ListBox);
        M_MF()->UnlockDisplay(mpo_VarsView);

        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELFILE:
        ul_Address = LOA_ul_SearchAddress(BIG_PosFile(_ul_Param2));
        if(ul_Address == BIG_C_InvalidIndex) return FALSE;
        ul_Address = (ULONG) LINK_p_SearchPointer((void *) ul_Address);
        if(ul_Address == 0) return FALSE;
        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELDATA:
        ul_Address = (ULONG) LINK_p_SearchPointer((void *) _ul_Param2);
        if(ul_Address == 0) return FALSE;
        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_REFRESH:
        mpo_VarsView->mpo_ListBox->Invalidate();
        return FALSE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;

        /* Drag a data */
        if(pst_DragDrop->i_Type == EDI_DD_Data)
        {
            p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) pst_DragDrop->i_Param2);
            if(!p2) return FALSE;

            /* Transform to a D&D of a LONG ? */
            i_Type = P2TypeToVavType(p2->i_Type);
            if(i_Type == -1) return TRUE;
            if
            (
                mpo_VarsView->mpo_ListBox->DragALong
                    (
                        pst_DragDrop->o_Pt,
                        (void *) &pst_DragDrop->i_Param2,
                        i_Type,
                        NULL,
						NULL,
                        FALSE
                    )
            )
            {
                ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGCOPY1));
                EDI_gst_DragDrop.b_Copy = TRUE;
            }

            return TRUE;
        }

        /* Drag a LONG */
        if(pst_DragDrop->i_Type == EDI_DD_Long)
        {
            return mpo_VarsView->mpo_ListBox->DragALong
                (
                    pst_DragDrop->o_Pt,
                    (void *) pst_DragDrop->i_Param1,
                    pst_DragDrop->i_Param2,
                    NULL,
					NULL,
                    FALSE
                );
        }

        /* Drag & drop from the same editor type */
        if(pst_DragDrop->po_SourceEditor->mi_PosInGArray == mi_PosInGArray)
        {
            /* Can't copy a first level item in the same editor */
            if
            (
                ((EPRO_cl_Frame *) pst_DragDrop->po_SourceEditor == this) &&
                (((EVAV_cl_ViewItem *) pst_DragDrop->i_Param1)->mi_Indent == 1)
            ) return FALSE;

            /* Get group under mouse */
            po_Item = pst_GetGroupByMouse(pst_DragDrop->o_Pt);
            if(po_Item)
            {
                po_Item1 = (EVAV_cl_ViewItem *) pst_DragDrop->i_Param1;
                if(po_Item1->me_Type == EVAV_EVVIT_Separator) return FALSE;
                if(L_strcmpi(po_Item1->masz_Group, po_Item->masz_Group)) return FALSE;
            }

            return TRUE;
        }

        /* Drag a file : Is it loaded and is it registered ? */
        ul_Address = LOA_ul_SearchAddress(BIG_PosFile(pst_DragDrop->ul_FatFile));
        if(ul_Address == BIG_C_InvalidIndex) return FALSE;
        ul_Address = (ULONG) LINK_p_SearchPointer((void *) ul_Address);
        if(!ul_Address) return FALSE;

        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_ADDSELDATA:
        p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
        if(p2 == NULL) break;
        if(!mpo_VarsView->mpo_ListBox->mo_MulSel.Find(p2))
        {
            mpo_VarsView->mpo_ListBox->mo_MulSel.AddTail(p2);
            mpo_VarsView->mpo_ListBox->Invalidate();
        }

        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_DELSELDATA:
        p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
        if(p2 == NULL) break;
        pos = mpo_VarsView->mpo_ListBox->mo_MulSel.Find(p2);
        if(pos)
        {
            mpo_VarsView->mpo_ListBox->mo_MulSel.RemoveAt(pos);
            mpo_VarsView->mpo_ListBox->Invalidate();
        }

        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELFILE:
        _ul_Param2 = LOA_ul_SearchAddress(BIG_PosFile(_ul_Param2));
        if(!_ul_Param2) break;

    case EDI_MESSAGE_SELDATA:
        mpo_VarsView->mpo_ListBox->mo_MulSel.RemoveAll();
        ERR_X_Assert(_ul_Param2);
        p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
        if(p2 == NULL) break;

        /* Add to history */
        ul_Index = LOA_ul_SearchKeyWithAddress(_ul_Param2);
        if(ul_Index != BIG_C_InvalidIndex)
            M_MF()->AddHistoryFile(this, ul_Index);

        M_MF()->LockDisplay(mpo_VarsView);

        /* Delete all groups that are not locked */
        pos = mo_ListItems.GetHeadPosition();
        po_Item1 = NULL;
        while(pos)
        {
            ppos = pos;
            po_Item = mo_ListItems.GetNext(pos);
l_Recom:
            if(po_Item->me_Type == EVAV_EVVIT_Separator)
            {
                if(!po_Item->mb_Locked)
                {
                    /* Look if the pointer is the same type as the new one (for replace) */
                    if(!po_Item1 && pos)
                    {
                        posgrp = ppos;
                        pos1 = pos;
                        po_Item1 = mo_ListItems.GetAt(pos1);
                        if
                        (
                            (
                                (po_Item1->me_Type == EVAV_EVVIT_Pointer) ||
                                (po_Item1->me_Type == EVAV_EVVIT_SubStruct)
                            ) &&
                            (po_Item1->mi_Indent == 1)
                        )
                        {
                            p1 = (LINK_tdst_Pointer *) LINK_p_SearchPointer(po_Item1->mp_Data);
                            if(p1 && p1->i_Type == p2->i_Type)
                            {
                                while(pos)
                                {
                                    ppos = pos;
                                    po_Item = mo_ListItems.GetNext(pos);
                                    if(po_Item->me_Type == EVAV_EVVIT_Separator) goto l_Recom;
                                }

                                break;
                            }
                        }

                        po_Item1 = NULL;
                    }

                    /* Delete all the group */
                    mo_ListItems.RemoveAt(ppos);
                    delete po_Item;
                    while(pos)
                    {
                        ppos = pos;
                        po_Item = mo_ListItems.GetNext(pos);
                        if(po_Item->me_Type == EVAV_EVVIT_Separator) break;
                        delete po_Item;
                        mo_ListItems.RemoveAt(ppos);
                    }

                    if(!pos) break;
                    po_Item = mo_ListItems.GetAt(pos);
                    goto l_Recom;
                }
            }
        }

        if(po_Item1)
		{
			po_Item = ReplaceItemByPointer(p1, p2, posgrp);
		}
        else
        {
            po_Item = AddItemByPointer(p2, NULL, TRUE);
            mpo_VarsView->SetItemList(&mo_ListItems);
            if(!po_Item->mb_Expand)
                mpo_VarsView->mpo_ListBox->OnExpand(po_Item->mi_ID + WM_USER);
        }

        M_MF()->UnlockDisplay(mpo_VarsView);
		Refresh();
        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_ENDDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;

        /* Drag&Drop of a LONG value */
        if(pst_DragDrop->i_Type == EDI_DD_Long)
        {
            mpo_VarsView->mpo_ListBox->DragALong
                (
                    pst_DragDrop->o_Pt,
                    (void *) pst_DragDrop->i_Param1,
                    pst_DragDrop->i_Param2,
                    NULL
                );
            return TRUE;
        }

        /* Drag&Drop from the same editor type */
        if(pst_DragDrop->po_SourceEditor->mi_PosInGArray == mi_PosInGArray)
        {
            po_Src = (EPRO_cl_Frame *) pst_DragDrop->po_SourceEditor;
            po_Item = (EVAV_cl_ViewItem *) pst_DragDrop->i_Param1;
            DragDropContent(pst_DragDrop->o_Pt, po_Src, po_Item);

            /*
             * Force resize, cause if there was no entry before drag & drop, the list box was
             * forced to be empty size
             */
            GetClientRect(&o_Rect);
            OnSize(0, o_Rect.right, o_Rect.bottom + 1);
            OnSize(0, o_Rect.right, o_Rect.bottom);
            return TRUE;
        }

        /* Drag&drop of a file */
        if(pst_DragDrop->i_Type == EDI_DD_File)
        {
            pst_DragDrop->i_Type = EDI_DD_Data;
            pst_DragDrop->i_Param2 = LOA_ul_SearchAddress(BIG_PosFile(pst_DragDrop->ul_FatFile));
        }

        /* Drag&Drop of an engine data */
        if(pst_DragDrop->i_Type == EDI_DD_Data)
        {
            if(pst_DragDrop->i_Param2)
            {
                /* Add corresponding file to history */
                ul_Index = LOA_ul_SearchKeyWithAddress(pst_DragDrop->i_Param2);
                if(ul_Index != BIG_C_InvalidIndex)
                    M_MF()->AddHistoryFile(this, ul_Index);

                p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) pst_DragDrop->i_Param2);
                ERR_X_Assert(p2);

                /* Transform to a D&D of a LONG ? */
                i_Type = P2TypeToVavType(p2->i_Type);
                if(i_Type != -1)
                {
                    if
                    (
                        mpo_VarsView->mpo_ListBox->DragALong
                            (
                                pst_DragDrop->o_Pt,
                                (void *) &pst_DragDrop->i_Param2,
                                i_Type,
                                NULL
                            )
                    ) return TRUE;
                }

                /* Has we drag & drop in an existing group ? */
                po_Group = pst_GetGroupByMouse(pst_DragDrop->o_Pt);
                if(!po_Group)
                {
                    M_MF()->LockDisplay(mpo_VarsView);
                    po_Item = AddItemByPointer(p2, NULL, TRUE, TRUE);
                    mpo_VarsView->SetItemList(&mo_ListItems);
                    if(!po_Item->mb_Expand)
                        mpo_VarsView->mpo_ListBox->OnExpand(po_Item->mi_ID + WM_USER);
                    M_MF()->UnlockDisplay(mpo_VarsView);
                }
                else
                {
                    /* Expand group if necessary */
                    if(!po_Group->mb_Expand)
                        mpo_VarsView->mpo_ListBox->OnExpand(po_Group->mi_ID + WM_USER);

                    /* Search a pointer with the same data in the group, and replace it */
                    posgrp = mo_ListItems.Find(po_Group);
                    pos = posgrp;
                    mo_ListItems.GetNext(pos);
                    while(pos)
                    {
                        po_Item1 = mo_ListItems.GetAt(pos);
                        if
                        (
                            (
                                (po_Item1->me_Type == EVAV_EVVIT_Pointer) ||
                                (po_Item1->me_Type == EVAV_EVVIT_SubStruct)
                            ) &&
                            (po_Item1->mi_Indent == 1)
                        )
                        {
                            p1 = (LINK_tdst_Pointer *) LINK_p_SearchPointer(po_Item1->mp_Data);
                            if(p1 && p1->pv_Data == p2->pv_Data) break;
                        }

                        /* Detect another separator. Stop scan */
                        if(po_Item1->mi_Indent == 0)
                        {
                            po_Item1 = NULL;
                            break;
                        }

                        mo_ListItems.GetNext(pos);
                        if(pos)
                            po_Item1 = mo_ListItems.GetAt(pos);
                        else
                            po_Item1 = NULL;
                    }

                    /* Replace the same pointer ? */
                    if(po_Item1)
                    {
                        M_MF()->LockDisplay(mpo_VarsView);
                        ReplaceItemByPointer(p1, p2, posgrp);
                        M_MF()->UnlockDisplay(mpo_VarsView);
                    }

                    /* Else add a new one */
                    else
                    {
                        M_MF()->LockDisplay(mpo_VarsView);
                        po_Item = AddItemByPointer(p2, posgrp, FALSE, TRUE);
                        mpo_VarsView->SetItemList(&mo_ListItems);
                        if(!po_Item->mb_Expand)
                            mpo_VarsView->mpo_ListBox->OnExpand(po_Item->mi_ID + WM_USER);
                        M_MF()->UnlockDisplay(mpo_VarsView);
                    }
                }
            }

            return TRUE;
        }

        break;
    }

    return TRUE;
}

#endif /* ACTIVE_EDITORS */

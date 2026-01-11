#include "StdAfx.h"
#include "JadeBonesGroupDlg.h"
#include "JadeBonesGroup.h"
#include "resource.h"

#include <vector>
#include <set>


// User Message to receive notification of checkbox change state
#define UM_CHECKSTATECHANGE (WM_USER + 300)

// To easily retrieve the check state
#define CHECK_STATE_UNCHECKED   0x1000
#define CHECK_STATE_CHECKED     0x2000


CJadeBonesGroupDlg::CJadeBonesGroupDlg(CJadeBonesGroup& _jbg, Interface& _ip):
jbg(_jbg),
ip(_ip),
bIsRefreshingUI(false),
bJustCreated(false),
dragNDropTreeBone(NULL)
{
}


CJadeBonesGroupDlg::~CJadeBonesGroupDlg()
{
}

void CJadeBonesGroupDlg::OnValidate(HWND hWnd)
{
    if (jbg.ValidateGroup())
        MessageBox(hWnd, _T("Group is valid."), GetString(IDS_CLASS_NAME_JADE_BONES_GROUP), MB_OK);
}

void CJadeBonesGroupDlg::OnBonesGroupListNotify(HWND hWnd, const NMLISTVIEW& nmv)
{
    LVHITTESTINFO ht = {0};
    assert(nmv.hdr.idFrom == IDC_BONES_GROUP_LIST);
    HWND hList = nmv.hdr.hwndFrom;

    ht.pt = nmv.ptAction;
    ListView_HitTest(hList, &ht);

    switch (nmv.hdr.code)
    {
    case LVN_ITEMCHANGED:
        OnBonesGroupListItemChanged(hWnd, nmv);
		break;

    case LVN_BEGINDRAG:
        OnBonesGroupListBeginDrag(hWnd, nmv);
        break;

    case NM_RCLICK:
        {
            std::vector<int> iListItems;
            int iItem = -1;
            while (1)
            {
                iItem = ListView_GetNextItem(hList, iItem, LVIS_SELECTED);
                if (iItem==-1)
                    break;
                iListItems.push_back(iItem);
            }

            ClientToScreen(hList, &ht.pt);
            RightClickMenu(hWnd, ht.pt.x, ht.pt.y, &iListItems, NULL);
        }
        break;
    }
}

void CJadeBonesGroupDlg::OnBonesGroupListItemChanged(HWND hWnd, const NMLISTVIEW& nmv)
{
    if (bIsRefreshingUI)
        return;

    // Check for check state change
    if (nmv.uChanged&LVIF_STATE && ((nmv.uNewState&TVIS_STATEIMAGEMASK)!=(nmv.uOldState&TVIS_STATEIMAGEMASK)))
	{
        CJadeBone* bone = reinterpret_cast<CJadeBone*>(nmv.lParam);
        if (bone && bone->GetIndex()<jbg.iNumAnimatables)
        {
            bone->SetIsNotAnimated((nmv.uNewState&TVIS_STATEIMAGEMASK)==CHECK_STATE_UNCHECKED);

            // Update the corresponding tree item
            HWND hTree = GetDlgItem(hWnd, IDC_BONES_GROUP_TREE);
            TVITEMEX tvItem;
            tvItem.mask = TVIF_HANDLE | TVIF_STATE;
            tvItem.hItem = bone->GetTreeItemHandle();
            tvItem.stateMask = TVIS_STATEIMAGEMASK;
            tvItem.state = (nmv.uNewState&TVIS_STATEIMAGEMASK);
            TreeView_SetItem(hTree, &tvItem);

            jbg.SetModified(false);
            RefreshStatus(hWnd);
        }
        else
        {
            // Ensure there is no check box beside a hole or a not animatable bone
            ListView_SetItemState( nmv.hdr.hwndFrom, nmv.iItem, 0, LVIS_STATEIMAGEMASK );
        }
    }
}

void CJadeBonesGroupDlg::OnBonesGroupListBeginDrag(HWND hWnd, const NMLISTVIEW& nmv)
{
    if (dragNDropTreeBone)
        OnBonesGroupListEndDrag(hWnd, -1, -1);
    if (!dragNDropListBones.empty())
        OnBonesGroupListEndDrag(hWnd, -1, -1);

    HWND hList = nmv.hdr.hwndFrom;
    int iListItem = -1;
    while (1)
    {
        iListItem = ListView_GetNextItem(hList, iListItem, LVNI_SELECTED);
        if (iListItem==-1)
            break;

        LVITEM lvItem;
        lvItem.mask = LVIF_PARAM;
        lvItem.iItem = iListItem;
        ListView_GetItem(hList, &lvItem);
        CJadeBone* bone = reinterpret_cast<CJadeBone*>(lvItem.lParam);
        if (bone && bone->GetIndex()>=jbg.GetNumAnimatables())
        {
            dragNDropListBones.push_back(bone);

            lvItem.mask = LVIF_STATE;
            lvItem.stateMask = LVIS_CUT;
            lvItem.state = LVIS_CUT;
            ListView_SetItem(hList, &lvItem);
        }
    }

    if (dragNDropListBones.empty())
        return;

    // Direct mouse input to the parent window. 
    SetCapture(hWnd); 
}

void CJadeBonesGroupDlg::OnBonesGroupListMouseMove(HWND hWnd, int x, int y)
{
    if (dragNDropListBones.empty())
        return;

    HWND hList = GetDlgItem(hWnd, IDC_BONES_GROUP_LIST);
    int iDropTarget = GetBonesGroupListDropTarget(hWnd, hList, x, y);

    LVITEM lvItem;
    lvItem.mask = LVIF_STATE;
    lvItem.stateMask = LVNI_DROPHILITED;

    // Remove other drop target highlighed state
    lvItem.state = 0;
    lvItem.iItem = -1;
    while (1)
    {
        lvItem.iItem = ListView_GetNextItem(hList, lvItem.iItem, LVNI_DROPHILITED);
        if (lvItem.iItem==-1)
            break;
        if (lvItem.iItem==iDropTarget)
            continue;

        ListView_SetItem(hList, &lvItem);
    }

    if (iDropTarget!=-1)
    {
        // Set drop target highlighed state
        lvItem.iItem = iDropTarget;
        lvItem.state = LVNI_DROPHILITED;
        ListView_SetItem(hList, &lvItem);
    }
} 

void CJadeBonesGroupDlg::OnBonesGroupListEndDrag(HWND hWnd, int x, int y)
{
    if (dragNDropListBones.empty())
        return;

    ReleaseCapture(); 

    HWND hList = GetDlgItem(hWnd, IDC_BONES_GROUP_LIST);

    LVITEM lvItem;
    lvItem.mask = LVIF_STATE;

    // Remove all cut state
    lvItem.stateMask = LVNI_CUT;
    lvItem.state = 0;
    lvItem.iItem = -1;
    while (1)
    {
        lvItem.iItem = ListView_GetNextItem(hList, lvItem.iItem, LVNI_CUT);
        if (lvItem.iItem==-1)
            break;

        ListView_SetItem(hList, &lvItem);
    }

    // Remove all drop target highlighed state
    lvItem.stateMask = LVNI_DROPHILITED;
    lvItem.state = 0;
    lvItem.iItem = -1;
    while (1)
    {
        lvItem.iItem = ListView_GetNextItem(hList, lvItem.iItem, LVNI_DROPHILITED);
        if (lvItem.iItem==-1)
            break;

        ListView_SetItem(hList, &lvItem);
    }

    int iDropTarget = -1;
    if (x!=-1 && y!=-1)
        iDropTarget = GetBonesGroupListDropTarget(hWnd, hList, x, y);

    if (iDropTarget!=-1)
    {
        // Find the index of the drop target
        TCHAR sz_Buf[10] = "";
        ListView_GetItemText(hList, iDropTarget, 0, sz_Buf, 10);
        int index = _ttoi(sz_Buf);
        assert(index>=jbg.GetNumAnimatables());

        std::vector<int> newIndices(jbg.group.back().GetIndex()+1, -1);

        CJadeBoneList::iterator itBone;
        for(itBone=jbg.group.begin(); itBone!=jbg.group.end(); ++itBone)
            itBone->SetListItemIndex(-1);

        int i;
        if (index<dragNDropListBones.front()->GetIndex())
        {
            // Find the indices of the cut bones
            for (i=0; i<dragNDropListBones.size(); i++)
            {
                CJadeBone& bone = *dragNDropListBones[i];
                bone.SetListItemIndex(index+i);
                if (newIndices[bone.GetIndex()]==-1)
                    newIndices[bone.GetIndex()] = index+i;
            }

            // Find the indices of the subsequent bones
            int iNextIndex = index + dragNDropListBones.size();
            for(itBone=jbg.group.begin(); itBone!=jbg.group.end(); ++itBone)
            {
                if (itBone->GetIndex()<index || itBone->GetListItemIndex()!=-1)
                    continue;

                itBone->SetListItemIndex(iNextIndex);
                if (newIndices[itBone->GetIndex()]==-1)
                    newIndices[itBone->GetIndex()] = iNextIndex;
                iNextIndex++;
            }
        }
        else
        {
            assert(index!=dragNDropListBones.front()->GetIndex());

            // Check if there is a bone at the drop target
            lvItem.mask = LVIF_PARAM;
            lvItem.iItem = iDropTarget;
            ListView_GetItem(hList, &lvItem);
            CJadeBone* dropBone = reinterpret_cast<CJadeBone*>(lvItem.lParam);

            CJadeBoneList::iterator itFirstReindexedBone;
            for(itFirstReindexedBone=jbg.group.begin(); itFirstReindexedBone!=jbg.group.end(); ++itFirstReindexedBone)
            {
                assert(&*itFirstReindexedBone!=dropBone);
                if (&*itFirstReindexedBone==dragNDropListBones.front())
                    break;
            }

            // Move up the bones between the cut bones and the drop target
            int iFirstCutBone = -1;
            int iNextIndex = dragNDropListBones.front()->GetIndex();
            for(itBone=itFirstReindexedBone; itBone!=jbg.group.end(); ++itBone)
            {
                if (iFirstCutBone==-1 && ((dropBone && &*itBone==dropBone) || (!dropBone && itBone->GetIndex()>=index)))
                {
                    // Skip the indices that will be used by the cut bones
                    iFirstCutBone = iNextIndex;
                    iNextIndex += dragNDropListBones.size();
                }

                // Dont reindex the cut bones
                for (i=0; i<dragNDropListBones.size(); i++)
                {
                    if (&*itBone==dragNDropListBones[i])
                        break;
                }
                if (i<dragNDropListBones.size())
                    continue;

                itBone->SetListItemIndex(iNextIndex);
                if (newIndices[itBone->GetIndex()]==-1)
                    newIndices[itBone->GetIndex()] = iNextIndex;
                iNextIndex++;
            }

            if (iFirstCutBone!=-1)
                iNextIndex = iFirstCutBone;

            // Reindex the cut bones
            for (i=0; i<dragNDropListBones.size(); i++)
            {
                CJadeBone& bone = *dragNDropListBones[i];
                bone.SetListItemIndex(iNextIndex);
                if (newIndices[bone.GetIndex()]==-1)
                    newIndices[bone.GetIndex()] = iNextIndex;
                iNextIndex++;
            }
        }

        // Reindex the bones
        for(itBone=jbg.group.begin(); itBone!=jbg.group.end(); ++itBone)
        {
            // Set parent index
            if (itBone->GetParent()!=-1 && itBone->GetParent()<newIndices.size())
            {
                const int iNewParent = newIndices[itBone->GetParent()];
                if (iNewParent!=-1)
                    itBone->SetParent(iNewParent);
            }

            // Set current index
            if (itBone->GetListItemIndex()!=-1)
                itBone->SetIndex(itBone->GetListItemIndex());
        }

        jbg.group.sort();
        jbg.SetModified(false);
        RefreshGroupListAndTree(hWnd);
        for (i=0; i<dragNDropListBones.size(); i++)
            ListView_SetItemState(hList, dragNDropListBones[i]->GetListItemIndex(), LVIS_SELECTED, LVIS_SELECTED);
    }

    dragNDropListBones.clear();
}

int CJadeBonesGroupDlg::GetBonesGroupListDropTarget(HWND hWnd, HWND hList, int x, int y) const
{
    LVHITTESTINFO lvht;  // hit test information 
    lvht.pt.x = x; 
    lvht.pt.y = y; 
    ClientToScreen(hWnd, &lvht.pt);
    ScreenToClient(hList, &lvht.pt);

    // Find out if the pointer is on the item. If it is, 
    // highlight the item as a drop target. 
    int htiTarget = ListView_HitTest(hList, &lvht);
    if (htiTarget==-1) 
        return -1;

    LVITEM lvItem;
    lvItem.mask = LVIF_PARAM;
    lvItem.iItem = htiTarget;
    ListView_GetItem(hList, &lvItem);
    CJadeBone* dropTarget = reinterpret_cast<CJadeBone*>(lvItem.lParam);
    if (dropTarget)
    {
        if (dropTarget->GetIndex()<jbg.GetNumAnimatables())
            return -1;

        for (int i=0; i<dragNDropListBones.size(); i++)
        {
            if (dropTarget==dragNDropListBones[i])
                return -1;

            lvItem.iItem = dragNDropListBones[i]->GetListItemIndex();
            ListView_GetItem(hList, &lvItem);
            if (lvItem.lParam)
            {
                if (dropTarget->GetIndex()==reinterpret_cast<CJadeBone*>(lvItem.lParam)->GetIndex())
                    return -1;
            }
        }
    }
    
    return htiTarget;
}

void CJadeBonesGroupDlg::OnBonesGroupTreeNotify(HWND hWnd, const NMHDR& nmh)
{
    TVHITTESTINFO ht = {0};
    assert(nmh.idFrom == IDC_BONES_GROUP_TREE);
    HWND hTree = nmh.hwndFrom;

    DWORD dwpos = GetMessagePos();
    ht.pt.x = GET_X_LPARAM(dwpos);
    ht.pt.y = GET_Y_LPARAM(dwpos);

    ScreenToClient(hTree, &ht.pt);
    TreeView_HitTest(hTree, &ht);

    switch (nmh.code)
    {
    case TVN_BEGINDRAG:
        OnBonesGroupTreeBeginDrag(hWnd, reinterpret_cast<const NMTREEVIEW&>(nmh));
        break;

    case NM_CLICK:
        // grabbed this little snippet to handle checkbox change state from MSDN Q261289: HOWTO: Know When the User Clicks a Check Box in a TreeView Control
        // When the user clicks the check box of a TreeView item, an NM_CLICK notification is sent to the parent window. 
        // When this occurs, the TVM_HITTEST message returns TVHT_ONITEMSTATEICON. 
        // The TreeView control uses this same condition to toggle the state of the check box. 
        // Unfortunately, the TreeView control toggles the state after the NM_CLICK notification is sent.
        // You can post a user-defined message to the same window that is processing the NM_CLICK notification, 
        // and treat this user-defined message as a notification that the checked state has changed. 
        if(TVHT_ONITEMSTATEICON & ht.flags)
            PostMessage(hWnd, UM_CHECKSTATECHANGE, IDC_BONES_GROUP_TREE, reinterpret_cast<LPARAM>(ht.hItem));
        break;

    case NM_RCLICK:
        RightClickMenu(hWnd, GET_X_LPARAM(dwpos), GET_Y_LPARAM(dwpos), NULL, &ht.hItem);
        break;

    case NM_CUSTOMDRAW:
        if (bJustCreated)
        {
            bJustCreated = false;
            RefreshGroupListAndTree(hWnd);
        }
        break;
    }
}

void CJadeBonesGroupDlg::OnBonesGroupTreeCheckStateChanged(HWND hWnd, HTREEITEM hTreeItem)
{
    HWND hTree = GetDlgItem(hWnd, IDC_BONES_GROUP_TREE);

    TVITEMEX tvItem;
    tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_PARAM;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;
    tvItem.hItem = hTreeItem;
    TreeView_GetItem(hTree, &tvItem);

    CJadeBone* bone = reinterpret_cast<CJadeBone*>(tvItem.lParam);

    if (bone && bone->GetIndex()<jbg.iNumAnimatables)
    {
        bone->SetIsNotAnimated((tvItem.state&TVIS_STATEIMAGEMASK)==CHECK_STATE_UNCHECKED);

        // Update the corresponding list item
        HWND hList = GetDlgItem(hWnd, IDC_BONES_GROUP_LIST);
        ListView_SetItemState(hList, bone->GetListItemIndex(), tvItem.state&TVIS_STATEIMAGEMASK, TVIS_STATEIMAGEMASK);

        jbg.SetModified(false);
        RefreshStatus(hWnd);
    }
    else
    {
        // Ensure there is no check box beside a hole or a not animatable bone
        tvItem.mask = TVIF_HANDLE | TVIF_STATE;
        tvItem.hItem = hTreeItem;
        tvItem.stateMask = TVIS_STATEIMAGEMASK;
        tvItem.state = 0;
        TreeView_SetItem(hTree, &tvItem);
    }
}

void CJadeBonesGroupDlg::OnBonesGroupTreeBeginDrag(HWND hWnd, const NMTREEVIEW& nmv)
{
    if (!dragNDropListBones.empty())
        OnBonesGroupListEndDrag(hWnd, -1, -1);
    if (dragNDropTreeBone)
        OnBonesGroupListEndDrag(hWnd, -1, -1);

    if (!nmv.itemNew.hItem)
        return;

    HWND hTree = nmv.hdr.hwndFrom;
    TVITEMEX tvItem;
    tvItem.mask = TVIF_HANDLE | TVIF_PARAM;
    tvItem.hItem = nmv.itemNew.hItem;
    TreeView_GetItem(hTree, &tvItem);
    dragNDropTreeBone = reinterpret_cast<CJadeBone*>(tvItem.lParam);
    if (dragNDropTreeBone && dragNDropTreeBone->GetIndex()<jbg.GetNumAnimatables())
        dragNDropTreeBone = NULL;

    if (!dragNDropTreeBone)
        return;

    tvItem.mask = TVIF_HANDLE | TVIF_STATE;
    tvItem.stateMask = TVIS_CUT;
    tvItem.state = TVIS_CUT;
    TreeView_SetItem(hTree, &tvItem);

    // Direct mouse input to the parent window. 
    SetCapture(hWnd); 
}

void CJadeBonesGroupDlg::OnBonesGroupTreeMouseMove(HWND hWnd, int x, int y)
{ 
    if (!dragNDropTreeBone)
        return;

    HWND hTree = GetDlgItem(hWnd, IDC_BONES_GROUP_TREE);

    CJadeBone* dropTarget = GetBonesGroupTreeDropTarget(hWnd, hTree, x, y);
    TreeView_SelectDropTarget(hTree, dropTarget?dropTarget->GetTreeItemHandle():NULL); 
} 

void CJadeBonesGroupDlg::OnBonesGroupTreeEndDrag(HWND hWnd, int x, int y)
{
    if (!dragNDropTreeBone)
        return;

    CJadeBone& dragBone = *dragNDropTreeBone;

    //ImageList_EndDrag(); 
    ReleaseCapture(); 
    //ShowCursor(TRUE); 

    HWND hTree = GetDlgItem(hWnd, IDC_BONES_GROUP_TREE);
    TVITEMEX tvItem;
    tvItem.mask = TVIF_HANDLE | TVIF_STATE;
    tvItem.hItem = dragNDropTreeBone->GetTreeItemHandle();
    tvItem.stateMask = TVIS_CUT;
    tvItem.state = 0;
    TreeView_SetItem(hTree, &tvItem);

    TreeView_SelectDropTarget(hTree, NULL); 

    CJadeBone* dropTarget = NULL;
    if (x!=-1 && y!=-1)
        dropTarget = GetBonesGroupTreeDropTarget(hWnd, hTree, x, y);

    if (dropTarget)
    {
        dragNDropTreeBone->SetParent(dropTarget->GetIndex());
        jbg.SetModified(false);
        RefreshGroupListAndTree(hWnd);
        TreeView_SelectItem(hTree, dragNDropTreeBone->GetTreeItemHandle());
    }

    dragNDropTreeBone = NULL;
}

CJadeBone* CJadeBonesGroupDlg::GetBonesGroupTreeDropTarget(HWND hWnd, HWND hTree, int x, int y) const
{
    TVHITTESTINFO tvht;  // hit test information 
    tvht.pt.x = x; 
    tvht.pt.y = y; 
    ClientToScreen(hWnd, &tvht.pt);
    ScreenToClient(hTree, &tvht.pt);

    // Find out if the pointer is on the item. If it is, 
    // highlight the item as a drop target. 
    HTREEITEM htiTarget = TreeView_HitTest(hTree, &tvht);  // handle to target item 
    if (!htiTarget) 
        return NULL;

    TVITEMEX tvItem;
    tvItem.mask = TVIF_HANDLE | TVIF_PARAM;
    tvItem.hItem = htiTarget;
    TreeView_GetItem(hTree, &tvItem);
    CJadeBone* dropTarget = reinterpret_cast<CJadeBone*>(tvItem.lParam);
    if (!dropTarget || dropTarget==dragNDropTreeBone || dropTarget->GetIndex()==dragNDropTreeBone->GetIndex())
        return NULL;

    while (1)
    {
        tvItem.hItem = TreeView_GetParent(hTree, tvItem.hItem);
        if (!tvItem.hItem)
            break;

        TreeView_GetItem(hTree, &tvItem);
        CJadeBone* bone = reinterpret_cast<CJadeBone*>(tvItem.lParam);
        if (!bone || bone==dragNDropTreeBone || bone->GetIndex()==dragNDropTreeBone->GetIndex())
            return NULL;
    }

    return dropTarget;
}

int CJadeBonesGroupDlg::DoModal()
{
	return (DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_JADE_BONES_GROUP), ip.GetMAXHWnd(), DlgProc, (LPARAM) this));
}

BOOL CALLBACK CJadeBonesGroupDlg::DlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	CJadeBonesGroupDlg* dlg = NULL;

	if (msg==WM_INITDIALOG)
	{
		dlg = reinterpret_cast<CJadeBonesGroupDlg*>(lParam);
		SetWindowLong(hWnd, GWL_USERDATA, lParam);
	}
	else
	{
        dlg=reinterpret_cast<CJadeBonesGroupDlg*>(GetWindowLong(hWnd, GWL_USERDATA));
		if (!dlg)
			return FALSE;
	}

    switch (msg)
    {
    case WM_INITDIALOG:
        dlg->Init( hWnd );
        break;

    case WM_DESTROY:
        dlg->Destroy( hWnd );
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_GET_FROM_MAD:
            if (dlg->jbg.GetCreator()==CJadeBonesGroup::EExportTRL)
            {
                if (MessageBox(hWnd, _T("This modification could generate a TRL file that is not compatible with the skeleton."),
                    GetString(IDS_CLASS_NAME_JADE_BONES_GROUP), MB_OKCANCEL|MB_ICONWARNING)!=IDOK)
                    break;
            }
            dlg->OnGetFromFile(hWnd, true);
            break;

        case IDC_GET_FROM_GAS:
            dlg->OnGetFromFile(hWnd, false);
            break;

        case IDC_RESET_FROM_SCENE:
            if (dlg->jbg.GetCreator()==CJadeBonesGroup::EExportTRL)
            {
                if (MessageBox(hWnd, _T("This modification could generate a TRL file that is not compatible with the skeleton."),
                    GetString(IDS_CLASS_NAME_JADE_BONES_GROUP), MB_OKCANCEL|MB_ICONWARNING)!=IDOK)
                    break;
            }
            dlg->jbg.GetFromScene();
            dlg->RefreshGroupListAndTree(hWnd);
            break;

        case IDC_VALIDATE:
            dlg->OnValidate(hWnd);
            break;

        case IDOK:
            if (dlg->jbg.ValidateGroup())
            {
                if (dlg->jbg.IsModified() && dlg->jbg.GetCreator()!=CJadeBonesGroup::EExportTRL)
                {
                    if (dlg->jbg.GetCreator()==CJadeBonesGroup::EExportGAS)
                        MessageBox(hWnd, _T("Bones group has been modified.\nYou will have to save your MAX scene to keep these changes."),
                            GetString(IDS_CLASS_NAME_JADE_BONES_GROUP), MB_OK|MB_ICONINFORMATION);
                    dlg->jbg.SetToScene();
                }
                EndDialog(hWnd, 1);
            }
            break;

        case IDCANCEL:
            if (dlg->jbg.IsModified() && dlg->jbg.GetCreator()==CJadeBonesGroup::EJadeBonesGroupUtility)
            {
                int res = MessageBox(hWnd, _T("Bones group has been modified.\nDo you want the changes to be applied into the scene?"),
                    GetString(IDS_CLASS_NAME_JADE_BONES_GROUP), MB_YESNOCANCEL|MB_ICONWARNING);
                if (res==IDCANCEL)
                    break;
                if (res==IDYES)
                {
                    if (dlg->jbg.ValidateGroup())
                        dlg->jbg.SetToScene();
                    else
                        break;
                }
            }
            EndDialog(hWnd, 0);
            break;
        }
        break;

    case WM_NOTIFY:
		switch(wParam)
		{
        case IDC_BONES_GROUP_LIST:
            dlg->OnBonesGroupListNotify(hWnd, *reinterpret_cast<LPNMLISTVIEW>(lParam));
            return FALSE;

        case IDC_BONES_GROUP_TREE:
            dlg->OnBonesGroupTreeNotify(hWnd, *reinterpret_cast<LPNMHDR>(lParam));
            return FALSE;
		}
		break;

    case WM_MOUSEMOVE:
        if (!dlg->dragNDropListBones.empty())
            dlg->OnBonesGroupListMouseMove(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        else if (dlg->dragNDropTreeBone)
            dlg->OnBonesGroupTreeMouseMove(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return FALSE;

    case WM_LBUTTONUP:
        if (!dlg->dragNDropListBones.empty())
            dlg->OnBonesGroupListEndDrag(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        else if (dlg->dragNDropTreeBone)
            dlg->OnBonesGroupTreeEndDrag(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return FALSE;

    case UM_CHECKSTATECHANGE:
        switch (wParam)
        {
        case IDC_BONES_GROUP_TREE:
            dlg->OnBonesGroupTreeCheckStateChanged(hWnd, reinterpret_cast<HTREEITEM>(lParam));
            break;
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

void CJadeBonesGroupDlg::Init( HWND hWnd )
{
    // Update the window title
    const int textLength = GetWindowTextLength( hWnd );
    if (textLength)
    {
        TCHAR* szBuffer = new TCHAR[textLength+1];
        TCHAR* szBuffer2 = new TCHAR[textLength+32];

        GetWindowText( hWnd, szBuffer, textLength+1 );
        _stprintf( szBuffer2, szBuffer, C_szVersionString );
        SetWindowText( hWnd, szBuffer2 );

        delete [] szBuffer;
        delete [] szBuffer2;
    }

    CenterWindow(hWnd, ip.GetMAXHWnd());

    HWND hList = GetDlgItem(hWnd, IDC_BONES_GROUP_LIST);
    ListView_SetExtendedListViewStyleEx(hList, LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT, LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);
    LVCOLUMN col;
    col.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
    col.pszText = "Rank";
    col.fmt = LVCFMT_RIGHT;
    col.cx = 40;
	ListView_InsertColumn(hList, 0, &col);
    col.pszText = "Bone";
    col.fmt = LVCFMT_LEFT;
    col.cx = 180;
	ListView_InsertColumn(hList, 1, &col);
    col.pszText = "Parent";
    col.fmt = LVCFMT_LEFT;
    col.cx = 180;
	ListView_InsertColumn(hList, 2, &col);

    bJustCreated = true;
}

void CJadeBonesGroupDlg::Destroy( HWND hWnd )
{
}

void CJadeBonesGroupDlg::RightClickMenu(HWND hWnd, int x, int y, const std::vector<int>* piListItems, const HTREEITEM* phTreeItem)
{
    HWND hList = GetDlgItem(hWnd, IDC_BONES_GROUP_LIST);
    HWND hTree = GetDlgItem(hWnd, IDC_BONES_GROUP_TREE);

    HMENU hMenu = CreatePopupMenu();
    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING | MIIM_ID;

    mii.wID = 1;
    mii.dwTypeData = _T("Append New Bones");
    InsertMenuItem(hMenu, 0, TRUE, &mii);

    CJadeBone* bone = NULL;
    if (piListItems)
    {
        assert(!phTreeItem);
        if (!piListItems->empty())
        {
            mii.wID = 10;
            mii.dwTypeData = _T("Remove");
            InsertMenuItem(hMenu, 1, TRUE, &mii);
        }

        if (piListItems->size()==1)
        {
            LVITEM lvItem;
            lvItem.mask = LVIF_PARAM;
            lvItem.iItem = (*piListItems)[0];
            ListView_GetItem(hList, &lvItem);
            bone = reinterpret_cast<CJadeBone*>(lvItem.lParam);

            if (bone)
            {
                mii.wID = 11;
                mii.dwTypeData = _T("Set As Last Animatable");
                InsertMenuItem(hMenu, 2, TRUE, &mii);
            }
        }

        mii.wID = 12;
        mii.dwTypeData = _T("Set No Animatables");
        InsertMenuItem(hMenu, 3, TRUE, &mii);
    }

    int iHole = -1;
    if (phTreeItem && *phTreeItem)
    {
        assert(!piListItems);
        TVITEMEX tvItem;
        tvItem.mask = TVIF_HANDLE | TVIF_PARAM;
        tvItem.hItem = *phTreeItem;
        TreeView_GetItem(hTree, &tvItem);
        bone = reinterpret_cast<CJadeBone*>(tvItem.lParam);

        if (bone && bone->GetIndex()>=jbg.GetNumAnimatables())
        {
            mii.wID = 20;
            mii.dwTypeData = _T("Remove Bone");
            InsertMenuItem(hMenu, 1, TRUE, &mii);
        }

        if (!bone && *phTreeItem)
        {
            // Find the index of the hole
            tvItem.hItem = TreeView_GetChild(hTree, *phTreeItem);
            assert(tvItem.hItem);
            TreeView_GetItem(hTree, &tvItem);
            CJadeBone* childBone = reinterpret_cast<CJadeBone*>(tvItem.lParam);
            assert(childBone && childBone->GetParent()!=-1);

            if (childBone->GetParent()>=jbg.GetNumAnimatables())
            {
                iHole = childBone->GetParent();
                mii.wID = 21;
                mii.dwTypeData = _T("Remove Hole");
                InsertMenuItem(hMenu, 2, TRUE, &mii);
            }
        }

        if (bone && bone->GetIndex()>=jbg.GetNumAnimatables() && *phTreeItem && bone->GetParent()!=-1)
        {
            mii.wID = 22;
            mii.dwTypeData = _T("Set At Root");
            InsertMenuItem(hMenu, 3, TRUE, &mii);
        }
    }

    UINT id = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, x, y, 0, hWnd, 0);
    switch (id)
    {
    case 1:
        if (jbg.AppendNewBones())
            RefreshGroupListAndTree(hWnd);
        break;

    case 10:
        BonesGroupListRemove(hWnd, *piListItems);
        RefreshGroupListAndTree(hWnd);
        break;

    case 11:
    case 12:
        if (jbg.GetCreator()==CJadeBonesGroup::EExportTRL)
        {
            if (MessageBox(ip.GetMAXHWnd(), _T("This modification could generate a TRL file that is not compatible with the skeleton."),
                GetString(IDS_CLASS_NAME_JADE_BONES_GROUP), MB_OKCANCEL|MB_ICONWARNING)!=IDOK)
                break;
        }

        jbg.iNumAnimatables = (id==11)?(bone->GetIndex()+1):0;
        jbg.SetModified(true);
        RefreshGroupListAndTree(hWnd);
        break;

    case 20:
        {
            const int index = bone->GetIndex();
            assert(index>=jbg.GetNumAnimatables());
            jbg.RemoveBone(*bone);
            CJadeBoneList::const_iterator itBone = jbg.FindInGroup(index);
            if (itBone==jbg.group.end())
                jbg.DecrementFollowingIndices(index);
            RefreshGroupListAndTree(hWnd);
        }
        break;

    case 21:
        assert(iHole!=-1 && iHole>=jbg.GetNumAnimatables());
        jbg.DecrementFollowingIndices(iHole);
        RefreshGroupListAndTree(hWnd);
        break;

    case 22:
        bone->SetParent(-1);
        jbg.SetModified(false);
        RefreshGroupListAndTree(hWnd);
        break;
    }
}

// Removes the items in the list view by reindexing all following bones
void CJadeBonesGroupDlg::BonesGroupListRemove(HWND hWnd, const std::vector<int>& iListItems)
{
    HWND hList = GetDlgItem(hWnd, IDC_BONES_GROUP_LIST);

    std::set<int> holes;

    // First remove all bones
    const int numListItemIndices = iListItems.size();
    for (int i=0; i<numListItemIndices; i++)
    {
        LVITEM lvItem;
        lvItem.mask = LVIF_PARAM;
        lvItem.iItem = iListItems[i];
        ListView_GetItem(hList, &lvItem);

        if (lvItem.lParam)
        {
            const CJadeBone& bone = *reinterpret_cast<CJadeBone*>(lvItem.lParam);
            const int index = bone.GetIndex();
            if (index>=jbg.GetNumAnimatables())
            {
                jbg.RemoveBone(bone);
                CJadeBoneList::const_iterator itBone = jbg.FindInGroup(index);
                if (itBone==jbg.group.end())
                    holes.insert(index);
            }
        }
        else
        {
            // Find the index of the hole
            TCHAR sz_Buf[10] = "";
            ListView_GetItemText(hList, lvItem.iItem, 0, sz_Buf, 10);
            int index = _ttoi(sz_Buf);
            assert(index>=0);
            if (index>=jbg.GetNumAnimatables())
                holes.insert(index);
        }
    }

    // Then remove the holes
    std::set<int>::const_iterator itHole = holes.end();
    while (itHole!=holes.begin())
    {
        --itHole;
        jbg.DecrementFollowingIndices(*itHole);
    }
}

void CJadeBonesGroupDlg::OnGetFromFile(HWND hWnd, bool bFromMad)
{
	OPENFILENAME OpenFileName;
	TCHAR szFile[_MAX_PATH] = _T("\0");
	TCHAR finalPath[_MAX_PATH] = _T("\0");
	TCHAR szFileName[100], *szLast;
	
	/*if (_r_linkObjects.szBankName[0])
		_tcscpy( szFile, _r_linkObjects.szBankName );*/
	
	// fill the OPENFILENAME required structure:	
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
	OpenFileName.hwndOwner         = hWnd;
 	OpenFileName.hInstance         = NULL;
    OpenFileName.lpstrFilter       = bFromMad?"MAD Files (*.MAD)\0*.mad\0":"GAS Files (*.GAS)\0*.gas\0";
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter    = 0;
    OpenFileName.nFilterIndex      = 0;
    OpenFileName.lpstrFile         = szFile;
	OpenFileName.nMaxFile          = sizeof(szFile);
	OpenFileName.lpstrFileTitle    = NULL;
	OpenFileName.nMaxFileTitle     = 0;
	
	// the path to the first /3ds directory
	/*if (_tcslen( _exp.GetActorDir() ))
		OpenFileName.lpstrInitialDir   = _exp.GetActorDir();
	// the current directory if there isn't any \3ds directory
	else*/
		OpenFileName.lpstrInitialDir   = NULL;
	
        OpenFileName.lpstrTitle        = bFromMad?"Open a MAD File":"Open a GAS File";
    OpenFileName.nFileOffset       = 0;
    OpenFileName.nFileExtension    = 0;
	OpenFileName.lpstrDefExt       = NULL;
	OpenFileName.lCustData         = NULL;
	OpenFileName.lpfnHook 		   = NULL;
	OpenFileName.lpTemplateName    = NULL;
    OpenFileName.Flags             = /*OFN_ALLOWMULTISELECT | */OFN_EXPLORER |
									 OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
									 LBS_EXTENDEDSEL ;
	szFileName[0] = '\0';
	if (GetOpenFileName(&OpenFileName))
	{
		strcpy(szFileName,&OpenFileName.lpstrFile[0]);
	
		szLast = strrchr(szFileName,'\\');
		if(szLast)
		{
			szLast++;
			// call the function who will add the mod file 
			//in the global structures
            if (bFromMad)
                jbg.ProcessMad(szFileName);
            else
                jbg.ProcessGas(szFileName);

            RefreshGroupListAndTree(hWnd);
		}
	}			
}

void CJadeBonesGroupDlg::RefreshStatus(HWND hWnd)
{
    HWND hStatus = GetDlgItem(hWnd, IDC_STATUS);
    SetWindowText(hStatus, jbg.statusStr.data());
}

// This method assumes the group is sorted
void CJadeBonesGroupDlg::RefreshGroupListAndTree(HWND hWnd)
{
    bIsRefreshingUI = true;

    RefreshStatus(hWnd);

    TCHAR asz_Temp[512];

    // Init list view
    HWND hList = GetDlgItem(hWnd, IDC_BONES_GROUP_LIST);
    ListView_DeleteAllItems(hList);

	LVITEM lvItem, lvSubItem;
	lvItem.mask = LVIF_TEXT | LVIF_PARAM;
	lvItem.pszText = asz_Temp;
    lvItem.iSubItem = 0;
	lvSubItem.mask = LVIF_TEXT;

    // Init tree view
    HWND hTree = GetDlgItem(hWnd, IDC_BONES_GROUP_TREE);
    TreeView_DeleteAllItems(hTree);

    // Reset all UI info and get last index
    CJadeBoneList::const_iterator itBone;
    int lastIndex = 0;
    for (itBone=jbg.group.begin(); itBone!=jbg.group.end(); ++itBone)
    {
        lastIndex = itBone->GetIndex();
        itBone->SetListItemIndex();
        itBone->SetTreeItemHandle();
    }

    hEmptyTreeItems.clear();
    hEmptyTreeItems.resize(lastIndex+1, NULL);

    // Insert all group items in the list view
    itBone = jbg.group.begin();
    for (int index=0; itBone!=jbg.group.end() && index<=lastIndex; index++)
	{
        if (itBone->GetIndex()!=index)
        {
            // There is no bone at this index, insert a hole
            assert(itBone->GetIndex()>index);
		    _stprintf(asz_Temp, _T("%.2d"), index);
            lvItem.lParam = NULL;
		    lvSubItem.iItem = ListView_InsertItem(hList, &lvItem);
            lvSubItem.iSubItem = 1;
            lvSubItem.pszText = _T("<Hole>");
            ListView_SetItem(hList, &lvSubItem);

            ListView_SetItemState( hList, lvSubItem.iItem, 0, LVIS_STATEIMAGEMASK );
        }
        else
        {
            for (; itBone!=jbg.group.end() && itBone->GetIndex()==index; ++itBone)
            {
		        _stprintf(asz_Temp, _T("%.2d"), index);
                const CJadeBone& bone = *itBone;
                lvItem.lParam = reinterpret_cast<LPARAM>(&*itBone);
		        bone.SetListItemIndex(ListView_InsertItem(hList, &lvItem));

                lvSubItem.iItem = bone.GetListItemIndex();
                lvSubItem.iSubItem = 1;
                lvSubItem.pszText = bone.GetNode().GetName();
                ListView_SetItem(hList, &lvSubItem);

                if (bone.GetIndex()>=jbg.iNumAnimatables) // Clear the check if it is not animatable
                {
                    ListView_SetItemState( hList, lvSubItem.iItem, 0, LVIS_STATEIMAGEMASK );
                }
                else if (!bone.IsNotAnimated()) // Set the check if it is animated
                {
                    ListView_SetItemState( hList, lvSubItem.iItem, CHECK_STATE_CHECKED, LVIS_STATEIMAGEMASK );
                }
                
                if (bone.GetParent()!=-1)
                {
                    lvSubItem.iSubItem = 2;
                    bool bAmbiguous = false;
                    const CJadeBone* parentBone = jbg.FindParentBone(bone, &bAmbiguous);
                    _stprintf(asz_Temp, _T("%.2d  -  %s"), bone.GetParent(), parentBone?parentBone->GetNode().GetName():_T("<Hole>"));
                    if (bAmbiguous)
                        _tcscat(asz_Temp, _T(" (ambiguous)"));
                    lvSubItem.pszText = asz_Temp;
                    ListView_SetItem(hList, &lvSubItem);
                }
            }
        }
	}

    // Insert all group items in the tree view
    for (itBone=jbg.group.begin(); itBone!=jbg.group.end(); ++itBone)
    {
        InsertBoneInTree(hTree, itBone->GetIndex(), &*itBone);
    }

    bIsRefreshingUI = false;
}

HTREEITEM CJadeBonesGroupDlg::InsertBoneInTree(HWND hTree, int index, const CJadeBone* bone)
{
    // Ensure the table is big enough
    while (index>=hEmptyTreeItems.size())
        hEmptyTreeItems.push_back(NULL);
    
    if (bone && bone->GetTreeItemHandle()!=NULL)
        return bone->GetTreeItemHandle();

    if (!bone && hEmptyTreeItems[index]!=NULL)
        return hEmptyTreeItems[index];

    // This is to prevent circular hierarchy
    if (bone)
        bone->SetTreeItemHandle(TVI_ROOT);
    else
        hEmptyTreeItems[index] = TVI_ROOT;

    TCHAR asz_Temp[512];
    TV_INSERTSTRUCT tvis;

    // Get the parent tree item handle
    tvis.hParent = TVI_ROOT;
    const CJadeBone* parentBone = NULL;
    bool bAmbiguousParent = false;
    if (bone && bone->GetParent()!=-1)
    {
        parentBone = jbg.FindParentBone(*bone, &bAmbiguousParent);
        tvis.hParent = InsertBoneInTree(hTree, bone->GetParent(), parentBone);
    }

    tvis.itemex.mask = TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
    tvis.itemex.pszText = asz_Temp;
    tvis.itemex.stateMask = TVIS_EXPANDED;

    _stprintf(asz_Temp, _T("%.2d  -  %s"), index, bone?bone->GetNode().GetName():_T("<Hole>"));
    if (bAmbiguousParent)
        _tcscat(asz_Temp, _T(" (ambiguous parent)"));
    if (parentBone && tvis.hParent==TVI_ROOT)
    {
        TCHAR asz_Temp2[512];
        _stprintf(asz_Temp2, _T(" (circular parent : %.2d  -  %s)"), bone->GetParent(), parentBone->GetNode().GetName());
        _tcscat(asz_Temp, asz_Temp2);
    }

    tvis.itemex.state = TVIS_EXPANDED;
    tvis.itemex.lParam = reinterpret_cast<LPARAM>(bone);

    const HTREEITEM hTreeItem = TreeView_InsertItem(hTree, &tvis);
    assert(hTreeItem);
    if (bone)
        bone->SetTreeItemHandle(hTreeItem);
    else
        hEmptyTreeItems[index] = hTreeItem;

    // Set the check state
    TVITEMEX tvItem;
    tvItem.mask = TVIF_HANDLE | TVIF_STATE;
    tvItem.hItem = hTreeItem;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;
    tvItem.state = 0;
    if (bone && bone->GetIndex()<jbg.iNumAnimatables)
        tvItem.state = bone->IsNotAnimated()?CHECK_STATE_UNCHECKED:CHECK_STATE_CHECKED;
    TreeView_SetItem(hTree, &tvItem);

    return hTreeItem;
}

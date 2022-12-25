#ifndef __JADEBONESGROUPDLG__H
#define __JADEBONESGROUPDLG__H


#include <vector>

class CJadeBone;
class CJadeBonesGroup;


class CJadeBonesGroupDlg
{
public:
    //Constructor/Destructor
    CJadeBonesGroupDlg(CJadeBonesGroup& _jbg, Interface& _ip);
    ~CJadeBonesGroupDlg();

    int DoModal();

private:
    static BOOL CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void Init(HWND hWnd);
    void Destroy(HWND hWnd);

    void RightClickMenu(HWND hWnd, int x, int y, const std::vector<int>* piListItems, const HTREEITEM* phTreeItem);

    void BonesGroupListRemove(HWND hWnd, const std::vector<int>& iListItems);

    void OnGetFromFile(HWND hWnd, bool bFromMad);

    void OnValidate(HWND hWnd);

    void OnBonesGroupListNotify(HWND hWnd, const NMLISTVIEW& nmv);
    void OnBonesGroupListItemChanged(HWND hWnd, const NMLISTVIEW& nmv);
    void OnBonesGroupListBeginDrag(HWND hWnd, const NMLISTVIEW& nmv);
    void OnBonesGroupListMouseMove(HWND hWnd, int x, int y);
    void OnBonesGroupListEndDrag(HWND hWnd, int x, int y);
    int GetBonesGroupListDropTarget(HWND hWnd, HWND hList, int x, int y) const;

    void OnBonesGroupTreeNotify(HWND hWnd, const NMHDR& nmh);
    void OnBonesGroupTreeCheckStateChanged(HWND hWnd, HTREEITEM hTreeItem);
    void OnBonesGroupTreeBeginDrag(HWND hWnd, const NMTREEVIEW& nmv);
    void OnBonesGroupTreeMouseMove(HWND hWnd, int x, int y);
    void OnBonesGroupTreeEndDrag(HWND hWnd, int x, int y);
    CJadeBone* GetBonesGroupTreeDropTarget(HWND hWnd, HWND hTree, int x, int y) const;

    void RefreshStatus(HWND hWnd);
    void RefreshGroupListAndTree(HWND hWnd);
    HTREEITEM InsertBoneInTree(HWND hTree, int index, const CJadeBone* bone);

    CJadeBonesGroup& jbg;
    Interface& ip;

    std::vector<HTREEITEM> hEmptyTreeItems;
    bool bIsRefreshingUI;
    bool bJustCreated;

    std::vector<CJadeBone*> dragNDropListBones;
    CJadeBone* dragNDropTreeBone;
};


#endif // __JADEBONESGROUPDLG__H

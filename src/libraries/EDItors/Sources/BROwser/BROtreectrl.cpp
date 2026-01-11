/*$T BROtreectrl.cpp GC!1.71 02/17/00 14:52:18 */

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
#include "BROframe.h"
#include "BROtreectrl.h"
#include "BROmsg.h"
#include "EDImainframe.h"
#include "ENGine/Sources/ENGinit.h"
#include "EDIapp.h"
#include "LINKs/LINKstruct.h"
#include "EDIpaths.h"
#include "EDIicons.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

#define EDI_M_GetBrowserFrame() ((EBRO_cl_Frame *) (GetParent()->GetParent()->GetParent()))

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EBRO_cl_TreeCtrl, CTreeCtrl)
    ON_WM_LBUTTONDBLCLK()
    ON_WM_SETFOCUS()
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChange)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnExpanded)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnExpanding)
    ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
    ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    OVERWRITE
 ***********************************************************************************************************************
 */

int sgi_LockUpdate = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EBRO_cl_TreeCtrl::EBRO_cl_TreeCtrl(void)
{
    mb_LockSel = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::DeleteAllItems(void)
{
    M_MF()->LockDisplay(this);

    sgi_LockUpdate++;
    CTreeCtrl::DeleteAllItems();
    sgi_LockUpdate--;

    M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::Expand(HTREEITEM hItem, UINT nCode)
{
    M_MF()->LockDisplay(this);

    CTreeCtrl::Expand(hItem, nCode);

    M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::EnsureVisible(HTREEITEM hItem)
{
    M_MF()->LockDisplay(this);

    CTreeCtrl::EnsureVisible(hItem);

    M_MF()->UnlockDisplay(this);
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
void EBRO_cl_TreeCtrl::OnRButtonDown(UINT n, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~*/
    HTREEITEM   h_Item;
    UINT        ui_Dum;
    /*~~~~~~~~~~~~~~~~~~~*/

    h_Item = HitTest(pt, &ui_Dum);
    if(h_Item)
    {
		::SetFocus(this->m_hWnd);
        SelectItem(h_Item);
        ClientToScreen(&pt);
        EDI_M_GetBrowserFrame()->OnCtrlPopup(pt);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::OnSelChange(NMHDR *pNotifyStruct2, LRESULT *result)
{
	NM_TREEVIEW *pNotifyStruct;
	pNotifyStruct = (NM_TREEVIEW *)pNotifyStruct2;
_Try_
    if((sgi_LockUpdate == 0) && !mb_LockSel)
    {
        CTreeCtrl::EnsureVisible(pNotifyStruct->itemNew.hItem);
        EDI_M_GetBrowserFrame()->OnTreeCtrlSelChange();
    }

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::OnSetFocus(CWnd *pold)
{
    CTreeCtrl::OnSetFocus(pold);
    EDI_M_GetBrowserFrame()->RefreshMenu();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::OnExpanded(NMHDR *pNotifyStruct2, LRESULT *result)
{
	NM_TREEVIEW *pNotifyStruct;
	pNotifyStruct = (NM_TREEVIEW *)pNotifyStruct2;

_Try_
    EDI_M_GetBrowserFrame()->OnTreeCtrlExpanded(pNotifyStruct);
_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::OnExpanding(NMHDR *pNotifyStruct2, LRESULT *result)
{
	NM_TREEVIEW *pNotifyStruct;
	pNotifyStruct = (NM_TREEVIEW *)pNotifyStruct2;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::OnBeginDrag(NMHDR *pNotifyStruct2, LRESULT *result)
{
		NM_TREEVIEW *pNotifyStruct;
	pNotifyStruct = (NM_TREEVIEW *)pNotifyStruct2;

_Try_
    SetFocus();
    CTreeCtrl::EnsureVisible(pNotifyStruct->itemNew.hItem);
    CTreeCtrl::SelectItem(pNotifyStruct->itemNew.hItem);
    EDI_M_GetBrowserFrame()->OnTreeCtrlBeginDrag(pNotifyStruct->ptDrag);
_Catch_
_End_
}

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    To create hiearchy in tree control depending on current open bigfile.
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::FillDirRecurse
(
    int         _i_NumCopy,
    HTREEITEM   _h_Parent,
    BIG_INDEX   _ul_ParentDir,
    BIG_INDEX   _ul_CurDir,
    char        *_psz_FullPath
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TV_INSERTSTRUCT st_TreeCtrlItem;
    HTREEITEM       h_TreeItem;
    BIG_INDEX       ul_SubDir;
    char            asz_FullPath[BIG_C_MaxLenPath];
    char            asz_Temp[BIG_C_MaxLenPath];
    char            asz_Ext[L_MAX_PATH];
    UINT            ui_Flags;
    BAS_tdst_barray *ptree;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Get icon in system image list */
    ui_Flags = SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES;

    /*
     * In engine mode, check if a pointer is registered in current path, else exit
     */
    if(EDI_M_GetBrowserFrame()->mst_Ini.b_EngineMode)
    {
        L_strcpy(asz_FullPath, "c:\\");
        if(!LINK_gx_TreeToPointers.Lookup(_psz_FullPath, (void * &) ptree))
            return;
        st_TreeCtrlItem.item.iImage = M_MF()->i_GetIconImage(_psz_FullPath, NULL);
        if(st_TreeCtrlItem.item.iImage == EDI_IMAGE_FOLDER1)
            st_TreeCtrlItem.item.iImage = EDI_IMAGE_FOLDER2;
    }
    else
    {
        L_strcpy(asz_FullPath, BIG_NameDir(_ul_CurDir));
        st_TreeCtrlItem.item.iImage = M_MF()->i_GetIconImage(_psz_FullPath, NULL);
    }

    /* Create current dir in bigfile (if necessary) */
    st_TreeCtrlItem.hParent = _h_Parent;
    st_TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    st_TreeCtrlItem.hInsertAfter = TVI_SORT;
    st_TreeCtrlItem.item.pszText = BIG_NameDir(_ul_CurDir);
    st_TreeCtrlItem.item.iSelectedImage = st_TreeCtrlItem.item.iImage;
    st_TreeCtrlItem.item.state = 0;

    /* The dir is linked to external path ? */
    if(!EDI_M_GetBrowserFrame()->mst_Ini.b_EngineMode)
    {
        if(EDI_M_GetBrowserFrame()->b_GetLinkedPath(_ul_CurDir, asz_Ext))
        {
            L_strcpy(asz_Temp, BIG_NameDir(_ul_CurDir));
            L_strcat(asz_Temp, " (");
            L_strcat(asz_Temp, asz_Ext);
            L_strcat(asz_Temp, ")");
            st_TreeCtrlItem.item.pszText = asz_Temp;

            st_TreeCtrlItem.item.mask |= TVIF_STATE;
            st_TreeCtrlItem.item.state |= TVIS_BOLD;
        }
    }

    /* Retreive infos in FAT */
    if(BIG_gst.dst_DirTable[_ul_CurDir].st_BRO.b_IsExpanded[_i_NumCopy])
    {
        st_TreeCtrlItem.item.mask |= TVIF_STATE;
        st_TreeCtrlItem.item.state |= TVIS_EXPANDED;
    }

    st_TreeCtrlItem.item.stateMask = st_TreeCtrlItem.item.state;

    /* Additional infos for item */
    st_TreeCtrlItem.item.lParam = _ul_CurDir;

    /* Insert item in tree ctrl */
    h_TreeItem = InsertItem(&st_TreeCtrlItem);

    /* Recurse call for all subdirs of source bigfile */
    ul_SubDir = BIG_SubDir(_ul_CurDir);
    while(ul_SubDir != BIG_C_InvalidIndex)
	{
		if(L_strlen(_psz_FullPath) + L_strlen(BIG_NameDir(ul_SubDir)) < BIG_C_MaxLenPath - 1)
		{
			/* Compute full path name */
			L_strcpy(asz_FullPath, _psz_FullPath);
			L_strcat(asz_FullPath, "/");
			L_strcat(asz_FullPath, BIG_NameDir(ul_SubDir));

			/* Recurse call */
			FillDirRecurse(_i_NumCopy, h_TreeItem, _ul_CurDir, ul_SubDir, asz_FullPath);
		}

        /* Pass to brother (dir) */
        ul_SubDir = BIG_NextDir(ul_SubDir);
    }
}

/*
 =======================================================================================================================
    Aim:    Main function call to fill a treectrl. This function call a recursive one just after.
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::FillDir
(
    EDI_cl_BaseView *_po_MyView,
    int             _i_NumCopy,
    HTREEITEM       _h_Parent,
    BIG_INDEX       _ul_ParentDir,
    BIG_INDEX       _ul_CurDir
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Path[BIG_C_MaxLenPath];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_ul_CurDir == BIG_C_InvalidIndex) return;

    mpo_MyView = _po_MyView;
    M_MF()->LockDisplay(this);
    BIG_ComputeFullName(_ul_CurDir, asz_Path);
    FillDirRecurse(_i_NumCopy, _h_Parent, _ul_ParentDir, _ul_CurDir, asz_Path);
    M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
    Aim:    Cause display is not the same as the real name, retreive item text must retreive the
            real name of the dir.
 =======================================================================================================================
 */
CString EBRO_cl_TreeCtrl::GetItemText(HTREEITEM _h_Item)
{
    return BIG_NameDir(GetItemData(_h_Item));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_TreeCtrl::Collapse(void)
{
	Expand(GetRootItem(), TVE_COLLAPSE);
}

#endif /* ACTIVE_EDITORS */

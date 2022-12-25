/*$T BROgrpctrl.cpp GC!1.71 01/13/00 11:26:01 */

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
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BROstrings.h"
#include "BROframe.h"
#include "BROlistctrl.h"
#include "BROgrpctrl.h"
#include "BROmsg.h"
#include "EDImainframe.h"
#include "ENGine/Sources/ENGinit.h"
#include "LINKs/LINKstruct.h"
#include "EDIapp.h"
#include "BASe/CLIbrary/CLImem.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

#define EDI_M_GetBrowserFrame() ((EBRO_cl_Frame *) (GetParent()->GetParent()))

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EBRO_cl_GrpCtrl, CListCtrlStyled)
    ON_WM_SETFOCUS()
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)
    ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_PARENTNOTIFY()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EBRO_cl_GrpCtrl::EBRO_cl_GrpCtrl(void)
{
    mb_LockSel = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::OnRButtonDown(UINT n, CPoint pt)
{
    CListCtrlStyled::OnLButtonDown(n, pt);
    ClientToScreen(&pt);
    EDI_M_GetBrowserFrame()->OnCtrlPopup(pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::OnSetFocus(CWnd *pold)
{
    CListCtrlStyled::OnSetFocus(pold);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::OnBeginDrag(NMHDR *pNotifyStruct2, LRESULT *result)
{
		NM_LISTVIEW *pNotifyStruct;
	pNotifyStruct = (NM_LISTVIEW *)pNotifyStruct2;

_Try_
    SetFocus();
    EnsureVisible(pNotifyStruct->iItem, FALSE);
    SetItemState(pNotifyStruct->iItem, LVIS_SELECTED, LVIF_STATE);
    Update(pNotifyStruct->iItem);
    EDI_M_GetBrowserFrame()->OnGrpCtrlBeginDrag(pNotifyStruct->ptAction, pNotifyStruct->iItem);
_Catch_
_End_
}

/*
 =======================================================================================================================
    Aim:    Right button click on header columns. Display a menu to show/hide all columns.
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::OnParentNotify(UINT message, LONG lParam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
    CMenu   o_Menu;
    CPoint  o_Point;
    char    asz_Temp[200];
    int     i;
    /*~~~~~~~~~~~~~~~~~~~~~~*/

    if(message == WM_RBUTTONDOWN)
    {
        o_Menu.CreatePopupMenu();

        for(i = 1; i < EBRO_C_GrpCtrlMaxColumns; i++)
        {
            /* In engine mode, special process */
            if((EDI_M_GetBrowserFrame()->mst_Ini.b_EngineMode) && (i > 1))
                continue;

            L_strcpy(asz_Temp, EBRO_STR_Csz_ListColDsp);
            L_strcat(asz_Temp, EBRO_gasz_GrpColNames[i]);
            o_Menu.InsertMenu(i - 1, MF_BYPOSITION, WM_USER + 200 + i, asz_Temp);
            if(EDI_M_GetBrowserFrame()->mst_Ini.ai_NumColGrpCtrl[i] != -1)
                o_Menu.CheckMenuItem(i - 1, MF_CHECKED | MF_BYPOSITION);
        }

        o_Point.x = LOWORD(lParam);
        o_Point.y = HIWORD(lParam);
        ClientToScreen(&o_Point);
        o_Menu.TrackPopupMenu(TPM_LEFTALIGN, o_Point.x, o_Point.y, EDI_M_GetBrowserFrame());
        return;
    }

    CListCtrlStyled::OnParentNotify(message, lParam);
}

static BIG_INDEX    sgul_Group = BIG_C_InvalidIndex;

/*
 =======================================================================================================================
    Aim:    Compute a string for a given file and list column.

    In:     _ul_File    File concerned.
            _i_NumCol   The number of the column.
            _psz_Out    Output string.
 =======================================================================================================================
 */
static void s_ComputeColName(BIG_INDEX _ul_File, int _i_NumCol, char *_psz_Out)
{
    /*~~~~~~~~~~~~~~~~~~*/
    char    *psz_Temp;
    /*~~~~~~~~~~~~~~~~~~*/

	if(_ul_File == BIG_C_InvalidIndex) return;

    switch(_i_NumCol)
    {
    /* Name */
    case 0:
        L_strcpy(_psz_Out, BIG_NameFile(_ul_File));
        psz_Temp = L_strrchr(_psz_Out, '.');
        if(psz_Temp) *psz_Temp = 0;
        break;

    /* Type */
    case 1:
        psz_Temp = L_strrchr(BIG_NameFile(_ul_File), '.');
        if(psz_Temp)
            L_strcpy(_psz_Out, psz_Temp + 1);
        else
            *_psz_Out = 0;
        break;

    /* Rank */
    case 2:
        sprintf(_psz_Out, "%d", BIG_i_IsRefInGroup(sgul_Group, BIG_FileKey(_ul_File)));
        break;

    /* Path */
    case 3:
        BIG_ComputeFullName(BIG_ParentFile(_ul_File), _psz_Out);
        break;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int CALLBACK si_CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char        asz_Temp1[BIG_C_MaxLenPath];
    char        asz_Temp2[BIG_C_MaxLenPath];
    BIG_INDEX   ul_Index1, ul_Index2;
    int         a, b;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_Index1 = BIG_ul_SearchKeyToFat(lParam1);
	if(ul_Index1 == BIG_C_InvalidIndex) return 0;
    ul_Index2 = BIG_ul_SearchKeyToFat(lParam2);
	if(ul_Index2 == BIG_C_InvalidIndex) return 0;
    s_ComputeColName(ul_Index1, lParamSort, asz_Temp1);
    s_ComputeColName(ul_Index2, lParamSort, asz_Temp2);

    switch(lParamSort)
    {
    case 0: /* Name */
    case 1: /* Type */
    case 3: /* Path */
        return L_strcmpi(asz_Temp1, asz_Temp2);

    case 2: /* Rank */
        a = L_atoi(asz_Temp1);
        b = L_atoi(asz_Temp2);
        return a < b ? -1 : a == b ? 0 : 1;
    }

    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::AddGameObjectSub(void *p)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LINK_tdst_Pointer   *pst_Pointer;
    LV_ITEM             st_ListCtrlItem;
    int                 i_Res;
    int                 i_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Size of icon */
    i_Size = 16;
    if(EDI_M_GetBrowserFrame()->mst_Ini.i_GrpDispMode == EBRO_C_ListModeIcon)
        i_Size = 32;
    else
        i_Size = 16;

    if(!p) return;
    pst_Pointer = LINK_p_SearchPointer(p);
    if(pst_Pointer)
    {
        st_ListCtrlItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
        st_ListCtrlItem.iItem = 0;
        st_ListCtrlItem.iSubItem = 0;
        st_ListCtrlItem.iImage = M_MF()->i_GetIconImage(NULL, pst_Pointer->asz_Name, i_Size);
        st_ListCtrlItem.pszText = pst_Pointer->asz_Name;
        st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
        st_ListCtrlItem.lParam = (ULONG) p;
        i_Res = InsertItem(&st_ListCtrlItem);
        st_ListCtrlItem.mask = LVIF_TEXT;
        st_ListCtrlItem.iItem = i_Res;
        st_ListCtrlItem.iSubItem = 1;
        st_ListCtrlItem.pszText = LINK_gast_StructTypes[pst_Pointer->i_Type].psz_Name;
        st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
        SetItem(&st_ListCtrlItem);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::FillGameObject(EDI_cl_BaseView *, OBJ_tdst_GameObject *pst_Object)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    AI_tdst_Instance    *pst_Instance;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Extended struct
     -------------------------------------------------------------------------------------------------------------------
     */

    if(pst_Object->pst_Extended)
    {
        pst_Instance = (AI_tdst_Instance *) pst_Object->pst_Extended->pst_Ai;
        if(pst_Instance)
        {
            AddGameObjectSub(pst_Instance->pst_Model);
            AddGameObjectSub(pst_Instance);
        }

        AddGameObjectSub(pst_Object->pst_Extended->pst_Col);
        AddGameObjectSub(pst_Object->pst_Extended->pst_Design);
        AddGameObjectSub(pst_Object->pst_Extended->pst_Group);
        AddGameObjectSub(pst_Object->pst_Extended->pst_Light);
        AddGameObjectSub(pst_Object->pst_Extended->pst_Msg);
        AddGameObjectSub(pst_Object->pst_Extended->pst_Links);
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Base struct
     -------------------------------------------------------------------------------------------------------------------
     */

    if(pst_Object->pst_Base)
    {
        AddGameObjectSub(pst_Object->pst_Base->pst_Visu);
        AddGameObjectSub(pst_Object->pst_Base->pst_Dyna);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::FillGrp(EDI_cl_BaseView *, BIG_INDEX _ul_Group)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_KEY     *pul_Buf;
    ULONG       ul_Size, i, j;
    LV_ITEM     st_ListCtrlItem;
    BIG_KEY     ul_Key;
    BIG_INDEX   ul_Index;
    char        asz_Temp[BIG_C_MaxLenPath];
    int         i_Col, i_Res;
    int         i_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Size of icon */
    i_Size = 16;
    if(EDI_M_GetBrowserFrame()->mst_Ini.i_GrpDispMode == EBRO_C_ListModeIcon)
        i_Size = 32;
    else
        i_Size = 16;

    pul_Buf = (BIG_KEY *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(_ul_Group), &ul_Size);
	if(ul_Size % 4) return;
    sgul_Group = _ul_Group;

    for(i = 0; i < (ul_Size >> 2); i += 2)
    {
        ul_Key = pul_Buf[i];
        if(ul_Key)
        {
            ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
            if(ul_Index != BIG_C_InvalidIndex)
            {
                st_ListCtrlItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
                st_ListCtrlItem.iItem = 0;
                st_ListCtrlItem.iSubItem = 0;
                st_ListCtrlItem.iImage = M_MF()->i_GetIconImage(NULL, BIG_NameFile(ul_Index), i_Size);

                s_ComputeColName(ul_Index, 0, asz_Temp);
                st_ListCtrlItem.pszText = asz_Temp;

                st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
                st_ListCtrlItem.lParam = ul_Key;
                i_Res = InsertItem(&st_ListCtrlItem);

                /* Columns */
                i_Col = 1;
                for(j = 1; j < EBRO_C_GrpCtrlMaxColumns; j++)
                {
                    if(EDI_M_GetBrowserFrame()->mst_Ini.ai_NumColGrpCtrl[j] != -1)
                    {
                        st_ListCtrlItem.mask = LVIF_TEXT;
                        st_ListCtrlItem.iItem = i_Res;
                        st_ListCtrlItem.iSubItem = i_Col++;

                        s_ComputeColName(ul_Index, j, asz_Temp);

                        st_ListCtrlItem.pszText = (LPTSTR) asz_Temp;
                        st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
                        SetItem(&st_ListCtrlItem);
                    }
                }
            }

            /* Error. Key is not here */
            else
            {
                st_ListCtrlItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
                st_ListCtrlItem.iItem = 0;
                st_ListCtrlItem.iSubItem = 0;
                st_ListCtrlItem.iImage = 0;

                sprintf(asz_Temp, "ERROR <%x>", ul_Key);
                st_ListCtrlItem.pszText = asz_Temp;

                st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
                st_ListCtrlItem.lParam = ul_Key;
                i_Res = InsertItem(&st_ListCtrlItem);
            }
        }
    }

    /* Sort items */
    SortItems(si_CompareFunc, mi_SortColumn);

    L_free(pul_Buf);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::OnColumnClick(NMHDR *pNotifyStruct2, LRESULT *result)
{
		NM_LISTVIEW *pNotifyStruct;
	pNotifyStruct = (NM_LISTVIEW *)pNotifyStruct2;

    if(!EDI_M_GetBrowserFrame()->mst_Ini.b_EngineMode)
    {
        mi_SortColumn = pNotifyStruct->iSubItem;
        SortItems(si_CompareFunc, mi_SortColumn);
    }
}

/*
 =======================================================================================================================
    Aim:    Cause display is not the same as the real name, retreive item text must retreive the
            real name of the file (in the display, the extension of the file is not in the name).
 =======================================================================================================================
 */
CString EBRO_cl_GrpCtrl::GetItemText(int _i_Item, int)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~*/

    ul_Index = BIG_ul_SearchKeyToFat(GetItemData(_i_Item));
	if(ul_Index == BIG_C_InvalidIndex) return "<INVALID>";
    return BIG_NameFile(ul_Index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::OnLButtonDown(UINT n, CPoint pt)
{
    SetFocus();
    CListCtrlStyled::OnLButtonDown(n, pt);
    EDI_M_GetBrowserFrame()->RefreshMenu();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::OnLButtonDblClk(UINT n, CPoint pt)
{
    CListCtrlStyled::OnLButtonDblClk(n, pt);
    if(HitTest(pt) != -1) EDI_M_GetBrowserFrame()->OnGrpCtrlDblClk(HitTest(pt));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::OnItemChanged(NMHDR *pNotifyStruct2, LRESULT *result)
{

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    static BOOL b_Rec = FALSE;
    POINT       pt, pt1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NM_LISTVIEW *pNotifyStruct;
	pNotifyStruct = (NM_LISTVIEW *)pNotifyStruct2;
    if(b_Rec) return;
    b_Rec = TRUE;
    if((pNotifyStruct->uNewState & LVIS_SELECTED) && !mb_LockSel)
    {
        GetCursorPos(&pt);
        ScreenToClient(&pt);

        EDI_M_GetBrowserFrame()->OnGrpCtrlItemSel(pNotifyStruct->iItem);

        GetCursorPos(&pt1);
        ScreenToClient(&pt1);
        if((pt.x != pt1.x) || (pt.y != pt1.y)) PostMessage(WM_LBUTTONUP, 0, pt.x + (pt.y << 16));
    }

    b_Rec = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_GrpCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    if(GetAsyncKeyState(VK_CONTROL) < 0)
    {
        switch(nChar)
        {
        case 'A':
            for(i = 0; i < GetItemCount(); i++) SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
            return;
        }
    }

    CListCtrlStyled::OnKeyDown(nChar, nRepCnt, nFlags);
}

#endif /* ACTIVE_EDITORS */

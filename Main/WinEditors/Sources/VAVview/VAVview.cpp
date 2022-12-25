/*$T VAVview.cpp GC!1.52 10/07/99 17:37:03 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "VAVview/VAVview.h"
#include "LINKs/LINKstruct.h"
#include "EDImainframe.h"
#include "Dialogs/DIAbase.h"
#include "Dialogs/FlatHeaderCtrl.h"

/*$2
 ---------------------------------------------------------------------------------------------------
    Array for drawing values. Index must be the same of the enum type.
 ---------------------------------------------------------------------------------------------------
 */

#define EVAV_DEFINE_TYPE(a, b, c, d)    c,
static tdpfnv_DrawValue sgapfnv_DrawArray[] =
{
#include "VAVtypes.h"
};
#undef EVAV_DEFINE_TYPE

/*$2
 ---------------------------------------------------------------------------------------------------
    Array for drawing values. Index must be the same of the enum type.
 ---------------------------------------------------------------------------------------------------
 */

#define EVAV_DEFINE_TYPE(a, b, c, d)    d,
static tdpfnv_SelectValue sgapfnv_SelectArray[] =
{
#include "VAVtypes.h"
};
#undef EVAV_DEFINE_TYPE

/*$2
 ---------------------------------------------------------------------------------------------------
    Array of edit types.
 ---------------------------------------------------------------------------------------------------
 */

#define EVAV_DEFINE_TYPE(a, b, c, d)    b,
static EVAV_tde_VarsViewItemEditType sgae_EditTypesArray[] =
{
#include "VAVtypes.h"
};
#undef EVAV_DEFINE_TYPE

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EVAV_cl_View, CWnd)
    ON_WM_SIZE()
    ON_LBN_SELCHANGE(1, OnSelChangeList)
    ON_NOTIFY(HDN_ITEMCHANGED, 2, OnHeaderNotify)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*
 ===================================================================================================
 ===================================================================================================
 */
EVAV_cl_View::EVAV_cl_View(void)
{
    mpo_ListBox = new EVAV_cl_ListBox;
    mpo_ListBox->mpo_CurrentWnd = NULL;
    mpo_ListBox->mp_Owner = NULL;
    mpo_ListBox->mpfnv_Callback = NULL;

    mpo_HeaderCtrl = new CFlatHeaderCtrl;
    mul_ID = 0;
    mb_CanDragDrop = FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EVAV_cl_View::~EVAV_cl_View(void)
{
    ResetList();
    mpo_ListBox->mo_ComboSelect.DestroyWindow();
    mpo_ListBox->mo_EditSelect.DestroyWindow();

    mpo_ListBox->DestroyWindow();
    delete mpo_ListBox;

    mpo_HeaderCtrl->DestroyWindow();
    delete mpo_HeaderCtrl;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EVAV_cl_View::OnEraseBkgnd(CDC *pDC)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DWORD   x_Col;
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    x_Col = GetSysColor(COLOR_BTNFACE);

    GetWindowRect(&o_Rect);
    ScreenToClient(&o_Rect);
    pDC->FillSolidRect(o_Rect, x_Col);

    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_View::MyCreate
(
    CWnd                        *_po_Parent,
    EVAV_tdst_VarsViewStruct    *_pst_Define,
    void                        *_p_Owner,
    int                         _i_ID
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    HD_ITEM st_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Create owner window */
    mpst_Define = _pst_Define;
    Create
    (
        NULL,
        NULL,
        WS_VISIBLE | WS_CLIPCHILDREN | WS_CHILD,
        CRect(0, 0, 100, 100),
        _po_Parent,
        _i_ID,
        NULL
    );

    /* Creation of the list box */
    mpo_ListBox->Create
        (
            WS_CLIPCHILDREN | WS_VISIBLE | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL | LBS_HASSTRINGS,
            CRect(0, 0, 100, 100),
            this,
            1
        );
    mpo_ListBox->EnableScrollBar(SB_VERT);
    mpo_ListBox->mp_Owner = _p_Owner;

    mpo_ListBox->SetFont(&(M_MF()->mo_Fnt2));
    mpo_ListBox->mb_CanDragDrop = mb_CanDragDrop;

    /* Create an header control */
    mpo_HeaderCtrl->Create
        (
            WS_VISIBLE | HDS_BUTTONS | HDS_HORZ | CCS_TOP,
            CRect(0, 0, 100, 100),
            this,
            2
        );

    mpo_HeaderCtrl->SetFont(&(M_MF()->mo_Fnt));

    /* Columns. */
    st_Item.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
    st_Item.fmt = HDF_STRING;
    st_Item.cxy = mpo_ListBox->mi_WidthCol1 = _pst_Define->i_WidthCol1;
    st_Item.pszText = _pst_Define->psz_NameCol1;
    mpo_HeaderCtrl->InsertItem(0, &st_Item);

    st_Item.pszText = _pst_Define->psz_NameCol2;
    st_Item.cxy = mpo_ListBox->mi_WidthCol1;
    mpo_HeaderCtrl->InsertItem(1, &st_Item);

    /* Update group number. */
    mpo_ListBox->mpo_ListItems = NULL;
    mpo_ListBox->SetItemList(_pst_Define->po_ListItems);

    /* Combo for selection. */
    mpo_ListBox->mo_ComboSelect.Create
        (
            CBS_SORT | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL,
            CRect(0, 0, 100, 240),
            mpo_ListBox,
            3
        );
    mpo_ListBox->mo_ComboSelect.SetFont(&(M_MF()->mo_Fnt2));

    /* Edit for selection. */
    mpo_ListBox->mo_EditSelect.Create
        (
            ES_AUTOHSCROLL | WS_BORDER,
            CRect(0, 0, 100, 100),
            mpo_ListBox,
            4
        );
    mpo_ListBox->mo_EditSelect.SetFont(&(M_MF()->mo_Fnt2));

    /* Button for selection. */
    mpo_ListBox->mo_ButtonSelect.Create
        (
            "...",
            BS_PUSHBUTTON,
            CRect(0, 0, 100, 100),
            mpo_ListBox,
            5
        );
    mpo_ListBox->mo_ButtonSelect.SetFont(&(M_MF()->mo_Fnt));

    /* Check box */
    mpo_ListBox->mo_CheckSelect.Create("", BS_AUTOCHECKBOX, CRect(0, 0, 100, 100), mpo_ListBox, 6);

	EDIA_cl_BaseDialog::SetTheme(this); 
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_View::OnSize(UINT, int cx, int cy)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    HD_LAYOUT   st_Layout;
    RECT        st_Rect;
    WINDOWPOS   st_Pos;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(::IsWindow(mpo_HeaderCtrl->m_hWnd))
    {
        /* Hide current edit window */
        if(mpo_ListBox->mpo_CurrentWnd)
        {
            mpo_ListBox->mpo_CurrentWnd->ShowWindow(SW_HIDE);
            mpo_ListBox->mpo_CurrentWnd = NULL;
        }

        /* Move header control and list box */
        st_Rect.left = 0;
        st_Rect.top = 0;
        st_Rect.right = cx - 1;
        st_Rect.bottom = cy - 1;
        st_Layout.prc = &st_Rect;
        st_Layout.pwpos = &st_Pos;
        mpo_HeaderCtrl->Layout(&st_Layout);
        mpo_HeaderCtrl->MoveWindow(st_Pos.x, st_Pos.y, st_Pos.cx, st_Pos.cy);

        /* Resize list box */
        mpo_ListBox->MoveWindow(0, st_Pos.cy, st_Pos.cx, cy - st_Pos.cy);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_View::SetColWidth(int i1, int i2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    HD_ITEM st_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    st_Item.mask = HDI_WIDTH;
    st_Item.cxy = i1;
    mpo_HeaderCtrl->SetItem(0, &st_Item);
    st_Item.cxy = i2;
    mpo_HeaderCtrl->SetItem(1, &st_Item);

    mpo_ListBox->mi_WidthCol1 = i1;
    mpst_Define->i_WidthCol1 = i1;
    mpo_ListBox->RedrawWindow();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_View::GetColWidth(int &i1, int &i2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    HD_ITEM st_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    st_Item.mask = HDI_WIDTH;
    mpo_HeaderCtrl->GetItem(0, &st_Item);
    i1 = st_Item.cxy;

    mpo_HeaderCtrl->GetItem(1, &st_Item);
    i2 = st_Item.cxy;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_View::SetItemList(CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *> *_po_ListItems)
{
    if(IsWindow(mpo_ListBox->m_hWnd))
    {
        mpo_ListBox->SetItemList(_po_ListItems);
        OnSelChangeList();
    }
}

/*
 ===================================================================================================
    Aim:    A macro to delete a list.
 ===================================================================================================
 */
void EVAV_cl_View::ResetList(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            pos;
    EVAV_cl_ViewItem    *po_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!mpo_ListBox->mpo_ListItems) return;

    pos = mpo_ListBox->mpo_ListItems->GetHeadPosition();
    while(pos)
    {
        po_Item = mpo_ListBox->mpo_ListItems->GetNext(pos);
        delete po_Item;
    }

    mpo_ListBox->mpo_ListItems->RemoveAll();
    if(IsWindow(mpo_ListBox->m_hWnd))
    {
        mpo_ListBox->ResetContent();
        mpo_ListBox->MoveWindow(0, 0, 0, 0);
    }
}

/*
 ===================================================================================================
    Aim:    To add a new item in the list.
 ===================================================================================================
 */
EVAV_cl_ViewItem *EVAV_cl_View::AddItem
(
    char                        *_psz_Name,
    EVAV_tde_VarsViewItemType   _e_Type,
    void                        *_p_Data,
    ULONG                       _ul_Flags,
    int                         _i_Param1,
    int                         _i_Param2,
    int                         _i_Param3,
    POSITION                    pos,
    int                         _i_Param4,
    int                         _i_Param5,
    int                         _i_Param6,
    int                         _i_Param7,
    void                        (*_pf_CB)(void*, void*, void*, long)
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!mpo_ListBox->mpo_ListItems) return NULL;
    po_Item = new EVAV_cl_ViewItem
        (
            _psz_Name,
            _e_Type,
            _p_Data,
            _ul_Flags,
            _i_Param1,
            _i_Param2,
            _i_Param3,
            _pf_CB,
            GetSysColor(COLOR_WINDOW),
            _i_Param4,
            _i_Param5,
            _i_Param6,
            _i_Param7
        );
    if(pos)
        mpo_ListBox->mpo_ListItems->InsertAfter(pos, po_Item);
    else
        mpo_ListBox->mpo_ListItems->AddTail(po_Item);
    return po_Item;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_View::SetChangeCallback(EVAV_tdpfnv_Change _pfn)
{
    mpo_ListBox->mpfnv_Callback = _pfn;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_View::OnHeaderNotify(NMHDR *ph2, LRESULT *lResult)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    HD_ITEM     st_Item;
    CRect       o_Rect;
    static BOOL b_Rec;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HD_NOTIFY *ph;
	ph = (HD_NOTIFY *)ph2;

    b_Rec = FALSE;
    if(b_Rec) return;
    b_Rec = TRUE;

    GetClientRect(&o_Rect);

    st_Item.mask = HDI_WIDTH;
    mpo_HeaderCtrl->GetItem(0, &st_Item);
    mpo_ListBox->mi_WidthCol1 = st_Item.cxy;

    /* Remember width in user structure */
    mpst_Define->i_WidthCol1 = st_Item.cxy;

    mpo_ListBox->RedrawWindow();

    b_Rec = FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_View::UnExpandAll(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            pos;
    EVAV_cl_ViewItem    *po_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!mpo_ListBox->mpo_ListItems) return;

    pos = mpo_ListBox->mpo_ListItems->GetHeadPosition();
    while(pos)
    {
        po_Item = mpo_ListBox->mpo_ListItems->GetNext(pos);
		if(po_Item->mb_Expand && po_Item->mi_Indent > 1)
			mpo_ListBox->OnExpand(po_Item->mi_ID + WM_USER);
    }
    mpo_ListBox->SetItemList(mpo_ListBox->mpo_ListItems);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_View::OnSelChangeList(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    mpo_ListBox->Invalidate();

    /*
     * Force resize, cause if there was no entry before drag & drop, the list box was forced to
     * be empty size
     */
    GetClientRect(&o_Rect);
    OnSize(0, o_Rect.right, o_Rect.bottom + 1);
    OnSize(0, o_Rect.right, o_Rect.bottom);
}

/*$4
 ***************************************************************************************************
    Custom constrol
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_Custom_DrawItem(EVAV_cl_ViewItem *, void *)
{
    return "";
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_Custom_FillSelect(CWnd *, EVAV_cl_ViewItem *, void *, BOOL)
{
}

#endif /* ACTIVE_EDITORS */

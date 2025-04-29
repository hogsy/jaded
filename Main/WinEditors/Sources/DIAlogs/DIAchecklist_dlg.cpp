/*$T DIAchecklist_dlg.cpp GC!1.52 11/12/99 11:32:08 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAchecklist_dlg.h"
#include "Res/Res.h"
#include "EDItors/Sources/PROperties/PROstrings.h"
#include "EDImainframe.h"
#include "EDIapp.h"

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_CheckListDialog, EDIA_cl_BaseDialog)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_GETMINMAXINFO()
    ON_WM_NCLBUTTONDBLCLK()
END_MESSAGE_MAP()

/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_CheckListDialog::EDIA_cl_CheckListDialog
(
    char                *_psz_Title,
    CPoint              _o_Pos,
    EDI_cl_BaseFrame    *_po_Owner,
    BOOL                _b_CanFree
) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_CHECKLIST)
{
    mb_Minimize = FALSE;
    mpsz_Title = _psz_Title;
    mb_CanFree = _b_CanFree;
    mo_InitPos = _o_Pos;

    mpo_VarsView = new EVAV_cl_View;

    mpl = NULL;
    mst_VarsViewStruct.po_ListItems = &mo_ListItems;
    mpo_VarsView->mpo_ListBox->mpo_ListItems = &mo_ListItems;
    mst_VarsViewStruct.psz_NameCol1 = EPRO_STR_Csz_Name;
    mst_VarsViewStruct.psz_NameCol2 = EPRO_STR_Csz_Value;
    mpo_VarsView->mb_CanDragDrop = FALSE;
    mpo_Owner = _po_Owner;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *> *EDIA_cl_CheckListDialog::GetList(void)
{
    mpl = new CList < EVAV_cl_ViewItem *, EVAV_cl_ViewItem * > ;
    mst_VarsViewStruct.po_ListItems = mpl;
    mpo_VarsView->mpo_ListBox->mpo_ListItems = mpl;
    return mpl;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_CheckListDialog::~EDIA_cl_CheckListDialog(void)
{
    if(mb_CanFree) delete mpo_VarsView;
    if(mpl) delete mpl;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_CheckListDialog::OnInitDialog(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BOOL    bRes;
    CRect   o_Rect;
    int     cx, cy, imax;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    bRes = EDIA_cl_BaseDialog::OnInitDialog();

    /* Init position and size */
    cx = GetSystemMetrics(SM_CXSCREEN);
    cy = GetSystemMetrics(SM_CYSCREEN);
    if((mo_InitPos.x == 0) && (mo_InitPos.y == 0))
        GetCursorPos(&mo_InitPos);

    o_Rect.left = mo_InitPos.x;
    o_Rect.top = mo_InitPos.y;

    /* Process pos and size */
    o_Rect.right = o_Rect.left + (cx / 3);
    if(o_Rect.right >= cx)
    {
        o_Rect.left = cx - (cx / 3) - 1;
        o_Rect.right = cx - 1;
    }

    /* Create var view */
    mst_VarsViewStruct.i_WidthCol1 = o_Rect.Width() / 2;
    mpo_VarsView->MyCreate(this, &mst_VarsViewStruct, mpo_Owner);

    /* Height */
    imax = i_MaxSize();
    if(imax < cy / 4) imax = cy / 4;
    if(imax > cy / 3) imax = cy / 3;
    o_Rect.bottom = o_Rect.top + imax;
    if(o_Rect.bottom >= cy)
    {
        o_Rect.top = cy - imax - 1;
        o_Rect.bottom = cy - 1;
    }

    /* Max size */
    SetWindowText(mpsz_Title);
    MoveWindow(o_Rect);
    GetClientRect(&o_Rect);
    mpo_VarsView->MoveWindow(o_Rect);

    return bRes;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_CheckListDialog::OnCancel(void)
{
    EDIA_cl_BaseDialog::OnCancel();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_CheckListDialog::OnOK(void)
{
    mpo_VarsView->SendMessage(WM_CHAR, VK_RETURN, 0);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int EDIA_cl_CheckListDialog::i_MaxSize(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int     i_Count;
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(mpo_VarsView && IsWindow(mpo_VarsView->m_hWnd))
    {
        i_Count = mpo_VarsView->mpo_ListBox->GetCount();
        mpo_VarsView->mpo_ListBox->GetItemRect(0, &o_Rect);
        return 36 + ((o_Rect.bottom + 1) * i_Count);
    }

    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_CheckListDialog::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
    CDialog::OnGetMinMaxInfo(lpMMI);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_CheckListDialog::OnSize(UINT n, int cx, int cy)
{
    EDIA_cl_BaseDialog::OnSize(n, cx, cy);

    M_MF()->LockDisplay(this);
    if(mpo_VarsView && IsWindow(mpo_VarsView->m_hWnd))
        mpo_VarsView->MoveWindow(0, 0, cx, cy);
    M_MF()->UnlockDisplay(this);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_CheckListDialog::OnNcLButtonDblClk(UINT n, CPoint pt)
{
	EDIA_cl_BaseDialog::OnNcLButtonDblClk(n, pt);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_CheckListDialog::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
}

/*
 ===================================================================================================
    Aim:    To add a new item in the list.
 ===================================================================================================
 */
EVAV_cl_ViewItem *EDIA_cl_CheckListDialog::AddItem
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
    int                         _i_Param7
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_Item = new EVAV_cl_ViewItem
        (
            _psz_Name,
            _e_Type,
            _p_Data,
            _ul_Flags,
            _i_Param1,
            _i_Param2,
            _i_Param3,
            NULL,
            GetSysColor(COLOR_WINDOW),
            _i_Param4,
            _i_Param5,
            _i_Param6,
            _i_Param7
        );
    if(pos)
        mo_ListItems.InsertAfter(pos, po_Item);
    else
        mo_ListItems.AddTail(po_Item);
    return po_Item;
}

// Returned value : bit 1 means "copy RLI", bit 2 means "copy SPG2"
ULONG F3D_ulGetCopyMask(BOOL _bCopySPG2)
{
    int i,iTypeNb;
    ULONG ulReturn = 0;
    static int	aiTypes[3];
    static char *asNameConst[2] = { "copy rli", "copy spg2"};

    EDIA_cl_CheckListDialog o_Dialog(_bCopySPG2 ? "Do you want to copy rli and/or SPG2 ?" : "Do you want to copy rli ?", CPoint(0, 0));
    iTypeNb = (_bCopySPG2 ? 2 : 1);

    for(i = 0; i < iTypeNb; i++)
    {
        aiTypes[i] = 1;
        o_Dialog.AddItem(asNameConst[i] , EVAV_EVVIT_Bool, &aiTypes[i]);
    }
    aiTypes[iTypeNb] = 0;

    o_Dialog.DoModal();

    if (aiTypes[0])
        ulReturn |= 1;
    if (aiTypes[1])
        ulReturn |= 2;

	return ulReturn ;
}

#endif /* ACTIVE_EDITORS */

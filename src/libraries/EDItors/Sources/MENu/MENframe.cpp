/*$T MENframe.cpp GC!1.28 05/04/99 17:00:02 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"

#define ACTION_GLOBAL

#include "MENframe.h"
#include "MENmenu.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"
#include "EDImsg.h"

/*$4
 ***************************************************************************************************
    GLOBAL VARS
 ***************************************************************************************************
 */

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EMEN_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EMEN_cl_Frame, EDI_cl_BaseFrame)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
END_MESSAGE_MAP()

/*$4
 ***************************************************************************************************
    CONSTRUCT
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
EMEN_cl_Frame::EMEN_cl_Frame(void)
{
    mpo_CurrentExpand = NULL;
	mb_ChangePane = FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EMEN_cl_Frame::~EMEN_cl_Frame(void)
{
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int EMEN_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
    if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1) return -1;
    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_Frame::OnSize(UINT n, int cx, int cy)
{
    EDI_cl_BaseFrame::OnSize(n, cx, cy);
    if(!mo_List.GetCount()) return;
    NcClick(mpo_CurrentExpand);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_Frame::OnDestroy(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION        pos;
    EMEN_cl_FrameIn *po_Left;
    EMEN_cl_Frame   *po_Main;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_Main = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, 0);
    if(!po_Main) return;

    pos = mo_List.GetHeadPosition();
    while(pos)
    {
        po_Left = mo_List.GetNext(pos);
        if((IsWindow(po_Left->m_hWnd)) && (IsWindow(po_Main->m_hWnd)))
            MoveMenu(po_Left, po_Main);
    }
}

/*
 ===================================================================================================
    Aim:    Call to add a frame menu in editor.

    In:     _psz_Name   Name of the menu (that will appeared in caption).
            _po_List    The action list linked to the menu.
            _po_Editor  The editor to send actions. Can be NULL for main frame.
            _i_Index    The index in the list of frame. It's the priority.
            _b_Sel      TRUE to force the added menu to be expand. FALSE else. 

    Out:    A pointer on the created frame menu.
 ===================================================================================================
 */
EMEN_cl_FrameIn *EMEN_cl_Frame::po_AddMenu
(
    char                *_psz_Name,
    EDI_cl_ActionList   *_po_List,
    EDI_cl_BaseFrame    *_po_Editor,
    int                 _i_Index,
    BOOL                _b_Sel
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION        pos, pos1;
    EMEN_cl_FrameIn *po_Left, *po_Next;
    CWnd            *mpo_Parent;
    char            c_ForceExpand;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    c_ForceExpand = 0;

    if(mb_IsActivate)
        M_MF()->LockDisplay(this);

    /*
     * Search a menu with the same action list. If we found one (that have the same indent), do
     * nothing. If we found one with a different index, we delete it cause we will create a new
     * frame at the new given position.
     */
    pos = mo_List.GetHeadPosition();
    while(pos)
    {
        po_Left = mo_List.GetNext(pos);
        if(po_Left->mpo_Actions == _po_List)
        {
            if(po_Left->mi_Num == _i_Index)
            {
                if(mb_IsActivate)
                    M_MF()->UnlockDisplay(this);
                return po_Left;
            }

            RemoveMenu(po_Left);
            break;
        }
    }

    /*
     * Search an existing menu with the same index. If we found it, we delete it cause it will be
     * replaced by the new created menu.
     */
    pos = mo_List.GetHeadPosition();
    while(pos)
    {
        po_Left = mo_List.GetNext(pos);
        if(po_Left->mi_Num == _i_Index)
        {
            if(mpo_CurrentExpand == po_Left) c_ForceExpand = 1;
            RemoveMenu(po_Left);
            break;
        }
    }

    /*
     * Create a new menu frame £
     */
    po_Left = new EMEN_cl_FrameIn(_po_List, _po_Editor);
    po_Left->mi_Num = _i_Index;
    po_Left->mpo_Parent = this;

    /*
     * Search position to insert the menu. Each frame menu if the child of the frame menu with
     * the index just below. So we need to get the next frame menu (the one with the index above)
     * to change its parent
     */
    pos = NULL;
    po_Next = NULL;
    mpo_Parent = (CWnd *) this;
    if(mo_List.GetCount())
    {
        pos = mo_List.GetHeadPosition();
        while(pos)
        {
            pos1 = pos;
            po_Next = mo_List.GetNext(pos);
            if(po_Next->mi_Num >= _i_Index)
            {
                pos = pos1;
                mo_List.GetPrev(pos1);
                if(pos1)
                    mpo_Parent = (CWnd *) mo_List.GetAt(pos1);
                break;
            }
        }

        if(!pos)
        {
            po_Next = NULL;
            mpo_Parent = mo_List.GetTail();
        }
    }

    po_Left->CreateEx
        (
            WS_EX_TOOLWINDOW,
            NULL,
            _psz_Name,
            WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE,
            CRect(0, 0, 0, 0),
            mpo_Parent,
            5,  /* Used in MEN_cl_FrameIn to retreive child */
            NULL
        );
    po_Left->SetFont(&M_MF()->mo_Fnt);

    /* Set parent of next window as current one */
    if(po_Next)
        po_Next->SetParent(po_Left);

    /* Insert new frame at the right position in list */
    if(!pos)
        mo_List.AddTail(po_Left);
    else
        mo_List.InsertBefore(pos, po_Left);

    /*
     * Maximize the frame if c_ForceExpand is TRUE. c_ForceExpand is TRUE if the newly created
     * frame replace another one that is expanded.
     */
    if(c_ForceExpand)
        mpo_CurrentExpand = po_Left;

    /*
     * Need to select created frame ? We select it only if current selected menu is an editor
     * one, and not global one
     */
    else if(_b_Sel && !mpo_CurrentExpand)
        mpo_CurrentExpand = po_Left;
    else if(mo_List.GetCount() == 1)
        mpo_CurrentExpand = po_Left;
    else if(_b_Sel && mpo_CurrentExpand->mpo_Actions->mpo_Editor && !mpo_CurrentExpand->mb_Locked)
        mpo_CurrentExpand = po_Left;

    /* Force new frame to be fill */
    if(mpo_CurrentExpand != po_Left) po_Left->FillList();

    /* Select the added frame ? */
    NcClick(mpo_CurrentExpand);

    if(mb_IsActivate)
        M_MF()->UnlockDisplay(this);
    return po_Left;
}

/*
 ===================================================================================================
    Aim:    Call to remove a menu frame.

    In:     _po_Frame   The menu frame to remove.
 ===================================================================================================
 */
void EMEN_cl_Frame::RemoveMenu(EMEN_cl_FrameIn *_po_Frame)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION        pos, opos;
    EMEN_cl_FrameIn *po_Left, *po_Next;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(mb_IsActivate)
        M_MF()->LockDisplay(this);

    /* Search the frame */
    pos = mo_List.GetHeadPosition();
    while(pos)
    {
        opos = pos;
        po_Left = mo_List.GetNext(pos);
        if(po_Left == _po_Frame)
        {
            /* Change parent of child */
            if(pos)
            {
                po_Next = mo_List.GetNext(pos);
                po_Next->SetParent(po_Left->GetParent());
            }
            else
            {
                pos = opos;
                mo_List.GetPrev(pos);
                if(pos)
                    po_Next = mo_List.GetAt(pos);
                else
                    po_Next = NULL;
            }

            /* Delete frame */
            if(mpo_CurrentExpand == po_Left)
                mpo_CurrentExpand = po_Next;

            /* Delete reference in depending editor (or mainframe) */
            if(po_Left->mpo_Editor)
                po_Left->mpo_Editor->mpo_MenuFrame = NULL;
            else
                M_MF()->mpo_MenuFrame = NULL;

            /* Destroy */
            mo_List.RemoveAt(opos);
            po_Left->DestroyWindow();

            /* Expand new */
            if(!mb_ChangePane) NcClick(mpo_CurrentExpand);
            break;
        }
    }

    if(mb_IsActivate)
        M_MF()->UnlockDisplay(this);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_Frame::MoveMenu(EMEN_cl_FrameIn *_po_Source, EMEN_cl_Frame *_po_DestEd)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EMEN_cl_FrameIn     *po_Menu;
    CString             o_String;
    EDI_cl_BaseFrame    *po_Editor;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Create copy of the menu */
    _po_Source->GetWindowText(o_String);
    po_Menu = _po_DestEd->po_AddMenu
        (
            (char *) (LPCSTR) o_String,
            _po_Source->mpo_Actions,
            _po_Source->mpo_Editor,
            _po_Source->mi_Num,
            TRUE
        );

    /* Remove menu of source */
    po_Editor = _po_Source->mpo_Editor;
	if(!po_Editor) return;
    if(!IsWindow(po_Editor->m_hWnd)) return;
    _po_Source->mpo_Parent->RemoveMenu(_po_Source);

    /* Set num menu of editor or MF (depending on menu owner) */
    if(po_Editor)
    {
        po_Editor->mst_BaseIni.i_CountMenu = _po_DestEd->mi_NumEdit;
        po_Editor->mpo_MenuFrame = po_Menu;
        if(po_Editor->mpo_Menu)
            po_Editor->mpo_Menu->mpo_MenuFrame = po_Menu;
    }
    else
    {
        M_MF()->mst_Ini.i_CountMenu = _po_DestEd->mi_NumEdit;
        M_MF()->mpo_MenuFrame = po_Menu;
        if(M_MF()->mpo_Menu)
            M_MF()->mpo_Menu->mpo_MenuFrame = po_Menu;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_Frame::NcClick(EMEN_cl_FrameIn *_po_Frame)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION        pos;
    EMEN_cl_FrameIn *po_Left;
    CRect           o_Rect, o_Rect1, mo_Size;
    char            top;
    int             i1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_po_Frame) return;
    if(mpo_CurrentExpand && mpo_CurrentExpand != _po_Frame && mpo_CurrentExpand->mb_Locked) return;

    if(mb_IsActivate)
        M_MF()->LockDisplay(this);

    /* Remember expanded frame */
    mpo_CurrentExpand = _po_Frame;
	mpo_CurrentExpand->SaveIni();

    /* Compute position of each frame */
    pos = mo_List.GetHeadPosition();
    top = 1;
    i1 = 0;
    while(pos)
    {
        po_Left = mo_List.GetNext(pos);
        if(po_Left == _po_Frame)
        {
            po_Left->MoveWindow(0, 0, 0, 0);
            if(!pos) break;

            i1 = mo_List.GetCount() - i1 - 1;
            if(i1)
            {
                i1 *= SIZE_CAPTIONY;

                GetWindowRect(&o_Rect);
                po_Left->ScreenToClient(&o_Rect);

                /* Move first child (next window in list. Force to be >= 0 */
                po_Left = mo_List.GetNext(pos);
                i1 = o_Rect.bottom - i1 - 2;
                if(i1 < 0) i1 = 0;

                po_Left->MoveWindow(0, i1, 0, 0);
            }
        }
        else
        {
            po_Left->MoveWindow(0, 0, 0, 0);
        }

        i1++;
    }

    /* Force resize frames */
    GetClientRect(&mo_Size);
    pos = mo_List.GetHeadPosition();
    while(pos)
    {
        po_Left = mo_List.GetNext(pos);
        po_Left->GetParent()->GetClientRect(&o_Rect);

        po_Left->GetWindowRect(&o_Rect1);
        po_Left->GetParent()->ScreenToClient(&o_Rect1);
        po_Left->MoveWindow(0, o_Rect1.top, mo_Size.right, o_Rect.bottom - o_Rect.top + 2);
    }

    /* Redraw all */
    mpo_CurrentExpand->FillList();
    if(mb_IsActivate)
        M_MF()->UnlockDisplay(this);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_Frame::Refresh(void)
{
    if(mpo_CurrentExpand)
        mpo_CurrentExpand->FillList();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EMEN_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
    return M_MF()->b_EditKey(_uw_Key);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_Frame::SaveIni(void)
{
    POSITION        pos;
    EMEN_cl_FrameIn *po_Left;

    pos = mo_List.GetHeadPosition();
    while(pos)
    {
        po_Left = mo_List.GetNext(pos);
		po_Left->SaveIni();
	}
}

#endif /* ACTIVE_EDITORS */

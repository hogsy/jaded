/*$T EDImainframe_dd.cpp GC!1.60 12/24/99 12:02:41 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "EDImsg.h"
#include "EDIerrid.h"
#include "EDImainframe.h"
#include "EDIbaseframe.h"
#include "Res/Res.h"
#include "ENGine/Sources/ENGinit.h"

/*$4
 ***********************************************************************************************************************
    Global variables
 ***********************************************************************************************************************
 */

EDI_tdst_DragDrop   EDI_gst_DragDrop;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::BeginDragDrop(CPoint _o_Pt, CWnd *_po_Mse, EDI_cl_BaseFrame *_po_Src, int _i_Type)
{
    /* Window that will receive mouse focus */
    EDI_gst_DragDrop.po_CaptureWnd = _po_Mse;
    EDI_gst_DragDrop.po_CaptureWnd->SetCapture();

    /* The source editor that began the d&d operation */
    EDI_gst_DragDrop.po_SourceEditor = _po_Src;
    EDI_gst_DragDrop.po_CaptureWnd->ClientToScreen(&_o_Pt);
    EDI_gst_DragDrop.o_Pt = _o_Pt;

    /* Compute the rect to really begin the d&d operation */
    EDI_gst_DragDrop.o_OutRect = CRect(_o_Pt.x - 8, _o_Pt.y - 8, _o_Pt.x + 8, _o_Pt.y + 8);

    /* Other fields */
    EDI_gst_DragDrop.i_Type = _i_Type;
    EDI_gst_DragDrop.b_CanDragDrop = FALSE;
    EDI_gst_DragDrop.b_BeginDragDrop = TRUE;
    EDI_gst_DragDrop.po_FocusEditor = NULL;
    EDI_gst_DragDrop.po_DestEditor = NULL;
	EDI_gst_DragDrop.b_ForceCopy = FALSE;
}

/*
 =======================================================================================================================
    Aim:    Must be called each time mouse is moving during a drag & drop process.
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::b_MoveDragDrop(CPoint _o_Pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_cl_BaseView *po_View;
    BOOL            b_Res;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* No darg & drop operation for now... */
    if(!EDI_gst_DragDrop.b_BeginDragDrop) return FALSE;

    /*
     * Determin if the mouse hase move out a given rect (else no
     * d&d yet
     */
    EDI_gst_DragDrop.b_CanDragDrop = FALSE;
    if(EDI_gst_DragDrop.po_CaptureWnd)
        EDI_gst_DragDrop.po_CaptureWnd->ClientToScreen(&_o_Pt);
    if(EDI_gst_DragDrop.o_OutRect.PtInRect(_o_Pt)) return FALSE;
    EDI_gst_DragDrop.o_OutRect.SetRectEmpty();

    /* No source editor, the d&d is not for editors */
    if(EDI_gst_DragDrop.po_SourceEditor == NULL)
    {
        EDI_gst_DragDrop.b_CanDragDrop = TRUE;
        return TRUE;
    }

    /* Determin what is the editor under mouse */
    EDI_gst_DragDrop.po_DestEditor = M_MF()->po_EditorUnderPoint(_o_Pt, &po_View);

    /* Focus for drag and drop. Inform new and last editor */
    if(EDI_gst_DragDrop.po_FocusEditor != EDI_gst_DragDrop.po_DestEditor)
    {
        if(EDI_gst_DragDrop.po_FocusEditor)
        {
            EDI_gst_DragDrop.po_FocusEditor->i_OnMessage((ULONG) EDI_MESSAGE_LOOSEFOCUSDRAGDROP, 0, 0);
        }

        if(EDI_gst_DragDrop.po_DestEditor)
        {
            EDI_gst_DragDrop.po_DestEditor->i_OnMessage((ULONG) EDI_MESSAGE_GAINFOCUSDRAGDROP, 0, 0);
        }

        EDI_gst_DragDrop.po_FocusEditor = EDI_gst_DragDrop.po_DestEditor;
    }

    /*
     * If we have found an editor, ask it if it can accept the
     * data
     */
    if(EDI_gst_DragDrop.po_DestEditor)
    {
        EDI_gst_DragDrop.o_Pt = _o_Pt;

        /* Default cursor */
        if(GetKeyState(VK_CONTROL) < 0)
        {
            ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGCOPY));
			EDI_gst_DragDrop.b_Copy = TRUE;
        }
        else if(EDI_gst_DragDrop.i_Type == EDI_DD_Long)
        {
            ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGCOPY1));
            EDI_gst_DragDrop.b_Copy = TRUE;
        }
        else
        {
            ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
            EDI_gst_DragDrop.b_Copy = FALSE;
        }

_Try_
        b_Res = EDI_gst_DragDrop.po_DestEditor->i_OnMessage
            (
                (ULONG) EDI_MESSAGE_CANDRAGDROP,
                (ULONG) & EDI_gst_DragDrop,
                0
            );
_Catch_
        b_Res = FALSE;
_End_
        if(b_Res)
        {
            po_View->IWantToBeActive(EDI_gst_DragDrop.po_DestEditor);
            EDI_gst_DragDrop.b_CanDragDrop = TRUE;
            return TRUE;
        }
    }

    /* Invalid drag and drop operation */
    ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGNONE));
    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::EndDragDrop(CPoint _o_Pt)
{
    if(EDI_gst_DragDrop.b_BeginDragDrop)
    {
        EDI_gst_DragDrop.b_BeginDragDrop = FALSE;

        ReleaseCapture();
        if(EDI_gst_DragDrop.b_CanDragDrop)
        {
			if(EDI_gst_DragDrop.po_CaptureWnd)
				EDI_gst_DragDrop.po_CaptureWnd->ClientToScreen(&_o_Pt);
            EDI_gst_DragDrop.o_Pt = _o_Pt;

            /* Send a message to destination editor */
            if(EDI_gst_DragDrop.po_DestEditor)
            {
                EDI_gst_DragDrop.po_DestEditor->i_OnMessage
                    (
                        (ULONG) EDI_MESSAGE_ENDDRAGDROP,
                        (ULONG) & EDI_gst_DragDrop,
                        0
                    );
            }
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::CancelDragDrop(void)
{
    if(EDI_gst_DragDrop.b_BeginDragDrop)
    {
        ReleaseCapture();
        EDI_gst_DragDrop.b_BeginDragDrop = FALSE;

        if(EDI_gst_DragDrop.po_DestEditor)
        {
            EDI_gst_DragDrop.po_DestEditor->i_OnMessage((ULONG) EDI_MESSAGE_CANCELDRAGDROP, 0, 0);
        }
    }
}

#endif /* ACTIVE_EDITORS */

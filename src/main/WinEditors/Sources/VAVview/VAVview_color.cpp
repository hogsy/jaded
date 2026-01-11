/*$T VAVview_color.cpp GC!1.52 12/02/99 15:28:09 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "VAVview/VAVview.h"
#include "LINKs/LINKtoed.h"
#include "EDIapp.h"
#include "DIAlogs/DIACOLOR_dlg.h"
#include "EDImainframe.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_Color_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COLORREF    col;
    char        asz_Temp[128];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    col = *((COLORREF *) _p_Value);
    L_ltoa(col, asz_Temp, 10);
    return asz_Temp;
}

static CColorDialog *gpo_ColorDlg;
static COLORREF     *gpx_ColVal;
static BOOL         gb_CanColor;
static HWND         gh_Static;

/*
 ===================================================================================================
 ===================================================================================================
 */
void HookProc(ULONG, ULONG)
{
	LINK_Refresh();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_Color_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDIA_cl_ColorDialog *po_Dial;
	CString				o_Title;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_Title = "Pick a color for " + CString(_po_Data->masz_Name);
    po_Dial = new EDIA_cl_ColorDialog((char *) (LPCSTR) o_Title, (ULONG *) _p_Value, HookProc, 0, 0);
    po_Dial->DoModeless();
}

#endif /* ACTIVE_EDITORS */

/*$T AIview_scan.cpp GC!1.52 11/16/99 09:54:35 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "../AIframe.h"
#include "AIview.h"
#include "AIleftview.h"
#include "DIAlogs/DIAcompletion_dlg.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
LONG EAI_cl_View::l_FirstVisibleLine(void)
{
    return GetFirstVisibleLine();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
LONG EAI_cl_View::l_LastVisibleLine(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CDC     *pDC;
    CSize   o_Size;
    CRect   o_Rect;
    LONG    l_Return;
    CFont   *po_OldFnt;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pDC = GetDC();
    po_OldFnt = pDC->SelectObject(mpo_Fnt);
    o_Size = pDC->GetOutputTextExtent("X");
    GetClientRect(o_Rect);
    l_Return = ((o_Rect.bottom - o_Rect.top) / o_Size.cy);
    pDC->SelectObject(po_OldFnt);
    ReleaseDC(pDC);

    l_Return += l_FirstVisibleLine();
    if(l_Return >= GetLineCount())
        l_Return = GetLineCount() - 1;
    if(l_Return < 0) l_Return = 0;

    return l_Return;
}

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EAI_cl_View::o_GetWordLeft(LONG _l_Sel)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    l_Beg;
    CString o_String;
    int     i;
    CString o_Text;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GetWindowText(o_Text);

    l_Beg = LineIndex(LineFromChar(_l_Sel));
    while(l_Beg < _l_Sel)
	{
        o_String += ((char *) (LPCSTR) o_Text)[l_Beg];
		l_Beg++;
	}

    for(i = o_String.GetLength() - 1; (i >= 0) && (L_isalnum(o_String[i]) || o_String[i] == '.' || o_String[i] == '/' || o_String[i] == '\\'); i--) ;
    o_String = o_String.Right(o_String.GetLength() - i - 1);

    return o_String;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EAI_cl_View::o_GetWordRight(LONG _l_Sel)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    l_End;
    CString o_Text;
    CString o_String;
    int     i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GetWindowText(o_Text);

    l_End = LineIndex(LineFromChar(_l_Sel)) + LineLength(_l_Sel);
	if(_l_Sel && ((char *) (LPCSTR) o_Text)[_l_Sel - 1] == ' ') return "";

    o_String += ((char *) (LPCSTR) o_Text)[_l_Sel];
	_l_Sel++;

    while(_l_Sel < l_End)
	{
        o_String += ((char *) (LPCSTR) o_Text)[_l_Sel];
		_l_Sel ++;
	}

    for(i = 0; i < o_String.GetLength() && (L_isalnum(o_String[i]) || /*o_String[i] == '.' ||*/ o_String[i] == '/' || o_String[i] == '\\'); i++) ;
    o_String = o_String.Left(i);

    return o_String;
}

#endif /* ACTIVE_EDITORS */

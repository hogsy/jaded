/*$T AIview_colors.cpp GC!1.71 01/19/00 09:42:03 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "AIview.h"
#include "../AIframe.h"
#include "EDImainframe.h"
#include "EDIpaths.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::ResetDefaultFormat(BOOL _b_UpdateCol)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CHARFORMAT  st_Format;
    LONG        l_Start, l_End;
    BOOL        b_Modified;
    int         i_Tab;
    LOGFONT     *plf;
    PARAFORMAT  pf;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    SetEventMask(0);

    LockWindowUpdate();
    HideSelection(TRUE, FALSE);

    /* Create font. */
    plf = (LOGFONT *) LocalAlloc(LPTR, sizeof(LOGFONT));
    lstrcpy(plf->lfFaceName, mpo_Frame->mst_Ini.asz_FaceName);
    plf->lfHeight = mpo_Frame->mst_Ini.i_FontSize;
    plf->lfEscapement = 0;
    if(mpo_Fnt) mpo_Fnt->DeleteObject();
    mpo_Fnt->CreateFontIndirect(plf);
    LocalFree((LOCALHANDLE) plf);
    SetFont(mpo_Fnt);

    /* Save modified state */
    b_Modified = GetModify();

    /* Save selection state */
    GetSel(l_Start, l_End);

    /* Select all the text */
    if(mpo_Frame->mul_CurrentEditFile != BIG_C_InvalidIndex)
        SetSel(0, -1);
    GetSelectionCharFormat(st_Format);

    st_Format.dwMask = CFM_SIZE | CFM_PROTECTED;
    if(_b_UpdateCol) st_Format.dwMask |= CFM_COLOR;
    st_Format.dwEffects = CFE_PROTECTED;

    /* Color */
    st_Format.crTextColor = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorDefault];

    /* Reset format */
    if(mpo_Frame->mul_CurrentEditFile != BIG_C_InvalidIndex)
        SetSelectionCharFormat(st_Format);
    SetDefaultCharFormat(st_Format);

    /* Tab stops */
    pf.cbSize = sizeof(PARAFORMAT);
    pf.dwMask = PFM_TABSTOPS;
    pf.cTabCount = MAX_TAB_STOPS;
    for(i_Tab = 0; i_Tab < pf.cTabCount; i_Tab++)
        pf.rgxTabs[i_Tab] = (i_Tab + 1) * mpo_Frame->mst_Ini.i_TabStop * mpo_Frame->mst_Ini.i_FontSize * 4;
    SetParaFormat(pf);

    /* Set background color */
    SetBackgroundColor(FALSE, mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorBkGnd]);

    /* Restore sel */
    if(mpo_Frame->mul_CurrentEditFile != BIG_C_InvalidIndex)
        SetSel(l_Start, l_End);

    /* Restore modified state */
    SetModify(b_Modified);

    UnlockWindowUpdate();
    HideSelection(FALSE, FALSE);

    SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_PROTECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::ColorText(LONG _l_Beg, LONG _l_End, COLORREF _x_Color)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    CHARFORMAT  st_Format;
    BOOL        b_Modified;
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Modified = GetModify();

    /* Change color if necessary */
    SetSel(_l_Beg, _l_End);
    GetSelectionCharFormat(st_Format);

    if(!(st_Format.dwMask & CFM_COLOR) || (st_Format.crTextColor != _x_Color))
    {
        st_Format.dwMask = CFM_COLOR | CFM_BOLD | CFM_SIZE | CFM_PROTECTED;
        st_Format.dwEffects = CFE_PROTECTED;
        st_Format.crTextColor = _x_Color;
        SetSelectionCharFormat(st_Format);
    }

    /* Restore modified state */
    SetModify(b_Modified);
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern CMapStringToString   go_PPConstants;
extern CMapStringToPtr      go_PPMacros;

/*
 =======================================================================================================================
    Aim:    Search if a word is in a special AI list.
 =======================================================================================================================
 */
COLORREF EAI_cl_View::x_GetColorWord(char *_psz_Word)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
    int         i_Index;
    char        *psz_Temp;
    COLORREF    x_Col;
	CString		o_Value;
    /*~~~~~~~~~~~~~~~~~~~~~~*/

    x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorDefault];

    psz_Temp = L_strdup(_psz_Word);
    if(psz_Temp)
    {
        /* Function */
        if(mpo_Frame->mo_FunctionList.Lookup(psz_Temp, (void * &) i_Index))
            x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorFunction];

        /* Keyword */
        else if(mpo_Frame->mo_KeywordList.Lookup(psz_Temp, (void * &) i_Index))
            x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorKeyword];

        /* Type */
        else if(mpo_Frame->mo_TypeList.Lookup(psz_Temp, (void * &) i_Index))
            x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorType];

        /* Constant */
        else if(mpo_Frame->mo_ConstantList.Lookup(psz_Temp, (void * &) i_Index))
            x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorConstant];

        /* Field */
        else if(mpo_Frame->mo_FieldList.Lookup(psz_Temp, (void * &) i_Index))
            x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorField];

        /* PP */
        else if(mpo_Frame->mo_PPList.Lookup(psz_Temp, (void * &) i_Index))
            x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorPP];

        /* Reserved */
        else if(mpo_Frame->mo_ReservedList.Lookup(psz_Temp, (void * &) i_Index))
            x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorReserved];

        /* Constants */
        else if(go_PPConstants.Lookup(psz_Temp, o_Value))
            x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorPPConstants];

        /* Macros */
        else if(go_PPMacros.Lookup(psz_Temp, (void * &) i_Index))
            x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorPPMacros];

        /* Procedure */
        else if(mpo_Frame->mo_Compiler.mo_ProcList.Lookup(psz_Temp, (void * &) i_Index))
            x_Col = mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorProcedures];
        L_free(psz_Temp);
    }

    return x_Col;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::ColorCppComment(char **_psz_Text, LONG &_l_Beg, LONG &_l_TmpEnd, LONG &_l_End)
{
    /*~~~~~~~~~~~~~~~~~~*/
    char    *psz_Text;
    /*~~~~~~~~~~~~~~~~~~*/

    psz_Text = *_psz_Text;
    if((psz_Text[0] == '/') && (psz_Text[1] == '/'))
    {
        /* Search the end of line */
        while((_l_TmpEnd < _l_End) && (*psz_Text != '\r'))
        {
            _l_TmpEnd++;
            psz_Text++;
        }

        ColorText(_l_Beg, _l_TmpEnd, mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorComment]);
    }

    *_psz_Text = psz_Text;
}

/*
 =======================================================================================================================
    Aim:    Color one complete line, words by words.

    In:     _l_Beg  0 base index of the first line to color.
            _l_End  0 base index of the last line to color.
 =======================================================================================================================
 */
void EAI_cl_View::ColorRange(LONG _l_Beg, LONG _l_End)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CHARRANGE   crOldSel;
    char        *psz_Memo, *psz_Start, *psz_Text;
    char        c_Mem;
    LONG        l_TmpEnd;
    BOOL        b_Lock;
    int         i_ScrollH, i_ScrollV;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GetSel(crOldSel);
    i_ScrollH = GetScrollPos(SB_HORZ);
    i_ScrollV = GetScrollPos(SB_VERT);
	M_MF()->LockDisplay(this);

    /* Lock update if we colorize a visible part */
    b_Lock = FALSE;
    if
    (
        ((_l_Beg >= l_FirstVisibleLine()) && (_l_Beg <= l_LastVisibleLine())) ||
        ((_l_End >= l_FirstVisibleLine()) && (_l_End <= l_LastVisibleLine()))
    )
    {
        LockWindowUpdate();
        b_Lock = TRUE;
    }
	else
	{
//		HideSelection(TRUE, FALSE);
	}

    /* Convert lines to char indexs */
    _l_Beg = LineIndex(_l_Beg);
    if(_l_End >= GetLineCount()) _l_End = GetLineCount() - 1;
    _l_End = LineIndex(_l_End);
    _l_End += LineLength(_l_End);

    /* Read buffer to color */
    SetSel(_l_Beg, _l_End);
    GetSel(_l_Beg, _l_End);
    psz_Memo = psz_Text = (char *) L_malloc(_l_End - _l_Beg + 1);
    if(!psz_Memo) return;
    GetSelText(psz_Memo);

    l_TmpEnd = _l_Beg;
    while(l_TmpEnd < _l_End)
    {
        /* Skip non alphanum characters */
        psz_Start = psz_Text;
        _l_Beg = l_TmpEnd;
        while((l_TmpEnd < _l_End) && ((psz_Text[0] != '/') || (psz_Text[1] != '/')) &&  /* C++ comment */
        (!L_isalnum(*psz_Text)) && (*psz_Text != '#') &&                                /* PP directive */
        (*psz_Text != '.') &&                                                           /* Field */
        (*psz_Text != '"')                                                              /* String */
        )
        {
            l_TmpEnd++;
            psz_Text++;
        }

        /* Color non alnum character */
        if(psz_Text != psz_Start) ColorText(_l_Beg, l_TmpEnd, mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorDefault]);

        /* String */
        psz_Start = psz_Text;
        _l_Beg = l_TmpEnd;
        if(*psz_Text == '"')
        {
            l_TmpEnd++;
            psz_Text++;
            while(*psz_Text && *psz_Text != '"')
            {
                l_TmpEnd++;
                psz_Text++;
            }

            l_TmpEnd++;
            psz_Text++;
            ColorText(_l_Beg, l_TmpEnd, mpo_Frame->mst_Ini.ax_Colors[EAI_C_ColorString]);
        }

        /* Color comment */
        psz_Start = psz_Text;
        _l_Beg = l_TmpEnd;
        ColorCppComment(&psz_Text, _l_Beg, l_TmpEnd, _l_End);

        /* Skip alphanum chars */
        psz_Start = psz_Text;
        _l_Beg = l_TmpEnd;

        /* PP or field */
        if(psz_Text[0] == '#' || psz_Text[0] == '.')
        {
            l_TmpEnd++;
            psz_Text++;
        }

        /* Isolate word */
        while((l_TmpEnd < _l_End) && ((psz_Text[0] != '/') || (psz_Text[1] != '/')) && ((L_isalnum(*psz_Text) || *psz_Text == ':')))
        {
            l_TmpEnd++;
            psz_Text++;
        }

        /* Color alnum character */
        if(psz_Text != psz_Start)
        {
            c_Mem = *psz_Text;
            *psz_Text = 0;
            ColorText(_l_Beg, l_TmpEnd, x_GetColorWord(psz_Start));
            *psz_Text = c_Mem;
        }

        /* Color comment */
        psz_Start = psz_Text;
        _l_Beg = l_TmpEnd;
        ColorCppComment(&psz_Text, _l_Beg, l_TmpEnd, _l_End);
    }

    L_free(psz_Memo);
    SetSel(crOldSel);

    if(b_Lock) UnlockWindowUpdate();

    if(i_ScrollH != GetScrollPos(SB_HORZ))
    {
        SetScrollPos(SB_HORZ, i_ScrollH);
        SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (i_ScrollH << 16), 0);
    }

    if(i_ScrollV != GetScrollPos(SB_VERT))
    {
        SetScrollPos(SB_VERT, i_ScrollV);
        SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (i_ScrollV << 16), 0);
    }

//    UpdateWindow();
//    HideSelection(FALSE, FALSE);
	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::ColorSel(LONG _l_Beg, LONG _l_End)
{
    ColorRange(LineFromChar(_l_Beg), LineFromChar(_l_End));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::OnChange(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    CHARRANGE   mst_CurSel;
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Set mb_HasTouchedVars of editor */
    if(mpo_Frame->mul_CurrentEditFile != BIG_C_InvalidIndex)
    {
        if(BIG_b_IsFileExtension(mpo_Frame->mul_CurrentEditFile, EDI_Csz_ExtAIEditorVars))
        {
            switch(me_ChangeType)
            {
            case ctReplSel:
            case ctPaste:
            case ctDelete:
            case ctBack:
            case ctCut:
            case ctUndo:
            case ctMove:
                mpo_Frame->mb_HasTouchedVars = TRUE;
                break;
            }
        }
    }

    GetSel(mst_CurSel);

    /* Cut was canceled, so this is paste operation */
    if(me_ChangeType == ctMove && mst_CurSel.cpMin == mst_CurSel.cpMax)
        me_ChangeType = ctPaste;

    switch(me_ChangeType)
    {
    /* Old=(x,y) -> cur=(x+len,x+len) */
    case ctReplSel:

    /* Old=(x,y) -> cur=(x+len,x+len) */
    case ctPaste:
        ColorSel(mst_OldSel.cpMin, mst_CurSel.cpMax);
        break;

    /* Old=(x,y) -> cur=(x,x) */
    case ctDelete:

    /* Old=(x,y) -> cur=(x,x), newline del => old=(x,x+1) -> cur=(x-1,x-1) */
    case ctBack:

    /* Old=(x,y) -> cur=(x,x) */
    case ctCut:
        ColorSel(mst_CurSel.cpMin, mst_CurSel.cpMax);
        break;

    /* Old=(?,?) -> cur=(x,y) */
    case ctUndo:
        ColorSel(mst_CurSel.cpMin, mst_CurSel.cpMax);
        break;

    /* Old=(x,x+len) -> cur=(y-len,y) | cur=(y,y+len) */
    case ctMove:
        ColorSel(mst_CurSel.cpMin, mst_CurSel.cpMax);

        /* Move after */
        if(mst_CurSel.cpMin > mst_OldSel.cpMin) ColorSel(mst_OldSel.cpMin, mst_OldSel.cpMin);

        /* Move before */
        else
            ColorSel(mst_OldSel.cpMax, mst_OldSel.cpMax);
        break;

    default: 
        ColorSel(mst_OldSel.cpMin, mst_CurSel.cpMax);
		break;
    }

    /* Undo action does not call OnProtected, so make it default */
    me_ChangeType = ctUndo;
}

/*
 =======================================================================================================================
    Aim:    Init me_ChangeType depending of the user request.
 =======================================================================================================================
 */
void EAI_cl_View::OnProtected(NMHDR *pNMHDR, LRESULT *pResult)
{
    /*~~~~~~~~~~~~~~~~~~~~*/
    ENPROTECTED *pEP;
    LONG        l_Limit;
    /*~~~~~~~~~~~~~~~~~~~~*/

    pEP = (ENPROTECTED *) pNMHDR;

    switch(pEP->msg)
    {
    case WM_KEYDOWN:
        switch(pEP->wParam)
        {
        case VK_DELETE:
            me_ChangeType = ctDelete;
            break;
        case VK_BACK:
            me_ChangeType = ctBack;
            break;
        default:
            me_ChangeType = ctUnknown;
            break;
        }
        break;

    case EM_REPLACESEL:
    case WM_CHAR:
        me_ChangeType = ctReplSel;
        break;

    case WM_PASTE:
        me_ChangeType = (me_ChangeType == ctCut) ? ctMove : ctPaste;
        break;

    case WM_CUT:
        me_ChangeType = ctCut;
        break;

    case EM_SETCHARFORMAT:
        break;

    default:
        me_ChangeType = ctUnknown;
        break;
    };

    if(pEP->msg != EM_SETCHARFORMAT && me_ChangeType != ctMove)
        mst_OldSel = pEP->chrg;

    /* Be carreful of edit max size */
    if(me_ChangeType == ctPaste || me_ChangeType == ctReplSel)
    {
        l_Limit = GetLimitText();
        if(l_Limit - GetTextLength() < 1024) LimitText(GetTextLength() + 1024);
    }

    *pResult = FALSE;
}

#endif /* ACTIVE_EDITORS */

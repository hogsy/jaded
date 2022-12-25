/*$T AIview_km.cpp GC!1.71 03/02/00 12:04:18 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
#include "EDIapp.h"
#include "EDImainframe.h"




/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    /*~~~~~~~~~~~~~~~~~~~~*/
    CHARRANGE   cr;
    char        astr[2];
    /*~~~~~~~~~~~~~~~~~~~~*/

    if((nChar != 9) && (nChar < 32)) return;

    /* If character treatment is locked, exit */
    if(mb_CharTreat)
    {
        mb_CharTreat = FALSE;
        return;
    }

    /* First delete selection if one */
    GetSel(cr);
    if(cr.cpMin != cr.cpMax) DeleteSel();

    /* Return */
    if(nChar == VK_RETURN)
    {
        AddString(CString("\r\n"));
        return;
    }

    /* Add normal char */
    astr[1] = 0;
    astr[0] = nChar;
    AddString(CString(astr));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BOOL        b_Ctrl, b_Shift;
    CString     o_Text, o_AddTab, o_Str;
    char        *pcur;
    CHARRANGE   cr;
    LONG        l_Index, l_Index1, l_CptTab;
    CString     o_Left, o_Right, o_Word;
    char        asz_Path[BIG_C_MaxLenPath];
    BOOL        b_OnlySpaces;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Shift = GetAsyncKeyState(VK_SHIFT) & 0x8000 ? TRUE : FALSE;
    b_Ctrl = GetAsyncKeyState(VK_CONTROL) & 0x8000 ? TRUE : FALSE;
    mb_CharTreat = FALSE;
    GetSel(cr);

	// Modif on dec.16.2004, Jean-Sylvain Sormany (Part 1)
	// corrected a Bug with US keyb usage (Impossible to put quote mark)
	char  aszLang[10];
	memset(aszLang, 0, sizeof(aszLang));
	GetKeyboardLayoutName(aszLang);
	// End of modification (Part 1) (Part 2 below)

    /* Help */
    if(nChar == VK_F1)
    {
        GetSel(cr);
        o_Left = o_GetWordLeft(cr.cpMin);
        o_Right = o_GetWordRight(cr.cpMax);
        cr.cpMin -= o_Left.GetLength();
        cr.cpMax += o_Right.GetLength();
        SetSel(cr);
        o_Word = GetSelText();
        L_strcpy(asz_Path, "AI script/");
        L_strcat(asz_Path, (char *) (LPCSTR) o_Word);
        EDI_go_TheApp.SearchHelp(asz_Path);
        return;
    }

    /* Inhibit some keys */
    if(nChar == VK_ESCAPE)
    {
        mb_CharTreat = TRUE;
        return;
    }

	/* Completion (²) */
	// Modif on dec.16.2004, Jean-Sylvain Sormany (Part 2)
	// corrected a Bug with US keyb usage (Impossible to put quote mark)
	// Replacing :
	//if( nChar == 222 || nChar == 192) 
	// 192 is the corner key for US Keyb, 222 for FR Keyb
	if ((atol(aszLang) == 409l && nChar == 192)
		|| (nChar == 222 && atol(aszLang) != 409l))
	// End of modification Part 2
    {
        mb_CharTreat = TRUE;
        if (b_Ctrl && b_Shift)
            OnCompletion(AI_C_Completion_Variable);
        else if (b_Shift)
            OnCompletion(AI_C_Completion_ProcList);
        else if (b_Ctrl)
            OnCompletion(AI_C_Completion_Macro);
        else
            OnCompletion(AI_C_Completion_All);
        return;
    }

    /* Select all */
    if(b_Ctrl && nChar == 'A')
    {
        mb_CharTreat = TRUE;
        SetSel(0, -1);
        return;
    }

    /* Undo */
    if(b_Ctrl && nChar == 'Z')
    {
        mb_CharTreat = TRUE;
        LockWindowUpdate();
        HideSelection(TRUE, FALSE);
        mo_UndoManager.b_Undo();
        HideSelection(FALSE, FALSE);
        UnlockWindowUpdate();
        return;
    }

    /* Redo */
    if(b_Ctrl && nChar == 'Y')
    {
        mb_CharTreat = TRUE;
        LockWindowUpdate();
        HideSelection(TRUE, FALSE);
        mo_UndoManager.b_Redo();
        HideSelection(FALSE, FALSE);
        UnlockWindowUpdate();
        return;
    }

    /* Copies into clipboard */
    if(b_Ctrl && (nChar == 'C' || nChar == VK_INSERT))
    {
        mb_CharTreat = TRUE;
        o_Text = GetSelText();
        if(cr.cpMin != cr.cpMax) CopyIntoClipBoard(o_Text);
        return;
    }

    if((b_Ctrl && nChar == 'V') || (b_Shift && nChar == VK_INSERT))
    {
        mb_CharTreat = TRUE;
        o_Text = o_GetStringFromClipBoard();
        if(cr.cpMin != cr.cpMax) DeleteSel();
        AddString(o_Text);
        return;
    }

    if((b_Ctrl && nChar == 'X') || (b_Shift && nChar == VK_DELETE))
    {
        mb_CharTreat = TRUE;
        o_Text = GetSelText();
        if(cr.cpMin != cr.cpMax) DeleteSel();
        CopyIntoClipBoard(o_Text);
        return;
    }

    /* Delete current line */
    if(b_Ctrl && nChar == 'L')
    {
        mb_CharTreat = TRUE;
        cr.cpMin = LineIndex(LineFromChar(cr.cpMin));
        SetSel(cr.cpMin, cr.cpMin + LineLength(cr.cpMin) + 2);
        DeleteSel();
        return;
    }

    /* Scroll one line up/down */
    if(b_Ctrl && nChar == VK_UP)
    {
        mb_CharTreat = TRUE;
        LineScroll(-1);
        return;
    }

    if(b_Ctrl && nChar == VK_DOWN)
    {
        mb_CharTreat = TRUE;
        LineScroll(1);
        return;
    }

    /* Move one word right */
    if(b_Ctrl && nChar == VK_RIGHT)
    {
        mb_CharTreat = TRUE;
        GetWindowText(o_Text);
        CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
        GetSel(cr);
        while((cr.cpMax < o_Text.GetLength()) && (o_Text[(int) cr.cpMax] == '_'))
        {
            CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
            CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
            GetSel(cr);
        }

        return;
    }

    /* Move one word left */
    if(b_Ctrl && nChar == VK_LEFT)
    {
        mb_CharTreat = TRUE;
        GetWindowText(o_Text);
        CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
        GetSel(cr);
        while(cr.cpMin && (o_Text[(int)(cr.cpMin - 1)] == '_'))
        {
            CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
            CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
            GetSel(cr);
        }

        return;
    }

    /* Tabulation */
    if(nChar == VK_TAB)
    {
        GetSel(cr);
        if(cr.cpMin != cr.cpMax)
        {
            mb_CharTreat = TRUE;
            if(b_Shift)
                DelBlockString(CString("\t"));
            else
                AddBlockString(CString("\t"));
            return;
        }
        else if(b_Shift)
        {
            mb_CharTreat = TRUE;
            cr.cpMin--;
            cr.cpMax--;
            SetSel(cr);
            return;
        }
    }

    /* Comment block */
    if(nChar == 0xbe)
    {
        GetSel(cr);
        if(b_Ctrl)
        {
            mb_CharTreat = TRUE;
            if(b_Shift)
                DelBlockString(CString("//"));
            else
                AddBlockString(CString("//"));
            SetSel(cr.cpMax, cr.cpMax);
        }

        return;
    }

    /* Delete */
    if(nChar == VK_DELETE)
    {
        GetSel(cr);

        /* Delete word right */
        if(b_Ctrl && cr.cpMin == cr.cpMax)
        {
            mb_CharTreat = TRUE;
            o_Str = o_GetWordRight(cr.cpMin);
            SetSel(cr.cpMin, cr.cpMin + o_Str.GetLength());
            DeleteSel();
            return;
        }

        /* Delete char or selection */
        else
        {
            mb_CharTreat = TRUE;
            DeleteSel();
            return;
        }
    }

    /* Backspace */
    if(nChar == VK_BACK)
    {
        mb_CharTreat = TRUE;
        GetSel(cr);
        if((cr.cpMin != cr.cpMax) || (cr.cpMin > 0))
        {
            if(cr.cpMin == cr.cpMax)
            {
                cr.cpMin--;
                SetSel(cr);
            }

            DeleteSel();
        }

        return;
    }

    /* Beg line */
    if((nChar == VK_HOME) && (!b_Ctrl) && (!b_Shift))
    {
		int		StartCharInLine;
		BOOL	b_StartLine;

        GetSel(cr);

        mb_CharTreat = TRUE;
        GetWindowText(o_Text);

		StartCharInLine = this->LineIndex(this->LineFromChar(cr.cpMin));
		b_StartLine = (StartCharInLine == cr.cpMin);

		if(cr.cpMin <= 1) return;

		if(!b_StartLine)
			cr.cpMin --;

		pcur = (char *) (LPCSTR) o_Text + cr.cpMin;

        b_OnlySpaces = TRUE;
		while(cr.cpMin > StartCharInLine)
		{
           if(!L_isspace(*pcur)) b_OnlySpaces = FALSE;
            cr.cpMin--;
            pcur--;
		}

        if(!b_OnlySpaces)
		{
			while(L_isspace(*pcur) && (*pcur != '\r'))
			{
				cr.cpMin++;
				pcur++;
			}
		}

        cr.cpMax = cr.cpMin;

        SetSel(cr);
        return;
    }

    /* Return (smart indent) */
    if(!b_Shift && !b_Ctrl && (nChar == VK_RETURN))
    {
        mb_CharTreat = TRUE;

        M_MF()->LockDisplay(this);
        HideSelection(TRUE, FALSE);

        GetSel(cr);
        GetWindowText(o_Text);

        pcur = (char *) (LPCSTR) o_Text;
        l_Index = cr.cpMin - 1;
        if (l_Index < 0) l_Index = 0;
        l_CptTab = 0;
        l_Index1 = l_Index;

        if(pcur[l_Index] == '{')
        {
            l_CptTab++;
            o_AddTab += "\t";
        }

        while(l_Index && pcur[l_Index] != '\n')
            l_Index--;
        if(pcur[l_Index] == '\n')
        {
            l_Index++;
            while(pcur[l_Index] && (l_Index < l_Index1) && ((pcur[l_Index] == '\t') || (pcur[l_Index] == ' ')))
            {
                o_AddTab += pcur[l_Index];
                l_Index++;
                l_CptTab++;
            }
        }

        AddString(CString("\r\n"));
        if(l_CptTab) AddString(o_AddTab, TRUE);

		/* Scroll si dernière ligne */
        GetSel(cr);
		cr.cpMin = LineFromChar(cr.cpMin);
		if(cr.cpMin >= l_LastVisibleLine()) LineScroll(1, 0);
        SetScrollPos(SB_HORZ, 0);
        SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (0 << 16), 0);

        HideSelection(FALSE, FALSE);
        M_MF()->UnlockDisplay(this);
        return;
    }

    CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CRichEditCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

#endif /* ACTIVE_EDITORS */

/*$T AIview_mdfy.cpp GC!1.39 07/02/99 17:38:32 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "AIview.h"
#include "AIundo.h"
#include "DIAlogs/DIAcompletion_dlg.h"
#include "BASe/CLIbrary/CLIstr.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_View::AddString(CString &_o_Str, BOOL _b_Locked)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EAI_cl_UndoString   *p;
    CHARRANGE           cr;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GetSel(cr);
    p = new EAI_cl_UndoString(this, cr.cpMin, _o_Str, _b_Locked);
    mo_UndoManager.b_AskFor(p);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_View::DeleteSel(BOOL _b_Locked)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EAI_cl_UndoDelete   *p;
    CHARRANGE           cr;
    CString             o_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    HideSelection(TRUE, FALSE);

    GetSel(cr);
    if(cr.cpMin == cr.cpMax)
    {
        cr.cpMax = cr.cpMin + 1;
        GetWindowText(o_String);
        SetSel(cr);
        o_String = GetSelText();
    }
    else
    {
        o_String = GetSelText();
    }

    p = new EAI_cl_UndoDelete(this, cr, o_String, _b_Locked);

    HideSelection(FALSE, FALSE);
    mo_UndoManager.b_AskFor(p);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_View::CopyIntoClipBoard(CString &csText)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    lSize;
    HANDLE  hText;
    LPVOID  pvText;
    HANDLE  hRes;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(OpenClipboard())
    {
        if(EmptyClipboard())
        {
            lSize = csText.GetLength();
            hText = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, lSize + 1);

            if(hText != NULL)
            {
                pvText = GlobalLock(hText);

                if(pvText != NULL)
                {
                    CopyMemory(pvText, LPCTSTR(csText), lSize + 1);
                    hRes = SetClipboardData(CF_TEXT, hText);
                    if(hRes == NULL) return;
                    GlobalUnlock(hText);
                }
            }
        }

        CloseClipboard();
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EAI_cl_View::o_GetStringFromClipBoard(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CString csText;
    HANDLE  hText;
    LPVOID  pvText;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    csText = "";

    if(OpenClipboard())
    {
        hText = GetClipboardData(CF_TEXT);

        if(hText != NULL)
        {
            pvText = GlobalLock(hText);

            if(pvText != NULL)
            {
                csText = (char *) pvText;
                GlobalUnlock(hText);
            }
        }

        CloseClipboard();
    }

    return csText;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_View::AddBlockString(CString &_o_Ref)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CString     o_Text;
    char        *psz_Temp;
	char		*pcur;
    CHARRANGE   cr, crmemo;
    int         i, i_Beg;
    BOOL        bSingle;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    LockWindowUpdate();
    HideSelection(TRUE, FALSE);

    GetWindowText(o_Text);
    GetSel(cr);

    crmemo.cpMin = cr.cpMin;
    crmemo.cpMax = cr.cpMax;
    psz_Temp = (char *) (LPCSTR) o_Text;

    bSingle = TRUE;
    i_Beg = cr.cpMax;
    if(cr.cpMin != cr.cpMax)
    {
        bSingle = FALSE;
        i_Beg--;
    }

    i = i_Beg;
    while(i)
    {
		pcur = psz_Temp + i;
		if((i != i_Beg) && (*pcur == '\n'))
        {
            SetSel(i + 1, i + 1);
            AddString(_o_Ref, TRUE);
            cr.cpMax += _o_Ref.GetLength();
            if(i <= cr.cpMin)
            {
                cr.cpMin = i + 1;
                break;
            }
        }

        i--;
    }

    if(!i)
    {
        SetSel(0, 0);
        AddString(_o_Ref, TRUE);
        cr.cpMax += _o_Ref.GetLength();
        cr.cpMin = 0;
    }

    /* Select the end of the line */
    if(!bSingle)
    {
        GetWindowText(o_Text);
        psz_Temp = (char *) (LPCSTR) o_Text;
		pcur = psz_Temp + cr.cpMax - 1;
        if(*pcur != '\n')
		{
			pcur = psz_Temp + cr.cpMax;
            while(*pcur && (*pcur != '\n'))
			{
				cr.cpMax++;
				pcur = psz_Temp + cr.cpMax;
			}
		}
        SetSel(cr);
    }
    else
    {
        crmemo.cpMin += _o_Ref.GetLength();
        crmemo.cpMax += _o_Ref.GetLength();
        SetSel(crmemo);
    }

    HideSelection(FALSE, FALSE);
    UnlockWindowUpdate();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_View::DelBlockString(CString &_o_Ref)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CString     o_Text;
    char        *psz_Temp;
	char		*pcur;
    CHARRANGE   cr, crmemo;
    int         i, j, i_Beg;
    BOOL        b_Res;
    BOOL        bSingle, bOK;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    LockWindowUpdate();
    HideSelection(TRUE, FALSE);

    GetWindowText(o_Text);
    GetSel(cr);
    crmemo.cpMin = cr.cpMin;
    crmemo.cpMax = cr.cpMax;
    psz_Temp = (char *) (LPCSTR) o_Text;

    bSingle = TRUE;
    i_Beg = cr.cpMax;
    if(cr.cpMin != cr.cpMax)
    {
        bSingle = FALSE;
        i_Beg--;
    }

    i = i_Beg;
    bOK = FALSE;
    while(i >= 0)
    {
        j = 0;
        b_Res = TRUE;
        while(j < _o_Ref.GetLength())
        {
			pcur = psz_Temp + i - j;
            if(*pcur != _o_Ref[_o_Ref.GetLength() - (j + 1)])
            {
                b_Res = FALSE;
                break;
            }

            j++;
        }

        if(b_Res)
        {
			pcur = psz_Temp + i - _o_Ref.GetLength();
            if((i != i_Beg) && ((!i) || (i - _o_Ref.GetLength() + 1 == 0) || (*pcur == '\n')))
            {
                SetSel(i - _o_Ref.GetLength() + 1, i + 1);
                DeleteSel(TRUE);
                bOK = TRUE;
                cr.cpMax -= _o_Ref.GetLength();
                if(i <= cr.cpMin)
                {
                    cr.cpMin = i;
                    break;
                }
            }
        }

		pcur = psz_Temp + i;
        if(((*pcur) == '\n') && (i <= cr.cpMin))
        {
            cr.cpMin = i + 1;
            break;
        }

        i--;
    }

    /* Select the end of the line */
    if(!bSingle)
    {
        GetWindowText(o_Text);
        psz_Temp = (char *) (LPCSTR) o_Text;
		pcur = psz_Temp + cr.cpMax - 1;
        if(*pcur != '\n')
		{
			pcur = psz_Temp + cr.cpMax;
            while(*pcur && (*pcur != '\n'))
			{
				cr.cpMax++;
				pcur = psz_Temp + cr.cpMax;
			}
		}
        SetSel(cr);
    }
    else
    {
        if(bOK)
        {
            crmemo.cpMin -= _o_Ref.GetLength();
            crmemo.cpMax -= _o_Ref.GetLength();
        }

        SetSel(crmemo);
    }

    HideSelection(FALSE, FALSE);
    UnlockWindowUpdate();
}

/*
 ===================================================================================================
    Aim:    Call to complete a word. If there's only one solution, the dialog is not displayed,
            else a dialog is displayed to choose between all the words.
 ===================================================================================================
 */
void EAI_cl_View::OnCompletion(int what)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDIA_cl_Completion  o_Dial(what);
    CHARRANGE           cr, cr1;
    CString             o_Str, o_Str1, o_Text;
    char                *psz_Str, *psz_Temp, *psz_Temp1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_Dial.mpo_Frame = mpo_Frame;
    if(o_Dial.DoModal() == IDOK)
    {
        GetSel(cr);
        o_Str = o_GetWordLeft(cr.cpMin);
        o_Str1 = o_GetWordRight(cr.cpMin);

        HideSelection(TRUE, FALSE);

        /* Delete previous word */
        SetSel(cr.cpMin - o_Str.GetLength(), cr.cpMin + o_Str1.GetLength());
        DeleteSel(FALSE);
        GetSel(cr);

        o_Str = o_Dial.mo_Word;

		/* Eliminate return value */
		psz_Temp = (char *) (LPCSTR) o_Str;
		if(psz_Temp = L_strchr(psz_Temp, ' '))
		{
			psz_Temp1 = L_strchr((char *) (LPCSTR) o_Str, '(');
			if(psz_Temp < psz_Temp1)
			{
				o_Str = psz_Temp;

				while(psz_Temp && *psz_Temp && L_isspace(*psz_Temp)) psz_Temp++;
				if(psz_Temp)
					o_Str = psz_Temp;
			}
		}

        /* Add parameters for function ? */
        psz_Str = (char *) (LPCSTR) o_Dial.mo_Word;
        if((psz_Str = L_strchr(psz_Str, '(')))
        {
            /* Add only if there's not already an open parenthese */
            GetSel(cr1);
            SetSel(cr1.cpMin, cr1.cpMin + 3);
            o_Text = GetSelText();
            SetSel(cr);

            if(!L_strchr((char *) (LPCSTR) o_Text, '('))
            {
                psz_Temp = L_strchr((char *) (LPCSTR) o_Str, '(');
                psz_Temp++;
                if(*psz_Temp != ')')
                {
                    *psz_Temp = 0;

                    psz_Str++;
                    while(*psz_Str != ')')
                    {
                        if(*psz_Str == ' ') *(psz_Temp++) = ',';
                        psz_Str++;
                    }
                }

                *(psz_Temp++) = ')';
                *(psz_Temp++) = 0;
            }
            else
            {
                psz_Str = L_strchr((char *) (LPCSTR) o_Str, '(');
                *psz_Str = 0;
            }
        }

        /* Add new one */
        GetSel(cr);
        AddString(o_Str, TRUE);

        psz_Str = L_strchr((char *) (LPCSTR) o_Str, '(');
        if(psz_Str)
        {
            cr.cpMin += (psz_Str - (char *) (LPCSTR) o_Str) + 1;
            cr.cpMax = cr.cpMin;
            SetSel(cr);
        }

        HideSelection(FALSE, FALSE);
    }

	SetFocus();
}

#endif /* ACTIVE_EDITORS */

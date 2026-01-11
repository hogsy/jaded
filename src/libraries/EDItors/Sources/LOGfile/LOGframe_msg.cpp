/*$T LOGframe_msg.cpp GC! 1.081 06/19/00 10:36:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "EDImainframe.h"
#include "LOGmsg.h"
#include "LOGframe.h"
#include "EDImsg.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLImem.h"
#include "LINKs/LINKmsg.h"
#include "EDIapp.h"


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ELOG_cl_Frame::AddText(const char *_psz_Text, BOOL _bAddToStrings, COLORREF _x_Color,int _iCanal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i_Len, i_Line1, i_Line2;
	char		*psz_Cpy;
	CHARFORMAT	st_Format;
	L_FILE		f;
	char		asz_Path[512];
	char		*psz_Temp;
	char		VisualNET_TaMere[2048];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (_bAddToStrings)
    {
        ELOGString *pString = ma_Strings+mi_StringIndex;
        pString->mi_Canal = _iCanal;
        pString->ms_String = _psz_Text;
        pString->mx_Color = _x_Color;
        mi_StringIndex++;
        if (mi_StringIndex >= ELOG_MaxStringNb)
        {
            mi_StringIndex = 0;
            mb_IsBufferFull = TRUE;
        }
    }

	if(!mao_Filter[0].IsEmpty())
	{
		CString o1(_psz_Text);
		if(StringIsSkipped(o1)) return;
		_x_Color = 0x00FFA000;
	}

	i_Len = mpo_Edit->GetWindowTextLength();
	mpo_Edit->SetSel(i_Len, i_Len);
	i_Line1 = mpo_Edit->LineFromChar(i_Len);

	st_Format.dwMask = CFM_COLOR | CFM_BOLD | CFM_ITALIC;
	st_Format.dwEffects = CFE_PROTECTED;

	while(*_psz_Text == '%')
	{
		switch(_psz_Text[1])
		{
		case 'b':
			st_Format.dwEffects |= CFE_BOLD;
			_psz_Text += 2;
			break;
		case 'i':
			st_Format.dwEffects |= CFE_ITALIC;
			_psz_Text += 2;
			break;
		default: goto end;
		}
	}

	if(!mao_Filter[0].IsEmpty()) st_Format.dwEffects |= CFE_BOLD;

end:

	/* Write to file ? */
	if(mst_Ini.b_LogToFile && _bAddToStrings)
	{
		/* Create file at the same pos as exe */
		L_strcpy(asz_Path, EDI_go_TheApp.m_pszHelpFilePath);
		psz_Temp = L_strrchr(asz_Path, '\\');
		if(!psz_Temp) psz_Temp = L_strrchr(asz_Path, '/');
		if(psz_Temp)
			L_strcpy(psz_Temp + 1, ELOG_Csz_FileName);
		else
			L_strcpy(asz_Path, ELOG_Csz_FileName);

		if(!(L_access(asz_Path, 0))) L_chmod(asz_Path, L_S_IREAD | L_S_IWRITE);
		f = L_fopen(asz_Path, "at");
		if(!CLI_FileOpen(f)) f = L_fopen(asz_Path, "wt");
		if(CLI_FileOpen(f))
		{
			L_fputs(_psz_Text, f);
			if(_psz_Text[L_strlen(_psz_Text) - 1] != '\n') L_fputc('\n', f);
			L_fclose(f);
		}
	}

    if ((1<<_iCanal) & mui_ActiveCanal)
    {
        st_Format.crTextColor = _x_Color;
        mpo_Edit->SetSelectionCharFormat(st_Format);
        mpo_Edit->SetOptions(ECOOP_OR, ECO_WANTRETURN);

        /* Check max size of text */
        if(mpo_Edit->GetTextLength() + (LONG) L_strlen(_psz_Text) >= mpo_Edit->GetLimitText() - 512)
            mpo_Edit->LimitText(mpo_Edit->GetTextLength() * 2);

        psz_Cpy = L_strdup(_psz_Text);

        if(psz_Cpy[L_strlen(psz_Cpy) - 1] == '\n')
        {
            psz_Cpy[L_strlen(psz_Cpy) - 1] = 0;
            mpo_Edit->ReplaceSel(psz_Cpy);
            LINK_gb_OnSameLine = TRUE;
        }
        else
        {
            /* BEFORE VISUAL 7 ... GREAT CODE ...

            mpo_Edit->ReplaceSel(_psz_Text);
            mpo_Edit->SendMessage(WM_CHAR, VK_RETURN, 0);
            LINK_gb_OnSameLine = FALSE;
            */

            /* AFTER VISUAL 7 --> SHIT */
            sprintf(VisualNET_TaMere, "%s\n", psz_Cpy );
            mpo_Edit->ReplaceSel(VisualNET_TaMere);
            LINK_gb_OnSameLine = FALSE;
        }

        L_free(psz_Cpy);

        /* Scroll */
        i_Len = mpo_Edit->GetWindowTextLength();
        i_Line2 = mpo_Edit->LineFromChar(i_Len);
        if(mpo_Edit->GetStyle() & WS_VSCROLL) mpo_Edit->LineScroll(i_Line2 - i_Line1);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ELOG_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString				o_String;
	char				*psz_Temp;
	EDI_tdst_DragDrop	*pst_DragDrop;
	char				*pc_Temp;
	ULONG				ul_Size;
    BOOL                bResult = TRUE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    int                 iCanal =  _ul_Msg>>16;
    if (iCanal)
        iCanal--;

	switch(_ul_Msg & 0xFFFF)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ELOG_MESSAGE_CLEARLOG:
		mpo_Edit->SetWindowText("");
		bResult = FALSE;
        break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANSELFILE:
	case EDI_MESSAGE_CANSELDATA:
		bResult = FALSE;
        break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ELOG_MESSAGE_ADDLINE:
		if(!mst_Ini.b_Freeze)
		{
			psz_Temp = (char *) _ul_Param1;
			while(L_isspace(*psz_Temp)) psz_Temp++;
			AddText(psz_Temp, TRUE, (COLORREF) _ul_Param2,iCanal);
		}

		bResult = TRUE;
        break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANDRAGDROP:
		pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
		if ((pst_DragDrop->ul_FatFile == BIG_C_InvalidIndex) ||
            (pst_DragDrop->i_Type == EDI_DD_Long))
            bResult = FALSE;
        else 
            bResult = TRUE;
        break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_ENDDRAGDROP:
		pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
		pc_Temp = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(pst_DragDrop->ul_FatFile), &ul_Size);
		AddText(pc_Temp, TRUE, 0,iCanal);
		L_free(pc_Temp);
		break;
	}

	return bResult;
}

#endif /* ACTIVE_EDITORS */

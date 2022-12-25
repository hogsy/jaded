/*$T LOGframe.cpp GC 1.138 08/02/05 09:32:56 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#define ACTION_GLOBAL
#include "EDImainframe.h"
#include "LOGframe_act.h"
#include "LOGframe.h"
#include "BIGfiles/BIGfat.h"
#include "EDImsg.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "DIAlogs/DIAname_dlg.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(ELOG_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(ELOG_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ELOG_cl_Frame::ELOG_cl_Frame(void)
{
    mpo_Edit = new CRichEditCtrl;
    mui_ActiveCanal = 0x3F; // 6 active canals.
    mi_StringIndex=0;
    mb_IsBufferFull=FALSE;

	mb_ValidFont = FALSE;
	mi_SizeFont = 14;
	L_strcpy(masz_NameFont, "courier new");
	for(int i=0; i<10; i++)
		mao_Filter[i].Empty();

	mb_FilterOut = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ELOG_cl_Frame::~ELOG_cl_Frame(void)
{
	mo_Fnt.DeleteObject();
    mpo_Edit->DestroyWindow();
    delete mpo_Edit;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ELOG_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1) return -1;

	/* Create edit box */
    mpo_Edit->Create
        (
        WS_HSCROLL |
        WS_VSCROLL |
        WS_VISIBLE |
        WS_BORDER |
        WS_CHILD |
        ES_MULTILINE |
        ES_AUTOVSCROLL |
        ES_AUTOHSCROLL,
        CRect(0, 0, 0, 0),
        this,
        0
        );
    DefineFont(masz_NameFont, mi_SizeFont);
    mpo_Edit->Invalidate();

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ELOG_cl_Frame::StringIsSkipped(CString & str)
{
	int i, j, k;

	if(mao_Filter[0].IsEmpty()) return (!mb_FilterOut);

	str.MakeLower();
	for(i=j=0; (i<10) && !mao_Filter[i].IsEmpty(); i++)
	{
		k = str.Find(mao_Filter[i], j);
		if(k == -1) return mb_FilterOut;
		j=k+1;
	}
	return (!mb_FilterOut);
}

void ELOG_cl_Frame::OnFilter(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static EDIA_cl_NameDialog	o_Dialog("Enter string filter");
	CString str;
	CString resToken;
	int curPos;
	int i ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	if(o_Dialog.DoModal() == IDOK)
	{
		mb_FilterOut = TRUE;
		for(i=0; i<10; i++) mao_Filter[i].Empty();
		
		str = o_Dialog.mo_Name;
		str.MakeLower();

		if(str.GetAt(0) == '!') mb_FilterOut = FALSE;
		str.Remove('!');

		i = 0;curPos= 0;
		resToken = str.Tokenize("*",curPos);
		while( (resToken != "") && (i<10) )
		{
			mao_Filter[i++] = resToken;
			resToken= str.Tokenize("*",curPos);
		};
	}

	if(mao_Filter[0].IsEmpty())
		mpo_Edit->SetBackgroundColor(FALSE, RGB(0xff, 0xff, 0xff));
	else
		mpo_Edit->SetBackgroundColor(FALSE, RGB(0, 0, 0));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ELOG_cl_Frame::PreTranslateMessage(MSG *pMsg)
{
	if(pMsg->message == WM_LBUTTONDBLCLK)
	{
		if(pMsg->hwnd == mpo_Edit->GetSafeHwnd())
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			ULONG			ul_Key;
			LONG			l_Start, l_End;
			char			*sz_Str;
			CString			o_Str;
			EBRO_cl_Frame	*po_Browser;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			l_Start = mpo_Edit->LineIndex();
			l_End = mpo_Edit->LineLength();
			mpo_Edit->SetSel(l_Start, l_Start + l_End);
			o_Str = mpo_Edit->GetSelText();
			sz_Str = (char *) (LPCTSTR) o_Str;

			while((sz_Str = strchr(sz_Str, '[')) != NULL)
			{
				/* zap warning */
				if(sz_Str[1] == 'W')
				{
					sz_Str++;
					continue;
				}

				/* zap others [...] strings */
				if(sz_Str[9] != ']')
				{
					sz_Str++;
					continue;
				}

				ul_Key = 0;
				l_Start = 1;
				while(l_Start < 9)
				{
					if(isdigit(sz_Str[l_Start]))
						ul_Key = (ul_Key << 4) + (sz_Str[l_Start] - '0');
					else if((sz_Str[l_Start] >= 'A') && (sz_Str[l_Start] <= 'F'))
						ul_Key = (ul_Key << 4) + (sz_Str[l_Start] - 'A' + 10);
					else if((sz_Str[l_Start] >= 'a') && (sz_Str[l_Start] <= 'f'))
						ul_Key = (ul_Key << 4) + (sz_Str[l_Start] - 'a' + 10);
					else
						break;
					l_Start++;
				}

				if((l_Start == 9) && (ul_Key != BIG_C_InvalidKey))
				{
					ul_Key = BIG_ul_SearchKeyToFat(ul_Key);
					if(ul_Key != BIG_C_InvalidIndex)
					{
						po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
						po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ul_Key), ul_Key);
						break;
					}
				}

				sz_Str++;
			}
		}
	}

	return EDI_cl_BaseFrame::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ELOG_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	/* Call parent function */
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);

	/* Resize edit */
	if(mpo_Edit) mpo_Edit->MoveWindow(5, 0, cx - 10, cy - 5);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ELOG_cl_Frame::OnMouseWheel(UINT, short _zdelta, CPoint)
{
	if(GetAsyncKeyState(VK_SHIFT) < 0)
	{
		mi_SizeFont += _zdelta < 0 ? -1 : 1;
		DefineFont(masz_NameFont, mi_SizeFont);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ELOG_cl_Frame::DefineFont(char *_psz_Name, int _i_Width)
{
	/*~~~~~~~~~*/
	LOGFONT *plf;
	/*~~~~~~~~~*/

	if(mb_ValidFont)
	{
		mo_Fnt.Detach();
		DeleteObject(&mo_Fnt);
	}

	if(_i_Width < 4) _i_Width = 4;

	/* Create font. */
	plf = (LOGFONT *) LocalAlloc(LPTR, sizeof(LOGFONT));
	lstrcpy(plf->lfFaceName, _psz_Name);
	plf->lfHeight = _i_Width;
	plf->lfWeight = 500;
	plf->lfEscapement = 0;
	mo_Fnt.CreateFontIndirect(plf);
	LocalFree((LOCALHANDLE) plf);
	mb_ValidFont = TRUE;
	mi_SizeFont = _i_Width;

	/* Set font in edit box */
	mpo_Edit->SetFont(&mo_Fnt);
	mpo_Edit->Invalidate();
}
#endif /* ACTIVE_EDITORS */

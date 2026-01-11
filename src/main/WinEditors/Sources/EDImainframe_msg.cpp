/*$T EDImainframe_msg.cpp GC! 1.081 06/20/00 11:17:23 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "EDImsg.h"
#include "EDIbaseview.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "LINKs/LINKstruct.h"
#include "DIAlogs/DIAbase.h"
#include "EDIapp.h"
#include "LINKs/LINKtoed.h"

/*$4
 ***********************************************************************************************************************
    Global
 ***********************************************************************************************************************
 */

CMap<BIG_INDEX, BIG_INDEX, int, int>	EDI_go_FHCDir;
CMap<BIG_INDEX, BIG_INDEX, int, int>	EDI_go_FHCFile;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Call when the fat of the bigfile has changed. This function looks every files and dir in fat to determin if
            the have changed. It constructs a list with all modified files and dirs (and with the type of the change).
            Editors can the react if they are concerned with a given file or dir.

    Note:   Not optimised, cause parse all the fat.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::FatHasChanged(void)
{
	/*~~~~~~~~~~~~~~~*/
	BIG_INDEX	i;
	char		change;
	char		cc;
	MSG			msg;
	/*~~~~~~~~~~~~~~~*/

	/* Construct file list */
	for(i = 0; i < BIG_MaxFile(); i++)
	{
		change = BIG_FileChanged(i) &~EDI_FHC_Touch &~EDI_FHC_Loaded &~EDI_FHC_Create;
		if(change && (change != EDI_FHC_Deleted))
		{
			EDI_go_FHCFile.SetAt(i, change);
			if(change == EDI_FHC_Delete)
			{
				BIG_FileChanged(i) = EDI_FHC_Deleted;
			}
			else
			{
				BIG_FileChanged(i) = (BIG_FileChanged(i) & EDI_FHC_Loaded);
				EDI_go_FHCDir.SetAt(BIG_ParentFile(i), EDI_FHC_Content);
			}
		}
	}

	/* Construct dir list */
	cc = 0;
	for(i = 0; i < BIG_MaxDir(); i++)
	{
		change = BIG_DirChanged(i) &~EDI_FHC_Touch &~EDI_FHC_Loaded;
		if(change) cc = 1;
		if(change && (change != EDI_FHC_Deleted))
		{
			EDI_go_FHCDir.SetAt(i, change);
			if(change == EDI_FHC_Delete)
			{
				BIG_DirChanged(i) = EDI_FHC_Deleted;
			}
			else
			{
				BIG_DirChanged(i) = EDI_FHC_None;
				EDI_go_FHCDir.SetAt(BIG_ParentDir(i), EDI_FHC_Content);
			}
		}
	}

	/* Inform all editors */
	if(!cc) LINK_gb_RefreshOnlyFiles = TRUE;
	SendMessageToEditors(EDI_MESSAGE_FATHASCHANGED, 0, 0);
	LINK_gb_RefreshOnlyFiles = FALSE;

	/* Delete lists */
	EDI_go_FHCFile.RemoveAll();
	EDI_go_FHCDir.RemoveAll();

	while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		switch(msg.message)
		{
		case WM_PAINT:
		case WM_ERASEBKGND:
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			break;
		default: goto l_break;
		}
	}

l_break: ;
}

/*
 =======================================================================================================================
    Aim:    Call when there's some engine pointers that have been deleted or added. This is called to inform all
            editors.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::DataHasChanged(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_BaseDialog	*po_Base;
	POSITION			pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!LINK_gb_AllRefreshEnable) return;

	if
	(
		(LINK_gx_PointersJustDeleted.GetCount() == 0)
	&&	(LINK_gx_PointersJustAdded.GetCount() == 0)
	&&	(LINK_gx_PointersJustUpdated.GetCount() == 0)
	) return;

	/* Inform all editors */
	SendMessageToEditors(EDI_MESSAGE_DATAHASCHANGED, 0, 0);

	/* Inform all modless dialogs */
	pos = APP_go_ModelessDia.GetHeadPosition();
	while(pos)
	{
		po_Base = (EDIA_cl_BaseDialog *) CWnd::FromHandle(APP_go_Modeless.GetNext(pos));
		po_Base->i_OnMessage(EDI_MESSAGE_DATAHASCHANGED, 0, 0);
	}

	/* Delete lists */
	LINK_gx_PointersJustDeleted.RemoveAll();
	LINK_gx_PointersJustAdded.RemoveAll();
	LINK_gx_PointersJustUpdated.RemoveAll();
}

/*
 =======================================================================================================================
    Aim:    Returns the view associated with a given number. Useful to loop on all views.
 =======================================================================================================================
 */
EDI_cl_BaseView *EDI_cl_MainFrame::po_GetViewWithNum(int _i_Num)
{
	switch(_i_Num)
	{
	case 0:
		return mpo_MenuView;
	case 1:
		return mpo_LeftView;
	case 2:
		return mpo_RightView;
	case 3:
		return mpo_CornerView;
	case 4:
		return mpo_BottomView;
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Returns the number associated with a given view.
 =======================================================================================================================
 */
int EDI_cl_MainFrame::i_GetNumWithView(EDI_cl_BaseView *_po_View)
{
	if(_po_View == mpo_MenuView) return 0;
	if(_po_View == mpo_LeftView) return 1;
	if(_po_View == mpo_RightView) return 2;
	if(_po_View == mpo_CornerView) return 3;
	if(_po_View == mpo_BottomView) return 4;

	return 0;
}

/*
 =======================================================================================================================
    Aim:    Return instance of an editor corresponding to a given type anc count.
 =======================================================================================================================
 */
EDI_cl_BaseFrame *EDI_cl_MainFrame::po_GetEditorByType(int _i_Type, int _i_Count)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j;
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Search the editor with right count */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);
		for(j = 0; j < mst_Desktop.ast_View[i].i_NumEditors; j++)
		{
			po_Editor = mst_Desktop.ast_View[i].ast_Editors[j].po_Editor;
			if
			(
				(po_Editor)
			&&	(mst_Desktop.ast_View[i].ast_Editors[j].i_Type == _i_Type)
			&&	(mst_Desktop.ast_View[i].ast_Editors[j].i_Cpt == _i_Count)
			) return po_Editor;
		}
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Send message to all editors that are linked to a given one.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SendMessageToLinks
(
	EDI_cl_BaseFrame	*_po_This,
	ULONG				ul_Message,
	ULONG				ul_Param1,
	ULONG				ul_Param2
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!LINK_gb_AllRefreshEnable) return;

_Try_
	for(i = 0; i <= EDI_C_MaxViews; i++)
	{
		/* Test the current pipe editor */
		if(i == EDI_C_MaxViews)
		{
			po_Editor = mpo_PipeFrame;
			if(!po_Editor) break;
			if(!po_Editor->b_CanActivate()) break;
			j = 0;
			goto l_Pipe;
		}

		/* Test all views, and all editors */
		po_View = po_GetViewWithNum(i);
		for(j = 0; j < M_CurNumEd(po_View->mi_NumPane); j++)
		{
			po_Editor = M_CurEd(po_View->mi_NumPane, j).po_Editor;
			if(po_Editor && po_Editor->b_CanActivate())
			{
				if(_po_This->mst_BaseIni.ab_LinkTo[po_Editor->mi_PosInGArray][po_Editor->mi_NumEdit])
				{
l_Pipe:
					switch(ul_Message)
					{
					case EDI_MESSAGE_SELFILE:
						if(!po_Editor->i_OnMessage(EDI_MESSAGE_CANSELFILE, ul_Param1, ul_Param2)) continue;
						break;
					case EDI_MESSAGE_SELDIR:
						if(!po_Editor->i_OnMessage(EDI_MESSAGE_CANSELDIR, ul_Param1, ul_Param2)) continue;
						break;
					case EDI_MESSAGE_SELDATA:
						if(!ul_Param2) continue;
						if(!po_Editor->i_OnMessage(EDI_MESSAGE_CANSELDATA, ul_Param1, ul_Param2)) continue;
						break;
					case EDI_MESSAGE_SELDIRDATA:
						if(!po_Editor->i_OnMessage(EDI_MESSAGE_CANSELDIRDATA, ul_Param1, ul_Param2)) continue;
						break;
					}

					po_Editor->i_OnMessage(ul_Message, ul_Param1, ul_Param2);
				}
			}
		}
	}

_Catch_
_End_
}

/*
 =======================================================================================================================
    Aim:    To send one message to all editors (left & right).
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SendMessageToEditors(ULONG ul_Message, ULONG ul_Param1, ULONG ul_Param2)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < EDI_C_MaxViews; i++)
		po_GetViewWithNum(i)->SendMessageToEditors(ul_Message, ul_Param1, ul_Param2, 1);
	for(i = 0; i < EDI_C_MaxViews; i++)
		po_GetViewWithNum(i)->SendMessageToEditors(ul_Message, ul_Param1, ul_Param2, 2);
}

/*
 =======================================================================================================================
    Aim:    Return the editor that is under a given point.

    In:     pt          Point to test, in screen coordinates.
            _ppo_View   To return the view that corresponds to the editor. 

    Out:    Return a EDI_cl_BaseFrame * on the given editor, or NULL if not.
 =======================================================================================================================
 */
EDI_cl_BaseFrame *EDI_cl_MainFrame::po_EditorUnderPoint(CPoint &pt, EDI_cl_BaseView **_ppo_View)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TC_HITTESTINFO		st_Hit;
	int					i_Res;
	CRect				o_Rect;
	int					i;
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);

		/* Convert point in client coordinates */
		st_Hit.pt = pt;
		po_View->ScreenToClient(&st_Hit.pt);

		/* First in pane header */
		i_Res = po_View->HitTest(&st_Hit);
		if(i_Res != -1)
		{
			*_ppo_View = po_View;
			po_Editor = M_CurEd(po_View->mi_NumPane, po_View->i_NumEditFromPane(i_Res)).po_Editor;
			return po_Editor;
		}

		/* Else in pane body */
		po_View->GetWindowRect(&o_Rect);
		if(o_Rect.PtInRect(pt))
		{
			*_ppo_View = po_View;
			po_Editor = M_CurEd(po_View->mi_NumPane, po_View->i_NumEditFromPane(po_View->GetCurFocus())).po_Editor;
			if(po_Editor && po_Editor->mst_BaseIni.b_IsVisible) return po_Editor;
		}
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Returns the pane view under a given point. Useful for drag & drop.
 =======================================================================================================================
 */
EDI_cl_BaseView *EDI_cl_MainFrame::po_ViewUnderPoint(CPoint &pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	CRect			o_Rect;
	int				i;
	EDI_cl_BaseView *po_View;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);
		po_View->GetWindowRect(o_Rect);
		if(o_Rect.PtInRect(pt)) return po_View;
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Determin if a given key is known for a normal edition.
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::b_EditKey(USHORT _uw_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	c_Key, c_Asc;
	BOOL			b_Shift, b_Ctrl;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Shift = _uw_Key & SHIFT;
	b_Ctrl = _uw_Key & CONTROL;
	c_Key = (char) _uw_Key;

	/* Alt Gr */
	if((GetAsyncKeyState(VK_MENU) < 0) && (GetAsyncKeyState(VK_CONTROL) < 0)) return TRUE;

	/* Printable character */
	if(!b_Ctrl)
	{
		if(c_Key >= '0' && c_Key <= '9') return TRUE;
		c_Asc = MapVirtualKey(c_Key, 2);
		if(L_isprint(c_Asc))
		{
			if(b_Shift && c_Asc == ' ') return FALSE;
			return TRUE;
		}

		if
		(
			c_Asc == (UCHAR) 'é'
		||	c_Asc == (UCHAR) 'è'
		||	c_Asc == (UCHAR) 'à'
		||	c_Asc == (UCHAR) '@'
		||	c_Asc == (UCHAR) 'ç'
		) return TRUE;
		if(c_Key >= VK_NUMPAD0 && c_Key <= VK_DIVIDE) return TRUE;
		if((c_Key >= 0xbc) && (c_Key <= 0xbf)) return TRUE;
	}

	switch(c_Key)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_RIGHT:
	case VK_LEFT:
	case VK_BACK:
		return TRUE;

	case VK_RETURN:
		if(!b_Ctrl && !b_Shift) return TRUE;
		return FALSE;

	case VK_DELETE:
	case VK_INSERT:
		if(!b_Shift) return TRUE;
		break;

	case VK_PRIOR:
	case VK_NEXT:
		if(!b_Ctrl && !b_Shift) return TRUE;
		if(!b_Ctrl && b_Shift) return TRUE;
		break;

	case VK_HOME:
	case VK_END:
		return TRUE;

	case VK_TAB:
		if(!b_Ctrl && !b_Shift) return TRUE;
		break;

	case 'C':
	case 'V':
	case 'Z':
	case 'Y':
	case 'A':
		if(b_Ctrl && !b_Shift) return TRUE;
		break;
	}

	return FALSE;
}

#endif /* ACTIVE_EDITORS */

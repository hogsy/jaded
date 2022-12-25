/*$T BROlistctrl.cpp GC! 1.098 10/19/00 16:04:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BROstrings.h"
#include "BROframe.h"
#include "BROlistctrl.h"
#include "BROmsg.h"
#include "EDImainframe.h"
#include "ENGine/Sources/ENGinit.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "EDIapp.h"
#include "EDIpaths.h"
#include "EDIicons.h"

#include "DATaControl/DATCPerforce.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

#define EDI_M_GetBrowserFrame() ((EBRO_cl_Frame *) (GetParent()->GetParent()->GetParent()))

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EBRO_cl_ListCtrl, CListCtrlStyled)
	ON_WM_SETFOCUS()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_PARENTNOTIFY()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    MESSAGES: COMMANDS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EBRO_cl_ListCtrl::EBRO_cl_ListCtrl(void)
{
	mb_LockSel = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::OnBeginDrag(NMHDR *pNotifyStruct2, LRESULT *result)
{
	NM_LISTVIEW *pNotifyStruct;
	pNotifyStruct = (NM_LISTVIEW *)pNotifyStruct2;

_Try_
	SetFocus();
	EnsureVisible(pNotifyStruct->iItem, FALSE);
	SetItemState(pNotifyStruct->iItem, LVIS_SELECTED, LVIF_STATE);
	Update(pNotifyStruct->iItem);
	EDI_M_GetBrowserFrame()->OnListCtrlBeginDrag(pNotifyStruct->ptAction, pNotifyStruct->iItem);
_Catch_
_End_
}

/*
 =======================================================================================================================
    Aim:    Right button click on header columns. Display a menu to show/hide all columns.
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::OnParentNotify(UINT message, LONG lParam)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CMenu	o_Menu;
	CPoint	o_Point;
	char	asz_Temp[200];
	int		i;
	/*~~~~~~~~~~~~~~~~~~*/

	if(message == WM_RBUTTONDOWN)
	{
		o_Menu.CreatePopupMenu();

		for(i = 1; i < EBRO_C_ListCtrlMaxColumns; i++)
		{
			/* In engine mode, special process */
			if((EDI_M_GetBrowserFrame()->mst_Ini.b_EngineMode) && (i > 1)) continue;
			if((!EDI_M_GetBrowserFrame()->mst_Ini.b_EngineMode) && (i >= EBRO_C_ListCtrlSpecial)) continue;

			L_strcpy(asz_Temp, EBRO_STR_Csz_ListColDsp);
			L_strcat(asz_Temp, EBRO_gasz_ListColNames[i]);
			o_Menu.InsertMenu(i - 1, MF_BYPOSITION, WM_USER + 100 + i, asz_Temp);
			if(EDI_M_GetBrowserFrame()->mst_Ini.ai_NumColListCtrl[i] != -1)
				o_Menu.CheckMenuItem(i - 1, MF_CHECKED | MF_BYPOSITION);
		}

		o_Point.x = LOWORD(lParam);
		o_Point.y = HIWORD(lParam);
		ClientToScreen(&o_Point);
		o_Menu.TrackPopupMenu(TPM_LEFTALIGN, o_Point.x, o_Point.y, EDI_M_GetBrowserFrame());
		return;
	}

	CListCtrlStyled::OnParentNotify(message, lParam);
}

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 * Aim: Compute a string for a given file and list column. In: _ul_File File
 * concerned. _i_NumCol The number of the column. _psz_Out Output string.
 */
static BOOL gb_EngineMode;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_ComputeColName(BIG_INDEX _ul_File, int _i_NumCol, char *_psz_Out)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*psz_Temp;
	struct L_tm			*newtime;
	LINK_tdst_Pointer	*p;
	OBJ_tdst_GameObject *pst_Obj;
	AI_tdst_Instance	*pst_Instance;
	BIG_KEY				ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_i_NumCol)
	{
	/* Name */
	case 0:
		if(gb_EngineMode)
		{
			*_psz_Out = 0;
			p = LINK_p_SearchPointer((void *) _ul_File);
			if(p) L_strcpy(_psz_Out, p->asz_Name);
		}
		else
		{
			L_strcpy(_psz_Out, BIG_NameFile(_ul_File));
			psz_Temp = L_strrchr(_psz_Out, '.');
			if(psz_Temp) *psz_Temp = 0;
		}
		break;

	/* Type */
	case 1:
		if(gb_EngineMode)
		{
			*_psz_Out = 0;
			p = LINK_p_SearchPointer((void *) _ul_File);
			if(p) L_strcpy(_psz_Out, LINK_gast_StructTypes[p->i_Type].psz_Name);
		}
		else
		{
			*_psz_Out = 0;
			psz_Temp = L_strrchr(BIG_NameFile(_ul_File), '.');
			if(psz_Temp) L_strcpy(_psz_Out, psz_Temp + 1);
		}
		break;

	/* Size */
	case 2:
		L_ltoa(BIG_ul_GetLengthFile(BIG_PosFile(_ul_File)), _psz_Out, 10);
		break;

	/* Key */
	case 3:
		sprintf(_psz_Out, "%08x", BIG_FileKey(_ul_File));
		break;

	/* Time */
	case 4:
		{
			time_t* time = &BIG_TimeFile(_ul_File);
			if (time)
			{
				newtime = localtime(time);
		L_strftime(_psz_Out, 255, "%m/%d/%y %H:%M", newtime);
			}
			else
				sprintf(_psz_Out, "Invalid");
		}
		break;

	/* Owner */
	case 5:
		L_strcpy(_psz_Out, BIG_P4OwnerName(_ul_File));
		break;
	/* Loaded */
	case 6:
		*_psz_Out = 0;
		if(BIG_FileChanged(_ul_File) & EDI_FHC_Loaded) L_strcpy(_psz_Out, "Yes");
//		if(LOA_ul_SearchAddress(BIG_PosFile(_ul_File)) != -1) L_strcpy(_psz_Out, "Yes");
		break;

	/* Revision (Perforce) */
	case 7:
		// If local revision is different from server revision, print it
        if (BIG_P4RevisionClient(_ul_File) != BIG_TmpRevisionClient(_ul_File))
        {
            _psz_Out += sprintf(_psz_Out, "bf%d/", BIG_TmpRevisionClient(_ul_File));
        }

		// File not present on P4
		if ( BIG_P4RevisionServer(_ul_File) == 0 && BIG_P4Action(_ul_File)[0] == P4_INVALIDSTRING ) 
		{
			sprintf(_psz_Out, P4_ACTION_NEW);	
			break;
		}
		// File being added/edited/deleted/etc... on P4
		if ( BIG_P4Action(_ul_File)[0] != P4_INVALIDSTRING )
		{
			sprintf(_psz_Out, "#%d/%d - %s", BIG_P4RevisionClient(_ul_File), BIG_P4RevisionServer(_ul_File),BIG_P4Action(_ul_File));
			break;
		}

		// File has been removed by someone on P4
		if ( (strcmp(BIG_P4HeadAction(_ul_File), P4_ACTION_DELETE) == 0) && (BIG_P4RevisionClient(_ul_File) == 0) 
			&& !(BIG_P4RevisionServer(_ul_File) == 0) )
		{
			sprintf(_psz_Out, "#%d/%d - %s", BIG_P4RevisionClient(_ul_File), BIG_P4RevisionServer(_ul_File),P4_ACTION_REMOVE);
			break;
		}
		
		sprintf(_psz_Out, "#%d/%d", BIG_P4RevisionClient(_ul_File), BIG_P4RevisionServer(_ul_File));
		break;
		
	/* AI */
	case 8:
		if(gb_EngineMode)
		{
			*_psz_Out = 0;
			p = LINK_p_SearchPointer((void *) _ul_File);
			if(!p) return;
			if(p->i_Type == LINK_C_ENG_GameObjectOriented)
			{
				pst_Obj = (OBJ_tdst_GameObject *) _ul_File;
				if(pst_Obj->pst_Extended && pst_Obj->pst_Extended->pst_Ai)
				{
					pst_Instance = (AI_tdst_Instance *) pst_Obj->pst_Extended->pst_Ai;
					ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Instance->pst_Model);
					ul_Key = BIG_ul_SearchKeyToFat(ul_Key);
					L_strcpy(_psz_Out, BIG_NameFile(ul_Key));
				}
			}
		}
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int CALLBACK si_CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp1[BIG_C_MaxLenPath];
	char	asz_Temp2[BIG_C_MaxLenPath];
	LONG	l1, l2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	s_ComputeColName(lParam1, lParamSort, asz_Temp1);
	s_ComputeColName(lParam2, lParamSort, asz_Temp2);

	switch(lParamSort)
	{
	case 0: /* Name */
	case 1: /* Type */
	case 4: /* Time */
	case 5: /* Owner */
	case 7: /* Loaded */
		return L_strcmpi(asz_Temp1, asz_Temp2);

	case 6: /* AI presence */
		if(*asz_Temp1 && !(*asz_Temp2)) return -1;
		if(!(*asz_Temp1) && *asz_Temp2) return 1;
		return 0;

	case 2: /* Size */
		l1 = atol(asz_Temp1);
		l2 = atol(asz_Temp2);
		goto l_CompInt;

	case 3: /* Key */
		sscanf(asz_Temp1, "%x", &l1);
		sscanf(asz_Temp2, "%x", &l2);

l_CompInt:
		if(l1 == l2) return 0;
		if(l1 < l2) return -1;
		return 1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::FillFileEngineDatas(EDI_cl_BaseView *_po_MyView, ULONG _ul_ParentDir)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				asz_Path[BIG_C_MaxLenPath];
	BAS_tdst_barray		*ptree;
	int					i, j, i_Col, i_Res;
	LINK_tdst_Pointer	*p;
	LV_ITEM				st_ListCtrlItem;
	char				asz_Temp[BIG_C_MaxLenPath];
	OBJ_tdst_GameObject *pst_Obj;
	int					i_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Size of icon */
	i_Size = 16;
	if(EDI_M_GetBrowserFrame()->mst_Ini.i_ListDispMode == EBRO_C_ListModeIcon)
		i_Size = 32;
	else
		i_Size = 16;

	gb_EngineMode = EDI_M_GetBrowserFrame()->mst_Ini.b_EngineMode;
	BIG_ComputeFullName(_ul_ParentDir, asz_Path);
	if(LINK_gx_TreeToPointers.Lookup(asz_Path, (void * &) ptree))
	{
		for(i = 0; ptree && (i < ptree->num); i++)
		{
			/* Insert pointer name */
			p = (LINK_tdst_Pointer *) ptree->base[i].ul_Val;
			st_ListCtrlItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			st_ListCtrlItem.iItem = 0;
			st_ListCtrlItem.iSubItem = 0;
			st_ListCtrlItem.iImage = M_MF()->i_GetIconImage(EDI_Csz_Path_GameData, p->asz_Name, i_Size);

			/* Special icons for GAO */
			if(st_ListCtrlItem.iImage == EDI_IMAGE_GAO)
			{
				pst_Obj = (OBJ_tdst_GameObject *) ptree->base[i].ul_Key;

				/* AI */
				if(pst_Obj->pst_Extended && pst_Obj->pst_Extended->pst_Ai) st_ListCtrlItem.iImage = EDI_IMAGE_GAOAI;
			}

			st_ListCtrlItem.pszText = p->asz_Name;
			st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
			st_ListCtrlItem.lParam = (LONG) ptree->base[i].ul_Key;
			i_Res = InsertItem(&st_ListCtrlItem);

			/* Insert type */
			i_Col = 1;
			for(j = 1; j < EBRO_C_ListCtrlMaxColumns; j++)
			{
				if(EDI_M_GetBrowserFrame()->mst_Ini.ai_NumColListCtrl[j] != -1)
				{
					if((j > 1) && (j < EBRO_C_ListCtrlSpecial)) continue;
					st_ListCtrlItem.mask = LVIF_TEXT;
					st_ListCtrlItem.iItem = i_Res;
					st_ListCtrlItem.iSubItem = i_Col++;

					s_ComputeColName(ptree->base[i].ul_Key, j, asz_Temp);

					st_ListCtrlItem.pszText = (LPTSTR) asz_Temp;
					st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
					SetItem(&st_ListCtrlItem);
				}
			}
		}
	}

	/* Sort items */
	if((mi_SortColumn > 1) && (mi_SortColumn < EBRO_C_ListCtrlSpecial)) mi_SortColumn = 1;
	SortItems(si_CompareFunc, mi_SortColumn);
}

/*
 =======================================================================================================================
    Aim:    To create hiearchy in list control.

    In:     _po_MyView      Main view of parent.
            _ul_ParentDir   Parent directory.
            _b_VSS          Request for check out infos ?
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::FillFile(EDI_cl_BaseView *_po_MyView, BIG_INDEX _ul_ParentDir, BOOL _b_VSS)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LV_ITEM					st_ListCtrlItem;
	ULONG					ul_File;
	int						i_Res;
	char					asz_Temp[BIG_C_MaxLenPath];
	char					asz_Name[BIG_C_MaxLenPath];
	char					asz_Path[BIG_C_MaxLenPath];
	char					*psz_Temp;
	int						i, i_Col, i_Icon;
	BIG_KEY					ul_Key;
	char					asz_CurrentOwner[BIG_C_MaxLenOwner];
	int						i_Size;
	CString					o_Word;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Refresh VSS or not ? */
	_b_VSS = TRUE; //EDI_M_GetBrowserFrame()->mst_Ini.mst_Options.b_LinkRefresh;

	/* Size of icon */
	i_Size = 16;
	if(EDI_M_GetBrowserFrame()->mst_Ini.i_ListDispMode == EBRO_C_ListModeIcon)
		i_Size = 32;
	else
		i_Size = 16;

	gb_EngineMode = EDI_M_GetBrowserFrame()->mst_Ini.b_EngineMode;
	mpo_MyView = _po_MyView;
	BIG_ComputeFullName(_ul_ParentDir, asz_Path);

	/* Request to get owner of file ? If yes, open the vss file */
	if(_b_VSS && M_MF()->mst_Ini.b_LinkControlON) M_MF()->VssFileChanged(EDI_M_GetBrowserFrame()->mb_NoUpdateVss);
	EDI_M_GetBrowserFrame()->mb_NoUpdateVss = FALSE;

_Try_
	/* All files */
	ul_File = BIG_FirstFile(_ul_ParentDir);
	while(ul_File != BIG_C_InvalidIndex)
	{
		o_Word = BIG_NameFile(ul_File);
		if
		(
			*EDI_M_GetBrowserFrame()->maz_Filter 
		&&	(o_Word.Find(EDI_M_GetBrowserFrame()->maz_Filter) < 0)
		) 
		{
			ul_File = BIG_NextFile(ul_File);
			continue;
		}

		/*
		 * VSS. Determin if a file is currently checked out, and if yes, fill the owner
		 * name field of the file
		 */
		if(_b_VSS && M_MF()->mst_Ini.b_LinkControlON)
		{
			ul_Key = BIG_FileKey(ul_File);
			*asz_CurrentOwner = 0;
			if(ul_Key != BIG_C_InvalidKey)
			{
				SWAP_BIG();
_Try_
				M_MF()->b_UndoCheckOut(ul_Key, asz_CurrentOwner, FALSE);
_Catch_
_End_
				if(!L_strnicmp(asz_CurrentOwner, "_nobody_", 8)) *asz_CurrentOwner = 0;
				SWAP_BIG();
			}

			/* Isolate user name part */
			if(*asz_CurrentOwner)
				L_strcpy(BIG_P4OwnerName(ul_File), asz_CurrentOwner);
			else
				*BIG_P4OwnerName(ul_File) = 0;
		}


		/* No extension on name */
		i_Icon = M_MF()->i_GetIconImage(asz_Path, BIG_NameFile(ul_File), i_Size);
		L_strcpy(asz_Name, BIG_NameFile(ul_File));
		psz_Temp = L_strrchr(asz_Name, '.');
		if(psz_Temp) *psz_Temp = 0;

		/* Item (file) */
		st_ListCtrlItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		st_ListCtrlItem.iItem = 0;
		st_ListCtrlItem.iSubItem = 0;
		st_ListCtrlItem.pszText = asz_Name;
		st_ListCtrlItem.iImage = i_Icon;
		st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
		st_ListCtrlItem.lParam = ul_File;
		i_Res = InsertItem(&st_ListCtrlItem);

		/* Columns */
		i_Col = 1;
		for(i = 1; i < EBRO_C_ListCtrlMaxColumns; i++)
		{
			if(EDI_M_GetBrowserFrame()->mst_Ini.ai_NumColListCtrl[i] != -1)
			{
				st_ListCtrlItem.mask = LVIF_TEXT;
				st_ListCtrlItem.iItem = i_Res;
				st_ListCtrlItem.iSubItem = i_Col++;

				s_ComputeColName(ul_File, i, asz_Temp);

				st_ListCtrlItem.pszText = (LPTSTR) asz_Temp;
				st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
				SetItem(&st_ListCtrlItem);
			}
		}

		/* Pass to next file */
		ul_File = BIG_NextFile(ul_File);
	}

_Catch_
_EndThrow_

	/* Close VSS file */
	if(_b_VSS && M_MF()->mst_Ini.b_LinkControlON) M_MF()->CloseVssFile();

	/* Sort items */
	SortItems(si_CompareFunc, mi_SortColumn);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::OnColumnClick(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~*/
	int i, i_Col;
	/*~~~~~~~~~*/
	NM_LISTVIEW *pNotifyStruct;
	pNotifyStruct = (NM_LISTVIEW *)pNotifyStruct2;

	gb_EngineMode = EDI_M_GetBrowserFrame()->mst_Ini.b_EngineMode;

	mi_SortColumn = pNotifyStruct->iSubItem;
	i_Col = 1;
	for(i = 1; i < EBRO_C_ListCtrlMaxColumns; i++)
	{
		if(gb_EngineMode && (i > 1) && (i < EBRO_C_ListCtrlSpecial)) continue;
		if(EDI_M_GetBrowserFrame()->mst_Ini.ai_NumColListCtrl[i] != -1)
		{
			if(i_Col == mi_SortColumn)
			{
				mi_SortColumn = i;
				break;
			}

			i_Col++;
		}
	}

	EDI_M_GetBrowserFrame()->mst_Ini.i_SortColumnListCtrl = mi_SortColumn;
	SortItems(si_CompareFunc, mi_SortColumn);
}

/*
 =======================================================================================================================
    Aim:    Cause display is not the same as the real name, retreive item text must retreive the real name of the file
            (in the display, the extension of the file is not in the name).
 =======================================================================================================================
 */
CString EBRO_cl_ListCtrl::GetItemText(int _i_Item, int)
{
	return BIG_NameFile(GetItemData(_i_Item));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::OnSetFocus(CWnd *pold)
{
	EDI_M_GetBrowserFrame()->RefreshMenu();
	SetFocus();
	CListCtrlStyled::OnSetFocus(pold);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::OnLButtonDblClk(UINT n, CPoint pt)
{
	CListCtrlStyled::OnLButtonDblClk(n, pt);
	if(HitTest(pt) != -1) EDI_M_GetBrowserFrame()->OnListCtrlDblClk(HitTest(pt));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::OnLButtonDown(UINT n, CPoint pt)
{
	SetFocus();
	CListCtrlStyled::OnLButtonDown(n, pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::OnRButtonDown(UINT n, CPoint pt)
{
	CListCtrlStyled::OnLButtonDown(n, pt);
	ClientToScreen(&pt);
	::SetFocus(this->m_hWnd);
	EDI_M_GetBrowserFrame()->OnCtrlPopup(pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::OnItemChanged(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	static BOOL b_Rec = FALSE;
	POINT		pt, pt1;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	NM_LISTVIEW *pNotifyStruct;
	pNotifyStruct = (NM_LISTVIEW *)pNotifyStruct2;

	if(b_Rec) return;
	b_Rec = TRUE;
	if((pNotifyStruct->uNewState & LVIS_FOCUSED) && !mb_LockSel)
	{
		GetCursorPos(&pt);
		ScreenToClient(&pt);

		EDI_M_GetBrowserFrame()->OnListCtrlItemSel(pNotifyStruct->iItem);

		GetCursorPos(&pt1);
		ScreenToClient(&pt1);
		if((pt.x != pt1.x) || (pt.y != pt1.y)) PostMessage(WM_LBUTTONUP, 0, pt.x + (pt.y << 16));
	}

	b_Rec = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_ListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	/*~~*/
	int i;
	/*~~*/

	if(GetAsyncKeyState(VK_CONTROL) < 0)
	{
		switch(nChar)
		{
		case 'A':
			for(i = 0; i < GetItemCount(); i++)
				SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			return;
		}
	}

	CListCtrlStyled::OnKeyDown(nChar, nRepCnt, nFlags);
}

#endif /* ACTIVE_EDITORS */

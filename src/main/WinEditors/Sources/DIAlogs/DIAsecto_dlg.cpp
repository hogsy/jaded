/*$T DIAsecto_dlg.cpp GC 1.134 06/08/04 13:34:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "DIAlogs/DIAsecto_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "DIAlogs/DIAlist_dlg.h"
#include "LINks/LINKtoed.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/ENGcall.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "EDIapp.h"
#include "LINks/LINKmsg.h"
#include "EDImsg.h"

#ifdef JADEFUSION
#include "DIAlogs/DIAfile_dlg.h"
#include "EDIstrings.h"
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLIerrid.h"
#endif

extern EDIA_cl_SectoDialog	*gspo_SectoDialog;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_SectoDialog, EDIA_cl_BaseDialog)
	ON_LBN_SELCHANGE(IDC_LIST_SECT, OnSelSect)
	ON_LBN_DBLCLK(IDC_LIST_SECT, OnSectDblClk)
	ON_COMMAND(IDC_CHECK_AV, OnBnClick)
	ON_COMMAND(IDC_CHECK_NV, OnBnClick)
	ON_COMMAND(IDC_CHECK_AA, OnBnClick)
	ON_COMMAND(IDC_CHECK_NA, OnBnClick)
#ifdef JADEFUSION
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnBnExport)
#endif
	ON_COMMAND(IDCOPY, OnCopy)
	ON_COMMAND(IDPASTE, OnPaste)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SectoDialog::EDIA_cl_SectoDialog(struct WOR_tdst_World_ *W) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_SECTO)
{
	mpst_World = W;
	mb_SpecSort = FALSE;
	mi_SpecSort = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SectoDialog::~EDIA_cl_SectoDialog(void)
{
	gspo_SectoDialog = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SectoDialog::OnInitDialog(void)
{
#ifdef JADEFUSION
	/*~~~~~~~~~~~~~~~~~*/
	CListCtrl	*po_Obj;
	CRect		o_Rect;
	/*~~~~~~~~~~~~~~~~~*/

	CenterWindow();

	po_Obj = (CListCtrl *) GetDlgItem(IDC_LIST_OBJ);
	if(po_Obj)
	{
		po_Obj->SetImageList(&(M_MF()->mo_FileImageList), LVSIL_SMALL);

		po_Obj->GetWindowRect(&o_Rect);
		po_Obj->InsertColumn(0, "Object");
		po_Obj->SetColumnWidth(0, o_Rect.Width() - 20);
		po_Obj->InsertColumn(1, "Warnings Number");
		po_Obj->SetColumnWidth(1, 20);
	}
#else
	CenterWindow();
#endif
	return  EDIA_cl_BaseDialog::OnInitDialog();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 500;
	lpMMI->ptMinTrackSize.y = 400;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnSize(UINT n, int x, int y)
{
	/*~~~~~~~~~~~*/
    int h, w, h2;
	/*~~~~~~~~~~~*/
    
	EDIA_cl_BaseDialog::OnSize(n, x, y);

    if ( !GetDlgItem( IDC_STATIC_SECT ) ) return;

	M_MF()->LockDisplay( this );

    h = y - 60;
	h2 = h / 3;
    w = (x - 20) / 3;

    GetDlgItem( IDC_STATIC_SECT)->MoveWindow( 5, 7, w, 16 );
    GetDlgItem( IDC_LIST_SECT )->MoveWindow( 5, 25, w, h - 20 );

	GetDlgItem( IDC_STATIC_VIS)->MoveWindow( 10 + w, 7, w, 16 );
    GetDlgItem( IDC_LIST_VIS )->MoveWindow( 10 + w, 25, w, h2 - 25 );

	GetDlgItem( IDC_STATIC_ACT)->MoveWindow( 10 + w, 7 + h2, w, 16 );
    GetDlgItem( IDC_LIST_ACT )->MoveWindow( 10 + w, 25 + h2, w, h2 - 25 );

	GetDlgItem( IDC_STATIC_PORTAL)->MoveWindow( 10 + w, 7 + (2*h2), w, 16 );
    GetDlgItem( IDC_LIST_PORTAL )->MoveWindow( 10 + w, 25 + (2*h2), w, h-20 - (2*h2)); //h2 - 25 );
	
	GetDlgItem( IDC_STATIC_OBJ)->MoveWindow( 15 + (2*w), 7, w, 16 );
    GetDlgItem( IDC_LIST_OBJ )->MoveWindow( 15 + (2*w), 25, w, h - 20 );

	GetDlgItem( IDC_CHECK_AV )->MoveWindow( 5 , y - 45, 100, 16 );
	GetDlgItem( IDC_CHECK_NV )->MoveWindow( 5 , y - 25, 100, 16 );
	GetDlgItem( IDC_CHECK_AA )->MoveWindow( 105 , y - 45, 100, 16 );
	GetDlgItem( IDC_CHECK_NA )->MoveWindow( 105 , y - 25, 100, 16 );

#ifdef JADEFUSION
// POPOWARNING
//	GetDlgItem( IDC_OBJ_WARNING )->MoveWindow( 15 + (2*w) + 50, 7, 120, 16 );
#endif

	GetDlgItem( IDCOPY )->MoveWindow( x - 250, y - 30, 80, 20 );
	GetDlgItem( IDPASTE )->MoveWindow( x - 170 , y - 30, 80, 20 );
	GetDlgItem( IDC_BUTTON_EXPORT )->MoveWindow( x - 90, y - 30, 80, 20 );

    M_MF()->UnlockDisplay( this );
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SectoDialog::PreTranslateMessage(MSG *pMsg)
{
	if(mpst_World)
	{
		if((pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem(IDC_LIST_SECT)->GetSafeHwnd()))
		{
			OnMenuSect();
			return TRUE;
		}

		if((pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem(IDC_LIST_VIS)->GetSafeHwnd()))
		{
			OnMenuSectVis();
			return TRUE;
		}

		if((pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem(IDC_LIST_ACT)->GetSafeHwnd()))
		{
			OnMenuSectAct();
			return TRUE;
		}

		if((pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem(IDC_LIST_OBJ)->GetSafeHwnd()))
		{
			OnMenuSectObj();
			return TRUE;
		}

		if((pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem(IDC_LIST_PORTAL)->GetSafeHwnd()))
		{
			OnMenuSectPortal();
			return TRUE;
		}
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::DoDataExchange(CDataExchange *pDX)
{
	FillListSect();
	EDIA_cl_BaseDialog::DoDataExchange(pDX);
}

#ifdef JADEFUSION
extern ENG_gb_EngineRunning;
#else
extern "C" BOOL ENG_gb_EngineRunning;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::SortSecto(CList<int, int> &lst)
{
	int	i, j, first;
	int next[255];

	if(!mb_SpecSort) return;
	memset(next, 0, sizeof(next));

	first = mi_SpecSort;
	lst.AddTail(first);
	next[first] = 1;
recom:
	for(i = 1; mpst_World && i < WOR_C_MaxSecto; i++)
	{
		if(i == first) continue;
		if(lst.Find(i)) continue;
		if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)
		{
			for(j = 0; j < WOR_C_MaxSectoPortals; j++)
			{
				if(mpst_World->ast_AllSectos[i].ast_Portals[j].uw_Flags & WOR_CF_PortalValid)
				{
					if(mpst_World->ast_AllSectos[i].ast_Portals[j].uw_Flags & WOR_CF_PortalShare)
					{
						if(!lst.Find(mpst_World->ast_AllSectos[i].ast_Portals[j].uc_ShareSect))
						{
							lst.AddTail(mpst_World->ast_AllSectos[i].ast_Portals[j].uc_ShareSect);
							first = mpst_World->ast_AllSectos[i].ast_Portals[j].uc_ShareSect;
							next[first] = 1;
							goto recom;
						}
					}
				}
			}
		}
	}

	for(i = 1; mpst_World && i < WOR_C_MaxSecto; i++)
	{
		if(lst.Find(i)) continue;
		if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid) lst.AddTail(i + 1000);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::FillListSect(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int			i, index;
	int			sel, rank;
	CString		str;
	CListBox	*po_Box;
	CList<int, int> lst;
	POSITION	pos;
	int			bidon;
	/*~~~~~~~~~~~~~~~~~*/

	po_Box = (CListBox *) GetDlgItem(IDC_LIST_SECT);
	po_Box->SetRedraw(FALSE);
	sel = po_Box->GetCurSel();
	po_Box->ResetContent();

	SortSecto(lst);
	if(lst.IsEmpty())
	{
		for(i = 1; mpst_World && i < WOR_C_MaxSecto; i++)
		{
			if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)
			{
				str.Format("%3d - %s", i, mpst_World->ast_AllSectos[i].az_Name);
				if(i == mpst_World->ul_CurrentSector)
				{
					str += " [Current]";
					if(ENG_gb_EngineRunning)
						sel = i - 1;
				}
				else
				{
					if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorActive) str += " [A]";
					if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorVisible) str += " [V]";
				}

				index = po_Box->AddString(str);
				po_Box->SetItemData(index, i);
			}
		}
	}
	else
	{
		pos = lst.GetHeadPosition();
		rank = 0;
		while(pos)
		{
			i = lst.GetNext(pos);
			bidon = 0;
			if(i >= 1000) { bidon = 1; i -= 1000; }
			if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)
			{
				if(bidon)
					str.Format("(%3d) ALONE %3d - %s", rank++, i, mpst_World->ast_AllSectos[i].az_Name);
				else
					str.Format("(%3d) %3d - %s", rank++, i, mpst_World->ast_AllSectos[i].az_Name);
				if(i == mpst_World->ul_CurrentSector)
				{
					str += " [Current]";
					if(ENG_gb_EngineRunning)
						sel = i - 1;
				}
				else
				{
					if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorActive) str += " [A]";
					if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorVisible) str += " [V]";
				}

				index = po_Box->AddString(str);
				po_Box->SetItemData(index, i);
			}
		}
	}

	if(sel >= po_Box->GetCount()) sel = po_Box->GetCount() - 1;
	if(sel != -1) po_Box->SetCurSel(sel);
	OnSelSect();
	po_Box->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::UpdateFlags(BOOL sens)
{
	/*~~~~~~~~~~~~~~~~~*/
	int			sect;
	CListBox	*po_Sect;
	ULONG		flags;
	/*~~~~~~~~~~~~~~~~~*/

	po_Sect = (CListBox *) GetDlgItem(IDC_LIST_SECT);
	sect = po_Sect->GetCurSel();
	if(sect != -1) sect = po_Sect->GetItemData(sect);
	if(!sens)
	{
		if(sect == -1)
			flags = 0;
		else
			flags = mpst_World->ast_AllSectos[sect].ul_Flags;
		((CButton *) GetDlgItem(IDC_CHECK_AV))->SetCheck(flags & WOR_CF_SectorAlwaysVisible ? TRUE : FALSE);
		((CButton *) GetDlgItem(IDC_CHECK_NV))->SetCheck(flags & WOR_CF_SectorNeverVisible ? TRUE : FALSE);
		((CButton *) GetDlgItem(IDC_CHECK_AA))->SetCheck(flags & WOR_CF_SectorAlwaysActive ? TRUE : FALSE);
		((CButton *) GetDlgItem(IDC_CHECK_NA))->SetCheck(flags & WOR_CF_SectorNeverActive ? TRUE : FALSE);
	}
	else
	{
		if(sect == -1) return;
		mpst_World->ast_AllSectos[sect].ul_Flags &= ~WOR_CF_SectorAlwaysVisible;
		mpst_World->ast_AllSectos[sect].ul_Flags &= ~WOR_CF_SectorNeverVisible;
		mpst_World->ast_AllSectos[sect].ul_Flags &= ~WOR_CF_SectorAlwaysActive;
		mpst_World->ast_AllSectos[sect].ul_Flags &= ~WOR_CF_SectorNeverActive;
		if(((CButton *) GetDlgItem(IDC_CHECK_AV))->GetCheck())
			mpst_World->ast_AllSectos[sect].ul_Flags |= WOR_CF_SectorAlwaysVisible;
		if(((CButton *) GetDlgItem(IDC_CHECK_NV))->GetCheck())
			mpst_World->ast_AllSectos[sect].ul_Flags |= WOR_CF_SectorNeverVisible;
		if(((CButton *) GetDlgItem(IDC_CHECK_AA))->GetCheck())
			mpst_World->ast_AllSectos[sect].ul_Flags |= WOR_CF_SectorAlwaysActive;
		if(((CButton *) GetDlgItem(IDC_CHECK_NA))->GetCheck())
			mpst_World->ast_AllSectos[sect].ul_Flags |= WOR_CF_SectorNeverActive;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnBnClick(void)
{
	UpdateFlags(TRUE);
}
#ifdef JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnBnExport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Enter the file name", 1);
	CString				strFileName, strWorldName, strSectorName, strGaoName, strLine;
	char				asz_FullPath[BIG_C_MaxLenPath];
	L_FILE				x_File;
	char				*psz_Temp;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
	OBJ_tdst_GameObject	*pst_GAO;
	CList<int, int>		ListSectorIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		/* Get file name */
		o_File.GetItem(o_File.mo_File, 1, strFileName);
		psz_Temp = (char *) (LPCSTR) strFileName;

		/* Compute full name on disk of new file */
		L_strcpy(asz_FullPath, o_File.masz_FullPath);
		if(asz_FullPath[L_strlen(asz_FullPath) - 1] != '\\') L_strcat(asz_FullPath, "\\");
		L_strcat(asz_FullPath, psz_Temp);

		/* Check if file already exists */
		if(!L_access(asz_FullPath, 0))
			if(MessageBox(EDI_STR_Csz_ConfirmOverwrite, EDI_STR_Csz_TitleConfirm, MB_YESNO | MB_ICONQUESTION) == IDNO)
				return;

		//Create and open file
		x_File = L_fopen(asz_FullPath, L_fopen_WB);
		ERR_X_Error(CLI_FileOpen(x_File), L_ERR_Csz_FOpen, asz_FullPath);

		//write text to file
		pst_AIEOT = &(mpst_World->st_AllWorldObjects);
		
		//get world name without extension
		strWorldName = mpst_World->sz_Name;
		strWorldName.Replace(EDI_Csz_ExtWorld, "");
		strWorldName.Replace(EDI_Csz_ExtWorldList, "");

		//for each sectors
		for(int i = 1; mpst_World && i < WOR_C_MaxSecto; i++)
		{
			if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)
			{	
				//add sector index
				ListSectorIndex.AddTail(i);

				//get sector name
				strSectorName = mpst_World->ast_AllSectos[i].az_Name;

				pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
				pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);

				//for each objects
				for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
				{
					pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
					if(TAB_b_IsAHole(pst_GAO))	continue;
					if(!pst_GAO->pst_Extended)	continue;
					if
						(
							(pst_GAO->pst_Extended->auc_Sectos[0] == i)
						||	(pst_GAO->pst_Extended->auc_Sectos[1] == i)
						||	(pst_GAO->pst_Extended->auc_Sectos[2] == i)
						||	(pst_GAO->pst_Extended->auc_Sectos[3] == i)
						)
					{
						//get object name
						strGaoName = pst_GAO->sz_Name;
						strLine = strWorldName + "\t" + strSectorName + "\t" + strGaoName + "\n";
						L_fwrite((char *) (LPCSTR)strLine, strLine.GetLength(), 1, x_File);			
					}
				}		
			}
		}
		
		//add all gao that is not in a Sector
		//for each sectors
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);

		//for each objects
		for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GAO))	continue;
			if(!pst_GAO->pst_Extended)	continue;
			if
				(
					(ListSectorIndex.Find(pst_GAO->pst_Extended->auc_Sectos[0]))
				||	(ListSectorIndex.Find(pst_GAO->pst_Extended->auc_Sectos[1]))
				||	(ListSectorIndex.Find(pst_GAO->pst_Extended->auc_Sectos[2]))
				||	(ListSectorIndex.Find(pst_GAO->pst_Extended->auc_Sectos[3]))
				)
			{
				continue;
			}
			strGaoName = pst_GAO->sz_Name;
			strLine = strWorldName + "\t" + "not in any Sectors" + "\t" + strGaoName + "\n";
			L_fwrite((char *) (LPCSTR)strLine, strLine.GetLength(), 1, x_File);
		}
	
		//close file
		if(CLI_FileOpen(x_File))
			L_fclose(x_File);
	}
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnMenuSect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu		o_SubMenu(FALSE);
	CPoint				o_Point;
	int					i_Res, sel;
	int					i;
	EDIA_cl_NameDialog	o_Id("Enter ID");
	EDIA_cl_NameDialog	o_Name("Enter Name");
	int					sect;
	CListBox			*po_Sect;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Sect = (CListBox *) GetDlgItem(IDC_LIST_SECT);
	sel = sect = po_Sect->GetCurSel();
	if(sect != -1) sect = po_Sect->GetItemData(sect);

	GetCursorPos(&o_Point);
	M_MF()->InitPopupMenuAction(NULL, &o_SubMenu);
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 1, TRUE, "New Sector");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 2, TRUE, "Rename Sector");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 3, TRUE, "Delete Sector");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 5, TRUE, "Set As Current");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 6, TRUE, "Reset Current");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 10, TRUE, "Colorize");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 11, TRUE, "Reset Colorize");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 12, TRUE, "Show Portals");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 13, TRUE, "Hide Portals");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 22, TRUE, "Show Objects");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 21, TRUE, "Hide Objects");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 666, TRUE, "Sort by portals");

	i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Point, &o_SubMenu);
	switch(i_Res)
	{
	case 666:
		mb_SpecSort = mb_SpecSort ? FALSE : TRUE;
		FillListSect();
		break;

	case 1:
		/* Get first valid id */
		for(i = 1; i < WOR_C_MaxSecto; i++)
		{
			if(!(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)) break;
		}

		/* Id */
		ERR_X_Error(i != WOR_C_MaxSecto, "Too many defined sectors", NULL);
		o_Id.mo_Name.Format("%d", i);
		if(o_Id.DoModal() != IDOK) return;

		i = L_atoi((char *) (LPCSTR) o_Id.mo_Name);
		if(i <= 0 || i >= WOR_C_MaxSecto)
		{
			ERR_X_Error(0, "Bad sector ID", NULL);
			return;
		}
		if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)
		{
			ERR_X_Error(0, "Sector ID already exists", NULL);
			return;
		}

		/* Name */
		if(o_Name.DoModal() != IDOK) return;
		L_memset(&mpst_World->ast_AllSectos[i], 0, sizeof(WOR_tdst_Secto));
		strncpy(mpst_World->ast_AllSectos[i].az_Name, (char *) (LPCSTR) o_Name.mo_Name, WOR_C_MaxLenNameSecto);
		mpst_World->ast_AllSectos[i].ul_Flags = WOR_CF_SectorValid;
		FillListSect();
		break;

	/* Rename */
	case 2:
		if(sect == -1) break;
		o_Name.mo_Name = mpst_World->ast_AllSectos[sect].az_Name;
		if(o_Name.DoModal() != IDOK) return;
		strncpy(mpst_World->ast_AllSectos[sect].az_Name, (char *) (LPCSTR) o_Name.mo_Name, WOR_C_MaxLenNameSecto);
		FillListSect();
		break;

	/* Delete */
	case 3:
		if(sect == -1) break;
		if(MessageBox("This will delete sector. Do you want to continue ?", "Please confirm", MB_YESNO) != IDYES)
			break;
		mpst_World->ast_AllSectos[sect].ul_Flags &= ~WOR_CF_SectorValid;
		po_Sect->SetRedraw(FALSE);
		FillListSect();
		if(sel == po_Sect->GetCount()) sel--;
		if(sel >= 0)
		{
			po_Sect->SetCurSel(sel);
			OnSelSect();
		}

		/* Remove objects */
		pst_AIEOT = &(mpst_World->st_AllWorldObjects);
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
		for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GAO)) continue;
			if(!pst_GAO->pst_Extended) continue;
			if(pst_GAO->pst_Extended->auc_Sectos[0] == sect) pst_GAO->pst_Extended->auc_Sectos[0] = 0;
			if(pst_GAO->pst_Extended->auc_Sectos[1] == sect) pst_GAO->pst_Extended->auc_Sectos[1] = 0;
			if(pst_GAO->pst_Extended->auc_Sectos[2] == sect) pst_GAO->pst_Extended->auc_Sectos[2] = 0;
			if(pst_GAO->pst_Extended->auc_Sectos[3] == sect) pst_GAO->pst_Extended->auc_Sectos[3] = 0;
		}

		po_Sect->SetRedraw(TRUE);
		DeleteSharePortalTo(sect, -1);
		break;

	/* Set As Current */
	case 5:
		if(sect == -1) break;
		WOR_SetCurrentSector(mpst_World, sect, TRUE);
		FillListSect();
		break;

	/* Reset current */
	case 6:
		if(sect == -1) break;
		WOR_SetCurrentSector(mpst_World, 0, TRUE);
		FillListSect();
		break;

	/* Colorize */
	case 10:
		ColorizeSel();
		break;

	/* Hide colorize */
	case 11:
		for(i = 1; i < WOR_C_MaxSecto; i++)
		{
			mpst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor1;
			mpst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor2;
			mpst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor3;
		}
		break;

	/* Show Portals */
	case 12:
		if(sect == -1) break;
		for(i = 0; i < WOR_C_MaxSectoPortals; i++)
		{
			mpst_World->ast_AllSectos[sect].ast_Portals[i].uw_Flags |= WOR_CF_PortalRender;
		}
		break;

	/* Hide Portals */
	case 13:
		if(sect == -1) break;
		for(i = 0; i < WOR_C_MaxSectoPortals; i++)
		{
			mpst_World->ast_AllSectos[sect].ast_Portals[i].uw_Flags &= ~WOR_CF_PortalRender;
		}
		break;

	/* Hide */
	case 21:
		if(sect == -1) break;
		pst_AIEOT = &(mpst_World->st_AllWorldObjects);
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
		for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GAO)) continue;
			if(!pst_GAO->pst_Extended) continue;
			if
			(
				(pst_GAO->pst_Extended->auc_Sectos[0] == sect)
			||	(pst_GAO->pst_Extended->auc_Sectos[1] == sect)
			||	(pst_GAO->pst_Extended->auc_Sectos[2] == sect)
			||	(pst_GAO->pst_Extended->auc_Sectos[3] == sect)
			)
			{
				pst_GAO->ul_EditorFlags |= OBJ_C_EditFlags_Hidden;
			}
		}
		break;

	/* UnHide */
	case 22:
		if(sect == -1) break;
		pst_AIEOT = &(mpst_World->st_AllWorldObjects);
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
		for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GAO)) continue;
			if(!pst_GAO->pst_Extended) continue;
			if
			(
				(pst_GAO->pst_Extended->auc_Sectos[0] == sect)
			||	(pst_GAO->pst_Extended->auc_Sectos[1] == sect)
			||	(pst_GAO->pst_Extended->auc_Sectos[2] == sect)
			||	(pst_GAO->pst_Extended->auc_Sectos[3] == sect)
			)
			{
				if (!(pst_GAO->ul_AdditionalFlags & OBJ_C_EdAddFlags_HiddenByWowFilter))
					pst_GAO->ul_EditorFlags &= ~OBJ_C_EditFlags_Hidden;
			}
		}
		break;
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnSelSect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*po_Box;
	CListBox			*po_Vis;
	CListBox			*po_Act;
	CListBox			*po_Port;
	int					sel, i, index;
	int					rank;
	CString				str;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;

#ifdef JADEFUSION
	CListCtrl			*po_Obj;
	int					i_bmp, i_count, i_lights_num;
	CRect				o_Rect;

	//counter for gao with too many lights
	i_count = 0;
	i_lights_num = 0;
#else
	CListBox			*po_Obj;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Box = (CListBox *) GetDlgItem(IDC_LIST_SECT);

	po_Vis = (CListBox *) GetDlgItem(IDC_LIST_VIS);
	po_Act = (CListBox *) GetDlgItem(IDC_LIST_ACT);
	po_Port = (CListBox *) GetDlgItem(IDC_LIST_PORTAL);
#ifdef JADEFUSION
	po_Obj = (CListCtrl *) GetDlgItem(IDC_LIST_OBJ);
#else
	po_Obj = (CListBox *) GetDlgItem(IDC_LIST_OBJ);
#endif

	po_Vis->SetRedraw(FALSE);
	po_Act->SetRedraw(FALSE);
	po_Port->SetRedraw(FALSE);
	po_Obj->SetRedraw(FALSE);

	po_Vis->ResetContent();
	po_Act->ResetContent();
	po_Port->ResetContent();
#ifdef JADEFUSION
	po_Obj->DeleteAllItems();
#else
	po_Obj->ResetContent();
#endif
	sel = po_Box->GetCurSel();
	if(sel != -1)
	{
		sel = po_Box->GetItemData(sel);

		/* Vis */
		for(i = 0; i < WOR_C_MaxSectoRef; i++)
		{
			rank = mpst_World->ast_AllSectos[sel].auc_RefVis[i];
			if(rank)
			{
				str.Format("%3d - %s", rank, mpst_World->ast_AllSectos[rank].az_Name);
				index = po_Vis->AddString(str);
				po_Vis->SetItemData(index, i);
			}
		}

		/* Act */
		for(i = 0; i < WOR_C_MaxSectoRef; i++)
		{
			rank = mpst_World->ast_AllSectos[sel].auc_RefAct[i];
			if(rank)
			{
				str.Format("%3d - %s", rank, mpst_World->ast_AllSectos[rank].az_Name);
				index = po_Act->AddString(str);
				po_Act->SetItemData(index, i);
			}
		}

		/* Portals */
		for(i = 0; i < WOR_C_MaxSectoPortals; i++)
		{
			if(mpst_World->ast_AllSectos[sel].ast_Portals[i].uw_Flags & WOR_CF_PortalValid)
			{
				str = mpst_World->ast_AllSectos[sel].ast_Portals[i].az_Name;
				if(mpst_World->ast_AllSectos[sel].ast_Portals[i].uw_Flags & WOR_CF_PortalShare) str += " [Share]";
				index = po_Port->AddString(str);
				po_Port->SetItemData(index, i);
			}
		}

		/* Objects */
		pst_AIEOT = &(mpst_World->st_AllWorldObjects);
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
#ifdef JADEFUSION
		for(i = 0,pst_CurrentElem; pst_CurrentElem && pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
#else
		for(pst_CurrentElem; pst_CurrentElem && pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
#endif
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GAO)) continue;
			if(!pst_GAO->pst_Extended) continue;
			if
			(
				(pst_GAO->pst_Extended->auc_Sectos[0] == sel)
			||	(pst_GAO->pst_Extended->auc_Sectos[1] == sel)
			||	(pst_GAO->pst_Extended->auc_Sectos[2] == sel)
			||	(pst_GAO->pst_Extended->auc_Sectos[3] == sel)
			)
			{
#ifdef JADEFUSION
				i_bmp = -1;
				i_lights_num = 0;
				if(pst_GAO->ul_AdditionalFlags & OBJ_C_EdAddFlags_TooManyLights)
				{
					pst_GAO->ul_AdditionalFlags &= ~(OBJ_C_EdAddFlags_TooManyLights);
					i_bmp = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicLight, 0);
					i_count++;

					//count lights num
					for(; i_lights_num < VS_MAX_LIGHTS; i_lights_num++)
						if(!pst_GAO->apst_IsLightBy[i_lights_num])
							break;
				}
				index = po_Obj->InsertItem(i, pst_GAO->sz_Name, i_bmp);
				
				if(i_lights_num)
				{
					str.Format(_T("%d"), i_lights_num);
					po_Obj->SetItemText(index, 1, str);
				}

#else
				index = po_Obj->AddString(pst_GAO->sz_Name);
#endif
				po_Obj->SetItemData(index, (int) pst_GAO);
#ifdef JADEFUSION			
				i++;
#endif
			}
		}
#ifdef JADEFUSION
		//update Warning message
		if(i_count)
		{
			str.Format(_T("%d warning"), i_count);
			if(i_count > 1)
				str+= "s";
		}
		else
			str = "";
//POPOWARNING
//		GetDlgItem(IDC_OBJ_WARNING)->SetWindowText(str);
#endif
	}

	po_Vis->SetRedraw(TRUE);
	po_Act->SetRedraw(TRUE);
	po_Port->SetRedraw(TRUE);
	po_Obj->SetRedraw(TRUE);
	UpdateFlags(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::RefSetMenu(CListBox *po_List, UCHAR *pwhat, UCHAR *pwhat1)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu		o_SubMenu(FALSE);
	CPoint				o_Point;
	int					i_Res, i, sel;
	EDIA_cl_ListDialog	o_Lst(EDIA_List_SpeedSelectSector);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&o_Point);
	M_MF()->InitPopupMenuAction(NULL, &o_SubMenu);
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 1, TRUE, "New Reference");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 2, TRUE, "Delete Reference");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	if(po_List == (CListBox *) GetDlgItem(IDC_LIST_VIS))
		M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 3, TRUE, "Synchronize With Activation");
	else
		M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 3, TRUE, "Synchronize With Visibility");
	i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Point, &o_SubMenu);
	switch(i_Res)
	{
	case 1:
		o_Lst.mpst_World = mpst_World;
		if(o_Lst.DoModal() != IDOK) return;

		/* Already here ? */
		for(i = 0; i < WOR_C_MaxSectoRef; i++)
		{
			if(pwhat[i] == (UCHAR) o_Lst.mul_Model) return;
		}

		/* New place */
		for(i = 0; i < WOR_C_MaxSectoRef; i++)
		{
			if(!pwhat[i]) break;
		}

		if(i == WOR_C_MaxSectoRef)
		{
			ERR_X_Error(0, "Two many dependencies", NULL);
			return;
		}

		pwhat[i] = (UCHAR) o_Lst.mul_Model;
		OnSelSect();
		break;

	/* Delete reference */
	case 2:
		sel = po_List->GetCurSel();
		if(sel == -1) return;
		sel = po_List->GetItemData(sel);
		for(i = 0; i < WOR_C_MaxSectoRef; i++)
		{
			if(!pwhat[i]) break;
		}

		pwhat[sel] = pwhat[i - 1];
		pwhat[i - 1] = 0;
		OnSelSect();
		break;

	case 3:
		L_memcpy(pwhat, pwhat1, WOR_C_MaxSectoRef);
		OnSelSect();
		break;
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::RefreshFromExt(void)
{
	FillListSect();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_SectoDialog::i_OnMessage(ULONG msg, ULONG, ULONG)
{
	if(msg == EDI_MESSAGE_REFRESHDLG)
	{
		RefreshFromExt();
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnMenuSectVis(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	CListBox	*po_Sect;
	int			sect;
	/*~~~~~~~~~~~~~~~~~*/

	po_Sect = (CListBox *) GetDlgItem(IDC_LIST_SECT);
	sect = po_Sect->GetCurSel();
	if(sect == -1) return;
	sect = po_Sect->GetItemData(sect);
	RefSetMenu
	(
		(CListBox *) GetDlgItem(IDC_LIST_VIS),
		&mpst_World->ast_AllSectos[sect].auc_RefVis[0],
		&mpst_World->ast_AllSectos[sect].auc_RefAct[0]
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnMenuSectAct(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	CListBox	*po_Sect;
	int			sect;
	/*~~~~~~~~~~~~~~~~~*/

	po_Sect = (CListBox *) GetDlgItem(IDC_LIST_SECT);
	sect = po_Sect->GetCurSel();
	if(sect == -1) return;
	sect = po_Sect->GetItemData(sect);
	RefSetMenu
	(
		(CListBox *) GetDlgItem(IDC_LIST_ACT),
		&mpst_World->ast_AllSectos[sect].auc_RefAct[0],
		&mpst_World->ast_AllSectos[sect].auc_RefVis[0]
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnMenuSectObj(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*po_Sect;
	int					sect;
	EMEN_cl_SubMenu		o_SubMenu(FALSE);
	CPoint				o_Point;
	int					i_Res, i;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	CString				str;
	EOUT_cl_Frame		*po_Output;
#ifdef JADEFUSION
	int					j;
	CListCtrl			*po_Obj;
	POSITION			pos;
#else
	CListBox			*po_Obj;
	int					selcount;
	int					*pisel;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Sect = (CListBox *) GetDlgItem(IDC_LIST_SECT);
	sect = po_Sect->GetCurSel();
	if(sect == -1) return;
	sect = po_Sect->GetItemData(sect);

	GetCursorPos(&o_Point);
	M_MF()->InitPopupMenuAction(NULL, &o_SubMenu);
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 1, TRUE, "Add 3DView Selection");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 3, TRUE, "Add By Picking");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 4, TRUE, "Remove 3DView Selection");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 5, TRUE, "Remove By Picking");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 10, TRUE, "Remove");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 11, TRUE, "Remove From All");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 12, TRUE, "Select Objects");

#ifdef JADEFUSION
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 13, TRUE, "Show Gao Info Dialog");
#endif
	i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Point, &o_SubMenu);
	switch(i_Res)
	{
	/* Ajout de la selection */
	case 1:
		pst_AIEOT = &(mpst_World->st_AllWorldObjects);
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
		for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GAO)) continue;
			if(pst_GAO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
			{
				if(!pst_GAO->pst_Extended)
				{
					OBJ_GameObject_CreateExtended(pst_GAO);
					pst_GAO->ul_IdentityFlags |= OBJ_C_IdentityFlag_ExtendedObject;
				}

				if(pst_GAO->pst_Extended->auc_Sectos[0] == sect) continue;
				if(pst_GAO->pst_Extended->auc_Sectos[1] == sect) continue;
				if(pst_GAO->pst_Extended->auc_Sectos[2] == sect) continue;
				if(pst_GAO->pst_Extended->auc_Sectos[3] == sect) continue;

				for(i = 0; i < 4; i++)
				{
					if(pst_GAO->pst_Extended->auc_Sectos[i] == 0)
					{
						pst_GAO->pst_Extended->auc_Sectos[i] = sect;
						break;
					}
				}

				if(i == 4)
				{
					str.Format("Object %s is already in 4 different sectors", pst_GAO->sz_Name);
					ERR_X_Warning(0, (char *) (LPCSTR) str, NULL);
				}
			}
		}

		OnSelSect();
		break;

	/* Remove selection */
	case 4:
		pst_AIEOT = &(mpst_World->st_AllWorldObjects);
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
		for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GAO)) continue;
			if(pst_GAO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
			{
				if(!pst_GAO->pst_Extended) OBJ_GameObject_CreateExtended(pst_GAO);
				if(pst_GAO->pst_Extended->auc_Sectos[0] == sect) pst_GAO->pst_Extended->auc_Sectos[0] = 0;
				if(pst_GAO->pst_Extended->auc_Sectos[1] == sect) pst_GAO->pst_Extended->auc_Sectos[1] = 0;
				if(pst_GAO->pst_Extended->auc_Sectos[2] == sect) pst_GAO->pst_Extended->auc_Sectos[2] = 0;
				if(pst_GAO->pst_Extended->auc_Sectos[3] == sect) pst_GAO->pst_Extended->auc_Sectos[3] = 0;
			}
		}

		OnSelSect();
		break;

	/* Remove from selection */
	case 10:
#ifdef JADEFUSION
		po_Obj = (CListCtrl *) GetDlgItem(IDC_LIST_OBJ);
		pos = po_Obj->GetFirstSelectedItemPosition();
		while(pos)



		{
			j = po_Obj->GetNextSelectedItem(pos);

			if(j == -1)
				break;

			pst_GAO = (OBJ_tdst_GameObject *) po_Obj->GetItemData(j);
#else
		po_Obj = (CListBox *) GetDlgItem(IDC_LIST_OBJ);
		selcount = po_Obj->GetSelCount();
		if(!selcount) return;
		pisel = (int *) malloc(selcount * 4);
		po_Obj->GetSelItems(selcount, pisel);
		for(i = 0; i < selcount; i++)
		{
			pst_GAO = (OBJ_tdst_GameObject *) po_Obj->GetItemData(pisel[i]);
#endif
			if(!pst_GAO->pst_Extended) continue;
			if(pst_GAO->pst_Extended->auc_Sectos[0] == sect) pst_GAO->pst_Extended->auc_Sectos[0] = 0;
			if(pst_GAO->pst_Extended->auc_Sectos[1] == sect) pst_GAO->pst_Extended->auc_Sectos[1] = 0;
			if(pst_GAO->pst_Extended->auc_Sectos[2] == sect) pst_GAO->pst_Extended->auc_Sectos[2] = 0;
			if(pst_GAO->pst_Extended->auc_Sectos[3] == sect) pst_GAO->pst_Extended->auc_Sectos[3] = 0;
		}
#ifndef JADEFUSION
		free(pisel);
#endif
		OnSelSect();
		break;

	/* Remove from all */
	case 11:
		{
#ifdef JADEFUSION
			po_Obj = (CListCtrl *) GetDlgItem(IDC_LIST_OBJ);
			pos = po_Obj->GetFirstSelectedItemPosition();
			while(pos)
			{
				j = po_Obj->GetNextSelectedItem(pos);

				if(j == -1)
					break;

				pst_GAO = (OBJ_tdst_GameObject *) po_Obj->GetItemData(j);
#else
			/*~~~~~~~~~*/
			int selcount;
			int *pisel;
			/*~~~~~~~~~*/

			po_Obj = (CListBox *) GetDlgItem(IDC_LIST_OBJ);
			selcount = po_Obj->GetSelCount();
			if(!selcount) return;
			pisel = (int *) malloc(selcount * 4);
			po_Obj->GetSelItems(selcount, pisel);
			for(i = 0; i < selcount; i++)
			{
				pst_GAO = (OBJ_tdst_GameObject *) po_Obj->GetItemData(pisel[i]);
#endif
				if(!pst_GAO->pst_Extended) continue;
				pst_GAO->pst_Extended->auc_Sectos[0] = 0;
				pst_GAO->pst_Extended->auc_Sectos[1] = 0;
				pst_GAO->pst_Extended->auc_Sectos[2] = 0;
				pst_GAO->pst_Extended->auc_Sectos[3] = 0;
			}
#ifndef JADEFUSION
			free(pisel);
#endif
			OnSelSect();
		}
		break;

	/* Picking Add */
	case 3:
		AddPickObj(sect, TRUE);
		break;

	/* Picking Del */
	case 5:
		AddPickObj(sect, FALSE);
		break;

	/* Select objects */
	case 12:
#ifdef JADEFUSION
		po_Obj = (CListCtrl *) GetDlgItem(IDC_LIST_OBJ);

#else
		po_Obj = (CListBox *) GetDlgItem(IDC_LIST_OBJ);
		selcount = po_Obj->GetSelCount();
		if(!selcount) return;
		pisel = (int *) malloc(selcount * 4);
		po_Obj->GetSelItems(selcount, pisel);
#endif
		po_Output = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		po_Output->mpo_EngineFrame->mpo_DisplayView->Selection_b_Treat(NULL, 0, 0, 0);
#ifdef JADEFUSION
		pos = po_Obj->GetFirstSelectedItemPosition();
		while(pos)
		{
			j = po_Obj->GetNextSelectedItem(pos);

			if(j == -1)
				break;

			pst_GAO = (OBJ_tdst_GameObject *) po_Obj->GetItemData(j);
			SEL_DelItem(mpst_World->pst_Selection, pst_GAO);
			po_Output->mpo_EngineFrame->mpo_DisplayView->Selection_Object(pst_GAO, SEL_C_SIF_Object);

		}
#else
		for(i = 0; i < selcount; i++)
		{
			pst_GAO = (OBJ_tdst_GameObject *) po_Obj->GetItemData(pisel[i]);
			SEL_DelItem(mpst_World->pst_Selection, pst_GAO);
			po_Output->mpo_EngineFrame->mpo_DisplayView->Selection_Object(pst_GAO, SEL_C_SIF_Object);
		}
#endif
		LINK_Refresh();
		po_Output->mpo_EngineFrame->mpo_DisplayView->mb_SelectionChange = TRUE;
		po_Output->mpo_EngineFrame->mpo_DisplayView->Selection_Change();
		break;

#ifdef JADEFUSION
	/* Show Gao Info Dialog */
	case 13:
		// first, select object in 3DView
		po_Obj = (CListCtrl *) GetDlgItem(IDC_LIST_OBJ);

		po_Output = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		po_Output->mpo_EngineFrame->mpo_DisplayView->Selection_b_Treat(NULL, 0, 0, 0);

		pos = po_Obj->GetFirstSelectedItemPosition();
		if(pos)
		{
			j = po_Obj->GetNextSelectedItem(pos);

			if(j == -1)
				break;

			pst_GAO = (OBJ_tdst_GameObject *) po_Obj->GetItemData(j);
			SEL_DelItem(mpst_World->pst_Selection, pst_GAO);
			po_Output->mpo_EngineFrame->mpo_DisplayView->Selection_Object(pst_GAO, SEL_C_SIF_Object);

			LINK_Refresh();
			po_Output->mpo_EngineFrame->mpo_DisplayView->mb_SelectionChange = TRUE;
			po_Output->mpo_EngineFrame->mpo_DisplayView->Selection_Change();
		
			//then, popup the Info Dialog
			po_Output->OnAction(EOUT_ACTION_GO_LIGHT_INFORMATION);
		}

		break;
#endif

	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::AddPickObj(int sect, BOOL add)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MSG					msg;
	CPoint				pt;
	BOOL				ok;
	EOUT_cl_Frame		*po_Output;
	CRect				rect;
	OBJ_tdst_GameObject *pst_GAO;
	int					i, index;
	CString				str;
#ifdef JADEFUSION
	int					j;
	CListCtrl			*po_List;
#else
	CListBox			*po_List;
#endif

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Picking position */
#ifdef JADEFUSION
	po_List = (CListCtrl *) GetDlgItem(IDC_LIST_OBJ);
#else
	po_List = (CListBox *) GetDlgItem(IDC_LIST_OBJ);
#endif
	ok = FALSE;
	SetCapture();
	::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));

#ifdef JADEFUSION
	j = 0;
#endif
	while(1)
	{
		::GetMessage(&msg, NULL, 0, 0);

		if((GetAsyncKeyState(VK_SPACE) < 0) || (GetAsyncKeyState(VK_MENU) < 0))
		{
			if(EDI_go_TheApp.b_ProcessMouseWheel(&(msg))) continue;
			if(GetCapture() == this) ReleaseCapture();
			DispatchMessage(&msg);
			continue;
		}

		if(GetCapture() != this)
		{
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
			SetCapture();
		}

		if(msg.message == WM_RBUTTONDOWN) break;
		if(msg.message == WM_LBUTTONDOWN)
		{
			/* Pick to know pos */
			GetCursorPos(&pt);
			po_Output = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
			po_Output->mpo_EngineFrame->mpo_DisplayView->GetClientRect(&rect);
			po_Output->mpo_EngineFrame->mpo_DisplayView->ScreenToClient(&pt);
			if(!rect.PtInRect(pt)) break;
			pst_GAO = po_Output->mpo_EngineFrame->mpo_DisplayView->pst_Pick(pt);
			if(pst_GAO)
			{
				/* Add */
				if(add)
				{
					if(!pst_GAO->pst_Extended)
					{
						OBJ_GameObject_CreateExtended(pst_GAO);
						pst_GAO->ul_IdentityFlags |= OBJ_C_IdentityFlag_ExtendedObject;
					}

					for(i = 0; i < 4; i++)
					{
						if(pst_GAO->pst_Extended->auc_Sectos[i] == sect) break;
					}

					if(i == 4)
					{
						for(i = 0; i < 4; i++)
						{
							if(pst_GAO->pst_Extended->auc_Sectos[i] == 0)
							{
								pst_GAO->pst_Extended->auc_Sectos[i] = sect;
								break;
							}
						}

						if(i == 4)
						{
							str.Format("Object %s is already in 4 different sectors", pst_GAO->sz_Name);
							ERR_X_Warning(0, (char *) (LPCSTR) str, NULL);
						}

#ifdef JADEFUSION
						index = po_List->InsertItem(j++, pst_GAO->sz_Name, 0);
#else
						index = po_List->AddString(pst_GAO->sz_Name);
#endif
						po_List->SetItemData(index, (int) pst_GAO);
						po_Output->mpo_EngineFrame->mpo_DisplayView->Invalidate();
						po_Output->mpo_EngineFrame->mpo_DisplayView->RedrawWindow();
					}
				}

				/* Remove */
				else
				{
					for(i = 0; i < 4; i++)
					{
						if(!pst_GAO->pst_Extended) break;
						if(pst_GAO->pst_Extended->auc_Sectos[i] == sect) pst_GAO->pst_Extended->auc_Sectos[i] = 0;
					}
#ifdef JADEFUSION
					LVFINDINFO info;
					info.flags = LVFI_PARTIAL|LVFI_STRING;
					info.psz = pst_GAO->sz_Name;

					index = po_List->FindItem(&info);
					if(index != -1) po_List->DeleteItem(index);
#else
					index = po_List->FindString(-1, pst_GAO->sz_Name);
					if(index != -1) po_List->DeleteString(index);
#endif
					po_Output->mpo_EngineFrame->mpo_DisplayView->Invalidate();
					po_Output->mpo_EngineFrame->mpo_DisplayView->RedrawWindow();
				}
			}
		}
	}

	ReleaseCapture();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SectoDialog::PickPoint3D(MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EOUT_cl_Frame	*po_Output;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Output = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	return po_Output->mpo_EngineFrame->mpo_DisplayView->PickPoint3D(v);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::DeleteSharePortalTo(int secto, int portal)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int				i, j, k;
	WOR_tdst_Portal *port;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(portal != -1)
	{
		port = &mpst_World->ast_AllSectos[secto].ast_Portals[portal];
		if(port->uw_Flags & WOR_CF_PortalShare) return;
	}

	for(i = 0; i < WOR_C_MaxSecto; i++)
	{
		if(!(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)) continue;
		for(j = 0; j < WOR_C_MaxSectoPortals; j++)
		{
			port = &mpst_World->ast_AllSectos[i].ast_Portals[j];
			if(!(port->uw_Flags & WOR_CF_PortalValid)) break;

			if
			(
				(port->uw_Flags & WOR_CF_PortalShare)
			&&	(port->uc_ShareSect == secto)
			&&	(portal == -1 || port->uc_SharePortal == portal)
			)
			{
				/* Delete portal */
				for(k = j; k < WOR_C_MaxSectoPortals; k++)
				{
					if(!(mpst_World->ast_AllSectos[i].ast_Portals[k].uw_Flags & WOR_CF_PortalValid)) break;
				}

				L_memcpy
				(
					&mpst_World->ast_AllSectos[i].ast_Portals[j],
					&mpst_World->ast_AllSectos[i].ast_Portals[k - 1],
					sizeof(WOR_tdst_Portal)
				);
				L_memset(&mpst_World->ast_AllSectos[i].ast_Portals[k - 1], 0, sizeof(WOR_tdst_Portal));
				j--;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::CopySharePortalTo(int secto, int portal)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int				i, j, k;
	WOR_tdst_Portal *port;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(portal != -1)
	{
		port = &mpst_World->ast_AllSectos[secto].ast_Portals[portal];
		if(port->uw_Flags & WOR_CF_PortalShare) return;
	}

	for(i = 0; i < WOR_C_MaxSecto; i++)
	{
		if(!(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)) continue;
		for(j = 0; j < WOR_C_MaxSectoPortals; j++)
		{
			port = &mpst_World->ast_AllSectos[i].ast_Portals[j];
			if(!(port->uw_Flags & WOR_CF_PortalValid)) break;

			if
			(
				(port->uw_Flags & WOR_CF_PortalShare)
			&&	(port->uc_ShareSect == secto)
			&&	(portal == -1 || port->uc_SharePortal == portal)
			)
			{
				/* Delete portal */
				for(k = j; k < WOR_C_MaxSectoPortals; k++)
				{
					if(!(mpst_World->ast_AllSectos[i].ast_Portals[k].uw_Flags & WOR_CF_PortalValid)) break;
				}

				L_memcpy
				(
					&mpst_World->ast_AllSectos[i].ast_Portals[k],
					&mpst_World->ast_AllSectos[i].ast_Portals[j],
					sizeof(WOR_tdst_Portal)
				);

				break;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnMenuSectPortal(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu		o_SubMenu(FALSE);
	CPoint				o_Point;
	CListBox			*po_Sect;
	CListBox			*po_Port;
	int					i_Res, sect, i, sel;
	int					newsect;
	EDIA_cl_NameDialog	o_Dlg("Enter name");
	MATH_tdst_Vector	v;
	EDIA_cl_ListDialog	o_Lst(EDIA_List_SpeedSelectSector);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Sect = (CListBox *) GetDlgItem(IDC_LIST_SECT);
	po_Port = (CListBox *) GetDlgItem(IDC_LIST_PORTAL);
	sect = po_Sect->GetCurSel();
	if(sect != -1) sect = po_Sect->GetItemData(sect);

	sel = -1;
	if(sect != -1)
	{
		sel = po_Port->GetCurSel();
		if(sel != -1) sel = po_Port->GetItemData(sel);
	}

	GetCursorPos(&o_Point);
	M_MF()->InitPopupMenuAction(NULL, &o_SubMenu);
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 1, TRUE, "New Portal");
//	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 50, TRUE, "Duplicate Portal");

	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 2, TRUE, "Rename Portal");
	if(sel != -1)
	{
		if(mpst_World->ast_AllSectos[sect].ast_Portals[sel].uw_Flags & WOR_CF_PortalShare)
			M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 4, TRUE, "Unshare");
		else
			M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 3, TRUE, "Share Portal With");
	}

	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 10, TRUE, "Delete Portal");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");

	if(sel != -1)
	{
		if(mpst_World->ast_AllSectos[sect].ast_Portals[sel].uw_Flags & WOR_CF_PortalRender)
			M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 20, TRUE, "Hide Portal");
		else
			M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 20, TRUE, "Show Portal");
	}

	i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Point, &o_SubMenu);
	switch(i_Res)
	{
	/* New portal */
	case 1:
		for(i = 0; i < WOR_C_MaxSectoPortals; i++)
		{
			if(!(mpst_World->ast_AllSectos[sect].ast_Portals[i].uw_Flags & WOR_CF_PortalValid)) break;
		}

		if(i == WOR_C_MaxSectoPortals)
		{
			ERR_X_Error(0, "Two many portals", NULL);
			return;
		}

		if(o_Dlg.DoModal() != IDOK) return;
		if(!PickPoint3D(&v)) return;

		L_memset(&mpst_World->ast_AllSectos[sect].ast_Portals[i], 0, sizeof(WOR_tdst_Portal));
		strncpy
		(
			mpst_World->ast_AllSectos[sect].ast_Portals[i].az_Name,
			(char *) (LPCSTR) o_Dlg.mo_Name,
			WOR_C_MaxLenNamePortal
		);

		mpst_World->ast_AllSectos[sect].ast_Portals[i].uw_Flags = WOR_CF_PortalValid | WOR_CF_PortalRender;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].uc_ShareSect = sect;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].uc_SharePortal = i;
		MATH_CopyVector(&mpst_World->ast_AllSectos[sect].ast_Portals[i].vA, &v);
		MATH_CopyVector(&mpst_World->ast_AllSectos[sect].ast_Portals[i].vB, &v);
		MATH_CopyVector(&mpst_World->ast_AllSectos[sect].ast_Portals[i].vC, &v);
		MATH_CopyVector(&mpst_World->ast_AllSectos[sect].ast_Portals[i].vD, &v);
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vA.x -= 2;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vA.z += 4;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vB.x += 2;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vB.z += 4;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vC.x += 2;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vD.x -= 2;
		OnSelSect();
		break;

	/* Rename portal */
	case 2:
		if(sect == -1) break;
		sel = po_Port->GetCurSel();
		if(sel == -1) return;
		sel = po_Port->GetItemData(sel);
		o_Dlg.mo_Name = mpst_World->ast_AllSectos[sect].ast_Portals[sel].az_Name;
		if(o_Dlg.DoModal() != IDOK) return;
		strncpy
		(
			mpst_World->ast_AllSectos[sect].ast_Portals[sel].az_Name,
			(char *) (LPCSTR) o_Dlg.mo_Name,
			WOR_C_MaxLenNamePortal
		);
		OnSelSect();
		WOR_ValidatePortal(mpst_World, &mpst_World->ast_AllSectos[sect].ast_Portals[sel]);
		break;

	/* Unshare portal */
	case 4:
		if(sect == -1) break;
		sel = po_Port->GetCurSel();
		if(sel == -1) return;
		sel = po_Port->GetItemData(sel);
		if(!(mpst_World->ast_AllSectos[sect].ast_Portals[sel].uw_Flags & WOR_CF_PortalShare))
		{
			ERR_X_Error(0, "Portal not shared", NULL);
			return;
		}

		mpst_World->ast_AllSectos[sect].ast_Portals[sel].uw_Flags &= ~WOR_CF_PortalShare;
		OnSelSect();
		break;

	/* Share portal */
	case 3:
		if(sect == -1) break;
		sel = po_Port->GetCurSel();
		if(sel == -1) return;
		sel = po_Port->GetItemData(sel);
		if(mpst_World->ast_AllSectos[sect].ast_Portals[sel].uw_Flags & WOR_CF_PortalShare)
		{
			ERR_X_Error(0, "Portal is already shared", NULL);
			return;
		}

		o_Lst.mpst_World = mpst_World;
		if(o_Lst.DoModal() != IDOK) return;
		newsect = (int) o_Lst.mul_Model;
		if(newsect == sect) return;
		for(i = 0; i < WOR_C_MaxSectoPortals; i++)
		{
			if(!(mpst_World->ast_AllSectos[newsect].ast_Portals[i].uw_Flags & WOR_CF_PortalValid)) break;
		}

		if(i == WOR_C_MaxSectoPortals)
		{
			ERR_X_Error(0, "Two many portals", NULL);
			return;
		}

		L_memcpy
		(
			&mpst_World->ast_AllSectos[newsect].ast_Portals[i],
			&mpst_World->ast_AllSectos[sect].ast_Portals[sel],
			sizeof(WOR_tdst_Portal)
		);

		mpst_World->ast_AllSectos[newsect].ast_Portals[i].uw_Flags |= WOR_CF_PortalShare;
		break;

	/* Delete portal */
	case 10:
		if(MessageBox("This will delete portal. Do you want to continue ?", "Please confirm", MB_YESNO) != IDYES)
			break;
		if(sect == -1) break;
		sel = po_Port->GetCurSel();
		if(sel == -1) return;
		sel = po_Port->GetItemData(sel);
		for(i = 0; i < WOR_C_MaxSectoPortals; i++)
		{
			if(!(mpst_World->ast_AllSectos[sect].ast_Portals[i].uw_Flags & WOR_CF_PortalValid)) break;
		}

		L_memcpy
		(
			&mpst_World->ast_AllSectos[sect].ast_Portals[sel],
			&mpst_World->ast_AllSectos[sect].ast_Portals[i - 1],
			sizeof(WOR_tdst_Portal)
		);
		L_memset(&mpst_World->ast_AllSectos[sect].ast_Portals[i - 1], 0, sizeof(WOR_tdst_Portal));
		OnSelSect();
		DeleteSharePortalTo(sect, sel);
		break;

	/* Copy portal */
	case 50:
		if(sect == -1) break;
		sel = po_Port->GetCurSel();
		if(sel == -1) return;
		sel = po_Port->GetItemData(sel);

		for(i = 0; i < WOR_C_MaxSectoPortals; i++)
		{
			if(!(mpst_World->ast_AllSectos[sect].ast_Portals[i].uw_Flags & WOR_CF_PortalValid)) break;
		}

		if(i == WOR_C_MaxSectoPortals) break;
		if(!PickPoint3D(&v)) return;
		L_memcpy
		(
			&mpst_World->ast_AllSectos[sect].ast_Portals[i],
			&mpst_World->ast_AllSectos[sect].ast_Portals[sel],
			sizeof(WOR_tdst_Portal)
		);

		MATH_CopyVector(&mpst_World->ast_AllSectos[sect].ast_Portals[i].vA, &v);
		MATH_CopyVector(&mpst_World->ast_AllSectos[sect].ast_Portals[i].vB, &v);
		MATH_CopyVector(&mpst_World->ast_AllSectos[sect].ast_Portals[i].vC, &v);
		MATH_CopyVector(&mpst_World->ast_AllSectos[sect].ast_Portals[i].vD, &v);
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vA.x -= 2;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vA.z += 4;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vB.x += 2;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vB.z += 4;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vC.x += 2;
		mpst_World->ast_AllSectos[sect].ast_Portals[i].vD.x -= 2;

		OnSelSect();
		CopySharePortalTo(sect, sel);
		break;

	/* Show/Hide portal */
	case 20:
		if(sect == -1) break;
		sel = po_Port->GetCurSel();
		if(sel == -1) return;
		sel = po_Port->GetItemData(sel);
		if(mpst_World->ast_AllSectos[sect].ast_Portals[sel].uw_Flags & WOR_CF_PortalRender)
			mpst_World->ast_AllSectos[sect].ast_Portals[sel].uw_Flags &= ~WOR_CF_PortalRender;
		else
			mpst_World->ast_AllSectos[sect].ast_Portals[sel].uw_Flags |= WOR_CF_PortalRender;
		break;
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::ColorizeSel(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int			sect;
	CListBox	*po_Sect;
	int			i;
	/*~~~~~~~~~~~~~~~~~*/

	for(i = 1; i < WOR_C_MaxSecto; i++)
	{
		mpst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor1;
		mpst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor2;
		mpst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor3;
	}

	po_Sect = (CListBox *) GetDlgItem(IDC_LIST_SECT);
	sect = po_Sect->GetCurSel();
	if(sect == -1) return;
	sect = po_Sect->GetItemData(sect);

	mpst_World->ast_AllSectos[sect].ul_Flags |= WOR_CF_SectorColor1;
	for(i = 0; i < WOR_C_MaxSectoRef; i++)
	{
		mpst_World->ast_AllSectos[mpst_World->ast_AllSectos[sect].auc_RefVis[i]].ul_Flags |= WOR_CF_SectorColor2;
		mpst_World->ast_AllSectos[mpst_World->ast_AllSectos[sect].auc_RefAct[i]].ul_Flags |= WOR_CF_SectorColor3;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::ShowPortalsSel(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int			sect;
	CListBox	*po_Sect;
	int			i, j;
	/*~~~~~~~~~~~~~~~~~*/

	for(i = 1; i < WOR_C_MaxSecto; i++)
	{
		for(j = 0; j < WOR_C_MaxSectoPortals; j++)
		{
			mpst_World->ast_AllSectos[i].ast_Portals[j].uw_Flags &= ~WOR_CF_PortalRender;
		}
	}

	po_Sect = (CListBox *) GetDlgItem(IDC_LIST_SECT);
	sect = po_Sect->GetCurSel();
	if(sect == -1) return;
	sect = po_Sect->GetItemData(sect);
	for(j = 0; j < WOR_C_MaxSectoPortals; j++)
	{
		mpst_World->ast_AllSectos[sect].ast_Portals[j].uw_Flags |= WOR_CF_PortalRender;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnSectDblClk(void)
{
	ColorizeSel();
	ShowPortalsSel();
	LINK_Refresh();
}

static WOR_tdst_Secto ast_AllSectos[WOR_C_MaxSecto];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnCopy(void)
{
	L_memcpy(&ast_AllSectos, &mpst_World->ast_AllSectos[0], sizeof(mpst_World->ast_AllSectos));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SectoDialog::OnPaste(void)
{
	L_memcpy(&mpst_World->ast_AllSectos[0], &ast_AllSectos, sizeof(mpst_World->ast_AllSectos));
}

#endif /* ACTIVE_EDITORS */

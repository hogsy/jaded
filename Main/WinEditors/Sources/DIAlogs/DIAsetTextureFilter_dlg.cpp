/*$T DIAname_dlg.cpp GC! 1.078 03/16/00 10:33:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIASetTextureFilter_dlg.h"
//#include "DIAlogs/DIAComboBoxCompletion.h"
//#include "BIGfiles/BIGdefs.h"
//#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
//#include "ENGine/Sources/WORld/WORstruct.h"
#include "DIAlogs/DIAname_dlg.h"

#include "edipaths.h"
//#include "engine/sources/animation/aniinit.h"
//#include "bigfiles/bigkey.h"
//#include "bigfiles/bigfat.h"
//#include "engine/sources/world/worsave.h"
//#include "f3dframe/f3dview.h"
//#include "engine/sources/objects/objinit.h"
#include <string>

#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles\LOAding\LOAdefs.h"
//#include "ENGine/Sources/ANImation/ANIstruct.h"
//#include "Engine/sources/OBJects/OBJslowaccess.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
//#include "LINks/LINKmsg.h"
#include "LINKs/LINKtoed.h"

//ULONG IsSubGAODirectoryPresent(WOR_tdst_World* pWorld, char* pstrSubDirName);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*EDIA_cl_SetTextureFilterDialog::EDIA_cl_SetTextureFilterDialog(char *_psz_Title, int _mi_MaxLen, BOOL _b_Pass) :
	EDIA_cl_BaseDialog(IDD_DIALOG_SETTEXTUREFILTER)
{
	mo_Title = _psz_Title;
	mi_MaxLen = _mi_MaxLen;
	mb_Pass = _b_Pass;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*void EDIA_cl_SetTextureFilterDialog::DoDataExchange(CDataExchange *pDX)
{
	
	CEdit	*po_Edit, *po_Edit1;
	int		nIDC; // Control ID to do data exchange

	CDialog::DoDataExchange(pDX);

	po_Edit = ((CEdit *) GetDlgItem(IDC_EDIT));
	po_Edit1 = ((CEdit *) GetDlgItem(IDC_EDIT1));

	if(mb_Pass)
	{
		po_Edit->ShowWindow(SW_HIDE);
		po_Edit1->ShowWindow(SW_SHOW);
		po_Edit = po_Edit1;
		nIDC = IDC_EDIT1;
	}
	else
	{
		po_Edit->ShowWindow(SW_SHOW);
		po_Edit1->ShowWindow(SW_HIDE);
		nIDC = IDC_EDIT;
	}

	SetWindowText(mo_Title);

	DDX_Text(pDX, nIDC, mo_Name);

	// -----------------------------------------
	// Strip trailing spaces in variable mo_Name
	mo_Name.TrimLeft();
	mo_Name.TrimRight();

	po_Edit->SetLimitText(mi_MaxLen);
	po_Edit->SetFocus();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SetTextureFilterDialogCombo::EDIA_cl_SetTextureFilterDialogCombo(char *_psz_Title, int _mi_MaxLen) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_FILTERTEXTURE)
{
	mo_Title = _psz_Title;
	mi_MaxLen = _mi_MaxLen;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*void EDIA_cl_SetTextureFilterDialogCombo::AddItem(char *_psz_Name, int _i_Data)
{
	mo_List.AddTail(_psz_Name);
	mo_ListDatas.AddTail(_i_Data);
}*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SetTextureFilterDialogCombo::SetDefault(char *_psz_Name)
{
	if(_psz_Name)
		mo_Name = _psz_Name;
}
/*void EDIA_cl_SetTextureFilterDialogCombo::OnRadio_NewFilter( void ) 
{
	//CComboBox	*po_Combo;
	//po_Combo = ((CComboBox *) GetDlgItem(IDC_COMBO1));
	//po_Combo->LimitText(mi_MaxLen);
	//po_Combo->Clear;
	
	//po_Combo->SetFocus();
	AddItem("popo1", EVAV_EVVIT_String );
	//po_Combo->UpdateData(FALSE);
	//po_Combo->UpdateDialogControls;
	//po_Combo->UpdateLayeredWindow;
	//po_Combo->

	UpdateData(FALSE);

}*/

void EDIA_cl_SetTextureFilterDialogCombo::OnBnClickedOk()
{
        ULONG   ul_Version;
		/* save ini file */
        SAV_Begin( EDI_Csz_Ini, "TextureFilter.ini" );
        ul_Version = 1;
        SAV_Buffer(&ul_Version, 4 );
//		 SAV_Buffer( &mo_List, sizeof( mo_List ) );
       /* GetWindowRect( &st_Rect );
        SAV_Buffer( &st_Rect, sizeof( RECT ) );
        SAV_Buffer( &mpo_View->mst_GeosphereData, sizeof( GEO_tdst_CO_Geosphere ) );*/
        SAV_ul_End();
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SetTextureFilterDialogCombo::OnInitDialog(void)
{
	CenterWindow();
	return  EDIA_cl_BaseDialog::OnInitDialog();
}
BOOL EDIA_cl_SetTextureFilterDialogCombo::PreTranslateMessage(MSG *pMsg)
{
		if((pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem(IDC_LIST2)->GetSafeHwnd()))
		{
			OnMenuFilter();
			return TRUE;
		}
	
	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}
void EDIA_cl_SetTextureFilterDialogCombo::OnMenuFilter(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu		o_SubMenu(FALSE);
	CPoint				o_Point;
	int					i_Res, sel;
	int					i,cur;
	EDIA_cl_NameDialog	o_Id("Enter ID");
	EDIA_cl_NameDialog	o_Name("Enter Name");
	int					sect;
	CListBox			*po_Sect;
	//TAB_tdst_PFtable	*pst_AIEOT;
	//TAB_tdst_PFelem		*pst_CurrentElem;
	//TAB_tdst_PFelem		*pst_EndElem;
	//static char Name[100][64];
	//OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	
	po_Sect = (CListBox *) GetDlgItem(IDC_LIST2);
	sel = sect = po_Sect->GetCurSel();
	if(sect != -1) sect = po_Sect->GetItemData(sect);

	GetCursorPos(&o_Point);
	M_MF()->InitPopupMenuAction(NULL, &o_SubMenu);
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 1, TRUE, "New Filter");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 2, TRUE, "Rename Filter");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 3, TRUE, "Delete Filter");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");

	i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Point, &o_SubMenu);


	switch(i_Res)
	{
/*	case 666:
		mb_SpecSort = mb_SpecSort ? FALSE : TRUE;
		FillListSect();
		break;*/

	case 1:
		/* Get first valid id */
		cur=0;
		for(i = 0; i < 1000; i++)
		{
			if( !TEX_FilterID[i][0] ) break;
			cur++;
		}

		/* Id */
		/*ERR_X_Error(i != WOR_C_MaxSecto, "Too many defined sectors", NULL);*/
		//o_Id.mo_Name.Format("%d", 0);
		//if(o_Id.DoModal() != IDOK) return;

		//i = L_atoi((char *) (LPCSTR) o_Id.mo_Name);
		/*if(i <= 0 || i >= WOR_C_MaxSecto)
		{
			ERR_X_Error(0, "Bad sector ID", NULL);
			return;
		}

		if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)
		{
			ERR_X_Error(0, "Sector ID already exists", NULL);
			return;
		}*/

		/* Name */
		if(o_Name.DoModal() != IDOK) return;
		//CString		str;
		//str.Format("%s", "chouchou");
		//L_memset(&mpst_World->ast_AllSectos[i], 0, sizeof(WOR_tdst_Secto));
		//strncpy(Name, (char *) (LPCSTR) o_Name.mo_Name, 64);
		strncpy(TEX_FilterID[cur], (char *) (LPCSTR) o_Name.mo_Name, 32);
		//mpst_World->ast_AllSectos[i].ul_Flags = WOR_CF_SectorValid;
		//AddItem(Name, EVAV_EVVIT_String );
		//sprintf(Name,"choucroute");
		//AddItem(TEX_FilterID[i], EVAV_EVVIT_String );
		FillListFilter();
		break;

	/* Rename */
	case 2:
		if(sect == -1) break;
		//o_Name.mo_Name = mpst_World->ast_AllSectos[sect].az_Name;
		if(o_Name.DoModal() != IDOK) return;
		strncpy(TEX_FilterID[sect], (char *) (LPCSTR) o_Name.mo_Name, 32);
		FillListFilter();
		break;

	/* Delete */
	case 3:
		if(sect == -1) break;
		if(MessageBox("This will delete Filter. Do you want to continue ?", "Please confirm", MB_YESNO) != IDYES)
			break;
		//mpst_World->ast_AllSectos[sect].ul_Flags &= ~WOR_CF_SectorValid;
		//po_Sect->SetRedraw(FALSE);
		FillListFilter();
		/*if(sel == po_Sect->GetCount()) sel--;
		if(sel >= 0)
		{
			po_Sect->SetCurSel(sel);
			OnSelSect();
		}*/

		/* Remove objects */
/*		pst_AIEOT = &(mpst_World->st_AllWorldObjects);
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
/*	case 5:
		if(sect == -1) break;
		WOR_SetCurrentSector(mpst_World, sect, TRUE);
		FillListSect();
		break;

	/* Reset current */
/*	case 6:
		if(sect == -1) break;
		WOR_SetCurrentSector(mpst_World, 0, TRUE);
		FillListSect();
		break;

	/* Colorize */
/*	case 10:
		ColorizeSel();
		break;

	/* Hide colorize */
/*	case 11:
		for(i = 1; i < WOR_C_MaxSecto; i++)
		{
			mpst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor1;
			mpst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor2;
			mpst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor3;
		}
		break;

	/* Show Portals */
/*	case 12:
		if(sect == -1) break;
		for(i = 0; i < WOR_C_MaxSectoPortals; i++)
		{
			mpst_World->ast_AllSectos[sect].ast_Portals[i].uw_Flags |= WOR_CF_PortalRender;
		}
		break;

	/* Hide Portals */
/*	case 13:
		if(sect == -1) break;
		for(i = 0; i < WOR_C_MaxSectoPortals; i++)
		{
			mpst_World->ast_AllSectos[sect].ast_Portals[i].uw_Flags &= ~WOR_CF_PortalRender;
		}
		break;

	/* Hide */
/*	case 21:
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
/*	case 22:
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
				pst_GAO->ul_EditorFlags &= ~OBJ_C_EditFlags_Hidden;
			}
		}*/
		//break;
	}

	LINK_Refresh();
}
void EDIA_cl_SetTextureFilterDialogCombo::FillListFilter(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int			i, index;
	int			sel;//, rank;
	CString		str;
	CListBox	*po_Box;
	//CList<int, int> lst;
//	POSITION	pos;//,pos1;
//	int			bidon;
//	char		asz_Name[BIG_C_MaxLenName];
//	char		*psz_Temp;
	/*~~~~~~~~~~~~~~~~~*/

	po_Box = (CListBox *) GetDlgItem(IDC_LIST2);
	po_Box->SetRedraw(FALSE);
	sel = po_Box->GetCurSel();
	po_Box->ResetContent();

/*	SortSecto(lst);
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

*/
	//for (i=0;i<5;i++)
	i=0;
	while(TEX_FilterID[i][0] != 0)
	{
		//char popo[32]="popo pipi slip";
		//strncpy(str, (LPCSTR*) TEX_FilterID[i][0],32);
		str.Format("%3d - %s", i, TEX_FilterID[i]);
		index = po_Box->AddString(str);
		po_Box->SetItemData(index, i);
		i++;
	}
	/*	pos = mo_List.GetHeadPosition();
		pos1 = mo_ListDatas.GetHeadPosition();
		//while(pos)
		{
			//L_strcpy(asz_Name, mo_List.GetAt(pos));
			strncpy(str, (LPCSTR*) TEX_FilterID[i][0],32);
			//str.Format("%3d - %s", mo_List.GetCount, mo_List.GetAt(pos));

			//psz_Temp = L_strrchr(asz_Name, '.');
			//if(psz_Temp) *psz_Temp = 0;
			index = po_Box->AddString(asz_Name);
			po_Box->SetItemData(index, mo_ListDatas.GetAt(pos1));

			//po_Box->SetItemData(index, i);

			//mo_ListDatas.GetNext(pos1);
			//mo_List.GetNext(pos);
			//i++;
		}*/
	//if(sel >= po_Box->GetCount()) sel = po_Box->GetCount() - 1;
	//if(sel != -1) po_Box->SetCurSel(sel);
	//OnSelSect();
	po_Box->SetRedraw(TRUE);
}

void EDIA_cl_SetTextureFilterDialogCombo::DoDataExchange(CDataExchange *pDX)
{
	//
/*	CComboBox	*po_Combo;
	POSITION	pos, pos1;
	char		asz_Name[BIG_C_MaxLenName];
	char		*psz_Temp;
	int			index;
	int count;
    CString     o_Str;
	//
	CDialog::DoDataExchange(pDX);
	SetWindowText(mo_Title);
	po_Combo = ((CComboBox *) GetDlgItem(IDC_COMBO1));
	po_Combo->LimitText(5);
	//po_Combo->Clear();
	//po_Combo->ResetContent();
	//po_Combo->DeleteItem;
	//po_Combo->DeleteString;
	//po_Combo->SetFocus();

	if(pDX->m_bSaveAndValidate == FALSE)
	{
	count = po_Combo->GetCount();

		while (count>=0)
		{
			po_Combo->DeleteString(count);
			po_Combo->DeleteItem;
			count--;
		}
		/*po_Combo->SetEditSel(0, -1);
		po_Combo->Clear();*/


/*		if(mo_Name) po_Combo->SetWindowText(mo_Name);
		pos = mo_List.GetHeadPosition();
		pos1 = mo_ListDatas.GetHeadPosition();
		while(pos)
		{
			L_strcpy(asz_Name, mo_List.GetAt(pos));
			psz_Temp = L_strrchr(asz_Name, '.');
			if(psz_Temp) *psz_Temp = 0;
			index = po_Combo->AddString(asz_Name);
			po_Combo->SetItemData(index, mo_ListDatas.GetNext(pos1));
			mo_List.GetNext(pos);
		}
	}
	else
	{
		po_Combo->GetWindowText(mo_Name);
		index = po_Combo->GetCurSel();

        if (index == -1)
        {
            index = po_Combo->GetCount() - 1;
            while (index >= 0)
            {
                po_Combo->GetLBText( index, o_Str );
                //if (L_strcmp( (char *) (LPSTR) &o_Str, _psz_Name ) == 0)
                if (mo_Name == o_Str )
                    break;
                index--;
            }
        }
        mi_CurSelData = po_Combo->GetItemData(index);
	}
*/
		FillListFilter();
		EDIA_cl_BaseDialog::DoDataExchange(pDX);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_SetTextureFilterDialogCombo, EDIA_cl_BaseDialog)
//	ON_CBN_SELCHANGE(IDC_COMBO4, OnSelChange)
//	ON_BN_CLICKED( IDC_RADIO_GAO, OnRadio_GAO )
	//ON_BN_CLICKED( IDC_BUTTONNEWFILTER, OnRadio_NewFilter )
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*EDIA_cl_NameRLIDialogCombo::EDIA_cl_NameRLIDialogCombo(char *_psz_Title,
													   OBJ_tdst_GameObject	**_mo_ListGAO,
													   GEO_tdst_Object	**_mo_ListGRO,
													   int _mi_MaxLen) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_NAMERLICOMBO)
{
	mo_Title = _psz_Title;
	mi_MaxLen = _mi_MaxLen;

	WorkWithGRORLI = FALSE;
	mo_ListGAO = _mo_ListGAO;
	mo_ListGRO = _mo_ListGRO;
}*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*void EDIA_cl_NameRLIDialogCombo::AddItem(int _i_Data)
{
	mo_List.AddTail(mo_ListGAO[_i_Data]->sz_Name);
	mo_ListDatas.AddTail(_i_Data);
}*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*void EDIA_cl_NameRLIDialogCombo::SetDefault(char *_psz_Name)
{
	if(_psz_Name)
		mo_Name = _psz_Name;
}*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*void EDIA_cl_NameRLIDialogCombo::DoDataExchange(CDataExchange *pDX)
{
	CComboBox	*po_Combo;
	POSITION	pos, pos1;
	char		asz_Name[BIG_C_MaxLenName];
	char		*psz_Temp;
	int			index;
    CString     o_Str;

	CDialog::DoDataExchange(pDX);
	SetWindowText(mo_Title);
	po_Combo = ((CComboBox *) GetDlgItem(IDC_COMBO4));
	po_Combo->LimitText(mi_MaxLen);
	po_Combo->SetFocus();

	if(pDX->m_bSaveAndValidate == FALSE)
	{
		if(mo_Name) po_Combo->SetWindowText(mo_Name);
		pos = mo_List.GetHeadPosition();
		pos1 = mo_ListDatas.GetHeadPosition();
		while(pos)
		{
			L_strcpy(asz_Name, mo_List.GetAt(pos));
			psz_Temp = L_strrchr(asz_Name, '.');
			if(psz_Temp) *psz_Temp = 0;
			index = po_Combo->AddString(asz_Name);
			po_Combo->SetItemData(index, mo_ListDatas.GetNext(pos1));
			mo_List.GetNext(pos);
		}
	}
	else
	{
		po_Combo->GetWindowText(mo_Name);
		index = po_Combo->GetCurSel();

        if (index == -1)
        {
            index = po_Combo->GetCount() - 1;
            while (index >= 0)
            {
                po_Combo->GetLBText( index, o_Str );
                //if (L_strcmp( (char *) (LPSTR) &o_Str, _psz_Name ) == 0)
                if (mo_Name == o_Str )
                    break;
                index--;
            }
        }
        mi_CurSelData = po_Combo->GetItemData(index);
	}
}*/

/*void EDIA_cl_NameRLIDialogCombo::OnSelChange( void ) {

CComboBox			*po_Combo;
	int					index;
	u8					RLIloc;
	OBJ_tdst_GameObject	*CurGAO;
	GEO_tdst_Object		*CurGRO;

	po_Combo = ((CComboBox *) GetDlgItem(IDC_COMBO4));
	index = po_Combo->GetCurSel();
	mi_CurSelData = po_Combo->GetItemData(index);
	CurGAO = mo_ListGAO[mi_CurSelData];
	CurGRO = mo_ListGRO[mi_CurSelData];

	// Where is the RLI ?
	RLIloc = OBJ_i_RLIlocation(CurGAO);
	GetDlgItem(IDC_RADIO_GAO)->EnableWindow(RLIloc & u8_RLIinGAO);
	GetDlgItem(IDC_RADIO_GRO)->EnableWindow(RLIloc >= u8_RLIinGRO);
	// Check the right radio button
	WorkWithGRORLI = !(RLIloc & u8_RLIinGAO) && (RLIloc >= u8_RLIinGRO);
	((CButton *) GetDlgItem(IDC_RADIO_GRO))->SetCheck( WorkWithGRORLI && (RLIloc >= u8_RLIinGRO));
	((CButton *) GetDlgItem(IDC_RADIO_GAO))->SetCheck(!WorkWithGRORLI && (RLIloc & u8_RLIinGAO));

	return;
}*/

/*void EDIA_cl_NameRLIDialogCombo::OnRadio_GAO( void ) {
	WorkWithGRORLI = FALSE;
}

void EDIA_cl_NameRLIDialogCombo::OnRadio_GRO( void ) {
	WorkWithGRORLI = TRUE;
}*/

/*$4
***********************************************************************************************************************
***********************************************************************************************************************
*/

//#define OBJECTNAME_NOTSET "Unknown or not set"
//#define TYPEDUPLICATION_COPY IDC_RADIO1
//#define TYPEDUPLICATION_INST IDC_RADIO2

/*
=======================================================================================================================
=======================================================================================================================
*/
/*EDIA_cl_NameDialogDuplicate::EDIA_cl_NameDialogDuplicate(F3D_cl_View *po_View, OBJ_tdst_GameObject *po_Gao) :
EDIA_cl_BaseDialog(DIALOGS_IDD_NAMEDUPLICATE)
{
	mpo_View = po_View ;
	mpo_Gao = po_Gao ;
	
	m_pTreeCtl = NULL ;

	m_bDlgIsInitialized = FALSE ;
}


BEGIN_MESSAGE_MAP(EDIA_cl_NameDialogDuplicate, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio)
	ON_CBN_EDITCHANGE(IDC_COMBO, OnEnChangeCombo)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/*
=======================================================================================================================
=======================================================================================================================
*/
/*BOOL EDIA_cl_NameDialogDuplicate::OnInitDialog(void)
{
	//
	ULONG ul_File;
	std::string strGaoName ;
	char strSubDirName[BIG_C_MaxLenPath] = "";
	//
	
	EDIA_cl_BaseDialog::OnInitDialog();

	if( ! mpo_Gao ) 
		return TRUE ;

	// -------------------------------------------------------------------------
	// Check GAO name to duplicate
	if ( mpo_Gao->sz_Name )
	{
		strGaoName = mpo_Gao->sz_Name;
		
		// Remove file extension if present
		if (strGaoName.find(EDI_Csz_ExtGameObject) != -1)
			strGaoName.erase(strGaoName.size()-strlen(EDI_Csz_ExtGameObject));
		//initial name of source Gao
		m_strInitialGaoName = mpo_Gao->sz_Name;
	}
	else
		strGaoName = OBJECTNAME_NOTSET ;
	
	// -------------------------------------------------------------------------
	// Get GAO sub directory name from name
	int index = 0;
	index = strGaoName.find('_',index);
	index = strGaoName.find('_',index+1);
	strcpy(strSubDirName, strGaoName.substr(0,index).c_str());

	ul_File = IsSubGAODirectoryPresent(mpo_Gao->pst_World, strSubDirName);
	
	// -------------------------------------------------------------------------
	// Setup working directory where we are getting the GAOs
	char strName[BIG_C_MaxLenPath] = "";
	sprintf(strName,"(List of GAO coming from:\r\n %s)", strSubDirName);
	GetDlgItem(IDC_STATIC)->SetWindowText(strName);

	// -------------------------------------------------------------------------
	// Setup ComboBox with current GAO and list of GAO in WOW
	m_cbCompletion.SubclassDlgItem(IDC_COMBO, this);
	m_cbCompletion.FillCombo(ul_File);
	m_cbCompletion.SetWindowText(strGaoName.c_str());

	// -------------------------------------------------------------------------
	// Setup OKButton
	m_OKButton = (CButton*) GetDlgItem(IDOK);
	// -------------------------------------------------------------------------
	// Setup RadioButton
	CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
	m_iTypeOfDuplication = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2) ;

	// -------------------------------------------------------------------------
	// Setup TreeCtrl with current GAO
	m_pTreeCtl = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	m_pTreeCtl->SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);
	m_pTreeCtl->SetFont(&M_MF()->mo_Fnt);

	mi_IconGro = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicObject, 0);
	mi_IconGrm = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicMaterial, 0);
	mi_IconTex = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtTexture1, 0);
	mi_IconGao = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGameObject, 0);
	mi_IconGrl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicLight, 0);
	mi_IconMap = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLMap, 0);
	mi_IconCob = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLObject, 0);
	mi_IconGam = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLGMAT, 0);
	mi_IconCin = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLInstance, 0);
	mi_IconCmd = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLSetModel, 0);
	mi_IconSkl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtSkeleton, 0);
	mi_IconShp = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtShape, 0);
	mi_IconAck = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtActionKit, 0);
	mi_IconTrl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtEventAllsTracks, 0);
	mi_IconOin = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtAIEngineInstance, 0);
	mi_IconOva = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtAIEngineVars, 0 );
	mi_IconMdl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtAIEditorModel, 0);
	mi_IconSnk = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtSoundBank, 0);
	mi_IconMsk = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtSoundMetaBank, 0);
	mi_IconGrp = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtObjGroups, 0);

	//UpdateTree(m_iTypeOfDuplication);

	m_bDlgIsInitialized = TRUE ;

	//pour obtenir un nouveau nom au départ
	VerifyName();
	//disable OKbutton to force name change
	//m_OKButton->EnableWindow(FALSE);

	return TRUE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
/*void EDIA_cl_SetTextureFilterDialogDuplicate::OnBnClickedRadio()
{
	if(!mpo_Gao || !m_bDlgIsInitialized) 
		return;

	// Empty TreeCtrl
	m_pTreeCtl->DeleteAllItems();

	// Get the radio button that has been clicked
	m_iTypeOfDuplication = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2);
	
	// Update the content of the tree
	UpdateTree(m_iTypeOfDuplication) ;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
/*void EDIA_cl_SetTextureFilterDialogDuplicate::OnEnChangeCombo()
{
	if(! mpo_Gao || ! m_bDlgIsInitialized) 
		return;

	CString strGaoName;

	// Empty TreeCtrl
	m_pTreeCtl->DeleteAllItems();

	// Get the text typed in combobox
	m_cbCompletion.GetWindowText(strGaoName);
	
	// Remove file extension if present
	if (strGaoName.Find(EDI_Csz_ExtGameObject) != -1)
		strGaoName.Delete(strGaoName.GetLength()-strlen(EDI_Csz_ExtGameObject), strlen(EDI_Csz_ExtGameObject));

	// Update the content of the tree
	UpdateTree(m_iTypeOfDuplication) ;

	//Verify new typed name
	VerifyName();
}

void EDIA_cl_SetTextureFilterDialogDuplicate::OnBnClickedOk()
{
	if(! mpo_Gao || ! m_bDlgIsInitialized || ! mpo_View)
		return ;
	
	CString strInitialGaoName(mpo_Gao->sz_Name);
	CString strGaoName, strPreviousGAOName;

	// Remove file extension if present
	if (strInitialGaoName.Find(EDI_Csz_ExtGameObject) != -1)
		strInitialGaoName.Delete(strInitialGaoName.GetLength()-strlen(EDI_Csz_ExtGameObject), strlen(EDI_Csz_ExtGameObject));

	// Get the text typed in combobox
	m_cbCompletion.GetWindowText(strGaoName);

	// Strip trailing spaces in variable mo_Name
	strGaoName.TrimLeft();
	strGaoName.TrimRight();

	strPreviousGAOName = strGaoName ;

	if ( strInitialGaoName != strGaoName && m_cbCompletion.IsNamePresent(strGaoName) )
	{
		CString strMessage;
		strMessage.Format("'%s' is already used in this wow", strPreviousGAOName);
		GetDlgItem(IDC_STATIC)->SetWindowText(strMessage);

		m_cbCompletion.SetWindowText(strGaoName);
		m_cbCompletion.GetFocus();
		m_cbCompletion.SetEditSel(strGaoName.GetLength(),strGaoName.GetLength()); 


	}
	else
	{
		// Verify format of GAO name 
		if(BIG_b_CheckName((LPSTR)(LPCSTR) strGaoName))
		{
			char asz_NewName[BIG_C_MaxLenPath];

			if(strGaoName.Find(EDI_Csz_ExtGameObject) == -1) 
				sprintf(asz_NewName, "%s%s", (LPSTR)(LPCSTR) strGaoName, EDI_Csz_ExtGameObject);
			else
				sprintf(asz_NewName, "%s", (LPSTR)(LPCSTR) strGaoName);
				
			// Rename GAO if different
			if ( stricmp(mpo_Gao->sz_Name, asz_NewName) != 0 )
				OBJ_GameObject_Rename(mpo_Gao, asz_NewName);

			// If COPY was asked, copy each element of GAO & propose new name (except for materials)
			if ( m_iTypeOfDuplication == TYPEDUPLICATION_COPY )	
			{
				// COPY lights
				if ( mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)
					mpo_View->Selection_DuplicateGro(F3D_Duplicate_Light | F3D_Duplicate_AllLocation);

				if ( mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu )
				{
					// COPY geometry
					if ( mpo_Gao->pst_Base->pst_Visu->pst_Object->i->ul_Type ==	GRO_Geometric )			
						mpo_View->Selection_DuplicateGro(F3D_Duplicate_Geometry | F3D_Duplicate_AllLocation);
				
					// COPY particle generator
					if ( mpo_Gao->pst_Base->pst_Visu->pst_Object->i->ul_Type ==	GRO_ParticleGenerator )			
						mpo_View->Selection_DuplicateGro(F3D_Duplicate_PAG | F3D_Duplicate_AllLocation);
				}


					//Copy Cob
					if (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap )
					{
						COL_tdst_ColMap *pst_ColMap = ((COL_tdst_Base *) mpo_Gao->pst_Extended->pst_Col)->pst_ColMap;
						if(pst_ColMap)
						{
							if(pst_ColMap->uc_NbOfCob)
							{
								for(int i = 0; i < pst_ColMap->uc_NbOfCob; i++)
								{
									if(!pst_ColMap->dpst_Cob[i]) continue;
									COL_tdst_Cob *pst_Cob = pst_ColMap->dpst_Cob[i];
									//SEL_pst_AddItem(pst_Selection, pst_Cob, SEL_C_SIF_Cob);
									// on envoi TRUE pour dire a la fct que c'est un GAO qui est Sel et non un COB
									mpo_View->Selection_DuplicateCob(i);
									//SEL_DelItem(pst_Selection, pst_Cob);
								}
							}
						}
					}


			}
			
			// Exit dialog
			OnOK();
		}
	}
}	
/*
=======================================================================================================================
=======================================================================================================================
*/

/*void EDIA_cl_SetTextureFilterDialogDuplicate::VerifyName(void)
{

	CString strPreviousGAOName;

	// Get the text typed in combobox
	m_cbCompletion.GetWindowText(m_strGaoName);
	// Strip trailing spaces in variable mo_Name
	m_strGaoName.TrimLeft();
	m_strGaoName.TrimRight();

	strPreviousGAOName = m_strGaoName ;

	if ( m_strInitialGaoName != m_strGaoName && m_cbCompletion.IsNamePresent(m_strGaoName) )
	{
		CString strMessage;
		strMessage.Format("'%s' is already used in this wow", strPreviousGAOName);
		GetDlgItem(IDC_STATIC3)->SetWindowText(strMessage);

		m_cbCompletion.SetWindowText(m_strGaoName);
		m_cbCompletion.GetFocus();
		m_cbCompletion.SetEditSel(m_strGaoName.GetLength(),m_strGaoName.GetLength()); 
		//disable OK button - mis en comment car l'auto completion propose un new name
		//m_OKButton->EnableWindow(FALSE);
	}
	else
	{
		//enable OK button
		m_OKButton->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC3)->SetWindowText("");

	}
	//voir le changement de nom dû à l'auto incrémentation
	
	// Empty TreeCtrl
	m_pTreeCtl->DeleteAllItems();
	UpdateTree(m_iTypeOfDuplication) ;
}

/*
=======================================================================================================================
=======================================================================================================================
*/

/*void EDIA_cl_SetTextureFilterDialogDuplicate::UpdateTree(int iTypeOfDuplication)
{
	//
	CString					strGaoName;
	HTREEITEM			    h_Parent, hItem, hItemSub, hItemSubSub;
	
	GRO_tdst_Struct		    *pst_Gro;
	COL_tdst_ColMap		    *pst_ColMap;
	COL_tdst_Instance	    *pst_Zdm;
	ANI_st_GameObjectAnim   *pst_Anim;
	AI_tdst_Instance        *pst_AI;

	BIG_INDEX			    ul_Index;
	//

	if( ! mpo_Gao ) 
		return;

	m_cbCompletion.GetWindowText(strGaoName);

	// Insert root of the tree
	h_Parent = m_pTreeCtl->InsertItem(strGaoName, mi_IconGao, mi_IconGao);
	m_pTreeCtl->SetItemData(h_Parent, (DWORD) mpo_Gao);

	// Bring value to boolean value ( 0 or 1 )
	iTypeOfDuplication = iTypeOfDuplication - TYPEDUPLICATION_COPY ;

	// Insert leaves of the tree
	if(mpo_Gao->pst_Base && mpo_Gao->pst_Base->pst_Visu)
	{
		if (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
		{
			pst_Anim = mpo_Gao->pst_Base->pst_GameObjectAnim;
			if (pst_Anim->pst_SkeletonModel)
			{
				hItem = m_pTreeCtl->InsertItem(pst_Anim->pst_SkeletonModel->sz_Name, mi_IconSkl, mi_IconSkl, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Anim->pst_SkeletonModel);
			}
			if (pst_Anim->pst_Shape)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_Shape);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconShp, mi_IconShp, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Anim->pst_Shape);
				}
			}
			if (pst_Anim->pst_ActionKit)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_ActionKit );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconAck, mi_IconAck, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Anim->pst_ActionKit);
				}
			}
		}
		else
		{
			pst_Gro = mpo_Gao->pst_Base->pst_Visu->pst_Object;
			if(pst_Gro)
			{   
				hItem = m_pTreeCtl->InsertItem(iTypeOfDuplication ? GRO_sz_Struct_GetName(pst_Gro) : strGaoName, mi_IconGro, mi_IconGro, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Gro);
			}

			pst_Gro = mpo_Gao->pst_Base->pst_Visu->pst_Material;
			if(pst_Gro)
			{
				hItem = m_pTreeCtl->InsertItem(GRO_sz_Struct_GetName(pst_Gro), mi_IconGrm, mi_IconGrm, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Gro);
			}
		}
	}

	// Insert leaves of the tree
	if ( mpo_Gao->pst_Extended )
	{
		if((mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Events) )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Gao->pst_Extended->pst_Events->pst_ListTracks );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = m_pTreeCtl->InsertItem(strGaoName, mi_IconTrl, mi_IconTrl, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) mpo_Gao->pst_Extended->pst_Events->pst_ListTracks);
			}

			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Gao->pst_Extended->pst_Events->pst_ListParam );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconTrl, mi_IconTrl, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) mpo_Gao->pst_Extended->pst_Events->pst_ListParam);
			}
		}
		if((mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights) )
		{
			pst_Gro = mpo_Gao->pst_Extended->pst_Light;
			if(pst_Gro)
			{
				hItem = m_pTreeCtl->InsertItem(iTypeOfDuplication ? GRO_sz_Struct_GetName(pst_Gro) : strGaoName, mi_IconGrl, mi_IconGrl, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Gro);
			}
		}

		if( (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Group) )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Gao->pst_Extended->pst_Group );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconGrp, mi_IconGrp, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) mpo_Gao->pst_Extended->pst_Group );
			}
		}

		if ( (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap) && (mpo_Gao->pst_Extended->pst_Col) )
		{
			pst_ColMap = ((COL_tdst_Base *) mpo_Gao->pst_Extended->pst_Col)->pst_ColMap;
			if(pst_ColMap)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(strGaoName, mi_IconMap, mi_IconMap, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_ColMap);

					if(pst_ColMap->uc_NbOfCob)
					{
						for(int i = 0; i < pst_ColMap->uc_NbOfCob; i++)
						{
							if(!pst_ColMap->dpst_Cob[i]) continue;
							ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap->dpst_Cob[i]);
							if(ul_Index == BIG_C_InvalidIndex) continue;

							hItemSub = m_pTreeCtl->InsertItem(iTypeOfDuplication ? BIG_NameFile(ul_Index) : strGaoName + EDI_Csz_ExtCOLObject, mi_IconCob, mi_IconCob, hItem);
							m_pTreeCtl->SetItemData(hItemSub, (DWORD) pst_ColMap->dpst_Cob[i]);

							if(pst_ColMap->dpst_Cob[i]->pst_GMatList)
							{
								hItemSubSub = m_pTreeCtl->InsertItem(pst_ColMap->dpst_Cob[i]->sz_GMatName, mi_IconGam, mi_IconGam, hItemSub);
								m_pTreeCtl->SetItemData(hItemSubSub, (DWORD) pst_ColMap->dpst_Cob[i]->pst_GMatList);
							}
						}
					}
				}
			}
		}

		if ( (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ZDM) && (mpo_Gao->pst_Extended->pst_Col)	)
		{
			pst_Zdm = ((COL_tdst_Base *) mpo_Gao->pst_Extended->pst_Col)->pst_Instance;
			if(pst_Zdm)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Zdm);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconCin, mi_IconCin, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Zdm);

					if(pst_Zdm->pst_ColSet)
					{
						ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Zdm->pst_ColSet);
						if(ul_Index != BIG_C_InvalidIndex)
						{
							hItemSub = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconCmd, mi_IconCmd, hItem);
							m_pTreeCtl->SetItemData(hItemSub, (DWORD) pst_Zdm->pst_ColSet);
						}
					}
				}
			}
		}

		if ( (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) && (mpo_Gao->pst_Extended->pst_Ai) )
		{
			pst_AI = (AI_tdst_Instance *) mpo_Gao->pst_Extended->pst_Ai;
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = m_pTreeCtl->InsertItem(iTypeOfDuplication ? BIG_NameFile(ul_Index) : strGaoName, mi_IconOin, mi_IconOin, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_AI);
			}

			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_VarDes );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = m_pTreeCtl->InsertItem(iTypeOfDuplication ? BIG_NameFile(ul_Index) : strGaoName, mi_IconOva, mi_IconOva, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_AI->pst_VarDes);
			}

			if(pst_AI->pst_Model)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_Model );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconMdl, mi_IconMdl, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_AI->pst_Model);
				}
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_Model->pst_VarDes );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconOva, mi_IconOva, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_AI->pst_VarDes);
				}
			}
		}
	}

	m_pTreeCtl->Expand( h_Parent, TVE_EXPAND );
}*/

/*$4
***********************************************************************************************************************
***********************************************************************************************************************
*/


/*
=======================================================================================================================
=======================================================================================================================
*/
/*EDIA_cl_SetTextureFilterDialogCompletion::EDIA_cl_SetTextureFilterDialogCompletion(WOR_tdst_World *pst_World, int iType, OBJ_tdst_GameObject *pst_GO ) :
EDIA_cl_BaseDialog(DIALOGS_IDD_NAMECOMPLETION)
{
	m_pWorld = pst_World;
	m_pst_GO = pst_GO;
	m_iType = iType ;

}

BEGIN_MESSAGE_MAP(EDIA_cl_NameDialogCompletion, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_EDITCHANGE(IDC_COMBO, OnEnChangeCombo)
END_MESSAGE_MAP()

/*
=======================================================================================================================
=======================================================================================================================
*/
/*BOOL EDIA_cl_SetTextureFilterDialogCompletion::OnInitDialog(void)
{
	ULONG ul_File;
	char strNewNamePrefix[BIG_C_MaxLenName] = "";
	char strSubDirName[BIG_C_MaxLenPath] = "";
	
	EDIA_cl_BaseDialog::OnInitDialog();

	// -------------------------------------------------------------------------
	// Disable OKButton to force name completion
	m_OKButton = (CButton*) GetDlgItem(IDOK);
	m_OKButton->EnableWindow(FALSE);
	// -------------------------------------------------------------------------

	//rename GAO
	if(m_pst_GO)
	{
		strcpy(strNewNamePrefix, m_pst_GO->sz_Name);
		char *psz_Tmp = strrchr(strNewNamePrefix, '.');
		if(psz_Tmp)
			*psz_Tmp = 0;
	}
	else	//enter name for new GAO
	{
		// Setup new GAO name
		if ( strNewNamePrefix[0] != '\0' )
		{
			if ( m_iType == DLG_LIGHT )
				strcat(strNewNamePrefix,"_LUM_");
		}
		else
		{
			//strcpy(strNewNamePrefix, OBJECTNAME_NOTSET) ;

			// Get GAO world name
			//strcpy(strNewNamePrefix, m_pWorld->sz_Name);
			//char *psz_Tmp = strrchr(strNewNamePrefix, '.');
			//if(psz_Tmp)
			//	*psz_Tmp = 0;
			//strcat(strNewNamePrefix,"_");
		}
	}


	// -------------------------------------------------------------------------
	
	ul_File = IsSubGAODirectoryPresent(m_pWorld, strSubDirName);

	// -------------------------------------------------------------------------
	// Setup working directory where we are getting the GAOs
	char strName[BIG_C_MaxLenPath] = "";
	sprintf(strName,"(List of GAO coming from:\r\n %s)", strSubDirName);
	GetDlgItem(IDC_STATIC)->SetWindowText(strName);

	// -------------------------------------------------------------------------
	// Setup ComboBox with current GAO and list of GAO in WOW
	m_cbCompletion.SubclassDlgItem(IDC_COMBO, this);
	m_cbCompletion.FillCombo(ul_File);
	m_cbCompletion.SetWindowText(strNewNamePrefix);

	return TRUE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
/*void EDIA_cl_SetTextureFilterDialogCompletion::OnEnChangeCombo()
{
	m_OKButton->EnableWindow(TRUE);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
/*void EDIA_cl_SetTextureFilterDialogCompletion::OnBnClickedOk()
{
	CString strPreviousName;

	// Get the text typed in combobox
	m_cbCompletion.GetWindowText(m_strName);

	// Strip trailing spaces in variable mo_Name
	m_strName.TrimLeft();
	m_strName.TrimRight();

	strPreviousName = m_strName;

	if ( m_cbCompletion.IsNamePresent(m_strName) )
	{
		CString strMessage;
		strMessage.Format("'%s' is already used in this wow", strPreviousName);
		GetDlgItem(IDC_STATIC)->SetWindowText(strMessage);
		
		m_cbCompletion.SetWindowText(m_strName);
		m_cbCompletion.GetFocus();
		m_cbCompletion.SetEditSel(m_strName.GetLength(),m_strName.GetLength()); 
	}
	else
	{
		// Verify format of GAO name & exit dialog
		if(BIG_b_CheckName((LPSTR)(LPCSTR) m_strName))
			OnOK();
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/


#endif /* ACTIVE_EDITORS */

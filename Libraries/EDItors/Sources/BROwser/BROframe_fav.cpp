/*$T BROframe_fav.cpp GC! 1.098 10/13/00 10:53:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGfat.h"
#include "BROframe.h"
#include "BROtreectrl.h"
#include "BROlistctrl.h"
#include "BROmsg.h"
#include "BROstrings.h"
#include "BROerrid.h"
#include "DIAlogs/DIAfavorites_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDImainframe.h"
#include "ENGine/Sources/ENGinit.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnAddFavorites(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX			ul_Dir;
	int					mi_Res;
	int					i_Free;
	HTREEITEM			h_Item;
	EDIA_cl_NameDialog	o_Dialog(EBRO_STR_Csz_TitleNameFavorite, BIG_C_MaxLenPath - 1);
	char				asz_PathName[BIG_C_MaxLenPath];
	CComboBox			*po_Combo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get selected item */
	h_Item = mpo_TreeCtrl->GetSelectedItem();
	ul_Dir = mpo_TreeCtrl->GetItemData(h_Item);
	o_Dialog.mo_Name = mpo_TreeCtrl->GetItemText(h_Item);

	/* Open dialog */
	if(o_Dialog.DoModal() == IDOK)
	{
		/* Test for an invalid name */
		if(L_strlen((char *) (LPCSTR) o_Dialog.mo_Name) == 0)
		{
			ERR_X_ForceError(EBRO_ERR_Csz_BadName, NULL);
			return;
		}

		/* Compute full path name */
		BIG_ComputeFullName(ul_Dir, asz_PathName);

		/*
		 * Add favorite. £
		 * Search a free place
		 */
		for(i_Free = 0; i_Free < mst_Ini.i_NumFavorites; i_Free++)
		{
			if(mst_Ini.ast_Favorites[i_Free].asz_DisplayName[0] == '\0')
			{
				break;
			}
		}

		/* Init */
		L_strcpy(mst_Ini.ast_Favorites[i_Free].asz_PathName, asz_PathName);
		L_strcpy(mst_Ini.ast_Favorites[i_Free].asz_DisplayName, (char *) (LPCSTR) o_Dialog.mo_Name);

		/* Add to combo box */
		po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(BROWSER_IDC_COMBOFAVORITES);
		mi_Res = po_Combo->AddString(o_Dialog.mo_Name);
		po_Combo->SetItemData(mi_Res, i_Free);

		/* One more favorite if no free place */
		if(i_Free == mst_Ini.i_NumFavorites)
		{
			mst_Ini.i_NumFavorites++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnOrganizeFavorites(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_OrganizeFavorites	o_Dialog;
	CComboBox					*po_Combo;
	int							i, mi_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memcpy(o_Dialog.ast_Favorites, mst_Ini.ast_Favorites, sizeof(mst_Ini.ast_Favorites));
	o_Dialog.i_NumFavorites = mst_Ini.i_NumFavorites;
	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(BROWSER_IDC_COMBOFAVORITES);

	if(o_Dialog.DoModal() == IDOK)
	{
		L_memcpy(mst_Ini.ast_Favorites, o_Dialog.ast_Favorites, sizeof(mst_Ini.ast_Favorites));
		mst_Ini.i_NumFavorites = o_Dialog.i_NumFavorites;
		po_Combo->ResetContent();
		for(i = 0; i < mst_Ini.i_NumFavorites; i++)
		{
			if(mst_Ini.ast_Favorites[i].asz_DisplayName[0])
			{
				mi_Res = po_Combo->AddString(mst_Ini.ast_Favorites[i].asz_DisplayName);
				po_Combo->SetItemData(mi_Res, i);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::SelectDir(BIG_INDEX _ul_Dir)
{
	/*~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Item;
	/*~~~~~~~~~~~~~~~*/

	if(_ul_Dir == BIG_C_InvalidIndex) return;
	M_MF()->LockDisplay(mpo_TreeCtrl);

	h_Item = x_GetTreeItemByName(_ul_Dir);
	if(h_Item == NULL)
	{
		RefreshAll();
		h_Item = x_GetTreeItemByName(_ul_Dir);
	}

	if(h_Item)
	{
		if(mpo_TreeCtrl->GetSelectedItem() != h_Item)
		{
			mpo_TreeCtrl->EnsureVisible(h_Item);
			mpo_TreeCtrl->Expand(h_Item, TVE_EXPAND);
			mpo_TreeCtrl->SelectItem(h_Item);
			OnTreeCtrlSelChange();
		}
		else
		{
			mpo_TreeCtrl->Expand(h_Item, TVE_EXPAND);
		}
	}

	M_MF()->UnlockDisplay(mpo_TreeCtrl);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnFavoriteChange(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Dir;
	CComboBox	*po_Combo;
	CString		o_String;
	int			i_Res;
	/*~~~~~~~~~~~~~~~~~~*/

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(BROWSER_IDC_COMBOFAVORITES);
	i_Res = po_Combo->GetCurSel();
	if(i_Res == CB_ERR) return;

	M_MF()->LockDisplay(this);

_Try_
	i_Res = po_Combo->GetItemData(i_Res);
	ul_Dir = BIG_ul_SearchDir(mst_Ini.ast_Favorites[i_Res].asz_PathName);
	ERR_X_Error(ul_Dir != BIG_C_InvalidIndex, EBRO_ERR_Csz_InvalidFavorite, mst_Ini.ast_Favorites[i_Res].asz_PathName);

	/* Select the directory */
	SelectDir(ul_Dir);

_Catch_
_End_
	M_MF()->UnlockDisplay(this);
	mpo_TreeCtrl->SetFocus();
}

#endif /* ACTIVE_EDITORS */

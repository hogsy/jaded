/*$T BROframe_act.cpp GC 1.134 04/23/04 15:40:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BROwser/BROframe.h"
#include "BROwser/BROtreectrl.h"
#include "BROwser/BROlistctrl.h"
#include "BROwser/BROgrpctrl.h"
#include "BROwser/BROframe_act.h"
#include "BROwser/BROstrings.h"
#include "DIAlogs/DIAfindfile_dlg.h"
#include "EDIapp.h"
#include "EDImainframe.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGgroup.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "Res/Res.h"
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "VAVview/VAVview.h"
#include "EDIeditors_infos.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDIstrings.h"
#include "EDImsg.h"
#include "ENGine/Sources/WORld/WORcheck.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"





#ifdef JADEFUSION
extern BOOL BIG_gb_IgnoreRecent;
extern int	ANI_gi_ImportMode;
extern ULONG LOA_gul_MakeFileRefSpy ;
#else
extern "C" BOOL BIG_gb_IgnoreRecent;
extern "C" int	ANI_gi_ImportMode;
extern "C" ULONG LOA_gul_MakeFileRefSpy ;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnAction(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static EDIA_cl_NameDialog	o_Dialog("Enter key to search");
	EDIA_cl_NameDialog			o_Dlg("Enter filter");
	BIG_KEY						ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_Try_ switch(_ul_Action)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_STRINGFILTER:
		o_Dlg.mo_Name = maz_Filter;
		if(o_Dlg.DoModal() != IDOK) return;
		L_strcpy(maz_Filter, (char *) (LPCSTR) o_Dlg.mo_Name);
		OnRefresh();
		break;

	case EBRO_ACTION_COLLAPSE:
		mpo_TreeCtrl->Collapse();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IMPORT:
		OnImport();
		ANI_gi_ImportMode = -1;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_EXPORT:
		OnExport();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IMPORTMERGE:
		OnImportMerge(0);
		break;
    case EBRO_ACTION_IMPORTMERGEFILT:
		OnImportMerge(1);
		break;
    case EBRO_ACTION_IMPORTMERGEEDITFILT:
		OnImportMergeEdit();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_EXPORTMERGE:
		OnExportMerge(0);
		break;
    case EBRO_ACTION_EXPORTMERGEFILT:
		OnExportMerge(1);
		break;
    case EBRO_ACTION_EXPORTMERGEEDITFILT:
		OnExportMergeEdit();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_EXPORTKEYS:
		OnExportKeys();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	case EBRO_ACTION_IMPORTKEYS:
		OnImportKeys();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ADDFAV:
	{
		
		OnAddFavorites();
	}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ORGFAV:
		OnOrganizeFavorites();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CREATEDIR:
		OnCreateDir();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CREATEFILE:
		OnCreateFile();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CREATESPECIALFILE:
		mb_CreateSpecialFile = TRUE;
		OnCreateFile();
		mb_CreateSpecialFile = FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_RENAME:
		OnRename();
		break;

	case EBRO_ACTION_RENAMEMULTI:
		OnRenameMulti();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CHGKEY:
		OnChangeKey();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_DELETE:
		OnDelete();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_LOGSIZE:
		OnLogSize();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_REFRESH:
		OnRefresh();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ENGINEMODE:
		OnEngineMode();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_FIND:
		go_FindFileDialog.mpo_Browser = this;
		go_FindFileDialog.DoModal();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_FINDKEY:
		if(o_Dialog.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dialog.mo_Name, "%x", &ul_Key);
			ul_Key = BIG_ul_SearchKeyToFat(ul_Key);
			if(ul_Key == BIG_C_InvalidIndex)
				M_MF()->MessageBox("Unknown key", EDI_STR_Csz_Title, MB_ICONINFORMATION | MB_OK);
			else
				i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ul_Key), ul_Key);
		}
		break;

	case EBRO_ACTION_TRACEKEY:
		if(o_Dialog.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dialog.mo_Name, "%x", &ul_Key);
			LOA_gul_MakeFileRefSpy = ul_Key;
		}
		else
		{
			LOA_gul_MakeFileRefSpy = -1;
		}
		break;

	case EBRO_ACTION_SCANKEY:
		if(o_Dialog.DoModal() == IDOK)
		{
            extern void AI_PrintKeyDependencies(BIG_KEY h_Key);
			sscanf((char *) (LPCSTR) o_Dialog.mo_Name, "%x", &ul_Key);
            AI_PrintKeyDependencies(ul_Key);
		}
		break;
        
	case EBRO_ACTION_SCANTRIGGER:
		if(o_Dialog.DoModal() == IDOK)
		{
            extern void AI_PrintTriggerDependencies(const char *s_TriggerName);
            AI_PrintTriggerDependencies((char *) (LPCSTR) o_Dialog.mo_Name);
		}
		break;

	case EBRO_ACTION_LISTUNUSEDGAO:
		{
            extern void AI_PrintUnusedGAO();
            AI_PrintUnusedGAO();
		}
		break;

	case EBRO_ACTION_LISTEXTERNREF:
		{
            extern void AI_PrintExternREF();
            AI_PrintExternREF();
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_FINDNEXT:
		go_FindFileDialog.mpo_Browser = this;
		if(go_FindFileDialog.mo_Name.IsEmpty())
			go_FindFileDialog.DoModal();
		else
			go_FindFileDialog.FindNext();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CHECKIN:
		AfxGetApp()->DoWaitCursor(1);
		OnCheckIn();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EBRO_ACTION_DELDIR:
		AfxGetApp()->DoWaitCursor(1);
		OnDelDirBase();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EBRO_ACTION_CHECKINMIRROR:
		AfxGetApp()->DoWaitCursor(1);
		OnCheckInMirror();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EBRO_ACTION_IGNORERECENT:
		BIG_gb_IgnoreRecent = BIG_gb_IgnoreRecent ? FALSE : TRUE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CHECKINNOOUT:
		AfxGetApp()->DoWaitCursor(1);
		OnCheckInNoOut();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CHECKOUT:
		AfxGetApp()->DoWaitCursor(1);
		OnCheckOut();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_LOCALCHECKOUT:
		AfxGetApp()->DoWaitCursor(1);
		OnLocalCheckOut();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_UNDOCHECKOUT:
		AfxGetApp()->DoWaitCursor(1);
		OnUndoCheckOut();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_GETLVERSION:
		AfxGetApp()->DoWaitCursor(1);
		OnGetLatestVersion();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_PERFORCE_SYNC:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceSync();
		AfxGetApp()->DoWaitCursor(-1);
		break;
	case EBRO_ACTION_PERFORCE_FORCESYNC:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceSync(TRUE);
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EBRO_ACTION_PERFORCE_SERVERSYNC:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceServerSync();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EBRO_ACTION_PERFORCE_EDIT:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceEdit();
		AfxGetApp()->DoWaitCursor(-1);
		break;
	case EBRO_ACTION_PERFORCE_ADD:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceAdd();
		AfxGetApp()->DoWaitCursor(-1);
		break;
	case EBRO_ACTION_PERFORCE_DELETE:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceDelete();
		AfxGetApp()->DoWaitCursor(-1);
		break;
	case EBRO_ACTION_PERFORCE_DIFF:
		AfxGetApp()->DoWaitCursor(1);
   		OnPerforceDiff(); 
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EBRO_ACTION_PERFORCE_HISTORY:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceHistory(); 
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EBRO_ACTION_PERFORCE_SUBMIT:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceSubmit();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EBRO_ACTION_PERFORCE_SUBMITEDIT:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceSubmitEdit();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EBRO_ACTION_PERFORCE_REVERT:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceRevert();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_LINKTOEXT:
		OnLinkToExt(NULL);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IMPFROMEXT:
		OnImpFromExt();
		ANI_gi_ImportMode = -1;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_EXPTOEXT:
		OnExpToExt();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_REFLINKED:
		OnRefreshLinkedPaths();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_UNLINK:
		OnUnlinkPath();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IMPORTONLYLOADED:
		OnImportOnlyLoaded();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IMPORTONLYTRUNCATED:
		OnImportOnlyTruncated();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_SETASMIRROR:
		OnSetAsMirror();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_SETASINVMIRROR:
		OnSetAsInvMirror();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ORDERPATHS:
		OnOrderPaths();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ORDERGRP:
		OnOrderGroup();
		break;

	case EBRO_ACTION_CLEANGRP:
		OnCleanGroup();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CLEARUNLOADED:
		ClearUnloaded();
		break;

	case EBRO_ACTION_GROUPCHECK:
		WORCheck_LoadAllGroup();
		WORCheck_GroupStat();
		break;

	case EBRO_ACTION_GROUPOWNEROF:
		if(LIST_ACTIVATED())
			WORCheck_GroupOwnerOfFile(mpo_ListCtrl->GetItemData(mpo_ListCtrl->GetNextItem(-1, LVIS_SELECTED)));
		else if(mpo_TreeCtrl->GetSelectedItem())
			WORCheck_GroupOwnerOfDir(mpo_TreeCtrl->GetItemData(mpo_TreeCtrl->GetSelectedItem()));
		break;

	case EBRO_ACTION_FILETYPEREFINGROUP:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_NameDialogCombo o_TypeDlg("Type of file");
			char					sz_Ext[8];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			o_TypeDlg.AddItem("gao", 0);
			o_TypeDlg.AddItem("fct", 1);
			o_TypeDlg.AddItem("var", 2);
			o_TypeDlg.AddItem("ofc", 3);
			o_TypeDlg.AddItem("act", 5);
			o_TypeDlg.AddItem("snk", 6);
			o_TypeDlg.SetDefault("gao");

			if(o_TypeDlg.DoModal() != IDOK) break;

			L_strcpy(sz_Ext + 1, (char *) (LPCSTR) o_TypeDlg.mo_Name);
			sz_Ext[0] = '.';
			if(mpo_TreeCtrl->GetSelectedItem() == NULL)
				WORCheck_GaoNotInGroup(sz_Ext, BIG_Root());
			else
				WORCheck_GaoNotInGroup(sz_Ext, mpo_TreeCtrl->GetItemData(mpo_TreeCtrl->GetSelectedItem()));
			break;
		}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ZOOM3D1:
		Zoom3DView(0);
		break;
	case EBRO_ACTION_SELECTGROUP1:
		SelectGroup(0);
		break;
	case EBRO_ACTION_FORCELOAD1:
		ForceLoadObject(0);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
	{
		case EBRO_ACTION_FINDCHECKWORLD:
			FindCheckWorld();
			break;
	}
#endif

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CREATECOB:
		CreateCob();
		break;

	case EBRO_ACTION_DISPLAYINFO:
		GMAT_DisplayInfo();
		break;

	case EBRO_ACTION_CREATEASSSMD:
		{
			/*~~~~~~~~~~*/
			ULONG	index;
			/*~~~~~~~~~~*/

			index = ul_CreateAssociatedSMD();
			if(index != -1) i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(index), index);
		}
		break;

	case EBRO_ACTION_CREATEASSMTX:
		{
			/*~~~~~~~~~~*/
			ULONG	index;
			/*~~~~~~~~~~*/

			index = ul_CreateAssociatedMTX();
			if(index != -1) i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(index), index);
		}
		break;
	}

	_Catch_ _End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EBRO_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UINT					ui_State;
	HTREEITEM				h_Item;
	char					asz_Path[L_MAX_PATH];
	EBRO_tdst_LinkedPath	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ui_State = (UINT) - 1;
	switch(_ul_Action)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IMPORTONLYLOADED:
		h_Item = mpo_TreeCtrl->GetSelectedItem();
		if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Path, TRUE, &pst_Link)) return ui_State;
		ui_State = DFCS_BUTTONCHECK;
		if(pst_Link->ui_Flags & EBRO_C_ImpOnlyLoaded) ui_State |= DFCS_CHECKED;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IMPORTONLYTRUNCATED:
		h_Item = mpo_TreeCtrl->GetSelectedItem();
		if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Path, TRUE, &pst_Link)) return ui_State;
		ui_State = DFCS_BUTTONCHECK;
		if(pst_Link->ui_Flags & EBRO_C_ImpOnlyTruncated) ui_State |= DFCS_CHECKED;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_SETASMIRROR:
		h_Item = mpo_TreeCtrl->GetSelectedItem();
		if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Path, TRUE, &pst_Link)) return ui_State;
		ui_State = DFCS_BUTTONCHECK;
		if(pst_Link->ui_Flags & EBRO_C_Mirror) ui_State |= DFCS_CHECKED;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_SETASINVMIRROR:
		h_Item = mpo_TreeCtrl->GetSelectedItem();
		if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Path, TRUE, &pst_Link)) return ui_State;
		ui_State = DFCS_BUTTONCHECK;
		if(pst_Link->ui_Flags & EBRO_C_InvMirror) ui_State |= DFCS_CHECKED;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IGNORERECENT:
		ui_State = DFCS_BUTTONCHECK;
		if(BIG_gb_IgnoreRecent) ui_State |= DFCS_CHECKED;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ENGINEMODE:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.b_EngineMode) ui_State |= DFCS_CHECKED;
		break;
	}

	return ui_State;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EBRO_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	char			asz_Path[BIG_C_MaxLenPath];
	char			asz_Ext[L_MAX_PATH];
	int				i_Item;
	HTREEITEM		h_Item;
	BIG_INDEX		ul_Index;
	EOUT_cl_Frame	*po_Out;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);

	if(h_Item) BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Path);

	switch(_ul_Action)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EBRO_ACTION_IMPORTMERGEEDITFILT:
    case EBRO_ACTION_EXPORTMERGEEDITFILT:
        return TRUE;

	case EBRO_ACTION_CREATEDIR:
	case EBRO_ACTION_CREATEFILE:
	case EBRO_ACTION_CREATESPECIALFILE:
	case EBRO_ACTION_LINKTOEXT:
	case EBRO_ACTION_IMPORT:
	case EBRO_ACTION_IMPORTKEYS:
	case EBRO_ACTION_IMPORTMERGE:
    case EBRO_ACTION_IMPORTMERGEFILT:
	case EBRO_ACTION_LOGSIZE:
		if(LIST_ACTIVATED()) return FALSE;
		if(GRP_ACTIVATED()) return FALSE;

	case EBRO_ACTION_EXPORT:
	case EBRO_ACTION_EXPORTKEYS:
	case EBRO_ACTION_EXPORTMERGE:
	case EBRO_ACTION_EXPORTMERGEFILT:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_REFLINKED:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mo_LinkedPath.GetCount() == 0) return FALSE;
		return TRUE;

	case EBRO_ACTION_ORDERPATHS:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mo_LinkedPath.GetCount() <= 1) return FALSE;
		return TRUE;

	case EBRO_ACTION_UNLINK:
	case EBRO_ACTION_SETASMIRROR:
	case EBRO_ACTION_SETASINVMIRROR:
	case EBRO_ACTION_IMPORTONLYLOADED:
	case EBRO_ACTION_IMPORTONLYTRUNCATED:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Ext)) return FALSE;
		break;

	case EBRO_ACTION_EXPTOEXT:
	case EBRO_ACTION_IMPFROMEXT:
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Ext, FALSE)) return FALSE;
		if(EBRO_M_EngineMode()) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ADDFAV:
		if(mst_Ini.i_NumFavorites == EBRO_C_MaxFavorites) return FALSE;
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		if(LIST_ACTIVATED()) return FALSE;
		if(GRP_ACTIVATED()) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ORGFAV:
		for(i = 0; i < mst_Ini.i_NumFavorites; i++)
		{
			if(mst_Ini.ast_Favorites[i].asz_DisplayName[0]) return TRUE;
		}

		return FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IGNORERECENT:
		//if(!M_MF()->mst_Ini.b_LinkControlON) return FALSE;
		return TRUE;
	case EBRO_ACTION_LOCALCHECKOUT:
		if(GRP_ACTIVATED()) return FALSE;
		if(LIST_ACTIVATED()) return FALSE;
	case EBRO_ACTION_CHECKIN:
	case EBRO_ACTION_CHECKINNOOUT:
	case EBRO_ACTION_UNDOCHECKOUT:
	case EBRO_ACTION_CHECKOUT:
	case EBRO_ACTION_GETLVERSION:
	case EBRO_ACTION_DELDIR:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		if(!M_MF()->mst_Ini.b_LinkControlON) return FALSE;
		if(GRP_ACTIVATED()) return FALSE;
		return TRUE;
	case EBRO_ACTION_CHECKINMIRROR:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		if(!M_MF()->mst_Ini.b_LinkControlON) return FALSE;
		if(GRP_ACTIVATED()) return FALSE;
		if(LIST_ACTIVATED()) return FALSE;
		return TRUE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_PERFORCE_SYNC:
	case EBRO_ACTION_PERFORCE_EDIT:
	case EBRO_ACTION_PERFORCE_SUBMIT:
	case EBRO_ACTION_PERFORCE_ADD:
	case EBRO_ACTION_PERFORCE_DELETE:
	case EBRO_ACTION_PERFORCE_DIFF:
	case EBRO_ACTION_PERFORCE_HISTORY:
	case EBRO_ACTION_PERFORCE_REVERT:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		if(GRP_ACTIVATED()) return FALSE;
		return TRUE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ORDERGRP:
	case EBRO_ACTION_CLEANGRP:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		if(mpo_ListCtrl->GetSelectedCount() == 0) return FALSE;
		if(mpo_ListCtrl->GetSelectedCount() > 1) return FALSE;
		ul_Index = mpo_ListCtrl->GetItemData(mpo_ListCtrl->GetNextItem(-1, LVIS_SELECTED));
		if(!BIG_b_IsGrpFile(ul_Index)) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CHGKEY:
		if(GRP_ACTIVATED()) return FALSE;
		if(!LIST_ACTIVATED()) return FALSE;
		if(mpo_ListCtrl->GetSelectedCount() > 1) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_RENAMEMULTI:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		if(EDI_go_TheApp.GetFocusedEditor() != this) return FALSE;
		if(LIST_ACTIVATED()) return FALSE;
		if(GRP_ACTIVATED()) return FALSE;
		break;

	case EBRO_ACTION_RENAME:
		if(GRP_ACTIVATED()) return FALSE;

	case EBRO_ACTION_DELETE:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		if(EDI_go_TheApp.GetFocusedEditor() != this) return FALSE;

		if((LIST_ACTIVATED()) || (GRP_ACTIVATED()))
		{
			if((_ul_Action == EBRO_ACTION_RENAME) && (mpo_ListCtrl->GetSelectedCount() > 1)) return FALSE;
		}
		else
		{
			/* Can't rename/delete ROOT directory */
			if(mpo_TreeCtrl->GetItemData(mpo_TreeCtrl->GetSelectedItem()) == 0) return FALSE;
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CREATECOB:
		if(!b_GetOBJ(NULL, EDI_Csz_ExtGraphicObject)) return FALSE;
		if(!(LIST_ACTIVATED())) return FALSE;
		break;

	case EBRO_ACTION_DISPLAYINFO:
		if(!b_GetOBJ(NULL, EDI_Csz_ExtCOLGMAT)) return FALSE;
		if(!(LIST_ACTIVATED())) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CLEARUNLOADED:
		if(EBRO_M_EngineMode()) return FALSE;
		if(mpo_TreeCtrl->GetSelectedItem() == NULL) return FALSE;
		break;
	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
	{
		case EBRO_ACTION_FINDCHECKWORLD:
			po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
			if(po_Out == NULL) return FALSE;
			if(po_Out->mul_CurrentWorld == BIG_C_InvalidIndex) return FALSE;
			//if(!po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_CheckWorldDialog) return FALSE;
			if(!LIST_ACTIVATED())	return FALSE;
			break;
	}
#endif

	case EBRO_ACTION_ZOOM3D1:
		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		if(po_Out == NULL) return FALSE;
		if(po_Out->mul_CurrentWorld == BIG_C_InvalidIndex) return FALSE;
		if(!b_GetOBJ(NULL, EDI_Csz_ExtGameObject)) return FALSE;
		break;

	case EBRO_ACTION_FORCELOAD1:
	case EBRO_ACTION_SELECTGROUP1:
		if(EBRO_M_EngineMode()) return FALSE;
		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		if(po_Out == NULL) return FALSE;
		if(po_Out->mul_CurrentWorld == BIG_C_InvalidIndex) return FALSE;
		if((_ul_Action == EBRO_ACTION_FORCELOAD1) && (!b_GetOBJ(NULL, EDI_Csz_ExtGameObject))) return FALSE;
		if
		(
			(_ul_Action == EBRO_ACTION_SELECTGROUP1)
		&&	(!b_GetOBJ(NULL, EDI_Csz_ExtObjGolGroups))
		&&	(!b_GetOBJ(NULL, EDI_Csz_ExtObjGroups))
		) return FALSE;
		break;

	case EBRO_ACTION_CREATEASSSMD:
		if
		(
			(!b_GetOBJ(NULL, EDI_Csz_ExtSoundMusic))
		&&	(!b_GetOBJ(NULL, EDI_Csz_ExtSoundAmbience))
		&&	(!b_GetOBJ(NULL, EDI_Csz_ExtSoundDialog))
		&&	(!b_GetOBJ(NULL, EDI_Csz_ExtSoundFile))
		&&	(!b_GetOBJ(NULL, EDI_Csz_ExtLoadingSound))
		) return FALSE;
		break;

	case EBRO_ACTION_CREATEASSMTX:
		if((!b_GetOBJ(NULL, EDI_Csz_ExtVideo1))) return FALSE;
		break;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnActionUI(ULONG _ul_Action, CString &_o_Ref, CString &o_Ret)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_Item, i_Item1;
	HTREEITEM		h_Item;
	char			asz_Ext[L_MAX_PATH];
	EOUT_cl_Frame	*po_Out;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_Ret = _o_Ref;
	h_Item = mpo_TreeCtrl->GetSelectedItem();
	i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
	i_Item1 = mpo_GrpCtrl->GetNextItem(-1, LVNI_SELECTED);

	switch(_ul_Action)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IMPORT:
	case EBRO_ACTION_IMPORTMERGE:
	case EBRO_ACTION_IMPORTMERGEFILT:
		if(h_Item)
		{
			o_Ret += EBRO_STR_Csz_PopupImportFiles2;
			o_Ret += mpo_TreeCtrl->GetItemText(h_Item);
			o_Ret += "\"";
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_IMPFROMEXT:
	case EBRO_ACTION_EXPTOEXT:
	case EBRO_ACTION_UNLINK:
		if(b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Ext, FALSE))
		{
			o_Ret += " (";
			o_Ret += asz_Ext;
			o_Ret += ")";
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_EXPORT:
	case EBRO_ACTION_EXPORTMERGE:
	case EBRO_ACTION_EXPORTMERGEFILT:
		if(LIST_ACTIVATED())
		{
			if(mpo_ListCtrl->GetSelectedCount() > 1)
			{
				o_Ret += EBRO_STR_Csz_PopupSelectedFiles;
			}
			else
			{
				o_Ret += EBRO_STR_Csz_PopupFile;
				o_Ret += mpo_ListCtrl->GetItemText(i_Item, 0);
				o_Ret += "\"";
			}
		}
		else
		{
			o_Ret += EBRO_STR_Csz_PopupFolder;
			o_Ret += mpo_TreeCtrl->GetItemText(h_Item);
			o_Ret += "\"";
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_ADDFAV:
	case EBRO_ACTION_ORGFAV:
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CREATEDIR:
	case EBRO_ACTION_CREATEFILE:
	case EBRO_ACTION_CREATESPECIALFILE:
		o_Ret += EBRO_STR_Csz_PopupInFolder;
		o_Ret += mpo_TreeCtrl->GetItemText(h_Item);
		o_Ret += "\"";
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_FORCELOAD1:
		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		o_Ret += BIG_NameFile(po_Out->mul_CurrentWorld);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EBRO_ACTION_CHGKEY:
	case EBRO_ACTION_RENAME:
	case EBRO_ACTION_DELETE:
	case EBRO_ACTION_LOGSIZE:
		if(GRP_ACTIVATED())
		{
			if(mpo_GrpCtrl->GetSelectedCount() == 1)
			{
				o_Ret += EBRO_STR_Csz_PopupShortCut;
				o_Ret += mpo_GrpCtrl->GetItemText(i_Item1, 0);
				o_Ret += "\"";
			}
			else
			{
				o_Ret += EBRO_STR_Csz_PopupSelectedShortCut;
			}
		}
		else if(LIST_ACTIVATED())
		{
			if(mpo_ListCtrl->GetSelectedCount() == 1)
			{
				o_Ret += EBRO_STR_Csz_PopupFile;
				o_Ret += mpo_ListCtrl->GetItemText(i_Item, 0);
				o_Ret += "\"";
			}
			else
			{
				o_Ret += EBRO_STR_Csz_PopupSelectedFiles;
			}
		}
		else
		{
			o_Ret += EBRO_STR_Csz_PopupFolder;
			o_Ret += mpo_TreeCtrl->GetItemText(h_Item);
			o_Ret += "\"";
		}
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EBRO_cl_Frame::i_IsItVarAction(ULONG _ul_Action, EVAV_cl_View *_po_List)
{
	switch(_ul_Action)
	{
	case EBRO_SEPACTION_OPTIONS:
		/* To avoid reset and refresh the list if not necessary */
		if(!_po_List) return 2;
		if(_po_List->mul_ID == EBRO_SEPACTION_OPTIONS) return 2;
		_po_List->mul_ID = EBRO_SEPACTION_OPTIONS;

		_po_List->ResetList();

		/* General */
//		_po_List->AddItem("General", EVAV_EVVIT_Separator, NULL);
//		_po_List->AddItem("Frame Group Auto Hide", EVAV_EVVIT_Bool, &mst_Ini.i_GroupAutoHide);

		/* Import/Export */
		_po_List->AddItem(EBRO_STR_Csz_OptImpExp, EVAV_EVVIT_Separator, NULL);
		_po_List->AddItem(EBRO_STR_Csz_RecImp, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_RecImp);
		_po_List->AddItem(EBRO_STR_Csz_RecExp, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_RecExp);

		/* Data control */
		_po_List->AddItem(EBRO_STR_Csz_OptDataCtrl, EVAV_EVVIT_Separator, NULL);
		_po_List->AddItem(EBRO_STR_Csz_RecIn, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_RecIn);
		_po_List->AddItem(EBRO_STR_Csz_RecOut, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_RecOut);
		_po_List->AddItem(EBRO_STR_Csz_RecUndoOut, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_RecUndoOut);
		_po_List->AddItem(EBRO_STR_Csz_RecGet, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_RecGet);
		_po_List->AddItem("GetLatestVersion Dir as Mirror", EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_MirrorGetLatest);
//		_po_List->AddItem(EBRO_STR_Csz_LinkRef, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_LinkRefresh);

		/* Auto import */
#if 0
		_po_List->AddItem(EBRO_STR_Csz_AutoImp, EVAV_EVVIT_Separator, NULL);
		_po_List->AddItem(EBRO_STR_Csz_RecImp, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_AutoRecImp);
		_po_List->AddItem(EBRO_STR_Csz_RecExp, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_AutoRecExp);
		_po_List->AddItem(EBRO_STR_Csz_AutoOn, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_AutoOn);
		_po_List->AddItem(EBRO_STR_Csz_AutoFreq, EVAV_EVVIT_Int, &mst_Ini.mst_Options.i_AutoFreq, 0, 1, 60, 0);
#endif

		/* Mad importation */
		_po_List->AddItem(EBRO_STR_Csz_MadImp, EVAV_EVVIT_Separator, NULL);
		_po_List->AddItem(EBRO_STR_Csz_ForceImp, EVAV_EVVIT_Bool, &mst_Ini.mst_MadOptions.b_ForceImp);
		_po_List->AddItem(EBRO_STR_Csz_AutomaticImp, EVAV_EVVIT_Bool, &mst_Ini.mst_MadOptions.b_AutoImp);
		_po_List->AddItem
			(
				EBRO_STR_Csz_MaterialUpdate,
				EVAV_EVVIT_ConstInt,
				&mst_Ini.mst_MadOptions.c_MaterialUpdate,
				0,
				0,
				0,
				1,
				0,
				(int) EBRO_STR_Csz_MaterialUpdateOption
			);
		_po_List->AddItem
			(
				EBRO_STR_Csz_MaterialMerge,
				EVAV_EVVIT_ConstInt,
				&mst_Ini.mst_MadOptions.c_MaterialMerge,
				0,
				0,
				0,
				1,
				0,
				(int) EBRO_STR_Csz_MaterialMergeOption
			);
		_po_List->AddItem
			(
				EBRO_STR_Csz_MaterialNoSingle,
				EVAV_EVVIT_ConstInt,
				&mst_Ini.mst_MadOptions.c_MaterialNoSingle,
				0,
				0,
				0,
				1,
				0,
				(int) EBRO_STR_Csz_MaterialNoSingleOption
			);

		_po_List->AddItem
			(
				EBRO_STR_Csz_GraphicObjectUpdate,
				EVAV_EVVIT_ConstInt,
				&mst_Ini.mst_MadOptions.c_GraphicObjectUpdate,
				0,
				0,
				0,
				1,
				0,
				(int) EBRO_STR_Csz_MaterialUpdateOption
			);
		_po_List->AddItem
			(
				EBRO_STR_Csz_GraphicObjectMerge,
				EVAV_EVVIT_ConstInt,
				&mst_Ini.mst_MadOptions.c_GraphicObjectMerge,
				0,
				0,
				0,
				1,
				0,
				(int) EBRO_STR_Csz_MaterialMergeOption
			);
		return 1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnToolBarCommand(UINT nID)
{
	/*~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~*/

	switch(nID)
	{
	case ID_01: ul_Index = BIG_ul_SearchDir(EDI_Csz_Path_Textures); SelectDir(ul_Index); break;
	case ID_02: ul_Index = BIG_ul_SearchDir(EDI_Csz_Path_Objects); SelectDir(ul_Index); break;
	case ID_03: ul_Index = BIG_ul_SearchDir(EDI_Csz_Path_Animation); SelectDir(ul_Index); break;
	case ID_04: ul_Index = BIG_ul_SearchDir(EDI_Csz_Path_Technical); SelectDir(ul_Index); break;
	case ID_05: ul_Index = BIG_ul_SearchDir(EDI_Csz_Path_Audio); SelectDir(ul_Index); break;
	case ID_06: ul_Index = BIG_ul_SearchDir(EDI_Csz_Path_Levels); SelectDir(ul_Index); break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnToolBarCommandUI(UINT nID, CCmdUI *pUI)
{
	pUI->Enable(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EBRO_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	/*~~~~~~~~~~~~~~~~~~*/
	unsigned char	c_Key;
	/*~~~~~~~~~~~~~~~~~~*/

	c_Key = (char) _uw_Key;
	if(_uw_Key & CONTROL)
	{
		switch(c_Key)
		{
		case 'A':	return TRUE;
		}
	}

	return FALSE;
}
#endif

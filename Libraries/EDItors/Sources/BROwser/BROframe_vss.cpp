/*$T BROframe_vss.cpp GC!1.71 02/08/00 13:59:29 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BROframe.h"
#include "BROtreectrl.h"
#include "BROlistctrl.h"
#include "BROstrings.h"
#include "BROerrid.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDImsg.h"
#include "EDIstrings.h"
#include "EDImainframe.h"
#include "EDIapp.h"
#include "ENGine/Sources/ENGinit.h"
#include "DIAlogs/DIAmsglink_dlg.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGkey.h"

extern char                     EDI_gasz_VssHistoryName[L_MAX_PATH];
extern BOOL                     EDI_gb_CheckInDel;
extern BOOL                     EDI_gb_GetLatestDel;
extern EDIA_cl_MsgLinkDialog    EDI_go_MsgGetLatest;
//extern EDIA_cl_MsgLinkDialog    EDI_go_MsgTruncateFiles;
extern EDIA_cl_MsgLinkDialog    EDI_go_MsgUndoCheck;
extern EDIA_cl_MsgLinkDialog    EDI_go_MsgCheckInNo;
extern BOOL EDI_gb_NoUpdateVSS;
extern BOOL EDI_gb_Local;

/*
 =======================================================================================================================
    In:     _i_Type     1 for check in £
                        2 for check out £
                        3 for undo check out £
                        4 for get latest version
						6 local checkout
 =======================================================================================================================
 */
void EBRO_cl_Frame::CommonVss(int _i_Type)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int         i_Item;
    HTREEITEM   h_Item;
    char        asz_Path[BIG_C_MaxLenPath + BIG_C_MaxLenName];
    char        *psz_Name;
    CString     o_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    h_Item = mpo_TreeCtrl->GetSelectedItem();
    i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
	//EDI_go_MsgTruncateFiles.mb_ApplyToAll = FALSE;
	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;

    /* For history */
    if(_i_Type == 1)
    {
        if
        (
            M_MF()->MessageBox
                (
                    "You will update linked bigfile",
                    EDI_STR_Csz_TitleConfirm,
                    MB_ICONQUESTION | MB_OKCANCEL
                ) != IDOK
        )
        {
            return;
        }
    }

    AfxGetApp()->DoWaitCursor(1);

_Try_
    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Request for a file.
     -------------------------------------------------------------------------------------------------------------------
     */

	mb_NoUpdateVss = TRUE;
    if(LIST_ACTIVATED())
    {
        ERR_X_Assert(i_Item < mpo_ListCtrl->GetItemCount());

        i_Item = -1;
        BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Path);
        while((i_Item = mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED)) != -1)
        {
            o_String = mpo_ListCtrl->GetItemText(i_Item, 0);
            psz_Name = (char *) (LPCSTR) o_String;

            switch(_i_Type)
            {
            case 1:
				EDI_gb_CheckInDel = FALSE;
                M_MF()->CheckInFile(asz_Path, psz_Name);
				EDI_gb_NoUpdateVSS = TRUE;
				LINK_gb_RefreshOnlyFiles = TRUE;
                break;
            case 2:
                M_MF()->CheckOutFile(asz_Path, psz_Name);
				EDI_gb_NoUpdateVSS = TRUE;
				LINK_gb_RefreshOnlyFiles = TRUE;
                break;
            case 3:
                M_MF()->UndoCheckOutFile(asz_Path, psz_Name);
				EDI_gb_NoUpdateVSS = TRUE;
				LINK_gb_RefreshOnlyFiles = TRUE;
                break;
            case 4:
				mb_NoUpdateVss = TRUE;
                M_MF()->GetLatestVersionFile(asz_Path, psz_Name);
				EDI_gb_NoUpdateVSS = TRUE;
				LINK_gb_RefreshOnlyFiles = TRUE;
                break;
            case 5:
				mb_NoUpdateVss = TRUE;
				EDI_gb_CheckInDel = FALSE;
                M_MF()->CheckInFileNoOut(asz_Path, psz_Name);
				EDI_gb_NoUpdateVSS = TRUE;
				LINK_gb_RefreshOnlyFiles = TRUE;
                break;
            }

            if(LINK_gb_EscapeDetected) break;
        }

		EDI_gb_NoUpdateVSS = FALSE;
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Request for a directory.
     -------------------------------------------------------------------------------------------------------------------
     */
    else
    {
        ERR_X_Assert(h_Item != NULL);
        BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Path);

        switch(_i_Type)
        {
        case 1:
            EDI_gb_RecurseVss = mst_Ini.mst_Options.b_RecIn;
            M_MF()->CheckInDir(asz_Path);
			EDI_gb_NoUpdateVSS = TRUE;
            EDI_gb_CheckInDel = FALSE;
			LINK_gb_RefreshOnlyFiles = TRUE;
            break;

        case 2:
            EDI_gb_RecurseVss = mst_Ini.mst_Options.b_RecOut;
		mst_Ini.mst_Options.b_MirrorGetLatest = FALSE;
            EDI_gb_GetLatestDel = mst_Ini.mst_Options.b_MirrorGetLatest;
            M_MF()->CheckOutDir(asz_Path);
			EDI_gb_NoUpdateVSS = TRUE;
            EDI_gb_GetLatestDel = FALSE;
			if(!mst_Ini.mst_Options.b_MirrorGetLatest) LINK_gb_RefreshOnlyFiles = TRUE;
            break;

        case 3:
            EDI_gb_RecurseVss = mst_Ini.mst_Options.b_RecUndoOut;
            M_MF()->UndoCheckOutDir(asz_Path);
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
            break;

        case 4:
			mb_NoUpdateVss = TRUE;
            EDI_gb_RecurseVss = mst_Ini.mst_Options.b_RecGet;
		mst_Ini.mst_Options.b_MirrorGetLatest = FALSE;
            EDI_gb_GetLatestDel = mst_Ini.mst_Options.b_MirrorGetLatest;
            M_MF()->GetLatestVersionDir(asz_Path);
			EDI_gb_NoUpdateVSS = TRUE;
            EDI_gb_GetLatestDel = FALSE;
            break;

        case 5:
			mb_NoUpdateVss = TRUE;
            EDI_gb_RecurseVss = mst_Ini.mst_Options.b_RecIn;
            M_MF()->CheckInDirNoOut(asz_Path);
            EDI_gb_CheckInDel = FALSE;
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
            break;

        case 6:
            EDI_gb_RecurseVss = mst_Ini.mst_Options.b_RecOut;
			EDI_gb_Local = TRUE;
            M_MF()->LocalCheckOutDir(asz_Path);
			EDI_gb_Local = FALSE;
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
            break;
        }

		EDI_gb_NoUpdateVSS = FALSE;
    }

_Catch_
_End_
    LINK_PrintStatusMsg("OK");
    AfxGetApp()->DoWaitCursor(-1);
    M_MF()->FatHasChanged();
	LINK_gb_EscapeDetected = FALSE;
	EDI_gb_NoUpdateVSS = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnCheckIn(void)
{
	EDI_gb_CheckInDel = FALSE;
    CommonVss(1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnDelDirBase(void)
{
	HTREEITEM	h_Item;
	char		asz_Path[BIG_C_MaxLenPath];

    if
    (
        M_MF()->MessageBox
            (
                "This will destroy the directory on base",
                EDI_STR_Csz_TitleConfirm,
                MB_ICONQUESTION | MB_OKCANCEL
            ) != IDOK
    )
    {
        return;
    }

    h_Item = mpo_TreeCtrl->GetSelectedItem();
    BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Path);

	if(!M_MF()->b_LockBigFile()) return;
	if(BIG_ul_SearchDir(asz_Path) != BIG_C_InvalidIndex) BIG_DelDir(asz_Path);
	M_MF()->REFFileChanged();
	SWAP_BIG();
	if(BIG_ul_SearchDir(asz_Path) != BIG_C_InvalidIndex) BIG_DelDir(asz_Path);
	M_MF()->CloseRefFile();
	RESTORE_BIG();
	M_MF()->UnLockBigFile();
    M_MF()->FatHasChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnCheckInMirror(void)
{
//	EDI_gb_CheckInDel = TRUE;
    CommonVss(1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnCheckInNoOut(void)
{
	EDI_gb_CheckInDel = FALSE;
    CommonVss(5);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnCheckOut(void)
{
    CommonVss(2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnLocalCheckOut(void)
{
    CommonVss(6);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnUndoCheckOut(void)
{
    CommonVss(3);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnGetLatestVersion(void)
{
    CommonVss(4);
}

#endif /* ACTIVE_EDITORS */

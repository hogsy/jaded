/*$T BROframe_impex.cpp GC! 1.081 09/27/02 09:01:38 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGcheck.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGexport.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BROframe.h"
#include "BROtreectrl.h"
#include "BROlistctrl.h"
#include "BROstrings.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "EDImainframe.h"
#include "EDIerrid.h"
#include "EDIapp.h"
#include "EDIpaths.h"
#include "ENGine/Sources/ENGinit.h"
#include "DIAlogs/DIAorder_dlg.h"
#include "EDIapp_options.h"
#include "ENGine/Sources/WORld/WORimportfrommad.h"

#include "EDImsg.h"

#include "EDItors/Sources/BROwser/BROmsg.h"
#include "DIAlogs\DIAname_dlg.h"
#include "DIAlogs/DIAbrofilter_dlg.h"
#include "DIAlogs/DIAmsglink_dlg.h"

#include "BIGfiles/STReams/STReamBigFile.h"
#include "DATaControl/DATCUtils.h"
#include "SOuNd\SONframe.h"

/* Save time for auto importation (global to all instances of browser) */
static L_time_t gx_LastTimeAutoImport = 0;


#define Csz_ImportMergeFilterFile   EDI_Csz_Path_EditData"/ImportMergeFilter.fct"
#define Csz_ExportMergeFilterFile   EDI_Csz_Path_EditData"/ExportMergeFilter.fct"

ULONG BRO_ul_LoadFilter(char* _asz_FilterFileName);
void BRO_UnloadFilter(void);
void BRO_DoAutoCheckOutFile(void);

BOOL BRO_b_FileIsExcluded(ULONG ul_Index);
BOOL BRO_b_FileIsForced(ULONG ul_Index);

BOOL BRO_gb_AutoCheckOutFile = FALSE;
CStringArray BRO_go_coPathList;
CStringArray BRO_go_coFileList;
CStringArray BRO_go_coDirList;

CList<CString, CString> EDI_go_ForceDirList;
BAS_tdst_barray     EDI_gst_ForceIdKeyList;
BAS_tdst_barray     EDI_gst_ForceFileKeyList;

CList<CString, CString> EDI_go_ExcludeDirList;
BAS_tdst_barray     EDI_gst_ExcludeIdKeyList;
BAS_tdst_barray     EDI_gst_ExcludeFileKeyList;

BOOL        EDI_gb_FIM_TestDate = FALSE;
BOOL        EDI_gb_FIM_Mirror = TRUE;
BOOL        BIG_gb_IgnoreRecent_Sav;
BOOL        BIG_gb_WarningTime_Sav;

#ifdef JADEFUSION
extern BOOL		BIG_gb_WarningTime ;
extern BOOL		BIG_gb_IgnoreRecent ;
extern BOOL		BIG_gb_IgnoreDateExt ;
#else
extern "C" BOOL		BIG_gb_WarningTime ;
extern "C" BOOL		BIG_gb_IgnoreRecent ;
extern "C" BOOL		BIG_gb_IgnoreDateExt ;
#endif

extern BOOL EDI_gb_EdiMerge;
extern char EDI_gaz_EdiMergeDir[];
extern char EDI_gaz_EdiBigfileSource[];


extern EDIA_cl_MsgLinkDialog    EDI_go_MsgGetLatest;
//extern EDIA_cl_MsgLinkDialog    EDI_go_MsgTruncateFiles;
extern EDIA_cl_MsgLinkDialog    EDI_go_MsgUndoCheck;
extern EDIA_cl_MsgLinkDialog    EDI_go_MsgCheckInNo;
extern BOOL					     EDI_can_lock ;
extern BOOL                     EDI_gb_NoUpdateVSS;

#ifdef JADEFUSION
extern BOOL                     LINK_gb_RefreshOnlyFiles ;
#else
extern "C" BOOL                     LINK_gb_RefreshOnlyFiles ;
#endif
extern BOOL EDI_gb_SoundCheck;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnImport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File(EBRO_STR_Csz_TitleImport, 3, 1);
	ULONG				ul_Dir;
	HTREEITEM			h_Item;
	char				asz_PathName[BIG_C_MaxLenPath];
	char				asz_Temp[BIG_C_MaxLenPath];
	char				asz_Temp1[BIG_C_MaxLenPath];
	int					i_Num;
	char				*psz_Temp;
	CString				o_Temp;
	BOOL				b_AnimCase;
	char				*sz_Ext;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Import files
		 ---------------------------------------------------------------------------------------------------------------
		 */

		h_Item = mpo_TreeCtrl->GetSelectedItem();
		ul_Dir = mpo_TreeCtrl->GetItemData(h_Item);

		AfxGetApp()->DoWaitCursor(1);
		LINK_BeginLongOperation(EBRO_STR_Csz_PopupImportFiles);

_Try_
		/* Compute full path of parent dir in bigfile, and add file to bigfile. */
		BIG_ComputeFullName(ul_Dir, asz_PathName);

		/* set extern parameters for importation */
		WOR_gl_ImportForced = mst_Ini.mst_MadOptions.b_ForceImp;
		WOR_gl_ImportAutomatic = mst_Ini.mst_MadOptions.b_AutoImp;
		WOR_gl_ImportGrm = mst_Ini.mst_MadOptions.c_MaterialUpdate | mst_Ini.mst_MadOptions.c_MaterialMerge | mst_Ini.mst_MadOptions.c_MaterialNoSingle;
		WOR_gl_ImportGro = mst_Ini.mst_MadOptions.c_GraphicObjectUpdate | mst_Ini.mst_MadOptions.c_GraphicObjectMerge;

		for(i_Num = 1; i_Num <= o_File.mi_NumFiles; i_Num++)
		{
			/* Get file name */
			o_File.GetItem(o_File.mo_File, i_Num, o_Temp);
			psz_Temp = (char *) (LPCSTR) o_Temp;

			 sz_Ext = strrchr(psz_Temp, '.');

			 b_AnimCase = sz_Ext && !L_strcmp(sz_Ext, ".trl");

			/* Check validity of name */
			if(BIG_b_CheckName(psz_Temp) == FALSE) continue;

			/* Compute full name on disk of current file */
			L_strcpy(asz_Temp, o_File.masz_FullPath);
			if(asz_Temp[L_strlen(asz_Temp) - 1] != '\\') L_strcat(asz_Temp, "\\");
			L_strcat(asz_Temp, psz_Temp);

			/* Print imported file name in status */
			if(!b_AnimCase) LINK_PrintStatusMsg(asz_Temp);
			if(LINK_gb_EscapeDetected) break;

			BIG_UpdateFileFromDisk(asz_Temp, asz_PathName, psz_Temp);
		}

		WOR_Import_ResetFlags();

		LINK_EndLongOperation();
		LINK_BeginLongOperation(EBRO_STR_Csz_PopupImportDirs);

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Import dirs
		 ---------------------------------------------------------------------------------------------------------------
		 */

		h_Item = mpo_TreeCtrl->GetSelectedItem();
		ERR_X_Assert(h_Item != NULL);
		ul_Dir = mpo_TreeCtrl->GetItemData(h_Item);

		/* Compute full path of parent dir in bigfile, and add file to bigfile. */
		BIG_ComputeFullName(ul_Dir, asz_PathName);

		for(i_Num = 1; i_Num <= o_File.mi_NumPaths; i_Num++)
		{
			if(LINK_gb_EscapeDetected) break;

			/* Get dir name */
			o_File.GetItem(o_File.mo_Path, i_Num, o_Temp);
			psz_Temp = (char *) (LPCSTR) o_Temp;

			/* Check validity of name */
			if(BIG_b_CheckName(psz_Temp) == FALSE) continue;

			/* Compute full name on disk of current dir */
			L_strcpy(asz_Temp, o_File.masz_FullPath);
			if((asz_Temp[L_strlen(asz_Temp) - 1] != '\\') && (asz_Temp[L_strlen(asz_Temp) - 1] != '/'))
			{
				L_strcat(asz_Temp, "/");
			}

			L_strcat(asz_Temp, psz_Temp);

			/* Compute full name on bigfile of current dir */
			L_strcpy(asz_Temp1, asz_PathName);
			L_strcat(asz_Temp1, "/");
			L_strcat(asz_Temp1, psz_Temp);

			BIG_gb_ImportRecurse = mst_Ini.mst_Options.b_RecImp;	/* Recursive or not ? */
			BIG_UpdateDirFromDisk(asz_Temp, asz_Temp1);

			if(LINK_gb_EscapeDetected) break;
		}

		LINK_EndLongOperation();
		M_MF()->FatHasChanged();

_Catch_
_End_
		AfxGetApp()->DoWaitCursor(-1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::CommonExport(char *_psz_Ext)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Item;
	int			i_Item;
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Temp[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
	AfxGetApp()->DoWaitCursor(1);

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	ERR_X_Assert(h_Item != NULL);
	BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Path);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Export files
	 -------------------------------------------------------------------------------------------------------------------
	 */

	i_Item = mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED);
	if(LIST_ACTIVATED())
	{
		LINK_BeginLongOperation(EBRO_STR_Csz_PopupExportFile);

		i_Item = -1;
		while((i_Item = mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED)) != -1)
		{
			/* Add selected file to output path */
			L_strcpy(asz_Temp, _psz_Ext);
			L_strcat(asz_Temp, "/");
			L_strcat(asz_Temp, (char *) (LPCSTR) mpo_ListCtrl->GetItemText(i_Item, 0));

			BIG_ExportFileToDisk(asz_Temp, asz_Path, (char *) (LPCSTR) mpo_ListCtrl->GetItemText(i_Item, 0));

			mpo_ListCtrl->SetItemState(i_Item, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		}

		LINK_EndLongOperation();
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Export dirs
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else
	{
		LINK_BeginLongOperation(EBRO_STR_Csz_PopupExportDir);

		/* Add selected dir to output path */
		L_strcpy(asz_Temp, _psz_Ext);
		L_strcat(asz_Temp, "/");
		L_strcat(asz_Temp, (char *) (LPCSTR) mpo_TreeCtrl->GetItemText(h_Item));
		BIG_ExportDirToDisk(asz_Temp, asz_Path);

		LINK_EndLongOperation();
	}

	AfxGetApp()->DoWaitCursor(-1);

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnExport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File(EBRO_STR_Csz_TitleExport, 2, FALSE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		BIG_gb_ExportRecurse = mst_Ini.mst_Options.b_RecExp;	/* Recursive or not ? */
		CommonExport(o_File.masz_FullPath);
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EBRO_cl_Frame::OnExportKeys(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File(EBRO_STR_Csz_TitleExport, 2, FALSE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		std::string fullPath = o_File.masz_FullPath;
		fullPath += "/";
		DAT_CUtils::Export(fullPath, std::string(""));
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EBRO_cl_Frame::OnImportKeys(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File(EBRO_STR_Csz_TitleImport, 2, FALSE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		std::string fullPath = o_File.masz_FullPath;
		fullPath += "/";
		DAT_CUtils::Import(fullPath, std::string(""));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnImportMerge(int iFilter)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File(EBRO_STR_Csz_ChooseImportBig, 1);
	EDIA_cl_FileDialog	o_File1(EBRO_STR_Csz_ChooseImport, 3, TRUE, TRUE);
	char				asz_Temp[L_MAX_PATH];
	char				*psz_Temp;
	CString				o_Temp;
	int					i_Num;
	BIG_INDEX			ul_Index;
    HWND                h_Focus;
    EDIA_cl_BroFilterDialog * po_Filter=NULL;
    int iRes;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    if(iFilter)
    {
        po_Filter = new EDIA_cl_BroFilterDialog(Csz_ImportMergeFilterFile);

        BRO_gb_AutoCheckOutFile = TRUE;
        BRO_go_coPathList.RemoveAll();
        BRO_go_coFileList.RemoveAll();
        BRO_go_coDirList.RemoveAll();
    }
     

_Try_

    BIG_gpsz_RealName = NULL;
    iRes = IDOK;
    if(!EDI_gb_EdiMerge) iRes = o_File.DoModal();

	if(iRes == IDOK)
	{
		AfxGetApp()->DoWaitCursor(1);

		/* Get full bigfile name */
        if(EDI_gb_EdiMerge)
        {
            L_strcpy(asz_Temp, EDI_gaz_EdiBigfileSource);
        }
        else
        {
		    L_strcpy(asz_Temp, o_File.masz_FullPath);
		    L_strcat(asz_Temp, "/");
		    o_File.GetItem(o_File.mo_File, 0, o_Temp);
		    L_strcat(asz_Temp, (char *) (LPCSTR) o_Temp);

        }


		/* Save current bigfile */
		COPY_BIG();
		BIG_Open(asz_Temp);

        h_Focus = ::GetFocus();
        ::SetFocus( NULL );

        iRes = IDOK;
        if(EDI_gb_EdiMerge)
        {
            ul_Index = BIG_ul_SearchDir(EDI_gaz_EdiMergeDir);
		    if(ul_Index == BIG_C_InvalidIndex) goto l_Skip;
		    
            o_File1.mo_Path = CString(BIG_NameDir(ul_Index));
            o_File1.mi_NumPaths = 1;

            BIG_ComputeFullName(BIG_ParentDir(ul_Index), o_File1.masz_FullPath);

            o_File1.mi_NumFiles = 0;
            o_File1.mo_File = CString("");
        }
        else
        {
            iRes = o_File1.DoModal();
        }

		if(iRes == IDOK)
		{
            if(po_Filter  && EDI_gb_FIM_Mirror)
            {
		        if
		        (!EDI_gb_EdiMerge &&
			        (
				        MessageBox
					        (
						        "Do you want to make a mirror ?\nThis can erase some files on your bigfile. IT IS SAFER TO CLOSE ALL YOUR WORLDS BEFORE !!!",
						        (char *) (LPCSTR) o_File1.mo_Path,
						        MB_ICONERROR | MB_YESNO
					        )
			        ) == IDYES
		        )
                {
                    //go back to the bigfile we want to write to
                    SWAP_BIG();

			        //delete dir
                    for(i_Num = 1; i_Num <= o_File1.mi_NumPaths; i_Num++)
			        {
				        /* Get dir name */
				        o_File1.GetItem(o_File1.mo_Path, i_Num, o_Temp);
				        psz_Temp = (char *) (LPCSTR) o_Temp;

				        /* Check validity of name */
				        if(BIG_b_CheckName(psz_Temp) == FALSE) continue;

				        /* Compute full name on disk of current dir */
				        L_strcpy(asz_Temp, o_File1.masz_FullPath);
				        if((asz_Temp[L_strlen(asz_Temp) - 1] != '\\') && (asz_Temp[L_strlen(asz_Temp) - 1] != '/'))
				        {
					        L_strcat(asz_Temp, "/");
				        }

				        L_strcat(asz_Temp, psz_Temp);

				        ul_Index = BIG_ul_SearchDir(asz_Temp);
				        if(ul_Index == BIG_C_InvalidIndex)
				        {
					        ERR_X_ForceError(EDI_ERR_Csz_UnknowPath, asz_Temp);
					        break;
				        }

                        if(BRO_b_DirIsExcluded(asz_Temp)) continue;

                        BRO_AutoCheckOutDir(BIG_NameDir(BIG_ParentDir(ul_Index)));
                        BIG_DelDir(asz_Temp);
				        if(LINK_gb_EscapeDetected) goto l_Skip;
			        }

			        //delete files
                    for(i_Num = 1; i_Num <= o_File1.mi_NumFiles; i_Num++)
			        {
				        /* Get dir name */
				        o_File1.GetItem(o_File1.mo_File, i_Num, o_Temp);
				        psz_Temp = (char *) (LPCSTR) o_Temp;

				        /* Check validity of name */
				        if(BIG_b_CheckName(psz_Temp) == FALSE) continue;

				        /* Export to main bigfile (so import, that's the same) */
				        ul_Index = BIG_ul_SearchDir(o_File1.masz_FullPath);
				        if(ul_Index == BIG_C_InvalidIndex)
				        {
					        ERR_X_ForceError(EDI_ERR_Csz_UnknowPath, o_File1.masz_FullPath);
					        break;
				        }
                        
                        BRO_AutoCheckOutDir(BIG_NameDir(ul_Index));

				        ul_Index = BIG_ul_SearchFile(ul_Index, psz_Temp);
				        if(ul_Index == BIG_C_InvalidIndex)
				        {
					        ERR_X_ForceError(EDI_ERR_Csz_UnknowFile, psz_Temp);
					        break;
				        }

                        if( BRO_b_FileIsForced(ul_Index) || !BRO_b_FileIsExcluded(ul_Index) )
                        {
                            BRO_AutoCheckOutFile(o_File1.masz_FullPath, psz_Temp);
                            BIG_DelFile(o_File1.masz_FullPath, psz_Temp);
                        }
                        
                        if(LINK_gb_EscapeDetected) goto l_Skip;
			        }

                    //go back to the bigfile we want to read from
                    SWAP_BIG();
                }
            }


            /*$2
			 -----------------------------------------------------------------------------------------------------------
			    Merge paths
			 -----------------------------------------------------------------------------------------------------------
			 */

			LINK_BeginLongOperation(EBRO_STR_Csz_PopupImportDirs);
			for(i_Num = 1; i_Num <= o_File1.mi_NumPaths; i_Num++)
			{
				/* Get dir name */
				o_File1.GetItem(o_File1.mo_Path, i_Num, o_Temp);
				psz_Temp = (char *) (LPCSTR) o_Temp;

				/* Check validity of name */
				if(BIG_b_CheckName(psz_Temp) == FALSE) continue;

				/* Compute full name on disk of current dir */
				L_strcpy(asz_Temp, o_File1.masz_FullPath);
				if((asz_Temp[L_strlen(asz_Temp) - 1] != '\\') && (asz_Temp[L_strlen(asz_Temp) - 1] != '/'))
				{
					L_strcat(asz_Temp, "/");
				}

				L_strcat(asz_Temp, psz_Temp);

				/* Export to main bigfile (so import, that's the same) */
				ul_Index = BIG_ul_SearchDir(asz_Temp);
				if(ul_Index == BIG_C_InvalidIndex)
				{
					ERR_X_ForceError(EDI_ERR_Csz_UnknowPath, asz_Temp);
					break;
				}

				BIG_gb_MergeRecurse = mst_Ini.mst_Options.b_RecImp;
				BIG_ExportMergeDirRec(asz_Temp, ul_Index);
				if(LINK_gb_EscapeDetected) goto l_Skip;
			}


			/*$2
			 -----------------------------------------------------------------------------------------------------------
			    Merge files
			 -----------------------------------------------------------------------------------------------------------
			 */

			LINK_BeginLongOperation(EBRO_STR_Csz_PopupImportFiles);
			for(i_Num = 1; i_Num <= o_File1.mi_NumFiles; i_Num++)
			{
				/* Get dir name */
				o_File1.GetItem(o_File1.mo_File, i_Num, o_Temp);
				psz_Temp = (char *) (LPCSTR) o_Temp;

				/* Check validity of name */
				if(BIG_b_CheckName(psz_Temp) == FALSE) continue;

				/* Export to main bigfile (so import, that's the same) */
				ul_Index = BIG_ul_SearchDir(o_File1.masz_FullPath);
				if(ul_Index == BIG_C_InvalidIndex)
				{
					ERR_X_ForceError(EDI_ERR_Csz_UnknowPath, o_File1.masz_FullPath);
					break;
				}

				ul_Index = BIG_ul_SearchFile(ul_Index, psz_Temp);
				if(ul_Index == BIG_C_InvalidIndex)
				{
					ERR_X_ForceError(EDI_ERR_Csz_UnknowFile, psz_Temp);
					break;
				}

                if( BRO_b_FileIsForced(ul_Index) || !BRO_b_FileIsExcluded(ul_Index) )
                {
                    BRO_AutoCheckOutFile(o_File1.masz_FullPath, psz_Temp);
                    BIG_ul_ExportMergeFileRec(o_File1.masz_FullPath, psz_Temp);
                }
                
                if(LINK_gb_EscapeDetected) goto l_Skip;
			}
		}

l_Skip:
		BIG_Close();
		RESTORE_BIG();

        ::SetFocus( h_Focus );

		M_MF()->FatHasChanged();
		AfxGetApp()->DoWaitCursor(-1);
	}

_Catch_
_End_
	RESTORE_BIG();

    if(iFilter)
    {
        if(po_Filter) delete po_Filter;
        BRO_DoAutoCheckOutFile();
        BRO_gb_AutoCheckOutFile = FALSE;
    }
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnExportMerge(int iFilter)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File(EBRO_STR_Csz_ChooseExportBig, 1);
	CString				o_Temp;
	HTREEITEM			h_Item;
	int					i_Item;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Temp[L_MAX_PATH];
	BIG_INDEX			ul_Res;
    EDIA_cl_BroFilterDialog * po_Filter=NULL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    if(iFilter)
    {
        po_Filter = new EDIA_cl_BroFilterDialog(Csz_ExportMergeFilterFile);
    }


_Try_
	if(o_File.DoModal() == IDOK)
	{
		AfxGetApp()->DoWaitCursor(1);

		/* Get full bigfile name */
		L_strcpy(asz_Temp, o_File.masz_FullPath);
		L_strcat(asz_Temp, "/");
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		L_strcat(asz_Temp, (char *) (LPCSTR) o_Temp);

		/* Get source path name */
		h_Item = mpo_TreeCtrl->GetSelectedItem();
		ERR_X_Assert(h_Item != NULL);
		BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Path);

        if(iFilter && EDI_gb_FIM_Mirror)
        {
		    if
		    (
			    (
				    MessageBox
					    (
						    "Do you want to make a mirror ?\nThis can erase some files on your bigfile. IT IS SAFER TO CLOSE ALL YOUR WORLDS BEFORE !!!",
						    "",
						    MB_ICONERROR | MB_YESNO
					    )
			    ) == IDYES
		    )
            {
		        COPY_BIG();
		        BIG_Open(asz_Temp);
                BIG_DelDir(asz_Path);
                SWAP_BIG();
            }
        }

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Export one or more files
		 ---------------------------------------------------------------------------------------------------------------
		 */

		i_Item = mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED);
		if(LIST_ACTIVATED())
		{

			LINK_BeginLongOperation(EBRO_STR_Csz_PopupExportFile);

			i_Item = -1;
			while((i_Item = mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED)) != -1)
			{
                
                if(iFilter)
                {   // if filtering we need to find the key
                    ULONG ul_Dir, ul_File;
				    ul_Dir = BIG_ul_SearchDir(asz_Path);
				    if(ul_Dir == BIG_C_InvalidIndex)
				    {
					    ERR_X_ForceError(EDI_ERR_Csz_UnknowPath, asz_Path);
					    break;
				    }

				    ul_File = BIG_ul_SearchFile(ul_Dir, asz_Temp);
				    if(ul_File == BIG_C_InvalidIndex)
				    {
					    ERR_X_ForceError(EDI_ERR_Csz_UnknowFile, asz_Temp);
					    break;
				    }

                    if( BRO_b_FileIsExcluded(ul_File) && !BRO_b_FileIsForced(ul_File) ) continue;                    
                }


                ul_Res = BIG_ul_ExportMergeFile
					(
						asz_Temp,
						asz_Path,
						(char *) (LPCSTR) mpo_ListCtrl->GetItemText(i_Item, 0)
					);

				if(LINK_gb_EscapeDetected) break;

				mpo_ListCtrl->SetItemState(i_Item, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			}
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Export a dir
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else
		{
			LINK_BeginLongOperation(EBRO_STR_Csz_PopupExportDir);
			BIG_gb_MergeRecurse = mst_Ini.mst_Options.b_RecExp;
			BIG_ExportMergeDir(asz_Temp, asz_Path);
		}

		LINK_EndLongOperation();
		AfxGetApp()->DoWaitCursor(-1);
	}

_Catch_
_End_
	RESTORE_BIG()

    if(iFilter)
    {
        if(po_Filter) delete po_Filter ;
    }
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LinkedPathCB(ULONG _ul1, ULONG _ul2)
{
	/*~~~~~~~~*/
	int i_Order;
	/*~~~~~~~~*/

	i_Order = ((EBRO_tdst_LinkedPath *) _ul1)->i_Order;
	((EBRO_tdst_LinkedPath *) _ul1)->i_Order = ((EBRO_tdst_LinkedPath *) _ul2)->i_Order;
	((EBRO_tdst_LinkedPath *) _ul2)->i_Order = i_Order;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnOrderPaths(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_OrderDialog		o_Dlg("Order paths");
	POSITION				pos;
	CString					key;
	EBRO_tdst_LinkedPath	*pst_Link;
	int						i_Order, i_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Add all paths */
	i_Num = 0;
	i_Order = -1;
	while(i_Num < mo_LinkedPath.GetCount())
	{
		pos = mo_LinkedPath.GetStartPosition();
		while(pos)
		{
			mo_LinkedPath.GetNextAssoc(pos, key, (CObject * &) pst_Link);
			if(pst_Link->i_Order == i_Order + 1)
			{
				key += " => ";
				key += pst_Link->asz_Path;
				o_Dlg.AddItem((char *) (LPCSTR) key, (ULONG) pst_Link);
				i_Order++;
				i_Num++;
				break;
			}
			else if(!pos) i_Order++;
		}
	}

	/* Set callback */
	o_Dlg.SetCallback(LinkedPathCB);
	o_Dlg.DoModal();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnImportOnlyLoaded(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM				h_Item;
	char					asz_Path[L_MAX_PATH];
	EBRO_tdst_LinkedPath	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Path, TRUE, &pst_Link)) return;
	pst_Link->ui_Flags ^= EBRO_C_ImpOnlyLoaded;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnImportOnlyTruncated(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM				h_Item;
	char					asz_Path[L_MAX_PATH];
	EBRO_tdst_LinkedPath	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Path, TRUE, &pst_Link)) return;
	pst_Link->ui_Flags ^= EBRO_C_ImpOnlyTruncated;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnSetAsMirror(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM				h_Item;
	char					asz_Path[L_MAX_PATH];
	EBRO_tdst_LinkedPath	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Path, TRUE, &pst_Link)) return;
	if(pst_Link->ui_Flags & EBRO_C_Mirror)
		pst_Link->ui_Flags &= ~EBRO_C_Mirror;
	else
		pst_Link->ui_Flags |= EBRO_C_Mirror;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnSetAsInvMirror(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM				h_Item;
	char					asz_Path[L_MAX_PATH];
	EBRO_tdst_LinkedPath	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Path, TRUE, &pst_Link)) return;
	if(pst_Link->ui_Flags & EBRO_C_InvMirror)
		pst_Link->ui_Flags &= ~EBRO_C_InvMirror;
	else
		pst_Link->ui_Flags |= EBRO_C_InvMirror;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::ReadLinkedPath(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EBRO_tdst_LinkedPath	*pst_Link;
	char					asz_Name[BIG_C_MaxLenName];
	char					*pc_Buf;
	ULONG					ul_Size, ul_Num;
	BIG_INDEX				ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	DeleteLinkedPath();
	L_strcpy(asz_Name, mst_Def.asz_Name);
	L_strcat(asz_Name, EDI_Csz_ExtLnk);
	ul_Index = BIG_ul_SearchFileExt(EDI_Csz_Ini, asz_Name);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		pc_Buf = BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), &ul_Size);
		if(!ul_Size) return;

		/* Number of linked paths */
		ul_Num = *(ULONG *) pc_Buf;
		pc_Buf += 4;

		/* Read all linked paths */
		while(ul_Num)
		{
			pst_Link = (EBRO_tdst_LinkedPath *) L_malloc(sizeof(EBRO_tdst_LinkedPath));
			L_memcpy(pst_Link, pc_Buf, sizeof(EBRO_tdst_LinkedPath));
			mo_LinkedPath.SetAt(pst_Link->asz_PathSrc, (CObject *) pst_Link);
			pc_Buf += sizeof(EBRO_tdst_LinkedPath);
			ul_Size -= sizeof(EBRO_tdst_LinkedPath);
			ul_Num--;
		}

		/* Extensions */
		while(ul_Size)
		{
			ul_Size--;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::SaveLinkedPath(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EBRO_tdst_LinkedPath	*pst_Link;
	char					asz_Name[BIG_C_MaxLenName];
	POSITION				pos;
	CString					key;
	ULONG					ul_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_strcpy(asz_Name, mst_Def.asz_Name);
	L_strcat(asz_Name, EDI_Csz_ExtLnk);
	SAV_Begin(EDI_Csz_Ini, asz_Name);

	/* Number of linked paths */
	ul_Num = mo_LinkedPath.GetCount();
	SAV_Buffer(&ul_Num, 4);

_Try_
	pos = mo_LinkedPath.GetStartPosition();
	while(pos)
	{
		mo_LinkedPath.GetNextAssoc(pos, key, (CObject * &) pst_Link);
		L_strcpy(pst_Link->asz_PathSrc, (char *) (LPCSTR) key);
		SAV_Buffer(pst_Link, sizeof(EBRO_tdst_LinkedPath));
	}

	SAV_ul_End();
_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::DeleteLinkedPath(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION				pos;
	CString					key;
	EBRO_tdst_LinkedPath	*pst_Linked;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mo_LinkedPath.GetStartPosition();
	while(pos)
	{
		mo_LinkedPath.GetNextAssoc(pos, key, (CObject * &) pst_Linked);
		L_free(pst_Linked);
	}

	mo_LinkedPath.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EBRO_cl_Frame::b_GetLinkedPath(BIG_INDEX _ul_Index, char *_psz_Out, BOOL _b_Real, EBRO_tdst_LinkedPath **_pRet)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					asz_Name[BIG_C_MaxLenPath];
	char					*psz_Temp, *psz_Temp1;
	EBRO_tdst_LinkedPath	*pst_Linked;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* First search the path itself */
	BIG_ComputeFullName(_ul_Index, asz_Name);
	if(mo_LinkedPath.Lookup(asz_Name, (CObject * &) pst_Linked))
	{
		L_strcpy(_psz_Out, pst_Linked->asz_Path);
		if(_pRet) *_pRet = pst_Linked;
		return TRUE;
	}

	/* If not found, search a key for a parent directory */
	if(!_b_Real)
	{
		psz_Temp = L_strrchr(asz_Name, '/');
		while(psz_Temp)
		{
			*psz_Temp = 0;
			if(mo_LinkedPath.Lookup(asz_Name, (CObject * &) pst_Linked))
			{
				L_strcpy(_psz_Out, pst_Linked->asz_Path);
				L_strcat(_psz_Out, "/");
				L_strcat(_psz_Out, psz_Temp + 1);
				if(_pRet) *_pRet = pst_Linked;
				return TRUE;
			}

			psz_Temp1 = psz_Temp;
			psz_Temp = L_strrchr(asz_Name, '/');
			*psz_Temp1 = '/';
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnUnlinkPath(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM				h_Item;
	char					asz_Path[BIG_C_MaxLenPath];
	EBRO_tdst_LinkedPath	*pst_Linked, *pst_Linked1;
	POSITION				pos;
	CString					key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	ERR_X_Assert(h_Item != NULL);
	BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Path);
	if(mo_LinkedPath.Lookup(asz_Path, (CObject * &) pst_Linked))
	{
		/* Decrease order */
		pos = mo_LinkedPath.GetStartPosition();
		while(pos)
		{
			mo_LinkedPath.GetNextAssoc(pos, key, (CObject * &) pst_Linked1);
			if(pst_Linked1->i_Order > pst_Linked->i_Order) pst_Linked1->i_Order--;
		}

		L_free(pst_Linked);
		mo_LinkedPath.RemoveKey(asz_Path);
	}

	RefreshAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnLinkToExt(char *_asz_LinkPath)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog		o_Dlg(EBRO_STR_Csz_TitleLink, 2, FALSE);
	HTREEITEM				h_Item;
	char					asz_Path[BIG_C_MaxLenPath];
	CString					o_Temp;
	EBRO_tdst_LinkedPath	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_asz_LinkPath)
	{
		if(o_Dlg.DoModal() == IDOK) _asz_LinkPath = (char *) (LPCSTR) o_Dlg.masz_FullPath;
	}

	if(_asz_LinkPath)
	{
		/* Get path name */
		h_Item = mpo_TreeCtrl->GetSelectedItem();
		ERR_X_Assert(h_Item != NULL);
		BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Path);

		/* Add in list */
		pst_Link = (EBRO_tdst_LinkedPath *) L_malloc(sizeof(EBRO_tdst_LinkedPath));
		L_strcpy(pst_Link->asz_Path, _asz_LinkPath);
		pst_Link->ui_Flags = 0;
		pst_Link->i_Order = mo_LinkedPath.GetCount();
		mo_LinkedPath.SetAt(asz_Path, (CObject *) pst_Link);
		RefreshAll();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnImpFromExt(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM				h_Item;
	int						i_Item;
	char					asz_Path[BIG_C_MaxLenPath];
	char					asz_Ext[L_MAX_PATH];
	char					asz_Temp[BIG_C_MaxLenPath];
	EBRO_tdst_LinkedPath	*pst_Link;
	BIG_INDEX				ul_DirRef;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BIG_gb_ImportWithExt = TRUE;	/* Filter files with extension */

_Try_
	AfxGetApp()->DoWaitCursor(1);

	/* Get full path of selected dir */
	h_Item = mpo_TreeCtrl->GetSelectedItem();
	ERR_X_Assert(h_Item != NULL);
	BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Path);

	/* Get linked path */
	if(!b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Ext, FALSE, &pst_Link)) _Return_(;);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Import files
	 -------------------------------------------------------------------------------------------------------------------
	 */

	M_MF()->LockDisplay(this);

	i_Item = mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED);
	if(LIST_ACTIVATED())
	{
		LINK_BeginLongOperation(EBRO_STR_Csz_PopupImportFiles);

		i_Item = -1;
		while((i_Item = mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED)) != -1)
		{
			L_strcpy(asz_Temp, asz_Ext);
			L_strcat(asz_Temp, "/");
			L_strcat(asz_Temp, (char *) (LPCSTR) mpo_ListCtrl->GetItemText(i_Item, 0));

			if(!L_access(asz_Temp, 0))
			{
				BIG_UpdateFileFromDisk(asz_Temp, asz_Path, (char *) (LPCSTR) mpo_ListCtrl->GetItemText(i_Item, 0));
			}

			mpo_ListCtrl->SetItemState(i_Item, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		}

		LINK_EndLongOperation();
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Import dirs
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else
	{
		LINK_BeginLongOperation(EBRO_STR_Csz_PopupImportDirs);

		BIG_gb_ImportRecurse = mst_Ini.mst_Options.b_RecImp;	/* Recursive or not ? */
		BIG_gb_ImportDirMirror = pst_Link->ui_Flags & EBRO_C_Mirror ? TRUE : FALSE;
		BIG_gb_ImportDirInvMirror = pst_Link->ui_Flags & EBRO_C_InvMirror ? TRUE : FALSE;
		BIG_gb_ImportDirOnlyLoaded = pst_Link->ui_Flags & EBRO_C_ImpOnlyLoaded ? TRUE : FALSE;
		BIG_gb_ImportDirOnlyTruncated = pst_Link->ui_Flags & EBRO_C_ImpOnlyTruncated ? TRUE : FALSE;

		if(pst_Link->ui_Flags & EBRO_C_Mirror)
		{
			ul_DirRef = BIG_ul_SearchDir(asz_Path);
			BIG_UntouchFilesRec(ul_DirRef);
		}

		BIG_UpdateDirFromDisk(asz_Ext, asz_Path);

		if(pst_Link->ui_Flags & EBRO_C_Mirror)
		{
			MirrorImport(asz_Path, pst_Link->asz_Path);
			BIG_UntouchFilesRec(ul_DirRef);
		}

		BIG_gb_ImportDirMirror = FALSE;
		BIG_gb_ImportDirInvMirror = FALSE;
		BIG_gb_ImportDirOnlyLoaded = FALSE;
		BIG_gb_ImportDirOnlyTruncated = FALSE;

		LINK_EndLongOperation();
	}

	M_MF()->FatHasChanged();
	M_MF()->UnlockDisplay(this);
	AfxGetApp()->DoWaitCursor(-1);

_Catch_
_End_
	BIG_gb_ImportWithExt = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnExpToExt(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Item;
	char		asz_Ext[L_MAX_PATH];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get linked path */
	h_Item = mpo_TreeCtrl->GetSelectedItem();
	if(b_GetLinkedPath(mpo_TreeCtrl->GetItemData(h_Item), asz_Ext, FALSE))
	{
		BIG_gb_ExportRecurse = mst_Ini.mst_Options.b_AutoRecExp;	/* Recursive or not ? */
		CommonExport(asz_Ext);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnRefreshLinkedPaths(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION				pos;
	CString					key;
	EBRO_tdst_LinkedPath	*pst_Link;
	int						i_Order, i_Num;
	char					asz_Date[10];
	char					asz_Time[10];
	char					asz_Msg[255];
	BIG_INDEX				ul_DirRef;
	BOOL					b_Touch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mo_LinkedPath.GetCount() == 0) return;

	BIG_gb_ImportWithExt = TRUE;								/* Filter files with extension */

	/* Banner for auto importation */
	L_strdate(asz_Date);
	L_strtime(asz_Time);
	LINK_gul_ColorTxt = 0x00A00000;
	LINK_PrintStatusMsg("---------------------------------------------");
	sprintf(asz_Msg, "Starting auto importation %s %s", asz_Date, asz_Time);
	LINK_PrintStatusMsg(asz_Msg);
	LINK_PrintStatusMsg("---------------------------------------------");
	LINK_gul_ColorTxt = 0;

_Try_
	AfxGetApp()->DoWaitCursor(1);
	LINK_BeginLongOperation(EBRO_STR_Csz_PopupImportDirs);

	BIG_gb_ImportRecurse = mst_Ini.mst_Options.b_AutoRecImp;	/* Recursive or not ? */
	BIG_gb_ImportOne = FALSE;									/* No change in bigfile for now... */

	i_Num = 0;
	i_Order = -1;
	while(i_Num < mo_LinkedPath.GetCount())
	{
		pos = mo_LinkedPath.GetStartPosition();
		b_Touch = FALSE;
		while(pos)
		{
			mo_LinkedPath.GetNextAssoc(pos, key, (CObject * &) pst_Link);
			if(pst_Link->i_Order == i_Order + 1)
			{
				BIG_gb_ImportDirMirror = pst_Link->ui_Flags & EBRO_C_Mirror ? TRUE : FALSE;
				BIG_gb_ImportDirInvMirror = pst_Link->ui_Flags & EBRO_C_InvMirror ? TRUE : FALSE;
				BIG_gb_ImportDirOnlyLoaded = pst_Link->ui_Flags & EBRO_C_ImpOnlyLoaded ? TRUE : FALSE;
				BIG_gb_ImportDirOnlyTruncated = pst_Link->ui_Flags & EBRO_C_ImpOnlyTruncated ? TRUE : FALSE;

				if(pst_Link->ui_Flags & EBRO_C_Mirror)
				{
					ul_DirRef = BIG_ul_SearchDir((char *) (LPCSTR) key);
					BIG_UntouchFilesRec(ul_DirRef);
				}

				BIG_UpdateDirFromDisk(pst_Link->asz_Path, (char *) (LPCSTR) key);

				if(pst_Link->ui_Flags & EBRO_C_Mirror)
				{
					MirrorImport((char *) (LPCSTR) key, pst_Link->asz_Path);
					BIG_UntouchFilesRec(ul_DirRef);
				}

				BIG_gb_ImportDirMirror = FALSE;
				BIG_gb_ImportDirInvMirror = FALSE;
				BIG_gb_ImportDirOnlyLoaded = FALSE;
				BIG_gb_ImportDirOnlyTruncated = FALSE;
				i_Order++;
				i_Num++;
				b_Touch = TRUE;
				break;
			}
		}

		if(!b_Touch)
		{
			i_Order++;
			i_Num++;
		}
	}

	LINK_EndLongOperation();

	/* Ask to refresh editors if at least one change */
	if(BIG_gb_ImportOne) M_MF()->FatHasChanged();

	AfxGetApp()->DoWaitCursor(-1);
_Catch_
_End_
	BIG_gb_ImportWithExt = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::MirrorImport(char *_psz_Big, char *_psz_Disk)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_DirIndex;
	char	sz_TrashDir[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_DirIndex = BIG_ul_SearchDir(_psz_Big);
	if(ul_DirIndex == BIG_C_InvalidIndex) return;

	/* Textures directory management */
	if(BIG_b_IsDirInDirRec(BIG_ul_SearchDir(EDI_Csz_Path_Textures), ul_DirIndex))
	{
		L_strcpy(sz_TrashDir, EDI_Csz_Path_TrashCan_Textures);
		L_strcat(sz_TrashDir, _psz_Big + L_strlen(EDI_Csz_Path_Textures));
		BIG_MoveUnchangedFiles(ul_DirIndex, sz_TrashDir, EDI_Csz_ExtTexture1 "\0");
		return;
	}

	/* Levels and object directories management : do nothing cause it's already done */
	if(BIG_b_IsDirInDirRec(BIG_ul_SearchDir(EDI_Csz_Path_Objects), ul_DirIndex)) return;
	if(BIG_b_IsDirInDirRec(BIG_ul_SearchDir(EDI_Csz_Path_Levels), ul_DirIndex)) return;

	/* Gamedata directories */
	if(BIG_b_IsDirInDirRec(BIG_ul_SearchDir(EDI_Csz_Path_GameData), ul_DirIndex))
	{
		L_strcpy(sz_TrashDir, EDI_Csz_Path_TrashCan);
		L_strcat(sz_TrashDir, _psz_Big + L_strlen(EDI_Csz_Path_GameData));
		BIG_MoveUnchangedFiles(ul_DirIndex, sz_TrashDir, NULL);
		return;
	}

	/* Others directory */
	if(BIG_b_IsDirInDirRec(BIG_ul_SearchDir(BIG_Csz_Root), ul_DirIndex))
	{
		L_strcpy(sz_TrashDir, EDI_Csz_Path_TrashCan);
		L_strcat(sz_TrashDir, _psz_Big + L_strlen(BIG_Csz_Root));
		BIG_MoveUnchangedFiles(ul_DirIndex, sz_TrashDir, NULL);
		return;
	}

	/* All case are treated except gig dir that aren't in root dir !!! */
	return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnRealIdle(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	L_time_t	x_Time;
	extern char EDI_gaz_EdiImportSource[L_MAX_PATH];
	extern char EDI_gaz_EdiImportDestination[L_MAX_PATH];
	extern BOOL EDI_gb_EdiImport;
	ULONG		ul_Index;
	BOOL		b_exit=FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_Ini.mst_Options.b_AutoOn || EDI_gb_SlashQ)
	{
		if(EDI_gb_SlashQ)
		{
			BIG_b_CheckFile(FALSE);
		}

		time(&x_Time);
		if(EDI_gb_SlashQ || (x_Time - gx_LastTimeAutoImport >= mst_Ini.mst_Options.i_AutoFreq))
		{
			LINK_BeginLongOperation(EBRO_STR_Csz_PopupImport);
			LINK_PrintStatusMsg("");

			LINK_gb_CanDisplay = FALSE;
			OnRefreshLinkedPaths();
			LINK_gb_CanDisplay = TRUE;

			LINK_EndLongOperation();
			gx_LastTimeAutoImport = time(&x_Time);
		}

		if(EDI_gb_SlashQ)
		{
			BIG_b_CheckFile(FALSE);
			M_MF()->SendMessage(WM_CLOSE);
		}
	}

	if(EDI_gb_EdiImport)
	{
		/* sel sound dir */
		ul_Index = BIG_ul_SearchDir(EDI_gaz_EdiImportDestination);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			char	asz_Temp[L_MAX_PATH];
			char	asz_dst[128];
			char	asz_src[128];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(strlen(EDI_gaz_EdiImportSource) > 32)
			{
				sprintf(asz_src, "[...]%s", &EDI_gaz_EdiImportSource[strlen(EDI_gaz_EdiImportSource) - 32 + 6]);
			}
			else
			{
				sprintf(asz_src, "%s", EDI_gaz_EdiImportSource);
			}

			if(strlen(EDI_gaz_EdiImportDestination) > 32)
			{
				sprintf
				(
					asz_dst,
					"[...]%s",
					&EDI_gaz_EdiImportDestination[strlen(EDI_gaz_EdiImportDestination) - 32 + 6]
				);
			}
			else
			{
				sprintf(asz_dst, "%s", EDI_gaz_EdiImportSource);
			}

			sprintf(asz_Temp, "Importing %s -> %s ...", asz_src, asz_dst);
			EDI_gpo_EnterWnd->DisplayMessage(asz_Temp);

			SelectDir((BIG_INDEX) ul_Index);
			OnLinkToExt(EDI_gaz_EdiImportSource);
			OnImpFromExt();

			EDI_gb_EdiImport = FALSE;
		}
		b_exit = TRUE;
	}

	if(EDI_gb_SoundCheck)
	{
		ESON_cl_Frame	*po_Sound;
		po_Sound = (ESON_cl_Frame*) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
		po_Sound->OnAction(ESON_ACTION_CHECKALLFILES);
		b_exit = TRUE;
	}

	if(b_exit) ExitProcess(0);

	if(EDI_gb_EdiMerge)
	{
        char	asz_Temp[L_MAX_PATH];
        sprintf(asz_Temp, "Merging %s ...", EDI_gaz_EdiMergeDir);
		EDI_gpo_EnterWnd->DisplayMessage(asz_Temp);

        OnImportMerge(1);
		ExitProcess(0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BRO_UnloadFilter(void)
{
    BAS_bfree(&EDI_gst_ForceIdKeyList);
    BAS_bfree(&EDI_gst_ForceFileKeyList);
    EDI_go_ForceDirList.RemoveAll();

    BAS_bfree(&EDI_gst_ExcludeIdKeyList);
    BAS_bfree(&EDI_gst_ExcludeFileKeyList);
    EDI_go_ExcludeDirList.RemoveAll();

    BIG_gb_IgnoreRecent = BIG_gb_IgnoreRecent_Sav;
    BIG_gb_WarningTime= BIG_gb_WarningTime_Sav ;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BRO_b_FileIsExcluded(ULONG ul_Index)
{
    ULONG ul_FileKey, ul_FileIdKey;
    char asz_Path[BIG_C_MaxLenPath];
    char *pz;
    POSITION pos;
    int j;
    
    
    ul_FileKey = BIG_FileKey(ul_Index);
    if(BAS_bsearch(ul_FileKey, &EDI_gst_ExcludeFileKeyList) != -1) return TRUE;

    ul_FileIdKey = ul_FileKey & 0xFF000000;
    if(BAS_bsearch(ul_FileIdKey, &EDI_gst_ExcludeIdKeyList) != -1) return TRUE;


    BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

    pos = EDI_go_ExcludeDirList.GetHeadPosition();
    while(pos)
    {
        pz = (char *) (LPCSTR) EDI_go_ExcludeDirList.GetNext(pos);
        j = L_strlen(pz);

        if(!L_strncmp(asz_Path, pz, j)) return TRUE;
    }

    return FALSE;
}

BOOL BRO_b_DirIsExcluded(char*asz_Path)
{
    char *pz;
    POSITION pos;
    int j;

    pos = EDI_go_ExcludeDirList.GetHeadPosition();
    while(pos)
    {
        pz = (char *) (LPCSTR) EDI_go_ExcludeDirList.GetNext(pos);
        j = L_strlen(pz);

        if(!L_strncmp(asz_Path, pz, j)) return TRUE;
    }

    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BRO_AutoCheckOutFile(char*_asz_Path, char*_asz_Name)
{
    if(!BRO_gb_AutoCheckOutFile) return;
    int nb = BRO_go_coPathList.GetCount();
    BRO_go_coPathList.SetAtGrow(nb, CString(_asz_Path));
    BRO_go_coFileList.SetAtGrow(nb, CString(_asz_Name));
}

void BRO_AutoCheckOutDir(char*_asz_Path)
{
    if(!BRO_gb_AutoCheckOutFile) return;
    int nb = BRO_go_coDirList.GetCount();
    BRO_go_coDirList.SetAtGrow(nb, CString(_asz_Path));
}

void BRO_DoAutoCheckOutFile(void)
{
    CString oPath, oFile;

    if(!BRO_gb_AutoCheckOutFile) return;

	//EDI_go_MsgTruncateFiles.mb_ApplyToAll = FALSE;
	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;

	if(M_MF()->b_LockBigFile())
    {
        EDI_can_lock = FALSE;
        while(BRO_go_coPathList.GetCount())
        {
            oPath = BRO_go_coPathList.GetAt(0);
            oFile = BRO_go_coFileList.GetAt(0);

            BRO_go_coPathList.RemoveAt(0);
            BRO_go_coFileList.RemoveAt(0);

            M_MF()->CheckOutFile((char *) (LPCSTR) oPath, (char *) (LPCSTR) oFile);
        }

        while(BRO_go_coDirList.GetCount())
        {
            oPath = BRO_go_coDirList.GetAt(0);
            BRO_go_coDirList.RemoveAt(0);
 
            M_MF()->CheckOutDir((char *) (LPCSTR) oPath);
        }

        EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
    }
    else
    {
        ERR_X_Warning(0, "Data control failure : we cannot lock the data base => there is no automatic check-out", NULL);
    }

	EDI_gb_NoUpdateVSS = TRUE;	
	LINK_gb_RefreshOnlyFiles = TRUE;
	M_MF()->FatHasChanged();
}

BOOL BRO_b_FileIsForced(ULONG ul_Index)
{
    ULONG ul_FileKey, ul_FileIdKey;
    char asz_Path[BIG_C_MaxLenPath];
    char *pz;
    POSITION pos;
    int j;
    
    
    ul_FileKey = BIG_FileKey(ul_Index);
    if(BAS_bsearch(ul_FileKey, &EDI_gst_ForceFileKeyList) != -1) return TRUE;

    ul_FileIdKey = ul_FileKey & 0xFF000000;
    if(BAS_bsearch(ul_FileIdKey, &EDI_gst_ForceIdKeyList) != -1) return TRUE;


    BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

    pos = EDI_go_ForceDirList.GetHeadPosition();
    while(pos)
    {
        pz = (char *) (LPCSTR) EDI_go_ForceDirList.GetNext(pos);
        j = L_strlen(pz);

        if(!L_strncmp(asz_Path, pz, j)) 
        {
            if(asz_Path[j] && ((asz_Path[j] != '/') && (asz_Path[j] != '\\')))
                continue;

            return TRUE;
        }
    }

    return FALSE;
}


BOOL BRO_b_DirIsForced(char *asz_Path)
{
    char *pz;
    POSITION pos;
    int j;
    
    pos = EDI_go_ForceDirList.GetHeadPosition();
    while(pos)
    {
        pz = (char *) (LPCSTR) EDI_go_ForceDirList.GetNext(pos);
        j = L_strlen(pz);

        if(!L_strncmp(asz_Path, pz, j)) 
        {
            if(asz_Path[j] && ((asz_Path[j] != '/') && (asz_Path [j] != '\\')))
                continue;

            return TRUE;
        }
    }

    return FALSE;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG BRO_ul_LoadFilter(char* _asz_FilterFileName)
{
    char asz_Dir[BIG_C_MaxLenPath];
    char asz_File[BIG_C_MaxLenPath];
    char *pz,*pc;
    ULONG ul_Index, ul_Size, ul_Key;
    CString oName;

    pz = _asz_FilterFileName;
    while(*pz) { if(*pz == '\\') *pz = '/'; pz++;}

    L_strcpy(asz_Dir, _asz_FilterFileName);
    pz = L_strrchr(asz_Dir, '/');
    if(*pz) *pz=0;


    pz = L_strrchr(_asz_FilterFileName, '/');
    if(pz)
        L_strcpy(asz_File, pz+1);
    else
        L_strcpy(asz_File, _asz_FilterFileName);


    ul_Index = BIG_ul_SearchDir(asz_Dir);
    if(ul_Index == BIG_C_InvalidIndex)
	{
	    //ERR_X_Warning(0, "cannot find the filter file", NULL);
		return BIG_C_InvalidIndex;
	}

	ul_Index = BIG_ul_SearchFile(ul_Index, asz_File);
    if(ul_Index == BIG_C_InvalidIndex)
	{
	    //ERR_X_Warning(0, "cannot find the filter file", NULL);
		return BIG_C_InvalidIndex;
	}

    BAS_binit(&EDI_gst_ForceIdKeyList, 50);
    BAS_binit(&EDI_gst_ForceFileKeyList, 50);
    EDI_go_ForceDirList.RemoveAll();

    BAS_binit(&EDI_gst_ExcludeIdKeyList, 50);
    BAS_binit(&EDI_gst_ExcludeFileKeyList, 50);
    EDI_go_ExcludeDirList.RemoveAll();

    EDI_gb_FIM_Mirror = TRUE;
    EDI_gb_FIM_TestDate = FALSE;

    pz =pc = BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), &ul_Size);
    
    while(*pc && ((ULONG)(pc-pz) <= ul_Size))
    {
        switch(*pc)
        {
        case 'o':pc+=2;  //OPTION--------------------------------
            if(strncmp(pc, "testdate-off", 12) == 0)
                EDI_gb_FIM_TestDate = FALSE;
            else if(strncmp(pc, "testdate-on", 11) == 0)
                EDI_gb_FIM_TestDate = TRUE;
            else if(strncmp(pc, "mirror-on", 9) == 0)
                EDI_gb_FIM_Mirror = TRUE;
            else if(strncmp(pc, "mirror-off", 10) == 0)
                EDI_gb_FIM_Mirror = FALSE;
            pc = L_strchr(pc, 0x0a)+1;
           break;

        case 'f':pc++;  //FORCE---------------------------------
            switch(*pc)
            {
            case 'f':   //file
                pc += 2;
                sscanf(pc, "%x", &ul_Key);
                pc = L_strchr(pc, 0x0a)+1;
                BAS_binsert(ul_Key, ul_Key, &EDI_gst_ForceFileKeyList);
                break;

            case 'd':   //directory
                pc += 2;
                L_memcpy(asz_Dir, pc, min(sizeof(asz_Dir), L_strlen(pc)+1));
                if(L_strchr(asz_Dir, 0x0a)) *L_strchr(asz_Dir, 0x0a) = 0;
                pc += L_strlen(asz_Dir)+1;
                EDI_go_ForceDirList.AddHead(CString(asz_Dir));
                break;

            case 'n':   //bigkey
                pc += 2;
                sscanf(pc, "%x", &ul_Key);
                pc = L_strchr(pc, 0x0a)+1;
                BAS_binsert(ul_Key, ul_Key, &EDI_gst_ForceIdKeyList);
                break;
            default:pc++;break;
            }
            break;


        case 'e':pc++;  //EXCLUDE---------------------------------
            switch(*pc)
            {
            case 'f':   //file
                pc += 2;
                sscanf(pc, "%x", &ul_Key);
                pc = L_strchr(pc, 0x0a)+1;
                BAS_binsert(ul_Key, ul_Key, &EDI_gst_ExcludeFileKeyList);
                break;

            case 'd':   //directory
                pc += 2;
                L_memcpy(asz_Dir, pc, min(sizeof(asz_Dir), L_strlen(pc)+1));
                if(L_strchr(asz_Dir, 0x0a)) *L_strchr(asz_Dir, 0x0a) = 0;
                pc += L_strlen(asz_Dir)+1;

                ul_Key = BIG_ul_SearchDir(asz_Dir);
                EDI_go_ExcludeDirList.AddHead(CString(asz_Dir));
                break;

            case 'n':   //bigkey
                pc += 2;
                sscanf(pc, "%x", &ul_Key);
                pc = L_strchr(pc, 0x0a)+1;
                BAS_binsert(ul_Key, ul_Key, &EDI_gst_ExcludeIdKeyList);
                break;
            default:pc++;break;
            }
            break;
        default:pc++;break;
        }
    }

    BIG_gb_IgnoreRecent_Sav = BIG_gb_IgnoreRecent;
    BIG_gb_WarningTime_Sav = BIG_gb_WarningTime ;
    if(EDI_gb_FIM_TestDate)
    {
        BIG_gb_IgnoreRecent = FALSE;
        BIG_gb_WarningTime = TRUE;
    }
    else
    {
        BIG_gb_IgnoreRecent = TRUE;
        BIG_gb_WarningTime = FALSE;
    }

    return ul_Index;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnImportMergeEdit(void)
{
    EDIA_cl_BroFilterDialog*po_Filter ;
    po_Filter = new EDIA_cl_BroFilterDialog(Csz_ImportMergeFilterFile);
    po_Filter->DoModal();
    delete po_Filter;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnExportMergeEdit(void)
{
    EDIA_cl_BroFilterDialog*po_Filter ;
    po_Filter = new EDIA_cl_BroFilterDialog(Csz_ExportMergeFilterFile);
    po_Filter->DoModal();
    delete po_Filter;
}

#endif /* ACTIVE_EDITORS */

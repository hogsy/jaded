/*$T BROframe_mdfy.cpp GC! 1.097 09/25/01 12:57:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include <vector>
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BROframe.h"
#include "BROtreectrl.h"
#include "BROlistctrl.h"
#include "BROgrpctrl.h"
#include "BROstrings.h"
#include "BROerrid.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDImsg.h"
#include "EDIstrings.h"
#include "EDImainframe.h"
#include "ENGine/Sources/ENGinit.h"
#include "EDIapp.h"
#include "DIAlogs/DIAorder_dlg.h"
#include "TEXture/TEXfile.h"
#include "SOuNd/SONmsg.h"
#include "EDIpaths.h"
#include "LINks/LINKtoed.h"
#include "PERForce/PERmsg.h"

void ListOpenedFilesInDir( BIG_INDEX ulDir, std::vector< ULONG >& lstKeys )
{
	// process files in this folder
	BIG_INDEX ulFile = BIG_FirstFile( ulDir );
	while( ulFile != BIG_C_InvalidIndex )
	{
		if( BIG_P4Action( ulFile )[0] != '\0' )
			lstKeys.push_back( ulFile );

		ulFile = BIG_NextFile( ulFile );
	}

	// recursively process sub-folders
	BIG_INDEX ulSubDir = BIG_SubDir(ulDir);
	while(ulSubDir != BIG_C_InvalidIndex)
	{
		ListOpenedFilesInDir( ulSubDir, lstKeys );
		ulSubDir = BIG_NextDir(ulSubDir);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnCreateDir(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EBRO_STR_Csz_TitleEnterFolderName);
	HTREEITEM			h_Item;
	char				asz_Temp[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
	if(o_Dialog.DoModal() == IDOK)
	{
		/* Check validity of name */
		if(BIG_b_CheckName((char *) (LPCSTR) o_Dialog.mo_Name) == FALSE) return;

		h_Item = mpo_TreeCtrl->GetSelectedItem();
		ERR_X_Assert(h_Item != NULL);

		BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Temp);
		L_strcat(asz_Temp, "/");
		L_strcat(asz_Temp, (char *) (LPCSTR) o_Dialog.mo_Name);

		if(BIG_ul_SearchDir(asz_Temp) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError(EBRO_ERR_Csz_DirAlreadyExists, asz_Temp);
		}
		else
		{
			BIG_ul_CreateDir(asz_Temp);
			M_MF()->FatHasChanged();
		}
	}

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnCreateFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EBRO_STR_Csz_TitleEnterFileName);
	HTREEITEM			h_Item;
	char				asz_Temp[BIG_C_MaxLenPath];
	char				*psz_Name;
    char*pz;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
	if(o_Dialog.DoModal() == IDOK)
	{
		/* Check validity of name */
		psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
		if(BIG_b_CheckName(psz_Name) == FALSE) return;

		h_Item = mpo_TreeCtrl->GetSelectedItem();
		ERR_X_Assert(h_Item != NULL);

		BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Temp);

		if(BIG_ul_SearchFileExt(asz_Temp, psz_Name) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError(EBRO_ERR_Csz_FileAlreadyExists, psz_Name);
		}
		else
		{
			if(mb_CreateSpecialFile)
			{
				if(b_CreateSpecialFile(asz_Temp, psz_Name)) M_MF()->FatHasChanged();
			}
			else
			{
                /* put extension in lower case */
	            pz = L_strrchr(psz_Name, '.');
	            if(pz)
	            {
		            while(*(++pz)) *pz = tolower(*pz);
	            }
                
				BIG_ul_CreateFile(asz_Temp, psz_Name);
				M_MF()->FatHasChanged();
			}
		}
	}

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EBRO_cl_Frame::b_CreateSpecialFile(char *_psz_Path, char *_sz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialogCombo o_TypeDlg("Type of file");
	TEX_tdst_File_Params	st_Params;
	char					*psz_Ext, sz_Name[BIG_C_MaxLenName];
	GRO_tdst_Struct			*pst_Gro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
	if(!mb_CreateSpecialFile) return FALSE;
	mb_CreateSpecialFile = FALSE;

	o_TypeDlg.AddItem("pro (Procedural texture)", 0);
    o_TypeDlg.AddItem("ant (Animated texture)", 1);
	o_TypeDlg.AddItem("spr (Sprite Generator)", 2);
	o_TypeDlg.AddItem("grm (Material)", 3);
	o_TypeDlg.AddItem("grm (Multi-Material)", 4);
	o_TypeDlg.SetDefault("pro (Procedural texture)");

	if(o_TypeDlg.DoModal() != IDOK) return FALSE;

	strcpy(sz_Name, _sz_Name);
	psz_Ext = L_strrchr(sz_Name, '.');
	if(!psz_Ext)
	{
		psz_Ext = sz_Name + strlen(sz_Name);
		*psz_Ext = '.';
	}

	L_memcpy(psz_Ext + 1, (char *) (LPCSTR) o_TypeDlg.mo_Name, 3);
	psz_Ext[4] = 0;

	if(o_TypeDlg.mi_CurSelData == 0)
	{
		TEX_File_InitParams(&st_Params);
		st_Params.uc_Type = TEX_FP_ProFile;
		st_Params.uw_Flags = TEX_FP_QualityVeryHigh;

		SAV_Begin(_psz_Path, sz_Name);
		SAV_Buffer(&st_Params, sizeof(TEX_tdst_File_Params));
		SAV_ul_End();
		return TRUE;
	}
	else if(o_TypeDlg.mi_CurSelData == 1)
	{
		TEX_File_InitParams(&st_Params);
        st_Params.uc_Type = TEX_FP_AniFile;
		st_Params.uw_Flags = TEX_FP_QualityVeryHigh;

		SAV_Begin(_psz_Path, sz_Name);
		SAV_Buffer(&st_Params, sizeof(TEX_tdst_File_Params));
		SAV_ul_End();
		return TRUE;
	}
    else if(o_TypeDlg.mi_CurSelData == 2)
	{
		TEX_File_InitParams(&st_Params);
		st_Params.uw_Flags = 0;
		st_Params.uc_Type = TEX_FP_SprFile;

		SAV_Begin(_psz_Path, sz_Name);
		SAV_Buffer(&st_Params, sizeof(TEX_tdst_File_Params));
		SAV_ul_End();
		return TRUE;
	}
	else if(o_TypeDlg.mi_CurSelData == 3)
	{
		pst_Gro = GRO_pst_Struct_Create(GRO_MaterialSingle);
		GRO_Struct_SetName(pst_Gro, sz_Name);
		GRO_ul_Struct_FullSave(pst_Gro, _psz_Path, sz_Name, &TEX_gst_GlobalList);
		return TRUE;
	}
	else if(o_TypeDlg.mi_CurSelData == 4)
	{
		pst_Gro = GRO_pst_Struct_Create(GRO_MaterialMulti);
		GRO_Struct_SetName(pst_Gro, sz_Name);
		GRO_ul_Struct_FullSave(pst_Gro, _psz_Path, sz_Name, &TEX_gst_GlobalList);
		return TRUE;
	}

_Catch_
_End_
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnRenameMulti(void)
{
	EDIA_cl_NameDialog	o_Dialog("Sequence DX EX PX AX");
	HTREEITEM			h_Item;
	BIG_INDEX			ul_Dir;
	BIG_INDEX			ul_File;
	char				azext[10];
	char				az[BIG_C_MaxLenName];
	char				*pz;
	int					count, len;

	if(o_Dialog.DoModal() != IDOK) return;

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	ul_Dir = mpo_TreeCtrl->GetItemData(h_Item);
	ul_File = BIG_FirstFile(ul_Dir);
	while(ul_File != BIG_C_InvalidIndex)
	{
		L_strcpy(az, BIG_NameFile(ul_File));

		/* Extension */
		azext[0] = 0;
		pz = L_strrchr(az, '.');
		if(pz) 
		{
			L_strcpy(azext, pz);
			*pz = 0;
		}

		pz = (char *) (LPCSTR) o_Dialog.mo_Name;
		while(*pz)
		{
			switch(*pz)
			{
				case 'e':
				case 'E':
					count = L_atol(pz + 1);
					pz += 2;
					if(count >= (int) strlen(az))
						az[0] = 0;
					else
						az[strlen(az) - count] = 0;
					break;

				case 'd':
				case 'D':
					count = L_atol(pz + 1);
					pz += 2;
					if(count >= (int) strlen(az))
						az[0] = 0;
					else
						L_memmove(az, az + count, strlen(az) - count + 1);
					break;

				case 'p':
				case 'P':
					pz++;
					len = strlen(pz);
					L_memmove(az + len, az, strlen(az) + 1);
					L_memcpy(az, pz, len);
					goto end;

				case 'a':
				case 'A':
					pz++;
					L_strcat(az, pz);
					goto end;
			}
		}

end:
		L_strcat(az, azext);
		L_strcpy(BIG_NameFile(ul_File), az);
		BIG_UpdateOneFileInFat(ul_File);
		ul_File = BIG_NextFile(ul_File);
	}

	M_MF()->FatHasChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnRename(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EBRO_STR_Csz_TitleEnterNewName);
	int					i_Item;
	HTREEITEM			h_Item;
	char				asz_NewName[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenPath];
	char				*psz_NewName;
	char				asz_OldName[BIG_C_MaxLenPath];
	char				*psz_Text, *psz_Temp;
	CString				o_Text;
	BOOL				b_ForFile;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);

_Try_
	/* Init actual name in dialog box */
	b_ForFile = LIST_ACTIVATED();
	if(!b_ForFile)
	{
		ERR_X_Assert(h_Item != NULL);
		o_Dialog.mo_Name = mpo_TreeCtrl->GetItemText(h_Item);
	}
	else
	{
		/* Set name without the extension */
		ERR_X_Assert(i_Item < mpo_ListCtrl->GetItemCount());
		o_Text = mpo_ListCtrl->GetItemText(i_Item, 0);
		psz_Text = (char *) (LPCSTR) o_Text;
		L_strcpy(asz_Name, psz_Text);
		psz_Temp = L_strrchr(asz_Name, '.');
		if(psz_Temp) *psz_Temp = 0;
		o_Dialog.mo_Name = asz_Name;
	}

	if(o_Dialog.DoModal() == IDOK)
	{
		/* Check validity of name */
		if(BIG_b_CheckName((char *) (LPCSTR) o_Dialog.mo_Name) == FALSE) return;
		BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_OldName);

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Rename a directory.
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(!b_ForFile)
		{
			L_strcpy(asz_NewName, asz_OldName);
			*(L_strrchr(asz_NewName, '/') + 1) = 0;
			L_strcat(asz_NewName, (char *) (LPCSTR) o_Dialog.mo_Name);

			/* Test if new name is already here */
			if(L_strcmpi(asz_NewName, asz_OldName))
			{
				ERR_X_Error
				(
					BIG_ul_SearchDir(asz_NewName) == BIG_C_InvalidIndex,
					EBRO_ERR_Csz_NameAlreadyExists,
					asz_NewName
				);

				/* Rename directory */
				BIG_RenDir(asz_NewName, asz_OldName);
				M_MF()->FatHasChanged();
			}
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Rename a file.
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else
		{
            char*pz;
			o_Text = mpo_ListCtrl->GetItemText(i_Item, 0);
			psz_Text = (char *) (LPCSTR) o_Text;
			pz=psz_Temp = L_strrchr(psz_Text, '.');
	        

			/* Test if new name is already here */
			L_strcpy(asz_Name, (char *) (LPCSTR) o_Dialog.mo_Name);
			if(psz_Temp) L_strcat(asz_Name, psz_Temp);
			psz_NewName = asz_Name;

            /* put extension is lower case */
            pz= L_strrchr(psz_NewName, '.');
            if(pz)
	        {
		        while(*(++pz)) *pz = tolower(*pz);
	        }

			if(L_strcmpi(psz_NewName, psz_Text))
			{
				ERR_X_Error
				(
					BIG_ul_SearchFileExt(asz_OldName, psz_NewName) == BIG_C_InvalidIndex,
					EBRO_ERR_Csz_NameAlreadyExists,
					psz_Text
				);

				/* Rename file */
				BIG_RenFile(psz_NewName, asz_OldName, psz_Text);
				M_MF()->FatHasChanged();
			}
		}
	}

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnChangeKey(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EBRO_STR_Csz_TitleEnterNewKey);
	int					i_Item;
	BIG_INDEX			ul_Index;
	BIG_KEY				ul_Key;
	char				asz_Key[10];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
	ERR_X_Assert(i_Item != -1);
	ul_Index = mpo_ListCtrl->GetItemData(i_Item);

	sprintf(asz_Key, "%X", BIG_FileKey(ul_Index));
	o_Dialog.mo_Name = asz_Key;
	while(1)
	{
		if(o_Dialog.DoModal() == IDOK)
		{
			sscanf(o_Dialog.mo_Name, "%x", &ul_Key);
			if(BIG_ul_SearchKeyToFat(ul_Key) != -1)
			{
				ERR_X_ForceError("Key already exists", NULL);
			}
			else
			{
				BIG_ChangeKey(ul_Index, ul_Key);
				M_MF()->FatHasChanged();
				break;
			}
		}
		else
			break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ComputeSizeOfDirectory(BIG_INDEX ul_Index, ULONG *pul_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Dir, ul_Next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Dir = BIG_SubDir(ul_Index);
	while(ul_Dir != BIG_C_InvalidIndex)
	{
		ComputeSizeOfDirectory(ul_Dir, pul_Size);
		if((GetAsyncKeyState(VK_ESCAPE) < 0)) break;
		ul_Dir = BIG_NextDir(ul_Dir);
	}

	ul_Next = BIG_FirstFile(ul_Index);
	while(ul_Next != BIG_C_InvalidIndex)
	{
		*pul_Size += BIG_ul_GetLengthFile(BIG_PosFile(ul_Next));
		if((GetAsyncKeyState(VK_ESCAPE) < 0)) break;
		ul_Next = BIG_NextFile(ul_Next);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnLogSize(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i_Item, i_Item1;
	HTREEITEM	h_Item;
	char		asz_Message[512];
	char		asz_Log[1024];
	CString		o_String;
    BIG_INDEX   ul_Index;
	ULONG		ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
	i_Item1 = mpo_GrpCtrl->GetNextItem(-1, LVNI_SELECTED);

	AfxGetApp()->DoWaitCursor(1);
	ul_Size = 0;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Directory.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(!LIST_ACTIVATED() && !GRP_ACTIVATED())
	{
		ERR_X_Assert(h_Item != NULL);

		BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Message);
		{

		    /* Retrieve index of directory to check if it exists */
			ul_Index = BIG_ul_SearchDir(asz_Message);
			ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

			ComputeSizeOfDirectory(ul_Index, &ul_Size);

			if((GetAsyncKeyState(VK_ESCAPE) < 0))
				LINK_PrintStatusMsg("Operation aborted");
			else
			{
				sprintf(asz_Log, "[%s], Size = %u bytes", asz_Message, ul_Size);
				LINK_PrintStatusMsg(asz_Log);
			}
		}
	}

	AfxGetApp()->DoWaitCursor(-1);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnDelete(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i_Item, i_Item1;
	HTREEITEM	h_Item;
	char		asz_Message[512];
	char		asz_Temp[BIG_C_MaxLenPath];
	char		*psz_Name;
	CString		o_String;
	std::vector<ULONG >	v_p4DelFile;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
	i_Item1 = mpo_GrpCtrl->GetNextItem(-1, LVNI_SELECTED);

	AfxGetApp()->DoWaitCursor(1);

_Try_

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Request to delete a directory.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(!LIST_ACTIVATED() && !GRP_ACTIVATED())
	{
		ERR_X_Assert(h_Item != NULL);

		L_strcpy(asz_Message, EBRO_STR_Csz_ConfirmDeleteFolder);
		L_strcat(asz_Message, (char *) (LPCSTR) mpo_TreeCtrl->GetItemText(h_Item));
		L_strcat(asz_Message, "\" ?");
		if(M_MF()->MessageBox(asz_Message, EDI_STR_Csz_TitleConfirm, MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			LINK_BeginLongOperation(EBRO_STR_Csz_PopupDelDir);

			BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Message);

			std::vector< ULONG > lstKeys;
			ListOpenedFilesInDir( mpo_TreeCtrl->GetItemData(h_Item), lstKeys );

			std::vector< ULONG >::const_iterator i = lstKeys.begin( );
			std::vector< ULONG >::const_iterator e = lstKeys.end( );
			
			for( ; i != e; ++i )
			{
				const ULONG ulFile = *i;

				char szFilename[ 1024 ];
				BIG_ComputeFullName(BIG_ParentFile(ulFile), szFilename);
				strcat( szFilename, "/" );
				strcat( szFilename, BIG_NameFile(ulFile) );

				char szMessage[ 512 ];
				sprintf( szMessage, "File \"%s\" is already opened for \"%s\".\nYou must revert it before being able to delete the directory.", szFilename, BIG_P4Action( *i ) );
				M_MF( )->MessageBox( szMessage, "Notice", MB_OK );
			}

			// Cannot delete DIR if some files are opened on Perforce
			if ( lstKeys.size() == 0 )	
			{
				/* Notify P4 */
				LINK_SendMessageToEditors(EPER_MESSAGE_FLUSH_DIR, (ULONG )mpo_TreeCtrl->GetItemData(h_Item), 0);

				BIG_DelDir(asz_Message);
			}

			LINK_EndLongOperation();
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Request to delete a shortcut in a group.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else if(GRP_ACTIVATED())
	{
		ERR_X_Assert(i_Item < mpo_ListCtrl->GetItemCount());
		ERR_X_Assert(i_Item1 < mpo_GrpCtrl->GetItemCount());

		LINK_BeginLongOperation(EBRO_STR_Csz_PopupDelShortCut);

		i_Item1 = -1;
		BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Message);
		while((i_Item1 = mpo_GrpCtrl->GetNextItem(i_Item1, LVNI_SELECTED)) != -1)
		{
			o_String = mpo_GrpCtrl->GetItemText(i_Item1, 0);
			psz_Name = (char *) (LPCSTR) o_String;

			/* Display delete file in status */
			L_strcpy(asz_Temp, asz_Message);
			L_strcat(asz_Temp, "/");
			L_strcat(asz_Temp, psz_Name);
			LINK_PrintStatusMsg(asz_Temp);
			if(LINK_gb_EscapeDetected) break;

			/* Delete */
			BIG_DelRefFromGroup(mpo_ListCtrl->GetItemData(i_Item), mpo_GrpCtrl->GetItemData(i_Item1));
		}

		LINK_EndLongOperation();
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Request to delete a file.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else
	{
		ERR_X_Assert(i_Item < mpo_ListCtrl->GetItemCount());

		if(mpo_ListCtrl->GetSelectedCount() == 1)
		{
			L_strcpy(asz_Message, EBRO_STR_Csz_ConfirmDeleteFile);
			L_strcat(asz_Message, (char *) (LPCSTR) mpo_ListCtrl->GetItemText(i_Item, 0));
			L_strcat(asz_Message, "\" ?");
		}
		else
		{
			L_strcpy(asz_Message, EBRO_STR_Csz_ConfirmDeleteAllFiles);
		}

		if(M_MF()->MessageBox(asz_Message, EDI_STR_Csz_TitleConfirm, MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			LINK_BeginLongOperation(EBRO_STR_Csz_PopupDelFile);

			i_Item = -1;
			BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Message);
			while((i_Item = mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED)) != -1)
			{
				o_String = mpo_ListCtrl->GetItemText(i_Item, 0);
				psz_Name = (char *) (LPCSTR) o_String;
				L_strcpy(asz_Temp, asz_Message);
				L_strcat(asz_Temp, "/");
				L_strcat(asz_Temp, psz_Name);

				/* warn if file was opened in Perforce */
				std::string strAction = BIG_P4Action( mpo_ListCtrl->GetItemData(i_Item) );
				const bool bOpened = strAction[0] != '\0';

				if( bOpened )
				{
					char szMessage[ 512 ];
					sprintf( szMessage, "File \"%s\" is already opened for \"%s\".\nYou must revert it before being able to delete the file.", asz_Temp, strAction.c_str( ) );
					M_MF( )->MessageBox( szMessage, "Notice", MB_OK );
				}
				else
				{
					/* Display delete file in status */
					LINK_PrintStatusMsg(asz_Temp);
					if(LINK_gb_EscapeDetected) break;

					/* Add file to P4 list */
					v_p4DelFile.push_back(BIG_FileKey(mpo_ListCtrl->GetItemData(i_Item)));

					/* Notify P4 */
					LINK_SendMessageToEditors(EPER_MESSAGE_FLUSH_SELECTED, (ULONG )&v_p4DelFile, 0);

					/* Delete */
					BIG_DelFile(asz_Message, psz_Name);
				}
			}

			LINK_EndLongOperation();
		}
	}

_Catch_
_End_
	M_MF()->FatHasChanged();
	AfxGetApp()->DoWaitCursor(-1);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

static BIG_tdst_GroupElem	*gpst_OrderBuf;
static ULONG				gul_OrderSize;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnCleanGroup(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	BIG_INDEX	ul_Group;
	ULONG		i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Destroy holes */
	ul_Group = mpo_ListCtrl->GetItemData(mpo_ListCtrl->GetNextItem(-1, LVIS_SELECTED));
	BIG_ComputeFullName(BIG_ParentFile(ul_Group), asz_Path);
	gpst_OrderBuf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(ul_Group), &gul_OrderSize);
	SAV_Begin(asz_Path, BIG_NameFile(ul_Group));
	for(i = 0; i < (gul_OrderSize / sizeof(BIG_tdst_GroupElem)); i++)
	{
		if(gpst_OrderBuf[i].ul_Key && gpst_OrderBuf[i].ul_Key != BIG_C_InvalidIndex)
		{
			SAV_Buffer(&gpst_OrderBuf[i].ul_Key, 4);
			SAV_Buffer(&gpst_OrderBuf[i].ul_Type, 4);
		}
	}

	SAV_ul_End();

	RefreshAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BRO_OrderGroupFile(ULONG _ul_Group)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_OrderDialog *po_Dlg;
	ULONG				i;
	BIG_INDEX			ul_Index;
	char				asz_Path[BIG_C_MaxLenPath];
	POSITION			pos, pos1;
	BIG_tdst_GroupElem	st_Dum;
	CString				o_Str;
	char				*psz_Temp;
	char				*psz_Name, *pz_Temp1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Dlg = new EDIA_cl_OrderDialog(BIG_NameFile(_ul_Group), TRUE);

	/* Add all references */
	gpst_OrderBuf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Group), &gul_OrderSize);
	for(i = 0; i < (gul_OrderSize / sizeof(BIG_tdst_GroupElem)); i++)
	{
		if((gpst_OrderBuf[i].ul_Key == 0) || (gpst_OrderBuf[i].ul_Key == BIG_C_InvalidKey))
		{
			po_Dlg->AddItem("", 0);
		}
		else
		{
			ul_Index = BIG_ul_SearchKeyToFat(gpst_OrderBuf[i].ul_Key);
			if(ul_Index != BIG_C_InvalidIndex)
				po_Dlg->AddItem(BIG_NameFile(ul_Index), (ULONG) gpst_OrderBuf[i].ul_Key);
		}
	}

	/* Set callback */
	po_Dlg->DoModal();

	/* Save group */
	BIG_ComputeFullName(BIG_ParentFile(_ul_Group), asz_Path);
	SAV_Begin(asz_Path, BIG_NameFile(_ul_Group));

	pos = po_Dlg->mo_StrList.GetHeadPosition();
	pos1 = po_Dlg->mo_DataList.GetHeadPosition();
	while(pos)
	{
		o_Str = po_Dlg->mo_StrList.GetNext(pos);
		st_Dum.ul_Key = po_Dlg->mo_DataList.GetNext(pos1);
		if(o_Str.IsEmpty())
		{
			st_Dum.ul_Type = st_Dum.ul_Key = 0;
		}
		else
		{
			psz_Temp = L_strrchr((char *) (LPCSTR) o_Str, '.');
			if(psz_Temp)
				st_Dum.ul_Type = *((LONG *) psz_Temp);
			else
				st_Dum.ul_Type = 0;
		}

		SAV_Buffer(&st_Dum, sizeof(BIG_tdst_GroupElem));
	}

	SAV_ul_End();

	psz_Name = BIG_NameFile(_ul_Group);
	pz_Temp1 = L_strrchr(psz_Name, '.');
	if(pz_Temp1)
	{
		if((!L_strnicmp(pz_Temp1, EDI_Csz_ExtSoundBank, 4)) || (!L_strnicmp(pz_Temp1, EDI_Csz_ExtSoundMetaBank, 4)))
			LINK_SendMessageToEditors(ESON_MESSAGE_REFRESH_BANK, _ul_Group, 1);
	}

	delete po_Dlg;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnOrderGroup(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Group;
	/*~~~~~~~~~~~~~~~~~*/

	ul_Group = mpo_ListCtrl->GetItemData(mpo_ListCtrl->GetNextItem(-1, LVIS_SELECTED));
	BRO_OrderGroupFile(ul_Group);
	RefreshAll();
}

#endif /* ACTIVE_EDITORS */

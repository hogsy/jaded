/*$T EDImainframe_vss.cpp GC! 1.100 08/27/01 10:56:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/VERsion/VERsion_Number.h"
#include "EDIpaths.h"
#include "EDIerrid.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAlink_dlg.h"
#include "DIAlogs/DIAmsglink_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDIstrings.h"
#include "LINKs/LINKtoed.h"
#include "EDImsg.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

BOOL								VSS_gb_OpenOneTime = FALSE;
char								EDI_gaz_Message[4096];
BOOL								hassomething = FALSE;
BOOL								hassomethingmsg = FALSE;
static DWORD						timemsg = 0;
EDIA_cl_LinkControlDialog			go_VSSDialog;
BOOL								EDI_gb_RecurseVss = TRUE;
BOOL								EDI_gb_CheckInOut = TRUE;
BOOL								EDI_gb_CheckInDel = FALSE;
BOOL								EDI_gb_GetLatestDel = FALSE;
BOOL								EDI_gb_IsMirror = FALSE;
BOOL								EDI_can_lock = TRUE;
BOOL								EDI_gb_Local = FALSE;
#ifdef JADEFUSION
extern BOOL						EDI_gi_SoundTruncate;
#else
extern "C" BOOL						EDI_gi_SoundTruncate;
#endif
int                                 EDI_gi_GetLatest_SkipSoundFile=0;;
EDIA_cl_MsgLinkDialog				EDI_go_MsgGetLatest("File has been changed. Are you sure you wan't to overwrite it ?", "Warning");
EDIA_cl_MsgLinkDialog				EDI_go_MsgUndoCheck("File has been changed. Are you sure you wan't to undocheckout it ?", "Warning");
EDIA_cl_MsgLinkDialog				EDI_go_MsgCheckInNo("File is not on the base and is not checkout. Do you want to add it on base ?", "Warning");
//EDIA_cl_MsgLinkDialog				EDI_go_MsgTruncateFiles("Truncate the files to 8 oct ?", "Question");
char								EDI_gasz_VssHistoryName[L_MAX_PATH];
extern CList<BIG_INDEX, BIG_INDEX>	EDI_go_FHAddedFiles;
#ifdef JADEFUSION
extern BOOL						BIG_gb_WarningTime;
extern ULONG						BIG_ul_MaxSizeFile;
extern BIG_KEY					BIG_gul_CanAskKey;
extern BOOL						EDI_gb_NoVerbose;
extern BOOL							EDI_gb_SlashC;
#else
extern "C" BOOL						BIG_gb_WarningTime;
extern "C" BOOL						BIG_ul_MaxSizeFile;
extern "C" BIG_KEY					BIG_gul_CanAskKey;
extern "C" BOOL						EDI_gb_NoVerbose;
extern "C" BOOL							EDI_gb_SlashC;
#endif
BOOL								EDI_gb_OpVSS = FALSE;
VSSCB								gpfn_VSSR = NULL;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int s_SkipSoundFile(char *_psz_PathName, char *_psz_FileName)
{
	/*~~~~*/
    ULONG ul_fat;
    ULONG ul_size;
	/*~~~~*/

    if(EDI_gb_IsMirror)
    {
        EDI_gi_GetLatest_SkipSoundFile = 0;
        return 0;
    }
    

	ul_fat = BIG_ul_SearchFileExt(_psz_PathName, _psz_FileName);

    if
	(
		(BIG_b_IsFileExtension(ul_fat, EDI_Csz_ExtSoundMusic))
	||	(BIG_b_IsFileExtension(ul_fat, EDI_Csz_ExtSoundAmbience))
	||	(BIG_b_IsFileExtension(ul_fat, EDI_Csz_ExtSoundDialog))
	||	(BIG_b_IsFileExtension(ul_fat, EDI_Csz_ExtSoundFile))
	||	(BIG_b_IsFileExtension(ul_fat, EDI_Csz_ExtLoadingSound))
	)
	{
        if(!EDI_gi_GetLatest_SkipSoundFile)
        {
            // Don't ask question, answer yes automatically.
            EDI_gi_GetLatest_SkipSoundFile = 1;
            /*switch(M_MF()->MessageBox("Would you skip \"complete\" (size>8) sound files ?", "Request", MB_YESNO))
            {
            case IDYES:                
                EDI_gi_GetLatest_SkipSoundFile = 1;
                break;
            case IDNO:
                EDI_gi_GetLatest_SkipSoundFile = -1;
                break;
            default:
                EDI_gi_GetLatest_SkipSoundFile = 0;
                return 0;
            }*/
        }
        
        
        if(EDI_gi_GetLatest_SkipSoundFile < 0)
        {
            return 0;
        }
        else if(EDI_gi_GetLatest_SkipSoundFile > 0)
        {
            SWAP_BIG();
            ul_fat = BIG_ul_SearchFileExt(_psz_PathName, _psz_FileName);
            ul_size = BIG_ul_EditorGetSizeOfFile(BIG_FileKey(ul_fat));
            SWAP_BIG();

            if( ul_size > 8) 
            {                
                return 1;
            }
        }
    }
    
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SoundFile(BIG_INDEX _ul_File)
{
	/*~~~~*/
	int res;
	/*~~~~*/

	if
	(
		(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtSoundMusic))
	||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtSoundAmbience))
	||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtSoundDialog))
	||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtSoundFile))
	||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtLoadingSound))
	||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtVideo1))
	||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtVideo2))
	||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtVideo4))
	||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtVideo5))
	||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtVideo3))
	)
	{
        // Don't ask question, always answer yes

        //EDI_go_MsgTruncateFiles.mo_Name = CString(BIG_NameFile(_ul_File));
		//::SetFocus(0);

		//if(((res = EDI_go_MsgTruncateFiles.DoModal()) == IDYES)) 
            BIG_ul_MaxSizeFile = 8;
        //else
        //    BIG_ul_MaxSizeFile = 0;
	}
	else
	{
		BIG_ul_MaxSizeFile = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::ComputeLockedFileOpen(char *_psz_Ref, char *_psz_NameR)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp[L_MAX_PATH];
	char	*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_psz_NameR = 0;

	L_strcpy(asz_Temp, _psz_Ref);
	psz_Temp = L_strrchr(asz_Temp, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Temp, "_");
	L_strcpy(_psz_NameR, asz_Temp);
	L_strcat(_psz_NameR, EDI_Csz_LockedFileWrite);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::ComputeLockedFileName(char *_psz_Ref, char *_psz_NameW)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*psz_Temp;
	char	asz_Temp[L_MAX_PATH];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_psz_NameW = 0;

	L_strcpy(asz_Temp, _psz_Ref);
	psz_Temp = L_strrchr(asz_Temp, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Temp, "_");

	L_strcpy(_psz_NameW, asz_Temp);
	L_strcat(_psz_NameW, EDI_Csz_LockedFileWrite);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int s_GetLatestVersion(char *_psz_PathName, char *_psz_FileName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Sub;
	BIG_KEY		ul_Key;
	char		asz_CurrentOwner[BIG_C_MaxLenPath];
	L_time_t	ul_Time;
	int			i_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
    /* Update owner. Can't get latest version if current owner has checked out the file */
	ul_Sub = BIG_ul_SearchFileExt(_psz_PathName, _psz_FileName);
	ul_Key = BIG_FileKey(ul_Sub);
	ul_Time = BIG_TimeFile(ul_Sub);

    if(s_SkipSoundFile(_psz_PathName, _psz_FileName)) return 1;

	SWAP_BIG();
	*asz_CurrentOwner = 0;
	ul_Sub = BIG_ul_SearchKeyToFat(ul_Key);
	if(ul_Sub != BIG_C_InvalidIndex)
	{
		L_strcpy(asz_CurrentOwner, BIG_P4OwnerName(ul_Sub));
	}

	SWAP_BIG();

	/* Update date to be sure that file will be get */
	if(L_strcmpi(asz_CurrentOwner, M_MF()->mst_Ini.asz_CurrentUserName))
	{
		SWAP_BIG();
		if((ul_Sub != BIG_C_InvalidIndex) && (BIG_TimeFile(ul_Sub) > ul_Time))
		{
			i_Res = IDYES;
			EDI_go_MsgGetLatest.mo_Name = CString(_psz_PathName) + "/" + CString(_psz_FileName);
			::SetFocus(0);
			if(((i_Res = EDI_go_MsgGetLatest.DoModal()) == IDYES)) 
				BIG_TimeFile(ul_Sub) = 0;

			// Check if somebody has modified the file
			else if(i_Res == IDNO)
			{
				char *pz;
				if(!L_strnicmp(asz_CurrentOwner, "_nobody_", 8))
				{
					pz = asz_CurrentOwner + 8;
					if(*pz)
					{
						if(L_stricmp(M_MF()->mst_Ini.asz_CurrentUserName, pz))
						{
							char az[512];
							sprintf(az, "%s: You have lose the work of %s", _psz_FileName, pz);
							ERR_X_Warning(0, az, NULL);
						}
					}
				}
			}
			else if(i_Res == IDCANCEL)
			{
				LINK_gb_EscapeDetected = TRUE;
				SWAP_BIG();
				return 0;
			}
		}

		SWAP_BIG();
		BIG_ul_ExportMergeFileRec(_psz_PathName, _psz_FileName);
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDI_cl_MainFrame::GetLatestVersionFile(char *_psz_PathName, char *_psz_FileName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat, ul_Fat1;
	BIG_KEY		ul_Key;
	char		asz_CurrentOwner[BIG_C_MaxLenPath];
	char		asz_Tmp[512];
	char		asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile = 0;
    
	ul_Fat = BIG_ul_SearchFileExt(_psz_PathName, _psz_FileName);
	if(ul_Fat == BIG_C_InvalidIndex) return 1;
	ul_Key = BIG_FileKey(ul_Fat);
	if(ul_Key == BIG_C_InvalidKey) return 1;

    
	/* Lock bigfile */
	if(!b_LockBigFile()) return 0;

	/* Update owner */
_Try_
	VssFileChanged();
	SWAP_BIG();
	b_UndoCheckOut(ul_Key, asz_CurrentOwner, FALSE, TRUE);
	SWAP_BIG();
	CloseVssFile();
	L_strcpy(BIG_P4OwnerName(ul_Fat), asz_CurrentOwner);

	COPY_BIG();

	/* Open reference file */
	REFFileChanged();
	SWAP_BIG();
	if((ul_Fat1 = BIG_ul_SearchKeyToFat(ul_Key)) != -1)
	{
		BIG_ComputeFullName(BIG_ParentFile(ul_Fat1), asz_Path);
		if(s_GetLatestVersion(asz_Path, BIG_NameFile(ul_Fat1)) == 0)
		{
			CloseRefFile();
			RESTORE_BIG();
			UnLockBigFile();
			_Return_(1);
		}
	}

	CloseRefFile();
	RESTORE_BIG();

	/* Unlock bigfile */
	UnLockBigFile();

	/* Really update */
	if(BIG_FileChanged(ul_Fat) & EDI_FHC_AddUpdate)
	{
		sprintf(asz_Tmp, "Get Latest Version : %s/%s", _psz_PathName, _psz_FileName);
		LINK_PrintStatusMsg(asz_Tmp);
	}
_Catch_
	UnLockBigFile();
	if(BIG_gb_NotCurrent && BIG_gst.h_CLibFileHandle) BIG_Close();
	RESTORE_BIG();
	return 0;
_End_
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::b_UndoCheckOut(BIG_KEY _ul_Key, char *_psz_Owner, BOOL _b_Del, BOOL _b_Force)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index, ul_Fat;
	char		asz_Name[BIG_C_MaxLenPath];
	L_time_t	ul_Time;
	int			i_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get source time */
	SWAP_BIG();
	ul_Time = 0;
	ul_Index = BIG_ul_SearchKeyToFat(_ul_Key);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		L_strcpy(asz_Name, BIG_NameFile(ul_Index));
		ul_Time = BIG_TimeFile(ul_Index);
	}

	SWAP_BIG();

	*_psz_Owner = 0;
	ul_Fat = BIG_ul_SearchKeyToFat(_ul_Key);
	if(ul_Fat != BIG_C_InvalidIndex)
	{
		/* Retreive owner of file */
		ul_Index = ul_Fat;
		L_strcpy(_psz_Owner, BIG_NameFile(ul_Index));

		/* Check if file is checked out by current user */
		if(!L_strcmpi(_psz_Owner, M_MF()->mst_Ini.asz_CurrentUserName))
		{
			/* Delete checkout mark in VSS file */
			if(_b_Del)
			{
				i_Res = IDYES;
				EDI_go_MsgUndoCheck.mo_Name = CString(asz_Name);
				::SetFocus(0);
				if
				(
					_b_Force
				||	(ul_Time <= BIG_TimeFile(ul_Index))
				||	((i_Res = EDI_go_MsgUndoCheck.DoModal()) == IDYES)
				)
				{
					/* Delete file */
					L_strcpy(BIG_NameFile(ul_Fat), "_nobody_");
					L_strcat(BIG_NameFile(ul_Fat), M_MF()->mst_Ini.asz_CurrentUserName);
					BIG_UpdateOneFileInFat(ul_Fat);
//					BIG_DeleteFile(0, ul_Fat);
					*_psz_Owner = 0;
				}

				if(i_Res == IDCANCEL)
				{
					LINK_gb_EscapeDetected = TRUE;
				}
			}

			if(!*_psz_Owner) return TRUE;
			if(!_b_Del) return TRUE;
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::UndoCheckOutFile(char *_psz_PathName, char *_psz_FileName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat;
	BIG_KEY		ul_Key;
	char		asz_CurrentOwner[BIG_C_MaxLenPath];
	char		asz_Tmp[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_gb_OpVSS = TRUE;

	/* Get fat pos of file in current project */
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
	ul_Fat = BIG_ul_SearchFileExt(_psz_PathName, _psz_FileName);
	if(ul_Fat == BIG_C_InvalidIndex) return;
	ul_Key = BIG_FileKey(ul_Fat);
	if(ul_Key == BIG_C_InvalidKey) return;

	/* Lock bigfile */
	if(!b_LockBigFile()) return;

_Try_
	VssFileChanged();
	SWAP_BIG();
	if(b_UndoCheckOut(ul_Key, asz_CurrentOwner, TRUE, FALSE))
	{
		sprintf(asz_Tmp, "Undo Check Out : %s/%s", _psz_PathName, _psz_FileName);
		LINK_PrintStatusMsg(asz_Tmp);
	}

	SWAP_BIG();
	CloseVssFile();

	/* Update owner in fat */
	L_strcpy(BIG_P4OwnerName(ul_Fat), asz_CurrentOwner);

	/* Unlock bigfile */
	UnLockBigFile();
_Catch_
	UnLockBigFile();
	if(BIG_gb_NotCurrent && BIG_gst.h_CLibFileHandle) BIG_Close();
	RESTORE_BIG();
_End_
}

/*
 =======================================================================================================================
    Aim:    To save a checked out mark in the VSS file.

    Note:   Current big file must be VSS file.
 =======================================================================================================================
 */
static void s_CheckOutSave(char *_psz_PathName, char *_psz_FileName, char *_psz_Out)
{
_Try_
	L_time(&BIG_gx_GlobalTime);
	BIG_gb_CanAskKey = FALSE;
	BIG_gpsz_RealName = NULL;
	BIG_gp_Buffer = "                         ";
	BIG_gul_Length = 16;
	BIG_ul_UpdateCreateFileOptim(0, -1, _psz_PathName, M_MF()->mst_Ini.asz_CurrentUserName, NULL, TRUE);
	BIG_gb_CanAskKey = TRUE;
_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL sb_CheckOut(BIG_KEY _ul_Key, char *_psz_Owner)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	char		asz_Temp[512];
	char		asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_gb_OpVSS = TRUE;

	ul_Index = BIG_ul_SearchKeyToFat(_ul_Key);
	if(ul_Index == BIG_C_InvalidIndex)
	{
		/* Add checkout mark in VSS file */
		BIG_ul_CreateDir(BIG_Csz_Root);
		BIG_gul_CanAskKey = _ul_Key;
		s_CheckOutSave(BIG_Csz_Root, asz_Temp, _psz_Owner);
		return TRUE;
	}

	/* If file is already checked out, do nothing */
	else
	{
		/* Read owner to update in fat */
		L_strcpy(_psz_Owner, BIG_NameFile(ul_Index));
		if(!L_strnicmp(_psz_Owner, "_nobody_", 8))
		{
			/* Can't do a local checkout if somebody else has checkin the file */
			if(EDI_gb_Local)
			{
				char *pz;
				pz = _psz_Owner + 8;
				if(*pz)
				{
					if(L_strcmpi(pz, M_MF()->mst_Ini.asz_CurrentUserName))
					{
						char asz_Temp[512];
						sprintf(asz_Temp, "File has been modified by %s. Local checkout IMPOSSIBLE.", pz);
						ERR_X_Warning(0, asz_Temp, NULL);
						return FALSE;
					}
				}
			}

			L_strcpy(BIG_NameFile(ul_Index), M_MF()->mst_Ini.asz_CurrentUserName);
			L_strcpy(_psz_Owner, BIG_NameFile(ul_Index));
			BIG_UpdateOneFileInFat(ul_Index);
			return TRUE;
		}
		else if(L_strcmpi(_psz_Owner, M_MF()->mst_Ini.asz_CurrentUserName))
		{
			SWAP_BIG();
			ul_Index = BIG_ul_SearchKeyToFat(_ul_Key);
			if(ul_Index != BIG_C_InvalidIndex)
			{
				BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
				sprintf(asz_Temp, "%s/%s is already checkout by %s", asz_Path, BIG_NameFile(ul_Index), _psz_Owner);
				ERR_X_Warning(0, asz_Temp, NULL);
			}

			SWAP_BIG();
		}

		return FALSE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::CheckOutFile(char *_psz_PathName, char *_psz_FileName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat;
	BIG_KEY		ul_Key;
	char		asz_CurrentOwner[BIG_C_MaxLenPath];
	char		asz_Tmp[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_gb_OpVSS = TRUE;

	/* Get fat pos of file in current project */
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
	ul_Fat = BIG_ul_SearchFileExt(_psz_PathName, _psz_FileName);
	if(ul_Fat == BIG_C_InvalidIndex) return;
	ul_Key = BIG_FileKey(ul_Fat);
	if(ul_Key == BIG_C_InvalidKey) return;

	/* Get latest version of file */
	if(GetLatestVersionFile(_psz_PathName, _psz_FileName) == 0) return;

	/* Lock bigfile */
	if(!b_LockBigFile()) return;
_Try_
	VssFileChanged();
	SWAP_BIG();
	if(sb_CheckOut(ul_Key, asz_CurrentOwner))
	{
		sprintf(asz_Tmp, "Check Out : %s/%s", _psz_PathName, _psz_FileName);
		LINK_PrintStatusMsg(asz_Tmp);
	}

	SWAP_BIG();
	CloseVssFile();

	/* Update owner in fat */
	L_strcpy(BIG_P4OwnerName(ul_Fat), asz_CurrentOwner);

	/* Unlock bigfile */
	UnLockBigFile();
_Catch_
	UnLockBigFile();
	if(BIG_gb_NotCurrent && BIG_gst.h_CLibFileHandle) BIG_Close();
	RESTORE_BIG();
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
	return;
_End_
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::CheckInFileNoOut(char *_psz_PathName, char *_psz_FileName)
{
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
	EDI_gb_CheckInOut = FALSE;
	CheckInFile(_psz_PathName, _psz_FileName);
	EDI_gb_CheckInOut = TRUE;
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::CheckInFile(char *_psz_PathName, char *_psz_FileName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat;
	BIG_KEY		ul_Key;
	char		asz_CurrentOwner[BIG_C_MaxLenPath];
	char		asz_Temp[BIG_C_MaxLenPath + BIG_C_MaxLenName];
	char		asz_Tmp[512];
	BOOL		bDis;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_gb_OpVSS = TRUE;

	EDI_gi_SoundTruncate = 0;
	BIG_ul_MaxSizeFile = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    

	/* Get fat pos of file in current project */
	ul_Fat = BIG_ul_SearchFileExt(_psz_PathName, _psz_FileName);
	SoundFile(ul_Fat);
	if(ul_Fat == BIG_C_InvalidIndex) return;
	ul_Key = BIG_FileKey(ul_Fat);
	if(ul_Key == BIG_C_InvalidKey) return;
	bDis = TRUE;

	/* Lock bigfile */
	if(!b_LockBigFile()) return;

	VssFileChanged();
	SWAP_BIG();
	if(b_UndoCheckOut(ul_Key, asz_CurrentOwner, EDI_gb_CheckInOut, TRUE))
	{
		SWAP_BIG();
		CloseVssFile();

		SWAP_BIG();
_Try_
		REFFileChanged();
		SWAP_BIG();
_Catch_
		RESTORE_BIG();
		UnLockBigFile();
		return;
_End_
		SWAP_BIG();
		BIG_gb_WarningTime = TRUE;
		BIG_ul_ExportMergeFileRec(_psz_PathName, _psz_FileName);
		BIG_gb_WarningTime = FALSE;
		SWAP_BIG();
		CloseRefFile();
		RESTORE_BIG();
	}
	else if(EDI_gb_CheckInOut)
	{
		sprintf(asz_Temp, "Check In : %s/%s is not checkout", _psz_PathName, _psz_FileName);
		ERR_X_Warning(0, asz_Temp, NULL);
		bDis = FALSE;
		SWAP_BIG();
		CloseVssFile();
	}
	else
	{
		SWAP_BIG();
		CloseVssFile();
	}

	/* Update owner in fat */
	L_strcpy(BIG_P4OwnerName(ul_Fat), asz_CurrentOwner);

	/* Unlock bigfile */
	UnLockBigFile();

	if(!*BIG_P4OwnerName(ul_Fat) && bDis)
	{
		sprintf(asz_Tmp, "Check In : %s/%s", _psz_PathName, _psz_FileName);
		LINK_PrintStatusMsg(asz_Tmp);
	}
}

/*$4
 ***********************************************************************************************************************
    PUBLIC DIR
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void s_VssRecurse(char *_psz_Path, BIG_INDEX _ul_Dir, int _i_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Sub, ul_Res;
	BIG_KEY		ul_Key;
	char		asz_Temp[BIG_C_MaxLenPath + BIG_C_MaxLenName];
	char		asz_CurrentOwner[BIG_C_MaxLenPath];
	BOOL		b_Dis;
	char		*psz_Title;
	int			i_Res;
	char		*psz_Name;
	BOOL		b_First;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(LINK_gb_EscapeDetected) return;

	/* Parse all sub dirs */
	if(EDI_gb_RecurseVss)
	{
		ul_Sub = BIG_SubDir(_ul_Dir);
		while(ul_Sub != BIG_C_InvalidIndex)
		{
			L_strcpy(asz_Temp, _psz_Path);
			L_strcat(asz_Temp, "/");
			L_strcat(asz_Temp, BIG_NameDir(ul_Sub));
			s_VssRecurse(asz_Temp, ul_Sub, _i_Type);
			ul_Sub = BIG_NextDir(ul_Sub);
			if(LINK_gb_EscapeDetected) return;
		}
	}

	b_First = TRUE;

	/* Parse all files of current dir */
	ul_Sub = BIG_FirstFile(_ul_Dir);
	while(ul_Sub != BIG_C_InvalidIndex)
	{
		b_Dis = FALSE;
		switch(_i_Type)
		{
		/* Check in. Current bigfile must be ref one */
		case 1:
			EDI_gb_OpVSS = TRUE;
			psz_Title = "Check In : ";
			L_strcpy(asz_CurrentOwner, BIG_P4OwnerName(ul_Sub));
			if(*asz_CurrentOwner)
			{
				if(!L_strcmpi(asz_CurrentOwner, M_MF()->mst_Ini.asz_CurrentUserName))
				{
					BIG_gb_WarningTime = TRUE;
_Try_
					SoundFile(ul_Sub);
					BIG_ul_ExportMergeFileRec(_psz_Path, BIG_NameFile(ul_Sub));
_Catch_
					BIG_gb_WarningTime = FALSE;
_EndThrow_
					b_Dis = TRUE;
					BIG_gb_WarningTime = FALSE;
				}
			}
			else
			{
				/* File not checkout, and file not in base ? */
				psz_Name = BIG_NameFile(ul_Sub);
				SWAP_BIG();
				ul_Res = BIG_ul_SearchFileExt(_psz_Path, psz_Name);
				SWAP_BIG();
				if(ul_Res == BIG_C_InvalidIndex)
				{
					EDI_go_MsgCheckInNo.mo_Name = CString(_psz_Path) + "/" + CString(psz_Name);
					::SetFocus(0);
					i_Res = IDYES;
					if((EDI_go_MsgCheckInNo.mb_ApplyToAll) || ((i_Res = EDI_go_MsgCheckInNo.DoModal()) == IDYES))
					{
						SoundFile(ul_Sub);
						BIG_ul_ExportMergeFileRec(_psz_Path, psz_Name);
						b_Dis = TRUE;
					}

					if(i_Res == IDCANCEL) LINK_gb_EscapeDetected = TRUE;
				}
			}
			break;

		/* Check out. Current bigfile must be vss one */
		case 2:
			EDI_gb_OpVSS = TRUE;
			psz_Title = "Check Out : ";
			ul_Key = BIG_FileKey(ul_Sub);
			SWAP_BIG();
			if(sb_CheckOut(ul_Key, asz_CurrentOwner)) b_Dis = TRUE;
			SWAP_BIG();
			L_strcpy(BIG_P4OwnerName(ul_Sub), asz_CurrentOwner);
			break;

		/* Undo check out. Current bigfile must be vss one */
		case 3:
			EDI_gb_OpVSS = TRUE;
			psz_Title = "Undo Check Out : ";
			ul_Key = BIG_FileKey(ul_Sub);
			SWAP_BIG();
			if(M_MF()->b_UndoCheckOut(ul_Key, asz_CurrentOwner, TRUE, FALSE)) b_Dis = TRUE;
			SWAP_BIG();
			L_strcpy(BIG_P4OwnerName(ul_Sub), asz_CurrentOwner);
			break;

		/* Get latest version. Current bigfile must be reference one */
		case 4:
			psz_Title = "Get Latest Version : ";
			s_GetLatestVersion(_psz_Path, BIG_NameFile(ul_Sub));
			break;

		/* Update owner (for checkin). Current bigfile must be vss one */
		case 5:
			psz_Title = NULL;
			ul_Key = BIG_FileKey(ul_Sub);
			SWAP_BIG();
			M_MF()->b_UndoCheckOut(ul_Key, asz_CurrentOwner, FALSE, TRUE);
			SWAP_BIG();
			L_strcpy(BIG_P4OwnerName(ul_Sub), asz_CurrentOwner);
			b_Dis = FALSE;
			break;

		/* Undo check out. Force */
		case 6:
			EDI_gb_OpVSS = TRUE;
			psz_Title = NULL;
			ul_Key = BIG_FileKey(ul_Sub);
			SWAP_BIG();
			M_MF()->b_UndoCheckOut(ul_Key, asz_CurrentOwner, TRUE, TRUE);
			SWAP_BIG();
			L_strcpy(BIG_P4OwnerName(ul_Sub), asz_CurrentOwner);
			break;

		/* Show checkout */
		case 7:
		case 8:
			psz_Title = "=> ";
			ul_Key = BIG_FileKey(ul_Sub);
			SWAP_BIG();
			if(M_MF()->b_UndoCheckOut(ul_Key, asz_CurrentOwner, FALSE, TRUE))
			{
				if(!L_strcmpi(asz_CurrentOwner, M_MF()->mst_Ini.asz_CurrentUserName)) 
				{
					SWAP_BIG();
					L_strcpy(BIG_P4OwnerName(ul_Sub), asz_CurrentOwner);
					sprintf(asz_Temp, "%s%s/%s", psz_Title, _psz_Path, BIG_NameFile(ul_Sub));
					if(_i_Type == 8) 
					{
						if(gpfn_VSSR) gpfn_VSSR(ul_Sub);
					}
					else
						LINK_PrintStatusMsg(asz_Temp);
					psz_Title = NULL;
					break;
				}
			}

			SWAP_BIG();
			L_strcpy(BIG_P4OwnerName(ul_Sub), asz_CurrentOwner);
			ul_Sub = BIG_NextFile(ul_Sub);
			continue;
		}

		if(b_First && psz_Title)
		{
			b_First = FALSE;
			sprintf(asz_Temp, "%s%s", psz_Title, _psz_Path);
			LINK_PrintStatusMsg(asz_Temp);
			if(LINK_gb_EscapeDetected) break;
		}
		else
		{
			if(LINK_gb_EscapeDetected) break;
		}

		ul_Sub = BIG_NextFile(ul_Sub);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::GetLatestVersionDir(char *_psz_PathName)
{
	/*~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat;
	int			i_Res;
	BOOL		bcan;
	/*~~~~~~~~~~~~~~~*/

	/* Lock bigfile */
	if(!b_LockBigFile()) return;

	ul_Fat = BIG_ul_SearchDir(_psz_PathName);
	if(ul_Fat == BIG_C_InvalidIndex)
	{
		UnLockBigFile();
		return;
	}

	/* Delete destination directory ? */
	EDI_gb_IsMirror = FALSE;
	if(EDI_gb_GetLatestDel)
	{
		M_MF()->SetFocus();
		if
		(
			(
				i_Res = MessageBox
					(
						"Do you want to make a mirror ?\nThis can erase some files on your bigfile. IT IS SAFER TO CLOSE ALL YOUR WORLDS BEFORE !!!",
						_psz_PathName,
						MB_ICONERROR | MB_YESNOCANCEL
					)
			) == IDYES
		)
		{
			if(BIG_ul_SearchDir(_psz_PathName) != BIG_C_InvalidIndex)
			{
				/* Be sure that path is present on base */
				bcan = TRUE;
				COPY_BIG();
_Try_
				REFFileChanged();
				SWAP_BIG();
_Catch_
				RESTORE_BIG();
				UnLockBigFile();
				return;
_End_
				if(BIG_ul_SearchDir(_psz_PathName) == BIG_C_InvalidIndex) bcan = FALSE;
				CloseRefFile();
				RESTORE_BIG();

				if(bcan)
				{
					BIG_DelDir(_psz_PathName);
					FatHasChanged();
				}

				EDI_gb_IsMirror = TRUE;
			}
		}
		else if(i_Res == IDCANCEL)
		{
			UnLockBigFile();
			LINK_gb_EscapeDetected = TRUE;
			return;
		}
	}

	AfxGetApp()->DoWaitCursor(1);

	/* First update owner of each files */
	VssFileChanged();
	s_VssRecurse(_psz_PathName, ul_Fat, 5);
	CloseVssFile();

	/* And do the getlatest version */
	COPY_BIG();
_Try_
	REFFileChanged();
	SWAP_BIG();
	ul_Fat = BIG_ul_SearchDir(_psz_PathName);
	if(ul_Fat != BIG_C_InvalidIndex) s_VssRecurse(_psz_PathName, ul_Fat, 4);
	CloseRefFile();
	RESTORE_BIG();

_Catch_
	CloseRefFile();
	RESTORE_BIG();
_End_
	/* Unlock bigfile */
	UnLockBigFile();
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::UndoCheckOutDir(char *_psz_PathName)
{
	/*~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat;
	/*~~~~~~~~~~~~~~~*/

	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
	ul_Fat = BIG_ul_SearchDir(_psz_PathName);
	if(ul_Fat == BIG_C_InvalidIndex) return;

	/* Lock bigfile */
	if(!b_LockBigFile()) return;

	AfxGetApp()->DoWaitCursor(1);
	VssFileChanged();
	s_VssRecurse(_psz_PathName, ul_Fat, 3);
	CloseVssFile();

	/* Unlock bigfile */
	UnLockBigFile();
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::LocalCheckOutDir(char *_psz_PathName, BOOL _b_Msg)
{
	/*~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat;
	int			i_Res;
	/*~~~~~~~~~~~~~~~*/

	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
	ul_Fat = BIG_ul_SearchDir(_psz_PathName);
	if(ul_Fat == BIG_C_InvalidIndex) return;

	if(_b_Msg)
	{
		i_Res = M_MF()->MessageBox
			(
				"You want to checkout files without getting the last version.\nIf someone has modified those files, everything will be LOOSE !!!\n(et il sera vraiment pas content)\nDo you want to continue ?",
				"Are you SURE ?",
				MB_ICONWARNING | MB_YESNO
			);
		if(i_Res == IDNO) return;
	}

	/* Lock bigfile */
	if(!b_LockBigFile()) return;

	AfxGetApp()->DoWaitCursor(1);
	VssFileChanged();
	s_VssRecurse(_psz_PathName, ul_Fat, 2);
	CloseVssFile();

	/* Unlock bigfile */
	UnLockBigFile();
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::CheckOutDir(char *_psz_PathName)
{
	/*~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat;
	/*~~~~~~~~~~~~~~~*/

	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
	ul_Fat = BIG_ul_SearchDir(_psz_PathName);
	if(ul_Fat == BIG_C_InvalidIndex) return;

	LINK_gb_EscapeDetected = FALSE;

	/* Get latest version */
	AfxGetApp()->DoWaitCursor(1);
	GetLatestVersionDir(_psz_PathName);
	if(LINK_gb_EscapeDetected) return;

	/* Local checkout */
	LocalCheckOutDir(_psz_PathName, FALSE);
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::CheckInDirNoOut(char *_psz_PathName)
{
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;
    
	EDI_gb_CheckInOut = FALSE;
	AfxGetApp()->DoWaitCursor(1);
	CheckInDir(_psz_PathName);
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gb_CheckInOut = TRUE;
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::b_CheckOwnerDir(BIG_INDEX _ul_Dir)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	h_Index;
	char		asz_CurrentOwner[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Index = BIG_FirstFile(_ul_Dir);
	while(h_Index != BIG_C_InvalidIndex)
	{
		L_strcpy(asz_CurrentOwner, BIG_P4OwnerName(h_Index));
		if(!*asz_CurrentOwner) return FALSE;
		if(L_strcmpi(asz_CurrentOwner, M_MF()->mst_Ini.asz_CurrentUserName)) return FALSE;
		h_Index = BIG_NextFile(h_Index);
	}

	h_Index = BIG_SubDir(_ul_Dir);
	while(h_Index != BIG_C_InvalidIndex)
	{
		if(!b_CheckOwnerDir(h_Index)) return FALSE;
		h_Index = BIG_NextDir(h_Index);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::CheckInDir(char *_psz_PathName)
{
	/*~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat;
	int			i_Res;
	BOOL		b_Can;
	/*~~~~~~~~~~~~~~~*/

	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;    
	BIG_ul_MaxSizeFile = 0;

	b_Can = TRUE;
	ul_Fat = BIG_ul_SearchDir(_psz_PathName);
	if(ul_Fat == BIG_C_InvalidIndex) return;

	/* Lock bigfile */
	if(!b_LockBigFile()) return;

	AfxGetApp()->DoWaitCursor(1);

	VssFileChanged();
	s_VssRecurse(_psz_PathName, ul_Fat, 5);
	CloseVssFile();

	/* And then do a checkin depending of that owner */
	COPY_BIG();
_Try_
	REFFileChanged();
	SWAP_BIG();

	/* Delete destination directory ? */
	if(EDI_gb_CheckInDel)
	{
		M_MF()->SetFocus();
		if
		(
			(
				i_Res = MessageBox
					(
						"Do you want to make a mirror ?\nThis can erase some files on the base !!!",
						_psz_PathName,
						MB_ICONERROR | MB_YESNOCANCEL
					)
			) == IDYES
		)
		{
			if(BIG_ul_SearchDir(_psz_PathName) != BIG_C_InvalidIndex)
			{
				/* Check that all files have the correct owner */
				SWAP_BIG();
				if(!b_CheckOwnerDir(ul_Fat))
				{
					ERR_X_ForceError("One or more files are not check out by you. Mirror is IMPOSSIBLE.", NULL);
					b_Can = FALSE;
				}

				SWAP_BIG();
				if(b_Can) BIG_DelDir(_psz_PathName);
			}
		}
		else if(i_Res == IDCANCEL)
		{
			b_Can = FALSE;
		}
	}

	SWAP_BIG();
	if(b_Can) s_VssRecurse(_psz_PathName, ul_Fat, 1);
	SWAP_BIG();
	CloseRefFile();
	RESTORE_BIG();

_Catch_
	CloseRefFile();
	RESTORE_BIG();
_End_
	if(EDI_gb_CheckInOut)
	{
		if(b_Can)
		{
			VssFileChanged();
			s_VssRecurse(_psz_PathName, ul_Fat, 6);
			CloseVssFile();
		}
	}

	/* Unlock bigfile */
	UnLockBigFile();
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gi_SoundTruncate = 0;
    EDI_gi_GetLatest_SkipSoundFile=0;    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::ShowCheckOut(void)
{
	/*~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat;
	/*~~~~~~~~~~~~~~~*/

	ul_Fat = BIG_ul_SearchDir(BIG_Csz_Root);
	if(ul_Fat == BIG_C_InvalidIndex) return;

	/* Lock bigfile */
	if(!b_LockBigFile()) return;

	LINK_PrintStatusMsg("Check out files are :");
	VssFileChanged(FALSE, TRUE);
	s_VssRecurse(BIG_Csz_Root, ul_Fat, 7);
	CloseVssFile();

	/* Unlock bigfile */
	UnLockBigFile();
	LINK_PrintStatusMsg("End");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::UpdateOwnerPath(char *_psz_PathName, char *_psz_PathName1, char *_psz_FileName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_CurrentOwner[BIG_C_MaxLenOwner];
	BIG_INDEX	ul_Fat;
	BIG_KEY		ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	VssFileChanged();
	ul_Fat = BIG_ul_SearchFileExt(_psz_PathName1, _psz_FileName);
	if(ul_Fat != BIG_C_InvalidIndex)
	{
		ul_Key = BIG_FileKey(ul_Fat);
		if(ul_Key != BIG_C_InvalidKey)
		{
			SWAP_BIG();
			b_UndoCheckOut(ul_Key, asz_CurrentOwner, FALSE, TRUE);
			SWAP_BIG();
		}
	}

	ul_Fat = BIG_ul_SearchDir(_psz_PathName);
	if(ul_Fat != BIG_C_InvalidIndex)
	{
		s_VssRecurse(_psz_PathName, ul_Fat, 5);
	}

	CloseVssFile();
	L_strcpy(BIG_P4OwnerName(ul_Fat), asz_CurrentOwner);
}

/*$4
 ***********************************************************************************************************************
    LOCK PART
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *CleanAZ(char *pz)
{
	int	i;

	pz[100] = 0;
	L_strupr(pz);
	for(i = 0;i < 100; i++)
	{
		if(L_isalnum(*pz) || *pz == ' ' || *pz == '(' || *pz == ')' || pz[i] != '-') break;
		if(!*pz) break;
		pz++;
	}

	for(i = 0; i < 100; i++)
	{
		if(!L_isalnum(pz[i]) && pz[i] != ' ' && pz[i] != '(' && pz[i] != ')' && pz[i] != '-' )
		{
			pz[i] = 0;
			break;
		}
	}

	return pz;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SendMsg(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static EDIA_cl_NameDialog	o_Dlg("Who ?");
	static EDIA_cl_NameDialog	o_Dlg1("What ?");
	CString				who, what, what1;
	char				asz_NameR[L_MAX_PATH];
	char				az[4096], *pz;
	FILE				*f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_Dlg.DoModal() == IDOK)
	{
		who = o_Dlg.mo_Name;
		if(o_Dlg1.DoModal() == IDOK)
		{
			what = o_Dlg1.mo_Name;

			strcpy(az, mst_Ini.asz_CurrentRefFile);
			pz = L_strrchr(az, '\\');
			if(pz) *pz = 0;
			strcpy(asz_NameR, az);
			strcat(asz_NameR, "/");
			strcat(asz_NameR, (char *) (LPCSTR) who);
			strcat(asz_NameR, ".msg");

			if(what.IsEmpty())
				L_unlink(asz_NameR);
			else
			{
				f = fopen(asz_NameR, "w");
				if(f)
				{
					what1.Format("FROM %s : ", mst_Ini.asz_CurrentUserName);
					what1 += what;
					fwrite((char *) (LPCSTR) what1, what1.GetLength() + 1, 1, f);
					fclose(f);
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::DisplayMsg(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static DWORD timebeg = 0;
	static DWORD timesta = 0;
	static int state = 0;
	CString	str;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	str = mo_Status.GetPaneText(2);
	if(!str.IsEmpty())
	{
		CDC *pdc;
		CRect rect;
		CFont *poldfnt;
		rect.InflateRect(-2, -2);
		pdc = mo_Status.GetDC();
		mo_Status.GetItemRect(2, &rect);
		pdc->SetBkColor(0x005050FF);
		poldfnt = pdc->SelectObject(&mo_Fnt);
		pdc->ExtTextOut(rect.left + 10, rect.top + 2, ETO_OPAQUE | ETO_CLIPPED, &rect, str, NULL);
		pdc->SelectObject(poldfnt);
		ReleaseDC(pdc);
	}


	if(EDI_gaz_Message[0] == 0) return;

	if(!timebeg)
	{
		timebeg = timeGetTime();
		timesta = timeGetTime();
		state = 0;
	}

	if(timeGetTime() - timebeg > 6000)
	{
		mo_Status.SetPaneText(1, EDI_gaz_Message);
		mo_Status.Invalidate();
		EDI_gaz_Message[0] = 0;
		timebeg = 0;
		state = 0;
		return;
	}

	if(timeGetTime() - timesta > (DWORD) (state == 0 ? 500 : 300))
	{
		timesta = timeGetTime();
		state = (state + 1) % 2;
	}

	L_strupr(EDI_gaz_Message);
	switch(state)
	{
	case 0:	
		{
			CDC *pdc;
			CRect rect;
			CFont *poldfnt;
			rect.InflateRect(-2, -2);
			pdc = mo_Status.GetDC();
			mo_Status.GetItemRect(1, &rect);
			pdc->SetBkColor(0x005050FF);
			poldfnt = pdc->SelectObject(&mo_Fnt);
			pdc->ExtTextOut(rect.left + 10, rect.top + 2, ETO_OPAQUE | ETO_CLIPPED, &rect, EDI_gaz_Message, NULL);
			pdc->SelectObject(poldfnt);
			ReleaseDC(pdc);
			break;
		}
	case 1:	
		{
			mo_Status.SetPaneText(1, " "); 
			mo_Status.Invalidate();
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::VssUpdateStatus(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef MONTREAL_SPECIFIC
	return;
#endif 

	static DWORD dwTickCount = GetTickCount() ;

	// checking messages 
	if ( !mst_Ini.b_LinkControlON && (GetTickCount() - dwTickCount) < 360000 )
	return; 

	char	asz_NameR[L_MAX_PATH];
	char	az[4096], *pz;

	dwTickCount = GetTickCount();

	if ( !mst_Ini.b_LinkControlON )
	{
		strcpy(mst_Ini.asz_CurrentRefFile,"y:\\King Kong\\Bigfile\\");
	}
	
	if(*mst_Ini.asz_CurrentRefFile)
	{
		/////////////////////
		strcpy(az, mst_Ini.asz_CurrentRefFile);
		pz = L_strlen(az) + az;
		while(*pz != '/' && *pz != '\\') pz--;
		if(pz) *pz = 0;

		strcpy(asz_NameR, az);
		strcat(asz_NameR, "/");
		strcat(asz_NameR, mst_Ini.asz_CurrentUserName);
		strcat(asz_NameR, ".msg");
		if(!L_access(asz_NameR, 0))
		{
			FILE *f = L_fopen(asz_NameR, "r");
			if(f)
			{
				L_fread(EDI_gaz_Message, 4096, 1, f);
				fclose(f);
			}

			L_unlink(asz_NameR);
		}
		else
		{
			strcpy(asz_NameR, az);
			strcat(asz_NameR, "/");
			strcat(asz_NameR, "all.msg");
			if(!L_access(asz_NameR, 0))
			{
				if(!hassomethingmsg)
				{
					FILE *f = L_fopen(asz_NameR, "r");
					hassomethingmsg = TRUE;
					if(f)
					{
						L_fread(EDI_gaz_Message, 4096, 1, f);
						fclose(f);
					}
				}
			}
			else
			{
				hassomethingmsg = FALSE;
				strcpy(asz_NameR, az);
				strcat(asz_NameR, "/");
				strcat(asz_NameR, "version.msg");
				if(!L_access(asz_NameR, 0))
				{
					struct L_finddata_t st_FileInfo;
					ULONG				ul_Handle;

					ul_Handle = L_findfirst(asz_NameR, &st_FileInfo);
					if(ul_Handle != -1) L_findclose(ul_Handle);		
					if(timemsg != st_FileInfo.time_write)
					{
						timemsg = st_FileInfo.time_write;
						FILE *f = L_fopen(asz_NameR, "r");
						if(f)
						{
							char azaz[1000];
							char *pz;
							int	 version;
							L_fread(azaz, 4096, 1, f);
							fclose(f);
							pz = azaz;
							while(*pz && !L_isdigit(*pz)) pz++;
							version = L_atoi(pz);
							if(version && (version > BIG_Cu4_AppVersion))
							{
								sprintf(EDI_gaz_Message, "You have an old version of jade.       Latest version is %d", version);
							}
						}
					}
				}
				else
				{
					timemsg = 0;
				}
			}
		}

		/////////////////////
		if ( mst_Ini.b_LinkControlON ) 
		{
			char	az1[4096];
			char	asz_NameW[L_MAX_PATH];

			ComputeLockedFileName(mst_Ini.asz_CurrentRefFile, asz_NameW);
			if(!L_access(asz_NameW, 0))
			{
				if(!hassomething)
				{
					FILE *f = L_fopen(asz_NameW, "r");
					if(f)
					{
						L_fread(az, 4096, 1, f);
						L_fclose(f);
						if(!*az)
							mo_Status.SetPaneText(2, " WRITE ACCESS");
						else
						{
							pz = CleanAZ(az);
							sprintf(az1, "%s", pz);
							mo_Status.SetPaneText(2, az1);
						}

						hassomething = TRUE;
					}
					else
					{
						mo_Status.SetPaneText(2, " WRITE ACCESS");
						hassomething = TRUE;
					}
				}
				return;
			}
		}
	}

	hassomething = FALSE;
	mo_Status.SetPaneText(2, "");
}

/*
 =======================================================================================================================
    Aim:    To lock a bigfile (in fact to lock access to vss ones). If file is locked for reading, a __locked__.r file
            is created to tell that the file is locked. To save, a __locked__.w file is created. Generate an error if
            file is already locked, and can't be access (only a read access is permit during a read acess).

    In:     _b_ForRead  TRUE for a read access. 

    Out:    TRUE if no error, FALSE else.
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::b_LockBigFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_NameW[L_MAX_PATH];
	FILE	*p_File;
	DWORD	time;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!EDI_can_lock) return TRUE;

	/* Can write ? */
	if(!mi_VssCanWrite)
	{
		ERR_X_ForceError("You do not have write permission on reference bigfile", NULL);
		return FALSE;
	}

	/* Reference bigfile has been open by someone ? */
	ComputeLockedFileOpen(mst_Ini.asz_CurrentRefFile, asz_NameW);
	if(!L_access(asz_NameW, 0))
	{
		hassomething = FALSE;
		ERR_X_ForceError("Reference BigFile is currently open by someone ! Can't access", NULL);
		return FALSE;
	}

	time = timeGetTime();
loop:
	ComputeLockedFileName(mst_Ini.asz_CurrentRefFile, asz_NameW);
	if(!L_access(asz_NameW, 0)) 
	{
		if(timeGetTime() - time < 1000) goto loop;
		ERR_X_ForceError(EDI_ERR_Csz_Locked, NULL);
		return FALSE;
	}

	p_File = L_fopen(asz_NameW, L_fopen_WB);
	if(!p_File)
	{
		ERR_X_ForceError(EDI_ERR_Csz_Locked, NULL);
		return FALSE;
	}

	fwrite(M_MF()->mst_Ini.asz_CurrentUserName, strlen(M_MF()->mst_Ini.asz_CurrentUserName) + 1, 1, p_File);
	fclose(p_File);

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    To unlock file.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::UnLockBigFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_NameW[L_MAX_PATH];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!EDI_can_lock) return;

	ComputeLockedFileName(mst_Ini.asz_CurrentRefFile, asz_NameW);

	/* Only one can locked writing. So we can delete file */
	if(!L_access(asz_NameW, 0))
	{
		L_unlink(asz_NameW);
		return;
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
void EDI_cl_MainFrame::OnLinkControl(void)
{
	VSS_gb_OpenOneTime = TRUE;
	go_VSSDialog.mb_Open = FALSE;
	if(go_VSSDialog.DoModal() == IDOK)
	{
		/* Set new values in ini */
		mst_Ini.b_LinkControlON = (BOOL) go_VSSDialog.mi_LinkControlON;
		L_strcpy(mst_Ini.asz_CurrentUserName, go_VSSDialog.mo_UserName);
		L_strcpy(mst_Ini.asz_UserPassWord, (char *) (LPCSTR) go_VSSDialog.mo_UserPassWord);
		L_strcpy(mst_Ini.asz_CurrentVSSFile, go_VSSDialog.masz_LinkControlFileVSS);
		L_strcpy(mst_Ini.asz_CurrentRefFile, go_VSSDialog.masz_LinkControlFileREF);
		mi_VssCanWrite = go_VSSDialog.mi_CanWrite;
	}

	SendMessageToEditors(EDI_MESSAGE_REFRESHMENU, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnLinkControlBeg(void)
{
	if(EDI_gb_NoVerbose || EDI_gb_SlashC)
	{
		mst_Ini.b_LinkControlON = FALSE;
		return;
	}

	go_VSSDialog.mb_Open = TRUE;
	if(go_VSSDialog.DoModal() == IDOK)
	{
		/* Set new values in ini */
		mst_Ini.b_LinkControlON = (BOOL) go_VSSDialog.mi_LinkControlON;
		L_strcpy(mst_Ini.asz_CurrentUserName, go_VSSDialog.mo_UserName);
		L_strcpy(mst_Ini.asz_UserPassWord, (char *) (LPCSTR) go_VSSDialog.mo_UserPassWord);
		L_strcpy(mst_Ini.asz_CurrentVSSFile, go_VSSDialog.masz_LinkControlFileVSS);
		L_strcpy(mst_Ini.asz_CurrentRefFile, go_VSSDialog.masz_LinkControlFileREF);
		mi_VssCanWrite = go_VSSDialog.mi_CanWrite;
	}

	LINK_Refresh();
}

BOOL EDI_gb_NoUpdateVSS = FALSE;
int EDI_gi_OnlyOneVssCheck= 0;
int EDI_gi_OnlyOneRefCheck= 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::VssFileChanged(BOOL _b_NoUpdateVss, BOOL _b_Force)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct L_finddata_t st_FileInfo;
	ULONG				ul_Handle;
	BOOL				b_VSS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*
   
    finallement on n a pas confiance dans cette modif
    on la vire 

    if(EDI_gi_OnlyOneVssCheck>0)
    {
        EDI_gi_OnlyOneVssCheck = -1;
    }
    else if(EDI_gi_OnlyOneVssCheck<0)
    {
        _b_NoUpdateVss = TRUE;
    }
    */

	ul_Handle = L_findfirst(M_MF()->mst_Ini.asz_CurrentVSSFile, &st_FileInfo);
	if(ul_Handle != -1) L_findclose(ul_Handle);
	if(_b_NoUpdateVss && x_TimeVSS) goto zap;
	if(_b_Force || (x_TimeVSS != st_FileInfo.time_write))
	{
		COPY_BIG2TO1();	/* VSS en 1 */
		SWAP_BIG();		/* VSS en 0 */
		BIG_Close();
		RESTORE_BIG();

		LINK_PrintStatusMsg("Update datacontrol status");
		x_TimeVSS = st_FileInfo.time_write;

		b_VSS = TRUE;
		COPY_BIG();
_Try_
		BIG_Open(mst_Ini.asz_CurrentVSSFile);
_Catch_
		b_VSS = FALSE;
		x_TimeVSS = 0;
		SWAP_BIG();
_End_
		if(b_VSS)
		{
			COPY_BIG2();	/* VSS en 2 */
			SWAP_BIG();		/* Restore BF en 0 */
			COPY_BIG2TO1();	/* VSS en 1 */
		}
	}
	else
	{
zap:
		COPY_BIG2TO1();	/* VSS en 1 */
		do
		{
			BIG_gst1.h_CLibFileHandle = L_fopen(M_MF()->mst_Ini.asz_CurrentVSSFile, L_fopen_RPB);
		} while(!BIG_gst1.h_CLibFileHandle);
	}

	EDI_gb_NoUpdateVSS = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::REFFileChanged(BOOL _b_NoUpdateVss, BOOL _b_Force)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct L_finddata_t st_FileInfo;
	ULONG				ul_Handle;
	BOOL				b_VSS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*
   
    finallement on n a pas confiance dans cette modif
    on la vire 

    if(EDI_gi_OnlyOneRefCheck >0)
    {
        EDI_gi_OnlyOneRefCheck = -1;
    }
    else if(EDI_gi_OnlyOneRefCheck<0)
    {
        _b_NoUpdateVss = TRUE;
    }
    */

    RESTORE_BIG();
	ul_Handle = L_findfirst(M_MF()->mst_Ini.asz_CurrentRefFile, &st_FileInfo);
	if(ul_Handle != -1) L_findclose(ul_Handle);
	if((_b_NoUpdateVss) && x_TimeREF) goto zap;
	if(_b_Force || (x_TimeREF != st_FileInfo.time_write))
	{
		COPY_BIG3TO1();	/* REF en 1 */
		SWAP_BIG();		/* REF en 0 */
		BIG_Close();
		RESTORE_BIG();

		LINK_PrintStatusMsg("Update reference file");
		x_TimeREF = st_FileInfo.time_write;

		b_VSS = TRUE;
		COPY_BIG();
_Try_
		BIG_Open(mst_Ini.asz_CurrentRefFile);
_Catch_
		b_VSS = FALSE;
		x_TimeREF = 0;
		SWAP_BIG();
_End_
		if(b_VSS)
		{
			COPY_BIG3();	/* REF en 3 */
			SWAP_BIG();		/* Restore BF en 0 */
			COPY_BIG3TO1();	/* REF en 1 */
		}
	}
	else
	{
zap:
		COPY_BIG3TO1();	/* REF en 1 */
		do
		{
			BIG_gst1.h_CLibFileHandle = L_fopen(M_MF()->mst_Ini.asz_CurrentRefFile, L_fopen_RPB);
		} while(!BIG_gst1.h_CLibFileHandle);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::CloseVssFile(void)
{
	SWAP_BIG();
	if(BIG_gst.h_CLibFileHandle) L_fclose(BIG_gst.h_CLibFileHandle);
	BIG_Handle() = NULL;
	BIG_gst2.h_CLibFileHandle = NULL;
    COPY_BIG2();
	RESTORE_BIG();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::DestroyVssFile(void)
{
	if(x_TimeVSS)
	{
		x_TimeVSS = 0;
		COPY_BIG2TO1();
		SWAP_BIG();
		BIG_Close();
		RESTORE_BIG();
		L_memset(&BIG_gst2, 0, sizeof(BIG_gst2));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::CloseRefFile(void)
{
	if(BIG_gst.h_CLibFileHandle) L_fclose(BIG_gst.h_CLibFileHandle);
	BIG_Handle() = NULL;
	BIG_gst3.h_CLibFileHandle = NULL;
    COPY_BIG3();
	RESTORE_BIG();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::DestroyRefFile(void)
{
	if(x_TimeREF)
	{
		x_TimeREF = 0;
		COPY_BIG3TO1();
		SWAP_BIG();
		BIG_Close();
		RESTORE_BIG();
		L_memset(&BIG_gst3, 0, sizeof(BIG_gst3));
	}
}

#endif /* ACTIVE_EDITORS */

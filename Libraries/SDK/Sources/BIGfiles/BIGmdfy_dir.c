/*$T BIGmdfy_dir.c GC 1.138 05/16/03 13:45:15 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGerrid.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "EDIpaths.h"

/* If TRUE, s_CreateDirFromDisk will be recursive */
BOOL	BIG_gb_ImportRecurse = TRUE;

/* Import a dir as a mirror */
BOOL	BIG_gb_ImportDirMirror = FALSE;
BOOL	BIG_gb_ImportDirInvMirror = FALSE;

/* Import a dir only if files are yet loaded once */
BOOL	BIG_gb_ImportDirOnlyLoaded = FALSE;

/* Import a dir only if files are truncated */
BOOL	BIG_gb_ImportDirOnlyTruncated = FALSE;

/* Take care of valids extensions to import */
BOOL	BIG_gb_ImportWithExt = FALSE;

BOOL	BIG_b_CheckUserAbortCommand(void);

/*
 =======================================================================================================================
    Aim: Recurse delete a directory. In: _psz_Path Full path name (to display) of dir to delete. _ul_Index Current fat
    index of the dir to delete.
 =======================================================================================================================
 */
static void s_RecurseDeleteDir(char *_psz_Path, BIG_INDEX _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Prev, ul_Next, ul_End, ul_Memo;
	BIG_INDEX	i;
	char		asz_Temp[BIG_C_MaxLenPath];
	char		asz_Msg[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(asz_Msg, "%s%s", "Delete Dir : ", _psz_Path);
	LINK_PrintStatusMsg(asz_Msg);

	/* Delete files of that directory */
	ul_Next = BIG_FirstFile(_ul_Index);
	while(ul_Next != BIG_C_InvalidIndex)
	{
		ul_Memo = BIG_NextFile(ul_Next);
		BIG_DeleteFile(_ul_Index, ul_Next);
		ul_Next = ul_Memo;
	}

	/* Recurse call for all sons. */
	i = BIG_SubDir(_ul_Index);
	if(i != BIG_C_InvalidIndex)
	{
		while(i != BIG_C_InvalidIndex)
		{
			L_strcpy(asz_Temp, _psz_Path);
			L_strcat(asz_Temp, "/");
			L_strcat(asz_Temp, BIG_NameDir(i));
			s_RecurseDeleteDir(asz_Temp, i);

			BIG_DirChanged(i) = EDI_FHC_Delete;
			BIG_ParentDir(i) = BIG_C_InvalidIndex;
			BIG_SubDir(i) = BIG_C_InvalidIndex;
			BIG_UpdateOneDirInFat(i);

			ul_End = i;
			i = BIG_NextDir(ul_End);
		}

		/* Link all dirs to free dir list */
		ul_Memo = BIG_FreeDir();
		ul_Prev = BIG_SubDir(_ul_Index);
		BIG_FreeDir() = ul_Prev;
		BIG_NextDir(ul_End) = ul_Memo;
		if(ul_Memo != BIG_C_InvalidIndex)
		{
			BIG_PrevDir(ul_Memo) = ul_End;
			BIG_UpdateOneDirInFat(ul_Memo);
		}

		/* Update fat */
		BIG_UpdateOneDirInFat(ul_End);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BIG_b_CanImportFile(char *_psz_Name)
{
	/*~~~~~~~~~~~~~~*/
	char	*psz_Temp;
	/*~~~~~~~~~~~~~~*/

	psz_Temp = L_strrchr(_psz_Name, '.');
	if(!psz_Temp) return FALSE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtTexture1, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, ".mad", 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtSoundFile, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtLoadingSound, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtSoundMusic, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtSoundAmbience, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtSoundDialog, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtAnimation, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtSkeleton, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtSkin, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtAnimTbl, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtEventAllsTracks, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtVideo1, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtVideo2, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtVideo3, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtVideo4, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, EDI_Csz_ExtVideo5, 4)) return TRUE;
	if(!L_strnicmp(psz_Temp, ".bin", 4)) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_CreateDirFromDisk(char *_psz_RealName, char *_psz_BigName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct L_finddata_t st_FileInfo;
	ULONG				ul_Handle;
	char				asz_Temp[BIG_C_MaxLenPath];
	char				asz_RealName[BIG_C_MaxLenPath];
	char				asz_BigName[BIG_C_MaxLenPath];
	BIG_INDEX			ul_Dir, ul_File;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg(_psz_RealName);
	if(LINK_gb_EscapeDetected) return;

	/* Create current dir in bigfile (if necessary). Get the index. */
	ul_Dir = BIG_ul_CreateDir(_psz_BigName);
	BIG_DirChanged(ul_Dir) |= EDI_FHC_Touch;

	/* Find all files for that dir */
	L_strcpy(asz_Temp, _psz_RealName);
	L_strcat(asz_Temp, "/*.*");
	ul_Handle = L_findfirst(asz_Temp, &st_FileInfo);
	if(ul_Handle != -1)
	{
		do
		{
			/* Compute real name and file/dir bigfile name */
			L_strcpy(asz_RealName, _psz_RealName);
			L_strcat(asz_RealName, "/");
			L_strcat(asz_RealName, st_FileInfo.name);

			/* One dir has been detected. Recurse call except for "." and "..". */
			if(st_FileInfo.attrib & L_A_SUBDIR)
			{
				if(!L_strcmpi(st_FileInfo.name, ".")) continue;
				if(!L_strcmpi(st_FileInfo.name, "..")) continue;
				if(!BIG_gb_ImportRecurse) continue;

				/* Compute name of directory */
				L_strcpy(asz_BigName, _psz_BigName);
				L_strcat(asz_BigName, "/");
				L_strcat(asz_BigName, st_FileInfo.name);

				s_CreateDirFromDisk(asz_RealName, asz_BigName);

				BIG_b_CheckUserAbortCommand();
				if(LINK_gb_EscapeDetected) return;
			}

			/* One file has been detected. Add it to bigfile. */
			else
			{
				BIG_b_CheckUserAbortCommand();
				if(LINK_gb_EscapeDetected) return;

				BIG_gpsz_RealName = asz_RealName;	/* Save real name of file on disk */
				BIG_gb_GlobalTimeValid = TRUE;		/* We have already the write time */
				BIG_gx_GlobalTimeWrite = st_FileInfo.time_write;

				if(!BIG_gb_ImportWithExt || BIG_b_CanImportFile(st_FileInfo.name))
				{
					ul_File = BIG_ul_SearchFile(ul_Dir, st_FileInfo.name);
					if(!BIG_gb_ImportDirInvMirror || (ul_File == BIG_C_InvalidIndex))
					{
						/*~~~~~~~~~~~~~~~*/
						BOOL	b_DoImport;
						/*~~~~~~~~~~~~~~~*/

						b_DoImport = TRUE;

						if(ul_File != BIG_C_InvalidIndex)
						{
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
							extern ULONG	BIG_ul_EditorGetSizeOfFile(ULONG);
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

							if(BIG_gb_ImportDirOnlyLoaded && ((BIG_FileChanged(ul_File) & EDI_FHC_Loaded) == 0))
							{
								b_DoImport = FALSE;
							}

							if(BIG_gb_ImportDirOnlyTruncated && (BIG_ul_EditorGetSizeOfFile(BIG_FileKey(ul_File)) != 8))
							{
								b_DoImport = FALSE;
							}
						}

						if(b_DoImport)
						{
							BIG_ul_UpdateCreateFileOptim
							(
								ul_Dir,
								ul_File,
								_psz_BigName,
								st_FileInfo.name,
								BIG_CallbackLoadFileFromDisk,
								TRUE
							);
						}
					}
				}

				BIG_gb_GlobalTimeValid = FALSE;
			}
		} while(L_findnext(ul_Handle, &st_FileInfo) != -1);
		L_findclose(ul_Handle);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_UpdateDirFromDisk(char *_psz_RealName, char *_psz_BigName)
{
	s_CreateDirFromDisk(_psz_RealName, _psz_BigName);
	BIG_WriteHeader();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BIG_INDEX BIG_ul_CreateDir(char *_psz_PathName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_NewIndex, ul_ParentIndex, ul_Memo;
	char		asz_TempName[BIG_C_MaxLenPath];
	char		*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Create all parent dir */
	L_strcpy(asz_TempName, _psz_PathName);
	psz_Temp = L_strchr(asz_TempName, '/');
	while(psz_Temp)
	{
		*psz_Temp = 0;
		ul_NewIndex = BIG_ul_SearchDir(asz_TempName);
		if(ul_NewIndex == BIG_C_InvalidIndex)
		{
			BIG_ul_CreateDir(asz_TempName);
		}

		*psz_Temp = '/';
		psz_Temp = L_strchr(psz_Temp + 1, '/');
	}

	/* If new directory exists, make nothing */
	ul_NewIndex = BIG_ul_SearchDir(_psz_PathName);
	if(ul_NewIndex != BIG_C_InvalidIndex) return ul_NewIndex;

	/* Name of parent. If no parent, exit (cause there's ROOT directory) */
	L_strcpy(asz_TempName, _psz_PathName);
	psz_Temp = L_strrchr(asz_TempName, '/');
	if(psz_Temp)
	{
		*psz_Temp = '\0';
		psz_Temp++;
	}
	else
	{
		psz_Temp = asz_TempName;
	}

	/* Get parent index */
	ul_ParentIndex = BIG_ul_SearchDir(asz_TempName);

	/* Add a new directory in table */
	ul_NewIndex = BIG_ul_AddDirInTable();

	/* Remember operation */
	BIG_DirChanged(ul_NewIndex) = EDI_FHC_AddUpdate;

	/* Update parent directory and file link */
	ul_Memo = BIG_C_InvalidIndex;
	if(ul_ParentIndex != BIG_C_InvalidIndex)
	{
		ul_Memo = BIG_SubDir(ul_ParentIndex);
		BIG_SubDir(ul_ParentIndex) = ul_NewIndex;
	}

	BIG_NextDir(ul_NewIndex) = ul_Memo;
	BIG_PrevDir(ul_NewIndex) = BIG_C_InvalidIndex;
	BIG_ParentDir(ul_NewIndex) = ul_ParentIndex;
	if(ul_Memo != BIG_C_InvalidIndex)
	{
		BIG_PrevDir(ul_Memo) = ul_NewIndex;
	}

	/* Name of the new directory (atomic last part) */
	if(L_strlen(psz_Temp) >= BIG_C_MaxLenPath)
	{
		L_memcpy(BIG_NameDir(ul_NewIndex), psz_Temp, BIG_C_MaxLenPath - 1);
		BIG_NameDir(ul_NewIndex)[BIG_C_MaxLenPath - 1] = 0;
	}
	else
	{
		L_strcpy(BIG_NameDir(ul_NewIndex), psz_Temp);
	}

	/* Content of structure */
	BIG_FirstFile(ul_NewIndex) = BIG_C_InvalidIndex;
	BIG_SubDir(ul_NewIndex) = BIG_C_InvalidIndex;

	/* Update header */
	BIG_WriteHeader();

	/* Update fat dir */
	if(ul_ParentIndex != BIG_C_InvalidIndex) BIG_UpdateOneDirInFat(ul_ParentIndex);

	BIG_UpdateOneDirInFat(ul_NewIndex);
	if(ul_Memo != BIG_C_InvalidIndex) BIG_UpdateOneDirInFat(ul_Memo);

	return ul_NewIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_DelDir(char *_psz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Parent, ul_Index, ul_Memo;
	char		asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Retrieve index of directory to check if it exists */
	ul_Index = BIG_ul_SearchDir(_psz_Name);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

	/* Recurse delete */
	BIG_ComputeFullName(ul_Index, asz_Path);
	s_RecurseDeleteDir(asz_Path, ul_Index);

	/* Unlink dir of list */
	ul_Parent = BIG_ParentDir(ul_Index);
	if((ul_Parent != BIG_C_InvalidIndex) && (BIG_SubDir(ul_Parent) == ul_Index))
	{
		BIG_SubDir(ul_Parent) = BIG_NextDir(ul_Index);
		BIG_UpdateOneDirInFat(ul_Parent);
	}
	else
	{
		ul_Memo = BIG_PrevDir(ul_Index);
		if(ul_Memo != BIG_C_InvalidIndex)
		{
			BIG_NextDir(ul_Memo) = BIG_NextDir(ul_Index);
			BIG_UpdateOneDirInFat(ul_Memo);
		}
	}

	/* Unlink from next */
	ul_Memo = BIG_NextDir(ul_Index);
	if(ul_Memo != BIG_C_InvalidIndex)
	{
		BIG_PrevDir(ul_Memo) = BIG_PrevDir(ul_Index);
		BIG_UpdateOneDirInFat(ul_Memo);
	}

	/* Link dir to free dir list */
	ul_Memo = BIG_FreeDir();
	BIG_FreeDir() = ul_Index;
	BIG_PrevDir(ul_Index) = BIG_C_InvalidIndex;
	BIG_NextDir(ul_Index) = ul_Memo;
	if(ul_Memo != BIG_C_InvalidIndex)
	{
		BIG_PrevDir(ul_Memo) = ul_Index;
		BIG_UpdateOneDirInFat(ul_Memo);
	}

	BIG_DirChanged(ul_Index) = EDI_FHC_Delete;
	BIG_UpdateOneDirInFat(ul_Index);

	/* Update header */
	BIG_WriteHeader();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_RenDir(char *_psz_NewName, char *_psz_OldName)
{
	/*~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	char		*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~*/

	/* New directory must not already exists */
	ERR_X_Assert(BIG_ul_SearchDir(_psz_NewName) == BIG_C_InvalidIndex);

	/* Retrieve index of old directory (not its parent) */
	ul_Index = BIG_ul_SearchDir(_psz_OldName);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

	/* Rename the directory */
	psz_Temp = L_strrchr(_psz_NewName, '/');
	if(psz_Temp)
		psz_Temp++;
	else
		psz_Temp = _psz_NewName;

	if(L_strlen(psz_Temp) >= BIG_C_MaxLenName)
	{
		L_memcpy(BIG_NameDir(ul_Index), psz_Temp, BIG_C_MaxLenName - 1);
		BIG_NameDir(ul_Index)[BIG_C_MaxLenName - 1] = 0;
	}
	else
	{
		L_strcpy(BIG_NameDir(ul_Index), psz_Temp);
	}

	/* Remember operation */
	BIG_DirChanged(ul_Index) = EDI_FHC_Rename;

	/* Update fat */
	BIG_UpdateOneDirInFat(ul_Index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_CheckBadDir(ULONG _ul_DestPath, ULONG _ul_SrcPath)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ul_Parent;
	/*~~~~~~~~~~~~~~*/

	ul_Parent = BIG_ParentDir(_ul_DestPath);
	while(ul_Parent != BIG_C_InvalidIndex)
	{
		ERR_X_Error(ul_Parent != _ul_SrcPath, ERR_BIG_Csz_CantMoveDir, NULL);
		ul_Parent = BIG_ParentDir(ul_Parent);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_CopyDirRecurse(char *_psz_Dest, char *_psz_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	BIG_INDEX	ul_DestPath, ul_SrcPath;
	char		asz_DestName[BIG_C_MaxLenPath];
	char		asz_SrcName[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg(_psz_Src);

	BIG_b_CheckUserAbortCommand();
	if(LINK_gb_EscapeDetected) return;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	_Try_	ul_DestPath = BIG_ul_SearchDir(_psz_Dest);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ul_DestPath == BIG_C_InvalidIndex) BIG_ul_CreateDir(_psz_Dest);

	ul_SrcPath = BIG_ul_SearchDir(_psz_Src);

	/* Recurse call for each subdir */
	ul_Index = BIG_SubDir(ul_SrcPath);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		L_strcpy(asz_DestName, _psz_Dest);
		L_strcat(asz_DestName, "/");
		L_strcat(asz_DestName, BIG_NameDir(ul_Index));

		L_strcpy(asz_SrcName, _psz_Src);
		L_strcat(asz_SrcName, "/");
		L_strcat(asz_SrcName, BIG_NameDir(ul_Index));

		s_CopyDirRecurse(asz_DestName, asz_SrcName);

		BIG_b_CheckUserAbortCommand();
		if(LINK_gb_EscapeDetected) return;

		ul_Index = BIG_NextDir(ul_Index);
	}

	/* Copy all files */
	ul_Index = BIG_FirstFile(ul_SrcPath);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		BIG_b_CheckUserAbortCommand();
		if(LINK_gb_EscapeDetected) return;

		BIG_CopyFile(_psz_Dest, _psz_Src, BIG_NameFile(ul_Index));
		ul_Index = BIG_NextFile(ul_Index);
	}

	_Catch_ _End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_CopyDir(char *_psz_Dest, char *_psz_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_DestName[BIG_C_MaxLenPath];
	char	*psz_Tmp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	psz_Tmp = L_strrchr(_psz_Src, '/');
	if(!psz_Tmp) return;

	L_strcpy(asz_DestName, _psz_Dest);
	L_strcat(asz_DestName, psz_Tmp);

	s_CopyDirRecurse(asz_DestName, _psz_Src);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_MoveDir(char *_psz_Dest, char *_psz_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	BIG_INDEX	ul_DestPath, ul_SrcPath;
	_Try_		ul_DestPath = BIG_ul_SearchDir(_psz_Dest);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(ul_DestPath != BIG_C_InvalidIndex);
	ul_SrcPath = BIG_ul_SearchDir(_psz_Src);
	ERR_X_Assert(ul_SrcPath != BIG_C_InvalidIndex);

	if(ul_DestPath == ul_SrcPath) _Return_(;);

	/* Check if dest is correct */
	s_CheckBadDir(ul_DestPath, ul_SrcPath);

	/* Check if dir already exists in dest */
	ul_Index = BIG_ul_SearchDirInDir(ul_DestPath, BIG_NameDir(ul_SrcPath));
	ERR_X_Error(ul_Index == BIG_C_InvalidIndex, ERR_BIG_Csz_CantMoveDir, BIG_NameDir(ul_SrcPath));

	/* Unlink path from current parent */
	ul_Index = BIG_NextDir(ul_SrcPath);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		BIG_PrevDir(ul_Index) = BIG_PrevDir(ul_SrcPath);
		BIG_UpdateOneDirInFat(ul_Index);
	}

	ul_Index = BIG_PrevDir(ul_SrcPath);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		BIG_NextDir(ul_Index) = BIG_NextDir(ul_SrcPath);
		BIG_UpdateOneDirInFat(ul_Index);
	}
	else
	{
		ul_Index = BIG_ParentDir(ul_SrcPath);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			BIG_SubDir(ul_Index) = BIG_NextDir(ul_SrcPath);
			BIG_UpdateOneDirInFat(ul_Index);
		}
	}

	/* Add dir at the first position of the new path */
	ul_Index = BIG_SubDir(ul_DestPath);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		BIG_PrevDir(ul_Index) = ul_SrcPath;
		BIG_UpdateOneDirInFat(ul_Index);
	}

	BIG_SubDir(ul_DestPath) = ul_SrcPath;
	BIG_NextDir(ul_SrcPath) = ul_Index;
	BIG_ParentDir(ul_SrcPath) = ul_DestPath;
	BIG_PrevDir(ul_SrcPath) = BIG_C_InvalidIndex;

	/* Update fat */
	BIG_UpdateOneDirInFat(ul_DestPath);
	BIG_UpdateOneDirInFat(ul_SrcPath);

	/* Set as changed */
	BIG_DirChanged(ul_DestPath) = EDI_FHC_Content;
	ul_Index = BIG_ParentDir(ul_SrcPath);
	if(ul_Index != BIG_C_InvalidIndex) BIG_DirChanged(ul_Index) = EDI_FHC_Content;
	BIG_DirChanged(ul_SrcPath) = EDI_FHC_Move;

	_Catch_ _End_
}

/*
 =======================================================================================================================
    Aim: To move all file with no change to another directory (keep the sub tree) In: _ul_SrcRef Index of dir to scan
    _sz_TgtName New root for unchanged file (assume that name length is at least BIG_C_MaxLenPath)
 =======================================================================================================================
 */
void BIG_MoveUnchangedFiles(BIG_INDEX _ul_SrcRef, char *_sz_TgtName, char *_sz_Filter)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Child, ul_ChildNext, ul_Dir, ul_Target;
	char		sz_SrcName[BIG_C_MaxLenPath];
	char		*psz_EndTgtName;
	char		c_DirHasChanged;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Init */
	*sz_SrcName = 0;

	/* Treat file */
	ul_Child = BIG_FirstFile(_ul_SrcRef);
	while(ul_Child != BIG_C_InvalidIndex)
	{
		ul_ChildNext = BIG_NextFile(ul_Child);

		if(BIG_b_IsFileExtensionIn(ul_Child, _sz_Filter))
		{
			if(!BIG_FileChanged(ul_Child))
			{
				ul_Dir = BIG_ul_CreateDir(_sz_TgtName);
				if(*sz_SrcName == 0) BIG_ComputeFullName(_ul_SrcRef, sz_SrcName);

				ul_Target = BIG_ul_SearchFileExt(_sz_TgtName, BIG_NameFile(ul_Child));
				if(ul_Target != BIG_C_InvalidIndex) BIG_DelFile(_sz_TgtName, BIG_NameFile(ul_Child));
				BIG_MoveFile(_sz_TgtName, sz_SrcName, BIG_NameFile(ul_Child));
			}
		}

		ul_Child = ul_ChildNext;
	}

	/* Treat sub dir */
	psz_EndTgtName = _sz_TgtName + strlen(_sz_TgtName);
	*psz_EndTgtName = '/';

	ul_Child = BIG_SubDir(_ul_SrcRef);
	while(ul_Child != BIG_C_InvalidIndex)
	{
		c_DirHasChanged = BIG_DirChanged(ul_Child);
		ul_ChildNext = BIG_NextDir(ul_Child);

		L_strcpy(psz_EndTgtName + 1, BIG_NameDir(ul_Child));
		BIG_MoveUnchangedFiles(ul_Child, _sz_TgtName, _sz_Filter);

		/* Delete dir if not touch and no more file inside <- just to be sure */
		if((!c_DirHasChanged) && (BIG_FirstFile(ul_Child) == BIG_C_InvalidIndex))
		{
			BIG_ComputeFullName(ul_Child, sz_SrcName);
			BIG_DelDir(sz_SrcName);
		}

		ul_Child = ul_ChildNext;
	}

	*psz_EndTgtName = 0;
}

/*
 =======================================================================================================================
    Aim: Clean the touch flag of all files contained in given dir and sub dirs
 =======================================================================================================================
 */
void BIG_UntouchFilesRec(BIG_INDEX _ul_RefDir)
{
	/*~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Child;
	/*~~~~~~~~~~~~~~~~~*/

	if(_ul_RefDir == BIG_C_InvalidIndex) return;

	/* Treat file */
	ul_Child = BIG_FirstFile(_ul_RefDir);
	while(ul_Child != BIG_C_InvalidIndex)
	{
		BIG_FileChanged(ul_Child) &= ~EDI_FHC_Touch;
		ul_Child = BIG_NextFile(ul_Child);
	}

	/* Treat sub dir */
	ul_Child = BIG_SubDir(_ul_RefDir);
	while(ul_Child != BIG_C_InvalidIndex)
	{
		BIG_DirChanged(ul_Child) &= ~EDI_FHC_Touch;
		BIG_UntouchFilesRec(ul_Child);
		ul_Child = BIG_NextDir(ul_Child);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BIG_b_CheckUserAbortCommand(void)
{
	if(LINK_gb_EscapeDetected) return LINK_gb_EscapeDetected;
	if(LINK_gb_IgnoreEscape) return FALSE;

	if(GetAsyncKeyState(VK_ESCAPE) < 0)
	{
		if(MessageBox(NULL, "Abort last command ?", "Please confirm", MB_ICONSTOP | MB_YESNO) == IDYES)
		{
			LINK_gb_EscapeDetected = TRUE;
			LINK_PrintStatusMsg("[WARNING] COMMAND ABORTED BY USER [WARNING]");
		}
	}

	return LINK_gb_EscapeDetected;
}
#endif /* ACTIVE_EDITORS */

/*$T BIGmdfy_file.c GC! 1.081 05/26/00 09:59:24 */


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
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "BIGerrid.h"
#include "BIGmdfy_file.h"
#include "BIGmdfy_dir.h"
#include "BIGio.h"
#include "BIGfat.h"
#include "BIGopen.h"
#include "BIGread.h"
#include "BIGkey.h"
#include "BIGmerge.h"
#include "IMPort/IMPbase.h"
#include "LOAding/LOAdefs.h"

#include "TIMer/PROfiler/PROPS2.h"

/*$4
 ***********************************************************************************************************************
    Global variables
 ***********************************************************************************************************************
 */

char		*BIG_gpsz_RealName;
void		*BIG_gp_Buffer = NULL;
void		*BIG_gp_Buffer1 = NULL;
void		*BIG_gp_Buffer2 = NULL;
ULONG		BIG_gul_Length = 0;
ULONG		BIG_gul_Length1 = 0;
ULONG		BIG_gul_Length2 = 0;
BOOL		BIG_gb_CheckSameName = TRUE;

BOOL		BIG_gb_GlobalTimeValid = FALSE;
L_time_t	BIG_gx_GlobalTimeWrite = 0;
BOOL		BIG_gb_ImportOne = TRUE;
BOOL		BIG_gb_WarningTime = FALSE;
BOOL		BIG_gb_IgnoreRecent = FALSE;
BOOL		BIG_gb_IgnoreDateExt = FALSE;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_CallbackLoadFileFromDisk(BIG_INDEX _ul_Dir, BIG_INDEX _ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	L_FILE				h_TempFile;
	struct L_finddata_t st_FileInfo;
	ULONG				ul_Handle,r;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_TempFile = NULL;

_Try_
	BIG_gp_Buffer = NULL;
    BIG_gp_Buffer1 = BIG_gp_Buffer2 = NULL;
	BIG_gul_Length = 0;

	/*
	 * Retreive infos about source file (to get time). If BIG_gb_GlobalTimeValid is
	 * true, the time is already set in BIG_gx_GlobalTimeWrite
	 */
	if(BIG_gb_GlobalTimeValid)
	{
		st_FileInfo.time_write = BIG_gx_GlobalTimeWrite;
		BIG_gb_GlobalTimeValid = FALSE;
	}
	else
	{
		ul_Handle = L_findfirst(BIG_gpsz_RealName, &st_FileInfo);
		ERR_X_Error(ul_Handle != -1, ERR_BIG_Csz_CantOpenFile, BIG_gpsz_RealName);
		L_findclose(ul_Handle);
	}

	/* If file is already present in the bigfile, test the date */
	if(!BIG_gb_IgnoreDateExt)
	{
		if((_ul_File != BIG_C_InvalidIndex) && (st_FileInfo.time_write == BIG_TimeFile(_ul_File))) return;
	}

	BIG_gx_GlobalTime = st_FileInfo.time_write;

	/* To indicate that we have made one importation (to optimise later) */
	BIG_gb_ImportOne = TRUE;

	/* Open file to read */
	h_TempFile = L_fopen(BIG_gpsz_RealName, L_fopen_RB);
	r=CLI_FileOpen(h_TempFile);
	ERR_X_Error(r, L_ERR_Csz_FOpen, BIG_gpsz_RealName);

	/* Compute file length */
	r=L_fseek(h_TempFile, 0, L_SEEK_END);
	ERR_X_Error(r == 0, L_ERR_Csz_FSeek, BIG_gpsz_RealName);
	BIG_gul_Length = L_ftell(h_TempFile);

	/* Seek to beginning of file */
	r=L_fseek(h_TempFile, 0, L_SEEK_SET);
	ERR_X_Error(r == 0, L_ERR_Csz_FSeek, BIG_gpsz_RealName);

	/* Read the file in the buffer */
	if(BIG_gul_Length != 0)
	{
		/* Allocate buffer to recieve the file */
		BIG_gp_Buffer = BIG_p_RequestBuffer(BIG_gul_Length);
		ERR_X_Error(BIG_gp_Buffer != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

		/* Read the file in the buffer */
		r=BIG_fread(BIG_gp_Buffer, BIG_gul_Length, h_TempFile);
		ERR_X_Error(r == 1, L_ERR_Csz_FRead, BIG_gpsz_RealName);
	}

	/* Close file */
	r=L_fclose(h_TempFile);
	ERR_X_Error(r == 0, L_ERR_Csz_FClose, BIG_gpsz_RealName);

_Catch_
	if(CLI_FileOpen(h_TempFile)) L_fclose(h_TempFile);
_EndThrow_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG BIG_ul_UpdateCreateFile(char *_psz_DirName, char *_psz_FileName, BIG_tdpfnv_Callback _p_Callback, BOOL _b_AskKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Dir, ul_File;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Search dir first (in current bigfile) */
	ul_Dir = BIG_ul_SearchDir(_psz_DirName);
	if(ul_Dir == BIG_C_InvalidIndex) return -1;
	ul_File = BIG_ul_SearchFile(ul_Dir, _psz_FileName);
	return BIG_ul_UpdateCreateFileOptim(ul_Dir, ul_File, _psz_DirName, _psz_FileName, _p_Callback, _b_AskKey);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG BIG_ul_UpdateCreateFileOptim
(
	BIG_INDEX			ul_Dir,
	BIG_INDEX			ul_NewIndex,
	char				*_psz_DirName,
	char				*_psz_FileName,
	BIG_tdpfnv_Callback _p_Callback,
	BOOL				_b_AskKey
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Memo, ul_NewPos;
	ULONG		ul_Pos, ul_LengthOnDisk;
	BOOL		b_AtEnd;
	ULONG		ul_LOAPos, ul_LOAAddr;
	char		*psz_RealName;
	char		asz_Msg[512];
	BOOL		b_Create;
	char		asz_Path[BIG_C_MaxLenPath];
	BOOL		b_Loaded;
	ULONG		ul_Res,r;
	BOOL		bKeepCurrent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Loaded = FALSE;
	b_Create = FALSE;
	ul_LOAPos = BIG_C_InvalidIndex;
	bKeepCurrent = FALSE;

	/*
	 * First ask import module if we have the right to simply import the external file
	 * like this
	 */
	if(BIG_gpsz_RealName)
	{
		psz_RealName = L_strdup(BIG_gpsz_RealName);

		if(!IMP_b_SpecialImportFile(_psz_DirName, _psz_FileName, psz_RealName))
		{
			if(BIG_gpsz_RealName) *BIG_gpsz_RealName = 0;
			BIG_gb_ImportOne = TRUE;
			L_free(psz_RealName);
			return BIG_C_InvalidIndex;
		}

		L_free(psz_RealName);
	}

	/* Call the callback and get length of datas to save */
	if(_p_Callback) _p_Callback(ul_Dir, ul_NewIndex);

	/* No update if no buffer */
	if(!BIG_gp_Buffer && !BIG_gp_Buffer1)
	{
		if(ul_NewIndex != BIG_C_InvalidIndex)
		{
			BIG_FileChanged(ul_NewIndex) = (BIG_FileChanged(ul_NewIndex) & EDI_FHC_Loaded) | EDI_FHC_Touch;
		}

		BIG_gul_Length = 0;
		return BIG_C_InvalidIndex;
	}

	/*
	 * If we don't need to request a key, that's for a merge. We search the key in the
	 * current bigfile. If it exists, we update the corresponding file, else we create
	 * the source dir and will create the file.
	 */
	if(!_b_AskKey)
	{
recom:
		ul_NewIndex = BIG_ul_SearchKeyToFat(BIG_gul_GlobalKey);
		if(ul_NewIndex == BIG_C_InvalidIndex)
		{
			if(BIG_gb_CheckSameName)
			{
				ul_NewIndex = BIG_ul_SearchFileExt(_psz_DirName, _psz_FileName);
				if(ul_NewIndex != BIG_C_InvalidIndex)
				{
					char szMessage[ 512 ];
					sprintf( szMessage, "File \"%s\" already exists locally with another key in folder \"%s\"!\n\n        Local file key: %08X\n        Perforce file key: %08X\n\nOverwrite local file with Perforce version?", _psz_FileName, _psz_DirName, BIG_FileKey(ul_NewIndex), BIG_gul_GlobalKey );
					if( MessageBox( NULL, szMessage, "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
					{
					sprintf
					(
						asz_Msg,
						"%s/%s exists with another key (%x). Key is changed to (%x).",
						_psz_DirName,
						_psz_FileName,
						BIG_FileKey(ul_NewIndex),
						BIG_gul_GlobalKey
					);
					ERR_X_Warning(0, asz_Msg, NULL);
					BIG_DeleteKeyToFat(BIG_FileKey(ul_NewIndex));
					BIG_FileKey(ul_NewIndex) = BIG_gul_GlobalKey;
					BIG_InsertKeyToFat(BIG_gul_GlobalKey, ul_NewIndex);
				}
					else
					{
						sprintf
							(
							asz_Msg,
							"%s/%s exists with another key (%x). Key is *NOT* changed to (%x) and file is *NOT* replaced.",
							_psz_DirName,
							_psz_FileName,
							BIG_FileKey(ul_NewIndex),
							BIG_gul_GlobalKey
							);
						ERR_X_Warning(0, asz_Msg, NULL);
						bKeepCurrent = TRUE;
					}
				}
				else if(BIG_gul_GlobalKey == BIG_C_InvalidKey) _b_AskKey = TRUE;
				ul_Dir = BIG_ul_SearchDir(_psz_DirName);
				if(ul_Dir == BIG_C_InvalidIndex) ul_Dir = BIG_ul_CreateDir(_psz_DirName);
			}
			else
			{
				ul_Dir = BIG_ul_SearchDir(_psz_DirName);
				if(ul_Dir == BIG_C_InvalidIndex) ul_Dir = BIG_ul_CreateDir(_psz_DirName);
			}
		}
		else
		{
			/* The directory is not the same ? */
			BIG_ComputeFullName(BIG_ParentFile(ul_NewIndex), asz_Path);
			if(L_strcmpi(asz_Path, _psz_DirName))
			{
				//sprintf(asz_Msg, "File has been moved from %s to %s", asz_Path, _psz_DirName);
				//ERR_X_Warning(0, asz_Msg, BIG_NameFile(ul_NewIndex));
				BIG_DelFile(asz_Path, BIG_NameFile(ul_NewIndex));
				goto recom;
			}

			/* For a merge, no update if date is equal or older */
			if(!BIG_gb_IgnoreRecent && (BIG_gx_GlobalTime <= BIG_TimeFile(ul_NewIndex)))
			{
				BIG_FileChanged(ul_NewIndex) = (BIG_FileChanged(ul_NewIndex) & EDI_FHC_Loaded) | EDI_FHC_Touch;
				if(BIG_gb_WarningTime && (BIG_gx_GlobalTime < BIG_TimeFile(ul_NewIndex)))
				{
					ERR_X_Warning(0, "No update cause of time for file :", BIG_NameFile(ul_NewIndex));
				}
				return ul_NewIndex;
			}

			ul_Dir = BIG_ParentFile(ul_NewIndex);

			/* Rename file with source name */
			L_strcpy(BIG_NameFile(ul_NewIndex), BIG_gpsz_GlobalName);
		}
	}

	if( bKeepCurrent )
		return BIG_gul_GlobalKey;

	/* Update or create ?. */
	if(ul_NewIndex != BIG_C_InvalidIndex)
	{
		/* Get current position in bigfile */
		ul_Memo = BIG_C_InvalidIndex;
		ul_LOAPos = ul_Pos = BIG_PosFile(ul_NewIndex);
		ul_LengthOnDisk = BIG_LengthDiskFile(ul_NewIndex);

		/* No need to get a new position ? */
		if(ul_LengthOnDisk < BIG_gul_Length)
		{
			/* If file is currently loaded, delete pos/address association cause pause will change */
			ul_Res = LOA_ul_SearchAddress(ul_NewIndex);
			if(ul_Res != BIG_C_InvalidIndex)
			{
				b_Loaded = TRUE;
				LOA_DeleteAddress((void *) ul_Res);
			}

			ul_Pos = BIG_ul_GetFreePos(BIG_gul_Length, &ul_LengthOnDisk, &ul_NewPos);
			if(ul_Pos != BIG_C_InvalidIndex) 
			{
				BIG_UpdateOneFileInFat(ul_NewPos);
			}
		}

		goto l_Write;
	}

	/* Add a new file in table */
_Try_
	ul_NewIndex = BIG_ul_AddFileInTable(BIG_gul_Length, &ul_Pos, &ul_LengthOnDisk, _b_AskKey);
	b_Create = TRUE;

_Catch_
_EndThrow_
	/* Copy current key if we do not want another one */
	if(!_b_AskKey) BIG_FileKey(ul_NewIndex) = BIG_gul_GlobalKey;

	/* Update parent directory and file links */
	ul_Memo = BIG_FirstFile(ul_Dir);
	BIG_FirstFile(ul_Dir) = ul_NewIndex;
	BIG_NextFile(ul_NewIndex) = ul_Memo;
	BIG_PrevFile(ul_NewIndex) = BIG_C_InvalidIndex;
	BIG_ParentFile(ul_NewIndex) = ul_Dir;
	if(ul_Memo != BIG_C_InvalidIndex) BIG_PrevFile(ul_Memo) = ul_NewIndex;

	/* "Big Name" of the new file */
	if(L_strlen(_psz_FileName) >= BIG_C_MaxLenName)
	{
		L_memcpy(BIG_NameFile(ul_NewIndex), _psz_FileName, BIG_C_MaxLenName - 1);
		BIG_NameFile(ul_NewIndex)[BIG_C_MaxLenName - 1] = 0;
	}
	else
	{
		L_strcpy(BIG_NameFile(ul_NewIndex), _psz_FileName);
	}

l_Write:

	/* Update length */
	if(ul_LengthOnDisk < 4) ul_LengthOnDisk = 4;
	BIG_LengthDiskFile(ul_NewIndex) = ul_LengthOnDisk;

	/* Seek to pos */
	if(ul_Pos == BIG_C_InvalidIndex)
	{
		r=L_fseek(BIG_Handle(), 0, L_SEEK_END);
		ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);
		ul_Pos = L_ftell(BIG_Handle());
		b_AtEnd = TRUE;
	}	
	else
	{
		r=L_fseek(BIG_Handle(), ul_Pos, L_SEEK_SET);
		ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);
		b_AtEnd = FALSE;
	}

	if((int) ul_Pos < 0)
	{
		fpos_t pos;
		int test;

		test = fgetpos(BIG_Handle(), &pos);

		ul_Pos = ul_Pos;
	}

	/* Other updates */
	BIG_PosFile(ul_NewIndex) = ul_Pos;

	/* If file was loaded, create new pos association */
	if(b_Loaded) LOA_AddAddress(ul_NewIndex, (void *) ul_Res);

	BIG_FileChanged(ul_NewIndex) = (BIG_FileChanged(ul_NewIndex) & EDI_FHC_Loaded) | EDI_FHC_AddUpdate | EDI_FHC_Touch;
	if(b_Create) BIG_FileChanged(ul_NewIndex) |= EDI_FHC_Create;
	BIG_TimeFile(ul_NewIndex) = BIG_gx_GlobalTime;
	if ( b_Create )
	{
		BIG_P4RevisionClient(ul_NewIndex) = BIG_gx_GlobalClientRev;
		BIG_P4Time(ul_NewIndex) = 0xFFFFFFFF;
	}

	/* Write length at the beginning of the file */
	r=BIG_fwrite(&BIG_gul_Length, sizeof(ULONG), BIG_Handle());
	ERR_X_Error(r == 1, L_ERR_Csz_FWrite, NULL);

	/* Write file */
	if(b_AtEnd)
	{
		if(BIG_gp_Buffer1)
		{
			r=BIG_fwrite(BIG_gp_Buffer1, BIG_gul_Length1, BIG_Handle());
			ERR_X_Error(r == 1, L_ERR_Csz_FWrite, NULL);
			r=BIG_fwrite(BIG_gp_Buffer2, BIG_gul_Length2, BIG_Handle());
			ERR_X_Error(r == 1, L_ERR_Csz_FWrite, NULL);
			r=BIG_fwrite(BIG_gp_Buffer1, ul_LengthOnDisk - (BIG_gul_Length1 + BIG_gul_Length2), BIG_Handle());
			ERR_X_Error(r == 1, L_ERR_Csz_FWrite, NULL);
		}
		else
		{
			r=BIG_fwrite(BIG_gp_Buffer, ul_LengthOnDisk, BIG_Handle());
			ERR_X_Error(r == 1, L_ERR_Csz_FWrite, NULL);
		}
	}
	else if(BIG_gul_Length != 0)
	{
		if(BIG_gp_Buffer1)
		{
			r=BIG_fwrite(BIG_gp_Buffer1, BIG_gul_Length1, BIG_Handle());
			ERR_X_Error(r == 1, L_ERR_Csz_FWrite, NULL);
			r=BIG_fwrite(BIG_gp_Buffer2, BIG_gul_Length2, BIG_Handle());
			ERR_X_Error(r == 1, L_ERR_Csz_FWrite, NULL);
		}
		else
		{
			r=BIG_fwrite(BIG_gp_Buffer, BIG_gul_Length, BIG_Handle());
			ERR_X_Error(r == 1, L_ERR_Csz_FWrite, NULL);
		}
	}

	BIG_gp_Buffer1 = NULL;

	/* Update Header */
	BIG_WriteHeader();

	/* Update parent directory */
	BIG_UpdateOneDirInFat(ul_Dir);

	/* Update file */
	BIG_UpdateOneFileInFat(ul_NewIndex);

	/* Update tables */
	if(BIG_gst.dst_FileTable[ul_NewIndex].ul_Key != BIG_C_InvalidIndex)
		BIG_InsertKeyToFat(BIG_gst.dst_FileTable[ul_NewIndex].ul_Key, ul_NewIndex);

	/*
	 * Change LOA for an update of a file. Search if the previous position was
	 * associated with and address. If yes, delete the association and make a new one
	 * with the new position of file
	 */
	if((ul_LOAPos != BIG_C_InvalidIndex) && (ul_LOAPos != BIG_PosFile(ul_NewIndex)))
	{
		ul_LOAAddr = LOA_ul_SearchAddress(ul_LOAPos);
		if(ul_LOAAddr != BIG_C_InvalidIndex)
		{
			LOA_DeleteAddress((void *) ul_LOAAddr);
			LOA_AddAddress(ul_NewIndex, (void *) ul_LOAAddr);
		}
	}

	/* Update next file in dir */
	if(ul_Memo != BIG_C_InvalidIndex) BIG_UpdateOneFileInFat(ul_Memo);

	return ul_NewIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_DeleteFile(BIG_INDEX _ul_DirIndex, BIG_INDEX _ul_FileIndex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Memo, ul_Prev, ul_Next;
	int			*pi;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Unlink file of list */
	ul_Prev = BIG_PrevFile(_ul_FileIndex);
	ul_Next = BIG_NextFile(_ul_FileIndex);
	if(BIG_FirstFile(_ul_DirIndex) == _ul_FileIndex)
	{
		if(ul_Next != BIG_C_InvalidIndex)
		{
			BIG_PrevFile(ul_Next) = BIG_C_InvalidIndex;
			BIG_UpdateOneFileInFat(ul_Next);
		}

		BIG_FirstFile(_ul_DirIndex) = ul_Next;
		BIG_UpdateOneDirInFat(_ul_DirIndex);
	}
	else
	{
		if(ul_Prev != BIG_C_InvalidIndex)
		{
			BIG_NextFile(ul_Prev) = BIG_NextFile(_ul_FileIndex);
			BIG_UpdateOneFileInFat(ul_Prev);
		}

		if(ul_Next != BIG_C_InvalidIndex)
		{
			BIG_PrevFile(ul_Next) = ul_Prev;
			BIG_UpdateOneFileInFat(ul_Next);
		}
	}

	/* Delete the associated key in key table */
	BIG_DeleteKeyToFat(BIG_FileKey(_ul_FileIndex));

	/* Save key at the end of the name */
	pi = (int *) (BIG_NameFile(_ul_FileIndex) + BIG_C_MaxLenName - sizeof(int) - 2);
	*pi = BIG_FileKey(_ul_FileIndex);
	BIG_NameFile(_ul_FileIndex)[BIG_C_MaxLenName - 1] = 0;

	BIG_FileKey(_ul_FileIndex) = BIG_C_InvalidKey;

	/* Remember operation */
	BIG_FileChanged(_ul_FileIndex) = EDI_FHC_Delete;

	/* Link file to free file list */
	ul_Memo = BIG_FreeFile();
	BIG_FreeFile() = _ul_FileIndex;
	BIG_PrevFile(_ul_FileIndex) = BIG_C_InvalidIndex;
	BIG_NextFile(_ul_FileIndex) = ul_Memo;
	if(ul_Memo != BIG_C_InvalidIndex)
	{
		BIG_PrevFile(ul_Memo) = _ul_FileIndex;
		BIG_UpdateOneFileInFat(ul_Memo);
	}

	/* Update header */
	BIG_WriteHeader();

	/* Update fat for current file */
	BIG_UpdateOneFileInFat(_ul_FileIndex);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Basic function to create/update file with a content that is found on disk, in normal drive.

    In:     _psz_RealName   Name of file to load from disk (this will contain the content of the file).
            _psz_Path       Full path in bigfile to create/update the file.
            _psz_File       Name of file to create/update.
 =======================================================================================================================
 */
void BIG_UpdateFileFromDisk(char *_psz_RealName, char *_psz_Path, char *_psz_File)
{
	BIG_gpsz_RealName = _psz_RealName;
	BIG_ul_UpdateCreateFile(_psz_Path, _psz_File, BIG_CallbackLoadFileFromDisk, TRUE);
}

/*
 =======================================================================================================================
    Aim:    Very important function that is the base to create/update a new file with a given content.

    In:     _psz_Path   Full path name of the file to create/update.
            _psz_File   Name of file to create/update.
            _p_Buffer   Content of the file to save to bigfile.
            _ul_Size    Size of the buffer to save, in bytes.
 =======================================================================================================================
 */
void BIG_UpdateFileFromBuffer(char *_psz_Path, char *_psz_File, void *_p_Buffer, ULONG _ul_Size)
{
	BIG_gpsz_RealName = NULL;
	BIG_gp_Buffer = _p_Buffer;
	BIG_gul_Length = _ul_Size;
	L_time(&BIG_gx_GlobalTime);
	BIG_ul_UpdateCreateFile(_psz_Path, _psz_File, NULL, TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_UpdateFileFromBufferWithDate
(
	char		*_psz_Path,
	char		*_psz_File,
	void		*_p_Buffer,
	ULONG		_ul_Size,
	L_time_t	_x_Time
)
{
	BIG_gpsz_RealName = NULL;
	BIG_gp_Buffer = _p_Buffer;
	BIG_gul_Length = _ul_Size;
	BIG_gx_GlobalTime = _x_Time;
	BIG_ul_UpdateCreateFile(_psz_Path, _psz_File, NULL, TRUE);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    To delete a from of a given folder.

    In:     _psz_PathName   Name of path (full name in bigfile) where the file to delete is.
            _psz_FileName   Name of file to delete.
 =======================================================================================================================
 */
void BIG_DelFile(char *_psz_PathName, char *_psz_FileName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_FileIndex, mul_DirIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Retrieve index of parent directory */
	mul_DirIndex = BIG_ul_SearchDir(_psz_PathName);
	if(mul_DirIndex == BIG_C_InvalidIndex) return;

	/* Retrieve index of file in parent directory */
	ul_FileIndex = BIG_ul_SearchFile(mul_DirIndex, _psz_FileName);
	if(ul_FileIndex == BIG_C_InvalidIndex) return;

	/* Delete file */
	BIG_DeleteFile(mul_DirIndex, ul_FileIndex);

	/* Remember operation */
	BIG_FileChanged(ul_FileIndex) = EDI_FHC_Delete;
}

/*
 =======================================================================================================================
    Aim:    To rename a file.

    In:     _psz_NewName    New name of file (atomic part).
            _psz_PathName   Name of folder where the file to be rename is.
            _psz_OldName    Old name of the file to rename.
 =======================================================================================================================
 */
void BIG_RenFile(char *_psz_NewName, char *_psz_PathName, char *_psz_OldName)
{
	/*~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~*/

	/* Retrieve index of directory */
	ul_Index = BIG_ul_SearchFileExt(_psz_PathName, _psz_OldName);

	if(BIG_ul_SearchFileExt(_psz_PathName, _psz_NewName) != BIG_C_InvalidIndex)
	{
		char		asz_Log[512];

		sprintf(asz_Log, "A file with the name [%s] already exists in folder [%s]. Canceled", _psz_NewName, _psz_PathName);
		LINK_PrintStatusMsg(asz_Log);
	}

	ERR_X_Error
	(
		BIG_ul_SearchFileExt(_psz_PathName, _psz_NewName) == BIG_C_InvalidIndex,
		"A file with the same name already exists",
		NULL
	);

	/* Rename the file */
	if(L_strlen(_psz_NewName) >= BIG_C_MaxLenName)
	{
		L_memcpy(BIG_NameFile(ul_Index), _psz_NewName, BIG_C_MaxLenName - 1);
		BIG_NameFile(ul_Index)[BIG_C_MaxLenName - 1] = 0;
	}
	else
	{
		L_strcpy(BIG_NameFile(ul_Index), _psz_NewName);
	}

	/* Remember operation */
	L_time(&BIG_TimeFile(ul_Index));
	BIG_FileChanged(ul_Index) = (BIG_FileChanged(ul_Index) & EDI_FHC_Loaded) | EDI_FHC_Rename;

	/* Update fat */
	BIG_UpdateOneFileInFat(ul_Index);
}

/*
 =======================================================================================================================
    Aim:    To copy a file in a destination folder.

    In:     _psz_Dest   Destination folder (full path in bigfile) to copy the file to.
            _psz_Src    Source folder of the file.
            _psz_File   Name of file to copy.
 =======================================================================================================================
 */
BIG_INDEX BIG_CopyFile(char *_psz_Dest, char *_psz_Src, char *_psz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	BIG_INDEX	ul_DestPath, ul_SrcPath, ul_File;
	char		*pc_Buf;
	ULONG		ul_Length;
	char		asz_Name[BIG_C_MaxLenName];
	char		asz_Name1[BIG_C_MaxLenName];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
	ul_DestPath = BIG_ul_SearchDir(_psz_Dest);
	ERR_X_Assert(ul_DestPath != BIG_C_InvalidIndex);
	ul_SrcPath = BIG_ul_SearchDir(_psz_Src);
	ERR_X_Assert(ul_SrcPath != BIG_C_InvalidIndex);
	ul_File = BIG_ul_SearchFile(ul_SrcPath, _psz_File);
	ERR_X_Assert(ul_File != BIG_C_InvalidIndex);

	/* Check if file already exists in dest. If yes, try to rename it */
	L_strcpy(asz_Name, _psz_File);

	ul_Index = BIG_ul_SearchFile(ul_DestPath, asz_Name);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		if(L_strlen(asz_Name) == BIG_C_MaxLenName)
		{
			ERR_X_Error(ul_Index == BIG_C_InvalidIndex, ERR_BIG_Csz_CantMoveFile, _psz_File);
		}

		L_strcpy(asz_Name1, "_");
		L_strcat(asz_Name1, asz_Name);
		L_strcpy(asz_Name, asz_Name1);

		ul_Index = BIG_ul_SearchFile(ul_DestPath, asz_Name);
	}

	/* Load the file */
	pc_Buf = BIG_pc_ReadFileTmp(BIG_PosFile(ul_File), &ul_Length);

	/* No key creation for ini files */
	BIG_UpdateFileFromBuffer(_psz_Dest, asz_Name, pc_Buf, ul_Length);

	ul_Index = BIG_ul_SearchFile(ul_DestPath, asz_Name);
	return ul_Index;

_Catch_
_End_
	return BIG_C_InvalidIndex;
}

/*
 =======================================================================================================================
    Aim:    To move a file from on dir to another.

    In:     _psz_Dest   Full path (in bigfile) of destination folder.
            _psz_Src    Full path (in bigfile) of source folder.
            _psz_File   Atomic file name.
 =======================================================================================================================
 */
void BIG_MoveFile(char *_psz_Dest, char *_psz_Src, char *_psz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	BIG_INDEX	ul_DestPath, ul_SrcPath, ul_File;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
	ul_DestPath = BIG_ul_CreateDir(_psz_Dest);
	ERR_X_Assert(ul_DestPath != BIG_C_InvalidIndex);
	ul_SrcPath = BIG_ul_SearchDir(_psz_Src);
	ERR_X_Assert(ul_SrcPath != BIG_C_InvalidIndex);
	ul_File = BIG_ul_SearchFile(ul_SrcPath, _psz_File);
	ERR_X_Assert(ul_File != BIG_C_InvalidIndex);

	if(ul_DestPath == ul_SrcPath) _Return_(;);

	/* Check if file already exists in dest */
	ul_Index = BIG_ul_SearchFile(ul_DestPath, BIG_NameFile(ul_File));
	ERR_X_Error(ul_Index == BIG_C_InvalidIndex, ERR_BIG_Csz_CantMoveFile, BIG_NameFile(ul_File));

	/* Unlink file from current path */
	ul_Index = BIG_NextFile(ul_File);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		BIG_PrevFile(ul_Index) = BIG_PrevFile(ul_File);
		BIG_UpdateOneFileInFat(ul_Index);
	}

	ul_Index = BIG_PrevFile(ul_File);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		BIG_NextFile(ul_Index) = BIG_NextFile(ul_File);
		BIG_UpdateOneFileInFat(ul_Index);
	}
	else
	{
		BIG_FirstFile(ul_SrcPath) = BIG_NextFile(ul_File);
		BIG_UpdateOneDirInFat(ul_SrcPath);
	}

	/* Add file at the first position of the new path */
	ul_Index = BIG_FirstFile(ul_DestPath);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		BIG_PrevFile(ul_Index) = ul_File;
		BIG_UpdateOneFileInFat(ul_Index);
	}

	BIG_FirstFile(ul_DestPath) = ul_File;
	BIG_NextFile(ul_File) = ul_Index;
	BIG_ParentFile(ul_File) = ul_DestPath;
	BIG_PrevFile(ul_File) = BIG_C_InvalidIndex;

	/* Update fat */
	BIG_UpdateOneDirInFat(ul_DestPath);
	BIG_UpdateOneFileInFat(ul_File);

	/* Set as changed */
	BIG_DirChanged(ul_DestPath) = EDI_FHC_Content;
	BIG_DirChanged(ul_SrcPath) = EDI_FHC_Content;
	BIG_FileChanged(ul_File) = (BIG_FileChanged(ul_File) & EDI_FHC_Loaded) | EDI_FHC_Move;

_Catch_
_End_
}

/*
 =======================================================================================================================
    Aim:    To create an empty file.

    In:     _psz_Path   Full path (in bigfile) of folder.
            _psz_File   Atomic file name.
 =======================================================================================================================
 */
BIG_INDEX BIG_ul_CreateFile(char *_psz_Path, char *_psz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Path, ul_File;
	LONG		l_Dum;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Check if dir is correct */
	ul_Path = BIG_ul_CreateDir(_psz_Path);
	ERR_X_Assert(ul_Path != BIG_C_InvalidIndex);

	/* Create the file with a dummy buffer */
	l_Dum = 0x20202020;
	BIG_gp_Buffer = &l_Dum;
	BIG_gul_Length = 0;
	L_time(&BIG_gx_GlobalTime);
	ul_File = BIG_ul_UpdateCreateFile(_psz_Path, _psz_File, NULL, TRUE);

	/* Set as changed */
	BIG_DirChanged(ul_Path) = EDI_FHC_Content;

	return ul_File;
}

/*
 =======================================================================================================================
    Aim:    To build a new name.

    In:     _psz_Path   dir to look in.
            _psz_File   Atomic file name.

    Note:   the new name is tha atomic name with eventually a number after if atomic name is found
            so be sure that name is a variable  with enough character to store a number after name
 =======================================================================================================================
 */
void BIG_BuildNewName( char *_psz_Path, char *_psz_File, char *_sz_Ext )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX	ul_Path, ul_File;
    char        sz_Ext[16];
    char        *sz_Number;
    int         i_Number;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Check if dir is correct */
	ul_Path = BIG_ul_CreateDir(_psz_Path);
	ERR_X_Assert(ul_Path != BIG_C_InvalidIndex);

    sz_Number = L_strrchr( _psz_File, '.' );
    if (sz_Number)
    {
        if ( !_sz_Ext )
        {
            L_strcpy( sz_Ext, sz_Number );
            _sz_Ext = sz_Ext;
        }
        *sz_Number = 0;
    }
    else
    {
        sz_Number = _psz_File + strlen( _psz_File );
        if (!_sz_Ext)
        {
            *sz_Ext = 0;
            _sz_Ext = sz_Ext;
        }
    }

    L_strcat( sz_Number, _sz_Ext );
    i_Number = 0;
    while( 1 )
    {
        ul_File = BIG_ul_SearchFileExt(_psz_Path, _psz_File);
        if (ul_File == BIG_C_InvalidIndex) return;

	    sprintf( sz_Number, "%i", i_Number++);
	    L_strcat( sz_Number, _sz_Ext);
    };
}

#endif /* ACTIVE_EDITORS */

/*$T BIGcheck.c GC! 1.097 04/12/02 10:14:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGio.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGerrid.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "LINks/LINKmsg.h"
#include "LINks/LINKtoed.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"

#include "INOut/INO.h"

#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#endif
#ifdef ACTIVE_EDITORS
BOOL			BIG_gb_CanClean = FALSE;
BAS_tdst_barray BIG_gst_DeletedFiles;
BAS_tdst_barray BIG_gst_DeletedDirs;
BAS_tdst_barray BIG_gst_TouchedFiles;
BAS_tdst_barray BIG_gst_TouchedDirs;
extern int		WOR_gi_CurrentConsole;
BOOL			BIG_gb_CleanFatEmpty = FALSE;
extern BOOL    EDI_gb_NoVerbose;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *BIG_FileName(BIG_INDEX _ul_Index)
{
	if(_ul_Index == BIG_C_InvalidIndex) return "NULL";
	return BIG_NameFile(_ul_Index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *BIG_DirName(BIG_INDEX _ul_Index)
{
	if(_ul_Index == BIG_C_InvalidIndex) return "NULL";
	return BIG_NameDir(_ul_Index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BIG_CheckDeleted(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index, ul_Next;
	BOOL		b_Res;
	char		asz_Msg[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* File list */
	BAS_binit(&BIG_gst_DeletedFiles, 200);
	b_Res = TRUE;
	ul_Index = BIG_gst.st_ToSave.ul_FirstFreeFile;
	while(ul_Index != BIG_C_InvalidIndex)
	{
		ul_Next = BAS_bsearch(ul_Index, &BIG_gst_DeletedFiles);
		if(ul_Next != -1)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "ERROR : File list is cycling");
			LINK_PrintStatusMsg(asz_Msg);
			return FALSE;
		}

		BAS_binsert(ul_Index, ul_Index, &BIG_gst_DeletedFiles);
		ul_Index = BIG_NextFile(ul_Index);
	}

	/* Dir list */
	BAS_binit(&BIG_gst_DeletedDirs, 200);
	ul_Index = BIG_gst.st_ToSave.ul_FirstFreeDir;
	while(ul_Index != BIG_C_InvalidIndex)
	{
		ul_Next = BAS_bsearch(ul_Index, &BIG_gst_DeletedDirs);
		if(ul_Next != -1)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "ERROR : Dir list is cycling");
			LINK_PrintStatusMsg(asz_Msg);
			return FALSE;
		}

		BAS_binsert(ul_Index, ul_Index, &BIG_gst_DeletedDirs);
		ul_Index = BIG_NextDir(ul_Index);
	}

	return b_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BIG_CheckHierarchy(BIG_INDEX ul_Root)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX		ul_File, ul_Next;
	char			asz_Msg[1024];
	char			asz_Path[1024];
	char			asz_Path1[1024];
	BAS_tdst_barray st_Files;
	BAS_tdst_barray st_Dirs;
	BOOL			b_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Res = TRUE;
	BAS_binit(&st_Dirs, 200);

	BIG_ComputeFullName(ul_Root, asz_Path);
	L_strcat(asz_Path, "/");
	if(L_strlen(asz_Path) >= BIG_C_MaxLenPath)
	{
		sprintf(asz_Path, "ERROR : Total dir path name is too long !!!!!", asz_Path);
		LINK_gul_ColorTxt = 0x000000FF;
		LINK_PrintStatusMsg(asz_Path);
		b_Res = FALSE;
	}

	/* Check first */
	ul_File = BIG_SubDir(ul_Root);
	if(ul_File != BIG_C_InvalidIndex)
	{
		if(BIG_PrevDir(ul_File) != BIG_C_InvalidIndex)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "ERROR : First dir of %s has not a good previous link", BIG_NameDir(ul_Root));
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;

			if(BIG_gb_CanClean)
			{
				LINK_gul_ColorTxt = 0x00FF0000;
				sprintf(asz_Msg, "=> Break link", BIG_DirName(ul_Root));
				LINK_PrintStatusMsg(asz_Msg);
				if(BIG_NextDir(BIG_PrevDir(ul_File)) == ul_File)
				{
					BIG_NextDir(BIG_PrevDir(ul_File)) = BIG_C_InvalidIndex;
					BIG_UpdateOneDirInFat(BIG_PrevDir(ul_File));
				}

				BIG_PrevDir(ul_File) = BIG_C_InvalidIndex;
				BIG_UpdateOneDirInFat(ul_File);
			}
		}
	}

	while(ul_File != BIG_C_InvalidIndex)
	{
		/* Dir already seen ? */
		ul_Next = BAS_bsearch(ul_File, &BIG_gst_TouchedDirs);
		if(ul_Next != -1)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "ERROR : Dir %s is in more that one list", BIG_NameDir(ul_File));
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;
		}

		BAS_binsert(ul_File, ul_File, &BIG_gst_TouchedDirs);

		/* Cycling ? */
		ul_Next = BAS_bsearch(ul_File, &st_Dirs);
		if(ul_Next != -1)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "ERROR : Dir list is cycling in (%s)", BIG_NameDir(ul_Root));
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;
			break;
		}

		BAS_binsert(ul_File, ul_File, &st_Dirs);

		/* Correct parent */
		if(BIG_ParentDir(ul_File) != ul_Root)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf
			(
				asz_Msg,
				"ERROR : Dir %s has not the good parent (%s instead of %s)",
				BIG_DirName(ul_File),
				BIG_DirName(BIG_ParentDir(ul_File)),
				BIG_DirName(ul_Root)
			);
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;

			if(BIG_gb_CanClean)
			{
				LINK_gul_ColorTxt = 0x00FF0000;
				sprintf(asz_Msg, "=> Restoring parent to dir %s", BIG_DirName(ul_Root));
				LINK_PrintStatusMsg(asz_Msg);
				BIG_ParentDir(ul_File) = ul_Root;
				BIG_UpdateOneDirInFat(ul_File);
			}
		}

		/* Is dir is deleted ? */
		ul_Next = BAS_bsearch(ul_File, &BIG_gst_DeletedDirs);
		if(ul_Next != -1)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "ERROR : Dir is present in deleted list (%s)", BIG_NameDir(ul_File));
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;

			if(BIG_gb_CanClean)
			{
				if(BIG_PrevDir(ul_File) == BIG_C_InvalidIndex)
				{
					LINK_gul_ColorTxt = 0x00FF0000;
					sprintf(asz_Msg, "=> Remove dir from dir %s", BIG_DirName(ul_Root));
					LINK_PrintStatusMsg(asz_Msg);
					BIG_SubDir(ul_Root) = BIG_C_InvalidIndex;
					BIG_UpdateOneDirInFat(ul_Root);
				}
			}
		}

		if(!BIG_CheckHierarchy(ul_File)) b_Res = FALSE;
		ul_File = BIG_NextDir(ul_File);
	}

	BAS_bfree(&st_Dirs);

	BAS_binit(&st_Files, 200);
	ul_File = BIG_FirstFile(ul_Root);

	/* Check first */
	if(ul_File != BIG_C_InvalidIndex)
	{
		if(ul_File > BIG_MaxFile())
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "ERROR : A dir is referenced a bad file", BIG_DirName(ul_Root));
			LINK_PrintStatusMsg(asz_Msg);
			BIG_FirstFile(ul_Root) = BIG_C_InvalidIndex;
			BIG_UpdateOneDirInFat(ul_Root);
			ul_File = BIG_C_InvalidIndex;
			LINK_gul_ColorTxt = 0x00FF0000;
			LINK_PrintStatusMsg("I'm forced to clean...");
			b_Res = FALSE;
		}

		if((ul_File != BIG_C_InvalidIndex) && (BIG_PrevFile(ul_File) != BIG_C_InvalidIndex))
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf
			(
				asz_Msg,
				"ERROR : First file of %s has not a good previous link (%s)",
				BIG_DirName(ul_Root),
				BIG_FileName(BIG_PrevFile(ul_File))
			);
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;

			if(BIG_gb_CanClean)
			{
				LINK_gul_ColorTxt = 0x00FF0000;
				sprintf(asz_Msg, "=> Break link", BIG_DirName(ul_Root));
				LINK_PrintStatusMsg(asz_Msg);
				if(BIG_NextFile(BIG_PrevFile(ul_File)) == ul_File)
				{
					BIG_NextFile(BIG_PrevFile(ul_File)) = BIG_C_InvalidIndex;
					BIG_UpdateOneFileInFat(BIG_PrevFile(ul_File));
				}

				BIG_PrevFile(ul_File) = BIG_C_InvalidIndex;
				BIG_UpdateOneFileInFat(ul_File);
			}
		}
	}

	while(ul_File != BIG_C_InvalidIndex)
	{
		/* File already seen ? */
		ul_Next = BAS_bsearch(ul_File, &BIG_gst_TouchedFiles);
		if(ul_Next != -1)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "ERROR : File %s is in more that one list", BIG_NameFile(ul_File));
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;

			if(BIG_gb_CanClean)
			{
				LINK_gul_ColorTxt = 0x00FF0000;
				sprintf(asz_Msg, "=> Delete reference in %s", BIG_DirName(ul_Root));
				LINK_PrintStatusMsg(asz_Msg);
				BIG_FirstFile(ul_Root) = BIG_C_InvalidIndex;
				BIG_UpdateOneDirInFat(ul_Root);
			}
		}

		BAS_binsert(ul_File, ul_File, &BIG_gst_TouchedFiles);

		/* Cycling ? */
		ul_Next = BAS_bsearch(ul_File, &st_Files);
		if(ul_Next != -1)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "ERROR : File list is cycling in (%s)", BIG_DirName(ul_Root));
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;
			break;
		}

		BAS_binsert(ul_File, ul_File, &st_Files);

		/* Correct parent */
		if(BIG_ParentFile(ul_File) != ul_Root)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf
			(
				asz_Msg,
				"ERROR : File %s has not the good parent (%s instead of %s)",
				BIG_FileName(ul_File),
				BIG_DirName(BIG_ParentFile(ul_File)),
				BIG_DirName(ul_Root)
			);
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;

			if(BIG_gb_CanClean)
			{
				LINK_gul_ColorTxt = 0x00FF0000;
				sprintf(asz_Msg, "=> Restoring parent to dir %s", BIG_DirName(ul_Root));
				LINK_PrintStatusMsg(asz_Msg);
				BIG_ParentFile(ul_File) = ul_Root;
				BIG_UpdateOneFileInFat(ul_File);
			}
		}

		/* Is file deleted ? */
		ul_Next = BAS_bsearch(ul_File, &BIG_gst_DeletedFiles);
		if(ul_Next != -1)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "ERROR : File is present in deleted list (%s)", BIG_FileName(ul_File));
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;

			if(BIG_gb_CanClean)
			{
				if(BIG_PrevFile(ul_File) == BIG_C_InvalidIndex)
				{
					LINK_gul_ColorTxt = 0x00FF0000;
					sprintf(asz_Msg, "=> Remove file from dir %s", BIG_DirName(ul_Root));
					LINK_PrintStatusMsg(asz_Msg);
					BIG_FirstFile(ul_Root) = BIG_C_InvalidIndex;
					BIG_UpdateOneDirInFat(ul_Root);
				}
			}
		}

		ul_File = BIG_NextFile(ul_File);
	}

	BAS_bfree(&st_Files);

	ul_File = BIG_FirstFile(ul_Root);
	while(ul_File != BIG_C_InvalidIndex)
	{
		L_strcpy(asz_Path1, asz_Path);
		L_strcat(asz_Path1, BIG_NameFile(ul_File));
		ul_File = BIG_NextFile(ul_File);

		if(L_strlen(asz_Path1) >= BIG_C_MaxLenPath)
		{
			sprintf(asz_Path1, "ERROR : Total dir path name + file is too long !!!!!", asz_Path1);
			LINK_gul_ColorTxt = 0x000000FF;
			LINK_PrintStatusMsg(asz_Path1);
			b_Res = FALSE;
		}
	}

	return b_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BIG_CheckKeys(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	BAS_tdst_barray st_Key;
	ULONG			i, j;
	BOOL			b_Res;
	char			az[512];
	/*~~~~~~~~~~~~~~~~~~~~*/

	BAS_binit(&st_Key, 200);
	b_Res = TRUE;
	for(i = 0; i < BIG_gst.st_ToSave.ul_MaxFile; i++)
	{
		if(BIG_FileKey(i) != BIG_C_InvalidKey)
		{
			if((j = BAS_bsearch(BIG_FileKey(i), &st_Key)) != BIG_C_InvalidIndex)
			{
				LINK_gul_ColorTxt = 0x000000FF;
				sprintf(az, "Two files with the same key (%s, %s)", BIG_NameFile(i), BIG_NameFile(j));
				LINK_PrintStatusMsg(az);

				/* b_Res = FALSE; */
			}
		}

		BAS_binsert(BIG_FileKey(i), i, &st_Key);
	}

	BAS_bfree(&st_Key);
	return b_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BIG_CheckLinks(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BAS_tdst_barray st_PosFile;
	BAS_tdst_barray st_Pos;
	BAS_tdst_barray st_PosDir;
	ULONG			i;
	BIG_INDEX		ul_Next, ul_Next1;
	char			asz_Msg[512];
	BOOL			b_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

recom:
	b_Res = TRUE;

	BAS_binit(&st_Pos, 200);
	BAS_binit(&st_PosFile, 200);
	BAS_binit(&st_PosDir, 200);

	if(BIG_gst.st_ToSave.ul_FirstFreeFile != BIG_C_InvalidIndex)
		BAS_binsert(BIG_gst.st_ToSave.ul_FirstFreeFile, BIG_gst.st_ToSave.ul_FirstFreeFile, &st_PosFile);
	if(BIG_gst.st_ToSave.ul_FirstFreeDir != BIG_C_InvalidIndex)
		BAS_binsert(BIG_gst.st_ToSave.ul_FirstFreeDir, BIG_gst.st_ToSave.ul_FirstFreeDir, &st_PosDir);
	BAS_binsert(BIG_Root(), BIG_Root(), &st_PosDir);

	/* Check maxfiles */
	if(BIG_gst.st_ToSave.ul_MaxFile > BIG_gst.dst_FatTable[BIG_gst.st_ToSave.ul_NumFat - 1].ul_LastIndex)
	{
		LINK_gul_ColorTxt = 0x000000FF;
		sprintf(asz_Msg, "ERROR : Number of files is incorrect in header");
		LINK_PrintStatusMsg(asz_Msg);
		BIG_gst.st_ToSave.ul_MaxFile = BIG_gst.dst_FatTable[BIG_gst.st_ToSave.ul_NumFat - 1].ul_LastIndex;
		BIG_WriteHeader();
		LINK_gul_ColorTxt = 0x00FF0000;
		LINK_PrintStatusMsg("I'm forced to clean...");
		return FALSE;
	}

	/* File links */
	for(i = 0; i < BIG_gst.st_ToSave.ul_MaxFile; i++)
	{
		ul_Next = BIG_gst.dst_FileTableExt[i].st_ToSave.ul_Next;
		if(ul_Next != BIG_C_InvalidIndex)
		{
			BAS_binsert(ul_Next, ul_Next, &st_PosFile);
			ul_Next1 = BIG_gst.dst_FileTableExt[ul_Next].st_ToSave.ul_Prev;
			if(ul_Next1 != i)
			{
				sprintf
				(
					asz_Msg,
					"ERROR : File link corrupt (%s next = %s prev = %s)",
					BIG_FileName(i),
					BIG_FileName(ul_Next),
					BIG_FileName(ul_Next1)
				);
				LINK_PrintStatusMsg(asz_Msg);
				b_Res = FALSE;

				if(BIG_gb_CanClean)
				{
					LINK_gul_ColorTxt = 0x00FF0000;
					sprintf(asz_Msg, "=> Break next link of %s", BIG_FileName(ul_Next1));
					LINK_PrintStatusMsg(asz_Msg);
					BIG_NextFile(ul_Next1) = BIG_C_InvalidIndex;
					if(ul_Next1 != BIG_C_InvalidIndex)
					{
						BIG_UpdateOneFileInFat(ul_Next1);
						sprintf(asz_Msg, "=> Restoring prev link of %s", BIG_FileName(ul_Next));
						LINK_PrintStatusMsg(asz_Msg);
					}

					BIG_PrevFile(ul_Next) = i;
					BIG_UpdateOneFileInFat(ul_Next);
					LINK_gul_ColorTxt = 0x000000FF;
				}
			}
		}

		ul_Next = BIG_gst.dst_FileTableExt[i].st_ToSave.ul_Prev;
		if(ul_Next != BIG_C_InvalidIndex)
		{
			ul_Next1 = BIG_gst.dst_FileTableExt[ul_Next].st_ToSave.ul_Next;
			if(ul_Next1 != i)
			{
				sprintf
				(
					asz_Msg,
					"ERROR : File link corrupt (%s prev = %s next = %s)",
					BIG_FileName(i),
					BIG_FileName(ul_Next),
					BIG_FileName(ul_Next1)
				);
				LINK_PrintStatusMsg(asz_Msg);
				b_Res = FALSE;

				if(BIG_gb_CanClean)
				{
					LINK_gul_ColorTxt = 0x00FF0000;
					sprintf(asz_Msg, "=> Break prev link of %s", BIG_FileName(i));
					LINK_PrintStatusMsg(asz_Msg);
					BIG_PrevFile(i) = BIG_C_InvalidIndex;
					BIG_UpdateOneFileInFat(i);
					LINK_gul_ColorTxt = 0x000000FF;
				}
			}
		}

		/* Check position (only one position) */
		if(BIG_gst.dst_FileTableExt[i].st_ToSave.ul_LengthOnDisk)
		{
			ul_Next = BAS_bsearch(BIG_gst.dst_FileTable[i].ul_Pos, &st_Pos);
			if(ul_Next != -1)
			{
				sprintf
				(
					asz_Msg,
					"ERROR : Two files with the same position (%s, %s)",
					BIG_FileName(ul_Next),
					BIG_FileName(i)
				);
				LINK_PrintStatusMsg(asz_Msg);
				b_Res = FALSE;

				if(BIG_gb_CanClean)
				{
					LINK_gul_ColorTxt = 0x00FF0000;
					sprintf(asz_Msg, "=> Set file length to 0 of %s", BIG_FileName(i));
					LINK_PrintStatusMsg(asz_Msg);
					BIG_LengthDiskFile(i) = 0;
					BIG_UpdateOneFileInFat(i);
					LINK_gul_ColorTxt = 0x000000FF;
				}
			}
			else
			{
				BAS_binsert(BIG_gst.dst_FileTable[i].ul_Pos, i, &st_Pos);
			}
		}
	}

	/* Dir links */
	for(i = 0; i < BIG_gst.st_ToSave.ul_MaxDir; i++)
	{
		if(BIG_FirstFile(i) != BIG_C_InvalidIndex) BAS_binsert(BIG_FirstFile(i), BIG_FirstFile(i), &st_PosFile);
		if(BIG_SubDir(i) != BIG_C_InvalidIndex) BAS_binsert(BIG_SubDir(i), BIG_SubDir(i), &st_PosDir);

		ul_Next = BIG_gst.dst_DirTable[i].st_ToSave.ul_Next;
		if(ul_Next != BIG_C_InvalidIndex)
		{
			BAS_binsert(ul_Next, ul_Next, &st_PosDir);
			ul_Next1 = BIG_gst.dst_DirTable[ul_Next].st_ToSave.ul_Prev;
			if(ul_Next1 != i)
			{
				sprintf
				(
					asz_Msg,
					"ERROR : Dir link corrupt (%s => %s <= %s)",
					BIG_DirName(i),
					BIG_DirName(ul_Next),
					BIG_DirName(ul_Next1)
				);
				LINK_PrintStatusMsg(asz_Msg);
				b_Res = FALSE;

				if(BIG_gb_CanClean)
				{
					LINK_gul_ColorTxt = 0x00FF0000;
					sprintf(asz_Msg, "=> Breaking...");
					LINK_PrintStatusMsg(asz_Msg);
					BIG_gst.dst_DirTable[i].st_ToSave.ul_Next = BIG_C_InvalidIndex;
					BIG_gst.dst_DirTable[ul_Next].st_ToSave.ul_Prev = BIG_C_InvalidIndex;
					BIG_UpdateOneDirInFat(ul_Next);
					LINK_gul_ColorTxt = 0x000000FF;
				}
			}
		}
	}

	/* Never referenced ? */
	for(i = 0; i < BIG_gst.st_ToSave.ul_MaxFile; i++)
	{
		if(BAS_bsearch(i, &st_PosFile) == -1)
		{
			sprintf(asz_Msg, "ERROR : File is never referenced (%s)", BIG_FileName(i));
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;

			if(BIG_gb_CanClean)
			{
				LINK_gul_ColorTxt = 0x00FF0000;
				LINK_PrintStatusMsg("=> Moving to ROOT");
				ul_Next = BIG_FirstFile(BIG_Root());
				BIG_FirstFile(BIG_Root()) = i;
				if(ul_Next != BIG_C_InvalidIndex) BIG_PrevFile(ul_Next) = i;
				BIG_NextFile(i) = ul_Next;
				BIG_PrevFile(i) = BIG_C_InvalidIndex;
				BIG_ParentFile(i) = BIG_Root();
				BIG_UpdateOneFileInFat(i);
				if(ul_Next != BIG_C_InvalidIndex) BIG_UpdateOneFileInFat(ul_Next);
				BIG_UpdateOneDirInFat(BIG_Root());
				LINK_gul_ColorTxt = 0x000000FF;
			}
		}
	}

	for(i = 0; i < BIG_gst.st_ToSave.ul_MaxDir; i++)
	{
		if(BAS_bsearch(i, &st_PosDir) == -1)
		{
			sprintf(asz_Msg, "ERROR : Dir is never referenced (%s)", BIG_DirName(i));
			LINK_PrintStatusMsg(asz_Msg);
			b_Res = FALSE;

			if(BIG_gb_CanClean)
			{
				LINK_gul_ColorTxt = 0x00FF0000;
				LINK_PrintStatusMsg("=> Moving to ROOT");

				ul_Next = BIG_SubDir(BIG_Root());
				BIG_PrevDir(i) = BIG_C_InvalidIndex;
				BIG_NextDir(i) = ul_Next;
				BIG_ParentDir(i) = BIG_Root();
				if(ul_Next != BIG_C_InvalidIndex)
				{
					BIG_PrevDir(ul_Next) = i;
					BIG_UpdateOneDirInFat(ul_Next);
				}

				BIG_SubDir(BIG_Root()) = i;
				BIG_UpdateOneDirInFat(BIG_Root());
				BIG_UpdateOneDirInFat(i);
				LINK_gul_ColorTxt = 0x000000FF;
			}
		}
	}

	BAS_bfree(&st_Pos);
	BAS_bfree(&st_PosFile);
	BAS_bfree(&st_PosDir);

	if(!b_Res && BIG_gb_CanClean) goto recom;

	return b_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BIG_b_CheckFile(BOOL _b_Stats)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_File	*pst_File;
	ULONG			i;
	char			asz_Text[512];
	char			asz_Text1[12];
	char			asz_Ext[10];
	ULONG			ul_TotalSize, ul_RealTotalSize;
	ULONG			ul_HoleMinSize, ul_HoleMaxSize, ul_HoleNumber, ul_HoleTotalSize;
	ULONG			ul_Size, ul_DelDirs;
	BIG_INDEX		ul_Index;
	BOOL			b_Res, b_Err;
	BAS_tdst_barray st_Ext;
	BAS_tdst_barray st_ExtCount;
	char			*psz_Tmp;
	ULONG			ul_Count;
	BIG_INDEX		ul1, ul2;
	unsigned short	shKeyState;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg("-- Check Project -------------------------------------------");
	L_strdate(asz_Text);
	L_strcat(asz_Text, " ");
	L_strtime(asz_Text + L_strlen(asz_Text));
	LINK_PrintStatusMsg(asz_Text);
	LINK_PrintStatusMsg("------------------------------------------------------------");

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	do
	{
		b_Err = FALSE;

		/*$2----------------------------------------------------------------------------------------------------------*/

		LINK_gul_ColorTxt = 0;
		LINK_PrintStatusMsg("General...");
		BIG_GetLastKey((int*)&ul1);
		ul2 = BIG_ul_GetKeyValue(NULL);
		if(ul2 <= ul1)
		{
			b_Err = TRUE;
			LINK_gul_ColorTxt = 0x000000FF;
			LINK_PrintStatusMsg("Your current key value is lesser than the last one found on BF !!! Very critical !!!");
			if(BIG_gb_CanClean)
			{
				LINK_gul_ColorTxt = 0x00FF0000;
				sprintf(asz_Text, "=> Set next key value to %x", ul1 + 1);
				LINK_PrintStatusMsg(asz_Text);
				BIG_SetKeyValue(ul1 + 1);
				b_Err = FALSE;
			}
		}

		LINK_gul_ColorTxt = 0;
		LINK_PrintStatusMsg("Checking deleted lists...");
		LINK_gul_ColorTxt = 0x000000FF;
		b_Res = BIG_CheckDeleted();
		if(!b_Res) b_Err = TRUE;

		BAS_binit(&BIG_gst_TouchedFiles, 200);
		BAS_binit(&BIG_gst_TouchedDirs, 200);

		LINK_gul_ColorTxt = 0;
		LINK_PrintStatusMsg("Checking hierarchy...");
		LINK_gul_ColorTxt = 0x000000FF;
		b_Res = BIG_CheckHierarchy(BIG_Root());
		if(!b_Res) b_Err = TRUE;

		LINK_gul_ColorTxt = 0;
		LINK_PrintStatusMsg("Checking links...");
		LINK_gul_ColorTxt = 0x000000FF;
		b_Res = BIG_CheckLinks();
		if(!b_Res) b_Err = TRUE;

		LINK_gul_ColorTxt = 0;
		LINK_PrintStatusMsg("Checking keys...");
		LINK_gul_ColorTxt = 0x000000FF;
		b_Res = BIG_CheckKeys();
		if(!b_Res) b_Err = TRUE;

		BAS_bfree(&BIG_gst_DeletedFiles);
		BAS_bfree(&BIG_gst_DeletedDirs);
		BAS_bfree(&BIG_gst_TouchedFiles);
		BAS_bfree(&BIG_gst_TouchedDirs);
	} while(b_Err && BIG_gb_CanClean && ( ((GetAsyncKeyState(VK_ESCAPE) >= 0) || !EDI_gb_NoVerbose ) ) );
	
    if(!EDI_gb_NoVerbose)
        while(GetAsyncKeyState(VK_ESCAPE) < 0);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	LINK_gul_ColorTxt = 0;
	LINK_PrintStatusMsg("------------------------------------------------------------");
	if(b_Err)
	{
		do
		{
			ERR_X_ForceError("BIGFILE IS CORRUPT !!! (SHIFT + OK to continue)", NULL);
		} while((GetAsyncKeyState(VK_SHIFT) >= 0) || !EDI_gb_NoVerbose);
		return TRUE;
	}

	/* Init stats */
	ul_TotalSize = 0;
	ul_RealTotalSize = 0;

	/* Count deleted dirs */
	ul_DelDirs = 0;
	ul_Index = BIG_gst.st_ToSave.ul_FirstFreeDir;
	while(ul_Index != BIG_C_InvalidIndex)
	{
		ul_DelDirs++;
		ul_Index = BIG_NextDir(ul_Index);
	}

	if(_b_Stats)
	{
		BAS_binit(&st_Ext, 200);
		BAS_binit(&st_ExtCount, 200);
		for(i = 0; i < BIG_gst.st_ToSave.ul_MaxFile; i++)
		{
			psz_Tmp = L_strrchr(BIG_NameFile(i), '.');
			if(psz_Tmp)
			{
				L_strcpy(asz_Ext, psz_Tmp);
				L_strupr(asz_Ext);
				psz_Tmp = (char *) (*(int *) asz_Ext);
			}

			if((ul_Count = BAS_bsearch((ULONG) psz_Tmp, &st_Ext)) != -1)
			{
				ul_Count += BIG_LengthDiskFile(i);
				BAS_binsert((ULONG) psz_Tmp, ul_Count, &st_Ext);
			}
			else
			{
				BAS_binsert((ULONG) psz_Tmp, BIG_LengthDiskFile(i), &st_Ext);
			}

			if((ul_Count = BAS_bsearch((ULONG) psz_Tmp, &st_ExtCount)) != -1)
			{
				ul_Count++;
				BAS_binsert((ULONG) psz_Tmp, ul_Count, &st_ExtCount);
			}
			else
			{
				BAS_binsert((ULONG) psz_Tmp, 1, &st_ExtCount);
			}

			pst_File = &BIG_gst.dst_FileTable[i];

			/* Stats */
			ul_TotalSize += BIG_gst.dst_FileTableExt[i].st_ToSave.ul_LengthOnDisk;
			ul_RealTotalSize += BIG_ul_GetLengthFile(BIG_PosFile(i));
		}

		for(i = 0; i < (ULONG) st_Ext.num; i++)
		{
			asz_Text1[0] = (char) (st_Ext.base[i].ul_Key & 0x000000FF);
			asz_Text1[1] = (char) ((st_Ext.base[i].ul_Key & 0x0000FF00) >> 8);
			asz_Text1[2] = (char) ((st_Ext.base[i].ul_Key & 0x00FF0000) >> 16);
			asz_Text1[3] = (char) ((st_Ext.base[i].ul_Key & 0xFF000000) >> 24);
			asz_Text1[4] = 0;
			sprintf(asz_Text, "%s: %d (%d)", asz_Text1, st_Ext.base[i].ul_Val, st_ExtCount.base[i].ul_Val);
			LINK_PrintStatusMsg(asz_Text);
		}

		BAS_bfree(&st_Ext);
		BAS_bfree(&st_ExtCount);
		LINK_PrintStatusMsg("------------------------------------------------------------");
	}

	/* Compute number of holes and size in holes, (total, min and max ) */
	i = BIG_FreeFile();
	ul_HoleMinSize = 0xFFFFFFFF;
	ul_HoleMaxSize = 0;
	ul_HoleNumber = 0;
	ul_HoleTotalSize = 0;
	while(i != BIG_C_InvalidIndex)
	{
		ul_Size = BIG_gst.dst_FileTableExt[i].st_ToSave.ul_LengthOnDisk;
		ul_HoleTotalSize += ul_Size;
		if(ul_Size < ul_HoleMinSize) ul_HoleMinSize = ul_Size;
		if(ul_Size > ul_HoleMaxSize) ul_HoleMaxSize = ul_Size;
		ul_HoleNumber++;
		i = BIG_NextFile(i);
	}

	if ( BIG_gst.st_ToSave.ul_MaxDir >= BIG_gst.dst_FatTable[0].ul_LastIndex - BIG_gst.dst_FatTable[0].ul_FirstIndex + 1)
	{
		if ( !EDI_gb_NoVerbose ) 
		{
			do 
			{
				ERR_X_ForceError("BIGFILE IS CORRUPT, directory count exceeds default fat size ! (SHIFT + OK to continue)", NULL);
				shKeyState =  GetKeyState(VK_SHIFT);
				shKeyState >>= 15;
			}
			while ( shKeyState != 1 ) ;
		}
		b_Err = TRUE;
	}


	sprintf(asz_Text, "Number of files  : %d", BIG_gst.st_ToSave.ul_MaxFile);
	LINK_PrintStatusMsg(asz_Text);
	sprintf(asz_Text, "Deleted files    : %d", ul_HoleNumber);
	LINK_PrintStatusMsg(asz_Text);
	sprintf(asz_Text, "Number of dirs   : %d", BIG_gst.st_ToSave.ul_MaxDir);
	LINK_PrintStatusMsg(asz_Text);
	sprintf(asz_Text, "Deleted dirs     : %d", ul_DelDirs);
	LINK_PrintStatusMsg(asz_Text);
	sprintf(asz_Text, "Number of fat    : %d", BIG_gst.st_ToSave.ul_NumFat);
	LINK_PrintStatusMsg(asz_Text);
	sprintf(asz_Text, "Default fat size : %d", BIG_gst.st_ToSave.ul_SizeOfFat);
	LINK_PrintStatusMsg(asz_Text);
	sprintf(asz_Text, "Size of fat(s)   : \n");
	LINK_PrintStatusMsg(asz_Text);
	for(i = 0; i < BIG_NumFat(); i++)
	{
		sprintf(asz_Text, "%d \n", BIG_gst.dst_FatTable[i].ul_LastIndex - BIG_gst.dst_FatTable[i].ul_FirstIndex + 1);
		LINK_PrintStatusMsg(asz_Text);
	}

	sprintf(asz_Text, "");
	LINK_PrintStatusMsg(asz_Text);

	if(_b_Stats)
	{
		sprintf(asz_Text, "Total file size on disk : %d bytes", ul_TotalSize);
		LINK_PrintStatusMsg(asz_Text);
		sprintf(asz_Text, "Total file size         : %d bytes", ul_RealTotalSize);
		LINK_PrintStatusMsg(asz_Text);
	}

	/* Holes information */
	if(ul_HoleNumber)
	{
		sprintf(asz_Text, "Total size of holes     : %d bytes", ul_HoleTotalSize);
		LINK_PrintStatusMsg(asz_Text);
		sprintf
		(
			asz_Text,
			"Stat on size of holes (min/average/max) : %d / %d / %d",
			ul_HoleMinSize,
			ul_HoleTotalSize / ul_HoleNumber,
			ul_HoleMaxSize
		);
		LINK_PrintStatusMsg(asz_Text);
	}

	LINK_PrintStatusMsg("------------------------------------------------------------");

	LINK_FatHasChanged();
	LINK_Refresh();
	return b_Err;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_RestoreDeleted(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	i, ul_Next, *pi;
	char		asz_Msg[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* First check bigfile */
	if(BIG_b_CheckFile(FALSE)) return;

	i = BIG_gst.st_ToSave.ul_FirstFreeFile;
	if(i == BIG_C_InvalidIndex) return;

	while(i != BIG_C_InvalidIndex)
	{
		ul_Next = BIG_NextFile(i);

		if(BIG_LengthDiskFile(i) == 0)
		{
			LINK_gul_ColorTxt = 0x000000FF;
			sprintf(asz_Msg, "Can't restore %s, size of file is null...", BIG_NameFile(i));
			LINK_PrintStatusMsg(asz_Msg);
			LINK_gul_ColorTxt = 0;
		}
		else
		{
			sprintf(asz_Msg, "Restoring deleted file %s in ROOT", BIG_NameFile(i));
			LINK_PrintStatusMsg(asz_Msg);
#ifdef JADEFUSION
			pi = (ULONG *) (BIG_NameFile(i) + BIG_C_MaxLenName - sizeof(int) - 2);
#else
			pi = (int *) (BIG_NameFile(i) + BIG_C_MaxLenName - sizeof(int) - 2);
#endif
			BIG_FileKey(i) = *pi;
			BIG_InsertKeyToFat(BIG_FileKey(i), i);

			if(BIG_NextFile(i) != BIG_C_InvalidIndex) BIG_PrevFile(BIG_NextFile(i)) = BIG_PrevFile(i);
			if(BIG_PrevFile(i) != BIG_C_InvalidIndex)
				BIG_NextFile(BIG_PrevFile(i)) = BIG_NextFile(i);
			else
				BIG_gst.st_ToSave.ul_FirstFreeFile = BIG_NextFile(i);

			BIG_PrevFile(i) = BIG_C_InvalidIndex;
			BIG_NextFile(i) = BIG_FirstFile(BIG_Root());
			if(BIG_FirstFile(BIG_Root()) != BIG_C_InvalidIndex) BIG_PrevFile(BIG_FirstFile(BIG_Root())) = i;
			BIG_FirstFile(BIG_Root()) = i;
			BIG_ParentFile(i) = BIG_Root();
			BIG_UpdateOneFileInFat(i);
			if(BIG_NextFile(i) != BIG_C_InvalidIndex) BIG_UpdateOneFileInFat(BIG_NextFile(i));
		}

		i = ul_Next;
	}

	/* Update header and root */
	BIG_UpdateOneDirInFat(BIG_Root());
	BIG_WriteHeader();

	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL b_CanOuputFinal(char *psz_Name)
{
	/*~~~~~~~~~~~~~*/
	char	*pz_Temp;
	/*~~~~~~~~~~~~~*/

	pz_Temp = L_strrchr(psz_Name, '.');
	if(!pz_Temp) return FALSE;

	if
	(
		!L_strcmpi(pz_Temp, EDI_Csz_ExtAIEditorModel)
	||	!L_strcmpi(pz_Temp, EDI_Csz_ExtAIEditorVars)
	||	!L_strcmpi(pz_Temp, EDI_Csz_ExtAIEditorFct)
	||	!L_strcmpi(pz_Temp, EDI_Csz_ExtAIEditorDepend)
	||	!L_strcmpi(pz_Temp, EDI_Csz_ExtIni)
	||	!L_strcmpi(pz_Temp, ".dsk")
	||	!L_strcmpi(pz_Temp, ".ini")
	||	!L_strcmpi(pz_Temp, ".tmp")
	)
	{
		return FALSE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BAS_tdst_barray BIG_gst_UniversRef;
static BOOL BIG_gb_UniversRefLoaded=FALSE;

static void LoadUniversRef(void)
{
    ULONG ul_fat,ul_key;
	char	*psz_Temp;
	char	asz_Name[BIG_C_MaxLenPath];
	char	asz_Dir[BIG_C_MaxLenPath];
	char	asz_Path[BIG_C_MaxLenPath];
    char*pc;
    ULONG ul_length;

    if(BIG_gb_UniversRefLoaded) return;

	ul_key = BIG_UniverseKey(); //univers.oin key
	if(ul_key != BIG_C_InvalidKey)
	{
        //get univers.oin fat index 
		ul_fat = BIG_ul_SearchKeyToFat(ul_key);
        
        //search univers.omd
        L_strcpy(asz_Name, BIG_NameFile(ul_fat));
	    psz_Temp = L_strrchr(asz_Name, '.');
	    if(psz_Temp) *psz_Temp = 0;
	    L_strcpy(asz_Dir, asz_Name);
	    L_strcat(asz_Name, EDI_Csz_ExtAIEngineModel);
	    BIG_ComputeFullName(BIG_ParentFile(ul_fat), asz_Path);
	    psz_Temp = L_strrchr(asz_Path, '/');
	    if(psz_Temp) *psz_Temp = 0;
        ul_fat = BIG_ul_SearchFileExt(asz_Path, asz_Name);

        //now load the contents
        if(ul_fat != BIG_C_InvalidIndex)
        {
            BAS_binit(&BIG_gst_UniversRef, 10);

            ul_key = BIG_FileKey(ul_fat);
            pc = BIG_pc_ReadFileTmp(BIG_ul_SearchKeyToPos(ul_key), &ul_length);
            ul_length = ul_length / 8 ;
            while(ul_length)
            {
                if(*(ULONG*)pc && (*(ULONG*)pc != BIG_C_InvalidKey)) 
                    BAS_binsert(*(ULONG*)pc, 1, &BIG_gst_UniversRef);

                ul_length--;
                pc += 8;
            }
            BIG_gb_UniversRefLoaded = TRUE;
        }
    }
}

static BOOL b_IsUniversFile(BIG_INDEX ul_Index)
{    
    if(ul_Index == BIG_C_InvalidIndex) return FALSE;
    if(!BIG_gb_UniversRefLoaded) return FALSE;
    if(BAS_bsearch(BIG_FileKey(ul_Index),&BIG_gst_UniversRef)!=-1) return TRUE;
    return FALSE;
}
    
static void UnloadUniversRef(void)
{
    if(BIG_gb_UniversRefLoaded) BAS_bfree(&BIG_gst_UniversRef);
    BIG_gb_UniversRefLoaded = FALSE;
}
    

BOOL b_DummyFile(BIG_INDEX ul_Index, BOOL *_pb_Univ)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL		b_Can;
	char		*pz_Temp;
	char		az_UnivPath[BIG_C_MaxLenPath];
	char		az_UnivPath1[BIG_C_MaxLenPath];
	BIG_INDEX	ul_Univ;
	char		*pz;
	char		asz_Path[BIG_C_MaxLenPath];
	char		*psz_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_pb_Univ = FALSE;
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	psz_Name = BIG_NameFile(ul_Index);

	pz_Temp = L_strrchr(psz_Name, '.');
	if(!pz_Temp) return TRUE;

	/* Universe */
	*az_UnivPath = 0;
	ul_Univ = BIG_UniverseKey();
	if(ul_Univ != BIG_C_InvalidKey)
	{
		ul_Univ = BIG_ul_SearchKeyToFat(ul_Univ);
		if(ul_Univ != BIG_C_InvalidIndex)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Univ), az_UnivPath);
			L_strcpy(az_UnivPath1, az_UnivPath);
			pz = L_strrchr(az_UnivPath1, '/');
			if(pz) *pz = 0;
		}
	}

	b_Can = FALSE;
	if
	(
		L_strnicmp(pz_Temp, EDI_Csz_ExtSoundMusic,4)
	&&	L_strnicmp(pz_Temp, EDI_Csz_ExtSoundAmbience,4)
	&&	L_strnicmp(pz_Temp, EDI_Csz_ExtLoadingSound,4)
	&&	L_strnicmp(pz_Temp, EDI_Csz_ExtSoundDialog,4)
	&&	L_strnicmp(pz_Temp, ".bin",4)
	&&	(L_strnicmp(pz_Temp, EDI_Csz_ExtVideo1,4) || (WOR_gi_CurrentConsole!=1))
	&&	(L_strnicmp(pz_Temp, EDI_Csz_ExtVideo2,4) || (WOR_gi_CurrentConsole!=0))
	&&	(L_strnicmp(pz_Temp, EDI_Csz_ExtVideo3,4) || (WOR_gi_CurrentConsole!=1))
	&&	(L_strnicmp(pz_Temp, EDI_Csz_ExtVideo4,4) || (WOR_gi_CurrentConsole!=0) && (WOR_gi_CurrentConsole!=2))
	&&	(L_strnicmp(pz_Temp, EDI_Csz_ExtVideo5,4) || (WOR_gi_CurrentConsole!=3))
	&&	L_strnicmp(pz_Temp, EDI_Csz_ExtAIEngineModel,4)
	&&	L_strnicmp(pz_Temp, EDI_Csz_ExtAIEngineFct,4)
	&&	L_strnicmp(pz_Temp, EDI_Csz_ExtAIEngineVars,4)
	&&	L_strnicmp(pz_Temp, EDI_Csz_ExtAIEngineInstance,4)
	) b_Can = TRUE;

	if
	(
		!L_strnicmp(pz_Temp, EDI_Csz_ExtAIEngineModel,4)
	||	!L_strnicmp(pz_Temp, EDI_Csz_ExtAIEngineFct,4)
	||	!L_strnicmp(pz_Temp, EDI_Csz_ExtAIEngineVars,4)
	||	!L_strnicmp(pz_Temp, EDI_Csz_ExtAIEngineInstance,4)
	)
	{
		if((L_strcmpi(az_UnivPath, asz_Path)) && (L_strcmpi(az_UnivPath1, asz_Path)) && !b_IsUniversFile(ul_Index))
		{
			b_Can = TRUE;
		}
		else
		{
			*_pb_Univ = TRUE;
		}
	}

	return b_Can;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern BOOL LOA_b_FileIsInLoadedList(ULONG _h_Key);
extern void LOA_FreeLoadedFileList(void);
extern void LOA_LoadLoadedFileList(void);

void BIG_CleanFinal(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		ul_Index, ul1;
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Path2[BIG_C_MaxLenPath];
	char		*psz_Name;
	extern BOOL BIG_gb_NoMerge;
	extern BOOL BIG_gb_AlignFile;
	char		asz_Msg[512];
	BOOL		b_Univ;
	int			pos;
	BIG_KEY		h_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Create directories */
	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(!b_CanOuputFinal(BIG_NameFile(ul_Index))) continue;

		BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
		psz_Name = BIG_NameFile(ul_Index);

		SWAP_BIG();
		BIG_ul_CreateDir(asz_Path);
		if(BIG_ul_SearchFileExt(asz_Path, psz_Name) != BIG_C_InvalidIndex)
		{
			sprintf(asz_Msg, "%s/%s", asz_Path, psz_Name);
			ERR_X_Warning(0, "Two files with the same name !!!", asz_Msg);
		}

		SWAP_BIG();
	}

	/* Pos init file */
	SWAP_BIG();
	L_fseek(BIG_gst.h_CLibFileHandle, 0, L_SEEK_END);
	pos = L_ftell(BIG_gst.h_CLibFileHandle);
	pos += 2048;
	pos &= 0xFFFFF800;
	L_fseek(BIG_gst.h_CLibFileHandle, pos - 4, L_SEEK_SET);
	BIG_fwrite(&pos, sizeof(ULONG), BIG_Handle());
	SWAP_BIG();

	BIG_gb_AlignFile = TRUE;

	/* Universe */
	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(!b_CanOuputFinal(BIG_NameFile(ul_Index))) continue;
		if(!b_DummyFile(ul_Index, &b_Univ) && b_Univ)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			BIG_ul_ExportMergeFileRec(asz_Path, BIG_NameFile(ul_Index));
		}
	}

	/* Files */
	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		/*~~*/
		int i;
		/*~~*/

		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(!b_CanOuputFinal(BIG_NameFile(ul_Index))) continue;
		psz_Name = L_strrchr(BIG_NameFile(ul_Index), '.');
		if(!L_strcmpi(psz_Name, EDI_Csz_ExtWorldList))
		{
			/* WOL */
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			BIG_ul_ExportMergeFileRec(asz_Path, BIG_NameFile(ul_Index));

			h_Key = BIG_FileKey(ul_Index);

			/* Bin engine */
			ul1 = BIG_ul_SearchKeyToFat(h_Key | 0xFF000000);
			if(ul1 != BIG_C_InvalidIndex)
			{
				BIG_ComputeFullName(BIG_ParentFile(ul1), asz_Path);
				BIG_ul_ExportMergeFileRec(asz_Path, BIG_NameFile(ul1));
			}

			/* Bin sounds */
			ul1 = BIG_ul_SearchKeyToFat((0x0007FFFF&h_Key) | 0xFF400000);
			if(ul1 != BIG_C_InvalidIndex)
			{
				BIG_ComputeFullName(BIG_ParentFile(ul1), asz_Path);
				BIG_ul_ExportMergeFileRec(asz_Path, BIG_NameFile(ul1));
			}

			/* Bin sounds headers */
			for(i = 0; i < 32; i ++)
			{
				if(!INO_b_LanguageIsPresent(i) ) continue;
				ul1 = 0xFE000000 | (h_Key&0x0007FFFF) | ((i+ 1) << 19);
				ul1 = BIG_ul_SearchKeyToFat(ul1);
				if(ul1 != BIG_C_InvalidIndex)
				{
					BIG_ComputeFullName(BIG_ParentFile(ul1), asz_Path);
					BIG_ul_ExportMergeFileRec(asz_Path, BIG_NameFile(ul1));
				}
			}

			/* Bin text */
			for(i = 0; i < 32; i++)
			{
				if(!INO_b_LanguageIsPresent(i) ) continue;
				ul1 = 0xFD000000 | (h_Key&0x0007FFFF) | ((i+ 1) << 19);
				ul1 = BIG_ul_SearchKeyToFat(ul1);
				if(ul1 != BIG_C_InvalidIndex)
				{
					BIG_ComputeFullName(BIG_ParentFile(ul1), asz_Path);
					BIG_ul_ExportMergeFileRec(asz_Path, BIG_NameFile(ul1));
				}
			}

			/* Bin textures */
			ul1 = BIG_ul_SearchKeyToFat((h_Key&0x0007FFFF) | 0xFF800000);
			if(ul1 != BIG_C_InvalidIndex)
			{
				BIG_ComputeFullName(BIG_ParentFile(ul1), asz_Path);
				BIG_ul_ExportMergeFileRec(asz_Path, BIG_NameFile(ul1));
			}
		}
	}

	/* video */
	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(!b_CanOuputFinal(BIG_NameFile(ul_Index))) continue;
		psz_Name = L_strrchr(BIG_NameFile(ul_Index), '.');
		if
		(
			(!L_strnicmp(psz_Name, EDI_Csz_ExtVideo1,4) && (WOR_gi_CurrentConsole==1))
		||	(!L_strnicmp(psz_Name, EDI_Csz_ExtVideo2,4) && ((WOR_gi_CurrentConsole==0)||WOR_gi_CurrentConsole==3))
		||	!L_strnicmp(psz_Name, EDI_Csz_ExtVideo3,4)
		||	(!L_strnicmp(psz_Name, EDI_Csz_ExtVideo4,4) && (WOR_gi_CurrentConsole==2))
		||	(!L_strnicmp(psz_Name, EDI_Csz_ExtVideo5,4) && (WOR_gi_CurrentConsole==3))
		)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			BIG_ul_ExportMergeFileRec(asz_Path, BIG_NameFile(ul_Index));
		}
	}

    /* streamed sound files */
    LOA_LoadLoadedFileList();
	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(!b_CanOuputFinal(BIG_NameFile(ul_Index))) continue;
		psz_Name = L_strrchr(BIG_NameFile(ul_Index), '.');
		if
		(
			!L_strnicmp(psz_Name, EDI_Csz_ExtSoundMusic,4)
		||	!L_strnicmp(psz_Name, EDI_Csz_ExtSoundAmbience,4)
		||	!L_strnicmp(psz_Name, EDI_Csz_ExtLoadingSound,4)
		||	!L_strnicmp(psz_Name, EDI_Csz_ExtSoundDialog,4)
#ifdef JADEFUSION
		||	!L_strnicmp(psz_Name, EDI_Csz_ExtSoundFile,4)
#endif
		)
		{
            if(LOA_b_FileIsInLoadedList(BIG_FileKey(ul_Index)))
            {

			    BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

				// del dialogs of others languages
				if(!L_strnicmp(psz_Name, EDI_Csz_ExtSoundDialog,4))
				{
					L_strcpy(asz_Path2, asz_Path);
					_strupr(asz_Path2);

					if(!INO_b_LanguageIsPresent(INO_e_French) && strstr(asz_Path2, "/LANGUAGE/FRENCH/")) continue;
					if(!INO_b_LanguageIsPresent(INO_e_English) && strstr(asz_Path2, "/LANGUAGE/ENGLISH/")) continue;
					if(!INO_b_LanguageIsPresent(INO_e_German) && strstr(asz_Path2, "/LANGUAGE/GERMAN/")) continue;
					if(!INO_b_LanguageIsPresent(INO_e_Spanish) && strstr(asz_Path2, "/LANGUAGE/SPANISH/")) continue;
					if(!INO_b_LanguageIsPresent(INO_e_Italian) && strstr(asz_Path2, "/LANGUAGE/ITALIAN/")) continue;
                    if(!INO_b_LanguageIsPresent(INO_e_Dutch) && strstr(asz_Path2, "/LANGUAGE/DUTCH/")) continue;
                    if(!INO_b_LanguageIsPresent(INO_e_Polish) && strstr(asz_Path2, "/LANGUAGE/POLISH/")) continue;
					if(!INO_b_LanguageIsPresent(INO_e_Finnish) && strstr(asz_Path2, "/LANGUAGE/FINNISH/")) continue;
					if(!INO_b_LanguageIsPresent(INO_e_Swedish) && strstr(asz_Path2, "/LANGUAGE/SWEDISH/")) continue;
					if(!INO_b_LanguageIsPresent(INO_e_Norwegian) && strstr(asz_Path2, "/LANGUAGE/NORWEGIAN/")) continue;
                    if(!INO_b_LanguageIsPresent(INO_e_Danish) && strstr(asz_Path2, "/LANGUAGE/DANISH/")) continue;
				}

			    BIG_ul_ExportMergeFileRec(asz_Path, BIG_NameFile(ul_Index));
            }
		}
	}
    LOA_FreeLoadedFileList();

	BIG_gb_AlignFile = FALSE;
}


extern void BIG_CallbackLoadFileFromBigFile(BIG_INDEX, BIG_INDEX);
extern BOOL BIG_gb_CheckSameName;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_CleanByFat(void)
{
	ULONG	ul_Index;
	char	asz_Name[L_MAX_PATH];
	char	asz_Path[BIG_C_MaxLenPath];
	ULONG	ul_SizeFat;
	char	asz_Msg[1024];
	char	*psz_Name;
	ULONG	ul_Dir;

	ul_SizeFat = BIG_gst.st_ToSave.ul_MaxFile;

	/* Create a new bigfile to copy into. Allocate the exact fat size */
	L_strcpy(asz_Name, BIG_gst.asz_Name);
	if(L_strrchr(asz_Name, '.')) *L_strrchr(asz_Name, '.') = 0;
	L_strcat(asz_Name, "_clean");
	L_strcat(asz_Name, BIG_Csz_BigExt);

	SWAP_BIG();
	BIG_CreateEmptyPriv(asz_Name, BIG_gst1.st_ToSave.ul_Version, ul_SizeFat, 0, 0, 0);

	sprintf(asz_Msg, "*** Creating bigfile %s ***", asz_Name);
	LINK_PrintStatusMsg(asz_Msg);

	BIG_Open(asz_Name);

	/* Copy infos of header */
	BIG_gst.st_ToSave.ul_UniverseKey = BIG_gst1.st_ToSave.ul_UniverseKey;
	BIG_WriteHeader();
	SWAP_BIG();

	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) != EDI_FHC_Deleted && BIG_FileKey(ul_Index) != BIG_C_InvalidKey)
		{
			if(!BIG_gb_CleanFatEmpty || L_strnicmp(BIG_NameFile(ul_Index), "_nobody_", 8))
			{
				BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
				ul_Dir = BIG_ParentFile(ul_Index);
				psz_Name = BIG_NameFile(ul_Index);

				SWAP_BIG();
				BIG_ul_CreateDir(asz_Path);
				SWAP_BIG();

				BIG_gpsz_RealName = NULL;
				BIG_gb_CheckSameName = FALSE;
				BIG_ul_UpdateCreateFileOptim(ul_Dir, ul_Index, asz_Path, psz_Name, BIG_CallbackLoadFileFromBigFile, FALSE);
				BIG_gb_CheckSameName = TRUE;
				SWAP_BIG();
			}
		}
	}

	/* Close copy */
	SWAP_BIG();
	BIG_Close();
	SWAP_BIG();   
	LINK_PrintStatusMsg("... Finished");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BIG_gb_CleanLoaded = FALSE;


void BIG_Clean(BOOL _b_Final)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Name[L_MAX_PATH];
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Msg[512];
	ULONG		ul_NumFiles, ul_NumDirs, ul_SizeFat;
	BIG_INDEX	ul_Index;
	char		*psz_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//	BIG_CleanByFat();
//	return;

    LoadUniversRef();

	/* Count number of files without holes */
	ul_NumFiles = BIG_gst.st_ToSave.ul_MaxFile;
	ul_Index = BIG_gst.st_ToSave.ul_FirstFreeFile;
	while(ul_Index != BIG_C_InvalidIndex)
	{
		ul_NumFiles--;
		BIG_FileChanged(ul_Index) = EDI_FHC_Deleted;
		ul_Index = BIG_NextFile(ul_Index);
	}

	ul_NumDirs = BIG_gst.st_ToSave.ul_MaxDir;
	ul_Index = BIG_gst.st_ToSave.ul_FirstFreeDir;
	while(ul_Index != BIG_C_InvalidIndex)
	{
		ul_NumDirs--;
		BIG_DirChanged(ul_Index) = EDI_FHC_Deleted;
		ul_Index = BIG_NextDir(ul_Index);
	}

	/* Size of fat is the max */
	if(ul_NumDirs > ul_NumFiles)
		ul_SizeFat = ul_NumDirs + 1;
	else
		ul_SizeFat = ul_NumFiles + 1;

	if(!_b_Final) ul_SizeFat += 1000;

	/* Decreased by number of dummy files (final) */
	if(_b_Final)
	{
		/*~~~~~~~~~~~*/
		BOOL	b_Univ;
		/*~~~~~~~~~~~*/

		for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
		{
			if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
			if(!b_CanOuputFinal(BIG_NameFile(ul_Index)))
			{
				ul_SizeFat--;
				continue;
			}

			if(b_DummyFile(ul_Index, &b_Univ))
			{
				ul_SizeFat--;
				continue;
			}
		}
	}

    if(ul_NumDirs > ul_SizeFat) 
		ul_SizeFat = ul_NumDirs + 1;


	/* Create a new bigfile to copy into. Allocate the exact fat size */
	L_strcpy(asz_Name, BIG_gst.asz_Name);
	if(L_strrchr(asz_Name, '.')) *L_strrchr(asz_Name, '.') = 0;
	L_strcat(asz_Name, "_clean");
	L_strcat(asz_Name, BIG_Csz_BigExt);

	SWAP_BIG();
	BIG_CreateEmptyPriv(asz_Name, BIG_gst1.st_ToSave.ul_Version, ul_SizeFat, 0, 0, _b_Final ? 1 : 0);

	sprintf(asz_Msg, "*** Creating bigfile %s ***", asz_Name);
	LINK_PrintStatusMsg(asz_Msg);

	BIG_Open(asz_Name);

	/* Copy infos of header */
	BIG_gst.st_ToSave.ul_UniverseKey = BIG_gst1.st_ToSave.ul_UniverseKey;
	BIG_WriteHeader();

	/* Write all files */
	SWAP_BIG();
	if(_b_Final)
	{
		BIG_CleanFinal();
	}
	else
	{
		for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
		{
			if(BIG_FileChanged(ul_Index) != EDI_FHC_Deleted)
			{
                if(BIG_gb_CleanLoaded)
                {
                    if(!(BIG_FileChanged(ul_Index) & EDI_FHC_Loaded)) continue;
                }

				BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
				psz_Name = BIG_NameFile(ul_Index);

				SWAP_BIG();
				BIG_ul_CreateDir(asz_Path);
				if(BIG_ul_SearchFileExt(asz_Path, psz_Name) != BIG_C_InvalidIndex)
				{
					sprintf(asz_Msg, "%s/%s", asz_Path, psz_Name);
					ERR_X_Warning(0, "Two files with the same name !!!", asz_Msg);
				}

				SWAP_BIG();

				BIG_ul_ExportMergeFileRec(asz_Path, psz_Name);
			}
		}
	}

	/* Close copy */
	SWAP_BIG();
	BIG_Close();
	SWAP_BIG();
    
    UnloadUniversRef();

	LINK_PrintStatusMsg("... Finished");
}

void BIG_CleanLoaded(void)
{
    BIG_gb_CleanLoaded = TRUE;
    BIG_Clean(FALSE);
    BIG_gb_CleanLoaded = FALSE;
}

extern BOOL SND_b_CheckSoundFiles(ULONG _ul_FileNb, ULONG *_pul_KeyList, ULONG *_pul_TypeList);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL BIG_b_CheckAllSoundFiles(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	char	*pz_Temp, *psz_Name;
	char	asz_Path[1024];
	ULONG	ul_SndFileNb;
	ULONG	*pul_SndFileKey;
	ULONG	*pul_SndFileType;
	BOOL	ret;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_SndFileNb = 0;
	pul_SndFileKey = NULL;
	pul_SndFileType = NULL;

	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;

		
		
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			psz_Name = BIG_NameFile(ul_Index);

			pz_Temp = L_strrchr(psz_Name, '.');
			if(!pz_Temp) continue;

			if
			(
				!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundMusic,4)
			||	!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundAmbience,4)
			||	!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundDialog,4)
			||	!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundFile,4)
			||	!L_strnicmp(pz_Temp, EDI_Csz_ExtLoadingSound,4)
			||	!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundBank,4)
			||	!L_strnicmp(pz_Temp, EDI_Csz_ExtSModifier,4)
			||	!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundMetaBank,4)
			)
			{
				ul_SndFileNb++;

				if(pul_SndFileKey == NULL)
				{
					pul_SndFileKey = (ULONG*)malloc(sizeof(ULONG));
					pul_SndFileType = (ULONG*)malloc(sizeof(ULONG));
				}
				else
				{
					pul_SndFileKey = (ULONG*)realloc(pul_SndFileKey, ul_SndFileNb * sizeof(ULONG));
					pul_SndFileType = (ULONG*)realloc(pul_SndFileType, ul_SndFileNb * sizeof(ULONG));
				}

				pul_SndFileKey[ul_SndFileNb - 1] = BIG_FileKey(ul_Index);

				if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundMusic,4))
					pul_SndFileType[ul_SndFileNb - 1] = SND_Cte_Type_Music;
				else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundAmbience,4))
					pul_SndFileType[ul_SndFileNb - 1] = SND_Cte_Type_Ambience;
				else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundDialog,4))
					pul_SndFileType[ul_SndFileNb - 1] = SND_Cte_Type_Dialog;
				else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundFile,4))
					pul_SndFileType[ul_SndFileNb - 1] = SND_Cte_Type_Sound;
				else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtLoadingSound,4))
					pul_SndFileType[ul_SndFileNb - 1] = SND_Cte_Type_LoadingSound;
				else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundBank,4))
					pul_SndFileType[ul_SndFileNb - 1] = SND_Cte_Type_Bank;
				else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSModifier,4))
					pul_SndFileType[ul_SndFileNb - 1] = SND_Cte_Type_SModifier;
				else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundMetaBank,4))
					pul_SndFileType[ul_SndFileNb - 1] = SND_Cte_Type_MetaBank;
				else
					pul_SndFileType[ul_SndFileNb - 1] = -1;
			}
		
	}

	if(ul_SndFileNb)
	{
		ret = SND_b_CheckSoundFiles(ul_SndFileNb, pul_SndFileKey, pul_SndFileType);
	}
	else
	{
		ret = TRUE;
		LINK_PrintStatusMsg("This bigfile doesn't contain any sound file.");
	}

	if(pul_SndFileKey) free(pul_SndFileKey);
	if(pul_SndFileType) free(pul_SndFileType);

	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int si_compare(const void *arg1, const void *arg2)
{
	if(((*(char *) arg1 == '_') && (*(char *) arg2 != '_'))) return -1;
	if(((*(char *) arg2 == '_') && (*(char *) arg1 != '_'))) return 1;

	return _stricmp((char *) arg1, (char *) arg2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_MakeWolList(char *_asz_Path)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	char	*pz_Temp, *psz_Name;
	char	asz_Path[1024];
	L_FILE	h_File;
	char	asz_FileName[] = "maplist.txt";
	char	*psz_Dir;
	char	lower[1024];
	char	*zone;
	int		i_size, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg("Searching all *.wol files...");

	if(!_asz_Path)
		h_File = L_fopen(asz_FileName, "w+t");
	else
		h_File = L_fopen(_asz_Path, "w+t");

	if(!h_File)
	{
		LINK_PrintStatusMsg("[ERROR] Can't create list file, operation canceled.");
		return;
	}

	zone = NULL;
	i_size = 0;

	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;

		BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
		psz_Name = BIG_NameFile(ul_Index);

		pz_Temp = L_strrchr(psz_Name, '.');
		if(!pz_Temp) continue;

		/* get only wol file */
		if(L_strcmpi(pz_Temp, EDI_Csz_ExtWorldList)) continue;

		/* get only level dir */
		if(L_strnicmp(asz_Path, EDI_Csz_Path_Levels, L_strlen(EDI_Csz_Path_Levels))) continue;

		psz_Dir = asz_Path + L_strlen(EDI_Csz_Path_Levels) + 1;

		/* filter basic */
		if(!L_strnicmp(psz_Dir, "_basic", L_strlen("_basic"))) continue;

		L_strcpy(lower, psz_Dir);
		_strlwr(lower);

        /* filter 06 Levels/1xx_... */
        pz_Temp = lower;
        {
            int i,j;
            i=0;
            j=0;
            
            while(*pz_Temp)
            {                
                if( (*pz_Temp >= '0') && (*pz_Temp <= '9') )
                {
                    i++;                
                    if(!j) 
                    {
                        j = *pz_Temp - '0' + 1;
                        pz_Temp++;
                        continue;
                    }
                }
                else
                    break;
                pz_Temp++;
            }
            
            if(i> 3) continue; 
            if(i==3)
            {
                if(j-1 >= 1) continue;
            }
        }


		/* filter test */
        /*
		pz_Temp = L_strchr(lower, 't');
		while(pz_Temp)
		{
			if(!L_strnicmp(pz_Temp, "test", L_strlen("test"))) break;
			pz_Temp = L_strchr(pz_Temp + 1, 't');
		}
		if(pz_Temp) continue;
        */

		/* filter _main_fix */
		pz_Temp = L_strchr(lower, 'm');
		while(pz_Temp)
		{
			if(!L_strnicmp(pz_Temp, "main_fix", L_strlen("main_fix"))) break;
			pz_Temp = L_strchr(pz_Temp + 1, 'm');
		}

		if(pz_Temp) continue;

		/* add in list */
		if(i_size)
		{
			zone = (char *) L_realloc(zone, (i_size + 1) * 512);
		}
		else
		{
			zone = (char *) L_malloc(512);
		}

		L_memset(zone + (i_size * 512), 0, 512);
		sprintf(zone + (i_size * 512), "%s/%s.%08x",psz_Dir, psz_Name, BIG_FileKey(ul_Index));
		i_size++;
	}

	qsort(zone, i_size, 512, si_compare);

	lower[0] = '#';
	lower[1] = '\0';

	for(i = 0; i < i_size; i++)
	{
		if(L_strnicmp(zone + (i * 512), lower, L_strlen(lower)))
		{
			pz_Temp = L_strchr(zone + (i * 512), '/');
			L_memcpy(lower, zone + (i * 512), pz_Temp - (zone + (i * 512)));
			lower[pz_Temp - (zone + (i * 512))] = '\0';
			fprintf(h_File, ";===========================\n");
			fprintf(h_File, "; Map %s\n", lower);
			fprintf(h_File, ";===========================\n");
		}

		fprintf(h_File, "%s\n", zone + (i * 512));
	}

	L_free(zone);
	L_fclose(h_File);
	LINK_PrintStatusMsg("Done.");
}

/*
 =======================================================================================================================
    delete all bin files created for the map _ul_Key
 =======================================================================================================================
 */
void BIG_CleanBinFiles(ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_BinKey;
	ULONG	ul_BinKeyStart;
	ULONG	ul_BinKeyEnd;
	ULONG	ul_Fat;
	char	asz_Path[BIG_C_MaxLenPath];
    char	asz_File[BIG_C_MaxLenPath];
    char	asz_Msg[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Key == BIG_C_InvalidKey) return;
    LINK_PrintStatusMsg("Cleaning Bin Files...");

	/*$1- 0xFF...... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_BinKey = 0xFF000000 | (0x00FFFFFF & _ul_Key);
	ul_Fat = BIG_ul_SearchKeyToFat(ul_BinKey);
	if(ul_Fat != BIG_C_InvalidIndex)
	{
        sprintf(asz_Msg, "delete %s",  BIG_NameFile(ul_Fat));
		LINK_PrintStatusMsg(asz_Msg);
        BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		BIG_DelFile(asz_Path, BIG_NameFile(ul_Fat));
	}

    /*$1- 0xFF8..... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_BinKey = 0xFF800000 | (0x0007FFFF & _ul_Key);
	ul_Fat = BIG_ul_SearchKeyToFat(ul_BinKey);
	if(ul_Fat != BIG_C_InvalidIndex)
	{
        sprintf(asz_Msg, "delete %s",  BIG_NameFile(ul_Fat));
		LINK_PrintStatusMsg(asz_Msg);
		BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		BIG_DelFile(asz_Path, BIG_NameFile(ul_Fat));
	}
	
    /*$1- 0xFF4..... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_BinKey = 0xFF400000 | (0x0007FFFF&_ul_Key);
	ul_Fat = BIG_ul_SearchKeyToFat(ul_BinKey);
	if(ul_Fat != BIG_C_InvalidIndex)
	{
        sprintf(asz_Msg, "delete %s",  BIG_NameFile(ul_Fat));
		LINK_PrintStatusMsg(asz_Msg);
		BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		BIG_DelFile(asz_Path, BIG_NameFile(ul_Fat));
	}

    sprintf(asz_File, "%x.tmp", ul_BinKey);
    ul_Fat=BIG_ul_SearchFileExt("Root/Bin",asz_File);	
	if(ul_Fat != BIG_C_InvalidIndex)
	{
        sprintf(asz_Msg, "delete %s",  BIG_NameFile(ul_Fat));
		LINK_PrintStatusMsg(asz_Msg);
		BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		BIG_DelFile(asz_Path, BIG_NameFile(ul_Fat));
	}

    /*$1- 0xFEi.... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_BinKeyStart = 0xFE000000 | (0x0007FFFF&_ul_Key);
    ul_BinKeyEnd = 0xFEF80000 | (0x0007FFFF & _ul_Key);
    for(ul_BinKey= ul_BinKeyStart; ul_BinKey <= ul_BinKeyEnd; ul_BinKey += 0x00080000)
    {
	    ul_Fat = BIG_ul_SearchKeyToFat(ul_BinKey);
	    if(ul_Fat != BIG_C_InvalidIndex)
	    {
            sprintf(asz_Msg, "delete %s",  BIG_NameFile(ul_Fat));
		    LINK_PrintStatusMsg(asz_Msg);
		    BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		    BIG_DelFile(asz_Path, BIG_NameFile(ul_Fat));
	    }
        sprintf(asz_File, "%x.tmp", ul_BinKey);
        ul_Fat=BIG_ul_SearchFileExt("Root/Bin",asz_File);	
	    if(ul_Fat != BIG_C_InvalidIndex)
	    {
            sprintf(asz_Msg, "delete %s",  BIG_NameFile(ul_Fat));
		    LINK_PrintStatusMsg(asz_Msg);
		    BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		    BIG_DelFile(asz_Path, BIG_NameFile(ul_Fat));
	    }
    }

	/*$1- 0xFDi.... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ul_BinKeyStart = 0xFD000000 | (0x0007FFFF & _ul_Key);
    ul_BinKeyEnd = 0xFDF80000 | (0x0007FFFF & _ul_Key);
    for(ul_BinKey= ul_BinKeyStart; ul_BinKey <= ul_BinKeyEnd; ul_BinKey += 0x00080000)
    {
		ul_Fat = BIG_ul_SearchKeyToFat(ul_BinKey);
	    if(ul_Fat != BIG_C_InvalidIndex)
	    {
            sprintf(asz_Msg, "delete %s",  BIG_NameFile(ul_Fat));
		    LINK_PrintStatusMsg(asz_Msg);
		    BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		    BIG_DelFile(asz_Path, BIG_NameFile(ul_Fat));
	    }
    }

	//$1- BinTextures
	ul_Fat = BIG_ul_SearchKeyToFat(_ul_Key);
	if(ul_Fat != BIG_C_InvalidIndex)
	{
		BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		L_strcat(asz_Path, "/BinTextures");
        sprintf(asz_Msg, "delete dir %s",  asz_Path);
		LINK_PrintStatusMsg(asz_Msg);
		BIG_DelDir(asz_Path);
	}
}

#endif /* ACTIVE_EDITORS */

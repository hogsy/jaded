/*$T BIGdefs.h GC!1.38 05/25/99 15:08:19 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif


#ifndef __BIGDEFS_H__
#define __BIGDEFS_H__

#include "BASe/BAStypes.h"
#include "BASe/BASarray.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIxxx.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 ---------------------------------------------------------------------------------------------------
    Some constants.
 ---------------------------------------------------------------------------------------------------
 */

#define BIG_C_MaxLenPath        255         /* Max len of total file name */
#define BIG_C_MaxLenName        64          /* Max len of dir and file name in a bigfile */
#define BIG_C_MaxRealLenName    59          /* Max len of file name in a bigfile without extention*/

#define BIG_Csz_Header          "BIG\0"     /* String that must be found a the beginning of each
                                             * bigfile */
#define BIG_Csz_HeaderCrypted   "BUG\0"     /* String that must be found a the beginning of each
                                             * bigfile whose FAT is crypted*/

#define BIG_Csz_BigExt          ".bf"       /* Extension for a bigfile project */

#define BIG_C_MaxLenUserName    16          /* Max len of a user name (for owner) */
#define BIG_C_MaxLenOwner       16 + 20
#define BIG_Csz_Root            "ROOT"

/*
 * Unvalid extension for a bigfile. This extension is reserved to save a bigfile in a temporary
 * file
 */
#define BIG_Csz_BadExtension    ".$$$"
#define BIG_C_InvalidIndex      0xFFFFFFFF  /* When no file or directory (index value) */

/*$2
 ---------------------------------------------------------------------------------------------------
    For file creation
 ---------------------------------------------------------------------------------------------------
 */

typedef void (*BIG_tdpfnv_Callback) (ULONG, ULONG);

/* Bigfile handle on file and dir */
#define BIG_INDEX   ULONG

/* Key */
#define BIG_KEY ULONG

#ifdef ACTIVE_EDITORS

/* For mc_HasChanged field */
#define EDI_FHC_None        (char) 0x00
#define EDI_FHC_Delete      (char) 0x01
#define EDI_FHC_Rename      (char) 0x02
#define EDI_FHC_AddUpdate   (char) 0x03
#define EDI_FHC_Content     (char) 0x04
#define EDI_FHC_Move        (char) 0x05
#define EDI_FHC_Touch       (char) 0x10
#define EDI_FHC_Loaded		(char) 0x20
#define EDI_FHC_Create		(char) 0x40
#define EDI_FHC_Deleted     (char) 0xFF

/*
 ---------------------------------------------------------------------------------------------------
    A directory description.
 ---------------------------------------------------------------------------------------------------
 */
typedef struct BIG_tdst_Directory_
{
    struct
    {
        BIG_INDEX   ul_FirstFile;               /* Index of the first file in file table */
        BIG_INDEX   ul_FirstSubDir;             /* Index of the first sub-dir in dir table */

        /* To link */
        BIG_INDEX   ul_Prev;
        BIG_INDEX   ul_Next;
        BIG_INDEX   ul_Parent;

        char        asz_Name[BIG_C_MaxLenName]; /* Name of the directory */
    } st_ToSave;

    char    mc_HasChanged;

    struct
    {
        BOOL    b_IsExpanded[5];
    } st_BRO;
} BIG_tdst_Directory;
#endif /* ACTIVE_EDITORS */

/*
 ---------------------------------------------------------------------------------------------------
    A file description in a bigfile. For engine and editors.
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  BIG_tdst_File_
{
    ULONG   ul_Pos;                             /* Position of file data in the bigfile */
    BIG_KEY ul_Key;                             /* Associated key */
} BIG_tdst_File;

#ifdef ACTIVE_EDITORS

/*
 ---------------------------------------------------------------------------------------------------
    A file description in a bigfile. For editors.
 ---------------------------------------------------------------------------------------------------
 */
typedef struct BIG_tdst_FileExt_
{
    struct
    {
        ULONG       ul_LengthOnDisk;            /* Length of file in disk */

        /* To link */
        BIG_INDEX   ul_Prev;
        BIG_INDEX   ul_Next;
        BIG_INDEX   ul_Parent;

        L_time_t    x_Time;                     /* Creation/Update date */
        char        asz_Name[BIG_C_MaxLenName]; /* Name of the file */
		
		ULONG		ul_P4RevisionClient;				// [Perforce] Client revision of file (kept in BF but given by FStat query to Perforce). 
    } st_ToSave;

	char	 c_HasChanged;
    
	char	 asz_P4OwnerName[BIG_C_MaxLenOwner]; // [Perforce] Owner of the file on server (given by FStat query to Perforce)
	//char	 asz_P4PathInBF[BIG_C_MaxLenPath+BIG_C_MaxLenName]; // [Perforce] Path in BF of the file as stored in Perforce (given by FStat query to Perforce) 
	ULONG	 ul_P4RevisionServer;				// [Perforce] Server revision of the file on server (given by FStat query to Perforce)
	L_time_t x_P4Time;							// [Perforce] Server time of the file on server (given by FStat query to Perforce)
	ULONG	 ul_P4ChangeList;					// [Perforce] Changelist in which file is currently opened for edit (given by FStat query to Perforce)
	char	 asz_P4HeadAction[10];				// [Perforce] Server head action on file (given by FStat query to Perforce)
	char	 asz_P4Action[10];					// [Perforce] Server action on file (given by FStat query to Perforce)

	ULONG	 ul_TmpRevisionClient;				// [Perforce] Temporay client revision used when starting Jade to validate BF against Perforce

} BIG_tdst_FileExt;
#endif /* ACTIVE_EDITORS */

/*
 ---------------------------------------------------------------------------------------------------
    A fat description
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  BIG_tdst_FatDes_
{
    ULONG       ul_MaxFile;                     /* Real number of files in the fat */
    ULONG       ul_MaxDir;                      /* Real number of dirs in the fat */
    ULONG       ul_PosFat;                      /* Position of the fat in the bigfile */
    ULONG       ul_NextPosFat;                  /* Next fat in the big file (-1 if not) */
    BIG_INDEX   ul_FirstIndex;                  /* First valid index of the fat */
    BIG_INDEX   ul_LastIndex;                   /* The last valid index in the fat */
} BIG_tdst_FatDes;

/*
 ---------------------------------------------------------------------------------------------------
    A bigfile description.
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  BIG_tdst_BigFile_
{
#ifdef JADEFUSION
	struct st_ToSave_
#else
	struct
#endif
	{
        char        ac_Def[4];                  /* To receive bigfile id */
        ULONG       ul_Version;                 /* Version of the bigfile */
        ULONG       ul_MaxFile;                 /* Number of the last file index */
        ULONG       ul_MaxDir;                  /* Number of the last dir index */
        ULONG       ul_MaxKey;                  /* Max key in table */
        BIG_INDEX   ul_Root;                    /* The root dir */
        BIG_INDEX   ul_FirstFreeFile;           /* The first free file */
        BIG_INDEX   ul_FirstFreeDir;            /* The first free dir */
        ULONG       ul_SizeOfFat;               /* Size of fat */
        ULONG       ul_NumFat;                  /* Number of fat in that bigfile */
        BIG_KEY     ul_UniverseKey;             /* key of universe AI */
    } st_ToSave;

    char                asz_Name[L_MAX_PATH];   /* Complete name of the bigfile */
    L_FILE              h_CLibFileHandle;       /* Real file handle. NULL if invalid bigfile */
#ifndef ACTIVE_EDITORS
    BAS_tdst_barray     st_KeyTableToPos;       /* Key table for loading */
#endif
    BIG_tdst_File       *dst_FileTable;         /* Minimal fat is kept loaded in editor mode */
    BIG_tdst_FatDes     *dst_FatTable;          /* All fats in the bigfile */

#ifdef ACTIVE_EDITORS
    BAS_tdst_barray     st_KeyTableToFat;       /* Key table for editor loading */
	BAS_tdst_barray     st_PosTableToFat;		/* Pos table to fat index for editor */
    BIG_tdst_FileExt    *dst_FileTableExt;      /* Array of all extended files */
    BIG_tdst_Directory  *dst_DirTable;          /* Array of all directories */
#endif /* ACTIVE_EDITORS */
}BIG_tdst_BigFile;

#ifdef ACTIVE_EDITORS
extern BOOL BIG_b_CheckName(char *);
extern void BIG_ComputeFullName(BIG_INDEX, char *);
extern BOOL BIG_b_IsFileExtension(BIG_INDEX, char *);
extern BOOL BIG_b_IsFileExtensionIn(BIG_INDEX, char *);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __BIGDEFS_H__ */

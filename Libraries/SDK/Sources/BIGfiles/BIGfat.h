/*$T BIGfat.h GC!1.38 05/28/99 14:54:45 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGopen.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 ---------------------------------------------------------------------------------------------------
    Macros to access to fat
 ---------------------------------------------------------------------------------------------------
 */

#define BIG_Version()           (BIG_gst.st_ToSave.ul_Version)
#define BIG_Root()              (BIG_gst.st_ToSave.ul_Root)
#define BIG_MaxFile()           (BIG_gst.st_ToSave.ul_MaxFile)
#define BIG_MaxDir()            (BIG_gst.st_ToSave.ul_MaxDir)
#define BIG_MaxKey()            (BIG_gst.st_ToSave.ul_MaxKey)
#define BIG_FreeFile()          (BIG_gst.st_ToSave.ul_FirstFreeFile)
#define BIG_FreeDir()           (BIG_gst.st_ToSave.ul_FirstFreeDir)
#define BIG_SizeFat()           (BIG_gst.st_ToSave.ul_SizeOfFat)
#define BIG_NumFat()            (BIG_gst.st_ToSave.ul_NumFat)
#define BIG_UniverseKey()       (BIG_gst.st_ToSave.ul_UniverseKey)
#define BIG_Handle()            (BIG_gst.h_CLibFileHandle)

#define BIG_FirstFile(d)        (BIG_gst.dst_DirTable[d].st_ToSave.ul_FirstFile)
#define BIG_SubDir(d)           (BIG_gst.dst_DirTable[d].st_ToSave.ul_FirstSubDir)
#define BIG_ParentDir(d)        (BIG_gst.dst_DirTable[d].st_ToSave.ul_Parent)
#define BIG_NameDir(d)          (BIG_gst.dst_DirTable[d].st_ToSave.asz_Name)
#define BIG_PrevDir(d)          (BIG_gst.dst_DirTable[d].st_ToSave.ul_Prev)
#define BIG_NextDir(d)          (BIG_gst.dst_DirTable[d].st_ToSave.ul_Next)
#define BIG_DirChanged(d)       (BIG_gst.dst_DirTable[d].mc_HasChanged)

#define BIG_PosFile(d)          (BIG_gst.dst_FileTable[d].ul_Pos)
#define BIG_FileKey(d)          (BIG_gst.dst_FileTable[d].ul_Key)

#define BIG_TimeFile(d)         (BIG_gst.dst_FileTableExt[d].st_ToSave.x_Time)
#define BIG_NameFile(d)         (BIG_gst.dst_FileTableExt[d].st_ToSave.asz_Name)
#define BIG_ParentFile(d)       (BIG_gst.dst_FileTableExt[d].st_ToSave.ul_Parent)
#define BIG_PrevFile(d)         (BIG_gst.dst_FileTableExt[d].st_ToSave.ul_Prev)
#define BIG_NextFile(d)         (BIG_gst.dst_FileTableExt[d].st_ToSave.ul_Next)
#define BIG_LengthDiskFile(d)   (BIG_gst.dst_FileTableExt[d].st_ToSave.ul_LengthOnDisk)
#define BIG_FileChanged(d)      (BIG_gst.dst_FileTableExt[d].c_HasChanged)

#define BIG_P4OwnerName(d)        (BIG_gst.dst_FileTableExt[d].asz_P4OwnerName)
#define BIG_P4RevisionServer(d)   (BIG_gst.dst_FileTableExt[d].ul_P4RevisionServer)
#define BIG_P4RevisionClient(d)   (BIG_gst.dst_FileTableExt[d].st_ToSave.ul_P4RevisionClient)
#define BIG_P4Time(d)			  (BIG_gst.dst_FileTableExt[d].x_P4Time)	
#define BIG_P4ChangeList(d)		  (BIG_gst.dst_FileTableExt[d].ul_P4ChangeList)	
#define BIG_P4HeadAction(d)		  (BIG_gst.dst_FileTableExt[d].asz_P4HeadAction)	
#define BIG_P4Action(d)			  (BIG_gst.dst_FileTableExt[d].asz_P4Action)	

#define BIG_TmpRevisionClient(d)  (BIG_gst.dst_FileTableExt[d].ul_TmpRevisionClient)
/*$2
 ---------------------------------------------------------------------------------------------------
    Constants
 ---------------------------------------------------------------------------------------------------
 */

#define BIG_MAX_FAT     (LONG)10000

/*$2
 ---------------------------------------------------------------------------------------------------
    Macros
 ---------------------------------------------------------------------------------------------------
 */

/*$off*/
#define BIG_PosFatFile      (0)
#define BIG_PosFatFileExt   (BIG_PosFatFile + (BIG_SizeFat() * sizeof(BIG_tdst_File)))
#define BIG_PosFatDir       (BIG_PosFatFileExt + (BIG_SizeFat() * sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave)))
/*$on*/

/*$2
 ---------------------------------------------------------------------------------------------------
    Functions
 ---------------------------------------------------------------------------------------------------
 */

extern void         BIG_ReadHeader(void);
extern void         BIG_ReadAllFats(void);
extern void         BIG_ResetFat(void);

#ifdef ACTIVE_EDITORS
extern void         BIG_ReadFatFileExt(BIG_tdst_FatDes *);
extern void         BIG_ReadFatDir(BIG_tdst_FatDes *);

extern void         BIG_WriteHeader(void);
extern void         BIG_UpdateOneFileInFat(BIG_INDEX);
extern void         BIG_UpdateOneDirInFat(BIG_INDEX);

extern BIG_INDEX    BIG_ul_SearchFile(BIG_INDEX, const char *);
extern BIG_INDEX    BIG_ul_SearchDir( const char *);
extern BIG_INDEX    BIG_ul_SearchDirInDir(BIG_INDEX, const char *);
extern BIG_INDEX    BIG_ul_SearchFileExt(const char *, const char *);
extern BIG_INDEX	BIG_ul_SearchFileInDirRec(BIG_INDEX, const char *);
extern BOOL         BIG_b_IsFileInDirRec(BIG_INDEX, BIG_INDEX );
extern BOOL         BIG_b_IsDirInDirRec(BIG_INDEX, BIG_INDEX );

extern BIG_INDEX    BIG_ul_GetFreePos(ULONG, ULONG *, BIG_INDEX *);
extern BIG_INDEX    BIG_ul_AddFileInTable(ULONG, ULONG *, ULONG *, BOOL);
extern BIG_INDEX    BIG_ul_AddDirInTable(void);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

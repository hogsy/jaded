/*$T BIGmerge.c GC!1.5 10/04/99 15:28:55 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "EDItors/Sources/BROwser/BROmsg.h"

/*$4
 ***************************************************************************************************
    Global variables
 ***************************************************************************************************
 */

BIG_tdst_BigFile    BIG_gst1;						/* The dummy bigfile to permit 2 open bigfiles at the same time */
BIG_tdst_BigFile    BIG_gst2;
BIG_tdst_BigFile    BIG_gst3;
BIG_tdst_BigFile    BIG_gst4;
BOOL                BIG_gb_NotCurrent;				/* To tell that current valid is in fact dummy one */
BIG_KEY             BIG_gul_GlobalKey;				/* To remember a key value */
L_time_t            BIG_gx_GlobalTime;				/* To remember date of file */
ULONG				BIG_gx_GlobalClientRev;			/* To remember client revision of file */
char                *BIG_gpsz_GlobalName;			/* To remember source name */
BOOL				BIG_gb_NoMerge = FALSE;
BOOL				BIG_gb_AlignFile = FALSE;

BOOL                BIG_gb_MergeRecurse = TRUE;
ULONG				BIG_ul_MaxSizeFile = 0;
BOOL			    EDI_gi_SoundTruncate = 0;
BOOL			    EDI_gi_UseIdKeyFilter = 0;
BAS_tdst_barray     EDI_gst_IdKeyFilter;

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
void BIG_CallbackLoadFileFromBigFile(BIG_INDEX _ul_Dir, BIG_INDEX _ul_File)
{
	ULONG	ul_Length;

    /* Remember key of source file */
    BIG_gul_GlobalKey = BIG_FileKey(_ul_File);
    BIG_gx_GlobalTime = BIG_TimeFile(_ul_File);
	BIG_gpsz_GlobalName = BIG_NameFile(_ul_File);
	BIG_gx_GlobalClientRev = BIG_P4RevisionClient(_ul_File);

    BIG_gp_Buffer = NULL;

    /* Read the file in the buffer */
	if(BIG_gb_NoMerge)
	{
		BIG_gp_Buffer = &BIG_gb_NoMerge;
		BIG_gul_Length = 0;
	}
	else
	{
		BIG_gp_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(_ul_File), &BIG_gul_Length);

		/* Max size for content (for sound checkin) */
		if(BIG_ul_MaxSizeFile && BIG_gul_Length > BIG_ul_MaxSizeFile)
		{
			BIG_gul_Length = BIG_ul_MaxSizeFile;
			BIG_ul_MaxSizeFile = 0;
		}

		/* Alignement to sector boundary */
		if(BIG_gb_AlignFile)
		{
			ul_Length = BIG_gul_Length;
			BIG_gul_Length += 2048;
			BIG_gul_Length &= 0xFFFFF800;
			BIG_gul_Length -= 4;
			if(BIG_gul_Length < ul_Length) BIG_gul_Length += 2048;
			L_memset((char *) BIG_gp_Buffer + ul_Length, 0, BIG_gul_Length - ul_Length);
		}
	}

    /* Src bigfile became dest one... */
    SWAP_BIG();
}


/*
 ===================================================================================================
 ===================================================================================================
 */

void BIG_ExportMergeDirRec(char *_psz_BigPathName, BIG_INDEX _ul_Index)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Index;
    char        asz_DestName[BIG_C_MaxLenPath];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    LINK_PrintStatusMsg(BIG_NameDir(_ul_Index));
    if(LINK_gb_EscapeDetected) return;

    ul_Index = BIG_FirstFile(_ul_Index);
    while(ul_Index != BIG_C_InvalidIndex)
    {
        if(LINK_gb_EscapeDetected) return;

        if( BRO_b_FileIsForced(ul_Index) || !BRO_b_FileIsExcluded(ul_Index) )
        {
            BRO_AutoCheckOutFile(_psz_BigPathName, BIG_NameFile(ul_Index));
            BIG_ul_ExportMergeFileRec(_psz_BigPathName, BIG_NameFile(ul_Index));
        }

        ul_Index = BIG_NextFile(ul_Index);
    }

    /* Recurse call for all subdir */
    if(BIG_gb_MergeRecurse)
    {
        ul_Index = BIG_SubDir(_ul_Index);
        while(ul_Index != BIG_C_InvalidIndex)
        {
            /* Construct full path name */
            L_strcpy(asz_DestName, _psz_BigPathName);
            L_strcat(asz_DestName, "/");
            L_strcat(asz_DestName, BIG_NameDir(ul_Index));

            /* Recurse call */
            BIG_ExportMergeDirRec(asz_DestName, ul_Index);
            if(LINK_gb_EscapeDetected) return;

            ul_Index = BIG_NextDir(ul_Index);
        }
    }
}

/*
 ===================================================================================================
    Aim:    To merge a complete folder of the current bigfile with a destination one.

    In:     _psz_DestName       Name of the bigfile we wanted to merge with.
            _psz_BigPathName    Full name of the path we wanted to merge in the current bigfile.
 ===================================================================================================
 */
void BIG_ExportMergeDir(char *_psz_DestName, char *_psz_BigPathName)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Index;
    char        asz_Name[L_MAX_PATH];
    char        *psz_Temp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
    COPY_BIG();

    ul_Index = BIG_ul_SearchDir(_psz_BigPathName);

    /* Create dest bigfile if it does not exist */
    L_strcpy(asz_Name, _psz_DestName);
    if(L_access(_psz_DestName, 0))
    {
        psz_Temp = L_strrchr(asz_Name, '.');
        if(!psz_Temp || L_strcmpi(psz_Temp, BIG_Csz_BigExt))
            L_strcat(asz_Name, BIG_Csz_BigExt);
        if(L_access(asz_Name, 0))
            BIG_CreateEmpty(asz_Name);
    }

    /* Open it */
    BIG_Open(asz_Name);
    SWAP_BIG();

    BIG_ExportMergeDirRec(_psz_BigPathName, ul_Index);

    SWAP_BIG();
    BIG_Close();

_Catch_
_End_
    RESTORE_BIG();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BIG_INDEX BIG_ul_ExportMergeFileRec(char *_psz_BigPathName, char *_psz_BigFileName)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_Index = BIG_ul_UpdateCreateFile
        (
            _psz_BigPathName,
            _psz_BigFileName,
            BIG_CallbackLoadFileFromBigFile,
            FALSE
        );

    SWAP_BIG();
    return ul_Index;
}

/*
 ===================================================================================================
    Aim:    To merge a file of the current bigfile with another bigfile.

    In:     _psz_DestName       Name of the destination bigfile to merge with.
            _psz_BigPathName    Path of the file we wanted to merge (path in current bigfile).
            _psz_BigFileName    Name of the file we wanted to merge.
 ===================================================================================================
 */
BIG_INDEX BIG_ul_ExportMergeFile
(
    char    *_psz_DestName,
    char    *_psz_BigPathName,
    char    *_psz_BigFileName
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Index;
    char        asz_Name[L_MAX_PATH];
	char		*psz_Temp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_Index = BIG_C_InvalidIndex;

_Try_
    COPY_BIG();

    /* Create dest bigfile if it does not exist */
    L_strcpy(asz_Name, _psz_DestName);
    if(L_access(_psz_DestName, 0))
	{
        psz_Temp = L_strrchr(asz_Name, '.');
        if(!psz_Temp || L_strcmpi(psz_Temp, BIG_Csz_BigExt))
            L_strcat(asz_Name, BIG_Csz_BigExt);
        if(L_access(asz_Name, 0))
            BIG_CreateEmpty(asz_Name);
	}

    /* Open it */
    BIG_Open(asz_Name);
    SWAP_BIG();

    ul_Index = BIG_ul_ExportMergeFileRec(_psz_BigPathName, _psz_BigFileName);

    SWAP_BIG();
    BIG_Close();

_Catch_
_End_
    RESTORE_BIG();

    return ul_Index;
}

#endif /* ACTIVE_EDITORS */

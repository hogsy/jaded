/*$T BIGexport.c GC!1.20 04/01/99 14:36:50 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGio.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGread.h"

/* If TRUE, s_CreateDirFromDisk will be recursive */
BOOL    BIG_gb_ExportRecurse = TRUE;

static void BIG_CheckWindowsFileName(char*_pz)
{
    char asz_Prohibit[] = "\\/:*?\"<>|";
    char asz_OldName[L_MAX_PATH+64];
    int i = 0;
    BOOL b_NameHasChanged = FALSE;


    while(_pz[i])
    {
        if(L_strchr(asz_Prohibit, _pz[i]))
        {
            if(!b_NameHasChanged) sprintf(asz_OldName,"The file name contains bad caracters : %s, we renamed it.", _pz);
            b_NameHasChanged = TRUE;
            _pz[i] = '_';
        }
        i++;
    }

    ERR_X_Warning(b_NameHasChanged==FALSE, asz_OldName, NULL);
}
/*
 ===================================================================================================
 ===================================================================================================
 */
static void s_ExportDir(char *_psz_RealName, char *_psz_BigPathName, BIG_INDEX _ul_Index)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_SubDir, ul_FileIndex;
    ULONG       ul_Length;
    char        asz_TmpName[BIG_C_MaxLenPath];
    char        asz_SrcName[BIG_C_MaxLenPath];
    char        asz_DestName[BIG_C_MaxLenPath];
    void        *p_Buffer;
    L_FILE      h_TmpFile;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    h_TmpFile = NULL;
    

_Try_
    LINK_PrintStatusMsg(BIG_NameDir(_ul_Index));
    if(LINK_gb_EscapeDetected) return;

    /* Create dir if it does not exist yet */
    if(L_access(_psz_RealName, 0))
        ERR_X_Error(L_mkdir(_psz_RealName) == 0, L_ERR_Csz_MkDir, _psz_RealName);

    /* Parse all subdirs. */
    if(BIG_gb_ExportRecurse)
    {
        ul_SubDir = BIG_SubDir(_ul_Index);
        while(ul_SubDir != BIG_C_InvalidIndex)
        {
            L_strcpy(asz_SrcName, _psz_BigPathName);
            L_strcat(asz_SrcName, "/");
            L_strcat(asz_SrcName, BIG_NameDir(ul_SubDir));

            L_strcpy(asz_DestName, _psz_RealName);
            L_strcat(asz_DestName, "/");
            L_strcat(asz_DestName, BIG_NameDir(ul_SubDir));

            s_ExportDir(asz_DestName, asz_SrcName, ul_SubDir);
            if(LINK_gb_EscapeDetected) return;

            /* Pass to brother (dir) */
            ul_SubDir = BIG_NextDir(ul_SubDir);
        }
    }

    /* Create all files. */
    ul_FileIndex = BIG_FirstFile(_ul_Index);
    while(ul_FileIndex != BIG_C_InvalidIndex)
    {
        /* Print msg */
        if(LINK_gb_EscapeDetected) return;
        

        /* Construct name */
        L_strcpy(asz_DestName, _psz_RealName);
        L_strcat(asz_DestName, "/");
        L_strcpy(asz_TmpName,  BIG_NameFile(ul_FileIndex));
        BIG_CheckWindowsFileName(asz_TmpName);
        L_strcat(asz_DestName, asz_TmpName);

        /* Open file to write */
        h_TmpFile = L_fopen(asz_DestName, L_fopen_WB);
        ERR_X_Error(CLI_FileOpen(h_TmpFile), L_ERR_Csz_FOpen, asz_DestName);

        /* Write file */
        p_Buffer = (void *) BIG_pc_ReadFileTmp(BIG_PosFile(ul_FileIndex), &ul_Length);
        ERR_X_Error(BIG_fwrite(p_Buffer, ul_Length, h_TmpFile) == 1, L_ERR_Csz_FWrite, NULL);

        /* Close file */
        ERR_X_Error(L_fclose(h_TmpFile) == 0, L_ERR_Csz_FClose, asz_DestName);

        /* Pass to brother */
        ul_FileIndex = BIG_NextFile(ul_FileIndex);
    }

_Catch_
    if(CLI_FileOpen(h_TmpFile)) L_fclose(h_TmpFile);
_EndThrow_
}



/*
 ===================================================================================================
 ===================================================================================================
 */
void BIG_ExportFileToDisk(char *_psz_RealName, char *_psz_BigPathName, char *_psz_BigFileName)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Index;
    ULONG       ul_Length;
    L_FILE      h_TmpFile;
    void        *p_Buffer;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    h_TmpFile = NULL;
    p_Buffer = NULL;

_Try_
    /* Retrieve file */
    ul_Index = BIG_ul_SearchFileExt(_psz_BigPathName, _psz_BigFileName);
    ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

    /* Open file to write */
    h_TmpFile = L_fopen(_psz_RealName, L_fopen_WB);
    ERR_X_Error(CLI_FileOpen(h_TmpFile), L_ERR_Csz_FOpen, _psz_RealName);

    /* Write file */
    p_Buffer = (void *) BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), &ul_Length);
    ERR_X_Error(BIG_fwrite(p_Buffer, ul_Length, h_TmpFile) == 1, L_ERR_Csz_FWrite, NULL);

    /* Close file */
    ERR_X_Error(L_fclose(h_TmpFile) == 0, L_ERR_Csz_FClose, NULL);

_Catch_
    if(CLI_FileOpen(h_TmpFile)) L_fclose(h_TmpFile);
_EndThrow_
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void BIG_ExportDirToDisk(char *_psz_RealName, char *_psz_BigPathName)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   mul_DirIndex;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    mul_DirIndex = BIG_ul_SearchDir(_psz_BigPathName);
    ERR_X_Assert(mul_DirIndex != BIG_C_InvalidIndex);
    s_ExportDir(_psz_RealName, _psz_BigPathName, mul_DirIndex);
}

#endif /* ACTIVE_EDITORS */

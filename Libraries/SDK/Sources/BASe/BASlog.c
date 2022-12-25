/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#ifndef PSX2_TARGET
/* this file isn't used with PSX2 target */


#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGio.h"

/*
 ===================================================================================================
    Aim:    To open a log file to write. If file is read only, it is forced to be write enable.

    In:     _psz_NameLog    Complete name (with path) of log file to open. 

    Out:    A L_FILE handle to the open log file.
 ===================================================================================================
 */
static L_FILE sh_OpenLog(char *_psz_NameLog)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_FILE  h_LogFile;
    BOOL    b_Create;
    char    asz_Temp[255];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    h_LogFile = NULL;
    b_Create = FALSE;

_Try_
    if(!(L_access(_psz_NameLog, 0)))
    {
        ERR_X_Error(L_chmod(_psz_NameLog, L_S_IREAD | L_S_IWRITE) != -1, L_ERR_Csz_ChMode, NULL);
    }
    else
    {
        b_Create = TRUE;
    }

    h_LogFile = L_fopen(_psz_NameLog, L_fopen_AB);
	if(!CLI_FileOpen(h_LogFile)) return NULL;

    /* Write current version of exe. */
    if(b_Create)
    {
        L_strcpy(asz_Temp, "EXE [");
        L_strcat(asz_Temp, __DATE__);
        L_strcat(asz_Temp, "  ");
        L_strcat(asz_Temp, __TIME__);
        L_strcat(asz_Temp, "]\n\n");

        ERR_X_Error
        (
            L_fwrite(asz_Temp, L_strlen(asz_Temp), 1, h_LogFile) == 1,
            L_ERR_Csz_FWrite,
            NULL
        );
    }

_Catch_
    if(CLI_FileOpen(h_LogFile))
    {
        L_fclose(h_LogFile);
    }

_EndThrow_
    return(h_LogFile);
}

/*
 ===================================================================================================
    Aim:    To write in a log file current date and time.

    In:     _h_Log  Handle of log file.
 ===================================================================================================
 */
static void s_WriteDateTime(L_FILE _h_Log)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Temp[50];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Date */
    L_strdate(asz_Temp);
    L_strcat(asz_Temp, "  ");
    ERR_X_Error(L_fwrite(asz_Temp, L_strlen(asz_Temp), 1, _h_Log) == 1, L_ERR_Csz_FWrite, NULL);

    /* Time */
    L_strtime(asz_Temp);
    L_strcat(asz_Temp, "  ");
    ERR_X_Error(L_fwrite(asz_Temp, L_strlen(asz_Temp), 1, _h_Log) == 1, L_ERR_Csz_FWrite, NULL);
}


/*
 ===================================================================================================
    Aim:    To update a log file with a string. The string is preceded with the current date and
            time.

    In:     _psz_NameLog    Complete name of log file to update. 
            _psz_String     String to add in the log file. Can be NULL to ignore the call. Can be
                            an empty string. 
            _uc_DateTime    0 to write nothing (except string of course). 1 to write date and time.
                            2 to only write separator with good indentation.
 ===================================================================================================
 */
void BAS_UpdateLog(char *_psz_NameLog, char *_psz_String, UCHAR _uc_DateTime)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_FILE      h_LogFile;
    char        asz_Temp[50];
    static BOOL sb_Lock;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    h_LogFile = NULL;
    sb_Lock = FALSE;

_Try_
    if(_psz_String == NULL)
    {
        _Return_(;);
    }

    /* To avoid function to call itself */
    if(sb_Lock == TRUE)
    {
        _Return_(;);
    }
    else
    {
        sb_Lock = TRUE;
    }

    /* Open log file */
    h_LogFile = sh_OpenLog(_psz_NameLog);
	if(!CLI_FileOpen(h_LogFile)) return;

    /* Write date and time */
    if(_uc_DateTime == 1)
    {
        s_WriteDateTime(h_LogFile);
    }

    /* Mark */
    if(_uc_DateTime != 0)
    {
        *asz_Temp = '\0';
        if(_uc_DateTime == 2)
        {
            L_strcpy(asz_Temp, "                    ");
        }

        /* Size of date + time */
        L_strcat(asz_Temp, "***  ");
        ERR_X_Error
        (
            L_fwrite(asz_Temp, L_strlen(asz_Temp), 1, h_LogFile) == 1,
            L_ERR_Csz_FWrite,
            _psz_NameLog
        );
    }

    /* Write string */
    if(*_psz_String != '\0')
    {
        ERR_X_Error
        (
            L_fwrite(_psz_String, L_strlen(_psz_String), 1, h_LogFile) == 1,
            L_ERR_Csz_FWrite,
            _psz_NameLog
        );
    }

    /* EOL */
    L_strcpy(asz_Temp, "\n");
    ERR_X_Error
    (
        L_fwrite(asz_Temp, L_strlen(asz_Temp), 1, h_LogFile) == 1,
        L_ERR_Csz_FWrite,
        _psz_NameLog
    );

    /* Close file */
    ERR_X_Error(L_fclose(h_LogFile) == 0, L_ERR_Csz_FClose, _psz_NameLog);

_Catch_
    if(CLI_FileOpen(h_LogFile))
    {
        L_fclose(h_LogFile);
    }

_End_

    /* Unlock recurse calls */
    sb_Lock = FALSE;
}


/*
 ===================================================================================================
    Aim:    To update a log file with a string. The string is preceeded with the current date and
            time. Take care of source file and position. This function can be used for assertion to
            know the place in the source file where it has occured.

    In:     _psz_NameLog        Complete name of log file to update. 
            _psz_SourceFile     Name of source file to update in log. 
            _ul_Line            Line in source file. 
            _psz_String         String to add in the log file.
 ===================================================================================================
 */
void BAS_UpdateLogSourceFile
(
    char    *_psz_NameLog,
    char    *_psz_SourceFile,
    ULONG   _ul_Line,
    char    *_psz_String
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_FILE      h_LogFile;
    char        asz_Temp[255];
    static BOOL sb_Lock;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    h_LogFile = NULL;
    sb_Lock = FALSE;

_Try_

    /* To avoid function to call itself */
    if(sb_Lock == TRUE)
    {
        _Return_(;);
    }
    else
    {
        sb_Lock = TRUE;
    }

    /* Open log file */
    h_LogFile = sh_OpenLog(_psz_NameLog);

    /* Write date and time */
    s_WriteDateTime(h_LogFile);

    /* Mark */
    L_strcpy(asz_Temp, "***  ");
    ERR_X_Error
    (
        L_fwrite(asz_Temp, L_strlen(asz_Temp), 1, h_LogFile) == 1,
        L_ERR_Csz_FWrite,
        _psz_NameLog
    );

    /* Write file */
    ERR_X_Error
    (
        L_fwrite(_psz_SourceFile, L_strlen(_psz_SourceFile), 1, h_LogFile) == 1,
        L_ERR_Csz_FWrite,
        _psz_NameLog
    );

    /* Write line */
    L_strcpy(asz_Temp, " -- Line ");
    L_ltoa(_ul_Line, asz_Temp + L_strlen(asz_Temp), 10);
    ERR_X_Error
    (
        L_fwrite(asz_Temp, L_strlen(asz_Temp), 1, h_LogFile) == 1,
        L_ERR_Csz_FWrite,
        _psz_NameLog
    );

    /* Write string */
    L_strcpy(asz_Temp, " -- ");
    L_strcat(asz_Temp, _psz_String);
    ERR_X_Error
    (
        L_fwrite(asz_Temp, L_strlen(asz_Temp), 1, h_LogFile) == 1,
        L_ERR_Csz_FWrite,
        _psz_NameLog
    );

    /* EOL */
    L_strcpy(asz_Temp, "\n");
    ERR_X_Error
    (
        L_fwrite(asz_Temp, L_strlen(asz_Temp), 1, h_LogFile) == 1,
        L_ERR_Csz_FWrite,
        _psz_NameLog
    );

    /* Close file */
    ERR_X_Error(L_fclose(h_LogFile) == 0, L_ERR_Csz_FClose, _psz_NameLog);

_Catch_
    if(CLI_FileOpen(h_LogFile))
    {
        L_fclose(h_LogFile);
    }

_End_

    /* Unlock recurse calls */
    sb_Lock = FALSE;
}

#endif /* not used if PSX2 */
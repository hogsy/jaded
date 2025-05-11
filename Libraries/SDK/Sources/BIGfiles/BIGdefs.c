/*$T BIGdefs.c GC!1.59 12/21/99 15:02:51 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGerrid.h"
#include "BIGfiles/BIGdefs.h"

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Check if a name for a dir or a file is correct. For example, a dir/ file name can't
            possess the '/' character. An error is raised if name is incorrect.

    In:     _psz_Name   Name of file or dir to check. 

    Out:    TRUE if name is correct, FALSE else.
 =======================================================================================================================
 */
BOOL BIG_b_CheckName(char *_psz_Name)
{
    if((_psz_Name == NULL) || (*_psz_Name == 0) || (strchr(_psz_Name, '/')) || (strchr(_psz_Name, '\\')))
    {
        ERR_X_ForceError(ERR_BIG_Csz_InvalidName, _psz_Name);
        return FALSE;
    }

    return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Compute the full path name of a dir.

    In:     _ul_Dir     Index in fat of dir to compute full path.
            _psz_Ext    Return full path string.
 =======================================================================================================================
 */
void BIG_ComputeFullName(BIG_INDEX _ul_Dir, char *_psz_Ext)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *mpsz_Name;
    char    asz_Name[BIG_C_MaxLenPath];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if !defined(XML_CONV_TOOL)
    L_strcpy(_psz_Ext, BIG_NameDir(_ul_Dir));
    _ul_Dir = BIG_ParentDir(_ul_Dir);
    while(_ul_Dir != BIG_C_InvalidIndex)
    {
        mpsz_Name = BIG_NameDir(_ul_Dir);
		snprintf( asz_Name, sizeof( asz_Name ), "%s/%s", mpsz_Name, _psz_Ext );
        L_strcpy(_psz_Ext, asz_Name);
        _ul_Dir = BIG_ParentDir(_ul_Dir);
    }
#endif // XML_CONV_TOOL
}

/*
 =======================================================================================================================
    Aim:    Determin if a file has a given extension.
 =======================================================================================================================
 */
BOOL BIG_b_IsFileExtension(BIG_INDEX _ul_File, char *_psz_Ext)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Name[BIG_C_MaxLenName];
    char    *psz_Temp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_ul_File == BIG_C_InvalidIndex) return FALSE;
    L_strcpy(asz_Name, BIG_NameFile(_ul_File));
    psz_Temp = L_strrchr(asz_Name, '.');
    if(!psz_Temp) return FALSE;
    if(!L_strnicmp(psz_Temp, _psz_Ext, L_strlen(_psz_Ext))) return TRUE;
    return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Determin if a file extension is a filter extension string filter extension string is
            composed with several extension (such as .gao) separated with null character and
            terminated with two nulml character here is a sample:: ".gro/0.zob/0" if filter
            extension string is NULL or Empty function return always true
 =======================================================================================================================
 */
BOOL BIG_b_IsFileExtensionIn(BIG_INDEX _ul_File, char *_psz_Ext)
{
    /*~~~~~~~~~~~~~~~~~~~~*/
    char    *psz_Temp;
    char    *psz_CurExt;
    /*~~~~~~~~~~~~~~~~~~~~*/

    if(_ul_File == BIG_C_InvalidIndex) return FALSE;
    if((!_psz_Ext) || (!(*_psz_Ext))) return TRUE;

    psz_Temp = L_strrchr(BIG_NameFile(_ul_File), '.');
    if(!psz_Temp) return FALSE;

    psz_CurExt = _psz_Ext;
    while(*psz_CurExt != 0)
    {
        if(!L_strnicmp(psz_Temp, psz_CurExt, L_strlen(psz_CurExt))) return TRUE;
        psz_CurExt += L_strlen(psz_CurExt) + 1;
    }

    return FALSE;
}

#endif

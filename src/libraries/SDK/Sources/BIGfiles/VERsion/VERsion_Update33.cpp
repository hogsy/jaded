/*$T VERsion_Update5.cpp GC!1.5 11/12/99 15:31:36 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/VERsion/VERsion_Update.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGkey.h"

/*
 ===================================================================================================
    Aim:    Update bigfile from version 32 to Version 33
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion33(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Extension of file */
    char    *psz_ext;
    ULONG   ul_StatusFlags;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Find extension of file */
    psz_ext = strrchr(_psz_File, '.');

    /* Update Gao */
    if(psz_ext && !strcmp(psz_ext, EDI_Csz_ExtGameObject))
    {
        ul_StatusFlags = *(ULONG *) (_pc_Buf + 16);   /* 4 : Editor Flag + 4 Version + 4 Extension  + 4 Identity Flags */
        ul_StatusFlags &= ~0x00000080;
        *(ULONG *) (_pc_Buf + 16) = ul_StatusFlags;

        /* Finally, we update the file */
        BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, _pc_Buf, _ul_Len);
    }
}

#endif

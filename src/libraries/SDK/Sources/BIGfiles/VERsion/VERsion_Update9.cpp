/*$T VERsion_Update6.cpp GC!1.52 11/26/99 10:43:09 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#include "BASe/BAStypes.h"
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
    Aim:    Update bigfile from version 8 to Version 9
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion9(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char            *psz_Ext, *pc_Cur;
    ULONG   l_Value;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Find extension of file */
    psz_Ext = strrchr(_psz_File, '.');
    if(!psz_Ext) return;

    /* Update texture file */
    if(L_stricmp(psz_Ext, ".grl") == 0)
    {
        pc_Cur = _pc_Buf;

        /* swap gro struct data */
        pc_Cur += 4;
        l_Value = *(LONG *) pc_Cur;
        pc_Cur += 4 + l_Value;

        /* get flag */
        l_Value = *(LONG *) pc_Cur;
        l_Value |= 0x00000320;
        *(LONG *) pc_Cur = l_Value;

        /* Finally, we update the file */
        BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, _pc_Buf, _ul_Len );
    }
}

#endif

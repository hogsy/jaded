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
    Aim:    Update bigfile from version 5 to Version 6
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion27(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char            *psz_Ext, *buf;
    LONG            *pul_Buf;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Find extension of file */
    psz_Ext = strrchr(_psz_File, '.');
    if(!psz_Ext) return;

    /* Update texture file */
    if(L_stricmp(psz_Ext, ".spr") != 0) return;

    buf = (char *) L_malloc(_ul_Len + 32);
    L_memcpy(buf, _pc_Buf, _ul_Len);

    pul_Buf = (LONG *) (buf + _ul_Len);
    pul_Buf[0] = 0x40000000;
    pul_Buf[1] = 0;
    pul_Buf[2] = 0;
    pul_Buf[3] = 0;
    pul_Buf[4] = 0;
    pul_Buf[5] = 0;
    pul_Buf[6] = 0;
    pul_Buf[7] = 0;

    /* Finally, we update the file */
    BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, buf, _ul_Len + 32);
    L_free(buf);
}

#endif

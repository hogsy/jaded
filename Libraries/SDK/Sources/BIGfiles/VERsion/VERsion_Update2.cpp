/*$T VERsion_Update2.cpp GC!1.52 11/06/99 11:22:15 */

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
 Aim: Update bigfile from version 1 to Version 2
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion2(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Destination buffer */
    char    *pc_dstBuff;

    /* Extension of file */
    char    *psz_ext;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* YLT */
    /* Find extension of file */
    psz_ext = strrchr(_psz_File, '.');

    /* Update lights */
    if(psz_ext && !strcmp(psz_ext, EDI_Csz_ExtGraphicLight))
    {
        /* We found a light to update */

        /*
         * We create a new buffer (destination buffer) with the right size (4 bytes more) than
         * original light
         */
        pc_dstBuff = (char *) L_malloc(_ul_Len + 4);

        /*
         * We copy the original light from the source buffer, add an "InvalidKey" at the end
         */
        L_memcpy(pc_dstBuff, _pc_Buf, _ul_Len);
        *(LONG *) (pc_dstBuff + _ul_Len) = BIG_C_InvalidKey;

		/* Finally, we replace the file with the destination buffer */
        BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, pc_dstBuff, _ul_Len+4);
        L_free(pc_dstBuff);
    }
}

#endif

/*$T VERsion_Update16.cpp GC!1.55 02/01/00 10:00:13 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#ifdef ACTIVE_EDITORS

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGmdfy_file.h"


/*
 =======================================================================================================================
    Aim:    Update animation file (.NIM) to add reference to a skeleton, a skin and an object table file
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion16(char *_pc_Buf, ULONG _ul_Len, char *_pz_Path, char *_pz_File)
{
    /*~~~~~~~~~~~~~~~~~*/
    char    *psz_ext;
    char    *pc_Dst;
    /*~~~~~~~~~~~~~~~~~*/

    /* Find extension of file */
    psz_ext = L_strrchr(_pz_File, '.');

    /* If this is not a animation file, nothing to do */
    if(!psz_ext || L_stricmp(psz_ext, EDI_Csz_ExtAnimation) != 0) return;

    /*
     * We insert 12 bytes to store a key for a default skeleton, a
     * key for a default skin and a key for a default object table
     */
    pc_Dst = (char *) L_malloc(_ul_Len + 12);
    L_memcpy(pc_Dst, _pc_Buf, 16);
    L_memset(pc_Dst + 16, 0, 12);    /* Insert 12 bytes equal to 0 */
    L_memcpy(pc_Dst + 16 + 12, _pc_Buf + 16, _ul_Len - 16);

    /* Finally, we update the file */
    BIG_UpdateFileFromBuffer(_pz_Path, _pz_File, pc_Dst, _ul_Len + 12);
    L_free(pc_Dst);
}

#endif /* ACTIVE_EDITORS */

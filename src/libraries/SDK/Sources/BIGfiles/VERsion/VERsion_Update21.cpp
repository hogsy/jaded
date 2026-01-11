/*$T VERsion_Update19.cpp GC! 1.075 03/06/00 15:36:23 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGmdfy_file.h"

/*
 =======================================================================================================================
 Aim: Add a 2 bytes save after uc_VisiCoeff to realign the structure.
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion21(char *_pc_Buf, ULONG _ul_Len, char *_pz_Path, char *_pz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *pz_Ext, *pc_Cur, *pc_Dst;
    ULONG   ul_IdentityFlags;
    LONG    l_Offset;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Find extension of file */
    pz_Ext = L_strrchr(_pz_File, '.');

    /* If this is not a game object file, nothing to do */
    if(!pz_Ext || L_stricmp(pz_Ext, EDI_Csz_ExtGameObject) != 0) return;

    /* Get identity flags */
    pc_Cur = _pc_Buf + 12;
    ul_IdentityFlags = *(ULONG *) (pc_Cur); /* 4 : Editor Flag + 4 Version + 4 Extension */
	pc_Cur += 4;

	// Skip Control Flags(4) + uc_ActCoeff(1) + uc_VisiCoeff(1) 
    pc_Cur += 6;

    l_Offset = pc_Cur - _pc_Buf;

    if((ULONG) l_Offset >= _ul_Len) return;

    pc_Dst = (char *) L_malloc(_ul_Len + 2); // We add 2 BIG_KEYs
    L_memcpy(pc_Dst, _pc_Buf, l_Offset);
    *(char *) (pc_Dst + l_Offset + 0) = 0;
    *(char *) (pc_Dst + l_Offset + 1) = 0;


    L_memcpy(pc_Dst + l_Offset + 2, pc_Cur, _ul_Len - l_Offset);

    /* Finally, we update the file */
    BIG_UpdateFileFromBuffer(_pz_Path, _pz_File, pc_Dst, _ul_Len + 2);
    L_free(pc_Dst);
}

#endif /* ACTIVE_EDITORS */

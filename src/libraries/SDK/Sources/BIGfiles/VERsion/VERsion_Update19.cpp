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
    Un commentaire juste pour cet abruti de lhullier
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion19(char *_pc_Buf, ULONG _ul_Len, char *_pz_Path, char *_pz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *pz_Ext, *pc_Cur, *pc_Dst;
    ULONG   ul_IdentityFlags, ul_Key;
    LONG    l_Offset;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Find extension of file */
    pz_Ext = L_strrchr(_pz_File, '.');

    /* If this is not a game object file, nothing to do */
    if(!pz_Ext || L_stricmp(pz_Ext, EDI_Csz_ExtGameObject) != 0) return;

    /* Get identity flags */
    pc_Cur = _pc_Buf + 12;
    ul_IdentityFlags = *(ULONG *) (pc_Cur); /* 4 : Editor Flag + 4 Version + 4 Extension */

    /* Change only if there is an extended struct */
    if(!(ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject)) return;

    pc_Cur += 14;

    if(ul_IdentityFlags & 0x00000800 /* OBJ_C_IdentityFlag_OrientedObject */ )
        pc_Cur += sizeof(MATH_tdst_Matrix);
    else
        pc_Cur += sizeof(MATH_tdst_Vector);

    if(ul_IdentityFlags & 0x00080000 /* OBJ_C_IdentityFlag_OBBox */ )
        pc_Cur += sizeof(OBJ_tdst_ComplexBV);
    else
        pc_Cur += sizeof(OBJ_tdst_SingleBV);

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Skip base struct
     -------------------------------------------------------------------------------------------------------------------
     */

    if(ul_IdentityFlags & OBJ_C_IdentityFlag_BaseObject)
    {
        if(ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
        {
            pc_Cur += 4; /* gro */
            pc_Cur += 4; /* grm */
            pc_Cur += 4; /* draw mask */

			/* modifier */
            ul_Key = *(LONG *) pc_Cur;
			while(ul_Key != -1)
			{
				pc_Cur += 4;
				pc_Cur += 4;
				ul_Key = *(LONG *) pc_Cur;
				pc_Cur += ul_Key + 4;
				ul_Key = *(LONG *) pc_Cur;
			}
            pc_Cur += 4;

			/* rli */
            ul_Key = *(LONG *) pc_Cur;
            pc_Cur += 4;
			pc_Cur += ul_Key * 4;
        }

        if(ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		{
            pc_Cur += 4;
            pc_Cur += sizeof(MATH_tdst_Matrix);
		}

        if(ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
        {
            pc_Cur += 4;    /* Key of the anim */
            pc_Cur += 4;    /* Key of the action kit */
        }

        /* Additional matrix */
        if(ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)
        {
            ul_Key = *(LONG *) pc_Cur;
            pc_Cur += 4;
			if(!(ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer))
				pc_Cur += ul_Key * sizeof(MATH_tdst_Matrix);
        }
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        We are at the top of the extended struct: we skip until the uc_AiPrio position
     -------------------------------------------------------------------------------------------------------------------
     */

    pc_Cur += 28;

    /* We also insert 3 zero chars for 4 bytes alignment (and future savings) */
    l_Offset = pc_Cur - _pc_Buf;

    if((ULONG) l_Offset >= _ul_Len) return;

    pc_Dst = (char *) L_malloc(_ul_Len + 4);
    L_memcpy(pc_Dst, _pc_Buf, l_Offset);
    *(char *) (pc_Dst + l_Offset + 0) = 0;
    *(char *) (pc_Dst + l_Offset + 1) = 0;
    *(char *) (pc_Dst + l_Offset + 2) = 0;
    *(char *) (pc_Dst + l_Offset + 3) = 0;

    L_memcpy(pc_Dst + l_Offset + 4, pc_Cur, _ul_Len - l_Offset);

    /* Finally, we update the file */
    BIG_UpdateFileFromBuffer(_pz_Path, _pz_File, pc_Dst, _ul_Len + 4);
    L_free(pc_Dst);
}

#endif /* ACTIVE_EDITORS */

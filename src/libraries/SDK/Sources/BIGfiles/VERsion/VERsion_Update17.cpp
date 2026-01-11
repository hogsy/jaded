/*$T VERsion_Update14.cpp GC!1.52 01/07/00 16:01:23 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
 ===================================================================================================
    Aim:    Update bigfile from version 16 to Version 17 Update game object file 
            delete base structure structure dumping
            add modifier data in visu structure
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion17(char *_pc_Buf, ULONG _ul_Len, char *_pz_Path, char *_pz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *pz_Ext, *pc_Cur;
    ULONG   ul_IdentityFlags;
    LONG    sizeofMatrix = 68; /*sizeof( MATH_tdst_Matrix )*/
    LONG    sizeofVector = 12; /*sizeof( MATH_tdst_Vector )*/
    LONG    sizeofComplexBV = 52; /*sizeof(OBJ_tdst_ComplexBV)*/
    LONG    sizeofSingleBV = 28; /*sizeof(OBJ_tdst_SingleBV)*/
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Find extension of file */
    pz_Ext = L_strrchr(_pz_File, '.');

    /* If this is not a game object file, nothing to do */
    if(!pz_Ext || L_stricmp(pz_Ext, EDI_Csz_ExtGameObject) != 0) 
        return;

    /* Get identity flags */
    pc_Cur = _pc_Buf + 12;
    ul_IdentityFlags = *(ULONG *) (pc_Cur); /* 4 : Editor Flag + 4 Version + 4 Extension */

    /* Change only if there is a base struct */
    if(!(ul_IdentityFlags & 0x00001000 /*OBJ_C_IdentityFlag_BaseObject*/)) 
        return;

    pc_Cur += 14;

    if(ul_IdentityFlags & 0x00000800/* OBJ_C_IdentityFlag_OrientedObject */)
        pc_Cur += sizeofMatrix;
    else
        pc_Cur += sizeofVector;

    if(ul_IdentityFlags & 0x00080000/* OBJ_C_IdentityFlag_OBBox */)
        pc_Cur += sizeofComplexBV;
    else
        pc_Cur += sizeofSingleBV;

    /*$2
     -----------------------------------------------------------------------------------------------
        delete base structure
     -----------------------------------------------------------------------------------------------
     */
    L_memmove( pc_Cur, pc_Cur + 16 /* sizeof( OBJ_tdst_Base) */, _ul_Len - (pc_Cur + 16 - _pc_Buf) );
    _ul_Len -= 16;

    if(ul_IdentityFlags &  0x00004000 /*OBJ_C_IdentityFlag_Visu*/ )
    {
        pc_Cur += 12; /* Gro key + grm key + draw mask */
        L_memmove( pc_Cur + 4, pc_Cur, _ul_Len - (pc_Cur - _pc_Buf) );
        _ul_Len += 4;
        *(long *) pc_Cur = 0xFFFFFFFF;
    }

    /* Finally, we update the file */
    BIG_UpdateFileFromBuffer(_pz_Path, _pz_File, _pc_Buf, _ul_Len );
}

#endif /* ACTIVE_EDITORS */

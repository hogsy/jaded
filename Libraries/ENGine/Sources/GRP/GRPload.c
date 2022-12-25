/*$T GRPload.c GC!1.71 03/01/00 14:40:05 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGkey.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BASe/BAStypes.h"

#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#define _ReadLong(_a)   ReadLong((char*)_a)
#else
#define _ReadLong(_a)    *(ULONG*)_a
#endif


#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
    Aim:    Load a group
 =======================================================================================================================
 */
ULONG GRP_ul_LoadGrpCallback(ULONG _ul_PosFile)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG           l_Length;
    OBJ_tdst_Group  *pst_Group;
    ULONG           *pul_Buffer;
#ifdef ACTIVE_EDITORS
    char            sz_Tmp[255];
    BIG_INDEX       ul_Index;
    BIG_KEY         ul_Key;
    int             i_NameLength;
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Allocate a new group */
    pst_Group = (OBJ_tdst_Group *) MEM_p_Alloc(sizeof(OBJ_tdst_Group));
    L_memset(pst_Group, 0, sizeof(OBJ_tdst_Group));

    /* Find name of group */
#ifdef ACTIVE_EDITORS
    ul_Key = LOA_ul_GetCurrentKey();
    ul_Index = BIG_ul_SearchKeyToFat(ul_Key);

    /* Assign Name of file (without extension) to group */
    strcpy(sz_Tmp, BIG_NameFile(ul_Index));
    i_NameLength = L_strlen(sz_Tmp) - 4;

    pst_Group->sz_Name = (char *) MEM_p_Alloc(i_NameLength + 1);
    L_memcpy(pst_Group->sz_Name, sz_Tmp, i_NameLength);

    /* Set null end caracter */
    pst_Group->sz_Name[i_NameLength] = 0;
#endif
    pul_Buffer = (ULONG *) BIG_pc_ReadFileTmp(_ul_PosFile, &l_Length);

    /* Gol */
    LOA_MakeFileRef(LOA_ReadLong((CHAR**)&pul_Buffer), (ULONG *) &pst_Group->pst_AllObjects, OBJ_ul_World_LoadGolCallback, LOA_C_MustExists);

    /* Read flags */
    pst_Group->ul_Flags = LOA_ReadLong((CHAR **)&pul_Buffer);

	// Unused data...
	LOA_ReadLong_Ed((CHAR **)&pul_Buffer, NULL); // Ul2 
	LOA_ReadLong_Ed((CHAR **)&pul_Buffer, NULL); // Ul3 

    return (ULONG) pst_Group;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

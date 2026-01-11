/*$T WAYload.c GC!1.71 02/18/00 16:57:57 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "ENGine/Sources/WAYs/WAYstruct.h"
#include "ENGine/Sources/WAYs/WAYload.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "ENGine/Sources/WORld/WORload.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG WAY_ul_AllNetWorksCallback(ULONG _ul_PosFile)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CHAR					*pc_Buffer;
    ULONG                   l_Length;
    WAY_tdst_AllNetworks    *pst_AllNet;
    ULONG                   i;
	BIG_KEY					ul_Key;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_AllNet = (WAY_tdst_AllNetworks *) MEM_p_Alloc(sizeof(WAY_tdst_AllNetworks));
    L_memset(pst_AllNet, 0, sizeof(WAY_tdst_AllNetworks));

    /* Read file */
    pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &l_Length);
#if defined(PSX2_TARGET) && defined(__CW__)
    if(!pc_Buffer) return (ULONG)NULL;
#else
    if(!pc_Buffer) return NULL;
#endif

    /* Number of networks */
    pst_AllNet->ul_Num = LOA_ReadULong(&pc_Buffer);
    if(!pst_AllNet->ul_Num) return (ULONG) pst_AllNet;

    /* All the networks */
    pst_AllNet->ppst_AllNetworks = (WAY_tdst_Network **) MEM_p_Alloc(pst_AllNet->ul_Num * sizeof(WAY_tdst_Network *));
    for(i = 0; i < pst_AllNet->ul_Num; i++)
    {
		ul_Key = LOA_ReadULong(&pc_Buffer);
        LOA_MakeFileRef
        (
            ul_Key,
            (ULONG *) &pst_AllNet->ppst_AllNetworks[i],
            WAY_ul_NetWorkCallback,
            LOA_C_MustExists
        );
    }

    return (ULONG) pst_AllNet;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG WAY_ul_NetWorkCallback(ULONG _ul_PosFile)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    WAY_tdst_Network    *pst_NetWork;
    CHAR				*pc_Buffer;
    ULONG               l_Length;
	BIG_KEY				ul_Key;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_NetWork = (WAY_tdst_Network *) MEM_p_Alloc(sizeof(WAY_tdst_Network));
    L_memset(pst_NetWork, 0, sizeof(WAY_tdst_Network));

    /* Read file */
    pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &l_Length);
	ul_Key = LOA_ReadULong(&pc_Buffer);
    if(ul_Key && (ul_Key != BIG_C_InvalidKey))
    {
        LOA_MakeFileRef
        (
            ul_Key,
            (ULONG *) &pst_NetWork->pst_Root,
            OBJ_ul_GameObjectCallback,
            LOA_C_MustExists
        );
    }
    else
    {
        pst_NetWork->pst_Root = NULL;
    }


    /* Flags */
    pst_NetWork->ul_Flags = LOA_ReadULong(&pc_Buffer);

    return (ULONG) pst_NetWork;
}

extern BOOL WOR_gb_DoNotLoadLinkedObjects;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG WAY_ul_AllLinkListsCallback(ULONG _ul_PosFile)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    WAY_tdst_Struct *pst_Struct;
    CHAR			*pc_Buffer;
    ULONG           l_Length;
    ULONG           i, j;
    BIG_KEY         ul_Key;
    ULONG           ulVersion;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Struct = (WAY_tdst_Struct *) MEM_p_Alloc(sizeof(WAY_tdst_Struct));
    L_memset(pst_Struct, 0, sizeof(WAY_tdst_Struct));

    /* Read file */
    pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &l_Length);

    /* Number of lists (and version number in high short) */
    pst_Struct->ul_Num = LOA_ReadULong(&pc_Buffer);
    ulVersion = pst_Struct->ul_Num >> 16;
    pst_Struct->ul_Num &= 0x0000FFFF;

    /* Allocate all link lists */
    if(pst_Struct->ul_Num)
    {
        pst_Struct->pst_AllLinks = (WAY_tdst_LinkList *) MEM_p_Alloc(pst_Struct->ul_Num * sizeof(WAY_tdst_LinkList));
    }

    for(j = 0; j < pst_Struct->ul_Num; j++)
    {
        ul_Key = LOA_ReadULong(&pc_Buffer);
        LOA_MakeFileRef
        (
            ul_Key,
            (ULONG *) &pst_Struct->pst_AllLinks[j].pst_Network,
            WAY_ul_NetWorkCallback,
            LOA_C_MustExists
        );

        /* Number of links */
        pst_Struct->pst_AllLinks[j].pst_Links = NULL;
        pst_Struct->pst_AllLinks[j].ul_Num = LOA_ReadULong(&pc_Buffer);

        /* Read all links */
        if(pst_Struct->pst_AllLinks[j].ul_Num)
        {
            pst_Struct->pst_AllLinks[j].pst_Links = (WAY_tdst_Link *) MEM_p_Alloc(pst_Struct->pst_AllLinks[j].ul_Num * sizeof(WAY_tdst_Link));

            for(i = 0; i < pst_Struct->pst_AllLinks[j].ul_Num; i++)
            {
                if (ulVersion)
                {
                    pst_Struct->pst_AllLinks[j].pst_Links[i].uw_CapacitiesInit =
                    pst_Struct->pst_AllLinks[j].pst_Links[i].uw_Capacities = LOA_ReadULong(&pc_Buffer);
                }
                else
                {
                    pst_Struct->pst_AllLinks[j].pst_Links[i].uw_CapacitiesInit =
                    pst_Struct->pst_AllLinks[j].pst_Links[i].uw_Capacities = LOA_ReadUShort(&pc_Buffer);
                }
				ul_Key = LOA_ReadULong(&pc_Buffer);
				if(!WOR_gb_DoNotLoadLinkedObjects)
					LOA_MakeFileRef
					(
						ul_Key,
						(ULONG *) &pst_Struct->pst_AllLinks[j].pst_Links[i].pst_Next,
						OBJ_ul_GameObjectCallback,
						LOA_C_MustExists
					);
				else
					pst_Struct->pst_AllLinks[j].pst_Links[i].pst_Next = NULL;


                pst_Struct->pst_AllLinks[j].pst_Links[i].c_DesignInit = 
				pst_Struct->pst_AllLinks[j].pst_Links[i].c_Design = LOA_ReadChar(&pc_Buffer);

                pst_Struct->pst_AllLinks[j].pst_Links[i].uw_Design2 = LOA_ReadUShort(&pc_Buffer);
            }
        }
    }

    return (ULONG) pst_Struct;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

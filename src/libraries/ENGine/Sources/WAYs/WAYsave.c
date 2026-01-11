/*$T WAYsave.c GC! 1.081 06/14/00 16:42:15 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/WAYs/WAYstruct.h"
#include "ENGine/Sources/WAYs/WAYload.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "EDIpaths.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_SaveOneNetwork(WOR_tdst_World *_pst_World, WAY_tdst_Network *_pst_Net)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Key, ul_Index;
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Name[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get index of world */
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

	/* Compute path and file name */
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, EDI_Csz_Path_Network);

	/* Save network */
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Net);
	if(ul_Index == BIG_C_InvalidIndex)
	{
		if(_pst_Net->pst_Root)
		{
			ERR_X_Warning(0, "File of network can't be found !!!", _pst_Net->pst_Root->sz_Name);
		}
		else
		{
			ERR_X_Warning(0, "A network does not have an associated file !!!", NULL);
		}
		return;
	}
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	L_strcpy(asz_Name, BIG_NameFile(ul_Index));
	SAV_Begin(asz_Path, asz_Name);

	/* Key of root */
	if(_pst_Net->pst_Root)
	{
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Net->pst_Root);
		ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);
	}
	else
	{
		ul_Key = BIG_C_InvalidKey;
	}

	SAV_Buffer(&ul_Key, 4);

	/* Flags */
	SAV_Buffer(&_pst_Net->ul_Flags, 4);

	ul_Key = SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_SaveWorldNetworks(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		i;
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Name[BIG_C_MaxLenPath];
	BIG_INDEX	ul_Index;
	BIG_KEY		ul_Key;
	ULONG		ul_Num;
	char		*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_World->pst_AllNetworks) return;

	/* Get index of world */
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World);
	if(ul_Index == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(0, "File of world can't be found !!!", _pst_World->sz_Name);
		return;
	}
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

	/* Compute path and file name */
	L_strcpy(asz_Name, BIG_NameFile(ul_Index));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtNetWorld);

	ul_Num = _pst_World->pst_AllNetworks->ul_Num;
	for(i = 0; i < _pst_World->pst_AllNetworks->ul_Num; i++)
	{
		if(!_pst_World->pst_AllNetworks->ppst_AllNetworks[i]) ul_Num--;
		else if(_pst_World->pst_AllNetworks->ppst_AllNetworks[i]->ul_Flags & WAY_C_HasBeenMerge) ul_Num--;
	}

	if(!ul_Num) return;

	SAV_Begin(asz_Path, asz_Name);

	/* Number of ways */
	SAV_Buffer(&ul_Num, 4);

	/* Save references to each network */
	for(i = 0; i < _pst_World->pst_AllNetworks->ul_Num; i++)
	{
		if(_pst_World->pst_AllNetworks->ppst_AllNetworks[i])
		{
			if(!(_pst_World->pst_AllNetworks->ppst_AllNetworks[i]->ul_Flags & WAY_C_HasBeenMerge))
			{
				ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World->pst_AllNetworks->ppst_AllNetworks[i]);
				if(ul_Key == BIG_C_InvalidIndex)
				{
					ERR_X_Warning(0, "Network file of world can't be found !!!", _pst_World->sz_Name);
				}
				SAV_Buffer(&ul_Key, 4);
			}
		}
	}

	ul_Index = SAV_ul_End();
	LOA_AddAddress(ul_Index, _pst_World->pst_AllNetworks);

	/* Save all networks */
	for(i = 0; i < _pst_World->pst_AllNetworks->ul_Num; i++)
	{
		if
		(
			(_pst_World->pst_AllNetworks->ppst_AllNetworks[i])
		&&	(!(_pst_World->pst_AllNetworks->ppst_AllNetworks[i]->ul_Flags & WAY_C_HasBeenMerge))
		)
		{
			WAY_SaveOneNetwork(_pst_World, _pst_World->pst_AllNetworks->ppst_AllNetworks[i]);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG WAY_ul_SaveStruct(OBJ_tdst_GameObject *_pst_GO, WAY_tdst_Struct *_pst_Links)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	char	asz_Path[BIG_C_MaxLenPath];
	ULONG	i, j;
	BIG_KEY ul_Key;
	ULONG	ul_Num;
    ULONG   ulVersion = 1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Links);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

	/* Begin save */
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	SAV_Begin(asz_Path, BIG_NameFile(ul_Index));

	/* Number of lists (and version number in high short) */
    ulVersion = (ulVersion<<16) + _pst_Links->ul_Num;
	SAV_Buffer(&ulVersion, 4);

	for(j = 0; j < _pst_Links->ul_Num; j++)
	{
		/* Reference to the network */
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Links->pst_AllLinks[j].pst_Network);
		ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);
		SAV_Buffer(&ul_Key, 4);

		/* Check */
		ul_Num = _pst_Links->pst_AllLinks[j].ul_Num;
		for(i = 0; i < _pst_Links->pst_AllLinks[j].ul_Num; i++)
		{
			if(_pst_Links->pst_AllLinks[j].pst_Links[i].pst_Next == NULL) ul_Num--;
		}

		/* Number of links */
		SAV_Buffer(&ul_Num, 4);

		/* All the links */
		for(i = 0; i < _pst_Links->pst_AllLinks[j].ul_Num; i++)
		{
			if(_pst_Links->pst_AllLinks[j].pst_Links[i].pst_Next)
			{
				SAV_Buffer(&_pst_Links->pst_AllLinks[j].pst_Links[i].uw_CapacitiesInit, 4);
				ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Links->pst_AllLinks[j].pst_Links[i].pst_Next);
				if(ul_Key == BIG_C_InvalidIndex)
				{
					ERR_X_Warning
					(
						0,
						"Object is linked (in a network) to an object that does not have a file !!!",
						_pst_GO->sz_Name
					);
				}

				SAV_Buffer(&ul_Key, 4);
				SAV_Buffer(&_pst_Links->pst_AllLinks[j].pst_Links[i].c_DesignInit, 1);
				SAV_Buffer(&_pst_Links->pst_AllLinks[j].pst_Links[i].uw_Design2, 2);
			}
		}
	}

	return SAV_ul_End();
}

#endif /* ACTIVE_EDITORS */

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

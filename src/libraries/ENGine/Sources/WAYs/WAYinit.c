/*$T WAYinit.c GC! 1.081 05/22/00 10:57:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine\Sources\OBJects\OBJsave.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/WAYs/WAY.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "ENGine/Sources/WAYs/WAYload.h"
#include "ENGine/Sources/WAYs/WAYerrid.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORsave.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

WOR_tdst_World	*gpst_CurrentWorld;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_ComputePoundNetworks(WOR_tdst_World *_pst_World)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	if(!_pst_World->pst_AllNetworks) return;
	for(i = 0; i < _pst_World->pst_AllNetworks->ul_Num; i++)
	{
		gpst_CurrentWorld = _pst_World;
		if(_pst_World->pst_AllNetworks->ppst_AllNetworks[i])
			WAY_ComputePoundOneNetwork(_pst_World->pst_AllNetworks->ppst_AllNetworks[i]);
		gpst_CurrentWorld = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_ComputePoundOneNetwork(WAY_tdst_Network *_pst_Net)
{
	WAY_ComputePoundOneNetworkForObj(_pst_Net, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_ComputePoundNetworksRec
(
	WAY_tdst_Network	*_pst_Net,
	OBJ_tdst_GameObject *_pst_Root,
	OBJ_tdst_GameObject *_pst_Stop
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct		*pst_Struct;
	WAY_tdst_LinkList	*pst_LinkList;
	ULONG				i;
	MATH_tdst_Vector	st_Vec;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	if(TAB_ul_PFtable_GetElemIndexWithPointer(&gpst_CurrentWorld->st_AllWorldObjects, _pst_Root) == TAB_Cul_BadIndex)
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		BIG_INDEX	ul_Index;
		BIG_INDEX	ul_Index1;
		char		asz_Msg[512];
		/*~~~~~~~~~~~~~~~~~~~~~*/

		if(!_pst_Root || ((int) _pst_Root == -1))
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Net);
			sprintf(asz_Msg, "Network %s : Root is incorrect", BIG_NameFile(ul_Index));
			ERR_X_Warning(0, asz_Msg, NULL);
		}
		else
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Net);
			ul_Index1 = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Root);
			sprintf
			(
				asz_Msg,
				"Network %s : Root (%s) is referenced but is not in world. Check your .gol",
				BIG_NameFile(ul_Index),
				BIG_NameFile(ul_Index1)
			);
			ERR_X_Warning(0, asz_Msg, NULL);
			OBJ_GameObject_Remove(_pst_Root, 0);
			_pst_Root = NULL;
		}

		_pst_Net->pst_Root = NULL;
	}

#endif
	/* Object already seen */
	if(BAS_bsearch((ULONG) _pst_Root, &WAY_gst_Seen) != (ULONG) - 1) return;
	BAS_binsert((ULONG) _pst_Root, (ULONG) _pst_Root, &WAY_gst_Seen);

	if(!_pst_Root) return;
	if(!_pst_Root->pst_Extended) return;
	if(!_pst_Root->pst_Extended->pst_Links) return;

	pst_Struct = (WAY_tdst_Struct *) _pst_Root->pst_Extended->pst_Links;
	pst_LinkList = WAY_pst_SearchLinkList(_pst_Root, _pst_Net);
	if(!pst_LinkList) return;

	for(i = 0; i < pst_LinkList->ul_Num; i++)
	{
		MATH_CopyVector(&st_Vec, OBJ_pst_GetAbsolutePosition(_pst_Root));
		if(!(pst_LinkList->pst_Links[i].pst_Next)) continue;
		MATH_SubVector(&st_Vec, &st_Vec, OBJ_pst_GetAbsolutePosition(pst_LinkList->pst_Links[i].pst_Next));
		pst_LinkList->pst_Links[i].f_Pound = MATH_f_NormVector(&st_Vec);

		if(pst_LinkList->pst_Links[i].pst_Next == _pst_Stop) return;

#ifdef ACTIVE_EDITORS
		if
		(
			TAB_ul_PFtable_GetElemIndexWithPointer
			(
				&gpst_CurrentWorld->st_AllWorldObjects,
				pst_LinkList->pst_Links[i].pst_Next
			) == TAB_Cul_BadIndex
		)
		{
			/*~~~~~~~~~~~~~~~~~~~~~*/
			BIG_INDEX	ul_Index;
			BIG_INDEX	ul_Index1;
			BIG_INDEX	ul_Index2;
			char		asz_Msg[512];
			/*~~~~~~~~~~~~~~~~~~~~~*/

			if(!pst_LinkList->pst_Links[i].pst_Next || ((int) pst_LinkList->pst_Links[i].pst_Next == -1))
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Net);
				ul_Index1 = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Root);
				sprintf
				(
					asz_Msg,
					"Object %s in network %s : A link is referencing a bad object",
					BIG_NameFile(ul_Index1),
					BIG_NameFile(ul_Index)
				);
				ERR_X_Warning(0, asz_Msg, NULL);
			}
			else
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Net);
				ul_Index1 = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Root);
				ul_Index2 = LOA_ul_SearchIndexWithAddress((ULONG) pst_LinkList->pst_Links[i].pst_Next);
				sprintf
				(
					asz_Msg,
					"Object %s in network %s : Object linked %s is referenced but not in world. Check your .gol",
					ul_Index1 == BIG_C_InvalidIndex ? "__NoName__" : BIG_NameFile(ul_Index1),
					ul_Index == BIG_C_InvalidIndex ? "__NoName__" : BIG_NameFile(ul_Index),
					ul_Index2 == BIG_C_InvalidIndex ? "__NoName__" : BIG_NameFile(ul_Index2)
				);
				ERR_X_Warning(0, asz_Msg, NULL);
				OBJ_GameObject_Remove(pst_LinkList->pst_Links[i].pst_Next, 0);
			}

			pst_LinkList->pst_Links[i].pst_Next = NULL;
			return;
		}

#endif
		WAY_ComputePoundNetworksRec(_pst_Net, pst_LinkList->pst_Links[i].pst_Next, _pst_Stop);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_ComputePoundOneNetworkForObj(WAY_tdst_Network *_pst_Net, OBJ_tdst_GameObject *_pst_Stop)
{
	WAY_gst_Seen.num = 0;
	if(!_pst_Net->pst_Root) return;
	WAY_ComputePoundNetworksRec(_pst_Net, _pst_Net->pst_Root, _pst_Stop);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_RemoveAllNetworks(WAY_tdst_AllNetworks *_pst_AllNet)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	if(!_pst_AllNet) return;

	/* Delete all ways */
	for(i = 0; i < _pst_AllNet->ul_Num; i++)
	{
		if(_pst_AllNet->ppst_AllNetworks[i])
		{
			LOA_DeleteAddress(_pst_AllNet->ppst_AllNetworks[i]);
			MEM_Free(_pst_AllNet->ppst_AllNetworks[i]);
		}
	}

	/* Delete all networks */
	if(_pst_AllNet->ppst_AllNetworks) MEM_Free(_pst_AllNet->ppst_AllNetworks);
	LOA_DeleteAddress(_pst_AllNet);
	MEM_Free(_pst_AllNet);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_FreeStruct(WAY_tdst_Struct *_pst_Struct)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	/* Delete all links */
	if(!_pst_Struct) return;
	if(_pst_Struct->pst_AllLinks)
	{
		for(i = 0; i < _pst_Struct->ul_Num; i++)
		{
			if(_pst_Struct->pst_AllLinks[i].pst_Links)
			{
				MEM_Free(_pst_Struct->pst_AllLinks[i].pst_Links);
				_pst_Struct->pst_AllLinks[i].pst_Links = NULL;
			}
		}

		MEM_Free(_pst_Struct->pst_AllLinks);
		_pst_Struct->pst_AllLinks = NULL;
	}

	LOA_DeleteAddress(_pst_Struct);
	MEM_Free(_pst_Struct);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_LinkList *WAY_pst_SearchLinkList(OBJ_tdst_GameObject *_pst_Src, WAY_tdst_Network *_pst_Net)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct *pst_Struct;
	ULONG			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Src->pst_Extended) return NULL;
	pst_Struct = (WAY_tdst_Struct *) _pst_Src->pst_Extended->pst_Links;
	if(!pst_Struct) return NULL;
	if(!pst_Struct->pst_AllLinks) return NULL;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		if(pst_Struct->pst_AllLinks[i].pst_Network == _pst_Net) return &pst_Struct->pst_AllLinks[i];
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_Link *WAY_pst_SearchOneLinkInList(WAY_tdst_LinkList *pst_List, OBJ_tdst_GameObject *_pst_Dest)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	for(i = 0; i < pst_List->ul_Num; i++)
	{
		if(pst_List->pst_Links[i].pst_Next == _pst_Dest) return &pst_List->pst_Links[i];
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_LinkList *WAY_pst_SearchOneLinkList(OBJ_tdst_GameObject *_pst_GO, WAY_tdst_Link *_pst_Link)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				i, j;
	WAY_tdst_LinkList	*pst_List;
	WAY_tdst_Struct		*pst_Struct;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO->pst_Extended) return NULL;
	if(!_pst_GO->pst_Extended->pst_Links) return NULL;
	pst_Struct = (WAY_tdst_Struct *) _pst_GO->pst_Extended->pst_Links;

	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		pst_List = &pst_Struct->pst_AllLinks[i];
		for(j = 0; j < pst_List->ul_Num; j++)
		{
			if(&pst_List->pst_Links[j] == _pst_Link) return pst_List;
		}
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_ReinitLinks(WAY_tdst_Struct *_pst_Struct, int _i_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			i, j;
	WAY_tdst_Link	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Struct) return;
	for(i = 0; i < _pst_Struct->ul_Num; i++)
	{
		for(j = 0; j < _pst_Struct->pst_AllLinks[i].ul_Num; j++)
		{
			pst_Link = &_pst_Struct->pst_AllLinks[i].pst_Links[j];
			pst_Link->uw_Capacities = pst_Link->uw_CapacitiesInit;
			pst_Link->c_Design = pst_Link->c_DesignInit;
		}
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_Network *WAY_pst_SearchNetwork(WOR_tdst_World *_pst_World, char *_psz_Name, OBJ_tdst_GameObject *_pst_Root)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		i;
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Name[BIG_C_MaxLenName];
	BIG_INDEX	ul_Index, ul_Index1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Compute network path */
	WOR_GetSubPath(_pst_World, EDI_Csz_Path_Network, asz_Path);

	/* Net file of world */
	L_strcpy(asz_Name, _psz_Name);
	L_strcat(asz_Name, EDI_Csz_ExtNetWay);

	/* Network already here ? */
	ul_Index = BIG_ul_SearchFileExt(asz_Path, asz_Name);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		/* Is the network referenced ? */
		if(_pst_World->pst_AllNetworks)
		{
			for(i = 0; i < _pst_World->pst_AllNetworks->ul_Num; i++)
			{
				ul_Index1 = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World->pst_AllNetworks->ppst_AllNetworks[i]);
				ERR_X_Assert(ul_Index1 != BIG_C_InvalidIndex);
				ul_Index1 = BIG_ul_SearchKeyToFat(ul_Index1);

				/* Net already exists. Add to it ? */
				if(ul_Index == ul_Index1) return _pst_World->pst_AllNetworks->ppst_AllNetworks[i];
			}
		}
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_Network *WAY_pst_CreateNetwork(WOR_tdst_World *_pst_World, char *_psz_Name, OBJ_tdst_GameObject *_pst_Root)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					asz_Path[BIG_C_MaxLenPath];
	char					asz_Path1[BIG_C_MaxLenPath];
	char					asz_Name[BIG_C_MaxLenPath];
	char					asz_Name1[BIG_C_MaxLenPath];
	BIG_INDEX				ul_Index;
	char					*psz_Temp;
	BIG_KEY					ul_Key, ul_Key1;
	WAY_tdst_AllNetworks	*pst_AllNet;
	ULONG					i;
	int						i_NameNumber;
	char					az[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get index of world */
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	L_strcpy(asz_Path1, asz_Path);

	/* Compute level path */
	L_strcpy(asz_Name1, BIG_NameFile(ul_Index));
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, EDI_Csz_Path_Network);

	/* Net file of world */
	L_strcpy(asz_Name, _psz_Name);
	L_strcat(asz_Name, EDI_Csz_ExtNetWay);

	if(BIG_ul_SearchFileExt(asz_Path, asz_Name) != BIG_C_InvalidIndex)
	{
        sprintf(az,"File already exists : %s. Do you want to overwrite it ?",asz_Name);

        if (MessageBox(NULL,az, "Overwrite", MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            BIG_DelFile(asz_Path, asz_Name);
        }
        else
        {
            i_NameNumber = 0;
            sprintf(asz_Name, "Unnamed%i", i_NameNumber);
            L_strcat(asz_Name, EDI_Csz_ExtNetWay);
            while(1)
            {
                ul_Key = BIG_ul_SearchFileExt(asz_Path, asz_Name);
                if(ul_Key == BIG_C_InvalidIndex) break;
                i_NameNumber++;
                sprintf(asz_Name, "Unnamed%i", i_NameNumber);
                L_strcat(asz_Name, EDI_Csz_ExtNetWay);
            };

            sprintf(az, "Net file already exists. Renamed to %s", asz_Name);
            ERR_X_Warning(0, az, NULL);
        }
	}

	/* Save a null network */
	SAV_Begin(asz_Path, asz_Name);
	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Root);
	ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);
	SAV_Buffer(&ul_Key, 4);
	i = 0;
	SAV_Buffer(&i, 4);
	SAV_Buffer(&i, 4);
	ul_Key = SAV_ul_End();

	/* This is the first network. Create a dummy file */
	ul_Key1 = BIG_C_InvalidIndex;
	if(!_pst_World->pst_AllNetworks)
	{
		psz_Temp = L_strrchr(asz_Name1, '.');
		if(psz_Temp) *psz_Temp = 0;
		L_strcat(asz_Name1, EDI_Csz_ExtNetWorld);
		SAV_Begin(asz_Path1, asz_Name1);
		ul_Key1 = SAV_ul_End();
	}

	/* Add one network in world */
	if(!_pst_World->pst_AllNetworks)
	{
		_pst_World->pst_AllNetworks = (WAY_tdst_AllNetworks*)MEM_p_Alloc(sizeof(WAY_tdst_AllNetworks));
		pst_AllNet = _pst_World->pst_AllNetworks;
		pst_AllNet->ppst_AllNetworks = (WAY_tdst_Network**)MEM_p_Alloc(sizeof(WAY_tdst_Network *));
		pst_AllNet->ul_Num = 1;
	}
	else
	{
		pst_AllNet = _pst_World->pst_AllNetworks;
		if(!pst_AllNet->ul_Num || !pst_AllNet->ppst_AllNetworks)
		{
			pst_AllNet->ppst_AllNetworks = (WAY_tdst_Network**)MEM_p_Alloc(sizeof(WAY_tdst_Network *));
			pst_AllNet->ul_Num = 1;
		}
		else
		{
			pst_AllNet->ul_Num++;
			pst_AllNet->ppst_AllNetworks = (WAY_tdst_Network**)MEM_p_Realloc
				(
					pst_AllNet->ppst_AllNetworks,
					pst_AllNet->ul_Num * sizeof(WAY_tdst_Network *)
				);
		}
	}

	pst_AllNet->ppst_AllNetworks[pst_AllNet->ul_Num - 1] = (WAY_tdst_Network*)MEM_p_Alloc(sizeof(WAY_tdst_Network));
	L_memset(pst_AllNet->ppst_AllNetworks[pst_AllNet->ul_Num - 1], 0, sizeof(WAY_tdst_Network));
	pst_AllNet->ppst_AllNetworks[pst_AllNet->ul_Num - 1]->pst_Root = _pst_Root;
	pst_AllNet->ppst_AllNetworks[pst_AllNet->ul_Num - 1]->ul_Flags |= WAY_C_DisplayNet;

	LOA_AddAddress(ul_Key, pst_AllNet->ppst_AllNetworks[pst_AllNet->ul_Num - 1]);

	if(ul_Key1 != BIG_C_InvalidIndex) LOA_AddAddress(ul_Key1, pst_AllNet);

	return pst_AllNet->ppst_AllNetworks[pst_AllNet->ul_Num - 1];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_Struct *WAY_pst_CreateStruct(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Name[BIG_C_MaxLenPath];
	BIG_INDEX	ul_Index;
	char		*psz_Temp;
	BIG_KEY		ul_Key;
	ULONG		ul_Num;
	static int	i_NameNumber;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Object has already a link struct */
	if(OBJ_ul_FlagsIdentityGet(_pst_GO) & OBJ_C_IdentityFlag_Links) return (WAY_tdst_Struct*)_pst_GO->pst_Extended->pst_Links;
	/* Get index of world */
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

	/* Compute level path */
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, EDI_Csz_Path_NetLinks);

	/* Get name of GO world */
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GO);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

	/* Compute name of struct */
	L_strcpy(asz_Name, BIG_NameFile(ul_Index));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtNetObject);

	if(BIG_ul_SearchFileExt(asz_Path, asz_Name) != BIG_C_InvalidIndex)
	{
		i_NameNumber = 0;
		sprintf(asz_Name, "Unnamed%i", i_NameNumber);
		L_strcat(asz_Name, EDI_Csz_ExtNetObject);
		while(1)
		{
			ul_Key = BIG_ul_SearchFileExt(asz_Path, asz_Name);
			if(ul_Key == BIG_C_InvalidIndex) break;
			i_NameNumber++;
			sprintf(asz_Name, "Unnamed%i", i_NameNumber);
			L_strcat(asz_Name, EDI_Csz_ExtNetObject);
		};
	}

	/* Begin save */
	SAV_Begin(asz_Path, asz_Name);

	/* Number of link lists */
	ul_Num = 0;
	SAV_Buffer(&ul_Num, 4);

	ul_Key = SAV_ul_End();

	/* Allocate */
	OBJ_ChangeIdentityFlags
	(
		_pst_GO,
		OBJ_ul_FlagsIdentityGet(_pst_GO) | OBJ_C_IdentityFlag_Links,
		OBJ_ul_FlagsIdentityGet(_pst_GO)
	);
	LOA_AddAddress(ul_Key, _pst_GO->pst_Extended->pst_Links);
	return (WAY_tdst_Struct *) _pst_GO->pst_Extended->pst_Links;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_LinkList *WAY_pst_AddOneLinkList(OBJ_tdst_GameObject *_pst_Src, WAY_tdst_Network *_pst_Net)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct		*pst_Struct;
	WAY_tdst_LinkList	*pst_List;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Search if link list is already here */
	pst_List = WAY_pst_SearchLinkList(_pst_Src, _pst_Net);
	if(pst_List) return pst_List;

	pst_Struct = (WAY_tdst_Struct*)_pst_Src->pst_Extended->pst_Links;
	ERR_X_Assert(pst_Struct);

	/* Add one link list */
	if(!pst_Struct->pst_AllLinks)
	{
		pst_Struct->pst_AllLinks = (WAY_tdst_LinkList*)MEM_p_Alloc(sizeof(WAY_tdst_LinkList));
		pst_Struct->ul_Num = 1;
	}
	else
	{
		pst_Struct->ul_Num++;
		pst_Struct->pst_AllLinks = (WAY_tdst_LinkList*)MEM_p_Realloc
			(
				pst_Struct->pst_AllLinks,
				pst_Struct->ul_Num * sizeof(WAY_tdst_LinkList)
			);
	}

	L_memset(&pst_Struct->pst_AllLinks[pst_Struct->ul_Num - 1], 0, sizeof(WAY_tdst_LinkList));
	pst_Struct->pst_AllLinks[pst_Struct->ul_Num - 1].pst_Network = _pst_Net;
	return &pst_Struct->pst_AllLinks[pst_Struct->ul_Num - 1];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_RegisterLink(WAY_tdst_Network *_pst_Network, OBJ_tdst_GameObject *_pst_Src, WAY_tdst_Link *_pst_Link)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Name[512];
	char		asz_Path[512];
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Link->pst_Next) return;

	/* Compute path for registered link */
	WOR_GetSubPath(_pst_Src->pst_World, EDI_Csz_Path_Network, asz_Path);

	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Src);
	L_strcpy(asz_Name, BIG_NameFile(ul_Index));
	L_strcat(asz_Name, " => ");
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Link->pst_Next);
	L_strcat(asz_Name, BIG_NameFile(ul_Index));
	*L_strrchr(asz_Name, '.') = 0;

	LINK_RegisterPointer(_pst_Link, LINK_C_NetLink, asz_Name, asz_Path);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_RegisterAllLinks(OBJ_tdst_GameObject *_pst_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct *pst_Struct;
	ULONG			i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Src->pst_Extended) return;
	if(!_pst_Src->pst_Extended->pst_Links) return;
	pst_Struct = (WAY_tdst_Struct*)_pst_Src->pst_Extended->pst_Links;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
		{
			WAY_RegisterLink
			(
				pst_Struct->pst_AllLinks[i].pst_Network,
				_pst_Src,
				&pst_Struct->pst_AllLinks[i].pst_Links[j]
			);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_UnRegisterAllLinks(OBJ_tdst_GameObject *_pst_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct *pst_Struct;
	ULONG			i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Src->pst_Extended->pst_Links) return;
	pst_Struct = (WAY_tdst_Struct*)_pst_Src->pst_Extended->pst_Links;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		if(pst_Struct->pst_AllLinks)
		{
			for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
			{
				LINK_DelRegisterPointer(&pst_Struct->pst_AllLinks[i].pst_Links[j]);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_Link *WAY_pst_AddOneLink
(
	OBJ_tdst_GameObject *_pst_Src,
	WAY_tdst_LinkList	*pst_List,
	OBJ_tdst_GameObject *_pst_Dest
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Link	*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* Search if link is already here */
	pst_Link = WAY_pst_SearchOneLinkInList(pst_List, _pst_Dest);
	if(pst_Link) return pst_Link;

	WAY_UnRegisterAllLinks(_pst_Src);

	/* Else create a new one */
	if(!pst_List->pst_Links)
	{
		pst_List->pst_Links = (WAY_tdst_Link*)MEM_p_Alloc(sizeof(WAY_tdst_Link));
		pst_List->ul_Num = 1;
	}
	else
	{
		pst_List->ul_Num++;
		pst_List->pst_Links = (WAY_tdst_Link*)MEM_p_Realloc(pst_List->pst_Links, pst_List->ul_Num * sizeof(WAY_tdst_Link));
	}

	L_memset(&pst_List->pst_Links[pst_List->ul_Num - 1], 0, sizeof(WAY_tdst_Link));
	pst_List->pst_Links[pst_List->ul_Num - 1].pst_Next = _pst_Dest;

	WAY_RegisterAllLinks(_pst_Src);

	return &pst_List->pst_Links[pst_List->ul_Num - 1];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_DelOneLink(OBJ_tdst_GameObject *_pst_Src, WAY_tdst_LinkList *pst_List, OBJ_tdst_GameObject *_pst_Dest)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	for(i = 0; i < pst_List->ul_Num; i++)
	{
		if(pst_List->pst_Links[i].pst_Next == _pst_Dest)
		{
			LINK_DelRegisterPointer(&pst_List->pst_Links[i]);

			/* Shift all links */
			if(i != pst_List->ul_Num - 1)
			{
				L_memmove
				(
					&pst_List->pst_Links[i],
					&pst_List->pst_Links[i + 1],
					(pst_List->ul_Num - i - 1) * sizeof(WAY_tdst_Link)
				);
			}

			pst_List->ul_Num--;

			if(!pst_List->ul_Num && pst_List->pst_Network->pst_Root == _pst_Src)
				pst_List->pst_Network->pst_Root = _pst_Dest;
			if(((WAY_tdst_Struct *) _pst_Src->pst_Extended->pst_Links)->ul_Num == 0)
			{
				WAY_FreeStruct((WAY_tdst_Struct*)_pst_Src->pst_Extended->pst_Links);
				_pst_Src->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Links;
				_pst_Src->pst_Extended->pst_Links = NULL;
				break;
			}
		}
	}
}


void WAY_DelOneNetwork(WOR_tdst_World *_pst_World, WAY_tdst_Network *_pst_Network, BOOL _bDeleteWP, BOOL _bDeleteOnlyNet)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_AllNetworks	*pst_AllNet;
	WAY_tdst_Struct			*pst_Struct;
	ULONG					i, j;
	OBJ_tdst_GameObject		*pst_GO;
	TAB_tdst_PFelem			*pst_PFElem, *pst_PFLastElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // Remove .way file from bf.
    // No (done by hand so that undo is possible)
    /*
    BIG_INDEX ul_IndexNet = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Network);
    if(ul_IndexNet != BIG_C_InvalidIndex)
    {
        char asz_Path[256];
        BIG_ComputeFullName(BIG_ParentFile(ul_IndexNet), asz_Path);
        BIG_DelFile(asz_Path, BIG_NameFile(ul_IndexNet));
    }*/

	pst_AllNet = _pst_World->pst_AllNetworks;
	ERR_X_Assert(pst_AllNet);
	ERR_X_Assert(pst_AllNet->ul_Num);

	/* Search net to delete */
	for(i = 0; i < pst_AllNet->ul_Num; i++)
	{
		if(pst_AllNet->ppst_AllNetworks[i] == _pst_Network) break;
	}

	ERR_X_Assert(i < pst_AllNet->ul_Num);

	/* Delete network in world */
	LOA_DeleteAddress(pst_AllNet->ppst_AllNetworks[i]);
	MEM_Free(pst_AllNet->ppst_AllNetworks[i]);
	if(pst_AllNet->ul_Num == 1)
	{
		MEM_Free(_pst_World->pst_AllNetworks);
		_pst_World->pst_AllNetworks = NULL;
		pst_AllNet->ul_Num = 0;
	}
	else
	{
		L_memmove
		(
			&pst_AllNet->ppst_AllNetworks[i],
			&pst_AllNet->ppst_AllNetworks[i + 1],
			(pst_AllNet->ul_Num - i - 1) * sizeof(WAY_tdst_Network *)
		);
		pst_AllNet->ul_Num--;
	}

    if (_bDeleteOnlyNet)
        return;

	/* Delete all related links in the world */
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	while(pst_PFElem <= pst_PFLastElem)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		pst_PFElem++;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(!pst_GO->pst_Extended) continue;
		if(!pst_GO->pst_Extended->pst_Links) continue;
		pst_Struct = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;
		if(pst_Struct->ul_Num == 0) goto del;

		/* Is there a link of the given network ? */
		for(i = 0; i < pst_Struct->ul_Num; i++)
		{
			if(pst_Struct->pst_AllLinks[i].pst_Network == _pst_Network) break;
		}

		if(i == pst_Struct->ul_Num) continue;

		/* Free all links */
		for(j = pst_Struct->pst_AllLinks[i].ul_Num; (int) j > 0; j--)
		{
			WAY_DelOneLink(pst_GO, &pst_Struct->pst_AllLinks[i], pst_Struct->pst_AllLinks[i].pst_Links[0].pst_Next);
		}

		/* Free link list */
		L_memmove
		(
			&pst_Struct->pst_AllLinks[i],
			&pst_Struct->pst_AllLinks[i + 1],
			(pst_Struct->ul_Num - i - 1) * sizeof(WAY_tdst_Link *)
		);
		pst_Struct->ul_Num--;

		/* Free struct ? */
		if(pst_Struct->ul_Num == 0)
		{
del:
			WAY_FreeStruct(pst_Struct);
			pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Links;
			pst_GO->pst_Extended->pst_Links = NULL;

            if (_bDeleteWP) 
            {
                // Remove from world
                OBJ_WhenDestroyAnObject(_pst_World, pst_GO);
                WOR_World_DetachObject(_pst_World, pst_GO);
                INT_SnP_DetachObject(pst_GO, _pst_World);
                OBJ_ul_GameObject_Save(_pst_World, pst_GO, NULL);
                OBJ_GameObject_Remove(pst_GO, 1);

                // Delete gao file from bf
                // No (do it by hand so that undo is possible)
                /*
                BIG_INDEX ul_IndexGao = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO);
                if(ul_IndexGao != BIG_C_InvalidIndex)
                {
                    char asz_Path[256];
                    BIG_ComputeFullName(BIG_ParentFile(ul_IndexGao), asz_Path);
                    BIG_DelFile(asz_Path, BIG_NameFile(ul_IndexGao));
                }*/
            }
		}
	}
}


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_WhenDestroyAnObject(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_AllNetworks	*pst_AllNet;
	WAY_tdst_LinkList		*pst_LinkList;
	ULONG					i, j;
	TAB_tdst_PFelem			*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject		*pst_GO;
	WAY_tdst_Struct			*pst_Struct;
	WAY_tdst_Link			*pst_Link;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_AllNet = _pst_World->pst_AllNetworks;
	if(!pst_AllNet) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    First test is the object is a root network
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(i = 0; i < pst_AllNet->ul_Num; i++)
	{
		if(pst_AllNet->ppst_AllNetworks[i]->pst_Root == _pst_Obj)
		{
			/* Search the first link, and replace the root */
			pst_LinkList = NULL;
			if(_pst_Obj->pst_Extended && _pst_Obj->pst_Extended->pst_Links)
				pst_LinkList = WAY_pst_SearchLinkList(_pst_Obj, pst_AllNet->ppst_AllNetworks[i]);
			if(pst_LinkList && pst_LinkList->pst_Links)
			{
				if(pst_LinkList->ul_Num == 0)
				{
					pst_AllNet->ppst_AllNetworks[i]->pst_Root = NULL;
					WAY_DelOneNetwork(_pst_World, pst_AllNet->ppst_AllNetworks[i],FALSE,FALSE);
				}
				else
					pst_AllNet->ppst_AllNetworks[i]->pst_Root = pst_LinkList->pst_Links[0].pst_Next;
			}
			else
			{
				WAY_DelOneNetwork(_pst_World, pst_AllNet->ppst_AllNetworks[i],FALSE,FALSE);
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Test is the object is referenced by a link
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_pst_Obj->pst_Extended && _pst_Obj->pst_Extended->pst_Links)
	{
		pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
		pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
		while(pst_PFElem <= pst_PFLastElem)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
			pst_PFElem++;
			if(TAB_b_IsAHole(pst_GO)) continue;
			if(!pst_GO->pst_Extended) continue;
			if(!pst_GO->pst_Extended->pst_Links) continue;

			pst_Struct = (WAY_tdst_Struct*)pst_GO->pst_Extended->pst_Links;
			for(i = 0; i < pst_Struct->ul_Num; i++)
			{
				pst_LinkList = &pst_Struct->pst_AllLinks[i];
				if(pst_LinkList->pst_Links)
				{
					for(j = 0; j < pst_LinkList->ul_Num; j++)
					{
						pst_Link = &pst_LinkList->pst_Links[j];
						if(pst_Link->pst_Next == _pst_Obj)
						{
							WAY_DelOneLink(pst_GO, pst_LinkList, _pst_Obj);
							j--;
						}
					}
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL WAY_CheckObject(OBJ_tdst_GameObject *_pst_GO, WAY_tdst_Network *_pst_Net, OBJ_tdst_GameObject *_pst_Root)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct *pst_Struct;
	ULONG			i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GO == _pst_Root) return TRUE;
	if(_pst_Root == NULL) return FALSE;

	/* Already test */
	if(BAS_bsearch((ULONG) _pst_Root, &WAY_gst_Seen) != -1) return FALSE;
	BAS_binsert((ULONG) _pst_Root, (ULONG) _pst_Root, &WAY_gst_Seen);

	if(!_pst_Root->pst_Extended) return FALSE;
	if(!_pst_Root->pst_Extended->pst_Links) return FALSE;
	pst_Struct = (WAY_tdst_Struct *) _pst_Root->pst_Extended->pst_Links;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		if(!pst_Struct->pst_AllLinks) continue;
		if(pst_Struct->pst_AllLinks[i].pst_Network != _pst_Net) continue;
		for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
		{
			if(WAY_CheckObject(_pst_GO, _pst_Net, pst_Struct->pst_AllLinks[i].pst_Links[j].pst_Next)) return TRUE;
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_CheckObjectInNetwork(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			i;
	WAY_tdst_Struct *pst_Link;
	char			asz_Msg[512];
	BIG_INDEX		ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO->pst_Extended) return;
	if(!_pst_GO->pst_Extended->pst_Links) return;
	pst_Link = (WAY_tdst_Struct *) _pst_GO->pst_Extended->pst_Links;

	for(i = 0; i < pst_Link->ul_Num; i++)
	{
		if(!pst_Link->pst_AllLinks)
		{
			pst_Link->ul_Num = 0;
			ERR_X_Warning(0, "Object has incorrect link structure", _pst_GO->sz_Name);
			return;
		}

		if(pst_Link->pst_AllLinks[i].pst_Network)
		{
			WAY_gst_Seen.num = 0;
			if
			(
				!WAY_CheckObject
				(
					_pst_GO,
					pst_Link->pst_AllLinks[i].pst_Network,
					pst_Link->pst_AllLinks[i].pst_Network->pst_Root
				)
			)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Link->pst_AllLinks[i].pst_Network);
				ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
				if(pst_Link->pst_AllLinks[i].ul_Num)
				{
					sprintf
					(
						asz_Msg,
						"Object %s is not referenced in network %s. Root must be incorrect (changing...).",
						_pst_GO->sz_Name,
						BIG_NameFile(ul_Index)
					);
					ERR_X_Warning(0, asz_Msg, NULL);
					pst_Link->pst_AllLinks[i].pst_Network->pst_Root = _pst_GO;
				}
				else
				{
					sprintf
					(
						asz_Msg,
						"Object %s is not referenced in network %s and has no links.",
						_pst_GO->sz_Name,
						BIG_NameFile(ul_Index)
					);
					ERR_X_Warning(0, asz_Msg, NULL);

					/* Free link list */
					L_memmove
					(
						&pst_Link->pst_AllLinks[i],
						&pst_Link->pst_AllLinks[i + 1],
						(pst_Link->ul_Num - i - 1) * sizeof(WAY_tdst_Link *)
					);
					pst_Link->ul_Num--;
					if(pst_Link->ul_Num == 0)
					{
						WAY_FreeStruct(pst_Link);
						_pst_GO->pst_Extended->pst_Links = NULL;
						_pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Links;
						return;
					}

					i--;
				}
			}
		}
	}
}

#endif /* ACTIVE_EDITORS */

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

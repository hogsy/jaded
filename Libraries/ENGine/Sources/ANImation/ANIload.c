/*$T ANIload.c GC! 1.081 12/03/01 16:15:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"

#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"

#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "ENGine/Sources/GRP/GRPload.h"
#include "ENGine/Sources/GRP/GRPsave.h"
#include "ENGine/Sources/ACTions/ACTinit.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "GraphicDK/Sources/SELection/SELection.h"

#include "ANImation/ANIstruct.h"
#include "ANImation/ANIinit.h"
#include "ANImation/ANIload.h"
#include "ANImation/ANIsave.h"
#include "ANImation/ANImain.h"

#include "MoDiFier/MDFstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"

#ifdef JADEFUSION
#include "Light/LIGHTrejection.h"
#endif

#include "EDIpaths.h"

#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"

#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "AIinterp/Sources/Events/EVEsave.h"
#include "ENGine/Sources/ANImation/NIMtoTracks.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern BOOL WOR_gb_HasAddedMaterial;
extern BOOL LOA_gb_SpeedMode;

#ifdef ACTIVE_EDITORS
#define ANI_C_Error_Hole		0

#define ANI_C_Warning_NoCanal	1
#define ANI_C_Warning_Father	2
#define ANI_C_Warning_Order		3

char		*ANI_Csz_CheckMessages[6] =
{
	"*Skeleton* Hole Detected",
	"[WARNING] *Skeleton* No associated Canal",
	"[WARNING] *Skeleton* No Father",
	"[WARNING] *Skeleton* Group rank different from Gizmo number",
	"[WARNING] *Skeleton* Bone/Gizmo order"
};
#endif

/*
 =======================================================================================================================
    Aim:    Callback to load a Shape which is the correspondance between a Bone a Group a Visu.
 =======================================================================================================================
 */
ULONG ANI_ul_ShapeCallback(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	char			*pc_Buffer, c_Canal;
	ULONG			ul_Size;
	ANI_tdst_Shape	*pst_Shape;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Read the entire Shape into a temporary buffer */
	pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);

	/* Allocate the necessary memory to store the anim data */
	pst_Shape = (ANI_tdst_Shape *) MEM_p_Alloc(sizeof(ANI_tdst_Shape));
	L_memset(pst_Shape, 0xFF, sizeof(ANI_tdst_Shape));

	/* Resets the internal counter. */
	pst_Shape->uw_Counter = 0;

	pst_Shape->uw_LastCanal = (USHORT) LOA_ReadUChar(&pc_Buffer);

	pst_Shape->uw_LastCanal = (pst_Shape->uw_LastCanal > ANI_C_MaxNumberOfCanal) ? ANI_C_MaxNumberOfCanal : pst_Shape->uw_LastCanal;

	for(i = 0; i < pst_Shape->uw_LastCanal; i++)
	{
		c_Canal = LOA_ReadChar(&pc_Buffer);
		
		if((c_Canal < 0) || (c_Canal >= ANI_C_MaxNumberOfCanal))
		{
			ERR_X_Warning(0, "Shape Canal Overflow Error", NULL);
		}
		else
		{
			pst_Shape->auc_AI_Canal[c_Canal] = LOA_ReadUChar(&pc_Buffer);
			pst_Shape->auc_Visu[c_Canal] = LOA_ReadUChar(&pc_Buffer);
		}
	}

	return (ULONG) pst_Shape;
}

/*
 =======================================================================================================================
    Aim:    Decreases the skeleton internal counter and if it becomes 0, deletes it.
 =======================================================================================================================
 */
void ANI_DestroySkeleton(OBJ_tdst_Group *_pst_Skeleton)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
	OBJ_tdst_GameObject *pst_GrpGO;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Skeleton->ul_NbObjectsUsingMe--;
	if(!_pst_Skeleton->ul_NbObjectsUsingMe)
	{
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Skeleton->pst_AllObjects);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Skeleton->pst_AllObjects);
		for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GrpGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GrpGO)) continue;
			if(!(pst_GrpGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)) break;

			TAB_Ptable_RemoveElem((TAB_tdst_Ptable *)(_pst_Skeleton->pst_AllObjects), (void **)(pst_CurrentElem));

			pst_World = WOR_World_GetWorldOfObject(pst_GrpGO);
			WOR_World_DetachObject(pst_World, pst_GrpGO);
			OBJ_GameObject_Remove(pst_GrpGO, 1);
#ifdef ACTIVE_EDITORS
			SEL_DelItem(pst_World->pst_Selection, pst_GrpGO);
#endif
		}

		OBJ_FreeGroup(_pst_Skeleton);
	}
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_CheckSkeleton(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_Group		*pst_Skeleton;
	ANI_tdst_Anim		*pst_Anim;
	OBJ_tdst_GameObject *pst_BoneGO, *pst_BoneFather;
	ULONG				i;
	UCHAR				uc_EndLoop;
	char				asz_Log[200], asz_BoneName[100];
	BOOL				b_Anim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Anim = (_pst_GO->pst_Base->pst_GameObjectAnim->uc_AnimUsed) ? TRUE : FALSE;

	pst_Skeleton = _pst_GO->pst_Base->pst_GameObjectAnim->pst_SkeletonModel;
	pst_Anim = _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0];

	if(!pst_Skeleton) return;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);

	if(b_Anim) uc_EndLoop = (pst_Anim->pst_Data->pst_ListTracks->uw_NumTracks >> 1) - 1;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	if(!pst_CurrentBone)
	{
		ERR_X_Warning(0, "Skeleton group with Hole at first place", _pst_GO->sz_Name);
		return;
	}

	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);

	for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
	{
		if((b_Anim) && (i >= uc_EndLoop)) break;

		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO))
		{
			ERR_X_Warning(0, ANI_Csz_CheckMessages[ANI_C_Error_Hole], NULL);
		}
		else
		{
			L_strcpy(asz_BoneName, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_BoneGO)));

			if
			(
				(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Hierarchy))
			&&	(pst_BoneGO->pst_Base->pst_Hierarchy)
			) pst_BoneFather = pst_BoneGO->pst_Base->pst_Hierarchy->pst_Father;
			else
				pst_BoneFather = NULL;

			if((i != 0) && !pst_BoneFather)
			{
				sprintf(asz_Log, "%s [%s]", ANI_Csz_CheckMessages[ANI_C_Warning_Father], asz_BoneName);
				LINK_PrintStatusMsg(asz_Log);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_CheckInternalGroups(OBJ_tdst_Group *_pst_Skeleton, char *_psz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentBone, *pst_EndBone;
	TAB_tdst_PFelem		*pst_CurrentVisu, *pst_EndVisu;
	TAB_tdst_PFelem		*pst_Current, *pst_End;
	OBJ_tdst_GameObject *pst_BoneGO, *pst_VisuGO, *pst_GO;
	OBJ_tdst_Group		*pst_Group;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(_pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(_pst_Skeleton->pst_AllObjects);
	for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO)) continue;

		if
		(
			(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Group))
		&&	pst_BoneGO->pst_Extended
		&&	pst_BoneGO->pst_Extended->pst_Group
		)
		{
			pst_Group = pst_BoneGO->pst_Extended->pst_Group;
			pst_CurrentVisu = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
			pst_EndVisu = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
			i = 0;
			for(; pst_CurrentVisu && (pst_CurrentVisu <= pst_EndVisu); pst_CurrentVisu++)
			{
				pst_VisuGO = (OBJ_tdst_GameObject *) pst_CurrentVisu->p_Pointer;
				if(TAB_b_IsAHole(pst_VisuGO)) continue;

				pst_Current = TAB_pst_PFtable_GetFirstElem(_pst_Skeleton->pst_AllObjects);
				pst_End = TAB_pst_PFtable_GetLastElem(_pst_Skeleton->pst_AllObjects);
				for(; pst_Current <= pst_End; pst_Current++)
				{
					pst_GO = (OBJ_tdst_GameObject *) pst_Current->p_Pointer;
					if(TAB_b_IsAHole(pst_GO)) continue;

					if((pst_GO == pst_VisuGO) && (pst_GO != pst_BoneGO))
					{
						/*~~~~~~~~~~~~~~~~~*/
						char	asz_Log[300];
						/*~~~~~~~~~~~~~~~~~*/

						if(_psz_Name)
						{
							snprintf
							(
								asz_Log,
								sizeof(asz_Log),
								"%s's group detached because it contains %s in %s",
								pst_BoneGO->sz_Name,
								pst_GO->sz_Name,
								_psz_Name
							);
						}
						else
						{
							snprintf
							(
								asz_Log,
								sizeof( asz_Log ),
								"%s's group detached because it contains %s",
								pst_BoneGO->sz_Name,
								pst_GO->sz_Name
							);
						}

						ERR_X_Warning(0, asz_Log, NULL);
						GRP_ObjDetachGroup(pst_BoneGO);
						pst_BoneGO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Group;
					}
				}

				if
				(
					pst_VisuGO->pst_Base
				&&	pst_VisuGO->pst_Base->pst_AddMatrix
				&&	(pst_VisuGO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
				)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					OBJ_tdst_GameObject *pst_PonderationGO;
					int					i;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					for(i = 0; i < pst_VisuGO->pst_Base->pst_AddMatrix->l_Number; i++)
					{
						pst_PonderationGO = pst_VisuGO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO;

						if(!pst_PonderationGO) continue;

						pst_Current = TAB_pst_PFtable_GetFirstElem(_pst_Skeleton->pst_AllObjects);
						pst_End = TAB_pst_PFtable_GetLastElem(_pst_Skeleton->pst_AllObjects);
						for(; pst_Current <= pst_End; pst_Current++)
						{
							pst_GO = (OBJ_tdst_GameObject *) pst_Current->p_Pointer;
							if(TAB_b_IsAHole(pst_GO)) continue;

							if(pst_GO == pst_PonderationGO) break;
						}

						if(pst_Current > pst_End)
						{
							/*~~~~~~~~~~~~~~~~~*/
							char	asz_Log[300];
							/*~~~~~~~~~~~~~~~~~*/

							snprintf
							(
								asz_Log,
								sizeof(asz_Log),
								"%s is ponderating by %s which is not in the same Skeleton",
								pst_VisuGO->sz_Name,
								pst_PonderationGO->sz_Name
							);

							ERR_X_Warning(0, asz_Log, NULL);
						}
					}
				}

				if
				(
					pst_VisuGO
				&&	pst_VisuGO->pst_Extended
				&&	pst_VisuGO->pst_Extended->pst_Group
				&&	(pst_VisuGO != pst_BoneGO)
				)
				{
					/*~~~~~~~~~~~~~~~~~*/
					char	asz_Log[300];
					/*~~~~~~~~~~~~~~~~~*/

					sprintf
					(
						asz_Log,
						"%s is one of the possible visuals of %s and has also a group",
						pst_VisuGO->sz_Name,
						pst_BoneGO->sz_Name
					);

					ERR_X_Warning(0, asz_Log, NULL);
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BIG_KEY ANI_DataToTrack(BIG_KEY _ul_DataKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_KEY		ul_TrackListKey;
	BIG_INDEX	ul_DataIndex, ul_TrackListIndex;
	char		az_FileName[BIG_C_MaxLenName];
	char		*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_DataKey == BIG_C_InvalidKey) return BIG_C_InvalidKey;
	ul_DataIndex = BIG_ul_SearchKeyToFat(_ul_DataKey);
	if(ul_DataIndex == BIG_C_InvalidIndex) return BIG_C_InvalidKey;

	/* Gets the name of the .nim */
	L_strcpy(az_FileName, BIG_NameFile(ul_DataIndex));

	/* Is it a .trl or a .nim ? */
	psz_Temp = L_strrchr(az_FileName, '.');
	if(psz_Temp && (L_strcmp(psz_Temp, EDI_Csz_ExtEventAllsTracks)))
	{
		/* Creates the corresponding .trl filename */
		psz_Temp = L_strrchr(az_FileName, '.');
		if(*psz_Temp) *psz_Temp = 0;
		L_strcat(az_FileName, EDI_Csz_ExtEventAllsTracks);

		ul_TrackListIndex = BIG_ul_SearchFileInDirRec(BIG_ParentFile(ul_DataIndex), az_FileName);

		if(ul_TrackListIndex == BIG_C_InvalidIndex)
		{
			ul_TrackListKey = ANI_ConvertNIMfileToTracks(ul_DataIndex);
			return(ul_TrackListKey);
		}
		else
			return(BIG_FileKey(ul_TrackListIndex));
	}
	else
	{
		if(psz_Temp && (!L_strcmp(psz_Temp, EDI_Csz_ExtEventAllsTracks))) return(_ul_DataKey);
	}

#ifdef ACTIVE_EDITORS
	ERR_X_Warning(0, "Anim Extension File invalid", NULL);
#endif
	return BIG_C_InvalidKey;
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_AttachSkeletonModelToWorld(OBJ_tdst_Group *_pst_Model, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Aim: Add all the GameObjects of the Skeleton Model in the World. */
	TAB_tdst_PFelem		*pst_CurrentBone, *pst_EndBone;
	TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
	OBJ_tdst_GameObject *pst_BoneGO, *pst_GrpGO;
	OBJ_tdst_Group		*pst_Group;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(_pst_Model->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(_pst_Model->pst_AllObjects);
	for(; pst_CurrentBone && (pst_CurrentBone <= pst_EndBone); pst_CurrentBone++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

		if(TAB_b_IsAHole(pst_BoneGO)) continue;

		/* If the Gao is already in the World, we do not add it again. */
		if(TAB_ul_PFtable_GetElemIndexWithPointer(&_pst_World->st_AllWorldObjects, pst_BoneGO) != TAB_Cul_BadIndex)
		{
#ifdef ACTIVE_EDITORS
			if((OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Bone)) && (_pst_Model->ul_NbObjectsUsingMe == 0))
			{
				ERR_X_Warning(0, "A bone is present in several skeletons.", pst_BoneGO->sz_Name);
			}

#endif
			continue;
		}

		pst_BoneGO->ul_IdentityFlags |= OBJ_C_IdentityFlag_Bone;

#ifdef ACTIVE_EDITORS
		WOR_World_JustAfterLoadObject(_pst_World, pst_BoneGO, TRUE, TRUE);
#else
		WOR_World_JustAfterLoadObject(_pst_World, pst_BoneGO, FALSE, TRUE);
#endif
		if(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Group))
		{
			pst_Group = pst_BoneGO->pst_Extended->pst_Group;

			pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
			pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
			for(; pst_CurrentElem && (pst_CurrentElem <= pst_EndElem); pst_CurrentElem++)
			{
				pst_GrpGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

				if(TAB_b_IsAHole(pst_GrpGO)) continue;

				/* If the Gao is already in the World, we do not add it again. */
				if
				(
					TAB_ul_PFtable_GetElemIndexWithPointer
					(
						&_pst_World->st_AllWorldObjects,
						pst_GrpGO
					) != TAB_Cul_BadIndex
				)
				{
#ifdef ACTIVE_EDITORS
					if
					(
						(OBJ_b_TestIdentityFlag(pst_GrpGO, OBJ_C_IdentityFlag_Bone))
					&&	(pst_Group->ul_NbObjectsUsingMe == 0)
					)
					{
						ERR_X_Warning(0, "A bone is present in several skeletons.", pst_BoneGO->sz_Name);
					}

#endif
					continue;
				}

				pst_GrpGO->ul_IdentityFlags |= OBJ_C_IdentityFlag_Bone;
				WOR_World_JustAfterLoadObject(_pst_World, pst_GrpGO, TRUE, TRUE);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_ApplyShape(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_GameObject		*pst_BoneGO;
	UCHAR					uc_Canal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO) return;

	if
	(
		!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	) return;

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	if(!pst_GOAnim->pst_Skeleton) return;
	if(!pst_GOAnim->pst_Shape) return;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_GOAnim->pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_GOAnim->pst_Skeleton->pst_AllObjects);
	for(uc_Canal = 0; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++, uc_Canal++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO)) continue;

		if(pst_GOAnim->pst_Shape == pst_GOAnim->pst_DefaultShape)
			ANI_ChangeBoneShape(_pst_GO, uc_Canal, pst_GOAnim->auc_DefaultVisu[uc_Canal], FALSE);
		else
			ANI_ChangeBoneShape(_pst_GO, uc_Canal, pst_GOAnim->pst_Shape->auc_Visu[uc_Canal], FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void UpdateModifiersGroup(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_Group *_pt_Skeleton)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_GameObject *pst_BoneGO;
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GO->pst_Extended)
	{
		pst_Mod = _pst_GO->pst_Extended->pst_Modifiers;
		while(pst_Mod)
		{
			pst_Mod->pst_Group = _pt_Skeleton;
			if(pst_Mod->i->pfnv_Reinit) pst_Mod->i->pfnv_Reinit(pst_Mod);
			pst_Mod = pst_Mod->pst_Next;
		}
	}

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(_pt_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(_pt_Skeleton->pst_AllObjects);
	for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO)) continue;
		if(!pst_BoneGO->pst_Extended) continue;
		if(!pst_BoneGO->pst_Extended->pst_Modifiers) continue;

		pst_Mod = pst_BoneGO->pst_Extended->pst_Modifiers;
		while(pst_Mod)
		{
			pst_Mod->pst_Group = _pt_Skeleton;
			if(pst_Mod->i->pfnv_Reinit) pst_Mod->i->pfnv_Reinit(pst_Mod);
			pst_Mod = pst_Mod->pst_Next;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_pst_Load
(
	BIG_KEY				ul_TrackListKey,
	BIG_KEY				ul_ShapeKey,
	BIG_KEY				ul_ModelKey,
	OBJ_tdst_GameObject *_pst_GO,
	WOR_tdst_World		*_pst_World
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_ListTracks		*pst_TrackList;
	ANI_tdst_Shape			*pst_Shape;
	OBJ_tdst_Group			*pst_Model, *pst_Skeleton;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	LONG					l_NbGizmos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEMpro_StartMemRaster();
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	pst_GOAnim->uc_Synchro = 1;
	pst_GOAnim->pst_Shape = NULL;
	pst_GOAnim->pst_Skeleton = NULL;
	pst_GOAnim->pst_SkeletonModel = NULL;
	pst_GOAnim->pst_DefaultSkeleton = NULL;

	pst_TrackList = NULL;
	pst_Shape = NULL;
	pst_Model = NULL;
	pst_Skeleton = NULL;

	if(ul_ShapeKey != BIG_C_InvalidKey)
	{
		LOA_MakeFileRef(ul_ShapeKey, (ULONG *) &pst_Shape, ANI_ul_ShapeCallback, LOA_C_MustExists);
		LOA_Resolve();
	}
	if(ul_ModelKey != BIG_C_InvalidKey)
	{
		LOA_MakeFileRef(ul_ModelKey, (ULONG *) &pst_Model, GRP_ul_LoadGrpCallback, LOA_C_MustExists);
		LOA_Resolve();
	}
	

	if(pst_Model)
	{
		pst_Skeleton = GRP_pst_CreateNewGroup();
		ANI_AttachSkeletonModelToWorld(pst_Model, _pst_World);
#ifdef ACTIVE_EDITORS
		ANI_CheckInternalGroups(pst_Model, _pst_GO->sz_Name);
#endif
#ifdef JADEFUSION
		GRP_DuplicateGroupObjects(_pst_World, pst_Model, pst_Skeleton, eLRLLoading);
#else
		GRP_DuplicateGroupObjects(_pst_World, pst_Model, pst_Skeleton);
#endif
		UpdateModifiersGroup(_pst_GO, pst_Skeleton);
		ANI_UseSkeleton(pst_Skeleton);
		ANI_UseSkeleton(pst_Model);
		pst_GOAnim->pst_SkeletonModel = pst_Model;
		pst_GOAnim->pst_Skeleton = pst_Skeleton;
	}

	if(pst_Shape)
	{
		ANI_UseShape(pst_Shape);
		pst_GOAnim->pst_Shape = pst_Shape;
		pst_GOAnim->pst_DefaultShape = pst_Shape;

		/* Inializing Default Visuals */
		L_memcpy(pst_GOAnim->auc_DefaultVisu, pst_Shape->auc_Visu, sizeof(UCHAR) * ANI_C_MaxNumberOfCanal);

		ANI_ApplyShape(_pst_GO);
	}

	/*
	 * Either the Object has an ActionKit or a single Anim. We do not load the Anim if
	 * the Object has a Kit.
	 */
	if((ul_TrackListKey != BIG_C_InvalidKey) && (!pst_GOAnim->pst_ActionKit))
	{
		LOA_MakeFileRef(ul_TrackListKey, (ULONG *) &pst_TrackList, EVE_ul_CallbackLoadListTracks, LOA_C_MustExists);
		LOA_Resolve();

#ifdef _GAMECUBE
		if(pst_TrackList->ul_GC_Flags | EVE_C_ListTracks_UseARAM)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			u32				u32_ARAM_Pointer;
			volatile BOOL	b_Sent;
			int				i;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			
			pst_TrackList->ul_GC_Flags &= ~EVE_C_ListTracks_UseARAM;		
			for(i = 0; i < (int) pst_TrackList->uw_NumTracks; i++)
			{			
				b_Sent = FALSE;
				if(pst_TrackList->pst_AllTracks[i].w_Type & EVE_C_Track_UseARAM)
				{
					u32_ARAM_Pointer = (u32)pst_TrackList->pst_AllTracks[i].pst_AllEvents;
					pst_TrackList->pst_AllTracks[i].pst_AllEvents = (EVE_tdst_Event *) MEM_p_AllocAlign(OSRoundUp32B(pst_TrackList->pst_AllTracks[i].uw_NumEvents * sizeof(EVE_tdst_Event)), 32);
					ARAM_GetFromARAM((u32)pst_TrackList->pst_AllTracks[i].pst_AllEvents, u32_ARAM_Pointer, OSRoundUp32B(pst_TrackList->pst_AllTracks[i].uw_NumEvents * sizeof(EVE_tdst_Event)), (u32) &b_Sent, NULL);
					pst_TrackList->pst_AllTracks[i].w_Type &=  ~EVE_C_Track_UseARAM;
					while(!b_Sent) {}
				}
			}			
		}
#endif
		pst_GOAnim->apst_Anim[0] = ANI_AllocAnim();
		ANI_SetTrackList(pst_GOAnim->apst_Anim[0], pst_TrackList);
		pst_GOAnim->uc_AnimUsed = 1;
		if(pst_Model) l_NbGizmos = TAB_ul_PFtable_GetNbElems(pst_Skeleton->pst_AllObjects);
	}
	else
	{
		pst_GOAnim->apst_Anim[0] = NULL;
		if(pst_Model) l_NbGizmos = TAB_ul_PFtable_GetNbElems(pst_Skeleton->pst_AllObjects);
	}

	/* Check if the Reference Gao has enough Gizmos. */
	if
	(
		pst_Model
	&&	(
			(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_AdditionalMatrix))
		||	((_pst_GO->pst_Base->pst_AddMatrix->l_Number >> 1) < l_NbGizmos)
		)
	) OBJ_GameObject_AllocateGizmo(_pst_GO, l_NbGizmos, FALSE);

	/* Sets a default Flash Matrix */
	OBJ_SetFlashMatrix(_pst_GO, OBJ_pst_GetAbsoluteMatrix(_pst_GO));

	if(pst_Model)
	{
		ANI_InitSkeleton(pst_Skeleton, _pst_GO);
#ifdef ACTIVE_EDITORS
		ANI_CheckSkeleton(_pst_GO);
#endif
		UpdateModifiersGroup(_pst_GO, pst_Skeleton);
	}

	MEMpro_StopMemRaster(MEMpro_Id_ANI_Load);
}

#ifdef ACTIVE_EDITORS

/*
 ==========================================================================================	=============================
 =======================================================================================================================
 */
void ANI_RemoveUselessTranslationTracks(OBJ_tdst_GameObject *_pst_GO, EVE_tdst_ListTracks *_pst_ListTracks)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	int				i;	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if 0
	/* DEBUG PURPOSE */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		BIG_KEY			Index;
		char			asz_Name[200];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		
		Index = LOA_ul_SearchIndexWithAddress((ULONG)_pst_ListTracks);
		if(Index != BIG_C_InvalidIndex)
		{
			L_strcpy(asz_Name, BIG_NameFile(Index));
			if(!L_strcmp(asz_Name, "Gabcost march portecaisse cycl.trl"))
			{
				int i;
				
				i = 1;
			}
		}
	}
#endif

	for(i = 0; i < _pst_ListTracks->uw_NumTracks; i++)
	{

		pst_Track = &_pst_ListTracks->pst_AllTracks[i];
		if(!(pst_Track->uw_Flags & EVE_C_Track_Anims)) continue;

		if((pst_Track->uw_Gizmo != 0xFFFF) && (pst_Track->uw_Gizmo != 0))
		{
			if(pst_Track->pst_AllEvents && EVE_w_Event_InterpolationKey_GetType(&pst_Track->pst_AllEvents[0]) & EVE_InterKeyType_Translation_Mask)
			{
				OBJ_tdst_GameObject	*pst_BoneGO;
				MATH_tdst_Vector	*pst_Pos, *pst_T;
				float				f_Distance;

				pst_BoneGO = ANI_pst_GetObjectByCanal(_pst_GO, (UCHAR) pst_Track->uw_Gizmo);
                if(!pst_BoneGO ) continue;

				if(pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
				{
					pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(&pst_Track->pst_AllEvents[0]);
					pst_T = &pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T;
					f_Distance = MATH_f_Distance(pst_Pos, pst_T);
					
					if(f_Distance < 2E-2f)
					{
						EVE_FreeTrack(&_pst_ListTracks->pst_AllTracks[i]);
						L_memcpy(&_pst_ListTracks->pst_AllTracks[i], &_pst_ListTracks->pst_AllTracks[i+1], (_pst_ListTracks->uw_NumTracks - i - 1) * sizeof(EVE_tdst_Track));
						_pst_ListTracks->uw_NumTracks --;
						i --;
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
void ANI_ResolveAllTracksAndOptimize(OBJ_tdst_GameObject *_pst_GO)
{
	if(!LOA_gb_SpeedMode)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int					i, j;
		ACT_st_ActionKit	*pst_ActionKit;
		ACT_st_Action		*pst_Action;
		ACT_st_ActionItem	*pst_ActionItem;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if
		(
			!_pst_GO
		||	!_pst_GO->pst_Base
		||	!_pst_GO->pst_Base->pst_GameObjectAnim
		||	!_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit
		) return;

		pst_ActionKit = _pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit;

		/* Update events */
		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Events)) EVE_ResolveRefs(_pst_GO, FALSE);

		for(i = 0; i < pst_ActionKit->uw_NumberOfAction; i++)
		{
			pst_Action = pst_ActionKit->apst_Action[i];
			if(!pst_Action) continue;

			pst_ActionItem = pst_Action->ast_ActionItem;
			for(j = 0; j < pst_Action->uc_NumberOfActionItem; j++, pst_ActionItem++)
			{
				if(!pst_ActionItem->pst_TrackList) continue;

				/* Resolve List Track Refs */
				EVE_ResolveListTrackRef(pst_ActionItem->pst_TrackList);
			}
		}
	}
}

#endif

/*$F
 =======================================================================================================================
    Aim:    We have just loaded an object that has an anim. This anim references a Skeleton Model
            Group that contains many GameObjects. We wait for the world to be entirely loaded before
            resolving the references.

  Note:		The Key of the Data is "stored" in apst_Anim.
			The Key of the Skeleton is "stored" in pst_SkeletonModel.
			The Key of the Shape is "stored" in pst_Shape.
 =======================================================================================================================
 */

void ANI_ResolveRefs(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	WOR_tdst_World			*pst_World;
	BIG_KEY					ul_Data, ul_Shape, ul_Model;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	ERR_X_Assert(pst_GOAnim);

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	ul_Data = (BIG_KEY) pst_GOAnim->apst_Anim[0];
	ul_Shape = (BIG_KEY) pst_GOAnim->pst_Shape;
	ul_Model = (BIG_KEY) pst_GOAnim->pst_SkeletonModel;

#ifdef ACTIVE_EDITORS
	if(ul_Model == BIG_C_InvalidIndex)
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[100];
		/*~~~~~~~~~~~~~~~~~*/

		sprintf(asz_Log, "[Warning] No Skeleton for %s ...", _pst_GO->sz_Name);
		LINK_PrintStatusMsg(asz_Log);
	}

#endif
#ifdef ACTIVE_EDITORS
	ul_Data = ANI_DataToTrack(ul_Data);
#endif
	ANI_pst_Load(ul_Data, ul_Shape, ul_Model, _pst_GO, pst_World);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

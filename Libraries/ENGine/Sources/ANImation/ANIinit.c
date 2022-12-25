/*$T ANIinit.c GC! 1.081 11/06/01 11:48:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "WORld/WORstruct.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/ACTions/ACTinit.h"
#include "ENGine/Sources/ACTions/ACTload.h"
#include "ENGine/Sources/ACTions/ACTcompute.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "SDK/Sources/BASe/CLIbrary/CLImem.h"
#include "SDK/Sources/BASe/MEMory/MEM.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "AIinterp/Sources/Events/EVEinit.h"

#ifdef JADEFUSION
#include "Light\LIGHTrejection.h"
#endif

#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/ANImation/NIMtoTracks.h"
#include "LINks/LINKtoed.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern EVE_tdst_ListTracks			*EVE_gpst_CurrentListTracks;
extern struct OBJ_tdst_GameObject_	*EVE_gpst_CurrentGAO;

extern void	UpdateModifiersGroup(OBJ_tdst_GameObject *, OBJ_tdst_Group *);


static char ANI_c_DefaultBitFieldLOD = (char) 0x80;	/* Allow optimization when culled */

extern ULONG		LOA_ul_FileTypeSize[40];


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_Free(ANI_st_GameObjectAnim *pst_GOAnim)
{
	/*~~*/
	int i;
	/*~~*/

	if(pst_GOAnim->pst_ActionKit)
	{
		ACT_FreeActionKit(&pst_GOAnim->pst_ActionKit);
		for(i = 0; i < ANI_C_MaxNumberOfSimultaneousAnim; i++)
		{
			if(pst_GOAnim->apst_Anim[i] && ((ULONG) pst_GOAnim->apst_Anim[i] != (ULONG) - 1))
			{
#ifdef ACTIVE_EDITORS
				if(BIG_ul_SearchKeyToPos((ULONG) pst_GOAnim->apst_Anim[i]) == -1)
#endif
				{
					if(pst_GOAnim->apst_Anim[i]->pst_Data->pst_ListParam)
						MEM_Free(pst_GOAnim->apst_Anim[i]->pst_Data->pst_ListParam);
#if defined(_XBOX) || defined(_XENON)
					MEM_FreeAlign(pst_GOAnim->apst_Anim[i]->pst_Data);
					MEM_FreeAlign(pst_GOAnim->apst_Anim[i]);
#else
					MEM_Free(pst_GOAnim->apst_Anim[i]->pst_Data);
					MEM_Free(pst_GOAnim->apst_Anim[i]);
#endif
				}
			}


			if(pst_GOAnim->apst_PartialAnim[i] && ((ULONG) pst_GOAnim->apst_PartialAnim[i] != (ULONG) - 1))
			{
#ifdef ACTIVE_EDITORS
				if(BIG_ul_SearchKeyToPos((ULONG) pst_GOAnim->apst_PartialAnim[i]) == -1)
#endif
				{
					if(pst_GOAnim->apst_PartialAnim[i]->pst_Data->pst_ListParam)
						MEM_Free(pst_GOAnim->apst_PartialAnim[i]->pst_Data->pst_ListParam);
#if defined(_XBOX) || defined(_XENON)
					MEM_FreeAlign(pst_GOAnim->apst_PartialAnim[i]->pst_Data);
					MEM_FreeAlign(pst_GOAnim->apst_PartialAnim[i]);
#else
					MEM_Free(pst_GOAnim->apst_PartialAnim[i]->pst_Data);
					MEM_Free(pst_GOAnim->apst_PartialAnim[i]);
#endif
				}
			}

		}
	}
	else
	{
		for(i = 0; i < ANI_C_MaxNumberOfSimultaneousAnim; i++)
		{
			if((pst_GOAnim->apst_Anim[i]) && ((ULONG) pst_GOAnim->apst_Anim[i] != (ULONG) - 1))
				ANI_DestroyAnimation(pst_GOAnim->apst_Anim[i]);

			if((pst_GOAnim->apst_PartialAnim[i]) && ((ULONG) pst_GOAnim->apst_PartialAnim[i] != (ULONG) - 1))
				ANI_DestroyAnimation(pst_GOAnim->apst_PartialAnim[i]);

		}

	}

	if(pst_GOAnim->pst_Skeleton)
	{
		if(pst_GOAnim->pst_DefaultSkeleton)
			ANI_DestroySkeleton(pst_GOAnim->pst_DefaultSkeleton);
		else
			ANI_DestroySkeleton(pst_GOAnim->pst_Skeleton);
	}

	if(pst_GOAnim->pst_SkeletonModel) 
	{
#ifdef ACTIVE_EDITORS
		if(BIG_ul_SearchKeyToPos((ULONG) pst_GOAnim->pst_SkeletonModel) == -1)
#endif
			ANI_DestroySkeleton(pst_GOAnim->pst_SkeletonModel);
	}
	if(pst_GOAnim->pst_DefaultShape) 
	{
#ifdef ACTIVE_EDITORS
		if(BIG_ul_SearchKeyToPos((ULONG) pst_GOAnim->pst_DefaultShape) == -1)
#endif
			ANI_FreeShape(&pst_GOAnim->pst_DefaultShape);
	}

	if(pst_GOAnim->dpst_Stock)
		MEM_Free(pst_GOAnim->dpst_Stock);

	MEM_Free(pst_GOAnim);
}

/*
 =======================================================================================================================
    Aim:    Allocates an Anim structure.
 =======================================================================================================================
 */
ANI_tdst_Anim *ANI_AllocAnim(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_tdst_Anim	*pst_Anim;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
#if defined(_XBOX) || defined(_XENON)
	pst_Anim = (ANI_tdst_Anim *) MEM_p_AllocAlign(sizeof(ANI_tdst_Anim),16);
	L_memset(pst_Anim, 0, sizeof(ANI_tdst_Anim));
	pst_Anim->pst_Data = (EVE_tdst_Data *) MEM_p_AllocAlign(sizeof(EVE_tdst_Data),16);
	L_memset(pst_Anim->pst_Data, 0, sizeof(EVE_tdst_Data));
#else
	pst_Anim = (ANI_tdst_Anim *) MEM_p_Alloc(sizeof(ANI_tdst_Anim));
	L_memset(pst_Anim, 0, sizeof(ANI_tdst_Anim));
	pst_Anim->pst_Data = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
	L_memset(pst_Anim->pst_Data, 0, sizeof(EVE_tdst_Data));
#endif
	return pst_Anim;
}

/*
 =======================================================================================================================
    Aim:    Initialize an Anim structure with a TrackList.
 =======================================================================================================================
 */
void ANI_SetTrackList(ANI_tdst_Anim *_pst_Anim, EVE_tdst_ListTracks *_pst_TrackList)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			i;
	EVE_tdst_Params *pst_AllParams;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Anim || !_pst_Anim->pst_Data) return;
	if(!_pst_TrackList) return;

	/* Allocates the Params structure depending on the number of tracks. */
	EVE_InitData(_pst_Anim->pst_Data, _pst_TrackList);

	pst_AllParams = _pst_Anim->pst_Data->pst_ListParam;
	for(i = 0; i < _pst_TrackList->uw_NumTracks; i++)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		EVE_tdst_Track	*pst_Track;
		EVE_tdst_Params *pst_Param;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Track = &_pst_TrackList->pst_AllTracks[i];
		pst_Param = &pst_AllParams[i];

		pst_Track->uw_Flags |= EVE_C_Track_RunningInit | EVE_C_Track_AutoLoop;
		pst_Param->uw_Flags |= EVE_C_Track_Running;

		/* Forces the Flash: first Track, First Event. */

		if((i == 0) && (pst_Track->pst_AllEvents)) 
			pst_Track->pst_AllEvents->w_Flags |= EVE_C_EventFlag_Flash;


	}

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~*/
		float	f_Cur, f_Tot;
		/*~~~~~~~~~~~~~~~~~*/

		EVE_GetTracksTime(_pst_Anim->pst_Data, 0, &f_Cur, &f_Tot);

		_pst_Anim->uw_CurrentFrame = 0;
		_pst_Anim->uw_TotalFrame = EVE_TimeToFrame(f_Tot);
	}

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_ReinitAnimation(ANI_tdst_Anim *_pst_Anim)
{
	if(_pst_Anim && _pst_Anim->pst_Data)  EVE_ReinitData(_pst_Anim->pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_DestroyAnimation(ANI_tdst_Anim *_pst_Anim)
{
	if(!_pst_Anim) return;

	/* Destroy list of tracks (only if the internal counter is 0) */
	EVE_DeleteListTracks(_pst_Anim->pst_Data->pst_ListTracks);
	if(_pst_Anim->pst_Data->pst_ListParam) MEM_Free(_pst_Anim->pst_Data->pst_ListParam);
#if defined(_XBOX) || defined(_XENON)
	MEM_FreeAlign(_pst_Anim->pst_Data);
	MEM_FreeAlign(_pst_Anim);
#else
	MEM_Free(_pst_Anim->pst_Data);
	MEM_Free(_pst_Anim);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ANI_st_GameObjectAnim *ANI_pst_CreateGameObjectAnim(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pst_GOAnim = (ANI_st_GameObjectAnim *) MEM_p_Alloc(sizeof(ANI_st_GameObjectAnim));
	LOA_ul_FileTypeSize[37] += sizeof(ANI_st_GameObjectAnim);

	L_memset(pst_GOAnim, 0, sizeof(ANI_st_GameObjectAnim));
	pst_GOAnim->pst_Transition = NULL;
	pst_GOAnim->uw_CurrentActionIndex = (USHORT) - 1;
	pst_GOAnim->uc_CurrentActionItemIndex = (UCHAR) - 1;
	pst_GOAnim->c_LOD_Dynamic = 0;
	pst_GOAnim->c_LOD_IA = 0;
	pst_GOAnim->c_LOD_Bitfield = ANI_c_DefaultBitFieldLOD;
	pst_GOAnim->f_Z_Offset = 0.0f;
	pst_GOAnim->uc_ForceMode = 0;
	pst_GOAnim->dpst_Stock = (MATH_tdst_Matrix *) MEM_p_VMAlloc(ANI_C_MaxNumberOfCanal * sizeof(MATH_tdst_Matrix));
	pst_GOAnim->aul_PartialMask[0] = 0;
	pst_GOAnim->aul_PartialMask[1] = 0;

	pst_GOAnim->uc_BlendLength_IN = 10;
	pst_GOAnim->uc_BlendLength_OUT = 10;

	return pst_GOAnim;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_InitSkeleton(OBJ_tdst_Group *_pst_Skeleton, OBJ_tdst_GameObject *_pst_RefGO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
	OBJ_tdst_GameObject *pst_BoneObject;
	ULONG				i, j, ul_MatrixID;
#ifdef ACTIVE_EDITORS
	char				sz_GizmoName[50];
	char				*psz_Temp;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Skeleton->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Skeleton->pst_AllObjects);
	for(i = 0; pst_CurrentElem <= pst_EndElem; i++, pst_CurrentElem++)
	{
		pst_BoneObject = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if(TAB_b_IsAHole(pst_BoneObject)) continue;

		/* Sets the Waypoint as the "No Father Bones"' Father. */
		if(!OBJ_b_TestIdentityFlag(pst_BoneObject, OBJ_C_IdentityFlag_Hierarchy))
		{
			OBJ_ChangeIdentityFlags
			(
				pst_BoneObject,
				OBJ_ul_FlagsIdentityGet(pst_BoneObject) | OBJ_C_IdentityFlag_Hierarchy,
				OBJ_ul_FlagsIdentityGet(pst_BoneObject)
			);
		}

		if(!pst_BoneObject->pst_Base->pst_Hierarchy->pst_Father)
		{
			pst_BoneObject->pst_Base->pst_Hierarchy->pst_FatherInit = _pst_RefGO;
			pst_BoneObject->pst_Base->pst_Hierarchy->pst_Father = _pst_RefGO;

			//POPOWARNING XENON -> ?? if(!OBJ_b_TestIdentityFlag(pst_BoneObject, OBJ_C_IdentityFlag_HasInitialPos))
			if((i == 0) && !OBJ_b_TestIdentityFlag(pst_BoneObject, OBJ_C_IdentityFlag_HasInitialPos))
			{
				OBJ_ChangeIdentityFlags
				(
					pst_BoneObject,
					OBJ_ul_FlagsIdentityGet(pst_BoneObject) | OBJ_C_IdentityFlag_HasInitialPos,
					OBJ_ul_FlagsIdentityGet(pst_BoneObject)
				);
				MATH_CopyMatrix(pst_BoneObject->pst_GlobalMatrix + 1, &MATH_gst_IdentityMatrix);
				MATH_CopyVector(&(pst_BoneObject->pst_GlobalMatrix + 1)->T, &MATH_gst_NulVector);
			}

			MATH_CopyMatrix
			(
				&pst_BoneObject->pst_Base->pst_Hierarchy->st_LocalMatrix,
				pst_BoneObject->pst_GlobalMatrix + 1
			);
		}

		/* Sets the Bone Flag. */
		pst_BoneObject->ul_IdentityFlags |= OBJ_C_IdentityFlag_Bone;

		if((OBJ_b_TestIdentityFlag(pst_BoneObject, OBJ_C_IdentityFlag_AdditionalMatrix)))
		{
			/* Update the bone gizmo pointer structure to a Reference Gizmo. */
			for(j = 0; j < (ULONG) pst_BoneObject->pst_Base->pst_AddMatrix->l_Number; j++)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				OBJ_tdst_GizmoPtr	*pst_GizmoPtr;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_GizmoPtr = (pst_BoneObject->pst_Base->pst_AddMatrix->dst_GizmoPtr + j);
				ul_MatrixID = pst_GizmoPtr->l_MatrixId;
				pst_GizmoPtr->pst_GO = _pst_RefGO;
				pst_GizmoPtr->pst_Matrix = &(_pst_RefGO->pst_Base->pst_AddMatrix->dst_Gizmo + ul_MatrixID)->st_Matrix;
			}

#ifdef ACTIVE_EDITORS
			if(!L_strlen((_pst_RefGO->pst_Base->pst_AddMatrix->dst_Gizmo + i)->sz_Name))
			{
				L_strcpy(sz_GizmoName, pst_BoneObject->sz_Name);
				psz_Temp = L_strrchr(sz_GizmoName, '@');
				if(psz_Temp) *psz_Temp = 0;
				L_strcpy((_pst_RefGO->pst_Base->pst_AddMatrix->dst_Gizmo + i)->sz_Name, sz_GizmoName);
			}
#endif
		}
	}

	/* Reinit Stock Matrix */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
		OBJ_tdst_Group			*pst_Skeleton;
		OBJ_tdst_GameObject		*pst_BoneGO;
		int						i;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Skeleton = _pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
		pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
		for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
		{
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
			ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));
			
			MATH_CopyMatrix(&_pst_RefGO->pst_Base->pst_GameObjectAnim->dpst_Stock[i], OBJ_pst_GetLocalMatrix(pst_BoneGO));		
		}
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_pst_DuplicateGameObjectAnim
(
	WOR_tdst_World		*_pst_World,
	OBJ_tdst_GameObject *_pst_GODest,
	OBJ_tdst_GameObject *_pst_GOSrc
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group			*pst_Skeleton;
	ANI_st_GameObjectAnim	*pst_GOAnimDest;
	ANI_st_GameObjectAnim	*pst_GOAnimSrc;
	MATH_tdst_Matrix		*pst_Mat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GOAnimDest = _pst_GODest->pst_Base->pst_GameObjectAnim;
	pst_GOAnimSrc = _pst_GOSrc->pst_Base->pst_GameObjectAnim;

	/* rajouter temporairement pour éviter les memory leaks sur les track list */
	if(pst_GOAnimSrc->pst_ActionKit) ACT_ActionRestoreInit(pst_GOAnimSrc->pst_ActionKit);

	pst_Mat = pst_GOAnimDest->dpst_Stock;

	L_memcpy(pst_GOAnimDest, pst_GOAnimSrc, sizeof(ANI_st_GameObjectAnim));

	pst_GOAnimDest->dpst_Stock = pst_Mat;

	/* Dont want to copy the Blend Anim if there is one. */
	pst_GOAnimDest->uc_AnimUsed &= ~2;
	pst_GOAnimDest->apst_Anim[1] = NULL;

	pst_GOAnimDest->apst_PartialAnim[0] = NULL;
	pst_GOAnimDest->apst_PartialAnim[1] = NULL;
	pst_GOAnimDest->apst_PartialAnim[2] = NULL;
	pst_GOAnimDest->apst_PartialAnim[3] = NULL;


	pst_GOAnimDest->aul_PartialMask[0] = 0;
	pst_GOAnimDest->aul_PartialMask[1] = 0;

	pst_GOAnimDest->uc_BlendLength_IN = 10;
	pst_GOAnimDest->uc_BlendLength_OUT = 10;

	if(!pst_GOAnimDest->dpst_Stock)
		pst_GOAnimDest->dpst_Stock = (MATH_tdst_Matrix *) MEM_p_Alloc(ANI_C_MaxNumberOfCanal * sizeof(MATH_tdst_Matrix));

	/* Resets the Current Action to avoid a #$@!% blend. */
	pst_GOAnimDest->uw_CurrentActionIndex = (USHORT) - 1;
	pst_GOAnimDest->uc_CurrentActionItemIndex = (UCHAR) - 1;

	/* Duplicate the Anim Info */
	if(pst_GOAnimDest->uc_AnimUsed)
	{
		pst_GOAnimDest->apst_Anim[0] = ANI_AllocAnim();
		pst_GOAnimDest->apst_Anim[0]->uc_AnimFrequency = pst_GOAnimSrc->apst_Anim[0]->uc_AnimFrequency;
		pst_GOAnimDest->apst_Anim[0]->uc_BlendedActionItem = 0;
		pst_GOAnimDest->apst_Anim[0]->uc_BlendFlags = 0;
		pst_GOAnimDest->apst_Anim[0]->uc_BlendTime = 0;
		pst_GOAnimDest->apst_Anim[0]->uc_MiscFlags = 0;
		pst_GOAnimDest->apst_Anim[0]->uw_BlendCurTime = 0;
		pst_GOAnimDest->apst_Anim[0]->uw_BlendedAction = 0;
		pst_GOAnimDest->apst_Anim[0]->uw_Flag = pst_GOAnimSrc->apst_Anim[0]->uw_Flag;
		ANI_SetTrackList(pst_GOAnimDest->apst_Anim[0], pst_GOAnimSrc->apst_Anim[0]->pst_Data->pst_ListTracks);
	}

	if(pst_GOAnimSrc->pst_SkeletonModel)
	{
		pst_Skeleton = GRP_pst_CreateNewGroup();
#ifdef JADEFUSION
		GRP_DuplicateGroupObjects(_pst_World, pst_GOAnimSrc->pst_SkeletonModel, pst_Skeleton, eLRLSpawing);
#else
		GRP_DuplicateGroupObjects(_pst_World, pst_GOAnimSrc->pst_SkeletonModel, pst_Skeleton);
#endif
		UpdateModifiersGroup(_pst_GODest, pst_Skeleton);
		pst_GOAnimDest->pst_Skeleton = pst_Skeleton;
		ANI_UseSkeleton(pst_GOAnimDest->pst_Skeleton);
		ANI_UseSkeleton(pst_GOAnimDest->pst_SkeletonModel);
		ANI_InitSkeleton(pst_GOAnimDest->pst_Skeleton, _pst_GODest);
	}

	if(pst_GOAnimSrc->pst_Shape)
	{
		pst_GOAnimDest->pst_DefaultShape = pst_GOAnimSrc->pst_DefaultShape;
		pst_GOAnimDest->pst_Shape = pst_GOAnimSrc->pst_Shape;
		ANI_UseShape(pst_GOAnimDest->pst_DefaultShape);
		ANI_ApplyShape(_pst_GODest);
	}
}

/*
 =======================================================================================================================
    Aim:    Reinit the GameObjectAnim.
 =======================================================================================================================
 */
void ANI_Reinit(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim *pst_GOAnim;
	ULONG					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	if(!pst_GOAnim) return;

	/* No transition */
	pst_GOAnim->pst_Transition = NULL;
	pst_GOAnim->uw_CurrentActionIndex = (USHORT) - 1;
	pst_GOAnim->uc_CurrentActionItemIndex = 0;
	pst_GOAnim->uc_CounterForRepetition = 0;
	pst_GOAnim->uw_NextActionIndex = (USHORT) - 1;
	pst_GOAnim->c_LOD_Dynamic = 0;
//	pst_GOAnim->c_LOD_IA = 0;
	pst_GOAnim->c_LOD_Bitfield = ANI_c_DefaultBitFieldLOD;
//	pst_GOAnim->f_Z_Offset = 0.0f;
	pst_GOAnim->uc_ForceMode = 0;
	pst_GOAnim->uc_PauseAnim = 0;



	for(i = 0; i < ANI_C_MaxNumberOfSimultaneousAnim; i++)
	{
		if(pst_GOAnim->uc_AnimUsed & (1 << i)) ANI_ReinitAnimation(pst_GOAnim->apst_Anim[i]);
	}

	for(i = 0; i < ANI_C_MaxNumberOfSimultaneousAnim; i++)
	{
		if(pst_GOAnim->apst_PartialAnim[i]) 
		{
			if(pst_GOAnim->apst_PartialAnim[i]->pst_Data)
			{
				if(pst_GOAnim->apst_PartialAnim[i]->pst_Data->pst_ListParam) MEM_Free(pst_GOAnim->apst_PartialAnim[i]->pst_Data->pst_ListParam);
#if defined(_XBOX) || defined(_XENON)
				MEM_FreeAlign(pst_GOAnim->apst_PartialAnim[i]->pst_Data);
#else
				MEM_Free(pst_GOAnim->apst_PartialAnim[i]->pst_Data);
#endif
			}
#if defined(_XBOX) || defined(_XENON)
			MEM_FreeAlign(pst_GOAnim->apst_PartialAnim[i]);
#else
			MEM_Free(pst_GOAnim->apst_PartialAnim[i]);
#endif

			pst_GOAnim->apst_PartialAnim[i] = NULL;
		}
	}

	pst_GOAnim->uw_CurrentPartialActionIndex = 0;
	pst_GOAnim->uc_CurrentPartialActionItemIndex = 0;

	pst_GOAnim->aul_PartialMask[0] = 0;
	pst_GOAnim->aul_PartialMask[1] = 0;

	pst_GOAnim->uc_BlendLength_IN = 10;
	pst_GOAnim->uc_BlendLength_OUT = 10;


	pst_GOAnim->uc_AnimUsed &= 1;

	/* Action kit */
	if(pst_GOAnim->pst_ActionKit) ACT_ActionRestoreInit(pst_GOAnim->pst_ActionKit);

	/* Shape */
	if(pst_GOAnim->pst_DefaultShape)
	{
		pst_GOAnim->pst_Shape = pst_GOAnim->pst_DefaultShape;
		ANI_ApplyShape(_pst_GO);
	}

	/* Skeleton */
	if(pst_GOAnim->pst_DefaultSkeleton)
	{
		pst_GOAnim->pst_Skeleton = pst_GOAnim->pst_DefaultSkeleton;
		pst_GOAnim->pst_DefaultSkeleton = NULL;
	}

	/* Reinit Stock Matrix */
	if(pst_GOAnim->pst_Skeleton)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
		OBJ_tdst_Group			*pst_Skeleton;
		OBJ_tdst_GameObject		*pst_BoneGO;
		int						i;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Skeleton = pst_GOAnim->pst_Skeleton;
		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
		pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
		for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
		{
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
			ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));

			if(!(pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
				continue;
			
			MATH_CopyMatrix(&pst_GOAnim->dpst_Stock[i], OBJ_pst_GetLocalMatrix(pst_BoneGO));		
		}
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *ANI_pst_GetReference(OBJ_tdst_GameObject *_pst_BoneGO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_CurrentGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((!OBJ_b_TestIdentityFlag(_pst_BoneGO, OBJ_C_IdentityFlag_Bone))
#ifdef ACTIVE_EDITORS
#ifdef JADEFUSION
		&& (!(_pst_BoneGO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject))
#else
		&& (!_pst_BoneGO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
#endif
#endif
	) return _pst_BoneGO;

	pst_CurrentGO = _pst_BoneGO;
	while(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_Bone)
#ifdef ACTIVE_EDITORS
	|| (pst_CurrentGO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
#endif
	)
	{
		if
		(
			pst_CurrentGO->pst_Base
		&&	pst_CurrentGO->pst_Base->pst_Hierarchy
		&&	pst_CurrentGO->pst_Base->pst_Hierarchy->pst_FatherInit
		) pst_CurrentGO = pst_CurrentGO->pst_Base->pst_Hierarchy->pst_FatherInit;
		else
			return _pst_BoneGO;
	}

	return pst_CurrentGO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *ANI_pst_GetReferenceInit(OBJ_tdst_GameObject *_pst_BoneGO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_CurrentGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((!OBJ_b_TestIdentityFlag(_pst_BoneGO, OBJ_C_IdentityFlag_Bone))
#ifdef ACTIVE_EDITORS
	&& (!_pst_BoneGO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
#endif
	) return _pst_BoneGO;

	pst_CurrentGO = _pst_BoneGO;
	while(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_Bone)
#ifdef ACTIVE_EDITORS
	|| (pst_CurrentGO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
#endif
	)
	{
		if
		(
			pst_CurrentGO->pst_Base
		&&	pst_CurrentGO->pst_Base->pst_Hierarchy
		&&	pst_CurrentGO->pst_Base->pst_Hierarchy->pst_FatherInit
		) pst_CurrentGO = pst_CurrentGO->pst_Base->pst_Hierarchy->pst_FatherInit;
		else
			return _pst_BoneGO;
	}

	return pst_CurrentGO;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_ChangeDefaultSkeleton(OBJ_tdst_GameObject *_pst_GO1, OBJ_tdst_GameObject *_pst_GO2)
{
	if
	(
		_pst_GO1
	&&	(_pst_GO1->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	&&	_pst_GO1->pst_Base
	&&	_pst_GO1->pst_Base->pst_GameObjectAnim
	&&	_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton
	&&	_pst_GO2
	&&	(_pst_GO2->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	&&	_pst_GO2->pst_Base
	&&	_pst_GO2->pst_Base->pst_GameObjectAnim
	&&	_pst_GO2->pst_Base->pst_GameObjectAnim->pst_Skeleton
	)
	{
		if(!_pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultSkeleton)
		{
			_pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultSkeleton = _pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton;
		}

		_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton = _pst_GO2->pst_Base->pst_GameObjectAnim->pst_Skeleton;
		ANI_InitSkeleton(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton, _pst_GO1);
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
			OBJ_tdst_GameObject *pst_BoneObject;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
			pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
			for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
				pst_BoneObject = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
				if(TAB_b_IsAHole(pst_BoneObject)) continue;

				if(pst_BoneObject->pst_Base->pst_Hierarchy->pst_Father == _pst_GO2)
				{
					pst_BoneObject->pst_Base->pst_Hierarchy->pst_FatherInit = _pst_GO1;
					pst_BoneObject->pst_Base->pst_Hierarchy->pst_Father = _pst_GO1;
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_RestoreDefaultSkeleton(OBJ_tdst_GameObject *_pst_GO1, OBJ_tdst_GameObject *_pst_GO2)
{
	if
	(
		_pst_GO1
	&&	(_pst_GO1->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	&&	_pst_GO1->pst_Base
	&&	_pst_GO1->pst_Base->pst_GameObjectAnim
	&&	_pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultSkeleton
	&&	_pst_GO2
	&&	(_pst_GO2->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	&&	_pst_GO2->pst_Base
	&&	_pst_GO2->pst_Base->pst_GameObjectAnim
	&&	_pst_GO2->pst_Base->pst_GameObjectAnim->pst_Skeleton
	)
	{
		_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton = _pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultSkeleton;
		_pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultSkeleton = NULL;
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
			OBJ_tdst_GameObject *pst_BoneObject;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_GO2->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
			pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_GO2->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
			for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
				pst_BoneObject = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
				if(TAB_b_IsAHole(pst_BoneObject)) continue;

				if(pst_BoneObject->pst_Base->pst_Hierarchy->pst_Father == _pst_GO1)
				{
					pst_BoneObject->pst_Base->pst_Hierarchy->pst_FatherInit = _pst_GO2;
					pst_BoneObject->pst_Base->pst_Hierarchy->pst_Father = _pst_GO2;
				}
			}
		}
	}
}

extern void ANI_PlayLeadAnimation(OBJ_tdst_GameObject *, ANI_st_GameObjectAnim *, ANI_tdst_Anim *);
extern BOOL EVE_gb_ForcePlayerOnAllTracks;

/*$F
 =======================================================================================================================
 Aim: Copy the Skeleton and the Shape of GO2 into GO1. Tries also to set a default animation.
 =======================================================================================================================
 */

void ANI_CloneSet(OBJ_tdst_GameObject *_pst_GO1, OBJ_tdst_GameObject *_pst_GO2, ULONG _ul_NumAction)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_tdst_Shape	*pst_GO2_Shape;
	OBJ_tdst_Group	*pst_GO2_Skeleton, *pst_GO1_Skeleton;
	WOR_tdst_World	*pst_World;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!(_pst_GO2->ul_IdentityFlags & OBJ_C_IdentityFlag_BaseObject)
	||	!(_pst_GO2->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	||	!_pst_GO2->pst_Base
	||	!_pst_GO2->pst_Base->pst_GameObjectAnim
	||	!_pst_GO2->pst_Base->pst_GameObjectAnim->pst_Skeleton
	) return;

	/* Visual is not compatible with Animation. If GO1 has a visual, return; */
	if(_pst_GO1->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu) return;

	pst_World = WOR_World_GetWorldOfObject(_pst_GO2);
	if(!pst_World) return;
	/* Alloc Base structure if necessary. */
	if(!(_pst_GO1->ul_IdentityFlags & OBJ_C_IdentityFlag_BaseObject))
	{
		_pst_GO1->pst_Base = (OBJ_tdst_Base *) MEM_p_Alloc(sizeof(OBJ_tdst_Base));
		L_memset(_pst_GO1->pst_Base, 0, sizeof(OBJ_tdst_Base));

		_pst_GO1->ul_IdentityFlags |= OBJ_C_IdentityFlag_BaseObject;
	}

	/* Alloc GameObjectAnim structure if necessary. */
	if(!(_pst_GO1->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims))
	{
		_pst_GO1->pst_Base->pst_GameObjectAnim = ANI_pst_CreateGameObjectAnim();
		_pst_GO1->ul_IdentityFlags |= OBJ_C_IdentityFlag_Anims;
	}
	else
	{
		/* Free previous Skeleton/Shape */
		if(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton)
		{
			if(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultSkeleton)
				ANI_DestroySkeleton(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultSkeleton);
			//else
				ANI_DestroySkeleton(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton);
		}

		if(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_SkeletonModel)
			ANI_DestroySkeleton(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_SkeletonModel);

		if(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultShape)
			ANI_FreeShape(&_pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultShape);		
		
		/* Free action kit */
		if (_pst_GO1->pst_Base->pst_GameObjectAnim->pst_ActionKit)
			ACT_FreeActionKit(&_pst_GO1->pst_Base->pst_GameObjectAnim->pst_ActionKit);

		/*if (_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Transition)
			MEM_Free(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Transition);*/

		/*if (_pst_GO1->pst_Base->pst_GameObjectAnim->pst_CurrentAction)
			ACT_FreeAction(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_CurrentAction);*/

		else if
		(
			_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0]
		&&	_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data
		&&	_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks
		)
		{
			ANI_DestroyAnimation(_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0]);
			_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0] = NULL;
		}

	}

	pst_GO2_Skeleton = _pst_GO2->pst_Base->pst_GameObjectAnim->pst_SkeletonModel;
	pst_GO2_Shape = _pst_GO2->pst_Base->pst_GameObjectAnim->pst_DefaultShape;

	if(pst_GO2_Skeleton)
	{
		_pst_GO1->pst_Base->pst_GameObjectAnim->pst_SkeletonModel = pst_GO2_Skeleton;
		ANI_UseSkeleton(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_SkeletonModel);
		pst_GO1_Skeleton = GRP_pst_CreateNewGroup();
		GRP_DuplicateGroupObjects(pst_World, pst_GO2_Skeleton, pst_GO1_Skeleton);
		UpdateModifiersGroup(_pst_GO1, pst_GO1_Skeleton);
		_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton = pst_GO1_Skeleton;
		ANI_UseSkeleton(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton);

		OBJ_GameObject_AllocateGizmo(_pst_GO1, pst_GO2_Skeleton->pst_AllObjects->ul_NbElems, FALSE);

		ANI_InitSkeleton(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Skeleton, _pst_GO1);
	}

	if(pst_GO2_Shape)
	{
		_pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultShape = _pst_GO2->pst_Base->pst_GameObjectAnim->pst_DefaultShape;
		_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Shape = _pst_GO2->pst_Base->pst_GameObjectAnim->pst_Shape;
		L_memcpy(_pst_GO1->pst_Base->pst_GameObjectAnim->auc_DefaultVisu, _pst_GO2->pst_Base->pst_GameObjectAnim->auc_DefaultVisu, ANI_C_MaxNumberOfCanal * sizeof(UCHAR));
		ANI_UseShape(_pst_GO1->pst_Base->pst_GameObjectAnim->pst_DefaultShape);
		ANI_ApplyShape(_pst_GO1);
	}

	if((_ul_NumAction != (ULONG) - 1) && (_pst_GO2->pst_Base->pst_GameObjectAnim->pst_ActionKit))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		ACT_st_Action	*pst_Action;
		float			f_DT;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		//_pst_GO1->pst_Base->pst_GameObjectAnim->pst_ActionKit = (ACT_st_ActionKit *) MEM_p_Alloc(sizeof(ACT_st_ActionKit));

		ACT_UseActionKit(_pst_GO2->pst_Base->pst_GameObjectAnim->pst_ActionKit);
		_pst_GO1->pst_Base->pst_GameObjectAnim->pst_ActionKit = _pst_GO2->pst_Base->pst_GameObjectAnim->pst_ActionKit;
		/*_pst_GO1->pst_Base->pst_GameObjectAnim->pst_Transition = _pst_GO2->pst_Base->pst_GameObjectAnim->pst_Transition;
		_pst_GO1->pst_Base->pst_GameObjectAnim->pst_CurrentAction = _pst_GO2->pst_Base->pst_GameObjectAnim->pst_CurrentAction;
		_pst_GO1->pst_Base->pst_GameObjectAnim->uw_CurrentActionIndex = _pst_GO2->pst_Base->pst_GameObjectAnim->uw_CurrentActionIndex ;
*/
		if(_pst_GO2->pst_Base->pst_GameObjectAnim->pst_ActionKit->ppst_ActionInits)
			pst_Action = _pst_GO2->pst_Base->pst_GameObjectAnim->pst_ActionKit->ppst_ActionInits[_ul_NumAction];
		else
		{
			if(_pst_GO2->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action)
				pst_Action = _pst_GO2->pst_Base->pst_GameObjectAnim->pst_ActionKit->apst_Action[_ul_NumAction];
			else
				pst_Action = NULL;
		}

		if(pst_Action && pst_Action->ast_ActionItem && pst_Action->ast_ActionItem[0].pst_TrackList)
		{
			if(!_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0])
				_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0] = ANI_AllocAnim();

#ifdef ANIMS_USE_ARAM
    		/* If TrackList of ActionItem is not in Cache (and should be), Load it From ARAM */
    		if(pst_Action->ast_ActionItem[0].pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_UseARAM)
    		{        		
    			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                extern int ACT_i_Cache_LoadAnim(ACT_st_ActionKit *, EVE_tdst_ListTracks *);
    			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    			ACT_i_Cache_LoadAnim(_pst_GO2->pst_Base->pst_GameObjectAnim->pst_ActionKit, pst_Action->ast_ActionItem[0].pst_TrackList);
    			pst_Action->ast_ActionItem[0].pst_TrackList->pst_Cache->l_Shares ++;
    			
    		}
#endif

			//pst_Action->ast_ActionItem[0].pst_TrackList->ul_NbOfInstances++;
			ANI_SetTrackList
			(
				_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0],
				pst_Action->ast_ActionItem[0].pst_TrackList
			);
			_pst_GO1->pst_Base->pst_GameObjectAnim->uc_AnimUsed = 1;

			/* Sets a default Flash Matrix */
			OBJ_SetFlashMatrix(_pst_GO1, OBJ_pst_GetAbsoluteMatrix(_pst_GO1));

			_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0]->uc_AnimFrequency = ANI_C_DefaultAnimFrequency;

			/* - Play first frame - */
			f_DT = TIM_gf_dt;
			TIM_gf_dt = 0.0f;
			EVE_gb_ForcePlayerOnAllTracks = TRUE;
			ANI_PlayLeadAnimation
			(
				_pst_GO1,
				_pst_GO1->pst_Base->pst_GameObjectAnim,
				_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0]
			);
			EVE_gb_ForcePlayerOnAllTracks = FALSE;
			TIM_gf_dt = f_DT;

			/* No longer play this Animation */
			_pst_GO1->pst_Base->pst_GameObjectAnim->apst_Anim[0]->uc_AnimFrequency = 0;
		}
	}
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ANI_b_IsGizmoAnim(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject **_pst_Father)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GOFather;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
		pst_GOFather = ANI_pst_GetReference(_pst_GO);
	else
		pst_GOFather = _pst_GO;

	if
	(
		(pst_GOFather->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	&&	(pst_GOFather->ul_EditorFlags & OBJ_C_EditFlags_ShowGizmo)
	&&	(pst_GOFather->pst_Base)
	&&	(pst_GOFather->pst_Base->pst_GameObjectAnim)
	&&	(pst_GOFather->pst_Base->pst_GameObjectAnim->apst_Anim[0])
	)
	{
		*_pst_Father = pst_GOFather;
		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ANI_i_CurFrame(OBJ_tdst_GameObject *_pst_GAO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	EVE_tdst_Track			*pst_Track;
	EVE_tdst_Event			*pst_Event;
	float					t;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GAO) return 0;

	pst_GOAnim = _pst_GAO->pst_Base->pst_GameObjectAnim;
	pst_Track = &pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks->pst_AllTracks[0];

	pst_Event = pst_Track->pst_AllEvents;
	t = 0;
	while(pst_Event != pst_Track->pst_AllEvents + pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListParam[0].uw_CurrentEvent)
	{
		t += EVE_FrameToTime(pst_Event->uw_NumFrames);
		pst_Event++;
	}

	t += pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListParam[0].f_Time;
	return (int) ((t * 60.0f) + 0.5f);
}

/*
 =======================================================================================================================
    Aim:    Determines wether a given bone has an associated Anim Track or not.
 =======================================================================================================================
 */
BOOL ANI_b_BoneHasTrack(OBJ_tdst_GameObject *_pst_BoneGO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_RefGO;
	EVE_tdst_ListTracks *pst_ListTracks;
	OBJ_tdst_Group		*pst_Skeleton;
	EVE_tdst_Track		*pst_Track;
	ULONG				ul_Gizmo;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(OBJ_b_TestIdentityFlag(_pst_BoneGO, OBJ_C_IdentityFlag_Anims)) return TRUE;
	if(!(OBJ_b_TestIdentityFlag(_pst_BoneGO, OBJ_C_IdentityFlag_Bone))) return FALSE;
	pst_RefGO = ANI_pst_GetReference(_pst_BoneGO);
	if
	(
		!pst_RefGO
	||	!pst_RefGO->pst_Base
	||	!(pst_RefGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	||	!pst_RefGO->pst_Base->pst_GameObjectAnim
	||	!pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton
	||	!pst_RefGO->pst_Base->pst_GameObjectAnim->apst_Anim[0]
	||	!pst_RefGO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data
	||	!pst_RefGO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks
	) return FALSE;

	pst_ListTracks = pst_RefGO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks;
	pst_Skeleton = pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton;

	ul_Gizmo = TAB_ul_PFtable_GetElemIndexWithPointer(pst_Skeleton->pst_AllObjects, _pst_BoneGO);

	for(i = 0; i < pst_ListTracks->uw_NumTracks; i++)
	{
		pst_Track = &pst_ListTracks->pst_AllTracks[i];
		if(pst_Track->uw_Gizmo == ul_Gizmo) return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Determines wether a given gizmo object has an associated Anim Track or not.
 =======================================================================================================================
 */
BOOL ANI_b_GizmoHasTrack(OBJ_tdst_GameObject *_pst_GizmoGO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_BoneGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GizmoGO || !_pst_GizmoGO->pst_World) return FALSE;
	pst_BoneGO = WOR_pst_GetBoneFromGizmo(_pst_GizmoGO);
	if(!pst_BoneGO) return FALSE;

	return(ANI_b_BoneHasTrack(pst_BoneGO));
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

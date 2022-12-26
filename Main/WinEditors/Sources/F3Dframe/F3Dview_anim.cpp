/*$T F3Dview_anim.cpp GC! 1.081 12/08/00 15:50:52 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

 
#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/ERROrs/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/ACTions/ACTinit.h"
#include "ENGine/Sources/ACTions/ACTcompute.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "AIinterp/Sources/Events/EVEsave.h"
#include "LINKs/LINKtoed.h"
#include "EDIpaths.h"
#include "F3Dview.h"
#include "Res/Res.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "GraphicDK/Sources/SELection/SELection.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAanim_dlg.h"
#include "EDIstrings.h"
#include "EDItors/Sources/OUTput/OUTframe_act.h"
#include "EDItors/Sources/EVEnts/EVEmsg.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "DIAlogs/DIAname_dlg.h"

#ifdef JADEFUSION
extern char SOFT_gac_BoneCurveStatus[64];
#else
extern "C" char SOFT_gac_BoneCurveStatus[64];
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropShape(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Device					*pst_Dev;
	OBJ_tdst_GameObject				*pst_GO;
	BIG_KEY							ul_Key;
	BIG_INDEX						ul_ShapeIndex;
	ANI_st_GameObjectAnim			*pst_GOAnim;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	WOR_tdst_World					*pst_World;
	ANI_tdst_Shape					*pst_Shape;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find the game object where to drop anim */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);

	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();

	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
	if(ul_Key == BIG_C_InvalidIndex) return;

	/* Create a game object anim and set it to the game object */
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		OBJ_ChangeIdentityFlags
		(
			pst_GO,
			pst_GO->ul_IdentityFlags &~OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_BaseObject | OBJ_C_IdentityFlag_Anims,
			pst_GO->ul_IdentityFlags
		);
	}

	pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;

	/* Load the Shape */
	ul_ShapeIndex = _pst_DragDrop->ul_FatFile;
	LOA_MakeFileRef(BIG_FileKey(ul_ShapeIndex), (ULONG *) &pst_Shape, ANI_ul_ShapeCallback, LOA_C_MustExists);
	LOA_Resolve();

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	ERR_X_Assert(pst_World);

	if(pst_Shape == pst_GOAnim->pst_Shape) return;

	if(pst_GOAnim->pst_DefaultShape) ANI_FreeShape(&pst_GOAnim->pst_DefaultShape);
	pst_GOAnim->pst_DefaultShape = pst_Shape;

	/* Inializing Default Visuals */
	L_memcpy(pst_GOAnim->auc_DefaultVisu, pst_Shape->auc_Visu, sizeof(UCHAR) * ANI_C_MaxNumberOfCanal);

	ANI_UseShape(pst_Shape);

	ANI_ChangeShape(pst_GO, pst_Shape);
	if(mpo_ToolBoxDialog) mpo_ToolBoxDialog->SelectGaoFromPtr(Selection_pst_GetFirstObject());
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropTrackList(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Device					*pst_Dev;
	OBJ_tdst_GameObject				*pst_GO;
	BIG_KEY							ul_Key;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	ULONG							ul_Adr;
	EVE_tdst_ListTracks				*pst_All;
	BIG_INDEX						ul_TrackListIndex;
	ANI_st_GameObjectAnim			*pst_GOAnim;
	WOR_tdst_World					*pst_World;
	EVE_tdst_ListTracks				*pst_TrackList;
	EDIA_cl_NameDialogCombo			o_Dialog("Selection");
	BOOL							b_Anim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find the game object where to drop track list */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);

	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();

	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
	if(ul_Key == BIG_C_InvalidIndex) return;

	o_Dialog.AddItem("Anims");
	o_Dialog.AddItem("Events");

	o_Dialog.SetDefault("Anims");

	if(o_Dialog.DoModal() == IDOK)
	{
		b_Anim = (!L_strcmpi((char *) (LPCSTR) o_Dialog.mo_Name, "Anims"));
	}
	else
		return;

	if(!b_Anim)
	{
		/* Load track list */
		LOA_MakeFileRef
		(
			BIG_FileKey(_pst_DragDrop->ul_FatFile),
			&ul_Adr,
			EVE_ul_CallbackLoadListTracks,
			LOA_C_MustExists
		);
		LOA_Resolve();
		pst_All = (EVE_tdst_ListTracks *) ul_Adr;
		ERR_X_Assert(pst_All);
		EVE_ResolveListTrackRef(pst_All);

		pst_All->ul_NbOfInstances++;

		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Events)) 
		{
			EVE_FreeMainStruct(pst_GO);
			pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Events;
		}

		/* Object with Anims - Delete Anim to set Events */
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			ANI_st_GameObjectAnim	*pst_GOAnim;
			int						i;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;
			if(pst_GOAnim->pst_ActionKit)
			{
				ACT_FreeActionKit(&pst_GOAnim->pst_ActionKit);
				for(i = 0; i < ANI_C_MaxNumberOfSimultaneousAnim; i++)
				{
					if(pst_GOAnim->apst_Anim[i] && ((ULONG) pst_GOAnim->apst_Anim[i] != (ULONG) - 1))
					{
						if(pst_GOAnim->apst_Anim[i]->pst_Data->pst_ListParam)
							MEM_Free(pst_GOAnim->apst_Anim[i]->pst_Data->pst_ListParam);
						MEM_Free(pst_GOAnim->apst_Anim[i]->pst_Data);
						MEM_Free(pst_GOAnim->apst_Anim[i]);
					}
				}
			}
			else
			{
				for(i = 0; i < ANI_C_MaxNumberOfSimultaneousAnim; i++)
				{
					if((pst_GOAnim->apst_Anim[i]) && ((ULONG) pst_GOAnim->apst_Anim[i] != (ULONG) - 1))
						ANI_DestroyAnimation(pst_GOAnim->apst_Anim[i]);
				}
			}

			if(pst_GOAnim->pst_Skeleton) ANI_DestroySkeleton(pst_GOAnim->pst_Skeleton);
			if(pst_GOAnim->pst_SkeletonModel) ANI_DestroySkeleton(pst_GOAnim->pst_SkeletonModel);
			if(pst_GOAnim->pst_Shape) ANI_FreeShape(&pst_GOAnim->pst_Shape);

			MEM_Free(pst_GOAnim);
			pst_GO->pst_Base->pst_GameObjectAnim = NULL;
			pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Anims;
		}

		if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject))
		{
			OBJ_GameObject_CreateExtended(pst_GO);	
			pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_ExtendedObject;
		}

		pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_Events;

		pst_GO->pst_Extended->pst_Events = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
		L_memset(pst_GO->pst_Extended->pst_Events, 0, sizeof(EVE_tdst_Data));

		pst_GO->pst_Extended->pst_Events->pst_ListTracks = NULL;
		pst_GO->pst_Extended->pst_Events->pst_ListParam = NULL;

		EVE_InitData(pst_GO->pst_Extended->pst_Events, pst_All);
	}
	else
	{
		/* Create a game object anim and set it to the game object */
		if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
		{
			OBJ_ChangeIdentityFlags
			(
				pst_GO,
				pst_GO->ul_IdentityFlags &~OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_BaseObject | OBJ_C_IdentityFlag_Anims,
				pst_GO->ul_IdentityFlags
			);
		}

		pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;
		if(pst_GOAnim->pst_ActionKit) ACT_FreeActionKit(&pst_GOAnim->pst_ActionKit);
		pst_GOAnim->uc_AnimUsed = 1;	/* Only the first anim is used */

		/* Load the anim */
		ul_TrackListIndex = _pst_DragDrop->ul_FatFile;
		LOA_MakeFileRef
		(
			BIG_FileKey(ul_TrackListIndex),
			(ULONG *) &pst_TrackList,
			EVE_ul_CallbackLoadListTracks,
			LOA_C_MustExists
		);
		LOA_Resolve();

		if
		(
			pst_GOAnim->apst_Anim[0]
		&&	pst_GOAnim->apst_Anim[0]->pst_Data
		&&	pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks
		&&	(pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks != pst_TrackList)
		)
		{
			EVE_DeleteListTracks(pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks);

			/* Update GO */
			LINK_UpdatePointer(pst_GO);
			LINK_UpdatePointers();

			M_MF()->DataHasChanged();
		}

		pst_TrackList->ul_NbOfInstances++;

		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		ERR_X_Assert(pst_World);

		if(!pst_GOAnim->apst_Anim[0]) pst_GOAnim->apst_Anim[0] = ANI_AllocAnim();
		ANI_SetTrackList(pst_GOAnim->apst_Anim[0], pst_TrackList);
		EVE_ResolveListTrackRef(pst_TrackList);
		pst_GOAnim->apst_Anim[0]->uc_AnimFrequency = ANI_C_DefaultAnimFrequency;

		/* Check if the Reference Gao has enough Gizmos. */
		if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_AdditionalMatrix))
			OBJ_GameObject_AllocateGizmo(pst_GO, pst_TrackList->uw_NumTracks >> 1, FALSE);
		else
		{
			if(pst_GO->pst_Base->pst_AddMatrix->l_Number < (pst_TrackList->uw_NumTracks >> 1))
				OBJ_GameObject_AllocateGizmo(pst_GO, pst_TrackList->uw_NumTracks >> 1, FALSE);
		}

		/*
		 * If the Gao has a Skeleton, we have to update the Bones Gizmo pointers due to
		 * the possible realloc of the dst_Gizmo structure of the reference Gao.
		 */
		if
		(
			pst_GO->pst_Base
		&&	pst_GO->pst_Base->pst_GameObjectAnim
		&&	pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
		) ANI_InitSkeleton(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton, pst_GO);

		//if(L_setjmp(AI_gst_ContextOut) == 1)
		//	return;

		if(L_setjmp(AI_gst_ContextCheck))
		{
			ERR_X_ForceError("This is an event error...", NULL);
			//L_longjmp(AI_gst_ContextOut, 1);
			return;
		}

		ANI_PlayGameObjectAnimation(pst_GO, pst_GOAnim, 0.0f, NULL);

		LINK_Refresh();
	}

	/* Update GO */
	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();

	M_MF()->DataHasChanged();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::CreateEvents(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	BIG_INDEX			ul_Index;
	char				asz_Name[BIG_C_MaxLenPath];
	char				asz_Path[BIG_C_MaxLenPath];
	char				*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO);
	if(ul_Index == BIG_C_InvalidIndex) return;
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	psz_Temp = L_strrchr(asz_Path, '/');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, EDI_Csz_Path_ListTracks);

	OBJ_ChangeIdentityFlags(pst_GO, pst_GO->ul_IdentityFlags | OBJ_C_IdentityFlag_Events, pst_GO->ul_IdentityFlags);
	L_strcpy(asz_Name, BIG_NameFile(ul_Index));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtEventAllsTracks);

	BIG_ul_CreateDir(asz_Path);
	ul_Index = BIG_ul_CreateFile(asz_Path, asz_Name);
	LOA_AddAddress(ul_Index, pst_GO->pst_Extended->pst_Events->pst_ListTracks);

	/* Update GO */
	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DeleteEvents(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	LINK_gx_PointersJustDeleted.SetAt(pst_GO->pst_Extended->pst_Events, NULL);
	OBJ_ChangeIdentityFlags(pst_GO, pst_GO->ul_IdentityFlags &~OBJ_C_IdentityFlag_Events, pst_GO->ul_IdentityFlags);

	/* Update GO */
	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
    Note:   We cannot enter this function if the GO has no Animation Flag or no Default Skeleton.
 =======================================================================================================================
 */
void F3D_cl_View::CreateAnimation(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog		o_File("Animation file", 0, 0, 1, EDI_Csz_Path_Animation, "*" EDI_Csz_ExtEventAllsTracks);
	CString					o_Temp;
	OBJ_tdst_GameObject		*pst_GO;
	BIG_INDEX				ul_Fat;
	EVE_tdst_ListTracks		*pst_ListTracks;
	EVE_tdst_Track			*pst_Track;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	EVE_tdst_Data			*pst_EVEData;
	int						i, iBone, iFrameNumber;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	ERR_X_Assert(WOR_World_GetWorldOfObject(pst_GO));

	pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;

	/* Create list track file */
	if(o_File.DoModal() != IDOK) return;

	o_File.GetItem(o_File.mo_File, 0, o_Temp);
	o_Temp += ".trl";
	if(BIG_b_CheckName((char *) (LPCSTR) o_Temp) == FALSE) return;

	ul_Fat = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
	if(ul_Fat != BIG_C_InvalidIndex)
	{
		if
		(
			M_MF()->MessageBox
				(
					"This Animation already exists. Do you want to overwrite it ?",
					EDI_STR_Csz_TitleConfirm,
					MB_ICONQUESTION | MB_YESNO
				) == IDNO
		) return;
	}
	else
	{
		BIG_ul_CreateDir(o_File.masz_FullPath);
		ul_Fat = BIG_ul_CreateFile(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
	}

	/* Destroy current anim */
	if
	(
		pst_GOAnim->apst_Anim[0]
	&&	pst_GOAnim->apst_Anim[0]->pst_Data
	&&	pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks
	) 
	{
		EVE_DeleteListTracks(pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks);
		LINK_UpdatePointers();
		pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks = NULL;
	}

	/* Create list of tracks */
	pst_ListTracks = (EVE_tdst_ListTracks *) MEM_p_Alloc(sizeof(EVE_tdst_ListTracks));
	L_memset(pst_ListTracks, 0, sizeof(EVE_tdst_ListTracks));
	pst_ListTracks->ul_NbOfInstances++;

	LOA_AddAddress(ul_Fat, pst_ListTracks);

	/* Create temp data */
	pst_EVEData = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
	L_memset(pst_EVEData, 0, sizeof(EVE_tdst_Data));
	pst_EVEData->pst_ListTracks = pst_ListTracks;

	/* magic box */
	pst_Track = EVE_pst_NewTrack(pst_EVEData);
	pst_Track->uw_Gizmo = (unsigned short) - 1;
	pst_Track = EVE_pst_NewTrack(pst_EVEData);
	pst_Track->uw_Gizmo = (unsigned short) - 1;

	/* Create all Tracks */
	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	for(iBone = 0; pst_CurrentBone <= pst_EndBone; iBone++, pst_CurrentBone++)
	{
		pst_Track = EVE_pst_NewTrack(pst_EVEData);
		pst_Track->uw_Gizmo = (unsigned short) iBone;
		pst_Track = EVE_pst_NewTrack(pst_EVEData);
		pst_Track->uw_Gizmo = (unsigned short) iBone;
	}

	/* create first and last (dummy) rotation/transl events for each track */
	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	for(iFrameNumber = 0; iFrameNumber < 2; iFrameNumber++)
	{
		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
		for(i = 0; i < pst_ListTracks->uw_NumTracks / 2; i++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EVE_tdst_Track			*pst_TranslTrack, *pst_RotTrack;
			EVE_tdst_Event			*pst_RotEvent, *pst_TransEvent;
			OBJ_tdst_GameObject		*pst_BoneGO;
			MATH_tdst_Matrix		*pst_LocalMatrix;
			MATH_tdst_Quaternion	st_Quaternion;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_RotTrack = &pst_ListTracks->pst_AllTracks[i * 2];
			pst_TranslTrack = &pst_ListTracks->pst_AllTracks[i * 2 + 1];
			if(i) pst_TranslTrack->uw_Flags |= EVE_C_Track_Hidden;

			pst_RotEvent = EVE_pst_NewEvent(pst_RotTrack, -1, EVE_C_EventFlag_InterpolationKey);
			pst_RotEvent->uw_NumFrames = 1;

			if(i > 0)
			{
				pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
				if(!pst_BoneGO->pst_Base->pst_Hierarchy) continue;
				pst_LocalMatrix = &pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix;
			}
			else
				pst_LocalMatrix = &MATH_gst_IdentityMatrix;
			MATH_ConvertMatrixToQuaternion(&st_Quaternion, pst_LocalMatrix);

			EVE_Event_InterpolationKey_AllocData(pst_RotEvent, EVE_InterKeyType_Rotation_Quaternion, 0);
			EVE_Event_InterpolationKey_GetQuaternion(pst_RotEvent, &st_Quaternion);

			pst_TransEvent = EVE_pst_NewEvent(pst_TranslTrack, -1, EVE_C_EventFlag_InterpolationKey);
			pst_TransEvent->uw_NumFrames = 1;
			EVE_Event_InterpolationKey_AllocData(pst_TransEvent, EVE_InterKeyType_Translation_0, 0);
			MATH_CopyVector(EVE_pst_Event_InterpolationKey_GetPos(pst_TransEvent), &pst_LocalMatrix->T);

			/* We dont want an interpolation between the last frame and the first one. */
			if(iFrameNumber == 1)
			{
				pst_RotEvent->uw_NumFrames = 0;
				pst_TransEvent->uw_NumFrames = 0;
			}

			if(i > 0) pst_CurrentBone++;
		}
	}

	/* Update GO */
	pst_GOAnim->uc_AnimUsed = 1;	/* Only the first anim is used */

	pst_GOAnim->apst_Anim[0] = ANI_AllocAnim();
	ANI_SetTrackList(pst_GOAnim->apst_Anim[0], pst_ListTracks);
	pst_GOAnim->apst_Anim[0]->uc_AnimFrequency = ANI_C_DefaultAnimFrequency;

	/* Save track list */
	EVE_SaveListTracks(pst_GO, pst_ListTracks);

	/* Play first Frame to see something. */
	ANI_PlayGameObjectAnimation(pst_GO, pst_GOAnim, 0.0f, NULL);

	/* Free temp Events Data. */
	MEM_Free(pst_EVEData);

	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();
	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::ActionAnimation
(
	ULONG				_ul_Action,
	ULONG				_ul_Param,
	OBJ_tdst_GameObject *_pst_MainGO,
	OBJ_tdst_GameObject *_pst_Gizmo
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, icur;
	float				f_Frame;
	EDIA_cl_NameDialog	o_Dialog("Enter frame number");
	char				*pz;
	int					inum;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mst_WinHandles.pst_World) return;
	if(!_pst_MainGO)
	{
		_pst_Gizmo = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem
			(
				mst_WinHandles.pst_World->pst_Selection,
				SEL_C_SIF_Object
			);
		if(!_pst_Gizmo) return;
		if(!ANI_b_IsGizmoAnim(_pst_Gizmo, &_pst_MainGO)) return;
	}

	switch(_ul_Action)
	{
	case EOUT_ACTION_ANIMPIVOTTRAJROT:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_ADDDELCURVEROT, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMPIVOTTRAJTRA:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_ADDDELCURVETRA, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMTHISTRAJROT:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_SHOWTHISCURVEROT, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMADDKEYROT:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_SPLITKEYROT, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMADDKEYTRA:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_SPLITKEYTRA, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMADDKEY:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_SPLITKEY, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMDELKEY:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_DELKEY, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMDELKEYROT:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_DELKEYROT, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMDELKEYTRA:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_DELKEYTRA, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMMOVEKEYLEFT:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_MOVEKEYLEFT, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMMOVEKEYLEFTROT:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_MOVEKEYLEFTROT, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMMOVEKEYLEFTTRA:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_MOVEKEYLEFTTRA, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMMOVEKEYRIGHT:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_MOVEKEYRIGHT, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMMOVEKEYRIGHTROT:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_MOVEKEYRIGHTROT, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMMOVEKEYRIGHTTRA:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_MOVEKEYRIGHTTRA, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMIK:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_IK, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMSWITCHTRANS:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_SWITCHTRANS, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EOUT_ACTION_ANIMTOOLBOX:
		Selection_Anim();
		break;
	case EOUT_ACTION_ANIMGOTOFRAME:
		icur = ANI_i_CurFrame(_pst_MainGO) + 1;
		o_Dialog.mo_Name.Format("%d", icur);
		if(o_Dialog.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dialog.mo_Name, "%d", &i);
			i -= icur;
			f_Frame = i * (1.0f / 60.0f);
			M_MF()->SendMessageToEditors(EEVE_MESSAGE_SETFRAME, (ULONG) _pst_MainGO, *(LONG *) &f_Frame);
		}
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_MESSAGE_TBGENERAL:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_TBGENERAL, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EEVE_MESSAGE_TBPLAY:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_TBPLAY, (ULONG) _pst_MainGO, (ULONG) _pst_Gizmo);
		break;
	case EEVE_MESSAGE_ADDSETFRAME:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_ADDSETFRAME, (ULONG) _pst_MainGO, _ul_Param);
		break;
	case EEVE_MESSAGE_ADDSETFRAME2:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_ADDSETFRAME2, (ULONG) _pst_MainGO, _ul_Param);
		break;
	case EEVE_MESSAGE_SETFRAME:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_SETFRAME, (ULONG) _pst_MainGO, _ul_Param);
		break;
	case EOUT_ACTION_DISPLAYCURVEBONES:
		if(_pst_MainGO != _pst_Gizmo)
		{
			pz = _pst_Gizmo->sz_Name + L_strlen(_pst_Gizmo->sz_Name) - 1;
			if(*pz == ']')
			{
				while(*pz != '[') pz--;
				pz--;
				while(L_isspace(*pz)) pz--;
			}

			while(L_isdigit(*pz)) pz--;
			pz++;
			inum = L_atoi(pz);
			SOFT_gac_BoneCurveStatus[inum] = SOFT_gac_BoneCurveStatus[inum] ? 0 : 1;
		}
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EOUT_ACTION_ANIMCOPYKEYS:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_COPY, (ULONG) _pst_MainGO, 0);
		break;

	case EOUT_ACTION_ANIMPASTEKEYS:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_PASTE, (ULONG) _pst_MainGO, 0);
		break;

	case EOUT_ACTION_ANIMPASTEKEYSABSOLUTE:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_PASTEABSOLUTE, (ULONG) _pst_MainGO, 0);
		break;

	case EOUT_ACTION_ANIMPASTEKEYSSYMETRYREF:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_PASTESYMETRYREF, (ULONG) _pst_MainGO, 0);
		break;

	case EOUT_ACTION_ANIMPASTEKEYSSYMETRY:
		M_MF()->SendMessageToEditors(EEVE_MESSAGE_PASTESYMETRYINSERT, (ULONG) _pst_MainGO, 0);
		break;
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::PopupAnimation(OBJ_tdst_GameObject *_pst_MainGO, OBJ_tdst_GameObject *_pst_Gizmo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	CPoint			point;
	int				ires;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&point);
	M_MF()->InitPopupMenuAction(mpo_AssociatedEditor, &o_Menu);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, 0, TRUE, "Bone");
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMADDKEY);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMDELKEY);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMMOVEKEYLEFT);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMMOVEKEYRIGHT);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMPIVOTTRAJROT);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMPIVOTTRAJTRA);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMTHISTRAJROT);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_DISPLAYCURVEBONES);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMCOPYKEYS);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMPASTEKEYS);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMPASTEKEYSABSOLUTE);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMPASTEKEYSSYMETRY);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMPASTEKEYSSYMETRYREF);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMIK);
	M_MF()->AddPopupMenuAction(mpo_AssociatedEditor, &o_Menu, EOUT_ACTION_ANIMSWITCHTRANS);
	ires = M_MF()->TrackPopupMenuAction(mpo_AssociatedEditor, point, &o_Menu);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_Anim(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GAO, *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mb_AnimOn)
	{
		mb_AnimOn = FALSE;
		mpo_AnimDialog->ShowWindow(SW_HIDE);
		return;
	}

	pst_GAO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	if(!pst_GAO) return;
	if(!ANI_b_IsGizmoAnim(pst_GAO, &pst_Father)) return;
	if(!mpo_AnimDialog)
	{
		mpo_AnimDialog = new EDIA_cl_AnimDialog(this, pst_Father);
		mpo_AnimDialog->Create(MAKEINTRESOURCE(DIALOGS_IDD_ANIM), this);
	}

	ENG_gb_GlobalLock = TRUE;
	mb_AnimOn = TRUE;
	mpo_AnimDialog->ShowWindow(SW_SHOW);
	GetParent()->SendMessage(WM_SIZE, 0, 0);
	ENG_gb_GlobalLock = FALSE;
	GetParent()->SetFocus();
}

#endif

/*$T F3Dview_action.cpp GC! 1.081 05/11/00 12:36:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "F3Dview.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "ENGine/Sources/ACTions/ACTload.h"
#include "ENGine/Sources/ACTions/ACtinit.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "ENGine/Sources/ACTions/ACTinit.h"
#include "ENGine/Sources/ACTions/ACTcompute.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKstruct.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropActionKit(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Device					*pst_Dev;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	OBJ_tdst_GameObject				*pst_GO;
	ANI_st_GameObjectAnim			*pst_GOAnim;
	EVE_tdst_ListTracks				*pst_TrackList;
	ACT_st_ActionKit				*pst_ActionKit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Must be loaded */
	pst_ActionKit = (ACT_st_ActionKit *) LOA_ul_SearchAddress(BIG_PosFile(_pst_DragDrop->ul_FatFile));
	if((int) pst_ActionKit == BIG_C_InvalidIndex) 
	{
		LOA_MakeFileRef
		(
			BIG_FileKey(_pst_DragDrop->ul_FatFile),
			(ULONG *) &pst_ActionKit,
			ACT_ul_ActionKitCallback,
			LOA_C_MustExists
		);
		LOA_Resolve();
	}

	/* Find the game object where to drop the action kit */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);

	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();
	if(!pst_GO) return;

	/* Create a game object anim and set it to the game object */
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		/* This create the GameObjectAnim structure */
		OBJ_ChangeIdentityFlags
		(
			pst_GO,
			pst_GO->ul_IdentityFlags &~OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_BaseObject | OBJ_C_IdentityFlag_Anims,
			pst_GO->ul_IdentityFlags
		);
	}

	pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;
	if(pst_ActionKit == pst_GOAnim->pst_ActionKit) return;

	if(pst_GOAnim->pst_ActionKit) 
		ACT_FreeActionKit(&pst_GOAnim->pst_ActionKit);
	else
	{
		if
			(
			pst_GOAnim->apst_Anim[0]
			&&	pst_GOAnim->apst_Anim[0]->pst_Data
			&&	pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks
			)
		{
			EVE_DeleteListTracks(pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks);
			M_MF()->DataHasChanged();
		}
		
	}
	ACT_UseActionKit(pst_ActionKit);

	pst_GOAnim->pst_ActionKit = pst_ActionKit;
	pst_GOAnim->pst_Transition = NULL;
	pst_GOAnim->pst_CurrentAction = NULL;
	pst_GOAnim->uw_CurrentActionIndex = ACT_C_NoAction;

	/* Set action 0 by default */
	if(pst_GOAnim->pst_Skeleton) ACT_SetAction(pst_GO, 0, 0, FALSE);
	pst_GOAnim->uc_AnimUsed = 1;

	/* Play the frame # 0 */
	if(pst_GOAnim->pst_CurrentAction && pst_GOAnim->apst_Anim[0])
	{
		pst_TrackList = pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks;

		if
		(
			(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_AdditionalMatrix))
		||	(pst_GO->pst_Base->pst_AddMatrix->l_Number < (pst_TrackList->uw_NumTracks >> 1))
		) OBJ_GameObject_AllocateGizmo(pst_GO, pst_TrackList->uw_NumTracks >> 1, FALSE);

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

		ANI_PlayGameObjectAnimation(pst_GO, pst_GOAnim, 0.0f, NULL);
	}

	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();
	LINK_Refresh();
}


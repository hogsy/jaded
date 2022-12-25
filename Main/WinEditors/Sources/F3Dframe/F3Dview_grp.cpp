/*$T F3Dview_grp.cpp GC! 1.081 07/03/00 09:53:56 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "F3Dview.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIstrings.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "BASe/ERRors/ERRasser.h"
#include "GDInterface/GDInterface.h"
#include "ENGine/Sources/GRP/GRPsave.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "ENGine/Sources/GRP/GRPorient.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIrender.h"
#include "SELection/SELection.h"
#include "DIAlogs/DIAname_dlg.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/GRP/GRPload.h"

#ifdef JADEFUSION
extern void UpdateModifiersGroup(OBJ_tdst_GameObject *, OBJ_tdst_Group *);
#else
extern "C" void UpdateModifiersGroup(OBJ_tdst_GameObject *, OBJ_tdst_Group *);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropObjGroup(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject				*pst_GO;
	WOR_tdst_World					*pst_World;
	GDI_tdst_Device					*pst_Dev;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	BIG_KEY							ul_ObjKey, ul_GrpKey;
	OBJ_tdst_Group					*pst_Group;
	MATH_tdst_Vector				v;
	POINT							point;
	CAM_tdst_Camera					*pst_Cam;
	BOOL							b_Associate;
	char							asz_Msg[512];
	EDIA_cl_NameDialogCombo			o_Dialog("Group Selection");
	BOOL							b_Skeleton;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Instanciation, or association ? */
	b_Associate = FALSE;
	b_Skeleton = FALSE;

	o_Dialog.AddItem("Group Association");
	o_Dialog.AddItem("Group Copy");
	o_Dialog.AddItem("Skeleton");
	o_Dialog.SetDefault("Skeleton");

	if(o_Dialog.DoModal() == IDOK)
	{
		b_Skeleton = (!L_strcmpi((char *) (LPCSTR) o_Dialog.mo_Name, "Skeleton"));
		if(!b_Skeleton) b_Associate = (!L_strcmpi((char *) (LPCSTR) o_Dialog.mo_Name, "Group Association"));
	}
	else
		return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Find object to drop group on
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);
	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();
	if(!pst_GO) return;

	ul_ObjKey = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
	if(ul_ObjKey == BIG_C_InvalidIndex) return;
	pst_World = mst_WinHandles.pst_World;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Handle the group (get key and pointer, duplicate if necessary )
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ul_GrpKey = BIG_FileKey(_pst_DragDrop->ul_FatFile);
	ERR_X_Assert(ul_GrpKey != BIG_C_InvalidIndex);

	pst_Group = NULL;
	if(b_Associate)
	{
		/* Gameobject has already a group */
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Group))
		{
			if
			(
				M_MF()->MessageBox
					(
						"This object has already a group. Are you sure you want to overwrite it ?",
						EDI_STR_Csz_TitleConfirm,
						MB_ICONQUESTION | MB_YESNO
					) == IDNO
			) return;
		}

		pst_Group = (OBJ_tdst_Group *) LOA_ul_SearchAddress(BIG_PosFile(_pst_DragDrop->ul_FatFile));
		if((int) pst_Group == -1)
		{
			WOR_gpst_WorldToLoadIn = pst_World;
			LOA_MakeFileRef(ul_GrpKey, (ULONG *) &pst_Group, GRP_ul_LoadGrpCallback, LOA_C_MustExists);
			LOA_Resolve();
			if(!pst_Group) return;
		}

		/* Attach the group to the object it was dragged on, and update objects */
		GRP_ObjAttachReplaceGroup(pst_GO, pst_Group);
		OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_AABBox);
		LINK_UpdatePointer(pst_GO);
		LINK_UpdatePointers();
		sprintf(asz_Msg, "Associate group to %s", pst_GO->sz_Name);
		LINK_PrintStatusMsg(asz_Msg);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    The group must be duplicated and loaded into the object's world
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else
	{
		/*$2- Duplicate the Group ------------------------------------------------------------------------------------*/

		if(!b_Skeleton)
		{
			LINK_gb_AllRefreshEnable = FALSE;
			pst_Group = GRP_pst_DuplicateGroupFile(pst_World, _pst_DragDrop->ul_FatFile);
			LINK_gb_AllRefreshEnable = TRUE;

			/* Set relative position of each object of the group */
			GetCursorPos(&point);
			ScreenToClient(&point);
			pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
			v.x = (float) point.x / (float) pst_Dev->l_Width;
			v.y = 1.0f - ((float) point.y / (float) pst_Dev->l_Height);
			b_PickDepth(&v, &v.z);
			pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
			v.x = (float) point.x;
			v.y = (float) point.y;
			CAM_2Dto3D(pst_Cam, &v, &v);
			GRP_SetRelativePosition(pst_Group, &v);

			LINK_Refresh();
		}

		/*$2- Skeleton -----------------------------------------------------------------------------------------------*/

		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			BIG_KEY			ul_Key;
			ULONG			ul_NbBones;
			OBJ_tdst_Group	*pst_DestGroup;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Group = (OBJ_tdst_Group *) LOA_ul_SearchAddress(BIG_PosFile(_pst_DragDrop->ul_FatFile));

			/* If not loaded, we load. */
			if((int) pst_Group == -1)
			{
				ul_Key = BIG_FileKey(_pst_DragDrop->ul_FatFile);
				LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Group, GRP_ul_LoadGrpCallback, LOA_C_MustExists);
				LOA_Resolve();
			}

			ANI_AttachSkeletonModelToWorld(pst_Group, pst_GO->pst_World);
			ul_NbBones = TAB_ul_PFtable_GetNbElems(pst_Group->pst_AllObjects);

			pst_DestGroup = GRP_pst_CreateNewGroup();
			if(pst_GO->pst_World)
			{
				if
				(
					(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
				&&	pst_GO->pst_Base
				&&	pst_GO->pst_Base->pst_GameObjectAnim
				&&	pst_GO->pst_Base->pst_GameObjectAnim->pst_SkeletonModel
				)
				{
					/* If we have DnD the same Group, we return; */
					if(pst_GO->pst_Base->pst_GameObjectAnim->pst_SkeletonModel == pst_Group) return;

					/* We detach the previous Skeleton. */
					GRP_ObjDetachSkeleton(pst_GO);
				}

				GRP_DuplicateGroupObjects(pst_GO->pst_World, pst_Group, pst_DestGroup);
				UpdateModifiersGroup(pst_GO, pst_DestGroup);

				/* Object doesn't have a anim structure yet so we create one */
				if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
				{
					OBJ_ChangeIdentityFlags
					(
						pst_GO,
						OBJ_ul_FlagsIdentityGet(pst_GO) &~OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_Anims,
						OBJ_C_UnvalidFlags
					);
				}

				GRP_ObjAttachSkeleton(pst_GO, pst_Group, pst_DestGroup);

				/* Allocation of the Gizmos */
				if
				(
					(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_AdditionalMatrix))
				||	(pst_GO->pst_Base->pst_AddMatrix->l_Number < (LONG) ul_NbBones)
				) OBJ_GameObject_AllocateGizmo(pst_GO, ul_NbBones, FALSE);

				/* Sets a default Flash Matrix */
				OBJ_SetFlashMatrix(pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO));

				/* We create the Gizmos from the Hierarchy of the GameObjects */
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
					OBJ_tdst_GameObject *pst_BoneObject;
					ULONG				i;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					/* We set the First Gizmo to the IdentityMatrix. */
					MATH_SetIdentityMatrix(&(pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo)->st_Matrix);

					pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_DestGroup->pst_AllObjects);
					pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_DestGroup->pst_AllObjects);

					/* We skip the Main bone that has already been processed. */
					pst_CurrentElem++;
					for(i = 1; pst_CurrentElem <= pst_EndElem; i++, pst_CurrentElem++)
					{
						pst_BoneObject = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
						if(!OBJ_b_TestIdentityFlag(pst_BoneObject, OBJ_C_IdentityFlag_Hierarchy))
						{
							if(i == 0)
								MATH_SetIdentityMatrix(&(pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo)->st_Matrix);
							else
							{
								MATH_CopyMatrix
								(
									&(pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo + i)->st_Matrix,
									OBJ_pst_GetAbsoluteMatrix(pst_BoneObject)
								);
							}
						}
						else
						{
							MATH_CopyMatrix
							(
								&(pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo + i)->st_Matrix,
								OBJ_pst_GetLocalMatrix(pst_BoneObject)
							);
						}

						ANI_ComputeBoneMatrix
						(
							pst_BoneObject,
							pst_GO,
							&(pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo + i)->st_Matrix,
							TRUE
						);
					}
				}

				/*
				 * We have created the Gizmos for the Gao. We update the Skeleton Bones Pointer to
				 * Gizmo
				 */
				ANI_InitSkeleton(pst_DestGroup, pst_GO);
			}

			LINK_UpdatePointer(pst_GO);
			sprintf(asz_Msg, "Associate skeleton to %s", pst_GO->sz_Name);
			LINK_PrintStatusMsg(asz_Msg);
		}
	}

	/* Load new textures if needed */
	if(WOR_gb_HasAddedMaterial)
	{
        GDI_l_AttachWorld( mst_WinHandles.pst_DisplayData, mst_WinHandles.pst_World );
		WOR_gb_HasAddedMaterial = FALSE;
	}

	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::SelectGroup(BIG_INDEX _ul_Index, BOOL _b_Single)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFtable	*pst_AllObjects;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	BOOL				b_MustDel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_MustDel = TRUE;
	pst_AllObjects = (TAB_tdst_PFtable *) LOA_ul_SearchAddress(BIG_PosFile(_ul_Index));
	if(pst_AllObjects && ((int) pst_AllObjects != -1))
		b_MustDel = FALSE;
	else
	{
		LOA_MakeFileRef
		(
			BIG_FileKey(_ul_Index),
			(ULONG *) &pst_AllObjects,
			OBJ_ul_World_LoadGolCallback,
			LOA_C_MustExists
		);
		LOA_Resolve();
	}

	/* Select all objects */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AllObjects);
	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(!TAB_b_IsAHole(pst_GO))
		{
			ForceSelectObject(pst_GO, _b_Single && pst_CurrentElem == TAB_pst_PFtable_GetFirstElem(pst_AllObjects));
		}
	}

	/* Delete group */
	if(b_MustDel) OBJ_FreeGroupGrl(pst_AllObjects);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_GroupObject(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group	*pst_Group;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Group = (OBJ_tdst_Group *) _ul_Param1;
	OBJ_AddInGroup(pst_Group, _pst_Sel->p_Content);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_GroupByName(char *_psz_Name, BOOL _b_Ask)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group	*pst_Group;
	BOOL			del;
	char			asz_Path[BIG_C_MaxLenPath];
	char			asz_Name[BIG_C_MaxLenName];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(BIG_b_CheckName(_psz_Name) == FALSE) return;
	WOR_GetSubPath(mst_WinHandles.pst_World, EDI_Csz_Path_Groups, asz_Path);

	L_strcpy(asz_Name, _psz_Name);
	if(L_strrchr(asz_Name, '.')) *L_strrchr(asz_Name, '.') = 0; 
	L_strcat(asz_Name, EDI_Csz_ExtObjGroups);
	if(BIG_ul_SearchFileExt(asz_Path, asz_Name) != BIG_C_InvalidIndex)
	{
		if(_b_Ask)
		{
			if
			(
				M_MF()->MessageBox
					(
						"That group already exists !\nDo you want to owerwrite it ?",
						"Please Confirm",
						MB_ICONQUESTION | MB_YESNO
					) == IDNO
			) return;
		}
//		BIG_DelFile(asz_Path, asz_Name);
	}

	L_strcpy(asz_Name, _psz_Name);
	if(L_strrchr(asz_Name, '.')) *L_strrchr(asz_Name, '.') = 0; 
	pst_Group = GRP_pst_CreateAndSaveNewGroup(asz_Path, asz_Name, TRUE, _b_Ask ? FALSE : TRUE, &del);
	if(!pst_Group) return;

	SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_GroupObject, (ULONG) pst_Group, 0);
	GRP_SaveGroup(pst_Group);
	if(del) OBJ_FreeGroup(pst_Group);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_Group(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog("Enter group name");
	char				*psz_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return;
	if(o_Dialog.DoModal() == IDOK)
	{
		psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
		Selection_GroupByName(psz_Name);
	}
}

#endif /* ACTIVE_EDITORS */

/*$T OBJgizmo.c GC! 1.081 06/18/01 11:32:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "BASe/MEMory/MEM.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKtoed.h"
#include "GDInterface/GDInterface.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEO_MRM.h"
#include "LIGHT/LIGHTstruct.h"
#include "OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "TABles/TABles.h"
#include "OBJgrp.h"
#include "COLlision/COLstruct.h"
#include "COLlision/COLaccess.h"
#include "BASe/BAStypes.h"
#include "GEOmetric/GEODebugObject.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern BOOL ENG_gb_EngineRunning;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_Gizmo_UpdateGizmoPtr(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_AdditionalMatrix	*pst_AddMat;
	LONG						i;
	OBJ_tdst_GameObject			*pst_Other;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) return;
	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)) return;
	pst_AddMat = (OBJ_tdst_AdditionalMatrix *) _pst_GO->pst_Base->pst_AddMatrix;
	for(i = 0; i < pst_AddMat->l_Number; i++)
	{
		pst_AddMat->dst_GizmoPtr[i].pst_Matrix = NULL;
		pst_Other = pst_AddMat->dst_GizmoPtr[i].pst_GO;
		if(pst_Other)
		{
			if(pst_AddMat->dst_GizmoPtr[i].l_MatrixId == -1)
			{
				pst_AddMat->dst_GizmoPtr[i].pst_Matrix = pst_Other->pst_GlobalMatrix;
			}
			else if
				(
					((pst_Other->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix))
				&&	(!((pst_Other->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)))
				&&	(pst_AddMat->dst_GizmoPtr[i].l_MatrixId < pst_Other->pst_Base->pst_AddMatrix->l_Number)
				)
			{
				pst_AddMat->dst_GizmoPtr[i].pst_Matrix = &(pst_Other->pst_Base->pst_AddMatrix->dst_Gizmo[pst_AddMat->dst_GizmoPtr[i].l_MatrixId].st_Matrix);
			}
		}
	}
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_Gizmo_AddEditObject(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group				*pst_Skeleton;
	TAB_tdst_PFelem				*pst_CurrentBone;
	OBJ_tdst_GameObject			*pst_BoneGO;
	LONG						l_NbOfBones, l_EndLoop;
	OBJ_tdst_AdditionalMatrix	*pst_AM;
	OBJ_tdst_GameObject			*pst_GizmoObj;
	int							i;
	char						sz_Path[BIG_C_MaxLenPath];
	char						asz_BoneName[BIG_C_MaxLenName];
	char						*psz_Temp;
	BOOL						b_ValidBone;
	ULONG						ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) return;
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer) return;

	pst_AM = _pst_GO->pst_Base->pst_AddMatrix;
	if((!pst_AM) || (!pst_AM->l_Number)) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Create object
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_AM->dpst_EditionGO = (OBJ_tdst_GameObject **) MEM_p_Alloc(pst_AM->l_Number * 4);
	b_ValidBone =
		(
			_pst_GO->pst_Base && _pst_GO->pst_Base->pst_GameObjectAnim && _pst_GO->pst_Base->pst_GameObjectAnim->pst_SkeletonModel
		) ? TRUE : FALSE;
	if(b_ValidBone)
	{
		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(_pst_GO->pst_Base->pst_GameObjectAnim->pst_SkeletonModel->pst_AllObjects);
		l_NbOfBones = TAB_ul_PFtable_GetNbElems(_pst_GO->pst_Base->pst_GameObjectAnim->pst_SkeletonModel->pst_AllObjects);
	}

	for(i = 0; i < pst_AM->l_Number; i++)
	{
		if(b_ValidBone)
		{
			if(i == l_NbOfBones)
				b_ValidBone = FALSE;
			else
				pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

			if(!pst_BoneGO || !(pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer))
				b_ValidBone = FALSE;
			else
			{
				pst_CurrentBone++;
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_BoneGO);
				if(ul_Index != BIG_C_InvalidIndex)
					L_strcpy(asz_BoneName, BIG_NameFile(ul_Index));
				else if(pst_BoneGO->sz_Name)
					L_strcpy(asz_BoneName, pst_BoneGO->sz_Name);
				else
					L_strcpy(asz_BoneName, "NoName");

				psz_Temp = L_strrchr(asz_BoneName, '.');
				if(psz_Temp) *psz_Temp = 0;
			}
		}

		pst_GizmoObj = OBJ_GameObject_Create(OBJ_C_IdentityFlag_BaseObject | OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_Hierarchy);
		pst_AM->dpst_EditionGO[i] = pst_GizmoObj;
		WOR_World_AttachGameObject(_pst_GO->pst_World, pst_GizmoObj);
		WOR_GetGaoPath(_pst_GO->pst_World, sz_Path);

		if(pst_GizmoObj)
		{
			if(_pst_GO->sz_Name)
			{
				if(b_ValidBone)
				{
					pst_GizmoObj->sz_Name = (char *) MEM_p_Alloc(50 + strlen(_pst_GO->sz_Name));
					sprintf(pst_GizmoObj->sz_Name, "%s - Gizmo %d [%s]", _pst_GO->sz_Name, i, asz_BoneName);
				}
				else
				{
					pst_GizmoObj->sz_Name = (char *) MEM_p_Alloc(16 + strlen(_pst_GO->sz_Name));
					sprintf(pst_GizmoObj->sz_Name, "%s - Gizmo %d", _pst_GO->sz_Name, i);
				}
			}
			else
			{
				if(b_ValidBone)
				{
					pst_GizmoObj->sz_Name = (char *) MEM_p_Alloc(50);
					sprintf(pst_GizmoObj->sz_Name, "??? - Gizmo %d [%s]", i, asz_BoneName);
				}
				else
				{
					pst_GizmoObj->sz_Name = (char *) MEM_p_Alloc(16);
					sprintf(pst_GizmoObj->sz_Name, "??? - Gizmo %d", i);
				}
			}

			pst_GizmoObj->ul_EditorFlags |= OBJ_C_EditFlags_NoSubObjectMode | OBJ_C_EditFlags_GizmoObject;
			pst_GizmoObj->pst_Base->pst_Hierarchy->pst_Father = _pst_GO;
			MATH_CopyMatrix(&pst_GizmoObj->pst_Base->pst_Hierarchy->st_LocalMatrix, &pst_AM->dst_Gizmo[i].st_Matrix);
			pst_GizmoObj->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) GEO_pst_DebugObject_Get(GEO_DebugObject_Gizmo);
			pst_GizmoObj->ul_IdentityFlags |= OBJ_C_IdentityFlag_Generated;
			LINK_RegisterPointer(pst_GizmoObj, LINK_C_ENG_GameObjectOriented, pst_GizmoObj->sz_Name, sz_Path);
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Process if bones
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if
	(
		OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)
	&&	_pst_GO->pst_Base->pst_GameObjectAnim
	&&	_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
	)
	{
		pst_Skeleton = _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
		l_NbOfBones = TAB_ul_PFtable_GetNbElems(pst_Skeleton->pst_AllObjects);
		l_EndLoop = lMin(l_NbOfBones, pst_AM->l_Number);

		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
		for(i = 0; i < l_EndLoop; pst_CurrentBone++, i++)
		{
			pst_GizmoObj = pst_AM->dpst_EditionGO[i];
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

			/* Free position and hierarchy of gizmo object */
			MEM_Free(pst_GizmoObj->pst_GlobalMatrix);
			if(OBJ_b_TestIdentityFlag(pst_GizmoObj, OBJ_C_IdentityFlag_Hierarchy))
				MEM_Free(pst_GizmoObj->pst_Base->pst_Hierarchy);

			/* Change matrix so that the gizmo object references the bone */
			pst_GizmoObj->pst_GlobalMatrix = pst_BoneGO->pst_GlobalMatrix;

			/* Hierarchy */
			if(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Hierarchy))
				pst_GizmoObj->pst_Base->pst_Hierarchy = pst_BoneGO->pst_Base->pst_Hierarchy;

			pst_GizmoObj->ul_IdentityFlags |= OBJ_C_IdentityFlag_SharedMatrix;
			if(pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_HasInitialPos)
				pst_GizmoObj->ul_IdentityFlags |= OBJ_C_IdentityFlag_HasInitialPos;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_Gizmo_DelEditObject(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_AdditionalMatrix	*pst_AM;
	OBJ_tdst_GameObject			*pst_GizmoObj;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) return;
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer) return;

	pst_AM = _pst_GO->pst_Base->pst_AddMatrix;
	if((!pst_AM) || (!pst_AM->l_Number)) return;

	for(i = 0; i < pst_AM->l_Number; i++)
	{
		if((pst_AM->dpst_EditionGO) && (pst_AM->dpst_EditionGO[i]))
		{
			pst_GizmoObj = pst_AM->dpst_EditionGO[i];
			pst_GizmoObj->pst_Base->pst_Visu->pst_Object = NULL;
			WOR_World_DetachObject(_pst_GO->pst_World, pst_GizmoObj);
			OBJ_GameObject_Remove(pst_GizmoObj, 1);
		}
	}

	if(pst_AM->dpst_EditionGO)
	{
		MEM_Free(pst_AM->dpst_EditionGO);
		pst_AM->dpst_EditionGO = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_Gizmo_Update_LOAD(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_AdditionalMatrix	*pst_AM;
	int							i;
	OBJ_tdst_GameObject			*pst_Gizmo;
	MATH_tdst_Matrix			st_LocalMatrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) return;
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer) return;

	pst_AM = _pst_GO->pst_Base->pst_AddMatrix;
	if((!pst_AM) || (!pst_AM->l_Number)) return;

	if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowGizmo)
	{
		if(!pst_AM->dpst_EditionGO) OBJ_Gizmo_AddEditObject(_pst_GO);

		for(i = 0; i < pst_AM->l_Number; i++)
		{
			pst_Gizmo = pst_AM->dpst_EditionGO[i];
			if(pst_Gizmo)
			{
				/* Gets the Gizmo in the Character Coordinate system. */
				MATH_CopyMatrix(&st_LocalMatrix, &pst_AM->dst_Gizmo[i].st_Matrix);

				/* Compute the Real Global Matrix of the Gizmo Object. */
				MATH_MulMatrixMatrix
				(
					OBJ_pst_GetAbsoluteMatrix(pst_Gizmo),
					&st_LocalMatrix,
					OBJ_pst_GetAbsoluteMatrix(_pst_GO)
				);

				/* Compute the Valid Local Matrix of the Gizmo Object. */
				OBJ_ComputeLocalWhenHie(pst_Gizmo);
			}
		}
	}
	else
	{
		if(pst_AM->dpst_EditionGO) OBJ_Gizmo_DelEditObject(_pst_GO);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_Gizmo_Update(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_AdditionalMatrix				*pst_AM;
	int										i;
	OBJ_tdst_GameObject						*pst_Gizmo;
	MATH_tdst_Matrix st_InvertGlobalMatrix	ONLY_PSX2_ALIGNED(16);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ENG_gb_EngineRunning)
	{
		OBJ_Gizmo_Update_LOAD(_pst_GO);
		return;
	}

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) return;
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer) return;

	MATH_InvertMatrix(&st_InvertGlobalMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
	pst_AM = _pst_GO->pst_Base->pst_AddMatrix;
	if((!pst_AM) || (!pst_AM->l_Number)) return;

	if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowGizmo)
	{
		if(!pst_AM->dpst_EditionGO) OBJ_Gizmo_AddEditObject(_pst_GO);

		for(i = 0; i < pst_AM->l_Number; i++)
		{
			pst_Gizmo = pst_AM->dpst_EditionGO[i];
			if(pst_Gizmo)
			{
				/* Compute a matrix in the Character Coordinate system. */
				MATH_MulMatrixMatrix
				(
					&pst_AM->dst_Gizmo[i].st_Matrix,
					OBJ_pst_GetAbsoluteMatrix(pst_Gizmo),
					&st_InvertGlobalMatrix
				);
			}
		}
	}
	else
	{
		if(pst_AM->dpst_EditionGO) OBJ_Gizmo_DelEditObject(_pst_GO);
	}
}

void OBJ_Gizmo_ChangeNumberOfAdditionalMatrix(OBJ_tdst_AdditionalMatrix *_pstAdditionalMatrix,ULONG _ulNewNb)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG ulOldNb = _pstAdditionalMatrix->l_Number;
	ULONG ulSizeOfGizmos,i;
	OBJ_tdst_Gizmo				*pst_Gizmo;
	OBJ_tdst_GizmoPtr			*pst_GizmoPtr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (_ulNewNb == ulOldNb) 
        return;

	if(_ulNewNb == 0)
	{
		MEM_Free(_pstAdditionalMatrix->dst_Gizmo);
	}
	else
	{
		if(_pstAdditionalMatrix->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
		{
			ulSizeOfGizmos = sizeof(OBJ_tdst_GizmoPtr) * _ulNewNb;
			if (ulOldNb == 0)
				_pstAdditionalMatrix->dst_GizmoPtr = (OBJ_tdst_GizmoPtr *) MEM_p_Alloc(ulSizeOfGizmos);
			else
				_pstAdditionalMatrix->dst_GizmoPtr = (OBJ_tdst_GizmoPtr *) MEM_p_Realloc(_pstAdditionalMatrix->dst_GizmoPtr, ulSizeOfGizmos);

			for(i = ulOldNb; i < _ulNewNb; i++)
			{
				pst_GizmoPtr = _pstAdditionalMatrix->dst_GizmoPtr + i;
				pst_GizmoPtr->pst_Matrix = NULL;
				pst_GizmoPtr->pst_GO = NULL;
				pst_GizmoPtr->l_MatrixId = -1;
			}
		}
		else
		{
			ulSizeOfGizmos = sizeof(OBJ_tdst_Gizmo) * _ulNewNb;
			if(ulOldNb == 0)
				_pstAdditionalMatrix->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_Alloc(ulSizeOfGizmos);
			else
				_pstAdditionalMatrix->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_Realloc(_pstAdditionalMatrix->dst_Gizmo, ulSizeOfGizmos);

			for(i = ulOldNb; i < _ulNewNb; i++)
			{
				pst_Gizmo = _pstAdditionalMatrix->dst_Gizmo + i;
				MATH_SetIdentityMatrix(&pst_Gizmo->st_Matrix);
				sprintf(pst_Gizmo->sz_Name, "Gizmo %d", i);
			}
		}
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_Gizmo_ForceEditionPos(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_AdditionalMatrix	*pst_AM;
	int							i;
	OBJ_tdst_GameObject			*pst_Gizmo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowGizmo)) return;
	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) return;
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer) return;

	pst_AM = _pst_GO->pst_Base->pst_AddMatrix;
	if((!pst_AM) || (!pst_AM->l_Number)) return;
	if(!pst_AM->dpst_EditionGO) return;

	for(i = 0; i < pst_AM->l_Number; i++)
	{
		pst_Gizmo = pst_AM->dpst_EditionGO[i];
		if(pst_Gizmo)
		{
			MATH_CopyMatrix(&pst_Gizmo->pst_Base->pst_Hierarchy->st_LocalMatrix, &pst_AM->dst_Gizmo[i].st_Matrix);
		}
	}
}
#ifdef JADEFUSIONDD//POPOWARNING function existe deja quelle prendre ??
/*
=======================================================================================================================
Aim:    Change the number of additional matrices.
pst_AM->lNumber must already contain the new number.
l_OldData must contain the old number. That's the way it was.
=======================================================================================================================
*/
void OBJ_Gizmo_ChangeNumberOfAdditionalMatrix(OBJ_tdst_AdditionalMatrix* pst_AM, long l_OldData)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    long						l_Decal, l_NewNumber, i;
    OBJ_tdst_Gizmo				*pst_Gizmo;
    OBJ_tdst_GizmoPtr			*pst_GizmoPtr;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    l_NewNumber = pst_AM->l_Number;

    if(l_NewNumber == l_OldData) return;

    if(l_NewNumber == 0)
    {
        MEM_Free(pst_AM->dst_Gizmo);
    }
    else
    {
        if(pst_AM->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
        {
            l_Decal = sizeof(OBJ_tdst_GizmoPtr) * l_NewNumber;
            if(l_OldData == 0)
                pst_AM->dst_GizmoPtr = (OBJ_tdst_GizmoPtr *) MEM_p_Alloc(l_Decal);
            else
                pst_AM->dst_GizmoPtr = (OBJ_tdst_GizmoPtr *) MEM_p_Realloc(pst_AM->dst_GizmoPtr, l_Decal );

            for(i = l_OldData; i < l_NewNumber; i++)
            {
                pst_GizmoPtr = pst_AM->dst_GizmoPtr + i;
                pst_GizmoPtr->pst_Matrix = NULL;
                pst_GizmoPtr->pst_GO = NULL;
                pst_GizmoPtr->l_MatrixId = -1;
            }
        }
        else
        {
            l_Decal = sizeof(OBJ_tdst_Gizmo) * l_NewNumber;
            if(l_OldData == 0)
                pst_AM->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_Alloc(l_Decal);
            else
                pst_AM->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_Realloc(pst_AM->dst_Gizmo, l_Decal );

            for(i = l_OldData; i < l_NewNumber; i++)
            {
                pst_Gizmo = pst_AM->dst_Gizmo + i;
                MATH_SetIdentityMatrix(&pst_Gizmo->st_Matrix);
                sprintf(pst_Gizmo->sz_Name, "Gizmo %d", i);
            }
        }
    }

    LINK_UpdatePointer(pst_AM->pst_GO);
    LINK_UpdatePointers();
}
#endif //jadefus
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

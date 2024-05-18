/*$T ANImain.c GC! 1.081 01/16/02 15:16:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "TABles/TABles.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "MATHs/MATH.h"
#include "MATHs/MATHdebug.h"

#include "ENGine/Sources/ANImation/ANIplay.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"

#ifdef ACTIVE_EDITORS
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#endif

#ifdef _GAMECUBE
#include "ENGine/Sources/ACTions/ACTload.h"
#include "SDK/Sources/GameCube/GC_arammng.h"
#include "SDK/Sources/GameCube/GC_aramheap.h"
#endif

#include "TIMer/TIMdefs.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern EVE_tdst_ListTracks	*EVE_gpst_CurrentListTracks;
extern BOOL					EVE_gb_CanFlash;
extern void					UpdateModifiersGroup(OBJ_tdst_GameObject *, OBJ_tdst_Group *);

#define ANI_b_PlayAfterEndOfAnim(anim) \
		( \
			((anim)->uc_MiscFlags & ANI_C_MiscFlag_MagicBox_AfterEnd) \
		||	((anim)->uc_MiscFlags & ANI_C_MiscFlag_MagicBox_Interpolation) \
		)


/*
 =======================================================================================================================
    Aim:    We have just set a Father to _pst_GO (Dynamic Link or Editor). We want to update the Flash Matrix of
            _pst_GO (and all the blend reference matrix if needed) so that they will be now in the Father Coordinate
            System.
 =======================================================================================================================
 */
void ANI_ApplyHierarchyOnFlash(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject				*pst_Father;
	ANI_st_GameObjectAnim			*pst_GOAnim;
	MATH_tdst_Matrix				*pst_Ref, M;
	MATH_tdst_Matrix st_InvFather	ONLY_PSX2_ALIGNED(16);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	||	(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
	) return;

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	/* Anti-Bug */
	if(!pst_GOAnim->apst_Anim[0]) return;

	pst_Father = _pst_GO->pst_Base->pst_Hierarchy->pst_Father;

	MATH_InvertMatrix(&st_InvFather, OBJ_pst_GetAbsoluteMatrix(pst_Father));

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
	{
		MATH_MulMatrixMatrix(&M, OBJ_pst_GetFlashMatrix(_pst_GO), &st_InvFather);
		MATH_CopyMatrix(OBJ_pst_GetFlashMatrix(_pst_GO), &M);
	}

	if(ANI_b_Blend(_pst_GO))
	{
		pst_Ref = &pst_GOAnim->apst_Anim[1]->st_Ref;

		MATH_MulMatrixMatrix(&M, pst_Ref, &st_InvFather);
		MATH_CopyMatrix(pst_Ref, &M);

		/*$F
		if(ANI_b_PlayAfterEndOfAnim(pst_GOAnim->apst_Anim[1]))
		{
			MATH_TransformVector
			(
				(MATH_tdst_Vector *) &pst_GOAnim->apst_Anim[0]->st_Ref,
				&st_InvFather,
				(MATH_tdst_Vector *) &pst_GOAnim->apst_Anim[0]->st_Ref
			);
		}
		*/
	}
}

/*
 =======================================================================================================================
    Aim:    We have just removed the Hierarchy on _pst_GO. We want to update the Flash Matrix of _pst_GO (and the blend
            reference matrix if needed) so that they will be now in the Global Coordinate System.
 =======================================================================================================================
 */
void ANI_RemoveHierarchyOnFlash(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_Father;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	MATH_tdst_Matrix		M;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	||	(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
	) return;

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	/* Anti-Bug */
	if(!pst_GOAnim->apst_Anim[0]) return;

	pst_Father = _pst_GO->pst_Base->pst_Hierarchy->pst_Father;

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
	{
		MATH_CopyMatrix(&M, OBJ_pst_GetFlashMatrix(_pst_GO));
		MATH_MulMatrixMatrix(OBJ_pst_GetFlashMatrix(_pst_GO), &M, OBJ_pst_GetAbsoluteMatrix(pst_Father));
	}

	if(ANI_b_Blend(_pst_GO))
	{
		MATH_CopyMatrix(&M, &pst_GOAnim->apst_Anim[1]->st_Ref);
		MATH_MulMatrixMatrix(&pst_GOAnim->apst_Anim[1]->st_Ref, &M, OBJ_pst_GetAbsoluteMatrix(pst_Father));

		/*$F
		if(ANI_b_PlayAfterEndOfAnim(pst_GOAnim->apst_Anim[1]))
		{
			MATH_TransformVector
			(
				(MATH_tdst_Vector *) &pst_GOAnim->apst_Anim[0]->st_Ref,
				OBJ_pst_GetAbsoluteMatrix(OBJ_pst_GetFather(_pst_GO)),
				(MATH_tdst_Vector *) &pst_GOAnim->apst_Anim[0]->st_Ref
			);
		}
		*/
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *ANI_pst_GetObjectByAICanal(OBJ_tdst_GameObject *_pst_RefGO, UCHAR _uc_AI_Canal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_GameObject *pst_BoneGO;
	OBJ_tdst_Group		*pst_Skeleton;
	ANI_tdst_Shape		*pst_Shape;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!OBJ_b_TestIdentityFlag(_pst_RefGO, OBJ_C_IdentityFlag_Anims))
	||	(!_pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
	||	(!_pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects)
	) return NULL;

	pst_Skeleton = _pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
	pst_Shape = _pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Shape;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
	for(i = 0; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++, i++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO)) continue;

		if(pst_Shape)
		{
			if(pst_Shape->auc_AI_Canal[i] == _uc_AI_Canal) return pst_BoneGO;
		}
		else
		{
			if(i == _uc_AI_Canal) return pst_BoneGO;
		}
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UCHAR ANI_uc_GetEngineCanalByAICanal(OBJ_tdst_GameObject *_pst_RefGO, UCHAR _uc_AI_Canal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem *pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_Group	*pst_Skeleton;
	ANI_tdst_Shape	*pst_Shape;
	UCHAR			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!OBJ_b_TestIdentityFlag(_pst_RefGO, OBJ_C_IdentityFlag_Anims))
	||	(!_pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
	||	(!_pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects)
	||	(!_pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Shape)
	) return 0;

	pst_Skeleton = _pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
	pst_Shape = _pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Shape;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
	for(i = 0; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++, i++)
	{
		if(pst_Shape)
		{
			if(pst_Shape->auc_AI_Canal[i] == _uc_AI_Canal) return i;
		}
		else
		{
			if(i == _uc_AI_Canal) return i;
		}
	}

	return 0xFF;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ANI_i_GetVisualOfAICanal(OBJ_tdst_GameObject *_pst_RefGO, UCHAR _uc_AI_Canal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_BoneGO, *pst_VisuGO;
	TAB_tdst_PFelem		*pst_CurrentVisu, *pst_EndVisu;
	int					visual;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BoneGO = ANI_pst_GetObjectByAICanal(_pst_RefGO, _uc_AI_Canal);

	if(pst_BoneGO && pst_BoneGO->pst_Extended && pst_BoneGO->pst_Extended->pst_Group)
	{
		pst_CurrentVisu = TAB_pst_PFtable_GetFirstElem(pst_BoneGO->pst_Extended->pst_Group->pst_AllObjects);
		pst_EndVisu = TAB_pst_PFtable_GetLastElem(pst_BoneGO->pst_Extended->pst_Group->pst_AllObjects);
		visual = 0;
		while(pst_CurrentVisu <= pst_EndVisu)
		{
			pst_VisuGO = (OBJ_tdst_GameObject *) pst_CurrentVisu->p_Pointer;
			if
			(
				!pst_VisuGO
			||	!pst_VisuGO->pst_Base
			||	!pst_VisuGO->pst_Base->pst_Visu
			||	!pst_VisuGO->pst_Base->pst_Visu->pst_Object
			) continue;
			if(pst_BoneGO->pst_Base->pst_Visu->pst_Object == pst_VisuGO->pst_Base->pst_Visu->pst_Object)
				return visual;

			pst_CurrentVisu++;
			visual++;
		}
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *ANI_pst_GetClosestObjCanal(OBJ_tdst_GameObject *_pst_RefGO, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_GameObject *pst_BoneGO;
	OBJ_tdst_Group		*pst_Skeleton;
	float				f_BestDist;
	float				f_Dist;
	MATH_tdst_Vector	st_Temp;
	OBJ_tdst_GameObject *pst_BestGO;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!OBJ_b_TestIdentityFlag(_pst_RefGO, OBJ_C_IdentityFlag_Anims))
	||	(!_pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
	) return NULL;

	pst_Skeleton = _pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
	f_BestDist = 1000000.0f;
	for(i = 0; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++, i++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO)) continue;
		MATH_SubVector(&st_Temp, OBJ_pst_GetAbsolutePosition(pst_BoneGO), _pst_Pos);
		f_Dist = MATH_f_NormVector(&st_Temp);
		if(f_Dist < f_BestDist)
		{
			f_BestDist = f_Dist;
			pst_BestGO = pst_BoneGO;
		}
	}

	return pst_BestGO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *ANI_pst_GetObjectByCanal(OBJ_tdst_GameObject *_pst_RefGO, UCHAR _uc_Canal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentBone;
	OBJ_tdst_GameObject *pst_BoneGO;
	OBJ_tdst_Group		*pst_Skeleton;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!OBJ_b_TestIdentityFlag(_pst_RefGO, OBJ_C_IdentityFlag_Anims))
	||	(!_pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
	) return NULL;

	pst_Skeleton = _pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton;

	pst_CurrentBone = pst_Skeleton->pst_AllObjects->p_Table + _uc_Canal;
	pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
	if(TAB_b_IsAHole(pst_BoneGO)) return NULL;
	return pst_BoneGO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_ChangeBoneShape(OBJ_tdst_GameObject *_pst_RefGO, UCHAR _uc_Canal, UCHAR _uc_Pos, BOOL _b_AIFunction)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_CurrentBone;
	TAB_tdst_PFelem			*pst_CurrentVisu, *pst_EndVisu;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	OBJ_tdst_Group			*pst_Skeleton;
	OBJ_tdst_GameObject		*pst_BoneGO, *pst_VisuGO;
	ULONG					ul_MatrixID;
	GRO_tdst_Visu			*pst_Visu;
	GRO_tdst_Struct			*pst_Gro;
	ULONG					ul_NbOfGizmos;
	UCHAR					i, uc_ENG_Canal;
	MDF_tdst_Modifier		*pst_NextMdf, *pst_Modifier;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_uc_Pos == 0xFF) return;

	if(!_pst_RefGO || !_pst_RefGO->pst_Base || !_pst_RefGO->pst_Base->pst_GameObjectAnim) return;

	pst_GOAnim = _pst_RefGO->pst_Base->pst_GameObjectAnim;
	pst_Skeleton = pst_GOAnim->pst_Skeleton;

	if(_b_AIFunction)
	{
		pst_BoneGO = ANI_pst_GetObjectByAICanal(_pst_RefGO, _uc_Canal);

		uc_ENG_Canal = ANI_uc_GetEngineCanalByAICanal(_pst_RefGO, _uc_Canal);

		if((uc_ENG_Canal == 255) || !pst_BoneGO)
		{
			ERR_X_Warning(0, "[Shape] Invalid Canal (AI)", _pst_RefGO->sz_Name);
			return;
		}
	}
	else
	{
		if(_uc_Canal >= TAB_ul_PFtable_GetNbElems(pst_Skeleton->pst_AllObjects))
		{
			ERR_X_Warning(0, "[Shape] Invalid Canal (Engine)", _pst_RefGO->sz_Name);
			return;
		}

		pst_CurrentBone = pst_Skeleton->pst_AllObjects->p_Table + _uc_Canal;
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

		uc_ENG_Canal = _uc_Canal;
	}

	/* IA can modify the default visuals */
	if(_b_AIFunction)
		pst_GOAnim->auc_DefaultVisu[uc_ENG_Canal] = _uc_Pos;

	if(!pst_BoneGO || TAB_b_IsAHole(pst_BoneGO)) return;

	if(_uc_Pos == 0xFE)
	{
		pst_BoneGO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_ForceInvisible;
		pst_BoneGO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_ForceInvisibleInit;
		return;
	}

	if(_uc_Pos == 0xFD)
	{
		pst_BoneGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceInvisible;
		pst_BoneGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceInvisibleInit;
		return;
	}

	
	// First, we check that we are not trying to change for the same visual ...
	if(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Group))
	{
		pst_Visu = pst_BoneGO->pst_Base->pst_Visu;
		
		if(!pst_BoneGO->pst_Extended || !pst_BoneGO->pst_Extended->pst_Group) return;
		
		/* Gets the new visual from the Shape. */
		pst_CurrentVisu = TAB_pst_PFtable_GetFirstElem(pst_BoneGO->pst_Extended->pst_Group->pst_AllObjects);
		pst_EndVisu = TAB_pst_PFtable_GetLastElem(pst_BoneGO->pst_Extended->pst_Group->pst_AllObjects);
		i = 0;
		
		while(i < _uc_Pos)
		{
			i++;
			pst_CurrentVisu++;
			if(pst_CurrentVisu > pst_EndVisu)
			{
#ifdef ACTIVE_EDITORS
				pst_CurrentVisu = pst_EndVisu;
				if ( !_pst_RefGO->emittedBoneWarning )
				{
					/*~~~~~~~~~~~~*/
					char az[ 512 ];
					/*~~~~~~~~~~~~*/
					snprintf(
						    az, sizeof(az),
						    "SHAPE : Invalid visual number (%d) for bone number (%d) for gao (%s)",
						    _uc_Pos,
						    _uc_Canal,
						    _pst_RefGO->sz_Name );
					ERR_X_Warning( 0, az, NULL );
					_pst_RefGO->emittedBoneWarning = true;
				}
#endif
				return;
			}			
		}		

		pst_VisuGO = (OBJ_tdst_GameObject *) pst_CurrentVisu->p_Pointer;
		if(!pst_VisuGO) return;

/* */
		OBJ_VertexColor_Free( pst_BoneGO );
		/*
		if(pst_Visu && pst_Visu->dul_VertexColors)
		{
			MEM_Free(pst_Visu->dul_VertexColors);
			pst_Visu->dul_VertexColors = NULL;
		}
		*/

		if(pst_Visu && pst_VisuGO && pst_VisuGO->pst_Base && pst_VisuGO->pst_Base->pst_Visu && pst_VisuGO->pst_Base->pst_Visu->dul_VertexColors)
		{
			LONG	l_Size;

			l_Size = (*pst_VisuGO->pst_Base->pst_Visu->dul_VertexColors + 1) * 4;
			pst_Visu->dul_VertexColors = (ULONG *) MEM_p_Alloc(l_Size);
			L_memcpy(pst_Visu->dul_VertexColors, pst_VisuGO->pst_Base->pst_Visu->dul_VertexColors, l_Size);
		}
/* */

		if(pst_Visu && pst_VisuGO->pst_Base && pst_VisuGO->pst_Base->pst_Visu && (pst_VisuGO->pst_Base->pst_Visu->pst_Material == pst_Visu->pst_Material) && (pst_VisuGO->pst_Base->pst_Visu->pst_Object == pst_Visu->pst_Object) && !(pst_BoneGO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInvisible))
			return;

		/* Destruct all modifiers on current bone */
		if(pst_BoneGO->pst_Extended && pst_BoneGO->pst_Extended->pst_Modifiers)
		{
			pst_Modifier = pst_BoneGO->pst_Extended->pst_Modifiers;
			while(pst_Modifier)
			{
				pst_NextMdf = pst_Modifier->pst_Next;
				MDF_Modifier_Destroy(pst_Modifier);
				pst_Modifier = pst_NextMdf;
			}

			pst_BoneGO->pst_Extended->pst_Modifiers = NULL;
		}

		if(pst_Visu && ((pst_Gro = pst_Visu->pst_Object) != NULL))
		{
			pst_Gro->i->pfn_AddRef(pst_Gro, -1);
			pst_Visu->pst_Object = NULL;
		}

		if(pst_Visu && ((pst_Gro = pst_Visu->pst_Material) != NULL))
		{
			pst_Gro->i->pfn_AddRef(pst_Gro, -1);
			pst_Visu->pst_Material = NULL;
		}

		if(_uc_Pos == (UCHAR) - 1)
		{
			/* Resets the Visu flag. */
			pst_BoneGO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Visu;
			return;
		}
	}
	else
		return;

	pst_BoneGO->ul_IdentityFlags |= OBJ_C_IdentityFlag_Visu;

	/* We check if the GO has enough Additionnal Matrix to fit the new visual (Skin) */
	if(pst_VisuGO->pst_Base && pst_VisuGO->pst_Base->pst_AddMatrix)
	{
		ul_NbOfGizmos = (ULONG) pst_VisuGO->pst_Base->pst_AddMatrix->l_Number;

		if(ul_NbOfGizmos)
		{
			OBJ_GameObject_AllocateGizmo(pst_BoneGO, ul_NbOfGizmos, TRUE);

			/* Copy the Gizmos of the Visual in the Bone. */
			L_memcpy
			(
				pst_BoneGO->pst_Base->pst_AddMatrix->dst_GizmoPtr,
				pst_VisuGO->pst_Base->pst_AddMatrix->dst_GizmoPtr,
				ul_NbOfGizmos * sizeof(OBJ_tdst_GizmoPtr)
			);

			for(i = 0; i < ul_NbOfGizmos; i++)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				OBJ_tdst_GizmoPtr	*pst_GizmoPtr;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_GizmoPtr = (pst_BoneGO->pst_Base->pst_AddMatrix->dst_GizmoPtr + i);
				ul_MatrixID = pst_GizmoPtr->l_MatrixId;
				pst_GizmoPtr->pst_GO = _pst_RefGO;
				pst_GizmoPtr->pst_Matrix = &(_pst_RefGO->pst_Base->pst_AddMatrix->dst_Gizmo + ul_MatrixID)->st_Matrix;
			}
		}
		else
		{
			if(pst_BoneGO->pst_Base->pst_AddMatrix)
			{
				if(pst_BoneGO->pst_Base->pst_AddMatrix->dst_Gizmo)
				{
					MEM_SafeFree(pst_BoneGO->pst_Base->pst_AddMatrix->dst_Gizmo);
				}

				MEM_SafeFree(pst_BoneGO->pst_Base->pst_AddMatrix);
				pst_BoneGO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_AdditionalMatrix;
				pst_BoneGO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_AddMatArePointer;
			}
		}
	}

	if(pst_BoneGO->pst_Base && !pst_BoneGO->pst_Base->pst_Visu)
		pst_BoneGO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Visu;
	OBJ_ChangeIdentityFlags
	(
		pst_BoneGO,
		pst_BoneGO->ul_IdentityFlags | OBJ_C_IdentityFlag_Visu,
		pst_BoneGO->ul_IdentityFlags
	);

	/* We have changed the visual of a bone. We increase its new visual Ref. */
	if(pst_VisuGO->pst_Base)
	{
		pst_Visu = pst_VisuGO->pst_Base->pst_Visu;

		if(pst_Visu && ((pst_Gro = pst_Visu->pst_Object) != NULL))
		{
			pst_Gro->i->pfn_AddRef(pst_Gro, 1);
			pst_BoneGO->pst_Base->pst_Visu->pst_Object = pst_Gro;
#if defined(_XBOX)
			pst_BoneGO->pst_Base->pst_Visu->IndexInMultipleVBList = pst_VisuGO->pst_Base->pst_Visu->IndexInMultipleVBList;
#endif	// defined(_XBOX)

#if defined(_XENON_RENDER)
            // Must update all Xenon fields of the visu for the new geo
            OBJ_UpdateXenonVisu(pst_BoneGO->pst_Base->pst_Visu, pst_Visu, TRUE);
#endif
		}

		if(pst_Visu && ((pst_Gro = pst_Visu->pst_Material) != NULL))
		{
			pst_Gro->i->pfn_AddRef(pst_Gro, 1);
			pst_BoneGO->pst_Base->pst_Visu->pst_Material = pst_Gro;
		}

		if(pst_Visu) pst_BoneGO->pst_Base->pst_Visu->ul_DrawMask = pst_VisuGO->pst_Base->pst_Visu->ul_DrawMask;
	}

	if(pst_VisuGO->pst_Extended && pst_VisuGO->pst_Extended->pst_Modifiers)
	{
		MDF_Modifier_DuplicateList(pst_BoneGO, pst_VisuGO->pst_Extended->pst_Modifiers);
		UpdateModifiersGroup(pst_BoneGO, pst_Skeleton);
	}

	/* Resets Flags (Anti-Bug) */
	pst_BoneGO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_ForceInvisible;
	pst_BoneGO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_ForceInvisibleInit;

	/* Recompute the BV of the Bone cause the Visual has changed. */
	OBJ_ComputeBV(pst_BoneGO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_ChangeShape(OBJ_tdst_GameObject *_pst_GO, ANI_tdst_Shape *_pst_NewShape)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	ANI_tdst_Shape			*pst_Shape;
	BOOL					b_OldShape;
	UCHAR					auc_AI_Canal[ANI_C_MaxNumberOfCanal];
	UCHAR					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	) return;

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	if(!_pst_NewShape)
	{
		if(!pst_GOAnim->pst_DefaultShape) return;
		pst_Shape = pst_GOAnim->pst_DefaultShape;
	}
	else
		pst_Shape = _pst_NewShape;

	/* If we change to the same Shape, we do nothing */
	if(pst_GOAnim->pst_Shape == pst_Shape) return;

	/* We save the previous Shape AI-ENG canal associations in case the new one has only Partial info with "Keep current" */
	if(pst_GOAnim->pst_Shape)
	{
		b_OldShape = TRUE;
		L_memcpy(auc_AI_Canal, pst_GOAnim->pst_Shape->auc_AI_Canal, sizeof(UCHAR) * ANI_C_MaxNumberOfCanal);
	}
	else
		b_OldShape = FALSE;

	pst_GOAnim->pst_Shape = pst_Shape;

	if(b_OldShape)
	{
		for(i = 0; i < ANI_C_MaxNumberOfCanal; i++)
		{		
			 /* If the new Shape has no AI canal for the bone number i, we take the AI canal of the previous shape */			
			if(pst_GOAnim->pst_Shape->auc_AI_Canal[i] == 0xFF)
				pst_GOAnim->pst_Shape->auc_AI_Canal[i] = auc_AI_Canal[i];
		}
	}

	ANI_ApplyShape(_pst_GO);
}

/*
 =======================================================================================================================
    Aim:    This function is call once every frame and compute the animations of all objects
 =======================================================================================================================
 */
void ANI_OneCall(OBJ_tdst_GameObject *_pst_GAO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Speed;
	MATH_tdst_Vector	*pst_Speed;
	float				f_Time, f_Total;
	EVE_tdst_Data		*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * Get the speed of the game object (in case the frequency is enslaved to the
	 * speed)
	 */
	if(OBJ_b_TestIdentityFlag(_pst_GAO, OBJ_C_IdentityFlag_Dyna))
	{
		DYN_GetSpeedVector(_pst_GAO->pst_Base->pst_Dyna, &st_Speed);
		pst_Speed = &st_Speed;
	}
	else
		pst_Speed = NULL;

	/*
	 * The GAO was culled during the previous frame. It was played in SpeedMode. Now,
	 * it is visible again. We align all the Tracks on the MagicBox one.
	 */
	if
	(
		_pst_GAO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible
	&&	(!(_pst_GAO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled) || (_pst_GAO->pst_Extended && _pst_GAO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_AlwaysPlay))
	&&	_pst_GAO->pst_Base
	&&	_pst_GAO->pst_Base->pst_GameObjectAnim
	&&	_pst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0]
	&&	!(_pst_GAO->pst_Base->pst_GameObjectAnim->uc_Synchro)
	)
	{
		pst_Data = _pst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data;
		EVE_GetTracksTime(pst_Data, 0, &f_Time, &f_Total);
		EVE_SetTracksTime(pst_Data, f_Time);

		/* If we are in a Blend. We stop it. */
		if(_pst_GAO->pst_Base->pst_GameObjectAnim->uc_AnimUsed != 1)
			_pst_GAO->pst_Base->pst_GameObjectAnim->uc_AnimUsed = 1;

		_pst_GAO->pst_Base->pst_GameObjectAnim->uc_Synchro = 1;
	}

	ANI_PlayGameObjectAnimation(_pst_GAO, _pst_GAO->pst_Base->pst_GameObjectAnim, TIM_gf_dt, pst_Speed);
}

/*
 =======================================================================================================================
    Aim:    This function is call once every frame and compute the animations of all objects
 =======================================================================================================================
 */
void ANI_MainCall(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFtable	*pst_AnimEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GameObject;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	ERR_gpst_ContextGAO = NULL;
	ERR_gpsz_ContextString = "Anim";
#endif

	/* Loop thru the EOT Anim Table. */
	pst_AnimEOT = &_pst_World->st_EOT.st_Anims;

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AnimEOT);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AnimEOT);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GameObject = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GameObject) || !pst_GameObject->pst_Base || !pst_GameObject->pst_Base->pst_GameObjectAnim)
			continue;

#ifdef ACTIVE_EDITORS
		ERR_gpst_ContextGAO = pst_GameObject;
#endif

		if(!pst_GameObject->pst_Extended || !(pst_GameObject->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_OptimAnim))
		{
			ANI_OneCall(pst_GameObject);
		}
		else
		{
			if(pst_GameObject->pst_Extended)
				pst_GameObject->pst_Extended->uw_ExtraFlags &= ~OBJ_C_ExtraFlag_OptimAnim;

			if
			(
				pst_GameObject->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible
			&&	(!(pst_GameObject->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled) || (pst_GameObject->pst_Extended && (pst_GameObject->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_AlwaysPlay)))
			&&	pst_GameObject->pst_Base
			&&	pst_GameObject->pst_Base->pst_GameObjectAnim
			&&	pst_GameObject->pst_Base->pst_GameObjectAnim->apst_Anim[0]
			&&	!(pst_GameObject->pst_Base->pst_GameObjectAnim->uc_Synchro)
			)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~*/
				float			f_Time;
				EVE_tdst_Data	*pst_Data;
				/*~~~~~~~~~~~~~~~~~~~~~~*/

				pst_Data = pst_GameObject->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data;
				EVE_GetTracksTime(pst_Data, 0, &f_Time, NULL);
				EVE_SetTracksTime(pst_Data, f_Time);

				/* If we are in a Blend. We stop it. */
				if(pst_GameObject->pst_Base->pst_GameObjectAnim->uc_AnimUsed != 1)
					pst_GameObject->pst_Base->pst_GameObjectAnim->uc_AnimUsed = 1;

				pst_GameObject->pst_Base->pst_GameObjectAnim->uc_Synchro = 1;
			}
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Recomputes Flash Matrix or Reference Matrix of the Blended Anim.

    Note:   This function does not save and restore the context. (Some global variables for Event player are modified)
 =======================================================================================================================
 */
void ANI_RecomputeFlash(OBJ_tdst_GameObject *_pst_GO, ANI_tdst_Anim *_pst_Anim)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix st_Delta	ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Matrix			st_Matrix, st_Save, *pst_Save;
	USHORT						uw_SaveNumTracks;
	float						f_Time;
	ANI_tdst_Anim				*pst_Anim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO)
	{
		return;
	}

	pst_Anim = _pst_Anim;

	/* Save the Global/Local Matrix of the Object depending on hierarchy or not */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
	{
		MATH_CopyMatrix(&st_Save, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
		pst_Save = &st_Save;
	}
	else
	{
		MATH_CopyMatrix(&st_Matrix, _pst_GO->pst_GlobalMatrix);
		pst_Save = _pst_GO->pst_GlobalMatrix;
		_pst_GO->pst_GlobalMatrix = &st_Matrix;
	}

	/* Update Events global vars. */
	EVE_gpst_OwnerGAO = _pst_GO;
	EVE_gpst_CurrentData = pst_Anim->pst_Data;
	EVE_gpst_CurrentListTracks = EVE_gpst_CurrentData->pst_ListTracks;
	EVE_gpst_CurrentRefMatrix = &MATH_gst_IdentityMatrix;
	ENG_gb_EVERunning = FALSE;
	
#ifdef ANIMS_USE_ARAM
	if
	(
		_pst_GO->pst_Base 
	&& 	_pst_GO->pst_Base->pst_GameObjectAnim 
	&& 	_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit 
	&& 	(EVE_gpst_CurrentListTracks->ul_GC_Flags & EVE_C_ListTracks_UseARAM)
	)
	{		
		ACT_i_Cache_LoadAnim(_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit, EVE_gpst_CurrentListTracks);
	}

	if(EVE_gpst_CurrentListTracks && EVE_gpst_CurrentListTracks->pst_Cache)
		EVE_gpst_CurrentListTracks->pst_Cache->l_Counter = 0;
#endif	


	/* Resets scale because the Animation Player only modifies T and the 3x3 matrix. */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		MATH_ClearScale(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, 1);
	else
		MATH_ClearScale(_pst_GO->pst_GlobalMatrix, 1);

	if(EVE_gpst_CurrentListTracks)
	{
		/* Finds the number of Magic Box Tracks */
		uw_SaveNumTracks = EVE_gpst_CurrentListTracks->uw_NumTracks;
		if(EVE_gpst_CurrentListTracks->pst_AllTracks[1].uw_Gizmo == (USHORT) - 1)
			EVE_gpst_CurrentListTracks->uw_NumTracks = 2;
		else
			EVE_gpst_CurrentListTracks->uw_NumTracks = 1;

		/* Run Event Tracks of Magix Box with a dt of 0 */
		f_Time = TIM_gf_dt;
		TIM_gf_dt = 0.0f;

		EVE_gb_CanFlash = FALSE;
		EVE_b_RunAllTracks(pst_Anim->pst_Data, NULL);
		EVE_gb_CanFlash = TRUE;

		TIM_gf_dt = f_Time;

		/* Restore real number of track */
		EVE_gpst_CurrentListTracks->uw_NumTracks = uw_SaveNumTracks;
	}

	/* Restore the Global/Local Matrix of the Object. */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
	{
		MATH_CopyMatrix(&st_Matrix, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
		MATH_CopyMatrix(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, &st_Save);
	}
	else
		_pst_GO->pst_GlobalMatrix = pst_Save;

	MATH_InvertMatrix(&st_Delta, &st_Matrix);

	/* Delta (Old Flash/Object) * pst_Save (Object/[World or Father]) = New Flash */
	if(pst_Anim == _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0])
		MATH_MulMatrixMatrix(OBJ_pst_GetFlashMatrix(_pst_GO), &st_Delta, pst_Save);
	else
		MATH_MulMatrixMatrix(&pst_Anim->st_Ref, &st_Delta, pst_Save);
}

/*$F
 =======================================================================================================================
    Aim:    The Absolute Matrix of _pst_GO has changed (DYN, AI, COL ... whatever).
			We want to update the Flash matrix (or/and the Reference Matrix) of this Object.

    Note:   To update the Flash Matrix, we compute the "Delta" matrix to go from the Flash matrix to the place where
            the Anim player would like to place the GO at the current Frame. We apply the invert matrix of the result
            to the real position of the GameObject and we find the new Flash matrix.

			We use EVE_RunAllTracks on only the 2 MagicBox tracks to do that. Nevertheless, we do NOT want to allow Flash 
			Events.
 =======================================================================================================================
 */

void ANI_UpdateFlash(OBJ_tdst_GameObject *_pst_GO, UCHAR _uc_Mode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_SaveGO;
	EVE_tdst_Data		*pst_SaveData;
	EVE_tdst_ListTracks *pst_SaveListTracks;
	EVE_tdst_Track		*pst_SaveTrack;
	EVE_tdst_Event		*pst_SaveEvent;
	EVE_tdst_Params		*pst_SaveParam;
	MATH_tdst_Matrix	*pst_SaveRefMatrix;
	BOOL				b_EVERunningSave;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!_pst_GO
	||	(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	||	(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]
	) return;

	/* Save the Events Global vars. */
	pst_SaveGO = EVE_gpst_OwnerGAO;
	pst_SaveData = EVE_gpst_CurrentData;
	pst_SaveListTracks = EVE_gpst_CurrentListTracks;
	pst_SaveRefMatrix = EVE_gpst_CurrentRefMatrix;
	pst_SaveTrack = EVE_gpst_CurrentTrack;
	pst_SaveParam = EVE_gpst_CurrentParam;
	pst_SaveEvent = EVE_gpst_CurrentEvent;
	b_EVERunningSave = ENG_gb_EVERunning;

	if(_uc_Mode & ANI_C_UpdateLeadAnim)
	{
		ANI_RecomputeFlash(_pst_GO, _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]);
	}

	if(ANI_b_Blend(_pst_GO) && (_uc_Mode & ANI_C_UpdateBlendAnim))
	{
		ANI_RecomputeFlash(_pst_GO, _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]);
	}

	/* Restore the Events Global vars. */
	EVE_gpst_OwnerGAO = pst_SaveGO;
	EVE_gpst_CurrentData = pst_SaveData;
	EVE_gpst_CurrentListTracks = pst_SaveListTracks;
	EVE_gpst_CurrentRefMatrix = pst_SaveRefMatrix;
	EVE_gpst_CurrentTrack = pst_SaveTrack;
	EVE_gpst_CurrentParam = pst_SaveParam;
	EVE_gpst_CurrentEvent = pst_SaveEvent;
	ENG_gb_EVERunning = b_EVERunningSave;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

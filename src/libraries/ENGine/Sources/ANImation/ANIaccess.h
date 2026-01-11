/*$T ANIaccess.h GC! 1.081 05/29/01 10:10:13 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$F
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    File name   : ANIaccess.h
    Module name : ENGine / ANImation

    Description : This file contains the fonctions to access the structure of the ANI module
                  (animation player).

    Author      : Alexis Vaisse
  
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __ANIACCESS_H__
#define __ANIACCESS_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "BASe/BASsys.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "SDK/Sources/TABles/TABles.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL ANI_b_TestPlayMode(ANI_tdst_Anim *_pst_Anim, USHORT _uw_PlayMode)
{
	return((_pst_Anim->uw_Flag & ANI_C_AnimFlag_PlayModeMask) == _uw_PlayMode);
}

/*
 =======================================================================================================================
    Aim:    Is this Anim still played or is it finished ?
 =======================================================================================================================
 */
_inline_ BOOL ANI_b_PlayAnim(ANI_tdst_Anim *_pst_Anim)
{
	return(!(_pst_Anim->uw_Flag & ANI_C_AnimFlag_DontPlayAnim));
}

/*
 =======================================================================================================================
    Aim:    Is the GO processing a Blend ?
 =======================================================================================================================
 */
_inline_ BOOL ANI_b_Blend(OBJ_tdst_GameObject *_pst_GO)
{
	if
	(
		!_pst_GO
	||	!(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	) return FALSE;

	return(_pst_GO->pst_Base->pst_GameObjectAnim->uc_AnimUsed & 2);
}

/*
 =======================================================================================================================
    Aim:    The Blend length has been reached. We stop the Blend.
 =======================================================================================================================
 */
_inline_ void ANI_StopBlend(OBJ_tdst_GameObject *_pst_GO)
{
		if
	(
		!_pst_GO
	||	!(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	) return;

	_pst_GO->pst_Base->pst_GameObjectAnim->uc_AnimUsed &= ~2;
}

/*
 =======================================================================================================================
    Aim:    We have translated the MagicBox of an object by a given vector. We also aplly this vector to Flash Matrix
            (and Reference Matrix if the GO is playing a Blend)
 =======================================================================================================================
 */
_inline_ void ANI_UpdateFlashAfterMagicBoxTranslation
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Vector,
	UCHAR				_uc_Mode
)
{
	if
	(
		!_pst_GO
	||	!(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	) return;

	/* Update Flash Matrix */
	if((_uc_Mode & ANI_C_UpdateLeadAnim) && OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	*pst_FlashMatrix;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_FlashMatrix = OBJ_pst_GetFlashMatrix(_pst_GO);

		MATH_AddEqualVector(&pst_FlashMatrix->T, _pst_Vector);
	}

	/* Update Anim blend Reference Matrix if needed */
	if((_uc_Mode & ANI_C_UpdateBlendAnim) && ANI_b_Blend(_pst_GO))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ANI_st_GameObjectAnim	*pst_GOAnim;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

		/* Update Blend Reference Matrix. */
		MATH_AddEqualVector(&pst_GOAnim->apst_Anim[1]->st_Ref.T, _pst_Vector);

		/* Update Other Blend Data. */
		MATH_AddEqualVector((((MATH_tdst_Vector *) &pst_GOAnim->apst_Anim[0]->st_Ref) + 1), _pst_Vector);
		MATH_AddEqualVector(&pst_GOAnim->apst_Anim[0]->st_Ref.T, _pst_Vector);
	}
}

/*
 =======================================================================================================================
    Note:   Can return TAB_Cul_BadIndex.
 =======================================================================================================================
 */
_inline_ ULONG ANI_ul_GetFatherIndex(OBJ_tdst_Group *_pst_Skeleton, ULONG _ul_ChildIndex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Child, *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Skeleton) return TAB_Cul_BadIndex;

	pst_Child = (OBJ_tdst_GameObject *) (_pst_Skeleton->pst_AllObjects->p_Table + _ul_ChildIndex)->p_Pointer;

	if
	(
		TAB_b_IsAHole(pst_Child)
	||	!OBJ_b_TestIdentityFlag(pst_Child, OBJ_C_IdentityFlag_Hierarchy)
	||	!OBJ_pst_GetFather(pst_Child)
	) return TAB_Cul_BadIndex;

	pst_Father = OBJ_pst_GetFather(pst_Child);

	return(TAB_ul_PFtable_GetElemIndexWithPointer(_pst_Skeleton->pst_AllObjects, pst_Father));
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __ANIACCESS_H__ */

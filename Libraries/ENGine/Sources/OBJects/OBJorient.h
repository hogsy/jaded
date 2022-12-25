/*$T OBJorient.h GC! 1.081 05/09/00 11:47:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __OBJ_ORIENT_H__
#define __OBJ_ORIENT_H__

#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "MATHs/MATH.h"
#include "BASe/MEMory/MEM.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Functions handling the rotation matrix of an object
 ***********************************************************************************************************************
 */

void				OBJ_RotateLocalX(OBJ_tdst_GameObject *, float);
void				OBJ_RotateLocalY(OBJ_tdst_GameObject *, float);
void				OBJ_RotateLocalZ(OBJ_tdst_GameObject *, float);
void				OBJ_SightSet(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void				OBJ_SightGeneralSet(OBJ_tdst_GameObject *, MATH_tdst_Vector *, MATH_tdst_Vector *);
void				OBJ_SightGet(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void				OBJ_SightInitGet(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void				OBJ_BankingGet(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void				OBJ_HorizonGet(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void				OBJ_BankingSet(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void				OBJ_BankingGeneralSet(OBJ_tdst_GameObject *, MATH_tdst_Vector *, MATH_tdst_Vector *);
void				OBJ_ScaleSet(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void				OBJ_ZoomSet(OBJ_tdst_GameObject *, float);
void				OBJ_TransformAbsoluteMatrix(OBJ_tdst_GameObject *, MATH_tdst_Matrix *, MATH_tdst_Vector *);
void				OBJ_EyeFollow(OBJ_tdst_GameObject *, MATH_tdst_Vector *, float, ULONG, BOOL, BOOL, BOOL, BOOL, BOOL, BOOL);

/*$4
 ***********************************************************************************************************************
    Direct access to absolute matrix £
    OBJ_SetAbsolutePosition £
    OBJ_pst_GetAbsolutePosition £
    OBJ_pst_GetAbsoluteMatrix £
    OBJ_MakeAbsoluteMatrix £
    OBJ_SetAbsoluteMatrix £
    OBJ_AddAbsoluteVector £
 ***********************************************************************************************************************
 */

void				OBJ_SetAbsolutePosition(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void                OBJ_SetLocalPosition(OBJ_tdst_GameObject *, MATH_tdst_Vector * );
void				OBJ_AddAbsoluteVector(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
MATH_tdst_Matrix	*OBJ_pst_GetAbsoluteMatrix(OBJ_tdst_GameObject *);
MATH_tdst_Matrix	*OBJ_pst_GetLocalMatrix(OBJ_tdst_GameObject *);
void				OBJ_SetAbsoluteMatrix(OBJ_tdst_GameObject *, MATH_tdst_Matrix *);

/*
 =======================================================================================================================
    Aim:    Return a pointer to the absolute position of an object
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *OBJ_pst_GetAbsolutePosition(OBJ_tdst_GameObject *_pst_Object)
{
	return(MATH_pst_GetTranslation(_pst_Object->pst_GlobalMatrix));
}

/*$4
 ***********************************************************************************************************************
    Direct access to initial absolute matrix £
    OBJ_pst_GetInitialAbsoluteMatrix £
    OBJ_SetInitialAbsoluteMatrix(object* matrix*) £
    OBJ_SetInitialAbsolutePosition £
    OBJ_pst_GetInitialAbsolutePosition £
    OBJ_RestoreInitialPos £
    OBJ_AddAbsoluteInitalVector £
 ***********************************************************************************************************************
 */

void	OBJ_MakeAbsoluteMatrix(OBJ_tdst_GameObject *, MATH_tdst_Matrix *);
void	OBJ_AddAbsoluteInitalVector(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void	OBJ_SetInitialAbsolutePosition(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
void	OBJ_ComputeGlobalWhenHie(OBJ_tdst_GameObject *);
void	OBJ_ComputeLocalWhenHie(OBJ_tdst_GameObject *);
void    OBJ_ComputeGlobalWithLocal(OBJ_tdst_GameObject *, MATH_tdst_Matrix *, BOOL );

/*
 =======================================================================================================================
    Aim:    Returns a pointer to the initial absolute matrix of the object, (in case it has one)

    Note:   If the object has no orientation, the returned matrix does not countain the translation of the object. You
            can use (OBJ_MakeAbsoluteMatrix to do this)
 =======================================================================================================================
 */
_inline_ MATH_tdst_Matrix *OBJ_pst_GetInitialAbsoluteMatrix(OBJ_tdst_GameObject *_pst_Object)
{
	/* If object has a matrix, we return it */
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
		return(_pst_Object->pst_GlobalMatrix + 1);
	return(_pst_Object->pst_GlobalMatrix);
}

/*
 =======================================================================================================================
    Aim:    Return a pointer to the absolute position of an object
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *OBJ_pst_GetInitialAbsolutePosition(OBJ_tdst_GameObject *_pst_Object)
{
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
		return(MATH_pst_GetTranslation(_pst_Object->pst_GlobalMatrix + 1));
	else
		return(MATH_pst_GetTranslation(_pst_Object->pst_GlobalMatrix));
}

/*
 =======================================================================================================================
    Aim:    Returns a pointer to the Flash matrix of the object, (in case it has one)
 =======================================================================================================================
 */
_inline_ MATH_tdst_Matrix *OBJ_pst_GetFlashMatrix(OBJ_tdst_GameObject *_pst_Object)
{
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_FlashMatrix))
	{
		if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
			return(_pst_Object->pst_GlobalMatrix + 2);
		else
			return(_pst_Object->pst_GlobalMatrix + 1);
	}
	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Sets the Flash Matrix of an object.

    Note:   Allocates it if not.
 =======================================================================================================================
 */
_inline_ void OBJ_SetFlashMatrix(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Matrix *_pst_Matrix2Flash)
{
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_FlashMatrix))
		MATH_CopyMatrix(OBJ_pst_GetFlashMatrix(_pst_Object), _pst_Matrix2Flash);
	else
	{
		MATH_tdst_Matrix	st_Temp;

		_pst_Object->ul_IdentityFlags |= OBJ_C_IdentityFlag_FlashMatrix;
		if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
		{
			/* We Copy it because _pst_Matrix2Flash is often the GlobalMatrix and we are about to realloc it */
			MATH_CopyMatrix(&st_Temp, _pst_Matrix2Flash);

			_pst_Object->pst_GlobalMatrix = (MATH_tdst_Matrix *) MEM_p_Realloc
				(
					_pst_Object->pst_GlobalMatrix,
					3 * sizeof(MATH_tdst_Matrix)
				);
			MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix + 2, &st_Temp);
		}
		else
		{
			/* We Copy it because _pst_Matrix2Flash is often the GlobalMatrix and we are about to realloc it */
			MATH_CopyMatrix(&st_Temp, _pst_Matrix2Flash);

			_pst_Object->pst_GlobalMatrix = (MATH_tdst_Matrix *) MEM_p_Realloc
				(
					_pst_Object->pst_GlobalMatrix,
					2 * sizeof(MATH_tdst_Matrix)
				);
			MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix + 1, &st_Temp);
		}
	}
}

extern void OBJ_SetInitialAbsoluteMatrix(OBJ_tdst_GameObject *, MATH_tdst_Matrix *);
extern void OBJ_RestoreInitialPos(OBJ_tdst_GameObject *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif

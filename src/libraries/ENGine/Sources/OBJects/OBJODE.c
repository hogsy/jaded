/*$T OBJODE.c GC! 1.081 08/02/01 17:56:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "OBJODE.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#ifdef ODE_INSIDE
/*
=======================================================================================================================
Aim:    Has the object a valid ODE ?
=======================================================================================================================
*/
BOOL OBJ_HasODE(OBJ_tdst_GameObject *_pst_Obj)
{
	if((_pst_Obj && _pst_Obj->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) &&
		_pst_Obj->pst_Base && _pst_Obj->pst_Base->pst_ODE)
		return TRUE;
	else 
		return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Returns wether the ODE of the Object is a box or not.
 =======================================================================================================================
 */
BOOL OBJ_ODE_IsBox(OBJ_tdst_GameObject *_pst_GO)
{
	if(!OBJ_HasODE(_pst_GO))
		return FALSE;

	if(_pst_GO->pst_Base->pst_ODE->uc_Type == ODE_TYPE_BOX)
		return TRUE;

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Translates an ODE. _pst_Move is the translation vector in the Global coordinate system.
 =======================================================================================================================
 */
void OBJ_ODE_Move(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Move)
{
	/*~~~~~~~~~~~~*/
	DYN_tdst_ODE	*pst_ODE;
	/*~~~~~~~~~~~~*/

	if(!OBJ_HasODE(_pst_GO))
		return;

	pst_ODE = _pst_GO->pst_Base->pst_ODE;

	if(OBJ_ODE_IsBox(_pst_GO))
		MATH_AddEqualVector(&pst_ODE->st_Offset, _pst_Move);
}

/*
=======================================================================================================================
Aim:    Scale an ODE. _pst_Scale is the scale vector in the Global coordinate system.
=======================================================================================================================
*/
void OBJ_ODE_Scale(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Scale)
{
	/*~~~~~~~~~~~~*/
	DYN_tdst_ODE	*pst_ODE;
	/*~~~~~~~~~~~~*/

	if(!OBJ_HasODE(_pst_GO))
		return;

	pst_ODE = _pst_GO->pst_Base->pst_ODE;

	if(OBJ_ODE_IsBox(_pst_GO))
	{
		pst_ODE->f_X *= _pst_Scale->x;
		pst_ODE->f_Y *= _pst_Scale->y;
		pst_ODE->f_Z *= _pst_Scale->z;
	}
}
/*
=======================================================================================================================
Aim:    Rotate an ODE. 
=======================================================================================================================
*/
void OBJ_ODE_Rotate(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Global_Axis, MATH_tdst_Vector *_pst_Angle)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE	*pst_ODE;
	MATH_tdst_Vector	*pst_Axis, st_Axis;
	MATH_tdst_Matrix	st_TempMatrix, st_TempMatrix2, st_Rotation;
	float				f_Angle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_HasODE(_pst_GO))
		return;

	pst_ODE = _pst_GO->pst_Base->pst_ODE;

	if(OBJ_ODE_IsBox(_pst_GO))
	{

		pst_Axis = _pst_Global_Axis;

		f_Angle = _pst_Angle->x;
		if(f_Angle == 0)
		{
			f_Angle = _pst_Angle->y;
			if(f_Angle == 0)
			{
				f_Angle = _pst_Angle->z;
				if(f_Angle == 0) return;
			}
		}

		/* Transform axis into localaxis of object */
		MATH_SetIdentityMatrix(&st_TempMatrix);
		MATH_GetRotationMatrix(&st_TempMatrix, &pst_ODE->st_RotMatrix);
		MATH_SetRotationType(&st_TempMatrix);
		MATH_TranspEq33Matrix(&st_TempMatrix);

		MATH_SetIdentityMatrix(&st_TempMatrix2);
		st_TempMatrix2.lType = MATH_Ci_NoRotationNoAnyFormOfScale;
		MATH_Invert33Matrix(&st_TempMatrix2, _pst_GO->pst_GlobalMatrix);
		MATH_TransformVector(&st_Axis, &st_TempMatrix2, pst_Axis);

		MATH_TransformVector(&st_Axis, &st_TempMatrix, &st_Axis);
		MATH_NormalizeVector(&st_Axis, &st_Axis);

		/* Compute the matrix for the rotation around that axis */
		MATH_MakeRotationMatrix_AxisAngle(&st_Rotation, &st_Axis, f_Angle, NULL, 1);

		/* Make the rotation */ 
		MATH_TranspEq33Matrix(&st_TempMatrix);
		MATH_Mul33MatrixMatrix(&pst_ODE->st_RotMatrix, &st_Rotation, &st_TempMatrix, 1);
	}

}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

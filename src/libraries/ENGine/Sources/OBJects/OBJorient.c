/*$T OBJorient.c GC! 1.081 09/25/01 17:40:03 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Main functions that handle objects */
#include "Precomp.h"

/* #include "OBJ.h" */
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/GRP/GRPorient.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Functions handling the orientation
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Returns a pointer to the absolute matrix of the object, (in case it has one)

    Note:   WARNING!!: If the object has no orientation, the returned pointer points to a global matrix that will be
            valid only now ! be sure to copy the matrix if you need to keep it
 =======================================================================================================================
 */
MATH_tdst_Matrix *OBJ_pst_GetAbsoluteMatrix(OBJ_tdst_GameObject *_pst_Object)
{
	return(_pst_Object->pst_GlobalMatrix);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Matrix *OBJ_pst_GetLocalMatrix(OBJ_tdst_GameObject *_pst_Object)
{
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Hierarchy))
		return(&_pst_Object->pst_Base->pst_Hierarchy->st_LocalMatrix);
	else
		return NULL;
}

/*
 =======================================================================================================================
    Aim:    Sets the matrix of an object (or the position if no matrix)

    Note:   If object is a group, the matrix will be set to all the objects of the group, taking the center of the BV
            of the group as center for rotation
 =======================================================================================================================
 */
void OBJ_SetAbsoluteMatrix(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Matrix *_pst_Matrix)
{
#if 0
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Group))
		GRP_SetAbsoluteMatrix(_pst_Object, _pst_Matrix);
	else
#endif
		MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix, _pst_Matrix);
}

/*
 =======================================================================================================================
    Aim:    Makes an absolute matrix from the position or/and orientation of a object

    Note:   If the object has a matrix, it is only copied. If not, the matrix made is an identity matrix with the
            translation of the object
 =======================================================================================================================
 */
void OBJ_MakeAbsoluteMatrix(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Matrix *_pst_DestMatrix)
{
	MATH_CopyMatrix(_pst_DestMatrix, _pst_Object->pst_GlobalMatrix);
}

/*
 =======================================================================================================================
    Aim:    Rotates an object around a pivot and translates the object

    Note:   To be finished...
 =======================================================================================================================
 */
void OBJ_TransformAbsoluteMatrix
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Matrix	*_pst_RotMatrix,
	MATH_tdst_Vector	*_pst_Pivot
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_DstMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_MulMatrixMatrix(&st_DstMat, OBJ_pst_GetAbsoluteMatrix(_pst_GO), _pst_RotMatrix);
}

/*
 =======================================================================================================================
    Aim:    Rotates an object local around X

    Note:   Object must have the flag oriented object. To be finished...
 =======================================================================================================================
 */
void OBJ_RotateLocalX(OBJ_tdst_GameObject *_pst_Object, float _f_Angle)
{
_Try_
	MATH_RotateMatrix_AroundLocalXAxis(_pst_Object->pst_GlobalMatrix, _f_Angle);
_Catch_
_End_
}

/*
 =======================================================================================================================
    Aim:    Rotates an object local around X

    Note:   Object must have the flag oriented object
 =======================================================================================================================
 */
void OBJ_RotateLocalY(OBJ_tdst_GameObject *_pst_Object, float _f_Angle)
{
_Try_
	MATH_RotateMatrix_AroundLocalYAxis(_pst_Object->pst_GlobalMatrix, _f_Angle);
_Catch_
_End_
}

/*
 =======================================================================================================================
    Aim:    Rotates an object local around X

    Note:   Object must have the flag oriented object
 =======================================================================================================================
 */
void OBJ_RotateLocalZ(OBJ_tdst_GameObject *_pst_Object, float _f_Angle)
{
_Try_
	MATH_RotateMatrix_AroundLocalZAxis(_pst_Object->pst_GlobalMatrix, _f_Angle);
_Catch_
_End_
}

/*
 =======================================================================================================================
    Aim:    Changes the sight of a object

    Note:   The sight is normalized and imposed. The current banking (Z axis) of the object is used to keep the current
            banking (Z axis) of the object (when possible)
 =======================================================================================================================
 */
void OBJ_SightSet(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Sight)
{
	ERR_X_Assert(_pst_Object != 0);
	ERR_X_Assert(!MATH_b_NulVectorWithEpsilon(_pst_Sight, Cf_Epsilon));
	MATH_OrientMatrix_UsingSight(_pst_Object->pst_GlobalMatrix, _pst_Sight, 0);
}

/*
 =======================================================================================================================
    Aim:    Changes the banking of a object

    Note:   The banking (local Z) is normalized and imposed. The current Y axis of the object is used to keep the
            current sighting (when possible)
 =======================================================================================================================
 */
void OBJ_BankingSet(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Banking)
{
	ERR_X_Assert(_pst_Object != 0);
	ERR_X_Assert(!MATH_b_NulVectorWithEpsilon(_pst_Banking, Cf_Epsilon));
	MATH_OrientMatrix_UsingBanking(_pst_Object->pst_GlobalMatrix, 0, _pst_Banking);
}

/*
 =======================================================================================================================
    Aim:    Changes the sighting of a object, using a given wanted vector for the banking

    Note:   The sighting is normalized and imposed. The current Z axis of the object is used to keep the object as near
            as possible from its current orientation
 =======================================================================================================================
 */
void OBJ_SightGeneralSet(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Sight, MATH_tdst_Vector *_pst_Banking)
{
	ERR_X_Assert(_pst_Object != 0);
	ERR_X_Assert(!MATH_b_NulVectorWithEpsilon(_pst_Sight, Cf_Epsilon));
	ERR_X_Assert(!MATH_b_NulVectorWithEpsilon(_pst_Banking, Cf_Epsilon));
	MATH_OrientMatrix_UsingSight(_pst_Object->pst_GlobalMatrix, _pst_Sight, _pst_Banking);
}

/*
 =======================================================================================================================
    Aim:    Changes the banking of a object

    Note:   The banking (local Z) is normalized and imposed. The given sighting is used to keep the object with the
            same sighting when possible
 =======================================================================================================================
 */
void OBJ_BankingGeneralSet
(
	OBJ_tdst_GameObject *_pst_Object,
	MATH_tdst_Vector	*_pst_Sight,
	MATH_tdst_Vector	*_pst_Banking
)
{
	ERR_X_Assert(_pst_Object != 0);
	ERR_X_Assert(!MATH_b_NulVectorWithEpsilon(_pst_Banking, Cf_Epsilon));
	ERR_X_Assert(!MATH_b_NulVectorWithEpsilon(_pst_Sight, Cf_Epsilon));
	MATH_OrientMatrix_UsingBanking(_pst_Object->pst_GlobalMatrix, _pst_Sight, _pst_Banking);
}

/*
 =======================================================================================================================
    Aim:    Returns the sight of an object
 =======================================================================================================================
 */
void OBJ_SightGet(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Sight)
{
_Try_
	ERR_X_Assert(_pst_Object != 0);
	MATH_NegVector(_pst_Sight, MATH_pst_GetYAxis(_pst_Object->pst_GlobalMatrix));
_Catch_
_End_
}

/*
 =======================================================================================================================
    Aim:    Returns the sight of an object
 =======================================================================================================================
 */
void OBJ_SightInitGet(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Sight)
{
	if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos)) return;

_Try_
	ERR_X_Assert(_pst_Object != 0);
	MATH_NegVector(_pst_Sight, MATH_pst_GetYAxis(_pst_Object->pst_GlobalMatrix + 1));
_Catch_
_End_
}

/*
 =======================================================================================================================
    Aim:    Returns the banking of an object
 =======================================================================================================================
 */
void OBJ_BankingGet(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Banking)
{
_Try_
	ERR_X_Assert(_pst_Object != 0);
	MATH_NormalizeVector(_pst_Banking, MATH_pst_GetZAxis(_pst_Object->pst_GlobalMatrix));
_Catch_
_End_
}

/*
 =======================================================================================================================
    Aim:    Returns the horizon of an object
 =======================================================================================================================
 */
void OBJ_HorizonGet(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Horizon)
{
_Try_
	ERR_X_Assert(_pst_Object != 0);
	MATH_NormalizeVector(_pst_Horizon, MATH_pst_GetXAxis(_pst_Object->pst_GlobalMatrix));
_Catch_
_End_
}

/*$4
 ***********************************************************************************************************************
    Functions handling the scale / zoom
 ***********************************************************************************************************************
 */
/*
 =======================================================================================================================
    Aim:
 =======================================================================================================================
 */

void OBJ_ComputeRecursivScale(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Scale)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Father;
	MATH_tdst_Vector	st_LocalScale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_InitVector(_pst_Scale, 1.0f, 1.0f, 1.0f);

	if
	(
		(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
	||	(!(_pst_GO->pst_Base))
	||	(!(_pst_GO->pst_Base->pst_Hierarchy))
	||	(!(_pst_GO->pst_Base->pst_Hierarchy->pst_Father))
	)
	{
		MATH_GetScale(_pst_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
		return;
	}


	pst_Father = _pst_GO;
	while(pst_Father->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		MATH_GetScale(&st_LocalScale, OBJ_pst_GetLocalMatrix(pst_Father));

		_pst_Scale->x *= st_LocalScale.x;
		_pst_Scale->y *= st_LocalScale.y;
		_pst_Scale->z *= st_LocalScale.z;

		pst_Father = OBJ_pst_GetFather(pst_Father);
	}

	MATH_GetScale(&st_LocalScale, pst_Father->pst_GlobalMatrix);

	_pst_Scale->x *= st_LocalScale.x;
	_pst_Scale->y *= st_LocalScale.y;
	_pst_Scale->z *= st_LocalScale.z;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ScaleSet(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Scale)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Hierarchy);
    if(b_Hierarchy && _pst_Object->pst_Base->pst_Hierarchy->pst_Father)  
	{

		if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Bone))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Scale;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			OBJ_ComputeRecursivScale(_pst_Object, &st_Scale);

			_pst_Scale->x /= st_Scale.x;
			_pst_Scale->y /= st_Scale.y;
			_pst_Scale->z /= st_Scale.z;
		}

		MATH_SetScale(OBJ_pst_GetLocalMatrix(_pst_Object), _pst_Scale);
		OBJ_ComputeGlobalWhenHie(_pst_Object);
	}
	else
		MATH_SetScale(_pst_Object->pst_GlobalMatrix, _pst_Scale);

	OBJ_ComputeBV(_pst_Object, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ZoomSet(OBJ_tdst_GameObject *_pst_Object, float f_Zoom)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*pst_M;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_M = OBJ_pst_GetAbsoluteMatrix(_pst_Object);
	MATH_SetZoom(pst_M, f_Zoom);

	/* If Object has animation, update its Flash Matrix */
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Anims) && OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_FlashMatrix))
	{
		MATH_SetZoom(OBJ_pst_GetFlashMatrix(_pst_Object), f_Zoom);
	}
	OBJ_ComputeBV(_pst_Object, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);
}

/*$4
 ***********************************************************************************************************************
    Functions handling the position
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_SetLocalPosition(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Pos )
{
	if( !OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Hierarchy) ) return;

	MATH_SetTranslation( &_pst_Object->pst_Base->pst_Hierarchy->st_LocalMatrix, _pst_Pos );
    OBJ_ComputeGlobalWhenHie( _pst_Object );
}

/*
 =======================================================================================================================
    Aim:    Set the position of an object
 =======================================================================================================================
 */
void OBJ_SetAbsolutePosition(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Vector)
{
	/* If the object controls a group we also move aqll objects of the group */
#if 0
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Group))
		GRP_SetAbsolutePosition(_pst_Object, _pst_Vector);
#endif
	/*
	 * If the object is oriented, we set the translation of the matrix, else we set
	 * directly the absolute position of the object
	 */
	MATH_SetTranslation(_pst_Object->pst_GlobalMatrix, _pst_Vector);
}

/*
 =======================================================================================================================
    Aim:    Adds a vector (in absolute coordinates) to an absolute position
 =======================================================================================================================
 */
void OBJ_AddAbsoluteVector(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Vector)
{
	/* If the object controls a group we also move aqll objects of the group */
#if 0
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Group)) GRP_AddAbsoluteVector(_pst_Object, _pst_Vector);
#endif
	MATH_AddEqualVector(&(_pst_Object->pst_GlobalMatrix->T), _pst_Vector);
	MATH_UpdateTranslationType(_pst_Object->pst_GlobalMatrix);
}

/*
 =======================================================================================================================
    Aim:    Adds a vector to the initial position of an actor
 =======================================================================================================================
 */
void OBJ_AddAbsoluteInitalVector(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Vector)
{
	/* If the object controls a group we also move aqll objects of the group */
#if 0
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Group))
		GRP_AddAbsoluteInitialVector(_pst_Object, _pst_Vector);
#endif
	MATH_AddEqualVector(OBJ_pst_GetInitialAbsolutePosition(_pst_Object), _pst_Vector);
	MATH_UpdateTranslationType(_pst_Object->pst_GlobalMatrix);
}

/*
 =======================================================================================================================
    Aim:    Set the initial position of an object
 =======================================================================================================================
 */
void OBJ_SetInitialAbsolutePosition(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Vector)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix M	ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Matrix	M1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * If the object controls a group we also move all initial positions of the
	 * objects of the group
	 */
#if 0
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Group))
		GRP_SetAbsoluteInitialPosition(_pst_Object, _pst_Vector);
#endif
	/*
	 * If the object is oriented, we set the translation of the matrix, else we set
	 * directly the absolute position of the object
	 */
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
	{
		MATH_SetTranslation(_pst_Object->pst_GlobalMatrix + 1, _pst_Vector);
	}
	else
	{
		MATH_SetTranslation(_pst_Object->pst_GlobalMatrix, _pst_Vector);
	}

	/* If hierarchy, initial pos is local to father */
	if
	(
		(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Hierarchy))
	&&	(_pst_Object->pst_Base->pst_Hierarchy->pst_Father)
	&&	(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
	)
	{
		MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(_pst_Object->pst_Base->pst_Hierarchy->pst_Father));
		MATH_MulMatrixMatrix(&M1, _pst_Object->pst_GlobalMatrix + 1, &M);
		MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix + 1, &M1);
	}
}

/*
 =======================================================================================================================
    Aim:    Sets the initial absolute matrix of an object
 =======================================================================================================================
 */
void OBJ_SetInitialAbsoluteMatrix(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Matrix *_pst_Matrix)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix M	ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Matrix	M1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos)) return;

	/* If object has a matrix, we return it */
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
		MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix + 1, _pst_Matrix);
	else
		MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix, _pst_Matrix);

	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_FlashMatrix))
		MATH_CopyMatrix(OBJ_pst_GetFlashMatrix(_pst_Object), _pst_Matrix);

	/* If hierarchy, initial pos is local to father */
	if
	(
		(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Hierarchy))
	&&	(_pst_Object->pst_Base->pst_Hierarchy->pst_Father)
	&&	(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
	)
	{
		MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(_pst_Object->pst_Base->pst_Hierarchy->pst_Father));
		MATH_MulMatrixMatrix(&M1, _pst_Object->pst_GlobalMatrix + 1, &M);
		MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix + 1, &M1);
	}
}

/*
 =======================================================================================================================
    Aim:    Recompute global matrix depending of local and father
 =======================================================================================================================
 */
void OBJ_ComputeGlobalWhenHie(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Father;
	OBJ_tdst_Hierarchy	*pst_Hie;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && _pst_GO->pst_Base && _pst_GO->pst_Base->pst_Hierarchy) 
	{
		pst_Hie = _pst_GO->pst_Base->pst_Hierarchy;
		pst_Father = pst_Hie->pst_Father;
		if(pst_Father)
		{
			MATH_MulMatrixMatrix
			(
				OBJ_pst_GetAbsoluteMatrix(_pst_GO),
				&pst_Hie->st_LocalMatrix,
				OBJ_pst_GetAbsoluteMatrix(pst_Father)
			);
		}
	}
}


/*
 =======================================================================================================================
    Aim:    Recompute global matrix depending of local and father
 =======================================================================================================================
 */
void OBJ_ComputeGlobalWithLocal(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Matrix *_pst_Matrix, BOOL _b_UseLast)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Father;
	MATH_tdst_Matrix	st_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy))
	||	(!(_pst_GO->pst_Base))
	||	(!(_pst_GO->pst_Base->pst_Hierarchy))
	||	(!(_pst_GO->pst_Base->pst_Hierarchy->pst_Father))
	)
	{
		MATH_CopyMatrix(_pst_Matrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
		return;
	}

	MATH_SetIdentityMatrix(_pst_Matrix);

	while(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		pst_Father = _pst_GO->pst_Base->pst_Hierarchy->pst_Father;
		if(pst_Father)
		{
			MATH_CopyMatrix(&st_Temp, _pst_Matrix);
			MATH_MulMatrixMatrix(_pst_Matrix, &st_Temp, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
			_pst_GO = pst_Father;
		}
		else
			break;
	}

	if(_b_UseLast)
	{
		MATH_CopyMatrix(&st_Temp, _pst_Matrix);
		MATH_MulMatrixMatrix(_pst_Matrix, &st_Temp, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
	}
}

/*
 =======================================================================================================================
    Aim:    Recompute local matrix depending of father
 =======================================================================================================================
 */
void OBJ_ComputeLocalWhenHie(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Father;
	MATH_tdst_Matrix M	ONLY_PSX2_ALIGNED(16);
	OBJ_tdst_Hierarchy	*pst_Hie;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		pst_Hie = _pst_GO->pst_Base->pst_Hierarchy;
		pst_Father = pst_Hie->pst_Father;
		if(pst_Father)
		{
			/* We want a valid Father Global Matrix before computing its child Local Matrix. */
			if(pst_Father->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	st_Temp;
				/*~~~~~~~~~~~~~~~~~~~~~~~~*/

				OBJ_ComputeGlobalWithLocal(pst_Father, &st_Temp, 1);
				MATH_InvertMatrix(&M, &st_Temp);
			}
			else
				MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(pst_Father));

			MATH_MulMatrixMatrix(&pst_Hie->st_LocalMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO), &M);
			if(pst_Hie->st_LocalMatrix.lType & MATH_Ci_Scale)
			{
				MATH_tdst_Vector st_scale;
				
				MATH_GetScale(&st_scale, &pst_Hie->st_LocalMatrix);
			    if(
			    	MATH_b_EqVectorWithEpsilon(&MATH_gst_IdentityVector, &st_scale, 0.001f/*Cf_EpsilonBig*/) ||
			    	MATH_b_EqVectorWithEpsilon(&MATH_gst_NulVector, &st_scale, 0.001f/*Cf_EpsilonBig*/)
			      )
			        MATH_ClearScale(&pst_Hie->st_LocalMatrix,1);

			}
			MATH_Orthonormalize(&pst_Hie->st_LocalMatrix);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Copy initial position (matrix or vector) into current pos
 =======================================================================================================================
 */
void OBJ_RestoreInitialPos(OBJ_tdst_GameObject *_pst_Object)
{
	/* Object has a father : Initial matrix is local */
	if
	(
		(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Hierarchy))
	&&	(_pst_Object->pst_Base)
	&&	(_pst_Object->pst_Base->pst_Hierarchy)
	&&	(_pst_Object->pst_Base->pst_Hierarchy->pst_FatherInit)
	)
	{
		if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
		{
			MATH_CopyMatrix(&_pst_Object->pst_Base->pst_Hierarchy->st_LocalMatrix, _pst_Object->pst_GlobalMatrix + 1);
		}

		MATH_MulMatrixMatrix
		(
			OBJ_pst_GetAbsoluteMatrix(_pst_Object),
			&_pst_Object->pst_Base->pst_Hierarchy->st_LocalMatrix,
			OBJ_pst_GetAbsoluteMatrix(_pst_Object->pst_Base->pst_Hierarchy->pst_FatherInit)
		);
	}

	/* Normal copy */
	else if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
	{
		MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix, _pst_Object->pst_GlobalMatrix + 1);
	}

	/* If the Object has a flash matrix, we also update it */
	// Old version : if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Anims))
	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_FlashMatrix))
	{
		if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Hierarchy))
			OBJ_SetFlashMatrix(_pst_Object, OBJ_pst_GetLocalMatrix(_pst_Object));
		else
			OBJ_SetFlashMatrix(_pst_Object, OBJ_pst_GetAbsoluteMatrix(_pst_Object));
	}
}

extern int MATH_VecRotate(MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, float f_Angle);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float Blend(float _f1, float _f2, float _fCoeff)
{
	return(_f1 + (_f2 - _f1) * _fCoeff);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float MATH_f_ZAngle(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Res;
	MATH_tdst_Vector	st_V1, st_V2, st_Axis;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector(&st_V1, _pst_V1);
	MATH_CopyVector(&st_V2, _pst_V2);
	st_V1.z = 0.0f;
	st_V2.z = 0.0f;
	MATH_NormalizeEqualVector(&st_V1);
	MATH_NormalizeEqualVector(&st_V2);

	MATH_CopyVector(&st_Axis, &MATH_gst_BaseVectorK);

	f_Res = MATH_f_VecAngle(&st_V1, &st_V2, &st_Axis);

	if(f_Res > Cf_Pi) f_Res -= Cf_2Pi;
	if(f_Res < -Cf_Pi) f_Res += Cf_2Pi;

	return f_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float MATH_f_XAngle(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_V1, MATH_tdst_Vector *_pst_V2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Alpha1, f_Alpha2, f_Res;
	MATH_tdst_Vector	st_V1, st_V2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector(&st_V1, _pst_V1);
	MATH_CopyVector(&st_V2, _pst_V2);
	st_V1.z = 0.0f;
	st_V2.z = 0.0f;
	MATH_NormalizeEqualVector(&st_V1);
	MATH_NormalizeEqualVector(&st_V2);
	f_Alpha1 = MATH_f_VecAngle(&st_V1, _pst_V1, &MATH_gst_BaseVectorI);
	f_Alpha1 = (_pst_V1->z < 0) ? -fAbs(f_Alpha1) : +fAbs(f_Alpha1);
	f_Alpha2 = MATH_f_VecAngle(&st_V2, _pst_V2, &MATH_gst_BaseVectorI);
	f_Alpha2 = (_pst_V2->z < 0) ? -fAbs(f_Alpha2) : +fAbs(f_Alpha2);

	f_Res = f_Alpha1 - f_Alpha2;

	if(f_Res > Cf_Pi) f_Res -= Cf_2Pi;
	if(f_Res < -Cf_Pi) f_Res += Cf_2Pi;
	if(f_Res > Cf_PiBy2) f_Res = Cf_Pi - f_Res;
	if(f_Res < -Cf_PiBy2) f_Res = -Cf_Pi - f_Res;

	return f_Res;
}

extern float	OBJ_gf_EYEFOLLOW_HeadZMax;
extern float	OBJ_gf_EYEFOLLOW_HeadZMin;
extern float	OBJ_gf_EYEFOLLOW_HeadXMax;
extern float	OBJ_gf_EYEFOLLOW_HeadXMin;
extern float	OBJ_gf_EYEFOLLOW_TorsoZMax;
extern float	OBJ_gf_EYEFOLLOW_TorsoZMin;
extern float	OBJ_gf_EYEFOLLOW_HeadXOffset;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_EyeFollow
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Target,
	float				_f_Blend,
	ULONG				_ul_ActorID,
	BOOL				_b_FlagX,	/* En contre-butees, la tete revient en position droite */
	BOOL				_b_FlagXX,	/* Redefinition des parametres max et min */
	BOOL				_b_FlagXXX, /* Le torse bouge du meme angle que la tete (Arme par ex) */
	BOOL				_b_FlagX4,	/* La tete bouge, pas le torso */
	BOOL				_b_FlagX5,	/* Definition de tous les parametres Min/Max/Offset manuellemenent */
	BOOL				_b_FlagX6   /* Rotation Ventre aussi */
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_HeadGO, *pst_TorsoGO, *pst_PelvisGO, *pst_BellyGO;
	OBJ_tdst_GameObject		*pst_Father;
	MATH_tdst_Matrix		st_Temp;
	MATH_tdst_Vector		st_Actor_Sight;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	BOOL					b_FreeAction;
	BOOL					b_BlendWithAnim;
	BOOL					b_AddZPelvis;
	float					f_10Degrees;
	MATH_tdst_Vector		st_Scale;
	BOOL					b_Scale;

	/* Head */
	MATH_tdst_Vector		st_Head_LastFrameSight;
	MATH_tdst_Vector		st_Head_AnimSight;
	MATH_tdst_Vector		st_Head_AnimBanking;
	MATH_tdst_Vector		st_Head_GoodSight;
	MATH_tdst_Vector		st_Head_NewSight;
	MATH_tdst_Vector		st_Head_PosAfterAnim;
	MATH_tdst_Vector		st_Head_Scale;
	float					f_Head_Z_LastFrameAngle, f_Head_X_LastFrameAngle;
	float					f_Head_Z_GoodAngle, f_Head_X_GoodAngle;
	float					f_Head_Z_Delta, f_Head_X_Delta;
	float					f_Head_Z_Max, f_Head_X_Max;
	float					f_Head_Z_Min, f_Head_X_Min;
	float					f_Head_X_Offset;

	/* Torso */
	MATH_tdst_Vector		st_Torso_LastFrameSight;
	MATH_tdst_Vector		st_Torso_NewSight;
	MATH_tdst_Vector		st_Torso_AnimSight;
	MATH_tdst_Vector		st_Torso_AnimBanking;
	MATH_tdst_Vector		st_Torso_PosAfterAnim;
	MATH_tdst_Vector		st_Torso_Scale;
	float					f_Torso_Z_LastFrameAngle, f_Torso_X_LastFrameAngle;
	float					f_Torso_Z_GoodAngle, f_Torso_X_AnimAngle;
	float					f_Torso_Z_Delta;
	float					f_Torso_Z_Max, f_Torso_Z_Min;
	BOOL					bTorsoAlmostVertical;

	/* Belly */
	MATH_tdst_Vector		st_Belly_LastFrameSight;
	MATH_tdst_Vector		st_Belly_NewSight;
	MATH_tdst_Vector		st_Belly_AnimSight;
	MATH_tdst_Vector		st_Belly_AnimBanking;
	MATH_tdst_Vector		st_Belly_PosAfterAnim;
	MATH_tdst_Vector		st_Belly_Scale;
	float					f_Belly_Z_LastFrameAngle, f_Belly_X_LastFrameAngle;
	float					f_Belly_Z_GoodAngle, f_Belly_X_AnimAngle;
	float					f_Belly_Z_Delta;
	float					f_Belly_Z_Max, f_Belly_Z_Min;


	/* Bassin */
	MATH_tdst_Vector		st_Pelvis_LastFrameSight;
	MATH_tdst_Vector		st_Pelvis_AnimSight;
	float					f_Pelvis_Z_Delta;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	||	(!_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
	||	(!_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects)
	||	(!_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit)
	||	(!_pst_GO->pst_Base->pst_GameObjectAnim->pst_CurrentAction)
	) return;

	pst_HeadGO = ANI_pst_GetObjectByAICanal(_pst_GO, 0);
	pst_TorsoGO = ANI_pst_GetObjectByAICanal(_pst_GO, 2);
	if(_b_FlagX6)	pst_BellyGO = ANI_pst_GetObjectByAICanal(_pst_GO, 3);
	pst_PelvisGO = (OBJ_tdst_GameObject *) (_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects->p_Table)->p_Pointer;

	if(!pst_HeadGO || !pst_TorsoGO || !pst_PelvisGO) return;
	if(_b_FlagX6 && !pst_BellyGO) return;

	b_Scale = FALSE;

	MATH_GetScale(&st_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
	if
	(
		(!fEqWithEpsilon(st_Scale.x, st_Scale.y, (float) 1E-5))
	||	(!fEqWithEpsilon(st_Scale.x, st_Scale.z, (float) 1E-5))
	||	(!fEqWithEpsilon(st_Scale.y, st_Scale.z, (float) 1E-5))
	)
	{
		b_Scale = TRUE;
	}

	b_BlendWithAnim = FALSE;

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	/* Test Action CustomBits to see if EyeFollow is disabled during it (CustomBit 4) */
	/*$off*/
	if(pst_GOAnim->uc_AnimUsed == 1)
	{
		b_FreeAction = !(pst_GOAnim->pst_CurrentAction->ast_ActionItem[pst_GOAnim->uc_CurrentActionItemIndex].uc_CustomBit & 4);
	}
	else
	{
		b_FreeAction = !(pst_GOAnim->pst_CurrentAction->ast_ActionItem[pst_GOAnim->uc_CurrentActionItemIndex].uc_CustomBit & 4) || !(pst_GOAnim->pst_ActionKit->apst_Action[pst_GOAnim->apst_Anim[1]->uw_BlendedAction]->ast_ActionItem[pst_GOAnim->apst_Anim[1]->uc_BlendedActionItem].uc_CustomBit & 4);
	}
	/*$on*/

	if(b_FreeAction)
	{
		if(pst_GOAnim->pst_CurrentAction->ast_ActionItem[pst_GOAnim->uc_CurrentActionItemIndex].uw_DesignFlags & 0x8000)
		{
			pst_GOAnim->uc_Dummy = (pst_GOAnim->uc_Dummy < 16) ? pst_GOAnim->uc_Dummy + (UCHAR) ((TIM_gf_dt / 0.0166666666f) + 0.5f) : 16;
			_f_Blend = ((float) pst_GOAnim->uc_Dummy) / 16.0f;
			if(pst_GOAnim->uc_Dummy >= 16) return;
		}
		else
		{
			/* Flag to avoid blend between EyeFollowed actions and not-EyeFollowed actions. (Camera cut purpose) */
			if(pst_GOAnim->pst_CurrentAction->ast_ActionItem[pst_GOAnim->uc_CurrentActionItemIndex].uw_DesignFlags & 0x1000)
			{
				return;
			}
			else
			{
				pst_GOAnim->uc_Dummy = (pst_GOAnim->uc_Dummy < 6) ? pst_GOAnim->uc_Dummy + 1 : 6;
				_f_Blend = 0.4f;
				if(pst_GOAnim->uc_Dummy >= 6) return;
			}
		}
	}
	else
		pst_GOAnim->uc_Dummy = 0;

	f_Head_Z_Max = 90.0f * Cf_Pi / 180.0f;
	f_Torso_Z_Max = 45.0f * Cf_Pi / 180.0f;
	if(_b_FlagX6) f_Belly_Z_Max = 45.0f * Cf_Pi / 180.0f;

	f_Head_Z_Min = -90.0f * Cf_Pi / 180.0f;
	f_Torso_Z_Min = -45.0f * Cf_Pi / 180.0f;
	if(_b_FlagX6) f_Belly_Z_Min = -45.0f * Cf_Pi / 180.0f;

	f_Head_X_Max = 25.0f * Cf_Pi / 180.0f;
	f_Head_X_Min = -45.0f * Cf_Pi / 180.0f;

	/* Min/Max Values. */
	switch(_ul_ActorID)
	{
	case 0:											/* Sally */
		f_Head_X_Offset = 0.0f * Cf_Pi / 180.0f;	/* 0 ° */
		f_Head_Z_Max = 120.0f * Cf_Pi / 180.0f;
		f_Head_Z_Min = -120.0f * Cf_Pi / 180.0f;
		f_Torso_Z_Max = 60.0f * Cf_Pi / 180.0f;
		f_Torso_Z_Min = -60.0f * Cf_Pi / 180.0f;
		if(_b_FlagX6)
		{
			f_Belly_Z_Max = 60.0f * Cf_Pi / 180.0f;
			f_Belly_Z_Min = -60.0f * Cf_Pi / 180.0f;
		}
		break;

	case 1:											/* Militaire */
		f_Head_X_Offset = 30.0f * Cf_Pi / 180.0f;
		break;

	case 2:											/* Gros Porc */
		f_Head_X_Offset = 35.0f * Cf_Pi / 180.0f;
		f_Head_X_Max = 7.0f * Cf_Pi / 180.0f;
		f_Head_X_Min = -45.0f * Cf_Pi / 180.0f;
		break;

	case 3:											/* Garde */
		f_Head_X_Offset = 21.25f * Cf_Pi / 180.0f;
		break;

	default:
		f_Head_X_Offset = 0.0f * Cf_Pi / 180.0f;	/* 0 ° */
		break;
	}

	if(_b_FlagX5)
	{
		f_Head_Z_Max = (OBJ_gf_EYEFOLLOW_HeadZMax == 1000) ? f_Head_Z_Max : OBJ_gf_EYEFOLLOW_HeadZMax;
		f_Head_Z_Min = (OBJ_gf_EYEFOLLOW_HeadZMin == 1000) ? f_Head_Z_Min : OBJ_gf_EYEFOLLOW_HeadZMin;
		f_Head_X_Max = (OBJ_gf_EYEFOLLOW_HeadXMax == 1000) ? f_Head_X_Max : OBJ_gf_EYEFOLLOW_HeadXMax;
		f_Head_X_Min = (OBJ_gf_EYEFOLLOW_HeadXMin == 1000) ? f_Head_X_Min : OBJ_gf_EYEFOLLOW_HeadXMin;
		f_Torso_Z_Max = (OBJ_gf_EYEFOLLOW_TorsoZMax == 1000) ? f_Torso_Z_Max : OBJ_gf_EYEFOLLOW_TorsoZMax;
		f_Torso_Z_Min = (OBJ_gf_EYEFOLLOW_TorsoZMin == 1000) ? f_Torso_Z_Min : OBJ_gf_EYEFOLLOW_TorsoZMin;
		f_Head_X_Offset = (OBJ_gf_EYEFOLLOW_HeadXOffset == 1000) ? f_Head_X_Offset : OBJ_gf_EYEFOLLOW_HeadXOffset;
	}


	f_Head_X_Max += f_Head_X_Offset;
	f_Head_X_Min += f_Head_X_Offset;

	if(_b_FlagXX)
	{
		f_Head_Z_Max = Cf_Pi;
		f_Torso_Z_Max = Cf_Pi;

		f_Head_Z_Min = -Cf_Pi;
		f_Torso_Z_Min = -Cf_Pi;

		f_Head_X_Max = Cf_Pi;
		f_Head_X_Min = -Cf_Pi;
	}

	f_10Degrees = 10.0f * Cf_Pi / 180.0f;

	/* - ACTOR : sight - */
	OBJ_SightGet(_pst_GO, &st_Actor_Sight);

	MATH_CopyVector(&st_Head_PosAfterAnim, &pst_HeadGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
	MATH_GetScale(&st_Head_Scale, &pst_HeadGO->pst_Base->pst_Hierarchy->st_LocalMatrix);
	MATH_GetScale(&st_Torso_Scale, &pst_TorsoGO->pst_Base->pst_Hierarchy->st_LocalMatrix);
	MATH_CopyVector(&st_Torso_PosAfterAnim, &pst_TorsoGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
	if(_b_FlagX6) 
	{
		MATH_GetScale(&st_Belly_Scale, &pst_BellyGO->pst_Base->pst_Hierarchy->st_LocalMatrix);
		MATH_CopyVector(&st_Belly_PosAfterAnim, &pst_BellyGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
	}

	/* - PELVIS - */
	{
		OBJ_SightGet(pst_PelvisGO, &st_Pelvis_LastFrameSight);
		OBJ_ComputeGlobalWithLocal(pst_PelvisGO, &st_Temp, TRUE);
		MATH_NegVector(&st_Pelvis_AnimSight, MATH_pst_GetYAxis(&st_Temp));

		f_Pelvis_Z_Delta = MATH_f_ZAngle(pst_PelvisGO, &st_Pelvis_LastFrameSight, &st_Pelvis_AnimSight);
	}

	/* - HEAD - */
	{
		OBJ_ComputeGlobalWithLocal(pst_HeadGO, &st_Temp, TRUE);
		MATH_NegVector(&st_Head_AnimSight, MATH_pst_GetYAxis(&st_Temp));
		MATH_CopyVector(&st_Head_AnimBanking, MATH_pst_GetZAxis(&st_Temp));

		OBJ_SightGet(pst_HeadGO, &st_Head_LastFrameSight);

		f_Head_Z_LastFrameAngle = MATH_f_ZAngle(pst_HeadGO, &st_Actor_Sight, &st_Head_LastFrameSight);
		f_Head_X_LastFrameAngle = MATH_f_XAngle(pst_HeadGO, &st_Actor_Sight, &st_Head_LastFrameSight);

		if(b_FreeAction || ((_pst_Target->x == 0.0f) && (_pst_Target->y == 0.0f) && (_pst_Target->z == 0.0f)))
		{
			MATH_CopyVector(&st_Head_GoodSight, &st_Head_AnimSight);

			f_Head_Z_GoodAngle = MATH_f_ZAngle(pst_HeadGO, &st_Actor_Sight, &st_Head_GoodSight);
			f_Head_X_GoodAngle = MATH_f_XAngle(pst_HeadGO, &st_Actor_Sight, &st_Head_GoodSight);
			b_BlendWithAnim = TRUE;
		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix	st_Temp2;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			OBJ_ComputeGlobalWithLocal(pst_HeadGO, &st_Temp2, TRUE);
			MATH_SubVector(&st_Head_GoodSight, _pst_Target, &st_Temp2.T);
			MATH_NormalizeEqualVector(&st_Head_GoodSight);

			f_Head_Z_GoodAngle = MATH_f_ZAngle(pst_HeadGO, &st_Actor_Sight, &st_Head_GoodSight);
			f_Head_X_GoodAngle = MATH_f_XAngle(pst_HeadGO, &st_Actor_Sight, &st_Head_GoodSight);

			if
			(
				(_b_FlagX)
			&&	(
					((f_Head_Z_GoodAngle > f_Head_Z_Max + f_10Degrees))
				||	((f_Head_Z_GoodAngle < f_Head_Z_Min - f_10Degrees))
				)
			)
			{
				f_Head_Z_GoodAngle = MATH_f_ZAngle(pst_HeadGO, &st_Actor_Sight, &st_Head_AnimSight);
				f_Head_X_GoodAngle = MATH_f_XAngle(pst_HeadGO, &st_Actor_Sight, &st_Head_AnimSight);
				b_BlendWithAnim = TRUE;
			}
			else
				f_Head_X_GoodAngle += f_Head_X_Offset;
		}
	}

	/* - TORSO - */
	{
		OBJ_ComputeGlobalWithLocal(pst_TorsoGO, &st_Temp, TRUE);
		MATH_NegVector(&st_Torso_AnimSight, MATH_pst_GetYAxis(&st_Temp));
		MATH_CopyVector(&st_Torso_AnimBanking, MATH_pst_GetZAxis(&st_Temp));

		OBJ_SightGet(pst_TorsoGO, &st_Torso_LastFrameSight);

		{
			MATH_tdst_Vector	st_Temp;

			OBJ_BankingGet(pst_TorsoGO, &st_Temp);
			bTorsoAlmostVertical = fAbs(st_Temp.z) < 0.05f;
		}

		f_Torso_Z_LastFrameAngle = MATH_f_ZAngle(pst_TorsoGO, &st_Actor_Sight, &st_Torso_LastFrameSight);
		f_Torso_X_LastFrameAngle = MATH_f_XAngle(pst_TorsoGO, &st_Actor_Sight, &st_Torso_LastFrameSight);

		f_Torso_X_AnimAngle = MATH_f_XAngle(pst_TorsoGO, &st_Actor_Sight, &st_Torso_AnimSight);

		if(b_BlendWithAnim)
			f_Torso_Z_GoodAngle = MATH_f_ZAngle(pst_TorsoGO, &st_Actor_Sight, &st_Torso_AnimSight);
		else
		{
			if(_b_FlagXXX)
				f_Torso_Z_GoodAngle = f_Head_Z_GoodAngle;
			else
			{
				if(_b_FlagX4)
					f_Torso_Z_GoodAngle = f_Torso_Z_LastFrameAngle;
				else
					f_Torso_Z_GoodAngle = f_Head_Z_GoodAngle / 3.0f;
			}
		}
	}


	/* - BELLY - */
	if(_b_FlagX6)
	{
		OBJ_ComputeGlobalWithLocal(pst_BellyGO, &st_Temp, TRUE);
		MATH_NegVector(&st_Belly_AnimSight, MATH_pst_GetYAxis(&st_Temp));
		MATH_CopyVector(&st_Belly_AnimBanking, MATH_pst_GetZAxis(&st_Temp));

		OBJ_SightGet(pst_BellyGO, &st_Belly_LastFrameSight);

		f_Belly_Z_LastFrameAngle = MATH_f_ZAngle(pst_BellyGO, &st_Actor_Sight, &st_Belly_LastFrameSight);
		f_Belly_X_LastFrameAngle = MATH_f_XAngle(pst_BellyGO, &st_Actor_Sight, &st_Belly_LastFrameSight);

		f_Belly_X_AnimAngle = MATH_f_XAngle(pst_BellyGO, &st_Actor_Sight, &st_Belly_AnimSight);

		if(b_BlendWithAnim)
			f_Belly_Z_GoodAngle = MATH_f_ZAngle(pst_BellyGO, &st_Actor_Sight, &st_Belly_AnimSight);
		else
			f_Belly_Z_GoodAngle = f_Head_Z_GoodAngle / 5.0f;
	}


	/* The Rotation of the Pelvis may not help to reach the wanted sight. */
	b_AddZPelvis =
		(
			(_f_Blend == 1.0f)
		||	(f_Pelvis_Z_Delta && (f_Head_Z_GoodAngle > f_Head_Z_LastFrameAngle) && (f_Pelvis_Z_Delta < 0.0f))
		||	(f_Pelvis_Z_Delta && (f_Head_Z_GoodAngle < f_Head_Z_LastFrameAngle) && (f_Pelvis_Z_Delta > 0.0f))
		) ? FALSE : TRUE;

	/* Gestion des Contre-butees et calcul des angles delta */
	if
	(
		!_b_FlagX
	&&	(
			((fEqWithEpsilon(f_Head_Z_LastFrameAngle, f_Head_Z_Max, 1E-3f)) && (f_Head_Z_GoodAngle < f_Head_Z_Min))
		||	((fEqWithEpsilon(f_Head_Z_LastFrameAngle, f_Head_Z_Min, 1E-3f)) && (f_Head_Z_GoodAngle > f_Head_Z_Max))
		)
	)
	{
		f_Head_Z_Delta = 0.0f;
		f_Head_X_Delta = 0.0f;
		f_Torso_Z_Delta = 0.0f;
		f_Belly_Z_Delta = 0.0f;
	}
	else
	{
		if
		(
			(
				(f_Head_Z_GoodAngle > 0.0f)
			&&	(f_Head_Z_LastFrameAngle < f_Head_Z_GoodAngle)
			&&	((f_Head_Z_LastFrameAngle + f_Pelvis_Z_Delta) > f_Head_Z_GoodAngle)
			)
		||	(
				(f_Head_Z_GoodAngle < 0.0f)
			&&	(f_Head_Z_LastFrameAngle > f_Head_Z_GoodAngle)
			&&	((f_Head_Z_LastFrameAngle + f_Pelvis_Z_Delta) < f_Head_Z_GoodAngle)
			)
		)
		{
			f_Head_Z_Delta = f_Head_Z_GoodAngle - f_Head_Z_LastFrameAngle;
		}
		/*$off*/
		else
		{
			f_Head_Z_Delta = Blend(0.0f, f_Head_Z_GoodAngle - f_Head_Z_LastFrameAngle, _f_Blend) + (b_AddZPelvis ? f_Pelvis_Z_Delta : 0.0f);
			f_Head_Z_Delta = ((f_Head_Z_LastFrameAngle + f_Head_Z_Delta > f_Head_Z_Max) || (f_Head_Z_LastFrameAngle + f_Head_Z_Delta < f_Head_Z_Min))  ?  ((f_Head_Z_LastFrameAngle + f_Head_Z_Delta > f_Head_Z_Max) ? f_Head_Z_Max - f_Head_Z_LastFrameAngle : f_Head_Z_Min - f_Head_Z_LastFrameAngle) : f_Head_Z_Delta;
		}

		f_Head_X_Delta = Blend(0.0f, f_Head_X_GoodAngle - f_Head_X_LastFrameAngle, _f_Blend);
		f_Head_X_Delta = ((f_Head_X_LastFrameAngle + f_Head_X_Delta > f_Head_X_Max) || (f_Head_X_LastFrameAngle + f_Head_X_Delta < f_Head_X_Min)) ? ((f_Head_X_LastFrameAngle + f_Head_X_Delta > f_Head_X_Max) ? f_Head_X_Max - f_Head_X_LastFrameAngle: f_Head_X_Min - f_Head_X_LastFrameAngle) : f_Head_X_Delta;


		f_Torso_Z_Delta = Blend(0.0f, f_Torso_Z_GoodAngle - f_Torso_Z_LastFrameAngle, _f_Blend) + (b_AddZPelvis ? f_Pelvis_Z_Delta : 0.0f);
		f_Torso_Z_Delta = ((f_Torso_Z_LastFrameAngle + f_Torso_Z_Delta > f_Torso_Z_Max) || (f_Torso_Z_LastFrameAngle + f_Torso_Z_Delta < f_Torso_Z_Min)) ? ((f_Torso_Z_LastFrameAngle + f_Torso_Z_Delta > f_Torso_Z_Max) ?	f_Torso_Z_Max - f_Torso_Z_LastFrameAngle : f_Torso_Z_Min - f_Torso_Z_LastFrameAngle) : f_Torso_Z_Delta;

		if(_b_FlagX6)
		{
			f_Belly_Z_Delta = Blend(0.0f, f_Belly_Z_GoodAngle - f_Belly_Z_LastFrameAngle, _f_Blend) + (b_AddZPelvis ? f_Pelvis_Z_Delta : 0.0f);
			f_Belly_Z_Delta = ((f_Belly_Z_LastFrameAngle + f_Belly_Z_Delta > f_Belly_Z_Max) || (f_Belly_Z_LastFrameAngle + f_Belly_Z_Delta < f_Belly_Z_Min)) ? ((f_Belly_Z_LastFrameAngle + f_Belly_Z_Delta > f_Belly_Z_Max) ?	f_Belly_Z_Max - f_Belly_Z_LastFrameAngle : f_Belly_Z_Min - f_Belly_Z_LastFrameAngle) : f_Belly_Z_Delta;
		}

		/*$on*/
	}

	/* - HEAD Rotations - */
	{
		if(!fNulWithEpsilon(f_Head_Z_Delta, 1E-5f))
			MATH_RotateMatrix_AroundGlobalZAxis(pst_HeadGO->pst_GlobalMatrix, f_Head_Z_Delta);
		if(!fNulWithEpsilon(f_Head_X_Delta, 1E-5f))
			MATH_RotateMatrix_AroundLocalXAxis(pst_HeadGO->pst_GlobalMatrix, f_Head_X_Delta);

		OBJ_SightGet(pst_HeadGO, &st_Head_NewSight);
		OBJ_SightGeneralSet(pst_HeadGO, &st_Head_NewSight, &st_Head_AnimBanking);
	}

	/* - TORSO Rotations - */
	{
		if(!bTorsoAlmostVertical && !fNulWithEpsilon(f_Torso_Z_Delta, 1E-5f))
			MATH_RotateMatrix_AroundGlobalZAxis(pst_TorsoGO->pst_GlobalMatrix, f_Torso_Z_Delta);

		if(!bTorsoAlmostVertical)
		{
			MATH_RotateMatrix_AroundLocalXAxis
			(
				pst_TorsoGO->pst_GlobalMatrix,
				f_Torso_X_AnimAngle - f_Torso_X_LastFrameAngle
			);
		}

		if(!bTorsoAlmostVertical)
		{
			OBJ_SightGet(pst_TorsoGO, &st_Torso_NewSight);
			OBJ_SightGeneralSet(pst_TorsoGO, &st_Torso_NewSight, &st_Torso_AnimBanking);
		}
	}

	/* - BELLY Rotations - */
	if(_b_FlagX6)
	{
		if(!fNulWithEpsilon(f_Belly_Z_Delta, 1E-5f))
			MATH_RotateMatrix_AroundGlobalZAxis(pst_BellyGO->pst_GlobalMatrix, f_Belly_Z_Delta);

		MATH_RotateMatrix_AroundLocalXAxis
		(
			pst_BellyGO->pst_GlobalMatrix,
			f_Belly_X_AnimAngle - f_Belly_X_LastFrameAngle
		);

		OBJ_SightGet(pst_BellyGO, &st_Belly_NewSight);
		OBJ_SightGeneralSet(pst_BellyGO, &st_Belly_NewSight, &st_Belly_AnimBanking);
	}


	/* BELLY */
	if(_b_FlagX6)
	{
		pst_Father = OBJ_pst_GetFather(pst_BellyGO);
		OBJ_ComputeGlobalWithLocal(pst_Father, pst_Father->pst_GlobalMatrix, TRUE);
		OBJ_ComputeLocalWhenHie(pst_BellyGO);
		MATH_CopyVector(&pst_BellyGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_Belly_PosAfterAnim);
	}

	/* TORSO */
	pst_Father = OBJ_pst_GetFather(pst_TorsoGO);
	OBJ_ComputeGlobalWithLocal(pst_Father, pst_Father->pst_GlobalMatrix, TRUE);
	if(bTorsoAlmostVertical)
		OBJ_ComputeGlobalWhenHie(pst_TorsoGO);
	else
		OBJ_ComputeLocalWhenHie(pst_TorsoGO);
	MATH_CopyVector(&pst_TorsoGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_Torso_PosAfterAnim);

	/* HEAD */
	pst_Father = OBJ_pst_GetFather(pst_HeadGO);
	OBJ_ComputeGlobalWithLocal(pst_Father, pst_Father->pst_GlobalMatrix, TRUE);
	OBJ_ComputeLocalWhenHie(pst_HeadGO);

	MATH_SetScale(OBJ_pst_GetLocalMatrix(pst_HeadGO), &st_Head_Scale);
	MATH_SetScale(OBJ_pst_GetLocalMatrix(pst_TorsoGO), &st_Torso_Scale);
	if(_b_FlagX6) MATH_SetScale(OBJ_pst_GetLocalMatrix(pst_BellyGO), &st_Belly_Scale);

	MATH_CopyVector(&pst_HeadGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_Head_PosAfterAnim);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

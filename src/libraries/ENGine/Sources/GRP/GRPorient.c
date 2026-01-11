/*$T GRPorient.c GC! 1.081 05/22/00 16:33:02 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Main functions of the GRP module */

/*
 * OBJ_SetAbsolutePosition £
 * OBJ_pst_GetAbsolutePosition £
 * OBJ_pst_GetAbsoluteMatrix £
 * OBJ_MakeAbsoluteMatrix £
 * OBJ_SetAbsoluteMatrix £
 */
#include "Precomp.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "BASe/MEMory/MEM.h"

/* #include "ENGine/Sources/WORld/WORstruct.h" */
#include "TABles/TABles.h"
#include "ENGine/Sources/WORld/WORaccess.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
    Aim:    Return a pointer to the group structure of an object (if allocated)
 =======================================================================================================================
 */
_inline_ OBJ_tdst_Group *OBJ_pst_GetGroup(OBJ_tdst_GameObject *_pst_Object)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Extended	*pst_Extended;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Extended = _pst_Object->pst_Extended;
	if(pst_Extended)
		return(pst_Extended->pst_Group);
	else
		return NULL;
}

/*
 =======================================================================================================================
    Aim:    Sets the center of BV to the absolute position given, and change position of all objects to keep there
            relative position to BV center
 =======================================================================================================================
 */
void GRP_SetAbsolutePosition(OBJ_tdst_GameObject *_pst_GroupObj, MATH_tdst_Vector *_pst_NewPos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_DeltaPos;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_Group		*pst_Group;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_DeltaPos, _pst_NewPos, OBJ_pst_GetAbsolutePosition(_pst_GroupObj));

	/* Loop thru objects of the group to change the position of each of them */
	pst_Group = OBJ_pst_GetGroup(_pst_GroupObj);
	if(MEM_b_CheckPointer(pst_Group))
	{
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
		for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(!TAB_b_IsAHole(pst_GO))
			{
				if(pst_GO != _pst_GroupObj) OBJ_AddAbsoluteVector(pst_GO, &st_DeltaPos);
			}
		}
	}
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Sets the initial position of all objects of the group. First object is took as reference.
 =======================================================================================================================
 */
void GRP_SetRelativePosition(OBJ_tdst_Group *pst_Group, MATH_tdst_Vector *_pst_NewPos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_MemPos, st_Tmp;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO, *pst_First;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OBJ_OrderGroupByHie(pst_Group);
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
	pst_First = NULL;
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(!TAB_b_IsAHole(pst_GO))
		{
			if(!pst_First)
			{
				pst_First = pst_GO;
				MATH_CopyVector(&st_MemPos, OBJ_pst_GetAbsolutePosition(pst_GO));
				OBJ_SetAbsolutePosition(pst_GO, _pst_NewPos);
				OBJ_ComputeLocalWhenHie(pst_GO);
				OBJ_SetInitialAbsoluteMatrix(pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO));
			}
			else
			{
				MATH_SubVector(&st_Tmp, OBJ_pst_GetAbsolutePosition(pst_GO), &st_MemPos);
				MATH_AddVector(&st_Tmp, &st_Tmp, OBJ_pst_GetAbsolutePosition(pst_First));
				OBJ_SetAbsolutePosition(pst_GO, &st_Tmp);
				OBJ_ComputeLocalWhenHie(pst_GO);
				OBJ_SetInitialAbsoluteMatrix(pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO));
			}
		}
	}
}

#endif

/*
 =======================================================================================================================
    Aim:    Sets the initial position of a group (and of all objects of the group)
 =======================================================================================================================
 */
void GRP_SetAbsoluteInitialPosition(OBJ_tdst_GameObject *_pst_GroupObj, MATH_tdst_Vector *_pst_NewPos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_DeltaPos;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_Group		*pst_Group;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_DeltaPos, _pst_NewPos, OBJ_pst_GetInitialAbsolutePosition(_pst_GroupObj));

	/* Loop thru objects of the group to change the position of each of them */
	pst_Group = OBJ_pst_GetGroup(_pst_GroupObj);
	if(MEM_b_CheckPointer(pst_Group))
	{
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
		for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(!TAB_b_IsAHole(pst_GO))
			{
				if(pst_GO != _pst_GroupObj) OBJ_AddAbsoluteInitalVector(pst_GO, &st_DeltaPos);
			}
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Add a vector to the position of a group
 =======================================================================================================================
 */
void GRP_AddAbsoluteVector(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Vector)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_Group		*pst_Group;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Loop thru objects of the group to change the position of each of them */
	pst_Group = OBJ_pst_GetGroup(_pst_Object);
	if(MEM_b_CheckPointer(pst_Group))
	{
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
		for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(!TAB_b_IsAHole(pst_GO))
			{
				if(pst_GO != _pst_Object) OBJ_AddAbsoluteVector(pst_GO, _pst_Vector);
			}
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Add a vector to the position of a group
 =======================================================================================================================
 */
void GRP_AddAbsoluteInitialVector(OBJ_tdst_GameObject *_pst_Object, MATH_tdst_Vector *_pst_Vector)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_Group		*pst_Group;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Loop thru objects of the group to change the position of each of them */
	pst_Group = OBJ_pst_GetGroup(_pst_Object);
	if(MEM_b_CheckPointer(pst_Group))
	{
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
		for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(!TAB_b_IsAHole(pst_GO))
			{
				if(pst_GO != _pst_Object) OBJ_AddAbsoluteInitalVector(pst_GO, _pst_Vector);
			}
		}

		/* We change the BV of the objects using this group */
	}
}

/*
 =======================================================================================================================
    Aim:    Sets the absolute matrix of a group, and update the matrix of all the objects of the group
 =======================================================================================================================
 */
void GRP_SetAbsoluteMatrix(OBJ_tdst_GameObject *_pst_GroupObj, MATH_tdst_Matrix *_pst_Matrix)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_DeltaPos;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_Group		*pst_Group;
	MATH_tdst_Matrix	st_RotMatrix;
	MATH_tdst_Matrix	st_InvInitMatrix;
	MATH_tdst_Vector	*pst_Pivot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * Choose pivot to rotate around (for the moment absolute position of the group
	 * master)
	 */
	pst_Pivot = OBJ_pst_GetAbsolutePosition(_pst_GroupObj);

	/* Calculate translation to apply */
	MATH_SubVector(&st_DeltaPos, MATH_pst_GetTranslation(_pst_Matrix), OBJ_pst_GetAbsolutePosition(_pst_GroupObj));

	/* Precalculate rotation matrix (same for each member of the group) */
	MATH_Transp33MatrixWithoutBuffer(&st_InvInitMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GroupObj));
	MATH_Mul33MatrixMatrix(&st_RotMatrix, _pst_Matrix, &st_InvInitMatrix, 1);

	/* Set the translation */
	MATH_SetTranslation(&st_RotMatrix, &st_DeltaPos);

	/* Loop thru objects of the group to change the matrix of each of them */
	pst_Group = OBJ_pst_GetGroup(_pst_GroupObj);
	if(MEM_b_CheckPointer(pst_Group))
	{
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
		for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(!TAB_b_IsAHole(pst_GO))
			{
				if(pst_GO != _pst_GroupObj)
				{
					OBJ_TransformAbsoluteMatrix(pst_GO, &st_RotMatrix, pst_Pivot);
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRP_SetAbsoluteInitialMatrix(OBJ_tdst_GameObject *_pst_GroupObj, MATH_tdst_Matrix *_pst_Matrix)
{
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

/*$T INTinit.c GC! 1.081 06/19/00 10:43:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "INTstruct.h"
#include "WORld/WORstruct.h"
#include "INTaccess.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"
#include "OBJects/OBJconst.h"
#include "OBJects/OBJorient.h"
#include "OBJects/OBJBoundingVolume.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

extern ULONG		LOA_ul_FileTypeSize[40];

/*
 =======================================================================================================================
    Aim:    This function go thru the IndexToObj Table and finds the first Index that is free to give it to a new
            object. This index is now the SnP Reference of the objbect.
 =======================================================================================================================
 */
USHORT INT_SnP_GetFirstFreeIndex(INT_tdst_SnP *_pst_SnP)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject **ppst_First, **ppst_Obj, **ppst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ppst_First = _pst_SnP->apst_IndexToObj;
	ppst_Obj = ppst_First;
	ppst_Last = ppst_First + INT_Cul_MaxObjects - 1;
	while((*ppst_Obj) && (ppst_Obj < ppst_Last)) ppst_Obj++;

	if(!(*ppst_Obj))
		return(ppst_Obj - ppst_First);
	else
		return 0xFFFF;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
INT_tdst_AxisTable *INT_SnP_pstCreateAxisTable(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_AxisTable	*pst_NewTable;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_NewTable = (INT_tdst_AxisTable *) MEM_p_VMAlloc(sizeof(INT_tdst_AxisTable));
	LOA_ul_FileTypeSize[38] += sizeof(INT_tdst_AxisTable);
	L_memset(pst_NewTable, 0, sizeof(INT_tdst_AxisTable));
	pst_NewTable->pst_Nodes = (INT_tdst_AxisNode *) MEM_p_VMAlloc((INT_Cul_MaxNodes) * sizeof(INT_tdst_AxisNode));
	LOA_ul_FileTypeSize[38] += (INT_Cul_MaxNodes) * sizeof(INT_tdst_AxisNode);
	L_memset(pst_NewTable->pst_Nodes, 0, sizeof(INT_tdst_AxisNode));

	return pst_NewTable;
}

/*
 =======================================================================================================================
    Aim:    When a Reference is given to a new object, we must deal with the fact that it could have already been the
            Ref of another one that has been erased. So, we set to FALSE all the Axis Tables flags concerning this Ref.
 =======================================================================================================================
 */
void INT_SnP_ResetAllReferenceFlags(INT_tdst_SnP *_pst_SnP, USHORT us_Ref)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT	us_CurrentRef;
	LONG	ul_FlagIndexInLong, ul_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(us_CurrentRef = 0; us_CurrentRef < INT_Cul_MaxObjects; us_CurrentRef++)
	{
		if(us_Ref == us_CurrentRef) continue;
		
		INT_SnP_FullSetFlag
		(
			(ULONG *) _pst_SnP->apst_AxisTable[INT_Cul_AxisX]->al_Flags,
			us_Ref,
			us_CurrentRef,
			INT_Cul_MaxObjects,
			(ULONG *) &ul_FlagIndexInLong,
			(ULONG *) &ul_Offset,
			FALSE
		);

		INT_SnP_SetFlag
		(
			(ULONG *) _pst_SnP->apst_AxisTable[INT_Cul_AxisY]->al_Flags,
			ul_FlagIndexInLong,
			ul_Offset,
			FALSE
		);

		INT_SnP_SetFlag
		(
			(ULONG *) _pst_SnP->apst_AxisTable[INT_Cul_AxisZ]->al_Flags,
			ul_FlagIndexInLong,
			ul_Offset,
			FALSE
		);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_AddNode
(
	INT_tdst_SnP		*_pst_SnP,
	OBJ_tdst_GameObject *_pst_Obj,
	MATH_tdst_Vector	*_pst_Point,
	UCHAR				_uc_Flags
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_AxisNode	*pst_CurrentNode;
	short				us_Axis, us_NbElems;
	float				f_Radius;
	MATH_tdst_Vector	*pst_T, *pst_OCS_Center, st_GCS_Center;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Radius = 0.0f;

	us_Axis = INT_Cul_AxisX;

	for(us_Axis = INT_Cul_AxisX; us_Axis <= INT_Cul_AxisZ; us_Axis++)
	{
		us_NbElems = _pst_SnP->us_NbElems;
		pst_CurrentNode = &_pst_SnP->apst_AxisTable[us_Axis]->pst_Nodes[us_NbElems];
		pst_CurrentNode->pst_Obj = _pst_Obj;

		if((us_Axis == INT_Cul_AxisX) && (_uc_Flags & INT_Cul_StartPoint))
		{
			_pst_Obj->us_SnP_Ref = INT_SnP_GetFirstFreeIndex(_pst_SnP);
			INT_SnP_ResetAllReferenceFlags(_pst_SnP, _pst_Obj->us_SnP_Ref);
			_pst_SnP->apst_IndexToObj[_pst_Obj->us_SnP_Ref] = _pst_Obj;
		}

		pst_CurrentNode->us_Ref = _pst_Obj->us_SnP_Ref;

		/* We set the Node Flags. */
		pst_CurrentNode->uc_Flags = _uc_Flags;

		/* For sphere, we get the Center. */
		if(_uc_Flags & INT_Cul_Sphere)
		{
			pst_OCS_Center = _pst_Point;
			f_Radius = OBJ_f_BV_GetRadius(_pst_Obj->pst_BV);

			/* The Scale is already in the BV's structure -> MATH_TransformVertexNoScale */
			MATH_TransformVertexNoScale(&st_GCS_Center, OBJ_pst_GetAbsoluteMatrix(_pst_Obj), pst_OCS_Center);
			pst_CurrentNode->pst_Center = pst_OCS_Center;
		}

		pst_T = OBJ_pst_GetAbsolutePosition(_pst_Obj);
		switch(us_Axis)
		{
		case INT_Cul_AxisX:
			if(_uc_Flags & INT_Cul_Sphere)
			{
				if(_uc_Flags & INT_Cul_StartPoint)
					pst_CurrentNode->f_Val = fSub(st_GCS_Center.x, f_Radius);
				else
					pst_CurrentNode->f_Val = fAdd(st_GCS_Center.x, f_Radius);
			}
			else
			{
				pst_CurrentNode->pf_BVCoord = &_pst_Point->x;
				pst_CurrentNode->f_Val = fAdd(_pst_Point->x, pst_T->x);
			}
			break;

		case INT_Cul_AxisY:
			if(_uc_Flags & INT_Cul_Sphere)
			{
				if(_uc_Flags & INT_Cul_StartPoint)
					pst_CurrentNode->f_Val = fSub(st_GCS_Center.y, f_Radius);
				else
					pst_CurrentNode->f_Val = fAdd(st_GCS_Center.y, f_Radius);
			}
			else
			{
				pst_CurrentNode->pf_BVCoord = &_pst_Point->y;
				pst_CurrentNode->f_Val = fAdd(_pst_Point->y, pst_T->y);
			}
			break;

		case INT_Cul_AxisZ:
			if(_uc_Flags & INT_Cul_Sphere)
			{
				if(_uc_Flags & INT_Cul_StartPoint)
					pst_CurrentNode->f_Val = fSub(st_GCS_Center.z, f_Radius);
				else
					pst_CurrentNode->f_Val = fAdd(st_GCS_Center.z, f_Radius);
			}
			else
			{
				pst_CurrentNode->pf_BVCoord = &_pst_Point->z;
				pst_CurrentNode->f_Val = fAdd(_pst_Point->z, pst_T->z);
			}
			break;
		}
	}

	_pst_SnP->us_NbElems++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_AddBox
(
	INT_tdst_SnP		*_pst_SnP,
	OBJ_tdst_GameObject *_pst_Obj,
	MATH_tdst_Vector	*_pst_GMin,
	MATH_tdst_Vector	*_pst_GMax
)
{
	INT_SnP_AddNode(_pst_SnP, _pst_Obj, _pst_GMin, INT_Cul_StartPoint | INT_Cul_AABBox);
	INT_SnP_AddNode(_pst_SnP, _pst_Obj, _pst_GMax, INT_Cul_EndPoint | INT_Cul_AABBox);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_AddSphere
(
	INT_tdst_SnP		*_pst_SnP,
	OBJ_tdst_GameObject *_pst_Obj,
	MATH_tdst_Vector	*_pst_Center,
	float				f_Radius
)
{
	INT_SnP_AddNode(_pst_SnP, _pst_Obj, OBJ_pst_BV_GetCenter(_pst_Obj->pst_BV), INT_Cul_StartPoint | INT_Cul_Sphere);
	INT_SnP_AddNode(_pst_SnP, _pst_Obj, OBJ_pst_BV_GetCenter(_pst_Obj->pst_BV), INT_Cul_EndPoint | INT_Cul_Sphere);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_Alloc(WOR_tdst_World *_pst_World)
{
	_pst_World->pst_SnP = (INT_tdst_SnP *) MEM_p_Alloc(sizeof(INT_tdst_SnP));
	LOA_ul_FileTypeSize[38] += sizeof(INT_tdst_SnP);
	L_memset(_pst_World->pst_SnP, 0, sizeof(INT_tdst_SnP));
	_pst_World->pst_SnP->pst_Manager = (INT_tdst_SnP_Manager *) MEM_p_Alloc(sizeof(INT_tdst_SnP_Manager));
	LOA_ul_FileTypeSize[38] += sizeof(INT_tdst_SnP_Manager);
	L_memset(_pst_World->pst_SnP->pst_Manager, 0, sizeof(INT_tdst_SnP_Manager));
	_pst_World->pst_SnP->apst_AxisTable[INT_Cul_AxisX] = INT_SnP_pstCreateAxisTable();
	_pst_World->pst_SnP->apst_AxisTable[INT_Cul_AxisY] = INT_SnP_pstCreateAxisTable();
	_pst_World->pst_SnP->apst_AxisTable[INT_Cul_AxisZ] = INT_SnP_pstCreateAxisTable();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_DesAlloc(WOR_tdst_World *_pst_World)
{
	if(!_pst_World->pst_SnP) return;

	/* Axis Tables Desallocation. */
	MEM_Free(_pst_World->pst_SnP->apst_AxisTable[INT_Cul_AxisX]->pst_Nodes);
	MEM_Free(_pst_World->pst_SnP->apst_AxisTable[INT_Cul_AxisX]);
	MEM_Free(_pst_World->pst_SnP->apst_AxisTable[INT_Cul_AxisY]->pst_Nodes);
	MEM_Free(_pst_World->pst_SnP->apst_AxisTable[INT_Cul_AxisY]);
	MEM_Free(_pst_World->pst_SnP->apst_AxisTable[INT_Cul_AxisZ]->pst_Nodes);
	MEM_Free(_pst_World->pst_SnP->apst_AxisTable[INT_Cul_AxisZ]);

	/* Manager desallocation. */
	MEM_Free(_pst_World->pst_SnP->pst_Manager);

	/* SnP Final desallocation. */
	MEM_Free(_pst_World->pst_SnP);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_Reinit(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_SnP	*pst_SnP;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_SnP = _pst_World->pst_SnP;

	/* If the SnP structure hasn't been allocated yet, we return. */
	if(!pst_SnP) return;

	/* Number of game objects included in the SnP structure set to 0. */
	pst_SnP->us_NbElems = 0;

	/*
	 * The table that indicates the object that corresponds to an index is
	 * reinitialized. This operation is important for the INT_SnP_Refresh that goes
	 * thru this table in order to find the objects that are in the SnP but no LONGer
	 * in the active table.
	 */
	L_memset(pst_SnP->apst_IndexToObj, 0, INT_Cul_MaxObjects * sizeof(OBJ_tdst_GameObject *));

	/* We reset all the Axis Tables flags. */
	L_memset(pst_SnP->apst_AxisTable[INT_Cul_AxisX]->al_Flags, 0, INT_Cul_MaxFlags * sizeof(LONG));
	L_memset(pst_SnP->apst_AxisTable[INT_Cul_AxisY]->al_Flags, 0, INT_Cul_MaxFlags * sizeof(LONG));
	L_memset(pst_SnP->apst_AxisTable[INT_Cul_AxisZ]->al_Flags, 0, INT_Cul_MaxFlags * sizeof(LONG));

	/* Force the reconstruction of the SnP after a Reinit. */
	_pst_World->b_ForceBVRefresh = TRUE;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

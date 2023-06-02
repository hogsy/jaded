/*$T INTSnP.c GC! 1.081 09/24/01 15:15:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"

#include "INTinit.h"

#include "OBJects/OBJconst.h"
#include "OBJects/OBJorient.h"
#include "OBJects/OBJBoundingVolume.h"
#include "COLlision/COLstruct.h"
#include "COLlision/COLconst.h"
#include "WORld/WORstruct.h"

#ifdef ACTIVE_EDITORS
#include "SDK/Sources/LINks/LINKmsg.h"
#include "COLlision/COLaccess.h"
#include "float.h"
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
    Aim:    Add GameObj.A (B) to the GameObj.B (A) Collision List .
 =======================================================================================================================
 */
void INT_SnP_AddOverlap(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_DetectionList	*pst_DL;
	OBJ_tdst_GameObject		**ppst_LastObj;
	OBJ_tdst_GameObject		**ppst_CollidedObject;
	ULONG					ul_NbCollidedObjects;
#ifdef ACTIVE_EDITORS
	char					*sz_Name;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	sz_Name = _pst_A->sz_Name;
	sz_Name = _pst_B->sz_Name;
#endif
	/* If A has an extra collision structure */
	if(OBJ_b_TestStatusFlag(_pst_A, OBJ_C_StatusFlag_Detection))
	{
		if((!_pst_A->pst_Extended)) return;
		if((!_pst_A->pst_Extended->pst_Col)) return;
		if(!((COL_tdst_Base *) _pst_A->pst_Extended->pst_Col)->pst_List) return;

		pst_DL = ((COL_tdst_Base *) _pst_A->pst_Extended->pst_Col)->pst_List;
		ppst_CollidedObject = pst_DL->dpst_CollidedObject;
		ul_NbCollidedObjects = pst_DL->ul_NbCollidedObjects;

		/* If A has already a or some collided object(s), we check if it is not B. */
		if(ul_NbCollidedObjects)
		{
			ppst_LastObj = ppst_CollidedObject + ul_NbCollidedObjects - 1;
			while((*ppst_CollidedObject != _pst_B) && (ppst_CollidedObject != ppst_LastObj)) ppst_CollidedObject++;

			/*
			 * If B is not in the A Collision List, we want to add it but first we check the
			 * number of collided objects. There can be a Realloc
			 */
			if(*ppst_CollidedObject != _pst_B)
			{
				/* Is a ReAlloc needed ? */
				if(ul_NbCollidedObjects >= COL_Cul_MaxDefaultCollidedObjects)
				{
					pst_DL->dpst_CollidedObject = (OBJ_tdst_GameObject **) MEM_p_Realloc
						(
							pst_DL->dpst_CollidedObject,
							(ul_NbCollidedObjects + 1) * sizeof(OBJ_tdst_GameObject *)
						);
				}

				/* We add the new collided object into A Collision List */
				pst_DL->dpst_CollidedObject[pst_DL->ul_NbCollidedObjects++] = _pst_B;
			}
		}
		else
			pst_DL->dpst_CollidedObject[pst_DL->ul_NbCollidedObjects++] = _pst_B;
	}

	/* If B has an extra collision structure */
	if(OBJ_b_TestStatusFlag(_pst_B, OBJ_C_StatusFlag_Detection))
	{
		if((!_pst_B->pst_Extended)) return;
		if((!_pst_B->pst_Extended->pst_Col)) return;
		if(!((COL_tdst_Base *) _pst_B->pst_Extended->pst_Col)->pst_List) return;

		pst_DL = ((COL_tdst_Base *) _pst_B->pst_Extended->pst_Col)->pst_List;
		ppst_CollidedObject = pst_DL->dpst_CollidedObject;
		ul_NbCollidedObjects = pst_DL->ul_NbCollidedObjects;

		/* If B has already a or some collided object(s), we check if it is not A. */
		if(ul_NbCollidedObjects)
		{
			ppst_LastObj = ppst_CollidedObject + ul_NbCollidedObjects - 1;
			while((*ppst_CollidedObject != _pst_A) && (ppst_CollidedObject != ppst_LastObj)) ppst_CollidedObject++;

			/*
			 * If A is not in the B Collision List, we want to add it but first we check the
			 * number of collided objects. There can be a Realloc
			 */
			if(*ppst_CollidedObject != _pst_A)
			{
				/* Is a ReAlloc needed ? */
				if(ul_NbCollidedObjects >= COL_Cul_MaxDefaultCollidedObjects)
				{
					pst_DL->dpst_CollidedObject = (OBJ_tdst_GameObject **) MEM_p_Realloc
						(
							pst_DL->dpst_CollidedObject,
							(ul_NbCollidedObjects + 1) * sizeof(OBJ_tdst_GameObject *)
						);
				}

				/* We add the new collided object into A Collision List */
				pst_DL->dpst_CollidedObject[pst_DL->ul_NbCollidedObjects++] = _pst_A;
			}
		}
		else
			pst_DL->dpst_CollidedObject[pst_DL->ul_NbCollidedObjects++] = _pst_A;
	}
}

/*
 =======================================================================================================================
    Aim Remove GameObj.A (B) to the GameObj.B (A) Collision List.. If A (B) is not in the B (A) Collision list, we also
    return.wihtout any change
 =======================================================================================================================
 */
void INT_SnP_RemoveOverlap(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		**ppst_CollidedObject;
	COL_tdst_DetectionList	*pst_DL;
	OBJ_tdst_GameObject		**ppst_LastObj;
	ULONG					ul_NbCollidedObjects;
#ifdef ACTIVE_EDITORS
	char					*sz_Name;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	ERR_X_Assert(_pst_A);
	sz_Name = _pst_A->sz_Name;
	ERR_X_Assert(_pst_B);
	sz_Name = _pst_B->sz_Name;
#endif
	if(OBJ_b_TestStatusFlag(_pst_A, OBJ_C_StatusFlag_Detection))
	{
		/* If the COL structure is not allocated for A, we stop there. */
		if((!_pst_A->pst_Extended)) return;
		if((!_pst_A->pst_Extended->pst_Col)) return;
		if(!((COL_tdst_Base *) _pst_A->pst_Extended->pst_Col)->pst_List) return;

		/* Remove B from the A CollidedObjects List */
		pst_DL = ((COL_tdst_Base *) _pst_A->pst_Extended->pst_Col)->pst_List;
		ppst_CollidedObject = pst_DL->dpst_CollidedObject;
		ul_NbCollidedObjects = pst_DL->ul_NbCollidedObjects;
		if(ul_NbCollidedObjects)
		{
			ppst_LastObj = ppst_CollidedObject + ul_NbCollidedObjects - 1;
			while((*ppst_CollidedObject != _pst_B) && (ppst_CollidedObject != ppst_LastObj)) ppst_CollidedObject++;
			if(*ppst_CollidedObject == _pst_B)
			{
				L_memcpy(ppst_CollidedObject, ppst_CollidedObject + 1, (ppst_LastObj - ppst_CollidedObject) << 2);
				pst_DL->ul_NbCollidedObjects--;
			}
		}
	}

	if(OBJ_b_TestStatusFlag(_pst_B, OBJ_C_StatusFlag_Detection))
	{
		/* If the COL structure is not allocated for B, we stop there. */
		if((!_pst_B->pst_Extended)) return;
		if((!_pst_B->pst_Extended->pst_Col)) return;

		/* Remove A from the B CollidedObjects List */
		pst_DL = ((COL_tdst_Base *) _pst_B->pst_Extended->pst_Col)->pst_List;
		if(!pst_DL) return;
		ppst_CollidedObject = pst_DL->dpst_CollidedObject;
		ul_NbCollidedObjects = pst_DL->ul_NbCollidedObjects;
		if(ul_NbCollidedObjects)
		{
			ppst_LastObj = ppst_CollidedObject + ul_NbCollidedObjects - 1;
			while((*ppst_CollidedObject != _pst_A) && (ppst_CollidedObject != ppst_LastObj)) ppst_CollidedObject++;
			if(*ppst_CollidedObject == _pst_A)
			{
				L_memcpy(ppst_CollidedObject, ppst_CollidedObject + 1, (ppst_LastObj - ppst_CollidedObject) << 2);
				pst_DL->ul_NbCollidedObjects--;
			}
		}
	}
}

/*
 =======================================================================================================================
    Aim:    The insertion Sort caused an inversion in one of the three Axis Tables. We update this Table's Flags. If a
            new overlap has occured due to this inversion, we check if there is not a real BV collision between the 2
            objects that are concerned. If it is the case, we update the collision list of the two objects
 =======================================================================================================================
 */
void INT_SnP_ComputeInversion
(
	INT_tdst_SnP	*_pst_SnP,
	LONG			_ul_Axis,
	LONG			_ul_RefA,
	LONG			_ul_RefB,
	BOOL			_b_AIsS,
	BOOL			_b_BIsS
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT	us_HalfElems;
	BOOL	b_IsAlreadySet;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	us_HalfElems = _pst_SnP->us_NbElems >> 1;

	if((_b_BIsS) && !(_b_AIsS))
	{
		b_IsAlreadySet = _pst_SnP->apst_AxisTable[ _ul_Axis ]->flags.Set( _ul_RefA, _ul_RefB, true );

		if (
			!b_IsAlreadySet
			&& _pst_SnP->apst_AxisTable[ ( _ul_Axis + 1 ) % 3 ]->flags.Get( _ul_RefA, _ul_RefB )
			&& _pst_SnP->apst_AxisTable[ ( _ul_Axis + 2 ) % 3 ]->flags.Get( _ul_RefA, _ul_RefB )
			)
		{
			INT_SnP_AddOverlap(_pst_SnP->apst_IndexToObj[_ul_RefA], _pst_SnP->apst_IndexToObj[_ul_RefB]);
		}
	}
	else if(!(_b_BIsS) && (_b_AIsS))
	{
		b_IsAlreadySet = _pst_SnP->apst_AxisTable[ _ul_Axis ]->flags.Set( _ul_RefA, _ul_RefB, false );
		if(b_IsAlreadySet)
			INT_SnP_RemoveOverlap(_pst_SnP->apst_IndexToObj[_ul_RefA], _pst_SnP->apst_IndexToObj[_ul_RefB]);
	}
}

/*
 =======================================================================================================================
    Aim:    Updates the SnP Node internal value if needed
 =======================================================================================================================
 */
_inline_ void INT_SnP_UpdateInternalValue(INT_tdst_AxisNode *_pst_Node, LONG _l_Axis)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_T;
	float				TCoord, CCoord;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Node->uc_Flags & INT_Cul_AABBox)
	{
		pst_T = OBJ_pst_GetAbsolutePosition(_pst_Node->pst_Obj);
		TCoord = (_l_Axis == INT_Cul_AxisX) ? pst_T->x : (_l_Axis == INT_Cul_AxisY) ? pst_T->y : pst_T->z;
		_pst_Node->f_Val = *(_pst_Node->pf_BVCoord) + TCoord;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_GCS_Center;
		float				f_Radius;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		f_Radius = OBJ_f_BV_GetRadius(_pst_Node->pst_Obj->pst_BV);

		/* The Scale is already in the BV's structure -> MATH_TransformVertexNoScale */
		MATH_TransformVertexNoScale
		(
			&st_GCS_Center,
			OBJ_pst_GetAbsoluteMatrix(_pst_Node->pst_Obj),
			_pst_Node->pst_Center
		);

		CCoord = (_l_Axis == INT_Cul_AxisX) ? st_GCS_Center.x : (_l_Axis == INT_Cul_AxisY) ? st_GCS_Center.y : st_GCS_Center.z;
		_pst_Node->f_Val = (_pst_Node->uc_Flags & INT_Cul_StartPoint) ? CCoord - f_Radius : CCoord + f_Radius;
	}
}

#ifdef ACTIVE_EDITORS
extern ULONG	COL_ul_SnP_Permutations;
#endif

/*
 =======================================================================================================================
    Aim:    Insertion sort is used to update the three Axis Tables. This Algo. is launched each time the Refresh is
            performed
 =======================================================================================================================
 */
void INT_SnP_InsertionSort(INT_tdst_SnP *_pst_SnP, ULONG _ul_UpdateMode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_AxisNode	*pst_FirstNode;
	INT_tdst_AxisNode	*pst_LastNode;
	INT_tdst_AxisNode	*pst_NodeUp;
	INT_tdst_AxisNode	*pst_NodeDown;
	INT_tdst_AxisNode	st_NodeUp;
	LONG				l_Axis, l_Start, l_End;
	BOOL				b_AlreadySorted;
	USHORT				us_NbElems;
	float				f_UpCoord;
	float				f_DownCoord;
	float				f_Previous;
#ifdef ACTIVE_EDITORS
	char				*psz_NameA, *psz_NameB;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (!_pst_SnP) return;
	us_NbElems = _pst_SnP->us_NbElems;
	if(!us_NbElems) return;

	switch(_ul_UpdateMode)
	{
	case 0:
		l_Start = INT_Cul_AxisX;
		l_End = INT_Cul_AxisZ;
		break;

	case 1:
		l_Start = INT_Cul_AxisX;
		l_End = INT_Cul_AxisY;
		break;

	case 2:
		l_Start = INT_Cul_AxisZ;
		l_End = INT_Cul_AxisZ;
		break;

	default:
		l_Start = INT_Cul_AxisX;
		l_End = INT_Cul_AxisZ;
		break;
	}

	for(l_Axis = l_Start; l_Axis <= l_End; l_Axis++)
	{
		pst_FirstNode = _pst_SnP->apst_AxisTable[l_Axis]->pst_Nodes;
		pst_LastNode = pst_FirstNode + us_NbElems - 1;
		pst_NodeUp = pst_FirstNode;

		INT_SnP_UpdateInternalValue(pst_FirstNode, l_Axis);

		f_Previous = pst_NodeUp->f_Val;

		while(pst_NodeUp < pst_LastNode)
		{
			pst_NodeUp++;
			INT_SnP_UpdateInternalValue(pst_NodeUp, l_Axis);

			if(pst_NodeUp->f_Val + 1E-3f < f_Previous)
			{
				b_AlreadySorted = TRUE;
				f_UpCoord = pst_NodeUp->f_Val;

#ifdef ACTIVE_EDITORS
				if(_isnan(f_UpCoord))
				{
					ERR_X_Warning
					(
						0,
						"[SnP] Object has invalid coordinates. Replaced by (0,0,0)",
						pst_NodeUp->pst_Obj->sz_Name
					);
					OBJ_SetAbsolutePosition(pst_NodeUp->pst_Obj, &MATH_gst_NulVector);
					pst_NodeUp->f_Val = 0.0f;
					f_UpCoord = 0.0f;
				}

#endif
				pst_NodeDown = pst_NodeUp - 1;
				f_DownCoord = pst_NodeDown->f_Val;

				while(f_UpCoord + 1E-3f < f_DownCoord)
				{
#ifdef ACTIVE_EDITORS
					psz_NameA = pst_NodeUp->pst_Obj->sz_Name;
					psz_NameB = pst_NodeDown->pst_Obj->sz_Name;
					COL_ul_SnP_Permutations++;
#endif
					if(b_AlreadySorted)
					{
						st_NodeUp = *pst_NodeUp;
						b_AlreadySorted = FALSE;
					}

					INT_SnP_ComputeInversion
					(
						_pst_SnP,
						l_Axis,
						pst_NodeDown->us_Ref,
						st_NodeUp.us_Ref,
						pst_NodeDown->uc_Flags & INT_Cul_StartPoint,
						st_NodeUp.uc_Flags & INT_Cul_StartPoint
					);

					pst_NodeDown--;
					if(pst_NodeDown < pst_FirstNode) break;
					f_DownCoord = pst_NodeDown->f_Val;
				}

				if(!b_AlreadySorted)
				{
					L_memmove
					(
						pst_NodeDown + 2,
						pst_NodeDown + 1,
						((pst_NodeUp - pst_NodeDown) - 1) * sizeof(INT_tdst_AxisNode)
					);
					*(pst_NodeDown + 1) = st_NodeUp;
				}
			}

			f_Previous = pst_NodeUp->f_Val;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_AddObject(OBJ_tdst_GameObject *_pst_GO, INT_tdst_SnP *_pst_SnP)
{
	/* If it is a new object */
	if((_pst_GO->us_SnP_Ref == 0xFFFF) && (_pst_SnP->us_NbElems < INT_Cul_MaxNodes))
	{
		/*~~~~~~~~~~~~~~~~~~*/
		void	*pst_SingleBV;
		/*~~~~~~~~~~~~~~~~~~*/

		pst_SingleBV = _pst_GO->pst_BV;

		/* If the object has a AABBox BV. */
		if(OBJ_BV_IsAABBox(pst_SingleBV))
			INT_SnP_AddBox(_pst_SnP, _pst_GO, OBJ_pst_BV_GetGMin(pst_SingleBV), OBJ_pst_BV_GetGMax(pst_SingleBV));

		/* If the object has a sphere BV. */
		else
		{
			INT_SnP_AddSphere(_pst_SnP, _pst_GO, OBJ_pst_BV_GetCenter(pst_SingleBV), OBJ_f_BV_GetRadius(pst_SingleBV));
		}
	}
}

/*
 =======================================================================================================================
    Aim:    As soon as the ActiveObjects have changed, we ADD the new objects in the SnP structure. We DO NOT delete
            the obsolethe objects yet. It will be done later at the Refresh Rate of the SnP
 =======================================================================================================================
 */
void INT_SnP_UpdateFromActiveObjects(INT_tdst_SnP *_pst_SnP, TAB_tdst_PFtable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO) || !(OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_EnableSnP))) continue;

#ifdef ACTIVE_EDITORS
		if(0)
		{
			if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				COL_tdst_Base		*pst_Base;
				OBJ_tdst_GameObject **dpst_Obj, **dpst_LastObj;
				char				sz_Name[50];
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_Base = (COL_tdst_Base *) pst_GO->pst_Extended->pst_Col;
				if(pst_Base && (pst_Base->pst_List) && (pst_Base->pst_List->dpst_CollidedObject))
				{
					dpst_Obj = pst_Base->pst_List->dpst_CollidedObject;
					dpst_LastObj = dpst_Obj + pst_Base->pst_List->ul_NbCollidedObjects;
					sprintf(sz_Name, "%s", pst_GO->sz_Name);
					LINK_PrintStatusMsg("--------------");
					LINK_PrintStatusMsg(sz_Name);
					LINK_PrintStatusMsg("--------------");
					for(; dpst_Obj < dpst_LastObj; dpst_Obj++)
					{
						sprintf(sz_Name, "%s", (*dpst_Obj)->sz_Name);
						LINK_PrintStatusMsg(sz_Name);
					}
				}
			}
		}

#endif
		INT_SnP_AddObject(pst_GO, _pst_SnP);

#ifdef ACTIVE_EDITORS
		ERR_X_Assert(_pst_SnP->us_NbElems < INT_Cul_MaxNodes);
#endif
	}
}

/*
 =======================================================================================================================
    Aim:    Finds if the computed object has been registred for removal
 =======================================================================================================================
 */
BOOL INT_SnP_ObjectMustBeRemoved(INT_tdst_SnP_Manager *_pst_Manager, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject **ppst_Obj;
	OBJ_tdst_GameObject **ppst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ppst_Obj = _pst_Manager->apst_ObjToRemove;
	ppst_Last = ppst_Obj + _pst_Manager->us_NbObjToRemove - 1;

	for(; ppst_Obj <= ppst_Last; ppst_Obj++)
		if(*ppst_Obj == _pst_GO) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Register object for removal
 =======================================================================================================================
 */
void INT_RegisterObjectForRemoving(INT_tdst_SnP_Manager *_pst_Manager, OBJ_tdst_GameObject *_pst_GO)
{
	_pst_Manager->apst_ObjToRemove[_pst_Manager->us_NbObjToRemove++] = _pst_GO;
}

/*
 =======================================================================================================================
    Aim:    We go thru an AxisTable and delete the nodes whose object has been registred for removal.
 =======================================================================================================================
 */
void INT_SnP_UpdateAxisTable(INT_tdst_SnP *_pst_SnP, ULONG _ul_Axis)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_AxisNode		*pst_FirstNode, *pst_Node, *pst_LastNode;
	INT_tdst_SnP_Manager	*pst_Manager;
	USHORT					us_Index;
	USHORT					*pus_FirstRank, *pus_CurrentRank, *pus_LastRank;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Manager = _pst_SnP->pst_Manager;
	pst_Manager->us_NbRanks = 0;
	pst_FirstNode = _pst_SnP->apst_AxisTable[_ul_Axis]->pst_Nodes;
	pst_Node = pst_FirstNode;
	pst_LastNode = pst_Node + _pst_SnP->us_NbElems - 1;

	for(us_Index = 0; pst_Node <= pst_LastNode; pst_Node++, us_Index++)
	{
		ERR_X_Assert(pst_Node);
		if(INT_SnP_ObjectMustBeRemoved(pst_Manager, pst_Node->pst_Obj))
			pst_Manager->aus_Rank[pst_Manager->us_NbRanks++] = us_Index;
	}

	pus_FirstRank = pst_Manager->aus_Rank;
	pus_CurrentRank = pus_FirstRank;
	pus_LastRank = pus_FirstRank + pst_Manager->us_NbRanks - 1;

	pst_Manager->us_IndexToPlace = *pus_FirstRank;

	for(; pus_CurrentRank <= pus_LastRank - 1; pus_CurrentRank++)
	{
		L_memcpy
		(
			pst_FirstNode + pst_Manager->us_IndexToPlace,
			pst_FirstNode + *pus_CurrentRank + 1,
			(*(pus_CurrentRank + 1) - *pus_CurrentRank - 1) * sizeof(INT_tdst_AxisNode)
		);
		pst_Manager->us_IndexToPlace += *(pus_CurrentRank + 1) - *pus_CurrentRank - 1;
	}

	if(pst_Manager->us_NbRanks)
	{
		L_memcpy
		(
			pst_FirstNode + pst_Manager->us_IndexToPlace,
			pst_FirstNode + *pus_LastRank + 1,
			(_pst_SnP->us_NbElems - *pus_LastRank - 1) * sizeof(INT_tdst_AxisNode)
		);
	}
}

/*$F
 =======================================================================================================================
    Aim:    We are about to remove objects from the SnP and we want to update the Detection List of
            the object that contains one of the removed objects.

    Note:   For the objects that are going to be removed. 
			1 - Either they have a Detection List and we only need to go thru it to know the overlapping 
			objects and update them.
			2 - Either the dont have a Detection List and we have to go thru all the possible SnP References and
			find if we are overlapping with an objet in the three axis. If it is the case, we update those objects
			Detection List.
 =======================================================================================================================
 */

void INT_SnP_UpdateAllDetectionLists(INT_tdst_SnP *_pst_SnP)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_SnP_Manager	*pst_Manager;
	OBJ_tdst_GameObject		**dpst_First, **dpst_Last;
	OBJ_tdst_GameObject		**dpst_DL_First, **dpst_DL_Last;
	COL_tdst_Base			*pst_Base;
	USHORT					uw_CurrentRef;
	BOOL					b_WasSet;
#ifdef ACTIVE_EDITORS
	char					*sz_Name;
	USHORT					uw_SnP;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Manager = _pst_SnP->pst_Manager;
	pst_Manager->us_NbRanks = 0;
	dpst_First = pst_Manager->apst_ObjToRemove;
	dpst_Last = dpst_First + pst_Manager->us_NbObjToRemove;

	for(; dpst_First < dpst_Last; dpst_First++)
	{
		/* "Bullets" Bug ---> we return; */
		if(((*dpst_First)->us_SnP_Ref) == 0xFFFF) continue;

#ifdef ACTIVE_EDITORS
		/* Fucking Visual 6 Debugger bug. */
		sz_Name = (*dpst_First)->sz_Name;
		uw_SnP = (*dpst_First)->us_SnP_Ref;
#endif
		if(OBJ_b_TestStatusFlag(*dpst_First, OBJ_C_StatusFlag_Detection))
		{
			pst_Base = (COL_tdst_Base *) (*dpst_First)->pst_Extended->pst_Col;

			dpst_DL_First = pst_Base->pst_List->dpst_CollidedObject;
			dpst_DL_Last = dpst_DL_First + pst_Base->pst_List->ul_NbCollidedObjects;
			for(; dpst_DL_First < dpst_DL_Last; dpst_DL_First++)
			{
#ifdef ACTIVE_EDITORS
				/* Fucking Visual 6 Debugger bug. */
				sz_Name = (*dpst_DL_First)->sz_Name;
#endif
				_pst_SnP->apst_AxisTable[ INT_Cul_AxisX ]->flags.Set( ( *dpst_First )->us_SnP_Ref, ( *dpst_DL_First )->us_SnP_Ref, false );
				_pst_SnP->apst_AxisTable[ INT_Cul_AxisY ]->flags.Set( ( *dpst_First )->us_SnP_Ref, ( *dpst_DL_First )->us_SnP_Ref, false );
				_pst_SnP->apst_AxisTable[ INT_Cul_AxisZ ]->flags.Set( ( *dpst_First )->us_SnP_Ref, ( *dpst_DL_First )->us_SnP_Ref, false );

				if(*dpst_DL_First) INT_SnP_RemoveOverlap(*dpst_First, *dpst_DL_First);

				/* We have performed a Memcpy on the object Detection List, we go back once. */
				dpst_DL_First--;
				dpst_DL_Last--;
			}
		}
		else
		{
			for(uw_CurrentRef = 0; uw_CurrentRef < INT_Cul_MaxObjects; uw_CurrentRef++)
			{
				// don't process the ref to itself!
				if((*dpst_First)->us_SnP_Ref == uw_CurrentRef)
				{
					continue;
				}

				b_WasSet = _pst_SnP->apst_AxisTable[ INT_Cul_AxisX ]->flags.Set( ( *dpst_First )->us_SnP_Ref, uw_CurrentRef, false );

				if (
				    b_WasSet
					&& _pst_SnP->apst_AxisTable[ INT_Cul_AxisY ]->flags.Get( ( *dpst_First )->us_SnP_Ref, uw_CurrentRef )
					&& _pst_SnP->apst_AxisTable[ INT_Cul_AxisZ ]->flags.Get( ( *dpst_First )->us_SnP_Ref, uw_CurrentRef ))
				{
					if(_pst_SnP->apst_IndexToObj[uw_CurrentRef])
						INT_SnP_RemoveOverlap(*dpst_First, _pst_SnP->apst_IndexToObj[uw_CurrentRef]);
				}

				_pst_SnP->apst_AxisTable[ INT_Cul_AxisY ]->flags.Set( ( *dpst_First )->us_SnP_Ref, uw_CurrentRef, false );
				_pst_SnP->apst_AxisTable[ INT_Cul_AxisZ ]->flags.Set( ( *dpst_First )->us_SnP_Ref, uw_CurrentRef, false );
			}
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Sets the SnP ref of all the removed objects to 0xFFFF.
 =======================================================================================================================
 */
void INT_SnP_InvalidateRemovedRef(INT_tdst_SnP *_pst_SnP)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_SnP_Manager	*pst_Manager;
	OBJ_tdst_GameObject		**dpst_First, **dpst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Manager = _pst_SnP->pst_Manager;
	pst_Manager->us_NbRanks = 0;
	dpst_First = pst_Manager->apst_ObjToRemove;
	dpst_Last = dpst_First + pst_Manager->us_NbObjToRemove;

	for(; dpst_First < dpst_Last; dpst_First++)
	{
		if(((*dpst_First)->us_SnP_Ref) != 0xFFFF) *(_pst_SnP->apst_IndexToObj + ((*dpst_First)->us_SnP_Ref)) = NULL;
		(*dpst_First)->us_SnP_Ref = 0xFFFF;
	}
}

/*
 =======================================================================================================================
    Aim:    Refresh the SnP structure. We go thru the IndexToObj Table and looks whether the Obj. is actually in the
            activeObjects List. If not, we register it for removal and we finally update the 3 Axis Tables.
 =======================================================================================================================
 */
void INT_SnP_Refresh(INT_tdst_SnP *_pst_SnP, TAB_tdst_PFtable *_pst_ActiveObjects)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		**ppst_Obj, **ppst_Last;
	INT_tdst_SnP_Manager	*pst_Manager;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Manager = _pst_SnP->pst_Manager;
	pst_Manager->us_NbObjToRemove = 0;

	ppst_Obj = _pst_SnP->apst_IndexToObj;
	ppst_Last = ppst_Obj + INT_Cul_MaxObjects;

	if(0)
	{
		for(; ppst_Obj < ppst_Last; ppst_Obj++)
		{
			if(*ppst_Obj)
			{
				/*$F
             * There, we want to delete all the non-active objects. There
             * are 2 ways to determine if an object is active.
			 * 1 - It is no longer in the ActiveObjects table.
			 * 2 - It is in the ActivObjects Table but its us_SnP_Ref is not valid. 
			 * That means that the object has been erased and that another object of the same type 
			 * has been created AT THE SAME PLACE. (Bullets for example)
			 * In That case, we must delete all the information concerning the first object before adding the second.
             */
				if
				(
					(TAB_ul_PFtable_GetElemIndexWithPointer(_pst_ActiveObjects, *ppst_Obj) == TAB_Cul_BadIndex)
				||	((*ppst_Obj)->us_SnP_Ref == 0xFFFF)
				)
				{
					INT_RegisterObjectForRemoving(pst_Manager, *ppst_Obj);
				}
			}
		}
	}

	if(pst_Manager->us_NbObjToRemove)
	{
		INT_SnP_UpdateAllDetectionLists(_pst_SnP);
		INT_SnP_UpdateAxisTable(_pst_SnP, INT_Cul_AxisX);
		INT_SnP_UpdateAxisTable(_pst_SnP, INT_Cul_AxisY);
		INT_SnP_UpdateAxisTable(_pst_SnP, INT_Cul_AxisZ);
		INT_SnP_InvalidateRemovedRef(_pst_SnP);

		_pst_SnP->us_NbElems -= pst_Manager->us_NbObjToRemove << 1;
	}

	INT_SnP_UpdateFromActiveObjects(_pst_SnP, _pst_ActiveObjects);
}

/*
 =======================================================================================================================
    Aim:    We are about to destroy an object. We delete it from the SnP structure.
 =======================================================================================================================
 */
void INT_SnP_DetachObject(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_SnP			*pst_SnP;
	INT_tdst_SnP_Manager	*pst_Manager;
	OBJ_tdst_GameObject		**ppst_Obj, **ppst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SnP = _pst_World->pst_SnP;

	/* If the SnP structure is empty, we return. */
	if(!pst_SnP) return;
	if(!pst_SnP->us_NbElems) return;

	pst_Manager = pst_SnP->pst_Manager;
	pst_Manager->us_NbObjToRemove = 0;

	INT_RegisterObjectForRemoving(pst_Manager, _pst_GO);

	INT_SnP_UpdateAllDetectionLists(pst_SnP);
	INT_SnP_UpdateAxisTable(pst_SnP, INT_Cul_AxisX);
	INT_SnP_UpdateAxisTable(pst_SnP, INT_Cul_AxisY);
	INT_SnP_UpdateAxisTable(pst_SnP, INT_Cul_AxisZ);

	/* We loop thru the SnP IndexToObj Table to delete the _pst_GO */
	ppst_Obj = pst_SnP->apst_IndexToObj;
	ppst_Last = ppst_Obj + (INT_Cul_MaxObjects - 1);

	for(; ppst_Obj <= ppst_Last; ppst_Obj++)
	{
		/* Found it ! */
		if(*ppst_Obj == _pst_GO)
		{
			*ppst_Obj = NULL;
			_pst_GO->us_SnP_Ref = 0xFFFF;
			pst_SnP->us_NbElems -= pst_Manager->us_NbObjToRemove << 1;
			return;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_AttachObject(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_SnP	*pst_SnP;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_SnP = _pst_World->pst_SnP;

	/* If it is a new object */
	if((_pst_GO->us_SnP_Ref == 0xFFFF) && (OBJ_b_TestControlFlag(_pst_GO, OBJ_C_ControlFlag_EnableSnP)) && (pst_SnP->us_NbElems < INT_Cul_MaxNodes))
	{
		/*~~~~~~~~~~~~~~~~~~*/
		void	*pst_SingleBV;
		/*~~~~~~~~~~~~~~~~~~*/

		pst_SingleBV = _pst_GO->pst_BV;

		if(OBJ_BV_IsAABBox(pst_SingleBV))
			INT_SnP_AddBox(pst_SnP, _pst_GO, OBJ_pst_BV_GetGMin(pst_SingleBV), OBJ_pst_BV_GetGMax(pst_SingleBV));
		else
		{
			INT_SnP_AddSphere(pst_SnP, _pst_GO, OBJ_pst_BV_GetCenter(pst_SingleBV), OBJ_f_BV_GetRadius(pst_SingleBV));
		}
	}
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    We have just DnD an AI on a GO. The Detection List of this GO has been allocated but is empty. We update it
            with the overlapping objects.
 =======================================================================================================================
 */
void INT_SnP_UpdateDetectionList(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_SnP	*pst_SnP;
	USHORT			uw_CurrentRef;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SnP = _pst_World->pst_SnP;
	for(uw_CurrentRef = 0; uw_CurrentRef < INT_Cul_MaxObjects; uw_CurrentRef++)
	{
		if ( pst_SnP->apst_AxisTable[ INT_Cul_AxisX ]->flags.Get(_pst_GO->us_SnP_Ref, uw_CurrentRef)
			&& pst_SnP->apst_AxisTable[ INT_Cul_AxisY ]->flags.Get( _pst_GO->us_SnP_Ref, uw_CurrentRef )
			&& pst_SnP->apst_AxisTable[ INT_Cul_AxisZ ]->flags.Get( _pst_GO->us_SnP_Ref, uw_CurrentRef ) )
		{
			if(pst_SnP->apst_IndexToObj[uw_CurrentRef])
			{
				/*~~~~~~~~~~~~~*/
				char	*sz_Name;
				/*~~~~~~~~~~~~~*/

				sz_Name = pst_SnP->apst_IndexToObj[uw_CurrentRef]->sz_Name;
				INT_SnP_AddOverlap(_pst_GO, pst_SnP->apst_IndexToObj[uw_CurrentRef]);
			}
		}
	}
}


#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_Check(INT_tdst_SnP *_pst_SnP, TAB_tdst_PFtable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	COL_tdst_Base		*pst_ColBase;
	ULONG				ul_WorldObjects, ul_ColMapObjects, ul_ZDMObjects;
	ULONG				ul_ODEObjects;
	char				asz_Log[200];
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_ColMapObjects = 0;
	ul_WorldObjects = 0;
	ul_ZDMObjects = 0;
	ul_ODEObjects = 0;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	LINK_PrintStatusMsg("");
	LINK_PrintStatusMsg("Checking SnP...");
	LINK_PrintStatusMsg("----------------------------------------------------------------------");
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		/* Bones are not inserted in SnP. */
		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone) continue;

		ul_WorldObjects ++;

		if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap) && (pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active))
		{
			ul_ColMapObjects ++;
			continue;
		}

		if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ZDM)  && (pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active))
		{
			pst_ColBase = (COL_tdst_Base *)pst_GO->pst_Extended->pst_Col;
			for(i = 0; i < pst_ColBase->pst_Instance->uc_NbOfZDx; i++)
			{
				if
				(
					COL_b_Zone_TestFlag(pst_ColBase->pst_Instance->dpst_ZDx[i], COL_C_Zone_ZDM)
				&&	COL_b_Instance_IsActive(pst_ColBase->pst_Instance, (UCHAR)i)
				)
				{
					ul_ZDMObjects ++;
					break;					
				}
			}
			continue;
		}

		if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && (pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active))
		{
			ul_ODEObjects ++;
		}


		if(pst_GO->us_SnP_Ref != (USHORT) -1)
		{
			if(pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active)
			{
				sprintf(asz_Log, "- %s has no ZDM/ColMap/ODE but is in the SnP", pst_GO->sz_Name);
			}
			else
			{
				sprintf(asz_Log, "- %s is not (or no more) active but is in the SnP.", pst_GO->sz_Name);
			}

			LINK_PrintStatusMsg(asz_Log);
		}

	}

	LINK_PrintStatusMsg("----------------------------------------------------------------------");
	sprintf
	(
		asz_Log,
		"There are currently %u objects in SnP. (Active Objects with ColMap: %u) (Active Objects with ZDM but no ColMap %u) (Active Objects with ODE %u) (World :%u)",
		_pst_SnP->us_NbElems / 2,
		ul_ColMapObjects,
		ul_ZDMObjects,
		ul_ODEObjects,
		ul_WorldObjects
	);
	LINK_PrintStatusMsg(asz_Log);
}

#endif

#endif
#if defined(__cplusplus)
}
#endif

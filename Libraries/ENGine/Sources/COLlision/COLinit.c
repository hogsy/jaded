/*$T COLinit.c GC! 1.081 09/11/02 16:58:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "BASe/MEMory/MEM.h"
#include "SDK/Sources/MATHs/MATH_MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINks/LINKstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/COLlision/COLset.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/COLlision/COLaccess.h"

#include "MATHs/MATH.h"

#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLload.h"

#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"

#include "GEOmetric/GEOsubobject.h"

#ifdef ACTIVE_EDITORS
#include "EDIPaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "COLsave.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKstruct_reg.h"

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern ULONG	LOA_ul_FileTypeSize[40];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_b_UnCollidable(OBJ_tdst_GameObject * _pst_A, OBJ_tdst_GameObject * _pst_B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Base *pst_Base;
	ULONG			i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!_pst_A)
	||	(!_pst_A->pst_Extended)
	||	(!_pst_A->pst_Extended->pst_Col)
	||	(!((COL_tdst_Base *)_pst_A->pst_Extended->pst_Col)->pst_UnCollidable)
	||	(!_pst_B)
	||	(!_pst_B->pst_Extended)
	||	(!_pst_B->pst_Extended->pst_Col)
	) return FALSE;

	pst_Base = (COL_tdst_Base *) _pst_A->pst_Extended->pst_Col;

	for(i = 0; i < pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects; i++)
	{
		if(pst_Base->pst_UnCollidable->dpst_UnCollidableObject[i] == _pst_B)
		{
			return TRUE;
		}
	}

	return FALSE;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_AddUnCollidableObject(OBJ_tdst_GameObject * _pst_A, OBJ_tdst_GameObject * _pst_B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Base *pst_Base;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!_pst_A)
	||	(!_pst_A->pst_Extended)
	||	(!_pst_A->pst_Extended->pst_Col)
	||	(!_pst_B)
	||	(!_pst_B->pst_Extended)
	||	(!_pst_B->pst_Extended->pst_Col)
	) return;


	/* A already in B list ... return */
	if(COL_b_UnCollidable(_pst_A, _pst_B))
		return;

	/**********************************/
	/* Add B into A UnCollidable List */
	/**********************************/
	pst_Base = (COL_tdst_Base *) _pst_A->pst_Extended->pst_Col;

	if(!pst_Base->pst_UnCollidable)
	{
		pst_Base->pst_UnCollidable = (COL_tdst_DetectionList *) MEM_p_Alloc(sizeof(COL_tdst_DetectionList));
		L_memset(pst_Base->pst_UnCollidable, 0, sizeof(COL_tdst_DetectionList));
	}

	if(!pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects)
	{
		pst_Base->pst_UnCollidable->dpst_UnCollidableObject = (OBJ_tdst_GameObject **) MEM_p_Alloc(5 * sizeof(OBJ_tdst_GameObject *));
	}

	pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects ++;

	if(!(pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects % 5))
	{
		pst_Base->pst_UnCollidable->dpst_UnCollidableObject = (OBJ_tdst_GameObject **) MEM_p_Realloc(pst_Base->pst_UnCollidable->dpst_UnCollidableObject, (pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects + 5) * sizeof(OBJ_tdst_GameObject *));
	}

	pst_Base->pst_UnCollidable->dpst_UnCollidableObject[pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects - 1] = _pst_B;

	/**********************************/
	/* Add A into B UnCollidable List */
	/**********************************/
	pst_Base = (COL_tdst_Base *) _pst_B->pst_Extended->pst_Col;

	if(!pst_Base->pst_UnCollidable)
	{
		pst_Base->pst_UnCollidable = (COL_tdst_DetectionList *) MEM_p_Alloc(sizeof(COL_tdst_DetectionList));
		L_memset(pst_Base->pst_UnCollidable, 0, sizeof(COL_tdst_DetectionList));
	}

	if(!pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects)
	{
		pst_Base->pst_UnCollidable->dpst_UnCollidableObject = (OBJ_tdst_GameObject **) MEM_p_Alloc(5 * sizeof(OBJ_tdst_GameObject *));
	}

	pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects ++;

	if(!(pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects % 5))
	{
		pst_Base->pst_UnCollidable->dpst_UnCollidableObject = (OBJ_tdst_GameObject **) MEM_p_Realloc(pst_Base->pst_UnCollidable->dpst_UnCollidableObject, (pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects + 5) * sizeof(OBJ_tdst_GameObject *));
	}

	pst_Base->pst_UnCollidable->dpst_UnCollidableObject[pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects - 1] = _pst_A;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_RemoveUnCollidableObject(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Base		*pst_Base;
	ULONG				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!_pst_A)
	||	(!_pst_A->pst_Extended)
	||	(!_pst_A->pst_Extended->pst_Col)
	||	(!_pst_B)
	||	(!_pst_B->pst_Extended)
	||	(!_pst_B->pst_Extended->pst_Col)
	) return;

	/*************************************/
	/* Remove B from A UnCollidable List */
	/*************************************/
	pst_Base = (COL_tdst_Base *) _pst_A->pst_Extended->pst_Col;
	if(pst_Base->pst_UnCollidable && pst_Base->pst_UnCollidable->dpst_UnCollidableObject)
	{
		for(i = 0; i < pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects; i++)
		{
			if(pst_Base->pst_UnCollidable->dpst_UnCollidableObject[i] == _pst_B)
			{
				L_memcpy(&pst_Base->pst_UnCollidable->dpst_UnCollidableObject[i], &pst_Base->pst_UnCollidable->dpst_UnCollidableObject[i+1], (pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects - i - 1) * sizeof(OBJ_tdst_GameObject *));
				pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects --;

				if(!pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects)
				{
					MEM_Free(pst_Base->pst_UnCollidable->dpst_UnCollidableObject);
					MEM_Free(pst_Base->pst_UnCollidable);
					pst_Base->pst_UnCollidable = NULL;
				}
				break;
			}
		}
	}

	/*************************************/
	/* Remove A from A UnCollidable List */
	/************************************/
	pst_Base = (COL_tdst_Base *) _pst_B->pst_Extended->pst_Col;
	if(pst_Base->pst_UnCollidable && pst_Base->pst_UnCollidable->dpst_UnCollidableObject)
	{
		for(i = 0; i < pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects; i++)
		{
			if(pst_Base->pst_UnCollidable->dpst_UnCollidableObject[i] == _pst_A)
			{
				L_memcpy(&pst_Base->pst_UnCollidable->dpst_UnCollidableObject[i], &pst_Base->pst_UnCollidable->dpst_UnCollidableObject[i+1], (pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects - i - 1) * sizeof(OBJ_tdst_GameObject *));
				pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects --;

				if(!pst_Base->pst_UnCollidable->ul_NbUnCollidableObjects)
				{
					MEM_Free(pst_Base->pst_UnCollidable->dpst_UnCollidableObject);
					MEM_Free(pst_Base->pst_UnCollidable);
					pst_Base->pst_UnCollidable = NULL;
				}
				break;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_FlushAndFreeUnCollidableList(OBJ_tdst_GameObject *_pst_A)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Base		*pst_Base;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!_pst_A)
	||	(!_pst_A->pst_Extended)
	||	(!_pst_A->pst_Extended->pst_Col)
	) return;

	pst_Base = (COL_tdst_Base *) _pst_A->pst_Extended->pst_Col;

	while(pst_Base->pst_UnCollidable)
	{
		COL_RemoveUnCollidableObject(_pst_A, pst_Base->pst_UnCollidable->dpst_UnCollidableObject[0]);
	}
}

/*
 =======================================================================================================================
    Aim:    Alloc the Detection List for an object. This List is in fact the array of objects that overlap the current
            object. This structure is filled by the Sweep and Prune.
 =======================================================================================================================
 */
void COL_AllocDetectionList(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Base			*pst_ColBase;
	COL_tdst_DetectionList	*pst_DL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO->pst_Extended)
	{
		OBJ_GameObject_CreateExtended(_pst_GO);
		_pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_ExtendedObject;
	}

	if(!_pst_GO->pst_Extended->pst_Col)
	{
		_pst_GO->pst_Extended->pst_Col = MEM_p_Alloc(sizeof(COL_tdst_Base));
		LOA_ul_FileTypeSize[38] += sizeof(COL_tdst_Base);
		L_memset((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col, 0, sizeof(COL_tdst_Base));
	}

	pst_ColBase = (COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col;

	pst_ColBase->pst_List = (COL_tdst_DetectionList *) MEM_p_Alloc(sizeof(COL_tdst_DetectionList));
	LOA_ul_FileTypeSize[38] += sizeof(COL_tdst_DetectionList);
	pst_DL = pst_ColBase->pst_List;

	pst_DL->ul_NbCollidedObjects = 0;

	pst_DL->dpst_CollidedObject = (OBJ_tdst_GameObject **) MEM_p_Alloc(COL_Cul_MaxDefaultCollidedObjects * sizeof(OBJ_tdst_GameObject *));
	LOA_ul_FileTypeSize[38] += COL_Cul_MaxDefaultCollidedObjects * sizeof(OBJ_tdst_GameObject *);
	L_memset(pst_DL->dpst_CollidedObject, 0, COL_Cul_MaxDefaultCollidedObjects * sizeof(OBJ_tdst_GameObject *));
}

/*
 =======================================================================================================================
    Aim:    Alloc the Collision Instance for an object. This is done when you "drag and drop" a ColSet onto an object.
 =======================================================================================================================
 */
void COL_AllocInstance(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Base	*pst_ColBase;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO->pst_Extended->pst_Col)
	{
		_pst_GO->pst_Extended->pst_Col = MEM_p_Alloc(sizeof(COL_tdst_Base));
		LOA_ul_FileTypeSize[38] += sizeof(COL_tdst_Base);
		L_memset((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col, 0, sizeof(COL_tdst_Base));
	}

	pst_ColBase = (COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col;

	pst_ColBase->pst_Instance = (COL_tdst_Instance *) MEM_p_Alloc(sizeof(COL_tdst_Instance));
	LOA_ul_FileTypeSize[38] += sizeof(COL_tdst_Instance);
	L_memset(pst_ColBase->pst_Instance, 0, sizeof(COL_tdst_Instance));

	/* Allocation of the Old frame matrix. */
	pst_ColBase->pst_Instance->pst_OldGlobalMatrix = MATH_Matrix_Alloc();
	LOA_ul_FileTypeSize[38] += sizeof(MATH_tdst_Matrix);

	/* Sets the old matrix to identity */
	MATH_SetIdentityMatrix(pst_ColBase->pst_Instance->pst_OldGlobalMatrix);

	pst_ColBase->pst_Instance->ul_Filter_Type = (ULONG) -1;
	pst_ColBase->pst_Instance->ul_Filter_On = 0;
	pst_ColBase->pst_Instance->ul_Filter_Off = 0;

}

/*
 =======================================================================================================================
    Aim:    Alloc a ColSet.
 =======================================================================================================================
 */
COL_tdst_ColSet *COL_AllocColSet(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet *pst_ColSet;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ColSet = (COL_tdst_ColSet *) MEM_p_Alloc(sizeof(COL_tdst_ColSet));
	LOA_ul_FileTypeSize[38] += sizeof(COL_tdst_ColSet);
	L_memset(pst_ColSet, 0, sizeof(COL_tdst_ColSet));
	pst_ColSet->pauc_AI_Indexes = (UCHAR *) MEM_p_Alloc(COL_Cul_MaxNbOfZDx * sizeof(UCHAR));
	LOA_ul_FileTypeSize[38] += sizeof(COL_Cul_MaxNbOfZDx * sizeof(UCHAR));
	L_memset(pst_ColSet->pauc_AI_Indexes, 0xFF, COL_Cul_MaxNbOfZDx * sizeof(UCHAR));

	return pst_ColSet;
}

/*
 =======================================================================================================================
    Aim:    Alloc the ColMap, the list of the collision objects of the current Game object.
 =======================================================================================================================
 */
void COL_AllocColMap(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Base	*pst_ColBase;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO->pst_Extended->pst_Col)
	{
		_pst_GO->pst_Extended->pst_Col = MEM_p_Alloc(sizeof(COL_tdst_Base));
		LOA_ul_FileTypeSize[38] += sizeof(COL_tdst_Base);
		L_memset((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col, 0, sizeof(COL_tdst_Base));
	}

	pst_ColBase = (COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col;

	pst_ColBase->pst_ColMap = (COL_tdst_ColMap *) MEM_p_Alloc(sizeof(COL_tdst_ColMap));
	LOA_ul_FileTypeSize[38] += sizeof(COL_tdst_ColMap);
	L_memset(pst_ColBase->pst_ColMap, 0, sizeof(COL_tdst_ColMap));

	/* Init of the Activation */
	pst_ColBase->pst_ColMap->uc_Activation = 0xFF;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_FreeDetectionList(COL_tdst_Base *_pst_ColBase)
{
	/* To avoid errors. */
	if(!_pst_ColBase) return;

	if(_pst_ColBase->pst_List)
	{
		MEM_Free(_pst_ColBase->pst_List->dpst_CollidedObject);
		MEM_Free(_pst_ColBase->pst_List);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_FreeUnCollidableList(COL_tdst_Base *_pst_ColBase)
{
	/* To avoid errors. */
	if(!_pst_ColBase) return;

	if(_pst_ColBase->pst_UnCollidable)
	{
		MEM_Free(_pst_ColBase->pst_UnCollidable->dpst_UnCollidableObject);
		MEM_Free(_pst_ColBase->pst_UnCollidable);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_FreeColSet(COL_tdst_ColSet *_pst_ColSet)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ZDx	*pst_ZDx, *pst_LastZDx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* To avoid errors. */
	if(!_pst_ColSet) return;

	LOA_DeleteAddress(_pst_ColSet);

	if(_pst_ColSet->uc_NbOfZDx)
	{
		pst_ZDx = _pst_ColSet->past_ZDx;
		pst_LastZDx = pst_ZDx + _pst_ColSet->uc_NbOfZDx;
		for(; pst_ZDx < pst_LastZDx; pst_ZDx++)
		{
			LINK_DelRegisterPointer(pst_ZDx);
#ifdef ACTIVE_EDITORS
			MEM_Free(pst_ZDx->sz_Name);
#endif
			MEM_Free(pst_ZDx->p_Shape);
		}

		/* Free the Array of zone structure. */
		MEM_Free(_pst_ColSet->past_ZDx);
	}

	/* Free the AI-ENG Index Translation Table. */
	MEM_Free(_pst_ColSet->pauc_AI_Indexes);

	/* Free the ColSet. */
	MEM_Free(_pst_ColSet);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_FreeInstance(COL_tdst_Base *_pst_ColBase)
{
	/* To avoid errors. */
	if(!_pst_ColBase) return;

	if(_pst_ColBase->pst_Instance)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Instance	*pst_Instance;
		COL_tdst_ZDx		*pst_ZDx, *pst_LastZDx;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Instance = _pst_ColBase->pst_Instance;

		/*
		 * If the instance has a ColSet, we check the number of loaded instances of this
		 * ColSet to see if we can unload the ColSet.
		 */
		if(pst_Instance->pst_ColSet)
		{
#ifdef ACTIVE_EDITORS
			COL_ColSet_RemoveInstance(pst_Instance->pst_ColSet, pst_Instance);
#endif
			pst_Instance->pst_ColSet->uw_NbOfInstances--;

			/* If the ColSet has no more instance loaded. */
			if(!pst_Instance->pst_ColSet->uw_NbOfInstances) COL_FreeColSet(pst_Instance->pst_ColSet);
		}

		LINK_DelRegisterPointer(pst_Instance);
		LOA_DeleteAddress(pst_Instance);

		/* Free the Old Frame Matrix. */
        MATH_Matrix_Free( pst_Instance->pst_OldGlobalMatrix );

		if(pst_Instance->uc_NbOfSpecific)
		{
			pst_ZDx = pst_Instance->past_Specific;
			pst_LastZDx = pst_ZDx + pst_Instance->uc_NbOfSpecific;
			for(; pst_ZDx < pst_LastZDx; pst_ZDx++)
			{
				LINK_DelRegisterPointer(pst_ZDx);
#ifdef ACTIVE_EDITORS
				MEM_Free(pst_ZDx->sz_Name);
#endif
				MEM_Free(pst_ZDx->p_Shape);
			}

			MEM_Free(pst_Instance->past_Specific);
		}

		if(pst_Instance->dpst_ZDx) MEM_Free(pst_Instance->dpst_ZDx);

		/* Free the instance. */
		MEM_Free(pst_Instance);

		_pst_ColBase->pst_Instance = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_FreeMathematicalCob(COL_tdst_Cob *_pst_Cob)
{
    if (_pst_Cob->pst_MathCob)
    	MEM_Free(_pst_Cob->pst_MathCob->p_Shape);
	MEM_Free(_pst_Cob->pst_MathCob);
#ifdef ACTIVE_EDITORS
	MEM_Free(_pst_Cob->sz_GMatName);
#endif
	MEM_Free(_pst_Cob);
}

void COL_OK3_RecursiveFree(COL_tdst_OK3_Node *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_FreeTrianglesIndexedCob(COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	COL_tdst_IndexedTriangles			*pst_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * We loop thru all the Elements to see if there is not a GameMaterial structure
	 * to desallocate.
	 */
	pst_Col = _pst_Cob->pst_TriangleCob;

#ifdef ACTIVE_EDITORS

	/* Free the Geometric Cob */
	if(_pst_Cob->p_GeoCob)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_ElementIndexedTriangles	*pst_GeoElement, *pst_LastElement;
		GEO_tdst_Object						*pst_GeoCob;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_GeoCob = (GEO_tdst_Object *) _pst_Cob->p_GeoCob;
		if(pst_GeoCob->dst_Point) MEM_Free(pst_GeoCob->dst_Point);
		if(pst_GeoCob->dst_PointNormal) MEM_Free(pst_GeoCob->dst_PointNormal);

		pst_GeoElement = pst_GeoCob->dst_Element;
		pst_LastElement = pst_GeoElement + pst_GeoCob->l_NbElements;
		for(; pst_GeoElement < pst_LastElement; pst_GeoElement++)
		{
			if(pst_GeoElement->l_NbTriangles && pst_GeoElement->dst_Triangle) MEM_Free(pst_GeoElement->dst_Triangle);
		}

		if(pst_GeoCob->dst_Element) MEM_Free(pst_GeoCob->dst_Element);

		if(pst_GeoCob->pst_SubObject) GEO_SubObject_Free(pst_GeoCob);

		MEM_Free(pst_GeoCob);
	}

#endif
	pst_Element = pst_Col->dst_Element;
	pst_LastElement = pst_Element + pst_Col->l_NbElements;
	for(; pst_Element < pst_LastElement; pst_Element++)
	{
		if(pst_Element->uw_NbTriangles && pst_Element->dst_Triangle) MEM_Free(pst_Element->dst_Triangle);
	}

	MEM_Free(pst_Col->dst_Element);
	MEM_Free(pst_Col->dst_FaceNormal);
	MEM_Free(pst_Col->dst_Point);
	if(pst_Col->pst_OK3)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int						i, j;
		COL_tdst_OK3_Box		*pst_Box;
		COL_tdst_OK3_Element	*pst_Element;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(i = 0; i < (int) pst_Col->pst_OK3->ul_NumBox; i++)
		{
			pst_Box = &pst_Col->pst_OK3->pst_OK3_Boxes[i];

			for(j = 0; j < (int) pst_Box->ul_NumElement; j++)
			{
				pst_Element = &pst_Box->pst_OK3_Element[j];

				if(pst_Element->puw_OK3_Triangle) MEM_Free(pst_Element->puw_OK3_Triangle);
			}

			MEM_Free(pst_Box->pst_OK3_Element);
		}

		MEM_Free(pst_Col->pst_OK3->pst_OK3_Boxes);
		MEM_Free(pst_Col->pst_OK3->paul_Tag);

		COL_OK3_RecursiveFree(pst_Col->pst_OK3->pst_OK3_God);

		MEM_Free(pst_Col->pst_OK3);
	}

	MEM_Free(_pst_Cob->pst_TriangleCob);
#ifdef ACTIVE_EDITORS
	MEM_Free(_pst_Cob->sz_GMatName);
	_pst_Cob->sz_GMatName = NULL;
#endif
	MEM_Free(_pst_Cob);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_FreeGameMaterial(COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_GameMatList	*pst_GMatList;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Cob && _pst_Cob->pst_GMatList)
	{
		pst_GMatList = _pst_Cob->pst_GMatList;
		pst_GMatList->ul_NbOfInstances--;
		if(!pst_GMatList->ul_NbOfInstances)
		{
			MEM_Free(pst_GMatList->pal_Id);
			MEM_Free(pst_GMatList->pst_GMat);
			MEM_Free(pst_GMatList);
			LOA_DeleteAddress(pst_GMatList);
			_pst_Cob->pst_GMatList = NULL;
			_pst_Cob->uc_Flag |= ~COL_C_Cob_GameMat;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_FreeCob(COL_tdst_Cob *pst_Cob)
{
	if(pst_Cob != NULL)
	{

		/*~~~~~~~~~~~~~~~~*/
		BOOL	b_Triangles;
		/*~~~~~~~~~~~~~~~~*/

		b_Triangles = (COL_Cob_GetType(pst_Cob) == COL_C_Zone_Triangles);
		(pst_Cob)->uw_NbOfInstances--;

		if(!(pst_Cob)->uw_NbOfInstances)
		{
			COL_FreeGameMaterial(pst_Cob);

			LOA_DeleteAddress(pst_Cob);
			LINK_DelRegisterPointer(pst_Cob);
			if(b_Triangles)
				COL_FreeTrianglesIndexedCob(pst_Cob);
			else
				COL_FreeMathematicalCob(pst_Cob);
		}
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_FreeColMap(COL_tdst_Base *_pst_ColBase)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap *pst_ColMap;
	COL_tdst_Cob	**dpst_Cob, **dpst_LastCob;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* To avoid errors. */
	if((!_pst_ColBase) || (!_pst_ColBase->pst_ColMap)) return;

	pst_ColMap = _pst_ColBase->pst_ColMap;
	LOA_DeleteAddress(pst_ColMap);
	if(pst_ColMap->uc_NbOfCob)
	{
		dpst_Cob = pst_ColMap->dpst_Cob;
		dpst_LastCob = dpst_Cob + pst_ColMap->uc_NbOfCob;
		for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
			COL_FreeCob(*dpst_Cob);

		MEM_Free(pst_ColMap->dpst_Cob);
	}

	MEM_Free(pst_ColMap);

	_pst_ColBase->pst_ColMap = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_Free(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Base	*pst_ColBase;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* To avoid errors .. */
	if((!_pst_GO) || (!_pst_GO->pst_Extended) || (!_pst_GO->pst_Extended->pst_Col)) return;

	pst_ColBase = (COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col;
	if(pst_ColBase->pst_Instance) COL_FreeInstance(pst_ColBase);
	if(pst_ColBase->pst_List) COL_FreeDetectionList(pst_ColBase);
	if(pst_ColBase->pst_UnCollidable) COL_FreeUnCollidableList(pst_ColBase);
	if(pst_ColBase->pst_ColMap) COL_FreeColMap(pst_ColBase);
	MEM_Free(pst_ColBase);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_Reinit(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	if(pst_World)
		INT_SnP_DetachObject(_pst_GO, pst_World);
	else
		_pst_GO->us_SnP_Ref = 0xFFFF;

	/* To avoid errors. */
	if((!_pst_GO->pst_Extended) || (!_pst_GO->pst_Extended->pst_Col)) return;

	/* Sweep and Prune. */
	if(OBJ_b_TestStatusFlag(_pst_GO, OBJ_C_StatusFlag_Detection))
	{
		/* Sets the number of overlapping objects to 0 */
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_List->ul_NbCollidedObjects = 0;
	}

	COL_FreeUnCollidableList((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col);
	((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_UnCollidable = NULL;

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		/* To avoid errors */
		if(!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance)
			OBJ_ClearIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE);
		else
		{
			/* Sets the Activation Flag to all true. */
			((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Activation = 0xFFFF;

			/* Reset the Crossable Flag. */
			((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Crossable = COL_Cul_DefaultCross;

			((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->ul_Filter_Type = (ULONG) -1;
			((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->ul_Filter_On = 0;
			((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->ul_Filter_Off = 0;

			/* Init of the "previous frame" matrix. */
			if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				MATH_CopyMatrix
				(
					((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix
				);
			}
			else
			{
				MATH_CopyMatrix
				(
					((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					OBJ_pst_GetAbsoluteMatrix(_pst_GO)
				);
			}
		}
	}

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
	{
		/* To avoid errors */
		if(!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap)
			OBJ_ClearIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap);
		else
			/* Sets the Activation Flag to all true. */
			((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->uc_Activation = 0xFF;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_DuplicateInstance(OBJ_tdst_GameObject *_pst_DestGO, OBJ_tdst_GameObject *_pst_SrcGO, BOOL _b_Create)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance		*pst_A_Instance, *pst_B_Instance;
	COL_tdst_Base			*pst_B_Base;
	COL_tdst_ColSet			*pst_A_ColSet;
	COL_tdst_ZDx			*pst_A_ZDx, *pst_B_ZDx, *pst_A_LastZDx;
	COL_tdst_ZDx			*pst_ColSet_ZDx, *pst_Spec_ZDx;
	COL_tdst_ZDx			**dpst_ZDx, **dpst_A_ZDx, **dpst_LastZDx;
	UCHAR					uc_Index, uc_Type, uc_ENG_Index;
#ifdef ACTIVE_EDITORS
	BIG_KEY					ul_Key;
	BIG_INDEX				ul_ColSetFat;
	BIG_INDEX				ul_ColSetKey;
	COL_tdst_ColSet			*pst_ColSet;
	struct WOR_tdst_World_	*pst_World;

#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_A_Instance = ((COL_tdst_Base *) _pst_SrcGO->pst_Extended->pst_Col)->pst_Instance;
	pst_B_Base = (COL_tdst_Base *) _pst_DestGO->pst_Extended->pst_Col;

	COL_AllocInstance(_pst_DestGO);

	pst_B_Instance = ((COL_tdst_Base *) _pst_DestGO->pst_Extended->pst_Col)->pst_Instance;
	pst_A_ColSet = pst_A_Instance->pst_ColSet;

	pst_B_Instance->pst_ColSet = pst_A_ColSet;
	pst_B_Instance->uc_NbOfZDx = pst_A_Instance->uc_NbOfZDx;
	pst_B_Instance->uc_NbOfShared = pst_A_Instance->uc_NbOfShared;
	pst_B_Instance->uc_NbOfSpecific = pst_A_Instance->uc_NbOfSpecific;
	pst_B_Instance->uw_Specific = pst_A_Instance->uw_Specific;
	pst_B_Instance->uw_Activation = pst_A_Instance->uw_Activation;
	pst_B_Instance->uc_MaxLOD = pst_A_Instance->uc_MaxLOD;
	pst_B_Instance->uc_MinLOD = pst_A_Instance->uc_MinLOD;

	pst_B_Instance->ul_Filter_On = pst_A_Instance->ul_Filter_On;
	pst_B_Instance->ul_Filter_Off = pst_A_Instance->ul_Filter_Off;
	pst_B_Instance->ul_Filter_Type = pst_A_Instance->ul_Filter_Type;

	pst_B_Instance->dpst_ZDx = (COL_tdst_ZDx **) MEM_p_Alloc(pst_A_ColSet->uc_NbOfZDx * sizeof(COL_tdst_ZDx *));
	LOA_ul_FileTypeSize[38] += pst_A_ColSet->uc_NbOfZDx * sizeof(COL_tdst_ZDx *);

	if(pst_A_Instance->uc_NbOfSpecific)
	{
		pst_B_Instance->past_Specific = (COL_tdst_ZDx *) MEM_p_Alloc(pst_A_Instance->uc_NbOfSpecific * sizeof(COL_tdst_ZDx));
		LOA_ul_FileTypeSize[38] += pst_A_Instance->uc_NbOfSpecific * sizeof(COL_tdst_ZDx);
		L_memcpy
		(
			pst_B_Instance->past_Specific,
			pst_A_Instance->past_Specific,
			pst_A_Instance->uc_NbOfSpecific * sizeof(COL_tdst_ZDx)
		);

		pst_A_ZDx = pst_A_Instance->past_Specific;
		pst_B_ZDx = pst_B_Instance->past_Specific;
		pst_A_LastZDx = pst_A_ZDx + pst_A_Instance->uc_NbOfSpecific;
		for(; pst_A_ZDx < pst_A_LastZDx; pst_A_ZDx++, pst_B_ZDx++)
		{
			uc_Type = (UCHAR) COL_Zone_GetType(pst_A_ZDx);
			switch(uc_Type)
			{
			case COL_C_Zone_Box:
				pst_B_ZDx->p_Shape = MEM_p_Alloc(sizeof(COL_tdst_Box));
				LOA_ul_FileTypeSize[38] += sizeof(COL_tdst_Box);
				L_memcpy
				(
					COL_pst_Shape_GetMax(pst_B_ZDx->p_Shape),
					COL_pst_Shape_GetMax(pst_A_ZDx->p_Shape),
					sizeof(MATH_tdst_Vector)
				);
				L_memcpy
				(
					COL_pst_Shape_GetMin(pst_B_ZDx->p_Shape),
					COL_pst_Shape_GetMin(pst_A_ZDx->p_Shape),
					sizeof(MATH_tdst_Vector)
				);
#ifdef ACTIVE_EDITORS
				pst_B_ZDx->pst_Itself = pst_B_ZDx;
				pst_B_ZDx->sz_Name = (char*)MEM_p_Alloc(L_strlen(pst_A_ZDx->sz_Name) + 1);
				L_strcpy(pst_B_ZDx->sz_Name, pst_A_ZDx->sz_Name);
#endif
				break;

			case COL_C_Zone_Sphere:
				pst_B_ZDx->p_Shape = MEM_p_Alloc(sizeof(COL_tdst_Sphere));
				LOA_ul_FileTypeSize[38] += sizeof(COL_tdst_Sphere);
				L_memcpy
				(
					COL_pst_Shape_GetCenter(pst_B_ZDx->p_Shape),
					COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape),
					sizeof(MATH_tdst_Vector)
				);
				COL_Shape_SetRadius(pst_B_ZDx->p_Shape, COL_f_Shape_GetRadius(pst_A_ZDx->p_Shape));
#ifdef ACTIVE_EDITORS
				pst_B_ZDx->pst_Itself = pst_B_ZDx;
				pst_B_ZDx->sz_Name = (char*)MEM_p_Alloc(L_strlen(pst_A_ZDx->sz_Name) + 1);
				L_strcpy(pst_B_ZDx->sz_Name, pst_A_ZDx->sz_Name);
#endif
				break;
			}

#ifdef ACTIVE_EDITORS
			LINK_RegisterDuplicate(pst_A_ZDx, pst_B_ZDx, 0);
#endif
		}
	}

	dpst_ZDx = pst_B_Instance->dpst_ZDx;
	dpst_A_ZDx = pst_A_Instance->dpst_ZDx;
	dpst_LastZDx = dpst_ZDx + pst_B_Instance->uc_NbOfZDx;
	pst_ColSet_ZDx = pst_B_Instance->pst_ColSet->past_ZDx;
	pst_Spec_ZDx = pst_B_Instance->past_Specific;
	for(uc_Index = 0; dpst_ZDx < dpst_LastZDx; dpst_A_ZDx++, dpst_ZDx++, uc_Index++)
	{
		if(COL_b_Instance_IsSpecific(pst_B_Instance, uc_Index))
		{
			uc_ENG_Index = COL_uc_Instance_GetSpecificIndexWithZone(pst_A_Instance, *dpst_A_ZDx);
			*dpst_ZDx = pst_Spec_ZDx + uc_ENG_Index;
		}
		else
		{
			uc_ENG_Index = COL_uc_ColSet_GetIndexWithZone(pst_A_Instance->pst_ColSet, *dpst_A_ZDx);
			*dpst_ZDx = pst_ColSet_ZDx + uc_ENG_Index;
		}
	}

#ifdef ACTIVE_EDITORS
	COL_ColSet_AddInstance(pst_A_ColSet, pst_B_Instance);
#endif /* ACTIVE_EDITORS */
	pst_A_ColSet->uw_NbOfInstances++;

#ifdef ACTIVE_EDITORS
	if(_b_Create)
	{
		pst_World = _pst_DestGO->pst_World;
		pst_ColSet = ((COL_tdst_Base *) _pst_SrcGO->pst_Extended->pst_Col)->pst_Instance->pst_ColSet;
		ul_ColSetFat = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColSet);
		ul_ColSetKey = BIG_FileKey(ul_ColSetFat);
		ul_Key = COL_ul_CreateInstanceFile(pst_World, &ul_ColSetKey, _pst_DestGO->sz_Name, NULL);

		LOA_AddAddress(BIG_ul_SearchKeyToFat(ul_Key), pst_B_Instance);
	}

#endif /* ACTIVE_EDITORS */
}

/*
 =======================================================================================================================
    Aim:    Duplicate the ColMap of an object.

    Note:   When parsing the array of Cobs, we do not update the uw_NbOfInstances because it will be done later in the
            COL_JustAfterLoad Function
 =======================================================================================================================
 */
void COL_DuplicateColMap(OBJ_tdst_GameObject *_pst_DestGO, OBJ_tdst_GameObject *_pst_SrcGO, BOOL _b_Create)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap *pst_A_ColMap, *pst_B_ColMap;
	COL_tdst_Cob	**dpst_Cob, **dpst_LastCob;
	UCHAR			uc_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_A_ColMap = ((COL_tdst_Base *) _pst_SrcGO->pst_Extended->pst_Col)->pst_ColMap;
	if (!pst_A_ColMap)
	{
		if (!_pst_DestGO->pst_Extended) return;
		if (!_pst_DestGO->pst_Extended->pst_Col) return;
		pst_B_ColMap = ((COL_tdst_Base *) _pst_DestGO->pst_Extended->pst_Col)->pst_ColMap;
		if (pst_B_ColMap)
			COL_FreeColMap( (COL_tdst_Base *) _pst_DestGO->pst_Extended->pst_Col );
		return;
	}

	pst_B_ColMap = ((COL_tdst_Base *) _pst_DestGO->pst_Extended->pst_Col)->pst_ColMap;

	/* The B ColMap has not been allocated yet. */
	if (!pst_B_ColMap)
	{
		COL_AllocColMap(_pst_DestGO);
		pst_B_ColMap = ((COL_tdst_Base *) _pst_DestGO->pst_Extended->pst_Col)->pst_ColMap;
	}
	else
	{
		/* Free dpst_Cob before allocating different cobs in it */
		if(pst_B_ColMap->uc_NbOfCob)
		{
			dpst_Cob = pst_B_ColMap->dpst_Cob;
			dpst_LastCob = dpst_Cob + pst_B_ColMap->uc_NbOfCob;
			for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
				COL_FreeCob(*dpst_Cob);

			MEM_Free(pst_B_ColMap->dpst_Cob);
		}
	}

	pst_B_ColMap->uc_Activation = pst_A_ColMap->uc_Activation;
	pst_B_ColMap->uc_CustomBits1 = pst_A_ColMap->uc_CustomBits1;
	pst_B_ColMap->uc_CustomBits2 = pst_A_ColMap->uc_CustomBits2;
	pst_B_ColMap->uc_NbOfCob = pst_A_ColMap->uc_NbOfCob;

	/* Allocation of the array of pointers to Cob. */
	pst_B_ColMap->dpst_Cob = (COL_tdst_Cob **) MEM_p_Alloc(pst_A_ColMap->uc_NbOfCob * sizeof(COL_tdst_Cob *));
	LOA_ul_FileTypeSize[38] += pst_A_ColMap->uc_NbOfCob * sizeof(COL_tdst_Cob *);

	dpst_Cob = pst_A_ColMap->dpst_Cob;
	dpst_LastCob = dpst_Cob + pst_A_ColMap->uc_NbOfCob;
	for(uc_Index = 0; dpst_Cob < dpst_LastCob; dpst_Cob++, uc_Index++)
	{
		*(pst_B_ColMap->dpst_Cob + uc_Index) = *dpst_Cob;

		/* We increase the number of instances to this Cob. */
		(*dpst_Cob)->uw_NbOfInstances++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_SwapToSpecific(OBJ_tdst_GameObject *_pst_A, UCHAR _uc_AI_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_ZDx		*pst_ZDx;
	COL_tdst_ZDx		*pst_CurrentZDx;
	COL_tdst_ZDx		*pst_LastZDx;
	COL_tdst_ZDx		**dpst_ZDx;
	COL_tdst_ZDx		**dpst_LastZDx;
	UCHAR				uc_Index;
	UCHAR				uc_Cpt;
	ULONG				ul_OldPosition;
	ULONG				ul_Offset;
#ifdef ACTIVE_EDITORS
	BIG_INDEX			ul_Key;
	BIG_INDEX			ul_Fat;
	char				asz_Path[BIG_C_MaxLenPath];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(_uc_AI_Index == 0xFF)
	||	!_pst_A
	||	!_pst_A->pst_Extended
	||	!_pst_A->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) _pst_A->pst_Extended->pst_Col)->pst_Instance
	||	!((COL_tdst_Base *) _pst_A->pst_Extended->pst_Col)->pst_Instance->pst_ColSet
	) return;

	pst_Instance = ((COL_tdst_Base *) _pst_A->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;

	uc_Index = pst_ColSet->pauc_AI_Indexes[_uc_AI_Index];

	if(uc_Index > pst_Instance->uc_NbOfZDx) return;
	if(COL_b_Instance_IsSpecific(pst_Instance, uc_Index)) return;

	pst_ZDx = *(((COL_tdst_Base *) _pst_A->pst_Extended->pst_Col)->pst_Instance->dpst_ZDx + uc_Index);

	/* Allocation/Reallocation. */
	if(!pst_Instance->uc_NbOfSpecific)
		pst_Instance->past_Specific = (COL_tdst_ZDx *) MEM_p_Alloc(sizeof(COL_tdst_ZDx));
	else
	{
		/*
		 * We unregister all the specific zones cause the realloc can change the place of
		 * the array in memory.
		 */
		pst_CurrentZDx = pst_Instance->past_Specific;
		pst_LastZDx = pst_CurrentZDx + pst_Instance->uc_NbOfSpecific;
		for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++) LINK_DelRegisterPointer(pst_CurrentZDx);

		/*
		 * We are about to make a Realloc of the instance's specific array of zones. That
		 * means that all the pointers that points to thoses zones in the instance array
		 * of pointers are going to be obsoletes. So we compute the "offset" of the
		 * realloc in order to be able to recompute the real position of the pointers to
		 * those zones in memory.
		 */
		ul_OldPosition = (ULONG) pst_Instance->past_Specific;
		pst_Instance->past_Specific = (COL_tdst_ZDx *) MEM_p_Realloc
			(
				pst_Instance->past_Specific,
				(pst_Instance->uc_NbOfSpecific + 1) * sizeof(COL_tdst_ZDx)
			);
		ul_Offset = (ULONG) pst_Instance->past_Specific - ul_OldPosition;

		/* We loop thru the instance's array of pointers to update pointers. */
		dpst_ZDx = pst_Instance->dpst_ZDx;
		dpst_LastZDx = dpst_ZDx + pst_Instance->uc_NbOfZDx;
		for(uc_Cpt = 0; dpst_ZDx < dpst_LastZDx; dpst_ZDx++, uc_Cpt++)
		{
			if(COL_b_Instance_IsSpecific(pst_Instance, uc_Cpt))
			{
				ul_OldPosition = (ULONG) * dpst_ZDx;
				*(dpst_ZDx) = (COL_tdst_ZDx *) (ul_OldPosition + ul_Offset);
			}
		}
	}

	/* We get the pointer on the new structure just allocated. */
	pst_CurrentZDx = pst_Instance->past_Specific + pst_Instance->uc_NbOfSpecific;
	*pst_CurrentZDx = *pst_ZDx;

	switch(pst_ZDx->uc_Type)
	{
	case COL_C_Zone_Sphere:
		pst_CurrentZDx->p_Shape = MEM_p_Alloc(sizeof(COL_tdst_Sphere));
		*(COL_tdst_Sphere *) (pst_CurrentZDx->p_Shape) = *(COL_tdst_Sphere *) (pst_ZDx->p_Shape);
		pst_CurrentZDx->uc_Type = pst_ZDx->uc_Type;
		pst_CurrentZDx->uc_Flag = pst_ZDx->uc_Flag;
		pst_CurrentZDx->uc_BoneIndex = pst_ZDx->uc_BoneIndex;
		pst_CurrentZDx->uc_Design = pst_ZDx->uc_Design;
#ifdef ACTIVE_EDITORS
		pst_CurrentZDx->pst_Itself = pst_CurrentZDx;
		pst_CurrentZDx->sz_Name = (char *) MEM_p_Alloc(30);
		strcpy(pst_CurrentZDx->sz_Name, "Specific");
		sprintf(pst_CurrentZDx->sz_Name + L_strlen(pst_CurrentZDx->sz_Name), "@%x", (ULONG) pst_CurrentZDx->p_Shape);
#endif
		break;

	case COL_C_Zone_Box:
		pst_CurrentZDx->p_Shape = MEM_p_Alloc(sizeof(COL_tdst_Box));
		*(COL_tdst_Box *) (pst_CurrentZDx->p_Shape) = *(COL_tdst_Box *) (pst_ZDx->p_Shape);
		pst_CurrentZDx->uc_Type = pst_ZDx->uc_Type;
		pst_CurrentZDx->uc_Flag = pst_ZDx->uc_Flag;
		pst_CurrentZDx->uc_BoneIndex = pst_ZDx->uc_BoneIndex;
		pst_CurrentZDx->uc_Design = pst_ZDx->uc_Design;
#ifdef ACTIVE_EDITORS
		pst_CurrentZDx->pst_Itself = pst_CurrentZDx;
		pst_CurrentZDx->sz_Name = (char *) MEM_p_Alloc(30);
		strcpy(pst_CurrentZDx->sz_Name, "Specific");
		sprintf(pst_CurrentZDx->sz_Name + L_strlen(pst_CurrentZDx->sz_Name), "@%x", (ULONG) pst_CurrentZDx->p_Shape);
#endif
		break;
	}

#ifdef ACTIVE_EDITORS
	/* We remember the ColSet ZDx; */
	pst_CurrentZDx->pst_ColSetZDx = pst_ZDx;
#endif

	/* We set the flag that indicates that this zone is specific in the INSTANCE. */
	COL_Instance_SetSpecificFlag(pst_Instance, uc_Index);

	/* We set the flag that indicates that this zone is specific in the zone itself. */
	COL_Zone_SetFlag(pst_CurrentZDx, COL_C_Zone_Specific);

	/* We update the Instance Array of pointers. */
	COL_Instance_UpdatePointerAtIndex(pst_Instance, pst_CurrentZDx, uc_Index);

	/* The number of Specific zones increases. */
	pst_Instance->uc_NbOfSpecific++;

	/* The number of Shared zones decreases. */
	pst_Instance->uc_NbOfShared--;

#ifdef ACTIVE_EDITORS
	/* We have to re-register all the previous specific zones + the new one. */
	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Instance);
	if(ul_Key != BIG_C_InvalidKey)
	{
		ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
		BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);

		pst_CurrentZDx = pst_Instance->past_Specific;
		pst_LastZDx = pst_CurrentZDx + pst_Instance->uc_NbOfSpecific;
		for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
			LINK_RegisterPointer(pst_CurrentZDx, LINK_C_Zone, BIG_NameFile(ul_Fat), asz_Path);
	}

	LINK_UpdatePointers();
//	LINK_Refresh();
#endif

}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

/*$T COLsave.c GC! 1.081 10/12/01 15:15:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "LINKs/LINKtoed.h"

#include "COLstruct.h"
#include "COLset.h"
#include "COLconst.h"
#include "COLaccess.h"
#include "COLcob.h"
#include "COLedit.h"

#include "WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORsave.h"

#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "GEOmetric/GEOobject.h"

#include "EDIpaths.h"

extern BOOL	EDI_gb_ComputeMap;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_SortGameMaterial(COL_tdst_GameMatList *_pst_GMatList)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_GameMat	*pst_CurrentGMat, *pst_LastGMat;
	COL_tdst_GameMat	st_TempGMat;
	ULONG				ul_CurrentID, ul_PreviousID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GMatList) return;

	pst_CurrentGMat = _pst_GMatList->pst_GMat;
	pst_LastGMat = _pst_GMatList->pst_GMat + _pst_GMatList->ul_GMat;

	ul_PreviousID = _pst_GMatList->pal_Id[0];

	while(pst_CurrentGMat < pst_LastGMat - 1)
	{
		pst_CurrentGMat++;

		ul_CurrentID = _pst_GMatList->pal_Id[pst_CurrentGMat - _pst_GMatList->pst_GMat];
		if(ul_CurrentID < ul_PreviousID)
		{
			st_TempGMat = *pst_CurrentGMat;
			*pst_CurrentGMat = *(pst_CurrentGMat - 1);
			*(pst_CurrentGMat - 1) = st_TempGMat;
			_pst_GMatList->pal_Id[pst_CurrentGMat - _pst_GMatList->pst_GMat] = ul_PreviousID;
			_pst_GMatList->pal_Id[pst_CurrentGMat - 1 - _pst_GMatList->pst_GMat] = ul_CurrentID;
			if(pst_CurrentGMat != _pst_GMatList->pst_GMat + 1)
			{
				ul_PreviousID = _pst_GMatList->pal_Id[pst_CurrentGMat - 2 - _pst_GMatList->pst_GMat];
				pst_CurrentGMat -= 2;
			}
			else
			{
				pst_CurrentGMat -= 1;
				ul_PreviousID = _pst_GMatList->pal_Id[0];
			}
		}
		else
			ul_PreviousID = ul_CurrentID;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_SaveGameMaterial(COL_tdst_GameMatList *_pst_GMatList, BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Name[BIG_C_MaxLenPath];
	char		*psz_Temp;
	BIG_INDEX	ul_GMatIndex;
	BIG_KEY		ul_GMatKey;
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Key == BIG_C_InvalidIndex)
		ul_GMatKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GMatList);
	else
		ul_GMatKey = _ul_Key;

	ul_GMatIndex = BIG_ul_SearchKeyToFat(ul_GMatKey);
	if(ul_GMatIndex == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(0, "Cannot find Game Material file", NULL);
		return;
	}

	BIG_ComputeFullName(BIG_ParentFile(ul_GMatIndex), asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(ul_GMatIndex));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtCOLGMAT);

	COL_SortGameMaterial(_pst_GMatList);

	SAV_Begin(asz_Path, asz_Name);
	SAV_Buffer(&_pst_GMatList->ul_GMat, 4);
	SAV_Buffer(_pst_GMatList->pal_Id, _pst_GMatList->ul_GMat * 4);

	/* Game Material Version */
	for(i = 0; i < (int) _pst_GMatList->ul_GMat; i++)
	{
		_pst_GMatList->pst_GMat[i].uc_Dummy = 3;
	}

	SAV_Buffer(_pst_GMatList->pst_GMat, _pst_GMatList->ul_GMat * sizeof(COL_tdst_GameMat));
	SAV_ul_End();
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_Save_Nodes_Recursively(COL_tdst_OK3_Node *_pst_Node, COL_tdst_OK3 *_pst_OK3)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if(!_pst_Node) return;

	if(_pst_Node->pst_OK3_Box)
	{
		val = _pst_Node->pst_OK3_Box - _pst_OK3->pst_OK3_Boxes;
		SAV_Buffer(&val, 4);
	}

	if(_pst_Node->pst_Son)
	{
		val = 1;					/* Tag to indicate Son */
		SAV_Buffer(&val, 4);
		COL_Save_Nodes_Recursively(_pst_Node->pst_Son, _pst_OK3);
	}

	if(_pst_Node->pst_Next)
	{
		val = 2;					/* Tag to indicate Next */
		SAV_Buffer(&val, 4);
		COL_Save_Nodes_Recursively(_pst_Node->pst_Next, _pst_OK3);
	}

	val = 0;						/* Tag to indicate end node */
	SAV_Buffer(&val, 4);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_SaveCob(COL_tdst_Cob *_pst_Cob, BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Name[BIG_C_MaxLenName];
	char		*psz_Temp;
	BIG_INDEX	ul_CobIndex, ul_GMatIndex;
	BIG_KEY		ul_CobKey, ul_GMatKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Key == BIG_C_InvalidIndex)
		ul_CobKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Cob);
	else
		ul_CobKey = _ul_Key;

	ul_CobIndex = BIG_ul_SearchKeyToFat(ul_CobKey);
	if(ul_CobIndex == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(ul_CobIndex != BIG_C_InvalidIndex, "Cannot find COB file", NULL);
		return;
	}

	if(_pst_Cob->uc_Flag & COL_C_Cob_Updated) COL_SynchronizeCob(_pst_Cob, FALSE, FALSE);

#ifdef ACTIVE_EDITORS
	COL_UpdateCameraNumberOfFaces(_pst_Cob);
#endif

	if(_pst_Cob->pst_GMatList)
	{
		ul_GMatKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Cob->pst_GMatList);
		if(ul_GMatKey == BIG_C_InvalidKey)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			char	asz_PathToGMat[BIG_C_MaxLenPath];
			char	asz_Name[BIG_C_MaxLenName];
			char	*psz_Temp;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/* We want to create one Game Material file. */
			L_strcpy(asz_PathToGMat, EDI_Csz_Path_COLGameMaterials);

			L_strcpy(asz_Name, BIG_NameFile(ul_CobIndex));
/*
			psz_Inst = L_strrchr(asz_Name, '@');
			if(psz_Inst)
				*psz_Inst = 0;
			else
*/
			{
				psz_Temp = L_strrchr(asz_Name, '.');
				if(psz_Temp) *psz_Temp = 0;
			}
			L_strcat(asz_Name, EDI_Csz_ExtCOLGMAT);

			if(BIG_ul_SearchFileExt(asz_PathToGMat, asz_Name) != BIG_C_InvalidIndex)
			{
				/*~~~~~~~~~~~~~~~~~*/
				char	asz_Log[150];
				/*~~~~~~~~~~~~~~~~~*/

				sprintf(asz_Log, "%s already exist in %s", asz_Name, asz_PathToGMat);
				ul_GMatKey = BIG_C_InvalidKey;
				ERR_X_Warning(0, asz_Log, NULL);
			}
			else
			{
				ul_GMatIndex = BIG_ul_CreateFile(asz_PathToGMat, asz_Name);
				LOA_AddAddress(ul_GMatIndex, _pst_Cob->pst_GMatList);
				ul_GMatKey = BIG_FileKey(ul_GMatIndex);
				COL_SaveGameMaterial(_pst_Cob->pst_GMatList, BIG_C_InvalidIndex);
			}
		}
		else
			COL_SaveGameMaterial(_pst_Cob->pst_GMatList, BIG_C_InvalidIndex);
	}
	else
		ul_GMatKey = BIG_C_InvalidKey;

	/* Update the Game Material file Name. */
	COL_GMat_UpdateFileName(_pst_Cob);

	BIG_ComputeFullName(BIG_ParentFile(ul_CobIndex), asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(ul_CobIndex));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtCOLObject);

	SAV_Begin(asz_Path, asz_Name);

	SAV_Buffer(&ul_GMatKey, sizeof(BIG_KEY));

	SAV_Buffer(&_pst_Cob->uc_Type, 1);

	/* Remove Hypothetical OK3 Flag if needed. */
	if((_pst_Cob->uc_Type == COL_C_Zone_Triangles) && !(_pst_Cob->pst_TriangleCob->pst_OK3))
		_pst_Cob->uc_Flag &= ~COL_C_Cob_OK3;

	if(EDI_gb_ComputeMap)
		_pst_Cob->uc_Flag |= COL_C_Cob_Updated;	/* Fake use of this Flag to indicate that the preprocess has been completed */
	else
		_pst_Cob->uc_Flag &= ~COL_C_Cob_Updated;


	SAV_Buffer(&_pst_Cob->uc_Flag, 1);

	switch(COL_Cob_GetType(_pst_Cob))
	{
	case COL_C_Zone_Sphere:
		SAV_Buffer(&_pst_Cob->pst_MathCob->l_MaterialId, sizeof(LONG));
		SAV_Buffer(COL_pst_Shape_GetCenter(_pst_Cob->pst_MathCob->p_Shape), sizeof(MATH_tdst_Vector));
		SAV_Buffer(&((COL_tdst_Sphere *) _pst_Cob->pst_MathCob->p_Shape)->f_Radius, sizeof(float));
		break;

	case COL_C_Zone_Box:
		SAV_Buffer(&_pst_Cob->pst_MathCob->l_MaterialId, sizeof(LONG));
		SAV_Buffer(COL_pst_Shape_GetMax(_pst_Cob->pst_MathCob->p_Shape), sizeof(MATH_tdst_Vector));
		SAV_Buffer(COL_pst_Shape_GetMin(_pst_Cob->pst_MathCob->p_Shape), sizeof(MATH_tdst_Vector));
		break;

	case COL_C_Zone_Cylinder:
		SAV_Buffer(&_pst_Cob->pst_MathCob->l_MaterialId, sizeof(LONG));
		SAV_Buffer(COL_pst_Shape_GetCenter(_pst_Cob->pst_MathCob->p_Shape), sizeof(MATH_tdst_Vector));
		SAV_Buffer(&((COL_tdst_Cylinder *) _pst_Cob->pst_MathCob->p_Shape)->f_Radius, sizeof(float));
		SAV_Buffer(&((COL_tdst_Cylinder *) _pst_Cob->pst_MathCob->p_Shape)->f_Height, sizeof(float));
		break;

	case COL_C_Zone_Triangles:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_IndexedTriangles			*pst_Col;
			COL_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
			int									j, k;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Col = _pst_Cob->pst_TriangleCob;
			SAV_Buffer(&pst_Col->l_NbPoints, 4);
			SAV_Buffer(pst_Col->dst_Point, pst_Col->l_NbPoints * sizeof(MATH_tdst_Vector));
			SAV_Buffer(&pst_Col->l_NbFaces, 4);
			SAV_Buffer(pst_Col->dst_FaceNormal, pst_Col->l_NbFaces * sizeof(MATH_tdst_Vector));
			SAV_Buffer(&pst_Col->l_NbElements, 4);
			pst_Element = pst_Col->dst_Element;
			pst_LastElement = pst_Element + pst_Col->l_NbElements;
			for(; pst_Element < pst_LastElement; pst_Element++)
			{
				SAV_Buffer(&pst_Element->uw_NbTriangles, 2);
				SAV_Buffer(&pst_Element->uc_Design, 1);
				SAV_Buffer(&pst_Element->uc_Flag, 1);
				SAV_Buffer(&pst_Element->l_MaterialId, 4);
				for(j = 0; j < pst_Element->uw_NbTriangles; j++)
				{
					SAV_Buffer(&pst_Element->dst_Triangle[j].auw_Index[0], 6);
				}
			}

			/* Save Proximity in Binarization Process */
			if(EDI_gb_ComputeMap)
			{
				pst_Element = pst_Col->dst_Element;
				pst_LastElement = pst_Element + pst_Col->l_NbElements;
				for(; pst_Element < pst_LastElement; pst_Element++)
				{
					for(j = 0; j < pst_Element->uw_NbTriangles; j++)
					{
						SAV_Buffer(&pst_Element->dst_Triangle[j].auw_Prox[0], 6);
					}
				}
			}

			/* Save OK3 only in Binarization Process */
			if((pst_Col->pst_OK3) && EDI_gb_ComputeMap)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				COL_tdst_OK3_Box		*pst_OK3_Box;
				COL_tdst_OK3_Element	*pst_OK3_Element;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				SAV_Buffer(&pst_Col->pst_OK3->ul_NumBox, 4);
				for(j = 0; j < (int) pst_Col->pst_OK3->ul_NumBox; j++)
				{
					pst_OK3_Box = &pst_Col->pst_OK3->pst_OK3_Boxes[j];

					SAV_Buffer(&pst_OK3_Box->ul_NumElement, 4);
					SAV_Buffer(&pst_OK3_Box->st_Max, 12);
					SAV_Buffer(&pst_OK3_Box->st_Min, 12);

					for(k = 0; k < (int) pst_OK3_Box->ul_NumElement; k++)
					{
						pst_OK3_Element = &pst_OK3_Box->pst_OK3_Element[k];

						SAV_Buffer(&pst_OK3_Element->uw_Element, 2);
						SAV_Buffer(&pst_OK3_Element->uw_NumTriangle, 2);
						SAV_Buffer(pst_OK3_Element->puw_OK3_Triangle, pst_OK3_Element->uw_NumTriangle * sizeof(USHORT));
					}				
				}

				/* Save OK3 Hierarchy */
				COL_Save_Nodes_Recursively(pst_Col->pst_OK3->pst_OK3_God, pst_Col->pst_OK3);
			}
		}
		break;
	}

	SAV_ul_End();
}

/*
 =======================================================================================================================
    Aim:    Saves the entire ColMap of an object. Activations, Flags and all the Cob's Key.
 =======================================================================================================================
 */
void COL_SaveColMap(COL_tdst_ColMap *_pst_ColMap, BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob	**dpst_Cob, **dpst_LastCob;
	char			asz_Path[BIG_C_MaxLenPath];
	char			asz_Name[BIG_C_MaxLenName];
	char			*psz_Temp;
	UCHAR			uc_Temp;
	BIG_INDEX		ul_ColMapIndex, ul_CobIndex;
	BIG_KEY			ul_ColMapKey, ul_CobKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Key == BIG_C_InvalidIndex)
		ul_ColMapKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_ColMap);
	else
		ul_ColMapKey = _ul_Key;

	ul_ColMapIndex = BIG_ul_SearchKeyToFat(ul_ColMapKey);

	if(ul_ColMapIndex == BIG_C_InvalidIndex) return;

	/*
	 * If the Cob files does not exist ... we create them .. We do it first because we
	 * cannot begin a Save within a Save.
	 */
	dpst_Cob = _pst_ColMap->dpst_Cob;
	dpst_LastCob = dpst_Cob + _pst_ColMap->uc_NbOfCob;
	for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
	{
		ul_CobKey = LOA_ul_SearchKeyWithAddress((ULONG) (*dpst_Cob));
		if(ul_CobKey == BIG_C_InvalidKey)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			char	asz_PathToCob[BIG_C_MaxLenPath];
			char	asz_Name[BIG_C_MaxLenPath];
//			char	*psz_Temp, *psz_Inst;
			ULONG	ul_Dummy;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/* We want to create one Cob file. */
			BIG_ComputeFullName(BIG_ParentDir(BIG_ParentFile(ul_ColMapIndex)), asz_PathToCob);
			L_strcat(asz_PathToCob, "/");
			L_strcat(asz_PathToCob, EDI_Csz_Path_COLObjects);

			L_strcpy(asz_Name, BIG_NameFile(ul_ColMapIndex));
/*
			psz_Inst = L_strrchr(asz_Name, '@');
			if(psz_Inst)
				*psz_Inst = 0;
			else
*/
			{
				psz_Temp = L_strrchr(asz_Name, '.');
				if(psz_Temp) *psz_Temp = 0;
			}


			L_strcat(asz_Name, EDI_Csz_ExtCOLObject);

			SAV_Begin(asz_PathToCob, asz_Name);
			ul_Dummy = 0;
			SAV_Buffer(&ul_Dummy, sizeof(ULONG));
			ul_CobIndex = SAV_ul_End();
			LOA_AddAddress(ul_CobIndex, *dpst_Cob);
			ul_CobKey = BIG_FileKey(ul_CobIndex);
		}
	}
#if !defined(XML_CONV_TOOL)
	/* Real ColMap Save */
	BIG_ComputeFullName(BIG_ParentFile(ul_ColMapIndex), asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(ul_ColMapIndex));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtCOLMap);
#endif
	SAV_Begin(asz_Path, asz_Name);

	SAV_Buffer(&_pst_ColMap->uc_NbOfCob, 1);

	/* Save all Cobs activated. */
	uc_Temp = 0xFF;
	SAV_Buffer(&uc_Temp, sizeof(UCHAR));

	SAV_Buffer(&_pst_ColMap->uc_CustomBits1, 1);
	SAV_Buffer(&_pst_ColMap->uc_CustomBits2, 1);

	/* Saves the keys in the ColMap File. */
	dpst_Cob = _pst_ColMap->dpst_Cob;
	dpst_LastCob = dpst_Cob + _pst_ColMap->uc_NbOfCob;
	for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
	{
		ul_CobKey = LOA_ul_SearchKeyWithAddress((ULONG) (*dpst_Cob));
		SAV_Buffer(&ul_CobKey, sizeof(BIG_KEY));
	}

	SAV_ul_End();

	/* Save all the Cobs */
	dpst_Cob = _pst_ColMap->dpst_Cob;
	dpst_LastCob = dpst_Cob + _pst_ColMap->uc_NbOfCob;
	for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
	{
		ul_CobKey = LOA_ul_SearchKeyWithAddress((ULONG) (*dpst_Cob));
		if(ul_CobKey != BIG_C_InvalidKey) COL_SaveCob(*dpst_Cob, BIG_C_InvalidKey);
	}
}

/*
 =======================================================================================================================
    Aim:    Save this ColSet.

    Note:   The _ul_Key is there if you want to overwrite a existing file.
 =======================================================================================================================
 */
void COL_SaveColSet(COL_tdst_ColSet *_pst_ColSet, BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			asz_Path[BIG_C_MaxLenPath];
	char			asz_Name[BIG_C_MaxLenPath];
	char			*psz_Temp;
	ULONG			ul_Type, ul_Length;
	UCHAR			uc_Index;
	BIG_KEY			ul_ColSetKey;
	BIG_INDEX		ul_ColSetIndex;
	COL_tdst_ZDx	*pst_CurrentZDx, *pst_LastZDx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Key == BIG_C_InvalidIndex)
		ul_ColSetKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_ColSet);
	else
		ul_ColSetKey = _ul_Key;
	ul_ColSetIndex = BIG_ul_SearchKeyToFat(ul_ColSetKey);
	if(ul_ColSetIndex == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(ul_ColSetIndex != BIG_C_InvalidIndex, "Cannot find ColSet file", NULL);
		return;
	}
#if !defined(XML_CONV_TOOL)
	BIG_ComputeFullName(BIG_ParentFile(ul_ColSetIndex), asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(ul_ColSetIndex));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtCOLSetModel);
#endif
	SAV_Begin(asz_Path, asz_Name);
	SAV_Buffer(&_pst_ColSet->uc_NbOfZDx, sizeof(UCHAR));
	SAV_Buffer(&_pst_ColSet->uc_Flag, sizeof(UCHAR));
	SAV_Buffer(&_pst_ColSet->uw_NbOfInstances, sizeof(USHORT));

	/* Save the IA-ENG translation array */
	for(uc_Index = 0; uc_Index < COL_Cul_MaxNbOfZDx; uc_Index++)
		SAV_Buffer(&_pst_ColSet->pauc_AI_Indexes[uc_Index], sizeof(UCHAR));

	pst_CurrentZDx = _pst_ColSet->past_ZDx;
	pst_LastZDx = pst_CurrentZDx + _pst_ColSet->uc_NbOfZDx;
	for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
	{
		SAV_Buffer(&pst_CurrentZDx->uc_Flag, sizeof(UCHAR));
		SAV_Buffer(&pst_CurrentZDx->uc_Type, sizeof(UCHAR));
		SAV_Buffer(&pst_CurrentZDx->uc_BoneIndex, sizeof(UCHAR));
		SAV_Buffer(&pst_CurrentZDx->uc_Design, sizeof(UCHAR));
		if(pst_CurrentZDx->sz_Name)
		{
			ul_Length = L_strlen(pst_CurrentZDx->sz_Name) + 1;
			SAV_Buffer(&ul_Length, 4);
			SAV_Buffer(pst_CurrentZDx->sz_Name, ul_Length);
		}

		SAV_Buffer(&pst_CurrentZDx->uc_AI_Index, sizeof(UCHAR));

		ul_Type = COL_Zone_GetType(pst_CurrentZDx);
		switch(ul_Type)
		{
		case COL_C_Zone_Sphere:
			SAV_Buffer(COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape), sizeof(MATH_tdst_Vector));
			SAV_Buffer(&((COL_tdst_Sphere *) pst_CurrentZDx->p_Shape)->f_Radius, sizeof(float));
			break;
		case COL_C_Zone_Box:
			SAV_Buffer(COL_pst_Shape_GetMax(pst_CurrentZDx->p_Shape), sizeof(MATH_tdst_Vector));
			SAV_Buffer(COL_pst_Shape_GetMin(pst_CurrentZDx->p_Shape), sizeof(MATH_tdst_Vector));
			break;
		case COL_C_Zone_Cylinder:
			SAV_Buffer(COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape), sizeof(MATH_tdst_Vector));
			SAV_Buffer(&((COL_tdst_Cylinder *) pst_CurrentZDx->p_Shape)->f_Radius, sizeof(float));
			SAV_Buffer(&((COL_tdst_Cylinder *) pst_CurrentZDx->p_Shape)->f_Height, sizeof(float));
			break;
		case COL_C_Zone_Cone:
			break;
		}
	}

	SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_SaveInstance(COL_tdst_Instance *_pst_Instance, BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			asz_Path[BIG_C_MaxLenPath];
	char			asz_Name[BIG_C_MaxLenPath];
	char			*psz_Temp;
	USHORT			uw_Temp;
	UCHAR			uc_Index, uc_Rank;
	ULONG			ul_Length;
	BIG_KEY			ul_InstanceKey, ul_ColSetKey;
	BIG_INDEX		ul_InstanceIndex;
	COL_tdst_ZDx	*pst_CurrentZDx, *pst_LastZDx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Key == BIG_C_InvalidIndex)
		ul_InstanceKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Instance);
	else
		ul_InstanceKey = _ul_Key;

	if(ul_InstanceKey == BIG_C_InvalidIndex) return;
	ul_InstanceIndex = BIG_ul_SearchKeyToFat(ul_InstanceKey);
	if(ul_InstanceIndex == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(ul_InstanceIndex != BIG_C_InvalidIndex, "Cannot find Collision Instance file", NULL);
		return;
	}
#if !defined(XML_CONV_TOOL)
	BIG_ComputeFullName(BIG_ParentFile(ul_InstanceIndex), asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(ul_InstanceIndex));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtCOLInstance);

	COL_SaveColSet(_pst_Instance->pst_ColSet, BIG_C_InvalidIndex);
#endif
	SAV_Begin(asz_Path, asz_Name);
	if(_pst_Instance->pst_ColSet)
		ul_ColSetKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Instance->pst_ColSet);
	else
		ul_ColSetKey = BIG_C_InvalidIndex;
	SAV_Buffer(&ul_ColSetKey, sizeof(BIG_KEY));

	SAV_Buffer(&_pst_Instance->uc_NbOfZDx, sizeof(UCHAR));
	SAV_Buffer(&_pst_Instance->uc_NbOfShared, sizeof(UCHAR));
	SAV_Buffer(&_pst_Instance->uc_NbOfSpecific, sizeof(UCHAR));
	SAV_Buffer(&_pst_Instance->c_Priority, sizeof(CHAR));

	for(uc_Index = 0; uc_Index < _pst_Instance->uc_NbOfZDx; uc_Index++)
	{
		if(COL_b_Instance_IsSpecific(_pst_Instance, uc_Index))
		{
			uc_Rank = COL_uc_Instance_GetSpecificIndexWithZone(_pst_Instance, *(_pst_Instance->dpst_ZDx + uc_Index));
			SAV_Buffer(&uc_Rank, sizeof(UCHAR));
		}
		else
		{
			uc_Rank = COL_uc_ColSet_GetIndexWithZone(_pst_Instance->pst_ColSet, *(_pst_Instance->dpst_ZDx + uc_Index));
			SAV_Buffer(&uc_Rank, sizeof(UCHAR));
		}
	}

	/* Save the instance specific zone array. */
	if(_pst_Instance->uc_NbOfSpecific)
	{
		pst_CurrentZDx = _pst_Instance->past_Specific;
		pst_LastZDx = pst_CurrentZDx + _pst_Instance->uc_NbOfSpecific;
		for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
		{
			SAV_Buffer(&pst_CurrentZDx->uc_Flag, sizeof(UCHAR));
			SAV_Buffer(&pst_CurrentZDx->uc_Type, sizeof(UCHAR));
			SAV_Buffer(&pst_CurrentZDx->uc_BoneIndex, sizeof(UCHAR));
			SAV_Buffer(&pst_CurrentZDx->uc_Design, sizeof(UCHAR));

			/* We want to be able to come back to the ColSet index. */
			uc_Rank = COL_uc_ColSet_GetIndexWithZone(_pst_Instance->pst_ColSet, pst_CurrentZDx->pst_ColSetZDx);
			SAV_Buffer(&uc_Rank, sizeof(UCHAR));

			if(pst_CurrentZDx->sz_Name)
			{
				ul_Length = L_strlen(pst_CurrentZDx->sz_Name) + 1;
				SAV_Buffer(&ul_Length, 4);
				SAV_Buffer(pst_CurrentZDx->sz_Name, ul_Length);
			}

			SAV_Buffer(&pst_CurrentZDx->uc_AI_Index, sizeof(UCHAR));

			switch(pst_CurrentZDx->uc_Type)
			{
/*$off*/
			case COL_C_Zone_Sphere:
#ifdef _DEBUG
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	*pst_Center;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if
				(
					(COL_f_Shape_GetRadius(pst_CurrentZDx->p_Shape) > 10000.0f) ||
					(COL_f_Shape_GetRadius(pst_CurrentZDx->p_Shape) < 0.0f)
				)
				{
					ERR_X_Warning(0, "Collision Zone Radius over 10000m", NULL);
				}

				pst_Center = COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape);
				if
				(
					(pst_Center->x > 10000.0f) ||
					(pst_Center->x < -10000.0f) ||
					(pst_Center->y > 10000.0f) ||
					(pst_Center->y < -10000.0f) ||
					(pst_Center->z > 10000.0f) ||
					(pst_Center->z < -10000.0f)
				)
				{
					ERR_X_Warning(0, "Collision Zone Center invalid", NULL);
				}
			}

#endif /* _DEBUG */

				SAV_Buffer(COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape), sizeof(MATH_tdst_Vector));
				SAV_Buffer(&((COL_tdst_Sphere *) pst_CurrentZDx->p_Shape)->f_Radius, sizeof(float));
				break;
			case COL_C_Zone_Box:
				SAV_Buffer(COL_pst_Shape_GetMax(pst_CurrentZDx->p_Shape), sizeof(MATH_tdst_Vector));
				SAV_Buffer(COL_pst_Shape_GetMin(pst_CurrentZDx->p_Shape), sizeof(MATH_tdst_Vector));
				break;
			case COL_C_Zone_Cylinder:
				SAV_Buffer(COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape), sizeof(MATH_tdst_Vector));
				SAV_Buffer(&((COL_tdst_Cylinder *) pst_CurrentZDx->p_Shape)->f_Radius, sizeof(float));
				SAV_Buffer(&((COL_tdst_Cylinder *) pst_CurrentZDx->p_Shape)->f_Height, sizeof(float));
				break;
			case COL_C_Zone_Cone:
			break;
			/*$on*/
			}
		}
	}

	/* We want to save all the Activation On. */
	uw_Temp = 0xFFFF;
	SAV_Buffer(&uw_Temp, sizeof(USHORT));

	SAV_Buffer(&_pst_Instance->uw_Specific, sizeof(USHORT));

	/* Before the AllowChange Field was saved. Field erased */
	SAV_Buffer(&uw_Temp, sizeof(USHORT));

	SAV_Buffer(&_pst_Instance->uw_Crossable, sizeof(USHORT));
	SAV_Buffer(&_pst_Instance->uw_Flags, sizeof(USHORT));
	SAV_Buffer(&_pst_Instance->uc_MaxLOD, sizeof(UCHAR));
	SAV_Buffer(&_pst_Instance->uc_MinLOD, sizeof(UCHAR));

	SAV_ul_End();
}

/*$F
 ===================================================================================================
    Aim:    This function returns a BigFile Index to an nearly empty instance file.
			In fact, the file is not really empty: The BIG_KEY of the ColSet file is at 
			the very beginning of the file ---> Just 4 bytes are allocated.

    In:     - The world in which the instance is created in. 
			- The Bigfile Index to the ColSet file.
            - The GameObject name (.gao) of this instance
            - The ColSet name of this instance (can be NULL if the BigFile Index != BIG_C_InvalidIndex)
 ===================================================================================================
 */

BIG_KEY COL_ul_CreateInstanceFile
(
	WOR_tdst_World	*_pst_World,
	BIG_KEY			*_pul_ColSetKey,
	char			*_psz_GO_Name,
	char			*_psz_ColSet_Name
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_ColSetFile, ul_InstanceFile;
	char		asz_PathToColSet[BIG_C_MaxLenPath];
	char		asz_PathToInstance[BIG_C_MaxLenPath];
	char		asz_Name[BIG_C_MaxLenName];
	char		asz_InstanceName[BIG_C_MaxLenName];
	char		*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(*_pul_ColSetKey == BIG_C_InvalidKey)
	{
		L_strcpy(asz_PathToColSet, EDI_Csz_Path_COLModels);
		L_strcpy(asz_Name, _psz_ColSet_Name);
		L_strcat(asz_Name, EDI_Csz_ExtCOLSetModel);
		BIG_ul_CreateDir(asz_PathToColSet);
		ul_ColSetFile = BIG_ul_CreateFile(asz_PathToColSet, asz_Name);
		*_pul_ColSetKey = BIG_FileKey(ul_ColSetFile);
	}
	else
		ul_ColSetFile = BIG_ul_SearchKeyToFat(*_pul_ColSetKey);

	if(_pst_World)
	{
		WOR_GetSubPath(_pst_World, EDI_Csz_Path_COLInstances, asz_PathToInstance);
		L_strcat(asz_PathToInstance, "/");

		L_strcpy(asz_Name, BIG_NameFile(ul_ColSetFile));
		psz_Temp = L_strrchr(asz_Name, '.');
		if(psz_Temp) *psz_Temp = 0;
		L_strcat(asz_PathToInstance, asz_Name);

		L_strcpy(asz_InstanceName, _psz_GO_Name);
		psz_Temp = L_strrchr(asz_InstanceName, '.');
		if(psz_Temp) *psz_Temp = 0;
		L_strcat(asz_InstanceName, EDI_Csz_ExtCOLInstance);
	}
	else
		return BIG_C_InvalidIndex;

	SAV_Begin(asz_PathToInstance, asz_InstanceName);
	SAV_Buffer((_pul_ColSetKey), sizeof(BIG_INDEX));
	ul_InstanceFile = SAV_ul_End();

	LINK_FatHasChanged();
	return BIG_FileKey(ul_InstanceFile);
}

/*
 =======================================================================================================================
    Aim:    Creates the ColMap file of an object after a Drag and Drop. This file will be stored in the "COL Instances"
            directory of the Level.

    Note:   The File only contains a Key to the Collision object.
 =======================================================================================================================
 */
BIG_KEY COL_ul_CreateColMapFile(WOR_tdst_World *_pst_World, BIG_INDEX _ul_Cob, char *_psz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_File;
	char		asz_PathToColMap[BIG_C_MaxLenPath];
	char		asz_ColMapName[BIG_C_MaxLenName];
	char		*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_World)
	{
		WOR_GetSubPath(_pst_World, EDI_Csz_Path_COLInstances, asz_PathToColMap);
		L_strcpy(asz_ColMapName, _psz_Name);
		psz_Temp = L_strrchr(asz_ColMapName, '.');
		if(psz_Temp) *psz_Temp = 0;
		L_strcat(asz_ColMapName, EDI_Csz_ExtCOLMap);
	}
	else
		return BIG_C_InvalidIndex;

	SAV_Begin(asz_PathToColMap, asz_ColMapName);

	/* We save the key of the Cob. */
	SAV_Buffer(&BIG_FileKey(_ul_Cob), 4);

	ul_File = SAV_ul_End();
	return BIG_FileKey(ul_File);
}

/*
 =======================================================================================================================
    Aim:    Creates a Cob file . This file will be stored in the "COL Objects" directory of the Level.
 =======================================================================================================================
 */
BIG_KEY COL_ul_CreateCobFile(WOR_tdst_World *_pst_World, char *_psz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_CobFile;
	char		asz_PathToCob[BIG_C_MaxLenPath];
	char		asz_CobName[BIG_C_MaxLenName];
	char		*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_World)
	{
		WOR_GetSubPath(_pst_World, EDI_Csz_Path_COLObjects, asz_PathToCob);
		L_strcpy(asz_CobName, _psz_Name);
/*
		psz_Inst = L_strrchr(asz_CobName, '@');
		if(psz_Inst)
			*psz_Inst = 0;
		else
*/
		{
			psz_Temp = L_strrchr(asz_CobName, '.');
			if(psz_Temp) *psz_Temp = 0;
		}

		L_strcat(asz_CobName, EDI_Csz_ExtCOLObject);
	}
	else
		return BIG_C_InvalidIndex;

	BIG_ul_CreateDir(asz_PathToCob);
	ul_CobFile = BIG_ul_CreateFile(asz_PathToCob, asz_CobName);

	LINK_FatHasChanged();
	return BIG_FileKey(ul_CobFile);
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

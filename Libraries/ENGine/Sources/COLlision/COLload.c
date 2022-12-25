/*$T COLload.c GC! 1.081 11/28/01 12:31:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$F GC Dependencies 11/22/99 */
#include "Precomp.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"

#include "MATHs/MATH.h"

#include "OBJects/OBJstruct.h"
#include "OBJects/OBJconst.h"
#include "OBJects/OBJaccess.h"
#include "OBJects/OBJorient.h"

#include "COLstruct.h"
#include "COLconst.h"
#include "COLaccess.h"
#include "COLedit.h"
#include "COLinit.h"
#include "COLcob.h"
#include "MATHs/MATH_MEM.h"

#include "INTersection/INTmain.h"

#ifdef ACTIVE_EDITORS
#include "COLset.h"
#include "WORld/WORsave.h"
#include "EDIPaths.h"
#include "GraphicDK/Sources/GEOmetric/GEOload.h"
#endif /* ACTIVE_EDITORS */

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
extern		COL_ColMap_Check(OBJ_tdst_GameObject *);
extern char *BIG_FileName(BIG_INDEX);
#endif

#ifdef JADEFUSION
void COL_OK3_Build(void *_p_Data, BOOL _b_ColMap, BOOL _b_ReBuild);
#else
void		COL_OK3_Build(void *, BOOL, BOOL);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_ul_CallBackLoadGameMaterial(ULONG _ul_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR					*pc_Buffer;
	ULONG					ul_Size;
	COL_tdst_GameMatList	*pst_GMatList;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Load the file. */
	pc_Buffer = BIG_pc_ReadFileTmp(_ul_Pos, &ul_Size);

	pst_GMatList = (COL_tdst_GameMatList *) MEM_p_VMAlloc(sizeof(COL_tdst_GameMatList));
	L_memset(pst_GMatList, 0, sizeof(COL_tdst_GameMatList));

	pst_GMatList->ul_GMat = LOA_ReadULong(&pc_Buffer);

#ifdef ACTIVE_EDITORS
	if(pst_GMatList->ul_GMat > 50000)
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[200];
		/*~~~~~~~~~~~~~~~~~*/

		sprintf(asz_Log, "Game Material %x File corrupted ", LOA_ul_GetCurrentKey());
		ERR_X_ForceError(asz_Log, NULL);

		MEM_Free(pst_GMatList);
		return NULL;
	}

#endif
	if(pst_GMatList->ul_GMat)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG i;
		ULONG *pPalID;
		COL_tdst_GameMat *pGMat;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_GMatList->pal_Id = (ULONG *) MEM_p_VMAlloc(pst_GMatList->ul_GMat * sizeof(ULONG));
		pst_GMatList->pst_GMat = (COL_tdst_GameMat *) MEM_p_VMAlloc(pst_GMatList->ul_GMat * sizeof(COL_tdst_GameMat));
		for(i = 0, pPalID = pst_GMatList->pal_Id; i < pst_GMatList->ul_GMat; ++i, ++pPalID)
		{
			*pPalID = LOA_ReadULong(&pc_Buffer);
		}

		for(i = 0, pGMat = pst_GMatList->pst_GMat; i < pst_GMatList->ul_GMat; ++i, ++pGMat)
		{
			pGMat->ul_CustomBits = LOA_ReadULong(&pc_Buffer);
			pGMat->f_Slide = LOA_ReadFloat(&pc_Buffer);
			pGMat->f_Rebound = LOA_ReadFloat(&pc_Buffer);
			pGMat->uc_Sound = LOA_ReadUChar(&pc_Buffer);
			pGMat->uc_Dummy = LOA_ReadUChar(&pc_Buffer);
			pGMat->uw_Dummy = LOA_ReadUShort(&pc_Buffer);
#ifdef ACTIVE_EDITORS
            pGMat->b_Transparent = FALSE;
#endif //ACTIVE_EDITORS
			if(pGMat->uc_Dummy > 0)
			{
				LOA_ReadString_Ed(&pc_Buffer, pGMat->asz_Comment, 64);
				LOA_ReadULong_Ed(&pc_Buffer, (ULONG *) &pGMat->ul_Color);
				if(pGMat->uc_Dummy > 1)
				{
					LOA_ReadULong_Ed(&pc_Buffer, (ULONG *) &pGMat->ul_SoundColor);
				}
#ifdef ACTIVE_EDITORS
				else
					pGMat->ul_SoundColor = 0x00dcad21;
#endif

				LOA_ReadULong_Ed(&pc_Buffer, NULL); // pGMat->b_Display

				if (pGMat->uc_Dummy > 2)
				{
    				LOA_ReadULong_Ed(&pc_Buffer, (ULONG *)&pGMat->b_Transparent); 
                }
			}
#ifdef ACTIVE_EDITORS
			else
			{
				sprintf(pGMat->asz_Comment, "");
				pGMat->ul_Color = 0xFF00FF00;
				pGMat->ul_SoundColor = 0x00dcad21;
			}

			pGMat->b_Display = TRUE;

#endif

		}
	}

#ifdef ACTIVE_EDITORS
	if(pst_GMatList->ul_GMat >= 1)
	{
		/*~~~~~~~~~~~~~~~~~*/
		ULONG	i;
		char	asz_Log[200];
		/*~~~~~~~~~~~~~~~~~*/

		for(i = 0; i < pst_GMatList->ul_GMat - 1; i++)
		{
			if(pst_GMatList->pal_Id[i] > pst_GMatList->pal_Id[i + 1])
			{
				sprintf(asz_Log, "GameMaterialFile %x not sorted. SAVE THE MAP for correction", LOA_ul_GetCurrentKey());
				ERR_X_Warning(0, asz_Log, NULL);
				break;
			}
		}
	}

#endif
	return (ULONG) pst_GMatList;
}

static ULONG	ColMapPoints = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_Load_Nodes_Recursively(COL_tdst_OK3_Node *_pst_Node, COL_tdst_OK3 *_pst_OK3, char **_pp_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	val = LOA_ReadULong(_pp_Buffer);
	_pst_Node->pst_OK3_Box = _pst_OK3->pst_OK3_Boxes + val;

	_pst_Node->pst_Son = NULL;
	_pst_Node->pst_Next = NULL;

	val = LOA_ReadULong(_pp_Buffer);

	/* Load Son */
	if(val == 1)
	{
		_pst_Node->pst_Son = (COL_tdst_OK3_Node *) MEM_p_VMAlloc(sizeof(COL_tdst_OK3_Node));
		L_memset(_pst_Node->pst_Son, 0, sizeof(COL_tdst_OK3_Node));

		COL_Load_Nodes_Recursively(_pst_Node->pst_Son,_pst_OK3, _pp_Buffer);

		val = LOA_ReadULong(_pp_Buffer);
	}

	/* Load Son */
	if(val == 2)
	{
		_pst_Node->pst_Next = (COL_tdst_OK3_Node *) MEM_p_VMAlloc(sizeof(COL_tdst_OK3_Node));
		L_memset(_pst_Node->pst_Next, 0, sizeof(COL_tdst_OK3_Node));

		COL_Load_Nodes_Recursively(_pst_Node->pst_Next,_pst_OK3, _pp_Buffer);

		val = LOA_ReadULong(_pp_Buffer);
	}

	/* Load Son */
	if(val == 0)
	{
		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_ul_CallBackLoadCob(ULONG _ul_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR			*pc_Buffer;
	ULONG			ul_Size;
	COL_tdst_Cob	*pst_Cob;
	void			*p_Shape;
	BIG_KEY			ul_GMatKey;
#ifdef ACTIVE_EDITORS
	BIG_KEY			ul_File;
	char			asz_Path[BIG_C_MaxLenPath];
#endif
	int				j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Load the file. */
	pc_Buffer = BIG_pc_ReadFileTmp(_ul_Pos, &ul_Size);
	pst_Cob = (COL_tdst_Cob *) MEM_p_Alloc(sizeof(COL_tdst_Cob));
	L_memset(pst_Cob, 0, sizeof(COL_tdst_Cob));

	ul_GMatKey = LOA_ReadULong(&pc_Buffer);

	pst_Cob->uc_Type = LOA_ReadUChar(&pc_Buffer);
	pst_Cob->uc_Flag = LOA_ReadUChar(&pc_Buffer);

	pst_Cob->uc_Flag &= ~COL_C_Cob_GameMat;

	if
	(
		(pst_Cob->uc_Type != COL_C_Zone_Box)
	&&	(pst_Cob->uc_Type != COL_C_Zone_Sphere)
	&&	(pst_Cob->uc_Type != COL_C_Zone_Cylinder)
	&&	(pst_Cob->uc_Type != COL_C_Zone_Triangles)
	)
	{
		return (ULONG) NULL;
	}

	if(ul_GMatKey != BIG_C_InvalidKey)
	{
		LOA_MakeFileRef
		(
			ul_GMatKey,
			(ULONG *) &(pst_Cob->pst_GMatList),
			COL_ul_CallBackLoadGameMaterial,
			LOA_C_MustExists
		);

		pst_Cob->uc_Flag |= COL_C_Cob_GameMat;
	}

	/* If the Cob is Mathematical, we must load the Game Material ID */
	switch(pst_Cob->uc_Type)
	{
	case COL_C_Zone_Sphere:
	case COL_C_Zone_Box:
	case COL_C_Zone_Cylinder:
		pst_Cob->pst_MathCob = (COL_tdst_Mathematical *) MEM_p_Alloc(sizeof(COL_tdst_Mathematical));
		L_memset(pst_Cob->pst_MathCob, 0, sizeof(COL_tdst_Mathematical));
		pst_Cob->pst_MathCob->l_MaterialId = LOA_ReadLong(&pc_Buffer);
#ifdef ACTIVE_EDITORS
		pst_Cob->pst_MathCob->pst_Cob = pst_Cob;
#endif
		break;
	}

	switch(pst_Cob->uc_Type)
	{
	case COL_C_Zone_Sphere:
		{
			pst_Cob->pst_MathCob->p_Shape = MEM_p_VMAlloc(sizeof(COL_tdst_Sphere));
			p_Shape = pst_Cob->pst_MathCob->p_Shape;
			LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetCenter(p_Shape));
			((COL_tdst_Sphere *) p_Shape)->f_Radius = LOA_ReadFloat(&pc_Buffer);
		}
		break;

	case COL_C_Zone_Box:
		{
			pst_Cob->pst_MathCob->p_Shape = MEM_p_VMAlloc(sizeof(COL_tdst_Box));
			p_Shape = pst_Cob->pst_MathCob->p_Shape;
			LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetMax(p_Shape));
			LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetMin(p_Shape));
		}
		break;

	case COL_C_Zone_Cylinder:
		{
			pst_Cob->pst_MathCob->p_Shape = MEM_p_VMAlloc(sizeof(COL_tdst_Cylinder));
			p_Shape = pst_Cob->pst_MathCob->p_Shape;
			LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetCenter(p_Shape));
			((COL_tdst_Cylinder *) p_Shape)->f_Radius = LOA_ReadFloat(&pc_Buffer);
			((COL_tdst_Cylinder *) p_Shape)->f_Height = LOA_ReadFloat(&pc_Buffer);
		}
		break;
	case COL_C_Zone_Triangles:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_IndexedTriangles			*pst_Col;
			COL_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
			ULONG								i;
			MATH_tdst_Vector					*pVec;
			BOOL								bin;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Cob->pst_TriangleCob = (COL_tdst_IndexedTriangles *) MEM_p_VMAlloc(sizeof(COL_tdst_IndexedTriangles));
			pst_Col = pst_Cob->pst_TriangleCob;

			pst_Col->pst_OK3 = NULL;

			pst_Col->l_NbPoints = LOA_ReadULong(&pc_Buffer);
			pst_Col->dst_Point = (MATH_tdst_Vector *) MEM_p_VMAlloc(pst_Col->l_NbPoints * sizeof(MATH_tdst_Vector));

			for(i = 0, pVec = pst_Col->dst_Point; i < pst_Col->l_NbPoints; ++i, ++pVec)
			{
				LOA_ReadVector(&pc_Buffer, pVec);
			}

			pst_Col->l_NbFaces = LOA_ReadULong(&pc_Buffer);
			pst_Col->dst_FaceNormal = (MATH_tdst_Vector *) MEM_p_VMAlloc(pst_Col->l_NbFaces * sizeof(MATH_tdst_Vector));

			for(i = 0, pVec = pst_Col->dst_FaceNormal; i < pst_Col->l_NbFaces; ++i, ++pVec)
			{
				LOA_ReadVector(&pc_Buffer, pVec);
			}

			pst_Col->l_NbElements = LOA_ReadULong(&pc_Buffer);
			pst_Col->dst_Element = (COL_tdst_ElementIndexedTriangles *) MEM_p_Alloc(pst_Col->l_NbElements * sizeof(COL_tdst_ElementIndexedTriangles));

			pst_Element = pst_Col->dst_Element;
			pst_LastElement = pst_Element + pst_Col->l_NbElements;
			for(i = 0; pst_Element < pst_LastElement; pst_Element++, i++)
			{
				pst_Element->uw_NbTriangles = LOA_ReadUShort(&pc_Buffer);

				ColMapPoints += pst_Element->uw_NbTriangles;

				pst_Element->uc_Design = LOA_ReadUChar(&pc_Buffer);
				pst_Element->uc_Flag = LOA_ReadUChar(&pc_Buffer);

				pst_Element->l_MaterialId = LOA_ReadLong(&pc_Buffer);

				if(pst_Element->uw_NbTriangles)
				{
					pst_Element->dst_Triangle = (COL_tdst_IndexedTriangle *) MEM_p_VMAlloc(pst_Element->uw_NbTriangles * sizeof(COL_tdst_IndexedTriangle));
					for(j = 0; j < pst_Element->uw_NbTriangles; j++)
					{
						pst_Element->dst_Triangle[j].auw_Index[0] = LOA_ReadUShort(&pc_Buffer);
						pst_Element->dst_Triangle[j].auw_Index[1] = LOA_ReadUShort(&pc_Buffer);
						pst_Element->dst_Triangle[j].auw_Index[2] = LOA_ReadUShort(&pc_Buffer);
					}
				}
				else
					pst_Element->dst_Triangle = NULL;

#ifdef ACTIVE_EDITORS
				pst_Element->pst_Cob = pst_Cob;
#endif
			}

#ifdef ACTIVE_EDITORS
			bin = LOA_IsBinaryData() || LOA_IsBinarizing();
#else
			bin = LOA_IsBinaryData();
#endif

			/* Load Proximity in Binarization Process */
			if((pst_Cob->uc_Flag & COL_C_Cob_Updated) && bin)
			{
				pst_Element = pst_Col->dst_Element;
				pst_LastElement = pst_Element + pst_Col->l_NbElements;
				for(i = 0; pst_Element < pst_LastElement; pst_Element++, i++)
				{
					if(pst_Element->uw_NbTriangles)
					{
						for(j = 0; j < pst_Element->uw_NbTriangles; j++)
						{
							pst_Element->dst_Triangle[j].auw_Prox[0] = LOA_ReadUShort(&pc_Buffer);
							pst_Element->dst_Triangle[j].auw_Prox[1] = LOA_ReadUShort(&pc_Buffer);
							pst_Element->dst_Triangle[j].auw_Prox[2] = LOA_ReadUShort(&pc_Buffer);
						}
					}
				}
			}

			/* Load OK3 in Binarization Process */
			if(((pst_Cob->uc_Flag & (COL_C_Cob_OK3 + COL_C_Cob_Updated)) == (COL_C_Cob_OK3 + COL_C_Cob_Updated)) && bin)
			{
				/*~~~~~~~~*/
				int j, k, l;
				/*~~~~~~~~*/

				pst_Col->pst_OK3 = (COL_tdst_OK3 *) MEM_p_Alloc(sizeof(COL_tdst_OK3));
				pst_Col->pst_OK3->paul_Tag = (ULONG *) MEM_p_Alloc(((pst_Col->l_NbFaces >> 5) + 1) << 2);
				L_memset(pst_Col->pst_OK3->paul_Tag, 0, ((pst_Col->l_NbFaces >> 5) + 1) << 2);

				pst_Col->pst_OK3->ul_NumBox = LOA_ReadULong(&pc_Buffer);
				pst_Col->pst_OK3->pst_OK3_Boxes = (COL_tdst_OK3_Box *) MEM_p_VMAlloc(pst_Col->pst_OK3->ul_NumBox * sizeof(COL_tdst_OK3_Box));

				for(j = 0; j < (int) pst_Col->pst_OK3->ul_NumBox; j++)
				{
					pst_Col->pst_OK3->pst_OK3_Boxes[j].ul_NumElement = LOA_ReadULong(&pc_Buffer);

					LOA_ReadVector(&pc_Buffer, &(pst_Col->pst_OK3->pst_OK3_Boxes[j].st_Max));
					LOA_ReadVector(&pc_Buffer, &(pst_Col->pst_OK3->pst_OK3_Boxes[j].st_Min));

					if(pst_Col->pst_OK3->pst_OK3_Boxes[j].ul_NumElement)
					{
						pst_Col->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element = (COL_tdst_OK3_Element *) MEM_p_VMAlloc(pst_Col->pst_OK3->pst_OK3_Boxes[j].ul_NumElement * sizeof(COL_tdst_OK3_Element));
						for(k = 0; k < (int) pst_Col->pst_OK3->pst_OK3_Boxes[j].ul_NumElement; k++)
						{
							pst_Col->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].uw_Element = LOA_ReadUShort(&pc_Buffer);
							pst_Col->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].uw_NumTriangle = LOA_ReadUShort(&pc_Buffer);
							pst_Col->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].puw_OK3_Triangle = (USHORT *) MEM_p_VMAlloc(pst_Col->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].uw_NumTriangle * sizeof(USHORT));

							for(l = 0; l < pst_Col->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].uw_NumTriangle; l++)
							{
								pst_Col->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].puw_OK3_Triangle[l] = LOA_ReadUShort(&pc_Buffer);
							}
						}
					}
					else
						pst_Col->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element = NULL;

				}

				/* Load Hierarchy */
				pst_Col->pst_OK3->pst_OK3_God = (COL_tdst_OK3_Node *) MEM_p_Alloc(sizeof(COL_tdst_OK3_Node));
				COL_Load_Nodes_Recursively(pst_Col->pst_OK3->pst_OK3_God, pst_Col->pst_OK3, &pc_Buffer);


			}
			else
				pst_Col->pst_OK3 = NULL;

		}
		break;
	}

	pst_Cob->uc_Flag &=  ~COL_C_Cob_Updated;
	/* Init of the instances that contains this Cob. */
	pst_Cob->uw_NbOfInstances = 0;

#ifdef ACTIVE_EDITORS
	pst_Cob->ul_EditedElement = 0xFFFFFFFF;
	pst_Cob->pst_Itself = pst_Cob;
	pst_Cob->p_GeoCob = NULL;

	/*$F---------- Register pointer --------------- */
	ul_File = LOA_ul_GetCurrentKey();
	ul_File = BIG_ul_SearchKeyToFat(ul_File);
	BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Path);
	LINK_RegisterPointer(pst_Cob, LINK_C_Cob, BIG_NameFile(ul_File), asz_Path);

#endif
	return (ULONG) pst_Cob;
}

/*
 =======================================================================================================================
    Aim:    Load the ColMap of an Object.
 =======================================================================================================================
 */
ULONG COL_ul_CallBackLoadColMap(ULONG _ul_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap *pst_ColMap;
	COL_tdst_Cob	**dpst_Cob, **dpst_LastCob;
	ULONG			ul_Size;
	CHAR			*pc_Buffer;
	BIG_KEY			ul_CobKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Load the file. */
	pc_Buffer = BIG_pc_ReadFileTmp(_ul_Pos, &ul_Size);
	pst_ColMap = (COL_tdst_ColMap *) MEM_p_Alloc(sizeof(COL_tdst_ColMap));

	L_memset(pst_ColMap, 0, sizeof(COL_tdst_ColMap));
	pst_ColMap->uc_Activation = 0xFF;

	/*
	 * If the ColMap file is only 4 bytes LONG, that means that only the BIG_KEY to a
	 * Cob has been saved. We just have to load it.
	 */
	if(ul_Size == 4)
	{
		ul_CobKey = LOA_ReadLong(&pc_Buffer);
		pst_ColMap->uc_NbOfCob = 1;
		pst_ColMap->uc_Activation = 0xFF;
		pst_ColMap->uc_CustomBits1 = 0;
		pst_ColMap->uc_CustomBits2 = 0;
		pst_ColMap->dpst_Cob = (COL_tdst_Cob **) MEM_p_VMAlloc(sizeof(COL_tdst_Cob *));
		LOA_MakeFileRef(ul_CobKey, (ULONG *) &(*(pst_ColMap->dpst_Cob)), COL_ul_CallBackLoadCob, LOA_C_MustExists);
	}
	else
	{
		pst_ColMap->uc_NbOfCob = LOA_ReadUChar(&pc_Buffer);
		pst_ColMap->uc_Activation = LOA_ReadUChar(&pc_Buffer);
		pst_ColMap->uc_CustomBits1 = LOA_ReadUChar(&pc_Buffer);
		pst_ColMap->uc_CustomBits2 = LOA_ReadUChar(&pc_Buffer);
		pst_ColMap->dpst_Cob = (COL_tdst_Cob **) MEM_p_Alloc(pst_ColMap->uc_NbOfCob * sizeof(COL_tdst_Cob *));
        L_memset(pst_ColMap->dpst_Cob, 0, pst_ColMap->uc_NbOfCob * sizeof(COL_tdst_Cob *));

		dpst_Cob = pst_ColMap->dpst_Cob;
		dpst_LastCob = dpst_Cob + pst_ColMap->uc_NbOfCob;
		for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
		{
			ul_CobKey = LOA_ReadULong(&pc_Buffer);
			if(ul_CobKey != BIG_C_InvalidKey)
				LOA_MakeFileRef(ul_CobKey, (ULONG *) &(*dpst_Cob), COL_ul_CallBackLoadCob, LOA_C_MustExists);
		}
	}

	return (ULONG) pst_ColMap;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_ul_CallBackLoadColSet(ULONG _ul_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet *pst_ColSet;
	COL_tdst_ZDx	*pst_CurrentZDx, *pst_LastZDx;
	UCHAR			uc_Index;
	CHAR			*pc_Buffer;
	ULONG			ul_Size;
	ULONG			ul_Type, ul_Length;
#ifdef ACTIVE_EDITORS
	BIG_KEY			ul_File;
	char			asz_Path[BIG_C_MaxLenPath];

#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Load the file. */
	pc_Buffer = BIG_pc_ReadFileTmp(_ul_Pos, &ul_Size);

	pst_ColSet = COL_AllocColSet();

	pst_ColSet->uc_NbOfZDx = LOA_ReadUChar(&pc_Buffer);
	pst_ColSet->uc_Flag = LOA_ReadUChar(&pc_Buffer);

	/* We do NOT load the uw_NbOfInstances from the BigFile -> Stupid boy */
	pst_ColSet->uw_NbOfInstances = 0;
	LOA_ReadShort_Ed(&pc_Buffer, NULL);

	/* Load the IA-ENG Translation array */
	for(uc_Index = 0; uc_Index < COL_Cul_MaxNbOfZDx; uc_Index++)
		pst_ColSet->pauc_AI_Indexes[uc_Index] = LOA_ReadUChar(&pc_Buffer);

	/* Alloc ColSet array of zones. */
	if(pst_ColSet->uc_NbOfZDx)
		pst_ColSet->past_ZDx = (COL_tdst_ZDx *) MEM_p_VMAlloc(pst_ColSet->uc_NbOfZDx * sizeof(COL_tdst_ZDx));

	pst_CurrentZDx = pst_ColSet->past_ZDx;
	pst_LastZDx = pst_CurrentZDx + pst_ColSet->uc_NbOfZDx;

	/*$F ------------- Loop thru al the zones in that ColSet ------------------- */
	for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
	{
		pst_CurrentZDx->uc_Flag = LOA_ReadUChar(&pc_Buffer);
		pst_CurrentZDx->uc_Type = LOA_ReadUChar(&pc_Buffer);
		pst_CurrentZDx->uc_BoneIndex = LOA_ReadUChar(&pc_Buffer);
		pst_CurrentZDx->uc_Design = LOA_ReadUChar(&pc_Buffer);

		ul_Length = LOA_ReadULong(&pc_Buffer);

		if(ul_Length)
		{
#ifdef ACTIVE_EDITORS
#ifdef JADEFUSION
			pst_CurrentZDx->sz_Name = (char*)MEM_p_VMAlloc(ul_Length);
#else
			pst_CurrentZDx->sz_Name = MEM_p_VMAlloc(ul_Length);
#endif
#endif /* #ifdef ACTIVE_EDITORS */
			LOA_ReadString_Ed(&pc_Buffer, pst_CurrentZDx->sz_Name, ul_Length);
		}

		LOA_ReadUChar_Ed(&pc_Buffer, &pst_CurrentZDx->uc_AI_Index);

#ifdef ACTIVE_EDITORS
		pst_CurrentZDx->pst_GO = NULL;
		pst_CurrentZDx->pst_Itself = pst_CurrentZDx;
#endif
		ul_Type = COL_Zone_GetType(pst_CurrentZDx);
		switch(ul_Type)
		{
		case COL_C_Zone_Sphere:

			/*$F-------Allocation of the Shape ------------ */
			pst_CurrentZDx->p_Shape = MEM_p_VMAlloc(sizeof(COL_tdst_Sphere));

			/*$F--------- Load of the Center -------------- */
			LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape));

			/*$F--------- Load of the Radius -------------- */
			((COL_tdst_Sphere *) pst_CurrentZDx->p_Shape)->f_Radius = LOA_ReadFloat(&pc_Buffer);

#ifdef _DEBUG
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	*pst_Center;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if
				(
					(COL_f_Shape_GetRadius(pst_CurrentZDx->p_Shape) > 300.0f)
				||	(COL_f_Shape_GetRadius(pst_CurrentZDx->p_Shape) < 0.0f)
				)
				{
					ERR_X_Warning(0, "Collision Zone Radius over 300 meters -> Reset to default", NULL);
					((COL_tdst_Sphere *) pst_CurrentZDx->p_Shape)->f_Radius = 1.0f;
				}

				pst_Center = COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape);
				if
				(
					(pst_Center->x > 100000.0f)
				||	(pst_Center->x < -100000.0f)
				||	(pst_Center->y > 100000.0f)
				||	(pst_Center->y < -100000.0f)
				||	(pst_Center->z > 100000.0f)
				||	(pst_Center->z < -100000.0f)
				)
				{
					ERR_X_Warning(0, "Collision Zone Center invalid -> Reset to default", NULL);
					MATH_InitVector(pst_Center, 0.0f, 0.0f, 0.0f);
				}
			}

#endif /* _DEBUG */
			break;

		case COL_C_Zone_Box:

			/*$F-------Allocation of the Shape ------------ */
			pst_CurrentZDx->p_Shape = MEM_p_VMAlloc(sizeof(COL_tdst_Box));

			/*$F--------- Load of the Max -------------- */
			LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetMax(pst_CurrentZDx->p_Shape));

			/*$F--------- Load of the Min -------------- */
			LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetMin(pst_CurrentZDx->p_Shape));
			break;

		case COL_C_Zone_Cylinder:

			/*$F-------Allocation of the Shape ------------ */
			pst_CurrentZDx->p_Shape = MEM_p_VMAlloc(sizeof(COL_tdst_Cylinder));

			/*$F--------- Load of the Center -------------- */
			LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape));

			/*$F--------- Load of the Radius -------------- */
			((COL_tdst_Cylinder *) pst_CurrentZDx->p_Shape)->f_Radius = LOA_ReadFloat(&pc_Buffer);

			/*$F--------- Load of the Height -------------- */
			((COL_tdst_Cylinder *) pst_CurrentZDx->p_Shape)->f_Height = LOA_ReadFloat(&pc_Buffer);
			break;

		case COL_C_Zone_Cone:
			break;
		}

#ifdef ACTIVE_EDITORS
		/*
		for(uc_Index = 0; uc_Index < COL_Cul_MaxNbOfZDx; uc_Index++)
		{
			if((!pst_ColSet->pauc_AI_Indexes[uc_Index]) && (pst_ColSet->past_ZDx->uc_AI_Index != uc_Index))
				pst_ColSet->pauc_AI_Indexes[uc_Index] = 0xFF;
		}
		*/

		pst_CurrentZDx->pst_ColSetZDx = NULL;

		/*$F---------- Register pointer --------------- */
		ul_File = LOA_ul_GetCurrentKey();
		ul_File = BIG_ul_SearchKeyToFat(ul_File);
		BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Path);
		LINK_RegisterPointer(pst_CurrentZDx, LINK_C_Zone, BIG_NameFile(ul_File), asz_Path);
#endif
	}

	return (ULONG) pst_ColSet;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_ul_CallBackLoadInstance(ULONG _ul_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR				*pc_Buffer;
	COL_tdst_ZDx		*pst_CurrentZDx, *pst_LastZDx;
	BIG_KEY				ul_ColSetFile;
	COL_tdst_Instance	*pst_Instance;
	ULONG				ul_Size, ul_Length;
	UCHAR				uc_Index;
#ifdef ACTIVE_EDITORS
	BIG_KEY				ul_File;
	char				asz_Path[BIG_C_MaxLenPath];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Instance = (COL_tdst_Instance *) MEM_p_Alloc(sizeof(COL_tdst_Instance));
	L_memset(pst_Instance, 0, sizeof(COL_tdst_Instance));

	/* Init of the Instance activation. */
	pst_Instance->uw_Activation = 0xFFFF;

	/* Allocation of the Old frame matrix. */
	pst_Instance->pst_OldGlobalMatrix = MATH_Matrix_Alloc();

	/* Load the file. */
	pc_Buffer = BIG_pc_ReadFileTmp(_ul_Pos, &ul_Size);

	ul_ColSetFile = LOA_ReadULong(&pc_Buffer);
	if(ul_ColSetFile != BIG_C_InvalidIndex)
	{
		LOA_MakeFileRef
		(
			ul_ColSetFile,
			(ULONG *) &pst_Instance->pst_ColSet,
			COL_ul_CallBackLoadColSet,
			LOA_C_MustExists
		);
	}

	/*
	 * If the File is only 4 bytes LONG, that means that only the BIG_KEY to a ColSet
	 * has been saved. We just have to load it and return. The init will be done later
	 * in the "After loading' function.
	 */
	if(ul_Size == 4) return (ULONG) pst_Instance;

	pst_Instance->uc_NbOfZDx = LOA_ReadUChar(&pc_Buffer);
	pst_Instance->uc_NbOfShared = LOA_ReadUChar(&pc_Buffer);
	pst_Instance->uc_NbOfSpecific = LOA_ReadUChar(&pc_Buffer);

	if(pst_Instance->uc_NbOfSpecific + pst_Instance->uc_NbOfShared != pst_Instance->uc_NbOfZDx)
	{
		/* Get the Key of the Col Instance; */
		pst_Instance->uc_NbOfZDx = 0;
		pst_Instance->uc_NbOfShared = 0;
		pst_Instance->uc_NbOfSpecific = 0;
		ERR_X_Warning(0, "Col File Corrupted.", NULL);
		return (ULONG) pst_Instance;
	}

	pst_Instance->c_Priority = LOA_ReadChar(&pc_Buffer);

	if(pst_Instance->uc_NbOfZDx)
	{
		pst_Instance->dpst_ZDx = (COL_tdst_ZDx **) MEM_p_VMAlloc(pst_Instance->uc_NbOfZDx * sizeof(COL_tdst_ZDx *));
		L_memset(pst_Instance->dpst_ZDx, 0, pst_Instance->uc_NbOfZDx * sizeof(COL_tdst_ZDx *));
	}

	/*
	 * Load the Instance's array of pointers. NB: During the Load/Save procedure, the
	 * pointers are changed to indexes.
	 */
	for(uc_Index = 0; uc_Index < pst_Instance->uc_NbOfZDx; uc_Index++)
	{
		pst_Instance->dpst_ZDx[uc_Index] = (COL_tdst_ZDx *) LOA_ReadUChar(&pc_Buffer);
	}

	if(pst_Instance->uc_NbOfSpecific)
	{
		pst_Instance->past_Specific = (COL_tdst_ZDx *) MEM_p_VMAlloc(pst_Instance->uc_NbOfSpecific * sizeof(COL_tdst_ZDx));

		pst_CurrentZDx = pst_Instance->past_Specific;
		pst_LastZDx = pst_CurrentZDx + pst_Instance->uc_NbOfSpecific;

		/*$F ------------- Loop thru all the specific zones------------------- */
		for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
		{
			pst_CurrentZDx->uc_Flag = LOA_ReadUChar(&pc_Buffer);
			pst_CurrentZDx->uc_Type = LOA_ReadUChar(&pc_Buffer);
			pst_CurrentZDx->uc_BoneIndex = LOA_ReadUChar(&pc_Buffer);
			pst_CurrentZDx->uc_Design = LOA_ReadUChar(&pc_Buffer);

#ifdef ACTIVE_EDITORS
			pst_CurrentZDx->pst_Itself = pst_CurrentZDx;
#endif
			LOA_ReadUChar_Ed(&pc_Buffer, (UCHAR *) &pst_CurrentZDx->pst_ColSetZDx);
			ul_Length = LOA_ReadULong(&pc_Buffer);

			if(ul_Length)
			{
#ifdef ACTIVE_EDITORS
#ifdef JADEFUSION
				pst_CurrentZDx->sz_Name = (char*)MEM_p_VMAlloc(ul_Length);
#else
				pst_CurrentZDx->sz_Name = MEM_p_VMAlloc(ul_Length);
#endif
#endif /* #ifdef ACTIVE_EDITORS */
				LOA_ReadString_Ed(&pc_Buffer, pst_CurrentZDx->sz_Name, ul_Length);
			}

			LOA_ReadUChar_Ed(&pc_Buffer, &pst_CurrentZDx->uc_AI_Index);

			switch(pst_CurrentZDx->uc_Type)
			{
			case COL_C_Zone_Sphere:

				/*$F-------Allocation of the Shape ------------ */
				pst_CurrentZDx->p_Shape = MEM_p_VMAlloc(sizeof(COL_tdst_Sphere));

				/*$F--------- Load of the Center -------------- */
				LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape));

				/*$F--------- Load of the Radius -------------- */
				((COL_tdst_Sphere *) pst_CurrentZDx->p_Shape)->f_Radius = LOA_ReadFloat(&pc_Buffer);
#ifdef _DEBUG
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	*pst_Center;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					if
					(
						(COL_f_Shape_GetRadius(pst_CurrentZDx->p_Shape) > 100000.0f)
					||	(COL_f_Shape_GetRadius(pst_CurrentZDx->p_Shape) < 0.0f)
					)
					{
						ERR_X_Warning(0, "Collision Zone Radius over 100000 meters -> Reset to default", NULL);
						((COL_tdst_Sphere *) pst_CurrentZDx->p_Shape)->f_Radius = 1.0f;
					}

					pst_Center = COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape);
					if
					(
						(pst_Center->x > 10000.0f)
					||	(pst_Center->x < -10000.0f)
					||	(pst_Center->y > 10000.0f)
					||	(pst_Center->y < -10000.0f)
					||	(pst_Center->z > 10000.0f)
					||	(pst_Center->z < -10000.0f)
					)
					{
						ERR_X_Warning(0, "Collision Zone Center invalid -> Reset to default", NULL);
						MATH_InitVector(pst_Center, 0.0f, 0.0f, 0.0f);
					}
				}

#endif /* _DEBUG */
				break;

			case COL_C_Zone_Box:

				/*$F-------Allocation of the Shape ------------ */
				pst_CurrentZDx->p_Shape = MEM_p_VMAlloc(sizeof(COL_tdst_Box));

				/*$F--------- Load of the Max -------------- */
				LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetMax(pst_CurrentZDx->p_Shape));

				/*$F--------- Load of the Min -------------- */
				LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetMin(pst_CurrentZDx->p_Shape));
				break;

			case COL_C_Zone_Cylinder:

				/*$F-------Allocation of the Shape ------------ */
				pst_CurrentZDx->p_Shape = MEM_p_VMAlloc(sizeof(COL_tdst_Cylinder));

				/*$F--------- Load of the Center -------------- */
				LOA_ReadVector(&pc_Buffer, COL_pst_Shape_GetCenter(pst_CurrentZDx->p_Shape));

				/*$F--------- Load of the Radius -------------- */
				((COL_tdst_Cylinder *) pst_CurrentZDx->p_Shape)->f_Radius = LOA_ReadFloat(&pc_Buffer);

				/*$F--------- Load of the Height -------------- */
				((COL_tdst_Cylinder *) pst_CurrentZDx->p_Shape)->f_Height = LOA_ReadFloat(&pc_Buffer);
				break;

#ifdef ACTIVE_EDITORS
			default:
				{
					/*~~~~~~~~~~~~~~~~~*/
					char	asz_Log[300];
					/*~~~~~~~~~~~~~~~~~*/

					ul_File = LOA_ul_GetCurrentKey();

					sprintf
					(
						asz_Log,
						"Specific zone loading error. You must ERASE the Collision instance file %x and re-drag and drop the concerning ColSet",
						ul_File
					);
					ERR_X_Warning(0, asz_Log, NULL);
				}
				break;
#endif
			}

#ifdef ACTIVE_EDITORS
			/*$F---------- Register pointer --------------- */
			ul_File = LOA_ul_GetCurrentKey();
			ul_File = BIG_ul_SearchKeyToFat(ul_File);
			BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Path);
			LINK_RegisterPointer(pst_CurrentZDx, LINK_C_Zone, BIG_NameFile(ul_File), asz_Path);
#endif
		}
	}

	pst_Instance->uw_Activation = LOA_ReadUShort(&pc_Buffer);
	pst_Instance->uw_Specific = LOA_ReadUShort(&pc_Buffer);

	/* Before a AllowChange field was saved. Field erased */
	LOA_ReadUShort_Ed(&pc_Buffer, NULL);

	pst_Instance->uw_Crossable = LOA_ReadUShort(&pc_Buffer);

	 pst_Instance->uw_Crossable = COL_Cul_DefaultCross;

	pst_Instance->uw_Flags = LOA_ReadUShort(&pc_Buffer);
	pst_Instance->uc_MaxLOD = LOA_ReadUChar(&pc_Buffer);
	pst_Instance->uc_MinLOD = LOA_ReadUChar(&pc_Buffer);

	pst_Instance->b_InTheWall = TRUE;
	pst_Instance->uc_UserBackFace = 0;

	pst_Instance->ul_Filter_On = 0;
	pst_Instance->ul_Filter_Off = 0;
	pst_Instance->ul_Filter_Type = (ULONG) -1;


	return (ULONG) pst_Instance;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
COL_tdst_Instance *COL_pst_LoadInstance(BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_Instance;
	UCHAR				uc_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Load instance */
	LOA_MakeFileRef(_ul_Key, (ULONG *) &pst_Instance, COL_ul_CallBackLoadInstance, LOA_C_MustExists);
	LOA_Resolve();

	/* We init the data of the instance. */
	pst_Instance->uc_NbOfSpecific = 0;
	pst_Instance->uc_NbOfShared = pst_Instance->pst_ColSet->uc_NbOfZDx;
	pst_Instance->uc_NbOfZDx = pst_Instance->pst_ColSet->uc_NbOfZDx;

	pst_Instance->dpst_ZDx = (COL_tdst_ZDx **) MEM_p_VMAlloc(pst_Instance->pst_ColSet->uc_NbOfZDx * sizeof(COL_tdst_ZDx *));

	/*
	 * We want the pointers of the instance's array of pointers to point to the ColSet
	 * zones.
	 */
	for(uc_Index = 0; uc_Index < pst_Instance->uc_NbOfZDx; uc_Index++)
		*(pst_Instance->dpst_ZDx + uc_Index) = (pst_Instance->pst_ColSet->past_ZDx + uc_Index);

	/*
	 * If the instance has a ColSet, we increase the number of this ColSet loaded
	 * instances.
	 */
	if(pst_Instance->pst_ColSet)
	{
#ifdef ACTIVE_EDITORS
		COL_ColSet_AddInstance(pst_Instance->pst_ColSet, pst_Instance);
#endif
		pst_Instance->pst_ColSet->uw_NbOfInstances++;
	}

	return pst_Instance;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
COL_tdst_ColMap *COL_pst_LoadColMap(BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap *pst_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Load ColMap */
	LOA_MakeFileRef(_ul_Key, (ULONG *) &pst_ColMap, COL_ul_CallBackLoadColMap, LOA_C_MustExists);
	LOA_Resolve();

	pst_ColMap->uc_NbOfCob = 1;
	pst_ColMap->uc_Activation = 0xFF;
	pst_ColMap->uc_CustomBits1 = 0;
	pst_ColMap->uc_CustomBits2 = 0;

	return pst_ColMap;
}

/*$F
 ===================================================================================================
 Aim:	- Increase the number of the Instance's Colset number of instances

 Note:	- Classical case:
		1) COLMAP:
			- The ColMap has been properly loaded. Nevertheless, the uw_NbOfInstances field of the cobs 
			have not been computed. So, we loop thru all the ColMap's cob and update the missing field.
			Moreover, in Editors, we update the Cob's p_Visu Information to be able to see it.
		2) COLSET/INSTANCE:
			- The Instance has been properly loaded.
				-> We init the Instance OldFrameMatrix with the current object Global Matrix.
				-> We compute the Instance's array of pointers to zone.
				-> We update the Instance's ColSet uw_NbOfInstances.
   
	 
		- Many other cases are possible.
		1) COLMAP:
			- The GameObject has the ColMap flag but no ColMap structure. That means that the ColMap 
			file has been erased (or not properly loaded). In that case, we add a Triangles-based Self
			Cob to the GameObject.
			- The GameObject has a ColMap but on e of its Cob has not been properly loaded. In that case,
			we remove everything concerning this Object's ColMap (Flag and structure).
		2) COLSET:
			- The GameObject has one of ZDM/ZDE Identity Flags but its instance or its colset is not 
			a valid one. That means that there was a problem when loading the corresponding file. So,
			we reset both the concerning Identity Flags.
			- If the number of ZDx is 0, that means that we enter this function after a Duplication. 
			We fill the instance structure with the ColSet info.
			- If the number of ZDx is different from the ColSet and the Instance, that means that the 
			ColSet has been changed in another map. In that case, we free all the possible instance's 
			specific ZDx and use the ColSet structure to fill the instance's one.
 ===================================================================================================
 */

void COL_ResolveRefs(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_Instance;
	UCHAR				uc_Index;
	UCHAR				uc_Rank;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* To avoid errors. */
	ERR_X_Assert(_pst_GO->pst_Extended->pst_Col);

	/*$F ---------------------------------------- 
		 -------		COLMAP		 ------------ 
		 ---------------------------------------- */
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_ColMap *pst_ColMap;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;

		if((!pst_ColMap) || (!pst_ColMap->uc_NbOfCob))
		{
#ifdef ACTIVE_EDITORS
			if(!pst_ColMap)
			{
				ERR_X_Warning(0, "A ColMap has not been loaded -> swap to ZDR itself", _pst_GO->sz_Name);
			}
			else
			{
				ERR_X_Warning(0, "A ColMap has no Cob -> swap to ZDR itself", _pst_GO->sz_Name);
#ifdef JADEFUSION
				COL_FreeColMap((COL_tdst_Base_*)_pst_GO->pst_Extended->pst_Col);
#else
				COL_FreeColMap(_pst_GO->pst_Extended->pst_Col);
#endif
			}

#endif
			COL_AllocColMap(_pst_GO);
			COL_ColMap_AddCob(_pst_GO, COL_C_Zone_Triangles, NULL, NULL);
		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_Cob	**dpst_Cob, **dpst_LastCob;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			dpst_Cob = pst_ColMap->dpst_Cob;
			dpst_LastCob = dpst_Cob + pst_ColMap->uc_NbOfCob;
			for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
			{
				/*
				 * If there is a BigFile problem and the Cob has not been found, we reset
				 * everything concerning this Object's ColMap.
				 */
				if(!(*dpst_Cob))
				{
#ifdef ACTIVE_EDITORS
					ERR_X_Warning(0, "A Cob has not been loaded.", _pst_GO->sz_Name);
#endif
					OBJ_ClearIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap);
					MEM_Free(pst_ColMap->dpst_Cob);
					MEM_Free(pst_ColMap);
					LOA_DeleteAddress(pst_ColMap);
					((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap = NULL;

					/* We break the "FOR" loop. */
					break;
				}

#ifdef ACTIVE_EDITORS
				(*dpst_Cob)->pst_GO = _pst_GO;
#endif
				if((*dpst_Cob)->pst_GMatList && !(*dpst_Cob)->uw_NbOfInstances)
				{
					(*dpst_Cob)->pst_GMatList->ul_NbOfInstances++;
				}

#ifdef ACTIVE_EDITORS
			    COL_GMat_UpdateFileName(*dpst_Cob);
#endif
				(*dpst_Cob)->uw_NbOfInstances++;

				if((*dpst_Cob)->uc_Type == COL_C_Zone_Triangles)
				{
#ifdef ACTIVE_EDITORS
					COL_UpdateCameraNumberOfFaces(*dpst_Cob);
#endif

					if(!LOA_IsBinaryData()) COL_ComputeProximity(*dpst_Cob);
					if(!LOA_IsBinaryData()) COL_OK3_Build(_pst_GO, TRUE, FALSE);
				}
			}
		}
	}

	/*$F ---------------------------------------- 
		 -------	COLSET/INSTANCE	   ---------- 
		 ---------------------------------------- */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
		if((!pst_Instance) || (!pst_Instance->pst_ColSet))
		{
			OBJ_ClearIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE);
#ifdef ACTIVE_EDITORS
			if(!pst_Instance)
			{
				ERR_X_Warning(0, "A Col Instance has not been loaded.", _pst_GO->sz_Name);
			}
			else
			{
				ERR_X_Warning(0, "A ColSet has not been loaded.", _pst_GO->sz_Name);
			}

#endif
			return;
		}

		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			MATH_CopyMatrix(pst_Instance->pst_OldGlobalMatrix, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
		else
			MATH_CopyMatrix(pst_Instance->pst_OldGlobalMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO));

		if(pst_Instance->uc_NbOfZDx != pst_Instance->pst_ColSet->uc_NbOfZDx)
		{
			pst_Instance->uc_NbOfZDx = pst_Instance->pst_ColSet->uc_NbOfZDx;
			pst_Instance->uc_NbOfShared = pst_Instance->uc_NbOfZDx;

			/* In this case, we desalloc the specific zones. */
			if(pst_Instance->uc_NbOfSpecific)
			{
				for(uc_Index = 0; uc_Index < pst_Instance->uc_NbOfSpecific; uc_Index++)
				{
					MEM_Free((pst_Instance->past_Specific + uc_Index)->p_Shape);
#ifdef ACTIVE_EDITORS
					MEM_Free((pst_Instance->past_Specific + uc_Index)->sz_Name);
#endif
				}

				MEM_Free(pst_Instance->past_Specific);
			}

			pst_Instance->uc_NbOfSpecific = 0;
			if(pst_Instance->dpst_ZDx) MEM_Free(pst_Instance->dpst_ZDx);
			pst_Instance->dpst_ZDx = (COL_tdst_ZDx **) MEM_p_VMAlloc(pst_Instance->uc_NbOfZDx * sizeof(COL_tdst_ZDx *));
			L_memset(pst_Instance->dpst_ZDx, 0, pst_Instance->uc_NbOfZDx * sizeof(COL_tdst_ZDx *));

			for(uc_Index = 0; uc_Index < pst_Instance->uc_NbOfZDx; uc_Index++)
				pst_Instance->dpst_ZDx[uc_Index] = (COL_tdst_ZDx *) uc_Index;

			pst_Instance->uw_Specific = 0;
		}

		/* We compute the instance array of pointers to zone. */
		for(uc_Index = 0; uc_Index < pst_Instance->uc_NbOfZDx; uc_Index++)
		{
			uc_Rank = (UCHAR) pst_Instance->dpst_ZDx[uc_Index];

			if(uc_Rank > 16)
			{
				OBJ_ClearIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE);
#ifdef ACTIVE_EDITORS
				ERR_X_Warning(0, "Invalid ZDx index", _pst_GO->sz_Name);
#endif
				return;
			}

			if(COL_b_Instance_IsSpecific(pst_Instance, uc_Index))
			{
#ifdef ACTIVE_EDITORS
				(pst_Instance->past_Specific + uc_Rank)->pst_ColSetZDx = pst_Instance->pst_ColSet->past_ZDx + (UCHAR) ((pst_Instance->past_Specific + uc_Rank)->pst_ColSetZDx);

				if((pst_Instance->past_Specific + uc_Rank)->pst_ColSetZDx->uc_AI_Index != (pst_Instance->past_Specific + uc_Rank)->uc_AI_Index)
				{
					(pst_Instance->past_Specific + uc_Rank)->uc_AI_Index = (pst_Instance->past_Specific + uc_Rank)->pst_ColSetZDx->uc_AI_Index;
				}
#endif
				*(pst_Instance->dpst_ZDx + uc_Index) = pst_Instance->past_Specific + uc_Rank;
			}
			else
				*(pst_Instance->dpst_ZDx + uc_Index) = pst_Instance->pst_ColSet->past_ZDx + uc_Rank;
		}

		/*$F	----------------- Increase the number of this ColSet instances ----------- */
		if(pst_Instance->pst_ColSet)
		{
#ifdef ACTIVE_EDITORS
			COL_ColSet_AddInstance(pst_Instance->pst_ColSet, pst_Instance);
#endif
			pst_Instance->pst_ColSet->uw_NbOfInstances++;
		}
	}
}

/*$F
 =======================================================================================================================



												OK3



 =======================================================================================================================
 */

void COL_pst_OK3_GetTriangle
(
	void				*_p_Data,
	USHORT				_uw_Element,
	USHORT				_uw_Triangle,
	MATH_tdst_Vector	**pp_T1,
	MATH_tdst_Vector	**pp_T2,
	MATH_tdst_Vector	**pp_T3,
	BOOL				_b_ColMap
)
{
	if(_b_ColMap)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_IndexedTriangles	*pst_Cob;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Cob = (COL_tdst_IndexedTriangles *) _p_Data;
		*pp_T1 = &(pst_Cob->dst_Point[pst_Cob->dst_Element[_uw_Element].dst_Triangle[_uw_Triangle].auw_Index[0]]);
		*pp_T2 = &(pst_Cob->dst_Point[pst_Cob->dst_Element[_uw_Element].dst_Triangle[_uw_Triangle].auw_Index[1]]);
		*pp_T3 = &(pst_Cob->dst_Point[pst_Cob->dst_Element[_uw_Element].dst_Triangle[_uw_Triangle].auw_Index[2]]);
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_Object *pst_Object;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Object = (GEO_tdst_Object *) _p_Data;
		*pp_T1 = &(pst_Object->dst_Point[pst_Object->dst_Element[_uw_Element].dst_Triangle[_uw_Triangle].auw_Index[0]]);
		*pp_T2 = &(pst_Object->dst_Point[pst_Object->dst_Element[_uw_Element].dst_Triangle[_uw_Triangle].auw_Index[1]]);
		*pp_T3 = &(pst_Object->dst_Point[pst_Object->dst_Element[_uw_Element].dst_Triangle[_uw_Triangle].auw_Index[2]]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_OK3_OptimizeBoxBV(COL_tdst_OK3_Box *_pst_Box, void *_p_Data, BOOL _b_ColMap)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_OK3_Element	*pst_OK3_Element;
	MATH_tdst_Vector		st_Min, st_Max;
	MATH_tdst_Vector		*T1, *T2, *T3;
	MATH_tdst_Vector		st_Dir, st_Point;
	MATH_tdst_Vector		Pik, Pok;
	INT_tdst_Box			st_AABBox;
	float					f_Norm;
	int						i, j;
	BOOL					b_T1InTri, b_T2InTri, b_T3InTri;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Box || !_p_Data) return;

	/* Empty Box ... Tag its box */
	if(!_pst_Box->ul_NumElement)
	{
		_pst_Box->st_Min.x = (float) 0xFFFFFFFF;
		return;
	}

	if(_b_ColMap && (_pst_Box == ((COL_tdst_IndexedTriangles *) _p_Data)->pst_OK3->pst_OK3_Boxes + 28))
	{
		_b_ColMap = _b_ColMap;
	}

	MATH_InitVector(&st_Min, 1000000.0f, 1000000.0f, 1000000.0f);
	MATH_InitVector(&st_Max, -1000000.0f, -1000000.0f, -1000000.0f);

	MATH_CopyVector(&Pik, &_pst_Box->st_Min);
	MATH_CopyVector(&Pok, &_pst_Box->st_Max);

	for(i = 0; i < (int) _pst_Box->ul_NumElement; i++)
	{
		pst_OK3_Element = &_pst_Box->pst_OK3_Element[i];
		for(j = 0; j < (int) pst_OK3_Element->uw_NumTriangle; j++)
		{
			COL_pst_OK3_GetTriangle
			(
				_p_Data,
				pst_OK3_Element->uw_Element,
				pst_OK3_Element->puw_OK3_Triangle[j],
				&T1,
				&T2,
				&T3,
				_b_ColMap
			);

			b_T1InTri = 
			(
				(((T1->x + 1E-2f) > _pst_Box->st_Min.x) && ((T1->x - 1E-2f) < _pst_Box->st_Max.x))
			&&	(((T1->y + 1E-2f) > _pst_Box->st_Min.y) && ((T1->y - 1E-2f) < _pst_Box->st_Max.y))
			&&	(((T1->z + 1E-2f) > _pst_Box->st_Min.z) && ((T1->z - 1E-2f) < _pst_Box->st_Max.z))
			);

			b_T2InTri = 
			(
				(((T2->x + 1E-2f) > _pst_Box->st_Min.x) && ((T2->x - 1E-2f) < _pst_Box->st_Max.x))
			&&	(((T2->y + 1E-2f) > _pst_Box->st_Min.y) && ((T2->y - 1E-2f) < _pst_Box->st_Max.y))
			&&	(((T2->z + 1E-2f) > _pst_Box->st_Min.z) && ((T2->z - 1E-2f) < _pst_Box->st_Max.z))
			);

			b_T3InTri = 
			(
				(((T3->x + 1E-2f) > _pst_Box->st_Min.x) && ((T3->x - 1E-2f) < _pst_Box->st_Max.x))
			&&	(((T3->y + 1E-2f) > _pst_Box->st_Min.y) && ((T3->y - 1E-2f) < _pst_Box->st_Max.y))
			&&	(((T3->z + 1E-2f) > _pst_Box->st_Min.z) && ((T3->z - 1E-2f) < _pst_Box->st_Max.z))
			);


			/* A défaut d'un algo bien ... on prend un algo pourri qui marche */
			b_T1InTri = TRUE;
			b_T2InTri = TRUE;
			b_T3InTri = TRUE;

			if(b_T1InTri)
			{
				if(T1->x > st_Max.x) st_Max.x = T1->x;
				if(T1->x < st_Min.x) st_Min.x = T1->x;
				if(T1->y > st_Max.y) st_Max.y = T1->y;
				if(T1->y < st_Min.y) st_Min.y = T1->y;
				if(T1->z > st_Max.z) st_Max.z = T1->z;
				if(T1->z < st_Min.z) st_Min.z = T1->z;
			}
			else
			{
				MATH_SubVector(&st_Dir, T2, T1);
				f_Norm = MATH_f_NormVector(&st_Dir);

				MATH_NormalizeEqualVector(&st_Dir);
				MATH_CopyVector(&st_AABBox.st_Min, &_pst_Box->st_Min);
				MATH_CopyVector(&st_AABBox.st_Max, &_pst_Box->st_Max);

				if(INT_FullRayAABBox(T1, &st_Dir, &st_AABBox, &st_Point, f_Norm, NULL))
				{
					if(st_Point.x > st_Max.x) st_Max.x = st_Point.x;
					if(st_Point.x < st_Min.x) st_Min.x = st_Point.x;
					if(st_Point.y > st_Max.y) st_Max.y = st_Point.y;
					if(st_Point.y < st_Min.y) st_Min.y = st_Point.y;
					if(st_Point.z > st_Max.z) st_Max.z = st_Point.z;
					if(st_Point.z < st_Min.z) st_Min.z = st_Point.z;
				}


				MATH_SubVector(&st_Dir, T3, T1);
				f_Norm = MATH_f_NormVector(&st_Dir);

				MATH_NormalizeEqualVector(&st_Dir);
				MATH_CopyVector(&st_AABBox.st_Min, &_pst_Box->st_Min);
				MATH_CopyVector(&st_AABBox.st_Max, &_pst_Box->st_Max);

				if(INT_FullRayAABBox(T1, &st_Dir, &st_AABBox, &st_Point, f_Norm, NULL))
				{
					if(st_Point.x > st_Max.x) st_Max.x = st_Point.x;
					if(st_Point.x < st_Min.x) st_Min.x = st_Point.x;
					if(st_Point.y > st_Max.y) st_Max.y = st_Point.y;
					if(st_Point.y < st_Min.y) st_Min.y = st_Point.y;
					if(st_Point.z > st_Max.z) st_Max.z = st_Point.z;
					if(st_Point.z < st_Min.z) st_Min.z = st_Point.z;
				}
			}

			if(b_T2InTri)
			{
				if(T2->x > st_Max.x) st_Max.x = T2->x;
				if(T2->x < st_Min.x) st_Min.x = T2->x;
				if(T2->y > st_Max.y) st_Max.y = T2->y;
				if(T2->y < st_Min.y) st_Min.y = T2->y;
				if(T2->z > st_Max.z) st_Max.z = T2->z;
				if(T2->z < st_Min.z) st_Min.z = T2->z;
			}
			else
			{
				MATH_SubVector(&st_Dir, T1, T2);
				f_Norm = MATH_f_NormVector(&st_Dir);

				MATH_NormalizeEqualVector(&st_Dir);
				MATH_CopyVector(&st_AABBox.st_Min, &_pst_Box->st_Min);
				MATH_CopyVector(&st_AABBox.st_Max, &_pst_Box->st_Max);

				if(INT_FullRayAABBox(T2, &st_Dir, &st_AABBox, &st_Point, f_Norm, NULL))
				{
					if(st_Point.x > st_Max.x) st_Max.x = st_Point.x;
					if(st_Point.x < st_Min.x) st_Min.x = st_Point.x;
					if(st_Point.y > st_Max.y) st_Max.y = st_Point.y;
					if(st_Point.y < st_Min.y) st_Min.y = st_Point.y;
					if(st_Point.z > st_Max.z) st_Max.z = st_Point.z;
					if(st_Point.z < st_Min.z) st_Min.z = st_Point.z;
				}


				MATH_SubVector(&st_Dir, T3, T2);
				f_Norm = MATH_f_NormVector(&st_Dir);

				MATH_NormalizeEqualVector(&st_Dir);
				MATH_CopyVector(&st_AABBox.st_Min, &_pst_Box->st_Min);
				MATH_CopyVector(&st_AABBox.st_Max, &_pst_Box->st_Max);

				if(INT_FullRayAABBox(T2, &st_Dir, &st_AABBox, &st_Point, f_Norm, NULL))
				{
					if(st_Point.x > st_Max.x) st_Max.x = st_Point.x;
					if(st_Point.x < st_Min.x) st_Min.x = st_Point.x;
					if(st_Point.y > st_Max.y) st_Max.y = st_Point.y;
					if(st_Point.y < st_Min.y) st_Min.y = st_Point.y;
					if(st_Point.z > st_Max.z) st_Max.z = st_Point.z;
					if(st_Point.z < st_Min.z) st_Min.z = st_Point.z;
				}
			}

			if(b_T3InTri)
			{
				if(T3->x > st_Max.x) st_Max.x = T3->x;
				if(T3->x < st_Min.x) st_Min.x = T3->x;
				if(T3->y > st_Max.y) st_Max.y = T3->y;
				if(T3->y < st_Min.y) st_Min.y = T3->y;
				if(T3->z > st_Max.z) st_Max.z = T3->z;
				if(T3->z < st_Min.z) st_Min.z = T3->z;
			}
			else
			{
				MATH_SubVector(&st_Dir, T1, T3);
				f_Norm = MATH_f_NormVector(&st_Dir);

				MATH_NormalizeEqualVector(&st_Dir);
				MATH_CopyVector(&st_AABBox.st_Min, &_pst_Box->st_Min);
				MATH_CopyVector(&st_AABBox.st_Max, &_pst_Box->st_Max);

				if(INT_FullRayAABBox(T3, &st_Dir, &st_AABBox, &st_Point, f_Norm, NULL))
				{
					if(st_Point.x > st_Max.x) st_Max.x = st_Point.x;
					if(st_Point.x < st_Min.x) st_Min.x = st_Point.x;
					if(st_Point.y > st_Max.y) st_Max.y = st_Point.y;
					if(st_Point.y < st_Min.y) st_Min.y = st_Point.y;
					if(st_Point.z > st_Max.z) st_Max.z = st_Point.z;
					if(st_Point.z < st_Min.z) st_Min.z = st_Point.z;
				}


				MATH_SubVector(&st_Dir, T2, T3);
				f_Norm = MATH_f_NormVector(&st_Dir);

				MATH_NormalizeEqualVector(&st_Dir);
				MATH_CopyVector(&st_AABBox.st_Min, &_pst_Box->st_Min);
				MATH_CopyVector(&st_AABBox.st_Max, &_pst_Box->st_Max);

				if(INT_FullRayAABBox(T3, &st_Dir, &st_AABBox, &st_Point, f_Norm, NULL))
				{
					if(st_Point.x > st_Max.x) st_Max.x = st_Point.x;
					if(st_Point.x < st_Min.x) st_Min.x = st_Point.x;
					if(st_Point.y > st_Max.y) st_Max.y = st_Point.y;
					if(st_Point.y < st_Min.y) st_Min.y = st_Point.y;
					if(st_Point.z > st_Max.z) st_Max.z = st_Point.z;
					if(st_Point.z < st_Min.z) st_Min.z = st_Point.z;
				}
			}
		}
	}

	if((st_Max.x != -1000000.0f) && (st_Max.x < _pst_Box->st_Max.x)) _pst_Box->st_Max.x = st_Max.x;
	if((st_Min.x != 1000000.0f) && (st_Min.x > _pst_Box->st_Min.x)) _pst_Box->st_Min.x = st_Min.x;

	if((st_Max.y != -1000000.0f) && (st_Max.y < _pst_Box->st_Max.y)) _pst_Box->st_Max.y = st_Max.y;
	if((st_Min.y != 1000000.0f) && (st_Min.y > _pst_Box->st_Min.y)) _pst_Box->st_Min.y = st_Min.y;

	if((st_Max.z != -1000000.0f) && (st_Max.z < _pst_Box->st_Max.z)) _pst_Box->st_Max.z = st_Max.z;
	if((st_Min.z != 1000000.0f) && (st_Min.z > _pst_Box->st_Min.z)) _pst_Box->st_Min.z = st_Min.z;

	/* anti bug */
	if
	(
		(_pst_Box->st_Min.x >= _pst_Box->st_Max.x)
	||	(_pst_Box->st_Min.y >= _pst_Box->st_Max.y)
	||	(_pst_Box->st_Min.z >= _pst_Box->st_Max.z)
	)
	{
		MATH_CopyVector(&_pst_Box->st_Min, &Pik);
		MATH_CopyVector(&_pst_Box->st_Max, &Pok);
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_OK3_CleanNodes(COL_tdst_OK3_Node *_pst_Node)
{
	if(_pst_Node->pst_Son && !_pst_Node->pst_Son->pst_Son && (_pst_Node->pst_Son->pst_OK3_Box->st_Min.x == (float) 0xFFFFFFFF))
	{
		if(_pst_Node->pst_Son->pst_Next)
		{
			COL_tdst_OK3_Node	*pst_N;

			pst_N = _pst_Node->pst_Son->pst_Next;
			L_memcpy(_pst_Node->pst_Son, _pst_Node->pst_Son->pst_Next, sizeof(COL_tdst_OK3_Node));
			MEM_Free(pst_N);
		}
		else
		{
			MEM_Free(_pst_Node->pst_Son);
			_pst_Node->pst_Son = NULL;
		}
	}

	if(_pst_Node->pst_Next && !_pst_Node->pst_Next->pst_Son && (_pst_Node->pst_Next->pst_OK3_Box->st_Min.x == (float) 0xFFFFFFFF))
	{
		if(_pst_Node->pst_Next->pst_Next)
		{
			COL_tdst_OK3_Node	*pst_N;

			pst_N = _pst_Node->pst_Next->pst_Next;
			L_memcpy(_pst_Node->pst_Next, _pst_Node->pst_Next->pst_Next, sizeof(COL_tdst_OK3_Node));
			MEM_Free(pst_N);
		}
		else
		{
			MEM_Free(_pst_Node->pst_Next);
			_pst_Node->pst_Next = NULL;
		}
	}

	if(_pst_Node->pst_Son)
		COL_OK3_CleanNodes(_pst_Node->pst_Son);

	if(_pst_Node->pst_Next)
		COL_OK3_CleanNodes(_pst_Node->pst_Next);

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_OK3_OptimizeBVRecursive(COL_tdst_OK3_Node *_pst_Node)
{
	if
	(
		_pst_Node->pst_Son 
	&&	_pst_Node->pst_Son->pst_OK3_Box 
	)
	{
		COL_OK3_OptimizeBVRecursive(_pst_Node->pst_Son);
	}



	if
	(
		_pst_Node->pst_Next
	&&	_pst_Node->pst_Next->pst_OK3_Box 
	)
	{
		COL_OK3_OptimizeBVRecursive(_pst_Node->pst_Next);
	}

	if(_pst_Node && _pst_Node->pst_OK3_Box && (_pst_Node->pst_OK3_Box->st_Min.x != (float) 0xFFFFFFFF))
		return;

	if(_pst_Node->pst_Son && _pst_Node->pst_Son->pst_OK3_Box)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_OK3_Node		*pst_N;
		MATH_tdst_Vector		st_Min;
		MATH_tdst_Vector		st_Max;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_N = _pst_Node->pst_Son;

		MATH_CopyVector(&_pst_Node->pst_OK3_Box->st_Min, &pst_N->pst_OK3_Box->st_Min);
		MATH_CopyVector(&_pst_Node->pst_OK3_Box->st_Max, &pst_N->pst_OK3_Box->st_Max);

		while(pst_N->pst_Next && pst_N->pst_Next->pst_OK3_Box)
		{
			pst_N = pst_N->pst_Next;

			MATH_CopyVector(&st_Min, &pst_N->pst_OK3_Box->st_Min);
			MATH_CopyVector(&st_Max, &pst_N->pst_OK3_Box->st_Max);

			if(pst_N->pst_OK3_Box->st_Min.x == (float) 0xFFFFFFFF)
			{
				pst_N->pst_OK3_Box->st_Min.x = 100000000.0f;
			}

			/* Max */
			if(st_Max.x > _pst_Node->pst_OK3_Box->st_Max.x)
				_pst_Node->pst_OK3_Box->st_Max.x = st_Max.x;

			if(st_Max.y > _pst_Node->pst_OK3_Box->st_Max.y)
				_pst_Node->pst_OK3_Box->st_Max.y = st_Max.y;

			if(st_Max.z > _pst_Node->pst_OK3_Box->st_Max.z)
				_pst_Node->pst_OK3_Box->st_Max.z = st_Max.z;

			/* Min */
			if(st_Min.x < _pst_Node->pst_OK3_Box->st_Min.x)
				_pst_Node->pst_OK3_Box->st_Min.x = st_Min.x;

			if(st_Min.y < _pst_Node->pst_OK3_Box->st_Min.y)
				_pst_Node->pst_OK3_Box->st_Min.y = st_Min.y;

			if(st_Min.z < _pst_Node->pst_OK3_Box->st_Min.z)
				_pst_Node->pst_OK3_Box->st_Min.z = st_Min.z;

			if(_pst_Node->pst_OK3_Box->st_Min.x == (float) 0xFFFFFFFF)
			{
				_pst_Node->pst_OK3_Box->st_Min.x = (float) 0xFFFFFFFF;
			}
		}
	}
	else
	{
			_pst_Node = _pst_Node;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_OK3_Optimize(COL_tdst_OK3 *_pst_OK3, void *_p_Data, BOOL _b_ColMap)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_OK3_Box		*pst_Box;
	COL_tdst_OK3_Element	*pst_Element;
	int						i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < (int) _pst_OK3->ul_NumBox; i++)
	{
		pst_Box = &_pst_OK3->pst_OK3_Boxes[i];

		for(j = 0; j < (int) pst_Box->ul_NumElement; j++)
		{
			pst_Element = &pst_Box->pst_OK3_Element[j];

			if(!pst_Element->uw_NumTriangle)
			{
				if(pst_Box->ul_NumElement == 1)
				{
					MEM_Free(pst_Box->pst_OK3_Element);
					pst_Box->pst_OK3_Element = NULL;
					pst_Box->ul_NumElement = 0;
					break;
				}
				else
				{
					L_memcpy
					(
						&pst_Box->pst_OK3_Element[j],
						&pst_Box->pst_OK3_Element[j + 1],
						(pst_Box->ul_NumElement - j - 1) * sizeof(COL_tdst_OK3_Element)
					);
					pst_Box->ul_NumElement--;
					j--;
				}
			}
		}
	}

	/* First Pass ... Optimize BV of all box containing triangles */
	for(i = 0; i < (int) _pst_OK3->ul_NumBox; i++)
	{
		pst_Box = &_pst_OK3->pst_OK3_Boxes[i];
		COL_OK3_OptimizeBoxBV(pst_Box, _p_Data, _b_ColMap);
	}

	/* Second Pass ... Optimize BV of all "empty" boxes */
	COL_OK3_OptimizeBVRecursive(_pst_OK3->pst_OK3_God);

	/* Clean nodes deleting empty ones */
	COL_OK3_CleanNodes(_pst_OK3->pst_OK3_God);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG Face_Plane(MATH_tdst_Vector *P, MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max)
{
	/*~~~~~~~~~~~~*/
	LONG	outcode;
	/*~~~~~~~~~~~~*/

	outcode = 0;
	if(P->x > _pst_Max->x) outcode |= 0x01;
	if(P->x < _pst_Min->x) outcode |= 0x02;
	if(P->y > _pst_Max->y) outcode |= 0x04;
	if(P->y < _pst_Min->y) outcode |= 0x08;
	if(P->z > _pst_Max->z) outcode |= 0x10;
	if(P->z < _pst_Min->z) outcode |= 0x20;

	return outcode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG bevel_2D(MATH_tdst_Vector *Point, MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				outcode;
	MATH_tdst_Vector	st_Center;
	MATH_tdst_Vector	P, Max, Min;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Center, _pst_Max, _pst_Min);
	MATH_MulEqualVector(&st_Center, 0.5f);
	MATH_AddEqualVector(&st_Center, _pst_Min);

	MATH_SubVector(&P, Point, &st_Center);
	MATH_SubVector(&Max, _pst_Max, &st_Center);
	MATH_SubVector(&Min, _pst_Min, &st_Center);

	outcode = 0;
	if(P.x + P.y > Max.x + Max.y) outcode |= 0x001;
	if(P.x - P.y > Max.x + Max.y) outcode |= 0x002;
	if(-P.x + P.y > Max.x + Max.y) outcode |= 0x004;
	if(-P.x - P.y > Max.x + Max.y) outcode |= 0x008;

	if(P.x + P.z > Max.x + Max.z) outcode |= 0x010;
	if(P.x - P.z > Max.x + Max.z) outcode |= 0x020;
	if(-P.x + P.z > Max.x + Max.z) outcode |= 0x040;
	if(-P.x - P.z > Max.x + Max.z) outcode |= 0x080;

	if(P.y + P.z > Max.y + Max.z) outcode |= 0x100;
	if(P.y - P.z > Max.y + Max.z) outcode |= 0x200;
	if(-P.y + P.z > Max.y + Max.z) outcode |= 0x400;
	if(-P.y - P.z > Max.y + Max.z) outcode |= 0x800;

	return outcode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG bevel_3D(MATH_tdst_Vector *Point, MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				outcode;
	MATH_tdst_Vector	st_Center;
	MATH_tdst_Vector	P, Max, Min;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Center, _pst_Max, _pst_Min);
	MATH_MulEqualVector(&st_Center, 0.5f);
	MATH_AddEqualVector(&st_Center, _pst_Min);

	MATH_SubVector(&P, Point, &st_Center);
	MATH_SubVector(&Max, _pst_Max, &st_Center);
	MATH_SubVector(&Min, _pst_Min, &st_Center);

	outcode = 0;

	if(P.x + P.y + P.z > Max.x + Max.y + Max.z) outcode |= 0x01;
	if(P.x + P.y - P.z > Max.x + Max.y + Max.z) outcode |= 0x02;
	if(P.x - P.y + P.z > Max.x + Max.y + Max.z) outcode |= 0x04;
	if(P.x - P.y - P.z > Max.x + Max.y + Max.z) outcode |= 0x08;
	if(-P.x + P.y + P.z > Max.x + Max.y + Max.z) outcode |= 0x10;
	if(-P.x + P.y - P.z > Max.x + Max.y + Max.z) outcode |= 0x20;
	if(-P.x - P.y + P.z > Max.x + Max.y + Max.z) outcode |= 0x40;
	if(-P.x - P.y - P.z > Max.x + Max.y + Max.z) outcode |= 0x80;

	return outcode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG check_point
(
	MATH_tdst_Vector	*P1,
	MATH_tdst_Vector	*P2,
	float				Alpha,
	LONG				Mask,
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	Plane_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Plane_Point.x = P1->x + Alpha * (P2->x - P1->x);
	Plane_Point.y = P1->y + Alpha * (P2->y - P1->y);
	Plane_Point.z = P1->z + Alpha * (P2->z - P1->z);

	return(Face_Plane(&Plane_Point, _pst_Min, _pst_Max) & Mask);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG check_line
(
	MATH_tdst_Vector	*P1,
	MATH_tdst_Vector	*P2,
	LONG				outcode_diff,
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max
)
{
	if((0x01 & outcode_diff) != 0)
	{
		if(check_point(P1, P2, (-_pst_Max->x + P1->x) / (P1->x - P2->x), 0x3e, _pst_Min, _pst_Max) == 0) return 1;
	}

	if((0x02 & outcode_diff) != 0)
	{
		if(check_point(P1, P2, (-_pst_Min->x + P1->x) / (P1->x - P2->x), 0x3d, _pst_Min, _pst_Max) == 0) return 1;
	}

	if((0x04 & outcode_diff) != 0)
	{
		if(check_point(P1, P2, (-_pst_Max->y + P1->y) / (P1->y - P2->y), 0x3b, _pst_Min, _pst_Max) == 0) return 1;
	}

	if((0x08 & outcode_diff) != 0)
	{
		if(check_point(P1, P2, (-_pst_Min->y + P1->y) / (P1->y - P2->y), 0x37, _pst_Min, _pst_Max) == 0) return 1;
	}

	if((0x10 & outcode_diff) != 0)
	{
		if(check_point(P1, P2, (-_pst_Max->z + P1->z) / (P1->z - P2->z), 0x2f, _pst_Min, _pst_Max) == 0) return 1;
	}

	if((0x20 & outcode_diff) != 0)
	{
		if(check_point(P1, P2, (-_pst_Min->z + P1->z) / (P1->z - P2->z), 0x1f, _pst_Min, _pst_Max) == 0) return 1;
	}

	return 0;
}

extern BOOL INT_PointInTriangle
			(
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				UCHAR *
			);

/*
 =======================================================================================================================
    Aim:    Algo from Graphics Gems 3. p 522
 =======================================================================================================================
 */
BOOL COL_OK3_TriangleTouchBox
(
	MATH_tdst_Vector	*V1,
	MATH_tdst_Vector	*V2,
	MATH_tdst_Vector	*V3,
	MATH_tdst_Vector	*_pst_Min,
	MATH_tdst_Vector	*_pst_Max
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	Vect12, Vect13, Vect23;
	MATH_tdst_Vector	st_Normal;
	MATH_tdst_Vector	st_Diag1, st_Diag2;
	MATH_tdst_Vector	st_Diag1V1, st_Diag12;
	MATH_tdst_Vector	st_Inter;
	LONG				V1_Test, V2_Test, V3_Test;
#ifdef JADEFUSION
	FLOAT               fDot;
#endif
	float				Alpha;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * First, compare all 3 vertexes with all 6 face-planes. If any vertex is inside
	 * the cube, return TRUE
	 */
	V1_Test = Face_Plane(V1, _pst_Min, _pst_Max);
	if(!V1_Test) return TRUE;
	V2_Test = Face_Plane(V2, _pst_Min, _pst_Max);
	if(!V2_Test) return TRUE;
	V3_Test = Face_Plane(V3, _pst_Min, _pst_Max);
	if(!V3_Test) return TRUE;

	/* If all 3 vertexes were outside of one or more face-planes, return FALSE. */
	if(V1_Test & V2_Test & V3_Test) return FALSE;

	/* Now, do the same rejection test of the 12 edge planes. */
	V1_Test |= bevel_2D(V1, _pst_Min, _pst_Max) << 8;
	V2_Test |= bevel_2D(V2, _pst_Min, _pst_Max) << 8;
	V3_Test |= bevel_2D(V3, _pst_Min, _pst_Max) << 8;
	if(V1_Test & V2_Test & V3_Test) return FALSE;

	/* Now, do the same rejection test of the 8 corner planes. */
	V1_Test |= bevel_3D(V1, _pst_Min, _pst_Max) << 24;
	V2_Test |= bevel_3D(V2, _pst_Min, _pst_Max) << 24;
	V3_Test |= bevel_3D(V3, _pst_Min, _pst_Max) << 24;
	if(V1_Test & V2_Test & V3_Test) return FALSE;

	/*
	 * If vertex 1 and 2, as a pair, cannot be trivially rejected by the above tests,
	 * then see if the V1-->V2 triangle edge intersects the cube. Do the same for
	 * V1-->V3 and V2-->V3. Pass to the intersection algorithm the "OR" of the outcode
	 * bits, so that only those cube faces which are spanned by each triangle edge
	 * need to be tested.
	 */
	if(!(V1_Test & V2_Test))
	{
		if(check_line(V1, V2, V1_Test | V2_Test, _pst_Min, _pst_Max)) return TRUE;
	}

	if(!(V1_Test & V3_Test))
	{
		if(check_line(V1, V3, V1_Test | V3_Test, _pst_Min, _pst_Max)) return TRUE;
	}

	if(!(V2_Test & V3_Test))
	{
		if(check_line(V2, V3, V2_Test | V3_Test, _pst_Min, _pst_Max)) return TRUE;
	}

	/*
	 * By now, we know that the triangle is not off to any side, and that its side do
	 * not penetrate the cube. We must now test for the cube intersecting the interior
	 * of the triangle. We do this by looking for intersections between the cube
	 * diagonals and the triangle ... first finding intersection of the four diagonals
	 * with the plane of the triangle, and then if that intersection is inside the
	 * cube, pursuing whether the intersection point is inside the triangle itself.
	 */
	MATH_SubVector(&Vect12, V2, V1);
	MATH_SubVector(&Vect13, V3, V1);
	MATH_SubVector(&Vect23, V3, V2);

	MATH_CrossProduct(&st_Normal, &Vect12, &Vect13);

#ifdef JADEFUSION
    if( MATH_b_NulVector(&st_Normal) )
    {
        return FALSE;
    }
#endif

	/* First Diagonal */
	st_Diag1.x = _pst_Min->x;
	st_Diag1.y = _pst_Min->y;
	st_Diag1.z = _pst_Min->z;

	st_Diag2.x = _pst_Max->x;
	st_Diag2.y = _pst_Max->y;
	st_Diag2.z = _pst_Max->z;

	MATH_SubVector(&st_Diag1V1, V1, &st_Diag1);
	MATH_SubVector(&st_Diag12, &st_Diag2, &st_Diag1);

	
#ifdef JADEFUSION
    fDot = MATH_f_DotProduct(&st_Diag12, &st_Normal);
    if (fDot != 0.0f)
#endif
	{
#ifdef JADEFUSION
		Alpha = (MATH_f_DotProduct(&st_Diag1V1, &st_Normal)) / fDot;
#else
		Alpha = (MATH_f_DotProduct(&st_Diag1V1, &st_Normal)) / (MATH_f_DotProduct(&st_Diag12, &st_Normal));
#endif
		MATH_MulVector(&st_Inter, &st_Diag12, Alpha);
		MATH_AddEqualVector(&st_Inter, &st_Diag1);

		if(INT_PointInTriangle(&st_Inter, V1, V2, V3, &st_Normal, NULL)) return TRUE;
	}
	/* Second Diagonal */
	st_Diag1.x = _pst_Min->x;
	st_Diag1.y = _pst_Max->y;
	st_Diag1.z = _pst_Min->z;

	st_Diag2.x = _pst_Max->x;
	st_Diag2.y = _pst_Min->y;
	st_Diag2.z = _pst_Max->z;

	MATH_SubVector(&st_Diag1V1, V1, &st_Diag1);
	MATH_SubVector(&st_Diag12, &st_Diag2, &st_Diag1);

#ifdef JADEFUSION
    fDot = MATH_f_DotProduct(&st_Diag12, &st_Normal);
    if (fDot != 0.0f)
#endif
	{
#ifdef JADEFUSION
        Alpha = (MATH_f_DotProduct(&st_Diag1V1, &st_Normal)) / fDot;
#else
		Alpha = (MATH_f_DotProduct(&st_Diag1V1, &st_Normal)) / (MATH_f_DotProduct(&st_Diag12, &st_Normal));
#endif
		MATH_MulVector(&st_Inter, &st_Diag12, Alpha);
		MATH_AddEqualVector(&st_Inter, &st_Diag1);

		if(INT_PointInTriangle(&st_Inter, V1, V2, V3, &st_Normal, NULL)) return TRUE;

	}
	/* Third Diagonal */
	st_Diag1.x = _pst_Max->x;
	st_Diag1.y = _pst_Max->y;
	st_Diag1.z = _pst_Min->z;

	st_Diag2.x = _pst_Min->x;
	st_Diag2.y = _pst_Min->y;
	st_Diag2.z = _pst_Max->z;

	MATH_SubVector(&st_Diag1V1, V1, &st_Diag1);
	MATH_SubVector(&st_Diag12, &st_Diag2, &st_Diag1);

	Alpha = (MATH_f_DotProduct(&st_Diag1V1, &st_Normal)) / (MATH_f_DotProduct(&st_Diag12, &st_Normal));
	MATH_MulVector(&st_Inter, &st_Diag12, Alpha);
	MATH_AddEqualVector(&st_Inter, &st_Diag1);

	if(INT_PointInTriangle(&st_Inter, V1, V2, V3, &st_Normal, NULL)) return TRUE;

	/* Fourth Diagonal */
	st_Diag1.x = _pst_Max->x;
	st_Diag1.y = _pst_Min->y;
	st_Diag1.z = _pst_Min->z;

	st_Diag2.x = _pst_Min->x;
	st_Diag2.y = _pst_Max->y;
	st_Diag2.z = _pst_Max->z;

	MATH_SubVector(&st_Diag1V1, V1, &st_Diag1);
	MATH_SubVector(&st_Diag12, &st_Diag2, &st_Diag1);

#ifdef JADEFUSION
    fDot = MATH_f_DotProduct(&st_Diag12, &st_Normal);
    if (fDot != 0.0f)
#endif
	{
#ifdef JADEFUSION
		Alpha = (MATH_f_DotProduct(&st_Diag1V1, &st_Normal)) / fDot;
#else
		Alpha = (MATH_f_DotProduct(&st_Diag1V1, &st_Normal)) / (MATH_f_DotProduct(&st_Diag12, &st_Normal));
#endif
		MATH_MulVector(&st_Inter, &st_Diag12, Alpha);
		MATH_AddEqualVector(&st_Inter, &st_Diag1);

		if(INT_PointInTriangle(&st_Inter, V1, V2, V3, &st_Normal, NULL)) return TRUE;
	}
	return FALSE;
}

/* Min size of OK3 Box on all 3 axis */
#define COL_f_OK3_MinSizeX	1.0f
#define COL_f_OK3_MinSizeY	1.0f
#define COL_f_OK3_MinSizeZ	1.00f

/* Realloction granularity */
#define COL_ul_Box_Granularity			50
#define COL_ul_Triangles_Granularity	100

/* Max Number of Faces in OK3 box without subdividing */
#define COL_ul_ColMap_OK3_Threshold		40
#define COL_ul_Geometric_OK3_Threshold	30

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_OK3_GetNumberOfFacesInBox(COL_tdst_OK3_Box *_pst_Box)
{
	/*~~~~~~~*/
	int i, res;
	/*~~~~~~~*/

	res = 0;
	for(i = 0; i < (int) _pst_Box->ul_NumElement; i++)
	{
		res += _pst_Box->pst_OK3_Element[i].uw_NumTriangle;
	}

	return res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_OK3_RecursiveFree(COL_tdst_OK3_Node *_pst_OK3_Node)
{
	if(!_pst_OK3_Node) return;

	if(_pst_OK3_Node->pst_Son)
	{
		COL_OK3_RecursiveFree(_pst_OK3_Node->pst_Son);
	}

	if(_pst_OK3_Node->pst_Next)
	{
		COL_OK3_RecursiveFree(_pst_OK3_Node->pst_Next);
	}

	MEM_Free(_pst_OK3_Node);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_OK3_RecursivePointerUpdate(COL_tdst_OK3_Node *_pst_OK3_Node, LONG _l_Offset)
{
	if(!_pst_OK3_Node) return;

	if(_pst_OK3_Node->pst_Son)
	{
		COL_OK3_RecursivePointerUpdate(_pst_OK3_Node->pst_Son, _l_Offset);
	}

	if(_pst_OK3_Node->pst_Next)
	{
		COL_OK3_RecursivePointerUpdate(_pst_OK3_Node->pst_Next, _l_Offset);
	}

	_pst_OK3_Node->pst_OK3_Box = (COL_tdst_OK3_Box *) ((LONG)_pst_OK3_Node->pst_OK3_Box + _l_Offset); 
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
USHORT const COL_OK3_MAX_TRIANGLES = 20000;
#endif
BOOL COL_OK3_SubdivideBox(void *_p_Data, COL_tdst_OK3_Node *_pst_OK3_Node, ULONG *_pul_Mask, BOOL _b_ColMap)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_OK3				*pst_OK3;
	COL_tdst_IndexedTriangles	*pst_Cob;
	GEO_tdst_Object				*pst_Object;
	COL_tdst_OK3_Box			*pst_Box, *pst_NewBox;
	MATH_tdst_Vector			st_Min, st_Max, st_Diag;
	COL_tdst_OK3_Element		*pst_OK3_Element;
	int							i, j, k, Loop;
	MATH_tdst_Vector			*T1, *T2, *T3;
	ULONG						Mask;
#ifdef JADEFUSION
	USHORT						auw_Triangles[COL_OK3_MAX_TRIANGLES];
#else
	USHORT						auw_Triangles[10000];
#endif
	USHORT						uw_NumTriangles;
	COL_tdst_OK3_Node			*pst_Node;
	LONG						l_OffsetRAM;
	int							NumFaces, Ratio;
	float						f_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_p_Data) return FALSE;

	if(_b_ColMap)
	{
		pst_Cob = (COL_tdst_IndexedTriangles *) _p_Data;
		pst_OK3 = pst_Cob->pst_OK3;
	}
	else
	{
		pst_Object = (GEO_tdst_Object *) _p_Data;
		pst_OK3 = pst_Object->pst_OK3;
	}

	if(!_pst_OK3_Node) 
	{
		return FALSE;
	}

	pst_Box = _pst_OK3_Node->pst_OK3_Box;

	if(!pst_Box) 
	{
		return FALSE;
	}

	NumFaces = COL_OK3_GetNumberOfFacesInBox(pst_Box);

	MATH_SubVector(&st_Diag, &pst_Box->st_Max, &pst_Box->st_Min);

	if(_pul_Mask)
		Mask = *_pul_Mask;
	else
	{
		Mask = 0;

		Ratio = (int)((float) NumFaces / (float) (_b_ColMap ? COL_ul_ColMap_OK3_Threshold: COL_ul_Geometric_OK3_Threshold)) + 1;

		/*
		 * First, we only try to cut along 1 or 2 axis if this (those) axis is (are) more
		 * than 2 times longer than the other axis.
		 */
		if((st_Diag.z > (1.5f * st_Diag.x)) || (st_Diag.z > (1.5f * st_Diag.y)))
			Mask |= (st_Diag.z > COL_f_OK3_MinSizeZ) ? 1 : 0;

		if((st_Diag.y > (1.5f * st_Diag.x)) || (st_Diag.y > (1.5f * st_Diag.z)))
			Mask |= (st_Diag.y > COL_f_OK3_MinSizeY) ? 2 : 0;

		if((st_Diag.x > (1.5f * st_Diag.y)) || (st_Diag.x > (1.5f * st_Diag.z)))
			Mask |= (st_Diag.x > COL_f_OK3_MinSizeX) ? 4 : 0;

		if(Mask == 0)
		{
			Mask |= (st_Diag.z > COL_f_OK3_MinSizeZ) ? 1 : 0;
			Mask |= (st_Diag.y > COL_f_OK3_MinSizeY) ? 2 : 0;
			Mask |= (st_Diag.x > COL_f_OK3_MinSizeX) ? 4 : 0;
		}
	}

	switch(Mask)
	{
	case 7: /* All 3 axis will be subdivided */
		Loop = 8;
		break;

	case 6: /* 2 axis will be subdivided */
	case 5:
	case 3:
		Loop = 4;
		break;

	case 1: /* 1 axis will be subdivided */
	case 2:
	case 4:
		Loop = 2;
		break;

	case 0: /* 0 axis will be subdivided. We RETURN */
		return FALSE;

#ifdef ACTIVE_EDITORS
	default:
		LINK_PrintStatusMsg("Unvalid Mask given to OK3");
		break;
#endif
	}

	if(Ratio < Loop)
	{
		f_Max = fMax3(st_Diag.z, st_Diag.y, st_Diag.x);
		if(f_Max == st_Diag.z)
		{
			Mask = 1;
			Loop = 2;
		}
		else if(f_Max == st_Diag.y)
		{
			Mask = 2;
			Loop = 2;
		}
		else if(f_Max == st_Diag.x)
		{
			Mask = 4;
			Loop = 2;
		}
	}

	/* Granularity / Realloc and update of the pointers */
	pst_OK3->ul_NumBox += Loop;
	if((pst_OK3->ul_NumBox % COL_ul_Box_Granularity) <= (ULONG) Loop)
	{
		l_OffsetRAM = -((LONG) pst_OK3->pst_OK3_Boxes);
		pst_OK3->pst_OK3_Boxes = (COL_tdst_OK3_Box *) MEM_p_VMRealloc( pst_OK3->pst_OK3_Boxes, (pst_OK3->ul_NumBox + COL_ul_Box_Granularity) * sizeof(COL_tdst_OK3_Box));
		l_OffsetRAM += ((LONG) pst_OK3->pst_OK3_Boxes);

		COL_OK3_RecursivePointerUpdate(pst_OK3->pst_OK3_God, l_OffsetRAM);
	}

	/* Realloc has changed the pointer !!! */
	pst_Box = _pst_OK3_Node->pst_OK3_Box;

	MATH_MulEqualVector(&st_Diag, 0.5f);
	for(i = 0; i < Loop; i++)
	{
		if(!_pst_OK3_Node->pst_Son)
		{
			_pst_OK3_Node->pst_Son = (COL_tdst_OK3_Node *) MEM_p_VMAlloc(sizeof(COL_tdst_OK3_Node));
			pst_Node = _pst_OK3_Node->pst_Son;
			pst_Node->pst_Next = NULL;
			pst_Node->pst_Son = NULL;
		}
		else
		{
			pst_Node = _pst_OK3_Node->pst_Son;
			while(pst_Node->pst_Next)
				pst_Node = pst_Node->pst_Next;

			pst_Node->pst_Next = (COL_tdst_OK3_Node *) MEM_p_VMAlloc(sizeof(COL_tdst_OK3_Node));
			pst_Node = pst_Node->pst_Next;
			pst_Node->pst_Next = NULL;
			pst_Node->pst_Son = NULL;
		}

		if(Mask == 7)/* X, Y, Z */
		{
			st_Min.z = (i & 4) ? pst_Box->st_Min.z : pst_Box->st_Min.z + st_Diag.z;
			st_Max.z = (i & 4) ? pst_Box->st_Max.z - st_Diag.z : pst_Box->st_Max.z;
			st_Min.y = (i & 2) ? pst_Box->st_Min.y : pst_Box->st_Min.y + st_Diag.y;
			st_Max.y = (i & 2) ? pst_Box->st_Max.y - st_Diag.y : pst_Box->st_Max.y;
			st_Min.x = (i & 1) ? pst_Box->st_Min.x : pst_Box->st_Min.x + st_Diag.x;
			st_Max.x = (i & 1) ? pst_Box->st_Max.x - st_Diag.x : pst_Box->st_Max.x;
		}

		if(Mask == 6)	/* X and Y */
		{
			st_Min.z = pst_Box->st_Min.z;
			st_Max.z = pst_Box->st_Max.z;
			st_Min.y = (i & 1) ? pst_Box->st_Min.y : pst_Box->st_Min.y + st_Diag.y;
			st_Max.y = (i & 1) ? pst_Box->st_Max.y - st_Diag.y : pst_Box->st_Max.y;
			st_Min.x = (i & 2) ? pst_Box->st_Min.x : pst_Box->st_Min.x + st_Diag.x;
			st_Max.x = (i & 2) ? pst_Box->st_Max.x - st_Diag.x : pst_Box->st_Max.x;
		}

		if(Mask == 5)	/* X and Z */
		{
			st_Min.z = (i & 2) ? pst_Box->st_Min.z : pst_Box->st_Min.z + st_Diag.z;
			st_Max.z = (i & 2) ? pst_Box->st_Max.z - st_Diag.z : pst_Box->st_Max.z;
			st_Min.y = pst_Box->st_Min.y;
			st_Max.y = pst_Box->st_Max.y;
			st_Min.x = (i & 1) ? pst_Box->st_Min.x : pst_Box->st_Min.x + st_Diag.x;
			st_Max.x = (i & 1) ? pst_Box->st_Max.x - st_Diag.x : pst_Box->st_Max.x;
		}

		if(Mask == 3)	/* Y and Z */
		{
			st_Min.z = (i & 1) ? pst_Box->st_Min.z : pst_Box->st_Min.z + st_Diag.z;
			st_Max.z = (i & 1) ? pst_Box->st_Max.z - st_Diag.z : pst_Box->st_Max.z;
			st_Min.y = (i & 2) ? pst_Box->st_Min.y : pst_Box->st_Min.y + st_Diag.y;
			st_Max.y = (i & 2) ? pst_Box->st_Max.y - st_Diag.y : pst_Box->st_Max.y;
			st_Min.x = pst_Box->st_Min.x;
			st_Max.x = pst_Box->st_Max.x;
		}

		if(Mask == 1)	/* Z */
		{
			st_Min.z = (i & 1) ? pst_Box->st_Min.z : pst_Box->st_Min.z + st_Diag.z;
			st_Max.z = (i & 1) ? pst_Box->st_Max.z - st_Diag.z : pst_Box->st_Max.z;
			st_Min.y = pst_Box->st_Min.y;
			st_Max.y = pst_Box->st_Max.y;
			st_Min.x = pst_Box->st_Min.x;
			st_Max.x = pst_Box->st_Max.x;
		}

		if(Mask == 2)	/* Y */
		{
			st_Min.z = pst_Box->st_Min.z;
			st_Max.z = pst_Box->st_Max.z;
			st_Min.y = (i & 1) ? pst_Box->st_Min.y : pst_Box->st_Min.y + st_Diag.y;
			st_Max.y = (i & 1) ? pst_Box->st_Max.y - st_Diag.y : pst_Box->st_Max.y;
			st_Min.x = pst_Box->st_Min.x;
			st_Max.x = pst_Box->st_Max.x;
		}

		if(Mask == 4)	/* X */
		{
			st_Min.z = pst_Box->st_Min.z;
			st_Max.z = pst_Box->st_Max.z;
			st_Min.y = pst_Box->st_Min.y;
			st_Max.y = pst_Box->st_Max.y;
			st_Min.x = (i & 1) ? pst_Box->st_Min.x : pst_Box->st_Min.x + st_Diag.x;
			st_Max.x = (i & 1) ? pst_Box->st_Max.x - st_Diag.x : pst_Box->st_Max.x;
		}

		pst_NewBox = pst_OK3->pst_OK3_Boxes + pst_OK3->ul_NumBox - Loop + i;

		pst_Node->pst_OK3_Box = pst_NewBox;

		MATH_CopyVector(&pst_NewBox->st_Min, &st_Min);
		MATH_CopyVector(&pst_NewBox->st_Max, &st_Max);

		pst_NewBox->ul_NumElement = pst_Box->ul_NumElement;
		pst_NewBox->pst_OK3_Element = (COL_tdst_OK3_Element *) MEM_p_VMAlloc(pst_NewBox->ul_NumElement * sizeof(COL_tdst_OK3_Element));
		L_memset(pst_NewBox->pst_OK3_Element, 0, pst_NewBox->ul_NumElement * sizeof(COL_tdst_OK3_Element));

		for(j = 0; j < (int) pst_Box->ul_NumElement; j++)
		{
			uw_NumTriangles = 0;

			pst_OK3_Element = &pst_NewBox->pst_OK3_Element[j];
			pst_OK3_Element->uw_Element = pst_Box->pst_OK3_Element[j].uw_Element;
			pst_OK3_Element->uw_NumTriangle = 0;

			for(k = 0; k < pst_Box->pst_OK3_Element[j].uw_NumTriangle; k++)
			{
				COL_pst_OK3_GetTriangle
				(
					_p_Data,
					pst_OK3_Element->uw_Element,
					pst_Box->pst_OK3_Element[j].puw_OK3_Triangle[k],
					&T1,
					&T2,
					&T3,
					_b_ColMap
				);

				if(COL_OK3_TriangleTouchBox(T1, T2, T3, &st_Min, &st_Max))
				{
#ifdef JADEFUSION
					if (uw_NumTriangles < COL_OK3_MAX_TRIANGLES)
                    {
					    auw_Triangles[uw_NumTriangles++] = pst_Box->pst_OK3_Element[j].puw_OK3_Triangle[k];
                    }
                    else
                    {
                        ERR_OutputDebugString("[Warning] COL_OK3_SubdivideBox() - Triangle buffer is too small, some triangles will be skipped\n");
                    }
#else
					auw_Triangles[uw_NumTriangles++] = pst_Box->pst_OK3_Element[j].puw_OK3_Triangle[k];
#endif
				}
			}

			if(uw_NumTriangles)
			{
				pst_OK3_Element->uw_NumTriangle = uw_NumTriangles;
				pst_OK3_Element->puw_OK3_Triangle = (USHORT *) MEM_p_VMAlloc(uw_NumTriangles * sizeof(USHORT));
				L_memcpy(pst_OK3_Element->puw_OK3_Triangle, auw_Triangles, uw_NumTriangles * sizeof(USHORT));
			}
		}
	}


	/* Free the origin Box */
	for(i = 0; i < (int) _pst_OK3_Node->pst_OK3_Box->ul_NumElement; i++)
	{
		if(_pst_OK3_Node->pst_OK3_Box->pst_OK3_Element[i].puw_OK3_Triangle)
			MEM_Free(_pst_OK3_Node->pst_OK3_Box->pst_OK3_Element[i].puw_OK3_Triangle);
	}

	MEM_Free(_pst_OK3_Node->pst_OK3_Box->pst_OK3_Element);
	_pst_OK3_Node->pst_OK3_Box->pst_OK3_Element = NULL;
	_pst_OK3_Node->pst_OK3_Box->ul_NumElement = 0;

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_OK3_SubdivideBoxRecursive(void *_p_Data, COL_tdst_OK3_Node *_pst_OK3_Node, ULONG *_pul_Mask, BOOL _b_ColMap)
{
	if(!_pst_OK3_Node) return FALSE;

//	if(COL_OK3_GetNumberOfFacesInBox(_pst_OK3_Node->pst_OK3_Box) <= (_b_ColMap ? COL_ul_ColMap_OK3_Threshold : COL_ul_Geometric_OK3_Threshold))
	if(COL_OK3_GetNumberOfFacesInBox(_pst_OK3_Node->pst_OK3_Box) <= COL_ul_Geometric_OK3_Threshold)
		return FALSE;

	COL_OK3_OptimizeBoxBV(_pst_OK3_Node->pst_OK3_Box, _p_Data, _b_ColMap);
	COL_OK3_SubdivideBox(_p_Data, _pst_OK3_Node, _pul_Mask, _b_ColMap);

	if(_pst_OK3_Node->pst_Son)
	{
		COL_OK3_SubdivideBoxRecursive(_p_Data, _pst_OK3_Node->pst_Son, _pul_Mask, _b_ColMap);
	}

	if(_pst_OK3_Node->pst_Next)
	{
		COL_OK3_SubdivideBoxRecursive(_p_Data, _pst_OK3_Node->pst_Next, _pul_Mask, _b_ColMap);
	}

	return TRUE;
}


#ifdef ACTIVE_EDITORS
static ULONG	COL_OK3_Memory_ColMap = 0;
static ULONG	COL_OK3_RealMemory_ColMap = 0;
static ULONG	COL_OK3_Boxes_ColMap = 0;
static ULONG	COL_OK3_Total_ColMap = 0;

static ULONG	COL_OK3_Memory_Geometric = 0;
static ULONG	COL_OK3_RealMemory_Geometric = 0;
static ULONG	COL_OK3_Boxes_Geometric = 0;
static ULONG	COL_OK3_Total_Geometric = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_OK3_Summary(OBJ_tdst_GameObject *_pst_GO, void *_p_Data, BOOL _b_ColMap, BOOL _b_Log)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_IndexedTriangles	*_pst_Cob;
	GEO_tdst_Object				*_pst_Object;
	char						asz_Log[200];
	COL_tdst_OK3				*pst_OK3;
	COL_tdst_OK3_Box			*pst_OK3_Box;
	int							i, j, Faces;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_ColMap)
		_pst_Cob = (COL_tdst_IndexedTriangles *) _p_Data;
	else
		_pst_Object = (GEO_tdst_Object *) _p_Data;

	if(_b_Log)
	{
		if(_b_ColMap)
		{
			sprintf(asz_Log, "ColMap OK3 Summary for %s", _pst_GO->sz_Name);
			LINK_PrintStatusMsg(asz_Log);

			sprintf(asz_Log, "Total Faces: %u", _pst_Cob->l_NbFaces);
			LINK_PrintStatusMsg(asz_Log);
		}
		else
		{
			/*~~~~~~~~~~~~~~~*/
			BIG_INDEX	ul_Fat;
			/*~~~~~~~~~~~~~~~*/

			ul_Fat = LOA_ul_GetCurrentKey();
			ul_Fat = BIG_ul_SearchKeyToFat(ul_Fat);

			if(ul_Fat != BIG_C_InvalidIndex)
			{
				/*~~~~~~~~~~~~~~~~~~*/
				char	asz_Name[100];
				/*~~~~~~~~~~~~~~~~~~*/

				L_strcpy(asz_Name, BIG_FileName(ul_Fat));
				sprintf(asz_Log, "Geometric OK3 Summary for %s", asz_Name);
			}
			else
				sprintf(asz_Log, "Geometric OK3 Summary for <Unknown>");
			LINK_PrintStatusMsg(asz_Log);

			sprintf(asz_Log, "Total Points: %u", _pst_Object->l_NbPoints);
			LINK_PrintStatusMsg(asz_Log);
		}
	}

	if(_b_ColMap)
		pst_OK3 = _pst_Cob->pst_OK3;
	else
		pst_OK3 = _pst_Object->pst_OK3;

	if(_b_Log)
	{
		sprintf(asz_Log, "Total OK3 Boxes: %u", pst_OK3->ul_NumBox);
		LINK_PrintStatusMsg(asz_Log);
	}

	if(_b_ColMap)
	{
		COL_OK3_Memory_ColMap += 4;
		COL_OK3_Boxes_ColMap += pst_OK3->ul_NumBox;
	}
	else
	{
		COL_OK3_Memory_Geometric += 4;
		COL_OK3_Boxes_Geometric += pst_OK3->ul_NumBox;
	}

	for(i = 0; i < (int) pst_OK3->ul_NumBox; i++)
	{
		pst_OK3_Box = &pst_OK3->pst_OK3_Boxes[i];

		if(_b_ColMap)
			COL_OK3_Memory_ColMap += 28;
		else
			COL_OK3_Memory_Geometric += 28;

		Faces = 0;
		for(j = 0; j < (int) pst_OK3_Box->ul_NumElement; j++)
		{
			Faces += pst_OK3_Box->pst_OK3_Element[j].uw_NumTriangle;
			if(_b_ColMap)
				COL_OK3_Memory_ColMap += 4 + pst_OK3_Box->pst_OK3_Element[j].uw_NumTriangle * 2;
			else
				COL_OK3_Memory_Geometric += 4 + pst_OK3_Box->pst_OK3_Element[j].uw_NumTriangle * 2;
		}

		if(_b_Log)
		{
			sprintf(asz_Log, "-Box n°%u : %u Faces", i, Faces);
			LINK_PrintStatusMsg(asz_Log);
		}

		if(_b_ColMap)
			COL_OK3_Total_ColMap += Faces;
		else
			COL_OK3_Total_Geometric += Faces;
	}

	if(_b_ColMap)
	{
		COL_OK3_RealMemory_ColMap = COL_OK3_Memory_ColMap;
		COL_OK3_RealMemory_ColMap += (pst_OK3->ul_NumBox % COL_ul_Box_Granularity) * 28;
	}
	else
	{
		COL_OK3_RealMemory_Geometric = COL_OK3_Memory_Geometric;
		COL_OK3_RealMemory_Geometric += (pst_OK3->ul_NumBox % COL_ul_Box_Granularity) * 28;
	}
}

#endif

/*
 =======================================================================================================================
    Aim:    Build OK3 for the ColMap or the visual
 =======================================================================================================================
 */
void COL_OK3_Build(void *_p_Data, BOOL _b_ColMap, BOOL _b_ReBuild)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*_pst_GO;
	GEO_tdst_Object			*_pst_Object;
	COL_tdst_OK3_Element	*pst_OK3_Element;

	MATH_tdst_Vector		st_Min, st_Max, *pst_Point;
	MATH_tdst_Vector		st_Diagonal;
	int						i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_p_Data) return;

	if(_b_ColMap)
		_pst_GO = (OBJ_tdst_GameObject *) _p_Data;
	else
		_pst_Object = (GEO_tdst_Object *) _p_Data;


	/*$off*/
	if
	(
		_b_ColMap
	&&	(
			!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap) ||	
			!(_pst_GO->pst_Extended) ||
			!(_pst_GO->pst_Extended->pst_Col) ||
			!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap ||
			!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob ||
			(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles)
		)
	) return;


	/*$on*/
	if(_b_ColMap)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_IndexedTriangles			*pst_Cob;
		COL_tdst_ElementIndexedTriangles	*pst_Cob_Element;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Remove hypothetical OK3 Flag on the Cob; */
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Flag &= ~COL_C_Cob_OK3;

		pst_Cob = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->pst_TriangleCob;

		if(_b_ReBuild && pst_Cob->pst_OK3)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			int						i, j;
			COL_tdst_OK3_Box		*pst_Box;
			COL_tdst_OK3_Element	*pst_Element;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			for(i = 0; i < (int) pst_Cob->pst_OK3->ul_NumBox; i++)
			{
				pst_Box = &pst_Cob->pst_OK3->pst_OK3_Boxes[i];

				for(j = 0; j < (int) pst_Box->ul_NumElement; j++)
				{
					pst_Element = &pst_Box->pst_OK3_Element[j];

					if(pst_Element->puw_OK3_Triangle) MEM_Free(pst_Element->puw_OK3_Triangle);
				}

				MEM_Free(pst_Box->pst_OK3_Element);
			}

			MEM_Free(pst_Cob->pst_OK3->pst_OK3_Boxes);
			MEM_Free(pst_Cob->pst_OK3->paul_Tag);

			COL_OK3_RecursiveFree(pst_Cob->pst_OK3->pst_OK3_God);

			MEM_Free(pst_Cob->pst_OK3);

			pst_Cob->pst_OK3 = NULL;
		}

		if(pst_Cob->pst_OK3) 
		{
			/* Remove hypothetical OK3 Flag on the Cob; */
			((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Flag |= COL_C_Cob_OK3;
			return;
		}

		if(pst_Cob->l_NbFaces < COL_ul_ColMap_OK3_Threshold) return;

		MATH_CopyVector(&st_Min, &pst_Cob->dst_Point[0]);
		MATH_CopyVector(&st_Max, &pst_Cob->dst_Point[0]);

		/* Computes the OBBox that englobes perfectly the ColMap; */
		for(i = 0; i < (int) pst_Cob->l_NbPoints; i++)
		{
			pst_Point = &pst_Cob->dst_Point[i];

			if(pst_Point->x < st_Min.x) st_Min.x = pst_Point->x;
			if(pst_Point->y < st_Min.y) st_Min.y = pst_Point->y;
			if(pst_Point->z < st_Min.z) st_Min.z = pst_Point->z;

			if(pst_Point->x > st_Max.x) st_Max.x = pst_Point->x;
			if(pst_Point->y > st_Max.y) st_Max.y = pst_Point->y;
			if(pst_Point->z > st_Max.z) st_Max.z = pst_Point->z;
		}

		MATH_SubVector(&st_Diagonal, &st_Max, &st_Min);

		/* Object is too small to create an OK3 */
		if((st_Diagonal.x < COL_f_OK3_MinSizeX) && (st_Diagonal.y < COL_f_OK3_MinSizeY) && (st_Diagonal.z < COL_f_OK3_MinSizeZ)) return;

		pst_Cob->pst_OK3 = (COL_tdst_OK3 *) MEM_p_VMAlloc(sizeof(COL_tdst_OK3));

		pst_Cob->pst_OK3->paul_Tag = (ULONG *) MEM_p_VMAlloc(((pst_Cob->l_NbFaces >> 5) + 1) << 2);
		L_memset(pst_Cob->pst_OK3->paul_Tag, 0, ((pst_Cob->l_NbFaces >> 5) + 1) << 2);

		pst_Cob->pst_OK3->ul_NumBox = 1;
		pst_Cob->pst_OK3->pst_OK3_God = (COL_tdst_OK3_Node *) MEM_p_VMAlloc(sizeof(COL_tdst_OK3_Node));
		pst_Cob->pst_OK3->pst_OK3_Boxes = (COL_tdst_OK3_Box *) MEM_p_VMAlloc(COL_ul_Box_Granularity * sizeof(COL_tdst_OK3_Box));

		pst_Cob->pst_OK3->pst_OK3_God->pst_Next = NULL;
		pst_Cob->pst_OK3->pst_OK3_God->pst_Son = NULL;
		pst_Cob->pst_OK3->pst_OK3_God->pst_OK3_Box = &pst_Cob->pst_OK3->pst_OK3_Boxes[0];

		MATH_CopyVector(&pst_Cob->pst_OK3->pst_OK3_Boxes[0].st_Min, &st_Min);
		MATH_CopyVector(&pst_Cob->pst_OK3->pst_OK3_Boxes[0].st_Max, &st_Max);

		/* Initialize First OK3 box with all faces of cob. */
		pst_Cob->pst_OK3->pst_OK3_Boxes[0].ul_NumElement = pst_Cob->l_NbElements;
		pst_Cob->pst_OK3->pst_OK3_Boxes[0].pst_OK3_Element = (COL_tdst_OK3_Element *) MEM_p_VMAlloc(pst_Cob->l_NbElements * sizeof(COL_tdst_OK3_Element));

		pst_Cob_Element = pst_Cob->dst_Element;
		pst_OK3_Element = pst_Cob->pst_OK3->pst_OK3_Boxes[0].pst_OK3_Element;
		for(i = 0; i < (int) pst_Cob->l_NbElements; pst_Cob_Element++, pst_OK3_Element++, i++)
		{
			pst_OK3_Element->uw_Element = i;
			pst_OK3_Element->uw_NumTriangle = pst_Cob_Element->uw_NbTriangles;
			if(pst_Cob_Element->uw_NbTriangles)
			{
				pst_OK3_Element->puw_OK3_Triangle = (USHORT *) MEM_p_VMAlloc(pst_Cob_Element->uw_NbTriangles * sizeof(USHORT));

				for(j = 0; j < pst_Cob_Element->uw_NbTriangles; j++)
				{
					pst_OK3_Element->puw_OK3_Triangle[j] = j;
				}
			}
			else
				pst_OK3_Element->puw_OK3_Triangle = NULL;
		}

		COL_OK3_SubdivideBoxRecursive((void *) pst_Cob, pst_Cob->pst_OK3->pst_OK3_God, NULL, TRUE);

		/* Sets the OK3 Flag on the Cob */
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Flag |= COL_C_Cob_OK3;

		COL_OK3_Optimize(pst_Cob->pst_OK3, (void *) pst_Cob, _b_ColMap);

#ifdef ACTIVE_EDITORS
		COL_OK3_Summary(_pst_GO, pst_Cob, _b_ColMap, FALSE);
#endif
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_ElementIndexedTriangles	*pst_Geo_Element;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_b_ReBuild && _pst_Object->pst_OK3)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			int						i, j;
			COL_tdst_OK3_Box		*pst_Box;
			COL_tdst_OK3_Element	*pst_Element;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			for(i = 0; i < (int) _pst_Object->pst_OK3->ul_NumBox; i++)
			{
				pst_Box = &_pst_Object->pst_OK3->pst_OK3_Boxes[i];

				for(j = 0; j < (int) pst_Box->ul_NumElement; j++)
				{
					pst_Element = &pst_Box->pst_OK3_Element[j];

					if(pst_Element->puw_OK3_Triangle) MEM_Free(pst_Element->puw_OK3_Triangle);
				}

				MEM_Free(pst_Box->pst_OK3_Element);
			}

			MEM_Free(_pst_Object->pst_OK3->pst_OK3_Boxes);
			if(_pst_Object->pst_OK3->paul_Tag) MEM_Free(_pst_Object->pst_OK3->paul_Tag);

			COL_OK3_RecursiveFree(_pst_Object->pst_OK3->pst_OK3_God);

			MEM_Free(_pst_Object->pst_OK3);

			_pst_Object->pst_OK3 = NULL;
		}

		if(_pst_Object->pst_OK3) return;

		if(_pst_Object->l_NbPoints < COL_ul_Geometric_OK3_Threshold) return;

		MATH_CopyVector(&st_Min, &_pst_Object->dst_Point[0]);
		MATH_CopyVector(&st_Max, &_pst_Object->dst_Point[0]);

		/* Computes the OBBox that englobes perfectly the ColMap; */
		for(i = 0; i < (int) _pst_Object->l_NbPoints; i++)
		{
			pst_Point = &_pst_Object->dst_Point[i];

			if(pst_Point->x < st_Min.x) st_Min.x = pst_Point->x;
			if(pst_Point->y < st_Min.y) st_Min.y = pst_Point->y;
			if(pst_Point->z < st_Min.z) st_Min.z = pst_Point->z;

			if(pst_Point->x > st_Max.x) st_Max.x = pst_Point->x;
			if(pst_Point->y > st_Max.y) st_Max.y = pst_Point->y;
			if(pst_Point->z > st_Max.z) st_Max.z = pst_Point->z;
		}

		MATH_SubVector(&st_Diagonal, &st_Max, &st_Min);

		/* Object is too small to create an OK3 */
		if((st_Diagonal.x < 2.0f) && (st_Diagonal.y < 2.0f) && (st_Diagonal.z < 2.0f)) return;

		_pst_Object->pst_OK3 = (COL_tdst_OK3 *) MEM_p_VMAlloc(sizeof(COL_tdst_OK3));
		_pst_Object->pst_OK3->paul_Tag = NULL;
#ifdef ACTIVE_EDITORS
		if((_pst_Object->st_Id.i->ul_Type == GRO_Geometric) && _pst_Object->l_NbPoints)
		{
			_pst_Object->pst_OK3->paul_Tag = (ULONG *) MEM_p_VMAlloc(_pst_Object->l_NbPoints);
		}
#endif
		_pst_Object->pst_OK3->ul_NumBox = 1;
		_pst_Object->pst_OK3->pst_OK3_Boxes = (COL_tdst_OK3_Box *) MEM_p_VMAlloc(COL_ul_Box_Granularity * sizeof(COL_tdst_OK3_Box));

		MATH_CopyVector(&_pst_Object->pst_OK3->pst_OK3_Boxes[0].st_Min, &st_Min);
		MATH_CopyVector(&_pst_Object->pst_OK3->pst_OK3_Boxes[0].st_Max, &st_Max);

		_pst_Object->pst_OK3->pst_OK3_God = (COL_tdst_OK3_Node *) MEM_p_VMAlloc(sizeof(COL_tdst_OK3_Node));
		_pst_Object->pst_OK3->pst_OK3_God->pst_Next = NULL;
		_pst_Object->pst_OK3->pst_OK3_God->pst_Son = NULL;
		_pst_Object->pst_OK3->pst_OK3_God->pst_OK3_Box = &_pst_Object->pst_OK3->pst_OK3_Boxes[0];

		/* Initialize First OK3 box with all faces of cob. */
		_pst_Object->pst_OK3->pst_OK3_Boxes[0].ul_NumElement = _pst_Object->l_NbElements;
		_pst_Object->pst_OK3->pst_OK3_Boxes[0].pst_OK3_Element = (COL_tdst_OK3_Element *) MEM_p_VMAlloc(_pst_Object->l_NbElements * sizeof(COL_tdst_OK3_Element));

		pst_Geo_Element = _pst_Object->dst_Element;
		pst_OK3_Element = _pst_Object->pst_OK3->pst_OK3_Boxes[0].pst_OK3_Element;
		for(i = 0; i < (int) _pst_Object->l_NbElements; pst_Geo_Element++, pst_OK3_Element++, i++)
		{
			pst_OK3_Element->uw_Element = i;
			pst_OK3_Element->uw_NumTriangle = (USHORT) pst_Geo_Element->l_NbTriangles;
			if(pst_Geo_Element->l_NbTriangles)
			{
				pst_OK3_Element->puw_OK3_Triangle = (USHORT *) MEM_p_VMAlloc(pst_Geo_Element->l_NbTriangles * sizeof(USHORT));

				for(j = 0; j < pst_Geo_Element->l_NbTriangles; j++)
				{
					pst_OK3_Element->puw_OK3_Triangle[j] = j;
				}
			}
			else
				pst_OK3_Element->puw_OK3_Triangle = NULL;
		}

		COL_OK3_SubdivideBoxRecursive((void *) _pst_Object, _pst_Object->pst_OK3->pst_OK3_God, NULL, _b_ColMap);

		COL_OK3_Optimize(_pst_Object->pst_OK3, (void *) _pst_Object, _b_ColMap);

#ifdef ACTIVE_EDITORS
		COL_OK3_Summary(NULL, _pst_Object, FALSE, FALSE);
#endif
	}
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif


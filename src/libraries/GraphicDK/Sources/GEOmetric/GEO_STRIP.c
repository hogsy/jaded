/*$T GEO_STRIP.c GC! 1.081 05/09/00 15:13:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#define __GEO_STRIP_C__

#ifdef ACTIVE_EDITORS

#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEO_STRIP.h"
#ifdef JADEFUSION
#include "ENGine/Sources/OBJects/OBJaccess.h"
#endif
#include "MAD_mem/Sources/MAD_mem.h"
#include "MAD_loadSave/Sources/MAD_Struct_V0.h"
#include "SDK/Sources/BASe/ERRors/ERRasser.h"
#include "SDK/Sources/BASe/MEMory/MEM.h"
#include "SDK/Sources/BASe/CLIbrary/CLIstr.h"
#include "GEOmetric/GEO_SKIN.h"
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#define GEO_STRIP_VERSION_MASK	0xff000000
#define GEO_STRIP_VERSION		0x02000000


/************************************************************************ 
STRIPS
************************************************************************/



/*
 =======================================================================================================================
    update all strip list with vertex indexes. There is only one strip by element, and it begins with the first
    triangle, third edge.
 =======================================================================================================================
 */

/*unsigned long MAD_GetFace
(
 MAD_GeometricObject *MO,
 unsigned long       FaceNum,
 MAD_Face            **Face,
 unsigned long       *ElementNumber,
 unsigned long       *MaterialID
 )
u l MAD_GetNumberOfFace(MAD_GeometricObject *MO)*/

void GEO_STRIP_UnLink2Faces(MAD_Face *Face1 , MAD_Face *Face2 , ULONG FaceNum1 , ULONG FaceNum2)
{
	if (Face1->Undefined1 == FaceNum2) Face1->Undefined1 = MAD_NULL_INDEX;
	if (Face1->Undefined2 == FaceNum2) Face1->Undefined2 = MAD_NULL_INDEX;
	if (Face1->Undefined3 == FaceNum2) Face1->Undefined3 = MAD_NULL_INDEX;
	if (Face2->Undefined1 == FaceNum1) Face2->Undefined1 = MAD_NULL_INDEX;
	if (Face2->Undefined2 == FaceNum1) Face2->Undefined2 = MAD_NULL_INDEX;
	if (Face2->Undefined3 == FaceNum1) Face2->Undefined3 = MAD_NULL_INDEX;
}
void GEO_STRIP_Unlink_UnCompatible_Faces(MAD_GeometricObject *__pMO)
{
	ULONG FaceNum1 , FaceNum2 , FaceNumber;
	ULONG ElementNumber1 , ElementNumber2 , Prout;
	MAD_Face *Face1 , *Face2 ;
	FaceNumber = MAD_GetNumberOfFace(__pMO);
	for (FaceNum1  = 0 ; FaceNum1 < FaceNumber ; FaceNum1 ++)
	{
		MAD_GetFace( __pMO, FaceNum1 , &Face1 , &ElementNumber1 , &Prout );
		FaceNum2 = Face1->Undefined1 ;

		if (FaceNum2  != MAD_NULL_INDEX)
		{
			MAD_GetFace( __pMO, FaceNum2 , &Face2 , &ElementNumber2 , &Prout );
			if (Face1&&Face2&&(ElementNumber2 != ElementNumber1)) GEO_STRIP_UnLink2Faces(Face1 , Face2 , FaceNum1 , FaceNum2);
		}

		FaceNum2 = Face1->Undefined2 ;
		if (FaceNum2  != MAD_NULL_INDEX)
		{
			MAD_GetFace( __pMO, FaceNum2 , &Face2 , &ElementNumber2 , &Prout );
			if (Face1&&Face2&&(ElementNumber2 != ElementNumber1)) GEO_STRIP_UnLink2Faces(Face1 , Face2 , FaceNum1 , FaceNum2);
		}

		FaceNum2 = Face1->Undefined3 ;
		if (FaceNum2  != MAD_NULL_INDEX)
		{
			MAD_GetFace( __pMO, FaceNum2 , &Face2 , &ElementNumber2 , &Prout );
			if (Face1&&Face2&&(ElementNumber2 != ElementNumber1)) GEO_STRIP_UnLink2Faces(Face1 , Face2 , FaceNum1 , FaceNum2);
		}
	}
}

static ULONG FLIP;
static ULONG STRIP_DEPTH;
static ULONG STRIP_UNMARK;
static ULONG STRIP_MARK;
MAD_GeometricObject *MAD_OBJECT_LOCAL;
MAD_Face *GEO_STRIP_GetFace(ULONG FaceNum)
{
	ULONG FUCK;
	MAD_Face *Face1 ;
	MAD_GetFace( MAD_OBJECT_LOCAL, FaceNum , &Face1 , &FUCK, &FUCK);
	return Face1 ;
}
#define STRIP_WRITE_AW(a) if (p_AtomeWrier != NULL) {p_AtomeWrier->auw_Index = (unsigned short)CurrentFace->Index[a];(p_AtomeWrier++)->auw_UV = (unsigned short)CurrentFace->UVIndex[a];}
static ULONG COMPUTEFAN = 0;
static ULONG gs_ulArgument = 0;

void GEO_STRIP_GetStripDepth(ULONG Current, ULONG Previous , GEO_tdst_MinVertexData *p_AtomeWrier , GEO_tdst_UV  *p_MinUV , GEO_tdst_UV  *p_MaxUV)
{
	MAD_Face *CurrentFace;
	if (Current == MAD_NULL_INDEX) return;
	CurrentFace = GEO_STRIP_GetFace(Current);
	
	if (!CurrentFace) return;
	if (CurrentFace->Undefined0) return;	/* on  est déjá passé */
	if (MAD_OBJECT_LOCAL->NumberOfUV)
	{
		p_MaxUV->fU = fMax(p_MaxUV->fU , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[0]].x);
		p_MaxUV->fU = fMax(p_MaxUV->fU , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[1]].x);
		p_MaxUV->fU = fMax(p_MaxUV->fU , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[2]].x);
		p_MinUV->fU = fMin(p_MinUV->fU , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[0]].x);
		p_MinUV->fU = fMin(p_MinUV->fU , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[1]].x);
		p_MinUV->fU = fMin(p_MinUV->fU , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[2]].x);

		p_MaxUV->fV = fMax(p_MaxUV->fV , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[0]].y);
		p_MaxUV->fV = fMax(p_MaxUV->fV , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[1]].y);
		p_MaxUV->fV = fMax(p_MaxUV->fV , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[2]].y);
		p_MinUV->fV = fMin(p_MinUV->fV , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[0]].y);
		p_MinUV->fV = fMin(p_MinUV->fV , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[1]].y);
		p_MinUV->fV = fMin(p_MinUV->fV , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[2]].y);

		if (((p_MaxUV->fU - p_MinUV->fU) > 15.0f) ||
			((p_MaxUV->fV - p_MinUV->fV) > 15.0f))
		{
			return;
		}
	}

	{
		ULONG gs_ulArgumentSave;
		gs_ulArgumentSave = gs_ulArgument;
		if ((CurrentFace->Undefined1 == MAD_NULL_INDEX) || GEO_STRIP_GetFace(CurrentFace->Undefined1)->Undefined0) gs_ulArgument ++;
		if ((CurrentFace->Undefined2 == MAD_NULL_INDEX) || GEO_STRIP_GetFace(CurrentFace->Undefined2)->Undefined0) gs_ulArgument ++;
		if ((CurrentFace->Undefined3 == MAD_NULL_INDEX) || GEO_STRIP_GetFace(CurrentFace->Undefined3)->Undefined0) gs_ulArgument ++;//*/
		gs_ulArgument --;
	}
	STRIP_DEPTH++;
	FLIP ^= 1;
	if (COMPUTEFAN) FLIP = 0;
	CurrentFace->Undefined0 |= STRIP_MARK;/* On passe pour le test */
	if (FLIP)
	{
		if (CurrentFace->Undefined1 == Previous ) {STRIP_WRITE_AW(2); GEO_STRIP_GetStripDepth(CurrentFace->Undefined2 , Current , p_AtomeWrier , p_MinUV , p_MaxUV);};
		if (CurrentFace->Undefined2 == Previous ) {STRIP_WRITE_AW(0); GEO_STRIP_GetStripDepth(CurrentFace->Undefined3 , Current , p_AtomeWrier , p_MinUV , p_MaxUV);};
		if (CurrentFace->Undefined3 == Previous ) {STRIP_WRITE_AW(1); GEO_STRIP_GetStripDepth(CurrentFace->Undefined1 , Current , p_AtomeWrier , p_MinUV , p_MaxUV);};
	} else
	{
		if (CurrentFace->Undefined1 == Previous ) {STRIP_WRITE_AW(2); GEO_STRIP_GetStripDepth(CurrentFace->Undefined3 , Current , p_AtomeWrier , p_MinUV , p_MaxUV);};
		if (CurrentFace->Undefined2 == Previous ) {STRIP_WRITE_AW(0); GEO_STRIP_GetStripDepth(CurrentFace->Undefined1 , Current , p_AtomeWrier , p_MinUV , p_MaxUV);};
		if (CurrentFace->Undefined3 == Previous ) {STRIP_WRITE_AW(1); GEO_STRIP_GetStripDepth(CurrentFace->Undefined2 , Current , p_AtomeWrier , p_MinUV , p_MaxUV);};
	}
	CurrentFace->Undefined0 &= ~STRIP_UNMARK; /* On efface pour le test */
}
ULONG GEO_STRIP_GetStripDepthStart(MAD_GeometricObject *__pMO , ULONG FaceNum , ULONG SideTest , GEO_tdst_MinVertexData *p_AtomeWrier)
{
	GEO_tdst_UV  stMinUV , stMaxUV;
	STRIP_DEPTH = 0;
	gs_ulArgument = 0;
	if (GEO_STRIP_GetFace(FaceNum)->Undefined0) return 0;	/* on  est déjá passé */
	FLIP = 1;
	STRIP_DEPTH = 3;
	gs_ulArgument = 3;
	GEO_STRIP_GetFace(FaceNum)->Undefined0 |= STRIP_MARK;/* On passe pour le test */
	if (p_AtomeWrier)
	{
		p_AtomeWrier[0].auw_Index = (unsigned short)GEO_STRIP_GetFace(FaceNum)->Index[(0 + SideTest) % 3];
		p_AtomeWrier[1].auw_Index = (unsigned short)GEO_STRIP_GetFace(FaceNum)->Index[(1 + SideTest) % 3];
		p_AtomeWrier[2].auw_Index = (unsigned short)GEO_STRIP_GetFace(FaceNum)->Index[(2 + SideTest) % 3];

		p_AtomeWrier[0].auw_UV = (unsigned short)GEO_STRIP_GetFace(FaceNum)->UVIndex[(0 + SideTest) % 3];
		p_AtomeWrier[1].auw_UV = (unsigned short)GEO_STRIP_GetFace(FaceNum)->UVIndex[(1 + SideTest) % 3];
		p_AtomeWrier[2].auw_UV = (unsigned short)GEO_STRIP_GetFace(FaceNum)->UVIndex[(2 + SideTest) % 3];
		p_AtomeWrier+= 3;
	}
	if (MAD_OBJECT_LOCAL->NumberOfUV)
	{
		MAD_Face *CurrentFace;
		CurrentFace = GEO_STRIP_GetFace(FaceNum);

		stMaxUV.fU = MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[0]].x;
		stMaxUV.fU = fMax(stMaxUV.fU , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[1]].x);
		stMaxUV.fU = fMax(stMaxUV.fU , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[2]].x);
		stMinUV.fU = MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[0]].x;
		stMinUV.fU = fMin(stMinUV.fU , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[1]].x);
		stMinUV.fU = fMin(stMinUV.fU , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[2]].x);

		stMaxUV.fV = MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[0]].y;
		stMaxUV.fV = fMax(stMaxUV.fV , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[1]].y);
		stMaxUV.fV = fMax(stMaxUV.fV , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[2]].y);
		stMinUV.fV = MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[0]].y;
		stMinUV.fV = fMin(stMinUV.fV , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[1]].y);
		stMinUV.fV = fMin(stMinUV.fV , MAD_OBJECT_LOCAL->OBJ_UVtextureList[CurrentFace->UVIndex[2]].y);

	}

	if (COMPUTEFAN) 
	{
		if (SideTest == 0) GEO_STRIP_GetStripDepth(GEO_STRIP_GetFace(FaceNum)->Undefined3 , FaceNum, p_AtomeWrier , &stMinUV , &stMaxUV);
		if (SideTest == 1) GEO_STRIP_GetStripDepth(GEO_STRIP_GetFace(FaceNum)->Undefined1 , FaceNum, p_AtomeWrier , &stMinUV , &stMaxUV);
		if (SideTest == 2) GEO_STRIP_GetStripDepth(GEO_STRIP_GetFace(FaceNum)->Undefined2 , FaceNum, p_AtomeWrier , &stMinUV , &stMaxUV);
	} else {
		if (SideTest == 0) GEO_STRIP_GetStripDepth(GEO_STRIP_GetFace(FaceNum)->Undefined2 , FaceNum, p_AtomeWrier , &stMinUV , &stMaxUV);
		if (SideTest == 1) GEO_STRIP_GetStripDepth(GEO_STRIP_GetFace(FaceNum)->Undefined3 , FaceNum, p_AtomeWrier , &stMinUV , &stMaxUV);
		if (SideTest == 2) GEO_STRIP_GetStripDepth(GEO_STRIP_GetFace(FaceNum)->Undefined1 , FaceNum, p_AtomeWrier , &stMinUV , &stMaxUV);
	}


	GEO_STRIP_GetFace(FaceNum)->Undefined0 &= ~STRIP_UNMARK; /* On efface pour le test */

	gs_ulArgument &= 0x7fffffff;
	return STRIP_DEPTH;
}

ULONG GEO_STRIP_ComputeStripNumber(MAD_GeometricObject *__pMO , 	GEO_tdst_MinVertexData				*p_AllAtoms , GEO_tdst_OneStrip					*p_AllStrips , ULONG *p_ElementIndexes , void (*SetpercentOfJob)(float))
{
	ULONG FaceNum1 , FaceNumber , MaxLenght, VeryMaxLenght;
	ULONG FaceWinner , FaceWinnerSide , FaceWinnerFan;
	ULONG ulNumberOfStrips , ulNumberOfAtoms;
	ULONG ulNumberOFEradicatedTriangle , ulFanCounter , ulArgumentMax;
	ulNumberOfStrips = 0;
	ulNumberOfAtoms  = 0;
	FaceNumber = MAD_GetNumberOfFace(__pMO);
	MAD_OBJECT_LOCAL = __pMO ;
	for (FaceNum1 = 0 ; FaceNum1 < FaceNumber ; FaceNum1 ++)
	{
		GEO_STRIP_GetFace(FaceNum1)->Undefined0 = 0;
	}
	/* GetDepth Max */
	MaxLenght  = 1;
	VeryMaxLenght = 0;
	ulNumberOFEradicatedTriangle = 0;
	while (MaxLenght)
	{	
		MaxLenght = 0; 
		STRIP_UNMARK = 1;
		STRIP_MARK = 1;
		ulArgumentMax = 0;
		for (ulFanCounter = 1 ; ulFanCounter < 2 ; ulFanCounter++)
		{
			COMPUTEFAN = ulFanCounter ^ 1;
			for (FaceNum1 = 0 ; FaceNum1 < FaceNumber ; FaceNum1 ++)
			{
				ULONG SideCounter;
				for (SideCounter = 0 ; SideCounter < 3 ; SideCounter ++)
				{
					GEO_STRIP_GetStripDepthStart(__pMO , FaceNum1 , SideCounter ,NULL);
//					if (STRIP_DEPTH > ulArgumentMax) 
					if (gs_ulArgument > ulArgumentMax) 
					{
						FaceWinner = FaceNum1;
						FaceWinnerSide = SideCounter;
						FaceWinnerFan = COMPUTEFAN;
						MaxLenght = STRIP_DEPTH ;
						ulArgumentMax = gs_ulArgument;
					}
				}
			}
		}
		ulNumberOfAtoms += MaxLenght;
		ulNumberOFEradicatedTriangle += MaxLenght - 2 ;
		if (SetpercentOfJob) SetpercentOfJob((float)ulNumberOFEradicatedTriangle / (float)FaceNumber);
		VeryMaxLenght = lMax(MaxLenght , VeryMaxLenght);
		STRIP_UNMARK = 0;
		STRIP_MARK = 2;
		COMPUTEFAN = FaceWinnerFan;
		GEO_STRIP_GetStripDepthStart(__pMO , FaceWinner , FaceWinnerSide,p_AllAtoms);
		p_AllStrips->pMinVertexDataList = p_AllAtoms;
		p_AllStrips->ulVertexNumber = MaxLenght;
		{
			ULONG FUCK;
			MAD_Face *Face1 ;
			MAD_GetFace( __pMO , FaceWinner , &Face1 , p_ElementIndexes++ , &FUCK);
		}
		p_AllStrips++;
		p_AllAtoms += MaxLenght;
		ulNumberOfStrips++;
	}
	return ulNumberOfStrips - 1;
}
static void GEO_STRIP_GenerateStrip(GEO_tdst_Object *__pObject, MAD_GeometricObject *__pMO , void (*SetpercentOfJob)(float))
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pElement, *pLastElement;
	GEO_tdst_MinVertexData				*p_AllAtoms , *p_AllAtomsParser;
	GEO_tdst_OneStrip					*p_AllStrips;
	ULONG								*p_ElementIndexes;
	ULONG								ulNumberOfStrips;
	ULONG								Counter;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (SetpercentOfJob) SetpercentOfJob(0.0f);


	GEO_STRIP_Unlink_UnCompatible_Faces(__pMO);

	p_AllAtoms =		(GEO_tdst_MinVertexData*)MEM_p_Alloc(sizeof(GEO_tdst_MinVertexData) * MAD_GetNumberOfFace(__pMO) * 3);
	p_AllStrips =		(GEO_tdst_OneStrip*)MEM_p_Alloc(sizeof(GEO_tdst_OneStrip) * MAD_GetNumberOfFace(__pMO) * 3);
	p_ElementIndexes =	(ULONG*)MEM_p_Alloc(sizeof(ULONG) * MAD_GetNumberOfFace(__pMO) * 3);

	ulNumberOfStrips = GEO_STRIP_ComputeStripNumber(__pMO , p_AllAtoms , p_AllStrips , p_ElementIndexes , SetpercentOfJob);

	/* Allocate Element strip Structures */
	pElement = __pObject->dst_Element;
	pLastElement = pElement + __pObject->l_NbElements;
	while (pElement < pLastElement)
	{
		pElement->pst_StripData = (GEO_tdst_StripData*)MEM_p_Alloc(sizeof(GEO_tdst_StripData));
		L_memset(pElement->pst_StripData, 0, sizeof(GEO_tdst_StripData));
		pElement->pst_StripData->ulFlag = 0;
		pElement->pst_StripData->ulStripNumber = 0;
		pElement->pst_StripData->ulMaxLength = 0;
		pElement->pst_StripData->ulMinLength = 0xffffffff;
		pElement->pst_StripData->ulAveLength = 0;
		pElement++;
	}

	/* Counting number of strip per elements */
	pElement = __pObject->dst_Element;
	Counter = ulNumberOfStrips ;
	while (Counter--) pElement[p_ElementIndexes[Counter]].pst_StripData->ulStripNumber ++;

	/* Allocate strip per elements */
	pElement = __pObject->dst_Element;
	Counter = __pObject->l_NbElements;
	while (Counter--) 
	{
		pElement[Counter].pst_StripData->pStripList = (GEO_tdst_OneStrip*)MEM_p_Alloc(sizeof(GEO_tdst_OneStrip) * pElement[Counter].pst_StripData->ulStripNumber);
		L_memset(pElement[Counter].pst_StripData->pStripList, 0, sizeof(GEO_tdst_OneStrip) * pElement[Counter].pst_StripData->ulStripNumber);
		pElement[Counter].pst_StripData->ulStripNumber = 0;
	}

	/* Parse all the strips */
	p_AllAtomsParser = p_AllAtoms;
	for (Counter = 0 ; Counter < ulNumberOfStrips ; Counter++)
	{
		pElement = &__pObject->dst_Element[p_ElementIndexes[Counter]];
		pElement->pst_StripData->pStripList[pElement->pst_StripData->ulStripNumber].ulVertexNumber = p_AllStrips[Counter].ulVertexNumber;
		pElement->pst_StripData->pStripList[pElement->pst_StripData->ulStripNumber].pMinVertexDataList = (GEO_tdst_MinVertexData*)MEM_p_Alloc(sizeof(GEO_tdst_MinVertexData) * p_AllStrips[Counter].ulVertexNumber);
		memcpy(pElement->pst_StripData->pStripList[pElement->pst_StripData->ulStripNumber].pMinVertexDataList , p_AllAtomsParser , sizeof(GEO_tdst_MinVertexData) * p_AllStrips[Counter].ulVertexNumber);
		p_AllAtomsParser += p_AllStrips[Counter].ulVertexNumber;
		pElement->pst_StripData->ulStripNumber++;
	}

	MEM_Free(p_AllAtoms);
	MEM_Free(p_AllStrips);
	MEM_Free(p_ElementIndexes);

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_STRIP_ComputeStat(GEO_tdst_Object *__pst_Object, char *__asz)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pElement, *pLastElement;
	GEO_tdst_OneStrip					*pStrip, *pLastStrip;
	ULONG								ulNbOfPoints, ulNbOfStrip;
	ULONG								ulMaxLength, ulMinLength;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(__pst_Object->ulStripFlag & GEO_C_Strip_DataValid)) return;

	ulNbOfPoints = 0;
	ulNbOfStrip = 0;
	ulMaxLength = 0;
	ulMinLength = 0xffffffff;

	pElement = __pst_Object->dst_Element;
	pLastElement = pElement + __pst_Object->l_NbElements;

	for(; pElement < pLastElement; pElement++)
	{
		pElement->pst_StripData->ulMaxLength = 0;
		pElement->pst_StripData->ulMinLength = 0xffffffff;
		pElement->pst_StripData->ulAveLength = 0;

		pStrip = pElement->pst_StripData->pStripList;
		pLastStrip = pStrip + pElement->pst_StripData->ulStripNumber;
		for(; pStrip < pLastStrip; pStrip++)
		{
			pElement->pst_StripData->ulMaxLength = max(pElement->pst_StripData->ulMaxLength, pStrip->ulVertexNumber);
			pElement->pst_StripData->ulMinLength = min(pElement->pst_StripData->ulMinLength, pStrip->ulVertexNumber);
			pElement->pst_StripData->ulAveLength += pStrip->ulVertexNumber;
			ulNbOfPoints += pStrip->ulVertexNumber;
		}

		pElement->pst_StripData->ulAveLength /= pElement->pst_StripData->ulStripNumber;

		ulMaxLength = max(ulMaxLength, pElement->pst_StripData->ulMaxLength);
		ulMinLength = min(ulMinLength, pElement->pst_StripData->ulMinLength);
		ulNbOfStrip += pElement->pst_StripData->ulStripNumber;
	}

	ulNbOfPoints /= ulNbOfStrip;
	sprintf(__asz, "Max %d Min %d Ave %d Nb %d", ulMaxLength, ulMinLength, ulNbOfPoints, ulNbOfStrip);

	/* save flags only */
	__pst_Object->ulStripFlag &= 0x000000ff | GEO_STRIP_VERSION_MASK;

	/* put stat */
	ulNbOfPoints <<= 8;
	__pst_Object->ulStripFlag |= ulNbOfPoints;

	GEO_STRIP_SetFlag(GEO_C_Strip_StatData, __pst_Object);
}

void RestoreNonMagnifoldIndexes(GEO_tdst_Object *__pst_Object ,ULONG *p_Reorder)
{
	GEO_tdst_ElementIndexedTriangles	*pElement , *pLastElement;
	GEO_tdst_IndexedTriangle *p_Face,*p_FaceLast;
	/* ------------------------- */
	/* E: Reordering	 		 */
	/* ------------------------- */
	pElement = __pst_Object->dst_Element;
	pLastElement = pElement + __pst_Object->l_NbElements;
	while (pElement < pLastElement)
	{
		p_Face = pElement->dst_Triangle;
		p_FaceLast = p_Face + pElement->l_NbTriangles;
		while (p_Face < p_FaceLast )
		{
			p_Face ->auw_Index[0] = (unsigned short)p_Reorder[p_Face ->auw_Index[0]];
			p_Face ->auw_Index[1] = (unsigned short)p_Reorder[p_Face ->auw_Index[1]];
			p_Face ->auw_Index[2] = (unsigned short)p_Reorder[p_Face ->auw_Index[2]];
			p_Face++;
		}
		if (pElement->pst_StripData)
		{
			GEO_tdst_OneStrip	*pStripList,*pStripListLast;
			GEO_tdst_MinVertexData *pNodeList,*pNodeListLast;
			pStripList = pElement->pst_StripData->pStripList;
			pStripListLast = pStripList + pElement->pst_StripData->ulStripNumber;
			while (pStripList < pStripListLast)
			{
				pNodeList = pStripList->pMinVertexDataList;
				pNodeListLast = pNodeList + pStripList->ulVertexNumber;
				while (pNodeList  < pNodeListLast )
				{
					pNodeList  ->auw_Index = (unsigned short)p_Reorder[pNodeList  ->auw_Index ];
					pNodeList  ++;
				}
				pStripList ++;
			}
		}
		pElement ++;
	}
}
/*
 =======================================================================================================================
    Compute the UV to obtain only one value by vertex, then compute the neightbouring of vertices, and finally create
    the strip list
 =======================================================================================================================
 */
#ifdef JADEFUSION
extern void COL_OK3_Build(void *_p_Data, BOOL _b_ColMap, BOOL _b_ReBuild);
#else
extern void COL_OK3_Build(void *, BOOL,BOOL);
#endif
void GEO_STRIP_ComputeTrianglesFromStrips(GEO_tdst_Object *_pst_Object)
{
	GEO_tdst_ElementIndexedTriangles	*pst_Element , *pst_ElementLast;

	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object ->l_NbElements;
	while (pst_Element < pst_ElementLast)
	{
		ULONG 						NumberOfTriangles;
		GEO_tdst_OneStrip			*pStripList,*pStripLast;
		GEO_tdst_IndexedTriangle	*pTriangle;
		USHORT						p1,p2,p3,u1,u2,u3,bFlip;
		if (!pst_Element -> pst_StripData) {pst_Element++;continue;};

		pStripList = pst_Element -> pst_StripData -> pStripList;
		pStripLast = pStripList + pst_Element -> pst_StripData -> ulStripNumber;
		NumberOfTriangles = 0;
		while (pStripList < pStripLast)
		{
			NumberOfTriangles += pStripList->ulVertexNumber - 2;
			pStripList ++;
		}

		if (NumberOfTriangles != pst_Element ->l_NbTriangles)
			NumberOfTriangles  = NumberOfTriangles + 1; // BUG!

		pTriangle = pst_Element ->dst_Triangle;
		L_memset(pTriangle , 0 , sizeof(GEO_tdst_IndexedTriangle) * NumberOfTriangles);

		pStripList = pst_Element -> pst_StripData -> pStripList;
		while (pStripList < pStripLast)
		{
			GEO_tdst_MinVertexData		*pFirstAtom,*pLasttAtom;
			pFirstAtom = pStripList->pMinVertexDataList;
			pLasttAtom = pFirstAtom + pStripList->ulVertexNumber;
			bFlip = 0;
			p1 = (pFirstAtom)->auw_Index;
			u1 = (pFirstAtom++)->auw_UV;
			p2 = (pFirstAtom)->auw_Index;
			u2 = (pFirstAtom++)->auw_UV;
			while (pFirstAtom < pLasttAtom)
			{
				p3 = pFirstAtom->auw_Index;
				u3 = pFirstAtom->auw_UV;
				if (bFlip)
				{
					pTriangle->auw_Index[0] = p2;
					pTriangle->auw_Index[1] = p1;
					pTriangle->auw_UV[0] = u2;
					pTriangle->auw_UV[1] = u1;
				} else
				{
					pTriangle->auw_Index[0] = p1;
					pTriangle->auw_Index[1] = p2;
					pTriangle->auw_UV[0] = u1;
					pTriangle->auw_UV[1] = u2;
				} 
				pTriangle->auw_Index[2] = p3;
				pTriangle->auw_UV[2] = u3;
				bFlip ^= 1;
				u1 = u2;
				u2 = u3;
				p1 = p2;
				p2 = p3;
				pTriangle++;
				pFirstAtom++;
			}
			pStripList ++;
		}
		pst_Element++;
	}
}


/*
 =======================================================================================================================
    Compute the UV to obtain only one value by vertex, then compute the neightbouring of vertices, and finally create
    the strip list
 =======================================================================================================================
 */
#ifdef JADEFUSION
extern void COL_OK3_Build(void *_p_Data, BOOL _b_ColMap, BOOL _b_ReBuild);
#else
extern void COL_OK3_Build(void *, BOOL,BOOL);
#endif
BOOL GEO_STRIP_ComputeLevel(OBJ_tdst_GameObject *pst_GAO,GEO_tdst_Object *__pst_Object , ULONG *p_AdditionalVertexColor , BOOL _b_Msg , void (*SetpercentOfJob)(float))
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_GeometricObject *p_MadObject;
	GEO_tdst_Object		*pst_NewObject;
	GEO_tdst_Object		Swp;
	GRO_tdst_Struct		GRO_Keeper;
	MATH_tdst_Vector	*p_SaveNormals;
	ULONG		*pReorder;
	ULONG		ulSavedulNBPoint;
	extern void GEO_Clean_After_strips(OBJ_tdst_GameObject *pst_GO , GEO_tdst_Object *__pst_Object , ULONG *p_AdditionalVertexColor);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if (!__pst_Object->l_NbElements) return FALSE;
	if (!__pst_Object->l_NbPoints) return FALSE;
	if(__pst_Object->ulStripFlag & GEO_C_Strip_DataValid) 
	{
		if ((__pst_Object->ulStripFlag & GEO_STRIP_VERSION_MASK) == GEO_STRIP_VERSION) return TRUE;
	}

	if (GEO_MRM_ul_IsMrmObject(__pst_Object))
	{
		GEO_MRM_Destroy(__pst_Object);
	}

	GEO_Clean_After_strips(pst_GAO , __pst_Object , p_AdditionalVertexColor);
    if (!__pst_Object->l_NbElements) return FALSE;
	if (!__pst_Object->l_NbPoints) return FALSE;
	Mad_meminit();

	/* Jade -> MAD format */
	p_SaveNormals = (MATH_tdst_Vector*)MEM_p_Alloc(sizeof(MATH_tdst_Vector) * __pst_Object->l_NbPoints);
	memcpy(p_SaveNormals , __pst_Object->dst_PointNormal , sizeof(MATH_tdst_Vector) * __pst_Object->l_NbPoints);
	p_MadObject = (MAD_GeometricObject *) __pst_Object->st_Id.i->pfnp_ToMad(__pst_Object, NULL);
	pReorder = NULL;
	ulSavedulNBPoint = p_MadObject->NumberOfPoints;
/*	if(!MAD_IsMagnifold(p_MadObject))
	{
		pReorder = MAD_MakeItMagnifold(p_MadObject );
	}
    */

	/* 2 consecutive elements that use the same material are merge in one element */
	MAD_ReComputeElements(p_MadObject);

	p_MadObject->RealNumberOfPoints = __pst_Object->l_NbPoints;

	/* compute one index per UV per element */
	MAD_ComputeOneIndexPerUVPerElementSolution(p_MadObject, 1);
	MAD_OptimizeElements(p_MadObject);
	MAD_RestoreOneIndexPerUVPerElementSolution(p_MadObject);

	/* compute the object neightbouring */
	MAD_ComputeNeightbouringOnObject(p_MadObject);

	/* convert from MAD to Jade */
	pst_NewObject = (GEO_tdst_Object*)__pst_Object->st_Id.i->pfnp_CreateFromMad(p_MadObject);

	/*
	 * we delete the old GEO object, but saving st_Id, and p_SKN_Objectponderation 1)
	 * __pst_Object <-> *pst_NewObject 2) save st_Id 3) save p_SKN_Objectponderation
	 * 4) remove *pst_NewObject
	 */

	/* 1 */
	Swp = *__pst_Object;
	*__pst_Object = *pst_NewObject;
	*pst_NewObject = Swp;

	/* 2 */
	GRO_Keeper = __pst_Object->st_Id;
	__pst_Object->st_Id = pst_NewObject->st_Id;
	pst_NewObject->st_Id = GRO_Keeper;

	/* 3 */
	__pst_Object->p_SKN_Objectponderation = pst_NewObject->p_SKN_Objectponderation;

	/* 4 */
	pst_NewObject->p_SKN_Objectponderation = NULL;
	pst_NewObject->st_Id.i->pfn_Destroy(pst_NewObject);

	ERR_X_Assert(__pst_Object->dst_Element->pst_StripData == NULL);
	GEO_STRIP_GenerateStrip(__pst_Object, p_MadObject , SetpercentOfJob);

	/* reset all flags */
	GEO_STRIP_UnSetFlag(0xffffffff, __pst_Object);

	/* set validity */
	GEO_STRIP_SetFlag(GEO_C_Strip_DataValid, __pst_Object);

	memcpy(__pst_Object->dst_PointNormal , p_SaveNormals , sizeof(MATH_tdst_Vector) * __pst_Object->l_NbPoints);
	MEM_Free(p_SaveNormals );

	if (pReorder)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		/* restore non magnifold state this must absolutely be done before GEO_Clean */
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		LONG       Counter; 
		/* Retreive real number of points  */
		for(Counter = 0; Counter < __pst_Object->l_NbPoints; Counter++)
		{
			if (pReorder[Counter] == 0xffffffff) pReorder[Counter] = Counter;
		}
		__pst_Object->l_NbPoints = ulSavedulNBPoint ;
		RestoreNonMagnifoldIndexes(__pst_Object,pReorder);
		free(pReorder);
	}
	/* Optimize number of elements, & merge equal points */
	GEO_Clean_After_strips(pst_GAO , __pst_Object , p_AdditionalVertexColor);

	Mad_free();
	
	/* Finaly recompute OK3 */
	/* 5 Recreate triangles from strips */
	GEO_STRIP_ComputeTrianglesFromStrips(__pst_Object);
	/* 6 (Re)Compute OK3 */
	COL_OK3_Build((void *)__pst_Object, FALSE, TRUE);

	/* Check some bug relative to SPG2 modifier */
	if (pst_GAO)
	{
		if (!OBJ_b_TestIdentityFlag(pst_GAO, (OBJ_C_IdentityFlag_Dyna|OBJ_C_IdentityFlag_Anims|OBJ_C_IdentityFlag_Bone|OBJ_C_IdentityFlag_Generated|OBJ_C_IdentityFlag_AI))) 
		{	
			/* 1) disable "do not cache" */
			if ((pst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (pst_GAO->pst_Extended))
			{
				MDF_tdst_Modifier  *pst_Modifier;
				pst_Modifier = pst_GAO->pst_Extended->pst_Modifiers;
				while(pst_Modifier)
				{
					if (pst_Modifier->i->ul_Type == MDF_C_Modifier_SpriteMapper2) 
					{
						SPG2_tdst_Modifier				*_pst_SPG2;
						_pst_SPG2 = (SPG2_tdst_Modifier *) pst_Modifier->p_Data;
						_pst_SPG2 ->ulFlags &= ~SPG2_DontUseCache;
					}
					pst_Modifier = pst_Modifier->pst_Next;
				}
			}

			/* 2) disable compute Skinning in draw mask */
			if (GEO_SKN_IsSkinned(__pst_Object))
			{
				pst_GAO->pst_Base->pst_Visu->ul_DrawMask &= ~GDI_Cul_DM_ActiveSkin;
			}
		}
	}


	return TRUE;
}

BOOL GEO_STRIP_Compute(OBJ_tdst_GameObject *pst_GAO,GEO_tdst_Object *__pst_Object , ULONG *p_AdditionalVertexColor , BOOL _b_Msg , void (*SetpercentOfJob)(float))
{
    if (__pst_Object->p_MRM_Levels)
    {
        int i,j,k,l,TotalElementNb;
        unsigned short *dlUVReorder;
        GEO_tdst_Object *pObjectLevel0;
        GEO_tdst_Object **ddObjects = (GEO_tdst_Object **)MEM_p_Alloc(sizeof(GEO_tdst_Object*) * __pst_Object->p_MRM_Levels->ul_LevelNb);

        // Split object (__pst_Object) in as many objects as the number of levels (ddObjects).
        for (i=0; i<(int)__pst_Object->p_MRM_Levels->ul_LevelNb; i++)
        {
            GEO_tdst_Object *pObject; 
            if (__pst_Object->dul_PointColors)
                pObject = GEO_pst_Create(__pst_Object->l_NbPoints,__pst_Object->l_NbUVs,__pst_Object->p_MRM_Levels->dl_ElementNb[i],*(__pst_Object->dul_PointColors));
            else
                pObject = GEO_pst_Create(__pst_Object->l_NbPoints,__pst_Object->l_NbUVs,__pst_Object->p_MRM_Levels->dl_ElementNb[i],0);

            ddObjects[i] = pObject;

            // Copy data from level to object
            pObject->st_Id = __pst_Object->st_Id;
            ERR_X_Assert(pObject->l_NbPoints == __pst_Object->l_NbPoints);
            L_memcpy(pObject->dst_Point, __pst_Object->dst_Point, sizeof(GEO_Vertex) * __pst_Object->l_NbPoints);
            L_memcpy(pObject->dst_PointNormal, __pst_Object->dst_PointNormal, sizeof(MATH_tdst_Vector) * __pst_Object->l_NbPoints);
            if (__pst_Object->dul_PointColors)
                L_memcpy(pObject->dul_PointColors, __pst_Object->dul_PointColors, sizeof(ULONG) * (__pst_Object->l_NbPoints + 1));
            ERR_X_Assert(pObject->l_NbUVs == __pst_Object->l_NbUVs);
            L_memcpy(pObject->dst_UV, __pst_Object->dst_UV, sizeof(GEO_tdst_UV) * (__pst_Object->l_NbUVs ));
            ERR_X_Assert(pObject->l_NbElements == __pst_Object->p_MRM_Levels->dl_ElementNb[i]);
            for (j=0; j<pObject->l_NbElements ; j++)
            {
                GEO_tdst_ElementIndexedTriangles *pElementSrc = (__pst_Object->p_MRM_Levels->aa_Element[i])+j;
                GEO_tdst_ElementIndexedTriangles *pElementDst = pObject->dst_Element+j;

                pElementDst->l_NbTriangles = pElementSrc->l_NbTriangles;
                pElementDst->l_MaterialId = (pElementSrc->l_MaterialId & 0xFF);
                if (pElementSrc->l_NbTriangles)
                {   
                    pElementDst->dst_Triangle = (GEO_tdst_IndexedTriangle* )MEM_p_Alloc((pElementSrc->l_NbTriangles+1) * sizeof(GEO_tdst_IndexedTriangle));
                    L_memcpy(pElementDst->dst_Triangle,pElementSrc->dst_Triangle,(pElementSrc->l_NbTriangles+1) * sizeof(GEO_tdst_IndexedTriangle));
                }
                else 
                    pElementDst->dst_Triangle = NULL;

                ERR_X_Assert(pElementSrc->ul_NumberOfUsedIndex == 0);
                ERR_X_Assert(pElementSrc->pus_ListOfUsedIndex == NULL);
                ERR_X_Assert(pElementSrc->p_MrmElementAdditionalInfo == NULL);
                ERR_X_Assert(pElementSrc->pst_StripData == NULL);
                ERR_X_Assert(pElementSrc->pst_Gx8Add == NULL);
            }
            //ERR_X_Assert(__pst_Object->pst_BV == NULL);
            ERR_X_Assert(__pst_Object->p_MRM_ObjectAdditionalInfo == NULL);
            //ERR_X_Assert(__pst_Object->p_SKN_Objectponderation == NULL);
            ERR_X_Assert(__pst_Object->pst_SubObject == NULL);
            pObject->ul_EditorFlags = __pst_Object->ul_EditorFlags;
            pObject->ulStripFlag = __pst_Object->ulStripFlag;
            ERR_X_Assert(__pst_Object->l_NbSpritesElements == 0);
            ERR_X_Assert(__pst_Object->dst_SpritesElements == NULL);
            //ERR_X_Assert(__pst_Object->pst_OK3 == NULL);
            //ERR_X_Assert(__pst_Object->pBiNormales == NULL);
        }

        // Compute strips on all levels
        for (i=0; i<(int)__pst_Object->p_MRM_Levels->ul_LevelNb; i++)
        {
            GEO_tdst_Object *pObject = ddObjects[i];

            BOOL bSuccess = GEO_STRIP_ComputeLevel(pst_GAO,pObject, p_AdditionalVertexColor , _b_Msg , SetpercentOfJob);
            ERR_X_Assert(bSuccess);
        }

        // Check that the points have not been changed
        for (i=0; i<(int)__pst_Object->p_MRM_Levels->ul_LevelNb; i++)
        {
            GEO_tdst_Object *pObject = ddObjects[i];
            ERR_X_Assert(__pst_Object->l_NbPoints == pObject->l_NbPoints);
        }

        for (i=0; i<(int)__pst_Object->p_MRM_Levels->ul_LevelNb; i++)
        {
            GEO_tdst_Object *pObject = ddObjects[i];
            ERR_X_Assert(0 == L_memcmp(pObject->dst_Point,__pst_Object->dst_Point,__pst_Object->l_NbPoints * sizeof(GEO_Vertex)));
        }

        // Destroy previous elements and UV in __pst_Object.
        pObjectLevel0 = ddObjects[0];
        for (i=0; i<__pst_Object->l_NbElements; i++)
        {
            GEO_tdst_ElementIndexedTriangles *pElements = __pst_Object->dst_Element + i;
            GEO_FreeElementContent(pElements);
        }
        MEM_Free(__pst_Object->dst_Element);
        MEM_Free(__pst_Object->dst_UV);
        MEM_Free(__pst_Object->p_MRM_Levels->dus_ReorderBuffer);
        __pst_Object->p_MRM_Levels->dus_ReorderBuffer = NULL;

        // Create new elements and UV in __pst_Object.
        TotalElementNb = 0;
        for (i=0; i<(int)__pst_Object->p_MRM_Levels->ul_LevelNb; i++)
        {
            GEO_tdst_Object *pObject = ddObjects[i];
            TotalElementNb += pObject->l_NbElements;
        }

        __pst_Object->dst_Element = (GEO_tdst_ElementIndexedTriangles* )MEM_p_Alloc(sizeof(GEO_tdst_ElementIndexedTriangles) * TotalElementNb);
        L_memset(__pst_Object->dst_Element,0,sizeof(GEO_tdst_ElementIndexedTriangles) * TotalElementNb);
        __pst_Object->p_MRM_Levels->a_ElementBase = __pst_Object->dst_Element;
        __pst_Object->p_MRM_Levels->l_TotalElementNb = TotalElementNb;
        __pst_Object->l_NbElements = TotalElementNb;
        // UVs
        __pst_Object->l_NbUVs = pObjectLevel0->l_NbUVs;
        if (pObjectLevel0->l_NbUVs>0)
            __pst_Object->dst_UV = (GEO_tdst_UV* )MEM_p_Alloc(pObjectLevel0->l_NbUVs * sizeof(GEO_tdst_UV));
        else
            __pst_Object->dst_UV = NULL;
        for (i=0; i<pObjectLevel0->l_NbUVs; i++)
        {
            __pst_Object->dst_UV[i] = pObjectLevel0->dst_UV[i];
        }

        // Compute and apply indirection between level 0 and other levels for vertex and UV,
        // and merge level objects (ddObjects) into one (__pst_Object). 
        if (pObjectLevel0->l_NbUVs>0)
            dlUVReorder = (unsigned short *) MEM_p_Alloc(sizeof(unsigned short) *pObjectLevel0->l_NbUVs);
        else
            dlUVReorder = NULL;
        TotalElementNb = 0;
        for (i=0; i<(int)__pst_Object->p_MRM_Levels->ul_LevelNb; i++)
        {
            GEO_tdst_Object *pObject = ddObjects[i];

            __pst_Object->p_MRM_Levels->aa_Element[i] = __pst_Object->dst_Element + TotalElementNb;
            __pst_Object->p_MRM_Levels->dl_ElementNb[i] = pObject->l_NbElements;
            TotalElementNb += pObject->l_NbElements;
            /*
            ULONG *dlVertexReorder = MEM_p_Alloc(sizeof(ULONG) *pObjectLevel0->l_NbPoints);

            // Vertex reorder
            for (j=0; j<pObjectLevel0->l_NbPoints; j++)
            {
                dlVertexReorder[j] == 0xFFFFFFFF;
                for (k=0; k<pObject->l_NbPoints; k++)
                {
                    if (pObjectLevel0->dst_Point[j] == pObject->dst_Point[k])
                    {
                        dlVertexReorder[j] = k;
                        break;
                    }
                }
                ERR_X_Assert(dlVertexReorder[j] != 0xFFFFFFFF);
            }*/

            // UV reorder (from level i to level 0)
            if (i == 0)
            {
                for (j=0; j<pObjectLevel0->l_NbUVs; j++)
                    dlUVReorder[j] = j;
            }
            else
            {
                for (j=0; j<pObject->l_NbUVs; j++)
                {
                    dlUVReorder[j] = 0xFFFF;
                    for (k=0; k<__pst_Object->l_NbUVs; k++)
                    {
                        if ((pObject->dst_UV[j].fU == __pst_Object->dst_UV[k].fU) &&
                            (pObject->dst_UV[j].fV == __pst_Object->dst_UV[k].fV))
                        {
                            dlUVReorder[j] = k;
                            break;
                        }
                    }

                    // If we couln't find the UV in the UV buffer (should be impossible, but ...),
                    // add the UV in the UV buffer.
                    if (dlUVReorder[j] == 0xFFFF)
                    {
                        dlUVReorder[j] = (unsigned short) __pst_Object->l_NbUVs;
                        __pst_Object->l_NbUVs++;
                        __pst_Object->dst_UV = (GEO_tdst_UV *) MEM_p_Realloc(__pst_Object->dst_UV,__pst_Object->l_NbUVs * sizeof(GEO_tdst_UV));
                        __pst_Object->dst_UV[__pst_Object->l_NbUVs-1].fU = pObject->dst_UV[j].fU;
                        __pst_Object->dst_UV[__pst_Object->l_NbUVs-1].fV = pObject->dst_UV[j].fV;
                    }

                    ERR_X_Assert(dlUVReorder[j] != 0xFFFF);
                }
            }

            for (j=0; j<pObject->l_NbElements; j++)
            {
                GEO_tdst_ElementIndexedTriangles *pSrcElement = pObject->dst_Element+j;
                GEO_tdst_ElementIndexedTriangles *pDstElement = __pst_Object->p_MRM_Levels->aa_Element[i] + j;

		        pDstElement->l_NbTriangles = pSrcElement->l_NbTriangles ;
		        GEO_AllocElementContent(pDstElement);

                for (k=0; k<pDstElement->l_NbTriangles; k++)
                {
                    GEO_tdst_IndexedTriangle *pSrcTriangle = pSrcElement->dst_Triangle + k;
                    GEO_tdst_IndexedTriangle *pDstTriangle = pDstElement->dst_Triangle + k;
                    pDstTriangle->auw_Index[0] = pSrcTriangle->auw_Index[0];
                    pDstTriangle->auw_Index[1] = pSrcTriangle->auw_Index[1];
                    pDstTriangle->auw_Index[2] = pSrcTriangle->auw_Index[2];
                    if (dlUVReorder)
                    {
                        ERR_X_Assert(pSrcTriangle->auw_UV[0] < pObject->l_NbUVs);
                        ERR_X_Assert(pSrcTriangle->auw_UV[1] < pObject->l_NbUVs);
                        ERR_X_Assert(pSrcTriangle->auw_UV[2] < pObject->l_NbUVs);
                        pDstTriangle->auw_UV[0] = dlUVReorder[pSrcTriangle->auw_UV[0]];
                        pDstTriangle->auw_UV[1] = dlUVReorder[pSrcTriangle->auw_UV[1]];
                        pDstTriangle->auw_UV[2] = dlUVReorder[pSrcTriangle->auw_UV[2]];
                        ERR_X_Assert(pDstTriangle->auw_UV[0] < __pst_Object->l_NbUVs);
                        ERR_X_Assert(pDstTriangle->auw_UV[1] < __pst_Object->l_NbUVs);
                        ERR_X_Assert(pDstTriangle->auw_UV[2] < __pst_Object->l_NbUVs);
                    }
                    else
                    {
                        pDstTriangle->auw_UV[0] = pSrcTriangle->auw_UV[0];
                        pDstTriangle->auw_UV[1] = pSrcTriangle->auw_UV[1];
                        pDstTriangle->auw_UV[2] = pSrcTriangle->auw_UV[2];
                    }
                    pDstTriangle->ul_MaxFlags = pSrcTriangle->ul_MaxFlags;
                    pDstTriangle->ul_SmoothingGroup = pSrcTriangle->ul_SmoothingGroup ;
                }

                pDstElement->l_MaterialId = pSrcElement->l_MaterialId | (i<<8);
                pDstElement->pst_StripData = pSrcElement->pst_StripData;
                pSrcElement->pst_StripData = NULL;

                for (k=0; k<(int)pDstElement->pst_StripData->ulStripNumber; k++)
                {
                    GEO_tdst_OneStrip *pCurrentStrip = pDstElement->pst_StripData->pStripList+k;
                    for (l=0; l<(int)pCurrentStrip->ulVertexNumber; l++)
                    {
                        GEO_tdst_MinVertexData *pVertex = pCurrentStrip->pMinVertexDataList + l;
                        if (dlUVReorder)
                        {
                            ERR_X_Assert(pVertex->auw_UV < pObject->l_NbUVs);
                            pVertex->auw_UV = dlUVReorder[pVertex->auw_UV];
                            ERR_X_Assert(pVertex->auw_UV < __pst_Object->l_NbUVs);
                        }
                    }
                }

                ERR_X_Assert(pSrcElement->ul_NumberOfUsedIndex == 0);
                ERR_X_Assert(pSrcElement->pus_ListOfUsedIndex == NULL);
                ERR_X_Assert(pSrcElement->p_MrmElementAdditionalInfo == NULL);
                ERR_X_Assert(pSrcElement->pst_Gx8Add == NULL);
            }
            ERR_X_Assert(pObject->p_MRM_Levels == NULL);

            // Delete object for level i.
            GEO_Free(pObject);
        }
        MEM_Free(dlUVReorder);
        return TRUE;
    }
    else
        return GEO_STRIP_ComputeLevel(pst_GAO,__pst_Object , p_AdditionalVertexColor , _b_Msg , SetpercentOfJob);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_STRIP_Delete(GEO_tdst_Object *__pst_Object)
{
	/*~~~~~~~~~~~~~*/
	char	Str[100];
	LONG	i;
	ULONG	j;
	/*~~~~~~~~~~~~~*/

	if(__pst_Object->ulStripFlag & GEO_C_Strip_DataValid)
	{
		for(i = 0; i < __pst_Object->l_NbElements; i++)
		{
			if(__pst_Object->dst_Element[i].pst_StripData->pStripList)
			{
				for(j = 0; j < __pst_Object->dst_Element[i].pst_StripData->ulStripNumber; j++)
				{
					if(__pst_Object->dst_Element[i].pst_StripData->pStripList[j].pMinVertexDataList)
					{
						MEM_Free(__pst_Object->dst_Element[i].pst_StripData->pStripList[j].pMinVertexDataList);
						__pst_Object->dst_Element[i].pst_StripData->pStripList[j].pMinVertexDataList = NULL;
					}

					__pst_Object->dst_Element[i].pst_StripData->pStripList[j].ulVertexNumber = 0;
				}

				MEM_Free(__pst_Object->dst_Element[i].pst_StripData->pStripList);
				__pst_Object->dst_Element[i].pst_StripData->pStripList = NULL;
			}

			__pst_Object->dst_Element[i].pst_StripData->ulFlag = 0;
			__pst_Object->dst_Element[i].pst_StripData->ulStripNumber = 0;
			__pst_Object->dst_Element[i].pst_StripData->ulAveLength = 0;
			__pst_Object->dst_Element[i].pst_StripData->ulMaxLength = 0;
			__pst_Object->dst_Element[i].pst_StripData->ulMinLength = 0xfffffff;
		}

		GEO_STRIP_UnSetFlag(GEO_C_Strip_DataValid, __pst_Object);
	}
	else
	{
		sprintf(Str, "The object isn't stripped.");

		MessageBox(NULL, Str, TEXT("Don't be silly !"), MB_OK | MB_ICONWARNING | MB_TASKMODAL | MB_SETFOREGROUND);
		Mad_free();
	}
}

/*
 =======================================================================================================================
    set a flag in the object and in each element too.
 =======================================================================================================================
 */
void GEO_STRIP_SetFlag(LONG __command, GEO_tdst_Object *__pst_Object)
{
	/*~~~~~~*/
	LONG	i;
	/*~~~~~~*/

	__pst_Object->ulStripFlag |= __command;
	if (__command & GEO_C_Strip_DataValid)
	{
		__pst_Object->ulStripFlag &= ~GEO_STRIP_VERSION_MASK;
		__pst_Object->ulStripFlag |= GEO_STRIP_VERSION;
	}

	for(i = 0; i < __pst_Object->l_NbElements; i++)
	{
		if(__pst_Object->dst_Element[i].pst_StripData)
			__pst_Object->dst_Element[i].pst_StripData->ulFlag |= __command;
	}
}

/*
 =======================================================================================================================
    reset a flag in the object and in each element too.
 =======================================================================================================================
 */
void GEO_STRIP_UnSetFlag(LONG __command, GEO_tdst_Object *__pst_Object)
{
	/*~~~~~~*/
	LONG	i;
	/*~~~~~~*/

	if(!(__pst_Object->ulStripFlag & GEO_C_Strip_DataValid))
	{
		__pst_Object->ulStripFlag = 0;
		return;
	}

	__pst_Object->ulStripFlag &= ~__command;

	for(i = 0; i < __pst_Object->l_NbElements; i++)
	{
		if(__pst_Object->dst_Element[i].pst_StripData)
			__pst_Object->dst_Element[i].pst_StripData->ulFlag &= ~__command;
	}
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* ACTIVE_EDITORS */
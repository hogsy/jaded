/*$T GEOobjectcomputing.c GC!1.55 01/03/00 15:55:08 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#ifdef JADEFUSION
#include "BASe/ERRors/ERRdefs.h"
#endif
#include "MATHs/MATH.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_SKIN.h"
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEO_STRIP.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"

#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#endif

#if defined(PSX2_TARGET)  && defined(__cplusplus)
extern "C" {
#endif

#ifdef ACTIVE_EDITORS
#ifdef JADEFUSION
extern void GEO_FreeElementContent(GEO_tdst_ElementIndexedTriangles* _pst_Element);
#endif

void GEO_Merge2Elements(GEO_tdst_ElementIndexedTriangles	*pElementDest , GEO_tdst_ElementIndexedTriangles	*pElementSource )
{
	if (!pElementSource->l_NbTriangles) return;
	/* 1: Merge triangles */
	pElementDest->dst_Triangle = (GEO_tdst_IndexedTriangle *) MEM_p_Realloc(pElementDest->dst_Triangle,sizeof(GEO_tdst_IndexedTriangle) * (pElementDest->l_NbTriangles + pElementSource->l_NbTriangles));
	memcpy( pElementDest->dst_Triangle + pElementDest->l_NbTriangles , pElementSource->dst_Triangle , pElementSource->l_NbTriangles * sizeof(GEO_tdst_IndexedTriangle));
	pElementDest->l_NbTriangles += pElementSource->l_NbTriangles;
	/* 2: Merge strips */
	if ((pElementDest->pst_StripData) && (pElementDest->pst_StripData->ulFlag & GEO_C_Strip_DataValid))
	{
		pElementDest->pst_StripData->pStripList = (GEO_tdst_OneStrip *) 
			MEM_p_Realloc(
				pElementDest->pst_StripData->pStripList,
				sizeof(GEO_tdst_OneStrip) * (pElementDest->pst_StripData->ulStripNumber + pElementSource->pst_StripData->ulStripNumber));
		memcpy( 
			pElementDest->pst_StripData->pStripList + pElementDest->pst_StripData->ulStripNumber , 
			pElementSource->pst_StripData->pStripList , 
			pElementSource->pst_StripData->ulStripNumber * sizeof(GEO_tdst_OneStrip));
#ifdef JADEFUSION
        // SC: Make a copy of the min vertex data list
        for (ULONG i = 0; i < pElementSource->pst_StripData->ulStripNumber; ++i)
        {
            ULONG ulSize = sizeof(GEO_tdst_MinVertexData) * pElementSource->pst_StripData->pStripList[i].ulVertexNumber;
            pElementDest->pst_StripData->pStripList[i + pElementDest->pst_StripData->ulStripNumber].pMinVertexDataList = (GEO_tdst_MinVertexData*)MEM_p_Alloc(ulSize);
            L_memcpy(pElementDest->pst_StripData->pStripList[i + pElementDest->pst_StripData->ulStripNumber].pMinVertexDataList, 
                     pElementSource->pst_StripData->pStripList[i].pMinVertexDataList, ulSize);
        }
#endif
		pElementDest->pst_StripData->ulStripNumber += pElementSource->pst_StripData->ulStripNumber ;
	}
}


void GEO_OptimizeNumberOfElement(GEO_tdst_Object *__pst_Object)
{
	GEO_tdst_ElementIndexedTriangles	*pElement1 , *pElement2 , *pLastElement;
	ULONG ulBefore,ulAfter;

	ulBefore = __pst_Object->l_NbElements;

	pLastElement = __pst_Object->dst_Element + __pst_Object->l_NbElements;

	for (pElement1 = __pst_Object->dst_Element;pElement1 < pLastElement - 1 ; pElement1++)
	{
		for (pElement2 = pElement1 + 1;pElement2 < pLastElement; pElement2++)
		{
			if (pElement1->l_MaterialId == pElement2->l_MaterialId)
			{
				GEO_Merge2Elements(pElement1 , pElement2 );

#ifdef JADEFUSION
                // SC: Free the second element since we are not using it anymore
                GEO_FreeElementContent(pElement2);
#endif
				pElement2->l_NbTriangles = 0;
			}
		}
	}
	__pst_Object->l_NbElements = 0;
	for (pElement1 = pElement2 = __pst_Object->dst_Element;pElement1 < pLastElement; pElement1++)
	{
		if (pElement1 ->l_NbTriangles != 0)
		{
			__pst_Object->l_NbElements++;
			*(pElement2++) = *(pElement1);
		}
	}
	ulAfter = __pst_Object->l_NbElements;
}

/* This function will limit the uv to the segment -8 minimum to 8 maximum */
#define UV_LIMIT 8.0f
void GEO_Limit_UVS(GEO_tdst_Object *__pst_Object )
{
	float UMAX, UMIN, VMAX, VMIN ;
	long LA, LB;
	long LAv, LBv;
	GEO_tdst_UV  *p_PointUV;
	GEO_tdst_ElementIndexedTriangles	*pElement , *pLastElement;
	ULONG Counter;
	if (!(__pst_Object->l_NbUVs)) return;
	UMAX = UMIN = __pst_Object->dst_UV[0].fU;
	VMAX = VMIN = __pst_Object->dst_UV[0].fV;
	Counter = __pst_Object->l_NbUVs;
	p_PointUV = __pst_Object->dst_UV;
	while (Counter--)
	{
		UMAX = fMax(p_PointUV->fU , UMAX);
		UMIN = fMin(p_PointUV->fU , UMIN);
		VMAX = fMax(p_PointUV->fV , VMAX);
		VMIN = fMin(p_PointUV->fV , VMIN);
		p_PointUV++;
	}
	if ((VMAX <= UV_LIMIT) && (VMIN >= -UV_LIMIT) && (UMAX <= UV_LIMIT) && (UMIN >= -UV_LIMIT)) return;

	/* Else do it for each strip */
	pElement = __pst_Object->dst_Element;
	pLastElement = pElement + __pst_Object->l_NbElements;
	while (pElement < pLastElement)
	{
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
				UMAX = UMIN = __pst_Object->dst_UV[pNodeList  ->auw_UV].fU;
				VMAX = VMIN = __pst_Object->dst_UV[pNodeList  ->auw_UV].fV;
				while (pNodeList  < pNodeListLast )
				{
					UMAX = fMax(__pst_Object->dst_UV[pNodeList  ->auw_UV].fU , UMAX);
					UMIN = fMin(__pst_Object->dst_UV[pNodeList  ->auw_UV].fU , UMIN);
					VMAX = fMax(__pst_Object->dst_UV[pNodeList  ->auw_UV].fV , VMAX);
					VMIN = fMin(__pst_Object->dst_UV[pNodeList  ->auw_UV].fV , VMIN);
					pNodeList  ++;
				}
				if ((VMAX <= UV_LIMIT) && (VMIN >= -UV_LIMIT) && (UMAX <= UV_LIMIT) && (UMIN >= -UV_LIMIT)) 
				{
				} else
				{
/*					unsigned short IndexRetreiver[3];
					unsigned short UVIndexRetreiver[3];
					unsigned short NodeCounter;//*/
					/* Create new UV pStripList->ulVertexNumber Entry's */
					__pst_Object->dst_UV = (GEO_tdst_UV  *) MEM_p_Realloc(__pst_Object->dst_UV,sizeof(GEO_tdst_UV) * (__pst_Object->l_NbUVs + pStripList->ulVertexNumber));
					/* Recompute Indexes */
					LA = (long)((UMIN + 1024.0f) * 1024.0f);
					LA >>= 10;
					LA -= 1024;
					LB = (long)((UMAX + 1024.0f) * 1024.0f);
					LB >>= 10;
					LB -= 1024;
					LB++;
					LA = LA + ((LB - LA) >> 1);
					LAv = (long)((VMIN + 1024.0f) * 1024.0f);
					LAv >>= 10;
					LAv -= 1024;
					LBv = (long)((VMAX + 1024.0f) * 1024.0f);
					LBv >>= 10;
					LBv -= 1024;
					LBv++;
					LAv = LAv + ((LBv - LAv) >> 1);
					pNodeList = pStripList->pMinVertexDataList;
					pNodeListLast = pNodeList + pStripList->ulVertexNumber;
					p_PointUV = __pst_Object->dst_UV + __pst_Object->l_NbUVs;
					while (pNodeList  < pNodeListLast )
					{
						p_PointUV->fU =  __pst_Object->dst_UV[pNodeList  ->auw_UV].fU - LA;
						p_PointUV->fV =  __pst_Object->dst_UV[pNodeList  ->auw_UV].fV - LAv;
						pNodeList->auw_UV = (unsigned short)__pst_Object->l_NbUVs++;
						p_PointUV++;
						pNodeList  ++;
					}
				}
				pStripList ++;
			}
			/* Then re-deduct triangles from strips */
			{
				unsigned short IndexRetreiver[3];
				unsigned short UVIndexRetreiver[3];
				GEO_tdst_IndexedTriangle *p_Face;
				ULONG NodeCounter;
				p_Face = pElement->dst_Triangle;
				pStripList = pElement->pst_StripData->pStripList;
				pStripListLast = pStripList + pElement->pst_StripData->ulStripNumber;
				NodeCounter = 0;
				while (pStripList < pStripListLast)
				{
					NodeCounter = 0;

					pNodeList = pStripList->pMinVertexDataList;
					pNodeListLast = pNodeList + pStripList->ulVertexNumber;
					while (pNodeList  < pNodeListLast )
					{
						IndexRetreiver[NodeCounter % 3] = pNodeList->auw_Index;
						UVIndexRetreiver[NodeCounter % 3] = pNodeList->auw_UV;
						if (NodeCounter >= 2)
						{
							if (NodeCounter & 1)
							{
								p_Face->auw_Index[1] = IndexRetreiver[0];
								p_Face->auw_Index[0] = IndexRetreiver[1];
								p_Face->auw_Index[2] = IndexRetreiver[2];
								p_Face->auw_UV[1] = UVIndexRetreiver[0];
								p_Face->auw_UV[0] = UVIndexRetreiver[1];
								p_Face->auw_UV[2] = UVIndexRetreiver[2];
							} else
							{
								p_Face->auw_Index[0] = IndexRetreiver[0];
								p_Face->auw_Index[1] = IndexRetreiver[1];
								p_Face->auw_Index[2] = IndexRetreiver[2];
								p_Face->auw_UV[0] = UVIndexRetreiver[0];
								p_Face->auw_UV[1] = UVIndexRetreiver[1];
								p_Face->auw_UV[2] = UVIndexRetreiver[2];
							} 
							p_Face++;
						}
						NodeCounter++;
						pNodeList  ++;
					}
					pStripList ++;
				}
			}
		}
		pElement ++;
	}
}
ULONG GEO_IsABadFace(GEO_tdst_Object *__pst_Object , GEO_tdst_IndexedTriangle	*p_Face)
{
	/* Index Test */
	if (p_Face->auw_Index[0] == p_Face->auw_Index[1]) return TRUE;
	if (p_Face->auw_Index[1] == p_Face->auw_Index[2]) return TRUE;
	if (p_Face->auw_Index[2] == p_Face->auw_Index[0]) return TRUE;
	/* Index Test */
	if (p_Face->auw_Index[0] >= __pst_Object->l_NbPoints) return TRUE;
	if (p_Face->auw_Index[1] >= __pst_Object->l_NbPoints) return TRUE;
	if (p_Face->auw_Index[2] >= __pst_Object->l_NbPoints) return TRUE;
	return FALSE;
}

void GEO_Clean_Faces(GEO_tdst_Object *__pst_Object )
{
	GEO_tdst_ElementIndexedTriangles	*pElement , *pElementCompressed , *pLastElement;
	GEO_tdst_IndexedTriangle			*p_Face,*p_FaceCompresed,*p_FaceLast;
	if (GEO_MRM_ul_IsMrmObject(__pst_Object)) return;
	/* ------------------------- */
	/* A: Remove bad faces		 */
	/* ------------------------- */
	pElement = __pst_Object->dst_Element;
	pLastElement = pElement + __pst_Object->l_NbElements;
	while (pElement < pLastElement)
	{
		p_FaceCompresed = p_Face = pElement->dst_Triangle;
		p_FaceLast = p_Face + pElement->l_NbTriangles;
		pElement->l_NbTriangles = 0;
		while (p_Face < p_FaceLast )
		{
			if (!GEO_IsABadFace(__pst_Object , p_Face))
			{
				*(p_FaceCompresed++) = *p_Face;
				pElement->l_NbTriangles ++;
			} 
			if (p_Face->auw_UV[0] >= __pst_Object->l_NbUVs) p_Face->auw_UV[0] = 0;
			if (p_Face->auw_UV[1] >= __pst_Object->l_NbUVs) p_Face->auw_UV[1] = 0;
			if (p_Face->auw_UV[2] >= __pst_Object->l_NbUVs) p_Face->auw_UV[2] = 0;
			p_Face++;
		}
#ifdef JADEFUSION
        // SC: Free the content of the element since we won't need it anymore
        if (pElement->l_NbTriangles == 0)
        {
            GEO_FreeElementContent(pElement);
        }
#endif	
		
		pElement ++;
	}
	/* ------------------------- */
	/* B: Remove empty elements	 */
	/* ------------------------- */
	pElementCompressed = pElement = __pst_Object->dst_Element;
	__pst_Object->l_NbElements = 0;
	while (pElement < pLastElement)
	{
		if (pElement->l_NbTriangles)
		{
			*(pElementCompressed++) = *(pElement);
			__pst_Object->l_NbElements++;
		}
		pElement ++;
	}
}

void GEO_Clean_XYZ_UV(OBJ_tdst_GameObject *pst_GO , GEO_tdst_Object *__pst_Object ,ULONG *p_OtherColor , BOOL CleanXYZ , BOOL CleanUV)
{
	GEO_tdst_ElementIndexedTriangles	*pElement , *pLastElement;
	GEO_Vertex                          *p_Point3D , *p_PointNormal ;
	GEO_tdst_UV                         *p_PointUV;
	ULONG *p_TagODrome , *p_TagODromeUV , *p_ColorBase , *p_ColorBase2;
	LONG Counter , CounterCompressed , OriginalNumberofPoints;
	GEO_tdst_IndexedTriangle *p_Face,*p_FaceLast;
	if (GEO_MRM_ul_IsMrmObject(__pst_Object)) return;
	if (!__pst_Object->l_NbPoints) return;
	/* ------------------------- */
	/* A: INIT					 */
	/* ------------------------- */
	/* Solve BugOf morphing */
	if (CleanXYZ)
	{
		if ((pst_GO) && (pst_GO->pst_Extended))
		{
			MDF_tdst_Modifier	*pst_Modifier;
			pst_Modifier = pst_GO->pst_Extended->pst_Modifiers;
			while (pst_Modifier && CleanXYZ)
			{
				if (pst_Modifier->i->ul_Type == MDF_C_Modifier_Morphing) CleanXYZ = 0;
				pst_Modifier = pst_Modifier->pst_Next;
			}
		}
	}
	if (CleanXYZ)
	{
		p_ColorBase = __pst_Object->dul_PointColors;
		if (p_ColorBase) p_ColorBase++;
		p_ColorBase2 = p_OtherColor;
		if (p_ColorBase2) p_ColorBase2++;
		p_TagODrome = (ULONG*)MEM_p_Alloc(sizeof(ULONG) * __pst_Object->l_NbPoints);
		memset(p_TagODrome , 0 , sizeof(ULONG) * __pst_Object->l_NbPoints );
		OriginalNumberofPoints = __pst_Object->l_NbPoints ;
	}
	if (CleanUV)
	{
		if (!(__pst_Object->l_NbUVs)) 
			CleanUV = FALSE;
		else
		{
			p_TagODromeUV = (ULONG*)MEM_p_Alloc(sizeof(ULONG) * __pst_Object->l_NbUVs);
			memset(p_TagODromeUV , 0 , sizeof(ULONG) * __pst_Object->l_NbUVs );
		}
	}
	/* ------------------------- */
	/* B: Tagging				 */
	/* ------------------------- */
	pElement = __pst_Object->dst_Element;
	pLastElement = pElement + __pst_Object->l_NbElements;
	while (pElement < pLastElement)
	{
		if (CleanXYZ)
		{
			p_Face = pElement->dst_Triangle;
			p_FaceLast = p_Face + pElement->l_NbTriangles;
			while (p_Face < p_FaceLast )
			{
				p_TagODrome[p_Face ->auw_Index[0]] = 0xffffffff;
				p_TagODrome[p_Face ->auw_Index[1]] = 0xffffffff;
				p_TagODrome[p_Face ->auw_Index[2]] = 0xffffffff;
				p_Face++;
			}
		}
		
		if (CleanUV)
		{
			p_Face = pElement->dst_Triangle;
			p_FaceLast = p_Face + pElement->l_NbTriangles;
			while (p_Face < p_FaceLast )
			{
				p_TagODromeUV[p_Face ->auw_UV[0]] = 0xffffffff;
				p_TagODromeUV[p_Face ->auw_UV[1]] = 0xffffffff;
				p_TagODromeUV[p_Face ->auw_UV[2]] = 0xffffffff;
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
						p_TagODromeUV[pNodeList  ->auw_UV] = 0xffffffff;
						pNodeList  ++;
					}
					pStripList ++;
				}
			}
		}
		pElement ++;
	}
	/* ------------------------- */
	/* C: Compress points 		 */
	/* ------------------------- */
	if (CleanXYZ)
	{
		p_Point3D = __pst_Object->dst_Point;
		p_PointNormal = __pst_Object->dst_PointNormal;
		if (GEO_SKN_IsSkinned(__pst_Object)) GEO_SKN_Expand(__pst_Object);
		for (Counter = 0 , CounterCompressed = 0; Counter < __pst_Object->l_NbPoints ; Counter ++)
		{
			p_Point3D		[CounterCompressed ]  = p_Point3D [Counter];
			p_PointNormal	[CounterCompressed ]  = p_PointNormal [Counter];
			if (GEO_SKN_IsSkinned(__pst_Object))
			{
				ULONG ulCounter;
				ulCounter = __pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
				while (ulCounter--) 
					__pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ulCounter]->p_PdrtVrc_E[CounterCompressed].f_Ponderation = 
						__pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ulCounter]->p_PdrtVrc_E[Counter].f_Ponderation;
			}
			if (p_ColorBase) p_ColorBase[CounterCompressed ]  = p_ColorBase[Counter];
			if (p_ColorBase2)p_ColorBase2[CounterCompressed ]  = p_ColorBase2[Counter];

			if (p_TagODrome[Counter] == 0xffffffff) CounterCompressed++;
			p_TagODrome		[Counter]			  = CounterCompressed - 1;
		}
		__pst_Object->l_NbPoints = CounterCompressed;
		if (p_ColorBase) p_ColorBase[-1] = CounterCompressed; // Super connerie
		if (p_ColorBase2) p_ColorBase2[-1] = CounterCompressed; // Super connerie
		if (GEO_SKN_IsSkinned(__pst_Object)) 
		{
			ULONG ulCounter;
			if (__pst_Object->l_NbPoints == 0)
			{
				GEO_SKN_DestroyObjPonderation(__pst_Object->p_SKN_Objectponderation);
				__pst_Object->p_SKN_Objectponderation = NULL;
			} else
			{
				ulCounter = __pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
				while (ulCounter--) __pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ulCounter]->us_NumberOfPonderatedVertices = (unsigned short)CounterCompressed;
				GEO_SKN_Compress(__pst_Object);
			}
		}
	}
	/* ------------------------- */
	/* D: Compress UV'S 		 */
	/* ------------------------- */
	if (CleanUV)
	{
		p_PointUV = __pst_Object->dst_UV;
		for (Counter = 0 , CounterCompressed = 0; Counter < __pst_Object->l_NbUVs ; Counter ++)
		{
			p_PointUV		[CounterCompressed ]  = p_PointUV [Counter];
			if (p_TagODromeUV[Counter] == 0xffffffff) CounterCompressed++;
			p_TagODromeUV	[Counter]			  = CounterCompressed-1;
		}
		__pst_Object->l_NbUVs = CounterCompressed;
	}
	/* ------------------------- */
	/* E: Reordering	 		 */
	/* ------------------------- */
	pElement = __pst_Object->dst_Element;
	pLastElement = pElement + __pst_Object->l_NbElements;
	while (pElement < pLastElement)
	{
		if (CleanXYZ)
		{
			p_Face = pElement->dst_Triangle;
			p_FaceLast = p_Face + pElement->l_NbTriangles;
			while (p_Face < p_FaceLast )
			{
				p_Face ->auw_Index[0] = (unsigned short)p_TagODrome[p_Face ->auw_Index[0]];
				p_Face ->auw_Index[1] = (unsigned short)p_TagODrome[p_Face ->auw_Index[1]];
				p_Face ->auw_Index[2] = (unsigned short)p_TagODrome[p_Face ->auw_Index[2]];
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
						pNodeList  ->auw_Index = (unsigned short)p_TagODrome[pNodeList  ->auw_Index ];
						pNodeList  ++;
					}
					pStripList ++;
				}
			}
		}
		if (CleanUV)
		{
			p_Face = pElement->dst_Triangle;
			p_FaceLast = p_Face + pElement->l_NbTriangles;
			while (p_Face < p_FaceLast )
			{
				p_Face ->auw_UV[0] = (unsigned short)p_TagODromeUV[p_Face ->auw_UV[0]];
				p_Face ->auw_UV[1] = (unsigned short)p_TagODromeUV[p_Face ->auw_UV[1]];
				p_Face ->auw_UV[2] = (unsigned short)p_TagODromeUV[p_Face ->auw_UV[2]];
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
						pNodeList  ->auw_UV = (unsigned short)p_TagODromeUV[pNodeList  ->auw_UV];
						pNodeList  ++;
					}
					pStripList ++;
				}
			}
		}
		pElement ++;
	}

	
	if (CleanXYZ) MEM_Free(p_TagODrome);
	if (CleanUV)  MEM_Free(p_TagODromeUV);
}
void GEO_Weld_XYZ_UV(GEO_tdst_Object *__pst_Object , BOOL WeldXYZ , BOOL WeldUV )
{
	GEO_tdst_ElementIndexedTriangles	*pElement , *pLastElement;
	ULONG								*p_TagODrome , *p_TagODromeUV ;
	GEO_tdst_IndexedTriangle			*p_Face,*p_FaceLast;
	LONG								Counter , Counter2;
	if (GEO_MRM_ul_IsMrmObject(__pst_Object)) return;
	if (!__pst_Object->l_NbPoints) return;
	/* ------------------------- */
	/* A: INIT					 */
	/* ------------------------- */
	if (WeldXYZ)
	{
		p_TagODrome = (ULONG*)MEM_p_Alloc(sizeof(ULONG) * __pst_Object->l_NbPoints);
		Counter = __pst_Object->l_NbPoints;
		while (Counter--) p_TagODrome[Counter] = Counter;
	}
	if (WeldUV)
	{
		if (!(__pst_Object->l_NbUVs)) 
			WeldUV = FALSE;
		else
		{
			p_TagODromeUV = (ULONG*)MEM_p_Alloc(sizeof(ULONG) * __pst_Object->l_NbUVs);
			Counter = __pst_Object->l_NbUVs;
			while (Counter--) p_TagODromeUV [Counter] = Counter;
		}
	}
	
	/* ------------------------- */
	/* B: XYZ(Square comparison) */
	/* ------------------------- */
	if (WeldXYZ)
	{
		GEO_Vertex                          *p_Point3D , *p_PointNormal ;
		p_Point3D = __pst_Object->dst_Point;
		p_PointNormal = __pst_Object->dst_PointNormal;
		for (Counter = 0 ; Counter < __pst_Object->l_NbPoints-1; Counter++)
		{
			for (Counter2 = Counter + 1; Counter2 < __pst_Object->l_NbPoints ; Counter2++)
			{
				if (MATH_f_Distance(p_Point3D + p_TagODrome[Counter], p_Point3D + p_TagODrome[Counter2]) == 0.0f)	
				{
					if (MATH_f_Distance(p_PointNormal + p_TagODrome[Counter], p_PointNormal + p_TagODrome[Counter2]) == 0.0f)	
					{
						p_TagODrome[Counter2] = p_TagODrome[Counter];
					}
				}
			}
		}
	}
	/* ------------------------- */
	/* D: UV					 */
	/* ------------------------- */
	if (WeldUV)
	{
		GEO_tdst_UV                *p_PointUV1 , *p_PointUV2;
		for (Counter = 0 ; Counter < __pst_Object->l_NbUVs-1; Counter++)
		{
			p_PointUV1 = __pst_Object->dst_UV + p_TagODromeUV[Counter];
			for (Counter2 = Counter + 1; Counter2 < __pst_Object->l_NbUVs ; Counter2++)
			{
				p_PointUV2 = __pst_Object->dst_UV + p_TagODromeUV[Counter2];
				if (fSqr(p_PointUV1->fU - p_PointUV2->fU) + fSqr(p_PointUV1->fV - p_PointUV2->fV) < Cf_Epsilon)	
				{
					p_TagODromeUV[Counter2] = p_TagODromeUV[Counter];
				}
			}
		}
	}
	/* ------------------------- */
	/* D: TRIANGLES				 */
	/* ------------------------- */
	pElement = __pst_Object->dst_Element;
	pLastElement = pElement + __pst_Object->l_NbElements;
	while (pElement < pLastElement)
	{
		if (WeldXYZ)
		{
			p_Face = pElement->dst_Triangle;
			p_FaceLast = p_Face + pElement->l_NbTriangles;
			while (p_Face < p_FaceLast )
			{
				p_Face ->auw_Index[0] = (unsigned short)p_TagODrome[p_Face ->auw_Index[0]];
				p_Face ->auw_Index[1] = (unsigned short)p_TagODrome[p_Face ->auw_Index[1]];
				p_Face ->auw_Index[2] = (unsigned short)p_TagODrome[p_Face ->auw_Index[2]];
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
						pNodeList  ->auw_Index = (unsigned short)p_TagODrome[pNodeList  ->auw_Index ];
						pNodeList  ++;
					}
					pStripList ++;
				}
			}
		}
		if (WeldUV)
		{
			p_Face = pElement->dst_Triangle;
			p_FaceLast = p_Face + pElement->l_NbTriangles;
			while (p_Face < p_FaceLast )
			{
				p_Face ->auw_UV[0] = (unsigned short)p_TagODromeUV[p_Face ->auw_UV[0]];
				p_Face ->auw_UV[1] = (unsigned short)p_TagODromeUV[p_Face ->auw_UV[1]];
				p_Face ->auw_UV[2] = (unsigned short)p_TagODromeUV[p_Face ->auw_UV[2]];
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
						pNodeList  ->auw_UV = (unsigned short)p_TagODromeUV[pNodeList  ->auw_UV];
						pNodeList  ++;
					}
					pStripList ++;
				}
			}
		}
		pElement ++;
	}
	/* ------------------------- */
	/* E: END					 */
	/* ------------------------- */
	if (WeldXYZ)	MEM_Free(p_TagODrome);
	if (WeldUV)		MEM_Free(p_TagODromeUV);
}

void GEO_Clean(OBJ_tdst_GameObject *pst_GO , GEO_tdst_Object *__pst_Object , ULONG *p_AdditionalVertexColor)
{
    int i;

	// special for 3D-Text : separate UV before cleaning
    if (__pst_Object->ul_EditorFlags & GEO_CEF_GeomFor3DText)
    {
        //for (i = 0; i < __pst_Object->l_NbUVs; i++)
		for (i = 0; i < (__pst_Object->dst_Element->l_NbTriangles * 2); i++)
            __pst_Object->dst_UV[i].fU = __pst_Object->dst_UV[i].fV = ((float) i) / 1000.0f;
    }

	GEO_Clean_Faces(__pst_Object);
	GEO_Limit_UVS(__pst_Object );
	GEO_OptimizeNumberOfElement(__pst_Object);	
	GEO_Weld_XYZ_UV(__pst_Object , FALSE , TRUE);
	GEO_Clean_Faces(__pst_Object );//*/
	GEO_Clean_XYZ_UV(pst_GO , __pst_Object , p_AdditionalVertexColor , TRUE , TRUE);
}
void GEO_Clean_After_strips(OBJ_tdst_GameObject *pst_GO , GEO_tdst_Object *__pst_Object , ULONG *p_AdditionalVertexColor)
{
    int i;

	// special for 3D-Text : separate UV before cleaning
    if (__pst_Object->ul_EditorFlags & GEO_CEF_GeomFor3DText)
    {
        //for (i = 0; i < __pst_Object->l_NbUVs; i++)
		for (i = 0; i < (__pst_Object->dst_Element->l_NbTriangles * 2); i++)
            __pst_Object->dst_UV[i].fU = __pst_Object->dst_UV[i].fV = ((float) i) / 1000.0f;
    }

	GEO_Clean_Faces(__pst_Object);
	GEO_Limit_UVS(__pst_Object );
	GEO_OptimizeNumberOfElement(__pst_Object);	
	GEO_Weld_XYZ_UV(__pst_Object , FALSE , TRUE);
	GEO_Clean_Faces(__pst_Object );//*/
	GEO_Clean_XYZ_UV(pst_GO , __pst_Object , p_AdditionalVertexColor , FALSE , TRUE);
}
#endif /*ACTIVE_EDITOR*/

void MATH_VectorMinMax2(GEO_Vertex *SRC,GEO_Vertex *MAX,GEO_Vertex *MIN)
{
	if (MAX->x < SRC->x) MAX->x = SRC->x;
	if (MAX->y < SRC->y) MAX->y = SRC->y;
	if (MAX->z < SRC->z) MAX->z = SRC->z;
	if (MIN->x > SRC->x) MIN->x = SRC->x;
	if (MIN->y > SRC->y) MIN->y = SRC->y;
	if (MIN->z > SRC->z) MIN->z = SRC->z;
}

u32 IsTouchBorder(GEO_Vertex *SRC,GEO_tdst_IndexedTriangle  *pst_Triangle,float *TST,u32 VTT,u32 *A, u32 *B)
{
	u32 RetV;
	float TSDT[3];

	RetV = 0;
	TSDT[0] = ((float *)&SRC[pst_Triangle->auw_Index[0]])[VTT] - TST[VTT];
	TSDT[1] = ((float *)&SRC[pst_Triangle->auw_Index[1]])[VTT] - TST[VTT];
	TSDT[2] = ((float *)&SRC[pst_Triangle->auw_Index[2]])[VTT] - TST[VTT];

#define OBB_EPSILON (0.30f * 0.30f)
	TSDT[0] *= TSDT[0];
	TSDT[1] *= TSDT[1];
	TSDT[2] *= TSDT[2];
	if ((TSDT[0] < OBB_EPSILON) && (TSDT[1] < OBB_EPSILON))
	{
		*A = pst_Triangle->auw_Index[0] ;		*B = pst_Triangle->auw_Index[1] ;		return 1;
	}
	if ((TSDT[1] < OBB_EPSILON) && (TSDT[2] < OBB_EPSILON))
	{
		*A = pst_Triangle->auw_Index[1] ;		*B = pst_Triangle->auw_Index[2] ;		return 1;
	}
	if ((TSDT[2] < OBB_EPSILON) && (TSDT[0] < OBB_EPSILON))
	{
		*A = pst_Triangle->auw_Index[2] ;		*B = pst_Triangle->auw_Index[0] ;		return 1;
	}
	return 0;
}

/*
 =======================================================================================================================
 this function is used to compute normals continuity according to "puzzle design"
 This should be called once per objet per loading -> no need to optimize
 =======================================================================================================================
 */
void GEO_ComputeNormals_OBBBOX_CORRECTION(GEO_tdst_Object *_pst_Object)
{
#ifdef ACTIVE_EDITORS    
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_Vertex                          *pst_Point,*pst_LastPoint;
    GEO_Vertex                          *pst_Pt[3];
	GEO_Vertex                          MAX,MIN;
    MATH_tdst_Vector                    *pst_Normal;
    MATH_tdst_Vector                    *pst_Normal2, *pst_LastNormal2;
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
	u32 Modulo;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_InitVector(&MAX,-1000000,-1000000,-1000000);
	MATH_InitVector(&MIN, 1000000, 1000000, 1000000);
    pst_Point = _pst_Object->dst_Point;
    pst_LastPoint = pst_Point + _pst_Object->l_NbPoints;
    for(; pst_Point < pst_LastPoint; pst_Point++)
	{
        MATH_VectorMinMax2(pst_Point++,&MAX,&MIN);
	}

	MATH_ScaleVector(&MAX,&MAX,2.0f);
	MATH_ScaleVector(&MIN,&MIN,2.0f);

	if (MAX.x - MIN.x < 9.95f) return;
	if (MAX.y - MIN.y < 9.95f) return;

	/* Puzzle design is aligned on multiple of 10 */
#define OBB_EPSILON_BV 30 // this is centimeters
	Modulo = (u32)((MAX.x - MIN.x) * 100) % 1000;
	if ((Modulo < 1000 - OBB_EPSILON_BV) && (Modulo > OBB_EPSILON_BV))	return; 
	Modulo = (u32)((MAX.y - MIN.y) * 100) % 1000;
	if ((Modulo < 1000 - OBB_EPSILON_BV) && (Modulo > OBB_EPSILON_BV))	return; 

	MATH_ScaleVector(&MAX,&MAX,0.5f);
	MATH_ScaleVector(&MIN,&MIN,0.5f);

	memset((void *)GDI_gpst_CurDD ->pst_ComputingBuffers->ast_SpecialVB2,0,sizeof(MATH_tdst_Vector) * _pst_Object->l_NbPoints);

    pst_Point = _pst_Object->dst_Point;
    pst_Normal = (MATH_tdst_Vector*)GDI_gpst_CurDD ->pst_ComputingBuffers->ast_SpecialVB2;

    pst_Element = _pst_Object->dst_Element;
    pst_LastElement = pst_Element + _pst_Object->l_NbElements;
    for(; pst_Element < pst_LastElement; pst_Element++)
    {
        pst_Triangle = pst_Element->dst_Triangle;
        pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

        for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
        {
			u32 A,B,K;
            pst_Pt[0] = pst_Point + pst_Triangle->auw_Index[0];
            pst_Pt[1] = pst_Point + pst_Triangle->auw_Index[1];
            pst_Pt[2] = pst_Point + pst_Triangle->auw_Index[2];
			/* 2 Point Must be on the same OBBBorder */
			K = 0;
			/* TEST X+ */
			if (IsTouchBorder(pst_Point,pst_Triangle,(float *)&MAX,0,&A, &B))
			{
				pst_Normal[B].y -= pst_Point[B].z - pst_Point[A].z;
				pst_Normal[A].y -= pst_Point[B].z - pst_Point[A].z;
				pst_Normal[A].z += MATH_f_Distance(&pst_Point[B] , &pst_Point[A]);
				pst_Normal[B].z += MATH_f_Distance(&pst_Point[B] , &pst_Point[A]);
			}
			/* TEST X- */
			if (IsTouchBorder(pst_Point,pst_Triangle,(float *)&MIN,0,&A, &B))
			{
				pst_Normal[A].y += pst_Point[B].z - pst_Point[A].z;
				pst_Normal[B].y += pst_Point[B].z - pst_Point[A].z;
				pst_Normal[A].z += MATH_f_Distance(&pst_Point[B] , &pst_Point[A]);
				pst_Normal[B].z += MATH_f_Distance(&pst_Point[B] , &pst_Point[A]);
			}
			/* TEST Y+ */
			if (IsTouchBorder(pst_Point,pst_Triangle,(float *)&MAX,1,&A, &B))
			{
				pst_Normal[B].x += pst_Point[B].z - pst_Point[A].z;
				pst_Normal[A].x += pst_Point[B].z - pst_Point[A].z;
				pst_Normal[A].z += MATH_f_Distance(&pst_Point[B] , &pst_Point[A]);
				pst_Normal[B].z += MATH_f_Distance(&pst_Point[B] , &pst_Point[A]);
			}
			/* TEST Y- */
			if (IsTouchBorder(pst_Point,pst_Triangle,(float *)&MIN,1,&A, &B))
			{
				pst_Normal[A].x -= pst_Point[B].z - pst_Point[A].z;
				pst_Normal[B].x -= pst_Point[B].z - pst_Point[A].z;
				pst_Normal[A].z += MATH_f_Distance(&pst_Point[B] , &pst_Point[A]);
				pst_Normal[B].z += MATH_f_Distance(&pst_Point[B] , &pst_Point[A]);
			}
        }
    }

	pst_Normal2 = _pst_Object->dst_PointNormal;
    pst_LastNormal2 = pst_Normal2 + _pst_Object->l_NbPoints ;
	pst_Normal = (MATH_tdst_Vector*)GDI_gpst_CurDD ->pst_ComputingBuffers->ast_SpecialVB2;

    for(; pst_Normal2 < pst_LastNormal2; pst_Normal++,pst_Normal2 ++)
	{
		if (pst_Normal->z)
		{
	        MATH_NormalizeAnyVector(pst_Normal, pst_Normal);
			pst_Normal2 -> x = pst_Normal -> x;
			pst_Normal2 -> y = pst_Normal -> y;
			pst_Normal2 -> z = pst_Normal -> z;
		}
	}
#endif	
}

void GEO_ComputeNormals(GEO_tdst_Object *_pst_Object)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_Vertex                          *pst_Point;
    GEO_Vertex                          *pst_Pt[3];
    MATH_tdst_Vector                    *pst_Normal, *pst_LastNormal;
    MATH_tdst_Vector                    st_TriangleNormal, st_Vect1, st_Vect2;
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef ACTIVE_EDITORS    
	GEO_SKN_ComputeNormals(_pst_Object , _pst_Object->dst_Point); // Optimized Version 
#ifdef PSX2_TARGET
	GEO_SKN_ComputeNormals(_pst_Object , _pst_Object->dst_Point); // This is an antibug specific PS2 !!!!
#endif	
	return;
#endif	
    pst_Normal = _pst_Object->dst_PointNormal;
    pst_LastNormal = pst_Normal + _pst_Object->l_NbPoints;
    for(; pst_Normal < pst_LastNormal; pst_Normal++)
        MATH_InitVectorToZero(pst_Normal);

    pst_Point = _pst_Object->dst_Point;
    pst_Normal = _pst_Object->dst_PointNormal;

    pst_Element = _pst_Object->dst_Element;
    pst_LastElement = pst_Element + _pst_Object->l_NbElements;
    for(; pst_Element < pst_LastElement; pst_Element++)
    {
        pst_Triangle = pst_Element->dst_Triangle;
        pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

        for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
        {
            pst_Pt[0] = pst_Point + pst_Triangle->auw_Index[0];
            pst_Pt[1] = pst_Point + pst_Triangle->auw_Index[1];
            pst_Pt[2] = pst_Point + pst_Triangle->auw_Index[2];

            MATH_SubVector(&st_Vect1, VCast( pst_Pt[0] ), VCast( pst_Pt[1] ) );
            MATH_SubVector(&st_Vect2, VCast( pst_Pt[0] ), VCast( pst_Pt[2] ) );

            MATH_CrossProduct(&st_TriangleNormal, &st_Vect1, &st_Vect2);
            MATH_NormalizeAnyVector(&st_TriangleNormal, &st_TriangleNormal);

            MATH_AddEqualVector(pst_Normal + pst_Triangle->auw_Index[0], &st_TriangleNormal);
            MATH_AddEqualVector(pst_Normal + pst_Triangle->auw_Index[1], &st_TriangleNormal);
            MATH_AddEqualVector(pst_Normal + pst_Triangle->auw_Index[2], &st_TriangleNormal);
        }
    }



    pst_Point = _pst_Object->dst_Point;
    pst_Normal = _pst_Object->dst_PointNormal;
    for(; pst_Normal < pst_LastNormal; pst_Normal++,pst_Point ++)
	{
		if (pst_Point->x * pst_Point->x < 0.001f)
			pst_Normal->x = 0.0f; /* Half Skin continuity */

        MATH_NormalizeAnyVector(pst_Normal, pst_Normal);
	}


/*    pst_Point = _pst_Object->dst_Point;
    pst_Normal = _pst_Object->dst_PointNormal;
    for(; pst_Normal < pst_LastNormal; pst_Normal++)
        MATH_NormalizeAnyVector(pst_Normal, pst_Normal);*/

	GEO_ComputeNormals_OBBBOX_CORRECTION(_pst_Object);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ComputeBiNormals(GEO_tdst_Object *_pst_Object , ULONG *pBiNormales)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector                    *pst_Normal, *pst_LastNormal  ;
    ULONG *pBinormalPtr;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_UseNormals(_pst_Object );
#ifdef PSX2_TARGET
	if (_pst_Object->p_CompressedNormals)
	{
		u_int *pCompreesedN,*pCompreesedNLast;
		pCompreesedN = _pst_Object->p_CompressedNormals;
		pCompreesedNLast = pCompreesedN + _pst_Object->l_NbPoints;
		pBinormalPtr = pBiNormales;
		for(; pCompreesedN < pCompreesedNLast; pCompreesedN++)
		{
			MATH_tdst_Vector X,Y,Up,stNormal;
			ULONG nX,nY,nZ;
			stNormal . x = (float)(((*pCompreesedN >> 0) & 0xff) - 128.0f) / 120.0f;
			stNormal . y = (float)(((*pCompreesedN >> 8) & 0xff) - 128.0f) / 120.0f;
			stNormal . z = (float)(((*pCompreesedN >> 16) & 0xff) - 128.0f) / 120.0f;
			MATH_NormalizeAnyVector(&stNormal, &stNormal);
			if (stNormal.z * stNormal.z < 0.999f)
			{
				Up.y = Up .x = 0.0f; 
				Up.z = 1.0f; 
				MATH_CrossProduct(&X , &stNormal , &Up);
				MATH_CrossProduct(&Y , &stNormal , &X);
				MATH_NormalizeAnyVector(&X, &X);
				MATH_NormalizeAnyVector(&Y, &Y);
				nX = (ULONG)((fMin(1.0f,fMax(-1.0f , X.x))) * 120.0f + 128.0f);
				nY = (ULONG)((fMin(1.0f,fMax(-1.0f , X.y))) * 120.0f + 128.0f);
				nZ = (ULONG)((fMin(1.0f,fMax(-1.0f , X.z))) * 120.0f + 128.0f);
				*(pBinormalPtr++) = (nX << 0) | (nY << 8) | (nZ << 16);
				nX = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.x))) * 120.0f + 128.0f);
				nY = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.y))) * 120.0f + 128.0f);
				nZ = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.z))) * 120.0f + 128.0f);
				*(pBinormalPtr++) = (nX << 0) | (nY << 8) | (nZ << 16);
			} else {
				Up.y = Up .z = 0.0f; 
				Up.x = 1.0f; 
				MATH_CrossProduct(&X , &stNormal , &Up);
				MATH_CrossProduct(&Y , &stNormal , &X);
				MATH_NormalizeAnyVector(&X, &X);
				MATH_NormalizeAnyVector(&Y, &Y);
				nX = (ULONG)((fMin(1.0f,fMax(-1.0f , X.x))) * 120.0f + 128.0f);
				nY = (ULONG)((fMin(1.0f,fMax(-1.0f , X.y))) * 120.0f + 128.0f);
				nZ = (ULONG)((fMin(1.0f,fMax(-1.0f , X.z))) * 120.0f + 128.0f);
				*(pBinormalPtr++) = (nX << 0) | (nY << 8) | (nZ << 16);
				nX = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.x))) * 120.0f + 128.0f);
				nY = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.y))) * 120.0f + 128.0f);
				nZ = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.z))) * 120.0f + 128.0f);
				*(pBinormalPtr++) = (nX << 0) | (nY << 8) | (nZ << 16);
			}//*/
		}
	} else
#endif
	{
		pst_Normal = _pst_Object ->dst_PointNormal;
		pst_LastNormal = pst_Normal+ _pst_Object->l_NbPoints;
		pBinormalPtr = pBiNormales;
		for(; pst_Normal < pst_LastNormal; pst_Normal++)
		{
			MATH_tdst_Vector X,Y,Up;
			ULONG nX,nY,nZ;
			if (pst_Normal->z * pst_Normal->z < 0.999f)
			{
				Up.y = Up .x = 0.0f; 
				Up.z = 1.0f; 
				MATH_CrossProduct(&X , pst_Normal , &Up);
				MATH_CrossProduct(&Y , pst_Normal , &X);
				MATH_NormalizeAnyVector(&X, &X);
				MATH_NormalizeAnyVector(&Y, &Y);
				nX = (ULONG)((fMin(1.0f,fMax(-1.0f , X.x))) * 120.0f + 128.0f);
				nY = (ULONG)((fMin(1.0f,fMax(-1.0f , X.y))) * 120.0f + 128.0f);
				nZ = (ULONG)((fMin(1.0f,fMax(-1.0f , X.z))) * 120.0f + 128.0f);
				*(pBinormalPtr++) = (nX << 0) | (nY << 8) | (nZ << 16);
				nX = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.x))) * 120.0f + 128.0f);
				nY = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.y))) * 120.0f + 128.0f);
				nZ = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.z))) * 120.0f + 128.0f);
				*(pBinormalPtr++) = (nX << 0) | (nY << 8) | (nZ << 16);
			} else {
				Up.y = Up .z = 0.0f; 
				Up.x = 1.0f; 
				MATH_CrossProduct(&X , pst_Normal , &Up);
				MATH_CrossProduct(&Y , pst_Normal , &X);
				MATH_NormalizeAnyVector(&X, &X);
				MATH_NormalizeAnyVector(&Y, &Y);
				nX = (ULONG)((fMin(1.0f,fMax(-1.0f , X.x))) * 120.0f + 128.0f);
				nY = (ULONG)((fMin(1.0f,fMax(-1.0f , X.y))) * 120.0f + 128.0f);
				nZ = (ULONG)((fMin(1.0f,fMax(-1.0f , X.z))) * 120.0f + 128.0f);
				*(pBinormalPtr++) = (nX << 0) | (nY << 8) | (nZ << 16);
				nX = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.x))) * 120.0f + 128.0f);
				nY = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.y))) * 120.0f + 128.0f);
				nZ = (ULONG)((fMin(1.0f,fMax(-1.0f , Y.z))) * 120.0f + 128.0f);
				*(pBinormalPtr++) = (nX << 0) | (nY << 8) | (nZ << 16);
			}
		}
	}
 }

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_GetNumberOfTriangles(GEO_tdst_Object *_pst_Obj)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    LONG                                l_Number;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_Obj) return 0;

    pst_Element = _pst_Obj->dst_Element;
    pst_LastElement = pst_Element + _pst_Obj->l_NbElements;
    for(l_Number = 0; pst_Element < pst_LastElement; pst_Element++)
    {
        l_Number += pst_Element->l_NbTriangles;
    }
    return l_Number;
}
#if defined(_XENON_RENDER)

/*
=======================================================================================================================
=======================================================================================================================
*/
void GEO_CalculateTangentSpaceBasis(GEO_tdst_Object* poGeoObj,
                                    ULONG ulElement,                   
                                    GEO_tdst_TextureSpaceBasis *pBasis,
                                    GEO_Vertex* _pVertexList)
{
    const FLOAT SMALL_FLOAT = 1e-12f;

    static BOOL         s_abSharingRejected[SOFT_Cul_ComputingBufferSize];
    GEO_tdst_IndexedTriangle* pTris;
    GEO_tdst_IndexedTriangle* pTrisEnd;
    GEO_Vertex          *pVertices = (_pVertexList != NULL) ? _pVertexList : poGeoObj->dst_Point;
    GEO_tdst_UV         *pUVs = poGeoObj->dst_UV;
    MATH_tdst_Vector    *poNormals = poGeoObj->dst_PointNormal;

    if (poGeoObj->l_NbPoints > SOFT_Cul_ComputingBufferSize)
    {
        ERR_OutputDebugString("[Error] GEO_CalculateTangentSpaceBasis() - Too many points in the object (%d, maximum is %u)\n",
                              poGeoObj->l_NbPoints, SOFT_Cul_ComputingBufferSize);
        return;
    }

    // triangle list
   // Clear the basis vectors
    MATH_tdst_Vector oNullVec = {0.0f, 0.0f, 0.0f};
    for( int i = 0; i < poGeoObj->l_NbPoints; i++)
    {
        pBasis[i].S = oNullVec;
        pBasis[i].T = oNullVec;
        pBasis[i].Active = 0;
        pBasis[i].BinormalDirection = 1.0f;
        poNormals[i] = oNullVec;
    }

    // Walk through the triangle list and calculate gradiants for each triangle.
    // Sum the results into the S and T components.
    DWORD index = 0, ind0 = 0, ind1 = 0, ind2 = 0;

    for (ULONG e = 0; e < poGeoObj->l_NbElements; ++e)
    {
        pTris       = poGeoObj->dst_Element[e].dst_Triangle;
        pTrisEnd    = pTris + poGeoObj->dst_Element[e].l_NbTriangles;

        if (poGeoObj->dst_Element[e].l_NbTriangles == 0)
            continue;

        while ( pTris < pTrisEnd )
        {
            GEO_Vertex *pVert0, *pVert1, *pVert2;
            GEO_tdst_UV *pUV0, *pUV1, *pUV2;
            GEO_tdst_TextureSpaceBasis *pBV0, *pBV1, *pBV2;

    
            // first vertex
            pVert0 = &pVertices[pTris->auw_Index[0]];

            // second vertex
            pVert1 = &pVertices[pTris->auw_Index[1]];
            pBasis[pTris->auw_Index[1]].Active |= 1;

            // third vertex
            pVert2 = &pVertices[pTris->auw_Index[2]];
            pBasis[pTris->auw_Index[2]].Active |= 1;

            pUV0 = &pUVs[pTris->auw_UV[0]];
            pUV1 = &pUVs[pTris->auw_UV[1]];
            pUV2 = &pUVs[pTris->auw_UV[2]];

            pBV0 = &pBasis[pTris->auw_Index[0]];
            pBasis[pTris->auw_Index[0]].Active |= 1;

            pBV1 = &pBasis[pTris->auw_Index[1]];
            pBasis[pTris->auw_Index[1]].Active |= 1;

            pBV2 = &pBasis[pTris->auw_Index[2]];
            pBasis[pTris->auw_Index[2]].Active |= 1;

            MATH_tdst_Vector v1, v2, vFaceNormal;
            MATH_SubVector(&v1, pVert1, pVert0);
            MATH_SubVector(&v2, pVert2, pVert0);
            MATH_CrossProduct( &vFaceNormal, &v1, &v2 );
            D3DXVec3Normalize( (D3DXVECTOR3*)&vFaceNormal, (D3DXVECTOR3*)&vFaceNormal );

            for( int j=0; j<3; ++j )
            {
                int index = pTris->auw_Index[j];
                if( (pBasis[pTris->auw_Index[j]].Active & 2) == 0)
                {
                    poNormals[pTris->auw_Index[j]] = vFaceNormal;
                    pBasis[pTris->auw_Index[j]].Active |= 2;
                }
            }

            ++pTris;

            MATH_tdst_Vector edge01;
            MATH_tdst_Vector edge02;
            MATH_tdst_Vector cp;

            FLOAT ds1 = pUV1->fU - pUV0->fU;
            FLOAT dt1 = pUV1->fV - pUV0->fV;

            FLOAT ds2 = pUV2->fU - pUV0->fU;
            FLOAT dt2 = pUV2->fV - pUV0->fV;

            // x, s, t
            edge01.x = pVert1->x - pVert0->x;
            edge01.y = ds1;
            edge01.z = dt1;
            edge02.x = pVert2->x - pVert0->x;
            edge02.y = ds2;
            edge02.z = dt2;

            MATH_CrossProduct( &cp, &edge01, &edge02 );
            if ( fabs(cp.x) > SMALL_FLOAT )
            {
                FLOAT dsdx = -cp.y / cp.x;
                FLOAT dtdx = -cp.z / cp.x;

                pBV0->S.x += dsdx;
                pBV0->T.x += dtdx;

                pBV1->S.x += dsdx;
                pBV1->T.x += dtdx;

                pBV2->S.x += dsdx;
                pBV2->T.x += dtdx;
            }

            // y, s, t
            edge01.x = pVert1->y - pVert0->y;
            edge02.x = pVert2->y - pVert0->y;

            MATH_CrossProduct( &cp, &edge01, &edge02 );
            if ( fabs(cp.x) > SMALL_FLOAT )
            {
                FLOAT dsdx = -cp.y / cp.x;
                FLOAT dtdx = -cp.z / cp.x;

                pBV0->S.y += dsdx;
                pBV0->T.y += dtdx;

                pBV1->S.y += dsdx;
                pBV1->T.y += dtdx;

                pBV2->S.y += dsdx;
                pBV2->T.y += dtdx;
            }

            // z, s, t
            edge01.x = pVert1->z - pVert0->z;
            edge02.x = pVert2->z - pVert0->z;

            MATH_CrossProduct( &cp, &edge01, &edge02 );
            if ( fabs(cp.x) > SMALL_FLOAT )
            {
                FLOAT dsdx = -cp.y / cp.x;
                FLOAT dtdx = -cp.z / cp.x;

                pBV0->S.z += dsdx;
                pBV0->T.z += dtdx;

                pBV1->S.z += dsdx;
                pBV1->T.z += dtdx;

                pBV2->S.z += dsdx;
                pBV2->T.z += dtdx;
            }
        }
    }
    /*
    // Paramerters that define if verts are shared.
    const FLOAT fDistMax = 1e-6f;
    const FLOAT fNormAngMin = 0.939693f;  // 20 degrees
    const FLOAT fTanAngMin = 0.939693f;   // 20 degrees

    BOOL* bSharingRejected = s_abSharingRejected;

    memset( bSharingRejected, 0, sizeof(BOOL) * poGeoObj->l_NbPoints);

    // Any verts that share position and normal, but don't share texture
    // co-ordinates should be treated as shared.
    for ( DWORD i = 0; i < (DWORD) poGeoObj->l_NbPoints; i++ )
    {
    if (bSharingRejected[i] || (pBasis[i].Active == 0))
    continue;

    MATH_tdst_Vector vS = pBasis[i].S;
    MATH_tdst_Vector vT = pBasis[i].T;

    for ( DWORD j = i+1; j < (DWORD) poGeoObj->l_NbPoints; j++ )
    {
    if (pBasis[j].Active == 0)
    continue;

    // Are position and normal shared?
    MATH_tdst_Vector vDelta;
    MATH_SubVector(&vDelta, &pVertices[i], &pVertices[j]);
    FLOAT fDist = MATH_f_NormVector( &vDelta );
    FLOAT fCosA = MATH_f_DotProduct( &poNormals[i], &poNormals[j] );

    // Less than 1 / 1000000 distance, and 20 degrees.
    if ( fDist < fDistMax && fCosA > fNormAngMin )
    {
    // Average S and T vectors for i and j.
    MATH_AddVector(&vS, &vS, &pBasis[j].S);
    MATH_AddVector(&vT, &vT, &pBasis[j].T);
    }
    }

    // Do a check of the existing basis vectors vs. the newly calculated 
    // ones to make sure we really should average the textures spaces.
    // For example at the poles of a sphere we do not want to average the
    // texture spaces.
    BOOL bUseAverage = true;

    MATH_tdst_Vector vSavg, vTavg;
    MATH_NormalizeAnyVector( &vSavg, &vS );
    MATH_NormalizeAnyVector( &vTavg, &vT );

    MATH_tdst_Vector vSi, vTi;
    MATH_NormalizeAnyVector( &vSi, &pBasis[i].S );
    MATH_NormalizeAnyVector( &vTi, &pBasis[i].T );

    FLOAT fDotS = MATH_f_DotProduct( &vSi, &vSavg );
    FLOAT fDotT = MATH_f_DotProduct( &vTi, &vTavg );

    if (fDotS <= fTanAngMin || fDotT <= fTanAngMin)
    {
    bUseAverage = false;
    }

    for ( DWORD j = i+1; j < (DWORD) poGeoObj->l_NbPoints; j++ )
    {
    if (pBasis[j].Active == 0)
    continue;

    // Are position and normal shared?
    MATH_tdst_Vector vDelta;
    MATH_SubVector(&vDelta, &pVertices[i], &pVertices[j]);
    FLOAT fDist = MATH_f_NormVector( &vDelta );
    FLOAT fCosA = MATH_f_DotProduct( &poNormals[i], &poNormals[j] );

    // Less than 1 / 1000000 distance, and 20 degrees.
    if ( fDist < fDistMax && fCosA > fNormAngMin )
    {
    MATH_tdst_Vector vSj, vTj;
    MATH_NormalizeAnyVector( &vSj, &pBasis[j].S );
    MATH_NormalizeAnyVector( &vTj, &pBasis[j].T );

    fDotS = MATH_f_DotProduct( &vSj, &vSavg );
    fDotT = MATH_f_DotProduct( &vTj, &vTavg );

    if (fDotS <= fTanAngMin || fDotT <= fTanAngMin)
    {
    bUseAverage = false;
    break;
    }
    }
    }

    if( bUseAverage )
    {
    // Set the basis vectors for all the verts to the average.
    for ( DWORD j = i+1; j < (DWORD) poGeoObj->l_NbPoints; j++ )
    {
    if (pBasis[j].Active == 0)
    continue;

    // Are position and normal shared?
    MATH_tdst_Vector vDelta;
    MATH_SubVector(&vDelta, &pVertices[i], &pVertices[j]);
    FLOAT fDist = MATH_f_NormVector( &vDelta );
    FLOAT fCosA = MATH_f_DotProduct( &poNormals[i], &poNormals[j] );

    // Less than 1 / 1000000 distance, and 10 degrees.
    if ( fDist < fDistMax && fCosA > fNormAngMin )
    {
    pBasis[j].S = vS;
    pBasis[j].T = vT;
    }
    }

    pBasis[i].S = vS;
    pBasis[i].T = vT;
    }
    else
    {
    // Prevent the vertices from being considered again.
    for ( DWORD j = i+1; j < (DWORD) poGeoObj->l_NbPoints; j++ )
    {
    if (pBasis[j].Active == 0)
    continue;

    // Are position and normal shared?
    MATH_tdst_Vector vDelta;
    MATH_SubVector(&vDelta, &pVertices[i], &pVertices[j]);
    FLOAT fDist = MATH_f_NormVector( &vDelta );
    FLOAT fCosA = MATH_f_DotProduct( &poNormals[i], &poNormals[j] );

    // Less than 1 / 1000000 distance, and 10 degrees.
    if ( fDist < fDistMax && fCosA > fNormAngMin )
    {
    bSharingRejected[j] = true;
    }
    }

    bSharingRejected[i] = true;
    }
    }
    */

    // Calculate the SxT vector
    for ( DWORD i = 0; i < (DWORD) poGeoObj->l_NbPoints; i++ )
    {
        pBasis[i].Active &= ~2;

        if ((pBasis[i].Active & 1) == 0)
            continue;

        // Normalize the S, T vectors
        MATH_NormalizeAnyVector( &pBasis[i].S, &pBasis[i].S );
        MATH_NormalizeAnyVector( &pBasis[i].T, &pBasis[i].T );

        // Get the cross of the S and T vectors
        MATH_CrossProduct( &pBasis[i].SxT, &pBasis[i].S, &pBasis[i].T );

        MATH_NormalizeAnyVector( &pBasis[i].SxT, &pBasis[i].SxT );

        // Fix the direction of the SxT vector
        if ( MATH_f_DotProduct( &pBasis[i].SxT, &poNormals[i]) < 0.0f )
        {
            MATH_NegVector(&pBasis[i].SxT, &pBasis[i].SxT);
            pBasis[i].BinormalDirection = -1.0f;
        }
    }
}

#endif // _XENON || ACTIVE_EDITORS
#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ComputeNormalsOfSelected(GEO_tdst_Object *_pst_Object, char *_pc_Sel)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_Vertex                          *pst_Point, *pst_Pt[3];
    MATH_tdst_Vector                    *pst_Normal, *pst_LastNormal;
    MATH_tdst_Vector                    st_TriangleNormal, st_Vect1, st_Vect2;
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
    unsigned short                      pt1, pt2, pt3;
    LONG                                flag;
    char                                *dc_Sel, *pc_CurSel;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    dc_Sel = (char *) L_malloc( _pst_Object->l_NbPoints );
    L_memcpy(dc_Sel, _pc_Sel, _pst_Object->l_NbPoints);

    pst_Element = _pst_Object->dst_Element;
    pst_LastElement = pst_Element + _pst_Object->l_NbElements;
    for(; pst_Element < pst_LastElement; pst_Element++) 
    {
        pst_Triangle = pst_Element->dst_Triangle;
        pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

        for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
        {
            pt1 = pst_Triangle->auw_Index[0];
            pt2 = pst_Triangle->auw_Index[1];
            pt3 = pst_Triangle->auw_Index[2];

            if((_pc_Sel[pt1] & 1) || (_pc_Sel[pt2] & 1) || (_pc_Sel[pt3] & 1))
            {
                dc_Sel[pt1] = 1;
                dc_Sel[pt2] = 1;
                dc_Sel[pt3] = 1;
            }
        }
    }

    pc_CurSel = dc_Sel;
    pst_Normal = _pst_Object->dst_PointNormal;
    pst_LastNormal = pst_Normal + _pst_Object->l_NbPoints;
    for(; pst_Normal < pst_LastNormal; pst_Normal++, pc_CurSel++)
        if(*pc_CurSel & 1)
            MATH_InitVectorToZero(pst_Normal);

    pst_Point = _pst_Object->dst_Point;
    pst_Normal = _pst_Object->dst_PointNormal;

    pst_Element = _pst_Object->dst_Element;
    for(; pst_Element < pst_LastElement; pst_Element++)
    {
        pst_Triangle = pst_Element->dst_Triangle;
        pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

        for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
        {
            pt1 = pst_Triangle->auw_Index[0];
            pt2 = pst_Triangle->auw_Index[1];
            pt3 = pst_Triangle->auw_Index[2];

            flag = (dc_Sel[pt1] & 1) ? 1 : 0;
            flag |= (dc_Sel[pt2] & 1) ? 2 : 0;
            flag |= (dc_Sel[pt3] & 1) ? 4 : 0;

            if(!flag) continue;

            pst_Pt[0] = pst_Point + pt1;
            pst_Pt[1] = pst_Point + pt2;
            pst_Pt[2] = pst_Point + pt3;

            MATH_SubVector(&st_Vect1, VCast( pst_Pt[0] ), VCast( pst_Pt[1] ) );
            MATH_SubVector(&st_Vect2, VCast( pst_Pt[0] ), VCast( pst_Pt[2] ) );

            MATH_CrossProduct(&st_TriangleNormal, &st_Vect1, &st_Vect2);
            MATH_NormalizeAnyVector(&st_TriangleNormal, &st_TriangleNormal);

            if(flag & 1)
                MATH_AddEqualVector(pst_Normal + pt1, &st_TriangleNormal);
            if(flag & 2)
                MATH_AddEqualVector(pst_Normal + pt2, &st_TriangleNormal);
            if(flag & 4)
                MATH_AddEqualVector(pst_Normal + pt3, &st_TriangleNormal);
        }
    }

    pc_CurSel = dc_Sel;
    for(; pst_Normal < pst_LastNormal; pst_Normal++, pc_CurSel++)
        if(*pc_CurSel & 1)
            MATH_NormalizeAnyVector(pst_Normal, pst_Normal);

    L_free(dc_Sel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ComputeNormalsOfPlanetAura( GEO_tdst_Object *_pst_Obj )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    *pst_Normal;
    GEO_Vertex          *pst_Point, *pst_Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Point = _pst_Obj->dst_Point;
    pst_Last = pst_Point + _pst_Obj->l_NbPoints;
    pst_Normal = _pst_Obj->dst_PointNormal;

    for ( ; pst_Point < pst_Last; pst_Point++, pst_Normal++ )
    {
        MATH_NormalizeVector( pst_Normal, VCast( pst_Point ) );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_PickRLI( GEO_tdst_Object *_pst_Tgt, ULONG **_ppul_Tgt, GEO_tdst_Object *_pst_Src, ULONG *_pul_Src, char _c_Sel )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG                           ul_Nearest;
    float                           delta, f_Cur, f_Dist;
    int                             i, j;
    GEO_Vertex                      *pst_SrcPt, *pst_TgtPt;
    char                            *dc_Sel, *pc_Sel;
    ULONG                           *pul_Tgt;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    if ( !_pul_Src || !_pst_Src || !_pst_Tgt || !_ppul_Tgt ) return;
    
    if ( (_c_Sel) && (_pst_Tgt->pst_SubObject != NULL) )
    {
        dc_Sel = NULL;
        pc_Sel = _pst_Tgt->pst_SubObject->dc_VSel;
    }
    else
    {
        dc_Sel = (char*)L_malloc( _pst_Tgt->l_NbPoints );
        L_memset( dc_Sel, 1, _pst_Tgt->l_NbPoints );
        pc_Sel = dc_Sel;
    }

    if (*_ppul_Tgt)
        pul_Tgt = *_ppul_Tgt;
    else
    {
        pul_Tgt = (ULONG   *) MEM_p_Alloc( 4 * (_pst_Tgt->l_NbPoints + 1) );
        pul_Tgt[0] = _pst_Tgt->l_NbPoints;
        *_ppul_Tgt = pul_Tgt;
    }
        
    pst_TgtPt = _pst_Tgt->dst_Point;
    for ( i = 0; i < _pst_Tgt->l_NbPoints; i++, pst_TgtPt++, pc_Sel++)
    {
        if ( !(*pc_Sel & 1) ) continue;

        f_Dist = Cf_Infinit;

        pst_SrcPt = _pst_Src->dst_Point;
        for ( j = 0; j < _pst_Src->l_NbPoints; j++, pst_SrcPt++)
        {
            delta = pst_SrcPt->x - pst_TgtPt->x;
            delta *= delta;
            if (delta < f_Dist)
            {
                f_Cur = pst_SrcPt->y - pst_TgtPt->y;
                f_Cur *= f_Cur;
                f_Cur += delta;

                if (delta < f_Dist)
                {
                    delta = pst_SrcPt->z - pst_TgtPt->z;
                    delta *= delta;
                    f_Cur += delta;
                    if (f_Cur < f_Dist)
                    {
                        f_Dist = f_Cur;
                        ul_Nearest = j;
                    }
                }
            }
        }

        pul_Tgt[ i + 1 ] = _pul_Src[ ul_Nearest + 1 ];
    }

    if (dc_Sel)
        L_free( dc_Sel );

    //GEO_DestroyRLI( _pst_Tgt );
}

#endif 


 

#if defined(PSX2_TARGET)  && defined(__cplusplus)
}
#endif

/* GEO_LIGHTCUT_GEO.c */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "SELection/SELection.h"
#include "GRObject/GROstruct.h"
#include "GRObject/GROedit.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOobjectaccess.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_LIGHTCUT.h"
#include "GEOmetric/GEO_MRM.h"

#include <math.h>
#include <STDLIB.H>
#include "../dlls/MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "../dlls/MAD_mem/Sources/MAD_mem.h"
#include "GEOmetric/GEO_LODCmpt.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


tdst_GLV *GLV_Geo2GLV 		(GEO_tdst_Object *pst_Object,MATH_tdst_Matrix	*p_Matrix , ULONG *p_ulColours , ULONG ulFlags , tdst_ObjectBDVolume *p_stOBV ,ULONG MaterialFlags)
{
	tdst_GLV *p_Ret;
	MATH_tdst_Vector stRes;
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
	tdst_GLV_Face						*pst_GLVFace;
	LONG								ulNumberOfTriangles,ulNP;
	MATH_tdst_Vector					stMin,stMax,stObvvLocal;

	GEO_MRM_RESET(pst_Object);
	p_Ret	=	NewGLV();

	if (p_ulColours == NULL) 
		if (pst_Object->dul_PointColors)
			p_ulColours = pst_Object->dul_PointColors + 1;


	/* Compute OBV */
	stMin = *(MATH_tdst_Vector *) &pst_Object->dst_Point[0];
	stMax = *(MATH_tdst_Vector *) &pst_Object->dst_Point[0];
	for (ulNP = 0; ulNP < pst_Object->l_NbPoints; ulNP++)
	{
		stMin.x = pst_Object->dst_Point[ulNP].x > stMin.x ? stMin.x : pst_Object->dst_Point[ulNP].x;
		stMin.y = pst_Object->dst_Point[ulNP].y > stMin.y ? stMin.y : pst_Object->dst_Point[ulNP].y;
		stMin.z = pst_Object->dst_Point[ulNP].z > stMin.z ? stMin.z : pst_Object->dst_Point[ulNP].z;
		stMax.x = pst_Object->dst_Point[ulNP].x < stMax.x ? stMax.x : pst_Object->dst_Point[ulNP].x;
		stMax.y = pst_Object->dst_Point[ulNP].y < stMax.y ? stMax.y : pst_Object->dst_Point[ulNP].y;
		stMax.z = pst_Object->dst_Point[ulNP].z < stMax.z ? stMax.z : pst_Object->dst_Point[ulNP].z;
	}

	for (ulNP = 0; ulNP < 8; ulNP++)
	{
		stObvvLocal.x = (ulNP & 1) ? stMin.x : stMax.x;
		stObvvLocal.y = (ulNP & 2) ? stMin.y : stMax.y;
		stObvvLocal.z = (ulNP & 4) ? stMin.z : stMax.z;
		MATH_TransformVertex(&stRes, p_Matrix, &stObvvLocal);
		MATH_To_MATHD(&p_stOBV->stVectors[ulNP] , &stRes);
	}
	

	/* Points */
	GLV_SetNumbers(p_Ret , pst_Object->l_NbPoints , 0 , 0 ,1 /* points */);
	for (ulNP = 0; ulNP < pst_Object->l_NbPoints; ulNP++)
	{
		MATH_TransformVertex(&stRes, p_Matrix, (MATH_tdst_Vector *) &pst_Object->dst_Point[ulNP]);
		p_Ret->p_stPoints[ulNP].P3D . x = (GLV_Scalar)stRes.x;
		p_Ret->p_stPoints[ulNP].P3D . y = (GLV_Scalar)stRes.y;
		p_Ret->p_stPoints[ulNP].P3D . z = (GLV_Scalar)stRes.z;
		p_Ret->p_stPoints[ulNP].ulRefIndex = ulNP;
		p_Ret->p_stPoints[ulNP].ulFlags = GLV_FLGS_Original;
		p_Ret->p_stPoints[ulNP].ulColor = 0xffffff;
	}

	/* UV's */
	if (pst_Object->dst_UV)
	{
		ulNP = pst_Object->l_NbUVs;
		while (ulNP--)
		{
			if (_isnan(pst_Object->dst_UV[ulNP].fU)) 
				pst_Object->dst_UV[ulNP].fU = 0.0f;
			if (_isnan(pst_Object->dst_UV[ulNP].fV)) 
				pst_Object->dst_UV[ulNP].fV = 0.0f;
		}
	} 


	/* Triangles */
	pst_Element = pst_Object->dst_Element;
	pst_LastElement = pst_Element + pst_Object->l_NbElements;
	ulNumberOfTriangles = 0;
	while (pst_Element < pst_LastElement) ulNumberOfTriangles += (pst_Element++)->l_NbTriangles;
	GLV_SetNumbers(p_Ret , pst_Object->l_NbPoints , 0 , ulNumberOfTriangles ,4 /* faces */);
	pst_Element = pst_Object->dst_Element;
	pst_LastElement = pst_Element + pst_Object->l_NbElements;
	pst_GLVFace =  p_Ret->p_stFaces;
	while (pst_Element < pst_LastElement)
	{
		pst_Triangle = pst_Element->dst_Triangle;
		pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;
		while (pst_Triangle < pst_LastTriangle)
		{
			pst_GLVFace->ulElementJADE = pst_Element->l_MaterialId;

			pst_GLVFace->Index[0] = pst_Triangle->auw_Index[0];
			pst_GLVFace->Index[1] = pst_Triangle->auw_Index[1];
			pst_GLVFace->Index[2] = pst_Triangle->auw_Index[2];
			MATH_TransformVector(&pst_GLVFace->OriginalPointsNormales[0], p_Matrix, &pst_Object->dst_PointNormal[pst_Triangle->auw_Index[0]]);
			MATH_TransformVector(&pst_GLVFace->OriginalPointsNormales[1], p_Matrix, &pst_Object->dst_PointNormal[pst_Triangle->auw_Index[1]]);
			MATH_TransformVector(&pst_GLVFace->OriginalPointsNormales[2], p_Matrix, &pst_Object->dst_PointNormal[pst_Triangle->auw_Index[2]]);
			if (pst_Object->dst_UV)
			{
				if (pst_Object->l_NbUVs)
				{
					if (pst_Triangle->auw_UV[0] > pst_Object->l_NbUVs) 
						pst_Triangle->auw_UV[0] = (unsigned short)(pst_Object->l_NbUVs - 1);
					if (pst_Triangle->auw_UV[1] > pst_Object->l_NbUVs) 
						pst_Triangle->auw_UV[1] = (unsigned short)(pst_Object->l_NbUVs - 1);
					if (pst_Triangle->auw_UV[2] > pst_Object->l_NbUVs) 
						pst_Triangle->auw_UV[2] = (unsigned short)(pst_Object->l_NbUVs - 1);
				}
				pst_GLVFace->UV[0] = pst_Object->dst_UV[pst_Triangle->auw_UV[0]].fU;
				pst_GLVFace->UV[1] = pst_Object->dst_UV[pst_Triangle->auw_UV[0]].fV;
				pst_GLVFace->UV[2] = pst_Object->dst_UV[pst_Triangle->auw_UV[1]].fU;
				pst_GLVFace->UV[3] = pst_Object->dst_UV[pst_Triangle->auw_UV[1]].fV;
				pst_GLVFace->UV[4] = pst_Object->dst_UV[pst_Triangle->auw_UV[2]].fU;
				pst_GLVFace->UV[5] = pst_Object->dst_UV[pst_Triangle->auw_UV[2]].fV;
			} else
			{
				pst_GLVFace->UV[0] = pst_GLVFace->UV[1] = pst_GLVFace->UV[2] = pst_GLVFace->UV[3] = pst_GLVFace->UV[4] = pst_GLVFace->UV[5] = 0.0f;
			}
			if (p_ulColours)
			{
				pst_GLVFace->Colours[0] = p_ulColours[pst_Triangle->auw_Index[0]] & 0xffffff;
				pst_GLVFace->Colours[1] = p_ulColours[pst_Triangle->auw_Index[1]] & 0xffffff;
				pst_GLVFace->Colours[2] = p_ulColours[pst_Triangle->auw_Index[2]] & 0xffffff;
				pst_GLVFace->falpha[0] = (float)(p_ulColours[pst_Triangle->auw_Index[0]] >> 24);
				pst_GLVFace->falpha[1] = (float)(p_ulColours[pst_Triangle->auw_Index[1]] >> 24);
				pst_GLVFace->falpha[2] = (float)(p_ulColours[pst_Triangle->auw_Index[2]] >> 24);
			} else
			{
				pst_GLVFace->Colours[0] = pst_GLVFace->Colours[1] = pst_GLVFace->Colours[2] = 0;
				pst_GLVFace->falpha[0] = 0.0f;
				pst_GLVFace->falpha[1] = 0.0f;
				pst_GLVFace->falpha[2] = 0.0f;
			} 
			pst_GLVFace->Nghbr[0] = pst_GLVFace->Nghbr[1] = pst_GLVFace->Nghbr[2] = -1;

			pst_GLVFace->ulFlags  = ulFlags ;
			if (MaterialFlags & (1 << pst_Element->l_MaterialId))
				pst_GLVFace->ulFlags |=  GLV_FLGS_Transparent | GLV_FLGS_DoNotCut;

			pst_GLVFace->ulMARK   = 0xffffffff;
			pst_GLVFace++;
			pst_Triangle++;
		}
		pst_Element++;
	}
	GLV_ComputeNormales(p_Ret);
//	GLV_SubdivideLenght(p_Ret, 0.15f);
//	GLV_BreakUncompatibleLinks_JadeElement_and_UV(p_Ret);
	return p_Ret;
}
/* This function will re-create UV, */
/* Object is Cut. */
GEO_tdst_Object *GLV_GLV2GEO (	tdst_GLV *p_stGLVOriginal,MATH_tdst_Matrix	*p_Matrix , ULONG ulMark)
{
	GEO_tdst_Object *p_RetGEO;
	MATH_tdst_Vector stConv;
	MATH_tdst_Matrix	stMatrixIV;
	ULONG NumBErOfUV,NumBerOfPoints,NumVC,NumElem,Counter,Counter3;

	ULONG *p_NumFacePElement ; 
	ULONG *p_ElementMID ; 

	GLV_Cluster *p_stCluster1;

	tdst_GLV *p_stGLV2 , *p_stGLV;

	p_stCluster1 = GLV_NewCluster();
	GLV_ResetCluster(p_stCluster1);

	p_stGLV = GLV_Duplicate(p_stGLVOriginal);
	GLV_RemoveKILT(p_stGLV);
	/* Erase all faces != of ulMark */
	Counter = p_stGLV->ulNumberOfFaces;
	while (Counter--)
	{
		if ((p_stGLV->p_stFaces[Counter].ulMARK & 0x3FFFFFFF) != ulMark)
			p_stGLV->p_stFaces[Counter].ulFlags |= GLV_FLGS_DeleteIt;
		else
		{
			GLV_Clst_ADDV(p_stCluster1 , p_stGLV->p_stFaces[Counter].ulElementJADE);
			p_stGLV->p_stFaces[Counter].ulFlags &= ~GLV_FLGS_DeleteIt;
		}
	}
	NumElem	= GLV_ulGetClusterNumber(p_stCluster1);
	p_NumFacePElement = (ULONG*)GLV_ALLOC(4L * NumElem	);
	p_ElementMID	  = (ULONG*)GLV_ALLOC(4L * NumElem	);
	L_memset(p_NumFacePElement , 0 , 4L * NumElem	);
	GLD_Remove2BD(p_stGLV);
	GLV_RemoveKILT(p_stGLV);
	GLV_SetNumbers(p_stGLV , 0 , 0 , 0 , 2); /* remove edges */
	GLD_RemoveUnusedIndexes(p_stGLV);

	Counter = p_stGLV->ulNumberOfFaces;
	while (Counter--)
	{
		Counter3 = GLV_ulGetPos(p_stCluster1 , p_stGLV->p_stFaces[Counter].ulElementJADE);
		p_ElementMID[Counter3] = p_stGLV->p_stFaces[Counter].ulElementJADE;
		p_NumFacePElement[Counter3]++;
	}


	p_stGLV2 = GLD_CompressUVIndexion( p_stGLV );

	/* Count number of UV */
	NumBErOfUV = p_stGLV2 -> ulNumberOfPoints; 
	/* Count number of Points */
	NumBerOfPoints = p_stGLV->ulNumberOfPoints;
	/* Count number of VC */
	NumVC = NumBerOfPoints;
	/* Count number of Elements */

	p_RetGEO = GEO_pst_Create(NumBerOfPoints, NumBErOfUV, NumElem, NumVC);
	Counter = NumElem;
	while (Counter--)
	{
		p_RetGEO->dst_Element[Counter].l_NbTriangles = p_NumFacePElement[Counter];
		p_RetGEO->dst_Element[Counter].l_MaterialId = p_ElementMID[Counter];
		p_RetGEO->dst_Element[Counter].pus_ListOfUsedIndex = NULL;
		p_RetGEO->dst_Element[Counter].ul_NumberOfUsedIndex = 0;
		p_RetGEO->dst_Element[Counter].pst_StripData = NULL;
		p_RetGEO->dst_Element[Counter].p_MrmElementAdditionalInfo = NULL;
		GEO_AllocElementContent(&p_RetGEO->dst_Element[Counter]);
	}
	

	{
		MATH_tdst_Matrix	stMatrix12;
		stMatrixIV = stMatrix12 = *p_Matrix;
		if (MATH_b_TestScaleType(p_Matrix))
		{
			MATH_ScaleEqualVector(MATH_pst_GetXAxis(&stMatrix12) , stMatrix12.Sx);
			MATH_ScaleEqualVector(MATH_pst_GetYAxis(&stMatrix12) , stMatrix12.Sy);
			MATH_ScaleEqualVector(MATH_pst_GetZAxis(&stMatrix12) , stMatrix12.Sz);
		}
		stMatrix12.Sx = stMatrix12.Sz = stMatrix12.Sy = 1.0f;
		stMatrixIV.Sx = stMatrixIV.Sz = stMatrixIV.Sy = 1.0f;
		MATH_Invert33Matrix(&stMatrixIV, &stMatrix12);
		MATH_NegEqualVector(&stMatrix12.T);
		MATH_TransformVector(&stMatrixIV.T, &stMatrixIV , &stMatrix12.T);
	}

//	MATH_InvertMatrix(&stMatrixIV , p_Matrix);
	/* Restore Alphas */
	Counter = p_stGLV->ulNumberOfFaces;
	while (Counter--)
	{
		for (Counter3 = 0 ; Counter3 < 3 ; Counter3++)
		{
			ULONG Color,Alpha;
			if (p_stGLV->p_stFaces[Counter].falpha[Counter3] > 255.0f) p_stGLV->p_stFaces[Counter].falpha[Counter3] = 255.0f;
			if (p_stGLV->p_stFaces[Counter].falpha[Counter3] < 0.0f) p_stGLV->p_stFaces[Counter].falpha[Counter3] = 0.0f;
			Alpha = (ULONG)p_stGLV->p_stFaces[Counter].falpha[Counter3];
			Color = p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[Counter3]].ulColor;
			Color &= 0xffffff;
			Color |= Alpha << 24;
			p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[Counter3]].ulColor = Color ;
		}
	}

	Counter = p_stGLV->ulNumberOfPoints;
	while (Counter--)
	{
		MATHD_To_MATH(&stConv,&p_stGLV->p_stPoints[Counter].P3D);
		MATH_TransformVertex((MATH_tdst_Vector *) &p_RetGEO->dst_Point[Counter], &stMatrixIV , &stConv);
		p_RetGEO ->dul_PointColors[Counter + 1] = p_stGLV->p_stPoints[Counter].ulColor;
	}

	Counter = p_RetGEO->l_NbUVs;
	while (Counter--)
	{
		p_RetGEO -> dst_UV[Counter].fU = (float)p_stGLV2->p_stPoints[Counter].P3D.x;
		p_RetGEO -> dst_UV[Counter].fV = (float)p_stGLV2->p_stPoints[Counter].P3D.y;
	}

	Counter = NumElem;
	while (Counter--)
	{
		p_RetGEO->dst_Element[Counter].l_NbTriangles = 0;
	}

	Counter = p_stGLV->ulNumberOfFaces;
	while (Counter--)
	{
		ULONG Pos;
		Pos = GLV_ulGetPos(p_stCluster1 , p_stGLV->p_stFaces[Counter].ulElementJADE);
		p_RetGEO ->dst_Element[Pos].dst_Triangle[p_RetGEO ->dst_Element[Pos].l_NbTriangles].auw_Index[0] = (unsigned short)p_stGLV->p_stFaces[Counter].Index[0];
		p_RetGEO ->dst_Element[Pos].dst_Triangle[p_RetGEO ->dst_Element[Pos].l_NbTriangles].auw_Index[1] = (unsigned short)p_stGLV->p_stFaces[Counter].Index[1];
		p_RetGEO ->dst_Element[Pos].dst_Triangle[p_RetGEO ->dst_Element[Pos].l_NbTriangles].auw_Index[2] = (unsigned short)p_stGLV->p_stFaces[Counter].Index[2];

		p_RetGEO ->dst_Element[Pos].dst_Triangle[p_RetGEO ->dst_Element[Pos].l_NbTriangles].auw_UV[0] = (unsigned short)p_stGLV->p_stFaces[Counter].UVIndex[0];
		p_RetGEO ->dst_Element[Pos].dst_Triangle[p_RetGEO ->dst_Element[Pos].l_NbTriangles].auw_UV[1] = (unsigned short)p_stGLV->p_stFaces[Counter].UVIndex[1];
		p_RetGEO ->dst_Element[Pos].dst_Triangle[p_RetGEO ->dst_Element[Pos].l_NbTriangles].auw_UV[2] = (unsigned short)p_stGLV->p_stFaces[Counter].UVIndex[2];
		p_RetGEO ->dst_Element[Pos].l_NbTriangles++;

		p_RetGEO ->dst_PointNormal[p_stGLV->p_stFaces[Counter].Index[0]] = p_stGLV->p_stFaces[Counter].OriginalPointsNormales[0];
		p_RetGEO ->dst_PointNormal[p_stGLV->p_stFaces[Counter].Index[1]] = p_stGLV->p_stFaces[Counter].OriginalPointsNormales[1];
		p_RetGEO ->dst_PointNormal[p_stGLV->p_stFaces[Counter].Index[2]] = p_stGLV->p_stFaces[Counter].OriginalPointsNormales[2];
	}

	Counter = p_stGLV->ulNumberOfPoints;
	while (Counter--)
	{
		MATH_TransformVector(&p_RetGEO ->dst_PointNormal[Counter], &stMatrixIV , &p_RetGEO ->dst_PointNormal[Counter]);
	}

	GLV_FREE(p_NumFacePElement);
	GLV_FREE(p_ElementMID);
	GLV_DelCluster(p_stCluster1);
	GLV_Destroy(p_stGLV2);
	GLV_Destroy(p_stGLV);

	return p_RetGEO;
}
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif

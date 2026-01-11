/* GEO_LIGHTCUT_RT.c */

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Light Cut Ray-tracing module 
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
#include "GDInterface/GDIrequest.h"
#include "SELection/SELection.h"
#include "GRObject/GROstruct.h"
#include "GRObject/GROedit.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOobjectaccess.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_LIGHTCUT.h"

#include <math.h>
#include <STDLIB.H>
#include "../dlls/MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "../dlls/MAD_mem/Sources/MAD_mem.h"
#include "GEOmetric/GEO_LODCmpt.h"
/* Globals var for recusrsive funcitons */
static tdst_GLV				*p_stCurGLV;
static MATHD_tdst_Vector	stgsCurBoxMin;
static MATHD_tdst_Vector	stgsCurBoxMax;
static ULONG				CurMAXDepth;
static ULONG				CurMAXFaces;
static float fSPP;
static float fSPI;
static ULONG (*SeprogressPosREC) (float F01, char *);
static MATHD_tdst_Vector stCache[64];
static ULONG ulCachePOS;
//#define GLV_USE_OC3

/* ================================================================================================================ */
/*												ok3 functions														*/
/* ================================================================================================================ */


ULONG GLV_ClipList(MATHD_tdst_Vector **p_bDst , MATHD_tdst_Vector **p_bSrc , ULONG Number , tdst_GLV_Plane *p_Clipping)
{
	ULONG Counter,Flags,ReturnVal;
	GLV_Scalar Result,Resultm1;
	MATHD_tdst_Vector **p_bSrcP, **p_bSrcPm1;
	Counter = Number;
	p_bSrcP = p_bSrc;
	p_bSrcPm1 = p_bSrc + Number - 1;
	Resultm1 = MATHD_f_DotProduct(&p_Clipping->Normale , *p_bSrcPm1) - p_Clipping->fDistanceTo0;
	Flags = 0; 
	if (Resultm1 <= GLF_PrecMetric)
		Flags = 1;
	ReturnVal = 0;
	while (Counter--)
	{
		Result = MATHD_f_DotProduct(&p_Clipping->Normale , *p_bSrcP) - p_Clipping->fDistanceTo0;
		if (Result <= GLF_PrecMetric) 
			Flags |= 2;
		switch (Flags & 3)
		{
		case 0: /* All is out */
			break;
		case 1: /* Go out */
			p_bDst[ReturnVal++] = *p_bSrcPm1;
		case 2: /* Go in */
			p_bDst[ReturnVal] = &stCache[ulCachePOS++];
			MATHD_BlendVector(p_bDst[ReturnVal++] , *p_bSrcPm1 , *p_bSrcP , -Resultm1 / (Result - Resultm1));
			break;
		case 3: /* All is in */
			p_bDst[ReturnVal++] = *p_bSrcPm1;
			break;
		}
		Flags >>= 1;
		p_bSrcPm1 = p_bSrcP++;
		Resultm1 = Result;
	}
	return ReturnVal;
}
ULONG GLV_IsPlaneTouchBox(tdst_GLV_Plane	 *Plane, MATHD_tdst_Vector    *p_bMin,MATHD_tdst_Vector    *p_bMax)
{
	MATHD_tdst_Vector UP,Down;
	UP.x = Plane ->Normale.x > 0.0f ? p_bMax->x : p_bMin->x;
	UP.y = Plane ->Normale.y > 0.0f ? p_bMax->y : p_bMin->y;
	UP.z = Plane ->Normale.z > 0.0f ? p_bMax->z : p_bMin->z;
	if (MATHD_f_DotProduct(&UP , &Plane ->Normale) < Plane ->fDistanceTo0 - GLF_PrecMetric) 
		return 0;
	Down.x = Plane ->Normale.x < 0.0f ? p_bMax->x : p_bMin->x;
	Down.y = Plane ->Normale.y < 0.0f ? p_bMax->y : p_bMin->y;
	Down.z = Plane ->Normale.z < 0.0f ? p_bMax->z : p_bMin->z;
	if (MATHD_f_DotProduct(&Down , &Plane ->Normale) > Plane ->fDistanceTo0 + GLF_PrecMetric) 
		return 0;
	return 1;

}
ULONG GLV_IsTriangleTouchBox(ULONG ulFI, MATHD_tdst_Vector    *p_bMin,MATHD_tdst_Vector    *p_bMax)
{
	ULONG Flag;
	MATHD_tdst_Vector  stP , stP2;
	float fExpand;
	fExpand = 0.0f;
/*	if (!GLV_IsPlaneTouchBox(&p_stCurGLV->p_stFaces[ulFI].Plane, p_bMin,p_bMax)) 
		return 0;*/
	GLV_MinMaxFace	(p_stCurGLV , ulFI , &stP , &stP2);
	stP2.x += fExpand;
	stP2.y += fExpand;
	stP2.z += fExpand;
	stP.x -= fExpand;
	stP.y -= fExpand;
	stP.z -= fExpand;
	if (!GLV_IsBoxTouchBox(&stP , &stP2,p_bMin, p_bMax)) 
		return 0;

	if (GLV_IsPointInBox(p_bMin , p_bMax, &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[0]].P3D)) return 1;
	if (GLV_IsPointInBox(p_bMin , p_bMax, &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[1]].P3D)) return 1;
	if (GLV_IsPointInBox(p_bMin , p_bMax, &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[2]].P3D)) return 1;
//*/
	{
		MATHD_tdst_Vector *S1[16] , *S2[16];
		tdst_GLV_Plane stClipping;
		ulCachePOS = 0;
		S1[0] = &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[0]].P3D;
		S1[1] = &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[1]].P3D;
		S1[2] = &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[2]].P3D;
		MATHD_InitVector(&stClipping.Normale ,-1.0f , 0.0f , 0.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S2 , S1 , 3    , &stClipping))) return 0;
		MATHD_InitVector(&stClipping.Normale , 1.0f , 0.0f , 0.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
		MATHD_InitVector(&stClipping.Normale , 0.0f ,-1.0f , 0.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S2 , S1 , Flag , &stClipping))) return 0;
		MATHD_InitVector(&stClipping.Normale , 0.0f , 1.0f , 0.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
		MATHD_InitVector(&stClipping.Normale , 0.0f , 0.0f ,-1.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S2 , S1 , Flag , &stClipping))) return 0;
		MATHD_InitVector(&stClipping.Normale , 0.0f , 0.0f , 1.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
	}
	return 1;
}
ULONG GLV_IsTriangleTouchBox_MinMax(ULONG ulFI, MATHD_tdst_Vector    *p_bMin,MATHD_tdst_Vector    *p_bMax, MATHD_tdst_Vector    *p_bMinDest,MATHD_tdst_Vector    *p_bMaxDest)
{
	ULONG Flag;
	MATHD_tdst_Vector  stP , stP2;
	MATHD_tdst_Vector *S1[16] , *S2[16];
	tdst_GLV_Plane stClipping;
	float fExpand;
	ulCachePOS = 0;
	fExpand = 0.0f;
/*	if (!GLV_IsPlaneTouchBox(&p_stCurGLV->p_stFaces[ulFI].Plane, p_bMin,p_bMax)) 
		return 0;*/
	GLV_MinMaxFace	(p_stCurGLV , ulFI , &stP , &stP2);
	stP2.x += fExpand;
	stP2.y += fExpand;
	stP2.z += fExpand;
	stP.x -= fExpand;
	stP.y -= fExpand;
	stP.z -= fExpand;
	if (!GLV_IsBoxTouchBox(&stP , &stP2,p_bMin, p_bMax)) 
		return 0;
/*
	if (GLV_IsPointInBox(p_bMin , p_bMax, &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[0]].P3D)) return 1;
	if (GLV_IsPointInBox(p_bMin , p_bMax, &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[1]].P3D)) return 1;
	if (GLV_IsPointInBox(p_bMin , p_bMax, &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[2]].P3D)) return 1;
//*/
	{
		S1[0] = &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[0]].P3D;
		S1[1] = &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[1]].P3D;
		S1[2] = &p_stCurGLV ->p_stPoints[p_stCurGLV ->p_stFaces[ulFI].Index[2]].P3D;
		MATHD_InitVector(&stClipping.Normale ,-1.0f , 0.0f , 0.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S2 , S1 , 3    , &stClipping))) return 0;
		MATHD_InitVector(&stClipping.Normale , 1.0f , 0.0f , 0.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
		MATHD_InitVector(&stClipping.Normale , 0.0f ,-1.0f , 0.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S2 , S1 , Flag , &stClipping))) return 0;
		MATHD_InitVector(&stClipping.Normale , 0.0f , 1.0f , 0.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
		MATHD_InitVector(&stClipping.Normale , 0.0f , 0.0f ,-1.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S2 , S1 , Flag , &stClipping))) return 0;
		MATHD_InitVector(&stClipping.Normale , 0.0f , 0.0f , 1.0f);
		stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale) + fExpand;
		if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
	}
	while (Flag--)
	{
		if (S1[Flag]->x > p_bMaxDest->x) p_bMaxDest->x = S1[Flag]->x;
		if (S1[Flag]->y > p_bMaxDest->y) p_bMaxDest->y = S1[Flag]->y;
		if (S1[Flag]->z > p_bMaxDest->z) p_bMaxDest->z = S1[Flag]->z;
		if (S1[Flag]->x < p_bMinDest->x) p_bMinDest->x = S1[Flag]->x;
		if (S1[Flag]->y < p_bMinDest->y) p_bMinDest->y = S1[Flag]->y;
		if (S1[Flag]->z < p_bMinDest->z) p_bMinDest->z = S1[Flag]->z;
	}
	return 1;
}
void GLV_MinMaxEdge(MATHD_tdst_Vector    *p_bA , MATHD_tdst_Vector    *p_bB ,MATHD_tdst_Vector    *pstMin , MATHD_tdst_Vector    *pstMax)
{
	if (p_bA->x < p_bB->x ) 
	{ 
		pstMin->x = p_bA->x;
		pstMax->x = p_bB->x;
	} else
	{ 
		pstMin->x = p_bB->x;
		pstMax->x = p_bA->x;
	} 

	if (p_bA->y < p_bB->y ) 
	{ 
		pstMin->y = p_bA->y;
		pstMax->y = p_bB->y;
	} else
	{ 
		pstMin->y = p_bB->y;
		pstMax->y = p_bA->y;
	} 

	if (p_bA->z < p_bB->z ) 
	{ 
		pstMin->z = p_bA->z;
		pstMax->z = p_bB->z;
	} else
	{ 
		pstMin->z = p_bB->z;
		pstMax->z = p_bA->z;
	} 
}

ULONG GLV_IsLineTouchBox(MATHD_tdst_Vector    *p_bMin,MATHD_tdst_Vector    *p_bMax, MATHD_tdst_Vector    *p_bA,MATHD_tdst_Vector    *p_bB)
{
	ULONG Flag;
	MATHD_tdst_Vector *S1[16] , *S2[16];
	tdst_GLV_Plane stClipping;
	MATHD_tdst_Vector  stP , stP2;
	GLV_MinMaxEdge(p_bA , p_bB , &stP , &stP2);
	if (!GLV_IsBoxTouchBox(&stP , &stP2,p_bMin, p_bMax)) 
		return 0;//*/

	if (GLV_IsPointInBox(p_bMin , p_bMax, p_bA )) return 1;
	if (GLV_IsPointInBox(p_bMin , p_bMax, p_bB )) return 1;

	ulCachePOS = 0;

	S1[0] = p_bA;
	S1[1] = p_bB;
	S1[2] = p_bA;
	MATHD_InitVector(&stClipping.Normale ,-1.0f , 0.0f , 0.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S2 , S1 , 3    , &stClipping))) return 0;
	MATHD_InitVector(&stClipping.Normale , 1.0f , 0.0f , 0.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
	MATHD_InitVector(&stClipping.Normale , 0.0f ,-1.0f , 0.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S2 , S1 , Flag , &stClipping))) return 0;
	MATHD_InitVector(&stClipping.Normale , 0.0f , 1.0f , 0.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
	MATHD_InitVector(&stClipping.Normale , 0.0f , 0.0f ,-1.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S2 , S1 , Flag , &stClipping))) return 0;
	MATHD_InitVector(&stClipping.Normale , 0.0f , 0.0f , 1.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
	return 1;
}


ULONG GLV_IsLineTouchBox_MinMax(MATHD_tdst_Vector    *p_bMin,MATHD_tdst_Vector    *p_bMax, MATHD_tdst_Vector    *p_bA,MATHD_tdst_Vector    *p_bB,MATHD_tdst_Vector    *p_bMinDest,MATHD_tdst_Vector    *p_bMaxDest)
{
	ULONG Flag;
	MATHD_tdst_Vector *S1[16] , *S2[16];
	tdst_GLV_Plane stClipping;
	MATHD_tdst_Vector  stP , stP2;
	GLV_MinMaxEdge(p_bA , p_bB , &stP , &stP2);
	if (!GLV_IsBoxTouchBox(&stP , &stP2,p_bMin, p_bMax)) 
		return 0;//*/
/*
	if (GLV_IsPointInBox(p_bMin , p_bMax, p_bA )) return 1;
	if (GLV_IsPointInBox(p_bMin , p_bMax, p_bB )) return 1;
*/
	ulCachePOS = 0;

	S1[0] = p_bA;
	S1[1] = p_bB;
	S1[2] = p_bA;
	MATHD_InitVector(&stClipping.Normale ,-1.0f , 0.0f , 0.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S2 , S1 , 3    , &stClipping))) return 0;
	MATHD_InitVector(&stClipping.Normale , 1.0f , 0.0f , 0.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
	MATHD_InitVector(&stClipping.Normale , 0.0f ,-1.0f , 0.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S2 , S1 , Flag , &stClipping))) return 0;
	MATHD_InitVector(&stClipping.Normale , 0.0f , 1.0f , 0.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
	MATHD_InitVector(&stClipping.Normale , 0.0f , 0.0f ,-1.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMin,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S2 , S1 , Flag , &stClipping))) return 0;
	MATHD_InitVector(&stClipping.Normale , 0.0f , 0.0f , 1.0f);
	stClipping.fDistanceTo0 = MATHD_f_DotProduct(p_bMax,&stClipping.Normale);
	if (!(Flag = GLV_ClipList(S1 , S2 , Flag , &stClipping))) return 0;
	while (Flag--)
	{
		if (S1[Flag]->x > p_bMaxDest->x) p_bMaxDest->x = S1[Flag]->x;
		if (S1[Flag]->y > p_bMaxDest->y) p_bMaxDest->y = S1[Flag]->y;
		if (S1[Flag]->z > p_bMaxDest->z) p_bMaxDest->z = S1[Flag]->z;
		if (S1[Flag]->x < p_bMinDest->x) p_bMinDest->x = S1[Flag]->x;
		if (S1[Flag]->y < p_bMinDest->y) p_bMinDest->y = S1[Flag]->y;
		if (S1[Flag]->z < p_bMinDest->z) p_bMinDest->z = S1[Flag]->z;
	}

	return 1;
}

void GLV_Octree_Destroy(GLV_OCtreeNode *p_Octree )
{
	ULONG Counter;
	/* Else create 8 new octrees */
	for (Counter = 0 ; Counter < 8 ; Counter++)
	{
		if (p_Octree->p_SubOcTree[Counter]) 
		{
			GLV_Octree_Destroy(p_Octree->p_SubOcTree[Counter] );
			GLV_FREE(p_Octree->p_SubOcTree[Counter]);
		}
	}
	if (p_Octree->p_stThisCLT)
		GLV_DelCluster(p_Octree->p_stThisCLT);
}

void GLV_Octree_Expand_REC(GLV_OCtreeNode *p_Octree , float fExpand)
{

	GLV_ExpandBox( &p_Octree->stAxisMin, &p_Octree->stAxisMax, fExpand);

	if (p_Octree->p_SubOcTree[0])
	{
		ULONG Counter ;
		Counter = p_Octree->ulNumberOfSubNodes;
		while (Counter--)
			GLV_Octree_Expand_REC(p_Octree->p_SubOcTree[Counter] , fExpand);
	}
}	

void GLV_Octree_DiveAndCompute_REC(GLV_OCtreeNode *p_Octree , ULONG MAXDepth , ULONG MAXFaces)
{
	ULONG Counter,ulNumberOf3;
	MATHD_tdst_Vector    stHalfSize ;

	GLV_ExpandBox( &p_Octree->stAxisMin, &p_Octree->stAxisMax, GLF_PrecMetric);

	ulNumberOf3 = 0;
	if (p_Octree->p_OctreeFather)
	{
		MATHD_tdst_Vector stMinDest,stMaxDest;
		p_Octree->p_stThisCLT = GLV_NewCluster();
		stMinDest = p_Octree->stAxisMax;
		stMaxDest = p_Octree->stAxisMin;
		GLV_Enum_Cluster_Value(p_Octree ->p_OctreeFather->p_stThisCLT);
		if (GLV_IsTriangleTouchBox_MinMax(MCL_Value , &p_Octree->stAxisMin,&p_Octree->stAxisMax,&stMinDest,&stMaxDest))
		{
			GLV_Clst_ADDV(p_Octree->p_stThisCLT, MCL_Value);
			ulNumberOf3++;
		}
		GLV_Enum_Cluster_Value_End();
		if (ulNumberOf3)
		{
			GLV_MinMaxEdge(&stMinDest, &stMaxDest,&p_Octree->stAxisMin, &p_Octree->stAxisMax);
		}//*/
		GLV_ExpandBox( &p_Octree->stAxisMin, &p_Octree->stAxisMax, GLF_PrecMetric);
	} else
	{
		GLV_Enum_Cluster_Value(p_Octree ->p_stThisCLT);
		ulNumberOf3++;
		GLV_Enum_Cluster_Value_End();
	}
	if ((ulNumberOf3 < MAXFaces ) || (!MAXDepth))
	{
		fSPI += fSPP;
		SeprogressPosREC(fSPI , "Compute OK3" );
		return;
	}
	/* Else create 8 new octrees */
	MATHD_SubVector(&stHalfSize , &p_Octree ->stAxisMax , &p_Octree ->stAxisMin);
	MATHD_ScaleEqualVector(&stHalfSize , 0.5f);
	for (Counter = 0 ; Counter < 8 ; Counter++)
	{
		p_Octree->ulNumberOfSubNodes = 8;
#ifdef JADEFUSION
		p_Octree->p_SubOcTree[Counter] = (GLV_OCtreeNode_*)GLV_ALLOC(sizeof (GLV_OCtreeNode));
#else
		p_Octree->p_SubOcTree[Counter] = GLV_ALLOC(sizeof (GLV_OCtreeNode));
#endif
		L_memset(p_Octree->p_SubOcTree[Counter] , 0 , sizeof (GLV_OCtreeNode));
		p_Octree->p_SubOcTree[Counter]->p_OctreeFather = p_Octree;
		p_Octree->p_SubOcTree[Counter]->stAxisMin = p_Octree->stAxisMin;
		if (Counter & 1) p_Octree->p_SubOcTree[Counter]->stAxisMin.x += stHalfSize.x;
		if (Counter & 2) p_Octree->p_SubOcTree[Counter]->stAxisMin.y += stHalfSize.y;
		if (Counter & 4) p_Octree->p_SubOcTree[Counter]->stAxisMin.z += stHalfSize.z;
		MATHD_AddVector(&p_Octree->p_SubOcTree[Counter]->stAxisMax , &p_Octree->p_SubOcTree[Counter]->stAxisMin , &stHalfSize);
		fSPP /= 8.0f;
		GLV_Octree_DiveAndCompute_REC(p_Octree->p_SubOcTree[Counter] , MAXDepth-1 , MAXFaces);
		fSPP *= 8.0f;
	}
/*	GLV_DelCluster(p_Octree->p_stThisCLT);
	p_Octree->p_stThisCLT = NULL;*/
}	

void GLV_Octree_DiveAndCompute_for_edges_REC(GLV_OCtreeNode *p_Octree , ULONG MAXDepth , ULONG MAXEdges)
{
	ULONG Counter,ulNumberOf3;
	MATHD_tdst_Vector    stHalfSize ;
	GLV_ExpandBox( &p_Octree->stAxisMin, &p_Octree->stAxisMax, GLF_PrecMetric);

	ulNumberOf3 = 0;
	if (p_Octree->p_OctreeFather)
	{
		MATHD_tdst_Vector stMinDest,stMaxDest;
		stMinDest = p_Octree->stAxisMax;
		stMaxDest = p_Octree->stAxisMin;

		p_Octree->p_stThisCLT = GLV_NewCluster();
		GLV_Enum_Cluster_Value(p_Octree ->p_OctreeFather->p_stThisCLT);
		if (GLV_IsLineTouchBox_MinMax(&p_Octree->stAxisMin,&p_Octree->stAxisMax, &p_stCurGLV->p_stPoints[p_stCurGLV->p_stEdges[MCL_Value].Index[0]].P3D,&p_stCurGLV->p_stPoints[p_stCurGLV->p_stEdges[MCL_Value].Index[1]].P3D,&stMinDest, &stMaxDest))
		{
			GLV_Clst_ADDV(p_Octree->p_stThisCLT, MCL_Value);
			ulNumberOf3++;
		}
		GLV_Enum_Cluster_Value_End();
		if (ulNumberOf3)
		{
			GLV_MinMaxEdge(&stMinDest, &stMaxDest,&p_Octree->stAxisMin, &p_Octree->stAxisMax);
		}//*/
		GLV_ExpandBox( &p_Octree->stAxisMin, &p_Octree->stAxisMax, GLF_PrecMetric);
	} else
	{
		GLV_Enum_Cluster_Value(p_Octree ->p_stThisCLT);
		ulNumberOf3++;
		GLV_Enum_Cluster_Value_End();
	}
	if ((ulNumberOf3 < MAXEdges) || (!MAXDepth) )
	{
		fSPI += fSPP;
		SeprogressPosREC(fSPI , "Compute OK3 for EDGES" );
		return;
	}
	/* Else create 8 new octrees */
	MATHD_SubVector(&stHalfSize , &p_Octree ->stAxisMax , &p_Octree ->stAxisMin);
	MATHD_ScaleEqualVector(&stHalfSize , 0.5f);
	for (Counter = 0 ; Counter < 8 ; Counter++)
	{
		p_Octree->ulNumberOfSubNodes = 8;
#ifdef JADEFUSION
		p_Octree->p_SubOcTree[Counter] = (GLV_OCtreeNode_*)GLV_ALLOC(sizeof (GLV_OCtreeNode));
#else
		p_Octree->p_SubOcTree[Counter] = GLV_ALLOC(sizeof (GLV_OCtreeNode));
#endif
		L_memset(p_Octree->p_SubOcTree[Counter] , 0 , sizeof (GLV_OCtreeNode));
		p_Octree->p_SubOcTree[Counter]->p_OctreeFather = p_Octree;
		p_Octree->p_SubOcTree[Counter]->stAxisMin = p_Octree->stAxisMin;
		if (Counter & 1) p_Octree->p_SubOcTree[Counter]->stAxisMin.x += stHalfSize.x;
		if (Counter & 2) p_Octree->p_SubOcTree[Counter]->stAxisMin.y += stHalfSize.y;
		if (Counter & 4) p_Octree->p_SubOcTree[Counter]->stAxisMin.z += stHalfSize.z;
		MATHD_AddVector(&p_Octree->p_SubOcTree[Counter]->stAxisMax , &p_Octree->p_SubOcTree[Counter]->stAxisMin , &stHalfSize);
		fSPP /= 8.0f;
		GLV_Octree_DiveAndCompute_for_edges_REC( p_Octree->p_SubOcTree[Counter] , MAXDepth-1 , MAXEdges);
		fSPP *= 8.0f;
	}
	GLV_DelCluster(p_Octree->p_stThisCLT);
	p_Octree->p_stThisCLT = NULL;
}	

ULONG GLV_Octree_GetNOL(GLV_OCtreeNode *p_Octree )
{
	if (p_Octree->p_stThisCLT) 
		return p_Octree->p_stThisCLT->ulNumFLGS;
	else 	if (p_Octree->p_SubOcTree[0])
	{
		ULONG Counter , retCv;
		retCv = 0;
		for (Counter = 0 ; Counter < p_Octree->ulNumberOfSubNodes ; Counter++)
		{
			retCv += GLV_Octree_GetNOL(p_Octree->p_SubOcTree[Counter]);
		}
		return retCv ;
	}
	return 0;
}


ULONG GLV_Octree_GetNumberOfNodes(GLV_OCtreeNode *p_Octree )
{
	ULONG Ret;
	Ret = 1;
	if (p_Octree->p_SubOcTree[0])
	{
		ULONG Counter ;
		for (Counter = 0 ; Counter < p_Octree->ulNumberOfSubNodes ; Counter++)
		{
			Ret += GLV_Octree_GetNumberOfNodes(p_Octree->p_SubOcTree[Counter]);
		}
	}
	return Ret;
}

void GLV_Octree_Optimize_REC(GLV_OCtreeNode *p_Octree )
{
	if (p_Octree->p_SubOcTree[0])
	{
		ULONG Counter,CounterREAL;
		CounterREAL = 0;
		for (Counter = 0 ; Counter < p_Octree->ulNumberOfSubNodes ; Counter++)
		{
			GLV_Octree_Optimize_REC(p_Octree->p_SubOcTree[Counter]);
			if ((p_Octree->p_SubOcTree[Counter]) && (!GLV_Octree_GetNOL(p_Octree->p_SubOcTree[Counter])))
			{
				GLV_Octree_Destroy(p_Octree->p_SubOcTree[Counter]);
				p_Octree->p_SubOcTree[Counter] = NULL;
			}
			else p_Octree->p_SubOcTree[CounterREAL ++] = p_Octree->p_SubOcTree[Counter];
		}
		p_Octree->ulNumberOfSubNodes = CounterREAL ;
		while (CounterREAL < 8)
		{
			p_Octree->p_SubOcTree[CounterREAL ++] = NULL;
		}
	}
}

void GLV_Octree_DiveAndCompute(tdst_GLV *p_stGLV , GLV_OCtreeNode *p_Octree , ULONG MAXDepth , ULONG MAXFaces, ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG ulNumberOf3;
	ulNumberOf3 = 0;
	fSPP = 1.0f;
	fSPI = 0.0f;
	SeprogressPosREC = SeprogressPos;

	p_stCurGLV = p_stGLV;

	ulNumberOf3 = GLV_ulGetClusterNumber(p_Octree ->p_stThisCLT);

	if (ulNumberOf3 < GLV_OC3_SQUARE_TOLERANCE)
		GLV_Octree_DiveAndCompute_REC( p_Octree , 0 , MAXFaces);	
	else
		GLV_Octree_DiveAndCompute_REC( p_Octree , MAXDepth , MAXFaces);

	{
		ULONG Bef,AFT;
		Bef = GLV_Octree_GetNumberOfNodes(	p_Octree );
		GLV_Octree_Optimize_REC( p_Octree );
		AFT = GLV_Octree_GetNumberOfNodes(	p_Octree );
		AFT =  AFT ;
	}
}	


void GLV_Octree_DiveAndCompute_for_edges(tdst_GLV *p_stGLV , GLV_OCtreeNode *p_Octree , ULONG MAXDepth , ULONG MAXEdges, ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG ulNumberOf3;
	fSPP = 1.0f;
	fSPI = 0.0f;
	SeprogressPosREC = SeprogressPos;
	ulNumberOf3 = 0;
	p_stCurGLV = p_stGLV;
	GLV_Enum_Cluster_Value(p_Octree ->p_stThisCLT);
		ulNumberOf3++;
	GLV_Enum_Cluster_Value_End();
	if (ulNumberOf3 < GLV_OC3_SQUARE_TOLERANCE_EDGES)
		GLV_Octree_DiveAndCompute_for_edges_REC( p_Octree , 0 , MAXEdges);	
	else
		GLV_Octree_DiveAndCompute_for_edges_REC( p_Octree , MAXDepth , MAXEdges);
	
	{
		ULONG Bef,AFT;
		Bef = GLV_Octree_GetNumberOfNodes(	p_Octree );
		GLV_Octree_Optimize_REC( p_Octree );
		AFT = GLV_Octree_GetNumberOfNodes(	p_Octree );
		AFT =  AFT ;
	}
}	

static GLV_Cluster *p_ClusterToFill;
static MATHD_tdst_Vector *p_Line1 , *p_Line2;
void GLV_Octree_DiveAndIntesectLine_REC( GLV_OCtreeNode *p_Octree )
{
	if ((p_Octree->p_stThisCLT) && (p_Octree->p_stThisCLT->ulNumFLGS))
	{
		if (!GLV_IsLineTouchBox( &p_Octree ->stAxisMin , &p_Octree->stAxisMax , p_Line1 , p_Line2)) return;

		if (p_Octree->p_SubOcTree[0])
		{
			ULONG Counter ;
			Counter = p_Octree->ulNumberOfSubNodes ;
			while (Counter--)
				GLV_Octree_DiveAndIntesectLine_REC( p_Octree->p_SubOcTree[Counter] );
		}
		else
			GLV_Clst_OR( p_ClusterToFill , p_Octree ->p_stThisCLT);
	}
}

void GLV_Octree_DiveAndIntesectTriangle( GLV_OCtreeNode *p_Octree , ULONG Triangle , GLV_Cluster *p_stCluster )
{
	if (p_Octree->p_SubOcTree[0])
	{
		ULONG Counter ;
		if ((p_Octree->p_stThisCLT) && (p_Octree->p_stThisCLT->ulNumFLGS))
		{
			if (!GLV_Clst_IsExist(p_Octree->p_stThisCLT , Triangle)) return;
		}
		Counter = p_Octree->ulNumberOfSubNodes;
		while (Counter--)
			GLV_Octree_DiveAndIntesectTriangle( p_Octree->p_SubOcTree[Counter] , Triangle , p_stCluster );
	} else
	if ((p_Octree->p_stThisCLT) && (p_Octree->p_stThisCLT->ulNumFLGS))
	{
		if (GLV_Clst_IsExist(p_Octree->p_stThisCLT , Triangle))
			GLV_Clst_OR( p_stCluster , p_Octree ->p_stThisCLT);
	}
}

void GLV_Octree_DiveAndIntesectTriangle_On_Line_OK3_REC( GLV_OCtreeNode *p_Octree , ULONG Triangle , GLV_Cluster *p_stCluster )
{
	if (p_Octree->p_SubOcTree[0])
	{
		ULONG Counter ;
		if (!GLV_IsTriangleTouchBox(Triangle , &p_Octree->stAxisMin,&p_Octree->stAxisMax))
			return;
		Counter = p_Octree->ulNumberOfSubNodes;
		while (Counter--)
			GLV_Octree_DiveAndIntesectTriangle_On_Line_OK3_REC( p_Octree->p_SubOcTree[Counter] , Triangle , p_stCluster );
	}
	if ((p_Octree->p_stThisCLT) && (p_Octree->p_stThisCLT->ulNumFLGS))
	{
		if (GLV_IsTriangleTouchBox(Triangle , &p_Octree->stAxisMin,&p_Octree->stAxisMax))
			GLV_Clst_OR( p_stCluster , p_Octree ->p_stThisCLT);
	}
}

void GLV_Octree_DiveAndIntesectTriangle_On_Line_OK3( GLV_OCtreeNode *p_Octree , tdst_GLV	*p_stGLV , ULONG Triangle , GLV_Cluster *p_stCluster )
{
	p_stCurGLV = p_stGLV;
	GLV_Octree_DiveAndIntesectTriangle_On_Line_OK3_REC( p_Octree , Triangle , p_stCluster );
}


void GLV_Octree_Draw_REC(GLV_OCtreeNode *p_Octree , GDI_tdst_DisplayData *_pst_DD , ULONG Color)
{
	if (p_Octree->p_SubOcTree[0])
	{
		ULONG Counter ;
		Counter = p_Octree->ulNumberOfSubNodes;
		while (Counter--)
			GLV_Octree_Draw_REC(p_Octree->p_SubOcTree[Counter] , _pst_DD , 0x808000);
	}else
	if ((p_Octree->p_stThisCLT) && (p_Octree->p_stThisCLT->ulNumFLGS))
	{
		MATH_tdst_Vector *Quad[2],VE[2],CENTER;
		MATH_tdst_Vector stMin,stMax;
		ULONG Counter,Counter2;
		_pst_DD->ul_ColorConstant = Color;
		MATHD_To_MATH(&stMin , &p_Octree->stAxisMin);
		MATHD_To_MATH(&stMax , &p_Octree->stAxisMax);
		MATH_AddVector(&CENTER , &stMin,&stMax);
		MATH_ScaleEqualVector(&CENTER , 0.5f);
		Quad[0] = &VE[0];
		Quad[1] = &VE[1];
		for (Counter2 = 0 ; Counter2 < 3 ; Counter2++)
		{
			((float *)&VE[0])[Counter2] = ((float *)&stMin)[Counter2];
			((float *)&VE[1])[Counter2] = ((float *)&stMax)[Counter2];
			for (Counter = 0 ; Counter < 4 ; Counter++)
			{
				if (Counter & 1)
					((float *)&VE[1])[(Counter2 + 1) % 3] = ((float *)&VE[0])[(Counter2 + 1) % 3] = ((float *)&stMin)[(Counter2 + 1) % 3];
				else
					((float *)&VE[1])[(Counter2 + 1) % 3] = ((float *)&VE[0])[(Counter2 + 1) % 3] = ((float *)&stMax)[(Counter2 + 1) % 3];
				if (Counter & 2)
					((float *)&VE[1])[(Counter2 + 2) % 3] = ((float *)&VE[0])[(Counter2 + 2) % 3] = ((float *)&stMin)[(Counter2 + 2) % 3];
				else
					((float *)&VE[1])[(Counter2 + 2) % 3] = ((float *)&VE[0])[(Counter2 + 2) % 3] = ((float *)&stMax)[(Counter2 + 2) % 3];
				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
			}
		}
	}
}

void GLV_Octree_Draw(GLV_OCtreeNode *p_Octree , GDI_tdst_DisplayData *_pst_DD)
{
	GLV_Octree_Draw_REC(p_Octree , _pst_DD , 0x0000ff);
}


void GLV_Octree_DiveAndIntesectLine( GLV_OCtreeNode *p_Octree , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , GLV_Cluster *p_stCluster)
{
	p_Line1 = p_P1;
	p_Line2 = p_P2;
	p_ClusterToFill = p_stCluster;
	GLV_Octree_DiveAndIntesectLine_REC( p_Octree );
}

#endif

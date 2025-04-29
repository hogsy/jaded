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

ULONG GLV_IsHide(tdst_GLV_Face *p_F , tdst_GLV_Face *p_FH)
{
	if (p_F -> ulKey == p_FH -> ulKey) return 0;

	if (p_FH ->ulFlags & (GLV_FLGS_Transparent | GLV_FLGS_Transparent2)) return 0;

	if (p_F -> ulFlags & GLV_FLGS_Kilt)
	{
//		if (!(p_F -> ulFlags & GLV_FLGS_WhiteFront))
		{
			if ( p_FH -> ulFlags & GLV_FLGS_Kilt )
				if ( p_FH -> ulFlags & GLV_FLGS_WhiteFront ) return 0;
		}
		/* only if from the same light */
		if ((p_FH ->ulFlags & GLV_FLGS_Kilt) && (p_F->ulMARK != p_FH -> ulMARK)) return 0;
		/* all the rest is hide */
		return 1;
	}
	/* Original face hide everything */
	if (p_F ->ulFlags & GLV_FLGS_Original)
	{
		/* Originals is hiden by everything */
		return 1;
	}
	return 0;
}
void GLV_GetTriRayPoint(tdst_GLV *p_stGLV , ULONG C1, MATHD_tdst_Vector *p_PR )
{
	MATHD_AddVector(p_PR , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[1]].P3D);
	MATHD_AddVector(p_PR , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[2]].P3D , p_PR);
	MATHD_ScaleVector(p_PR , p_PR , 1.0f / 3.0f );
	MATHD_AddScaleVector(p_PR , p_PR , &p_stGLV->p_stFaces[C1].Plane.Normale , GLF_FaceExtraction );
}
void GLV_GetTriRayPointAndLightCenter(tdst_GLV *p_stGLV , ULONG C1, ULONG LightNum , MATHD_tdst_Vector *p_PR , MATHD_tdst_Vector *p_LC )
{
	MATHD_tdst_Vector ST;
	GLV_GetTriRayPoint(p_stGLV , C1, p_PR );
	GLV_RT_ComputeLightCenter(p_stGLV, LightNum , p_PR , p_LC);
	MATHD_SubVector(&ST , p_LC , p_PR);
	MATHD_SetNormVector(&ST , &ST , GLF_FaceExtraction * 10.0f);
	MATHD_AddVector(p_PR , p_PR , &ST);
}

void GLV_GetEdgeRayPointAndLightCenter(tdst_GLV *p_stGLV , ULONG C1, ULONG LightNum , MATHD_tdst_Vector *p_PR , MATHD_tdst_Vector *p_LC )
{
	MATHD_tdst_Vector ST;
	MATHD_AddVector(p_PR , &p_stGLV->p_stPoints[p_stGLV->p_stEdges[C1].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stEdges[C1].Index[1]].P3D);
	MATHD_ScaleEqualVector(p_PR , 0.5f);
	if ((p_stGLV->p_stEdges[C1].FromFaces[0] | p_stGLV->p_stEdges[C1].FromFaces[1]) != 0xffffffff)
	{
		MATHD_AddVector(&ST , &p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[0]].Plane.Normale , &p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[1]].Plane.Normale );
		MATHD_SetNormVector(&ST , &ST ,GLF_FaceExtraction * 10.0f);
		MATHD_AddVector(p_PR , p_PR , &ST );
	}
	GLV_RT_ComputeLightCenter(p_stGLV, LightNum , p_PR , p_LC);
	MATHD_SubVector(&ST , p_LC , p_PR);
	MATHD_SetNormVector(&ST , &ST , GLF_FaceExtraction * 10.0f);
	MATHD_AddVector(p_PR , p_PR , &ST);
}

ULONG GLV_RT_GetIsHitChannel(tdst_GLV *p_stGLV , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , tdst_GLV_Face *p_F , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill , ULONG Channel)
{
	GLV_ResetCluster(p_ClusterToFill);
	GLV_Octree_DiveAndIntesectLine( p_stOctreeROOT , p_P1 , p_P2 , p_ClusterToFill);
	GLV_Enum_Cluster_Value(p_ClusterToFill)
		if (Channel & (p_stGLV ->p_stFaces + MCL_Value)->ulChannel)
		{
		if (GLV_IsHide(p_F , p_stGLV ->p_stFaces + MCL_Value))
			if (GLV_IsEdgeTouchFace	(p_stGLV , MCL_Value , NULL , p_P1 , p_P2 , NULL))
			{
				return (MCL_Value | 0x80000000);
			}
		}
	GLV_Enum_Cluster_Value_End();
	return 0;//*/
}
ULONG GLV_RT_GetIsHit(tdst_GLV *p_stGLV , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , tdst_GLV_Face *p_F , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill )
{
	GLV_ResetCluster(p_ClusterToFill);
	GLV_Octree_DiveAndIntesectLine( p_stOctreeROOT , p_P1 , p_P2 , p_ClusterToFill);
	GLV_Enum_Cluster_Value(p_ClusterToFill)
		if (GLV_IsHide(p_F , p_stGLV ->p_stFaces + MCL_Value))
			if (GLV_IsEdgeTouchFace	(p_stGLV , MCL_Value , NULL , p_P1 , p_P2 , NULL))
			{
				return (MCL_Value | 0x80000000);
			}
			GLV_Enum_Cluster_Value_End();
			return 0;//*/
}
ULONG GLV_RT_GetIsHitAll(tdst_GLV *p_stGLV , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , tdst_GLV_Face *p_F , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill , GLV_Cluster *p_ClusterResult)
{
	ULONG RetValue;
	RetValue = 0;
	GLV_ResetCluster(p_ClusterResult);
	GLV_ResetCluster(p_ClusterToFill);
	GLV_Octree_DiveAndIntesectLine( p_stOctreeROOT , p_P1 , p_P2 , p_ClusterToFill);
	GLV_Enum_Cluster_Value(p_ClusterToFill)
		if (p_F->ulChannel & (p_stGLV ->p_stFaces + MCL_Value)->ulChannel)
		{
			if (GLV_IsHide(p_F , p_stGLV ->p_stFaces + MCL_Value))
				if (GLV_IsEdgeTouchFace	(p_stGLV , MCL_Value , NULL , p_P1 , p_P2 , NULL))
				{
					GLV_Clst_ADDV(p_ClusterResult, MCL_Value);
					RetValue = 1;
				}
		}
	GLV_Enum_Cluster_Value_End();
	return RetValue;//*/
}

ULONG GLV_RT_GetIsHitNearest(tdst_GLV *p_stGLV , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , tdst_GLV_Face *p_F , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill , ULONG *FaceResult)
{
	ULONG RetValue;
	GLV_Scalar Distance,LocalDistance;
	RetValue = 0;
	Distance = 1000000000000000000000.0;
	GLV_ResetCluster(p_ClusterToFill);
	GLV_Octree_DiveAndIntesectLine( p_stOctreeROOT , p_P1 , p_P2 , p_ClusterToFill);
	GLV_Enum_Cluster_Value(p_ClusterToFill)
		if (p_F->ulChannel & (p_stGLV ->p_stFaces + MCL_Value)->ulChannel)
		{
			if (GLV_IsHide(p_F , p_stGLV ->p_stFaces + MCL_Value))
				if (GLV_IsEdgeTouchFace	(p_stGLV , MCL_Value , NULL , p_P1 , p_P2 , &LocalDistance))
				{
					if (LocalDistance < Distance)
					{
						Distance = LocalDistance;
						*FaceResult = MCL_Value;
					}
					RetValue = 1;
				}
		}
	GLV_Enum_Cluster_Value_End();
	return RetValue;//*/
}
ULONG GLV_RT_LightGeomRemove(tdst_GLV *p_stGLV , MATHD_tdst_Vector *stDirection, ULONG LightNum)
{
	GLV_Scalar D,L;
	switch(p_stGLV->p_Lights[LightNum].ulLightFlags & LIGHT_Cul_LF_Type)
	{
	case LIGHT_Cul_LF_Direct:
		break;
	case LIGHT_Cul_LF_Omni:
		if (MATHD_f_NormVector(stDirection) > p_stGLV->p_Lights[LightNum].fLightFar) return 1;
		break;
	case LIGHT_Cul_LF_Spot:
		L = MATHD_f_NormVector(stDirection);
		if (L > p_stGLV->p_Lights[LightNum].fLightFar) return 1;
		D = MATHD_f_DotProduct ( stDirection , &p_stGLV->p_Lights[LightNum].LDir);
		if ( D < 0.0f) return 1;
		if (D / L < p_stGLV->p_Lights[LightNum].fCosBigAlpha) return 1;
		break;
	}
	return 0;
	
}
void GLV_RT_ComputeLightCenter(tdst_GLV *p_stGLV, ULONG LightNum , MATHD_tdst_Vector  *pstTriCenter , MATHD_tdst_Vector  *pstLightCenter)
{
	switch(p_stGLV->p_Lights[LightNum].ulLightFlags & LIGHT_Cul_LF_Type)
	{
	case LIGHT_Cul_LF_Direct:
		{
			GLV_Scalar Dist;
			Dist = MATHD_f_DotProduct(pstTriCenter , &p_stGLV->p_Lights[LightNum].LDir) - p_stGLV->p_Lights[LightNum].fLightNear;
			MATHD_AddScaleVector(pstLightCenter , pstTriCenter , &p_stGLV->p_Lights[LightNum].LDir , -Dist);
		}
		break;
	case LIGHT_Cul_LF_Omni:
	case LIGHT_Cul_LF_Spot:
		*pstLightCenter = p_stGLV->p_Lights[LightNum].stLightCenterOS;
		break;
	}
}
ULONG GLV_RemoveOverlapedEdges(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *) , GLV_OCtreeNode *p_stOctreeROOT)
{
	ULONG C1;
	GLV_Cluster *p_ClusterToFill;
//	ULONG *p_PointCount;
	char Text[256];


	p_ClusterToFill = GLV_NewCluster();
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0);
	GLD_RemoveBadEdges( p_stGLV );
	GLD_RemoveDoubleEdges( p_stGLV );
	GLD_RemoveUnusedIndexes( p_stGLV );
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0);
	snprintf( Text, sizeof( Text ), "Culling %u Edges...", p_stGLV->ulNumberOfEdges );
	
	
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfEdges ; C1 ++) p_stGLV->p_stEdges[C1].ulFlags &= ~GLV_FLGS_DeleteIt;
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfEdges ; C1 ++)
	{
		ULONG XXXX;
		if (SeprogressPos((float)C1 / (float)p_stGLV->ulNumberOfEdges , Text)) return 1;
		XXXX = 0xffffffff;
		if ((p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[0]].ulFlags & p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[1]].ulFlags) & GLV_FLGS_Original) 
		{
			continue;
		}
		if ((p_stGLV->p_stEdges[C1].FromFaces[1] | p_stGLV->p_stEdges[C1].FromFaces[0]) == 0xffffffff) 
		{
			p_stGLV->p_stEdges[C1].ulFlags |= GLV_FLGS_DeleteIt;
			continue;
		}
		if (!((p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[0]].ulFlags | p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[1]].ulFlags) & GLV_FLGS_Original)) 
		{
			p_stGLV->p_stEdges[C1].ulFlags |= GLV_FLGS_DeleteIt;
			continue;
		}
		if (p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[0]].ulFlags & GLV_FLGS_Kilt) XXXX = 0;
		else
		{
			if (p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[1]].ulFlags & GLV_FLGS_Kilt) XXXX = 1;
		}
		if (XXXX != 0xffffffff)
		{
			MATHD_tdst_Vector stCenter,stDistance,stLightCenter;
			ULONG SF,lightChannel;
			lightChannel = p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[XXXX]].ulChannel;
			lightChannel &= p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[XXXX ^ 1]].ulChannel;
			SF = p_stGLV->p_stEdges[C1].ulFlags;
			GLV_GetEdgeRayPointAndLightCenter(p_stGLV, C1 , p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[XXXX]].ulMARK , &stCenter , &stLightCenter);	
			MATHD_SubVector(&stDistance , &stLightCenter , &stCenter );
			if (GLV_RT_LightGeomRemove(p_stGLV , &stDistance , p_stGLV->p_stFaces[p_stGLV->p_stEdges[C1].FromFaces[XXXX]].ulMARK))
				SF |= GLV_FLGS_DeleteIt;
			else
				if (GLV_RT_GetIsHitChannel(p_stGLV , &stCenter , &stLightCenter , p_stGLV->p_stFaces + p_stGLV->p_stEdges[C1].FromFaces[XXXX] , p_stOctreeROOT , p_ClusterToFill , lightChannel))
					SF |= GLV_FLGS_DeleteIt;
			p_stGLV->p_stEdges[C1].ulFlags = SF;
		}
	}

	/* Following thing will avoid some T jonctions */
//	p_PointCount = GLV_ALLOC(4L * p_stGLV ->ulNumberOfPoints);
/*	{
		ulong C2;
		for (C2 = 0 ; C2 < 4 ; C2 ++)
		{
			L_memset(p_PointCount , 0 , 4L * p_stGLV ->ulNumberOfPoints);
			for (C1 = 0 ; C1 < p_stGLV->ulNumberOfEdges ; C1 ++) 
			{
				if (!(p_stGLV->p_stEdges[C1].ulFlags & GLV_FLGS_DeleteIt))
				{
					p_PointCount[p_stGLV->p_stEdges[C1].Index[0]]++;
					p_PointCount[p_stGLV->p_stEdges[C1].Index[1]]++;
				}
			}
			
			for (C1 = 0 ; C1 < p_stGLV->ulNumberOfEdges ; C1 ++) 
			{
				if ((p_PointCount[p_stGLV->p_stEdges[C1].Index[0]] == 1) ||
					(p_PointCount[p_stGLV->p_stEdges[C1].Index[1]] == 1))
				{
					p_stGLV->p_stEdges[C1].ulFlags &= ~GLV_FLGS_DeleteIt;
				}
			}
		}
	}//*/


	GLV_DelCluster(p_ClusterToFill);

	GLD_Remove2BD(p_stGLV);//*/
//	GLV_FREE(p_PointCount);
	return 0;
	
}
ULONG GLV_RemoveOverLap(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG C1;
	char Text[256];
	GLV_OCtreeNode stOctreeROOT;
	GLV_Cluster *p_ClusterToFill;
#ifndef GLV_OVERLAPP
	return 0;
#endif
	
	//	GLD_RemoveIllegalFaces  ( p_stGLV );
	GLD_RemoveUnusedIndexes ( p_stGLV );
	
#ifdef GLV_USE_OK3
	L_memset(&stOctreeROOT , 0 , sizeof (GLV_OCtreeNode));
	stOctreeROOT.p_OctreeFather = NULL;
	stOctreeROOT.p_stThisCLT = GLV_NewCluster();
	C1 = p_stGLV->ulNumberOfFaces;
	while (C1--) GLV_Clst_ADDV(stOctreeROOT.p_stThisCLT , C1);
	GLV_ComputeGLVBox(p_stGLV , &stOctreeROOT.stAxisMin, &stOctreeROOT.stAxisMax , GLV_FLGS_Original);
	SeprogressPos(0 , "Compute OK3") ;
	GLV_Octree_DiveAndCompute( p_stGLV , &stOctreeROOT , GLV_OC3_MAX_DEPTH , GLV_OC3_MAX_FPO,SeprogressPos);
	p_ClusterToFill = GLV_NewCluster();
#endif
	
	sprintf (Text , "Culling on %d faces", p_stGLV->ulNumberOfFaces);
	
	
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++) p_stGLV->p_stFaces[C1].ulFlags &= ~GLV_FLGS_DeleteIt;
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
	{
		if (SeprogressPos((float)C1 / (float)p_stGLV->ulNumberOfFaces , Text)) return 1;
		if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Kilt)
		{
			MATHD_tdst_Vector stCenter,stDistance,stLightCenter;
			ULONG SF;
			SF = p_stGLV->p_stFaces[C1].ulFlags;
			GLV_GetTriRayPointAndLightCenter(p_stGLV , C1, p_stGLV->p_stFaces[C1].ulMARK , &stCenter , &stLightCenter);
			/*				GLV_GetTriRayPoint(p_stGLV , C1, &stCenter );
			GLV_RT_ComputeLightCenter(p_stGLV, p_stGLV->p_stFaces[C1].ulMARK , &stCenter , &stLightCenter);*/
			
			MATHD_SubVector(&stDistance , &stLightCenter , &stCenter );
			if (GLV_RT_LightGeomRemove(p_stGLV , &stDistance , p_stGLV->p_stFaces[C1].ulMARK))
				SF |= GLV_FLGS_DeleteIt;
			else
				if (GLV_RT_GetIsHit(p_stGLV , &stCenter , &stLightCenter , p_stGLV->p_stFaces + C1 , &stOctreeROOT , p_ClusterToFill ))
					SF |= GLV_FLGS_DeleteIt;
				p_stGLV->p_stFaces[C1].ulFlags = SF;
		} 
	}
	
	
#ifdef GLV_USE_OK3
	GLV_DelCluster(p_ClusterToFill);
	GLV_Octree_Destroy( &stOctreeROOT );
#endif
	
	GLD_Remove2BD(p_stGLV);//*/
	return 0;
}

void GLV_RemoveOverLap_LOCAL(tdst_GLV *p_stGLVSrc , tdst_GLV *p_stGLV_LOCAL , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill )
{
	ULONG C1;
	for (C1 = 0 ; C1 < p_stGLV_LOCAL->ulNumberOfFaces ; C1 ++)
		if (!(p_stGLV_LOCAL->p_stFaces[C1].ulFlags & GLV_FLGS_DeleteIt))
		{
			if (p_stGLV_LOCAL->p_stFaces[C1].ulFlags & GLV_FLGS_Kilt)
			{
				MATHD_tdst_Vector stCenter,stDistance , stLightCenter ;
				ULONG SF;
				SF = p_stGLV_LOCAL->p_stFaces[C1].ulFlags;
				GLV_GetTriRayPointAndLightCenter(p_stGLV_LOCAL , C1, p_stGLV_LOCAL->p_stFaces[C1].ulMARK , &stCenter , &stLightCenter);
				/*				GLV_GetTriRayPoint(p_stGLV_LOCAL , C1, &stCenter );
				GLV_RT_ComputeLightCenter(p_stGLV_LOCAL, p_stGLV_LOCAL->p_stFaces[C1].ulMARK , &stCenter , &stLightCenter);*/
				MATHD_SubVector(&stDistance , &stLightCenter , &stCenter );
				if (GLV_RT_LightGeomRemove(p_stGLV_LOCAL , &stDistance , p_stGLV_LOCAL->p_stFaces[C1].ulMARK))
					SF |= GLV_FLGS_DeleteIt;
				else
					if (GLV_RT_GetIsHit(p_stGLVSrc , &stCenter , &stLightCenter , p_stGLV_LOCAL->p_stFaces  + C1 , p_stOctreeROOT , p_ClusterToFill ))
						SF |= GLV_FLGS_DeleteIt;
					
					p_stGLV_LOCAL->p_stFaces[C1].ulFlags = SF;
			} 
		}
}

#endif

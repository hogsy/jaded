/* GEO_LIGHTCUT_TOOLS.c */

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

#include <math.h>
#include <STDLIB.H>
#include "../dlls/MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "../dlls/MAD_mem/Sources/MAD_mem.h"
#include "GEOmetric/GEO_LODCmpt.h"
/* ================================================================================================================ */
/*											clusters functions														*/
/* ================================================================================================================ */
GLV_Cluster *p_FirstFreeCluster = NULL;
ULONG *GLV_FlagMem = NULL;

#ifdef CLUSTER_MODE_SIMPLE

void GLV_Clst_SetNumbers_0(GLV_Cluster *p_Dst , ULONG Numbers)
{
	Numbers += ((8 << GLV_CLUSTER_256_KEY));
	Numbers &= ~((8 << GLV_CLUSTER_256_KEY) - 1);
	if (p_Dst->ulNumFLGS < Numbers)
	{
		if		(!p_Dst->p_AllFLGS_256) p_Dst->p_AllFLGS_256 = (ULONG **)GLV_ALLOC((4L * Numbers)>>(GLV_CLUSTER_256_KEY));
		else	p_Dst->p_AllFLGS_256 = (ULONG **)GLV_REALLOC(p_Dst->p_AllFLGS_256 ,(4L * Numbers)>>(GLV_CLUSTER_256_KEY));
		while (p_Dst->ulNumFLGS < Numbers)
		{
			p_Dst->p_AllFLGS_256[p_Dst->ulNumFLGS >> GLV_CLUSTER_256_KEY] = 0;
			p_Dst->ulNumFLGS += 1 << GLV_CLUSTER_256_KEY;
		}
		p_Dst->ulNumFLGS = Numbers;
	}
}

ULONG *GLV_GetFlags()
{
	ULONG *F;
	if (GLV_FlagMem)
	{
		F = GLV_FlagMem;
		GLV_FlagMem = (ULONG *)*GLV_FlagMem;
	} else
	{
		F = (ULONG *)GLV_ALLOC(4 + ((4 << GLV_CLUSTER_256_KEY) >> 5));
	}
	L_memset(F , 0 , 4 + ((4 << GLV_CLUSTER_256_KEY) >> 5));
	return F + 1;
}

void GLV_ReleaseFlags(ULONG *F)
{
	F -= 1;
	*F = (ULONG)GLV_FlagMem;
	GLV_FlagMem = F;
}
#ifdef JADEFUSION
void GLV_FreeFlagMem(void)
{
    ULONG* pNext   = GLV_FlagMem;
    ULONG* pToFree = NULL;

    while (pNext != NULL)
    {
        pToFree = pNext;
        pNext   = (ULONG*)(*pNext);

        GLV_FREE(pToFree);
    }

    GLV_FlagMem = NULL;
}
#endif

ULONG GLV_Clst_IsExist(GLV_Cluster *p_Dst , ULONG Value)
{
	if (Value < p_Dst->ulNumFLGS) 
	{
		ULONG *FlagT;
		FlagT = p_Dst->p_AllFLGS_256[Value >> GLV_CLUSTER_256_KEY];
		if (FlagT)
		{
			Value &= (1 << GLV_CLUSTER_256_KEY) - 1;
			return FlagT[Value >> 5] & (1 << (Value & 31));
		}
	}
	return 0;
}

void GLV_Clst_ADDV(GLV_Cluster *p_Dst , ULONG Value)
{
	ULONG *FlagT;
	if (Value >= p_Dst->ulNumFLGS) GLV_Clst_SetNumbers_0(p_Dst , Value);

	FlagT = p_Dst->p_AllFLGS_256[Value >> GLV_CLUSTER_256_KEY];
	if (!FlagT) FlagT = p_Dst->p_AllFLGS_256[Value >> GLV_CLUSTER_256_KEY] = GLV_GetFlags();

	Value &= (1 << GLV_CLUSTER_256_KEY) - 1;
	FlagT[Value >> 5] |= (1 << (Value & 31));
}



GLV_Cluster *GLV_NewCluster()
{
	GLV_Cluster *NC;
	if (p_FirstFreeCluster)
	{
		NC = p_FirstFreeCluster;
		p_FirstFreeCluster = NC->p_NextFreeCluster;
	} else
	{
		NC = (GLV_Cluster *)GLV_ALLOC(sizeof(GLV_Cluster));
	}
	L_memset(NC , 0 , sizeof(GLV_Cluster));
	return NC;
}

void GLV_ResetCluster(GLV_Cluster *pCC)
{																		
	ULONG **p_CurFlags256,**p_CurFlags256Last;									
	p_CurFlags256 = pCC->p_AllFLGS_256;	
	p_CurFlags256Last = p_CurFlags256 + (pCC->ulNumFLGS >> GLV_CLUSTER_256_KEY);
	while (p_CurFlags256 < p_CurFlags256Last)									
	{																	
		if (*(p_CurFlags256))										
		{		
			GLV_ReleaseFlags(*(p_CurFlags256));
			*(p_CurFlags256) = NULL;
		} 
		p_CurFlags256++;
	}
}																		
void GLV_DelCluster(GLV_Cluster *CC)
{
	GLV_ResetCluster(CC);
	if (CC->p_AllFLGS_256) GLV_FREE(CC->p_AllFLGS_256);
	CC->p_NextFreeCluster = p_FirstFreeCluster;
	p_FirstFreeCluster = CC;
}
#ifdef JADEFUSION
void GLV_FreeClusterMem(void)
{
    GLV_Cluster* pCluster = p_FirstFreeCluster;
    GLV_Cluster* pNext;

    while (pCluster != NULL)
    {
        pNext = pCluster->p_NextFreeCluster;

        GLV_FREE(pCluster);

        pCluster = pNext;
    }

    p_FirstFreeCluster = NULL;
}
#endif

void GLV_Clst_OR(GLV_Cluster *p_Dst , GLV_Cluster *p_Src)
{
	ULONG **p_SrcFlags256,**p_SrcFlags256Last, **p_DstFlags256;
	if (p_Dst->ulNumFLGS < p_Src->ulNumFLGS) GLV_Clst_SetNumbers_0(p_Dst , p_Src->ulNumFLGS);

	p_SrcFlags256		= p_Src->p_AllFLGS_256;
	p_SrcFlags256Last	= p_SrcFlags256 + (p_Src->ulNumFLGS >> GLV_CLUSTER_256_KEY);
	p_DstFlags256		= p_Dst->p_AllFLGS_256;

	while (p_SrcFlags256 < p_SrcFlags256Last)
	{
		if (*p_SrcFlags256)
		{
			ULONG *SrcFlg,*DstFlg,*SrcFLGLast;
			if (!*p_DstFlags256) 
			{
				*p_DstFlags256 = GLV_GetFlags();
			}
			SrcFlg = *p_SrcFlags256 ;
			SrcFLGLast = SrcFlg + ((1 << GLV_CLUSTER_256_KEY) >> 5);
			DstFlg = *p_DstFlags256 ;
			while (SrcFlg < SrcFLGLast)
			{
				*(DstFlg++) |= *(SrcFlg++);
				*(DstFlg++) |= *(SrcFlg++);
				*(DstFlg++) |= *(SrcFlg++);
				*(DstFlg++) |= *(SrcFlg++);
			}

		}
		p_SrcFlags256++;
		p_DstFlags256++;
	}
}

#else
ULONG GLV_Clst_GetAFlag(GLV_Cluster *p_Dst)
{
	ULONG ret , Counter;
	if (p_Dst->ulFirstVoidFLGS == -1) 
	{
		ret = p_Dst->ulNumFLGS;
		p_Dst->ulNumFLGS = ((p_Dst->ulNumFLGS >> 10L) + 1L ) << 10L;
		if (p_Dst->p_AllFLGS)
			p_Dst->p_AllFLGS = GLV_REALLOC(p_Dst->p_AllFLGS , p_Dst->ulNumFLGS << (2 + GLV_ClusterShift));
		else
			p_Dst->p_AllFLGS = GLV_ALLOC(p_Dst->ulNumFLGS << (2 + GLV_ClusterShift));
		while (ret < p_Dst->ulNumFLGS)
		{
			p_Dst->p_AllFLGS[ret] = p_Dst->ulFirstVoidFLGS;
			p_Dst->ulFirstVoidFLGS = ret;
			ret += 1 << GLV_ClusterShift;
		}
	}
	ret = p_Dst->ulFirstVoidFLGS;
	p_Dst->ulFirstVoidFLGS = p_Dst->p_AllFLGS[p_Dst->ulFirstVoidFLGS];
	Counter = 1L << GLV_ClusterShift;
	while (Counter--)
		p_Dst->p_AllFLGS[ret + Counter] = 0;
	
	return ret;
}
void GLV_Clst_ReleaseAFlag(GLV_Cluster *p_Dst , ULONG Num)
{
	p_Dst->p_AllFLGS[Num] = p_Dst->ulFirstVoidFLGS;
	p_Dst->ulFirstVoidFLGS = Num ;
}
void GLV_Clst_SetNumbers_0(GLV_Cluster *p_Dst , ULONG Numbers)
{
	ULONG SaveN;
	if (Numbers >= p_Dst->ulNumClts0)
	{
		SaveN = p_Dst->ulNumClts0;
		p_Dst->ulNumClts0 = ((Numbers >> 8L) + 1L) << 8L;
		if (p_Dst->p_HieClts0)
			p_Dst->p_HieClts0 = GLV_REALLOC(p_Dst->p_HieClts0 , p_Dst->ulNumClts0 * 4L);
		else
			p_Dst->p_HieClts0 = GLV_ALLOC(p_Dst->ulNumClts0 * 4L);
		while (SaveN < p_Dst->ulNumClts0)
		{
			p_Dst->p_HieClts0[SaveN++] = -1;
		}
	}
}

ULONG GLV_Clst_IsExist(GLV_Cluster *p_Dst , ULONG Value)
{
	if ((Value >> (5 + GLV_ClusterShift)) >= p_Dst -> ulNumClts0) return 0;
	if (p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)] == -2) return 1;
	if (p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)] == -1) return 0;
	return p_Dst->p_AllFLGS[p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)] + ((Value >> 5L) & ((1 << GLV_ClusterShift) - 1))] & (1 << (Value & 0x1f));
}



void GLV_Clst_ADDV(GLV_Cluster *p_Dst , ULONG Value)
{
	if ((Value >> (5 + GLV_ClusterShift)) >= p_Dst -> ulNumClts0) GLV_Clst_SetNumbers_0(p_Dst , Value >> (5 + GLV_ClusterShift));
	if (p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)] == -2) return;
	if (p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)] == -1) 
		p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)] = GLV_Clst_GetAFlag(p_Dst);
	p_Dst->p_AllFLGS[p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)] + ((Value >> 5L) & ((1 << GLV_ClusterShift) - 1))] |= (1L << (Value & 0x1f));

	if (p_Dst->p_AllFLGS[p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)] + ((Value >> 5L) & ((1 << GLV_ClusterShift) - 1))] == -1)
	{
		ULONG *p_First , *p_Last;
		/* detect if all flg are 1 */
		p_First = &p_Dst->p_AllFLGS[p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)]];
		p_Last  = p_First + (1L << GLV_ClusterShift);
		while (p_First < p_Last )
		{
			if (*p_First != -1) return;
			p_First ++;
		}
		GLV_Clst_ReleaseAFlag(p_Dst , p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)]);
		p_Dst->p_HieClts0[Value >> (5 + GLV_ClusterShift)] = -2;
	}
}

void GLV_Clst_OR(GLV_Cluster *p_Dst , GLV_Cluster *p_Src)
{
	ULONG Counter, Counter2 , Withnss;
	ULONG *pSrcFLG , *pDstFLG;
	GLV_Clst_SetNumbers_0(p_Dst , p_Src ->ulNumClts0);
	Counter = p_Src ->ulNumClts0;
	while (Counter--)
	{
		if (p_Dst ->p_HieClts0[Counter] == -2) continue;
		if (p_Src ->p_HieClts0[Counter] == -1) continue;
		if (p_Src ->p_HieClts0[Counter] == -2)
		{
			if (p_Dst ->p_HieClts0[Counter] != -1)
				GLV_Clst_ReleaseAFlag(p_Dst , p_Dst ->p_HieClts0[Counter]);
			p_Dst ->p_HieClts0[Counter] = -2;
			continue;
		}
		if (p_Dst ->p_HieClts0[Counter] == -1) p_Dst ->p_HieClts0[Counter] = GLV_Clst_GetAFlag(p_Dst);
		pSrcFLG = &p_Src ->p_AllFLGS[p_Src ->p_HieClts0[Counter]];
		pDstFLG = &p_Dst ->p_AllFLGS[p_Dst ->p_HieClts0[Counter]];
		Counter2 = 1L << GLV_ClusterShift;
		Withnss = -1;
		while (Counter2--)
		{
			*(pDstFLG) |= *(pSrcFLG++);
			Withnss &= *(pDstFLG++);
		}
		if (Withnss == -1)
		{
			GLV_Clst_ReleaseAFlag(p_Dst , p_Dst ->p_HieClts0[Counter]);
			p_Dst->p_HieClts0[Counter] = -2;
		}
	}
}

GLV_Cluster *GLV_NewCluster()
{
	GLV_Cluster *NC;
	NC = GLV_ALLOC(sizeof(GLV_Cluster));
	L_memset(NC , 0 , sizeof(GLV_Cluster));
	NC ->ulFirstVoidFLGS = -1;
	return NC;
}
void GLV_DelCluster(GLV_Cluster *CC)
{
	if (CC->p_AllFLGS) GLV_FREE(CC->p_AllFLGS);
	if (CC->p_HieClts0) GLV_FREE(CC->p_HieClts0);
	GLV_FREE(CC);
}
void GLV_ResetCluster(GLV_Cluster *pCC)
{
	ULONG Counter;
	Counter = pCC->ulNumClts0;
	while (Counter --)
	{
		if (pCC ->p_HieClts0[Counter] == -1) 
			continue;
		if (pCC ->p_HieClts0[Counter] != -2) 
			GLV_Clst_ReleaseAFlag(pCC , pCC ->p_HieClts0[Counter]);
		pCC ->p_HieClts0[Counter] = -1;
	}
}
#define GLV_DBG_TEST_SZE 1000000
void GLV_Clst_TST()
{
	ULONG Counter1;
	GLV_Cluster *C1 , *C2 , *C3 ;
	C1 = GLV_NewCluster();
	C2 = GLV_NewCluster();
	C3 = GLV_NewCluster();
	for (Counter1 = 0 ; Counter1 < GLV_DBG_TEST_SZE ; Counter1+= 10)
		GLV_Clst_ADDV(C1 , Counter1);
	GLV_ResetCluster(C1);
	for (Counter1 = 0 ; Counter1 < GLV_DBG_TEST_SZE ; Counter1+= 7)
		GLV_Clst_ADDV(C2 , Counter1);
	for (Counter1 = 50 ; Counter1 < 100 ; Counter1++)
		GLV_Clst_ADDV(C3 , Counter1);
	GLV_Clst_OR(C1 , C2);
	GLV_Clst_OR(C1 , C3);

	GLV_Enum_Cluster_Value(C1);
		MCL_Value = MCL_Value;
		if (!GLV_Clst_IsExist(C1 , MCL_Value))
			MCL_Value = MCL_Value;
		GLV_Clst_IsExist(C1 , MCL_Value);
	GLV_Enum_Cluster_Value_End();

	for (Counter1 = 0 ; Counter1 < GLV_DBG_TEST_SZE ; Counter1+= 10)
		GLV_Clst_ADDV(C1 , Counter1);
	for (Counter1 = 0 ; Counter1 < GLV_DBG_TEST_SZE ; Counter1+= 7)
		GLV_Clst_ADDV(C1 , Counter1);
	for (Counter1 = 50 ; Counter1 < 100 ; Counter1++)
		GLV_Clst_ADDV(C1 , Counter1);

	GLV_Enum_Cluster_Value(C1);
		MCL_Value = MCL_Value;
		if (!GLV_Clst_IsExist(C1 , MCL_Value))
			MCL_Value = MCL_Value;
		GLV_Clst_IsExist(C1 , MCL_Value);
	GLV_Enum_Cluster_Value_End();


}
#endif /*CLUSTER_MODE_SIMPLE*/

ULONG GLV_ulGetClusterNumber(GLV_Cluster *CC)
{
	ULONG Counter;
	Counter = 0;
	GLV_Enum_Cluster_Value(CC);
		Counter++;
	GLV_Enum_Cluster_Value_End();
	return Counter;
}
ULONG GLV_ulGetPos(GLV_Cluster *CC , ULONG Value)
{
	ULONG Counter;
	Counter = 0;
	GLV_Enum_Cluster_Value(CC);
		if (MCL_Value == Value) return Counter;
		Counter++;
	GLV_Enum_Cluster_Value_End();
	return 0x80000000; 
}



void GLV_Clip(tdst_GLV *p_GLV,tdst_GLV_Plane *p_Plane)
{
	GLV_Scalar *p_Result,*p_Crnt;
	ULONG ulCounter,ulCounter2,Index[3];
	tdst_GLV_Point *p_PF,*p_PL;
	tdst_GLV_Face *p_FF ;
#ifdef JADEFUSION
	p_Crnt = p_Result = (double*)GLV_ALLOC(sizeof(GLV_Scalar) * p_GLV->ulNumberOfPoints);
#else
	p_Crnt = p_Result = GLV_ALLOC(sizeof(GLV_Scalar) * p_GLV->ulNumberOfPoints);
#endif
	p_PF = p_GLV ->p_stPoints;
	p_PL = p_PF + p_GLV->ulNumberOfPoints;
	while (p_PF < p_PL) *(p_Crnt++) = MATHD_f_DotProduct(&(p_PF++)->P3D , &p_Plane->Normale) - p_Plane->fDistanceTo0;
	ulCounter = p_GLV->ulNumberOfFaces;
	while (ulCounter--)
	{
		p_FF = p_GLV ->p_stFaces + ulCounter;
		if ((p_Result[p_FF->Index[0]] <= GLF_PrecMetric) &&  (p_Result[p_FF->Index[1]] <= GLF_PrecMetric) && (p_Result[p_FF->Index[2]] <= GLF_PrecMetric))
		{
			p_FF->ulFlags |= GLV_FLGS_ClipResultBF;
			continue;
		}
		if ((p_Result[p_FF->Index[0]] >= -GLF_PrecMetric) &&  (p_Result[p_FF->Index[1]] >= -GLF_PrecMetric) && (p_Result[p_FF->Index[2]] >= -GLF_PrecMetric))
		{
			p_FF->ulFlags |= GLV_FLGS_ClipResultFF;
			continue;
		}
		/* Create 2 new triangle & 2 Points */
		GLV_SetNumbers(p_GLV , p_GLV->ulNumberOfPoints + 2, 0 , p_GLV->ulNumberOfFaces + 2 , 5);
		p_FF = p_GLV ->p_stFaces + ulCounter;
		p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-1] = *p_FF;
		p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-2] = *p_FF;
		ulCounter2  = ( ((ULONG *)&p_Result[p_FF->Index[0]])[1] ^ ((ULONG *)&p_Result[p_FF->Index[1]])[1] & 0x80000000) >> 30;
		ulCounter2 |= ( ((ULONG *)&p_Result[p_FF->Index[1]])[1] ^ ((ULONG *)&p_Result[p_FF->Index[2]])[1] & 0x80000000) >> 31;
		//*/
		if (ulCounter2 == 3) ulCounter2 = 1;
		else ulCounter2 = (ulCounter2 << 1) & 3; /* Clipping version n° 316 */
		Index[0] = p_FF->Index[ulCounter2];
		Index[1] = p_FF->Index[(ulCounter2 + 1) % 3];
		Index[2] = p_FF->Index[(ulCounter2 + 2) % 3];
		if (Index[0] < Index[1])
			MATHD_BlendVector(&p_GLV->p_stPoints[p_GLV->ulNumberOfPoints-1].P3D,&p_GLV->p_stPoints[Index[0]].P3D,&p_GLV->p_stPoints[Index[1]].P3D, p_Result[Index[0]] / (p_Result[Index[0]] - p_Result[Index[1]]));
		else
			MATHD_BlendVector(&p_GLV->p_stPoints[p_GLV->ulNumberOfPoints-1].P3D,&p_GLV->p_stPoints[Index[1]].P3D,&p_GLV->p_stPoints[Index[0]].P3D, p_Result[Index[1]] / (p_Result[Index[1]] - p_Result[Index[0]]));
		if (Index[0] < Index[2])
			MATHD_BlendVector(&p_GLV->p_stPoints[p_GLV->ulNumberOfPoints-2].P3D,&p_GLV->p_stPoints[Index[0]].P3D,&p_GLV->p_stPoints[Index[2]].P3D, p_Result[Index[0]] / (p_Result[Index[0]] - p_Result[Index[2]]));
		else
			MATHD_BlendVector(&p_GLV->p_stPoints[p_GLV->ulNumberOfPoints-2].P3D,&p_GLV->p_stPoints[Index[2]].P3D,&p_GLV->p_stPoints[Index[0]].P3D, p_Result[Index[2]] / (p_Result[Index[2]] - p_Result[Index[0]]));
		p_FF->Index[1] = p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-2].Index[2] = p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-1].Index[0] = p_GLV->ulNumberOfPoints - 1;
		p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-1].Index[1] = Index[1];
		p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-2].Index[0] = p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-1].Index[2] = Index[2];
		p_FF->Index[2] = p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-2].Index[1] = p_GLV->ulNumberOfPoints - 2;
		p_FF->Index[0] = Index[0];
		if (p_Result[p_FF->Index[0]] >= -GLF_PrecMetric)
		{
			p_FF->ulFlags |= GLV_FLGS_ClipResultFF|GLV_FLGS_Clipped;
			p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-1].ulFlags |= GLV_FLGS_ClipResultBF|GLV_FLGS_Clipped;
			p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-2].ulFlags |= GLV_FLGS_ClipResultBF|GLV_FLGS_Clipped;
		} else
		{
			p_FF->ulFlags |= GLV_FLGS_ClipResultBF|GLV_FLGS_Clipped;
			p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-1].ulFlags |= GLV_FLGS_ClipResultFF|GLV_FLGS_Clipped;
			p_GLV ->p_stFaces[p_GLV->ulNumberOfFaces-2].ulFlags |= GLV_FLGS_ClipResultFF|GLV_FLGS_Clipped;
		} 	
	}
	GLV_FREE(p_Result);
}

void GLV_MinMaxFace(tdst_GLV *p_stGLV , ULONG F1 , MATHD_tdst_Vector *p_PMIN , MATHD_tdst_Vector *p_PMAX)
{
	MATHD_tdst_Vector *p_P1 , *p_P2 , *p_P3;
	p_P1 = &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F1].Index[0]].P3D;
	p_P2 = &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F1].Index[1]].P3D;
	p_P3 = &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F1].Index[2]].P3D;

	if (p_P1 -> x < p_P2 -> x)
	{
		p_PMIN -> x = p_P1 -> x;
		p_PMAX -> x = p_P2 -> x;
	}else{
		p_PMIN -> x = p_P2 -> x;
		p_PMAX -> x = p_P1 -> x;
	}
	if (p_P1 -> y < p_P2 -> y)
	{
		p_PMIN -> y = p_P1 -> y;
		p_PMAX -> y = p_P2 -> y;
	}else{
		p_PMIN -> y = p_P2 -> y;
		p_PMAX -> y = p_P1 -> y;
	}
	if (p_P1 -> z < p_P2 -> z)
	{
		p_PMIN -> z = p_P1 -> z;
		p_PMAX -> z = p_P2 -> z;
	}else{
		p_PMIN -> z = p_P2 -> z;
		p_PMAX -> z = p_P1 -> z;
	}
	
	p_PMIN -> x = p_PMIN -> x < p_P3 -> x ? p_PMIN -> x : p_P3 -> x ;
	p_PMIN -> y = p_PMIN -> y < p_P3 -> y ? p_PMIN -> y : p_P3 -> y ;
	p_PMIN -> z = p_PMIN -> z < p_P3 -> z ? p_PMIN -> z : p_P3 -> z ;
	p_PMAX -> x = p_PMAX -> x > p_P3 -> x ? p_PMAX -> x : p_P3 -> x ;
	p_PMAX -> y = p_PMAX -> y > p_P3 -> y ? p_PMAX -> y : p_P3 -> y ;
	p_PMAX -> z = p_PMAX -> z > p_P3 -> z ? p_PMAX -> z : p_P3 -> z ;

	p_PMIN -> x -= GLF_PrecMetric;
	p_PMIN -> y -= GLF_PrecMetric;
	p_PMIN -> z -= GLF_PrecMetric;
	p_PMAX -> x += GLF_PrecMetric;
	p_PMAX -> y += GLF_PrecMetric;
	p_PMAX -> z += GLF_PrecMetric;
}

int GLV_IsBoxTouchBox(MATHD_tdst_Vector *p_PMIN1 , MATHD_tdst_Vector *p_PMAX1,MATHD_tdst_Vector *p_PMIN2 , MATHD_tdst_Vector *p_PMAX2)
{
	if (p_PMIN1->x > p_PMAX2->x) return 0;
	if (p_PMIN1->y > p_PMAX2->y) return 0;
	if (p_PMIN1->z > p_PMAX2->z) return 0;
	if (p_PMIN2->x > p_PMAX1->x) return 0;
	if (p_PMIN2->y > p_PMAX1->y) return 0;
	if (p_PMIN2->z > p_PMAX1->z) return 0;
	return 1;
}

int GLV_IsPointInBox(MATHD_tdst_Vector *p_PMIN1 , MATHD_tdst_Vector *p_PMAX1,MATHD_tdst_Vector *p_P )
{
	if (p_P->x > p_PMAX1->x) return 0;
	if (p_P->y > p_PMAX1->y) return 0;
	if (p_P->z > p_PMAX1->z) return 0;
	if (p_P->x < p_PMIN1->x) return 0;
	if (p_P->y < p_PMIN1->y) return 0;
	if (p_P->z < p_PMIN1->z) return 0;
	return 1;
}

void GLV_MARK(tdst_GLV *p_GLV , ULONG MARK , ULONG ulChannel)
{
	tdst_GLV_Face *p_FT,*p_LT;
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT) (p_FT++)->ulMARK = MARK;
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT) (p_FT++)->ulChannel = ulChannel;
	
}
void GLD_RemoveEdges2BD ( tdst_GLV * p_GLV )
{
	tdst_GLV_Edge *p_FE,*p_FE2,*p_LE;
	ULONG Counter;
	Counter = 0;
	p_FE2 = p_FE = p_GLV->p_stEdges;
	p_LE = p_FE + p_GLV->ulNumberOfEdges;
	while (p_FE < p_LE)
	{
		if (!(p_FE->ulFlags & GLV_FLGS_DeleteIt))
		{
			*(p_FE2++) = *p_FE;
			Counter++;
		} 
		p_FE++;
	}
	GLV_SetNumbers(p_GLV,0, Counter, 0,2);
}

void GLD_Remove2BD ( tdst_GLV * p_GLV )
{
	tdst_GLV_Face *p_FT,*p_FT2,*p_LT;
	ULONG *p_Reorder,ulReodr,ulIntgr;
	ULONG Counter;
	p_Reorder = (ULONG*)GLV_ALLOC(p_GLV->ulNumberOfFaces * 4L);
	L_memset(p_Reorder , 0 , p_GLV->ulNumberOfFaces * 4L); 
	ulReodr = ulIntgr = 0;
	p_FT2 = p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		for (Counter = 0 ; Counter < 3 ; Counter++)
			if (p_FT->Nghbr[Counter] < 0xff000000)
				if (p_GLV->p_stFaces[p_FT->Nghbr[Counter]].ulFlags & GLV_FLGS_DeleteIt) 
					p_FT->Nghbr[Counter] = -1;
		if (!(p_FT->ulFlags & GLV_FLGS_DeleteIt))
		{
			*(p_FT2++) = *p_FT;
			p_Reorder[ulReodr] = ulIntgr++;
		} else
		{
			if (p_FT->ulFlags & GLV_FLGS_Original)
				ulReodr = ulReodr;
			p_Reorder[ulReodr] = -1;
		}
		ulReodr++;
		p_FT++;
	}
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		for (Counter = 0 ; Counter < 3 ; Counter++)
		{
			if (p_FT->Nghbr[Counter] < 0xff000000)
				p_FT->Nghbr[Counter] = p_Reorder[p_FT->Nghbr[Counter]];
		}
		p_FT++;
	}
/*	for (Counter = 0 ; Counter < p_GLV ->ulNumberOfEdges ; Counter ++)
	{
		if (p_GLV ->p_stEdges [Counter].FromFaces [0] < p_GLV->ulNumberOfFaces)
		{
			p_GLV ->p_stEdges [Counter].FromFaces [0] = p_Reorder[p_GLV ->p_stEdges [Counter].FromFaces [0]];
		}
		if (p_GLV ->p_stEdges [Counter].FromFaces [1] < p_GLV->ulNumberOfFaces)
		{
			p_GLV ->p_stEdges [Counter].FromFaces [1] = p_Reorder[p_GLV ->p_stEdges [Counter].FromFaces [1]];
		}
	}*/
	GLV_SetNumbers(p_GLV,0, 0, ulIntgr ,4);
	GLV_FREE(p_Reorder);
	GLD_RemoveEdges2BD ( p_GLV );
}
void GLD_RemoveBadEdges( tdst_GLV * p_GLV )
{
	ULONG Counter,NewEdgeNum;
	NewEdgeNum = 0;
	for (Counter = 0 ; Counter < p_GLV->ulNumberOfEdges ; Counter++)
	{
		if (p_GLV->p_stEdges[Counter].Index[0] != p_GLV->p_stEdges[Counter].Index[1])
		{
			p_GLV->p_stEdges[NewEdgeNum++] = p_GLV->p_stEdges[Counter];
		}
	}
	GLV_SetNumbers(p_GLV , 0 , NewEdgeNum , 0 , 2);
}

int GLV_QSortEdgeClabk(const void *T1,const void *T2)
{
	if (((tdst_GLV_Edge *)T1)->Index[0] == ((tdst_GLV_Edge *)T2)->Index[0]) 
	{
		if (((tdst_GLV_Edge *)T1)->Index[1] <= ((tdst_GLV_Edge *)T2)->Index[1]) 
			return -1;
		else 
			return 1;
	}
	if (((tdst_GLV_Edge *)T1)->Index[0] < ((tdst_GLV_Edge *)T2)->Index[0]) 
		return -1;
	else 
		return 1;
}
void GLD_OptimizeEdges( tdst_GLV * p_GLV )
{
	ULONG *p_TagODrome , Counter , C2;
	ULONG *p_E1 , 	*p_E2 ;
	MATHD_tdst_Vector *p_P1 , *p_P2 ;
	p_TagODrome = (ULONG*)GLV_ALLOC(4L * p_GLV ->ulNumberOfPoints);
	L_memset(p_TagODrome , 0 , 4L * p_GLV ->ulNumberOfPoints);
	p_E1 = (ULONG*)GLV_ALLOC(4L * p_GLV ->ulNumberOfPoints);
	p_E2 = (ULONG*)GLV_ALLOC(4L * p_GLV ->ulNumberOfPoints);
	/* first detect point with only 2 Edges */
	for (Counter = 0 ; Counter < p_GLV ->ulNumberOfEdges ; Counter ++)
	{
		p_GLV ->p_stEdges[Counter].ulFlags &= ~(GLV_FLGS_RSV1|GLV_FLGS_RSV2);
		p_TagODrome[p_GLV ->p_stEdges[Counter].Index[0]] ++;
		if (p_TagODrome[p_GLV ->p_stEdges[Counter].Index[0]] == 1) p_E1[p_GLV ->p_stEdges[Counter].Index[0]] = Counter;
		if (p_TagODrome[p_GLV ->p_stEdges[Counter].Index[0]] == 2) p_E2[p_GLV ->p_stEdges[Counter].Index[0]] = Counter;
		p_TagODrome[p_GLV ->p_stEdges[Counter].Index[1]] ++;
		if (p_TagODrome[p_GLV ->p_stEdges[Counter].Index[1]] == 1) p_E1[p_GLV ->p_stEdges[Counter].Index[1]] = Counter;
		if (p_TagODrome[p_GLV ->p_stEdges[Counter].Index[1]] == 2) p_E2[p_GLV ->p_stEdges[Counter].Index[1]] = Counter;
	}
	/* Merge them if colinears */
	for (Counter = 0 ; Counter < p_GLV ->ulNumberOfPoints ; Counter ++)
	{
		if (p_TagODrome[Counter] == 2)
		{
			if (p_GLV ->p_stEdges[p_E1[Counter]].ulFlags & GLV_FLGS_RSV1) continue;
			if (p_GLV ->p_stEdges[p_E2[Counter]].ulFlags & GLV_FLGS_RSV1) continue;
			if (Counter == p_GLV ->p_stEdges[p_E1[Counter]].Index[1])
				p_P1 = &p_GLV ->p_stPoints[p_GLV ->p_stEdges[p_E1[Counter]].Index[0]] .P3D;
			else
				p_P1 = &p_GLV ->p_stPoints[p_GLV ->p_stEdges[p_E1[Counter]].Index[1]] .P3D;
				
			if (Counter == p_GLV ->p_stEdges[p_E2[Counter]].Index[1])
				p_P2 = &p_GLV ->p_stPoints[p_GLV ->p_stEdges[p_E2[Counter]].Index[0]] .P3D;
			else
				p_P2 = &p_GLV ->p_stPoints[p_GLV ->p_stEdges[p_E2[Counter]].Index[1]] .P3D;
				
			if (GLV_f_IsPointOnEdge( p_P1 , p_P2 , &p_GLV ->p_stPoints[Counter] .P3D))
			{
				/* Merge */
				ULONG I1,I2;
				if (Counter == p_GLV ->p_stEdges[p_E1[Counter]].Index[1])
					I1 = p_GLV ->p_stEdges[p_E1[Counter]].Index[0];
				else
					I1 = p_GLV ->p_stEdges[p_E1[Counter]].Index[1];
					
				if (Counter == p_GLV ->p_stEdges[p_E2[Counter]].Index[1])
					I2 = p_GLV ->p_stEdges[p_E2[Counter]].Index[0];
				else
					I2 = p_GLV ->p_stEdges[p_E2[Counter]].Index[1];
				p_GLV ->p_stEdges[p_E1[Counter]].Index[0] = I1;
				p_GLV ->p_stEdges[p_E1[Counter]].Index[1] = I2;
				p_GLV ->p_stEdges[p_E1[Counter]].ulFlags |= GLV_FLGS_RSV1;
				p_GLV ->p_stEdges[p_E2[Counter]].ulFlags |= GLV_FLGS_RSV1 | GLV_FLGS_RSV2;
			}
		}
	}
	/* Delete edges */
	C2 = 0;
	for (Counter = 0 ; Counter < p_GLV ->ulNumberOfEdges ; Counter ++)
	{
		if (!(p_GLV ->p_stEdges[Counter].ulFlags & GLV_FLGS_RSV2))
		{
			p_GLV ->p_stEdges[C2++] = p_GLV ->p_stEdges[Counter];
		}
	}
	GLV_SetNumbers(p_GLV , 0 , C2 , 0 , 2);
	GLV_FREE(p_TagODrome);
	GLV_FREE(p_E1);
	GLV_FREE(p_E2);
}
void GLD_RemoveDoubleEdges( tdst_GLV * p_GLV )
{
	ULONG Counter,NewEdgeNum;
	NewEdgeNum = 0;
	if (!p_GLV->ulNumberOfEdges) return;
	for (Counter = 0 ; Counter < p_GLV->ulNumberOfEdges ; Counter++)
	{
		if (p_GLV->p_stEdges[Counter].Index[0] > p_GLV->p_stEdges[Counter].Index[1])
		{
			NewEdgeNum = p_GLV->p_stEdges[Counter].Index[0];
			p_GLV->p_stEdges[Counter].Index[0] = p_GLV->p_stEdges[Counter].Index[1];
			p_GLV->p_stEdges[Counter].Index[1] = NewEdgeNum;
		}
	}
	qsort ((void *)p_GLV->p_stEdges,p_GLV->ulNumberOfEdges,sizeof(tdst_GLV_Edge),GLV_QSortEdgeClabk);
	NewEdgeNum = 1;
	for (Counter = 1 ; Counter < p_GLV->ulNumberOfEdges ; Counter++)
	{
		if ((p_GLV->p_stEdges[NewEdgeNum - 1].Index[0] == p_GLV->p_stEdges[Counter].Index[0]) &&
			(p_GLV->p_stEdges[NewEdgeNum - 1].Index[1] == p_GLV->p_stEdges[Counter].Index[1]))
		{
			p_GLV->p_stEdges[NewEdgeNum - 1].ulFlags |= p_GLV->p_stEdges[Counter].ulFlags | GLV_FLGS_DBG1;
			continue;
		}
		p_GLV->p_stEdges[NewEdgeNum++] = p_GLV->p_stEdges[Counter];
	}
	GLV_SetNumbers(p_GLV , 0 , NewEdgeNum , 0 , 2);
}

void GLD_RemoveUnusedIndexes( tdst_GLV * p_GLV )
{
    ULONG   Counter, ECounter,*TAGODROME;
    TAGODROME = (ULONG*)GLV_ALLOC(4L * p_GLV->ulNumberOfPoints);
    L_memset(TAGODROME, 0, 4L * p_GLV->ulNumberOfPoints);
    for(Counter = 0; Counter < p_GLV->ulNumberOfFaces; Counter++)
    {
        TAGODROME[p_GLV->p_stFaces[Counter].Index[0]] = 1;
        TAGODROME[p_GLV->p_stFaces[Counter].Index[1]] = 1;
        TAGODROME[p_GLV->p_stFaces[Counter].Index[2]] = 1;
    }

    for(Counter = 0; Counter < p_GLV->ulNumberOfEdges ; Counter++)
    {
        TAGODROME[p_GLV->p_stEdges[Counter].Index[0]] = 1;
        TAGODROME[p_GLV->p_stEdges[Counter].Index[1]] = 1;
    }

    ECounter = 0;
    for(Counter = 0; Counter < p_GLV->ulNumberOfPoints; Counter++)
    {
        if(TAGODROME[Counter])
        {
            p_GLV->p_stPoints[ECounter] = p_GLV->p_stPoints[Counter];
            TAGODROME[Counter] = ECounter++;
        }
    }

	GLV_SetNumbers(p_GLV , ECounter , 0 , 0 , 1);
    for(Counter = 0; Counter < p_GLV->ulNumberOfFaces; Counter++)
    {
        p_GLV->p_stFaces[Counter].Index[0] = TAGODROME[p_GLV->p_stFaces[Counter].Index[0]];
        p_GLV->p_stFaces[Counter].Index[1] = TAGODROME[p_GLV->p_stFaces[Counter].Index[1]];
        p_GLV->p_stFaces[Counter].Index[2] = TAGODROME[p_GLV->p_stFaces[Counter].Index[2]];
    }
    for(Counter = 0; Counter < p_GLV->ulNumberOfEdges ; Counter++)
    {
        p_GLV->p_stEdges[Counter].Index[0] = TAGODROME[p_GLV->p_stEdges[Counter].Index[0]];
        p_GLV->p_stEdges[Counter].Index[1] = TAGODROME[p_GLV->p_stEdges[Counter].Index[1]];
    }

	
    GLV_FREE(TAGODROME);
}

int GLD_IsIllegalFaces( tdst_GLV * p_GLV , tdst_GLV_Face *p_F )
{
	if (p_F->Index[0] == p_F->Index[1]) return 1;
	if (p_F->Index[1] == p_F->Index[2]) return 1;
	if (p_F->Index[2] == p_F->Index[0]) return 1;
	if (p_F->Index[0] >= p_GLV->ulNumberOfPoints ) return 1;
	if (p_F->Index[1] >= p_GLV->ulNumberOfPoints ) return 1;
	if (p_F->Index[2] >= p_GLV->ulNumberOfPoints ) return 1;
//	if (GLV_IsColinear( &p_GLV->p_stPoints[p_F->Index[0]].P3D , &p_GLV->p_stPoints[p_F->Index[1]].P3D , &p_GLV->p_stPoints[p_F->Index[2]].P3D)) return 1;
	return 0;
}

void GLD_RemoveIllegalFaces( tdst_GLV * p_GLV )
{
    ULONG   Counter;
    for(Counter = 0; Counter < p_GLV->ulNumberOfFaces; Counter++)
    {
		p_GLV->p_stFaces[Counter].ulFlags &= ~GLV_FLGS_DeleteIt;
		if (GLD_IsIllegalFaces( p_GLV , &p_GLV->p_stFaces[Counter])) p_GLV->p_stFaces[Counter].ulFlags |= GLV_FLGS_DeleteIt;
    }
	GLD_Remove2BD ( p_GLV );
}
void		GLD_RemoveZeroSurfaces( tdst_GLV * p_GLV )
{
    ULONG   Counter;
    for(Counter = 0; Counter < p_GLV->ulNumberOfFaces; Counter++)
    {
		p_GLV->p_stFaces[Counter].ulFlags &= ~GLV_FLGS_DeleteIt;
		if (GLV_GetSurf( p_GLV , &p_GLV->p_stFaces[Counter]) < GLF_PrecMetric) 
			p_GLV->p_stFaces[Counter].ulFlags |= GLV_FLGS_DeleteIt;
    }
	GLD_Remove2BD ( p_GLV );
}

static MATHD_tdst_Vector stSortVector;
int GLV_QSortClabk(const void *T1,const void *T2)
{
	if (((tdst_GLV_Point *)T1)->P3D.x == ((tdst_GLV_Point *)T2)->P3D.x) 
		return 0;
	if (((tdst_GLV_Point *)T1)->P3D.x <= ((tdst_GLV_Point *)T2)->P3D.x) 
		return -1;
	else 
		return 1;
}
void		GLV_ExpandBox( MATHD_tdst_Vector *p_PMIN , MATHD_tdst_Vector *p_PMAX , GLV_Scalar Value)
{
	p_PMIN -> x -= GLF_PrecMetric;
	p_PMIN -> y -= GLF_PrecMetric;
	p_PMIN -> z -= GLF_PrecMetric;
	p_PMAX -> x += GLF_PrecMetric;
	p_PMAX -> y += GLF_PrecMetric;
	p_PMAX -> z += GLF_PrecMetric;
}
void		GLV_ComputeGLVBox	(tdst_GLV * p_GLV ,MATHD_tdst_Vector *p_PMIN , MATHD_tdst_Vector *p_PMAX , ULONG Flags)
{
	ULONG Counter , CounterF , C2;
	MATHD_InitVector(p_PMIN , Cf_Infinit , Cf_Infinit , Cf_Infinit);
	MATHD_InitVector(p_PMAX ,-Cf_Infinit ,-Cf_Infinit ,-Cf_Infinit);
	for (CounterF=0;CounterF<p_GLV->ulNumberOfFaces ; CounterF++) 
	{
 		if (!(p_GLV->p_stFaces[CounterF].ulFlags & Flags)) continue;
		for (C2 = 0 ; C2 < 3 ; C2 ++)
		{
			Counter = p_GLV->p_stFaces[CounterF].Index[C2];
			if (p_PMIN->x > p_GLV->p_stPoints[Counter] .P3D.x) p_PMIN->x = p_GLV->p_stPoints[Counter] .P3D.x;
			if (p_PMIN->y > p_GLV->p_stPoints[Counter] .P3D.y) p_PMIN->y = p_GLV->p_stPoints[Counter] .P3D.y;
			if (p_PMIN->z > p_GLV->p_stPoints[Counter] .P3D.z) p_PMIN->z = p_GLV->p_stPoints[Counter] .P3D.z;
			if (p_PMAX->x < p_GLV->p_stPoints[Counter] .P3D.x) p_PMAX->x = p_GLV->p_stPoints[Counter] .P3D.x;
			if (p_PMAX->y < p_GLV->p_stPoints[Counter] .P3D.y) p_PMAX->y = p_GLV->p_stPoints[Counter] .P3D.y;
			if (p_PMAX->z < p_GLV->p_stPoints[Counter] .P3D.z) p_PMAX->z = p_GLV->p_stPoints[Counter] .P3D.z;
		}
	}
	GLV_ExpandBox( p_PMIN , p_PMAX , GLF_PrecMetric);
}
ULONG GLV_EqualColor(ULONG A , ULONG B )
{
	GLV_Scalar Color1[4],Color2[4] , Res;
/*	if (!((A | B) & 0xf0f0f0f0))
		return 1;*/
	GLV_ULto4Scalar(A, Color1);
	GLV_ULto4Scalar(B, Color2);
	Color1[0] -= Color2[0];
	Color1[1] -= Color2[1];
	Color1[2] -= Color2[2];
	Color1[3] -= Color2[3];
	Res = Color1[0] * Color1[0] + Color1[1] * Color1[1] + Color1[2] * Color1[2] + Color1[3] * Color1[3];
	if (Res < 25.0)
		return 1;
	else
		return 0;
}

LONG GLV_Inf(ULONG A , ULONG B )
{
	GLV_Scalar Color1[4],Color2[4] , Res;
/*	if (!((A | B) & 0xf0f0f0f0))
		return 1;*/
	GLV_ULto4Scalar(A, Color1);
	GLV_ULto4Scalar(B, Color2);
	Color1[0] -= Color2[0];
	Color1[1] -= Color2[1];
	Color1[2] -= Color2[2];
	Color1[3] -= Color2[3];
	Res = Color1[0]+ Color1[1]  + Color1[2] + Color1[3];
	if (Res*Res < 5.0)
		return 0;
	else
		if (Res < 0.0)
			return -1;
		else
			return 1;
}
ULONG GLV_OriginalPointAreCompatible( tdst_GLV_Point *p_1 , tdst_GLV_Point *p_2 )
{
	if (p_1->ulFlags != p_2->ulFlags) return 0;
	if (p_1->ulSurfaceOwner != p_2->ulSurfaceOwner) return 0;
	if (MATHD_f_DotProduct(&p_1->UpPoint , &p_2->UpPoint) < 0.95f) return 0;
	return GLV_EqualColor(p_1->ulColor , p_2->ulColor);
}

ULONG GLV_OriginalPointAreCompatible_UV( tdst_GLV_Point *p_1 , tdst_GLV_Point *p_2 )
{
	if (p_1->ulFlags != p_2->ulFlags) return 0;
	if (p_1->stUVInfo.MaterialNum != p_2->stUVInfo.MaterialNum) return 0;
	if (p_1->ulSurfaceOwner != p_2->ulSurfaceOwner) return 0;
	if (fAbs(p_1->stUVInfo.UV[0] - p_2->stUVInfo.UV[0]) > GLF_PrecMetric_UV ) return 0;
	if (fAbs(p_1->stUVInfo.UV[1] - p_2->stUVInfo.UV[1]) > GLF_PrecMetric_UV ) return 0;
	if (MATHD_f_DotProduct(&p_1->UpPoint , &p_2->UpPoint) < 0.95f) return 0;//*/
	return GLV_EqualColor(p_1->ulColor , p_2->ulColor);
}
ULONG GLV_OriginalPointAreCompatible_Overlap( tdst_GLV_Point *p_1 , tdst_GLV_Point *p_2 )
{
	if (p_1->ulNumberOfPlanes == 1)
	{
		if (MATHD_f_DotProduct(&p_1->GLV_Point_Plane[0].Normale,&p_2->GLV_Point_Plane[0].Normale) < 0.999)
//		if (!GLV_IsEqualPlanes(&p_1->GLV_Point_Plane[0],&p_2->GLV_Point_Plane[0]))
		{
			p_1->GLV_Point_Plane[1] = p_2->GLV_Point_Plane[0];
			p_1->ulNumberOfPlanes++;
		} else
			if (p_1->ulNumberOfPlanes == 2)
			{
				if (MATHD_f_DotProduct(&p_1->GLV_Point_Plane[0].Normale,&p_2->GLV_Point_Plane[1].Normale) < 0.999)
		//		if (!GLV_IsEqualPlanes(&p_1->GLV_Point_Plane[0],&p_2->GLV_Point_Plane[1]))
				{
					p_1->GLV_Point_Plane[1] = p_2->GLV_Point_Plane[1];
					p_1->ulNumberOfPlanes++;
				} 
			}
	}
	p_1->ulSurfaceOwner += p_2->ulSurfaceOwner;
	return 1;
}


static float fNormalWeldThresh = 0.0f;
/* 
This function will collapse nearest point with the specified threshold 
if ulMode = 0 it will collapse all point 
if ulMode = 1 it will collapse all point with near normal(point.uppoint) & near color (point.ulcolor)
if ulMode = 2 it will collapse all point with near normal(fNormalWeldThresh)
if ulMode = 2 it will collapse all point with Color, UV & elementjade 
*/
void GLD_SortOverX( tdst_GLV * p_GLV )
{
	ULONG Counter,*pRedir;
	GLV_Verify(p_GLV);
	if (!p_GLV->ulNumberOfPoints) return;
	pRedir		 = (ULONG*)GLV_ALLOC(4L * p_GLV->ulNumberOfPoints);
	for (Counter=0;Counter<p_GLV->ulNumberOfPoints;Counter++) 
		p_GLV->p_stPoints[Counter] . ulRefIndex = Counter;
	MATHD_InitVector(&stSortVector , 1.0 , 1.0 , 1.0 );
	qsort ((void *)p_GLV->p_stPoints,p_GLV->ulNumberOfPoints,sizeof(tdst_GLV_Point),GLV_QSortClabk);
	for (Counter=0;Counter<p_GLV->ulNumberOfPoints;Counter++) 
		pRedir[p_GLV->p_stPoints[Counter] . ulRefIndex] = Counter;
	for (Counter=0;Counter<p_GLV->ulNumberOfFaces;Counter++)
	{
		p_GLV->p_stFaces[Counter].Index[0] = pRedir[p_GLV->p_stFaces[Counter].Index[0]];
		p_GLV->p_stFaces[Counter].Index[1] = pRedir[p_GLV->p_stFaces[Counter].Index[1]];
		p_GLV->p_stFaces[Counter].Index[2] = pRedir[p_GLV->p_stFaces[Counter].Index[2]];
	}
	for (Counter=0;Counter<p_GLV->ulNumberOfEdges;Counter++)
	{
		p_GLV->p_stEdges[Counter].Index[0] = pRedir[p_GLV->p_stEdges[Counter].Index[0]];
		p_GLV->p_stEdges[Counter].Index[1] = pRedir[p_GLV->p_stEdges[Counter].Index[1]];
	}
	GLV_FREE(pRedir);
}

void GLD_Weld( tdst_GLV * p_GLV , GLV_Scalar Thresh , ULONG ulMode)
{
	ULONG Counter,Counter2,*pRedir;
	MATHD_tdst_Vector fDist;
	GLV_Scalar fNSpr;
	GLV_Verify(p_GLV);
	GLD_RemoveUnusedIndexes(p_GLV);

	Thresh = (GLV_Scalar)sqrt(Thresh);
    pRedir = (ULONG*)GLV_ALLOC(4L * p_GLV->ulNumberOfPoints);
	if (!p_GLV->ulNumberOfPoints) return;
	for (Counter=0;Counter<p_GLV->ulNumberOfPoints;Counter++) 
		p_GLV->p_stPoints[Counter] . ulRefIndex = Counter;
	MATHD_InitVector(&stSortVector , 1.0 , 1.0 , 1.0 );
	qsort ((void *)p_GLV->p_stPoints,p_GLV->ulNumberOfPoints,sizeof(tdst_GLV_Point),GLV_QSortClabk);
	for (Counter=0;Counter<p_GLV->ulNumberOfPoints;Counter++) 
		pRedir[p_GLV->p_stPoints[Counter] . ulRefIndex] = Counter;
	for (Counter=0;Counter<p_GLV->ulNumberOfFaces;Counter++)
	{
		p_GLV->p_stFaces[Counter].Index[0] = pRedir[p_GLV->p_stFaces[Counter].Index[0]];
		p_GLV->p_stFaces[Counter].Index[1] = pRedir[p_GLV->p_stFaces[Counter].Index[1]];
		p_GLV->p_stFaces[Counter].Index[2] = pRedir[p_GLV->p_stFaces[Counter].Index[2]];
	}
	for (Counter=0;Counter<p_GLV->ulNumberOfEdges;Counter++)
	{
		p_GLV->p_stEdges[Counter].Index[0] = pRedir[p_GLV->p_stEdges[Counter].Index[0]];
		p_GLV->p_stEdges[Counter].Index[1] = pRedir[p_GLV->p_stEdges[Counter].Index[1]];
	}
	for (Counter = 0 ; Counter < p_GLV -> ulNumberOfPoints ; Counter ++) 
		pRedir[Counter] = Counter;
	switch (ulMode)
	{
	case 0: /* General case */
		for ( Counter = 0 ; Counter < p_GLV -> ulNumberOfPoints  -  1 ; Counter ++)
		{
			if (pRedir[Counter] == Counter)
			{
				for (Counter2 = Counter + 1 ; Counter2 < p_GLV->ulNumberOfPoints ;  Counter2++)
				{
					if ((p_GLV->p_stPoints[Counter2].P3D.x - p_GLV->p_stPoints[pRedir[Counter]].P3D.x) > Thresh) 
						Counter2 = p_GLV->ulNumberOfPoints;//*/
					else
					{
					if (fabs(p_GLV->p_stPoints[Counter2].P3D.y - p_GLV->p_stPoints[pRedir[Counter]].P3D.y) > Thresh) continue;
					if (fabs(p_GLV->p_stPoints[Counter2].P3D.z - p_GLV->p_stPoints[pRedir[Counter]].P3D.z) > Thresh) continue;//*/
						
					{
						MATHD_SubVector(&fDist , &p_GLV->p_stPoints[Counter2].P3D , &p_GLV->p_stPoints[pRedir[Counter]].P3D);
						fNSpr = MATHD_f_SqrNormVector(&fDist);
						if (fNSpr <= Thresh)
						{
							if ((p_GLV->p_stPoints[Counter2].ulFlags & GLV_FreeDomDegree) > (p_GLV->p_stPoints[Counter].ulFlags & GLV_FreeDomDegree))
								p_GLV->p_stPoints[Counter2] = p_GLV->p_stPoints[Counter];//*/
							pRedir[Counter2] = pRedir[Counter];
						}
					}
					}
				}
			}
		}
		break;
	case 1: /* Color Case */
		for ( Counter = 0 ; Counter < p_GLV -> ulNumberOfPoints  -  1 ; Counter ++)
		{
			if (pRedir[Counter] == Counter)
			{
				for (Counter2 = Counter + 1;Counter2<p_GLV->ulNumberOfPoints;Counter2++)
				{
					if ((p_GLV->p_stPoints[Counter2].P3D.x - p_GLV->p_stPoints[pRedir[Counter]].P3D.x) > Thresh) 
						Counter2 = p_GLV->ulNumberOfPoints;
					else//*/
					{
						MATHD_SubVector(&fDist , &p_GLV->p_stPoints[Counter2].P3D , &p_GLV->p_stPoints[pRedir[Counter]].P3D);
						fNSpr = MATHD_f_SqrNormVector(&fDist);
						if (fNSpr <= Thresh * Thresh)
						{
							/* Compatiblity test */
							if (GLV_OriginalPointAreCompatible(&p_GLV->p_stPoints[pRedir[Counter]] , &p_GLV->p_stPoints[Counter2]))
								pRedir[Counter2] = pRedir[Counter];
						}
					}
				}
			}
		}
		break;
	case 2: /* Normal thresh test */
		for ( Counter = 0 ; Counter < p_GLV -> ulNumberOfPoints  -  1 ; Counter ++)
		{
			{
				for (Counter2 = Counter + 1;Counter2<p_GLV->ulNumberOfPoints;Counter2++)
				{
					if ((p_GLV->p_stPoints[Counter2].P3D.x - p_GLV->p_stPoints[pRedir[Counter]].P3D.x) > Thresh) 
						Counter2 = p_GLV->ulNumberOfPoints;
					else//*/
					{
						MATHD_SubVector(&fDist , &p_GLV->p_stPoints[Counter2].P3D , &p_GLV->p_stPoints[pRedir[Counter]].P3D);
						fNSpr = MATHD_f_SqrNormVector(&fDist);
						if (fNSpr <= Thresh * Thresh)
						{
							MATHD_tdst_Vector Stl , Stl2;
							MATHD_NormalizeVector(&Stl,&p_GLV->p_stPoints[pRedir[Counter]].UpPoint);
							MATHD_NormalizeVector(&Stl2,&p_GLV->p_stPoints[Counter2].UpPoint);
							if (MATHD_f_DotProduct(&Stl2 , &Stl) >= fNormalWeldThresh)
							{
								pRedir[Counter2] = pRedir[Counter];
								MATHD_AddVector(&p_GLV->p_stPoints[pRedir[Counter]].UpPoint , &p_GLV->p_stPoints[pRedir[Counter]].UpPoint , &p_GLV->p_stPoints[Counter2].UpPoint);
							}
						}
					}
				}
			}
		}
		break;
	case 3: /* Color, UV & elementjade Weld  */
		for ( Counter = 0 ; Counter < p_GLV -> ulNumberOfPoints  -  1 ; Counter ++)
		{
			{
				for (Counter2 = Counter + 1;Counter2<p_GLV->ulNumberOfPoints;Counter2++)
				{
					if ((p_GLV->p_stPoints[Counter2].P3D.x - p_GLV->p_stPoints[pRedir[Counter]].P3D.x) > Thresh) 
						Counter2 = p_GLV->ulNumberOfPoints;
					else//*/
					{
						MATHD_SubVector(&fDist , &p_GLV->p_stPoints[Counter2].P3D , &p_GLV->p_stPoints[pRedir[Counter]].P3D);
						fNSpr = MATHD_f_SqrNormVector(&fDist);
						if (fNSpr <= Thresh * Thresh)
						{
							/* Compatiblity test */
							if (GLV_OriginalPointAreCompatible_UV(&p_GLV->p_stPoints[pRedir[Counter]] , &p_GLV->p_stPoints[Counter2]))
								pRedir[Counter2] = pRedir[Counter];
						}
					}
				}
			}
		}
		break;
	case 4: /* Bool overlap case */
		for ( Counter = 0 ; Counter < p_GLV -> ulNumberOfPoints  -  1 ; Counter ++)
		{
			if (pRedir[Counter] == Counter)
			{
				for (Counter2 = Counter + 1;Counter2<p_GLV->ulNumberOfPoints;Counter2++)
				{
					if ((p_GLV->p_stPoints[Counter2].P3D.x - p_GLV->p_stPoints[pRedir[Counter]].P3D.x) > Thresh) 
						Counter2 = p_GLV->ulNumberOfPoints;
					else//*/
					{
						MATHD_SubVector(&fDist , &p_GLV->p_stPoints[Counter2].P3D , &p_GLV->p_stPoints[pRedir[Counter]].P3D);
						fNSpr = MATHD_f_SqrNormVector(&fDist);
						if (fNSpr <= Thresh)
						{
							/* Compatiblity test */
							if (GLV_OriginalPointAreCompatible_Overlap(&p_GLV->p_stPoints[pRedir[Counter]] , &p_GLV->p_stPoints[Counter2]))
								pRedir[Counter2] = pRedir[Counter];
						}
					}
				}
			}
		}
		break;
	};
	for (Counter=0;Counter<p_GLV->ulNumberOfFaces;Counter++)
	{
		p_GLV->p_stFaces[Counter].Index[0] = pRedir[p_GLV->p_stFaces[Counter].Index[0]];
		p_GLV->p_stFaces[Counter].Index[1] = pRedir[p_GLV->p_stFaces[Counter].Index[1]];
		p_GLV->p_stFaces[Counter].Index[2] = pRedir[p_GLV->p_stFaces[Counter].Index[2]];
	}
	for (Counter=0;Counter<p_GLV->ulNumberOfEdges;Counter++)
	{
		p_GLV->p_stEdges[Counter].Index[0] = pRedir[p_GLV->p_stEdges[Counter].Index[0]];
		p_GLV->p_stEdges[Counter].Index[1] = pRedir[p_GLV->p_stEdges[Counter].Index[1]];
	}
	GLD_RemoveUnusedIndexes(p_GLV);
	GLV_Verify(p_GLV);
	GLV_FREE(pRedir);
}
u32  GLD_GetNearestIndex( tdst_GLV * p_GLV , MATHD_tdst_Vector *P3D)
{
	u32 Counter,CounterEst,Dicoto;
	GLV_Scalar Thresh;
	Dicoto = p_GLV->ulNumberOfPoints >> 1;
	Counter = 0;
	CounterEst = 0;
	Thresh = (GLV_Scalar)sqrt(GLV_WELD_FCT) * 4.0;
	while (Dicoto)
	{
		CounterEst += Dicoto;
		if (CounterEst >= p_GLV->ulNumberOfPoints) CounterEst = p_GLV->ulNumberOfPoints - 1;
		if ((P3D->x - p_GLV->p_stPoints[CounterEst].P3D.x) > Thresh) 
		{
			Counter = CounterEst;
		} else
			CounterEst = Counter;
		Dicoto >>= 1;
	}//*/
	while (Counter < p_GLV->ulNumberOfPoints)
	{
		if ((p_GLV->p_stPoints[Counter].P3D.x - P3D->x) > Thresh ) 
			return 0xffffffff;
		else
		{
			MATHD_tdst_Vector fDist;
			GLV_Scalar fNSpr;
			MATHD_SubVector(&fDist , P3D , &p_GLV->p_stPoints[Counter].P3D);
			fNSpr = MATHD_f_SqrNormVector(&fDist);
			if (fNSpr <= Thresh ) 
				return Counter;
		}
		Counter ++;
	}
	return 0xffffffff;
}

/* this function is the invert of WEld 
	it will create 3 points per face
*/
void GLD_BlowUpIndexion(  tdst_GLV * p_GLV , ULONG ModeUV)
{
	ULONG Counter;
	tdst_GLV_Point *pOldVertices;
	pOldVertices = (tdst_GLV_Point*)GLV_ALLOC ( sizeof ( tdst_GLV_Point ) * p_GLV -> ulNumberOfPoints );
	L_memcpy( pOldVertices , p_GLV -> p_stPoints , sizeof ( tdst_GLV_Point ) * p_GLV -> ulNumberOfPoints );
	GLV_SetNumbers( p_GLV , p_GLV -> ulNumberOfFaces * 3L , 0 , 0 , 1 );
	for ( Counter = 0 ; Counter < p_GLV -> ulNumberOfFaces ; Counter ++ )
	{
		p_GLV -> p_stPoints [ Counter * 3L + 0] = pOldVertices [ p_GLV -> p_stFaces [ Counter ] . Index [0] ];
		p_GLV -> p_stPoints [ Counter * 3L + 1] = pOldVertices [ p_GLV -> p_stFaces [ Counter ] . Index [1] ];
		p_GLV -> p_stPoints [ Counter * 3L + 2] = pOldVertices [ p_GLV -> p_stFaces [ Counter ] . Index [2] ];
		p_GLV -> p_stFaces [ Counter ] . Index [0] = Counter * 3L + 0;
		p_GLV -> p_stFaces [ Counter ] . Index [1] = Counter * 3L + 1;
		p_GLV -> p_stFaces [ Counter ] . Index [2] = Counter * 3L + 2;
		MATH_To_MATHD (&p_GLV -> p_stPoints [ Counter * 3L + 0] . UpPoint , &p_GLV -> p_stFaces [ Counter ] . OriginalPointsNormales [0]);
		MATH_To_MATHD (&p_GLV -> p_stPoints [ Counter * 3L + 1] . UpPoint , &p_GLV -> p_stFaces [ Counter ] . OriginalPointsNormales [1]);
		MATH_To_MATHD (&p_GLV -> p_stPoints [ Counter * 3L + 2] . UpPoint , &p_GLV -> p_stFaces [ Counter ] . OriginalPointsNormales [2]);
		p_GLV -> p_stPoints [ Counter * 3L + 0] . ulColor = p_GLV -> p_stFaces [ Counter ] . Colours [0];
		p_GLV -> p_stPoints [ Counter * 3L + 1] . ulColor = p_GLV -> p_stFaces [ Counter ] . Colours [1];
		p_GLV -> p_stPoints [ Counter * 3L + 2] . ulColor = p_GLV -> p_stFaces [ Counter ] . Colours [2];
		p_GLV -> p_stPoints [ Counter * 3L + 0] .ulSurfaceOwner= p_GLV -> p_stFaces [ Counter ] .ulSurfaceNumber;
		p_GLV -> p_stPoints [ Counter * 3L + 1] .ulSurfaceOwner= p_GLV -> p_stFaces [ Counter ] .ulSurfaceNumber;
		p_GLV -> p_stPoints [ Counter * 3L + 2] .ulSurfaceOwner= p_GLV -> p_stFaces [ Counter ] .ulSurfaceNumber;
		if (ModeUV)
		{
			p_GLV -> p_stPoints [ Counter * 3L + 0] .stUVInfo.UV[0] = p_GLV -> p_stFaces [ Counter ] .UV [0];
			p_GLV -> p_stPoints [ Counter * 3L + 0] .stUVInfo.UV[1] = p_GLV -> p_stFaces [ Counter ] .UV [1];
			p_GLV -> p_stPoints [ Counter * 3L + 0] .stUVInfo.MaterialNum = p_GLV -> p_stFaces [ Counter ] . ulElementJADE;
			p_GLV -> p_stPoints [ Counter * 3L + 1] .stUVInfo.UV[0] = p_GLV -> p_stFaces [ Counter ] .UV [2];
			p_GLV -> p_stPoints [ Counter * 3L + 1] .stUVInfo.UV[1] = p_GLV -> p_stFaces [ Counter ] .UV [3];
			p_GLV -> p_stPoints [ Counter * 3L + 1] .stUVInfo.MaterialNum = p_GLV -> p_stFaces [ Counter ] . ulElementJADE;
			p_GLV -> p_stPoints [ Counter * 3L + 2] .stUVInfo.UV[0] = p_GLV -> p_stFaces [ Counter ] .UV [4];
			p_GLV -> p_stPoints [ Counter * 3L + 2] .stUVInfo.UV[1] = p_GLV -> p_stFaces [ Counter ] .UV [5];
			p_GLV -> p_stPoints [ Counter * 3L + 2] .stUVInfo.MaterialNum = p_GLV -> p_stFaces [ Counter ] . ulElementJADE;
		}
	}
	GLV_FREE(pOldVertices);
}


void GLD_ComputePointsNormals(  tdst_GLV * p_GLV )
{
	ULONG Counter;
	for (Counter = 0 ; Counter < p_GLV->ulNumberOfPoints; Counter ++)
		MATHD_InitVector(&p_GLV->p_stPoints[Counter].UpPoint , 0.0 , 0.0 , 0.0 );

	for (Counter = 0 ; Counter < p_GLV->ulNumberOfFaces; Counter ++)
	{
		MATHD_AddVector(&p_GLV->p_stPoints[p_GLV->p_stFaces[Counter].Index[0]].UpPoint , &p_GLV->p_stPoints[p_GLV->p_stFaces[Counter].Index[0]].UpPoint , &p_GLV->p_stFaces[Counter].Plane.Normale );
		MATHD_AddVector(&p_GLV->p_stPoints[p_GLV->p_stFaces[Counter].Index[1]].UpPoint , &p_GLV->p_stPoints[p_GLV->p_stFaces[Counter].Index[1]].UpPoint , &p_GLV->p_stFaces[Counter].Plane.Normale );
		MATHD_AddVector(&p_GLV->p_stPoints[p_GLV->p_stFaces[Counter].Index[2]].UpPoint , &p_GLV->p_stPoints[p_GLV->p_stFaces[Counter].Index[2]].UpPoint , &p_GLV->p_stFaces[Counter].Plane.Normale );
	}
	for (Counter = 0 ; Counter < p_GLV->ulNumberOfPoints; Counter ++)
	{
		MATHD_NormalizeVector(&p_GLV->p_stPoints[Counter].UpPoint , &p_GLV->p_stPoints[Counter].UpPoint );
	}
}

void GLD_BackUpPointsNormals(  tdst_GLV * p_GLV )
{
	ULONG Counter;
	for ( Counter = 0 ; Counter < p_GLV->ulNumberOfFaces ; Counter ++)
	{
		MATHD_To_MATH(&p_GLV->p_stFaces[Counter].OriginalPointsNormales[0] , &p_GLV->p_stPoints[p_GLV->p_stFaces[Counter].Index[0]].UpPoint);
		MATHD_To_MATH(&p_GLV->p_stFaces[Counter].OriginalPointsNormales[1] , &p_GLV->p_stPoints[p_GLV->p_stFaces[Counter].Index[1]].UpPoint);
		MATHD_To_MATH(&p_GLV->p_stFaces[Counter].OriginalPointsNormales[2] , &p_GLV->p_stPoints[p_GLV->p_stFaces[Counter].Index[2]].UpPoint);
	}
}

void GLD_BreakEdge(  tdst_GLV * p_GLV , float thresh)
{
	GLD_BlowUpIndexion( p_GLV , 0 ) ;
	GLD_ComputePointsNormals(  p_GLV ) ;
	fNormalWeldThresh = thresh;
	GLD_Weld( p_GLV , GLV_WELD_FCT , 2); /* Weld all */
	GLD_ComputePointsNormals(  p_GLV ) ;
	GLD_BackUpPointsNormals(  p_GLV );
}


/* 
	this function will sort the triangle by indexes 
	Goal is to optimize (theoricaly) clusters in Octree;
*/
int GLV_SortFaceIndexion_Clabk(const void *T1,const void *T2)
{
	ULONG SWP;
	ULONG I1[3] , I2[3] ;
	I1[0] = ((tdst_GLV_Face *)T1)->Index[0];
	I1[1] = ((tdst_GLV_Face *)T1)->Index[1];
	I1[2] = ((tdst_GLV_Face *)T1)->Index[2];
	if (I1[0] < I1[1]) { SWP = I1[1] ; I1[1] = I1[0] ; I1[0] = SWP;}
	if (I1[0] < I1[2]) { SWP = I1[2] ; I1[2] = I1[0] ; I1[0] = SWP;}
	if (I1[1] < I1[2]) { SWP = I1[2] ; I1[2] = I1[1] ; I1[1] = SWP;}
	I2[0] = ((tdst_GLV_Face *)T2)->Index[0];
	I2[1] = ((tdst_GLV_Face *)T2)->Index[1];
	I2[2] = ((tdst_GLV_Face *)T2)->Index[2];
	if (I2[0] < I2[1]) { SWP = I2[1] ; I2[1] = I2[0] ; I2[0] = SWP;}
	if (I2[0] < I2[2]) { SWP = I2[2] ; I2[2] = I2[0] ; I2[0] = SWP;}
	if (I2[1] < I2[2]) { SWP = I2[2] ; I2[2] = I2[1] ; I2[1] = SWP;}

	if (I1[0] < I2[0]) 
		return -1;
	else 
		if (I1[0] > I2[0])
			return 1;
		else
		{
			if (I1[1] < I2[1]) 
				return -1;
			else 
				if (I1[1] > I2[1]) 
					return 1;
		}
		return 0;
}

void GLD_SortFaceIndexion( tdst_GLV * p_GLV )
{
	qsort ((void *)p_GLV->p_stFaces , p_GLV->ulNumberOfFaces , sizeof(tdst_GLV_Face) , GLV_SortFaceIndexion_Clabk);
}

/* this function is the invert of WEld 
	it will create 3 points per face
*/
tdst_GLV *GLD_CompressUVIndexion( tdst_GLV * p_GLV )
{
	ULONG Counter , Counter2;
	tdst_GLV * p_GLV2;
	p_GLV2 = NewGLV();
	GLV_SetNumbers(p_GLV2 , p_GLV->ulNumberOfFaces * 3L , 0 , p_GLV->ulNumberOfFaces , 5 );
	for ( Counter = 0 ; Counter < p_GLV -> ulNumberOfFaces ; Counter ++ )
	{
		for (Counter2 = 0 ; Counter2 < 3 ; Counter2 ++)
		{
			p_GLV2->p_stPoints[(Counter * 3L) + Counter2].ulFlags = 0;
			p_GLV2->p_stPoints[(Counter * 3L) + Counter2].P3D.x = p_GLV->p_stFaces[Counter].UV[0 + (Counter2 << 1L)];
			p_GLV2->p_stPoints[(Counter * 3L) + Counter2].P3D.y = p_GLV->p_stFaces[Counter].UV[1 + (Counter2 << 1L)];
			p_GLV2->p_stPoints[(Counter * 3L) + Counter2].P3D.z = 0.0f;
			p_GLV2->p_stFaces[Counter].Index[Counter2] = (Counter * 3L) + Counter2;
			p_GLV2->p_stFaces[Counter].Nghbr[Counter2] = 0xffffffff;
		}
	}
	GLD_Weld( p_GLV2 , GLF_PrecMetric_UV , 0);
	GLD_RemoveUnusedIndexes(p_GLV2);//*/
	for ( Counter = 0 ; Counter < p_GLV -> ulNumberOfFaces ; Counter ++ )
	{
		for (Counter2 = 0 ; Counter2 < 3 ; Counter2 ++)
		{
			p_GLV->p_stFaces[Counter].UVIndex[Counter2] = p_GLV2->p_stFaces[Counter].Index[Counter2];
		}
	}
	return p_GLV2;
}


ULONG GLV_Unify2Face(tdst_GLV *p_GLV, ULONG N1, ULONG N2)
{
	u32 N1_in_N2,N2_in_N1;
    /* Test for Neightbour 1 */
	N2_in_N1 = 0xffffffff;
	N1_in_N2 = 0xffffffff;

	if (p_GLV->p_stFaces[N1].ulMARK != p_GLV->p_stFaces[N2].ulMARK) return 0;
    if((p_GLV->p_stFaces[N1].Index[0] == p_GLV->p_stFaces[N2].Index[1]) && (p_GLV->p_stFaces[N1].Index[1] == p_GLV->p_stFaces[N2].Index[0]))
    {
/*        p_GLV->p_stFaces[N1].Nghbr[0] = N2;
        p_GLV->p_stFaces[N2].Nghbr[0] = N1;
        return 1;//*/
		N2_in_N1 = 0;
		N1_in_N2 = 0;
    }
	
    if((p_GLV->p_stFaces[N1].Index[1] == p_GLV->p_stFaces[N2].Index[1]) && (p_GLV->p_stFaces[N1].Index[2] == p_GLV->p_stFaces[N2].Index[0]))
    {
		N2_in_N1 = 1;
		N1_in_N2 = 0;
    }
	
    if((p_GLV->p_stFaces[N1].Index[2] == p_GLV->p_stFaces[N2].Index[1]) && (p_GLV->p_stFaces[N1].Index[0] == p_GLV->p_stFaces[N2].Index[0]))
    {
		N2_in_N1 = 2;
		N1_in_N2 = 0;
    }
	
    if((p_GLV->p_stFaces[N1].Index[0] == p_GLV->p_stFaces[N2].Index[2]) && (p_GLV->p_stFaces[N1].Index[1] == p_GLV->p_stFaces[N2].Index[1]))
    {
		N2_in_N1 = 0;
		N1_in_N2 = 1;
    }
	
    if((p_GLV->p_stFaces[N1].Index[1] == p_GLV->p_stFaces[N2].Index[2]) && (p_GLV->p_stFaces[N1].Index[2] == p_GLV->p_stFaces[N2].Index[1]))
    {
		N2_in_N1 = 1;
		N1_in_N2 = 1;
    }
	
    if((p_GLV->p_stFaces[N1].Index[2] == p_GLV->p_stFaces[N2].Index[2]) && (p_GLV->p_stFaces[N1].Index[0] == p_GLV->p_stFaces[N2].Index[1]))
    {
		N2_in_N1 = 2;
		N1_in_N2 = 1;
    }
	
    if((p_GLV->p_stFaces[N1].Index[0] == p_GLV->p_stFaces[N2].Index[0]) && (p_GLV->p_stFaces[N1].Index[1] == p_GLV->p_stFaces[N2].Index[2]))
    {
		N2_in_N1 = 0;
		N1_in_N2 = 2;
    }
	
    if((p_GLV->p_stFaces[N1].Index[1] == p_GLV->p_stFaces[N2].Index[0]) && (p_GLV->p_stFaces[N1].Index[2] == p_GLV->p_stFaces[N2].Index[2]))
    {
		N2_in_N1 = 1;
		N1_in_N2 = 2;
    }
	
    if((p_GLV->p_stFaces[N1].Index[2] == p_GLV->p_stFaces[N2].Index[0]) && (p_GLV->p_stFaces[N1].Index[0] == p_GLV->p_stFaces[N2].Index[2]))
    {
		N2_in_N1 = 2;
		N1_in_N2 = 2;
    }

	if ((N2_in_N1 | N1_in_N2) != 0xffffffff)
	{
		if (p_GLV->p_stFaces[N1].Nghbr[N2_in_N1] < 0xff000000)
		{
			GLV_Scalar OldDP,NewDP;
			OldDP = MATHD_f_DotProduct(&p_GLV->p_stFaces[N1].Plane.Normale , &p_GLV->p_stFaces[p_GLV->p_stFaces[N1].Nghbr[N2_in_N1]].Plane.Normale);
			NewDP = MATHD_f_DotProduct(&p_GLV->p_stFaces[N1].Plane.Normale , &p_GLV->p_stFaces[N2].Plane.Normale);
			/* Merge if better */
			if (NewDP > OldDP)
				p_GLV->p_stFaces[N1].Nghbr[N2_in_N1] = N2;
			else
			{
				/* */
				OldDP = GLV_GetSurf(p_GLV,&p_GLV->p_stFaces[p_GLV->p_stFaces[N1].Nghbr[N2_in_N1]]);
				NewDP = GLV_GetSurf(p_GLV,&p_GLV->p_stFaces[N2]);
				if (NewDP < OldDP)
					p_GLV->p_stFaces[N1].Nghbr[N2_in_N1] = N2;
			}
		} else
			p_GLV->p_stFaces[N1].Nghbr[N2_in_N1] = N2;

		if (p_GLV->p_stFaces[N2].Nghbr[N1_in_N2] < 0xff000000)
		{
			GLV_Scalar OldDP,NewDP;
			OldDP = MATHD_f_DotProduct(&p_GLV->p_stFaces[N2].Plane.Normale , &p_GLV->p_stFaces[p_GLV->p_stFaces[N2].Nghbr[N1_in_N2]].Plane.Normale);
			NewDP = MATHD_f_DotProduct(&p_GLV->p_stFaces[N2].Plane.Normale , &p_GLV->p_stFaces[N1].Plane.Normale);
			/* Merge if better */
			if (NewDP > OldDP)
				p_GLV->p_stFaces[N2].Nghbr[N1_in_N2] = N1;
			else
			{
				/* */
				OldDP = GLV_GetSurf(p_GLV,&p_GLV->p_stFaces[p_GLV->p_stFaces[N2].Nghbr[N1_in_N2]]);
				NewDP = GLV_GetSurf(p_GLV,&p_GLV->p_stFaces[N1]);
				if (NewDP < OldDP)
					p_GLV->p_stFaces[N2].Nghbr[N1_in_N2] = N1;
			}
		} else
			p_GLV->p_stFaces[N2].Nghbr[N1_in_N2] = N1;
		return 1;
	}
    return 0;   /* Bug! */
}
static ULONG FaceKeyCounter = 10;
void GLV_SetNumbers(tdst_GLV *p_GLV,ULONG Points , ULONG Edges , ULONG Triangles,ULONG ulFlag)
{
	ULONG ulAlloc;
	if (ulFlag & 1)
	{
		if (Points != p_GLV ->ulNumberOfPoints)
		{
			ulAlloc = p_GLV ->ulRealNumberOfPoints;
			if (p_GLV ->p_stPoints)
			{
				while (ulAlloc <= Points) ulAlloc += GLV_MEMORY_GRANULARITY;
				if (ulAlloc != p_GLV ->ulRealNumberOfPoints)
					p_GLV ->p_stPoints = (tdst_GLV_Point*)GLV_REALLOC(p_GLV ->p_stPoints , sizeof (tdst_GLV_Point) * ulAlloc);
			}
			else
			{
				ulAlloc = GLV_MEMORY_GRANULARITY;
				while (ulAlloc <= Points) ulAlloc += GLV_MEMORY_GRANULARITY;
					p_GLV ->p_stPoints = (tdst_GLV_Point*)GLV_ALLOC(sizeof (tdst_GLV_Point) * ulAlloc);
			}
			p_GLV ->ulRealNumberOfPoints = ulAlloc;

			while (p_GLV ->ulNumberOfPoints < Points)
			{
				memset(p_GLV ->p_stPoints + p_GLV ->ulNumberOfPoints , 0 , sizeof(tdst_GLV_Point));
				p_GLV ->ulNumberOfPoints ++;
			}
			p_GLV ->ulNumberOfPoints = Points;
		}
	}
	if (ulFlag & 2)
	{
		if (p_GLV ->ulNumberOfEdges != Edges)
		{
			ulAlloc = p_GLV ->ulRealNumberOfEdges;
			if (p_GLV ->p_stEdges)
			{
				while (ulAlloc <= Edges) ulAlloc += GLV_MEMORY_GRANULARITY;
				if (ulAlloc != p_GLV ->ulRealNumberOfEdges)
					p_GLV ->p_stEdges = (tdst_GLV_Edge*)GLV_REALLOC(p_GLV ->p_stEdges , sizeof (tdst_GLV_Edge) * ulAlloc);
			}
			else
			{
				ulAlloc = GLV_MEMORY_GRANULARITY;
				while (ulAlloc <= Edges) ulAlloc += GLV_MEMORY_GRANULARITY;
					p_GLV ->p_stEdges = (tdst_GLV_Edge*)GLV_ALLOC(sizeof (tdst_GLV_Edge) * ulAlloc);
			}
			p_GLV ->ulRealNumberOfEdges = ulAlloc;

			p_GLV ->ulNumberOfEdges  = Edges;
		}
	}
	if (ulFlag & 4)
	{
		if (p_GLV ->ulNumberOfFaces != Triangles)
		{
			ULONG OriginalNF;
			OriginalNF = p_GLV->ulRealNumberOfFaces;
			ulAlloc = p_GLV ->ulRealNumberOfFaces;
			if (p_GLV ->p_stFaces)
			{
				while (ulAlloc <= Triangles) ulAlloc += GLV_MEMORY_GRANULARITY;
				if (ulAlloc != p_GLV ->ulRealNumberOfFaces)
					p_GLV->p_stFaces = (tdst_GLV_Face*)GLV_REALLOC(p_GLV ->p_stFaces , sizeof (tdst_GLV_Face) * ulAlloc);
			}
			else
			{
				ulAlloc = GLV_MEMORY_GRANULARITY;
				while (ulAlloc <= Triangles) ulAlloc += GLV_MEMORY_GRANULARITY;
					p_GLV->p_stFaces = (tdst_GLV_Face*)GLV_ALLOC(sizeof (tdst_GLV_Face) * ulAlloc);
			}
			p_GLV ->ulRealNumberOfFaces = ulAlloc;

			while (OriginalNF < p_GLV->ulRealNumberOfFaces)
				p_GLV->p_stFaces[OriginalNF++].ulKey = FaceKeyCounter++;

#ifdef JADEFUSION
            // SC: Free the form factors if any
            if (p_GLV->p_FormFactors)
            {
                ULONG Counter;
                Counter = p_GLV->ulNumberOfFaces * 3l;
                while (Counter--) if (p_GLV ->p_FormFactors[Counter]) GLV_FREE(p_GLV->p_FormFactors[Counter]);
                GLV_FREE(p_GLV->p_FormFactors);
                p_GLV->p_FormFactors = NULL;
            }
#endif

			p_GLV ->ulNumberOfFaces = Triangles;
		}
	}
}
void GLV_Merge2Face(tdst_GLV * p_GLV , ULONG F1 , ULONG F2 , ULONG P)
{
	ULONG PE1,PE2;
	if ((p_GLV->p_stFaces[F1].ulFlags | p_GLV->p_stFaces[F2].ulFlags) & GLV_FLGS_RSV1) return;
	if ((p_GLV->p_stFaces[F1].ulFlags | p_GLV->p_stFaces[F2].ulFlags) & GLV_FLGS_Original) return;
	PE1 = p_GLV->p_stFaces[F1].Index[0];
	if ((p_GLV->p_stFaces[F2].Index[0] == PE1) || (p_GLV->p_stFaces[F2].Index[1] == PE1) || (p_GLV->p_stFaces[F2].Index[2] == PE1))
	{
		PE1 = p_GLV->p_stFaces[F1].Index[1];
		if ((p_GLV->p_stFaces[F2].Index[0] == PE1) || (p_GLV->p_stFaces[F2].Index[1] == PE1) || (p_GLV->p_stFaces[F2].Index[2] == PE1))
		{
			PE1 = p_GLV->p_stFaces[F1].Index[2];
		}
	}
	PE2 = p_GLV->p_stFaces[F2].Index[0];
	if ((p_GLV->p_stFaces[F1].Index[0] == PE2) || (p_GLV->p_stFaces[F1].Index[1] == PE2) || (p_GLV->p_stFaces[F1].Index[2] == PE2))
	{
		PE2 = p_GLV->p_stFaces[F2].Index[1];
		if ((p_GLV->p_stFaces[F1].Index[0] == PE2) || (p_GLV->p_stFaces[F1].Index[1] == PE2) || (p_GLV->p_stFaces[F1].Index[2] == PE2))
			PE2 = p_GLV->p_stFaces[F2].Index[2];
	}
	if (GLV_IsBetween( &p_GLV->p_stPoints[PE1].P3D  , &p_GLV->p_stPoints[P].P3D , &p_GLV->p_stPoints[PE2].P3D  ))
	/* test colinearity */
	if (GLV_IsColinear( &p_GLV->p_stPoints[PE1].P3D  , &p_GLV->p_stPoints[PE2].P3D  , &p_GLV->p_stPoints[P].P3D ))
	{
		for (PE2 = 0 ; PE2 < 3 ; PE2++)
		{
			if (p_GLV->p_stFaces[F1].Index[PE2] == P) p_GLV->p_stFaces[F1].Index[PE2] = PE1;
			if (p_GLV->p_stFaces[F2].Index[PE2] == P) p_GLV->p_stFaces[F2].Index[PE2] = PE1;
		}
		p_GLV->p_stFaces[F1].ulFlags |=  GLV_FLGS_RSV1;
		p_GLV->p_stFaces[F2].ulFlags |=  GLV_FLGS_RSV1;
	}
}
void GLV_Merge3Face(tdst_GLV * p_GLV , ULONG F1 , ULONG F2 , ULONG F3 , ULONG P)
{
}


ULONG GLD_RemoveCurtain( tdst_GLV * p_GLV )
{
	ULONG *p_ulTouchCount;
	ULONG *p_ulF1, *p_ulF2, *p_ulF3;
	ULONG Counter,Tr1,Res;
	Res = 0;
	p_ulTouchCount = (ULONG*)GLV_ALLOC(4L * p_GLV->ulNumberOfPoints);
	p_ulF1 = (ULONG*)GLV_ALLOC(4L * p_GLV->ulNumberOfPoints);
	p_ulF2 = (ULONG*)GLV_ALLOC(4L * p_GLV->ulNumberOfPoints);
	p_ulF3 = (ULONG*)GLV_ALLOC(4L * p_GLV->ulNumberOfPoints);
	L_memset(p_ulTouchCount , 0 , 4L * p_GLV->ulNumberOfPoints);
	for (Counter= 0; Counter < p_GLV->ulNumberOfFaces; Counter++)
	{
		p_GLV ->p_stFaces[Counter].ulFlags &= ~(GLV_FLGS_RSV1|GLV_FLGS_DeleteIt);
		for (Tr1 = 0 ; Tr1 < 3 ; Tr1 ++)
		{
			if (p_ulTouchCount[p_GLV ->p_stFaces[Counter].Index[Tr1]] == 0) p_ulF1 [p_GLV ->p_stFaces[Counter].Index[Tr1]] = Counter;
			if (p_ulTouchCount[p_GLV ->p_stFaces[Counter].Index[Tr1]] == 1) p_ulF2 [p_GLV ->p_stFaces[Counter].Index[Tr1]] = Counter;
			if (p_ulTouchCount[p_GLV ->p_stFaces[Counter].Index[Tr1]] == 2) p_ulF3 [p_GLV ->p_stFaces[Counter].Index[Tr1]] = Counter;
			p_ulTouchCount[p_GLV ->p_stFaces[Counter].Index[Tr1]]++;
		}
	}
	for (Counter= 0; Counter < p_GLV->ulNumberOfPoints; Counter++)
	{
		if (p_ulTouchCount[Counter] == 2)
			GLV_Merge2Face( p_GLV , p_ulF1[Counter] , p_ulF2[Counter] , Counter);
		else
		if (p_ulTouchCount[Counter] == 3)
			GLV_Merge3Face( p_GLV , p_ulF1[Counter] , p_ulF2[Counter] , p_ulF3[Counter] , Counter);
	}
	Res = p_GLV ->ulNumberOfPoints;
	GLD_Remove2BD ( p_GLV );
	GLD_RemoveIllegalFaces( p_GLV );
	GLD_RemoveUnusedIndexes( p_GLV );
	Res = Res - p_GLV ->ulNumberOfPoints;
	GLV_FREE(p_ulTouchCount);
	GLV_FREE(p_ulF1);
	GLV_FREE(p_ulF2);
	GLV_FREE(p_ulF3);
	return (Res);
}
GLV_Scalar GLD_GetFaceDistortion( tdst_GLV * p_GLV , ULONG F1 )
{
	MATHD_tdst_Vector P1;
	GLV_Scalar Lenght[3],SWP;
	MATHD_SubVector(&P1, &p_GLV->p_stPoints[p_GLV->p_stFaces[F1].Index[0]].P3D , &p_GLV->p_stPoints[p_GLV->p_stFaces[F1].Index[1]].P3D );
	Lenght[0] = MATHD_f_NormVector(&P1);
	MATHD_SubVector(&P1, &p_GLV->p_stPoints[p_GLV->p_stFaces[F1].Index[1]].P3D , &p_GLV->p_stPoints[p_GLV->p_stFaces[F1].Index[2]].P3D );
	Lenght[1] = MATHD_f_NormVector(&P1);
	MATHD_SubVector(&P1, &p_GLV->p_stPoints[p_GLV->p_stFaces[F1].Index[2]].P3D , &p_GLV->p_stPoints[p_GLV->p_stFaces[F1].Index[0]].P3D );
	Lenght[2] = MATHD_f_NormVector(&P1);
	if (Lenght[0] > Lenght[1])
	{
		SWP = Lenght[0] ; Lenght[0] = Lenght[1] ; Lenght[1] = SWP;
	}
	if (Lenght[0] > Lenght[2])
	{
		SWP = Lenght[0] ; Lenght[0] = Lenght[2] ; Lenght[2] = SWP;
	}
	if (Lenght[1] > Lenght[2])
	{
		SWP = Lenght[1] ; Lenght[1] = Lenght[2] ; Lenght[2] = SWP;
	}
	if (Lenght[0] == 0.0f) return 0.0f;
	else return Lenght[2] / Lenght[0];
}
ULONG GLV_IsEqualPlanes(tdst_GLV_Plane *P1, tdst_GLV_Plane *P2)
{
	MATHD_tdst_Vector std;
	MATHD_SubVector(&std , &P1->Normale , &P2->Normale);
	if (MATHD_f_SqrNormVector(&std) > GLF_PrecMetric) return 0;
	if (P1->fDistanceTo0 - P2->fDistanceTo0 < -GLF_PrecMetric) return 0;
	if (P1->fDistanceTo0 - P2->fDistanceTo0 >  GLF_PrecMetric) return 0;
	return 1;
}
void MATHD_NegEqualVector(MATHD_tdst_Vector    *pst_Dest)
{
	pst_Dest->x = -pst_Dest->x;
	pst_Dest->y = -pst_Dest->y;
	pst_Dest->z = -pst_Dest->z;
}
ULONG GLV_IsEqualPlanesNoBackface(tdst_GLV_Plane *P1, tdst_GLV_Plane *P2)
{
	ULONG ret;
	if (GLV_IsEqualPlanes(P1, P2)) return 1;
	ret = 0;
	MATHD_NegEqualVector(&P1->Normale);
	P1->fDistanceTo0 = -P1->fDistanceTo0;
	if (GLV_IsEqualPlanes(P1, P2)) ret = 1;
	MATHD_NegEqualVector(&P1->Normale);
	P1->fDistanceTo0 = -P1->fDistanceTo0;
	return ret;
}


ULONG GLD_TurnEdges_On_2_Faces( tdst_GLV * p_GLV , ULONG F1 , ULONG F2 , ULONG ulMode)
{
	ULONG AinB,BinA,Counter;
	GLV_Scalar D1 , D2;
	MATHD_tdst_Vector std;
	tdst_GLV_Face SF1,SF2;
	if ((p_GLV->p_stFaces[F1].ulFlags | p_GLV->p_stFaces[F2].ulFlags) & GLV_FLGS_RSV1) return 0;		// Mean they already been turned 

	BinA = AinB = -1;
	for (Counter = 0 ; Counter < 3 ; Counter ++)
	{
		if (p_GLV->p_stFaces[F1].Nghbr[Counter] == F2) BinA = Counter;
		if (p_GLV->p_stFaces[F2].Nghbr[Counter] == F1) AinB = Counter;
	}
	if ((AinB == -1) || (BinA == -1)) 
		return 0;
	SF1 = p_GLV->p_stFaces[F1];
	SF2 = p_GLV->p_stFaces[F2];
	switch (ulMode)
	{
	case 0:/* Shorter */
		MATHD_SubVector(&std , &p_GLV->p_stPoints[SF1.Index[(BinA + 2) % 3]].P3D , &p_GLV->p_stPoints[SF2.Index[(AinB + 2) % 3]].P3D);
		D1 = MATHD_f_NormVector(&std);
		
		MATHD_SubVector(&std , &p_GLV->p_stPoints[SF1.Index[(BinA + 0) % 3]].P3D , &p_GLV->p_stPoints[SF1.Index[(BinA + 1) % 3]].P3D);
		D2 = MATHD_f_NormVector(&std);
		if (D2 <= D1 + GLF_PrecMetric)
			return 0;
		break;
	case 2:/* longer */
		MATHD_SubVector(&std , &p_GLV->p_stPoints[SF1.Index[(BinA + 2) % 3]].P3D , &p_GLV->p_stPoints[SF2.Index[(AinB + 2) % 3]].P3D);
		D1 = MATHD_f_NormVector(&std);
		
		MATHD_SubVector(&std , &p_GLV->p_stPoints[SF1.Index[(BinA + 0) % 3]].P3D , &p_GLV->p_stPoints[SF1.Index[(BinA + 1) % 3]].P3D);
		D2 = MATHD_f_NormVector(&std);
		if (D1 <= D2 + GLF_PrecMetric)
			return 0;
		break;
	case 1:/* RSV1 (used in optimize) */
		/* First condition */
		if ((p_GLV->p_stPoints[SF1.Index[(BinA + 0) % 3]].ulFlags | p_GLV->p_stPoints[SF1.Index[(BinA + 1) % 3]].ulFlags) & GLV_FLGS_RSV1)
		{
			if ((p_GLV->p_stPoints[SF1.Index[(BinA + 2) % 3]].ulFlags | p_GLV->p_stPoints[SF2.Index[(AinB + 2) % 3]].ulFlags) & GLV_FLGS_RSV1)
			{
				return 0;
			}
		} else
			return 0;
		break;
	case 3:/* Avoid Surface 0 */
		MATHD_SubVector(&std , &p_GLV->p_stPoints[SF1.Index[(BinA + 2) % 3]].P3D , &p_GLV->p_stPoints[SF2.Index[(AinB + 2) % 3]].P3D);
		D1 = GLV_GetSurf(p_GLV,&p_GLV->p_stFaces[F1]);
		D2 = GLV_GetSurf(p_GLV,&p_GLV->p_stFaces[F2]);
		if ((D1 >= GLF_PrecMetric * 100000.0) && (D2 >= GLF_PrecMetric * 100000.0))
			return 0;
		else
			D1 = D1 ;
		break;
	}

	p_GLV->p_stFaces[F1].Index[BinA] = SF2.Index[(AinB + 2 ) % 3] ;
	p_GLV->p_stFaces[F2].Index[AinB] = SF1.Index[(BinA + 2 ) % 3] ;
	if (GLV_FlicTst_With_0(p_GLV,F1) || GLV_FlicTst_With_0(p_GLV,F2))
	{
		p_GLV->p_stFaces[F1] = SF1;
		p_GLV->p_stFaces[F2] = SF2;
		return 0;
	} else
	{
		/* Neighbours */
		p_GLV->p_stFaces[F1].Nghbr[BinA] = SF2.Nghbr[(AinB + 2) % 3];
		p_GLV->p_stFaces[F1].Nghbr[(BinA + 2) % 3] = F2;
		p_GLV->p_stFaces[F2].Nghbr[AinB] = SF1.Nghbr[(BinA + 2) % 3];
		p_GLV->p_stFaces[F2].Nghbr[(AinB + 2) % 3] = F1;
		/* UV */
		p_GLV->p_stFaces[F1].UV[(BinA << 1) + 0] = SF2.UV[(((AinB + 2) % 3) << 1) + 0];
		p_GLV->p_stFaces[F1].UV[(BinA << 1) + 1] = SF2.UV[(((AinB + 2) % 3) << 1) + 1];
		p_GLV->p_stFaces[F2].UV[(AinB << 1) + 0] = SF1.UV[(((BinA + 2) % 3) << 1) + 0];
		p_GLV->p_stFaces[F2].UV[(AinB << 1) + 1] = SF1.UV[(((BinA + 2) % 3) << 1) + 1];
		/* Colors */
		p_GLV->p_stFaces[F1].Colours[BinA] = SF2.Colours[(AinB + 2) % 3];
		p_GLV->p_stFaces[F2].Colours[AinB] = SF1.Colours[(BinA + 2) % 3];
		/* Alphas */
		p_GLV->p_stFaces[F1].falpha[BinA] = SF2.falpha[(AinB + 2) % 3];
		p_GLV->p_stFaces[F2].falpha[AinB] = SF1.falpha[(BinA + 2) % 3];
		/* Vertex normals  */
		p_GLV->p_stFaces[F1].OriginalPointsNormales[BinA] = SF2.OriginalPointsNormales[(AinB + 2) % 3];
		p_GLV->p_stFaces[F2].OriginalPointsNormales[AinB] = SF1.OriginalPointsNormales[(BinA + 2) % 3];
		/* MARK Faces */
		p_GLV->p_stFaces[F1].ulFlags |= GLV_FLGS_RSV1;
		p_GLV->p_stFaces[F2].ulFlags |= GLV_FLGS_RSV1;
		return 1;
	}
}
ULONG GLV_EqualScalar(GLV_Scalar A , GLV_Scalar B )
{
	GLV_Scalar D;
	D = A - B;
	if (D < 0.0f) D = -D;
	if (D < GLF_PrecMetric) return 1;
	return 0;
}

ULONG GLV_b_IsCompatible(tdst_GLV * p_GLV , ULONG C1 , ULONG C2 )
{
	ULONG I1, I2;
	I1 = I2 = 0xffffffff;
	if (p_GLV->p_stFaces[C1].Nghbr[0] == C2) I1 = 0;
	if (p_GLV->p_stFaces[C1].Nghbr[1] == C2) I1 = 1;
	if (p_GLV->p_stFaces[C1].Nghbr[2] == C2) I1 = 2;
	if (p_GLV->p_stFaces[C2].Nghbr[0] == C1) I2 = 0;
	if (p_GLV->p_stFaces[C2].Nghbr[1] == C1) I2 = 1;
	if (p_GLV->p_stFaces[C2].Nghbr[2] == C1) I2 = 2;
	if ((I1 | I2) == 0xffffffff) return 0;
	if (p_GLV->p_stFaces[C2].ulPolylineNum != p_GLV->p_stFaces[C1].ulPolylineNum) return 0;
	if (p_GLV->p_stFaces[C2].ulSurfaceNumber != p_GLV->p_stFaces[C1].ulSurfaceNumber) return 0;
	if (p_GLV->p_stFaces[C2].ulElementJADE != p_GLV->p_stFaces[C1].ulElementJADE) return 0;
	if (!GLV_IsEqualPlanes(&p_GLV->p_stFaces[C2].Plane, &p_GLV->p_stFaces[C1].Plane)) return 0;
//	if (MATHD_f_DotProduct(&p_GLV->p_stFaces[C2].Plane.Normale,&p_GLV->p_stFaces[C1].Plane.Normale) < 0.999)
	/* UV Comparison */
	if (!GLV_EqualScalar(p_GLV->p_stFaces[C2].UV[(((I2 + 1) % 3) << 1L) + 0] , p_GLV->p_stFaces[C1].UV[(((I1 + 0) % 3) << 1L) + 0])) return 0;
	if (!GLV_EqualScalar(p_GLV->p_stFaces[C2].UV[(((I2 + 1) % 3) << 1L) + 1] , p_GLV->p_stFaces[C1].UV[(((I1 + 0) % 3) << 1L) + 1])) return 0;
	if (!GLV_EqualScalar(p_GLV->p_stFaces[C2].UV[(((I2 + 0) % 3) << 1L) + 0] , p_GLV->p_stFaces[C1].UV[(((I1 + 1) % 3) << 1L) + 0])) return 0;
	if (!GLV_EqualScalar(p_GLV->p_stFaces[C2].UV[(((I2 + 0) % 3) << 1L) + 1] , p_GLV->p_stFaces[C1].UV[(((I1 + 1) % 3) << 1L) + 1])) return 0;//*/
	/* falpha Comparison */
	if (!GLV_EqualScalar(p_GLV->p_stFaces[C2].falpha[(((I2 + 1) % 3) ) + 0] , p_GLV->p_stFaces[C1].falpha[(((I1 + 0) % 3) ) + 0])) return 0;
	if (!GLV_EqualScalar(p_GLV->p_stFaces[C2].falpha[(((I2 + 0) % 3) ) + 0] , p_GLV->p_stFaces[C1].falpha[(((I1 + 1) % 3) ) + 0])) return 0;
	/* Colors Comparison */
/*	if (!GLV_EqualColor(p_GLV->p_stFaces[C2].Colours[(I2 + 1) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 0) % 3])) return 0;
	if (!GLV_EqualColor(p_GLV->p_stFaces[C2].Colours[(I2 + 0) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 1) % 3])) return 0;//*/
	/* Color slope test */
	{
		GLV_Scalar s3[3];
		GLV_Scalar Color[4];
		GLV_Scalar G1[3];
		MATHD_tdst_Vector std;
		tdst_GLV_Plane pl1 , pl2;
		GLV_ULto4Scalar(p_GLV ->p_stFaces[C1].Colours[0], Color);
		G1[0] = Color[0] + Color[1] + Color[2] + Color[3];
		GLV_ULto4Scalar(p_GLV ->p_stFaces[C1].Colours[1], Color);
		G1[1] = Color[0] + Color[1] + Color[2] + Color[3];
		GLV_ULto4Scalar(p_GLV ->p_stFaces[C1].Colours[2], Color);
		G1[2] = Color[0] + Color[1] + Color[2] + Color[3];
		s3 [0] = G1[0];
		s3 [1] = G1[1];
		s3 [2] = G1[2];
		GLV_OPT_GetScalarPlane ( p_GLV , C1 , s3 , &pl1 );

		GLV_ULto4Scalar(p_GLV ->p_stFaces[C2].Colours[0], Color);
		G1[0] = Color[0] + Color[1] + Color[2] + Color[3];
		GLV_ULto4Scalar(p_GLV ->p_stFaces[C2].Colours[1], Color);
		G1[1] = Color[0] + Color[1] + Color[2] + Color[3];
		GLV_ULto4Scalar(p_GLV ->p_stFaces[C2].Colours[2], Color);
		G1[2] = Color[0] + Color[1] + Color[2] + Color[3];
		s3 [0] = G1[0];
		s3 [1] = G1[1];
		s3 [2] = G1[2];
		GLV_OPT_GetScalarPlane ( p_GLV , C2 , s3 , &pl2 );
		MATHD_SubVector(&std , &pl1.Normale , &pl2.Normale);
		if (MATHD_f_NormVector(&std) > 5.0f) return 0;
/*		if (pl1.fDistanceTo0 - pl2.fDistanceTo0 < -2.0f) return 0;
		if (pl1.fDistanceTo0 - pl2.fDistanceTo0 >  2.0f) return 0;*/
	}//*/

	return 1;
}
ULONG GLV_b_IsCompatibleColors(tdst_GLV * p_GLV , ULONG C1 , ULONG C2 )
{
	ULONG I1, I2;
	I1 = I2 = 0xffffffff;
	if (C2 & 0xff000000) return 1;
	if (MATHD_f_DotProduct(&p_GLV->p_stFaces[C1].Plane.Normale,&p_GLV->p_stFaces[C2].Plane.Normale) < 0.999) return 1000;
	if (p_GLV->p_stFaces[C1].Nghbr[0] == C2) I1 = 0;
	if (p_GLV->p_stFaces[C1].Nghbr[1] == C2) I1 = 1;
	if (p_GLV->p_stFaces[C1].Nghbr[2] == C2) I1 = 2;
	if (p_GLV->p_stFaces[C2].Nghbr[0] == C1) I2 = 0;
	if (p_GLV->p_stFaces[C2].Nghbr[1] == C1) I2 = 1;
	if (p_GLV->p_stFaces[C2].Nghbr[2] == C1) I2 = 2;
	if ((I1 | I2) == 0xffffffff) return 0;

	/* Colors Comparison */
	if (!GLV_EqualColor(p_GLV->p_stFaces[C2].Colours[(I2 + 1) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 0) % 3])) return 0;
	if (!GLV_EqualColor(p_GLV->p_stFaces[C2].Colours[(I2 + 0) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 1) % 3])) return 0;//*/
	return 1;
}
ULONG GLV_b_IsCompatibleColors2(tdst_GLV * p_GLV , ULONG C1 , ULONG C2 )
{
	ULONG I1, I2;
	I1 = I2 = 0xffffffff;
	if (C2 & 0xff000000) return 0;
	if (p_GLV->p_stFaces[C1].Nghbr[0] == C2) I1 = 0;
	if (p_GLV->p_stFaces[C1].Nghbr[1] == C2) I1 = 1;
	if (p_GLV->p_stFaces[C1].Nghbr[2] == C2) I1 = 2;
	if (p_GLV->p_stFaces[C2].Nghbr[0] == C1) I2 = 0;
	if (p_GLV->p_stFaces[C2].Nghbr[1] == C1) I2 = 1;
	if (p_GLV->p_stFaces[C2].Nghbr[2] == C1) I2 = 2;
	/* Colors Comparison */
	if (!GLV_EqualColor(p_GLV->p_stFaces[C2].Colours[(I2 + 1) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 0) % 3])) return 0;
	if (!GLV_EqualColor(p_GLV->p_stFaces[C2].Colours[(I2 + 0) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 1) % 3])) return 0;//*/
	return 1;
}
ULONG GLV_b_IsCompatibleColors_NearestBorder(tdst_GLV * p_GLV , ULONG C1 , ULONG C2 )
{
	ULONG I1, I2;
	I1 = I2 = 0xffffffff;
	if (C2 & 0xff000000) return 1;
	if (MATHD_f_DotProduct(&p_GLV->p_stFaces[C1].Plane.Normale,&p_GLV->p_stFaces[C2].Plane.Normale) < 0.999) return 1000;
	if (p_GLV->p_stFaces[C1].Nghbr[0] == C2) I1 = 0;
	if (p_GLV->p_stFaces[C1].Nghbr[1] == C2) I1 = 1;
	if (p_GLV->p_stFaces[C1].Nghbr[2] == C2) I1 = 2;
	if (p_GLV->p_stFaces[C2].Nghbr[0] == C1) I2 = 0;
	if (p_GLV->p_stFaces[C2].Nghbr[1] == C1) I2 = 1;
	if (p_GLV->p_stFaces[C2].Nghbr[2] == C1) I2 = 2;
	if ((I1 | I2) == 0xffffffff) return 0;

	/* Colors Comparison */
	if (!GLV_EqualColor(p_GLV->p_stFaces[C2].Colours[(I2 + 1) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 0) % 3])) return 0;
	if (!GLV_EqualColor(p_GLV->p_stFaces[C2].Colours[(I2 + 0) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 1) % 3])) return 0;//*/
	return 1;
}

void GLV_IsPassive(tdst_GLV * p_GLV , ULONG C1 , ULONG C2 )
{
	ULONG I1, I2;
	I1 = I2 = 0xffffffff;
	if (C2 & 0xff000000) return;
//	if (MATHD_f_DotProduct(&p_GLV->p_stFaces[C1].Plane.Normale,&p_GLV->p_stFaces[C2].Plane.Normale) < 0.999) return 0;
	if (p_GLV->p_stFaces[C1].Nghbr[0] == C2) I1 = 0;
	if (p_GLV->p_stFaces[C1].Nghbr[1] == C2) I1 = 1;
	if (p_GLV->p_stFaces[C1].Nghbr[2] == C2) I1 = 2;
	if (p_GLV->p_stFaces[C2].Nghbr[0] == C1) I2 = 0;
	if (p_GLV->p_stFaces[C2].Nghbr[1] == C1) I2 = 1;
	if (p_GLV->p_stFaces[C2].Nghbr[2] == C1) I2 = 2;
	if ((I1 | I2) == 0xffffffff) return;


	/* Colors Comparison */
	if ((GLV_Inf(p_GLV->p_stFaces[C2].Colours[(I2 + 1) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 0) % 3])== 1) &&
		(GLV_Inf(p_GLV->p_stFaces[C2].Colours[(I2 + 0) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 1) % 3])== 1) &&
		p_GLV->p_stFaces[C2].ulSurfaceNumber == p_GLV->p_stFaces[C1].ulSurfaceNumber)
	{
		p_GLV->p_stFaces[C1].PassivityFlags |= 0xf0;
		p_GLV->p_stFaces[C2].PassivityFlags |= 0xf000;
	}


	if ((GLV_EqualColor(p_GLV->p_stFaces[C2].Colours[(I2 + 1) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 0) % 3])) &&
		(GLV_EqualColor(p_GLV->p_stFaces[C2].Colours[(I2 + 0) % 3] , p_GLV->p_stFaces[C1].Colours[(I1 + 1) % 3])) &&
		p_GLV->p_stFaces[C2].ulSurfaceNumber == p_GLV->p_stFaces[C1].ulSurfaceNumber)
	{
		u32 Blur,BigOr;
		Blur = ((p_GLV->p_stFaces[C1].PassivityFlags & 0xfefe) + (p_GLV->p_stFaces[C2].PassivityFlags & 0xfefe))>>1;
		BigOr = 0;
		if ((p_GLV->p_stFaces[C1].PassivityFlags & 0xff) == 0xff) BigOr |= 0xff;
		if ((p_GLV->p_stFaces[C1].PassivityFlags & 0xff00) == 0xff00) BigOr |= 0xff00;
		p_GLV->p_stFaces[C1].PassivityFlags = Blur | BigOr; // Else don't blur
		BigOr = 0;
		if ((p_GLV->p_stFaces[C2].PassivityFlags & 0xff) == 0xff) BigOr |= 0xff;
		if ((p_GLV->p_stFaces[C2].PassivityFlags & 0xff00) == 0xff00) BigOr |= 0xff00;
		p_GLV->p_stFaces[C2].PassivityFlags = Blur | BigOr; // Else don't blur
	}
}

void GLV_MergeColors(tdst_GLV * p_GLV , ULONG CDest , ULONG CSrc )
{
	ULONG I1, I2;
	I1 = I2 = 0xffffffff;
	if (CSrc & 0xff000000) return;
	if (p_GLV->p_stFaces[CDest].Nghbr[0] == CSrc) I1 = 0;
	if (p_GLV->p_stFaces[CDest].Nghbr[1] == CSrc) I1 = 1;
	if (p_GLV->p_stFaces[CDest].Nghbr[2] == CSrc) I1 = 2;
	if (p_GLV->p_stFaces[CSrc].Nghbr[0] == CDest) I2 = 0;
	if (p_GLV->p_stFaces[CSrc].Nghbr[1] == CDest) I2 = 1;
	if (p_GLV->p_stFaces[CSrc].Nghbr[2] == CDest) I2 = 2;
#ifdef JADEFUSION
	if ((I1 | I2) == 0xffffffff) return;
#else
	if ((I1 | I2) == 0xffffffff) return 0;
#endif
	/* Colors Merge */
	p_GLV->p_stFaces[CDest].Colours[(I1 + 0) % 3] = p_GLV->p_stFaces[CSrc].Colours[(I2 + 1) % 3];
	p_GLV->p_stFaces[CDest].Colours[(I1 + 1) % 3] = p_GLV->p_stFaces[CSrc].Colours[(I2 + 0) % 3];
}


void GLV_BreakLink(tdst_GLV * p_GLV , ULONG C1 , ULONG C2 )
{
	if (p_GLV->p_stFaces[C1].Nghbr[0] == C2) p_GLV->p_stFaces[C1].Nghbr[0] = 0xffffffff;
	if (p_GLV->p_stFaces[C1].Nghbr[1] == C2) p_GLV->p_stFaces[C1].Nghbr[1] = 0xffffffff;
	if (p_GLV->p_stFaces[C1].Nghbr[2] == C2) p_GLV->p_stFaces[C1].Nghbr[2] = 0xffffffff;
	if (p_GLV->p_stFaces[C2].Nghbr[0] == C1) p_GLV->p_stFaces[C2].Nghbr[0] = 0xffffffff;
	if (p_GLV->p_stFaces[C2].Nghbr[1] == C1) p_GLV->p_stFaces[C2].Nghbr[1] = 0xffffffff;
	if (p_GLV->p_stFaces[C2].Nghbr[2] == C1) p_GLV->p_stFaces[C2].Nghbr[2] = 0xffffffff;
}
void GLV_BreakUncompatibleLinks( tdst_GLV * p_GLV )
{
	ULONG Counter, Counter2;
	for (Counter = 0 ; Counter < p_GLV -> ulNumberOfFaces ; Counter ++)
	{
		for (Counter2 = 0 ; Counter2 < 3 ; Counter2++)
		{
			if ( p_GLV ->p_stFaces[Counter].Nghbr[Counter2] < 0xffff)
			{
				if (!GLV_b_IsCompatible(p_GLV , Counter , p_GLV ->p_stFaces[Counter].Nghbr[Counter2] ))
					GLV_BreakLink(p_GLV , Counter , p_GLV ->p_stFaces[Counter].Nghbr[Counter2] );
			}
		}
	}
}
void GLV_BreakUncompatibleLinks_JadeElement( tdst_GLV * p_GLV )
{
	ULONG Counter, Counter2;
	for (Counter = 0 ; Counter < p_GLV -> ulNumberOfFaces ; Counter ++)
	{
		for (Counter2 = 0 ; Counter2 < 3 ; Counter2++)
		{
			if ( p_GLV ->p_stFaces[Counter].Nghbr[Counter2] < 0xffff)
			{
				if (p_GLV ->p_stFaces[Counter].ulElementJADE != p_GLV ->p_stFaces[p_GLV ->p_stFaces[Counter].Nghbr[Counter2]].ulElementJADE)
					GLV_BreakLink(p_GLV , Counter , p_GLV ->p_stFaces[Counter].Nghbr[Counter2] );
			}
		}
	}
}
void GLV_BreakUncompatibleLinks_JadeElement_and_UV( tdst_GLV * p_GLV )
{
	GLD_BlowUpIndexion(  p_GLV , 1 );
	GLD_Weld(p_GLV , GLV_WELD_FCT , 3 );
}
/* This function wlii turn compatible edge to make them shorter */
void GLD_MakeItShorter( tdst_GLV * p_GLV , ULONG ulMode)
/*
ulMode = 0 -> Shorter
ulMode = 1 -> RSV1 (optimization)
ulMode = 2 -> longer
ulMode = 3 -> Avoid Surface = 0

*/
{
	ULONG Res , Counter , Counter2 ,infinite;
	GLD_RemoveIllegalFaces( p_GLV );
	Res = 1;
	infinite = 0;
	while (Res --)
	{
		GLV_ComputeNghbr(p_GLV);
		GLV_BreakUncompatibleLinks(p_GLV);
		for (Counter = 0 ; Counter < p_GLV -> ulNumberOfFaces ; Counter ++)
			p_GLV ->p_stFaces [Counter].ulFlags &= ~GLV_FLGS_RSV1;
		for (Counter = 0 ; Counter < p_GLV -> ulNumberOfFaces ; Counter ++)
		{
			for (Counter2 = 0 ; Counter2 < 3 ; Counter2 ++)
			{
				if (p_GLV ->p_stFaces [Counter].Nghbr[Counter2] < 0xffff)
				{
					if (GLD_TurnEdges_On_2_Faces( p_GLV , Counter , p_GLV ->p_stFaces [Counter].Nghbr[Counter2] , ulMode))
						Res = 4;
				}
			}
		}
		infinite++;
		if (infinite == 10) return;
		if (ulMode == 1) return;
	}
}


void MATHD_InitVector(MATHD_tdst_Vector *VDst, GLV_Scalar x, GLV_Scalar y, GLV_Scalar z)
{
    VDst->x = x;
    VDst->y = y;
    VDst->z = z;
}
void MATHD_AddVector(MATHD_tdst_Vector *VDst, MATHD_tdst_Vector *A, MATHD_tdst_Vector *B)
{
    VDst->x = (A->x + B->x);
    VDst->y = (A->y + B->y);
    VDst->z = (A->z + B->z);
}

void MATHD_SubVector(MATHD_tdst_Vector *VDst, MATHD_tdst_Vector *A, MATHD_tdst_Vector *B)
{
    VDst->x = (A->x - B->x);
    VDst->y = (A->y - B->y);
    VDst->z = (A->z - B->z);
}

void MATHD_CrossProduct(MATHD_tdst_Vector *Dest, MATHD_tdst_Vector *A, MATHD_tdst_Vector *B)
{
    Dest->x = (A->y* B->z) - (A->z* B->y);
    Dest->y = (A->z* B->x) - (A->x* B->z);
    Dest->z = (A->x* B->y) - (A->y* B->x);
}

GLV_Scalar MATHD_f_InvNormVector(MATHD_tdst_Vector *A)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GLV_Scalar   f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    f = (A->x*A->x) + (A->y*A->y) + (A->z*A->z);
    f = 1.0 / (GLV_Scalar)sqrt(f);

    /* FOptInvSqrt(f); vOptInvSqrt(&f); */
    return f;
}

void MATHD_NormalizeVector(MATHD_tdst_Vector *Dest, MATHD_tdst_Vector *A)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GLV_Scalar   fInvNorm;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    fInvNorm = MATHD_f_InvNormVector(A);
    Dest->x = (A->x * fInvNorm);
    Dest->y = (A->y * fInvNorm);
    Dest->z = (A->z * fInvNorm);
}


void MATHD_ScaleVector(MATHD_tdst_Vector *Dest, MATHD_tdst_Vector *A, GLV_Scalar f)
{
    Dest->x = (A->x* f);
    Dest->y = (A->y* f);
    Dest->z = (A->z* f);
}
void MATHD_ScaleEqualVector(MATHD_tdst_Vector *A, GLV_Scalar f)
{
    A->x *= f;
    A->y *= f;
    A->z *= f;
}

GLV_Scalar MATHD_f_SqrNormVector(MATHD_tdst_Vector *A)
{
    return(A->x * A->x + A->y * A->y + A->z * A->z);
}
GLV_Scalar MATHD_f_NormVector(MATHD_tdst_Vector *_pst_V)
{
    return((GLV_Scalar)sqrt(MATHD_f_SqrNormVector(_pst_V)));
}
GLV_Scalar MATHD_f_SqrVector(MATHD_tdst_Vector *A)
{
    return((A->x*A->x) + (A->y*A->y) + (A->z*A->z));
}
void MATHD_SetNormVector(MATHD_tdst_Vector *Dest, MATHD_tdst_Vector *Src, GLV_Scalar _f_Norm)
{
    MATHD_NormalizeVector(Dest, Src);
	Dest->x*=_f_Norm;
	Dest->y*=_f_Norm;
	Dest->z*=_f_Norm;
}

char MATHD_b_NulVector(MATHD_tdst_Vector *pst_Vector)
{
    if(pst_Vector->x != 0.0f) return 0;
    if(pst_Vector->y != 0.0f) return 0;
    if(pst_Vector->z != 0.0f) return 0;
    return 1;
}


void MATHD_NormalizeAnyVector(MATHD_tdst_Vector *Dest, MATHD_tdst_Vector *A)
{
    if(MATHD_b_NulVector(A))
	{
        MATHD_InitVector(Dest,0.0,0.0,0.0);
	}
    else
        MATHD_NormalizeVector(Dest, A);
}
void MATHD_AddScaleVector
(
    MATHD_tdst_Vector    *Dst,
    MATHD_tdst_Vector    *A,
    MATHD_tdst_Vector    *B,
    GLV_Scalar               f
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATHD_tdst_Vector    st_Tmp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATHD_ScaleVector(&st_Tmp, B, f);
    MATHD_AddVector(Dst, A, &st_Tmp);
}
void MATHD_BlendVector
(
    MATHD_tdst_Vector    *_pst_Dest,
    MATHD_tdst_Vector    *_pst_1,
    MATHD_tdst_Vector    *_pst_2,
    GLV_Scalar               _f_Factor
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GLV_Scalar   f_b;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    f_b = 1.0 - _f_Factor;
    _pst_Dest->x = (_pst_2->x * _f_Factor) + (_pst_1->x * f_b);
    _pst_Dest->y = (_pst_2->y * _f_Factor) + (_pst_1->y * f_b);
    _pst_Dest->z = (_pst_2->z * _f_Factor) + (_pst_1->z * f_b);
}
void MATHD_To_MATH(MATH_tdst_Vector *p_A ,MATHD_tdst_Vector *p_B )
{
	p_A->x = (float)p_B->x;
	p_A->y = (float)p_B->y;
	p_A->z = (float)p_B->z;
}
void MATH_To_MATHD(MATHD_tdst_Vector *p_A ,MATH_tdst_Vector *p_B )
{
	p_A->x = (GLV_Scalar)p_B->x;
	p_A->y = (GLV_Scalar)p_B->y;
	p_A->z = (GLV_Scalar)p_B->z;
}
void MATHD_AbsoluteVector(MATHD_tdst_Vector *p_a)
{
	if (p_a->x < 0.0) p_a->x = -p_a->x;
	if (p_a->y < 0.0) p_a->y = -p_a->y;
	if (p_a->z < 0.0) p_a->z = -p_a->z;
}
/*(
	MATHD_tdst_Vector	*_pst_M,
	MATHD_tdst_Vector	*_pst_A,
	MATHD_tdst_Vector	*_pst_B,
	MATHD_tdst_Vector	*_pst_C,
	MATHD_tdst_Vector	*_pst_N
)*/
int GLV_FAST_PointInTriangle(tdst_GLV *p_stGLV , ULONG F , MATHD_tdst_Vector *p_P3D)
{
	MATHD_tdst_Vector	st_VectMA, st_VectMB, st_VectMC , *_pst_N;
	GLV_Scalar			_fVal;

	MATHD_SubVector(&st_VectMA, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[0]].P3D, p_P3D);
	MATHD_SubVector(&st_VectMB, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[1]].P3D, p_P3D);
	MATHD_SubVector(&st_VectMC, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[2]].P3D, p_P3D);
	_pst_N = &p_stGLV->p_stFaces[F].Plane.Normale;

	if (fabs(_pst_N->x) >= Cf_InvSqrt3)
	{
		_fVal = st_VectMA.y * st_VectMB.z - st_VectMA.z * st_VectMB.y;
		if(ulDifferentSign(_pst_N->x, _fVal))
			return 0;
		else
		{
			_fVal = st_VectMB.y * st_VectMC.z - st_VectMB.z * st_VectMC.y;
			if(ulDifferentSign(_pst_N->x, _fVal))
				return 0;
			else
			{
				_fVal = st_VectMC.y * st_VectMA.z - st_VectMC.z * st_VectMA.y;
				if (ulDifferentSign(_pst_N->x, _fVal))
					return 0;
				else
					return 1;
			}
		}
	}
	else
	{
		if(fabs(_pst_N->y) >= Cf_InvSqrt3)
		{
			_fVal = st_VectMA.z * st_VectMB.x - st_VectMA.x * st_VectMB.z;
			if(ulDifferentSign(_pst_N->y, _fVal))
				return 0;
			else
			{
				_fVal = st_VectMB.z * st_VectMC.x - st_VectMB.x * st_VectMC.z;
				if (ulDifferentSign(_pst_N->y, _fVal))
					return 0;
				else
				{
					_fVal = st_VectMC.z * st_VectMA.x - st_VectMC.x * st_VectMA.z;
					if (ulDifferentSign(_pst_N->y, _fVal))
						return 0;
					else
						return 1;
				}
			}
		}
		else
		{
			_fVal = st_VectMA.x * st_VectMB.y - st_VectMA.y * st_VectMB.x;
			if(ulDifferentSign(_pst_N->z, _fVal))
				return 0;
			else
			{
				_fVal = st_VectMB.x * st_VectMC.y - st_VectMB.y * st_VectMC.x;
				if(ulDifferentSign(_pst_N->z, _fVal))
					return 0;
				else
				{
					_fVal = st_VectMC.x * st_VectMA.y - st_VectMC.y * st_VectMA.x;
					if (ulDifferentSign(_pst_N->z, _fVal))
						return 0;
					else
						return 1;
				}
			}
		}
	}
}
//#undef GLV_FastPit
int GLV_IsPointInTriangle(tdst_GLV *p_stGLV , ULONG F , MATHD_tdst_Vector *p_P3D)
{
#ifndef GLV_FastPit
	ULONG Counter , Res , Res2, Resb;
	MATHD_tdst_Vector st1,st2,stCP;
	GLV_Scalar NN;
#endif
	GLV_Scalar Norm;
	Norm = MATHD_f_DotProduct(&p_stGLV->p_stFaces[F].Plane.Normale , p_P3D) - p_stGLV->p_stFaces[F].Plane.fDistanceTo0;
	if ((Norm >= GLF_PrecMetric ) || (Norm <= -GLF_PrecMetric ))
		return 0;
#ifdef GLV_FastPit 
	return GLV_FAST_PointInTriangle(p_stGLV , F , p_P3D);
#else
	Res = 0;
	Resb = 0;
	Res2 = 0;
	for (Counter = 0 ; Counter < 3 ; Counter ++)
	{
		MATHD_SubVector(&st1 , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[(Counter + 1) % 3]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[Counter]].P3D);
		MATHD_SubVector(&st2 , p_P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[Counter]].P3D);
		NN = MATHD_f_NormVector(&st1);
		MATHD_NormalizeVector(&stCP , &st1 );
		Norm = MATHD_f_DotProduct(&stCP , &st2);
		if ((Norm >= -GLF_PrecMetric) && (Norm <= NN + GLF_PrecMetric)) Res2++;
		MATHD_SubVector(&st2 , p_P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[Counter]].P3D);
		MATHD_CrossProduct(&stCP , &st1 , &p_stGLV->p_stFaces[F].Plane.Normale);
		MATHD_NormalizeVector(&stCP , &stCP );
		Norm = MATHD_f_DotProduct(&stCP , &st2);
		if (Norm > -GLF_PrecMetric) Res ++;
		if (Norm < GLF_PrecMetric) Resb ++;//*/
/*		if (Norm > 0.0f) Res ++;
		if (Norm < 0.0f) Resb ++;//*/
		
	}
	if (Res2 < 2) 
		return 0;//*/
	if ((Res == 3) || (Resb == 3)) 
		return 1;
	else
		return 0;
#endif
}
GLV_Scalar DistanceToPlane(tdst_GLV_Plane *p , MATHD_tdst_Vector *p_P)
{
	return MATHD_f_DotProduct(p_P , &p->Normale) - p->fDistanceTo0;
}
int GLV_PointFromEdgeTouchFace(tdst_GLV_Face *p_FT , tdst_GLV_Face *p_FTSourceIfExist , MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_P2, MATHD_tdst_Vector *p_PR)
{
	GLV_Scalar  D1,D2,D3;
	MATHD_tdst_Vector *p_PSWP;
	if (p_P1->x < p_P2->x)
	{
		p_PSWP = p_P2;
		p_P2 = p_P1;
		p_P1 = p_PSWP; 
	}
	D1 = MATHD_f_DotProduct(p_P1 , &p_FT -> Plane.Normale) - p_FT->Plane.fDistanceTo0;
	D2 = MATHD_f_DotProduct(p_P2 , &p_FT -> Plane.Normale) - p_FT->Plane.fDistanceTo0;
	if ((D1 > 0.0f) && (D2 > 0.0f)) return 0;
	if ((D1 < 0.0f) && (D2 < 0.0f)) return 0;
	if (D1 == D2) return 0;
	D3 = (0.0f - D1) / (D2 - D1);
	if (D3 >= 1.0f) D3 = 1.0f;
	if (D3 <= 0.0f) D3 = 0.0f;
	MATHD_BlendVector(p_PR,p_P1,p_P2,D3);
	return 1;

	if ((D1 > -GLF_PrecMetric) && (D2 > -GLF_PrecMetric)) return 0;
	if ((D1 < GLF_PrecMetric) && (D2 < GLF_PrecMetric)) return 0;
	if (D1 == D2) return 0;
	D3 = (0.0f - D1) / (D2 - D1);
	if (D3 >= 1.0f) return 0;
	if (D3 <= 0.0f) return 0;
	MATHD_BlendVector(p_PR,p_P1,p_P2,D3);
	if (D1 < 0.0f) D1 = -D1;
	if (D2 < 0.0f) D2 = -D2;
	if ((D1 < GLF_PrecMetric) && (D2 < GLF_PrecMetric)) return 0;
	/* Converge */
	return 1;
}

int GLV_PointTouchPlane(tdst_GLV_Face *p_FT , MATHD_tdst_Vector *p_P1 )
{
	GLV_Scalar  D1;
	D1 = MATHD_f_DotProduct(p_P1 , &p_FT -> Plane.Normale) - p_FT->Plane.fDistanceTo0;
	if ((D1 > -GLF_PrecMetric) && (D1 < GLF_PrecMetric)) return 1;
	return 0;
}


int GLV_IsColinear( MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_P2 , MATHD_tdst_Vector *p_P3)
{
	MATHD_tdst_Vector S1 , S2 , SR;
	MATHD_SubVector(&S1 , p_P2 , p_P1);
	MATHD_SubVector(&S2 , p_P3 , p_P2);
	if (MATHD_f_SqrNormVector(&S1) < GLF_PrecMetric) return 1;
	if (MATHD_f_SqrNormVector(&S2) < GLF_PrecMetric) return 1;
	MATHD_NormalizeVector(&S1 , &S1 );
	MATHD_NormalizeVector(&S2 , &S2 );
	MATHD_CrossProduct(&SR, &S1 , &S2);
	if (MATHD_f_SqrNormVector(&SR) < (GLF_PrecMetric * 10.0f))
		return 1;
	else
		return 0;
}

int GLV_IsBetween( MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_PM , MATHD_tdst_Vector *p_P2)
{
	MATHD_tdst_Vector S1 , S2 ;
	MATHD_SubVector(&S1 , p_P2 , p_P1);
	MATHD_SubVector(&S2 , p_PM , p_P1);
	if (MATHD_f_DotProduct(&S1,&S2) <= GLF_PrecMetric) return 0;
	MATHD_SubVector(&S1 , p_P1 , p_P2);
	MATHD_SubVector(&S2 , p_PM , p_P2);
	if (MATHD_f_DotProduct(&S1,&S2) <= GLF_PrecMetric) return 0;
	return 1;
}

int GLV_IsEdgeTouchFace(tdst_GLV *p_stGLV , ULONG F , tdst_GLV_Face *p_FTSourceIfExist , MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_P2 , GLV_Scalar *Distance)
{
	tdst_GLV_Face *p_FT;
	GLV_Scalar D1,D2;
	MATHD_tdst_Vector stProjPoint;

	p_FT = p_stGLV -> p_stFaces + F;

	/* Fast Culling */

	D1 = MATHD_f_DotProduct(p_P1 , &p_FT -> Plane.Normale) - p_FT->Plane.fDistanceTo0;
	D2 = MATHD_f_DotProduct(p_P2 , &p_FT -> Plane.Normale) - p_FT->Plane.fDistanceTo0;
	if ((D1 > GLF_PrecMetric) && (D2 > GLF_PrecMetric)) return 0;
	if ((D1 < GLF_PrecMetric) && (D2 < -GLF_PrecMetric)) return 0;

	{
		MATHD_tdst_Vector Min,Max,MinE,MaxE;
		GLV_MinMaxFace	(p_stGLV , F , &Min,&Max);
		GLV_MinMaxEdge(p_P1, p_P2, &MinE , &MaxE );
		if (!GLV_IsBoxTouchBox(&Min , &Max,&MinE, &MaxE)) 
			return 0;
	}

	/* Real culling */


	if (GLV_PointFromEdgeTouchFace(p_FT , p_FTSourceIfExist , p_P1 , p_P2, &stProjPoint))
	{
		if (GLV_IsPointInTriangle(p_stGLV , F , &stProjPoint))
		{
			if (Distance)
			{
				*Distance = MATHD_f_Distance(p_P1 , &stProjPoint);
			}
			return 1 ;
		}
		else 
			return 0;
	} else 
		return 0;
}

void		GLV_BUG(ULONG ulLevel , char *String)
{
	char text[1024];
	if (ulLevel)
	{
	sprintf(text, "GLV BUG(lvl %d):%s ",ulLevel , String);
	LINK_PrintStatusMsg(text);
	} else
		LINK_PrintStatusMsg(String);
}

tdst_GLV *	GLV_Duplicate		(tdst_GLV *	p_stGLV)
{
	tdst_GLV *p_NewGLV;
	ULONG Counter;
	p_NewGLV = NewGLV();
	GLV_SetNumbers(p_NewGLV , p_stGLV ->ulNumberOfPoints , p_stGLV->ulNumberOfEdges , p_stGLV->ulNumberOfFaces , 7);
	Counter = p_stGLV->ulNumberOfFaces;
	while (Counter--)
		p_NewGLV ->p_stFaces[Counter] = p_stGLV ->p_stFaces[Counter];
	Counter = p_stGLV->ulNumberOfEdges;
	while (Counter--)
		p_NewGLV ->p_stEdges[Counter] = p_stGLV ->p_stEdges[Counter];
	Counter = p_stGLV->ulNumberOfPoints;
	while (Counter--)
		p_NewGLV ->p_stPoints[Counter] = p_stGLV ->p_stPoints[Counter];
	GLV_COPY_VARS	(p_NewGLV , p_stGLV);

	if (p_stGLV->p_FormFactors)
	{
		p_NewGLV->p_FormFactors = (ULONG**)GLV_ALLOC(3l * 4l * p_stGLV ->ulNumberOfFaces);
		Counter = p_stGLV->ulNumberOfFaces * 3l;
		while (Counter--) 
		{
			if (p_stGLV->p_FormFactors[Counter])
			{
			p_NewGLV->p_FormFactors[Counter] = (ULONG*)GLV_ALLOC(8l * *p_stGLV->p_FormFactors[Counter] + 4l);
			memcpy(p_NewGLV->p_FormFactors[Counter] , p_stGLV->p_FormFactors[Counter] , 8l * *p_stGLV->p_FormFactors[Counter] + 4l);
			} else 
				p_NewGLV->p_FormFactors[Counter] = NULL;
		}
	}
	return p_NewGLV;
}


tdst_GLV *	GLV_Create	(LIGHT_tdst_Light *p_SourceLight , MATH_tdst_Matrix	*p_stMatrix)
{
	tdst_GLV *p_NewGLV;
	p_NewGLV = NewGLV();
	p_NewGLV->ulNumberOfLight = 0;
	return p_NewGLV;
}

void GLV_Destroy(tdst_GLV *p_GLV)
{
#ifdef JADEFUSION
	if (p_GLV ->p_OBV) GLV_FREE(p_GLV ->p_OBV);
#endif	
	if (p_GLV ->p_stEdges) GLV_FREE(p_GLV ->p_stEdges);
	if (p_GLV ->p_stPoints) GLV_FREE(p_GLV ->p_stPoints);
	if (p_GLV ->p_stFaces) GLV_FREE(p_GLV ->p_stFaces);
	if (p_GLV ->p_Lights) GLV_FREE(p_GLV ->p_Lights);
	if (p_GLV ->p_FormFactors)
	{
		ULONG Counter;
		Counter = p_GLV ->ulNumberOfFaces * 3l;
		while (Counter--) if (p_GLV ->p_FormFactors[Counter]) GLV_FREE(p_GLV ->p_FormFactors[Counter]);
		GLV_FREE(p_GLV->p_FormFactors);
		p_GLV->p_FormFactors = NULL;
	}
	GLV_FREE(p_GLV);
}

void GLV_AddGLV (tdst_GLV *p_Dst,tdst_GLV *p_Src)
{
	ULONG NP,NT,NE,Counter;
	NP = p_Dst->ulNumberOfPoints;
	NT = p_Dst->ulNumberOfFaces ;
	NE = p_Dst->ulNumberOfEdges;
	GLV_SetNumbers(p_Dst , p_Dst->ulNumberOfPoints + p_Src->ulNumberOfPoints , p_Dst->ulNumberOfEdges + p_Src->ulNumberOfEdges , p_Dst->ulNumberOfFaces + p_Src->ulNumberOfFaces , 7/* Points + triangles + Edges*/);
	Counter = p_Src->ulNumberOfPoints;
	while (Counter--)
	{
		p_Dst->p_stPoints[Counter + NP] = p_Src->p_stPoints[Counter];
	}
	Counter = p_Src->ulNumberOfFaces;
	while (Counter--)
	{
		p_Dst->p_stFaces[Counter + NT] = p_Src->p_stFaces[Counter];
		p_Dst->p_stFaces[Counter + NT].Index[0] += NP;
		p_Dst->p_stFaces[Counter + NT].Index[1] += NP;
		p_Dst->p_stFaces[Counter + NT].Index[2] += NP;
	}
	Counter = p_Src->ulNumberOfEdges;
	while (Counter--)
	{
		p_Dst->p_stEdges[Counter + NE] = p_Src->p_stEdges[Counter];
		p_Dst->p_stEdges[Counter + NE].Index[0] += NP;
		p_Dst->p_stEdges[Counter + NE].Index[1] += NP;
	}
}
void GLV_ComputeNghbr(tdst_GLV *p_GLV)
{
	ULONG *p_NumPerPoint,*p_BasePerPoint,*p_TrianglePerPoint,ulLoc,ulInt;
	tdst_GLV_Face *p_FT,*p_LT;
	p_NumPerPoint = (ULONG*)GLV_ALLOC(p_GLV->ulNumberOfPoints * 4L);
	L_memset(p_NumPerPoint , 0 ,p_GLV->ulNumberOfPoints * 4L);
	p_BasePerPoint = (ULONG*)GLV_ALLOC(p_GLV->ulNumberOfPoints * 4L);
	L_memset(p_BasePerPoint , 0 ,p_GLV->ulNumberOfPoints * 4L);
	
	/* Compute n T -> P */
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		p_NumPerPoint[p_FT->Index[0]]++;
		p_NumPerPoint[p_FT->Index[1]]++;
		p_NumPerPoint[p_FT->Index[2]]++;

		p_FT ->Nghbr[0] = -1;
		p_FT ->Nghbr[1] = -1;
		p_FT ->Nghbr[2] = -1;
		p_FT++;
	}
	ulLoc = 0;
	ulInt = 0;
	while (ulLoc < p_GLV->ulNumberOfPoints) 
	{
		p_BasePerPoint[ulLoc] = ulInt;
		ulInt += p_NumPerPoint[ulLoc];
		p_NumPerPoint[ulLoc++] = 0;
	}
	p_TrianglePerPoint = (ULONG*)GLV_ALLOC(ulInt * 4L);
	L_memset(p_TrianglePerPoint, 0 ,ulInt * 4L);

	/* enum face in p_TrianglePerPoint */
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	ulInt = 0;
	while (p_FT < p_LT)
	{
		p_TrianglePerPoint[p_BasePerPoint[p_FT->Index[0]] + (p_NumPerPoint[p_FT->Index[0]])++] = ulInt;
		p_TrianglePerPoint[p_BasePerPoint[p_FT->Index[1]] + (p_NumPerPoint[p_FT->Index[1]])++] = ulInt;
		p_TrianglePerPoint[p_BasePerPoint[p_FT->Index[2]] + (p_NumPerPoint[p_FT->Index[2]])++] = ulInt;
		ulInt++;
		p_FT++;
	}
	 
	/* Unify */
	ulLoc = 0;
	ulInt = 0;
	while (ulLoc < p_GLV->ulNumberOfPoints) 
	{
		ULONG i,j;
		if (p_NumPerPoint[ulLoc] != 0)
		{
			for ( i = 0 ; i < p_NumPerPoint[ulLoc] - 1 ; i++ )
				for ( j = 1 ; j < p_NumPerPoint[ulLoc] ; j++ )
					GLV_Unify2Face(p_GLV , p_TrianglePerPoint[p_BasePerPoint[ulLoc] + i] , p_TrianglePerPoint[p_BasePerPoint[ulLoc] + j]);
		}
		ulLoc++;
	}

	GLV_FREE(p_NumPerPoint);
	GLV_FREE(p_TrianglePerPoint);
	GLV_FREE(p_BasePerPoint);
}
GLV_Scalar  GLV_DistancePointPoint( MATHD_tdst_Vector *p_E1 , MATHD_tdst_Vector *p_E2 )
{
	MATHD_tdst_Vector stE12 ;
	MATHD_SubVector(&stE12 , p_E2 , p_E1 );
	return (MATHD_f_NormVector(&stE12));
}
GLV_Scalar  GLV_DistanceEdgePoint( MATHD_tdst_Vector *p_E1 , MATHD_tdst_Vector *p_E2 , MATHD_tdst_Vector *p_P)
{
	GLV_Scalar Ret,DP;
	MATHD_tdst_Vector stE12 , stEP1;
	MATHD_SubVector(&stE12 , p_E2 , p_E1 );
	MATHD_SubVector(&stEP1 , p_P  , p_E1 );
	Ret = MATHD_f_NormVector(&stE12);
	MATHD_NormalizeVector(&stE12 , &stE12);
	DP = MATHD_f_DotProduct(&stE12 , &stEP1);
	if (DP < 0.0f)
	{
		/* return distance E1 <-> P */
		return GLV_DistancePointPoint( p_E1 , p_P );
	}
	if (DP > Ret)
	{
		/* return distance E2 <-> P */
		return GLV_DistancePointPoint( p_E2 , p_P );
	}
	/* Compute the nearest point */
	MATHD_AddScaleVector(&stEP1 , p_E1 , &stE12 , DP * Ret);
	return GLV_DistancePointPoint( &stEP1 , p_P );
	
}
GLV_Scalar GLV_GetSurf(tdst_GLV *p_GLV,tdst_GLV_Face *p_FT)
{
	MATHD_tdst_Vector	st_Vect1,st_Vect2 , Norm;
	MATHD_SubVector(&st_Vect1, &p_GLV->p_stPoints[p_FT->Index[0]].P3D, &p_GLV->p_stPoints[p_FT->Index[1]].P3D);
	MATHD_SubVector(&st_Vect2, &p_GLV->p_stPoints[p_FT->Index[0]].P3D, &p_GLV->p_stPoints[p_FT->Index[2]].P3D);
	MATHD_CrossProduct(&Norm, &st_Vect1, &st_Vect2);
	return MATHD_f_NormVector(&Norm) * 0.5f;
}
void GLV_ComputeNormale(tdst_GLV *p_GLV,tdst_GLV_Face *p_FT)
{
	MATHD_tdst_Vector	st_Vect1,st_Vect2;
	MATHD_SubVector(&st_Vect1, &p_GLV->p_stPoints[p_FT->Index[0]].P3D, &p_GLV->p_stPoints[p_FT->Index[1]].P3D);
	MATHD_SubVector(&st_Vect2, &p_GLV->p_stPoints[p_FT->Index[0]].P3D, &p_GLV->p_stPoints[p_FT->Index[2]].P3D);
	MATHD_CrossProduct(&p_FT->Plane.Normale, &st_Vect1, &st_Vect2);
	MATHD_NormalizeAnyVector(&p_FT->Plane.Normale, &p_FT->Plane.Normale);
	p_FT->Plane.fDistanceTo0 = MATHD_f_DotProduct(&p_FT->Plane.Normale , &p_GLV->p_stPoints[p_FT->Index[0]].P3D);
}
void GLV_ComputeNormales(tdst_GLV *p_GLV)
{
	tdst_GLV_Face *p_FT,*p_LT;
	/* Compute n T -> P */
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT) GLV_ComputeNormale(p_GLV,p_FT++);
}
ULONG GLV_FlicTst(tdst_GLV *p_GLV,ULONG FNum)
{
	tdst_GLV_Plane stP;
	GLV_Scalar Test;
	ULONG Ret;
	stP = p_GLV->p_stFaces[FNum].Plane;
	GLV_ComputeNormale(p_GLV,p_GLV->p_stFaces+FNum);
	Ret = 0;
	Test = MATHD_f_DotProduct(&stP.Normale,&p_GLV->p_stFaces[FNum].Plane.Normale);
	if (Test <= 0.0f)
		Ret = 1;
	p_GLV->p_stFaces[FNum].Plane = stP;
	return Ret;
}
void GLV_ComputeNormale_NoNormalize(tdst_GLV *p_GLV,tdst_GLV_Face *p_FT)
{
	MATHD_tdst_Vector	st_Vect1,st_Vect2;
	MATHD_SubVector(&st_Vect1, &p_GLV->p_stPoints[p_FT->Index[0]].P3D, &p_GLV->p_stPoints[p_FT->Index[1]].P3D);
	MATHD_SubVector(&st_Vect2, &p_GLV->p_stPoints[p_FT->Index[0]].P3D, &p_GLV->p_stPoints[p_FT->Index[2]].P3D);
	MATHD_CrossProduct(&p_FT->Plane.Normale, &st_Vect1, &st_Vect2);
	p_FT->Plane.fDistanceTo0 = MATHD_f_DotProduct(&p_FT->Plane.Normale , &p_GLV->p_stPoints[p_FT->Index[0]].P3D);
}
ULONG GLV_FlicTst_With_0(tdst_GLV *p_GLV,ULONG FNum)
{
	tdst_GLV_Plane stP;
	GLV_Scalar Test;
	ULONG Ret;
	stP = p_GLV->p_stFaces[FNum].Plane;
	GLV_ComputeNormale_NoNormalize(p_GLV,p_GLV->p_stFaces+FNum);
	Test = MATHD_f_NormVector(&p_GLV->p_stFaces[FNum].Plane.Normale);
	Ret = 0;
	if (Test <= GLF_PrecMetric)
		Ret = 1;
	else
	{
		MATHD_NormalizeAnyVector(&p_GLV->p_stFaces[FNum].Plane.Normale, &p_GLV->p_stFaces[FNum].Plane.Normale);
		Test = MATHD_f_DotProduct(&stP.Normale,&p_GLV->p_stFaces[FNum].Plane.Normale);
		if (Test <= 0.0f)
			Ret = 1;
	}
	p_GLV->p_stFaces[FNum].Plane = stP;
	return Ret;
}

tdst_GLV *NewGLV()
{
	tdst_GLV *p_Ret;
	p_Ret = (tdst_GLV*)GLV_ALLOC(sizeof (tdst_GLV));
	L_memset(p_Ret , 0 , sizeof (tdst_GLV));
	return p_Ret;
}
void GLV_Verify(tdst_GLV *p_GLV)
{
	tdst_GLV_Face *p_FT,*p_LT;
	ULONG Counter;
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		for (Counter = 0 ; Counter < 3 ; Counter ++)
		{
		if (p_FT->Index[Counter] >= p_GLV->ulNumberOfPoints)
			GLV_BUG(4, "Verify");
		if ((p_FT->Nghbr[Counter] != -1) && (p_FT->Nghbr[Counter] != -2) && (p_FT->Nghbr[Counter] >= p_GLV->ulNumberOfFaces))
			GLV_BUG(4, "Verify on neightbourgs ");
		}
		p_FT++;
	}
	for (Counter=0;Counter<p_GLV->ulNumberOfEdges;Counter++)
	{
		if (p_GLV->p_stEdges[Counter].Index[0] >= p_GLV->ulNumberOfPoints)
			GLV_BUG(4, "Verify");
		if (p_GLV->p_stEdges[Counter].Index[1] >= p_GLV->ulNumberOfPoints)
			GLV_BUG(4, "Verify");
	}

}
void GLV_Normalize_GLV(tdst_GLV *p_GLV , ULONG Expand)
{
	/*
	MATHD_tdst_Vector	Pos,OriginalScale;
	ULONG Counter;
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		for (Counter = 0 ; Counter < 3 ; Counter ++)
		{
		if (p_FT->Index[Counter] >= p_GLV->ulNumberOfPoints)
			GLV_BUG(4, "Verify");
		if ((p_FT->Nghbr[Counter] != -1) && (p_FT->Nghbr[Counter] != -2) && (p_FT->Nghbr[Counter] >= p_GLV->ulNumberOfFaces))
			GLV_BUG(4, "Verify on neightbourgs ");
		}
		p_FT++;
	}
	for (Counter=0;Counter<p_GLV->ulNumberOfEdges;Counter++)
	{
		if (p_GLV->p_stEdges[Counter].Index[0] >= p_GLV->ulNumberOfPoints)
			GLV_BUG(4, "Verify");
		if (p_GLV->p_stEdges[Counter].Index[1] >= p_GLV->ulNumberOfPoints)
			GLV_BUG(4, "Verify");
	}
*/
}




void		GLV_COPY_VARS		(tdst_GLV *	p_stGLVDst , tdst_GLV *	p_stGLVSrc)
{
	ULONG LightCounter;


	LightCounter = p_stGLVSrc->ulNumberOfLight;
	if (p_stGLVDst->p_Lights) GLV_FREE(p_stGLVDst->p_Lights);
	p_stGLVDst->p_Lights = (tdst_LightDescription*)GLV_ALLOC(sizeof(tdst_LightDescription) * p_stGLVSrc->ulNumberOfLight);
	L_memcpy(p_stGLVDst->p_Lights , p_stGLVSrc->p_Lights , sizeof(tdst_LightDescription) * p_stGLVSrc->ulNumberOfLight);
	while (LightCounter--)
	{
		p_stGLVDst->p_Lights[LightCounter] = p_stGLVSrc->p_Lights[LightCounter];
	}
	p_stGLVDst->ulNumberOfLight = p_stGLVSrc->ulNumberOfLight;

	if (p_stGLVDst->p_OBV) GLV_FREE(p_stGLVDst->p_OBV);
	p_stGLVDst->p_OBV = (tdst_ObjectBDVolume*)GLV_ALLOC(sizeof(tdst_ObjectBDVolume) * p_stGLVSrc->ulNumberOfOBV);
	L_memcpy(p_stGLVDst->p_OBV , p_stGLVSrc->p_OBV , sizeof(tdst_ObjectBDVolume) * p_stGLVSrc->ulNumberOfOBV);
	p_stGLVDst->ulNumberOfOBV = p_stGLVSrc->ulNumberOfOBV ;


}

void GLV_RecursiveSetChannel(tdst_GLV *p_stGLV,u32 FaceNum,u32 Value)
{
	if (p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber & 0x00ffffff) return;
	p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber = Value;
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[0] & 0xff000000)) GLV_RecursiveSetChannel(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[0],Value);
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[1] & 0xff000000)) GLV_RecursiveSetChannel(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[1],Value);
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[2] & 0xff000000)) GLV_RecursiveSetChannel(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[2],Value);
}

void GLV_RecursiveSetChannel_CC(tdst_GLV *p_stGLV,u32 FaceNum,u32 Value)
{
	if (p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber & 0x00ffffff) return;
	p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber = Value;
	if ((!(p_stGLV->p_stFaces[FaceNum].Nghbr[0] & 0xff000000)) && GLV_b_IsCompatibleColors2(p_stGLV , FaceNum , p_stGLV->p_stFaces[FaceNum].Nghbr[0] )) GLV_RecursiveSetChannel_CC(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[0],Value);
	if ((!(p_stGLV->p_stFaces[FaceNum].Nghbr[1] & 0xff000000)) && GLV_b_IsCompatibleColors2(p_stGLV , FaceNum , p_stGLV->p_stFaces[FaceNum].Nghbr[1] )) GLV_RecursiveSetChannel_CC(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[1],Value);
	if ((!(p_stGLV->p_stFaces[FaceNum].Nghbr[2] & 0xff000000)) && GLV_b_IsCompatibleColors2(p_stGLV , FaceNum , p_stGLV->p_stFaces[FaceNum].Nghbr[2] )) GLV_RecursiveSetChannel_CC(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[2],Value);
}

u32 GLV_RecursiveGetChannel(tdst_GLV *p_stGLV,u32 FaceNum)
{
	u32 ReturnV;
	if (p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber & 0x20000000) return 0;
	p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber |= 0x20000000;
	ReturnV = p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber;
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[0] & 0xff000000)) ReturnV |= GLV_RecursiveGetChannel(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[0]);
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[1] & 0xff000000)) ReturnV |= GLV_RecursiveGetChannel(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[1]);
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[2] & 0xff000000)) ReturnV |= GLV_RecursiveGetChannel(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[2]);
	return ReturnV & 0xff000000;
}

u32 GLV_RecursiveGetChannel_CC(tdst_GLV *p_stGLV,u32 FaceNum)
{
	u32 ReturnV;
	if (p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber & 0x20000000) return 0;
	p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber |= 0x20000000;
	ReturnV = p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber;
	if ((!(p_stGLV->p_stFaces[FaceNum].Nghbr[0] & 0xff000000)) && GLV_b_IsCompatibleColors2(p_stGLV , FaceNum , p_stGLV->p_stFaces[FaceNum].Nghbr[0] ))  ReturnV |= GLV_RecursiveGetChannel(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[0]);
	if ((!(p_stGLV->p_stFaces[FaceNum].Nghbr[1] & 0xff000000)) && GLV_b_IsCompatibleColors2(p_stGLV , FaceNum , p_stGLV->p_stFaces[FaceNum].Nghbr[1] ))  ReturnV |= GLV_RecursiveGetChannel(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[1]);
	if ((!(p_stGLV->p_stFaces[FaceNum].Nghbr[2] & 0xff000000)) && GLV_b_IsCompatibleColors2(p_stGLV , FaceNum , p_stGLV->p_stFaces[FaceNum].Nghbr[2] ))  ReturnV |= GLV_RecursiveGetChannel(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[2]);
	return ReturnV;
}

GLV_Scalar GLV_RecursiveGetChannelSurface(tdst_GLV *p_stGLV,u32 FaceNum)
{
	GLV_Scalar ReturnV;
	if (p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber & 0x10000000) return 0.0;
	p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber |= 0x10000000;
	ReturnV = GLV_GetSurf( p_stGLV , &p_stGLV->p_stFaces[FaceNum]);
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[0] & 0xff000000)) ReturnV += GLV_RecursiveGetChannelSurface(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[0]);
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[1] & 0xff000000)) ReturnV += GLV_RecursiveGetChannelSurface(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[1]);
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[2] & 0xff000000)) ReturnV += GLV_RecursiveGetChannelSurface(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[2]);
	return ReturnV;
}
GLV_Scalar	NearestDistance;
u32			OtherColor;
#define BorderSize 10.0


void GLV_ComputeDistanceToNearestBorder(tdst_GLV *p_stGLV,u32 FaceNum,MATHD_tdst_Vector *Src)
{
	GLV_Scalar	LocalDistance;
	u32 Counter;
	u32 Reorder[3];

	Reorder[0] = 0;
	Reorder[1] = 1;
	Reorder[2] = 2;
	{
		GLV_Scalar	ThreeDistance[3];
		ThreeDistance[0] = GLV_DistanceEdgePoint( &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[FaceNum].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[FaceNum].Index[1]].P3D , Src);
		ThreeDistance[1] = GLV_DistanceEdgePoint( &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[FaceNum].Index[1]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[FaceNum].Index[2]].P3D , Src);
		ThreeDistance[2] = GLV_DistanceEdgePoint( &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[FaceNum].Index[2]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[FaceNum].Index[0]].P3D , Src);
		if (ThreeDistance[0] > ThreeDistance[1])
		{
			GLV_Scalar SWAP;
			SWAP = ThreeDistance[0];
			ThreeDistance[0] = ThreeDistance[1];
			ThreeDistance[1] = SWAP;
			Reorder[0] = 1;
			Reorder[1] = 0;
		}
		
		if (ThreeDistance[0] > ThreeDistance[2])
		{
			GLV_Scalar SWAP;
			u32 Swapul;
			SWAP = ThreeDistance[0];
			ThreeDistance[0] = ThreeDistance[2];
			ThreeDistance[2] = SWAP;
			Swapul = Reorder[0];
			Reorder[0] = Reorder[2];
			Reorder[2] = Swapul;
		}
		
		if (ThreeDistance[1] > ThreeDistance[2])
		{
			u32 Swapul;
			Swapul = Reorder[1];
			Reorder[1] = Reorder[2];
			Reorder[2] = Swapul;
		}
	}
	
	if (p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber & 0x10000000) return;
	p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber |= 0x10000000;
	for (Counter = 0; Counter < 3;Counter++)
	{
		if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[Reorder[Counter]] & 0xff000000)) 
		{
			if (GLV_b_IsCompatibleColors_NearestBorder(p_stGLV , FaceNum , p_stGLV->p_stFaces[FaceNum].Nghbr[Reorder[Counter]] ))  
			{
				if (GLV_b_IsCompatibleColors2(p_stGLV , FaceNum , p_stGLV->p_stFaces[FaceNum].Nghbr[Reorder[Counter]] ))  
				{
					if ((p_stGLV->p_stPoints[p_stGLV->p_stFaces[p_stGLV->p_stFaces[FaceNum].Nghbr[Reorder[Counter]]].Index[0]].Distance2SB < NearestDistance) ||
						(p_stGLV->p_stPoints[p_stGLV->p_stFaces[p_stGLV->p_stFaces[FaceNum].Nghbr[Reorder[Counter]]].Index[1]].Distance2SB < NearestDistance) ||
						(p_stGLV->p_stPoints[p_stGLV->p_stFaces[p_stGLV->p_stFaces[FaceNum].Nghbr[Reorder[Counter]]].Index[2]].Distance2SB < NearestDistance))
					{
						LocalDistance = GLV_DistanceEdgePoint( &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[FaceNum].Index[(Reorder[Counter] + 0)%3]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[FaceNum].Index[(Reorder[Counter] + 1)%3]].P3D , Src) / BorderSize;
						if (LocalDistance < 2.0)
							GLV_ComputeDistanceToNearestBorder(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[Reorder[Counter]],Src);
					}
				}
			} else
			{
				{
					LocalDistance = GLV_DistanceEdgePoint( &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[FaceNum].Index[(Reorder[Counter] + 0)%3]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[FaceNum].Index[(Reorder[Counter] + 1)%3]].P3D , Src) / BorderSize;
					if (LocalDistance < NearestDistance) NearestDistance = LocalDistance;
				}
			}
		}
	}
}


void GLV_Clear0x1(tdst_GLV *p_stGLV,u32 FaceNum)
{
	GLV_Scalar	LocalDistance;
	u32 Counter;
	if (!(p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber & 0x10000000)) return;
	p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber &= ~0x10000000;
	for (Counter = 0; Counter < 3;Counter++)
	{
		if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[Counter] & 0xff000000)) 
		{
			GLV_Clear0x1(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[Counter]);
		}
	}
}


u32 ColorTEst(u32 C)
{
	if ((C != 0xff)  && (C != 0xff00) && (C != 0xffff)) return 0;
	return 1;
}
void GLV_MaxColors(tdst_GLV * p_GLV , ULONG CDest , ULONG CSrc )
{
	ULONG I1, I2 , Color;
	I1 = I2 = 0xffffffff;
	if (CSrc & 0xff000000) return;
	if (p_GLV->p_stFaces[CDest].Nghbr[0] == CSrc) I1 = 0;
	if (p_GLV->p_stFaces[CDest].Nghbr[1] == CSrc) I1 = 1;
	if (p_GLV->p_stFaces[CDest].Nghbr[2] == CSrc) I1 = 2;
	if (p_GLV->p_stFaces[CSrc].Nghbr[0] == CDest) I2 = 0;
	if (p_GLV->p_stFaces[CSrc].Nghbr[1] == CDest) I2 = 1;
	if (p_GLV->p_stFaces[CSrc].Nghbr[2] == CDest) I2 = 2;
	if ((I1 | I2) == 0xffffffff) return;
	/* Colors Merge */
	if ((p_GLV->p_stFaces[CDest].Colours[(I1 + 0) % 3] & 0xff) < (p_GLV->p_stFaces[CSrc].Colours[(I2 + 1) % 3] & 0xff))
	{
		p_GLV->p_stFaces[CDest].Colours[(I1 + 0) % 3] &= ~0xff;
		p_GLV->p_stFaces[CDest].Colours[(I1 + 0) % 3] |= p_GLV->p_stFaces[CSrc].Colours[(I2 + 1) % 3] & 0xff;
	}
	if ((p_GLV->p_stFaces[CDest].Colours[(I1 + 0) % 3] & 0xff00) < (p_GLV->p_stFaces[CSrc].Colours[(I2 + 1) % 3] & 0xff00))
	{
		p_GLV->p_stFaces[CDest].Colours[(I1 + 0) % 3] &= ~0xff00;
		p_GLV->p_stFaces[CDest].Colours[(I1 + 0) % 3] |= p_GLV->p_stFaces[CSrc].Colours[(I2 + 1) % 3] & 0xff00;
	}
	if ((p_GLV->p_stFaces[CDest].Colours[(I1 + 0) % 3] & 0xff0000) < (p_GLV->p_stFaces[CSrc].Colours[(I2 + 1) % 3] & 0xff0000))
	{
		p_GLV->p_stFaces[CDest].Colours[(I1 + 0) % 3] &= ~0xff0000;
		p_GLV->p_stFaces[CDest].Colours[(I1 + 0) % 3] |= p_GLV->p_stFaces[CSrc].Colours[(I2 + 1) % 3] & 0xff0000;
	}

	if ((p_GLV->p_stFaces[CDest].Colours[(I1 + 1) % 3] & 0xff) < (p_GLV->p_stFaces[CSrc].Colours[(I2 + 0) % 3] & 0xff))
	{
		p_GLV->p_stFaces[CDest].Colours[(I1 + 1) % 3] &= ~0xff;
		p_GLV->p_stFaces[CDest].Colours[(I1 + 1) % 3] |= p_GLV->p_stFaces[CSrc].Colours[(I2 + 0) % 3] & 0xff;
	}
	if ((p_GLV->p_stFaces[CDest].Colours[(I1 + 1) % 3] & 0xff00) < (p_GLV->p_stFaces[CSrc].Colours[(I2 + 0) % 3] & 0xff00))
	{
		p_GLV->p_stFaces[CDest].Colours[(I1 + 1) % 3] &= ~0xff00;
		p_GLV->p_stFaces[CDest].Colours[(I1 + 1) % 3] |= p_GLV->p_stFaces[CSrc].Colours[(I2 + 0) % 3] & 0xff00;
	}
	if ((p_GLV->p_stFaces[CDest].Colours[(I1 + 1) % 3] & 0xff0000) < (p_GLV->p_stFaces[CSrc].Colours[(I2 + 0) % 3] & 0xff0000))
	{
		p_GLV->p_stFaces[CDest].Colours[(I1 + 1) % 3] &= ~0xff0000;
		p_GLV->p_stFaces[CDest].Colours[(I1 + 1) % 3] |= p_GLV->p_stFaces[CSrc].Colours[(I2 + 0) % 3] & 0xff0000;
	}
}

void GLV_RecursiveClearOne(tdst_GLV *p_stGLV,u32 FaceNum)
{
	GLV_Scalar ReturnV;
	if (!(p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber & 0x01000000)) return;
	p_stGLV->p_stFaces[FaceNum].ulSurfaceNumber &= ~0x01000000;
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[0] & 0xff000000)) GLV_RecursiveClearOne(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[0]);
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[1] & 0xff000000)) GLV_RecursiveClearOne(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[1]);
	if (!(p_stGLV->p_stFaces[FaceNum].Nghbr[2] & 0xff000000)) GLV_RecursiveClearOne(p_stGLV,p_stGLV->p_stFaces[FaceNum].Nghbr[2]);
	return ;
}
#define ColorOptTest
void GLV_ClearSurfaces(tdst_GLV *p_stGLV )
{
	u32 Counter;
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfPoints ; Counter ++)
	{
		p_stGLV->p_stPoints[Counter].ulSurfaceOwner = 0;
	}
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		p_stGLV->p_stFaces[Counter].ulSurfaceNumber = 0;
	}
}
void GLV_ComputeSurfaces(tdst_GLV *p_stGLV , u32 ulColorCompatible)
{
	ULONG Counter,ChannelNum,CornerCounter,Counter2;
	tdst_GLV *p_stGLVDetector;
	
	p_stGLVDetector = GLV_Duplicate(p_stGLV);
	GLD_BlowUpIndexion( p_stGLVDetector , 0);
	for (Counter = 0 ; Counter < p_stGLVDetector->ulNumberOfFaces ; Counter ++)
	{
		MATHD_tdst_Vector p3[3];
		MATHD_BlendVector(&p3[0],&p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[0]].P3D,&p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[1]].P3D,0.5);
		MATHD_BlendVector(&p3[1],&p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[1]].P3D,&p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[2]].P3D,0.5);
		MATHD_BlendVector(&p3[2],&p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[2]].P3D,&p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[0]].P3D,0.5);
		p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[0]].P3D = p3[0];
		p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[1]].P3D = p3[1];
		p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[2]].P3D = p3[2];
		p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[0]].GLV_Point_Plane[0] = p_stGLVDetector->p_stFaces[Counter].Plane;
		p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[1]].GLV_Point_Plane[0] = p_stGLVDetector->p_stFaces[Counter].Plane;
		p_stGLVDetector->p_stPoints[p_stGLVDetector->p_stFaces[Counter].Index[2]].GLV_Point_Plane[0] = p_stGLVDetector->p_stFaces[Counter].Plane;
	}
	for (Counter = 0 ; Counter < p_stGLVDetector->ulNumberOfPoints ; Counter ++)
	{
		p_stGLVDetector->p_stPoints[Counter].ulSurfaceOwner = 1;
		p_stGLVDetector->p_stPoints[Counter].ulNumberOfPlanes = 1;
		
	}
	GLD_Weld( p_stGLVDetector , GLV_WELD_FCT , 4 );
	GLD_SortOverX( p_stGLVDetector  );

	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		p_stGLV->p_stFaces[Counter].ulSurfaceNumber = 0;
		p_stGLV->p_stFaces[Counter].ulPolylineNum = 0;
	}
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfPoints ; Counter ++)
	{
		p_stGLV->p_stPoints[Counter].ulSurfaceOwner = 0;
		p_stGLV->p_stPoints[Counter].ulColor = 0;
	}
	/* Weld total */
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0 );
	GLV_ComputeNghbr(p_stGLV);
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		u32 IndexD;
		MATHD_tdst_Vector p3;
		for (CornerCounter = 0 ; CornerCounter < 3 ; CornerCounter++)
		{
			if (p_stGLV->p_stFaces[Counter].Nghbr[CornerCounter] < 0xff000000)
			{
				MATHD_BlendVector(&p3,&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[CornerCounter]].P3D,&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[(CornerCounter + 1) % 3]].P3D,0.5);
				IndexD = GLD_GetNearestIndex( p_stGLVDetector , &p3);
				if ((IndexD != 0xffffffff) && (p_stGLVDetector ->p_stPoints[IndexD].ulSurfaceOwner > 2))
				{
					u32 CounterDFGH ;
					CounterDFGH = p_stGLVDetector -> p_stPoints[IndexD].ulNumberOfPlanes;
					while (CounterDFGH--)
					{
						if (MATHD_f_DotProduct(&p_stGLVDetector ->p_stPoints[IndexD].GLV_Point_Plane[CounterDFGH].Normale,&p_stGLV->p_stFaces[Counter].Plane.Normale) < 0.999)
						{
							GLV_Scalar DistanceToplane;
							DistanceToplane = MATHD_f_DistanceToPlane(&p_stGLVDetector->p_stPoints[IndexD].GLV_Point_Plane[CounterDFGH] , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[(CornerCounter + 2 ) % 3]].P3D);
							if (fAbs(DistanceToplane) > GLF_PrecMetric)
							{
								if (DistanceToplane  < 0)
								{
									// Dessous
									p_stGLV->p_stFaces[Counter].ulSurfaceNumber |= 0x80000000;
									p_stGLV->p_stFaces[Counter].PassivityFlags |= 0x1 << CornerCounter;
								}
								else
								{
									p_stGLV->p_stFaces[Counter].ulSurfaceNumber |= 0x40000000;
									p_stGLV->p_stFaces[Counter].PassivityFlags |= 0x1 << (CornerCounter + 8);
								}
							}
						}
					}
					p_stGLV->p_stFaces[Counter].Nghbr[CornerCounter] = 0xffffffff;
				}
			}
		}
	}

	ChannelNum = 1;
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		
		if ((p_stGLV->p_stFaces[Counter].ulSurfaceNumber & 0x00ffffff) == 0)
		{
			if (ulColorCompatible)
				GLV_RecursiveSetChannel_CC(p_stGLV,Counter,ChannelNum | GLV_RecursiveGetChannel_CC(p_stGLV,Counter));
			else
				GLV_RecursiveSetChannel(p_stGLV,Counter,ChannelNum | GLV_RecursiveGetChannel(p_stGLV,Counter));
			ChannelNum ++;
		}
	}

	GLV_Destroy(p_stGLVDetector);


}

void GLV_OptimizeHardBorders(tdst_GLV *p_stGLV)
{
	ULONG Merge,Counter,ChannelNum;
	GLV_Scalar *pChannelNumSurfaces;

	ChannelNum = 0;
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		if ((p_stGLV->p_stFaces[Counter].ulSurfaceNumber & 0x00ffffff) > ChannelNum)
		{
			ChannelNum = p_stGLV->p_stFaces[Counter].ulSurfaceNumber & 0x00ffffff;
		}
	}
	ChannelNum++;

	Merge = 200;
	while (Merge)
	{
		pChannelNumSurfaces = (GLV_Scalar* )GLV_ALLOC(sizeof(GLV_Scalar) * ChannelNum); // Memset to 0 is assumed
		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
		{
			if (pChannelNumSurfaces[p_stGLV->p_stFaces[Counter].ulSurfaceNumber & 0x00ffffff] == 0.0)
			{
				pChannelNumSurfaces[p_stGLV->p_stFaces[Counter].ulSurfaceNumber & 0x00ffffff] = GLV_RecursiveGetChannelSurface(p_stGLV,Counter);
			}
		}
		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
		{
			p_stGLV->p_stFaces[Counter].ulSurfaceNumber &= ~0x30000000;
		}
		Merge = 0;
		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
		{
			u32 IndexD;
			GLV_Scalar Coef;
			// 1 First Candidate, the surface of the triangle divided by surface of hist channele is < to a coeficient
			Coef = GLV_GetSurf( p_stGLV , &p_stGLV->p_stFaces[Counter]) / pChannelNumSurfaces[p_stGLV->p_stFaces[Counter].ulSurfaceNumber & 0x00ffffff];
			if (Coef < 1.0/2000.0)
			{
				// 2: The 3 neighbour must be correct
				if (((p_stGLV->p_stFaces[Counter].Nghbr[0] | p_stGLV->p_stFaces[Counter].Nghbr[1] | p_stGLV->p_stFaces[Counter].Nghbr[2]) & 0xff000000) == 0)
				{
					ULONG Color[6];
					ULONG Compatible[3];
					Compatible[0] = GLV_b_IsCompatibleColors(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].Nghbr[0] );
					Compatible[1] = GLV_b_IsCompatibleColors(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].Nghbr[1] );
					Compatible[2] = GLV_b_IsCompatibleColors(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].Nghbr[2] );
					if ((Compatible[0] + Compatible[1] + Compatible[2]) <= 1)
					{
						// OK -> Merge
						if (!Compatible[0]) GLV_MergeColors(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].Nghbr[0] );
						if (!Compatible[1]) GLV_MergeColors(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].Nghbr[1] );
						if (!Compatible[2]) GLV_MergeColors(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].Nghbr[2] );
	//					p_stGLV->p_stFaces[Counter].ulSurfaceNumber = ChannelNum++;
						Merge  |= 0x80000000;
					}
				}
			}//*/
		}
		GLV_FREE(pChannelNumSurfaces);
		if (Merge & 0x80000000)
			Merge &= ~0x80000000;
		else
			Merge = 0;
	}
}


void GLV_ShowSurfaceInColor(tdst_GLV *p_stGLV )
{
	u32 Counter;
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		u32 Color;
		if ((p_stGLV->p_stFaces[Counter].ulSurfaceNumber & 0xc0000000) == 0x80000000)
		{
			Color  = 0xff;
		} else
/*		if (p_stGLV->p_stFaces[Counter].PassivityFlags)
		{
			Color = p_stGLV->p_stFaces[Counter].PassivityFlags;
		} else*/
		{
			Color = (p_stGLV->p_stFaces[Counter].ulSurfaceNumber & 3) << 5;													
			Color += ((p_stGLV->p_stFaces[Counter].ulSurfaceNumber >> 2) & 3) << (5 + 8);										
			Color += ((p_stGLV->p_stFaces[Counter].ulSurfaceNumber >> 4) & 3) << (5 + 16);
	/*		Color &= 0xfefefe;
			Color >>= 1;*/
		}
		p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[0]].ulColor = Color;
		p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[1]].ulColor = Color;
		p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[2]].ulColor = Color;
		p_stGLV->p_stFaces[Counter].Colours[0] = Color;	
		p_stGLV->p_stFaces[Counter].Colours[1] = Color;	
		p_stGLV->p_stFaces[Counter].Colours[2] = Color;	
		p_stGLV->p_stFaces[Counter].ulSurfaceNumber = 0;
	}//*/
}

void GLV_ComputeDistanceToBorder(tdst_GLV *p_stGLV)
{
	u32 Counter,Merge;
	u32 Counter2;

	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0 );
	GLV_ComputeNghbr(p_stGLV);
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
					p_stGLV->p_stFaces[Counter].ulDistanceToBorder = 0xffffff;

	Merge = 20;

	while (Merge--)
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		for (Counter2 = 0 ; Counter2  < 3 ; Counter2 ++)
		{
			if (!(p_stGLV->p_stFaces[Counter].Nghbr[Counter2 ] & 0xff000000))
			{
				if ((p_stGLV->p_stFaces[p_stGLV->p_stFaces[Counter].Nghbr[Counter2 ]].ulSurfaceNumber == p_stGLV->p_stFaces[Counter].ulSurfaceNumber))
				{
					if (p_stGLV->p_stFaces[p_stGLV->p_stFaces[Counter].Nghbr[Counter2 ]].ulDistanceToBorder + 1 < p_stGLV->p_stFaces[Counter].ulDistanceToBorder) 
						p_stGLV->p_stFaces[Counter].ulDistanceToBorder = p_stGLV->p_stFaces[p_stGLV->p_stFaces[Counter].Nghbr[Counter2 ]].ulDistanceToBorder + 1;
				}
				else
					p_stGLV->p_stFaces[Counter].ulDistanceToBorder = 0;
			}
		}
	}


}


void GLV_EraseUnderPoys(tdst_GLV *p_stGLV)
{
	u32 Counter;

	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0 );
	GLV_ComputeNghbr(p_stGLV);

	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		if (p_stGLV->p_stFaces[Counter].PassivityFlags & (0x1 << 0))
		{
			GLV_MergeColors(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].Nghbr[0] );
			p_stGLV->p_stFaces[Counter].Colours[2] = (p_stGLV->p_stFaces[Counter].Colours[0] & 0xFEFEFEFE)>>1;
			p_stGLV->p_stFaces[Counter].Colours[2] += (p_stGLV->p_stFaces[Counter].Colours[1] & 0xFEFEFEFE)>>1;
		}
		if (p_stGLV->p_stFaces[Counter].PassivityFlags & (0x1 << 1))
		{
			GLV_MergeColors(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].Nghbr[1] );
			p_stGLV->p_stFaces[Counter].Colours[0] = (p_stGLV->p_stFaces[Counter].Colours[2] & 0xFEFEFEFE)>>1;
			p_stGLV->p_stFaces[Counter].Colours[0] += (p_stGLV->p_stFaces[Counter].Colours[1] & 0xFEFEFEFE)>>1;
		}
		if (p_stGLV->p_stFaces[Counter].PassivityFlags & (0x1 << 2))
		{
			GLV_MergeColors(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].Nghbr[2] );
			p_stGLV->p_stFaces[Counter].Colours[1] = (p_stGLV->p_stFaces[Counter].Colours[0] & 0xFEFEFEFE)>>1;
			p_stGLV->p_stFaces[Counter].Colours[1] += (p_stGLV->p_stFaces[Counter].Colours[2] & 0xFEFEFEFE)>>1;
		}
	}
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		u32 Counter2;
		for (Counter2 = 0 ; Counter2  < 3 ; Counter2 ++)
		{
			if ((p_stGLV->p_stFaces[Counter].PassivityFlags & (1 << Counter2)) == 0)
			{
				if (!(p_stGLV->p_stFaces[Counter].Nghbr[Counter2 ] &0xff000000))
					if ((p_stGLV->p_stFaces[p_stGLV->p_stFaces[Counter].Nghbr[Counter2 ]].ulSurfaceNumber == p_stGLV->p_stFaces[Counter].ulSurfaceNumber))
						if ((p_stGLV->p_stFaces[p_stGLV->p_stFaces[Counter].Nghbr[Counter2 ]].ulDistanceToBorder <= p_stGLV->p_stFaces[Counter].ulDistanceToBorder))
							GLV_MaxColors(p_stGLV , Counter  ,p_stGLV->p_stFaces[Counter].Nghbr[Counter2 ]);
			}
		}
	}//*/
}
void GLV_SmoothHardBorders(tdst_GLV *p_stGLV )
{
	ULONG Merge,Counter,ChannelNum,CornerCounter;

	GLV_ComputeSurfaces(p_stGLV , 0);
	GLV_BreakUncompatibleLinks_JadeElement_and_UV(p_stGLV);
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0 );
	GLV_ComputeNghbr(p_stGLV);
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		p_stGLV ->p_stFaces[Counter].ulSubdivideFlags = 0;
		p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[0]].Distance2SB = 0.0;
		p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[1]].Distance2SB = 0.0;
		p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[2]].Distance2SB = 0.0;
		p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[0]].ulFlags = 0;
		p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[1]].ulFlags = 0;
		p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[2]].ulFlags = 0;
	}
	Merge = 8;
	while (Merge--)
	{
		GLV_Subdivide(p_stGLV,0);
		GLV_ComputeSurfaces(p_stGLV , 0);
		GLD_Weld( p_stGLV , GLV_WELD_FCT , 1 );
		GLV_ComputeNghbr(p_stGLV);//*/
		GLD_BlowUpIndexion(  p_stGLV , 0);
		GLD_Weld( p_stGLV , GLV_WELD_FCT , 1 );
		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
		{
			p_stGLV ->p_stFaces[Counter].ulSubdivideFlags = 0;
			p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[0]].Distance2SB = 1.0;
			p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[1]].Distance2SB = 1.0;
			p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[2]].Distance2SB = 1.0;
		}


		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
		{
			{
				NearestDistance = 1000000000000.0;
				
				GLV_ComputeDistanceToNearestBorder(p_stGLV,Counter,&p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[0]].P3D);
				GLV_Clear0x1(p_stGLV,Counter);
				if (NearestDistance < p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[0]].Distance2SB)
					p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[0]].Distance2SB = NearestDistance ;
				
				NearestDistance = 1000000000000.0;
				
				GLV_ComputeDistanceToNearestBorder(p_stGLV,Counter,&p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[1]].P3D);
				GLV_Clear0x1(p_stGLV,Counter);
				if (NearestDistance < p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[1]].Distance2SB)
					p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[1]].Distance2SB = NearestDistance ;
				NearestDistance = 1000000000000.0;
				
				GLV_ComputeDistanceToNearestBorder(p_stGLV,Counter,&p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[2]].P3D);
				GLV_Clear0x1(p_stGLV,Counter);
				if (NearestDistance < p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[2]].Distance2SB)
					p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[2]].Distance2SB = NearestDistance ;
			}//*/
		}
		if (Merge)
		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
		{
			p_stGLV ->p_stFaces[Counter].ulSubdivideFlags = 0;
			for (CornerCounter = 0 ; CornerCounter < 3 ; CornerCounter++)
			{
				MATHD_tdst_Vector Local;
				GLV_Scalar Zgeg,Zgeg2;
				if (MATHD_f_Distance(&p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[CornerCounter]].P3D,&p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[(CornerCounter + 1 ) % 3]].P3D) > BorderSize * 0.25)
				{
					Zgeg = p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[CornerCounter]].Distance2SB;
					Zgeg2 = p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[(CornerCounter + 1 ) % 3]].Distance2SB;
/*					if ((Zgeg > 1.01) && (Zgeg2 < 1.0))
					{
						p_stGLV ->p_stFaces[Counter].ulSubdivideFlags |= 7;
					} else
						if ((Zgeg2 > 1.01) && (Zgeg < 1.0))
						{
							p_stGLV ->p_stFaces[Counter].ulSubdivideFlags |= 7;
						}//*/

					if ((!(p_stGLV ->p_stFaces[Counter].ulSubdivideFlags & (1<<CornerCounter))))
					{
						NearestDistance = 1000000000000.0;
						MATHD_BlendVector(&Local,&p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[CornerCounter]].P3D,&p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[(CornerCounter + 1 ) % 3]].P3D,0.5);
						
						GLV_ComputeDistanceToNearestBorder(p_stGLV,Counter,&Local);
						GLV_Clear0x1(p_stGLV,Counter);
						if (NearestDistance > 1.0) NearestDistance = 1.0f;
						{
							{
								Zgeg = (p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[(CornerCounter + 0 ) % 3]].Distance2SB  + p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[(CornerCounter + 1 ) % 3]].Distance2SB ) * 0.5;
								{
									if (fAbs(Zgeg - NearestDistance) > 0.005)
										p_stGLV ->p_stFaces[Counter].ulSubdivideFlags |= 1<<CornerCounter;
								}
							}
						}
					}//*/
				}

			}//*/
		}
		//GLV_BreakUncompatibleLinks_JadeElement_and_UV(p_stGLV);
	}
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter ++)
	{
		{
			if (p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[0]].Distance2SB > 1.0) p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[0]].Distance2SB = 1.0;
			if (p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[1]].Distance2SB > 1.0) p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[1]].Distance2SB = 1.0;
			if (p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[2]].Distance2SB > 1.0) p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[2]].Distance2SB = 1.0;
			p_stGLV ->p_stFaces[Counter].Colours[0] = p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[0]].Distance2SB * 128.0;
			p_stGLV ->p_stFaces[Counter].Colours[1] = p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[1]].Distance2SB * 128.0;
			p_stGLV ->p_stFaces[Counter].Colours[2] = p_stGLV->p_stPoints[p_stGLV ->p_stFaces[Counter].Index[2]].Distance2SB * 128.0;
			p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[0]].ulColor = p_stGLV ->p_stFaces[Counter].Colours[0] ;
			p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[1]].ulColor = p_stGLV ->p_stFaces[Counter].Colours[1] ;
			p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[2]].ulColor = p_stGLV ->p_stFaces[Counter].Colours[2] ;
		}//*/
	}
	GLV_BreakUncompatibleLinks_JadeElement_and_UV(p_stGLV);
}

void GLV_CutBooleanOverlad(tdst_GLV *p_stGLV , u32 DestroyInvisible)
{
	return;
	/*Destroy formfactors */
	if (p_stGLV ->p_FormFactors)
	{
		ULONG Counter;
		Counter = p_stGLV ->ulNumberOfFaces * 3l;
		while (Counter--) if (p_stGLV ->p_FormFactors[Counter]) GLV_FREE(p_stGLV ->p_FormFactors[Counter]);
		GLV_FREE(p_stGLV->p_FormFactors);
		p_stGLV->p_FormFactors = NULL;
	}

	
	GLV_ComputeSurfaces(p_stGLV , 1);
	GLV_ComputeDistanceToBorder(p_stGLV);
	GLV_EraseUnderPoys(p_stGLV);
	GLV_ComputeSurfaces(p_stGLV , 0);
	GLV_OptimizeHardBorders(p_stGLV);
	GLV_ComputeSurfaces(p_stGLV , 0);
	GLV_OptimizeHardBorders(p_stGLV);
	GLV_ComputeSurfaces(p_stGLV , 0);
	GLV_OptimizeHardBorders(p_stGLV);


	//GLV_SmoothHardBorders(p_stGLV);


	GLV_ComputeSurfaces(p_stGLV , 1);
	//GLV_ShowSurfaceInColor(p_stGLV );
	GLV_BreakUncompatibleLinks_JadeElement_and_UV(p_stGLV);
	return;
}

#endif

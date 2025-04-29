/* GEO_LIGHTCUT.c */

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
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEO_LIGHTCUT.h"
#include "GEOmetric/GEO_SKIN.h"

#ifdef JADEFUSION
#include "GEOmetric/GEOXenonPack.h"
#endif

#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"


#include <math.h>
#include <STDLIB.H>
#include "../dlls/MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "../dlls/MAD_mem/Sources/MAD_mem.h"
#include "GEOmetric/GEO_LODCmpt.h"

#define GLV_REMOVE_FLAG (OBJ_C_IdentityFlag_Dyna|OBJ_C_IdentityFlag_Anims|OBJ_C_IdentityFlag_Bone|OBJ_C_IdentityFlag_Generated|OBJ_C_IdentityFlag_AI)
void GLV_ComputeRadiosity_Compute_FF_Fast_RT (tdst_GLV *p_stGLV ,  ULONG RadiosityColor , ULONG bSmoothResult , ULONG bUseFastRadio , ULONG (*SeprogressPos) (float F01, char *));
#define GLV_MAX_GO 2048
static OBJ_tdst_GameObject *p_AllComputedGO[GLV_MAX_GO];
static ULONG				ulNumberOfGO = 0;

#ifdef GLV_DEBUG
tdst_GLV *BugReport = NULL;
#endif

#ifdef JADEFUSION
	#if defined(_GLV_KEEP_KILT)
	tdst_GLV *KiltToDraw = NULL;
	#endif
	#if defined(_GLV_KEEP_LAST)
	tdst_GLV *p_LastGLV = NULL;
	#endif
#else
	tdst_GLV *KiltToDraw = NULL;
	tdst_GLV *p_LastGLV = NULL;
#endif

//#ifdef GLV_DRAW_OK3
GLV_OCtreeNode *p_Octree = NULL;
GLV_OCtreeNode stOctree;
//#endif
static 	LONG Polygone1 , Polygone2;

#ifdef JADEFUSION
#if defined(_GLV_TRACE_MEMORY)

#include <map>

struct GLV_MemoryInfo
{
    const char* szFile;     // SC: Can be const char* since we are using __FILE__ to populate the map
    int         iLine;
    int         iSize;
};

typedef std::map<ULONG, GLV_MemoryInfo> GLV_MemoryMap;

GLV_MemoryMap g_oGLVMemoryTrace;
ULONG         g_ulGLVAllocatedMemory = 0;
ULONG         g_ulGLVPeakMemory      = 0;

void* GLV_p_Alloc(int _iSize, const char* _szFile, int _iLine)
{
    GLV_MemoryInfo oMemoryBlock;

    void* pPtr = malloc(_iSize);
    ERR_X_Assert((pPtr != NULL) && "Out of memory while computing radiosity");

    oMemoryBlock.szFile = _szFile;
    oMemoryBlock.iLine  = _iLine;
    oMemoryBlock.iSize  = _iSize;
    g_oGLVMemoryTrace[(ULONG)pPtr] = oMemoryBlock;

    g_ulGLVAllocatedMemory += _iSize;
    if (g_ulGLVAllocatedMemory > g_ulGLVPeakMemory)
        g_ulGLVPeakMemory = g_ulGLVAllocatedMemory;

    return pPtr;
}

void* GLV_p_Realloc(void* _pPtr, int _iSize, const char* _szFile, int _iLine)
{
    GLV_MemoryMap::iterator it;
    GLV_MemoryInfo oMemoryBlock;

    it = g_oGLVMemoryTrace.find((ULONG)_pPtr);
    if (it != g_oGLVMemoryTrace.end())
    {
        g_ulGLVAllocatedMemory -= it->second.iSize;

        g_oGLVMemoryTrace.erase(it);
    }

    void* pPtr = realloc(_pPtr, _iSize);
    ERR_X_Assert((pPtr != NULL) && "Out of memory while computing radiosity");

    oMemoryBlock.szFile = _szFile;
    oMemoryBlock.iLine  = _iLine;
    oMemoryBlock.iSize  = _iSize;
    g_oGLVMemoryTrace[(ULONG)pPtr] = oMemoryBlock;

    g_ulGLVAllocatedMemory += _iSize;
    if (g_ulGLVAllocatedMemory > g_ulGLVPeakMemory)
        g_ulGLVPeakMemory = g_ulGLVAllocatedMemory;

    return pPtr;
}

void GLV_Free(void* _pPtr)
{
    GLV_MemoryMap::iterator it;

    it = g_oGLVMemoryTrace.find((ULONG)_pPtr);
    if (it != g_oGLVMemoryTrace.end())
    {
        g_ulGLVAllocatedMemory -= it->second.iSize;

        g_oGLVMemoryTrace.erase(it);
    }

    free(_pPtr);
}

void GLV_DumpMemory(void)
{
    GLV_MemoryMap::iterator it;

    ERR_OutputDebugString("]> GLV_DumpMemory() - Allocated memory %d - Peak memory usage %d\n", g_ulGLVAllocatedMemory, g_ulGLVPeakMemory);

    it = g_oGLVMemoryTrace.begin();
    if (it != g_oGLVMemoryTrace.end())
    {
        ERR_OutputDebugString("]> GLV_DumpMemory() - %u allocations\n", (ULONG)g_oGLVMemoryTrace.size());

        while (it != g_oGLVMemoryTrace.end())
        {
            // SC: Dump the actual allocations
            //ERR_OutputDebugString("%s(%d) - %d bytes @ 0x%08x\n", it->second.szFile, it->second.iLine, it->second.iSize, (ULONG)it->first);

#if !defined(_GLV_KEEP_LAST) && !defined(_GLV_KEEP_KILT)
            free((void*)it->first);
#endif
            ++it;
        }
    }
    ERR_OutputDebugString("]> End of GLV_DumpMemory()\n");

#if !defined(_GLV_KEEP_LAST) && !defined(_GLV_KEEP_KILT)
    g_oGLVMemoryTrace.clear();
    g_ulGLVAllocatedMemory = 0;
#endif
}

#endif
#endif

void GLV_ComputeBckFaceSpherical ( tdst_GLV *p_GLV , ULONG LightNum)
{
	tdst_GLV_Face *p_FT,*p_LT;
	MATHD_tdst_Vector *pst_LightCenter ;
	GLV_Scalar Radius;
	pst_LightCenter = &p_GLV -> p_Lights[LightNum].stLightCenterOS;
	Radius = p_GLV ->p_Lights[LightNum].fLightRadius;
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		if (p_FT->ulFlags & GLV_FLGS_Original)
		{
			MATHD_tdst_Vector st_Local;
			GLV_Scalar	fDistanceToPlane;
			p_FT -> ulFlags &= ~(GLV_FLGS_WhiteFront|GLV_FLGS_BlackFront);
			MATHD_SubVector(&st_Local, pst_LightCenter , &p_GLV->p_stPoints[p_FT->Index[0]].P3D);
			fDistanceToPlane = MATHD_f_DotProduct(&st_Local,&p_FT->Plane.Normale);
			if (fDistanceToPlane > Radius)
				p_FT -> ulFlags |= GLV_FLGS_WhiteFront;
			else
				if (fDistanceToPlane < -Radius)
					p_FT -> ulFlags |= GLV_FLGS_BlackFront;
				else
					p_FT -> ulFlags |= GLV_FLGS_WhiteFront|GLV_FLGS_BlackFront;
		}
		p_FT++;
	}
}

void GLV_SetChannelOnPoints( tdst_GLV *p_GLV )
{
	tdst_GLV_Face *p_FT,*p_LT;
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		p_GLV->p_stPoints[p_FT->Index[0]].ulChannel = p_FT->ulChannel;
		p_GLV->p_stPoints[p_FT->Index[1]].ulChannel = p_FT->ulChannel;
		p_GLV->p_stPoints[p_FT->Index[2]].ulChannel = p_FT->ulChannel;
		p_FT++;
	}
}


void GLV_ComputeBckFaceParrallel( tdst_GLV *p_GLV , ULONG LightNum)
{
	tdst_GLV_Face *p_FT,*p_LT;
	GLV_Scalar Radius;
	Radius = (GLV_Scalar )sin(p_GLV ->p_Lights[LightNum].fLightRadius);
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		if (p_FT->ulFlags & GLV_FLGS_Original)
		{
			GLV_Scalar	fD;
			p_FT -> ulFlags &= ~(GLV_FLGS_WhiteFront|GLV_FLGS_BlackFront);
			fD = MATHD_f_DotProduct(&p_GLV->p_Lights[LightNum].LDir,&p_FT->Plane.Normale);
			if (fD > Radius)
				p_FT -> ulFlags |= GLV_FLGS_WhiteFront;
			else
				if (fD < -Radius)
					p_FT -> ulFlags |= GLV_FLGS_BlackFront;
				else
					p_FT -> ulFlags |= GLV_FLGS_WhiteFront|GLV_FLGS_BlackFront;
		}
		p_FT++;
	}
}

void GLV_ComputeBckFace( tdst_GLV *p_GLV , ULONG LightNum)
{
		switch(p_GLV->p_Lights[LightNum].ulLightFlags & LIGHT_Cul_LF_Type)
		{
		case LIGHT_Cul_LF_Direct:
			GLV_ComputeBckFaceParrallel( p_GLV , LightNum);
			break;
		case LIGHT_Cul_LF_Omni:
		case LIGHT_Cul_LF_Spot:
			GLV_ComputeBckFaceSpherical ( p_GLV , LightNum);
			break;
		case LIGHT_Cul_LF_Fog:
			break;
		}
}

void GLV_st_FindAPoint(tdst_GLV *p_GLV , MATHD_tdst_Vector *p_P1,MATHD_tdst_Vector *p_P2, ULONG ulLightNum , MATHD_tdst_Vector *p_PDOWN)
{
	MATHD_tdst_Vector stProjCenter,stL1,stL2 , stUP;
	GLV_Scalar f1,Dlt,H;

	switch(p_GLV ->p_Lights[ulLightNum].ulLightFlags & LIGHT_Cul_LF_Type)
	{
	case LIGHT_Cul_LF_Direct:
		MATHD_SubVector(&stL2, p_P1, p_P2 );
		MATHD_CrossProduct(&stL1 , &p_GLV ->p_Lights[ulLightNum].LDir , &stL2);
		MATHD_SetNormVector(&stL1, &stL1 , (GLV_Scalar)cos(p_GLV ->p_Lights[ulLightNum].fLightRadius));
		f1 = (GLV_Scalar)sin(p_GLV ->p_Lights[ulLightNum].fLightRadius);
		MATHD_AddScaleVector(&stL1 , &stL1 , &p_GLV ->p_Lights[ulLightNum].LDir , f1);
		MATHD_NormalizeVector(p_PDOWN , &stL1);
		break;
	case LIGHT_Cul_LF_Omni:
	case LIGHT_Cul_LF_Spot:
		{
			MATHD_tdst_Vector st_LightCenter;
			GLV_Scalar fRadius;
			st_LightCenter = p_GLV -> p_Lights[ulLightNum].stLightCenterOS;
			fRadius = p_GLV ->p_Lights[ulLightNum].fLightRadius;
			MATHD_SubVector(&stL1, &st_LightCenter , p_P1);
			f1 = MATHD_f_NormVector(&stL1);
			MATHD_SubVector(&stL2, p_P2 , p_P1);
			MATHD_NormalizeAnyVector(&stL1, &stL1);
			MATHD_NormalizeAnyVector(&stL2, &stL2);
			f1 *= MATHD_f_DotProduct(&stL1,&stL2);
			MATHD_AddScaleVector(&stProjCenter  , p_P1 , &stL2 , f1 );
			MATHD_SubVector(&stL1, &st_LightCenter , &stProjCenter );
			f1 = MATHD_f_NormVector(&stL1);
			Dlt = fRadius * fRadius / f1;
			H = (GLV_Scalar)sqrt(fRadius * fRadius - Dlt * Dlt);
			MATHD_CrossProduct(&stUP, &stL2, &stL1);
			MATHD_SetNormVector(&stUP , &stUP , H);
			MATHD_SetNormVector(&stL1 , &stL1 , f1 - Dlt);
			MATHD_AddVector(&stProjCenter  , &stProjCenter  , &stL1 );
			MATHD_SubVector(p_PDOWN , &stProjCenter  , &stUP );
			MATHD_SubVector(p_PDOWN , p_PDOWN, &st_LightCenter);
			MATHD_NormalizeAnyVector(p_PDOWN, p_PDOWN);
			break;
		}
	}
}

void GLV_GenerateKILTFace( tdst_GLV *p_GLV , ULONG FaceNum , ULONG ulFlags , ULONG LightNum)
{
	ULONG Counter;
	MATHD_tdst_Vector stDwn;
	MATHD_tdst_Vector stP1;
	ULONG ulChannel;
	
	if (!(p_GLV->p_stFaces[FaceNum].ulFlags & GLV_FLGS_Original)) return;
	if (p_GLV->p_stFaces[FaceNum].ulFlags & GLV_FLGS_Transparent) return;

	ulChannel = p_GLV->p_stFaces[FaceNum].ulChannel & p_GLV ->p_Lights[LightNum].ulChannel;
	if (!ulChannel) return;


	for (Counter = 0; Counter < 3 ; Counter++)
	{
		if (p_GLV->p_stFaces[FaceNum].Nghbr[Counter] != -2)
		{
			if ((p_GLV->p_stFaces[FaceNum].Nghbr[Counter] == -1) || (((p_GLV->p_stFaces[FaceNum].ulFlags & (GLV_FLGS_BlackFront|GLV_FLGS_WhiteFront)) == GLV_FLGS_WhiteFront) && ((p_GLV->p_stFaces[p_GLV->p_stFaces[FaceNum].Nghbr[Counter]].ulFlags & GLV_FLGS_BlackFront))))
			{
				
				if ((p_GLV->p_stFaces[FaceNum].ulFlags & GLV_FLGS_BlackFront ))
					GLV_st_FindAPoint( p_GLV , &p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3]].P3D,&p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[Counter]].P3D,LightNum,&stDwn);
				else
					GLV_st_FindAPoint( p_GLV , &p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[Counter]].P3D,&p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3]].P3D,LightNum,&stDwn);
				
				GLV_SetNumbers(p_GLV , 0 , 0 , p_GLV->ulNumberOfFaces + 2 ,4 /* faces */);
				if (p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[Counter]].DownIndex != -1)
				{
					MATHD_NormalizeAnyVector(&p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[Counter]].DownPoint,&p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[Counter]].DownPoint);
					MATHD_BlendVector(&stP1 , &stDwn , &p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[Counter]].DownPoint,0.5f);
					MATHD_SetNormVector(&p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[Counter]].DownPoint , &stP1 , 1.0f / MATHD_f_NormVector(&stP1));
					p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[Counter]].DownIndex ++;
				} else
				{
					p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[Counter]].DownPoint = stDwn;
					p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[Counter]].DownIndex = 0;
				}
				
				if (p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3]].DownIndex != -1)
				{
					MATHD_NormalizeAnyVector(&p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3]].DownPoint,&p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3]].DownPoint);
					MATHD_BlendVector(&stP1 , &stDwn , &p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3]].DownPoint,0.5f);
					MATHD_SetNormVector(&p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3]].DownPoint , &stP1 , 1.0f / MATHD_f_NormVector(&stP1)); 
					p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3]].DownIndex ++;
				} else
				{
					p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3]].DownPoint = stDwn;
					p_GLV->p_stPoints[p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3]].DownIndex = 0;
				}
				
				p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].ulFlags = ulFlags;
				p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].ulMARK = LightNum;
				p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].ulChannel = ulChannel;
				
				if ((p_GLV->p_stFaces[FaceNum].ulFlags &	GLV_FLGS_BlackFront	))
				{
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].Index[0] = p_GLV->p_stFaces[FaceNum].Index[Counter] | 0x40000000;
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].Index[1] = p_GLV->p_stFaces[FaceNum].Index[Counter];
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].Index[2] = p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3];
				} else
				{
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].Index[0] = p_GLV->p_stFaces[FaceNum].Index[Counter] | 0x40000000;
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].Index[2] = p_GLV->p_stFaces[FaceNum].Index[Counter];
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].Index[1] = p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3];
				} 

				p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].Nghbr[0] = p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].Nghbr[1] = p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].Nghbr[2] = -1;
				
				p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].ulFlags = ulFlags;
				p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].ulMARK = LightNum;
				p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].ulChannel = ulChannel;


				if ((p_GLV->p_stFaces[FaceNum].ulFlags &	GLV_FLGS_BlackFront	))
				{
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].Index[0] = p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3];
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].Index[1] = p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3] | 0x40000000;
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].Index[2] = p_GLV->p_stFaces[FaceNum].Index[Counter] | 0x40000000;
				} else {
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].Index[0] = p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3];
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].Index[2] = p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3] | 0x40000000;
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].Index[1] = p_GLV->p_stFaces[FaceNum].Index[Counter] | 0x40000000;
				}
				p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].Nghbr[0] = p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].Nghbr[1] = p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].Nghbr[2] = -1;
				
				/* Compute Penombria plane */
				{
					ULONG Indx1,Indx2;
					MATHD_tdst_Vector st12,st1L,stCP,stCP2,stLightCenter;
					Indx1 = p_GLV->p_stFaces[FaceNum].Index[(Counter+1) % 3];
					Indx2 = p_GLV->p_stFaces[FaceNum].Index[Counter];
					MATHD_SubVector(&st12 , &p_GLV->p_stPoints[Indx1].P3D , &p_GLV->p_stPoints[Indx2].P3D);
					GLV_RT_ComputeLightCenter(p_GLV, LightNum , &p_GLV->p_stPoints[Indx1].P3D , &stLightCenter);
					MATHD_SubVector(&st1L , &p_GLV->p_stPoints[Indx1].P3D , &stLightCenter);
					MATHD_CrossProduct(&stCP , &st1L , &st12 );
					MATHD_CrossProduct(&stCP2 , &stCP , &st12 );
					MATHD_NormalizeVector(&stCP2 , &stCP2 );
					if (MATHD_f_DotProduct(&st1L , &stCP2) < 0.0f)
						MATHD_ScaleEqualVector(&stCP2 , -1.0f);
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].PenombriaPlane.Normale = stCP2;
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].PenombriaPlane.fDistanceTo0 = MATHD_f_DotProduct(&p_GLV->p_stPoints[Indx1].P3D , &stCP2);
					p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 2].PenombriaPlane = p_GLV->p_stFaces[p_GLV->ulNumberOfFaces - 1].PenombriaPlane;
				}
			}
		}
	}
}
/* Call this function AFTER compute Neighbourgs */
void GLV_ComputeConcavesEdges(tdst_GLV *p_GLV)
{
	tdst_GLV_Face *p_FT,*p_LT,*p_FT2;
	ULONG Counter;
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		for (Counter = 0 ;Counter < 3 ; Counter ++)
		{
			MATHD_tdst_Vector st_Local,st_Local2;
			if (p_FT->Nghbr[Counter] < 0xff000000)
			{
				p_FT2 = &p_GLV->p_stFaces[p_FT->Nghbr[Counter]];
				MATHD_SubVector(&st_Local, &p_GLV->p_stPoints[p_FT->Index[Counter]].P3D , &p_GLV->p_stPoints[p_FT->Index[(Counter + 1 ) % 3]].P3D);
				MATHD_NormalizeAnyVector(&st_Local, &st_Local);
				MATHD_CrossProduct(&st_Local2, &p_FT->Plane.Normale, &p_FT2->Plane.Normale);
				if (MATHD_f_DotProduct(&st_Local , &st_Local2) > -0.00000001f)
					p_FT->Nghbr[Counter] = -2;
			}
		}
		p_FT++;
	}
}
void GLV_RemoveKILT(tdst_GLV *p_GLV)
{
	tdst_GLV_Face *p_FT,*p_LT;
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		p_FT->ulFlags &= ~GLV_FLGS_DeleteIt;
		if (p_FT->ulFlags & GLV_FLGS_Kilt)
			p_FT->ulFlags |= GLV_FLGS_DeleteIt;
		p_FT++;
	}
	GLD_Remove2BD(p_GLV);
	GLD_RemoveUnusedIndexes(p_GLV);
}

void GLV_RemoveOriginals(tdst_GLV *p_GLV)
{
	tdst_GLV_Face *p_FT,*p_LT;
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		p_FT->ulFlags &= ~GLV_FLGS_DeleteIt;
		if (p_FT->ulFlags & GLV_FLGS_Original)
			p_FT->ulFlags |= GLV_FLGS_DeleteIt;
		p_FT++;
	}
	GLD_Remove2BD(p_GLV);
	GLD_RemoveUnusedIndexes(p_GLV);
}
void GLV_ExtractEdges(tdst_GLV *p_GLV)
{
	ULONG Counter;
	for (Counter = 0 ; Counter < p_GLV->ulNumberOfEdges ; Counter ++)
	{
/*		if (p_GLV->p_stEdges[Counter].FromFaces[0] != 0xffffffff)
		{
			MATHD_AddScaleVector(&p_GLV->p_stPoints[p_GLV->p_stEdges[Counter].Index[0]].P3D , &p_GLV->p_stPoints[p_GLV->p_stEdges[Counter].Index[0]].P3D , &p_GLV->p_stFaces[p_GLV->p_stEdges[Counter].FromFaces[0]].Plane.Normale , 0.001f);
			MATHD_AddScaleVector(&p_GLV->p_stPoints[p_GLV->p_stEdges[Counter].Index[1]].P3D , &p_GLV->p_stPoints[p_GLV->p_stEdges[Counter].Index[1]].P3D , &p_GLV->p_stFaces[p_GLV->p_stEdges[Counter].FromFaces[0]].Plane.Normale , 0.001f);
		}
		if (p_GLV->p_stEdges[Counter].FromFaces[1] != 0xffffffff)
		{
			MATHD_AddScaleVector(&p_GLV->p_stPoints[p_GLV->p_stEdges[Counter].Index[0]].P3D , &p_GLV->p_stPoints[p_GLV->p_stEdges[Counter].Index[0]].P3D , &p_GLV->p_stFaces[p_GLV->p_stEdges[Counter].FromFaces[1]].Plane.Normale , 0.001f);
			MATHD_AddScaleVector(&p_GLV->p_stPoints[p_GLV->p_stEdges[Counter].Index[1]].P3D , &p_GLV->p_stPoints[p_GLV->p_stEdges[Counter].Index[1]].P3D , &p_GLV->p_stFaces[p_GLV->p_stEdges[Counter].FromFaces[1]].Plane.Normale , 0.001f);
		}*/
	}
}
/* Call this function AFTER compute Neighbourgs */
void GLV_VisibleIfNeight(tdst_GLV *p_GLV)
{
	tdst_GLV_Face *p_FT,*p_LT;
	p_FT = p_GLV->p_stFaces;
	p_LT = p_FT + p_GLV->ulNumberOfFaces;
	while (p_FT < p_LT)
	{
		p_FT->ulFlags &= ~(GLV_FLGS_AIsVisible|GLV_FLGS_BIsVisible|GLV_FLGS_CIsVisible);
		if (p_FT ->Nghbr[0] > 0xff000000) p_FT->ulFlags |= GLV_FLGS_AIsVisible;
		if (p_FT ->Nghbr[1] > 0xff000000) p_FT->ulFlags |= GLV_FLGS_BIsVisible;
		if (p_FT ->Nghbr[2] > 0xff000000) p_FT->ulFlags |= GLV_FLGS_CIsVisible;
		p_FT++;
	}
}

/* Must be call after compute BCKFCE */
void GLV_GenerateKILT( tdst_GLV *p_GLV ,ULONG ulFlags , ULONG LightNum )
{
	ULONG Counter,Counter2,SP;
	MATHD_tdst_Vector stP1,stP2;
	Counter = p_GLV->ulNumberOfPoints;
	while (Counter--)
	{
		p_GLV ->p_stPoints [Counter] .DownIndex = -1;
	}

	Counter = p_GLV->ulNumberOfFaces;
	while (Counter--)
	{
		if (!(p_GLV->p_stFaces[Counter].ulFlags & GLV_FLGS_DontCutOther))
			if (!((ulFlags & GLV_FLGS_WhiteFront) && (p_GLV->p_stFaces[Counter].ulFlags & GLV_FLGS_Nopenombria)))
				GLV_GenerateKILTFace( p_GLV , Counter , ulFlags , LightNum );
	}

	/* COMPUTE NEW NUMBER OF POINTS */ 
	SP = Counter2 = Counter = p_GLV->ulNumberOfPoints;
	while (Counter--)
	{
		if (p_GLV->p_stPoints[Counter].DownIndex != -1)
			p_GLV->p_stPoints[Counter].DownIndex = Counter2++;
	}
	GLV_SetNumbers(p_GLV , Counter2 , 0 , 0 , 1 /* Points */);
	for (; SP < Counter2 ; SP ++)
	{
		p_GLV ->p_stPoints [SP] .DownIndex = -1;
		p_GLV ->p_stPoints [SP] .ulChannel = -1;
	}
	Counter2 = Counter = p_GLV->ulNumberOfPoints;
	while (Counter--)
	{
		switch(p_GLV ->p_Lights[LightNum].ulLightFlags & LIGHT_Cul_LF_Type)
		{
		case LIGHT_Cul_LF_Direct:
			if (p_GLV->p_stPoints[Counter].DownIndex != -1)
			{
				GLV_Scalar Dist;
				if (!p_GLV ->p_Lights[LightNum].bLightPenombriaOn)
				{
					Dist = MATHD_f_DotProduct(&p_GLV->p_stPoints[Counter].P3D , &p_GLV->p_Lights[LightNum].LDir) - p_GLV->p_Lights[LightNum].fLightFar;
					MATHD_SetNormVector(&stP1 , &p_GLV->p_Lights[LightNum].LDir , -Dist);
					MATHD_AddVector(&p_GLV->p_stPoints[p_GLV->p_stPoints[Counter].DownIndex].P3D , &p_GLV->p_stPoints[Counter].P3D , &stP1);
				} else
				{
					MATHD_CrossProduct(&stP2,&p_GLV->p_Lights[LightNum].LDir,&p_GLV->p_stPoints[Counter].DownPoint);
					MATHD_CrossProduct(&stP1,&stP2,&p_GLV->p_stPoints[Counter].DownPoint);
					MATHD_NormalizeVector(&stP1,&stP1);
					if (MATHD_f_DotProduct(&stP1 , &p_GLV->p_Lights[LightNum].LDir) > -0.995f)
					{
						MATHD_tdst_Vector LocalZob;
						MATHD_ScaleVector(&LocalZob , &p_GLV->p_Lights[LightNum].LDir , MATHD_f_DotProduct(&stP1 , &p_GLV->p_Lights[LightNum].LDir));
						MATHD_SubVector(&LocalZob ,&stP1 ,&LocalZob );
						MATHD_SetNormVector(&LocalZob , &LocalZob , 0.1f);
						MATHD_AddScaleVector(&stP1 , &LocalZob , &p_GLV->p_Lights[LightNum].LDir , -0.995f);
					}
					/* COMPUTE DISTANCE TO FAR PLANE */
					Dist = MATHD_f_DotProduct(&p_GLV->p_stPoints[Counter].P3D , &p_GLV->p_Lights[LightNum].LDir) - p_GLV->p_Lights[LightNum].fLightFar;
					MATHD_SetNormVector(&stP1 , &stP1 , Dist);
					MATHD_AddVector(&p_GLV->p_stPoints[p_GLV->p_stPoints[Counter].DownIndex].P3D , &p_GLV->p_stPoints[Counter].P3D , &stP1);
				}
			}
			break;
		case LIGHT_Cul_LF_Omni:
		case LIGHT_Cul_LF_Spot:
			if (p_GLV->p_stPoints[Counter].DownIndex != -1)
			{
				MATHD_AddScaleVector(&stP2,&p_GLV->p_Lights[LightNum].stLightCenterOS,&p_GLV->p_stPoints[Counter].DownPoint , p_GLV->p_Lights[LightNum].fLightRadius);
				MATHD_SubVector(&stP1,&p_GLV->p_stPoints[Counter].P3D,&stP2);
				MATHD_SetNormVector(&stP1 , &stP1 , p_GLV->p_Lights[LightNum].fLightFar * 2.0f);
				MATHD_AddVector(&p_GLV->p_stPoints[p_GLV->p_stPoints[Counter].DownIndex].P3D , &stP2 , &stP1);
			}
			break;
		}
	}
	Counter = p_GLV -> ulNumberOfFaces;
	while (Counter--)
	{
		for ( Counter2 = 0 ; Counter2 < 3 ; Counter2 ++)
		  if (p_GLV->p_stFaces[Counter].Index[Counter2] & 0x40000000)
			  p_GLV->p_stFaces[Counter].Index[Counter2] = p_GLV->p_stPoints[p_GLV->p_stFaces[Counter].Index[Counter2] & 0x3fffffff].DownIndex;
	}
	GLV_ComputeNormales(p_GLV);
}


#ifdef JADEFUSION
BOOL GLV_AddGRO(tdst_GLV *p_KiltGLV , OBJ_tdst_GameObject *p_stGRO , ULONG (*SeprogressPos) (float F01, char *) , ULONG Mark , ULONG BE, float ET )
#else
void	GLV_AddGRO(tdst_GLV *p_KiltGLV , OBJ_tdst_GameObject *p_stGRO , ULONG (*SeprogressPos) (float F01, char *) , ULONG Mark , ULONG BE, float ET )
#endif
{
	tdst_GLV *p_NewGLV;
	ULONG ulFlags;
	ULONG MaterialFlags;
	ULONG *pColor;
	ULONG Counter;
	ULONG ulChannel;
	tdst_ObjectBDVolume stOBV;
	pColor = NULL;
	
#ifdef JADEFUSION
    // SC: Don't add objects with no elements or no points
    if (p_stGRO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
    {
        GEO_tdst_Object* pGeo = (GEO_tdst_Object*)p_stGRO->pst_Base->pst_Visu->pst_Object;

        if ((pGeo->l_NbElements == 0) || (pGeo->l_NbPoints == 0))
            return FALSE;
    }
#endif
	
	if(p_stGRO->pst_Base->pst_Visu->dul_VertexColors) 
		pColor = p_stGRO->pst_Base->pst_Visu->dul_VertexColors + 1;

	ulChannel = p_stGRO->ul_EditorFlags & OBJ_C_EditFlags_Radiosity_Channel_ALL;


	ulFlags =  GLV_FLGS_Original | GLV_FLGS_AIsVisible	| GLV_FLGS_BIsVisible | GLV_FLGS_CIsVisible	/*| GLV_FLGS_DoNotCut*/;
	if (p_stGRO->ul_EditorFlags & OBJ_C_EditFlags_Radiosity_TRANSPARENT) 
		ulFlags |=  GLV_FLGS_Transparent;
	if (p_stGRO->ul_EditorFlags & OBJ_C_EditFlags_Radiosity_DONTCUT) 
		ulFlags |=  GLV_FLGS_DoNotCut;
	if (p_stGRO->ul_EditorFlags & OBJ_C_EditFlags_Radiosity_Nopenombria) 
		ulFlags |=  GLV_FLGS_Nopenombria;

	MaterialFlags = 0;

	if (p_stGRO->ul_EditorFlags & OBJ_C_EditFlags_Radiosity_DONTCUTOTHER)
		ulFlags |=  GLV_FLGS_DontCutOther;
	else
	{
		Counter = 32;

		while (Counter--)
		{
			MAT_tdst_Material	*_pst_Material;
			_pst_Material = (MAT_tdst_Material *)p_stGRO ->pst_Base->pst_Visu->pst_Material;
			if(_pst_Material  && (_pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti))
			{
				if(((MAT_tdst_Multi *)_pst_Material)->l_NumberOfSubMaterials == 0)
					_pst_Material = NULL;
				else
					_pst_Material = ((MAT_tdst_Multi *)_pst_Material)->dpst_SubMaterial[lMin(Counter , ((MAT_tdst_Multi *)_pst_Material)->l_NumberOfSubMaterials - 1)];
			}
			if (_pst_Material && (_pst_Material->st_Id.i->ul_Type == GRO_MaterialMultiTexture))
			{
				MAT_tdst_MultiTexture	*pst_MLTTX;
				MAT_tdst_MTLevel		*pst_MLTTXLVL;
				pst_MLTTX = (MAT_tdst_MultiTexture *) _pst_Material;
				pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;
				while (pst_MLTTXLVL && ((LONG)pst_MLTTXLVL != -1) && (pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_InActive)) pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
				if (pst_MLTTXLVL && ((LONG)pst_MLTTXLVL != -1) )
				{
					/* test transparency */
					if (MAT_GET_Blending(pst_MLTTXLVL->ul_Flags) != MAT_Cc_Op_Copy)
						MaterialFlags |= 1 << Counter;
					if (pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_AlphaTest)
						MaterialFlags |= 1 << Counter;
				}
			}

		}
	}


    if (p_stGRO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
	{
		if ((((GEO_tdst_StaticLOD *) p_stGRO->pst_Base->pst_Visu->pst_Object)->uc_NbLOD == 1) || ((GEO_tdst_Object *) ((GEO_tdst_StaticLOD *) p_stGRO->pst_Base->pst_Visu->pst_Object)->dpst_Id[1] == NULL))
			p_NewGLV = GLV_Geo2GLV((GEO_tdst_Object *) ((GEO_tdst_StaticLOD *) p_stGRO->pst_Base->pst_Visu->pst_Object)->dpst_Id[0],p_stGRO->pst_GlobalMatrix,pColor, ulFlags , &stOBV,MaterialFlags);
		else
		{
			/* Recompute */
			/* RLI are taken in the LOD 0 if possible !! Originals (small mesh) are stored in the big mesh. */
			if (((GEO_tdst_Object *) ((GEO_tdst_StaticLOD *) p_stGRO->pst_Base->pst_Visu->pst_Object)->dpst_Id[0])->dul_PointColors)
				pColor = ((GEO_tdst_Object *) ((GEO_tdst_StaticLOD *) p_stGRO->pst_Base->pst_Visu->pst_Object)->dpst_Id[0])->dul_PointColors + 1;
			else	
				pColor = NULL;
			p_NewGLV = GLV_Geo2GLV((GEO_tdst_Object *) ((GEO_tdst_StaticLOD *) p_stGRO->pst_Base->pst_Visu->pst_Object)->dpst_Id[1],p_stGRO->pst_GlobalMatrix,pColor, ulFlags , &stOBV,MaterialFlags);
		}
	}
	else
	{
		/* create it the first time.*/ 
		/* RLI are taken in the VISU */
		p_NewGLV = GLV_Geo2GLV((GEO_tdst_Object *) p_stGRO ->pst_Base->pst_Visu->pst_Object,p_stGRO->pst_GlobalMatrix,pColor , ulFlags , &stOBV,MaterialFlags);
	}

	GLD_RemoveZeroSurfaces  ( p_NewGLV );
	Polygone1 += p_NewGLV->ulNumberOfFaces;
	GLV_COPY_VARS(p_NewGLV , p_KiltGLV);
	GLV_MARK(p_NewGLV , Mark , ulChannel);
	

	if (BE)
		GLD_BreakEdge ( p_NewGLV , ET );

	/* Clean original */
	GLD_Weld ( p_NewGLV , GLV_WELD_FCT , 0);
	GLD_RemoveIllegalFaces  ( p_NewGLV );
	GLD_RemoveUnusedIndexes ( p_NewGLV );

	/* Cut kilted source */
	GLD_Weld( p_NewGLV , GLV_WELD_FCT , 0);
	GLD_RemoveIllegalFaces  ( p_NewGLV );
	GLD_RemoveUnusedIndexes ( p_NewGLV );
	GLV_ComputeNghbr(p_NewGLV);

	GLV_AddGLV (p_KiltGLV,p_NewGLV);

	stOBV .ulMARK = Mark;
	p_KiltGLV->ulNumberOfOBV++;
	if (p_KiltGLV->p_OBV)
		p_KiltGLV->p_OBV  = (tdst_ObjectBDVolume*)GLV_REALLOC(p_KiltGLV->p_OBV , p_KiltGLV->ulNumberOfOBV * sizeof(tdst_ObjectBDVolume));
	else
		p_KiltGLV->p_OBV  = (tdst_ObjectBDVolume*)GLV_ALLOC( p_KiltGLV->ulNumberOfOBV * sizeof(tdst_ObjectBDVolume));
	p_KiltGLV->p_OBV [ p_KiltGLV->ulNumberOfOBV - 1 ] = stOBV;

	GLV_Destroy(p_NewGLV);

#ifdef JADEFUSION
	return TRUE;
#endif
}
static ULONG (*SeprogressPosReal) (float F01, char *);
static float gs_f_LASTPOS;
static unsigned int MUSTRETUNR = 0;
static ULONG LastGetTime;
ULONG SeprogressPos(float F01, char *DD)
{
	ULONG Delta , tgt;
	tgt = timeGetTime();

	Delta = tgt - LastGetTime;

	if (F01 - gs_f_LASTPOS < 0.0f)
	{
		gs_f_LASTPOS = F01;
		MUSTRETUNR = SeprogressPosReal(F01 , DD);
		LastGetTime = tgt;
		return MUSTRETUNR;
	}
	if ((Delta > 500) || (F01 - gs_f_LASTPOS > 0.01f))
	{
		gs_f_LASTPOS = F01;
		MUSTRETUNR = SeprogressPosReal(F01 , DD);
		LastGetTime = tgt;
		return MUSTRETUNR;
	}
	return MUSTRETUNR;
}
void GLV_CreateJadeObject(WOR_tdst_World *_pst_World , OBJ_tdst_GameObject *pst_GO , GRO_tdst_Struct *pst_Gro , GEO_tdst_Object *p_NewObject)
{
	char			asz_Path[BIG_C_MaxLenPath];
	char			FileName[BIG_C_MaxLenPath];
	char			GroName[BIG_C_MaxLenPath];
	char			OriginalName[BIG_C_MaxLenPath];
	GEO_tdst_StaticLOD *p_LOD;
	GEO_tdst_Object *p_OldObject;
	ULONG Counter , ul_EngineFile;
	ULONG *NewRLI;
	ULONG *OriginalsRLI;
	ULONG MustFindAName;

#if defined(_XENON_RENDER)
    if (GDI_b_IsXenonGraphics() && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        if ((pst_GO->pst_Base->pst_Visu->pst_Object != NULL) &&
            (pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric))
        {
            GEO_ClearXenonMesh(pst_GO->pst_Base->pst_Visu,
                               (GEO_tdst_Object*)pst_GO->pst_Base->pst_Visu->pst_Object);
        }
    }
#endif

	WOR_GetGroPath(_pst_World, asz_Path);
	NewRLI = p_NewObject->dul_PointColors;

	if ((pst_Gro->i->ul_Type == GRO_GeoStaticLOD) && (((GEO_tdst_StaticLOD *)pst_Gro) ->uc_NbLOD != 1))
	{
		MustFindAName = 0;
		sprintf( OriginalName , "%s" , GRO_sz_Struct_GetName( ((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[0]) );
		p_OldObject = (GEO_tdst_Object *) ((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[1];
		OriginalsRLI = ((GEO_tdst_Object *) ((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[0])->dul_PointColors;

#ifdef JADEFUSION
		// SC: Meke sure the colors will be freed
		((GEO_tdst_Object *) ((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[0])->dul_PointColors = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
        pst_GO->pst_Base->pst_Visu->dul_VertexColors = NULL;
		// ! NO !
		/* Resolve pointer nightmare */
#else
		((GEO_tdst_Object *) ((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[0])->dul_PointColors = NULL;

		/* Resolve pointer nightmare */
		{
			GEO_tdst_Object SWAP,*pSWAP,*p_BigOldObject;
			p_BigOldObject = ((GEO_tdst_Object *) ((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[0]);
			SWAP = *p_NewObject;
			*p_NewObject = *p_BigOldObject;
			*p_BigOldObject = SWAP;
			p_BigOldObject->st_Id = p_NewObject->st_Id;
			pSWAP = p_NewObject;
			p_NewObject = p_BigOldObject;
			p_BigOldObject = pSWAP;
		}
		//(((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[0])->l_Ref = 0;
#endif
	}
	else
	{
		MustFindAName = 1;
		p_OldObject = (GEO_tdst_Object *) pst_Gro;
		OriginalsRLI = pst_GO ->pst_Base->pst_Visu->dul_VertexColors;
		sprintf( OriginalName , "%s" , GRO_sz_Struct_GetName( pst_Gro) );
	}

	/* Create GEO */
	if (MustFindAName)
	{
		sprintf(GroName , "%s_RADIED" , OriginalName );
		GRO_Struct_SetName(&p_NewObject->st_Id, GroName);
		sprintf(FileName , "%s_RADED.GRO" , OriginalName );
		ul_EngineFile = BIG_ul_SearchFileExt(asz_Path, FileName);
		if (ul_EngineFile != BIG_C_InvalidIndex)
		if (LOA_ul_SearchAddress(BIG_PosFile(ul_EngineFile)) == -1) ul_EngineFile = BIG_C_InvalidIndex;

		Counter = 0;
		while (ul_EngineFile != BIG_C_InvalidIndex)
		{
			sprintf(FileName, "%s_RADED_%d.GRO" , OriginalName  , Counter ++);
			ul_EngineFile = BIG_ul_SearchFileExt(asz_Path, FileName );
			if (ul_EngineFile != BIG_C_InvalidIndex)
			if (LOA_ul_SearchAddress(BIG_PosFile(ul_EngineFile)) == -1) ul_EngineFile = BIG_C_InvalidIndex;
		}//*/
	} else
	{
		sprintf(FileName , "%s.GRO" , OriginalName );
	}

	if (GEO_SKN_IsSkinned(p_OldObject))
	{
		p_NewObject->p_SKN_Objectponderation = GEO_SKN_Duplicate(p_OldObject->p_SKN_Objectponderation);
		GEO_SKN_Expand( p_NewObject);
		GEO_SKN_Expand( p_OldObject);
		GEO_SubObject_Create(p_NewObject);
		GEO_SKN_AdaptToAnotherSkin( NULL, pst_GO, p_NewObject, p_OldObject, 0, NULL, NULL);
		GEO_SKN_Compress( p_NewObject);
		GEO_SKN_Compress( p_OldObject);
		GEO_SubObject_Free(p_NewObject);
	}

	/* RLI use */
	
	p_NewObject->dul_PointColors = OriginalsRLI;
	pst_GO ->pst_Base->pst_Visu->dul_VertexColors = NewRLI ;


	GRO_ul_Struct_FullSave(&p_NewObject->st_Id, asz_Path , FileName , NULL);

	/* Create LOD */
	if (!MustFindAName)
	{
		p_LOD = (GEO_tdst_StaticLOD*)pst_Gro;
		sprintf(OriginalName , "%s", GRO_sz_Struct_GetName(pst_Gro));
		sprintf(FileName, "%s.GRO", OriginalName);
#ifdef JADEFUSION
		++p_NewObject->st_Id.l_Ref; // SC: Increment for the StaticLOD

        // SC: p_LOD->dpst_Id[0] will most likely have an object attached that must be freed
        if (p_LOD->dpst_Id[0] != NULL)
        {
            p_LOD->dpst_Id[0]->i->pfn_AddRef(p_LOD->dpst_Id[0], -1);
            p_LOD->dpst_Id[0]->i->pfn_Destroy(p_LOD->dpst_Id[0]);
        }

#else
		p_NewObject->st_Id.l_Ref = 2; /* One for the table, one for the GO */
		p_LOD->st_Id.l_Ref = 2; /* One for the table, one for the GO */
#endif
		p_LOD->dpst_Id[0] = (GRO_tdst_Struct*)p_NewObject;

	} else
	{
		p_LOD = GEO_pst_StaticLOD_Create(2);
		sprintf(GroName , "%s_LOD" , OriginalName );
		GRO_Struct_SetName(&p_LOD->st_Id, GroName);

		p_LOD->auc_EndDistance[0] = 2;
		p_LOD->auc_EndDistance[1] = 1;

		p_LOD ->dpst_Id[0] = (GRO_tdst_Struct *)p_NewObject;
#ifdef JADEFUSION
 		p_NewObject->st_Id.l_Ref = 1; // SC: One for the StaticLOD
		p_LOD->st_Id.l_Ref       = 1; // SC: One for the GAO
#else 
		p_NewObject->st_Id.l_Ref = 2; /* One for the table, one for the GO */
		p_LOD->st_Id.l_Ref = 2; /* One for the table, one for the GO */
#endif
		if ((pst_Gro->i->ul_Type == GRO_GeoStaticLOD) && (p_LOD ->uc_NbLOD != 1))
		{
			p_LOD ->dpst_Id[1] = ((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[1];
		}
		else
		{
			p_LOD ->dpst_Id[1] = (GRO_tdst_Struct *) pst_Gro;
		}
#ifdef JADEFUSION
        // SC: Update the graphic object table (GO is using the StaticLOD instead of the GEO now)
        if (pst_GO->pst_World)
        {
            if (p_LOD->dpst_Id[1]->l_Ref > 2)
            {
                // SC: GEO used elsewhere, add the LOD into the table
                TAB_Ptable_AddElemAndResize(&pst_GO->pst_World->st_GraphicObjectsTable, p_LOD);

                // SC: Increment the reference count of the StaticLOD since we added it into the table
                ++p_LOD->st_Id.l_Ref;
            }
            else
            {
                // SC: GEO was only used by this GAO, replace the pointer in the table
                LONG l_Index;

                l_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&pst_GO->pst_World->st_GraphicObjectsTable, p_LOD->dpst_Id[1]);
                if (l_Index != (LONG)TAB_Cul_BadIndex)
                {
                    pst_GO->pst_World->st_GraphicObjectsTable.p_Table[l_Index] = p_LOD;

                    // SC: Increment the reference count of the StaticLOD since we added it into the table
                    ++p_LOD->st_Id.l_Ref;
                }
            }

            // SC: Decrement the reference count since the GAO is not referencing the GEO directly
            --p_LOD->dpst_Id[1]->l_Ref;
        }
#endif
		sprintf(FileName , "%s_LOD.GRO" , OriginalName);
		ul_EngineFile = BIG_ul_SearchFileExt(asz_Path, FileName);
		if (ul_EngineFile != BIG_C_InvalidIndex)
		if (LOA_ul_SearchAddress(BIG_PosFile(ul_EngineFile)) == -1) ul_EngineFile = BIG_C_InvalidIndex;

		Counter = 0;
		while (ul_EngineFile != BIG_C_InvalidIndex)
		{
			sprintf( FileName , "%s_LOD_%d.GRO" , OriginalName , Counter ++);
			ul_EngineFile = BIG_ul_SearchFileExt(asz_Path, FileName );
			if (ul_EngineFile != BIG_C_InvalidIndex)
			if (LOA_ul_SearchAddress(BIG_PosFile(ul_EngineFile)) == -1) ul_EngineFile = BIG_C_InvalidIndex;
		}//*/
	}
	GRO_ul_Struct_FullSave(&p_LOD->st_Id, asz_Path , FileName , NULL);
	(GRO_tdst_Struct *) pst_GO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *)p_LOD;
	pst_GO->pst_Base->pst_Visu->ul_DrawMask &= ~GDI_Cul_DM_UseAmbient;

#if defined(_XENON_RENDER)
    if (GDI_b_IsXenonGraphics() && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        GEO_tdst_Object* pXenonGEO = NULL;

        if (pst_GO->pst_Base->pst_Visu->pst_Object != NULL)
        {
            if (pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
            {
                pXenonGEO = (GEO_tdst_Object*)pst_GO->pst_Base->pst_Visu->pst_Object;
            }
            else if (pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
            {
                GEO_tdst_StaticLOD* pStaticLOD = (GEO_tdst_StaticLOD*)pst_GO->pst_Base->pst_Visu->pst_Object;
                pXenonGEO = (GEO_tdst_Object*)pStaticLOD->dpst_Id[0];
            }
        }

        // Re-create the static mesh
        if (pXenonGEO != NULL)
        {
            GEO_PackGameObject(pst_GO);
        }
    }
#endif
}

void GLV_DestroyRadiosityObject(WOR_tdst_World *_pst_World , OBJ_tdst_GameObject *pst_GO , GRO_tdst_Struct *pst_Gro )
{
	if (pst_Gro->i->ul_Type == GRO_GeoStaticLOD)
	{
		if (((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[1])
		{

#ifdef JADEFUSION
            GEO_tdst_StaticLOD* pLOD          = (GEO_tdst_StaticLOD*)pst_Gro;
            GEO_tdst_Object*    pRadiosityGeo = ((GEO_tdst_Object*)((GEO_tdst_StaticLOD*)pst_Gro)->dpst_Id[0]);
            GEO_tdst_Object*    pOldGeo       = ((GEO_tdst_Object*)((GEO_tdst_StaticLOD*)pst_Gro)->dpst_Id[1]);

#if defined(_XENON_RENDER)
            if (GDI_b_IsXenonGraphics())
            {
                GEO_ClearXenonMesh(pst_GO->pst_Base->pst_Visu, pRadiosityGeo, FALSE, FALSE);
            }
#endif

            // SC: Swap the colors (Revert to original colors and make sure the radiosity colors will be destroyed)
            ULONG* pRadColors = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
            pst_GO->pst_Base->pst_Visu->dul_VertexColors = pRadiosityGeo->dul_PointColors;
            pRadiosityGeo->dul_PointColors = pRadColors;

            // Re-associate the game object with the original GEO
            pst_GO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct*)pOldGeo;

#if defined(_XENON_RENDER)
            if (GDI_b_IsXenonGraphics())
            {
                GEO_PackGameObject(pst_GO);
            }
#endif

            if (pst_GO->pst_World)
            {
                LONG l_Index;

                l_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&pst_GO->pst_World->st_GraphicObjectsTable, pOldGeo);
                if (l_Index == (LONG)TAB_Cul_BadIndex)
                {
                    // SC: Add the old GEO to the graphic objects table if it is not already in there
                    TAB_Ptable_AddElemAndResize(&pst_GO->pst_World->st_GraphicObjectsTable, pOldGeo);
                }
                // SC: Increment the reference count of the GEO since we added it into the table
                ++pOldGeo->st_Id.l_Ref;

                if (pLOD->st_Id.l_Ref == 2)
                {
                    // SC: The GAO was the only one to use the StaticLOD, remove it from the table
                    TAB_Ptable_RemoveElemWithPointer(&pst_GO->pst_World->st_GraphicObjectsTable, pLOD);

                    TAB_Ptable_RemoveHoles(&pst_GO->pst_World->st_GraphicObjectsTable);
                }

                // SC: Increment the reference count of the GEO since the GAO is using it directly
                ++pOldGeo->st_Id.l_Ref;

                // SC: Decrement the reference count since the GAO is not using the StaticLOD anymore
                --pLOD->st_Id.l_Ref;
            }

            // SC: Decrement the reference count of the LOD (Free it if no longer used)
            pLOD->st_Id.i->pfn_AddRef(pLOD, -1);
            pLOD->st_Id.i->pfn_Destroy(pLOD);
        }
    }
#else
			pst_GO->pst_Base->pst_Visu->dul_VertexColors = ((GEO_tdst_Object *) ((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[0])->dul_PointColors;
			(GRO_tdst_Struct *) pst_GO->pst_Base->pst_Visu->pst_Object = ((GEO_tdst_StaticLOD *)pst_Gro)->dpst_Id[1];
		}
	}
#endif
}

OBJ_tdst_GameObject *GLV_GET_SF(OBJ_tdst_GameObject *p_stSon)
{
	if ((OBJ_ul_FlagsIdentityGet(p_stSon) & OBJ_C_IdentityFlag_Hierarchy) && (p_stSon->pst_Base->pst_Hierarchy->pst_Father))
		return GLV_GET_SF(p_stSon->pst_Base->pst_Hierarchy->pst_Father);
	else
		return p_stSon;
}

void	GLV_DestroyRadiosity(WOR_tdst_World *_pst_World , ULONG OnlySel )
{
    TAB_tdst_PFelem     *pst_Elem, *pst_LastElem;
    OBJ_tdst_GameObject *pst_GO;
	GRO_tdst_Struct		*pst_Gro;

	if(!_pst_World) return;
	pst_Elem = _pst_World->st_AllWorldObjects.p_Table;
	pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem;
	/* add each object witch touch the light */
	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		if(TAB_b_IsAHole(pst_Elem->p_Pointer)) continue;
		pst_GO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
		if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) continue;
		if (OBJ_b_TestIdentityFlag(pst_GO, GLV_REMOVE_FLAG)) continue;
		pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
		if (!pst_Gro) continue;
		if (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden) continue;
		if (OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_ForceInvisible)) continue;
		{
			OBJ_tdst_GameObject *SF;
			SF = GLV_GET_SF(pst_GO);
			if (OBJ_b_TestControlFlag(SF, OBJ_C_ControlFlag_ForceInvisible)) continue;
			if (OBJ_b_TestIdentityFlag(SF, OBJ_C_EditFlags_Hidden)) 
				continue;
			if (OBJ_b_TestIdentityFlag(SF, GLV_REMOVE_FLAG))
				continue;
		}
		if ((pst_Gro->i->ul_Type == GRO_Geometric) || (pst_Gro->i->ul_Type == GRO_GeoStaticLOD))
		{
			{
				if (OnlySel && (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected))
				{
					GLV_DestroyRadiosityObject(_pst_World , pst_GO , pst_Gro );
				} 
			}
		}
	}//*/
	LINK_Refresh();
}

void GLV_CLipAndREmove(tdst_GLV *p_GLV , tdst_GLV_Plane *Plane)
{
	ULONG CCC;
	GLV_Clip(p_GLV ,Plane);
	for (CCC = 0 ; CCC < p_GLV->ulNumberOfFaces ; CCC++)
	{
		p_GLV->p_stFaces[CCC].ulFlags &= ~GLV_FLGS_DeleteIt;
		if (p_GLV->p_stFaces[CCC].ulFlags & GLV_FLGS_ClipResultBF)
			p_GLV->p_stFaces[CCC].ulFlags |= GLV_FLGS_DeleteIt;
	}
	GLD_Remove2BD(p_GLV);
}

void	GLV_ComputeAnObject(WOR_tdst_World *_pst_World , ULONG OnlySel , ULONG OPTIMIZE , ULONG BreakEdge , float BreakEdgeThresh , ULONG bEnablePenombria , ULONG SmooothFronteer , ULONG ComputeRadiosity , ULONG RadiosityColor , ULONG bSmoothRadioResult , ULONG bSubdivideForRadiosity , ULONG (*SeprogressPosEXTERN) (float F01, char *))
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_PFelem     *pst_Elem, *pst_LastElem;
    TAB_tdst_PFelem     *pst_ElemLight, *pst_LastElemLight;
    OBJ_tdst_GameObject *pst_GO,*p_Light;
	tdst_GLV			*p_GLV ;
	GRO_tdst_Struct		*pst_Gro;
	LIGHT_tdst_Light    *pst_Light;
	ULONG				ulMARK , ulNumberOfJadeObjects;
	LONG				lTotalTime;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	lTotalTime = timeGetTime();
	SeprogressPosReal = SeprogressPosEXTERN;
	Polygone1 = 0;
	gs_f_LASTPOS = 0.0f;

	ulMARK = 0;
	MUSTRETUNR = 0;

	p_GLV = NewGLV();
#ifdef JADEFUSION
	#if defined(_GLV_KEEP_KILT)
	if (KiltToDraw)
    {
        GLV_Destroy(KiltToDraw);
        KiltToDraw = NULL;
    }
	#endif
	#if defined(_GLV_KEEP_LAST)
    if (p_LastGLV)
    {
        GLV_Destroy(p_LastGLV);
        p_LastGLV = NULL;
    }
	#endif
#else
	if (KiltToDraw) GLV_Destroy(KiltToDraw);
	if (p_LastGLV) GLV_Destroy(p_LastGLV);
	p_LastGLV = NULL;
#endif

	if (p_Octree) GLV_Octree_Destroy(p_Octree);
	p_Octree = NULL;
#ifdef GLV_DEBUG
	if (BugReport) GLV_Destroy(BugReport);
	BugReport = NewGLV();
#endif
#ifndef JADEFUSION
	KiltToDraw = NULL;
#endif
	pst_ElemLight = _pst_World->st_AllWorldObjects.p_Table;
    pst_LastElemLight = _pst_World->st_AllWorldObjects.p_NextElem;
	/* for each light */
	for(; pst_ElemLight < pst_LastElemLight;pst_ElemLight++)
	{
		if(TAB_b_IsAHole(pst_ElemLight->p_Pointer)) continue;
		p_Light = (OBJ_tdst_GameObject *) (pst_ElemLight->p_Pointer);
		if (!(p_Light->ul_EditorFlags & OBJ_C_EditFlags_Radiosity_Channel_ALL)) p_Light->ul_EditorFlags |= OBJ_C_EditFlags_Radiosity_Channel_1;
		if(!OBJ_b_TestIdentityFlag(p_Light, OBJ_C_IdentityFlag_Lights)) continue;
		if (p_Light->ul_EditorFlags & OBJ_C_EditFlags_Hidden) continue;
		if (OBJ_b_TestControlFlag(p_Light , OBJ_C_ControlFlag_ForceInvisible)) continue;
		if(OBJ_b_TestIdentityFlag(p_Light, OBJ_C_IdentityFlag_Dyna)) continue;
		pst_Light = (LIGHT_tdst_Light *) p_Light->pst_Extended->pst_Light;
		if(!pst_Light) continue;
		if(!(pst_Light->ul_Flags & LIGHT_Cul_LF_Active)) continue;
//		if(!(pst_Light->ul_Flags & LIGHT_Cul_LF_RLIOnNonDynam)) continue;
	
		switch(pst_Light->ul_Flags & LIGHT_Cul_LF_Type)
		{
		case LIGHT_Cul_LF_Direct:
			if (p_GLV->p_Lights)
				p_GLV ->p_Lights = (tdst_LightDescription*)GLV_REALLOC(p_GLV->p_Lights , sizeof(tdst_LightDescription) * (p_GLV->ulNumberOfLight + 1));
			else
				p_GLV ->p_Lights = (tdst_LightDescription*)GLV_ALLOC(sizeof(tdst_LightDescription) * (p_GLV->ulNumberOfLight + 1));
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulLightFlags = pst_Light->ul_Flags;
			if (bEnablePenombria && (pst_Light->ul_Flags & LIGHT_Cul_LF_EnablePenombria))
				p_GLV->p_Lights[p_GLV->ulNumberOfLight].bLightPenombriaOn = 1;
			else
				p_GLV->p_Lights[p_GLV->ulNumberOfLight].bLightPenombriaOn = 0;

			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightRadius = 5.0f * 3.1415927f / 180.0f; /* 5° */
			MATH_To_MATHD(&p_GLV->p_Lights[p_GLV->ulNumberOfLight].LDir ,MATH_pst_GetYAxis(p_Light->pst_GlobalMatrix));
			p_GLV->p_Lights[p_GLV->ulNumberOfLight].ulColor = pst_Light->ul_Color;
			if (p_GLV ->p_Lights[p_GLV->ulNumberOfLight ].ulLightFlags & LIGHT_Cul_LF_Paint)
				p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulLightFlags &= ~LIGHT_Cul_LF_CastShadows;
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulChannel = p_Light->ul_EditorFlags & OBJ_C_EditFlags_Radiosity_Channel_ALL;
			p_GLV->ulNumberOfLight ++;

			GLV_ComputeNormales(p_GLV);
			break;
		case LIGHT_Cul_LF_Omni:
			if (p_GLV->p_Lights)
				p_GLV ->p_Lights = (tdst_LightDescription*)GLV_REALLOC(p_GLV->p_Lights , sizeof(tdst_LightDescription) * (p_GLV->ulNumberOfLight + 1));
			else
				p_GLV ->p_Lights = (tdst_LightDescription*)GLV_ALLOC(sizeof(tdst_LightDescription) * (p_GLV->ulNumberOfLight + 1));
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulLightFlags = pst_Light->ul_Flags;
			if (bEnablePenombria && (pst_Light->ul_Flags & LIGHT_Cul_LF_EnablePenombria))
				p_GLV->p_Lights[p_GLV->ulNumberOfLight].bLightPenombriaOn = 1;
			else
				p_GLV->p_Lights[p_GLV->ulNumberOfLight].bLightPenombriaOn = 0;

			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightRadius = pst_Light->st_Omni.f_Near;
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightNear = pst_Light->st_Omni.f_Near;
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightFar  = pst_Light->st_Omni.f_Far;
			MATH_To_MATHD(&p_GLV->p_Lights[p_GLV->ulNumberOfLight].stLightCenterOS ,&p_Light->pst_GlobalMatrix->T);
			p_GLV->p_Lights[p_GLV->ulNumberOfLight].ulColor = pst_Light->ul_Color;
			GLV_CreateSphere(p_GLV , GLV_FLGS_Kilt |GLV_FLGS_BlackFront| GLV_FLGS_DistanceFar , p_GLV->ulNumberOfLight , &p_GLV->p_Lights[p_GLV->ulNumberOfLight].stLightCenterOS , p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightFar);
			GLV_CreateSphere(p_GLV , GLV_FLGS_Kilt |GLV_FLGS_BlackFront| GLV_FLGS_Transparent | GLV_FLGS_DistanceNear, p_GLV->ulNumberOfLight , &p_GLV->p_Lights[p_GLV->ulNumberOfLight].stLightCenterOS , p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightNear);
			if (p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulLightFlags & LIGHT_Cul_LF_Paint)
				p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulLightFlags &= ~LIGHT_Cul_LF_CastShadows;
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulChannel = p_Light->ul_EditorFlags & OBJ_C_EditFlags_Radiosity_Channel_ALL;
			p_GLV->ulNumberOfLight ++;
			GLV_ComputeNormales(p_GLV);
			break;
		case LIGHT_Cul_LF_Spot:
			if (p_GLV->p_Lights)
				p_GLV ->p_Lights = (tdst_LightDescription*)GLV_REALLOC(p_GLV->p_Lights , sizeof(tdst_LightDescription) * (p_GLV->ulNumberOfLight + 1));
			else
				p_GLV ->p_Lights = (tdst_LightDescription*)GLV_ALLOC(sizeof(tdst_LightDescription) * (p_GLV->ulNumberOfLight + 1));
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulLightFlags = pst_Light->ul_Flags;
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightRadius = pst_Light->st_Omni.f_Near;
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightNear = pst_Light->st_Spot.f_Near / (GLV_Scalar)cos (pst_Light->st_Spot.f_LittleAlpha);
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightFar  = pst_Light->st_Spot.f_Far  / (GLV_Scalar)cos (pst_Light->st_Spot.f_BigAlpha   );
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fCosBigAlpha = (GLV_Scalar)cos (pst_Light->st_Spot.f_BigAlpha   );
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fCosLittleAlpha = (GLV_Scalar)cos (pst_Light->st_Spot.f_LittleAlpha   );
			if (bEnablePenombria && (pst_Light->ul_Flags & LIGHT_Cul_LF_EnablePenombria))
				p_GLV->p_Lights[p_GLV->ulNumberOfLight].bLightPenombriaOn = 1;
			else
				p_GLV->p_Lights[p_GLV->ulNumberOfLight].bLightPenombriaOn = 0;
			MATH_To_MATHD(&p_GLV->p_Lights[p_GLV->ulNumberOfLight].stLightCenterOS ,&p_Light->pst_GlobalMatrix->T);
			MATH_To_MATHD(&p_GLV->p_Lights[p_GLV->ulNumberOfLight].LDir ,MATH_pst_GetYAxis(p_Light->pst_GlobalMatrix));
			
			p_GLV->p_Lights[p_GLV->ulNumberOfLight].ulColor = pst_Light->ul_Color;
			GLV_CreateSphere(p_GLV , GLV_FLGS_Kilt |GLV_FLGS_BlackFront| GLV_FLGS_DistanceFar , p_GLV->ulNumberOfLight , &p_GLV->p_Lights[p_GLV->ulNumberOfLight].stLightCenterOS , p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightFar);
			GLV_CreateSphere(p_GLV , GLV_FLGS_Kilt |GLV_FLGS_BlackFront| GLV_FLGS_Transparent | GLV_FLGS_DistanceNear, p_GLV->ulNumberOfLight , &p_GLV->p_Lights[p_GLV->ulNumberOfLight].stLightCenterOS , p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightNear);
			GLV_CreateCone(p_GLV , GLV_FLGS_Kilt   |GLV_FLGS_BlackFront| GLV_FLGS_Transparent | GLV_FLGS_DistanceNear, p_GLV->ulNumberOfLight  , &p_GLV->p_Lights[p_GLV->ulNumberOfLight].stLightCenterOS  , &p_GLV->p_Lights[p_GLV->ulNumberOfLight].LDir , pst_Light->st_Spot.f_LittleAlpha  , p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightFar * 1.05f);
			GLV_CreateCone(p_GLV , GLV_FLGS_Kilt   |GLV_FLGS_BlackFront| GLV_FLGS_Transparent | GLV_FLGS_DistanceFar, p_GLV->ulNumberOfLight  , &p_GLV->p_Lights[p_GLV->ulNumberOfLight].stLightCenterOS  , &p_GLV->p_Lights[p_GLV->ulNumberOfLight].LDir , pst_Light->st_Spot.f_BigAlpha  , p_GLV ->p_Lights[p_GLV->ulNumberOfLight].fLightFar * 1.05f);
			if (p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulLightFlags & LIGHT_Cul_LF_Paint)
				p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulLightFlags &= ~LIGHT_Cul_LF_CastShadows;
			p_GLV ->p_Lights[p_GLV->ulNumberOfLight].ulChannel = p_Light->ul_EditorFlags & OBJ_C_EditFlags_Radiosity_Channel_ALL;
			p_GLV->ulNumberOfLight ++;
			GLV_ComputeNormales(p_GLV);
			continue;
		case LIGHT_Cul_LF_Fog:
			continue;
		}

	}


	pst_Elem = _pst_World->st_AllWorldObjects.p_Table;
	pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem;
	ulMARK = 0;
	ulNumberOfGO = 0;
	ulNumberOfJadeObjects = 0;
	/* add each object witch touch the light */
	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		if(TAB_b_IsAHole(pst_Elem->p_Pointer)) continue;
		pst_GO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
		if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) continue;
		if (OBJ_b_TestIdentityFlag(pst_GO, GLV_REMOVE_FLAG)) continue;
		pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
		if (!pst_Gro) continue;
		if (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden) continue;
		if (OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_ForceInvisible)) continue;
		{
			OBJ_tdst_GameObject *SF;
			SF = GLV_GET_SF(pst_GO);
			if (OBJ_b_TestControlFlag(SF, OBJ_C_ControlFlag_ForceInvisible)) continue;

			if (OBJ_b_TestIdentityFlag(SF, OBJ_C_EditFlags_Hidden)) 
				continue;
			if (OBJ_b_TestIdentityFlag(SF, GLV_REMOVE_FLAG)) 
				continue;
		}
		if ((pst_Gro->i->ul_Type == GRO_Geometric) || (pst_Gro->i->ul_Type == GRO_GeoStaticLOD))
		{
			{
				p_AllComputedGO[ulNumberOfGO++] = pst_GO;

#ifdef JADEFUSION
				if (OnlySel && (!(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)))
                {
					if (GLV_AddGRO(p_GLV , pst_GO , SeprogressPos , 0x80000000 | ulMARK++ , BreakEdge , BreakEdgeThresh))
                        p_AllComputedGO[ulNumberOfGO++] = pst_GO;
                }
				else
                {
					if (GLV_AddGRO(p_GLV , pst_GO , SeprogressPos , 0xC0000000 | ulMARK++ , BreakEdge , BreakEdgeThresh))
                        p_AllComputedGO[ulNumberOfGO++] = pst_GO;
                }
#else
				if (OnlySel && (!(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)))
					GLV_AddGRO(p_GLV , pst_GO , SeprogressPos , 0x80000000 | ulMARK++ , BreakEdge , BreakEdgeThresh   );
				else
					GLV_AddGRO(p_GLV , pst_GO , SeprogressPos , 0xC0000000 | ulMARK++ , BreakEdge , BreakEdgeThresh   );
#endif
			}
		}
	}//*/
	ulNumberOfJadeObjects = ulMARK;
	/* Compute Direct Far - Near */
	{
		ULONG ulLightCounter;
		GLV_ComputeNghbr(p_GLV);
		GLV_ComputeConcavesEdges(p_GLV);
		ulLightCounter = p_GLV->ulNumberOfLight ;
		while (ulLightCounter--)
		{
			if ((p_GLV ->p_Lights[ulLightCounter].ulLightFlags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct)
			{
				GLV_Scalar Dist;
				ULONG CounterX;
				p_GLV ->p_Lights[ulLightCounter].fLightNear = MATHD_f_DotProduct(&p_GLV ->p_stPoints[0].P3D , &p_GLV ->p_Lights[ulLightCounter].LDir);
				p_GLV ->p_Lights[ulLightCounter].fLightFar = MATHD_f_DotProduct(&p_GLV ->p_stPoints[0].P3D , &p_GLV ->p_Lights[ulLightCounter].LDir);
				for (CounterX = 0 ; CounterX < p_GLV ->ulNumberOfPoints ; CounterX++)
				{
					Dist = MATHD_f_DotProduct(&p_GLV ->p_stPoints[CounterX].P3D , &p_GLV ->p_Lights[ulLightCounter].LDir);
					if (Dist < p_GLV ->p_Lights[ulLightCounter].fLightFar) p_GLV ->p_Lights[ulLightCounter].fLightFar = Dist ;
					if (Dist > p_GLV ->p_Lights[ulLightCounter].fLightNear) p_GLV ->p_Lights[ulLightCounter].fLightNear = Dist ;
				}
				p_GLV ->p_Lights[ulLightCounter].fLightNear += GLF_FaceExtraction * 20.0f;
				p_GLV ->p_Lights[ulLightCounter].fLightFar  -= GLF_FaceExtraction * 20.0f;
			}
		}
	}
		
//	GLV_FirstLighSubdivide(p_GLV , SeprogressPos);	
	{
		ULONG ulLightCounter;
		GLV_ComputeNghbr(p_GLV);
		GLV_ComputeConcavesEdges(p_GLV);
		GLV_SetChannelOnPoints( p_GLV );
		ulLightCounter = p_GLV->ulNumberOfLight ;
		while (ulLightCounter--)
		{
			GLV_Scalar SaveRadius;
			if (p_GLV ->p_Lights[ulLightCounter].ulLightFlags & LIGHT_Cul_LF_CastShadows)
			{
				SaveRadius = p_GLV ->p_Lights [ulLightCounter ]  .fLightRadius;
				p_GLV ->p_Lights [ulLightCounter ]  .fLightRadius = GLV_LIGHTRADIUSMIN;
				GLV_ComputeBckFace ( p_GLV , ulLightCounter );
				GLV_GenerateKILT( p_GLV , GLV_FLGS_Kilt | GLV_FLGS_BlackFront , ulLightCounter);
				if (p_GLV ->p_Lights [ulLightCounter ].bLightPenombriaOn)
				{
					ULONG ulRecoverCounter;
					ulRecoverCounter = p_GLV->ulNumberOfPoints;
					while (ulRecoverCounter--)
						p_GLV->p_stPoints[ulRecoverCounter].ulRefIndex = p_GLV->p_stPoints[ulRecoverCounter].DownIndex;
					p_GLV ->p_Lights [ulLightCounter ]  . fLightRadius = SaveRadius;
					GLV_ComputeBckFace ( p_GLV , ulLightCounter );
					GLV_GenerateKILT( p_GLV , GLV_FLGS_Kilt |  GLV_FLGS_WhiteFront , ulLightCounter);
					ulRecoverCounter = p_GLV->ulNumberOfPoints;
					while (ulRecoverCounter--)
					{
						if ((p_GLV->p_stPoints[ulRecoverCounter].ulRefIndex != -1) && (p_GLV->p_stPoints[ulRecoverCounter].DownIndex != -1))
						{
							// Add curtain special kilt 
							// Create a triangle 
							GLV_SetNumbers(p_GLV , 0 , 0 , p_GLV->ulNumberOfFaces + 1 , 4);
							p_GLV ->p_stFaces[p_GLV ->ulNumberOfFaces-1].ulFlags = GLV_FLGS_Kilt|GLV_FLGS_BlackFront|GLV_FLGS_Transparent;
							p_GLV ->p_stFaces[p_GLV ->ulNumberOfFaces-1].Index[0] = p_GLV->p_stPoints[ulRecoverCounter].ulRefIndex;
							p_GLV ->p_stFaces[p_GLV ->ulNumberOfFaces-1].Index[1] = p_GLV->p_stPoints[ulRecoverCounter].DownIndex;
							p_GLV ->p_stFaces[p_GLV ->ulNumberOfFaces-1].Index[2] = ulRecoverCounter;
							p_GLV ->p_stFaces[p_GLV ->ulNumberOfFaces-1].Nghbr[0] = p_GLV ->p_stFaces[p_GLV ->ulNumberOfFaces-1].Nghbr[1] = p_GLV ->p_stFaces[p_GLV ->ulNumberOfFaces-1].Nghbr[2] = -1;
							p_GLV ->p_stFaces[p_GLV ->ulNumberOfFaces-1].ulMARK = ulLightCounter;
							p_GLV ->p_stFaces[p_GLV ->ulNumberOfFaces-1].ulChannel = p_GLV ->p_Lights [ulLightCounter ].ulChannel &  p_GLV->p_stPoints[ulRecoverCounter].ulChannel & p_GLV->p_stPoints[p_GLV->p_stPoints[ulRecoverCounter].DownIndex].ulChannel & p_GLV->p_stPoints[p_GLV->p_stPoints[ulRecoverCounter].ulRefIndex].ulChannel;
							GLV_ComputeNormale(p_GLV,&p_GLV ->p_stFaces[p_GLV ->ulNumberOfFaces-1]);
						}
					}//*/
				}
				p_GLV ->p_Lights [ulLightCounter ]  .fLightRadius = SaveRadius ;
			}
		}
	}
	if (ComputeRadiosity && bSubdivideForRadiosity)
	{
		ULONG CounterMax;
		CounterMax = 10;
		while (CounterMax && GLV_ComputeRadiosity_Compute_FF_Precut (p_GLV,SeprogressPos )) {CounterMax--;};
	}
#ifdef JADEFUSION
	#if defined(_GLV_KEEP_KILT)
	if (KiltToDraw)
	{
		GLV_Destroy(KiltToDraw);
		KiltToDraw = NULL;
	}
	#endif
#else
	if (KiltToDraw)
	{
		GLV_Destroy(KiltToDraw);
		KiltToDraw = NULL;	
	}
#endif

	GLD_RemoveZeroSurfaces  ( p_GLV );
	GLD_RemoveUnusedIndexes ( p_GLV );
	GLD_Weld( p_GLV , GLV_WELD_FCT , 0);
	GLD_RemoveIllegalFaces  ( p_GLV );
	GLD_RemoveUnusedIndexes ( p_GLV );
	GLV_ComputeNghbr(p_GLV);

//	GLV_ComputeNghbr(p_GLV);
	GLV_SetNumbers(p_GLV , 0 , 0 , 0 , 2); // Erase all edges
	GLD_RemoveZeroSurfaces  ( p_GLV );
//	GLV_FirstLighSubdivide(p_GLV , SeprogressPos);
	/* Cut the kilt with the kilt */
	/* remove some face */
	if (1)
	{
		/* build  OK3 ALL */
		static GLV_OCtreeNode stOctreeROOT;
		ULONG C1;
		L_memset(&stOctreeROOT , 0 , sizeof (GLV_OCtreeNode));
		p_Octree = &stOctreeROOT ;
		stOctreeROOT.p_OctreeFather = NULL;
		stOctreeROOT.p_stThisCLT = GLV_NewCluster();
		C1 = p_GLV->ulNumberOfFaces;
		while (C1--) GLV_Clst_ADDV(stOctreeROOT.p_stThisCLT , C1); /* Add all the faces */
		GLV_ComputeGLVBox(p_GLV , &stOctreeROOT.stAxisMin, &stOctreeROOT.stAxisMax , GLV_FLGS_Original);
		SeprogressPos (0.0f, "Build OK3..");
		GLV_Octree_DiveAndCompute( p_GLV , &stOctreeROOT , GLV_OC3_MAX_DEPTH , GLV_OC3_MAX_FPO , SeprogressPos);
		GLV_CutAll(p_GLV , SeprogressPos , &stOctreeROOT);
		if (MUSTRETUNR) return;
		GLV_RemoveOverlapedEdges( p_GLV , SeprogressPos , &stOctreeROOT);
		if (MUSTRETUNR) return;
		/* 1 OK3 ORIGINALS */
/*		GLV_DetectOO(p_GLV , SeprogressPos);
		if (MUSTRETUNR) return;//*/
		/* 1 OK3 EDGES */
#ifdef JADEFUSION
#if defined(_GLV_KEEP_KILT)
		{
			KiltToDraw = GLV_Duplicate(p_GLV);
			GLV_ExtractEdges(KiltToDraw );
			GLV_RemoveOriginals(KiltToDraw );
			GLV_VisibleIfNeight(KiltToDraw );
			LINK_Refresh();
		}
#endif
#else
		{
			KiltToDraw = GLV_Duplicate(p_GLV);
			GLV_ExtractEdges(KiltToDraw );
			GLV_RemoveOriginals(KiltToDraw );
			GLV_VisibleIfNeight(KiltToDraw );
			LINK_Refresh();
		}
#endif
		GLV_CutWithCoplanrEdges(p_GLV , SeprogressPos);
		if (MUSTRETUNR) return;
		/* 2 OK3 
		- KILT non whitefront & originals
		- KILT whitefront 
		*/
		GLD_Weld( p_GLV , GLV_WELD_FCT , 0);
//		GLD_MakeItShorter(p_GLV,0);//*/
		GLV_ComputeColors(p_GLV , SmooothFronteer , SeprogressPos);//*/
		if (MUSTRETUNR) return;
		//GLV_Octree_Destroy(&stOctreeROOT);
#ifdef JADEFUSION
        // SC: Free the octree
        GLV_Octree_Destroy(p_Octree);
        p_Octree = NULL;
#endif
	}

 

	/* cut originals with originals */
/*	KiltToDraw = GLV_Duplicate(p_GLV);
	GLV_SetNumbers(KiltToDraw , 0 , 0 , 0 , 4); 
/*	LINK_Refresh();//*/
#if 1
	GLV_RemoveKILT(p_GLV);
	GLD_RemoveUnusedIndexes(p_GLV);
	if (ComputeRadiosity)
		GLV_ComputeRadiosity_Compute_FF_Fast_RT(p_GLV , RadiosityColor , bSmoothRadioResult , bSubdivideForRadiosity  , SeprogressPos);//*/
#ifdef JADEFUSION
	#if defined(_GLV_KEEP_LAST)
    p_LastGLV = GLV_Duplicate(p_GLV);
	#endif
#else
	p_LastGLV = GLV_Duplicate(p_GLV);
#endif

	if (ComputeRadiosity)
		GLV_ComputeRadiosity_Use_FF(p_GLV , RadiosityColor , bSmoothRadioResult , bSubdivideForRadiosity  , SeprogressPos);//*/
	/* Compute colors */

	GLV_CutBooleanOverlad(p_GLV , 1);
	if (OPTIMIZE)
	{
		char T[1024];
		/* optimize result */
		sprintf(T , "Begin optimize LVL %d.." , OPTIMIZE);
		GLV_BUG(0, T);
		GLV_Optimize(p_GLV , OPTIMIZE , 1 , SeprogressPos);
			GLD_MakeItShorter( p_GLV , 0);
			GLD_Weld( p_GLV , GLV_WELD_FCT  , 1);
			GLD_RemoveIllegalFaces  ( p_GLV );
			GLD_RemoveUnusedIndexes ( p_GLV );
			GLV_ComputeNghbr(p_GLV);
		GLV_Optimize(p_GLV , OPTIMIZE , 1 , SeprogressPos);
			GLD_MakeItShorter( p_GLV , 0);
			GLD_Weld( p_GLV , GLV_WELD_FCT  , 1);
			GLD_RemoveIllegalFaces  ( p_GLV );
			GLD_RemoveUnusedIndexes ( p_GLV );
			GLV_ComputeNghbr(p_GLV);
		GLV_Optimize(p_GLV , OPTIMIZE , 1 , SeprogressPos);
			GLD_MakeItShorter( p_GLV , 0);
			GLD_Weld( p_GLV , GLV_WELD_FCT  , 1);
			GLD_RemoveIllegalFaces  ( p_GLV );
			GLD_RemoveUnusedIndexes ( p_GLV );
			GLV_ComputeNghbr(p_GLV);
		GLV_Optimize(p_GLV , OPTIMIZE , 1 , SeprogressPos);
			GLD_MakeItShorter( p_GLV , 0);
			GLD_Weld( p_GLV , GLV_WELD_FCT  , 1);
			GLD_RemoveIllegalFaces  ( p_GLV );
			GLD_RemoveUnusedIndexes ( p_GLV );
			GLV_ComputeNghbr(p_GLV);
		GLV_BUG(0, "Optimize finished");
	}
	Polygone2 = p_GLV -> ulNumberOfFaces;
	/* Re-create objects */
	pst_Elem = _pst_World->st_AllWorldObjects.p_Table;
	pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem;
	ulMARK = 0;
	/* add each object witch touch the light */
	GLV_BUG(0, "Begin Re-create jade object");
	ulMARK = ulNumberOfGO;
	while (ulMARK--)
	{
		GEO_tdst_Object *p_NewObj;
		SeprogressPos( (float)(ulNumberOfGO - ulMARK) / (float)ulNumberOfJadeObjects, "Create jade objects");
		pst_GO = p_AllComputedGO[ulMARK];
		pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
		if (!pst_Gro) continue;
		p_NewObj = GLV_GLV2GEO (p_GLV ,pst_GO->pst_GlobalMatrix , ulMARK);
		GLV_CreateJadeObject ( _pst_World  , pst_GO , pst_Gro , p_NewObj);
	}//*/
	GLV_BUG(0, "End Re-create jade object");
#endif
	lTotalTime = timeGetTime() - lTotalTime;
	{
		char TimeSpent[1024];
		ULONG Deciseconds , Seconds , Minutes , Hours , Days;
		Deciseconds = lTotalTime / 100L;
		Seconds = lTotalTime / 1000L;
		Minutes = lTotalTime / 60000L;
		Hours   = lTotalTime / 3600000L;
		Days	= Hours		 / 24L;
		Deciseconds %= 10;
		Seconds %= 60;
		Minutes %= 60;
		Hours %= 24;
		sprintf(TimeSpent , "Shadow computation time : ");
		if (Days) 
			sprintf(TimeSpent + strlen(TimeSpent), "%d Day(s) " , Days );
		if (Days||Hours) 
			sprintf(TimeSpent + strlen(TimeSpent), "%d Hours(s) " , Hours );
		if (Days||Hours||Minutes) 
			sprintf(TimeSpent + strlen(TimeSpent), "%d Minutes(s) " , Minutes );
		if (Days||Hours||Minutes||Seconds||Deciseconds) 
			sprintf(TimeSpent + strlen(TimeSpent), "%d,%d Second(s) " , Seconds , Deciseconds);
		GLV_BUG(0 , TimeSpent)	;
		sprintf(TimeSpent , "Polygones Ration : + %d %% (%d -> %d polygones)" , (ULONG)(100.0f * ( (float)Polygone2  / (float)Polygone1) - 100.0f ) , Polygone1 , Polygone2 );
		GLV_BUG(0 , TimeSpent)	;
	}
#ifdef GLV_DEBUG
	GLV_SetNumbers(BugReport , 0 , 0 , 0 , 4);
#endif
	GLV_Destroy(p_GLV);

#ifdef JADEFUSION
    // SC: Clean up memory
    GLV_FreeClusterMem();
    GLV_FreeFlagMem();

#if defined(_GLV_TRACE_MEMORY)
    GLV_DumpMemory();
#endif
#endif
	LINK_Refresh();
}

void GLV_AdjustRadiosity(WOR_tdst_World *_pst_World , ULONG OnlySel , ULONG OPTIMIZE , ULONG BreakEdge , float BreakEdgeThresh , ULONG bEnablePenombria , ULONG SmooothFronteer , ULONG ComputeRadiosity , ULONG RadiosityColor , ULONG bSmoothRadioResult , ULONG bSubdivideForRadiosity, ULONG (*SeprogressPosEXTERN) (float F01, char *))
{
#	if defined( _GLV_KEEP_LAST )//POPO WARNING rajout

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
	GRO_tdst_Struct		*pst_Gro;
	ULONG				ulMARK ;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (p_LastGLV)
	{
		tdst_GLV *p_GLV = NULL;
		if (ComputeRadiosity)
		{
			if (!p_LastGLV->p_FormFactors)	GLV_ComputeRadiosity_Compute_FF_Fast_RT (p_LastGLV , RadiosityColor , bSmoothRadioResult , bSubdivideForRadiosity  , SeprogressPos);//*/
		}

		p_GLV = GLV_Duplicate(p_LastGLV );

		if (ComputeRadiosity)
			GLV_ComputeRadiosity_Use_FF(p_GLV , RadiosityColor , bSmoothRadioResult , bSubdivideForRadiosity  , SeprogressPos);//*/

		/* Compute colors */
		GLV_CutBooleanOverlad(p_GLV , 1);
		if (OPTIMIZE)
		{
			char T[1024];
			/* optimize result */
			sprintf(T , "Begin optimize LVL %d.." , OPTIMIZE);
			GLV_BUG(0, T);
			GLV_Optimize(p_GLV , OPTIMIZE , 1 , SeprogressPos);
				GLD_MakeItShorter( p_GLV , 0);
				GLD_Weld( p_GLV , GLV_WELD_FCT  , 1);
				GLD_RemoveIllegalFaces  ( p_GLV );
				GLD_RemoveUnusedIndexes ( p_GLV );
				GLV_ComputeNghbr(p_GLV);
			GLV_Optimize(p_GLV , OPTIMIZE , 1 , SeprogressPos);
				GLD_MakeItShorter( p_GLV , 0);
				GLD_Weld( p_GLV , GLV_WELD_FCT  , 1);
				GLD_RemoveIllegalFaces  ( p_GLV );
				GLD_RemoveUnusedIndexes ( p_GLV );
				GLV_ComputeNghbr(p_GLV);
			GLV_Optimize(p_GLV , OPTIMIZE , 1 , SeprogressPos);
				GLD_MakeItShorter( p_GLV , 0);
				GLD_Weld( p_GLV , GLV_WELD_FCT  , 1);
				GLD_RemoveIllegalFaces  ( p_GLV );
				GLD_RemoveUnusedIndexes ( p_GLV );
				GLV_ComputeNghbr(p_GLV);
			GLV_Optimize(p_GLV , OPTIMIZE , 1 , SeprogressPos);
				GLD_MakeItShorter( p_GLV , 0);
				GLD_Weld( p_GLV , GLV_WELD_FCT  , 1);
				GLD_RemoveIllegalFaces  ( p_GLV );
				GLD_RemoveUnusedIndexes ( p_GLV );
				GLV_ComputeNghbr(p_GLV);
			GLV_BUG(0, "Optimize finished");
		}
		Polygone2 = p_GLV -> ulNumberOfFaces;
		/* Re-create objects */
		ulMARK = 0;
		/* add each object witch touch the light */
		GLV_BUG(0, "Begin Re-create jade object");
		ulMARK = ulNumberOfGO;
		while (ulMARK--)
		{
			GEO_tdst_Object *p_NewObj;
			SeprogressPos( (float)(ulNumberOfGO - ulMARK) / (float)ulNumberOfGO, "Create jade objects");
			pst_GO = p_AllComputedGO[ulMARK];
			if (OnlySel && (!(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected))) continue;
			pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
			if (!pst_Gro) continue;
			p_NewObj = GLV_GLV2GEO (p_GLV ,pst_GO->pst_GlobalMatrix , ulMARK);
			GLV_CreateJadeObject ( _pst_World  , pst_GO , pst_Gro , p_NewObj);
		}//*/
		GLV_BUG(0, "End Re-create jade object");
		Polygone2 = p_GLV -> ulNumberOfFaces;
		{
			char TimeSpent[1024];
			sprintf(TimeSpent , "Polygones Ration : + %d %% (%d -> %d polygones)" , (ULONG)(100.0f * ( (float)Polygone2  / (float)Polygone1) - 100.0f ) , Polygone1 , Polygone2 );
			GLV_BUG(0 , TimeSpent)	;
		}

		LINK_Refresh();
		GLV_Destroy(p_GLV);
	}

#endif
}

#endif
#if 0


#endif

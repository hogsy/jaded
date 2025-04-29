/*$T GEOobject.c GC! 1.081 03/06/01 09:37:33 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
#include "Precomp.h"
//#include "opengl/sources/OGLinit.h"

#ifdef PSX2_TARGET
#   include "PSX2debug.h"
#endif

#ifdef _GAMECUBE
#   include "GXI_GC/GXI_def.h"
#endif

#ifdef ACTIVE_EDITORS
#   include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#   include "MAD_mem/Sources/MAD_mem.h"
#	include "Engine/sources/OBJects/OBJslowaccess.h"
#endif

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"
#ifdef JADEFUSION
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#endif
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine\Sources\ANImation\ANIinit.h"

#ifdef ACTIVE_EDITORS
#   include "ENGine/Sources/COLlision/COLstruct.h"
#endif

#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/COLlision/COLload.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/MODifier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SDW.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_PROTEX.h"

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
#ifdef JADEFUSION
#include "GEOmetric/GEOstatistics.h"
#endif
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEO_STRIP.h"
#include "GEOmetric/GEO_SKIN.h"
#include "MATerial/MATShadow.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTlinear.h"
#include "SOFT/SOFTPickingBuffer.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"
#include "LINks/LINKtoed.h"

#ifdef ACTIVE_EDITORS
// For special display mode
#include "GEOmetric/GEOstaticLOD.cpp"
ULONG GAODisplayFlag;
#endif


#ifdef PSX2_TARGET
#   include <eeregs.h>
#   include <eestruct.h>
#   include <libgraph.h>
#   include <libdma.h>
#   include <libvu0.h>
#   include <sifdev.h>
#   include <libpc.h>
#   include "GS_PS2/Gsp.h"
#   define _PSX2_DSPLS
#endif

#include "BASe/BENch/BENch.h"

#ifdef _GAMECUBE
#   define _GAMECUBE_DSPLS
#   ifndef _FINAL_
#       include "GXI_GC/GXI_dbg.h"
#   endif
#endif

#if defined(_GAMECUBE_DSPLS) || defined(_PSX2_DSPLS)
#define _DSPLS
//#define NO_GC_FUR
#if (defined _PSX2_DSPLS) || (defined NO_GC_FUR)
#define _DSPL_ALLOC
#endif // _PSX2_DSPLS
#endif // #if defined(_GAMECUBE_DSPLS) || defined(_PSX2_DSPLS)

#ifdef _XBOX
#  include <assert.h> 
#  include "GX8\Gx8VertexBuffer.h" 
#  include "GX8\Gx8light.h" 
#  include "GX8\Gx8AddInfo.h" 
#  include "GX8\Gx8BuildUVs.h" 
#  include "Gx8/Gx8buffer.h" 
#  include "GX8/Gx8color.h"

extern Gx8_tdst_SpecificData   * p_gGx8SpecificData;

#if !defined(_XENON)
#define	OPT_MMX_SUB_LOOP
#endif

#endif // _XBOX

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeMesh.h"
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeVertexShaderManager.h"
#include "XenonGraphics/XeGDInterface.h"
#include "XenonGraphics/XeWaterManager.h"
#include "XenonGraphics/XeDynVertexBuffer.h"
#include "XenonGraphics/XeSharedDefines.h"

#include "GEOXenonPack.h"
#endif

#if defined(_PC_RETAIL)
#  include "Dx9/Dx9buffer.h" 
#endif	// defined(_PC_RETAIL)

#ifdef JADEFUSION
#if defined(ACTIVE_EDITORS)
#include "DATaControl/DATCPerforce.h"
#include "BIGfiles/BIGfat.h"
#endif
#endif

extern "C" ULONG				AI_C_Callback;

#ifdef Active_CloneListe		
		extern int renderState_Cloned;
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#ifdef _XBOX
extern u_int32 g_iNewLight;
#endif


#ifdef _XBOX

#define Gx8_M_ConvertColor(a) (a & 0xff00ff00) | ((a & 0xff) << 16) | ((a & 0xff0000) >> 16)

#endif



#ifndef KER_IS_NOT_A_CHEVRE
#define KER_IS_NOT_A_CHEVRE
#endif

/* Optimize an object by sorting Triangle */
#define GEO_OPT_CACHE_DEPTH 128
#define GEO_CACHE_SHIFT		2

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */
extern BOOL GEO_b_IsInFix;
extern "C" ULONG		LOA_ul_FileTypeSize[40];
extern "C" BOOL EDI_gb_ComputeMap;
extern BOOL LOA_gb_SpeedMode;

#ifdef GSP_PS2_BENCH
extern unsigned int NoADM;
ULONG GEO_MemRasterPs2 = 0;
#define GC_BENCHGRAPH
#endif

#ifdef GC_BENCHGRAPH
#ifdef JADEFUSION
extern __declspec(align(32)) MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#else
extern MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#endif
#ifdef PSX2_TARGET
#define GEOMEM_RASTER GEO_MemRasterPs2
#else
#define GEOMEM_RASTER GXI_Global_ACCESS(ulGEOMEM)
#endif
#endif

#ifdef _DSPL_ALLOC
unsigned long DoGEOAllocFromEnd = 0;
unsigned long LastAllocFromEnd = 0;
#endif

#if defined(PSX2_TARGET) || defined(ACTIVE_EDITORS)
#define  GEO_Cte_ObjectListSize  1500
GEO_tdst_Object*GEO_gast_ObjectList[GEO_Cte_ObjectListSize] ;
GEO_tdst_Object*GEO_gast_FixObjectList[GEO_Cte_ObjectListSize] ;
void *p_OriginalPtr_CN = NULL;
#endif

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */
extern void GEO_SKN_ComputeNormals(GEO_tdst_Object *_pst_Object , GEO_Vertex *pst_Point);

#ifdef _GAMECUBE /* Fix Display List Desallocation */
extern void GXI_DL_Free_FROM_OBJ(void *dl);
extern void GXI_CreateDisplayLists(GEO_tdst_Object *_pst_Object);
#endif 

#ifdef PSX2_TARGET
void GSP_UnCacheObject(GEO_tdst_Object *_pst_Object);
#endif
void 	GEO_FreeElementStripContent(GEO_tdst_ElementIndexedTriangles *_pst_Element);

#ifdef JADEFUSION
extern void GEO_ComputeFloatingSelection(GEO_tdst_Object *_pst_Obj);
#endif
/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
#ifdef MEM_OPT

#ifdef MEM_SPY 
void *_MEM_GEO_p_Alloc(MEM_tdst_MainStruct *_pMem,ULONG MemSize, char *_sFile, int _lLine)
#else // MEM_SPY 
void *_MEM_GEO_p_Alloc(MEM_tdst_MainStruct *_pMem,ULONG MemSize)
#endif //  MEM_SPY 
{
#ifdef _DSPL_ALLOC
	/* Allocate from End to Avoid memory holes */
	if (DoGEOAllocFromEnd) 
	{
		ULONG *pAllocated;
#ifdef MEM_SPY 
		pAllocated = _MEM_p_AllocFromEnd(_pMem,MemSize + 4,_sFile, _lLine);
#else // MEM_SPY 
		pAllocated = _MEM_p_AllocFromEnd(_pMem,MemSize + 4);
#endif // MEM_SPY 
		*pAllocated = DoGEOAllocFromEnd;
		LastAllocFromEnd = DoGEOAllocFromEnd = (ULONG)pAllocated;
		return (void *) (pAllocated + 1);
	}
#endif
	{
#ifdef GC_BENCHGRAPH
		void *ptrReturn;
		ULONG	ulPreviousSize;
		ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
#ifdef MEM_SPY 
		ptrReturn = _MEM_p_Alloc(_pMem,MemSize,_sFile, _lLine);
#else // MEM_SPY 
		ptrReturn = _MEM_p_Alloc(_pMem,MemSize);
#endif // MEM_SPY 
		GEOMEM_RASTER += MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated - ulPreviousSize;
		return ptrReturn;
#else // GC_BENCHGRAPH
#ifdef MEM_SPY 
		return _MEM_p_Alloc(_pMem,MemSize,_sFile, _lLine);
#else // MEM_SPY 
		return _MEM_p_Alloc(_pMem,MemSize);
#endif // MEM_SPY 
#endif // GC_BENCHGRAPH
	}
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef MEM_SPY 
void *_MEM_GEO_p_AllocAlign(MEM_tdst_MainStruct *_pMem,ULONG MemSize , ULONG Align , char *_sFile, int _lLine) 	
#else // MEM_SPY 
void *_MEM_GEO_p_AllocAlign(MEM_tdst_MainStruct *_pMem,ULONG MemSize , ULONG Align ) 	
#endif // MEM_SPY 
{
#ifdef _DSPL_ALLOC
	if (DoGEOAllocFromEnd)
#ifdef MEM_SPY 
	return _MEM_GEO_p_Alloc(_pMem,MemSize,_sFile, _lLine);
#else // MEM_SPY 
	return _MEM_GEO_p_Alloc(_pMem,MemSize);
#endif // MEM_SPY 
#endif // _DSPL_ALLOC


#ifdef GC_BENCHGRAPH
	{
	void *ptrReturn;
	ULONG	ulPreviousSize;
	ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
#ifdef MEM_SPY 
	ptrReturn = _MEM_p_AllocAlign(_pMem,MemSize,Align,_sFile, _lLine);
#else // MEM_SPY 
	ptrReturn = _MEM_p_AllocAlign(_pMem,MemSize,Align);
#endif // MEM_SPY 
	GEOMEM_RASTER += MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated - ulPreviousSize;
	return (void *) ptrReturn;
	}
#else // GC_BENCHGRAPH
#ifdef MEM_SPY 
	return _MEM_p_AllocAlign(_pMem,MemSize,Align,_sFile, _lLine);
#else // MEM_SPY 
	return _MEM_p_AllocAlign(_pMem,MemSize,Align);
#endif // MEM_SPY 
#endif // GC_BENCHGRAPH
}

#else // MEM_OPT

void *MEM_GEO_p_Alloc(ULONG MemSize ) 
{
#ifdef _DSPL_ALLOC
	/* Allocate from End to Avoid memory holes */
	if (DoGEOAllocFromEnd) 
	{
		ULONG *pAllocated;
		pAllocated = MEM_p_AllocFromEnd(MemSize + 4);
		*pAllocated = DoGEOAllocFromEnd;
		LastAllocFromEnd = DoGEOAllocFromEnd = (ULONG)pAllocated;
		return (void *) (pAllocated + 1);
	}
#endif
	{
#ifdef GC_BENCHGRAPH
		void *ptrReturn;
		ULONG	ulPreviousSize;
		ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
		ptrReturn = MEM_p_Alloc(MemSize);
		GEOMEM_RASTER += MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated - ulPreviousSize;
		return ptrReturn;
#else	
		return MEM_p_Alloc(MemSize);
#endif
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_GEO_p_AllocAlign(ULONG MemSize , ULONG Align ) 	
{
#ifdef _DSPL_ALLOC
	if (DoGEOAllocFromEnd) return MEM_GEO_p_Alloc(MemSize);
#endif
#ifdef GC_BENCHGRAPH
	{
	void *ptrReturn;
	ULONG	ulPreviousSize;
	ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	ptrReturn = MEM_p_AllocAlign(MemSize,Align);
	GEOMEM_RASTER += MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated - ulPreviousSize;
	return (void *) ptrReturn;
	}
#else
	return MEM_p_AllocAlign(MemSize,Align);
#endif	
}

#endif  // MEM_OPT


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_GEO_v_Free(void *ptr ) 			
{
#ifdef _DSPL_ALLOC
	if (DoGEOAllocFromEnd) return;
#endif
#ifdef GC_BENCHGRAPH
	{
	ULONG	ulPreviousSize;
	ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	MEM_Free(ptr);
	GEOMEM_RASTER -= ulPreviousSize - MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	}
#else
	MEM_Free(ptr);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_GEO_v_FreeAlign(void *ptr ) 		
{
#ifdef _DSPL_ALLOC
	if (DoGEOAllocFromEnd) return;
#endif
#ifdef GC_BENCHGRAPH
	{
	ULONG	ulPreviousSize;
	ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	MEM_FreeAlign(ptr);
	GEOMEM_RASTER -= ulPreviousSize - MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	}
#else
	MEM_FreeAlign(ptr);
#endif
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ ULONG FIFO_IsIn(ULONG Value, char *Cache)
{
	return Cache[Value >> GEO_CACHE_SHIFT];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void FIFO_ADD_Entry(ULONG *FIFO, ULONG Value, ULONG *FIFO_HEAD, char *Cache)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	LastCValue;
	/*~~~~~~~~~~~~~~~*/

	if(FIFO_IsIn(Value, Cache)) return; /* Nothing to change */
	LastCValue = FIFO[*FIFO_HEAD];
	FIFO[*FIFO_HEAD] = Value;
	(*FIFO_HEAD)++;
	(*FIFO_HEAD) &= GEO_OPT_CACHE_DEPTH - 1;
	Cache[Value >> GEO_CACHE_SHIFT] = 1;
	if(LastCValue != 0xffffffff) Cache[LastCValue >> GEO_CACHE_SHIFT] = 0;
}




/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_OptimzeCache(GEO_tdst_Object *_pst_Object)
{
	// NOT USED ANYMORE, all is striped
#ifdef ACTIVE_EDITORS
    if(!(EDI_gb_ComputeMap && (WOR_gi_CurrentConsole==1)) ) return;
#endif

    if(GEO_MRM_ul_IsMrmObject(_pst_Object)) return;

	GEO_MRM_ComputeUsedIndex(_pst_Object);

}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" void GEO_ResetUncacheObjectList(void)
{
#if defined(PSX2_TARGET) || defined(ACTIVE_EDITORS)
    L_memset(GEO_gast_ObjectList, 0, GEO_Cte_ObjectListSize*sizeof(GEO_tdst_Object*));
#endif    
}
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_RecordObject(GEO_tdst_Object* _pst_Object)
{
#if defined(PSX2_TARGET) || defined(ACTIVE_EDITORS)
    int i;
    GEO_tdst_Object**ppObj;
    static int DoInit=1;
    
    if(DoInit)
    {
        DoInit = 0;
        L_memset(GEO_gast_FixObjectList, 0, GEO_Cte_ObjectListSize*sizeof(GEO_tdst_Object*));
    }
    
	if(!_pst_Object->l_NbPoints) return;
    
    ppObj = GEO_b_IsInFix ? GEO_gast_FixObjectList : GEO_gast_ObjectList;
	for(i=0; i<GEO_Cte_ObjectListSize; i++, ppObj++)
    {
        if(! *ppObj) 
        {
            *ppObj = _pst_Object;
            return;
        }
    }

    ERR_X_Warning(0, "No more space for recording GEO object !!", NULL);
#endif
} 

void GEO_UnrecordObject(GEO_tdst_Object* _pst_Object)
{
#if defined(PSX2_TARGET) || defined(ACTIVE_EDITORS)
    int i;
    GEO_tdst_Object**ppObj;
    
    ppObj = GEO_gast_FixObjectList ;
    for(i=0; i<GEO_Cte_ObjectListSize; i++, ppObj++)
    {
        if(*ppObj==_pst_Object) 
        {
            *ppObj = NULL;
            return;
        }
    }
    ppObj = GEO_gast_ObjectList;
    for(i=0; i<GEO_Cte_ObjectListSize; i++, ppObj++)
    {
        if(*ppObj==_pst_Object) 
        {
            *ppObj = NULL;
            return;
        }
    }
#endif
} 


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DoUncache(void)
{
#ifdef PSX2_TARGET
    int i;
    GEO_tdst_Object**ppObj;
    
    
    for(i=0, ppObj = GEO_gast_FixObjectList; i<GEO_Cte_ObjectListSize; i++, ppObj++)
    {
        if(*ppObj) 
            GSP_UnCacheObject(*ppObj);
    }
    for(i=0, ppObj = GEO_gast_ObjectList; i<GEO_Cte_ObjectListSize; i++, ppObj++)
    {
        if(*ppObj) 
            GSP_UnCacheObject(*ppObj);
    }
    L_memset(GEO_gast_ObjectList, 0, GEO_Cte_ObjectListSize*sizeof(GEO_tdst_Object*));
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
void GEO_DoOptimzeCacheForPS2(void)
{
    int i;
    GEO_tdst_Object**ppObj;
    
    
    for(i=0, ppObj = GEO_gast_FixObjectList; i<GEO_Cte_ObjectListSize; i++, ppObj++)
    {
        if(*ppObj) 
            GEO_OptimzeCache(*ppObj);
        else
            break;
    }
    for(i=0, ppObj = GEO_gast_ObjectList; i<GEO_Cte_ObjectListSize; i++, ppObj++)
    {
        if(*ppObj) 
            GEO_OptimzeCache(*ppObj);
        else
            break;
    }
    L_memset(GEO_gast_ObjectList, 0, GEO_Cte_ObjectListSize*sizeof(GEO_tdst_Object*));
}
#endif


/*$4
 ***********************************************************************************************************************
    PSX2_TARGET
 ***********************************************************************************************************************
 */
#ifdef PSX2_TARGET

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GSP_Free_The_Packs(GSP_tdstTransfertOptimizer **PPacks)
{
	GSP_tdstTransfertOptimizer **PPacksLOCAL;
	PPacksLOCAL = PPacks;
	while (*PPacksLOCAL)
	{
		MEM_GEO_v_FreeAlign((*PPacksLOCAL)->SourceIndexes);
		MEM_GEO_v_Free((*PPacksLOCAL)->pus_ListOfUsedIndex_GSP_PACK);
		MEM_GEO_v_Free(*PPacksLOCAL);
		PPacksLOCAL++;
	}
	MEM_GEO_v_Free(PPacks);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SKN_OPTIMIZE_FOR_CACHE(GEO_tdst_Object *pst_Object)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_TransformSTRIPS(GEO_tdst_Object *_pst_Object)
{
	GEO_tdst_ElementIndexedTriangles	*pst_Element , *pst_ElementLast;
	ULONG UsedIndexAreComputed;
	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object ->l_NbElements;
	while (pst_Element < pst_ElementLast) (pst_Element++) -> pst_StripDataPS2 = NULL;
	if(GEO_MRM_ul_IsMrmObject(_pst_Object)) return;
	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object ->l_NbElements;

#ifdef _DSPL_ALLOC
	DoGEOAllocFromEnd = 0;
#endif
	/* 1 Compute used indexes ( for colors compute optimisations )*/
	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object->l_NbElements;
	UsedIndexAreComputed = 0;

	if (LastAllocFromEnd > 1)
		while (pst_Element < pst_ElementLast)
		{
#ifdef _DSPL_ALLOC
	    	if(pst_Element->pus_ListOfUsedIndex)
	    	{
	    	    unsigned short*pus;
	    	    pus = (unsigned short *) MEM_GEO_p_AllocAlign(pst_Element->ul_NumberOfUsedIndex * sizeof(unsigned short) + (LONG) 2 , 8);
	            L_memmove(pus, pst_Element->pus_ListOfUsedIndex, pst_Element->ul_NumberOfUsedIndex * sizeof(unsigned short) + (LONG) 2);
	    	    pst_Element->pus_ListOfUsedIndex=pus;
	    	}
#endif
			UsedIndexAreComputed += (pst_Element++)->ul_NumberOfUsedIndex;
		}
	
	if (!UsedIndexAreComputed) GEO_MRM_ComputeUsedIndex(_pst_Object);

	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object ->l_NbElements;
	while (pst_Element < pst_ElementLast)
	{
		ULONG NumberOfNodes;
		GEO_tdst_OneStrip			*pStripList,*pStripLast;
		USHORT 						*pDstAtoms,*pDstAtomsUV/*,*pStripsLenght*/;
		if (!pst_Element -> pst_StripData) {pst_Element++;continue;};
		NumberOfNodes = 0;
		
		pStripList = pst_Element -> pst_StripData -> pStripList;
		pStripLast = pStripList + pst_Element -> pst_StripData -> ulStripNumber;
		while (pStripList < pStripLast)
		{
			NumberOfNodes += pStripList->ulVertexNumber;
			pStripList ++;
		}
		{
			ULONG ulBufferLimits;
			ulBufferLimits = NumberOfNodes;
			while (ulBufferLimits >= (DP_MaxNumSTRP - 2))
			{
				ulBufferLimits -= (DP_MaxNumSTRP - 2);
				NumberOfNodes += 2;
			}
		}
		pst_Element -> pst_StripDataPS2 = (GEO_tdst_StripDataPS2 *) MEM_GEO_p_Alloc(sizeof(GEO_tdst_StripDataPS2));
		pst_Element -> pst_StripDataPS2 -> pVertexIndexes = (USHORT *) MEM_GEO_p_AllocAlign(2 * (NumberOfNodes + 8) , 16);
		pst_Element -> pst_StripDataPS2 -> pVertexUVIndexes = (USHORT *) MEM_GEO_p_AllocAlign(2 * (NumberOfNodes + 8) , 16);

		pDstAtoms = pst_Element -> pst_StripDataPS2 -> pVertexIndexes;
		pDstAtomsUV = pst_Element -> pst_StripDataPS2 -> pVertexUVIndexes;
		pStripList = pst_Element -> pst_StripData -> pStripList;
		pStripLast = pStripList + pst_Element -> pst_StripData -> ulStripNumber;
		{
			LONG ulBufferLimits;
			ulBufferLimits = 0;
			while (pStripList < pStripLast)
			{
				GEO_tdst_MinVertexData		*pFirstAtom,*pLasttAtom;
				u_int AtomCounter2l;
				pFirstAtom = pStripList->pMinVertexDataList;
				AtomCounter2l = 0;
				pLasttAtom = pFirstAtom + pStripList->ulVertexNumber;
				while (pFirstAtom < pLasttAtom)
				{
					*(pDstAtoms) = pFirstAtom->auw_Index;
					*(pDstAtomsUV) = pFirstAtom->auw_UV;
					if (AtomCounter2l < 2) *pDstAtoms |= 1<<15;
					if (ulBufferLimits == DP_MaxNumSTRP -1 ) 
					{
						*(pDstAtoms + 1) = *(pDstAtoms-1) | (1<<15);
						*(pDstAtomsUV + 1) = *(pDstAtomsUV-1);
						*(pDstAtoms + 2) = *(pDstAtoms) | (1<<15);
						*(pDstAtomsUV + 2) = *(pDstAtomsUV);
						pDstAtoms += 2;
						pDstAtomsUV += 2;
						ulBufferLimits = 1;
					}
					pFirstAtom++;
					AtomCounter2l++;
					pDstAtoms++;
					pDstAtomsUV++;
					ulBufferLimits++;
				}
				pStripList ++;
			}
		}
		pst_Element -> pst_StripDataPS2-> ulNumberOfAtoms = NumberOfNodes;
		
		NumberOfNodes = 8;
		while (NumberOfNodes--)
		{
			*(pDstAtoms) = *(pDstAtoms-1) | (1<<15);
			*(pDstAtomsUV) = *(pDstAtomsUV-1)  | (1<<15);
			pDstAtoms++;
			pDstAtomsUV++;

//			*(pStripsLenght++) = 0;
		}//*/

		pst_Element++;
	}
	/* 5 Free From end allocat */
#ifdef _DSPL_ALLOC
	/* 4 free the strips */	
	if (LastAllocFromEnd > 1)
	{
		DoGEOAllocFromEnd = LastAllocFromEnd;
		pst_Element = _pst_Object ->dst_Element;
		pst_ElementLast = pst_Element + _pst_Object->l_NbElements;
		while (pst_Element < pst_ElementLast)
		{
			if (pst_Element->pst_StripData)
			{
				pst_Element->dst_Triangle = NULL;
				pst_Element->l_NbTriangles = 0;
				pst_Element->pst_StripData = NULL;
			}
			pst_Element++;
		}
		while (LastAllocFromEnd != 1)
		{
			ULONG *pLocal;
			pLocal = (ULONG *)LastAllocFromEnd;
			LastAllocFromEnd = *pLocal;
			MEM_FreeFromEnd((void *)pLocal);
		}
	}
	LastAllocFromEnd = 0;
	DoGEOAllocFromEnd = 0;
#endif	
	
	
}


void GSP_AllocABackCache(struct GEO_tdst_ElementIndexedTriangles_ 	*p_ElementUser);
void GSP_FreeABackCache(struct GSP_GEOBackCache_ *p_BackCache);
#define PSX2_UnCachePtr(a)    *(u_int *)&a |= 0x30000000;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GSP_CompressNormals(MATH_tdst_Vector *pSRC , ULONG *pDST , ULONG ulNumber)
{
#if defined(GSP_PS2) || (defined(_GAMECUBE) && !defined(_FINAL_))
extern u_int NoSKN;
 if (NoSKN) return;
#endif 
	while (ulNumber--)
	{
		ULONG nX,nY,nZ;
		nX = (ULONG)((fMin(1.0f,fMax(-1.0f , pSRC->x))) * 120.0f + 128.0f);
		nY = (ULONG)((fMin(1.0f,fMax(-1.0f , pSRC->y))) * 120.0f + 128.0f);
		nZ = (ULONG)((fMin(1.0f,fMax(-1.0f , pSRC->z))) * 120.0f + 128.0f);
		*pDST = (nX << 0) | (nY << 8) | (nZ << 16);
		pDST++;
		pSRC++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GSP_UnCacheObject(GEO_tdst_Object *_pst_Object)
{
	GEO_tdst_ElementIndexedTriangles	*pst_Element , *pst_ElementLast;
	FlushCache(0);
	
	if(!_pst_Object->l_NbPoints) 
	{
		_pst_Object = _pst_Object;
		return;
	}
    if(!_pst_Object->p_CompressedNormals)
    {
	    _pst_Object->p_CompressedNormals = MEM_GEO_p_VMAllocAlign(sizeof(ULONG) * (_pst_Object->l_NbPoints + 4),16);
	    L_memset(_pst_Object->p_CompressedNormals , 0 , sizeof(ULONG) * (_pst_Object->l_NbPoints + 4));
    }
	GEO_UseNormals(_pst_Object); // OK
    GEO_ComputeNormals(_pst_Object); // this will Recompute
	if (_pst_Object->dst_PointNormal)
	{
		u32 NormalCounter,*pColor,*pNormal;
	    GSP_CompressNormals(_pst_Object->dst_PointNormal , _pst_Object->p_CompressedNormals , _pst_Object->l_NbPoints); // this will Recompute
	    pColor = _pst_Object->dul_PointColors;
	    if (pColor )
	    {
	    	pColor++;
	    	pNormal = _pst_Object->p_CompressedNormals;
		    NormalCounter= _pst_Object->l_NbPoints;
		    while (NormalCounter--)
		    {
		    	*pNormal	&= 0xffffff;
		    	*pNormal	|= *pColor & 0xff000000;
		    	pNormal++;
		    	pColor ++;
		    }
	    }
	    
		MEM_GEO_v_Free(_pst_Object->dst_PointNormal);
		_pst_Object->dst_PointNormal = NULL;
		
	}//*/
#ifdef PSX2_TARGET	
	if (GEO_SKN_IsSkinned(_pst_Object))
	{
		_pst_Object -> p_SKN_Objectponderation -> FUCKING_ANTIBUG_OF_THE_NORMALS = 1;
	}//*/
#endif	
	
	if(GEO_MRM_ul_IsMrmObject(_pst_Object)) 
	{
		_pst_Object = _pst_Object;
		return;
	}
	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object ->l_NbElements;
	while (pst_Element < pst_ElementLast)
	{
		GSP_AllocABackCache(pst_Element);
		pst_Element++;
	}
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Optimize_PS2(GEO_tdst_Object *_pst_Object)
{
	_pst_Object->l_NbSpritesElements = 0;
	GEO_OptimzeCache(_pst_Object);
	GEO_TransformSTRIPS(_pst_Object);
	GEO_SKN_OPTIMIZE_FOR_CACHE(_pst_Object);
	GEO_RecordObject(_pst_Object);
}
#endif /* PSX2_TARGET */


/*$4
 ***********************************************************************************************************************
    _GAMECUBE
 ***********************************************************************************************************************
 */

#ifdef _GAMECUBE

BOOL GXI_bMaterialHasFur(GRO_tdst_Struct *pst_Material,LONG _lMaterialID)
{
	if(pst_Material->i->ul_Type == GRO_MaterialMulti)
	{
		MAT_tdst_Multi *pst_Mat = (MAT_tdst_Multi *) pst_Material;
		int i;
		for (i=0; i<pst_Mat->l_NumberOfSubMaterials; i++)
		{
			if (GXI_bMaterialHasFur(pst_Mat->dpst_SubMaterial[_lMaterialID%pst_Mat->l_NumberOfSubMaterials],_lMaterialID))
				return TRUE;
		}
	}
	else if (pst_Material->i->ul_Type == GRO_MaterialSingle) 
	{
		MAT_tdst_Single *pst_Mat = (MAT_tdst_Single *) pst_Material;
		if (pst_Mat->ul_ValidateMask & MAT_ValidateMask_Fur) // Useful ???
			return TRUE;
	} 
	else if (pst_Material->i->ul_Type == GRO_MaterialMultiTexture) 
	{
		MAT_tdst_MultiTexture *pst_Mat = (MAT_tdst_MultiTexture *) pst_Material;
		if (pst_Mat->ul_ValidateMask & MAT_ValidateMask_Fur)
			return TRUE;
	} 
	return FALSE;
}

BOOL GXI_bHasFur(OBJ_tdst_GameObject *_pGAO,LONG _lMaterialID)
{
	// Does if have the fur modifier ?
	BOOL bHasFurModifier = FALSE;
	if (_pGAO->pst_Extended)
	{
		MDF_tdst_Modifier *pModifier = _pGAO->pst_Extended->pst_Modifiers;
		while (pModifier)
		{
			if (pModifier->i->ul_Type == MDF_C_Modifier_FUR)
				bHasFurModifier = TRUE;
			pModifier = pModifier->pst_Next;
		}
	} 
	
	if (!bHasFurModifier)
		return FALSE;
	
	return GXI_bMaterialHasFur(_pGAO->pst_Base->pst_Visu->pst_Material,_lMaterialID);
}

extern void GXI_ComputeDisplayLists(GEO_tdst_Object *_pst_Object);
extern WOR_tdst_World	*WOR_gpst_CurrentWorld;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Optimize_GAMECUBE(GEO_tdst_Object *_pst_Object)
{
	GEO_tdst_ElementIndexedTriangles *pst_Element,*pst_ElementLast;
	OBJ_tdst_GameObject *pGAO = NULL;
	ULONG UsedIndexAreComputed;
#ifdef 	_DSPL_ALLOC
	ULONG ulSaveDoGEOAllocFromEnd = DoGEOAllocFromEnd;
	/* 0 Set to Real Allocation */
	DoGEOAllocFromEnd = 0;
#endif // 	_DSPL_ALLOC

	/* 1 Compute used indexes ( for colors compute optimisations )*/
	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object->l_NbElements;
	UsedIndexAreComputed = 0;
	while (pst_Element < pst_ElementLast)
	{
    	if(pst_Element->pus_ListOfUsedIndex)
    	{
    	    unsigned short*pus;
    	    pus = (unsigned short *) MEM_GEO_p_VMAllocAlign(pst_Element->ul_NumberOfUsedIndex * sizeof(unsigned short) + (LONG) 2 , 8);
            L_memmove(pus, pst_Element->pus_ListOfUsedIndex, pst_Element->ul_NumberOfUsedIndex * sizeof(unsigned short) + (LONG) 2);
			MEM_GEO_v_FreeAlign(pst_Element->pus_ListOfUsedIndex);
    	    pst_Element->pus_ListOfUsedIndex=pus;
    	}
		UsedIndexAreComputed += (pst_Element++)->ul_NumberOfUsedIndex;
	}
	
	if (!UsedIndexAreComputed) GEO_MRM_ComputeUsedIndex(_pst_Object);

	/* 2 Create display lists from strips */	
	GXI_ComputeDisplayLists(_pst_Object);


#ifndef	NO_GC_FUR
	// Search game object that contains this geometry, to see if it has fur
	{
	    TAB_tdst_PFelem     *pst_Elem, *pst_LastElem;
        
        pst_Elem = WOR_gpst_CurrentWorld->st_AllWorldObjects.p_Table;
        pst_LastElem = WOR_gpst_CurrentWorld->st_AllWorldObjects.p_NextElem;
        for(; pst_Elem < pst_LastElem; pst_Elem++)
        {
            OBJ_tdst_GameObject *GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
            if (TAB_b_IsAHole(GO)) continue;

            if (GO && GO->pst_Base && GO->pst_Base->pst_Visu && 
                ((GEO_tdst_Object *)GO->pst_Base->pst_Visu->pst_Object == _pst_Object))
            {
            	pGAO = GO;
        		break;
            }
        }
	}
	ERR_X_Assert(pGAO);
#endif //	NO_GC_FUR

	
#ifdef 	_DSPL_ALLOC
	/* 3 Restore false Allocat */
	DoGEOAllocFromEnd = ulSaveDoGEOAllocFromEnd;
#endif // 	_DSPL_ALLOC
	
	/* 4 free the strips and triangles (except if object has fur) */	
	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object->l_NbElements;
	while (pst_Element < pst_ElementLast)
	{
		if (pst_Element->dst_Triangle && (pst_Element->dl || pst_Element->pst_StripData))
		{
			MEM_GEO_v_FreeAlign(pst_Element->dst_Triangle);
			pst_Element->dst_Triangle = NULL;
			pst_Element->l_NbTriangles = 0;
		}
		if (pst_Element->pst_StripData && pst_Element->dl && (!pGAO || !GXI_bHasFur(pGAO,pst_Element->l_MaterialId)))
		{
			GEO_FreeElementStripContent(pst_Element);
		}
		pst_Element++;
	}
	/* 5 Free From end allocat */
#ifdef _DSPL_ALLOC
	if (DoGEOAllocFromEnd > 1)
	{
		while (DoGEOAllocFromEnd != 1)
		{
			ULONG *pLocal;
			pLocal = (ULONG *)DoGEOAllocFromEnd;
			DoGEOAllocFromEnd = *pLocal;
			MEM_FreeFromEnd((void *)pLocal);
		}
	}
	DoGEOAllocFromEnd = 0;
#endif // _DSPL_ALLOC
}
#endif /* _GAMECUBE */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define MICRO_FREEZE_OPTMIS_STACK_SIZE 256
GEO_tdst_ElementIndexedTriangles    *MICRO_FREEZE_OPTMIS_STACK[MICRO_FREEZE_OPTMIS_STACK_SIZE];
ULONG MICRO_FREEZE_OPTMIS_STACK_NUM = 0;
extern "C" ULONG MICRO_FREEZE_OPTMIS = 0;
void GEO_I_Need_The_Triangles_End_Element(GEO_tdst_ElementIndexedTriangles    *pst_Element , ULONG FromEnd);
extern "C" void GEO_I_Need_The_Triangles_FlushAll()
{
	MICRO_FREEZE_OPTMIS = 0;
	while (MICRO_FREEZE_OPTMIS_STACK_NUM --)
	{
		GEO_I_Need_The_Triangles_End_Element(MICRO_FREEZE_OPTMIS_STACK[MICRO_FREEZE_OPTMIS_STACK_NUM], 1);
	}
	MICRO_FREEZE_OPTMIS_STACK_NUM = 0;
}
void GEO_I_Need_The_Triangles_Begin_Element(GEO_tdst_ElementIndexedTriangles    *pst_Element , ULONG FromEnd)
{
#ifdef _GAMECUBE_DSPLS
    ULONG                               ul_NbTriangle ;
	u8		                            *Stream, *StreamUV,*StreamLast ;
	u32 		                        Pitch , Mode8;
	u32                                 Mode4, Mode2;
	BOOL                                bflip;
	u16     	p1, p2, p3;
	u16         UV0, UV1, UV2;
	GEO_tdst_IndexedTriangle* pTriangle;
	ULONG*p;

    {
        if(pst_Element->dst_Triangle) return;
        if(!pst_Element->dl) return;
    	ul_NbTriangle = 0;    		
    	Stream = pst_Element->dl;
    	StreamLast = Stream + (pst_Element->dl_size & 0x0fffffff);
    	Pitch = 8;
    	Mode8=Mode4=Mode2 = 0;
    	if (pst_Element->dl_size & 0x80000000) {Pitch -= 2;Mode8 = 1;}
    	if (pst_Element->dl_size & 0x40000000) {Pitch -= 1;Mode4=1;}
    	if (pst_Element->dl_size & 0x20000000) {Pitch -= 1;Mode2=1;}

		while (Stream < StreamLast)
		{
		    u16 lNbVertex;
			u8	*StreamStripLast;
			
			Stream++; // Flags
			lNbVertex = *(u16 *)Stream;	// Number Of Vertexes
			Stream += 2; 				// Number Of Vertexes
		    
		    if(lNbVertex)
    			StreamStripLast = Stream + lNbVertex * Pitch;
    	    else
    	        StreamStripLast = StreamLast;
				
		    Stream += Pitch;
		    Stream += Pitch;
			
			while (Stream < StreamStripLast) // Parse VertexIndexes
			{
			    ul_NbTriangle++;
				Stream += Pitch;
			}
			while ((*Stream == 0) && (Stream < StreamLast)) Stream++;
		}
			
        if(!ul_NbTriangle) return;
        
        
	    p = MEM_p_VMAlloc((ul_NbTriangle * sizeof(GEO_tdst_IndexedTriangle))+sizeof(ULONG));
	    *p = 0xbade0ffe;
	    p++;
			    
	    pst_Element->dst_Triangle = (GEO_tdst_IndexedTriangle*)p;
	    pst_Element->l_NbTriangles =  ul_NbTriangle;
	    L_memset(pst_Element->dst_Triangle, 0, ul_NbTriangle * sizeof(GEO_tdst_IndexedTriangle));
	    pTriangle = pst_Element->dst_Triangle;
	    
	    
	    Stream = pst_Element->dl;
	    StreamLast = Stream + (pst_Element->dl_size & 0x0fffffff);
	    Pitch = 8;
	    Mode8 = Mode4 = Mode2 = 0;
    	if (pst_Element->dl_size & 0x80000000) {Pitch -= 2;Mode8 = 1;}
    	if (pst_Element->dl_size & 0x40000000) {Pitch -= 1;Mode4=1;}
    	if (pst_Element->dl_size & 0x20000000) {Pitch -= 1;Mode2=1;}
		ul_NbTriangle = 0;
		while (Stream < StreamLast)
		{
		    u16 lNbVertex;
			u8	*StreamStripLast;
			
			Stream++; // Flags
			lNbVertex = *(u16 *)Stream;	// Number Of Vertexes
			Stream += 2; 				// Number Of Vertexes
		    bflip=FALSE;
		    
        	StreamUV = Stream + Pitch - 2 + Mode2;


		    if(lNbVertex)
    			StreamStripLast = Stream + lNbVertex * Pitch;
    	    else
    	        StreamStripLast = StreamLast;

			if (Mode8)
			{
			    p1 = (u16)*(u8 *)Stream;
			    p2 = (u16)*(u8 *)(Stream + Pitch);
			} 
			else
			{
			    p1 = (u16)*(u16 *)Stream;
			    p2 = (u16)*(u16 *)(Stream + Pitch);
			}
		    
			if(Mode2)
			{
			    UV0 = (u16)*(u8 *)StreamUV;
			    UV1 = (u16)*(u8 *)(StreamUV + Pitch);
			}
			else
			{
			    UV0 = (u16)*(u16 *)StreamUV;
			    UV1 = (u16)*(u16 *)(StreamUV + Pitch);
			}
			
            Stream += Pitch <<1;
            StreamUV += Pitch<<1;
            
			while (Stream < StreamStripLast) // Parse VertexIndexes
			{
				if (Mode8)
				{
				    p3 = (u16)*(u8 *)Stream;
				} 
				else
				{
				    p3 = *(u16*)Stream;
				}
			
				if(Mode2)
				{
				    UV2 = (u16)*(u8 *)StreamUV;
				}
				else
				{
				    UV2 = *(u16 *)StreamUV;
				}
				
			    if(bflip) 
			    {
					pTriangle->auw_Index[0] = p2;
					pTriangle->auw_Index[1] = p1;
					pTriangle->auw_UV[0] = UV1;
					pTriangle->auw_UV[1] = UV0;
			    }
			    else
			    {
					pTriangle->auw_Index[0] = p1;
					pTriangle->auw_Index[1] = p2;
					pTriangle->auw_UV[0] = UV0;
					pTriangle->auw_UV[1] = UV1;
			    }
				pTriangle->auw_Index[2] = p3;
				pTriangle->auw_UV[2] = UV2;

				p1 = p2;
				p2 = p3;
				bflip =!bflip;
								
				pTriangle++;
				ul_NbTriangle++;
                Stream += Pitch;
                StreamUV += Pitch;
				
			}    									
		while ((*Stream == 0) && (Stream < StreamLast)) Stream++;
		}
	}
#endif 
#ifdef _PSX2_DSPLS
    ULONG                               ul_NbTriangle ;
	SHORT		                            *Stream, *StreamLast ;
	SHORT  								p1, p2, p3,bflip;
	GEO_tdst_IndexedTriangle* pTriangle;
	ULONG *p , ulBufferLimits;
	u_long64 Optimisor;

    {
        if(pst_Element->dst_Triangle) return;
        if(!pst_Element->pst_StripDataPS2) return;
    	/* Compute number of vertices */
    	Stream = pst_Element->pst_StripDataPS2->pVertexIndexes;
    	StreamLast =Stream + pst_Element->pst_StripDataPS2->ulNumberOfAtoms;
    	ul_NbTriangle = 0;//pst_Element->pst_StripDataPS2->ulNumberOfAtoms;    		
		while (Stream < StreamLast)
		{
			asm __volatile__ ("	pref    0, 128(Stream)");
			ul_NbTriangle += ((*(u16 *)Stream) >> 15) ^ 1 ;
			Stream++;
		}
			
        if(!ul_NbTriangle) return;
        if (FromEnd)
        {
        	u32 K;
		    if (MICRO_FREEZE_OPTMIS)
		    { 
		    	if (MICRO_FREEZE_OPTMIS_STACK_NUM == MICRO_FREEZE_OPTMIS_STACK_SIZE)
		    		GEO_I_Need_The_Triangles_FlushAll();
		    	MICRO_FREEZE_OPTMIS = 1;
		    }
        	
		    K = (u32)MEM_p_AllocFromEnd(((ul_NbTriangle + 1) * sizeof(GEO_tdst_IndexedTriangle)) + 128);
		    p = (ULONG *)((K + 128) & ~63);
		    *(p - 1) = K;
		    if (MICRO_FREEZE_OPTMIS)
		    { 
		    	MICRO_FREEZE_OPTMIS_STACK[MICRO_FREEZE_OPTMIS_STACK_NUM++] = pst_Element;
		    }
        }
        else
		    p = MEM_p_AllocAlign(((ul_NbTriangle + 1) * sizeof(GEO_tdst_IndexedTriangle)) , 16);
	    pst_Element->dst_Triangle = (GEO_tdst_IndexedTriangle*)p;
	    pst_Element->l_NbTriangles =  ul_NbTriangle;
	    L_memset(pst_Element->dst_Triangle, 0, ul_NbTriangle * sizeof(GEO_tdst_IndexedTriangle));
	    pTriangle = pst_Element->dst_Triangle;
    	Stream = pst_Element->pst_StripDataPS2->pVertexIndexes;
		ulBufferLimits = 0;
		bflip = 0;
		Optimisor = 0L;
/* Working C */			
//#if 0
#ifdef JADEFUSION
		while (Stream < StreamLast)
		{
			if (ulBufferLimits == DP_MaxNumSTRP) 
			{
				Stream += 2;
				ulBufferLimits = 2;
			}
			ulBufferLimits++;
		    p3 = *(Stream++);
		    if (!(p3 & (1 << 15))) 
		    {
		    	if (bflip)
		    	{
					pTriangle->auw_Index[0] = p2;
					pTriangle->auw_Index[1] = p1;
				} else
		    	{
					pTriangle->auw_Index[0] = p1;
					pTriangle->auw_Index[1] = p2;
				} 
				(pTriangle++)->auw_Index[2] = p3 & ~(1 << 15);
				bflip =!bflip;
		    } else bflip = 0;
			p1 = p2;
			p2 = p3 & ~(1 << 15);
		}
/* Optimised ASM */
#else
		while (Stream < StreamLast)
		{
			asm __volatile__ ("	pref    0, 128(Stream)");
			asm __volatile__ ("	pref    0, 128(pTriangle)");
			if (ulBufferLimits == DP_MaxNumSTRP) 
			{
				Stream += 2;
				ulBufferLimits = 2;
			}
			ulBufferLimits++;

		    Optimisor += *((u16 *)Stream) & 0x7fff;
		    if(!(*Stream & (1 << 15))) 
		    {
		    	if (bflip)
		    	{
		    		u_long64 Optimisor2;
			    	asm __volatile__ ("	PEXCH 	Optimisor2,Optimisor ");
		    		*(u_long64 *)&pTriangle->auw_Index[0] = Optimisor2; 
				} else
		    		*(u_long64 *)&pTriangle->auw_Index[0] = Optimisor; 
				
/*				pTriangle->auw_UV[0] = 0;
				pTriangle->auw_UV[1] = 1;
				pTriangle->auw_UV[2] = 2;*/
				pTriangle++;
				bflip ^= 1;
		    } else bflip = 1;
		    Stream++;
			Optimisor <<= 16;
		}
#endif			
	}
#endif
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
void GEO_I_Need_The_Triangles_End_Element(GEO_tdst_ElementIndexedTriangles    *pst_Element , ULONG FromEnd)
{
	if (MICRO_FREEZE_OPTMIS && FromEnd) return;
#if defined(_DSPLS)
    {
#ifdef _GAMECUBE
        if(pst_Element->dl && pst_Element->dst_Triangle)
#else
		if(pst_Element->pst_StripDataPS2 && pst_Element->dst_Triangle)
#endif        
        {
            ULONG * p;
            p = (ULONG*)pst_Element->dst_Triangle;
#ifdef _GAMECUBE
            p--;
            if(*p == 0xbade0ffe)
            {
				*p =0;
				if (FromEnd)
					MEM_FreeFromEnd(p);
				else
                	MEM_Free(p);
                pst_Element->dst_Triangle = NULL;
                pst_Element->l_NbTriangles = 0;
            }
#else
			if (FromEnd)
#ifdef _PSX2_DSPLS			
				MEM_FreeFromEnd((u32 *)*(p - 1));
#else
				MEM_FreeFromEnd(p);
#endif				
			else
            	MEM_Free(p);
            pst_Element->dst_Triangle = NULL;
            pst_Element->l_NbTriangles = 0;
#endif
        }
        
    }    
#endif
}
void GEO_I_Need_The_Triangles_End(GEO_tdst_Object *_pst_Object , ULONG FromEnd)
{
#if defined(_DSPLS)
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;

    pst_Element = _pst_Object->dst_Element;
    pst_LastElement = pst_Element + _pst_Object->l_NbElements;
    for(; pst_Element < pst_LastElement; pst_Element++) GEO_I_Need_The_Triangles_End_Element(pst_Element,FromEnd);
#endif
}    
void GEO_I_Need_The_Triangles_Begin(GEO_tdst_Object *_pst_Object , ULONG FromEnd)
{
#if defined(_DSPLS)
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;

    pst_Element = _pst_Object->dst_Element;
    pst_LastElement = pst_Element + _pst_Object->l_NbElements;
    for(; pst_Element < pst_LastElement; pst_Element++) GEO_I_Need_The_Triangles_Begin_Element(pst_Element,FromEnd);
#endif
}    

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SpriteTest(GEO_tdst_Object *_pst_Object)
{
		ULONG Counter;
		_pst_Object->l_NbSpritesElements = 0;
		return;
		_pst_Object->l_NbSpritesElements = 1;
		_pst_Object->dst_SpritesElements = (GEO_tdst_ElementIndexedSprite *)MEM_GEO_p_Alloc(sizeof(GEO_tdst_ElementIndexedSprite));
		_pst_Object->dst_SpritesElements[0].l_NbSprites = _pst_Object->l_NbPoints;
		_pst_Object->dst_SpritesElements[0].l_MaterialId = 0;
		_pst_Object->dst_SpritesElements[0].dst_Sprite = (GEO_tdst_IndexedSprite*)MEM_GEO_p_Alloc(sizeof(GEO_tdst_IndexedSprite) * _pst_Object->dst_SpritesElements[0].l_NbSprites);
		_pst_Object->dst_SpritesElements[0].fGlobalSize = 0.15f;
		_pst_Object->dst_SpritesElements[0].fGlobalRatio = 2.f;
		Counter = _pst_Object->dst_SpritesElements[0].l_NbSprites;
		while (Counter--)
		{
			*(float *)&_pst_Object->dst_SpritesElements[0].dst_Sprite[Counter] = (float)(Counter & 255) / 512.0f + 0.5f;
			_pst_Object->dst_SpritesElements[0].dst_Sprite[Counter].auw_Index = (unsigned short)Counter;
		}
}//*/

/*
 =======================================================================================================================
    Alloc content of an element
 =======================================================================================================================
 */

void GEO_FreeSpriteElementContent(GEO_tdst_ElementIndexedSprite *_pst_Element)
{
	MEM_GEO_v_Free(_pst_Element->dst_Sprite);
}

void GEO_FreeElementStripContent(GEO_tdst_ElementIndexedTriangles *_pst_Element)
{
	if(_pst_Element->pst_StripData)
	{
		if(_pst_Element->pst_StripData->pStripList)
		{
#ifndef ACTIVE_EDITORS
			MEM_GEO_v_Free(_pst_Element->pst_StripData->pStripDatas);
#else
			{ 
				ULONG i;
				for(i = 0; i < _pst_Element->pst_StripData->ulStripNumber; i++)
				{
					if(_pst_Element->pst_StripData->pStripList[i].pMinVertexDataList)
					{
						MEM_GEO_v_Free(_pst_Element->pst_StripData->pStripList[i].pMinVertexDataList);
						_pst_Element->pst_StripData->pStripList[i].pMinVertexDataList = NULL;
					}
				}
			}
#endif
			MEM_GEO_v_Free(_pst_Element->pst_StripData->pStripList);
			_pst_Element->pst_StripData->pStripList = NULL;
		}

#if defined(_PC_RETAIL) || defined(_XBOX) || defined(_XENON_RENDER)
		MEM_GEO_v_Free(_pst_Element->pst_StripData->pDrawStripList);
		_pst_Element->pst_StripData->pDrawStripList = NULL;
#endif	// defined(_PC_RETAIL) || defined(_XBOX)

		_pst_Element->pst_StripData->ulFlag = 0;
		_pst_Element->pst_StripData->ulStripNumber = 0;
		_pst_Element->pst_StripData->ulAveLength = 0;
		_pst_Element->pst_StripData->ulMaxLength = 0;
		_pst_Element->pst_StripData->ulMinLength = 0;

		MEM_GEO_v_Free(_pst_Element->pst_StripData);
		_pst_Element->pst_StripData = NULL;
	}
#ifdef PSX2_TARGET
	if(_pst_Element->pst_StripDataPS2)
	{
		if (_pst_Element -> pst_StripDataPS2 -> pVertexIndexes)   MEM_GEO_v_FreeAlign(_pst_Element -> pst_StripDataPS2 -> pVertexIndexes);
		_pst_Element -> pst_StripDataPS2 -> pVertexIndexes = NULL;
		if (_pst_Element -> pst_StripDataPS2 -> pVertexUVIndexes) MEM_GEO_v_FreeAlign(_pst_Element -> pst_StripDataPS2 -> pVertexUVIndexes);
		_pst_Element -> pst_StripDataPS2 -> pVertexUVIndexes = NULL;
		MEM_GEO_v_Free(_pst_Element -> pst_StripDataPS2);
		_pst_Element -> pst_StripDataPS2 = NULL;
	}
#endif		
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_FreeElementContent(GEO_tdst_ElementIndexedTriangles *_pst_Element)
{
	if(_pst_Element == NULL) return;

	if
	(
		_pst_Element->l_NbTriangles
	||	(
			_pst_Element->p_MrmElementAdditionalInfo && _pst_Element->p_MrmElementAdditionalInfo->
				ul_RealNumberOfTriangle
		)
	)
	{
		MEM_GEO_v_FreeAlign(_pst_Element->dst_Triangle);
		_pst_Element->l_NbTriangles = 0;
	}

	if(_pst_Element->p_MrmElementAdditionalInfo)
	{
		MEM_GEO_v_Free(_pst_Element->p_MrmElementAdditionalInfo);
		_pst_Element->p_MrmElementAdditionalInfo = NULL;
	}

	if(_pst_Element->ul_NumberOfUsedIndex)
	{
		MEM_GEO_v_FreeAlign(_pst_Element->pus_ListOfUsedIndex);
		_pst_Element->pus_ListOfUsedIndex = NULL;
	}

	GEO_FreeElementStripContent(_pst_Element);
	
#ifdef PSX2_TARGET	
	GSP_FreeABackCache(_pst_Element->p_ElementCache);
#endif	

#ifdef _GAMECUBE
	if ((_pst_Element->dl) && (_pst_Element->dl_size & 0x10000000)) //MEM_FreeAlign(_pst_Element->dl);
		GXI_DL_Free_FROM_OBJ(_pst_Element->dl);
	_pst_Element->dl = NULL;
	_pst_Element->dl_size = 0;
#endif
	
}

/*
 =======================================================================================================================
    Free content of an element
 =======================================================================================================================
 */
void GEO_AllocElementContent(GEO_tdst_ElementIndexedTriangles *_pst_Element)
{
	if(_pst_Element == NULL) return;

	if(_pst_Element->l_NbTriangles)
	{
		LOA_ul_FileTypeSize[24] += 16 * (_pst_Element->l_NbTriangles + 1);
		_pst_Element->dst_Triangle = (GEO_tdst_IndexedTriangle *) MEM_GEO_p_VMAllocAlign(sizeof(GEO_tdst_IndexedTriangle) * (_pst_Element->l_NbTriangles + 1),16);
#ifdef ACTIVE_EDITORS
		L_memset(_pst_Element->dst_Triangle, 0, sizeof(GEO_tdst_IndexedTriangle) * (_pst_Element->l_NbTriangles + 1));
#endif
		_pst_Element->dst_Triangle[_pst_Element->l_NbTriangles].ul_SmoothingGroup = 0xffffffff;
	}

	if(_pst_Element->p_MrmElementAdditionalInfo)
	{
		_pst_Element->p_MrmElementAdditionalInfo = (GEO_tdst_MRM_Element *) MEM_GEO_p_Alloc(sizeof(GEO_tdst_MRM_Element));
	}

	_pst_Element->pus_ListOfUsedIndex = NULL;
	if(_pst_Element->ul_NumberOfUsedIndex)
	{
		LOA_ul_FileTypeSize[25] += _pst_Element->ul_NumberOfUsedIndex * sizeof(unsigned short) + (LONG) 2;
		_pst_Element->pus_ListOfUsedIndex = (unsigned short *) MEM_GEO_p_VMAllocAlign(_pst_Element->ul_NumberOfUsedIndex * sizeof(unsigned short) + (LONG) 2 , 8);
	}

	_pst_Element->pst_StripData = NULL;
#ifdef PSX2_TARGET
	_pst_Element->pst_StripDataPS2 = NULL;
	_pst_Element->p_ElementCache = NULL;
#endif

#ifdef _GAMECUBE
	_pst_Element->dl = NULL;
	_pst_Element->dl_size = 0;
	
#endif

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void GEO_UseNormals(GEO_tdst_Object *_pst_Object)
{
#ifdef ACTIVE_EDITORS
	return;
#endif
	if(_pst_Object->dst_PointNormal) return;
#ifdef _GAMECUBE
	_pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_GEO_p_AllocAlign(sizeof(MATH_tdst_Vector) * _pst_Object->l_NbPoints,32);
#else	
#if defined(_XBOX) || defined(_XENON_RENDER)
	_pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_GEO_p_AllocAlign((sizeof(MATH_tdst_Vector) +4) * _pst_Object->l_NbPoints,16);
#else
	_pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_GEO_p_AllocAlign(sizeof(MATH_tdst_Vector) * _pst_Object->l_NbPoints,16);
#endif
#endif
	GEO_ComputeNormals(_pst_Object); // Editors Version
	
}


/*
 =======================================================================================================================
    Alloc content of an object
 =======================================================================================================================
 */
void GEO_AllocContent(GEO_tdst_Object *_pst_Object)
{
	/*~~~~~~~~~~~~~~*/
	LONG	l_Size;
	/*~~~~~~~~~~~~~~*/

	if(_pst_Object == NULL) return;

	if(_pst_Object->l_NbPoints)
	{
		LOA_ul_FileTypeSize[20] += sizeof(GEO_Vertex) * _pst_Object->l_NbPoints;
		LOA_ul_FileTypeSize[21] += sizeof(MATH_tdst_Vector) * _pst_Object->l_NbPoints;
#ifdef _GAMECUBE
		_pst_Object->dst_Point = (GEO_Vertex *) MEM_GEO_p_AllocAlign(sizeof(GEO_Vertex) * _pst_Object->l_NbPoints, 32);
		if(GEO_b_IsInFix)
			_pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_GEO_p_AllocAlign(sizeof(MATH_tdst_Vector) * _pst_Object->l_NbPoints,32);
		else
			_pst_Object->dst_PointNormal = NULL;
#else	

		_pst_Object->dst_Point = (GEO_Vertex *) MEM_GEO_p_AllocAlign(sizeof(GEO_Vertex) * _pst_Object->l_NbPoints, 16);
#ifdef ACTIVE_EDITORS
		_pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_GEO_p_AllocAlign(sizeof(MATH_tdst_Vector) * _pst_Object->l_NbPoints,16);
#else
#if defined(_XBOX) || defined(_M_X86) || defined(_XENON_RENDER)
    #ifdef _XENON_RENDER
        _pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_GEO_p_AllocAlign((sizeof(MATH_tdst_Vector)+4) * _pst_Object->l_NbPoints,16);
    #else
		if(GEO_b_IsInFix)		//allocate 4bytes more for aligned normals vector
			_pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_GEO_p_AllocAlign((sizeof(MATH_tdst_Vector)+4) * _pst_Object->l_NbPoints,16);
		else
			_pst_Object->dst_PointNormal = NULL;
	#endif
#else
		if(GEO_b_IsInFix)
			_pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_GEO_p_AllocAlign(sizeof(MATH_tdst_Vector) * _pst_Object->l_NbPoints,16);
		else
			_pst_Object->dst_PointNormal = NULL;
#endif
#endif

		_pst_Object->dst_OriginalPointNormal = NULL;

#ifdef PSX2_TARGET
		_pst_Object->p_CompressedNormals = NULL;
#endif		
#endif		

		if(_pst_Object->dul_PointColors)
		{
			l_Size = (LONG) _pst_Object->dul_PointColors;
			LOA_ul_FileTypeSize[22] += 4 * (l_Size + 1);
#ifdef ACTIVE_EDITORS
            _pst_Object->dul_PointColors = (ULONG *) MEM_GEO_p_Alloc(4 * (l_Size + 1));
#else
			_pst_Object->dul_PointColors = ((ULONG *) MEM_GEO_p_AllocAlign(4 * (l_Size) + 16, 16)) + 3;
#endif
			_pst_Object->dul_PointColors[0] = (ULONG) l_Size;
		}
	}

	/* Alloc MRM */
	if(_pst_Object->p_MRM_ObjectAdditionalInfo)
	{
		/*~~~~~~~~~~~~~~~*/
		ULONG	SavedVAlue;
		/*~~~~~~~~~~~~~~~*/

		SavedVAlue = (ULONG) _pst_Object->p_MRM_ObjectAdditionalInfo;
		_pst_Object->p_MRM_ObjectAdditionalInfo = (GEO_tdst_MRM_Object *) MEM_GEO_p_Alloc(sizeof(GEO_tdst_MRM_Object));
		_pst_Object->p_MRM_ObjectAdditionalInfo->Absorbers = (short *) MEM_GEO_p_Alloc(sizeof(unsigned short) * _pst_Object->l_NbPoints);
		if(SavedVAlue == 0xC0DE0008)
		{
			_pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer = (short *) MEM_GEO_p_Alloc(sizeof(unsigned short) * _pst_Object->l_NbPoints);
		}
		else
			_pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer = NULL;
		_pst_Object->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints = _pst_Object->l_NbPoints;
		_pst_Object->p_MRM_ObjectAdditionalInfo->MinimumNumberOfPoints = 0;
	}

	/* No SKIN allocation here, Skin is only readed */
	if(_pst_Object->l_NbUVs)
	{
		LOA_ul_FileTypeSize[23] += sizeof(GEO_tdst_UV) * _pst_Object->l_NbUVs;
#ifdef _GAMECUBE
		_pst_Object->dst_UV = (GEO_tdst_UV *) MEM_GEO_p_AllocAlign(sizeof(GEO_tdst_UV) * _pst_Object->l_NbUVs, 32);
#else	
		_pst_Object->dst_UV = (GEO_tdst_UV *) MEM_GEO_p_AllocAlign(sizeof(GEO_tdst_UV) * _pst_Object->l_NbUVs, 8);
#endif		
	}

	if(_pst_Object->l_NbElements)
	{
		_pst_Object->dst_Element = (GEO_tdst_ElementIndexedTriangles *) MEM_GEO_p_VMAlloc(sizeof(GEO_tdst_ElementIndexedTriangles) * _pst_Object->l_NbElements);
        L_memset(_pst_Object->dst_Element,0,sizeof(GEO_tdst_ElementIndexedTriangles) * _pst_Object->l_NbElements);
		LOA_ul_FileTypeSize[28] += sizeof(GEO_tdst_ElementIndexedTriangles) * _pst_Object->l_NbElements;
#ifdef JADEFUSION
		for(int i = 0; i < _pst_Object->l_NbElements; i++) 
		{
			_pst_Object->dst_Element[i].pst_StripData = NULL;
#ifdef PSX2_TARGET
			_pst_Object->dst_Element[i].pst_StripDataPS2 = NULL;
			_pst_Object->dst_Element[i].p_ElementCache = NULL;
#endif			
#ifdef _XBOX
			// allocate special DX8 ? 
			_pst_Object->dst_Element[i].pst_Gx8Add = NULL; 
#endif // _XBOX
		}
#endif	
	}

#ifdef _XENON_RENDER
    _pst_Object->b_ForceSoftSkinning = FALSE;
    _pst_Object->b_Particles = FALSE;
#endif
}

/*
 =======================================================================================================================
    Create an object
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_Create(LONG _l_NbPoints, LONG _l_NbUVs, LONG _l_NbElements, LONG _l_NbVertexColors)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object *pst_Object;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Object = (GEO_tdst_Object *) MEM_GEO_p_Alloc(sizeof(GEO_tdst_Object));
	L_memset(pst_Object, 0, sizeof(GEO_tdst_Object));

	GRO_Struct_Init(&pst_Object->st_Id, GRO_Geometric);

	pst_Object->l_NbPoints = _l_NbPoints;
	pst_Object->l_NbUVs = _l_NbUVs;
	pst_Object->l_NbElements = _l_NbElements;
	pst_Object->dul_PointColors = (ULONG *) _l_NbVertexColors;
	// hogsy:	UNDONE - originally was initialising a bunch of data to 0 here, 
	//			but that's unnecessary given the memset above...

	GEO_AllocContent(pst_Object);
	return pst_Object;
}

extern "C" void COL_OK3_RecursiveFree(COL_tdst_OK3_Node *);

/*
 =======================================================================================================================
    Free content of an object
 =======================================================================================================================
 */
void GEO_FreeContent(GEO_tdst_Object *_pst_Object)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	GEO_tdst_ElementIndexedSprite	*pst_ElementSPR, *pst_LastElementSPR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Object == NULL) return;

	GEO_SubObject_Free(_pst_Object);

	if(_pst_Object->l_NbPoints)
	{
		MEM_GEO_v_FreeAlign(_pst_Object->dst_Point);
        _pst_Object->dst_Point = NULL;
		if(_pst_Object->dst_PointNormal) 
        {
            MEM_GEO_v_FreeAlign(_pst_Object->dst_PointNormal);
            _pst_Object->dst_PointNormal = NULL;
        }
#ifdef ACTIVE_EDITORS
		if(_pst_Object->dul_PointColors) MEM_GEO_v_Free(_pst_Object->dul_PointColors);
#else
        if(_pst_Object->dul_PointColors) MEM_GEO_v_FreeAlign(_pst_Object->dul_PointColors - 3);
#endif
        _pst_Object->dul_PointColors = NULL;

#ifdef PSX2_TARGET	
		if(_pst_Object->p_CompressedNormals) MEM_GEO_v_FreeAlign(_pst_Object->p_CompressedNormals);
#endif

		if(_pst_Object->pBiNormales) 
        {
            MEM_GEO_v_FreeAlign(_pst_Object->pBiNormales);
		    _pst_Object->pBiNormales = NULL;
        }

		if(_pst_Object->dst_OriginalPointNormal)
		{
			MEM_FreeAlign(_pst_Object->dst_OriginalPointNormal);
			_pst_Object->dst_OriginalPointNormal = NULL;
		}

		_pst_Object->l_NbPoints = 0;
	}

	/* Free MRM */
	if(_pst_Object->p_MRM_ObjectAdditionalInfo)
	{
		MEM_GEO_v_Free(_pst_Object->p_MRM_ObjectAdditionalInfo->Absorbers);
		if(_pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer)
			MEM_GEO_v_Free(_pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer);
		MEM_GEO_v_Free(_pst_Object->p_MRM_ObjectAdditionalInfo);
		_pst_Object->p_MRM_ObjectAdditionalInfo = NULL;
	}

    GEO_MRM_DeleteLevels(_pst_Object);

	/* Free SKIN */
	if(GEO_SKN_IsSkinned(_pst_Object))
	{
		GEO_SKN_DestroyObjPonderation(_pst_Object->p_SKN_Objectponderation);
		_pst_Object->p_SKN_Objectponderation = NULL;
	}

	if(_pst_Object->l_NbUVs)
	{
		MEM_GEO_v_FreeAlign(_pst_Object->dst_UV);
		_pst_Object->dst_UV  = NULL;
		_pst_Object->l_NbUVs = 0;
	}

	if(_pst_Object->pst_OK3)
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

#if defined (_PC_RETAIL)
	Dx9_ReleaseVBForObject( _pst_Object );
#endif	// defined(_PC_RETAIL)

#if defined (_XBOX)
	Gx8_ReleaseVBForObject( _pst_Object );
#endif	// defined(_XBOX)

/*
#ifdef _XBOX
	Gx8_FreeAddInfo(_pst_Object);
#endif // _XBOX
*/

	if(_pst_Object->l_NbElements)
	{
		pst_Element = _pst_Object->dst_Element;
		pst_LastElement = pst_Element + _pst_Object->l_NbElements;

		for(; pst_Element < pst_LastElement; pst_Element++) 
			GEO_FreeElementContent(pst_Element);

		MEM_GEO_v_Free(_pst_Object->dst_Element);
		_pst_Object->l_NbElements = 0;
	}

	/* Free Sprites */
	if(_pst_Object->l_NbSpritesElements)
	{
		pst_ElementSPR = _pst_Object->dst_SpritesElements;
		pst_LastElementSPR = pst_ElementSPR + _pst_Object->l_NbSpritesElements;
		for(; pst_ElementSPR < pst_LastElementSPR; pst_ElementSPR ++) GEO_FreeSpriteElementContent(pst_ElementSPR);
		MEM_GEO_v_Free(_pst_Object->dst_SpritesElements);
		_pst_Object->l_NbSpritesElements = 0;
	}



	_pst_Object->ulStripFlag = 0;

	GRO_Struct_Free(&_pst_Object->st_Id);
}

/*
 =======================================================================================================================
    Free an object
 =======================================================================================================================
 */
void GEO_Free(GEO_tdst_Object *_pst_Object)
{
	if(_pst_Object == NULL) return;
	if(_pst_Object->st_Id.i->ul_Type != GRO_Geometric) return;	
	if(_pst_Object->st_Id.l_Ref > 0) return;

#ifdef ACTIVE_EDITORS
	LINK_SetDelPointer(_pst_Object);
//	LINK_DelRegisterPointer(_pst_Object);
#endif

	GEO_UnrecordObject(_pst_Object);
	GEO_FreeContent(_pst_Object);

	/* Remove address from loading tables */
	LOA_DeleteAddress(_pst_Object);

	MEM_GEO_v_Free(_pst_Object);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_GetStripFlag(GEO_tdst_Object	*pst_Object , unsigned char	*pc_Buffer)
{
	GEO_tdst_ElementIndexedTriangles	*pst_Element,*pst_LastElement;
	ULONG ReturnValue;
	pst_Element = pst_Object->dst_Element;
	pst_LastElement = pst_Element + pst_Object->l_NbElements;
	
	while (pst_Element < pst_LastElement)
	{
		/* triangles */
		pc_Buffer+= (2*6 + 4 + 4) * pst_Element->l_NbTriangles;
		if(pst_Element->p_MrmElementAdditionalInfo) 
		        pc_Buffer += 4 + 4 + 4;
		if(pst_Element->ul_NumberOfUsedIndex) 		
		        pc_Buffer += 2 * pst_Element->ul_NumberOfUsedIndex;
		pst_Element++;
	}
	((UCHAR *)&ReturnValue)[0] = pc_Buffer[0];
	((UCHAR *)&ReturnValue)[1] = pc_Buffer[1];
	((UCHAR *)&ReturnValue)[2] = pc_Buffer[2];
	((UCHAR *)&ReturnValue)[3] = pc_Buffer[3];
	if(LOA_IsSwapperActive())
	{
		SwapDWord((LONG *) &ReturnValue);
	}
	
	return ReturnValue;
}

#if defined(_GAMECUBE) && defined(_DEBUG)
static ULONG	MaxT = 0;
#endif

#ifdef _XENON_RENDER

void GEO_ResetAllXenonMesh(BOOL _b_PreserveColors, BOOL _b_InvalidateVertexCount, BOOL _b_IgnoreTangentSpace)
{
    OBJ_tdst_GameObject* pst_GO;
    GEO_tdst_Object*     pst_Obj;

    for (int i =0; i < (int) TAB_ul_PFtable_GetMaxNbElems(&GDI_gpst_CurDD->pst_World->st_AllWorldObjects); i++)
    {
        // get the object
        pst_GO = (OBJ_tdst_GameObject*) GDI_gpst_CurDD->pst_World->st_AllWorldObjects.p_Table[i].p_Pointer;
        pst_Obj = (GEO_tdst_Object*)OBJ_p_GetCurrentGeo(pst_GO);

        if ((pst_Obj != NULL) && (pst_Obj->st_Id.i->ul_Type == GRO_Geometric))
        {
            // Mesh was changed, update it
            GEO_ResetXenonMesh(pst_GO, pst_GO->pst_Base->pst_Visu, pst_Obj, _b_PreserveColors, _b_InvalidateVertexCount, _b_IgnoreTangentSpace);
        }
    }
}

void GEO_ResetXenonMesh(OBJ_tdst_GameObject *_pst_GO, GRO_tdst_Visu*, GEO_tdst_Object*, BOOL _b_PreserveColors, BOOL _b_InvalidateVertexCount, BOOL _b_IgnoreTangentSpace)
{
    // SC: Dumb, I know, but I want to keep the parameters for future use
    _b_PreserveColors        = _b_PreserveColors;
    _b_InvalidateVertexCount = _b_InvalidateVertexCount;
    _b_IgnoreTangentSpace    = _b_IgnoreTangentSpace;

    // Restore all the Xenon resources
    GEO_PackGameObject(_pst_GO);

    Xe_InvalidateRenderLists();
}

BOOL GEO_IsXenonMeshVisible(OBJ_tdst_GameObject* _pst_GO, XeMesh* _poMesh)
{
#if defined(ACTIVE_EDITORS)
    if (!GDI_gpst_CurDD->uc_EnableElementCulling)
        return TRUE;
#endif

    // No element culling on GAOs that are always visible
    if (_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysVisible)
        return TRUE;

	// Interface elements are always visible
	if (GDI_gpst_CurDD->ul_DisplayInfo & (GDI_Cul_DI_RenderingInterface | GDI_Cul_DI_RenderingGFX))
		return TRUE;

    if (_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled)
        return FALSE;

    // Always visible when there is no bounding volume to test
    if (!_poMesh->HasBoundingVolume())
        return TRUE;

    if (GDI_gpst_CurDD->pst_CurrentAnim != NULL)
        return TRUE;

    static MATH_tdst_Vector sBoxMin;
    static MATH_tdst_Vector sBoxMax;
    MATH_tdst_Vector* pBoxMin = _poMesh->GetBoundingVolumeMin();
    MATH_tdst_Vector* pBoxMax = _poMesh->GetBoundingVolumeMax();
    if (_pst_GO->pst_GlobalMatrix->lType & MATH_Ci_Scale)
    {
        sBoxMin.x = pBoxMin->x * _pst_GO->pst_GlobalMatrix->Sx;
        sBoxMin.y = pBoxMin->y * _pst_GO->pst_GlobalMatrix->Sy;
        sBoxMin.z = pBoxMin->z * _pst_GO->pst_GlobalMatrix->Sz;

        sBoxMax.x = pBoxMax->x * _pst_GO->pst_GlobalMatrix->Sx;
        sBoxMax.y = pBoxMax->y * _pst_GO->pst_GlobalMatrix->Sy;
        sBoxMax.z = pBoxMax->z * _pst_GO->pst_GlobalMatrix->Sz;

        pBoxMin = &sBoxMin;
        pBoxMax = &sBoxMax;
    }

    MATH_tdst_Matrix oGlobalMatrix;
    MATH_MulMatrixMatrix(&oGlobalMatrix, GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix, 
                                        &GDI_gpst_CurDD->st_Camera.st_Matrix);

#if !defined(XML_CONV_TOOL)
    if (OBJ_CullingOBBoxView(pBoxMin, pBoxMax, 
                              GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix, 
                             &GDI_gpst_CurDD->st_Camera))
    {
        return FALSE;
    }
#endif

    return TRUE;
}

void GEO_CreateXenonDynamicMesh(GRO_tdst_Visu *_pst_Visu, GEO_tdst_Object *_pst_Object)
{
#if !defined(XML_CONV_TOOL)
	GEO_tdst_IndexedTriangle			*pTris, *pTrisEnd;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;

    // alloc one mesh per element
    if(_pst_Visu->l_NbXeElements != 0 && _pst_Visu->l_NbXeElements != _pst_Object->l_NbElements && _pst_Visu->p_XeElements)
    {
        MEM_Free(_pst_Visu->p_XeElements);
        _pst_Visu->p_XeElements   = NULL;
        _pst_Visu->l_NbXeElements = 0;
    }

	if(_pst_Visu->p_XeElements == NULL)
	{
        _pst_Visu->l_NbXeElements = _pst_Object->l_NbElements;
		_pst_Visu->p_XeElements   = (GRO_tdst_XeElement*)MEM_p_Alloc(_pst_Object->l_NbElements*sizeof(GRO_tdst_XeElement));
		L_memset(_pst_Visu->p_XeElements, 0, (_pst_Object->l_NbElements*sizeof(GRO_tdst_XeElement)));
	}

	_pst_Visu->l_VBVertexCount = 0; // always 0 for dynamic mesh

	pst_Element = _pst_Object->dst_Element;
	pst_LastElement = pst_Element + _pst_Object->l_NbElements;

	for(int iElem=0; pst_Element < pst_LastElement; pst_Element++, iElem++)
	{
		// if a mesh is present, it is no longer valid
		_pst_Visu->p_XeElements[iElem].pst_Mesh = NULL;

		if(pst_Element->l_NbTriangles==0)
			continue;

		_pst_Visu->p_XeElements[iElem].pst_Mesh = g_oXeRenderer.RequestDynamicMesh();

		pTris = pst_Element->dst_Triangle;
		pTrisEnd = pTris + pst_Element->l_NbTriangles;

		int iSizeOfElements = sizeof(XeRenderer::XeVertexDyn);

		XeBuffer *pBuffer = _pst_Visu->p_XeElements[iElem].pst_Mesh->GetStream(0)->pBuffer;
		_pst_Visu->p_XeElements[iElem].pst_Mesh->SetStreamComponents(0, XEVC_POSITION | XEVC_COLOR0 | XEVC_TEXCOORD0);

		XeRenderer::XeVertexDyn *pBufferArray = (XeRenderer::XeVertexDyn *)pBuffer->Lock(pst_Element->l_NbTriangles*3, iSizeOfElements);

		pTris = pst_Element->dst_Triangle;
		pTrisEnd = pTris + pst_Element->l_NbTriangles;

		while (pTris < pTrisEnd)
		{
			for (int i = 0; i < 3; ++i)
			{
				*(GEO_Vertex*)&pBufferArray->vPos = _pst_Object->dst_Point[pTris->auw_Index[i]];
                if (_pst_Visu->dul_VertexColors)
                {
                    *(ULONG*)&pBufferArray->ulColor = XeConvertColor(_pst_Visu->dul_VertexColors[pTris->auw_Index[i]+1]);
                }
                else if(_pst_Object->dul_PointColors)
					*(ULONG*)&pBufferArray->ulColor = XeConvertColor(_pst_Object->dul_PointColors[pTris->auw_Index[i]+1]);
#if defined(ACTIVE_EDITORS)
                else
                    *(ULONG*)&pBufferArray->ulColor = XeConvertColor(GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors[pTris->auw_Index[i]]);
#else
				else
					*(ULONG*)&pBufferArray->ulColor = 0xffffffff;
#endif

				if(_pst_Object->dst_UV)
				{
					pBufferArray->UV.fU = _pst_Object->dst_UV[pTris->auw_UV[i]].fU;
					pBufferArray->UV.fV = _pst_Object->dst_UV[pTris->auw_UV[i]].fV;
				}

				pBufferArray++;
			}

			++pTris;
		}

		pBuffer->Unlock();
	}
#endif
}

void
GEO_CreateSoftSkinXenonMesh( GRO_tdst_Visu * _pst_Visu, GEO_tdst_Object *_pst_Object )
{
#if !defined(XML_CONV_TOOL)
    GEO_tdst_IndexedTriangle			*pTris, *pTrisEnd;
    GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;

    ERR_X_Assert( _pst_Visu );
    ERR_X_Assert( GDI_gpst_CurDD->p_Current_Vertex_List );

    // alloc one mesh per element
    if( ( _pst_Visu->l_NbXeElements != 0 && 
          _pst_Visu->l_NbXeElements != _pst_Object->l_NbElements &&
          _pst_Visu->p_XeElements ) ||
         (_pst_Visu->l_VBVertexCount != 0 )) // 0 when dynamic
    {

        GEO_ClearXenonMesh( _pst_Visu, _pst_Object, FALSE, FALSE );
    }

    // alloc one mesh per element
    if( _pst_Visu->p_XeElements == NULL )
    {
        _pst_Visu->p_XeElements = (GRO_tdst_XeElement*)MEM_p_Alloc(_pst_Object->l_NbElements*sizeof(GRO_tdst_XeElement));
        L_memset(_pst_Visu->p_XeElements, 0, (_pst_Object->l_NbElements*sizeof(GRO_tdst_XeElement)));
    }

    _pst_Visu->l_VBVertexCount = 0; // always 0 for dynamic mesh

    // Set the source for vertex color
    ULONG * pColorsSrc = NULL;
    if (_pst_Visu->dul_VertexColors)
    {
        pColorsSrc = _pst_Visu->dul_VertexColors+1;
    }
    else if(_pst_Object->dul_PointColors)
    {
        pColorsSrc = _pst_Object->dul_PointColors+1;
    }

    pst_Element = _pst_Object->dst_Element;
    pst_LastElement = pst_Element + _pst_Object->l_NbElements;
    for(int iElem=0; pst_Element < pst_LastElement; pst_Element++, iElem++)
    {
        // if a mesh is present, it is no longer valid
        _pst_Visu->p_XeElements[iElem].pst_Mesh = NULL;

        if(pst_Element->l_NbTriangles==0)
            continue;

        _pst_Visu->p_XeElements[iElem].pst_Mesh = g_oXeRenderer.RequestDynamicMesh();

        ULONG ulNormalOffset    = 0;
        ULONG ulColorOffset     = 0;
        ULONG ulUVOffset        = 0;

        // See what elements need to be added
        ULONG ulComponents = XEVC_POSITION;
        ULONG ulVertexStride = sizeof( GEO_Vertex );
        ERR_X_Assert( GDI_gpst_CurDD->p_Current_Vertex_List );

        if( pColorsSrc != NULL )
        {
            ulComponents |= XEVC_COLOR0;
            ulColorOffset = ulVertexStride;
            ulVertexStride += sizeof(ULONG);
        }

        if( _pst_Object->dst_UV != NULL )
        {
            ulComponents |= XEVC_TEXCOORD0;
            ulUVOffset = ulVertexStride;
            ulVertexStride += sizeof(GEO_tdst_UV);
        }

        if( _pst_Object->dst_PointNormal != NULL )
        {
            ulComponents |= XEVC_NORMAL;
            ulNormalOffset = ulVertexStride;
            ulVertexStride += sizeof(GEO_Vertex);
        }

        XeBuffer *pBuffer = _pst_Visu->p_XeElements[iElem].pst_Mesh->GetStream(0)->pBuffer;
        _pst_Visu->p_XeElements[iElem].pst_Mesh->SetStreamComponents(0, ulComponents );

        // NOTE : With software skinned objects, none of the vertices are shared
        BYTE * pBufferArray = (BYTE*)pBuffer->Lock( pst_Element->l_NbTriangles * 3, ulVertexStride);

        BYTE * pPositions = pBufferArray;
        BYTE * pNormals   = (ulNormalOffset==0) ? NULL : (pBufferArray + ulNormalOffset) ;
        BYTE * pColor     = (ulColorOffset==0)  ? NULL : (pBufferArray + ulColorOffset);
        BYTE * pUV        = (ulUVOffset==0)     ? NULL : (pBufferArray + ulUVOffset);

        pTris = pst_Element->dst_Triangle;
        pTrisEnd = pTris + pst_Element->l_NbTriangles;

        while (pTris < pTrisEnd)
        {
            for (int i = 0; i < 3; ++i)
            {
                *((GEO_Vertex*)pPositions) = GDI_gpst_CurDD->p_Current_Vertex_List[pTris->auw_Index[i]];
                pPositions += ulVertexStride;

                if( pColor )
                {
                    *((ULONG*)pColor) = XeConvertColor( pColorsSrc[pTris->auw_Index[i]] );
                    pColor += ulVertexStride;
                }

                if( pUV )
                {
                    GEO_tdst_UV * pCurrentUV = (GEO_tdst_UV *)(pUV);

                    pCurrentUV->fU = _pst_Object->dst_UV[pTris->auw_UV[i]].fU;
                    pCurrentUV->fV = _pst_Object->dst_UV[pTris->auw_UV[i]].fV;

                    pUV += ulVertexStride;
                }

                if( pNormals )
                {
                    *((GEO_Vertex*)pNormals) = _pst_Object->dst_PointNormal[pTris->auw_Index[i]];
                    pNormals += ulVertexStride;
                }
            }

            ++pTris;
        }

        pBuffer->Unlock();
    }
#endif
}


void
GEO_CreateMorphedXenonMesh( GRO_tdst_Visu * _pst_Visu, GEO_tdst_Object *_pst_Object )
{
#if !defined(XML_CONV_TOOL)

    // 
    // A morphed mesh is made up of a static part which remains constant over the 
    // life of the object (colors, uv, skinning info, indices) and a dynamic part that is updated 
    // every frame (positions, normals, tangents)
    //

    GEO_tdst_IndexedTriangle			*pTris, *pTrisEnd;
    GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
  
    ERR_X_Assert( _pst_Object->b_CreateMorphedMesh );

    if( !_pst_Visu )
    {
        return;
    }

    BOOL b_IsMorphInitialized = (_pst_Visu->pVB_MorphDynamic != NULL);

    //
    // Prepare mesh for morphing
    //
    if( !b_IsMorphInitialized )
    {
        GEO_ClearXenonMesh( _pst_Visu, _pst_Object, FALSE, TRUE );
       
        // Elements and tangent space have been allocated in modifier
        ERR_X_Assert( _pst_Visu->p_XeElements != NULL );

        //
        // Compute the total number of vertices (no vertices are shared)
        //
        ULONG nbVertex = 0;
        pst_Element = _pst_Object->dst_Element;
        pst_LastElement = pst_Element + _pst_Object->l_NbElements;
        for(; pst_Element < pst_LastElement; pst_Element++)
        {
            nbVertex += pst_Element->l_NbTriangles*3;
        }

        _pst_Visu->l_VBVertexCount = nbVertex;

        if( nbVertex == 0 )
        {
            return;
        }

        //
        // Build the static part of the mesh
        //

        // Set the source for vertex color
        ULONG * pColorsSrc = NULL;
        if( _pst_Visu->dul_VertexColors )
        {
            pColorsSrc = _pst_Visu->dul_VertexColors+1;
        }
        else if(_pst_Object->dul_PointColors)
        {
            pColorsSrc = _pst_Object->dul_PointColors+1;
        }

        // Compute the components flags and the size of a static vertex
        ULONG ulComponents   = 0;
        ULONG ulVertexStride = 0;
        ULONG ulSkinOffset   = 0;
        ULONG ulColorOffset  = 0;
        ULONG ulUVOffset     = 0;

        if( pColorsSrc != NULL )
        {
            ulComponents |= XEVC_COLOR0;
            ulColorOffset = ulVertexStride;
            ulVertexStride += sizeof(ULONG);
        }

        if( _pst_Object->dst_UV != NULL )
        {
            ulComponents |= XEVC_TEXCOORD0;
            ulUVOffset = ulVertexStride;
            ulVertexStride += sizeof(GEO_tdst_UV);
        }

#ifndef VS_FORCE_SW_SKINNING
        XePackSkinning oSkinningInfo( _pst_Object );

        // HW skinning
        BOOL bSkin = FALSE;
        BOOL bSkinning = FALSE;
        if(_pst_Object->p_SKN_Objectponderation)
        {
            for (ULONG ul_MatrixCounter = 0; ul_MatrixCounter < _pst_Object->p_SKN_Objectponderation->NumberPdrtLists; ul_MatrixCounter++)
            {
                if(!MATH_b_EqMatrixWithEpsilon(&MATH_gst_IdentityMatrix, &_pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ul_MatrixCounter]->st_FlashedMatrix, 0.0001f))
                {
                    bSkin = TRUE;
                    break;
                }
            }
        }

        if( GEO_SKN_IsSkinned(_pst_Object) && bSkin )
        {
            bSkinning = TRUE;
            ulComponents |= (XEVC_BLENDWEIGHT4 | XEVC_BLENDINDICES);
            ulSkinOffset = ulVertexStride;
            ulVertexStride += (4 * sizeof(FLOAT)) +(XENON_MAX_WEIGHT * sizeof(BYTE));
        }
#endif

        // Create one static vertex buffer per mesh
        BYTE * pStaticArray = NULL;
        if( ulComponents != 0 )
        {
            _pst_Visu->pVB_MorphStatic = g_XeBufferMgr.CreateVertexBuffer( nbVertex, ulVertexStride, TRUE );
            ERR_X_Assert( _pst_Visu->pVB_MorphStatic );

            pStaticArray = (BYTE*) _pst_Visu->pVB_MorphStatic->Lock( nbVertex, ulVertexStride );
            ERR_X_Assert( pStaticArray );
         }

        // Create one dynamic buffer per mesh
        _pst_Visu->pVB_MorphDynamic = g_XeBufferMgr.CreateDynVertexBuffer( );
        ERR_X_Assert( _pst_Visu->pVB_MorphDynamic );
  
        // Prepare the initial destination pointers
        BYTE * pSkinning  = (bSkinning && pStaticArray )           ? (pStaticArray + ulSkinOffset)  : NULL;
        BYTE * pColor     = (pColorsSrc && pStaticArray )          ? (pStaticArray + ulColorOffset) : NULL;
        BYTE * pUV        = (_pst_Object->dst_UV && pStaticArray ) ? (pStaticArray + ulUVOffset)    : NULL;

        //
        // Build a mesh object per element
        // 
        pst_Element = _pst_Object->dst_Element;
        pst_LastElement = pst_Element + _pst_Object->l_NbElements;
        LONG iElem = 0;
        USHORT us_Index = 0;
        for(; pst_Element < pst_LastElement; pst_Element++, iElem++ )
        {
            if( pst_Element->l_NbTriangles == 0 )
            {
                continue;
            }

            // Create a mesh for element
            _pst_Visu->p_XeElements[iElem].pst_Mesh = new XeMesh();

            if( ulComponents != 0 )
            {
                // Add the static vertex buffer to mesh 
                ERR_X_Assert( _pst_Visu->pVB_MorphStatic );             
                _pst_Visu->p_XeElements[iElem].pst_Mesh->AddStream( ulComponents,  _pst_Visu->pVB_MorphStatic );
            }
            _pst_Visu->p_XeElements[iElem].pst_Mesh->AddStream( (XEVC_POSITION | XEVC_TANGENT | XEVC_NORMAL),  _pst_Visu->pVB_MorphDynamic );

            // Allocate index array
            USHORT * pIndicesArray = NULL;
            _pst_Visu->p_XeElements[iElem].pst_IndexBuffer = (XeIndexBuffer*)g_XeBufferMgr.CreateIndexBuffer( pst_Element->l_NbTriangles * 3 );
            ERR_X_Assert( _pst_Visu->p_XeElements[iElem].pst_IndexBuffer );  
            _pst_Visu->p_XeElements[iElem].pst_Mesh->SetIndices( _pst_Visu->p_XeElements[iElem].pst_IndexBuffer );

            pIndicesArray = (USHORT*)_pst_Visu->p_XeElements[iElem].pst_IndexBuffer->Lock( pst_Element->l_NbTriangles * 3 );
            ERR_X_Assert( pIndicesArray );  
  
            pTris = pst_Element->dst_Triangle;
            pTrisEnd = pTris + pst_Element->l_NbTriangles;

            while( pTris < pTrisEnd )
            {
                for( int i = 0; i < 3; ++i )
                {
                    if( pColor )
                    {
                        *((ULONG*)pColor) = XeConvertColor( pColorsSrc[pTris->auw_Index[i]] );
                        pColor += ulVertexStride;
                    }

                    if( pUV )
                    {
                        GEO_tdst_UV * pCurrentUV = (GEO_tdst_UV *)(pUV);

                        pCurrentUV->fU = _pst_Object->dst_UV[pTris->auw_UV[i]].fU;
                        pCurrentUV->fV = _pst_Object->dst_UV[pTris->auw_UV[i]].fV;

                        pUV += ulVertexStride;
                    }

#ifndef VS_FORCE_SW_SKINNING
                    if( pSkinning )
                    {
                        L_memcpy(pSkinning, &oSkinningInfo.m_poVertex[pTris->auw_Index[i]], sizeof(VertexSkinning));
                        pSkinning += ulVertexStride;
                    }
#endif
                    *(pIndicesArray++) = us_Index++;
                }
                ++pTris;
            }

            _pst_Visu->p_XeElements[iElem].pst_IndexBuffer->Unlock( );

            if(bSkinning)
            {
#ifndef VS_FORCE_SW_SKINNING
                // save max number of weights and bones
                _pst_Visu->p_XeElements[iElem].pst_Mesh->SetMaxWeights(oSkinningInfo.m_iMaxNbrOfWeight);
                _pst_Visu->p_XeElements[iElem].pst_Mesh->SetMaxBones(oSkinningInfo.m_iMaxMatrixIndex + 1);
#else
                _pst_Visu->p_XeElements[iElem].pst_Mesh->SetMaxWeights(0);
                _pst_Visu->p_XeElements[iElem].pst_Mesh->SetMaxBones(0);
#endif
            }
            else
            {
                _pst_Visu->p_XeElements[iElem].pst_Mesh->SetMaxWeights(0);
                _pst_Visu->p_XeElements[iElem].pst_Mesh->SetMaxBones(0);
            }
        }

        if( pStaticArray )
        {
            _pst_Visu->pVB_MorphStatic->Unlock();
        }
    }

    if( _pst_Visu->l_VBVertexCount == 0 )
    {
        return;
    }

    //
    // Update the dynamic part of the morphing mesh
    // 
    ULONG ul_DynamicVertexSize = 3 * sizeof( GEO_Vertex ) + sizeof( float );
    ERR_X_Assert( _pst_Visu->pVB_MorphDynamic );
    BYTE * pDynamicArray = (BYTE*)_pst_Visu->pVB_MorphDynamic->Lock( _pst_Visu->l_VBVertexCount, ul_DynamicVertexSize, FALSE, TRUE );
    ERR_X_Assert( pDynamicArray != NULL );

    // Get the pointers to dest data
    BYTE * pPositionDst  = pDynamicArray;
    BYTE * pNormalDst    = pDynamicArray + sizeof( GEO_Vertex );
    BYTE * pTangentDst   = pDynamicArray + 2*sizeof( GEO_Vertex );

    pst_Element = _pst_Object->dst_Element;
    pst_LastElement = pst_Element + _pst_Object->l_NbElements;
    LONG iElem = 0;
    for(; pst_Element < pst_LastElement; pst_Element++, iElem++ )
    {
        if( pst_Element->l_NbTriangles == 0 )
        {
            continue;
        }
 
        // Get the pointers to source data
        GEO_Vertex * pPositionSrc  = GDI_gpst_CurDD->p_Current_Vertex_List;
        // TODO: JFR this is a temporary patch until morphing targets are created
        GEO_Vertex * pNormalSrc    = g_oMorphComputingBuffers.GetNormals( );
        //GEO_Vertex * pNormalSrc    = _pst_Object->dst_PointNormal;

        GEO_Vertex * pTangentSrc   = g_oMorphComputingBuffers.GetTangents(  );
        GEO_tdst_TextureSpaceBasis * dst_TangentSpaceSrc = _pst_Visu->p_XeElements[0].dst_TangentSpace;
        ERR_X_Assert( pPositionSrc && pNormalSrc && pTangentSrc && dst_TangentSpaceSrc );

        pTris = pst_Element->dst_Triangle;
        pTrisEnd = pTris + pst_Element->l_NbTriangles;
        LONG p = 0;
        while( pTris < pTrisEnd )
        {
            for( int i = 0; i < 3; ++i )
            {
                *((GEO_Vertex*)pPositionDst) = pPositionSrc[pTris->auw_Index[i]];
                pPositionDst += ul_DynamicVertexSize;

                //*((GEO_Vertex*)pNormalDst) = dst_TangentSpaceSrc[pTris->auw_Index[i]].SxT; //pNormalSrc[pTris->auw_Index[i]];
                *((GEO_Vertex*)pNormalDst) = pNormalSrc[pTris->auw_Index[i]];
                pNormalDst += ul_DynamicVertexSize;

                //*((GEO_Vertex*)pTangentDst) = dst_TangentSpaceSrc[pTris->auw_Index[i]].S;
                *((GEO_Vertex*)pTangentDst) = pTangentSrc[pTris->auw_Index[i]];
                *(((float*)pTangentDst)+3) = dst_TangentSpaceSrc[pTris->auw_Index[i]].BinormalDirection;
                pTangentDst += ul_DynamicVertexSize;
            }
            ++pTris;
        }
    }

    _pst_Visu->pVB_MorphDynamic->Unlock();
    _pst_Object->b_CreateMorphedMesh = FALSE;
#endif
}


void GEO_ClearXenonMesh(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Object, BOOL _b_PreserveColors, BOOL _b_KeepTangents)
{
#if !defined(XML_CONV_TOOL)
    if (!_pst_Object || !_pst_Visu)
        return;

    THREAD_SAFE_VB_RELEASE(_pst_Visu->pVB_MorphStatic );
    THREAD_SAFE_VB_RELEASE(_pst_Visu->pVB_MorphDynamic );
    _pst_Object->b_CreateMorphedMesh = FALSE;
    
    _pst_Visu->l_VBVertexCount = 0;

    THREAD_SAFE_VB_RELEASE(_pst_Visu->p_VBVertex);
    THREAD_SAFE_VB_RELEASE(_pst_Visu->p_VBFurOffsets);

    if (_pst_Visu->p_FurOffsetVertex != NULL)
    {
        MEM_Free(_pst_Visu->p_FurOffsetVertex);
        _pst_Visu->p_FurOffsetVertex = NULL;
    }

    _pst_Visu->ul_VBObjectValidate = 0;
    for (int i = 0; i < _pst_Visu->l_NbXeElements; ++i)
    {
		if ( _pst_Visu->p_XeElements[i].pst_Mesh &&
			!_pst_Visu->p_XeElements[i].pst_Mesh->IsDynamic())
		{
            THREAD_SAFE_MESH_DELETE(_pst_Visu->p_XeElements[i].pst_Mesh);
		}
		_pst_Visu->p_XeElements[i].pst_Mesh = NULL;

        THREAD_SAFE_IB_RELEASE(_pst_Visu->p_XeElements[i].pst_IndexBuffer);

#if defined(ACTIVE_EDITORS)
        if (_pst_Visu->p_XeElements[i].puw_Indices)
        {
            MEM_Free(_pst_Visu->p_XeElements[i].puw_Indices);
            _pst_Visu->p_XeElements[i].puw_Indices = NULL;
        }
#endif

        if (_pst_Visu->p_XeElements[i].dst_TangentSpace && !_b_KeepTangents)
        {
            MEM_GEO_v_Free(_pst_Visu->p_XeElements[i].dst_TangentSpace);
            _pst_Visu->p_XeElements[i].dst_TangentSpace = NULL;
        }
    }

    if( !_b_KeepTangents )
    {
        MEM_Free(_pst_Visu->p_XeElements);
        _pst_Visu->p_XeElements   = NULL;
        _pst_Visu->l_NbXeElements = 0;
    }

    _pst_Visu->ul_PackedDataKey = BIG_C_InvalidKey;

#endif // XML_CONV_TOOL
}

#endif

#if defined(XML_CONV_TOOL)
std::list<void* > gMrmElementAdditionalInfoPtrList;
#endif

void GEO_v_CreateElementsFromBuffer(char **ppc_Buffer, GEO_tdst_ElementIndexedTriangles *pst_Element , ULONG l_NbElements, GEO_tdst_Object *pst_Object)
{
    int i;
    GEO_tdst_IndexedTriangle *pst_Triangle, *pst_Last;
	GEO_tdst_OneStrip					*pStrip, *pLastStrip;
	ULONG								ul_Count;
#ifndef ACTIVE_EDITORS
	ULONG								ul_Count1;
#endif

    /* Read all element data */
    GEO_tdst_ElementIndexedTriangles *pst_FirstElement = pst_Element;
	GEO_tdst_ElementIndexedTriangles *pst_LastElement = pst_Element + l_NbElements;

	for(; pst_Element < pst_LastElement; pst_Element++)
	{
		pst_Element->l_NbTriangles = LOA_ReadLong(ppc_Buffer);
		pst_Element->l_MaterialId = LOA_ReadLong(ppc_Buffer);
		pst_Element->p_MrmElementAdditionalInfo = (GEO_tdst_MRM_Element *) LOA_ReadULong(ppc_Buffer);
#if defined(XML_CONV_TOOL)
		gMrmElementAdditionalInfoPtrList.push_back(pst_Element->p_MrmElementAdditionalInfo);
#endif
		pst_Element->ul_NumberOfUsedIndex = LOA_ReadULong(ppc_Buffer);

		//Skip ElementInfo
		for(i = 0; i < 6; ++i)
		{
			LOA_ReadLong_Ed(ppc_Buffer, NULL);
		}
	}
	
	/* */
#ifdef _DSPL_ALLOC	
    if(GEO_GetStripFlag(pst_Object , *ppc_Buffer) & GEO_C_Strip_DataValid)
	    DoGEOAllocFromEnd = 1;//*/
#endif

	/* Read all element content */
	{
		GEO_tdst_IndexedTriangle * p;
		for(pst_Element = pst_FirstElement; pst_Element < pst_LastElement; pst_Element++)
		{
			GEO_AllocElementContent(pst_Element);
			/* Read triangles */
			for(i = 0, p = pst_Element->dst_Triangle; i < pst_Element->l_NbTriangles; ++i, ++p)
			{
				_LOA_ReadUShort(ppc_Buffer, (USHORT *) &p->auw_Index, LOA_eBinEngineData);
				_LOA_ReadUShort(ppc_Buffer, (USHORT *) &p->auw_Index+1, LOA_eBinEngineData);
				_LOA_ReadUShort(ppc_Buffer, (USHORT *) &p->auw_Index+2, LOA_eBinEngineData);
				_LOA_ReadUShort(ppc_Buffer, (USHORT *) &p->auw_UV, LOA_eBinEngineData);
				_LOA_ReadUShort(ppc_Buffer, (USHORT *) &p->auw_UV+1, LOA_eBinEngineData);
				_LOA_ReadUShort(ppc_Buffer, (USHORT *) &p->auw_UV+2, LOA_eBinEngineData);
				_LOA_ReadULong(ppc_Buffer, &p->ul_SmoothingGroup, LOA_eBinEngineData);
				/* ULONG ul_MaxFlags doesn't exist in PS2! */
		#if	defined(PSX2_TARGET) || defined(_GAMECUBE)
				_LOA_ReadULong(ppc_Buffer, NULL, LOA_eBinEngineData);
		#else
				_LOA_ReadULong(ppc_Buffer, &p->ul_MaxFlags, LOA_eBinEngineData);
#if !defined(XML_CONV_TOOL)
				p->ul_MaxFlags &= 0x7FFFFFFF;
		#endif
#endif
			}

			if(pst_Element->p_MrmElementAdditionalInfo)
			{
				pst_Element->p_MrmElementAdditionalInfo->ul_One_UV_Per_Point_Per_Element_Base = LOA_ReadULong(ppc_Buffer);
				pst_Element->p_MrmElementAdditionalInfo->ul_RealNumberOfTriangle = LOA_ReadULong(ppc_Buffer);
				// pul_Number_Of_Triangle_vs_Point_Equivalence is not used
				pst_Element->p_MrmElementAdditionalInfo->pul_Number_Of_Triangle_vs_Point_Equivalence = NULL;
				LOA_ReadULong_Ed(ppc_Buffer, (ULONG*)&pst_Element->p_MrmElementAdditionalInfo->pul_Number_Of_Triangle_vs_Point_Equivalence);
			}

			if(pst_Element->ul_NumberOfUsedIndex)
			{
				USHORT * p;
				for(i = 0, p = pst_Element->pus_ListOfUsedIndex; (ULONG)i < pst_Element->ul_NumberOfUsedIndex; ++i, ++p)
				{
					*p = LOA_ReadUShort(ppc_Buffer);
				}
				pst_Element->pus_ListOfUsedIndex[pst_Element->ul_NumberOfUsedIndex] = 0xffff;
			}

			/* To avoid bad index in triangle */
			pst_Triangle = pst_Element->dst_Triangle;
			pst_Last = pst_Triangle + pst_Element->l_NbTriangles;
			for(; pst_Triangle < pst_Last; pst_Triangle++)
			{
				for(i = 0; i < 3; i++)
				{
					if(pst_Triangle->auw_Index[i] >= (unsigned short)pst_Object->l_NbPoints)
						pst_Triangle->auw_Index[i] = (unsigned short)pst_Object->l_NbPoints - 1;
					if(pst_Object->l_NbUVs)
					{
						if(pst_Triangle->auw_UV[i] >= (unsigned short)pst_Object->l_NbUVs)
							pst_Triangle->auw_UV[i] = (unsigned short)pst_Object->l_NbUVs - 1;
					}
				}
			}
		}
	}
	/* *** strip data begin *** */

	/* object flag */
    pst_Object->ulStripFlag = LOA_ReadULong(ppc_Buffer);

	if(pst_Object->ulStripFlag & GEO_C_Strip_DataValid)
	{
		GEO_tdst_MinVertexData * p;
	
		for(pst_Element = pst_FirstElement; pst_Element < pst_LastElement; pst_Element++)
		{
			/* allocate space for strip data */
			pst_Element->pst_StripData = (GEO_tdst_StripData *) MEM_GEO_p_VMAlloc(sizeof(GEO_tdst_StripData));
			L_memset( pst_Element->pst_StripData, 0, sizeof(GEO_tdst_StripData) );

			/* element flag */
			pst_Element->pst_StripData->ulFlag = LOA_ReadULong(ppc_Buffer);

			/* strip number */
			pst_Element->pst_StripData->ulStripNumber = LOA_ReadULong(ppc_Buffer);

			/* statistics aren't saved */
			pst_Element->pst_StripData->ulAveLength = 0;
			pst_Element->pst_StripData->ulMaxLength = 0;
			pst_Element->pst_StripData->ulMinLength = 0;

			/* list of strips */
			pst_Element->pst_StripData->pStripList = (GEO_tdst_OneStrip *) MEM_GEO_p_VMAlloc(sizeof(GEO_tdst_OneStrip) * pst_Element->pst_StripData->ulStripNumber);
			L_memset( pst_Element->pst_StripData->pStripList, 0, sizeof(GEO_tdst_OneStrip) * pst_Element->pst_StripData->ulStripNumber );

			pStrip = pst_Element->pst_StripData->pStripList;
			pLastStrip = pStrip + pst_Element->pst_StripData->ulStripNumber;
			ul_Count = 0;
			for(; pStrip < pLastStrip; pStrip++)
			{
				/* size of the strip */
				pStrip->ulVertexNumber = LOA_ReadULong(ppc_Buffer);
				ul_Count += (pStrip->ulVertexNumber & 0x7fffffff);

				/* index list */
				if (!(pStrip->ulVertexNumber & 0x80000000))
				{
					for(i = 0; (ULONG)i < pStrip->ulVertexNumber; ++i)
					{
						LOA_ReadULong_Ed(ppc_Buffer, NULL); // skip the vertex data because it hasn't the 0x80000000 flag
					}
				}
				else
					pStrip->ulVertexNumber &= 0x7fffffff;

				/* vertex data */
#if !defined(ACTIVE_EDITORS) && !defined(_DSPL_ALLOC)
				pStrip->pMinVertexDataList = (GEO_tdst_MinVertexData *) MEM_p_AllocFromEnd(sizeof(GEO_tdst_MinVertexData) * pStrip->ulVertexNumber);
#else
				pStrip->pMinVertexDataList = (GEO_tdst_MinVertexData *) MEM_GEO_p_Alloc(sizeof(GEO_tdst_MinVertexData) * pStrip->ulVertexNumber);
#endif
				for(i = 0, p = pStrip->pMinVertexDataList; (ULONG)i < pStrip->ulVertexNumber; ++i, ++p)
				{
					p->auw_Index = LOA_ReadUShort(ppc_Buffer);
					p->auw_UV = LOA_ReadUShort(ppc_Buffer);
				}
			}

#ifndef ACTIVE_EDITORS
			pst_Element->pst_StripData->pStripDatas = (GEO_tdst_MinVertexData *) MEM_GEO_p_VMAlloc(sizeof(GEO_tdst_MinVertexData) * ul_Count);
			pLastStrip = pst_Element->pst_StripData->pStripList;
			pStrip = pLastStrip + pst_Element->pst_StripData->ulStripNumber - 1;
			ul_Count1 = 0;
			for(; pStrip >= pLastStrip; pStrip--)
			{
				ul_Count1 += pStrip->ulVertexNumber;
				L_memcpy(pst_Element->pst_StripData->pStripDatas + ul_Count - ul_Count1, pStrip->pMinVertexDataList, sizeof(GEO_tdst_MinVertexData) * pStrip->ulVertexNumber);
#ifndef _DSPL_ALLOC
				MEM_FreeFromEnd(pStrip->pMinVertexDataList);
#endif				
				pStrip->pMinVertexDataList = pst_Element->pst_StripData->pStripDatas + ul_Count - ul_Count1;
			}
#endif
		}
	}
	else
	{
		for(pst_Element = pst_FirstElement; pst_Element < pst_LastElement; pst_Element++)
		{		
			pst_Element->pst_StripData = NULL;
#ifdef PSX2_TARGET
			pst_Element->pst_StripDataPS2 = NULL;
#endif
		}
	}
	/* *** strip data end *** */
}

#if defined(XML_CONV_TOOL)
BOOL gGeoHasCode2002;
int  gGeoVersion;
#endif

extern "C" void COL_Load_Nodes_Recursively(COL_tdst_OK3_Node *, COL_tdst_OK3 *, char **);


/*
 =======================================================================================================================
    Aim:    Load object from file
 =======================================================================================================================
 */
void *GEO_p_CreateFromBuffer(GRO_tdst_Struct *_pst_Id, char **ppc_Buffer, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char								*pc_Buffer;
	GEO_tdst_Object						*pst_Object;
	LONG								l_Size,bHasMRMLevels,bHasIndirection;
	ULONG								ul_Temp;
	int									i;
    ULONG                               ul_PointColors;
    LONG lVersionNb = 0; 
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(XML_CONV_TOOL)
	gGeoHasCode2002 = FALSE;
	gGeoVersion = 0;
#endif
#ifdef _DSPL_ALLOC
	DoGEOAllocFromEnd = 0;
#endif

	pc_Buffer = *ppc_Buffer;

	/* Create object structure and init structure Id */
	pst_Object = GEO_pst_Create(0, 0, 0, 0);

	/* GRO_Struct_SetName(&pst_Object->st_Id, _pst_Id->sz_Name); */

	/* Get structure content */
    // We insert the reading of a version number and additional data (size cannot be equal to 0xC0DE2002).
    pst_Object->p_MRM_Levels = NULL;
    ul_Temp = LOA_ReadLong(&pc_Buffer);
    if (ul_Temp == 0xC0DE2002)
    {
        lVersionNb = LOA_ReadLong(&pc_Buffer);
        ERR_X_Assert(lVersionNb <= 4);
#if defined(XML_CONV_TOOL)
		gGeoHasCode2002 = TRUE;
		gGeoVersion = lVersionNb;
#endif
        pst_Object->l_NbPoints = LOA_ReadLong(&pc_Buffer);
        
        bHasMRMLevels = LOA_ReadLong(&pc_Buffer);
        if (bHasMRMLevels)
	        bHasIndirection = LOA_ReadLong(&pc_Buffer);
	    else
		    bHasIndirection = 0;
    }
    else
    {
        bHasMRMLevels = 0;
        bHasIndirection = 0;
    	pst_Object->l_NbPoints = ul_Temp;
    }

    pst_Object->dul_PointColors = (ULONG *)LOA_ReadULong(&pc_Buffer);
	pst_Object->l_NbUVs = LOA_ReadLong(&pc_Buffer);
	pst_Object->l_NbElements = LOA_ReadLong(&pc_Buffer);
	
    /* editor */
    LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&pst_Object->ul_EditorFlags ); 

    ul_PointColors = (ULONG) pst_Object->dul_PointColors;
    if (ul_PointColors)
        pst_Object->dul_PointColors = (ULONG*)ul_PointColors;

	/* Get MRM */
	pst_Object->p_MRM_ObjectAdditionalInfo = (GEO_tdst_MRM_Object *) LOA_ReadULong(&pc_Buffer);

	/* Swap dummy data */
	ul_Temp = LOA_ReadULong(&pc_Buffer);
	pst_Object->p_SKN_Objectponderation = NULL;

	/* Load Skin if here */
	if((ul_Temp & 0xC0DE2002) == 0xC0DE2002)		/* Special Code for Skin. Could be used later for  other geometric informations */
		pc_Buffer += GEO_SKN_Load(pc_Buffer, pst_Object);

#ifdef KER_IS_NOT_A_CHEVRE
	/* Load OK3 if here */
	if(ul_Temp & 0x01)	/* Special Code for OK3. */
	{
		/*~~~~~~~~*/
		int j, k, l;
		/*~~~~~~~~*/
		
		pst_Object->pst_OK3 = (COL_tdst_OK3 *) MEM_p_Alloc(sizeof(COL_tdst_OK3));
#ifdef ACTIVE_EDITORS
		pst_Object->pst_OK3->paul_Tag = (ULONG *) MEM_p_Alloc(pst_Object->l_NbPoints);
#else
		pst_Object->pst_OK3->paul_Tag = NULL;
#endif
		
		pst_Object->pst_OK3->ul_NumBox = LOA_ReadULong(&pc_Buffer);
		pst_Object->pst_OK3->pst_OK3_Boxes = (COL_tdst_OK3_Box *) MEM_p_VMAlloc(pst_Object->pst_OK3->ul_NumBox * sizeof(COL_tdst_OK3_Box));
		
		for(j = 0; j < (int) pst_Object->pst_OK3->ul_NumBox; j++)
		{
			pst_Object->pst_OK3->pst_OK3_Boxes[j].ul_NumElement = LOA_ReadULong(&pc_Buffer);
			
			LOA_ReadVector(&pc_Buffer, &(pst_Object->pst_OK3->pst_OK3_Boxes[j].st_Max));
			LOA_ReadVector(&pc_Buffer, &(pst_Object->pst_OK3->pst_OK3_Boxes[j].st_Min));

			if(pst_Object->pst_OK3->pst_OK3_Boxes[j].ul_NumElement)
			{
				pst_Object->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element = (COL_tdst_OK3_Element *) MEM_p_VMAlloc(pst_Object->pst_OK3->pst_OK3_Boxes[j].ul_NumElement * sizeof(COL_tdst_OK3_Element));
				for(k = 0; k < (int) pst_Object->pst_OK3->pst_OK3_Boxes[j].ul_NumElement; k++)
				{
					pst_Object->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].uw_Element = LOA_ReadUShort(&pc_Buffer);
					pst_Object->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].uw_NumTriangle = LOA_ReadUShort(&pc_Buffer);
					pst_Object->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].puw_OK3_Triangle = (USHORT *) MEM_p_VMAlloc(pst_Object->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].uw_NumTriangle * sizeof(USHORT));

					for(l = 0; l < pst_Object->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].uw_NumTriangle; l++)
					{
						pst_Object->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element[k].puw_OK3_Triangle[l] = LOA_ReadUShort(&pc_Buffer);
					}
				}
			}
			else
				pst_Object->pst_OK3->pst_OK3_Boxes[j].pst_OK3_Element = NULL;
		}

		/* Load Hierarchy */
		pst_Object->pst_OK3->pst_OK3_God = (COL_tdst_OK3_Node *) MEM_p_VMAlloc(sizeof(COL_tdst_OK3_Node));
		COL_Load_Nodes_Recursively(pst_Object->pst_OK3->pst_OK3_God, pst_Object->pst_OK3, &pc_Buffer);
	
	}
#endif

	/* Alloc array for objects data (points, normals, UVs, elements) */
	GEO_AllocContent(pst_Object);

#ifdef ACTIVE_EDITORS
	if(pst_Object->l_NbPoints > 3000)
	{
		LINK_PrintStatusMsg("Object with more than 3000 points detected\n");
	}
#endif
#ifdef _DEBUG
#ifdef _GAMECUBE
	if(pst_Object->l_NbPoints > 3000)
	{
		char asz_Log[512];
		
		sprintf(asz_Log, "Object with more than 3000 points detected [%u] \n", pst_Object->l_NbPoints);
		OSReport(asz_Log);
	}
	
	if(pst_Object->l_NbPoints > MaxT)
	{
		char asz_Log[512];
		
		sprintf(asz_Log, "Max vertices = %u \n", pst_Object->l_NbPoints);
		OSReport(asz_Log);
		MaxT = pst_Object->l_NbPoints;
	}	
#endif
#endif
	/* Get list of points and normals */
	l_Size = sizeof(MATH_tdst_Vector) * pst_Object->l_NbPoints;

	{
		GEO_Vertex	* V = pst_Object->dst_Point;
		for(i = 0; i < pst_Object->l_NbPoints; i++, V++)
		{
			LOA_ReadVector(&pc_Buffer, (MATH_tdst_Vector*) V);
		}
	}
	{
		MATH_tdst_Vector * V = pst_Object->dst_PointNormal;
		for(i = 0; i < pst_Object->l_NbPoints; i++, V++)
		{
#if defined(_XENON_RENDER)
            LOA_ReadVector(&pc_Buffer, V);
#else
			LOA_ReadVector_Ed(&pc_Buffer, V);
#endif
		}
	}

	/* Get MRM Information */
	if(pst_Object->p_MRM_ObjectAdditionalInfo)
	{
		SHORT * p;
		
		for(i = 0, p = pst_Object->p_MRM_ObjectAdditionalInfo->Absorbers; i < pst_Object->l_NbPoints; i++, p++)
		{
			*p = LOA_ReadShort(&pc_Buffer);
		}
		
		if(pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer)
		{
			for(i = 0, p = pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer; i < pst_Object->l_NbPoints; i++, p++)
			{
				*p = LOA_ReadShort(&pc_Buffer);
			}
		}

		pst_Object->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints = pst_Object->l_NbPoints;
		pst_Object->p_MRM_ObjectAdditionalInfo->MinimumNumberOfPoints = LOA_ReadULong(&pc_Buffer);
        if (lVersionNb == 0)
        {
    		for(i = 0; i < 8; ++i)
    			GEO_gdf_MRMQualityCurve[i] = LOA_ReadFloat(&pc_Buffer);
        }
    }
    else
        GEO_MRM_SetCurve(pst_Object, 1.5f , 1.0f);

	/* Get list of vertex colors */
    if ( ul_PointColors )
    {
		ULONG * p;
        l_Size = ul_PointColors ;
		for(i = 0, p = pst_Object->dul_PointColors + 1;i < l_Size; ++i, ++p)
		{
			*p = LOA_ReadULong(&pc_Buffer);
		}
    }

	/* Get list of UVs */
	{
		GEO_tdst_UV * p;
		for(i = 0, p = pst_Object->dst_UV; i < pst_Object->l_NbUVs; ++i, ++p)
		{
			p->fU = LOA_ReadFloat(&pc_Buffer);
			p->fV = LOA_ReadFloat(&pc_Buffer);
		}
	}

    // Create elements for geometry.
    GEO_v_CreateElementsFromBuffer(&pc_Buffer, pst_Object->dst_Element, pst_Object->l_NbElements, pst_Object);

    // Create elements for MRM levels.
    if (bHasMRMLevels)
    {
        pst_Object->p_MRM_Levels = GEO_MRM_CreateLevels();

        if (lVersionNb >= 4)
            pst_Object->p_MRM_Levels->f_DistanceCoef = LOA_ReadFloat(&pc_Buffer);

        pst_Object->p_MRM_Levels->ul_LevelNb = LOA_ReadULong(&pc_Buffer);

        pst_Object->p_MRM_Levels->ul_CurrentLevel = 0xFFFFFFFF;

        pst_Object->p_MRM_Levels->a_ElementBase = pst_Object->dst_Element;
        pst_Object->p_MRM_Levels->l_TotalElementNb = 0;

        for (i=0; i<(int)pst_Object->p_MRM_Levels->ul_LevelNb ; i++)
            pst_Object->p_MRM_Levels->dl_ElementNb[i] = LOA_ReadInt(&pc_Buffer);

        for (i=0; i<(int)pst_Object->p_MRM_Levels->ul_LevelNb ; i++)
        {
            pst_Object->p_MRM_Levels->aa_Element[i] = pst_Object->p_MRM_Levels->a_ElementBase+pst_Object->p_MRM_Levels->l_TotalElementNb;
            pst_Object->p_MRM_Levels->l_TotalElementNb += pst_Object->p_MRM_Levels->dl_ElementNb[i];
        }

        ERR_X_Assert(pst_Object->p_MRM_Levels->l_TotalElementNb == pst_Object->l_NbElements);

        for (i=0; i<(int)pst_Object->p_MRM_Levels->ul_LevelNb-1 ; i++)
            pst_Object->p_MRM_Levels->f_Thresholds[i] = LOA_ReadFloat(&pc_Buffer);

        if (bHasIndirection)
        {
            pst_Object->p_MRM_Levels->dus_ReorderBuffer = (unsigned short *)MEM_p_Alloc(sizeof(unsigned short) * pst_Object->l_NbPoints);
            for (i=0; i<(int)pst_Object->l_NbPoints; i++)
                pst_Object->p_MRM_Levels->dus_ReorderBuffer[i] = LOA_ReadUShort(&pc_Buffer);
        }
        
        if (lVersionNb >= 3)
        {
        	pst_Object->p_MRM_Levels->l_TotalPointNb = LOA_ReadULong(&pc_Buffer);
	        for (i=0; i<(int)pst_Object->p_MRM_Levels->ul_LevelNb ; i++)
	        	pst_Object->p_MRM_Levels->dl_PointNb[i] = LOA_ReadULong(&pc_Buffer);
	        
        	pst_Object->p_MRM_Levels->l_TotalUVNb = LOA_ReadULong(&pc_Buffer);
	        for (i=0; i<(int)pst_Object->p_MRM_Levels->ul_LevelNb ; i++)
	        	pst_Object->p_MRM_Levels->dl_UVNb[i] = LOA_ReadULong(&pc_Buffer);
        
        }
    }

	/* ************ Sprite BEGIN ******************/
	pst_Object->l_NbSpritesElements = LOA_ReadLong(&pc_Buffer);
	if ((pst_Object->l_NbSpritesElements) && (!(pst_Object->l_NbSpritesElements & 0xffffff00)))
	{
		ULONG ElementCounter , Version;

		/* Load Version*/
		Version = LOA_ReadULong(&pc_Buffer);

		ElementCounter = pst_Object->l_NbSpritesElements;
		pst_Object->dst_SpritesElements = (GEO_tdst_ElementIndexedSprite *)MEM_GEO_p_Alloc(sizeof(GEO_tdst_ElementIndexedSprite) * ElementCounter);
		L_memset(pst_Object->dst_SpritesElements , 0 , sizeof(GEO_tdst_ElementIndexedSprite) * ElementCounter);

		while (ElementCounter--)
		{
			
			pst_Object->dst_SpritesElements[ElementCounter].fGlobalRatio = LOA_ReadFloat(&pc_Buffer);
			pst_Object->dst_SpritesElements[ElementCounter].fGlobalSize = LOA_ReadFloat(&pc_Buffer);
			pst_Object->dst_SpritesElements[ElementCounter].l_MaterialId = LOA_ReadLong(&pc_Buffer);
			pst_Object->dst_SpritesElements[ElementCounter].l_NbSprites = LOA_ReadLong(&pc_Buffer);

			// Skip the dummies
			LOA_ReadULong_Ed(&pc_Buffer, NULL);

			pst_Object->dst_SpritesElements[ElementCounter].dst_Sprite = NULL;
			if (pst_Object->dst_SpritesElements[ElementCounter].l_NbSprites)
			{
				GEO_tdst_IndexedSprite * p;
				pst_Object->dst_SpritesElements[ElementCounter].dst_Sprite = (GEO_tdst_IndexedSprite *)MEM_GEO_p_Alloc(sizeof(GEO_tdst_IndexedSprite) * pst_Object->dst_SpritesElements[ElementCounter].l_NbSprites);
				for(i = 0, p = pst_Object->dst_SpritesElements[ElementCounter].dst_Sprite; i < pst_Object->dst_SpritesElements[ElementCounter].l_NbSprites; ++i, ++p)
				{
					p->auw_Index = LOA_ReadUShort(&pc_Buffer);
					p->fSize = LOA_ReadUShort(&pc_Buffer);
				}
			}
		}
	} else pst_Object->l_NbSpritesElements = 0;
	/* ************ Sprite END ******************/

	*ppc_Buffer = pc_Buffer;
	
    if(GEO_b_IsInFix) GEO_ComputeNormals(pst_Object); // OK -> No recompute

#ifdef PSX2_TARGET
	GEO_Optimize_PS2(pst_Object);
#endif

#ifdef ACTIVE_EDITORS
    if(EDI_gb_ComputeMap && (WOR_gi_CurrentConsole==1)) GEO_RecordObject(pst_Object);
#endif

#ifdef NO_GC_FUR
	GEO_Optimize_GAMECUBE(pst_Object);
#endif

#ifdef _XBOX 
	{
		extern void GX8_ComputeTangentes(GEO_tdst_Object	*pst_Object);
		GX8_ComputeTangentes(pst_Object);
	}
#endif	
#if defined(_XENON_RENDER)

	// Create render elements
	// should be done at binarization then read directly

/*	pPoints      = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer(pst_Object->l_NbPoints, sizeof(GEO_Vertex));
	pPointsArray = (GEO_Vertex *)pPoints->Lock(pst_Object->l_NbPoints, sizeof(GEO_Vertex));
	memcpy(pPointsArray, pst_Object->dst_Point, pst_Object->l_NbPoints * sizeof(GEO_Vertex));
	pPoints->Unlock();

	if(pst_Object->dst_PointNormal)
	{
		pNormals      = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer(pst_Object->l_NbPoints, sizeof(GEO_Vertex));
		pNormalsArray = (GEO_Vertex *)pNormals->Lock(pst_Object->l_NbPoints, sizeof(GEO_Vertex));
		memcpy(pNormalsArray, pst_Object->dst_PointNormal, pst_Object->l_NbPoints * sizeof(GEO_Vertex));
		pPoints->Unlock();
	}
	else pNormals = NULL;

	pColors1      = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer(pst_Object->l_NbPoints, sizeof(ULONG));
	pColors1Array = (ULONG *)pColors1->Lock(pst_Object->l_NbPoints, sizeof(ULONG));
	memset(pColors1Array, 0xff, pst_Object->l_NbPoints * sizeof(ULONG));
	pColors1->Unlock();

	pUVs      = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer(pst_Object->l_NbPoints, sizeof(GEO_tdst_UV));
	pUVsArray = (GEO_tdst_UV *)pUVs->Lock(pst_Object->l_NbPoints, sizeof(GEO_tdst_UV));
	memset(pUVsArray, 0xff, pst_Object->l_NbPoints * sizeof(GEO_tdst_UV));
	pUVs->Unlock();

	pst_Element = pst_Object->dst_Element;
	pst_LastElement = pst_Element + pst_Object->l_NbElements;

	for(; pst_Element < pst_LastElement; pst_Element++)
	{
		if(pst_Element->pst_StripData || pst_Element->l_NbTriangles==0)
		{
			//ERR_OutputDebugStream("strips not supported")
			pst_Element->pMesh = NULL;
			continue;
		}

		pst_Element->pMesh = new XeMesh();
		pst_Element->pMesh->ClearAllStreams();

		pIndices      = (XeIndexBuffer*)g_XeBufferMgr.CreateIndexBuffer(pst_Element->l_NbTriangles*3);
		pIndicesArray = (USHORT*)pIndices->Lock(pst_Element->l_NbTriangles*3);
		GEO_tdst_IndexedTriangle* pTris;
		GEO_tdst_IndexedTriangle* pTrisEnd;
		pTris = pst_Element->dst_Triangle;
		pTrisEnd = pTris + pst_Element->l_NbTriangles;
		while (pTris < pTrisEnd)
		{
			for (int i = 0; i < 3; ++i)
			{
				*pIndicesArray++ = pTris->auw_Index[i];
			}

			pTris++;
		}

		pIndices->Unlock();
		pst_Element->pMesh->SetIndices(pIndices);

		pst_Element->pMesh->AddStream(XEVC_POSITION, pPoints);

		if(pNormals)
			pst_Element->pMesh->AddStream(XEVC_NORMAL, pNormals);

		pst_Element->pMesh->AddStream(XEVC_COLOR0, pColors1);

		pst_Element->pMesh->AddStream(XEVC_TEXCOORD0, pUVs);
	}

	/**/

#endif
#if (!defined NO_GC_FUR) && (defined _GAMECUBE)
	if (pst_Object->dst_Element)
		pst_Object->dst_Element->dl_size = 0xFFFFFFFF;
#endif 

	if (lVersionNb >= 2)
	{
	    ul_Temp = LOA_ReadLong(&pc_Buffer);
    	ERR_X_Assert(ul_Temp == 0xC0DE2009);
	}

	return pst_Object;
}


void GEO_v_DuplicateElements(ULONG l_NbElements, GEO_tdst_ElementIndexedTriangles *pst_NewElements, GEO_tdst_ElementIndexedTriangles *pst_OldElements)
{
    LONG								l_Size;
    GEO_tdst_ElementIndexedTriangles	*pst_NewElem, *pst_Elem, *pst_Last;
    pst_NewElem = pst_NewElements;

    pst_Elem = pst_OldElements;
    pst_Last = pst_Elem + l_NbElements;
    for(; pst_Elem < pst_Last; pst_Elem++, pst_NewElem++)
    {
        pst_NewElem->l_MaterialId = pst_Elem->l_MaterialId;
        pst_NewElem->l_NbTriangles = pst_Elem->l_NbTriangles;
        pst_NewElem->p_MrmElementAdditionalInfo = pst_Elem->p_MrmElementAdditionalInfo;
        pst_NewElem->ul_NumberOfUsedIndex = pst_Elem->ul_NumberOfUsedIndex;

        GEO_AllocElementContent(pst_NewElem);

        if(pst_Elem->p_MrmElementAdditionalInfo)
        {
            l_Size = sizeof(GEO_tdst_MRM_Element);
            L_memcpy(pst_NewElem->p_MrmElementAdditionalInfo, pst_Elem->p_MrmElementAdditionalInfo, l_Size);
        }

        if(pst_Elem->ul_NumberOfUsedIndex)
        {
            l_Size = pst_NewElem->ul_NumberOfUsedIndex * sizeof(unsigned short) + (LONG) 2;
            L_memcpy(pst_NewElem->pus_ListOfUsedIndex, pst_Elem->pus_ListOfUsedIndex, l_Size);
        }

        l_Size = pst_NewElem->l_NbTriangles * sizeof(GEO_tdst_IndexedTriangle);
        if(l_Size) L_memcpy(pst_NewElem->dst_Triangle, pst_Elem->dst_Triangle, l_Size);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GEO_p_Duplicate(GEO_tdst_Object *_pst_Geo, char *_asz_Path, char *_sz_Name, ULONG _ul_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	GEO_tdst_Object						*pst_NewGeo;
	LONG								l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // Elements should have full detail.
	GEO_MRM_RESET(_pst_Geo);


	pst_NewGeo = GEO_pst_Create
		(
			_pst_Geo->l_NbPoints,
			_pst_Geo->l_NbUVs,
			_pst_Geo->l_NbElements,
			_pst_Geo->dul_PointColors ? _pst_Geo->dul_PointColors[0] : 0
		);

	l_Size = _pst_Geo->l_NbPoints * sizeof(GEO_Vertex);
	if(l_Size)
	{
		L_memcpy(pst_NewGeo->dst_Point, _pst_Geo->dst_Point, l_Size);
		l_Size = _pst_Geo->l_NbPoints * sizeof(MATH_tdst_Vector);
		if(pst_NewGeo->dst_PointNormal && _pst_Geo->dst_PointNormal)
			L_memcpy(pst_NewGeo->dst_PointNormal, _pst_Geo->dst_PointNormal, l_Size);
	}

	l_Size = _pst_Geo->l_NbUVs * sizeof(GEO_tdst_UV);
	if(l_Size) L_memcpy(pst_NewGeo->dst_UV, _pst_Geo->dst_UV, l_Size);

	if(_pst_Geo->dul_PointColors)
	{
		l_Size = _pst_Geo->dul_PointColors[0] * sizeof(ULONG);
		if(l_Size) L_memcpy(pst_NewGeo->dul_PointColors + 1, _pst_Geo->dul_PointColors + 1, l_Size);
	}

	/* Duplicate MRM */
	if(_pst_Geo->p_MRM_ObjectAdditionalInfo)
	{
		pst_NewGeo->p_MRM_ObjectAdditionalInfo = (GEO_tdst_MRM_Object *) MEM_GEO_p_Alloc(sizeof(GEO_tdst_MRM_Object));
		*pst_NewGeo->p_MRM_ObjectAdditionalInfo = *_pst_Geo->p_MRM_ObjectAdditionalInfo;
		pst_NewGeo->p_MRM_ObjectAdditionalInfo->Absorbers = (short *) MEM_GEO_p_Alloc(sizeof(unsigned short) * _pst_Geo->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints);
		l_Size = pst_NewGeo->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints * sizeof(unsigned short);
		if(l_Size)
		{
			L_memcpy
			(
				pst_NewGeo->p_MRM_ObjectAdditionalInfo->Absorbers,
				_pst_Geo->p_MRM_ObjectAdditionalInfo->Absorbers,
				l_Size
			);
			if(_pst_Geo->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer)
			{
				pst_NewGeo->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer = (short *) MEM_GEO_p_Alloc(sizeof(unsigned short) * _pst_Geo->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints);
				L_memcpy
				(
					pst_NewGeo->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer,
					_pst_Geo->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer,
					l_Size
				);
			}
			else
				pst_NewGeo->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer = NULL;
		}
	}

	/* Duplicate skin */
	if(GEO_SKN_IsSkinned(_pst_Geo))
		pst_NewGeo->p_SKN_Objectponderation = GEO_SKN_Duplicate(_pst_Geo->p_SKN_Objectponderation);
	else
		pst_NewGeo->p_SKN_Objectponderation = NULL;

    // Duplicate elements in geometry
    GEO_v_DuplicateElements(_pst_Geo->l_NbElements, pst_NewGeo->dst_Element, _pst_Geo->dst_Element);

    // Duplicate MRM levels (elements and all).
    if (_pst_Geo->p_MRM_Levels)
    {
        int i ;

        pst_NewGeo->p_MRM_Levels = GEO_MRM_CreateLevels();

        pst_NewGeo->p_MRM_Levels->f_DistanceCoef = _pst_Geo->p_MRM_Levels->f_DistanceCoef;
        pst_NewGeo->p_MRM_Levels->ul_LevelNb = _pst_Geo->p_MRM_Levels->ul_LevelNb;
        pst_NewGeo->p_MRM_Levels->ul_CurrentLevel = _pst_Geo->p_MRM_Levels->ul_CurrentLevel;
        pst_NewGeo->p_MRM_Levels->a_ElementBase = pst_NewGeo->dst_Element;
        pst_NewGeo->p_MRM_Levels->l_TotalElementNb = 0;
        for (i=0; i<(int)pst_NewGeo->p_MRM_Levels->ul_LevelNb; i++)
        {
        	pst_NewGeo->p_MRM_Levels->aa_Element[i] = pst_NewGeo->p_MRM_Levels->a_ElementBase+pst_NewGeo->p_MRM_Levels->l_TotalElementNb;
            pst_NewGeo->p_MRM_Levels->dl_ElementNb[i] = _pst_Geo->p_MRM_Levels->dl_ElementNb[i];
            pst_NewGeo->p_MRM_Levels->l_TotalElementNb += pst_NewGeo->p_MRM_Levels->dl_ElementNb[i];
        }

        ERR_X_Assert(pst_NewGeo->p_MRM_Levels->l_TotalElementNb == _pst_Geo->p_MRM_Levels->l_TotalElementNb);

        for (i=0; i<(int)pst_NewGeo->p_MRM_Levels->ul_LevelNb-1; i++)
            pst_NewGeo->p_MRM_Levels->f_Thresholds[i] = _pst_Geo->p_MRM_Levels->f_Thresholds[i];

        if (_pst_Geo->p_MRM_Levels->dus_ReorderBuffer)
        {
            pst_NewGeo->p_MRM_Levels->dus_ReorderBuffer = (unsigned short *)MEM_p_Alloc(sizeof(unsigned short) * _pst_Geo->l_NbPoints);
            for (i=0; i<_pst_Geo->l_NbPoints; i++)
                pst_NewGeo->p_MRM_Levels->dus_ReorderBuffer[i] = _pst_Geo->p_MRM_Levels->dus_ReorderBuffer[i];
        }
        
    	pst_NewGeo->p_MRM_Levels->l_TotalPointNb = _pst_Geo->p_MRM_Levels->l_TotalPointNb;
        for (i=0; i<(int)_pst_Geo->p_MRM_Levels->ul_LevelNb ; i++)
        	pst_NewGeo->p_MRM_Levels->dl_PointNb[i] = _pst_Geo->p_MRM_Levels->dl_PointNb[i];
        
    	pst_NewGeo->p_MRM_Levels->l_TotalUVNb = _pst_Geo->p_MRM_Levels->l_TotalUVNb;
        for (i=0; i<(int)_pst_Geo->p_MRM_Levels->ul_LevelNb ; i++)
        	pst_NewGeo->p_MRM_Levels->dl_UVNb[i] = _pst_Geo->p_MRM_Levels->dl_UVNb[i];
        
    }

	if (_pst_Geo->l_NbSpritesElements)
	{
		ULONG ElementCounter;
		ElementCounter = pst_NewGeo->l_NbSpritesElements = _pst_Geo->l_NbSpritesElements;
		pst_NewGeo->dst_SpritesElements = (GEO_tdst_ElementIndexedSprite *)MEM_GEO_p_Alloc(sizeof(GEO_tdst_ElementIndexedSprite) * ElementCounter);
		L_memcpy(pst_NewGeo->dst_SpritesElements , _pst_Geo->dst_SpritesElements , sizeof(GEO_tdst_ElementIndexedSprite) * ElementCounter);

		while (ElementCounter--)
		{
			if (pst_NewGeo->dst_SpritesElements[ElementCounter].l_NbSprites)
			{
				pst_NewGeo->dst_SpritesElements[ElementCounter].dst_Sprite = (GEO_tdst_IndexedSprite*)MEM_GEO_p_Alloc(sizeof(GEO_tdst_IndexedSprite) * _pst_Geo->dst_SpritesElements[ElementCounter].l_NbSprites);
				L_memcpy(	pst_NewGeo->dst_SpritesElements[ElementCounter].dst_Sprite , 
							_pst_Geo->   dst_SpritesElements[ElementCounter].dst_Sprite , 
							sizeof(GEO_tdst_IndexedSprite) * _pst_Geo->dst_SpritesElements[ElementCounter].l_NbSprites);
			}
		}
	}


	if(_asz_Path) GRO_ul_Struct_FullSave(&pst_NewGeo->st_Id, _asz_Path, NULL, NULL);

	return pst_NewGeo;
#else
	return NULL;
#endif
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAD_GeometricObject *GEO_p_ToMad(GEO_tdst_Object *_pst_Object, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_GeometricObject					*pst_MadObj;
	MAD_Simple3DVertex					*pst_MadVertex, *pst_LastMadVertex;
	MATH_tdst_Vector					*pst_Normal;
	GEO_Vertex							*pst_Point;
	MAD_Vertex							*pst_MadUV, *pst_LastMadUV;
	GEO_tdst_UV							*pst_UV;
	MAD_GeometricObjectElement			*pst_MadElement;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Face;
	MAD_Face							*pst_MadFace, *pst_LastMadFace;
	LONG								l_Index;
	MAD_ColorARGB						*pst_MadRLI;
	ULONG								*pul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GEO_MRM_RESET(_pst_Object);
	if(GEO_MRM_ul_IsMrmObject(_pst_Object)) GEO_MRM_RemapUV(_pst_Object);

	MAD_MALLOC(MAD_GeometricObject, pst_MadObj, 1);

	pst_MadObj->ID.IDType = ID_MAD_GeometricObject_V0;
	pst_MadObj->ID.SizeOfThisOne = sizeof(MAD_GeometricObject);
	strcpy(pst_MadObj->ID.Name, GRO_sz_Struct_GetName(&_pst_Object->st_Id));

	pst_MadObj->TypeOfThis = MAD_MAXMESH_V0;

	/* Convert list of points */
	pst_MadObj->NumberOfPoints = _pst_Object->l_NbPoints;
	if(pst_MadObj->NumberOfPoints)
	{
		MAD_MALLOC(MAD_Simple3DVertex, pst_MadObj->OBJ_PointList, pst_MadObj->NumberOfPoints);

		pst_MadVertex = pst_MadObj->OBJ_PointList;
		pst_LastMadVertex = pst_MadVertex + pst_MadObj->NumberOfPoints;
		pst_Point = _pst_Object->dst_Point;
		pst_Normal = _pst_Object->dst_PointNormal;

		pul_Color = NULL;
		if(_pst_Object->dul_PointColors)
		{
			MAD_MALLOC(MAD_ColorARGB, pst_MadObj->Colors, pst_MadObj->NumberOfPoints);
			pul_Color = _pst_Object->dul_PointColors + 1;
			pst_MadRLI = pst_MadObj->Colors;
		}

		for(; pst_MadVertex < pst_LastMadVertex; pst_MadVertex++, pst_Point++, pst_Normal++)
		{
			MATH_CopyVector((MATH_tdst_Vector *) &pst_MadVertex->Point, VCast(pst_Point));
			MATH_CopyVector((MATH_tdst_Vector *) &pst_MadVertex->Normale, pst_Normal);
			if(_pst_Object->dul_PointColors)
			{
				*pst_MadRLI = *pul_Color;
				pst_MadRLI++;
				pul_Color++;
			}
		}
	}

	/* Convert list of uv */
	pst_MadObj->NumberOfUV = _pst_Object->l_NbUVs;
	if(pst_MadObj->NumberOfUV)
	{
		MAD_MALLOC(MAD_Vertex, pst_MadObj->OBJ_UVtextureList, pst_MadObj->NumberOfUV);

		pst_MadUV = pst_MadObj->OBJ_UVtextureList;
		pst_LastMadUV = pst_MadUV + pst_MadObj->NumberOfUV;
		pst_UV = _pst_Object->dst_UV;

		for(; pst_MadUV < pst_LastMadUV; pst_MadUV++, pst_UV++)
		{
			pst_MadUV->x = pst_UV->fU;
			pst_MadUV->y = pst_UV->fV;
			pst_MadUV->z = 0;
		}
	}

	/* Convert element */
	pst_MadObj->NumberOfElements = _pst_Object->l_NbElements;
	if(pst_MadObj->NumberOfElements)
	{
		MAD_MALLOC(MAD_GeometricObjectElement *, pst_MadObj->Elements, _pst_Object->l_NbElements);

		for(l_Index = 0; l_Index < _pst_Object->l_NbElements; l_Index++)
		{
			MAD_MALLOC(MAD_GeometricObjectElement, pst_MadElement, 1);
			pst_MadObj->Elements[l_Index] = pst_MadElement;

			pst_Element = &_pst_Object->dst_Element[l_Index];

			pst_MadElement->MaterialID = pst_Element->l_MaterialId;
			pst_MadElement->NumberOfTriangles = pst_Element->l_NbTriangles;

			/* Convert list of faces */
			if(pst_MadElement->NumberOfTriangles)
			{
				MAD_MALLOC(MAD_Face, pst_MadElement->Faces, pst_MadElement->NumberOfTriangles);

				pst_MadFace = pst_MadElement->Faces;
				pst_LastMadFace = pst_MadFace + pst_MadElement->NumberOfTriangles;
				pst_Face = pst_Element->dst_Triangle;

				for(; pst_MadFace < pst_LastMadFace; pst_MadFace++, pst_Face++)
				{
                    if ( (pst_Face->ul_MaxFlags != 0x53) && (pst_Face->ul_MaxFlags != 0x56) )
                        pst_Face->ul_MaxFlags = 0x53;

					pst_MadFace->Index[0] = pst_Face->auw_Index[0];
					pst_MadFace->Index[1] = pst_Face->auw_Index[1];
					pst_MadFace->Index[2] = pst_Face->auw_Index[2];

					pst_MadFace->UVIndex[0] = pst_Face->auw_UV[0];
					pst_MadFace->UVIndex[1] = pst_Face->auw_UV[1];
					pst_MadFace->UVIndex[2] = pst_Face->auw_UV[2];

					pst_MadFace->SmoothingGroup = 1 /* pst_Face->ul_SmoothingGroup */ ;
					pst_MadFace->MAXflags = (pst_Face->ul_MaxFlags & 0xFFFF) | (pst_MadElement->MaterialID << 16);
				}
			}
		}
	}

	return pst_MadObj;
}

#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_HasSomethingToRender(GRO_tdst_Visu *_pst_Visu, GEO_tdst_Object **ppst_PickableObject)
{
	*ppst_PickableObject = (GEO_tdst_Object *) _pst_Visu->pst_Object;
	return(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_Draw);
}

#ifdef ACTIVE_EDITORS

BOOL COL_gb_OnMaterialPaint = FALSE;
BOOL COL_gb_SoundColorDisplay = FALSE;

void MAT_DrawIndexedTriangle_ColMap
(
	GDI_tdst_DisplayData				*,
	GEO_tdst_Object						*,
	MAT_tdst_Material					*,
	GEO_tdst_ElementIndexedTriangles	*,
	MATH_tdst_Matrix					*
);

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void GEO_4Edit_ColMapOnly_BeforeDisplay
(
	GDI_tdst_DisplayData	*pst_DD, 
	OBJ_tdst_GameObject		*_pst_GO, 
	COL_tdst_Cob			**ppst_Cob, 
	BOOL					*_pb_SelectedCob
)
{
	*_pb_SelectedCob = FALSE;
	*ppst_Cob = NULL;

	// We are in ColMap Display mode ...
	if(pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCurrentCOB)
		*ppst_Cob = *((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob;
	if
	(
		(pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCurrentCOB)
	&&	(pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired)
	&&	(COL_gb_OnMaterialPaint || !(pst_DD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_SubObject))
	&&	SEL_RetrieveItem
		(
			pst_DD->pst_World->pst_Selection,
			*(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob)
		)
	)
	{
		*_pb_SelectedCob = TRUE;
	}
}

void GEO_SetVertexColor(LONG l_NbPoints,ULONG ulColor)
{
    int  i;
    for (i=0; i<l_NbPoints; i++)
    {
        GDI_gpst_CurDD_SPR.pst_ComputingBuffers->aul_Diffuse[i] = ulColor;
        GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ComputedColors[i] = ulColor;
        GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentColorField[i] = ulColor;
    }
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL GEO_4Edit_ColMapOnly_ComputeElementColor
(
	GDI_tdst_DisplayData 				*pst_DD,
	OBJ_tdst_GameObject					*_pst_GO,
	COL_tdst_Cob						*pst_Cob, 
	GEO_tdst_Object						*pst_Obj, 
	GEO_tdst_ElementIndexedTriangles	*pst_Element, 
	BOOL								b_SelectedCob
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_GameMat					*pst_GMat;
    BOOL                        bComputeVertexColor = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	pst_GMat = NULL;
	if(pst_Cob)
	{
		if(pst_Cob->pst_TriangleCob)
			pst_GMat = COL_pst_GMat_Get(pst_Cob, pst_Cob->pst_TriangleCob->dst_Element + (pst_Element - pst_Obj->dst_Element));
		else
			pst_GMat = COL_pst_GMat_Get(pst_Cob, NULL);
	}



	if(pst_Cob && pst_Cob->pst_TriangleCob)
	{	
		if(pst_GMat && !(pst_GMat->b_Display))
			return TRUE;

		if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera) && ((pst_DD->uc_ColMapDisplayMode & 0x7F) == 2))
			return TRUE;
		
		if(((pst_DD->uc_ColMapDisplayMode & 0x7F) == 1) && ((!pst_GMat) || (pst_GMat && !(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera))))
			return TRUE;
		
		if(pst_GMat && ((pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX) || (pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)) && ((pst_DD->uc_ColMapDisplayMode & 0x7F) == 3))
			return TRUE;
		
	}
	if(b_SelectedCob && ((pst_Obj->dst_Element + pst_Cob->ul_EditedElement) == pst_Element))
	{
		if(COL_gb_OnMaterialPaint && COL_gb_SoundColorDisplay)
		{
			if(pst_GMat)
				MAT_gst_DefaultSingleMaterial.ul_Diffuse = pst_GMat->ul_SoundColor;
			else
				MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFFFFFFFF;
#ifdef JADEFUSION
			LIGHT_ComputeVertexColor(pst_Obj->l_NbPoints);//popowarning a mettre ??
#endif
		}
		else
		{
			if(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->uc_Activation & 1)
				MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0x000000FF;
			else
				MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0x00333333;
		}
        bComputeVertexColor = TRUE;
	}
	else
	{
		if(pst_Cob)
		{
			if(COL_gb_SoundColorDisplay)
			{
				if(pst_GMat)
					MAT_gst_DefaultSingleMaterial.ul_Diffuse = pst_GMat->ul_SoundColor;
				else
					MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFFFFFFFF;

                bComputeVertexColor = TRUE;
			}
			else
			{
				if(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->uc_Activation & 1)
				{
					if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX)) 
					{
						MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0x00FF6666;
					}
					else
					{
						if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)) 
							MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0x0000FFFF;
						else
						{
							if(pst_GMat)
								MAT_gst_DefaultSingleMaterial.ul_Diffuse = pst_GMat->ul_Color;
							else
								MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFFFFFFFF;
						}
					}
                    bComputeVertexColor = TRUE;
				}
			}
			
		}
		else
		{
			MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0x00000000;
            bComputeVertexColor = TRUE;
		}
 	}

    if (pst_GMat && pst_GMat->b_Transparent)
    {
        MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0x40000000 | (MAT_gst_DefaultSingleMaterial.ul_Diffuse & 0x00FFFFFF);
        GEO_SetVertexColor(pst_Obj->l_NbPoints,MAT_gst_DefaultSingleMaterial.ul_Diffuse);
//    MAT_gst_DefaultSingleMaterial.ul_Ambiant = MAT_gst_DefaultSingleMaterial.ul_Diffuse;
    }
    else if (bComputeVertexColor)
		LIGHT_ComputeVertexColor(pst_Obj->l_NbPoints);

	return FALSE;
 }

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL GEO_ColMap_VerticeVisible(OBJ_tdst_GameObject *_pst_GO, USHORT uw_Vertice)
{
	COL_tdst_Cob						*pst_Cob;
	COL_tdst_IndexedTriangles			*pst_TriangleCob;
	COL_tdst_ElementIndexedTriangles	*pst_Element;
	COL_tdst_IndexedTriangle			*pst_Triangle;
	COL_tdst_GameMat					*pst_GMat;
	int									i, j;


	if(!_pst_GO || !(GDI_gpst_CurDD_SPR.ul_DisplayFlags & GDI_Cul_DF_ShowCOB)  || ((GDI_gpst_CurDD_SPR.uc_ColMapDisplayMode & 0x7F) == 0) || ((GDI_gpst_CurDD_SPR.uc_ColMapDisplayMode & 0x7F) == 4)) return TRUE;

	/* Subobject is here : We are editing a visual and not a cob */
	if(SEL_RetrieveItem(_pst_GO->pst_World->pst_Selection, _pst_GO)) return TRUE;

	pst_Cob = *(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob);
	pst_TriangleCob = pst_Cob->pst_TriangleCob;
	if(!pst_TriangleCob) return FALSE;
	
	for(i = 0; i < (int) pst_TriangleCob->l_NbElements; i++)
	{
		pst_Element = &pst_TriangleCob->dst_Element[i];
		pst_GMat = COL_pst_GMat_Get(pst_Cob, pst_Element);
		
		
		for(j = 0; j < pst_Element->uw_NbTriangles; j++)
		{
			pst_Triangle = &pst_Element->dst_Triangle[j];
			
			if((pst_Triangle->auw_Index[0] == uw_Vertice) || (pst_Triangle->auw_Index[1] == uw_Vertice) || (pst_Triangle->auw_Index[2] == uw_Vertice))
			{
				if(pst_GMat && !pst_GMat->b_Display)
					return FALSE;

				if((GDI_gpst_CurDD_SPR.uc_ColMapDisplayMode & 0x7F) == 1)
				{
					if(!pst_GMat) 
						return FALSE;
					if(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)
						return TRUE;
				}
				
				if((GDI_gpst_CurDD_SPR.uc_ColMapDisplayMode & 0x7F) == 2)
				{
					if(!pst_GMat || !(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)) 
						return TRUE;
				}
				
				if((GDI_gpst_CurDD_SPR.uc_ColMapDisplayMode & 0x7F) == 3)
				{
					if(!pst_GMat || !(pst_GMat->ul_CustomBits & (COL_Cul_GMat_FlagX | COL_Cul_GMat_Camera))) 
						return TRUE;
				}

			}
		}
	}

	return FALSE;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL GEO_ColMap_ElementVisible(OBJ_tdst_GameObject *_pst_GO, USHORT _uw_Element)
{
	COL_tdst_Cob						*pst_Cob;
	COL_tdst_IndexedTriangles			*pst_TriangleCob;
	COL_tdst_ElementIndexedTriangles	*pst_Element;
	COL_tdst_GameMat					*pst_GMat;

	
	if(!_pst_GO || !(GDI_gpst_CurDD_SPR.ul_DisplayFlags & GDI_Cul_DF_ShowCOB)  || ((GDI_gpst_CurDD_SPR.uc_ColMapDisplayMode & 0x7F) == 0) || ((GDI_gpst_CurDD_SPR.uc_ColMapDisplayMode & 0x7F) == 4)) return TRUE;
	
	/* Subobject is here : We are editing a visual and not a cob */
	if(SEL_RetrieveItem(_pst_GO->pst_World->pst_Selection, _pst_GO)) return TRUE;

	pst_Cob = *(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob);
	pst_TriangleCob = pst_Cob->pst_TriangleCob;
	
	pst_Element = &pst_TriangleCob->dst_Element[_uw_Element];
	pst_GMat = COL_pst_GMat_Get(pst_Cob, pst_Element);

	if(pst_GMat && !pst_GMat->b_Display) return FALSE;
		
	if((GDI_gpst_CurDD_SPR.uc_ColMapDisplayMode & 0x7F) == 1)
	{
		if(!pst_GMat)
			return FALSE;
		if(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)
			return TRUE;
	}
	
	if((GDI_gpst_CurDD_SPR.uc_ColMapDisplayMode & 0x7F) == 2)
	{
		if(!pst_GMat || !(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)) 
			return TRUE;
	}					

	if((GDI_gpst_CurDD_SPR.uc_ColMapDisplayMode & 0x7F) == 3)
	{
		if(!pst_GMat || !(pst_GMat->ul_CustomBits & (COL_Cul_GMat_FlagX | COL_Cul_GMat_Camera))) 
			return TRUE;
	}					

	
	return FALSE;
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Render_SubObject(OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex						*pst_Vertex, *pst_Last, *pst_V;
	MATH_tdst_Vector				*pst_Normal, v, *v0, *v1, *v2, *v2D;
	char							*pc_ESel, *pc_Sel, i, c_ColorOffset;
	GEO_tdst_SubObject_Edge			*pst_Edge, *pst_LastEdge;
	GEO_tdst_SubObject_Face			*pst_Face, *pst_LastFace;
	MATH_tdst_Vector				*pst_2D, *pst_3D;
	GDI_tdst_Request_DrawLineEx		st_Line;
	GDI_tdst_Request_DrawPointEx	st_Point, st_UV;
	GEO_tdst_IndexedTriangle		*pst_Triangle;
	GEO_tdst_IndexedTriangle		*pst_Triangle1, *pst_Triangle2;
	USHORT							uw_Vertice;
    int                             i_MorphVector;
	COL_tdst_Cob					*pst_Cob;
	BOOL							b_SelectedCob;
	BOOL							b_ShowSub;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(GDI_gpst_CurDD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_HideSubObject)
		return;

	if
	(
		!pst_Obj->pst_SubObject
	||	(
			!(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) 
			&& !
				(
					(GDI_gpst_CurDD_SPR.ul_DisplayFlags & GDI_Cul_DF_ShowCurrentCOB) &&
					SEL_RetrieveItem
					(
						GDI_gpst_CurDD_SPR.pst_World->pst_Selection,
						*(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob)
					)
				)
		)
	) return;
	
#ifdef ACTIVE_EDITORS
	{
		extern ULONG SmoothSelMustBeUpdate;
		extern void GEO_ComputeFloatingSelection(GEO_tdst_Object *_pst_Obj);
		if (SmoothSelMustBeUpdate)
			GEO_ComputeFloatingSelection(pst_Obj);
	}

	GEO_4Edit_ColMapOnly_BeforeDisplay(&GDI_gpst_CurDD_SPR, _pst_GO, &pst_Cob, &b_SelectedCob);
#endif

#if defined(ACTIVE_EDITORS) && defined(_XENON_RENDER)
    if (GDI_b_IsXenonGraphics() && GEO_b_IsSkinned(_pst_GO, pst_Obj) && 
        (GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_ShowNormals))
    {
        GRO_tdst_Visu * pst_Visu = _pst_GO->pst_Base->pst_Visu;
        if( pst_Visu != NULL )
        {
            if (!Xe_IsForcingSWSkinning())
            {
                ULONG ulDisplayFlagsBK = GDI_gpst_CurDD->ul_DisplayFlags;

                // The GDI_Cul_DF_UsePickingBuffer will force software skinning...
                GDI_gpst_CurDD->ul_DisplayFlags |= GDI_Cul_DF_UsePickingBuffer;

                // Vertex, normals and tangents were not skinned
                GEO_SKN_Compute4Display(_pst_GO, pst_Obj);

                // Compute the normals
                GEO_SKN_ComputeNormals(pst_Obj, GDI_gpst_CurDD->p_Current_Vertex_List);

                // Compute the tangents
                GEO_SKN_ComputeTangents(pst_Obj, GDI_gpst_CurDD->p_Current_Vertex_List, pst_Visu );

                // Back to previous display flags
                GDI_gpst_CurDD->ul_DisplayFlags = ulDisplayFlagsBK;
            }
            else
            {
                // Tangents are never skinned automatically
                GEO_SKN_ComputeTangents(pst_Obj, GDI_gpst_CurDD->p_Current_Vertex_List, pst_Visu );
            }
        }
    }
#endif

    if ( SEL_pst_GetFirstItem( GDI_gpst_CurDD->pst_World->pst_Selection, SEL_C_SIF_Object) == _pst_GO )
        c_ColorOffset = 0;
    else
        c_ColorOffset = 4;

    if ( (pst_Obj->pst_SubObject->pfnl_UVMapper_Request) && (pst_Obj->pst_SubObject->pfnl_UVMapper_Request( pst_Obj->pst_SubObject->ul_UVMapper_Param, 1) ) )
    {
		// hide BV
		OBJ_M_EdAddFlags_Set( _pst_GO, OBJ_C_EdAddFlags_HideBV );
		// compute some data
        if(pst_Obj->pst_SubObject->pc_UVMapper_Sel)
		{
    		pst_Face = pst_Obj->pst_SubObject->dst_Faces;
	    	pst_LastFace = pst_Face + pst_Obj->pst_SubObject->ul_NbFaces;
            pst_Vertex = GDI_gpst_CurDD->p_Current_Vertex_List;
		    pc_Sel = pst_Obj->pst_SubObject->dc_FSel;
			st_UV.f_Size = GDI_gpst_CurDD->pst_EditOptions->f_VertexSize;
			st_UV.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_VertexColor[c_ColorOffset + 1];

			for(; pst_Face < pst_LastFace; pst_Face++, *pc_Sel++)
			{
                if (*pc_Sel & 0x80) continue;
				if (!(*pc_Sel & 1)) continue;

                pst_Triangle = pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
				for(i = 0; i < 3; i++)
				{
				    if( pst_Obj->pst_SubObject->pc_UVMapper_Sel[pst_Triangle->auw_UV[i] * pst_Obj->pst_SubObject->c_UVMapper_SelInc] )
                    {
						st_UV.A = VCast(pst_Vertex + pst_Triangle->auw_Index[i]);
						GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointEx, (ULONG) & st_UV);
					}
                }
			}
		}
        return;
    }
    
    b_ShowSub = true;
    if ( pst_Obj->pst_SubObject->pst_MorphData && (GDI_gpst_CurDD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_HideSubObject4Morph) )
		b_ShowSub = false;
    
    
    // turn off fog
    GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_EnableFog, FALSE);

	pst_Vertex = GDI_gpst_CurDD->p_Current_Vertex_List;
	pst_Last = pst_Vertex + pst_Obj->l_NbPoints;

	/* compute pickable data */
	if(GDI_gpst_CurDD->pst_EditOptions->ul_Flags & (GRO_Cul_EOF_Vertex | GRO_Cul_EOF_Edge))
	{
		SOFT_TransformAndProjectIn2Buffers
		(
			pst_Obj->pst_SubObject->dst_3D,
			pst_Obj->pst_SubObject->dst_2D,
			pst_Vertex,
			pst_Obj->l_NbPoints,
			&GDI_gpst_CurDD->st_Camera
		);
	}

    /* store transformed points */
    if (pst_Vertex != pst_Obj->dst_Point )
    {
        if (!pst_Obj->pst_SubObject->dst_PointTransformed)
            pst_Obj->pst_SubObject->dst_PointTransformed = (GEO_Vertex *) L_malloc( pst_Obj->l_NbPoints * sizeof( GEO_Vertex ) );
        L_memcpy( pst_Obj->pst_SubObject->dst_PointTransformed, pst_Vertex, pst_Obj->l_NbPoints * sizeof( GEO_Vertex ) );
    }
    else if (pst_Obj->pst_SubObject->dst_PointTransformed)
    {
        L_free( pst_Obj->pst_SubObject->dst_PointTransformed);
        pst_Obj->pst_SubObject->dst_PointTransformed = NULL;
    }

	/* display vertices */
	if( ( GDI_gpst_CurDD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_Vertex ) && b_ShowSub )
	{
		float *pFloatingSel;
		st_Point.f_Size = GDI_gpst_CurDD->pst_EditOptions->f_VertexSize;
		pc_Sel = pst_Obj->pst_SubObject->dc_VSel;
		pFloatingSel  = pst_Obj->pst_SubObject->pf_FloatSelection_V;
		pst_V = pst_Vertex;
		for(uw_Vertice = 0; pst_V < pst_Last; uw_Vertice ++, pst_V++)
		{
			if (pFloatingSel) pFloatingSel++;
			if((GDI_gpst_CurDD_SPR.ul_DisplayFlags & GDI_Cul_DF_ShowCurrentCOB) && !(GEO_ColMap_VerticeVisible(_pst_GO, uw_Vertice))) 
			{
				pc_Sel++;
				continue;
			}

			if(pst_Obj->pst_SubObject->dc_VSel[uw_Vertice] & 0x80)
			{
				pc_Sel++;
				continue;
			}

			st_Point.A = VCast(pst_V);
			if (pFloatingSel) 
			{
				st_Point.ul_Color = 
				LIGHT_ul_Interpol2Colors(GDI_gpst_CurDD->pst_EditOptions->ul_VertexColor[c_ColorOffset ],
										 GDI_gpst_CurDD->pst_EditOptions->ul_VertexColor[c_ColorOffset + 1],
										 *(pFloatingSel - 1));
			} else
				st_Point.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_VertexColor[c_ColorOffset + (*pc_Sel++) ];

			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointEx, (ULONG) & st_Point);
		}
	}

	/* display normals */
	if((GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_ShowNormals) && pst_Obj->dst_PointNormal && b_ShowSub )
	{
		st_Line.f_Width = GDI_gpst_CurDD->pst_EditOptions->f_NormalSize;
		st_Line.ul_Flags = 0;
		pst_V = pst_Vertex;
		st_Line.B = &v;

		GEO_UseNormals( pst_Obj );
		pst_Normal = pst_Obj->dst_PointNormal;
		pc_Sel = pst_Obj->pst_SubObject->dc_VSel;

#ifdef JADEFUSION
        GRO_tdst_Visu * pst_Visu = _pst_GO->pst_Base->pst_Visu;
#endif		
#if defined(ACTIVE_EDITORS) && defined(_XENON_RENDER)
        if (pst_Visu && pst_Visu->p_XeElements && pst_Visu->p_XeElements[0].dst_TangentSpace != NULL)
        {
            const FLOAT fScale        = GDI_gpst_CurDD->pst_EditOptions->f_ScaleForNormals;
            const bool  bOnlySelected = (GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_ShowNormalsOfSelected) != 0;

            for (int iElement = 0; iElement < pst_Obj->l_NbElements; ++iElement)
            {
                GEO_tdst_TextureSpaceBasis* pst_TangentBasis = NULL;

                for(int iTriangle = 0;  iTriangle < pst_Obj->dst_Element[iElement].l_NbTriangles; ++iTriangle)
                {
                    const GEO_tdst_IndexedTriangle* pTriangle = &pst_Obj->dst_Element[iElement].dst_Triangle[iTriangle];

                    for (int iVtx = 0; iVtx < 3; ++iVtx)
                    {
                        const USHORT wCurVertex = pTriangle->auw_Index[iVtx];

                        pst_V            = &pst_Obj->dst_Point[wCurVertex];
                        pst_TangentBasis = &pst_Visu->p_XeElements[iElement].dst_TangentSpace[wCurVertex];

                        if(bOnlySelected && !(pc_Sel[wCurVertex] & 1)) continue;

                        st_Line.A = VCast(pst_V);

                        // Tangent (Red)
                        st_Line.ul_Color = (pc_Sel[wCurVertex] & 1) ? 0xff0000ff : 0xff000080;
                        MATH_AddScaleVector(st_Line.B, st_Line.A, &pst_TangentBasis->S, fScale);
                        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&st_Line);

                        // Binormal (Green)
                        st_Line.ul_Color = (pc_Sel[wCurVertex] & 1) ? 0xff00ff00 : 0xff008000;
                        MATH_AddScaleVector(st_Line.B, st_Line.A, &pst_TangentBasis->T, fScale * pst_TangentBasis->BinormalDirection);
                        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&st_Line);

                        // Normal (Blue)
                        st_Line.ul_Color = (pc_Sel[wCurVertex] & 1) ? 0xffff0000 : 0xff800000;
                        MATH_AddScaleVector(st_Line.B, st_Line.A, &pst_TangentBasis->SxT, fScale);
                        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&st_Line);
                    }
                }
            }
        }
        else
#endif
        {		
		if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_ShowNormalsOfSelected)
		{
			//st_Line.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_NormalColor[c_ColorOffset + 1];
			for(; pst_V < pst_Last; pst_V++, pst_Normal++, pc_Sel++)
			{
				if(!(*pc_Sel & 1)) continue;
				st_Line.A = VCast(pst_V);
				MATH_AddScaleVector
				(
					st_Line.B,
					st_Line.A,
					pst_Normal,
					GDI_gpst_CurDD->pst_EditOptions->f_ScaleForNormals
				);
#if defined(_XENON_RENDER)
                st_Line.ul_Color = 0xffff0000;
#else
                st_Line.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_NormalColor[c_ColorOffset + 1];
#endif				
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_Line);
			} 
		}
		else
		{
			for(; pst_V < pst_Last; pst_V++, pst_Normal++)
			{
				st_Line.A = VCast(pst_V);
				MATH_AddScaleVector
				(
					st_Line.B,
					st_Line.A,
					pst_Normal,
					GDI_gpst_CurDD->pst_EditOptions->f_ScaleForNormals
				);
#if defined(_XENON_RENDER)
                st_Line.ul_Color = ((*pc_Sel) & 1) ? 0xffff0000 : 0xff800000;
#else
				st_Line.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_NormalColor[c_ColorOffset + ((*pc_Sel++) & 1) ];
#endif
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_Line);
			}
			}
		}
	}

    /* display morph data */
    if ( pst_Obj->pst_SubObject->pst_MorphData && !(GDI_gpst_CurDD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_HideMorphVector) )
    {
        GEO_tdst_ModifierMorphing_Data *pst_Morph;

        pst_Morph =  (GEO_tdst_ModifierMorphing_Data *) pst_Obj->pst_SubObject->pst_MorphData;
        pst_V = pst_Obj->dst_Point;

        st_Line.f_Width = GDI_gpst_CurDD->pst_EditOptions->f_NormalSize;
        st_Line.ul_Flags = 0;
        st_Line.B = &v;

        for ( i_MorphVector = 0; i_MorphVector < pst_Morph->l_NbVector; i_MorphVector++)
        {
            if (pst_Obj->pst_SubObject->dc_VSel[ pst_Morph->dl_Index[i_MorphVector] ] & 1 )
                st_Line.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_MorphVectorColor[1];
            else
                st_Line.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_MorphVectorColor[0];
            st_Line.A = VCast(pst_V + pst_Morph->dl_Index[i_MorphVector] );
			MATH_AddVector( st_Line.B, st_Line.A, pst_Morph->dst_Vector + i_MorphVector );
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_Line);
        }
    }

	/* display edges */
	if (	b_ShowSub &&
			(
			  (GDI_gpst_CurDD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_Edge) ||
		 	  ((GDI_gpst_CurDD->pst_EditOptions->ul_Flags & (GRO_Cul_EOF_Vertex | GRO_Cul_EOF_ShowEdge)) == (GRO_Cul_EOF_Vertex | GRO_Cul_EOF_ShowEdge) )
		 	)
       )
	{
		st_Line.f_Width = GDI_gpst_CurDD->pst_EditOptions->f_EdgeSize;
		st_Line.ul_Flags = 0;
		
		pst_Edge = pst_Obj->pst_SubObject->dst_Edge;
		pst_LastEdge = pst_Edge + pst_Obj->pst_SubObject->ul_NbEdges;
		pc_Sel = pst_Obj->pst_SubObject->dc_ESel;
		
		for(; pst_Edge < pst_LastEdge; pst_Edge++)
		{
			st_Line.A = VCast(pst_Vertex + pst_Edge->ul_Point[0]);
			st_Line.B = VCast(pst_Vertex + pst_Edge->ul_Point[1]);

			pst_Triangle1 = pst_Triangle2 = NULL;
			if(pst_Edge->ul_Triangle[0] != -1) pst_Triangle1 = pst_Obj->dst_Element[pst_Edge->ul_Triangle[0] >> 16].dst_Triangle + (pst_Edge->ul_Triangle[0] & 0xFFFF);
			if(pst_Edge->ul_Triangle[1] != -1) pst_Triangle2 = pst_Obj->dst_Element[pst_Edge->ul_Triangle[1] >> 16].dst_Triangle + (pst_Edge->ul_Triangle[1] & 0xFFFF);
			if((!pst_Triangle1 || pst_Triangle1->ul_MaxFlags & 0x80000000) && (!pst_Triangle2 || pst_Triangle2->ul_MaxFlags & 0x80000000))
			{
				pc_Sel ++;
				continue;
			}

			st_Line.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_EdgeColor[c_ColorOffset + (*pc_Sel++)];

			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_Line);
		}
	}

	/* display triangles */
	if ((GDI_gpst_CurDD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_Face) && b_ShowSub )
	{

        /* compute 2D normal for selection backface rejection */
        if (GDI_gpst_CurDD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_FaceBackfaceTest)
        {
            pst_Vertex = GDI_gpst_CurDD->p_Current_Vertex_List;
            /* compute pickable data */
	        SOFT_TransformAndProject
		    (
			    pst_Obj->pst_SubObject->dst_2D,
			    pst_Vertex,
			    pst_Obj->l_NbPoints,
			    &GDI_gpst_CurDD->st_Camera
		    );
            v2D = pst_Obj->pst_SubObject->dst_2D;

            pst_Face = pst_Obj->pst_SubObject->dst_Faces;
            for (i = 0 ; i< pst_Obj->l_NbElements; i++ )
            {
                pst_Triangle = pst_Obj->dst_Element[i].dst_Triangle;
                pst_Triangle1 = pst_Triangle + pst_Obj->dst_Element[i].l_NbTriangles;
                while (pst_Triangle < pst_Triangle1 )
                {
                    v0 = v2D + pst_Triangle->auw_Index[ 0 ];
                    v1 = v2D + pst_Triangle->auw_Index[ 1 ];
                    v2 = v2D + pst_Triangle->auw_Index[ 2 ];

                    pst_Face->fNZ = (v1->y * v2->x) - (v1->x * v2->y) + (v0->x * (v2->y - v1->y)) + (v0->y * (v1->x - v2->x));
                    
                    pst_Face++;
                    pst_Triangle++;
                }
            }
        }

        /* recompute center of faces */
        if ( pst_Vertex != pst_Obj->dst_Point )
        {
            pst_V = pst_Obj->dst_Point;
            pst_Obj->dst_Point = pst_Vertex;
            GEO_SubObject_BuildFaceData( pst_Obj );
            pst_Obj->dst_Point = pst_V;
            pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_FaceUseTransformedPoint;
        }
        else if ( pst_Obj->pst_SubObject->ul_Flags & GEO_Cul_SOF_FaceUseTransformedPoint )
        {
            GEO_SubObject_BuildFaceData( pst_Obj );
            pst_Obj->pst_SubObject->ul_Flags &= ~GEO_Cul_SOF_FaceUseTransformedPoint;
        }
                
        pst_Face = pst_Obj->pst_SubObject->dst_Faces;
		pst_LastFace = pst_Face + pst_Obj->pst_SubObject->ul_NbFaces;
		pst_2D = pst_Obj->pst_SubObject->dst_2D;
		pst_3D = pst_Obj->pst_SubObject->dst_3D;

		pc_Sel = pst_Obj->pst_SubObject->dc_FSel;
		st_Point.f_Size = GDI_gpst_CurDD->pst_EditOptions->f_FaceSize;

		if(pst_Obj->pst_SubObject->pc_UVMapper_Sel)
		{
			st_UV.f_Size = GDI_gpst_CurDD->pst_EditOptions->f_VertexSize;
			st_UV.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_VertexColor[c_ColorOffset + 1];

			for(; pst_Face < pst_LastFace; pst_Face++, pst_2D++, pst_3D++, *pc_Sel++)
			{
                if (*pc_Sel & 0x80) continue;

				if(*pc_Sel & 1)
				{
					pst_Triangle = pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
					for(i = 0; i < 3; i++)
					{
						if
						(
							pst_Obj->pst_SubObject->pc_UVMapper_Sel[pst_Triangle->auw_UV[i] * pst_Obj->
								pst_SubObject->c_UVMapper_SelInc]
						)
						{
							st_UV.A = VCast(pst_Vertex + pst_Triangle->auw_Index[i]);
							GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointEx, (ULONG) & st_UV);
						}
					}
				}
				
				SOFT_TransformAndProjectIn2Buffers
				(
					pst_3D,
					pst_2D,
					AVCast(&pst_Face->st_Center),
					1,
					&GDI_gpst_CurDD->st_Camera
				);
				st_Point.A = &pst_Face->st_Center;
				st_Point.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_FaceColor[c_ColorOffset + (*pc_Sel & 1 )];
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointEx, (ULONG) & st_Point);
			}
		}
		else
		{
			if ( GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_ShowFaceNormal )
			{
				st_Line.f_Width = GDI_gpst_CurDD->pst_EditOptions->f_NormalSize;
				st_Line.ul_Flags = 0;
				st_Line.B = &v;
			
				for(i = 0; pst_Face < pst_LastFace; pst_Face++, pst_2D++, pst_3D++, pc_Sel++, i++)
				{
					if (*pc_Sel & 0x80) continue;

					SOFT_TransformAndProjectIn2Buffers
					(
						pst_3D,
						pst_2D,
						AVCast(&pst_Face->st_Center),
						1,
						&GDI_gpst_CurDD->st_Camera
					);
					st_Point.A = &pst_Face->st_Center;
					st_Point.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_FaceColor[c_ColorOffset + (*pc_Sel) ];
					GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointEx, (ULONG) & st_Point);
					
					if ( !(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_ShowNormalsOfSelected) || (*pc_Sel & 1) )
					{
						st_Line.A = st_Point.A;
						MATH_AddScaleVector
						(
							st_Line.B,
							st_Line.A,
							&pst_Face->st_Normal,
							GDI_gpst_CurDD->pst_EditOptions->f_ScaleForNormals
						);
						st_Line.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_NormalColor[c_ColorOffset + (*pc_Sel & 1) ];
						GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_Line);
					}
				}
			}
			else
			{
				for(i = 0; pst_Face < pst_LastFace; pst_Face++, pst_2D++, pst_3D++, pc_Sel++, i++)
				{
					if (*pc_Sel & 0x80) continue;

					SOFT_TransformAndProjectIn2Buffers
					(
						pst_3D,
						pst_2D,
						AVCast(&pst_Face->st_Center),
						1,
						&GDI_gpst_CurDD->st_Camera
					);
					st_Point.A = &pst_Face->st_Center;
					st_Point.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_FaceColor[c_ColorOffset + (*pc_Sel) ];
					GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointEx, (ULONG) & st_Point);
				}
			}
		}

        /* display lines */
		st_Line.f_Width = GDI_gpst_CurDD->pst_EditOptions->f_EdgeSize;
		st_Line.ul_Flags = 0;
		pc_Sel = pst_Obj->pst_SubObject->dc_FSel;
        pc_ESel = pst_Obj->pst_SubObject->dc_ESel;
		pst_Edge = pst_Obj->pst_SubObject->dst_Edge;
		pst_LastEdge = pst_Edge + pst_Obj->pst_SubObject->ul_NbEdges;
		for(; pst_Edge < pst_LastEdge; pst_Edge++, pc_ESel++ )
		{
            if (*pc_ESel & 0x80) 
                continue;

			if((pst_Edge->uw_Index[1] != 0xFFFF) && (pc_Sel[pst_Edge->uw_Index[1]] & 1))
				st_Line.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_FaceColor[ c_ColorOffset + (pc_Sel[pst_Edge->uw_Index[1]] & 1) ];
			else
				st_Line.ul_Color = GDI_gpst_CurDD->pst_EditOptions->ul_FaceColor[ c_ColorOffset + (pc_Sel[pst_Edge->uw_Index[0]] & 1) ];

			if((GDI_gpst_CurDD_SPR.ul_DisplayFlags & GDI_Cul_DF_ShowCurrentCOB) && (!(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) (pst_Edge->ul_Point[0]))) || !(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) (pst_Edge->ul_Point[1]))))) 
			{
				continue;
			}

			pst_Triangle1 = pst_Triangle2 = NULL;
			if(pst_Edge->ul_Triangle[0] != -1) pst_Triangle1 = pst_Obj->dst_Element[pst_Edge->ul_Triangle[0] >> 16].dst_Triangle + (pst_Edge->ul_Triangle[0] & 0xFFFF);
			if(pst_Edge->ul_Triangle[1] != -1) pst_Triangle2 = pst_Obj->dst_Element[pst_Edge->ul_Triangle[1] >> 16].dst_Triangle + (pst_Edge->ul_Triangle[1] & 0xFFFF);
	
			st_Line.A = VCast(pst_Vertex + pst_Edge->ul_Point[0]);
			st_Line.B = VCast(pst_Vertex + pst_Edge->ul_Point[1]);
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_Line);
		}
	}

	/* polygon offset */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		static float	f[2] = { 1, 1 };
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Enable, 0x8037 /* GL_POLYGON_OFFSET_FILL */ );
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Enable, 0x2A02 /* GL_POLYGON_OFFSET_LINE */ );
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PolygonOffset, (ULONG) f);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PolygonOffsetSave, 0); /* store polygon offset editor parameter */
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Render_SubObjectEnd(OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *pst_Obj)
{
	if(!(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) || !pst_Obj->pst_SubObject) return;
	if(GDI_gpst_CurDD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_HideSubObject)
		return;
	GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Disable, 0x8037 /* GL_POLYGON_OFFSET_FILL */ );
	GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Disable, 0x2A02 /* GL_POLYGON_OFFSET_LINE */ );
	GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PolygonOffsetSave, 0); /* store polygon offset editor parameter */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void			*GEO_gpst_SnapshotGAOSrc;
GEO_tdst_Object *GEO_gpst_SnapshotSrc;
GEO_tdst_Object *GEO_gpst_SnapshotTgt;
void			(*GEO_gpfnv_AdjustRLI) (ULONG *) = NULL;

void GEO_4Edit_Snapshot( GEO_tdst_Object *pst_Geo )
{
    GEO_Vertex                          *pst_Vertex;
    int                                 i, j, k;
    ULONG                               *pul_Src, *pul_Tgt;
    GEO_tdst_IndexedTriangle	        *T, *TSrc;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
    unsigned short						*p_Redir,*p_FrstElm,*p_LstElm;
	ULONG								Counter;

    /* set GEO_gpst_SnapshotSrc to null : only one snap */
    GEO_gpst_SnapshotSrc = NULL;

    /* delete skin data of target geo */
    GEO_SKN_DestroyObjPonderation( GEO_gpst_SnapshotTgt->p_SKN_Objectponderation );
	GEO_gpst_SnapshotTgt->p_SKN_Objectponderation = NULL;

    /* delete mrm data of target geo */
    GEO_MRM_Destroy( GEO_gpst_SnapshotTgt );

    /* get good list of vertices */
    if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer)
        pst_Vertex = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
	else
		pst_Vertex = pst_Geo->dst_Point;

    L_memcpy( GEO_gpst_SnapshotTgt->dst_Point, pst_Vertex, GEO_gpst_SnapshotTgt->l_NbPoints * sizeof( GEO_Vertex ) );

	if (pst_Geo->p_SKN_Objectponderation)
	GEO_gpst_SnapshotTgt->p_SKN_Objectponderation = GEO_SKN_Duplicate(pst_Geo->p_SKN_Objectponderation);

    if(GDI_gpst_CurDD->pus_ReorderBuffer == NULL )
        return;
	
    /* la suite n'est faite qu'en cas de MRM sur la source */
    GEO_gpst_SnapshotTgt->l_NbPoints = pst_Geo->l_NbPoints;

    /* RLI */
    if ((pul_Src = pst_Geo->dul_PointColors) && (pul_Tgt = GEO_gpst_SnapshotTgt->dul_PointColors) )
    {
        pul_Src++;
        *pul_Tgt++ = GEO_gpst_SnapshotTgt->l_NbPoints;
        for (i = 0; i < GEO_gpst_SnapshotTgt->l_NbPoints; i++)
        {
            pul_Tgt[ i ] = pul_Src[ GDI_gpst_CurDD->pus_ReorderBuffer[i] ];
        }
    }

    p_Redir = (unsigned short *) L_malloc( (LONG)2 * pst_Geo->l_NbPoints );

    /* triangles */
    for (i = 0; i < pst_Geo->l_NbElements; i++)
    {
        pst_Element = GEO_gpst_SnapshotTgt->dst_Element + i;
        pst_Element->l_MaterialId = pst_Geo->dst_Element[ i ].l_MaterialId;
        pst_Element->l_NbTriangles = pst_Geo->dst_Element[ i ].l_NbTriangles;

        if (pst_Geo->dst_Element[ i ].l_NbTriangles == 0)
            continue;

        L_memset(p_Redir , 0 , pst_Geo->l_NbPoints * 2);
		p_FrstElm = pst_Geo->dst_Element[ i ].pus_ListOfUsedIndex ;
		p_LstElm  = p_FrstElm + pst_Geo->dst_Element[ i ].ul_NumberOfUsedIndex;
		Counter = 0;
        k = pst_Geo->dst_Element[ i ].p_MrmElementAdditionalInfo->ul_One_UV_Per_Point_Per_Element_Base;
		while (p_FrstElm < p_LstElm ) p_Redir[*(p_FrstElm++)] = (unsigned short)(k + Counter++);

        T = pst_Element->dst_Triangle;
        TSrc = pst_Geo->dst_Element[i].dst_Triangle;
        for (j = 0; j < pst_Element->l_NbTriangles; j++, T++, TSrc++)
        {
            T->auw_Index[0] = TSrc->auw_Index[0];
            T->auw_Index[1] = TSrc->auw_Index[1];
            T->auw_Index[2] = TSrc->auw_Index[2];
            T->auw_UV[ 0 ] = p_Redir[TSrc->auw_Index[0]];
			T->auw_UV[ 1 ] = p_Redir[TSrc->auw_Index[1]];
			T->auw_UV[ 2 ] = p_Redir[TSrc->auw_Index[2]];
        }
    }
    L_free( p_Redir );

    GEO_Clean( NULL , GEO_gpst_SnapshotTgt , NULL);
    GEO_ComputeNormals( GEO_gpst_SnapshotTgt );
}

#define GEO_M_4Edit_Snapshot(a, b) if ( (GEO_gpst_SnapshotGAOSrc == a) && ( GEO_gpst_SnapshotSrc == b ) ) GEO_4Edit_Snapshot( b );
#define GEO_M_4Edit_AdjustRLI( a ) if (GEO_gpfnv_AdjustRLI && a) GEO_gpfnv_AdjustRLI( a - 1)

#else
#define GEO_Render_SubObject(a, b)
#define GEO_Render_SubObjectEnd(a, b)
#define GEO_M_4Edit_Snapshot(a, b)
#define GEO_M_4Edit_AdjustRLI( a )
#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Render_InitRLI_NoDiffuse(GDI_tdst_DisplayData *_pst_DD, GEO_tdst_Object *pst_Obj, GRO_tdst_Visu *pst_Visu)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	*pul_Init, *pul_Last, *pul_RLI;//, ul_Counter, ul_Sub;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pul_Init = _pst_DD->pst_ComputingBuffers->aul_Diffuse;
	pul_Last = pul_Init + pst_Obj->l_NbPoints;

    if(pst_Visu->dul_VertexColors)
        pul_RLI = pst_Visu->dul_VertexColors + 1;
	else if(pst_Obj->dul_PointColors)
		pul_RLI = pst_Obj->dul_PointColors + 1;
	else
		pul_RLI = NULL;
    
	_pst_DD->pul_RLI_Used = pul_RLI;

	_pst_DD->pst_ComputingBuffers->ul_Flags &=  ~SOFT_Cul_CB_SpecularColorField;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Render_InitRLI(GDI_tdst_DisplayData *_pst_DD, GEO_tdst_Object *pst_Obj, GRO_tdst_Visu *pst_Visu)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	*pul_Init, *pul_Last, *pul_RLI, ul_Counter, ul_Sub;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pul_Init = _pst_DD->pst_ComputingBuffers->aul_Diffuse;
	pul_Last = pul_Init + pst_Obj->l_NbPoints;



#if defined( _XBOX )
	if (((pst_Obj->dst_Element != NULL) && (pst_Obj->dst_Element[0].pst_Gx8Add != NULL)) )
    {
		GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UpdateRLI;
	}
#endif

	if(pst_Visu->dul_VertexColors)
	{
        pul_RLI = pst_Visu->dul_VertexColors + 1;
		
	}
	else if(pst_Obj->dul_PointColors)
	{
		pul_RLI = pst_Obj->dul_PointColors + 1;
	}
	else
	{
		pul_RLI = NULL;
	}

    
	_pst_DD->pul_RLI_Used = pul_RLI;
	
	GEO_M_4Edit_AdjustRLI( pul_RLI );

#ifdef _XENON_RENDER
	#if defined(ACTIVE_EDITORS)
	if (GDI_b_IsXenonGraphics() && (LIGHT_gl_ComputeRLI == 0) && !LIGHT_gb_DisableXenonSend)
	#endif
	{
		if(_pst_DD->pul_RLI_Used)
		    GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors = _pst_DD->pul_RLI_Used;
        else
        {
            GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors = GDI_gpst_CurDD->pst_ComputingBuffers->staticComputedColors;
            ZeroMemory(GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors, pst_Obj->l_NbPoints*sizeof(ULONG));
        }
        
        return;
	}
#endif
	if(pul_RLI && (_pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_UseRLI) ) 
    {
		if(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_BlendRLI)
		{
			_pst_DD->ul_DisplayInfo -= GDI_Cul_DI_BlendRLI;
			if(_pst_DD->pus_ReorderBuffer)
			{
				ul_Counter = 0;
				while(pul_Init < pul_Last)
				{
					*pul_Init++ = LIGHT_ul_Interpol2Colors
						(
							*pul_RLI++,
							_pst_DD->pul_RLIBlend[_pst_DD->pus_ReorderBuffer[ul_Counter++]],
							_pst_DD->f_RLIBlend
						);
				}
			}
			else
			{
				while(pul_Init < pul_Last)
				{
					*pul_Init++ = LIGHT_ul_Interpol2Colors
						(
							*pul_RLI++,
							*_pst_DD->pul_RLIBlend++,
							_pst_DD->f_RLIBlend
						);
				}
			}
		}
        else
		{
            if ( (_pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_DontScaleRLI) || (_pst_DD->f_RLIScale == 0.0f) || (_pst_DD->ul_RLIColorDest==0) )
            {
			    if(_pst_DD->pus_ReorderBuffer)
			    {
				    ul_Counter = 0;
				    while(pul_Init < pul_Last) *pul_Init++ = pul_RLI[_pst_DD->pus_ReorderBuffer[ul_Counter++]];
			    }
			    else
				    L_memcpy(pul_Init , pul_RLI , pst_Obj->l_NbPoints << 2);
            }
            else
            {
                
                ul_Sub = COLOR_ul_Mul( _pst_DD->ul_RLIColorDest, _pst_DD->f_RLIScale );


                if(_pst_DD->pus_ReorderBuffer)
			    {
				    ul_Counter = 0;
				    while(pul_Init < pul_Last) 
                    {
                        *pul_Init = pul_RLI[_pst_DD->pus_ReorderBuffer[ul_Counter++]];
                        LIGHT_SubColor( pul_Init++, ul_Sub );
                    }
                }
			    else
                {
#if defined(OPT_MMX_SUB_LOOP) 
					//let's do a FAST loop in MMX
                    while(pul_Init < pul_Last) 
                    {
						_mm_prefetch(((char * )pul_Init)+0x30 ,  _MM_HINT_NTA );
                        *pul_Init = *pul_RLI++;

						*pul_Init = _mm_cvtsi64_si32(_mm_subs_pu8 (_mm_cvtsi32_si64(*pul_Init) , _mm_cvtsi32_si64(ul_Sub)));
						pul_Init++;
					}
					_mm_empty();
#else
                    while(pul_Init < pul_Last) 
                    {
                        *pul_Init = *pul_RLI++;
                        LIGHT_SubColor( pul_Init++, ul_Sub );
                    }
#endif
                }
            }
		}
	}
	else
	{
		L_memset(pul_Init , 0x00 , pst_Obj->l_NbPoints << 2);
	}
	
	_pst_DD->pst_ComputingBuffers->ul_Flags &=  ~SOFT_Cul_CB_SpecularColorField;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_Material *GEO_GetMaterial(MAT_tdst_Material *_pst_Material, ULONG _l_SubMaterial)
{
	if(_pst_Material == NULL) return (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

	if(_pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti)
	{
		MAT_tdst_Multi			*pst_MMat;
		pst_MMat = (MAT_tdst_Multi *) _pst_Material;
		_l_SubMaterial = lMin(_l_SubMaterial , pst_MMat->l_NumberOfSubMaterials -1 );
		_pst_Material = (MAT_tdst_Material*) pst_MMat->dpst_SubMaterial[_l_SubMaterial];
		if(!_pst_Material) return (MAT_tdst_Material *)&MAT_gst_DefaultSingleMaterial;
	}
	return _pst_Material;
}
 
 static int MaxTransparentT = 0;
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Render_TransparentTriangles(GDI_tdst_DisplayData *_pst_DD , GEO_tdst_Object *pst_Obj, GRO_tdst_Visu *pst_Visu)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector					*pst_V1, *pst_V2, *pst_V3;
	GEO_Vertex							*pst_VertexList;
	GEO_tdst_IndexedTriangle			*pst_Triangle, *pst_LastTriangle;
	float								f_Z;
	SOFT_tdst_ZList_Node				*pst_Node;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	ULONG								ul_ObjectMask;
	MAT_tdst_Material					*p_MaterialToUse;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef _DEBUG
	if(pst_Obj->l_NbPoints > MaxTransparentT)
		MaxTransparentT = pst_Obj->l_NbPoints;		
#endif

    if (pst_Obj->l_NbPoints > SOFT_Cul_ComputingBufferSize) 
    {
#ifdef ACTIVE_EDITORS
        ERR_X_Warning(0, "The GFX size exceeds the RAM capacity", NULL);
#endif	
        return;
    }
    
	pst_Element = pst_Obj->dst_Element;
	pst_LastElement = pst_Element + pst_Obj->l_NbElements;
	ul_ObjectMask = _pst_DD->ul_CurrentDrawMask;

	SOFT_TransformInComputingBuffer
	(
		_pst_DD->pst_ComputingBuffers,
		_pst_DD->p_Current_Vertex_List,
		pst_Obj->l_NbPoints,
		&_pst_DD->st_Camera
	);
		
	for(; pst_Element < pst_LastElement; pst_Element++)
	{
		PRO_IncRasterLong(&_pst_DD->pst_Raster->st_NbTriangles, pst_Element->l_NbTriangles);

		/* No multitexture & transparency -> Classic algorithm */
		p_MaterialToUse = GEO_GetMaterial((MAT_tdst_Material *) pst_Visu->pst_Material, pst_Element->l_MaterialId);

		pst_VertexList = _pst_DD->pst_ComputingBuffers->ast_3D;

		pst_Triangle = pst_Element->dst_Triangle;
		pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

		for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
		{
			pst_V1 = VCast(pst_VertexList + pst_Triangle->auw_Index[0]);
			pst_V2 = VCast(pst_VertexList + pst_Triangle->auw_Index[1]);
			pst_V3 = VCast(pst_VertexList + pst_Triangle->auw_Index[2]);

			f_Z = pst_V1->z;
			f_Z = fMax(f_Z , pst_V2->z);
			f_Z = fMax(f_Z , pst_V3->z);
			
			if (f_Z > 0.0f)
			{
				f_Z = fOptInv(f_Z);

				if((pst_Node = SOFT_pst_ZList_AddNode(f_Z, 3, SOFT_l_TON_Vertices)) != NULL)
				{
					*((MATH_tdst_Vector *) &pst_Node->pst_Vertices[0]) = *pst_V1;
					*((MATH_tdst_Vector *) &pst_Node->pst_Vertices[1]) = *pst_V2;
					*((MATH_tdst_Vector *) &pst_Node->pst_Vertices[2]) = *pst_V3;
					if(pst_Obj->dst_UV)
					{
						pst_Node->pst_Vertices[0].u = pst_Obj->dst_UV[pst_Triangle->auw_UV[0]].fU;
						pst_Node->pst_Vertices[0].v = pst_Obj->dst_UV[pst_Triangle->auw_UV[0]].fV;
						pst_Node->pst_Vertices[1].u = pst_Obj->dst_UV[pst_Triangle->auw_UV[1]].fU;
						pst_Node->pst_Vertices[1].v = pst_Obj->dst_UV[pst_Triangle->auw_UV[1]].fV;
						pst_Node->pst_Vertices[2].u = pst_Obj->dst_UV[pst_Triangle->auw_UV[2]].fU;
						pst_Node->pst_Vertices[2].v = pst_Obj->dst_UV[pst_Triangle->auw_UV[2]].fV;
					}

					pst_Node->pst_Vertices[0].color = _pst_DD->pst_ComputingBuffers->ComputedColors[pst_Triangle->auw_Index[0]];
					pst_Node->pst_Vertices[1].color = _pst_DD->pst_ComputingBuffers->ComputedColors[pst_Triangle->auw_Index[1]];
					pst_Node->pst_Vertices[2].color = _pst_DD->pst_ComputingBuffers->ComputedColors[pst_Triangle->auw_Index[2]];

					pst_Node->pst_Material = (MAT_tdst_MultiTexture *)p_MaterialToUse;
					pst_Node->l_DrawMask = _pst_DD->ul_CurrentDrawMask;
				}
			}
		}

		_pst_DD->ul_CurrentDrawMask = ul_ObjectMask;
	}
}


#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_ElementIndexedTriangles *GEO_OK3_CreateFakeElementArray(GEO_tdst_Object *_pst_Obj, COL_tdst_OK3 *_pst_OK3)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*dpst_Element;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	GEO_tdst_ElementIndexedTriangles	*pst_GeoElement;
	GEO_tdst_IndexedTriangle			*pst_Triangle, *pst_LastTriangle;
	GEO_tdst_IndexedTriangle			*pst_Zlabog;
	COL_tdst_OK3_Box					*pst_OK3_Box, *pst_OK3_LastBox;
	COL_tdst_OK3_Element				*pst_OK3_Element, *pst_OK3_LastElement;
	ULONG								l;
	USHORT								i, k;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj || !_pst_OK3) return NULL;

	dpst_Element = (GEO_tdst_ElementIndexedTriangles *) MEM_p_Alloc(_pst_Obj->l_NbElements * sizeof(GEO_tdst_ElementIndexedTriangles));
	L_memcpy(&dpst_Element[0], &_pst_Obj->dst_Element[0], _pst_Obj->l_NbElements * sizeof(GEO_tdst_ElementIndexedTriangles));

	pst_GeoElement = _pst_Obj->dst_Element;
	pst_Element = dpst_Element;
	pst_LastElement = dpst_Element + _pst_Obj->l_NbElements;
	while(pst_Element < pst_LastElement)
	{
		pst_Element->dst_Triangle = NULL;

		if(pst_GeoElement->l_NbTriangles)
		{
			pst_Element->dst_Triangle = (GEO_tdst_IndexedTriangle *) MEM_p_Alloc(pst_GeoElement->l_NbTriangles * sizeof(GEO_tdst_ElementIndexedTriangles));
			L_memset(pst_Element->dst_Triangle, 0, pst_GeoElement->l_NbTriangles * sizeof(GEO_tdst_ElementIndexedTriangles));
		}
		pst_Element->l_NbTriangles = 0;

		pst_Element ++;
		pst_GeoElement++;
	}


	pst_OK3_Box = _pst_OK3->pst_OK3_Boxes;
	pst_OK3_LastBox = pst_OK3_Box + _pst_OK3->ul_NumBox;

	while(pst_OK3_Box < pst_OK3_LastBox)
	{
		/* If box Culled, skip box */
		if(!(pst_OK3_Box->ul_OK3_Flag & 2)) 
		{
			pst_OK3_Box ++;
			continue;
		}

		pst_OK3_Element = pst_OK3_Box->pst_OK3_Element;
		pst_OK3_LastElement = pst_OK3_Element + pst_OK3_Box->ul_NumElement;

		while(pst_OK3_Element < pst_OK3_LastElement)
		{
			pst_Element = &_pst_Obj->dst_Element[pst_OK3_Element->uw_Element];
			for(k = 0; k < pst_OK3_Element->uw_NumTriangle; k++)
			{
				/* Tag Triangle to indicate that it is visible */
				pst_Element->dst_Triangle[pst_OK3_Element->puw_OK3_Triangle[k]].auw_Index[0] |= 0x8000;
			}
			pst_OK3_Element ++;
		}

		pst_OK3_Box ++;
	}

	pst_GeoElement = _pst_Obj->dst_Element;
	pst_Element = dpst_Element;
	pst_LastElement = pst_GeoElement + _pst_Obj->l_NbElements;
	while(pst_GeoElement < pst_LastElement)
	{
		if(!pst_GeoElement->l_NbTriangles) 
		{
			pst_Element ++;
			pst_GeoElement ++;
			continue;
		}

		i = 0;
		l = 0;
		pst_Triangle = pst_GeoElement->dst_Triangle;
		pst_LastTriangle = pst_Triangle + pst_GeoElement->l_NbTriangles;
		while(pst_Triangle < pst_LastTriangle)
		{
			if(pst_Triangle->auw_Index[0] & 0x8000)
			{
				/* Remove Tag */
				pst_Triangle->auw_Index[0] &= ~0x8000;

				if(!i) pst_Zlabog = pst_Triangle;
				i ++;
			}
			else
			{
				if(i)
				{
					L_memcpy(&pst_Element->dst_Triangle[l], pst_Zlabog, i * sizeof(GEO_tdst_IndexedTriangle));
					l += i;
				}
				i = 0;
			}


			pst_Triangle ++;
		}
		pst_Element->l_NbTriangles = (long) l;

		pst_Element ++;
		pst_GeoElement ++;
	}

	return dpst_Element;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_OK3_FreeFakeElementArray(GEO_tdst_ElementIndexedTriangles *dpst_Element, LONG _l_NbElements)
{
	/*~~~~~~*/
	int		i;
	/*~~~~~~*/

	for(i = 0; i < _l_NbElements; i++)
	{
		if(dpst_Element[i].dst_Triangle)
			MEM_Free(dpst_Element[i].dst_Triangle);
	}

	MEM_Free(dpst_Element);
}
#endif

#ifdef USE_GO_DATA
extern u_int GSP_AllColorsAreCached(OBJ_tdst_GameObject *_pst_GO , GEO_tdst_Object	*pst_Obj);
extern void GSP_SaveCachedData(OBJ_tdst_GameObject *_pst_GO , GEO_tdst_Object	*pst_Obj);
#endif

#if defined(_XBOX)
extern int g_MultipleVBIndex;
#endif // defined(_XBOX)

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_GetRenderAddSupColor(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int	i, sect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO->pst_Extended) return 0;
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone) _pst_GO = ANI_pst_GetReferenceInit(_pst_GO);
	if(!_pst_GO->pst_Extended) return 0;
	for(i = 0; i < 4; i++)
	{
		sect = _pst_GO->pst_Extended->auc_Sectos[i];
		if(!sect) continue;
		if(_pst_GO->pst_World->ast_AllSectos[sect].ul_Flags & WOR_CF_SectorColor1) return 0x00AF0000;
		if(_pst_GO->pst_World->ast_AllSectos[sect].ul_Flags & WOR_CF_SectorColor2) return 0x0000AF00;
		if(_pst_GO->pst_World->ast_AllSectos[sect].ul_Flags & WOR_CF_SectorColor3) return 0x000000AF;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_RenderAddSup(OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	MAT_tdst_MultiTexture				st_Mat;
	MAT_tdst_MTLevel					st_Level;
	ULONG								ul_ObjectMask, SaveDI, col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	col = GEO_GetRenderAddSupColor(_pst_GO);
	if(!col) return;
	ul_ObjectMask = GDI_gpst_CurDD_SPR.ul_CurrentDrawMask;
	SaveDI = GDI_gpst_CurDD_SPR.ul_DisplayInfo;

	L_memset(&st_Mat, 0, sizeof(st_Mat));
	L_memset(&st_Level, 0, sizeof(st_Level));
	st_Mat.pst_FirstLevel = &st_Level;
	st_Level.s_TextureId = -1;
	MAT_SET_ColorOp(st_Level.ul_Flags, MAT_Cc_ColorOp_Disable);
	MAT_SET_Blending(st_Level.ul_Flags, MAT_Cc_Op_Add);
	st_Mat.ul_Specular = col;
	GRO_Struct_Init(&st_Mat.st_Id, GRO_MaterialMultiTexture);

	pst_Element = pst_Obj->dst_Element;
	pst_LastElement = pst_Element + pst_Obj->l_NbElements;
    GDI_gpst_CurDD_SPR.ul_DisplayInfo |= GDI_Cul_DI_RenderingTransparency;
	for(; pst_Element < pst_LastElement; pst_Element++)
	{
		MAT_DrawIndexedTriangle
		(
			pst_Obj,
			(MAT_tdst_Material *) &st_Mat,
			pst_Element
		);
	}

	GDI_gpst_CurDD_SPR.ul_CurrentDrawMask = ul_ObjectMask;
	GDI_gpst_CurDD_SPR.ul_DisplayInfo = SaveDI;
}

#endif /* ACTIVE_EDITORS */


/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef USE_GO_DATA
	u_int								ulPs2CacheFlags;
#endif

/*
 =======================================================================================================================
	macro pour geo_render en active_editors
 =======================================================================================================================
 */

/*----[ ACTIVE EDITORS ]----*/
#ifdef ACTIVE_EDITORS

#define M4Edit_ComputeElementColor\
	{\
		if(pst_Cob)\
		{\
			if(GEO_4Edit_ColMapOnly_ComputeElementColor(pst_DD, _pst_GO, pst_Cob, pst_Obj, pst_Element, b_SelectedCob)) continue;\
		}\
		else if ( (pst_DD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_ColorModeMask) == GRO_Cul_EOF_ColorModeFaceID )\
		{\
			ULONG	ul_SaveDM, ul_SaveCC;\
			ul_SaveDM = pst_DD->ul_CurrentDrawMask;\
			ul_SaveCC = pst_DD->ul_ColorConstant;\
			pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontForceColor;\
			pst_DD->ul_ColorConstant |= pst_DD->pst_EditOptions->ul_FaceIDColor[pst_Element->l_MaterialId % 32];\
			LIGHT_ComputeVertexColor(pst_Obj->l_NbPoints);\
			pst_DD->ul_CurrentDrawMask = ul_SaveDM;\
			pst_DD->ul_ColorConstant = ul_SaveCC;\
		}\
	}

/*----[ !ACTIVE EDITORS ]----*/
#else

#define M4Edit_ComputeElementColor

#endif /* ACTIVE_EDITORS */

void BackToFirstFog(GDI_tdst_DisplayData 		*pst_DD,GRO_tdst_Visu *pst_Visu)
{
		if ( !(pst_Visu->ul_DrawMask & GDI_Cul_DM_UseFog2) && pst_Visu->ul_DrawMask & GDI_Cul_DM_Fogged)
		{
			if (pst_DD->st_Fog2.c_Flag & SOFT_C_FogActive)
			pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_SetFogParams, (ULONG) & pst_DD->st_Fog1);
		}
}

void GEO_CopyMeshToColMap(OBJ_tdst_GameObject *_pst_GO,GEO_tdst_Object *_pst_Obj)
{
    if (_pst_GO && 
        (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap) &&	
        (_pst_GO->pst_Extended) &&
        (_pst_GO->pst_Extended->pst_Col) &&
        ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap &&
        ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob &&
        (((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Type == COL_C_Zone_Triangles))
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        COL_tdst_IndexedTriangles			*pst_Cob = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->pst_TriangleCob;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        if ((pst_Cob->l_NbPoints == _pst_Obj->l_NbPoints) && (pst_Cob->l_NbElements == _pst_Obj->l_NbElements))
        {
            MATH_tdst_Vector *pCobPoint = pst_Cob->dst_Point;
            GEO_Vertex *pGeoVertex = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
            GEO_Vertex *pLastVertex = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB+_pst_Obj->l_NbPoints;

            for (;pGeoVertex<pLastVertex;pGeoVertex++,pCobPoint++)
                MATH_CopyVector(pCobPoint,(MATH_tdst_Vector *)pGeoVertex);

            // Recompute normals
            COL_ColMap_RecomputeNormals(pst_Cob,_pst_Obj);

            // Recompute OK3
            COL_OK3_Build(_pst_GO, TRUE, TRUE);
        }
    }
}

#if defined(_XENON_RENDER)

#if defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL)
void GEO_Render_Default(OBJ_tdst_GameObject *_pst_GO);
#endif

void GEO_ComputeUV( OBJ_tdst_GameObject *_pst_GO, MAT_tdst_MTLevel * pst_MLTTXLVL )
{
    ERR_X_Assert( _pst_GO && pst_MLTTXLVL );

    // ComputeUV
    ULONG CurrentUVMode = MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags);
    ULONG CurrentAddFlag = pst_MLTTXLVL->s_AditionalFlags;
    MATH_tdst_Matrix	Matrix;
    MATH_tdst_Vector    CamPos;
    D3DXMATRIX	        Result;
    D3DXMatrixIdentity( &Result );

    GDI_tdst_DisplayData * pst_CurDD = GDI_gpst_CurDD;

    if( CurrentUVMode == MAT_Cc_UV_Planar_GZMO &&
        (MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_GIZMO) &&	
        (pst_CurDD->pst_CurrentGameObject->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix))
    {
        /*~~~~~~~~~~~~~*/
      LONG	GizmoNum;
        /*~~~~~~~~~~~~~*/

        GizmoNum = MAT_GET_GizmoNumber(CurrentAddFlag);
        if(_pst_GO->pst_Base->pst_AddMatrix->l_Number > GizmoNum)
        {
            if (pst_CurDD->pst_CurrentGameObject->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
            {
                if(pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO)
                {
                    if((!(pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) || (pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer))
                    {
                        MATH_tdst_Matrix	Matrix2;
                        MATH_InvertMatrix	(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO->pst_GlobalMatrix );
                        MATH_MulMatrixMatrix(&Matrix2 ,pst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix  , &Matrix );
                        MATH_MakeOGLMatrix	(&Matrix, &Matrix2);
                    }
                    else
                    {
                        if(pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_Matrix)
                        {
                            MATH_InvertMatrix(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_Matrix);
                            MATH_MakeOGLMatrix(&Matrix, &Matrix);
                        }
                        else
                        {
                            MATH_InvertMatrix(&Matrix, &MATH_gst_IdentityMatrix);
                            MATH_MakeOGLMatrix(&Matrix, &Matrix);
                        }
                    }
                }
                else
                {
                    MATH_InvertMatrix(&Matrix, &MATH_gst_IdentityMatrix);
                    MATH_MakeOGLMatrix(&Matrix, &Matrix);
                }
                CamPos = Matrix.T;
            }
            else
            {
                MATH_InvertMatrix(&Matrix,&pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_Gizmo[GizmoNum].st_Matrix);
                MATH_MakeOGLMatrix(&Matrix, &Matrix);
                CamPos = Matrix.T;
            }
        }
        else
        {
            MATH_MakeOGLMatrix(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);
            CamPos = Matrix.T;
        }

        switch(MAT_GET_XYZ(CurrentAddFlag))
        {
        case MAT_CC_X:
            Result._11 = Matrix.Iy;
            Result._21 = Matrix.Jy;
            Result._31 = Matrix.Ky;
            Result._41 = CamPos.y;

            Result._12 = Matrix.Iz;
            Result._22 = Matrix.Jz;
            Result._32 = Matrix.Kz;
            Result._42 = CamPos.z;
            break;
        case MAT_CC_Y:
            Result._11 = Matrix.Ix;
            Result._21 = Matrix.Jx;
            Result._31 = Matrix.Kx;
            Result._41 = CamPos.x;

            Result._12 = Matrix.Iz;
            Result._22 = Matrix.Jz;
            Result._32 = Matrix.Kz;
            Result._42 = CamPos.z;
            break;
        case MAT_CC_Z:
            Result._11 = Matrix.Ix;
            Result._21 = Matrix.Jx;
            Result._31 = Matrix.Kx;
            Result._41 = CamPos.x;

            Result._12 = Matrix.Iy;
            Result._22 = Matrix.Jy;
            Result._32 = Matrix.Ky;
            Result._42 = CamPos.y;
            break;

        case MAT_CC_XYZ:
            {
                MATH_tdst_Vector stU,stV,stUC,stVC ;
                MATH_tdst_Vector A;
                MATH_InitVector(&stUC , Cf_Sqrt2 * 0.5f , -Cf_Sqrt2 * 0.5f , 0.0f);
                MATH_InitVector(&stVC , -Cf_InvSqrt3 , -Cf_InvSqrt3 , Cf_InvSqrt3);
                A.x = MATH_f_DotProduct(&stUC , &CamPos);
                A.y = MATH_f_DotProduct(&stVC , &CamPos);
                MATH_TranspEq33Matrix(&Matrix);
                stU.x = Matrix.Ix * stUC.x + Matrix.Jx * stUC.y + Matrix.Kx * stUC.z ;
                stU.y = Matrix.Iy * stUC.x + Matrix.Jy * stUC.y + Matrix.Ky * stUC.z ;
                stU.z = Matrix.Iz * stUC.x + Matrix.Jz * stUC.y + Matrix.Kz * stUC.z ;
                stV.x = Matrix.Ix * stVC.x + Matrix.Jx * stVC.y + Matrix.Kx * stVC.z ;
                stV.y = Matrix.Iy * stVC.x + Matrix.Jy * stVC.y + Matrix.Ky * stVC.z ;
                stV.z = Matrix.Iz * stVC.x + Matrix.Jz * stVC.y + Matrix.Kz * stVC.z ;

                Result._11 = stU.x;
                Result._21 = stU.y;
                Result._31 = stU.z;
                Result._41 = A.x;

                Result._12 = stV.x;
                Result._22 = stV.y;
                Result._32 = stV.z;
                Result._42 = A.y;
            }
            break;
        }

        g_pXeContextManagerEngine->PushPlanarGizmoMatrix( &Result );
    }
}

/*
 =======================================================================================================================
	GEO_Render
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
ULONG GEO_ulCurrentTriangleNb;
#endif //ACTIVE_EDITORS

void GEO_Render(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Visu						*pst_Visu;
	GEO_tdst_Object						*pst_Obj;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	ULONG								ul_ObjectMask,ul_ValidityMask;

    ULONG                               ulExtraFlags = 0;
#ifdef XENONVIDEOSTATISTICS
    XeGOStatistics                      *XeStats = XeGOStatistics::Instance();
#endif

#if defined(ACTIVE_EDITORS)
	ULONG								ul_QMFREditorInfo;
	COL_tdst_Cob						*pst_Cob;
	BOOL								b_SelectedCob;
	u8									RLIloc;
#endif

	extern GEO_tdst_Object *GFX_gpst_Geo;

#if defined(USE_PIX)
    char s[256];
    strcpy(s, "GEO_Render(");
    if(_pst_GO->sz_Name)
    {
        strcat(s, _pst_GO->sz_Name);
    }
    else
    {
        char ptr[16];
        itoa((int)_pst_GO, ptr, 16);
        strcat(s, ptr);
    }
    strcat(s, ")");

    CXBBeginEventObjectCPU oEvent(0, s);
#endif

#if defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL)
    if (!GDI_b_IsXenonGraphics()
#if defined(ACTIVE_EDITORS)
        || (LIGHT_gl_ComputeRLI != 0)
#endif
        )
    {
        // Not using Xenon graphics, fallback to the default version of GEO_Render()
        GEO_Render_Default(_pst_GO);
        return;
    }
#endif

	_GSP_BeginRaster(23);

	pst_Visu = _pst_GO->pst_Base->pst_Visu;
	pst_Obj = (GEO_tdst_Object *) pst_Visu->pst_Object;
	GDI_gpst_CurDD->pst_CurrentGeo = pst_Obj;

	ul_ValidityMask = MAT_GetValidityMASK((MAT_tdst_Material *) pst_Visu->pst_Material, -1, pst_Obj);

	// AI
	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
        CXBBeginEventObjectCPU oEvent(0, "AI");

		if(_pst_GO->pst_Extended->pst_Ai)
		{
			AI_C_Callback = 1;
			AI_ExecCallback(_pst_GO, AI_C_Callback_BeforeDisplay);
			AI_C_Callback = 0;			
		}
		else if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)
		{
			OBJ_tdst_GameObject *pst_Ani = ANI_pst_GetReferenceInit(_pst_GO);
			if(pst_Ani && pst_Ani->pst_Extended && pst_Ani->pst_Extended->pst_Ai)
			{
				AI_C_Callback = 1;
				AI_ExecCallback(pst_Ani, AI_C_Callback_BeforeDisplay);
				AI_C_Callback = 0;				
			}
		}
	}

	if ( !pst_Obj->dst_Point ) 
	{
		_GSP_EndRaster(23);
		return;
	}

	// Modifiers 
	GDI_gpst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseSpecialVertexBuffer;
	GDI_gpst_CurDD->p_Current_Vertex_List = pst_Obj->dst_Point;
	
    // Reset the visible flag (Sprite mapper MDF will set it)
    pst_Visu->ucFlag &= ~GRO_VISU_FLAG_HIDE_MESH;

	// Apply modifiers
#ifdef _XENON_RENDER
    pst_Obj->m_pWYB1 = NULL;
    pst_Obj->m_pWYB2 = NULL;
#endif
	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
        CXBBeginEventObjectCPU oEvent(0, "MDF");

		if (_pst_GO->pst_Extended->pst_Modifiers)
			MDF_ApplyAll(_pst_GO, 0);
    }

    if(pst_Visu->ucFlag & GRO_VISU_FLAG_WATERFX)
    {
        CXBBeginEventObjectCPU oEvent(0, "WATER");
        void WATER3D_Modifier_Display(OBJ_tdst_GameObject *_pst_GO);
        
        if(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted)
        {
            LIGHT_SendObjectToLightXenon(&GDI_gpst_CurDD->st_LightList, _pst_GO, g_oXeRenderer.GetRequestedMaxLight());
        }

        WATER3D_Modifier_Display(_pst_GO);

        return;
    }

    // Symmetry modifier
    if (pst_Visu->ucFlag & GRO_VISU_FLAG_SYMMETRY)
    {
        MDF_tdst_Modifier* pModifier = MDF_pst_GetByType(_pst_GO, MDF_C_Modifier_Symetrie);
        if (pModifier != NULL)
        {
            GEO_tdst_ModifierSymetrie* pSymmetry = (GEO_tdst_ModifierSymetrie*)pModifier->p_Data;
            XeSymmetryParams*          pParams   = g_pXeContextManagerEngine->AddSymmetryParams();

            if (pSymmetry->ul_Flags & GEO_C_Symetrie_Z)
            {
                pParams->ulAxis = SYMMETRY_MODE_Z;
            }
            else if (pSymmetry->ul_Flags & GEO_C_Symetrie_Y)
            {
                pParams->ulAxis = SYMMETRY_MODE_Y;
            }
            else
            {
                pParams->ulAxis = SYMMETRY_MODE_X;
            }

            pParams->fOffset = pSymmetry->f_Offset;

            ulExtraFlags |= QMFR_SYMMETRY;
        }
    }

    if (GDI_gpst_CurDD->pst_CurrentAnim && 
        GDI_gpst_CurDD->pst_CurrentAnim->pst_Base &&
        GDI_gpst_CurDD->pst_CurrentAnim->pst_Base->pst_Visu)
    {
        ulExtraFlags |= QMFR_ANIMATED;
    }

    // if packing not done, pack the mesh for xenon
	// pst_Object->iVBNbVertex!=-1 will create dynamic meshes later instead as this mesh is temporary (not a loaded object)
	if( !pst_Obj->b_CreateMorphedMesh &&
         pst_Visu && pst_Obj && pst_Obj->l_NbElements && 
        ( (!pst_Visu->p_XeElements && pst_Visu->l_VBVertexCount == -1) ||
          ((pst_Visu->ul_VBObjectValidate != (ULONG)pst_Obj)   && (pst_Visu->l_VBVertexCount != 0)) ||
          ((pst_Visu->l_NbXeElements != pst_Obj->l_NbElements) && (pst_Visu->l_VBVertexCount != 0)) ) )
    {
        CXBBeginEventObjectCPU oEvent(0, "Create mesh");
        GEO_PackGameObject(_pst_GO);
    }

    BOOL bSoftwareSkinning = FALSE;

	// SKIN
	if(GEO_b_IsSkinned(_pst_GO, pst_Obj))
	{
        CXBBeginEventObjectCPU oEvent(0, "Skin");

        GEO_SKN_Compute4Display(_pst_GO, pst_Obj);

        // See if software skinning was requested
        bSoftwareSkinning = pst_Obj->b_ForceSoftSkinning;

#ifdef VS_FORCE_SW_SKINNING
        bSoftwareSkinning = TRUE;
#endif

        if( bSoftwareSkinning )
        {
            // See if we need to recompute the normals
            if( (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_DontRecomputeNormales ) )
            {
                CXBBeginEventObjectCPU oEvent(0, "Normals");
                GEO_SKN_ComputeNormals( pst_Obj, GDI_gpst_CurDD->p_Current_Vertex_List );
            }

            GEO_CreateSoftSkinXenonMesh( pst_Visu, pst_Obj );
        }
    }


	// RLI 
#ifndef _XENON_RENDER // make sure all objects have a color
	if ( (ul_ValidityMask & ( MAT_ValidateMask_RLI | MAT_ValidateMask_Diffuse | MAT_ValidateMask_Specular_Color )) )
#endif
	{
        CXBBeginEventObjectCPU oEvent(0, "RLI");

		_GSP_BeginRaster(8);

		GEO_Render_InitRLI(GDI_gpst_CurDD , pst_Obj, pst_Visu);

		// merge lighting with ambient and vertex paint
		if (ul_ValidityMask & (MAT_ValidateMask_Diffuse	|MAT_ValidateMask_Specular_Color))
		{
			if(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted)
			{
                LIGHT_SendObjectToLightXenon(&GDI_gpst_CurDD->st_LightList, _pst_GO, g_oXeRenderer.GetRequestedMaxLight());
                ulExtraFlags |= QMFR_LIGHTED;
            }

			// !!! must change the computedcolors to use a pointer instead of an array so we dont have to copy the same colors from rli to computed
#ifdef ACTIVE_EDITORS
			if(LIGHT_gb_DisableXenonSend)
			    LIGHT_ComputeVertexColor(pst_Obj->l_NbPoints); 
#endif
		}

		_GSP_EndRaster(8);
	}  

#if defined(ACTIVE_EDITORS)
    GEO_Render_SubObject(_pst_GO, pst_Obj);
#endif

	// send graphic elements
    pst_Element = pst_Obj->dst_Element;
    pst_LastElement = pst_Element + pst_Obj->l_NbElements;
	ul_ObjectMask = GDI_gpst_CurDD->ul_CurrentDrawMask;

	// for objects using the temporary geometric, create dynamic meshes

    MAT_tdst_Material* pst_Material = (MAT_tdst_Material *)pst_Visu->pst_Material;

#if defined(ACTIVE_EDITORS)
    if (pst_Material == NULL)
    {
        if (GDI_gpst_CurDD->pst_CurrentMat != NULL)
        {
            pst_Material = (MAT_tdst_Material*)GDI_gpst_CurDD->pst_CurrentMat;
        }
        else
        {
            // SC: Use the default material
            pst_Material = (MAT_tdst_Material*)&MAT_gst_DefaultSingleMaterial;
        }
    }
#endif

    if( !bSoftwareSkinning && 
        !pst_Obj->b_CreateMorphedMesh &&
        pst_Obj->l_NbElements && 
        (!pst_Visu->p_XeElements || pst_Visu->l_VBVertexCount == 0) && 
        pst_Material && 
        ((((MAT_tdst_Material *)pst_Material)->st_Id.i->ul_Type == GRO_MaterialMulti) || 
         (((MAT_tdst_Material *)pst_Material)->st_Id.i->ul_Type == GRO_MaterialMultiTexture)))
    {
        CXBBeginEventObjectCPU oEvent(0, "Create Dyn");
        GEO_CreateXenonDynamicMesh(pst_Visu, pst_Obj);
    }

    if( pst_Obj->b_CreateMorphedMesh )
    {
        CXBBeginEventObjectCPU oEvent(0, "Create MorphedXenonMesh");
        GEO_CreateMorphedXenonMesh( pst_Visu, pst_Obj );
    }


#if defined(ACTIVE_EDITORS)
	ul_QMFREditorInfo = 0;
#endif
    for(int iElem=0; pst_Element < pst_LastElement; pst_Element++, iElem++)
	{
        CXBBeginEventObjectCPU oEvent(0, "DRW");

        ULONG ulLMTexID = -1;

        ulExtraFlags &= ~QMFR_CULLEDFROMCAMERA;

        if (pst_Visu->pst_Material != NULL)
        {
            // Reset the material pointer so that we'll be able to fetch the right layer
            pst_Material = (MAT_tdst_Material *)pst_Visu->pst_Material;
        }

		if(pst_Visu->p_XeElements && pst_Visu->p_XeElements[iElem].pst_Mesh && pst_Material && ((pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti) || (pst_Material->st_Id.i->ul_Type == GRO_MaterialMultiTexture)))
		{
            XeMesh* poMesh = pst_Visu->p_XeElements[iElem].pst_Mesh;

            if (!GEO_IsXenonMeshVisible(_pst_GO, poMesh))
            {
                ulExtraFlags |= QMFR_CULLEDFROMCAMERA;
            }

            if(!(pst_Visu->ul_DrawMask & GDI_Cul_DM_DontReceiveLM) && pst_Visu->pp_st_LightmapCoords && pst_Visu->pp_st_LightmapCoords[iElem] && pst_Visu->pLMPage)
            {
                ulExtraFlags |= QMFR_USELIGHTMAP;
                ulLMTexID = pst_Visu->pLMPage->texIndex;
            }

            if (pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti)
            {
			    if(((MAT_tdst_Multi *)pst_Material)->l_NumberOfSubMaterials == 0)
				    pst_Material = NULL;
			    else
				    pst_Material = ((MAT_tdst_Multi *)pst_Material)->dpst_SubMaterial[lMin(pst_Element->l_MaterialId , ((MAT_tdst_Multi *)pst_Material)->l_NumberOfSubMaterials - 1)];
            }

            // Store WYB modifier data
            static FLOAT fSpeedScale = 1.33f;
            if(pst_Obj->m_pWYB1 != NULL)
            {
                GEO_tdst_ModifierOnduleTonCorps* pModifier = (GEO_tdst_ModifierOnduleTonCorps*)pst_Obj->m_pWYB1;

                ulExtraFlags |= QMFR_WYB1;
                XeWYBParams* p1 = g_pXeContextManagerEngine->AddWYBParams();
                p1->Axes[0].x = (pModifier->ul_Flags & GEO_C_OTCF_X) != 0 ? 1.0f : 0.0f;
                p1->Axes[0].y = (pModifier->ul_Flags & GEO_C_OTCF_Y) != 0 ? 1.0f : 0.0f;
                p1->Axes[0].z = (pModifier->ul_Flags & GEO_C_OTCF_Z) != 0 ? 1.0f : 0.0f;
                p1->Axes[0].w = 0;

                p1->Conditions[0].x = (pModifier->ul_Flags & GEO_C_OTCF_Planar) != 0 ? 1.0f : 0.0f;
                p1->Conditions[0].y = (pModifier->ul_Flags & GEO_C_OTCF_UseAlphaOfRLI) != 0 ? 1.0f : 0.0f;
                p1->Conditions[0].z = (pModifier->ul_Flags & GEO_C_OTCF_InvertAlpha) != 0 ? 1.0f : 0.0f;
                p1->Conditions[0].w = 1;    // Num WYB modifiers

                p1->Values[0].x = pModifier->f_Angle * fSpeedScale;
                p1->Values[0].y = pModifier->f_Amplitude;
                p1->Values[0].z = pModifier->f_Factor;
                p1->Values[0].w = 0.0f;

                if(pst_Obj->m_pWYB2 != NULL)
                {
                    ulExtraFlags |= QMFR_WYB2;
                    pModifier = (GEO_tdst_ModifierOnduleTonCorps*)pst_Obj->m_pWYB2;

                    p1->Axes[1].x = (pModifier->ul_Flags & GEO_C_OTCF_X) != 0 ? 1.0f : 0.0f;
                    p1->Axes[1].y = (pModifier->ul_Flags & GEO_C_OTCF_Y) != 0 ? 1.0f : 0.0f;
                    p1->Axes[1].z = (pModifier->ul_Flags & GEO_C_OTCF_Z) != 0 ? 1.0f : 0.0f;
                    p1->Axes[1].w = 0;

                    p1->Conditions[1].x = (pModifier->ul_Flags & GEO_C_OTCF_Planar) != 0 ? 1.0f : 0.0f;
                    p1->Conditions[1].y = (pModifier->ul_Flags & GEO_C_OTCF_UseAlphaOfRLI) != 0 ? 1.0f : 0.0f;
                    p1->Conditions[1].z = (pModifier->ul_Flags & GEO_C_OTCF_InvertAlpha) != 0 ? 1.0f : 0.0f;
                    p1->Conditions[1].w = 2;    // Num WYB modifiers
                    p1->Conditions[0].w = 2;    // Num WYB modifiers

                    p1->Values[1].x = pModifier->f_Angle * fSpeedScale;
                    p1->Values[1].y = pModifier->f_Amplitude;
                    p1->Values[1].z = pModifier->f_Factor;
                    p1->Values[1].w = 0.0f;
                }
            }
  
            MAT_tdst_MultiTexture	*pst_MLTTX = (MAT_tdst_MultiTexture *) pst_Material;
            MAT_tdst_MTLevel		*pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;

			if(pst_Material == NULL) pst_Material= (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

            if ((pst_MLTTXLVL != NULL) && !((pst_Visu->ucFlag & GRO_VISU_FLAG_HIDE_MESH) && (pst_Element->l_NbTriangles < 128)))
            {
                // All the multipass layers must be in the same rendering list
                eXeRENDERLISTTYPE  eRT;
                
                if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_RenderingInterface)
                    eRT = XeRT_INTERFACE;
                else if (((MAT_GET_Blending(pst_MLTTXLVL->ul_Flags) != MAT_Cc_Op_Copy) && (MAT_GET_Blending(pst_MLTTXLVL->ul_Flags) != MAT_Cc_Op_Glow)) || (pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_HideColor))
                {
                    eRT = XeRT_TRANSPARENT;
                    if( pst_MLTTXLVL->pst_XeLevel && (pst_MLTTXLVL->pst_XeLevel->ul_Flags & MAT_Xe_Flag_GlowEnable) != 0 )
                    {
                        eRT = XeRT_TRANSPARENT_GLOW;
                    }
                }
                else if( (pst_Visu->ul_DrawMask & GDI_Cul_DM_Fogged) == 0 )
                    eRT = XeRT_OPAQUE_NO_FOG;
                else
                    eRT = XeRT_OPAQUE;
                
                ULONG ulLayerIndex = 0;

				/* Test for highlight mode */
#if defined(ACTIVE_EDITORS)
				switch(GDI_gpst_CurDD->ul_HighlightMode)
				{
				case QMFR_EDINFO_TOO_MANY_LIGHTS:
					ULONG ul_LightSetIndex, ul_NbLights;

					if(pst_Visu->p_XeElements[iElem].pst_Mesh->UseLightSet())	//lighted per mesh
						ul_LightSetIndex = pst_Visu->p_XeElements[iElem].pst_Mesh->GetLightSetIndex();
					else	//lighted per gao
						ul_LightSetIndex = g_pXeContextManagerEngine->GetCurrentLightSetIndex();

					ul_NbLights = g_pXeContextManagerEngine->GetLightSetLightCount(ul_LightSetIndex) ;
					if(ul_NbLights > 4)
					{
						XeSetQMFREditorInfo(ulExtraFlags, QMFR_EDINFO_COLOR_01);
						_pst_GO->ul_AdditionalFlags |= OBJ_C_EdAddFlags_Highlighted;
					}
					else if(ul_NbLights == 4)
					{
						XeSetQMFREditorInfo(ulExtraFlags, QMFR_EDINFO_COLOR_02);
						_pst_GO->ul_AdditionalFlags |= OBJ_C_EdAddFlags_Highlighted;
					}
					else
						_pst_GO->ul_AdditionalFlags &= ~OBJ_C_EdAddFlags_Highlighted;
					break;

				case QMFR_EDINFO_BR_OR_INTEGRATE:
					BIG_INDEX ul_GROIndex;
					
					if(iElem == 0)
					{
						ul_GROIndex = LOA_ul_SearchIndexWithAddress((ULONG) pst_Obj);

						if( !strcmp(BIG_P4HeadAction(ul_GROIndex), P4_ACTION_BRANCH)
							&& !(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceRTL))
						{
							ul_QMFREditorInfo = QMFR_EDINFO_COLOR_01;
							XeSetQMFREditorInfo(ulExtraFlags, QMFR_EDINFO_COLOR_01);	
							_pst_GO->ul_AdditionalFlags |= OBJ_C_EdAddFlags_Highlighted;
						}
						else if(!strcmp(BIG_P4HeadAction(ul_GROIndex), P4_ACTION_INTEGRATE)
								&& !(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceRTL))
						{
							ul_QMFREditorInfo = QMFR_EDINFO_COLOR_02;
							XeSetQMFREditorInfo(ulExtraFlags, QMFR_EDINFO_COLOR_02);
							_pst_GO->ul_AdditionalFlags |= OBJ_C_EdAddFlags_Highlighted;
						}
						else if(pst_Visu->ul_XenonMeshProcessingFlags & GRO_XMPF_ConsistencyFailed)
						{
							ul_QMFREditorInfo = QMFR_EDINFO_COLOR_03;
							XeSetQMFREditorInfo(ulExtraFlags, QMFR_EDINFO_COLOR_03);
							_pst_GO->ul_AdditionalFlags |= OBJ_C_EdAddFlags_Highlighted;
						}
						else
							_pst_GO->ul_AdditionalFlags &= ~OBJ_C_EdAddFlags_Highlighted;
					}
					else if(ul_QMFREditorInfo != 0)	//apply same color to each element of objects
					{
						XeSetQMFREditorInfo(ulExtraFlags, ul_QMFREditorInfo);
					}
					
					break;

				case QMFR_EDINFO_PIXEL_LIGHTED:
					if(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceRTL)
					{
						XeSetQMFREditorInfo(ulExtraFlags, QMFR_EDINFO_COLOR_01);
						_pst_GO->ul_AdditionalFlags |= OBJ_C_EdAddFlags_Highlighted;
					}
					else
					{
						_pst_GO->ul_AdditionalFlags &= ~OBJ_C_EdAddFlags_Highlighted;
					}
					break;

				case QMFR_EDINFO_CAST_REC_SHADOW:
					if((pst_Visu->ul_DrawMask & GDI_Cul_DM_EmitShadowBuffer) && (pst_Visu->ul_DrawMask & GDI_Cul_DM_ReceiveShadowBuffer))
					{
						//cast/receive
						XeSetQMFREditorInfo(ulExtraFlags, QMFR_EDINFO_COLOR_01);
						_pst_GO->ul_AdditionalFlags |= OBJ_C_EdAddFlags_Highlighted;
					}
					else if(pst_Visu->ul_DrawMask & GDI_Cul_DM_EmitShadowBuffer)
					{
						//cast
						XeSetQMFREditorInfo(ulExtraFlags, QMFR_EDINFO_COLOR_02);
						_pst_GO->ul_AdditionalFlags |= OBJ_C_EdAddFlags_Highlighted;
					}
					else if(pst_Visu->ul_DrawMask & GDI_Cul_DM_ReceiveShadowBuffer)
					{
						//receive
						XeSetQMFREditorInfo(ulExtraFlags, QMFR_EDINFO_COLOR_03);
						_pst_GO->ul_AdditionalFlags |= OBJ_C_EdAddFlags_Highlighted;
					}
					else
					{
						_pst_GO->ul_AdditionalFlags &= ~OBJ_C_EdAddFlags_Highlighted;
					}
					break;

				default:
					break;
				}
#endif

		        // for all material layers
                while (pst_MLTTXLVL && (LONG)pst_MLTTXLVL != -1)
                {
                    if(!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_InActive))
                    {
#ifdef XENONVIDEOSTATISTICS                                        
                        XeStats->AddStatElement(_pst_GO,pst_Visu->p_XeElements[iElem].pst_Mesh);                     
#endif

                        GEO_ComputeUV( _pst_GO, pst_MLTTXLVL );


                        g_oXeRenderer.QueueMeshForRender(   _pst_GO->pst_GlobalMatrix,
                                                            pst_Visu->p_XeElements[iElem].pst_Mesh,
                                                            pst_MLTTXLVL->pst_XeMaterial,
                                                            GDI_gpst_CurDD->ul_CurrentDrawMask,
                                                            ulLMTexID,
                                                            eRT,
                                                            XeRenderObject::TriangleList,
                                                            _pst_GO->uc_LOD_Vis,
							                                GDI_gpst_CurDD->g_cZListCurrentDisplayOrder,
                                                            _pst_GO,
                                                            pst_Obj->b_Particles ? XeRenderObject::Particles : XeRenderObject::Common,
                                                            0,
                                                            pst_Visu->p_XeElements[iElem].pst_Mesh->IsDynamic() ? ulExtraFlags : (ulExtraFlags | QMFR_GAO),
                                                            ulLayerIndex);

                        ++ulLayerIndex;
                    }

                    pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
                }
            }
        }
		else
			MAT_DrawIndexedTriangle(pst_Obj, (MAT_tdst_Material *) pst_Visu->pst_Material, pst_Element);

		GDI_gpst_CurDD->ul_CurrentDrawMask = ul_ObjectMask;
	}

	// sprites elements
	if (pst_Obj->l_NbSpritesElements)
	{
        CXBBeginEventObjectCPU oEvent(0, "Sprites");
        // Always use the color information when rendering sprites
        GDI_gpst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_UseRLI;

		GEO_tdst_ElementIndexedSprite		*pst_SpritesElements,*pst_LastSpritesElements;
		pst_SpritesElements = pst_Obj->dst_SpritesElements;
		pst_LastSpritesElements = pst_SpritesElements + pst_Obj->l_NbSpritesElements;
		ul_ObjectMask = GDI_gpst_CurDD->ul_CurrentDrawMask;

		for(; pst_SpritesElements < pst_LastSpritesElements; pst_SpritesElements++)
		{
			// Standard material
			MAT_DrawIndexedSprites(pst_Obj, (MAT_tdst_Material *) pst_Visu->pst_Material, pst_SpritesElements);
			GDI_gpst_CurDD->ul_CurrentDrawMask = ul_ObjectMask;
		}
	}

	// Additionnal materials (not supported yet on Xenon... it is used?)
	if(GDI_gpst_CurDD->pst_AdditionalMaterial)
	{
		OutputDebugString("\nNo additional material support on Xenon\n");
	}

	// Shadows receivers
	if ( (GDI_gpst_CurDD->pv_ShadowStack) && (((DD_tdst_ShadowStack *) GDI_gpst_CurDD->pv_ShadowStack)->ulNumberOfNodes))
	{
	}

	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
        CXBBeginEventObjectCPU oEvent(0, "MDF Unapply");

		// UnApply modifiers
		if (_pst_GO->pst_Extended->pst_Modifiers) 
			MDF_UnApplyAll(_pst_GO, 0);

		// AI
		if(_pst_GO->pst_Extended->pst_Ai)
		{
			AI_ExecCallback(_pst_GO, AI_C_Callback_AfterDisplay);
		}
		else if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)
		{
			OBJ_tdst_GameObject *pst_Ani = ANI_pst_GetReferenceInit(_pst_GO);
			if(pst_Ani && pst_Ani->pst_Extended && pst_Ani->pst_Extended->pst_Ai)
				AI_ExecCallback(pst_Ani, AI_C_Callback_AfterDisplay);
		}//*/
	}

	// computed colors pointer may have change during lighting, revert to static array
	GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors = GDI_gpst_CurDD->pst_ComputingBuffers->staticComputedColors;

    // Make sure the next object will not be influenced by our lights
    // light set must remain valid until end of georender because SPG2 modifier uses the same lightset
    g_pXeContextManagerEngine->ResetLastLightSet();

    // SC: Clear the current skinning set - Allows us to detect objects with weights in their
    //     vertex buffers, but no real need for skinning
    g_pXeContextManagerEngine->ClearSkinning();

	_GSP_EndRaster(23);

}

#endif
// ***************** JADE NORMAL *************************
#if !defined(_XENON)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" void WATER3D_Modifier_Display( OBJ_tdst_GameObject *_pst_GO );
#ifdef USE_GO_DATA
	u_int								ulPs2CacheFlags;
#endif
#if defined (ACTIVE_EDITORS) && !defined (JADEFUSION)
ULONG GEO_ulCurrentTriangleNb;
#endif //ACTIVE_EDITORS
#if (defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL)) && defined(_XENON_RENDER)
void GEO_Render_Default(OBJ_tdst_GameObject *_pst_GO)
#else
void GEO_Render(OBJ_tdst_GameObject *_pst_GO)
#endif
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Visu						*pst_Visu;
	GEO_tdst_Object						*pst_Obj;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	ULONG								ul_ObjectMask,ul_ValidityMask;
	register GDI_tdst_DisplayData 		*pst_DD;
#ifdef ACTIVE_EDITORS
	COL_tdst_Cob						*pst_Cob;
	BOOL								b_SelectedCob;
	u8									RLIloc;
#endif
	//extern int NbrObjectClone;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* TEST TEMP PHIL */
	
	

	/*$2--------------------------------------------------------------------------------------------------------------*/
	_GSP_BeginRaster(23);

	pst_DD = &GDI_gpst_CurDD_SPR;
	pst_Visu = _pst_GO->pst_Base->pst_Visu;
	pst_Obj = (GEO_tdst_Object *) pst_Visu->pst_Object;
	pst_DD->pst_CurrentGeo = pst_Obj;
	pst_DD->ul_DisplayInfo &= ~GDI_Cul_DI_ObjectHasBeenZAdded;
	ul_ValidityMask = MAT_GetValidityMASK((MAT_tdst_Material *) pst_Visu->pst_Material, -1, pst_Obj);
	
	//TESTVINCENT
#ifdef JADEFUSION
	if ( !(pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_DontForceSorted) )
#else
		if ( !(pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_DontSort) )
#endif
	{
		pst_DD->ul_DisplayInfo |= GDI_Cul_DI_DontSortObject;
		ul_ValidityMask &= ~MAT_ValidateMask_Transparency;
	}

#ifdef ACTIVE_EDITORS
	// RLI location
	GAODisplayFlag = 0x00000000;
	RLIloc = OBJ_i_RLIlocation(_pst_GO);
	if (RLIloc & u8_RLIinGAO)  GAODisplayFlag |= 0x000000ff;
	if (RLIloc >= u8_RLIinGRO) GAODisplayFlag |= 0x0000ff00;
	// Choose better colors :
	if (!GAODisplayFlag) GAODisplayFlag=0x00ffffff;
	if (GAODisplayFlag == 0x0000ffff) GAODisplayFlag=0x00ff0000;

#endif //ACTIVE_EDITORS

	if(!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_DontSortObject))
	{
		if
		(
			(
				(ul_ValidityMask & MAT_ValidateMask_Transparency) && (!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency))
			)
		)
		{
			SOFT_AddCurrentObjectInZList(pst_DD,_pst_GO);
			if (ul_ValidityMask & MAT_ValidateMask_Transparency_And) 
			return;

		}
	}
	
	// AI
	if(_pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Ai)
	{
		#ifdef _XBOX
		if(!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)&&
		   !(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingSpotSwadows)&&
		   !(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingDiffuse))
		#endif
		{
			AI_C_Callback = 1;
			AI_ExecCallback(_pst_GO, AI_C_Callback_BeforeDisplay);
			AI_C_Callback = 0;					
		}
	}
	else if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_Ani;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Ani = ANI_pst_GetReferenceInit(_pst_GO);
		if(pst_Ani && pst_Ani->pst_Extended && pst_Ani->pst_Extended->pst_Ai)
		{
			#ifdef _XBOX
			if(!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)&&
			   !(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingSpotSwadows)&&
			   !(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingDiffuse))
			#endif
			{
				AI_C_Callback = 1;				
				AI_ExecCallback(pst_Ani, AI_C_Callback_BeforeDisplay);
				AI_C_Callback = 0;
			}
		}
	}
	
	/*$2- MRM --------------------------------------------------------------------------------------------------------*/
	pst_DD->pus_ReorderBuffer = NULL;
    if (pst_Obj->p_MRM_Levels)
    {
        GEO_MRM_SetLevelFromQuality(pst_Obj,GEO_MRM_GetFloatFromUC((ULONG) _pst_GO->uc_LOD_Vis));
#ifdef ACTIVE_EDITORS
		extern BOOL OGL_gb_DispLOD;
		extern ULONG OGL_ulLODAmbient;
        if (OGL_gb_DispLOD)
        {
            OGL_ulLODAmbient = 0xFF000000 |
                (pst_Obj->p_MRM_Levels->ul_CurrentLevel & 1 ? 0x000000A0 : 0x00000040) |
                (pst_Obj->p_MRM_Levels->ul_CurrentLevel & 2 ? 0x0000A000 : 0x00004000) |
                (pst_Obj->p_MRM_Levels->ul_CurrentLevel & 4 ? 0x00A00000 : 0x00400000);
        }
        else 
            OGL_ulLODAmbient = 0;
#endif //ACTIVE_EDITORS
    }
	else if(GEO_MRM_ul_IsMrmObject(pst_Obj))
		GEO_MRM_SetNumberOfPoints_Percent(pst_Obj, GEO_MRM_GetFloatFromUC((ULONG) _pst_GO->uc_LOD_Vis));


	/*$2- Modifiers --------------------------------------------------------------------------------------------------*/
	pst_DD->ul_DisplayInfo &= ~GDI_Cul_DI_UseSpecialVertexBuffer;
	pst_DD->p_Current_Vertex_List = pst_Obj->dst_Point;
	if ( !pst_DD->p_Current_Vertex_List )
		return;

#ifndef _XBOX //NO XBOX CODE...NORMAL SKINNING


#ifdef USE_GO_DATA	
	ulPs2CacheFlags = GSP_AllColorsAreCached(_pst_GO , pst_Obj);
	if (!(ulPs2CacheFlags & GCF_AllStripXyzAreIn))
	{
		if (ulPs2CacheFlags & GCF_AllStripCCAreIn)
			GDI_gpst_CurDD ->ul_CurrentDrawMask |= GDI_Cul_DM_DontRecomputeNormales;
#endif	
		if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
		{
#ifdef ACTIVE_EDITORS
			if(!(pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCOB))
#endif
			if (_pst_GO->pst_Extended->pst_Modifiers)
	 			MDF_ApplyAll(_pst_GO, 0);
		}

		if(pst_Visu->ucFlag & GRO_VISU_FLAG_WATERFX)
		{
			WATER3D_Modifier_Display(_pst_GO);
			return;
		}

		/*$2- SKIN -------------------------------------------------------------------------------------------------------*/
		if(GEO_SKN_IsSkinned(pst_Obj) && (GDI_gpst_CurDD ->ul_CurrentDrawMask & GDI_Cul_DM_ActiveSkin))
		{
			GEO_SKN_Compute4Display(_pst_GO, pst_Obj);
		}

	    /*$2- recomputing normals ----------------------------------------------------------------------------------------*/
#ifndef PSX2_TARGET
#ifndef _XBOX
	   if ( !(GDI_gpst_CurDD ->ul_CurrentDrawMask & GDI_Cul_DM_DontRecomputeNormales) && (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer) )
		    GEO_SKN_ComputeNormals(pst_Obj, GDI_gpst_CurDD->p_Current_Vertex_List);
#endif
#endif
#ifdef USE_GO_DATA	
	} else
	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
		if (_pst_GO->pst_Extended->pst_Modifiers)
		{
#ifdef JADEFUSION
			if ((_pst_GO->pst_Extended->pst_Modifiers->i->ul_Type == MDF_C_Modifier_FUR) || 
                (_pst_GO->pst_Extended->pst_Modifiers->i->ul_Type == MDF_C_Modifier_DYNFUR))
#else
			if (_pst_GO->pst_Extended->pst_Modifiers->i->ul_Type == MDF_C_Modifier_FUR)
#endif
			{
				MDF_ApplyAll(_pst_GO, 0);
			}
		}
	}

#endif	


#else   //XBOX CODE
    if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
    {
        if (_pst_GO->pst_Extended->pst_Modifiers)
            MDF_ApplyAll(_pst_GO, 0);
    }

    if(GEO_SKN_IsSkinned(pst_Obj))
    {
        if(_pst_GO->playSkinning!=pst_DD->ul_RenderingCounter)
            GEO_SKN_Compute4Display(_pst_GO, pst_Obj);
    }

    {
        //HACK TiZ
        static bool ProcessComputeNormals = true;
        if(ProcessComputeNormals)
        {
            if ( !(GDI_gpst_CurDD ->ul_CurrentDrawMask & GDI_Cul_DM_DontRecomputeNormales) && (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer) )
                GEO_SKN_ComputeNormals(pst_Obj, GDI_gpst_CurDD->p_Current_Vertex_List);
        }
    }
#endif   //END XBOX CODE

    // Copy of vertex buffer in Col map if needed (skining & morph).
    if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer)
        GEO_CopyMeshToColMap(_pst_GO,pst_Obj);

    // Set fog
 	if ( !(pst_Visu->ul_DrawMask & GDI_Cul_DM_UseFog2) && pst_Visu->ul_DrawMask & GDI_Cul_DM_Fogged )
	{
		if (pst_DD->st_Fog2.c_Flag & SOFT_C_FogActive)
		pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_SetFogParams, (ULONG) & pst_DD->st_Fog2);
	}

	/*$2- SubObject --------------------------------------------------------------------------------------------------*/

	GEO_Render_SubObject(_pst_GO, pst_Obj);

    /*$2- Creation of a new geometry (editor feature) ----------------------------------------------------------------*/
    GEO_M_4Edit_Snapshot( _pst_GO, pst_Obj );
    
	/*$2- RLI --------------------------------------------------------------------------------------------------------*/

	_GSP_BeginRaster(8);

#if defined(_XBOX)
	//TURN OFF HW LIGHTS
	LIGHT_TurnOffObjectLighting();
#endif // defined(_XBOX)

#if !defined( ACTIVE_EDITORS )
	if ( ul_ValidityMask & ( MAT_ValidateMask_RLI | MAT_ValidateMask_Diffuse | MAT_ValidateMask_Specular_Color ) )
#endif
	{
#ifdef USE_GO_DATA
		if (!(ulPs2CacheFlags & GCF_AllStripCCAreIn))
		{
#endif

#ifdef _XBOX
		if(!p_gGx8SpecificData->diffuseInTexture && ! (pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap))
#endif
			GEO_Render_InitRLI(pst_DD , pst_Obj, pst_Visu);
			if (ul_ValidityMask & (MAT_ValidateMask_Diffuse	|MAT_ValidateMask_Specular_Color))
			{
#ifdef ACTIVE_EDITORS

				if(LINK_gi_SpeedDraw != 1)
	            {
	                if (LIGHT_gl_ComputeRLI)
	                {
	                    if ( pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_UnlockedRLI )
	                        LIGHT_SendObjectToLight(&pst_DD->st_LightList, _pst_GO);
	                }
	                else 
	                {
	                    if ( pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted)
					        LIGHT_SendObjectToLight(&pst_DD->st_LightList, _pst_GO);
	                }
	            }
#else

#	if defined(_XBOX)
		if(!p_gGx8SpecificData->diffuseInTexture && ! (pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)) //Light object only in no diffuse lighting
#	endif
		{


# if defined (_XBOX) //XBOX SPECIFIC LIGHTING CODE
				//HW LIGHTING
				//HW light doesn't support ALPHA LIGHT
				if(pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted)
				{
					u32 NewNewLight;
					NewNewLight = g_iNewLight;
					if (_pst_GO->pst_Extended)
					{
						MDF_tdst_Modifier  *pst_Modifier;
						extern void GAO_ModifierBoneRefineApply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
						pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
						while(pst_Modifier && NewNewLight)
						{
							if (pst_Modifier->i->ul_Type == MDF_C_Modifier_SpriteMapper2) 
							{
								NewNewLight = 0;
							}
							pst_Modifier = pst_Modifier->pst_Next;
						}
					}
					
					if(NewNewLight && (!(ul_ValidityMask & MAT_ValidateMask_RLIAlpha)) )
					{
						int numLight;
   
						//New...use HW to light the object if possible.
						numLight=LIGHT_SendObjectToLight_HW(&pst_DD->st_LightList, _pst_GO,ul_ValidityMask);

						//SOFTWARE LIGHTING FOR NOT POSSIBLE HW LIGHTING
						LIGHT_SendObjectToLightXB(&pst_DD->st_LightList, _pst_GO);
						GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UpdateRLI;
					}
					else
					{
						LIGHT_TurnOffObjectLighting();
						LIGHT_SendObjectToLight(&pst_DD->st_LightList, _pst_GO);	
						GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UpdateRLI;
					}
				}
				else
				{
					LIGHT_TurnOffObjectLighting();
				}

#else //NO XBOX
				if(pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted)
				{
					LIGHT_SendObjectToLight(&pst_DD->st_LightList, _pst_GO);
					GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UpdateRLI;
				}
#endif
		}
#endif
			
			}
#ifdef _XBOX
			//FINAL VERTEX COLOR
			if(!p_gGx8SpecificData->diffuseInTexture && ! (pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap))
			{
				LIGHT_ComputeVertexColor(pst_Obj->l_NbPoints);
			}

#else
			LIGHT_ComputeVertexColor(pst_Obj->l_NbPoints);
#endif

#ifdef USE_GO_DATA  //CARLONE INTEGRAZIONE
		} else
		{
			GEO_Render_InitRLI_NoDiffuse(pst_DD , pst_Obj, pst_Visu);
		}
#endif

	}  
	
	_GSP_EndRaster(8);

/*$2- COB ------------------------------------------------------------------------------------------------------------*/

#ifdef ACTIVE_EDITORS
	GEO_4Edit_ColMapOnly_BeforeDisplay(pst_DD, _pst_GO, &pst_Cob, &b_SelectedCob);
#endif

	/*$2--------------------------------------------------------------------------------------------------------------*/

#ifdef ACTIVE_EDITORS
    {
        extern BOOL OGL_bCountTriangles;
        extern u32 Stats_ulCallToDrawNb;
        if (OGL_bCountTriangles && (pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired))
            Stats_ulCallToDrawNb++;

        if (GDI_gpst_CurDD_SPR.SMALL_ALarm > 0)
        {
            // Count number of triangles.
		    pst_Element = pst_Obj->dst_Element;
		    pst_LastElement = pst_Element + pst_Obj->l_NbElements;
            GEO_ulCurrentTriangleNb = 0;
            for(; pst_Element < pst_LastElement; pst_Element++)
                GEO_ulCurrentTriangleNb += pst_Element->l_NbTriangles;
        }
    }
#endif



#if defined( _XBOX )    

	//Don't update colors if not needed
    if(p_gGx8SpecificData->diffuseInTexture || (pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap))
	{
		GDI_gpst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UpdateRLI;
	}

	//Don't forget to call this one ALWAYS before calling Gx8_UpdateModifiedObject
	Gx8_SetCurrentGameObject(_pst_GO);

	Gx8_SetMultipleVBIndexForUpdate( _pst_GO->pst_Base->pst_Visu->IndexInMultipleVBList );

#endif // defined(_XBOX)

    if (pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_UseNormalMaterial)
    {
	    if( (pst_DD->ul_DisplayInfo & GDI_Cul_DI_ForceSortTriangle) &&(!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency)) )
	    {
		    GEO_Render_TransparentTriangles(pst_DD , pst_Obj, pst_Visu);
	    }
	    else
	    {
			extern BOOL OK3_OptimOn;

		    pst_Element = pst_Obj->dst_Element;
		    pst_LastElement = pst_Element + pst_Obj->l_NbElements;
		    ul_ObjectMask = pst_DD->ul_CurrentDrawMask;

			{
				for(; pst_Element < pst_LastElement; pst_Element++)
				{
					M4Edit_ComputeElementColor

					/* Standard material */
					PRO_StartTrameRaster(&pst_DD->pst_Raster->st_GeoRender_DrawIndexedTriangles);

#ifdef ACTIVE_EDITORS
					if(pst_Cob && (pst_DD->uc_ColMapShowSlope || (GetAsyncKeyState(VK_MENU) < 0) || (_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowSlope)))
						MAT_DrawIndexedTriangle_ColMap(&GDI_gpst_CurDD_SPR, pst_Obj, (MAT_tdst_Material *) pst_Visu->pst_Material, pst_Element, _pst_GO->pst_GlobalMatrix);
					else

						MAT_DrawIndexedTriangle(pst_Obj, (MAT_tdst_Material *) pst_Visu->pst_Material, pst_Element);
#else
					MAT_DrawIndexedTriangle(pst_Obj, (MAT_tdst_Material *) pst_Visu->pst_Material, pst_Element);
#endif
					
					PRO_StopTrameRaster(&pst_DD->pst_Raster->st_GeoRender_DrawIndexedTriangles);
					GDI_gpst_CurDD_SPR.ul_CurrentDrawMask = ul_ObjectMask;

				}
			}

		    if (pst_Obj->l_NbSpritesElements)
		    {
			    GEO_tdst_ElementIndexedSprite		*pst_SpritesElements,*pst_LastSpritesElements;
			    pst_SpritesElements = pst_Obj->dst_SpritesElements;
			    pst_LastSpritesElements = pst_SpritesElements + pst_Obj->l_NbSpritesElements;
			    ul_ObjectMask = pst_DD->ul_CurrentDrawMask;

			    for(; pst_SpritesElements < pst_LastSpritesElements; pst_SpritesElements++)
			    {
				    // Standard material
				    MAT_DrawIndexedSprites(pst_Obj, (MAT_tdst_Material *) pst_Visu->pst_Material, pst_SpritesElements);
				    GDI_gpst_CurDD_SPR.ul_CurrentDrawMask = ul_ObjectMask;
			    }
		    }
	    }
    }

    /*$2- Additionnal materials --------------------------------------------------------------------------------------*/
#ifndef PSX2_TARGET // DBUG PRESOUM
#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if((pst_DD->pst_AdditionalMaterial) && (!NoADM))
#else
	if(pst_DD->pst_AdditionalMaterial)
#endif
	{
		/*~~~~~~~~~~~*/
		ULONG	SaveDI;
		/*~~~~~~~~~~~*/

		pst_Element = pst_Obj->dst_Element;
		pst_LastElement = pst_Element + pst_Obj->l_NbElements;

		ul_ObjectMask = pst_DD->ul_CurrentDrawMask;
		SaveDI = pst_DD->ul_DisplayInfo;

	    if(MAT_IsMaterialTransparent((MAT_tdst_Material *) pst_DD->pst_AdditionalMaterial, -1, ul_ObjectMask, NULL)) 
	        pst_DD->ul_DisplayInfo |= GDI_Cul_DI_RenderingTransparency;
	    else
	        pst_DD->ul_DisplayInfo &= ~GDI_Cul_DI_RenderingTransparency;

		for(; pst_Element < pst_LastElement; pst_Element++)
		{
			MAT_DrawIndexedTriangle
			(
				pst_Obj,
				(MAT_tdst_Material *) pst_DD->pst_AdditionalMaterial,
				pst_Element
			);
		}

		pst_DD->ul_CurrentDrawMask = ul_ObjectMask;
		pst_DD->ul_DisplayInfo = SaveDI;
	}
#endif

#ifdef ACTIVE_EDITORS
	GEO_RenderAddSup(_pst_GO, pst_Obj);
#endif

#ifndef PSX2_TARGET
	/*$2- Modifiers --------------------------------------------------------------------------------------------------*/
	if (_pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Modifiers) 
	{
		MDF_UnApplyAll(_pst_GO, 0);
	}
#endif	

	/*$2- Shadows ----------------------------------------------------------------------------------------------------*/
	if ( (pst_DD->pv_ShadowStack) && (((DD_tdst_ShadowStack *) pst_DD->pv_ShadowStack)->ulNumberOfNodes))
	{
	
#ifdef Active_CloneListe		
		//renderState_Cloned = 0;
#endif

#ifdef _XBOX
		if ( (!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)) && (!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingDiffuse))&& (!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingSpotSwadows)))
#endif
		{
		PRO_StartTrameRaster(&pst_DD->pst_Raster->st_RTShadowGeneration);
		SDW_DisplayShadowsOnCurrentObject(pst_Obj);
		PRO_StopTrameRaster(&pst_DD->pst_Raster->st_RTShadowGeneration);
		}
	}

	/* Water effects */
	PROTEX_UpdateEarthWindAndFire(GDI_gpst_CurDD ->pst_CurrentGameObject, pst_Obj);

	/*$2- AI callback after display ----------------------------------------------------------------------------------*/
	if(_pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Ai)
	{
		#ifdef _XBOX
		if(!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)&&
		   !(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingSpotSwadows)&&
		   !(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingDiffuse))
		#endif
		{
			AI_ExecCallback(_pst_GO, AI_C_Callback_AfterDisplay);
		}
	}
	else if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_Ani;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Ani = ANI_pst_GetReferenceInit(_pst_GO);
		
		if(pst_Ani && pst_Ani->pst_Extended && pst_Ani->pst_Extended->pst_Ai)
		{
			#ifdef _XBOX
			if(!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)&&
			   !(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingSpotSwadows)&&
			   !(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingDiffuse))
			#endif
			{
				AI_ExecCallback(pst_Ani, AI_C_Callback_AfterDisplay);
			}
		}
	}

	/*$2- Sub object -------------------------------------------------------------------------------------------------*/

	GEO_Render_SubObjectEnd(_pst_GO, pst_Obj);
	
#ifdef _XBOX

	//Disable lighting after draw.
	LIGHT_TurnOffObjectLighting();

#endif

    // End of MRM level rendering
    if (pst_Obj->p_MRM_Levels)
        GEO_MRM_SetAllLevels(pst_Obj);


		// Back to First Fog
		BackToFirstFog(pst_DD,pst_Visu);
#ifdef Active_CloneListe		
		renderState_Cloned = 0;
#endif

	_GSP_EndRaster(23);
}

//***************** FIN DU RENDER JADE NORMAL ************
#endif // !defined(_XENON)popoverif


#ifdef ACTIVE_EDITORS

BOOL GEO_bHasTransparency(COL_tdst_Cob *pst_Cob,GEO_tdst_Object *pst_Obj)
{
	COL_tdst_GameMat *pst_GMat = NULL;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;

	if(pst_Cob)
	{
		if(pst_Cob->pst_TriangleCob)
        {
		    pst_Element = pst_Obj->dst_Element;
		    pst_LastElement = pst_Element + pst_Obj->l_NbElements;

            for(; pst_Element < pst_LastElement; pst_Element++)
            {
    			pst_GMat = COL_pst_GMat_Get(pst_Cob, pst_Cob->pst_TriangleCob->dst_Element + (pst_Element - pst_Obj->dst_Element));
                if (pst_GMat && pst_GMat->b_Transparent)
                    return TRUE;
            }
        }
		else
			pst_GMat = COL_pst_GMat_Get(pst_Cob, NULL);
	}
    return (pst_GMat && pst_GMat->b_Transparent);
}

void GEO_Render_TransparentTrianglesZone(GDI_tdst_DisplayData *_pst_DD ,
                                         GEO_tdst_Object *pst_Obj, 
                                         GRO_tdst_Visu *pst_Visu,
                                         OBJ_tdst_GameObject *_pst_GO,	
                                         COL_tdst_Cob *pst_Cob,
                                         BOOL b_SelectedCob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector					*pst_V1, *pst_V2, *pst_V3;
	GEO_Vertex							*pst_VertexList;
	GEO_tdst_IndexedTriangle			*pst_Triangle, *pst_LastTriangle;
	float								f_Z;
	SOFT_tdst_ZList_Node				*pst_Node;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	ULONG								ul_ObjectMask;
	MAT_tdst_Material					*p_MaterialToUse;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef _DEBUG
	if(pst_Obj->l_NbPoints > MaxTransparentT)
		MaxTransparentT = pst_Obj->l_NbPoints;		
#endif

    if (pst_Obj->l_NbPoints > SOFT_Cul_ComputingBufferSize) 
    {
        ERR_X_Warning(0, "The GFX size exceeds the RAM capacity", NULL);
        return;
    }
    
	pst_Element = pst_Obj->dst_Element;
	pst_LastElement = pst_Element + pst_Obj->l_NbElements;
	ul_ObjectMask = _pst_DD->ul_CurrentDrawMask;

	SOFT_TransformInComputingBuffer
	(
		_pst_DD->pst_ComputingBuffers,
		_pst_DD->p_Current_Vertex_List,
		pst_Obj->l_NbPoints,
		&_pst_DD->st_Camera
	);
		
	for(; pst_Element < pst_LastElement; pst_Element++)
	{
		PRO_IncRasterLong(&_pst_DD->pst_Raster->st_NbTriangles, pst_Element->l_NbTriangles);

		if(pst_Cob)
		{
			if(GEO_4Edit_ColMapOnly_ComputeElementColor(_pst_DD, _pst_GO, pst_Cob, pst_Obj, pst_Element, b_SelectedCob)) 
                continue;
		}
		else if ( (_pst_DD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_ColorModeMask) == GRO_Cul_EOF_ColorModeFaceID )
		{
			ULONG	ul_SaveDM, ul_SaveCC;
			ul_SaveDM = _pst_DD->ul_CurrentDrawMask;
			ul_SaveCC = _pst_DD->ul_ColorConstant;
			_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontForceColor;
			_pst_DD->ul_ColorConstant |= _pst_DD->pst_EditOptions->ul_FaceIDColor[pst_Element->l_MaterialId % 32];
			LIGHT_ComputeVertexColor(pst_Obj->l_NbPoints);
			_pst_DD->ul_CurrentDrawMask = ul_SaveDM;
			_pst_DD->ul_ColorConstant = ul_SaveCC;
		}

		/* No multitexture & transparency -> Classic algorithm */
		p_MaterialToUse = GEO_GetMaterial((MAT_tdst_Material *) pst_Visu->pst_Material, pst_Element->l_MaterialId);

		pst_VertexList = _pst_DD->pst_ComputingBuffers->ast_3D;

		pst_Triangle = pst_Element->dst_Triangle;
		pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

		for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
		{
			pst_V1 = VCast(pst_VertexList + pst_Triangle->auw_Index[0]);
			pst_V2 = VCast(pst_VertexList + pst_Triangle->auw_Index[1]);
			pst_V3 = VCast(pst_VertexList + pst_Triangle->auw_Index[2]);

			f_Z = pst_V1->z;
			f_Z = fMax(f_Z , pst_V2->z);
			f_Z = fMax(f_Z , pst_V3->z);
			
			if (f_Z > 0.0f)
			{
				f_Z = fOptInv(f_Z);

				if((pst_Node = SOFT_pst_ZList_AddNode(f_Z, 3, SOFT_l_TON_Vertices)) != NULL)
				{
					*((MATH_tdst_Vector *) &pst_Node->pst_Vertices[0]) = *pst_V1;
					*((MATH_tdst_Vector *) &pst_Node->pst_Vertices[1]) = *pst_V2;
					*((MATH_tdst_Vector *) &pst_Node->pst_Vertices[2]) = *pst_V3;
					if(pst_Obj->dst_UV)
					{
						pst_Node->pst_Vertices[0].u = pst_Obj->dst_UV[pst_Triangle->auw_UV[0]].fU;
						pst_Node->pst_Vertices[0].v = pst_Obj->dst_UV[pst_Triangle->auw_UV[0]].fV;
						pst_Node->pst_Vertices[1].u = pst_Obj->dst_UV[pst_Triangle->auw_UV[1]].fU;
						pst_Node->pst_Vertices[1].v = pst_Obj->dst_UV[pst_Triangle->auw_UV[1]].fV;
						pst_Node->pst_Vertices[2].u = pst_Obj->dst_UV[pst_Triangle->auw_UV[2]].fU;
						pst_Node->pst_Vertices[2].v = pst_Obj->dst_UV[pst_Triangle->auw_UV[2]].fV;
					}

					pst_Node->pst_Vertices[0].color = _pst_DD->pst_ComputingBuffers->ComputedColors[pst_Triangle->auw_Index[0]];
					pst_Node->pst_Vertices[1].color = _pst_DD->pst_ComputingBuffers->ComputedColors[pst_Triangle->auw_Index[1]];
					pst_Node->pst_Vertices[2].color = _pst_DD->pst_ComputingBuffers->ComputedColors[pst_Triangle->auw_Index[2]];

					pst_Node->pst_Material = (MAT_tdst_MultiTexture *)p_MaterialToUse;
					pst_Node->l_DrawMask = _pst_DD->ul_CurrentDrawMask;
				}
			}
		}

		_pst_DD->ul_CurrentDrawMask = ul_ObjectMask;
	}
}

// sigh...
extern void MAT_DrawIndexedTriangleZone(
        GEO_tdst_Object *pst_Obj,
        GEO_tdst_ElementIndexedTriangles *pst_Element,
        BOOL bHasTransparency );

void GEO_RenderZone(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Visu						*pst_Visu;
	GEO_tdst_Object						*pst_Obj;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	ULONG								ul_ObjectMask,ul_ValidityMask;
	register GDI_tdst_DisplayData 		*pst_DD;
	COL_tdst_Cob						*pst_Cob;
	BOOL								b_SelectedCob;
    BOOL								bHasTransparency;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2--------------------------------------------------------------------------------------------------------------*/
	_GSP_BeginRaster(23);

	pst_DD = &GDI_gpst_CurDD_SPR;
	pst_Visu = _pst_GO->pst_Base->pst_Visu;
	pst_Obj = (GEO_tdst_Object *) pst_Visu->pst_Object;
	pst_DD->pst_CurrentGeo = pst_Obj;
	pst_DD->ul_DisplayInfo &= ~GDI_Cul_DI_ObjectHasBeenZAdded;
	
	ul_ValidityMask = MAT_GetValidityMASK((MAT_tdst_Material *) pst_Visu->pst_Material, -1, pst_Obj);

	GEO_4Edit_ColMapOnly_BeforeDisplay(pst_DD, _pst_GO, &pst_Cob, &b_SelectedCob);

    bHasTransparency = GEO_bHasTransparency(pst_Cob,pst_Obj);
	
	//TESTVINCENT
#ifdef JADEFUSION
	if ( !(pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_DontForceSorted) )
#else
	if ( !(pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_DontSort) )
#endif
		ul_ValidityMask &= ~MAT_ValidateMask_Transparency;

	if(!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_DontSortObject))
	{
        if ((bHasTransparency || (ul_ValidityMask & MAT_ValidateMask_Transparency))
            && (!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency)))
        {
			SOFT_AddCurrentObjectInZList(pst_DD,_pst_GO);
			if (ul_ValidityMask & MAT_ValidateMask_Transparency_And) 
                return;
		}
	}
	

	// AI
	if(_pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Ai)
	{
        AI_C_Callback = 1;
        AI_ExecCallback(_pst_GO, AI_C_Callback_BeforeDisplay);
        AI_C_Callback = 0;					
	}

	
	/*$2- Modifiers --------------------------------------------------------------------------------------------------*/
	pst_DD->ul_DisplayInfo &= ~GDI_Cul_DI_UseSpecialVertexBuffer;
	pst_DD->p_Current_Vertex_List = pst_Obj->dst_Point;
	if ( !pst_DD->p_Current_Vertex_List ) 
		return;

	/*$2- SubObject --------------------------------------------------------------------------------------------------*/

	GEO_Render_SubObject(_pst_GO, pst_Obj);

    /*$2- Creation of a new geometry (editor feature) ----------------------------------------------------------------*/
    GEO_M_4Edit_Snapshot( _pst_GO, pst_Obj );
    
	/*$2- RLI --------------------------------------------------------------------------------------------------------*/

	_GSP_BeginRaster(8);

    GEO_Render_InitRLI(pst_DD , pst_Obj, pst_Visu);
    if (ul_ValidityMask & (MAT_ValidateMask_Diffuse	|MAT_ValidateMask_Specular_Color))
    {

        if(LINK_gi_SpeedDraw != 1)
        {
            if (LIGHT_gl_ComputeRLI)
            {
                if ( pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_UnlockedRLI )
                    LIGHT_SendObjectToLight(&pst_DD->st_LightList, _pst_GO);
            }
            else 
            {
                if ( pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted)
                    LIGHT_SendObjectToLight(&pst_DD->st_LightList, _pst_GO);
            }
        }

    }
    LIGHT_ComputeVertexColor(pst_Obj->l_NbPoints);
	
	_GSP_EndRaster(8);

/*$2- COB ------------------------------------------------------------------------------------------------------------*/


	/*$2--------------------------------------------------------------------------------------------------------------*/
    if (pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_UseNormalMaterial)
    {
	    if ( (pst_DD->ul_DisplayInfo & GDI_Cul_DI_ForceSortTriangle) &&(!(pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency)) )
	    {
		    GEO_Render_TransparentTrianglesZone(pst_DD , pst_Obj, pst_Visu,_pst_GO,pst_Cob, b_SelectedCob);
	    }
	    else
	    {
			extern BOOL OK3_OptimOn;

		    pst_Element = pst_Obj->dst_Element;
		    pst_LastElement = pst_Element + pst_Obj->l_NbElements;
		    ul_ObjectMask = pst_DD->ul_CurrentDrawMask;

			{
				for(; pst_Element < pst_LastElement; pst_Element++)
				{
					M4Edit_ComputeElementColor

					/* Standard material */
					PRO_StartTrameRaster(&pst_DD->pst_Raster->st_GeoRender_DrawIndexedTriangles);

					if(pst_Cob && (pst_DD->uc_ColMapShowSlope || (GetAsyncKeyState(VK_MENU) < 0) || (_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowSlope)))
						MAT_DrawIndexedTriangle_ColMap(&GDI_gpst_CurDD_SPR, pst_Obj, (MAT_tdst_Material *) pst_Visu->pst_Material, pst_Element, _pst_GO->pst_GlobalMatrix);
#ifndef JADEFUSION //POPOWARNING a finir
					else
						MAT_DrawIndexedTriangleZone(pst_Obj, pst_Element,bHasTransparency);
#endif
					PRO_StopTrameRaster(&pst_DD->pst_Raster->st_GeoRender_DrawIndexedTriangles);
					GDI_gpst_CurDD_SPR.ul_CurrentDrawMask = ul_ObjectMask;

				}
			}
	    }
    }

	GEO_RenderAddSup(_pst_GO, pst_Obj);

	/*$2- Modifiers --------------------------------------------------------------------------------------------------*/
	if (_pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Modifiers) 
	{
		MDF_UnApplyAll(_pst_GO, 0);
	}

	/*$2- Shadows ----------------------------------------------------------------------------------------------------*/
	if ( (pst_DD->pv_ShadowStack) && (((DD_tdst_ShadowStack *) pst_DD->pv_ShadowStack)->ulNumberOfNodes))
	{
		{
		PRO_StartTrameRaster(&pst_DD->pst_Raster->st_RTShadowGeneration);
		SDW_DisplayShadowsOnCurrentObject(pst_Obj);
		PRO_StopTrameRaster(&pst_DD->pst_Raster->st_RTShadowGeneration);
		}
	}

	/* Water effects */
	PROTEX_UpdateEarthWindAndFire(GDI_gpst_CurDD ->pst_CurrentGameObject, pst_Obj);

	/*$2- AI callback after display ----------------------------------------------------------------------------------*/
	if(_pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Ai)
	{
        AI_ExecCallback(_pst_GO, AI_C_Callback_AfterDisplay);
	}


	/*$2- Sub object -------------------------------------------------------------------------------------------------*/

	GEO_Render_SubObjectEnd(_pst_GO, pst_Obj);

	_GSP_EndRaster(23);
}


int     GEO_gi_CreateFromMad_DoClean = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_p_CreateObjectFromMad(MAD_GeometricObject *_pst_MadObject)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object				*pst_Object;
	MAD_Vertex					*pst_MadVertex;
	MAD_Simple3DVertex			*pst_Mad3DVertex;
	LONG						lIndex, lIndex2, lIndex3;
	MAD_GeometricObjectElement	*pst_MadElement;
	MAD_Face					*pst_MadFace;
	GEO_tdst_IndexedTriangle	st_ITriangle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Test if RLI are new zealanders */
	if((_pst_MadObject->Colors) && (!_pst_MadObject->SecondRLIField))
	{
		for(lIndex = 0; lIndex < (LONG) _pst_MadObject->NumberOfPoints; lIndex++)
		{
			if(_pst_MadObject->Colors[lIndex]) break;
		}

		if(lIndex == (LONG) _pst_MadObject->NumberOfPoints) _pst_MadObject->Colors = NULL;
	}

    /* test data */
    {
        char sz_Text[ 512 ];
        if (_pst_MadObject->NumberOfPoints > SOFT_Cul_ComputingBufferSize )
        {
            sprintf( sz_Text, "%s a trop de points (%d > %d)", _pst_MadObject->ID.Name, _pst_MadObject->NumberOfPoints, SOFT_Cul_ComputingBufferSize );
            LINK_PrintStatusMsg( sz_Text );
        }
        if (_pst_MadObject->NumberOfUV > SOFT_Cul_ComputingBufferSize )
        {
            sprintf( sz_Text, "%s a trop d'UV (%d > %d)", _pst_MadObject->ID.Name, _pst_MadObject->NumberOfUV, SOFT_Cul_ComputingBufferSize );
            LINK_PrintStatusMsg( sz_Text );
        }
    }

	lIndex = _pst_MadObject->Colors ? _pst_MadObject->NumberOfPoints : 0;
	lIndex += _pst_MadObject->SecondRLIField ? _pst_MadObject->NumberOfPoints : 0;
	pst_Object = GEO_pst_Create(
		_pst_MadObject->NumberOfPoints,
		_pst_MadObject->NumberOfUV,
		_pst_MadObject->NumberOfElements,
		lIndex);
	GRO_Struct_SetName(&pst_Object->st_Id, _pst_MadObject->ID.Name);

	pst_Mad3DVertex = _pst_MadObject->OBJ_PointList;
	for(lIndex = 0; lIndex < (LONG) _pst_MadObject->NumberOfPoints; lIndex++, pst_Mad3DVertex++)
	{
		GEO_SetPoint(
			pst_Object,
			lIndex,
			pst_Mad3DVertex->Point.x,
			pst_Mad3DVertex->Point.y,
			pst_Mad3DVertex->Point.z,
			pst_Mad3DVertex->Normale.x,
			pst_Mad3DVertex->Normale.y,
			pst_Mad3DVertex->Normale.z);
		if(_pst_MadObject->Colors)
		{
			pst_Object->dul_PointColors[lIndex + 1] = _pst_MadObject->Colors[lIndex];
		}

		if(_pst_MadObject->SecondRLIField)
		{
			pst_Object->dul_PointColors[lIndex + 1 + _pst_MadObject->NumberOfPoints] = _pst_MadObject->SecondRLIField[lIndex];
		}
	}

	pst_MadVertex = _pst_MadObject->OBJ_UVtextureList;
	for(lIndex = 0; lIndex < (LONG) _pst_MadObject->NumberOfUV; lIndex++, pst_MadVertex++)
	{
		GEO_SetUV(pst_Object, lIndex, pst_MadVertex->x, pst_MadVertex->y);
	}

	for(lIndex = 0; lIndex < (LONG) _pst_MadObject->NumberOfElements; lIndex++)
	{
		pst_MadElement = _pst_MadObject->Elements[lIndex];
		GEO_SetElement(pst_Object, lIndex, pst_MadElement->MaterialID, pst_MadElement->NumberOfTriangles);

		pst_MadFace = pst_MadElement->Faces;
		for(lIndex2 = 0; lIndex2 < (LONG) pst_MadElement->NumberOfTriangles; lIndex2++, pst_MadFace++)
		{
			for(lIndex3 = 0; lIndex3 < 3; lIndex3++)
			{
				if(pst_MadFace->Index[lIndex3] >= _pst_MadObject->NumberOfPoints)
					pst_MadFace->Index[lIndex3] = _pst_MadObject->NumberOfPoints - 1;
				st_ITriangle.auw_Index[lIndex3] = (unsigned short) pst_MadFace->Index[lIndex3];

				if(pst_MadFace->UVIndex[lIndex3] >= _pst_MadObject->NumberOfUV)
					pst_MadFace->UVIndex[lIndex3] = _pst_MadObject->NumberOfUV - 1;
				st_ITriangle.auw_UV[lIndex3] = (unsigned short) pst_MadFace->UVIndex[lIndex3];
			}

			st_ITriangle.ul_SmoothingGroup = pst_MadFace->SmoothingGroup;
			st_ITriangle.ul_MaxFlags = pst_MadFace->MAXflags;
			GEO_SetTriangle(&pst_Object->dst_Element[lIndex], lIndex2, &st_ITriangle);
		}
	}

	/*
	 * This computation is now done n the OBJ module to get the Absolute Matrix needed
	 * ... GEO_ComputeBoundingVolume(pst_Object, 0);
	 */
	GEO_ComputeNormals(pst_Object);
    if (GEO_gi_CreateFromMad_DoClean)
    {
        GEO_Clean_XYZ_UV(NULL , pst_Object , NULL, FALSE , TRUE);
        GEO_Clean( NULL, pst_Object, NULL );
        GEO_gi_CreateFromMad_DoClean = 0;
    }

	return pst_Object;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_p_CreateSkinnedObjectFromMad(MAD_SkinnedGeometricObject *_pst_MadObject)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object                 *pst_Object;
	GEO_tdst_ObjectPonderation      *pst_ObjPond;
	GEO_tdst_VertexPonderationList  *pst_PondList;
	unsigned long                   i;
	unsigned long                   j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Object = GEO_p_CreateObjectFromMad(_pst_MadObject->pst_GeoObj);
	pst_ObjPond = (GEO_tdst_ObjectPonderation*)MEM_GEO_p_Alloc(sizeof (GEO_tdst_ObjectPonderation));
	// VERIF with philippe
	pst_ObjPond->flags = 0;
	pst_ObjPond->PushStack = NULL;
	pst_ObjPond->REDOStack = NULL;
	pst_ObjPond->dul_PointColors = NULL;
	pst_ObjPond->ClassPtr = NULL;
	pst_ObjPond->SelectionCLBK = NULL;
	// !VERIF
	pst_ObjPond->NumberPdrtLists = _pst_MadObject->us_NumberOfPonderationLists;
	pst_ObjPond->pp_PdrtLst = (GEO_tdst_VertexPonderationList**)MEM_GEO_p_Alloc(sizeof (GEO_tdst_VertexPonderationList *) * pst_ObjPond->NumberPdrtLists);
	for (i = 0; i < pst_ObjPond->NumberPdrtLists; ++i)
	{
		pst_PondList = (GEO_tdst_VertexPonderationList*)MEM_GEO_p_Alloc(sizeof (GEO_tdst_VertexPonderationList));
		pst_PondList->st_FlashedMatrix.Ix = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.I.x;
		pst_PondList->st_FlashedMatrix.Iy = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.I.y;
		pst_PondList->st_FlashedMatrix.Iz = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.I.z;
		pst_PondList->st_FlashedMatrix.Jx = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.J.x;
		pst_PondList->st_FlashedMatrix.Jy = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.J.y;
		pst_PondList->st_FlashedMatrix.Jz = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.J.z;
		pst_PondList->st_FlashedMatrix.Kx = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.K.x;
		pst_PondList->st_FlashedMatrix.Ky = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.K.y;
		pst_PondList->st_FlashedMatrix.Kz = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.K.z;
		pst_PondList->st_FlashedMatrix.T.x = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.Translation.x;
		pst_PondList->st_FlashedMatrix.T.y = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.Translation.y;
		pst_PondList->st_FlashedMatrix.T.z = _pst_MadObject->pst_PonderationList[i].st_FlashedMatrix.Translation.z;
		pst_PondList->st_FlashedMatrix.Sx = 0.0f;
		pst_PondList->st_FlashedMatrix.Sy = 0.0f;
		pst_PondList->st_FlashedMatrix.Sz = 0.0f;
		pst_PondList->st_FlashedMatrix.w = 1.0f;
		MATH_SetCorrectType(&pst_PondList->st_FlashedMatrix);
		pst_PondList->us_IndexOfMatrix = _pst_MadObject->pst_PonderationList[i].us_MatrixIdx - 1;
		pst_PondList->us_NumberOfPonderatedVertices = _pst_MadObject->pst_PonderationList[i].us_NumberOfPonderatedVertices;
		pst_PondList->p_PdrtVrc_C = (GEO_tdst_CompressedVertexPonderation*)MEM_GEO_p_Alloc(sizeof (GEO_tdst_CompressedVertexPonderation) * pst_PondList->us_NumberOfPonderatedVertices);
		for (j = 0; j < pst_PondList->us_NumberOfPonderatedVertices; ++j)
		{
			pst_PondList->p_PdrtVrc_C[j].Index = _pst_MadObject->pst_PonderationList[i].p_CmpPdrtVrt[j].Index;
			pst_PondList->p_PdrtVrc_C[j].Ponderation = _pst_MadObject->pst_PonderationList[i].p_CmpPdrtVrt[j].Ponderation;
		}
		pst_ObjPond->pp_PdrtLst[i] = pst_PondList;
	}
	pst_Object->p_SKN_Objectponderation = pst_ObjPond;
	return pst_Object;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_p_CreateFromMad(MAD_NodeID *_pst_MadObject)
{
	switch (_pst_MadObject->IDType)
	{
	case ID_MAD_GeometricObject_V0:
		return GEO_p_CreateObjectFromMad((MAD_GeometricObject *)_pst_MadObject);
	case ID_MAD_SkinnedGeometricObject:
		return GEO_p_CreateSkinnedObjectFromMad((MAD_SkinnedGeometricObject *)_pst_MadObject);
	}
  return NULL;
}

#endif /* ACTIVE_EDITORS */

extern "C" void COL_Save_Nodes_Recursively(COL_tdst_OK3_Node *, COL_tdst_OK3 *);

#ifdef ACTIVE_EDITORS

void GEO_v_SaveElementsInBuffer(ULONG l_NbElements, GEO_tdst_ElementIndexedTriangles *_pst_Elements, ULONG *_pulStripFlag)
{
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	GEO_tdst_OneStrip					*pStrip, *pLastStrip;
	LONG								ElementInfo[8],l_Size;

	pst_Element = _pst_Elements;
	pst_LastElement = pst_Element + l_NbElements;

	l_Size = 8;
	while(l_Size--) ElementInfo[l_Size] = 0;


	for(; pst_Element < pst_LastElement; pst_Element++)
	{
		SAV_Buffer(&pst_Element->l_NbTriangles, 4);
		SAV_Buffer(&pst_Element->l_MaterialId, 4);
#if defined(XML_CONV_TOOL)
		void* val = gMrmElementAdditionalInfoPtrList.front();
		gMrmElementAdditionalInfoPtrList.pop_front();
		SAV_Buffer(&val, 4);
#else
		SAV_Buffer(&pst_Element->p_MrmElementAdditionalInfo, 4);
#endif
		SAV_Buffer(&pst_Element->ul_NumberOfUsedIndex, 4);
		SAV_Buffer(ElementInfo, (LONG) 4 * (LONG) 6);
	}

	for(pst_Element = _pst_Elements; pst_Element < pst_LastElement; pst_Element++)
	{
		l_Size = sizeof(GEO_tdst_IndexedTriangle) * pst_Element->l_NbTriangles;
		SAV_Buffer(pst_Element->dst_Triangle, l_Size);
		if(pst_Element->p_MrmElementAdditionalInfo)
		{
			SAV_Buffer(pst_Element->p_MrmElementAdditionalInfo, sizeof(GEO_tdst_MRM_Element));
		}

		if(pst_Element->ul_NumberOfUsedIndex)
		{
			SAV_Buffer(pst_Element->pus_ListOfUsedIndex, sizeof(unsigned short) * pst_Element->ul_NumberOfUsedIndex);
		}
	}

	/* *** strip data saving begin *** */

	/* save the strip data flag */
    SAV_Buffer(_pulStripFlag, sizeof(ULONG));

	/* if strip data valid -> save all strip data */
	if((*_pulStripFlag) & GEO_C_Strip_DataValid)
	{
		for(pst_Element = _pst_Elements; pst_Element < pst_LastElement; pst_Element++)
		{
			SAV_Buffer(&pst_Element->pst_StripData->ulFlag, sizeof(ULONG));
			SAV_Buffer(&pst_Element->pst_StripData->ulStripNumber, sizeof(ULONG));

			pStrip = pst_Element->pst_StripData->pStripList;
			pLastStrip = pStrip + pst_Element->pst_StripData->ulStripNumber;
			for(; pStrip < pLastStrip; pStrip++)
			{
				pStrip->ulVertexNumber |= 0x80000000;
				SAV_Buffer(&pStrip->ulVertexNumber, sizeof(ULONG));
				pStrip->ulVertexNumber &= ~0x80000000;
				SAV_Buffer(pStrip->pMinVertexDataList, sizeof(GEO_tdst_MinVertexData) * pStrip->ulVertexNumber);
			}
		}
	}
}
#endif //ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" LONG GEO_l_SaveInBuffer(GEO_tdst_Object *_pst_Object, void *p_Unused)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	LONG								i, bHasIndirection, bHasMRMLevels,l_Size, lTemp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // Elements should have full detail.
	GEO_MRM_RESET(_pst_Object);

	GRO_Struct_Save(&_pst_Object->st_Id);
 
#if defined(XML_CONV_TOOL)
	if (gGeoHasCode2002)
	{
		// Insert version number and version specific data
		lTemp = 0xC0DE2002;
		SAV_Buffer(&lTemp, 4);//popoverif

    // Version number (increment if you change format)
    //lTemp = 4; 
		SAV_Buffer(&gGeoVersion, 4);
	}
#else
    // Insert version number and version specific data
    lTemp = 0xC0DE2002;
	SAV_Buffer(&lTemp, 4);

    // Version number (increment if you change format)
    lTemp = 4; 
	SAV_Buffer(&lTemp, 4);
#endif

	/* Get structure content */
    // Point number
	SAV_Buffer(&_pst_Object->l_NbPoints, 4);

    // Warning if too many vertices/triangles
#	if 0// hogsy: removed per https://github.com/OldTimes-Software/jaded/issues/26
    if(_pst_Object->st_Id.i->ul_Type == GRO_Geometric)
    {
        LONG    l_Nb;
        char    sz_Msg[256];
        if ( _pst_Object->l_NbPoints > 5000 )
        {
            sprintf( sz_Msg, "GRO has more than 5000 vertex (%d)", _pst_Object->l_NbPoints );
            ERR_X_Warning(0,sz_Msg,_pst_Object->st_Id.sz_Name);
        }

        l_Nb = GEO_l_GetNumberOfTriangles( _pst_Object );
        if ( l_Nb > 3000 )
        {
            sprintf( sz_Msg, "GRO has more than 3000 triangles (%d)", l_Nb );
            ERR_X_Warning(0,sz_Msg,_pst_Object->st_Id.sz_Name);
        }
    }
#	endif
#if defined(XML_CONV_TOOL)
	if (gGeoHasCode2002)
	{
#endif
    
    bHasMRMLevels = (_pst_Object->p_MRM_Levels != NULL);
	SAV_Buffer(&bHasMRMLevels, 4);

    if (bHasMRMLevels)
    {
        // Indirections
        bHasIndirection = _pst_Object->p_MRM_Levels->dus_ReorderBuffer && !(EDI_gb_ComputeMap || LOA_gb_SpeedMode);
        SAV_Buffer(&bHasIndirection, 4);
    }

#if defined(XML_CONV_TOOL)
	}
	else
	{
		bHasMRMLevels = 0;
	}
#endif
	if(_pst_Object->dul_PointColors)
		SAV_Buffer(_pst_Object->dul_PointColors, 4);
	else
		SAV_Buffer(&_pst_Object->dul_PointColors, 4);

	SAV_Buffer(&_pst_Object->l_NbUVs, 4);
	SAV_Buffer(&_pst_Object->l_NbElements, 4);
	SAV_Buffer(&_pst_Object->ul_EditorFlags, 4);

	/* Save MRM */
	{
		/*~~~~~~~~~~~~~~~~~*/
		ULONG	ulSavedValue;
		/*~~~~~~~~~~~~~~~~~*/

		if(!_pst_Object->p_MRM_ObjectAdditionalInfo)
			ulSavedValue = 0;
		else if(_pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer)
			ulSavedValue = 0xC0DE0008;
		else
			ulSavedValue = 0xC0DE0007;
		SAV_Buffer(&ulSavedValue, 4);
	}

	l_Size = 0;

#ifdef KER_IS_NOT_A_CHEVRE
	if(EDI_gb_ComputeMap && _pst_Object->pst_OK3)
		l_Size |= 0x1;
#endif

	/* Swap dummy data :4 bytes */
	if(_pst_Object->p_SKN_Objectponderation)
		l_Size |= 0xC0DE2002;

	SAV_Buffer(&l_Size, 4);

	/* Save Skin if exist */
	if((ULONG) _pst_Object->p_SKN_Objectponderation) GEO_SKN_Save(_pst_Object);

#ifdef KER_IS_NOT_A_CHEVRE
	/* Save OK3 if exist */
	if(EDI_gb_ComputeMap && _pst_Object->pst_OK3) 
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_OK3_Box		*pst_OK3_Box;
		COL_tdst_OK3_Element	*pst_OK3_Element;
		int						j, k;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		
		SAV_Buffer(&_pst_Object->pst_OK3->ul_NumBox, 4);
		for(j = 0; j < (int) _pst_Object->pst_OK3->ul_NumBox; j++)
		{
			pst_OK3_Box = &_pst_Object->pst_OK3->pst_OK3_Boxes[j];
			
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
		COL_Save_Nodes_Recursively(_pst_Object->pst_OK3->pst_OK3_God, _pst_Object->pst_OK3);

	}
#endif

#ifdef ALIGNED_VERTEX
	{
		/*~~~~~~~~~~~~~~~~~~~*/
		GEO_Vertex	*p, *plast;
		/*~~~~~~~~~~~~~~~~~~~*/

		p = _pst_Object->dst_Point;
		plast = p + _pst_Object->l_NbPoints;
		for(; p < plast; p++)
		{
			SAV_Buffer(p, sizeof(MATH_tdst_Vector));
		}
	}

#else
	SAV_Buffer(_pst_Object->dst_Point, sizeof(MATH_tdst_Vector) * _pst_Object->l_NbPoints);
#endif
	SAV_Buffer(_pst_Object->dst_PointNormal, sizeof(MATH_tdst_Vector) * _pst_Object->l_NbPoints);

	if(_pst_Object->p_MRM_ObjectAdditionalInfo)
	{
		SAV_Buffer
		(
			_pst_Object->p_MRM_ObjectAdditionalInfo->Absorbers,
			sizeof(unsigned short) * _pst_Object->l_NbPoints
		);
		if(_pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer)
		{
			SAV_Buffer
			(
				_pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer,
				sizeof(unsigned short) * _pst_Object->l_NbPoints
			);
		}

		SAV_Buffer(&_pst_Object->p_MRM_ObjectAdditionalInfo->MinimumNumberOfPoints, 4);
		//SAV_Buffer(&_pst_Object->df_MRMQualityCurve, (LONG) 4 * (LONG) 8);
	}

	if(_pst_Object->dul_PointColors)
		SAV_Buffer(_pst_Object->dul_PointColors + 1, sizeof(ULONG) * _pst_Object->dul_PointColors[0]);

    if (_pst_Object->ul_EditorFlags & GEO_CEF_GeomFor3DText)
    {
        //for (i = 0; i < _pst_Object->l_NbUVs; i++)
		for (i = 0; i < (_pst_Object->dst_Element->l_NbTriangles * 2); i++)
            _pst_Object->dst_UV[i].fU = _pst_Object->dst_UV[i].fV = ((float) (i + 1)) / 1000.0f;
    }
    
	SAV_Buffer(_pst_Object->dst_UV, sizeof(GEO_tdst_UV) * _pst_Object->l_NbUVs);

    // Save elements for geometry
    GEO_v_SaveElementsInBuffer(_pst_Object->l_NbElements, _pst_Object->dst_Element,&_pst_Object->ulStripFlag);

    // Save elements for MRM levels.
    if (bHasMRMLevels)
    {
        int i;

        SAV_Buffer(&(_pst_Object->p_MRM_Levels->f_DistanceCoef),sizeof(float));
        SAV_Buffer(&(_pst_Object->p_MRM_Levels->ul_LevelNb),sizeof(LONG));

        for (i=0; i<(int)_pst_Object->p_MRM_Levels->ul_LevelNb ;i++)
            SAV_Buffer(&(_pst_Object->p_MRM_Levels->dl_ElementNb[i]), sizeof(int));

        for (i=0; i<(int)_pst_Object->p_MRM_Levels->ul_LevelNb-1 ;i++)
            SAV_Buffer(&(_pst_Object->p_MRM_Levels->f_Thresholds[i]), sizeof(float));

        if (bHasIndirection)
        {
            for (i=0; i<_pst_Object->l_NbPoints; i++)
                SAV_Buffer(&(_pst_Object->p_MRM_Levels->dus_ReorderBuffer[i]),sizeof(unsigned short));
        }
        
        SAV_Buffer(&(_pst_Object->p_MRM_Levels->l_TotalPointNb),sizeof(ULONG));
        for (i=0; i<(int)_pst_Object->p_MRM_Levels->ul_LevelNb ; i++)
    	    SAV_Buffer(&(_pst_Object->p_MRM_Levels->dl_PointNb[i]),sizeof(ULONG));
        
        SAV_Buffer(&(_pst_Object->p_MRM_Levels->l_TotalUVNb),sizeof(ULONG));
        for (i=0; i<(int)_pst_Object->p_MRM_Levels->ul_LevelNb ; i++)
    	    SAV_Buffer(&(_pst_Object->p_MRM_Levels->dl_UVNb[i]),sizeof(ULONG));

    }

	SAV_Buffer(&_pst_Object->l_NbSpritesElements , 4);
	/* *** strip data saving end *** */
	/* ************ Sprite BEGIN ******************/
	if (_pst_Object->l_NbSpritesElements)
	{
		ULONG ElementCounter;
		/* Save version */
		ElementCounter = 0;
		SAV_Buffer(&ElementCounter,4);
		ElementCounter = _pst_Object->l_NbSpritesElements;
		while (ElementCounter--)
		{
			ULONG Dummy;
			SAV_Buffer(&_pst_Object->dst_SpritesElements[ElementCounter].fGlobalRatio,4);
			SAV_Buffer(&_pst_Object->dst_SpritesElements[ElementCounter].fGlobalSize,4);
			SAV_Buffer(&_pst_Object->dst_SpritesElements[ElementCounter].l_MaterialId,4);
			SAV_Buffer(&_pst_Object->dst_SpritesElements[ElementCounter].l_NbSprites,4);
			Dummy = 0;
			SAV_Buffer(&Dummy,4);
			SAV_Buffer(&Dummy,4);
			SAV_Buffer(&Dummy,4);
			SAV_Buffer(&Dummy,4);

			if (_pst_Object->dst_SpritesElements[ElementCounter].l_NbSprites)
			{
				SAV_Buffer(_pst_Object->dst_SpritesElements[ElementCounter].dst_Sprite,sizeof(GEO_tdst_IndexedSprite) * _pst_Object->dst_SpritesElements[ElementCounter].l_NbSprites);
			}
		}
	}
	/* ************ Sprite END ******************/

#if defined(XML_CONV_TOOL)
	if (gGeoHasCode2002 && gGeoVersion > 1)
	{
#endif
	lTemp = 0xC0DE2009;
	SAV_Buffer(&lTemp, 4);
#if defined(XML_CONV_TOOL)
	}
#endif
#endif

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Interface	*i;
	/*~~~~~~~~~~~~~~~~~~~*/

	i = &GRO_gast_Interface[GRO_Geometric];
	i->pfnp_CreateFromBuffer = (void *(__cdecl *)(GRO_tdst_Struct *,char ** ,void *))GEO_p_CreateFromBuffer;
	i->pfnp_Duplicate = (void *(__cdecl *)(void *,char *,char*,ULONG))GEO_p_Duplicate;
	i->pfn_Destroy = (void (__cdecl *)(void *))GEO_Free;
	i->pfnl_HasSomethingToRender = (LONG (__cdecl *)(void *,void *))GEO_l_HasSomethingToRender;
	i->pfn_Render = (void (__cdecl *)(void *))GEO_Render;
#ifdef ACTIVE_EDITORS
	i->pfnl_SaveInBuffer = (LONG (__cdecl *)(void *,void *))GEO_l_SaveInBuffer;
	i->pfnp_CreateFromMad = (void*(__cdecl*)(void*))GEO_p_CreateFromMad;
	i->pfnp_ToMad = (void* (__cdecl *)(void *,void *))GEO_p_ToMad;
#endif
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_AdjustForSymetrie(GEO_tdst_Object *pst_Object)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*p_Normals;
	GEO_Vertex			*p_Src, *p_Lst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Src = pst_Object->dst_Point;
	p_Lst = p_Src + pst_Object->l_NbPoints;
	p_Normals = pst_Object->dst_PointNormal;
	while(p_Src < p_Lst)
	{
		if(p_Src->x < 0.005f)	/* 5 milimeters */
		{
			p_Normals->x = 0.0f;
			MATH_NormalizeVector(p_Normals, p_Normals);
			p_Src->x = 0.0f;
		}

		p_Normals++;
		p_Src++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ResetRLI(GEO_tdst_Object *_pst_Obj, ULONG _ul_Mask)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	*pul_RLI, *pul_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((!_pst_Obj) || (_pst_Obj->st_Id.i->ul_Type != GRO_Geometric)) return;

	if(_pst_Obj->dul_PointColors)
	{
		pul_RLI = _pst_Obj->dul_PointColors;
		pul_Last = pul_RLI + (pul_RLI[0] + 1);
		pul_RLI++;
		for(; pul_RLI < pul_Last; pul_RLI++) *pul_RLI &= _ul_Mask;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DestroyRLI(GEO_tdst_Object *_pst_Obj )
{
	if((!_pst_Obj) || (_pst_Obj->st_Id.i->ul_Type != GRO_Geometric)) return;

	if(_pst_Obj->dul_PointColors)
	{
		MEM_GEO_v_Free(_pst_Obj->dul_PointColors);
		_pst_Obj->dul_PointColors = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_RLIAlphaToColor( ULONG *_pul_RLI )
{
    ULONG ul_Color, *last;

    if (!_pul_RLI ) return;
    last = _pul_RLI + (1 + *_pul_RLI);
    _pul_RLI++;

    while (_pul_RLI < last)
    {
        ul_Color = (*_pul_RLI & 0xFF000000) >> 8;
        ul_Color |= (ul_Color >> 8) | (ul_Color >> 16);
        LIGHT_SubColor( _pul_RLI, ul_Color );
        _pul_RLI++;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_RLIInvertAlpha( ULONG *_pul_RLI )
{
    ULONG *last;

    if (!_pul_RLI ) return;
    last = _pul_RLI + (1 + *_pul_RLI);
    _pul_RLI++;

    while (_pul_RLI < last)
    {
        *_pul_RLI = ( ~*_pul_RLI & 0xFF000000) | (*_pul_RLI & 0xFFFFFF);
        _pul_RLI++;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ApplyTransfoToVertices( GEO_tdst_Object *_pst_GeoObj, MATH_tdst_Matrix *_pst_TransfoMat )
{
	MATH_tdst_Vector	vect;
	int					i;

	for (i = 0; i < _pst_GeoObj->l_NbPoints; i++)
	{
		MATH_TransformVector(&vect, _pst_TransfoMat, &_pst_GeoObj->dst_Point[i]);
		MATH_CopyVector( &_pst_GeoObj->dst_Point[i], &vect );
	}

	GEO_ComputeNormals( _pst_GeoObj );
}
#endif

#ifdef _XBOX
void GEO_FilterColorLuminanceCrominace(DWORD *pColors)
{
	unsigned int counter;

	if(pColors)
	{
		for(counter=1;counter<=pColors[0];++counter)
		{
			//CONVERT COLOR
			DWORD tempColor=pColors[counter];
			pColors[counter]=Gx8_ConvertChrominancePixel(Gx8_M_ConvertColor(tempColor)); 



			//Return to PS2 format color
			pColors[counter]=Gx8_M_ConvertColor(pColors[counter]);

		}

	}
}
#endif





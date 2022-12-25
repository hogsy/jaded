/*$T GSPinit.c GC! 1.081 05/04/00 15:08:05 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
/*#pragma global_optimizer on
#pragma optimization_level 4*/

#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>

#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "Gsp.h"
#include "GSPinit.h"
#include "GSPtex.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#include "MATerial/MATstruct.h"


#include "Gsp_Bench.h"
#include "BASe/BENch/Bench.h"
//#define BUFFER_FULL_TEST
//#define USE_BUFFER_IN_OTHER_PLACE
//#define DRAWBUF_DEBUG_MODE
//#define GSP_BUF_CHECK
//#define DRAWBUF_DEBUG_MODE_CHECK
//#define GSP_FULLSYNCHRO
#ifdef PSX2_TARGET
#define GSP_DRAWBUF_ASSEMBLY

//#define GSP_POF(a) a##_PUSHED
#define GSP_POF(a) a##_PUSHED_1

/**************************************************************************************************************************
**************************************************************************************************************************
		CACHE MANAGEMENT START
**************************************************************************************************************************
***************************************************************************************************************************/

u_long128 GS_RAMBUFFER1[(RAM_BUF_SIZE * BX_NumberOfBuffers) >> 4] __attribute__((aligned (64)));
u_long128 *RAMBUFFER1 = NULL;

void Gsp_InitRingBuffer(u32 NumAtoms);
u32 GSPDB_Allocate(u32 *size)
{
	u32 ret;
	Gsp_FIFO_STOP();
	Gsp_Flush(FLUSH_ALL);
	FlushCache(0);
	
	Gsp_InitRingBuffer(2);
	ret = (u32)&GS_RAMBUFFER1[(RAM_BUF_SIZE * 2) >> 4];
	*size = (RAM_BUF_SIZE * (BX_NumberOfBuffers - 2));
	FlushCache(0);
	return ret;
}
void GSPDB_Free()
{
	Gsp_FIFO_STOP();
	Gsp_Flush(FLUSH_ALL);
	FlushCache(0);
	
	Gsp_InitRingBuffer(BX_NumberOfBuffers);
	L_memset(GS_RAMBUFFER1,0,sizeof(GS_RAMBUFFER1));
	FlushCache(0);
}


extern MyMatrixFormat *Gsp_SetDrawBuffer_Matrix(Gsp_BigStruct	  *p_BIG , Gsp_tdst_DrawBuffer *p_MDB );

#ifdef GSP_GEO_ACTIVE_CACHE

#define GSP_GEO_CacheSize 1100
#define GSP_GEO_CacheSize_Colors 1100
#define GSP_GEO_BackCacheSize 4096

static GSP_GEOBackCache  GSP_GEO_AllBackCache  [GSP_GEO_BackCacheSize];

 u_long128 	GSP_GEO_Cache_XYZW	[3 * DP_MaxNumT * GSP_GEO_CacheSize] __attribute__((aligned (64)));
 u_long		GSP_GEO_Cache_UV	[3 * DP_MaxNumT * GSP_GEO_CacheSize] __attribute__((aligned (64)));
static GSP_GEOCacheBlock gs_st_XYZW_Cache_Block[GSP_GEO_CacheSize];
static GSP_GEOCacheBlock gs_st___UV_Cache_Block[GSP_GEO_CacheSize];
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
static u_int				GSP_GEO_Cache_CC[3 * DP_MaxNumT * GSP_GEO_CacheSize_Colors] __attribute__((aligned (64)));
static GSP_GEOCacheBlock 	gs_st___CC_Cache_Block[GSP_GEO_CacheSize_Colors] __attribute__((aligned (64)));
#endif


#ifdef DRAWBUF_DEBUG_MODE_CHECK
u_int CHECK_CACHEBLOCK_XY(GSP_GEOCacheBlock *pCB)
{
	u_int BAD;
	BAD = 0;
	if (pCB)
	{
 
		if (pCB < gs_st_XYZW_Cache_Block) BAD = 1;
		if (pCB >= &gs_st_XYZW_Cache_Block[GSP_GEO_CacheSize]) BAD |= 2;
		if ((0x0fffffff & ((u32)pCB->DataBlock)) < (u32)GSP_GEO_Cache_XYZW) BAD |= 4;
		if ((0x0fffffff & ((u32)pCB->DataBlock)) >= (u32)&GSP_GEO_Cache_XYZW[3 * DP_MaxNumT * GSP_GEO_CacheSize]) BAD |= 8;
		if (BAD)
			BAD = BAD + 1;
		else
			CHECK_CACHEBLOCK_XY(pCB->p_stNextBlock);
	}
	return BAD;
}
u_int CHECK_CACHEBLOCK_UV(GSP_GEOCacheBlock *pCB)
{
	u_int BAD;
	BAD = 0;
	if (pCB)
	{
		if (pCB < gs_st___UV_Cache_Block) BAD |= 1;
		if (pCB >= &gs_st___UV_Cache_Block[GSP_GEO_CacheSize]) BAD |= 2;
		if ((0x0fffffff & ((u32)pCB->DataBlock)) < (u32)GSP_GEO_Cache_UV) BAD |= 4;
		if ((0x0fffffff & ((u32)pCB->DataBlock)) >= (u32)&GSP_GEO_Cache_UV[3 * DP_MaxNumT * GSP_GEO_CacheSize]) BAD |= 8;
		if (BAD)
			BAD = BAD + 1;
		else
			CHECK_CACHEBLOCK_UV(pCB->p_stNextBlock);
	}
	return BAD;
}
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
u_int CHECK_CACHEBLOCK_CC(GSP_GEOCacheBlock *pCB)
{
	u_int BAD;
	BAD = 0;
	if (pCB)
	{
 
 		if (pCB < gs_st___CC_Cache_Block) BAD |= 1;
		if (pCB >= &gs_st___CC_Cache_Block[GSP_GEO_CacheSize_Colors]) BAD |= 2;
		if ((0x0fffffff & ((u32)pCB->DataBlock)) < (u32)GSP_GEO_Cache_CC) BAD |= 4;
		if ((0x0fffffff & ((u32)pCB->DataBlock)) >= (u32)&GSP_GEO_Cache_CC[3 * DP_MaxNumT * GSP_GEO_CacheSize_Colors]) BAD |= 8;
		if (BAD)
			BAD = BAD + 1;
		else
			CHECK_CACHEBLOCK_CC(pCB->p_stNextBlock);
	}
	return BAD;
}
#endif
u_int CHECK_GSP_FREE()
{
	CHECK_CACHEBLOCK_XY(GspGlobal_ACCESS(gs_st_First_Free_XYZW));
	CHECK_CACHEBLOCK_UV(GspGlobal_ACCESS(gs_st_First_Free___UV));
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	CHECK_CACHEBLOCK_CC(GspGlobal_ACCESS(gs_st_First_Free___CC));
#endif	
}
u_int CHECK_GSP_BACKCACHE(GSP_GEOBackCache *p_BCPtr)
{
	u_int BAD;
	BAD = 0;
	if (p_BCPtr < GSP_GEO_AllBackCache) BAD |= 64;
	if (p_BCPtr >= &GSP_GEO_AllBackCache[GSP_GEO_BackCacheSize]) BAD |= 128;
	if (BAD)
		BAD = BAD + 1;
	BAD |= CHECK_CACHEBLOCK_XY(p_BCPtr->p_STRIPED_XYZW_Block_Entry);
	BAD |= CHECK_CACHEBLOCK_UV(p_BCPtr->p_STRIPED_UV___Block_Entry);
	BAD |= CHECK_GSP_FREE();
	return BAD;
}
#else
#define CHECK_CACHEBLOCK_XY(a)
#define CHECK_CACHEBLOCK_UV(a)
#define CHECK_CACHEBLOCK_CC(a)
#define CHECK_GSP_FREE(a)
#define CHECK_GSP_BACKCACHE(a)
#endif


#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
#ifdef USE_GO_DATA
typedef struct GSP_GO_BackCache_
{
	u_int 										CacheFlags;
	u_int										ulSTRIP_Ponderation_CC;
	GSP_GEOCacheBlock							*p_STRIPED_CC___Block_Entry; /* (DP_MaxNumT * 3) * 16 blocks size = 2016 bytes */
	u_int										ulSTRIP_Ponderation_XYZW;
	GSP_GEOCacheBlock							*p_STRIPED_XYZW_Block_Entry; /* (DP_MaxNumT * 3) * 16 blocks size = 2016 bytes */
	struct GSP_GO_BackCache_					*p_NextBackCache;
} GSP_GO_BackCache;

typedef struct GSP_GO_DataCache_
{
	u_int 										ulTrace;
	GEO_tdst_Object								*p_stObj;
	OBJ_tdst_GameObject 						*_pst_Owner_GO;
	struct GSP_GO_BackCache_					*p_FirstBackCache;
} GSP_GO_DataCache;

#define GSP_GO_BackCacheSize 4096
#define GSP_GO_DataCacheSize 512

static GSP_GO_BackCache  	*gp_FirstFree_GO_BC = NULL;
static GSP_GO_BackCache  	GSP_GO_AllBackCache[GSP_GO_BackCacheSize];

static GSP_GO_DataCache  	GSP_GO_AllDataCache[GSP_GO_DataCacheSize];

GSP_GO_DataCache *Gsp_GO_DC_Allocate()
{
	u_int Counter;
	GSP_GO_DataCache  	*gp_GO_DC_Finder;
	Counter = GSP_GO_DataCacheSize;
	gp_GO_DC_Finder = GSP_GO_AllDataCache;
	while (Counter--)
	{
		if (!gp_GO_DC_Finder->_pst_Owner_GO) return gp_GO_DC_Finder;
		gp_GO_DC_Finder++;
	}
	return NULL;
}

GSP_GO_BackCache *Gsp_GO_BC_Allocate(u_int ulNumber)
{
	GSP_GO_BackCache  	*gp_GO_BC_Finder , *gp_GO_BC_FinderSWP;
	if ((!ulNumber) || (!gp_FirstFree_GO_BC)) return NULL;
	gp_GO_BC_Finder = gp_FirstFree_GO_BC;
	while (gp_GO_BC_Finder && ulNumber)
	{
		gp_GO_BC_FinderSWP = gp_GO_BC_Finder;
		gp_GO_BC_Finder = gp_GO_BC_Finder->p_NextBackCache;
		ulNumber --;
	}
	if (ulNumber) return NULL;
	gp_GO_BC_Finder = gp_FirstFree_GO_BC;
	gp_FirstFree_GO_BC = gp_GO_BC_FinderSWP->p_NextBackCache;
	gp_GO_BC_FinderSWP->p_NextBackCache = NULL;
	return gp_GO_BC_Finder;
}

u_int GSP_Allocate_GO_DATA(OBJ_tdst_GameObject *_pst_GO , GEO_tdst_Object	*pst_Obj)
{
	GSP_GO_DataCache *pdc;
	pdc = Gsp_GO_DC_Allocate();
	if (!pdc) return 0;
	pdc->p_FirstBackCache = Gsp_GO_BC_Allocate(pst_Obj->l_NbElements);
	if (!pdc->p_FirstBackCache) 
	{
		return 0;
	}
	pdc->_pst_Owner_GO = _pst_GO;
	pdc->ulTrace = 0;
	pdc->p_stObj = pst_Obj;
	_pst_GO->DataCacheOptimization = (void *)pdc;
	return 1;
}
void GSP_Swap_FreeList()
{
   	GSP_GEOCacheBlock *P_Swap;
	P_Swap = GspGlobal_ACCESS(GSP_POF(gs_st_First_Free_XYZW_SPECIAL_VB));
	GspGlobal_ACCESS(GSP_POF(gs_st_First_Free_XYZW_SPECIAL_VB)) = GspGlobal_ACCESS(gs_st_First_Free_XYZW);
	GspGlobal_ACCESS(gs_st_First_Free_XYZW) = P_Swap;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	P_Swap = GspGlobal_ACCESS(GSP_POF(gs_st_First_Free___CC_SPECIAL_VB));
	GspGlobal_ACCESS(GSP_POF(gs_st_First_Free___CC_SPECIAL_VB)) = GspGlobal_ACCESS(gs_st_First_Free___CC);
	GspGlobal_ACCESS(gs_st_First_Free___CC) = P_Swap;
#endif
}

GSP_GO_BackCache  	*GSP_DC_FREE_DATA_GEO(GSP_GO_BackCache  	*gp_GO_BC_Finder , u_int What)
{
	GSP_GO_BackCache  	*gp_GO_BC_Finder_SWP;
	gp_GO_BC_Finder_SWP = gp_GO_BC_Finder;
	while (gp_GO_BC_Finder) 
	{
		GSP_GEOCacheBlock *p_FinderLast;
		if (What & GCF_AllStripCCAreIn)
		{
			if (gp_GO_BC_Finder->p_STRIPED_CC___Block_Entry)
			{
				p_FinderLast = gp_GO_BC_Finder->p_STRIPED_CC___Block_Entry;
				while (p_FinderLast->p_stNextBlock) p_FinderLast = p_FinderLast->p_stNextBlock;
				p_FinderLast -> p_stNextBlock = GspGlobal_ACCESS(GSP_POF(gs_st_First_Free___CC_SPECIAL_VB));
				GspGlobal_ACCESS(GSP_POF(gs_st_First_Free___CC_SPECIAL_VB)) = gp_GO_BC_Finder->p_STRIPED_CC___Block_Entry;
				gp_GO_BC_Finder->p_STRIPED_CC___Block_Entry = NULL;
			}
			gp_GO_BC_Finder->ulSTRIP_Ponderation_CC = 0;
			gp_GO_BC_Finder->CacheFlags &= ~(GCF_AllStripCCAreIn);
		}

		if (What & GCF_AllStripXyzAreIn)
		{
			if (gp_GO_BC_Finder->p_STRIPED_XYZW_Block_Entry)
			{
				p_FinderLast = gp_GO_BC_Finder->p_STRIPED_XYZW_Block_Entry;
				while (p_FinderLast->p_stNextBlock) p_FinderLast = p_FinderLast->p_stNextBlock;
				p_FinderLast -> p_stNextBlock = GspGlobal_ACCESS(GSP_POF(gs_st_First_Free_XYZW_SPECIAL_VB));
				GspGlobal_ACCESS(GSP_POF(gs_st_First_Free_XYZW_SPECIAL_VB)) = gp_GO_BC_Finder->p_STRIPED_XYZW_Block_Entry;
				gp_GO_BC_Finder->p_STRIPED_XYZW_Block_Entry = NULL;
			}
			gp_GO_BC_Finder->ulSTRIP_Ponderation_XYZW = 0;		
			gp_GO_BC_Finder->CacheFlags &= ~GCF_AllStripXyzAreIn;
		}
		gp_GO_BC_Finder_SWP = gp_GO_BC_Finder;
		gp_GO_BC_Finder = gp_GO_BC_Finder->p_NextBackCache;
	}
	return gp_GO_BC_Finder_SWP;
}
void GSP_Free_DC_DATA(GSP_GO_DataCache *pdc)
{
	GSP_GO_BackCache  	*gp_GO_BC_Finder_SWP;
	if (!pdc) return;
	pdc->_pst_Owner_GO = NULL;
	gp_GO_BC_Finder_SWP = GSP_DC_FREE_DATA_GEO(pdc->p_FirstBackCache , GCF_AllStripCCAreIn | GCF_AllStripXyzAreIn);
	gp_GO_BC_Finder_SWP -> p_NextBackCache = gp_FirstFree_GO_BC;
	gp_FirstFree_GO_BC = 	pdc->p_FirstBackCache;
	pdc->p_FirstBackCache = NULL;
	CHECK_GSP_FREE();
}
void GSP_Free_GO_DATA(OBJ_tdst_GameObject *_pst_GO)
{
	GSP_Free_DC_DATA((GSP_GO_DataCache *)_pst_GO->DataCacheOptimization);
	_pst_GO->DataCacheOptimization = NULL;
}

u_int GSP_AllColorsAreCached(OBJ_tdst_GameObject *_pst_GO , GEO_tdst_Object	*pst_Obj)
{
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	GSP_GO_BackCache *p_FinderLast;
	u_int REturnValue;
	_GSP_BeginRaster(51);
	if (GspGlobal_ACCESS(RenderingInterface)) return 0;
	if (!pst_Obj->l_NbElements) return 0;
	if (!pst_Obj->dst_Element->pst_StripDataPS2) return 0;
	if (!pst_Obj->dst_Element->p_ElementCache) return 0;
	if (!_pst_GO->DataCacheOptimization)
	{
		GSP_Allocate_GO_DATA(_pst_GO , pst_Obj);
		_GSP_EndRaster(51);
		return 0;
	}
	if (	(((GSP_GO_DataCache *)_pst_GO->DataCacheOptimization)->p_stObj != pst_Obj) || 
			(((GSP_GO_DataCache *)_pst_GO->DataCacheOptimization)->_pst_Owner_GO != _pst_GO))
			
	{
		_pst_GO->DataCacheOptimization = NULL;
		GSP_Free_DC_DATA((GSP_GO_DataCache *)_pst_GO->DataCacheOptimization);
		GSP_Allocate_GO_DATA(_pst_GO , pst_Obj);
		_GSP_EndRaster(51);
		return 0;
	}

    pst_Element = pst_Obj->dst_Element;
    pst_LastElement = pst_Element + pst_Obj->l_NbElements;
    p_FinderLast = ((GSP_GO_DataCache *)_pst_GO->DataCacheOptimization)->p_FirstBackCache;
    REturnValue = 0xffffffff;
    while (pst_Element < pst_LastElement)
    {
    	if (pst_Element->p_ElementCache)
   	 	{
    		pst_Element->p_ElementCache->p_STRIPED_CC___Block_Entry = p_FinderLast->p_STRIPED_CC___Block_Entry;
   			pst_Element->p_ElementCache->ulSTRIP_Ponderation_CC = p_FinderLast->ulSTRIP_Ponderation_CC;
   			pst_Element->p_ElementCache->CacheFLags &= ~(GCF_AllStripCCAreIn);
   			pst_Element->p_ElementCache->CacheFLags |= p_FinderLast->CacheFlags & GCF_AllStripCCAreIn;
   			if (p_FinderLast->p_STRIPED_XYZW_Block_Entry)
			{
	    		pst_Element->p_ElementCache->p_STRIPED_XYZW_Block_Entry = p_FinderLast->p_STRIPED_XYZW_Block_Entry;
	   			pst_Element->p_ElementCache->ulSTRIP_Ponderation_XYZW  = p_FinderLast->ulSTRIP_Ponderation_XYZW;
	   			pst_Element->p_ElementCache->CacheFLags &= ~GCF_AllStripXyzAreIn;
	   			pst_Element->p_ElementCache->CacheFLags |= p_FinderLast->CacheFlags & GCF_AllStripXyzAreIn;
			} 
			REturnValue &= pst_Element->p_ElementCache->CacheFLags & (GCF_AllStripCCAreIn | GCF_AllStripXyzAreIn);
    	}
    	p_FinderLast = p_FinderLast->p_NextBackCache;
    	pst_Element++;
    }
    _GSP_EndRaster(51);
    return REturnValue & (GCF_AllStripCCAreIn|GCF_AllStripXyzAreIn);
}
void GSP_SaveCachedData(OBJ_tdst_GameObject *_pst_GO , GEO_tdst_Object	*pst_Obj)
{
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	GSP_GO_BackCache *p_FinderLast;
	u_int CacheFLags;
	if (!pst_Obj->l_NbElements) return;
	if (!pst_Obj->dst_Element->pst_StripDataPS2) return;
	if (!_pst_GO->DataCacheOptimization) return;
    _GSP_BeginRaster(51);
    pst_Element = pst_Obj->dst_Element;
    pst_LastElement = pst_Element + pst_Obj->l_NbElements;
    p_FinderLast = ((GSP_GO_DataCache *)_pst_GO->DataCacheOptimization)->p_FirstBackCache;
    while (pst_Element < pst_LastElement)
    {
    	if (pst_Element->p_ElementCache)
    	{
    		p_FinderLast->CacheFlags = 0;
    		p_FinderLast->p_STRIPED_CC___Block_Entry = pst_Element->p_ElementCache->p_STRIPED_CC___Block_Entry;
   			p_FinderLast->ulSTRIP_Ponderation_CC = pst_Element->p_ElementCache->ulSTRIP_Ponderation_CC ;
   			pst_Element->p_ElementCache->p_STRIPED_CC___Block_Entry = NULL;
   			pst_Element->p_ElementCache->ulSTRIP_Ponderation_CC = 0;
   			p_FinderLast->CacheFlags |= pst_Element->p_ElementCache->CacheFLags & GCF_AllStripCCAreIn;
   			pst_Element->p_ElementCache->CacheFLags &= ~(GCF_AllStripCCAreIn);
			if ((GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer) || (p_FinderLast->p_STRIPED_XYZW_Block_Entry))
			{
	    		p_FinderLast->p_STRIPED_XYZW_Block_Entry = pst_Element->p_ElementCache->p_STRIPED_XYZW_Block_Entry;
	   			p_FinderLast->ulSTRIP_Ponderation_XYZW = pst_Element->p_ElementCache->ulSTRIP_Ponderation_XYZW ;
	   			pst_Element->p_ElementCache->p_STRIPED_XYZW_Block_Entry = NULL;
	   			pst_Element->p_ElementCache->ulSTRIP_Ponderation_XYZW = 0;
	   			p_FinderLast->CacheFlags |= pst_Element->p_ElementCache->CacheFLags & GCF_AllStripXyzAreIn;
	   			pst_Element->p_ElementCache->CacheFLags &= ~GCF_AllStripXyzAreIn;
			}
    	}
    	p_FinderLast = p_FinderLast->p_NextBackCache;
    	pst_Element++;
    }
    _GSP_EndRaster(51);
}
void GSP_Init_GO_DATA()
{
	u_int Counter;
	L_memset(GSP_GO_AllBackCache , 0 , sizeof(GSP_GO_BackCache) * GSP_GO_BackCacheSize);
	L_memset(GSP_GO_AllDataCache , 0 , sizeof(GSP_GO_DataCache) * GSP_GO_DataCacheSize);
	Counter = GSP_GO_BackCacheSize;
	while (Counter--) GSP_GO_AllBackCache[Counter].p_NextBackCache = &GSP_GO_AllBackCache[Counter + 1];
	gp_FirstFree_GO_BC = &GSP_GO_AllBackCache[0];
	GSP_GO_AllBackCache[GSP_GO_BackCacheSize - 1].p_NextBackCache = NULL;
}
void GSP_FreeAll_GO_DATA()
{
	u_int Counter;
	GSP_GO_DataCache  	*gp_GO_DC_Finder;
	Counter = GSP_GO_DataCacheSize;
	gp_GO_DC_Finder = GSP_GO_AllDataCache;
	while (Counter--)
	{
		if (gp_GO_DC_Finder->_pst_Owner_GO) 
		{
			GSP_Free_GO_DATA(gp_GO_DC_Finder->_pst_Owner_GO);
			gp_GO_DC_Finder->_pst_Owner_GO = NULL;			
		}
		gp_GO_DC_Finder++;
	}
	GSP_Init_GO_DATA();
}
void GSP_Manage_GO_DATA()
{
	u_int Counter;
	GSP_GO_DataCache  	*gp_GO_DC_Finder;
	Counter = GSP_GO_DataCacheSize;
	gp_GO_DC_Finder = GSP_GO_AllDataCache;
	while (Counter--)
	{
		if (gp_GO_DC_Finder->_pst_Owner_GO)
		{
			if (gp_GO_DC_Finder->_pst_Owner_GO->DataCacheOptimization != (void *)gp_GO_DC_Finder)
			{
				GSP_Free_DC_DATA(gp_GO_DC_Finder); // ! Alert
			}
			else
			{
				u_int Limit,What;
				gp_GO_DC_Finder->ulTrace += 0x00010001;
				Limit = 0;
				if (gp_GO_DC_Finder->_pst_Owner_GO->uc_LOD_Vis == 0) Limit = 0;
				else
				{
					if (gp_GO_DC_Finder->_pst_Owner_GO->uc_LOD_Vis < 5) Limit = 16;
					else
					if (gp_GO_DC_Finder->_pst_Owner_GO->uc_LOD_Vis < 10) Limit = 8;
					else
					if (gp_GO_DC_Finder->_pst_Owner_GO->uc_LOD_Vis < 25) Limit = 4;
					else
					if (gp_GO_DC_Finder->_pst_Owner_GO->uc_LOD_Vis < 50) Limit = 2;
				}

				What = 0;
				if (OBJ_ul_FlagsIdentityGet(gp_GO_DC_Finder->_pst_Owner_GO) & (OBJ_C_IdentityFlag_Dyna|OBJ_C_IdentityFlag_Bone|OBJ_C_IdentityFlag_Anims)) 
				{
					Limit >>= 1;
					if ((gp_GO_DC_Finder->ulTrace & 0x0000ffff) >= Limit + 2)
					{
						What = GCF_AllStripCCAreIn;
						gp_GO_DC_Finder->ulTrace &= 0xffff0000;
					}
					if ((gp_GO_DC_Finder->ulTrace >> 16) >= Limit)
					{
						What |= GCF_AllStripXyzAreIn;
						gp_GO_DC_Finder->ulTrace &= 0x0000ffff;
					}//*/
				} else
				{
					if ((gp_GO_DC_Finder->ulTrace & 0x0000ffff) >= (Limit << 1) + 4)
					{
						What = GCF_AllStripCCAreIn;
						gp_GO_DC_Finder->ulTrace &= 0xffff0000;
					}
					if ((gp_GO_DC_Finder->ulTrace >> 16) >= Limit)
					{
						What |= GCF_AllStripXyzAreIn;
						gp_GO_DC_Finder->ulTrace &= 0x0000ffff;
					}//*/
				}
				if (What) 
					GSP_DC_FREE_DATA_GEO(gp_GO_DC_Finder->p_FirstBackCache , What);
			}
						
		}
		gp_GO_DC_Finder++;
	}
}
#endif //USE_GO_DATA
#endif

void GSP_FlushABackCache(GSP_GEOBackCache *p_BCPtr,u_int FlushWhat)
{
	GSP_GEOCacheBlock *p_FinderLast;
	CHECK_GSP_BACKCACHE(p_BCPtr);
	if (p_BCPtr->p_ElementUser)
	{
		if (FlushWhat & GCF_AllStripXyzAreIn)
		{
			if ((!p_BCPtr->ulSTRIP_Ponderation_XYZW) || (!(p_BCPtr->CacheFLags & GCF_AllStripXyzAreIn)))
			{
				if (p_BCPtr->p_STRIPED_XYZW_Block_Entry)
				/* Not used -> Free allocated */
				/* Bad use detected -> Free allocated */
				{
					/* find last block */
					p_FinderLast = p_BCPtr->p_STRIPED_XYZW_Block_Entry;
					while (p_FinderLast->p_stNextBlock) p_FinderLast = p_FinderLast->p_stNextBlock;
					p_FinderLast -> p_stNextBlock = GspGlobal_ACCESS(gs_st_First_Free_XYZW);
					GspGlobal_ACCESS(gs_st_First_Free_XYZW) = p_BCPtr->p_STRIPED_XYZW_Block_Entry;
					p_BCPtr->p_STRIPED_XYZW_Block_Entry = NULL;
				}
				p_BCPtr->CacheFLags &= ~GCF_AllStripXyzAreIn;
			}
		}
		if (FlushWhat & GCF_AllStripUVAreIn)
		{
			if ((!p_BCPtr->ulSTRIP_Ponderation_UV) || (!(p_BCPtr->CacheFLags & GCF_AllStripUVAreIn)))
			{
				if (p_BCPtr->p_STRIPED_UV___Block_Entry)
				/* Not used -> Free allocated */
				{
					/* find last block */
					p_FinderLast = p_BCPtr->p_STRIPED_UV___Block_Entry;
					while (p_FinderLast->p_stNextBlock) p_FinderLast = p_FinderLast->p_stNextBlock;
					p_FinderLast -> p_stNextBlock = GspGlobal_ACCESS(gs_st_First_Free___UV);
					GspGlobal_ACCESS(gs_st_First_Free___UV) = p_BCPtr->p_STRIPED_UV___Block_Entry;
					p_BCPtr->p_STRIPED_UV___Block_Entry = NULL;
				}
				p_BCPtr->CacheFLags &= ~GCF_AllStripUVAreIn;
			}
		}	
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
		if (FlushWhat & GCF_AllStripCCAreIn)
		{
			if ((!p_BCPtr->ulSTRIP_Ponderation_CC) || (!(p_BCPtr->CacheFLags & GCF_AllStripCCAreIn)))
			{
				if (p_BCPtr->p_STRIPED_CC___Block_Entry)
				/* Not used -> Free allocated */
				{
					/* find last block */
					p_FinderLast = p_BCPtr->p_STRIPED_CC___Block_Entry;
					while (p_FinderLast->p_stNextBlock) p_FinderLast = p_FinderLast->p_stNextBlock;
					p_FinderLast -> p_stNextBlock = GspGlobal_ACCESS(gs_st_First_Free___CC);
					GspGlobal_ACCESS(gs_st_First_Free___CC) = p_BCPtr->p_STRIPED_CC___Block_Entry;
					p_BCPtr->p_STRIPED_CC___Block_Entry = NULL;
				}
				p_BCPtr->CacheFLags &= ~(GCF_AllStripCCAreIn|GCF_AllStripNormalsAreIn);
			}
		}
#endif		
	}
	CHECK_GSP_BACKCACHE(p_BCPtr);
}

void GSP_GEO_Cache_Add_Flushed_SVB()
{
	GSP_GEOCacheBlock *p_FinderLast;
	p_FinderLast = GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_2);
	if (p_FinderLast)
	{
		while (p_FinderLast->p_stNextBlock) p_FinderLast = p_FinderLast->p_stNextBlock;
		p_FinderLast -> p_stNextBlock = GspGlobal_ACCESS(gs_st_First_Free_XYZW);
		GspGlobal_ACCESS(gs_st_First_Free_XYZW) = GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_2);
	} 
	
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_2) = GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_1);
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_1) = GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED);
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED) = GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB);
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB) = NULL;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	p_FinderLast = GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED_2);
	if (p_FinderLast)
	{
		while (p_FinderLast->p_stNextBlock) p_FinderLast = p_FinderLast->p_stNextBlock;
		p_FinderLast -> p_stNextBlock = GspGlobal_ACCESS(gs_st_First_Free___CC);
		GspGlobal_ACCESS(gs_st_First_Free___CC) = GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED_2);
	} 
	
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED_2) = GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED_1);
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED_1) = GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED);
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED) = GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB);
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB) = NULL;
#endif	
	
	CHECK_GSP_FREE();
}
void GSP_ManageGeoCache()
{
	GSP_GEOBackCache *p_BCPtr;
	u_int gs_BackCacheNumber;
	u_int FrameMask;
	_GSP_BeginRaster(51);
#ifdef USE_GO_DATA	
	GSP_Manage_GO_DATA();
#endif	
	FrameMask = GspGlobal_ACCESS(FrameCounter) & 7;
	gs_BackCacheNumber = GSP_GEO_BackCacheSize;
	p_BCPtr = GSP_GEO_AllBackCache + GSP_GEO_BackCacheSize;
	GSP_Swap_FreeList();
	while (gs_BackCacheNumber--)
	{
		p_BCPtr--;
		GSP_FlushABackCache(p_BCPtr , GCF_AllStripXyzAreIn | GCF_AllStripUVAreIn | GCF_AllStripCCAreIn);
		p_BCPtr->ulSTRIP_Ponderation_XYZW = 0;
		p_BCPtr->ulSTRIP_Ponderation_UV = 0;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE		
		p_BCPtr->ulSTRIP_Ponderation_CC = 0;
#endif		
	}
	GSP_Swap_FreeList();
	CHECK_GSP_FREE();
	GSP_GEO_Cache_Add_Flushed_SVB();
	CHECK_GSP_FREE();
	_GSP_EndRaster(51);
}
void GSP_FlushAll()
{
	u_int Counter;
	GSP_GO_DataCache  	*gp_GO_DC_Finder;
	Counter = GSP_GO_DataCacheSize;
	gp_GO_DC_Finder = GSP_GO_AllDataCache;
	while (Counter--)
	{
		if (gp_GO_DC_Finder->_pst_Owner_GO)
		{
			if (gp_GO_DC_Finder->_pst_Owner_GO->DataCacheOptimization != (void *)gp_GO_DC_Finder)
			{
				GSP_Free_DC_DATA(gp_GO_DC_Finder); // ! Alert
			}
			else
			{
				gp_GO_DC_Finder->ulTrace = 0;
				GSP_DC_FREE_DATA_GEO(gp_GO_DC_Finder->p_FirstBackCache , GCF_AllStripCCAreIn | GCF_AllStripXyzAreIn);
			}
		}
		gp_GO_DC_Finder++;
	}
	
}
void GSP_FlushGameObject(GEO_tdst_Object *pst_Object)
{
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
   	GSP_GEOCacheBlock *P_Swap;
	_GSP_BeginRaster(51);
	
	CHECK_GSP_FREE();
	GSP_Swap_FreeList();
	pst_Element = pst_Object->dst_Element;
	pst_LastElement = pst_Element + pst_Object->l_NbElements;
	for(; pst_Element < pst_LastElement; pst_Element++)
	{
		if (pst_Element->p_ElementCache)
		{
			pst_Element->p_ElementCache->ulSTRIP_Ponderation_XYZW = 0;
			GSP_FlushABackCache(pst_Element->p_ElementCache , GCF_AllStripXyzAreIn);
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
			pst_Element->p_ElementCache->ulSTRIP_Ponderation_CC = 0;
			GSP_FlushABackCache(pst_Element->p_ElementCache , GCF_AllStripCCAreIn);
#endif
		}
	}
	GSP_Swap_FreeList();
	_GSP_EndRaster(51);
	
	CHECK_GSP_FREE();
}

u_int GetLenght(GSP_GEOCacheBlock *PBlock)
{
	u_int Length;
	Length = 0;
	while (PBlock)
	{
		Length++;
		PBlock = PBlock->p_stNextBlock;
	}
	return Length;
}
u_int GSP_GetCacheINFO(u_int ulType)
{
	switch (ulType)
	{
		case 0: return 100-(u_int)(100.0f * (float)GetLenght(GspGlobal_ACCESS(gs_st_First_Free_XYZW)) / (float)GSP_GEO_CacheSize);
		case 1: return 100-(u_int)(100.0f * (float)GetLenght(GspGlobal_ACCESS(gs_st_First_Free___UV)) / (float)GSP_GEO_CacheSize);
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
		case 2: return 100-(u_int)(100.0f * (float)GetLenght(GspGlobal_ACCESS(gs_st_First_Free___CC)) / (float)GSP_GEO_CacheSize_Colors);
#endif		
	}
	return 0;
}
extern u_int LoadedTriangles;
void GSP_AllocABackCache(struct GEO_tdst_ElementIndexedTriangles_ 	*p_ElementUser)
{
	GSP_GEOBackCache *p_BCPtr;
	u_int gs_BackCacheNumber;
	gs_BackCacheNumber = GSP_GEO_BackCacheSize;
	if (p_ElementUser -> p_ElementCache) return;
	LoadedTriangles ++;
	
	p_BCPtr = GSP_GEO_AllBackCache + GSP_GEO_BackCacheSize;
	while (gs_BackCacheNumber--)
	{
		p_BCPtr--;
		if (p_BCPtr->p_ElementUser == NULL)
		{
			p_BCPtr->p_ElementUser = p_ElementUser;
			p_ElementUser -> p_ElementCache = p_BCPtr;
			p_BCPtr->CacheFLags = 0;
			p_BCPtr->p_STRIPED_XYZW_Block_Entry = NULL;
			p_BCPtr->p_STRIPED_UV___Block_Entry = NULL;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
			p_BCPtr->p_STRIPED_CC___Block_Entry = NULL;
#endif				
			CHECK_GSP_BACKCACHE(p_BCPtr);
			return;
		}
		
	}
	p_ElementUser -> p_ElementCache = NULL;
}
void GSP_FreeABackCache(struct GSP_GEOBackCache_ *p_BackCache)
{
	/* Clear cross ref */
	if (p_BackCache)
	{
		LoadedTriangles --;
		CHECK_GSP_BACKCACHE(p_BackCache);
		p_BackCache -> p_ElementUser -> p_ElementCache = NULL;
		p_BackCache -> p_ElementUser = NULL;
	}
}
void GSP_InvalidateGeoCache(u_int CacheSize)
{
	u_int Counter;
	GSP_GEOBackCache *p_BackCache;
	GSP_GEOCacheBlock *Parser_XYZW, *Parser_UV;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	GSP_GEOCacheBlock *Parser_CC;
#endif		
#ifdef USE_GO_DATA	
	GSP_FreeAll_GO_DATA();
#endif	

	CacheSize = lMin(CacheSize , GSP_GEO_CacheSize);
//	CacheSize = 16;
	Counter = GSP_GEO_BackCacheSize;
	p_BackCache = GSP_GEO_AllBackCache;
	while (Counter--)
	{
		p_BackCache->p_STRIPED_XYZW_Block_Entry = NULL;
		p_BackCache->p_STRIPED_UV___Block_Entry = NULL;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
		p_BackCache->p_STRIPED_CC___Block_Entry = NULL;
#endif		
		p_BackCache->CacheFLags = 0;
		p_BackCache++;
	}
	
	Counter = CacheSize - 1;
	Parser_XYZW  = GspGlobal_ACCESS(gs_st_First_Free_XYZW) = gs_st_XYZW_Cache_Block;
	Parser_UV	 = GspGlobal_ACCESS(gs_st_First_Free___UV) = gs_st___UV_Cache_Block;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	Parser_CC 	 = GspGlobal_ACCESS(gs_st_First_Free___CC) = gs_st___CC_Cache_Block;
#endif
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_1) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_2) = NULL;

	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED_1) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED_2) = NULL;

	while (Counter--)
	{
		Parser_XYZW->	p_stNextBlock = Parser_XYZW+1;
		Parser_XYZW	++;
		Parser_UV->		p_stNextBlock = Parser_UV+1;
		Parser_UV	++;
	}
	Counter = GSP_GEO_CacheSize_Colors - 1;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	while (Counter--)
	{
		Parser_CC->	p_stNextBlock = Parser_CC+1;
		Parser_CC	++;
	}
#endif		
	Parser_XYZW	->p_stNextBlock = NULL;
	Parser_UV	->p_stNextBlock = NULL;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	Parser_CC->	p_stNextBlock = NULL;
#endif		
	CHECK_GSP_FREE();
}

void GSP_DestroyAllBackCaches()
{
	u_int Counter;
	GSP_GEOBackCache *p_BackCache;
	GSP_InvalidateGeoCache(65536);
	Counter = GSP_GEO_BackCacheSize;
	p_BackCache = GSP_GEO_AllBackCache;
	while (Counter--)
	{
		if (p_BackCache->p_ElementUser)
		{
			p_BackCache->p_ElementUser->p_ElementCache = NULL;
			p_BackCache->p_ElementUser = NULL;
		}
		p_BackCache++;
	}
}


void GSP_InitGeoCache(u_int CacheSize)
{
	u_int Counter;
	GSP_GEOCacheBlock *Parser_XYZW, *Parser_UV;
	u_long128	*p_DATA_XYZW;
	u_long		*p_DATA_UV;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	GSP_GEOCacheBlock *Parser_CC;
	u_int		*p_DATA_CC;
#endif
	CacheSize = lMin(CacheSize , GSP_GEO_CacheSize);
#ifdef USE_GO_DATA	
	GSP_Init_GO_DATA();
#endif	
	L_memset(GSP_GEO_AllBackCache , 0 , GSP_GEO_BackCacheSize * sizeof(GSP_GEOBackCache));
	GspGlobal_ACCESS(gs_st_First_Free_XYZW) = gs_st_XYZW_Cache_Block;
	GspGlobal_ACCESS(gs_st_First_Free___UV) = gs_st___UV_Cache_Block;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	GspGlobal_ACCESS(gs_st_First_Free___CC) = gs_st___CC_Cache_Block;
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED_1) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free___CC_SPECIAL_VB_PUSHED_2) = NULL;
#endif	
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_1) = NULL;
	GspGlobal_ACCESS(gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_2) = NULL;
	Counter = CacheSize - 1;
	Parser_XYZW  = gs_st_XYZW_Cache_Block;
	p_DATA_XYZW  = GSP_GEO_Cache_XYZW;
	Parser_UV 	 = gs_st___UV_Cache_Block;
	p_DATA_UV	 = GSP_GEO_Cache_UV;
	GSP_UnCacheAccPtr(p_DATA_XYZW);
	GSP_UnCacheAccPtr(p_DATA_UV);
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	Parser_CC 	 = gs_st___CC_Cache_Block;
	p_DATA_CC	 = GSP_GEO_Cache_CC;
	GSP_UnCacheAccPtr(p_DATA_CC);
#endif	
	FlushCache(0);
	while (Counter--)
	{
		Parser_XYZW ->	DataBlock = p_DATA_XYZW;
		Parser_XYZW->	p_stNextBlock = Parser_XYZW+1;
		p_DATA_XYZW	+= 	DP_MaxNumT * 3;
		Parser_XYZW	++;
		Parser_UV ->	DataBlock = p_DATA_UV;
		Parser_UV->		p_stNextBlock = Parser_UV+1;
		p_DATA_UV	+= 	DP_MaxNumT * 3;
		Parser_UV	++;
	}
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	Counter = GSP_GEO_CacheSize_Colors - 1;
	while (Counter--)
	{
		Parser_CC ->	DataBlock = p_DATA_CC;
		Parser_CC->		p_stNextBlock = Parser_CC+1;
		p_DATA_CC	+= 	DP_MaxNumT * 3;
		Parser_CC	++;
	}
#endif		

	Parser_XYZW ->	DataBlock = p_DATA_XYZW;
	Parser_XYZW->	p_stNextBlock = NULL;
	Parser_UV ->	DataBlock = p_DATA_UV;
	Parser_UV->		p_stNextBlock = NULL;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	Parser_CC ->	DataBlock = p_DATA_CC;
	Parser_CC->		p_stNextBlock = NULL;
#endif	
	CHECK_GSP_FREE();
}
#else
void GSP_AllocABackCache(struct GEO_tdst_ElementIndexedTriangles_ 	*p_ElementUser){};
void GSP_FreeABackCache(struct GSP_GEOBackCache_ *p_BackCache){};
void GSP_InitGeoCache(u_int CacheSize){};
void GSP_ManageGeoCache(){};
#endif


/**************************************************************************************************************************
**************************************************************************************************************************
		CACHE MANAGEMENT END
**************************************************************************************************************************
***************************************************************************************************************************/

extern u_int 	LABEL_FIRSTLINE 	__attribute__((section(".vudata")));
extern u_int 	UV_WorldCompute		__attribute__((section(".vudata")));
extern u_int 	UV_ObjectCompute_X	__attribute__((section(".vudata")));
extern u_int 	UV_ObjectCompute_Y	__attribute__((section(".vudata")));
extern u_int 	UV_ObjectCompute_Z	__attribute__((section(".vudata")));
extern u_int 	UV_ComputeFinished	__attribute__((section(".vudata")));
extern u_int 	STRIP_UV_WorldCompute		__attribute__((section(".vudata")));
extern u_int 	STRIP_UV_ObjectCompute_X	__attribute__((section(".vudata")));
extern u_int 	STRIP_UV_ObjectCompute_Y	__attribute__((section(".vudata")));
extern u_int 	STRIP_UV_ObjectCompute_Z	__attribute__((section(".vudata")));
extern u_int 	STRIP_UV_ComputeFinished	__attribute__((section(".vudata")));
extern u_int 	UV_WorldCompute_For_Shadows	__attribute__((section(".vudata")));
extern u_int 	STRIP_UV_WorldCompute_For_Shadows	__attribute__((section(".vudata")));
				

extern u_int 	SYM_STRIP_UV_WorldCompute		__attribute__((section(".vudata")));
extern u_int 	SYM_STRIP_UV_ObjectCompute_X	__attribute__((section(".vudata")));
extern u_int 	SYM_STRIP_UV_ObjectCompute_Y	__attribute__((section(".vudata")));
extern u_int 	SYM_STRIP_UV_ObjectCompute_Z	__attribute__((section(".vudata")));
extern u_int 	SYM_STRIP_UV_ComputeFinished	__attribute__((section(".vudata")));
extern u_int 	SYM_STRIP_UV_WorldCompute_For_Shadows	__attribute__((section(".vudata")));

#include "ENGvars.h"

/* mamagouille */
#include "PSX2debug.h"
#endif


#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif

void Gsp_restoreBugHightUV(Gsp_BigStruct	  *p_BIG , MyMatrixUVFormat *p_MMUVF);
void MulMatrixes_UV(MyMatrixUVFormat *Dst,MyMatrixUVFormat *M1,MyMatrixUVFormat *M2);
void Gsp_InitDrawBuffer(Gsp_tdst_DrawBuffer *MDB,u_int Base,u_int BuffSize);
void Gsp_RenderBuffer(register Gsp_BigStruct	  *p_BIG , register ULONG Num);
extern u_int volatile __declspec(scratchpad) gsulSyncroPath;
//#undef USE_SAVE_XYZ_BUFFER



MAT_tdst_Decompressed_UVMatrix MatrixUVIdentity __attribute__((aligned (8))) = 
{
	{ 1.0f , 0.0f , 0.0f , 1.0f } , 
	0.0f , 
	0.0f 
} ;

_inline_ MyMatrixUVFormat *Gsp_SetDrawBuffer_Matrix_UVS( Gsp_BigStruct	  *p_BIG )
{
	register u_long128 *p_ReturnValue;
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	p_ReturnValue = (u_long128 *)p_BIG->p_CurrentBufferPointer;
	*(p_ReturnValue++) = Gsp_64_to_128(p_BIG->MDB->VIF_MatrixUV);
	p_BIG->p_CurrentBufferPointer = (u_int)p_ReturnValue + GSP_ALIGN(sizeof(MyMatrixUVFormat));
	return (MyMatrixUVFormat *)p_ReturnValue;
}
_inline_ float Gsp_CastUV(float ff)
{
    register float ft;
    asm __volatile__ ("cvt.w.s  ft, ff");
    asm __volatile__ ("cvt.s.w  ft, ft");
    return ft;
}


void Gsp_restoreBugHightUV(Gsp_BigStruct	  *p_BIG , MyMatrixUVFormat *p_MMUVF)
{
	/* World compute hight UV restore to near camera */
	MATH_tdst_Matrix stCamMatrix;
	MATH_tdst_Vector *p_stCamCoord;
	GEO_tdst_UV 	stResult;
	float 			fSaveU;
	/* compute UV off Camera coordinate */
	if ((p_BIG->ulNewBMD & (MAT_Cul_Flag_TileU|MAT_Cul_Flag_TileV)) != (MAT_Cul_Flag_TileU|MAT_Cul_Flag_TileV)) return;
	MATH_InvertMatrix(&stCamMatrix , gpst_GSP_stDD.st_MatrixStack.pst_CurrentMatrix);
	p_stCamCoord = &stCamMatrix.T;

	fSaveU =  		MATH_f_DotProduct(p_stCamCoord , (MATH_tdst_Vector *)&p_BIG->PlanarMatrix[0]) + p_BIG->PlanarMatrix[0].w ;
	stResult . fV = MATH_f_DotProduct(p_stCamCoord , (MATH_tdst_Vector *)&p_BIG->PlanarMatrix[1]) + p_BIG->PlanarMatrix[1].w ;
	/* cast U & V */
	stResult . fU = p_MMUVF-> ab.x * fSaveU + p_MMUVF-> cd.x * stResult . fV + p_MMUVF-> T . x;
	stResult . fV = p_MMUVF-> ab.y * fSaveU + p_MMUVF-> cd.y * stResult . fV + p_MMUVF-> T . y;
	p_MMUVF-> T . x -= Gsp_CastUV(stResult . fU);
	p_MMUVF-> T . y -= Gsp_CastUV(stResult . fV);//*/
}

void Gsp_SetUVMatrix_Identity()
{
	register MyMatrixUVFormat *p_MMUVF;
	register MAT_tdst_Decompressed_UVMatrix *TDST_ummAT;
	register Gsp_BigStruct	  *p_BIG;
	p_BIG = &gs_st_Globals;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	
	p_MMUVF = Gsp_SetDrawBuffer_Matrix_UVS(p_BIG);
	TDST_ummAT = &MatrixUVIdentity;
	*(u_long64 *)&p_BIG -> stLastUVMatrix.ab = *(u_long64 *)&p_MMUVF->ab = *(u_long64 *)&TDST_ummAT->UVMatrix[0];
	*(u_long64 *)&p_BIG -> stLastUVMatrix.cd = *(u_long64 *)&p_MMUVF->cd = *(u_long64 *)&TDST_ummAT->UVMatrix[2];
	*(u_long64 *)&p_BIG -> stLastUVMatrix.T  = *(u_long64 *)&p_MMUVF->T = *(u_long64 *)&TDST_ummAT->AddU;//*/
	if (p_BIG->bPlanarGizmoEnable == 4) Gsp_restoreBugHightUV(p_BIG , p_MMUVF);
}

void Gsp_SetUVMatrix(MAT_tdst_Decompressed_UVMatrix *TDST_ummAT)
{
	register MyMatrixUVFormat *p_MMUVF;
	register Gsp_BigStruct	  *p_BIG;
	p_BIG = &gs_st_Globals;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	
	p_MMUVF = Gsp_SetDrawBuffer_Matrix_UVS(p_BIG);
	*(u_long64 *)&p_BIG -> stLastUVMatrix.ab = *(u_long64 *)&p_MMUVF->ab = *(u_long64 *)&TDST_ummAT->UVMatrix[0];
	*(u_long64 *)&p_BIG -> stLastUVMatrix.cd = *(u_long64 *)&p_MMUVF->cd = *(u_long64 *)&TDST_ummAT->UVMatrix[2];
	*(u_long64 *)&p_BIG -> stLastUVMatrix.T = *(u_long64 *)&p_MMUVF->T = *(u_long64 *)&TDST_ummAT->AddU;//*/
	if (p_BIG->bPlanarGizmoEnable == 4) Gsp_restoreBugHightUV(p_BIG , p_MMUVF);
}


void MulMatrixes_UV(MyMatrixUVFormat *Dst,MyMatrixUVFormat *M1,MyMatrixUVFormat *M2)
{
	Dst->ab . x = M1->ab . x * M2->ab . x + M1->ab . y * M2->cd . x ;
	Dst->ab . y = M1->ab . x * M2->ab . y + M1->ab . y * M2->cd . y ;
	Dst->cd . x = M1->cd . x * M2->ab . x + M1->cd . y * M2->cd . x ;
	Dst->cd . y = M1->cd . x * M2->ab . y + M1->cd . y * M2->cd . y ;
	Dst->T . x = M2->T . x * M1->ab . x + M2->T . y * M1->cd . x + M1->T . x;
	Dst->T . y = M2->T . x * M1->ab . y + M2->T . y * M1->cd . y + M1->T . y;
}

void Gsp_SetUVMatrixAndMul(MAT_tdst_Decompressed_UVMatrix *TDST_ummAT)
{
	register MyMatrixUVFormat *p_MMUVF;
	register Gsp_BigStruct	  *p_BIG;
	p_BIG = &gs_st_Globals;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	
	p_MMUVF = Gsp_SetDrawBuffer_Matrix_UVS(p_BIG);
	if (!TDST_ummAT)
	{
		*(u_long128 *)&p_MMUVF->ab = *(u_long128 *)&p_BIG->stLastUVMatrix.ab;
		*(u_long128 *)&p_MMUVF->cd = *(u_long128 *)&p_BIG->stLastUVMatrix.cd;
		*(u_long128 *)&p_MMUVF->T  = *(u_long128 *)&p_BIG->stLastUVMatrix.T;
	} else
	{
		/* Zivaletrucrelou */
		MyMatrixUVFormat Res;
		*(u_long64 *)&p_MMUVF->ab = *(u_long64 *)&TDST_ummAT->UVMatrix[0];
		*(u_long64 *)&p_MMUVF->cd = *(u_long64 *)&TDST_ummAT->UVMatrix[2];
		*(u_long64 *)&p_MMUVF->T = *(u_long64 *)&TDST_ummAT->AddU;//*/
		MulMatrixes_UV(&Res,p_MMUVF,&p_BIG->stLastUVMatrix);
		*(u_long128 *)&p_MMUVF->ab = *(u_long128 *)&Res.ab;
		*(u_long128 *)&p_MMUVF->cd = *(u_long128 *)&Res.cd;
		*(u_long128 *)&p_MMUVF->T  = *(u_long128 *)&Res.T;
		*(u_long128 *)&p_BIG->stLastUVMatrix.ab = *(u_long128 *)&p_MMUVF->ab;
		*(u_long128 *)&p_BIG->stLastUVMatrix.cd = *(u_long128 *)&p_MMUVF->cd;
		*(u_long128 *)&p_BIG->stLastUVMatrix.T = *(u_long128 *)&p_MMUVF->T;
	}
}

ULONG VU0_JumpTatble[6];
ULONG VU0_JumpTatble_STRIPS[6];
ULONG VU0_JumpTatble_SYM_STRIPS[6];


void Gsp_SetPlanarProjectionUVMatrix(MATH_tdst_Vector *VU,MATH_tdst_Vector *VV,MATH_tdst_Vector *VT)
{
	MyVectorFormat		*pPlanarMatrix;
	MATH_tdst_Vector 	NoVt;
	MyVectorFormat		*pPlanarMatrixSave;
	register Gsp_BigStruct	  *p_BIG;
	p_BIG = &gs_st_Globals;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);

	if(!VT) 
	{
		NoVt . x = NoVt . y = 0.0f;
		VT = &NoVt;
	}
	
	/* Formulae 
		U = VU ^ P + VT
		V = VV ^ P + VT
		VT will be stored in the UV matrix
	*/
	pPlanarMatrixSave = p_BIG->PlanarMatrix;
	pPlanarMatrixSave->x = VU->x;
	pPlanarMatrixSave->y = VU->y;
	pPlanarMatrixSave->z = VU->z;
	pPlanarMatrixSave->w = VT->x;
	pPlanarMatrixSave++;
	pPlanarMatrixSave->x = VV->x;
	pPlanarMatrixSave->y = VV->y;
	pPlanarMatrixSave->z = VV->z;
	pPlanarMatrixSave->w = VT->y;
	
	pPlanarMatrix = Gsp_SetDrawBuffer_PLANAR_UVS(p_BIG);
	*(u_int *)&(pPlanarMatrix + 0)->x = *(u_int *)&VU->x;
	*(u_int *)&(pPlanarMatrix + 0)->z = *(u_int *)&VU->y;
	*(u_int *)&(pPlanarMatrix + 1)->x = *(u_int *)&VU->z;
	*(u_int *)&(pPlanarMatrix + 0)->y = *(u_int *)&VV->x;
	*(u_int *)&(pPlanarMatrix + 0)->w = *(u_int *)&VV->y;
	*(u_int *)&(pPlanarMatrix + 1)->y = *(u_int *)&VV->z;
	*(u_long64 *)&(pPlanarMatrix + 1)->z = *(u_long64 *)&VT->x;
	Gsp_SetPlanarProjectionMode(4);
}
void Gsp_SetPlanarProjectionUVMatrix_For_Shadows(MATH_tdst_Vector *VU,MATH_tdst_Vector *VV,MATH_tdst_Vector *VT,MATH_tdst_Vector *VAxis,float fDist)
{
	MyVectorFormat		*pPlanarMatrix;
	MATH_tdst_Vector 	NoVt;
	MyVectorFormat		*pPlanarMatrixSave;
	u32			Num;
	register MyMatrixUVFormat *p_MMUVF;
	register MAT_tdst_Decompressed_UVMatrix *TDST_ummAT;
	register Gsp_BigStruct	  *p_BIG;
	p_BIG = &gs_st_Globals;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	
	Num = (p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase);
	if ((Num + p_BIG->MDB->Size > (RAM_BUF_SIZE - 0x1000))) 
		Gsp_RenderBuffer(p_BIG,Num);
	
	p_MMUVF = Gsp_SetDrawBuffer_Matrix_UVS(p_BIG);
	TDST_ummAT = &MatrixUVIdentity;
	*(u_long64 *)&p_MMUVF->ab = *(u_long64 *)&TDST_ummAT->UVMatrix[0];
	*(u_long64 *)&p_MMUVF->cd = *(u_long64 *)&TDST_ummAT->UVMatrix[2];
	*(u_long64 *)&p_MMUVF->T = *(u_long64 *)&TDST_ummAT->AddU;//*/
	
	p_MMUVF->ab.z = VAxis->x;
	p_MMUVF->ab.w = VAxis->y;
	p_MMUVF->cd.z = VAxis->z;
	p_MMUVF->cd.w = fDist;
	
	
	if(!VT) 
	{
		NoVt . x = NoVt . y = 0.0f;
		VT = &NoVt;
	}
	
	/* Formulae 
		U = VU ^ P + VT
		V = VV ^ P + VT
		VT will be stored in the UV matrix
	*/
	pPlanarMatrixSave = p_BIG->PlanarMatrix;
	pPlanarMatrixSave->x = VU->x;
	pPlanarMatrixSave->y = VU->y;
	pPlanarMatrixSave->z = VU->z;
	pPlanarMatrixSave->w = VT->x;
	pPlanarMatrixSave++;
	pPlanarMatrixSave->x = VV->x;
	pPlanarMatrixSave->y = VV->y;
	pPlanarMatrixSave->z = VV->z;
	pPlanarMatrixSave->w = VT->y;
	
	pPlanarMatrix = Gsp_SetDrawBuffer_PLANAR_UVS(p_BIG);
	*(u_int *)&(pPlanarMatrix + 0)->x = *(u_int *)&VU->x;
	*(u_int *)&(pPlanarMatrix + 0)->y = *(u_int *)&VV->x;
	*(u_int *)&(pPlanarMatrix + 0)->z = *(u_int *)&VU->y;
	*(u_int *)&(pPlanarMatrix + 0)->w = *(u_int *)&VV->y;
	*(u_int *)&(pPlanarMatrix + 1)->x = *(u_int *)&VU->z;
	*(u_int *)&(pPlanarMatrix + 1)->y = *(u_int *)&VV->z;
	*(u_long64 *)&(pPlanarMatrix + 1)->z = *(u_long64 *)&VT->x;
	Gsp_SetPlanarProjectionMode(6);
}

void Gsp_InitDrawBuffer(Gsp_tdst_DrawBuffer *MDB,u_int Base,u_int BuffSize)
{
	register Gsp_BigStruct	  *p_BIG;
	p_BIG = &gs_st_Globals;
	Base = GSP_ALIGN(Base);
	p_BIG->p_CurrentBufferPointer = Base;
	//XYZ
	p_BIG->p_CurrentBufferPointer += 16 + GSP_ALIGN(sizeof(MyVectorFormat) * BuffSize * 3);
	//COLORS
	p_BIG->p_CurrentBufferPointer += 16 + GSP_ALIGN(sizeof(u_int) * BuffSize * 3);
	//UV
	p_BIG->p_CurrentBufferPointer += 16 + GSP_ALIGN(sizeof(u_long) * BuffSize * 3);
	//p_COMPRESSORS
	p_BIG->p_CurrentBufferPointer += 16 + GSP_ALIGN(sizeof(u_char) * BuffSize * 3);
	//Matrix
	p_BIG->p_CurrentBufferPointer += 16 + GSP_ALIGN(sizeof(MyMatrixFormat));
	//Matrix UV
	p_BIG->p_CurrentBufferPointer += 16 + GSP_ALIGN(sizeof(MyMatrixUVFormat));
	//Planar UV
	p_BIG->p_CurrentBufferPointer += 16 + GSP_ALIGN(sizeof(MyVectorFormat) * 4);
	//Command
	p_BIG->p_CurrentBufferPointer += 16 + GSP_ALIGN(2 * 16);
	p_BIG->p_CurrentBufferPointer += 16;

	MDB->Size		= (u_int)p_BIG->p_CurrentBufferPointer - (u_int)Base;/* size */
	p_BIG->p_CurrentBufferPointer = Base;
}
void Gsp_SetDrawBuffer()
{
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	GspGlobal_ACCESS(MDB)->VIF_Matrix   = 0x01000404L | ((0x6c000000L/* V4_32 */ | (((u_long)sizeof(MyMatrixFormat  ) >> 4L) << 16L) | ((u_long)GSP_DB_MatrixBase  )) << 32L);
	GspGlobal_ACCESS(MDB)->VIF_LIGHT    = 0x01000404L | ((0x6c000000L/* V4_32 */ | (((u_long)sizeof(MyMatrixFormat  ) >> 4L) << 16L) | ((u_long)GSP_DB_LightBase)) << 32L);
	GspGlobal_ACCESS(MDB)->VIF_CMA   	= 0x01000404L | ((0x6c000000L/* V4_32 */ | (((u_long)sizeof(MyColorMulAdd  ) >> 4L) << 16L) | ((u_long)GSP_DB_Color_Mul_Add_Base  )) << 32L);
	GspGlobal_ACCESS(MDB)->VIF_FUR   	= 0x01000404L | ((0x6c000000L/* V4_32 */ | (((u_long)sizeof(MyVectorFormat  ) >> 4L) << 16L) | ((u_long)GSP_DB_FURBASE  )) << 32L);
	
	GspGlobal_ACCESS(MDB)->VIF_MatrixUV = 0x01000404L | ((0x6c000000L/* V4_32 */ | (((u_long)sizeof(MyMatrixUVFormat) >> 4L) << 16L) | ((u_long)GSP_DB_MatrixUVBase)) << 32L);
	GspGlobal_ACCESS(MDB)->VIF_Points	= 0x01000103L/* stcycl WL = 1 ; CL = 3 */ | ((0x6c000000L | ((u_long)0L << 16L) | ((u_long)GspGlobal_ACCESS(MDB)->Base + 2L))<< 32L);
	GspGlobal_ACCESS(MDB)->VIF_Colors 	= 0x01000103L/* stcycl WL = 1 ; CL = 3 */ | ((0x6e000000L /* V4_8  */| ((u_long)GspGlobal_ACCESS(MDB)->Base + 1L) /* Base */ | (1L << 14L)/* unsigned */ | ((u_long)0L << 16L))<<32L); 
	GspGlobal_ACCESS(MDB)->VIF_PointsUV = 0x01000103L/* stcycl WL = 1 ; CL = 3 */ | ((0x64000000L /* V2_32 */| (u_long)GspGlobal_ACCESS(MDB)->Base /* Base */ | ((u_long)0L << 16L)) << 32L); 
	GspGlobal_ACCESS(MDB)->VIF_PlanarUV = 0x01000404L/* stcycl WL = 4 ; CL = 4 */ | ((0x64000000L /* V2_32 */| (u_long)GSP_DB_PlanarMatrixUVBase /* Base */ | (4L << 16L)) << 32L); 
	GspGlobal_ACCESS(MDB)->VIF_COMPRESSOR = 0x01000103L/* stcycl WL = 1 ; CL = 3 */ | ((0x72000000L /* S_8 */| (u_long)(GspGlobal_ACCESS(MDB)->Base + 2L) /* same Base as points base */ | (1L << 14L)/* unsigned */) << 32L); 
	GspGlobal_ACCESS(MDB)->VIF_COMPRESSOR0= 0x3f3f3f3f20000000L; /* stMask & Mask : Mask all without W */
	GspGlobal_ACCESS(MDB)->VIF_Command1 = 0x01000404L /* stcycl WL = 4 ; CL = 4 */ | ((0x6c000000L /* V4_32*/| ((u_long)GSP_DB_CommandWBase ) /* Base */ | (1L << 16L))<<32L); /* UnPack */
	GspGlobal_ACCESS(MDB)->p_Command[0] = GspGlobal_ACCESS(Current_UV_Compute_VU0_Jump) ; // <- UV Compute
	GspGlobal_ACCESS(MDB)->p_Command[1] = 0 ; // <- number of triangles 
	GspGlobal_ACCESS(MDB)->p_Command[2] = GspGlobal_ACCESS(MDB)->Base; // VU1 MEM BASE SOURCE
	GspGlobal_ACCESS(MDB)->p_Command[3] = SCE_VIF1_SET_MARK(0xb00b,0); /* nothing */
	GspGlobal_ACCESS(MDB)->p_Command[4] = 0; /* VIF NOP->Must be set To the VIF CALL*/
	GspGlobal_ACCESS(MDB)->p_Command[5] = 0x11000000; /* FLUSH */
	GspGlobal_ACCESS(MDB)->p_Command[6] = 0x17000000; /* MSCNT */
	GspGlobal_ACCESS(MDB)->p_Command[7] = SCE_VIF1_SET_MARK(0xb00c,0); /* VIF NOP */
}
void Gsp_SetDrawBufferRenderState()
{
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	GspGlobal_ACCESS(MDB)->VIF_SRS		= 0x01000404L/* stcycl WL = 4 ; CL = 4 */ | ((0x6c000000L/* V4_32 */ | ((0L/*  + GIF TAG */) << 16L) | ((u_long)GspGlobal_ACCESS(MDB)->Base)) << 32L);
	GspGlobal_ACCESS(MDB)->VIF_Command2 = 0x01000404L/* stcycl WL = 4 ; CL = 4 */ | ((0x6c000000L/* V4_32*/| ((u_long)GSP_DB_CommandWBase2) /* Base */ | (1L << 16L))<<32L); /* UnPack */
	GspGlobal_ACCESS(MDB)->p_Command[2] = GspGlobal_ACCESS(MDB)->Base; // RS MEM BASE SOURCE
	GspGlobal_ACCESS(MDB)->p_Command[3] = 0; // RS MEM BASE SOURCE
	GspGlobal_ACCESS(MDB)->p_Command[4] = 0x15000004; /* mscall 4  (Render state)*/
	GspGlobal_ACCESS(MDB)->p_Command[5] = 0x11000000; /* FLUSH */
	GspGlobal_ACCESS(MDB)->p_Command[6] = 0x17000000; /* MSCNT */
	GspGlobal_ACCESS(MDB)->p_Command[7] = SCE_VIF1_SET_MARK(0xb004,0); /* NOP */
}
void Gsp_DrawBuffer_Prepare()
{
	GspGlobal_ACCESS(MDB) = &GspGlobal_ACCESS(MDB_TABLE)[1];
	Gsp_SetDrawBufferRenderState();
	Gsp_SetDrawBuffer();
	GspGlobal_ACCESS(MDB) = &GspGlobal_ACCESS(MDB_TABLE)[0];
	Gsp_SetDrawBufferRenderState();
	Gsp_SetDrawBuffer();
}

u_int GSP_CheckIfMemIsInCache(char *p_Mem, u_int Size )
{
	char *p_100Meg,*p_unCache,*p_unCacheLast;	
	p_100Meg = (char *)(1024 *1024 * 100);
	p_unCache = p_Mem;
	GSP_UnCachePtr(p_100Meg);
	GSP_UnCachePtr(p_unCache);
	p_unCacheLast = p_unCache + Size;
	while(p_unCache < p_unCacheLast)
	{
		*p_100Meg = *p_unCache;
		p_100Meg++;
		p_unCache++;
	}
	p_100Meg = (char *)(1024 *1024 * 100);
	GSP_UnCachePtr(p_100Meg);
	while(Size--)
	{
		if (*p_100Meg != *p_Mem)
		{
			return 1;
		}
		p_100Meg++;
		p_Mem++;
	}
	return 0;
}

void GSP_DrawBuf_EndWorldLoad()
{
//	GSP_CheckIfMemIsInCache((char *)GS_RAMBUFFER1 , sizeof(GS_RAMBUFFER1));
}
void Gsp_InitRingBuffer(u32 NumAtoms)
{
	RAMBUFFER1 = GS_RAMBUFFER1;
	L_memset(RAMBUFFER1 , 0xff , sizeof(GS_RAMBUFFER1));

	GSP_UnCacheAccPtr(RAMBUFFER1);
	L_memset(RAMBUFFER1 , 0xff , sizeof(GS_RAMBUFFER1));
	
	GspGlobal_ACCESS(p_CurrentBufferPointer) = GspGlobal_ACCESS(p_BufferPointerBase) = (u_int)RAMBUFFER1;
	
	L_memset(&GspGlobal_ACCESS(GSP_BX_VAR) , 0 , sizeof(GSP_BX_BUFFER));
	
	{
		u_int Counter;
		GspGlobal_ACCESS(GSP_BX_VAR).Buffer[0].p_BufferBase = RAMBUFFER1;
		for (Counter = 1 ; Counter < NumAtoms; Counter++)
		{
			GspGlobal_ACCESS(GSP_BX_VAR).Buffer[Counter].p_BufferBase = (u_int128 *)((u_int)GspGlobal_ACCESS(GSP_BX_VAR).Buffer[Counter - 1].p_BufferBase + RAM_BUF_SIZE);
			GspGlobal_ACCESS(GSP_BX_VAR).Buffer[Counter - 1 ].p_NextBuffer = &GspGlobal_ACCESS(GSP_BX_VAR).Buffer[Counter];

		}
		GspGlobal_ACCESS(GSP_BX_VAR).Buffer[NumAtoms - 1].p_NextBuffer = &GspGlobal_ACCESS(GSP_BX_VAR).Buffer[0];
		GspGlobal_ACCESS(GSP_BX_VAR).pCurrentDmaBuffer = GspGlobal_ACCESS(GSP_BX_VAR).pCurrentCpuBuffer = &GspGlobal_ACCESS(GSP_BX_VAR).Buffer[0];
	}
	*(u_int *)&GspGlobal_ACCESS(BX_LastCntPtr) = (u_int)GspGlobal_ACCESS(p_BufferPointerBase);
	GspGlobal_ACCESS(p_CurrentBufferPointer) = GspGlobal_ACCESS(p_BufferPointerBase) = GspGlobal_ACCESS(p_BufferPointerBase) + 16;
}
void Gsp_DrawBufferFirstInit()
{
	MEMpro_StartMemRaster();

	
	Gsp_InitRingBuffer(BX_NumberOfBuffers);	
	
	Gsp_InitDrawBuffer(&GspGlobal_ACCESS(MDB_TABLE)[0], (u_int)RAMBUFFER1 , DP_MaxNumT);
	Gsp_InitDrawBuffer(&GspGlobal_ACCESS(MDB_TABLE)[1], (u_int)RAMBUFFER1 , DP_MaxNumT);
	

	GspGlobal_ACCESS(MDB_TABLE)[0].Base = GSP_DB_Base1;
	GspGlobal_ACCESS(MDB_TABLE)[1].Base = GSP_DB_Base2;
	GspGlobal_ACCESS(MDB) = &GspGlobal_ACCESS(MDB_TABLE)[0];
	GspGlobal_ACCESS(bGSUpdateFlag) = 0;
	
	
	
/*	GspGlobal_ACCESS(MDB_TABLE)[0].p_CurrentRefDmaTag8 = GspGlobal_ACCESS(MDB_TABLE)[0].p_REFDMATAGS8;
	GspGlobal_ACCESS(MDB_TABLE)[1].p_CurrentRefDmaTag8 = GspGlobal_ACCESS(MDB_TABLE)[1].p_REFDMATAGS8;*/

	GspGlobal_ACCESS(MDB_TABLE)[0].p_OtherMDB = &GspGlobal_ACCESS(MDB_TABLE)[1];
	GspGlobal_ACCESS(MDB_TABLE)[1].p_OtherMDB = &GspGlobal_ACCESS(MDB_TABLE)[0];

	GspGlobal_ACCESS(ulNextVU1Jump) = 0x15000000; /* Call 0 */
	VU0_JumpTatble[0] = ((u_int)&UV_ComputeFinished - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble[1] = ((u_int)&UV_ObjectCompute_X - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble[2] = ((u_int)&UV_ObjectCompute_Y - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble[3] = ((u_int)&UV_ObjectCompute_Z - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble[4] = ((u_int)&UV_WorldCompute    - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble[5] = ((u_int)&UV_WorldCompute    - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble[6] = ((u_int)&UV_WorldCompute_For_Shadows    - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_STRIPS[0] = ((u_int)&STRIP_UV_ComputeFinished - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_STRIPS[1] = ((u_int)&STRIP_UV_ObjectCompute_X - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_STRIPS[2] = ((u_int)&STRIP_UV_ObjectCompute_Y - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_STRIPS[3] = ((u_int)&STRIP_UV_ObjectCompute_Z - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_STRIPS[4] = ((u_int)&STRIP_UV_WorldCompute    - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_STRIPS[5] = ((u_int)&STRIP_UV_WorldCompute    - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_STRIPS[6] = ((u_int)&STRIP_UV_WorldCompute_For_Shadows  - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_SYM_STRIPS[0] = ((u_int)&SYM_STRIP_UV_ComputeFinished - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_SYM_STRIPS[1] = ((u_int)&SYM_STRIP_UV_ObjectCompute_X - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_SYM_STRIPS[2] = ((u_int)&SYM_STRIP_UV_ObjectCompute_Y - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_SYM_STRIPS[3] = ((u_int)&SYM_STRIP_UV_ObjectCompute_Z - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_SYM_STRIPS[4] = ((u_int)&SYM_STRIP_UV_WorldCompute    - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_SYM_STRIPS[5] = ((u_int)&SYM_STRIP_UV_WorldCompute    - (u_int)&LABEL_FIRSTLINE) >> 3;
	VU0_JumpTatble_SYM_STRIPS[6] = ((u_int)&SYM_STRIP_UV_WorldCompute_For_Shadows  - (u_int)&LABEL_FIRSTLINE) >> 3;
	
	
	
	Gsp_DrawBuffer_Prepare(); /* Must be call after each SPR acces */
	Gsp_SetPlanarProjectionMode(0);
	MEMpro_StopMemRaster(MEMpro_Id_GSP);
}
_inline_ void GSP_SpecialStore(u_long Value , u_long *Adress)
{
#if 1
	Adress[0] = 0L;
	Adress[1] = Value;
#else
	asm ("
		.set noreorder
		pcpyld	Value,Value,zero
		sq		Value,0(Adress)
		.set reorder
		");
#endif
}
/*
typedef struct 
{
	u_short			qwc;		// transfer count
	u_char			mark;		// mark
	u_char			id;		// tag
	u_int			next;		// next tag
	u_int			p[2];		// padding
} GSP_DMA_Source_Chain_TAG __attribute__ ((aligned(16)));*/


u_long *Gsp_SetDrawBuffer_XYZW	( register Gsp_BigStruct *p_BIG , register Gsp_tdst_DrawBuffer 	*p_MDB,register u_int NumberOfPoints)
{
	register u_long *p_ReturnValue;
	register u_long NumberOfPoints48;
	if (p_BIG -> Current_XYZW)
	{
		CHECK_GSP_FREE();
		CHECK_CACHEBLOCK_XY(p_BIG -> Current_XYZW);
		NumberOfPoints48 = (u_long)NumberOfPoints << 48L;
		p_ReturnValue = (u_long *)p_BIG->p_CurrentBufferPointer;
		GSP_SpecialStore(p_MDB->VIF_Points | NumberOfPoints48 , p_ReturnValue);
		p_ReturnValue += 2;
		/* 1 WriteDma Tag */
		NumberOfPoints48 = (((u_int)p_ReturnValue - (u_int)p_BIG->BX_LastCntPtr) >> 4) - 1;
		GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , NumberOfPoints48 , 0 , p_BIG->BX_LastCntPtr);
		GSP_SpecialStoreDma(GSP_SCDma_ID_Ref , NumberOfPoints , ((u_int)p_BIG -> Current_XYZW -> DataBlock) & 0x0fffffff , (GSP_DMA_Source_Chain_TAG *)p_ReturnValue);
		GSP_SpecialStoreDma(GSP_SCDma_ID_End , 0 , 0 , ((GSP_DMA_Source_Chain_TAG *)p_ReturnValue) + 1);
		p_BIG->BX_LastCntPtr = ((GSP_DMA_Source_Chain_TAG *)p_ReturnValue) + 1;
		p_ReturnValue+=  4; // 2 dma tags
		/* Prepare next block */
		p_BIG -> Current_XYZW = p_BIG -> Current_XYZW -> p_stNextBlock;
		/* Signal no - write */
		p_BIG->p_Last_XYZW_pointer = NULL;
	} else
	{
		NumberOfPoints48 = (u_long)NumberOfPoints << 48L;
		p_ReturnValue = (u_long *)p_BIG->p_CurrentBufferPointer;
		GSP_SpecialStore(p_MDB->VIF_Points | NumberOfPoints48 , p_ReturnValue);
		p_ReturnValue += 2;
		p_BIG->p_Last_XYZW_pointer = (MyVectorFormat *)p_ReturnValue;
		p_ReturnValue += NumberOfPoints << 1;
	}
	return p_ReturnValue;
}

u_long *Gsp_SetDrawBuffer_UVS	( register Gsp_BigStruct *p_BIG , register Gsp_tdst_DrawBuffer 	*p_MDB,register u_int NumberOfPoints , register u_long *p_ReturnValue)
{
	register u_long NumberOfPoints48;
	if (p_BIG -> Current___UV)
	{
		CHECK_GSP_FREE();
		CHECK_CACHEBLOCK_UV(p_BIG -> Current___UV);
		NumberOfPoints = GSP_ALIGN_X(NumberOfPoints,2);
		NumberOfPoints48 = (u_long)NumberOfPoints << 48L;
		GSP_SpecialStore(p_MDB->VIF_PointsUV | NumberOfPoints48 , p_ReturnValue);
		p_ReturnValue += 2;
		/* 1 WriteDma Tag */
		NumberOfPoints48 = (((u_int)p_ReturnValue - (u_int)p_BIG->BX_LastCntPtr) >> 4) - 1;
		GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , NumberOfPoints48 , 0 , p_BIG->BX_LastCntPtr);
		GSP_SpecialStoreDma(GSP_SCDma_ID_Ref , NumberOfPoints>>1 , ((u_int)p_BIG -> Current___UV -> DataBlock) & 0x0fffffff , (GSP_DMA_Source_Chain_TAG *)p_ReturnValue);
		GSP_SpecialStoreDma(GSP_SCDma_ID_End , 0 , 0 , ((GSP_DMA_Source_Chain_TAG *)p_ReturnValue) + 1);
		p_BIG->BX_LastCntPtr = ((GSP_DMA_Source_Chain_TAG *)p_ReturnValue) + 1;
		p_ReturnValue+=  4; // 2 dma tags
		/* Prepare next block */
		p_BIG -> Current___UV = p_BIG -> Current___UV -> p_stNextBlock;
		/* Signal no - write */
		p_BIG->p_Last_UV_pointer = NULL;
	} else
	{
		NumberOfPoints = GSP_ALIGN_X(NumberOfPoints,2);
		NumberOfPoints48 = (u_long)NumberOfPoints << 48L;
		GSP_SpecialStore(p_MDB->VIF_PointsUV | NumberOfPoints48 , p_ReturnValue);
		p_ReturnValue += 2;
		p_BIG->p_Last_UV_pointer = (u_long *)p_ReturnValue;
		p_ReturnValue += NumberOfPoints;
	}
	return p_ReturnValue;
}


u_long *Gsp_SetDrawBuffer_FUR	( register Gsp_BigStruct *p_BIG , register Gsp_tdst_DrawBuffer 	*p_MDB,register u_int NumberOfPoints , register u_long *p_ReturnValue)
{
	register u_long NumberOfPoints48;
	extern u_int 	INFLATE_Start	__attribute__((section(".vudata")));
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	if (p_BIG -> Current___CC)
	{
		NumberOfPoints = GSP_ALIGN_X(NumberOfPoints,4);
		NumberOfPoints48 = (u_long)NumberOfPoints << 48L;
		GSP_SpecialStore(p_MDB->VIF_Colors | NumberOfPoints48 , p_ReturnValue);
		p_ReturnValue += 2;
		/* 1 WriteDma Tag */
		NumberOfPoints48 = (((u_int)p_ReturnValue - (u_int)p_BIG->BX_LastCntPtr) >> 4) - 1;
		GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , NumberOfPoints48 , 0 , p_BIG->BX_LastCntPtr);
		GSP_SpecialStoreDma(GSP_SCDma_ID_Ref , NumberOfPoints>>2 , ((u_int)p_BIG -> Current___CC -> DataBlock) & 0x0fffffff , (GSP_DMA_Source_Chain_TAG *)p_ReturnValue);
		GSP_SpecialStoreDma(GSP_SCDma_ID_End , 0 , 0 , ((GSP_DMA_Source_Chain_TAG *)p_ReturnValue) + 1);
		p_BIG->BX_LastCntPtr = ((GSP_DMA_Source_Chain_TAG *)p_ReturnValue) + 1;
		p_ReturnValue+=  4; // 2 dma tags
		/* Prepare next block */
		p_BIG -> Current___CC = p_BIG -> Current___CC -> p_stNextBlock;
		/* Signal no - write */
		p_BIG->p_Last_FUR_pointer = NULL;
	} else
#endif		
	{
		NumberOfPoints = GSP_ALIGN_X(NumberOfPoints,4);
		NumberOfPoints48 = (u_long)NumberOfPoints << 48L;
		GSP_SpecialStore(p_MDB->VIF_Colors | NumberOfPoints48 , p_ReturnValue);
		p_ReturnValue += 2;
		p_BIG->p_Last_FUR_pointer = (u_int *)p_ReturnValue;
		p_ReturnValue += NumberOfPoints >> 1;
	} 
	
	*(u_long128 *)p_ReturnValue = Gsp_64_to_128(p_BIG->MDB->VIF_FUR);
	p_ReturnValue+=2;
	*(u_long128 *)p_ReturnValue = *(u_long128 *)&p_BIG->stCurrentFUR_SizeInX_Undef_Undef_Undef;
	p_ReturnValue+=2;
	p_MDB->p_Command[0] = p_BIG->Current_UV_Compute_VU0_Jump; // <- UV Compute
	p_MDB->p_Command[1] = NumberOfPoints ; // <- number
	p_MDB->p_Command[4] = 0x15000000 | (((u_int)&INFLATE_Start - (u_int)&LABEL_FIRSTLINE) >> 3 ); 
	p_MDB->p_Command[7] = SCE_VIF1_SET_MARK(0xb010,0); /* NOP */
	
	*(u_long128 *)&p_ReturnValue[0] = Gsp_64_to_128(p_MDB->VIF_Command1);
	*(u_long128 *)&p_ReturnValue[2] = ((u_long128 *)p_MDB->p_Command)[0];
	*(u_long128 *)&p_ReturnValue[4] = ((u_long128 *)p_MDB->p_Command)[1];//*/

	return p_ReturnValue + 6;
}
u_long *Gsp_SetDrawBuffer_COLORS	( register Gsp_BigStruct *p_BIG , register Gsp_tdst_DrawBuffer 	*p_MDB, register u_int NumberOfPoints , register u_int *p_Colors , register u_long *p_ReturnValue)
{
	register u_long NumberOfPoints48;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	if (p_BIG -> Current___CC)
	{
		NumberOfPoints = GSP_ALIGN_X(NumberOfPoints,4);
		NumberOfPoints48 = (u_long)NumberOfPoints << 48L;
		GSP_SpecialStore(p_MDB->VIF_Colors | NumberOfPoints48 , p_ReturnValue);
		p_ReturnValue += 2;
		/* 1 WriteDma Tag */
		NumberOfPoints48 = (((u_int)p_ReturnValue - (u_int)p_BIG->BX_LastCntPtr) >> 4) - 1;
		GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , NumberOfPoints48 , 0 , p_BIG->BX_LastCntPtr);
		GSP_SpecialStoreDma(GSP_SCDma_ID_Ref , NumberOfPoints>>2 , ((u_int)p_BIG -> Current___CC -> DataBlock) & 0x0fffffff , (GSP_DMA_Source_Chain_TAG *)p_ReturnValue);
		GSP_SpecialStoreDma(GSP_SCDma_ID_End , 0 , 0 , ((GSP_DMA_Source_Chain_TAG *)p_ReturnValue) + 1);
		p_BIG->BX_LastCntPtr = ((GSP_DMA_Source_Chain_TAG *)p_ReturnValue) + 1;
		p_ReturnValue+=  4; // 2 dma tags
		/* Prepare next block */
		p_BIG -> Current___CC = p_BIG -> Current___CC -> p_stNextBlock;
		/* Signal no - write */
		p_BIG->p_Last_COLOR_pointer = NULL;
	} else
#endif		
	if (p_Colors)	
	{
		NumberOfPoints = GSP_ALIGN_X(NumberOfPoints,4);
		NumberOfPoints48 = (u_long)NumberOfPoints << 48L;
		GSP_SpecialStore(p_MDB->VIF_Colors | NumberOfPoints48 , p_ReturnValue);
		p_ReturnValue += 2;
		p_BIG->p_Last_COLOR_pointer = (u_int *)p_ReturnValue;
		p_ReturnValue += NumberOfPoints >> 1;
	} else 
	{
		p_BIG->p_Last_COLOR_pointer = NULL;
	}
	return p_ReturnValue;
}


MyVectorFormat 	*Gsp_SetDrawBuffer_XYZ_FUR_COLORS_UV	( Gsp_BigStruct *p_BIG , u_int NumberOfPoints , u_int *p_Colors , void *p_UV)
{
	register u_long *p_ReturnValue;
	register Gsp_tdst_DrawBuffer 	*p_MDB;
	p_MDB = p_BIG->MDB;
	p_BIG->p_Last_FUR_pointer = p_BIG->p_Last_COLOR_pointer = p_BIG->p_Last_UV_pointer = NULL;
	
	p_ReturnValue = Gsp_SetDrawBuffer_XYZW	( p_BIG , p_MDB , NumberOfPoints);
	p_ReturnValue = Gsp_SetDrawBuffer_FUR	( p_BIG , p_MDB , NumberOfPoints , p_ReturnValue);
	if (p_Colors) p_ReturnValue = Gsp_SetDrawBuffer_COLORS	( p_BIG , p_MDB , NumberOfPoints , p_Colors , p_ReturnValue);
	if (p_UV) 	p_ReturnValue = Gsp_SetDrawBuffer_UVS	( p_BIG , p_MDB ,NumberOfPoints , p_ReturnValue);
	p_BIG->p_CurrentBufferPointer = (u32)p_ReturnValue;
	return (MyVectorFormat *)p_BIG->p_Last_XYZW_pointer;
}
MyVectorFormat 	*Gsp_SetDrawBuffer_XYZ_COLORS_UV	( Gsp_BigStruct *p_BIG , u_int NumberOfPoints , u_int *p_Colors)
{
	register u_long *p_ReturnValue;
	register Gsp_tdst_DrawBuffer 	*p_MDB;
	p_MDB = p_BIG->MDB;
	p_ReturnValue = Gsp_SetDrawBuffer_XYZW	( p_BIG , p_MDB ,NumberOfPoints);
	p_ReturnValue = Gsp_SetDrawBuffer_COLORS	( p_BIG , p_MDB ,NumberOfPoints , p_Colors , p_ReturnValue);
	p_BIG->p_CurrentBufferPointer = (u32)Gsp_SetDrawBuffer_UVS	( p_BIG , p_MDB ,NumberOfPoints , p_ReturnValue);
	return (MyVectorFormat *)p_BIG->p_Last_XYZW_pointer;
}
MyVectorFormat 	*Gsp_SetDrawBuffer_XYZ_COLORS	(Gsp_BigStruct *p_BIG , u_int NumberOfPoints , u_int *p_Colors)
{
	register u_long *p_ReturnValue;
	register Gsp_tdst_DrawBuffer 	*p_MDB;
	p_MDB = p_BIG->MDB;
	p_ReturnValue = Gsp_SetDrawBuffer_XYZW	( p_BIG , p_MDB ,NumberOfPoints);
	p_BIG->p_CurrentBufferPointer = (u32)Gsp_SetDrawBuffer_COLORS	( p_BIG , p_MDB ,NumberOfPoints , p_Colors , p_ReturnValue);
	return (MyVectorFormat *)p_BIG->p_Last_XYZW_pointer;
}
#ifndef _FINAL_
u32 DEbuigLezouf = 0;
u32 VIFPAss = 0;
u32 CurrenVIFCODE = 0;
u32 LastMARK = 0;
void Send_To_VU1Engine(u32 VIFCODE)
{
	u32 Vn,Vl;
	if (VIFPAss) 
	{
		VIFPAss--;
		return;
	}
	if (((VIFCODE>>29) & 0x3) == 3) 
	{
		Vn = (VIFCODE >> 26) & 0x3;
		Vl = (VIFCODE >> 24) & 0x3;
		VIFCODE &= 0x00FFffff;
		VIFCODE |= 0x70000000;
	}
	switch ((VIFCODE>>24) & 0x7f)
	{
		case 0x00://NOP
		case 0x10://FLUSHE
		case 0x11://FLUSH
		case 0x13://FLUSHA
		case 0x01://STCYCL
		case 0x14://MSCAL
		case 0x17://MSCNT
		case 0x15://MSCALF
		case 0x20://STMASK
		case 0x30://STROW
		case 0x31://STCOL
		case 0x05://STMOD
		case 0x03://BASE
		case 0x02://OFFSET
		case 0x04://ITOP
		case 0x06://MSKPATH3
			VIFPAss = 0; 
				break;
		case 0x07://MARK
			VIFPAss = 0; 
			LastMARK = VIFCODE & 0xffff;
			break;
				
		case 0x4A://MPG
		default:
			VIFPAss = 0; 
			break;
		case 0x50:
				VIFPAss = ((VIFCODE & 0xffff) << 2); 
			  	if (VIFPAss == 0) 
			  		*(u32 *)0 = *(u32 *)4;//Send a crash message
			  	break;//DIRECT

		case 0x51:
				VIFPAss = ((VIFCODE & 0xffff) << 2); 
			  	if (VIFPAss == 0) 
			  		*(u32 *)0 = *(u32 *)4;//Send a crash message
			  	break;//DIRECTHL
		case 0x70:
				VIFPAss = 32 >> Vl;
				VIFPAss = VIFPAss * (Vn+1); // Size in bits;
				VIFPAss = VIFPAss * ((VIFCODE & 0xff0000)>>16); // total Size in bits;
				VIFPAss >>= 5; // Size in 32b
				break;//UNPACK
	}
	CurrenVIFCODE = VIFCODE;
}
GSP_DMA_Source_Chain_TAG *GSP_Buf_GetNext(GSP_DMA_Source_Chain_TAG *pTagAddress)
{
	u32 Counter,*VIF;
	switch (pTagAddress -> id >> 4)
	{
		case 0:break;//REFE:CRASH
		case 1:
			Counter = pTagAddress -> qwc << 2;
			VIF = (u32*)(pTagAddress+1);
			while (Counter--) Send_To_VU1Engine(*(VIF++));
			return (GSP_DMA_Source_Chain_TAG *)VIF;
			break;//cnt
		case 2:break;//NEXT:CRASH
		case 3:
			Counter = pTagAddress -> qwc << 2;
			VIF = (u32*)(pTagAddress->next);
			while (Counter--) Send_To_VU1Engine(*(VIF++));
			return (GSP_DMA_Source_Chain_TAG *)pTagAddress + 1 ;
			break;//REF
		case 4:break;//REFS:CRASH
		case 5:return (GSP_DMA_Source_Chain_TAG *)pTagAddress+pTagAddress -> qwc + 1;break;//CALL
		case 6:break;//RET
		case 7:
			Counter = pTagAddress -> qwc << 2;
			VIF = (u32*)(pTagAddress+1);
			while (Counter--) Send_To_VU1Engine(*(VIF++));
			return NULL;
			break;//END
	}
	return (GSP_DMA_Source_Chain_TAG *)1; // Crash request
}
u32 GSP_CheckBuffer(GSP_DMA_Source_Chain_TAG *pTagAddress)
{
	while (pTagAddress)
	{
		pTagAddress = GSP_Buf_GetNext(pTagAddress);
	};
	return DEbuigLezouf;
}
#endif
_inline_ void Gsp_Pulse_BX(volatile GSP_BX	*p_Buffer , Gsp_BigStruct	  *p_BIG)
{
#ifdef GSP_PS2_BENCH
	p_BIG->ulRotativBufferTurns++;
	_GSP_BeginRaster(38);
#endif	
#ifdef USE_BUFFER_IN_OTHER_PLACE
	if (p_Buffer->BX_State != BX_READY_FOR_DMA)
		*(u_int *)0 = *(u_int *)4;
#define DESDBUG	(90L * 1024L * 1024L + 0x30000000L)
	/* DBUG */
	{
		u_long128 *Src,*Dst;
		u32 Counter;
		Src = (u_long128 *)((u32)p_Buffer->p_BufferBase );
		Dst = (u_long128 *)((u32)DESDBUG			    );
		Counter = RAM_BUF_SIZE >> 4;
		Counter-- ;
		while (Counter--)
		{
			*(Dst++) = *(Src++);
		}
	}
#else
#define DESDBUG p_Buffer->p_BufferBase
#endif

	p_Buffer->BX_State = BX_USED_BY_DMA;
//	__asm__ volatile("sync.l");
	*D1_TADR = (ULONG)DESDBUG & 0x0fffffff;
	gsulSyncroPath |= FLUSH_DMA1;
	*D1_CHCR = (1/*From	 memory */ | (1<<2)/*chain Mode*/ | /*Start */ (1<<8) | /* 1 adress pushed */(1<<4));
}
void Gsp_Pulse_BX_Back( volatile GSP_BX	*p_Buffer , Gsp_BigStruct	  *p_BIG)
{
	p_Buffer->BX_State = BX_USED_BY_DMA;
	*D1_MADR = (ULONG)p_Buffer->BX_BackAddress & 0x0fffffff;
	*D1_QWC = (ULONG)p_Buffer->BX_BackSize;
	gsulSyncroPath |= FLUSH_DMA1;
	*D1_CHCR = (0/*to memory */ | (0<<2)/*normal Mode*/ | /*Start */ (1<<8));
}

#pragma	nofpregforblkmv	on
int Gsp_DMA1End(int)
{
	volatile GSP_BX	*p_Buffer;
	Gsp_BigStruct	  *p_BIG;
	Interrupt_Entry();	
	gsulSyncroPath &= ~FLUSH_DMA1;
	p_BIG = &gs_st_Globals;
	p_Buffer = p_BIG->GSP_BX_VAR.pCurrentDmaBuffer;
#ifdef GSP_USE_TRIPLE_BUFFERRING	
	if (p_Buffer->BX_Option)
	{
		extern u_int volatile GSPDMASEMAPHORE;
		extern u_int volatile TRIPLE_BIGSWAP;
		GSPDMASEMAPHORE = 0;
		TRIPLE_BIGSWAP = p_Buffer->BX_Option;
		p_Buffer->BX_Option = 0;
	}
#endif	
	
#ifdef GSP_PS2_BENCH
	_GSP_EndRaster(38);
#endif	
	if (p_Buffer->BX_State == BX_USED_BY_DMA)
	{
		if (p_Buffer->BX_BackSize)
		{
			if (p_Buffer->BX_BackSize == 0xC0DE2012) // After effect BW
			{
				u_int Infinite;
#ifdef GSP_PS2_BENCH
				_GSP_BeginRaster(38);
#endif	
				Infinite = 10000000;
				while (( (DGET_GS_CSR() & GS_CSR_FINISH_M) == 0 ) && Infinite) 
				{
					Infinite--;
				};
				p_Buffer->BX_BackSize = 0;
#ifdef GSP_PS2_BENCH
				_GSP_EndRaster(38);
#endif	
				
			} else
			if (p_Buffer->BX_BackSize != 0xC0DE2002)
			{
				u_int Infinite;
#ifdef GSP_PS2_BENCH
				_GSP_BeginRaster(38);
#endif	
				/* Check the Finish Event here */
				Infinite = 10000000;
				while (( (DGET_GS_CSR() & GS_CSR_FINISH_M) == 0 ) && Infinite) 
				{
					Infinite--;
				};
				// change VIF1-FIFO transfer direction (VIF1 -> MAIN MEM or SPR)
				*VIF1_STAT = 0x00800000;
				DPUT_GS_BUSDIR((u_long)0x00000001);
				Gsp_Pulse_BX_Back(p_Buffer , p_BIG); // GS -> RAM
				p_Buffer->BX_BackSize = 0xC0DE2002;
			} else
			{
				// change VIF1-FIFO transfer direction (MAIN MEM or SPR -> VIF1)
				*VIF1_STAT = 0;
				// change GS bus direction (HOST->LOCAL)
				DPUT_GS_BUSDIR((u_long)0);
				// set the FINISH event
				DPUT_GS_CSR( GS_CSR_FINISH_M );
				p_Buffer->BX_BackSize = 0;
			}
		} 
		if (!p_Buffer->BX_BackSize)
		{
			p_Buffer->BX_State = BX_READY_FOR_CPU;
			p_BIG->GSP_BX_VAR.pCurrentDmaBuffer = p_Buffer = p_Buffer->p_NextBuffer;
#ifndef BUFFER_FULL_TEST
			if (p_Buffer->BX_State == BX_READY_FOR_DMA)
			{
				Gsp_Pulse_BX(p_Buffer , p_BIG); // // RAM -> GS
			}
#endif		
		}
	}
	Interrupt_Exit();
	ExitHandler();
   	return 0;
}
#pragma	nofpregforblkmv	reset

void Gsp_DrawBuf_JustAfterFlip()
{
	volatile GSP_BX	*p_Buffer;
	p_Buffer = GspGlobal_ACCESS(GSP_BX_VAR).pCurrentDmaBuffer; 
	while (p_Buffer->BX_State >= BX_READY_FOR_DMA_VSYNC_BASE)
	{
		p_Buffer->BX_State --;
		p_Buffer = p_Buffer->p_NextBuffer;
	}
	p_Buffer = GspGlobal_ACCESS(GSP_BX_VAR).pCurrentDmaBuffer; 
	if (!(gsulSyncroPath & FLUSH_DMA1)) 
		if (p_Buffer->BX_State == BX_READY_FOR_DMA)
		{
			Gsp_Pulse_BX(p_Buffer , &gs_st_Globals);
		}
	
}
void Gsp_ImpulseBX(Gsp_BigStruct	*p_BIG)
{
	if (!(gsulSyncroPath & FLUSH_DMA1)) 
		if (p_BIG->GSP_BX_VAR.pCurrentDmaBuffer->BX_State == BX_READY_FOR_DMA)
		{
			Gsp_Pulse_BX(p_BIG->GSP_BX_VAR.pCurrentDmaBuffer,p_BIG);
		}
}


void Gsp_RenderBuffer(register Gsp_BigStruct	  *p_BIG , register ULONG Num)
{
	GSP_BX	*p_Buffer;
	p_Buffer = p_BIG->GSP_BX_VAR.pCurrentCpuBuffer;
	p_Buffer->BX_Size = Num >> 4;
//	*(u_int *)(p_BIG->p_CurrentBufferPointer + 64) = 0;
#ifdef DRAWBUF_DEBUG_MODE
	if ((u32)p_BIG->p_CurrentBufferPointer >= (((u32)p_Buffer->p_BufferBase) + RAM_BUF_SIZE))
		*(u_int *)0 = *(u_int *)4;
	if (Num > RAM_BUF_SIZE - 16)
		*(u_int *)0 = *(u_int *)4;
	if (p_Buffer->BX_State != BX_READY_FOR_CPU)
		*(u_int *)0 = *(u_int *)4;
	if ((u32)p_BIG->p_CurrentBufferPointer & 0xf)
		*(u_int *)0 = *(u_int *)4;
		
#endif

	{
		GSP_DMA_Source_Chain_TAG *p_unCachedDmaTag;
		extern volatile u_int ulHiddenFloatingFlipFlag;
		GSP_SpecialStoreDma(GSP_SCDma_ID_End , (((u_int)p_BIG->p_CurrentBufferPointer - (u_int)p_BIG->BX_LastCntPtr) >> 4) - 1 , 0 , p_BIG->BX_LastCntPtr);
#ifndef GSP_USE_TRIPLE_BUFFERRING	
		if (p_BIG->ulFloatingFlipFlag)
			p_Buffer->BX_State = BX_READY_FOR_DMA_VSYNC_BASE + ulHiddenFloatingFlipFlag;
		else//*/
#endif
		
		{
			p_Buffer->BX_State = BX_READY_FOR_DMA;
#ifdef GSP_BUF_CHECK	
			GSP_CheckBuffer((GSP_DMA_Source_Chain_TAG *)p_Buffer->p_BufferBase);
#endif	
			Gsp_ImpulseBX(p_BIG);
		}
	}
	p_BIG->GSP_BX_VAR.pCurrentCpuBuffer = p_Buffer = p_Buffer->p_NextBuffer;
	if (p_Buffer->BX_State != BX_READY_FOR_CPU)
	{
		u_int Infinite;
		extern void GSP_RestoreCorrectPath(u_int Mode);
		extern void Gsp_Begin_WaitDL();
		extern void Gsp_End_WaitDL();
		Gsp_Begin_WaitDL();
		Infinite = 50000000;
		while ((p_Buffer->BX_State != BX_READY_FOR_CPU) && Infinite) 
		{
			Infinite--;
		};
		if (!Infinite) 
		{
			GSP_RestoreCorrectPath(1);
			p_Buffer = p_BIG->GSP_BX_VAR.pCurrentCpuBuffer;
		}
		Gsp_End_WaitDL();
	}
	*(u_int*)&p_BIG->BX_LastCntPtr = (u_int)p_Buffer->p_BufferBase;
	p_BIG->p_CurrentBufferPointer = p_BIG->p_BufferPointerBase = ((u_int)p_Buffer->p_BufferBase) + 16;
#ifdef DRAWBUF_DEBUG_MODE
	/* DBUG */
	{
		u_long128 Fill,*Pos;
		u32 Counter;
		((u32 *)&Fill)[0] = ((u32 *)&Fill)[1] = ((u32 *)&Fill)[2] = ((u32 *)&Fill)[3] = -1;
		Counter = RAM_BUF_SIZE >> 4;
		Counter-- ;
		Pos = (u_long128*)p_BIG->p_CurrentBufferPointer;
		while (Counter--)
			*(Pos++) = Fill;
	}
#endif	
#ifdef GSP_FULLSYNCHRO	
	{
		ULONG infinite;
		infinite = 10000000;
		while ((gsulSyncroPath & FLUSH_DMA1) && infinite)
		{
			infinite--;
		}
		if (!infinite)
		{
			*(u_int *)0 = *(u_int *)0;
		}
	}
#endif
}

void Gsp_FIFO_STOP()
{
	Gsp_BigStruct	  *p_BIG;
	p_BIG = &gs_st_Globals;
	if (p_BIG->p_CurrentBufferPointer > p_BIG->p_BufferPointerBase)
	{
		Gsp_RenderBuffer(p_BIG , p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase);
	}
}
void Gsp_FIFO_STOP_FINAL()
{
	Gsp_BigStruct	  *p_BIG;
	p_BIG = &gs_st_Globals;
	if (p_BIG->p_CurrentBufferPointer > p_BIG->p_BufferPointerBase)
	{
		Gsp_RenderBuffer(p_BIG , p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase);
	}
}
// Not clipped Symetrie Mode
void Gsp_Water_FullCompute_Hook_NC(Gsp_BigStruct	  *p_BIG,  u_int NumberOfPrims)
{
	u_long128 *p_ReturnValue;
	Gsp_tdst_DrawBuffer *p_MDB;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	
	{
		// Set render state reflexion
		p_ReturnValue = (u_long128 *)p_BIG->p_CurrentBufferPointer;
		p_MDB = p_BIG -> MDB;

		((u_int *)p_ReturnValue)[0] = SCE_VIF1_SET_MARK(0xb005,0); // VIF NOP 
		((u_int *)p_ReturnValue)[1] = 0;
		((u_int *)p_ReturnValue)[2] = 0x11000000; // FLUSH
		((u_int *)p_ReturnValue)[3] = 0x50000000 | 3; // DIRECT 
		p_ReturnValue++;
		Gsp_M_SetGifTag((GspGifTag *)p_ReturnValue, 2 , 1 , 0 , 0 , GSP_GIF_FLG_PACKED , 1 ,   0xE);
		p_ReturnValue++;
		((MyRegister *)p_ReturnValue)->ulRegister = (unsigned long)GSP_PRIM;
		((MyRegister *)p_ReturnValue)->ulValueToWrite = (unsigned long)4;
		p_ReturnValue++;

		((MyRegister *)p_ReturnValue)->ulRegister = (unsigned long)GSP_TEST_1;
		((MyRegister *)p_ReturnValue)->ulValueToWrite = p_BIG->last_GSP_TEST_1 | (1L << 14L) | (1L << 15L) ;	
		p_ReturnValue++;
//*/		
		p_MDB->p_Command[0] = VU0_JumpTatble_SYM_STRIPS[p_BIG->bPlanarGizmoEnable];
		p_MDB->p_Command[1] = NumberOfPrims ; // <- number
		p_MDB->p_Command[4] = p_BIG->ulNextVU1Jump + p_BIG->bGSUpdateFlag + 32 ;			 // Call Secnd Pass 
		p_MDB->p_Command[7] = SCE_VIF1_SET_MARK(0xb020,0); /* NOP */
		p_ReturnValue[0] = Gsp_64_to_128(p_MDB->VIF_Command1);
		p_ReturnValue[1] = ((u_long128 *)p_MDB->p_Command)[0];
		p_ReturnValue[2] = ((u_long128 *)p_MDB->p_Command)[1];
		p_ReturnValue += 3;//*/
		
		// Set render state normal
		((u_int *)p_ReturnValue)[0] = SCE_VIF1_SET_MARK(0xb006,0); // VIF NOP 
		((u_int *)p_ReturnValue)[1] = 0;
		((u_int *)p_ReturnValue)[2] = 0x11000000; // FLUSH 
		((u_int *)p_ReturnValue)[3] = 0x50000000 | 3; // DIRECT 
		p_ReturnValue++;
		Gsp_M_SetGifTag((GspGifTag *)p_ReturnValue, 2 , 1 , 0 , 0 , GSP_GIF_FLG_PACKED , 1 ,   0xE);
		p_ReturnValue++;
		((MyRegister *)p_ReturnValue)->ulRegister = (unsigned long)GSP_PRIM;
		((MyRegister *)p_ReturnValue)->ulValueToWrite = (unsigned long)4;
		p_ReturnValue++;
		((MyRegister *)p_ReturnValue)->ulRegister = (unsigned long)GSP_TEST_1;
		((MyRegister *)p_ReturnValue)->ulValueToWrite = p_BIG->last_GSP_TEST_1 | (1L << 14L);	
		p_ReturnValue++;
//*/		
		p_BIG->p_CurrentBufferPointer = (u_int)(p_ReturnValue);
	}
}
// Clipped Symetrie Mode
void Gsp_Water_FullCompute_Hook(Gsp_BigStruct	  *p_BIG,  u_int NumberOfPrims)
{
	 u_long128 *p_ReturnValue;
	 Gsp_tdst_DrawBuffer *p_MDB;
	 ULONG ulCDM;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	
	ulCDM = GDI_gpst_CurDD_SPR.ul_CurrentDrawMask;
	if (ulCDM & GDI_Cul_DM_ReflectOnWater)
	{
		MyMatrixFormat 						*p_MyMatrix;
		if (!(p_BIG->ulCurrentCullingMask & OBJ_Culling_Z_OverLap))
		{
			Gsp_Water_FullCompute_Hook_NC(p_BIG,  NumberOfPrims);
			return;
		}
		p_MDB = p_BIG -> MDB;
		/* Save GIF Tag in point 0*/
		p_MyMatrix = Gsp_SetDrawBuffer_Matrix(p_BIG, p_MDB );
		
		*(u_long128 *)&p_MyMatrix -> I = *(u_long128 *)&GspGlobal_ACCESS(stSymetrisedMatrix[0]);
		*(u_long128 *)&p_MyMatrix -> J = *(u_long128 *)&GspGlobal_ACCESS(stSymetrisedMatrix[1]);
		*(u_long128 *)&p_MyMatrix -> K = *(u_long128 *)&GspGlobal_ACCESS(stSymetrisedMatrix[2]);
		*(u_long128 *)&p_MyMatrix -> T = *(u_long128 *)&GspGlobal_ACCESS(stSymetrisedMatrix[3]);
		*(u_long128 *)&p_MyMatrix -> AxAyxxZM = *(u_long128 *)&GspGlobal_ACCESS(stCurrentAxAyxxZM);
		*(u_long128 *)&p_MyMatrix -> CxCyCzFC = *(u_long128 *)&GspGlobal_ACCESS(stCurrentCxCyCzFC);
		
		// Set render state reflexion

		p_ReturnValue = (u_long128 *)p_BIG->p_CurrentBufferPointer;

		((u_int *)p_ReturnValue)[0] = SCE_VIF1_SET_MARK(0xb007,0); // VIF NOP 
		((u_int *)p_ReturnValue)[1] = 0;
		((u_int *)p_ReturnValue)[2] = 0x11000000; // FLUSH 
		((u_int *)p_ReturnValue)[3] = 0x50000000 | 3; // DIRECT 
		p_ReturnValue++;
		Gsp_M_SetGifTag((GspGifTag *)p_ReturnValue, 2 , 1 , 0 , 0 , GSP_GIF_FLG_PACKED , 1 ,   0xE);
		p_ReturnValue++;
		((MyRegister *)p_ReturnValue)->ulRegister = (unsigned long)GSP_PRIM;
		((MyRegister *)p_ReturnValue)->ulValueToWrite = (unsigned long)4;
		p_ReturnValue++;
		((MyRegister *)p_ReturnValue)->ulRegister = (unsigned long)GSP_TEST_1;
		((MyRegister *)p_ReturnValue)->ulValueToWrite = p_BIG->last_GSP_TEST_1 | (1L << 14L) | (1L << 15L) ;	
		p_ReturnValue++;
//*/
		p_MDB->p_Command[0] = p_BIG->Current_UV_Compute_VU0_Jump;
		p_MDB->p_Command[1] = NumberOfPrims ; // <- number
		p_MDB->p_Command[4] = p_BIG->ulNextVU1Jump + p_BIG->bGSUpdateFlag;			 // Call Secnd Pass 
		p_MDB->p_Command[7] = SCE_VIF1_SET_MARK(0xb030,0); /* NOP */
		p_ReturnValue[0] = Gsp_64_to_128(p_MDB->VIF_Command1);
		p_ReturnValue[1] = ((u_long128 *)p_MDB->p_Command)[0];
		p_ReturnValue[2] = ((u_long128 *)p_MDB->p_Command)[1];
		p_ReturnValue += 3;//*/
		
		// Set render state normal
///*		
		((u_int *)p_ReturnValue)[0] = SCE_VIF1_SET_MARK(0xb008,0); // VIF NOP 
		((u_int *)p_ReturnValue)[1] = 0;
		((u_int *)p_ReturnValue)[2] = 0x11000000; // FLUSH 
		((u_int *)p_ReturnValue)[3] = 0x50000000 | 3; // DIRECT 
		p_ReturnValue++;
		Gsp_M_SetGifTag((GspGifTag *)p_ReturnValue, 2 , 1 , 0 , 0 , GSP_GIF_FLG_PACKED , 1 ,   0xE);
		p_ReturnValue++;
		((MyRegister *)p_ReturnValue)->ulRegister = (unsigned long)GSP_PRIM;
		((MyRegister *)p_ReturnValue)->ulValueToWrite = (unsigned long)4;
		p_ReturnValue++;
		((MyRegister *)p_ReturnValue)->ulRegister = (unsigned long)GSP_TEST_1;
		((MyRegister *)p_ReturnValue)->ulValueToWrite = p_BIG->last_GSP_TEST_1 | (1L << 14L);	
		p_ReturnValue++;//*/
//*/		
		p_BIG->p_CurrentBufferPointer = (u_int)(p_ReturnValue);

		p_MyMatrix = Gsp_SetDrawBuffer_Matrix( p_BIG , p_MDB );
		p_MDB = p_BIG -> MDB;
		
		*(u_long128 *)&p_MyMatrix -> I = *(u_long128 *)&GspGlobal_ACCESS(stCurrentMatrix[0]);
		*(u_long128 *)&p_MyMatrix -> J = *(u_long128 *)&GspGlobal_ACCESS(stCurrentMatrix[1]);
		*(u_long128 *)&p_MyMatrix -> K = *(u_long128 *)&GspGlobal_ACCESS(stCurrentMatrix[2]);
		*(u_long128 *)&p_MyMatrix -> T = *(u_long128 *)&GspGlobal_ACCESS(stCurrentMatrix[3]);
		*(u_long128 *)&p_MyMatrix -> AxAyxxZM = *(u_long128 *)&GspGlobal_ACCESS(stCurrentAxAyxxZM);
		*(u_long128 *)&p_MyMatrix -> CxCyCzFC = *(u_long128 *)&GspGlobal_ACCESS(stCurrentCxCyCzFC);
		
	}
}
#ifdef GSP_PS2_BENCH
u8	CurrentTicksInDraw;
u8	CurrentTicksInEngine;
u8	CurrentTicksInGlobals;
#endif
void Gsp_SetColorMulAdd(Gsp_BigStruct	  *p_BIG)
{
	
	 u_long128 *p_ReturnValue;
#ifdef GSP_PS2_BENCH	
	extern int ShowNormals;
	if(ShowNormals)
	{
		if (((ShowNormals & 3) != 2) && (GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency))
		{
			p_BIG->stCurrentCMA.ColorMul.x = p_BIG->stCurrentCMA.ColorMul.y = p_BIG->stCurrentCMA.ColorMul.z = p_BIG->stCurrentCMA.ColorMul.w = 0;
			p_BIG->stCurrentCMA.ColorMul.x = p_BIG->stCurrentCMA.ColorMul.y = p_BIG->stCurrentCMA.ColorMul.z = p_BIG->stCurrentCMA.ColorMul.w = 0;
		} else
		if ((ShowNormals & 3) == 3)
		{
			p_BIG->stCurrentCMA.ColorMul.x = p_BIG->stCurrentCMA.ColorMul.y = p_BIG->stCurrentCMA.ColorMul.z = p_BIG->stCurrentCMA.ColorMul.w = 0;
			p_BIG->stCurrentCMA.ColorAdd.z = (float)CurrentTicksInGlobals;
			p_BIG->stCurrentCMA.ColorAdd.x = 255.0f - p_BIG->stCurrentCMA.ColorAdd.z;//(float)CurrentTicksInEngine;
			p_BIG->stCurrentCMA.ColorAdd.y = 255.0f - p_BIG->stCurrentCMA.ColorAdd.z;
			p_BIG->stCurrentCMA.ColorAdd.x = p_BIG->stCurrentCMA.ColorAdd.y = p_BIG->stCurrentCMA.ColorAdd.w = 0;
			
		} /*else 
		if ((ShowNormals & 3) == 2)
		{
			p_BIG->stCurrentCMA.ColorMul.x = p_BIG->stCurrentCMA.ColorMul.y = p_BIG->stCurrentCMA.ColorMul.z = p_BIG->stCurrentCMA.ColorMul.w = 0;
			p_BIG->stCurrentCMA.ColorAdd.y = (float)CurrentTicksInDraw;
			p_BIG->stCurrentCMA.ColorAdd.x = 255.0f - p_BIG->stCurrentCMA.ColorAdd.y;//(float)CurrentTicksInEngine;
			p_BIG->stCurrentCMA.ColorAdd.z = 255.0f - p_BIG->stCurrentCMA.ColorAdd.y;
			p_BIG->stCurrentCMA.ColorAdd.x = p_BIG->stCurrentCMA.ColorAdd.z = p_BIG->stCurrentCMA.ColorAdd.w = 0;
		} */else 
		if ((ShowNormals & 3) == 1)
		{
			p_BIG->stCurrentCMA.ColorMul.x = p_BIG->stCurrentCMA.ColorMul.y = p_BIG->stCurrentCMA.ColorMul.z = p_BIG->stCurrentCMA.ColorMul.w = 0;
			p_BIG->stCurrentCMA.ColorAdd.x = (float)CurrentTicksInEngine;
			p_BIG->stCurrentCMA.ColorAdd.y = 255.0f - p_BIG->stCurrentCMA.ColorAdd.x;//(float)CurrentTicksInEngine;
			p_BIG->stCurrentCMA.ColorAdd.z = 255.0f - p_BIG->stCurrentCMA.ColorAdd.x;
			p_BIG->stCurrentCMA.ColorAdd.y = p_BIG->stCurrentCMA.ColorAdd.z = p_BIG->stCurrentCMA.ColorAdd.w = 0;
		}
		if (ShowNormals & 4) 
		{
			p_BIG->stCurrentCMA.ColorAdd.x *= p_BIG->stCurrentCMA.ColorAdd.x  / 255.0f; 
			p_BIG->stCurrentCMA.ColorAdd.y *= p_BIG->stCurrentCMA.ColorAdd.y  / 255.0f; 
			p_BIG->stCurrentCMA.ColorAdd.z *= p_BIG->stCurrentCMA.ColorAdd.z  / 255.0f; 
			p_BIG->stCurrentCMA.ColorAdd.w *= p_BIG->stCurrentCMA.ColorAdd.w  / 255.0f; 
		}
	}
#endif	
	p_ReturnValue = (u_long128 *)p_BIG->p_CurrentBufferPointer;
	*(p_ReturnValue++) = Gsp_64_to_128(p_BIG->MDB->VIF_CMA);
	*(p_ReturnValue++) = *(u_long128 *)&p_BIG->stCurrentCMA.ColorMul;
	*(p_ReturnValue++) = *(u_long128 *)&p_BIG->stCurrentCMA.ColorAdd;
	p_BIG->p_CurrentBufferPointer = (u_int)(p_ReturnValue);
}

void Gsp_DrawHook_SetFirstStrip(Gsp_BigStruct	  *p_BIG,  u_int NumberOfPrims)
{
	 u_long128 *p_ReturnValue;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);

	p_ReturnValue = (u_long128 *)p_BIG->p_CurrentBufferPointer;
	((u_int *)p_ReturnValue)[0] = SCE_VIF1_SET_MARK(0xb009,0); /* VIF NOP */
	((u_int *)p_ReturnValue)[1] = 0;
	((u_int *)p_ReturnValue)[2] = 0x11000000; /* FLUSH */
	((u_int *)p_ReturnValue)[3] = 0x50000000 | 2; /* DIRECT */
	p_ReturnValue++;
	Gsp_M_SetGifTag((GspGifTag *)p_ReturnValue, 1 , 1 , 0 , 0 , GSP_GIF_FLG_PACKED , 1 , 0xEL);
	p_ReturnValue++;
	((MyRegister *)p_ReturnValue)->ulRegister = (unsigned long)GSP_PRIM;
	((MyRegister *)p_ReturnValue)->ulValueToWrite = (unsigned long)4;
	p_ReturnValue++;
	((u_int *)p_ReturnValue)[0] = SCE_VIF1_SET_MARK(0xb00a,0); /* VIF NOP */
	((u_int *)p_ReturnValue)[1] = 0;
	((u_int *)p_ReturnValue)[2] = 0; 
	((u_int *)p_ReturnValue)[3] = 0x11000000; /* DIRECT FLUSH */
	p_ReturnValue++;
	
	p_BIG->p_CurrentBufferPointer = (u_int)(p_ReturnValue);
	
	p_BIG->Gsp_DrawHook = p_BIG->Gsp_DrawHook2;
	p_BIG->Gsp_DrawHook2 = NULL;
	if (p_BIG->Gsp_DrawHook) p_BIG->Gsp_DrawHook(p_BIG , NumberOfPrims);
}
void Gsp_SetDrawBuffer_CMD1(Gsp_BigStruct	  *p_BIG , u_int NumberOfPrims)
{
	 u_long128 *p_ReturnValue;
	 ULONG Num;
	 Gsp_tdst_DrawBuffer 	*p_MDB;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	
	if (p_BIG->bSRSMustBeUodate) 
	{
		GSP_SetTextureBlending12(p_BIG);
	}//*/	
	
	if (p_BIG->Gsp_DrawHook) p_BIG->Gsp_DrawHook(p_BIG , NumberOfPrims);
	
	p_MDB = p_BIG->MDB;
	
	p_MDB->p_Command[0] = p_BIG->Current_UV_Compute_VU0_Jump; // <- UV Compute
	p_MDB->p_Command[1] = NumberOfPrims ; // <- number
	p_MDB->p_Command[4] = p_BIG->ulNextVU1Jump + p_BIG->bGSUpdateFlag ;			 /* Call 0 */
	p_MDB->p_Command[7] = SCE_VIF1_SET_MARK(0xb040,0); /* NOP */
	p_ReturnValue = (u_long128 *)p_BIG->p_CurrentBufferPointer;
	p_ReturnValue[0] = Gsp_64_to_128(p_MDB->VIF_Command1);
	p_ReturnValue[1] = ((u_long128 *)p_MDB->p_Command)[0];
	p_ReturnValue[2] = ((u_long128 *)p_MDB->p_Command)[1];//*/
	p_BIG->p_CurrentBufferPointer = (u_int)(p_ReturnValue + 3);
	
	p_BIG->bGSUpdateFlag = 0;
	Num = (p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase);
	p_BIG->MDB = p_MDB = p_MDB->p_OtherMDB;
	if ((Num + p_MDB->Size > (RAM_BUF_SIZE - 0x1000))) 
		Gsp_RenderBuffer(p_BIG,Num);
}

void Gsp_Send_To_VU1_Ref(u_int pAdress , ULONG ulSize)
{
	 Gsp_BigStruct	  *p_BIG;
	p_BIG = &gs_st_Globals;
	
	/* Close current*/	
	GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , (((u_int)p_BIG->p_CurrentBufferPointer - (u_int)p_BIG->BX_LastCntPtr) >> 4) - 1 , 0 , p_BIG->BX_LastCntPtr);
	/* Call adress */
	GSP_SpecialStoreDma(GSP_SCDma_ID_Ref , ulSize , (u_int)pAdress , (GSP_DMA_Source_Chain_TAG *)p_BIG->p_CurrentBufferPointer);
	GSP_SpecialStoreDma(GSP_SCDma_ID_End , 0 , 0 , ((GSP_DMA_Source_Chain_TAG *)p_BIG->p_CurrentBufferPointer) + 1);
	/* Setup new buffer base */
	*(u_int *)&p_BIG->BX_LastCntPtr = ((u_int)p_BIG->p_CurrentBufferPointer) + 16;
	*(u_int *)&p_BIG->p_CurrentBufferPointer += 32;
}

void Gsp_Send_To_VU1_Call(u_int pAdress)
{
	 Gsp_BigStruct	  *p_BIG;
	p_BIG = &gs_st_Globals;
	/* Close current*/	
	GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , (((u_int)p_BIG->p_CurrentBufferPointer - (u_int)p_BIG->BX_LastCntPtr) >> 4) - 1 , 0 , p_BIG->BX_LastCntPtr);
	/* Call adress */
	GSP_SpecialStoreDma(GSP_SCDma_ID_Call , 0 , (u_int)pAdress , (GSP_DMA_Source_Chain_TAG *)p_BIG->p_CurrentBufferPointer);
	GSP_SpecialStoreDma(GSP_SCDma_ID_End , 0 , 0 , ((GSP_DMA_Source_Chain_TAG *)p_BIG->p_CurrentBufferPointer) + 1);
	/* Setup new buffer base */
	*(u_int *)&p_BIG->BX_LastCntPtr = ((u_int)p_BIG->p_CurrentBufferPointer) + 16;
	*(u_int *)&p_BIG->p_CurrentBufferPointer += 32;
}
void GSP_Vu1LoadCode(u_int Label)
{
	static u_int PreviousLabel = 0;
	if (PreviousLabel != Label)
	{
		Gsp_Send_To_VU1_Call(Label);
	}
	PreviousLabel = Label;
}

#if defined PSX2_TARGET && defined __cplusplus
}
#endif


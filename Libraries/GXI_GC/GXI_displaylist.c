//  =============================================================================
//  (C) Copyright 2001 Ubi Soft
//  =============================================================================
//
//  Description   : Displaylist fonctionality 
//
//  Author        : Dany Joannette
//  Date          : 09 Nov 2001
//
//  =============================================================================

#include "GEOmetric/GEOobject.h"
#include "GXI_init.h"
#include "GXI_displaylist.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"

#include "GXI_tex.h"
#include "GXI_font.h"
#include "GXI_render.h"
#include "GEOmetric/GEO_STRIP.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"

#include "GXI_bench.h"

u32 g_nbStripOverOne=0;
extern u32  *p_AfterFXBuffer;

void GXI_DL_Initialize(tdstDisplayList *dl)
{
	dl->data = 0;
	dl->count = 0;
	dl->nAttributes = 3;
	dl->nbStripOverOne = 0;
	dl->IsIndex8 = 0;
}

void GXI_DL_SetNbAttributes(tdstDisplayList *dl, u8 nb)
{
	dl->nAttributes=nb;
}

void GXI_DL_SetCount(tdstDisplayList *dl, u16 count)
{
	dl->count = count;
}

u32 GXI_DL_GetByteSize(tdstDisplayList *dl)
{
	return 3 + (3*dl->nbStripOverOne) + dl->count*dl->nAttributes;
}

u32 GXI_DL_GetPaddedByteSize(tdstDisplayList *dl)
{
	u32 res = GXI_DL_GetByteSize(dl);
	return (res+32) & (~31);
}

char *p_PointerForDisplayLists;
void GXI_DL_Allocate(tdstDisplayList *dl)
{
	dl->data = p_PointerForDisplayLists;
	p_PointerForDisplayLists += GXI_DL_GetPaddedByteSize(dl);
}

void GXI_DL_Free_FROM_OBJ(tdstDisplayList *dl)
{
#ifdef GC_BENCHGRAPH
	u32 ulPreviousSize;
	ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
	MEM_FreeAlign(dl);	
	GXI_Global_ACCESS(ulDisplayLST) -= ulPreviousSize - MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
#else
	MEM_FreeAlign(dl);	
#endif	
}

u16* GXI_DL_Lock(tdstDisplayList *dl)
{
	return (u16*)(dl->data+3);
}

void GXI_DL_Unlock(tdstDisplayList *dl)
{
	DCFlushRange(dl->data, GXI_DL_GetPaddedByteSize(dl));
}

void GXI_DL_SetPrimitiveAndFormat(tdstDisplayList *dl, u8 command, GXVtxFmt format)
{
	*dl->data = command | format;
}

GXVtxFmt GXI_DL_GetFormat(tdstDisplayList *dl)
{
	return (GXVtxFmt)(*dl->data & 0x07); // 3 least significant bits are the format
}

GXPrimitive GXI_DL_GetPrimitive(tdstDisplayList *dl)
{
	return (GXPrimitive)(*dl->data & ~(0x07)); // 5 most significant bits are the primitive type
}

BOOL GXI_DL_NeedNormal(tdstDisplayList *dl)
{
	return FALSE; // DJ_TEMP : not used right now
}

BOOL GXI_DL_NeedColor(tdstDisplayList *dl)
{
	return TRUE; // DJ_TEMP : ???
}
u16 *GXI_CopyIndex_TRI(u16 *pDest , GEO_tdst_IndexedTriangle	*t , BOOL Nrmcoltex , BOOL ui , u32 ulNumber )
{
	GEO_tdst_IndexedTriangle	*tLast;
	tLast = t + ulNumber;
	if (Nrmcoltex)
	{
		if (ui)
		{
			register u16 *pRedirectBuffer;
			pRedirectBuffer = (u16 *)p_AfterFXBuffer;//GDI_gpst_CurDD->pst_ComputingBuffers->aus_RedirectBuffer;
			while (t < tLast)
			{
				*(pDest++) = t->auw_Index[0];
				*(pDest++) = t->auw_Index[0];
				*(pDest++) = pRedirectBuffer[t->auw_Index[0]];
				*(pDest++) = t->auw_UV[0];
				*(pDest++) = t->auw_Index[1];
				*(pDest++) = t->auw_Index[1];
				*(pDest++) = pRedirectBuffer[t->auw_Index[1]];
				*(pDest++) = t->auw_UV[1];
				*(pDest++) = t->auw_Index[2];
				*(pDest++) = t->auw_Index[2];
				*(pDest++) = pRedirectBuffer[t->auw_Index[2]];
				*(pDest++) = t->auw_UV[2];
				t++;
			}
		} else
		{
			while (t < tLast)
			{
				*(pDest++) = t->auw_Index[0];
				*(pDest++) = t->auw_Index[0];
				*(pDest++) = t->auw_Index[0];
				*(pDest++) = t->auw_UV[0];
				*(pDest++) = t->auw_Index[1];
				*(pDest++) = t->auw_Index[1];
				*(pDest++) = t->auw_Index[1];
				*(pDest++) = t->auw_UV[1];
				*(pDest++) = t->auw_Index[2];
				*(pDest++) = t->auw_Index[2];
				*(pDest++) = t->auw_Index[2];
				*(pDest++) = t->auw_UV[2];
				t++;
			}
		}
	} else
	{
		while (t < tLast)
		{
			*(pDest++) = t->auw_Index[0];
			*(pDest++) = t->auw_Index[1];
			*(pDest++) = t->auw_Index[2];
			t++;
		}
	}
	return pDest;
}
u8 *GXI_CopyIndex_STRIP(u16 *pDest , GEO_tdst_MinVertexData *t , BOOL Nrmcoltex , BOOL ui , u32 ulNumber )
{
	GEO_tdst_MinVertexData	*tLast;
	tLast = t + ulNumber;
	if (Nrmcoltex)
	{
		if (ui)
		{
			register u16 *pRedirectBuffer;
			pRedirectBuffer = (u16*)p_AfterFXBuffer;//GDI_gpst_CurDD->pst_ComputingBuffers->aus_RedirectBuffer;
			while (t < tLast)
			{
				*(pDest++) = (t)->auw_Index;
				*(pDest++) = (t)->auw_Index;//Nrm
				*(pDest++) = pRedirectBuffer[(t)->auw_Index];//Col
				*(pDest++) = (t++)->auw_UV;//Tex
			}
		} else
		{
			while (t < tLast)
			{
				*(pDest++) = (t)->auw_Index;
				*(pDest++) = (t)->auw_Index;//Nrm
				*(pDest++) = (t)->auw_Index;//Col
				*(pDest++) = (t++)->auw_UV;//Tex
			}
		}
	} else
	{
		while (t < tLast) *(pDest++) = (t++)->auw_Index;
	}
	return pDest;
}

void GXI_STRIP_GetMaxIndex(GEO_tdst_MinVertexData *t , u32 ulNumber , BOOL ui , u32 *pulMaxIndex, u32 *pulMaxIndexCol, u32 *pulMaxIndexTex )
{
	GEO_tdst_MinVertexData	*tLast;
	tLast = t + ulNumber;
	if (ui)
	{
		register u16 *pRedirectBuffer;
		pRedirectBuffer = (u16*)p_AfterFXBuffer;//GDI_gpst_CurDD->pst_ComputingBuffers->aus_RedirectBuffer;
		while (t < tLast)
		{
			*pulMaxIndex = lMax(*pulMaxIndex , (t)->auw_Index);
			*pulMaxIndexTex = lMax(*pulMaxIndexTex , (t)->auw_UV);
			*pulMaxIndexCol = lMax(*pulMaxIndexCol , pRedirectBuffer[(t)->auw_Index]);
			t++;
		}
	} else
	{
		while (t < tLast)
		{

			*pulMaxIndex = lMax(*pulMaxIndex , (t)->auw_Index);
			*pulMaxIndexTex = lMax(*pulMaxIndexTex , (t)->auw_UV);
			t++;
		}
		*pulMaxIndexCol = *pulMaxIndex;
	}
}

u16 *GXI_CopyIndex_STRIP_8(u8 *pDest , GEO_tdst_MinVertexData *t , BOOL ui , u32 ulNumber , u32 IndexMode)
{
	GEO_tdst_MinVertexData	*tLast;
	tLast = t + ulNumber;
	if (ui)
	{
		register u16 *pRedirectBuffer;
		pRedirectBuffer = (u16 *)p_AfterFXBuffer;//GDI_gpst_CurDD->pst_ComputingBuffers->aus_RedirectBuffer;
		while (t < tLast)
		{
			if (IndexMode & 0x80000000)
			{
				*(pDest++) = (t)->auw_Index;
				*(pDest++) = (t)->auw_Index;//Nrm
			} else
			{
				*(((u16 *)pDest)) = (t)->auw_Index;
				pDest+=2;
				*(((u16 *)pDest)) = (t)->auw_Index;//Nrm
				pDest+=2;
			}
			if (IndexMode & 0x40000000)
			{
				*(pDest++) = pRedirectBuffer[(t)->auw_Index];//Col
			} else
			{
				*(((u16 *)pDest)) = pRedirectBuffer[(t)->auw_Index];//Col
				pDest+=2;
			}
			if (IndexMode & 0x20000000)
			{
				*(pDest++) = (t)->auw_UV;//Tex
			} else
			{
				*(((u16 *)pDest)) = (t)->auw_UV;//Tex
				pDest+=2;
			}
			t++;
		}
	} else
	{
		while (t < tLast)
		{
			if (IndexMode & 0x80000000)
			{
				*(pDest++) = (t)->auw_Index;
				*(pDest++) = (t)->auw_Index;//Nrm
			} else
			{
				*(((u16 *)pDest)) = (t)->auw_Index;
				pDest+=2;
				*(((u16 *)pDest)) = (t)->auw_Index;//Nrm
				pDest+=2;
			}
			if (IndexMode & 0x40000000)
			{
				*(pDest++) = (t)->auw_Index;//Col
			} else
			{
				*(((u16 *)pDest)) = (t)->auw_Index;//Col
				pDest+=2;
			}
			if (IndexMode & 0x20000000)
			{
				*(pDest++) = (t)->auw_UV;//Tex
			} else
			{
				*(((u16 *)pDest)) = (t)->auw_UV;//Tex
				pDest+=2;
			}
			t++;
		}
	}
	return pDest;
}
u32 GXI_VerifyDisplayList(unsigned char *Dsplist , u32 FlagSize)
{
	u32 Pitch;
	u32 *pLast;
	Pitch = 8;
	if (FlagSize & 0x80000000) Pitch -= 2;
	if (FlagSize & 0x40000000) Pitch -= 1;
	if (FlagSize & 0x20000000) Pitch -= 1;
	FlagSize &= 0x0fffffff;
	pLast = Dsplist + FlagSize;
	while (Dsplist < pLast)
	{
		if  (*Dsplist == (GX_TRIANGLESTRIP | GX_VTXFMT0))
		{
			u16 lNbVertex;
			Dsplist++;
			lNbVertex = *(u16 *)Dsplist;
			Dsplist += 2;
			Dsplist += lNbVertex * Pitch;
		} else
		{
			u8 BUG;
			BUG = *Dsplist;
			return 0;
		}
		while ((*Dsplist == 0) && (Dsplist < pLast)) Dsplist++;
	}
	return 1;
}



u32 GXI_GetDisplayListSize(tdstDisplayList	*dl,GEO_tdst_ElementIndexedTriangles *_pst_Element);
void GXI_ComputeDisplayLists(GEO_tdst_Object *_pst_Object)
{
	GEO_tdst_ElementIndexedTriangles *pst_Element,*pst_ElementLast,*pFirstElem;
	u32 AllSize;
	char *RealAllocated;
	
	/* 1 Compute All size */
	pFirstElem = NULL;
	AllSize = 0 ;
	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object ->l_NbElements;
	while (pst_Element < pst_ElementLast)
	{
		if (pst_Element->pst_StripData)
		{
			if(pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			{
				tdstDisplayList	newdl;
				GXI_DL_Initialize(&newdl);
				AllSize += GXI_GetDisplayListSize(&newdl,pst_Element);
				if (pFirstElem == NULL) pFirstElem = pst_Element;
			}
		}
		pst_Element++;
	}
	{
#ifdef GC_BENCHGRAPH
			u32 ulPreviousSize;
			ulPreviousSize = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
			if(AllSize)
			{
				RealAllocated = p_PointerForDisplayLists = (u8*)MEM_p_AllocAlignMem(AllSize, 32, &MEM_gst_GCMemoryInfo);
				L_memset(p_PointerForDisplayLists , 0 , AllSize);
				GXI_Global_ACCESS(ulDisplayLST) += MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated - ulPreviousSize;
			}
			else
			{
				return;
				/*
				p_PointerForDisplayLists = NULL;						
				RealAllocated = 0;
				*/
			}
#else
			RealAllocated = p_PointerForDisplayLists = (u8*)MEM_p_AllocAlignMem(AllSize, 32, &MEM_gst_GCMemoryInfo);
			L_memset(p_PointerForDisplayLists , 0 , AllSize);
#endif
	}
	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object ->l_NbElements;
	while (pst_Element < pst_ElementLast)
	{
		if (pst_Element->pst_StripData)
		{
			if(pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			{
				tdstDisplayList	newdl;
				GXI_DL_Initialize(&newdl);
				GXI_DL_fill_with_geometric_object(&newdl, pst_Element);
				pst_Element->dl = newdl.data;
				pst_Element->dl_size = GXI_DL_GetPaddedByteSize(&newdl) | newdl.IsIndex8;
			}
		}
		pst_Element++;
	}
	{
	  u32 Verfy;
	  Verfy = (u32)p_PointerForDisplayLists - (u32)RealAllocated;
	  DCFlushRange(RealAllocated, Verfy);	
	  GXInvalidateVtxCache();
	}
	if (pFirstElem) pFirstElem->dl_size |= 0x10000000;
	
/*	
	pst_Element = _pst_Object ->dst_Element;
	pst_ElementLast = pst_Element + _pst_Object ->l_NbElements;
	while (pst_Element < pst_ElementLast)
	{
		if (pst_Element->pst_StripData)
		{
			if(pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			{
				GXI_VerifyDisplayList(pst_Element->dl , pst_Element->dl_size);
    		}
    		pst_Element++;
    	}
    }
*/	
	 
}

u32 GXI_GetDisplayListSize(tdstDisplayList	*dl,GEO_tdst_ElementIndexedTriangles *_pst_Element)
{
	u8   						nAttrib; // the position is not optional
	BOOL 						bStrip;
	u32 						MaxTex,MaxCol,MaxInd;
	GEO_tdst_OneStrip			*pStrip, *pStripEnd;
	u32 						nbvertex;

	GXI_DL_Initialize(dl);

	if (_pst_Element->ul_NumberOfUsedIndex)
		GXI_ComputeRedirectBuffer((u16 *)p_AfterFXBuffer,_pst_Element->pus_ListOfUsedIndex , _pst_Element->ul_NumberOfUsedIndex);
	
	if(_pst_Element->pst_StripData != NULL)
	{
		if(_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			bStrip = TRUE;
		else
			bStrip = FALSE;
	}
	else
		bStrip = FALSE;
	
	dl->IsIndex8 = 0;
	nAttrib = 8; // indx(short) + col(short) + nrm(short) + tex(short) 
	dl->nbStripOverOne = 0;
	nbvertex = 0;
	
	if (bStrip)
	{
		MaxTex = MaxCol = MaxInd = 0;
		pStrip = _pst_Element->pst_StripData->pStripList;
		pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;
		for(; pStrip < pStripEnd; )
		{
			if (_pst_Element->pus_ListOfUsedIndex)
			{
				GXI_STRIP_GetMaxIndex(pStrip->pMinVertexDataList , pStrip->ulVertexNumber , TRUE  , &MaxInd , &MaxCol , &MaxTex);
			}
			else
				GXI_STRIP_GetMaxIndex(pStrip->pMinVertexDataList , pStrip->ulVertexNumber , FALSE , &MaxInd , &MaxCol , &MaxTex);
	   	    pStrip++;
		}
		if (MaxInd < 255) 
		{
			dl->IsIndex8 |= 0x80000000;
			nAttrib-=2;
		};
		if (MaxCol < 255) 
		{
			dl->IsIndex8 |= 0x40000000;
			nAttrib--;
		};
		if (MaxTex == 65535) MaxTex = 0;
		if (MaxTex < 255) 
		{
			dl->IsIndex8 |= 0x20000000;
			nAttrib--;
		};//*/

		pStrip = _pst_Element->pst_StripData->pStripList;
		pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;
		
		for(; pStrip < pStripEnd; )
		{
			nbvertex += pStrip->ulVertexNumber;			
			pStrip++;
			if(pStrip < pStripEnd) dl->nbStripOverOne++;
		}
	}
	else
		nbvertex = _pst_Element->l_NbTriangles * 3;
		
	GXI_DL_SetCount(dl, nbvertex);

	GXI_DL_SetNbAttributes(dl, nAttrib);
	
	return GXI_DL_GetPaddedByteSize(dl);
}


void GXI_DL_fill_with_geometric_object(tdstDisplayList *dl, GEO_tdst_ElementIndexedTriangles *_pst_Element)
{
	u16							* stream;	
	BOOL 						bStrip;
	u8 							* mini_stream;
	GEO_tdst_OneStrip			*pStrip, *pStripEnd;

	GXI_GetDisplayListSize(dl,_pst_Element);
	
	if(_pst_Element->pst_StripData != NULL)
	{
		if(_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			bStrip = TRUE;
		else
			bStrip = FALSE;
	}
	else
		bStrip = FALSE;
	
	
	GXI_DL_Allocate(dl);
	if(dl->data) *((u16*)(dl->data + 1)) = dl->count;

	GXI_DL_SetPrimitiveAndFormat(dl, bStrip ? GX_TRIANGLESTRIP : GX_TRIANGLES, GX_VTXFMT0);

	stream = GXI_DL_Lock(dl);
	
	if(!bStrip)
	{
		if (_pst_Element->pus_ListOfUsedIndex)
			GXI_CopyIndex_TRI(stream , _pst_Element->dst_Triangle , TRUE , TRUE , _pst_Element->l_NbTriangles );
		else
			GXI_CopyIndex_TRI(stream , _pst_Element->dst_Triangle , TRUE , FALSE , _pst_Element->l_NbTriangles );
	}
	else
	{
		pStrip = _pst_Element->pst_StripData->pStripList;
		pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;

		// set the real size of the first strip
		
		if(dl->nbStripOverOne)
			*((u16*)(dl->data + 1)) = pStrip->ulVertexNumber;
		
		for(; pStrip < pStripEnd; )
		{
			if (_pst_Element->pus_ListOfUsedIndex)
				stream = GXI_CopyIndex_STRIP_8(stream , pStrip->pMinVertexDataList , TRUE , pStrip->ulVertexNumber , dl->IsIndex8);
			else
				stream = GXI_CopyIndex_STRIP_8(stream , pStrip->pMinVertexDataList , FALSE , pStrip->ulVertexNumber , dl->IsIndex8);//*/
	   	    
	   	    pStrip++;
	   	    
	   	    if(pStrip < pStripEnd)
	   	    {
		   	    // prepare the next strip
		   	    
		   	    mini_stream = (u8*)stream;
		   	    *mini_stream = GX_TRIANGLESTRIP | GX_VTXFMT0;
		   	    *((u16*)(mini_stream + 1)) = pStrip->ulVertexNumber;
		   	    
		   	    stream = (u16*)(mini_stream+3);
		   	}
		}
	}
	
	GXI_DL_Unlock(dl);
}

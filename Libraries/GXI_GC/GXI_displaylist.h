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

#ifndef __GXI_DISPLAYLIST_H__
#define __GXI_DISPLAYLIST_H__

typedef struct tdstDisplayList_
{
	u8* 	data;
	u16		count;
	u16 	nbStripOverOne;
	u8  	nAttributes;
	u32		IsIndex8; 
} tdstDisplayList;

void 				GXI_DL_Initialize(tdstDisplayList *dl);
void 				GXI_DL_SetCount(tdstDisplayList *dl, u16 count);
void 				GXI_DL_SetPrimitiveAndFormat(tdstDisplayList *dl, u8 command, GXVtxFmt format);
GXVtxFmt 			GXI_DL_GetFormat(tdstDisplayList *dl);
GXPrimitive 		GXI_DL_GetPrimitive(tdstDisplayList *dl);
u16  				GXI_DL_GetCount(tdstDisplayList *dl);
u32  				GXI_DL_GetByteSize(tdstDisplayList *dl);
u32  				GXI_DL_GetPaddedByteSize(tdstDisplayList *dl);
void 				GXI_DL_AddPadding(tdstDisplayList *dl);
void 				GXI_DL_Allocate(tdstDisplayList *dl);
void 				GXI_DL_Free(tdstDisplayList *dl);
void* 				GXI_DL_GetPointer(tdstDisplayList *dl);
void 				GXI_DL_SetNbAttributes(tdstDisplayList *dl, u8 nb);
u16* 				GXI_DL_Lock(tdstDisplayList *dl);
void 				GXI_DL_Unlock(tdstDisplayList *dl);
BOOL	 			GXI_DL_NeedColor(tdstDisplayList *dl);
BOOL	 			GXI_DL_NeedNormal(tdstDisplayList *dl);
tdstDisplayList* 	GXI_DL_InitializeAndAllocate(u32 count, u8 nAttributes);
void 				GXI_DL_fill_with_geometric_object(tdstDisplayList *dl, struct GEO_tdst_ElementIndexedTriangles_ *_pst_Element);
void 				GXI_CreateDisplayLists(struct GEO_tdst_Object_ *_pst_Object);

#endif

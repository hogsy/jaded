//  =============================================================================
//  (C) Copyright 2001 Ubi Soft
//  =============================================================================
//
//  Description   : Manage the objects not to be drawn immediatly
//
//  Author        : Dany Joannette
//  Date          : 07 December 2001
//
//  =============================================================================

#ifndef __GXI_OBJECTLIST_H__
#define __GXI_OBJECTLIST_H__

typedef struct _GXI_ObjectList
{
	u32  	   		*vtx_array;
	u32  	   		*nrm_array;
	u32  	   		*col_array;
	u32  	   		*uv_array;
	
	u8 		    	vtx_step;
	u8 		    	nrm_step;
	u8 		    	col_step;
	u8 		    	uv_step;
	
	void 	   		*dl;
	u32   			dl_size;
	void 	   		*dl_pos;
	u32   			dl_pos_size;
		
	f32   			z;
	u8	  			bTransparent;
	u32   			idx;
	Mtx 			mv;
	u32				tex;
	u32				BM;
	u32 			draw_mask;
	BOOL			bUseShadow;
	BOOL			bShadow;
	
	tdstFogParams 	fog_params;
	
	u8				texmatrix_type;
	BOOL			texmatrix_useUvMatrix;
	Mtx				texmatrix_1;
	Mtx				texmatrix_2;

} GXI_ObjectList;

#define TRANSPARENT_OBJECT_LIST_SIZE 128

extern GXI_ObjectList transparent_object_list[TRANSPARENT_OBJECT_LIST_SIZE];

typedef enum
{
	eNonTransparentOnly,
	eTransparentOnly,
	eAll_Z_Only,
	eAll,	
} tdeSendType;

void GXI_ObjectList_Add();
void GXI_ObjectList_Sort();
void GXI_ObjectList_Send(tdeSendType eSendType);
void GXI_ObjectList_Clear();
void GXI_ObjectList_ResetCurrent();

#endif

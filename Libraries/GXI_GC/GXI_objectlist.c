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

#include "GEOmetric/GEOobject.h"
#include "GXI_init.h"
#include "GXI_render.h"
#include "GXI_displaylist.h"
#include "GXI_vertexspace.h"
#include "GXI_tex.h"
#include "GXI_font.h"
#include "GXI_shadows.h"
#include "GXI_renderstate.h"
#include "GXI_bench.h"
#include "BASe/MEMory/MEM.h"
#include "MATerial/MATstruct.h"
#include "GXI_dbg.h"

#include "GEOmetric/GEO_STRIP.h"
#include "GEOmetric/GEO_SKIN.h"

#include "GXI_objectlist.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SDW.h"
/*
GXI_ObjectList transparent_object_list[TRANSPARENT_OBJECT_LIST_SIZE];
u32  g_object_list_index;
BOOL g_bObjectListIsSorted;
*/
void GXI_ObjectList_Add()
{
/*	GXI_Global_ACCESS(current_object_list_item).idx += g_object_list_index;

	L_memcpy(&transparent_object_list[g_object_list_index], &GXI_Global_ACCESS(current_object_list_item), sizeof(GXI_ObjectList));
	MTXCopy(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), transparent_object_list[g_object_list_index].mv);
	g_object_list_index++;
	g_bObjectListIsSorted = FALSE;*/
}

int ObjectListQSort(const void *T1,const void *T2)
{
/*	// transparent vs non transparent
	if ((((GXI_ObjectList *)T1)->bTransparent) && !(((GXI_ObjectList *)T2)->bTransparent) )
		return -1;	
	if (!(((GXI_ObjectList *)T1)->bTransparent) && (((GXI_ObjectList *)T2)->bTransparent) )
		return 1;
	
	// sort based on priority
	if ((((GXI_ObjectList *)T1)->idx&0xffff0000) > (((GXI_ObjectList *)T2)->idx&0xffff0000) )
		return 1;
	if ((((GXI_ObjectList *)T1)->idx&0xffff0000) < (((GXI_ObjectList *)T2)->idx&0xffff0000) )
		return -1;
		
	// if same depth, use order of display
	if (((GXI_ObjectList *)T1)->z == ((GXI_ObjectList *)T2)->z) 
		return ((((GXI_ObjectList *)T1)->idx&0x0000ffff) > (((GXI_ObjectList *)T2)->idx&0x0000ffff) ? 1 : -1);
				
	// transparent sorting (back to front)
	if ((((GXI_ObjectList *)T1)->bTransparent) && (((GXI_ObjectList *)T2)->bTransparent) )
	{
		// sort back to front
		if (((GXI_ObjectList *)T1)->z >  ((GXI_ObjectList *)T2)->z) 
			return 1;
		else 
			return -1;
	}

	// non transparent sorting (front to back)
	if (!(((GXI_ObjectList *)T1)->bTransparent) && !(((GXI_ObjectList *)T2)->bTransparent) )
	{
		// sort front to back
		if (((GXI_ObjectList *)T1)->z >  ((GXI_ObjectList *)T2)->z) 
			return 1;
		else 
			return -1;
	}
	*/
	return -1;
}

void GXI_ObjectList_Sort()
{
/*	if(!g_bObjectListIsSorted && g_object_list_index>0)
		qsort ( (void *) transparent_object_list , g_object_list_index , sizeof ( GXI_ObjectList ) , ObjectListQSort );
	
	g_bObjectListIsSorted = TRUE;*/
}

extern void GXI_SetTextureBlending(u32 _l_Texture, u32 BM , u32 AF);
extern void GXI_set_draw_vertex_properties(GXAttrType _bUsePos, GXAttrType _bUseNrm, GXAttrType _bUseClr, GXAttrType _bUseTex);
extern void GXI_prepare_to_draw_material();
extern void GXI_set_color_generation_parameters(BOOL _bUseClr, BOOL _bUseTex);
extern void GXI_set_fog(BOOL _enable);
extern void GXI_SetTextureMatrix(u8 _u8Type, GEO_tdst_Object *pst_Obj, GEO_tdst_ElementIndexedTriangles *pst_Element, u32 CurrentAddFlag, MAT_tdst_Decompressed_UVMatrix *_uvMatrix);

void GXI_ObjectList_Send(tdeSendType _eSendType)
{
/*	s32 i;
	Mtx m;
	
#ifndef USE_TRIPPLE_BUFFERING
	GXDrawDone();
#endif	

	
	
	GXI_Global_ACCESS(bSendingObjectList) = TRUE;

	// sort objects only once (the first time with non transparent objects)
	GXI_ObjectList_Sort();
	
	
	if(_eSendType==eAll_Z_Only) 
	{
		GXSetColorUpdate(GX_FALSE);
		GXI_set_draw_vertex_properties(GX_INDEX16, GX_NONE, GX_NONE, GX_NONE);
		GXI_set_fog(FALSE);
		GX_GXSetCullMode(GX_CULL_NONE);
	}
	else
	{
		GXI_set_draw_vertex_properties(GX_INDEX16, GX_INDEX16, GX_INDEX16, GX_INDEX16);
	}
	
	for(i=0; i<g_object_list_index; i++)
	{
		if((_eSendType==eNonTransparentOnly) && transparent_object_list[i].bTransparent) 
			continue;
		if((_eSendType==eTransparentOnly) && !transparent_object_list[i].bTransparent) 
			continue;
			
		if(transparent_object_list[i].dl )
		{
			
			
			GXI_Global_ACCESS(pcurrent_object_list_item) = &transparent_object_list[i];

			GXLoadPosMtxImm(transparent_object_list[i].mv, GX_PNMTX0);
			
/*#ifdef USE_HARDWARE_LIGHTS			
			MTXInvXpose(transparent_object_list[i].mv, m); 
    		GXLoadNrmMtxImm(m, GX_PNMTX0);
#endif*/

	/*		if(_eSendType==eAll_Z_Only) 
			{
				// send object pos to update Z buffer only
				GX_GXSetZMode(GX_ENABLE, (MAT_GET_FLAG(GXI_Global_ACCESS(pcurrent_object_list_item)->BM) & MAT_Cul_Flag_ZEqual) ? GX_EQUAL : GX_LEQUAL, (MAT_GET_FLAG(GXI_Global_ACCESS(pcurrent_object_list_item)->BM) & MAT_Cul_Flag_NoZWrite) ? GX_DISABLE : GX_ENABLE);
				GX_GXSetArray(GX_VA_POS,  GXI_Global_ACCESS(pcurrent_object_list_item)->vtx_array, GXI_Global_ACCESS(pcurrent_object_list_item)->vtx_step);
					
				
			
				
							
				GXCallDisplayList(transparent_object_list[i].dl_pos, transparent_object_list[i].dl_pos_size);
			}		
			else
			{		
				GXI_SetTextureBlending(transparent_object_list[i].tex, transparent_object_list[i].BM);
				
				GXI_SetTextureMatrix(transparent_object_list[i].texmatrix_type, NULL, NULL, 0, NULL);
								
				GXI_prepare_to_draw_material();
				GXI_set_color_generation_parameters(TRUE, (transparent_object_list[i].tex!=-1));			
			
				if((transparent_object_list[i].draw_mask & GDI_Cul_DM_Fogged)!=0)
				{
					L_memcpy(&GXI_Global_ACCESS(FogParams), &transparent_object_list[i].fog_params, sizeof(tdstFogParams));
					GXI_set_fog(TRUE);
				}
				else GXI_set_fog(FALSE);
					
				GX_GXSetArray(GX_VA_POS,  transparent_object_list[i].vtx_array, transparent_object_list[i].vtx_step);
				GX_GXSetArray(GX_VA_NRM,  transparent_object_list[i].nrm_array, transparent_object_list[i].nrm_step);
				GX_GXSetArray(GX_VA_CLR0, transparent_object_list[i].col_array, transparent_object_list[i].col_step);
				GX_GXSetArray(GX_VA_TEX0, transparent_object_list[i].uv_array,  transparent_object_list[i].uv_step );
		
				if (transparent_object_list[i].draw_mask & GDI_Cul_DM_TestBackFace)
				{
					if (transparent_object_list[i].draw_mask & GDI_Cul_DM_NotInvertBF)
						GX_GXSetCullMode(GX_CULL_FRONT);
					else
						GX_GXSetCullMode(GX_CULL_BACK);
				} 
				else
				{
					GX_GXSetCullMode(GX_CULL_NONE);
				}
				
				
			
				
				GXCallDisplayList(transparent_object_list[i].dl, transparent_object_list[i].dl_size);
			}			
			
#ifndef USE_TRIPPLE_BUFFERING
			GXDrawDone();
#endif	

		}
	}	

	if(_eSendType==eAll_Z_Only) 
		GXSetColorUpdate(GX_TRUE);
	
	GXI_Global_ACCESS(bSendingObjectList) = FALSE;*/
}

void GXI_ObjectList_Clear()
{
	/*u32 i;
	
/*	for(i=0; i<g_object_list_index; i++)
	{
		if(transparent_object_list[i].bShadow)
		{
			if(transparent_object_list[i].dl) MEM_FreeAlign(transparent_object_list[i].dl);
			transparent_object_list[i].dl = NULL;
			if(transparent_object_list[i].dl_pos) MEM_FreeAlign(transparent_object_list[i].dl_pos);
			transparent_object_list[i].dl_pos = NULL;
		}
	}
*/	/*
	g_object_list_index = 0;
	
	GXI_Global_ACCESS(bSendingObjectList) = FALSE;
	GXI_Global_ACCESS(bPutObjectInObjectList) = FALSE;	
	GXI_Global_ACCESS(bFirstReadPixelThisFrame) = TRUE;*/
}

void GXI_ObjectList_ResetCurrent()
{
	//L_memset(&GXI_Global_ACCESS(current_object_list_item), NULL, sizeof(GXI_ObjectList));
}



#ifdef USE_FULL_SCENE_SHADOWS

// shadows **************************************************************************************************

static GXTexObj RampTex16;
static GXTexObj *pRampTex16=NULL;
static u8 		*pShadowTextureData;
static GXTexObj ShadowTexture;

/*---------------------------------------------------------------------------*
    Name:           CreateRampTex16
    
    Description:    Create a ramp texture (16bit version) which is used
                    to lookup depth value from a light.
                    This function is called only once at the beginning.
                    Actually you can prepare this ramp texture as static
                    data.

                    Created texture data (IA8 format) is like this:
                    
                    | 0000 0100 0200 .... FF00 |
                    | 0001 0101 0201 .... FF01 |
                    |   :    :    :         :  |
                    | 00FF 01FF 02FF .... FFFF |
                   
    Arguments:      to : a texture object where the texture data should be set
    
    Returns:        none
 *---------------------------------------------------------------------------*/
void CreateRampTex16( GXTexObj* to )
{
    u16*  data;
    u32   size, i, offset;
    
    size = GXGetTexBufferSize(256, 256, GX_TF_IA8, GX_FALSE, 0);
    data = (u16*)MEM_p_AllocAlign(size, 32);
    
    // Write ramp data pattern
    for ( i = 0 ; i < 0x10000 ; ++i )
    {
        // IA8 format tile offset
        offset = ((i & 0xFC00) >> 6)
               + ((i & 0x0300) >> 8)
               + ((i & 0x00FC) << 8)
               + ((i & 0x0003) << 2);
        *(data+offset) = (u16)i;
    }
   
    // Initialize texture object properties
    GXInitTexObj(to, data, 256, 256, GX_TF_IA8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(to, GX_NEAR, GX_NEAR, 0, 0, 0, 0, 0, GX_ANISO_1);

    // Make sure data is written into the main memory.
    DCFlushRange(data, size);
}

/*---------------------------------------------------------------------------*
    Name:           SetShaderModeFor2ndPass16
    
    Description:    Set up shading mode (color channel, TEV, etc.)
                    for 2nd pass which draws actual scene from the
                    viewer with full-scene shadow.

                    [ 16bit precision comparison version ]
                   
    Arguments:      none
    
    Returns:        none
 *---------------------------------------------------------------------------*/
void SetShaderModeFor2ndPass16( void )
{
    GXColor col_one = { 1, 1, 1, 1 };
    GXColor COL_AMBIENT = {0x40, 0x40, 0x40, 0xff};
        
    GX_GXSetNumChans(1);
    GX_GXSetChanCtrl(
        GX_COLOR0A0,
        GX_ENABLE,     // enable channel
        GX_SRC_REG,    // amb source
        GX_SRC_REG,    // mat source
        GX_LIGHT0,     // light mask
        GX_DF_CLAMP,   // diffuse function
        GX_AF_NONE );
    GX_GXSetChanAmbColor(GX_COLOR0A0, COL_AMBIENT);

    //-------------------------------------------
    //  TEV Codes for GX revision >= 2 (HW2, ...)
    //-------------------------------------------
    GX_GXSetNumTevStages(3);

    // Make a swap table which performs A->G, I->R conversion
    GXSetTevSwapModeTable(GX_TEV_SWAP1,
                          GX_CH_RED,  GX_CH_ALPHA,
                          GX_CH_BLUE, GX_CH_ALPHA);
    // Make a swap table which performs I->G, A->R conversion
    GXSetTevSwapModeTable(GX_TEV_SWAP2,
                          GX_CH_ALPHA, GX_CH_GREEN,
                          GX_CH_BLUE,  GX_CH_ALPHA);

    // TEV Stage 0  ( Loads a depth value from ramp texture )
    // REGPREV(R) = TEX(I) / REGPREV(G) = TEX(A)
    GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0,
                  GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO,
                    GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO,
                    GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GX_GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP1);
    
    // TEV Stage 1  ( Compare G+R 16bit value )
    // REGPREV(C) = REGPREV(GR) > shadow map texture(GR) ? 255 : 0
    GX_GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1,
                  GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC,
                    GX_CC_ONE, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_COMP_GR16_GT, GX_TB_ZERO,
                    GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
    GX_GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP2);

    // TEV Stage 2  ( Select shadow/lit color )
    // output = ( REGPREV == 0 ? rasterized color : shadow color )
    // Register 0 is supporsed to hold shadow color
    GX_GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL,
                  GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_RASC, GX_CC_C0,
                    GX_CC_CPREV, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO,
                    GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GX_GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GX_GXSetTevOpDirty();

    // Tex coords
    GX_GXSetNumTexGens(2);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_POS, GX_TEXMTX0);
    GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_POS, GX_TEXMTX1);
}


void GXI_ObjectList_SendFullSceneShadow()
{
	u32 i;
	
	if(!pRampTex16) return;
	
	// Set up shadow map texture
    GXInitTexObj(
        &ShadowTexture,
        pShadowTextureData,
        400,
        400,
        GX_TF_IA8,
        GX_CLAMP,
        GX_CLAMP,
        GX_FALSE );
        
    GXInitTexObjLOD(&ShadowTexture, GX_NEAR, GX_NEAR, 0, 0, 0, 0, 0, GX_ANISO_1);

	SetShaderModeFor2ndPass16();
    //GetTexProjMtx(&tmo, &sc->light.cam, 1);
    GXLoadTexObj(&RampTex16, GX_TEXMAP0);
    GXLoadTexObj(&ShadowTexture, GX_TEXMAP1);
    
    for(i=0; i<g_object_list_index; i++)
	{
		if( transparent_object_list[i].bUseShadow && transparent_object_list[i].dl_pos )
		{
			GXI_Global_ACCESS(pcurrent_object_list_item) = &transparent_object_list[i];
			
			GXLoadPosMtxImm(transparent_object_list[i].mv, GX_PNMTX0);
			GXI_set_draw_vertex_properties(GX_INDEX16, GX_NONE, GX_NONE, GX_NONE);
			GX_GXSetArray(GX_VA_POS,  transparent_object_list[i].vtx_array, transparent_object_list[i].vtx_step);			
			GXCallDisplayList(transparent_object_list[i].dl_pos, transparent_object_list[i].dl_pos_size);
		}
	}	
	
	GDI_gpst_CurDD->LastBlendingMode = 0;
}

void GXI_ObjectList_ComputeFullSceneShadowTextures()
{
	s32 i,j;
	
	f32 vp[GX_VIEWPORT_SZ];
	u32 oldScissorXOrig, oldScissorYOrig, oldScissorWidth, oldScissorHeight;

	if(!pRampTex16)
	{
		u32 size;
		
		CreateRampTex16(&RampTex16);
		pRampTex16 = &RampTex16;
		
	    // Memory area for dynamic shadow map
	    size = GXGetTexBufferSize(
	               400,
	               400,
	               GX_TF_RGBA8,
	               GX_FALSE,
	               0 );
	               
	    pShadowTextureData = MEM_p_AllocAlign(size, 32);
	}
		
	GXI_Global_ACCESS(bSendingObjectList) = TRUE;

	GX_GXSetColorUpdate(GX_FALSE);

	GX_GXSetCullMode(GX_CULL_NONE);
	
	GXI_set_fog(FALSE);
	
	GXGetViewportv(vp);
	GXGetScissor(&oldScissorXOrig, &oldScissorYOrig, &oldScissorWidth, &oldScissorHeight);		
	
	GXSetViewport(0, 0, 400, 400, 0.0F, 1.0F);
    GXSetScissor(0, 0, (u32)400, (u32)400);

	// clear Z
	
	{
		f32 vp[GX_VIEWPORT_SZ];
		f32 pm[GX_PROJECTION_SZ];
		u32 oldScissorXOrig, oldScissorYOrig, oldScissorWidth, oldScissorHeight;
		u8  *pos;
		u32 nb;
		static u32 aa=0;
		static u32 jj=0;
		u32 currentShadowNum;
		Mtx44 mOrtho;
		Mtx m;
		static f32 ortho = 1.0f;
		
		// save the context
		GXGetProjectionv(pm);
		
		MTXOrtho(mOrtho,-ortho,ortho,-ortho,ortho,0.0f,GX_MAX_Z24);
		MTXIdentity(m);
		GXSetProjection(mOrtho, GX_ORTHOGRAPHIC);					
		
		GXI_set_draw_vertex_properties(GX_DIRECT, GX_NONE, GX_NONE, GX_NONE);
		
		// clear frame buffer
		{
			GXLoadPosMtxImm(&m, GX_PNMTX0);

			GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);
			GX_GXSetVtxDesc(GX_VA_CLR0, GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
			GX_GXSetVtxDesc(GX_VA_CLR1, GX_NONE);
#endif
			GX_GXSetVtxDesc(GX_VA_TEX0, GX_NONE);
			GX_GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);

			// clear screen zbuffer
			{				
				GX_GXSetZMode(GX_ENABLE, GX_ALWAYS, GX_ENABLE);
					
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);
				GXPosition3f32(-ortho, -ortho, GX_MAX_Z24);
				GXPosition3f32(ortho, -ortho, GX_MAX_Z24);
				GXPosition3f32(ortho, ortho, GX_MAX_Z24);
				GXPosition3f32(-ortho, ortho, GX_MAX_Z24);
				GXEnd();
			}
		}			
	}								
				
	GX_GXSetCullMode(GX_CULL_FRONT);			
	GX_GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
			
	for(i=0; i<g_object_list_index; i++)
	{
		if( transparent_object_list[i].bUseShadow && transparent_object_list[i].dl_pos )
		{
			GXI_Global_ACCESS(pcurrent_object_list_item) = &transparent_object_list[i];
			
			GXLoadPosMtxImm(transparent_object_list[i].mv, GX_PNMTX0);
			GXI_set_draw_vertex_properties(GX_INDEX16, GX_NONE, GX_NONE, GX_NONE);
			GX_GXSetArray(GX_VA_POS,  transparent_object_list[i].vtx_array, transparent_object_list[i].vtx_step);			
			GXCallDisplayList(transparent_object_list[i].dl_pos, transparent_object_list[i].dl_pos_size);
		}
	}	
	
	// Copy shadow image into texture
    GXSetTexCopySrc(0, 0, 400, 400);
    GXSetTexCopyDst(400, 400, GX_TF_Z16, GX_FALSE);
    GXCopyTex(pShadowTextureData, GX_TRUE);

    // Wait for finishing the copy task in the graphics pipeline
    GXPixModeSync();
    
	GX_GXSetColorUpdate(GX_TRUE);
	GX_GXSetCullMode(GX_CULL_NONE);
	
	GXSetScissor(oldScissorXOrig, oldScissorYOrig, oldScissorWidth, oldScissorHeight);		
	GXSetViewportv(vp);	
	
	GXI_Global_ACCESS(bSendingObjectList) = FALSE;
	GDI_gpst_CurDD->LastBlendingMode = 0;
}

#endif // full scene shadows

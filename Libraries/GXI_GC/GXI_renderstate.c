/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "GXI_init.h"
#include "GXI_renderstate.h"
#include "BASe/BAStypes.h"
#include "TEXture/TEXstruct.h"
#include "GXI_shadows.h"

#if defined __cplusplus
    extern "C" {
#endif

BOOL g_bTevDirty = FALSE;
BOOL g_bColorDiv2X = FALSE;
BOOL g_bAlphaDiv2X = FALSE;

void GXI_HW_States_Init()
{
	// init everything to 0xff
	L_memset(&g_GXI_HW_States, 0xff, sizeof(GXI_HW_States));

	// Z buffer state
	GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
	g_GXI_HW_States.bCompareEnable = GX_ENABLE;
	g_GXI_HW_States.funcCompare = GX_LEQUAL;
	g_GXI_HW_States.bUpdateEnable = GX_ENABLE;

	// init texture pointers to NULL
	L_memset(&g_GXI_HW_States.pTexObj, 0x00, sizeof(GXTexObj*)*NB_TEXMAPID);

	// init vtx descriptors to GX_NONE
	L_memset(&g_GXI_HW_States.arAttrType, 0x00, sizeof(GXAttrType)*NB_ATTR);
	GXSetVtxDesc(GX_VA_POS,   GX_NONE);
	GXSetVtxDesc(GX_VA_NRM,   GX_NONE);
	GXSetVtxDesc(GX_VA_CLR0,  GX_NONE);
	GXSetVtxDesc(GX_VA_TEX0,  GX_NONE);

	// GXSetAlphaCompare
	g_GXI_HW_States.acCompare1 = GX_ALWAYS;
	g_GXI_HW_States.acVal1 = 0;
	g_GXI_HW_States.acOp = GX_AOP_OR;
	g_GXI_HW_States.acCompare2 = GX_ALWAYS;
	g_GXI_HW_States.acVal2 = 0;	
	GXSetAlphaCompare(g_GXI_HW_States.acCompare1, g_GXI_HW_States.acVal1, g_GXI_HW_States.acOp, g_GXI_HW_States.acCompare2, g_GXI_HW_States.acVal2);
	
	// GXSetZCompLoc
	g_GXI_HW_States.zcompBeforeTex = GX_FALSE;
	GXSetZCompLoc(g_GXI_HW_States.zcompBeforeTex);

	// Blend state
	GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_NOOP);
	g_GXI_HW_States.bmType = GX_BM_NONE;
	g_GXI_HW_States.bmSrcFactor = GX_BL_ZERO;
	g_GXI_HW_States.bmDstFactor = GX_BL_ZERO;
	g_GXI_HW_States.bmOp = GX_LO_NOOP;

	// GXSetNumChans
	GXSetNumChans(1);
	g_GXI_HW_States.u8NumChans = 1;
	
	// GXSetNumTexGens
	GXSetNumTexGens(1);
	g_GXI_HW_States.u8NumTexGens = 1;

	// GXSetNumTevStages
	GXSetNumTevStages(1);
	g_GXI_HW_States.u8NumTevStages = 1;

	// GXSetTevOrder
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	g_GXI_HW_States.toStage = GX_TEVSTAGE0;
	g_GXI_HW_States.toCoord = GX_TEXCOORD_NULL;
	g_GXI_HW_States.toMap = GX_TEXMAP_NULL;
	g_GXI_HW_States.toColor = GX_COLOR0A0;

    // GXSetChanCtrl
    GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    g_GXI_HW_States.chanID=GX_COLOR0A0;
    g_GXI_HW_States.chanEnable=GX_DISABLE;
    g_GXI_HW_States.chanColAmbSrc=GX_SRC_VTX;
    g_GXI_HW_States.chanColMatSrc=GX_SRC_VTX;
    g_GXI_HW_States.chanLightMask=GX_LIGHT_NULL;
    g_GXI_HW_States.chanDiffFn=GX_DF_CLAMP;
    g_GXI_HW_States.chanAttnFn=GX_AF_NONE;

	// GXSetTevOp
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    g_GXI_HW_States.opStage = GX_TEVSTAGE0;
    g_GXI_HW_States.opMode = GX_MODULATE;
    
    // GXSetChanMatColor
    GXSetChanMatColor( GX_COLOR0A0, GX_WHITE );
    g_GXI_HW_States.scChanMat = GX_COLOR0A0;
	g_GXI_HW_States.scColorMat = GX_WHITE;
		
    // GXSetChanAmbColor
    GXSetChanAmbColor( GX_COLOR0A0, GX_BLACK );
    g_GXI_HW_States.scChanAmb = GX_COLOR0A0;
	g_GXI_HW_States.scColorAmb = GX_BLACK;
		
    // GXSetCullMode
    GXSetCullMode(GX_CULL_FRONT);
	g_GXI_HW_States.cullMode = GX_CULL_FRONT;
    
    // FB Update
    g_GXI_HW_States.ColorUpdate = GX_TRUE;
    GXSetColorUpdate(GX_TRUE);    
    g_GXI_HW_States.AlphaUpdate = GX_TRUE;
    GXSetAlphaUpdate(GX_TRUE);
    
    // GXSetTevSwap
	{
		int i;
		for(i=0; i<GX_MAX_TEVSTAGE; i++)
		{
			g_GXI_HW_States.tsRascSel[i] = GX_TEV_SWAP0;
			g_GXI_HW_States.tsTexSel[i] = GX_TEV_SWAP0;
			GXSetTevSwapMode(i, g_GXI_HW_States.tsRascSel[i], g_GXI_HW_States.tsTexSel[i]);
		}
	}
    
	// GXSetArray
	L_memset(&g_GXI_HW_States.arBasePtr, 0x00, sizeof(void*)*NB_ATTR);
	L_memset(&g_GXI_HW_States.arStride, 0x00, sizeof(u8)*NB_ATTR);
	
	
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,  GX_POS_XYZ,  GX_F32,   0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM,  GX_NRM_XYZ,  GX_F32,   0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR1, GX_CLR_RGBA, GX_RGBA8, 0);
#endif
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST,   GX_F32,   0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
*/
#ifndef USE_FULL_SCENE_SHADOWS
extern GXTexObj g_ShadowTextures[]; // DJ_TEMP
#endif

LONG GXI_RS_UseTexture(LONG _l_Texture)
{
    //if(_l_Texture != _pst_SD->st_RS.l_LastTexture)
    {
        if(_l_Texture == -1)
        {
        }
#ifndef USE_FULL_SCENE_SHADOWS        
        else if(_l_Texture & 0x80000000) // use shadow texture
        {
        	extern u32 	 g_currentShadowNum2Set;
        	if ((_l_Texture & 0x0FFFFFFF) == g_currentShadowNum2Set) GXI_CompleteShadowRenderingB2D();
        	GX_GXLoadTexObj(&g_ShadowTextures[(_l_Texture & 0x0FFFFFFF)], GX_TEXMAP0);        	
        }
#endif        
        else
        {
            if(GXI_Global_ACCESS(p_MyTextureDescriptorsAligned))
            {
                if (GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[ _l_Texture ].bitmapData == NULL )
                {
                    TEX_tdst_Data *pst_Tex;

                    pst_Tex = &TEX_gst_GlobalList.dst_Texture[ _l_Texture ];
                    if (pst_Tex->uw_Flags & TEX_uw_RawPal)
                        _l_Texture = pst_Tex->w_Height;
                }

				if(!GXI_Global_ACCESS(bPutObjectInObjectList))               
                {
                	GXI_tdst_TextureDesc *l_pstTexDesc = &GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[_l_Texture];
                	GXI_tdst_PaletteDesc *l_pstTlutDesc = &GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)[l_pstTexDesc->paletteNum];

					if(l_pstTexDesc->paletteNum!=-1)
					{
						if(GXI_Global_ACCESS(p_MyPalletteDescriptorAligned)) 
							GXLoadTlut(&l_pstTlutDesc->obj, l_pstTlutDesc->paletteName);
					}							
						
	                //GX_GXLoadTexObj(&l_pstTexDesc->obj, GX_TEXMAP0);
	            }
                
            }
        }

    }
    return _l_Texture;
}

#if defined __cplusplus
    }
#endif

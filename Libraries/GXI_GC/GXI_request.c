/*$T OGLrequest.c GC! 1.086 06/28/00 17:18:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "GEOmetric/GEOobject.h"
#include "GXI_init.h"
#include "GXI_tex.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "GXI_request.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXstruct.h"


/*$4
 ***********************************************************************************************************************
    Full screen mode
 ***********************************************************************************************************************
 */

//extern HINSTANCE	MAI_gh_MainInstance;

/*LONG				GXI_l_FullScreenMode = 0;
RECT				GXI_gst_WindowPos;
HWND				GXI_gh_FullScreenWnd;
HWND				GXI_gh_WindowedHwndSave;
*/
/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern void GXI_set_draw_vertex_properties(GXAttrType _bUsePos, GXAttrType _bUseNrm, GXAttrType _bUseClr, GXAttrType _bUseTex);
extern void GXI_set_color_generation_parameters(BOOL _bUseClr, BOOL _bUseTex);
extern void GXI_set_fog(BOOL _enable);
extern void GXI_SetTextureMatrix(u8 _u8Type, GEO_tdst_Object *pst_Obj, GEO_tdst_ElementIndexedTriangles *pst_Element, u32 CurrentAddFlag, MAT_tdst_Decompressed_UVMatrix *_uvMatrix);


void GXI_SetFogParams(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_FogParams *_pst_Fog)
{
	GXI_Global_ACCESS(FogParams).FogColor.r = ((u8*)&_pst_Fog->ul_Color)[3];
	GXI_Global_ACCESS(FogParams).FogColor.g = ((u8*)&_pst_Fog->ul_Color)[2];
	GXI_Global_ACCESS(FogParams).FogColor.b = ((u8*)&_pst_Fog->ul_Color)[1];
	GXI_Global_ACCESS(FogParams).FogColor.a = ((u8*)&_pst_Fog->ul_Color)[0];

	GXI_Global_ACCESS(FogParams).FogType    = (_pst_Fog->c_Mode==0) ? GX_FOG_LIN : (_pst_Fog->c_Mode==1) ? GX_FOG_EXP : GX_FOG_EXP2;		
	GXI_Global_ACCESS(FogParams).FogDensity = _pst_Fog->f_Density;
	
	
	
	
	GXI_Global_ACCESS(FogParams).FogStart   = _pst_Fog->f_Start;
	GXI_Global_ACCESS(FogParams).FogEnd     = _pst_Fog->f_End;
	
	if ((_pst_Fog->f_Start == 100.0f) && (_pst_Fog->f_End == 105.0f))
		GXI_Global_ACCESS(FogParams).FogEnd     = 2000.0f;

	GXI_Global_ACCESS(FogParams).FogForceDisable = FALSE;
	
	if(_pst_Fog->c_Flag & SOFT_C_FogActive)
	{
		GXI_Global_ACCESS(FogParams).FogActive  = TRUE;
		GXI_set_fog(TRUE);
	}
	else
	{
		GXI_Global_ACCESS(FogParams).FogActive = FALSE;
		GXI_set_fog(FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_DrawProjectedTriangle(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Vertex *_pst_TV)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_Draw2DTriangle(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Vertex *_pst_TV)
{
}


/*
 =======================================================================================================================
    Aim:    Draw a point
 =======================================================================================================================
 */
void GXI_DrawPoint(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_DrawPointMin(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size
 =======================================================================================================================
 */
void GXI_DrawPoint_Size(GDI_tdst_DisplayData *_pst_DD, void **peewee)
{
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size and color
 =======================================================================================================================
 */
void GXI_DrawPointEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawPointEx *_pst_Data)
{
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size and color
 =======================================================================================================================
 */
void GXI_PointSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a line
 =======================================================================================================================
 */
void GXI_DrawLine(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a line with width and color given
 =======================================================================================================================
 */
void GXI_DrawLineEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawLineEx *_pst_Data)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a line with width and color given
 =======================================================================================================================
 */
void GXI_LineSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a quad
 =======================================================================================================================
 */
void GXI_DrawQuad(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v)
{
}


#if 0
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_BeforeDrawSprite2(GDI_tdst_DisplayData *_pst_DD)
{
/*	GXI_RS_DrawWired(GXI_M_RS(_pst_DD), 0);
	GXI_RS_CullFaceInverted(GXI_M_RS(_pst_DD), 0);
	glBegin(GL_QUADS);*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_AfterDrawSprite2(GDI_tdst_DisplayData *_pst_DD)
{
//	glEnd();
}

/*
 =======================================================================================================================
    Aim:    Draw a Sprite
 =======================================================================================================================
 */
void GXI_DrawSprite2(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
	GXColor col;
	
	GX_GXSetCullMode(GX_CULL_NONE);
	GX_GXSetZMode((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest) ? GX_ENABLE : GX_DISABLE, (GXCompare)-1, -1);
	GXI_set_fog((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged)!=0);
	GXI_set_draw_vertex_properties(GX_DIRECT, GX_NONE, GX_DIRECT, GX_DIRECT);
	GXI_prepare_to_draw_material(GX_SRC_VTX);
	GXI_set_color_generation_parameters(TRUE, TRUE);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);	

	*(ULONG*)&col = *(ULONG*)&v[4].x;
	
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(v[0].x, v[0].y, v[0].z);
#ifdef USE_PERPECTIVE_CORRECT_COLORS	
		GXColor1u32((((col.g)<<24)) | (((col.b)<<16)) | (((col.r)))); 
		GXColor1u32((((col.a)<<24)) | (((col.r)<<16))); 
#else			
		GXColor1u32((col.g)<<24 | (col.b)<<16 | (col.a)<<8 | (col.r));
#endif			
		GXTexCoord2f32(1.0f, 1.0f);
			
	   	GXPosition3f32(v[1].x, v[1].y, v[1].z);
#ifdef USE_PERPECTIVE_CORRECT_COLORS	
		GXColor1u32((((col.g)<<24)) | (((col.b)<<16)) | (((col.r)))); 
		GXColor1u32((((col.a)<<24)) | (((col.r)<<16))); 
#else			
		GXColor1u32((col.g)<<24 | (col.b)<<16 | (col.a)<<8 | (col.r));
#endif		
		GXTexCoord2f32(0.0f, 1.0f);	

	   	GXPosition3f32(v[2].x, v[2].y, v[2].z);
#ifdef USE_PERPECTIVE_CORRECT_COLORS	
		GXColor1u32((((col.g)<<24)) | (((col.b)<<16)) | (((col.r)))); 
		GXColor1u32((((col.a)<<24)) | (((col.r)<<16))); 
#else			
		GXColor1u32((col.g)<<24 | (col.b)<<16 | (col.a)<<8 | (col.r));
#endif			
		GXTexCoord2f32(0.0f, 0.0f);

	   	GXPosition3f32(v[3].x, v[3].y, v[3].z);
#ifdef USE_PERPECTIVE_CORRECT_COLORS	
		GXColor1u32((((col.g)<<24)) | (((col.b)<<16)) | (((col.r)))); 
		GXColor1u32((((col.a)<<24)) | (((col.r)<<16))); 
#else			
		GXColor1u32((col.g)<<24 | (col.b)<<16 | (col.a)<<8 | (col.r));
#endif			
		GXTexCoord2f32(1.0f, 0.0f);
		
	GXEnd();
}

#endif // 0 : old system

#define MAX_SPRITES_PER_CALL 500
Vec g_stTempSprite[4*MAX_SPRITES_PER_CALL];
u32 g_stColSprite[MAX_SPRITES_PER_CALL];
s16 g_iCurNbTempSprite = -1;
#include "GXI_vertexspace.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_BeforeDrawSprite(GDI_tdst_DisplayData *_pst_DD)
{
#ifdef USE_HARDWARE_LIGHTS
	extern BOOL g_bLightOff;
#endif

	if(g_iCurNbTempSprite!=-1)
	{
#ifdef _DEBUG	
		OSReport("BAD BAD before sprite call : two before, no after\n");
#endif		
		return;
	}
	
#ifdef USE_HARDWARE_LIGHTS 
	g_bLightOff = TRUE;
#endif

	GX_GXSetCullMode(GX_CULL_NONE);
	GX_GXSetZMode((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest) ? GX_ENABLE : GX_DISABLE, (GXCompare)-1, -1);
	GXI_set_fog((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged)!=0);
	
	GX_GXSetVtxDesc(GX_VA_POS,   GX_INDEX16);
	GX_GXSetVtxDesc(GX_VA_NRM,   GX_NONE);
	GX_GXSetVtxDesc(GX_VA_CLR0,  GX_INDEX16);
	GX_GXSetVtxDesc(GX_VA_TEX0,  GX_INDEX16);
	GX_GXSetVtxDesc(GX_VA_CLR1,  GX_NONE);
	
	GXI_prepare_to_draw_material(GX_SRC_VTX);
	
	//GXI_set_color_generation_parameters(TRUE, TRUE);
	GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(1);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);	
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);	
	
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);	
}

/*
 =======================================================================================================================
    Aim:    Draw a Sprite
 =======================================================================================================================
 */
void GXI_DrawSprite(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
	GXColor col;	
	
	if(g_iCurNbTempSprite>=(MAX_SPRITES_PER_CALL-1))
	{
#ifdef _DEBUG	
		OSReport("Multiple batch not supported yet\n");
#endif		
		return;
	}
	
	memcpy(&g_stTempSprite[4*(++g_iCurNbTempSprite)], &v[0], 12);
	memcpy(&g_stTempSprite[4*(g_iCurNbTempSprite)+1], &v[1], 12);
	memcpy(&g_stTempSprite[4*(g_iCurNbTempSprite)+2], &v[2], 12);
	memcpy(&g_stTempSprite[4*(g_iCurNbTempSprite)+3], &v[3], 12);
	
	*(ULONG*)&col = *(ULONG*)&v[4].x;
	
	g_stColSprite[g_iCurNbTempSprite] = ((col.g)<<24 | (col.b)<<16 | (col.a)<<8 | (col.r));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_AfterDrawSprite(GDI_tdst_DisplayData *_pst_DD)
{
//	static f32 tf_UV[8] = { 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
//	static f32 tf_UV[5] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };
	static f32 tf_UV[5] = { 0.0f, 1.0f, 1.0f, 0.0f, 0.0f };
	
	int i;	
	u32 *col_array;
	f32 *vec_array;
	
	if(g_iCurNbTempSprite == -1)
	{
		//OSReport("No sprites to draw\n");
		return;		
	}
	
	vec_array = (f32*)GXI_GetVertexSpace((g_iCurNbTempSprite+1)*4*sizeof(Vec));
	memcpy(vec_array, g_stTempSprite, (g_iCurNbTempSprite+1)*4*sizeof(Vec));
	DCFlushRange(vec_array, (g_iCurNbTempSprite+1)*4*sizeof(Vec));
	
	col_array = (u32*)GXI_GetVertexSpace((g_iCurNbTempSprite+1)*sizeof(u32));
	memcpy(col_array, g_stColSprite, (g_iCurNbTempSprite+1)*sizeof(u32));
	DCFlushRange(col_array, (g_iCurNbTempSprite+1)*4*sizeof(Vec));
	
	GX_GXSetArray(GX_VA_POS,  vec_array, sizeof(Vec));
	GX_GXSetArray(GX_VA_CLR0, col_array, sizeof(u32));
	GX_GXSetArray(GX_VA_TEX0, tf_UV, sizeof(f32));
	
	GXBegin(GX_QUADS, GX_VTXFMT0, (g_iCurNbTempSprite+1)*4);
	
		for(i=0; i<((g_iCurNbTempSprite+1)*4); i++)
		{
			GXPosition1x16(i);
			GXColor1x16(i/4);
			GXTexCoord1x16(3-(i%4));
		}
		
	GXEnd();
	
	g_iCurNbTempSprite = -1;
}

/*
 =======================================================================================================================
    Aim:    Draw a Sprite
 =======================================================================================================================
 */
void GXI_DecompressUV(unsigned char *U0U1V0V1 , float *Decomp)
{
	/*Decomp[6] = Decomp[0] = (float)(U0U1V0V1[3]) / 255.0f; //U0
	Decomp[4] = Decomp[2] = (float)(U0U1V0V1[2]) / 255.0f; //U1
	Decomp[3] = Decomp[1] = (float)(U0U1V0V1[1]) / 255.0f; //V0
	Decomp[5] = Decomp[7] = (float)(U0U1V0V1[0]) / 255.0f; //V1*/
}

void GXI_DrawSprite_UV(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
/*	float	tf_UV[8];
	GXI_DecompressUV((unsigned char *)&v[4].y, tf_UV);
	glColor4ubv((GLubyte *) &v[4].x);
	glTexCoord2fv(&tf_UV[0]);	/* U0 V0 */
	/*glVertex3fv((float *) &v[0]);
	glTexCoord2fv(&tf_UV[2]);	/* U1 V0 */
	/*glVertex3fv((float *) &v[1]);
	glTexCoord2fv(&tf_UV[4]);	/* 11 */
	/*glVertex3fv((float *) &v[2]);
	glTexCoord2fv(&tf_UV[6]);	/* 01 */
	//glVertex3fv((float *) &v[3]);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_ReloadTexture(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	u32 u32TextureSize, W, H;
	u8 *bitmap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GXI_tdst_TextureDesc *l_pstTexDesc = &GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[_pst_RTP->w_Texture];

	W = l_pstTexDesc->W;
	H = l_pstTexDesc->H;
	
	u32TextureSize = GXGetTexBufferSize( l_pstTexDesc->W, 
									 	 l_pstTexDesc->H, 
									 	 l_pstTexDesc->formatTEX, 
									 	 l_pstTexDesc->bUseMipMap, 
									 	 0xFF);

	// copy raw data
	bitmap = (UCHAR*)_pst_RTP->pc_Data;
	
	if (_pst_RTP->IsAlreadySwizzled == 24)
		L_memcpy(l_pstTexDesc->bitmapData , _pst_RTP->pc_Data , u32TextureSize);
	// now create the tiles to convert bitmap in gamecube format
	// 32 bits is trickier.  Fill two tiles at once.
	else
	{
		int nbXTiles, nbYTiles;
		u32 tileSize = 32;
		u8* currARTilePtr = (u8*)l_pstTexDesc->bitmapData;
		u8* currGBTilePtr = (u8*)l_pstTexDesc->bitmapData + tileSize;
		GXColor *color;
		GXColor black = { 0,0,0,255 };
		int ix, iy; // image x and y;
		int tileY, tileX;
		int nbTexelsPerTileX, nbTexelsPerTileY;
		u32 *srcImage = (u32*)bitmap;

		nbTexelsPerTileX = 4;
		nbTexelsPerTileY = 4;
		
		nbXTiles = W/nbTexelsPerTileX;
		nbYTiles = H/nbTexelsPerTileY;

		for(tileY = 0; tileY < nbYTiles; tileY++)
		{
			for(tileX = 0; tileX < nbXTiles; tileX++)
			{
				// fill the tiles texel by texel
				int tx, ty; // tile x and y
				for(ty = 0; ty < nbTexelsPerTileY; ty++)
				{
					for(tx = 0; tx < nbTexelsPerTileX; tx++)
					{
						// get the color in the image
						ix = tileX*nbTexelsPerTileX+tx;
						iy = tileY*nbTexelsPerTileY+ty;
						if(ix<W&&iy<H)
						{
							color = (GXColor*)(srcImage + (iy * W + ix));
							
							// set the color in both tiles si RGBA
							*currARTilePtr = color->r;
							*(currARTilePtr+1) = color->g;
							*currGBTilePtr = color->b;
							*(currGBTilePtr+1) = color->a;
						}
						currARTilePtr += 2;
						currGBTilePtr += 2;
					}
				}
				currARTilePtr+=tileSize;
				currGBTilePtr+=tileSize;
			}
		}
	}
	DCFlushRange(l_pstTexDesc->bitmapData, u32TextureSize);
	GXInitTexObjData(&l_pstTexDesc->obj, l_pstTexDesc->bitmapData);
}

void GXI_ReloadTexture2(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	u32 u32TextureSize, W, H;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GXI_tdst_TextureDesc *l_pstTexDesc = &GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[_pst_RTP->w_Texture];

	W = l_pstTexDesc->W;
	H = l_pstTexDesc->H;
	
	u32TextureSize = GXGetTexBufferSize( W, 
										 H, 
									 	 l_pstTexDesc->formatTEX, 
									 	 l_pstTexDesc->bUseMipMap, 
									 	 0xFF);
										 	 
		{
			int nbXTiles, nbYTiles, nbTexelsPerTileX, nbTexelsPerTileY;
			u32 ix, iy, tileX, tileY, tx, ty;
			u8 *currTilePtr = (u8*)l_pstTexDesc->bitmapData;
			u8 *destImage   = (u8*)_pst_RTP->pc_Data;
			
			nbTexelsPerTileX = 8;
			nbTexelsPerTileY = 4;
			
			nbXTiles = W/nbTexelsPerTileX;
			nbYTiles = H/nbTexelsPerTileY;
		
			// Fill the tiles sequentially.
			for(tileY = 0; tileY < nbYTiles; tileY++)
			{
				for(tileX = 0; tileX < nbXTiles; tileX++)
				{
					// fill the tile texel by texel
					for(ty = 0; ty < nbTexelsPerTileY; ty++)
					{
						for(tx = 0; tx < nbTexelsPerTileX; tx++)
						{
							// get the color in the image
							ix = tileX*nbTexelsPerTileX+tx;
							iy = tileY*nbTexelsPerTileY+ty;
							if(ix<W&&iy<H)
							{
								*currTilePtr = *(destImage + iy * W + ix);
							}

							// set the color in tile
							currTilePtr++;
						}
					}
				}
			}		
		}
		
	DCFlushRange(l_pstTexDesc->bitmapData, u32TextureSize);
	GXInitTexObjData(&l_pstTexDesc->obj, l_pstTexDesc->bitmapData);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_ReloadPalette(GDI_tdst_DisplayData *_pst_DD, short _w_Palette)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*	GXI_tdst_SpecificData	    *pst_SD;
	TEX_tdst_Data			    *pst_Tex;
    GXI_tdst_UpdatablePalette   *pst_Data;
    int                         i;
    UCHAR                       uc_ColorMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
	pst_SD = GXI_M_SD(_pst_DD);

    glGetBooleanv( GL_MAP_COLOR, &uc_ColorMap );
    if (!uc_ColorMap) 
       glPixelTransferi(GL_MAP_COLOR,TRUE);

    GXI_Texture_SetPalette( _w_Palette );

    /* loop through updatable palette data */
  /*  pst_Data = pst_SD->dst_UdatePalette;
    for (i = 0; i < pst_SD->l_NumberOfUpdatablePalettes; i++, pst_Data++)
    {
        if (pst_Data->w_Palette == _w_Palette )
        {
            pst_Tex = &TEX_gst_GlobalList.dst_Texture[pst_Data->w_Texture];
	        glEnable(GL_TEXTURE_2D);
	        glBindTexture(GL_TEXTURE_2D, pst_SD->dul_Texture[pst_Data->w_Texture]);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pst_Data->w_Width, pst_Data->w_Height, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, pst_Data->p_Raw);
        }
    }

    if (!uc_ColorMap)
        glPixelTransferi(GL_MAP_COLOR,FALSE);*/
}


extern u32 GFXF_ul_Color;
u32 GFXF_DoIt = 0;
extern f32 GFXF_f_depth;
#define ReductPo2 2
extern u32  *p_AfterFXBuffer_MB;
#define GXI_Z_FBW 640
#define GXI_Z_FBH 500
u16 *ZBUFFERU;
extern float fGXI_FadeTime; 


u32 GET_Z(u32 XPos,u32 YPos)
{
	u32 Z32;
	u16 *TilePtr;
	GFXF_DoIt = 1;
	if (ZBUFFERU)
	{
		XPos >>= 1;
		YPos >>= 1;
		
		Z32 = (YPos >> 2) * (GXI_Z_FBW >> (1 + 2));
		Z32 += (XPos >> 2);
		TilePtr = ZBUFFERU + Z32 * 16;
		TilePtr += XPos & 3;
		TilePtr += (YPos & 3) << 2;
		Z32 = (u32)*TilePtr;
		Z32 |= (Z32 & 0xff) << 16;
		Z32 &= 0xffff00;
		Z32 |= 0xff;
		return Z32 & 0xffffff;
	} else return 0;
}
GXTexObj G_Text;
void GXI_GETZ_OneFrameEnding()
{
   	GXColor ambcolor = GX_WHITE;
    extern void  GXI_AE_SendTextureToFrameBuffer(GXTexObj *_pTex, GXBlendFactor _srcBlend, GXBlendFactor _dstBlend, GXColor *_ambcolor, GXColor *_matcolor, INT DX, INT DY, FLOAT _textureOffsetX, FLOAT _textureOffsetY, FLOAT _rorationFactor , FLOAT XFactor , FLOAT BigScale);
    extern GXRenderModeObj *g_pst_mode;
	static u32 bFirst = 1;

	GXInvalidateTexAll(  ); 

	if (GXI_Global_ACCESS(Status) & GC_Status_AE_MB) return;
	if (fGXI_FadeTime) return;
	if (!GFXF_DoIt) return;
	GFXF_DoIt = 0;
	if (bFirst)
	{
		ZBUFFERU = (u16 *)p_AfterFXBuffer_MB;
		GXInitTexObj(&G_Text, ZBUFFERU , GXI_Z_FBW >> 1 , GXI_Z_FBH >> 1 , GX_TF_Z16 , GX_CLAMP , GX_CLAMP , GX_FALSE);
		bFirst = 0;
	}

    GXSetTexCopySrc( 0, 0, GXI_Z_FBW, GXI_Z_FBH);
	GXSetTexCopyDst(GXI_Z_FBW >> 1, GXI_Z_FBH >> 1, GX_TF_Z16, GX_TRUE );
    GXCopyTex(ZBUFFERU , GX_FALSE);
    GXPixModeSync();

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_ReadPixel(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector  *_pst_Pos )
{
	static u32 z;
	
	if(GXI_Global_ACCESS(bFirstReadPixelThisFrame))
	{
		// send objectlist to update zbuffer to get the real depth
		GXI_ObjectList_Send(eAll_Z_Only);
		GXI_Global_ACCESS(bFirstReadPixelThisFrame) = FALSE;
	}
	
	if(_pst_Pos)
	{
		if(_pst_Pos->y < WIDE_SCREEN_ADJUST || (GXI_Global_ACCESS(Ysize)-_pst_Pos->y) < WIDE_SCREEN_ADJUST)
		{
			GFXF_f_depth = 0.0f;
			return;
		}
			
		if((_pst_Pos->x < 0.0f) || (_pst_Pos->x > FRAME_BUFFER_WIDTH))
		{
			GFXF_f_depth = 0.0f;
			return;
		}
		
		z = GET_Z(lFloatToLong(_pst_Pos->x - 0.5f) , lFloatToLong((GXI_Global_ACCESS(Ysize)-_pst_Pos->y) - 0.5f));
 
// 		GXPeekZ(_pst_Pos->x, GXI_Global_ACCESS(Ysize)-_pst_Pos->y, &z);
		GFXF_f_depth = (f32)z/(f32)GX_MAX_Z24;
		
		GXPeekARGB(_pst_Pos->x, GXI_Global_ACCESS(Ysize)-_pst_Pos->y, &GFXF_ul_Color);

	    if ( GFXF_f_depth == 1.0f )
	        GFXF_f_depth = Cf_Infinit;
	    else
	        GFXF_f_depth = 1.0f / (20.0f * (1.0f - GFXF_f_depth) );
	}
	else
	{
		int x,y;
		for(x=0;x<512;x++)
			for(y=0;y<448;y++)
				GXPeekZ(x, y, &z);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 GXI_GetAfterFXColor(s32 X, s32 Y)
{
	u32 Color;
	GXPeekARGB( X + 64, Y, &Color );
	return Color;
}
void GXI_ReadScreen(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_RWPixels *d )
{
/*
	u32TextureSize = GXGetTexBufferSize( FRAME_BUFFER_WIDTH - SCREEN_DIFF, 
									 	 (gul_FRAME_BUFFER_HEIGHT), 
									 	 GX_TF_RGBA8, 
									 	 FALSE, 
									 	 0xFF);
*/

	u8 *bm = (u8*)d->p_Bitmap;

	GXWaitDrawDone(  )	;
	VIWaitForRetrace();
	VIWaitForRetrace();
	VIWaitForRetrace();
	if (d->c_Write == 1) // write bitmap to screen
	{
		// not done yet... used??
	}
	else if (d->c_Write == 0) // create bitmap from screen
	{
		s32 i,j;
		u32 color;
		
		for(i=d->h-1; i>=0; i--)
		{
			for(j=0; j<d->w; j++)
			{
//		GXPeekARGB(d->x+j, d->y+i, &color);
				color = GXI_GetAfterFXColor(j, d->y+i);
				
				// red
				*(bm++) = (u8)((color & 0xff0000) >> 16);
				// Blue
				*(bm++) = (u8)((color & 0xff) >> 0);
				// Green
				*(bm++) = (u8)((color & 0xff00) >> 8);
			}
		}//*/
	}//*/
}

void GXI_PushZBuffer(GDI_tdst_DisplayData *_pst_DD, float Z)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBlendingMode;
	MATH_tdst_Matrix ViewMatrix;
	MATH_tdst_Vector	QUAD[4];
	//u32 ColorRGBA;
	/*~~~~~~~~~~~~~~~~~~~*/

	Z = 2.0f;
	MATH_SetIdentityMatrix(&ViewMatrix);

	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);

	MATH_AddScaleVector(&QUAD[3] , &GDI_gpst_CurDD->st_Camera.st_Matrix.T , MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , Z);

	Z *= 4.0f;

	MATH_AddScaleVector(&QUAD[0] , &QUAD[3] , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , -Z);
	MATH_AddScaleVector(&QUAD[1] , &QUAD[3] , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , -Z);
	MATH_AddScaleVector(&QUAD[2] , &QUAD[3] , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , Z);
	MATH_AddScaleVector(&QUAD[3] , &QUAD[3] , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , Z);

	MATH_AddScaleVector(&QUAD[0] , &QUAD[0] , MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , Z);
	MATH_AddScaleVector(&QUAD[1] , &QUAD[1] , MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , -Z);
	MATH_AddScaleVector(&QUAD[2] , &QUAD[2] , MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , -Z);
	MATH_AddScaleVector(&QUAD[3] , &QUAD[3] , MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , Z);

	ulBlendingMode = MAT_Cul_Flag_HideColor;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	GXI_SetTextureBlending((ULONG) - 1, ulBlendingMode,0);
	
	
	GX_GXSetNumTexGens(0);
	GX_GXSetNumChans(1);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);	
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);


	// Vertex format.
	GX_GXSetVtxDesc(GX_VA_POS,  GX_DIRECT); 
	GX_GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);
	GX_GXSetVtxDesc(GX_VA_TEX0, GX_NONE);

	GX_GXSetZMode(GX_ENABLE,GX_GEQUAL,GX_TRUE);
	
	GX_GXSetCullMode(GX_CULL_NONE);
		
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	
	GXPosition3f32(QUAD[0].x, QUAD[0].y, QUAD[0].z);
	GXColor1u32(0); 
	GXPosition3f32(QUAD[1].x, QUAD[1].y, QUAD[1].z);
	GXColor1u32(0); 
	GXPosition3f32(QUAD[2].x, QUAD[2].y, QUAD[2].z);
	GXColor1u32(0); 
	GXPosition3f32(QUAD[3].x, QUAD[3].y, QUAD[3].z);
	GXColor1u32(0); 
	
	GXEnd();
	
	GX_GXSetZMode(GX_ENABLE,GX_LEQUAL,GX_TRUE);
}


/*
 =======================================================================================================================
    Aim:    Treat miscelleanous request ( see request constant in GDInterface.h file )
 =======================================================================================================================
 */
LONG GXI_l_Request(ULONG _ul_Request, ULONG _ul_Data)
{
	/*$off*/
	switch(_ul_Request)
	{
	case GDI_Cul_Request_SetFogParams:				GXI_SetFogParams(GDI_gpst_CurDD, (SOFT_tdst_FogParams *) _ul_Data); break;
	//case GDI_Cul_Request_DrawSoftEllipse:			GXI_DrawEllipse(GDI_gpst_CurDD, (SOFT_tdst_Ellipse *) _ul_Data); break;
	//case GDI_Cul_Request_DrawSoftArrow:				GXI_DrawArrow(GDI_gpst_CurDD, (SOFT_tdst_Arrow *) _ul_Data); break;
	//case GDI_Cul_Request_DrawSoftSquare:			GXI_DrawSquare(GDI_gpst_CurDD, (SOFT_tdst_Square *) _ul_Data); break;
	//case GDI_Cul_Request_DrawTransformedTriangle:	GXI_DrawProjectedTriangle(GDI_gpst_CurDD, (SOFT_tdst_Vertex *) _ul_Data); break;
//	case GDI_Cul_Request_DepthTest:					GXI_RS_DepthTest( GXI_M_RS( GDI_gpst_CurDD ), _ul_Data ); break;
//	case GDI_Cul_Request_DepthFunc:					GXI_RS_DepthFunc( GXI_M_RS( GDI_gpst_CurDD ), _ul_Data ? GX_GEQUAL : GX_ALWAYS ); break;
	case GDI_Cul_Request_DrawPoint:					GXI_DrawPoint( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_DrawLine:					GXI_DrawLine( GDI_gpst_CurDD, (MATH_tdst_Vector **) _ul_Data ); break;
	case GDI_Cul_Request_DrawTriangle:				break;
	case GDI_Cul_Request_DrawQuad:					GXI_DrawQuad( GDI_gpst_CurDD, (MATH_tdst_Vector **) _ul_Data ); break;
    case GDI_Cul_Request_ReloadTexture:				GXI_ReloadTexture( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
    case GDI_Cul_Request_ReloadTexture2:			GXI_ReloadTexture2( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
	case GDI_Cul_Request_SizeOfPoints:	            GXI_PointSize( GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_SizeOfLine:				GXI_LineSize(GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_DrawLineEx:				GXI_DrawLineEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawLineEx *) _ul_Data); break;
	case GDI_Cul_Request_DrawPointEx:				GXI_DrawPointEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawPointEx *)_ul_Data ); break;
	case GDI_Cul_Request_DrawPointSize:				GXI_DrawPoint_Size(GDI_gpst_CurDD, (void  **)_ul_Data);break;
	case GDI_Cul_Request_BeforeDrawSprite:			GXI_BeforeDrawSprite(GDI_gpst_CurDD);break;
	case GDI_Cul_Request_DrawSprite:				GXI_DrawSprite(GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data );break;
	case GDI_Cul_Request_DrawSpriteUV:				GXI_DrawSprite_UV(GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data );break;
	case GDI_Cul_Request_AfterDrawSprite:			GXI_AfterDrawSprite(GDI_gpst_CurDD);break;
    case GDI_Cul_Request_DrawPointMin:				GXI_DrawPointMin( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
    case GDI_Cul_Request_ReloadPalette:             GXI_ReloadPalette( GDI_gpst_CurDD, (short) _ul_Data ); break;
    case GDI_Cul_Request_Draw2DTriangle:            GXI_Draw2DTriangle(GDI_gpst_CurDD, (SOFT_tdst_Vertex *) _ul_Data); break;
    case GDI_Cul_Request_ReadScreen:                GXI_ReadScreen( GDI_gpst_CurDD, (GDI_tdst_Request_RWPixels *) _ul_Data ); break;
    //case GDI_Cul_Request_EnableFog:                 GXI_RS_Fogged( GXI_M_RS(GDI_gpst_CurDD), _ul_Data ); break;
    //case GDI_Cul_Request_NumberOfTextures:          return ( GXI_M_SD( GDI_gpst_CurDD )->l_NumberOfTextures != (LONG) _ul_Data );
    case GDI_Cul_Request_ReadPixel:                 GXI_ReadPixel( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
    //case GDI_Cul_Request_Enable:                    glEnable( _ul_Data ); break;
    //case GDI_Cul_Request_Disable:                   glDisable( _ul_Data ); break;
    //case GDI_Cul_Request_PolygonOffset:             glPolygonOffset( *(float *) _ul_Data, *(((float *) _ul_Data) + 1) ); break;
//    case GDI_Cul_Request_LoadInterfaceTex:          GXI_Texture_LoadInterfaceTex( GDI_gpst_CurDD ); break;
  //  case GDI_Cul_Request_UnloadInterfaceTex:        GXI_Texture_UnloadInterfaceTex( GDI_gpst_CurDD ); break;
    //case GDI_Cul_Request_GetInterfaceTexBuffer:     return (LONG) GXI_p_Texture_GetInterfaceTexBuffer( GDI_gpst_CurDD, _ul_Data ); break;
    //case GDI_Cul_Request_TextureUnloadCompare:      GXI_Texture_UnLoadCompare(); break;
	case GDI_Cul_Request_PushZBuffer:
		GXI_PushZBuffer(GDI_gpst_CurDD, *(float*)&_ul_Data);
		break;
    
	}
	/*$on*/
	return 0;
}

/*$T OGLrequest.c GC! 1.086 06/28/00 17:18:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#include "ddraw.h"
#endif

#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>
#include <devvif1.h>
#include <libvifpk.h>

#include "Gsp.h"
#include "GSP_Video.h"
#include "GSPinit.h"
#include "GSPtex.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "MATerial/MATstruct.h"
#include "GSPdebugfct.h"
#include "Gsp_Bench.h"



#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif
/*$4
 ***********************************************************************************************************************
    Full screen mode
 ***********************************************************************************************************************
 */
#define Y_Discretised_Shift 3
#define Y_Discretised (1<<Y_Discretised_Shift)
static u_long64	GSP_ZBF_YMask __attribute__((aligned(8))) = 0L;

extern HINSTANCE	MAI_gh_MainInstance;

LONG				GSP_l_FullScreenMode = 0;
RECT				GSP_gst_WindowPos;
HWND				GSP_gh_FullScreenWnd;
HWND				GSP_gh_WindowedHwndSave;
extern u_int *RamZBuffer;
void GSP_SetFogParams(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_FogParams *_pst_Fog);
void GSP_DrawProjectedTriangle(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Vertex *_pst_TV);
void GSP_DrawPoint(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v);
void GSP_DrawPointMin(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v);
void GSP_DrawPoint_Size(GDI_tdst_DisplayData *_pst_DD, void **peewee);
void GSP_DrawPointEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawPointEx *_pst_Data);
void GSP_PointSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size);
void GSP_DrawLine(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v);
void GSP_DrawLineEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawLineEx *_pst_Data);
void GSP_LineSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size);
void GSP_DrawQuad(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v);
void GSP_DrawSprite(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v);
void GSP_BeforeDrawSprite(GDI_tdst_DisplayData *_pst_DD);
void GSP_AfterDrawSprite(GDI_tdst_DisplayData *_pst_DD);
void GSP_ReloadTexture(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP);
void GSP_RS_DepthTest(ULONG Value);
void GSP_RS_DepthFunc(ULONG Value);


void StoreZBUFVIF1( u_long128* base_addr, short start_addr, short pixel_mode, int buff_width,short x, short y, short width, short height )
{
	int texture_qwc;
	sceVif1Packet vif1_pkt;	
	u_long128 settup_base[10];
	u_long prev_imr=0;
	static	u_int enable_path3[4] __attribute__((aligned(16))) = {
		0x06000000,
		0x00000000,
		0x00000000,
		0x00000000,
	};
	height = (height + 1) & ~1;
	// get quad word count for image
	if(( pixel_mode == SCE_GS_PSMCT32 ) || ( pixel_mode == SCE_GS_PSMZ32 ))
			texture_qwc = (width*height*32) >> 7;
 	else if(( pixel_mode == SCE_GS_PSMCT24 ) || ( pixel_mode == SCE_GS_PSMZ24 ))
			texture_qwc = (width*height*24) >> 7;
 	else if(( pixel_mode == SCE_GS_PSMCT16 ) || ( pixel_mode == SCE_GS_PSMZ16 )  || ( pixel_mode == SCE_GS_PSMZ16S ))
			texture_qwc = (width*height*16) >> 7;
 	else if( pixel_mode == SCE_GS_PSMT8 )
			texture_qwc = (width*height*8) >> 7;
	else
			texture_qwc = (width*height*4) >> 7;

	buff_width >>= 6;

	if( buff_width <= 0 )
	    buff_width = 1;

	// set base address of GIF packet
	sceVif1PkInit(&vif1_pkt, &settup_base[0] );
	sceVif1PkReset(&vif1_pkt);

	// will start transfer with VIF code and GS data will follow
	sceVif1PkAddCode(&vif1_pkt, SCE_VIF1_SET_NOP(0)); 
	// disable PATH 3 transfer
	sceVif1PkAddCode(&vif1_pkt, SCE_VIF1_SET_MSKPATH3(0x8000, 0)); 
	// wait for all 3 PATHS to GS to be complete
	sceVif1PkAddCode(&vif1_pkt, SCE_VIF1_SET_FLUSHA(0)); 
	// transfer 6 QW's to GS
	sceVif1PkAddCode(&vif1_pkt, SCE_VIF1_SET_DIRECT(6, 0)); 

	// GIF tag for texture settings		
	sceVif1PkAddGsData(&vif1_pkt, SCE_GIF_SET_TAG(5,1,NULL,NULL,SCE_GIF_PACKED,1) );
	sceVif1PkAddGsData(&vif1_pkt, 0xEL );

	// set transmission between buffers
	sceVif1PkAddGsData(&vif1_pkt, SCE_GS_SET_BITBLTBUF( start_addr, buff_width, pixel_mode , /* SRC */NULL, NULL, NULL ) );// DEST
	sceVif1PkAddGsData(&vif1_pkt, SCE_GS_BITBLTBUF );

	// set transmission area between buffers	( source x,y  dest x,y  and direction )
	sceVif1PkAddGsData(&vif1_pkt, SCE_GS_SET_TRXPOS(x, y, 0, 0, 0) );
	sceVif1PkAddGsData(&vif1_pkt, SCE_GS_TRXPOS );

	// set size of transmission area 
	sceVif1PkAddGsData(&vif1_pkt, SCE_GS_SET_TRXREG(width, height) );
	sceVif1PkAddGsData(&vif1_pkt, SCE_GS_TRXREG );

	// set FINISH event occurrence request
	sceVif1PkAddGsData(&vif1_pkt, (u_long)(0x0) );
	sceVif1PkAddGsData(&vif1_pkt, SCE_GS_FINISH ); 	

	// set transmission direction  ( LOCAL -> HOST Transmission )
	sceVif1PkAddGsData(&vif1_pkt, SCE_GS_SET_TRXDIR(1) );
	sceVif1PkAddGsData(&vif1_pkt, SCE_GS_TRXDIR ); 	

	// get packet size in quad words	
	sceVif1PkTerminate(&vif1_pkt);

	// set the FINISH event
	DPUT_GS_CSR( GS_CSR_FINISH_M );

	// DMA from memory and start DMA transfer
	FlushCache(WRITEBACK_DCACHE);
	DPUT_D1_QWC( 0x7 );
	DPUT_D1_MADR( (u_int)vif1_pkt.pBase & 0x0FFFFFFF );
	DPUT_D1_CHCR( 1 | (1<<8) );

	// check if DMA is complete (STR=0)
	while( DGET_D1_CHCR() & 0x0100 ) {};
	
	// check if FINISH event occured
	while( (DGET_GS_CSR() & GS_CSR_FINISH_M) == 0 ) {};

	// change VIF1-FIFO transfer direction (VIF1 -> MAIN MEM or SPR)
	*VIF1_STAT = 0x00800000;

	// change GS bus direction (LOCAL->HOST)
	DPUT_GS_BUSDIR((u_long)0x00000001);


	// DMA to memory and start DMA transfer
	FlushCache(WRITEBACK_DCACHE);
	DPUT_D1_QWC( texture_qwc );
	DPUT_D1_MADR( (u_int)base_addr & 0x0FFFFFFF );
	DPUT_D1_CHCR( 0 | (1<<8) );

	// check if DMA is complete (STR=0)
	while( DGET_D1_CHCR() & 0x0100 ) {};

	// change VIF1-FIFO transfer direction (MAIN MEM or SPR -> VIF1)
	*VIF1_STAT = 0;
	
	// change GS bus direction (HOST->LOCAL)
	DPUT_GS_BUSDIR((u_long)0);

	// set the FINISH event
	DPUT_GS_CSR( GS_CSR_FINISH_M );

	// MSKPATH3 is now enabled to allow transfer via PATH3
	DPUT_VIF1_FIFO(*(u_long128 *)enable_path3);
 
}// end StoreTextureVIF1

#define RS_SNAPBUFSIZE (1024 * 4)
void GS_GetZBuffer(ULONG *p_Destination)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GSP_SetFogParams(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_FogParams *_pst_Fog)
{

	GspGlobal_ACCESS(Status) &= ~GSP_Status_FogOn;
	if(_pst_Fog->c_Flag & SOFT_C_FogActive)
	{
		if (GspGlobal_ACCESS(fFogCorrectorFar) == 0.0f) GspGlobal_ACCESS(fFogCorrectorFar) = 1.0f;
		if (GspGlobal_ACCESS(fFogCorrector) == 0.0f)	GspGlobal_ACCESS(fFogCorrector) = 1.0f;
		GspGlobal_ACCESS(Status) |= GSP_Status_FogOn;
		GspGlobal_ACCESS(FogColor) = _pst_Fog->ul_Color;
		GspGlobal_ACCESS(FogColor) &= 0x00fefefe;
		GspGlobal_ACCESS(FogColor) += GspGlobal_ACCESS(FogColor);
		if (GspGlobal_ACCESS(FogColor) & 0x0100) GspGlobal_ACCESS(FogColor) |= 0xff;
		if (GspGlobal_ACCESS(FogColor) & 0x010000) GspGlobal_ACCESS(FogColor) |= 0xff00;
		if (GspGlobal_ACCESS(FogColor) & 0x01000000) GspGlobal_ACCESS(FogColor) |= 0xff0000;
		GspGlobal_ACCESS(FogColor) &= 0x00fefefe;
		{
			float Zmax , Zmin;
			Zmin = _pst_Fog->f_Start * GspGlobal_ACCESS(fFogCorrectorFar);
			Zmax = _pst_Fog->f_End * GspGlobal_ACCESS(fFogCorrector);
			Zmin = Zmax - (Zmax - Zmin) * 0.7;
			if (Zmin < 0.00001f) Zmin = 0.00001f;
			if (Zmax - Zmin < 0.00001f) Zmax = Zmin + 0.00001f;
			GspGlobal_ACCESS(FogZNear) = Zmin;
		} 
		GspGlobal_ACCESS(FogZFar) = _pst_Fog->f_End * GspGlobal_ACCESS(fFogCorrectorFar);
		Gsp_InitVU1Matrix_FOG(GspGlobal_ACCESS(FogColor),GspGlobal_ACCESS(FogZNear),GspGlobal_ACCESS(FogZFar));
	}
}
void GSP_Fogged(LONG _l_FogOn)
{
	GspGlobal_ACCESS(Status) &= ~GSP_Status_FogOn;
    if(_l_FogOn)
    {
		GspGlobal_ACCESS(Status) |= GSP_Status_FogOn;
    }
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GSP_DrawProjectedTriangle(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Vertex *_pst_TV)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a point
 =======================================================================================================================
 */
void GSP_DrawPoint(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GSP_DrawPointMin(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size
 =======================================================================================================================
 */
void GSP_DrawPoint_Size(GDI_tdst_DisplayData *_pst_DD, void **peewee)
{
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size and color
 =======================================================================================================================
 */
void GSP_DrawPointEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawPointEx *_pst_Data)
{
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size and color
 =======================================================================================================================
 */
void GSP_PointSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a line
 =======================================================================================================================
 */
void GSP_DrawLine(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a line with width and color given
 =======================================================================================================================
 */
void GSP_DrawLineEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawLineEx *_pst_Data)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a line with width and color given
 =======================================================================================================================
 */
void GSP_LineSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a quad
 =======================================================================================================================
 */
void GSP_DrawQuad(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a Sprite
 =======================================================================================================================
 */
void GSP_DrawSprite(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GSP_BeforeDrawSprite(GDI_tdst_DisplayData *_pst_DD)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GSP_AfterDrawSprite(GDI_tdst_DisplayData *_pst_DD)
{
}

 
extern void GSP_VRamLoadText(u_int VramPtr , u_int DBW , u_int *p_FirstPixel , u_int W , u_int H , u_int BPP , u_int IsPalette);
extern GSP_VideoStream stVideoStr;
extern GSP_VideoStream stVideoStr2;
void GSP_ReloadTexture(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP)
{
	if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[_pst_RTP->w_Texture].InterfaceFlags & GSP_InterfaceTexture)
	{
		L_memcpy(GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[_pst_RTP->w_Texture].p_BitmapPtr , (u_int*)_pst_RTP->pc_Data , ((u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[_pst_RTP->w_Texture].W * (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[_pst_RTP->w_Texture].H * 32)>>3);
	} else//*/
	{
		GSP_LoadTexture_SYNCRO_DMA(_pst_RTP->w_Texture , _pst_RTP->pc_Data);
	}
}

void GSP_ReloadTexture2(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP)
{
	GSP_LoadTexture_SYNCRO_DMA(_pst_RTP->w_Texture , _pst_RTP->pc_Data);
	
}

void GSP_RS_DepthTest(ULONG Value)
{
}

void GSP_RS_DepthFunc(ULONG Value)
{
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
extern float GFXF_f_depth;
extern ULONG GFXF_ul_Color;

void GSP_ReadPixel(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector  *_pst_Pos )
{

    int x, y;
    float RealY;
    ULONG ZINT;
	u_int Remorf;
	Remorf = GspGlobal_ACCESS(Morfling);
	Remorf &= ~31;
    
    GFXF_f_depth = -1000000000000000000.0f;
    x = (int) (_pst_Pos->x - _pst_DD->st_Camera.f_CenterX) * (float)GspGlobal_ACCESS(Xsize) / _pst_DD->st_Camera.f_Width  + _pst_DD->st_Camera.f_CenterX;
    
    RealY = _pst_Pos->y - _pst_DD->st_Camera.f_CenterY;
    y = (int) ((float)GspGlobal_ACCESS(Ysize) * 0.5f + RealY * (float)GspGlobal_ACCESS(Ysize) / _pst_DD->st_Camera.f_Height);
    if (_pst_Pos->z < 0.0f) 
    	return;
    if (x < 0) return;
    if (x > GspGlobal_ACCESS(Xsize)) return;
    y = GspGlobal_ACCESS(Ysize) - y;
    if (y < 0) return;
    if (y > (GspGlobal_ACCESS(Ysize)- 0)) return;
    
    GspGlobal_ACCESS(Status) |= GSP_Status_GetZBufferBack;
    if (!(GspGlobal_ACCESS(Status) & GSP_Status_ZBufferBackValid)) return;
    ZINT = RamZBuffer[((y + Remorf)>>1) * (GspGlobal_ACCESS(Xsize)>>1) + (x>>1)];
    GSP_ZBF_YMask |= 1 << (y >> Y_Discretised_Shift);
    GFXF_ul_Color = GDI_gpst_CurDD->pst_World->ul_BackgroundColor ^ 0xCACACACA;
    if (ZINT)
    {
	    GFXF_f_depth = ((float)ZINT); 	// OoZ
	    GFXF_f_depth = 16.0f * GspGlobal_ACCESS(ZFactor) / (GFXF_f_depth * GIGSCALE0);           // Z Factored 
	    GFXF_f_depth += 0.20f;
	} else
		GFXF_f_depth = 1000000000000000000.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern void StoreZBUFVIF1( u_long128* base_addr, short start_addr, short pixel_mode, int buff_width, short x, short y, short width, short height );

void GSP_Swap_RB_And_32_2_24(ULONG *p_Src , unsigned char *DST , ULONG NumberOfColors)
{
	while (NumberOfColors--)
	{
		*(DST++) = (*p_Src & 0xff0000) >> 16;
		*(DST++) = (*p_Src & 0xff00) >> 8;
		*(DST++) = (*p_Src & 0xff) >> 0;
		p_Src++;
	}
}
void GSP_ReadScreen(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_RWPixels *d )
{
	ULONG	*SNAP_BUFFER;
	GSP_FlushAllFloatingFlip();
	Gsp_Flush(FLUSH_ALL);
	Gsp_Flush(FLUSH_ALL);
	SNAP_BUFFER = RamZBuffer;
    if (d->c_Write == 0)
    {
		/* Copy Frame buf */
		/* Compute Max REC SIZE */
		u_int VPTR;
		u_int W,H,X,Y;
		u_int YPos,YSizePos;
		unsigned char *Dest;
		if (d->c_Buffer == 1)
		{
			if ((GSP_IsFrameODD()))
			{ // Select buffer 2
				VPTR = GspGlobal_ACCESS(FBP1);
			} else 
			{ /* Select Buffer 1 */
				VPTR = GspGlobal_ACCESS(FBP2);
			}
		}
		else /* back buffer */
		{
			if ((GSP_IsFrameODD()))
			{ // Select buffer 2
				VPTR = GspGlobal_ACCESS(FBP2);
			} else 
			{ /* Select Buffer 1 */
				VPTR = GspGlobal_ACCESS(FBP1);
			}
		}
		X = (GspGlobal_ACCESS(Xsize)>>1) - 256;
		Y = (GspGlobal_ACCESS(Ysize)>>1) - 128;
		W = 512;
		H = 256;
		if (GspGlobal_ACCESS(Ysize) < 300)
		{
			H = 128;
			Y = (GspGlobal_ACCESS(Ysize)>>1) - 64;
		}
		Dest = d->p_Bitmap;

		YPos = 0;
		VPTR <<= (11 - 6);
		YSizePos = RS_SNAPBUFSIZE / W;
		while ((YPos + YSizePos) <= H)
		{
			StoreZBUFVIF1( SNAP_BUFFER, VPTR , SCE_GS_PSMCT32, GspGlobal_ACCESS(Xsize) , X, Y + YPos, W, YSizePos );
			GSP_Swap_RB_And_32_2_24(SNAP_BUFFER , Dest , YSizePos * W);
			YPos += YSizePos;
			Dest += YSizePos * W * 3;
		}
		if (YPos < H)
		{
			StoreZBUFVIF1( SNAP_BUFFER, VPTR , SCE_GS_PSMCT32, GspGlobal_ACCESS(Xsize) , X, Y + YPos, W, H - YPos );
			GSP_Swap_RB_And_32_2_24(SNAP_BUFFER , Dest , (H - YPos) * W);
		}
		if (GspGlobal_ACCESS(Ysize) < 300)
		{
			u_int *p_DST , *p_Src;
			p_Src = p_DST = (u_int *)d->p_Bitmap;
			
			/* Double the pixels with bilinear filtering */
			H = 127;
			p_DST += 384 * (H << 1);
			p_Src += 384 * H;
			while (H--) 
			{
				W = 384;
				while (W--) 
				{
					*p_DST = (0x7f7f7f7f & (*(p_Src) >> 1)) + (0x7f7f7f7f & (*(p_Src + 384) >> 1));
					*(p_DST+384) = *(p_Src);
					p_DST++;
					p_Src++;
				}
				p_DST -= 384 * 3;
				p_Src -= 384 * 2;
			}
		}
		if (GspGlobal_ACCESS(Xsize) == 512)
		{
			u_char *p_LineParser;
			u_int *p_512,*p_640;
			u_int b640[520];
			u_int b512[520];
			/* Horizontal strech of 512 -> 640 for solving same cover than PC version */
			p_LineParser = (u_int *)d->p_Bitmap;
			H = 255;
			while (H--) 
			{
				u_char *p_24;
				W = 512;
				p_24 = (u_char *)p_LineParser;
				p_512 = b512;
				while (W--)
				{
					u_int Color;
					Color = (u_int)*(p_24++);
					Color |= ((u_int)*(p_24++)) << 8;
					Color |= ((u_int)*(p_24++)) << 16;
					*(p_512++) = Color;
				}
				p_512 = b512 + 64;
				p_640 = b640;
				W = 113;
				while (W--)	
				{
					p_640[0] = p_512[0];
					p_640[4] = p_512[3];
					p_640[2] = (0x7f7f7f7f & (p_512[1] >> 1)) + (0x7f7f7f7f & (p_512[2] >> 1));
					p_640[1] = (0x7f7f7f7f & (p_640[0] >> 1)) + (0x7f7f7f7f & (p_640[2] >> 1));
					p_640[3] = (0x7f7f7f7f & (p_640[2] >> 1)) + (0x7f7f7f7f & (p_640[4] >> 1));
					p_512 += 4;
					p_640 += 5;
				}
				W = 512;
				p_24 = (u_char *)p_LineParser;
				p_640 = b640;
				while (W--)
				{
					*(p_24++) = *p_640 & 0xff;
					*(p_24++) = (*p_640>>8) & 0xff;
					*(p_24++) = (*p_640>>16) & 0xff;
					p_640++;
				}
				p_LineParser += 512 * 3;
			}
		}
    }
}
extern u_int DISPLAYSTACKSPR;
u_int ulSaveSPRSTACK;
extern u_int SavedStack;


void GSP_ReadScreen_NoSpor(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_RWPixels *d )
{
	if ((GetSP() & 0x70000000) == 0x70000000)
	{
		ulSaveSPRSTACK = GetSP();
		SetSP(SavedStack);
		GSP_ReadScreen(_pst_DD, d );
		SetSP(ulSaveSPRSTACK);
	} else GSP_ReadScreen(_pst_DD, d );
}


extern TEX_tdst_List	TEX_gst_GlobalList;
static u_int Clut[256] __attribute__ ((aligned(16)));

void GSP_ReloadPalette(GDI_tdst_DisplayData *_pst_DD, short _w_Palette)
{
    TEX_tdst_Palette *pst_Pal;
    u_int *pul_Color; 
    u_long ul64PaletteDescriptor;
    u_int  FBP;
    if ( (_w_Palette < 0) || (_w_Palette >= TEX_gst_GlobalList.l_NumberOfPalettes) )
        return;

    pst_Pal = TEX_gst_GlobalList.dst_Palette + _w_Palette;
    pul_Color = pst_Pal->pul_Color;
    
    if (!pul_Color) return;
    
    if (_w_Palette > GspGlobal_ACCESS(ulNumberOfPalettes)) return;
    ul64PaletteDescriptor = GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[_w_Palette].ul64_SetTexturePaletteParam;
    if (ul64PaletteDescriptor == 0L) return;
    if (!(ul64PaletteDescriptor & (1L << 61L))) return;
    FBP = ((u_int)(ul64PaletteDescriptor >> 37L)) & ((1 << 15) - 1); 
	{ // Convert palette;
		ULONG SRC,DST;
		for (SRC = 0;SRC < 256 ; SRC ++)
		{
			DST = SRC;
			if ((DST & 0x18) == 0x08)
			{
				DST += 8;
			} else
			if ((DST & 0x18) == 0x10)
			{
				DST -= 8;
			}
			Clut[DST] = pul_Color[SRC];
		}
		GSP_LoadPalette_SYNCRO_DMA(_w_Palette , (void *)Clut);
	}
    
}
				
LONG GSP_GetInterfaceTexBuffer(ULONG ulKey)
{
    LONG TC;
    TC =  GspGlobal_ACCESS(ulNumberOfTextures);
    while(TC--)
    {
	    if ( ulKey == GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TC].ulBigKey)
	        return (LONG)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TC].p_BitmapPtr;
	}
	return 0;
}

extern void GSP_PushZBuffer(float Z);
/*
 =======================================================================================================================
    Aim:    Treat miscelleanous request ( see request constant in GDInterface.h file )
 =======================================================================================================================
 */
LONG GSP_l_Request(ULONG _ul_Request, ULONG _ul_Data)
{
	/*$off*/
	switch(_ul_Request)
	{
	case GDI_Cul_Request_SetFogParams:				GSP_SetFogParams(GDI_gpst_CurDD, (SOFT_tdst_FogParams *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftEllipse:			GSP_DrawEllipse(GDI_gpst_CurDD, (SOFT_tdst_Ellipse *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftArrow:				GSP_DrawArrow(GDI_gpst_CurDD, (SOFT_tdst_Arrow *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftSquare:			GSP_DrawSquare(GDI_gpst_CurDD, (SOFT_tdst_Square *) _ul_Data); break;
	case GDI_Cul_Request_DrawTransformedTriangle:	GSP_DrawProjectedTriangle(GDI_gpst_CurDD, (SOFT_tdst_Vertex *) _ul_Data); break;
	case GDI_Cul_Request_DepthTest:					GSP_RS_DepthTest( _ul_Data ); break;
	case GDI_Cul_Request_DepthFunc:					GSP_RS_DepthFunc( _ul_Data ); break;
	case GDI_Cul_Request_DrawPoint:					GSP_DrawPoint( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_DrawPointMin:				GSP_DrawPointMin( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_DrawLine:					GSP_DrawLine( GDI_gpst_CurDD, (MATH_tdst_Vector **) _ul_Data ); break;
	case GDI_Cul_Request_DrawTriangle:				break;
	case GDI_Cul_Request_DrawQuad:					GSP_DrawQuad( GDI_gpst_CurDD, (MATH_tdst_Vector **) _ul_Data ); break;
	case GDI_Cul_Request_ReloadTexture:				GSP_ReloadTexture( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
	case GDI_Cul_Request_ReloadTexture2:			GSP_ReloadTexture2( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
	case GDI_Cul_Request_SizeOfPoints:	   			GSP_PointSize( GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_SizeOfLine:				GSP_LineSize(GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_DrawLineEx:				GSP_DrawLineEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawLineEx *) _ul_Data); break;
	case GDI_Cul_Request_DrawPointEx:				GSP_DrawPointEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawPointEx *)_ul_Data ); break;
	case GDI_Cul_Request_DrawPointSize:				GSP_DrawPoint_Size(GDI_gpst_CurDD, (void  **)_ul_Data);break;
	case GDI_Cul_Request_BeforeDrawSprite:			GSP_BeforeDrawSprite(GDI_gpst_CurDD);break;
	case GDI_Cul_Request_DrawSprite:				GSP_DrawSprite(GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data );break;
	case GDI_Cul_Request_AfterDrawSprite:			GSP_AfterDrawSprite(GDI_gpst_CurDD);break;//*/
    case GDI_Cul_Request_EnableFog:                 GSP_Fogged( _ul_Data ); break;
	case GDI_Cul_Request_ReadPixel:                 GSP_ReadPixel( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
    case GDI_Cul_Request_ReadScreen:                GSP_ReadScreen_NoSpor( GDI_gpst_CurDD, (GDI_tdst_Request_RWPixels *) _ul_Data ); break;
    case GDI_Cul_Request_ReloadPalette:             GSP_ReloadPalette( GDI_gpst_CurDD, (short) _ul_Data ); break;
    case GDI_Cul_Request_GetInterfaceTexBuffer:     return GSP_GetInterfaceTexBuffer(_ul_Data);break;
	case GDI_Cul_Request_PushZBuffer: GSP_PushZBuffer(*(float *)&_ul_Data); break;
	}
	/*$on*/
	return 0;
}

#if defined PSX2_TARGET && defined __cplusplus
}
#endif

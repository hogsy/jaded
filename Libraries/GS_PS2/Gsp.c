

#include <sys/types.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libpad.h>
#include <libpC.h>
#include <string.h>

#include "Gsp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"

#ifdef __cplusplus
extern "C" {
#endif
/*$4 prototypes */
void Gsp_SheetCode();
void GSP_FirstInit_SPR();
void Gsp_FirstInit();
extern void TIM_UpdateTimerFrequency(void);

#define GSP_DefaultStatus (GSP_Status_SetRenderStateOn | /*GSP_Status_PAL_On | */GSP_Status_VAA_CRTS /*| GSP_Status_Raster_Details_On | GSP_Status_Raster_On*/)
extern u_int My_Triangle_GIF2 __attribute__((section(".vudata")));

extern void DisplayRights();
extern u_int volatile VBlankCounter;
extern u_int volatile VBlankOdd;
extern u_int ulVideoScreentValid;
extern u_int ShowNormals;
static u_int AvoidFirst = 50;
//u_int FuckTheZougl = 0;
#pragma	nofpregforblkmv	on
void GSP_SetVideoCRTC(int ODD , int both)
{
	u_int	SaveMAGH;
//	if (VBlankCounter - GspGlobal_ACCESS(LastVBlankCounter)) FuckTheZougl = 1024<<2;
#ifndef GSP_USE_TRIPLE_BUFFERRING
	if (both)
	{
		Gsp_Flush(FLUSH_ALL);
		DPUT_GS_CSR( GS_CSR_FINISH_M );
		GSP_SetRegister(GSP_FINISH 	, 0L);
		Gsp_SetGSRegisters();
		Gsp_Flush(FLUSH_ALL);
		// check if FINISH event occured
		while ((*GS_CSR & GS_CSR_FINISH_M) == 0 ) {};
		DPUT_GS_CSR( GS_CSR_FINISH_M ); // Set to 1 for next used
		while ((*GS_CSR & GS_CSR_FLUSH_M) != 0) {};
	}
	GspGlobal_ACCESS(LastVBlankCounter) = VBlankCounter;
#endif
	if ((!GspGlobal_ACCESS(ModePEnable)) && (!ulVideoScreentValid) && (GspGlobal_ACCESS(Status) & GSP_Status_VAA_CRTS) && (GspGlobal_ACCESS(VideoMode) == GSP_VRES_x2))
	{
		GspGlobal_ACCESS(gs_DE1).pmode.EN1 = 1;
		GspGlobal_ACCESS(gs_DE1).pmode.EN2 = 1;
		GspGlobal_ACCESS(gs_DE2).pmode.EN1 = 1;
		GspGlobal_ACCESS(gs_DE2).pmode.EN2 = 1;
	}
	else 
	{
		GspGlobal_ACCESS(gs_DE1).pmode.EN1 = 0;
		GspGlobal_ACCESS(gs_DE1).pmode.EN2 = 1;
		GspGlobal_ACCESS(gs_DE2).pmode.EN1 = 0;
		GspGlobal_ACCESS(gs_DE2).pmode.EN2 = 1;
	}
	if (GspGlobal_ACCESS(ulFade_Video_Enable)) 
	{
		SaveMAGH = GspGlobal_ACCESS(gs_DE1).display.MAGH;
		GspGlobal_ACCESS(gs_DE1).display.MAGH = SaveMAGH*2 + 1;
		GspGlobal_ACCESS(gs_DE1).display1.MAGH = SaveMAGH*2 + 1;
		GspGlobal_ACCESS(gs_DE2).display.MAGH = SaveMAGH*2 + 1;
		GspGlobal_ACCESS(gs_DE2).display1.MAGH = SaveMAGH*2 + 1;
	}
	if (!(ODD))
	{
		DPUT_GS_DISPLAY2(*(u_long *)&GspGlobal_ACCESS(gs_DE1).display); //DISPLAY2
		DPUT_GS_DISPLAY1( *(u_long *)&GspGlobal_ACCESS(gs_DE1).display1); //DISPLAY1
		DPUT_GS_DISPFB2(*(u_long *)&GspGlobal_ACCESS(gs_DE1).dispfb);  //DISPFB2
		DPUT_GS_DISPFB1(*(u_long *)&GspGlobal_ACCESS(gs_DE1).dispfb1);  //DISPFB1
		DPUT_GS_PMODE(*(u_long *)&GspGlobal_ACCESS(gs_DE1).pmode);   //PMODE
		DPUT_GS_SMODE2(*(u_long *)&GspGlobal_ACCESS(gs_DE1).smode2);  //SMODE2
	}
	else
	{
		DPUT_GS_DISPLAY2(*(u_long *)&GspGlobal_ACCESS(gs_DE2).display); //DISPLAY2
		DPUT_GS_DISPLAY1( *(u_long *)&GspGlobal_ACCESS(gs_DE2).display1); //DISPLAY1
		DPUT_GS_DISPFB2(*(u_long *)&GspGlobal_ACCESS(gs_DE2).dispfb);  //DISPFB2
		DPUT_GS_DISPFB1(*(u_long *)&GspGlobal_ACCESS(gs_DE2).dispfb1);  //DISPFB1
		DPUT_GS_PMODE(*(u_long *)&GspGlobal_ACCESS(gs_DE2).pmode);   //PMODE
		DPUT_GS_SMODE2(*(u_long *)&GspGlobal_ACCESS(gs_DE2).smode2);  //SMODE2
	}
	if (GspGlobal_ACCESS(ulFade_Video_Enable)) 
	{
		GspGlobal_ACCESS(gs_DE1).display.MAGH = SaveMAGH;
		GspGlobal_ACCESS(gs_DE1).display1.MAGH = SaveMAGH;
		GspGlobal_ACCESS(gs_DE2).display.MAGH = SaveMAGH;
		GspGlobal_ACCESS(gs_DE2).display1.MAGH = SaveMAGH;
	}
}
#pragma	nofpregforblkmv	reset
#ifndef _FINAL_



extern u_int bTakeSnapshot;
extern void Gsp_LaunchSnapShot(u_int Mode);
void Gsp_SheetCode()
{
	u_int paddata;
	GSP_tdst_DualShock2 rdata;
	static u_int Last = 0;
	static u_int Special = 0;
	if (AvoidFirst)
	{
		AvoidFirst--;
		return;
	}
	if ((GspGlobal_ACCESS(Status) & GSP_Status_Setup_On)) Gsp_Setup();
	
	if(scePadRead(0, 0, (char *)&rdata) > 0){
		paddata = 0xffff ^ ((((char *)&rdata)[2] << 8) | ((char *)&rdata)[3]);
	}
	else{
		paddata = 0;
	}
	if ((rdata.InfoC > 200)/* && (rdata.InfoX > 200)*/  && (rdata.InfoS > 200))
	{
		GspGlobal_ACCESS(Status) |= GSP_Status_Setup_On;
		if (bTakeSnapshot > 100)
		bTakeSnapshot = 101;
	}
	
	if ((rdata.InfoX > 200)/* && (rdata.InfoX > 200)*/  && (rdata.InfoC > 200))
	{
		if (!(Last & 1))
		{
			extern u_int NoZLST;
			NoZLST &= ~0x100000;
			GspGlobal_ACCESS(Status) &= ~(GSP_Status_Raster_On|GSP_Status_Raster_Details_On|GSP_Status_TNumCounterOn|GSP_Status_SVS|GSP_Status_Show_Depth);
			ShowNormals = 0;
			scePcStop();
			switch(Last >> 1)
			{
				case 0:break;
				case 1:GspGlobal_ACCESS(Status) |= (GSP_Status_Raster_On|GSP_Status_Raster_Details_On);break;
				case 2:ShowNormals = 1;NoZLST |= 0x100000;break;
				case 3:ShowNormals = 2;break;
				case 4:ShowNormals = 3;NoZLST |= 0x100000;break;
				case 5:GspGlobal_ACCESS(Status) |= (GSP_Status_Show_Depth);break;
				case 6:GspGlobal_ACCESS(Status) |= (GSP_Status_TNumCounterOn);break;
				case 7:GspGlobal_ACCESS(Status) |= (GSP_Status_SVS);break;
			}
			Last+=2;
			Last&=7<<1;
		}
		Last |= 1;
	} else Last &= ~1;
}
#endif

u_long128 D1_ASR[4] __attribute__((aligned (64)));
void Gsp_LoadVU1Code()
{
	GSP_DMA_Source_Chain_TAG DmaCallEnd[2] __attribute__((aligned(64)));
	GSP_DMA_Source_Chain_TAG *p_DmaCallEnd;
	GspGifTag *p_DmaTG;
	FlushCache(0);
	Gsp_Flush(FLUSH_ALL);
	
	// Set default GIF TAG 
	p_DmaTG = (GspGifTag*)&My_Triangle_GIF2;
	GSP_UnCachePtr( p_DmaTG );
	Gsp_M_SetGifTag(p_DmaTG, 	0 ,   1 ,    1 , 3    , 0  ,     12 , 0x5A125A125A12L); /* set VU1 GIF TAG for triangles */
	p_DmaTG ++;
 	Gsp_M_SetGifTag(p_DmaTG,    0 ,   0 ,    1 , 4    , 0  ,     4 , 0x5A12L); /* set VU1 GIF TAG for Strips */
	p_DmaTG ++;
	Gsp_M_SetGifTag(p_DmaTG,    1 ,   1 ,    0 , 4    , 0  ,     1 , 0x2L); /* set VU1 GIF TAG for EOP */
	
	p_DmaCallEnd = DmaCallEnd;
	GSP_UnCachePtr( p_DmaCallEnd );
	p_DmaCallEnd->id = GSP_SCDma_ID_Call;
	p_DmaCallEnd->qwc = 0;
	p_DmaCallEnd->mark = 0;
	p_DmaCallEnd->next = (u_int)&My_Triangle_code ;
	p_DmaCallEnd->p[0] = 0;
	p_DmaCallEnd->p[1] = 0;
	
	p_DmaCallEnd++;
	p_DmaCallEnd->id = GSP_SCDma_ID_End;
	p_DmaCallEnd->qwc = 0;
	p_DmaCallEnd->mark = 0;
	p_DmaCallEnd->next = 0;
	p_DmaCallEnd->p[0] = 0;
	p_DmaCallEnd->p[1] = 0;
	
	*D1_ASR0 = (ULONG)&D1_ASR[0];
	*D1_ASR1 = (ULONG)&D1_ASR[1];
	FlushCache(0);
	Gsp_SendToDma1((u_int)DmaCallEnd,(1/*From	 memory */ | (1<<2)/*chain Mode*/ | /*Start */ (1<<8) | /* 1 adress pushed */(1<<4)));
	Gsp_Flush(FLUSH_ALL);
}


void Gsp_DrawRectangle(float fx,float fy,float fw,float fh, u_int C0 , u_int C1 , unsigned long T)
{
	MyIVectorFormat vertex, vertexUV, color;
	Gsp_Primitiv *p_Prim;
	u_int x, y, w, h;
	unsigned long DPtr;
	DPtr = (unsigned long)(GspGlobal_ACCESS(FBP2)) * 3L;
	DPtr *= 2048L;
	DPtr >>= 6L;
	
	x = (u_int)(fx * (float)GspGlobal_ACCESS(Xsize));
	y = (u_int)(fy * (float)GspGlobal_ACCESS(Ysize));
	w = (u_int)(fw * (float)GspGlobal_ACCESS(Xsize));
	h = (u_int)(fh * (float)GspGlobal_ACCESS(Ysize));

	Gsp_Flush(FLUSH_ALL);

	GSP_SetRegister(GSP_TEST_1 	, ((1L << 16L) | (1L << 17L)));
	GSP_SetRegister(GSP_ALPHA_1 	, T);
	GSP_SetRegister(GSP_PRMODE 	, (1L<<8L) | (0L<<4L) | (1L<<6L) | (1L<<3L));
	GSP_SetRegister(GSP_TEX0_1 	, (DPtr<<0L) | (8L<<14L) | (2L<<20L) | (8L<<26L) | (8L<<30L) | (1L<<34L) | (0L<<35L) | (0L<<37L) | (0L<<51L) | (0L<<55L) | (0L<<56L) | (0L<<61L));
	GSP_SetRegister(GSP_TEX1_1 	, (0L<<0L) | (3L<<2L) | (1L<<5L) | (5L<<6L) | (0L<<9L) | (0L<<19L) | (0x68L<<32L));

	DPtr += 1024L;

	GSP_SetRegister(GSP_MIPTBP1_1 	, ((DPtr + 0L) << 0L) | (8L<<14L) | ((DPtr + 512L)<<20L) | (8L<<34L) | ((DPtr + 512L + 256L)<<40L) | (8L<<54L));
	GSP_SetRegister(GSP_MIPTBP2_1 	, ((DPtr + 512L + 256L + 128L)<<0L) | (8L<<14L) | ((DPtr + 512L + 256L + 128L + 64L)<<20L) | (8L<<34L) | ((DPtr + 512L + 256L + 128L + 64L + 32L)<<40L) | (8L<<54L));
	GSP_SetRegister(GSP_CLAMP_1 	, (0L<<0L) | (0L<<2L) | (0L<<4L) | (0L<<14L) | (0L<<24L) | (0L<<34L));
	GSP_SetRegister(GSP_SCANMSK 	, 0L);
	Gsp_SetGSRegisters();
	Gsp_Flush(FLUSH_DMA2);
	p_Prim = &GspGlobal_ACCESS(PrimX);
	GSP_UnCachePtr(p_Prim);
	color.x = C0 & 0xff;
	color.y = (C0 >> 8) & 0xff;
	color.z = (C0 >> 16) & 0xff;
	color.w = (C0 >> 24);
	Gsp_M_SetGifTag(&p_Prim->giftag, 1 , 1 , 1 , 4 , 0 , 8 , 0x51515151);
	p_Prim->NumRegs = 0;
	*(ULONG *)&vertex.z = 0;//0xC0DE0001;
	vertex.w = 0;
	
	vertex.x = (x + GspGlobal_ACCESS(BX0)) << 4L;
	vertex.y = (y + GspGlobal_ACCESS(BY0)) << 4L;
	p_Prim->pValues[p_Prim->NumRegs++] = *((u_long128*)&color);
	p_Prim->pValues[p_Prim->NumRegs++] = *((u_long128*)&vertex);
	vertex.x = (x + GspGlobal_ACCESS(BX0)) << 4L;
	vertex.y = (y + h + GspGlobal_ACCESS(BY0)) << 4L;
	p_Prim->pValues[p_Prim->NumRegs++] = *((u_long128*)&color);
	p_Prim->pValues[p_Prim->NumRegs++] = *((u_long128*)&vertex);
	
	color.x = C1 & 0xff;
	color.y = (C1 >> 8) & 0xff;
	color.z = (C1 >> 16) & 0xff;
	color.w = (C1 >> 24);
	
	vertex.x = (x + w + GspGlobal_ACCESS(BX0)) << 4L;
	vertex.y = (y + GspGlobal_ACCESS(BY0)) << 4L;
	p_Prim->pValues[p_Prim->NumRegs++] = *((u_long128*)&color);
	p_Prim->pValues[p_Prim->NumRegs++] = *((u_long128*)&vertex);
	vertex.x = (x + w + GspGlobal_ACCESS(BX0)) << 4L;
	vertex.y = (y + h + GspGlobal_ACCESS(BY0)) << 4L;
	p_Prim->pValues[p_Prim->NumRegs++] = *((u_long128*)&color);
	p_Prim->pValues[p_Prim->NumRegs++] = *((u_long128*)&vertex);
	
	Gsp_SendToGif((u_int)p_Prim , p_Prim->NumRegs + 1);
	Gsp_Flush(FLUSH_DMA2);
	GSP_SetRegister(GSP_TEST_1 	, ((1L << 16L) | (2L << 17L)));
	GSP_SetRegister(GSP_ALPHA_1 	, GST_COPY);
	GSP_SetRegister(GSP_PRMODE 	, (0<<4) | (1<<6) | (1<<3));
	GSP_SetRegister(GSP_COLCLAMP 	, 1L);
	GSP_SetRegister(GSP_SCANMSK 	, 0L);
	Gsp_SetGSRegisters();
	Gsp_Flush(FLUSH_DMA2);
	Gsp_Flush(FLUSH_ALL);
}

void Gsp_SetGSRegisters()
{
	if (!GspGlobal_ACCESS(p_CurrentRegWriter)->NumRegs) return;
	Gsp_M_SetGifTag(&GspGlobal_ACCESS(p_CurrentRegWriter->giftag), GspGlobal_ACCESS(p_CurrentRegWriter)->NumRegs , 1 , 0 , 0 , GSP_GIF_FLG_PACKED , 1 ,   0xE);
	Gsp_SendToGif((u_int)GspGlobal_ACCESS(p_CurrentRegWriter) , GspGlobal_ACCESS(p_CurrentRegWriter)->NumRegs + 1);
	if (GspGlobal_ACCESS(p_CurrentRegWriter) == &GspGlobal_ACCESS(stMRW1)) 
		GspGlobal_ACCESS(p_CurrentRegWriter) = &GspGlobal_ACCESS(stMRW2);
	else
		GspGlobal_ACCESS(p_CurrentRegWriter) = &GspGlobal_ACCESS(stMRW1);
	GSP_UnCachePtr(GspGlobal_ACCESS(p_CurrentRegWriter));
	GspGlobal_ACCESS(p_CurrentRegWriter)->NumRegs = 0;
}

Gsp_BigStruct 		*p_SavedGlobals;
__declspec(scratchpad) u_int volatile gsulSyncroPath;
__declspec(scratchpad) Gsp_BigStruct 		gs_st_Globals;
GDI_tdst_DisplayData gpst_GSP_stDD;

GDI_tdst_DisplayData  	*GDI_gpst_CurDD_Save;
void GSP_Save_SPR()
{
/*	L_memcpy((void *)p_SavedGlobals , (void *)&gs_st_Globals , 1024*16);
	L_memcpy((void *)GDI_gpst_CurDD_Save , (void *)&gpst_GSP_stDD , sizeof(GDI_tdst_DisplayData));
	GDI_gpst_CurDD = GDI_gpst_CurDD_Save;//*/
}
void GSP_Restore_SPR()
{
/*	L_memcpy((void *)&gs_st_Globals , (void *)p_SavedGlobals , 1024*16);
	L_memcpy((void *)&gpst_GSP_stDD , (void *)GDI_gpst_CurDD_Save , sizeof(GDI_tdst_DisplayData));
	GDI_gpst_CurDD = (GDI_tdst_DisplayData  *)&gpst_GSP_stDD;//*/
}
void GSP_FirstInit_SPR()
{
	GDI_gpst_CurDD_Save = 	GDI_gpst_CurDD ;
	L_memcpy((void *)&gpst_GSP_stDD , (void *)GDI_gpst_CurDD_Save , sizeof(GDI_tdst_DisplayData));
	GDI_gpst_CurDD = (GDI_tdst_DisplayData  *)&gpst_GSP_stDD;//*/
	
}

extern BOOL ps2MAI_gb_VideoModeNTSC;
extern float    STR_f_XOffset;
extern float    STR_f_XFactor;
extern float    STR_f_YOffset;
extern float    STR_f_YFactor;

int Gsp_DMABusError(int Cause)
{
   	ExitHandler();
   	return 0;
}

void Gsp_FirstInit()
{
	MEMpro_StartMemRaster();
	FlushCache(0);
	L_memset((char *)&GspGlobal_ACCESS(FIRSTVALUE) , 0 , sizeof(Gsp_BigStruct));
	gsulSyncroPath = 0;
	GspGlobal_ACCESS(p_CurrentRegWriter) = &GspGlobal_ACCESS(stMRW1);
	GSP_UnCachePtr(GspGlobal_ACCESS(p_CurrentRegWriter));

    	GspGlobal_ACCESS(fCurrentFocale) = 1.0f;
    	GspGlobal_ACCESS(fAddY) = 2048.0f;

    	sceGsResetPath();

	AddIntcHandler(INTC_VBLANK_E, Gsp_vblankHandler, 0);
    	
    	
	AddDmacHandler(DMAC_VIF1, Gsp_DMA1End, 0);
	AddDmacHandler(DMAC_GIF, Gsp_DMA2End, 0);
	AddDmacHandler(DMAC_TO_SPR, Gsp_DMA9End, 0);
	AddDmacHandler(DMAC_FROM_SPR, Gsp_DMA8End, 0);
	AddDmacHandler(DMAC_CIS, Gsp_DMABusError, 0);
	AddDmacHandler(DMAC_MEIS, Gsp_DMABusError, 0);
	AddDmacHandler(DMAC_BEIS, Gsp_DMABusError, 0);
	
	

    	
	if(ps2MAI_gb_VideoModeNTSC)
	{
    	GspGlobal_ACCESS(Status) = GSP_DefaultStatus & (~GSP_Status_PAL_On);        
    }
    else
    {
    	GspGlobal_ACCESS(Status) = GSP_DefaultStatus | (GSP_Status_PAL_On);
    }
    TIM_UpdateTimerFrequency();
	
	GspGlobal_ACCESS(fFogCorrector) = 0.5f;
	GspGlobal_ACCESS(fFogCorrectorFar) = 1.f;
	Gsp_DrawBufferFirstInit();
	
	EnableDmac(DMAC_VIF1);
	EnableDmac(DMAC_GIF);//*/
	EnableDmac(DMAC_TO_SPR);//*/
	EnableDmac(DMAC_FROM_SPR);//*/
	EnableIntc(INTC_VBLANK_E);
	
//	EnableDmac(DMAC_CIS);//*/
//	EnableDmac(DMAC_MEIS);//*/
//	EnableDmac(DMAC_BEIS);//*/
	

	Gsp_LoadVU1Code();
	GSP_ClearVRAM();
	
	Gsp_InitVU1Matrix_Extention();

	p_SavedGlobals = (Gsp_BigStruct *)malloc(1024 * 16);
	
	GspGlobal_ACCESS(f2DFFX_A2D) = 1.002f;
	GspGlobal_ACCESS(f2DFFY_A2D) = 1.153f;
	
	STR_f_YFactor = 1.0f/(1.0f + (GspGlobal_ACCESS(f2DFFY_A2D) - 1.0f) / 1.0f);
	
	
	GspGlobal_ACCESS(f2DFFX_B2D) = 1.000f;
	GspGlobal_ACCESS(f2DFFY_B2D) = 1.000f;
	
	
	GspGlobal_ACCESS(p_AE_Params) = (GSP_AfterEffectParams *)malloc(sizeof (GSP_AfterEffectParams));
	L_memset((void *)GspGlobal_ACCESS(p_AE_Params) , 0 , sizeof(GSP_AfterEffectParams));
	GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart = 5.0f;
	GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd   = 20.0f;
	GspGlobal_ACCESS(p_AE_Params)->Contrast = 0.5f;
	GspGlobal_ACCESS(p_AE_Params)->Brighness = 0.5f;
	GspGlobal_ACCESS(HRepos) = 20;
	
	
	GSP_FF_Init();
	GSP_InitGeoCache(65536);
	MEMpro_StopMemRaster(MEMpro_Id_GSP);
	GspGlobal_ACCESS(ulAvailableTextureCacheSize) = 2 << 20;
	
}

void GSP_TV_SetBackColor(u_int Color)
{
	*GS_BGCOLOR = Color;
}

extern float TIM_f_Timer1Frequency;
extern float TIM_f_OoTimer1Frequency;
extern float fGlobalAcc;
extern void GSP_RemapInterfaceTexture(u_int OldTBP);
void Gsp_InitFB_VIDEO(u_int VOn)
{
	u_int RealSize, RealY, Remorf , OldTBP;
	static sceGsDispEnv2Circuits 	gs_DE1,gs_DE2;
	static u_int FBP1 = 0;
	static u_int FBP2 = 0;
#define ALIGN_FBF(a) a = (a + 8191) & ~8191
#define ALIGN_TBF(a) a = (a + 127) & ~127
	if (!VOn) 
	{
		if (FBP1 || FBP2)
		{
			GspGlobal_ACCESS(gs_DE1) = gs_DE1;
			GspGlobal_ACCESS(gs_DE2) = gs_DE2;
			GspGlobal_ACCESS(FBP1) = FBP1;
			GspGlobal_ACCESS(FBP2) = FBP2;
		}
		FBP1 = FBP2 = 0;
	} else
	{
		
		gs_DE1 = GspGlobal_ACCESS(gs_DE1);
		gs_DE2 = GspGlobal_ACCESS(gs_DE2);
		FBP1 = GspGlobal_ACCESS(FBP1);
		FBP2 = GspGlobal_ACCESS(FBP2);
		Remorf = 0;
		GspGlobal_ACCESS(FBP1) = (GspGlobal_ACCESS(Xsize) * (32 >> 3)) /* / 2048 */;;
		ALIGN_FBF(GspGlobal_ACCESS(FBP1));
		RealY =  (GspGlobal_ACCESS(Ysize) + 31) & ~31;
		RealSize = RealY - Remorf * 2;
		RealSize *= GspGlobal_ACCESS(Xsize);
		RealSize *= 32 >> 3;
		GspGlobal_ACCESS(FBP2) = RealSize;
		ALIGN_FBF(GspGlobal_ACCESS(FBP2));
		/* align FBP2 */
		/* align TBP */
		GspGlobal_ACCESS(FBP1) >>= 13;
		GspGlobal_ACCESS(FBP2) >>= 13;
		GspGlobal_ACCESS(FBP2)-= GspGlobal_ACCESS(FBP1);
		if (Remorf) GspGlobal_ACCESS(FBP1) = 512 - GspGlobal_ACCESS(FBP1);
#ifdef GSP_USE_TRIPLE_BUFFERRING
		sceGsSetDefDispEnv(&GspGlobal_ACCESS(gs_DE1) , (short)2 , (short)GspGlobal_ACCESS(Xsize) , (short)GspGlobal_ACCESS(Ysize) , (short)0 , (short)0 );
		GspGlobal_ACCESS(gs_DE2) = GspGlobal_ACCESS(gs_DE1);
		GspGlobal_ACCESS(gs_DE1).dispfb.FBP = (unsigned long)GspGlobal_ACCESS(FBP1);
		GspGlobal_ACCESS(gs_DE2).dispfb.FBP = (unsigned long)GspGlobal_ACCESS(FBP1);
		GspGlobal_ACCESS(FBP2) = GspGlobal_ACCESS(FBP1);
		{
			extern u_int RealFBP2;		
			RealFBP2 = (GspGlobal_ACCESS(Ysize) + 31) & ~31;
			RealFBP2 *= GspGlobal_ACCESS(Xsize);
			RealFBP2 *= 32 >> 3;
			ALIGN_FBF(RealFBP2);
			RealFBP2 >>= 13;
		}
#else
		sceGsSetDefDispEnv(&GspGlobal_ACCESS(gs_DE1) , (short)GspGlobal_ACCESS(BppMode) , (short)GspGlobal_ACCESS(Xsize) , (short)GspGlobal_ACCESS(Ysize) , (short)0 , (short)0 );
		GspGlobal_ACCESS(gs_DE2) = GspGlobal_ACCESS(gs_DE1);
		GspGlobal_ACCESS(gs_DE1).dispfb.FBP = (unsigned long)GspGlobal_ACCESS(FBP1);
		GspGlobal_ACCESS(gs_DE2).dispfb.FBP = (unsigned long)GspGlobal_ACCESS(FBP2);
#endif		
		if (GspGlobal_ACCESS(Status) & GSP_Status_PAL_On)
		{
			GspGlobal_ACCESS(gs_DE1).display.DY += 16;
			GspGlobal_ACCESS(gs_DE2).display.DY += 16;
		}
		{
			GspGlobal_ACCESS(gs_DE1).display.DY += Remorf;
			GspGlobal_ACCESS(gs_DE2).display.DY += Remorf;
			GspGlobal_ACCESS(gs_DE1).dispfb.DBY += Remorf;
			GspGlobal_ACCESS(gs_DE2).dispfb.DBY += Remorf;
			GspGlobal_ACCESS(gs_DE1).display.DH -= Remorf * 2;
			GspGlobal_ACCESS(gs_DE2).display.DH -= Remorf * 2;
		}
		GspGlobal_ACCESS(gs_DE1).display.DX += GspGlobal_ACCESS(HRepos);
		GspGlobal_ACCESS(gs_DE2).display.DX += GspGlobal_ACCESS(HRepos);
		
		if (1)
		{
			// 2nd circuit
			*(u_long*)&GspGlobal_ACCESS(gs_DE1).dispfb1 = *(u_long*)&GspGlobal_ACCESS(gs_DE1).dispfb;
			*(u_long*)&GspGlobal_ACCESS(gs_DE1).display1 = *(u_long*)&GspGlobal_ACCESS(gs_DE1).display;
			

			GspGlobal_ACCESS(gs_DE1).dispfb.DBY ++; // dispfb2 has DBY field of 1
			GspGlobal_ACCESS(gs_DE1).display.DH = (GspGlobal_ACCESS(gs_DE1).display.DH)-1;
			GspGlobal_ACCESS(gs_DE1).display.DX = GspGlobal_ACCESS(gs_DE1).display.DX +2; // display2 has magh/2
			GspGlobal_ACCESS(gs_DE1).pmode.ALP = 0x80;
			GspGlobal_ACCESS(gs_DE1).pmode.EN1 = 1;
			GspGlobal_ACCESS(gs_DE1).pmode.EN2 = 1;
			// 2nd circuit
			*(u_long*)&GspGlobal_ACCESS(gs_DE2).dispfb1 = *(u_long*)&GspGlobal_ACCESS(gs_DE2).dispfb;
			*(u_long*)&GspGlobal_ACCESS(gs_DE2).display1 = *(u_long*)&GspGlobal_ACCESS(gs_DE2).display;

			GspGlobal_ACCESS(gs_DE2).dispfb.DBY ++; // dispfb2 has DBY field of 1
			GspGlobal_ACCESS(gs_DE2).display.DH = (GspGlobal_ACCESS(gs_DE2).display.DH)-1;
			GspGlobal_ACCESS(gs_DE2).display.DX = GspGlobal_ACCESS(gs_DE2).display.DX +2; // display2 has magh/2
			GspGlobal_ACCESS(gs_DE2).pmode.ALP = 0x80;
			GspGlobal_ACCESS(gs_DE2).pmode.EN1 = 1;
			GspGlobal_ACCESS(gs_DE2).pmode.EN2 = 1;
			
		}
	}
}

void Gsp_InitFB(u_int H_Resolution, u_int VRES, u_int ColorBitNum , u_int ZBufferBitNum)
{
	u_int bNeedFlip;
	u_int PALNTSC;
	u_int SpecialMde640480;
	static u_int bFirst = 1;
	if (bFirst)
	{
		Gsp_FirstInit();
		GspGlobal_ACCESS(Morfling) = 00;
		H_Resolution = GSP_640x2__;
		VRES = GSP_VRES_x2;
		ColorBitNum = 32;
		ZBufferBitNum = 32;
		bFirst = 0;
	}
	Gsp_Flush(FLUSH_ALL);
	bNeedFlip = 0;
	SpecialMde640480 = 0;
	fGlobalAcc = 1.0f;
	if (GspGlobal_ACCESS(ModePEnable))
	{
		GspGlobal_ACCESS(YCorrectionCoef) = (5.5f/4.0f); // 5.5 cause good ratio from Vince.. Should be 5/4
		PALNTSC = SCE_GS_DTV480P;
		GspGlobal_ACCESS(Ysize) = 239;
		TIM_f_Timer1Frequency = 15734.26573f;
		TIM_f_OoTimer1Frequency = 1.0f / TIM_f_Timer1Frequency;
		VRES = GSP_VRES_x2;
		fGlobalAcc = 2.0f;
	} else
	if (GspGlobal_ACCESS(Status) & GSP_Status_PAL_On)
	{
		GspGlobal_ACCESS(YCorrectionCoef) = (5.5f/4.0f); // 5.5 cause good ratio from Vince.. Should be 5/4
		PALNTSC = SCE_GS_PAL;
		if ((H_Resolution == 640) && (VRES == GSP_VRES_x2))
		{
			GspGlobal_ACCESS(Ysize) = 239;
			SpecialMde640480 = 1;
		}
		else
			GspGlobal_ACCESS(Ysize) = 255;
			
		TIM_f_Timer1Frequency = 15625.0f;
		TIM_f_OoTimer1Frequency = 1.0f / TIM_f_Timer1Frequency;
	} else
	{// NTSC
		GspGlobal_ACCESS(YCorrectionCoef) = (5.f/4.0f); // 5.5 cause good ratio from Vince.. Should be 5/4
		PALNTSC = SCE_GS_NTSC;
		GspGlobal_ACCESS(Ysize) = 223;
		TIM_f_Timer1Frequency = 15734.26573f;
		TIM_f_OoTimer1Frequency = 1.0f / TIM_f_Timer1Frequency;
	}
	{
		GspGlobal_ACCESS(VideoMode) = VRES;
		switch (VRES)
		{
			case GSP_VRES_xAUTO:
			case GSP_VRES_x1:
				sceGsResetGraph(0, SCE_GS_INTERLACE, PALNTSC, SCE_GS_FRAME);
				bNeedFlip = 1;			
				break;
			case GSP_VRES_x2:
				if (GspGlobal_ACCESS(ModePEnable))
					sceGsResetGraph(0, SCE_GS_NOINTERLACE, PALNTSC, SCE_GS_FRAME);
				else
					sceGsResetGraph(0, SCE_GS_INTERLACE, PALNTSC, SCE_GS_FIELD);
				bNeedFlip = 1;			
				GspGlobal_ACCESS(Ysize) *= 2;
				GspGlobal_ACCESS(Ysize) ++;
				break;
			case GSP_VRES_x2_TripleAA:
				sceGsResetGraph(0, SCE_GS_INTERLACE, PALNTSC, SCE_GS_FRAME);
				bNeedFlip = 1;			
				GspGlobal_ACCESS(Ysize) *= 2;
				GspGlobal_ACCESS(Ysize) ++;
				break;
				
		}
	}
	

	GspGlobal_ACCESS(Xsize) = H_Resolution;

	switch (ColorBitNum)
	{
		case 32:GspGlobal_ACCESS(BppMode) = 0;
			break; 
		case 24:GspGlobal_ACCESS(BppMode) = 1;
			break; 
		case 16:GspGlobal_ACCESS(BppMode) = 2;
			break; 
	}
	switch (ZBufferBitNum)
	{
		case 32:
			GspGlobal_ACCESS(ZBppMode) = 0;
			GspGlobal_ACCESS(ZFactor) = (float)0x7fffffF;
			break; 
		case 24:
			GspGlobal_ACCESS(ZBppMode) = 1;
			GspGlobal_ACCESS(ZFactor) = (float)0xffffff / 16.0f;
			break; 
		case 16:
			GspGlobal_ACCESS(ZBppMode) = 2;
			GspGlobal_ACCESS(ZFactor) = (float)0xffff / 16.0f;
			break; 
	}
#ifdef GSP_USE_TRIPLE_BUFFERRING
	sceGsSetDefDispEnv(&GspGlobal_ACCESS(gs_DE1) , (short)2 , (short)GspGlobal_ACCESS(Xsize) , (short)GspGlobal_ACCESS(Ysize) , (short)0 , (short)0 );
#else	
	if (GspGlobal_ACCESS(VideoMode) == GSP_VRES_x2_TripleAA)
	{
		sceGsSetDefDispEnv(&GspGlobal_ACCESS(gs_DE1) , (short)GspGlobal_ACCESS(BppMode) , (short)GspGlobal_ACCESS(Xsize) , (short)GspGlobal_ACCESS(Ysize)>>1 , (short)0 , (short)0 );
	}
	else
	{
		sceGsSetDefDispEnv(&GspGlobal_ACCESS(gs_DE1) , (short)GspGlobal_ACCESS(BppMode) , (short)GspGlobal_ACCESS(Xsize) , (short)GspGlobal_ACCESS(Ysize) , (short)0 , (short)0 );
	}
#endif
	{
		u_int RealSize, RealY, Remorf , OldTBP;
#define ALIGN_FBF(a) a = (a + 4093) & ~4093
#define ALIGN_TBF(a) a = (a + 127) & ~127
		OldTBP = GspGlobal_ACCESS(TBP);

		GspGlobal_ACCESS(Morfling)	= 0;
		Remorf = GspGlobal_ACCESS(Morfling) & ~31;
		GspGlobal_ACCESS(gs_DE2) = GspGlobal_ACCESS(gs_DE1);
		GspGlobal_ACCESS(FrameCounter) = 0;	
		GspGlobal_ACCESS(FBP1) = (GspGlobal_ACCESS(Xsize) * (Remorf) * (ColorBitNum >> 3)) /* / 2048 */;;
		ALIGN_FBF(GspGlobal_ACCESS(FBP1));
		
		RealY =  (GspGlobal_ACCESS(Ysize) + 63) & ~63;
		RealSize = RealY - Remorf * 2;
		RealSize *= GspGlobal_ACCESS(Xsize);
		RealSize *= ColorBitNum >> 3;
		GspGlobal_ACCESS(FBP2) = RealSize;
		ALIGN_FBF(GspGlobal_ACCESS(FBP2));
		/* align FBP2 */
		GspGlobal_ACCESS(ZBP) = GspGlobal_ACCESS(FBP2) + RealSize;
		ALIGN_FBF(GspGlobal_ACCESS(ZBP));
		GspGlobal_ACCESS(TBP) = GspGlobal_ACCESS(ZBP) + RealSize;
		ALIGN_FBF(GspGlobal_ACCESS(TBP));
		/* align TBP */
		GspGlobal_ACCESS(FBP1) >>= 13;
		GspGlobal_ACCESS(FBP2) >>= 13;
		
		GspGlobal_ACCESS(ZBP) >>= 13;
		GspGlobal_ACCESS(TBP) >>= 13;
		GspGlobal_ACCESS(TFBP1) = GspGlobal_ACCESS(FBP1);
		GspGlobal_ACCESS(TFBP2) = GspGlobal_ACCESS(FBP2) + GspGlobal_ACCESS(FBP1);
		GspGlobal_ACCESS(FBP2)-= GspGlobal_ACCESS(FBP1);
		GspGlobal_ACCESS(ZBP)-= GspGlobal_ACCESS(FBP1);
		if (Remorf) GspGlobal_ACCESS(FBP1) = 512 - GspGlobal_ACCESS(FBP1);
		GspGlobal_ACCESS(BX0) = 2048-(GspGlobal_ACCESS(Xsize) >> 1L);
		GspGlobal_ACCESS(BY0) = 2048-(GspGlobal_ACCESS(Ysize) >> 1L);
		GSP_RemapInterfaceTexture(OldTBP);
	}

#ifdef GSP_USE_TRIPLE_BUFFERRING
	GspGlobal_ACCESS(gs_DE1).dispfb.FBP = 0;
	GspGlobal_ACCESS(gs_DE2).dispfb.FBP = 0;
	{
		extern u_int RealFBP2;
		RealFBP2 =  (GspGlobal_ACCESS(Ysize) + 63) & ~63;
		RealFBP2 *= GspGlobal_ACCESS(Xsize);
		RealFBP2 *= 16 >> 3;
		ALIGN_FBF(RealFBP2);
		RealFBP2 >>= 13;
		
		GspGlobal_ACCESS(gs_DE2).dispfb.FBP = (unsigned long)RealFBP2;
		
/*		DPUT_GS_DISPFB2(*(u_long *)&GspGlobal_ACCESS(gs_DE2).dispfb);  //DISPFB2
		DPUT_GS_DISPFB1(*(u_long *)&GspGlobal_ACCESS(gs_DE2).dispfb1);  //DISPFB1
*/
	}
	
#else
	GspGlobal_ACCESS(gs_DE1).dispfb.FBP = (unsigned long)GspGlobal_ACCESS(FBP1);
	GspGlobal_ACCESS(gs_DE2).dispfb.FBP = (unsigned long)GspGlobal_ACCESS(FBP2);
#endif
	if (SpecialMde640480)
	{
		GspGlobal_ACCESS(gs_DE1).display.DY += 16;
		GspGlobal_ACCESS(gs_DE2).display.DY += 16;
	}
	{
		u_int Remorf;
		Remorf = (GspGlobal_ACCESS(Morfling)) & ~31;
		GspGlobal_ACCESS(gs_DE1).display.DY += Remorf;
		GspGlobal_ACCESS(gs_DE2).display.DY += Remorf;
		GspGlobal_ACCESS(gs_DE1).dispfb.DBY += Remorf;
		GspGlobal_ACCESS(gs_DE2).dispfb.DBY += Remorf;
		GspGlobal_ACCESS(gs_DE1).display.DH -= Remorf * 2;
		GspGlobal_ACCESS(gs_DE2).display.DH -= Remorf * 2;
	}
	GspGlobal_ACCESS(gs_DE1).display.DX += GspGlobal_ACCESS(HRepos);
	GspGlobal_ACCESS(gs_DE2).display.DX += GspGlobal_ACCESS(HRepos);
	
	if (1)
	{
		// 2nd circuit
		*(u_long*)&GspGlobal_ACCESS(gs_DE1).dispfb1 = *(u_long*)&GspGlobal_ACCESS(gs_DE1).dispfb;
		*(u_long*)&GspGlobal_ACCESS(gs_DE1).display1 = *(u_long*)&GspGlobal_ACCESS(gs_DE1).display;
		

		GspGlobal_ACCESS(gs_DE1).dispfb.DBY ++; // dispfb2 has DBY field of 1
		GspGlobal_ACCESS(gs_DE1).display.DH = (GspGlobal_ACCESS(gs_DE1).display.DH)-1;
		GspGlobal_ACCESS(gs_DE1).display.DX = GspGlobal_ACCESS(gs_DE1).display.DX +2; // display2 has magh/2
		GspGlobal_ACCESS(gs_DE1).pmode.ALP = 0x80;
		GspGlobal_ACCESS(gs_DE1).pmode.EN1 = 1;
		GspGlobal_ACCESS(gs_DE1).pmode.EN2 = 1;
		// 2nd circuit
		*(u_long*)&GspGlobal_ACCESS(gs_DE2).dispfb1 = *(u_long*)&GspGlobal_ACCESS(gs_DE2).dispfb;
		*(u_long*)&GspGlobal_ACCESS(gs_DE2).display1 = *(u_long*)&GspGlobal_ACCESS(gs_DE2).display;

		GspGlobal_ACCESS(gs_DE2).dispfb.DBY ++; // dispfb2 has DBY field of 1
		GspGlobal_ACCESS(gs_DE2).display.DH = (GspGlobal_ACCESS(gs_DE2).display.DH)-1;
		GspGlobal_ACCESS(gs_DE2).display.DX = GspGlobal_ACCESS(gs_DE2).display.DX +2; // display2 has magh/2
		GspGlobal_ACCESS(gs_DE2).pmode.ALP = 0x80;
		GspGlobal_ACCESS(gs_DE2).pmode.EN1 = 1;
		GspGlobal_ACCESS(gs_DE2).pmode.EN2 = 1;
		
	}
	
	Gsp_InitVU1Matrix_Extention();
	
	GspGlobal_ACCESS(f2DFFX_A2D) = 1.0000f;
	GspGlobal_ACCESS(f2DFFY_A2D) = 1.1530f;
	if ((GspGlobal_ACCESS(Status) & GSP_Status_PAL_On) | GspGlobal_ACCESS(ModePEnable))
	{
		GspGlobal_ACCESS(f2DFFX_A2D) = 1.0000f;
		GspGlobal_ACCESS(f2DFFY_A2D) = 0.9835f;
	}
	
	STR_f_YFactor = 1.0f/(1.0f + (GspGlobal_ACCESS(f2DFFY_A2D) - 1.0f) / 1.0f);
	
	

	STR_f_YOffset = GspGlobal_ACCESS(Ysize) * (1.0f - STR_f_YFactor) / 1.8f;
	
	
	GspGlobal_ACCESS(stHSP).XSize = GspGlobal_ACCESS(Xsize);
	GspGlobal_ACCESS(stHSP).YSize = GspGlobal_ACCESS(Ysize);
	GspGlobal_ACCESS(stHSP).XPos = 0;//(GspGlobal_ACCESS(Xsize)>>1) - (GspGlobal_ACCESS(stHSP).XSize>>1);
	GspGlobal_ACCESS(stHSP).YPos = 0;//GspGlobal_ACCESS(Ysize) - (GspGlobal_ACCESS(Morfling) & ~31) - GspGlobal_ACCESS(stHSP).YSize;
		
	GSP_SetVideoCRTC(0 , 0);
	if (bNeedFlip) Gsp_FlipFB();
	DisplayRights();
}
void Gsp_FlipFB()
{
#ifdef GSP_USE_TRIPLE_BUFFERRING
	extern void GSP_ArtisanalFlushAll();
	Gsp_FloatingFlip_START();
	GSP_ArtisanalFlushAll();
	return;
#endif
	Gsp_Flush(FLUSH_ALL);
	GSP_SetVideoCRTC(GSP_IsFrameODD() , 1);
	GspGlobal_ACCESS(LastVBlankCounter) = VBlankCounter;
	if ((GspGlobal_ACCESS(ZBppMode) == 2) && (GspGlobal_ACCESS(BppMode) != 2))
		GspGlobal_ACCESS(CUR_REG_ZBUF) = /*ZBP*/((unsigned long)GspGlobal_ACCESS(ZBP) << 0L) | /*OPP*/((unsigned long)10 << 24L) | /*ZMSK*/(0L << 32L);
	else
		GspGlobal_ACCESS(CUR_REG_ZBUF) = /*ZBP*/((unsigned long)GspGlobal_ACCESS(ZBP) << 0L) | /*OPP*/((unsigned long)GspGlobal_ACCESS(ZBppMode) << 24L) | /*ZMSK*/(0L << 32L);
	Gsp_Flush(FLUSH_ALL);
	GSP_SetRegister(GSP_ZBUF_1 	, GspGlobal_ACCESS(CUR_REG_ZBUF));
	GSP_SetRegister(GSP_ZBUF_2 	, GspGlobal_ACCESS(CUR_REG_ZBUF));
	GSP_SetRegister(GSP_XYOFFSET_1 	, /*X*/((2048L - ((unsigned long)GspGlobal_ACCESS(Xsize) >> 1L)) << (0L + 4L)) | /*Y*/((2048L - ((unsigned long)GspGlobal_ACCESS(Ysize) >> 1L)) << (32L + 4L)));
	GSP_SetRegister(GSP_XYOFFSET_2 	, /*X*/((2048L - ((unsigned long)GspGlobal_ACCESS(Xsize) >> 1L)) << (0L + 4L)) | /*Y*/((2048L - ((unsigned long)GspGlobal_ACCESS(Ysize) >> 1L)) << (32L + 4L)));
	GSP_SetRegister(GSP_PRMODECONT 	, 0L);
	GSP_SetRegister(GSP_PRMODE 	, (1L<<6L) | (1L<<3L));
	GSP_SetRegister(GSP_TEST_1 	, ((1L << 16L) | (2L << 17L)));
	GSP_SetRegister(GSP_TEST_2 	, ((1L << 16L) | (2L << 17L)));
	GSP_SetRegister(GSP_PABE 	, 0L);
	GSP_SetRegister(GSP_ALPHA_1 	, GST_ADD);
	GSP_SetRegister(GSP_ALPHA_2 	, GST_ADD);
	{
		u_int Remorf;
		Remorf = GspGlobal_ACCESS(Morfling) & ~31;
		GSP_SetRegister(GSP_SCISSOR_1 	, (0L << 0L) | (((unsigned long)GspGlobal_ACCESS(Xsize) - 1L) << 16L) | (((unsigned long)Remorf) << 32L) | (((unsigned long)GspGlobal_ACCESS(Ysize) - (unsigned long)Remorf - 1L) << 48L));
		GSP_SetRegister(GSP_SCISSOR_2 	, (0L << 0L) | (((unsigned long)GspGlobal_ACCESS(Xsize) - 1L) << 16L) | (((unsigned long)Remorf) << 32L) | (((unsigned long)GspGlobal_ACCESS(Ysize) - (unsigned long)Remorf - 1L) << 48L));
	}
	GSP_SetRegister(GSP_SCANMSK 	, 0L);
	GSP_SetRegister(GSP_TEXA 	, (0L<<0L)/*Ta0*/ | (0L<<15L) | (0x80L<<32L)/*Ta1*/);
	
	if (GspGlobal_ACCESS(BppMode) == 2)
	{
		GSP_SetRegister(GSP_DTHE 	, 1L);
		GSP_SetRegister(GSP_DIMX 	, 0x0617435217065243L);
	}
	else
	{
		GSP_SetRegister(GSP_DTHE 	, 0L);
	}
	if (!(GSP_IsFrameODD()))
	{ // Select buffer 2 
		GspGlobal_ACCESS(CUR_REG_FRAME) = /*FBP*/((unsigned long)GspGlobal_ACCESS(FBP2) << 0L) | /*FBW*/(((unsigned long)GspGlobal_ACCESS(Xsize)>> 6L) << 16L) | /*OPP*/((unsigned long)(GspGlobal_ACCESS(BppMode)) << 24L) | /*FBMSK*/(0L << 32L);
		GSP_SetRegister(GSP_FRAME_1 	, GspGlobal_ACCESS(CUR_REG_FRAME));
		GSP_SetRegister(GSP_FRAME_2 	, GspGlobal_ACCESS(CUR_REG_FRAME));
	} else 
	{ /* Select Buffer 1 */
		GspGlobal_ACCESS(CUR_REG_FRAME) = /*FBP*/((unsigned long)GspGlobal_ACCESS(FBP1) << 0L) | /*FBW*/(((unsigned long)GspGlobal_ACCESS(Xsize)>> 6L) << 16L) | /*OPP*/((unsigned long)(GspGlobal_ACCESS(BppMode)) << 24L) | /*FBMSK*/(0L << 32L);
		GSP_SetRegister(GSP_FRAME_1 	, GspGlobal_ACCESS(CUR_REG_FRAME));
		GSP_SetRegister(GSP_FRAME_2 	, GspGlobal_ACCESS(CUR_REG_FRAME));
	}
	Gsp_SetGSRegisters();
	{
#ifdef GSP_PS2_BENCH
		if ((GspGlobal_ACCESS(Status) & GSP_Status_Show_Depth)||ShowNormals)
		{
			Gsp_DrawRectangle(0.0f, 0.0f, 1.1f , 1.1f  , 0  , 0 , GST_COPY);
		} else
#endif			
		Gsp_DrawRectangle(0.0f, 0.0f, 1.1f , 1.1f  , (GspGlobal_ACCESS(ulBackColor) | 0x80000000) , (GspGlobal_ACCESS(ulBackColor) | 0x80000000) , GST_COPY);
	}
	Gsp_Flush(FLUSH_ALL);
	GspGlobal_ACCESS(FrameCounter) ++;
	Gsp_DrawBuf_JustAfterFlip();
}




#ifdef __cplusplus
}
#endif

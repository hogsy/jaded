
#include "GXI_tex.h"
#include "GXI_font.h"
#include "GXI_bench.h"
#include "GXI_rasters.h"
#include "GXI_render.h"
#include "BASe/MEMory/MEM.h"


#ifndef _FINAL_     

#define Raster_FilterEngine 1
#define Raster_FilterDisplay 2
#define Raster_FilterGlobal 4
#define Raster_FilterSkip 8
#define Raster_FilterIAUsr 8
#define Raster_FilterALL 7

#define DEFINE_RASTER(a,b,c,d)  
#include "BAse/BENch/BENch_RasterDef.h"
#undef DEFINE_RASTER

u32 RastersPreviousStatus = 0;
int Rasters[GSP_NRaster];
int Rasters_Max[GSP_NRaster];
int Rasters_MaxEnble = 0;

int Rasters1[GSP_NRaster];
float RastersSmooth[GSP_NRaster];
int RastersStarts[GSP_NRaster];
int RastersStarts1[GSP_NRaster];
int RasterFilter = 0;
int RasterMode = 0;
int RasterDetailMum = 10;
int RasterFrameCounter = 0;
static float fLastTime = 0.0f;
static float AverageRasters = 1.0f;
ULONG MEM_gul_DMARequests = 0;

typedef struct 
{
	unsigned char Name[32];
	unsigned int FilterType;
}
RasterDescriptor;

#define RASVSIZE 0.02f
#define RASVPOS YPos
#define HRECENTER 0.7f 
#define AR_SMOOTH 0.98f
#define AR_SMOOTHQ 0.30f

#define DEFINE_RASTER(a,b,c,d)  b,c,
RasterDescriptor RasterDescritpors[GXI_NRaster+2] =
{
#include "BAse/BENch/BENch_RasterDef.h"
};
#undef DEFINE_RASTER
	
void GXI_DrawGlobalRasters(char *Text , float FrameNumb , float YPos , float ARS, u_int TheColor)
{
	float fBase , fSize ;
	YPos -= (float)WIDE_SCREEN_ADJUST / (float)GXI_Global_ACCESS(Ysize);
	YPos = 1.0f - YPos;
	
	if (FrameNumb > 8.f) FrameNumb = 8.f;
	if (FrameNumb < 0.f) FrameNumb = 0.f;
	if (FrameNumb > 1.0f)
	fSize = HRECENTER * 0.5f;
	else
	fSize = HRECENTER;
	fBase = (1.0f - HRECENTER) * 0.5f;
	{
		char String[16];
		if (GXI_Global_ACCESS(Status) & GC_Status_Raster_Prop_On)
			sprintf(String, "%.0f%%", ARS * 100.0f);
		else
			sprintf(String, "%.3f", ARS);
		GXI_DrawText((u_int)((1.0f - (fBase * 0.9)) * GXI_Global_ACCESS(Xsize)),(u_int)(RASVPOS * GXI_Global_ACCESS(Ysize)) - 8,String,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);
		if (Text && (Text[0]))
		{
			//if(strlen(Text) > 7) Text[7] = 0;
			GXI_DrawText(8,(u_int)(RASVPOS * GXI_Global_ACCESS(Ysize)) - 8,Text,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);
		}
	}
	YPos -= (float)4.0f / (float)GXI_Global_ACCESS(Ysize);
	while (FrameNumb > 1.0f)
	{
		FrameNumb -= 1.0f;
		GXI_DrawShadowedRectangle(fBase,RASVPOS,fSize,RASVSIZE,TheColor);
		TheColor ^= 0x00ffFFFF;
		fBase += fSize;
		fSize *= 0.5f;
	}
	GXI_DrawShadowedRectangle(fBase,RASVPOS,fSize * FrameNumb,RASVSIZE,TheColor);
	GXI_DrawShadowedRectangle(fBase,RASVPOS - RASVSIZE * 0.33333f,0.009f,RASVSIZE * 1.5f,0xffff00ff);
	GXI_DrawShadowedRectangle(fBase + fSize,RASVPOS - RASVSIZE * 0.3333f,0.009f,RASVSIZE * 1.5f,0xffff00ff);
	{
		u_int SMC , Color , Max;
		Max = (u_int)(FrameNumb * 10.0f + 2.0f);
		if (Max > 10) Max = 10;
		for (SMC = 1 ; SMC < Max ; SMC ++)
		{	
			Color = 0xffffff;
			if (SMC == 5) Color = 0xff0000;
			GXI_DrawRectangle(fBase + fSize * ((float)SMC * 0.1f),RASVPOS,0.002f,RASVSIZE,Color,MAT_Cc_Op_Copy);
		}
	}
}

extern void MEM_ComputeMemoryHoleInfo(ULONG * , ULONG *, BOOL);
#ifdef GC_ENABLE_SOUND_PROFILER
extern void gcSND_ProfilerDisplay(f32);
#endif // GC_ENABLE_SOUND_PROFILER
void GXI_Rasters_PrintSize(u32 Size,char *String)
{
	if (Size > 1024 * 1024)
	{
		// MEGA
		sprintf(String , "%.3f Megs" , (float)Size / (float)(1024.0f * 1024.0f));
	} else
	if (Size > 1024)
	{
		// KILOS
		sprintf(String , "%d Ko" , Size >> 10);
	} else
	{
		// octets
		sprintf(String , "%d octets" , Size);
	} 
}
#ifdef GC_BENCHGRAPH
extern MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#endif

extern int MEM_gi_RasterState;
extern void MEM_RasterSetRequest(int rq);

void GXI_Rasters()
{
	int fDelta;
	ULONG		ul_NumHoles, ul_HoleSize;
	char txt[64];

	float ScreenFreq_Over_CPUFreq,Freq;
	
	if (GXI_Global_ACCESS(Status) & GC_Status_PAL_On)
		Freq = 50.f;
	else
		Freq = 60.f;
	fDelta = (((float)OSGetTick()) - fLastTime) ;

	ScreenFreq_Over_CPUFreq = Freq / OS_TIMER_CLOCK;
	RasterFrameCounter ++;
	MEM_ComputeMemoryHoleInfo(&ul_NumHoles, &ul_HoleSize, FALSE);
#ifdef GC_ENABLE_SOUND_PROFILER	
	gcSND_ProfilerDisplay(GXI_Global_ACCESS(Ysize));
#endif // GC_ENABLE_SOUND_PROFILER	
	if(MEM_gi_RasterState) MEM_RasterSetRequest(MEM_gi_RasterState);
	
	// Warning free memory < 500 ko
#ifdef MEM_MEASURES
	if (MEM_gst_MemoryInfo.ul_CurrentFree>>10 < 512)
    {
    	char 		asz_Log[512];
		sprintf(asz_Log,"WARNING:%dKo of free mem", MEM_gst_MemoryInfo.ul_CurrentFree>>10 );
    	GXI_DrawText(32,32, asz_Log, 0xff2020ff, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
    }
#endif
	
	if (RastersPreviousStatus != GXI_Global_ACCESS(Status))
	{
		RastersPreviousStatus = GXI_Global_ACCESS(Status);
		if (Rasters_MaxEnble)
			L_memset(Rasters_Max , 0 , GSP_NRaster * 4 );
	}
	
	if (GXI_Global_ACCESS(Status) & GC_Status_Raster_On) 
	{
		float DetailOo;
		DetailOo = ScreenFreq_Over_CPUFreq;

		if (GXI_Global_ACCESS(Status) & GC_Status_Raster_Details_On)
		{
			u_int RC,RCS;
			float Base;
			u_int Sorted[GSP_NRaster];
			for (RC = 0 ; RC < GSP_NRaster ; RC++) 
				Sorted[RC] = RC;
			for (RC = 0 ; RC < GSP_NRaster - 1 ; RC++) 
			for (RCS = RC + 1 ; RCS < GSP_NRaster; RCS++) 
			{
				if (RastersSmooth[Sorted[RC]] < RastersSmooth[Sorted[RCS]])
				{
					u_int SWAP;
					SWAP = Sorted[RC];
					Sorted[RC] = Sorted[RCS];
					Sorted[RCS] =  SWAP;
				}
			}

			Base = 0.95f ;
			for (RC = 0 ; RC < GSP_NRaster ; RC++) 
			{
				u_int Color;
				u_int Filter;
				Filter = RasterFilter;
				RCS = (RasterMode&(1<<16))? RC : Sorted[RC];
				if(fAbs(RastersSmooth[RCS]-((float)Rasters[RCS] * DetailOo)) < 0.1f)
					RastersSmooth[RCS] = RastersSmooth[RCS] * AR_SMOOTH + (float)Rasters[RCS] * DetailOo * (1.0f - AR_SMOOTH);
				else
					RastersSmooth[RCS] = RastersSmooth[RCS] * AR_SMOOTHQ + (float)Rasters[RCS] * DetailOo * (1.0f - AR_SMOOTHQ);

				if ((RastersSmooth[RCS] > 0.002f)  && (RasterDescritpors[RCS].FilterType & Filter))
				{
					Base -= 20.0f / (float)GXI_Global_ACCESS(Ysize);
					
					Color = 0xf0808080;
					if ((RCS + 1) & 1) Color |= 0x40;
					if ((RCS + 1) & 2) Color |= 0x4000;
					if ((RCS + 1) & 4) Color |= 0x400000;
					if((RC < RasterDetailMum) || (RasterMode&(1<<16)))
					{
						sprintf(txt,"% 2d %s", RCS, RasterDescritpors[RCS].Name);
						if (Rasters_MaxEnble)
						{
							Rasters_Max[RCS] = lMax(Rasters_Max[RCS] , Rasters[RCS]) ; 
							GXI_DrawGlobalRasters(txt, (float)Rasters_Max[RCS] * DetailOo, Base , Rasters_Max[RCS] * DetailOo, Color);
						}
						else
							GXI_DrawGlobalRasters(txt, (float)Rasters[RCS] * DetailOo, Base , RastersSmooth[RCS], Color);
					}
				}
			}
			
			//je force l'affichage total = total(eng) + total(display)
			if(1)// (((RasterMode & 0xff) == 0) &&  (GXI_Global_ACCESS(Status) & GC_Status_Raster_Details_On))
			{
				float Local;
				AverageRasters = AverageRasters * AR_SMOOTH + (float)fDelta  * ScreenFreq_Over_CPUFreq * (1.0f - AR_SMOOTH);
				Local = (float)fDelta  * ScreenFreq_Over_CPUFreq;
				Local = ((float)Rasters[18]+(float)Rasters[19])  * ScreenFreq_Over_CPUFreq;
//				AverageRasters = AverageRasters * AR_SMOOTH + (float)Local  * ScreenFreq_Over_CPUFreq * (1.0f - AR_SMOOTH);
				
				GXI_DrawGlobalRasters("" , (float)Local, 0.95f    , Local/*AverageRasters*/ , 0xf08080FF);
			} else
			{
				AverageRasters = AverageRasters * AR_SMOOTH + (float)fDelta  * ScreenFreq_Over_CPUFreq * (1.0f - AR_SMOOTH);
				GXI_DrawGlobalRasters("" , (float)fDelta  * ScreenFreq_Over_CPUFreq, 0.95f    , AverageRasters , 0xf08080FF);
			}
			for (RC = 0 ; RC < GSP_NRaster ; RC++) 
			{
				Rasters[RC] = 0;
				Rasters1[RC] = 0;
			}
			
		} else
		{
			if(fAbs(AverageRasters-((float)fDelta  * ScreenFreq_Over_CPUFreq)) < 0.1f)
				AverageRasters = AverageRasters * AR_SMOOTH + (float)fDelta  * ScreenFreq_Over_CPUFreq * (1.0f - AR_SMOOTH);
			else
				AverageRasters = AverageRasters * AR_SMOOTHQ + (float)fDelta  * ScreenFreq_Over_CPUFreq * (1.0f - AR_SMOOTHQ);
			GXI_DrawGlobalRasters("" , (float)fDelta  * ScreenFreq_Over_CPUFreq, 0.95f   , AverageRasters , 0xf08080FF);
		}
		//fLastTime = (float)OSGetTick() ;
	}
	
#ifdef GC_BENCHGRAPH
	if (GXI_Global_ACCESS(ulDisplayTnum))
	{
		char String[256];
		char StringSize[256];
		char StringSize2[256];
		u32  RealFifoSize;
		f32 YPos;
		extern GXFifoObj*  	  DefaultFifoObj;
		{
			u32 RP,WP;
			static u32 LastWP;
			GXBool Dum,Used;
			GXGetFifoPtrs(DefaultFifoObj,(void **)&RP,(void **)&WP);
			if (LastWP > WP) 
			{
				RealFifoSize = LastWP  - WP;
				RealFifoSize = GXGetFifoSize(DefaultFifoObj) - RealFifoSize;
			}
			else RealFifoSize = WP - LastWP;
			
			LastWP = WP;
			#if !defined(_RVL)
			GXGetFifoStatus(DefaultFifoObj,&Dum,&Dum,&Dum,&Dum,&Dum,&Used );
			if (Used)
			#endif
				RealFifoSize = GXGetFifoSize(DefaultFifoObj);

		}
		
		YPos = GXI_Global_ACCESS(Ysize) - WIDE_SCREEN_ADJUST - 14;
		sprintf(String, "Number Of Triangles %d", GXI_Global_ACCESS(ulNumberOfTri));
		GXI_DrawText(GXI_Global_ACCESS(Xsize)/2,YPos,String,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);	
		YPos -= 14;
		GXI_Rasters_PrintSize(RealFifoSize,StringSize);
		sprintf(String, "FIFO size %s", StringSize);
		GXI_DrawText(GXI_Global_ACCESS(Xsize)/2,YPos,String,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);	
		YPos -= 14;
		{
			extern const u32 VERTEX_SPACE_SIZE;
			GXI_Rasters_PrintSize(GXI_Global_ACCESS(ulVertxArraySize),StringSize);
			GXI_Rasters_PrintSize(VERTEX_SPACE_SIZE,StringSize2);
			sprintf(String, "Vrtx array size %s / %s ", StringSize , StringSize2);
			if (GXI_Global_ACCESS(ulVertxArraySize) > VERTEX_SPACE_SIZE)
				GXI_DrawText(GXI_Global_ACCESS(Xsize)/2,YPos,String,0xff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);	
			else
				GXI_DrawText(GXI_Global_ACCESS(Xsize)/2,YPos,String,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);	
		}
		YPos -= 14;
		GXI_Rasters_PrintSize(GXI_Global_ACCESS(ulGEOMEM),StringSize);
		sprintf(String, "GEO Mem %s", StringSize);
		GXI_DrawText(GXI_Global_ACCESS(Xsize)/2,YPos,String,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);	
		YPos -= 14;
		GXI_Rasters_PrintSize(GXI_Global_ACCESS(ulTEXMEM),StringSize);
		sprintf(String, "TEX Mem %s", StringSize);
		GXI_DrawText(GXI_Global_ACCESS(Xsize)/2,YPos,String,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);	
		YPos -= 14;
		GXI_Rasters_PrintSize(GXI_Global_ACCESS(ulDisplayLST),StringSize);
		sprintf(String, "DLS Mem %s", StringSize);
		GXI_DrawText(GXI_Global_ACCESS(Xsize)/2,YPos,String,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);	
		YPos -= 14;
		GXI_Rasters_PrintSize(MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated,StringSize);
		sprintf(String, "ALL Mem %s", StringSize);
		GXI_DrawText(GXI_Global_ACCESS(Xsize)/2,YPos,String,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);	
		
		GXI_Global_ACCESS(ulVertxArraySize) = 0;
		GXI_Global_ACCESS(ulFIFOSize) = 0;
		GXI_Global_ACCESS(ulNumberOfTri) = 0;
			
		
	}
#endif
	
	MEM_gul_DMARequests = 0;
	
	fLastTime = (float)OSGetTick() ;
}


#endif

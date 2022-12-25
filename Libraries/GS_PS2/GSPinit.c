/*$T GSPinit.c GC! 1.081 05/04/00 15:08:05 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include <eeregs.h>
#include <libgraph.h>
#include <devvif1.h>
#include <devvu1.h>
#include <devvu0.h>
#include <devgif.h>

#include "Gsp.h"
#include "GSPinit.h"
#include "GSPtex.h"
#include "GSP_Video.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"

#include "Gsp_Bench.h"
#include "IOP/ee/eeRPC_Debug.h"
#include "GDInterface/GDInterface.h"

#define DEFINE_RASTER(a,b,c,d)  
#include "BAse/BENch/BENch_RasterDef.h"
#undef DEFINE_RASTER


extern u_int volatile VBlankCounter;
u32 LocalVblankCounter = -1;
extern u_int volatile VBlankOdd;
extern __declspec(scratchpad) u_int volatile gsulSyncroPath;
extern int MEM_gi_RasterState;
extern void MEM_RasterSetRequest(int);
extern void scePcStop(void);
extern u_int VU1_Matrix   __attribute__((section(".vudata")));
extern u_int VU1_Matrix_B __attribute__((section(".vudata")));
extern u_int VU1_Matrix_E  __attribute__((section(".vudata")));
extern u_int My_Command_Vector __attribute__((section(".vudata")));
	extern u_int ulVideoScreentValid;
BOOL GSP_gb_NoFloatingFlip=FALSE;
extern u_int bTakeSnapshot;
extern u32 ShowNormals;
extern ULONG GEO_MemRasterPs2;

typedef struct 
{
	unsigned char Name[64];
	u_int FilterType;
}
RasterDescriptor;

#define Raster_FilterEngine 1
#define Raster_FilterDisplay 2
#define Raster_FilterGlobal 4
#define Raster_FilterIAUsr 8

#define Raster_FilterALL 7

#define DEFINE_RASTER(a,b,c,d)  b,d,
static RasterDescriptor RasterDescritpors[GSP_NRaster] =
{
#include "BAse/BENch/BENch_RasterDef.h"
};
#undef DEFINE_RASTER


/*
 * on a donc trame = ino + eng + dis + snd + txp
 * avec 
 *      eng = col + rec + ai + diveng
 */
int Rasters[GSP_NRaster];
int Rasters_Max[GSP_NRaster];
int Rasters_MaxEnble = 0;

int Rasters1[GSP_NRaster];
float RastersSmooth[GSP_NRaster];
int RastersStarts[GSP_NRaster];
int RastersStarts1[GSP_NRaster];
	
int ColorCost_GlobalTime_IA;
int ColorCost_MaxTime_IA;

int ColorCost_GlobalTime_GRAPHIC;
int ColorCost_MaxTime_GRAPHIC;

void GSP_SetRasterName(u32 Number , char *String)
{
	char *pDest;
	pDest = RasterDescritpors[Number].Name;
	*(pDest++) = ' ';
	while (*String) *(pDest++) = *(String++);
	*(pDest++) = 0;
}

/*$4
 ***********************************************************************************************************************
    Private function prototype
 ***********************************************************************************************************************
 */
void GSP_pst_InitDevice(void);
void GSP_DrawShadowedRectangle(float x, float y, float w, float h, u_int C);
void GSP_DrawGlobalRasters(char *Text , float FrameNumb , float YPos , float ARS, u_int TheColor);
void Gsp_InitStack();
u_int Gsp_GetStackSize();
void Gsp_DisplayStackInfo();
void GS_GetZBuffer_NoSpr(u_int *Address);
void Gsp_GiveACrashAdressName(ULONG CrashAddress , unsigned char *TEXT);
void Gsp_InfoScreen(unsigned char *TEXT , ULONG TExtCOLOR , ULONG BakcCOLOR);
void Gsp_Crash(ULONG CrashAddress , unsigned char *TEXT );
void GSP_BeginWorldLoad();
void GSP_EndWorldLoad();
void Gsp_Before2D();
void Gsp_Begin3DObjectWhileIn2D();
void Gsp_End3DObjectWhileIn2D();
void GSP_SetDCPixelFormat(HDC _hDC);
void GSP_SetupRC(GSP_tdst_SpecificData *_pst_SD);
void Gsp_DepthBlur(int OnOff , float ZStart ,float ZEnd);


/*$4
 ***********************************************************************************************************************
    Public Function
 ***********************************************************************************************************************
 */
ULONG *RamZBuffer = NULL;

/*
 =======================================================================================================================
    Aim:    Create a device
 =======================================================================================================================
 */
GSP_tdst_SpecificData *GSP_pst_CreateDevice(void)
{
	
	MEMpro_StartMemRaster();
	if (RamZBuffer == NULL) RamZBuffer = (float *)MEM_p_AllocAlign(320 * 256 * 4,16);
	MEMpro_StopMemRaster(MEMpro_Id_GSP);
	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Init Camera infos
 =======================================================================================================================
 */

void GSP_pst_InitDevice(void)
{
	GDI_gpst_CurDD->st_Device.l_Width = GspGlobal_ACCESS(Xsize); 
   	GDI_gpst_CurDD ->st_Device.l_Height = 256; 
}

/*
 =======================================================================================================================
    Aim:    Destroy a device
 =======================================================================================================================
 */
void GSP_DestroyDevice(void *_pst_SD)
{
}

/*
 =======================================================================================================================
 Aim: Close OpenGL display 
 =======================================================================================================================
 */
HRESULT GSP_l_Close(void* rien)
{
	return S_OK;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GSP_l_Init(void* rien)
{
	return S_OK;
}

/*
 =======================================================================================================================
    Aim:    Adapt OpenGL display to window
 =======================================================================================================================
 */
LONG GSP_l_ReadaptDisplay(HWND a0, void *a1)
{
	return S_OK;
}

/*
 =======================================================================================================================
 Aim: Flip buffer 
 =======================================================================================================================
 */
#define RASVSIZE 0.02f
#define RASVPOS YPos
#define HRECENTER 0.8f 
#define AR_SMOOTH 0.95f

void GSP_DrawShadowedRectangle(float x, float y, float w, float h, u_int C)
{
     Gsp_DrawRectangle(x + 0.005f,y + 0.01f,w,h,0x404040,0x404040,GST_SUB);
     Gsp_DrawRectangle(x,y,w,h,C,C,GST_COPY);
}
static float AverageRasters = 1.0f;
extern void GSP_ReadPixel(struct GDI_tdst_DisplayData_ *_pst_DD, MATH_tdst_Vector  *_pst_Pos );
extern float GFXF_f_depth;
extern float	fGlobalAcc;

void GSP_DrawGlobalRasters(char *Text , float FrameNumb , float YPos , float ARS, u_int TheColor)
{
	float fBase , fSize ;
	YPos -= (float)GspGlobal_ACCESS(Morfling) / (float)GspGlobal_ACCESS(Ysize);
	
	if (FrameNumb > 8.f) FrameNumb = 8.f;
	if (FrameNumb < 0.f) FrameNumb = 0.f;
	fSize = HRECENTER * 0.5f;
	fBase = (1.0f - HRECENTER) * 0.5f;
	fBase += fSize*0.4;
	fSize *= 0.85;
	{
		char String[16];
		if (GspGlobal_ACCESS(Status) & GSP_Status_Raster_Prop_On)
			sprintf(String, "%.0f%%", ARS * 100.0f);
		else
			sprintf(String, "%.3f", ARS);
		GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - 64,(u_int)(RASVPOS * GspGlobal_ACCESS(Ysize)) - 8,String,0xffffff,0xffffff,GST_COPY,GST_SUB);
		if (Text && (Text[0]))
		{
			Text[22] = 0;
			GSP_DrawTExt(8,(u_int)(RASVPOS * GspGlobal_ACCESS(Ysize)) - 8,Text,0xffffff,0xffffff,GST_ALPHAFIX | (0x40L << 32L),GST_SUB);
		}
	}
	while (FrameNumb > 1.0f)
	{
		FrameNumb -= 1.0f;
		GSP_DrawShadowedRectangle(fBase,RASVPOS,fSize,RASVSIZE,TheColor);
		TheColor ^= 0x00ffFFFF;
		fBase += fSize;
		fSize *= 0.5f;
	}
	GSP_DrawShadowedRectangle(fBase,RASVPOS,fSize * FrameNumb,RASVSIZE,TheColor);
	GSP_DrawShadowedRectangle(fBase,RASVPOS - RASVSIZE * 0.33333f,0.009f,RASVSIZE * 1.5f,0xffff00ff);
	GSP_DrawShadowedRectangle(fBase + fSize,RASVPOS - RASVSIZE * 0.3333f,0.009f,RASVSIZE * 1.5f,0xffff00ff);
	{
		u_int SMC , Color , Max;
		Max = (u_int)(FrameNumb * 10.0f + 2.0f);
		if (Max > 10) Max = 10;
		for (SMC = 1 ; SMC < Max ; SMC ++)
		{	
			Color = 0xffffff;
			if (SMC == 5) Color = 0xff0000;
			Gsp_DrawRectangle(fBase + fSize * ((float)SMC * 0.1f),RASVPOS,0.002f,RASVSIZE,Color,Color,GST_COPY);
		}
	}
	
}

typedef struct GSP_tdstRaster_
{
    float f_Val;
    float f_Relative;
    char  asz_Name[8];
} GSP_tdstRaster;
#define _HRECENTER  0.95f
void GSP_DrawGlobalRastersArrays(void*Array, int Size, float YPos, u_int TheColor)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	float	fBase, fSize;
	char	String[32];
	u_int	SMC, Color;
	int j;
	GSP_tdstRaster*ast_Array;
	/*~~~~~~~~~~~~~~~~~~~~*/

	YPos -= (float) GspGlobal_ACCESS(Morfling) / (float) GspGlobal_ACCESS(Ysize);
	
	ast_Array = (GSP_tdstRaster*)Array;
	

	fBase = (1.0f - _HRECENTER) * 0.5f;

    for(j=0; j<Size; j++)
	{
	    fSize = _HRECENTER*ast_Array[j].f_Relative;

        if(ast_Array[j].f_Val && ast_Array[j].asz_Name[0])
        {
    		sprintf(String, "%s %6.3f",ast_Array[j].asz_Name, ast_Array[j].f_Val);
            GSP_DrawTExt
    		(
    			lFloatToLong(fBase*fLongToFloat(GspGlobal_ACCESS(Xsize))),
    			lFloatToLong(RASVPOS * fLongToFloat(GspGlobal_ACCESS(Ysize))) - 16,
    			String,
    			0xffffff,
    			0x606060,
    			GST_ALPHAFIX | (0x40L << 32L),
    			GST_SUB
    		);
    	}

        if(ast_Array[j].f_Val)
        {
		    GSP_DrawShadowedRectangle(fBase, RASVPOS, fSize, RASVSIZE, TheColor);
		    TheColor ^= 0x00ffFFFF;
		    fBase += fSize;
		}
	}

    fBase = (1.0f - _HRECENTER) * 0.5f;
	GSP_DrawShadowedRectangle(fBase, RASVPOS - RASVSIZE * 0.33333f, 0.009f, RASVSIZE * 1.5f, 0xffff00ff);
	GSP_DrawShadowedRectangle(fBase + _HRECENTER, RASVPOS - RASVSIZE * 0.3333f, 0.009f, RASVSIZE * 1.5f, 0xffff00ff);

	for(SMC = 1; SMC < 10; SMC++)
	{
		Color = 0xffffff;
		if(SMC == 5) Color = 0xff0000;
		Gsp_DrawRectangle(fBase + _HRECENTER * ((float) SMC * 0.1f), RASVPOS, 0.002f, RASVSIZE, Color, Color, GST_COPY);
	}
}


int fLastTime;
int fLastTime1;
static float ZCenter2   = 0.0f;
extern u_int RasterMode;
extern u_int RasterDetailMum;
extern u_int RasterFilter;
extern float    TIM_f_Clock_TrueRead(void);
extern void Gsp_SheetCode();
#ifdef GSP_PS2_BENCH
extern u_int 	LoadTextureInterface;
extern u_int	n_Parralleles;
extern u_int	n_Omni;
extern u_int	n_Spot;
extern u_int	n_LightCalls;
extern Gsp_AE_ShowTexture();
#endif

#ifdef GSP_PS2_BENCH
u_int StackInit = 0;
u_int StackMax = 0x00080000;
u_int StackMaxSaved = 0x00080000;
u_int StackEngine = 0;
u_int StackEngineMax = 0;
u_int StackDisplay = 0;
u_int StackDisplayMax = 0;
u_int StackGO = 0;

void Gsp_InitStack()
{
	unsigned char *PP , *PPLAST;
	StackMaxSaved = 0x00080000;
	PP = (unsigned char *)(0x01F80000 + 0x00080000 - StackMaxSaved);
	while (*PP == 0xC0) PP++;
	PPLAST = (unsigned char *)GetSP();
	PPLAST -= 0x10;
	while (PP < PPLAST) *(PP++) = 0xd0;
}

u_int Gsp_GetStackSize()
{
	unsigned char *PP ;
	PP = (unsigned char *)0x01F80000 + 0x00080000 - StackMaxSaved;
	while (*PP == 0xC0) PP++;
	while (*PP == 0xD0) PP++;
	return GetSP() - (u_int)PP;
}

void Gsp_DisplayStackInfo()
{
	u_int Counter;
	unsigned char Name[256];
	if (!StackGO) return;
/*				stack	(RW)	: ORIGIN = 0x01F80000,	LENGTH = 0x00080000*/
	Counter = 0x00080000;
	for (Counter = 0 ; Counter < 0x00080000 ; Counter ++)
	{
		if (((unsigned char *)0x01F80000)[Counter] != 0xC0) break;
	}
	StackMax = (0x00080000 - Counter) - StackInit;
	sprintf(Name , "\nEstimation Stack Size:\n init = %X\n Max = %X\n Display = %X , max %X \n Engine = %X , max %X \n " , 
					StackInit , 
					StackMax , 
					StackDisplay , StackDisplayMax , StackEngine , StackEngineMax);
	GspGlobal_ACCESS(Status) |= GSP_Status_Console_On;
	GSP_ClsConsole();
	GSP_OutputConsole(Name);
}
#endif


extern u_int DISPLAYSTACKSPR;
extern u_int SavedStack;
u_int SaveSPRSTACK;
void GS_GetZBuffer_NoSpr(u_int *Address)
{
	gsulSyncroPath |= FLUSH_USER;
	GS_GetZBuffer(Address);
	gsulSyncroPath &= ~FLUSH_USER;
}


extern void SND_StreamInitModule();
extern void ENG_ReinitOneWorld();
extern void CAM_Init();
extern void AI_DuplicateInstance();
extern void BAS_bsort();
extern void asm_break();
extern void SPG_Modifier_Create();
extern void AI_EvalFunc_GFXDepthBlur();
extern void AI_EvalFunc_XMEN_SetSize();
extern void XMEN_OnOff();
int InfiniteCrash = 0;
void Gsp_GiveACrashAdressName(ULONG CrashAddress , unsigned char *TEXT)
{
	TEXT[0] = 0;

	if ((CrashAddress >= (u_int)ENG_ReinitOneWorld) && (CrashAddress < 	(u_int)SPG_Modifier_Create)) 			sprintf(TEXT , "(Engine library)");
	if ((CrashAddress >= (u_int)ENG_ReinitOneWorld) && (CrashAddress < 	(u_int)SPG_Modifier_Create)) 			sprintf(TEXT , "(Engine library)");
	if ((CrashAddress >= (u_int)sceGsResetGraph) && (CrashAddress < 	(u_int)powf)) 							sprintf(TEXT , "(Sony library)");
	if ((CrashAddress >= (u_int)Gsp_SyncVB) && (CrashAddress <= (u_int)GSP_GetTextureMemoryAvailable)) 			sprintf(TEXT , "(GSP library)");
	if ((CrashAddress >= (u_int)CAM_Init) && (CrashAddress <= (u_int)LIGHT_ul_Interpol2Colors)) 				sprintf(TEXT , "(GraphicDK library)");
	if ((CrashAddress >= (u_int)AI_DuplicateInstance) && (CrashAddress <= (u_int)AI_EvalFunc_GFXDepthBlur)) 	sprintf(TEXT , "(AIInterp library)");
	if ((CrashAddress >= (u_int)AI_EvalFunc_XMEN_SetSize) && (CrashAddress <= (u_int)XMEN_OnOff)) 				sprintf(TEXT , "(AIInterp library)");
	if ((CrashAddress >= (u_int)AI_EvalFunc_GFXDepthBlur) && (CrashAddress <= (u_int)AI_EvalFunc_XMEN_SetSize)) sprintf(TEXT , "(an AI compiled function)");
	if ((CrashAddress >= (u_int)BAS_bsort) && (CrashAddress <= (u_int)asm_break)) 								sprintf(TEXT , "(SDK Library)");
	if (InfiniteCrash)  sprintf(TEXT , "(infinite loop)" );
}

int gi_CrashY=0;
void Gsp_InfoScreen(unsigned char *TEXT , ULONG TExtCOLOR , ULONG BakcCOLOR)
{
	extern u32 GSP_BIGFADETOBLACK;
	GSP_BIGFADETOBLACK = 0x80;
	Gsp_Flush(FLUSH_ALL);
	GspGlobal_ACCESS(ulBackColor) = BakcCOLOR ; 
	GSP_FlushAllFloatingFlip();
	Gsp_FlipFB();
	Gsp_FlipFB();
	Gsp_FlipFB();
	gi_CrashY = 8 + GspGlobal_ACCESS(Morfling);
	GSP_DrawTExt(32,8 + GspGlobal_ACCESS(Morfling),TEXT,TExtCOLOR,0,GST_COPY,GST_SUB);	
	gi_CrashY += 16;
}

void Gsp_CrashPrintf(char *str)
{
	extern u32 GSP_BIGFADETOBLACK;
	GSP_BIGFADETOBLACK = 0x80;
	GSP_DrawTExt(32,gi_CrashY,str,0xffffff,0,GST_COPY,GST_SUB);	
	gi_CrashY += 12;
}
void Gsp_CrashPrintf2(char *str,u32 Color)
{
	extern u32 GSP_BIGFADETOBLACK;
	GSP_BIGFADETOBLACK = 0x80;
	GSP_DrawTExt(32,gi_CrashY,str,Color,0,GST_COPY,GST_SUB);	
	gi_CrashY += 13;
}
#ifdef GSP_FLUSH_DBG
ULONG GSP_GetPC()
{
	ULONG Ret;
	asm ("
		qmove	Ret,ra
		");
	return Ret;
}
#endif
void Gsp_Crash(ULONG CrashAddress , unsigned char *TEXT )
{
	unsigned char CRASHTEXT2[256];
	unsigned char CRASHTEXT3[256];
	unsigned char CRASHTEXT[256];
	GspGlobal_ACCESS(VBlankHook) = NULL;
	GspGlobal_ACCESS(ulFloatingFlipFlag) = 0;

//	EI();
	GspGlobal_ACCESS(ulFade_Counter) = 0;

	sceDmaReset(0);
	sceDevVu0Reset();
	sceDevVif1Reset();
	sceDevVu1Reset();
	sceDevGifReset();
	sceGsResetPath();
	sceGsSyncPath(0, 0);
	sceDmaReset(1);
	
   	EnableDmac(DMAC_VIF1);
   	EnableDmac(DMAC_GIF);//*/
   	EnableDmac(DMAC_TO_SPR);//*/
   	EnableDmac(DMAC_FROM_SPR);//*/
	GspGlobal_ACCESS(Morfling) = 0;
	GspGlobal_ACCESS(Status) &= ~GSP_Status_PAL_On;
	*GS_BGCOLOR = GspGlobal_ACCESS(ulBackColor) = 0xff ; 
	gsulSyncroPath = 0;
	if (GspGlobal_ACCESS(p_CurrentRegWriter) == &GspGlobal_ACCESS(stMRW1)) 
		GspGlobal_ACCESS(p_CurrentRegWriter) = &GspGlobal_ACCESS(stMRW2);
	else
		GspGlobal_ACCESS(p_CurrentRegWriter) = &GspGlobal_ACCESS(stMRW1);
	GSP_UnCachePtr(GspGlobal_ACCESS(p_CurrentRegWriter));
	GspGlobal_ACCESS(p_CurrentRegWriter)->NumRegs = 0;

	Gsp_InitFB(GSP_640x2__ , GSP_VRES_x2 , 32 , 32);
	CRASHTEXT[0] = 0;
	CRASHTEXT2[0] = 0;
	CRASHTEXT3[0] = 0;
	Gsp_GiveACrashAdressName(CrashAddress , CRASHTEXT2);
	CRASHTEXT3[0] = 0;
	if (DISPLAYSTACKSPR & 2) sprintf(CRASHTEXT3 , "(E-SPR!!)");
	sprintf(CRASHTEXT , "Crash at 0x%x %s%s\n" , CrashAddress , CRASHTEXT2 , CRASHTEXT3);
	Gsp_InfoScreen(CRASHTEXT , 0xffffff , 0xC060C0>>1);
#ifdef GSP_PS2_BENCH	
	/* Display activated rasters */
	{
		u_int RasterCounter , YTexPos;
		RasterCounter = GSP_NRaster;
		YTexPos = 8 + GspGlobal_ACCESS(Morfling);
		GSP_DrawTExt(500 - 8,YTexPos,"Activated rasters:",0xffffff,0,GST_COPY,GST_SUB);	
		YTexPos+= 16;
		while (RasterCounter--)
		{
			if (RastersStarts[RasterCounter] & 1)
			{
				sprintf(CRASHTEXT , " %2d: %s " , RasterCounter , RasterDescritpors[RasterCounter].Name+1);
				GSP_DrawTExt(500,YTexPos,CRASHTEXT,0xffffff,0,GST_COPY,GST_SUB);	
				YTexPos+= 16;
			}
		}
	}

#endif
}
u32 gsSavedTextureSize;
u_int gsWorldIsLoading = 0;
extern u_int LoadedTriangles;
extern u_int BarrePos;
extern void Gsp_ClearBorderExtend();
extern void GEO_DoUncache(void);
extern void GEO_ResetUncacheObjectList(void);
extern void GSP_DestroyAllBackCaches(void);
extern GSP_Bink_BeginLoad(u32 WOK);
void GSP_Bink_EndLoad();
void GSP_BeginWorldLoad()
{
	gsWorldIsLoading = (8) | 1;
	LoadedTriangles = 0;
	GSP_VideoFullScreenKill();
	GspGlobal_ACCESS(fFogCorrectorFar) = 0.00f;
	GspGlobal_ACCESS(fFogCorrector) = 0.00f;
#ifndef _FINAL_ 
	scePcStop();
#endif	
	if (!GspGlobal_ACCESS(ulFade_Counter))	
	{
		Gsp_FIFO_STOP();
		GSP_FlushAllFloatingFlip();
	}//*/
	Gsp_FIFO_STOP();
    Gsp_Flush(FLUSH_ALL);
    GSP_FlushAllFloatingFlip();
    Gsp_Flush(FLUSH_ALL);
    
	GSP_DestroyAllBackCaches();
	GEO_ResetUncacheObjectList();
	gsulSyncroPath |= FLUSH_WORLDLOAD;
	FlushCache(0);
	GspGlobal_ACCESS(Status) &= ~(GSP_Status_AE_DB | GSP_Status_AE_DBN | GSP_Status_AE_SM | GSP_Status_AE_SMSM)	;

	GSP_Bink_BeginLoad(0);
    
    /* Map with 2 Meg of textures */
    {
    	extern u32 WOR_gul_WorldKey;	
	    gsSavedTextureSize = GspGlobal_ACCESS(ulAvailableTextureCacheSize);
	    if (
			    (WOR_gul_WorldKey == 0x08008943) ||
			    (WOR_gul_WorldKey == 0x08006177) ||
			    (WOR_gul_WorldKey == 0xd9011976) ||
			    (WOR_gul_WorldKey == 0xd90145b1) ||
			    (WOR_gul_WorldKey == 0xbf00975d) ||
			    (WOR_gul_WorldKey == 0x0b004f21) ||
			    (WOR_gul_WorldKey == 0x0b0060e9) ||
			    (WOR_gul_WorldKey == 0x02008887) ||
			    (WOR_gul_WorldKey == 0x0b0057d6) ||
			    (WOR_gul_WorldKey == 0x08001b17) ||
			    (WOR_gul_WorldKey == 0x09001683) ||
			    (WOR_gul_WorldKey == 0x08002001) ||
			    (WOR_gul_WorldKey == 0x08005efa) ||
			    (WOR_gul_WorldKey == 0x10001793) ||
			    (WOR_gul_WorldKey == 0xbf00b3b3) ||
			    (WOR_gul_WorldKey == 0x0200beca) ||
			    (WOR_gul_WorldKey == 0x09001e3b) ||
			    (WOR_gul_WorldKey == 0x09004d69) ||
			    (WOR_gul_WorldKey == 0x080054c2) ||
			    (WOR_gul_WorldKey == 0x090099ac) ||
			    (WOR_gul_WorldKey == 0x0900373a) ||
			    (WOR_gul_WorldKey == 0x02009d63) ||
			    (WOR_gul_WorldKey == 0xd9012cf8) ||
			    (WOR_gul_WorldKey == 0x180003eb) ||
			    (WOR_gul_WorldKey == 0x180016af) ||
			    (WOR_gul_WorldKey == 0x18001b6f) ||
			    (WOR_gul_WorldKey == 0x3700ebef) ||
			    (WOR_gul_WorldKey == 0xc101eabc) ||
			    (WOR_gul_WorldKey == 0x82000213) ||
			    (WOR_gul_WorldKey == 0x1b000c5a) ||
			    (WOR_gul_WorldKey == 0x780048ef) ||
			    (WOR_gul_WorldKey == 0xc101fedb) ||
			    (WOR_gul_WorldKey == 0x28000105) ||
			    (WOR_gul_WorldKey == 0x3101468a) ||
			    (WOR_gul_WorldKey == 0) 
	    	)
	    	GspGlobal_ACCESS(ulAvailableTextureCacheSize) = 1024L * 1024L * 2L;
    }
}

void GSP_EndWorldLoad()
{
	extern volatile u_int gul_LastRealTimeCounter;
	extern volatile u_int gul_RealTimeCounter;
	
	FlushCache(0);
 
 	GspGlobal_ACCESS(ulAvailableTextureCacheSize) = gsSavedTextureSize;
 
	gul_RealTimeCounter = gul_LastRealTimeCounter;
 
	LocalVblankCounter = VBlankCounter;

	gsulSyncroPath &= ~FLUSH_WORLDLOAD;
	GSP_InitGeoCache(65536);
	GSP_InvalidateGeoCache(65536);
	GEO_DoUncache();
	gsWorldIsLoading &= ~1;
	if (GspGlobal_ACCESS(ulFade_Counter))
	{
		GspGlobal_ACCESS(ulFade_Time) = VBlankCounter;
		GspGlobal_ACCESS(ulFade_Alpha) = 1.0f;
	}//*/
	{
		extern void GSP_DrawBuf_EndWorldLoad();
		GSP_DrawBuf_EndWorldLoad();
	}
	
	FlushCache(0);
	
	{
		extern	void Gsp_FF_EndWorldLoad();

		Gsp_FF_EndWorldLoad();
	}
	GSP_Bink_EndLoad();
}
#ifdef GSP_PS2_BENCH
extern u_int NoMATDRAW;
extern u_int No2D;
#endif

void Gsp_Before2D()
{
	Gsp_FIFO_STOP();
    if (!ulVideoScreentValid) Gsp_AE();
	GspGlobal_ACCESS(RenderingInterface) = 1;
	GspGlobal_ACCESS(bSRS_FromScratch) = 1;
#ifdef GSP_PS2_BENCH
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_ShowTexture)  	Gsp_FIFO_STOP();
	if (LoadTextureInterface)
	if (No2D)
	{
		NoMATDRAW |= 8;
	}
#endif	
	GSP_LoadInterfaceTexture();
	GspGlobal_ACCESS(f2DFFX) = GspGlobal_ACCESS(f2DFFX_A2D);
	GspGlobal_ACCESS(f2DFFY) = GspGlobal_ACCESS(f2DFFY_A2D);
	GSP_VideoFullScreenUpdate();
}

// called to set camera before a 3D object rendering 
// when in interface(2D) rendering
void Gsp_Begin3DObjectWhileIn2D()
{
	GspGlobal_ACCESS(f2DFFX) = GspGlobal_ACCESS(f2DFFX_B2D);
	GspGlobal_ACCESS(f2DFFY) = GspGlobal_ACCESS(f2DFFY_B2D);
	Gsp_InitVU1Matrix_Extention();
}
// called to set camera after a 3D object rendering 
// when in interface(2D) rendering
void Gsp_End3DObjectWhileIn2D()
{
	GspGlobal_ACCESS(f2DFFX) = GspGlobal_ACCESS(f2DFFX_A2D);
	GspGlobal_ACCESS(f2DFFY) = GspGlobal_ACCESS(f2DFFY_A2D);
	Gsp_InitVU1Matrix_Extention();
}

extern int SND_i_DebugGetStatus(int);
GSP_VideoStream stVideoStr;
GSP_VideoStream stVideoStr2;
#ifndef _FINAL_	
#ifdef MEM_SPY
u32 DisplayMemSpy = 0;
#endif
#endif


void GSP_Flip(void)
{
	int fDelta,fDelta1;
	float Freq;
	float ScreenFreq_Over_CPUFreq;
	u_int CD,DC;
	ULONG Ytext=0;
	extern int BinkZob( );
	extern u32 GSP_BIGFADETOBLACK;
	static u32 Dec = 6;
	u32 FrametimerDEMO;
	extern BOOL	Demo_gb_IsActive;

	if (GspGlobal_ACCESS(Status) & GSP_Status_PAL_On)
	{
		if (GspGlobal_ACCESS(Xsize) != 512)
			Gsp_InitFB(512 , GspGlobal_ACCESS(VideoMode) , 32 - (GspGlobal_ACCESS(BppMode) << 3) , 32 - (GspGlobal_ACCESS(ZBppMode) << 3));
	}
//*/	
	

	if (!Dec)
	{
		BinkZob( );
		Dec = 0xffffffff;
	}
	else
	if (Dec <= 10)
		GSP_BIGFADETOBLACK = 0;
	Dec --;
	
	if (1)
	{
		extern u32 ps2INO_IsJOYTouched(u32 NoMushrooms);
		if (LocalVblankCounter == -1)
			LocalVblankCounter = VBlankCounter;
		/* Compuite 1 minute */

		if (GspGlobal_ACCESS(Status) & GSP_Status_PAL_On)
			FrametimerDEMO = 1 * 50 * 30; 
		else
			FrametimerDEMO = 1 * 60 * 30;
		
		if ((VBlankCounter - LocalVblankCounter > FrametimerDEMO) )
		{
			extern unsigned int WOR_gul_WorldKey;
			if (/*(WOR_gul_WorldKey == 0x4902ea59) || */(WOR_gul_WorldKey == 0x3d00c456) /*|| (WOR_gul_WorldKey == 0x3d00c45a)*/)
	   			Dec = 0; // 1 mn
			else
				Dec = 0xffffffff;
			LocalVblankCounter = VBlankCounter;
			
		}
		
		if (ps2INO_IsJOYTouched(0))
			LocalVblankCounter = VBlankCounter;
		
		
		
	}
	
	

	if (gsWorldIsLoading) 
	{
		if (gsWorldIsLoading & 1) 
			GSP_EndWorldLoad();
		gsWorldIsLoading -= 2;
	}


	if (VBlankCounter < 60 * 4)
	{
		extern void DisplayRights();

		Gsp_FIFO_STOP();

		DisplayRights();

		return;
	}


	

#ifdef GSP_PS2_BENCH
	if (No2D)
	{
		NoMATDRAW &= ~8;
	}
#endif	

#ifdef GSP_PS2_BENCH
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_ShowTexture)  	
	{

		Gsp_Flush(FLUSH_ALL);
		GSP_FlushAllFloatingFlip();

		Gsp_AE_ShowTexture();

		Gsp_FIFO_STOP();

		Gsp_Flush(FLUSH_ALL);

	}
#endif	

#ifndef GSP_USE_TRIPLE_BUFFERRING
	GSP_ManageGeoCache();
#endif



/**/
	DC = GspGlobal_ACCESS(DMA_VS_CPU);
	CD = GspGlobal_ACCESS(CPU_VS_DMA);
/*	{
	extern void GSP_DisplayMessageFullScreen(unsigned char *);
	while (1)	
	GSP_DisplayMessageFullScreen(" !\"#$%&'()*+,-./0123456789:;<=>?\n@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\n`abcdefghijklmnopqrstuvwxyz{|}~\nÄÅÇÉÑÖÜáàâäãåçéèêëíìîïñóòôöõúùûü\n†°¢£§•¶ß®©™´¨≠ÆØ∞±≤≥¥µ∂∑∏π∫ªºΩæø\n¿¡¬√ƒ≈∆«»… ÀÃÕŒœ–—“”‘’÷◊ÿŸ⁄€‹›ﬁﬂ\n‡·‚„‰ÂÊÁËÈÍÎÏÌÓÔÒÚÛÙıˆ˜¯˘˙˚¸˝˛ˇ\n");
	}//*/

	if (GspGlobal_ACCESS(Status) & GSP_Status_PAL_On)
		Freq = 50.f;
	else
		Freq = 60.f;
		
	ScreenFreq_Over_CPUFreq = Freq / 297000000.0f; // = 1.0f / number of cycles per frame

    GSP_EndRaster(19); // compte pas le flip dans display
#ifndef _FINAL_	

	fDelta = (scePcGetCounter0() - fLastTime) ;

	fDelta1 = (scePcGetCounter1() - fLastTime1);

#endif

	if (MEM_gi_RasterState || (GspGlobal_ACCESS(Status) & (GSP_Status_TNumCounterOn|GSP_Status_Raster_On|GSP_Status_Setup_On|GSP_Status_AE_ShowTexture)) || ShowNormals)
	{

		Gsp_Flush(FLUSH_ALL);

		GSP_FlushAllFloatingFlip();

	}

#ifndef _FINAL_ 
if(!Demo_gb_IsActive)
{


#ifdef MEM_SPY
	if (DisplayMemSpy)
#else	
    if(0)
#endif
	{
		Gsp_Flush(FLUSH_ALL);
		GSP_FlushAllFloatingFlip();
#ifdef MEM_SPY
		{
			extern void MEM_vBuildSummary();
			u32 CurrentNum,Summ;
			Ytext = 16 + GspGlobal_ACCESS(Morfling) + 16 * 5 - 8;
			CurrentNum = MEM_ulSummaryNb;
			MEM_vBuildSummary();
			if (MEM_ulSummaryNb != -1)
			{
				char string_MO[1024];
				char string[1024];
				Summ  = 0;
				for (CurrentNum = 0 ; CurrentNum < MEM_ulSummaryNb ; CurrentNum++) Summ += MEM_SortSummaryTable[CurrentNum]->ulAllocSize;
				sprintf(string_MO,"%2d.%03dMo(%d%%)", Summ >> 20 , (((Summ >> 10) & 1023) * 999) / 1024 , (u32)(100.0f * (float)Summ / (float)MEM_gst_MemoryInfo.ul_RealSize ));
				sprintf(string,"TOTAL:%s", string_MO );
				GSP_DrawTExt(16,Ytext,string,0xffffff,0x505050,GST_COPY,GST_SUB);
				Ytext += 16;

				for (CurrentNum = 0 ; CurrentNum < MEM_ulSummaryNb ; CurrentNum++)
				{
					if (MEM_SortSummaryTable[CurrentNum]->ulAllocSize > 128 * 1024)
					{
						u32 color ;
						sprintf(string_MO,"%2d.%03dMo", MEM_SortSummaryTable[CurrentNum]->ulAllocSize >> 20 , (((MEM_SortSummaryTable[CurrentNum]->ulAllocSize >> 10) & 1023) * 999) / 1024 );
						sprintf(string,"%s:%s", MEM_SortSummaryTable[CurrentNum]->sHeader , string_MO );
						color  = (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 4)) << (24 - 2 - 4);
						color |= (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 2)) << (16 - 2 - 2);
						color |= (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 0)) << (8 - 2);
						color |= 0x3f3f3f;
						
						GSP_DrawTExt(16,Ytext,string,color,0x505050,GST_COPY,GST_SUB);
						Ytext += 16;

						
						Gsp_CrashPrintf2(string,color);
					}
				}
			}
		}
		GSP_FLushAllTextureCache();
		
#endif

		
	}


#ifdef MEM_SPY
	if (MEM_bDisplayLeaks)
	{
		static u32 Cou = 0;
		float Radius;
		extern u32 ps2INO_IsJOYTouched(u32 NoMushrooms);
		extern void INO_Update();
		extern void Gsp_AE_Blur2(float Radius);
		Radius = 2.0f;
		if (Cou < 3)
		{
			Cou ++;
			while (ps2INO_IsJOYTouched(2)) {INO_Update();}; // Release Start
			while (!ps2INO_IsJOYTouched(2))
			{
				
				ULONG color;
				int i;
				LONG ulDeltaY = 32;
				LONG ulY;
				char sMsg[512];
				ulY = 100;
				sprintf(sMsg,"MEMORY LEAK : GAIN %d LOSS %d", MEM_iGain, MEM_iLoss);
				GSP_DrawTExt_X2Y2(32,ulY,sMsg,0x404080,0x505050,GST_COPY,GST_SUB,1,1);
				ulY += ulDeltaY;
				for (i=0; i<512; i++)
				{
					MEM_stElement *pElement = MEM_SortedLeaks[i];
					if (pElement)
					{
						sprintf(sMsg,"%s(%d) : %d",pElement->sFile, pElement->iLine, pElement->iAllocSize);
						GSP_DrawTExt_X2Y2(32,ulY, sMsg, 0xffeeffee, 0x505050,	GST_COPY,GST_SUB,0,0);
						ulY += 16;
					}
				}
				sprintf(sMsg,"\nPlease Send the bug to PVimont \nAnd the last loadded MAP path");
				GSP_DrawTExt_X2Y2(32,ulY, sMsg, 0xffeeffee, 0x404080,	GST_COPY,GST_SUB,0,0);
				sprintf(sMsg,"Press start to continue");
				GSP_DrawTExt_X2Y2(32,ulY + 64, sMsg, 0x008080, 0x404080,	GST_COPY,GST_SUB,1,1);
				Gsp_Flush(FLUSH_ALL);
				Gsp_AE_Blur2(Radius);
				Radius -= 1.0f/30.0f;
				if (Radius<0) Radius = 0;
				Gsp_FlipFB();
				Gsp_Flush(FLUSH_ALL);
				INO_Update();
			}
		}
	}
	{
	if (MEM_gst_MemoryInfo.ul_CurrentFree>>10 < 512)
		{
			extern u32 ps2_ScreenMode;
			u32 Y;
			char string[1024];
			sprintf(string,"WARNING:%dKo of free mem", MEM_gst_MemoryInfo.ul_CurrentFree>>10 );
			Gsp_Flush(FLUSH_ALL);
			GSP_FlushAllFloatingFlip();
			if (ps2_ScreenMode)
				Y = 8;
			else
				Y = 16 * 4 - 8;
			if (VBlankCounter & 16)
				GSP_DrawTExt_X2Y2(0,Y,string,0x404080,0xffffff,GST_COPY,GST_SUB,1,1);
			else
				GSP_DrawTExt_X2Y2(0,Y,string,0x404040,0xffffff,GST_COPY,GST_SUB,1,1);
			Gsp_Flush(FLUSH_ALL);
			GSP_FLushAllTextureCache();
		}
	}
#endif
	
	
	if (GspGlobal_ACCESS(Status) & GSP_Status_TNumCounterOn)
	{
		char String[64];
		extern u_int Statistic_NumberOfBranches;
		extern ULONG GetSPG2CachePercent();
		Ytext = 16 + GspGlobal_ACCESS(Morfling) + 16 * 5 - 8;
		sprintf(String, "Loaded triangles = %d", LoadedTriangles);
		GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		Ytext += 16;
		sprintf(String, "T = %d , branches = %d , branches cached = %d", GspGlobal_ACCESS(Tnum) , Statistic_NumberOfBranches , GetSPG2CachePercent());
		Statistic_NumberOfBranches = 0;
		GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		Ytext += 16;
		sprintf(String, "Number of elements = %d", GspGlobal_ACCESS(NCalls));
		GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		Ytext += 16;
		sprintf(String, "Average per call = %d", GspGlobal_ACCESS(Tnum) / GspGlobal_ACCESS(NCalls));
		GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		Ytext += 16;
		if (GspGlobal_ACCESS(NSPRITES))
		{
			sprintf(String, "NSpr = %d", GspGlobal_ACCESS(NSPRITES));
			GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		}
#ifdef GSP_PS2_BENCH
		Ytext += 16;
		if (n_Parralleles)
		{
			sprintf(String, "N light par = %d", n_Parralleles);
			GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		}
		Ytext += 16;
		if (n_Omni)
		{
			sprintf(String, "N light Omni = %d", n_Omni);
			GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		}
		Ytext += 16;
		if (n_Spot)
		{
			sprintf(String, "N light Spot = %d", n_Spot);
			GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		}
		Ytext += 16;
		if (n_LightCalls)
		{
			sprintf(String, "N lights calls = %d", n_LightCalls);
			GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		}
		Ytext += 16;
		sprintf(String, "***** Cache infos ********", n_LightCalls);
		GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		Ytext += 16;
		sprintf(String, "percent of XYZW = %d%%", GSP_GetCacheINFO(0));
		GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		Ytext += 16;
		sprintf(String, "percent of UV = %d%%", GSP_GetCacheINFO(1));
		GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		Ytext += 16;
		sprintf(String, "percent of Color = %d%%", GSP_GetCacheINFO(2));
		GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		Ytext += 16;
		sprintf(String, "Number Of Pulses = %d", GspGlobal_ACCESS(ulRotativBufferTurns));
		GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		{
			Ytext += 16;
			if (GEO_MemRasterPs2 > 1024 * 1024)
				sprintf(String, "Geo Ram = %d M %d K ", GEO_MemRasterPs2 >> 20 ,  (GEO_MemRasterPs2 >> 10) & ((1 << 10) - 1));
			else
				sprintf(String, "Geo Ram = %d K ", GEO_MemRasterPs2 >> 10);
			GSP_DrawTExt(GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 2) * 8,Ytext,String,0xffffff,0x606060,GST_COPY,GST_SUB);
		}

		GspGlobal_ACCESS(ulRotativBufferTurns) = 0;
		
		
		n_LightCalls = 0;
		
		n_Parralleles = 0;
		n_Omni = 0;
		n_Spot = 0;
		GSP_FLushAllTextureCache();
#endif
	}
	

	SND_i_DebugGetStatus(-1);

	eeRPC_i_DebugGetStatus(0);

	
	if (GspGlobal_ACCESS(Status) & GSP_Status_Console_On ) GSP_DrawConsole();
	
	if (GspGlobal_ACCESS(Status) & GSP_Status_Raster_On  ) 
	{
		float DetailOo;
		Gsp_Flush(FLUSH_ALL);
		{
			u_int control,Counter;
			scePcStop();
			Counter = GSP_NRaster;
			switch (RasterMode & 0xff)
			{
				case 0:// Normal
					control  = SCE_PC0_CPU_CYCLE|SCE_PC1_NO_EVENT;
					control |= (SCE_PC_U0|SCE_PC_S0|SCE_PC_K0|SCE_PC_EXL0);
					while (Counter--) Rasters[Counter] += Rasters1[Counter];
					fDelta += fDelta1;
					break;
				case 1:// ICache
					control  = SCE_PC0_ICACHE_MISS|SCE_PC1_NO_EVENT	  |SCE_PC_U0;
					ScreenFreq_Over_CPUFreq = 1.0f/100000.0f;
					while (Counter--) Rasters[Counter] += Rasters1[Counter];
					fDelta += fDelta1;
					break;
				case 2:// DCache
					control  = SCE_PC0_NO_EVENT	  |SCE_PC1_DCACHE_MISS|SCE_PC_U1;
					ScreenFreq_Over_CPUFreq = 1.0f/100000.0f;
					while (Counter--) Rasters[Counter] += Rasters1[Counter];
					fDelta += fDelta1;
					break;
				case 3:// Branch prediction misses
					control  = SCE_PC0_NO_EVENT	  |SCE_PC1_BRANCH_MISS_PREDICT|SCE_PC_U1;
					ScreenFreq_Over_CPUFreq = 1.0f/100000.0f;
					while (Counter--) Rasters[Counter] += Rasters1[Counter];
					fDelta += fDelta1;
					break;
				case 4:// INSTRUCTION COUNTER
					control  = SCE_PC0_NO_EVENT	  |SCE_PC1_INST_COMP|SCE_PC_U1;
					while (Counter--) Rasters[Counter] += Rasters1[Counter];
					fDelta += fDelta1;
					break;
				case 5:// Efficacity
					control  = SCE_PC0_CPU_CYCLE|SCE_PC1_INST_COMP|SCE_PC_U1|SCE_PC_U0;
					Counter = GSP_NRaster;
					while (Counter--) if (Rasters[Counter] == 0) 
											Rasters[Counter] = 0;
										else
											Rasters[Counter] = 256.0f * (float)Rasters1[Counter]/ (float)Rasters[Counter];
					ScreenFreq_Over_CPUFreq = 1.0f / 256.0f;
					fDelta = 256.0f * (float)fDelta / (float)fDelta1;
					break;
				case 6:// Reads
					control  = SCE_PC0_LOAD_COMP	  |SCE_PC1_STORE_COMP|SCE_PC_U0;
					ScreenFreq_Over_CPUFreq = 1.0f/100000.0f;
//					while (Counter--) Rasters[Counter] += Rasters1[Counter];
					fDelta += fDelta1;
					break;
				case 7:// Writes
					control  = SCE_PC0_LOAD_COMP	  |SCE_PC1_STORE_COMP|SCE_PC_U1;
					ScreenFreq_Over_CPUFreq = 1.0f/100000.0f;
					while (Counter--) Rasters[Counter] = Rasters1[Counter];
					fDelta += fDelta1;
					break;
				case 8:// COP1 COUNTER
					control  = SCE_PC0_NO_EVENT	  |SCE_PC1_COP1_COMP|SCE_PC_U1;
					while (Counter--) Rasters[Counter] += Rasters1[Counter];
					ScreenFreq_Over_CPUFreq = 1.0f/100000.0f;
					fDelta += fDelta1;
					break;
				case 9:// COP2 COUNTER
					control  = SCE_PC0_COP2_COMP	  |SCE_PC1_NO_EVENT|SCE_PC_U0;
					ScreenFreq_Over_CPUFreq = 1.0f/100000.0f;
					fDelta += fDelta1;
					break;
			}
			control |= SCE_PC_CTE;
			scePcStart(control, 0, 0);
		}
		DetailOo = ScreenFreq_Over_CPUFreq;
		if ((GspGlobal_ACCESS(Status) & GSP_Status_Raster_Prop_On)) DetailOo = 1.0f / (float)fDelta;

		if (GspGlobal_ACCESS(Status) & GSP_Status_Raster_Details_On)
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
			if (ShowNormals)			
					Base -= 16.0f / (float)GspGlobal_ACCESS(Ysize);
			for (RC = 0 ; RC < GSP_NRaster ; RC++) 
			{
				u_int Color;
				u_int Filter;
				Filter = RasterFilter;
				if (Filter == 0) Filter = Raster_FilterEngine|Raster_FilterDisplay|Raster_FilterGlobal;
				else
				if (Filter == 3) Filter = Raster_FilterGlobal;
				else
				if (Filter == 4) Filter = Raster_FilterIAUsr;
				RCS = (RasterMode&(1<<16))? RC : Sorted[RC];
				RastersSmooth[RCS] = RastersSmooth[RCS] * AR_SMOOTH + (float)Rasters[RCS] * DetailOo * (1.0f - AR_SMOOTH);

				if ((RastersSmooth[RCS] > 0.002f)  && (RasterDescritpors[RCS].FilterType & Filter))
				{
					Base -= 16.0f / (float)GspGlobal_ACCESS(Ysize);
					
					Color = 0xf0808080;
					if ((RCS + 1) & 1) Color |= 0x40;
					if ((RCS + 1) & 2) Color |= 0x4000;
					if ((RCS + 1) & 4) Color |= 0x400000;
					if((RC < RasterDetailMum) || (RasterMode&(1<<16)))
					{
						if (Rasters_MaxEnble)
						{
							if ((GspGlobal_ACCESS(FrameCounter) & 0x7f) == 0)
							{
								L_memset(Rasters_Max , 0 , GSP_NRaster * 4 );
								Rasters_MaxEnble = 1321635;
							}
							Rasters_Max[RCS] = lMax(Rasters_Max[RCS] , Rasters[RCS]) ; 
							GSP_DrawGlobalRasters(RasterDescritpors[RCS].Name+1, (float)Rasters_Max[RCS] * DetailOo, Base , RastersSmooth[RCS], Color);
						}
						else
							GSP_DrawGlobalRasters(RasterDescritpors[RCS].Name+1, (float)Rasters[RCS] * DetailOo, Base , RastersSmooth[RCS], Color);
					}
				}
				Gsp_Flush(FLUSH_ALL);
			}
			if (((RasterMode & 0xff) == 0) &&  (GspGlobal_ACCESS(Status) & GSP_Status_Raster_Details_On))
			{
				float Local;
				AverageRasters = AverageRasters * AR_SMOOTH + (float)fDelta  * ScreenFreq_Over_CPUFreq * (1.0f - AR_SMOOTH);
				Local = (float)fDelta  * ScreenFreq_Over_CPUFreq;
/*				Local -= (float)Rasters[18] * ScreenFreq_Over_CPUFreq;
				Local -= (float)Rasters[38] * ScreenFreq_Over_CPUFreq;
				Local += fMax((float)Rasters[18] , (float)Rasters[38]) * ScreenFreq_Over_CPUFreq;*/
				
				Local = ((float)RastersSmooth[18]+(float)RastersSmooth[19]);
				
				Base = 0.95f ;
				if (ShowNormals)			
						Base -= 16.0f / (float)GspGlobal_ACCESS(Ysize);
				GSP_DrawGlobalRasters("" , (float)Local, Base    , AverageRasters , 0xf08080FF);
			} else
			{
				Base = 0.95f ;
				if (ShowNormals)			
						Base -= 16.0f / (float)GspGlobal_ACCESS(Ysize);
				AverageRasters = AverageRasters * AR_SMOOTH + (float)fDelta  * ScreenFreq_Over_CPUFreq * (1.0f - AR_SMOOTH);
				GSP_DrawGlobalRasters("" , (float)AverageRasters/*fDelta  * ScreenFreq_Over_CPUFreq*/, Base    , AverageRasters , 0xf08080FF);
			}
			for (RC = 0 ; RC < GSP_NRaster ; RC++) 
			{
				Rasters[RC] = 0;
				Rasters1[RC] = 0;
			}
			
			Gsp_Flush(FLUSH_ALL);
		} else
		{
			scePcStop();
			scePcStart(SCE_PC0_CPU_CYCLE|SCE_PC1_NO_EVENT|(SCE_PC_U0|SCE_PC_S0|SCE_PC_K0|SCE_PC_EXL0)|SCE_PC_CTE, 0, 0);
			AverageRasters = AverageRasters * AR_SMOOTH + (float)fDelta  * ScreenFreq_Over_CPUFreq * (1.0f - AR_SMOOTH);
			GSP_DrawGlobalRasters("" , (float)fDelta  * ScreenFreq_Over_CPUFreq, 0.95f   , AverageRasters , 0xf08080FF);
		}
		GSP_FLushAllTextureCache();
	} 
	if (ShowNormals)
	{
		char String[64];
		u32 Color,TimMax,TimGlob;
		Gsp_Flush(FLUSH_ALL);		
		switch (ShowNormals & 3)
		{
			case 0:sprintf(String, "nada");
					break;
			case 1:sprintf(String, "Color cost=IA");
					TimGlob = ColorCost_GlobalTime_IA;
					TimMax = ColorCost_MaxTime_IA;
					Color = 0xff;
					break;
			case 2:sprintf(String, "Color cost=Graphic");
					TimGlob = ColorCost_GlobalTime_GRAPHIC;
					TimMax = ColorCost_MaxTime_GRAPHIC;
					Color = 0xff00;
					break;
			case 3:sprintf(String, "Color cost=Graphic+IA");
					TimGlob = ColorCost_GlobalTime_GRAPHIC + ColorCost_GlobalTime_IA;
					TimMax = ColorCost_MaxTime_GRAPHIC + ColorCost_MaxTime_IA;
					Color = 0xff0000;
					break;
		}
		if (!TimGlob) TimGlob = 1;
		GSP_DrawTExt_X2Y2(20,20,String,0xffffff,0x606060,GST_COPY,GST_SUB,1,0);
		Gsp_DrawRectangle(0.1f,0.08f,0.8f,RASVSIZE,0,Color,GST_COPY);
		Gsp_DrawRectangle(0.1f,0.95f,0.8f,RASVSIZE,0,Color,GST_COPY);
		sprintf(String, "Max %d%% of %.2f frame",100 * TimMax / TimGlob,(float)TimGlob * ScreenFreq_Over_CPUFreq);
		GSP_DrawTExt_X2Y2(GspGlobal_ACCESS(Xsize) - 180,20,String,0xffffff,0x606060,GST_COPY,GST_SUB,0,0);
		
		scePcStop();
		scePcStart(SCE_PC0_CPU_CYCLE|SCE_PC1_NO_EVENT|(SCE_PC_U0|SCE_PC_S0|SCE_PC_K0|SCE_PC_EXL0)|SCE_PC_CTE, 0, 0);
		GSP_FLushAllTextureCache();
	}
	

	if(MEM_gi_RasterState) 
	{

		Gsp_Flush(FLUSH_ALL);

		MEM_RasterSetRequest(MEM_gi_RasterState);

	}

}
#endif // _FINAL_
	
#ifndef _FINAL_ 
if(!Demo_gb_IsActive)
{
	Gsp_SheetCode();

#ifdef GSP_PS2_BENCH

	Gsp_DisplayStackInfo();

#endif	
}
#endif


	if (!ulVideoScreentValid) Gsp_AE_CopyShadows();

	if (!ulVideoScreentValid) Gsp_AE_Vertical_AA();

	Gsp_AE_FADE();

	Gsp_AE_FlushAllSpecial();

	Gsp_FIFO_STOP();


	
	
	
	GspGlobal_ACCESS(Tnum) = 0;
	GspGlobal_ACCESS(DMA_VS_CPU) = 0;
	GspGlobal_ACCESS(CPU_VS_DMA) = 0;
	GspGlobal_ACCESS(NCalls)    = 0;
	GspGlobal_ACCESS(NCallsSRS) = 0;
	GspGlobal_ACCESS(NSPRITES) = 0;
	GspGlobal_ACCESS(bSRS_FromScratch) = 2; // Used by setrenderstate
	GspGlobal_ACCESS(RenderingInterface) = 0;
	
   	GspGlobal_ACCESS(fAddY) = 2048.0f;
	if ((GspGlobal_ACCESS(VideoMode) == GSP_VRES_xAUTO) && VBlankOdd && (GspGlobal_ACCESS(fCurrentFrquency) <= 1.0f))
		GspGlobal_ACCESS(fAddY) = 2048.5f;
	/*fCurrentFrquency*/


	if (MEM_gi_RasterState || ShowNormals ||
	    (GspGlobal_ACCESS(Status) & (GSP_Status_TNumCounterOn|GSP_Status_Raster_On|GSP_Status_Setup_On|GSP_Status_AE_ShowTexture))
	    )
	{

		Gsp_Flush(FLUSH_ALL);

	 }


 	Gsp_FloatingFlip_START();

	 	
	{	 	
		float Factor;
		GspGlobal_ACCESS(p_AE_Params)->fWR_TransparencyMax= 0.0f;
		Factor = GDI_gpst_CurDD->st_Camera.st_Matrix.T.z - GspGlobal_ACCESS(p_AE_Params)->fWR_ZPlane;
		Factor -= 20.0f;
		Factor /= 200.0f;
		GspGlobal_ACCESS(p_AE_Params)->fWR_TransparencyMax = fMin(fMax(Factor , 0.0f) , 1.0f);

	}
	

	GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_WATEREFFECT;

	if ((GspGlobal_ACCESS(p_AE_Params)->fWR_ZHeight != 0.0f) && (GspGlobal_ACCESS(p_AE_Params)->fWR_TransparencyMax != 1.0f))
	{
		extern u_int WTR_IsSymetryActive();

		if (WTR_IsSymetryActive()) GspGlobal_ACCESS(Status) |= GSP_Status_AE_WATEREFFECT;

	}
	
#ifndef _FINAL_ 
	fLastTime  = scePcGetCounter0();
	fLastTime1 = scePcGetCounter1();
	GSP_BeginRaster(19); // compte pas le flip dans display
#endif	
	GspGlobal_ACCESS(f2DFFX) = GspGlobal_ACCESS(f2DFFX_B2D);
	GspGlobal_ACCESS(f2DFFY) = GspGlobal_ACCESS(f2DFFY_B2D);
}


/*
 =======================================================================================================================
 Aim: Clear buffer 
 =======================================================================================================================
 */
void GSP_Clear(LONG _l_Buffer, ULONG _ul_Color)
{
	/* Clear Zbuffer & color buffer */
	if (_l_Buffer & GDI_Cl_ColorBuffer)
	{
		GspGlobal_ACCESS(ulBackColor) = _ul_Color;
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gsp_InitVU1Matrix_FOG(ULONG Color,float Zmin,float Zmax)
{
	MyVectorFormat  *AxAyxxZM;
	MyVectorFormat  *CxCyCzFC;
	float	OOMin,OOMax,Factor;
	AxAyxxZM = &GspGlobal_ACCESS(stCurrentAxAyxxZM);
	CxCyCzFC = &GspGlobal_ACCESS(stCurrentCxCyCzFC);
	OOMin = AxAyxxZM->z /*GspGlobal_ACCESS(fCurrentFocale)*/ / (Zmin * GIGSCALE0);
	OOMax = AxAyxxZM->z /* GspGlobal_ACCESS(fCurrentFocale)*/ / (Zmax * GIGSCALE0);
	Factor = 255.0f / (OOMin - OOMax);
	AxAyxxZM->w = -Factor * OOMax; // FOGFac
	CxCyCzFC->w = Factor; // FOGFac
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef GSP_PS2_BENCH
extern u_int bTakeSnapshot;
extern float fAddXAA;
extern float fAddYAA;
#endif
void Gsp_InitVU1Matrix_Extention()
{
	MyVectorFormat  *AxAyxxZM;
	MyVectorFormat  *CxCyCzFC;
	ULONG XSz,YSz;
	XSz = GspGlobal_ACCESS(Xsize);
	YSz = GspGlobal_ACCESS(Ysize);
	AxAyxxZM = &GspGlobal_ACCESS(stCurrentAxAyxxZM);
	CxCyCzFC = &GspGlobal_ACCESS(stCurrentCxCyCzFC);
	AxAyxxZM->x = 2048.0f;
	AxAyxxZM->y = GspGlobal_ACCESS(fAddY);
#if 0	
	if (GDI_gpst_CurDD)
	{
		if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)
		{
			AxAyxxZM->x = 2048.0f + (XSz >> 2);
			XSz >>= 1;
		} else
		{
			AxAyxxZM->x = 2048.0f - (XSz >> 2);
			XSz >>= 1;
		} 
	}
#endif
	AxAyxxZM->w = 0.0f; // FOGFac
	CxCyCzFC->w = 0.0f; // FOGFac
	CxCyCzFC->x = (float)(XSz >> 1) * GspGlobal_ACCESS(f2DFFX);
	CxCyCzFC->y = (float)(YSz >> 1) * GspGlobal_ACCESS(f2DFFY);
#ifdef GSP_PS2_BENCH
	if (bTakeSnapshot)
	{
		AxAyxxZM->x += fAddXAA;
		AxAyxxZM->y += fAddYAA;
	}
#endif

	
	CxCyCzFC->z = GspGlobal_ACCESS(ZFactor);
	AxAyxxZM->z = GspGlobal_ACCESS(fCurrentFocale);
	CxCyCzFC->z /= AxAyxxZM->z;

	Gsp_InitVU1Matrix_FOG(GspGlobal_ACCESS(FogColor),GspGlobal_ACCESS(FogZNear),GspGlobal_ACCESS(FogZFar));

	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_WATEREFFECT)
	{	
		MATH_tdst_Vector stZProut;
		MyVectorFormat  *AxAyxxZM;
		MyVectorFormat  *CxCyCzFC;
		AxAyxxZM = &GspGlobal_ACCESS(stCurrentAxAyxxZM);
		CxCyCzFC = &GspGlobal_ACCESS(stCurrentCxCyCzFC);
		stZProut .x = stZProut.y = 0.0f;
		stZProut .z = -1.0f;

		/*  Z plane -> Camera coordinate */
		GspGlobal_ACCESS(Additionnal_Information1) . x = MATH_f_DotProduct(&stZProut,MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
		GspGlobal_ACCESS(Additionnal_Information1) . y = MATH_f_DotProduct(&stZProut,MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix)) / GspGlobal_ACCESS(YCorrectionCoef);
		GspGlobal_ACCESS(Additionnal_Information1) . z = MATH_f_DotProduct(&stZProut,MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
		/* Z plane distance */
		GspGlobal_ACCESS(Additionnal_Information1) . w = (-GspGlobal_ACCESS(p_AE_Params)->fWR_ZPlane-1.0f-MATH_f_DotProduct(&stZProut,&GDI_gpst_CurDD->st_Camera.st_Matrix.T)) * GIGSCALE0;
		GspGlobal_ACCESS(Additionnal_Information2) . x = GspGlobal_ACCESS(YCorrectionCoef);
		GspGlobal_ACCESS(Additionnal_Information2) . y = CxCyCzFC->x; // MULX
		GspGlobal_ACCESS(Additionnal_Information2) . z = CxCyCzFC->y; // MULY
		GspGlobal_ACCESS(Additionnal_Information2) . w = AxAyxxZM->y; /* aDDy */
	}
}
extern void Gsp_Water_FullCompute_Hook(Gsp_BigStruct	  *p_BIG,Gsp_tdst_DrawBuffer *p_MDB , u_int NumberOfPrims);
extern void Gsp_RenderBuffer(register Gsp_BigStruct	  *p_BIG , register ULONG Num);

MyMatrixFormat *Gsp_SetDrawBuffer_Matrix(Gsp_BigStruct	  *p_BIG, Gsp_tdst_DrawBuffer *p_MDB )
{
	register u_long128 *p_ReturnValue;
	register u_int				Num;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);	
	Num = (p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase);
	if ((Num + p_MDB->Size > (RAM_BUF_SIZE - 0x1000))) 
		Gsp_RenderBuffer(p_BIG,Num);

	p_ReturnValue = (u_long128 *)p_BIG->p_CurrentBufferPointer;
	*(p_ReturnValue++) = Gsp_64_to_128(p_MDB->VIF_Matrix);
	p_BIG->p_CurrentBufferPointer = (u_int)p_ReturnValue + GSP_ALIGN(sizeof(MyMatrixFormat));
	p_BIG->bGSUpdateFlag = 2;
	return (MyMatrixFormat *)p_ReturnValue;
}

MyMatrixFormat *Gsp_SetDrawBuffer_LIGHT(Gsp_BigStruct	  *p_BIG, Gsp_tdst_DrawBuffer *p_MDB )
{
	register u_long128 *p_ReturnValue;
	register u_int				Num;
	Num = (p_BIG->p_CurrentBufferPointer - p_BIG->p_BufferPointerBase);
	p_ReturnValue = (u_long128 *)p_BIG->p_CurrentBufferPointer;
	*(p_ReturnValue++) = Gsp_64_to_128(p_MDB->VIF_LIGHT);
	p_BIG->p_CurrentBufferPointer = (u_int)p_ReturnValue + GSP_ALIGN(sizeof(MyMatrixFormat));
	return (MyMatrixFormat *)p_ReturnValue;
}

/* Matrix Must be aligned !!!! */
void GSP_SetViewMatrix( MATH_tdst_Matrix *_pst_Matrix)
{
	MyMatrixFormat 						*p_MyMatrix;
	/* Save GIF Tag in point 0*/
	p_MyMatrix = Gsp_SetDrawBuffer_Matrix( &gs_st_Globals,GspGlobal_ACCESS(MDB) );
	
	if(MATH_b_TestScaleType(_pst_Matrix))
	{
		MATH_MakeOGLMatrix((MATH_tdst_Matrix *)p_MyMatrix, _pst_Matrix);
	} else
	{
		*(u_int *)&p_MyMatrix -> I . x = *(u_int *)&_pst_Matrix -> Ix;
		*(u_int *)&p_MyMatrix -> I . y = *(u_int *)&_pst_Matrix -> Iy;
		*(u_int *)&p_MyMatrix -> I . z = *(u_int *)&_pst_Matrix -> Iz;
		*(u_int *)&p_MyMatrix -> J . x = *(u_int *)&_pst_Matrix -> Jx;
		*(u_int *)&p_MyMatrix -> J . y = *(u_int *)&_pst_Matrix -> Jy;
		*(u_int *)&p_MyMatrix -> J . z = *(u_int *)&_pst_Matrix -> Jz;
		*(u_int *)&p_MyMatrix -> K . x = *(u_int *)&_pst_Matrix -> Kx;
		*(u_int *)&p_MyMatrix -> K . y = *(u_int *)&_pst_Matrix -> Ky;
		*(u_int *)&p_MyMatrix -> K . z = *(u_int *)&_pst_Matrix -> Kz;
		*(u_int *)&p_MyMatrix -> T . x = *(u_int *)&_pst_Matrix -> T.x;
		*(u_int *)&p_MyMatrix -> T . y = *(u_int *)&_pst_Matrix -> T.y;
		*(u_int *)&p_MyMatrix -> T . z = *(u_int *)&_pst_Matrix -> T.z;
	}
	*(u_long128 *)&p_MyMatrix -> AxAyxxZM = *(u_long128 *)&GspGlobal_ACCESS(stCurrentAxAyxxZM);
	*(u_long128 *)&p_MyMatrix -> CxCyCzFC = *(u_long128 *)&GspGlobal_ACCESS(stCurrentCxCyCzFC);
	p_MyMatrix -> I . w = 400.0f;
	p_MyMatrix -> J . w = GIGSCALE0;
	p_MyMatrix -> K . w = GIGSCALE0 * GspGlobal_ACCESS(YCorrectionCoef) ;
#define MipMapFactor 25000.0f
	p_MyMatrix -> T . w = MipMapFactor; /* DEBUG must be <= 1 */
	
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_WATEREFFECT)
	{
		float SymDP;
		ULONG Counter;
		MyVectorFormat	stSymetrisedMatrix;
		*(u_long128 *)&GspGlobal_ACCESS(stCurrentMatrix[0]) =  *(u_long128 *)&p_MyMatrix -> I;
		*(u_long128 *)&GspGlobal_ACCESS(stCurrentMatrix[1]) =  *(u_long128 *)&p_MyMatrix -> J;
		*(u_long128 *)&GspGlobal_ACCESS(stCurrentMatrix[2]) =  *(u_long128 *)&p_MyMatrix -> K;
		*(u_long128 *)&GspGlobal_ACCESS(stCurrentMatrix[3]) =  *(u_long128 *)&p_MyMatrix -> T;
		Counter = 3;
		while (Counter--)
		{
			GspGlobal_ACCESS(stSymetrisedMatrix[Counter]) = GspGlobal_ACCESS(stCurrentMatrix[Counter]);
			SymDP = GspGlobal_ACCESS(Additionnal_Information1).x * GspGlobal_ACCESS(stCurrentMatrix[Counter]).x + 
					GspGlobal_ACCESS(Additionnal_Information1).y * GspGlobal_ACCESS(stCurrentMatrix[Counter]).y * GspGlobal_ACCESS(YCorrectionCoef)+ 
					GspGlobal_ACCESS(Additionnal_Information1).z * GspGlobal_ACCESS(stCurrentMatrix[Counter]).z;
			SymDP *= -2.0f;
			stSymetrisedMatrix . x = GspGlobal_ACCESS(Additionnal_Information1).x * SymDP;
			stSymetrisedMatrix . y = GspGlobal_ACCESS(Additionnal_Information1).y * GspGlobal_ACCESS(YCorrectionCoef) * SymDP;
			stSymetrisedMatrix . z = GspGlobal_ACCESS(Additionnal_Information1).z * SymDP;
			GspGlobal_ACCESS(stSymetrisedMatrix[Counter]).x += stSymetrisedMatrix . x;
			GspGlobal_ACCESS(stSymetrisedMatrix[Counter]).y += stSymetrisedMatrix . y;
			GspGlobal_ACCESS(stSymetrisedMatrix[Counter]).z += stSymetrisedMatrix . z;
		}
		GspGlobal_ACCESS(stSymetrisedMatrix[3]) = GspGlobal_ACCESS(stCurrentMatrix[3]);
		SymDP = GspGlobal_ACCESS(Additionnal_Information1).x * GspGlobal_ACCESS(stCurrentMatrix[3]).x + 
				GspGlobal_ACCESS(Additionnal_Information1).y * GspGlobal_ACCESS(stCurrentMatrix[3]).y * GspGlobal_ACCESS(YCorrectionCoef)+ 
				GspGlobal_ACCESS(Additionnal_Information1).z * GspGlobal_ACCESS(stCurrentMatrix[3]).z;
		SymDP -= GspGlobal_ACCESS(Additionnal_Information1).w / GIGSCALE0;
		SymDP *= -2.0f;
		stSymetrisedMatrix . x = GspGlobal_ACCESS(Additionnal_Information1).x * SymDP;
		stSymetrisedMatrix . y = GspGlobal_ACCESS(Additionnal_Information1).y * GspGlobal_ACCESS(YCorrectionCoef) * SymDP;
		stSymetrisedMatrix . z = GspGlobal_ACCESS(Additionnal_Information1).z * SymDP;
		
		GspGlobal_ACCESS(stSymetrisedMatrix[3]).x += stSymetrisedMatrix . x;
		GspGlobal_ACCESS(stSymetrisedMatrix[3]).y += stSymetrisedMatrix . y;
		GspGlobal_ACCESS(stSymetrisedMatrix[3]).z += stSymetrisedMatrix . z;//*/

		*(u_long128 *)&p_MyMatrix -> Additionnal_Information1 = *(u_long128 *)&GspGlobal_ACCESS(Additionnal_Information1);
		*(u_long128 *)&p_MyMatrix -> Additionnal_Information2 = *(u_long128 *)&GspGlobal_ACCESS(Additionnal_Information2);
		p_MyMatrix -> T . w = p_MyMatrix -> AxAyxxZM.x; /* aDDX */
		
		GspGlobal_ACCESS(Gsp_DrawHook) = Gsp_Water_FullCompute_Hook;
	} else GspGlobal_ACCESS(Gsp_DrawHook) = NULL;
	
}



void GSP_SetViewMatrix_PlusLight( MATH_tdst_Matrix *_pst_Matrix)
{
	MyMatrixFormat 						*p_MyMatrix;
	/* Save GIF Tag in point 0*/
	p_MyMatrix = Gsp_SetDrawBuffer_Matrix( &gs_st_Globals, GspGlobal_ACCESS(MDB) );
	
	if(MATH_b_TestScaleType(_pst_Matrix))
	{
		MATH_MakeOGLMatrix((MATH_tdst_Matrix *)p_MyMatrix, _pst_Matrix);
	} else
	{
		*(u_int128 *)&p_MyMatrix -> I . x = *(u_int128 *)&_pst_Matrix -> Ix;
		*(u_int128 *)&p_MyMatrix -> J . x = *(u_int128 *)&_pst_Matrix -> Jx;
		*(u_int128 *)&p_MyMatrix -> K . x = *(u_int128 *)&_pst_Matrix -> Kx;
		*(u_int128 *)&p_MyMatrix -> T . x = *(u_int128 *)&_pst_Matrix -> T.x;
	}
	*(u_long128 *)&p_MyMatrix -> AxAyxxZM = *(u_long128 *)&GspGlobal_ACCESS(stCurrentAxAyxxZM);
	*(u_long128 *)&p_MyMatrix -> CxCyCzFC = *(u_long128 *)&GspGlobal_ACCESS(stCurrentCxCyCzFC);
	p_MyMatrix -> I . w = 400.0f;
	p_MyMatrix -> J . w = GIGSCALE0;
	p_MyMatrix -> K . w = GIGSCALE0 * GspGlobal_ACCESS(YCorrectionCoef) ;
	p_MyMatrix -> T . w = MipMapFactor; /* DEBUG must be <= 1 */
	GspGlobal_ACCESS(Gsp_DrawHook) = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void GSP_SetViewMatrix_SDW(MATH_tdst_Matrix *_pst_Matrix , float *Limits)
{
	MATH_tdst_Matrix					st_OGLMatrix;
	MyMatrixFormat *p_MyMatrix;
	u_int			Morfling_Aligned;
	
	if(MATH_b_TestScaleType(_pst_Matrix))
	{
		MATH_MakeOGLMatrix(&st_OGLMatrix, _pst_Matrix);
		_pst_Matrix = &st_OGLMatrix;
	}
	p_MyMatrix = Gsp_SetDrawBuffer_Matrix( &gs_st_Globals, GspGlobal_ACCESS(MDB) );
	
	Morfling_Aligned = (GspGlobal_ACCESS(Morfling) + 31) & ~31;
	
	p_MyMatrix -> I . x = _pst_Matrix -> Ix;
	p_MyMatrix -> I . y = _pst_Matrix -> Iy;
	p_MyMatrix -> I . z = _pst_Matrix -> Iz;
	p_MyMatrix -> J . x = _pst_Matrix -> Jx;
	p_MyMatrix -> J . y = _pst_Matrix -> Jy;
	p_MyMatrix -> J . z = _pst_Matrix -> Jz;
	p_MyMatrix -> K . x = _pst_Matrix -> Kx;
	p_MyMatrix -> K . y = _pst_Matrix -> Ky;
	p_MyMatrix -> K . z = _pst_Matrix -> Kz;
	p_MyMatrix -> T . x = 2048.0f - (float)GspGlobal_ACCESS(Xsize) * 0.5f + _pst_Matrix -> T.x;
	p_MyMatrix -> T . y = 2048.0f - (float)GspGlobal_ACCESS(Ysize) * 0.5f + _pst_Matrix -> T.y  +  (float)Morfling_Aligned;
	p_MyMatrix -> T . z = _pst_Matrix -> T.z;
	p_MyMatrix -> I . w = p_MyMatrix -> J . w = p_MyMatrix -> K . w = p_MyMatrix -> T . w = 1.0f;
/*
	AX AY = MIN XY;
	CX CY = MIN XY;
*/
	p_MyMatrix -> AxAyxxZM.x = Limits[0] + 2048.0f - (float)GspGlobal_ACCESS(Xsize) * 0.5f;
	p_MyMatrix -> AxAyxxZM.y = Limits[1] + 2048.0f - (float)GspGlobal_ACCESS(Ysize) * 0.5f;
	p_MyMatrix -> CxCyCzFC.x = Limits[2] + 2048.0f - (float)GspGlobal_ACCESS(Xsize) * 0.5f;
	p_MyMatrix -> CxCyCzFC.y = Limits[3] + 2048.0f - (float)GspGlobal_ACCESS(Ysize) * 0.5f +  (float)Morfling_Aligned;
	
	p_MyMatrix -> Additionnal_Information1 . x = 0.0f;
	p_MyMatrix -> Additionnal_Information1 . y = 0.0f;
	p_MyMatrix -> Additionnal_Information1 . z = 1.0f;
	p_MyMatrix -> Additionnal_Information1 . w = 16.0f;
	
	GspGlobal_ACCESS(Gsp_DrawHook) = NULL;
	
	
}

/*
 =======================================================================================================================
    Aim:    Send View matrix for render
 =======================================================================================================================
 */
void GSP_SetProjectionMatrix( CAM_tdst_Camera *_pst_Cam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float					f, f_ScreenRatio;
	LONG					w, h, W, H;
	LONG					x, y;
	ULONG					Flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Compute screen ratio */
	if
	(
		(GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst <= 0)
	||	(GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst >= GDI_Cul_SRC_Number)
	) f_ScreenRatio = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
	else
		f_ScreenRatio = GDI_gaf_ScreenRation[GDI_Cul_SRC_4over3];

	f = _pst_Cam->f_YoverX * GDI_gpst_CurDD->st_ScreenFormat.f_PixelYoverX * f_ScreenRatio;
	Flags = GDI_gpst_CurDD->st_ScreenFormat.ul_Flags;

	/* Compute height and width of screen */
	w = GDI_gpst_CurDD->st_Device.l_Width;
	h = GDI_gpst_CurDD->st_Device.l_Height;

	if(Flags & GDI_Cul_SFF_ReferenceIsY)
	{
		H = h;
		W = (LONG) (h / f);

		if(((Flags & GDI_Cul_SFF_CropToWindow) && (W > w)) || ((Flags & GDI_Cul_SFF_OccupyAll) && (W < w)))
		{
			H = (LONG) (w * f);
			W = w;
		}
	}
	else
	{
		H = (LONG) (w * f);
		W = w;

		if(((Flags & GDI_Cul_SFF_CropToWindow) && (H > h)) || ((Flags & GDI_Cul_SFF_OccupyAll) && (H < h)))
		{
			W = (LONG) (h / f);
			H = h;
		}
	}

	if ( _pst_Cam->f_ViewportWidth == 0)
	{
		_pst_Cam->f_ViewportWidth = 1.0f;
		_pst_Cam->f_ViewportHeight = 1.0f;
		_pst_Cam->f_ViewportLeft = 0.0f;
		_pst_Cam->f_ViewportTop = 0.0f;
	}

	x = (int) (_pst_Cam->f_ViewportLeft * W + ((w - W) / 2));
	y = (int) (_pst_Cam->f_ViewportTop * H + ((h - H) / 2));
	h = (int) (_pst_Cam->f_ViewportHeight * H);
	w = (int) (_pst_Cam->f_ViewportWidth * W);

	_pst_Cam->l_ViewportRealLeft = x;
	_pst_Cam->l_ViewportRealTop = y;

	GspGlobal_ACCESS(fCurrentFocale) = 1.0f / fNormalTan(_pst_Cam->f_FieldOfVision / 2);
	Gsp_InitVU1Matrix_Extention();
}

/*
 =======================================================================================================================
    Aim:    Attach world to device
 =======================================================================================================================
 */
LONG GSP_l_AttachWorld( WOR_tdst_World *_pst_World)
{
	return 1;
}

/*
 =======================================================================================================================
    Aim:    Detach world from device
 =======================================================================================================================
 */
LONG GSP_l_DetachWorld(void)
{
	return 1;
}

/*
 =======================================================================================================================
    Aim:    Notify that world had changed
 =======================================================================================================================
 */
LONG GSP_l_WorldChanged(void)
{
	return 1;
}

/*$4
 ***********************************************************************************************************************
    Private function
 ***********************************************************************************************************************
 */

/* Aim: Set Device Context pixel format */
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GSP_SetDCPixelFormat(HDC _hDC)
{
}

/*
 =======================================================================================================================
    Aim:    Setup rendering context
 =======================================================================================================================
 */
void GSP_SetupRC(GSP_tdst_SpecificData *_pst_SD)
{
}

/*
 =======================================================================================================================
    Aim:    Main init
 =======================================================================================================================
 */

void GSP_IniModule()
{
}

/*
 =======================================================================================================================
    Aim:    Main init
 =======================================================================================================================
 */
 
void Gsp_DepthBlur(int OnOff , float ZStart ,float ZEnd)
{
}


/*$T Gx8_Menus.c GC!  */
//#define PJ_VERSION
/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "GDInterface/GDInterface.h"
#include "GX8/RASter/Gx8_Bench.h"
#include "INOut/INOjoystick.h"
#include "GX8/RASter/Gx8_Ras_Def.h"
#include <d3d8.h>
#include <d3dx8.h>
#include "Gx8\Gx8init.h"
#include "Gx8\Gx8.h"
#include "STRing/STRstruct.h"
#ifndef __VERTEXSHADERS_H__
#include "GX8\Gx8VertexShaders.h"
#endif

#include "Gx8\Gx8renderstate.h"

#include "Sdk\Sources\TIMer\PROfiler\XBox\PRO_xb.h"

#ifndef _AFTERFX_DEF_H_
#include "Gx8\AfterFX\Gx8AfterFX_Def.h"
#endif

#include "Gx8_CheatFlags.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

extern u_int gAE_Status;
extern GSP_AfterEffectParams gAE_Params;
extern int nearAlphaAdjustment;
extern BOOL Normalmap;
extern BOOL flag_MemorySpy;

int gi_flag_cheat_invincible = 0;

#if defined( USE_DOUBLE_RENDERING )
    extern ULONG    ENG_gp_DoubleRendering;
#endif

    
#define Gsp_Stp_GetLenght(sz) strlen(sz)
#define C_XSIZE 512
#define C_YSIZE 400
//#define u_int32 g_iNoMDF NoMDF
enum
{
    GSP_STP_ID_RATIO,//TV Mode

	GSP_STP_ID_NULL,
    GSP_STP_ID_RASTER,
	GSP_STP_ID_MEMORYSPY,
    GSP_STP_ID_RASTER_FILTER,
    GSP_STP_ID_RASTER_FUR,
	GSP_STP_ID_RASTER_SORTED,
    GSP_STP_ID_EXIT,
    GSP_STP_ID_RASTER_Prop,
    GSP_STP_ID_RASTER_Threshold,
    GSP_STP_ID_DONTUV,
    GSP_STP_ID_MATDONTDRAW,
	GSP_STP_ID_NORMALMAP,
    GSP_STP_ID_NO_MULTI,
    GSP_STP_ID_GEODONTDRAW,
    GSP_STP_ID_NO_SPR,
    GSP_STP_ID_NO_ZLST,
    GSP_STP_ID_NO_SKN,


	GSP_STP_ID_NEW_LIGHT,


    GSP_STP_ID_STR_ON,
    GSP_STP_ID_MDF_ON,
    GSP_STP_ID_MDF2_ON,
	GSP_STP_ID_FUR_ON,
	GSP_STP_ID_FurMethod,
    GSP_STP_ID_NO_GFX,
    GSP_STP_ID_AE_SHEETS,
    GSP_STP_ID_NO_SDW,
    GSP_STP_ID_FOG_ON,
    GSP_STP_ID_AA_ON,
    GSP_STP_ID_DEBUGSHADOW_ON,
    GSP_STP_ID_DSHADOW_COUNT,
    GSP_STP_ID_AE_BrightNess,
    GSP_STP_ID_AE_Contrast,
    GSP_STP_ID_CB_INTENSITY,
    GSP_STP_ID_CB_SPECTRE,
    GSP_STP_ID_AE_MODECOMP,
    GSP_STP_ID_AE_ALLOFF,
    GSP_STP_ID_AE_MB,    
    GSP_STP_ID_AE_SMSM,    
    GSP_STP_ID_AE_BW,    
    GSP_STP_ID_AE_ZS,    
    GSP_STP_ID_AE_RS,    
    GSP_STP_ID_AE_REMANANCE,    
    GSP_STP_ID_AE_SMOOTH,    
    GSP_STP_ID_AE_FADE,    
    GSP_STP_ID_NO_AMBIENCE,
    GSP_STP_ID_NO_DIALOGS,
    GSP_STP_ID_NO_STREAM,
    GSP_STP_ID_MIPMAP,
    GSP_STP_ID_2DXPOS,
    GSP_STP_ID_2DYPOS,
    GSP_STP_ID_2DFFX,
    GSP_STP_ID_2DFFY,
	GSP_STP_ID_FRAME_RATE_ALARM,
	GSP_STP_ID_FRAME_RATE_NUMBER,
    GSP_STP_ID_SHADOWBUFFER,
    GSP_STP_ID_NOFSHADOWBUFFER,
    GSP_STP_ID_AE_DB,
    GSP_STP_ID_AE_DBN,
    GSP_STP_ID_AE_DBN_FAR,
    GSP_STP_ID_AE_DB_FAR,
    GSP_STP_ID_AE_DB_ALPHA_ADJUST,
#if defined( FAST_CAP )
    GSP_STP_ID_ENABLE_FASTCAP,
#endif
#if defined( USE_DOUBLE_RENDERING )
    GSP_STP_ID_DOUBLE_RENDERING
#endif
};

extern Gx8_tdst_SpecificData *p_gGx8SpecificData;

u_int Status=0;

float fAddXAA = 0.0f; // ?????
float fAddYAA = 0.0f;
static u_int Depth = 0;
static int DepthStack[12];
u_int32 RasterFilter = 2;
u_int32 RasterMode = 0x10000;
u_int32 g_iNoUV = 0;
u_int32 g_iNoGEODRAW = 0;
u_int32 g_iNoMATDRAW = 0;
u_int32 g_iNoMulti = 0;
u_int32 g_iNoSTR = 0;
u_int32 NoMDF = 0;
u_int32 NoMDF2 = 0;
u_int32 g_iNoSPR = 0;
u_int32 g_iNoGFX = 0;
u_int32 g_iNoZLST = 0;
u_int32 g_iNoSKN = 0;
u_int32 g_iNoSDW = 0;
u_int32 g_iNoFog = 0;
u_int32 g_iAA = 1;
u_int32 NoFUR = 0;
u_int32 FurMethod = 0;
u_int32 bNoAE = 0;
//u_int32 bTest = 0;
u_int32 g_iNewLight =  1;//1 default

// TV Options
u_int32 TVMode = 1;//2=16/9

//
int g_iNumShadow = 0;
u_int32 g_iDDShadow = 0;
int g_FrameRateNumber = 1;

bool g_bUseAmbience = true;
bool g_bUseDialogs = true;
bool g_bUseStream = true;

float g_MipMapLODBias = -0.6f;
float g_FrameRateAlarmLimit = 30.0f;

bool	bShadowBufferCheat=false;
int		iNOfShadowBufferCheat=1;

#if defined( FAST_CAP )
bool g_FastCapEnabled = false;
bool g_FastCapEnabled_Stop = false;
bool g_FastCapEnabled_Start = false;
#endif

static u_int g_NoOpenCloseCounter = 0;
static u_int g_DontForceNoOpenCloseCounter = 1;

typedef struct Gsp_Stp_OneLeaf_
{
	char  Name[32];
	u_int Hlight;
	u_int Open;
	int Selection;
	u_int ID;
	struct Gsp_SetupMenu_ * p_SubMenu;
	int DefaultSelection;
} Gsp_Stp_OneLeaf;

void Gsp_Validate(struct Gsp_Stp_OneLeaf_ *p_OL , u_int Selection , u_int SetMenu);

typedef struct Gsp_SetupMenu_
{	
	unsigned char Title[64];
	u_int NumberOfLeaves;
	u_int OpenSize;
	u_int AutoSelection;
	Gsp_Stp_OneLeaf Leaves[32];
} Gsp_SetupMenu;

// déclaration des menus
static Gsp_SetupMenu Gsp_Stp_PlusMoin =
	{
		"",
		0,0,1,
		{
			{"-",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"  On   ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"+",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};

static Gsp_SetupMenu Gsp_Stp_OnOff = 
	{
		"",
		0,0,1,
		{
			{"NO",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"YES",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};

static Gsp_SetupMenu Gsp_Stp_ColorBalance =
	{
		"",
		0,0,0,
		{
			{"Intensity",0,0,0,GSP_STP_ID_CB_INTENSITY,&Gsp_Stp_PlusMoin,-1},
			{"Spectre",0,0,0,GSP_STP_ID_CB_SPECTRE,&Gsp_Stp_PlusMoin,-1},
//			{"Pastel",0,0,0,GSP_STP_ID_CB_PASTEL,&Gsp_Stp_0_To_100,-1},
//			{"Photo filter",0,0,0,c,&Gsp_Stp_0_To_100,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
static Gsp_SetupMenu Gsp_Stp_AE =
	{
		"",
		0,0,0,
		{
			{"No After Effect",0,0,0,GSP_STP_ID_AE_ALLOFF,&Gsp_Stp_OnOff,-1},
			{"Motion blur",0,0,0,GSP_STP_ID_AE_MB,&Gsp_Stp_PlusMoin,-1},
            {"Depth bluring",0,0,0,GSP_STP_ID_AE_DB,&Gsp_Stp_OnOff,-1},
            {"Depth bluring far",0,0,0,GSP_STP_ID_AE_DB_FAR,&Gsp_Stp_PlusMoin,-1},
            {"Near bluring",0,0,0,GSP_STP_ID_AE_DBN,&Gsp_Stp_OnOff,-1},
            {"Near bluring far",0,0,0,GSP_STP_ID_AE_DBN_FAR,&Gsp_Stp_PlusMoin,-1},
           // {"Depth bluring Alpha Adjust",0,0,0,GSP_STP_ID_AE_DB_ALPHA_ADJUST,&Gsp_Stp_PlusMoin,-1},
			{"Smooth",0,0,0,GSP_STP_ID_AE_SMOOTH,&Gsp_Stp_PlusMoin,-1},
			{"Motion smooth",0,0,0,GSP_STP_ID_AE_SMSM,&Gsp_Stp_OnOff,-1},
/*
			{"Emboss",0,0,0,GSP_STP_ID_AE_EMB,&Gsp_Stp_0_To_100,-1},
			{"Show shadow buffer",0,0,0,GSP_STP_ID_AE_SHOWSDW,&Gsp_Stp_OnOff,-1},
*/
			{"Zoom smooth",0,0,0,GSP_STP_ID_AE_ZS,&Gsp_Stp_PlusMoin,-1},
			{"Rotate smooth",0,0,0,GSP_STP_ID_AE_RS,&Gsp_Stp_PlusMoin,-1},
			{"Remanance",0,0,0,GSP_STP_ID_AE_REMANANCE,&Gsp_Stp_PlusMoin,-1},
			{"Brightness",0,0,0,GSP_STP_ID_AE_BrightNess,&Gsp_Stp_PlusMoin,-1},
			{"Contrast",0,0,0,GSP_STP_ID_AE_Contrast,&Gsp_Stp_PlusMoin,-1},
			{"Color balance",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_ColorBalance,-1},
/*			
			{"Glow mode",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_GlowMode,-1},
*/
			{"BW",0,0,0,GSP_STP_ID_AE_BW,&Gsp_Stp_PlusMoin,-1},
			{"Fade",0,0,0,GSP_STP_ID_AE_FADE,&Gsp_Stp_OnOff,-1},
/*
			{"Blur 22",0,0,0,GSP_STP_ID_AE_BlurTest44,&Gsp_Stp_BlurMode,-1},
			{"Water ref",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_WaterReflection,-1},
*/
//			{"Mode compare",0,0,0,GSP_STP_ID_AE_MODECOMP,&Gsp_Stp_OnOff,-1},
/*			
			{"AE report",0,0,0,GSP_STP_ID_MAR,&Gsp_Stp_OnOff,-1},
*/
			{"",0,0,0,0,NULL}
		}
	};

static Gsp_SetupMenu Gsp_Stp_RasterFilter =
	{
		"",
		0,0,1,
		{
			{"All",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Engine Only",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Graphic Only",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Global Only",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};

static Gsp_SetupMenu Gsp_Stp_Rasters =
	{
		"",
		0,0,0,
		{
			{"Rasters Memory Spy",0,0,0,GSP_STP_ID_MEMORYSPY,&Gsp_Stp_OnOff,-1},
			{"Rasters",0,0,0,GSP_STP_ID_RASTER,&Gsp_Stp_OnOff,-1},
			{"Raster unsorted",0,0,0,GSP_STP_ID_RASTER_SORTED,&Gsp_Stp_OnOff,-1},
			{"Raster filter",0,0,0,GSP_STP_ID_RASTER_FILTER,&Gsp_Stp_RasterFilter,-1},
			{"Rasters Prop",0,0,0,GSP_STP_ID_RASTER_Prop,&Gsp_Stp_OnOff,-1},
			{"Rasters Threshold",0,0,0,GSP_STP_ID_RASTER_Threshold,&Gsp_Stp_OnOff,-1},
			{"Frame Rate Alarm",0,0,0,GSP_STP_ID_FRAME_RATE_ALARM,&Gsp_Stp_PlusMoin,-1},
			{"Frame Rate Number",0,0,0,GSP_STP_ID_FRAME_RATE_NUMBER,&Gsp_Stp_OnOff,-1},
			{"",0,0,0,0,NULL}
		}
	};
static Gsp_SetupMenu Gsp_Stp_AntiAlias =
	{
		"",
		0,0,1,
		{
			{"None",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"2MultiSamp_Q",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"4MultiSamp_L",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
//			{"4SuperSamp_G",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
//			{"9MultiSamp_G",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};

static Gsp_SetupMenu Gsp_Stp_FurMethod =
{
    "",
    0,0,1,
    {
        {"Vertex Shader",0,0,0,GSP_STP_ID_NULL,NULL,-1},
        {"Begin/End",0,0,0,GSP_STP_ID_NULL,NULL,-1 },
        {"",0,0,0,0,NULL}
    }
};
static Gsp_SetupMenu Gsp_Stp_Fur =
	{
		"",
		0,0,1,
		{
			{"No Fur",0,0,0,GSP_STP_ID_FUR_ON,&Gsp_Stp_OnOff,-1},
			{"Fur method",0,0,0,GSP_STP_ID_FurMethod,&Gsp_Stp_FurMethod,-1},
			{"",0,0,0,0,NULL}
		}
	};

static Gsp_SetupMenu Gsp_Stp_3D =
	{
		"",
		0,0,0,
		{
//			{"2D",0,0,0,GSP_STP_ID_NO2D,&Gsp_Stp_OnOff,-1},
			{"Don't MATDraw",0,0,0,GSP_STP_ID_MATDONTDRAW,&Gsp_Stp_OnOff,-1},
			{"Normal Map",0,0,0,GSP_STP_ID_NORMALMAP,&Gsp_Stp_OnOff,-1},
			{"Don't Compute UV",0,0,0,GSP_STP_ID_DONTUV,&Gsp_Stp_OnOff,-1},
//			{"Don't GO Draw",0,0,0,GSP_STP_ID_GODONTDRAW,&Gsp_Stp_OnOff,-1},
//			{"Don't Light",0,0,0,GSP_STP_ID_NOLIGHT,&Gsp_Stp_OnOff,-1},
//			{"Set RS",0,0,0,GSP_STP_ID_SETRENDERSTATE,&Gsp_Stp_OnOff,-1},
//			{"Don't draw",0,0,0,GSP_STP_ID_DONTDRAW,&Gsp_Stp_OnOff,-1},
			{"No texture multi-pass",0,0,0,GSP_STP_ID_NO_MULTI,&Gsp_Stp_OnOff,-1},
			{"No shadows",0,0,0,GSP_STP_ID_NO_SDW,&Gsp_Stp_OnOff,-1},
			{"No texture sprites",0,0,0,GSP_STP_ID_NO_SPR,&Gsp_Stp_OnOff,-1},
			{"No GFX",0,0,0,GSP_STP_ID_NO_GFX,&Gsp_Stp_OnOff,-1},
			{"No Z liste",0,0,0,GSP_STP_ID_NO_ZLST,&Gsp_Stp_OnOff,-1},
			{"No Skin",0,0,0,GSP_STP_ID_NO_SKN,&Gsp_Stp_OnOff,-1},


			{"New Light",0,0,0,GSP_STP_ID_NEW_LIGHT,&Gsp_Stp_OnOff,-1},


//			{"No Add mats",0,0,0,GSP_STP_ID_NO_ADM,&Gsp_Stp_OnOff,-1},
//			{"No Particules",0,0,0,GSP_STP_ID_PAG_ON,&Gsp_Stp_OnOff,-1},
			{"No strings",0,0,0,GSP_STP_ID_STR_ON,&Gsp_Stp_OnOff,-1},
			{"No ALL modifiers",0,0,0,GSP_STP_ID_MDF_ON,&Gsp_Stp_OnOff,-1},
			{"No modifiers2",0,0,0,GSP_STP_ID_MDF2_ON,&Gsp_Stp_OnOff,-1},
			//{"No Fur",0,0,0,GSP_STP_ID_FUR_ON,&Gsp_Stp_OnOff,-1},
			{"Fur",0,0,0,GSP_STP_ID_RASTER_FUR,&Gsp_Stp_Fur,-1},
			{"No fog",0,0,0,GSP_STP_ID_FOG_ON,&Gsp_Stp_OnOff,-1},
			{"AntiAlias",0,0,0,GSP_STP_ID_AA_ON,&Gsp_Stp_AntiAlias,-1},
			{"MipMap LODBias",0,0,0,GSP_STP_ID_MIPMAP,&Gsp_Stp_PlusMoin,-1},
			{"",0,0,0,0,NULL}
		}
	};

static Gsp_SetupMenu Gsp_Stp_Debug =
	{
		"",
		0,0,0,
		{
#if defined( USE_DOUBLE_RENDERING )
            {"DoubleRendering",0,0,0,GSP_STP_ID_DOUBLE_RENDERING,&Gsp_Stp_OnOff, -1},
#endif
			{"DisplayShadow",0,0,0,GSP_STP_ID_DEBUGSHADOW_ON,&Gsp_Stp_OnOff,-1},
			{"SelectShadowNumber",0,0,0,GSP_STP_ID_DSHADOW_COUNT,&Gsp_Stp_PlusMoin,-1},
			{"ShadowBuffer",0,0,0,GSP_STP_ID_SHADOWBUFFER,&Gsp_Stp_OnOff,-1},	
			{"NOfShadowBufferLights",0,0,0,GSP_STP_ID_NOFSHADOWBUFFER,&Gsp_Stp_PlusMoin,-1},	
			{"",0,0,0,0,NULL}
		}
	};

static Gsp_SetupMenu Gsp_Stp_SheetCodes =
	{
		"",
#ifndef PJ_VERSION
		0,0,0,
#else
		0,0,1,
#endif
		{
			{"Normal",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
#ifndef PJ_VERSION
			{"map menu",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
#endif
			{"Invincible",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Fly mode",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	

static Gsp_SetupMenu Gsp_Stp_Audio =
{
    "",
    0,0,0,
    {
        {"Load Ambience",0,0,0,GSP_STP_ID_NO_AMBIENCE,&Gsp_Stp_OnOff,-1},
        {"Load Dialogs",0,0,0,GSP_STP_ID_NO_DIALOGS,&Gsp_Stp_OnOff,-1 },
        {"Load Music",0,0,0,GSP_STP_ID_NO_STREAM,&Gsp_Stp_OnOff,-1},
        {"",0,0,0,0,NULL}
    }
};
/*
static Gsp_SetupMenu Gsp_Stp_RasterFilter =
	{
		"",
		0,0,1,
		{
			{"All",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Engine Only",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Graphic Only",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Global Only",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};

static Gsp_SetupMenu Gsp_Stp_Rasters =
	{
		"",
		0,0,0,
		{
			{"Rasters",0,0,0,GSP_STP_ID_RASTER,&Gsp_Stp_OnOff,-1},
			{"Raster unsorted",0,0,0,GSP_STP_ID_RASTER_SORTED,&Gsp_Stp_OnOff,-1},
			{"Raster filter",0,0,0,GSP_STP_ID_RASTER_FILTER,&Gsp_Stp_RasterFilter,-1},
			{"Rasters Prop",0,0,0,GSP_STP_ID_RASTER_Prop,&Gsp_Stp_OnOff,-1},
			{"Rasters Threshold",0,0,0,GSP_STP_ID_RASTER_Threshold,&Gsp_Stp_OnOff,-1},
			{"Frame Rate Alarm",0,0,0,GSP_STP_ID_FRAME_RATE_ALARM,&Gsp_Stp_PlusMoin,-1},
			{"Frame Rate Number",0,0,0,GSP_STP_ID_FRAME_RATE_NUMBER,&Gsp_Stp_OnOff,-1},
			{"",0,0,0,0,NULL}
		}
	};
*/

static Gsp_SetupMenu Gsp_Stp_RATIO =
{
    "",
    0,0,0,
    {
        {"4/3",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
        {"4/3 Black Band",0,0,-1,GSP_STP_ID_NULL,NULL,-1 },
        {"16/9 WideScreen",0,0,-1,GSP_STP_ID_NULL,NULL,-1 },
        {"16/9 Anamorphic",0,0,-1,GSP_STP_ID_NULL,NULL,-1 },
 /*       {"720p (1280x720)",0,0,-1,GSP_STP_ID_NULL,NULL,-1 },
        {"720p (1280x720) WideScreen",0,0,-1,GSP_STP_ID_NULL,NULL,-1 },
        {"720p (1280x720) Anamorphic",0,0,-1,GSP_STP_ID_NULL,NULL,-1 },
        {"720 test",0,0,-1,GSP_STP_ID_NULL,NULL,-1 },*/
        {"",0,0,0,0,NULL}
    }
};


static Gsp_SetupMenu Gsp_Stp_Video =
{
    "",
    0,0,0,
    {
/*
        {"Colored VSync",0,0,0,GSP_STP_ID_SHOWVSYNC,&Gsp_Stp_OnOff,-1},
        {"Resolution H",0,0,0,GSP_STP_ID_HRES,&Gsp_Stp_ResolutionH,-1},
        {"Resolution V",0,0,0,GSP_STP_ID_VRES,&Gsp_Stp_ResolutionV,-1},
        {"Resolution Bpp",0,0,0,GSP_STP_ID_CBPP,&Gsp_Stp_Bpp,-1},
        {"Zbuffer Bpp",0,0,0,GSP_STP_ID_ZBPP,&Gsp_Stp_Bpp,-1},
        {"Pal/Ntsc",0,0,0,GSP_STP_ID_PALNTSC,&Gsp_Stp_PalNtsc,-1},
        {"16 / 9",0,0,0,GSP_STP_ID_16_9,&Gsp_Stp_OnOff,-1},
        {"Gounzafiot Y 400",0,0,0,GSP_STP_ID_GZFY,&Gsp_Stp_Gounzafiot,-1},
        {"CRTC vertical AA",0,0,0,GSP_STP_ID_CRTC_VAA,&Gsp_Stp_OnOff,-1},
        {"VSYNC stablity",0,0,0,GSP_STP_ID_SYNCSTABILITY,&Gsp_Stp_0_To_100,-1},
        {"VIDEO HRepos",0,0,0,GSP_STP_ID_VHREPOS,&Gsp_Stp_PlusMoin,-1},
        {"VIDEO 2D X pos",0,0,0,GSP_STP_ID_2DXPOS,&Gsp_Stp_PlusMoin,-1},
        {"VIDEO 2D Y pos",0,0,0,GSP_STP_ID_2DYPOS,&Gsp_Stp_PlusMoin,-1},
 */
        {"VIDEO 2D FFX",0,0,0,GSP_STP_ID_2DFFX,&Gsp_Stp_PlusMoin,-1},
        {"VIDEO 2D FFY",0,0,0,GSP_STP_ID_2DFFY,&Gsp_Stp_PlusMoin,-1},
        {"",0,0,0,0,NULL}
    }
};

static Gsp_SetupMenu Gsp_Stp_Root =
	{
		"Jade version",
		0,0,0,
		{
			{"TV Options",0,0,0,GSP_STP_ID_RATIO,&Gsp_Stp_RATIO,-1},
			{"Cheat Codes",0,0,0,GSP_STP_ID_AE_SHEETS,&Gsp_Stp_SheetCodes,-1},
#ifndef PJ_VERSION
            {"Video",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_Video,-1},
			{"3D",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_3D,-1},
#if defined( FAST_CAP )
            {"FastCap",0,0,1,GSP_STP_ID_ENABLE_FASTCAP,&Gsp_Stp_OnOff, -1 },
#endif
			{"Rasters",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_Rasters,-1},
			{"After FX",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_AE,-1},
			{"Debug",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_Debug,-1},
            {"Audio",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_Audio,-1},
#endif
			{"",0,0,0,0,NULL}
		}
	};

/* pour les cheat codes */
extern unsigned char ucVirtualPressedKey[3];
int iVirtualPressedKeyTime=0;

void Gsp_FisrtInitMenu(Gsp_SetupMenu *p_Root)
{
	u_int LC;
	p_Root -> NumberOfLeaves = 0;
	while (p_Root -> Leaves[p_Root -> NumberOfLeaves].Name[0] != 0)
		p_Root -> NumberOfLeaves ++;
	LC = p_Root -> NumberOfLeaves;
	while (LC--)
	{
		if (p_Root -> Leaves[LC].p_SubMenu) 
		 Gsp_FisrtInitMenu(p_Root -> Leaves[LC].p_SubMenu);
	}
}

void Gsp_InitMenu(Gsp_SetupMenu *p_Root)
{
	u_int LC;
	for (LC = 0 ; LC < p_Root -> NumberOfLeaves ; LC ++)
	{
		Gsp_Validate(&p_Root -> Leaves[LC] , 0 , 1);
		if (p_Root -> Leaves[LC].p_SubMenu)
		{
		if (p_Root->Leaves[LC].Open)
			Gsp_InitMenu(p_Root -> Leaves[LC].p_SubMenu);
			}
	}
}

void Gsp_Stp_MenuClose(Gsp_SetupMenu *Root)
{
	u_int LC;
	for (LC = 0 ; LC < Root -> NumberOfLeaves ; LC++)
	{
		Root->Leaves[LC].Open = 0;
		Root->Leaves[LC].Hlight = 0;
		if (Root->Leaves[LC].p_SubMenu)
			Gsp_Stp_MenuClose(Root->Leaves[LC].p_SubMenu);
	}
}

/***********************************************
		DRAW
************************************************/
void Gsp_Stp_Draw(LPDIRECT3DDEVICE8 Device,Gsp_SetupMenu *p_Root , int X , int Y,int bIsSelection ,int bIsDefaultSelection)
{
#define SETUP_BigX 40
#define SETUP_BigY 20
#define C_ZOOM 0.7f
	ULONG ulBlendingMode=0;
	WCHAR   asz_Temp[512];
	u_int LC , Max;
	u_int Color;
	u_int ColorHL;
	char String[64];
	Max = 0;
//	Gsp_Flush(FLUSH_ALL);
	p_Root->OpenSize += 4;
/*	Depest_X = X;
	Depest_Y = Y;*/
	
	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	if (Max < Gsp_Stp_GetLenght(p_Root->Leaves[LC].Name))
	{
		Max = Gsp_Stp_GetLenght(p_Root->Leaves[LC].Name);
	}
	if (Max < Gsp_Stp_GetLenght( (const char*) p_Root->Title))
	{
		Max = Gsp_Stp_GetLenght( (const char*) p_Root->Title);
	}
	if (Max > p_Root->OpenSize) Max = p_Root->OpenSize;
	Max+=3;
	LC = Max;
	String[LC] = 0;
	if (bIsSelection != -1)
		Color = 0x30303000;
	else
		Color = 0x30003030;
	while (LC--)
	{
		String[LC] = ' ';
	}
	if ((X + SETUP_BigX + Max * 8) > 512)
	{
		X = C_XSIZE - (Max * 8);
	}
	{
		ULONG OPENC,ISOP;
		ISOP = 0xffffffff;
		OPENC = p_Root ->NumberOfLeaves;
		while (OPENC--) if (p_Root->Leaves[OPENC].Hlight || p_Root->Leaves[OPENC].Open) ISOP = OPENC;
		if (ISOP != 0xffffffff)
		{
			if ((SETUP_BigY + Y + (ISOP + 2) * 16) > (C_YSIZE /*- 2 * GspGlobal_ACCESS(Morfling)*/))
			{
				Y -= SETUP_BigY + Y + (ISOP + 2) * 16 - C_YSIZE /*+ 2 * GspGlobal_ACCESS(Morfling)*/;
			}
		}
	}
	//Y += GspGlobal_ACCESS(Morfling);
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode,0);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);
    Gx8_RS_CullFace(p_gGx8SpecificData, D3DCULL_CCW);

	IDirect3DDevice8_SetTexture(Device,0,NULL);

	if (p_Root->Title[0] != 0) 
	{
		Gx8_DrawRectangle((void *)Device,(float)(SETUP_BigX + X), (float)(SETUP_BigY + Y), (float)Max*8, 
				(float)(p_Root ->NumberOfLeaves*16+16),0x707070ff);

	    Gx8_SetRastersRenderState();
		swprintf (asz_Temp, L"%S", p_Root->Title);
		XB_fn_vDrawColoredZoomText (SETUP_BigX + X + 8, SETUP_BigY + Y, 0x8f7070ff, C_ZOOM , asz_Temp);
		Y += 16;
		//GSP_DrawTExt(SETUP_BigX + X    ,SETUP_BigY + Y,String,			0,0,GST_COPY,GST_ALPHA);
		//GSP_DrawTExt(SETUP_BigX + X + 8,SETUP_BigY + Y,p_Root->Title	,	0x7070ff,0,GST_COPY,GST_SUB);
	}
	else
	{
		Gx8_DrawRectangle((void *)Device,(float)(SETUP_BigX + X), (float)(SETUP_BigY + Y), (float)(Max*8),
				(float)(p_Root ->NumberOfLeaves*16),0x707070ff);
		Gx8_SetRastersRenderState();
	}


	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	{
		u_int SC;
		char SaveSM;
		char SaveSMOnOf;
		SC = Color;
		
		if (LC == (u_int)bIsSelection)
		{
			String[0] = '>';
			String[Max - 1] = '<';
		} 
		String[Max] = 0;
		SaveSM = p_Root->Leaves[LC].Name[Max];
		SaveSMOnOf = p_Root->Leaves[LC].Name[Max - 1];
		p_Root->Leaves[LC].Name[Max] = 0;
		if ((p_Root->Leaves[LC].p_SubMenu) && (p_Root->Leaves[LC].p_SubMenu == &Gsp_Stp_OnOff))
		{
			if (p_Root->Leaves[LC].Selection)
				String[Max - 1] = 'y';
			else
				String[Max - 1] = 'n';
		} 
		ColorHL = Color ^ 0xffffff;
		if ((p_Root->Leaves[LC].Selection != p_Root->Leaves[LC].DefaultSelection) && (p_Root->Leaves[LC].DefaultSelection != -1))
		{
			String[0] = '*';
		}
		if (p_Root->Leaves[LC].Hlight || p_Root->Leaves[LC].Open) 
		{
			u_int SWPO;
			SWPO = Color;
			Color = ColorHL;
			ColorHL = SWPO; 
		}
		{
/*			swprintf (asz_Temp, L"%S", String);
			XB_fn_vDrawColoredZoomText (SETUP_BigX + X, SETUP_BigY + Y+LC * 16, 0xff000000|Color, C_ZOOM , asz_Temp);*/
			if (String[0]!=' ')
			{
				String[1]=0;
				swprintf (asz_Temp, L"%S", String);
				XB_fn_vDrawColoredZoomText (SETUP_BigX + X, SETUP_BigY + Y+LC * 16, 0xff000000|Color, C_ZOOM , asz_Temp);
			}
			if (String[Max - 1]!=' ')
			{
				swprintf (asz_Temp, L"%S", &String[Max - 1]);
				XB_fn_vDrawColoredZoomText (SETUP_BigX + X + (Max-1)*8, SETUP_BigY + Y+LC * 16, 0xff000000|Color, C_ZOOM , asz_Temp);
			}
			swprintf (asz_Temp, L"%S", p_Root->Leaves[LC].Name);
			XB_fn_vDrawColoredZoomText (SETUP_BigX + X + 8, SETUP_BigY + Y+LC * 16, 0xff000000|Color, C_ZOOM , asz_Temp);
			//XB_fn_vDrawText (SETUP_BigX + X + 8, SETUP_BigY + Y+LC * 16,  asz_Temp);

//			GSP_DrawTExt(SETUP_BigX + X    ,SETUP_BigY + Y+LC * 16,String,			Color,ColorHL,GST_COPY,GST_ALPHA);
//			GSP_DrawTExt(SETUP_BigX + X + 8,SETUP_BigY + Y+LC * 16,p_Root->Leaves[LC].Name,	Color,0,GST_COPY,GST_SUB);
		}
		String[Max - 1] = ' ';
		Color = SC;
		String[0] = ' ';
		String[Max - 1] = ' ';
		String[Max] = ' ';
		p_Root->Leaves[LC].Name[Max] = SaveSM;
		p_Root->Leaves[LC].Name[Max - 1] = SaveSMOnOf;
	}
	//Y -= GspGlobal_ACCESS(Morfling);

	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	{
		if (p_Root->Leaves[LC].p_SubMenu)
		{
			if (p_Root->Leaves[LC].Open)
			{
				Gsp_Stp_Draw(Device,p_Root->Leaves[LC].p_SubMenu, X+ Max * 8 + 4, Y+LC * 16 , p_Root->Leaves[LC].Selection , p_Root->Leaves[LC].DefaultSelection);
			} else
				p_Root->Leaves[LC].p_SubMenu->OpenSize = 0;
			
		}
	}

//


}

Gsp_SetupMenu *Gsp_Stp_GetMenu(Gsp_SetupMenu *Root , u_int DepthL , u_int *DepthStackL)
{
	Gsp_SetupMenu *Ret;
	Ret = Root;
	while ((DepthL--) && Ret)
	{
		Ret = Ret->Leaves[*(DepthStackL++)].p_SubMenu;
	}
	return Ret;
}

void Gsp_DefaultSet1(Gsp_SetupMenu *p_Root)
{
	u_int LC;
	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	{
		if (p_Root->Leaves[LC].DefaultSelection == -2) 
			p_Root->Leaves[LC].DefaultSelection = -1;
		if (p_Root->Leaves[LC].p_SubMenu)
		{
			Gsp_DefaultSet1(p_Root->Leaves[LC].p_SubMenu);
		}
	}
}

u_int Gsp_DefaultSet2(Gsp_SetupMenu *p_Root)
{
	u_int LC , Setted , AtLeastOne;
	AtLeastOne = 0;
	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	{
		Setted = 0; 
		if ((p_Root->Leaves[LC].p_SubMenu) && (p_Root->Leaves[LC].Selection == -1))
		{
			Setted = Gsp_DefaultSet2(p_Root->Leaves[LC].p_SubMenu);
		}
		if ((p_Root->Leaves[LC].Selection != p_Root->Leaves[LC].DefaultSelection) && (p_Root->Leaves[LC].DefaultSelection != -1))
		{
			Setted = 1;
		}
		if ((p_Root->Leaves[LC].DefaultSelection == -1) && Setted)
			p_Root->Leaves[LC].DefaultSelection = -2;
		AtLeastOne |= Setted;
	}
	return AtLeastOne;
}

static void _doPadDown( Gsp_SetupMenu *CurrentMenu )
{
    DepthStack[Depth]++;
    DepthStack[Depth] %= CurrentMenu->NumberOfLeaves;
}

static void _doPadUp( Gsp_SetupMenu *CurrentMenu )
{
    if (DepthStack[Depth] == 0)
        DepthStack[Depth] = CurrentMenu->NumberOfLeaves - 1;
    else
        DepthStack[Depth]--;
}

#define PAD_QUIT        1
#define PAD_CONTINUE    0

static int
_handlePad( Gsp_SetupMenu* CurrentMenu, Gsp_Stp_OneLeaf *CurrentLeaf )
{
    static int repeat = 0;
    static const int REPEAT_TRIGGER = 10;

	if (INO_b_Joystick_IsButtonJustDown(e_DPadRight))
	{

		if (CurrentLeaf->p_SubMenu)
		{
			CurrentLeaf->Open = 1;
			g_NoOpenCloseCounter = 0;
			DepthStack[++Depth] = CurrentLeaf->Selection % CurrentMenu->NumberOfLeaves;
			if (CurrentLeaf->Selection == -1)
            {
                DepthStack[Depth] = 0;
            }
			else 
			{
			   Gsp_Validate(CurrentLeaf , 0 , 1);
			   DepthStack[Depth] = CurrentLeaf->Selection;
			}
		}

        return PAD_CONTINUE;
	}
    if (INO_b_Joystick_IsButtonJustDown(e_DPadLeft))
    {
        g_NoOpenCloseCounter = 0;
        CurrentLeaf->Open = 0;
        if (Depth != 0)
        {
            Depth --;
        }
        else
        {
            return PAD_QUIT;
        }
        CurrentMenu = Gsp_Stp_GetMenu(&Gsp_Stp_Root , Depth , (u_int*)DepthStack);
        CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
        CurrentLeaf->Open = 0;

        return PAD_CONTINUE;
    }
    if (INO_b_Joystick_IsButtonJustDown(e_DPadDown))
    {
        _doPadDown( CurrentMenu );
        repeat = 0;
        
        return PAD_CONTINUE;
    }
    else if( INO_b_Joystick_IsButtonDown(e_DPadDown))
    {
        repeat++;
        if( repeat >= REPEAT_TRIGGER )
        {
            _doPadDown( CurrentMenu );
            repeat = 0;
        }
        return PAD_CONTINUE;
    }

    if (INO_b_Joystick_IsButtonJustDown(e_DPadUp))
    {
        _doPadUp( CurrentMenu );
        repeat = 0;

        return PAD_CONTINUE;
    }
    else if( INO_b_Joystick_IsButtonDown(e_DPadUp))
    {
        repeat++;
        if( repeat >= REPEAT_TRIGGER )
        {
            _doPadUp( CurrentMenu );
            repeat = 0;
        }
        return PAD_CONTINUE;
    }

    return PAD_CONTINUE;
}
	
#ifndef _XBOX
// definition des touches !
#define e_ButtonX INO_JoyMask_C
#define e_ButtonB INO_JoyMask_B
#define e_DPadUp win32INO_l_Joystick_YUp
#define e_DPadDown win32INO_l_Joystick_YDown
#define e_DPadRight win32INO_l_Joystick_XRight
#define e_DPadLeft win32INO_l_Joystick_XLeft
#define e_LeftTrigger INO_JoyMask_L
#endif


/* appel toute les trames ? */
void Gsp_Setup(void *Dev)
{
	LPDIRECT3DDEVICE8 Device = (LPDIRECT3DDEVICE8)Dev;
	u_int paddata , bValidDetected ;
	static u_int CurrentFrameCounter = 0;
//	GSP_tdst_DualShock2 *P_DS2;
	Gsp_SetupMenu *CurrentMenu;
	Gsp_Stp_OneLeaf *CurrentLeaf;

	int iPadActivated = 0;
    
	//Note: 10 et 11 represente les 2 sticks PS2 definit dans INOjoystick.h
	//Eux meme active rightthumb et black de la XBOX dans INOjoystick.c

	if (INO_b_Joystick_IsButtonJustDown(10))//e_ButtonX//e_LeftThumb
	{
		if (INO_b_Joystick_IsButtonDown(11))//e_ButtonB//e_ButtonBlack
		{
			iPadActivated = 1;
		}
	}
	if (INO_b_Joystick_IsButtonJustDown(11))//e_ButtonB//e_ButtonBlack
	{
		if (INO_b_Joystick_IsButtonDown(10))//e_ButtonX//e_LeftThumb
		{
			iPadActivated = 1;
		}
	}

	if (iPadActivated) // toggle menus on/off
	{
		Status ^= GSP_Status_Setup_On;
	}

	if ((Status&GSP_Status_Setup_On)==0)
		return;

#if defined( FAST_CAP )
    if( g_FastCapEnabled && iPadActivated )
    {
		Status &= ~GSP_Status_Setup_On;
        g_FastCapEnabled_Stop = true;
        g_FastCapEnabled = false;
        return;
    }

    if( g_FastCapEnabled )
    {
		Status &= ~GSP_Status_Setup_On;
        return;
    }
#endif

/*
	{
		sprintf(Gsp_Stp_Root.Title ,"Jade version %d" , BIG_Cu4_AppVersion );
		sprintf(Gsp_Stp_Tool.Title ,"%dK / %dK tex mem used" , TextureUsed>>10 , GSP_GetTextureMemoryAvailable()>>10);
	}
*/

	paddata = 0;
//	MenuIsOpen = 1;

	if(iVirtualPressedKeyTime)
	{
	    iVirtualPressedKeyTime--;
	}
	else
	{
	    ucVirtualPressedKey[0] = ucVirtualPressedKey[1] =ucVirtualPressedKey[2]= 0;
	}
	fAddXAA = 0.0f;
	fAddYAA = 0.0f;

	/*ENG_gb_ForcePauseEngine = FALSE;
	GSP_gb_ForcePauseJoy = FALSE;*/
	
	Gsp_FisrtInitMenu(&Gsp_Stp_Root); // <= Extra SubCountering Xor Vlidate
//	if(scePadRead(0, 0, rdata) > 0) paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
/*	
	P_DS2 = (GSP_tdst_DualShock2 *)rdata;
	if ((paddataXor ^ paddata) != 0)
	{
		CurrentFrameCounter = GspGlobal_ACCESS(LastVBlankCounter);
	} else
	if (GspGlobal_ACCESS(LastVBlankCounter) - CurrentFrameCounter > 20)
	{
		gs_intensity = P_DS2->InfoD | P_DS2->InfoU | P_DS2->InfoU | P_DS2->InfoR;
		gs_intensity >>= 5;
		CurrentFrameCounter = GspGlobal_ACCESS(LastVBlankCounter) - 12 - gs_intensity;
		paddataXor = 0;
	}
	gs_intensity = lMax(lMax(P_DS2->InfoD , P_DS2->InfoU),1);
*/	
	
	bValidDetected = 0;
/*	paddataXor ^= paddata;
	paddataXor &= paddata;*/
	Gsp_InitMenu(&Gsp_Stp_Root);
	CurrentMenu = Gsp_Stp_GetMenu(&Gsp_Stp_Root , Depth , (u_int*)DepthStack);
	CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
	g_NoOpenCloseCounter ++;

    if( _handlePad( CurrentMenu, CurrentLeaf ) == PAD_QUIT )
    {
        goto SetupExit;
    }

    if (INO_b_Joystick_IsButtonJustDown(e_DPadRight) && !CurrentLeaf->p_SubMenu)
    {
        bValidDetected = 1;
    }

   	CurrentMenu = Gsp_Stp_GetMenu(&Gsp_Stp_Root , Depth , (u_int*)DepthStack);
	CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
	if (CurrentMenu->AutoSelection) 
		bValidDetected = 1;
	
/*	
	if (paddataXor & SCE_PADL1)
	{
		g_DontForceNoOpenCloseCounter ^= 1;
		g_NoOpenCloseCounter = 0;
	}*/
	if ((INO_b_Joystick_IsButtonJustDown(e_LeftTrigger))//left trigger pour valider
		|| bValidDetected)
	{
		if ((CurrentMenu->Leaves[DepthStack[Depth]].ID != GSP_STP_ID_NULL) && (CurrentMenu->Leaves[DepthStack[Depth]].Selection == -1))
		{
			if (CurrentMenu->Leaves[DepthStack[Depth]].ID == GSP_STP_ID_EXIT) goto SetupExit;
			Gsp_Validate(&CurrentMenu->Leaves[DepthStack[Depth]] , 0 , 0);
		}
		else
		if (Depth)
		{
			Gsp_SetupMenu *Previousmenu;
			Previousmenu = Gsp_Stp_GetMenu(&Gsp_Stp_Root , Depth - 1 , (u_int*)DepthStack);
			if (Previousmenu->Leaves[DepthStack[Depth-1]].Selection != -1)
			{
				Previousmenu->Leaves[DepthStack[Depth-1]].Selection = DepthStack[Depth];
				if (Previousmenu->Leaves[DepthStack[Depth - 1]].ID != GSP_STP_ID_NULL)
					Gsp_Validate(&Previousmenu->Leaves[DepthStack[Depth-1]] , Previousmenu->Leaves[DepthStack[Depth-1]].Selection , 0);
			}
		}
	}
	
	if (CurrentMenu == &Gsp_Stp_PlusMoin)
	{
		DepthStack[Depth] = 1;
	}
	
	
	CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
	CurrentLeaf -> Hlight = 1;
	
	Gsp_DefaultSet1(&Gsp_Stp_Root);
	Gsp_DefaultSet2(&Gsp_Stp_Root);
	Gsp_Stp_Draw(Device,&Gsp_Stp_Root ,10 , 10 , -1 , -1);
/*	if (g_DontForceNoOpenCloseCounter) g_NoOpenCloseCounter = 1000000;
	if(g_NoOpenCloseCounter > 400 / GspGlobal_ACCESS(fCurrentFrquency)) 
	{
		Depest_X = 20.0f - Depest_X;
		Depest_Y = 20.0f - Depest_Y;
	} else
	{
		Depest_X = 0.0f;
		Depest_Y = 0.0f;
	}
	{
		float fDX , fDY , fCoef;
		fDX = (Depest_X - SETUP_BigX);
		fDY = (Depest_Y - SETUP_BigY);
		fCoef = 0.0f;
		if(g_NoOpenCloseCounter > 400 / GspGlobal_ACCESS(fCurrentFrquency)) 
		{
			g_NoOpenCloseCounter = 100000;
			fCoef = 0.1f;
		}
		if (fDX > 0.0f) 
		{
			fCoef = 0.25f;
		}
		fCoef*=  GspGlobal_ACCESS(fCurrentFrquency);
		if (fCoef > 1.0f) fCoef = 1.0f;
		SETUP_BigX += fDX * fCoef;
		SETUP_BigY += fDY * fCoef;
	}*/
	CurrentLeaf -> Hlight = 0;
/*	paddataXor = paddata;
	if (AE_ON) AE_Report();*/
	return;
SetupExit:	
	g_DontForceNoOpenCloseCounter = 1;
	g_NoOpenCloseCounter = 0;
	Depth = 0;
	DepthStack[0] = 0;
/*	if (AE_ON) 	Status &= ~GSP_Status_Console_On;
	AE_ON = 0;
	PS2_MenuIsOpen = 0;*/
	Status &= ~GSP_Status_Setup_On;
	Gsp_Stp_MenuClose(&Gsp_Stp_Root);
}


#define TOGGLE_FLAG(Flag)               \
    {                                   \
        if( SetMenu )                   \
        {                               \
            p_OL->Selection = !(Flag);  \
        }                               \
        else                            \
        {                               \
            (Flag) = !p_OL->Selection;  \
        }                               \
    }

#define TOGGLE_NOT_FLAG(Flag)           \
    {                                   \
        if( SetMenu )                   \
        {                               \
            p_OL->Selection = (Flag);   \
        }                               \
        else                            \
        {                               \
            (Flag) = p_OL->Selection;   \
        }                               \
    }

#define INCDEC_VAR(X,Fmt,Step,Min,Max)                                                  \
    if( SetMenu )                                                                       \
    {                                                                                   \
        p_OL -> Selection = 1;                                                          \
        if (p_OL -> Open)                                                               \
        {                                                                               \
            sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , (Fmt) , Gx8Global_get(X));        \
        }                                                                               \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        if (Selection == 2)                                                             \
        {                                                                               \
            Gx8Global_set(X, Gx8Global_get(X) + (Step));                                \
        }                                                                               \
        if (Selection == 0)                                                             \
        {                                                                               \
            Gx8Global_set(X, Gx8Global_get(X) - (Step));                                \
        }                                                                               \
        Gx8Global_set(X, fMax((Min) , fMin((Max) , Gx8Global_get(X))));                 \
        p_OL -> Selection = 1;                                                          \
    }


/*******************************************************************
	TRAITEMENTS
*******************************************************************/
void Gsp_Validate(Gsp_Stp_OneLeaf *p_OL , u_int Selection , u_int SetMenu)
{
//	float *FTS;
//	float UP_LMT,DOWN_LMT;
	if ((p_OL->Selection == -1) && (SetMenu)) return; /* It's an action (EX SHOW VRAM) */
	switch (p_OL->ID)
	{
		case GSP_STP_ID_MEMORYSPY:
			if (SetMenu)
			{
				p_OL -> Selection = flag_MemorySpy; /* On */
			} else {
				flag_MemorySpy = p_OL -> Selection; /* On */
			}
			break;

		case GSP_STP_ID_RASTER_Prop:
			if (SetMenu)
			{
				if (Status & GSP_Status_Raster_Prop_On)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				Status &= ~GSP_Status_Raster_Prop_On;
				if (Selection) Status |= GSP_Status_Raster_Prop_On;
			}
			break;
		case GSP_STP_ID_RASTER_Threshold:
			if (SetMenu)
			{
				if (Status & GSP_Status_Raster_Threshold_On)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				Status &= ~GSP_Status_Raster_Threshold_On;
				if (Selection) Status |= GSP_Status_Raster_Threshold_On;
			}
			break;			
		case GSP_STP_ID_RASTER:
			if (SetMenu)
			{
				if (Status & GSP_Status_Raster_On)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				Status &= ~GSP_Status_Raster_On;
				if (Selection) 
					Status |= GSP_Status_Raster_On;
			}
			break;
		case GSP_STP_ID_RASTER_FILTER:
			if (SetMenu)
			{
				p_OL -> Selection = RasterFilter;
			} else
			{
				RasterFilter = p_OL -> Selection;
			}
			break;		
        
		case GSP_STP_ID_RASTER_SORTED:
			if (SetMenu)
			{
				p_OL -> Selection = RasterMode >> 16;
			} else
			{
				RasterMode &= ~(1 << 16);
				if (p_OL -> Selection)
					RasterMode |= 1 << 16;
			}
			break;		
        
			case GSP_STP_ID_FRAME_RATE_NUMBER:
			if (SetMenu)
			{
				p_OL -> Selection = g_FrameRateNumber;
			} else
			{
				g_FrameRateNumber = p_OL -> Selection;
			}
			break;		
        
		case GSP_STP_ID_FurMethod:
			if (SetMenu)
			{
				p_OL -> Selection = FurMethod; /* On */
			} else {
				FurMethod = p_OL -> Selection; /* On */
			}
			break;

		case GSP_STP_ID_DONTUV:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoUV; /* On */
			} else {
				g_iNoUV = p_OL -> Selection; /* On */
			}
			break;

		case GSP_STP_ID_GEODONTDRAW:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoGEODRAW; /* On */
			} else {
				g_iNoGEODRAW = p_OL -> Selection; /* On */
			}
			break;

		case GSP_STP_ID_MATDONTDRAW:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoMATDRAW; /* On */
			} else {
				g_iNoMATDRAW = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NORMALMAP:
			if (SetMenu)
			{
				p_OL -> Selection = Normalmap; /* On */
			} else {
				Normalmap = p_OL -> Selection; /* On */
			}
			break;


		case GSP_STP_ID_NO_MULTI:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoMulti; /* On */
			} else {
				g_iNoMulti = p_OL -> Selection; /* On */
			}
			break;

		case GSP_STP_ID_NO_SPR:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoSPR; /* On */
			} else {
				g_iNoSPR = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NO_GFX:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoGFX; /* On */
			} else {
				g_iNoGFX = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NO_ZLST:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoZLST; /* On */
			} else {
				g_iNoZLST = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NO_SKN:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoSKN; /* On */
			} else {
				g_iNoSKN = p_OL -> Selection; /* On */
			}
			break;





		case GSP_STP_ID_NEW_LIGHT:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNewLight; /* On */
			} else {
				g_iNewLight = p_OL -> Selection; /* On */
			}
			break;







		case GSP_STP_ID_STR_ON:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoSTR; /* On */
			} else {
				g_iNoSTR = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_MDF_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoMDF; /* On */
//			NoMDF=0;
			} else {
				NoMDF = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_MDF2_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoMDF2; /* On */
			} else {
				NoMDF2 = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_FUR_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoFUR; /* On */
			} else {
				NoFUR = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_FOG_ON:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoFog; /* On */
			} else {
				g_iNoFog = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_AA_ON:
			if (SetMenu)
			{
				p_OL -> Selection = g_iAA; /* On */
			} else {
				g_iAA = p_OL -> Selection; /* On */
				Gx8_fnvSetAntiAlias(g_iAA);
			}
			break;

		case GSP_STP_ID_MIPMAP:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , g_MipMapLODBias);		
				}
			} else
			{
				if (Selection == 2) g_MipMapLODBias += 0.05f;
				if (Selection == 0) g_MipMapLODBias -= 0.05f;
				g_MipMapLODBias = fMax(-1.0f , fMin(1.0f , g_MipMapLODBias ));
				
				p_OL -> Selection = 1;
			}
			break;

		case GSP_STP_ID_FRAME_RATE_ALARM:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.1f" , g_FrameRateAlarmLimit);
				}
			} else
			{
				if (Selection == 2) g_FrameRateAlarmLimit += 5.0f;
				if (Selection == 0) g_FrameRateAlarmLimit -= 5.0f;
				g_FrameRateAlarmLimit = fMax(10.0f , fMin(60.0f , g_FrameRateAlarmLimit ));
				
				p_OL -> Selection = 1;
			}
			break;

#if defined( USE_DOUBLE_RENDERING )
        case GSP_STP_ID_DOUBLE_RENDERING:
            TOGGLE_FLAG( ENG_gp_DoubleRendering );
            break;
#endif

		case GSP_STP_ID_DEBUGSHADOW_ON:
            TOGGLE_FLAG( g_iDDShadow );
			break;

		case GSP_STP_ID_DSHADOW_COUNT:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , g_iNumShadow);
				}
			} else
			{
				if (Selection == 2)
					if (g_iNumShadow<MaxShadowTexture-1)
					g_iNumShadow++;
				if (Selection == 0) 
					if (g_iNumShadow>0)
						g_iNumShadow--;
				p_OL -> Selection = 1;
			}
			break;

		case GSP_STP_ID_AE_MODECOMP:
			
			if (SetMenu)
			{
				if (gAE_Status & GSP_Status_AE_ModeCompare)
					p_OL -> Selection = 1; 
				else
					p_OL -> Selection = 0;
			} else {
				gAE_Status &= ~GSP_Status_AE_ModeCompare;
				if (Selection) 
				{	
					gAE_Status |= GSP_Status_AE_ModeCompare;
				}			
			}
			break;
		case GSP_STP_ID_AE_SMSM:
			if (SetMenu)
			{
				if (gAE_Status & GSP_Status_AE_SMSM)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				gAE_Status &= ~GSP_Status_AE_SMSM;
				if (Selection) 
				{	
					gAE_Status |= GSP_Status_AE_SMSM;
				}
			}
			break;
		case GSP_STP_ID_AE_BW:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.BWFactor);		
				}
			} else
			{
				if (Selection == 2) gAE_Params.BWFactor+= (float)0.1f;
				if (Selection == 0) gAE_Params.BWFactor-= (float)0.1f;
				gAE_Params.BWFactor = fMax(0.0 , fMin(1 , gAE_Params.BWFactor));
				
				p_OL -> Selection = 1;
			}
			if (gAE_Params.BWFactor>0)
			{
				gAE_Status |= GSP_Status_AE_BW;
			}
			else
			{
				gAE_Status &= ~GSP_Status_AE_BW;
			}
			break;

		case GSP_STP_ID_AE_FADE:

			if (SetMenu)
			{
				if (gAE_Status & GSP_Status_AE_FADE)
					p_OL -> Selection = 1; 
				else
					p_OL -> Selection = 0;
			} else {
				gAE_Status &= ~GSP_Status_AE_FADE;
				if (Selection) 
				{	
					gAE_Status |= GSP_Status_AE_FADE;
					if (p_gGx8SpecificData->iFadeAlfa==0)
						p_gGx8SpecificData->iFadeAlfa=255;
					gAE_Params.fadeTime=1;
				}
			}
			break;
		case GSP_STP_ID_AE_DB:
			if (SetMenu)
			{
				if (gAE_Status & GSP_Status_AE_DB)
					p_OL -> Selection = 1; 
				else
					p_OL -> Selection = 0;
			} else {
				gAE_Status &= ~GSP_Status_AE_DB;
				if (Selection) 
				{	
					gAE_Status |= GSP_Status_AE_DB;
				}
			}
			break;
		case GSP_STP_ID_AE_DBN:
			if (SetMenu)
			{
				if (gAE_Status & GSP_Status_AE_DBN)
					p_OL -> Selection = 1; 
				else
					p_OL -> Selection = 0;
			} else {
				gAE_Status &= ~GSP_Status_AE_DBN;
				if (Selection) 
				{	
					gAE_Status |= GSP_Status_AE_DBN;
				}
			}
			break;
		case GSP_STP_ID_AE_DBN_FAR:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.Depth_Blur_Near);		
				}
			} else
			{
				if (Selection == 2) gAE_Params.Depth_Blur_Near+= 0.5f;
				if (Selection == 0) gAE_Params.Depth_Blur_Near-= 0.5f;
				gAE_Params.Depth_Blur_Near = fMax(0.0 , fMin(100 , gAE_Params.Depth_Blur_Near));
				
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_DB_FAR:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.Depth_Blur_ZEnd);		
				}
			} else
			{
				if (Selection == 2) gAE_Params.Depth_Blur_ZEnd+= 1;
				if (Selection == 0) gAE_Params.Depth_Blur_ZEnd-= 1;
				gAE_Params.Depth_Blur_ZEnd = fMax(0.0 , fMin(500 , gAE_Params.Depth_Blur_ZEnd));
				
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_DB_ALPHA_ADJUST:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2i" , nearAlphaAdjustment);		
				}
			} else
			{
				if (Selection == 2) nearAlphaAdjustment+= 5;
				if (Selection == 0) nearAlphaAdjustment-= 5;
				nearAlphaAdjustment = (int)fMax(0.0 , fMin(500 , (float)nearAlphaAdjustment));
				
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_MB:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.MotionBlurFactor);		
				}
			} else
			{
				if (Selection == 2) gAE_Params.MotionBlurFactor+= 0.1f;
				if (Selection == 0) gAE_Params.MotionBlurFactor-= 0.1f;
				gAE_Params.MotionBlurFactor = fMax(0.0f , fMin(1.0f , gAE_Params.MotionBlurFactor));
				
				p_OL -> Selection = 1;
			}
			if (gAE_Params.MotionBlurFactor>0)
			{
				gAE_Status |= GSP_Status_AE_MB;
			}
			else
			{
				gAE_Status &= ~GSP_Status_AE_MB;
			}
			break;
		case GSP_STP_ID_AE_ZS:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.ZoomSmoothFactor);		
				}
			} else
			{
				if (Selection == 2) gAE_Params.ZoomSmoothFactor+= (float)0.05;
				if (Selection == 0) gAE_Params.ZoomSmoothFactor-= (float)0.05;
				gAE_Params.ZoomSmoothFactor = fMax(0.0 , fMin(1 , gAE_Params.ZoomSmoothFactor));
				
				p_OL -> Selection = 1;
			}
			if (gAE_Params.ZoomSmoothFactor>0)
			{
				gAE_Status |= GSP_Status_AE_ZS;
			}
			else
			{
				gAE_Status &= ~GSP_Status_AE_ZS;
			}
			break;
		case GSP_STP_ID_AE_RS:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.SpinSmoothFactor);		
				}
			} else
			{
				if (Selection == 2) gAE_Params.SpinSmoothFactor+= (float)0.05;
				if (Selection == 0) gAE_Params.SpinSmoothFactor-= (float)0.05;
				gAE_Params.SpinSmoothFactor = fMax(0.0 , fMin(1 , gAE_Params.SpinSmoothFactor));
				
				p_OL -> Selection = 1;
			}
			if (gAE_Params.SpinSmoothFactor>0)
			{
				gAE_Status |= GSP_Status_AE_RS;
			}
			else
			{
				gAE_Status &= ~GSP_Status_AE_RS;
			}
			break;
		case GSP_STP_ID_AE_REMANANCE:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.RemananceFactor);		
				}
			} else
			{
				if (Selection == 2) gAE_Params.RemananceFactor+= (float)0.05;
				if (Selection == 0) gAE_Params.RemananceFactor-= (float)0.05;
				gAE_Params.RemananceFactor = fMax(0.0 , fMin(1 , gAE_Params.RemananceFactor));
				
				p_OL -> Selection = 1;
			}
			if (gAE_Params.RemananceFactor>0)
			{
				gAE_Status |= GSP_Status_AE_REMANANCE;
			}
			else
			{
				gAE_Status &= ~GSP_Status_AE_REMANANCE;
			}
			break;
		case GSP_STP_ID_SHADOWBUFFER:
			if (SetMenu)
			{
				if (bShadowBufferCheat)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				bShadowBufferCheat=Selection;
			}
			break;
		case GSP_STP_ID_NOFSHADOWBUFFER:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2i" , iNOfShadowBufferCheat);		
				}
			} else
			{
				if (Selection == 2) iNOfShadowBufferCheat+= 1;
				if (Selection == 0) iNOfShadowBufferCheat-= 1;
				iNOfShadowBufferCheat = (int)fMax(1 , fMin(ShadowBuffer_MaxLights , (float)iNOfShadowBufferCheat));
				
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_SMOOTH:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.BlurFactor);		
				}
			} else
			{
				if (Selection == 2) gAE_Params.BlurFactor+= (float)5;
				if (Selection == 0) gAE_Params.BlurFactor-= (float)5;
				gAE_Params.BlurFactor = fMax(0.0 , fMin(100 , gAE_Params.BlurFactor));
				
				p_OL -> Selection = 1;
			}
			if (gAE_Params.BlurFactor>0)
			{
				gAE_Status |= GSP_Status_AE_SM;
			}
			else
			{
				gAE_Status &= ~GSP_Status_AE_SM;
			}
			break;


		case GSP_STP_ID_AE_ALLOFF:
	/*		if (SetMenu)
			{
				if (gAE_Status & GSP_Status_AE_NOAE)
					p_OL -> Selection = 0; 
				else
					p_OL -> Selection = 1;
			} else {
				gAE_Status &= ~GSP_Status_AE_NOAE;
				if (Selection) 
				{	
					gAE_Status |= GSP_Status_AE_NOAE;
				}*/
			if (SetMenu)
			{
				p_OL -> Selection = bNoAE;//bTest; bNoAE/* On */
			} else {
				bNoAE = p_OL -> Selection; /* On */
			}
			break;			

		case GSP_STP_ID_AE_BrightNess:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.Brighness);
					
				}
			} else
			{
				if (Selection == 2) gAE_Params.Brighness+= 0.01f;
				if (Selection == 0) gAE_Params.Brighness-= 0.01f;
				gAE_Params.Brighness = fMax(0.0 , fMin(1.0f , gAE_Params.Brighness));
				
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_Contrast:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.Contrast);
				}
			} else
			{
				if (Selection == 2) gAE_Params.Contrast+= 0.01f;
				if (Selection == 0) gAE_Params.Contrast-= 0.01f;
				gAE_Params.Contrast = fMax(0.0 , fMin(1.0f , gAE_Params.Contrast));
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_CB_INTENSITY:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (gAE_Status & GSP_Status_AE_CB_On)
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.CB_Intensity);
					else
					{
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "off");
					}
				}
			} else
			{
				if (Selection == 2) gAE_Params.CB_Intensity+= 0.01f;
				if (Selection == 0) gAE_Params.CB_Intensity-= 0.01f;
				gAE_Params.CB_Intensity = fMax(0.0 , fMin(1.0f , gAE_Params.CB_Intensity));
				gAE_Status &= ~GSP_Status_AE_CB_On;
				if (gAE_Params.CB_Intensity != 0.0f) gAE_Status |= GSP_Status_AE_CB_On;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_CB_SPECTRE:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , gAE_Params.CB_Spectre);
				}
			} else
			{
				if (Selection == 2) gAE_Params.CB_Spectre+= 0.01f;
				if (Selection == 0) gAE_Params.CB_Spectre-= 0.01f;
				if (gAE_Params.CB_Spectre >= 1.0f) gAE_Params.CB_Spectre -= 1.0f;
				if (gAE_Params.CB_Spectre < 0.0f) gAE_Params.CB_Spectre += 1.0f;
				p_OL -> Selection = 1;
			}
			break;
// ------------ TV Options ---------------

		case GSP_STP_ID_RATIO:
		{
			if (SetMenu)
			{
				p_OL -> Selection = TVMode; /* On */
				
			} else 
			{
						
				TVMode = p_OL -> Selection; /* On */
				Gx8_SwitchDisplay();


			}
			break;
		}
// ---------------------------------------

		case GSP_STP_ID_AE_SHEETS:
#ifdef _XBOX

#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5

#define    eBtn_Cross					0
#define 	eBtn_Circle					1
#define 	eBtn_Square					2
#define 	eBtn_Triangle				3
#define 	eBtn_L1						4
#define 	eBtn_R1						5
#define 	eBtn_L2						6
#define 	eBtn_R2						7
#define 	eBtn_Select					8
#define 	eBtn_Start					9
#define 	eBtn_R3						10
#define 	eBtn_L3						11
#define 	eBtn_Up						12
#define 	eBtn_Right					13
#define 	eBtn_Down					14
#define 	eBtn_Left					15

			if (!SetMenu)
			{
				switch (Selection) 
				{
					case 0:
                        {
                            ULONG l_Mask=0;
#ifndef PJ_VERSION
                            l_Mask |= (1<<eBtn_L1);
                            l_Mask |= (1<<eBtn_R1);
                            l_Mask |= (1<<eBtn_L2);
                            l_Mask |= (1<<eBtn_R2);
                            l_Mask |= (1<<eBtn_Select);
                            l_Mask |= (1<<eBtn_Start);
                            INO_JoySimulate(l_Mask , l_Mask , NULL, NULL);
#endif
                            gi_flag_cheat_invincible = 0;
                        }
                        break; // All objects KEY = 0x60 = NUMPAD0
					case 1:
                       {
                           gi_flag_cheat_invincible = 1;
                       }
                         break; // All objects KEY = 0x60 = NUMPAD0
					case 2:
                        {
                            ULONG l_Mask=0;

                            l_Mask |= (1<<eBtn_L1);
                            l_Mask |= (1<<eBtn_R1);
                            l_Mask |= (1<<eBtn_L2);
                            l_Mask |= (1<<eBtn_R2);
                            INO_JoySimulate(l_Mask , l_Mask , NULL, NULL);
                        }
                        break; // All objects KEY = 0x60 = NUMPAD0
					default:iVirtualPressedKeyTime=0; break;
				}
			}
#endif
			break;
			
		case GSP_STP_ID_NO_SDW:
			if (SetMenu)
			{
				p_OL -> Selection = g_iNoSDW; /* On */
			} else {
				g_iNoSDW = p_OL -> Selection; /* On */
			}
			break;

        case GSP_STP_ID_NO_AMBIENCE:
            TOGGLE_FLAG( g_bUseAmbience );
            break;

        case GSP_STP_ID_NO_DIALOGS:
            TOGGLE_FLAG( g_bUseDialogs );
            break;

        case GSP_STP_ID_NO_STREAM:
            TOGGLE_FLAG( g_bUseStream );
            break;

        case GSP_STP_ID_2DXPOS:
            break;

        case GSP_STP_ID_2DYPOS:
            break;
        
        case GSP_STP_ID_2DFFX:
            INCDEC_VAR(f2DFFX_A2D,"%.4f",0.001f,0.5f,10.5f);
            if( !SetMenu )
            {
                STR_f_XFactor = 1.0f/(1.0f + (Gx8Global_get(f2DFFX_A2D) - 1.0f) / 1.0f);
                STR_f_XOffset = Gx8Global_get(Xsize) * (1.0f - STR_f_XFactor) / 1.8f;
            }

            break;

        case GSP_STP_ID_2DFFY:
            INCDEC_VAR(f2DFFY_A2D,"%.4f",0.001f,0.5f,10.5f);
            if( !SetMenu )
            {
                STR_f_YFactor = 1.0f/(1.0f + (Gx8Global_get(f2DFFY_A2D) - 1.0f) / 1.0f);
                STR_f_YOffset = Gx8Global_get(Ysize) * (1.0f - STR_f_YFactor) / 1.8f;
            }
            break;
#if defined( FAST_CAP )
        case GSP_STP_ID_ENABLE_FASTCAP:
            if( !SetMenu )
            {
                Gsp_SetupMenu *CurrentMenu;
                Gsp_Stp_OneLeaf *CurrentLeaf;

                g_FastCapEnabled = true;
                Gsp_Stp_MenuClose(&Gsp_Stp_Root);
                Depth = 0;
            	CurrentMenu = Gsp_Stp_GetMenu(&Gsp_Stp_Root , Depth , (u_int*)DepthStack);
	            CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
	            CurrentLeaf -> Hlight = 1;
            }

            break;
#endif

		case GSP_STP_ID_NULL:
            break;

	}
	if (p_OL->DefaultSelection == -1) p_OL->DefaultSelection = p_OL->Selection;
}
##if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

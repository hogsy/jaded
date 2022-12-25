
/*$T GSPinit.c GC! 1.081 05/04/00 15:08:05 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include <libgraph.h>
#include <libpad.h>

#include "Gsp.h"
#include "GSPinit.h"
#include "GSPtex.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "Gsp_Bench.h"
#include "ENGvars.h"

#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDconst.h"
#include "IOP/RPC_Manager.h"
#include "IOP/ee/eeRPC_Debug.h"

#include "BIGfiles/VERsion/VERsion_Number.h"

#include "../../../Dlls/JPEGLIB/INTERFCE.H"


#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif
extern void TIM_UpdateTimerFrequency(void);
extern void GSP_DrawTExt_X2Y2(int X,int Y,char *P_String,u_int Color,u_int ColorBack,unsigned long TFront,unsigned long TBack,u_int X2,u_int Y2);
#define OPEN_FILE_MODE SCE_RDONLY , 0
#define CREATE_FILE_MODE SCE_CREAT | SCE_WRONLY  , 0777

#define GSP_RECORD_IO_JOY_ENABLE
//#define PSX2_Snap_Folder "host0:y:/PSX2_SNAPS/"
#define PSX2_Snap_Folder "host0:y:/PSX2_SNAPS/"
void SPECIAL_Write(void *Address , ULONG Size1 , ULONG Size2 , L_FILE      h_TmpFile)
{
	ULONG RealSize , LocalSize;
	RealSize = Size1 * Size2;
	while (RealSize)
	{
		LocalSize = lMin(RealSize , 4096);
		L_fwrite((void *)Address, 1, LocalSize, h_TmpFile);
		RealSize -= LocalSize;
		(u_char *)Address += LocalSize;
	}
}

extern int ps2SND_i_FxDelay;
extern float ps2SND_f_FxWetVol;
extern int ps2SND_i_FxFeedback;
extern int ps2SND_i_FxWetPan;
extern int	    ps2SND_i_FxSetDelay(int, int);
extern int	    ps2SND_i_FxGetDelay(int);
extern int	    ps2SND_i_FxSetFeedback(int, int);
extern int	    ps2SND_i_FxGetFeedback(int);
extern void	    ps2SND_i_FxSetWetVolume(int, int);
extern int	    ps2SND_i_FxGetWetVolume(int);
extern void	    ps2SND_i_FxSetWetPan(int, int);
extern int	    ps2SND_i_FxGetWetPan(int);
extern void ps2SND_FxFlushSettings(int);
extern LONG SND_l_GetAttFromVol(float);
extern float SND_f_GetVolFromAtt(LONG);


extern float fGlobalAcc;
extern float fOoGlobalAcc;
extern u_int ulMotionBlurValue;
extern u_int ulRemananceStrong;
extern u_int ulZoomSmoothStreinght;
extern u_int ulRotateSmoothStreinght;
extern u_int AI2C_ai2Ccan;
extern int	TextnumToShow;
#define MAX_Sequence_Images 4096

extern float    STR_f_XOffset;
extern float    STR_f_YOffset;
extern float    STR_f_XFactor;
extern float    STR_f_YFactor;
extern BOOL     ENG_gb_ForcePauseEngine;
extern float GlobalBrightness;
extern float GlobalContrast;

u_int PS2_MenuIsOpen = 0;
u_int NoMATDRAW = 0;
u_int No2D = 0;
u_int GLOBAL_LockCam = 0;
u_int NoGEODRAW = 0;
u_int NoGODRAW = 0;
u_int NoLIGH = 0;
u_int VideoMode;
u_int NoMulti = 0;
u_int NoSDW = 0;
u_int NoSPR = 0;
u_int NoGFX = 0;
u_int NoZLST = 0;
u_int NoSKN = 0;
u_int NoADM = 0;
u_int NoPAG = 0;
u_int NoSTR = 0;
u_int NoMDF = 0;
u_int No3DWTR = 0;
u_int ShowBpp = 0;
u_int RasterMode = 0;
u_int bForce60 = 0;
u_int NoVideo=0;
u_int NoPreload=0;
#ifdef GSP_RECORD_IO_JOY_ENABLE
u_int RecordIO = 0;
#define RecorIOMEM_BASE (1024*1024*128-1024*1024*2)

#define GSP_SnapRamBase (1024*1024*66)
//#define GSP_SnapRamBase (1024*1024*34)
u_char *RecordIOBase;
extern float TIM_gf_dt;
u_int GSP_gb_ForcePauseJoy = 0;
#endif

extern int Rasters_MaxEnble;
u_int RasterFilter = 0;
u_int RasterDetailMum = 10;
u_int AE_ON = 0;
u_int DISPLAYSTACKSPR = 0;
u_int bTakeSnapshot = 0;
float fAddXAA = 0.0f;
float fAddYAA = 0.0f;
extern unsigned int eeRPC_gui_PreloadDelay;

unsigned char *puc_Jpeg_Buffer_Dst = NULL;

u_int 	LoadTextureInterface = 1;
u_int 	LoadedTriangles = 0; 
u_int gs_intensity;


u_int ShowNormals=0;
#ifndef _FINAL_
u_int FultextureSize = 0;
u_int TextureUsed = 0;
u32 DisplayMire = 0;
#ifdef GSP_PS2_BENCH
extern u_int StackGO;
u_int SaveXSize = 0;
float SaveTimer = 0.0f;
u_int SaveGounzafiot = 0;
#endif
static u_int TgaMode = 0;

static u_int bTakeSnapshotShift;

#define STP_PAD_R (SCE_PADLright/*|SCE_PADRright*/)
#define STP_PAD_L (SCE_PADLleft/*|SCE_PADRleft*/)
#define STP_PAD_U (SCE_PADLup/*|SCE_PADRup*/)
#define STP_PAD_D (SCE_PADLdown/*|SCE_PADRdown*/)
#define STP_PAD_V (SCE_PADselect | SCE_PADstart /*| SCE_PADL1 | SCE_PADL2 | SCE_PADR1 | SCE_PADR2*/ )
static u_int paddataXor = 0;
static u_int Depth = 0;
static int DepthStack[12];
extern GSP_ShowAvailableTextureMemory();
extern Gsp_InitVRAMManager();
#define GSP_STP_ID_NULL			0
#define GSP_STP_ID_RASTER		1
#define GSP_STP_ID_CONSOLE		2
#define GSP_STP_ID_SHOWVSYNC		3
#define GSP_STP_ID_EXIT			4
#define GSP_STP_ID_16_9			5
#define GSP_STP_ID_DONTDRAW		6
#define GSP_STP_ID_CBPP			7
#define GSP_STP_ID_ZBPP			8
#define GSP_STP_ID_HRES			9
#define GSP_STP_ID_VRES			10
#define GSP_STP_ID_PALNTSC		11
#define GSP_STP_ID_SHOWVRAM		12
#define GSP_STP_ID_CLEARVRAM		13
#define GSP_STP_ID_TRICOUNTER		14
#define GSP_STP_ID_SETRENDERSTATE	15
#define GSP_STP_ID_AE_MB		17
#define GSP_STP_ID_AE_DB		18
#define GSP_STP_ID_AE_SM		19
#define GSP_STP_ID_MATDONTDRAW		20
#define GSP_STP_ID_GEODONTDRAW		21
#define GSP_STP_ID_GODONTDRAW		22
#define GSP_STP_ID_NOLIGHT		23
#define GSP_STP_ID_RASTER_DETAILS	24
#define GSP_STP_ID_AE_EMB		25
#define GSP_STP_ID_AE_SMSM		26
#define GSP_STP_ID_AE_SHOWSDW		27
#define GSP_STP_ID_AE_SZB		28
#define GSP_STP_ID_AE_ZS		29
#define GSP_STP_ID_AE_TIMER		30
#define GSP_STP_ID_REMANANCE		31
#define GSP_STP_ID_CRTC_VAA		32
#define GSP_STP_ID_SHOWDEPTHCOMPPLEXITY 33
#define GSP_STP_ID_SOUNDDEBUG 		34
#define GSP_STP_ID_SOUNDFORCE 		35
#define GSP_STP_ID_SOUNDNOWAV 		36
#define GSP_STP_ID_SOUNDNOWAD 		37
#define GSP_STP_ID_SOUNDNOWAM 		38
#define GSP_STP_ID_SOUNDNOWAA 		39
#define GSP_STP_ID_SOUNDNOSMD 		40
#define GSP_STP_ID_SOUNDFX   		41
#define GSP_STP_ID_AE_DBN		    54
#define GSP_STP_ID_AE_BrightNess    55
#define GSP_STP_ID_AE_Contrast	    56
#define GSP_STP_ID_AE_MODECOMP      57
#define GSP_STP_ID_AE_BlurTest44	70
#define GSP_STP_ID_AE_RS			71
#define GSP_STP_ID_AE_ALLOFF		72
#define GSP_STP_ID_AE_SNAPSHOT		73
#define GSP_STP_ID_NO_MULTI			75
#define GSP_STP_ID_NO_SDW			76
#define GSP_STP_ID_NO_SPR			77
#define GSP_STP_ID_NO_GFX			78
#define GSP_STP_ID_NO_ZLST			79
#define GSP_STP_ID_NO_SKN			80
#define GSP_STP_ID_NO_ADM			81
#define GSP_STP_ID_SHOWBPP			82
#define GSP_STP_ID_INTERP_C			83
#define GSP_STP_ID_RASTER_Prop		85
#define GSP_STP_ID_RASTER_MODE		86
#define GSP_STP_ID_RASTER_DETNUM	87
#define GSP_STP_ID_TOOLSHOWTEX		88
#define GSP_STP_ID_STRIP_ON			89
#define GSP_STP_ID_PAG_ON			90
#define GSP_STP_ID_STR_ON			91
#define GSP_STP_ID_MDF_ON			92
#define GSP_STP_ID_AE_SHEETS		93
#define GSP_STP_ID_AE_BW    		94
#define GSP_STP_ID_CB_INTENSITY		95
#define GSP_STP_ID_CB_SPECTRE		96
#define GSP_STP_ID_MAR				97
#define GSP_STP_ID_CB_PF			98
#define GSP_STP_ID_AE_DBNear		99
#define GSP_STP_ID_AE_DBFar			100
#define GSP_STP_ID_FOG_CORRECTION   101
#define GSP_STP_ID_AE_DBNSet		102
#define GSP_STP_ID_LoadTI			103
#define GSP_STP_ID_SYNCSTABILITY	104
#define GSP_STP_ID_TEST				105
#define GSP_STP_ID_RAMRASTER 		106
#define GSP_STP_ID_AE_WF_ZH			107
#define GSP_STP_ID_AE_WF_ZD			108
#define GSP_STP_ID_AE_WF_ZP				109
#define GSP_STP_ID_GZFY					110
#define GSP_STP_ID_CB_PASTEL    		111
#define GSP_STP_ID_UNCACHE_SP    		112
#define GSP_STP_ID_ESTIMATESTACKSIZE	113
#define GSP_STP_ID_DISLPAY_STACK_SPR	114
#define GSP_STP_ID_ENGINE_STACK_SPR		115
#define GSP_STP_ID_RASTER_SORTED		116
#define GSP_STP_ID_RASTER_FILTER		117
#define GSP_STP_ID_VHREPOS				118
#define GSP_STP_ID_CLEARTRAM			119
#define GSP_STP_ID_FOG_FAR_CORRECTION   120
#define GSP_STP_ID_TSS					121
#define GSP_STP_ID_TFADE				122
#define GSP_STP_ID_FGC					123
#define GSP_STP_ID_IOP					124
#define GSP_STP_ID_SOUNDFXENABLE0       125
#define GSP_STP_ID_SOUNDFXMODE0         126
#define GSP_STP_ID_SOUNDFXDELAY0        127
#define GSP_STP_ID_SOUNDFXFEEDBACK0     128
#define GSP_STP_ID_SOUNDFXWETPAN0       129
#define GSP_STP_ID_SOUNDFXWETVOL0       130
#define GSP_STP_ID_SOUNDNOPLAY          131
#define GSP_STP_ID_SOUNDNORQ            132
#define GSP_STP_ID_SOUNDNOVOL           133
#define GSP_STP_ID_SOUNDNOFREQ          134
#define GSP_STP_ID_2DFFX           		135
#define GSP_STP_ID_2DFFY          		136
#define GSP_STP_ID_AE_WF_TM				137
#define GSP_STP_ID_AE_WF_SE				138
#define GSP_STP_ID_AE_WF_DF				139
#define GSP_STP_ID_AE_WF_SEB			140
#define GSP_STP_ID_AE_WF_ZO				141
#define GSP_STP_ID_AE_SNAPSHOT_SEQ		142
#define GSP_STP_ID_AE_SNAPSHOT_SEQ_OS		143
#define GSP_STP_ID_NO2D					144
#define GSP_STP_ID_AE_BIGSNAPSHOT		145
#define GSP_STP_ID_AE_GM				146
#define GSP_STP_ID_AE_GI				147
#define GSP_STP_ID_AE_GMB				148
#define GSP_STP_ID_RASTER_MAX			149
#define GSP_STP_ID_RIO					150
#define GSP_STP_ID_CachText				151
#define GSP_STP_ID_NOVIDEO				152
#define GSP_STP_ID_NOPRELOAD			153
#define GSP_STP_ID_PRELOADDELAY         154
#define GSP_STP_ID_AE_WF_N3DWTR         155
#define GSP_STP_ID_AE_JPEG				156
#define GSP_STP_ID_DOUBLE_RENDERING		157
#define GSP_STP_ID_AE_WARP				158
#define GSP_STP_ID_2DFFX_O				159
#define GSP_STP_ID_2DFFY_O				160
#define GSP_STP_ID_LOCKCAM				161
#define GSP_STP_ID_SHOWNORMALS			162

#define GSP_STP_ID_SOUNDFXENABLE1       163
#define GSP_STP_ID_SOUNDFXMODE1         164
#define GSP_STP_ID_SOUNDFXDELAY1        165
#define GSP_STP_ID_SOUNDFXFEEDBACK1     166
#define GSP_STP_ID_SOUNDFXWETPAN1       167
#define GSP_STP_ID_SOUNDFXWETVOL1       168

#define GSP_STP_ID_MEM_SPY				169
#define GSP_STP_ID_MIRE					170
#define GSP_STP_ID_SPG2					171

struct Gsp_SetupMenu_;
void GSP_SNP_SetAA(u_int SubImgNum)
{
	float fAddV;
	fAddV = 0.5f;
	fAddXAA = 0.0f;
	fAddYAA = 0.0f;
	while (SubImgNum)
	{
		if (SubImgNum & 1) fAddXAA += fAddV;
		if (SubImgNum & 2) fAddYAA += fAddV;
		fAddV *= 0.5f;
		SubImgNum >>= 2;
	}
	Gsp_InitVU1Matrix_Extention();
}

typedef struct Gsp_Stp_OneLeaf_
{
	char  Name[32];
	u_int Hlight;
	u_int Open;
	int Selection;
	u_int ID;
	struct Gsp_SetupMenu_ * p_SubMenu;
	u_int DefaultSelection;
} Gsp_Stp_OneLeaf;

typedef struct Gsp_SetupMenu_
{	
	unsigned char Title[64];
	u_int NumberOfLeaves;
	u_int OpenSize;
	u_int AutoSelection;
	Gsp_Stp_OneLeaf Leaves[32];
} Gsp_SetupMenu;

static Gsp_SetupMenu Gsp_Stp_OnOff;
static Gsp_SetupMenu Gsp_Stp_INTERPAI;
static Gsp_SetupMenu Gsp_Stp_Bpp ;
static Gsp_SetupMenu Gsp_Stp_ResolutionV ;
static Gsp_SetupMenu Gsp_Stp_ResolutionVPAL ;
static Gsp_SetupMenu Gsp_Stp_ResolutionH ;
static Gsp_SetupMenu Gsp_Stp_PalNtsc ;
static Gsp_SetupMenu Gsp_Stp_Video ;
static Gsp_SetupMenu Gsp_Stp_Tool ;
static Gsp_SetupMenu Gsp_Stp_3D ;
static Gsp_SetupMenu Gsp_Stp_Root ;
static Gsp_SetupMenu Gsp_Stp_AE;
static Gsp_SetupMenu Gsp_Stp_Timer;
static Gsp_SetupMenu Gsp_Stp_0_To_100;
static Gsp_SetupMenu Gsp_Stp_SoundDebug;
static Gsp_SetupMenu Gsp_Stp_Sound;
static Gsp_SetupMenu Gsp_Stp_IOP;
static Gsp_SetupMenu Gsp_Stp_m10_To_p10;
static Gsp_SetupMenu Gsp_Stp_DistanceSet;
static Gsp_SetupMenu Gsp_Stp_Rasters;
static Gsp_SetupMenu Gsp_Stp_Rasters_Mode;
static Gsp_SetupMenu Gsp_Stp_4_To_20;
static Gsp_SetupMenu Gsp_Stp_PlusMoin;
static Gsp_SetupMenu Gsp_Stp_SheetCodes;
static Gsp_SetupMenu Gsp_Stp_0_To_200;
static Gsp_SetupMenu Gsp_Stp_BlurMode;
static Gsp_SetupMenu Gsp_Stp_ColorBalance;
static Gsp_SetupMenu Gsp_Stp_0_To_12_Spectre;
static Gsp_SetupMenu Gsp_Stp_DepthBluringSetting;
static Gsp_SetupMenu Gsp_Stp_WaterReflection;
static Gsp_SetupMenu Gsp_Stp_Gounzafiot;
static Gsp_SetupMenu Gsp_Stp_RasterFilter;
static Gsp_SetupMenu Gsp_Stp_RamRaster;
static Gsp_SetupMenu Gsp_Stp_SoundFx;
static Gsp_SetupMenu Gsp_Stp_SoundFxMode;
static Gsp_SetupMenu Gsp_Stp_Snap;
static Gsp_SetupMenu Gsp_Stp_GlowMode;
static Gsp_SetupMenu Gsp_Stp_GlowTransMode;
static Gsp_SetupMenu Gsp_Stp_ColorCost;
static Gsp_SetupMenu Gsp_Stp_Ratio;
#ifdef GSP_USE_TEXTURE_CACHE
static Gsp_SetupMenu Gsp_Stp_TextureRam;
#endif



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

float GSP_SET_SelectionToFloat(ULONG Selection)
{
	return ((float)Selection) / 10.0f;
}

float GSP_SET_SelectionToFloat2(ULONG Selection)
{
	return ((float)Selection) / 20.0f;
}

float GSP_SET_SelectionToFloat3(ULONG Selection)
{
	return ((float)Selection) / 12.0f;
}

ULONG GSP_SET_FloatToSelection(float fValue)
{
	return (ULONG)(fValue * 10.0f);
}
ULONG GSP_SET_FloatToSelection2(float fValue)
{
	return (ULONG)(fValue * 20.0f);
}

ULONG GSP_SET_FloatToSelection3(float fValue)
{
	return (ULONG)(fValue * 12.0f);
}

void GSP_SET_DeltaDistanceGet(ULONG Selection , float *Dist)
{
	float fDelta;
	switch(Selection)
	{
		case 0: fDelta = -1000.0f; break;
		case 1: fDelta = -100.0f; break;
		case 2: fDelta = -10.0f; break;
		case 3: fDelta = -1.0f; break;
		case 4: fDelta = -0.1f; break;
		case 5: fDelta = 0.0f; break;
		case 6: fDelta = 0.1f; break;
		case 7: fDelta = 1.0f; break;
		case 8: fDelta = 10.0f; break;
		case 9: fDelta = 100.0f; break; 
		case 10: fDelta = 1000.0f; break;
	}
	*Dist += fDelta;
	if (*Dist < 0.0f) *Dist = 0.0f;
}
void GSP_SET_DeltazGet(ULONG Selection , float *Dist)
{
	float fDelta;
	switch(Selection)
	{
		case 0: fDelta = -1000.0f; break;
		case 1: fDelta = -100.0f; break;
		case 2: fDelta = -10.0f; break;
		case 3: fDelta = -1.0f; break;
		case 4: fDelta = -0.1f; break;
		case 5: fDelta = 0.0f; break;
		case 6: fDelta = 0.1f; break;
		case 7: fDelta = 1.0f; break;
		case 8: fDelta = 10.0f; break;
		case 9: fDelta = 100.0f; break; 
		case 10: fDelta = 1000.0f; break;
	}
	*Dist += fDelta;
}

void GSP_STP_FirstInt()
{
	
#ifdef GSP_USE_TEXTURE_CACHE
	Gsp_Stp_TextureRam = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"NO",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"1 Meg",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"2 Meg",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"3 Meg",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"4 Meg",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
#endif	
	Gsp_Stp_OnOff = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"NO",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"YES",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_Gounzafiot = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"Off",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"400",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"360",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_PlusMoin = (Gsp_SetupMenu)
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
	
	Gsp_Stp_DistanceSet = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"-1000",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-100",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-10",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-1",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-0.1",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"On",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"+0.1",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"+1",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"+10",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"+100",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"+1000",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	

	Gsp_Stp_INTERPAI = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"Interpreted",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"C",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_0_To_100 = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"OFF",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"10",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"20",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"30",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"40",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"50",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"60",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"70",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"80",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"90",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"100",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_0_To_12_Spectre = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"Red    0.0",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Yellow 1/6",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Green  1/3",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Cyan   1/2",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Blue   2/3",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Magen. 5/6 ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Red    1.0",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_0_To_200 = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"-10",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-9",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-8",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-7",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-6",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-5",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-4",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-3",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-2",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"-1",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"OFF",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 1",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 2",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 3",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 4",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 5",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 6",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 7",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 8",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 9",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"10",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_4_To_20 = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"4",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"6",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"8",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"10",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"12",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"14",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"16",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"18",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"20",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"22",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"24",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_m10_To_p10 = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"---",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" - ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"OFF",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" + ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"+++",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	Gsp_Stp_ColorBalance = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"Intensity",0,0,0,GSP_STP_ID_CB_INTENSITY,&Gsp_Stp_PlusMoin,-1},
			{"Spectre",0,0,0,GSP_STP_ID_CB_SPECTRE,&Gsp_Stp_PlusMoin,-1},
//			{"Pastel",0,0,0,GSP_STP_ID_CB_PASTEL,&Gsp_Stp_0_To_100,-1},
			{"Photo filter",0,0,0,GSP_STP_ID_CB_PF,&Gsp_Stp_0_To_100,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	Gsp_Stp_Timer = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"* 8",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"* 4",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"* 2",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Normal",0,0,0,0,NULL,-1},
			{"/ 2",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"/ 4",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"/ 8",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Quasi paused",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	Gsp_Stp_Bpp = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{" 32 ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 24 ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{" 16 ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	 Gsp_Stp_ResolutionV = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"224",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"AUTO",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"448",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"224 AA 3Buf",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_BlurMode = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"Off",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"X",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Y",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"XY",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	 Gsp_Stp_ResolutionVPAL = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"256",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"AUTO",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"512",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"256 AA 3Buf",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	 Gsp_Stp_ResolutionH = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"256 * 2?? ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"320 * 2?? ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"380 * 2?? ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"512 * 2?? ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"640 * 2?? ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"720 * 2?? ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	 Gsp_Stp_PalNtsc = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"Ntsc ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Pal ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"DTV480P",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	 Gsp_Stp_Ratio = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"4/3",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"4/3 black bands ",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"16/9",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	 Gsp_Stp_Video = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"Colored VSync",0,0,0,GSP_STP_ID_SHOWVSYNC,&Gsp_Stp_OnOff,-1},
			{"Resolution H",0,0,0,GSP_STP_ID_HRES,&Gsp_Stp_ResolutionH,-1},
			{"Resolution V",0,0,0,GSP_STP_ID_VRES,&Gsp_Stp_ResolutionV,-1},
			{"Resolution Bpp",0,0,0,GSP_STP_ID_CBPP,&Gsp_Stp_Bpp,-1},
			{"Zbuffer Bpp",0,0,0,GSP_STP_ID_ZBPP,&Gsp_Stp_Bpp,-1},
			{"Pal/Ntsc/P",0,0,0,GSP_STP_ID_PALNTSC,&Gsp_Stp_PalNtsc,-1},
			{"16 / 9",0,0,0,GSP_STP_ID_16_9,&Gsp_Stp_Ratio,-1},
			{"Gounzafiot Y 400",0,0,0,GSP_STP_ID_GZFY,&Gsp_Stp_Gounzafiot,-1},
#ifdef GSP_USE_TEXTURE_CACHE
			{"Use Cache Texture",0,0,0,GSP_STP_ID_CachText,&Gsp_Stp_TextureRam,-1},
#endif			
			{"CRTC vertical AA",0,0,0,GSP_STP_ID_CRTC_VAA,&Gsp_Stp_OnOff,-1},
			{"VSYNC stablity",0,0,0,GSP_STP_ID_SYNCSTABILITY,&Gsp_Stp_0_To_100,-1},
			{"VIDEO HRepos",0,0,0,GSP_STP_ID_VHREPOS,&Gsp_Stp_PlusMoin,-1},
			{"Global Brightness",0,0,0,GSP_STP_ID_2DFFX,&Gsp_Stp_PlusMoin,-1},
			{"Global Contrast",0,0,0,GSP_STP_ID_2DFFY,&Gsp_Stp_PlusMoin,-1},
			{"",0,0,0,0,NULL}
		}
	};
	 Gsp_Stp_Tool = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"Console",0,0,0,GSP_STP_ID_CONSOLE,&Gsp_Stp_OnOff,-1},
			{"Show Vram",0,0,-1,GSP_STP_ID_SHOWVRAM,NULL,-1},
			{"No real-time tex load",0,0,0,GSP_STP_ID_SHOWBPP,&Gsp_Stp_OnOff,-1},
			{"Show textures",0,0,0,GSP_STP_ID_TOOLSHOWTEX,&Gsp_Stp_PlusMoin,-1},
			{"Clear Vram",0,0,-1,GSP_STP_ID_CLEARVRAM,NULL,-1},
			{"Clear free Tram",0,0,-1,GSP_STP_ID_CLEARTRAM,NULL,-1},
			{"Show Alpha Buf",0,0,0,GSP_STP_ID_AE_SZB,&Gsp_Stp_OnOff,-1},
			{"Snapshot",0,0,-1,GSP_STP_ID_AE_SNAPSHOT,NULL,-1},
			{"Big Snapshot",0,0,-1,GSP_STP_ID_AE_BIGSNAPSHOT,NULL,-1},
			{"Snapshot Sequence",0,0,-1,GSP_STP_ID_AE_SNAPSHOT_SEQ,NULL,-1},
			{"No real-time tex",0,0,0,GSP_STP_ID_LoadTI,&Gsp_Stp_OnOff,-1},
			{"Test fade",0,0,-1,GSP_STP_ID_TFADE,NULL,-1},
			{"Flush geo cache ",0,0,-1,GSP_STP_ID_FGC,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	
	Gsp_Stp_DepthBluringSetting =(Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"Depth bluring Enable",0,0,0,GSP_STP_ID_AE_DB,&Gsp_Stp_OnOff,-1},
			{"Depth bluring Near",0,0,0,GSP_STP_ID_AE_DBNear,&Gsp_Stp_PlusMoin,-1},
			{"Depth bluring Far",0,0,0,GSP_STP_ID_AE_DBFar,&Gsp_Stp_PlusMoin,-1},
			{"Depth bluring Near Enable",0,0,0,GSP_STP_ID_AE_DBN,&Gsp_Stp_OnOff,-1},
			{"Depth bluring Near Setting",0,0,0,GSP_STP_ID_AE_DBNSet,&Gsp_Stp_PlusMoin,-1},
			{"AE report",0,0,0,GSP_STP_ID_MAR,&Gsp_Stp_OnOff,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	Gsp_Stp_WaterReflection =(Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"Water Z",0,0,0,GSP_STP_ID_AE_WF_ZD,&Gsp_Stp_PlusMoin,-1},
			{"No 3D WTR",0,0,0,GSP_STP_ID_AE_WF_N3DWTR,&Gsp_Stp_OnOff,-1},
			{"Height",0,0,0,GSP_STP_ID_AE_WF_ZH,&Gsp_Stp_PlusMoin,-1},
			{"Zplane",0,0,0,GSP_STP_ID_AE_WF_ZP,&Gsp_Stp_PlusMoin,-1},
			{"Sinus Effect",0,0,0,GSP_STP_ID_AE_WF_SE,&Gsp_Stp_PlusMoin,-1},
			{"Sinus Effect Balance",0,0,0,GSP_STP_ID_AE_WF_SEB,&Gsp_Stp_PlusMoin,-1},
			{"Transparency Max",0,0,0,GSP_STP_ID_AE_WF_TM,&Gsp_Stp_PlusMoin,-1},
			{"Dark factor",0,0,0,GSP_STP_ID_AE_WF_DF,&Gsp_Stp_PlusMoin,-1},
			{"Z Order",0,0,0,GSP_STP_ID_AE_WF_ZO,&Gsp_Stp_PlusMoin,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	Gsp_Stp_GlowTransMode = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"Alpha",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Add",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	Gsp_Stp_GlowMode =(Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"blending mode",0,0,0,GSP_STP_ID_AE_GM,&Gsp_Stp_GlowTransMode,-1},
			{"Intensity",0,0,0,GSP_STP_ID_AE_GI,&Gsp_Stp_PlusMoin,-1},
			{"Add white",0,0,0,GSP_STP_ID_AE_GMB,&Gsp_Stp_PlusMoin,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	 Gsp_Stp_AE =(Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"Motion blur",0,0,0,GSP_STP_ID_AE_MB,&Gsp_Stp_0_To_100,-1},
			{"Depth bluring",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_DepthBluringSetting,-1},
			{"Smooth",0,0,0,GSP_STP_ID_AE_SM,&Gsp_Stp_0_To_100,-1},
			{"Motion smooth",0,0,0,GSP_STP_ID_AE_SMSM,&Gsp_Stp_OnOff,-1},
			{"Emboss",0,0,0,GSP_STP_ID_AE_EMB,&Gsp_Stp_0_To_100,-1},
			{"Warp",0,0,0,GSP_STP_ID_AE_WARP,&Gsp_Stp_PlusMoin,-1},
			{"Show shadow buffer",0,0,0,GSP_STP_ID_AE_SHOWSDW,&Gsp_Stp_OnOff,-1},
			{"Zoom smooth",0,0,0,GSP_STP_ID_AE_ZS,&Gsp_Stp_PlusMoin,-1},
			{"Rotate smooth",0,0,0,GSP_STP_ID_AE_RS,&Gsp_Stp_PlusMoin,-1},
			{"Remanance",0,0,0,GSP_STP_ID_REMANANCE,&Gsp_Stp_PlusMoin,-1},
			{"Brightness",0,0,0,GSP_STP_ID_AE_BrightNess,&Gsp_Stp_PlusMoin,-1},
			{"Contrast",0,0,0,GSP_STP_ID_AE_Contrast,&Gsp_Stp_PlusMoin,-1},
			{"Color balance",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_ColorBalance,-1},
			{"Glow mode",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_GlowMode,-1},
			{"BW",0,0,0,GSP_STP_ID_AE_BW,&Gsp_Stp_0_To_100,-1},
			{"Blur 22",0,0,0,GSP_STP_ID_AE_BlurTest44,&Gsp_Stp_BlurMode,-1},
			{"Water ref",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_WaterReflection,-1},
			{"Mode compare",0,0,0,GSP_STP_ID_AE_MODECOMP,&Gsp_Stp_OnOff,-1},
			{"All off",0,0,-1,GSP_STP_ID_AE_ALLOFF,NULL,-1},
			{"AE report",0,0,0,GSP_STP_ID_MAR,&Gsp_Stp_OnOff,-1},
			{"",0,0,0,0,NULL}
		}
	};
	 Gsp_Stp_3D =(Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"2D",0,0,0,GSP_STP_ID_NO2D,&Gsp_Stp_OnOff,-1},
			{"Lock camera",0,0,0,GSP_STP_ID_LOCKCAM,&Gsp_Stp_OnOff,-1},
			{"Don't MATDraw",0,0,0,GSP_STP_ID_MATDONTDRAW,&Gsp_Stp_OnOff,-1},
			{/*"Show strips"*/"Disable geocache",0,0,0,GSP_STP_ID_STRIP_ON,&Gsp_Stp_OnOff,-1},
			{"Don't Compute UV",0,0,0,GSP_STP_ID_GEODONTDRAW,&Gsp_Stp_OnOff,-1},
			{"Don't GO Draw",0,0,0,GSP_STP_ID_GODONTDRAW,&Gsp_Stp_OnOff,-1},
			{"Don't Light",0,0,0,GSP_STP_ID_NOLIGHT,&Gsp_Stp_OnOff,-1},
			{"Set RS",0,0,0,GSP_STP_ID_SETRENDERSTATE,&Gsp_Stp_OnOff,-1},
			{"Don't draw Clones",0,0,0,GSP_STP_ID_DONTDRAW,&Gsp_Stp_OnOff,-1},
			{"No texture multi-pass",0,0,0,GSP_STP_ID_NO_MULTI,&Gsp_Stp_OnOff,-1},
			{"No shadows",0,0,0,GSP_STP_ID_NO_SDW,&Gsp_Stp_OnOff,-1},
			{"No texture sprites",0,0,0,GSP_STP_ID_NO_SPR,&Gsp_Stp_OnOff,-1},
			{"No GFX",0,0,0,GSP_STP_ID_NO_GFX,&Gsp_Stp_OnOff,-1},
			{"No Z liste",0,0,0,GSP_STP_ID_NO_ZLST,&Gsp_Stp_OnOff,-1},
			{"No Skin",0,0,0,GSP_STP_ID_NO_SKN,&Gsp_Stp_OnOff,-1},
			{"No Add mats",0,0,0,GSP_STP_ID_NO_ADM,&Gsp_Stp_OnOff,-1},
			{"No Particules",0,0,0,GSP_STP_ID_PAG_ON,&Gsp_Stp_OnOff,-1},
			{"No strings",0,0,0,GSP_STP_ID_STR_ON,&Gsp_Stp_OnOff,-1},
			{"No modifiers",0,0,0,GSP_STP_ID_MDF_ON,&Gsp_Stp_OnOff,-1},
#ifdef GSP_PS2_BENCH
			{"Show Depth Complex.",0,0,0,GSP_STP_ID_SHOWDEPTHCOMPPLEXITY,&Gsp_Stp_OnOff,-1},
#endif			
			{"Fog Correction",0,0,0,GSP_STP_ID_FOG_CORRECTION,&Gsp_Stp_PlusMoin,-1},
			{"Fog Far Correction",0,0,0,GSP_STP_ID_FOG_FAR_CORRECTION,&Gsp_Stp_PlusMoin,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_Sound = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"DebugInfos",0,0,0,GSP_STP_ID_SOUNDDEBUG,&Gsp_Stp_SoundDebug,-1},
			{"ForceSound",0,0,0,GSP_STP_ID_SOUNDFORCE,&Gsp_Stp_OnOff,-1},
			{"NoWav",0,0,0,GSP_STP_ID_SOUNDNOWAV,&Gsp_Stp_OnOff,-1},
			{"NoWaa",0,0,0,GSP_STP_ID_SOUNDNOWAA,&Gsp_Stp_OnOff,-1},
			{"NoWad",0,0,0,GSP_STP_ID_SOUNDNOWAD,&Gsp_Stp_OnOff,-1},
			{"NoWam",0,0,0,GSP_STP_ID_SOUNDNOWAM,&Gsp_Stp_OnOff,-1},
			{"NoSmd",0,0,0,GSP_STP_ID_SOUNDNOSMD,&Gsp_Stp_OnOff,-1},
			{"Fx",0,0,0,GSP_STP_ID_SOUNDFX,&Gsp_Stp_SoundFx,-1},
			{"NoPlay",0,0,0,GSP_STP_ID_SOUNDNOPLAY,&Gsp_Stp_OnOff,-1},
			{"NoRq",0,0,0,GSP_STP_ID_SOUNDNORQ,&Gsp_Stp_OnOff,-1},
			{"NoVol",0,0,0,GSP_STP_ID_SOUNDNOVOL,&Gsp_Stp_OnOff,-1},
			{"NoFreq",0,0,0,GSP_STP_ID_SOUNDNOFREQ,&Gsp_Stp_OnOff,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_SoundDebug = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{".",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"SndHardBuffer",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"SndFile",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"SndSoftBuffer",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"AllInfos",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Test",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_SoundFxMode = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"Off",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Room",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"StdA",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"StdB",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"StdC",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Hall",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Space",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Echo",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Delay",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Pipe",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Max",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	Gsp_Stp_SoundFx = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"Enable A",0,0,0,GSP_STP_ID_SOUNDFXENABLE0,&Gsp_Stp_OnOff,-1},
			{"Mode A",0,0,0,GSP_STP_ID_SOUNDFXMODE0,&Gsp_Stp_SoundFxMode,-1},
			{"Delay A",0,0,0,GSP_STP_ID_SOUNDFXDELAY0,&Gsp_Stp_PlusMoin,-1},
			{"FeedBack A",0,0,0,GSP_STP_ID_SOUNDFXFEEDBACK0,&Gsp_Stp_PlusMoin,-1},
//			{"WetPan A",0,0,0,GSP_STP_ID_SOUNDFXWETPAN0,&Gsp_Stp_PlusMoin,-1},
			{"WetVol A",0,0,0,GSP_STP_ID_SOUNDFXWETVOL0,&Gsp_Stp_PlusMoin,-1},
			{"Enable B",0,0,0,GSP_STP_ID_SOUNDFXENABLE1,&Gsp_Stp_OnOff,-1},
			{"Mode B",0,0,0,GSP_STP_ID_SOUNDFXMODE1,&Gsp_Stp_SoundFxMode,-1},
			{"Delay B",0,0,0,GSP_STP_ID_SOUNDFXDELAY1,&Gsp_Stp_PlusMoin,-1},
			{"FeedBack B",0,0,0,GSP_STP_ID_SOUNDFXFEEDBACK1,&Gsp_Stp_PlusMoin,-1},
//			{"WetPan B",0,0,0,GSP_STP_ID_SOUNDFXWETPAN1,&Gsp_Stp_PlusMoin,-1},
			{"WetVol B",0,0,0,GSP_STP_ID_SOUNDFXWETVOL1,&Gsp_Stp_PlusMoin,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_IOP = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"off",0,0,-1,GSP_STP_ID_NULL,NULL,-1},			
			{"RAM",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Stack",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"SPU-RAM",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"CPU",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"RPC flush",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"RPC size",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"RPC sound",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"RPC all",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Error",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"NoVideo",0,0,0,GSP_STP_ID_NOVIDEO,&Gsp_Stp_OnOff,-1},
			{"NoPreload",0,0,0,GSP_STP_ID_NOPRELOAD,&Gsp_Stp_OnOff,-1},
			{"PreloadDelay",0,0,0,GSP_STP_ID_PRELOADDELAY,&Gsp_Stp_PlusMoin,-1},			
			{"",0,0,0,0,NULL}
		}
	};
#ifdef GSP_PS2_BENCH			
	Gsp_Stp_Rasters_Mode = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"Normal (CPU Cycles)",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"I Cache Miss",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"D Cache Miss",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Branch prediction misses",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Instruction counter (IC)",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Efficience (IC / CPU Cycles)",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Reads",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Writes",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Cop1 insts",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Cop2 insts",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	Gsp_Stp_RasterFilter = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"All",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Engine Only",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Graphic Only",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Global Only",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"IA user only",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	Gsp_Stp_ColorCost = (Gsp_SetupMenu)
	{
		"",
		0,0,1,
		{
			{"no",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Engine",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Graphic",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Engine+Graphic",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"no",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"XRAY Engine",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"XRAY Graphic",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"XRAY Engine+Graphic",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
#endif			

	Gsp_Stp_RamRaster = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"off",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"MEMdyn",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"32Mo",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Stat",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"History",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};

	Gsp_Stp_Rasters = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"Rasters",0,0,0,GSP_STP_ID_RASTER,&Gsp_Stp_OnOff,-1},
#ifdef GSP_PS2_BENCH			
			{"Rasters details",0,0,0,GSP_STP_ID_RASTER_DETAILS,&Gsp_Stp_OnOff,-1},
			{"ColorCost",0,0,0,GSP_STP_ID_SHOWNORMALS,&Gsp_Stp_ColorCost,-1},
			{"Rasters Prop",0,0,0,GSP_STP_ID_RASTER_Prop,&Gsp_Stp_OnOff,-1},
			{"Raster mode",0,0,0,GSP_STP_ID_RASTER_MODE,&Gsp_Stp_Rasters_Mode,-1},
			{"Raster unsorted",0,0,0,GSP_STP_ID_RASTER_SORTED,&Gsp_Stp_OnOff,-1},
			{"Raster filter",0,0,0,GSP_STP_ID_RASTER_FILTER,&Gsp_Stp_RasterFilter,-1},
			{"Raster det num",0,0,0,GSP_STP_ID_RASTER_DETNUM,&Gsp_Stp_4_To_20,-1},
			{"Raster Max",0,0,0,GSP_STP_ID_RASTER_MAX,&Gsp_Stp_OnOff,-1},
			{"Display TNUM",0,0,0,GSP_STP_ID_TRICOUNTER,&Gsp_Stp_OnOff,-1},
#endif
			{"Rasters RAM",0,0,0,GSP_STP_ID_RAMRASTER,&Gsp_Stp_RamRaster,-1},
			{"uncache SP",0,0,0,GSP_STP_ID_UNCACHE_SP,&Gsp_Stp_OnOff,-1},
#ifdef GSP_PS2_BENCH
			{"Estimate Stack",0,0,0,GSP_STP_ID_ESTIMATESTACKSIZE,&Gsp_Stp_OnOff,-1},
#endif			
			{"Display Stk in spr",0,0,0,GSP_STP_ID_DISLPAY_STACK_SPR,&Gsp_Stp_OnOff,-1},
			{"Engine Stk in spr",0,0,0,GSP_STP_ID_ENGINE_STACK_SPR,&Gsp_Stp_OnOff,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	
	Gsp_Stp_SheetCodes = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"RESET",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Init entrepot",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Init frizbee/gant",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Init egouts",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Init ilot",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"Init satellite",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"1/4",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"1/2",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"3/4",0,0,-1,GSP_STP_ID_NULL,NULL,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	 Gsp_Stp_Snap = (Gsp_SetupMenu)
	{
		"",
		0,0,0,
		{
			{"2D",0,0,0,GSP_STP_ID_NO2D,&Gsp_Stp_OnOff,-1},
#ifdef GSP_RECORD_IO_JOY_ENABLE
			{"Record IO JOY",0,0,0,GSP_STP_ID_RIO,&Gsp_Stp_OnOff,-1},
#endif			
			{"Snapshot",0,0,-1,GSP_STP_ID_AE_SNAPSHOT,NULL,-1},
			{"Big Snapshot",0,0,-1,GSP_STP_ID_AE_BIGSNAPSHOT,NULL,-1},
			{"Snapshot Sequence",0,0,-1,GSP_STP_ID_AE_SNAPSHOT_SEQ,NULL,-1},
			{"Snapshot Sequence OS",0,0,-1,GSP_STP_ID_AE_SNAPSHOT_SEQ_OS,NULL,-1},
			{"TGA Mode",0,0,0,GSP_STP_ID_AE_JPEG,&Gsp_Stp_OnOff,-1},
			{"",0,0,0,0,NULL}
		}
	};
	
	 Gsp_Stp_Root = (Gsp_SetupMenu)
	{
		"Jade version",
		0,0,0,
		{
			{"Video",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_Video,-1},
			{"3D",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_3D,-1},
			{"Sound",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_Sound,-1},
			{"Tool",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_Tool,-1},
			{"Rasters",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_Rasters,-1},
			{"After FX",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_AE,-1},
			{"AI",0,0,0,GSP_STP_ID_INTERP_C,&Gsp_Stp_INTERPAI,-1},
			{"Cheat Codes",0,0,0,GSP_STP_ID_AE_SHEETS,&Gsp_Stp_SheetCodes,-1},
			{"Timer",0,0,0,GSP_STP_ID_AE_TIMER,&Gsp_Stp_Timer,-1},
			{"IOP",0,0,0,GSP_STP_ID_IOP,&Gsp_Stp_IOP,-1},
			{"Snap",0,0,-1,GSP_STP_ID_NULL,&Gsp_Stp_Snap,-1},
			{"Double Rendr",0,0,0,GSP_STP_ID_DOUBLE_RENDERING,&Gsp_Stp_PlusMoin,-1},
#ifdef MEM_SPY			
			{"MEM Spy",0,0,0,GSP_STP_ID_MEM_SPY,&Gsp_Stp_OnOff,-1},
#endif
			{"Mire",0,0,0,GSP_STP_ID_MIRE,&Gsp_Stp_OnOff,-1},
			{"NO SPG2",0,0,0,GSP_STP_ID_SPG2,&Gsp_Stp_OnOff,-1},

			{"",0,0,0,0,NULL}
		}
	};
}
void Gsp_RefreshResolution()
{
	Gsp_InitFB(GspGlobal_ACCESS(Xsize) , VideoMode , 32 - (GspGlobal_ACCESS(BppMode) << 3) , 32 - (GspGlobal_ACCESS(ZBppMode) << 3));
}

extern unsigned char ucVirtualPressedKey[3];
int iVirtualPressedKeyTime=0;
extern void SND_DebugSetRequest(int);
extern int SND_i_DebugGetStatus(int);
u_int GSP_VideoSeqIsOverSampled = 1;

extern int MEM_i_RasterGetStatus(void);
extern void MEM_RasterSetRequest(int);

extern void SND_ForceNo(int _i_mask);
extern void SND_ResetForceNo(int _i_mask);
extern void SND_ForceNoSound(void);
extern void SND_ResetForceNoSound(void);

unsigned int UncacheSP = 0;
_inline_ void CleanSP(u_int SPValue)
{
	asm __volatile__ ("and     sp, SPValue");
}
#ifdef GSP_PS2_BENCH
void Gsp_LaunchSnapShot(u_int Mode)
{
	L_memset((void *)GSP_SnapRamBase , 0 , 1024*1024*128 - (u_int)GSP_SnapRamBase);
	switch (Mode)
	{
		case 1:
			{
				VideoMode = GspGlobal_ACCESS(VideoMode);
				SaveXSize = GspGlobal_ACCESS(Xsize);
				GspGlobal_ACCESS(Xsize) = 512;
				SaveTimer = fGlobalAcc;
				SaveGounzafiot = GspGlobal_ACCESS(Morfling);
				GspGlobal_ACCESS(Morfling) = 0;
				Gsp_RefreshResolution();
				bTakeSnapshotShift = 6;
				bTakeSnapshot = 1 + (1<<bTakeSnapshotShift);
				puc_Jpeg_Buffer_Dst = NULL;
				GSP_SNP_SetAA(0);
				fGlobalAcc = 5564.0f;
				ENG_gb_ForcePauseEngine = TRUE;
				break;
			}
		case 0:
			{
				VideoMode = GspGlobal_ACCESS(VideoMode);
				SaveTimer = fGlobalAcc;
				bTakeSnapshotShift = 4;
				bTakeSnapshot = 1 + (1<<bTakeSnapshotShift);
				puc_Jpeg_Buffer_Dst = NULL;
				GSP_SNP_SetAA(0);
				fGlobalAcc = 5564.0f;
				ENG_gb_ForcePauseEngine = TRUE;
				break;
			}
	}
}
#else
void Gsp_LaunchSnapShot(u_int Mode) {};
#endif
void Gsp_Validate(Gsp_Stp_OneLeaf *p_OL , u_int Selection , u_int SetMenu)
{
	float *FTS;
	float UP_LMT,DOWN_LMT;
	if ((p_OL->Selection == -1) && (SetMenu)) return; /* It's an action (EX SHOW VRAM) */
	switch (p_OL->ID)
	{
		
#ifdef USE_DOUBLE_RENDERING
        case GSP_STP_ID_DOUBLE_RENDERING:
	        {
	        	extern u_int ENG_gp_DoubleRendering;
				if (SetMenu)
				{
					p_OL -> Selection = 1;
					if (p_OL -> Open) 
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , ENG_gp_DoubleRendering);
				} 
				else
				{
    				if (Selection == 2) ENG_gp_DoubleRendering++;
    				if ((ENG_gp_DoubleRendering) &&
    				 	(Selection == 0)) ENG_gp_DoubleRendering--;

					p_OL -> Selection = 1;
/*					if (ENG_gp_DoubleRendering)	fGlobalAcc = 1.0f / (float)(ENG_gp_DoubleRendering + 1);
					else fGlobalAcc = 1.0f;
					fOoGlobalAcc = 1.0f / fGlobalAcc;//*/
				}
			}
#endif			
			break;
        case GSP_STP_ID_AE_WF_N3DWTR:
			if (SetMenu)
			{
				p_OL -> Selection = No3DWTR;
			} 
			else
			{
				No3DWTR = p_OL -> Selection;
			}
			break;
		case GSP_STP_ID_UNCACHE_SP:
			if (SetMenu)
			{
				p_OL -> Selection = 0;
				if (UncacheSP)	p_OL -> Selection = 1;
			} 
			else
			{
				UncacheSP &= ~0x30000000;
				if (p_OL -> Selection)
				{
					UncacheSP |= 0x30000000;
				}
				FlushCache(0);
				CleanSP(~0x30000000);
				FlushCache(0);
				SetSP(UncacheSP);
			}
			break;
		case GSP_STP_ID_DISLPAY_STACK_SPR:
			if (SetMenu)
			{
				p_OL -> Selection = DISPLAYSTACKSPR & 1;
			} 
			else
			{
				DISPLAYSTACKSPR &= 2;
				DISPLAYSTACKSPR |= p_OL -> Selection ;
			}
			break;
		case GSP_STP_ID_ENGINE_STACK_SPR:
			if (SetMenu)
			{
				p_OL -> Selection = DISPLAYSTACKSPR >> 1;
			} 
			else
			{
				DISPLAYSTACKSPR &= 1;
				DISPLAYSTACKSPR |= p_OL -> Selection << 1 ;
			}
			break;
#ifdef GSP_PS2_BENCH
		case GSP_STP_ID_ESTIMATESTACKSIZE:
			if (SetMenu)
			{
				p_OL -> Selection = StackGO;
			} 
			else
			{
				StackGO = Selection;
				if (!Selection) GspGlobal_ACCESS(Status) &= ~GSP_Status_Console_On;
			}
			break;
#endif			
#ifdef GSP_RECORD_IO_JOY_ENABLE
		case GSP_STP_ID_RIO:
			if (SetMenu)
			{
				p_OL -> Selection = RecordIO;
			} 
			else
			{
				RecordIO = p_OL -> Selection;
			}
			break;
#endif	
		case GSP_STP_ID_RAMRASTER:
			if (SetMenu)
			{
				p_OL -> Selection = MEM_i_RasterGetStatus();
			} 
			else
			{
				MEM_RasterSetRequest(Selection);
			}
			break;
	
		case GSP_STP_ID_IOP:
			if (SetMenu)
			{
				p_OL -> Selection = eeRPC_i_DebugGetStatus(0);
			} 
			else
			{
				eeRPC_DebugSetRequest(Selection);
			}
			break;
		
		case GSP_STP_ID_SOUNDDEBUG:
			if (SetMenu)
			{
				p_OL -> Selection = SND_i_DebugGetStatus(0);
			} 
			else
			{
				SND_DebugSetRequest(Selection);
			}
			break;
		
		case GSP_STP_ID_SOUNDFX:
			if (SetMenu)
			{
				p_OL -> Selection = SND_i_DebugGetStatus(0x100);
			} 
			else
			{
				SND_DebugSetRequest(Selection+0x100);
			}
			break;

		case GSP_STP_ID_SOUNDFXENABLE0:
			if (SetMenu)
			{
				p_OL -> Selection = SND_i_DebugGetStatus(0x200);
			} 
			else
			{
				SND_DebugSetRequest(Selection+0x200);
			}
			break;

		case GSP_STP_ID_SOUNDFXMODE0:
			if (SetMenu)
			{
				p_OL -> Selection = SND_i_DebugGetStatus(0x300);
			} 
			else
			{
				SND_DebugSetRequest(Selection+0x300);
			}
			break;

		case GSP_STP_ID_SOUNDFXDELAY0:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , ps2SND_i_FxGetDelay(0));
			} else
			{
				if((Selection == 0)||(Selection == 2))
				{
    				ps2SND_i_FxDelay = ps2SND_i_FxGetDelay(0);
    				if (Selection == 2) ps2SND_i_FxDelay += 10;
    				if (Selection == 0) ps2SND_i_FxDelay -= 10;
    				if(ps2SND_i_FxDelay > 1000) ps2SND_i_FxDelay = 1000;
    				if(ps2SND_i_FxDelay < 0) ps2SND_i_FxDelay = 0;
    				ps2SND_i_FxSetDelay(0, ps2SND_i_FxDelay);
    				ps2SND_FxFlushSettings(0);
    				ps2SND_i_FxDelay = ps2SND_i_FxGetDelay(0);
    			}
				p_OL -> Selection = 1;
			}
			break;


		case GSP_STP_ID_SOUNDFXWETVOL0:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					ps2SND_f_FxWetVol = SND_f_GetVolFromAtt(ps2SND_i_FxGetWetVolume(0));
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%2.3f" ,ps2SND_f_FxWetVol);
				}
			} else
			{
			    if((Selection == 0)||(Selection == 2))
			    {
					ps2SND_f_FxWetVol = SND_f_GetVolFromAtt(ps2SND_i_FxGetWetVolume(0));
    				if (Selection == 2) ps2SND_f_FxWetVol += 0.01f;
    				if (Selection == 0) ps2SND_f_FxWetVol -= 0.01f;
    				if(ps2SND_f_FxWetVol > 1.0f) ps2SND_f_FxWetVol = 1.0f;
    				if(ps2SND_f_FxWetVol < 0.0f) ps2SND_f_FxWetVol = 0.0f;
    				ps2SND_i_FxSetWetVolume(0, (int)SND_l_GetAttFromVol(ps2SND_f_FxWetVol));
    				ps2SND_FxFlushSettings(0);
    				
    			}
				p_OL -> Selection = 1;
			}
			break;

		case GSP_STP_ID_SOUNDFXFEEDBACK0:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , ps2SND_i_FxGetFeedback(0));
			} else
			{
			    if((Selection == 0)||(Selection == 2))
			    {
			    	ps2SND_i_FxFeedback = ps2SND_i_FxGetFeedback(0);
    				if (Selection == 2) ps2SND_i_FxFeedback += 2;
    				if (Selection == 0) ps2SND_i_FxFeedback -= 2;
    				if(ps2SND_i_FxFeedback > 100) ps2SND_i_FxFeedback = 100;
    				if(ps2SND_i_FxFeedback < 0) ps2SND_i_FxFeedback = 0;
    				ps2SND_i_FxSetFeedback(0, ps2SND_i_FxFeedback);
    				ps2SND_FxFlushSettings(0);
    				ps2SND_i_FxFeedback = ps2SND_i_FxGetFeedback(0);
    			}
				p_OL -> Selection = 1;
			}
			break;

		/*
		case GSP_STP_ID_SOUNDFXWETPAN0:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , ps2SND_i_FxGetWetPan(0));
			} else
			{
			    if((Selection == 0)||(Selection == 2))
			    {
    				if (Selection == 2) ps2SND_i_FxWetPan += 100;
    				if (Selection == 0) ps2SND_i_FxWetPan -= 100;
    				if(ps2SND_i_FxWetPan > 10000) ps2SND_i_FxWetPan = 10000;
    				if(ps2SND_i_FxWetPan < -10000) ps2SND_i_FxWetPan = -10000;
    				ps2SND_i_FxSetWetPan(0, ps2SND_i_FxWetPan);
    				ps2SND_FxFlushSettings(0);
    				ps2SND_i_FxWetPan = ps2SND_i_FxGetWetPan(0);
    			}
				p_OL -> Selection = 1;
			}
			break;
			*/

		case GSP_STP_ID_SOUNDFXENABLE1:
			if (SetMenu)
			{
				p_OL -> Selection = SND_i_DebugGetStatus(0xC00);
			} 
			else
			{
				SND_DebugSetRequest(Selection+0xC00);
			}
			break;

		case GSP_STP_ID_SOUNDFXMODE1:
			if (SetMenu)
			{
				p_OL -> Selection = SND_i_DebugGetStatus(0xD00);
			} 
			else
			{
				SND_DebugSetRequest(Selection+0xD00);
			}
			break;

		case GSP_STP_ID_SOUNDFXDELAY1:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , ps2SND_i_FxGetDelay(1));
			} else
			{
				if((Selection == 0)||(Selection == 2))
				{
    				ps2SND_i_FxDelay = ps2SND_i_FxGetDelay(1);
    				if (Selection == 2) ps2SND_i_FxDelay += 10;
    				if (Selection == 0) ps2SND_i_FxDelay -= 10;
    				if(ps2SND_i_FxDelay > 1000) ps2SND_i_FxDelay = 1000;
    				if(ps2SND_i_FxDelay < 0) ps2SND_i_FxDelay = 0;
    				ps2SND_i_FxSetDelay(1, ps2SND_i_FxDelay);
    				ps2SND_FxFlushSettings(1);
    				ps2SND_i_FxDelay = ps2SND_i_FxGetDelay(1);
    			}
				p_OL -> Selection = 1;
			}
			break;


		case GSP_STP_ID_SOUNDFXWETVOL1:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					ps2SND_f_FxWetVol = SND_f_GetVolFromAtt(ps2SND_i_FxGetWetVolume(1));
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%2.3f" ,ps2SND_f_FxWetVol);
				}
			} else
			{
			    if((Selection == 0)||(Selection == 2))
			    {
			    	ps2SND_f_FxWetVol = SND_f_GetVolFromAtt(ps2SND_i_FxGetWetVolume(1));
			    	ps2SND_i_FxSetWetVolume(1, (int)SND_l_GetAttFromVol(ps2SND_f_FxWetVol));
    				if (Selection == 2) ps2SND_f_FxWetVol += 0.01f;
    				if (Selection == 0) ps2SND_f_FxWetVol -= 0.01f;
    				if(ps2SND_f_FxWetVol > 1.0f) ps2SND_f_FxWetVol = 1.0f;
    				if(ps2SND_f_FxWetVol < 0.0f) ps2SND_f_FxWetVol = 0.0f;
    				ps2SND_i_FxSetWetVolume(1, (int)SND_l_GetAttFromVol(ps2SND_f_FxWetVol));
    				ps2SND_FxFlushSettings(1);
    				
    			}
				p_OL -> Selection = 1;
			}
			break;

		case GSP_STP_ID_SOUNDFXFEEDBACK1:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , ps2SND_i_FxGetFeedback(1));
			} else
			{
			    if((Selection == 0)||(Selection == 2))
			    {
			    	ps2SND_i_FxFeedback = ps2SND_i_FxGetFeedback(1);
    				if (Selection == 2) ps2SND_i_FxFeedback += 2;
    				if (Selection == 0) ps2SND_i_FxFeedback -= 2;
    				if(ps2SND_i_FxFeedback > 100) ps2SND_i_FxFeedback = 100;
    				if(ps2SND_i_FxFeedback < 0) ps2SND_i_FxFeedback = 0;
    				ps2SND_i_FxSetFeedback(1, ps2SND_i_FxFeedback);
    				ps2SND_FxFlushSettings(1);
    				ps2SND_i_FxFeedback = ps2SND_i_FxGetFeedback(1);
    			}
				p_OL -> Selection = 1;
			}
			break;

		/*case GSP_STP_ID_SOUNDFXWETPAN0:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , ps2SND_i_FxGetWetPan(0));
			} else
			{
			    if((Selection == 0)||(Selection == 2))
			    {
    				if (Selection == 2) ps2SND_i_FxWetPan += 100;
    				if (Selection == 0) ps2SND_i_FxWetPan -= 100;
    				if(ps2SND_i_FxWetPan > 10000) ps2SND_i_FxWetPan = 10000;
    				if(ps2SND_i_FxWetPan < -10000) ps2SND_i_FxWetPan = -10000;
    				ps2SND_i_FxSetWetPan(0, ps2SND_i_FxWetPan);
    				ps2SND_FxFlushSettings(0);
    				ps2SND_i_FxWetPan = ps2SND_i_FxGetWetPan(0);
    			}
				p_OL -> Selection = 1;
			}
			break;*/
		case GSP_STP_ID_SOUNDNOPLAY:
			if (SetMenu)
			{
				p_OL -> Selection = SND_i_DebugGetStatus(0x800);
			} 
			else
			{
				SND_DebugSetRequest(Selection+0x800);
			}
			break;
		case GSP_STP_ID_SOUNDNORQ:
			if (SetMenu)
			{
				p_OL -> Selection = SND_i_DebugGetStatus(0x900);
			} 
			else
			{
				SND_DebugSetRequest(Selection+0x900);
			}
			break;
		case GSP_STP_ID_SOUNDNOVOL:
			if (SetMenu)
			{
				p_OL -> Selection = SND_i_DebugGetStatus(0xA00);
			} 
			else
			{
				SND_DebugSetRequest(Selection+0xA00);
			}
			break;
		case GSP_STP_ID_SOUNDNOFREQ:
			if (SetMenu)
			{
				p_OL -> Selection = SND_i_DebugGetStatus(0xB00);
			} 
			else
			{
				SND_DebugSetRequest(Selection+0xB00);
			}
			break;

		case GSP_STP_ID_AE_GM: 
			if (SetMenu)
			{
				p_OL -> Selection = GspGlobal_ACCESS(p_AE_Params)->ulG_GlowMode & 1;
			} else {
				GspGlobal_ACCESS(p_AE_Params)->ulG_GlowMode &= ~1;
				GspGlobal_ACCESS(p_AE_Params)->ulG_GlowMode |= Selection;
			}
			break;
		
		case GSP_STP_ID_AE_WF_ZD:
			{
				extern float WTR_GetWaterZ();
				extern void WTR_SetWaterZ(float Z);
				if (SetMenu)
				{
					p_OL -> Selection = 1;
					if (p_OL -> Open) 
					{
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , WTR_GetWaterZ());
					}
				} else
				{
					if (Selection == 2) WTR_SetWaterZ(WTR_GetWaterZ() + 0.01f );
					if (Selection == 0) WTR_SetWaterZ(WTR_GetWaterZ() - 0.01f );
					p_OL -> Selection = 1;
				}
			}
				break;
		case GSP_STP_ID_AE_GMB: 
			FTS = &GspGlobal_ACCESS(p_AE_Params)->fG_GlowMotionBlur;
			UP_LMT	 = 1.0f;
			DOWN_LMT = 0.0f;
			goto WESET;
		case GSP_STP_ID_AE_GI: 
			FTS = &GspGlobal_ACCESS(p_AE_Params)->fG_Glowintensity;
			UP_LMT	 = 1.0f;
			DOWN_LMT = 0.0f;
			goto WESET;
		case GSP_STP_ID_AE_WF_ZP: 
			FTS = &GspGlobal_ACCESS(p_AE_Params)->fWR_ZPlane;
			UP_LMT	 = 100.0f;
			DOWN_LMT = -100.0f;
			goto WESET;
		case GSP_STP_ID_AE_WF_DF: // Dark factor
			FTS = &GspGlobal_ACCESS(p_AE_Params)->fWR_DarkFactor;
			UP_LMT	 = 1.0f;
			DOWN_LMT = 0.0f;
			goto WESET;
		case GSP_STP_ID_AE_WF_TM: // Transparency max 
			FTS = &GspGlobal_ACCESS(p_AE_Params)->fWR_TransparencyMax;
			UP_LMT	 = 1.0f;
			DOWN_LMT = 0.0f;
			goto WESET;
		case GSP_STP_ID_AE_WF_SE:// Sinus effect
			FTS = &GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffect;
			UP_LMT	 = 10.0f;
			DOWN_LMT = 0.0f;
			goto WESET;
		case GSP_STP_ID_AE_WF_SEB:// Sinus effect
			FTS = &GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffectBalance;
			UP_LMT	 = 1.0f;
			DOWN_LMT = 0.0f;
			goto WESET;
		case GSP_STP_ID_AE_WF_ZO:// Sinus effect
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , GspGlobal_ACCESS(p_AE_Params)->ulWR_ZOrder);
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->ulWR_ZOrder ++;
				if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->ulWR_ZOrder --;
				GspGlobal_ACCESS(p_AE_Params)->ulWR_ZOrder = lMin(lMax(GspGlobal_ACCESS(p_AE_Params)->ulWR_ZOrder , -8), 8);
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_WF_ZH: // Height
			FTS = &GspGlobal_ACCESS(p_AE_Params)->fWR_ZHeight;
			UP_LMT	 = 1.0f;
			DOWN_LMT = 0.0f;
			goto WESET;
WESET:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , *FTS);
				}
			} else
			{
				if (Selection == 2) *FTS += 0.01f * (float)(gs_intensity) / 50.0f;
				if (Selection == 0) *FTS -= 0.01f * (float)(gs_intensity) / 50.0f;
				*FTS = fMin(fMax(*FTS , DOWN_LMT), UP_LMT);
				p_OL -> Selection = 1;
			}
			break;
WESET2:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , *FTS);
				}
			} else
			{
				if (Selection == 2) *FTS += ((UP_LMT - DOWN_LMT) / 1000.0f)  * (float)(gs_intensity) / 50.0f;
				if (Selection == 0) *FTS -= ((UP_LMT - DOWN_LMT) / 1000.0f) * (float)(gs_intensity) / 50.0f;
				*FTS = fMin(fMax(*FTS , DOWN_LMT), UP_LMT);
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_SOUNDFORCE:
			if (SetMenu)
			{
				p_OL -> Selection = (SND_gst_Params.l_Available) ? 1 : 0;
			} 
			else
			{
				if (Selection)
					SND_ForceNoSound();
				else
					SND_ResetForceNoSound();
			}
			break;
		case GSP_STP_ID_SOUNDNOWAV:
			if (SetMenu)
			{
				p_OL -> Selection = (SND_gst_Params.ul_Flags & SND_Cte_ForceNoWav) ? 1 : 0;
			} 
			else
			{
				if (Selection)
					SND_ForceNo(SND_Cte_ForceNoWav);
				else
					SND_ResetForceNo(SND_Cte_ForceNoWav);
			}
			break;
		case GSP_STP_ID_SOUNDNOWAD:
			if (SetMenu)
			{
				p_OL -> Selection = (SND_gst_Params.ul_Flags & SND_Cte_ForceNoWad) ? 1 : 0;
			} 
			else
			{
				if (Selection)
					SND_ForceNo(SND_Cte_ForceNoWad);
				else
					SND_ResetForceNo(SND_Cte_ForceNoWad);
			}
			break;
		case GSP_STP_ID_SOUNDNOWAA:
			if (SetMenu)
			{
				p_OL -> Selection = (SND_gst_Params.ul_Flags & SND_Cte_ForceNoWaa) ? 1 : 0;
			} 
			else
			{
				if (Selection)
					SND_ForceNo(SND_Cte_ForceNoWaa);
				else
					SND_ResetForceNo(SND_Cte_ForceNoWaa);
			}
			break;
		case GSP_STP_ID_SOUNDNOWAM:
			if (SetMenu)
			{
				p_OL -> Selection = (SND_gst_Params.ul_Flags & SND_Cte_ForceNoWam) ? 1 : 0;
			} 
			else
			{
				if (Selection)
					SND_ForceNo(SND_Cte_ForceNoWam);
				else
					SND_ResetForceNo(SND_Cte_ForceNoWam);
			}
			break;
		case GSP_STP_ID_SOUNDNOSMD:
			if (SetMenu)
			{
				p_OL -> Selection = (SND_gst_Params.ul_Flags & SND_Cte_ForceNoSmd) ? 1 : 0;
			} 
			else
			{
				if (Selection)
					SND_ForceNo(SND_Cte_ForceNoSmd);
				else
					SND_ResetForceNo(SND_Cte_ForceNoSmd);
			}
			break;
			
		case GSP_STP_ID_RASTER:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_Raster_On)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				GspGlobal_ACCESS(Status) &= ~GSP_Status_Raster_On;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_Raster_On;
				else
					scePcStop();

			}
			break;
		case GSP_STP_ID_STRIP_ON:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_Strips_On)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				GspGlobal_ACCESS(Status) &= ~GSP_Status_Strips_On;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_Strips_On;
			}
			break;
			
		case GSP_STP_ID_RASTER_DETAILS:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_Raster_Details_On)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				GspGlobal_ACCESS(Status) &= ~GSP_Status_Raster_Details_On;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_Raster_Details_On;
				else
					scePcStop();
			}
			break;
		case GSP_STP_ID_RASTER_DETNUM:
			if (SetMenu)
			{
				p_OL -> Selection = (int)((RasterDetailMum - 4) >> 1);
			} else
			{
				RasterDetailMum = 4 + (((u_int)Selection) << 1);
			}
			break;
		case GSP_STP_ID_RASTER_Prop:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_Raster_Prop_On)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				GspGlobal_ACCESS(Status) &= ~GSP_Status_Raster_Prop_On;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_Raster_Prop_On;
			}
			break;
		case GSP_STP_ID_RASTER_MAX:
			if (SetMenu)
			{
				if (Rasters_MaxEnble)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				if (Selection) 
				{
					Rasters_MaxEnble |= 1;
				} else
					Rasters_MaxEnble = 0;

			}
			break;
		case GSP_STP_ID_2DFFX:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f " , GlobalBrightness);
			} else
			{
				if (Selection == 2) GlobalBrightness+= 0.01f;
				if (Selection == 0) GlobalBrightness-= 0.01f;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_2DFFY:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f " , GlobalContrast);
			} else
			{
				if (Selection == 2) GlobalContrast+= 0.01f;
				if (Selection == 0) GlobalContrast-= 0.01f;
				p_OL -> Selection = 1;
			}
			break;//*/
		case GSP_STP_ID_2DFFX_O:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.4f " , STR_f_XOffset);
			} else
			{
				if (Selection == 2) STR_f_XOffset+= 0.0001f;
				if (Selection == 0) STR_f_XOffset-= 0.0001f;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_2DFFY_O:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.4f " , STR_f_YOffset);
			} else
			{
				if (Selection == 2) STR_f_YOffset+= 0.0001f;
				if (Selection == 0) STR_f_YOffset-= 0.0001f;
				p_OL -> Selection = 1;
			}
			break;//*/
		case GSP_STP_ID_VHREPOS:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , GspGlobal_ACCESS(HRepos));
					
				}
			} else
			{
				GspGlobal_ACCESS(gs_DE1).display.DX -= GspGlobal_ACCESS(HRepos);
				GspGlobal_ACCESS(gs_DE2).display.DX -= GspGlobal_ACCESS(HRepos);
				GspGlobal_ACCESS(gs_DE1).display1.DX -= GspGlobal_ACCESS(HRepos);
				GspGlobal_ACCESS(gs_DE2).display1.DX -= GspGlobal_ACCESS(HRepos);
				
				if (Selection == 2) GspGlobal_ACCESS(HRepos)+= 4;
				if (Selection == 0) GspGlobal_ACCESS(HRepos)-= 4;
				GspGlobal_ACCESS(gs_DE1).display.DX += GspGlobal_ACCESS(HRepos);
				GspGlobal_ACCESS(gs_DE2).display.DX += GspGlobal_ACCESS(HRepos);
				GspGlobal_ACCESS(gs_DE1).display1.DX += GspGlobal_ACCESS(HRepos);
				GspGlobal_ACCESS(gs_DE2).display1.DX += GspGlobal_ACCESS(HRepos);
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_TOOLSHOWTEX:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_ShowTexture;
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , TextnumToShow);
				}
				else GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_ShowTexture;
			} else
			{
				if (Selection == 2) TextnumToShow++;
				if (Selection == 0) TextnumToShow--;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_TEST:
			if (SetMenu) p_OL -> Selection = 1;
			else
			{
				if (Selection == 2) 
				{
					GspGlobal_ACCESS(gs_DE1).display.DY++;
					GspGlobal_ACCESS(gs_DE2).display.DY++;
				} else
				if (Selection == 0) 
				{
					GspGlobal_ACCESS(gs_DE1).display.DY--;
					GspGlobal_ACCESS(gs_DE2).display.DY--;
				}
			}
//#define RONGING 40
		case GSP_STP_ID_GZFY:
			{
				if (SetMenu)
				{
					if (GspGlobal_ACCESS(Morfling) == 00) p_OL -> Selection = 0;
					if (GspGlobal_ACCESS(Morfling) == 40) p_OL -> Selection = 1;
					if (GspGlobal_ACCESS(Morfling) == 60) p_OL -> Selection = 2;
				} else
				{
					u_int NewMorf;
					
					/* Restore */
					NewMorf = 0;
					if (p_OL -> Selection == 1) /* 400 */
						NewMorf = 40;
					if (p_OL -> Selection == 2) /* 360 */
						NewMorf = 60;
					if (GspGlobal_ACCESS(Morfling) != NewMorf)
					{
						GspGlobal_ACCESS(Morfling) = NewMorf;
						Gsp_RefreshResolution();
					}
				}
			}
			break;
		case GSP_STP_ID_RASTER_MODE:
			if (SetMenu)
			{
				p_OL -> Selection = RasterMode & ~(1<<16);
			} else
			{
				RasterMode = p_OL -> Selection | (RasterMode & (1<<16));
			}
			break;		
		case GSP_STP_ID_AE_JPEG:
			if (SetMenu)
			{
				p_OL -> Selection = TgaMode;
			} else
			{
				TgaMode = p_OL -> Selection;
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
        
		case GSP_STP_ID_CONSOLE:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_Console_On )
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else {
				if ((Selection) && (!(GspGlobal_ACCESS(Status) & GSP_Status_Console_On)))
				{
					GSP_OutputConsole("\nConsole On\n");
					GspGlobal_ACCESS(Status) |= GSP_Status_Console_On;
				} else 	if (!Selection)GspGlobal_ACCESS(Status) &= ~GSP_Status_Console_On;

			}
			break;
		case GSP_STP_ID_SHOWVSYNC:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_SVS)
					p_OL -> Selection = 1; /* Off */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_SVS;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_SVS;
			}
			break;
		case GSP_STP_ID_CRTC_VAA:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_VAA_CRTS)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_VAA_CRTS;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_VAA_CRTS;
			}
			break;
		case GSP_STP_ID_SYNCSTABILITY:
			if (SetMenu)
			{
				p_OL -> Selection = GspGlobal_ACCESS(ulVSyncStability) >> 3;
			} else {
				GspGlobal_ACCESS(ulVSyncStability) = p_OL -> Selection << 3;
				}
			break;
			
		case GSP_STP_ID_SHOWNORMALS:
			if (SetMenu)
			{
				p_OL -> Selection = ShowNormals;
			} else {
				ShowNormals = Selection;
			}
			break;
		
#ifdef GSP_PS2_BENCH
		case GSP_STP_ID_SHOWDEPTHCOMPPLEXITY:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_Show_Depth)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_Show_Depth;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_Show_Depth;
			}
			break;
#endif			
		case GSP_STP_ID_16_9:
		{
			extern u32 ps2_ScreenMode;
			if (SetMenu)
			{
				p_OL -> Selection = ps2_ScreenMode;
			} else {
				ps2_ScreenMode = p_OL -> Selection;
			}
			}
			break;
			
		case GSP_STP_ID_DONTDRAW:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_VU1DontDraw)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_VU1DontDraw;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_VU1DontDraw;
			}
			break;
		case GSP_STP_ID_SETRENDERSTATE:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_SetRenderStateOn)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_SetRenderStateOn;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_SetRenderStateOn;
			}
			break;
		case GSP_STP_ID_AE_MB:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_MB)
				{
					p_OL -> Selection = GSP_SET_FloatToSelection(GspGlobal_ACCESS(p_AE_Params)->MotionBlurFactor);
				}
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_MB;
				if (Selection) 
				{
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_MB;
					GspGlobal_ACCESS(p_AE_Params)->MotionBlurFactor = GSP_SET_SelectionToFloat(p_OL -> Selection);
				}
			}
			break;
		case GSP_STP_ID_REMANANCE:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (GspGlobal_ACCESS(Status) & GSP_Status_AE_REMANANCE)
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor);
					else
					{
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "off");
						GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor = 0.0f;
					}
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor+= 0.01f;
				if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor-= 0.01f;
				if (GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor * GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor < 0.00005f) GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor = 0.0f;
				GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor = fMax(-1.0 , fMin(1.0f , GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor));
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_REMANANCE;
				if (GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor != 0.0f) 
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_REMANANCE;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_LoadTI:
			if (SetMenu)
			{
				p_OL -> Selection = LoadTextureInterface; /* On */
			} else {
				LoadTextureInterface = p_OL -> Selection ;
			}
			break;
		case GSP_STP_ID_AE_MODECOMP:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_ModeCompare)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_ModeCompare;
				if (Selection) 
				{	
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_ModeCompare;
				}
			}
			break;
		case GSP_STP_ID_AE_BrightNess:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(p_AE_Params)->Brighness);
					
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->Brighness+= 0.01f;
				if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->Brighness-= 0.01f;
				GspGlobal_ACCESS(p_AE_Params)->Brighness = fMax(0.0 , fMin(1.0f , GspGlobal_ACCESS(p_AE_Params)->Brighness));
				
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_Contrast:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(p_AE_Params)->Contrast);
					
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->Contrast+= 0.01f;
				if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->Contrast-= 0.01f;
				GspGlobal_ACCESS(p_AE_Params)->Contrast = fMax(0.0 , fMin(1.0f , GspGlobal_ACCESS(p_AE_Params)->Contrast));
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_DB:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_DB)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_DB;
				if (Selection) 
				{	
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_DB;
				}
			}
			break;
		case GSP_STP_ID_AE_DBNear:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart);
					
				}
			} else
			{
				if (GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart < 1.0f)
				{
					if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart += 0.01f;
					if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart -= 0.01f;
				} else
				{
					if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart*= 1.0f + 0.01f;
					if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart*= 1.0f - 0.01f;
				}
				if (GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart > GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd)
				     GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd = GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_DBFar:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd);
					
				}
			} else
			{
				if (GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd < 1.0f)
				{
					if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd += 0.01f;
					if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd -= 0.01f;
				} else
				{
					if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd*= 1.0f + 0.01f;
					if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd*= 1.0f - 0.01f;
				}
				if (GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart > GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd)
				     GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart = GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_DBNSet:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Near);
					
				}
			} else
			{
				if (GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Near < 1.0f)
				{
					if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Near += 0.01f;
					if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Near -= 0.01f;
				} else
				{
					if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Near*= 1.0f + 0.01f;
					if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Near*= 1.0f - 0.01f;
				}
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_DBN:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_DBN)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_DBN;
				if (Selection) 
				{	
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_DBN;
				}
			}
			break;
		case GSP_STP_ID_AE_SM:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_SM)
					p_OL -> Selection = GSP_SET_FloatToSelection(GspGlobal_ACCESS(p_AE_Params)->BlurFactor); /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_SM;
				if (Selection) 
				{
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_SM;
					GspGlobal_ACCESS(p_AE_Params)->BlurFactor = GSP_SET_SelectionToFloat(p_OL -> Selection);
				}
			}
			break;

		case GSP_STP_ID_AE_SMSM:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_SMSM)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_SMSM;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_AE_SMSM;
			}
			break;
		case GSP_STP_ID_AE_SHOWSDW:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_SHOWSDW)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_SHOWSDW;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_AE_SHOWSDW;
			}
			break;

		case GSP_STP_ID_AE_BW:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(p_AE_Params)->BWFactor != 0.0f)
					p_OL -> Selection = GSP_SET_FloatToSelection(GspGlobal_ACCESS(p_AE_Params)->BWFactor); /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(p_AE_Params)->BWFactor = GSP_SET_SelectionToFloat(p_OL -> Selection);
			}
			break;
		case GSP_STP_ID_AE_EMB:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_EMB)
					p_OL -> Selection = GSP_SET_FloatToSelection(GspGlobal_ACCESS(p_AE_Params)->EmbossFactor); /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_EMB;
				if (Selection) 
				{
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_EMB;
					GspGlobal_ACCESS(p_AE_Params)->EmbossFactor = GSP_SET_SelectionToFloat(p_OL -> Selection);
				}

			}
			break;
		case GSP_STP_ID_AE_WARP:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (GspGlobal_ACCESS(p_AE_Params)->fWarpFactor != 0.0f)
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , 	GspGlobal_ACCESS(p_AE_Params)->fWarpFactor);
					else
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "off");
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->fWarpFactor+= 0.01f;
				if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->fWarpFactor-= 0.01f;
				if (GspGlobal_ACCESS(p_AE_Params)->fWarpFactor * GspGlobal_ACCESS(p_AE_Params)->fWarpFactor < 0.00009f) GspGlobal_ACCESS(p_AE_Params)->fWarpFactor = 0.0f;
				p_OL -> Selection = 1;
			}
			break;
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_EMB)
					p_OL -> Selection = GSP_SET_FloatToSelection(GspGlobal_ACCESS(p_AE_Params)->EmbossFactor); /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_EMB;
				if (Selection) 
				{
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_EMB;
					GspGlobal_ACCESS(p_AE_Params)->EmbossFactor = GSP_SET_SelectionToFloat(p_OL -> Selection);
				}
			}
			break;
		case GSP_STP_ID_AE_SZB:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_SHOWZBUF)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_SHOWZBUF;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_AE_SHOWZBUF;
			}
			break;
			
		case GSP_STP_ID_CB_INTENSITY:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (GspGlobal_ACCESS(Status) & GSP_Status_AE_CB_On)
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(p_AE_Params)->CB_Intensity);
					else
					{
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "off");
					}
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->CB_Intensity+= 0.01f;
				if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->CB_Intensity-= 0.01f;
				GspGlobal_ACCESS(p_AE_Params)->CB_Intensity = fMax(0.0 , fMin(1.0f , GspGlobal_ACCESS(p_AE_Params)->CB_Intensity));
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_CB_On;
				if (GspGlobal_ACCESS(p_AE_Params)->CB_Intensity != 0.0f) GspGlobal_ACCESS(Status) |= GSP_Status_AE_CB_On;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_CB_SPECTRE:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(p_AE_Params)->CB_Spectre);
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->CB_Spectre+= 0.01f;
				if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->CB_Spectre-= 0.01f;
				if (GspGlobal_ACCESS(p_AE_Params)->CB_Spectre >= 1.0f) GspGlobal_ACCESS(p_AE_Params)->CB_Spectre -= 1.0f;
				if (GspGlobal_ACCESS(p_AE_Params)->CB_Spectre < 0.0f) GspGlobal_ACCESS(p_AE_Params)->CB_Spectre += 1.0f;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_CB_PASTEL:
			if (SetMenu)
			{
				p_OL -> Selection = GSP_SET_FloatToSelection(GspGlobal_ACCESS(p_AE_Params)->CB_Pastel);
			} else {
				GspGlobal_ACCESS(p_AE_Params)->CB_Pastel = GSP_SET_SelectionToFloat(p_OL -> Selection);
			}
			break;
		case GSP_STP_ID_CB_PF:
			if (SetMenu)
			{
				p_OL -> Selection = GSP_SET_FloatToSelection(GspGlobal_ACCESS(p_AE_Params)->CB_PhotoFilterEffect);
			} else {
				GspGlobal_ACCESS(p_AE_Params)->CB_PhotoFilterEffect = GSP_SET_SelectionToFloat(p_OL -> Selection);
			}
			break;
		case GSP_STP_ID_AE_RS:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (GspGlobal_ACCESS(Status) & GSP_Status_AE_RS)
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor);
					else
					{
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "off");
						GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor = 0.0f;
					}
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor+= 0.01f;
				if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor-= 0.01f;
				GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor = fMax(0.0 , fMin(1.0f , GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor));
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_RS;
				if (GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor != 0.0f) 
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_RS;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_AE_ZS:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (GspGlobal_ACCESS(Status) & GSP_Status_AE_ZS)
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor);
					else
					{
						sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "off");
						GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor = 0.0f;
					}
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor+= 0.01f;
				if (Selection == 0) GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor-= 0.01f;
				GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor = fMax(0.0 , fMin(1.0f , GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor));
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_ZS;
				if (GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor != 0.0f) 
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_ZS;
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_FOG_CORRECTION:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GspGlobal_ACCESS(fFogCorrector));
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(fFogCorrector)*= 1.010f;
				if (Selection == 0) GspGlobal_ACCESS(fFogCorrector)*= 0.990f;
//				GspGlobal_ACCESS(fFogCorrectorFar) = fMax(0.5f , GspGlobal_ACCESS(fFogCorrectorFar));
				p_OL -> Selection = 1;
			}
			break;
		case GSP_STP_ID_FOG_FAR_CORRECTION:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%.3f" , GspGlobal_ACCESS(fFogCorrectorFar));
				}
			} else
			{
				if (Selection == 2) GspGlobal_ACCESS(fFogCorrectorFar)*= 1.010f;
				if (Selection == 0) GspGlobal_ACCESS(fFogCorrectorFar)*= 0.990f;
//				GspGlobal_ACCESS(fFogCorrectorFar) = fMax(0.5f , GspGlobal_ACCESS(fFogCorrectorFar));
				p_OL -> Selection = 1;
			}
			break;//*/
		case GSP_STP_ID_TRICOUNTER:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_TNumCounterOn)
					p_OL -> Selection = 1; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_TNumCounterOn;
				if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_TNumCounterOn;
			}
			break;	
		case GSP_STP_ID_PALNTSC:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(ModePEnable))
					p_OL -> Selection = 2; /* DTV */
				else
				{
					if (GspGlobal_ACCESS(Status) & GSP_Status_PAL_On)
						p_OL -> Selection = 1; /* On */
					else
						p_OL -> Selection = 0;
				}
			} else {
				if (p_OL -> Selection == 2)
				{
					GspGlobal_ACCESS(ModePEnable) = 1;
				} else
				{
					GspGlobal_ACCESS(ModePEnable) = 0;
					GspGlobal_ACCESS(VideoMode) = (GspGlobal_ACCESS(VideoMode) + 1) % 3;
					GspGlobal_ACCESS(Status) &= ~GSP_Status_PAL_On;
					if (Selection) GspGlobal_ACCESS(Status) |= GSP_Status_PAL_On;
				}
				TIM_UpdateTimerFrequency();
				Gsp_RefreshResolution();
			}
			break;
		case GSP_STP_ID_AE_SHEETS:

#define VK_LSHIFT       0xA0
#define VK_RSHIFT       0xA1
#define VK_LCONTROL     0xA2
#define VK_RCONTROL     0xA3
#define VK_LMENU        0xA4
#define VK_RMENU        0xA5
#define VK_END			0x23
#define VK_HOME			0x24
#define VK_INSERT	 	0x2D

			if (!SetMenu)
			{
				extern void PS2_ResetWorld(void);
				switch (Selection) 
				{
					case 0:PS2_ResetWorld() ; break; // All objects KEY = 0x60 = NUMPAD0
					case 1:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x61 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // init entrepot KEY = 0x61 = NUMPAD1
					case 2:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x62 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // frizbee/gant = NUMPAD2
					case 3:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x63 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // init egout KEY = 0x63 = NUMPAD3
					case 4:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x65 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // init ilot KEY = 0x65 = NUMPAD5
					case 5:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x68 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // satellite KEY = NUMPAD8
					case 6:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = VK_INSERT ; ucVirtualPressedKey[1] = VK_LMENU ; break; // 
					case 7:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = VK_HOME ; ucVirtualPressedKey[1] = VK_LMENU ; break; // 
					case 8:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = VK_END ; ucVirtualPressedKey[1] = VK_LMENU ; break; // 
					default:iVirtualPressedKeyTime=0; break;
				}
			}
			break;
			
#ifdef GSP_USE_TEXTURE_CACHE
		case GSP_STP_ID_CachText:
			if (SetMenu)
			{
				p_OL -> Selection = GspGlobal_ACCESS(ulAvailableTextureCacheSize) >> 20; /* On */
			} else {
				GspGlobal_ACCESS(ulAvailableTextureCacheSize) = p_OL -> Selection << 20;
			}
			break;
#endif			
		case GSP_STP_ID_CBPP:
			if (SetMenu)
			{
				p_OL -> Selection = GspGlobal_ACCESS(BppMode); /* On */
			} else {
				GspGlobal_ACCESS(BppMode) = Selection;
				Gsp_RefreshResolution();
			}
			break;
		case GSP_STP_ID_ZBPP:
			if (SetMenu)
			{
				p_OL -> Selection = GspGlobal_ACCESS(ZBppMode); /* On */
			} else {
				GspGlobal_ACCESS(ZBppMode) = Selection;
				Gsp_RefreshResolution();
			}
			break;
		case GSP_STP_ID_HRES:
			if (SetMenu)
			{
				switch (GspGlobal_ACCESS(Xsize))
				{
					case 256:p_OL -> Selection = 0;break;
					case 320:p_OL -> Selection = 1;break;
					case 384:p_OL -> Selection = 2;break;
					case 512:p_OL -> Selection = 3;break;
					case 640:p_OL -> Selection = 4;break;
					case 720:p_OL -> Selection = 5;break;
				}
			} else {
				switch (Selection)
				{
					case 0:GspGlobal_ACCESS(Xsize) = 256;break;
					case 1:GspGlobal_ACCESS(Xsize) = 320;break;
					case 2:GspGlobal_ACCESS(Xsize) = 384;break;
					case 3:GspGlobal_ACCESS(Xsize) = 512;break;
					case 4:GspGlobal_ACCESS(Xsize) = 640;break;
					case 5:GspGlobal_ACCESS(Xsize) = 720;break;
				}
				Gsp_RefreshResolution();
			}
			break;
		case GSP_STP_ID_VRES:
			if (SetMenu)
			{
				p_OL -> Selection = GspGlobal_ACCESS(VideoMode); /* On */
				if (GspGlobal_ACCESS(Status) & GSP_Status_PAL_On)
					p_OL -> p_SubMenu = &Gsp_Stp_ResolutionVPAL;
				else
					p_OL -> p_SubMenu = &Gsp_Stp_ResolutionV;
			} else {
				VideoMode = Selection;
				Gsp_RefreshResolution();
			}
			break;
		case GSP_STP_ID_AE_TIMER:
			if (SetMenu)
			{
				if (fGlobalAcc == 0.125f) p_OL -> Selection = 0;
				if (fGlobalAcc == 0.25f) p_OL -> Selection = 1;
				if (fGlobalAcc == 0.5f) p_OL -> Selection = 2;
				if (fGlobalAcc == 1.0f) p_OL -> Selection = 3;
				if (fGlobalAcc == 2.0f) p_OL -> Selection = 4;
				if (fGlobalAcc == 4.0f) p_OL -> Selection = 5;
				if (fGlobalAcc == 8.0f) p_OL -> Selection = 6;
				if (fGlobalAcc == 200.0f) p_OL -> Selection = 7;
			} else {
				if (p_OL -> Selection == 0) fGlobalAcc = 0.125f;
				if (p_OL -> Selection == 1) fGlobalAcc = 0.25f;
				if (p_OL -> Selection == 2) fGlobalAcc = 0.5f;
				if (p_OL -> Selection == 3) fGlobalAcc = 1.0f;
				if (p_OL -> Selection == 4) fGlobalAcc = 2.0f;
				if (p_OL -> Selection == 5) fGlobalAcc = 4.0f;
				if (p_OL -> Selection == 6) fGlobalAcc = 8.0f;
				if (p_OL -> Selection == 7) fGlobalAcc = 200.0f;
			}
			fOoGlobalAcc = 1.0f / fGlobalAcc;
			break;
		case GSP_STP_ID_AE_BlurTest44:
			if (SetMenu)
			{
				if (GspGlobal_ACCESS(Status) & GSP_Status_AE_BlurTest44)
					p_OL -> Selection = GspGlobal_ACCESS(p_AE_Params)->AA22BlurMode; /* On */
				else
					p_OL -> Selection = 0;
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_AE_BlurTest44;
				if (Selection) 
				{
					GspGlobal_ACCESS(Status) |= GSP_Status_AE_BlurTest44;
					GspGlobal_ACCESS(p_AE_Params)->AA22BlurMode = Selection;
				}
			}
			break;
		case GSP_STP_ID_SHOWBPP:
			if (SetMenu)
			{
				p_OL -> Selection = ShowBpp; /* On */
			} else {
				ShowBpp = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_INTERP_C:
			if (SetMenu)
			{
				p_OL -> Selection = AI2C_ai2Ccan; /* On */
			} else {
				AI2C_ai2Ccan = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_MIRE:
			{
				if (SetMenu)
				{
					p_OL -> Selection = DisplayMire; /* On */
				} else {
					DisplayMire = p_OL -> Selection; /* On */
				}
			}break;
		case GSP_STP_ID_SPG2:
			{
				extern u32 SPG2_IsLocked;
				if (SetMenu)
				{
					p_OL -> Selection = SPG2_IsLocked ? 1 : 0; /* On */
				} else {
					if (p_OL -> Selection && (!SPG2_IsLocked))
					{
						/* force recompute spg2 */
						extern void SPG2_Modifier_FlushAll();
						SPG2_Modifier_FlushAll();
					}
					SPG2_IsLocked = p_OL -> Selection; /* On */
				}
			}break;

#ifdef MEM_SPY			
		case GSP_STP_ID_MEM_SPY:
			{
				extern u32 DisplayMemSpy;
				if (SetMenu)
				{
					p_OL -> Selection = DisplayMemSpy; /* On */
				} else {
					DisplayMemSpy = p_OL -> Selection; /* On */
				}
			}break;
#endif			

		case GSP_STP_ID_LOCKCAM:
			if (SetMenu)
			{
				p_OL -> Selection = GLOBAL_LockCam; /* On */
			} else {
				GLOBAL_LockCam = p_OL -> Selection; /* On */
			}
			break;
		
		case GSP_STP_ID_NO2D:
			if (SetMenu)
			{
				p_OL -> Selection = No2D; /* On */
			} else {
				No2D = p_OL -> Selection; /* On */
			}
			break;
		
		case GSP_STP_ID_MATDONTDRAW:
			if (SetMenu)
			{
				p_OL -> Selection = NoMATDRAW; /* On */
			} else {
				NoMATDRAW = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NO_MULTI:
			if (SetMenu)
			{
				p_OL -> Selection = NoMulti; /* On */
			} else {
				NoMulti = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NO_SDW:
			if (SetMenu)
			{
				p_OL -> Selection = NoSDW; /* On */
			} else {
				NoSDW = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NO_SPR:
			if (SetMenu)
			{
				p_OL -> Selection = NoSPR; /* On */
			} else {
				NoSPR = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NO_GFX:
			if (SetMenu)
			{
				p_OL -> Selection = NoGFX; /* On */
			} else {
				NoGFX = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NO_ZLST:
			if (SetMenu)
			{
				p_OL -> Selection = NoZLST; /* On */
			} else {
				NoZLST = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NO_SKN:
			if (SetMenu)
			{
				p_OL -> Selection = NoSKN; /* On */
			} else {
				NoSKN = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NO_ADM:
			if (SetMenu)
			{
				p_OL -> Selection = NoADM; /* On */
			} else {
				NoADM = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_PAG_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoPAG; /* On */
			} else {
				NoPAG = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_STR_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoSTR; /* On */
			} else {
				NoSTR = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_MDF_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoMDF; /* On */
			} else {
				NoMDF = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_GEODONTDRAW:
			if (SetMenu)
			{
				p_OL -> Selection = NoGEODRAW; /* On */
			} else {
				NoGEODRAW = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_GODONTDRAW:
			if (SetMenu)
			{
				p_OL -> Selection = NoGODRAW; /* On */
			} else {
				NoGODRAW = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_NOLIGHT:
			if (SetMenu)
			{
				p_OL -> Selection = NoLIGH; /* On */
			} else {
				NoLIGH = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_AE_ALLOFF:
			{
				GspGlobal_ACCESS(Status) &= ~(
						GSP_Status_AE_DB 			|
						GSP_Status_AE_MB 			|	
						GSP_Status_AE_SM 		|
						GSP_Status_AE_EMB 		|	
						GSP_Status_AE_SMSM		|	
						GSP_Status_AE_SHOWSDW	|	
						GSP_Status_AE_SHOWZBUF	|	
						GSP_Status_AE_ZS		|	
						GSP_Status_AE_RS		|	
						GSP_Status_AE_REMANANCE	|	
						GSP_Status_AE_DBN		|	
						GSP_Status_AE_BlurTest44	|	
						GSP_Status_AE_WATEREFFECT		|	
						GSP_Status_AE_ModeCompare);
				GspGlobal_ACCESS(p_AE_Params)->Brighness = 0.5f;
				GspGlobal_ACCESS(p_AE_Params)->Contrast = 0.5f;
				GspGlobal_ACCESS(p_AE_Params)->CB_Spectre = 0.0f;
				GspGlobal_ACCESS(p_AE_Params)->CB_Intensity = 0.0f;
				GspGlobal_ACCESS(p_AE_Params)->fWR_ZHeight = 0.0f;
			} 
			break;
			
		case GSP_STP_ID_AE_BIGSNAPSHOT:
			{
				Gsp_LaunchSnapShot(1);
				break;
			}
		case GSP_STP_ID_AE_SNAPSHOT:
			{
				Gsp_LaunchSnapShot(0);
				break;
			}
#ifdef 	GSP_PS2_BENCH
		case GSP_STP_ID_AE_SNAPSHOT_SEQ: 
			{
				SaveTimer = fGlobalAcc;
				bTakeSnapshot = 100 + 60 * 60 * 60; // one hour
				puc_Jpeg_Buffer_Dst = NULL;
				GSP_SNP_SetAA(0);
				fGlobalAcc = 2001.0f;
				GSP_VideoSeqIsOverSampled = 0;
				break;
			}
		case GSP_STP_ID_AE_SNAPSHOT_SEQ_OS: 
			{
				SaveTimer = fGlobalAcc;
				bTakeSnapshot = 100 + 60 * 60 * 60; // one hour
				GSP_SNP_SetAA(0);
				puc_Jpeg_Buffer_Dst = NULL;
				GSP_VideoSeqIsOverSampled = 1;
				fGlobalAcc = 2004.0f;
				break;
			}
#endif
		
		case GSP_STP_ID_SHOWVRAM:
			{
				u_int paddata,intensity;
				int X,Y,Counter;
				u_char rdata[32];
				GSP_tdst_DualShock2 *P_DS2;
				paddata = 0;
				X = Y = 0;
				Counter = 0;
				scePcStop();
				while (!(paddata & (STP_PAD_V | STP_PAD_L)))
				{
					paddata = 0;
					if(scePadRead(0, 0, rdata) > 0) 
						paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
					P_DS2 = (GSP_tdst_DualShock2 *)rdata;
					if (paddata & STP_PAD_D) 
					{
						intensity = P_DS2->InfoD;
						intensity = (u_int)(((float)intensity / 255.0f) * ((float)intensity / 255.0f) * 255.0f);
						Y += intensity;
					}
					if (paddata & STP_PAD_U) 
					{
						intensity = P_DS2->InfoU;
						intensity = (u_int)(((float)intensity / 255.0f) * ((float)intensity / 255.0f) * 255.0f);
						Y -= intensity;
					}
					
					Gsp_ShowVram(Y >> 14 , 0 , 32);
//					Gsp_SyncVB(0);
					Counter++;
				}
				GspGlobal_ACCESS(VideoMode) = (GspGlobal_ACCESS(VideoMode) + 1) % 3;
				Gsp_RefreshResolution();
			}
			break;
		case GSP_STP_ID_MAR:
			if (SetMenu)
			{
				p_OL -> Selection = AE_ON; /* On */
			} else {
				GspGlobal_ACCESS(Status) &= ~GSP_Status_Console_On;
				AE_ON = p_OL -> Selection; /* On */
				if (AE_ON) 	GspGlobal_ACCESS(Status) |= GSP_Status_Console_On;

			}
			break;
		case GSP_STP_ID_CLEARVRAM:
			GSP_ClearVRAM();
			break;
		case GSP_STP_ID_TSS:
//			GSP_SS_TestScreenSaver();
			break;
						
		case GSP_STP_ID_TFADE:
			Gsp_AE_FADE_PUSH(1.0f);
			break;
		case GSP_STP_ID_FGC:
			GSP_InvalidateGeoCache(65536);
			break;

		case GSP_STP_ID_CLEARTRAM:
			GSP_ShowAvailableTextureMemory();
			break;
						
		case GSP_STP_ID_NOVIDEO:
			if (SetMenu)
			{
				p_OL -> Selection = NoVideo; /* On */
			} else {
				NoVideo = p_OL -> Selection; /* On */
			}
			break;

		case GSP_STP_ID_NOPRELOAD:
			if (SetMenu)
			{
				p_OL -> Selection = NoPreload; /* On */
			} else {
				NoPreload = p_OL -> Selection; /* On */
			}
			break;
		case GSP_STP_ID_PRELOADDELAY:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(Gsp_Stp_PlusMoin.Leaves[1].Name , "%d" , eeRPC_gui_PreloadDelay);
				}
			} else
			{
				if (Selection == 2) eeRPC_gui_PreloadDelay += 1;
				if (Selection == 0) eeRPC_gui_PreloadDelay -= 1;
				p_OL -> Selection = 1;
			}
			break;

		case GSP_STP_ID_NULL: break;
	}
	if (p_OL->DefaultSelection == -1) p_OL->DefaultSelection = p_OL->Selection;
}
void AE_Report()
{
	char String[256];
	GSP_ClsConsole();
	GSP_OutputConsole("\nAfter effects report\n");
	GSP_OutputConsole("-----------------------------------------\n");
	sprintf(String , "Fog cor.:     %.2f\n" , GspGlobal_ACCESS(fFogCorrector));
	GSP_OutputConsole(String);
	sprintf(String , "Brightness:   %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->Brighness);
	GSP_OutputConsole(String);
	sprintf(String , "Contrast:     %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->Contrast);
	GSP_OutputConsole(String);
	sprintf(String , "CB_Intensity: %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->CB_Intensity);
	GSP_OutputConsole(String);
	sprintf(String , "CB_Spectre:   %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->CB_Spectre);
	GSP_OutputConsole(String);
	sprintf(String , "CB_PhotoFilt: %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->CB_PhotoFilterEffect);
	GSP_OutputConsole(String);
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_DB)	
	{
		sprintf(String , "Depth blur on\n    DB Start: %.2f\n    DB End:   %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZStart , GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_ZEnd);
		GSP_OutputConsole(String);
	}
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_DBN)	
	{
		sprintf(String , "Depth blur near on\n    DB Start: %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->Depth_Blur_Near);
		GSP_OutputConsole(String);
	}
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_WATEREFFECT)	
	{
		sprintf(String , "Water effect on\n     ZPlane:       %.2f\n     Height:       %.2f\n     Transparency: %.2f\n     Sinus:        %.2f\n     Sinus balance:%.2f\n     DarkFactor:   %.2f\n" , 
				GspGlobal_ACCESS(p_AE_Params)->fWR_ZPlane , 
				GspGlobal_ACCESS(p_AE_Params)->fWR_ZHeight , 
				GspGlobal_ACCESS(p_AE_Params)->fWR_TransparencyMax,
				GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffect,
				GspGlobal_ACCESS(p_AE_Params)->fWR_SinusEffectBalance,
				GspGlobal_ACCESS(p_AE_Params)->fWR_DarkFactor);
		GSP_OutputConsole(String);
	}
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_MB)	
	{
		sprintf(String , "Motion blur   %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->MotionBlurFactor);
		GSP_OutputConsole(String);
	}
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_SM)	
	{
		sprintf(String , "Blur 	        %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->BlurFactor);
		GSP_OutputConsole(String);
	}
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_EMB)	
	{
		sprintf(String , "Emboss        %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->EmbossFactor);
		GSP_OutputConsole(String);
	}
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_ZS)	
	{
		sprintf(String , "Zoom smooth   %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->ZoomSmoothFactor);
		GSP_OutputConsole(String);
	}
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_RS)	
	{
		sprintf(String , "Rotate smooth %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->SpinSmoothFactor);
		GSP_OutputConsole(String);
	}
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_REMANANCE)	
	{
		sprintf(String , "Remanance     %.2f\n" , GspGlobal_ACCESS(p_AE_Params)->RemanaceFactor);
		GSP_OutputConsole(String);
	}
	if (GspGlobal_ACCESS(Status) & GSP_Status_AE_BlurTest44)	
	{
		sprintf(String , "Blur 22 	    %d\n" , GspGlobal_ACCESS(p_AE_Params)->AA22BlurMode);
		GSP_OutputConsole(String);
	}
	GSP_OutputConsole("-----------------------------------------\n");
	GspGlobal_ACCESS(Status) |= GSP_Status_Console_On;
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

u_int Gsp_Stp_GetLenght(char *Str)
{	
	u_int Ret;
	Ret = 0 ;
	while (*(Str++)) Ret++;
	return Ret; 
}
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------ SNAP SHOT ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* CodeWarrior encoding form */
#pragma pack(1)
typedef struct
{
	u_long64	uc_Size : 8;
	u_long64	uc_ColorMapType : 8;
	u_long64	uc_ImageTypeCode : 8;
	u_long64	uw_Origin : 16;
	u_long64	uw_PaletteLength : 16;
	u_long64	uc_BPCInPalette : 8;
	u_long64	uw_Left : 16;
	u_long64	uw_Top : 16;
	u_long64	uw_Width : 16;
	u_long64	uw_Height : 16;
	u_long64	uc_BPP : 8;
	u_long64	ucDescriptorByte : 8;
} TEX_tdst_File_TgaHeader;
#pragma pack(0)
#define SIZEOFHEADERTGA sizeof(TEX_tdst_File_TgaHeader)

#define SNAPBUFSIZE (640 * 16)
extern void StoreZBUFVIF1( u_long128* base_addr, short start_addr, short pixel_mode, u_int bufferWith, short x, short y, short width, short height );
void GSP_SwapRB(ULONG *p , ULONG SIZE)
{
	while (SIZE--)
	{
		*p = (*p & 0xff00ff00) | ((*p & 0x000000ff) << 16) | ((*p & 0x00ff0000) >>16);
		p++;
	}
}
extern u_int *RamZBuffer;
void GSP_JpgConvert(u_char *SRC,u_char *DST,u_int PixelsNum)
{
	if (TgaMode)
	while (PixelsNum--)
	{
		*(DST++) = SRC[2];
		*(DST++) = SRC[1];
		*(DST++) = SRC[0];
		SRC+=4;
	}
	else
	while (PixelsNum--)
	{
		*(DST++) = *(SRC++);
		*(DST++) = *(SRC++);
		*(DST++) = *(SRC++);
		SRC++;
	}
}
u_int GSP_SequenceRecorderBases[MAX_Sequence_Images];
u_int GSP_SequenceRecorderSizes[MAX_Sequence_Images];
u_int GSP_SequenBase = 0;
u_int NameCounter = 0;
void GSP_Snapshot_Flush_Seq()
{
	L_FILE      h_TmpFile;	
	char TgAName[256];
	
	u_int SeqCounter;
	h_TmpFile = -1;
	
	scePcStop();
	
	Gsp_Flush(FLUSH_ALL);
	if (GspGlobal_ACCESS(ulFloatingFlipFlag)) Gsp_SyncVB(0);
	
	GSP_SequenBase &= ~1;
	if (!GSP_SequenBase) return;
	
	if (NameCounter == 0)
	{
		do 
		{
			if (h_TmpFile >= 0) {L_fclose(h_TmpFile);};
			sprintf(TgAName , "host0:x:/Snap/PS2_SEQ%05d.JPG" , NameCounter++);
			h_TmpFile = sceOpen(TgAName, OPEN_FILE_MODE);
		} 
		while (h_TmpFile >= 0);
		NameCounter --;
	}
	Gsp_Flush(FLUSH_ALL);	
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
	Gsp_Flush(FLUSH_ALL);	
	for (SeqCounter = 0 ; SeqCounter < GSP_SequenBase ; SeqCounter++)
	{
		Gsp_DrawRectangle(0.1f, 0.0f, 0.8f * (float)SeqCounter / (float)GSP_SequenBase , 0.1f  , 0xff , 0xff , GST_COPY);
		Gsp_Flush(FLUSH_ALL);	
		sprintf(TgAName , "host0:x:/Snap/PS2_SEQ%05d.JPG" , NameCounter++);
		h_TmpFile = sceOpen(TgAName, CREATE_FILE_MODE);
		if (h_TmpFile < 0) return;
		SPECIAL_Write((void *)GSP_SequenceRecorderBases[SeqCounter], 1, GSP_SequenceRecorderSizes[SeqCounter], h_TmpFile);
		L_fclose(h_TmpFile);
	}
	GSP_SequenBase = 0;
}

void GSP_Snapshot_Flush()
{
	L_FILE      h_TmpFile;
	void *p_DstJPG;	
	char TgAName[256];
	u_int ulNameNum;
	
	u_int SeqCounter;
	h_TmpFile = -1;
	ulNameNum = 0;
	GSP_SequenBase = 0;
	scePcStop();
	do 
	{
		if (h_TmpFile >= 0) {L_fclose(h_TmpFile);};
		if (TgaMode)
		{
			if (bTakeSnapshotShift == 6)
				sprintf(TgAName , "%sBigs/PS2_BIGSNP%03d.TGA" ,PSX2_Snap_Folder, ulNameNum++);
			else
				sprintf(TgAName , "%sPS2_SNP%03d.TGA" ,PSX2_Snap_Folder, ulNameNum++);
		} else
		{
			if (bTakeSnapshotShift == 6)
				sprintf(TgAName , "%sBigs/PS2_BIGSNP%03d.JPG" ,PSX2_Snap_Folder, ulNameNum++);
			else
				sprintf(TgAName , "%sPS2_SNP%03d.JPG" ,PSX2_Snap_Folder, ulNameNum++);
		}
		h_TmpFile = sceOpen(TgAName, OPEN_FILE_MODE);
	} 
	while (h_TmpFile >= 0);
	ulNameNum--;
	Gsp_Flush(FLUSH_ALL);	
	if (TgaMode)
	{
		if (bTakeSnapshotShift == 6)
			sprintf(TgAName , "%sBigs/PS2_BIGSNP%03d.TGA" ,PSX2_Snap_Folder, ulNameNum);
		else
			sprintf(TgAName , "%sPS2_SNP%03d.TGA" ,PSX2_Snap_Folder, ulNameNum);
	} else {
		if (bTakeSnapshotShift == 6)
			sprintf(TgAName , "%sBigs/PS2_BIGSNP%03d.JPG" ,PSX2_Snap_Folder, ulNameNum);
		else
			sprintf(TgAName , "%sPS2_SNP%03d.JPG" ,PSX2_Snap_Folder, ulNameNum);
	}
	if (bTakeSnapshotShift != 6)
	{
		h_TmpFile = sceOpen(TgAName, CREATE_FILE_MODE);
		if (h_TmpFile < 0) return;
		if (TgaMode)		
		{
			TEX_tdst_File_TgaHeader st_TgaHeader;
			L_memset(&st_TgaHeader , 0 , SIZEOFHEADERTGA);
			st_TgaHeader.uc_Size = 0;
			st_TgaHeader.uc_ColorMapType = 0;
			st_TgaHeader.uc_ImageTypeCode = 2;
			st_TgaHeader.uw_Origin = 0;
			st_TgaHeader.uw_PaletteLength = 0;
			st_TgaHeader.uc_BPCInPalette = 0;
			st_TgaHeader.uw_Left = 0;
			st_TgaHeader.uw_Top = 0;
			st_TgaHeader.uw_Width = GSP_SequenceRecorderSizes[0] & 0xffff;
			st_TgaHeader.uw_Height = GSP_SequenceRecorderSizes[0]>> 16;
			st_TgaHeader.uc_BPP = 24;
			st_TgaHeader.ucDescriptorByte = 1 << 5;
			SPECIAL_Write((void *)&st_TgaHeader, SIZEOFHEADERTGA, 1, h_TmpFile);
			SPECIAL_Write((void *)GSP_SequenceRecorderBases[0], st_TgaHeader.uw_Width * st_TgaHeader.uw_Height, 3, h_TmpFile);
			L_fclose(h_TmpFile);
			sprintf(TgAName , "%sPS2_SNP%03d_OS.TGA" ,PSX2_Snap_Folder, ulNameNum);
		} else
		{	
			JPEGDATA	st_JpegData;
			L_memset(&st_JpegData, 0, sizeof(JPEGDATA));
			st_JpegData.width = GSP_SequenceRecorderSizes[0] & 0xffff;
			st_JpegData.height = GSP_SequenceRecorderSizes[0]>> 16;
			st_JpegData.ptr = (unsigned char*)(u_char *)GSP_SequenceRecorderBases[0];
			st_JpegData.quality = 100;
			st_JpegData.output_file = (void *)(GSP_SnapRamBase);
			lJADE_JPEG_INPOUT_FILE_LENGHT = 0;
			JpegWrite(&st_JpegData);
			SPECIAL_Write((void *)(GSP_SnapRamBase), 1, lJADE_JPEG_INPOUT_FILE_LENGHT, h_TmpFile);
			L_fclose(h_TmpFile);
			sprintf(TgAName , "%sPS2_SNP%03d_OS.JPG" ,PSX2_Snap_Folder, ulNameNum);
		}
	}
	h_TmpFile = sceOpen(TgAName, CREATE_FILE_MODE);
	if (h_TmpFile < 0) return;
	/* Compute AA */
	{
		u_int W,H,OSCounter,Size;
		u_short *p_dest;
		u_char *pSrc;
		W = GSP_SequenceRecorderSizes[0] & 0xffff;
		H = GSP_SequenceRecorderSizes[0]>> 16;
		OSCounter = 1<< bTakeSnapshotShift;
		L_memset((void *)(GSP_SnapRamBase + 1024*1024) , 0 , W * H * 4 * 16);
		while (OSCounter--)
		{
			if (bTakeSnapshotShift == 6) 			
			{
				u_int WC,HC;
				u_int *p_uDST;
				p_uDST = (u_int *)(GSP_SnapRamBase + 1024*1024*1);
				HC = OSCounter ^ 0xffffffff;
				if (HC & 1) 		  p_uDST += 2;
				if (HC & (1 << 2)) 	  p_uDST += 1;
				if (HC & 2) 		  p_uDST += (W << 2) * 2;
				if (HC & (2 << 2)) 	  p_uDST += (W << 2) * 1;
				pSrc = (u_char *)GSP_SequenceRecorderBases[OSCounter];
				for (HC = 0 ; HC < H ; HC++)
				{
					for (WC = 0 ; WC < W ; WC++)
					{
						*p_uDST += (((u_int)(pSrc[0]))) + (((u_int)(pSrc[1])) << 10) + (((u_int)(pSrc[2])) << 20);
						pSrc+= 3;
						p_uDST+= 4;
					}
					p_uDST += W << 4;
					p_uDST -= W << 2;
				}
			} else
			{
				u_short *p_dest;
				p_dest = (u_short *)(GSP_SnapRamBase + 1024*1024);
				pSrc = (u_char *)GSP_SequenceRecorderBases[OSCounter];
				Size = W * H * 3;
				while (Size--)
				{
					*p_dest += (u_short)*pSrc;
					pSrc++;
					p_dest++;
				}
			}
		}
		if (bTakeSnapshotShift != 6) 
		{
			p_dest = (u_short *)(GSP_SnapRamBase + 1024*1024);
			pSrc = (u_char *)(GSP_SnapRamBase + 1024*1024);
			Size = W * H * 3;
			while (Size--)
			{
				*pSrc = (u_char)((*p_dest) >> bTakeSnapshotShift);
				pSrc++;
				p_dest++;
			}
		} else
		{
			u_int *p_uSrc , ulLocal;
			u_char *p_uDst;
			p_uSrc = (u_int *)(GSP_SnapRamBase + 1024*1024);
			p_uDst = (u_char *)(GSP_SnapRamBase + 1024*1024);
			Size = (W * H) << 4;
			while (Size--)
			{
				ulLocal = *(p_uSrc++);
				*(p_uDst++) = (u_char)((ulLocal) >> 2);
				*(p_uDst++) = (u_char)((ulLocal) >> 12);
				*(p_uDst++) = (u_char)((ulLocal) >> 22);
			}
		}
	}
	if (TgaMode)
	{
		TEX_tdst_File_TgaHeader st_TgaHeader;
		L_memset(&st_TgaHeader , 0 , SIZEOFHEADERTGA);
		st_TgaHeader.uc_Size = 0;
		st_TgaHeader.uc_ColorMapType = 0;
		st_TgaHeader.uc_ImageTypeCode = 2;
		st_TgaHeader.uw_Origin = 0;
		st_TgaHeader.uw_PaletteLength = 0;
		st_TgaHeader.uc_BPCInPalette = 0;
		st_TgaHeader.uw_Left = 0;
		st_TgaHeader.uw_Top = 0;
		st_TgaHeader.uw_Width = GSP_SequenceRecorderSizes[0] & 0xffff;
		st_TgaHeader.uw_Height = GSP_SequenceRecorderSizes[0]>> 16;
		if (bTakeSnapshotShift == 6)
		{
			st_TgaHeader.uw_Width  <<= 2;
			st_TgaHeader.uw_Height <<= 2;
		}
		
		st_TgaHeader.uc_BPP = 24;
		st_TgaHeader.ucDescriptorByte = 1 << 5;
		SPECIAL_Write((void *)&st_TgaHeader, SIZEOFHEADERTGA, 1, h_TmpFile);
		SPECIAL_Write((void *)(GSP_SnapRamBase + 1024*1024), st_TgaHeader.uw_Width * st_TgaHeader.uw_Height, 3, h_TmpFile);
		L_fclose(h_TmpFile);
		sprintf(TgAName , "%sPS2_SNP%03d_OS.TGA" ,PSX2_Snap_Folder, ulNameNum);
	} else {	
		JPEGDATA	st_JpegData;
		L_memset(&st_JpegData, 0, sizeof(JPEGDATA));
		st_JpegData.width = GSP_SequenceRecorderSizes[0] & 0xffff;
		st_JpegData.height = GSP_SequenceRecorderSizes[0]>> 16;
		if (bTakeSnapshotShift == 6)
		{
			st_JpegData.width <<= 2;
			st_JpegData.height <<= 2;
		}
		st_JpegData.ptr = (unsigned char*)(u_char *)(GSP_SnapRamBase + 1024*1024);
		st_JpegData.quality = 100;
		p_DstJPG = (void *)GSP_SequenceRecorderBases[0];
		p_DstJPG = (void *)(st_JpegData.ptr + (st_JpegData.width * st_JpegData.height * 4));
		
		st_JpegData.output_file = p_DstJPG;
		lJADE_JPEG_INPOUT_FILE_LENGHT = 0;
		JpegWrite(&st_JpegData);
	}
	SPECIAL_Write(p_DstJPG, 1, lJADE_JPEG_INPOUT_FILE_LENGHT, h_TmpFile);
	L_fclose(h_TmpFile);
	
}

void GSP_Snapshot(u_int bFullVRAM)
{
	TEX_tdst_File_TgaHeader st_TgaHeader;
	ULONG	*SNAP_BUFFER;
	u_int VPTR;
	u_int W,H,X,Y;
	u_int Border;
	unsigned char *puc_Jpeg_Buffer_Src;
	char TgAName[256];
	L_FILE      h_TmpFile;	
	scePcStop();
	SNAP_BUFFER = RamZBuffer;
	GSP_UnCachePtr(SNAP_BUFFER);
	puc_Jpeg_Buffer_Src = (unsigned char *)(GSP_SnapRamBase);
	if (bFullVRAM)
	{
		VPTR = 0;
		X = 0;
		Y = 0;
		W = GspGlobal_ACCESS(Xsize);
		H = (1024 * 1024) / W;
		Border = 0;
	} else
	{
		if (!(GSP_IsFrameODD()))
		{ // Select buffer 2 
			VPTR = GspGlobal_ACCESS(FBP1);
		} else 
		{ /* Select Buffer 1 */
			VPTR = GspGlobal_ACCESS(FBP2);
		}
		X = 0;
		Y = 0;
		W = GspGlobal_ACCESS(Xsize);
		Border = GspGlobal_ACCESS(Morfling) & ~31;
		H = GspGlobal_ACCESS(Ysize) - (Border << 1);
	}
	H = (H + 1) & 0xfffffffe;
	
	/* Find file name */
	/* Copy Frame buf */
	{
		/* Compute Max REC SIZE */
		u_int YPos,YSizePos;
		YPos = 0;
		VPTR <<= (11 - 6);
		YSizePos = SNAPBUFSIZE / W;
		while ((YPos + YSizePos) <= H)
		{
			L_memset(SNAP_BUFFER , 0 , YSizePos * W * 4 );
			StoreZBUFVIF1( SNAP_BUFFER, VPTR , SCE_GS_PSMCT32,GspGlobal_ACCESS(Xsize) , 0, YPos + Border, W, YSizePos );
			{
				GSP_JpgConvert((u_char *)SNAP_BUFFER,puc_Jpeg_Buffer_Src,YSizePos * W);
				puc_Jpeg_Buffer_Src += YSizePos * W * 3;
			}
			YPos += YSizePos;
		}
		if (YPos < H)
		{
			StoreZBUFVIF1( SNAP_BUFFER, VPTR , SCE_GS_PSMCT32, GspGlobal_ACCESS(Xsize),0 , YPos + Border, W, H - YPos );
			{
				GSP_JpgConvert((u_char *)SNAP_BUFFER,puc_Jpeg_Buffer_Src,(H - YPos) * W );
				puc_Jpeg_Buffer_Src += (H - YPos) * W * 3;
			}
		}
	}
	{
		u_int LocalH , LocalPitch , LocalCounter;
		unsigned char *p_dest,*p_Src;
		LocalH = H - 1;
		LocalPitch = W * 3;
		p_Src = (u_char *)(GSP_SnapRamBase);
		if (bTakeSnapshot > 90)  // Sequence 
		{
			LocalH >>= 1;
			LocalPitch <<= 1;
			if (GSP_SequenBase & 1) p_Src += W * 3;
		}
		if (bTakeSnapshot < 90)  // Snap 
		{ 
			if (puc_Jpeg_Buffer_Dst == NULL) puc_Jpeg_Buffer_Dst = (u_char *)(GSP_SnapRamBase + 1024*1024*(1 + 17));
			GSP_SequenceRecorderBases[GSP_SequenBase] 		= (u_int)puc_Jpeg_Buffer_Dst;
			p_dest = (u_char *)GSP_SequenceRecorderBases[GSP_SequenBase];
			LocalCounter = LocalH;
			while (LocalCounter--)
			{
				L_memcpy((void *)p_dest , (void *)p_Src , W * 3);
				p_dest+= W * 3;
				p_Src += LocalPitch;
			}
			GSP_SequenceRecorderSizes[GSP_SequenBase] = W | (LocalH << 16);
			puc_Jpeg_Buffer_Dst +=  W * LocalH * 3;
			GSP_SequenBase++;
			GSP_SNP_SetAA(GSP_SequenBase);
			fGlobalAcc = 5564.0f;
		} else // Snap sequence 
		{
			if (puc_Jpeg_Buffer_Dst == NULL) puc_Jpeg_Buffer_Dst = (u_char *)(GSP_SnapRamBase + 1024*1024*3);
			p_dest = (u_char *)(GSP_SnapRamBase);
			LocalCounter = LocalH;
			while (LocalCounter--)
			{
				L_memcpy((void *)p_dest , (void *)p_Src , W * 3);
				p_dest+= W * 3;
				p_Src += LocalPitch;
			}
			if (GSP_VideoSeqIsOverSampled)
			{
				if (GSP_VideoSeqIsOverSampled == 1) 
				{
					L_memset((void *)(GSP_SnapRamBase + 1024*1024) , 0 , 1024*1024 * 2);
				}
				/* Add to dest *//* Compute AA */
				{
					u_int Size;
					u_short *p_dest;
					u_char *pSrc;
					p_dest = (u_short *)(GSP_SnapRamBase + 1024*1024);
					pSrc = (unsigned char*)(u_char *)(GSP_SnapRamBase);
					Size = W * LocalH * 3;
					while (Size--)
					{
						*p_dest += (u_short)*pSrc;
						pSrc++;
						p_dest++;
					}
				}
				GSP_SNP_SetAA(GSP_VideoSeqIsOverSampled);
				if (GSP_VideoSeqIsOverSampled == 4)
				{
					u_int Size;
					u_char *p_dest;
					u_short *pSrc;
					p_dest = (u_char *)(GSP_SnapRamBase);
					pSrc = (u_short *)(GSP_SnapRamBase + 1024*1024);
					Size = W * LocalH * 3;
					while (Size--)
					{
						*p_dest = (u_char)((*pSrc) >> 2);
						pSrc++;
						p_dest++;
					}
					GSP_VideoSeqIsOverSampled = 1;
				}
				else 
				{
					GSP_VideoSeqIsOverSampled++;
					fGlobalAcc = 2004.0f;
					GSP_gb_ForcePauseJoy = TRUE;
					return;
				}
			}
			{		
				JPEGDATA	st_JpegData;
				L_memset(&st_JpegData, 0, sizeof(JPEGDATA));
				st_JpegData.width = W;
				st_JpegData.height = LocalH;
				st_JpegData.ptr = (unsigned char*)(u_char *)(GSP_SnapRamBase);
				st_JpegData.quality = 100;
				st_JpegData.output_file = (void *)puc_Jpeg_Buffer_Dst;
				lJADE_JPEG_INPOUT_FILE_LENGHT = 0;
				JpegWrite(&st_JpegData);
			}
			GSP_SequenceRecorderBases[GSP_SequenBase] = (u_int)puc_Jpeg_Buffer_Dst;
			GSP_SequenceRecorderSizes[GSP_SequenBase] = lJADE_JPEG_INPOUT_FILE_LENGHT;
			puc_Jpeg_Buffer_Dst +=  lJADE_JPEG_INPOUT_FILE_LENGHT;
			puc_Jpeg_Buffer_Dst = (u_char *)(((u_int)puc_Jpeg_Buffer_Dst + 31) & ~31);
			GSP_SequenBase++;
			GSP_gb_ForcePauseJoy = FALSE;
			fGlobalAcc = 2004.0f;
		}
/*
		if ((GSP_SequenBase == MAX_Sequence_Images) || (((u_int)puc_Jpeg_Buffer_Dst > 1024*1024*128-(1024*1024 * 4))&& (!(GSP_SequenBase & 1)))) // Save one SnapShot
		{
			GSP_Snapshot_Flush_Seq();
			puc_Jpeg_Buffer_Dst = NULL;
		}*/
	}
}


float SETUP_BigX = 0.0f;
float SETUP_BigY = 0.0f;
float Depest_X = 0.0f;
float Depest_Y = 0.0f;
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
#define GSP_STP_X2 0
#define GSP_STP_Y2 0

void Gsp_Stp_Draw(Gsp_SetupMenu *p_Root , int X , int Y,int bIsSelection ,int bIsDefaultSelection)
{
	u_int LC , Max;
	u_int Color;
	u_int ColorHL;
	char String[64];
	Max = 0;
	Gsp_Flush(FLUSH_ALL);
	p_Root->OpenSize += 4;
	Depest_X = X;
	Depest_Y = Y;
	
	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	if (Max < Gsp_Stp_GetLenght(p_Root->Leaves[LC].Name))
	{
		Max = Gsp_Stp_GetLenght(p_Root->Leaves[LC].Name);
	}
	if (Max < Gsp_Stp_GetLenght(p_Root->Title))
	{
		Max = Gsp_Stp_GetLenght(p_Root->Title);
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
	if ((X + SETUP_BigX + Max * 8) > GspGlobal_ACCESS(Xsize))
	{
		X = GspGlobal_ACCESS(Xsize) - (Max * 8);
	}
	{
		ULONG OPENC,ISOP;
		ISOP = 0xffffffff;
		OPENC = p_Root ->NumberOfLeaves;
		while (OPENC--) if (p_Root->Leaves[OPENC].Hlight || p_Root->Leaves[OPENC].Open) ISOP = OPENC;
		if (ISOP != 0xffffffff)
		{
			if ((SETUP_BigY + Y + (ISOP + 2) * 16 * (GSP_STP_Y2 + 1)) > (GspGlobal_ACCESS(Ysize) - 2 * GspGlobal_ACCESS(Morfling)))
			{
				Y -= SETUP_BigY + Y + (ISOP + 2) * 16 * (GSP_STP_Y2 + 1) - GspGlobal_ACCESS(Ysize) + 2 * GspGlobal_ACCESS(Morfling);
			}
		}
	}
	Y += GspGlobal_ACCESS(Morfling);
	if (p_Root->Title[0] != 0) 
	{
		GSP_DrawTExt_X2Y2(SETUP_BigX + X    ,SETUP_BigY + Y,String,			0,0,GST_COPY,GST_ALPHA , GSP_STP_X2 ,GSP_STP_Y2);
		GSP_DrawTExt_X2Y2(SETUP_BigX + X + 8  * (GSP_STP_X2 + 1),SETUP_BigY + Y,p_Root->Title	,	0x7070ff,0,GST_COPY,GST_SUB , GSP_STP_X2 ,GSP_STP_Y2);
	}
	if (p_Root->Title[0] != 0) Y += 16  * (GSP_STP_Y2 + 1);
	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	{
		u_int SC;
		char SaveSM;
		char SaveSMOnOf;
		SC = Color;
		
		if (LC == bIsSelection)
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
			GSP_DrawTExt_X2Y2(SETUP_BigX + X    ,SETUP_BigY + Y+LC * 16 * (GSP_STP_Y2 + 1),String,			Color,ColorHL,GST_COPY,GST_ALPHA , GSP_STP_X2 ,GSP_STP_Y2);
			GSP_DrawTExt_X2Y2(SETUP_BigX + X + 8  * (GSP_STP_X2 + 1),SETUP_BigY + Y+LC * 16 * (GSP_STP_Y2 + 1),p_Root->Leaves[LC].Name,	Color,0,GST_COPY,GST_SUB , GSP_STP_X2 ,GSP_STP_Y2);
		}
		String[Max - 1] = ' ';
		Color = SC;
		String[0] = ' ';
		String[Max - 1] = ' ';
		String[Max] = ' ';
		p_Root->Leaves[LC].Name[Max] = SaveSM;
		p_Root->Leaves[LC].Name[Max - 1] = SaveSMOnOf;
	}
	Y -= GspGlobal_ACCESS(Morfling);
	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	{
		if (p_Root->Leaves[LC].p_SubMenu)
		{
			if (p_Root->Leaves[LC].Open)
			{
				Gsp_Stp_Draw(p_Root->Leaves[LC].p_SubMenu, X+ Max * 8 * (GSP_STP_X2 + 1) + 4, Y+LC * 16 * (GSP_STP_Y2 + 1) , p_Root->Leaves[LC].Selection , p_Root->Leaves[LC].DefaultSelection);
			} else
				p_Root->Leaves[LC].p_SubMenu->OpenSize = 0;
			
		}
	}
	Gsp_Flush(FLUSH_ALL);	
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
static u_int b_first = 1;
void Gsp_Setup()
{
	u_int paddata , bValidDetected ;
	u_char rdata[32];
	static u_int CurrentFrameCounter = 0;
	static u_int NoOpenCloseCounter = 0;
	static u_int DontForceNoOpenCloseCounter = 1;
	GSP_tdst_DualShock2 *P_DS2;
	Gsp_SetupMenu *CurrentMenu;
	Gsp_Stp_OneLeaf *CurrentLeaf;
	{
		sprintf(Gsp_Stp_Root.Title ,"%s %s (%d)" , __DATE__, __TIME__, BIG_Cu4_AppVersion );
		if (TextureUsed)
			sprintf(Gsp_Stp_Tool.Title ,"Texture not fit !!!!" );
		else
			sprintf(Gsp_Stp_Tool.Title ,"Texture fit in Cache" );
		TextureUsed = 0;
	}
	Gsp_FIFO_STOP();
	Gsp_Flush(FLUSH_ALL);
	if (GspGlobal_ACCESS(ulFloatingFlipFlag)) Gsp_SyncVB(0);
	
	paddata = 0;
	PS2_MenuIsOpen = 1;
	if(iVirtualPressedKeyTime)
	{
	    iVirtualPressedKeyTime--;
	}
	else
	{
	
	    ucVirtualPressedKey[0] = ucVirtualPressedKey[1] =ucVirtualPressedKey[2]= 0;
	}
	if (b_first) GSP_STP_FirstInt();
	b_first = 0;
	if (bTakeSnapshot)
	{
		bTakeSnapshot--;
		
		FlushCache(0);
		GSP_Snapshot(0);
		if (bTakeSnapshot == 100)
		{
			GSP_Snapshot_Flush_Seq();
			bTakeSnapshot = 0;
		}
		if (bTakeSnapshot == 1)
		{
			GSP_Snapshot_Flush();
			bTakeSnapshot = 0;
		}
		return;
	}
	fAddXAA = 0.0f;
	fAddYAA = 0.0f;
#ifdef GSP_PS2_BENCH	
	if (SaveXSize)
	{
		GspGlobal_ACCESS(Xsize) = SaveXSize;
		GspGlobal_ACCESS(Morfling) = SaveGounzafiot;
		Gsp_RefreshResolution();
		SaveXSize = 0;
	}
	if (fGlobalAcc > 2000.0f) fGlobalAcc= SaveTimer; 
#endif	
	ENG_gb_ForcePauseEngine = FALSE;
	GSP_gb_ForcePauseJoy = FALSE;
	
	Gsp_FisrtInitMenu(&Gsp_Stp_Root); // <= Extra SubCountering Xor Vlidate
	if(scePadRead(0, 0, rdata) > 0) paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
	
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
	
	
	VideoMode = GspGlobal_ACCESS(VideoMode);

	bValidDetected = 0;
	paddataXor ^= paddata;
	paddataXor &= paddata;
	Gsp_InitMenu(&Gsp_Stp_Root);
	CurrentMenu = Gsp_Stp_GetMenu(&Gsp_Stp_Root , Depth , (u_int*)DepthStack);
	CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
	NoOpenCloseCounter ++;
	if (paddataXor & STP_PAD_R)
	{

		if (CurrentLeaf->p_SubMenu)
		{
			CurrentLeaf->Open = 1;
			NoOpenCloseCounter = 0;
			DepthStack[++Depth] = CurrentLeaf->Selection % CurrentMenu->NumberOfLeaves;
			if (CurrentLeaf->Selection == -1) DepthStack[Depth] = 0;
			else 
			{
			   Gsp_Validate(CurrentLeaf , 0 , 1);
			   DepthStack[Depth] = CurrentLeaf->Selection;
			}
		} else 
		{
			bValidDetected = 1;
		}
	} else
	if (paddataXor & STP_PAD_L)
	{
		NoOpenCloseCounter = 0;
		CurrentLeaf->Open = 0;
		if (Depth != 0)
		{
			Depth --;
		} else goto SetupExit;
		CurrentMenu = Gsp_Stp_GetMenu(&Gsp_Stp_Root , Depth , (u_int*)DepthStack);
		CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
		CurrentLeaf->Open = 0;
	} else
	if (paddataXor & STP_PAD_D)
	{
		DepthStack[Depth]++;
		DepthStack[Depth] %= CurrentMenu->NumberOfLeaves;
	} else
	if (paddataXor & STP_PAD_U)
	{
		if (DepthStack[Depth] == 0)
			DepthStack[Depth] = CurrentMenu->NumberOfLeaves - 1;
		else
			DepthStack[Depth]--;
	}
	
   	CurrentMenu = Gsp_Stp_GetMenu(&Gsp_Stp_Root , Depth , (u_int*)DepthStack);
	CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
	if (CurrentMenu->AutoSelection) 
		bValidDetected = 1;
	
	
	/*if (paddataXor & SCE_PADL1)
	{
		DontForceNoOpenCloseCounter ^= 1;
		NoOpenCloseCounter = 0;
	}*/
	if ((paddataXor & STP_PAD_V) || bValidDetected)
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
	Gsp_Stp_Draw(&Gsp_Stp_Root ,10 , 10 , -1 , -1);
	if (DontForceNoOpenCloseCounter) NoOpenCloseCounter = 1000000;
	if(NoOpenCloseCounter > 400 / GspGlobal_ACCESS(fCurrentFrquency)) 
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
		if(NoOpenCloseCounter > 400 / GspGlobal_ACCESS(fCurrentFrquency)) 
		{
			NoOpenCloseCounter = 100000;
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
	}
	CurrentLeaf -> Hlight = 0;
	paddataXor = paddata;
	if (AE_ON) AE_Report();
	return;
SetupExit:	
	DontForceNoOpenCloseCounter = 1;
	NoOpenCloseCounter = 0;
	Depth = 0;
	DepthStack[0] = 0;
	if (AE_ON) 	GspGlobal_ACCESS(Status) &= ~GSP_Status_Console_On;
	AE_ON = 0;
	PS2_MenuIsOpen = 0;
	GspGlobal_ACCESS(Status) &= ~GSP_Status_Setup_On;
	Gsp_Stp_MenuClose(&Gsp_Stp_Root);
}

int GSP_scePadRead( int port, int slot, unsigned char* rdata )
{
	int RetValue;
	RetValue = scePadRead( port, slot, rdata );
#ifdef GSP_RECORD_IO_JOY_ENABLE	
	bForce60 = 0;
	if (RecordIO) bForce60 = 1;

	if (RetValue > 0)
	{
		static float FirstOCLOCK = 0.0f;
		if (RecordIO)
		{
			u_int paddata;
			/* DETECT F6 */
			paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
			if ((paddata & (SCE_PADL1 | SCE_PADL2 | SCE_PADR1 | SCE_PADR2 | SCE_PADstart | SCE_PADselect)) == (SCE_PADL1 | SCE_PADL2 | SCE_PADR1 | SCE_PADR2 | SCE_PADstart | SCE_PADselect))
			{
				FirstOCLOCK = 0.0f;
				if (RecordIO == 1) // FirstStart
				{
					RecordIO = 6;
					RecordIOBase = (u_char *)RecorIOMEM_BASE;
				} else
				if (RecordIO == 2)
				{
					RecordIO = 7;
					RecordIOBase = (u_char *)RecorIOMEM_BASE;
				} else
				if (RecordIO == 3)
				{
					RecordIO = 7;
					RecordIOBase = (u_char *)RecorIOMEM_BASE;
				}

			} else
			if ((RecordIO == 2) || (RecordIO == 6)) /* record Each Frame */
			{
				RecordIO = 2;
				*(float *)RecordIOBase = FirstOCLOCK;
				RecordIOBase += 4;
				L_memcpy(RecordIOBase , rdata , 64);
				RecordIOBase += 64;
				*(u_int *)RecordIOBase = 0xC0DE2004;
				FirstOCLOCK += TIM_gf_dt;
			} else
			if ((RecordIO == 3) || (RecordIO == 7)) /* play Each Frame */
			{
				RecordIO = 3;
				if (*(u_int *)RecordIOBase == 0xC0DE2004) //Mean Is the End;
				{
				} else
				{
					/* play Each Frame */
					RecordIOBase += 4;
					L_memcpy(rdata , RecordIOBase , 64);
					RecordIOBase += 64;
					if (GSP_gb_ForcePauseJoy)
						RecordIOBase -= 68;
						
					
				}
				FirstOCLOCK += TIM_gf_dt;
			}
		}
	}
#endif	
	return RetValue;
}


#endif // _FINAL_

void GSP_SpecialInit(void)
{
	//VSync Stability = 70
	GspGlobal_ACCESS(ulVSyncStability) = 0/*7 << 3*/;
}

#if defined PSX2_TARGET && defined __cplusplus
}
#endif


//#endif

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
 
#include "GEOmetric/GEOobject.h"
#include "GXI_init.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGvars.h"

#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/gc/gcSND_debug.h"
#include "SouND/Sources/gc/gcSNDfx.h"
#include "SouND/Sources/gc/gcSND_Profile.h"
#include "INOut/INOjoystick.h"

#include "GXI_dbg.h"
#include "GXI_SpecialFX.h"

#include "BIGfiles/VERsion/VERsion_Number.h"

#define u_int unsigned int
u32 GC_MenuIsOpen = 0;

GXTevSwapSel g_iCurrentColorSwap = GX_TEV_SWAP0;
BOOL		 g_bUseDithering = TRUE;
int GXI_gb_MenuMode=0;
int GXI_gi_EnginePadState=0;

/*u32 NoMATDRAW = 0;
u32 NoGEODRAW = 0;
u32 NoGODRAW = 0;
u32 NoLIGH = 0;
u32 NoMulti = 0;
u32 NoSDW = 0;
u32 NoSPR = 0;
u32 NoGFX = 0;
u32 NoZLST = 0;
u32 NoSKN = 0;
u32 NoADM = 0;
u32 NoPAG = 0;
u32 NoSTR = 0;
u32 NoMDF = 0;*/

u32 TextnumToShow=0;

static u32 Depth = 0;
static s32 DepthStack[12];

extern unsigned char ucVirtualPressedKey[3];
int iVirtualPressedKeyTime=0;

#ifndef _FINAL_

#define GC_STP_ID_NULL				 0
#define GC_STP_ID_RASTER			 1
#define GC_STP_ID_EXIT				 4
#define GC_STP_ID_DONTDRAW			 6
#define GC_STP_ID_SETRENDERSTATE	 15
#define GC_STP_ID_AE_MB				 17
#define GC_STP_ID_AE_DB				 18
#define GC_STP_ID_AE_SM				 19
#define GC_STP_ID_MATDONTDRAW		 20
#define GC_STP_ID_GEODONTDRAW		 21
#define GC_STP_ID_GODONTDRAW		 22
#define GC_STP_ID_NOLIGHT			 23
#define GC_STP_ID_AE_EMB			 25
#define GC_STP_ID_AE_SMSM			 26
#define GC_STP_ID_AE_ZS				 29
#define GC_STP_ID_AE_TIMER			 30
#define GC_STP_ID_AE_BLUR			 31
#define GC_STP_ID_AE_DBN		     54
#define GC_STP_ID_AE_BrightNess      55
#define GC_STP_ID_AE_Contrast	     56
#define GC_STP_ID_CB_INTENSITY		 57
#define GC_STP_ID_CB_SPECTRE		 58
#define GC_STP_ID_CB_PF				 59
#define GC_STP_ID_AE_RS				 71
#define GC_STP_ID_AE_ALLOFF			 72
#define GC_STP_ID_NO_MULTI			 75
#define GC_STP_ID_NO_SDW			 76
#define GC_STP_ID_NO_SPR			 77
#define GC_STP_ID_NO_GFX			 78
#define GC_STP_ID_NO_ZLST			 79
#define GC_STP_ID_NO_SKN			 80
#define GC_STP_ID_NO_ADM			 81
#define GC_STP_ID_TOOLSHOWTEX		 88
#define GC_STP_ID_PAG_ON			 90
#define GC_STP_ID_STR_ON			 91
#define GC_STP_ID_MDF_ON			 92
#define GC_STP_ID_AE_BW    			 94
#define GC_STP_ID_Gamma				 95
#define GC_STP_ID_AE_DBNear			 99
#define GC_STP_ID_AE_DBFar			 100
#define GC_STP_ID_FOG_CORRECTION   	 101
#define GC_STP_ID_AE_DBNSet			 102
#define GC_STP_ID_RASTER_FILTER		 117
#define GC_STP_ID_SHOWVTXCOLOR		 120
#define GC_STP_ID_SHOWSKINELE		 121
#define GC_STP_ID_SHOWNORMALS		 122
#define GC_STP_ID_SHOWLIGHTS		 123
#define GC_STP_ID_USEHWTEXMATRIX	 124
#define GC_STP_ID_USEHWLIGHTS		 125
#define GC_STP_ID_USEHWSTATES		 126
#define GC_STP_ID_STORMINTENSITY	 127
#define GC_STP_ID_STORMSPEED    	 128
#define GC_STP_ID_STORMAMPLITUDE   	 129
#define GC_STP_ID_STORMGRAINSIZEF  	 130
#define GC_STP_ID_STORMGRAINSIZEB  	 131
#define GC_STP_ID_STORMACTIVATION  	 132
#define GC_STP_ID_COLORSWAP     	 133
#define GC_STP_ID_USEDITHERING     	 134
#define GC_STP_ID_CHEAT              135
#define GC_STP_ID_USEHWMUL2X		 136
//////////////////////
#define	GC_STP_ID_SOUNDAUXAENABLE    137
#define	GC_STP_ID_SOUNDAUXAMODE      138
#define	GC_STP_ID_SOUNDAUXAVOL       139
#define	GC_STP_ID_SOUNDAUXADELAY     140
#define	GC_STP_ID_SOUNDAUXAFEEDBACK  141


#define GC_STP_ID_SOUNDPROFILE       150
#define GC_STP_ID_SOUNDRESETPROFILE  151
/////////////////////
#define GC_STP_ID_RASTER_NUMBER		152
#define GC_STP_ID_RASTER_SORTED		153
#define GC_STP_ID_RASTER_MAX		154
#define GC_STP_ID_DTNUM				155
#define GC_STP_ID_RASTER_RAM        156

#define GC_STP_ID_WTR_LOCK			157
#define GC_STP_ID_WTR_ZFactor		158
#define GC_STP_ID_WTR_ChoppyFactor	159
#define GC_STP_ID_WTR_TileFactor	160
#define GC_STP_ID_WTR_WaterZ		161
#define GC_STP_ID_WTR_SpeedFactor	162
#define GC_STP_ID_WTR_WIRED			163
#define GC_STP_ID_WTR_DisturbFactor 164
#define GC_STP_ID_WTR_DepthFog		165
#define GC_STP_ID_WTR_ZOrder		166
#define GC_STP_ID_AE_REMANANCE		167
#define GC_STP_ID_RAM				168
#define GC_STP_ID_VIDEO				169

#define GC_STP_ID_FUR_ON			170
#define GC_STP_ID_SPG2_ON			171

struct GXI_SetupMenu_;

extern float fGlobalAcc;

typedef struct GXI_Stp_OneLeaf_
{
	char  Name[40];
	u32 Hlight;
	u32 Open;
	s32 Selection;
	u32 ID;
	struct GXI_SetupMenu_ * p_SubMenu;
	u32 DefaultSelection;
} GXI_Stp_OneLeaf;

typedef struct GXI_SetupMenu_
{	
	CHAR  Title[40];
	u32 NumberOfLeaves;
	u32 OpenSize;
	u32 AutoSelection;
	GXI_Stp_OneLeaf Leaves[32];
} GXI_SetupMenu;

static GXI_SetupMenu GXI_Stp_Root ;
static GXI_SetupMenu GXI_Stp_Water;
static GXI_SetupMenu GXI_Stp_Rasters;
static GXI_SetupMenu GXI_Stp_Rasters_RAM;
static GXI_SetupMenu GXI_Stp_Rasters_MODE;
static GXI_SetupMenu GXI_Stp_OnOff;
static GXI_SetupMenu GXI_Stp_ResolutionH ;
static GXI_SetupMenu GXI_Stp_3D ;
static GXI_SetupMenu GXI_Stp_AE;
static GXI_SetupMenu GXI_Stp_SpecialFX;
static GXI_SetupMenu GXI_Stp_SandStorm;
static GXI_SetupMenu GXI_Stp_Timer;
static GXI_SetupMenu GXI_Stp_0_To_100;
static GXI_SetupMenu GXI_Gamma;
static GXI_SetupMenu GXI_Stp_DistanceSet;
static GXI_SetupMenu GXI_Stp_Debug;
static GXI_SetupMenu GXI_Stp_PlusMoin;
static GXI_SetupMenu GXI_Stp_ColorBalance;
static GXI_SetupMenu GXI_Stp_DepthBluringSetting;
static GXI_SetupMenu GXI_Stp_WaterReflection;
static GXI_SetupMenu GXI_Stp_Cheat;
static GXI_SetupMenu GXI_Stp_ColorSwap;
static GXI_SetupMenu GXI_Stp_Sound;
static GXI_SetupMenu GXI_Stp_SoundProfiler;
static GXI_SetupMenu GXI_Stp_SoundAuxA;

extern void GXI_DrawText(int X,int Y,char *P_String,long Color,long ColorBack,long TFront,long TBack);

void GXI_FisrtInitMenu(GXI_SetupMenu *p_Root)
{
	u32 LC;
	
	p_Root -> NumberOfLeaves = 0;
	while (p_Root -> Leaves[p_Root -> NumberOfLeaves].Name[0] != 0)
		p_Root -> NumberOfLeaves ++;
	LC = p_Root -> NumberOfLeaves;
	while (LC--)
	{
		if (p_Root -> Leaves[LC].p_SubMenu) 
		 GXI_FisrtInitMenu(p_Root -> Leaves[LC].p_SubMenu);
	}
}

float GC_SET_SelectionToFloat(ULONG Selection)
{
	return ((float)Selection) / 10.0f;
}

float GC_SET_SelectionToFloat2(ULONG Selection)
{
	return ((float)Selection) / 20.0f;
}

float GC_SET_SelectionToFloat3(ULONG Selection)
{
	return ((float)Selection) / 12.0f;
}

ULONG GC_SET_FloatToSelection(float fValue)
{
	return (ULONG)(fValue * 10.0f);
}
ULONG GC_SET_FloatToSelection2(float fValue)
{
	return (ULONG)(fValue * 20.0f);
}

ULONG GC_SET_FloatToSelection3(float fValue)
{
	return (ULONG)(fValue * 12.0f);
}

void GC_SET_DeltaDistanceGet(ULONG Selection , float *Dist)
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
void GC_SET_DeltazGet(ULONG Selection , float *Dist)
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

void GC_STP_FirstInt()
{
	int Leave;

	// on/off menu

	L_strcpy(&GXI_Stp_OnOff.Title, "");
	GXI_Stp_OnOff.NumberOfLeaves = 0;
	GXI_Stp_OnOff.OpenSize = 0;
	GXI_Stp_OnOff.AutoSelection = 1;	
	
	Leave = 0;
	GXI_Stp_OnOff.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"NO",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_OnOff.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"YES",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_OnOff.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	
	
	// +/- menu

	L_strcpy(&GXI_Stp_PlusMoin.Title, "");
	GXI_Stp_PlusMoin.NumberOfLeaves = 0;
	GXI_Stp_PlusMoin.OpenSize = 0;
	GXI_Stp_PlusMoin.AutoSelection = 1;	
	
	Leave = 0;
	GXI_Stp_PlusMoin.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"-",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_PlusMoin.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"On",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_PlusMoin.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"+",0,0,-1,GC_STP_ID_NULL,NULL,-1};	
	GXI_Stp_PlusMoin.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	
	
	// Gamma menu

	L_strcpy(&GXI_Gamma.Title, "");
	GXI_Gamma.NumberOfLeaves = 0;
	GXI_Gamma.OpenSize = 0;
	GXI_Gamma.AutoSelection = 1;	
	
	Leave = 0;
	GXI_Gamma.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Gamma 1.0",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Gamma.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Gamma 1.7",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Gamma.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Gamma 2.2",0,0,-1,GC_STP_ID_NULL,NULL,-1};	
	GXI_Gamma.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	
	
	// 0-100 menu

	L_strcpy(&GXI_Stp_0_To_100.Title, "");
	GXI_Stp_0_To_100.NumberOfLeaves = 0;
	GXI_Stp_0_To_100.OpenSize = 0;
	GXI_Stp_0_To_100.AutoSelection = 1;	
	
	Leave = 0;
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"OFF",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"10",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"20",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"30",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"40",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"50",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"60",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"70",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"80",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"90",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"100",0,0,-1,GC_STP_ID_NULL,NULL,-1};			
	GXI_Stp_0_To_100.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	

	// timer menu

	L_strcpy(&GXI_Stp_Timer.Title, "");
	GXI_Stp_Timer.NumberOfLeaves = 0;
	GXI_Stp_Timer.OpenSize = 0;
	GXI_Stp_Timer.AutoSelection = 1;	
	
	Leave = 0;
	GXI_Stp_Timer.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Quasi paused",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Timer.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"/ 8",0,0,-1,GC_STP_ID_NULL,NULL,-1};		
	GXI_Stp_Timer.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"/ 4",0,0,-1,GC_STP_ID_NULL,NULL,-1};				
	GXI_Stp_Timer.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"/ 2",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Timer.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Normal",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Timer.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"* 2",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Timer.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"* 4",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Timer.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"* 8",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Timer.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	

	// resolution menu

	L_strcpy(&GXI_Stp_ResolutionH.Title, "");
	GXI_Stp_ResolutionH.NumberOfLeaves = 0;
	GXI_Stp_ResolutionH.OpenSize = 0;
	GXI_Stp_ResolutionH.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_ResolutionH.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"256 * 2??",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_ResolutionH.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"320 * 2??",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_ResolutionH.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"380 * 2??",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_ResolutionH.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"512 * 2??",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_ResolutionH.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"640 * 2??",0,0,-1,GC_STP_ID_NULL,NULL,-1};		
	GXI_Stp_ResolutionH.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	

	// depth blur menu

	L_strcpy(&GXI_Stp_DepthBluringSetting.Title, "");
	GXI_Stp_DepthBluringSetting.NumberOfLeaves = 0;
	GXI_Stp_DepthBluringSetting.OpenSize = 0;
	GXI_Stp_DepthBluringSetting.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_DepthBluringSetting.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Depth bluring Enable",0,0,0,GC_STP_ID_AE_DB,&GXI_Stp_OnOff,-1};
	GXI_Stp_DepthBluringSetting.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Depth bluring Near",0,0,0,GC_STP_ID_AE_DBNear,&GXI_Stp_DistanceSet,-1};
	GXI_Stp_DepthBluringSetting.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Depth bluring Far",0,0,0,GC_STP_ID_AE_DBFar,&GXI_Stp_DistanceSet,-1};
	GXI_Stp_DepthBluringSetting.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Depth bluring Near Enable",0,0,0,GC_STP_ID_AE_DBN,&GXI_Stp_OnOff,-1};
	GXI_Stp_DepthBluringSetting.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Depth bluring Near Setting",0,0,0,GC_STP_ID_AE_DBNSet,&GXI_Stp_DistanceSet,-1};
	GXI_Stp_DepthBluringSetting.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL};

	// color balance menu
	
	L_strcpy(&GXI_Stp_ColorBalance.Title, "");
	GXI_Stp_ColorBalance.NumberOfLeaves = 0;
	GXI_Stp_ColorBalance.OpenSize = 0;
	GXI_Stp_ColorBalance.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_ColorBalance.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Intensity",0,0,0,GC_STP_ID_CB_INTENSITY,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_ColorBalance.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Spectre",0,0,0,GC_STP_ID_CB_SPECTRE,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_ColorBalance.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Photo filter",0,0,0,GC_STP_ID_CB_PF,&GXI_Stp_0_To_100,-1};
	GXI_Stp_ColorBalance.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL};

	// sand storm menu
	
	L_strcpy(&GXI_Stp_SandStorm.Title, "");
	GXI_Stp_SandStorm.NumberOfLeaves = 0;
	GXI_Stp_SandStorm.OpenSize = 0;
	GXI_Stp_SandStorm.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_SandStorm.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Activation",0,0,0,GC_STP_ID_STORMACTIVATION,&GXI_Stp_OnOff,-1};
	GXI_Stp_SandStorm.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Intensity",0,0,0,GC_STP_ID_STORMINTENSITY,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_SandStorm.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Speed",0,0,0,GC_STP_ID_STORMSPEED,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_SandStorm.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Amplitude",0,0,0,GC_STP_ID_STORMAMPLITUDE,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_SandStorm.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Grain Size Front",0,0,0,GC_STP_ID_STORMGRAINSIZEF,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_SandStorm.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Grain Size Back",0,0,0,GC_STP_ID_STORMGRAINSIZEB,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_SandStorm.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};

	// color swap menu
	
	L_strcpy(&GXI_Stp_ColorSwap.Title, "");
	GXI_Stp_ColorSwap.NumberOfLeaves = 0;
	GXI_Stp_ColorSwap.OpenSize = 0;
	GXI_Stp_ColorSwap.AutoSelection = 1;	
	
	Leave = 0;
	GXI_Stp_ColorSwap.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No swap",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_ColorSwap.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Red",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_ColorSwap.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Green",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_ColorSwap.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Blue",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_ColorSwap.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	

	//GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
	//GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
	//GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
	//GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
	
	// afterFX menu

	L_strcpy(&GXI_Stp_AE.Title, "");
	GXI_Stp_AE.NumberOfLeaves = 0;
	GXI_Stp_AE.OpenSize = 0;
	GXI_Stp_AE.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Motion blur",0,0,0,GC_STP_ID_AE_MB,&GXI_Stp_0_To_100,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Bluring",0,0,0,GC_STP_ID_AE_BLUR,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Depth bluring",0,0,-1,GC_STP_ID_NULL,&GXI_Stp_DepthBluringSetting,-1};
	//GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Smooth",0,0,0,GC_STP_ID_AE_SM,&GXI_Stp_0_To_100,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Motion smooth",0,0,0,GC_STP_ID_AE_SMSM,&GXI_Stp_OnOff,-1};
	//GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Emboss",0,0,0,GC_STP_ID_AE_EMB,&GXI_Stp_0_To_100,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Remanance",0,0,0,GC_STP_ID_AE_REMANANCE,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Zoom smooth",0,0,0,GC_STP_ID_AE_ZS,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Rotate smooth",0,0,0,GC_STP_ID_AE_RS,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Brightness",0,0,0,GC_STP_ID_AE_BrightNess,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Contrast",0,0,0,GC_STP_ID_AE_Contrast,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Color balance",0,0,-1,GC_STP_ID_NULL,&GXI_Stp_ColorBalance,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"BW",0,0,0,GC_STP_ID_AE_BW,&GXI_Stp_0_To_100,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Gamma",0,0,0,GC_STP_ID_Gamma,&GXI_Gamma,-1};
	//GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Fog Correction",0,0,0,GC_STP_ID_FOG_CORRECTION,&GXI_Stp_0_To_100,-1};
	//GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Water ref",0,0,-1,GC_STP_ID_NULL,&GXI_Stp_WaterReflection,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"All off",0,0,-1,GC_STP_ID_AE_ALLOFF,NULL,-1};
	GXI_Stp_AE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};


	// specialfx menu

	L_strcpy(&GXI_Stp_SpecialFX.Title, "");
	GXI_Stp_SpecialFX.NumberOfLeaves = 0;
	GXI_Stp_SpecialFX.OpenSize = 0;
	GXI_Stp_SpecialFX.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_SpecialFX.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Sand Storm",0,0,-1,GC_STP_ID_NULL,&GXI_Stp_SandStorm,-1};
	GXI_Stp_SpecialFX.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Color Swap",0,0,0,GC_STP_ID_COLORSWAP,&GXI_Stp_ColorSwap,-1};
	GXI_Stp_SpecialFX.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	

	// 3D menu

	L_strcpy(&GXI_Stp_3D.Title, "");
	GXI_Stp_3D.NumberOfLeaves = 0;
	GXI_Stp_3D.OpenSize = 0;
	GXI_Stp_3D.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Don't MATDraw",0,0,0,GC_STP_ID_MATDONTDRAW,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Don't Compute UV",0,0,0,GC_STP_ID_GEODONTDRAW,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Don't GO Draw",0,0,0,GC_STP_ID_GODONTDRAW,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Don't Light",0,0,0,GC_STP_ID_NOLIGHT,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No texture multi-pass",0,0,0,GC_STP_ID_NO_MULTI,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No shadows",0,0,0,GC_STP_ID_NO_SDW,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No texture sprites",0,0,0,GC_STP_ID_NO_SPR,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No GFX",0,0,0,GC_STP_ID_NO_GFX,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No Z liste",0,0,0,GC_STP_ID_NO_ZLST,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No Skin",0,0,0,GC_STP_ID_NO_SKN,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No Add mats",0,0,0,GC_STP_ID_NO_ADM,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No Particules",0,0,0,GC_STP_ID_PAG_ON,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No strings",0,0,0,GC_STP_ID_STR_ON,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No modifiers",0,0,0,GC_STP_ID_MDF_ON,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No fur",0,0,0,GC_STP_ID_FUR_ON,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No SPG2",0,0,0,GC_STP_ID_SPG2_ON,&GXI_Stp_OnOff,-1};
	GXI_Stp_3D.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	


	// debug menu
	
	L_strcpy(&GXI_Stp_Debug.Title, "");
	GXI_Stp_Debug.NumberOfLeaves = 0;
	GXI_Stp_Debug.OpenSize = 0;
	GXI_Stp_Debug.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Show textures",0,0,0,GC_STP_ID_TOOLSHOWTEX,&GXI_Stp_PlusMoin,-1},
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Show vertex colors",0,0,0,GC_STP_ID_SHOWVTXCOLOR,&GXI_Stp_OnOff,-1};
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Highlight skin elements",0,0,0,GC_STP_ID_SHOWSKINELE,&GXI_Stp_OnOff,-1};
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Draw objects normals",0,0,0,GC_STP_ID_SHOWNORMALS,&GXI_Stp_OnOff,-1};
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Draw lights",0,0,0,GC_STP_ID_SHOWLIGHTS,&GXI_Stp_OnOff,-1};
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Use HW texture matrix",0,0,0,GC_STP_ID_USEHWTEXMATRIX,&GXI_Stp_OnOff,-1};
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Use HW Lights",0,0,0,GC_STP_ID_USEHWLIGHTS,&GXI_Stp_OnOff,-1};
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Use HW States",0,0,0,GC_STP_ID_USEHWSTATES,&GXI_Stp_OnOff,-1};
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Use HW Mul2X",0,0,0,GC_STP_ID_USEHWMUL2X,&GXI_Stp_OnOff,-1};	
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Use Dithering",0,0,0,GC_STP_ID_USEDITHERING,&GXI_Stp_OnOff,-1};		
	GXI_Stp_Debug.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	
	
	L_strcpy(&GXI_Stp_Cheat.Title, "");
	GXI_Stp_Cheat.NumberOfLeaves = 0;
	GXI_Stp_Cheat.OpenSize = 0;
	GXI_Stp_Cheat.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_Cheat.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"All full",0,0,0,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Cheat.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Init entrepot",0,0,0,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Cheat.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Init frizbee/gant",0,0,0,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Cheat.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Init egouts",0,0,0,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Cheat.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Init ilot",0,0,0,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Cheat.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Init satellite",0,0,0,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Cheat.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Active L2",0,0,0,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Cheat.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Off",0,0,0,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Cheat.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL};

    // sound
	L_strcpy(&GXI_Stp_Sound.Title, "");
	GXI_Stp_Sound.NumberOfLeaves = 0;
	GXI_Stp_Sound.OpenSize = 0;
	GXI_Stp_Sound.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_Sound.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Fx A",0,0,0,GC_STP_ID_NULL,&GXI_Stp_SoundAuxA,-1};
	GXI_Stp_Sound.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Profiler",0,0,0,GC_STP_ID_NULL,&GXI_Stp_SoundProfiler,-1};
	GXI_Stp_Sound.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL};
	
	// sound profiler 
	L_strcpy(&GXI_Stp_SoundProfiler.Title, "");
	GXI_Stp_SoundProfiler.NumberOfLeaves = 0;
	GXI_Stp_SoundProfiler.OpenSize = 0;
	GXI_Stp_SoundProfiler.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_SoundProfiler.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Active",0,0,0,GC_STP_ID_SOUNDPROFILE,&GXI_Stp_OnOff,-1};
	GXI_Stp_SoundProfiler.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Reset",0,0,0,GC_STP_ID_SOUNDRESETPROFILE,&GXI_Stp_OnOff,-1};
	GXI_Stp_SoundProfiler.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL};

    // sound Fx
	L_strcpy(&GXI_Stp_SoundAuxA.Title, "");
	GXI_Stp_SoundAuxA.NumberOfLeaves = 0;
	GXI_Stp_SoundAuxA.OpenSize = 0;
	GXI_Stp_SoundAuxA.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_SoundAuxA.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Enable",0,0,0,GC_STP_ID_SOUNDAUXAENABLE,&GXI_Stp_OnOff,-1};
	GXI_Stp_SoundAuxA.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Mode",0,0,0,GC_STP_ID_SOUNDAUXAMODE,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_SoundAuxA.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"WetVol",0,0,0,GC_STP_ID_SOUNDAUXAVOL,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_SoundAuxA.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"delay-Delay",0,0,0,GC_STP_ID_SOUNDAUXADELAY,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_SoundAuxA.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"delay-Feedback",0,0,0,GC_STP_ID_SOUNDAUXAFEEDBACK,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_SoundAuxA.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL};


	// raster menu
	
	L_strcpy(&GXI_Stp_Rasters_MODE.Title, "");
	GXI_Stp_Rasters_MODE.NumberOfLeaves = 0;
	GXI_Stp_Rasters_MODE.OpenSize = 0;
	GXI_Stp_Rasters_MODE.AutoSelection = 1;	
	
	Leave = 0;
	GXI_Stp_Rasters_MODE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Off",0,0,-1,GC_STP_ID_NULL,NULL,-1};	
	GXI_Stp_Rasters_MODE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Global only",0,0,-1,GC_STP_ID_NULL,NULL,-1};	
	GXI_Stp_Rasters_MODE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Engine",0,0,-1,GC_STP_ID_NULL,NULL,-1};		
	GXI_Stp_Rasters_MODE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Display",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Rasters_MODE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"ALL",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Rasters_MODE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"TOTAL only",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Rasters_MODE.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	
	
	L_strcpy(&GXI_Stp_Rasters.Title, "");
	GXI_Stp_Rasters.NumberOfLeaves = 0;
	GXI_Stp_Rasters.OpenSize = 0;
	GXI_Stp_Rasters.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_Rasters.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Raster mode",0,0,0,GC_STP_ID_RASTER,&GXI_Stp_Rasters_MODE,-1};	
	GXI_Stp_Rasters.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Sorted",0,0,0,GC_STP_ID_RASTER_SORTED,&GXI_Stp_OnOff,-1};		
	GXI_Stp_Rasters.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Number",0,0,0,GC_STP_ID_RASTER_NUMBER,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_Rasters.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Max mode",0,0,0,GC_STP_ID_RASTER_MAX,&GXI_Stp_OnOff,-1};		
	GXI_Stp_Rasters.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"disp TNUM",0,0,0,GC_STP_ID_DTNUM,&GXI_Stp_OnOff,-1};		
	GXI_Stp_Rasters.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"RAM",0,0,0,GC_STP_ID_RASTER_RAM,&GXI_Stp_Rasters_RAM,-1};		
	
	
	GXI_Stp_Rasters.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	
	
	//-------------------------------------------------------------------------------
	//
	L_strcpy(&GXI_Stp_Rasters_RAM.Title, "");
	GXI_Stp_Rasters_RAM.NumberOfLeaves = 0;
	GXI_Stp_Rasters_RAM.OpenSize = 0;
	GXI_Stp_Rasters_RAM.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_Rasters_RAM.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Off",0,0,-1,GC_STP_ID_NULL,NULL,-1};	
	GXI_Stp_Rasters_RAM.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"dynamic",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Rasters_RAM.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"static",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Rasters_RAM.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"details",0,0,-1,GC_STP_ID_NULL,NULL,-1};
	GXI_Stp_Rasters_RAM.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	
	
	//-------------------------------------------------------------------------------
	//
	L_strcpy(&GXI_Stp_Water.Title, "");
	GXI_Stp_Water.NumberOfLeaves = 0;
	GXI_Stp_Water.OpenSize = 0;
	GXI_Stp_Water.AutoSelection = 0;	
	Leave = 0;
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"WIRED",0,0,0,GC_STP_ID_WTR_WIRED,&GXI_Stp_OnOff,-1};	
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"ZFactor",0,0,0,GC_STP_ID_WTR_ZFactor,&GXI_Stp_PlusMoin,-1};	
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"ChoppyFactor",0,0,0,GC_STP_ID_WTR_ChoppyFactor,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Tile Factor",0,0,0,GC_STP_ID_WTR_TileFactor,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Displace Factor",0,0,0,GC_STP_ID_WTR_DisturbFactor,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Water Z",0,0,0,GC_STP_ID_WTR_WaterZ,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Speed factor",0,0,0,GC_STP_ID_WTR_SpeedFactor,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Depth Fog",0,0,0,GC_STP_ID_WTR_DepthFog,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Z order",0,0,0,GC_STP_ID_WTR_ZOrder,&GXI_Stp_PlusMoin,-1};
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"No Z liste",0,0,0,GC_STP_ID_NO_ZLST,&GXI_Stp_OnOff,-1};
	GXI_Stp_Water.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	
	
	
	
	//-------------------------------------------------------------------------------
	// main menu

	L_strcpy(&GXI_Stp_Root.Title, "");
	GXI_Stp_Root.NumberOfLeaves = 0;
	GXI_Stp_Root.OpenSize = 0;
	GXI_Stp_Root.AutoSelection = 0;	
	
	Leave = 0;
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Rasters",0,0,-1,GC_STP_ID_NULL,&GXI_Stp_Rasters,-1};
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Debug",0,0,-1,GC_STP_ID_NULL,&GXI_Stp_Debug,-1};
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"3D",0,0,-1,GC_STP_ID_NULL,&GXI_Stp_3D,-1};
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"AfterFX",0,0,-1,GC_STP_ID_NULL,&GXI_Stp_AE,-1};
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"SpecialFX",0,0,-1,GC_STP_ID_NULL,&GXI_Stp_SpecialFX,-1};
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"CheatCode",0,0,0,GC_STP_ID_CHEAT,&GXI_Stp_Cheat,-1};
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Sound",0,0,0,GC_STP_ID_NULL,&GXI_Stp_Sound,-1};
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Timer",0,0,0,GC_STP_ID_AE_TIMER,&GXI_Stp_Timer,-1};
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Water",0,0,-1,GC_STP_ID_NULL,&GXI_Stp_Water,-1};
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Ram",0,0,0,GC_STP_ID_RAM,&GXI_Stp_OnOff,-1};
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"Video",0,0,0,GC_STP_ID_VIDEO,&GXI_Stp_OnOff,-1};
	
/*
	GXI_Stp_Root.Leaves[Leave++]=(GXI_Stp_OneLeaf){" !\"#$%&'()*+,-./0123456789:;<=>?",0,0,0,0,NULL,-1};	
	GXI_Stp_Root.Leaves[Leave++]=(GXI_Stp_OneLeaf){"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_",0,0,0,0,NULL,-1};	
	GXI_Stp_Root.Leaves[Leave++]=(GXI_Stp_OneLeaf){"`abcdefghijklmnopqrstuvwxyz{|}~",0,0,0,0,NULL,-1};	
	GXI_Stp_Root.Leaves[Leave++]=(GXI_Stp_OneLeaf){"ÄÅÇÉÑÖÜáàâäãåçéèêëíìîïñóòôöõúùûü",0,0,0,0,NULL,-1};	
	GXI_Stp_Root.Leaves[Leave++]=(GXI_Stp_OneLeaf){"†°¢£§•¶ß®©™´¨≠ÆØ∞±≤≥¥µ∂∑∏π∫ªºΩæø",0,0,0,0,NULL,-1};	
	GXI_Stp_Root.Leaves[Leave++]=(GXI_Stp_OneLeaf){"¿¡¬√ƒ≈∆«»… ÀÃÕŒœ–—“”‘’÷◊ÿŸ⁄€‹›ﬁﬂ",0,0,0,0,NULL,-1};	
	GXI_Stp_Root.Leaves[Leave++]=(GXI_Stp_OneLeaf){"‡·‚„‰ÂÊÁËÈÍÎÏÌÓÔÒÚÛÙıˆ˜¯˘˙˚¸˝˛ˇ",0,0,0,0,NULL,-1};	
//*/
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	
	GXI_Stp_Root.Leaves[Leave++] = (GXI_Stp_OneLeaf) {"",0,0,0,0,NULL,-1};	
	
}


void GXI_RefreshResolution()
{
//	GXI_InitFB(GXI_Global_ACCESS(Xsize) , VideoMode , 32 - (GXI_Global_ACCESS(BppMode) << 3) , 32 - (GXI_Global_ACCESS(ZBppMode) << 3));
}

//extern char ucVirtualPressedKey;
extern int MEM_i_RasterGetStatus(void);
extern void MEM_RasterSetRequest(int);
extern int RasterFilter;
extern int RasterMode;
extern int RasterDetailMum;
extern int Rasters_MaxEnble;

#define Raster_FilterEngine 1
#define Raster_FilterDisplay 2
#define Raster_FilterGlobal 4

extern float ZFactor;
extern float fChoppyFactor;
extern float HCoef;
extern float DisturbFactor;
extern float GlobalWaterZ;
extern float SpeedFactor;
extern u32 bWiredLock;
extern int gcINO_gi_Special;
extern u32 MEM_bLogHeap;


void GXI_Validate(GXI_Stp_OneLeaf *p_OL , u32 Selection , u32 SetMenu)
{
	if ((p_OL->Selection == -1) && (SetMenu)) return; /* It's an action (EX SHOW VRAM) */
	switch (p_OL->ID)
	{
		case GC_STP_ID_WTR_WIRED:
			if (SetMenu)
				p_OL -> Selection = bWiredLock;
			else
			{
				bWiredLock = 0;
				if(p_OL -> Selection) bWiredLock = 1;
			}
			break;
			
		case GC_STP_ID_WTR_DepthFog:
			{
				extern float WaterFog;
				if (SetMenu)
				{
					p_OL -> Selection = 1;
					if (p_OL -> Open) sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , WaterFog);
				} 
				else
				{
					if (p_OL -> Selection == 2) WaterFog*= 1.1f;
					if (p_OL -> Selection == 0) WaterFog*= 0.9f;
					if (p_OL -> Selection != 1)
					{
						extern void GXI_InitFogMap();
						GXI_InitFogMap();
					}
					p_OL -> Selection = 1;
				}
			}
			break;
		case GC_STP_ID_WTR_ZOrder:
			{
				extern s32 ZListOrder;
				if (SetMenu)
				{
					p_OL -> Selection = 1;
					if (p_OL -> Open) sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%d" , ZListOrder);
				} 
				else
				{
					if (p_OL -> Selection == 2) ZListOrder++;
					if (p_OL -> Selection == 0) ZListOrder--;
					p_OL -> Selection = 1;
				}
			}
			break;
		case GC_STP_ID_WTR_SpeedFactor:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , SpeedFactor);
			} 
			else
			{
				if (p_OL -> Selection == 2) SpeedFactor+= 0.1f;
				if (p_OL -> Selection == 0) SpeedFactor-= 0.1f;
				p_OL -> Selection = 1;
			}
			break;
			
		case GC_STP_ID_WTR_WaterZ:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GlobalWaterZ);
			} 
			else
			{
				if (p_OL -> Selection == 2) GlobalWaterZ+= 0.1f;
				if (p_OL -> Selection == 0) GlobalWaterZ-= 0.1f;
				p_OL -> Selection = 1;
			}
			break;
		case GC_STP_ID_WTR_ZFactor:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , ZFactor);
			} 
			else
			{
				if (p_OL -> Selection == 2) ZFactor*= 1.1f;
				if (p_OL -> Selection == 0) ZFactor*= 0.9f;
				p_OL -> Selection = 1;
			}
			break;
			
		case GC_STP_ID_WTR_ChoppyFactor:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , fChoppyFactor);
			} 
			else
			{
				if (p_OL -> Selection == 2) fChoppyFactor*= 1.1f;
				if (p_OL -> Selection == 0) fChoppyFactor*= 0.9f;
				p_OL -> Selection = 1;
			}
			break;
		case GC_STP_ID_WTR_TileFactor:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , HCoef * 100.0f);
			} 
			else
			{
				if (p_OL -> Selection == 2) HCoef*= 1.1f;
				if (p_OL -> Selection == 0) HCoef*= 0.9f;
				p_OL -> Selection = 1;
			}
			break;
		case GC_STP_ID_WTR_DisturbFactor:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , DisturbFactor);
			} 
			else
			{
				if (p_OL -> Selection == 2) DisturbFactor*= 1.1f;
				if (p_OL -> Selection == 0) DisturbFactor*= 1.0F / 1.1f;
				p_OL -> Selection = 1;
			}
			break;
			
		case GC_STP_ID_DTNUM:
			if (SetMenu)
			{
				p_OL -> Selection = GXI_Global_ACCESS(ulDisplayTnum);
			} 
			else
			{
				GXI_Global_ACCESS(ulDisplayTnum) = 0;
				if(p_OL -> Selection) GXI_Global_ACCESS(ulDisplayTnum) = 1;
			}
		 break;
		case GC_STP_ID_RASTER:
			if (SetMenu)
			{
				if(GXI_Global_ACCESS(Status) & GC_Status_Raster_On)
				{
					if(GXI_Global_ACCESS(Status) & GC_Status_Raster_Details_On)
					{
						if (RasterFilter == Raster_FilterGlobal) p_OL -> Selection = 1;
						else
						if (RasterFilter == Raster_FilterEngine) p_OL -> Selection = 2;
						else
						if (RasterFilter == Raster_FilterDisplay) p_OL -> Selection = 3;
						else
						if (RasterFilter == Raster_FilterEngine|Raster_FilterDisplay) p_OL -> Selection = 4;
						
					} else p_OL -> Selection = 5;
				}
				else
					p_OL -> Selection = 0;
			} 
			else
			{
				GXI_Global_ACCESS(Status) &= ~(GC_Status_Raster_On|GC_Status_Raster_Details_On);
				switch (p_OL -> Selection)
				{
					case 0:GXI_Global_ACCESS(Status) &= ~(GC_Status_Raster_On|GC_Status_Raster_Details_On);break;
					case 1:GXI_Global_ACCESS(Status) |= GC_Status_Raster_On|GC_Status_Raster_Details_On;RasterFilter = Raster_FilterGlobal;break;
					case 2:GXI_Global_ACCESS(Status) |= GC_Status_Raster_On|GC_Status_Raster_Details_On;RasterFilter = Raster_FilterEngine;break;
					case 3:GXI_Global_ACCESS(Status) |= GC_Status_Raster_On|GC_Status_Raster_Details_On;RasterFilter = Raster_FilterDisplay;break;
					case 4:GXI_Global_ACCESS(Status) |= GC_Status_Raster_On|GC_Status_Raster_Details_On;RasterFilter = Raster_FilterEngine|Raster_FilterDisplay;break;
					case 5:GXI_Global_ACCESS(Status) |= GC_Status_Raster_On;break;
				}
			}
			break;
		case GC_STP_ID_RASTER_SORTED:
			if (SetMenu)
			{
				if(RasterMode & (1 << 16))
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} 
			else
			{
				RasterMode &= ~(1 << 16);
				if(p_OL -> Selection) RasterMode |= (1 << 16);
			}
			break;
		case GC_STP_ID_RASTER_MAX:
			if (SetMenu)
			{
				p_OL -> Selection = Rasters_MaxEnble;
			} 
			else
			{
				Rasters_MaxEnble = 0;
				if(p_OL -> Selection) Rasters_MaxEnble = 1;
			}
			break;
		case GC_STP_ID_RASTER_NUMBER:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%1d" , RasterDetailMum);
			} 
			else
			{
				if (p_OL -> Selection == 2) RasterDetailMum++;
				if (p_OL -> Selection == 0) RasterDetailMum--;
				p_OL -> Selection = 1;
			}
			break;
			
		case GC_STP_ID_RASTER_FILTER:
			/*if (SetMenu)
			{
				p_OL -> Selection = RasterFilter;
			} else
			{
				RasterFilter = p_OL -> Selection;
			}*/
			break;		
        
		case GC_STP_ID_RASTER_RAM:
			if (SetMenu)
			{
			    p_OL->Selection = MEM_i_RasterGetStatus();
			} 
			else
			{
			    MEM_RasterSetRequest(p_OL->Selection) ;
			}
			break;

		case GC_STP_ID_DONTDRAW:
			/*if (SetMenu)
			{
				if (GXI_Global_ACCESS(Status) & GC_Status_VU1DontDraw)
					p_OL -> Selection = 1; // On 
				else
					p_OL -> Selection = 0;
			} else {
				GXI_Global_ACCESS(Status) &= ~GC_Status_VU1DontDraw;
				if (Selection) GXI_Global_ACCESS(Status) |= GC_Status_VU1DontDraw;
			}*/
			break;
		case GC_STP_ID_SETRENDERSTATE:
			/*if (SetMenu)
			{
				if (GXI_Global_ACCESS(Status) & GC_Status_SetRenderStateOn)
					p_OL -> Selection = 1; // On 
				else
					p_OL -> Selection = 0;
			} else {
				GXI_Global_ACCESS(Status) &= ~GC_Status_SetRenderStateOn;
				if (Selection) GXI_Global_ACCESS(Status) |= GC_Status_SetRenderStateOn;
			}*/
			break;
		case GC_STP_ID_AE_MB:
			if (SetMenu)
			{
				if (GXI_Global_ACCESS(Status) & GC_Status_AE_MB)
				{
					p_OL -> Selection = GC_SET_FloatToSelection(GXI_Global_ACCESS(p_AE_Params)->MotionBlurFactor);
				}
				else
					p_OL -> Selection = 0;
			} else {
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_MB;
				if (Selection) 
				{
					GXI_Global_ACCESS(Status) |= GC_Status_AE_MB;
					GXI_Global_ACCESS(p_AE_Params)->MotionBlurFactor = GC_SET_SelectionToFloat(p_OL -> Selection);
				}
			}
			break;

		case GC_STP_ID_AE_BLUR:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (GXI_Global_ACCESS(Status) & GC_Status_AE_BLUR)
						sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_Global_ACCESS(p_AE_Params)->BlurFactor);
					else
					{
						sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "off");
						GXI_Global_ACCESS(p_AE_Params)->BlurFactor = 0.0f;
					}
				}
			} else
			{
				if (Selection == 2) GXI_Global_ACCESS(p_AE_Params)->BlurFactor+= 1.f;
				if (Selection == 0) GXI_Global_ACCESS(p_AE_Params)->BlurFactor-= 1.f;
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_BLUR;
				if (GXI_Global_ACCESS(p_AE_Params)->BlurFactor != 0.0f) 
					GXI_Global_ACCESS(Status) |= GC_Status_AE_BLUR;
				p_OL -> Selection = 1;
			}
			break;

		case GC_STP_ID_AE_BrightNess:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_Global_ACCESS(p_AE_Params)->Brighness);
					
				}
			} else
			{
				if (Selection == 2) GXI_Global_ACCESS(p_AE_Params)->Brighness+= 0.01f;
				if (Selection == 0) GXI_Global_ACCESS(p_AE_Params)->Brighness-= 0.01f;
				GXI_Global_ACCESS(p_AE_Params)->Brighness = fMax(0.0 , fMin(1.0f , GXI_Global_ACCESS(p_AE_Params)->Brighness));
				
				p_OL -> Selection = 1;
			}
			break;
		case GC_STP_ID_AE_Contrast:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_Global_ACCESS(p_AE_Params)->Contrast);
					
				}
			} else
			{
				if (Selection == 2) GXI_Global_ACCESS(p_AE_Params)->Contrast+= 0.01f;
				if (Selection == 0) GXI_Global_ACCESS(p_AE_Params)->Contrast-= 0.01f;
				GXI_Global_ACCESS(p_AE_Params)->Contrast = fMax(0.0 , fMin(1.0f , GXI_Global_ACCESS(p_AE_Params)->Contrast));
				p_OL -> Selection = 1;
			}
			break;
		case GC_STP_ID_CB_INTENSITY:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (GXI_Global_ACCESS(Status) & GC_Status_AE_CB_On)
						sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_Global_ACCESS(p_AE_Params)->CB_Intensity);
					else
					{
						sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "off");
					}
				}
			} else
			{
				if (Selection == 2) GXI_Global_ACCESS(p_AE_Params)->CB_Intensity+= 0.01f;
				if (Selection == 0) GXI_Global_ACCESS(p_AE_Params)->CB_Intensity-= 0.01f;
				GXI_Global_ACCESS(p_AE_Params)->CB_Intensity = fMax(0.0 , fMin(1.0f , GXI_Global_ACCESS(p_AE_Params)->CB_Intensity));
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_CB_On;
				if (GXI_Global_ACCESS(p_AE_Params)->CB_Intensity != 0.0f) GXI_Global_ACCESS(Status) |= GC_Status_AE_CB_On;
				p_OL -> Selection = 1;
			}
			break;
		case GC_STP_ID_CB_SPECTRE:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_Global_ACCESS(p_AE_Params)->CB_Spectre);
				}
			} else
			{
				if (Selection == 2) GXI_Global_ACCESS(p_AE_Params)->CB_Spectre+= 0.01f;
				if (Selection == 0) GXI_Global_ACCESS(p_AE_Params)->CB_Spectre-= 0.01f;
				if (GXI_Global_ACCESS(p_AE_Params)->CB_Spectre >= 1.0f) GXI_Global_ACCESS(p_AE_Params)->CB_Spectre -= 1.0f;
				if (GXI_Global_ACCESS(p_AE_Params)->CB_Spectre < 0.0f) GXI_Global_ACCESS(p_AE_Params)->CB_Spectre += 1.0f;
				p_OL -> Selection = 1;
			}
			break;
		case GC_STP_ID_CB_PF:
			if (SetMenu)
			{
				p_OL -> Selection = GC_SET_FloatToSelection(GXI_Global_ACCESS(p_AE_Params)->CB_PhotoFilterEffect);
			} else {
				GXI_Global_ACCESS(p_AE_Params)->CB_PhotoFilterEffect = GC_SET_SelectionToFloat(p_OL -> Selection);
			}
			break;
		case GC_STP_ID_AE_DB:
			if (SetMenu)
			{
				if (GXI_Global_ACCESS(Status) & GC_Status_AE_DB)
					p_OL -> Selection = 1; // On 
				else
					p_OL -> Selection = 0;
			} else {
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_DB;
				if (Selection) 
				{	
					GXI_Global_ACCESS(Status) |= GC_Status_AE_DB;
				}
			}
			break;
		case GC_STP_ID_AE_DBNear:
			if (SetMenu)
			{
				p_OL -> Selection = 5;
			} else {
				GC_SET_DeltaDistanceGet(Selection , &GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZStart);
				if (GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZStart > GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZEnd)
				     GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZEnd = GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZStart;
			}
			break;
		case GC_STP_ID_AE_DBFar:
			if (SetMenu)
			{
				p_OL -> Selection = 5;
			} else {
				GC_SET_DeltaDistanceGet(Selection , &GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZEnd);
				if (GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZStart > GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZEnd)
				     GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZStart = GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_ZEnd;
			}
			break;
		case GC_STP_ID_AE_DBNSet:
			if (SetMenu)
			{
				p_OL -> Selection = 5;
			} else {
				GC_SET_DeltaDistanceGet(Selection , &GXI_Global_ACCESS(p_AE_Params)->Depth_Blur_Near);
			}
			break;
		case GC_STP_ID_AE_DBN:
			if (SetMenu)
			{
				if (GXI_Global_ACCESS(Status) & GC_Status_AE_DBN)
					p_OL -> Selection = 1; //On 
				else
					p_OL -> Selection = 0;
			} else {
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_DBN;
				if (Selection) 
				{	
					GXI_Global_ACCESS(Status) |= GC_Status_AE_DBN;
				}
			}
			break;
		case GC_STP_ID_AE_SM:
		/*	if (SetMenu)
			{
				if (GXI_Global_ACCESS(Status) & GC_Status_AE_SM)
					p_OL -> Selection = GC_SET_FloatToSelection(GXI_Global_ACCESS(p_AE_Params)->BlurFactor); // On 
				else
					p_OL -> Selection = 0;
			} else {
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_SM;
				if (Selection) 
				{
					GXI_Global_ACCESS(Status) |= GC_Status_AE_SM;
					GXI_Global_ACCESS(p_AE_Params)->BlurFactor = GC_SET_SelectionToFloat(p_OL -> Selection);
				}
			}*/
			break;

		case GC_STP_ID_RAM:
		{
			if (SetMenu)
			{
				if (MEM_bLogHeap)
					p_OL -> Selection = 1; // On 
				else
					p_OL -> Selection = 0;
			} else {
				MEM_bLogHeap = Selection;
			}
		}
			break;
			
		case GC_STP_ID_VIDEO:
		{
			static BOOL bPlayVideo = FALSE;
			static BOOL bPrevPlayVideo = FALSE;
			
			if (SetMenu)
			{
				if (bPlayVideo)
					p_OL -> Selection = 1; // On 
				else
					p_OL -> Selection = 0;
			} else {
				if (bPlayVideo && !bPrevPlayVideo)
				{
					extern void GC_VideoFullScreenCreate(ULONG ulBK);
					GC_VideoFullScreenCreate(0);
				}

				bPrevPlayVideo = bPlayVideo;
				bPlayVideo = Selection;
			}
		}
			break;
			
			
		case GC_STP_ID_AE_SMSM:
			if (SetMenu)
			{
				if (GXI_Global_ACCESS(Status) & GC_Status_AE_SMSM)
					p_OL -> Selection = 1; // On 
				else
					p_OL -> Selection = 0;
			} else {
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_SMSM;
				if (Selection) GXI_Global_ACCESS(Status) |= GC_Status_AE_SMSM;
			}
			break;

		case GC_STP_ID_AE_EMB:
			/*if (SetMenu)
			{
				if (GXI_Global_ACCESS(Status) & GC_Status_AE_EMB)
					p_OL -> Selection = GC_SET_FloatToSelection(GXI_Global_ACCESS(p_AE_Params)->EmbossFactor); // On 
				else
					p_OL -> Selection = 0;
			} else {
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_EMB;
				if (Selection) 
				{
					GXI_Global_ACCESS(Status) |= GC_Status_AE_EMB;
					GXI_Global_ACCESS(p_AE_Params)->EmbossFactor = GC_SET_SelectionToFloat(p_OL -> Selection);
				}
			}*/
			break;

		case GC_STP_ID_AE_BW:
			{
				if (SetMenu)
				{
					if (GXI_Global_ACCESS(Status) & GC_Status_AE_BW)
					{
						p_OL -> Selection = GC_SET_FloatToSelection(GXI_Global_ACCESS(p_AE_Params)->BWFactor);
						
					}
					else
						p_OL -> Selection = 0;
				} else {
					GXI_Global_ACCESS(Status) &= ~GC_Status_AE_BW;
					if (Selection) 
					{
						GXI_Global_ACCESS(Status) |= GC_Status_AE_BW;
						GXI_Global_ACCESS(p_AE_Params)->BWFactor = GC_SET_SelectionToFloat(p_OL -> Selection);
					}	
				}
			}
			break;
			
		case GC_STP_ID_Gamma:
			{
				static GXGamma CurrentGamma = GX_GM_1_0;
				if (SetMenu)
				{
					p_OL -> Selection = CurrentGamma; 
				} 
				else 
				{
					CurrentGamma = p_OL -> Selection;
					GXSetDispCopyGamma(CurrentGamma);
				}
			}
			break;
			
			
		case GC_STP_ID_AE_RS:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (GXI_Global_ACCESS(Status) & GC_Status_AE_RS)
						sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_Global_ACCESS(p_AE_Params)->SpinSmoothFactor);
					else
					{
						sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "off");
						GXI_Global_ACCESS(p_AE_Params)->SpinSmoothFactor = 0.0f;
					}
				}
			} else
			{
				if (Selection == 2) GXI_Global_ACCESS(p_AE_Params)->SpinSmoothFactor+= 0.01f;
				if (Selection == 0) GXI_Global_ACCESS(p_AE_Params)->SpinSmoothFactor-= 0.01f;
				GXI_Global_ACCESS(p_AE_Params)->SpinSmoothFactor = fMax(0.0 , fMin(1.0f , GXI_Global_ACCESS(p_AE_Params)->SpinSmoothFactor));
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_RS;
				if (GXI_Global_ACCESS(p_AE_Params)->SpinSmoothFactor != 0.0f) 
					GXI_Global_ACCESS(Status) |= GC_Status_AE_RS;
				p_OL -> Selection = 1;
			}
			break;
		case GC_STP_ID_AE_ZS:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (GXI_Global_ACCESS(Status) & GC_Status_AE_ZS)
						sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor);
					else
					{
						sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "off");
						GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor = 0.0f;
					}
				}
			} else
			{
				if (Selection == 2) GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor+= 0.01f;
				if (Selection == 0) GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor-= 0.01f;
				GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor = fMax(0.0 , fMin(1.0f , GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor));
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_ZS;
				if (GXI_Global_ACCESS(p_AE_Params)->ZoomSmoothFactor != 0.0f) 
					GXI_Global_ACCESS(Status) |= GC_Status_AE_ZS;
				p_OL -> Selection = 1;
			}
			break;
		case GC_STP_ID_AE_REMANANCE:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					if (GXI_Global_ACCESS(Status) & GC_Status_AE_REMANANCE)
						sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor);
					else
					{
						sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "off");
						GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor = 0.0f;
					}
				}
			} else
			{
				if (Selection == 2) GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor+= 0.01f;
				if (Selection == 0) GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor-= 0.01f;
				GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor = fMax(-1.0 , fMin(1.0f , GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor));
				GXI_Global_ACCESS(Status) &= ~GC_Status_AE_REMANANCE;
				if (GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor * GXI_Global_ACCESS(p_AE_Params)->RemanaceFactor > 0.00008f) 
					GXI_Global_ACCESS(Status) |= GC_Status_AE_REMANANCE;
				p_OL -> Selection = 1;
			}
			break;
			
			
		case GC_STP_ID_FOG_CORRECTION:
			/*if (SetMenu)
			{
				p_OL -> Selection = GC_SET_FloatToSelection2(GXI_Global_ACCESS(fFogCorrector));
			} else {
				GXI_Global_ACCESS(fFogCorrector) = GC_SET_SelectionToFloat2(p_OL -> Selection);
			}*/
			break;
			
		case GC_STP_ID_AE_TIMER:
			if (SetMenu)
			{
				if (fGlobalAcc == 0.005f) p_OL -> Selection = 0;
				if (fGlobalAcc == 0.125f) p_OL -> Selection = 1;
				if (fGlobalAcc == 0.25f) p_OL -> Selection = 2;
				if (fGlobalAcc == 0.5f) p_OL -> Selection = 3;
				if (fGlobalAcc == 1.0f) p_OL -> Selection = 4;
				if (fGlobalAcc == 2.0f) p_OL -> Selection = 5;
				if (fGlobalAcc == 4.0f) p_OL -> Selection = 6;
				if (fGlobalAcc == 8.0f) p_OL -> Selection = 7;				
			} else {
				if (p_OL -> Selection == 0) fGlobalAcc = 0.005f;
				if (p_OL -> Selection == 1) fGlobalAcc = 0.125f;
				if (p_OL -> Selection == 2) fGlobalAcc = 0.25f;
				if (p_OL -> Selection == 3) fGlobalAcc = 0.5f;
				if (p_OL -> Selection == 4) fGlobalAcc = 1.0f;
				if (p_OL -> Selection == 5) fGlobalAcc = 2.0f;
				if (p_OL -> Selection == 6) fGlobalAcc = 4.0f;
				if (p_OL -> Selection == 7) fGlobalAcc = 8.0f;				
			}
			break;

		case GC_STP_ID_MATDONTDRAW:
			if (SetMenu)
			{
				p_OL -> Selection = NoMATDRAW; // On
			} else {
				NoMATDRAW = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_NO_MULTI:
			if (SetMenu)
			{
				p_OL -> Selection = NoMulti; // On
			} else {
				NoMulti = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_NO_SDW:
			if (SetMenu)
			{
				p_OL -> Selection = NoSDW; // On
			} else {
				NoSDW = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_NO_SPR:
			if (SetMenu)
			{
				p_OL -> Selection = NoSPR; // On
			} else {
				NoSPR = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_NO_GFX:
			if (SetMenu)
			{
				p_OL -> Selection = NoGFX; // On
			} else {
				NoGFX = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_NO_ZLST:
			if (SetMenu)
			{
				p_OL -> Selection = NoZLST; // On
			} else {
				NoZLST = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_NO_SKN:
			if (SetMenu)
			{
				p_OL -> Selection = NoSKN; // On
			} else {
				NoSKN = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_NO_ADM:
			if (SetMenu)
			{
				p_OL -> Selection = NoADM; // On
			} else {
				NoADM = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_PAG_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoPAG; // On
			} else {
				NoPAG = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_STR_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoSTR; // On
			} else {
				NoSTR = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_MDF_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoMDF; // On
			} else {
				NoMDF = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_FUR_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoFUR; // On
			} else {
				NoFUR = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_SPG2_ON:
			if (SetMenu)
			{
				p_OL -> Selection = NoSPG2; // On
			} else {
				NoSPG2 = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_GEODONTDRAW:
			if (SetMenu)
			{
				p_OL -> Selection = NoGEODRAW; // On
			} else {
				NoGEODRAW = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_GODONTDRAW:
			if (SetMenu)
			{
				p_OL -> Selection = NoGODRAW; // On
			} else {
				NoGODRAW = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_NOLIGHT:
			if (SetMenu)
			{
				p_OL -> Selection = NoLIGH; // On
			} else {
				NoLIGH = p_OL -> Selection; // On
			}
			break;
		case GC_STP_ID_AE_ALLOFF:
			{
				GXI_Global_ACCESS(Status) &= ~(
						GC_Status_AE_DB 			|
						GC_Status_AE_MB 			|	
						GC_Status_AE_SM 		|
						GC_Status_AE_EMB 		|	
						GC_Status_AE_SMSM		|	
						GC_Status_AE_SHOWSDW	|	
						GC_Status_AE_SHOWZBUF	|	
						GC_Status_AE_ZS		|	
						GC_Status_AE_RS		|	
						GC_Status_AE_REMANANCE	|	
						GC_Status_AE_DBN		|	
						GC_Status_AE_BlurTest44	|	
						GC_Status_AE_BW		|	
						GC_Status_AE_ModeCompare);
						
				GXI_Global_ACCESS(p_AE_Params)->Brighness = 0.5f;
				GXI_Global_ACCESS(p_AE_Params)->Contrast = 0.5f;
				GXI_Global_ACCESS(p_AE_Params)->CB_Spectre = 0.0f;
				GXI_Global_ACCESS(p_AE_Params)->CB_Intensity = 0.0f;
			} 
			break;
			
		case GC_STP_ID_SHOWVTXCOLOR:
			if (SetMenu)
			{
				if (g_bShowVertexColors)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				g_bShowVertexColors = Selection;
			}
		break;
		
		case GC_STP_ID_SHOWSKINELE:
			if (SetMenu)
			{
				if (g_bShowSkinElements)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				g_bShowSkinElements = Selection;
			}
		break;
		
		case GC_STP_ID_SHOWNORMALS:
			if (SetMenu)
			{
				if (g_bShowNormals)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				g_bShowNormals = Selection;
			}
		break;
		
		case GC_STP_ID_SHOWLIGHTS:
			if (SetMenu)
			{
				if (g_bShowLights)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				g_bShowLights = Selection;
			}
		break;

		case GC_STP_ID_USEHWTEXMATRIX:
			if (SetMenu)
			{
				if (g_bUseHardwareTextureMatrices)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				g_bUseHardwareTextureMatrices = Selection;
			}
		break;

		case GC_STP_ID_USEHWLIGHTS:
			if (SetMenu)
			{
				if (g_bUseHardwareLights)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				g_bUseHardwareLights = Selection;
			}
		break;

        case GC_STP_ID_USEHWSTATES:
			if (SetMenu)
			{
				if (g_bUseHWStates)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				g_bUseHWStates = Selection;
			}
		break;

		case GC_STP_ID_USEHWMUL2X:
			if (SetMenu)
			{
				if (g_bUseHWMul2x)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				g_bUseHWMul2x = Selection;
			}
		break;
		
		case GC_STP_ID_USEDITHERING:
			if (SetMenu)
			{
				if (g_bUseDithering)
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				g_bUseDithering = Selection;
				GXSetDither(g_bUseDithering);
			}
		break;

		case GC_STP_ID_TOOLSHOWTEX:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					GXI_Global_ACCESS(Status) |= GC_Status_AE_ShowTexture;
					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%d" , TextnumToShow);
				}
				else GXI_Global_ACCESS(Status) &= ~GC_Status_AE_ShowTexture;
			} else
			{
				if (Selection == 2) TextnumToShow++;
				if (Selection == 0) TextnumToShow--;
				p_OL -> Selection = 1;
			}
			break;
			
		case GC_STP_ID_STORMINTENSITY:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_SFX_SandStormGetIntensity());
				}
			} else
			{
				FLOAT StormIntensity = GXI_SFX_SandStormGetIntensity();
				if (Selection == 2) StormIntensity+=0.1f;
				if (Selection == 0) StormIntensity-=0.1f;
				StormIntensity = fMax(0.0f , fMin(1.0f , StormIntensity));
				GXI_SFX_SandStormSetIntensity(StormIntensity, StormIntensity, 0.0f);
				p_OL -> Selection = 1;
			}
			break;
			
		case GC_STP_ID_STORMSPEED:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_SFX_SandStormGetSpeed());
				}
			} else
			{
				FLOAT StormSpeed = GXI_SFX_SandStormGetSpeed();
				if (Selection == 2) StormSpeed+=0.1f;
				if (Selection == 0) StormSpeed-=0.1f;
				StormSpeed = fMax(0.0f , fMin(10.0f , StormSpeed));
				GXI_SFX_SandStormSetSpeed(StormSpeed);
				p_OL -> Selection = 1;
			}
			break;
			
		case GC_STP_ID_STORMAMPLITUDE:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_SFX_SandStormGetAmplitude());
				}
			} else
			{
				FLOAT StormAmplitude = GXI_SFX_SandStormGetAmplitude();
				if (Selection == 2) StormAmplitude+=0.1f;
				if (Selection == 0) StormAmplitude-=0.1f;
				StormAmplitude = fMax(0.0f , fMin(10.0f , StormAmplitude));
				GXI_SFX_SandStormSetAmplitude(StormAmplitude);
				p_OL -> Selection = 1;
			}
			break;
			
		case GC_STP_ID_STORMGRAINSIZEF:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_SFX_SandStormGetGrainSizeFront());
				}
			} else
			{
				FLOAT StormGrainSizeFront = GXI_SFX_SandStormGetGrainSizeFront();
				if (Selection == 2) StormGrainSizeFront+=0.1f;
				if (Selection == 0) StormGrainSizeFront-=0.1f;
				StormGrainSizeFront = fMax(0.0f , fMin(10.0f , StormGrainSizeFront));
				GXI_SFX_SandStormSetGrainSizeFront(StormGrainSizeFront);
				p_OL -> Selection = 1;
			}
			break;
			
		case GC_STP_ID_STORMGRAINSIZEB:
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%.2f" , GXI_SFX_SandStormGetGrainSizeBack());
				}
			} else
			{
				FLOAT StormGrainSizeBack = GXI_SFX_SandStormGetGrainSizeBack();
				if (Selection == 2) StormGrainSizeBack+=0.1f;
				if (Selection == 0) StormGrainSizeBack-=0.1f;
				StormGrainSizeBack = fMax(0.0f , fMin(10.0f , StormGrainSizeBack));
				GXI_SFX_SandStormSetGrainSizeBack(StormGrainSizeBack);
				p_OL -> Selection = 1;
			}
			break;	
			
		case GC_STP_ID_STORMACTIVATION:
			if (SetMenu)
			{
				if (GXI_SFX_SandStormGetActivation())
					p_OL -> Selection = 1;
				else
					p_OL -> Selection = 0;
			} else
			{
				GXI_SFX_SandStormSetActivation(Selection);
			}
		break;			
			
		case GC_STP_ID_COLORSWAP:
			if (SetMenu)
			{
				p_OL -> Selection = g_iCurrentColorSwap;
			} else
			{
				g_iCurrentColorSwap = (GXTevSwapSel)p_OL -> Selection;
				//GXSetTevSwapMode(GX_TEVSTAGE0, g_iCurrentColorSwap, g_iCurrentColorSwap);
			}
		break;	
        case GC_STP_ID_CHEAT:
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5

			if (SetMenu)
			{
			    if(gcINO_gi_Special & 1)
			        p_OL -> Selection = 6;
			    else
			        p_OL -> Selection = 0;
			}
			else
			{
				switch (Selection) 
				{
					case 0:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x60 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // All objects KEY = 0x60 = NUMPAD0
					case 1:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x61 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // init entrepot KEY = 0x61 = NUMPAD1
					case 2:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x62 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // frizbee/gant = NUMPAD2
					case 3:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x63 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // init egout KEY = 0x63 = NUMPAD3
					case 4:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x65 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // init ilot KEY = 0x65 = NUMPAD5
					case 5:iVirtualPressedKeyTime=15;ucVirtualPressedKey[0] = 0x68 ; ucVirtualPressedKey[1] = VK_LMENU ; break; // satellite KEY = NUMPAD8
					case 6:gcINO_gi_Special |= 1;
					default:iVirtualPressedKeyTime=0; gcINO_gi_Special=0;break;
				}
			}
			break;
        
        // *** sound ***
        case GC_STP_ID_SOUNDPROFILE:
			if (SetMenu)
			{
			    if(gcSND_b_ProfilerIsEnable())
    				p_OL -> Selection = 1;
    			else
    			    p_OL -> Selection = 0;
			} 
			else
			{
				if (Selection == 0) gcSND_ProfilerDisable();
				if (Selection == 1) gcSND_ProfilerEnable();				
			}
			break;
        case GC_STP_ID_SOUNDRESETPROFILE:
			if (SetMenu)
			{
   			    p_OL -> Selection = 0;
			} 
			else
			{
				if (Selection == 1) gcSND_ProfilerReset();				
			}
			break;
			
        case GC_STP_ID_SOUNDAUXAENABLE       :
			if (SetMenu)
			{
			    if(gcSND_i_FxGetMode(NULL) == SND_Cte_FxMode_Off)
    				p_OL -> Selection = 0;
    			else
    			    p_OL -> Selection = 1;
			} 
			else
			{
				if (Selection == 0) gcSND_i_FxDisable(NULL);
				if (Selection == 1) gcSND_i_FxEnable(NULL);				
			}
			break;
        case 	GC_STP_ID_SOUNDAUXAMODE      :
			if (SetMenu)
			{
				p_OL -> Selection = 1;
				if (p_OL -> Open) 
				{
				    gcSND_GetCurrentFxModeName(GXI_Stp_PlusMoin.Leaves[1].Name, 0);
				}
			} else
			{
				if (Selection == 2) gcSND_UpdateCurrentFxMode(0, 1);
				if (Selection == 0) gcSND_UpdateCurrentFxMode(0, -1);				
				
				p_OL -> Selection = 1;
			}
			break;
			
        case 	GC_STP_ID_SOUNDAUXAVOL       :
            {
                static float gcSND_f_FxWetVol=0.0f;
    			if (SetMenu)
    			{
    				p_OL -> Selection = 1;
    				if (p_OL -> Open) 
    					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%2.3f" ,gcSND_f_FxWetVol);
    			} else
    			{
    			    if((Selection == 0)||(Selection == 2))
    			    {
        				if (Selection == 2) gcSND_f_FxWetVol += 0.01f;
        				if (Selection == 0) gcSND_f_FxWetVol -= 0.01f;
        				if(gcSND_f_FxWetVol > 1.0f) gcSND_f_FxWetVol = 1.0f;
        				if(gcSND_f_FxWetVol < 0.0f) gcSND_f_FxWetVol = 0.0f;
        				
        				gcSND_i_FxSetWetVolume(NULL, (int)SND_l_GetAttFromVol(gcSND_f_FxWetVol));
        				gcSND_i_FxEnable(NULL);
        				
        			}
    				p_OL -> Selection = 1;
    			}
    		}
			break;
			
        case 	GC_STP_ID_SOUNDAUXADELAY     :
			{
			    static int gcSND_i_FxDelay=0;
    			if (SetMenu)
    			{
    				p_OL -> Selection = 1;
    				if (p_OL -> Open) 
    					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%d" , gcSND_i_FxGetDelay(NULL));
    			} else
    			{
    				if((Selection == 0)||(Selection == 2))
    				{
        				if (Selection == 2) gcSND_i_FxDelay += 10;
        				if (Selection == 0) gcSND_i_FxDelay -= 10;
        				if(gcSND_i_FxDelay > 5000) gcSND_i_FxDelay = 5000;
        				if(gcSND_i_FxDelay < 0) gcSND_i_FxDelay = 0;
        				
        				gcSND_i_FxSetDelay(NULL, gcSND_i_FxDelay);  				    				
        				gcSND_i_FxDelay = gcSND_i_FxGetDelay(NULL);
        				gcSND_i_FxEnable(NULL);
        			}
    				p_OL -> Selection = 1;
    			}
			}
			break;
        
        case 	GC_STP_ID_SOUNDAUXAFEEDBACK  :
            {
                static int gcSND_i_FxFeedback=0;
                
    			if (SetMenu)
    			{
    				p_OL -> Selection = 1;
    				if (p_OL -> Open) 
    					sprintf(GXI_Stp_PlusMoin.Leaves[1].Name , "%d" , gcSND_i_FxGetFeedback(NULL));
    			} else
    			{
    			    if((Selection == 0)||(Selection == 2))
    			    {
        				if (Selection == 2) gcSND_i_FxFeedback += 2;
        				if (Selection == 0) gcSND_i_FxFeedback -= 2;
        				if(gcSND_i_FxFeedback > 100) gcSND_i_FxFeedback = 100;
        				if(gcSND_i_FxFeedback < 0) gcSND_i_FxFeedback = 0;
        				gcSND_i_FxSetFeedback(NULL, gcSND_i_FxFeedback);
        				gcSND_i_FxEnable(NULL);
        				gcSND_i_FxFeedback = gcSND_i_FxGetFeedback(NULL);
        			}
    				p_OL -> Selection = 1;
    			}
            }
            break;
        
        
        // *** sound ***
    
		case GC_STP_ID_NULL: break;
	}
	if (p_OL->DefaultSelection == -1) p_OL->DefaultSelection = p_OL->Selection;
}

void AE_Report()
{
}

void GXI_InitMenu(GXI_SetupMenu *p_Root)
{
	u32 LC;
	for (LC = 0 ; LC < p_Root -> NumberOfLeaves ; LC ++)
	{
		GXI_Validate(&p_Root -> Leaves[LC] , 0 , 1);
		if (p_Root -> Leaves[LC].p_SubMenu)
		{
		if (p_Root->Leaves[LC].Open)
			GXI_InitMenu(p_Root -> Leaves[LC].p_SubMenu);
			}
	}
}
void GXI_Stp_MenuClose(GXI_SetupMenu *Root)
{
	u32 LC;
	for (LC = 0 ; LC < Root -> NumberOfLeaves ; LC++)
	{
		Root->Leaves[LC].Open = 0;
		Root->Leaves[LC].Hlight = 0;
		if (Root->Leaves[LC].p_SubMenu)
			GXI_Stp_MenuClose(Root->Leaves[LC].p_SubMenu);
	}
}

u32 GXI_Stp_GetLenght(char *Str)
{	
	u32 Ret;
	Ret = 0 ;
	while (*(Str++)) Ret++;
	return Ret; 
}


float SETUP_BigX = 0.0f;
float SETUP_BigY = 10.0f;
float Depest_X = 0.0f;
float Depest_Y = 0.0f;
void GXI_DefaultSet1(GXI_SetupMenu *p_Root)
{
	u32 LC;
	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	{
		if (p_Root->Leaves[LC].DefaultSelection == -2) 
			p_Root->Leaves[LC].DefaultSelection = -1;
		if (p_Root->Leaves[LC].p_SubMenu)
		{
			GXI_DefaultSet1(p_Root->Leaves[LC].p_SubMenu);
		}
	}
}

u32 GXI_DefaultSet2(GXI_SetupMenu *p_Root)
{
	u32 LC , Setted , AtLeastOne;
	AtLeastOne = 0;
	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	{
		Setted = 0; 
		if ((p_Root->Leaves[LC].p_SubMenu) && (p_Root->Leaves[LC].Selection == -1))
		{
			Setted = GXI_DefaultSet2(p_Root->Leaves[LC].p_SubMenu);
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
void GXI_DrawText_STP(int X,int Y,char *P_String,long Color,long ColorBack,long TFront,long TBack)
{
	GXI_DrawText(X,GXI_Global_ACCESS(Ysize) - Y ,P_String,Color,ColorBack,TFront,TBack);
}


void GXI_Stp_Draw(GXI_SetupMenu *p_Root , int X , int Y,int bIsSelection ,int bIsDefaultSelection)
{
	u_int LC , Max;
	u_int Color;
	u_int ColorHL;
	char String[100];
	Max = 0;
	p_Root->OpenSize += 4;
	Depest_X = X;
	Depest_Y = Y;
	
	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	if (Max < GXI_Stp_GetLenght(p_Root->Leaves[LC].Name))
	{
		Max = GXI_Stp_GetLenght(p_Root->Leaves[LC].Name);
	}
	if (Max < GXI_Stp_GetLenght(p_Root->Title))
	{
		Max = GXI_Stp_GetLenght(p_Root->Title);
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
	if ((X + SETUP_BigX + Max * 8) > GXI_Global_ACCESS(Xsize))
	{
		X = GXI_Global_ACCESS(Xsize) - (Max * 8);
	}
	{
		ULONG OPENC,ISOP;
		ISOP = 0xffffffff;
		OPENC = p_Root ->NumberOfLeaves;
		while (OPENC--) if (p_Root->Leaves[OPENC].Hlight || p_Root->Leaves[OPENC].Open) ISOP = OPENC;
		if (ISOP != 0xffffffff)
		{
			if ((SETUP_BigY + Y + (ISOP + 2) * 16) > (GXI_Global_ACCESS(Ysize) - 2 * WIDE_SCREEN_ADJUST))
			{
				Y -= SETUP_BigY + Y + (ISOP + 2) * 16 - GXI_Global_ACCESS(Ysize) + 2 * WIDE_SCREEN_ADJUST;
			}
		}
	}
	Y += WIDE_SCREEN_ADJUST;
	if (p_Root->Title[0] != 0) 
	{
		GXI_DrawText_STP(SETUP_BigX + X    ,SETUP_BigY + Y,String,			0,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Alpha);
		GXI_DrawText_STP(SETUP_BigX + X + 8,SETUP_BigY + Y,p_Root->Title	,	0x7070ff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);
	}
	if (p_Root->Title[0] != 0) Y += 16;
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
		if ((p_Root->Leaves[LC].p_SubMenu) && (p_Root->Leaves[LC].p_SubMenu == &GXI_Stp_OnOff))
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
			GXI_DrawText_STP(SETUP_BigX + X    ,SETUP_BigY + Y+LC * 16,String,			Color,ColorHL,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);
			GXI_DrawText_STP(SETUP_BigX + X + 8,SETUP_BigY + Y+LC * 16,p_Root->Leaves[LC].Name,	Color,ColorHL,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);
		}
		String[Max - 1] = ' ';
		Color = SC;
		String[0] = ' ';
		String[Max - 1] = ' ';
		String[Max] = ' ';
		p_Root->Leaves[LC].Name[Max] = SaveSM;
		p_Root->Leaves[LC].Name[Max - 1] = SaveSMOnOf;
	}
	Y -= WIDE_SCREEN_ADJUST;
	for (LC = 0 ; LC < p_Root ->NumberOfLeaves ; LC ++)
	{
		if (p_Root->Leaves[LC].p_SubMenu)
		{
			if (p_Root->Leaves[LC].Open)
			{
				GXI_Stp_Draw(p_Root->Leaves[LC].p_SubMenu, X + Max * 10 + 4, Y+LC * 16 , p_Root->Leaves[LC].Selection , p_Root->Leaves[LC].DefaultSelection);
			} else
				p_Root->Leaves[LC].p_SubMenu->OpenSize = 0;
			
		}
	}
}

#include "INOut/INOjoystick.h"
u_int NewJoy;
u_int OldJoy;
u_int DltJoy;
u_int OldDltJoy;
u_int CounterDelay;
void GXI_InitJoySetup()
{
	NewJoy = 0;
	NewJoy = GXI_gi_EnginePadState;
	DltJoy = OldJoy ^ NewJoy;
	DltJoy &= NewJoy;

	if (OldJoy == NewJoy)
	{
		CounterDelay++;
		if (CounterDelay > 100)
		{
			DltJoy = NewJoy;
		}
		else
		if (CounterDelay > 45)
		{
			if ((CounterDelay & 0x3) == 0)
			{
				DltJoy = NewJoy;
			}
		}
		else
		if (CounterDelay > 15)
		{
			if ((CounterDelay & 0x7) == 0)
			{
				DltJoy = NewJoy;
			}
		}
	} else CounterDelay = 0;

	OldJoy = NewJoy;
}

BOOL GXI_b_Joystick_IsButtonJustDown(INO_tden_ButtonId Item)
{
	switch(Item) 
	{
	case e_RIGHT: 	return DltJoy & 1;
	case e_LEFT: 	return DltJoy & 2;
	case e_DOWN: 	return DltJoy & 4;
	case e_UP: 		return DltJoy & 8;
	case e_L1: 		return DltJoy & 16;
	case e_R1: 		return DltJoy & 32;
	}
    return FALSE;
}
GXI_SetupMenu *GXI_Stp_GetMenu(GXI_SetupMenu *Root , u32 DepthL , u32 *DepthStackL)
{
	GXI_SetupMenu *Ret;
	Ret = Root;
	while ((DepthL--) && Ret)
	{
		Ret = Ret->Leaves[*(DepthStackL++)].p_SubMenu;
	}
	return Ret;
}
static u32 b_first = 1;
void GXI_Setup()
{
	static slowmenuinputs = 0;
	
	u32 paddata , bValidDetected ;
	static u32 CurrentFrameCounter = 0;
	static u32 NoOpenCloseCounter = 0;
	static u32 DontForceNoOpenCloseCounter = 0;
//	static u16 l_button;
//	u16 l_iJustDown;
    extern int gi_crash;
	
//	GC_tdst_DualShock2 *P_DS2;
	GXI_SetupMenu *CurrentMenu;
	GXI_Stp_OneLeaf *CurrentLeaf;
	{
		//sprintf(GXI_Stp_Root.Title ,"Jade version %d" , BIG_Cu4_AppVersion );
		//sprintf(GXI_Stp_Tool.Title ,"%dK / %dK tex mem used" , TextureUsed>>10 , GC_GetTextureMemoryAvailable()>>10);
	}
	
    if(gi_crash) return;
	paddata = 0;
	GC_MenuIsOpen = 1;
	if(iVirtualPressedKeyTime)
	{
	    iVirtualPressedKeyTime--;
	}
	else
	{
	    ucVirtualPressedKey[0] = ucVirtualPressedKey[1] =ucVirtualPressedKey[2]= 0;
	}

	if (b_first) GC_STP_FirstInt();
	b_first = 0;

/*	if (bTakeSnapshot)
	{
		bTakeSnapshot--;
		if (bTakeSnapshot == 1)
		{
			GC_Snapshot(0);
		}
		if (bTakeSnapshot == 5)
		{
			GC_Snapshot(1);
			bTakeSnapshot = 0;
		}
		return;
	}
	*/
	
	GXI_FisrtInitMenu(&GXI_Stp_Root); // <= Extra SubCountering Xor Vlidate

//	PADRead(gc_pad);
//	PADClamp(gc_pad);

/*	if(!(gc_pad[0].err != PAD_ERR_NO_CONTROLLER && gc_pad[0].err != PAD_ERR_NOT_READY))
	{
		PADReset(PAD_CHAN0_BIT);
		PADRecalibrate(PAD_CHAN0_BIT);
		l_button = 0;
	}
	else
	{	
	    // check for the states of the buttons on the device     
	    l_iJustDown = PADButtonDown(l_button,gc_pad[0].button);
	    l_button = gc_pad[0].button; //save the state
    }
*/	
	//VideoMode = GXI_Global_ACCESS(VideoMode);

	bValidDetected = 0;
	GXI_InitMenu(&GXI_Stp_Root);
	CurrentMenu = GXI_Stp_GetMenu(&GXI_Stp_Root , Depth , (u32*)DepthStack);
	CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
	NoOpenCloseCounter ++;
	
	GXI_InitJoySetup();
	if(GXI_gb_MenuMode)
	{
	if (GXI_b_Joystick_IsButtonJustDown(e_RIGHT))
	{
		if(!(GXI_Global_ACCESS(Status) & GC_Status_Setup_On))
		{
			GXI_Global_ACCESS(Status) |= GC_Status_Setup_On;
			return;
		}

		if (CurrentLeaf->p_SubMenu)
		{
			CurrentLeaf->Open = 1;
			NoOpenCloseCounter = 0;
			DepthStack[++Depth] = CurrentLeaf->Selection % CurrentMenu->NumberOfLeaves;
			if (CurrentLeaf->Selection == -1) DepthStack[Depth] = 0;
			else 
			{
			   GXI_Validate(CurrentLeaf , 0 , 1);
			   DepthStack[Depth] = CurrentLeaf->Selection;
			}
		} else 
		{
			bValidDetected = 1;
		}
	} else
	if (GXI_b_Joystick_IsButtonJustDown(e_LEFT))
	{
		NoOpenCloseCounter = 0;
		CurrentLeaf->Open = 0;
		if (Depth != 0)
		{
			Depth --;
		} else goto SetupExit;
		CurrentMenu = GXI_Stp_GetMenu(&GXI_Stp_Root , Depth , (u32*)DepthStack);
		CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
		CurrentLeaf->Open = 0;
	} else
	if (GXI_b_Joystick_IsButtonJustDown(e_DOWN))
	{
		DepthStack[Depth]++;
		DepthStack[Depth] %= CurrentMenu->NumberOfLeaves;
	} else
		if (GXI_b_Joystick_IsButtonJustDown(e_UP))
		{
			if (DepthStack[Depth] == 0)
				DepthStack[Depth] = CurrentMenu->NumberOfLeaves - 1;
			else
				DepthStack[Depth]--;
		}
	
	}
	
   	CurrentMenu = GXI_Stp_GetMenu(&GXI_Stp_Root , Depth , (u32*)DepthStack);
	CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
	if (CurrentMenu->AutoSelection) 
		bValidDetected = 1;
	
	
	if (GXI_b_Joystick_IsButtonJustDown(e_L1))
	{
		DontForceNoOpenCloseCounter ^= 1;
		NoOpenCloseCounter = 0;
	}
	if ((GXI_b_Joystick_IsButtonJustDown(e_R1)) || bValidDetected)
	{
		if ((CurrentMenu->Leaves[DepthStack[Depth]].ID != GC_STP_ID_NULL) && (CurrentMenu->Leaves[DepthStack[Depth]].Selection == -1))
		{
			if (CurrentMenu->Leaves[DepthStack[Depth]].ID == GC_STP_ID_EXIT) goto SetupExit;
			GXI_Validate(&CurrentMenu->Leaves[DepthStack[Depth]] , 0 , 0);
		}
		else
		if (Depth)
		{
			GXI_SetupMenu *Previousmenu;
			Previousmenu = GXI_Stp_GetMenu(&GXI_Stp_Root , Depth - 1 , (u32*)DepthStack);
			if (Previousmenu->Leaves[DepthStack[Depth-1]].Selection != -1)
			{
				Previousmenu->Leaves[DepthStack[Depth-1]].Selection = DepthStack[Depth];
				if (Previousmenu->Leaves[DepthStack[Depth - 1]].ID != GC_STP_ID_NULL)
					GXI_Validate(&Previousmenu->Leaves[DepthStack[Depth-1]] , Previousmenu->Leaves[DepthStack[Depth-1]].Selection , 0);
			}
		}
	}
	
	if (CurrentMenu == &GXI_Stp_PlusMoin)
	{
		DepthStack[Depth] = 1;
	}
	
	
	CurrentLeaf = &CurrentMenu->Leaves[DepthStack[Depth]];
	CurrentLeaf -> Hlight = 1;
	
	GXI_DefaultSet1(&GXI_Stp_Root);
	GXI_DefaultSet2(&GXI_Stp_Root);
	
	if(!(GXI_Global_ACCESS(Status) & GC_Status_Setup_On))
		return;
	
	GXI_Stp_Draw(&GXI_Stp_Root ,10 , 10 , -1 , -1);
	if (DontForceNoOpenCloseCounter) NoOpenCloseCounter = 1000000;
	Depest_X = 20.0f - Depest_X;
	Depest_Y = 20.0f - Depest_Y;
	{
		float fDX , fDY , fCoef;
		fDX = (Depest_X - SETUP_BigX);
		fDY = (Depest_Y - SETUP_BigY);
		fCoef = 0.05f;
		if (fDX > 0.0f) 
		{
			fCoef = 0.25f;
		}
		SETUP_BigX += fDX * fCoef;
		SETUP_BigY += fDY * fCoef;
	}
	CurrentLeaf -> Hlight = 0;

	return;
SetupExit:	
	DontForceNoOpenCloseCounter = 1;
	NoOpenCloseCounter = 0;
	Depth = 0;
	DepthStack[0] = 0;
	GXI_Global_ACCESS(Status) &= ~GC_Status_Console_On;
	GC_MenuIsOpen = 0;
	GXI_Global_ACCESS(Status) &= ~GC_Status_Setup_On;
	GXI_Stp_MenuClose(&GXI_Stp_Root);
    GXI_gb_MenuMode = 0;
}


#endif // _FINAL_

void GC_SpecialInit(void)
{
	//VSync Stability = 70
	//GXI_Global_ACCESS(ulVSyncStability) = 7 << 3;
}


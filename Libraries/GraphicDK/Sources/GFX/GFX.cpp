/*$T GFX.c GC! 1.100 03/08/01 17:24:45 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "GFX/GFX.h"
#include "GFX/GFXline.h"
#include "GFX/GFXlightning.h"
#include "GFX/GFXflare.h"
#include "GFX/GFXsmoke.h"
#include "GFX/GFXtable.h"
#include "GFX/GFXwater.h"
#include "GFX/GFXspeed.h"
#include "GFX/GFXdesintegrate.h"
#include "GFX/GFXspark.h"
#include "GFX/GFXfade.h"
#include "GFX/GFXProfiler.h"
#include "GFX/GFXExplode.h"
#include "GFX/GFXcarte.h"
#include "GFX/GFXstring.h"
#include "GFX/GFXripple.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTZList.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#if defined( _GAMECUBE )
#  if !defined( _FINAL_ )
#    include "GXI_GC/GXI_dbg.h"
#  endif
#elif defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL )
// nothing...
#elif defined( _XBOX )
#  include "GX8\Gx8VertexBuffer.h"
#elif defined( PSX2_TARGET )
/* mamagouille */
#  include "PSX2debug.h"
#elif defined( _PC_RETAIL )
// PCretail-specific include...
#elif defined(_XENON)
#else
#  error unsupported platform
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

GFX_tdst_List			*GFX_gpst_Current = NULL;
GFX_tdst_List			**GFX_ppst_CurrentList = NULL;
static unsigned short	GFX_sguw_Id = GFX_FirstNotSorted;
OBJ_tdst_GameObject		*GFX_gpst_GO = NULL;
GEO_tdst_Object			*GFX_gpst_Geo = NULL;
static ULONG			GFX_ul_MaxPoints;
static ULONG			GFX_ul_MaxUVs;

static ULONG			GFX_ul_MaxElements;
static ULONG			GFX_aul_MaxTriangles[64];

static ULONG            GFX_ul_MaxElemSprites;
static ULONG            GFX_aul_MaxSprites[32];

static ULONG			GFX_ul_MaxRLI = 0;
static ULONG			*GFX_pul_RLI = NULL;

#ifdef _DEBUGGFXGEOM

static ULONG			GFX_ul_AskedPoints;
static ULONG			GFX_ul_AskedUVs;
static ULONG			GFX_ul_AskedElements;
static ULONG			GFX_aul_AskedTriangles[64];
static ULONG            GFX_ul_AskedElemSprites;
static ULONG            GFX_aul_AskedSprites[32];
static ULONG			GFX_ul_AskefRLI = 0;

#endif

static int				ga_Paramf_First[GFX_Number] =
{
	0,
	GFX_LineParamsf_First,
	GFX_LightningParamsf_First,
	GFX_DesintegrateParamsf_First,
	GFX_FlareParamsf_First,
	GFX_ComplexFlareParamsf_First,
	GFX_SmokeParamsf_First,
	GFX_TableParamsf_First,
	GFX_SpeedParamsf_First,
	GFX_SparkParamsf_First,
	GFX_Water2Paramsf_First,
	GFX_FadeParamsf_First,
	GFX_ProfilerParamsf_First,
	GFX_ExplodeParamsf_First,
	GFX_BorderFadeParamsf_First,
	GFX_DiaphragmParamsf_First,
	GFX_CarteParamsf_First,
    GFX_GaoParamsf_First,
    GFX_SunParamsf_First,
    GFX_StrParamsf_First,
    GFX_RippleParamsf_First,
    GFX_AnotherFlareParamsf_First,
    GFX_TetrisParamsf_First,
};

static int				ga_Paramf_Number[GFX_Number] =
{
	0,
	GFX_LineParamsf_Number,
	GFX_LightningParamsf_Number,
	GFX_DesintegrateParamsf_Number,
	GFX_FlareParamsf_Number,
	GFX_ComplexFlareParamsf_Number,
	GFX_SmokeParamsf_Number,
	GFX_TableParamsf_Number,
	GFX_SpeedParamsf_Number,
	GFX_SparkParamsf_Number,
	GFX_Water2Paramsf_Number,
	GFX_FadeParamsf_Number,
	20,
	20,
	GFX_BorderFadeParamsf_Number,
	GFX_DiaphragmParamsf_Number,
	GFX_CarteParamsf_Number,
    GFX_GaoParamsf_Number,
    GFX_SunParamsf_Number,
    GFX_StrParamsf_Number,
    GFX_RippleParamsf_Number,
    GFX_AnotherFlareParamsf_Number,
    GFX_TetrisParamsf_Number,
};

static int				ga_Parami_First[GFX_Number] =
{
	0,
	GFX_LineParamsi_First,
	GFX_LightningParamsi_First,
	GFX_DesintegrateParamsi_First,
	GFX_FlareParamsi_First,
	GFX_ComplexFlareParamsi_First,
	GFX_SmokeParamsi_First,
	GFX_TableParamsi_First,
	GFX_SpeedParamsi_First,
	GFX_SparkParamsi_First,
	GFX_Water2Paramsi_First,
	GFX_FadeParamsi_First,
	GFX_ProfilerParamsi_First,
	GFX_ExplodeParamsi_First,
	GFX_BorderFadeParamsi_First,
	GFX_DiaphragmParamsi_First,
	GFX_CarteParamsi_First,
    GFX_GaoParamsi_First,
    GFX_SunParamsi_First,
    GFX_StrParamsi_First,
    GFX_RippleParamsi_First,
    GFX_AnotherFlareParamsi_First,
    GFX_TetrisParamsi_First,
};
static int				ga_Parami_Number[GFX_Number] =
{
	0,
	GFX_LineParamsi_Number,
	GFX_LightningParamsi_Number,
	GFX_DesintegrateParamsi_Number,
	GFX_FlareParamsi_Number,
	GFX_ComplexFlareParamsi_Number,
	GFX_SmokeParamsi_Number,
	GFX_TableParamsi_Number,
	GFX_SpeedParamsi_Number,
	GFX_SparkParamsi_Number,
	GFX_Water2Paramsi_Number,
	GFX_FadeParamsi_Number,
	20,
	20,
	GFX_BorderFadeParamsi_Number,
	GFX_DiaphragmParamsi_Number,
	GFX_CarteParamsi_Number,
    GFX_GaoParamsi_Number,
    GFX_SunParamsi_Number,
    GFX_StrParamsi_Number,
    GFX_RippleParamsi_Number,
    GFX_AnotherFlareParamsi_Number,
    GFX_TetrisParamsi_Number,
};

static int				ga_Paramv_First[GFX_Number] =
{
	0,
	GFX_LineParamsv_First,
	GFX_LightningParamsv_First,
	GFX_DesintegrateParamsv_First,
	GFX_FlareParamsv_First,
	GFX_ComplexFlareParamsv_First,
	GFX_SmokeParamsv_First,
	GFX_TableParamsv_First,
	GFX_SpeedParamsv_First,
	GFX_SparkParamsv_First,
	GFX_Water2Paramsv_First,
	GFX_FadeParamsv_First,
	GFX_ProfilerParamsv_First,
	GFX_ExplodeParamsv_First,
	GFX_BorderFadeParamsv_First,
	GFX_DiaphragmParamsv_First,
	GFX_CarteParamsv_First,
    GFX_GaoParamsv_First,
    GFX_SunParamsv_First,
    GFX_StrParamsv_First,
    GFX_RippleParamsv_First,
    GFX_AnotherFlareParamsv_First,
    GFX_TetrisParamsv_First,
};
static int				ga_Paramv_Number[GFX_Number] =
{
	0,
	GFX_LineParamsv_Number,
	GFX_LightningParamsv_Number,
	GFX_DesintegrateParamsv_Number,
	GFX_FlareParamsv_Number,
	GFX_ComplexFlareParamsv_Number,
	GFX_SmokeParamsv_Number,
	GFX_TableParamsv_Number,
	GFX_SpeedParamsv_Number,
	GFX_SparkParamsv_Number,
	GFX_Water2Paramsv_Number,
	GFX_FadeParamsv_Number,
	20,
	20,
	GFX_BorderFadeParamsv_Number,
	GFX_DiaphragmParamsv_Number,
	GFX_CarteParamsv_Number,
    GFX_GaoParamsv_Number,
    GFX_SunParamsv_Number,
    GFX_StrParamsv_Number,
    GFX_RippleParamsv_Number,
    GFX_AnotherFlareParamsv_Number,
    GFX_TetrisParamsv_Number,
};

/*$off*/
static short			gaa_Paramf_Offset[GFX_Number][GFX_Paramsf_MaxNumber] =
{
  /*Unknown		    */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Line		    */  { 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Lightning	    */  { 36, 40, 44, 48, 52, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Desintegrate    */  { 12, 16, 20, 24, 28, 32, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Flare		    */  { 24, 36, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*ComplexFlare    */  { 24, 36, 40, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144 },
  /*Smoke		    */  { 36, 40, 44, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Table		    */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Speed		    */  { 0, 4, 8, 12, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Spark           */  { 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Water2		    */  { 24 + GFX_Special, 28 + GFX_Special, 32 + GFX_Special, 36 + GFX_Special, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Fade            */  { 8, 12, 16, 20, 24, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Profiler	    */  { GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special },
  /*Explode		    */  { GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special },
  /*BorderFade      */  { 12, 16, 20, 24, 28, 32, 36, 44, 48, 52, 56, 60, 0, 0, 0, 0 },
  /*Diaphragm       */  { 4, 8, 12, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Carte           */	{ 84, 88, 92, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Gao             */  { 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Sun             */  { 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 0, 0, 0, 0, 0, 0, 0 },
  /*Str             */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Ripple          */  { 36, 40, 44 + GFX_Special, 48, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Another Flare   */  { 24, 28, 32, 36, 40, 44, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*tetris          */  { 36, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

static short			gaa_Parami_Offset[GFX_Number][GFX_Paramsi_MaxNumber] =
{
  /*Unknown		    */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Line		    */  { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Lightning	    */  { 60, 64, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Desintegrate    */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Flare		    */  { 28 + GFX_TwoByteInteger, 72, 76, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*ComplexFlare    */  { 28 + GFX_TwoByteInteger, 72, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212 },
  /*Smoke		    */  { GFX_Special, 52, 56, GFX_Special, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Table		    */  { 0 + GFX_TwoByteInteger, 2 + GFX_OneByteInteger, 3 + GFX_OneByteInteger, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 0, 0, 0, 0, 0, 0, 0 },
  /*Speed		    */  { 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Spark           */  { 84, 88, 92, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Water2		    */  { 0, 4, GFX_Special, 44 + GFX_Special, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Fade            */  { 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Profiler	    */  { GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special },
  /*Explode		    */  { GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special },
  /*BorderFade      */  { 0, 4, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Diaphragm       */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Carte           */  { GFX_Special, GFX_Special, GFX_Special, 104, 103 + GFX_OneByteInteger, 100 + GFX_OneByteInteger, 101 + GFX_OneByteInteger, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Gao             */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Sun             */  { GFX_Special, 72, 76, 80, 84 + GFX_OneByteInteger, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*String          */  { GFX_Special, 4, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Ripple          */  { GFX_Special, 60, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Another Flare   */  { 52 + GFX_OneByteInteger, 56, 60, 53 + GFX_OneByteInteger, 55 + GFX_OneByteInteger, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*tetris          */  { 44, 48, 52, 56, 60, 64, 0, 0, 0, 0, 0, 0, 0, 0 },
};

static short			gaa_Paramv_Offset[GFX_Number][GFX_Paramsv_MaxNumber] =
{
  /*Unknown		    */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Line		    */  { 0, 12, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Lightning	    */  { 0, 12, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Desintegrate    */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Flare		    */  { 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*ComplexFlare    */  { 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Smoke		    */  { 0, 12, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Table		    */  { 44, GFX_Special, GFX_Special, GFX_Special, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Speed		    */  { 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Spark           */  { 0, 12, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Water2		    */  { 0 + GFX_Special, 12 + GFX_Special, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Fade            */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Profiler	    */  { GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special },
  /*Explode		    */  { GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special, GFX_Special },
  /*BorderFade      */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Diaphragm       */  { 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Carte           */  { 0, 12, 24, 36, 48, 60, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Gao             */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Sun             */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*String          */  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Ripple          */  { 0, 12, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*Another Flare   */  { 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  /*tetris          */  { 0, 12, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};
/*$on*/

/*$4
 ***********************************************************************************************************************
    internal Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_FreeOne(GFX_tdst_List *_pst_GFX)
{
	if(_pst_GFX->c_Type == GFX_Desintegrate)
		GFX_Desintegrate_Destroy(_pst_GFX->p_Data);
	else if(_pst_GFX->c_Type == GFX_Lightning)
		GFX_Lightning_Destroy(_pst_GFX->p_Data);
    else if(_pst_GFX->c_Type == GFX_Smoke)
		GFX_Smoke_Destroy(_pst_GFX->p_Data);
	else if(_pst_GFX->c_Type == GFX_Spark)
		GFX_Spark_Destroy(_pst_GFX->p_Data);
	else if(_pst_GFX->c_Type == GFX_Water2)
		GFX_Water2_Destroy(_pst_GFX->p_Data);
	else if(_pst_GFX->c_Type == GFX_Profiler)
		GFX_Profiler_Destroy(_pst_GFX->p_Data);
	else if(_pst_GFX->c_Type == GFX_Explode)
	{
		GFX_Explode_Destroy(_pst_GFX->p_Data);
		MEM_FreeAlign(_pst_GFX->p_Data);
	}
	else if(_pst_GFX->c_Type == GFX_Carte)
		GFX_Carte_Destroy(_pst_GFX->p_Data, _pst_GFX->p_Material);
    else if (_pst_GFX->c_Type == GFX_Gao)
        GFX_Gao_Destroy( _pst_GFX->p_Data );
    else if (_pst_GFX->c_Type == GFX_String)
        GFX_Str_Destroy( _pst_GFX->p_Data );
    else if (_pst_GFX->c_Type == GFX_Ripple)
        GFX_Ripple_Destroy( _pst_GFX->p_Data );

	if (_pst_GFX->c_Type != GFX_Explode)
		MEM_Free(_pst_GFX->p_Data);
	MEM_Free(_pst_GFX);
	
	#ifdef _GAMECUBE
//	MEM_Defrag(0);
	#endif
}

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Attached( GFX_tdst_List *_pst_GFX )
{
    GFX_tdst_List *pst_Cur;

    pst_Cur = _pst_GFX;
    while( pst_Cur )
    {
        if (pst_Cur->c_Type == GFX_Carte)
        {
            ( (GFX_tdst_Carte *) pst_Cur->p_Data)->i_CurPieceStatus = 0xFFFFFFFF;
        }
        pst_Cur = pst_Cur->pst_Next;
    }
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
GFX_tdst_List	*GFX_IsExist(GFX_tdst_List *_pst_GFX, USHORT _uw_Id)
{
	if(GFX_gpst_Current && (GFX_gpst_Current->uw_Id == _uw_Id)) return GFX_gpst_Current;
	GFX_gpst_Current = _pst_GFX;
	while(GFX_gpst_Current != NULL)
	{
		if(GFX_gpst_Current->uw_Id == _uw_Id) return GFX_gpst_Current;
		GFX_gpst_Current = GFX_gpst_Current->pst_Next;
	}
	return 0;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_SetCurrent(GFX_tdst_List *_pst_GFX, USHORT _uw_Id)
{
	if(GFX_gpst_Current && (GFX_gpst_Current->uw_Id == _uw_Id)) return;
	GFX_gpst_Current = _pst_GFX;
	while(GFX_gpst_Current != NULL)
	{
		if(GFX_gpst_Current->uw_Id == _uw_Id) break;
		GFX_gpst_Current = GFX_gpst_Current->pst_Next;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Free(GFX_tdst_List **_ppst_GFX, int _i_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_List	*pst_Prev;
	GFX_tdst_List	*pst_Next;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Prev = NULL;
	pst_GFX = (*_ppst_GFX);

	while(pst_GFX)
	{
		pst_Next = pst_GFX->pst_Next;

		if
		(
			(_i_Type != UNI_Cuc_InitInPause)
		||	(!pst_GFX->p_Owner)
		||	(((OBJ_tdst_GameObject *) pst_GFX->p_Owner)->ul_StatusAndControlFlags & OBJ_C_ControlFlag_InPause)
		)
		{
			GFX_FreeOne(pst_GFX);
			if(!pst_Prev)
				*_ppst_GFX = pst_Next;
			else
				pst_Prev->pst_Next = pst_Next;
		}
		else
		{
			pst_Prev = pst_GFX;
		}

		pst_GFX = pst_Next;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_End(void)
{
	ULONG i;
	
	if(GFX_gpst_GO)
	{
#if defined(_XENON_RENDER)
        // SC: We dont have to free the meshes since they are dynamic and will be freed with
        //     the rest of the global pool
        if (GFX_gpst_GO->pst_Base->pst_Visu->p_XeElements)
        {
            MEM_Free(GFX_gpst_GO->pst_Base->pst_Visu->p_XeElements);
            GFX_gpst_GO->pst_Base->pst_Visu->p_XeElements   = NULL;
            GFX_gpst_GO->pst_Base->pst_Visu->l_NbXeElements = 0;
        }
#endif
		GFX_gpst_GO->pst_Base->pst_Visu->pst_Material = NULL;
        GFX_gpst_GO->pst_Base->pst_Visu->pst_Object = NULL;
		OBJ_GameObject_Remove(GFX_gpst_GO, 1);
		GFX_gpst_GO = NULL;
	}

	if(GFX_gpst_Geo)
	{
		GFX_gpst_Geo->dul_PointColors = NULL;
        GFX_gpst_Geo->l_NbElements = GFX_ul_MaxElements;
        for (i = 0; i < GFX_ul_MaxElements; i++)
			GFX_gpst_Geo->dst_Element[ i ].l_NbTriangles = GFX_aul_MaxTriangles[ i ];
        GFX_gpst_Geo->l_NbSpritesElements = GFX_ul_MaxElemSprites;
		GEO_Free(GFX_gpst_Geo);
		GFX_gpst_Geo = NULL;
	}

	if(GFX_pul_RLI)
	{
		MEM_Free(GFX_pul_RLI);
		GFX_pul_RLI = NULL;
		GFX_ul_MaxRLI = 0;
	}
}

#include "BASe/BENch/BENch.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GFX_tdst_List *GFX_pst_FindById(GFX_tdst_List **_ppst_GFX, USHORT _uw_Id)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_List	*pst_Cur;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cur = *_ppst_GFX;
	while(pst_Cur)
	{
		if(pst_Cur->uw_Id == _uw_Id) return pst_Cur;
		pst_Cur = pst_Cur->pst_Next;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_SetDeath(GFX_tdst_List **_ppst_GFX, GFX_tdst_List *pst_GFX)
{
	if (!pst_GFX) return 0;

	switch(pst_GFX->c_Type)
	{
	case GFX_Flare:	
		if(((GFX_tdst_ComplexFlare *) pst_GFX->p_Data)->l_Flags & GFX_Flare_DieTime)
		{
			((GFX_tdst_ComplexFlare *) pst_GFX->p_Data)->l_Flags |= GFX_Flare_Death;
			*(LONG *) &((GFX_tdst_ComplexFlare *) pst_GFX->p_Data)->st_BufferPos.z = -1;
			return 1;
		}
		break;
        
    case GFX_AnotherFlare:
        ((GFX_tdst_AnotherFlare *) pst_GFX->p_Data)->c_Flags |= GFX_AnotherFlare_Die;
        pst_GFX->f_LifeTime = ((GFX_tdst_AnotherFlare *) pst_GFX->p_Data)->f_DeathTime + 10.0f;
		return 1;
        
    case GFX_Explode:
		((GFX_tdst_Explode *) pst_GFX->p_Data)->ulNumberToGenerate = 0;
		return 1;
	}

	/* Real destruct */
	GFX_gpst_Current = pst_GFX;
	GFX_Del(_ppst_GFX, pst_GFX->uw_Id);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Del(GFX_tdst_List **_ppst_GFX, USHORT _uw_Id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_List	*pst_Prev, *pst_Cur;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Prev = NULL;
	pst_Cur = *_ppst_GFX;

	while(pst_Cur)
	{
		if(pst_Cur->uw_Id == _uw_Id)
		{
			if(pst_Prev)
				pst_Prev->pst_Next = pst_Cur->pst_Next;
			else
				*_ppst_GFX = pst_Cur->pst_Next;

			GFX_FreeOne(pst_Cur);
			break;
		}

		pst_Prev = pst_Cur;
		pst_Cur = pst_Cur->pst_Next;
	}

	GFX_gpst_Current = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_DelOwnedBy(GFX_tdst_List **_ppst_GFX, void *_p_Owner)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_List	*pst_Next, *pst_Cur;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cur = *_ppst_GFX;
	if((int) pst_Cur == -1) return;

	while(pst_Cur)
	{
		pst_Next = pst_Cur->pst_Next;
		if(pst_Cur->p_Owner == _p_Owner)
		{
			if(pst_Cur->c_Flags & GFX_DoNotDestroyWithOwner)
				pst_Cur->p_Owner = NULL;
			else
				GFX_i_SetDeath(_ppst_GFX, pst_Cur);
		}

		pst_Cur = pst_Next;
	}

	GFX_gpst_Current = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef PSX2_TARGET
extern u_int NoGFX;
#endif

#ifdef _XBOX
#include "GX8/RASter/Gx8_CheatFlags.h"
#endif // _XBOX
 
void GFX_SetRenderState( char _c_Sorted )
{
    GDI_gpst_CurDD->ul_CurrentDrawMask = GDI_gpst_CurDD->ul_DrawMask & (~GDI_Cul_DM_ReceiveDynSdw);
    GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix = 0;
	GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix = GDI_gpst_CurDD->st_MatrixStack.dst_Matrix;
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
    GDI_gpst_CurDD->pst_CurrentGameObject = GFX_gpst_GO;
	GDI_gpst_CurDD->pst_AdditionalMaterial = NULL;
    /*if (_c_Sorted )
        SOFT_ZList_Lock();
    else
    {
        SOFT_ZList_Unlock();
        GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_DontSortObject;
    }*/
}

void GFX_Render(GFX_tdst_List **_ppst_GFX, char _c_Sorted )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				DM, DI, DITrans, DITransNotSorted, DIOpaque;
	unsigned short		uw_SaveCurMatrix;
	GFX_tdst_List		*pst_GFX, GFX;
	GFX_tdst_List		*pst_NextGFX;
#ifdef PSX2_TARGET
	OBJ_tdst_GameObject	*pst_GO;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GFX = *_ppst_GFX;
	if(!pst_GFX) return;
#if defined(PSX2_TARGET) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if (NoGFX) return;
#endif	
#ifdef GX8_BENCH 
	if (g_iNoGFX) return; 
#endif	 

#ifdef ACTIVE_EDITORS
    if ( !(GDI_gpst_CurDD->ul_GFXRenderMask & 1)) 
        return;
#endif


	_GSP_BeginRaster(7);

    PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GFX );

	GFX_ppst_CurrentList = _ppst_GFX;
	DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
	DI = GDI_gpst_CurDD->ul_DisplayInfo;
	GDI_gpst_CurDD->ul_CurrentDrawMask = GDI_gpst_CurDD->ul_DrawMask & (~GDI_Cul_DM_ReceiveDynSdw);
	uw_SaveCurMatrix = GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix;
	GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix = 0;
	GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix = GDI_gpst_CurDD->st_MatrixStack.dst_Matrix;
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);

	GDI_gpst_CurDD->pst_CurrentGameObject = GFX_gpst_GO;
	GDI_gpst_CurDD->pst_AdditionalMaterial = NULL;

    if (_c_Sorted )
    {
        SOFT_ZList_Lock();
        DITrans = GDI_gpst_CurDD->ul_DisplayInfo | GDI_Cul_DI_RenderingTransparency;
        DITransNotSorted = GDI_gpst_CurDD->ul_DisplayInfo | GDI_Cul_DI_RenderingTransparency;
        DIOpaque = GDI_gpst_CurDD->ul_DisplayInfo & ~GDI_Cul_DI_RenderingTransparency;
    }
    else
    {
        GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_DontSortObject;
        DITrans = GDI_gpst_CurDD->ul_DisplayInfo | GDI_Cul_DI_ForceSortTriangle;
        DITransNotSorted = GDI_gpst_CurDD->ul_DisplayInfo | GDI_Cul_DI_RenderingTransparency;
        DIOpaque = GDI_gpst_CurDD->ul_DisplayInfo & ~GDI_Cul_DI_ForceSortTriangle;
    }
#ifdef JADEFUSION
	DITrans          |= GDI_Cul_DI_RenderingGFX;
	DITransNotSorted |= GDI_Cul_DI_RenderingGFX;
	DIOpaque         |= GDI_Cul_DI_RenderingGFX;
#endif
	while(pst_GFX)
	{
        PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_NbGFX, 1);
		pst_NextGFX = pst_GFX->pst_Next;

        GFX = *pst_GFX;

        /* GFX has ended : Self destruct ? */
		if(pst_GFX->c_Flags & GFX_Terminated)
		{
			if(pst_GFX->c_Flags & GFX_AutoDestruct)
			{
				GFX_gpst_Current = pst_GFX;
				GFX_Del(_ppst_GFX, pst_GFX->uw_Id);
				pst_GFX = pst_NextGFX;
				continue;
			}
		}

		/* GFX time life */
		pst_GFX->f_LifeTime -= TIM_gf_dt;
		if(pst_GFX->f_LifeTime <= 0)
		{
			if(!GFX_i_SetDeath(_ppst_GFX, pst_GFX))
			{
				pst_GFX = pst_NextGFX;
				continue;
			}
		}

		if((pst_GFX->c_Flags & GFX_Active) && !(pst_GFX->c_Flags & GFX_Terminated))
		{
#ifdef ACTIVE_EDITORS
            if ( (GDI_gpst_CurDD->ul_GFXRenderMask & (1 << pst_GFX->c_Type)) == 0 )
            {
                pst_GFX = pst_NextGFX;
                continue;
            }

#endif
			if(GFX_gpst_GO) GFX_gpst_GO->pst_Base->pst_Visu->pst_Material = (GRO_tdst_Struct *) pst_GFX->p_Material;

			if( pst_GFX->c_Flags & GFX_Transparent)
            {
                if (pst_GFX->c_Flags & GFX_NotSorted)
                    GDI_gpst_CurDD->ul_DisplayInfo = DITransNotSorted;
                else
                    GDI_gpst_CurDD->ul_DisplayInfo = DITrans;
            }
			else
				GDI_gpst_CurDD->ul_DisplayInfo = DIOpaque;

			/*$off*/
			GFX_gpst_Current = pst_GFX;
			switch(pst_GFX->c_Type)
			{

			case GFX_Line:			GFX_Line_Render(pst_GFX->p_Data); break;
			case GFX_Lightning:		if(!GFX_i_Lightning_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Desintegrate:	if(!GFX_i_Desintegrate_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Flare:			GFX_Flare_Render(pst_GFX->p_Data); break;
			case GFX_ComplexFlare:	GFX_Flare_Render(pst_GFX->p_Data); break;
			case GFX_Smoke:			if(!GFX_i_Smoke_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Table:			if(!GFX_i_Table_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Speed:			if(!GFX_i_Speed_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Spark:			if(!GFX_i_Spark_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Water2:		if(!GFX_i_Water2_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Fade:			if(!GFX_i_Fade_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Profiler:		if(!GFX_i_Profiler_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Explode:		if(!GFX_i_Explode_Render(pst_GFX->p_Data,(GRO_tdst_Struct *) pst_GFX->p_Material)) 
                                        pst_GFX->c_Flags |= GFX_Terminated;
                                    GFX_SetRenderState(_c_Sorted);
                                    break;
			case GFX_BorderFade:	if(!GFX_i_BorderFade_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Diaphragm:		if(!GFX_i_Diaphragm_Render(pst_GFX->p_Data)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Carte:			if(!GFX_i_Carte_Render(pst_GFX->p_Data, pst_GFX->p_Material, (OBJ_tdst_GameObject_*)pst_GFX->p_Owner)) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Gao:
				{
#ifdef PSX2_TARGET
					extern void Gsp_Begin3DObjectWhileIn2D( void );
					extern void Gsp_End3DObjectWhileIn2D( void );
					if (_c_Sorted)
					{
						pst_GO = ((GFX_tdst_Gao *) pst_GFX->p_Data)->pst_GO;
						if ( !pst_GO || !pst_GO->pst_Base || ! pst_GO->pst_Base->pst_Visu ) break;
						if ((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims) || (pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type != GRO_2DText) )
						{
							Gsp_Begin3DObjectWhileIn2D();
							GFX_Gao_Render(pst_GFX->p_Data);
							Gsp_End3DObjectWhileIn2D();
							goto GFXRender_endofgao;
						}
					}
#endif

					GFX_Gao_Render(pst_GFX->p_Data);

#ifdef PSX2_TARGET
GFXRender_endofgao:
#endif
					GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
					GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
					break;
				}
            case GFX_Sun:           GFX_Sun_Render( pst_GFX->p_Data ); break;
            case GFX_String:        if (!GFX_i_Str_Render( pst_GFX->p_Data )) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_Ripple:        if (!GFX_i_Ripple_Render( pst_GFX->p_Data, (OBJ_tdst_GameObject_*)pst_GFX->p_Owner )) pst_GFX->c_Flags |= GFX_Terminated; break;
			case GFX_AnotherFlare:  if (!GFX_i_AnotherFlare_Render( pst_GFX->p_Data )) pst_GFX->c_Flags |= GFX_Terminated; break;
            case GFX_Tetris:        GFX_Tetris_Render(pst_GFX->p_Data); break;
			}
			/*$on*/
            PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_NbGFXrendered, 1);
		}

		pst_GFX = pst_NextGFX;
	}

	GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
	GDI_gpst_CurDD->ul_DisplayInfo = DI;
	GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix = uw_SaveCurMatrix;
	GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix = GDI_gpst_CurDD->st_MatrixStack.dst_Matrix + uw_SaveCurMatrix;
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
	
    if (_c_Sorted )
        SOFT_ZList_Unlock();

    //FLARE_M_UpdateCounter();
    
	_GSP_EndRaster(7);
	
    PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GFX);
}

#ifdef USE_DOUBLE_RENDERING	
extern void  GFX_Line_Interpolate( void *p_Data , u_int Mode , float fInterpoler);
void GFX_Interpolate_All(GFX_tdst_List **_ppst_GFX, u_int Mode , float fInterpoler )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_List	*pst_GFX;
	GFX_tdst_List	*pst_NextGFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GFX = *_ppst_GFX;
	if(!pst_GFX) return;
	while(pst_GFX)
	{
        PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_NbGFX, 1);
		if((pst_GFX->c_Flags & GFX_Active) && !(pst_GFX->c_Flags & GFX_Terminated))
		{

			switch(pst_GFX->c_Type)
			{
			case GFX_AnotherFlare: 	GFX_AnotherFlare_Interpolate( pst_GFX->p_Data , Mode , fInterpoler); break;
			case GFX_Sun: 			GFX_Sun_Interpolate( pst_GFX->p_Data , Mode , fInterpoler); break;
			case GFX_Profiler: 		GFX_Profiler_Interpolate( pst_GFX->p_Data , Mode , fInterpoler); break;
			case GFX_Line: 			GFX_Line_Interpolate( pst_GFX->p_Data , Mode , fInterpoler); break;
			
			}
		}
		pst_GFX = pst_GFX->pst_Next;
	}
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if 0
void GFX_PreRender(GFX_tdst_List **_ppst_GFX)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned short	uw_SaveCurMatrix;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GFX = *_ppst_GFX;
	if(!pst_GFX) return;

	uw_SaveCurMatrix = GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix;
	GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix = 0;
	GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix = GDI_gpst_CurDD->st_MatrixStack.dst_Matrix;
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
	

	while(pst_GFX)
	{
		if((pst_GFX->c_Flags & GFX_Active) && !(pst_GFX->c_Flags & GFX_Terminated))
		{
			switch(pst_GFX->c_Type)
			{
			case GFX_Flare:
			case GFX_ComplexFlare:
				GFX_Flare_PreRender(pst_GFX->p_Data);
				break;
			}
		}

		pst_GFX = pst_GFX->pst_Next;
	}

	GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix = uw_SaveCurMatrix;
	GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix = GDI_gpst_CurDD->st_MatrixStack.dst_Matrix + uw_SaveCurMatrix;
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Test(GFX_tdst_List *_pst_GFX)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned short	uw_SaveCurMatrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GFX) return;

	uw_SaveCurMatrix = GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix;
	GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix = 0;
	GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix = GDI_gpst_CurDD->st_MatrixStack.dst_Matrix;
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;

    GFX_gpst_Current = _pst_GFX;

	while(GFX_gpst_Current)
	{
		if((GFX_gpst_Current->c_Flags & GFX_Active) && !(GFX_gpst_Current->c_Flags & GFX_Terminated))
		{
			switch(GFX_gpst_Current->c_Type)
			{
			case GFX_Flare:
			case GFX_ComplexFlare:
				GFX_Flare_Test(GFX_gpst_Current->p_Data);
				break;
			}
		}

		GFX_gpst_Current = GFX_gpst_Current->pst_Next;
	}

	GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix = uw_SaveCurMatrix;
	GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix = GDI_gpst_CurDD->st_MatrixStack.dst_Matrix + uw_SaveCurMatrix;
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Add(GFX_tdst_List **_ppst_GFX, char _c_Type, void *_p_Owner)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	void			*p_Data;
	GFX_tdst_List	*pst_NewGFX;
	char			c_Flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!GFX_gpst_GO) GFX_NeedGeom(4, 4, 2, TRUE);

	c_Flags = 0;

	switch(_c_Type)
	{
	case GFX_Line:			p_Data = GFX_Line_Create(); break;
	case GFX_Lightning:		p_Data = GFX_Lightning_Create(); break;
	case GFX_Desintegrate:	p_Data = GFX_Desintegrate_Create(_p_Owner); c_Flags = GFX_AutoDestruct; break;
	case GFX_Flare:			p_Data = GFX_Flare_Create(0); break;
	case GFX_ComplexFlare:	p_Data = GFX_Flare_Create(1); break;
	case GFX_Smoke:			p_Data = GFX_Smoke_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Table:			p_Data = GFX_Table_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Speed:			p_Data = GFX_Speed_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Spark:			p_Data = GFX_Spark_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Water2:		p_Data = GFX_Water2_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Fade:			p_Data = GFX_Fade_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Profiler:		p_Data = GFX_Profiler_Create(); break;
	case GFX_Explode:		p_Data = GFX_Explode_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_BorderFade:	p_Data = GFX_BorderFade_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Diaphragm:		p_Data = GFX_Diaphragm_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Carte:			p_Data = GFX_Carte_Create(); break;
	case GFX_Gao:		    p_Data = GFX_Gao_Create((OBJ_tdst_GameObject_*)_p_Owner); c_Flags = GFX_Active; break;
	case GFX_Sun:		    p_Data = GFX_Sun_Create(); c_Flags = GFX_Active; break;
    case GFX_String:        p_Data = GFX_Str_Create((OBJ_tdst_GameObject_*)_p_Owner); c_Flags = GFX_Active | GFX_AutoDestruct; break;
    case GFX_Ripple:        p_Data = GFX_Ripple_Create((OBJ_tdst_GameObject_*)_p_Owner); c_Flags = GFX_Transparent | GFX_Active | GFX_AutoDestruct; break;
    case GFX_AnotherFlare:  p_Data = GFX_AnotherFlare_Create( (OBJ_tdst_GameObject_*)_p_Owner ); c_Flags = GFX_Transparent | GFX_Active | GFX_AutoDestruct; break;
	case GFX_Tetris:        p_Data = GFX_Tetris_Create(); c_Flags = GFX_Active; break;
    default:                p_Data = NULL;
	}

	if(!p_Data) return -1;

    if (GFX_sguw_Id < GFX_FirstNotSorted)
        GFX_sguw_Id = GFX_FirstNotSorted;

	pst_NewGFX = (GFX_tdst_List *) MEM_p_Alloc(sizeof(GFX_tdst_List));
	pst_NewGFX->pst_Next = *_ppst_GFX;
	pst_NewGFX->c_Type = _c_Type;
	pst_NewGFX->c_Flags = c_Flags;
	pst_NewGFX->p_Data = p_Data;
	pst_NewGFX->p_Owner = _p_Owner;
	pst_NewGFX->p_Material = NULL;
	pst_NewGFX->uw_Id = GFX_sguw_Id++;
	pst_NewGFX->f_LifeTime = Cf_Infinit;

	*_ppst_GFX = GFX_gpst_Current = pst_NewGFX;
    return pst_NewGFX->uw_Id;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_AddSorted(GFX_tdst_List **_ppst_GFX, char _c_Type, USHORT _uw_Order, void *_p_Owner)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	void			*p_Data;
	GFX_tdst_List	*pst_NewGFX, *pst_Prev, *pst_Next;
	char			c_Flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!GFX_gpst_GO) GFX_NeedGeom(4, 4, 2, TRUE);
    
    if (_uw_Order >= GFX_FirstNotSorted) return -1;

    /* search if order is not already used */
    pst_Prev = NULL;
    pst_Next = *_ppst_GFX;
    while (pst_Next != NULL)
    {
        if (pst_Next->uw_Id < _uw_Order) 
            break;
        else if (pst_Next->uw_Id == _uw_Order) 
            return -1;
        pst_Prev = pst_Next;
        pst_Next = pst_Next->pst_Next;
    }

    /* create data */
	p_Data = NULL;
	c_Flags = 0;

	switch(_c_Type)
	{
	case GFX_Line:			p_Data = GFX_Line_Create(); break;
	case GFX_Lightning:		p_Data = GFX_Lightning_Create(); break;
	case GFX_Desintegrate:	p_Data = GFX_Desintegrate_Create(_p_Owner); c_Flags = GFX_AutoDestruct; break;
	case GFX_Flare:			p_Data = GFX_Flare_Create(0); break;
	case GFX_ComplexFlare:	p_Data = GFX_Flare_Create(1); break;
	case GFX_Smoke:			p_Data = GFX_Smoke_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Table:			p_Data = GFX_Table_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Speed:			p_Data = GFX_Speed_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Spark:			p_Data = GFX_Spark_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Water2:		p_Data = GFX_Water2_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Fade:			p_Data = GFX_Fade_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Profiler:		p_Data = GFX_Profiler_Create(); break;
	case GFX_Explode:		p_Data = GFX_Explode_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_BorderFade:	p_Data = GFX_BorderFade_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Diaphragm:		p_Data = GFX_Diaphragm_Create(); c_Flags = GFX_AutoDestruct; break;
	case GFX_Carte:			p_Data = GFX_Carte_Create(); break;
	case GFX_Gao:		    p_Data = GFX_Gao_Create( (OBJ_tdst_GameObject_*)_p_Owner ); c_Flags = GFX_Active; break;
	case GFX_Sun:		    p_Data = GFX_Sun_Create(); c_Flags = GFX_Active; break;
	case GFX_String:		p_Data = GFX_Str_Create( (OBJ_tdst_GameObject_*)_p_Owner ); c_Flags = GFX_Active | GFX_AutoDestruct; break;
    case GFX_Ripple:        p_Data = GFX_Ripple_Create((OBJ_tdst_GameObject_*)_p_Owner); c_Flags = GFX_Transparent | GFX_Active | GFX_AutoDestruct; break;
    case GFX_AnotherFlare:  p_Data = GFX_AnotherFlare_Create( (OBJ_tdst_GameObject_*)_p_Owner ); c_Flags = GFX_Transparent | GFX_Active | GFX_AutoDestruct; break;
	case GFX_Tetris:        p_Data = GFX_Tetris_Create(); c_Flags = GFX_Active; break;
	}

	if(!p_Data) return -1;

	pst_NewGFX = (GFX_tdst_List *) MEM_p_Alloc(sizeof(GFX_tdst_List));
	pst_NewGFX->pst_Next = pst_Next;
	pst_NewGFX->c_Type = _c_Type;
	pst_NewGFX->c_Flags = c_Flags;
	pst_NewGFX->p_Data = p_Data;
	pst_NewGFX->p_Owner = _p_Owner;
	pst_NewGFX->p_Material = NULL;
	pst_NewGFX->uw_Id = _uw_Order;
	pst_NewGFX->f_LifeTime = Cf_Infinit;
    GFX_gpst_Current = pst_NewGFX;

    if (pst_Prev == NULL)
	    *_ppst_GFX = pst_NewGFX;
    else
        pst_Prev->pst_Next = pst_NewGFX;
    
    return _uw_Order;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Stop(GFX_tdst_List *_pst_GFX, USHORT _uw_Id)
{
	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return;
	GFX_gpst_Current->c_Flags &= ~GFX_Active;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Start(GFX_tdst_List *_pst_GFX, USHORT _uw_Id)
{
	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return;
	GFX_gpst_Current->c_Flags |= GFX_Active;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_FlagSet(GFX_tdst_List *_pst_GFX, USHORT _uw_Id, int _i_Flag, int _i_Value)
{
	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return;

	_i_Flag = 1 << _i_Flag;
	if(_i_Value)
		GFX_gpst_Current->c_Flags |= _i_Flag;
	else
		GFX_gpst_Current->c_Flags &= ~_i_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_SetMaterial(GFX_tdst_List *_pst_GFX, USHORT _uw_Id, void *p_Mat)
{
	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return;
	GFX_gpst_Current->p_Material = p_Mat;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_NeedGeom(ULONG _ul_NbPoints, ULONG _ul_NbUVs, ULONG _ul_NbTriangles, BOOL _b_RLI)
{
	GFX_NeedGeomEx(_ul_NbPoints, _ul_NbUVs, 1, _ul_NbTriangles, _b_RLI);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef _GAMECUBE

#define GFX_ALIGNVERTEX 32
#define GFX_ALIGNUV     32

#else

#define GFX_ALIGNVERTEX 16
#define GFX_ALIGNUV     8

#endif

extern BOOL GEO_b_IsInFix;

void GFX_NeedGeomEx(ULONG _ul_NbPoints, ULONG _ul_NbUVs, ULONG _ul_NbElements, ULONG _ul_NbTriangles, BOOL _b_RLI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	int i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef _DEBUGGFXGEOM
    GFX_ul_AskedPoints = _ul_NbPoints;
    GFX_ul_AskedUVs = _ul_NbUVs;
    GFX_ul_AskedElements = _ul_NbElements;
    L_memset( GFX_aul_AskedTriangles, 0, sizeof( GFX_aul_AskedTriangles ) );
    GFX_aul_AskedTriangles[0] = _ul_NbTriangles;
    GFX_ul_AskedElemSprites = 0;
    GFX_ul_AskefRLI = _b_RLI;
#endif

	if(!_ul_NbPoints) return;
	if(_ul_NbPoints > SOFT_Cul_ComputingBufferSize) 
	{
#ifdef ACTIVE_EDITORS
        ERR_X_Warning(0, "The GFX size exceeds the RAM capacity", NULL);
#endif	
	    return;
	}

	if(_b_RLI)
	{
		if(GFX_ul_MaxRLI < _ul_NbPoints)
		{
			if(GFX_pul_RLI)
				GFX_pul_RLI = (ULONG *) MEM_p_Realloc(GFX_pul_RLI, (_ul_NbPoints + 1) * 4);
			else
				GFX_pul_RLI = (ULONG *) MEM_p_Alloc((_ul_NbPoints + 1) * 4);
			GFX_ul_MaxRLI = _ul_NbPoints;
		}

		*GFX_pul_RLI = _ul_NbPoints;
	}		


	if(GFX_gpst_GO == NULL)
	{
		for(i = 0; i < 64; i++) GFX_aul_MaxTriangles[i] = 0;
		

		GEO_b_IsInFix = TRUE;
		GFX_gpst_Geo = GEO_pst_Create(_ul_NbPoints, _ul_NbUVs, _ul_NbElements, 0);
		GEO_b_IsInFix = FALSE;

		pst_Element = GFX_gpst_Geo->dst_Element;
		pst_Element->l_NbTriangles = _ul_NbTriangles;
		pst_Element->p_MrmElementAdditionalInfo = NULL;
		pst_Element->pst_StripData = NULL;
		pst_Element->ul_NumberOfUsedIndex = 0;
		GEO_AllocElementContent(pst_Element);

		GFX_gpst_GO = OBJ_GameObject_Create(OBJ_C_IdentityFlag_BaseObject | OBJ_C_IdentityFlag_Visu);
		GFX_gpst_GO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) GFX_gpst_Geo;
		GDI_gpst_CurDD->pst_CurrentGameObject = GFX_gpst_GO;

		GFX_ul_MaxElements = _ul_NbElements;
        GFX_ul_MaxElemSprites = 0;
		GFX_ul_MaxPoints = _ul_NbPoints;
		GFX_ul_MaxUVs = _ul_NbUVs;
		GFX_aul_MaxTriangles[0] = _ul_NbTriangles;

		if(_b_RLI) GFX_gpst_Geo->dul_PointColors = GFX_pul_RLI;
		return;
	}

    GFX_gpst_Geo->l_NbSpritesElements = 0;

	if(_b_RLI)
		GFX_gpst_Geo->dul_PointColors = GFX_pul_RLI;
	else
		GFX_gpst_Geo->dul_PointColors = NULL;

	if(_ul_NbPoints > GFX_ul_MaxPoints)
	{
		MEM_FreeAlign(GFX_gpst_Geo->dst_Point);
		GFX_gpst_Geo->dst_Point = (GEO_Vertex *) MEM_p_AllocAlign(sizeof(GEO_Vertex) * _ul_NbPoints, GFX_ALIGNVERTEX);
		MEM_FreeAlign(GFX_gpst_Geo->dst_PointNormal);
		GFX_gpst_Geo->dst_PointNormal = (MATH_tdst_Vector *) MEM_p_AllocAlign(sizeof(MATH_tdst_Vector) * _ul_NbPoints, GFX_ALIGNVERTEX);
		GFX_ul_MaxPoints = _ul_NbPoints;
	}

	GFX_gpst_Geo->l_NbPoints = _ul_NbPoints;

	if(_ul_NbUVs > GFX_ul_MaxUVs)
	{
		MEM_FreeAlign(GFX_gpst_Geo->dst_UV);
		GFX_gpst_Geo->dst_UV = (GEO_tdst_UV *) MEM_p_AllocAlign(sizeof(GEO_tdst_UV) * _ul_NbUVs, GFX_ALIGNUV);
		GFX_ul_MaxUVs = _ul_NbUVs;
	}

	GFX_gpst_Geo->l_NbUVs = _ul_NbUVs;

	if(_ul_NbTriangles > GFX_aul_MaxTriangles[0])
	{
		MEM_FreeAlign(GFX_gpst_Geo->dst_Element->dst_Triangle);
		GFX_gpst_Geo->dst_Element->dst_Triangle = (GEO_tdst_IndexedTriangle *) MEM_p_AllocAlign(sizeof(GEO_tdst_IndexedTriangle) * _ul_NbTriangles, 16);
#ifdef ACTIVE_EDITORS
		L_memset(GFX_gpst_Geo->dst_Element->dst_Triangle, 0, sizeof(GEO_tdst_IndexedTriangle) * _ul_NbTriangles);
#endif
		GFX_aul_MaxTriangles[0] = _ul_NbTriangles;
	}

	GFX_gpst_Geo->dst_Element->l_NbTriangles = _ul_NbTriangles;

	if(_ul_NbElements > GFX_ul_MaxElements)
	{
        if (GFX_ul_MaxElements)
        {
			GFX_gpst_Geo->dst_Element = (GEO_tdst_ElementIndexedTriangles *) MEM_p_Realloc
			(
				GFX_gpst_Geo->dst_Element,
				sizeof(GEO_tdst_ElementIndexedTriangles) * _ul_NbElements
			);
			L_memset(GFX_gpst_Geo->dst_Element + GFX_ul_MaxElements, 0, sizeof(GEO_tdst_ElementIndexedTriangles) * (_ul_NbElements - GFX_ul_MaxElements));
        }
        else
        {
            GFX_gpst_Geo->dst_Element = (GEO_tdst_ElementIndexedTriangles *) MEM_p_Alloc(sizeof(GEO_tdst_ElementIndexedTriangles) * _ul_NbElements);
			L_memset(GFX_gpst_Geo->dst_Element , 0, sizeof(GEO_tdst_ElementIndexedTriangles) * _ul_NbElements);
        }
		GFX_ul_MaxElements = _ul_NbElements;
	}

	GFX_gpst_Geo->l_NbElements = _ul_NbElements;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_NeedGeomExElem(ULONG _ul_Element, ULONG _ul_NbTriangles)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef _DEBUGGFXGEOM
    GFX_aul_AskedTriangles[ _ul_Element ] = _ul_NbTriangles;
#endif

	if(_ul_Element >= (ULONG) GFX_gpst_Geo->l_NbElements) return;

	pst_Element = GFX_gpst_Geo->dst_Element + _ul_Element;

	if(_ul_NbTriangles > GFX_aul_MaxTriangles[_ul_Element])
	{
		if(GFX_aul_MaxTriangles[_ul_Element])
		{
			pst_Element->dst_Triangle = (GEO_tdst_IndexedTriangle *) MEM_p_VMReallocAlign
				(
					pst_Element->dst_Triangle,
					_ul_NbTriangles * sizeof(GEO_tdst_IndexedTriangle), 16
				);
#ifdef ACTIVE_EDITORS
			{
				ULONG	i;
				for(i = GFX_aul_MaxTriangles[_ul_Element]; i < _ul_NbTriangles; i++)
					pst_Element->dst_Triangle[i].ul_MaxFlags = 0;
			}
#endif
		}
		else
		{
			pst_Element->l_NbTriangles = _ul_NbTriangles;
			pst_Element->p_MrmElementAdditionalInfo = NULL;
			pst_Element->pst_StripData = NULL;			
			pst_Element->ul_NumberOfUsedIndex = 0;
			GEO_AllocElementContent(pst_Element);
		}

		GFX_aul_MaxTriangles[_ul_Element] = _ul_NbTriangles;
	}

	pst_Element->l_NbTriangles = _ul_NbTriangles;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_NeedGeomNbElemSprites( ULONG _ul_Nb )
{
    ULONG ul_Size;
    
#ifdef _DEBUGGFXGEOM
    GFX_ul_AskedElemSprites = _ul_Nb;
    L_memset( GFX_aul_AskedSprites, 0, sizeof( GFX_aul_AskedSprites ) );
#endif

    GFX_gpst_Geo->l_NbSpritesElements = _ul_Nb;
    if (_ul_Nb < GFX_ul_MaxElemSprites)
        return;

    ul_Size = sizeof(GEO_tdst_ElementIndexedSprite) * _ul_Nb;
    if (GFX_ul_MaxElemSprites)
        GFX_gpst_Geo->dst_SpritesElements = (GEO_tdst_ElementIndexedSprite *) MEM_p_Realloc( GFX_gpst_Geo->dst_SpritesElements, ul_Size );
    else
        GFX_gpst_Geo->dst_SpritesElements = (GEO_tdst_ElementIndexedSprite *) MEM_p_Alloc( ul_Size );
    GFX_ul_MaxElemSprites = _ul_Nb;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_NeedGeomElemNbSprites( ULONG _ul_Elem, ULONG _ul_Nb )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedSprite	*pst_Element;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef _DEBUGGFXGEOM
    GFX_aul_AskedSprites[ _ul_Elem ] = _ul_Nb;
#endif

	if(_ul_Elem >= (ULONG) GFX_gpst_Geo->l_NbSpritesElements) return;

	pst_Element = GFX_gpst_Geo->dst_SpritesElements + _ul_Elem;

	if(_ul_Nb > GFX_aul_MaxTriangles[_ul_Elem])
	{
		if(GFX_aul_MaxTriangles[_ul_Elem])
		{
            pst_Element->dst_Sprite = (GEO_tdst_IndexedSprite*)MEM_p_Realloc(pst_Element->dst_Sprite, sizeof(GEO_tdst_IndexedSprite) * _ul_Nb );
		}
		else
		{
            pst_Element->dst_Sprite = (GEO_tdst_IndexedSprite*)MEM_p_Alloc(sizeof(GEO_tdst_IndexedSprite) * _ul_Nb );
		}
		GFX_aul_MaxSprites[_ul_Elem] = _ul_Nb;
	}
	pst_Element->l_NbSprites = _ul_Nb;
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef _DEBUGGFXGEOM
void GFX_CheckGeom()
{
    int i;

    if (GFX_gpst_Geo->l_NbPoints > (LONG) GFX_ul_AskedPoints )
        _breakpoint_;
    if (GFX_gpst_Geo->l_NbUVs > (LONG) GFX_ul_AskedUVs )
        _breakpoint_;
    if (GFX_gpst_Geo->l_NbElements > (LONG) GFX_ul_AskedElements )
        _breakpoint_;

    for (i = 0; i < GFX_gpst_Geo->l_NbElements; i++)
    {
        if (GFX_gpst_Geo->dst_Element[ i ].l_NbTriangles > (LONG) GFX_aul_AskedTriangles[ i ] )
            _breakpoint_;
    }
    
    if (GFX_gpst_Geo->l_NbSpritesElements > (LONG) GFX_ul_AskedElemSprites )
        _breakpoint_;
    for (i = 0; i < GFX_gpst_Geo->l_NbSpritesElements; i++)
    {
        if (GFX_gpst_Geo->dst_SpritesElements[i].l_NbSprites > (LONG) GFX_aul_AskedSprites[i] )
            _breakpoint_;
    }

    if ( !GFX_ul_AskefRLI && GFX_gpst_Geo->dul_PointColors)
        _breakpoint_;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Request(int a, int b, int c, int d)
{
	if(a >= 1000 && a < 2000) return GFX_i_Line_Request(a, b, c, d);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Geti(GFX_tdst_List *_pst_GFX, USHORT _uw_Id, int _i_Param)
{
	/*~~~~~~~~~~~~~*/
	short	w_Offset;
	/*~~~~~~~~~~~~~*/

	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return 0;

	if
	(
		(_i_Param < ga_Parami_First[GFX_gpst_Current->c_Type])
	||	(_i_Param >= (ga_Parami_First[GFX_gpst_Current->c_Type] + ga_Parami_Number[GFX_gpst_Current->c_Type]))
	) return 0;

	w_Offset = gaa_Parami_Offset[GFX_gpst_Current->c_Type][_i_Param - ga_Parami_First[GFX_gpst_Current->c_Type]];

	if(!(w_Offset & GFX_Special))
	{
		if(w_Offset & GFX_OneByteInteger) return *(((char *) GFX_gpst_Current->p_Data) + (w_Offset & 0xFFF));
		if(w_Offset & GFX_TwoByteInteger)
			return *(short *) (((char *) GFX_gpst_Current->p_Data) + (w_Offset & 0xFFF));

		return *(int *) (((char *) GFX_gpst_Current->p_Data) + w_Offset);
	}

    w_Offset &= ~GFX_Special;

    if(GFX_gpst_Current->c_Type == GFX_Explode) 
        return GFX_i_Explode_Geti(GFX_gpst_Current->p_Data, _i_Param );

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Seti(GFX_tdst_List *_pst_GFX, USHORT _uw_Id, int _i_Param, int _i_Value)
{
	/*~~~~~~~~~~~~~*/
	short	w_Offset;
	/*~~~~~~~~~~~~~*/

	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return;

	if
	(
		(_i_Param < ga_Parami_First[GFX_gpst_Current->c_Type])
	||	(_i_Param >= (ga_Parami_First[GFX_gpst_Current->c_Type] + ga_Parami_Number[GFX_gpst_Current->c_Type]))
	) return;

	w_Offset = gaa_Parami_Offset[GFX_gpst_Current->c_Type][_i_Param - ga_Parami_First[GFX_gpst_Current->c_Type]];

	if(!(w_Offset & GFX_Special))
	{
		if(w_Offset & GFX_OneByteInteger)
			*(((char *) GFX_gpst_Current->p_Data) + (w_Offset & 0xFFF)) = (char) _i_Value;
		else if(w_Offset & GFX_TwoByteInteger)
			*(short *) (((char *) GFX_gpst_Current->p_Data) + (w_Offset & 0xFFF)) = (short) _i_Value;
		else
			*(int *) (((char *) GFX_gpst_Current->p_Data) + w_Offset) = _i_Value;
		return;
	}

	w_Offset &= ~GFX_Special;
    if(GFX_gpst_Current->c_Type == GFX_Smoke)
        GFX_Smoke_Seti( GFX_gpst_Current->p_Data, _i_Param, _i_Value);
	else if(GFX_gpst_Current->c_Type == GFX_Water2)
		GFX_Water2_Seti(GFX_gpst_Current->p_Data, _i_Param, w_Offset, _i_Value);
    else if(GFX_gpst_Current->c_Type == GFX_Profiler) 
        GFX_Profiler_Seti(GFX_gpst_Current->p_Data, _i_Param, _i_Value);
	else if(GFX_gpst_Current->c_Type == GFX_Explode) 
        GFX_Explode_Seti(GFX_gpst_Current->p_Data, _i_Param, _i_Value);
    else if(GFX_gpst_Current->c_Type == GFX_Sun)
        GFX_Sun_Seti( GFX_gpst_Current->p_Data, _i_Param, _i_Value);
    else if(GFX_gpst_Current->c_Type == GFX_Carte)
        GFX_Carte_Seti( GFX_gpst_Current->p_Data, _i_Param, _i_Value);
    else if (GFX_gpst_Current->c_Type == GFX_String)
        GFX_Str_Seti( GFX_gpst_Current->p_Data, _i_Param, _i_Value);
    else if(GFX_gpst_Current->c_Type == GFX_Ripple)
        GFX_Ripple_Seti( GFX_gpst_Current->p_Data, _i_Param, _i_Value);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float GFX_f_Getf(GFX_tdst_List *_pst_GFX, USHORT _uw_Id, int _i_Param)
{
	/*~~~~~~~~~~~~~*/
	short	w_Offset;
	/*~~~~~~~~~~~~~*/

	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return 0;

	if
	(
		(_i_Param < ga_Paramf_First[GFX_gpst_Current->c_Type])
	||	(_i_Param >= (ga_Paramf_First[GFX_gpst_Current->c_Type] + ga_Paramf_Number[GFX_gpst_Current->c_Type]))
	) return 0;

	w_Offset = gaa_Paramf_Offset[GFX_gpst_Current->c_Type][_i_Param - ga_Paramf_First[GFX_gpst_Current->c_Type]];
	if(!(w_Offset & GFX_Special)) return *(float *) (((char *) GFX_gpst_Current->p_Data) + w_Offset);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Setf(GFX_tdst_List *_pst_GFX, USHORT _uw_Id, int _i_Param, float _f_Value)
{
	/*~~~~~~~~~~~~~*/
	short	w_Offset;
	/*~~~~~~~~~~~~~*/

	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return;

	if
	(
		(_i_Param < ga_Paramf_First[GFX_gpst_Current->c_Type])
	||	(_i_Param >= (ga_Paramf_First[GFX_gpst_Current->c_Type] + ga_Paramf_Number[GFX_gpst_Current->c_Type]))
	) return;

	w_Offset = gaa_Paramf_Offset[GFX_gpst_Current->c_Type][_i_Param - ga_Paramf_First[GFX_gpst_Current->c_Type]];
	if(!(w_Offset & GFX_Special))
	{
		*(float *) (((char *) GFX_gpst_Current->p_Data) + w_Offset) = _f_Value;
		return;
	}

	w_Offset &= ~GFX_Special;
	if(GFX_gpst_Current->c_Type == GFX_Water2)
		GFX_Water2_Setf(GFX_gpst_Current->p_Data, _i_Param, w_Offset, _f_Value);
	else if (GFX_gpst_Current->c_Type == GFX_Profiler) 
        GFX_Profiler_Setf(GFX_gpst_Current->p_Data, _i_Param, _f_Value);
	else if (GFX_gpst_Current->c_Type == GFX_Explode) 
        GFX_Explode_Setf(GFX_gpst_Current->p_Data, _i_Param, _f_Value);
    else if (GFX_gpst_Current->c_Type == GFX_Ripple) 
        GFX_Ripple_Setf( GFX_gpst_Current->p_Data, _i_Param, _f_Value);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Vector *GFX_pst_Getv(GFX_tdst_List *_pst_GFX, USHORT _uw_Id, int _i_Param)
{
	/*~~~~~~~~~~~~~*/
	short	w_Offset;
	/*~~~~~~~~~~~~~*/

	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return 0;

	if
	(
		(_i_Param < ga_Paramv_First[GFX_gpst_Current->c_Type])
	||	(_i_Param >= (ga_Paramv_First[GFX_gpst_Current->c_Type] + ga_Paramv_Number[GFX_gpst_Current->c_Type]))
	) return 0;

	w_Offset = gaa_Paramv_Offset[GFX_gpst_Current->c_Type][_i_Param - ga_Paramv_First[GFX_gpst_Current->c_Type]];
	if(!(w_Offset & GFX_Special)) 
        return (MATH_tdst_Vector *) (((char *) GFX_gpst_Current->p_Data) + w_Offset);
        
    w_Offset &= ~GFX_Special;
	if (GFX_gpst_Current->c_Type == GFX_Profiler) 
        return GFX_pst_Profiler_Getv(GFX_gpst_Current->p_Data, _i_Param );

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Setv(GFX_tdst_List *_pst_GFX, USHORT _uw_Id, int _i_Param, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*dst;
	short				w_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return;

	if
	(
		(_i_Param < ga_Paramv_First[GFX_gpst_Current->c_Type])
	||	(_i_Param >= (ga_Paramv_First[GFX_gpst_Current->c_Type] + ga_Paramv_Number[GFX_gpst_Current->c_Type]))
	) return;

	w_Offset = gaa_Paramv_Offset[GFX_gpst_Current->c_Type][_i_Param - ga_Paramv_First[GFX_gpst_Current->c_Type]];
	if(!(w_Offset & GFX_Special))
	{
		dst = (MATH_tdst_Vector *) (((char *) GFX_gpst_Current->p_Data) + w_Offset);
		MATH_CopyVector(dst, v);
		return;
	}

	w_Offset &= ~GFX_Special;
	if(GFX_gpst_Current->c_Type == GFX_Water2) 
        GFX_Water2_Setv(GFX_gpst_Current->p_Data, _i_Param, w_Offset, v);
	else if(GFX_gpst_Current->c_Type == GFX_Profiler) 
        GFX_Profiler_Setv(GFX_gpst_Current->p_Data, _i_Param, v);
	else if(GFX_gpst_Current->c_Type == GFX_Explode) 
        GFX_Explode_Setv(GFX_gpst_Current->p_Data, _i_Param, v);
    else if (GFX_gpst_Current->c_Type == GFX_Table) 
        GFX_Table_Setv(GFX_gpst_Current->p_Data, _i_Param, v);
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

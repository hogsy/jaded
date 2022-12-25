/**
 * This file defines all externs flags driven by the cheat menu.
 *
 * @author Massimiliano Pagani
 * @date 2002 Oct 7
 */

#if !defined( Gx8_CheatFlags_h )
#  define Gx8_CheatFlags_h

#include "BASe/BAStypes.h"

// force menu et rasters.

#define GX8_BENCH
extern float g_fFramePerSecond;
extern u_int32 RasterFilter;
extern u_int32 RasterMode;
extern u_int32 g_iNoUV;
extern u_int32 g_iNoGEODRAW;
extern u_int32 g_iNoMATDRAW;
extern u_int32 g_iNoMulti;
extern u_int32 g_iNoSTR;
extern u_int32 g_iNoMDF;
extern u_int32 g_iNoSPR;
extern u_int32 g_iNoGFX;
extern u_int32 g_iNoZLST;
extern u_int32 g_iNoSKN;
extern u_int32 g_iNoSDW;
extern u_int32 g_iNoFog;
extern u_int32 g_iAA;
extern int g_iNumShadow;
extern u_int32 g_iDDShadow;
extern bool g_bUseAmbience;
extern bool g_bUseDialogs;
extern bool g_bUseStream;
extern float g_MipMapLODBias;
extern float g_FrameRateAlarmLimit;
extern int g_FrameRateNumber;
#if defined( FAST_CAP )
extern bool g_FastCapEnabled;
#endif
#endif

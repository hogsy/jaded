#ifndef __BENCH_H__
#define __BENCH_H__

#ifdef PSX2_TARGET
#include "GS_PS2/Gsp_Bench.h"
#define _GSP_BeginRaster(a)	GSP_BeginRaster(a)
#define _GSP_EndRaster(a)	GSP_EndRaster(a)

#elif defined(_GAMECUBE)
#include "GXI_GC/GXI_Bench.h"
#define _GSP_BeginRaster(a)	GXI_BeginRaster(a)
#define _GSP_EndRaster(a)	GXI_EndRaster(a)

#elif defined (_XENON)
#pragma message("Using XENON Bench profiling")
#include "XenonGraphics/DevHelpers/XeBench.h"
#include "XenonGraphics/DevHelpers/XePerf.h"
#define _GSP_BeginRaster(a)	XE_BeginRaster(a)
#define _GSP_EndRaster(a)	XE_EndRaster(a)

#elif defined (_XBOX)
#include "GX8/RASter/Gx8_Bench.h"
#define _GSP_BeginRaster(a)	Gx8_BeginRaster(a)
#define _GSP_EndRaster(a)	Gx8_EndRaster(a)

#elif defined (_PC_RETAIL)
#include "Dx9/Dx9bench.h"
#define _GSP_BeginRaster(a)	Dx9_BeginPerfCounter(a)
#define _GSP_EndRaster(a)	Dx9_EndPerfCounter(a)

#elif defined(_XENON_PROFILE)
#pragma message("Using XENON Bench profiling")
#include "XenonGraphics/DevHelpers/XeBench.h"
#include "XenonGraphics/DevHelpers/XePerf.h"
#define _GSP_BeginRaster(a)
#define _GSP_EndRaster(a)

#else
#define _GSP_BeginRaster(az)	
#define _GSP_EndRaster(az)		
#define PIX_XeBeginEventSpecific(a,b)
#define PIX_XeEndEventSpecific(a)
#endif

#endif
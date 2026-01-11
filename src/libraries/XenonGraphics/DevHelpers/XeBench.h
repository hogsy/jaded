#ifndef __XEBENCH_H__
#define __XEBENCH_H__

#if !defined(_FINAL_) && (defined(_XENON) || defined(_XENON_PROFILE))
	#define XE_BENCH
#endif

#define XE_StartRaster 72

#ifdef XE_BENCH

#include "Sdk\Sources\TIMer\TIMdefs.h"
#include "XenonGraphics/DevHelpers/XePerf.h"

typedef struct 
{
	char         Name[32];
	unsigned int FilterType;
} RasterDescriptor;

#if defined(_XENON)
extern int64 Rasters[];
extern LARGE_INTEGER RastersStarts[];
extern int64 RastersSmooth[];
#else
extern __int64 Rasters[];
extern __int64 RastersStarts[];
extern __int64 RastersSmooth[];
#endif
extern RasterDescriptor RasterDescriptors[];

#ifdef _DEBUG
static unsigned int iRasDbg = 200;
#endif
_inline_ void XE_BeginRaster(unsigned int Num)
{
#ifdef _DEBUG
	if(iRasDbg<Num) return;
#endif
#if defined(_XENON)
    QueryPerformanceCounter( &RastersStarts[Num] );
#else
    TIM_GetTimerInternalCounter(&RastersStarts[Num]);
#endif

#ifndef _XENON_RENDERER_USETHREAD
	PIX_XePerf_BeginEventSpecific(Num, RasterDescriptors[Num].Name);
#endif
}

_inline_ void XE_EndRaster(unsigned int Num)
{
#ifdef _DEBUG
	if(iRasDbg<Num) return;
#endif

	if (RastersStarts[Num].QuadPart>0)
    {
#if defined(_XENON)
        LARGE_INTEGER TicksPerSecond;
        QueryPerformanceCounter( &TicksPerSecond );
		Rasters[Num] += (int64)TicksPerSecond.QuadPart-(int64)RastersStarts[Num].QuadPart;
#else
        __int64 i64Res;
        TIM_GetTimerInternalCounter(&i64Res);
        Rasters[Num] += i64Res-RastersStarts[Num];
#endif
    }

	RastersStarts[Num].QuadPart = 0;

#ifndef _XENON_RENDERER_USETHREAD
	PIX_XePerf_EndEventSpecific(Num);
#endif
}

#else  //#ifdef XE_BENCH

#define XE_BeginRaster(a)
#define XE_EndRaster(a)

#endif //#ifdef XE_BENCH

#endif //#define __XEBENCH_H__
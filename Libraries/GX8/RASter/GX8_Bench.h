#ifndef GX8_BENCH_H
#define GX8_BENCH_H

#ifndef __TIMDEF_H__
#include "Sdk\Sources\TIMer\TIMdefs.h"
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#include "GX8/RASter/Gx8_CheatFlags.h"

#ifdef GX8_BENCH
extern void RAS_fnvDrawRasters(void *);

#define Gx8_RAS_fnvDrawRasters(dev) RAS_fnvDrawRasters(dev)

#define GSP_NRaster 52 // add spg2 et spg2b	
extern ULONG Rasters[GSP_NRaster];
extern ULONG RastersStarts[GSP_NRaster];
	
_inline_ void Gx8_BeginRaster(u_int Num)
{
	RastersStarts[Num] = TIM_ul_GetLowPartTimerInternalCounter();
}
_inline_ void Gx8_EndRaster(u_int Num)
{
	if (RastersStarts[Num]>0)
		Rasters[Num] += TIM_ul_GetLowPartTimerInternalCounter()-RastersStarts[Num];

	RastersStarts[Num] = 0;
}
#else  // GX8_BENCH
#define Gx8_RAS_fnvDrawRasters

#endif // GX8_BENCH

extern void Gx8_MemorySpy(void *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif// GX8_BENCH_H

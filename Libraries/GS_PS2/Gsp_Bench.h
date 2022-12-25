#ifndef GSP_BENCH_H
#define GSP_BENCH_H

#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>


#include "BASsys.h"
#include "GS_PS2/Gsp.h"

#ifdef __cplusplus
extern "C"
{
#endif


extern int Rasters[];
extern int Rasters1[];
extern int RastersStarts[];
extern int RastersStarts1[];
	
#ifdef _DEBUG
#define GSP_PS2_BENCH
#else
#define GSP_PS2_BENCH
#endif

#ifdef _FINAL_ 
#ifdef GSP_PS2_BENCH
#undef GSP_PS2_BENCH
#endif
#endif // _FINAL_

extern float    TIM_f_OoTimer1Frequency;
extern float    fOoGlobalAcc;

#ifdef GSP_PS2_BENCH

_inline_ void GSP_BeginRaster(u_int Num)
{
	RastersStarts[Num] = scePcGetCounter0() | 1;
	RastersStarts1[Num] = scePcGetCounter1();
}
_inline_ void GSP_EndRaster(u_int Num)
{
	Rasters[Num] += scePcGetCounter0() - RastersStarts[Num];
	Rasters1[Num] += scePcGetCounter1() - RastersStarts1[Num];
	RastersStarts[Num] --;
}
#else
#define GSP_BeginRaster(a)
#define GSP_EndRaster(a)
#endif



#ifdef __cplusplus
}
#endif

#endif// GSP_BENCH_H

#include "Precomp.h"

#include "XenonGraphics/DevHelpers/XeBench.h"

#if defined(XE_BENCH)

#define DEFINE_RASTER(a,b,c,d)  
#include "BAse/BENch/BENch_RasterDef.h"
#undef DEFINE_RASTER

#define Raster_FilterEngine 1
#define Raster_FilterDisplay 2
#define Raster_FilterGlobal 4
#define Raster_FilterIAUsr 8
#define Raster_FilterALL 7
#define Raster_FilterQAResume 16

#define DEFINE_RASTER(a,b,c,d)  b,d,
RasterDescriptor RasterDescriptors[XE_NRaster] =
{
#include "BAse/BENch/BENch_RasterDef.h"
};
#undef DEFINE_RASTER

//
// we have a frame = ino + eng + dis + snd + txp
// with eng = col + rec + ai + diveng
//

#if defined(_XENON)
int64 Rasters[XE_NRaster];
LARGE_INTEGER RastersStarts[XE_NRaster];
int64 RastersSmooth[XE_NRaster];
#else
__int64 Rasters[XE_NRaster];
__int64 RastersStarts[XE_NRaster];
__int64 RastersSmooth[XE_NRaster];
#endif

/*
int Rasters_Max[XE_NRaster];
int Rasters_MaxEnble = 0;

int Rasters1[XE_NRaster];
float RastersSmooth[XE_NRaster];
int RastersStarts1[XE_NRaster];
*/

#endif

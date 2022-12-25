/*$T Gx8color.c GC!  */

#if !defined( GX8_COLOR_H )
#define GX8_COLOR_H

#pragma once

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/* Aim XBox color processing to get the same color look both on PS2 and XBox */

#include "BASe/BASTypes.h"

#define Gx8_M_ConvertColor(a) (a & 0xff00ff00) | ((a & 0xff) << 16) | ((a & 0xff0000) >> 16)

ULONG
Gx8_ConvertChrominancePixel( ULONG value );

#endif
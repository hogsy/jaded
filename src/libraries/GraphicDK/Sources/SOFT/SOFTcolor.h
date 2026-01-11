/*$T SOFTcolor.h GC! 1.081 06/26/00 11:19:02 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __SOFTCOLOR_H__
#define __SOFTCOLOR_H__
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


#define RGB_INTENSITY_RED    0.30
#define RGB_INTENSITY_GREEN  0.59
#define RGB_INTENSITY_BLUE   0.11

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */
ULONG COLOR_ul_Blend(ULONG , ULONG , float );
ULONG COLOR_ul_Mul(ULONG , float );

#ifdef ACTIVE_EDITORS
ULONG COLOR_ul_Blend3(ULONG, ULONG, ULONG, float, float, float );

void COLOR_RLIAdjust_Free( void );
void COLOR_RLIAdjust_Restore( void );
void COLOR_RLIAdjust_SetAlphaParam( double a );
void COLOR_RLIAdjust_SetHLSParams( double h, double l, double s );
void COLOR_RLIAdjust_HLS( ULONG * );
void COLOR_RLIAdjust_SetHLSColorizeParams( double h, double l, double s );
void COLOR_RLIAdjust_HLSColorize( ULONG * );
void COLOR_RLIAdjust_SetBCParams( double contrast, double brightness );
void COLOR_RLIAdjust_BC( ULONG * );
void COLOR_RLIAdjust_SetRGBParams( double r, double g, double b );
void COLOR_RLIAdjust_RGBAdd( ULONG * );
void COLOR_RLIAdjust_RGBMul( ULONG * );
void RGB_to_HLS( double r, double g, double b, double *h, double *l , double *s );
void HLS_to_RGB( double h, double l, double s, double *r, double *g, double *b );
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SOFTCOLOR_H */

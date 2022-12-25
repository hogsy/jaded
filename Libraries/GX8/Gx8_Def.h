/*
 * Gx8 definitions.
 */

#if !defined( GX8_DEF_H )
#  define GX8_DEF_H

#pragma once

#if defined( __cplusplus )
extern "C"
{
#endif

typedef struct Gx8_BigStruct_ 
{
    float f2DFFX;
    float f2DFFY;
    float f2DFFX_B2D;
    float f2DFFX_A2D;
    float f2DFFY_B2D;
    float f2DFFY_A2D;
}
Gx8_BigStruct;


#if defined( __cplusplus )
}
#endif
#endif

/*
 * Gx8 definitions.
 */

#if !defined( GX8_H )
#  define GX8_H

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

    unsigned int Xsize;
    unsigned int Ysize;
}
Gx8_BigStruct;

/** Common macro for accessing the global Gx8 data
 *
 * Always access the global data through this macros.
 * If you need new fields, look to the macros below.
 */

#define  	Gx8Global_get(a) Gx8Global_get_##a()
#define     Gx8Global_set(a,v) Gx8Global_set_##a(v)

/**
 * Standard definition for get and set functions for the global data.
 */

#define Define_Gx8Global_get(T,a)               \
T Gx8Global_get_##a(void)                       \
{                                               \
    extern Gx8_BigStruct gs_st_Globals;         \
                                                \
    return gs_st_Globals.a;                     \
}

#define Define_Gx8Global_set(T,a)               \
void Gx8Global_set_##a( T v )                   \
{                                               \
    extern Gx8_BigStruct gs_st_Globals;         \
                                                \
    gs_st_Globals.a = v;                        \
}

/**
 * Standard declaration for get and set functions for the global data.
 */

#define Declare_Gx8Global_get(T,a)        \
T Gx8Global_get_##a(void)

#define Declare_Gx8Global_set(T,a)        \
void Gx8Global_set_##a( T v )

/**
 * Initialization of the global structure. Call it once at the start.
 * Then forget about it :-)
 */

void
Gx8_FirstInit( void );

// getters and setters for the global data.

__inline Define_Gx8Global_get( float, f2DFFX );
__inline Define_Gx8Global_set( float, f2DFFX );
__inline Define_Gx8Global_get( float, f2DFFX_A2D );
__inline Define_Gx8Global_set( float, f2DFFX_A2D );
__inline Define_Gx8Global_get( float, f2DFFX_B2D );
__inline Define_Gx8Global_set( float, f2DFFX_B2D );
__inline Define_Gx8Global_get( float, f2DFFY );
__inline Define_Gx8Global_set( float, f2DFFY );
__inline Define_Gx8Global_get( float, f2DFFY_A2D );
__inline Define_Gx8Global_set( float, f2DFFY_A2D );
__inline Define_Gx8Global_get( float, f2DFFY_B2D );
__inline Define_Gx8Global_set( float, f2DFFY_B2D );
__inline Define_Gx8Global_get( unsigned int, Xsize );
__inline Define_Gx8Global_set( unsigned int, Xsize );
__inline Define_Gx8Global_get( unsigned int, Ysize );
__inline Define_Gx8Global_set( unsigned int, Ysize );

#if defined( __cplusplus )
}
#endif
#endif

/*$T PAGstruct.h GC! 1.081 06/28/00 15:52:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXFLARE_H__
#define __GFXFLARE_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    constants
 ***********************************************************************************************************************
 */
#define     GFX_Flare_Hidden            0x01000000
#define     GFX_Flare_Complex           0x02000000
#define		GFX_Flare_Death				0x04000000

#define     GFX_Flare_HideChangeSize    0x00000001
#define     GFX_Flare_HideChangeColor   0x00000002
#define     GFX_Flare_DepthChangeSize   0x00000004
#define     GFX_Flare_Hide              0x00000008
#define     GFX_Flare_PosDontCare       0x00000010
#define     GFX_Flare_DieTime			0x00000020
#define		GFX_Flare_NoLODVis			0x00000040

#define     GFX_Flare_1Point            0
#define     GFX_Flare_4Points           0x00000100
#define     GFX_Flare_9Points           0x00000200
#define     GFX_Flare_PointMask         0x00000300
#define     GFX_Flare_PointShift        8

/* another flare - flags */
#define     GFX_AnotherFlare_Die        0x01
#define     GFX_AnotherFlare_Cut        0x02
#define     GFX_AnotherFlare_Back       0x04
#define     GFX_AnotherFlare_NoFog      0x08
#define		GFX_AnotherFlare_DeathSize	0x10

/* another flare - type */
#define     GFX_AnotherFlare_Simple     0x01

#define     GFX_AnotherFlare_Dying      0x01

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	GFX_tdst_ComplexFlare_
{
	MATH_tdst_Vector    st_Pos;
    MATH_tdst_Vector    st_Offset;
    float               f_Size;
    LONG                l_Flags;
    float               f_HiddenTime;
    float               f_HiddenSpeed;
    float               f_Interval;
    float               f_Ratio;
    MATH_tdst_Vector    st_BufferPos;
    MATH_tdst_Vector    st_OldPos; 

    /* data for complex flare */
    LONG                l_Nb;
    float               af_Pos[9];
    float               af_Scale[9];
    LONG                al_Id[9];
    ULONG               aul_Color[9];
} GFX_tdst_ComplexFlare;

typedef struct	GFX_tdst_Flare_
{
	MATH_tdst_Vector    st_Pos;
    MATH_tdst_Vector    st_Offset;
    float               f_Size;
    LONG                l_Flags;
    float               f_HiddenTime;
    float               f_HiddenSpeed;
    float               f_Interval;
    float               f_Ratio;

    MATH_tdst_Vector    st_BufferPos;
    MATH_tdst_Vector    st_OldPos; 

    /* data for single flare */
    LONG                l_Id;
    ULONG               ul_Color;
    ULONG               ul_DeathColor;
} GFX_tdst_Flare;

typedef struct	GFX_tdst_AnotherFlare_
{
	MATH_tdst_Vector    st_Pos;
    MATH_tdst_Vector    st_Sight;
    
    float               f_Size;
    float               f_Limit;
    float               f_Offset;
    float               f_InWallFactor;
    float               f_OutWallFactor;
    float               f_DeathTime;
    float               f_Angle;

    char                c_Flags;
    char                c_Type;
    char                c_State;
    char                c_Number;
    ULONG               ul_Color;
    ULONG               ul_DeathColor;

    float               f_TimeLeft;
    
#ifdef USE_DOUBLE_RENDERING	
	MATH_tdst_Vector    st_Pos1;
    MATH_tdst_Vector    st_Sight1;
	MATH_tdst_Vector    st_Pos2;
    MATH_tdst_Vector    st_Sight2;
#endif    
    
    
} GFX_tdst_AnotherFlare;

typedef struct GFX_tdst_SunRay_
{
    MATH_tdst_Vector    st_Axe;
    float               l, w;
    float               ratio;
    char                Pt0;
    char                Pt1;
} GFX_tdst_SunRay;

typedef struct GFX_tdst_Sun_
{
    MATH_tdst_Vector    st_Pos;

    float               f_BigRayAngleTresh;
    float               f_BigRayLMin;
    float               f_BigRayLMax;
    float               f_BigRayWMin;
    float               f_BigRayWMax;
    float               f_SmallRayAngleTresh;
    float               f_SmallRayLMin;
    float               f_SmallRayLMax;
    float               f_SmallRayWMin;
    float               f_SmallRayWMax;
    float               f_LengthRatio;
    float               f_Interval;
    float               f_HideTime;
    float               f_Dist;

    int                 i_Compute;
    int                 i_Number;
    ULONG               ul_Color;
    LONG                l_Flags;
    char                c_Die;
    char                c_FirstHideCheck;
    char                c_Dummy2;
    char                c_Dummy3;

    
    GFX_tdst_SunRay     ast_Ray[ 128 ];
    float               af_Lum[ 8 ];
    LONG                l_HideFlags;
    MATH_tdst_Vector    st_OldPos;

#ifdef USE_DOUBLE_RENDERING	
    MATH_tdst_Vector    st_Pos1;
    MATH_tdst_Vector    st_Pos2;
#endif
} GFX_tdst_Sun;

/*$4
 ***********************************************************************************************************************
    Globals et macros
 ***********************************************************************************************************************
 */
extern char    FLARE_c_MaxCounter;
extern char    FLARE_c_CurCounter;
extern char    FLARE_c_Tested;

#define FLARE_M_UpdateCounter()\
    /*FLARE_c_MaxCounter = (FLARE_c_CurCounter < 5) ? FLARE_c_CurCounter : 5;*/\
    FLARE_c_Tested = (FLARE_c_Tested + 1) % FLARE_c_MaxCounter;\
    FLARE_c_CurCounter = 0;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void			*GFX_Flare_Create( int );
void			GFX_Flare_Render( void * );
void            GFX_Flare_Test( void * );

void			*GFX_AnotherFlare_Create( struct OBJ_tdst_GameObject_ * );
int             GFX_i_AnotherFlare_Render( void * );
#ifdef USE_DOUBLE_RENDERING	
void             GFX_AnotherFlare_Interpolate( void * , u_int Mode , float fInterpoler);
void             GFX_Sun_Interpolate( void * , u_int Mode , float fInterpoler);
#endif    
    


void            *GFX_Sun_Create(void);
void            GFX_Sun_Render( void *);
void            GFX_Sun_Seti( void *, int, int );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXFLARE_H__ */

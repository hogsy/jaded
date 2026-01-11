/*$T GFXfade.h GC! 1.081 01/23/01 14:37:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXFADE_H__
#define __GFXFADE_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */
/* for border fade */
#define GFX_C_BorderFade_Center 0x00000001
#define GFX_C_BorderFade_Ratio  0x00000002

#define GFX_C_BorderFade_Format 3 

#define GFX_C_BorderFade_Line   0x00000010

#define GFX_C_BorderFade_In     0x00000100
#define GFX_C_BorderFade_Out    0x00000200


/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	GFX_tdst_Fade_
{
	ULONG				ul_ColorStart;
    ULONG				ul_ColorEnd;
    float               f_Time[6];
    float               f_Depth;
} GFX_tdst_Fade;

typedef struct	GFX_tdst_BorderFade_
{
    ULONG               ul_Flags;
	ULONG				ul_Color;
    ULONG               ul_ColorLine;
    float               x1, y1, x2, y2;
    float               f_TimeWait, f_TimeIn, f_TimeOut;
    float               f_TimeCur;
    float               dx, dy, sx, sy, w;
} GFX_tdst_BorderFade;

typedef struct	GFX_tdst_Diaphragm_
{
    ULONG				ul_Color;
    float               f_Time[5];
    MATH_tdst_Vector    st_Center;
} GFX_tdst_Diaphragm;



/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Fade_Create(void);
int		GFX_i_Fade_Render(void *);

void	*GFX_BorderFade_Create(void);
int		GFX_i_BorderFade_Render(void *);

void	*GFX_Diaphragm_Create(void);
int		GFX_i_Diaphragm_Render(void *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXFADE_H__ */

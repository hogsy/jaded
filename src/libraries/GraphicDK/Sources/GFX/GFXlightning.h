/*$T PAGstruct.h GC! 1.081 06/28/00 15:52:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXLIGHTNING_H__
#define __GFXLIGHTNING_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define LIGHTNING_Cross     1
#define LIGHTNING_UvCut     2


/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	GFX_tdst_Lightning_
{
	MATH_tdst_Vector    st_A, st_B, Z;
    float               f_Size;
    float               f_Radius;
    float               f_Time[4];
    LONG                l_NbPattern;
    LONG                l_Flags;
    LONG                l_MaxPattern;
    LONG                l_Color;
    MATH_tdst_Vector    *pst_Pattern;
} GFX_tdst_Lightning;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void			*GFX_Lightning_Create(void);
void            GFX_Lightning_Destroy(void *);
int             GFX_i_Lightning_Render( void * );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXLIGHTNING_H__ */

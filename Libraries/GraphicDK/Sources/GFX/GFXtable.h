/*$T GFXtable.h GC! 1.081 10/10/01 12:45:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXTABLE_H__
#define __GFXTABLE_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	GFX_tdst_Table_
{
	short				w_Flags;
	char				c_NbRow;
	char				c_NbCol;
	ULONG				ul_Color[10];

	MATH_tdst_Vector	st_Pos;
	float				f_RowSize[10];
	float				f_ColSize[10];

	char				ac_Content[10][10];
} GFX_tdst_Table;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Table_Create(void);
int		GFX_i_Table_Render(void *);
void	GFX_Table_Setv(void *, int, MATH_tdst_Vector *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXTABLE_H__ */

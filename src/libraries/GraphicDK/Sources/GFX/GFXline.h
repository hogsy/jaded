/*$T GFXline.h GC! 1.081 06/03/02 10:50:23 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXLINE_H__
#define __GFXLINE_H__

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

typedef struct	GFX_tdst_Line_
{
	MATH_tdst_Vector	st_A, st_B, Z;
	float				f_Size;
	ULONG				ul_Color;
#ifdef USE_DOUBLE_RENDERING	
	MATH_tdst_Vector	st_A1, st_B1;
	MATH_tdst_Vector	st_A2, st_B2;
#endif	
} GFX_tdst_Line;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Line_Create(void);
void	GFX_Line_Render(void *);

int		GFX_i_Line_Request(int, int, int, int);

/*$4
 ***********************************************************************************************************************
    Structures tetris
 ***********************************************************************************************************************
 */

#define TETRIS_COL	11
#define TETRIS_LINE 30

#define TETRIS_I	0
#define TETRIS_L	1
#define TETRIS_LI	2
#define TETRIS_S	3
#define TETRIS_Z	4
#define TETRIS_C	5

typedef struct	GFX_tdst_Tetris_
{
	MATH_tdst_Vector	st_Pos, X, Z;
	float				f_Size;
	float				f_Speed;
	ULONG				ul_Request;
    ULONG               ul_Line[5];

	float				f_Time;
	float				f_Pos;
	char				c_CurPiece;
	char				c_CurLine;
	char				c_CurCol;
	char				c_CurTurn;
	int                 i_NbSquare;

	char				ac_Grid[TETRIS_LINE][TETRIS_COL];
    int                 ai_Stat[ 6 ];
    unsigned char       ac_Prob[ 6 ];
} GFX_tdst_Tetris;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Tetris_Create(void);
void	GFX_Tetris_Render(void *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXLINE_H__ */

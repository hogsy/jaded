/*$T INOkeyboard.h GC! 1.100 07/12/01 10:29:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET /* there is no keyboard on PS2 !!! */
#pragma once
#endif
#include "BASe/BAStypes.h"
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4-******************************************************************************************************************/

void	INO_KeyboardInit(void);
void	INO_KeyboardClose(void);

void	INO_Keyboard_Update(void);

BOOL	INO_b_IsKeyPressed(UCHAR);
BOOL	INO_b_IsKeyJustPressed(UCHAR);
BOOL	INO_b_IsKeyReleased(UCHAR);
BOOL	INO_b_IsKeyJustReleased(UCHAR);

/*$4-******************************************************************************************************************/

extern UCHAR INO_gauc_KeyToPad[100];

/*$4-******************************************************************************************************************/

#define INO_CKTP_Left0			0
#define INO_CKTP_Up0			1
#define INO_CKTP_Right0			2
#define INO_CKTP_Down0			3
#define INO_CKTP_SpeedUp0		4
#define INO_CKTP_Left1			5
#define INO_CKTP_Up1			6
#define INO_CKTP_Right1			7
#define INO_CKTP_Down1			8
#define INO_CKPT_ButtonsFirst	10


/*$4-******************************************************************************************************************/

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

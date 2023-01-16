/*$T INOjoystick.h GC! 1.081 01/15/03 08:44:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __INOJOYSTICK_H__
#define __INOJOYSTICK_H__

#if defined(_XENON)
#include "Xenon/XeINOjoystick.h"
#elif defined(_XBOX)
#include <xtl.h>
#elif defined(PSX2_TARGET)
#elif defined(_GAMECUBE)
#else
#include <windows.h>
#endif
#include "BASe/BAStypes.h"
#include "INOstruct.h"
#include "MATHs/MATH.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define INO_JoyMask_A			0x00000001
#define INO_JoyMask_B			0x00000002
#define INO_JoyMask_C			0x00000004
#define INO_JoyMask_D			0x00000008
#define INO_JoyMask_L			0x00000010
#define INO_JoyMask_R			0x00000020
#define INO_JoyMask_L2			0x00000040
#define INO_JoyMask_R2			0x00000080
#define INO_JoyMask_Select		0x00000100
#define INO_JoyMask_Start		0x00000200
#define INO_JoyMask_StickR		0x00000400
#define INO_JoyMask_StickL		0x00000800

#define INO_JoyMask_Joy0		0x01000000
#define INO_JoyMask_Joy1		0x02000000
#define INO_JoyMask_Joy2		0x04000000
#define INO_JoyMask_Joy3		0x08000000
#define INO_JoyMask_JoyShift	24

#define INO_JoyMask_All			0xFFFFFFFF

#define INO_JoyDir_X0Invert		0x1
#define INO_JoyDir_Y0Invert		0x2
#define INO_JoyDir_X1Invert		0x4
#define INO_JoyDir_Y1Invert		0x8

#define INO_JoyUSBPS2_Version0	0
#define INO_JoyUSBPS2_Version1	1
#define INO_JoyUSBPS2_Version2	2
#define INO_JoyUSBPS2_Version3	3
#define INO_JoyUSBPS2_Version4	4

#define INO_Joy_PCMode			0
#define INO_Joy_Ps2Mode			1
#define INO_Joy_XenonMode		2
/*$4
 ***********************************************************************************************************************
    globals
 ***********************************************************************************************************************
 */

extern LONG INO_l_JoystickMask;
extern LONG INO_l_JoystickDir;

#ifdef WIN32
extern LONG win32INO_l_JoyUSBPS2;
extern LONG win32INO_l_Joystick_XLeft[2];
extern LONG win32INO_l_Joystick_XLeftStart[2];
extern LONG win32INO_l_Joystick_XRightStart[2];
extern LONG win32INO_l_Joystick_XRight[2];
extern LONG win32INO_l_Joystick_YUp[2];
extern LONG win32INO_l_Joystick_YUpStart[2];
extern LONG win32INO_l_Joystick_YDownStart[2];
extern LONG win32INO_l_Joystick_YDown[2];
extern LONG win32INO_l_Joystick_Mode;
#ifdef ACTIVE_EDITORS
extern LONG INO_l_Joystick_X[2];
extern LONG INO_l_Joystick_Y[2];
#endif /* ACTIVE_EDITORS */
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
typedef enum INO_tden_GenericButtonId_
{
	eBtn_Cross					= 0,
	eBtn_Circle					= 1,
	eBtn_Square					= 2,
	eBtn_Triangle				= 3,
	eBtn_L1						= 4,
	eBtn_R1						= 5,
	eBtn_L2						= 6,
	eBtn_R2						= 7,
	eBtn_Select					= 8,
	eBtn_Start					= 9,
	eBtn_R3						= 10,
	eBtn_L3						= 11,
	eBtn_Up						= 12,
	eBtn_Right					= 13,
	eBtn_Down					= 14,
	eBtn_Left					= 15,
	eBtn_GenericButtonNb
} INO_tden_GenericButtonId;

#ifdef _XBOX
typedef enum	xb_INO_tden_ButtonId_
{
	e_ButtonA = 0,
	e_ButtonB = 1,
	e_ButtonX = 2,
	e_ButtonY = 3,
	e_ButtonWhite = 4,
	e_ButtonBlack = 5,
	e_LeftTrigger = 6,
	e_RightTrigger = 7,
	e_Back = 8,
	e_Start = 9,
	e_LeftThumb = 10,
	e_RightThumb = 11,
	e_DPadUp = 12,
	e_DPadRight = 13,
	e_DPadDown = 14,
	e_DPadLeft = 15,
} xb_INO_tden_ButtonId;

void			INO_Joystick_Init(HWND _hWnd);
void			INO_Joystick_Close(void);
void			INO_Joystick_Update(void);
BOOL			INO_b_Joystick_IsButtonJustDown(LONG);
BOOL			INO_b_Joystick_IsButtonDown(LONG);
BOOL			INO_b_Joystick_IsButtonJustUp(LONG);
BOOL			INO_b_MultiJoystick_IsButtonJustUp(LONG);
void			INO_Joystick_Move(MATH_tdst_Vector *, int);

#endif

void			INO_Joystick_Init(HWND _hWnd);
void			INO_Joystick_Close(void);
void			INO_Joystick_Update(void);
BOOL			INO_b_Joystick_IsButtonJustDown(LONG);
BOOL			INO_b_Joystick_IsButtonDown(LONG);
BOOL			INO_b_Joystick_IsButtonJustUp(LONG);
void			INO_Joystick_Move(struct MATH_tdst_Vector_ *, int);
void			INO_PifSet(int _i_FrameDuration);
void			INO_PafSet(int _i_Intensity, int _i_FrameDuration);
int				INO_i_PifPafEnable(int _i_Enable);
void			INO_JoySimulate(int, int, struct MATH_tdst_Vector_ *, struct MATH_tdst_Vector_ *);
int				INO_i_SetCurrentPad(int id);
int				INO_i_GetCurrentPad(void);
void			INO_SetControlMode(int mode);
void			INO_SetControlSubmode(int submode);
int				INO_i_IsMouseBeingUsed(void);
void			INO_Joystick_GetPointer(MATH_tdst_Vector *);
void			INO_Joystick_GetAcc(MATH_tdst_Vector *, int);
int				INO_Joystick_GetLastMove(int);
float			INO_Joystick_GetDistance(void);
void			INO_Joystick_SetVibration(int,float);

#ifdef JOYRECORDER
void            INO_StartRecord(void);
void            INO_StartPlay(void);
void            INO_StopRecord(void);
void            INO_StopPlay(void);
#endif

#ifdef _PC_RETAIL
#define INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD
#endif

#ifdef INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD
void			pcINO_Joystick_Acquire(void);
void			pcINO_Joystick_Unacquire(void);
#define INO_Joystick_Acquire()		pcINO_Joystick_Acquire()
#define INO_Joystick_Unacquire()	pcINO_Joystick_Unacquire()
#else
#define INO_Joystick_Acquire()		((void)0)
#define INO_Joystick_Unacquire()	((void)0)
#endif

#if defined( _XBOX )
unsigned char xbINO_uc_GetButton(int port, int button);
bool xbINO_Joystick_IsJoyValid(void);

#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __INOJOYSTICK_H */

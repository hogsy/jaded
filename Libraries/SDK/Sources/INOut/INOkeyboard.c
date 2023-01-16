/*$T INOkeyboard.c GC!1.30 05/06/99 11:05:01 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "INOkeyboard.h"
#include "INOJoystick.h" // for INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD
#ifdef INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif



#if !defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_XENON)/* no keyboard on PS2 or GC or XBOX */
static char *INO_gspc_State;
static char *INO_gspc_PrevState;
UCHAR INO_gauc_KeyToPad[100];
#else
unsigned char ucVirtualPressedKey[3] = {0, 0, 0};
#endif

/*
 ===================================================================================================
 ===================================================================================================
 */
void INO_KeyboardInit(void)
{
#if !defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_XENON) /* no keyboard on PS2 or GC or XBOX */    
	INO_gspc_State = (char*)MEM_p_Alloc( 256 );
    INO_gspc_PrevState = (char*)MEM_p_Alloc( 256 );

	/* Default key to pad */
	memset(INO_gauc_KeyToPad, 0, sizeof(INO_gauc_KeyToPad));
#if 0
	INO_gauc_KeyToPad[INO_CKTP_Left0] = VK_LEFT;
	INO_gauc_KeyToPad[INO_CKTP_Up0] = VK_UP;
	INO_gauc_KeyToPad[INO_CKTP_Right0] = VK_RIGHT;
	INO_gauc_KeyToPad[INO_CKTP_Down0] = VK_DOWN;
	INO_gauc_KeyToPad[INO_CKTP_SpeedUp0] = VK_SHIFT;
	INO_gauc_KeyToPad[INO_CKTP_Left1] = 'Q';
	INO_gauc_KeyToPad[INO_CKTP_Up1] = 'S';
	INO_gauc_KeyToPad[INO_CKTP_Right1] = 'D';
	INO_gauc_KeyToPad[INO_CKTP_Down1] = 'X';

	INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst] = VK_SPACE;
	INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst + 1] = VK_RETURN;
	INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst + 2] = VK_SHIFT;
	INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst + 3] = VK_CONTROL;
	INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst + 4] = VK_NUMPAD1;
	INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst + 5] = VK_NUMPAD2;
	INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst + 6] = VK_NUMPAD3;
	INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst + 7] = VK_NUMPAD4;
	INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst + 8] = VK_NUMPAD5;
	INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst + 9] = VK_NUMPAD6;	
#endif
#endif
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void INO_KeyboardClose(void)
{
#if !defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_XENON) /* no keyboard on PS2 or GC or XBOX */    
	MEM_Free( INO_gspc_State );
    MEM_Free( INO_gspc_PrevState );
#endif
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void INO_Keyboard_Update(void)
{
#if !defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_XENON) /* no keyboard on PS2 or GC or XBOX */
    char *key;

    /* swap key state */
    key = INO_gspc_PrevState;
    INO_gspc_PrevState = INO_gspc_State;
    INO_gspc_State = key;

#ifndef INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD
    GetKeyboardState( (PBYTE)key );
#else
	{
		extern unsigned char pcINO_gab_LastKeyboardState[];

		ZeroMemory(INO_gspc_State, 256);
		if(pcINO_gab_LastKeyboardState[DIK_RCONTROL])
		{
			int i;
			for(i = 0; i < 10; ++i)
			{
				if(pcINO_gab_LastKeyboardState[DIK_F1 + i])
				{
					INO_gspc_State[VK_LMENU] = 0x80;
					INO_gspc_State[VK_NUMPAD0 + i] = 0x80;
				}
			}
		}
	}
#endif
#endif
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL INO_b_IsKeyPressed(unsigned char _uc_Key)
{
#if !defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_XENON) /* no keyboard on PS2 or GC or XBOX */
    return INO_gspc_State[ _uc_Key ] & 0x80;
#else
    int i;
    for(i=0; i<3; i++)
    {
	    if (_uc_Key == (0xFF & ucVirtualPressedKey[i])) {return TRUE;}
	}
	return 0;
#endif
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL INO_b_IsKeyJustPressed(unsigned char _uc_Key)
{
#if !defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_XENON) /* no keyboard on PS2 or GC or XBOX */
    return (INO_gspc_State[ _uc_Key ] & 0x80) && !(INO_gspc_PrevState[ _uc_Key ] & 0x80);
#else
    int i;
    for(i=0; i<3; i++)
    {
	    if (_uc_Key ==(0xFF&ucVirtualPressedKey[i])) {return TRUE;}
	}
	return 0;
#endif
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL INO_b_IsKeyReleased(unsigned char _uc_Key)
{
	return !(INO_gspc_State[ _uc_Key ] & 0x80);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL INO_b_IsKeyJustReleased(unsigned char _uc_Key)
{
    return !(INO_gspc_State[ _uc_Key ] & 0x80) && (INO_gspc_PrevState[ _uc_Key ] & 0x80);
}



#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
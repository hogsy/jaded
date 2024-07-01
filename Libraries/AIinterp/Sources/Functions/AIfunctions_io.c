/*$T AIfunctions_io.c GC 1.138 07/01/03 17:27:48 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "LINks/LINKmsg.h"
#include "stdio.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "INOut/INOkeyboard.h"
#include "INOut/INOjoystick.h"
#include "INOut/INOfile.h"
#include "INOut/INO.h"
#include "ENGine/Sources/ENGvars.h"

#ifdef PSX2_TARGET
#include <libscf.h>
#endif

#ifdef _XENON
#include "Xenon/Profile/Profile.h"
#endif

#ifdef PSX2_DEMO
#include <libsceedemo.h>
#include <libscedemo.h>
#endif

#ifdef PSX2_DEMO
extern unsigned short	Demo_gus_language;
extern unsigned short	Demo_gus_aspect;
extern unsigned short	Demo_gus_playmode;
extern unsigned short	Demo_gus_to_inactive;
extern unsigned short	Demo_gus_to_gameplay;
extern unsigned short	Demo_gus_Exit;
extern sceDemoEndReason	Demo_ge_EndReason;
#endif

extern BOOL				Demo_gb_Exit;
extern BOOL 			INO_gb_SavingScreen;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoGetProtect_C(int _i_Key)
{
	return _i_Key;
}
/**/
AI_tdst_Node *AI_EvalFunc_IoGetProtect(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoGetProtect_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoKeyPressed_C(int _i_Key)
{
	return(INO_b_IsKeyPressed((char) _i_Key) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_IoKeyPressed(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoKeyPressed_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoKeyJustPressed_C(int _i_Key)
{
	return(INO_b_IsKeyJustPressed((char) _i_Key) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_IoKeyJustPressed(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoKeyJustPressed_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoKeyReleased_C(int _i_Key)
{
	return(INO_b_IsKeyReleased((char) _i_Key) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_IoKeyReleased(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoKeyReleased_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoKeyJustReleased_C(int _i_Key)
{
	return(INO_b_IsKeyJustReleased((char) _i_Key) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_IoKeyJustReleased(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoKeyJustReleased_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoPadSet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(INO_i_SetCurrentPad(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoPadGet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(INO_i_GetCurrentPad());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoButtonPressed_C(int _i_Button)
{
#ifdef _GAMECUBE
	if(INO_gb_SavingScreen && (_i_Button == 9)) return FALSE;
#endif

#if defined(PSX2_TARGET) || defined(_GAMECUBE) || defined(_XBOX) || defined( _XENON )
	return INO_b_Joystick_IsButtonDown(_i_Button);
#else
	return(INO_b_Joystick_IsButtonDown(_i_Button) ? 255 : 0);
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_IoButtonPressed(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoButtonPressed_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static MATH_tdst_Vector	sgt_MoveMul = { 1, 1, 0 };
static MATH_tdst_Vector	sgt_MoveMinMaxX = { -1, 1, 0 };
static MATH_tdst_Vector	sgt_MoveMinMaxY = { -1, 1, 0 };
void AI_EvalFunc_IoJoyMoveMdf_C(MATH_tdst_Vector *v, MATH_tdst_Vector *v1, MATH_tdst_Vector *v2)
{
	MATH_CopyVector(&sgt_MoveMul, v);
	MATH_CopyVector(&sgt_MoveMinMaxX, v1);
	MATH_CopyVector(&sgt_MoveMinMaxY, v2);
}
AI_tdst_Node *AI_EvalFunc_IoJoyMoveMdf(AI_tdst_Node *_pst_Node)
{
	MATH_tdst_Vector	v, v1, v2;

	AI_PopVector(&v2);
	AI_PopVector(&v1);
	AI_PopVector(&v);
	AI_EvalFunc_IoJoyMoveMdf_C(&v, &v1, &v2);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int	IO_SpecialE3_JoyMoveModif = 3;
 
void AI_EvalFunc_IoJoyGetMove_C(MATH_tdst_Vector *v)
{
	INO_Joystick_Move(v, 0);

	v->x *= sgt_MoveMul.x;
	v->y *= sgt_MoveMul.y;
	if(v->x < sgt_MoveMinMaxX.x) v->x = sgt_MoveMinMaxX.x;
	if(v->x > sgt_MoveMinMaxX.y) v->x = sgt_MoveMinMaxX.y;
	if(v->y < sgt_MoveMinMaxY.x) v->y = sgt_MoveMinMaxY.x;
	if(v->y > sgt_MoveMinMaxY.y) v->y = sgt_MoveMinMaxY.y;
	
	switch ( IO_SpecialE3_JoyMoveModif )
	{
	case 1:	v->x *= (float) fabs( v->x );	v->y *= (float) fabs( v->y );	break;
	case 2: v->x *= 0.85f;	v->y *= 0.85f;	break;
	case 3: if (v->x >= 0)
				v->x = (v->x < 0.15f) ? 0.0f : (v->x - 0.15f) / 0.85f;
			else
				v->x = (v->x > -0.15f) ? 0.0f : (v->x + 0.15f) / 0.85f;
			if (v->y >= 0)
				v->y = (v->y < 0.15f) ? 0.0f : (v->y - 0.15f) / 0.85f;
			else
				v->y = (v->y > -0.15f) ? 0.0f : (v->y + 0.15f) / 0.85f;
			break;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_IoJoyGetMove(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	AI_EvalFunc_IoJoyGetMove_C(&v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_IoJoyGetMove1_C(MATH_tdst_Vector *v)
{
	INO_Joystick_Move(v, 1);
}
/**/
AI_tdst_Node *AI_EvalFunc_IoJoyGetMove1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	AI_EvalFunc_IoJoyGetMove1_C(&v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoButtonJustPressed_C(int _i_Button)
{
#ifdef _GAMECUBE
	if(INO_gb_SavingScreen && (_i_Button == 9)) return 0;
#endif

	return(INO_b_Joystick_IsButtonJustDown(_i_Button) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_IoButtonJustPressed(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoButtonJustPressed_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoButtonJustReleased_C(int _i_Button)
{
#ifdef _GAMECUBE
	if(INO_gb_SavingScreen && (_i_Button == 9)) return 0;
#endif

	return(INO_b_Joystick_IsButtonJustUp(_i_Button) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_IoButtonJustReleased(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoButtonJustReleased_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoJoyFreeze_C(int _i_Mask)
{
	/*~~~~~~*/
	int i_Old;
	/*~~~~~~*/

	i_Old = (int)~INO_l_JoystickMask;
	INO_l_JoystickMask = ~_i_Mask;
	return i_Old;
}
/**/
AI_tdst_Node *AI_EvalFunc_IoJoyFreeze(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoJoyFreeze_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoJoyDir_C(int _i_Value)
{
	/*~~~~~~*/
	int i_Old;
	/*~~~~~~*/

	i_Old = INO_l_JoystickDir;
	if(_i_Value != -1) INO_l_JoystickDir = _i_Value;
	return i_Old;
}
/**/
AI_tdst_Node *AI_EvalFunc_IoJoyDir(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoJoyDir_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoJoySimulate(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Buttons;
	MATH_tdst_Vector	*pst_Joy0, *pst_Joy1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Joy1 = AI_PopVectorPtr();
	pst_Joy0 = AI_PopVectorPtr();
	i_Buttons = AI_PopInt();
	INO_JoySimulate(AI_PopInt(), i_Buttons, pst_Joy0, pst_Joy1);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoPifSet(AI_tdst_Node *_pst_Node)
{
	INO_PifSet(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoPafSet(AI_tdst_Node *_pst_Node)
{
	/*~~*/
	int i;
	/*~~*/

	i = AI_PopInt();
	INO_PafSet(AI_PopInt(), i);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoPifPafEnable(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(INO_i_PifPafEnable(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int INO_i_GetDiscError(void)
{
	return INO_gi_DiscError;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoDiscErrorGet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(INO_i_GetDiscError());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoSetControlMode(AI_tdst_Node *_pst_Node)
{
	INO_SetControlMode(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoSetControlSubmode(AI_tdst_Node *_pst_Node)
{
	INO_SetControlSubmode(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoIsMouseBeingUsed(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(INO_i_IsMouseBeingUsed());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

int Demo_Geti(int rq)
{
#ifdef PSX2_DEMO
	switch(rq)
	{
	case 1: /* language */
		switch(Demo_gus_language)
		{
		case SCEE_DEMO_LANGUAGE_ENGLISH:	return 1;
		case SCEE_DEMO_LANGUAGE_FRENCH:		return 0;
		case SCEE_DEMO_LANGUAGE_GERMAN:		return 2;
		case SCEE_DEMO_LANGUAGE_SPANISH:	return 3;
		case SCEE_DEMO_LANGUAGE_ITALIAN:	return 4;
		default:							return -1;
		}
		break;

	case 2: /* video aspect ratio */
		switch(Demo_gus_aspect)
		{
		case SCEE_DEMO_ASPECT_4_3:	return 0;
		case SCEE_DEMO_ASPECT_16_9: return 1;
		default:					return -1;
		}
		break;

	case 3: /* playability mode */
		switch(Demo_gus_playmode)
		{
		case SCEE_DEMO_PLAYMODE_PLAYABLE:	return 0;
		case SCEE_DEMO_PLAYMODE_ATTRACT:	return 1;
		default:							return -1;
		}
		break;

	case 4: /* inactivity timeout */
		return Demo_gus_to_inactive;

	case 5: /* gameplay timeout */
		return Demo_gus_to_gameplay;

	case 6: /* select+start pressed ? */
		return Demo_gb_Exit;

	default:
		return -1;
	}

#else
	switch(rq)
	{
	case 1:		/* language */return 0;
	case 2:		/* video aspect ratio */return 0;
	case 3:		/* playability mode */return 0;
	case 4:		/* inactivity timeout */return(60 * 1);
	case 5:		/* gameplay timeout */return(60 * 4);
	case 6:		/* select+start pressed ? */
		{
			BOOL bb = Demo_gb_Exit;
			Demo_gb_Exit = FALSE;
			return bb;
		}
	default:	return -1;
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int INO_gi_LaunchDemo=-1; 
int Demo_Seti(int rq)
{
	switch(rq)
	{
	case 6:		ENG_gb_ExitApplication = TRUE; ENG_gb_ForceEndEngine = TRUE; return 0;
#ifdef PSX2_DEMO
	case 100:   Demo_ge_EndReason = SCE_DEMO_ENDREASON_ATTRACT_INTERRUPTED; break;
	case 101:   Demo_ge_EndReason =   SCE_DEMO_ENDREASON_ATTRACT_COMPLETE; break;
	case 102:   Demo_ge_EndReason =   SCE_DEMO_ENDREASON_PLAYABLE_INACTIVITY_TIMEOUT; break;
	case 103:   Demo_ge_EndReason =   SCE_DEMO_ENDREASON_PLAYABLE_GAMEPLAY_TIMEOUT; break;
	case 104:   Demo_ge_EndReason =   SCE_DEMO_ENDREASON_PLAYABLE_COMPLETE; break;
	case 105:   Demo_ge_EndReason =   SCE_DEMO_ENDREASON_PLAYABLE_QUIT; break;
#endif
    case 200:   INO_gi_LaunchDemo=0;ENG_gb_ExitApplication = TRUE; ENG_gb_ForceEndEngine = TRUE; return 0;
    case 201:   INO_gi_LaunchDemo=1;ENG_gb_ExitApplication = TRUE; ENG_gb_ForceEndEngine = TRUE; return 0;
    case 202:   INO_gi_LaunchDemo=2;ENG_gb_ExitApplication = TRUE; ENG_gb_ForceEndEngine = TRUE; return 0;
    case 203:   INO_gi_LaunchDemo=3;ENG_gb_ExitApplication = TRUE; ENG_gb_ForceEndEngine = TRUE; return 0;
	default:	return -1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoDemoGeti(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(Demo_Geti(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoDemoSeti(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(Demo_Seti(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
void AI_EvalFunc_IoJoyGetPointer_C(MATH_tdst_Vector *v)
{
	INO_Joystick_GetPointer(v);
}
/**/
AI_tdst_Node *AI_EvalFunc_IoJoyGetPointer(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	AI_EvalFunc_IoJoyGetPointer_C(&v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 

void AI_EvalFunc_IoJoyGetAcc_C(int _i_Num, MATH_tdst_Vector *v)
{
	INO_Joystick_GetAcc(v, _i_Num);
}


/**/
AI_tdst_Node *AI_EvalFunc_IoJoyGetAcc(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	int i_Num;
	/*~~~~~~~~~~~~~~~~~~*/

	i_Num = AI_PopInt();
	AI_EvalFunc_IoJoyGetAcc_C(i_Num, &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
int AI_EvalFunc_IoJoyGetLastMove_C(int _i_Num)
{
	return INO_Joystick_GetLastMove(_i_Num);
}
/**/
AI_tdst_Node *AI_EvalFunc_IoJoyGetLastMove(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int	i_move;
	int i_Num;
	/*~~~~~~~~~~~~~~~~~~*/

	i_Num = AI_PopInt();
	i_move = AI_EvalFunc_IoJoyGetLastMove_C(i_Num);
	AI_PushInt(i_move);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
float AI_EvalFunc_IoJoyGetDistance_C()
{
	return INO_Joystick_GetDistance();
}
/**/
AI_tdst_Node *AI_EvalFunc_IoJoyGetDistance(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	float f_distance;
	/*~~~~~~~~~~~~~~~~~~*/

	f_distance = AI_EvalFunc_IoJoyGetDistance_C();
	AI_PushFloat(f_distance);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
void AI_EvalFunc_IoJoySetVibration_C(int i_Num,float f_Delay)
{
	INO_Joystick_SetVibration(i_Num,f_Delay);
}
/**/
AI_tdst_Node *AI_EvalFunc_IoJoySetVibration(AI_tdst_Node *_pst_Node)
{
	int i_Num;
	float f_Delay;
	
	f_Delay = AI_PopFloat();
	i_Num = AI_PopInt();
	
	AI_EvalFunc_IoJoySetVibration_C(i_Num,f_Delay);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    LANGUAGE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoGameLangGet_C(void)
{
	/*~~~~~*/
	int i, j;
	/*~~~~~*/

	if ( INO_b_LanguageIsPresent( INO_e_Russian ) )
		return ( 1 << INO_e_Russian );
	else if ( INO_b_LanguageIsPresent( INO_e_Polish ) )
		return ( ( 1 << INO_e_English ) | ( 1 << INO_e_Polish ) );

	for(j = i = 0; i < INO_e_MaxLangNb; i++)
	{
		if(INO_b_LanguageIsPresent(i)) j |= (1 << i);
	}

	return j;
}
/**/
AI_tdst_Node *AI_EvalFunc_IoGameLangGet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoGameLangGet_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoGameLangSet_C(int i)
{
	extern void TEXT_ChangeLang(int);

	if(!INO_b_LanguageIsPresent(i)) return -1;

	TEXT_ChangeLang(i);
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_IoGameLangSet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoGameLangSet_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoGameSndLangSet_C(int i)
{
	extern void TEXT_ChangeSndLang(int);
#ifdef JADEFUSION
	if(!INO_b_LanguageIsPresent(i, TRUE)) return -1;
#else
	if(!INO_b_LanguageIsPresent(i)) return -1;
#endif
	TEXT_ChangeSndLang(i);
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_IoGameSndLangSet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoGameSndLangSet_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoConsLangGet_C(void)
{
	/* TODO FOR ALL CONSOLE */
#ifdef PSX2_TARGET
    switch(sceScfGetLanguage())
    {
    case SCE_JAPANESE_LANGUAGE:return INO_e_English;
    case SCE_ENGLISH_LANGUAGE:return INO_e_English;
    case SCE_FRENCH_LANGUAGE:return INO_e_French;
    case SCE_SPANISH_LANGUAGE:return INO_e_Spanish;
    case SCE_GERMAN_LANGUAGE:return INO_e_German;
    case SCE_ITALIAN_LANGUAGE:return INO_e_Italian;
    case SCE_DUTCH_LANGUAGE:return INO_e_Dutch;
    case SCE_PORTUGUESE_LANGUAGE:return INO_e_English;
    default:return INO_e_English;
    }
#endif

#ifdef _GAMECUBE
	if(VIGetTvFormat() == VI_NTSC)
		return INO_e_English;
	else
	{
	    switch(OSGetLanguage())
	    {
	    case OS_LANG_ENGLISH:return INO_e_English;
	    case OS_LANG_FRENCH:return INO_e_French;
	    case OS_LANG_SPANISH:return INO_e_Spanish;
	    case OS_LANG_GERMAN:return INO_e_German;
	    case OS_LANG_ITALIAN:return INO_e_Italian;
        case OS_LANG_DUTCH:return INO_e_Dutch;
	    default:return INO_e_English;
	    }
    }

#endif
#ifdef _XBOX
	{
		//DWORD xblang=XGetLanguage();
#ifndef _XBOXDEMO
		//return INO_e_English;
		switch (XGetLanguage())
		{
		case XC_LANGUAGE_ENGLISH:return INO_e_English;
		//case XC_LANGUAGE_JAPANESE:return INO_e_Japanese;
		case XC_LANGUAGE_GERMAN:return INO_e_German;
		case XC_LANGUAGE_FRENCH:return INO_e_French;
		case XC_LANGUAGE_SPANISH:return INO_e_Spanish;
		case XC_LANGUAGE_ITALIAN:return INO_e_Italian;
		//case XC_LANGUAGE_KOREAN:return INO_e_Korean;
		//case XC_LANGUAGE_TCHINESE:return INO_e_Chinese;
		//case XC_LANGUAGE_PORTUGUESE:return INO_e_Portuguese;//??
		}
#endif
		return INO_e_English;
		//return INO_e_Dutch;
	}
#endif

#ifdef _XENON
	switch( g_XeProfile.GetLanguage( ) )
	{
		case CXeProfile::LANGUAGE_ENGLISH:
			return INO_e_English;

		case CXeProfile::LANGUAGE_FRENCH:
			return INO_e_French;

		case CXeProfile::LANGUAGE_SPANISH:
			return INO_e_Spanish;

		case CXeProfile::LANGUAGE_GERMAN:
			return INO_e_German;

		case CXeProfile::LANGUAGE_ITALIAN:
			return INO_e_Italian;

		case CXeProfile::LANGUAGE_DUTCH:
			return INO_e_Dutch;

		case CXeProfile::LANGUAGE_JAPANESE:
			return INO_e_Japanese;
	}
#endif
	return INO_e_English;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoConsLangGet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoConsLangGet_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_IoConsLangSet_C(int i)
{
	/* TODO FOR ALL CONSOLE */
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_IoConsLangSet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_IoConsLangSet_C(AI_PopInt()));
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
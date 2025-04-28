/*$T STReffects.c GC! 1.081 10/10/02 14:56:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$F
Exemple d'IA

// effect blink
i = OBJ_TextEffectAdd( 2, String_Start, 0, -1)
OBJ_TextEffectSeti( i, 0, 1)                // smooth (avec fade) ou non
OBJ_TextEffectSeti( i, 200, 0xFF0000FF)     // une des couleurs
OBJ_TextEffectSeti(i, 201, 0xFFFF0000)      // l'autre couleur
OBJ_TextEffectSetf(i, 250, 0.4)             // temps avant chaque changement

// effect gouraud
i = OBJ_TextEffectAdd( 3, String_Start, 0, -1)
OBJ_TextEffectSeti( i, 300, 0xFF0000FF)     // couleurs aux 4 coins
OBJ_TextEffectSeti(i,  301, 0xFF0000FF)
OBJ_TextEffectSeti(i,  302, 0xFFFF0000)
OBJ_TextEffectSeti(i,  303, 0xFF000000)

// effet changement de couleur
i = OBJ_TextEffectAdd( 4, 3, 0, -1)
OBJ_TextEffectSeti( i, 400, 0xFF000000)     // couleur de départ
OBJ_TextEffectSeti(i, 401, 0xFFFFFFFF)      // couleur de fin
OBJ_TextEffectSetf(i, 450, 0.5)             // temps

// effet bounce
i = OBJ_TextEffectAdd( 5, 1, 0, -1)
OBJ_TextEffectSetf( i, 550, 0.002)          // A = amplitude en y
OBJ_TextEffectSetf( i, 551, 20)             // f0 facteur multiplicatif du temps
OBJ_TextEffectSetf( i, 552, 200)            // f1 facteur multiplicatif du x
OBJ_TextEffectSetf( i, 553, 1)              // temps ( t = temps restant)
    
      On calcule DiffY = A * t * t * sin( t * f0 + PosX * f1)
      ( t = temps restant)

// effet perturbation
i = OBJ_TextEffectAdd( 6, 3, 0, -1)
OBJ_TextEffectSetf( i, 650, 0.005)          // max perturb en X
OBJ_TextEffectSetf( i, 651, 0.005)          // max perturb en y
OBJ_TextEffectSetf( i, 652, 0.5)            // temps


*/
#include "Precomp.h"

#include "TIMer/TIMdefs.h"
#include "STRing/STRstruct.h"
#include "GEOmetric/GEOobject.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "SOFT/SOFTcolor.h"
#include "GFX/GFX.h"

#ifdef PSX2_TARGET

/* mamagouille */
#include "PSX2debug.h"
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

// Max, 09/01/2003 added Xbox below.

#if defined( PSX2_TARGET ) || defined( _XBOX ) || defined(_XENON)
#define C_Pos2DFactorX	1
#define C_Pos2DFactorY	1

#else
#define C_Pos2DFactorX	_pst_STR->uw_SW
#define C_Pos2DFactorY	_pst_STR->uw_SH

#endif

/*$4
 ***********************************************************************************************************************
    Effects
 ***********************************************************************************************************************
 */

static int				STR_si_Id = 0;
STR_tdst_Effect	*STR_spst_CurEffect = NULL;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
STR_tdst_Effect *STR_pst_GetUnusedEffect(STR_tdst_Struct *_pst_STR)
{
	if(_pst_STR->uc_NbCurEffects == _pst_STR->uc_NbMaxEffects) return NULL;
	return &_pst_STR->st_Effect[_pst_STR->uc_NbCurEffects++];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetCurEffect(STR_tdst_Struct *_pst_STR, int _i_Id)
{
	/*~~*/
	int i;
	/*~~*/

	if(STR_spst_CurEffect && (STR_spst_CurEffect->i_Id == _i_Id)) return;
	for(i = 0; i < _pst_STR->uc_NbCurEffects; i++)
	{
		if(_pst_STR->st_Effect[i].i_Id == _i_Id)
		{
			STR_spst_CurEffect = _pst_STR->st_Effect + i;
			return;
		}
	}

	STR_spst_CurEffect = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_EffectCheck( STR_tdst_Struct *_pst_STR, int i, int f, int n )
{
	int effect;

	for(effect = 0; effect < _pst_STR->uc_NbCurEffects; effect++)
	{
		if( _pst_STR->st_Effect[ effect ].i_String == i )
		{
			if ( f < _pst_STR->st_Effect[ effect ].i_From )
			{
				if ( f + n < _pst_STR->st_Effect[ effect ].i_From )
					_pst_STR->st_Effect[ effect ].i_From -= n;
				else
				{
					_pst_STR->st_Effect[ effect ].i_From = f;
					if (_pst_STR->st_Effect[effect ].i_Number > 0)
					{
						_pst_STR->st_Effect[ effect ].i_Number -= n;
						if (_pst_STR->st_Effect[ effect ].i_Number < 0)
							_pst_STR->st_Effect[ effect ].ul_Flags |= STR_Cul_ER_Terminated;
					}
				}
			}
			return;
		}
	}
}

/*$4
 ***********************************************************************************************************************
    Access
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_i_EffectAdd(OBJ_tdst_GameObject *_pst_GO, int Type, int String, int From, int Number)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	STR_tdst_Effect *pst_Effect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (Type & 0x80000000)
	{
		Type -= 0x80000000;
		pst_STR = (STR_tdst_Struct *) _pst_GO;
	}
	else
		pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return -1;

	if(pst_STR->uc_NbCurEffects == pst_STR->uc_NbMaxEffects) return -1;

	pst_Effect = &pst_STR->st_Effect[pst_STR->uc_NbCurEffects++];
	L_memset(pst_Effect, 0, sizeof(STR_tdst_Effect));
	pst_Effect->ul_Flags = Type;
	pst_Effect->i_String = String;
	pst_Effect->i_From = From;
	pst_Effect->i_Number = Number;
	pst_Effect->i_Id = STR_si_Id++;

	STR_spst_CurEffect = pst_Effect;

	/* default value */
	switch(Type)
	{
	case STR_Cul_EF_Perturb:
		pst_Effect->st_Perturb.f_Time = 1;
		pst_Effect->st_Perturb.fX = 0.005f;
		pst_Effect->st_Perturb.fY = 0.005f;
		break;
	case STR_Cul_EF_Shrink:
	case STR_Cul_EF_Unshrink:
	case STR_Cul_EF_UnshrinkCenter:
	case STR_Cul_EF_ShrinkCenter:
		pst_Effect->st_Shrink.f_Time = 1;
		pst_Effect->st_Shrink.f_LetterTime = 0.1f;
		pst_Effect->st_Shrink.f_Delay = 0.1f;
		break;

	case STR_Cul_EF_Enlarge:
	case STR_Cul_EF_Unenlarge:
		pst_Effect->st_Enlarge.f_Delay = 0;
		pst_Effect->st_Enlarge.f_Factor = 2;
		pst_Effect->st_Enlarge.f_Time = 1.0f;
		break;

	case STR_Cul_EF_Door:
	case STR_Cul_EF_Undoor:
		pst_Effect->st_Door.f_Time = 1.0f;
		pst_Effect->st_Door.f_Ref = 0;
		break;
	case STR_Cul_EF_Flow:
		pst_Effect->st_Flow.f_Time = 1;
		break;
	case STR_Cul_EF_Cut:
		pst_Effect->st_Cut.max = 1;
		break;
	case STR_Cul_EF_Italic:
		pst_Effect->st_Italic.value = 0.1f;
		break;
	case STR_Cul_EF_Blur:
		pst_Effect->st_Blur.f_CurTime = 0;
		pst_Effect->st_Blur.f_Time = 1.0f;
		pst_Effect->st_Blur.f_Dist = 0.2f;
		break;
	}

	return pst_Effect->i_Id;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_i_EffectGetId( STR_tdst_Struct *_pst_STR, int _i_String )
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < _pst_STR->uc_NbCurEffects; i++)
	{
		if(_pst_STR->st_Effect[i].i_String == _i_String)
		{
			STR_spst_CurEffect = &_pst_STR->st_Effect[i];
			return (_pst_STR->st_Effect[i].i_Id );
		}
	}
	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_EffectDelAll(STR_tdst_Struct *_pst_STR)
{
	_pst_STR->uc_NbCurEffects = 0;
	STR_spst_CurEffect = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_EffectDel(OBJ_tdst_GameObject *_pst_GO, int _i_Id)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return;

	if(_i_Id < 0)
	{
		STR_EffectDelAll(pst_STR);
		return;
	}

	STR_SetCurEffect(pst_STR, _i_Id);
	if(!STR_spst_CurEffect) return;

	i = STR_spst_CurEffect - pst_STR->st_Effect;
	pst_STR->uc_NbCurEffects--;
	if(i < pst_STR->uc_NbCurEffects)
	{
		L_memcpy(STR_spst_CurEffect, &pst_STR->st_Effect[pst_STR->uc_NbCurEffects], sizeof(STR_tdst_Effect));
	}

	STR_spst_CurEffect = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_EffectDelAllForAString(STR_tdst_Struct *_pst_STR, int _i_String)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < _pst_STR->uc_NbCurEffects;)
	{
		if(_pst_STR->st_Effect[i].i_String == _i_String)
		{
			_pst_STR->uc_NbCurEffects--;
			if(i < _pst_STR->uc_NbCurEffects)
			{
				L_memcpy
				(
					&_pst_STR->st_Effect[i],
					&_pst_STR->st_Effect[_pst_STR->uc_NbCurEffects],
					sizeof(STR_tdst_Effect)
				);
			}
		}
		else
			i++;
	}

	STR_spst_CurEffect = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_EffectSeti(OBJ_tdst_GameObject *_pst_GO, int _i_Id, int _i_Param, int _i_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return;

	/* command */
	if(_i_Param == -1)
	{
		if(_i_Id == 0)
			STR_EffectDelAll(pst_STR);
		else if (_i_Id == 1) 
			STR_EffectDelAllForAString(pst_STR, _i_Value);
		return;
	}
	else if (_i_Param == -2)
	{
		if (_i_Id < 0)
		{
			for (_i_Id = 0; _i_Id < 24; _i_Id++ )
				pst_STR->aw_Interligne[ _i_Id ] = _i_Value;
		}
		else if (_i_Id < 24 )
			pst_STR->aw_Interligne[ _i_Id ] = _i_Value;
		return;
	}
	else if (_i_Param == -3)
	{
		if (_i_Id < 0)
		{
			for (_i_Id = 0; _i_Id < 24; _i_Id++ )
				pst_STR->aw_LetterSpacing[ _i_Id ] = 0;
		}
		else if (_i_Id < 24 )
			pst_STR->aw_LetterSpacing[ _i_Id ] = _i_Value;
		return;
	}


	STR_SetCurEffect(pst_STR, _i_Id);
	if(!STR_spst_CurEffect) return;

	/*$off*/
	switch (_i_Param)
	{
	/* general */
	case 0:			STR_spst_CurEffect->ul_Flags = (STR_spst_CurEffect->ul_Flags & 0xFF00FFFF) | ((_i_Value & 0xFF)<< 16);	break;
	case 1:			STR_spst_CurEffect->ul_Flags = (STR_spst_CurEffect->ul_Flags & 0xFFFF00FF) | ((_i_Value & 0xFF)<< 8);	break;

	/* blink */
	case 200: STR_spst_CurEffect->st_Blink.ul_Color1 = _i_Value;		break;
	case 201: STR_spst_CurEffect->st_Blink.ul_Color2= _i_Value;			break;

	/* gouraud */
	case 300 : STR_spst_CurEffect->st_Gouraud.aul_Color[0] = _i_Value;	break;
	case 301 : STR_spst_CurEffect->st_Gouraud.aul_Color[1] = _i_Value;	break;
	case 302 : STR_spst_CurEffect->st_Gouraud.aul_Color[2] = _i_Value;	break;
	case 303 : STR_spst_CurEffect->st_Gouraud.aul_Color[3] = _i_Value;	break;

	/* change color */
	case 400: STR_spst_CurEffect->st_Fade.ul_ColorSrc = _i_Value;		break;
	case 401: STR_spst_CurEffect->st_Fade.ul_ColorTgt = _i_Value;		break;

	/* flow */
	case 1600:STR_spst_CurEffect->st_Flow.i_Dir = _i_Value;				break;

	/* scroll = 21 */
	case 2100:STR_spst_CurEffect->st_Scroll.i_NbLine = _i_Value;		break;
	}
	/*$on*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_EffectGeti(OBJ_tdst_GameObject *_pst_GO, int _i_Id, int _i_Param )
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return -1;

	/* command */
	if(_i_Id == -1)
		return( STR_i_EffectGetId( pst_STR, _i_Param) );

	STR_SetCurEffect(pst_STR, _i_Id);
	if(!STR_spst_CurEffect) return -1;

	/*$off*/
	switch (_i_Param)
	{
	/* general */
	case 0:			return (STR_spst_CurEffect->ul_Flags >> 16) & 0xFF;	break;
	case 1:			return (STR_spst_CurEffect->ul_Flags >> 8) & 0xFF;	break;
	case 2:			return (STR_spst_CurEffect->ul_Flags >> 24) & 0xFF; break;
	}
	/*$on*/
	return -1;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_EffectSetf(OBJ_tdst_GameObject *_pst_GO, int _i_Id, int _i_Param, float _f_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	STR_tdst_String *pst_String;
	int				f, n;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return;

	STR_SetCurEffect(pst_STR, _i_Id);
	if(!STR_spst_CurEffect) return;

	/*$off*/
	switch (_i_Param)
	{
	/* fade */
	case 150: STR_spst_CurEffect->st_Fade.f_Time = _f_Value;			break;
	case 151: STR_spst_CurEffect->st_Fade.f_Delay = _f_Value;			break;

	/* blink */
	case 250: STR_spst_CurEffect->st_Blink.f_Time = _f_Value;			break;

	/* change color */
	case 450: STR_spst_CurEffect->st_Fade.f_Time = _f_Value;			break;
	case 451: STR_spst_CurEffect->st_Fade.f_Delay = _f_Value;			break;

	/* bounce */
	case 550: STR_spst_CurEffect->st_Bounce.f_Amplitude = _f_Value;		break;
	case 551: STR_spst_CurEffect->st_Bounce.f_TimeFactor = _f_Value;	break;
	case 552: STR_spst_CurEffect->st_Bounce.f_YFactor = _f_Value;		break;
	case 553: STR_spst_CurEffect->st_Bounce.f_Time = _f_Value;			break;

	/* perturbation */
	case 650: STR_spst_CurEffect->st_Perturb.fX = _f_Value;				break;
	case 651: STR_spst_CurEffect->st_Perturb.fY = _f_Value;				break;
	case 652: STR_spst_CurEffect->st_Perturb.f_Time = _f_Value;			break;

	/* shrink */
	case 750:	if (_f_Value < 0)
				{
					if( (pst_String = STR_pst_GetString(pst_STR, STR_spst_CurEffect->i_String)) ) 
					{
						f = STR_l_GetValidIndex(pst_STR, pst_String, STR_spst_CurEffect->i_From);
						n = STR_l_GetValidNumber(pst_STR, pst_String, f, STR_spst_CurEffect->i_Number);
						STR_spst_CurEffect->st_Shrink.f_Time = n * (-_f_Value );
					}
				}
				else
					STR_spst_CurEffect->st_Shrink.f_Time = _f_Value;			

				break;
	case 751: STR_spst_CurEffect->st_Shrink.f_LetterTime = _f_Value;	break;
	case 752: STR_spst_CurEffect->st_Shrink.f_Delay = _f_Value;
			  STR_spst_CurEffect->st_Shrink.f_CurTime = 0;
			  break;

	/* enlarge */
	case 1150: STR_spst_CurEffect->st_Enlarge.f_Time = _f_Value;		break;
	case 1151: STR_spst_CurEffect->st_Enlarge.f_Factor = _f_Value;		break;
	case 1152: STR_spst_CurEffect->st_Enlarge.f_Delay = _f_Value;		break;

	/* door */
	case 1350: STR_spst_CurEffect->st_Door.f_Time = _f_Value;			break;
	case 1351: STR_spst_CurEffect->st_Door.f_Ref = _f_Value;			break;

	/* rotate */
	case 1550: STR_spst_CurEffect->st_Rotate.f_Angle = _f_Value;		break;
	case 1551: STR_spst_CurEffect->st_Rotate.f_Speed = _f_Value;		break;

	/* flow */
	case 1650: STR_spst_CurEffect->st_Flow.f_Time = _f_Value;			break;
	case 1651: STR_spst_CurEffect->st_Flow.f_Delta = _f_Value;			break;

	/* cut */
	case 1750: STR_spst_CurEffect->st_Cut.min = _f_Value;				break;
	case 1751: STR_spst_CurEffect->st_Cut.max = _f_Value;	            break;

	/* italic */
	case 1850: STR_spst_CurEffect->st_Italic.value = _f_Value;			break;

	/* blur */
	case 2050: STR_spst_CurEffect->st_Blur.f_Time = _f_Value;			break;
	case 2051: STR_spst_CurEffect->st_Blur.f_Dist = _f_Value;			break;

	/* scroll */
	case 2150: STR_spst_CurEffect->st_Scroll.f_Time = _f_Value;			break;
	}
	/*$on*/
}

/*$4
 ***********************************************************************************************************************
    Pre - Treat
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_Effect_PreTreat(STR_tdst_Struct *_pst_STR, STR_tdst_Effect *_pst_Effect)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_String *pst_String;
	int				f, n;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_pst_Effect->ul_Flags & STR_Cul_EF_Type)
	{
	case STR_Cul_EF_Flow:
		if((pst_String = STR_pst_GetString(_pst_STR, _pst_Effect->i_String)) == NULL) return 0;
		if(pst_String->uw_Flags & STR_Cuw_SF_Hide) return 0;
		f = STR_l_GetValidIndex(_pst_STR, pst_String, _pst_Effect->i_From);
		n = STR_l_GetValidNumber(_pst_STR, pst_String, f, _pst_Effect->i_Number);
		while(f < n) _pst_STR->pul_Sprite[pst_String->uw_First + f++] = 1;
		return n;

	case STR_Cul_EF_Blur:
		if((pst_String = STR_pst_GetString(_pst_STR, _pst_Effect->i_String)) == NULL) return 0;
		if(pst_String->uw_Flags & STR_Cuw_SF_Hide) return 0;
		f = STR_l_GetValidIndex(_pst_STR, pst_String, _pst_Effect->i_From);
		n = STR_l_GetValidNumber(_pst_STR, pst_String, f, _pst_Effect->i_Number);
		while(f < n) _pst_STR->pul_Sprite[pst_String->uw_First + f++] = 4;
		return (4 * n);
	}
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_Effect_PreTreatAll(STR_tdst_Struct *_pst_STR)
{
	/*~~~~~~~*/
	int i, res;
	/*~~~~~~~*/

	res = 0;
	L_memset(_pst_STR->pul_Sprite, 0, 4 * _pst_STR->ul_NbMaxLetters);
	for(i = 0; i < _pst_STR->uc_NbCurEffects; i++) res += STR_l_Effect_PreTreat(_pst_STR, &_pst_STR->st_Effect[i]);
	return res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_Effect_CountAdditionalSprite(STR_tdst_Struct *_pst_STR)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i, res;
	STR_tdst_String *pst_String;
	int				f, n;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	res = 0;
	for(i = 0; i < _pst_STR->uc_NbCurEffects; i++)
	{
		switch(_pst_STR->st_Effect[i].ul_Flags & STR_Cul_EF_Type)
		{
		case STR_Cul_EF_Flow:
			if((pst_String = STR_pst_GetString(_pst_STR, _pst_STR->st_Effect[i].i_String)) == NULL) break;;
			if(pst_String->uw_Flags & STR_Cuw_SF_Hide) break;;
			f = STR_l_GetValidIndex(_pst_STR, pst_String, _pst_STR->st_Effect[i].i_From);
			n = STR_l_GetValidNumber(_pst_STR, pst_String, f, _pst_STR->st_Effect[i].i_Number);
			res += n;
			break;
		case STR_Cul_EF_Blur:
			if((pst_String = STR_pst_GetString(_pst_STR, _pst_STR->st_Effect[i].i_String)) == NULL) break;;
			if(pst_String->uw_Flags & STR_Cuw_SF_Hide) break;;
			f = STR_l_GetValidIndex(_pst_STR, pst_String, _pst_STR->st_Effect[i].i_From);
			n = STR_l_GetValidNumber(_pst_STR, pst_String, f, _pst_STR->st_Effect[i].i_Number);
			res += 4 * n;
			break;
		}
	}

	return res;
}

/*$4
 ***********************************************************************************************************************
    Treat
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_Effect_Treat(STR_tdst_Struct *_pst_STR, STR_tdst_Effect *_pst_Effect)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Color, *pul_Color, C0, C1, C2;
	STR_tdst_String *pst_String;
	int				i, j0, j1, j2, j3, f, n, sprite, lastsprite;
	int				framesprite, framenbsprite;
	STR_tdst_Letter *pst_Letter, *pst_Last, *pst_First;
	GEO_Vertex		*V, *VLast, *VFrame;
	GEO_tdst_UV		*pst_UV;
	float			x, y, w, h, y0, y1, v0, v1, min, max, x0, x1, x2, x3, x4, x5;
	float			fval, fact, fact2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((pst_String = STR_pst_GetString(_pst_STR, _pst_Effect->i_String)) == NULL) return STR_Cul_ER_Terminated;

	if(pst_String->uw_Flags & STR_Cuw_SF_Hide) return 0;

	f = STR_l_GetValidIndex(_pst_STR, pst_String, _pst_Effect->i_From);
	n = STR_l_GetValidNumber(_pst_STR, pst_String, f, _pst_Effect->i_Number);

	pst_Letter = _pst_STR->dst_Letter + pst_String->uw_First + f;
	pst_Last = pst_Letter + n;

	sprite = _pst_STR->pul_Sprite[pst_Letter - _pst_STR->dst_Letter] << 2;
	lastsprite = (_pst_STR->pul_Sprite[(pst_Last - _pst_STR->dst_Letter) - 1] + 1) << 2;

	//if ( lastsprite == 4) 
	//	return STR_Cul_ER_Terminated;

	// effect frame
	if ( (_pst_Effect->ul_Flags & STR_Cul_EF_ApplyToFrame ) && (pst_String->uw_Flags & STR_Cuw_SF_ShowFrame ) )
	{
		if (_pst_Effect->ul_Flags & STR_Cul_EF_ApplyJustToFrame )
		{
			pst_Last = pst_Letter;
			lastsprite = sprite;
		}
	
		framenbsprite = 9 << 2;
		framesprite = pst_String->st_Frame.ul_Sprite << 2;
	}
	else
	{
		framenbsprite = 0;
		if (n == 0 ) return STR_Cul_ER_Terminated;
	}


	switch(_pst_Effect->ul_Flags & STR_Cul_EF_Type)
	{
	case STR_Cul_EF_Fade2Black:
		if ( _pst_Effect->st_Fade.f_CurTime < _pst_Effect->st_Fade.f_Delay )
			x = 0;
		else
			x = (_pst_Effect->st_Fade.f_CurTime - _pst_Effect->st_Fade.f_Delay) / _pst_Effect->st_Fade.f_Time;
		
		if ( _pst_Effect->ul_Flags & STR_Cul_EF_Fade2Black_Invert )
			x = 1 - x;
		
		pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + sprite;
		while(pst_Letter < pst_Last)
		{
			ul_Color = COLOR_ul_Blend(*pul_Color, 0, x);
			*pul_Color++ = ul_Color;
			*pul_Color++ = ul_Color;
			*pul_Color++ = ul_Color;
			*pul_Color++ = ul_Color;
			pst_Letter++;
		}

		pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + framesprite;
		while ( framenbsprite-- ) 
		{
			ul_Color = COLOR_ul_Blend(*pul_Color, 0, x);
			*pul_Color++ = ul_Color;
		}

		_pst_Effect->st_Fade.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Fade.f_CurTime >= (_pst_Effect->st_Fade.f_Time + _pst_Effect->st_Fade.f_Delay) ) 
		{
			if ( !(_pst_Effect->ul_Flags & STR_Cul_EF_Fade2Black_Invert) )
				pst_String->uw_Flags |= STR_Cuw_SF_Hide;
			return STR_Cul_ER_Terminated;
		}
		break;

	case STR_Cul_EF_Blink:
		if(_pst_Effect->ul_Flags & STR_Cul_EF_Blink_Smooth)
		{
			x = _pst_Effect->st_Fade.f_CurTime / _pst_Effect->st_Fade.f_Time;
			if(_pst_Effect->ul_Flags & STR_Cul_EF_Blink_Up) x = 1 - x;
			ul_Color = COLOR_ul_Blend(_pst_Effect->st_Blink.ul_Color1, _pst_Effect->st_Blink.ul_Color2, x);
		}
		else
		{
			ul_Color =
				(_pst_Effect->ul_Flags & STR_Cul_EF_Blink_Up)
					? _pst_Effect->st_Blink.ul_Color1 : _pst_Effect->st_Blink.ul_Color2;
		}

		pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + sprite;
		while(pst_Letter < pst_Last)
		{
			*pul_Color++ = ul_Color;
			*pul_Color++ = ul_Color;
			*pul_Color++ = ul_Color;
			*pul_Color++ = ul_Color;
			pst_Letter++;
		}

		_pst_Effect->st_Fade.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Fade.f_CurTime >= _pst_Effect->st_Fade.f_Time)
		{
			_pst_Effect->st_Fade.f_CurTime = 0;
			if(_pst_Effect->ul_Flags & STR_Cul_EF_Blink_Up)
				_pst_Effect->ul_Flags &= ~STR_Cul_EF_Blink_Up;
			else
				_pst_Effect->ul_Flags |= STR_Cul_EF_Blink_Up;
			_pst_Effect->ul_Flags |= STR_Cul_EF_Blink_Change;
		}
		else
			_pst_Effect->ul_Flags &= ~STR_Cul_EF_Blink_Change;
		break;
	case STR_Cul_EF_Gouraud:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~*/
			ULONG	c0, c1, *pul_Color;
			/*~~~~~~~~~~~~~~~~~~~~~~~*/

			V = &GFX_gpst_Geo->dst_Point[sprite];
			VLast = &GFX_gpst_Geo->dst_Point[lastsprite];
			pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + sprite;

			x = V->x;
			y = V->y;
			w = (VLast - 2)->x - x;
			h = (VLast - 2)->y - y;
			if((w == 0) || (h == 0)) return 0;
			w = 1 / w;
			h = 1 / h;

			while(V < VLast)
			{
				fact = (V->x - x) * w;
				c0 = COLOR_ul_Blend(_pst_Effect->st_Gouraud.aul_Color[0], _pst_Effect->st_Gouraud.aul_Color[1], fact);
				c1 = COLOR_ul_Blend(_pst_Effect->st_Gouraud.aul_Color[3], _pst_Effect->st_Gouraud.aul_Color[2], fact);
				fact = (V->y - y) * h;
				*pul_Color++ = COLOR_ul_Blend(c0, c1, fact);
				V++;
			}
			break;
		}

	case STR_Cul_EF_ChangeColor:
		if(_pst_Effect->st_Fade.f_CurTime < _pst_Effect->st_Fade.f_Delay)
			x = 0;
		else
			x = (_pst_Effect->st_Fade.f_CurTime - _pst_Effect->st_Fade.f_Delay) / _pst_Effect->st_Fade.f_Time;
		ul_Color = COLOR_ul_Blend(_pst_Effect->st_Fade.ul_ColorSrc, _pst_Effect->st_Fade.ul_ColorTgt, x);
		pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + sprite;

		if (_pst_Effect->ul_Flags & STR_Cul_EF_ChangeColor_Apply)
		{
			while(pst_Letter < pst_Last)
			{
				*pul_Color++ = ul_Color;
				*pul_Color++ = ul_Color;
				*pul_Color++ = ul_Color;
				*pul_Color++ = ul_Color;
				pst_Letter->ul_Color = ul_Color;
				pst_Letter++;
			}
		}
		else
		{
			while(pst_Letter < pst_Last)
			{
				*pul_Color++ = ul_Color;
				*pul_Color++ = ul_Color;
				*pul_Color++ = ul_Color;
				*pul_Color++ = ul_Color;
				pst_Letter++;
			}
		}

		pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + framesprite;
		while ( framenbsprite-- ) 
				*pul_Color++ = ul_Color;
		if (_pst_Effect->ul_Flags & STR_Cul_EF_ChangeColor_Apply )
			pst_String->st_Frame.ul_Color = ul_Color;

		_pst_Effect->st_Fade.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Fade.f_CurTime >= (_pst_Effect->st_Fade.f_Time + _pst_Effect->st_Fade.f_Delay))
		{
			if((_pst_Effect->st_Fade.ul_ColorTgt & 0xFFFFFF) == 0) pst_String->uw_Flags |= STR_Cuw_SF_Hide;
			return STR_Cul_ER_Terminated;
		}
		break;

	case STR_Cul_EF_Bounce:
		VLast = &GFX_gpst_Geo->dst_Point[lastsprite];
		V = &GFX_gpst_Geo->dst_Point[sprite];
		fact = _pst_Effect->st_Bounce.f_Time - _pst_Effect->st_Bounce.f_CurTime;
		fact2 = fact * fact * _pst_Effect->st_Bounce.f_Amplitude * C_Pos2DFactorY;
		fact *= _pst_Effect->st_Bounce.f_TimeFactor;

		while(V < VLast)
		{
			fval = (V->x + V[2].x) / 2;
			fval = fact2 * fAbs(fOptSin(fact + _pst_Effect->st_Bounce.f_YFactor * fval));
			(V++)->y -= fval;
			(V++)->y -= fval;
			(V++)->y -= fval;
			(V++)->y -= fval;
		}

		_pst_Effect->st_Bounce.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Bounce.f_CurTime >= _pst_Effect->st_Bounce.f_Time) return STR_Cul_ER_Terminated;

		break;

	case STR_Cul_EF_Perturb:
		VLast = &GFX_gpst_Geo->dst_Point[lastsprite];
		V = &GFX_gpst_Geo->dst_Point[sprite];

		fact = 1 - (_pst_Effect->st_Perturb.f_CurTime / _pst_Effect->st_Perturb.f_Time);
		fact *= fact;

		while(V < VLast)
		{
			fval = fact * C_Pos2DFactorX * fRand(-_pst_Effect->st_Perturb.fX, _pst_Effect->st_Perturb.fX);
			fact2 = fact * C_Pos2DFactorY * fRand(-_pst_Effect->st_Perturb.fY, _pst_Effect->st_Perturb.fY);

			V->x += fval;
			(V++)->y -= fact2;
			V->x += fval;
			(V++)->y -= fact2;
			V->x += fval;
			(V++)->y -= fact2;
			V->x += fval;
			(V++)->y -= fact2;
		}

		if(_pst_Effect->ul_Flags & (STR_Cul_EF_PerturbFadeIn | STR_Cul_EF_PerturbFadeOut))
		{
			fact = 2 * _pst_Effect->st_Perturb.f_CurTime / _pst_Effect->st_Perturb.f_Time;
			if(_pst_Effect->ul_Flags & STR_Cul_EF_PerturbFadeOut) fact = 2 - fact;
			if(fact > 1) fact = 1;
			ul_Color = COLOR_ul_Blend(0, 0xFFFFFFFF, fact);
			pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + sprite;
			while(pst_Letter < pst_Last)
			{
				*pul_Color++ = ul_Color;
				*pul_Color++ = ul_Color;
				*pul_Color++ = ul_Color;
				*pul_Color++ = ul_Color;
				pst_Letter++;
			}
		}

		_pst_Effect->st_Perturb.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Perturb.f_CurTime >= _pst_Effect->st_Perturb.f_Time) return STR_Cul_ER_Terminated;

		break;

	case STR_Cul_EF_Shrink:
		i = 0;
		V = &GFX_gpst_Geo->dst_Point[sprite];
		if(_pst_Effect->st_Shrink.f_CurTime < _pst_Effect->st_Shrink.f_Delay)
		{
			_pst_Effect->st_Shrink.f_CurTime += TIM_gf_dt;
			break;
		}

		fact = ((_pst_Effect->st_Shrink.f_Time - 0.05f) - _pst_Effect->st_Shrink.f_LetterTime) / n;
		while(pst_Letter < pst_Last)
		{
			fact2 = (_pst_Effect->st_Shrink.f_CurTime - _pst_Effect->st_Shrink.f_Delay) - (i * fact);
			if(fact2 <= 0) break;

			fact2 /= _pst_Effect->st_Shrink.f_LetterTime;
			if(fact2 >= 1) fact2 = 1;
			fact2 = 1 - fact2;

			y = (V[1].y + V->y) / 2;
			x = (V[1].y - V->y) / 2;
			V->y = V[3].y = y - (x * fact2);
			V[1].y = V[2].y = y + (x * fact2);

			V += 4;
			pst_Letter++;
			i++;
		}

		_pst_Effect->st_Shrink.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Shrink.f_CurTime >= (_pst_Effect->st_Shrink.f_Time + _pst_Effect->st_Shrink.f_Delay))
		{
			pst_String->uw_Flags |= STR_Cuw_SF_Hide;
			return STR_Cul_ER_Terminated;
		}
		break;

	case STR_Cul_EF_Unshrink:
		i = 0;
		V = &GFX_gpst_Geo->dst_Point[sprite];
		fact = ((_pst_Effect->st_Shrink.f_Time - 0.05f) - _pst_Effect->st_Shrink.f_LetterTime) / n;
		while(pst_Letter < pst_Last)
		{
			fact2 = ((_pst_Effect->st_Shrink.f_CurTime - _pst_Effect->st_Shrink.f_Delay) - (i * fact)) / _pst_Effect->st_Shrink.f_LetterTime;
			if(fact2 < 1)
			{
				if(fact2 < 0) fact2 = 0;
				y = (V[1].y + V->y) / 2;
				x = (V[1].y - V->y) / 2;
				V->y = V[3].y = y - (x * fact2);
				V[1].y = V[2].y = y + (x * fact2);
			}

			V += 4;
			pst_Letter++;
			i++;
		}

		_pst_Effect->st_Shrink.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Shrink.f_CurTime >= (_pst_Effect->st_Shrink.f_Time + _pst_Effect->st_Shrink.f_Delay))
			return STR_Cul_ER_Terminated;
		break;

	case STR_Cul_EF_ShrinkCenter:
	case STR_Cul_EF_UnshrinkCenter:
		V = &GFX_gpst_Geo->dst_Point[sprite];
		VLast = &GFX_gpst_Geo->dst_Point[lastsprite];
		x = w = V->x;
		y = h = V->y;
		while(V < VLast)
		{
			if(V->x < x) x = V->x;
			if(V->y < y) y = V->y;
			if(V->x > w) w = V->x;
			if(V->y > h) h = V->y;
			V++;
		}

		fact = (_pst_Effect->st_Shrink.f_CurTime - _pst_Effect->st_Shrink.f_Delay) / _pst_Effect->st_Shrink.f_Time;
		if (fact < 0) fact = 0;
		fact *= fact;
		if((_pst_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_ShrinkCenter) fact = 1 - fact;

		x = (x + w) / 2;
		y = (y + h) / 2;
		V = &GFX_gpst_Geo->dst_Point[sprite];
		while(V < VLast)
		{
			V->x = x + ((V->x - x) * fact);
			V->y = y + ((V->y - y) * fact);
			V++;
		}

		_pst_Effect->st_Shrink.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Shrink.f_CurTime >= (_pst_Effect->st_Shrink.f_Time + _pst_Effect->st_Shrink.f_Delay) )
		{
			if((_pst_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_ShrinkCenter)
				pst_String->uw_Flags |= STR_Cuw_SF_Hide;
			return STR_Cul_ER_Terminated;
		}
		break;

	case STR_Cul_EF_Enlarge:
	case STR_Cul_EF_Unenlarge:
		V = &GFX_gpst_Geo->dst_Point[sprite];
		VLast = &GFX_gpst_Geo->dst_Point[lastsprite];
		x = w = V->x;
		y = h = V->y;
		while(V < VLast)
		{
			if(V->x < x) x = V->x;
			if(V->y < y) y = V->y;
			if(V->x > w) w = V->x;
			if(V->y > h) h = V->y;
			V++;
		}

		if(_pst_Effect->st_Enlarge.f_CurTime < _pst_Effect->st_Enlarge.f_Delay)
		{
			if((_pst_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_Unenlarge)
			{
				V = &GFX_gpst_Geo->dst_Point[sprite];
				while(V < VLast)
				{
					V->x = V->y = -1;
					V++;
				}
				V = GFX_gpst_Geo->dst_Point + framesprite;
				VLast = &GFX_gpst_Geo->dst_Point[framesprite + framenbsprite];
				while ( V < VLast) 
				{
					V->x = V->y = -1;
					V++;
				}
			}

			_pst_Effect->st_Enlarge.f_CurTime += TIM_gf_dt;
			break;
		}

		fact = (_pst_Effect->st_Enlarge.f_CurTime - _pst_Effect->st_Enlarge.f_Delay) / _pst_Effect->st_Enlarge.f_Time;
		fact *= fact;
		if((_pst_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_Unenlarge) fact = 1 - fact;
		fact2 = 1 + (_pst_Effect->st_Enlarge.f_Factor - 1) * fact;

		x = (x + w) / 2;
		y = (y + h) / 2;
		V = &GFX_gpst_Geo->dst_Point[sprite];
		ul_Color = COLOR_ul_Blend(GFX_gpst_Geo->dul_PointColors[1 + sprite], 0, fact);
		pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + sprite;
		while(V < VLast)
		{
			V->x = x + ((V->x - x) * fact2);
			V->y = y + ((V->y - y) * fact2);
			V++;
			*pul_Color++ = ul_Color;
		}

		if (framenbsprite)
		{
			V = GFX_gpst_Geo->dst_Point + framesprite;
			VLast = &GFX_gpst_Geo->dst_Point[framesprite + framenbsprite];
			ul_Color = COLOR_ul_Blend(GFX_gpst_Geo->dul_PointColors[1 + framesprite], 0, fact);
			pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + framesprite;
			while ( V < VLast) 
			{
				V->x = x + ((V->x - x) * fact2);
				V->y = y + ((V->y - y) * fact2);
				V++;
				*pul_Color++ = ul_Color;
			}
		}

		_pst_Effect->st_Enlarge.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Enlarge.f_CurTime >= _pst_Effect->st_Enlarge.f_Time + _pst_Effect->st_Enlarge.f_Delay)
		{
			if((_pst_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_Enlarge)
				pst_String->uw_Flags |= STR_Cuw_SF_Hide;
			return STR_Cul_ER_Terminated;
		}
		break;

	case STR_Cul_EF_Door:
	case STR_Cul_EF_Undoor:
		V = &GFX_gpst_Geo->dst_Point[sprite];
		VLast = &GFX_gpst_Geo->dst_Point[lastsprite];

		fact = _pst_Effect->st_Door.f_CurTime / _pst_Effect->st_Door.f_Time;
		if((_pst_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_Undoor) fact = 1 - fact;
		fact2 = fOptCos(fact * Cf_PiBy2);
		fact = fOptSin(fact * Cf_PiBy2);

		while(V < VLast)
		{
			V->y = _pst_Effect->st_Door.f_Ref + ((V->y - _pst_Effect->st_Door.f_Ref) * fact);
			V->z += fAbs((V->y - _pst_Effect->st_Door.f_Ref)) * fact2;
			V++;
		}

		_pst_Effect->st_Door.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Door.f_CurTime >= _pst_Effect->st_Door.f_Time)
		{
			if((_pst_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_Undoor)
				pst_String->uw_Flags |= STR_Cuw_SF_Hide;
			return STR_Cul_ER_Terminated;
		}
		break;

	case STR_Cul_EF_Rotate:
		V = &GFX_gpst_Geo->dst_Point[sprite];
		VLast = &GFX_gpst_Geo->dst_Point[lastsprite];
		x = w = V->x;
		y = h = V->y;
		while(V < VLast)
		{
			if(V->x < x) x = V->x;
			if(V->y < y) y = V->y;
			if(V->x > w) w = V->x;
			if(V->y > h) h = V->y;
			V++;
		}

		x = (x + w) / 2;
		y = (y + h) / 2;

#if (defined(PSX2_TARGET) || defined(_GAMECUBE) )
		fact = fOptSin(-_pst_Effect->st_Rotate.f_Angle);
		fact2 = fOptCos(-_pst_Effect->st_Rotate.f_Angle);
#else
		fact = fOptSin(_pst_Effect->st_Rotate.f_Angle);
		fact2 = fOptCos(_pst_Effect->st_Rotate.f_Angle);
#endif
		_pst_Effect->st_Rotate.f_Angle += _pst_Effect->st_Rotate.f_Speed * TIM_gf_dt;

		V = &GFX_gpst_Geo->dst_Point[sprite];
		while(V < VLast)
		{
			V->x -= x;
			V->y -= y;

			w = V->x * fact2 - V->y * fact;
			h = V->y * fact2 + V->x * fact;

			V->x = w + x;
			V->y = h + y;
			V++;
		}

		V = GFX_gpst_Geo->dst_Point + framesprite;
		VLast = &GFX_gpst_Geo->dst_Point[framesprite + framenbsprite];
		while ( V < VLast) 
		{
			V->x -= x;
			V->y -= y;

			w = V->x * fact2 - V->y * fact;
			h = V->y * fact2 + V->x * fact;

			V->x = w + x;
			V->y = h + y;
			V++;
		}
		break;

	case STR_Cul_EF_Flow:
		y = h = GFX_gpst_Geo->dst_Point[sprite].y;
		x = w = GFX_gpst_Geo->dst_Point[sprite].x;
		for(i = 0; i < n; i++)
		{
			V = &GFX_gpst_Geo->dst_Point[_pst_STR->pul_Sprite[i + pst_String->uw_First + f] << 2];
			if(V->y > h) h = V->y;
			if((V + 1)->y < y) y = (V + 1)->y;
			if((V + 2)->x > w) w = (V + 2)->x;
			if(V->x < x) x = V->x;
		}

		h -= y;
		w -= x;
        //_pst_Effect->st_Flow.f_CurTime = 1;
		fact = (_pst_Effect->st_Flow.f_CurTime / _pst_Effect->st_Flow.f_Time);

		if((_pst_Effect->st_Flow.i_Dir == 0) || (_pst_Effect->st_Flow.i_Dir == 1))
		{
			j0 = _pst_Effect->st_Flow.i_Dir;
			j1 = 1 - j0;
			j2 = j0 ? 2 : 3;
			j3 = j0 ? 3 : 2;
			y = y + (j0 ? (1 - fact) : fact) * h;
			for(i = 0; i < n; i++)
			{
				V = &GFX_gpst_Geo->dst_Point[_pst_STR->pul_Sprite[i + pst_String->uw_First + f] << 2];
				if((V + 1)->y > y)
				{
					if(j1)
					{
						(V + 0)->x = (V + 1)->x = (V + 2)->x = (V + 3)->x = -100.0f;
						(V + 0)->y = (V + 1)->y = (V + 2)->y = (V + 3)->y = -100.0f;
					}
					(V + 4)->x = (V + 5)->x = (V + 6)->x = (V + 7)->x = -100.0f;
					(V + 4)->y = (V + 5)->y = (V + 6)->y = (V + 7)->y = -100.0f;
				}
				else if(V->y < y)
				{
					if(j0)
					{
						(V + 0)->x = (V + 1)->x = (V + 2)->x = (V + 3)->x = -100.0f;
						(V + 0)->y = (V + 1)->y = (V + 2)->y = (V + 3)->y = -100.0f;
					}
					(V + 4)->x = (V + 5)->x = (V + 6)->x = (V + 7)->x = -100.0f;
					(V + 4)->y = (V + 5)->y = (V + 6)->y = (V + 7)->y = -100.0f;
				}
				else
				{
					pst_UV = &GFX_gpst_Geo->dst_UV[_pst_STR->pul_Sprite[i + pst_String->uw_First + f] << 2];
					fact = (y - (V + j1)->y) / ((V + j0)->y - (V + j1)->y);
					(pst_UV +
						j0)->fV = (pst_UV + j2)->fV = (pst_UV + j1)->fV +
						((pst_UV + j0)->fV - (pst_UV + j1)->fV) *
						fact;
					(V + j0)->y = (V + j2)->y = y;
					(pst_UV + 4)->fV = (pst_UV + 5)->fV = (pst_UV + 6)->fV = (pst_UV + 7)->fV = (pst_UV + j0)->fV;
					(V + j0 + 4)->y = (V + j2 + 4)->y = y;
					(V + j1 + 4)->y = (V + j3 + 4)->y = (float) ((j0) ? 0 : _pst_STR->uw_SH);
					(V + j1 + 4)->x += _pst_Effect->st_Flow.f_Delta;
					(V + j3 + 4)->x += _pst_Effect->st_Flow.f_Delta;
				}
			}
		}
		else if(_pst_Effect->st_Flow.i_Dir == 2)
		{
			x = x + fact * w;
			for(i = 0; i < n; i++)
			{
				V = &GFX_gpst_Geo->dst_Point[_pst_STR->pul_Sprite[i + pst_String->uw_First + f] << 2];
				if((V + 3)->x < x)
				{
					(V + 4)->x = (V + 5)->x = (V + 6)->x = (V + 7)->x = -100.0f;
					(V + 4)->y = (V + 5)->y = (V + 6)->y = (V + 7)->y = -100.0f;
				}
				else if(V->x > x)
				{
					(V + 0)->x = (V + 1)->x = (V + 2)->x = (V + 3)->x = -100.0f;
					(V + 4)->x = (V + 5)->x = (V + 6)->x = (V + 7)->x = -100.0f;
					(V + 0)->y = (V + 1)->y = (V + 2)->y = (V + 3)->y = -100.0f;
					(V + 4)->y = (V + 5)->y = (V + 6)->y = (V + 7)->y = -100.0f;
				}
				else
				{
					pst_UV = &GFX_gpst_Geo->dst_UV[_pst_STR->pul_Sprite[i + pst_String->uw_First + f] << 2];
					fact = (x - V->x) / ((V + 3)->x - V->x);
					(pst_UV + 2)->fU = (pst_UV + 3)->fU = pst_UV->fU + ((pst_UV + 3)->fU - pst_UV->fU) * fact;
					(V + 2)->x = (V + 3)->x = x;
					(pst_UV + 4)->fU = (pst_UV + 5)->fU = (pst_UV + 6)->fU = (pst_UV + 7)->fU = (pst_UV + 2)->fU;
					(V + 4)->x = (V + 5)->x = x;
					(V + 6)->x = (V + 7)->x = _pst_STR->uw_SW;
					(V + 6)->y += _pst_Effect->st_Flow.f_Delta;
					(V + 7)->y += _pst_Effect->st_Flow.f_Delta;
				}
			}
		}
		else if(_pst_Effect->st_Flow.i_Dir == 3)
		{
			x = x + (1 - fact) * w;
			for(i = 0; i < n; i++)
			{
				V = &GFX_gpst_Geo->dst_Point[_pst_STR->pul_Sprite[i + pst_String->uw_First + f] << 2];
				if((V + 3)->x < x)
				{
					(V + 0)->x = (V + 1)->x = (V + 2)->x = (V + 3)->x = -100.0f;
					(V + 4)->x = (V + 5)->x = (V + 6)->x = (V + 7)->x = -100.0f;
					(V + 0)->y = (V + 1)->y = (V + 2)->y = (V + 3)->y = -100.0f;
					(V + 4)->y = (V + 5)->y = (V + 6)->y = (V + 7)->y = -100.0f;
				}
				else if(V->x > x)
				{
					(V + 4)->x = (V + 5)->x = (V + 6)->x = (V + 7)->x = -100.0f;
					(V + 4)->y = (V + 5)->y = (V + 6)->y = (V + 7)->y = -100.0f;
				}
				else
				{
					pst_UV = &GFX_gpst_Geo->dst_UV[_pst_STR->pul_Sprite[i + pst_String->uw_First + f] << 2];
					fact = (x - (V + 3)->x) / (V->x - (V + 3)->x);
					pst_UV->fU = (pst_UV + 1)->fU = (pst_UV + 3)->fU + (pst_UV->fU - (pst_UV + 3)->fU) * fact;
					V->x = (V + 1)->x = x;
					(pst_UV + 4)->fU = (pst_UV + 5)->fU = (pst_UV + 6)->fU = (pst_UV + 7)->fU = pst_UV->fU;
					(V + 4)->x = (V + 5)->x = 0;
					(V + 6)->x = (V + 7)->x = x;
					(V + 4)->y += _pst_Effect->st_Flow.f_Delta;
					(V + 5)->y += _pst_Effect->st_Flow.f_Delta;
				}
			}
		}

		_pst_Effect->st_Flow.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Flow.f_CurTime >= _pst_Effect->st_Flow.f_Time) return STR_Cul_ER_Terminated;
		break;

	case STR_Cul_EF_Cut:
		V = &GFX_gpst_Geo->dst_Point[sprite];
		VLast = &GFX_gpst_Geo->dst_Point[lastsprite];
		min = (1 - _pst_Effect->st_Cut.max) * C_Pos2DFactorY;
		max = (1 - _pst_Effect->st_Cut.min) * C_Pos2DFactorY;
		while(V < VLast)
		{
			if(((V + 1)->y > max) || (V->y < min))
			{
				(V + 0)->x = (V + 1)->x = (V + 2)->x = (V + 3)->x = -100.0f;
				(V + 0)->y = (V + 1)->y = (V + 2)->y = (V + 3)->y = -100.0f;
			}
			else if((V->y > max) || ((V + 1)->y < min))
			{
				y0 = (V->y > max) ? max : V->y;
				y1 = ((V + 1)->y < min) ? min : (V + 1)->y;

				pst_UV = &GFX_gpst_Geo->dst_UV[V - GFX_gpst_Geo->dst_Point];
				v0 = pst_UV->fV + ((pst_UV + 1)->fV - pst_UV->fV) * ((y0 - V->y) / ((V + 1)->y - V->y));
				v1 = pst_UV->fV + ((pst_UV + 1)->fV - pst_UV->fV) * ((y1 - V->y) / ((V + 1)->y - V->y));

				V->y = (V + 3)->y = y0;
				pst_UV->fV = (pst_UV + 3)->fV = v0;
				(V + 1)->y = (V + 2)->y = y1;
				(pst_UV + 1)->fV = (pst_UV + 2)->fV = v1;
			}

			V += 4;
		}
		break;

	case STR_Cul_EF_Italic:
		VLast = &GFX_gpst_Geo->dst_Point[lastsprite];
		V = &GFX_gpst_Geo->dst_Point[sprite];

		while(V < VLast)
		{
			(V++)->x += _pst_Effect->st_Italic.value;
			V+=2;
			(V++)->x += _pst_Effect->st_Italic.value;
		}
		break;

	case STR_Cul_EF_Joy:
		_pst_Effect->st_Joy.f_CurTime -= TIM_gf_dt;
		if (_pst_Effect->st_Joy.f_CurTime < 0)
		{
			_pst_Effect->st_Joy.f_CurTime = _pst_Effect->st_Joy.f_Time;

			i = (_pst_Effect->st_Joy.i_Dir >> (4 * _pst_Effect->st_Joy.c_CurDir )) & 0xF;
			if (i == 0 ) 
			{
				_pst_Effect->st_Joy.c_CurDir = 1;
				i = _pst_Effect->st_Joy.i_Dir & 0xF;
			}
			else
				_pst_Effect->st_Joy.c_CurDir++;
			
			if ( _pst_Effect->st_Joy.c_Value == 0)
				_pst_Effect->st_Joy.c_Value = (char) (pst_Letter->ul_Flags & 0xFF);
			if ( i == 1 )
				_pst_Effect->st_Joy.f_CurTime /= 2;
			
			pst_Letter->ul_Flags &= 0xFFFFFF00;
			pst_Letter->ul_Flags |= _pst_Effect->st_Joy.c_Value + (i - 1);
		}
		break;

	case STR_Cul_EF_Blur:
		fact = (_pst_Effect->st_Blur.f_CurTime / _pst_Effect->st_Blur.f_Time);
		fact2 = (_pst_STR->uw_SW * _pst_Effect->st_Blur.f_Dist);
		for(i = 0; i < n; i++)
		{
			V = &GFX_gpst_Geo->dst_Point[_pst_STR->pul_Sprite[ i + pst_String->uw_First + f] << 2];

			x = (V[2].x - V[0].x) * 0.2f;
			w = (V[2].x + V[0].x) * 0.5f;
			y = V[0].x;
			x0 = y + ( (y - w) * 2 * fact * fact2 );
			y += x;
			x1 = y + ( (y - w) * fact * fact2 );
			y += x;
			x2 = y + ( (y - w) * fact * fact * fact2 );
			y += x;
			x3 = y + ( (y - w) * fact * fact * fact2 );
			y += x;
			x4 = y + ( (y - w) * fact * fact2 );
			y += x;
			x5 = y + ( (y - w) * 2 * fact * fact2 );

			V[ 0].x = V[ 1].x = x0;
			V[ 2].x = V[ 3].x = V[ 4].x = V[ 5].x = x1;
			V[ 6].x = V[ 7].x = V[ 8].x = V[ 9].x = x2;
			V[10].x = V[11].x = V[12].x = V[13].x = x3;
			V[14].x = V[15].x = V[16].x = V[17].x = x4;
			V[18].x = V[19].x = x5;

			pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + (_pst_STR->pul_Sprite[ i + pst_String->uw_First + f] << 2);
			//C0 = (fact < 0.7) ? *pul_Color : COLOR_ul_Blend( *pul_Color, 0, (fact - 0.7f) / 0.3f );
			//C1 = (fact < 0.4) ? *pul_Color : COLOR_ul_Blend( *pul_Color, 0, (fact - 0.4f) / 0.6f );
			//C2 = COLOR_ul_Blend( *pul_Color, 0, fact * fact );
			C0 = COLOR_ul_Blend( *pul_Color, 0, fact * fact );
			C1 = COLOR_ul_Blend( *pul_Color, 0, fact );
			C2 = (2 * fact >= 1) ? 0 : COLOR_ul_Blend( *pul_Color, 0, 2 * fact );

			pul_Color[ 0] = pul_Color[ 1] = C2;
			pul_Color[ 2] = pul_Color[ 3] = pul_Color[ 4] = pul_Color[ 5] = C1;
			pul_Color[ 6] = pul_Color[ 7] = pul_Color[ 8] = pul_Color[ 9] = C0;
			pul_Color[10] = pul_Color[11] = pul_Color[12] = pul_Color[13] = C0;
			pul_Color[14] = pul_Color[15] = pul_Color[16] = pul_Color[17] = C1;
			pul_Color[18] = pul_Color[19] = C2;

			pst_UV = &GFX_gpst_Geo->dst_UV[_pst_STR->pul_Sprite[ i + pst_String->uw_First + f] << 2];
			x = (pst_UV[2].fU - pst_UV[0].fU) * 0.2f;
			pst_UV[ 2].fU = pst_UV[ 3].fU = pst_UV[ 4].fU = pst_UV[ 5].fU = pst_UV[0].fU + x;
			pst_UV[ 6].fU = pst_UV[ 7].fU = pst_UV[ 8].fU = pst_UV[ 9].fU = pst_UV[0].fU + (2 * x);
			pst_UV[10].fU = pst_UV[11].fU = pst_UV[12].fU = pst_UV[13].fU = pst_UV[0].fU + (3 * x);
			pst_UV[14].fU = pst_UV[15].fU = pst_UV[16].fU = pst_UV[17].fU = pst_UV[0].fU + (4 * x);
		}
		
		_pst_Effect->st_Flow.f_CurTime += TIM_gf_dt;
		if(_pst_Effect->st_Flow.f_CurTime >= _pst_Effect->st_Flow.f_Time) 
		{
			pst_String->uw_Flags |= STR_Cuw_SF_Hide;
			return STR_Cul_ER_Terminated;
		}
		break;

	case STR_Cul_EF_Scroll:
		{
			int		i_Char, First, Number, i_NbLine, SFirst, SLast, PrevLineStart[ 4 ];
			float	dy;

			_pst_Effect->st_Scroll.f_CurTime += TIM_gf_dt;
			if (_pst_Effect->st_Scroll.f_CurTime > _pst_Effect->st_Scroll.f_Time)
				_pst_Effect->st_Scroll.f_CurTime = _pst_Effect->st_Scroll.f_Time;

			i_Char = (int) (n * (_pst_Effect->st_Scroll.f_CurTime / _pst_Effect->st_Scroll.f_Time));

			pst_First = pst_Letter;
			First = 0;
			Number = 0;
			i_NbLine = 0;
			PrevLineStart[ 0 ] = PrevLineStart[ 1 ] = PrevLineStart[ 2 ] = PrevLineStart[ 3 ] = 0;
			
			y0 = y1 = pst_Letter->y;
			x0 = x1 = pst_Letter->x;

			/*
			for (pst_Letter = pst_First; pst_Letter < pst_Last; pst_Letter++ )
			{
				if (pst_Letter->ul_Flags & STR_Cul_LF_NewLine)
				{
					if (pst_Letter - pst_First < i_Char)
					{
						First = (pst_Letter - pst_First) + 1;
						PrevLineStart[ 3 ] = PrevLineStart[ 2 ];
						PrevLineStart[ 2 ] = PrevLineStart[ 1 ];
						PrevLineStart[ 1 ] = PrevLineStart[ 0 ];
						PrevLineStart[ 0 ] = First;
					}
					else 
					{
						i_NbLine++;
						if (i_NbLine == _pst_Effect->st_Scroll.i_NbLine )
						{
							Number = (pst_Letter - pst_First) - First + 1;
							break;
						}
					}
				}
			}
			if ( !Number )
			{
				First = PrevLineStart[ _pst_Effect->st_Scroll.i_NbLine - 1 - i_NbLine];
				Number = (pst_Letter - pst_First) - First;
			}
			*/
			
			for (pst_Letter = pst_First; pst_Letter < pst_Last; pst_Letter++ )
			{
				if (pst_Letter->ul_Flags & STR_Cul_LF_NewLine)
				{
					i_NbLine++;
					if (i_NbLine == _pst_Effect->st_Scroll.i_NbLine )
					{
						if (pst_Letter - pst_First < i_Char)
						{
							i_NbLine = 0;
							First = (pst_Letter - pst_First) + 1;
						}
						else
						{
							Number = (pst_Letter - pst_First) - First + 1;
							break;
						}
					}
				}
			}

			if ( !Number )
				Number = (pst_Letter - pst_First) - First;

			SFirst = _pst_STR->pul_Sprite[ pst_String->uw_First + First ] << 2;
			SLast = (_pst_STR->pul_Sprite[ pst_String->uw_First + First + Number - 1] + 1) << 2;

			V = &GFX_gpst_Geo->dst_Point[sprite];
			y0 = V->y;
			V = &GFX_gpst_Geo->dst_Point[lastsprite - 1];
			y1 = V->y;

			V = &GFX_gpst_Geo->dst_Point[sprite];
			VLast = &GFX_gpst_Geo->dst_Point[ SFirst ];
			
			while(V < VLast)
			{
				V->x = V->y = -100.0f;
				V++;
			}

			V = VLast;
			VLast = &GFX_gpst_Geo->dst_Point[ SLast ];
			if ( (pst_String->uw_Flags & STR_Cuw_SF_YJustifyMask) == STR_Cuw_SF_Top )
				dy = y0 - V->y;
			else if ( (pst_String->uw_Flags & STR_Cuw_SF_YJustifyMask) == STR_Cuw_SF_Bottom )
				dy = y1 - (VLast - 1)->y;
			else
				dy = 0;
			
			while(V < VLast)
				V++->y += dy;

			if ( pst_String->uw_Flags & STR_Cuw_SF_ShowFrame )
			{
				if ( (pst_String->uw_Flags & STR_Cuw_SF_YJustifyMask) == STR_Cuw_SF_Top )
					dy = y1 - V->y;
				else if ( (pst_String->uw_Flags & STR_Cuw_SF_YJustifyMask) == STR_Cuw_SF_Bottom )
					dy = y0 - GFX_gpst_Geo->dst_Point[ SFirst ].y;
				else
					dy = 0;
				
				framenbsprite = 9 << 2;
				VFrame = &GFX_gpst_Geo->dst_Point[pst_String->st_Frame.ul_Sprite << 2];

//#ifdef PSX2_TARGET
#if defined( PSX2_TARGET ) || defined( _GAMECUBE )
				while (framenbsprite--)
				{
					if (VFrame->y <= y1 ) VFrame->y -= dy; 
					VFrame++;
				}
#else
				while (framenbsprite--)
				{
					if (VFrame->y >= y1 ) VFrame->y -= dy; 
					VFrame++;
				}
#endif
			}

			V = VLast;
			VLast = &GFX_gpst_Geo->dst_Point[lastsprite];
			while(V < VLast)
			{
				V->x = V->y = -100.0f;
				V++;
			}
		}
		break;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Effect_TreatAll(STR_tdst_Struct *_pst_S)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < _pst_S->uc_NbCurEffects;)
	{
		if(STR_l_Effect_Treat(_pst_S, &_pst_S->st_Effect[i]) & STR_Cul_ER_Terminated)
		{
			_pst_S->uc_NbCurEffects--;
			if(i < _pst_S->uc_NbCurEffects)
				L_memcpy(&_pst_S->st_Effect[i], &_pst_S->st_Effect[_pst_S->uc_NbCurEffects], sizeof(STR_tdst_Effect));
		}
		else
			i++;
	}
}

/*$4
 ***********************************************************************************************************************
    Treat all 2
 ***********************************************************************************************************************
 */
STR_tdst_Struct *STRFX_STR;
STR_tdst_String *STRFX_String;
STR_tdst_Effect *STRFX_Effect;
int             STRFX_from;
int             STRFX_number;
int             STRFX_Sprite, STRFX_SpriteLast;
STR_tdst_Letter *STRFX_Letter, *STRFX_LetterLast;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatFade2Black()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Color, *pul_Color;
	float			x;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	x = STRFX_Effect->st_Fade.f_CurTime / STRFX_Effect->st_Fade.f_Time;
	pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + STRFX_Sprite;
	while(STRFX_Letter < STRFX_LetterLast)
	{
		ul_Color = COLOR_ul_Blend(*pul_Color, 0, x);
		*pul_Color++ = ul_Color;
		*pul_Color++ = ul_Color;
		*pul_Color++ = ul_Color;
		*pul_Color++ = ul_Color;
		STRFX_Letter++;
	}

	STRFX_Effect->st_Fade.f_CurTime += TIM_gf_dt;
    if (STRFX_Effect->st_Fade.f_CurTime >= STRFX_Effect->st_Fade.f_Time) 
    {
        STRFX_String->uw_Flags |= STR_Cuw_SF_Hide;
        return STR_Cul_ER_Terminated;
    }
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatBlink()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Color, *pul_Color;
	float			x;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(STRFX_Effect->ul_Flags & STR_Cul_EF_Blink_Smooth)
	{
		x = STRFX_Effect->st_Fade.f_CurTime / STRFX_Effect->st_Fade.f_Time;
		if(STRFX_Effect->ul_Flags & STR_Cul_EF_Blink_Up) x = 1 - x;
		ul_Color = COLOR_ul_Blend(STRFX_Effect->st_Blink.ul_Color1, STRFX_Effect->st_Blink.ul_Color2, x);
	}
	else
	{
		ul_Color = (STRFX_Effect->ul_Flags & STR_Cul_EF_Blink_Up) ? STRFX_Effect->st_Blink.ul_Color1 : STRFX_Effect->st_Blink.ul_Color2;
	}

	pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + STRFX_Sprite;
	while(STRFX_Letter < STRFX_LetterLast)
	{
		*pul_Color++ = ul_Color;
		*pul_Color++ = ul_Color;
		*pul_Color++ = ul_Color;
		*pul_Color++ = ul_Color;
		STRFX_Letter++;
	}

	STRFX_Effect->st_Fade.f_CurTime += TIM_gf_dt;
	if(STRFX_Effect->st_Fade.f_CurTime >= STRFX_Effect->st_Fade.f_Time)
	{
		STRFX_Effect->st_Fade.f_CurTime = 0;
		if(STRFX_Effect->ul_Flags & STR_Cul_EF_Blink_Up)
			STRFX_Effect->ul_Flags &= ~STR_Cul_EF_Blink_Up;
		else
			STRFX_Effect->ul_Flags |= STR_Cul_EF_Blink_Up;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatGouraud()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			c0, c1, *pul_Color;
	GEO_Vertex		*V, *VLast;
	float			x, y, w, h;
	float			fact;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	VLast = &GFX_gpst_Geo->dst_Point[STRFX_SpriteLast];
	pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + STRFX_Sprite;

	x = V->x;
	y = V->y;
	w = (VLast - 2)->x - x;
	h = (VLast - 2)->y - y;
	if((w == 0) || (h == 0)) return 0;
	w = 1 / w;
	h = 1 / h;

	while(V < VLast)
	{
		fact = (V->x - x) * w;
		c0 = COLOR_ul_Blend(STRFX_Effect->st_Gouraud.aul_Color[0], STRFX_Effect->st_Gouraud.aul_Color[1], fact);
		c1 = COLOR_ul_Blend(STRFX_Effect->st_Gouraud.aul_Color[3], STRFX_Effect->st_Gouraud.aul_Color[2], fact);
		fact = (V->y - y) * h;
		*pul_Color++ = COLOR_ul_Blend(c0, c1, fact);
		V++;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatChangeColor()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Color, *pul_Color;
	float			x;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(STRFX_Effect->st_Fade.f_CurTime < STRFX_Effect->st_Fade.f_Delay)
		x = 0;
	else
		x = (STRFX_Effect->st_Fade.f_CurTime - STRFX_Effect->st_Fade.f_Delay) / STRFX_Effect->st_Fade.f_Time;
	ul_Color = COLOR_ul_Blend(STRFX_Effect->st_Fade.ul_ColorSrc, STRFX_Effect->st_Fade.ul_ColorTgt, x);
	pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + STRFX_Sprite;
	while(STRFX_Letter < STRFX_LetterLast)
	{
		*pul_Color++ = ul_Color;
		*pul_Color++ = ul_Color;
		*pul_Color++ = ul_Color;
		*pul_Color++ = ul_Color;
		STRFX_Letter++;
	}

	STRFX_Effect->st_Fade.f_CurTime += TIM_gf_dt;
	if(STRFX_Effect->st_Fade.f_CurTime >= (STRFX_Effect->st_Fade.f_Time + STRFX_Effect->st_Fade.f_Delay))
	{
		if((STRFX_Effect->st_Fade.ul_ColorTgt & 0xFFFFFF) == 0) STRFX_String->uw_Flags |= STR_Cuw_SF_Hide;
		return STR_Cul_ER_Terminated;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatBounce()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex		*V, *VLast;
	float			fval, fact, fact2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	VLast = &GFX_gpst_Geo->dst_Point[STRFX_SpriteLast];
	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	
    fact = STRFX_Effect->st_Bounce.f_Time - STRFX_Effect->st_Bounce.f_CurTime;
	fact2 = fact * fact * STRFX_Effect->st_Bounce.f_Amplitude * STRFX_STR->uw_SH;
	fact *= STRFX_Effect->st_Bounce.f_TimeFactor;

	while(V < VLast)
	{
		fval = (V->x + V[2].x) / 2;
		fval = fact2 * fAbs(fOptSin(fact + STRFX_Effect->st_Bounce.f_YFactor * fval));
		(V++)->y += fval;
		(V++)->y += fval;
		(V++)->y += fval;
		(V++)->y += fval;
	}

	STRFX_Effect->st_Bounce.f_CurTime += TIM_gf_dt;
	if(STRFX_Effect->st_Bounce.f_CurTime >= STRFX_Effect->st_Bounce.f_Time) return STR_Cul_ER_Terminated;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatPerturb()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Color, *pul_Color;
	GEO_Vertex		*V, *VLast;
	float			fval, fact, fact2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	VLast = &GFX_gpst_Geo->dst_Point[STRFX_SpriteLast];
	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];

	fact = 1 - (STRFX_Effect->st_Perturb.f_CurTime / STRFX_Effect->st_Perturb.f_Time);
	fact *= fact;

	while(V < VLast)
	{
		fval = fact * STRFX_STR->uw_SW * fRand(-STRFX_Effect->st_Perturb.fX, STRFX_Effect->st_Perturb.fX);
		fact2 = fact * STRFX_STR->uw_SH * fRand(-STRFX_Effect->st_Perturb.fY, STRFX_Effect->st_Perturb.fY);

		V->x += fval;
		(V++)->y -= fact2;
		V->x += fval;
		(V++)->y -= fact2;
		V->x += fval;
		(V++)->y -= fact2;
		V->x += fval;
		(V++)->y -= fact2;
	}

	if(STRFX_Effect->ul_Flags & (STR_Cul_EF_PerturbFadeIn | STR_Cul_EF_PerturbFadeOut))
	{
		fact = 2 * STRFX_Effect->st_Perturb.f_CurTime / STRFX_Effect->st_Perturb.f_Time;
		if(STRFX_Effect->ul_Flags & STR_Cul_EF_PerturbFadeOut) fact = 2 - fact;
		if(fact > 1) fact = 1;
		ul_Color = COLOR_ul_Blend(0, 0xFFFFFFFF, fact);
		pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + STRFX_Sprite;
		while(STRFX_Letter < STRFX_LetterLast)
		{
			*pul_Color++ = ul_Color;
			*pul_Color++ = ul_Color;
			*pul_Color++ = ul_Color;
			*pul_Color++ = ul_Color;
			STRFX_Letter++;
		}
	}

	STRFX_Effect->st_Perturb.f_CurTime += TIM_gf_dt;
	if(STRFX_Effect->st_Perturb.f_CurTime >= STRFX_Effect->st_Perturb.f_Time) 
    {
        if (STRFX_Effect->ul_Flags & STR_Cul_EF_PerturbFadeOut)
            STRFX_String->uw_Flags |= STR_Cuw_SF_Hide;
        return STR_Cul_ER_Terminated;
    }
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatShrink()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	GEO_Vertex		*V;
	float			x, y;
	float			fact, fact2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	i = 0;
	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	if(STRFX_Effect->st_Shrink.f_CurTime < STRFX_Effect->st_Shrink.f_Delay)
	{
		STRFX_Effect->st_Shrink.f_CurTime += TIM_gf_dt;
		return 0;
	}

	fact = ((STRFX_Effect->st_Shrink.f_Time - 0.05f) - STRFX_Effect->st_Shrink.f_LetterTime) / STRFX_number;
	while(STRFX_Letter < STRFX_LetterLast)
	{
		fact2 = (STRFX_Effect->st_Shrink.f_CurTime - STRFX_Effect->st_Shrink.f_Delay) - (i * fact);
		if(fact2 <= 0) break;

		fact2 /= STRFX_Effect->st_Shrink.f_LetterTime;
		if(fact2 >= 1) fact2 = 1;
		fact2 = 1 - fact2;

		y = (V[1].y + V->y) / 2;
		x = (V[1].y - V->y) / 2;
		V->y = V[3].y = y - (x * fact2);
		V[1].y = V[2].y = y + (x * fact2);

		V += 4;
		STRFX_Letter++;
		i++;
	}

	STRFX_Effect->st_Shrink.f_CurTime += TIM_gf_dt;
	if(STRFX_Effect->st_Shrink.f_CurTime >= (STRFX_Effect->st_Shrink.f_Time + STRFX_Effect->st_Shrink.f_Delay))
	{
		STRFX_String->uw_Flags |= STR_Cuw_SF_Hide;
		return STR_Cul_ER_Terminated;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatUnshrink()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	GEO_Vertex		*V;
	float			x, y;
	float			fact, fact2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = 0;
	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	fact = ((STRFX_Effect->st_Shrink.f_Time - 0.05f) - STRFX_Effect->st_Shrink.f_LetterTime) / STRFX_number;
	while(STRFX_Letter < STRFX_LetterLast)
	{
		fact2 = ((STRFX_Effect->st_Shrink.f_CurTime - STRFX_Effect->st_Shrink.f_Delay) - (i * fact)) / STRFX_Effect->st_Shrink.f_LetterTime;
		if(fact2 < 1)
		{
			if(fact2 < 0) fact2 = 0;
			y = (V[1].y + V->y) / 2;
			x = (V[1].y - V->y) / 2;
			V->y = V[3].y = y - (x * fact2);
			V[1].y = V[2].y = y + (x * fact2);
		}

		V += 4;
		STRFX_Letter++;
		i++;
	}

	STRFX_Effect->st_Shrink.f_CurTime += TIM_gf_dt;
	if(STRFX_Effect->st_Shrink.f_CurTime >= (STRFX_Effect->st_Shrink.f_Time + STRFX_Effect->st_Shrink.f_Delay))
		return STR_Cul_ER_Terminated;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatShrinkCenter()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex		*V, *VLast;
	float			x, y, w, h;
	float			fact;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	VLast = &GFX_gpst_Geo->dst_Point[STRFX_SpriteLast];
	x = w = V->x;
	y = h = V->y;
	while(V < VLast)
	{
		if(V->x < x) x = V->x;
		if(V->y < y) y = V->y;
		if(V->x > w) w = V->x;
		if(V->y > h) h = V->y;
		V++;
	}

	fact = STRFX_Effect->st_Shrink.f_CurTime / STRFX_Effect->st_Shrink.f_Time;
	fact *= fact;
	if((STRFX_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_ShrinkCenter) fact = 1 - fact;

	x = (x + w) / 2;
	y = (y + h) / 2;
	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	while(V < VLast)
	{
		V->x = x + ((V->x - x) * fact);
		V->y = y + ((V->y - y) * fact);
		V++;
	}

	STRFX_Effect->st_Shrink.f_CurTime += TIM_gf_dt;
	if(STRFX_Effect->st_Shrink.f_CurTime >= STRFX_Effect->st_Shrink.f_Time)
	{
		if((STRFX_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_ShrinkCenter)
			STRFX_String->uw_Flags |= STR_Cuw_SF_Hide;
		return STR_Cul_ER_Terminated;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatEnlarge()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Color, *pul_Color;
	GEO_Vertex		*V, *VLast;
	float			x, y, w, h;
	float			fact, fact2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	VLast = &GFX_gpst_Geo->dst_Point[STRFX_SpriteLast];
	x = w = V->x;
	y = h = V->y;
	while(V < VLast)
	{
		if(V->x < x) x = V->x;
		if(V->y < y) y = V->y;
		if(V->x > w) w = V->x;
		if(V->y > h) h = V->y;
		V++;
	}

	if(STRFX_Effect->st_Enlarge.f_CurTime < STRFX_Effect->st_Enlarge.f_Delay)
	{
		if((STRFX_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_Unenlarge)
		{
			V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
			while(V < VLast)
			{
				V->x = V->y = -1;
				V++;
			}
		}

		STRFX_Effect->st_Enlarge.f_CurTime += TIM_gf_dt;
		return 0;
	}

	fact = (STRFX_Effect->st_Enlarge.f_CurTime - STRFX_Effect->st_Enlarge.f_Delay) / STRFX_Effect->st_Enlarge.f_Time;
	fact *= fact;
	if((STRFX_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_Unenlarge) fact = 1 - fact;
	fact2 = 1 + (STRFX_Effect->st_Enlarge.f_Factor - 1) * fact;

	x = (x + w) / 2;
	y = (y + h) / 2;
	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	ul_Color = COLOR_ul_Blend(GFX_gpst_Geo->dul_PointColors[1 + STRFX_Sprite], 0, fact);
	pul_Color = GFX_gpst_Geo->dul_PointColors + 1 + STRFX_Sprite;
	while(V < VLast)
	{
		V->x = x + ((V->x - x) * fact2);
		V->y = y + ((V->y - y) * fact2);
		V++;
		*pul_Color++ = ul_Color;
	}

	STRFX_Effect->st_Enlarge.f_CurTime += TIM_gf_dt;
	if(STRFX_Effect->st_Enlarge.f_CurTime >= STRFX_Effect->st_Enlarge.f_Time + STRFX_Effect->st_Enlarge.f_Delay)
	{
		if((STRFX_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_Enlarge) STRFX_String->uw_Flags |= STR_Cuw_SF_Hide;
		return STR_Cul_ER_Terminated;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatDoor()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex		*V, *VLast;
	float			yref, fact, fact2;
    static float    f_ZFactor = 0.001f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	VLast = &GFX_gpst_Geo->dst_Point[STRFX_SpriteLast];

	fact = STRFX_Effect->st_Door.f_CurTime / STRFX_Effect->st_Door.f_Time;
	if((STRFX_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_Undoor) fact = 1 - fact;
	fact2 = fOptCos(fact * Cf_PiBy2);
	fact = fOptSin(fact * Cf_PiBy2);
    yref = STRFX_Effect->st_Door.f_Ref * STRFX_STR->uw_SH;

	while(V < VLast)
	{
		V->y = yref + ((V->y - yref) * fact);
		V->z += f_ZFactor * fAbs((V->y - yref)) * fact2;
		V++;
	}

	STRFX_Effect->st_Door.f_CurTime += TIM_gf_dt;
	if(STRFX_Effect->st_Door.f_CurTime >= STRFX_Effect->st_Door.f_Time)
	{
		if((STRFX_Effect->ul_Flags & STR_Cul_EF_Type) == STR_Cul_EF_Undoor) STRFX_String->uw_Flags |= STR_Cuw_SF_Hide;
		return STR_Cul_ER_Terminated;
	}
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatRotate()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex		*V, *VLast;
	float			x, y, w, h;
	float			fact, fact2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	VLast = &GFX_gpst_Geo->dst_Point[STRFX_SpriteLast];
	x = w = V->x;
	y = h = V->y;
	while(V < VLast)
	{
		if(V->x < x) x = V->x;
		if(V->y < y) y = V->y;
		if(V->x > w) w = V->x;
		if(V->y > h) h = V->y;
		V++;
	}

	x = (x + w) / 2;
	y = (y + h) / 2;

	fact = fOptSin(STRFX_Effect->st_Rotate.f_Angle);
	fact2 = fOptCos(STRFX_Effect->st_Rotate.f_Angle);
	STRFX_Effect->st_Rotate.f_Angle += STRFX_Effect->st_Rotate.f_Speed * TIM_gf_dt;

	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	while(V < VLast)
	{
		V->x -= x;
		V->y -= y;

		w = V->x * fact2 - V->y * fact;
		h = V->y * fact2 + V->x * fact;

		V->x = w + x;
		V->y = h + y;
		V++;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Effect_TreatFlow_AddSprite( int sprite0, float newxy, float delta )
{
    int             i, sprite1;
    GEO_Vertex		*V0, *V1;
	GEO_tdst_UV		*UV0, *UV1;
    ULONG           *C0, *C1;
    float           fact, newuv;

    STRFX_STR->pul_Sprite[ STRFX_STR->ul_NbSprite ] = (STRFX_STR->pul_Sprite[ STRFX_STR->ul_NbSprite ] & 0xFFFFFF) | (STRFX_STR->pul_Sprite[ sprite0 >> 2 ] & 0xFF000000);
    STRFX_STR->pul_SpriteFont[ STRFX_STR->pul_Sprite[ STRFX_STR->ul_NbSprite ] >> 24 ]++;
    sprite1 = STRFX_STR->ul_NbSprite++ << 2;

    /* source data */
    V0 = &GFX_gpst_Geo->dst_Point[ sprite0 ];
    UV0 = &GFX_gpst_Geo->dst_UV[ sprite0 ];
    C0 = GFX_gpst_Geo->dul_PointColors + 1 + sprite0;

    /* destination data */
    V1 = &GFX_gpst_Geo->dst_Point[ sprite1 ];
    UV1 = &GFX_gpst_Geo->dst_UV[ sprite1 ];
    C1 = GFX_gpst_Geo->dul_PointColors + 1 + sprite1;

    /* copy data */
    for (i = 0; i < 4; i++)
    {
        MATH_CopyVector( V1+i, V0+i );
        C1[i] = C0[i];
        UV1[i].fU = UV0[i].fU;
        UV1[i].fV = UV0[i].fV;
    }

    /* modify data according to effect */
    switch (STRFX_Effect->st_Flow.i_Dir)
    {
    case 0:
        fact = (newxy - V0[1].y) / ( V0[0].y - V0[1].y);
        newuv = UV0[1].fV + (UV0->fV - UV0[1].fV) * fact;
        UV0[0].fV = UV0[3].fV = UV1[0].fV = UV1[1].fV = UV1[2].fV = UV1[3].fV = newuv;
        
        V0->y = V0[3].y = newxy;
        V1[1].y = V1[2].y = newxy;
        V1[0].y = V1[3].y = STRFX_STR->uw_SH;
        V1[0].x += delta;
        V1[3].x += delta;
        break;
    case 1:
        fact = (newxy - V0[0].y) / ( V0[1].y - V0[0].y);
        newuv = UV0->fV + (UV0[1].fV - UV0->fV) * fact;
        UV0[1].fV = UV0[2].fV = UV1[0].fV = UV1[1].fV = UV1[2].fV = UV1[3].fV = newuv;

        V0[1].y = V0[2].y = newxy;
        V1[0].y = V1[3].y = newxy;
        V1[1].y = V1[2].y = 0;
        V1[1].x += delta;
        V1[2].x += delta;
        break;
    case 2:
        fact = (newxy - V0[0].x) / (V0[3].x - V0[0].x);
		newuv = UV0[0].fU + (UV0[3].fU - UV0[0].fU) * fact;
        UV0[2].fU = UV0[3].fU = UV1[0].fU = UV1[1].fU = UV1[2].fU = UV1[3].fU = newuv;

        V0[2].x = V0[3].x = newxy;
		V1[0].x = V1[1].x = newxy;
		V1[2].x = V1[3].x = STRFX_STR->uw_SW;
        V1[2].y += delta;
        V1[3].y += delta;
        break;
    case 3:
        fact = (newxy - V0[3].x) / (V0[0].x - V0[3].x);
		newuv = UV0[3].fU + (UV0[0].fU - UV0[3].fU) * fact;
        UV0[0].fU = UV0[1].fU = UV1[0].fU = UV1[1].fU = UV1[2].fU = UV1[3].fU = newuv;

        V0[0].x = V0[1].x = newxy;
		V1[0].x = V1[1].x = 0;
		V1[2].x = V1[3].x = newxy;
        V1[0].y += delta;
        V1[1].y += delta;
        break;
    }
}

int STR_Effect_TreatFlow()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i, sprite0;
	GEO_Vertex		*V;
	float			x, y, w, h;
	float			fact, delta;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	y = h = GFX_gpst_Geo->dst_Point[STRFX_Sprite].y;
	x = w = GFX_gpst_Geo->dst_Point[STRFX_Sprite].x;
	for(i = 0; i < STRFX_number; i++)
	{
		V = &GFX_gpst_Geo->dst_Point[(STRFX_STR->pul_Sprite[i + STRFX_String->uw_First + STRFX_from] & 0xFFFFFF) << 2];
		if(V->y > h) h = V->y;
		if((V + 1)->y < y) y = (V + 1)->y;
		if((V + 2)->x > w) w = (V + 2)->x;
		if(V->x < x) x = V->x;
	}

	h -= y;
	w -= x;
    
	fact = (STRFX_Effect->st_Flow.f_CurTime / STRFX_Effect->st_Flow.f_Time);
    delta = STRFX_Effect->st_Flow.f_Delta * ( (STRFX_Effect->st_Flow.i_Dir <= 1) ? STRFX_STR->uw_SW : STRFX_STR->uw_SH );

	if((STRFX_Effect->st_Flow.i_Dir == 0) || (STRFX_Effect->st_Flow.i_Dir == 1))
	{
		/*j0 = STRFX_Effect->st_Flow.i_Dir;
		j1 = 1 - j0;
		j2 = j0 ? 2 : 3;
		j3 = j0 ? 3 : 2;*/
		y = y + (STRFX_Effect->st_Flow.i_Dir ? (1 - fact) : fact) * h;
		for(i = 0; i < STRFX_number; i++)
		{
            sprite0 = (STRFX_STR->pul_Sprite[i + STRFX_String->uw_First + STRFX_from] & 0xFFFFFF) << 2;
			V = &GFX_gpst_Geo->dst_Point[ sprite0 ];
			if ((V + 1)->y >= y)
			{
				if(!STRFX_Effect->st_Flow.i_Dir)
				{
					(V + 0)->x = (V + 1)->x = (V + 2)->x = (V + 3)->x = -100.0f;
					(V + 0)->y = (V + 1)->y = (V + 2)->y = (V + 3)->y = -100.0f;
				}
			}
			else if(V->y <= y)
			{
				if(STRFX_Effect->st_Flow.i_Dir)
				{
					(V + 0)->x = (V + 1)->x = (V + 2)->x = (V + 3)->x = -100.0f;
					(V + 0)->y = (V + 1)->y = (V + 2)->y = (V + 3)->y = -100.0f;
				}
			}
			else
                STR_Effect_TreatFlow_AddSprite( sprite0, y, delta );
		}
	}
	else if(STRFX_Effect->st_Flow.i_Dir == 2)
	{
		x = x + fact * w;
		for(i = 0; i < STRFX_number; i++)
		{
            sprite0 = (STRFX_STR->pul_Sprite[i + STRFX_String->uw_First + STRFX_from] & 0xFFFFFF) << 2;
			V = &GFX_gpst_Geo->dst_Point[ ( STRFX_STR->pul_Sprite[i + STRFX_String->uw_First + STRFX_from] & 0xFFFFFF ) << 2];
			if((V + 3)->x < x)
			{
				continue;
			}
			else if(V->x > x)
			{
				(V + 0)->x = (V + 1)->x = (V + 2)->x = (V + 3)->x = -100.0f;
				(V + 0)->y = (V + 1)->y = (V + 2)->y = (V + 3)->y = -100.0f;
			}
			else
                STR_Effect_TreatFlow_AddSprite( sprite0, x, delta );
		}
	}
	else if(STRFX_Effect->st_Flow.i_Dir == 3)
	{
		x = x + (1 - fact) * w;
		for(i = 0; i < STRFX_number; i++)
		{
            sprite0 = (STRFX_STR->pul_Sprite[i + STRFX_String->uw_First + STRFX_from] & 0xFFFFFF) << 2;
			V = &GFX_gpst_Geo->dst_Point[ (STRFX_STR->pul_Sprite[i + STRFX_String->uw_First + STRFX_from] & 0xFFFFFF)<< 2];
			if((V + 3)->x < x)
			{
				(V + 0)->x = (V + 1)->x = (V + 2)->x = (V + 3)->x = -100.0f;
				(V + 0)->y = (V + 1)->y = (V + 2)->y = (V + 3)->y = -100.0f;
			}
			else if(V->x > x)
			{
                continue;
			}
			else
                STR_Effect_TreatFlow_AddSprite( sprite0, x, delta );
		}
	}

	STRFX_Effect->st_Flow.f_CurTime += TIM_gf_dt;
	if(STRFX_Effect->st_Flow.f_CurTime >= STRFX_Effect->st_Flow.f_Time) return STR_Cul_ER_Terminated;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_Effect_TreatCut()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex		*V, *VLast;
	GEO_tdst_UV		*pst_UV;
	float			y0, y1, v0, v1, min, max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	V = &GFX_gpst_Geo->dst_Point[STRFX_Sprite];
	VLast = &GFX_gpst_Geo->dst_Point[STRFX_SpriteLast];
	min = (1 - STRFX_Effect->st_Cut.max) * STRFX_STR->uw_SH;
	max = (1 - STRFX_Effect->st_Cut.min) * STRFX_STR->uw_SH;
	while(V < VLast)
	{
		if(((V + 1)->y > max) || (V->y < min))
		{
			(V + 0)->x = (V + 1)->x = (V + 2)->x = (V + 3)->x = -100.0f;
			(V + 0)->y = (V + 1)->y = (V + 2)->y = (V + 3)->y = -100.0f;
		}
		else if((V->y > max) || ((V + 1)->y < min))
		{
			y0 = (V->y > max) ? max : V->y;
			y1 = ((V + 1)->y < min) ? min : (V + 1)->y;

			pst_UV = &GFX_gpst_Geo->dst_UV[V - GFX_gpst_Geo->dst_Point];
			v0 = pst_UV->fV + ((pst_UV + 1)->fV - pst_UV->fV) * ((y0 - V->y) / ((V + 1)->y - V->y));
			v1 = pst_UV->fV + ((pst_UV + 1)->fV - pst_UV->fV) * ((y1 - V->y) / ((V + 1)->y - V->y));

			V->y = (V + 3)->y = y0;
			pst_UV->fV = (pst_UV + 3)->fV = v0;
			(V + 1)->y = (V + 2)->y = y1;
			(pst_UV + 1)->fV = (pst_UV + 2)->fV = v1;
		}

		V += 4;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Effect_TreatAll2(STR_tdst_Struct *_pst_S)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i, res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

    STRFX_STR = _pst_S;

	for(i = 0; i < _pst_S->uc_NbCurEffects;)
	{
		STRFX_Effect = _pst_S->st_Effect + i;

        if((STRFX_String = STR_pst_GetString(_pst_S, STRFX_Effect->i_String)) == NULL) 
        {
            res = STR_Cul_ER_Terminated;
            goto STR_Effect_TreatAll_EndLoop;
        }
    	if(STRFX_String->uw_Flags & STR_Cuw_SF_Hide) 
        {
            res = 0;
            goto STR_Effect_TreatAll_EndLoop;
        }
        
        STRFX_from = STR_l_GetValidIndex(STRFX_STR, STRFX_String, STRFX_Effect->i_From);
	    STRFX_number = STR_l_GetValidNumber(STRFX_STR, STRFX_String, STRFX_from, STRFX_Effect->i_Number);
	    if(STRFX_number == 0) 
        {
            res = 0;
            goto STR_Effect_TreatAll_EndLoop;
        }

	    STRFX_Letter = _pst_S->dst_Letter + STRFX_String->uw_First + STRFX_from;
	    STRFX_LetterLast = STRFX_Letter + STRFX_number;

        STRFX_Sprite = (_pst_S->pul_Sprite[STRFX_Letter - _pst_S->dst_Letter] & 0xFFFFFF) << 2;
	    STRFX_SpriteLast = ((_pst_S->pul_Sprite[(STRFX_LetterLast - _pst_S->dst_Letter) - 1] & 0xFFFFFF) + 1) << 2;

        switch(STRFX_Effect->ul_Flags & STR_Cul_EF_Type)
        {
		    case STR_Cul_EF_Fade2Black:
			    res = STR_Effect_TreatFade2Black();
			    break;
		    case STR_Cul_EF_Blink:
			    res = STR_Effect_TreatBlink();
			    break;
		    case STR_Cul_EF_Gouraud:
			    res = STR_Effect_TreatGouraud();
			    break;
		    case STR_Cul_EF_ChangeColor:
			    res = STR_Effect_TreatChangeColor();
			    break;
		    case STR_Cul_EF_Bounce:
			    res = STR_Effect_TreatBounce();
			    break;
		    case STR_Cul_EF_Perturb:
			    res = STR_Effect_TreatPerturb();
			    break;
		    case STR_Cul_EF_Shrink:
			    res = STR_Effect_TreatShrink();
			    break;
		    case STR_Cul_EF_Unshrink:
			    res = STR_Effect_TreatUnshrink();
			    break;
		    case STR_Cul_EF_ShrinkCenter:
		    case STR_Cul_EF_UnshrinkCenter:
			    res = STR_Effect_TreatShrinkCenter();
			    break;
		    case STR_Cul_EF_Enlarge:
		    case STR_Cul_EF_Unenlarge:
			    res = STR_Effect_TreatEnlarge();
			    break;
		    case STR_Cul_EF_Door:
		    case STR_Cul_EF_Undoor:
			    res = STR_Effect_TreatDoor();
			    break;
		    case STR_Cul_EF_Rotate:
			    res = STR_Effect_TreatRotate();
			    break;
		    case STR_Cul_EF_Flow:
			    res = STR_Effect_TreatFlow();
			    break;
		    case STR_Cul_EF_Cut:
			    res = STR_Effect_TreatCut();
			    break;
        }

STR_Effect_TreatAll_EndLoop:
		if(res & STR_Cul_ER_Terminated)
		{
			_pst_S->uc_NbCurEffects--;
			if(i < _pst_S->uc_NbCurEffects)
				L_memcpy(&_pst_S->st_Effect[i], &_pst_S->st_Effect[_pst_S->uc_NbCurEffects], sizeof(STR_tdst_Effect));
		}
		else
			i++;
	}
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

/*$T STRparse.c GC! 1.081 01/28/02 10:07:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "STRing/STRstruct.h"
#include "STRing/STRdata.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
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
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_ReadFormat_Int(char *_sz_String)
{
	/*~~~~~~~~~~~~~*/
	LONG	l_Length;
	LONG	l_End;
	/*~~~~~~~~~~~~~*/

	l_Length = 0;
	l_End = 0;
	STR_sgst_Format.i_Param = 0;
	while((*_sz_String != '\\') && (*_sz_String != 0))
	{
		if(!l_End)
		{
			if(isdigit(*_sz_String))
			{
				STR_sgst_Format.i_Param = STR_sgst_Format.i_Param * 10 + (*_sz_String - '0');
			}
			else
			{
				l_End = 1;
			}
		}

		_sz_String++;
		l_Length++;
	}

	return l_Length;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_ReadFormat_Float(char *_sz_String)
{
	/*~~~~~~~~~~~~~*/
	LONG	l_Length;
	LONG	l_End;
	float	f_Mul;
	/*~~~~~~~~~~~~~*/

	l_Length = 0;
	l_End = 0;
	f_Mul = 0.0f;
	STR_sgst_Format.f_Param = 0;
	while((*_sz_String != '\\') && (*_sz_String != 0))
	{
		if(!l_End)
		{
			if(isdigit(*_sz_String))
			{
				if(f_Mul == 0.0f)
					STR_sgst_Format.f_Param = STR_sgst_Format.f_Param * 10.0f + (*_sz_String - '0');
				else
				{
					STR_sgst_Format.f_Param += (*_sz_String - '0') * f_Mul;
					f_Mul /= 10.0f;
				}
			}
			else if(*_sz_String == '.')
			{
				f_Mul = 0.1f;
			}
			else
			{
				l_End = 1;
			}
		}

		_sz_String++;
		l_Length++;
	}

	return l_Length;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_ReadFormat_Hexa(char *_sz_String)
{
	/*~~~~~~~~~~~~~*/
	LONG	l_Length;
	LONG	l_End;
	/*~~~~~~~~~~~~~*/

	l_Length = 0;
	l_End = 0;
	STR_sgst_Format.i_Param = 0;
	while((*_sz_String != '\\') && (*_sz_String != 0))
	{
		if(!l_End)
		{
			if(isdigit(*_sz_String))
				STR_sgst_Format.ul_Param = STR_sgst_Format.ul_Param * 16 + (*_sz_String - '0');
			else if((*_sz_String >= 'a') && (*_sz_String <= 'f'))
				STR_sgst_Format.ul_Param = STR_sgst_Format.ul_Param * 16 + (*_sz_String - 'a' + 10);
			else if((*_sz_String >= 'A') && (*_sz_String <= 'F'))
				STR_sgst_Format.ul_Param = STR_sgst_Format.ul_Param * 16 + (*_sz_String - 'A' + 10);
			else
				l_End = 1;
		}

		_sz_String++;
		l_Length++;
	}

	return l_Length;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_ReadFormat(char *_sz_String)
{
	/*~~~~~~~~~~~~~*/
	LONG	l_Length;
	int		x, y, xp, yp;
	/*~~~~~~~~~~~~~*/

	if(*_sz_String != '\\') return 0;

	STR_sgst_Format.i_Format = STR_Cul_Format_Unknown;
	l_Length = 1;
	_sz_String++;

	switch(*_sz_String)
	{
	case 'A':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Align;
		STR_sgst_Format.i_Param = 0;
		while ( ( *_sz_String != '\\' ) && ( *_sz_String ) )
		{
			switch ( tolower( *_sz_String ) )
			{
			case 'x': STR_sgst_Format.i_Param |= STR_String_AlignXCenter; break;
			case 'r': STR_sgst_Format.i_Param |= STR_String_AlignXRight; break;
			case 't': STR_sgst_Format.i_Param |= STR_String_AlignYTop; break;
			case 'y': STR_sgst_Format.i_Param |= STR_String_AlignYCenter; break;
			}
			l_Length++;
			_sz_String++;
		}
		if ( *_sz_String == '\\' )l_Length++;
		break;
	case 'n':
		l_Length++;
		STR_sgst_Format.i_Format = STR_Cul_Format_EOL;
		break;
	case 'T':
		l_Length++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Tab;
		break;
	case 't':
		l_Length++;
		STR_sgst_Format.i_Format = STR_Cul_Format_TabGlobal;
		break;
	case 'm':
#ifdef _XBOX
// -- WARNING Bricolage XBOX pour nombre de blocks sauvegarde --
// si /m1/* alors remplace * par le numero du blocks sauvegarde
// voir dans SDK/xbox/xbINOsaving.c
		{
			extern short BlocksSizeNeeded;
			if ( *(_sz_String+3) == '*' )
			*(_sz_String+3) = 48+BlocksSizeNeeded;//48 = 0 
		}
#endif
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Mark;
		l_Length += STR_l_ReadFormat_Int(_sz_String) + 1;
		break;

	case 'p':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_PageInst;
		l_Length += STR_l_ReadFormat_Int(_sz_String) + 1;
		break;

	case 'P':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Page;
		l_Length += STR_l_ReadFormat_Int(_sz_String) + 1;
		break;

	case 'x':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_X;
		l_Length += STR_l_ReadFormat_Float(_sz_String) + 1;
		break;
	case 'y':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Y;
		l_Length += STR_l_ReadFormat_Float(_sz_String) + 1;
		break;
	case 'w':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_W;
		l_Length += STR_l_ReadFormat_Float(_sz_String) + 1;
		break;
	case 'h':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_H;
		l_Length += STR_l_ReadFormat_Float(_sz_String) + 1;
		break;
	case 'c':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Color;
		l_Length += STR_l_ReadFormat_Hexa(_sz_String) + 1;
		break;
	case 'a':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Ascii;
		l_Length += STR_l_ReadFormat_Int(_sz_String) + 1;
		break;
	case '-':
		l_Length++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Retrait;
		break;
	case 'u':
		l_Length++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Unicode;
		break;
	case 'd':
		STR_sgst_Format.i_Format = STR_Cul_Format_GlobalString;
		STR_sgst_Format.i_Param = _sz_String[1] - '0';
		_sz_String += 2;
		l_Length += 2;
		break;
    case 'D':
        l_Length++;
		_sz_String++;
        STR_sgst_Format.i_Format = STR_Cul_Format_GlobalString;
        l_Length += STR_l_ReadFormat_Int(_sz_String) + 1;
        break;
	case 'b':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_LetterEffect;
		l_Length += STR_l_ReadFormat_Int(_sz_String) + 1;
		break;
	case 'j':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Justify;
		x = y = 0;
		while ( ( *_sz_String != '\\' ) && ( *_sz_String ) )
		{
			switch ( tolower( *_sz_String ) )
			{
			case 'l': x = 0; break;
			case 'x': x = STR_String_JustPivotXCenter; break;
			case 'r': x = STR_String_JustPivotXRight; break;
			case 't': y = 0; break;
			case 'y': y = STR_String_JustPivotYCenter; break;
			case 'b': y = STR_String_JustPivotYBottom; break;
			}
			l_Length++;
			_sz_String++;
		}
		if ( *_sz_String == '\\' )l_Length++;
		STR_sgst_Format.i_Param = x | (x << 2) | y | (y << 2);
		break;
	case 'J':
		l_Length++;
		_sz_String++;
		STR_sgst_Format.i_Format = STR_Cul_Format_Justify;
		x = y = xp = yp = 0;
		while ( ( *_sz_String != '\\' ) && ( *_sz_String ) )
		{
			switch ( *_sz_String )
			{
			case 'l': xp = 0; break;
			case 'x': xp = STR_String_JustPivotXCenter; break;
			case 'r': xp = STR_String_JustPivotXRight; break;
			case 't': yp = 0; break;
			case 'y': yp = STR_String_JustPivotYCenter; break;
			case 'b': yp = STR_String_JustPivotYBottom;	break;
			case 'L': x = 0; break;
			case 'X': x = STR_String_JustLetterXCenter; break;
			case 'R': x = STR_String_JustLetterXRight; break;
			case 'T': y = 0; break;
			case 'Y': y = STR_String_JustLetterYCenter; break;
			case 'B': y = STR_String_JustLetterYBottom; break;
			}
			l_Length++;
			_sz_String++;
		}
		if ( *_sz_String == '\\' )l_Length++;
		STR_sgst_Format.i_Param = x | xp | y | yp;
		break;
	case 'f':
		l_Length++;
		_sz_String++;
		if (*_sz_String == 'c' )
		{
			l_Length++;
			_sz_String++;
			STR_sgst_Format.i_Format = STR_Cul_Format_FrameColor;
			l_Length += STR_l_ReadFormat_Hexa(_sz_String) + 1;
		}
		else if (*_sz_String == 'i')
		{
			l_Length++;
			_sz_String++;
			STR_sgst_Format.i_Format = STR_Cul_Format_FrameIcon;
			l_Length += STR_l_ReadFormat_Int(_sz_String) + 1;
		}
		else if (*_sz_String == '\\')
		{
			l_Length++;
			_sz_String++;
			STR_sgst_Format.i_Format = STR_Cul_Format_Frame;
		}
		break;
	}

	return l_Length;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_GetStringLength(char *_sz_String)
{
	/*~~~~~~~~~~~~~*/
	LONG	l_Length;
	BOOL	b_Unicode;
	/*~~~~~~~~~~~~~*/

	l_Length = 0;
	
	// special for unicode
	b_Unicode = FALSE;
	if ( (*_sz_String == '\\') && (_sz_String[1] == 'u' ) )
	{
		b_Unicode = TRUE;
		_sz_String += 2;
	}

	while(*_sz_String)
	{
		if(*_sz_String == '\\')
		{
			_sz_String += STR_l_ReadFormat(_sz_String);
			if(STR_sgst_Format.i_Format == STR_Cul_Format_Ascii)
				l_Length++;
			else if(STR_sgst_Format.i_Format == STR_Cul_Format_GlobalString)
				l_Length += STR_i_GlobalLength(STR_sgst_Format.i_Param);
			continue;
		}
		else if((*_sz_String == 0x0A) || (*_sz_String == 0x0D))
		{
			_sz_String++;
			continue;
		}

		if (b_Unicode)
			_sz_String+=2;
		else
			_sz_String++;
		l_Length++;
	}

	return l_Length;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_ParseString
(
	STR_tdst_Struct *_pst_STR,
	int				i_String, 
	STR_tdst_Letter *_pst_Letter,
	unsigned char	*_puc_Buffer,
	LONG			n,
	float			w,
	float			h,
	ULONG			color,
	int				i_Offset

)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Letter *pst_Prev;
	unsigned char	uc_Ascii, b_Ascii;
	ULONG			ul_Page, ul_PageInst, ul_Flag;
	float			x, y;
	char			*sz_Global;
	STR_tdst_Letter *pst_LetterFirst;
	BOOL			b_Unicode;
	int				code;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uc_Ascii = '\0';
	pst_LetterFirst = _pst_Letter;

	ul_Flag = 0;
	x = 0;
	y = 0;
	ul_Page = ul_PageInst = 0;
	b_Ascii = 0;
	sz_Global = NULL;
	pst_Prev = NULL;
	
	// special for unicode
	b_Unicode = FALSE;
	if ( (*_puc_Buffer == '\\') && (_puc_Buffer[1] == 'u' ) )
	{
		b_Unicode = TRUE;
		_puc_Buffer += 2;
	}

	while(*_puc_Buffer)
	{
		/* Special character */
		if(*_puc_Buffer == '\\')
		{
			_puc_Buffer += STR_l_ReadFormat((char *) _puc_Buffer);
			switch(STR_sgst_Format.i_Format)
			{
			case STR_Cul_Format_EOL:
				if(pst_Prev) pst_Prev->ul_Flags |= STR_Cul_LF_NewLine;
				break;
			case STR_Cul_Format_Mark:
				ul_Flag &= ~STR_Cul_LF_MarkMask;
				ul_Flag += (STR_sgst_Format.i_Param) << STR_Cul_LF_MarkShift;
				break;
			case STR_Cul_Format_PageInst:
				ul_PageInst = STR_sgst_Format.i_Param;
				break;
			case STR_Cul_Format_Page:
				ul_Page = ul_PageInst = STR_sgst_Format.i_Param;
				break;
			case STR_Cul_Format_Color:
				color = STR_sgst_Format.ul_Param;
				break;
			case STR_Cul_Format_X:
				x = STR_sgst_Format.f_Param;
				break;
			case STR_Cul_Format_Y:
				y = STR_sgst_Format.f_Param;
				break;
			case STR_Cul_Format_W:
				w = STR_sgst_Format.f_Param;
				break;
			case STR_Cul_Format_H:
				h = STR_sgst_Format.f_Param;
				break;
			case STR_Cul_Format_Ascii:
				uc_Ascii = (unsigned char) STR_sgst_Format.i_Param;
				b_Ascii = 1;
				break;
			case STR_Cul_Format_Retrait:
				ul_Flag |= STR_Cul_LF_Retrait;
				break;
			case STR_Cul_Format_GlobalString:
				sz_Global = STR_sz_GlobalGet(STR_sgst_Format.i_Param);
				if (sz_Global && !*sz_Global) sz_Global = NULL;
				break;
			case STR_Cul_Format_LetterEffect:
				{
					int i_Effect, i;
					i_Effect = STR_i_EffectAdd( (OBJ_tdst_GameObject *) _pst_STR, 0x80000000 + STR_Cul_EF_Joy, i_String, i_Offset + (_pst_Letter - pst_LetterFirst), 1 );
					if (i_Effect != -1 )
					{
						STR_SetCurEffect( _pst_STR, i_Effect );
						if (STR_spst_CurEffect)
						{
							STR_spst_CurEffect->st_Joy.c_Value = 0;
							STR_spst_CurEffect->st_Joy.c_CurDir = 0;
							i = STR_sgst_Format.i_Param;
							STR_spst_CurEffect->st_Joy.i_Dir = 0;
							while ( i ) 
							{
								STR_spst_CurEffect->st_Joy.i_Dir = (STR_spst_CurEffect->st_Joy.i_Dir << 4) | (i % 10);
								i = ( i - ( i % 10 ) ) / 10;
							}
							STR_spst_CurEffect->st_Joy.f_CurTime = 0;
							STR_spst_CurEffect->st_Joy.f_Time = 0.4f;
						}
					}
				}
				break;
			}

			if((!b_Ascii) && !sz_Global ) continue;
		}
		else if(*_puc_Buffer == 0x0D)
		{
			_puc_Buffer++;
			continue;
		}
		else if(*_puc_Buffer == 0x0A)
		{
			_puc_Buffer++;
			if(pst_Prev) pst_Prev->ul_Flags |= STR_Cul_LF_NewLine;
			continue;
		}
		else if(*_puc_Buffer == 0) return;

		if(!n) return;

		if(b_Ascii)
			_pst_Letter->ul_Flags = ul_Flag | uc_Ascii;
		else if(!sz_Global)
		{
			if ( b_Unicode )
			{
				code = ((*_puc_Buffer - 32) * 200) + (_puc_Buffer[ 1 ] - 32);
				if (code < 256 )
					_pst_Letter->ul_Flags = ul_Flag | STR_sgac_CharTable[ code ];
				else
					_pst_Letter->ul_Flags = ul_Flag | (code & 0xFFF);
				_puc_Buffer++;
			}
			else
				_pst_Letter->ul_Flags = ul_Flag | STR_sgac_CharTable[*_puc_Buffer];
		}
		else
		{
strparsestring_insertglobal:
			_pst_Letter->ul_Flags = ul_Flag | STR_sgac_CharTable[*sz_Global];
			*sz_Global++;
		}

		_pst_Letter->ul_Flags |= (ul_PageInst << STR_Cul_LF_PageShift);

		_pst_Letter->x = (short) (x * _pst_STR->uw_SW);
		_pst_Letter->y = (short) (y * _pst_STR->uw_SH);
		_pst_Letter->w = (short) (w * _pst_STR->uw_SW);
		_pst_Letter->h = (short) (h * _pst_STR->uw_SH);
		_pst_Letter->ul_Color = color;

		if(sz_Global && *sz_Global)
		{
			pst_Prev = _pst_Letter++;
			n--;
			goto strparsestring_insertglobal;
		}

		if ( (!b_Ascii) && (!sz_Global) ) _puc_Buffer++;
		pst_Prev = _pst_Letter++;
		n--;

		ul_Flag = 0;
		ul_PageInst = ul_Page;
		b_Ascii = 0;
		sz_Global = NULL;
	}
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_DelFormatString(char *_sz_String)
{
	/*~~~~~~~~~~~~*/
	char	*sz_Cur;
	/*~~~~~~~~~~~~*/

	sz_Cur = _sz_String;
	while(*_sz_String)
	{
		if(*_sz_String == '\\')
		{
			_sz_String += STR_l_ReadFormat(_sz_String);
			if(STR_sgst_Format.i_Format == STR_Cul_Format_Ascii)
			{
//				*sz_Cur++ = (char) STR_sgst_Format.i_Param;
			}
			else if(STR_sgst_Format.i_Format == STR_Cul_Format_EOL) 
			{
				*sz_Cur++ = ' ';
			}
			continue;
		}

		*sz_Cur++ = *_sz_String++;
	}
    *sz_Cur = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_DelFormatStringKeepEOL(char *_sz_String)
{
	/*~~~~~~~~~~~~*/
	char	*sz_Cur;
	/*~~~~~~~~~~~~*/

	sz_Cur = _sz_String;
	while(*_sz_String)
	{
		if(*_sz_String == '\\')
		{
			_sz_String += STR_l_ReadFormat(_sz_String);
			if(STR_sgst_Format.i_Format == STR_Cul_Format_Ascii)
			{
//				*sz_Cur++ = (char) STR_sgst_Format.i_Param;
			}
			else if(STR_sgst_Format.i_Format == STR_Cul_Format_EOL) 
			{
				*sz_Cur++ = '\\';
				*sz_Cur++ = 'n';
			}
			continue;
		}

		*sz_Cur++ = *_sz_String++;
	}
    *sz_Cur = 0;
}

#endif /* ACTIVE_EDITORS */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

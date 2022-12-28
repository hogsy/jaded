/*$T CLIstr.h GC! 1.088 08/08/00 13:50:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __CLISTR_H__
#define __CLISTR_H__

#include "BASe/BASsys.h"
#include <ctype.h>
#include <string.h>

#if !defined( PSX2_TARGET ) && !defined( _GAMECUBE )
#  include <memory.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    target specific definitions
 ***********************************************************************************************************************
 */

#if defined( PSX2_TARGET )
#  define L_strcmpi	stricmp
#  define L_stricmp	stricmp
#  define L_strnicmp	strnicmp
#  define L_strupr	strupr
#  define L_strlwr	strlwr
#  define L_ltoa		ltoa
#  define L_atoi		atoi
#elif defined( _GAMECUBE )
#  define L_strcmpi	strcmp
#  define L_stricmp	strcmp
#  define L_strupr	strupr
#  define L_strlwr	strlwr
#  define L_ltoa		ltoa
#  define L_atoi		atol
#else
#  define L_strcmpi	_stricmp
#  define L_stricmp	_stricmp
#  define L_strnicmp	_strnicmp
#  define L_strupr	_strupr
#  define L_strlwr	_strlwr
#  define L_ltoa		_ltoa
#  define L_atoi		atol
#endif

/*$4
 ***********************************************************************************************************************
    common definitions
 ***********************************************************************************************************************
 */

#define L_strncmp		strncmp
#define L_strcmp		strcmp
#define L_strcat		strcat
#define L_strlen		strlen
#define L_strcpy		strcpy
#define L_strrchr		strrchr
#define L_strstr		strstr
#define L_size_t		size_t
#define L_memcmp		memcmp
#define L_atol			atoi
#define L_atof			atof
#define L_toupper(c)	(((UCHAR) (c) >= 'a') && ((UCHAR) (c) <= 'z') ? (UCHAR) (c) - ('a' - 'A') : toupper((UCHAR) (c)))
#define L_isalnum(a)	(((UCHAR) (a) == '_') || isalnum((UCHAR) (a)))
#define L_isprint(a)	isprint((UCHAR) (a))
#define L_isspace(a)	isspace((UCHAR) (a))
#define L_isalpha(a)	isalpha((UCHAR) (a))
#define L_isdigit(a)	isdigit((UCHAR) (a))
#define L_isxdigit(a)	isxdigit((UCHAR) (a))
#define L_isbinary(a)	(((UCHAR) (a) == '0') || ((UCHAR) (a) == '1'))
/*$4
 ***********************************************************************************************************************
    target specific functions
 ***********************************************************************************************************************
 */

#if defined(PSX2_TARGET) && defined(__GNUC__)
#define __EXTERN	extern
#else
#define __EXTERN
#endif	/* PSX2_TARGET & __GNUC__ */
#ifdef PSX2_TARGET

/*
 =======================================================================================================================
    only PS2 because the C lib. function contains bugs
 =======================================================================================================================
 */
__EXTERN _inline_ char *L_strchr(const char *string, int c)
{
	/*~~~~~~~~~*/
	char	*asz;
	/*~~~~~~~~~*/

	for(asz = (char *) string; (*asz != '\0' && *asz != (char) c); asz++)
	{
	};

	return(*asz == '\0' ? (char *) NULL : asz);
}

extern void *ps2memmove(char*dst, char*src, int size);
extern void *ps2memset(char *dst, unsigned char c, int size);
extern void *ps2memcpy(char *dst, char*src, int size);

#define L_memmove		ps2memmove
#define L_memset		ps2memset
#define L_memcpy	    ps2memcpy
#else
#define L_strchr	strchr
#define L_memmove		memmove
#define L_memset		memset
#ifdef _GAMECUBE
/* on game cube memmove is 4x faster than memcpy */
#define L_memcpy	    memmove
#else
#define L_memcpy	    memcpy
#endif
#endif	/* PSX2_TARGET */
#define L_zero( PTR, SIZE ) L_memset( ( PTR ), 0, ( SIZE ) )

/*$4
 ***********************************************************************************************************************
    common functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    PS2 and PC
 =======================================================================================================================
 */
__EXTERN _inline_ char *L_strdup(const char *_pc_Str)
{
	/*~~~~~~~*/
	char	*p;
	/*~~~~~~~*/

	p = (char *) malloc(L_strlen(_pc_Str) + 1);
	L_strcpy(p, _pc_Str);
	return p;
}

/*
 =======================================================================================================================
    PS2 and PC
 =======================================================================================================================
 */
__EXTERN _inline_ char *L_strstri(char *_psz_Ref, char *_psz_Src)
{
	while((*_psz_Src == *_psz_Ref) || (L_toupper(*_psz_Src) == L_toupper(*_psz_Ref)))
	{
		if(!(*_psz_Src))
		{
			return _psz_Ref;
		}

		_psz_Src++;
		_psz_Ref++;
	}

	if(!(*_psz_Src))
	{
		return _psz_Ref;
	}

	return (char *) NULL;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#undef __EXTERN
#endif	/* __CLISTR_H__ */

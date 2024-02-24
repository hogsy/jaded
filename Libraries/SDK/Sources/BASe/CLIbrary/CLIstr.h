/*$T CLIstr.h GC! 1.088 08/08/00 13:50:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#include "BASe/BASsys.h"
#include <ctype.h>
#include <string.h>

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include "BASe/BAStypes.h"

#if defined( __cplusplus ) && !defined( JADEFUSION )
extern "C"
{
#endif

	/*$4
 ***********************************************************************************************************************
    target specific definitions
 ***********************************************************************************************************************
 */

#define L_strcmpi  _stricmp
#define L_stricmp  _stricmp
#define L_strnicmp _strnicmp
#define L_strupr   _strupr
#define L_strlwr   _strlwr
#define L_ltoa     _ltoa
#define L_atoi     atol

	/*$4
 ***********************************************************************************************************************
    common definitions
 ***********************************************************************************************************************
 */

#define L_strncmp       strncmp
#define L_strcmp        strcmp
#define L_strcat        strcat
#define L_strlen        strlen
#define L_strcpy        strcpy
#define L_strrchr       strrchr
#define L_strstr        strstr
#define L_size_t        size_t
#define L_memcmp        memcmp
#define L_atol          atoi
#define L_atof          atof
#define L_toupper( c )  ( ( ( unsigned char ) ( c ) >= 'a' ) && ( ( unsigned char ) ( c ) <= 'z' ) ? ( unsigned char ) ( c ) - ( 'a' - 'A' ) : toupper( ( unsigned char ) ( c ) ) )
#define L_isalnum( a )  ( ( ( unsigned char ) ( a ) == '_' ) || isalnum( ( unsigned char ) ( a ) ) )
#define L_isprint( a )  isprint( ( unsigned char ) ( a ) )
#define L_isspace( a )  isspace( ( unsigned char ) ( a ) )
#define L_isalpha( a )  isalpha( ( unsigned char ) ( a ) )
#define L_isdigit( a )  isdigit( ( unsigned char ) ( a ) )
#define L_isxdigit( a ) isxdigit( ( unsigned char ) ( a ) )
#define L_isbinary( a ) ( ( ( unsigned char ) ( a ) == '0' ) || ( ( unsigned char ) ( a ) == '1' ) )
	/*$4
 ***********************************************************************************************************************
    target specific functions
 ***********************************************************************************************************************
 */

#define L_strchr            strchr
#define L_memmove           memmove
#define L_memset            memset
#define L_memcpy            memcpy
#define L_zero( PTR, SIZE ) L_memset( ( PTR ), 0, ( SIZE ) )

	/*$4
 ***********************************************************************************************************************
    common functions
 ***********************************************************************************************************************
 */

	static inline char *L_strtoupper( char *s )
	{
		for ( size_t i = 0; s[ i ] != '\0'; ++i )
			s[ i ] = ( char ) toupper( s[ i ] );

		return s;
	}

	static inline char *L_strntoupper( char *s, size_t n )
	{
		for ( size_t i = 0; i < n; ++i )
		{
			if ( s[ i ] == '\0' )
				break;

			s[ i ] = ( char ) toupper( s[ i ] );
		}
		return s;
	}

	static inline char *L_strtolower( char *s )
	{
		for ( size_t i = 0; s[ i ] != '\0'; ++i )
			s[ i ] = ( char ) tolower( s[ i ] );

		return s;
	}

	static inline char *L_strntolower( char *s, size_t n )
	{
		for ( size_t i = 0; i < n; ++i )
		{
			if ( s[ i ] == '\0' )
				break;

			s[ i ] = ( char ) tolower( s[ i ] );
		}
		return s;
	}

	/*
 =======================================================================================================================
    PS2 and PC
 =======================================================================================================================
 */
	extern _inline_ char *L_strdup( const char *_pc_Str )
	{
		/*~~~~~~~*/
		char *p;
		/*~~~~~~~*/

		p = ( char * ) malloc( L_strlen( _pc_Str ) + 1 );
		L_strcpy( p, _pc_Str );
		return p;
	}

	/*
 =======================================================================================================================
    PS2 and PC
 =======================================================================================================================
 */
	extern _inline_ char *L_strstri( char *_psz_Ref, char *_psz_Src )
	{
		while ( ( *_psz_Src == *_psz_Ref ) || ( L_toupper( *_psz_Src ) == L_toupper( *_psz_Ref ) ) )
		{
			if ( !( *_psz_Src ) )
			{
				return _psz_Ref;
			}

			_psz_Src++;
			_psz_Ref++;
		}

		if ( !( *_psz_Src ) )
		{
			return _psz_Ref;
		}

		return ( char * ) NULL;
	}

#if defined( __cplusplus ) && !defined( JADEFUSION )
}
#endif

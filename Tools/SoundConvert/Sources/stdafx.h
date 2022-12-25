/*$T stdafx.h GC 1.138 05/12/04 14:28:55 */

/*
 * stdafx.h : include file for standard system include files, £
 * or project specific include files that are used frequently, but £
 * are changed infrequently £
 */
#pragma once

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define WIN32_LEAN_AND_MEAN /* Exclude rarely-used stuff from Windows headers */
#include <afx.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <malloc.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <afxtempl.h>
#include <afxwin.h>

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
#define M_STR_SIZE						2048

#define XBOX_ADPCM_SAMPLES_PER_BLOCK	64


/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define ERR_X_Assert	ASSERT

#define L_memmove		memmove
#define L_memset		memset

#define L_strcpy		strcpy
#define L_stricmp		stricmp
#define L_strlen		strlen

#define L_qsort			qsort
#define L_isprint		isprint

#define MEM_p_Alloc		malloc
#define MEM_Free		free

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void *MEM_p_Realloc(void *p, unsigned int size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	void	*ptemp = malloc(size);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	memcpy(ptemp, p, size);
	free(p);
	return ptemp;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */


/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef int				INT, I32;
typedef unsigned int	UINT, UI32;
typedef long			LONG, L32;
typedef unsigned long	ULONG, UL32;
typedef short			SHORT, I16, L16;
typedef unsigned short	USHORT, UI16, UL16;
typedef char			CHAR, I8, L8;
typedef unsigned char	UCHAR, UI8, UL8, BYTE;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

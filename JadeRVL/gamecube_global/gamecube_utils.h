//------------------------------------------------------------------------
//
// (C) Copyright 2000 Ubisoft
//
// Author       Dany Joannette
// Date         17 Octobre 2001
//
// File         Gamecube_utils.h
// Description  Dolphin utilities for CrossPlatform compilation
//
//------------------------------------------------------------------------

#ifndef BASE_GAMECUBEUTILS_H
#define BASE_GAMECUBEUTILS_H

//#include <stdio.h>
//#include <cstdarg>
//#include <stdarg.h> // for va_start va_end
//#include <string.h>
//#include "alloca.h"

// Description      Equivalent to MS OutputDebugString but with variable args.
//------------------------------------------------------------------------
void GC_OutputDebugString(const char *_szFormat, ...);
/*{
#ifdef _DEBUG
    va_list         Marker;
    char            Buffer[4096];
    
    // Processing the Variable Arguments
    va_start(Marker, _szFormat);
    sprintf(Buffer, _szFormat, Marker);
    va_end(Marker);
    Buffer[4095] = '\0';

    OSReport(Buffer);
#endif    
}
*/

/*
#define _strlwr strlwr
#define _strupr strupr

typedef struct tagRGBQUAD {
	int rgbBlue; 
	int rgbGreen; 
	int rgbRed; 
	int rgbReserved;
} RGBQUAD ;
	
#define GetAsyncKeyState(x) false // temporary
#define FIELD_OFFSET(type, field)    ((s32)(s32 *)&(((type *)0)->field))

void _snprintf(char *, int, char*, ...); 
 
inline void _snprintf(char * _string, int _maxlen, char *_format, ...) 
{
	va_list Marker;
	
	va_start(Marker, _format);
	vsprintf(_string, _format, Marker);
	va_end(Marker);

}

// string manipulation
int   stricmp(const char *s1, const char *s2);
int   strnicmp(const char *s1, const char *s2, size_t len);
char *strupr(char *s);
char *strlwr(char *s);

// the 3 following are inlined
int   strcmpi(const char *s1, const char *s2);
char *_itoa( int value, char *string, int radix);
#define itoa _itoa;

inline int isdigit(int s1)
{
	return ((s1>'0')&&(s1<'9'));
}

inline int strcmpi(const char *s1, const char *s2)
{
	return (stricmp(s1, s2));
}

void Sleep( long u32_ms );

*/
#endif // BASE_DOLPHIN_H

// ---------------------------------------------------------------------------------------------------------------------------------
//        _   _ _
//       | | (_) |
//  _   _| |_ _| |
// | | | | __| | |
// | |_| | |_| | |
//  \__,_|\__|_|_|
//
//
//
// Description:
//
//   Miscellaneous utilitarian functions
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   04/13/2001 by Paul Nettle: Original creation
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//   This software is 100% free. If you use this software (in part or in whole) you must credit the author. This software may not be
//   re-distributed (in part or in whole) in a modified form without clear documentation on how to obtain a copy of the original
//   work. You may not use this software to directly or indirectly cause harm to others. This software is provided as-is and without
//   warrantee -- Use at your own risk. For more information, visit HTTP://www.FluidStudios.com/
//
// Copyright 2001, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_FSTL_UTIL
#define _FSTL_UTIL

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include <new>
#include "common.h"

FSTL_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	T	pi()
{
	return static_cast<T>(3.14159265359);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	T	toRadians(const T & a)
{
	return pi<T>() / static_cast<T>(180) * a;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	T	toDegrees(const T & a)
{
	return static_cast<T>(180) / pi<T>() * a;
}

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef	min
#undef	min
#endif

template <class T>
inline	T &	min(T & a, T & b)
{
	return (a < b) ? a : b;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	const T &	min(const T & a, const T & b)
{
	return (a < b) ? a : b;
}

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef	max
#undef	max
#endif

template <class T>
inline	T &	max(T & a, T & b)
{
	return (a > b) ? a : b;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	const T &	max(const T & a, const T & b)
{
	return (a > b) ? a : b;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	T	abs(const T & a)
{
	return (a < 0) ? -a : a;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	swap(T& a, T& b)
{
	T c(a);
	a = b;
	b = c;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	T *	allocate(const unsigned int count)
{
	T *	ptr = static_cast<T *>(operator new(sizeof(T) * count));
	#ifdef	UNICODE
	if (!ptr) throwstring(_T(""), _T("Out of memory"));
	#else
	if (!ptr) throwstring("", "Out of memory");
	#endif
	return	ptr;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	deallocate(T* ptr)
{
	operator delete(ptr);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	T *	construct(T* ptr)
{
	return new (static_cast<void *>(ptr)) T;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T, class Tz>
inline	T *	construct(T* ptr, Tz &src)
{
	return new (static_cast<void *>(ptr)) T(src);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	destruct(T* ptr)
{
	if (ptr) ptr->~T();
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	destructElements(T* ptr, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		destruct(&ptr[i]);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	moveElements(T* dst, T* src, const unsigned int count)
{
	// Supposed to be doing anything?

	if (!count) return;

	// Copying forward or backwards?
	
	if (dst < src)
	{
		for (unsigned int i = 0; i < count; ++i, ++dst, ++src)
		{
			*dst = *src;
		}
	}
	else
	{
		T*	s = src + count - 1;
		T*	d = dst + count - 1;
		for (unsigned int i = 0; i < count; ++i, --d, --s)
		{
			*d = *s;
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	copyElements(T* dst, T* src, const unsigned int count)
{
	// Supposed to be doing anything?

	if (!count) return;

	for (unsigned int i = 0; i < count; ++i, ++dst, ++src)
	{
		construct(dst, *src);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	fillElements(T* dst, const T& src, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i, ++dst)
	{
		*dst = src;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	invertElements(T* ptr, const unsigned int count)
{
	T *	p0 = ptr;
	T *	p1 = ptr + count - 1;

	while(p0 < p1)
	{
		swap(*p0, *p1);
		++p0;
		--p1;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	T	toupper(T& c)
{
	return (c >= static_cast<T>('a') && c <= static_cast<T>('z')) ? (c - static_cast<T>('a'-'A')):c;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	T	tolower(T& c)
{
	return (c >= static_cast<T>('A') && c <= static_cast<T>('Z')) ? (c + static_cast<T>('a'-'A')):c;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	bool	isspace(T c)
{
	return	(c == static_cast<T>(' ' )) ||
		(c == static_cast<T>('\t')) ||
		(c == static_cast<T>('\v')) ||
		(c == static_cast<T>('\r')) ||
		(c == static_cast<T>('\n')) ||
		(c == static_cast<T>('\f'));
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	bool	isdigit(T c)
{
	return (c >= static_cast<T>('0')) && (c <= static_cast<T>('9'));
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	bool	isalpha(T c)
{
	return	(c >= static_cast<T>('a') && c <= static_cast<T>('z')) ||
		(c >= static_cast<T>('A') && c <= static_cast<T>('Z'));
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	bool	isalnum(T c)
{
	return isalpha(c) || isdigit(c);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	zeromem(T* dst, unsigned int count = 1)
{
	::memset(dst, 0, sizeof(T) * count);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	memset(T* dst, T fill, unsigned int count)
{
	if (sizeof(T) == 1)
	{
		::memset(dst, fill, count);
	}
	else
	{
		while(count--) *(dst++) = fill;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	memcpy(T* dst, const T* src, unsigned int count)
{
	::memcpy(dst, src, sizeof(T) * count);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	void	memmove(T* dst, const T* src, unsigned int count)
{
	::memmove(dst, src, sizeof(T) * count);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline	T	rand(const T min, const T max)
{
	double	result = static_cast<double>(::rand()) / static_cast<double>(RAND_MAX) * static_cast<double>(max-min) + static_cast<double>(min);
	return	static_cast<T>(result);
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Wide-char low-level specialization routines
// ---------------------------------------------------------------------------------------------------------------------------------

inline	int		atoi(const char * str)						{return ::atoi(str);}
inline	int		atoi(const wchar_t * str)					{return ::_wtoi(str);}

inline	long		atol(const char * str)						{return ::atol(str);}
inline	long		atol(const wchar_t * str)					{return ::_wtol(str);}

inline	double		atof(const char * str)						{return ::atof(str);}
inline	double		atof(const wchar_t * str)					{return ::_wtof(str);}

inline	size_t		strlen(const char * a)						{return ::strlen(a);}
inline	size_t		strlen(const wchar_t * a)					{return ::wcslen(a);}

inline	char *		strcpy(char * a, const char * b)				{return ::strcpy(a,b);}
inline	wchar_t *	strcpy(wchar_t * a, const wchar_t * b)				{return ::wcscpy(a,b);}

inline	char *		strncpy(char * a, const char * b, size_t c)			{return ::strncpy(a,b,c);}
inline	wchar_t *	strncpy(wchar_t * a, const wchar_t * b, size_t c)		{return ::wcsncpy(a,b,c);}

inline	int		strcmp(const char * a, const char * b)				{return ::strcmp(a,b);}
inline	int		strcmp(const wchar_t * a, const wchar_t * b)			{return ::wcscmp(a,b);}

inline	int		strncmp(const char * a, const char * b, size_t c)		{return ::strncmp(a,b,c);}
inline	int		strncmp(const wchar_t * a, const wchar_t * b, size_t c)		{return ::wcsncmp(a,b,c);}

inline	int		stricmp(const char * a, const char * b)				{return ::stricmp(a,b);}
inline	int		stricmp(const wchar_t * a, const wchar_t * b)			{return ::wcsicmp(a,b);}

inline	int		strnicmp(const char * a, const char * b, size_t c)		{return ::strnicmp(a,b,c);}
inline	int		strnicmp(const wchar_t * a, const wchar_t * b, size_t c)	{return ::wcsnicmp(a,b,c);}

inline	char *		strstr(const char * a, const char * b)				{return ::strstr(a,b);}
inline	wchar_t *	strstr(const wchar_t * a, const wchar_t * b)			{return ::wcsstr(a,b);}

inline	char *		strchr(const char * a, int b)					{return ::strchr(a,b);}
inline	wchar_t *	strchr(const wchar_t * a, int b)				{return ::wcschr(a,b);}

inline	size_t		strcspn(const char * a, const char * b)				{return ::strcspn(a,b);}
inline	size_t		strcspn(const wchar_t * a, const wchar_t * b)			{return ::wcscspn(a,b);}

inline	size_t		strspn(const char * a, const char * b)				{return ::strspn(a,b);}
inline	size_t		strspn(const wchar_t * a, const wchar_t * b)			{return ::wcsspn(a,b);}

template<class T> T	space_char()							{return static_cast<T>(' ');}
template<class T> T *	empty_string()							{static	T nullchar; return &nullchar;}

FSTL_NAMESPACE_END
#endif // _FSTL_UTIL
// ---------------------------------------------------------------------------------------------------------------------------------
// util - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


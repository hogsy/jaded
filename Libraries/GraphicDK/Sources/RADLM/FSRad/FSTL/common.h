// ---------------------------------------------------------------------------------------------------------------------------------
//   ___  ___  _ __ ___  _ __ ___   ___  _ __
//  / __|/ _ \| '_ ` _ \| '_ ` _ \ / _ \| '_ \
// | (__| (_) | | | | | | | | | | | (_) | | | |
//  \___|\___/|_| |_| |_|_| |_| |_|\___/|_| |_|
//
// Description:
//
//   Common components for all FSTL modules
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

#ifndef	_FSTL_COMMON
#define _FSTL_COMMON

// ---------------------------------------------------------------------------------------------------------------------------------
// Microsoft defines these, but they should be templates, not macros.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

#define	FSTL_NAMESPACE_BEGIN	namespace fstl {
#define	FSTL_NAMESPACE_END	};

// ---------------------------------------------------------------------------------------------------------------------------------
// Often times, it's better to use the compiler to generate an error rather than a runtime assert. The following metaprogram (if
// you want to call it that) provides nice compile-time errors on a false condition.
//
// Note that the compiler will probably complain about a funciton that doesn't exist, which is fine. The fact still remains that
// the compiler DOES generate an error that points to the line containing the TemplateAssert macro call.
//
// Also note that much like runtime asserts that get conditionally compiled out during non-debug builds, this gets 'optimized' out
// always (provided it doesn't generate a compiler error.)
// ---------------------------------------------------------------------------------------------------------------------------------

//template <bool B> struct templateAssert {};
//struct templateAssert<true> {static void there_has_been_a_compile_time_assertion() {};};
#define	TemplateAssert(__a)
// {const bool __b = (__a) ? true:false; templateAssert<__b>::there_has_been_a_compile_time_assertion();}
#define	TemplateError(__msg) {const bool __b = (__a) ? true:false; templateAssert<__b>::##__msg();}

// ---------------------------------------------------------------------------------------------------------------------------------
// Assertion macros
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef	assert
#undef	assert
#endif
#ifdef	verify
#undef	verify
#endif

#ifdef	_DEBUG
#define	assert(a)	{if(!(a)) {__asm{int 3};static fstl::error err(fstl::string(__FILE__"[") + fstl::string(__LINE__) + fstl::string("]"), #a, "Assertion failed"); throw err;}}
#else // _DEBUG
#define	assert(a)	{}
#endif // _DEBUG

#ifdef	_DEBUG
#define	verify(a)	{if(!(a)) {__asm{int 3};static fstl::error err(fstl::string(__FILE__"[") + fstl::string(__LINE__) + fstl::string("]"), #a, "Verify failed"); throw err;}}
#else // _DEBUG
#define	verify(a)	a
#endif // _DEBUG

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef	_DEBUG

#ifdef	UNICODE
#define	throwstring(ins,msg)	{static fstl::werror err(fstl::wstring(_T(__FILE__) _T("[")) + fstl::wstring(__LINE__) + fstl::wstring(_T("]")), fstl::wstring(ins), fstl::wstring(msg)); __asm{int 3}; throw err;}
#define	cascadestring(err,msg)	{static fstl::werror cascaded(err + (fstl::wstring(_T(__FILE__) _T("[")) + fstl::wstring(__LINE__) + fstl::wstring(_T("]|cascaded|")) + fstl::wstring(msg))); __asm{int 3}; throw cascaded;}
#else // UNICODE
#define	throwstring(ins,msg)	{static fstl::error err(fstl::string(__FILE__ "[") + fstl::string(__LINE__) + fstl::string("]"), fstl::string(ins), fstl::string(msg)); __asm{int 3}; throw err;}
#define	cascadestring(err,msg)	{static fstl::error cascaded(err + (fstl::string(__FILE__ "[") + fstl::string(__LINE__) + fstl::string("]|cascaded|") + fstl::string(msg))); __asm{int 3}; throw cascaded;}
#endif // UNICODE

#else // _DEBUG

#ifdef	UNICODE
#define	throwstring(ins,msg)	{static fstl::werror err(fstl::wstring(_T(__FILE__) _T("[")) + fstl::wstring(__LINE__) + fstl::wstring(_T("]")), fstl::wstring(ins), fstl::wstring(msg)); throw err;}
#define	cascadestring(err,msg)	{static fstl::werror cascaded(err + (fstl::wstring(_T(__FILE__) _T("[")) + fstl::wstring(__LINE__) + fstl::wstring(_T("]|cascaded|")) + fstl::wstring(msg))); throw cascaded;}
#else // UNICODE
#define	throwstring(ins,msg)	{static fstl::error err(fstl::string(__FILE__ "[") + fstl::string(__LINE__) + fstl::string("]"), fstl::string(ins), fstl::string(msg)); throw err;}
#define	cascadestring(err,msg)	{static fstl::error cascaded(err + (fstl::string(__FILE__ "[") + fstl::string(__LINE__) + fstl::string("]|cascaded|") + fstl::string(msg))); throw cascaded;}
#endif // UNICODE

#endif // _DEBUG

// ---------------------------------------------------------------------------------------------------------------------------------
// Handy for making lists and arrays of a type
// ---------------------------------------------------------------------------------------------------------------------------------

#define LIST(type)\
	typedef	fstl::list<      type  >	type##List;\
	typedef	fstl::list<      type *>	type##PointerList;\
	typedef	fstl::list<const type  >	Const##type##List;\
	typedef	fstl::list<const type *>	Const##type##PointerList;

#define ARRAY(type)\
	typedef	fstl::array<      type  >	type##Array;\
	typedef	fstl::array<      type *>	type##PointerArray;\
	typedef	fstl::array<const type  >	Const##type##Array;\
	typedef	fstl::array<const type *>	Const##type##PointerArray;

#define LISTARRAY(type)\
	LIST(type)\
	ARRAY(type)

#endif // _FSTL_COMMON
// ---------------------------------------------------------------------------------------------------------------------------------
// common - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


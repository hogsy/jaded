/*
	stdint.h -- integer types

	Implements subclause 7.18 of ISO/IEC 9899:1999 (E) + TC1.
*/

#ifndef __STDINT_H__
#define __STDINT_H__

/*
	7.18.1.1	Exact-width integer types
*/

// signed
typedef signed char		    int8_t;
typedef short				int16_t;
#ifdef WIN32
typedef __int64				int64_t;
#else
typedef long long           int64_t;
#endif
typedef int					int32_t;

// unsigned
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
#ifdef WIN32
typedef unsigned __int64	uint64_t;
#else
typedef unsigned long long	uint64_t;
#endif

/*
	7.18.1.2	Minimum-width integer types
*/

typedef char				int_least8_t;
typedef unsigned char		uint_least8_t;
typedef short				int_least16_t;
typedef unsigned short		uint_least16_t;
typedef int					int_least32_t;
typedef unsigned int		uint_least32_t;

#ifdef WIN32
typedef __int64				int_least64_t;
typedef unsigned __int64	uint_least64_t;
#else
typedef long long int_least64_t;
typedef unsigned long long uint_least64_t;
#endif

/*
	7.18.1.3	Fastest minimum-width integer types
*/

typedef int					int_fast8_t;
typedef unsigned int		uint_fast8_t;
typedef int					int_fast16_t;
typedef unsigned int		uint_fast16_t;
typedef int					int_fast32_t;
typedef unsigned int		uint_fast32_t;
#ifdef WIN32
typedef __int64				int_fast64_t;
typedef unsigned __int64	uint_fast64_t;
#else
typedef long long			int_fast64_t;
typedef unsigned long long	uint_fast64_t;
#endif

/*
	7.18.1.4	Integer types capable of holding object pointers
*/

typedef int					intptr_t;
typedef unsigned int		uintptr_t;

/*
	7.18.1.5	Greatest-width integer types
*/
#ifdef WIN32
typedef __int64				intmax_t;
typedef unsigned __int64	uintmax_t;
#else
typedef long long				intmax_t;
typedef unsigned long	long uintmax_t;
#endif

#if	!defined(__cplusplus) || defined(__STDC_LIMIT_MACROS)

/*
	7.18.2.1	Limits of exact-width integer types

*/

#define	INT8_MAX			127
#define	INT8_MIN			(-128)
#define	UINT8_MAX			255u

#define	INT16_MAX			32767
#define	INT16_MIN			(-32768)
#define	UINT16_MAX			65535u

#define	INT32_MAX			2147483647
#define	INT32_MIN			(-2147483648)
#define	UINT32_MAX			4294967295u

#define	INT64_MAX			9223372036854775807i64
#define	INT64_MIN			(-9223372036854775808i64)
#define	UINT64_MAX			18446744073709551615ui64

/*
	7.18.2.2	Limits of minimum-width integer types
*/

#define	INT_LEAST8_MAX		INT8_MAX
#define	INT_LEAST8_MIN		INT8_MIN
#define	UINT_LEAST8_MAX		UINT8_MAX

#define	INT_LEAST16_MAX		INT16_MAX
#define	INT_LEAST16_MIN		INT16_MIN
#define	UINT_LEAST16_MAX	UINT16_MAX

#define	INT_LEAST32_MAX		INT32_MAX
#define	INT_LEAST32_MIN		INT32_MIN
#define	UINT_LEAST32_MAX	UINT32_MAX

#define	INT_LEAST64_MAX		INT64_MAX
#define	INT_LEAST64_MIN		INT64_MIN
#define	UINT_LEAST64_MAX	UINT64_MAX

/*
	7.18.2.3	Limits of fastest minimum-width integer types
*/

#define	INT_FAST8_MAX		INT32_MAX
#define	INT_FAST8_MIN		INT32_MIN
#define	UINT_FAST8_MAX		UINT32_MAX

#define	INT_FAST16_MAX		INT32_MAX
#define	INT_FAST16_MIN		INT32_MIN
#define	UINT_FAST16_MAX		UINT32_MAX

#define	INT_FAST32_MAX		INT32_MAX
#define	INT_FAST32_MIN		INT32_MIN
#define	UINT_FAST32_MAX		UINT32_MAX

#define	INT_FAST64_MAX		INT64_MAX
#define	INT_FAST64_MIN		INT64_MIN
#define	UINT_FAST64_MAX		UINT64_MAX

/*
	7.18.2.4	Limits of integer types
				capable of holding object pointers
*/

#define	INTPTR_MAX			INT32_MAX
#define	INTPTR_MIN			INT32_MIN
#define	UINTPTR_MAX			UINT32_MAX

/*
	7.18.2.5	Limits of greatest-width integer types
*/

#define	INTMAX_MAX			INT64_MAX
#define	INTMAX_MIN			INT64_MIN
#define	UINTMAX_MAX			UINT64_MAX

/*
	7.18.3		Limits of other integer types
*/

#define	PTRDIFF_MAX			INT32_MAX
#define	PTRDIFF_MIN			INT32_MIN

#define	SIG_ATOMIC_MAX		INT32_MAX
#define	SIG_ATOMIC_MIN		INT32_MIN

#define	SIZE_MAX			UINT32_MAX

#define	WCHAR_MAX			UINT16_MAX
#define	WCHAR_MIN			0u

#define	WINT_MAX			UINT16_MAX
#define	WINT_MIN			0u

#endif	/* !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS) */

#if	!defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)

/*
	7.18.4.1	Macros for minimum-width integer constants
*/

#define	INT8_C(c)		c
#define	UINT8_C(c)		c ## u
#define	INT16_C(c)		c
#define	UINT16_C(c)		c ## u
#define	INT32_C(c)		c
#define	UINT32_C(c)		c ## u
#define	INT64_C(c)		c ## i64
#define	UINT64_C(c)		c ## ui64

/*
	7.18.4.2	Macros for greatest-width integer constants
*/

#define	INTMAX_C(c)		c ## i64
#define	UINTMAX_C(c)	c ## ui64

#endif	/* !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS) */

#endif	/* __STDINT_H__ */

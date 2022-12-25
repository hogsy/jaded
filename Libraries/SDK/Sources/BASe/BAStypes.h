/*$T BAStypes.h GC! 1.077 03/24/00 14:11:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif // PSX2_TARGET

#ifdef ACTIVE_EDITORS
#ifdef _DEBUG
#define AI_FULL_RASTERS
#endif // _DEBUG
#endif // ACTIVE_EDITORS

#ifndef __BASTYPES_H__
#define __BASTYPES_H__

#ifdef _GAMECUBE
#   include <types.h>
#endif // _GAMECUBE
#if defined( _MSC_VER )
#   include <stdint.h>
#   include <stdbool.h>
#endif // #if defined( _MSC_VER )

#define BAS_C_Target_PS2    1
#define BAS_C_Target_GC     2
#define BAS_C_Target_XBox   3
#define BAS_C_Target_Pc     4
#define BAS_C_Target_Xenon  5



//###################################################################################
//###################################################################################
#if defined( PSX2_TARGET )
//###################################################################################
//###################################################################################


#   define BAS_C_CurrentTarget  BAS_C_Target_PS2



#define ONLY_PSX2_ALIGNED(a)   __attribute__((aligned(a)))
#define ONLY_XBOX_ALIGNED(a)
/*$4
    ***********************************************************************************************************************
        PS2 basic types
    ***********************************************************************************************************************
    */
    /* $off */
    /* recommended on the project : */
    typedef char				CHAR;
    typedef short				SHORT;
    typedef int					INT;
    typedef int					LONG;
    typedef unsigned char		UCHAR;
    typedef unsigned short		USHORT;
    typedef unsigned int		UINT,   ULONG;
    typedef int					BOOL;
    typedef float				FLOAT;

    typedef signed char			S8;
    typedef signed short		S16;
    typedef signed int			S32;
    typedef unsigned char		U8;
    typedef unsigned short		U16;
    typedef unsigned int		U32;
    typedef float				F32;

    typedef signed char			s8;
    typedef signed short		s16;
    typedef signed int			s32;
    typedef unsigned char		u8;
    typedef unsigned short		u16;
    typedef unsigned int		u32;
    typedef float				f32;


    typedef char				long8;
    typedef short               long16;
    typedef int					long32;
    typedef long				long64;
    typedef unsigned char		u_long8;
    typedef unsigned short		u_long16;
    typedef unsigned int		u_long32;
    typedef unsigned long		u_long64;
    typedef float               float32;
    typedef double              float64;
    typedef long8               int8;
    typedef long16              int16;
    typedef long32              int32;
    typedef long64              int64;
    typedef u_long8	            u_int8;
    typedef u_long16            u_int16;
    typedef u_long32            u_int32;
    typedef u_long64            u_int64;


#   if defined(__GNUC__) || defined (__CW__)
        typedef int                 int128      __attribute__((mode(TI)));
        typedef unsigned int        u_int128    __attribute__((mode(TI)));


#   endif /* __GNUC__ || __CW__ */

 

    /*$4
    ***********************************************************************************************************************
        PS2 types for windows compatibility
    ***********************************************************************************************************************
    */
    typedef u_long8             BYTE;
    typedef u_long16			WORD;
    typedef u_long32			DWORD;
    typedef CHAR*               LPCTSTR;
    typedef DWORD*              LPDWORD;
    typedef void*               LPVOID;
    typedef void*               HINSTANCE;
    typedef void*               HGLRC;
    typedef void*               HDC;
    typedef LONG				HRESULT;
    /* Types use for passing & returning polymorphic values */
    typedef UINT                WPARAM;
    typedef LONG                LPARAM;
    typedef LONG                LRESULT;

    typedef struct tagPOINT
    {
	    LONG	x;
	    LONG	y;
    } POINT;

    typedef struct tagRECT
    {
	    LONG	left;
	    LONG	top;
	    LONG	right;
	    LONG	bottom;
    } RECT, *LPRECT, *HWND;

    typedef struct tagMSG
    {
	    HWND	hwnd;
	    UINT	message;
	    WPARAM	wParam;
	    LPARAM	lParam;
	    UINT	time;
	    POINT	pt;
#   ifdef _MAC
	        UINT	lPrivate;
#   endif // _MAC
    } MSG;

    /*$4
    ***********************************************************************************************************************
        macros for windows compatibility
    ***********************************************************************************************************************
     */
#   ifndef WINGDIAPI
#       undef  WINGDIAPI
#       define WINGDIAPI
#   endif // WINGDIAPI

#   ifndef APIENTRY
#       define APIENTRY
#   endif // APIENTRY

#   ifndef CALLBACK
#       define CALLBACK    APIENTRY
#   endif // CALLBACK

#   ifdef  NULL
#       undef  NULL
#   endif // NULL

#   ifdef  __cplusplus
#       define  NULL	0
#   else // __cplusplus
#       define  NULL	((void *) 0)
#   endif // __cplusplus


#   ifdef  FALSE
#       undef  FALSE
#   endif // FALSE
#   define FALSE	    0

#   ifdef  TRUE
#       undef  TRUE
#   endif // TRUE
#   define TRUE	    !(FALSE)

#   ifndef NOMINMAX
#       ifndef  max
#           define  max(a, b)	(((a) > (b)) ? (a) : (b))
#       endif
#       ifndef  min
#           define  min(a, b)	(((a) < (b)) ? (a) : (b))
#       endif
#   endif // NOMINMAX

#   define MAKEWORD(a, b)	((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8))
#   define LOWORD(l)		((WORD) (l))
#   define HIWORD(l)		((WORD) (((DWORD) (l) >> 16) & 0xFFFF))

#   define E_FAIL			(HRESULT) 0x80004005
#   define S_OK			(HRESULT) 0
#   define WM_CLOSE		0x0010
#   define WM_QUIT			0x0012
#   define WM_SYSCOMMAND	0x0112
#   define WM_ACTIVATEAPP	0x001C
#   define SC_CLOSE		0xF060



//###################################################################################
//###################################################################################
// END OF #if defined( PSX2_TARGET )
#elif defined( _XBOX ) || defined( _PC_RETAIL ) || defined(_XENON)
//###################################################################################
//###################################################################################


#	if defined( _XENON )
#		define BAS_C_CurrentTarget  BAS_C_Target_Xenon
#	elif defined( _XBOX )
#   define BAS_C_CurrentTarget  BAS_C_Target_XBox
#		define _XBOX_ASYNCLOAD
#	else
#		define BAS_C_CurrentTarget  BAS_C_Target_Pc
#	endif

#define ONLY_PSX2_ALIGNED(a)
#define ONLY_XBOX_ALIGNED(a) __declspec(align(a))


    typedef char			CHAR;
    typedef short			SHORT;
    typedef long			LONG;	/* specific */
    typedef int				INT;
    typedef unsigned char	UCHAR;
    typedef unsigned short	USHORT;
    typedef unsigned long	ULONG;
    typedef unsigned int	UINT;
    typedef unsigned long   DWORD;
    typedef int             BOOL;
    typedef __int64				LONG64;
    typedef unsigned __int64 	ULONG64;

    typedef int8_t				long8;
    typedef int16_t             long16;
    typedef int32_t				long32;
    typedef long long         long64;
    typedef uint8_t		        u_long8;
    typedef uint16_t		    u_long16;
    typedef uint32_t		    u_long32;
    typedef unsigned long long u_long64;

    typedef float               float32;
    typedef double              float64;
    typedef long8               int8;
    typedef long16              int16;
    typedef long32              int32;
    typedef long int           int64;
    typedef u_long8	            u_int8;
    typedef u_long16            u_int16;
    typedef u_long32            u_int32;
    typedef unsigned long int  u_int64;

    typedef signed char			S8;
    typedef signed short		S16;
    typedef signed int			S32;
    typedef unsigned char		U8;
    typedef unsigned short		U16;
    typedef unsigned int		U32;
    typedef float				F32;

    typedef signed char			s8;
    typedef signed short		s16;
    typedef signed int			s32;
    typedef unsigned char		u8;
    typedef unsigned short		u16;
    typedef unsigned int		u32;
    typedef float				f32;


    typedef u_long8             BYTE;
    typedef u_long16			WORD;
    typedef DWORD*              LPDWORD;
    typedef void*               LPVOID;
    typedef LONG				HRESULT;

    /* Types use for passing & returning polymorphic values */
    typedef UINT                WPARAM;
    typedef LONG                LPARAM;
    typedef LONG                LRESULT;

    /*
    typedef struct tagMSG
    {
	    HWND	hwnd;
	    UINT	message;
	    WPARAM	wParam;
	    LPARAM	lParam;
	    UINT	time;
	    POINT	pt;
#   ifdef _MAC
	    UINT	lPrivate;
#   endif
    } MSG;
*/

#   ifndef NOMINMAX
#       ifndef  max
#           define  max(a, b)	(((a) > (b)) ? (a) : (b))
#       endif
#       ifndef  min
#           define  min(a, b)	(((a) < (b)) ? (a) : (b))
#       endif
#   endif

#   define WM_CLOSE		0x0010
#   define WM_QUIT			0x0012
#   define WM_SYSCOMMAND	0x0112
#   define WM_ACTIVATEAPP	0x001C
#   define SC_CLOSE		0xF060

/*$4
 ***********************************************************************************************************************
    Other useful types.
 ***********************************************************************************************************************
 */

#   ifdef FALSE
#       undef FALSE
#   endif
#   define FALSE    false

#   ifdef TRUE
#       undef TRUE
#   endif
#   define TRUE     true

#   if defined( NULL )
#       undef NULL
#   endif
#   define NULL 0       // I thought this was already done somewhere else O:-)


//###################################################################################
//###################################################################################
// END OF #elif defined( _XBOX ) || defined( _PC_RETAIL )
#elif defined( _GAMECUBE )
//###################################################################################
//###################################################################################

#   define BAS_C_CurrentTarget  BAS_C_Target_GC
#ifdef JADEFUSION
#   define ONLY_GNUC_ALIGNED_2 
#   define ONLY_GNUC_ALIGNED_4 
#   define ONLY_GNUC_ALIGNED_8 
#   define ONLY_GNUC_ALIGNED_16
#endif
#define ONLY_PSX2_ALIGNED(a)
#define ONLY_XBOX_ALIGNED(a)

/* $on */
/*$4
 ***********************************************************************************************************************
    Basic integer types.
 ***********************************************************************************************************************
 */
    typedef char			CHAR;
    typedef short			SHORT;
    typedef long			LONG;	/* specific */
    typedef int				INT;
    typedef unsigned char	UCHAR;
    typedef unsigned short	USHORT;
    typedef unsigned long	ULONG;
    typedef unsigned int	UINT;
    typedef float			FLOAT;

    typedef signed char			S8;
    typedef signed short		S16;
    typedef signed int			S32;
    typedef s64					S64;
    typedef unsigned char		U8;
    typedef unsigned short		U16;
    typedef unsigned int		U32;
    typedef u64					U64;
    typedef float				F32;

    typedef int 				bool;

    typedef char				long8;
    typedef short               long16;
    typedef int					long32;
    typedef long long			long64; 
    typedef unsigned char		u_long8;
    typedef unsigned short		u_long16;
    typedef unsigned int		u_long32;
    typedef unsigned long long	u_long64;
    typedef float               float32;
    typedef double              float64;
    typedef long8               int8;
    typedef long16              int16;
    typedef long32              int32;
    typedef long64              int64;
    typedef u_long8	            u_int8;
    typedef u_long16            u_int16;
    typedef u_long32            u_int32;
    typedef u_long64            u_int64;


    typedef u_long8             BYTE;
    typedef u_long16			WORD;
    typedef u_long32			DWORD;
    typedef CHAR*               LPCTSTR;
    typedef DWORD*              LPDWORD;
    typedef void*               LPVOID;
    typedef void*               HINSTANCE;
    typedef void*               HGLRC;
    typedef void*               HDC;
    typedef LONG				HRESULT;

    /* Types use for passing & returning polymorphic values */
    typedef UINT                WPARAM;
    typedef LONG                LPARAM;
    typedef LONG                LRESULT;

    typedef struct tagPOINT
    {
	    LONG	x;
	    LONG	y;
    } POINT;

    typedef struct tagRECT
    {
	    LONG	left;
	    LONG	top;
	    LONG	right;
	    LONG	bottom;
    } RECT, *LPRECT, *HWND;

    typedef struct tagMSG
    {
	    HWND	hwnd;
	    UINT	message;
	    WPARAM	wParam;
	    LPARAM	lParam;
	    UINT	time;
	    POINT	pt;
#   ifdef _MAC
	    UINT	lPrivate;
#   endif
    } MSG;

#   ifndef NOMINMAX
#       ifndef  max
#           define  max(a, b)	(((a) > (b)) ? (a) : (b))
#       endif
#       ifndef  min
#           define  min(a, b)	(((a) < (b)) ? (a) : (b))
#       endif
#   endif

#   define MAKEWORD(a, b)	((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8))
#   define LOWORD(l)		((WORD) (l))
#   define HIWORD(l)		((WORD) (((DWORD) (l) >> 16) & 0xFFFF))

#   define E_FAIL			(HRESULT) 0x80004005
#   define S_OK			(HRESULT) 0
#   define WM_CLOSE		0x0010
#   define WM_QUIT			0x0012
#   define WM_SYSCOMMAND	0x0112
#   define WM_ACTIVATEAPP	0x001C
#   define SC_CLOSE		0xF060

/*$4
 ***********************************************************************************************************************
    Other useful types.
 ***********************************************************************************************************************
 */

#   ifdef FALSE
#       undef FALSE
#   endif
#   define FALSE	0x00

#   ifdef TRUE
#       undef TRUE
#   endif
#   define TRUE	0xFF

#   ifdef NULL
#       undef NULL
#   endif
#   define NULL	0L

//###################################################################################
//###################################################################################
// END OF #elif defined( _GAMECUBE )
#elif defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL )
//###################################################################################
//###################################################################################

#define ONLY_PSX2_ALIGNED(a)
#define ONLY_XBOX_ALIGNED(a)

    typedef char			CHAR;
    typedef short			SHORT;
    typedef long			LONG;	/* specific */
    typedef int				INT;
    typedef unsigned char	UCHAR;
    typedef unsigned short	USHORT;
    typedef unsigned long	ULONG;
    typedef unsigned int	UINT;
    typedef unsigned long   DWORD;
    typedef int             BOOL;
    typedef __int64				LONG64;
    typedef unsigned __int64 	ULONG64;

    typedef signed char			S8;
    typedef signed short		S16;
    typedef signed int			S32;
    typedef unsigned char		U8;
    typedef unsigned short		U16;
    typedef unsigned int		U32;
    typedef float				F32;

    typedef signed char			s8;
    typedef signed short		s16;
    typedef signed int			s32;
    typedef unsigned char		u8;
    typedef unsigned short		u16;
    typedef unsigned int		u32;
    typedef float				f32;



    typedef int8_t				long8;
    typedef int16_t             long16;
    typedef int32_t				long32;
    typedef long long         long64;
    typedef uint8_t		        u_long8;
    typedef uint16_t		    u_long16;
    typedef uint32_t		    u_long32;
    typedef unsigned long long u_long64;
    typedef float               float32;
    typedef double              float64;
    typedef long8               int8;
    typedef long16              int16;
    typedef long32              int32;
    typedef long int          int64;
    typedef u_long8	            u_int8;
    typedef u_long16            u_int16;
    typedef u_long32            u_int32;
    //typedef unsigned long int u_int64;

    typedef u_long8             BYTE;
    typedef u_long16			WORD;
    typedef DWORD*              LPDWORD;
    typedef void*               LPVOID;
    typedef LONG				HRESULT;

    /* Types use for passing & returning polymorphic values */
    typedef UINT                WPARAM;
    typedef LONG                LPARAM;
    typedef LONG                LRESULT;

    /*
    typedef struct tagMSG
    {
	    HWND	hwnd;
	    UINT	message;
	    WPARAM	wParam;
	    LPARAM	lParam;
	    UINT	time;
	    POINT	pt;
#   ifdef _MAC
	    UINT	lPrivate;
#   endif
    } MSG;
*/

#   ifndef NOMINMAX
#       ifndef  max
#           define  max(a, b)	(((a) > (b)) ? (a) : (b))
#       endif
#       ifndef  min
#           define  min(a, b)	(((a) < (b)) ? (a) : (b))
#       endif
#   endif

#   define WM_CLOSE		0x0010
#   define WM_QUIT			0x0012
#   define WM_SYSCOMMAND	0x0112
#   define WM_ACTIVATEAPP	0x001C
#   define SC_CLOSE		0xF060

/*$4
 ***********************************************************************************************************************
    Other useful types.
 ***********************************************************************************************************************
 */

#   ifdef FALSE
#       undef FALSE
#   endif
#   define FALSE    false

#   ifdef TRUE
#       undef TRUE
#   endif
#   define TRUE     true

#   if defined( NULL )
#       undef NULL
#   endif
#   define NULL 0       // I thought this was already done somewhere else O:-)


//###################################################################################
//###################################################################################
// END OF #elif defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL )
#else 
//###################################################################################
//###################################################################################
#   error TODO: Unsupported platform
#endif /* platform selection */


#ifdef WIN32
#ifndef ACTIVE_EDITORS
//#define USE_DOUBLE_RENDERING
#endif
#endif


#endif /* __BASTYPES_H__ */
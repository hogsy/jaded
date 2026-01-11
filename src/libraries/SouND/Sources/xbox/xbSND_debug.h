/*$T xbSND_debug.h GC! 1.081 01/15/03 10:52:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __xbSND_debug_h__
#define __xbSND_debug_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    GET ERROR
 ***********************************************************************************************************************
 */

#ifdef _DEBUG
extern DWORD	xbSND_gi_LastErrorCode;
#define xbSND_M_GetLastError() \
	do \
	{ \
		xbSND_gi_LastErrorCode = GetLastError(); \
	} while(0);
#else
#define xbSND_M_GetLastError()
#endif

/*$4
 ***********************************************************************************************************************
    ASSERT
 ***********************************************************************************************************************
 */
#ifdef _DEBUG
#define xbSND_ENABLE_ASSERT
#endif

#ifdef xbSND_ENABLE_ASSERT

/*$2- ASSERT ENABLED -------------------------------------------------------------------------------------------------*/

#define xbSND_M_Assert(__Expr) \
	do \
	{ \
		char	sBuf[500]; \
		if(!(__Expr)) \
		{ \
			wsprintf(sBuf, "[SND] assertion failed %s(%d) : %s\n", __FILE__, __LINE__, #__Expr); \
			OutputDebugString(sBuf); \
			CallerBreak(); \
		} \
	} while(0);

#else

/*$2- ASSERT DISABLED ------------------------------------------------------------------------------------------------*/

#define xbSND_M_Assert(__Expr)
#endif

/*$4
 ***********************************************************************************************************************
    TRACE
 ***********************************************************************************************************************
 */

//#define xbSND_ENABLE_TRACE
#ifdef xbSND_ENABLE_TRACE

/*$2- TRACE ENABLED --------------------------------------------------------------------------------------------------*/

#define xbSND_M_Trace(__Str) \
	do \
	{ \
		char	sBuf[256]; \
		wsprintf(sBuf, "[SND] %s\n", __Str); \
		OutputDebugString(sBuf); \
	} while(0);

#define xbSND_M_TraceS(__Id, __Str) \
	do \
	{ \
		char	sBuf[500]; \
		wsprintf(sBuf, "[SND] S%d %s\n", __Id, __Str); \
		OutputDebugString(sBuf); \
	} while(0);

#define xbSND_M_BeginTraceBlock(__Str) \
	do \
	{ \
		char	asz_Log[128]; \
		sprintf __Str;

#define xbSND_M_EndTraceBlock() \
	xbSND_M_Trace(asz_Log); \
} while(0);

#else

/*$2- TRACE DISABLED -------------------------------------------------------------------------------------------------*/

#define xbSND_M_Trace(a)
#define xbSND_M_TraceS(a, b)
#define xbSND_M_BeginTraceBlock(__Str) \
	if(0) \
	{
#define xbSND_M_EndTraceBlock() \
}
#endif

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void	CallerBreak(void);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __xbSND_debug_h__ */

/*$T ERRasser.h GC! 1.081 07/13/00 17:46:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif // PSX2_TARGET
#ifndef __ERRASSER_H__
#define __ERRASSER_H__

#ifndef PSX2_TARGET
#ifndef _GAMECUBE
#if !defined(_XBOX) && !defined(_XENON)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 if !PSX2_TARGET
 -----------------------------------------------------------------------------------------------------------------------
 */
#include "BASe/BAStypes.h"
#include "BASe/BASsys.h"
#include "BASe/ERRors/ERRdefs.h"
#include "BASe/CLIbrary/CLIxxx.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplus */

/* For AI */
extern L_jmp_buf	AI_gst_ContextCheck;
extern BOOL			ENG_gb_AIRunning;

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Macro to mark the beginning of an exception statement. All code between _Try_ and _Catch_ is then protected
            against software errors (errors and assert).
 =======================================================================================================================
 */
#define __Try__() \
	{ \
		_ERR_tdst_Exception __st_Exception__; \
		__st_Exception__.pst_Previous = _ERR_gpst_LastException; \
		_ERR_gpst_LastException = &__st_Exception__; \
		if(L_setjmp(__st_Exception__.st_Buf) == 0) \
		{
/*
 =======================================================================================================================
    Aim:    Macro to catch an error. This must be always defined to end the exception statement.
 =======================================================================================================================
 */
#define __Catch__() \
	_ERR_gpst_LastException = __st_Exception__.pst_Previous; \
} \
else \
{
/*
 =======================================================================================================================
    Aim:    Macro to put at the end of the catch statement. Will go to the last exception handler, or to
            ERR_LastException function if none yet.
 =======================================================================================================================
 */
#define __EndThrow__() \
	_ERR_gpst_LastException = __st_Exception__.pst_Previous; \
	if(_ERR_gpst_LastException != NULL) \
	{ \
		L_longjmp(_ERR_gpst_LastException->st_Buf, 1); \
	} \
	else \
	{ \
		ERR_LastException(); \
	} \
} \
}
/*
 =======================================================================================================================
    Aim:    Macro to replace _EndThrow_ when we want to resume error instead of going to previous exception handler.
 =======================================================================================================================
 */
#define __End__() \
	_ERR_gpst_LastException = __st_Exception__.pst_Previous; \
} \
}

/*$3
 =======================================================================================================================
    Just to avoid to define __Try__() instead of a simple _Try_ without parenthesis.
 =======================================================================================================================
 */

/*$off*/
#ifdef RELEASE

#ifdef __cplusplus
#define _Try_		try { __Try__()
#define _Catch_		__Catch__()
#define _End_		__End__() } catch(...) { ERR_X_ForceError("Exception !!!!", NULL); }
#define _EndThrow_	__EndThrow__() } catch(...) { ERR_X_ForceErrorThrow("Exception !!!!", NULL); }
#else /* __cplusplus */
#define _Try_		__try { __Try__()
#define _Catch_		__Catch__()
#define _End_		__End__() } __except(1) { ERR_X_ForceError("Exception !!!!", NULL); }
#define _EndThrow_	__EndThrow__() } __except(1) { ERR_X_ForceErrorThrow("Exception !!!!", NULL); }
#endif /* __cplusplus */

#else /* RELEASE */
#define _Try_		__Try__()
#define _Catch_		__Catch__()
#define _End_		__End__()
#define _EndThrow_	__EndThrow__()
#endif /* RELEASE */
/*$on*/

/*
 =======================================================================================================================
    Aim:    Macro to enable a return in a function body. Can be called in _Catch_ section to resume the error. Must be
            used instead of the normal return in a try/catch section.
 =======================================================================================================================
 */
#define _Return_(__ReturnValue) \
	{ \
		/* Normal return */ \
		_ERR_gpst_LastException = __st_Exception__.pst_Previous; \
		return __ReturnValue; \
	}
#else /* !ACTIVE_EDITORS */

/*$3
 =======================================================================================================================
    Aim:    In engine mode, there's no exception handling
 =======================================================================================================================
 */

#define _Try_
#define _Catch_ if(0) \
	{
#define _End_ \
}
#define _EndThrow_ \
}
#define _Return_(a) return a
#endif /* ACTIVE_EDITORS */

/*$4
 ***********************************************************************************************************************
    Main macro to errors and assertions.
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Macro to force the execution to go to the last exception handler
 =======================================================================================================================
 */
#define ERR_X_ForceThrow() \
	{ \
		if(_ERR_gpst_LastException != NULL) \
		{ \
			L_longjmp(_ERR_gpst_LastException->st_Buf, 1); \
		} \
		else \
		{ \
			ERR_LastException(); \
		} \
	}

/*
 =======================================================================================================================
    Aim:    Common macro to raise an error. Private part that can't be used directly

    Note:   Editor function. Exception are present.

    In:     __b_Assert  TRUE for an assertion, FALSE for an error.
            __Expr      Expression evaluate (the expression that failed). Used to print it in debug mode.
            __T1        An optionnal parameter to print.
            __T2        A second optionnal parameter to print.
 =======================================================================================================================
 */
#define _X_ErrorRaised__(__b_Assert, __Expr, __T1, __T2) \
	{ \
		if(_ERR_fnb_AssertFailed(__b_Assert, __FILE__, __LINE__, #__Expr, (char *) __T1, (char *) __T2, TRUE)) \
		{ \
			_breakpoint_; \
		} \
		ERR_X_ForceThrow(); \
	}
#else /* !ACTIVE_EDITORS */
#define ERR_X_ForceThrow()

#ifdef _DEBUG

/*
 =======================================================================================================================
    Aim:    The same as before, exception that in debug mode, we can set a breakpoint

    Note:   Engine function. There's no exception handling. After breakpoint, continue normally.
 =======================================================================================================================
 */
#define _X_ErrorRaised__(__b_Assert, __Expr, __T1, __T2) \
	{ \
		if(_ERR_fnb_AssertFailed(__b_Assert, __FILE__, __LINE__, #__Expr, (char *) __T1, (char *) __T2, TRUE)) \
		{ \
			_breakpoint_; \
		} \
	}
#else /* _DEBUG */

/*
 =======================================================================================================================
    Aim:    The same as before, but in release mode. No exception, and no breakpoint.
 =======================================================================================================================
 */
#define _X_ErrorRaised__(__b_Assert, __Expr, __T1, __T2) \
	{ \
		_ERR_fnb_AssertFailed(FALSE, __FILE__, __LINE__, #__Expr, (char *) __T1, (char *) __T2, TRUE); \
	}
#endif /* _DEBUG */
#endif /* ACTIVE_EDITORS */
#ifndef NDEBUG

/*
 * Aim: Macro to check if an expression is true. Must be used for debugging only
 * (in final mode, macro is empty).
 */
#ifdef ACTIVE_EDITORS
#define ERR_X_Assert(__Expr) \
	{ \
		if(!(__Expr)) \
		{ \
			if(ENG_gb_AIRunning) \
			{ \
				ERR_X_ForceError(#__Expr, NULL); \
				L_longjmp(AI_gst_ContextCheck, 1); \
			} \
			else \
			{ \
				_X_ErrorRaised__(TRUE, __Expr, NULL, NULL); \
			} \
		} \
	}
#else /* ACTIVE_EDITORS */
#define ERR_X_Assert(__Expr) \
	{ \
		if(!(__Expr)) \
		{ \
			_X_ErrorRaised__(TRUE, __Expr, NULL, NULL); \
		} \
	}
#endif /* ACTIVE_EDITORS */

/*
 * Aim: Raises an error then breaks in the code In: __Expr Expression to evaluate
 * (error is raised if expression is false). __T1 First optionnal parameter. __T2
 * Second optionnal parameter.
 */
#ifdef ACTIVE_EDITORS
#define ERR_X_ErrorAssert(__Expr, __T1, __T2) \
	{ \
		if(!(__Expr)) \
		{ \
			if(ENG_gb_AIRunning) \
			{ \
				ERR_X_ForceError(#__Expr, NULL); \
				L_longjmp(AI_gst_ContextCheck, 1); \
			} \
			else \
			{ \
				_X_ErrorRaised__(TRUE, __Expr, __T1, __T2); \
			} \
		} \
	} 
#else /* ACTIVE_EDITORS */
#define ERR_X_ErrorAssert(__Expr, __T1, __T2) \
	{ \
		if(!(__Expr)) \
		{ \
			_X_ErrorRaised__(TRUE, __Expr, __T1, __T2); \
		} \
	}
#endif /*ACTIVE_EDITORS */

/* Aim: Macro to force an assertion to fail */
#ifdef ACTIVE_EDITORS
#define ERR_X_ForceAssert() \
	{ \
		if(ENG_gb_AIRunning) \
		{ \
			ERR_X_ForceError("Assert", NULL); \
			L_longjmp(AI_gst_ContextCheck, 1); \
		} \
		else \
		{ \
			_X_ErrorRaised__(TRUE, NULL, NULL, NULL); \
		} \
	}
#else /* ACTIVE_EDITORS */
#define ERR_X_ForceAssert() \
	{ \
		_X_ErrorRaised__(TRUE, NULL, NULL, NULL); \
	}
#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
    Aim:    Macro to force an assertion to fail
 =======================================================================================================================
 */
#define ERR_X_Warning(__Expr, __T1, __T2) \
	{ \
		if(!(__Expr)) \
		{ \
			ERR_gb_Warning = TRUE; \
			_ERR_fnb_AssertFailed(FALSE, __FILE__, __LINE__, #__Expr, (char *) __T1, (char *) __T2, FALSE); \
		} \
	}
#else /* NDEBUG */

/*
 =======================================================================================================================
    Aim:    In final mode, all assert are empty.
 =======================================================================================================================
 */
#define ERR_X_ErrorAssert(__Expr, __T1, __T2)
#define ERR_X_Assert(__Expr)
#define ERR_X_Warning(__Expr, __T1, __T2)
#define ERR_X_ForceAssert()
#define ERR_X_AssertAddress(__Addr)
#endif /* !NDEBUG */

/*
 * Aim: To raise errors. In: __Expr Expression to evaluate (error is raised if
 * expression is false). __T1 First optionnal parameter. __T2 Second optionnal
 * parameter.
 */
#ifdef ACTIVE_EDITORS
#define ERR_X_Error(__Expr, __T1, __T2) \
	{ \
		if(!(__Expr)) \
		{ \
			if(ENG_gb_AIRunning) \
			{ \
				ERR_X_ForceError(#__Expr, NULL); \
				L_longjmp(AI_gst_ContextCheck, 1); \
			} \
			else \
			{ \
				_X_ErrorRaised__(FALSE, __Expr, __T1, __T2); \
			} \
		} \
	}
#else /* ACTIVE_EDITORS */
#define ERR_X_Error(__Expr, __T1, __T2) \
	{ \
		if(!(__Expr)) \
		{ \
			_X_ErrorRaised__(FALSE, __Expr, __T1, __T2); \
		} \
	}
#endif /* ACTIVE_EDITORS */
#define ERR_X_ForceError(__T1, __T2) \
	{ \
		_ERR_fnb_AssertFailed(FALSE, __FILE__, __LINE__, NULL, (char *) __T1, (char *) __T2, TRUE); \
	}
#define ERR_X_ForceErrorThrow(__T1, __T2) \
	{ \
		_X_ErrorRaised__(FALSE, NULL, __T1, __T2); \
	}

#				define ERR_X_ScriptError( MSG ) ERR_ScriptAssertFailed( __FILE__, __LINE__, NULL, MSG )

/*$4
 ***********************************************************************************************************************
    Breakpoint definition
 ***********************************************************************************************************************
 */

#ifdef _DEBUG
#define _breakpoint_	__asm \
	{ \
		int 0x03 \
	}
#else /* _DEBUG */
#define _breakpoint_
#endif /* _DEBUG */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif // _XBOX
#endif /* !PSX2_TARGET */
#endif /* !_GAMECUBE */



#endif /* __ERRASSER_H__ */

/*$T gcSND_Debug.h GC! 1.097 05/28/02 13:25:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __gcSND_Debug_h__
#define __gcSND_Debug_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define __gcSND_Dbg_Err__

#ifndef _DEBUG
#undef __gcSND_Dbg_Err__
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    errors
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef __gcSND_Dbg_Err__
#define gcSND_Str_ErrHdr	"**[SND] "

void gcSND_PrintAssert(const char *_szFormat, ...);

#define gcSND_M_Assert(__cond__) \
	do \
	{ \
		if(!(__cond__)) \
		{ \
			gcSND_PrintAssert(gcSND_Str_ErrHdr "%s(%d): ASSERTION FAILED \"" #__cond__ "\" \n", __FILE__, __LINE__); \
		} \
	} while(0);
	
#define gcSND_M_AssertX(__cond__, __fct__, __msg__, ...) \
	do \
	{ \
		if(!(__cond__)) \
		{ \
			gcSND_PrintAssert(gcSND_Str_ErrHdr "%s(%d)"#__fct__ " -> "__msg__ "\n", __FILE__, __LINE__, __VA_ARGS__); \
		} \
	} while(0);

#define gcSND_M_Err(__fct__, __msg__) \
	gcSND_PrintAssert \
	( \
		gcSND_Str_ErrHdr "%s(%d)"#__fct__ " -> "__msg__ "\n", \
		__FILE__, \
		__LINE__ \
	)
	
#define gcSND_M_ErrX(__fct__, __msg__, ...) \
	gcSND_PrintAssert \
	( \
		gcSND_Str_ErrHdr "%s(%d)"#__fct__ " -> "__msg__ "\n", \
		__FILE__, \
		__LINE__, \
		__VA_ARGS__ \
	)
	
#else
#define gcSND_M_Assert(__cond__)
#define gcSND_M_AssertX(__cond__, __fct__, __msg__, ...)
#define gcSND_M_Err(__fct__, __msg__)
#define gcSND_M_ErrX(__fct__, __msg__, ...)
#endif /* __gcSND_Dbg_Err__ */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    SPY
 -----------------------------------------------------------------------------------------------------------------------
 */

//#define GAMECUBE_SNDSPY 
#ifdef GAMECUBE_SNDSPY
void	gcSND_StreamSpy(unsigned int _ui_Val, char *func, char *file, int line);
#define SPY(val, func)	gcSND_StreamSpy((unsigned int) (val), #func, __FILE__, __LINE__)
#else
#define SPY(val, func)
#endif

void gcSND_GetCurrentFxModeName(char *pz, int aux);
void gcSND_UpdateCurrentFxMode(int aux, int update);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __gcSND_Debug_h__ */

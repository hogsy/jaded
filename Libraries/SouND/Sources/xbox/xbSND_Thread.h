/*$T xbSND_Thread.h GC 1.138 01/19/05 12:01:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __xbSND_Thread_h__
#define __xbSND_Thread_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

typedef enum	xbSND_tden_StreamEvent_
{
	xbSND_e_EventExitThread	= 0,
/**/
	xbSND_e_EventReinitS0,
	xbSND_e_EventReinitS1,
	xbSND_e_EventReinitS2,
	xbSND_e_EventReinitS3,
	xbSND_e_EventReinitS4,
	xbSND_e_EventReinitS5,
	xbSND_e_EventReinitS6,
	xbSND_e_EventReinitS7,
/**/
	xbSND_e_EventReadDoneS0,
	xbSND_e_EventReadDoneS1,
	xbSND_e_EventReadDoneS2,
	xbSND_e_EventReadDoneS3,
	xbSND_e_EventReadDoneS4,
	xbSND_e_EventReadDoneS5,
	xbSND_e_EventReadDoneS6,
	xbSND_e_EventReadDoneS7,
/**/
	xbSND_e_EventNotifyS0_A,
	xbSND_e_EventNotifyS0_B,
	xbSND_e_EventNotifyS1_A,
	xbSND_e_EventNotifyS1_B,
	xbSND_e_EventNotifyS2_A,
	xbSND_e_EventNotifyS2_B,
	xbSND_e_EventNotifyS3_A,
	xbSND_e_EventNotifyS3_B,
	xbSND_e_EventNotifyS4_A,
	xbSND_e_EventNotifyS4_B,
	xbSND_e_EventNotifyS5_A,
	xbSND_e_EventNotifyS5_B,
	xbSND_e_EventNotifyS6_A,
	xbSND_e_EventNotifyS6_B,
	xbSND_e_EventNotifyS7_A,
	xbSND_e_EventNotifyS7_B,
/**/
	xbSND_e_EventEndHitS0,
	xbSND_e_EventEndHitS1,
	xbSND_e_EventEndHitS2,
	xbSND_e_EventEndHitS3,
	xbSND_e_EventEndHitS4,
	xbSND_e_EventEndHitS5,
	xbSND_e_EventEndHitS6,
	xbSND_e_EventEndHitS7,
/**/
	xbSND_e_EventPrefetch0Done,
	xbSND_e_EventPrefetch1Done,
	xbSND_e_EventPrefetch2Done,
	xbSND_e_EventPrefetch3Done,
	xbSND_e_EventPrefetch4Done,
	xbSND_e_EventPrefetch5Done,
	xbSND_e_EventPrefetch6Done,
	xbSND_e_EventPrefetch7Done,
/**/
	xbSND_e_EventReloadDone,
	xbSND_e_EventDoReload,
/**/
	xbSND_e_EventAsyncLoading,	/* LEAR: Added for use with xboxStreamingEmul */
/**/
	xbSND_e_EventNb
} xbSND_tden_StreamEvent;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern HANDLE	xbSND_gah_Event[xbSND_e_EventNb];
extern HANDLE	xbSND_gh_ThreadHandler;
extern HANDLE	xbSND_gh_BigfileHandler;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

DWORD WINAPI	xbSND_Thread(LPVOID lp);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __xbSND_Thread_h__ */

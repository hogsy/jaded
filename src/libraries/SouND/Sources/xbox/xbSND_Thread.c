/*$T xbSND_Thread.c GC 1.138 01/19/05 12:13:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef _XBOX

/*$4
 ***********************************************************************************************************************
    Headers
 ***********************************************************************************************************************
 */

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDstruct.h"

#include "SouND/Sources/xbox/xbSND.h"
#include "SouND/Sources/xbox/xbSND_Stream.h"
#include "SouND/Sources/xbox/xbSND_Thread.h"
#include "SouND/Sources/xbox/xbSND_debug.h"

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

HANDLE	xbSND_gah_Event[xbSND_e_EventNb];
HANDLE	xbSND_gh_ThreadHandler;
HANDLE	xbSND_gh_BigfileHandler;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
DWORD WINAPI xbSND_Thread(LPVOID lp)
{
	/*~~~~~~~~~~~~~~*/
	DWORD	dwEventId;
	/*~~~~~~~~~~~~~~*/

	while(1)
	{
		dwEventId = WaitForMultipleObjects(xbSND_e_EventNb, xbSND_gah_Event, FALSE, INFINITE);
		dwEventId = dwEventId - WAIT_OBJECT_0;
		switch(dwEventId)
		{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    EXIT
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case xbSND_e_EventExitThread:
			ResetEvent(xbSND_gah_Event[xbSND_e_EventExitThread]);
			ExitThread(0);
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    RELOADING
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case xbSND_e_EventReloadDone:
			ResetEvent(xbSND_gah_Event[xbSND_e_EventReloadDone]);
			xbSND_EventReloadDone();
			break;

		case xbSND_e_EventDoReload:
			ResetEvent(xbSND_gah_Event[xbSND_e_EventDoReload]);
			xbSND_EventDoReload();
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    STREAM
		 ---------------------------------------------------------------------------------------------------------------
		 */
        case xbSND_e_EventEndHitS0:
        case xbSND_e_EventEndHitS1:
        case xbSND_e_EventEndHitS2:
        case xbSND_e_EventEndHitS3:
        case xbSND_e_EventEndHitS4:
        case xbSND_e_EventEndHitS5:
        case xbSND_e_EventEndHitS6:
        case xbSND_e_EventEndHitS7:
            ResetEvent(xbSND_gah_Event[dwEventId]);
            xbSND_ThStreamEndHit(dwEventId - xbSND_e_EventEndHitS0);
            break;

		case xbSND_e_EventReinitS0:
		case xbSND_e_EventReinitS1:
		case xbSND_e_EventReinitS2:
		case xbSND_e_EventReinitS3:
		case xbSND_e_EventReinitS4:
		case xbSND_e_EventReinitS5:
		case xbSND_e_EventReinitS6:
		case xbSND_e_EventReinitS7:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThStreamReinit(dwEventId - xbSND_e_EventReinitS0);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case xbSND_e_EventReadDoneS0:
		case xbSND_e_EventReadDoneS1:
		case xbSND_e_EventReadDoneS2:
		case xbSND_e_EventReadDoneS3:
		case xbSND_e_EventReadDoneS4:
		case xbSND_e_EventReadDoneS5:
		case xbSND_e_EventReadDoneS6:
		case xbSND_e_EventReadDoneS7:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThStreamReadDone(dwEventId - xbSND_e_EventReadDoneS0);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case xbSND_e_EventNotifyS0_A:
		case xbSND_e_EventNotifyS0_B:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThStreamNotify(0, dwEventId - xbSND_e_EventNotifyS0_A);
			break;

		case xbSND_e_EventNotifyS1_A:
		case xbSND_e_EventNotifyS1_B:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThStreamNotify(1, dwEventId - xbSND_e_EventNotifyS1_A);
			break;

		case xbSND_e_EventNotifyS2_A:
		case xbSND_e_EventNotifyS2_B:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThStreamNotify(2, dwEventId - xbSND_e_EventNotifyS2_A);
			break;

		case xbSND_e_EventNotifyS3_A:
		case xbSND_e_EventNotifyS3_B:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThStreamNotify(3, dwEventId - xbSND_e_EventNotifyS3_A);
			break;

		case xbSND_e_EventNotifyS4_A:
		case xbSND_e_EventNotifyS4_B:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThStreamNotify(4, dwEventId - xbSND_e_EventNotifyS4_A);
			break;

		case xbSND_e_EventNotifyS5_A:
		case xbSND_e_EventNotifyS5_B:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThStreamNotify(5, dwEventId - xbSND_e_EventNotifyS5_A);
			break;

		case xbSND_e_EventNotifyS6_A:
		case xbSND_e_EventNotifyS6_B:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThStreamNotify(6, dwEventId - xbSND_e_EventNotifyS6_A);
			break;

		case xbSND_e_EventNotifyS7_A:
		case xbSND_e_EventNotifyS7_B:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThStreamNotify(7, dwEventId - xbSND_e_EventNotifyS7_A);
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    PREFETCH
		 ---------------------------------------------------------------------------------------------------------------
		 */

        case xbSND_e_EventPrefetch0Done:
		case xbSND_e_EventPrefetch1Done:
		case xbSND_e_EventPrefetch2Done:
		case xbSND_e_EventPrefetch3Done:
		case xbSND_e_EventPrefetch4Done:
		case xbSND_e_EventPrefetch5Done:
		case xbSND_e_EventPrefetch6Done:
		case xbSND_e_EventPrefetch7Done:
			ResetEvent(xbSND_gah_Event[dwEventId]);
			xbSND_ThPrefetchDone(dwEventId - xbSND_e_EventPrefetch0Done);
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    LEAR: Added for use with xboxStreamingEmul
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case xbSND_e_EventAsyncLoading:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				extern void xbox_ThreadedTask(void);
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				ResetEvent(xbSND_gah_Event[dwEventId]);
				xbox_ThreadedTask();
				break;
			}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    errors
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case WAIT_FAILED - WAIT_OBJECT_0:
			xbSND_M_GetLastError();
			ERR_X_Error(0, "MultiThreadSynchro failure !", NULL);
			break;

		default:
			xbSND_M_GetLastError();
			ERR_X_Error(0, "MultiThreadSynchro failure !", NULL);
			break;
		}
	}

	return 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* _XBOX */

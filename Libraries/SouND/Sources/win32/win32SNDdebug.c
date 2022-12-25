/*$T win32SNDdebug.c GC! 1.081 02/06/03 18:18:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
#include "Precomp.h"
#if defined(PCWIN_TOOL) || defined(_PC_RETAIL)
#include "SouND/Sources/win32/win32SNDdebug.h"

#include <dsound.h>
/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */
void win32SND_Assert(char*pz_Cond, char*file,int line)
{
	char asz_log[512];
	sprintf(asz_log, "[SND] assertion failed : %s %s(%d)\n",pz_Cond, file, line);
	OutputDebugString(asz_log);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_CheckHRESULT(HRESULT hr)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char	asz_log[512];
	char	asz_cause[128];
	/*~~~~~~~~~~~~~~~~~~~*/

	switch(hr)
	{
	case DS_OK:
		return;
	case DSERR_ALLOCATED:
		strcpy(asz_cause, "DSERR_ALLOCATED");
		break;
	case DSERR_CONTROLUNAVAIL:
		strcpy(asz_cause, "DSERR_CONTROLUNAVAIL");
		break;
	case DSERR_INVALIDPARAM:
		strcpy(asz_cause, "DSERR_INVALIDPARAM");
		break;
	case DSERR_INVALIDCALL:
		strcpy(asz_cause, "DSERR_INVALIDCALL");
		break;
	case DSERR_GENERIC:
		strcpy(asz_cause, "DSERR_GENERIC");
		break;
	case DSERR_PRIOLEVELNEEDED:
		strcpy(asz_cause, "DSERR_PRIOLEVELNEEDED");
		break;
	case DSERR_OUTOFMEMORY:
		strcpy(asz_cause, "DSERR_OUTOFMEMORY");
		break;
	case DSERR_BADFORMAT:
		strcpy(asz_cause, "DSERR_BADFORMAT");
		break;
	case DSERR_UNSUPPORTED:
		strcpy(asz_cause, "DSERR_UNSUPPORTED");
		break;
	case DSERR_NODRIVER:
		strcpy(asz_cause, "DSERR_NODRIVER");
		break;
	case DSERR_ALREADYINITIALIZED:
		strcpy(asz_cause, "DSERR_ALREADYINITIALIZED");
		break;
	case DSERR_NOAGGREGATION:
		strcpy(asz_cause, "DSERR_NOAGGREGATION");
		break;
	case DSERR_BUFFERLOST:
		strcpy(asz_cause, "DSERR_BUFFERLOST");
		break;
	case DSERR_OTHERAPPHASPRIO:
		strcpy(asz_cause, "DSERR_OTHERAPPHASPRIO");
		break;
	case DSERR_UNINITIALIZED:
		strcpy(asz_cause, "DSERR_UNINITIALIZED");
		break;
	case DSERR_NOINTERFACE:
		strcpy(asz_cause, "DSERR_NOINTERFACE");
		break;
	case DS_NO_VIRTUALIZATION:
		strcpy(asz_cause, "DS_NO_VIRTUALIZATION");
		break;
	//case DS_INCOMPLETE:
	//	strcpy(asz_cause, "DS_INCOMPLETE");
	//	break;
	case DSERR_ACCESSDENIED:
		strcpy(asz_cause, "DSERR_ACCESSDENIED");
		break;
	case DSERR_BUFFERTOOSMALL:
		strcpy(asz_cause, "DSERR_BUFFERTOOSMALL");
		break;
	case DSERR_DS8_REQUIRED:
		strcpy(asz_cause, "DSERR_DS8_REQUIRED");
		break;
	case DSERR_SENDLOOP:
		strcpy(asz_cause, "DSERR_SENDLOOP");
		break;
	case DSERR_BADSENDBUFFERGUID:
		strcpy(asz_cause, "DSERR_BADSENDBUFFERGUID");
		break;
	case DSERR_OBJECTNOTFOUND:
		strcpy(asz_cause, "DSERR_OBJECTNOTFOUND");
		break;
	default:
		strcpy(asz_cause, "other code");
		break;
	}

	sprintf(asz_log, "bad DirectSound return value %x, %s", hr, asz_cause);
	MessageBox(0, asz_log, "SND error", MB_OK);
}

#endif
/*$4EOF
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */


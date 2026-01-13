/*$T ediSNDdebug.c GC! 1.081 02/06/03 18:18:34 */


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
#ifdef ACTIVE_EDITORS

#	include <dsound.h>

#include "SouND/Sources/edi/ediSNDdebug.h"

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */
void ediSND_Assert(char*pz_Cond, char*file,int line)
{
	char asz_log[512];
	sprintf(asz_log, "[SND] assertion failed : %s %s(%d)\n",pz_Cond, file, line);
	OutputDebugString(asz_log);
}

const char *ediSND_GetErrorString( HRESULT hr )
{
	switch(hr)
	{
		case DS_OK:
			return "DS_OK";
		case DSERR_ALLOCATED:
			return "DSERR_ALLOCATED";
		case DSERR_CONTROLUNAVAIL:
			return "DSERR_CONTROLUNAVAIL";
		case DSERR_INVALIDPARAM:
			return "DSERR_INVALIDPARAM";
		case DSERR_INVALIDCALL:
			return "DSERR_INVALIDCALL";
		case DSERR_GENERIC:
			return "DSERR_GENERIC";
		case DSERR_PRIOLEVELNEEDED:
			return "DSERR_PRIOLEVELNEEDED";
		case DSERR_OUTOFMEMORY:
			return "DSERR_OUTOFMEMORY";
		case DSERR_BADFORMAT:
			return "DSERR_BADFORMAT";
		case DSERR_UNSUPPORTED:
			return "DSERR_UNSUPPORTED";
		case DSERR_NODRIVER:
			return "DSERR_NODRIVER";
		case DSERR_ALREADYINITIALIZED:
			return "DSERR_ALREADYINITIALIZED";
		case DSERR_NOAGGREGATION:
			return "DSERR_NOAGGREGATION";
		case DSERR_BUFFERLOST:
			return "DSERR_BUFFERLOST";
		case DSERR_OTHERAPPHASPRIO:
			return "DSERR_OTHERAPPHASPRIO";
		case DSERR_UNINITIALIZED:
			return "DSERR_UNINITIALIZED";
		case DSERR_NOINTERFACE:
			return "DSERR_NOINTERFACE";
		case DS_NO_VIRTUALIZATION:
			return "DS_NO_VIRTUALIZATION";
			/*No longer defined in DirectX 9
			case DS_INCOMPLETE:
				strcpy(asz_cause, "DS_INCOMPLETE");
				break;*/
		case DSERR_ACCESSDENIED:
			return "DSERR_ACCESSDENIED";
		case DSERR_BUFFERTOOSMALL:
			return "DSERR_BUFFERTOOSMALL";
		case DSERR_DS8_REQUIRED:
			return "DSERR_DS8_REQUIRED";
		case DSERR_SENDLOOP:
			return "DSERR_SENDLOOP";
		case DSERR_BADSENDBUFFERGUID:
			return "DSERR_BADSENDBUFFERGUID";
		case DSERR_OBJECTNOTFOUND:
			return "DSERR_OBJECTNOTFOUND";
		case CO_E_NOTINITIALIZED:
			return "CO_E_NOTINITIALIZED";
		default:
			return "Unknown";
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_CheckHRESULT(HRESULT hr)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char	asz_log[512];
	/*~~~~~~~~~~~~~~~~~~~*/

	if ( hr == DS_OK )
	{
		return;
	}

	const char *cause = ediSND_GetErrorString( hr );
	snprintf(asz_log, sizeof( asz_log ), "bad DirectSound return value %x, %s", hr, cause);
	MessageBox(0, asz_log, "SND error", MB_OK);
}

/*$4EOF
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#endif
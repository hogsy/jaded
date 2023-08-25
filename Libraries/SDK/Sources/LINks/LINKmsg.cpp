/*$T LINKmsg.cpp GC 1.138 12/09/03 13:59:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "EDImainframe.h"
#include "EDIstrings.h"
#include "EDItors/Sources/LOGfile/LOGmsg.h"

#include "../../Shared/MainSharedSystem.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

BOOL		LINK_gb_EscapeDetected = FALSE;
BOOL		LINK_gb_IgnoreEscape = FALSE;
BOOL		LINK_gb_CanLog = TRUE;
BOOL		LINK_gb_CanDisplay = TRUE;
BOOL		LINK_gb_CanAbort = TRUE;
BOOL		LINK_gb_UseSecond = FALSE;
static char *sgpsz_LongOperationName = NULL;
BOOL		LINK_gb_OnSameLine = FALSE;
COLORREF	LINK_gul_ColorTxt = 0;


/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim: To print a message in the status bar. In: _psz_Text Text to display.
 =======================================================================================================================
 */
void LINK_PrintStatusMsgEOLCanal(char *_psz_Text,int _iCanal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static char msg[4096] = { 0 };
	EDI_cl_BaseFrame	*po_Editor;
	char *pz, *pzb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!LINK_gb_CanLog) return;
	if ( strlen(msg) > 1024) 
		strcpy( msg, "Overflow DBG_Trace\\n" );
	else
		L_strcat(msg, _psz_Text);
		
	pz = pzb = msg;
	while(*pz)
	{
		if(pz[0] == '\\' && pz[1] == 'n')
		{
			*pz = 0;
			if ( jaded::sys::launchOperations.editorMode )
			{
				po_Editor = M_MF()->po_GetEditorByType( EDI_IDEDIT_LOGFILE, 0 );
				if ( po_Editor ) po_Editor->i_OnMessage( ELOG_MESSAGE_ADDLINE | ( ( _iCanal + 1 ) << 16 ), ( ULONG ) pzb, LINK_gul_ColorTxt );
			}
			
			if ( jaded::sys::launchOperations.debugConsole )
				printf( "%s\n", pzb );

			pz += 2;
			pzb = pz;
			continue;
		}

		pz++;
	}

	if(pzb != msg) 
	{
		if(*pzb == 0)
			*msg = 0;
		else
			L_memmove(msg, pzb, L_strlen(pzb) + 1);
	}
}

void LINK_PrintStatusMsgEOL(char *_psz_Text)
{
    LINK_PrintStatusMsgEOLCanal(_psz_Text,0);
}

/*
 =======================================================================================================================
    Aim: To print a message in the status bar. In: _psz_Text Text to display.
 =======================================================================================================================
 */
void LINK_PrintStatusMsgCanal(char *_psz_Text,int _iCanal)
{
#if !defined(XML_CONV_TOOL)
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString				o_Msg;
	EDI_cl_BaseFrame	*po_Editor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_gb_CanAbort = TRUE;

	/* Add LONG operation name before the text */
	if(LINK_gb_CanDisplay && _psz_Text && sgpsz_LongOperationName && !LINK_gb_OnSameLine)
	{
		o_Msg += sgpsz_LongOperationName;
		o_Msg += "  ";
		o_Msg += _psz_Text;
		_psz_Text = (char *) (LPCSTR) o_Msg;
	}

	LINK_gb_OnSameLine = FALSE;

	/* Display message in status */
	if(_psz_Text)
	{
		if ( jaded::sys::launchOperations.editorMode )
		{
#if defined( _WIN32 )
			if ( LINK_gb_CanDisplay ) M_MF()->mo_Status.SetPaneText( LINK_gb_UseSecond ? 1 : 0, _psz_Text );

			/* Update log editor if present */
			if ( LINK_gb_CanLog )
			{
				po_Editor = M_MF()->po_GetEditorByType( EDI_IDEDIT_LOGFILE, 0 );
				if ( po_Editor )
				{
					po_Editor->i_OnMessage( ELOG_MESSAGE_ADDLINE | ( ( _iCanal + 1 ) << 16 ), ( ULONG ) _psz_Text, LINK_gul_ColorTxt );
					po_Editor->UpdateWindow();
				}
			}
#endif
		}
		
		if ( jaded::sys::launchOperations.debugConsole )
			printf( "%s\n", _psz_Text );
	}

	LINK_gb_UseSecond = FALSE;
#endif
}

void LINK_PrintStatusMsg(char *_psz_Text)
{
#if !defined(XML_CONV_TOOL)
    LINK_PrintStatusMsgCanal(_psz_Text,1);
#endif
}

/*
 =======================================================================================================================
    Aim: Call to mark the beginning of a LONG operation. Display the "cancel" button in status bar. In: _psz_Name Name
    of the LONG operation to perform (to display in the status).
 =======================================================================================================================
 */
void LINK_BeginLongOperation(char *_psz_Name)
{
	sgpsz_LongOperationName = _psz_Name;
	LINK_gb_EscapeDetected = FALSE;
}

/*
 =======================================================================================================================
    Aim: To end a LONG operation. Hide "cancel" button.
 =======================================================================================================================
 */
void LINK_EndLongOperation(void)
{
	sgpsz_LongOperationName = NULL;
	LINK_gb_EscapeDetected = FALSE;
}
#endif /* ACTIVE_EDITORS */

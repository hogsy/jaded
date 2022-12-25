//////////////////////////////////////////////////////////////////////////
// Message types
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"
#include "MessageType.h"

//////////////////////////////////////////////////////////////////////////
// externs

extern char* AI_EvalFunc_TEXTToString_C( TEXT_tdst_Eval* );

//////////////////////////////////////////////////////////////////////////
// Message types

namespace MessageType
{
	const char* GetText( MsgID nMsgID )
	{
		TEXT_tdst_Eval text;
			text.i_FileKey = 0x35001D04;
			text.i_Id = 0x00000000;

		const char* szText = NULL;

		switch( nMsgID )
		{
			/*
			case LIVE_CONNECTED:
				text.i_Id = 0x35001D1F;
				break;

			case LIVE_SESSION_STARTED_OK:
				text.i_Id = 0x35001D20;
				break;

			case LIVE_CONNECTION_LOST:
				text.i_Id = 0x35001D21;
				break;

			case LIVE_DUPLICATE_LOGON:
				text.i_Id = 0x35001D22;
				break;

			case LIVE_CANT_CONNECT:
				text.i_Id = 0x35001D23;
				break;

			case LIVE_NO_NETWORK_CONNECTION:
				text.i_Id = 0x35001D24;
				break;

			case LIVE_CANT_ACCESS_SERVICE:
				text.i_Id = 0x35001D25;
				break;

			case LIVE_UPDATE_REQUIRED:
				text.i_Id = 0x35001D26;
				break;

			case LIVE_SERVER_BUSY:
				text.i_Id = 0x35001D27;
				break;

			case LIVE_INVALID_USER:
				text.i_Id = 0x35001D28;
				break;
			*/

			case ACHIEVEMENT_GAINED:
				text.i_Id = 0x35001D29;
				break;

			case ACHIEVEMENT_CANT_WRITE:
				text.i_Id = 0x35001D2A;
				break;

			default:
				ERR_X_Assert( false );
		}

		// read text from big file
		const char* szRequestedText = AI_EvalFunc_TEXTToString_C( &text );
		if( szRequestedText == NULL || strlen(szRequestedText) == 0 )
			ERR_X_Assert( false );

		return szRequestedText;
	}
}

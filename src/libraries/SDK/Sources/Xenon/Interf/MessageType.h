//////////////////////////////////////////////////////////////////////////
// Message types
//////////////////////////////////////////////////////////////////////////

#ifndef __MESSAGETYPE_H__
#define __MESSAGETYPE_H__

//////////////////////////////////////////////////////////////////////////
// Message types

namespace MessageType
{
	enum MsgID
	{
		LIVE_CONNECTED,
		LIVE_SESSION_STARTED_OK,
		LIVE_CONNECTION_LOST,
		LIVE_DUPLICATE_LOGON,
		LIVE_CANT_CONNECT,
		LIVE_NO_NETWORK_CONNECTION,
		LIVE_CANT_ACCESS_SERVICE,
		LIVE_UPDATE_REQUIRED,
		LIVE_SERVER_BUSY,
		LIVE_INVALID_USER,

		ACHIEVEMENT_GAINED,
		ACHIEVEMENT_CANT_WRITE,

		MsgID_INVALID
	};

	// text accessor
	const char* GetText( MsgID nMsgID );
}

#endif // __MESSAGETYPE_H__

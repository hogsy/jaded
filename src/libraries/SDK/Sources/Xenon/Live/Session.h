//////////////////////////////////////////////////////////////////////////
// Xenon Session
// ------------------------
// Manages the Xbox Live session
//////////////////////////////////////////////////////////////////////////

#ifndef __SESSION_H__
#define __SESSION_H__

#ifdef _XENON

extern bool g_XeLiveConnected;

// TCR modification (August 2005)
// it is no longer necessary to create a session for single player games

/*

#include <list>

//////////////////////////////////////////////////////////////////////////
// CXeLiveSession

class CXeLiveSession
{
// type definitions
public:
	enum XeSessionCommand
	{
		XE_SESSION_CONNECT,
		XE_SESSION_DISCONNECT
	};

// construction / destruction
public:
	CXeLiveSession( );
	~CXeLiveSession( );

// interface
public:
	bool	Initialize( );
	bool	Shutdown( );

	int		GetUserIndex( );
	void	SetUserIndex( int nIndex );

	void	ActivateSession( bool bActivate );

	bool	IsSessionActive( );
	void	UpdateSession( );

	void	SetLiveConnected( bool bConnected );
	bool	IsLiveConnected( ) const;

// implementation
private:
	bool	CreateSession( );
	bool	DestroySession( );

	bool	JoinSession( );
	bool	LeaveSession( );

	bool	StartSession( );
	bool	DeleteSession( );

	void	InternalCleanUp( );

	void	QueueCommand( XeSessionCommand nCommand );

// fields
private:
	typedef std::list< XeSessionCommand > CmdQueue;

	XOVERLAPPED		m_xov;
	int				m_nState;
	bool			m_bSessionCreated;
	char			m_szUserName[ 128 ];

	CmdQueue		m_lstCommands;
	bool			m_bProcessingCommand;

	XSESSION_INFO	m_SessionInfo;
	ULONGLONG		m_SessionNonce;
	HANDLE			m_hSession;

	DWORD			m_dwUserIndex;
	BOOL			m_bPrivateSlot;

	bool			m_bLiveConnected;

	int				m_nUserIndex;
};

//////////////////////////////////////////////////////////////////////////
// inline

inline int CXeLiveSession::GetUserIndex( )
{
	return m_nUserIndex;
}

inline void CXeLiveSession::SetUserIndex( int nIndex )
{
	m_nUserIndex = nIndex;
}

inline bool CXeLiveSession::IsSessionActive( )
{
	return m_hSession != INVALID_HANDLE_VALUE;
}

inline void CXeLiveSession::SetLiveConnected( bool bConnected )
{
	m_bLiveConnected = bConnected;
}

inline bool CXeLiveSession::IsLiveConnected( ) const
{
	return m_bLiveConnected;
}

//////////////////////////////////////////////////////////////////////////
// extern definition

extern CXeLiveSession g_XeLiveSession;

*/

#endif // _XENON

#endif // __SESSION_H__

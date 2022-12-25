//////////////////////////////////////////////////////////////////////////
// MessageBar
// ------------------------
// A quick way of displaying a string on screen at any point in the game
// Could be useful for debugging purposes too
//////////////////////////////////////////////////////////////////////////

#ifndef __MESSAGEBAR_H__
#define __MESSAGEBAR_H__

#include <list>
#include "MessageType.h"

#if defined(_XENON)

using namespace MessageType;

//////////////////////////////////////////////////////////////////////////
// CXeMessageBar

class CXeMessageBar
{
// construction / destruction
public:
	CXeMessageBar( const char* szMessage, const DWORD dwDisplayTime );  // time specified in milliseconds
	~CXeMessageBar( );

// interface
public:
	void		Display( );
	bool		IsCompleted( ) const;

// fields
private:
	wchar_t	m_wszWideChar[ 1024 ];

	float	m_fSizeX;
	float	m_fSizeY;

	DWORD	m_dwDisplayTime;

	DWORD	m_dwArriveTime;
	DWORD	m_dwLeaveTime;

	bool	m_bCompleted;
};

//////////////////////////////////////////////////////////////////////////
// CXeMessageBarManager

class CXeMessageBarManager
{
// construction
private:
	CXeMessageBarManager( );
public:
	~CXeMessageBarManager( );

// interface
public:
	static CXeMessageBarManager& Get( );

	void		AddMessage( CXeMessageBar* pDlgBox );
	void		AddMessage( DWORD dwMilliseconds, const char* szMessage );
	void		AddMessage( DWORD dwMilliseconds, MsgID nMsgID, MsgID nMsgID2 = MsgID_INVALID );

	void		Display( );

// implementation
private:
	void		DeleteCurrent( );

// fields
private:
	std::list< CXeMessageBar* >	m_lstBars;
	CRITICAL_SECTION			m_cs;
};

#endif // defined(_XENON)

#endif // __MESSAGEBAR_H__

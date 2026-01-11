//////////////////////////////////////////////////////////////////////////
// MessageBar
// ------------------------
// A quick way of displaying a string on screen at any point in the game
// Could be useful for debugging purposes too
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"
#include "MessageBar.h"

#include "..\Desktop\Desktop.h"

extern float TIM_gf_MainClock;

#if defined(_XENON)

//////////////////////////////////////////////////////////////////////////
// constants 

const float FONT_SCALING = 0.18f;
const DWORD SLIDE_LENGTH = 1000;

//////////////////////////////////////////////////////////////////////////
// CXeMessageBar

CXeMessageBar::CXeMessageBar( const char* szMessage, const DWORD dwDisplayTime )
	: m_bCompleted( false ),
	  m_dwArriveTime( 0 ),
	  m_dwLeaveTime( 0 ),
	  m_dwDisplayTime( dwDisplayTime )
{
	ZeroMemory( m_wszWideChar, sizeof( m_wszWideChar ) );
	const int nLength = strlen( szMessage );
	if( nLength > 0 )
		MultiByteToWideChar( CP_ACP, 0, szMessage, nLength, m_wszWideChar, sizeof( m_wszWideChar ) / sizeof( m_wszWideChar[0] ) );

#if defined(DESKTOP_ENABLE)
	GetStringSize( &m_fSizeX, &m_fSizeY, m_wszWideChar, FONT_SCALING, FONT_SCALING );
#endif
}

CXeMessageBar::~CXeMessageBar( )
{
}

void CXeMessageBar::Display( )
{
	// check if we are initialized
	if( m_dwArriveTime == 0 && m_dwLeaveTime == 0 )
	{
		m_dwArriveTime = DWORD( TIM_gf_MainClock * 1000 ) + SLIDE_LENGTH;
		m_dwLeaveTime = m_dwArriveTime + m_dwDisplayTime;
	}

	// calculate percent
	float fPercent = 1.0f;

	DWORD dwTime = (DWORD)( TIM_gf_MainClock * 1000 );

	if( dwTime < m_dwArriveTime )
		fPercent = 1.0f - float( m_dwArriveTime - dwTime ) / SLIDE_LENGTH;
	else if( dwTime > m_dwLeaveTime )
		fPercent = 1.0f - float( dwTime - m_dwLeaveTime ) / SLIDE_LENGTH;

	if( dwTime > ( m_dwLeaveTime + SLIDE_LENGTH ) )
		m_bCompleted = true;

#if defined(DESKTOP_ENABLE)
	// draw text
	float fPosX = 0.5f - ( m_fSizeX / 2 );
	float fPosY = -0.35f + ( ( 0.35f + 0.065f ) * fPercent );
	DrawString( m_wszWideChar, fPosX, fPosY, FONT_SCALING, FONT_SCALING, 0xFFAA5511 );
#endif
}

bool CXeMessageBar::IsCompleted( ) const
{
	return m_bCompleted;
}

//////////////////////////////////////////////////////////////////////////
// CXeMessageBarManager

CXeMessageBarManager::CXeMessageBarManager( )
{
	InitializeCriticalSection( &m_cs );
}

CXeMessageBarManager::~CXeMessageBarManager( )
{
	while( !m_lstBars.empty( ) )
		DeleteCurrent( );

	DeleteCriticalSection( &m_cs );
}

CXeMessageBarManager& CXeMessageBarManager::Get( )
{
	static CXeMessageBarManager instance;
	return instance;
}

void CXeMessageBarManager::AddMessage( CXeMessageBar* pDlgBox )
{
	EnterCriticalSection( &m_cs );
	m_lstBars.push_back( pDlgBox );
	LeaveCriticalSection( &m_cs );
}

void CXeMessageBarManager::AddMessage( DWORD dwMilliseconds, const char* szMessage )
{
	CXeMessageBar* pBar = new CXeMessageBar( szMessage, dwMilliseconds );
	AddMessage( pBar );
}

void CXeMessageBarManager::AddMessage( DWORD dwMilliseconds, MsgID nMsgID, MsgID nMsgID2 )
{
	std::string strMessage = GetText( nMsgID );
	if( nMsgID2 != MsgID_INVALID )
	{
		strMessage += "\n";
		strMessage += GetText( nMsgID2 );
	}

	AddMessage( dwMilliseconds, strMessage.c_str( ) );
}

void CXeMessageBarManager::DeleteCurrent( )
{
	if( !m_lstBars.empty( ) )
	{
		EnterCriticalSection( &m_cs );
		delete m_lstBars.front( );
		m_lstBars.pop_front( );
		LeaveCriticalSection( &m_cs );
	}
}

void CXeMessageBarManager::Display( )
{
	// quick-out
	if( m_lstBars.empty( ) )
		return;

	EnterCriticalSection( &m_cs );

	// display current bar
	CXeMessageBar* pBar = m_lstBars.front( );
	pBar->Display( );

	// clean up if done
	const bool bCompleted = m_lstBars.front( )->IsCompleted( );

	LeaveCriticalSection( &m_cs );

	if( bCompleted )
		DeleteCurrent( );
}

#endif // defined(_XENON)

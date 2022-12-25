#ifndef __VIDEONOLOOP_H__
#define __VIDEONOLOOP_H__

#ifdef _XENON

#include "Xmedia.h"

//////////////////////////////////////////////////////////////////////////
// CVideoNoLoop

class CVideoNoLoop
{
// type definitions
public:
	enum PlaybackMode
	{
		PLAY_LOOPING,
		PLAY_FREEZE_AT_END,
		PLAY_ALL_ONCE
	};

// construction / destruction
public:
    M_DeclareOperatorNewAndDelete();

	CVideoNoLoop( );
	~CVideoNoLoop( );

// interface
public:
	bool	Open( const char* szPath, BOOL bFromMemory = TRUE );
	bool	Close( );

	void	SetMode( PlaybackMode mode );

	bool	IsDone( );

	void	DoFrame( );

// fields
private:
	IXMediaXmvPlayer*		m_pMediaPlayer;
	bool					m_bDisplayGrab;
	IDirect3DStateBlock9*	m_pStateBlock;
	void*					m_pMemoryBuffer;
	bool					m_bIsLooping;
	bool					m_bPlayAllOnce;
	bool					m_bFreezeOnLastFrame;
};

#endif // _XENON

#endif // __VIDEONOLOOP_H__

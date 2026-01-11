#include "Precomp.h"

#ifdef _XENON

#include "..\..\INOut\INOjoystick.h"
#include "..\..\INOut\Xenon\XeINOjoystick.h"

#include "SouND/Sources/SNDvolume.h"

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "VideoNoLoop.h"

#include "VideoManager.h"
#include "XenonGraphics/XeTextureManager.h"
#include "XenonGraphics/XeRenderer.h"

// ***********************************************************************************************************************
//    Class Methods - CVideoNoLoop
// ***********************************************************************************************************************

CVideoNoLoop::CVideoNoLoop( )
	: m_pMediaPlayer( NULL ),
	  m_bDisplayGrab( false ),
	  m_pStateBlock( NULL ),
	  m_pMemoryBuffer( NULL )
{
	SetMode( PLAY_LOOPING );
}

// ***********************************************************************************************************************

CVideoNoLoop::~CVideoNoLoop( )
{
	Close( );
}

// ***********************************************************************************************************************

bool CVideoNoLoop::Open( const char* szPath, BOOL bFromMemory )
{
	HRESULT hResult;

    // Prepare Overlays

    g_pVideoManager->PrepareOverlay();

    if (bFromMemory)
	{
		// Open File

		HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		// Fetch Size

		unsigned int uiMemorySize = GetFileSize(hFile, NULL);

		if (uiMemorySize == 0)
		{
			CloseHandle( hFile );
			return FALSE;
		}

		// Allocate Buffer

		m_pMemoryBuffer = L_malloc(uiMemorySize);

		if (m_pMemoryBuffer == NULL)
		{
			CloseHandle( hFile );
			return FALSE;
		}

		// Read From File

		DWORD dwSizeRead;

		BOOL bSuccess = ReadFile(hFile, m_pMemoryBuffer, uiMemorySize, &dwSizeRead, NULL);

		if (!bSuccess || (dwSizeRead != uiMemorySize))
		{
			CloseHandle( hFile );
			return FALSE;
		}

		CloseHandle(hFile);

		// Create XMV Player

		hResult = XMediaCreateXmvPlayerFromMemory(g_oXeRenderer.GetDevice(), m_pMemoryBuffer, uiMemorySize, &m_pMediaPlayer);
	}
	else
	{
		hResult = XMediaCreateXmvPlayerFromFile(g_oXeRenderer.GetDevice(), szPath, &m_pMediaPlayer);
	}

	if (FAILED(hResult))
	{
		return false;
	}

	// Set Volume

	IXAudioSourceVoice* pSourceVoice = NULL;

	hResult = m_pMediaPlayer->GetSourceVoice(&pSourceVoice);

	if ((pSourceVoice != NULL) && (hResult == S_OK))
	{
		pSourceVoice->SetVolume(SND_gdst_Group[SND_e_UserGrpMaster].f_Volume * SND_gdst_Group[SND_e_MasterGrp].f_Volume);
	}

	// Prepare Subtitles

	g_pVideoManager->GetSubtitles()->Open(szPath);

	// Create State Block

	g_oRenderStateMgr.UnbindAll();

	// Create State Block
	g_oXeRenderer.GetDevice()->CreateStateBlock( D3DSBT_ALL, &m_pStateBlock );

	return true;
}

// ***********************************************************************************************************************

bool CVideoNoLoop::Close( )
{
	g_oXeRenderer.InvalidateRenderLists();

	// Destroy Overlay

	g_pVideoManager->DestroyOverlay();

	// Stop Subtitles

	g_pVideoManager->GetSubtitles()->Close();

	// Release Objects

	if( m_pStateBlock )
	{
		m_pStateBlock->Release( );
		m_pStateBlock = NULL;
	}

	if( m_pMediaPlayer )
	{
		XMEDIA_PLAYBACK_STATUS xmps;

		m_pMediaPlayer->Stop(XMEDIA_STOP_IMMEDIATE);

		int nCounter = 0;
		do
		{
			m_pMediaPlayer->GetStatus(&xmps);

			if( xmps.Status == XMEDIA_PLAYER_PLAYING )
				m_pMediaPlayer->RenderNextFrame( 0, NULL );

			++nCounter;
		}
		while( ( xmps.Status != XMEDIA_PLAYER_STOPPED ) && ( xmps.Status != XMEDIA_PLAYER_TERMINATING ) && nCounter < 10 );

		m_pMediaPlayer->Release( );
		m_pMediaPlayer = NULL;
	}

	if ( m_pMemoryBuffer )
	{
		L_free(m_pMemoryBuffer);
		m_pMemoryBuffer = NULL;
	}

	return true;
}

// ***********************************************************************************************************************

void CVideoNoLoop::SetMode( PlaybackMode mode )
{
	switch( mode )
	{
		case PLAY_LOOPING:
			m_bIsLooping = true;
			m_bPlayAllOnce = false;
			m_bFreezeOnLastFrame = false;
			break;

		case PLAY_ALL_ONCE:
			m_bIsLooping = false;
			m_bPlayAllOnce = true;
			m_bFreezeOnLastFrame = false;
			break;

		case PLAY_FREEZE_AT_END:
			m_bIsLooping = false;
			m_bPlayAllOnce = true;
			m_bFreezeOnLastFrame = true;
			break;

		default:
			ERR_X_Assert( false );
			break;
	}
}

// ***********************************************************************************************************************

bool CVideoNoLoop::IsDone( )
{
#ifndef _FINAL_
	INO_Joystick_Update( );
	if( INO_b_Joystick_IsButtonDown( eXeButton_Trigger_Left ) )
		return true;
#endif

	if( m_bIsLooping )
		return true;
	else if( m_bPlayAllOnce )
		return m_pMediaPlayer == NULL;
	else if( m_bFreezeOnLastFrame )
		return m_bDisplayGrab;
	else
		ERR_X_Assert( false );

	return true;
}

// ***********************************************************************************************************************

void CVideoNoLoop::DoFrame( )
{
	if( m_pMediaPlayer )
	{
		xeINO_ResetInactivityTimer( TimerAttractMode );

		// Backup D3D Renderstates

		m_pStateBlock->Capture( );

		// Play video frame

		g_oXeRenderer.GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL);

        // SC: Force the render state manager into thinking alpha blending is disabled since the state
        //     block will not restore the options set by SetBlendState...
        g_oRenderStateMgr.SetBlendState(FALSE);

		g_oXeRenderer.Clear( 0, 1.0f, 0 );

		const HRESULT hr = m_pMediaPlayer->RenderNextFrame(m_bIsLooping ? XMEDIA_PLAY_LOOP : 0, NULL );

		// Display Overlays

		g_pVideoManager->DisplayOverlay();

		// Display Subtitles

		g_pVideoManager->GetSubtitles()->Draw();

		// Restore Renderstates

		m_pStateBlock->Apply( );

		if( hr != XMEDIA_W_EOF )
		{
			// grab current screen in case it is the last
			if( m_bFreezeOnLastFrame )
				g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID );
		}
		else
		{
			// destroy movie
			Close( );

			// switch to displaying grabbed screen
			if( m_bFreezeOnLastFrame )
				m_bDisplayGrab = true;
		}
	}
    else
    {
		g_oXeRenderer.Clear( 0, 1.0f, 0 );

        // Display Overlays

        g_pVideoManager->DisplayOverlay();
    }

	if( m_bDisplayGrab )
	{
		// display last frame that was grabbed
		UINT nWidth, nHeight;
		g_oXeRenderer.GetBackbufferResolution( &nWidth, &nHeight );

		DrawRectangleEx
			(
			0.0f - (0.5f / nWidth  ),
			0.0f - (0.5f / nHeight ),
			1.0f - (0.5f / nWidth  ),
			1.0f - (0.5f / nHeight ),
			0.0f,
			0.0f,
			1.0f,
			1.0f,
			0xffffffff,
			0xffffffff,
			0xffffffff,
			0xffffffff,
			0.0f,
			MAT_Cc_Op_Copy,
			XESCRATCHBUFFER0_ID
			);
	}
}

// ***********************************************************************************************************************

#endif // _XENON

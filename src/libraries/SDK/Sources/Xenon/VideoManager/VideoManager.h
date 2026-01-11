// ************************************************
// Video Playback Manager
//
// Used for King Kong Xenon
// By Alexandre David (March 2005)
// ************************************************

#ifndef __VideoManager_h__
#define __VideoManager_h__

#if defined(_XENON)

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "Xmedia.h"

#include "VideoSubtitles.h"

// ***********************************************************************************************************************
//    Macros
// ***********************************************************************************************************************

#if !defined(CALLBACK)
#define CALLBACK __stdcall
#endif

// ***********************************************************************************************************************

namespace VideoManager
{

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const int	kFilename_MaxLength = 256;

const float	kfVideoOverlay_Loading_Screen_Width		= 0.60f;
const float	kfVideoOverlay_Loading_Screen_Height	= 0.06f;
const float	kfVideoOverlay_Loading_Screen_OffsetX	= 0.05f;
const float	kfVideoOverlay_Loading_Screen_OffsetY	= 0.05f;
const int	kiVideoOverlay_Loading_Format_Width		= 512;
const int	kiVideoOverlay_Loading_Format_Height	= 32;

const float	kfVideoOverlay_MapName_Screen_Width		= 0.80f;
const float	kfVideoOverlay_MapName_Screen_Height	= 0.08f;
const float	kfVideoOverlay_MapName_Screen_OffsetX	= 0.10f;
const float	kfVideoOverlay_MapName_Screen_OffsetY	= 0.05f;
const int	kiVideoOverlay_MapName_Format_Width		= 512;
const int	kiVideoOverlay_MapName_Format_Height	= 32;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CVideoManager
{
private:
	char				m_pFilename[kFilename_MaxLength];
	IXMediaXmvPlayer*	m_pMediaPlayer;
	void*				m_pMemoryBuffer;
	unsigned int		m_uiMemorySize;
	BOOL				m_bIsPlaying;
	BOOL				m_bIsPlaybackRequested;
	DWORD				m_dwStartTime;
	int					m_nMinPlayTime;
	IDirect3DTexture9*	m_pOverlay_Loading_Texture;
	int					m_iOverlay_Loading_ID;
	IDirect3DTexture9*	m_pOverlay_MapName_Texture;
	int					m_iOverlay_MapName_ID;
	CVideoSubtitles*	m_pVideoSubtitles;
	BOOL				m_bExclusivePlayback;
	BOOL				m_bXMPOverride;

public:
    M_DeclareOperatorNewAndDelete();

	CVideoManager();
	~CVideoManager();

	BOOL				IsPlaying();
	BOOL				HasRequest();
	void				SetPlaying(BOOL Param_bIsPlaying);
	IXMediaXmvPlayer*	GetPlayer();
	BOOL				Play(char* Param_pFilename, BOOL Param_bPlayFromMemory = FALSE, int Param_nMinPlayTime = 0);
	void				PlayExclusive(char* Param_pFilename);
	void				ManageExclusive();
	void				StopExclusive();
	void				Stop();
	BOOL				PerformRequests();
	void				PrepareOverlay();
	void				DestroyOverlay();
	void				DisplayOverlay();
	CVideoSubtitles*	GetSubtitles();

private:
	static void CALLBACK	VideoManager_Callback_BeginFrame(void* Param_pVideoManager);
	static void CALLBACK	VideoManager_Callback_EndFrame(void* Param_pVideoManager);
};

// ***********************************************************************************************************************
//    Functions
// ***********************************************************************************************************************

void VideoManager_Callback_BeginFrame(void* Param_pMediaPlayer);
void VideoManager_Callback_EndFrame(void* Param_pMediaPlayer);

// ***********************************************************************************************************************
//    Global Variables
// ***********************************************************************************************************************

extern __declspec(align(32)) CVideoManager* g_pVideoManager;

// ***********************************************************************************************************************

} // Namespace

using namespace VideoManager;

// ***********************************************************************************************************************

#endif // _XENON

#endif // __VideoManager_h__

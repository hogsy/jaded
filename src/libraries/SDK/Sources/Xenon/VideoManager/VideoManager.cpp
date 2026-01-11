// ************************************************
// Video Playback Manager
//
// Used for King Kong Xenon
// By Alexandre David (March 2005)
// ************************************************

#include "Precomp.h"

#if defined(_XENON)

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "VideoManager.h"

#include "VideoNoLoop.h"

#include "XenonGraphics/XeTextureManager.h"
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeRenderStateManager.h"
#include "INOut/INO.h"
#include "INOut/INOjoystick.h"
#include "SOUnd/Sources/Xenon/xeSND_Engine.h"

#include "SouND/Sources/SNDvolume.h"

#include "Xenon/Demo/DemoLaunchData.h"

#include <xmp.h>

// ***********************************************************************************************************************
//    Externs
// ***********************************************************************************************************************

extern bool	g_bHideFPSAndVersionInfo;

extern int	TEXT_i_GetLang();

// ***********************************************************************************************************************

namespace VideoManager
{

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const char* kFilename_Path		= "D:\\Video\\";
const char* kFilename_Extension	= ".wmv";

// ***********************************************************************************************************************
//    Global Variables
// ***********************************************************************************************************************

CVideoManager* g_pVideoManager = NULL;

// ***********************************************************************************************************************
//    Class Methods : CVideoManager
// ***********************************************************************************************************************

CVideoManager::CVideoManager()
{
	m_pMediaPlayer				= NULL;
	m_pMemoryBuffer				= NULL;
	m_bIsPlaying				= FALSE;
	m_bIsPlaybackRequested		= FALSE;
	m_dwStartTime				= 0;
	m_pOverlay_Loading_Texture	= NULL;
	m_iOverlay_Loading_ID		= MAT_Xe_InvalidTextureId;
	m_pOverlay_MapName_Texture	= NULL;
	m_iOverlay_MapName_ID		= MAT_Xe_InvalidTextureId;
	m_bExclusivePlayback		= FALSE;

	m_pVideoSubtitles			= new CVideoSubtitles();
}

// ***********************************************************************************************************************

CVideoManager::~CVideoManager()
{
	delete m_pVideoSubtitles;

	DestroyOverlay();
}

// ***********************************************************************************************************************

BOOL CVideoManager::IsPlaying()
{
	return m_bIsPlaying;
}

// ***********************************************************************************************************************

BOOL CVideoManager::HasRequest()
{
	return m_bIsPlaybackRequested;
}

// ***********************************************************************************************************************

void CVideoManager::SetPlaying(BOOL Param_bIsPlaying)
{
	m_bIsPlaying = Param_bIsPlaying;
}

// ***********************************************************************************************************************

IXMediaXmvPlayer* CVideoManager::GetPlayer()
{
	return m_pMediaPlayer;
}

// ***********************************************************************************************************************

BOOL CVideoManager::Play(char* Param_pFilename, BOOL Param_bPlayFromMemory, int Param_nMinPlayTime)
{
	// Error Checking

	if ((Param_pFilename == NULL) || IsPlaying() || m_bIsPlaybackRequested)
	{
		return FALSE;
	}

	// Qualify Filename

	int iFilename_Length = strlen(Param_pFilename);

	if ((iFilename_Length == 0) || ((strlen(kFilename_Path) + iFilename_Length + strlen(kFilename_Extension)) >= kFilename_MaxLength))
	{
		return FALSE;
	}

	m_nMinPlayTime = Param_nMinPlayTime;

	sprintf(m_pFilename, "%s%s%s", kFilename_Path, Param_pFilename, kFilename_Extension);

	// Load In Memory (If Applicable)

	if (Param_bPlayFromMemory)
	{
		// Open File

		HANDLE hFile = CreateFile(m_pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == NULL)
		{
			return FALSE;
		}

		// Fetch Size

		m_uiMemorySize = GetFileSize(hFile, NULL);

		if (m_uiMemorySize == 0)
		{
			return FALSE;
		}

		// Allocate Buffer

		m_pMemoryBuffer = L_malloc(m_uiMemorySize);

		if (m_pMemoryBuffer == NULL)
		{
			return FALSE;
		}

		// Read From File

		DWORD dwSizeRead;

		BOOL bSuccess = ReadFile(hFile, m_pMemoryBuffer, m_uiMemorySize, &dwSizeRead, NULL);

		if (!bSuccess || (dwSizeRead != m_uiMemorySize))
		{
			return FALSE;
		}

		CloseHandle(hFile);
	}

	m_bIsPlaybackRequested = TRUE;

	return TRUE;
}

// ***********************************************************************************************************************

void CVideoManager::PlayExclusive(char* Param_pFilename)
{
	// Error Checking

	if (m_bExclusivePlayback)
	{
		return;
	}

	// Lock Input

	xeINO_bDesktop_InputLock = TRUE;

	// Enable Exclusive Mode

	m_bExclusivePlayback = TRUE;

	// Create Video

	CVideoNoLoop* pVideo = new CVideoNoLoop();

#ifdef _XENON_RENDERER_USETHREAD
	g_oXeRenderer.AcquireThreadOwnership();
#endif

	pVideo->Open(Param_pFilename, FALSE);
	pVideo->SetMode(CVideoNoLoop::PLAY_ALL_ONCE);
	g_oXeRenderer.SetVideo(pVideo);

#ifdef _XENON_RENDERER_USETHREAD
	g_oXeRenderer.ReleaseThreadOwnership();
#endif
	// Suspend User Background Music (If Applicable)

	m_bXMPOverride			= FALSE;
	BOOL bTitleHasControl	= TRUE;

	DWORD dwResult = XMPTitleHasPlaybackControl(&bTitleHasControl);

	XNotifyDelayUI(120 * 1000);

	if (dwResult == ERROR_SUCCESS && !bTitleHasControl)
	{
		m_bXMPOverride = TRUE;
		XMPOverrideBackgroundMusic();
	}

	// Kill Sound Volume

#if _XENON_SOUND_ENGINE
	GetAudioEngine()->SetGlobalVolume(-8000);
#endif

	// Hide FPS

	g_bHideFPSAndVersionInfo = true;
}

// ***********************************************************************************************************************

void CVideoManager::ManageExclusive()
{
	// Error Checking

	if (!m_bExclusivePlayback)
	{
		return;
	}

	if (g_oXeRenderer.m_pVideo != NULL)
	{
		if (g_oXeRenderer.m_pVideo->IsDone())
		{
			StopExclusive();
			return;
		}

		xeINO_bDesktop_InputLock = FALSE;

		if (INO_b_Joystick_IsButtonJustDown(eXeButton_A) || INO_b_Joystick_IsButtonJustDown(eXeButton_Start))
		{
			StopExclusive();
			return;
		}

		xeINO_bDesktop_InputLock = TRUE;
	}
}

// ***********************************************************************************************************************

void CVideoManager::StopExclusive()
{
	// Error Checking

	if (!m_bExclusivePlayback)
	{
		return;
	}

	// Destroy Video

	if (g_oXeRenderer.m_pVideo != NULL)
	{
		delete g_oXeRenderer.m_pVideo;
		g_oXeRenderer.m_pVideo = NULL;
	}

	// Disable Exclusive Mode

	m_bExclusivePlayback = FALSE;

	// Restore User Background Music (If Applicable)

	XNotifyDelayUI(0);

	if (m_bXMPOverride)
	{
		XMPRestoreBackgroundMusic();
	}

	// Restore Sound Volume

#if _XENON_SOUND_ENGINE
	GetAudioEngine()->SetGlobalVolume(0);
#endif

	// Show FPS

	g_bHideFPSAndVersionInfo = false;

	// Unlock Input

	xeINO_bDesktop_InputLock = FALSE;
}

// ***********************************************************************************************************************

void CVideoManager::Stop()
{
	// Stop Playback (If Applicable)

	if ((m_pMediaPlayer != NULL) && IsPlaying())
	{
		m_pMediaPlayer->Stop(XMEDIA_STOP_IMMEDIATE);

		SetPlaying(FALSE);
	}

	// Stop Subtitles

	//m_pVideoSubtitles->Close();

	// Free Memory Buffer (If Applicable)

	if (m_pMemoryBuffer != NULL)
	{
		L_free(m_pMemoryBuffer);
		m_pMemoryBuffer = NULL;
	}
}

// ***********************************************************************************************************************

BOOL CVideoManager::PerformRequests()
{
	// Special Exclusive Mode

	ManageExclusive();

	// Error Checking

	if (IsPlaying() || !m_bIsPlaybackRequested)
	{
		return FALSE;
	}

	// Prepare Subtitles

	//m_pVideoSubtitles->Open(m_pFilename);

	// Reset Inactivity Timer To Prevent Successive Attract Modes

	xeINO_ResetInactivityTimer( TimerAttractMode );

	// Suspend User Background Music (If Applicable)

	BOOL bXMPOverride		= FALSE;
	BOOL bTitleHasControl	= TRUE;

	DWORD dwResult = XMPTitleHasPlaybackControl(&bTitleHasControl);

	XNotifyDelayUI(120 * 1000);

	if (dwResult == ERROR_SUCCESS && !bTitleHasControl)
	{
		bXMPOverride = TRUE;
		XMPOverrideBackgroundMusic();
	}

	// Create Player

	HRESULT hResult;

	if (m_pMemoryBuffer != NULL)
	{
		// From Memory

		hResult = XMediaCreateXmvPlayerFromMemory(g_oXeRenderer.GetDevice(), m_pMemoryBuffer, m_uiMemorySize, &m_pMediaPlayer);
	}
	else
	{
		// From File

		hResult = XMediaCreateXmvPlayerFromFile(g_oXeRenderer.GetDevice(), m_pFilename, &m_pMediaPlayer);
	}

	if (hResult != S_OK)
	{
		m_bIsPlaybackRequested = FALSE;
		return FALSE;
	}

	// Set Callback

	m_pMediaPlayer->SetCallback(XMEDIA_NOTIFY_BEGIN_SCENE , VideoManager_Callback_BeginFrame, this);
	m_pMediaPlayer->SetCallback(XMEDIA_NOTIFY_END_OF_FRAME, VideoManager_Callback_EndFrame  , this);

	// Set Volume

	IXAudioSourceVoice* pSourceVoice = NULL;

	hResult = m_pMediaPlayer->GetSourceVoice(&pSourceVoice);

	if ((pSourceVoice != NULL) && (hResult == S_OK))
	{
		pSourceVoice->SetVolume(SND_gdst_Group[SND_e_UserGrpMaster].f_Volume * SND_gdst_Group[SND_e_MasterGrp].f_Volume);
	}

    // Set GPR Allocation In Favor Of Pixel Shading

    g_oXeRenderer.GetDevice()->SetShaderGPRAllocation(0, 0, 0);

	// Reset Render Manager States (Pre Video)

    g_oRenderStateMgr.SetDefaultStates();
	g_oRenderStateMgr.Update();

	// Backup presentation interval

	DWORD dwOldValue;

	g_oXeRenderer.GetDevice()->GetRenderState( D3DRS_PRESENTINTERVAL, &dwOldValue );

	// Set new presentation interval

	g_oXeRenderer.GetDevice()->SetRenderState( D3DRS_PRESENTINTERVAL, D3DPRESENT_INTERVAL_ONE );

	// Clear Screen

	g_oXeRenderer.ClearTarget(0);

	// Suspend Sound Streams

#if _XENON_SOUND_ENGINE
	GetAudioEngine()->Streams(eStreamOperation_Pause);
#endif

	m_dwStartTime = GetTickCount( );

	bool bBackupTimer = false;
	if( CXeDemoLaunchData::GetDemoMode( ) == DEMO_MODE_USER_SELECTED )
		bBackupTimer = true;

	float fTimerBackup = 0.0f;
	if( bBackupTimer )
		fTimerBackup = xeINO_GetInactivitySeconds( TimerExitDemo );

	// Launch Playback (Blocking)

	hResult = m_pMediaPlayer->Play(0, NULL);

	if( bBackupTimer )
		xeINO_SetInactivitySeconds( TimerExitDemo, fTimerBackup );

	// Restore User Background Music (If Applicable)

	XNotifyDelayUI(0);

	if (bXMPOverride)
	{
		XMPRestoreBackgroundMusic();
	}

	// Resume Sound Streams

#if _XENON_SOUND_ENGINE
	GetAudioEngine()->Streams(eStreamOperation_Resume);
#endif

	// Reset Inactivity Timer To Prevent Successive Attract Modes

	xeINO_ResetInactivityTimer( TimerAttractMode );

	// Restore presentation interval

	g_oXeRenderer.GetDevice()->SetRenderState( D3DRS_PRESENTINTERVAL, dwOldValue );

	// Playback Complete

	Stop();

	// Destroy Media Player

	m_pMediaPlayer->Release();
	m_pMediaPlayer = NULL;

	// Reset Render Manager States (Post Video)

	g_oRenderStateMgr.ReInit();

	m_bIsPlaybackRequested = FALSE;
	return (hResult == S_OK);
}

// ***********************************************************************************************************************

void CVideoManager::PrepareOverlay()
{
	char* pLangSuffix;
	char* pMapID;
	BOOL  bShowLoading = TRUE;

	if (m_bExclusivePlayback)
	{
		return;
	}

	// Determine Language Suffix

	switch(TEXT_i_GetLang())
	{
		case INO_e_French		: pLangSuffix = "Fre"; break;
		default:
		case INO_e_English		: pLangSuffix = "Eng"; break;
		case INO_e_Danish		: pLangSuffix = "Dan"; break;
		case INO_e_Dutch		: pLangSuffix = "Dut"; break;
		case INO_e_Finnish		: pLangSuffix = "Fin"; break;
		case INO_e_German		: pLangSuffix = "Ger"; break;
		case INO_e_Italian		: pLangSuffix = "Ita"; break;
		case INO_e_Spanish		: pLangSuffix = "Spa"; break;
		case INO_e_Portuguese	: pLangSuffix = "Por"; break;
		case INO_e_Swedish		: pLangSuffix = "Swe"; break;
		case INO_e_Polish		: pLangSuffix = "Pol"; break;
		case INO_e_Russian		: pLangSuffix = "Rus"; break;
		case INO_e_Japanese		: pLangSuffix = "Jap"; break;
		case INO_e_Chinese		: pLangSuffix = "Chi"; break;
		case INO_e_Albanian		: pLangSuffix = "Alb"; break;
		case INO_e_Arabic		: pLangSuffix = "Ara"; break;
		case INO_e_Bulgarian	: pLangSuffix = "Bul"; break;
		case INO_e_Byelorussian	: pLangSuffix = "Bye"; break;
		case INO_e_Greek		: pLangSuffix = "Gre"; break;
		case INO_e_Korean		: pLangSuffix = "Kor"; break;
		case INO_e_Norwegian	: pLangSuffix = "Nor"; break;
		case INO_e_Romanian		: pLangSuffix = "Rom"; break;
		case INO_e_Serbian		: pLangSuffix = "Ser"; break;
		case INO_e_Slovak		: pLangSuffix = "Slk"; break;
		case INO_e_Slovenian	: pLangSuffix = "Sln"; break;
		case INO_e_Turkish		: pLangSuffix = "Tur"; break;
		case INO_e_Czech		: pLangSuffix = "Cze"; break;
		case INO_e_Hungarian	: pLangSuffix = "Hun"; break;
		case INO_e_Taiwanese	: pLangSuffix = "Tai"; break;
	}

	// Determine Map Identifier

	switch(g_oXeSimpleRenderer.GetLoadingWorldID())
	{
		default   : pMapID = NULL  ; break;
		case  100 : pMapID = NULL  ; bShowLoading = FALSE; break;
		case  101 : pMapID = "01B" ; break;
		case  201 : pMapID = "02A" ; break;
		case  203 : pMapID = "02C1"; break;
		case  204 : pMapID = "02C2"; break;
		case  301 : pMapID = "03A" ; break;
		case  302 : pMapID = "03B" ; break;
		case  303 : pMapID = "03C" ; break;
		case  305 : pMapID = "03E" ; break;
		case  306 : pMapID = "03F" ; break;
		case  304 : pMapID = "03D" ; break;
		case  401 : pMapID = "04A" ; break;
		case  402 : pMapID = "04A1"; break;
		case  403 : pMapID = "04A2"; break;
		case  404 : pMapID = "04B" ; break;
		case  501 : pMapID = "05A1"; break;
		case  502 : pMapID = "05A2"; break;
		case  504 : pMapID = "05C" ; break;
		case  702 : pMapID = "07B" ; break;
		case  705 : pMapID = "07D" ; break;
		case  901 : pMapID = "09B" ; break;
		case 1002 : pMapID = "10B" ; break;
		case 1101 : pMapID = "11A" ; break;
		case 1200 : pMapID = "12A" ; break;
		case 1201 : pMapID = "12A2"; break;
		case  703 : pMapID = "07B2"; break;
		case 1401 : pMapID = "14A" ; break;
		case 1402 : pMapID = "14B" ; break;
		case 1501 : pMapID = "15A" ; break;
		case 1701 : pMapID = "17A" ; break;
		case 1702 : pMapID = "17A2"; break;
		case 1703 : pMapID = "17B" ; break;
		case 1801 : pMapID = "18A" ; break;
		case 1803 : pMapID = "03CR"; break;
		case 1804 : pMapID = "03BR"; break;
		case 1805 : pMapID = "03AR"; break;
		case 1901 : pMapID = "19A" ; break;
		case 1902 : pMapID = "19B" ; break;
		case 2001 : pMapID = "20A" ; break;
		case 2003 : pMapID = "20C" ; break;
		case 2004 : pMapID = "20D" ; break;
	}

	// Prepare Filenames

	char acFileName_Loading[128];
	char acFileName_MapName[128];

	if (bShowLoading)
	{
		sprintf(acFileName_Loading, "GAME:\\Loading\\Loading_%s.raw", pLangSuffix);
	}
	else
	{
		sprintf(acFileName_Loading, "GAME:\\Loading\\00_Empty.raw");
	}

	if (pMapID != NULL)
	{
		sprintf(acFileName_MapName, "GAME:\\Loading\\%s_%s.raw", pMapID, pLangSuffix);
	}
	else
	{
		sprintf(acFileName_MapName, "GAME:\\Loading\\00_Empty.raw");
	}

	// Create File Handles

	HANDLE hFile_Loading = CreateFile(acFileName_Loading, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile_Loading == INVALID_HANDLE_VALUE)
	{
		return;
	}

	HANDLE hFile_MapName = CreateFile(acFileName_MapName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile_MapName == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile_Loading);
		return;
	}

	// Create Buffers

	int iSize_Loading = kiVideoOverlay_Loading_Format_Width * kiVideoOverlay_Loading_Format_Height * sizeof(int);
	int iSize_MapName = kiVideoOverlay_MapName_Format_Width * kiVideoOverlay_MapName_Format_Height * sizeof(int);

	void* pBuffer_Loading = L_malloc(iSize_Loading);

	if (pBuffer_Loading == NULL)
	{
		CloseHandle(hFile_Loading);
		CloseHandle(hFile_MapName);
		return;
	}

	void* pBuffer_MapName = L_malloc(iSize_MapName);

	if (pBuffer_MapName == NULL)
	{
		L_free(pBuffer_Loading);
		CloseHandle(hFile_Loading);
		CloseHandle(hFile_MapName);
		return;
	}

	// Load File Data

	int iBytesRead = 0;

	if (!ReadFile(hFile_Loading, pBuffer_Loading, iSize_Loading, (LPDWORD)&iBytesRead, NULL))
	{
		CloseHandle(hFile_Loading);
		CloseHandle(hFile_MapName);
		L_free(pBuffer_Loading);
		L_free(pBuffer_MapName);
		return;
	}

	if (!ReadFile(hFile_MapName, pBuffer_MapName, iSize_MapName, (LPDWORD)&iBytesRead, NULL))
	{
		CloseHandle(hFile_Loading);
		CloseHandle(hFile_MapName);
		L_free(pBuffer_Loading);
		L_free(pBuffer_MapName);
		return;
	}

	CloseHandle(hFile_Loading);
	CloseHandle(hFile_MapName);

	// Create Textures

	HRESULT hResult_Loading = g_oXeRenderer.GetDevice()->CreateTexture(kiVideoOverlay_Loading_Format_Width, kiVideoOverlay_Loading_Format_Height, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED, &m_pOverlay_Loading_Texture, NULL);
	HRESULT hResult_MapName = g_oXeRenderer.GetDevice()->CreateTexture(kiVideoOverlay_MapName_Format_Width, kiVideoOverlay_MapName_Format_Height, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED, &m_pOverlay_MapName_Texture, NULL);

	if (FAILED(hResult_Loading) || FAILED(hResult_MapName) || (m_pOverlay_Loading_Texture == NULL) || (m_pOverlay_MapName_Texture == NULL))
	{
		L_free(pBuffer_Loading);
		L_free(pBuffer_MapName);
		return;
	}

	// Lock Surfaces

	D3DLOCKED_RECT stRect_Loading;
	D3DLOCKED_RECT stRect_MapName;

	hResult_Loading = m_pOverlay_Loading_Texture->LockRect(0, &stRect_Loading, NULL, 0);
	hResult_MapName = m_pOverlay_MapName_Texture->LockRect(0, &stRect_MapName, NULL, 0);

	if (FAILED(hResult_Loading) || FAILED(hResult_MapName))
	{
		L_free(pBuffer_Loading);
		L_free(pBuffer_MapName);
		return;
	}

	// Fill Pixel Surface - Loading

	int* pImage_Loading = (int*)pBuffer_Loading;

	for (int iLoopY = 0; iLoopY < kiVideoOverlay_Loading_Format_Height; iLoopY++)
	{
		int* pTextureLine = (int*)stRect_Loading.pBits + ((iLoopY * stRect_Loading.Pitch) >> 2);

		for (int iLoopX = 0; iLoopX < kiVideoOverlay_Loading_Format_Width; iLoopX++)
		{
			unsigned int uiColor = *(pImage_Loading++);
			SwapDWord(&uiColor);
			*(pTextureLine++) = XeConvertColor(uiColor);
		}
	}

	// Fill Pixel Surface - Map Name

	int* pImage_MapName = (int*)pBuffer_MapName;

	for (int iLoopY = 0; iLoopY < kiVideoOverlay_MapName_Format_Height; iLoopY++)
	{
		int* pTextureLine = (int*)stRect_MapName.pBits + ((iLoopY * stRect_MapName.Pitch) >> 2);

		for (int iLoopX = 0; iLoopX < kiVideoOverlay_MapName_Format_Width; iLoopX++)
		{
			unsigned int uiColor = *(pImage_MapName++);
			SwapDWord(&uiColor);
			*(pTextureLine++) = XeConvertColor(uiColor);
		}
	}

	// Release Buffers

	L_free(pBuffer_Loading);
	L_free(pBuffer_MapName);

	// Unlock Surfaces

	m_pOverlay_Loading_Texture->UnlockRect(0);
	m_pOverlay_MapName_Texture->UnlockRect(0);

	// Add / Update User Textures In Manager

	if (m_iOverlay_Loading_ID == (UINT)MAT_Xe_InvalidTextureId)
	{
		m_iOverlay_Loading_ID = g_oXeTextureMgr.RegisterUserTexture(m_pOverlay_Loading_Texture);
	}
	else
	{
		g_oXeTextureMgr.UpdateUserTexture(m_iOverlay_Loading_ID, m_pOverlay_Loading_Texture);
	}

	if (m_iOverlay_MapName_ID == (UINT)MAT_Xe_InvalidTextureId)
	{
		m_iOverlay_MapName_ID = g_oXeTextureMgr.RegisterUserTexture(m_pOverlay_MapName_Texture);
	}
	else
	{
		g_oXeTextureMgr.UpdateUserTexture(m_iOverlay_MapName_ID, m_pOverlay_MapName_Texture);
	}
}

// ***********************************************************************************************************************

void CVideoManager::DestroyOverlay()
{
	SAFE_RELEASE(m_pOverlay_MapName_Texture);
	//popo SAFE_RELEASE(m_pOverlay_Loading_Texture);

	g_oXeTextureMgr.UpdateUserTexture(m_iOverlay_Loading_ID, NULL);
	g_oXeTextureMgr.UpdateUserTexture(m_iOverlay_MapName_ID, NULL);
}

// ***********************************************************************************************************************

void CVideoManager::DisplayOverlay()
{
	if (m_bExclusivePlayback)
	{
		return;
	}

	if (m_iOverlay_MapName_ID != MAT_Xe_InvalidTextureId)
	{
		DrawRectangleEx(kfVideoOverlay_MapName_Screen_OffsetX,
						kfVideoOverlay_MapName_Screen_OffsetY,
						kfVideoOverlay_MapName_Screen_Width  + kfVideoOverlay_MapName_Screen_OffsetX,
						kfVideoOverlay_MapName_Screen_Height + kfVideoOverlay_MapName_Screen_OffsetY,
						0.0f, 0.0f, 1.0f, 1.0f,
						0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
						0.0f, MAT_Cc_Op_Alpha, m_iOverlay_MapName_ID);
	}

	if (m_iOverlay_Loading_ID != MAT_Xe_InvalidTextureId)
	{
		DrawRectangleEx((1.0f - kfVideoOverlay_Loading_Screen_Width ) - kfVideoOverlay_Loading_Screen_OffsetX,
						(1.0f - kfVideoOverlay_Loading_Screen_Height) - kfVideoOverlay_Loading_Screen_OffsetY,
						1.0f - kfVideoOverlay_Loading_Screen_OffsetX,
						1.0f - kfVideoOverlay_Loading_Screen_OffsetY,
						0.0f, 0.0f, 1.0f, 1.0f,
						0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
						0.0f, MAT_Cc_Op_Alpha, m_iOverlay_Loading_ID);
	}
}

// ***********************************************************************************************************************

CVideoSubtitles* CVideoManager::GetSubtitles()
{
	return m_pVideoSubtitles;
}

// ***********************************************************************************************************************
//    Functions
// ***********************************************************************************************************************

void CALLBACK CVideoManager::VideoManager_Callback_BeginFrame(void* Param_pVideoManager)
{
	g_oXeRenderer.Clear( 0, 1.0f, 0 );
}

// ***********************************************************************************************************************

void CALLBACK CVideoManager::VideoManager_Callback_EndFrame(void* Param_pVideoManager)
{
	// Fetch Manager Class

    CVideoManager* pVideoManager = (CVideoManager*)Param_pVideoManager;

	// Error Checking

	if ((pVideoManager == NULL) || (pVideoManager->GetPlayer() == NULL))
	{
		return;
	}

	// Flag Status as Playing

	pVideoManager->SetPlaying(TRUE);

	// Display Subtitles

	//pVideoManager->GetSubtitles()->Draw();

	// Update Engine Trackers

	TIM_Clock_Update();
	INO_Joystick_Update();

	// Clear Inactivity Timer

	xeINO_ResetInactivityTimer( TimerAttractMode );

	// Check For Skipping

	BOOL bInputLockBackup = xeINO_bDesktop_InputLock;

	xeINO_bDesktop_InputLock = FALSE;

	if( ( GetTickCount( ) - pVideoManager->m_dwStartTime ) > pVideoManager->m_nMinPlayTime
#ifndef _FINAL_
		|| INO_b_Joystick_IsButtonDown(eXeButton_Trigger_Left)
#endif
		)
	{
		if (INO_b_Joystick_IsButtonJustUp(eXeButton_A) || INO_b_Joystick_IsButtonJustUp(eXeButton_Start))
		{
			pVideoManager->Stop();
		}
	}

	xeINO_bDesktop_InputLock = bInputLockBackup;
}

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************

#endif // _XENON

// ****************************************
// Audio Console Debugging Desktop
//
// Used for King Kong Xenon Sound Debugging
//
// By Alexandre David (January 2005)
// ****************************************

#include "Precomp.h"

#include "xeSND_AudioConsole.h"

#if defined(AUDIOCONSOLE_ENABLE)

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "SouND/Sources/SNDvolume.h"

// ***********************************************************************************************************************
//    Private Variables
// ***********************************************************************************************************************

CAudioConsole g_AudioConsole;

// ***********************************************************************************************************************
//    Prototypes
// ***********************************************************************************************************************

void	AudioConsole_Initialize()						{g_AudioConsole.Initialize();}
void	AudioConsole_Uninit()							{g_AudioConsole.Uninit();}
void	AudioConsole_Tick(TFloat Param_fMSecTimeDelta)	{g_AudioConsole.Tick(Param_fMSecTimeDelta);}
void	AudioConsole_Render()							{g_AudioConsole.Render();}

void	AudioConsole_AddEvent  (TPWString Param_pString, TPWString Param_pModuleName, stStringList_EntryType Param_stEntryType)	{g_AudioConsole.AddEvent(Param_pString, Param_pModuleName, Param_stEntryType);}
void	AudioConsole_AddMessage(TPWString Param_pString, TPWString Param_pModuleName, stStringList_EntryType Param_stEntryType)	{g_AudioConsole.AddMessage(Param_pString, Param_pModuleName, Param_stEntryType);}
void	AudioConsole_AddEventParameter(TPWString Param_pValue, TPWString Param_pName)								{g_AudioConsole.AddParameter(Param_pValue, Param_pName, TRUE);}
void	AudioConsole_AddEventParameter(TInt      Param_iValue, TPWString Param_pName, TBool Param_bDisplayAsHex)	{g_AudioConsole.AddParameter(Param_iValue, Param_pName, TRUE, Param_bDisplayAsHex);}
void	AudioConsole_AddEventParameter(TFloat    Param_fValue, TPWString Param_pName)								{g_AudioConsole.AddParameter(Param_fValue, Param_pName, TRUE);}
void	AudioConsole_AddEventParameter(TBool     Param_bValue, TPWString Param_pName)								{g_AudioConsole.AddParameter(Param_bValue, Param_pName, TRUE);}
void	AudioConsole_AddMessageParameter(TPWString Param_pValue, TPWString Param_pName)								{g_AudioConsole.AddParameter(Param_pValue, Param_pName, FALSE);}
void	AudioConsole_AddMessageParameter(TInt      Param_iValue, TPWString Param_pName, TBool Param_bDisplayAsHex)	{g_AudioConsole.AddParameter(Param_iValue, Param_pName, FALSE, Param_bDisplayAsHex);}
void	AudioConsole_AddMessageParameter(TFloat    Param_fValue, TPWString Param_pName)								{g_AudioConsole.AddParameter(Param_fValue, Param_pName, FALSE);}
void	AudioConsole_AddMessageParameter(TBool     Param_bValue, TPWString Param_pName)								{g_AudioConsole.AddParameter(Param_bValue, Param_pName, FALSE);}
void	AudioConsole_WaitForMessageExclusion()	{g_AudioConsole.WaitForMessageExclusion();}

TBool	AudioConsole_GetConfiguration(eConfiguration_Options Param_eOption)	{return g_AudioConsole.GetConfiguration(Param_eOption);}

TListID	AudioConsole_AddSetting(TPWString Param_pName, void*   Param_pLinkedVariable, stTrackerType Param_stType, fnOptionNotify Param_fnOptionNotify)													{return g_AudioConsole.AddSetting(Param_pName, Param_pLinkedVariable, Param_stType, Param_fnOptionNotify);}
TListID	AudioConsole_AddSetting(TPWString Param_pName, TBool*  Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify)																		{return g_AudioConsole.AddSetting(Param_pName, Param_pLinkedVariable, Param_fnOptionNotify);}
TListID	AudioConsole_AddSetting(TPWString Param_pName, TInt*   Param_pLinkedVariable, TInt Param_iMinimum, TInt Param_iMaximum, TInt Param_iDelta, fnOptionNotify_Int Param_fnOptionNotify)			{return g_AudioConsole.AddSetting(Param_pName, Param_pLinkedVariable, Param_iMinimum, Param_iMaximum, Param_iDelta, Param_fnOptionNotify);}
TListID	AudioConsole_AddSetting(TPWString Param_pName, TFloat* Param_pLinkedVariable, TFloat Param_fMinimum, TFloat Param_fMaximum, TFloat Param_fDelta, fnOptionNotify_Float Param_fnOptionNotify)	{return g_AudioConsole.AddSetting(Param_pName, Param_pLinkedVariable, Param_fMinimum, Param_fMaximum, Param_fDelta, Param_fnOptionNotify);}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Audio_Panel_Buffer
// ***********************************************************************************************************************

CWidget_Audio_Panel_Buffer::CWidget_Audio_Panel_Buffer(TInt Param_iBufferID)
{
	TFloat fPosition_X = 0.005f;

	// Initialize Fields

	m_iBufferID			= Param_iBufferID;
	m_bIsUpdateForced	= TRUE;

	// Create SubWidget - Type

	CWidget_Base_FontGlyph* pWidget_Type = new CWidget_Base_FontGlyph();

	pWidget_Type->Get_Position()->Set(fPosition_X, kfBuffers_Icon_Position_Y);
	pWidget_Type->Get_Size()->Set(0.2f, 1.0f);

	m_WID_Type = AddSubWidget(pWidget_Type);

	fPosition_X += 0.050f;

	// Create SubWidget - Format

	CWidget_Base_FontGlyph* pWidget_Format = new CWidget_Base_FontGlyph();

	pWidget_Format->Get_Position()->Set(fPosition_X, kfBuffers_Icon_Position_Y);
	pWidget_Format->Get_Size()->Set(0.2f, 1.0f);

	m_WID_Format = AddSubWidget(pWidget_Format);

	fPosition_X += 0.102f;

	// Create SubWidget - Channels

	CWidget_Base_FontGlyph* pWidget_Channels = new CWidget_Base_FontGlyph();

	pWidget_Channels->Get_Position()->Set(fPosition_X, kfBuffers_Icon_Position_Y);
	pWidget_Channels->Get_Size()->Set(0.2f, 1.0f);

	m_WID_Channels = AddSubWidget(pWidget_Channels);

	fPosition_X += 0.102f;

	// Create SubWidget - Fx

	CWidget_Base_FontGlyph* pWidget_Fx = new CWidget_Base_FontGlyph();

	pWidget_Fx->Get_Position()->Set(fPosition_X, kfBuffers_Icon_Position_Y);
	pWidget_Fx->Get_Size()->Set(0.2f, 1.0f);

	m_WID_Fx = AddSubWidget(pWidget_Fx);

	fPosition_X += 0.102f;

	// Create SubWidget - State

	CWidget_Base_FontGlyph* pWidget_State = new CWidget_Base_FontGlyph();

	pWidget_State->Get_Position()->Set(fPosition_X, kfBuffers_Icon_Position_Y);
	pWidget_State->Get_Size()->Set(0.2f, 1.0f);

	m_WID_State = AddSubWidget(pWidget_State);

	fPosition_X += 0.050f;

	// Create SubWidget - Loop

	CWidget_Base_FontGlyph* pWidget_Loop = new CWidget_Base_FontGlyph();

	pWidget_Loop->Get_Position()->Set(fPosition_X, kfBuffers_Icon_Position_Y);
	pWidget_Loop->Get_Size()->Set(0.2f, 1.0f);

	m_WID_Loop = AddSubWidget(pWidget_Loop);

	fPosition_X += 0.050f;

	// Create SubWidget - Frequency

	fPosition_X += 0.010f;

	CWidget_Base_Text* pWidget_Frequency = new CWidget_Base_Text();

	pWidget_Frequency->Get_Position()->Set(fPosition_X, kfBuffers_Icon_Position_Y);
	pWidget_Frequency->Get_Size()->Set(0.2f, 1.0f);

	m_WID_Frequency = AddSubWidget(pWidget_Frequency);

	fPosition_X += 0.150f;

	// Create SubWidget - Volume

	CWidget_Base_ValueTracker* pWidget_Volume = new CWidget_Base_ValueTracker(kstTrackerType_Bar_Dotted);

	pWidget_Volume->Get_Position()->Set(fPosition_X, kfBuffers_Icon_Position_Y);
	pWidget_Volume->Get_Size()->Set(0.075f, 1.0f);

	m_WID_Volume = AddSubWidget(pWidget_Volume);

	// Create SubWidget - Size / Duration - Playback

	CWidget_Base_Text* pWidget_SizeDur_Playback = new CWidget_Base_Text();

	pWidget_SizeDur_Playback->Get_Position()->Set(kfBufferBar_Playback_Position_X, 0.50f);
	pWidget_SizeDur_Playback->Get_Size()->Set(0.175f, 0.87f);

	m_WID_SizeDur_Playback = AddSubWidget(pWidget_SizeDur_Playback);

	// Create SubWidget - Size / Duration - File

	CWidget_Base_Text* pWidget_SizeDur_File = new CWidget_Base_Text();

	pWidget_SizeDur_File->Get_Position()->Set(kfBufferBar_File_Position_X, 0.50f);
	pWidget_SizeDur_File->Get_Size()->Set(0.175f, 0.875f);

	m_WID_SizeDur_File = AddSubWidget(pWidget_SizeDur_File);

	// Create SubWidget - Playback Buffer Bar

	CWidget_Base_ColorQuad* pWidget_BufferBar_Playback = new CWidget_Base_ColorQuad(0x80804000);

	pWidget_BufferBar_Playback->Get_Position()->Set(kfBufferBar_Playback_Position_X, kfBufferBar_Position_Y);
	pWidget_BufferBar_Playback->Get_Size()->Set(kfBufferBar_Size_X, kfBufferBar_Size_Y);

	m_WID_BufferBar_Playback = AddSubWidget(pWidget_BufferBar_Playback);

	// Create SubWidget - File Buffer Bar

	CWidget_Base_ColorQuad* pWidget_BufferBar_File = new CWidget_Base_ColorQuad(0x80808000);

	pWidget_BufferBar_File->Get_Position()->Set(kfBufferBar_File_Position_X, kfBufferBar_Position_Y);
	pWidget_BufferBar_File->Get_Size()->Set(kfBufferBar_Size_X, kfBufferBar_Size_Y);

	m_WID_BufferBar_File = AddSubWidget(pWidget_BufferBar_File);

	// Create SubWidget - Position Marker - Load File

	CWidget_Base_FontGlyph* pWidget_PositionMarker_LoadFile = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_PositionMarker_Load);

	pWidget_PositionMarker_LoadFile->Get_Position()->Get_Y()->Set(kfBufferBar_Position_Y - kfBufferBar_Marker_Offset_Y);
	pWidget_PositionMarker_LoadFile->Get_Size()->Set(kfBufferBar_Marker_Size_X, kfBufferBar_Marker_Size_Y);

	m_WID_PositionMarker_LoadFile = AddSubWidget(pWidget_PositionMarker_LoadFile);

	// Create SubWidget - Position Marker - Load Playback

	CWidget_Base_FontGlyph* pWidget_PositionMarker_LoadPlayback = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_PositionMarker_Load);

	pWidget_PositionMarker_LoadPlayback->Get_Position()->Get_Y()->Set(kfBufferBar_Position_Y - kfBufferBar_Marker_Offset_Y);
	pWidget_PositionMarker_LoadPlayback->Get_Size()->Set(kfBufferBar_Marker_Size_X, kfBufferBar_Marker_Size_Y);

	m_WID_PositionMarker_LoadPlayback = AddSubWidget(pWidget_PositionMarker_LoadPlayback);

	// Create SubWidget - Position Marker - Loop Start

	CWidget_Base_FontGlyph* pWidget_PositionMarker_LoopStart = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_PositionMarker_LoopStart);

	pWidget_PositionMarker_LoopStart->Get_Position()->Get_Y()->Set(kfBufferBar_Position_Y - kfBufferBar_Marker_Offset_Y);
	pWidget_PositionMarker_LoopStart->Get_Size()->Set(kfBufferBar_Marker_Size_X, kfBufferBar_Marker_Size_Y);

	m_WID_PositionMarker_LoopStart = AddSubWidget(pWidget_PositionMarker_LoopStart);

	// Create SubWidget - Position Marker - Loop End

	CWidget_Base_FontGlyph* pWidget_PositionMarker_LoopEnd = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_PositionMarker_LoopEnd);

	pWidget_PositionMarker_LoopEnd->Get_Position()->Get_Y()->Set(kfBufferBar_Position_Y - kfBufferBar_Marker_Offset_Y);
	pWidget_PositionMarker_LoopEnd->Get_Size()->Set(kfBufferBar_Marker_Size_X, kfBufferBar_Marker_Size_Y);

	m_WID_PositionMarker_LoopEnd = AddSubWidget(pWidget_PositionMarker_LoopEnd);

	// Create SubWidget - Position Marker - Play

	CWidget_Base_FontGlyph* pWidget_PositionMarker_Play = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_PositionMarker_Play);

	pWidget_PositionMarker_Play->Get_Position()->Get_Y()->Set(kfBufferBar_Position_Y - kfBufferBar_Marker_Offset_Y);
	pWidget_PositionMarker_Play->Get_Size()->Set(kfBufferBar_Marker_Size_X, kfBufferBar_Marker_Size_Y);

	m_WID_PositionMarker_Play = AddSubWidget(pWidget_PositionMarker_Play);

	// Create SubWidget - Position Marker - Decode

	CWidget_Base_FontGlyph* pWidget_PositionMarker_Decode = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_PositionMarker_Decode);

	pWidget_PositionMarker_Decode->Get_Position()->Get_Y()->Set(kfBufferBar_Position_Y - kfBufferBar_Marker_Offset_Y);
	pWidget_PositionMarker_Decode->Get_Size()->Set(kfBufferBar_Marker_Size_X, kfBufferBar_Marker_Size_Y);

	m_WID_PositionMarker_Decode = AddSubWidget(pWidget_PositionMarker_Decode);

	// Create SubWidget - Position Marker - Chain

	CWidget_Base_FontGlyph* pWidget_PositionMarker_Chain = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_PositionMarker_Chain);

	pWidget_PositionMarker_Chain->Get_Position()->Get_Y()->Set(kfBufferBar_Position_Y - kfBufferBar_Marker_Offset_Y);
	pWidget_PositionMarker_Chain->Get_Size()->Set(kfBufferBar_Marker_Size_X, kfBufferBar_Marker_Size_Y);

	m_WID_PositionMarker_Chain = AddSubWidget(pWidget_PositionMarker_Chain);

	// Create SubWidget - Position Marker - Cursor File

	CWidget_Base_FontGlyph* pWidget_PositionMarker_CursorFile = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_PositionMarker_Cursor);

	pWidget_PositionMarker_CursorFile->Get_Position()->Get_Y()->Set(kfBufferBar_Position_Y - kfBufferBar_Marker_Offset_Y);
	pWidget_PositionMarker_CursorFile->Get_Size()->Set(kfBufferBar_Marker_Size_X, kfBufferBar_Marker_Size_Y);

	m_WID_PositionMarker_CursorFile = AddSubWidget(pWidget_PositionMarker_CursorFile);

	// Create SubWidget - Position Marker - Cursor Playback

	CWidget_Base_FontGlyph* pWidget_PositionMarker_CursorPlayback = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_PositionMarker_Cursor);

	pWidget_PositionMarker_CursorPlayback->Get_Position()->Get_Y()->Set(kfBufferBar_Position_Y - kfBufferBar_Marker_Offset_Y);
	pWidget_PositionMarker_CursorPlayback->Get_Size()->Set(kfBufferBar_Marker_Size_X, kfBufferBar_Marker_Size_Y);

	m_WID_PositionMarker_CursorPlayback = AddSubWidget(pWidget_PositionMarker_CursorPlayback);

	// Create SubWidget - Pan Base

	CWidget_Base_FontGlyph* pWidget_PanBase = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_Pan_Base);

	pWidget_PanBase->Get_Position()->Set(kfBuffers_PanBase_Position_X, kfBuffers_Icon_Position_Y);
	pWidget_PanBase->Get_Size()->Set(0.2f, 1.0f);

	m_WID_Pan_Base = AddSubWidget(pWidget_PanBase);

	// Create SubWidget - Pan Arrow

	CWidget_Base_FontGlyph* pWidget_PanArrow = new CWidget_Base_FontGlyph(kcFontGlyph_Audio_Pan_Arrow);

	pWidget_PanArrow->Get_Size()->Set(0.2f, 1.0f);

	m_WID_Pan_Arrow = AddSubWidget(pWidget_PanArrow);
}

// ***********************************************************************************************************************

CWidget_Audio_Panel_Buffer::~CWidget_Audio_Panel_Buffer()
{
}

// ***********************************************************************************************************************

void CWidget_Audio_Panel_Buffer::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Reject Further Processing If Not Visible (Saves CPU Cycles)

	if (!IsVisible() && !m_bIsUpdateForced)
	{
		return;
	}

	// Ensure Buffer Is Active

	TWChar acTextBuffer[64];

	if (m_iBufferID >= AudioBuffers_GetCount())
	{
		swprintf(acTextBuffer, L"Unused Duplicate", m_iBufferID);
		SetTitle(acTextBuffer);
		return;
	}

	// Fetch Associated Voice

	CVoice* pVoice = GetAudioEngine()->GetVoice(m_iBufferID);

	if (pVoice == NULL)
	{
		return;
	}

	m_bIsUpdateForced = FALSE;

	// Set Title To ID Number

	swprintf(acTextBuffer, L"Voice %i (Key %08x)", m_iBufferID, pVoice->GetKey());

	SetTitle(acTextBuffer);

	// Update Type

	CWidget_Base_FontGlyph* pWidget_Type = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_Type);

	if (pWidget_Type != NULL)
	{
		pWidget_Type->SetGlyph(pVoice->IsAStream() ? kcFontGlyph_Audio_Type_Stream : kcFontGlyph_Audio_Type_Static);
	}

	// Update Format

	CWidget_Base_FontGlyph* pWidget_Format = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_Format);

	if (pWidget_Format != NULL)
	{
		switch (pVoice->GetEncoding())
		{
			default:
			case eEncoding_PCM	: pWidget_Format->SetGlyph(kcFontGlyph_Audio_Format_PCM); break;
			case eEncoding_ADPCM: pWidget_Format->SetGlyph(kcFontGlyph_Audio_Format_ADPCM); break;
			case eEncoding_XMA	: pWidget_Format->SetGlyph(kcFontGlyph_Audio_Format_XMA); break;
		}		
	}

	// Update Channels

	CWidget_Base_FontGlyph* pWidget_Channels = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_Channels);

	if (pWidget_Channels != NULL)
	{
		switch (pVoice->GetChannelCount())
		{
			default:
			case 1: pWidget_Channels->SetGlyph(kcFontGlyph_Audio_Channels_Mono); break;
			case 2: pWidget_Channels->SetGlyph(kcFontGlyph_Audio_Channels_Stereo); break;
			case 6: pWidget_Channels->SetGlyph(kcFontGlyph_Audio_Channels_Dolby); break;
		}		
	}

	// Update Fx

	CWidget_Base_FontGlyph* pWidget_Fx = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_Fx);

	if (pWidget_Fx != NULL)
	{
		switch (pVoice->GetFXCoreID())
		{
			case -1				: pWidget_Fx->SetGlyph(kcFontGlyph_Audio_Fx_No); break;
			case SND_Cte_FxCoreA: pWidget_Fx->SetGlyph(kcFontGlyph_Audio_Fx_A); break;
			case SND_Cte_FxCoreB: pWidget_Fx->SetGlyph(kcFontGlyph_Audio_Fx_B); break;
		}
	}

	// Update State

	CWidget_Base_FontGlyph* pWidget_State = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_State);

	if (pWidget_State != NULL)
	{
		switch (pVoice->GetState())
		{
			case eState_None	: pWidget_State->SetGlyph(kcFontGlyph_Audio_State_Setup); break;
			case eState_Setup	: pWidget_State->SetGlyph(kcFontGlyph_Audio_State_Setup); break;
			case eState_Playing	: pWidget_State->SetGlyph(kcFontGlyph_Audio_State_Play); break;
			case eState_Paused	: pWidget_State->SetGlyph(kcFontGlyph_Audio_State_Pause); break;
			case eState_Stopped	: pWidget_State->SetGlyph(kcFontGlyph_Audio_State_Stop); break;
		}		
	}

	// Update Loop

	CWidget_Base_FontGlyph* pWidget_Loop = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_Loop);

	if (pWidget_Loop != NULL)
	{
		pWidget_Loop->SetGlyph(pVoice->IsLooping() ? kcFontGlyph_Audio_Loop_Yes : kcFontGlyph_Audio_Loop_No);
	}

	// Update Frequency

	CWidget_Base_Text* pWidget_Frequency = (CWidget_Base_Text*)GetSubWidget(m_WID_Frequency);

	if (pWidget_Frequency != NULL)
	{
		TWChar acValueBuffer[16];

		// Fetch Frequency

		int iFrequency = pVoice->GetFrequency();

		if (iFrequency < 0)
		{
			iFrequency = 0;
		}

		// Build String

		IntegerToString(iFrequency, acValueBuffer, TRUE);

		swprintf(acTextBuffer, L"%s Hz", acValueBuffer);

		// Update Text

		pWidget_Frequency->SetText(acTextBuffer);
	}

	// Update Volume

	CWidget_Base_ValueTracker* pWidget_Volume = (CWidget_Base_ValueTracker*)GetSubWidget(m_WID_Volume);

	if (pWidget_Volume != NULL)
	{
		pWidget_Volume->UpdateValue(SND_f_GetVolFromAtt(pVoice->GetVolume()) + pVoice->GetVolumeAddGain());
	}

	// Update Size / Duration - Playback

	CWidget_Base_Text* pWidget_SizeDur_Playback = (CWidget_Base_Text*)GetSubWidget(m_WID_SizeDur_Playback);

	if (pWidget_SizeDur_Playback != NULL)
	{
		TWChar acBuffer_BufferSize[16];

		// Build String

		IntegerToString(pVoice->GetPlaybackBuffer()->GetSize_Capacity(), acBuffer_BufferSize, TRUE);

		swprintf(acTextBuffer, L"Mem: %s Bytes (%.2fs)", acBuffer_BufferSize, pVoice->GetBufferDurationInSeconds());

		// Update Text

		pWidget_SizeDur_Playback->SetText(acTextBuffer);
	}

	// Update Size / Duration - File

	CWidget_Base_Text* pWidget_SizeDur_File = (CWidget_Base_Text*)GetSubWidget(m_WID_SizeDur_File);

	if (pWidget_SizeDur_File != NULL)
	{
		TWChar acBuffer_FileSize[16];

		// Build String

		IntegerToString(pVoice->GetFileSize(), acBuffer_FileSize, TRUE);

		swprintf(acTextBuffer, L"File: %s Bytes (%.2fs)", acBuffer_FileSize, pVoice->GetFileDurationInSeconds());

		// Update Text

		pWidget_SizeDur_File->SetText(acTextBuffer);
	}

	// Update Position Marker - Load File

	CWidget_Base_FontGlyph* pWidget_PositionMarker_LoadFile = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_PositionMarker_LoadFile);

	if (pWidget_PositionMarker_LoadFile != NULL)
	{
		TFloat fFactor = 0.0f;

		if (pVoice->GetFileSize() > 0)
		{
			fFactor = (float)pVoice->GetPosition_File_Current() / (float)pVoice->GetFileSize();
		}

		if (pVoice->IsAStream() && pVoice->IsPlaying())
		{
			pWidget_PositionMarker_LoadFile->Get_Position()->Get_X()->Set((kfBufferBar_File_Position_X - kfBufferBar_Marker_Offset_X) + (fFactor * kfBufferBar_Size_X));
			pWidget_PositionMarker_LoadFile->Get_Color()->Get_Alpha()->Set(1.0f);
		}
		else
		{
			pWidget_PositionMarker_LoadFile->Get_Color()->Get_Alpha()->Set(0.0f);
		}
	}

	// Update Position Marker - Load Playback

	CWidget_Base_FontGlyph* pWidget_PositionMarker_LoadPlayback = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_PositionMarker_LoadPlayback);

	if (pWidget_PositionMarker_LoadPlayback != NULL)
	{
		TFloat fFactor = 0.0f;

		if (pVoice->GetPlaybackBuffer()->GetSize_Capacity() > 0)
		{
			fFactor = (float)pVoice->GetPosition_Buffer_Load() / (float)pVoice->GetPlaybackBuffer()->GetSize_Capacity();
		}

		if (pVoice->IsAStream() && pVoice->IsPlaying())
		{
			pWidget_PositionMarker_LoadPlayback->Get_Position()->Get_X()->Set((kfBufferBar_Playback_Position_X - kfBufferBar_Marker_Offset_X) + (fFactor * kfBufferBar_Size_X));
			pWidget_PositionMarker_LoadPlayback->Get_Color()->Get_Alpha()->Set(1.0f);
		}
		else
		{
			pWidget_PositionMarker_LoadPlayback->Get_Color()->Get_Alpha()->Set(0.0f);
		}
	}

	// Update Position Marker - Loop Start

	CWidget_Base_FontGlyph* pWidget_PositionMarker_LoopStart = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_PositionMarker_LoopStart);

	if (pWidget_PositionMarker_LoopStart != NULL)
	{
		TFloat fFactor = 0.0f;

		if (pVoice->GetFileSize() > 0)
		{
			fFactor = (float)pVoice->GetPosition_File_LoopBegin() / (float)pVoice->GetFileSize();
		}

		if (fFactor > 0.0f)
		{
			pWidget_PositionMarker_LoopStart->Get_Position()->Get_X()->Set((kfBufferBar_File_Position_X - kfBufferBar_Marker_Offset_X) + (fFactor * kfBufferBar_Size_X));
			pWidget_PositionMarker_LoopStart->Get_Color()->Get_Alpha()->Set(1.0f);
		}
		else
		{
			pWidget_PositionMarker_LoopStart->Get_Color()->Get_Alpha()->Set(0.0f);
		}
	}

	// Update Position Marker - Loop End

	CWidget_Base_FontGlyph* pWidget_PositionMarker_LoopEnd = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_PositionMarker_LoopEnd);

	if (pWidget_PositionMarker_LoopEnd != NULL)
	{
		TFloat fFactor = 0.0f;

		if (pVoice->GetFileSize() > 0)
		{
			fFactor = (float)pVoice->GetPosition_File_LoopEnd() / (float)pVoice->GetFileSize();
		}

		if ((fFactor > 0.0f) && (fFactor < 1.0f))
		{
			pWidget_PositionMarker_LoopEnd->Get_Position()->Get_X()->Set((kfBufferBar_File_Position_X - kfBufferBar_Marker_Offset_X) + (fFactor * kfBufferBar_Size_X));
			pWidget_PositionMarker_LoopEnd->Get_Color()->Get_Alpha()->Set(1.0f);
		}
		else
		{
			pWidget_PositionMarker_LoopEnd->Get_Color()->Get_Alpha()->Set(0.0f);
		}
	}

	// Update Position Marker - Play

	CWidget_Base_FontGlyph* pWidget_PositionMarker_Play = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_PositionMarker_Play);

	if (pWidget_PositionMarker_Play != NULL)
	{
		TFloat fFactor = 0.0f;

		if (pVoice->GetPlaybackBuffer()->GetSize_Capacity() > 0)
		{
			fFactor = (float)pVoice->GetPosition_Buffer_Play() / (float)pVoice->GetPlaybackBuffer()->GetSize_Capacity();
		}

		if (pVoice->IsAStream() && pVoice->IsPlaying())
		{
			pWidget_PositionMarker_Play->Get_Position()->Get_X()->Set((kfBufferBar_Playback_Position_X - kfBufferBar_Marker_Offset_X) + (fFactor * kfBufferBar_Size_X));
			pWidget_PositionMarker_Play->Get_Color()->Get_Alpha()->Set(1.0f);
		}
		else
		{
			pWidget_PositionMarker_Play->Get_Color()->Get_Alpha()->Set(0.0f);
		}
	}

	// Update Position Marker - Decode

	CWidget_Base_FontGlyph* pWidget_PositionMarker_Decode = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_PositionMarker_Decode);

	if (pWidget_PositionMarker_Decode != NULL)
	{
		TFloat fFactor = 0.0f;

		if (pVoice->GetPlaybackBuffer()->GetSize_Capacity() > 0)
		{
			fFactor = (float)pVoice->GetPosition_Buffer_Decode() / (float)pVoice->GetPlaybackBuffer()->GetSize_Capacity();
		}

		if (pVoice->IsAStream() && pVoice->IsPlaying())
		{
			pWidget_PositionMarker_Decode->Get_Position()->Get_X()->Set((kfBufferBar_Playback_Position_X - kfBufferBar_Marker_Offset_X) + (fFactor * kfBufferBar_Size_X));
			pWidget_PositionMarker_Decode->Get_Color()->Get_Alpha()->Set(1.0f);
		}
		else
		{
			pWidget_PositionMarker_Decode->Get_Color()->Get_Alpha()->Set(0.0f);
		}
	}

	// Update Position Marker - Chain

	CWidget_Base_FontGlyph* pWidget_PositionMarker_Chain = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_PositionMarker_Chain);

	if (pWidget_PositionMarker_Chain != NULL)
	{
		TFloat fFactor = 0.0f;

		if (pVoice->GetFileSize() > 0)
		{
			fFactor = (float)pVoice->GetPosition_File_Chain() / (float)pVoice->GetFileSize();
		}

		if (fFactor > 0.0f)
		{
			pWidget_PositionMarker_Chain->Get_Position()->Get_X()->Set((kfBufferBar_File_Position_X - kfBufferBar_Marker_Offset_X) + (fFactor * kfBufferBar_Size_X));
			pWidget_PositionMarker_Chain->Get_Color()->Get_Alpha()->Set(1.0f);
		}
		else
		{
			pWidget_PositionMarker_Chain->Get_Color()->Get_Alpha()->Set(0.0f);
		}
	}

	// Update Position Marker - Cursor File

	CWidget_Base_FontGlyph* pWidget_PositionMarker_CursorFile = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_PositionMarker_CursorFile);

	if (pWidget_PositionMarker_CursorFile != NULL)
	{
		TFloat fFactor = 0.0f;

		if (pVoice->GetFileSize() > 0)
		{
			fFactor = (float)pVoice->GetCursorPosition_PCM() / (float)pVoice->GetFileSize();
		}

		if (pVoice->IsPlaying())
		{
			pWidget_PositionMarker_CursorFile->Get_Position()->Get_X()->Set((kfBufferBar_File_Position_X - kfBufferBar_Marker_Offset_X) + (fFactor * kfBufferBar_Size_X));
			pWidget_PositionMarker_CursorFile->Get_Color()->Get_Alpha()->Set(1.0f);
		}
		else
		{
			pWidget_PositionMarker_CursorFile->Get_Color()->Get_Alpha()->Set(0.0f);
		}
	}

	// Update Position Marker - Cursor Playback

	CWidget_Base_FontGlyph* pWidget_PositionMarker_CursorPlayback = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_PositionMarker_CursorPlayback);

	if (pWidget_PositionMarker_CursorPlayback != NULL)
	{
		TFloat fFactor = 0.0f;

		if (pVoice->GetPlaybackBuffer()->GetSize_Capacity() > 0)
		{
			fFactor = (float)pVoice->GetCursorInBuffer_PCM() / (float)pVoice->GetPlaybackBuffer()->GetSize_Capacity();
		}

		if (pVoice->IsPlaying())
		{
			pWidget_PositionMarker_CursorPlayback->Get_Position()->Get_X()->Set((kfBufferBar_Playback_Position_X - kfBufferBar_Marker_Offset_X) + (fFactor * kfBufferBar_Size_X));
			pWidget_PositionMarker_CursorPlayback->Get_Color()->Get_Alpha()->Set(1.0f);
		}
		else
		{
			pWidget_PositionMarker_CursorPlayback->Get_Color()->Get_Alpha()->Set(0.0f);
		}
	}

	// Update Pan

	TInt	iPanLeftRight = pVoice->GetPan();
	TInt	iPanFrontBack = pVoice->GetSurroundPan();
	TFloat	fPanLeftRight;
	TFloat	fPanFrontBack;

	// - Calculate Factor (Left / Right)

	if (iPanLeftRight < 0)
	{
		fPanLeftRight = -(1.0f - SND_f_GetVolFromAtt(iPanLeftRight));
	}
	else
	{
		fPanLeftRight = 1.0f - SND_f_GetVolFromAtt(-iPanLeftRight);
	}

	// - Calculate Factor (Front / Back)

	if (iPanFrontBack < 0)
	{
		fPanFrontBack = 1.0f - SND_f_GetVolFromAtt(iPanFrontBack);
	}
	else
	{
		fPanFrontBack = -(1.0f - SND_f_GetVolFromAtt(-iPanFrontBack));
	}

	// - Move Arrow Widget

	CWidget_Base_FontGlyph* pWidget_PanArrow = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_Pan_Arrow);

	if (pWidget_PanArrow != NULL)
	{
		TFloat fPosition_X = kfBuffers_PanBase_Position_X + kfBuffers_PanArrow_Center_X + (fPanLeftRight * kfBuffers_PanArrow_Range_X);
		TFloat fPosition_Y = kfBuffers_Icon_Position_Y + kfBuffers_PanArrow_Center_Y + (fPanFrontBack * kfBuffers_PanArrow_Range_Y);

		pWidget_PanArrow->Get_Position()->Set(fPosition_X, fPosition_Y);
	}
}

// ***********************************************************************************************************************

void CWidget_Audio_Panel_Buffer::Signal(eNavLink Param_eNavLink)
{
	// Fetch Associated Voice

	CVoice* pVoice = GetAudioEngine()->GetVoice(m_iBufferID);

	if (pVoice == NULL)
	{
		return;
	}

	// Determine Action (Based on NavLink)

	switch (Param_eNavLink)
	{
		case eNavLink_Forward:
		{
			// Play (If Applicable)

			if (!pVoice->IsPlaying())
			{
				pVoice->Play();
			}
			break;
		}

		case eNavLink_Back:
		{
			// Stop (If Applicable)

			if (pVoice->IsPlaying())
			{
				pVoice->Stop();
			}
			break;
		}

		case eNavLink_Left:
		{
			// Lower Volume Additive Gain

			pVoice->SetVolumeAddGain(pVoice->GetVolumeAddGain() - 0.1f);
			break;
		}

		case eNavLink_Right:
		{
			// Raise Volume Additive Gain

			pVoice->SetVolumeAddGain(pVoice->GetVolumeAddGain() + 0.1f);
			break;
		}
	}
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Audio_Window_Memory
// ***********************************************************************************************************************

CWidget_Audio_Window_Memory::CWidget_Audio_Window_Memory() : CWidget_Base_Window(L"Memory")
{
	CWidget_Base_Text*			pWidget_Text;
	CWidget_Base_FontGlyph*		pWidget_Glyph;
	CWidget_Base_ValueTracker*	pWidget_ValueTracker;
	float						fPosition_Y = 0.0f;

	// Create SubWidget - Allocated Section Name

	fPosition_Y += 0.07;

	pWidget_Text = new CWidget_Base_Text(L"Allocated Memory");

	pWidget_Text->Get_Position()->Set(0.32f, fPosition_Y);
	pWidget_Text->Get_TextSize()->Set(kfMemory_Text_Size);

	m_WID_Allocated_Name = AddSubWidget(pWidget_Text);

	fPosition_Y += 0.05f;

	// Create SubWidget - Allocated Section Line

	pWidget_Glyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_Bottom);

	pWidget_Glyph->Get_Position()->Set(0.2f, fPosition_Y);
	pWidget_Glyph->Get_Size()->Set(4.5f, 0.1f);

	m_WID_Allocated_Line = AddSubWidget(pWidget_Glyph);

	fPosition_Y += 0.02f;

	// Create SubWidget - Allocated General

	pWidget_ValueTracker = new CWidget_Base_ValueTracker(L"General", kstTrackerType_Integer);

	pWidget_ValueTracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_ValueTracker->SetTextSize(kfMemory_Text_Size);

	m_WID_General_Allocated = AddSubWidget(pWidget_ValueTracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Allocated Static

	pWidget_ValueTracker = new CWidget_Base_ValueTracker(L"Static Buffers", kstTrackerType_Integer);

	pWidget_ValueTracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_ValueTracker->SetTextSize(kfMemory_Text_Size);

	m_WID_Static_Allocated = AddSubWidget(pWidget_ValueTracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Allocated Stream

	pWidget_ValueTracker = new CWidget_Base_ValueTracker(L"Stream Buffers", kstTrackerType_Integer);

	pWidget_ValueTracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_ValueTracker->SetTextSize(kfMemory_Text_Size);

	m_WID_Stream_Allocated = AddSubWidget(pWidget_ValueTracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Block Section Name

	fPosition_Y += 0.05f;

	pWidget_Text = new CWidget_Base_Text(L"Block Count");

	pWidget_Text->Get_Position()->Set(0.40f, fPosition_Y);
	pWidget_Text->Get_TextSize()->Set(kfMemory_Text_Size);

	m_WID_Blocks_Name = AddSubWidget(pWidget_Text);

	fPosition_Y += 0.05f;

	// Create SubWidget - Block Section Line

	pWidget_Glyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_Bottom);

	pWidget_Glyph->Get_Position()->Set(0.2f, fPosition_Y);
	pWidget_Glyph->Get_Size()->Set(4.5f, 0.1f);

	m_WID_Blocks_Line = AddSubWidget(pWidget_Glyph);

	fPosition_Y += 0.02f;

	// Create SubWidget - Blocks Struct

	pWidget_ValueTracker = new CWidget_Base_ValueTracker(L"General", kstTrackerType_Integer);

	pWidget_ValueTracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_ValueTracker->SetTextSize(kfMemory_Text_Size);

	m_WID_General_Blocks = AddSubWidget(pWidget_ValueTracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Blocks Static

	pWidget_ValueTracker = new CWidget_Base_ValueTracker(L"Static Buffers", kstTrackerType_Integer);

	pWidget_ValueTracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_ValueTracker->SetTextSize(kfMemory_Text_Size);

	m_WID_Static_Blocks = AddSubWidget(pWidget_ValueTracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Blocks Stream

	pWidget_ValueTracker = new CWidget_Base_ValueTracker(L"Stream Buffers", kstTrackerType_Integer);

	pWidget_ValueTracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_ValueTracker->SetTextSize(kfMemory_Text_Size);

	m_WID_Stream_Blocks = AddSubWidget(pWidget_ValueTracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Totals Section Name

	fPosition_Y += 0.05f;

	pWidget_Text = new CWidget_Base_Text(L"Totals");

	pWidget_Text->Get_Position()->Set(0.45f, fPosition_Y);
	pWidget_Text->Get_TextSize()->Set(kfMemory_Text_Size);

	m_WID_Totals_Name = AddSubWidget(pWidget_Text);

	fPosition_Y += 0.05f;

	// Create SubWidget - Totals Section Line

	pWidget_Glyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_Bottom);

	pWidget_Glyph->Get_Position()->Set(0.2f, fPosition_Y);
	pWidget_Glyph->Get_Size()->Set(4.5f, 0.1f);

	m_WID_Totals_Line = AddSubWidget(pWidget_Glyph);

	fPosition_Y += 0.02f;

	// Create SubWidget - Totals Allocated

	pWidget_ValueTracker = new CWidget_Base_ValueTracker(L"Allocated", kstTrackerType_Integer);

	pWidget_ValueTracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_ValueTracker->SetTextSize(kfMemory_Text_Size);

	m_WID_Totals_Allocated = AddSubWidget(pWidget_ValueTracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Totals Block

	pWidget_ValueTracker = new CWidget_Base_ValueTracker(L"Blocks", kstTrackerType_Integer);

	pWidget_ValueTracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_ValueTracker->SetTextSize(kfMemory_Text_Size);

	m_WID_Totals_Blocks = AddSubWidget(pWidget_ValueTracker);

	fPosition_Y += 0.05f;
}

// ***********************************************************************************************************************

CWidget_Audio_Window_Memory::~CWidget_Audio_Window_Memory()
{
}

// ***********************************************************************************************************************

void CWidget_Audio_Window_Memory::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Reject Further Processing If Not Visible (Saves CPU Cycles)

	if (!IsVisible())
	{
		return;
	}

	// Update Trackers

	UpdateTrackerValue(m_WID_General_Allocated	, GetAudioEngine()->GetMemoryTracker(eMemoryTracker_General      )->iAllocatedSize);
	UpdateTrackerValue(m_WID_General_Blocks		, GetAudioEngine()->GetMemoryTracker(eMemoryTracker_General      )->iBlockCount);
	UpdateTrackerValue(m_WID_Static_Allocated	, GetAudioEngine()->GetMemoryTracker(eMemoryTracker_Buffer_Static)->iAllocatedSize);
	UpdateTrackerValue(m_WID_Static_Blocks		, GetAudioEngine()->GetMemoryTracker(eMemoryTracker_Buffer_Static)->iBlockCount);
	UpdateTrackerValue(m_WID_Stream_Allocated	, GetAudioEngine()->GetMemoryTracker(eMemoryTracker_Buffer_Stream)->iAllocatedSize);
	UpdateTrackerValue(m_WID_Stream_Blocks		, GetAudioEngine()->GetMemoryTracker(eMemoryTracker_Buffer_Stream)->iBlockCount);

	int TotalAllocated	= 0;
	int TotalBlocks		= 0;

	for (int Loop = 0; Loop < eMemoryTracker_Count; Loop++)
	{
		TotalAllocated	+= GetAudioEngine()->GetMemoryTracker((eMemoryTrackers)Loop)->iAllocatedSize;
		TotalBlocks		+= GetAudioEngine()->GetMemoryTracker((eMemoryTrackers)Loop)->iBlockCount;
	}

	UpdateTrackerValue(m_WID_Totals_Allocated	, TotalAllocated);
	UpdateTrackerValue(m_WID_Totals_Blocks		, TotalBlocks);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Audio_Window_Effects
// ***********************************************************************************************************************

CWidget_Audio_Window_Effects::CWidget_Audio_Window_Effects() : CWidget_Base_Window(L"Effects")
{
	CWidget_Base_Text*			pWidget_Text;
	CWidget_Base_FontGlyph*		pWidget_Glyph;
	CWidget_Base_ValueTracker*	pWidget_Tracker;
	float						fPosition_Y = 0.0f;

	// Create SubWidget - Core A Title

	fPosition_Y += 0.07;

	pWidget_Text = new CWidget_Base_Text(L"Core A");

	pWidget_Text->Get_Position()->Set(0.35f, fPosition_Y);
	pWidget_Text->Get_TextSize()->Set(kfEffects_Text_Size);

	m_WID_CoreA_Title = AddSubWidget(pWidget_Text);

	fPosition_Y += 0.05f;

	// Create SubWidget - Core A Line

	pWidget_Glyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_Bottom);

	pWidget_Glyph->Get_Position()->Set(0.2f, fPosition_Y);
	pWidget_Glyph->Get_Size()->Set(3.5f, 0.1f);

	m_WID_CoreA_Line = AddSubWidget(pWidget_Glyph);

	fPosition_Y += 0.02f;

	// Create SubWidget - Core A Enabled

	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Enabled", kstTrackerType_Boolean);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreA_Enabled = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Core A Mode

	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Mode", kstTrackerType_Enum_FXModes);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreA_Mode = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Core A Delay
/*
	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Delay", kstTrackerType_Integer);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreA_Delay = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;
*/
	// Create SubWidget - Core A Feedback
/*
	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Feedback", kstTrackerType_Integer);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreA_Feedback = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;
*/
	// Create SubWidget - Core A Wet Volume

	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Wet Volume", kstTrackerType_Float);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreA_WetVolume = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Core A Wet Pan

	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Wet Pan", kstTrackerType_Integer);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreA_WetPan = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Core A Delay Algorithm
/*
	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Delay Algorithm", kstTrackerType_Enum_FXDelayAlgorithms);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreA_DelayAlgorithm = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;
*/
	// Create SubWidget - Core B Title

	fPosition_Y += 0.05;

	pWidget_Text = new CWidget_Base_Text(L"Core B");

	pWidget_Text->Get_Position()->Set(0.35f, fPosition_Y);
	pWidget_Text->Get_TextSize()->Set(kfEffects_Text_Size);

	m_WID_CoreB_Title = AddSubWidget(pWidget_Text);

	fPosition_Y += 0.05f;

	// Create SubWidget - Core B Line

	pWidget_Glyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_Bottom);

	pWidget_Glyph->Get_Position()->Set(0.2f, fPosition_Y);
	pWidget_Glyph->Get_Size()->Set(3.5f, 0.1f);

	m_WID_CoreB_Line = AddSubWidget(pWidget_Glyph);

	fPosition_Y += 0.02f;

	// Create SubWidget - Core B Enabled

	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Enabled", kstTrackerType_Boolean);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreB_Enabled = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Core B Mode

	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Mode", kstTrackerType_Enum_FXModes);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreB_Mode = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Core B Delay
/*
	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Delay", kstTrackerType_Integer);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreB_Delay = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;
*/
	// Create SubWidget - Core B Feedback
/*
	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Feedback", kstTrackerType_Integer);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreB_Feedback = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;
*/
	// Create SubWidget - Core B Wet Volume

	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Wet Volume", kstTrackerType_Float);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreB_WetVolume = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Core B Wet Pan

	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Wet Pan", kstTrackerType_Integer);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreB_WetPan = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;

	// Create SubWidget - Core B Delay Algorithm
/*
	pWidget_Tracker = new CWidget_Base_ValueTracker(L"Delay Algorithm", kstTrackerType_Enum_FXDelayAlgorithms);

	pWidget_Tracker->SetPositions(0.05f, 0.6f, fPosition_Y);
	pWidget_Tracker->SetTextSize(kfEffects_Text_Size);

	m_WID_CoreB_DelayAlgorithm = AddSubWidget(pWidget_Tracker);

	fPosition_Y += 0.05f;
*/
}

// ***********************************************************************************************************************

CWidget_Audio_Window_Effects::~CWidget_Audio_Window_Effects()
{
}

// ***********************************************************************************************************************

void CWidget_Audio_Window_Effects::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Reject Further Processing If Not Visible (Saves CPU Cycles)

	if (!IsVisible())
	{
		return;
	}

	// Update Trackers

	UpdateTrackerValue(m_WID_CoreA_Enabled			, (TBool )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->GetEnabled());
	UpdateTrackerValue(m_WID_CoreA_Mode				, (TInt  )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->GetMode());
	//UpdateTrackerValue(m_WID_CoreA_Delay			, (TInt  )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->GetDelay());
	//UpdateTrackerValue(m_WID_CoreA_Feedback			, (TInt  )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->GetFeedback());
	UpdateTrackerValue(m_WID_CoreA_WetVolume		, (TFloat)GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->GetWetVolume());
	UpdateTrackerValue(m_WID_CoreA_WetPan			, (TInt  )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->GetWetPan());
	//UpdateTrackerValue(m_WID_CoreA_DelayAlgorithm	, (TInt  )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreA)->GetDelayAlgorithm());
	UpdateTrackerValue(m_WID_CoreB_Enabled			, (TBool )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreB)->GetEnabled());
	UpdateTrackerValue(m_WID_CoreB_Mode				, (TInt  )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreB)->GetMode());
	//UpdateTrackerValue(m_WID_CoreB_Delay			, (TInt  )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreB)->GetDelay());
	//UpdateTrackerValue(m_WID_CoreB_Feedback			, (TInt  )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreB)->GetFeedback());
	UpdateTrackerValue(m_WID_CoreB_WetVolume		, (TFloat)GetAudioEngine()->GetFXCore(SND_Cte_FxCoreB)->GetWetVolume());
	UpdateTrackerValue(m_WID_CoreB_WetPan			, (TInt  )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreB)->GetWetPan());
	//UpdateTrackerValue(m_WID_CoreB_DelayAlgorithm	, (TInt  )GetAudioEngine()->GetFXCore(SND_Cte_FxCoreB)->GetDelayAlgorithm());
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Audio_Window_Buffers
// ***********************************************************************************************************************

CWidget_Audio_Window_Buffers::CWidget_Audio_Window_Buffers() : CWidget_Base_Window_Panels(L"Buffers", kfPanel_Height_Buffers)
{
	// Initialize Navigation

	SetNavLink(eNavLink_Forward, kiWidgetID_Signal);
	SetNavLink(eNavLink_Back   , kiWidgetID_Signal);

	// Create SubWidget - Info - Count

	CWidget_Base_Text* pWidget_Info_Count = new CWidget_Base_Text();

	pWidget_Info_Count->Get_Position()->Set(0.25f, 0.95f);
	pWidget_Info_Count->Get_TextSize()->Set(0.2f);

	m_WID_Info_Count = AddSubWidget(pWidget_Info_Count);

	// Create SubWidget - Info - Memory Used

	CWidget_Base_Text* pWidget_Info_MemoryUsed = new CWidget_Base_Text();

	pWidget_Info_MemoryUsed->Get_Position()->Set(0.50f, 0.95f);
	pWidget_Info_MemoryUsed->Get_TextSize()->Set(0.2f);

	m_WID_Info_MemoryUsed = AddSubWidget(pWidget_Info_MemoryUsed);

	// Initialize Fields

	m_iWorldID = kiEngineResID_None;
}

// ***********************************************************************************************************************

CWidget_Audio_Window_Buffers::~CWidget_Audio_Window_Buffers()
{
}

// ***********************************************************************************************************************

void CWidget_Audio_Window_Buffers::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Reject Further Processing If Not Visible (Saves CPU Cycles)

	if (!IsVisible())
	{
		return;
	}

	// Fetch PanelList Widget

	CWidget_Base_PanelList* pWidget_PanelList = (CWidget_Base_PanelList*)GetSubWidget(m_WID_PanelList);

	if (pWidget_PanelList == NULL)
	{
		return;
	}

	// Reset List (On World Change)

	if (m_iWorldID != World_GetID())
	{
		pWidget_PanelList->Reset();
	}

	// Add Panels (If Necessary)

	for (TInt iLoop = pWidget_PanelList->GetCount(); iLoop < AudioBuffers_GetCount(); iLoop++)
	{
		AddPanel(new CWidget_Audio_Panel_Buffer(iLoop));
	}

	// Update On-Screen Info - Count

	TWChar acTextBuffer[64];

	CWidget_Base_Text* pWidget_Info_Count = (CWidget_Base_Text*)GetSubWidget(m_WID_Info_Count);

	if (pWidget_Info_Count != NULL)
	{
		swprintf(acTextBuffer, L"Count: %i", AudioBuffers_GetCount());

		pWidget_Info_Count->SetText(acTextBuffer);
	}

	// Update On-Screen Info - Memory Used

	CWidget_Base_Text* pWidget_Info_MemoryUsed = (CWidget_Base_Text*)GetSubWidget(m_WID_Info_MemoryUsed);

	if (pWidget_Info_MemoryUsed != NULL)
	{
		swprintf(acTextBuffer, L"Memory Used: %3.2fMB", (float)AudioBuffers_GetUsedMemory() / 1048576.0f);

		pWidget_Info_MemoryUsed->SetText(acTextBuffer);
	}

	// Update World ID

	m_iWorldID = World_GetID();
}

// ***********************************************************************************************************************

void CWidget_Audio_Window_Buffers::Signal(eNavLink Param_eNavLink)
{
	// Fetch Panel List Widget

	CWidget_Base_PanelList* pWidget_PanelList = (CWidget_Base_PanelList*)GetSubWidget(m_WID_PanelList);

	if (pWidget_PanelList == NULL)
	{
		return;
	}

	// Forward Signal To Selected Panel

	stPanelList_Entry* pstEntry = pWidget_PanelList->GetSelectedPanel();

	if (pstEntry != NULL)
	{
		CWidget_Audio_Panel_Buffer* pWidget_Panel = (CWidget_Audio_Panel_Buffer*)pWidget_PanelList->GetSubWidget(pstEntry->iWidgetID);

		if (pWidget_Panel != NULL)
		{
			pWidget_Panel->Signal(Param_eNavLink);
		}
	}
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Audio_Window_Events
// ***********************************************************************************************************************

CWidget_Audio_Window_Events::CWidget_Audio_Window_Events() : CWidget_Base_Window_Message(L"Events", kiEvents_List_MaxSize)
{
}

// ***********************************************************************************************************************

CWidget_Audio_Window_Events::~CWidget_Audio_Window_Events()
{
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Audio_Window_Messages
// ***********************************************************************************************************************

CWidget_Audio_Window_Messages::CWidget_Audio_Window_Messages() : CWidget_Base_Window_Message(L"Messages", kiMessages_List_MaxSize)
{
}

// ***********************************************************************************************************************

CWidget_Audio_Window_Messages::~CWidget_Audio_Window_Messages()
{
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Audio_Window_Prefetch
// ***********************************************************************************************************************

CWidget_Audio_Window_Prefetch::CWidget_Audio_Window_Prefetch() : CWidget_Base_Window(L"Prefetch")
{
	// Create SubWidget - Count

	CWidget_Base_ValueTracker* pWidget_ListSize = new CWidget_Base_ValueTracker(L"List Size", kstTrackerType_Integer);

	pWidget_ListSize->SetPositions(0.01f, 0.60f, 0.05f);
	pWidget_ListSize->SetTextSize(0.2f);

	m_WID_ListSize = AddSubWidget(pWidget_ListSize);
}

// ***********************************************************************************************************************

CWidget_Audio_Window_Prefetch::~CWidget_Audio_Window_Prefetch()
{
}

// ***********************************************************************************************************************

void CWidget_Audio_Window_Prefetch::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Update List Count

	CWidget_Base_ValueTracker* pWidget_ListSize = (CWidget_Base_ValueTracker*)GetSubWidget(m_WID_ListSize);

	if (pWidget_ListSize != NULL)
	{
		pWidget_ListSize->UpdateValue(GetAudioEngine()->GetPrefetchList()->GetItemCount());
	}
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Audio_Window_Configuration
// ***********************************************************************************************************************

CWidget_Audio_Window_Configuration::CWidget_Audio_Window_Configuration() : CWidget_Base_Window_Configuration(eConfiguration_Count, (stConfigurationEntry*)kstConfiguration)
{
}

// ***********************************************************************************************************************

CWidget_Audio_Window_Configuration::~CWidget_Audio_Window_Configuration()
{
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Audio_Window_Settings
// ***********************************************************************************************************************

CWidget_Audio_Window_Settings::CWidget_Audio_Window_Settings() : CWidget_Base_Window_Options(L"Runtime Settings", 0.7f)
{
}

// ***********************************************************************************************************************

CWidget_Audio_Window_Settings::~CWidget_Audio_Window_Settings()
{
}

// ***********************************************************************************************************************
//    Class Methods : CAudioConsole
// ***********************************************************************************************************************

CAudioConsole::CAudioConsole()
{
	m_pDesktop				= NULL;

	m_WID_Window_Memory		= kiWidgetID_Invalid;
	m_WID_Window_Buffers	= kiWidgetID_Invalid;
	m_WID_Window_Events		= kiWidgetID_Invalid;
	m_WID_Window_Messages	= kiWidgetID_Invalid;
	m_WID_Window_Settings	= kiWidgetID_Invalid;

	m_WID_NotifyIcon_Error	= kiWidgetID_Invalid;
	m_WID_NotifyIcon_Disc	= kiWidgetID_Invalid;
}

// ***********************************************************************************************************************

CAudioConsole::~CAudioConsole()
{
	Uninit();
}

// ***********************************************************************************************************************

void CAudioConsole::Initialize()
{
	// Ensure Non-Initialization

	if (m_pDesktop != NULL)
	{
		return;
	}

	// Create Desktop

	m_pDesktop = new CDesktop(L"Audio Console", kiColor_Desktop_Theme_Blue);

	// Create Additional Fonts

	g_FontManager.CreateFont(kiFontID_AudioConsole, kiBigFileKey_Texture_AudioConsole, kiBigFileKey_Texture_AudioConsole, 2.0f, 4.0f);

	// Create Windows

	m_WID_Window_Effects	= m_pDesktop->AddWindow(new CWidget_Audio_Window_Effects());
	m_WID_Window_Events		= m_pDesktop->AddWindow(new CWidget_Audio_Window_Events());
	m_WID_Window_Messages	= m_pDesktop->AddWindow(new CWidget_Audio_Window_Messages());
	m_WID_Window_Memory		= m_pDesktop->AddWindow(new CWidget_Audio_Window_Memory());
	m_WID_Window_Buffers	= m_pDesktop->AddWindow(new CWidget_Audio_Window_Buffers());
	m_WID_Window_Prefetch	= m_pDesktop->AddWindow(new CWidget_Audio_Window_Prefetch());
	m_WID_Window_Settings	= m_pDesktop->AddWindow(new CWidget_Audio_Window_Settings());

	m_pDesktop->AddConfigurationWindow(new CWidget_Audio_Window_Configuration());

	// Setup Desktop Parameters

	m_pDesktop->SetInputMonitoringToggleButtons(eButton_Thumb_Right, eButton_DPad_Down);
	m_pDesktop->SetupIconBar(kfIcon_Audio_StartPosition_Y, kcFontGlyph_Audio_Icon_Input);

	// Create Notification Icons

	m_WID_NotifyIcon_Error	= m_pDesktop->AddNotifyIcon(new CWidget_Base_NotifyIcon(kcFontGlyph_Audio_Icon_Error, 5.0f, 1.0f));
	m_WID_NotifyIcon_Disc	= m_pDesktop->AddNotifyIcon(new CWidget_Base_NotifyIcon(kcFontGlyph_Audio_Icon_Disc , 3.0f, 1.0f));
}

// ***********************************************************************************************************************

void CAudioConsole::Uninit()
{
	if (m_pDesktop != NULL)
	{
		delete m_pDesktop;
		m_pDesktop = NULL;
	}
}

// ***********************************************************************************************************************

void CAudioConsole::Tick(TFloat Param_fSecTimeDelta)
{
	Profile_Begin("AudioConsole.Tick");

	if (m_pDesktop != NULL)
	{
		m_pDesktop->Tick(Param_fSecTimeDelta);
	}

	Profile_End();
}

// ***********************************************************************************************************************

void CAudioConsole::Render()
{
	Profile_Begin("AudioConsole.Render");

	if (m_pDesktop != NULL)
	{
		m_pDesktop->Render();
	}

	Profile_End();
}

// ***********************************************************************************************************************

void CAudioConsole::AddEvent(TPWString Param_pString, TPWString Param_pModuleName, stStringList_EntryType Param_stEntryType)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveEventLog))
	{
		((CWidget_Audio_Window_Events*)m_pDesktop->GetWindow(m_WID_Window_Events))->AddMessage(m_pDesktop->GetMessageMaker()->Make(Param_pString, Param_pModuleName), Param_stEntryType);
	}
}

// ***********************************************************************************************************************

void CAudioConsole::AddMessage(TPWString Param_pString, TPWString Param_pModuleName, stStringList_EntryType Param_stEntryType)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		((CWidget_Audio_Window_Messages*)m_pDesktop->GetWindow(m_WID_Window_Messages))->AddMessage(m_pDesktop->GetMessageMaker()->Make(Param_pString, Param_pModuleName), Param_stEntryType);

		// Error Notification

		if ((Param_stEntryType.iFlags_StringList & kiFlags_StringList_IsAnError) && GetConfiguration(eConfiguration_Notify_ErrorMessage))
		{
			m_pDesktop->ActivateNotifyIcon(m_WID_NotifyIcon_Error);
		}
	}
}

// ***********************************************************************************************************************

void CAudioConsole::AddParameter(TPWString Param_pValue, TPWString Param_pName, TBool Param_bIsEvent)
{
	if (m_pDesktop != NULL)
	{
		if ((Param_bIsEvent && GetConfiguration(eConfiguration_ActiveEventLog)) || (!Param_bIsEvent && GetConfiguration(eConfiguration_ActiveMessageLog)))
		{
			m_pDesktop->GetMessageMaker()->AddParameter(Param_pValue, Param_pName);
		}
	}
}

// ***********************************************************************************************************************

void CAudioConsole::AddParameter(TInt Param_iValue, TPWString Param_pName, TBool Param_bIsEvent, TBool Param_bDisplayAsHex)
{
	if (m_pDesktop != NULL)
	{
		if ((Param_bIsEvent && GetConfiguration(eConfiguration_ActiveEventLog)) || (!Param_bIsEvent && GetConfiguration(eConfiguration_ActiveMessageLog)))
		{
			m_pDesktop->GetMessageMaker()->AddParameter(Param_iValue, Param_pName, Param_bDisplayAsHex);
		}
	}
}

// ***********************************************************************************************************************

void CAudioConsole::AddParameter(TFloat Param_fValue, TPWString Param_pName, TBool Param_bIsEvent)
{
	if (m_pDesktop != NULL)
	{
		if ((Param_bIsEvent && GetConfiguration(eConfiguration_ActiveEventLog)) || (!Param_bIsEvent && GetConfiguration(eConfiguration_ActiveMessageLog)))
		{
			m_pDesktop->GetMessageMaker()->AddParameter(Param_fValue, Param_pName);
		}
	}
}

// ***********************************************************************************************************************

void CAudioConsole::AddParameter(TBool Param_bValue, TPWString Param_pName, TBool Param_bIsEvent)
{
	if (m_pDesktop != NULL)
	{
		if ((Param_bIsEvent && GetConfiguration(eConfiguration_ActiveEventLog)) || (!Param_bIsEvent && GetConfiguration(eConfiguration_ActiveMessageLog)))
		{
			m_pDesktop->GetMessageMaker()->AddParameter(Param_bValue, Param_pName);
		}
	}
}

// ***********************************************************************************************************************

void CAudioConsole::WaitForMessageExclusion()
{
	if (m_pDesktop != NULL)
	{
		m_pDesktop->WaitForMessageExclusion();
	}
}

// ***********************************************************************************************************************

TBool CAudioConsole::GetConfiguration(eConfiguration_Options Param_eOption)
{
	// Error Checking

	if (m_pDesktop == NULL)
	{
		return FALSE;
	}

	// Fetch Configuration Option

	return m_pDesktop->GetConfiguration(Param_eOption);
}

// ***********************************************************************************************************************

TListID CAudioConsole::AddSetting(TPWString Param_pName, void* Param_pLinkedVariable, stTrackerType Param_stType, fnOptionNotify Param_fnOptionNotify)
{
	// Error Checking

	if (m_pDesktop == NULL)
	{
		return kiListID_Invalid;
	}

	// Force Empty Type If No Linked Variable

	if (Param_pLinkedVariable == NULL)
	{
		Param_stType = kstTrackerType_None;
	}

	// Fetch Options Window

	CWidget_Audio_Window_Settings* pWindow = ((CWidget_Audio_Window_Settings*)m_pDesktop->GetWindow(m_WID_Window_Settings));

	if (pWindow == NULL)
	{
		return kiListID_Invalid;
	}

	// Add Option

	TListID iOptionID = pWindow->AddOption(Param_pName, Param_stType, Param_pLinkedVariable);

	// Set Notify Callback (If Applicable)

	if (Param_fnOptionNotify != NULL)
	{
		pWindow->SetNotifyCallback(iOptionID, Param_fnOptionNotify);
	}

	return iOptionID;
}

// ***********************************************************************************************************************

TListID CAudioConsole::AddSetting(TPWString Param_pName, TBool* Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify)
{
	// Error Checking

	if (m_pDesktop == NULL)
	{
		return kiListID_Invalid;
	}

	// Fetch Options Window

	CWidget_Audio_Window_Settings* pWindow = ((CWidget_Audio_Window_Settings*)m_pDesktop->GetWindow(m_WID_Window_Settings));

	if (pWindow == NULL)
	{
		return kiListID_Invalid;
	}

	// Add Option

	TListID iOptionID = pWindow->AddOption(Param_pName, kstTrackerType_Boolean, Param_pLinkedVariable);

	// Set Notify Callback (If Applicable)

	if (Param_fnOptionNotify != NULL)
	{
		pWindow->SetNotifyCallback(iOptionID, Param_fnOptionNotify);
	}

	return iOptionID;
}

// ***********************************************************************************************************************

TListID CAudioConsole::AddSetting(TPWString Param_pName, TInt* Param_pLinkedVariable, TInt Param_iMinimum, TInt Param_iMaximum, TInt Param_iDelta, fnOptionNotify_Int Param_fnOptionNotify)
{
	// Error Checking

	if (m_pDesktop == NULL)
	{
		return kiListID_Invalid;
	}

	// Fetch Options Window

	CWidget_Audio_Window_Settings* pWindow = ((CWidget_Audio_Window_Settings*)m_pDesktop->GetWindow(m_WID_Window_Settings));

	if (pWindow == NULL)
	{
		return kiListID_Invalid;
	}

	// Add Option

	TListID iOptionID = pWindow->AddOption(Param_pName, kstTrackerType_Integer, Param_pLinkedVariable);

	// Set Option Limits

	pWindow->SetLimits(iOptionID, Param_iMinimum, Param_iMaximum, Param_iDelta);

	// Set Notify Callback (If Applicable)

	if (Param_fnOptionNotify != NULL)
	{
		pWindow->SetNotifyCallback(iOptionID, Param_fnOptionNotify);
	}

	return iOptionID;
}

// ***********************************************************************************************************************

TListID CAudioConsole::AddSetting(TPWString Param_pName, TFloat* Param_pLinkedVariable, TFloat Param_fMinimum, TFloat Param_fMaximum, TFloat Param_fDelta, fnOptionNotify_Float Param_fnOptionNotify)
{
	// Error Checking

	if (m_pDesktop == NULL)
	{
		return kiListID_Invalid;
	}

	// Fetch Options Window

	CWidget_Audio_Window_Settings* pWindow = ((CWidget_Audio_Window_Settings*)m_pDesktop->GetWindow(m_WID_Window_Settings));

	if (pWindow == NULL)
	{
		return kiListID_Invalid;
	}

	// Add Option

	TListID iOptionID = pWindow->AddOption(Param_pName, kstTrackerType_Float, Param_pLinkedVariable);

	// Set Option Limits

	pWindow->SetLimits(iOptionID, Param_fMinimum, Param_fMaximum, Param_fDelta);

	// Set Notify Callback (If Applicable)

	if (Param_fnOptionNotify != NULL)
	{
		pWindow->SetNotifyCallback(iOptionID, Param_fnOptionNotify);
	}

	return iOptionID;
}

// ***********************************************************************************************************************

#endif // AUDIOCONSOLE_ENABLE

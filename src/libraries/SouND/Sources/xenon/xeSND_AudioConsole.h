// ****************************************
// Audio Console Debugging Desktop
//
// Used for King Kong Xenon Sound Debugging
//
// By Alexandre David (January 2005)
// ****************************************

#ifndef __xeSND_AudioConsole_h__
#define __xeSND_AudioConsole_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "Xenon/Desktop/Desktop.h"

#if defined(AUDIOCONSOLE_ENABLE)

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "xeSND_Engine.h"

// ***********************************************************************************************************************

namespace AudioConsole
{

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const TPWString kszModuleName_XenonSound = L"XeSnd";

// ***********************************************************************************************************************

const TPWString	kaszTrackerEnumStrings_FXCores[] =
{
	L"Free",
	L"Core A",
	L"Core B"
};

// ***********************************************************************************************************************

const TPWString	kaszTrackerEnumStrings_FXModes[] =
{
	L"00 (None)",
	L"01 (Room)",
	L"02 (Studio A)",
	L"03 (Studio B)",
	L"04 (Studio C)",
	L"05 (Hall)",
	L"06 (Space)",
	L"07 (Echo)",
	L"08 (Delay)",
	L"09 (Pipe)",
	L"10 (Mountains)",
	L"11 (City)"
};

// ***********************************************************************************************************************

const TPWString	kaszTrackerEnumStrings_FXDelayAlgorithms[] =
{
	L"None",
	L"AllPass",
	L"Comb"
};

// ***********************************************************************************************************************

const TFontID kiFontID_AudioConsole					= 20;

// ***********************************************************************************************************************

const TInt kiEvents_List_MaxSize					= 250;
const TInt kiMessages_List_MaxSize					= 1000;

// ***********************************************************************************************************************

const TFloat kfMemory_Text_Size						= 0.2f;
const TFloat kfMemory_Bar_Size_X					= 0.2f;
const TFloat kfMemory_Bar_Size_Y					= 0.2f;

const TFloat kfEffects_Text_Size					= 0.2f;

const TFloat kfBuffers_Icon_Position_Y				= 0.24f;
const TFloat kfBuffers_PanBase_Position_X			= 0.80f;
const TFloat kfBuffers_PanArrow_Center_X			= 0.0875;
const TFloat kfBuffers_PanArrow_Center_Y			= 0.315;
const TFloat kfBuffers_PanArrow_Range_X				= 0.0850;
const TFloat kfBuffers_PanArrow_Range_Y				= 0.300f;

const TFloat kfBufferBar_Playback_Position_X		= 0.01f;
const TFloat kfBufferBar_File_Position_X			= 0.40f;
const TFloat kfBufferBar_Position_Y					= 0.75f;
const TFloat kfBufferBar_Size_X						= 0.38f;
const TFloat kfBufferBar_Size_Y						= 0.175f;
const TFloat kfBufferBar_Marker_Size_X				= 0.1f;
const TFloat kfBufferBar_Marker_Size_Y				= 1.0f;
const TFloat kfBufferBar_Marker_Offset_X			= 0.0075f;
const TFloat kfBufferBar_Marker_Offset_Y			= 0.010f;

// ***********************************************************************************************************************

const stTrackerEnum kstTrackerEnum_FXCores				= {-1,  3, kaszTrackerEnumStrings_FXCores};
const stTrackerEnum kstTrackerEnum_FXModes				= { 0, 12, kaszTrackerEnumStrings_FXModes};
const stTrackerEnum kstTrackerEnum_FXDelayAlgorithms	= { 0,  3, kaszTrackerEnumStrings_FXDelayAlgorithms};

// ***********************************************************************************************************************

const stTrackerType kstTrackerType_Enum_FXCores				= {kiTrackerStorage_Integer, kcFontGlyph_None, kcFontGlyph_None, &kstTrackerEnum_FXCores};
const stTrackerType kstTrackerType_Enum_FXModes				= {kiTrackerStorage_Integer, kcFontGlyph_None, kcFontGlyph_None, &kstTrackerEnum_FXModes};
const stTrackerType kstTrackerType_Enum_FXDelayAlgorithms	= {kiTrackerStorage_Integer, kcFontGlyph_None, kcFontGlyph_None, &kstTrackerEnum_FXDelayAlgorithms};

// ***********************************************************************************************************************

const stStringList_EntryType kstEventType_Init		= {0xFF00FF00, kiFlags_StringList_None};
const stStringList_EntryType kstEventType_Shutdown	= {0xFFFF0000, kiFlags_StringList_None};
const stStringList_EntryType kstEventType_Create	= {0xFF80FF00, kiFlags_StringList_None};
const stStringList_EntryType kstEventType_Destroy	= {0xFFFF8000, kiFlags_StringList_None};
const stStringList_EntryType kstEventType_Start		= {0xFF00FF80, kiFlags_StringList_None};
const stStringList_EntryType kstEventType_Stop		= {0xFFFF0080, kiFlags_StringList_None};
const stStringList_EntryType kstEventType_Update	= {0xFF00FFFF, kiFlags_StringList_None};

// ***********************************************************************************************************************

const stStringList_EntryType kstMessageType_Normal	= {0xFFFFFFFF, kiFlags_StringList_None     };
const stStringList_EntryType kstMessageType_Warning	= {0xFFFFFF00, kiFlags_StringList_None     };
const stStringList_EntryType kstMessageType_Error	= {0xFFFF0000, kiFlags_StringList_IsAnError};

// ***********************************************************************************************************************

const TFontGlyph kcFontGlyph_Audio_Icon_Input				= MakeFontGlyph( 65, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Icon_Error				= MakeFontGlyph( 66, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Icon_Disc				= MakeFontGlyph( 67, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Format_PCM				= MakeFontGlyph( 68, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Format_ADPCM				= MakeFontGlyph( 69, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Format_XMA				= MakeFontGlyph( 70, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Fx_No					= MakeFontGlyph( 71, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Fx_A						= MakeFontGlyph( 72, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Fx_B						= MakeFontGlyph( 73, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Type_Static				= MakeFontGlyph( 74, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Type_Stream				= MakeFontGlyph( 75, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Loop_No					= MakeFontGlyph( 76, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Loop_Yes					= MakeFontGlyph( 77, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_State_Play				= MakeFontGlyph( 78, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_State_Stop				= MakeFontGlyph( 79, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_State_Pause				= MakeFontGlyph( 80, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_State_Setup				= MakeFontGlyph( 81, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Pan_Base					= MakeFontGlyph( 82, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Pan_Arrow				= MakeFontGlyph( 83, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Channels_Mono			= MakeFontGlyph( 84, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Channels_Stereo			= MakeFontGlyph( 85, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_Channels_Dolby			= MakeFontGlyph( 86, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_PositionMarker_Load		= MakeFontGlyph( 87, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_PositionMarker_LoopStart	= MakeFontGlyph( 88, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_PositionMarker_LoopEnd	= MakeFontGlyph( 89, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_PositionMarker_Play		= MakeFontGlyph( 90, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_PositionMarker_Decode	= MakeFontGlyph( 91, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_PositionMarker_Chain		= MakeFontGlyph( 92, kiFontID_AudioConsole);
const TFontGlyph kcFontGlyph_Audio_PositionMarker_Cursor	= MakeFontGlyph( 93, kiFontID_AudioConsole);

// ***********************************************************************************************************************

const TFloat kfPanel_Height_Buffers					= 0.15f;

// ***********************************************************************************************************************

const TFloat kfIcon_Audio_StartPosition_Y			= kfIcon_DefaultStartPosition_Y;

// ***********************************************************************************************************************

const TEngineResID kiBigFileKey_Texture_AudioConsole	= 0xa5001712;

// ***********************************************************************************************************************
//    Configuration Options
// ***********************************************************************************************************************

typedef enum
{
	eConfiguration_Title_EventLogging,
	eConfiguration_ActiveEventLog,
	eConfiguration_LogEvent_BufferStop,
	eConfiguration_LogEvent_BufferGetStatus,
	eConfiguration_LogEvent_BufferSetVolume,
	eConfiguration_LogEvent_BufferSetFrequency,
	eConfiguration_LogEvent_BufferSetPan,
	eConfiguration_LogEvent_BufferSetCurrentPos,
	eConfiguration_LogEvent_StreamUpdate,
	eConfiguration_LogEvent_StreamPrefetchStatus,
	eConfiguration_Title_MessageLogging,
	eConfiguration_ActiveMessageLog,
	eConfiguration_Title_Notification,
	eConfiguration_Notify_ErrorMessage,
	eConfiguration_Notify_Loading,

	eConfiguration_Count
}
eConfiguration_Options;

// ***********************************************************************************************************************

#define TITLE	TRUE
#define OPTION	FALSE

const stConfigurationEntry kstConfiguration[eConfiguration_Count] =
{
	{L"Event Logging"							, FALSE, TITLE },
	{L"Active Event Log"						, TRUE , OPTION},
	{L"Log: xeSND_SB_Stop ( )"					, FALSE, OPTION},
	{L"Log: xeSND_SB_GetStatus ( )"				, FALSE, OPTION},
	{L"Log: xeSND_SB_SetVolume ( )"				, FALSE, OPTION},
	{L"Log: xeSND_SB_SetFrequency ( )"			, FALSE, OPTION},
	{L"Log: xeSND_SB_SetPan ( )"				, FALSE, OPTION},
	{L"Log: xeSND_SB_SetCurrentPos ( )"			, FALSE, OPTION},
	{L"Log: xeSND_StreamUpdate ( )"				, FALSE, OPTION},
	{L"Log: xeSND_StreamGetPrefetchStatus ( )"	, FALSE, OPTION},
	{L"Message Logging"							, FALSE, TITLE },
	{L"Active Message Log"						, TRUE , OPTION},
	{L"Notification"							, FALSE, TITLE },
	{L"Notify: Error Message"					, TRUE , OPTION},
	{L"Notify: Error Loading"					, TRUE , OPTION}
};

#undef TITLE
#undef OPTION

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CWidget_Audio_Panel_Buffer : public CWidget_Base_Panel
{
private:
	TWidgetID	m_WID_Type;
	TWidgetID	m_WID_Format;
	TWidgetID	m_WID_Channels;
	TWidgetID	m_WID_Fx;
	TWidgetID	m_WID_State;
	TWidgetID	m_WID_Loop;
	TWidgetID	m_WID_Pan_Base;
	TWidgetID	m_WID_Pan_Arrow;
	TWidgetID	m_WID_Frequency;
	TWidgetID	m_WID_Volume;
	TWidgetID	m_WID_SizeDur_Playback;
	TWidgetID	m_WID_SizeDur_File;
	TWidgetID	m_WID_BufferBar_Playback;
	TWidgetID	m_WID_BufferBar_File;
	TWidgetID	m_WID_PositionMarker_LoadFile;
	TWidgetID	m_WID_PositionMarker_LoadPlayback;
	TWidgetID	m_WID_PositionMarker_LoopStart;
	TWidgetID	m_WID_PositionMarker_LoopEnd;
	TWidgetID	m_WID_PositionMarker_Play;
	TWidgetID	m_WID_PositionMarker_Decode;
	TWidgetID	m_WID_PositionMarker_Chain;
	TWidgetID	m_WID_PositionMarker_CursorFile;
	TWidgetID	m_WID_PositionMarker_CursorPlayback;
	TInt		m_iBufferID;
	TBool		m_bIsUpdateForced;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Audio_Panel_Buffer(TInt Param_iBufferID);
	virtual ~CWidget_Audio_Panel_Buffer();

	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
	virtual void Signal(eNavLink Param_eNavLink = eNavLink_None);
};

// ***********************************************************************************************************************

class CWidget_Audio_Window_Memory : public CWidget_Base_Window
{
private:
	TWidgetID m_WID_Allocated_Name;
	TWidgetID m_WID_Allocated_Line;
	TWidgetID m_WID_General_Allocated;
	TWidgetID m_WID_Static_Allocated;
	TWidgetID m_WID_Stream_Allocated;
	TWidgetID m_WID_Blocks_Name;
	TWidgetID m_WID_Blocks_Line;
	TWidgetID m_WID_General_Blocks;
	TWidgetID m_WID_Static_Blocks;
	TWidgetID m_WID_Stream_Blocks;
	TWidgetID m_WID_Totals_Name;
	TWidgetID m_WID_Totals_Line;
	TWidgetID m_WID_Totals_Allocated;
	TWidgetID m_WID_Totals_Blocks;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Audio_Window_Memory();
	virtual ~CWidget_Audio_Window_Memory();

	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
};

// ***********************************************************************************************************************

class CWidget_Audio_Window_Effects : public CWidget_Base_Window
{
private:
	TWidgetID m_WID_CoreA_Title;
	TWidgetID m_WID_CoreA_Line;
	TWidgetID m_WID_CoreA_Enabled;
	TWidgetID m_WID_CoreA_Mode;
	//TWidgetID m_WID_CoreA_Delay;
	//TWidgetID m_WID_CoreA_Feedback;
	TWidgetID m_WID_CoreA_WetVolume;
	TWidgetID m_WID_CoreA_WetPan;
	//TWidgetID m_WID_CoreA_DelayAlgorithm;
	TWidgetID m_WID_CoreB_Title;
	TWidgetID m_WID_CoreB_Line;
	TWidgetID m_WID_CoreB_Enabled;
	TWidgetID m_WID_CoreB_Mode;
	//TWidgetID m_WID_CoreB_Delay;
	//TWidgetID m_WID_CoreB_Feedback;
	TWidgetID m_WID_CoreB_WetVolume;
	TWidgetID m_WID_CoreB_WetPan;
	//TWidgetID m_WID_CoreB_DelayAlgorithm;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Audio_Window_Effects();
	virtual ~CWidget_Audio_Window_Effects();

	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
};

// ***********************************************************************************************************************

class CWidget_Audio_Window_Buffers : public CWidget_Base_Window_Panels
{
private:
	TWidgetID		m_WID_Info_Count;
	TWidgetID		m_WID_Info_MemoryUsed;
	TEngineResID	m_iWorldID;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Audio_Window_Buffers();
	virtual ~CWidget_Audio_Window_Buffers();

	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
	virtual void Signal(eNavLink Param_eNavLink = eNavLink_None);
};

// ***********************************************************************************************************************

class CWidget_Audio_Window_Events : public CWidget_Base_Window_Message
{
public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Audio_Window_Events();
	virtual ~CWidget_Audio_Window_Events();
};

// ***********************************************************************************************************************

class CWidget_Audio_Window_Messages : public CWidget_Base_Window_Message
{
public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Audio_Window_Messages();
	virtual ~CWidget_Audio_Window_Messages();
};

// ***********************************************************************************************************************

class CWidget_Audio_Window_Prefetch : public CWidget_Base_Window
{
private:
	TWidgetID m_WID_ListSize;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Audio_Window_Prefetch();
	virtual ~CWidget_Audio_Window_Prefetch();

	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
};

// ***********************************************************************************************************************

class CWidget_Audio_Window_Configuration : public CWidget_Base_Window_Configuration
{
public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Audio_Window_Configuration();
	virtual ~CWidget_Audio_Window_Configuration();
};

// ***********************************************************************************************************************

class CWidget_Audio_Window_Settings : public CWidget_Base_Window_Options
{
public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Audio_Window_Settings();
	virtual ~CWidget_Audio_Window_Settings();
};

// ***********************************************************************************************************************

class CAudioConsole
{
private:
	CDesktop*	m_pDesktop;
	TWidgetID	m_WID_Window_Memory;
	TWidgetID	m_WID_Window_Buffers;
	TWidgetID	m_WID_Window_Events;
	TWidgetID	m_WID_Window_Messages;
	TWidgetID	m_WID_Window_Effects;
	TWidgetID	m_WID_Window_Prefetch;
	TWidgetID	m_WID_Window_Settings;
	TWidgetID	m_WID_NotifyIcon_Error;
	TWidgetID	m_WID_NotifyIcon_Disc;

public:
    M_DeclareOperatorNewAndDelete();

	CAudioConsole();
	~CAudioConsole();

	void Initialize();
	void Uninit();
	void Tick(TFloat Param_fSecTimeDelta);
	void Render();

	void	AddEvent(TPWString Param_pString, TPWString Param_pModuleName = NULL, stStringList_EntryType Param_stEntryType = kstStringList_EntryType_Basic);
	void	AddMessage(TPWString Param_pString, TPWString Param_pModuleName = NULL, stStringList_EntryType Param_stEntryType = kstStringList_EntryType_Basic);
	void	AddParameter(TPWString Param_pValue, TPWString Param_pName = NULL, TBool Param_bIsEvent = FALSE);
	void	AddParameter(TInt Param_iValue     , TPWString Param_pName = NULL, TBool Param_bIsEvent = FALSE, TBool Param_bDisplayAsHex = FALSE);
	void	AddParameter(TFloat Param_fValue   , TPWString Param_pName = NULL, TBool Param_bIsEvent = FALSE);
	void	AddParameter(TBool Param_bValue    , TPWString Param_pName = NULL, TBool Param_bIsEvent = FALSE);
	void	WaitForMessageExclusion();
	TBool	GetConfiguration(eConfiguration_Options Param_eOption);
	TListID	AddSetting(TPWString Param_pName, void* Param_pLinkedVariable = NULL, stTrackerType Param_stType = kstTrackerType_None, fnOptionNotify Param_fnOptionNotify = NULL);
	TListID	AddSetting(TPWString Param_pName, TBool* Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify = NULL);
	TListID	AddSetting(TPWString Param_pName, TInt* Param_pLinkedVariable, TInt Param_iMinimum = 0, TInt Param_iMaximum = 0x7FFFFFFF, TInt Param_iDelta = 1, fnOptionNotify_Int Param_fnOptionNotify = NULL);
	TListID	AddSetting(TPWString Param_pName, TFloat* Param_pLinkedVariable, TFloat Param_fMinimum = 0.0f, TFloat Param_fMaximum = 1.0f, TFloat Param_fDelta = 0.01f, fnOptionNotify_Float Param_fnOptionNotify = NULL);
};

// ***********************************************************************************************************************

} // Namespace

using namespace AudioConsole;

// ***********************************************************************************************************************
//    variables
// ***********************************************************************************************************************

extern CAudioConsole g_AudioConsole;

// ***********************************************************************************************************************
//    Prototypes
// ***********************************************************************************************************************

void	AudioConsole_Initialize();
void	AudioConsole_Uninit();
void	AudioConsole_Tick(TFloat Param_fMSecTimeDelta = 0.0f);
void	AudioConsole_Render();

void	AudioConsole_AddEvent  (TPWString Param_pString, TPWString Param_pModuleName = NULL, stStringList_EntryType Param_stEntryType = kstStringList_EntryType_Basic);
void	AudioConsole_AddMessage(TPWString Param_pString, TPWString Param_pModuleName = NULL, stStringList_EntryType Param_stEntryType = kstStringList_EntryType_Basic);
void	AudioConsole_AddEventParameter(TPWString Param_pValue, TPWString Param_pName = NULL);
void	AudioConsole_AddEventParameter(TInt Param_iValue     , TPWString Param_pName = NULL, TBool Param_bDisplayAsHex = FALSE);
void	AudioConsole_AddEventParameter(TFloat Param_fValue   , TPWString Param_pName = NULL);
void	AudioConsole_AddEventParameter(TBool Param_bValue    , TPWString Param_pName = NULL);
void	AudioConsole_AddMessageParameter(TPWString Param_pValue, TPWString Param_pName = NULL);
void	AudioConsole_AddMessageParameter(TInt Param_iValue     , TPWString Param_pName = NULL, TBool Param_bDisplayAsHex = FALSE);
void	AudioConsole_AddMessageParameter(TFloat Param_fValue   , TPWString Param_pName = NULL);
void	AudioConsole_AddMessageParameter(TBool Param_bValue    , TPWString Param_pName = NULL);
void	AudioConsole_WaitForMessageExclusion();

TBool	AudioConsole_GetConfiguration(AudioConsole::eConfiguration_Options Param_eOption);

TListID	AudioConsole_AddSetting(TPWString Param_pName, void*   Param_pLinkedVariable = NULL, stTrackerType Param_stType = kstTrackerType_None, fnOptionNotify Param_fnOptionNotify = NULL);
TListID	AudioConsole_AddSetting(TPWString Param_pName, TBool*  Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify = NULL);
TListID	AudioConsole_AddSetting(TPWString Param_pName, TInt*   Param_pLinkedVariable, TInt Param_iMinimum = 0, TInt Param_iMaximum = 0x7FFFFFFF, TInt Param_iDelta = 1, fnOptionNotify_Int Param_fnOptionNotify = NULL);
TListID	AudioConsole_AddSetting(TPWString Param_pName, TFloat* Param_pLinkedVariable, TFloat Param_fMinimum = 0.0f, TFloat Param_fMaximum = 1.0f, TFloat Param_fDelta = 0.01f, fnOptionNotify_Float Param_fnOptionNotify = NULL);

// ***********************************************************************************************************************

#endif // AUDIOCONSOLE_ENABLE

#endif // __xeSND_AudioConsole_h__

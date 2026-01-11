// ****************************************
// Video Console Debugging Desktop
//
// Used for King Kong Xenon Graphics Debugging
//
// By Alexandre David (January 2005)
// ****************************************

#ifndef __XeVideoConsole_h__
#define __XeVideoConsole_h__

#include "Xenon/Desktop/Desktop.h"

#if defined(VIDEOCONSOLE_ENABLE)

// ***********************************************************************************************************************
//    Forward Declarations
// ***********************************************************************************************************************

namespace VideoConsole
{

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const TPWString kszModuleName_XenonGraphics			= L"XeGra";

// ***********************************************************************************************************************

const TFontID kiFontID_VideoConsole					= 50;

// ***********************************************************************************************************************

const TInt kiMessages_List_MaxSize					= 1000;

// ***********************************************************************************************************************

const stStringList_EntryType kstMessageType_Normal	= {0xFFFFFFFF, kiFlags_StringList_None     };
const stStringList_EntryType kstMessageType_Warning	= {0xFFFFFF00, kiFlags_StringList_None     };
const stStringList_EntryType kstMessageType_Error	= {0xFFFF0000, kiFlags_StringList_IsAnError};

// ***********************************************************************************************************************

const TFontGlyph kcFontGlyph_Video_Icon_Input		= MakeFontGlyph( 65, kiFontID_VideoConsole);
const TFontGlyph kcFontGlyph_Video_Icon_Error		= MakeFontGlyph( 66, kiFontID_VideoConsole);

// ***********************************************************************************************************************

const TFloat kfIcon_Video_StartPosition_Y			= 0.3f;

// ***********************************************************************************************************************

const TFloat kfZoomWindow_Size_X = 0.8f;
const TFloat kfZoomWindow_Size_Y = 0.8f;

// ***********************************************************************************************************************

const TFloat kfPanel_Height_TextureViewer			= 0.15f;

// ***********************************************************************************************************************

const TEngineResID kiBigFileKey_Texture_VideoConsole	= 0xa5001718;

const TPWString	kaszTrackerEnumStrings_ObjectType[] =
{
    L"None",
    L"Compute All GRO",
    L"ODE",
    L"Collision Map",
    L"SnP",
    L"Animated Objects",
    L"Collision Mesh"
};

const TPWString	kaszTrackerEnumStrings_Sectorisation[] =
{
    L"Current View",
    L"This Sector",
    L"All Visible Sectors"
};

const TPWString	kaszTrackerEnumStrings_Sorting[] =
{
    L"Poly Count",
    L"Key",
    L"Instances",
    L"Poly * Instances",
    L"PolyCount (Instance only)",
    L"Key (Instance only)",
    L"Instance (Instance only)",
    L"Poly * Instance (Instance only)",
    L"Poly (Single only)",
    L"Key (Single only)"
};

const stTrackerEnum kstTrackerEnum_ObjectType			= {0,  7, kaszTrackerEnumStrings_ObjectType};
const stTrackerEnum kstTrackerEnum_Sectorisation		= {0,  3, kaszTrackerEnumStrings_Sectorisation};
const stTrackerEnum kstTrackerEnum_Sorting				= {0,  10, kaszTrackerEnumStrings_Sorting};

const stTrackerType kstTrackerType_Enum_ObjectType		= {kiTrackerStorage_Integer, kcFontGlyph_None, kcFontGlyph_None, &kstTrackerEnum_ObjectType};
const stTrackerType kstTrackerType_Enum_Sectorisation	= {kiTrackerStorage_Integer, kcFontGlyph_None, kcFontGlyph_None, &kstTrackerEnum_Sectorisation};
const stTrackerType kstTrackerType_Enum_Sorting			= {kiTrackerStorage_Integer, kcFontGlyph_None, kcFontGlyph_None, &kstTrackerEnum_Sorting};

// ***********************************************************************************************************************
//    Configuration Options
// ***********************************************************************************************************************

typedef enum
{
	eConfiguration_Title_Notification,
	eConfiguration_Notify_ErrorMessage,
	eConfiguration_Title_MessageLogging,
	eConfiguration_ActiveMessageLog,
	eConfiguration_Title_DebugDisplay,
	eConfiguration_Display_HideFrameRate,
	eConfiguration_Display_HideFrameCount,
	eConfiguration_Display_HideObjectCount,
	eConfiguration_Display_HideTriangleCount,
	eConfiguration_Display_HideMemory,
	eConfiguration_Display_HideVersionInfo,

	eConfiguration_Count
}
eConfiguration_Options;

// ***********************************************************************************************************************

#define TITLE	TRUE
#define OPTION	FALSE

const stConfigurationEntry kstConfiguration[eConfiguration_Count] =
{
	{L"Notification"			, FALSE, TITLE },
	{L"Notify: Error Message"	, TRUE , OPTION},
	{L"Message Logging"			, FALSE, TITLE },
	{L"Active Message Log"		, TRUE , OPTION},
	{L"Debug Display"			, FALSE, TITLE },
	{L"Hide Frame Rate"			, FALSE, OPTION},
	{L"Hide Frame Count"		, FALSE, OPTION},
	{L"Hide Object Count"		, FALSE, OPTION},
	{L"Hide Triangle Count"		, FALSE, OPTION},
	{L"Hide Memory"				, FALSE, OPTION},
	{L"Hide Version"			, FALSE, OPTION}
};

#undef TITLE
#undef OPTION

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CWidget_Video_Panel_Texture : public CWidget_Base_Panel
{
private:
	TWidgetID m_WID_Thumbnail;
	TWidgetID m_WID_Size;
	TWidgetID m_WID_SizeBar;
	TWidgetID m_WID_Key;
	TWidgetID m_WID_Format;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Video_Panel_Texture(TInt Param_iTextureID, TInt Param_iTextureCount, TInt Param_iWidth, TInt Param_iHeight, TEngineResID Param_iResourceID);
	virtual ~CWidget_Video_Panel_Texture();

	TInt GetTextureID();
};

// ***********************************************************************************************************************

class CWidget_Video_Window_Statistics : public CWidget_Base_Window
{
private:
	TWidgetID m_WID_Statistics;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Video_Window_Statistics();
	virtual ~CWidget_Video_Window_Statistics();

	void UpdateString(TPWString Param_pString);
};

// ***********************************************************************************************************************

class CWidget_Video_Window_AdditonalStatistics: public CWidget_Base_Window_Message
{
public:
    M_DeclareOperatorNewAndDelete();

    CWidget_Video_Window_AdditonalStatistics();
    virtual ~CWidget_Video_Window_AdditonalStatistics();
};

// ***********************************************************************************************************************

class CWidget_Video_Window_AdditonalStatisticsOptions: public CWidget_Base_Window_Options
{
public:
    M_DeclareOperatorNewAndDelete();

    CWidget_Video_Window_AdditonalStatisticsOptions();
    virtual ~CWidget_Video_Window_AdditonalStatisticsOptions();
};

// ***********************************************************************************************************************

class CWidget_Video_Window_Messages : public CWidget_Base_Window_Message
{
public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Video_Window_Messages();
	virtual ~CWidget_Video_Window_Messages();
};

// ***********************************************************************************************************************

class CWidget_Video_Window_DTChart : public CWidget_Base_Window_Chart
{
private:
	TWidgetID m_WID_Line_60FPS;
	TWidgetID m_WID_Line_30FPS;
	TWidgetID m_WID_Text_60FPS;
	TWidgetID m_WID_Text_30FPS;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Video_Window_DTChart();
	virtual ~CWidget_Video_Window_DTChart();

	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
};

// ***********************************************************************************************************************

class CWidget_Video_Window_RasterOptions : public CWidget_Base_Window_Options
{
public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Video_Window_RasterOptions();
	virtual ~CWidget_Video_Window_RasterOptions();
};

// ***********************************************************************************************************************

class CWidget_Video_Window_TextureViewer : public CWidget_Base_Window_Panels
{
private:
	TWidgetID	m_WID_Zoom_Frame;
	TWidgetID	m_WID_Zoom_Texture;
	TWidgetID	m_WID_Info_DisplayedCount;
	TWidgetID	m_WID_Info_MemoryUsed;

	TBool		m_bIsZoomWindowActive;
	TBool		m_bIsBackgroundTransparent;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Video_Window_TextureViewer();
	virtual ~CWidget_Video_Window_TextureViewer();

	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
	virtual void Signal(eNavLink Param_eNavLink = eNavLink_None);

	void Populate();
	void ZoomWindow_Show(TBool Param_bForcedUpdate = FALSE);
	void ZoomWindow_Hide();
};

// ***********************************************************************************************************************

class CWidget_Video_Window_Configuration : public CWidget_Base_Window_Configuration
{
public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Video_Window_Configuration();
	virtual ~CWidget_Video_Window_Configuration();
};

// ***********************************************************************************************************************

class CVideoConsole
{
private:
	CDesktop*	m_pDesktop;
	TWidgetID	m_WID_Window_Statistics;
	TWidgetID	m_WID_Window_Messages;
	TWidgetID	m_WID_Window_DTChart;
	TWidgetID	m_WID_Window_RasterOptions;
	TWidgetID	m_WID_Window_TextureViewer;
	TWidgetID	m_WID_NotifyIcon_Error;
    TWidgetID	m_WID_Window_AdditionalStatistics;
    TWidgetID	m_WID_Window_AdditionalStatisticsOptions;

public:
    M_DeclareOperatorNewAndDelete();

	CVideoConsole();
	~CVideoConsole();

	void	Initialize();
	void	Uninit();
	void	Tick(TFloat Param_fSecTimeDelta);
	void	Render();

	void	UpdateStatistics(TPWString Param_pString);
	void	AddMessage(TPWString Param_pString, TPWString Param_pModuleName = NULL, stStringList_EntryType Param_stEntryType = kstStringList_EntryType_Basic);
	void	AddParameter(TPWString Param_pValue, TPWString Param_pName = NULL);
	void	AddParameter(TInt      Param_iValue, TPWString Param_pName = NULL, TBool Param_bDisplayAsHex = FALSE);
	void	AddParameter(TFloat    Param_fValue, TPWString Param_pName = NULL);
	void	AddParameter(TBool     Param_bValue, TPWString Param_pName = NULL);
	void	WaitForMessageExclusion();
	TBool	GetConfiguration(eConfiguration_Options Param_eOption);
	TListID	AddRasterOption(TPWString Param_pName, void* Param_pLinkedVariable = NULL, stTrackerType Param_stType = kstTrackerType_None, fnOptionNotify Param_fnOptionNotify = NULL);
	TListID	AddRasterOption(TPWString Param_pName, TBool* Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify = NULL);
	TListID	AddRasterOption(TPWString Param_pName, TInt* Param_pLinkedVariable, TInt Param_iMinimum = 0, TInt Param_iMaximum = 0x7FFFFFFF, TInt Param_iDelta = 1, fnOptionNotify_Int Param_fnOptionNotify = NULL);
	TListID	AddRasterOption(TPWString Param_pName, TFloat* Param_pLinkedVariable, TFloat Param_fMinimum = 0.0f, TFloat Param_fMaximum = 1.0f, TFloat Param_fDelta = 0.01f, fnOptionNotify_Float Param_fnOptionNotify = NULL);

    TListID	AddAdditionalStatisticsOption(TPWString Param_pName, void* Param_pLinkedVariable = NULL, stTrackerType Param_stType = kstTrackerType_None, fnOptionNotify Param_fnOptionNotify = NULL);
    TListID	AddAdditionalStatisticsOption(TPWString Param_pName, TBool* Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify = NULL);    
    void	ClearStatistics();
    void	AddStatistics(TPWString Param_pString, TPWString Param_pModuleName = NULL, stStringList_EntryType Param_stEntryType = kstStringList_EntryType_Basic);

	void	UpdateLinkedVariable(TListID Param_iOptionID, TFloat* Param_pLinkedVariable);
    void	UpdateLinkedVariable(TListID Param_iOptionID, TBool* Param_pLinkedVariable);

	//void	choucroutte();
};

} // Namespace

using namespace VideoConsole;

// ***********************************************************************************************************************
//    variables
// ***********************************************************************************************************************

extern CVideoConsole g_VideoConsole;

// ***********************************************************************************************************************
//    Prototypes
// ***********************************************************************************************************************

void	VideoConsole_Initialize();
void	VideoConsole_Uninit();
void	VideoConsole_Tick(TFloat Param_fMSecTimeDelta = 0.0f);
void	VideoConsole_Render();

void	VideoConsole_AddMessage  (TPWString Param_pString, TPWString Param_pModuleName = NULL, stStringList_EntryType Param_stEntryType = kstStringList_EntryType_Basic);
void	VideoConsole_AddParameter(TPWString Param_pValue, TPWString Param_pName = NULL);
void	VideoConsole_AddParameter(TInt      Param_iValue, TPWString Param_pName = NULL, TBool Param_bDisplayAsHex = FALSE);
void	VideoConsole_AddParameter(TFloat    Param_fValue, TPWString Param_pName = NULL);
void	VideoConsole_AddParameter(TBool     Param_bValue, TPWString Param_pName = NULL);
void	VideoConsole_WaitForMessageExclusion();

void	VideoConsole_UpdateStatistics(TPWString Param_pString = NULL);
TBool	VideoConsole_GetConfiguration(VideoConsole::eConfiguration_Options Param_eOption);

TListID	VideoConsole_AddRasterOption(TPWString Param_pName, void* Param_pLinkedVariable = NULL, stTrackerType Param_stType = kstTrackerType_None, fnOptionNotify Param_fnOptionNotify = NULL);
TListID	VideoConsole_AddRasterOption(TPWString Param_pName, TBool* Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify = NULL);
TListID	VideoConsole_AddRasterOption(TPWString Param_pName, TInt* Param_pLinkedVariable, TInt Param_iMinimum = 0, TInt Param_iMaximum = 0x7FFFFFFF, TInt Param_iDelta = 1, fnOptionNotify_Int Param_fnOptionNotify = NULL);
TListID	VideoConsole_AddRasterOption(TPWString Param_pName, TFloat* Param_pLinkedVariable, TFloat Param_fMinimum = 0.0f, TFloat Param_fMaximum = 1.0f, TFloat Param_fDelta = 0.01f, fnOptionNotify_Float Param_fnOptionNotify = NULL);

TListID	VideoConsole_AddAdditionalStatisticsOption(TPWString Param_pName, void* Param_pLinkedVariable = NULL, stTrackerType Param_stType = kstTrackerType_None, fnOptionNotify Param_fnOptionNotify = NULL);
TListID	VideoConsole_AddAdditionalStatisticsOption(TPWString Param_pName, TBool* Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify = NULL);
void	VideoConsole_ClearStatistics();
void	VideoConsole_AddStatistics(TPWString Param_pString, TPWString Param_pModuleName = NULL, stStringList_EntryType Param_stEntryType = kstStringList_EntryType_Basic);

void	VideoConsole_UpdateLinkedVariable(TListID Param_iOptionID, TFloat* Param_pLinkedVariable);
void	VideoConsole_UpdateLinkedVariable(TListID Param_iOptionID, TBool* Param_pLinkedVariable);

// ***********************************************************************************************************************

#endif // VIDEOCONSOLE_ENABLE

#endif // __XeVideoConsole_h__

// ****************************************
// Engine Console Debugging Desktop
//
// Used for King Kong Xenon General Debugging
//
// By Alexandre David (June 2005)
// ****************************************

#ifndef __EngineConsole_h__
#define __EngineConsole_h__

#include "Xenon/Desktop/Desktop.h"

#if defined(ENGINECONSOLE_ENABLE)

// ***********************************************************************************************************************
//    Forward Declarations
// ***********************************************************************************************************************

namespace EngineConsole
{

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const TPWString kszModuleName_Engine					= L"Eng";

// ***********************************************************************************************************************

const TFontID kiFontID_EngineConsole					= 60;

// ***********************************************************************************************************************

const TInt kiMessages_List_MaxSize						= 1000;

// ***********************************************************************************************************************

const stStringList_EntryType kstMessageType_Normal		= {0xFFFFFFFF, kiFlags_StringList_None     };
const stStringList_EntryType kstMessageType_Warning		= {0xFFFFFF00, kiFlags_StringList_None     };
const stStringList_EntryType kstMessageType_Error		= {0xFFFF0000, kiFlags_StringList_IsAnError};

// ***********************************************************************************************************************

const TFontGlyph kcFontGlyph_Engine_Icon_Input			= MakeFontGlyph( 65, kiFontID_EngineConsole);
const TFontGlyph kcFontGlyph_Engine_Icon_Error			= MakeFontGlyph( 66, kiFontID_EngineConsole);

// ***********************************************************************************************************************

const TFloat kfIcon_Engine_StartPosition_Y				= 0.6f;

// ***********************************************************************************************************************

const TEngineResID kiBigFileKey_Texture_EngineConsole	= 0xa500201b;

// ***********************************************************************************************************************
//    Configuration Options
// ***********************************************************************************************************************

typedef enum
{
	eConfiguration_Title_Notification,
	eConfiguration_Notify_ErrorMessage,
	eConfiguration_Title_MessageLogging,
	eConfiguration_ActiveMessageLog,

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
};

#undef TITLE
#undef OPTION

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CWidget_Engine_Window_Version : public CWidget_Base_Window
{
private:
	TWidgetID m_WID_Version;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Engine_Window_Version();
	virtual ~CWidget_Engine_Window_Version();
};

// ***********************************************************************************************************************

class CWidget_Engine_Window_Messages : public CWidget_Base_Window_Message
{
public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Engine_Window_Messages();
	virtual ~CWidget_Engine_Window_Messages();
};

// ***********************************************************************************************************************

class CWidget_Engine_Window_Configuration : public CWidget_Base_Window_Configuration
{
public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Engine_Window_Configuration();
	virtual ~CWidget_Engine_Window_Configuration();
};

// ***********************************************************************************************************************

class CEngineConsole
{
private:
	CDesktop*	m_pDesktop;
	TWidgetID	m_WID_Window_Version;
	TWidgetID	m_WID_Window_Messages;
	TWidgetID	m_WID_NotifyIcon_Error;

public:
    M_DeclareOperatorNewAndDelete();

	CEngineConsole();
	~CEngineConsole();

	void	Initialize();
	void	Uninit();
	void	Tick(TFloat Param_fSecTimeDelta);
	void	Render();

	void	AddMessage(TPWString Param_pString, TPWString Param_pModuleName = NULL, stStringList_EntryType Param_stEntryType = kstStringList_EntryType_Basic);
	void	AddParameter(TPWString Param_pValue, TPWString Param_pName = NULL);
	void	AddParameter(TInt      Param_iValue, TPWString Param_pName = NULL, TBool Param_bDisplayAsHex = FALSE);
	void	AddParameter(TFloat    Param_fValue, TPWString Param_pName = NULL);
	void	AddParameter(TBool     Param_bValue, TPWString Param_pName = NULL);
	void	WaitForMessageExclusion();
	TBool	GetConfiguration(EngineConsole::eConfiguration_Options Param_eOption);
};

} // Namespace

using namespace EngineConsole;

// ***********************************************************************************************************************
//    variables
// ***********************************************************************************************************************

extern CEngineConsole g_EngineConsole;

// ***********************************************************************************************************************
//    Prototypes
// ***********************************************************************************************************************

void	EngineConsole_Initialize();
void	EngineConsole_Uninit();
void	EngineConsole_Tick(TFloat Param_fMSecTimeDelta = 0.0f);
void	EngineConsole_Render();

void	EngineConsole_AddMessage  (TPWString Param_pString, TPWString Param_pModuleName = NULL, stStringList_EntryType Param_stEntryType = kstStringList_EntryType_Basic);
void	EngineConsole_AddParameter(TPWString Param_pValue, TPWString Param_pName = NULL);
void	EngineConsole_AddParameter(TInt      Param_iValue, TPWString Param_pName = NULL, TBool Param_bDisplayAsHex = FALSE);
void	EngineConsole_AddParameter(TFloat    Param_fValue, TPWString Param_pName = NULL);
void	EngineConsole_AddParameter(TBool     Param_bValue, TPWString Param_pName = NULL);
void	EngineConsole_WaitForMessageExclusion();

TBool	EngineConsole_GetConfiguration(EngineConsole::eConfiguration_Options Param_eOption);

// ***********************************************************************************************************************

#endif // ENGINECONSOLE_ENABLE

#endif // __EngineConsole_h__

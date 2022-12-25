// ****************************************
// Engine Console Debugging Desktop
//
// Used for King Kong Xenon General Debugging
//
// By Alexandre David (January 2005)
// ****************************************

#include "Precomp.h"

#include "EngineConsole.h"

#if defined(ENGINECONSOLE_ENABLE)

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "BIGfiles/VERsion/VERsion_Number.h"

// ***********************************************************************************************************************
//    Prototypes
// ***********************************************************************************************************************

void	EngineConsole_Initialize()						{g_EngineConsole.Initialize();}
void	EngineConsole_Uninit()							{g_EngineConsole.Uninit();}
void	EngineConsole_Tick(TFloat Param_fMSecTimeDelta)	{g_EngineConsole.Tick(Param_fMSecTimeDelta);}
void	EngineConsole_Render()							{g_EngineConsole.Render();}

void	EngineConsole_AddMessage  (TPWString Param_pString, TPWString Param_pModuleName, stStringList_EntryType Param_stEntryType)	{g_EngineConsole.AddMessage(Param_pString, Param_pModuleName, Param_stEntryType);}
void	EngineConsole_AddParameter(TPWString Param_pValue, TPWString Param_pName)							{g_EngineConsole.AddParameter(Param_pValue, Param_pName);}
void	EngineConsole_AddParameter(TInt      Param_iValue, TPWString Param_pName, TBool Param_bDisplayAsHex)	{g_EngineConsole.AddParameter(Param_iValue, Param_pName, Param_bDisplayAsHex);}
void	EngineConsole_AddParameter(TFloat    Param_fValue, TPWString Param_pName)							{g_EngineConsole.AddParameter(Param_fValue, Param_pName);}
void	EngineConsole_AddParameter(TBool     Param_bValue, TPWString Param_pName)							{g_EngineConsole.AddParameter(Param_bValue, Param_pName);}
void	EngineConsole_WaitForMessageExclusion()	{g_EngineConsole.WaitForMessageExclusion();}

TBool	EngineConsole_GetConfiguration(eConfiguration_Options Param_eOption)	{return g_EngineConsole.GetConfiguration(Param_eOption);}

// ***********************************************************************************************************************
//    Private Variables
// ***********************************************************************************************************************

CEngineConsole g_EngineConsole;

// ***********************************************************************************************************************
//    Class Methods : CWidget_Engine_Window_Version
// ***********************************************************************************************************************

CWidget_Engine_Window_Version::CWidget_Engine_Window_Version() : CWidget_Base_Window(L"Version")
{
    TWChar acVersionString[1024];

	swprintf(acVersionString, L"Version %i\n----------------\n", BIG_Cu4_MontrealXeAppVersion);

	wcscat(acVersionString, L"Preprocessor Defines :\n \n");

#if defined(_DEBUG)
    wcscat(acVersionString, L"<+ _DEBUG +> ");
#else
    wcscat(acVersionString, L"(- _DEBUG -) ");
#endif

#if defined(NDEBUG)
    wcscat(acVersionString, L"<+ NDEBUG +> ");
#else
    wcscat(acVersionString, L"(- NDEBUG -) ");
#endif

#if defined(RELEASE)
    wcscat(acVersionString, L"<+ RELEASE +> ");
#else
    wcscat(acVersionString, L"(- RELEASE -) ");
#endif

#if defined(PROFILE_ENABLE)
    wcscat(acVersionString, L"<+ PROFILE_ENABLE +> ");
#else
    wcscat(acVersionString, L"(- PROFILE_ENABLE -) ");
#endif

#if defined(_FINAL_)
    wcscat(acVersionString, L"<+ _FINAL_ +> ");
#else
    wcscat(acVersionString, L"(- _FINAL_ -) ");
#endif

    wcscat(acVersionString, L"\n \n");

#if defined(_XENON)
    wcscat(acVersionString, L"<+ _XENON +> ");
#else
    wcscat(acVersionString, L"(- _XENON -) ");
#endif

#if defined(_XENON_RENDER)
    wcscat(acVersionString, L"<+ _XENON_RENDER +> ");
#else
    wcscat(acVersionString, L"(- _XENON_RENDER -) ");
#endif

#if defined(_XENON_RENDERER_USETHREAD)
    wcscat(acVersionString, L"<+ _XENON_USETHREAD +> ");
#else
    wcscat(acVersionString, L"(- _XENON_USETHREAD -) ");
#endif

    wcscat(acVersionString, L"\n");

#if defined(_XENON_PROFILE)
    wcscat(acVersionString, L"<+ _XENON_PROFILE +> ");
#else
    wcscat(acVersionString, L"(- _XENON_PROFILE -) ");
#endif

    wcscat(acVersionString, L"\n \n");

#if defined(_XE_COMPRESS_VERTEX)
    wcscat(acVersionString, L"<+ _XE_COMPRESS_VERTEX +> ");
#else
    wcscat(acVersionString, L"(- _XE_COMPRESS_VERTEX -) ");
#endif

#if defined(_XE_COMPRESS_NORMALS)
    wcscat(acVersionString, L"<+ _XE_COMPRESS_NORMALS +> ");
#else
    wcscat(acVersionString, L"(- _XE_COMPRESS_NORMALS -) ");
#endif

    wcscat(acVersionString, L"\n");

#if defined(_XE_COMPRESS_TANGENTS)
    wcscat(acVersionString, L"<+ _XE_COMPRESS_TANGENTS +> ");
#else
    wcscat(acVersionString, L"(- _XE_COMPRESS_TANGENTS -) ");
#endif

#if defined(_XE_COMPRESS_WEIGHTS)
    wcscat(acVersionString, L"<+ _XE_COMPRESS_WEIGHTS +> ");
#else
    wcscat(acVersionString, L"(- _XE_COMPRESS_WEIGHTS -) ");
#endif

    wcscat(acVersionString, L"\n");

#if defined(_XENON_PACK_MESH)
	wcscat(acVersionString, L"<+ _XENON_PACK_MESH +> ");
#else
    wcscat(acVersionString, L"(- _XENON_PACK_MESH -) ");
#endif

    wcscat(acVersionString, L"\n \n");

#if defined(DESKTOP_ENABLE)
	wcscat(acVersionString, L"<+ DESKTOP_ENABLE +> ");
#else
    wcscat(acVersionString, L"(- DESKTOP_ENABLE -) ");
#endif

#if defined(AUDIOCONSOLE_ENABLE)
	wcscat(acVersionString, L"<+ AUDIOCONSOLE_ENABLE +> ");
#else
    wcscat(acVersionString, L"(- AUDIOCONSOLE_ENABLE -) ");
#endif

    wcscat(acVersionString, L"\n");

#if defined(VIDEOCONSOLE_ENABLE)
	wcscat(acVersionString, L"<+ VIDEOCONSOLE_ENABLE +> ");
#else
    wcscat(acVersionString, L"(- VIDEOCONSOLE_ENABLE -) ");
#endif

#if defined(ENGINECONSOLE_ENABLE)
	wcscat(acVersionString, L"<+ ENGINECONSOLE_ENABLE +> ");
#else
    wcscat(acVersionString, L"(- ENGINECONSOLE_ENABLE -) ");
#endif

    wcscat(acVersionString, L"\n \n");

#if _XENON_DEMO_INACTIVITYEXIT
	wcscat(acVersionString, L"<+ _XENON_DEMO_INACTIVITYEXIT +> ");
#else
	wcscat(acVersionString, L"(- _XENON_DEMO_INACTIVITYEXIT -) ");
#endif

#if _XENON_DEMO_HIDEFPS
	wcscat(acVersionString, L"<+ _XENON_DEMO_HIDEFPS +> ");
#else
	wcscat(acVersionString, L"(- _XENON_DEMO_HIDEFPS -) ");
#endif

    wcscat(acVersionString, L"\n");

#if _XENON_DEMO_HIDEVERSION
	wcscat(acVersionString, L"<+ _XENON_DEMO_HIDEVERSION +> ");
#else
	wcscat(acVersionString, L"(- _XENON_DEMO_HIDEVERSION -) ");
#endif

	wcscat(acVersionString, L"\n");

#if _XENON_DEMO_NOSTORAGEACCESS
	wcscat(acVersionString, L"<+ _XENON_DEMO_NOSTORAGEACCESS +> ");
#else
	wcscat(acVersionString, L"(- _XENON_DEMO_NOSTORAGEACCESS -) ");
#endif

#if _XENON_DEMO_NOACHIEVEMENTS
	wcscat(acVersionString, L"<+ _XENON_DEMO_NOACHIEVEMENTS +> ");
#else
	wcscat(acVersionString, L"(- _XENON_DEMO_NOACHIEVEMENTS -) ");
#endif

    wcscat(acVersionString, L"\n \n");

#if _XENON_SOUND_ENGINE
	wcscat(acVersionString, L"<+ _XENON_SOUND_ENGINE +> ");
#else
	wcscat(acVersionString, L"(- _XENON_SOUND_ENGINE -) ");
#endif

#if _XENON_SOUND_FX
	wcscat(acVersionString, L"<+ _XENON_SOUND_FX +> ");
#else
	wcscat(acVersionString, L"(- _XENON_SOUND_FX -) ");
#endif

    wcscat(acVersionString, L"\n \n");

#if _XENON_THREAD_ALLOWSETNAME
	wcscat(acVersionString, L"<+ _XENON_THREAD_ALLOWSETNAME +> ");
#else
	wcscat(acVersionString, L"(- _XENON_THREAD_ALLOWSETNAME -) ");
#endif

#if _XENON_DEBUG_SAFEMODE
	wcscat(acVersionString, L"<+ _XENON_DEBUG_SAFEMODE +> ");
#else
	wcscat(acVersionString, L"(- _XENON_DEBUG_SAFEMODE -) ");
#endif

	// Create SubWidget - Version Display Text

	CWidget_Base_Text* pWidget_Text = new CWidget_Base_Text(acVersionString);

	pWidget_Text->Get_Position()->Set(0.01f, 0.05f);
	pWidget_Text->Get_TextSize()->Set(0.15f);

	m_WID_Version = AddSubWidget(pWidget_Text);
}

// ***********************************************************************************************************************

CWidget_Engine_Window_Version::~CWidget_Engine_Window_Version()
{
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Engine_Window_Messages
// ***********************************************************************************************************************

CWidget_Engine_Window_Messages::CWidget_Engine_Window_Messages() : CWidget_Base_Window_Message(L"Messages", kiMessages_List_MaxSize)
{
}

// ***********************************************************************************************************************

CWidget_Engine_Window_Messages::~CWidget_Engine_Window_Messages()
{
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Engine_Window_Configuration
// ***********************************************************************************************************************

CWidget_Engine_Window_Configuration::CWidget_Engine_Window_Configuration() : CWidget_Base_Window_Configuration(eConfiguration_Count, (stConfigurationEntry*)kstConfiguration)
{
}

// ***********************************************************************************************************************

CWidget_Engine_Window_Configuration::~CWidget_Engine_Window_Configuration()
{
}

// ***********************************************************************************************************************
//    Class Methods : CEngineConsole
// ***********************************************************************************************************************

CEngineConsole::CEngineConsole()
{
	m_pDesktop					= NULL;

	m_WID_Window_Version		= kiWidgetID_Invalid;
	m_WID_Window_Messages		= kiWidgetID_Invalid;

	m_WID_NotifyIcon_Error		= kiWidgetID_Invalid;
}

// ***********************************************************************************************************************

CEngineConsole::~CEngineConsole()
{
	Uninit();
}

// ***********************************************************************************************************************

void CEngineConsole::Initialize()
{
	// Ensure Non-Initialization

	if (m_pDesktop != NULL)
	{
		return;
	}

	// Create Desktop

	m_pDesktop = new CDesktop(L"Engine Console", kiColor_Desktop_Theme_Red);

	// Create Additional Fonts

	g_FontManager.CreateFont(kiFontID_EngineConsole, kiBigFileKey_Texture_EngineConsole, kiBigFileKey_Texture_EngineConsole, 1.0f, 1.0f);

	// Create Windows

	m_WID_Window_Version	= m_pDesktop->AddWindow(new CWidget_Engine_Window_Version());
    m_WID_Window_Messages	= m_pDesktop->AddWindow(new CWidget_Engine_Window_Messages());

	m_pDesktop->AddConfigurationWindow(new CWidget_Engine_Window_Configuration());

	// Setup Desktop Parameters

	m_pDesktop->SetInputMonitoringToggleButtons(eButton_Thumb_Right, eButton_DPad_Left);
	m_pDesktop->SetupIconBar(kfIcon_Engine_StartPosition_Y, kcFontGlyph_Engine_Icon_Input);

	// Create Notification Icons

	m_WID_NotifyIcon_Error = m_pDesktop->AddNotifyIcon(new CWidget_Base_NotifyIcon(kcFontGlyph_Engine_Icon_Error, 5.0f, 1.0f));
}

// ***********************************************************************************************************************

void CEngineConsole::Uninit()
{
	if (m_pDesktop != NULL)
	{
		delete m_pDesktop;
		m_pDesktop = NULL;
	}
}

// ***********************************************************************************************************************

void CEngineConsole::Tick(TFloat Param_fSecTimeDelta)
{
	Profile_Begin("EngineConsole.Tick");

	if (m_pDesktop != NULL)
	{
		m_pDesktop->Tick(Param_fSecTimeDelta);
	}

	Profile_End();
}

// ***********************************************************************************************************************

void CEngineConsole::Render()
{
	Profile_Begin("EngineConsole.Render");

	if (m_pDesktop != NULL)
	{
		m_pDesktop->Render();
	}

	Profile_End();
}

// ***********************************************************************************************************************

void CEngineConsole::AddMessage(TPWString Param_pString, TPWString Param_pModuleName, stStringList_EntryType Param_stEntryType)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		((CWidget_Engine_Window_Messages*)m_pDesktop->GetWindow(m_WID_Window_Messages))->AddMessage(m_pDesktop->GetMessageMaker()->Make(Param_pString, Param_pModuleName), Param_stEntryType);

		// Error Notification

		if ((Param_stEntryType.iFlags_StringList & kiFlags_StringList_IsAnError) && GetConfiguration(eConfiguration_Notify_ErrorMessage))
		{
			m_pDesktop->ActivateNotifyIcon(m_WID_NotifyIcon_Error);
		}
	}
}

// ***********************************************************************************************************************

void CEngineConsole::AddParameter(TPWString Param_pValue, TPWString Param_pName)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		m_pDesktop->GetMessageMaker()->AddParameter(Param_pValue, Param_pName);
	}
}

// ***********************************************************************************************************************

void CEngineConsole::AddParameter(TInt Param_iValue, TPWString Param_pName, TBool Param_bDisplayAsHex)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		m_pDesktop->GetMessageMaker()->AddParameter(Param_iValue, Param_pName, Param_bDisplayAsHex);
	}
}

// ***********************************************************************************************************************

void CEngineConsole::AddParameter(TFloat Param_fValue, TPWString Param_pName)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		m_pDesktop->GetMessageMaker()->AddParameter(Param_fValue, Param_pName);
	}
}

// ***********************************************************************************************************************

void CEngineConsole::AddParameter(TBool Param_bValue, TPWString Param_pName)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		m_pDesktop->GetMessageMaker()->AddParameter(Param_bValue, Param_pName);
	}
}

// ***********************************************************************************************************************

void CEngineConsole::WaitForMessageExclusion()
{
	if (m_pDesktop != NULL)
	{
		m_pDesktop->WaitForMessageExclusion();
	}
}

// ***********************************************************************************************************************

TBool CEngineConsole::GetConfiguration(eConfiguration_Options Param_eOption)
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

#endif // ENGINECONSOLE_ENABLE

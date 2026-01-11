// ****************************************
// Video Console Debugging Desktop
//
// Used for King Kong Xenon Graphics Debugging
//
// By Alexandre David (January 2005)
// ****************************************

#include "Precomp.h"

#include "XeVideoConsole.h"

#include "Xenon/MenuManager/MenuManager.h"

#if defined(VIDEOCONSOLE_ENABLE)

// ***********************************************************************************************************************
//    Prototypes
// ***********************************************************************************************************************

void	VideoConsole_Initialize()						{g_VideoConsole.Initialize();}
void	VideoConsole_Uninit()							{g_VideoConsole.Uninit();}
void	VideoConsole_Tick(TFloat Param_fMSecTimeDelta)	{g_VideoConsole.Tick(Param_fMSecTimeDelta);}
void	VideoConsole_Render()							{g_VideoConsole.Render();}

void	VideoConsole_AddMessage  (TPWString Param_pString, TPWString Param_pModuleName, stStringList_EntryType Param_stEntryType)	{g_VideoConsole.AddMessage(Param_pString, Param_pModuleName, Param_stEntryType);}
void	VideoConsole_AddParameter(TPWString Param_pValue, TPWString Param_pName)							{g_VideoConsole.AddParameter(Param_pValue, Param_pName);}
void	VideoConsole_AddParameter(TInt      Param_iValue, TPWString Param_pName, TBool Param_bDisplayAsHex)	{g_VideoConsole.AddParameter(Param_iValue, Param_pName, Param_bDisplayAsHex);}
void	VideoConsole_AddParameter(TFloat    Param_fValue, TPWString Param_pName)							{g_VideoConsole.AddParameter(Param_fValue, Param_pName);}
void	VideoConsole_AddParameter(TBool     Param_bValue, TPWString Param_pName)							{g_VideoConsole.AddParameter(Param_bValue, Param_pName);}
void	VideoConsole_WaitForMessageExclusion()	{g_VideoConsole.WaitForMessageExclusion();}

void	VideoConsole_UpdateStatistics(TPWString Param_pString)	{g_VideoConsole.UpdateStatistics(Param_pString);}
TBool	VideoConsole_GetConfiguration(eConfiguration_Options Param_eOption)	{return g_VideoConsole.GetConfiguration(Param_eOption);}

TListID	VideoConsole_AddRasterOption(TPWString Param_pName, void* Param_pLinkedVariable, stTrackerType Param_stType, fnOptionNotify Param_fnOptionNotify)													{return g_VideoConsole.AddRasterOption(Param_pName, Param_pLinkedVariable, Param_stType, Param_fnOptionNotify);}
TListID	VideoConsole_AddRasterOption(TPWString Param_pName, TBool* Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify)																			{return g_VideoConsole.AddRasterOption(Param_pName, Param_pLinkedVariable, Param_fnOptionNotify);}
TListID	VideoConsole_AddRasterOption(TPWString Param_pName, TInt* Param_pLinkedVariable, TInt Param_iMinimum, TInt Param_iMaximum, TInt Param_iDelta, fnOptionNotify_Int Param_fnOptionNotify)				{return g_VideoConsole.AddRasterOption(Param_pName, Param_pLinkedVariable, Param_iMinimum, Param_iMaximum, Param_iDelta, Param_fnOptionNotify);}
TListID	VideoConsole_AddRasterOption(TPWString Param_pName, TFloat* Param_pLinkedVariable, TFloat Param_fMinimum, TFloat Param_fMaximum, TFloat Param_fDelta, fnOptionNotify_Float Param_fnOptionNotify)	{return g_VideoConsole.AddRasterOption(Param_pName, Param_pLinkedVariable, Param_fMinimum, Param_fMaximum, Param_fDelta, Param_fnOptionNotify);}



TListID	VideoConsole_AddAdditionalStatisticsOption(TPWString Param_pName, void* Param_pLinkedVariable, stTrackerType Param_stType, fnOptionNotify Param_fnOptionNotify)										{return g_VideoConsole.AddAdditionalStatisticsOption(Param_pName, Param_pLinkedVariable, Param_stType, Param_fnOptionNotify);}
TListID	VideoConsole_AddAdditionalStatisticsOption(TPWString Param_pName, TBool* Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify)													        {return g_VideoConsole.AddAdditionalStatisticsOption(Param_pName, Param_pLinkedVariable, Param_fnOptionNotify);}
void	VideoConsole_ClearStatistics()	{g_VideoConsole.ClearStatistics();}
void	VideoConsole_AddStatistics(TPWString Param_pString, TPWString Param_pModuleName, stStringList_EntryType Param_stEntryType)	{g_VideoConsole.AddStatistics(Param_pString, Param_pModuleName, Param_stEntryType);}

void	VideoConsole_choucroutte() {int i;i=10;}

void	VideoConsole_UpdateLinkedVariable(TListID Param_iOptionID, TFloat* Param_pLinkedVariable) {g_VideoConsole.UpdateLinkedVariable(Param_iOptionID, Param_pLinkedVariable);}
void	VideoConsole_UpdateLinkedVariable(TListID Param_iOptionID, TBool* Param_pLinkedVariable) {g_VideoConsole.UpdateLinkedVariable(Param_iOptionID, Param_pLinkedVariable);}

// ***********************************************************************************************************************
//    Private Variables
// ***********************************************************************************************************************

CVideoConsole g_VideoConsole;

// ***********************************************************************************************************************
//    Class Methods : CWidget_Video_Panel_Texture
// ***********************************************************************************************************************

CWidget_Video_Panel_Texture::CWidget_Video_Panel_Texture(TInt Param_iTextureID, TInt Param_iTextureCount, TInt Param_iWidth, TInt Param_iHeight, TEngineResID Param_iResourceID) : CWidget_Base_Panel()
{
	TWChar acTextBuffer[64];

	// Fetch Texture Size

	TInt iSize = Texture_GetSize(Param_iTextureID);

	// Create SubWidget - Thumbnail

	CWidget_Base_TexturedQuad* pWidget_Thumbnail = new CWidget_Base_TexturedQuad(Param_iTextureID);

	pWidget_Thumbnail->Get_Position()->Set(0.89f, 0.25f);
	pWidget_Thumbnail->Get_Size()->Set(0.10f, 0.70f);

	m_WID_Thumbnail = AddSubWidget(pWidget_Thumbnail);

	// Create SubWidget - Size

	TWChar acSizeBuffer[32];

	IntegerToString(iSize, acSizeBuffer, TRUE);

	swprintf(acTextBuffer, L"Size %i x %i (%s bytes)", Param_iWidth, Param_iHeight, acSizeBuffer);

	CWidget_Base_Text* pWidget_Size = new CWidget_Base_Text(acTextBuffer);

	pWidget_Size->Get_Position()->Set(0.02f, 0.25f);
	pWidget_Size->Get_Size()->Set(0.25f, 1.4f);

	m_WID_Size = AddSubWidget(pWidget_Size);

	// Create SubWidget - Size Bar

	CWidget_Base_ValueTracker* pWidget_SizeBar = new CWidget_Base_ValueTracker(kstTrackerType_Bar_Size);

	pWidget_SizeBar->Get_Position()->Set(0.65f, 0.30f);
	pWidget_SizeBar->Get_Size()->Set(0.10f, 1.0f);

	m_WID_SizeBar = AddSubWidget(pWidget_SizeBar);

	pWidget_SizeBar->SetValue((float)(iSize) / 1048576.0f);

	// Create SubWidget - Key

	swprintf(acTextBuffer, L"Key %08x", Param_iResourceID);

	CWidget_Base_Text* pWidget_Key = new CWidget_Base_Text(acTextBuffer);

	pWidget_Key->Get_Position()->Set(0.02f, 0.60f);
	pWidget_Key->Get_Size()->Set(0.25f, 1.4f);

	m_WID_Key = AddSubWidget(pWidget_Key);

	// Create SubWidget - Format

	swprintf(acTextBuffer, L"Format %s", Texture_GetFormat(Param_iTextureID));

	CWidget_Base_Text* pWidget_Format = new CWidget_Base_Text(acTextBuffer);

	pWidget_Format->Get_Position()->Set(0.30f, 0.60f);
	pWidget_Format->Get_Size()->Set(0.25f, 1.4f);

	m_WID_Format = AddSubWidget(pWidget_Format);

	// Set Title To ID Number

	swprintf(acTextBuffer, L"Index %i / %i", Param_iTextureID, Param_iTextureCount - 1);

	SetTitle(acTextBuffer);
}

// ***********************************************************************************************************************

CWidget_Video_Panel_Texture::~CWidget_Video_Panel_Texture()
{
}

// ***********************************************************************************************************************

TInt CWidget_Video_Panel_Texture::GetTextureID()
{
	CWidget_Base_TexturedQuad* pWidget_Thumbnail = (CWidget_Base_TexturedQuad*)GetSubWidget(m_WID_Thumbnail);

	if (pWidget_Thumbnail != NULL)
	{
		return pWidget_Thumbnail->GetTextureID();
	}

	return kiTextureID_None;
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Video_Window_Statistics
// ***********************************************************************************************************************

CWidget_Video_Window_Statistics::CWidget_Video_Window_Statistics() : CWidget_Base_Window(L"Statistics")
{
	// Create SubWidget - Statistics Display Text

	CWidget_Base_Text* pWidget_Text = new CWidget_Base_Text();

	pWidget_Text->Get_Position()->Set(0.01f, 0.05f);
	pWidget_Text->Get_TextSize()->Set(0.2f);

	m_WID_Statistics = AddSubWidget(pWidget_Text);
}

// ***********************************************************************************************************************

CWidget_Video_Window_Statistics::~CWidget_Video_Window_Statistics()
{
}

// ***********************************************************************************************************************

void CWidget_Video_Window_Statistics::UpdateString(TPWString Param_pString)
{
	((CWidget_Base_Text*)GetSubWidget(m_WID_Statistics))->SetText(Param_pString);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Video_Window_AdditionalStatistics
// ***********************************************************************************************************************

CWidget_Video_Window_AdditonalStatistics::CWidget_Video_Window_AdditonalStatistics() : CWidget_Base_Window_Message(L"Additional Statistics", kiMessages_List_MaxSize)
{

}

// ***********************************************************************************************************************

CWidget_Video_Window_AdditonalStatistics::~CWidget_Video_Window_AdditonalStatistics()
{
}


// ***********************************************************************************************************************
//    Class Methods : CWidget_Video_Window_AdditionalStatisticsOptions
// ***********************************************************************************************************************

CWidget_Video_Window_AdditonalStatisticsOptions::CWidget_Video_Window_AdditonalStatisticsOptions() : CWidget_Base_Window_Options(L"Additional Statistics Options", 0.35f)
{
}

// ***********************************************************************************************************************

CWidget_Video_Window_AdditonalStatisticsOptions::~CWidget_Video_Window_AdditonalStatisticsOptions()
{
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Video_Window_Messages
// ***********************************************************************************************************************

CWidget_Video_Window_Messages::CWidget_Video_Window_Messages() : CWidget_Base_Window_Message(L"Messages", kiMessages_List_MaxSize)
{
}

// ***********************************************************************************************************************

CWidget_Video_Window_Messages::~CWidget_Video_Window_Messages()
{
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Video_Window_DTChart
// ***********************************************************************************************************************

CWidget_Video_Window_DTChart::CWidget_Video_Window_DTChart() : CWidget_Base_Window_Chart(L"DT Chart", 512)
{
	// Create SubWidget - Line - 60FPS

	CWidget_Base_ColorQuad* pWidget_Line_60FPS = new CWidget_Base_ColorQuad(0xFFFFFF00);

	pWidget_Line_60FPS->Get_Position()->Set(0.005f, (1.0f - 0.1666f));
	pWidget_Line_60FPS->Get_Size()->Set(0.992f, 0.005f);

	m_WID_Line_60FPS = AddSubWidget(pWidget_Line_60FPS);

	// Create SubWidget - Line - 30FPS

	CWidget_Base_ColorQuad* pWidget_Line_30FPS = new CWidget_Base_ColorQuad(0xFFFF0000);

	pWidget_Line_30FPS->Get_Position()->Set(0.005f, (1.0f - 0.3333f));
	pWidget_Line_30FPS->Get_Size()->Set(0.992f, 0.005f);

	m_WID_Line_30FPS = AddSubWidget(pWidget_Line_30FPS);

	// Create SubWidget - Text - 60FPS

	CWidget_Base_Text* pWidget_Text_60FPS = new CWidget_Base_Text(L"16ms\n60 FPS");

	pWidget_Text_60FPS->Get_Position()->Set(0.01f, (1.01f - 0.1666f));
	pWidget_Text_60FPS->Get_Size()->Set(0.18f, 0.18f);

	m_WID_Text_60FPS = AddSubWidget(pWidget_Text_60FPS);

	// Create SubWidget - Text - 30FPS

	CWidget_Base_Text* pWidget_Text_30FPS = new CWidget_Base_Text(L"33ms\n30 FPS");

	pWidget_Text_30FPS->Get_Position()->Set(0.01f, (1.01f - 0.3333f));
	pWidget_Text_30FPS->Get_Size()->Set(0.18f, 0.18f);

	m_WID_Text_30FPS = AddSubWidget(pWidget_Text_30FPS);
}

// ***********************************************************************************************************************

CWidget_Video_Window_DTChart::~CWidget_Video_Window_DTChart()
{
}

// ***********************************************************************************************************************

void CWidget_Video_Window_DTChart::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Record DT Value (Scaled on 100ms)

	RecordValue(Param_fMSecTimeDelta / 100.0f);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Video_Window_RasterOptions
// ***********************************************************************************************************************

CWidget_Video_Window_RasterOptions::CWidget_Video_Window_RasterOptions() : CWidget_Base_Window_Options(L"Raster Options")
{
}

// ***********************************************************************************************************************

CWidget_Video_Window_RasterOptions::~CWidget_Video_Window_RasterOptions()
{
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Video_Window_TextureViewer
// ***********************************************************************************************************************

CWidget_Video_Window_TextureViewer::CWidget_Video_Window_TextureViewer() : CWidget_Base_Window_Panels(L"Texture Viewer", kfPanel_Height_TextureViewer)
{
	// Initialize Navigation

	SetNavLink(eNavLink_Forward, kiWidgetID_Signal);
	SetNavLink(eNavLink_Back   , kiWidgetID_Signal);

	// Initialize Fields

	m_bIsBackgroundTransparent = FALSE;

	// Create SubWidget - Zoom - Frame

	CWidget_Base_Frame* pWidget_Zoom_Frame = new CWidget_Base_Frame();

	pWidget_Zoom_Frame->SetBackgroundColor(1.0f, 0.0f, 0.0f, 0.0f);

	m_WID_Zoom_Frame = AddSubWidget(pWidget_Zoom_Frame);

	// Create SubWidget - Zoom - Texture

	CWidget_Base_TexturedQuad* pWidget_Zoom_Texture = new CWidget_Base_TexturedQuad();

	m_WID_Zoom_Texture = AddSubWidget(pWidget_Zoom_Texture);

	// Create SubWidget - Info - Displayed Count

	CWidget_Base_Text* pWidget_Info_DisplayedCount = new CWidget_Base_Text();

	pWidget_Info_DisplayedCount->Get_Position()->Set(0.15f, 0.95f);
	pWidget_Info_DisplayedCount->Get_TextSize()->Set(0.2f);

	m_WID_Info_DisplayedCount = AddSubWidget(pWidget_Info_DisplayedCount);

	// Create SubWidget - Info - Memory Used

	CWidget_Base_Text* pWidget_Info_MemoryUsed = new CWidget_Base_Text();

	pWidget_Info_MemoryUsed->Get_Position()->Set(0.56f, 0.95f);
	pWidget_Info_MemoryUsed->Get_TextSize()->Set(0.2f);

	m_WID_Info_MemoryUsed = AddSubWidget(pWidget_Info_MemoryUsed);

	// Hide Zoom Window

	ZoomWindow_Hide();
}

// ***********************************************************************************************************************

CWidget_Video_Window_TextureViewer::~CWidget_Video_Window_TextureViewer()
{
}

// ***********************************************************************************************************************

void CWidget_Video_Window_TextureViewer::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Reset On World Change

	if (Param_bWorldHasChanged)
	{
		// Reset Widgets

		CWidget_Base_PanelList* pWidget_PanelList = (CWidget_Base_PanelList*)GetSubWidget(m_WID_PanelList);

		if (pWidget_PanelList != NULL)
		{
			pWidget_PanelList->Reset();
		}

		// Re-Populate Panel List

		Populate();
	}

	// Update Zoom Window (If Applicable)

	if (m_bIsZoomWindowActive)
	{
		ZoomWindow_Show();
	}
}

// ***********************************************************************************************************************

void CWidget_Video_Window_TextureViewer::Signal(eNavLink Param_eNavLink)
{
	switch (Param_eNavLink)
	{
		case eNavLink_Forward:
		{
			if (m_bIsZoomWindowActive)
			{
				ZoomWindow_Hide();
			}
			else
			{
				ZoomWindow_Show();
			}
			break;
		}

		case eNavLink_Back:
		{
			m_bIsBackgroundTransparent = !m_bIsBackgroundTransparent;

			CWidget_Base_Frame* pWidget_Zoom_Frame = (CWidget_Base_Frame*)GetSubWidget(m_WID_Zoom_Frame);

			if (m_bIsBackgroundTransparent)
			{
				pWidget_Zoom_Frame->SetBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
			}
			else
			{
				pWidget_Zoom_Frame->SetBackgroundColor(1.0f, 0.0f, 0.0f, 0.0f);
			}

			ZoomWindow_Show(TRUE);
			break;
		}
	}
}

// ***********************************************************************************************************************

void CWidget_Video_Window_TextureViewer::Populate()
{
	TWChar acTextBuffer[64];

	// Add Panel For Each Texture

	TInt iDisplayedCount = 0;

	for (TInt iLoop = 0; iLoop < Texture_GetCount(); iLoop++)
	{
		if (Texture_IsValid(iLoop))
		{
			iDisplayedCount++;
			AddPanel(new CWidget_Video_Panel_Texture(iLoop, Texture_GetCount(), Texture_GetWidth(iLoop), Texture_GetHeight(iLoop), Texture_GetIDFromIndex(iLoop)));
		}
	}

	// Update On-Screen Info - Displayed Count

	CWidget_Base_Text* pWidget_Info_DisplayedCount = (CWidget_Base_Text*)GetSubWidget(m_WID_Info_DisplayedCount);

	if (pWidget_Info_DisplayedCount != NULL)
	{
		swprintf(acTextBuffer, L"Textures Displayed: %i", iDisplayedCount);

		pWidget_Info_DisplayedCount->SetText(acTextBuffer);
	}

	// Update On-Screen Info - Memory Used

	CWidget_Base_Text* pWidget_Info_MemoryUsed = (CWidget_Base_Text*)GetSubWidget(m_WID_Info_MemoryUsed);

	if (pWidget_Info_MemoryUsed != NULL)
	{
		swprintf(acTextBuffer, L"Memory Used: %3.2fMB", (float)Texture_GetTotalMemoryUsed() / 1048576.0f);

		pWidget_Info_MemoryUsed->SetText(acTextBuffer);
	}
}

// ***********************************************************************************************************************

void CWidget_Video_Window_TextureViewer::ZoomWindow_Show(TBool Param_bForcedUpdate)
{
	// Fetch Widgets

	CWidget_Base_PanelList*			pWidget_PanelList		= (CWidget_Base_PanelList*)GetSubWidget(m_WID_PanelList);
	CWidget_Video_Panel_Texture*	pWidget_Panel			= (CWidget_Video_Panel_Texture*)pWidget_PanelList->GetSubWidget(pWidget_PanelList->GetSelectedPanel()->iWidgetID);
	CWidget_Base_Frame*				pWidget_Zoom_Frame		= (CWidget_Base_Frame*)GetSubWidget(m_WID_Zoom_Frame);
	CWidget_Base_TexturedQuad*		pWidget_Zoom_Texture	= (CWidget_Base_TexturedQuad*)GetSubWidget(m_WID_Zoom_Texture);

	// Exit If Already Display Correct Info

	TInt iTextureID = pWidget_Panel->GetTextureID();

	if (!Param_bForcedUpdate && m_bIsZoomWindowActive && (iTextureID == pWidget_Zoom_Texture->GetTextureID()))
	{
		return;
	}

	// Update Fields

	m_bIsZoomWindowActive = TRUE;

	// Calculate Aspect Ratio

	TInt	iWidth	= Texture_GetWidth(iTextureID);
	TInt	iHeight	= Texture_GetHeight(iTextureID);
	TFloat	fXRatio	= 1.0f;
	TFloat	fYRatio	= 1.0f;

	if ((iWidth > 0) && (iHeight > 0))
	{
		if (iWidth >= iHeight)
		{
			fYRatio = (float)iHeight / (float)iWidth;
		}
		else
		{
			fXRatio = (float)iWidth / (float)iHeight;
		}
	}

	// Setup New Title

	TWChar acTextBuffer[32];

	swprintf(acTextBuffer, L"#%i (%i x %i) %s", iTextureID, iWidth, iHeight, m_bIsBackgroundTransparent ? L"A" : L"NA");

	// Update Widget - Frame

	pWidget_Zoom_Frame->SetTitle(acTextBuffer);
	pWidget_Zoom_Frame->SetTopBarHeight(1.0f / fYRatio);
	pWidget_Zoom_Frame->SetTitleWidth(1.0f / fXRatio);

	pWidget_Zoom_Frame->Get_Position()->MoveTo(0.5f - ((kfZoomWindow_Size_X * 0.5f) * fXRatio), 0.5f - ((kfZoomWindow_Size_Y * 0.5f) * fYRatio), 0.5f * kfMSecValueDelta_PanelMove);
	pWidget_Zoom_Frame->Get_Size()->MoveTo(kfZoomWindow_Size_X * fXRatio, kfZoomWindow_Size_Y * fYRatio, kfMSecValueDelta_PanelMove);
	pWidget_Zoom_Frame->Get_Color()->Get_Alpha()->InterpolateTo(1.0f, 2.0f * kfMSecValueDelta_SelectionBarFade);

	// Update Widget - Texture

	pWidget_Zoom_Texture->SetTextureID(iTextureID);

	pWidget_Zoom_Texture->Get_Position()->MoveTo(0.5f - (((kfZoomWindow_Size_X * 0.5f) - 0.0045f) * fXRatio), 0.535f - ((kfZoomWindow_Size_Y * 0.5f) * fYRatio), 0.5f * kfMSecValueDelta_PanelMove);
	pWidget_Zoom_Texture->Get_Size()->MoveTo((kfZoomWindow_Size_X - 0.007f) * fXRatio, (kfZoomWindow_Size_Y * fYRatio) - 0.036f, kfMSecValueDelta_PanelMove);
	pWidget_Zoom_Texture->Get_Color()->Get_Alpha()->InterpolateTo(1.0f, 2.0f * kfMSecValueDelta_SelectionBarFade);
}

// ***********************************************************************************************************************

void CWidget_Video_Window_TextureViewer::ZoomWindow_Hide()
{
	// Update Fields

	m_bIsZoomWindowActive = FALSE;

	// Fetch Widgets

	CWidget_Base_Frame*			pWidget_Zoom_Frame		= (CWidget_Base_Frame*)GetSubWidget(m_WID_Zoom_Frame);
	CWidget_Base_TexturedQuad*	pWidget_Zoom_Texture	= (CWidget_Base_TexturedQuad*)GetSubWidget(m_WID_Zoom_Texture);

	// Update Position & Alpha - Frame

	if (pWidget_Zoom_Frame != NULL)
	{
		pWidget_Zoom_Frame->Get_Position()->MoveTo(0.49f, 0.49f, 0.5f * kfMSecValueDelta_PanelMove);
		pWidget_Zoom_Frame->Get_Size()->MoveTo(0.02f, 0.02f, kfMSecValueDelta_PanelMove);
		pWidget_Zoom_Frame->Get_Color()->Get_Alpha()->InterpolateTo(0.0f, 2.0f * kfMSecValueDelta_SelectionBarFade);
	}

	// Update Position & Alpha - Texture

	if (pWidget_Zoom_Texture != NULL)
	{
		pWidget_Zoom_Texture->Get_Position()->MoveTo(0.495f, 0.495f, 0.5f * kfMSecValueDelta_PanelMove);
		pWidget_Zoom_Texture->Get_Size()->MoveTo(0.01f, 0.01f, kfMSecValueDelta_PanelMove);
		pWidget_Zoom_Texture->Get_Color()->Get_Alpha()->InterpolateTo(0.0f, 2.0f * kfMSecValueDelta_SelectionBarFade);
	}
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Video_Window_Configuration
// ***********************************************************************************************************************

CWidget_Video_Window_Configuration::CWidget_Video_Window_Configuration() : CWidget_Base_Window_Configuration(eConfiguration_Count, (stConfigurationEntry*)kstConfiguration)
{
}

// ***********************************************************************************************************************

CWidget_Video_Window_Configuration::~CWidget_Video_Window_Configuration()
{
}

// ***********************************************************************************************************************
//    Class Methods : CVideoConsole
// ***********************************************************************************************************************

CVideoConsole::CVideoConsole()
{
	m_pDesktop									= NULL;

	m_WID_Window_Statistics						= kiWidgetID_Invalid;
	m_WID_Window_Messages						= kiWidgetID_Invalid;
	m_WID_Window_DTChart						= kiWidgetID_Invalid;
	m_WID_Window_RasterOptions					= kiWidgetID_Invalid;
	m_WID_Window_TextureViewer					= kiWidgetID_Invalid;
	m_WID_Window_AdditionalStatistics			= kiWidgetID_Invalid;
	m_WID_Window_AdditionalStatisticsOptions	= kiWidgetID_Invalid;

	m_WID_NotifyIcon_Error						= kiWidgetID_Invalid;
}

// ***********************************************************************************************************************

CVideoConsole::~CVideoConsole()
{
	Uninit();
}

// ***********************************************************************************************************************

void CVideoConsole::Initialize()
{
	// Ensure Non-Initialization

	if (m_pDesktop != NULL)
	{
		return;
	}

	// Create Desktop

	m_pDesktop = new CDesktop(L"Video Console", kiColor_Desktop_Theme_Green);

	// Create Additional Fonts

	g_FontManager.CreateFont(kiFontID_VideoConsole, kiBigFileKey_Texture_VideoConsole, kiBigFileKey_Texture_VideoConsole, 1.0f, 1.0f);

	// Create Windows

	m_WID_Window_Statistics						= m_pDesktop->AddWindow(new CWidget_Video_Window_Statistics());
	m_WID_Window_RasterOptions					= m_pDesktop->AddWindow(new CWidget_Video_Window_RasterOptions());
	m_WID_Window_DTChart						= m_pDesktop->AddWindow(new CWidget_Video_Window_DTChart());
	m_WID_Window_Messages						= m_pDesktop->AddWindow(new CWidget_Video_Window_Messages());
	m_WID_Window_TextureViewer					= m_pDesktop->AddWindow(new CWidget_Video_Window_TextureViewer());
	m_WID_Window_AdditionalStatisticsOptions	= m_pDesktop->AddWindow(new CWidget_Video_Window_AdditonalStatisticsOptions());
	m_WID_Window_AdditionalStatistics			= m_pDesktop->AddWindow(new CWidget_Video_Window_AdditonalStatistics());    

	m_pDesktop->AddConfigurationWindow(new CWidget_Video_Window_Configuration());

	// Setup Desktop Parameters

	m_pDesktop->SetInputMonitoringToggleButtons(eButton_Thumb_Right, eButton_DPad_Up);
	m_pDesktop->SetupIconBar(kfIcon_Video_StartPosition_Y, kcFontGlyph_Video_Icon_Input);

	// Create Notification Icons

	m_WID_NotifyIcon_Error = m_pDesktop->AddNotifyIcon(new CWidget_Base_NotifyIcon(kcFontGlyph_Video_Icon_Error, 5.0f, 1.0f));

	// Update Configuration

	m_pDesktop->SetConfiguration(eConfiguration_Display_HideFrameRate, g_MenuManager.IsInDemoMode_HiddenFPS());
	m_pDesktop->SetConfiguration(eConfiguration_Display_HideVersionInfo, g_MenuManager.IsInDemoMode_HiddenVersion());
}

// ***********************************************************************************************************************

void CVideoConsole::Uninit()
{
	if (m_pDesktop != NULL)
	{
		delete m_pDesktop;
		m_pDesktop = NULL;
	}
}

// ***********************************************************************************************************************

void CVideoConsole::Tick(TFloat Param_fSecTimeDelta)
{
	Profile_Begin("VideoConsole.Tick");

	if (m_pDesktop != NULL)
	{
		m_pDesktop->Tick(Param_fSecTimeDelta);
	}

	Profile_End();
}

// ***********************************************************************************************************************

void CVideoConsole::Render()
{
	Profile_Begin("VideoConsole.Render");

	if (m_pDesktop != NULL)
	{
		m_pDesktop->Render();
	}

	Profile_End();
}

// ***********************************************************************************************************************

void CVideoConsole::UpdateStatistics(TPWString Param_pString)
{
	if (m_pDesktop != NULL)
	{
		CWidget_Video_Window_Statistics* pWindow = ((CWidget_Video_Window_Statistics*)m_pDesktop->GetWindow(m_WID_Window_Statistics));

		if (pWindow != NULL)
		{
			pWindow->UpdateString(Param_pString);
		}
	}
}

// ***********************************************************************************************************************

void CVideoConsole::AddMessage(TPWString Param_pString, TPWString Param_pModuleName, stStringList_EntryType Param_stEntryType)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		((CWidget_Video_Window_Messages*)m_pDesktop->GetWindow(m_WID_Window_Messages))->AddMessage(m_pDesktop->GetMessageMaker()->Make(Param_pString, Param_pModuleName), Param_stEntryType);
		((CWidget_Video_Window_Messages*)m_pDesktop->GetWindow(m_WID_Window_Messages))->AddMessage(m_pDesktop->GetMessageMaker()->Make(Param_pString, Param_pModuleName), Param_stEntryType);

		// Error Notification

		if ((Param_stEntryType.iFlags_StringList & kiFlags_StringList_IsAnError) && GetConfiguration(eConfiguration_Notify_ErrorMessage))
		{
			m_pDesktop->ActivateNotifyIcon(m_WID_NotifyIcon_Error);
		}
	}
}

// ***********************************************************************************************************************

void CVideoConsole::ClearStatistics()
{
    if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
    {
		CWidget_Video_Window_Messages* pWindow = ((CWidget_Video_Window_Messages*)m_pDesktop->GetWindow(m_WID_Window_AdditionalStatistics));

		if (pWindow != NULL)
		{
			pWindow->Clear();
		}
    }
}

// ***********************************************************************************************************************

void CVideoConsole::AddStatistics(TPWString Param_pString, TPWString Param_pModuleName, stStringList_EntryType Param_stEntryType)
{
    if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
    {        
		CWidget_Video_Window_Messages* pWindow = ((CWidget_Video_Window_Messages*)m_pDesktop->GetWindow(m_WID_Window_AdditionalStatistics));

		if (pWindow != NULL)
		{
			pWindow->AddMessage(Param_pString, Param_stEntryType);
		}

		// Error Notification

		if ((Param_stEntryType.iFlags_StringList & kiFlags_StringList_IsAnError) && GetConfiguration(eConfiguration_Notify_ErrorMessage))
		{
			m_pDesktop->ActivateNotifyIcon(m_WID_NotifyIcon_Error);
		}
	}
}

// ***********************************************************************************************************************

void CVideoConsole::AddParameter(TPWString Param_pValue, TPWString Param_pName)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		m_pDesktop->GetMessageMaker()->AddParameter(Param_pValue, Param_pName);
	}
}

// ***********************************************************************************************************************

void CVideoConsole::AddParameter(TInt Param_iValue, TPWString Param_pName, TBool Param_bDisplayAsHex)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		m_pDesktop->GetMessageMaker()->AddParameter(Param_iValue, Param_pName, Param_bDisplayAsHex);
	}
}

// ***********************************************************************************************************************

void CVideoConsole::AddParameter(TFloat Param_fValue, TPWString Param_pName)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		m_pDesktop->GetMessageMaker()->AddParameter(Param_fValue, Param_pName);
	}
}

// ***********************************************************************************************************************

void CVideoConsole::AddParameter(TBool Param_bValue, TPWString Param_pName)
{
	if ((m_pDesktop != NULL) && GetConfiguration(eConfiguration_ActiveMessageLog))
	{
		m_pDesktop->GetMessageMaker()->AddParameter(Param_bValue, Param_pName);
	}
}

// ***********************************************************************************************************************

void CVideoConsole::WaitForMessageExclusion()
{
	if (m_pDesktop != NULL)
	{
		m_pDesktop->WaitForMessageExclusion();
	}
}

// ***********************************************************************************************************************

TBool CVideoConsole::GetConfiguration(eConfiguration_Options Param_eOption)
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

TListID CVideoConsole::AddRasterOption(TPWString Param_pName, void* Param_pLinkedVariable, stTrackerType Param_stType, fnOptionNotify Param_fnOptionNotify)
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

	CWidget_Video_Window_RasterOptions* pWindow = ((CWidget_Video_Window_RasterOptions*)m_pDesktop->GetWindow(m_WID_Window_RasterOptions));

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

TListID CVideoConsole::AddRasterOption(TPWString Param_pName, TBool* Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify)
{
	// Error Checking

	if (m_pDesktop == NULL)
	{
		return kiListID_Invalid;
	}

	// Fetch Options Window

	CWidget_Video_Window_RasterOptions* pWindow = ((CWidget_Video_Window_RasterOptions*)m_pDesktop->GetWindow(m_WID_Window_RasterOptions));

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

TListID CVideoConsole::AddRasterOption(TPWString Param_pName, TInt* Param_pLinkedVariable, TInt Param_iMinimum, TInt Param_iMaximum, TInt Param_iDelta, fnOptionNotify_Int Param_fnOptionNotify)
{
	// Error Checking

	if (m_pDesktop == NULL)
	{
		return kiListID_Invalid;
	}

	// Fetch Options Window

	CWidget_Video_Window_RasterOptions* pWindow = ((CWidget_Video_Window_RasterOptions*)m_pDesktop->GetWindow(m_WID_Window_RasterOptions));

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

TListID CVideoConsole::AddRasterOption(TPWString Param_pName, TFloat* Param_pLinkedVariable, TFloat Param_fMinimum, TFloat Param_fMaximum, TFloat Param_fDelta, fnOptionNotify_Float Param_fnOptionNotify)
{
	// Error Checking

	if (m_pDesktop == NULL)
	{
		return kiListID_Invalid;
	}

	// Fetch Options Window

	CWidget_Video_Window_RasterOptions* pWindow = ((CWidget_Video_Window_RasterOptions*)m_pDesktop->GetWindow(m_WID_Window_RasterOptions));

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

TListID CVideoConsole::AddAdditionalStatisticsOption(TPWString Param_pName, void* Param_pLinkedVariable, stTrackerType Param_stType, fnOptionNotify Param_fnOptionNotify)
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

    CWidget_Video_Window_RasterOptions* pWindow = ((CWidget_Video_Window_RasterOptions*)m_pDesktop->GetWindow(m_WID_Window_AdditionalStatisticsOptions));

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

TListID CVideoConsole::AddAdditionalStatisticsOption(TPWString Param_pName, TBool* Param_pLinkedVariable, fnOptionNotify_Bool Param_fnOptionNotify)
{
    // Error Checking

    if (m_pDesktop == NULL)
    {
        return kiListID_Invalid;
    }

    // Fetch Options Window

    CWidget_Video_Window_AdditonalStatisticsOptions* pWindow = ((CWidget_Video_Window_AdditonalStatisticsOptions*)m_pDesktop->GetWindow(m_WID_Window_AdditionalStatisticsOptions));

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

void CVideoConsole::UpdateLinkedVariable(TListID Param_iOptionID, TFloat* Param_pLinkedVariable)
{
	// Error Checking

    if (m_pDesktop == NULL)
    {
        return;
    }

	// Fetch Options Window

	CWidget_Video_Window_RasterOptions* pWindow = ((CWidget_Video_Window_RasterOptions*)m_pDesktop->GetWindow(m_WID_Window_RasterOptions));

	if (pWindow == NULL)
	{
		return;
	}

    // Set Notify Callback (If Applicable)

    if (Param_pLinkedVariable != NULL)
    {
        pWindow->SetLinkedVariable(Param_iOptionID, Param_pLinkedVariable);
    }
}

void CVideoConsole::UpdateLinkedVariable(TListID Param_iOptionID, TBool* Param_pLinkedVariable)
{	
    // Error Checking

    if (m_pDesktop == NULL)
    {
        return;
    }

    // Fetch Options Window

    CWidget_Video_Window_RasterOptions* pWindow = ((CWidget_Video_Window_RasterOptions*)m_pDesktop->GetWindow(m_WID_Window_RasterOptions));

    if (pWindow == NULL)
    {
        return;
    }

    // Set Notify Callback (If Applicable)

    if (Param_pLinkedVariable != NULL)
    {
        pWindow->SetLinkedVariable(Param_iOptionID, Param_pLinkedVariable);
    }
}


// ***********************************************************************************************************************

#endif // VIDEOCONSOLE_ENABLE

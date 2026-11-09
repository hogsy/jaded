// File created for Jaded, the community patched Jade engine
// Purpose: ImGui interface
// Author:  Mark E. Sowden

#include "Precomp.h"

#include "MainSharedSystem.h"
#include "ImGuiInterface.h"
#include "Profiler.h"

#include "GDInterface/GDInterface.h"

#include "AIengine.h"
#include "AIdebug.h"

#include "EDIapp.h"

#include "core/core_logger.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "extern/imgui/imgui.cpp"
#include "extern/imgui/imgui_widgets.cpp"
#include "extern/imgui/imgui_draw.cpp"
#include "extern/imgui/imgui_tables.cpp"
#include "extern/imgui/imgui_demo.cpp"

#include "extern/imguinotify/ImGuiNotify.hpp"
#include "extern/imguinotify/fa-solid-900.h"

//#define IMGUI_USE_SDL3
#ifdef IMGUI_USE_SDL3
#	include "extern/imgui/backends/imgui_impl_sdl3.cpp"
#else
#	include "extern/imgui/backends/imgui_impl_win32.cpp"
#endif
#include "extern/imgui/backends/imgui_impl_opengl3.cpp"

using namespace core;

static ImGuiContext *context;

// ForestGreen by TheAncientOwl
// https://github.com/ocornut/imgui/issues/707#issuecomment-4107169777
static void SetTheme()
{
	ImGuiStyle &style  = ImGui::GetStyle();
	ImVec4     *colors = style.Colors;

	// --- 1. Sizing and Spacing ---
	style.WindowPadding     = ImVec2( 10.0f, 10.0f );
	style.FramePadding      = ImVec2( 6.0f, 4.0f );
	style.CellPadding       = ImVec2( 6.0f, 4.0f );
	style.ItemSpacing       = ImVec2( 8.0f, 6.0f );
	style.ItemInnerSpacing  = ImVec2( 6.0f, 4.0f );
	style.TouchExtraPadding = ImVec2( 0.0f, 0.0f );
	style.IndentSpacing     = 20.0f;
	style.ScrollbarSize     = 14.0f;
	style.GrabMinSize       = 12.0f;

	// --- 2. Borders ---
	style.WindowBorderSize = 1.0f;
	style.ChildBorderSize  = 1.0f;
	style.PopupBorderSize  = 1.0f;
	style.FrameBorderSize  = 1.0f;
	style.TabBorderSize    = 0.0f;

	// --- 3. Rounding ---
	style.WindowRounding    = 6.0f;
	style.ChildRounding     = 4.0f;
	style.FrameRounding     = 4.0f;
	style.PopupRounding     = 4.0f;
	style.ScrollbarRounding = 4.0f;
	style.GrabRounding      = 4.0f;
	style.LogSliderDeadzone = 4.0f;
	style.TabRounding       = 4.0f;

	// --- 4. Full Color Palette ---

	// Text
	colors[ ImGuiCol_Text ]         = ImVec4( 0.85f, 0.90f, 0.85f, 1.00f );
	colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.50f, 0.55f, 0.50f, 1.00f );

	// Backgrounds
	colors[ ImGuiCol_WindowBg ] = ImVec4( 0.06f, 0.09f, 0.06f, 1.00f );// Deep pine
	colors[ ImGuiCol_ChildBg ]  = ImVec4( 0.08f, 0.11f, 0.08f, 1.00f );
	colors[ ImGuiCol_PopupBg ]  = ImVec4( 0.07f, 0.10f, 0.07f, 0.96f );

	// Borders
	colors[ ImGuiCol_Border ]       = ImVec4( 0.18f, 0.28f, 0.18f, 0.80f );
	colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );

	// Frames (Inputs, Checkboxes, etc.)
	colors[ ImGuiCol_FrameBg ]        = ImVec4( 0.12f, 0.18f, 0.12f, 1.00f );
	colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.18f, 0.30f, 0.18f, 1.00f );
	colors[ ImGuiCol_FrameBgActive ]  = ImVec4( 0.24f, 0.42f, 0.24f, 1.00f );

	// Title Bars
	colors[ ImGuiCol_TitleBg ]          = ImVec4( 0.09f, 0.14f, 0.09f, 1.00f );
	colors[ ImGuiCol_TitleBgActive ]    = ImVec4( 0.14f, 0.26f, 0.14f, 1.00f );
	colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.05f, 0.08f, 0.05f, 1.00f );

	// Menus
	colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.09f, 0.14f, 0.09f, 1.00f );

	// Scrollbars
	colors[ ImGuiCol_ScrollbarBg ]          = ImVec4( 0.05f, 0.08f, 0.05f, 1.00f );
	colors[ ImGuiCol_ScrollbarGrab ]        = ImVec4( 0.18f, 0.28f, 0.18f, 1.00f );
	colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.25f, 0.38f, 0.25f, 1.00f );
	colors[ ImGuiCol_ScrollbarGrabActive ]  = ImVec4( 0.32f, 0.48f, 0.32f, 1.00f );

	// Interactables
	colors[ ImGuiCol_CheckMark ]        = ImVec4( 0.45f, 0.75f, 0.45f, 1.00f );
	colors[ ImGuiCol_SliderGrab ]       = ImVec4( 0.35f, 0.55f, 0.35f, 1.00f );
	colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.45f, 0.70f, 0.45f, 1.00f );
	colors[ ImGuiCol_Button ]           = ImVec4( 0.18f, 0.35f, 0.18f, 1.00f );
	colors[ ImGuiCol_ButtonHovered ]    = ImVec4( 0.25f, 0.45f, 0.25f, 1.00f );
	colors[ ImGuiCol_ButtonActive ]     = ImVec4( 0.32f, 0.55f, 0.32f, 1.00f );
	colors[ ImGuiCol_Header ]           = ImVec4( 0.18f, 0.35f, 0.18f, 1.00f );
	colors[ ImGuiCol_HeaderHovered ]    = ImVec4( 0.25f, 0.45f, 0.25f, 1.00f );
	colors[ ImGuiCol_HeaderActive ]     = ImVec4( 0.32f, 0.55f, 0.32f, 1.00f );

	// Separators and Resizing
	colors[ ImGuiCol_Separator ]         = ImVec4( 0.18f, 0.28f, 0.18f, 1.00f );
	colors[ ImGuiCol_SeparatorHovered ]  = ImVec4( 0.25f, 0.45f, 0.25f, 1.00f );
	colors[ ImGuiCol_SeparatorActive ]   = ImVec4( 0.32f, 0.55f, 0.32f, 1.00f );
	colors[ ImGuiCol_ResizeGrip ]        = ImVec4( 0.18f, 0.35f, 0.18f, 0.80f );
	colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.25f, 0.45f, 0.25f, 1.00f );
	colors[ ImGuiCol_ResizeGripActive ]  = ImVec4( 0.32f, 0.55f, 0.32f, 1.00f );

	// Tabs
	colors[ ImGuiCol_Tab ]                = ImVec4( 0.12f, 0.22f, 0.12f, 1.00f );
	colors[ ImGuiCol_TabHovered ]         = ImVec4( 0.25f, 0.45f, 0.25f, 1.00f );
	colors[ ImGuiCol_TabActive ]          = ImVec4( 0.20f, 0.38f, 0.20f, 1.00f );
	colors[ ImGuiCol_TabUnfocused ]       = ImVec4( 0.08f, 0.15f, 0.08f, 1.00f );
	colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.12f, 0.22f, 0.12f, 1.00f );

	// Plots
	colors[ ImGuiCol_PlotLines ]            = ImVec4( 0.40f, 0.70f, 0.40f, 1.00f );
	colors[ ImGuiCol_PlotLinesHovered ]     = ImVec4( 0.50f, 0.85f, 0.50f, 1.00f );
	colors[ ImGuiCol_PlotHistogram ]        = ImVec4( 0.40f, 0.70f, 0.40f, 1.00f );
	colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 0.50f, 0.85f, 0.50f, 1.00f );

	// Tables
	colors[ ImGuiCol_TableHeaderBg ]     = ImVec4( 0.12f, 0.22f, 0.12f, 1.00f );
	colors[ ImGuiCol_TableBorderStrong ] = ImVec4( 0.20f, 0.35f, 0.20f, 1.00f );
	colors[ ImGuiCol_TableBorderLight ]  = ImVec4( 0.15f, 0.25f, 0.15f, 1.00f );
	colors[ ImGuiCol_TableRowBg ]        = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	colors[ ImGuiCol_TableRowBgAlt ]     = ImVec4( 0.08f, 0.14f, 0.08f, 0.50f );

	// Misc
	colors[ ImGuiCol_TextSelectedBg ]        = ImVec4( 0.25f, 0.55f, 0.25f, 0.50f );
	colors[ ImGuiCol_DragDropTarget ]        = ImVec4( 0.60f, 0.90f, 0.60f, 1.00f );
	colors[ ImGuiCol_NavHighlight ]          = ImVec4( 0.40f, 0.80f, 0.40f, 1.00f );
	colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 0.85f, 0.90f, 0.85f, 0.70f );
	colors[ ImGuiCol_NavWindowingDimBg ]     = ImVec4( 0.10f, 0.15f, 0.10f, 0.50f );
	colors[ ImGuiCol_ModalWindowDimBg ]      = ImVec4( 0.05f, 0.08f, 0.05f, 0.60f );

#ifdef IMGUI_HAS_DOCK
	// Docking (If using the docking branch)
	colors[ ImGuiCol_DockingPreview ] = ImVec4( 0.25f, 0.55f, 0.25f, 0.50f );
	colors[ ImGuiCol_DockingEmptyBg ] = ImVec4( 0.06f, 0.09f, 0.06f, 1.00f );
#endif
}

constexpr float BASE_FONT_SIZE = 14.0f;

static void ImGuiInterface_SetupNotify()
{
	const ImGuiIO &io = ImGui::GetIO();

	constexpr float ICON_FONT_SIZE = BASE_FONT_SIZE + 3.0f;

	static constexpr ImWchar ICON_RANGES[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig             iconsConfig;
	iconsConfig.MergeMode        = true;
	iconsConfig.PixelSnapH       = true;
	iconsConfig.GlyphMinAdvanceX = ICON_FONT_SIZE;
	io.Fonts->AddFontFromMemoryCompressedTTF( fa_solid_900_compressed_data, fa_solid_900_compressed_size, ICON_FONT_SIZE, &iconsConfig, ICON_RANGES );
}

static void ImGuiInterface_NotificationCallback( const Logger::Channel::Type type, const bool notify, const char *msg )
{
	if ( !notify )
	{
		return;
	}

	ImGuiToastType toastType;
	switch ( type )
	{
		default:
			toastType = ImGuiToastType::Info;
			break;
		case Logger::Channel::WARN:
			toastType = ImGuiToastType::Warning;
			break;
		case Logger::Channel::ERROR:
			toastType = ImGuiToastType::Error;
			break;
	}

	ImGui::InsertNotification( ImGuiToast( toastType, NOTIFY_DEFAULT_DISMISS, msg ) );
}

void ImGuiInterface_Initialize( void *windowPtr )
{
	if ( context != nullptr )
	{
		return;
	}

	IMGUI_CHECKVERSION();
	context = ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = "";

	SetTheme();

	io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Tahoma.ttf", BASE_FONT_SIZE );

#ifdef IMGUI_USE_SDL3
	// GL context is actually unused here, so just pass null
	ImGui_ImplSDL3_InitForOpenGL( ( SDL_Window * ) windowPtr, nullptr );
#else
	ImGui_ImplWin32_InitForOpenGL( windowPtr );
#endif
	ImGui_ImplOpenGL3_Init();

	// for now we'll only support notifications in the editor, but in the long-term we might want to allow in debug builds or something
	if ( jaded::sys::launchOperations.editorMode )
	{
		ImGuiInterface_SetupNotify();

		Logger::GetInstance().AddCallback( ImGuiInterface_NotificationCallback );
	}
}

void ImGuiInterface_Shutdown()
{
	if ( context != nullptr )
	{
		ImGui_ImplOpenGL3_Shutdown();
#ifdef IMGUI_USE_SDL3
		ImGui_ImplSDL3_Shutdown();
#else
		ImGui_ImplWin32_Shutdown();
#endif

		ImGui::DestroyContext( context );
		context = nullptr;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

extern "C" bool ImGuiInterface_ProcessEvents( const SDL_Event *event )
{
#ifdef IMGUI_USE_SDL3
	// Currently only available outside editor mode...
	if ( jaded::sys::launchOperations.editorMode )
	{
		return false;
	}

	return ImGui_ImplSDL3_ProcessEvent( event );
#else
	return false;
#endif
}

static void ShowPerformanceOverlay()
{
	if ( !jaded::sys::profiler.GetState() )
	{
		return;
	}

	constexpr float      PAD      = 10.0f;
	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImVec2               window_pos;
	window_pos.x = viewport->WorkPos.x + PAD;
	window_pos.y = viewport->WorkPos.y + PAD;
	ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always );
	ImGui::SetNextWindowBgAlpha( 0.35f );// Transparent background
	if ( ImGui::Begin( "Performance Metrics", nullptr,
	                   ImGuiWindowFlags_NoMove |
	                           ImGuiWindowFlags_AlwaysAutoResize |
	                           ImGuiWindowFlags_NoSavedSettings |
	                           ImGuiWindowFlags_NoFocusOnAppearing |
	                           ImGuiWindowFlags_NoNav ) )
	{
		ImGui::Text( "Number of GPU batches: %u", GDI_gpst_CurDD->profilingInformation.numBatches );
		ImGui::Text( "Number of GDI requests: %u", GDI_gpst_CurDD->profilingInformation.numRequests );

		// temp crap...
		ImGui::Separator();
		const jaded::sys::Profiler::ProfileMap &profSets = jaded::sys::profiler.GetProfilerSets();
		for ( auto i : profSets )
		{
			ImGui::Text( "%s : %f", i.first.c_str(), i.second.GetTimeTaken() );
		}
	}
	ImGui::End();
}

extern "C" void ImGuiInterface_NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
#ifdef IMGUI_USE_SDL3
	ImGui_ImplSDL3_NewFrame();
#else
	ImGui_ImplWin32_NewFrame();
#endif

	ImGui::NewFrame();

	ShowScriptErrorModal();
	ShowPerformanceOverlay();

	ImGui::RenderNotifications();
}

extern "C" void ImGuiInterface_GetCaptureState( bool *mouseState, bool *keyboardState )
{
	const ImGuiIO &io = ImGui::GetIO();
	*mouseState       = io.WantCaptureMouse;
	*keyboardState    = io.WantCaptureKeyboard;
}

extern "C" void ImGuiInterface_Render()
{
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}

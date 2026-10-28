// File created for Jaded, the community patched Jade engine
// Purpose: ImGui interface
// Author:  Mark E. Sowden

#include "Precomp.h"

#include "MainSharedSystem.h"
#include "ImGuiInterface.h"
#include "Profiler.h"

#include "GDInterface/GDInterface.h"

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

static ImGuiContext *context;

// https://github.com/ocornut/imgui/issues/707#issuecomment-1372640066
static void SetDraculaTheme()
{
	auto &colors                 = ImGui::GetStyle().Colors;
	colors[ ImGuiCol_WindowBg ]  = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
	colors[ ImGuiCol_MenuBarBg ] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Border
	colors[ ImGuiCol_Border ]       = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
	colors[ ImGuiCol_BorderShadow ] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

	// Text
	colors[ ImGuiCol_Text ]         = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
	colors[ ImGuiCol_TextDisabled ] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

	// Headers
	colors[ ImGuiCol_Header ]        = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
	colors[ ImGuiCol_HeaderHovered ] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ ImGuiCol_HeaderActive ]  = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Buttons
	colors[ ImGuiCol_Button ]        = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
	colors[ ImGuiCol_ButtonHovered ] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ ImGuiCol_ButtonActive ]  = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ ImGuiCol_CheckMark ]     = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

	// Popups
	colors[ ImGuiCol_PopupBg ] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

	// Slider
	colors[ ImGuiCol_SliderGrab ]       = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
	colors[ ImGuiCol_SliderGrabActive ] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

	// Frame BG
	colors[ ImGuiCol_FrameBg ]        = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
	colors[ ImGuiCol_FrameBgHovered ] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ ImGuiCol_FrameBgActive ]  = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Tabs
	colors[ ImGuiCol_Tab ]                = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ ImGuiCol_TabHovered ]         = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };
	colors[ ImGuiCol_TabActive ]          = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
	colors[ ImGuiCol_TabUnfocused ]       = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Title
	colors[ ImGuiCol_TitleBg ]          = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ ImGuiCol_TitleBgActive ]    = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Scrollbar
	colors[ ImGuiCol_ScrollbarBg ]          = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
	colors[ ImGuiCol_ScrollbarGrab ]        = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ ImGuiCol_ScrollbarGrabActive ]  = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

	// Seperator
	colors[ ImGuiCol_Separator ]        = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
	colors[ ImGuiCol_SeparatorHovered ] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
	colors[ ImGuiCol_SeparatorActive ]  = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

	// Resize Grip
	colors[ ImGuiCol_ResizeGrip ]        = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
	colors[ ImGuiCol_ResizeGripHovered ] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
	colors[ ImGuiCol_ResizeGripActive ]  = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

	auto &style             = ImGui::GetStyle();
	style.TabRounding       = 4;
	style.ScrollbarRounding = 9;
	style.WindowRounding    = 7;
	style.GrabRounding      = 3;
	style.FrameRounding     = 3;
	style.PopupRounding     = 4;
	style.ChildRounding     = 4;
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

	SetDraculaTheme();

	io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Tahoma.ttf", BASE_FONT_SIZE );

#ifdef IMGUI_USE_SDL3
	// GL context is actually unused here, so just pass null
	ImGui_ImplSDL3_InitForOpenGL( ( SDL_Window * ) windowPtr, nullptr );
#else
	ImGui_ImplWin32_InitForOpenGL( windowPtr );
#endif
	ImGui_ImplOpenGL3_Init();

	ImGuiInterface_SetupNotify();
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

extern "C" void ImGuiInterface_PushError( const char *message )
{
	ImGui::InsertNotification( ImGuiToast( ImGuiToastType::Error, NOTIFY_DEFAULT_DISMISS, message ) );
}

void ImGuiInterface_PushScriptError( const char *message )
{
	ImGui::InsertNotification( ImGuiToast( ImGuiToastType::Error, NOTIFY_DEFAULT_DISMISS, "Inspect", []{}, message ) );
}

extern "C" void ImGuiInterface_PushWarning( const char *message )
{
	ImGui::InsertNotification( ImGuiToast( ImGuiToastType::Warning, NOTIFY_DEFAULT_DISMISS, message ) );
}

extern "C" void ImGuiInterface_PushInfo( const char *message )
{
	ImGui::InsertNotification( ImGuiToast( ImGuiToastType::Info, NOTIFY_DEFAULT_DISMISS, message ) );
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

#if 1
	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu( "File" ) )
		{
			ShowExampleMenuFile();
			ImGui::EndMenu();
		}
		if ( ImGui::BeginMenu( "Edit" ) )
		{
			if ( ImGui::MenuItem( "Undo", "CTRL+Z" ) ) {}
			if ( ImGui::MenuItem( "Redo", "CTRL+Y", false, false ) ) {}// Disabled item
			ImGui::Separator();
			if ( ImGui::MenuItem( "Cut", "CTRL+X" ) ) {}
			if ( ImGui::MenuItem( "Copy", "CTRL+C" ) ) {}
			if ( ImGui::MenuItem( "Paste", "CTRL+V" ) ) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
#endif

	ShowPerformanceOverlay();

	ImGui::RenderNotifications();
}

extern "C" void ImGuiInterface_Render()
{
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}

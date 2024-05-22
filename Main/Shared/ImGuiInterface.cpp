// Created by Mark "hogsy" Sowden, 2023-2024 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#include "Precomp.h"

#include "MainSharedSystem.h"
#include "ImGuiInterface.h"
#include "Profiler.h"

#include "GDInterface/GDInterface.h"
#include "BIGfiles/VERsion/VERsion_Number.h"

#include "../Extern/imgui/imgui.cpp"
#include "../Extern/imgui/imgui_widgets.cpp"
#include "../Extern/imgui/imgui_draw.cpp"
#include "../Extern/imgui/imgui_tables.cpp"
#include "../Extern/imgui/imgui_demo.cpp"

#include "../Extern/imgui/backends/imgui_impl_sdl2.cpp"
#include "../Extern/imgui/backends/imgui_impl_opengl2.cpp"

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
	colors[ ImGuiCol_FrameBg ]        = ImVec4{ 0.13f, 0.13, 0.17, 1.0f };
	colors[ ImGuiCol_FrameBgHovered ] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ ImGuiCol_FrameBgActive ]  = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Tabs
	colors[ ImGuiCol_Tab ]                = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ ImGuiCol_TabHovered ]         = ImVec4{ 0.24, 0.24f, 0.32f, 1.0f };
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

void ImGuiInterface_Initialize( SDL_Window *window )
{
	IMGUI_CHECKVERSION();
	context = ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	SetDraculaTheme();

	io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Tahoma.ttf", 14.0f );

	// GL context is actually unused here, so just pass null
	ImGui_ImplSDL2_InitForOpenGL( window, nullptr );
	ImGui_ImplOpenGL2_Init();
}

void ImGuiInterface_Shutdown()
{
	if ( context != nullptr )
	{
		ImGui_ImplOpenGL2_Shutdown();
		ImGui_ImplSDL2_Shutdown();

		ImGui::DestroyContext( context );
		context = nullptr;
	}
}

extern "C" bool ImGuiInterface_ProcessEvents( const SDL_Event *event )
{
	// Currently only available outside editor mode...
	if ( jaded::sys::launchOperations.editorMode )
	{
		return false;
	}

	return ImGui_ImplSDL2_ProcessEvent( event );
}

static void ShowPerformanceOverlay()
{
	const float PAD               = 10.0f;
	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImVec2 window_pos;
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
	// Currently only available outside editor mode...
	if ( jaded::sys::launchOperations.editorMode )
	{
		return;
	}

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();

	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

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

	ShowPerformanceOverlay();
}

extern "C" void ImGuiInterface_Render()
{
	// Currently only available outside editor mode...
	if ( jaded::sys::launchOperations.editorMode )
	{
		return;
	}

	ImGui::Render();

	ImGui_ImplOpenGL2_RenderDrawData( ImGui::GetDrawData() );
}

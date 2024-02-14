// Created by Mark "hogsy" Sowden, 2023-2024 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#include "Precomp.h"

#include "MainSharedSystem.h"
#include "ImGuiInterface.h"

#include "../Extern/imgui/imgui.cpp"
#include "../Extern/imgui/imgui_widgets.cpp"
#include "../Extern/imgui/imgui_draw.cpp"
#include "../Extern/imgui/imgui_tables.cpp"
#include "../Extern/imgui/imgui_demo.cpp"

#include "../Extern/imgui/backends/imgui_impl_sdl2.cpp"
#include "../Extern/imgui/backends/imgui_impl_opengl2.cpp"

static ImGuiContext *context;

void ImGuiInterface_Initialize( SDL_Window *window )
{
	IMGUI_CHECKVERSION();
	context = ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

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

void ImGuiInterface_NewFrame()
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();

	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
}

extern "C" void ImGuiInterface_Render()
{
	ImGui::Render();

	ImGui_ImplOpenGL2_RenderDrawData( ImGui::GetDrawData() );
}

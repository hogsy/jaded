// Created by Mark "hogsy" Sowden, 2023-2024 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#pragma once

#ifdef __cplusplus
#	define IMGUI_DEFINE_MATH_OPERATORS
#	include "imgui.h"

extern "C"
{
#endif

	struct SDL_Window;

	void ImGuiInterface_Initialize( SDL_Window *window );
	void ImGuiInterface_Shutdown();
	void ImGuiInterface_NewFrame();
	void ImGuiInterface_Render();

#ifdef __cplusplus
};
#endif

// Created by Mark "hogsy" Sowden, 2023-2024 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#pragma once

#ifdef __cplusplus
#	define IMGUI_DEFINE_MATH_OPERATORS
#	include "imgui/imgui.h"

extern "C"
{
#endif

	struct SDL_Window;
	union SDL_Event;

	void ImGuiInterface_Initialize( void *windowPtr );
	void ImGuiInterface_Shutdown();
	bool ImGuiInterface_ProcessEvents( const SDL_Event *event );
	void ImGuiInterface_NewFrame();
	void ImGuiInterface_Render();

#ifdef __cplusplus
};
#endif

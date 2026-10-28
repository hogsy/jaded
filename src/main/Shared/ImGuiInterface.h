// File created for Jaded, the community patched Jade engine

#pragma once

#ifdef __cplusplus

extern "C"
{
#endif

	struct SDL_Window;
	union SDL_Event;

	void ImGuiInterface_Initialize( void *windowPtr );
	void ImGuiInterface_Shutdown();
	bool ImGuiInterface_ProcessEvents( const SDL_Event *event );

	void ImGuiInterface_PushError( const char *message );
	void ImGuiInterface_PushWarning( const char *message );
	void ImGuiInterface_PushInfo( const char *message );

	void ImGuiInterface_NewFrame();
	void ImGuiInterface_Render();

#ifdef __cplusplus
};
#endif

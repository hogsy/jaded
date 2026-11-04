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

	/**
	 * Throws a modal prompt on the screen notifying of a runtime script error.
	 */
	void ImGuiInterface_ScriptError( const std::string &filename, unsigned int line, const std::string &expression, const std::string &object, const std::string &message );

	bool ImGuiInterface_ProcessEvents( const SDL_Event *event );

	void ImGuiInterface_NewFrame();

	/**
	 * Gross method to expose ImGui's capture state, given we don't want to expose
	 * the entire API.
	 */
	void ImGuiInterface_GetCaptureState( bool *mouseState, bool *keyboardState );

	void ImGuiInterface_Render();

#ifdef __cplusplus
};
#endif

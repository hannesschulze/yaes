#include "state.hh"
#include <SDL3/SDL_init.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

auto SDL_AppInit(void** appstate, int argc, char** argv) -> SDL_AppResult
{
	(void)argc;
	(void)argv;

	auto const state = new nes::app::sdl::state;
	*appstate = state;

	return state->get_status() == nes::status::success ? SDL_APP_CONTINUE : SDL_APP_FAILURE;
}

auto SDL_AppIterate(void* appstate) -> SDL_AppResult
{
	auto const state = static_cast<nes::app::sdl::state*>(appstate);
	state->handle_iterate();
	return state->get_status() == nes::status::success ? SDL_APP_CONTINUE : SDL_APP_FAILURE;
}

auto SDL_AppEvent(void* appstate, SDL_Event* event) -> SDL_AppResult
{
	if (event->type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS;
	}

	auto const state = static_cast<nes::app::sdl::state*>(appstate);
	state->handle_event(event);
	return state->get_status() == nes::status::success ? SDL_APP_CONTINUE : SDL_APP_FAILURE;
}

auto SDL_AppQuit(void* appstate, SDL_AppResult) -> void
{
	delete static_cast<nes::app::sdl::state*>(appstate);
}

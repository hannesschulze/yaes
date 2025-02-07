#pragma once

#include "nes/common/status.hh"
#include "nes/app/application.hh"
#include "impl/display-sdl.hh"
#include "impl/input-device-keyboard-sdl.hh"
#include "impl/file-browser-posix.hh"
#include <chrono>
#include <optional>

#include <SDL3/SDL.h>

namespace nes::app::sdl
{
	/// Manages the SDL application lifecycle.
	class state
	{
		status status_{ status::success };
		SDL_Window* window_{ nullptr };
		SDL_Renderer* renderer_{ nullptr };
		SDL_Texture* texture_{ nullptr };
		std::optional<std::chrono::steady_clock::time_point> last_time_point_;
		display_sdl display_;
		input_device_keyboard_sdl keyboard_;
		file_browser_posix file_browser_;
		application application_;

	public:
		explicit state();
		~state();

		auto get_status() const -> status { return status_; }

		auto handle_event(SDL_Event* event) -> void;
		auto handle_iterate() -> void;
	};
} // namespace nes::app::sdl

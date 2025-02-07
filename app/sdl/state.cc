#include "state.hh"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <iostream>

namespace nes::app::sdl
{
	state::state()
		: application_{ display_, keyboard_, file_browser_ }
	{
		SDL_SetAppMetadata("NES", "1.0", "com.github.hannesschulze.nes");

		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
			status_ = status::error_system_error;
			return;
		}

		if (!SDL_CreateWindowAndRenderer("NES", display::width * 3, display::height * 3, 0, &window_, &renderer_))
		{
			std::cerr << "Couldn't create window/renderer: " << SDL_GetError() << std::endl;
			status_ = status::error_system_error;
			return;
		}

		SDL_SetRenderVSync(renderer_, SDL_RENDERER_VSYNC_ADAPTIVE);

		if (!(texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, display::width, display::height)))
		{
			std::cerr << "Couldn't create texture: " << SDL_GetError() << std::endl;
			status_ = status::error_system_error;
			return;
		}

		if (!SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST))
		{
			std::cerr << "Unable to set texture scale mode: " << SDL_GetError() << std::endl;
			status_ = status::error_system_error;
			return;
		}

		display_.set_texture(texture_);
	}

	state::~state()
	{
		if (texture_) { SDL_DestroyTexture(texture_); }
	}

	auto state::handle_event(SDL_Event* event) -> void
	{
		switch (event->type)
		{
			case SDL_EVENT_KEY_DOWN:
				keyboard_.handle_key_down(event);
				break;
			case SDL_EVENT_KEY_UP:
				keyboard_.handle_key_up(event);
				break;
			default:
				break;
		}
	}

	auto state::handle_iterate() -> void
	{
		auto const current = std::chrono::steady_clock::now();
		auto const previous = last_time_point_.value_or(current);
		last_time_point_ = current;

		auto const elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(current - previous);
		application_.frame(static_cast<u32>(elapsed_us.count()));

		if (!SDL_RenderTexture(renderer_, texture_, nullptr, nullptr))
		{
			std::cerr << "Unable to render texture: " << SDL_GetError() << std::endl;
			status_ = status::error_system_error;
			return;
		}

		if (!SDL_RenderPresent(renderer_))
		{
			std::cerr << "Unable to present frame: " << SDL_GetError() << std::endl;
			status_ = status::error_system_error;
			return;
		}
	}
} // namespace nes::app::sdl

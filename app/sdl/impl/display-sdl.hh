#pragma once

#include "nes/common/display.hh"

#include <SDL3/SDL.h>

namespace nes::app::sdl
{
	/// Display implementation rendering contents into an SDL texture.
	class display_sdl final : public display
	{
		SDL_Texture* texture_{ nullptr };
		u32 buffer_back_[width * height]{};

	public:
		explicit display_sdl() = default;

		auto get_texture() const -> SDL_Texture* { return texture_; }
		auto set_texture(SDL_Texture* v) -> void { texture_ = v; }

		auto switch_buffers() -> void override;
		auto set(u32 x, u32 y, rgb value) -> void override;
	};
} // namespace nes::app::sdl

#include "impl/display-sdl.hh"

namespace nes::app::sdl
{
	auto display_sdl::set(u32 const x, u32 const y, rgb const color) -> void
	{
		buffer_back_[(y * width) + x] = (color.r << 24) | (color.g << 16) | (color.b << 8);
	}

	auto display_sdl::switch_buffers() -> void
	{
		if (!texture_) { return; }

		SDL_UpdateTexture(texture_, nullptr, buffer_back_, width * 4);
	}
} // namespace nes::app::sdl

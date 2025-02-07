#pragma once

#include "nes/app/input/input-device-keyboard.hh"
#include "nes/app/input/input-buffer.hh"
#include <optional>

#include <SDL3/SDL.h>

namespace nes::app::sdl
{
	/// Keyboard implementation using SDL.
	class input_device_keyboard_sdl final : public input_device_keyboard
	{
		input_buffer buffer_;

	public:
		explicit input_device_keyboard_sdl() = default;

		auto handle_key_down(SDL_Event*) -> void;
		auto handle_key_up(SDL_Event*) -> void;

		auto read_key(key const key) -> bool override { return buffer_.read_key(key); }
		auto poll_event() -> input_event override { return buffer_.poll_event(); }

	private:
		auto convert_key(SDL_Scancode) const -> std::optional<key>;
	};
} // namespace nes::app::sdl

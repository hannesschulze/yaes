#pragma once

#import "nes/app/input/input-device-keyboard.hh"
#import "nes/app/input/input-buffer.hh"
#import <optional>

#import <GameController/GameController.h>

namespace nes::app::mac
{
	/// Input device wrapping a GCKeyboard.
	class input_device_gc_keyboard final : public input_device_keyboard
	{
		GCKeyboardInput* profile_{ nil };
		input_buffer buffer_;

	public:
		explicit input_device_gc_keyboard(GCKeyboardInput* profile);
		~input_device_gc_keyboard() override;

		auto set_profile(GCKeyboardInput* profile) -> void;

		auto read_key(key const key) -> bool override { return buffer_.read_key(key); }
		auto poll_event() -> input_event override { return buffer_.poll_event(); }

	private:
		auto handle_key_changed(GCKeyCode key_code, BOOL pressed) -> void;
		auto convert_key_code(GCKeyCode key_code) -> std::optional<key>;
	};
} // namespace nes::app::mac
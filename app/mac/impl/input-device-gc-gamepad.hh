#pragma once

#import "nes/app/input/input-device-controller.hh"

#import <GameController/GameController.h>

namespace nes::app::mac
{
	/// Input device wrapping a GCExtendedGamepad.
	class input_device_gc_gamepad final : public input_device_controller
	{
		GCExtendedGamepad* profile_;

	public:
		explicit input_device_gc_gamepad(GCExtendedGamepad* profile);

		auto is_reliable() const -> bool override { return true; }
		auto get_controller() const -> GCController* { return [profile_ controller]; }
		auto get_name() const -> string_view override;
		auto get_index() const -> index override;
		auto set_index(index) -> void override;
		auto read_buttons() -> sys::button_mask override;
	};
} // namespace nes::app::mac
#pragma once

#include "nes/sys/types/button-mask.hh"
#include "nes/common/types.hh"

namespace nes::sys
{
	class controller
	{
		button_mask pressed_;
		u32 index_{ 0 };
		bool strobing_{ false };

	public:
		explicit controller() = default;

		controller(controller const&) = delete;
		controller(controller&&) = delete;
		auto operator=(controller const&) -> controller& = delete;
		auto operator=(controller&&) -> controller& = delete;

		auto get_pressed() const -> button_mask { return pressed_;}
		auto ref_pressed() -> button_mask& { return pressed_; }
		auto set_pressed(button_mask const value) -> void { pressed_ = value; }

		// IO register

		auto read() -> u8;
		auto write(u8) -> void;
	};
} // namespace nes::sys
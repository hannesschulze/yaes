#pragma once

#include "nes/util/button-mask.hh"

namespace nes
{
	class controller
	{
		button_mask pressed_;

	public:
		explicit controller() = default;

		controller(controller const&) = delete;
		controller(controller&&) = delete;
		auto operator=(controller const&) -> controller& = delete;
		auto operator=(controller&&) -> controller& = delete;

		auto get_pressed() const -> button_mask { return pressed_;}
		auto get_pressed() -> button_mask& { return pressed_; }
		auto set_pressed(button_mask const value) -> void { pressed_ = value; }
	};
} // namespace nes
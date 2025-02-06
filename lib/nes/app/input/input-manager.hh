#pragma once

#include "nes/app/input/input-device-keyboard.hh"
#include "nes/app/input/input-device-controller.hh"
#include "nes/common/containers/list.hh"
#include "nes/common/types.hh"

namespace nes::app
{
	/// Manages input devices.
	///
	/// The main purpose of this class is to assign newly plugged in controllers as the primary/secondary input and fall
	/// back to the keyboard if needed.
	class input_manager
	{
		static constexpr auto input_count = u32{ 2 };

		input_device_keyboard* keyboard_;
		input_device_controller::index keyboard_index_{ input_device_controller::index::unused };
		list<input_device_controller> controllers_;
		struct
		{
			input_device* device{ nullptr };
			input_device_controller* controller{ nullptr };
		} inputs_[input_count]{};

	public:
		explicit input_manager(input_device_keyboard&);

		input_manager(input_manager const&) = delete;
		input_manager(input_manager&&) = delete;
		auto operator=(input_manager const&) -> input_manager& = delete;
		auto operator=(input_manager&&) -> input_manager& = delete;

		auto get_input_1() const -> input_device& { return *inputs_[0].device; }
		auto get_input_2() const -> input_device& { return *inputs_[1].device; }

		auto toggle_input_1() -> void { toggle_input(0); }
		auto toggle_input_2() -> void { toggle_input(1); }

		auto get_keyboard() const -> input_device_keyboard& { return *keyboard_; }
		auto get_controllers() const -> list<input_device_controller> const& { return controllers_; }

		auto add_controller(input_device_controller&) -> void;
		auto remove_controller(input_device_controller&) -> void;

	private:
		auto toggle_input(u32) -> void;
		auto fill_unassigned_inputs() -> void;
	};
} // namespace nes::app
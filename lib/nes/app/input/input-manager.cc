#include "nes/app/input/input-manager.hh"

namespace nes::app
{
	namespace
	{
		class input_device_dummy final : public input_device
		{
		public:
			explicit input_device_dummy() = default;

			static auto get_instance() -> input_device_dummy&
			{
				static auto res = input_device_dummy{};
				return res;
			}

			auto read_key_up() -> bool override { return false; }
			auto read_key_down() -> bool override { return false; }
			auto read_key_left() -> bool override { return false; }
			auto read_key_right() -> bool override { return false; }
			auto read_key_a() -> bool override { return false; }
			auto read_key_b() -> bool override { return false; }
			auto read_key_select() -> bool override { return false; }
			auto read_key_start() -> bool override { return false; }
		};
	} // namespace

	input_manager::input_manager(input_device_keyboard& keyboard)
		: keyboard_{ &keyboard }
	{
		fill_unassigned_inputs();
	}

	auto input_manager::add_controller(input_device_controller& controller) -> void
	{
		controllers_.append(controller);
		if (controller.is_reliable())
		{
			auto const index = controller.get_index();
			if (index != input_device_controller::index::unused)
			{
				// This controller was previously used and assigned an index -> replace the current device with this
				// (known) controller.
				auto const i = static_cast<u32>(index);
				if (inputs_[i].controller)
				{
					inputs_[i].controller->set_index(input_device_controller::index::unused);
				}

				inputs_[i].controller = &controller;
				inputs_[i].device = &controller;
			}

			// There may be another free controller now.
			fill_unassigned_inputs();
		}
	}

	auto input_manager::remove_controller(input_device_controller& controller) -> void
	{
		controllers_.remove(controller);
		fill_unassigned_inputs();
	}

	auto input_manager::fill_unassigned_inputs() -> void
	{
		auto it = controllers_.begin();
		input_device* fallback = keyboard_;

		for (auto i = u32{ 0 }; i < input_count; ++i)
		{
			if (inputs_[i].controller != nullptr) { continue; }

			// This input is not assigned -> use the next available controller if available.
			while (it != controllers_.end())
			{
				auto controller = *it;
				if (controller->is_reliable() && controller->get_index() == input_device_controller::index::unused)
				{
					break;
				}
				++it;
			}

			if (it != controllers_.end())
			{
				// We've found an unused controller -> assign it.
				inputs_[i].controller = *it;
				inputs_[i].controller->set_index(static_cast<input_device_controller::index>(i));
				inputs_[i].device = *it;
			}
			else
			{
				// We're all out of controllers -> fall back to the keyboard (if this is the first missing controller)
				// or nothing.
				inputs_[i].device = fallback;
				fallback = &input_device_dummy::get_instance(); // Only use the keyboard as a fallback once.
			}
		}
	}
} // namespace nes::app
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

			auto get_name() const -> string_view override { return "(None)"; }
			auto read_buttons() -> sys::button_mask override { return sys::button_mask{}; }
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
				if (keyboard_index_ == index)
				{
					keyboard_index_ = input_device_controller::index::unused;
				}
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

	auto input_manager::toggle_input(u32 const i) -> void
	{
		// If currently none/keyboard, use the first available controller.
		auto curr_was_prev = inputs_[i].controller == nullptr;
		if (curr_was_prev && keyboard_index_ == input_device_controller::index::unused)
		{
			// If currently none and keyboard is available, switch to keyboard.
			keyboard_index_ = static_cast<input_device_controller::index>(i);
			inputs_[i].controller = nullptr;
			inputs_[i].device = keyboard_;
			return;
		}

		// Reset old controller index
		if (inputs_[i].controller) { inputs_[i].controller->set_index(input_device_controller::index::unused); }
		if (keyboard_index_ == static_cast<input_device_controller::index>(i)) { keyboard_index_ = input_device_controller::index::unused; }

		for (auto& controller : controllers_)
		{
			if (&controller == inputs_[i].controller)
			{
				curr_was_prev = true;
			}
			else if (curr_was_prev && controller.get_index() == input_device_controller::index::unused)
			{
				inputs_[i].controller = &controller;
				inputs_[i].controller->set_index(static_cast<input_device_controller::index>(i));
				inputs_[i].device = &controller;
				return;
			}
		}

		// No successor found, switch to none.
		inputs_[i].controller = nullptr;
		inputs_[i].device = &input_device_dummy::get_instance();
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
				if (it->is_reliable() && it->get_index() == input_device_controller::index::unused)
				{
					break;
				}
				++it;
			}

			if (it != controllers_.end())
			{
				// We've found an unused controller -> assign it.
				inputs_[i].controller = &*it;
				inputs_[i].controller->set_index(static_cast<input_device_controller::index>(i));
				inputs_[i].device = &*it;
			}
			else
			{
				// We're all out of controllers -> fall back to the keyboard (if this is the first missing controller)
				// or nothing.
				inputs_[i].device = fallback;
				if (fallback == keyboard_) { keyboard_index_ = static_cast<input_device_controller::index>(i); }
				fallback = &input_device_dummy::get_instance(); // Only use the keyboard as a fallback once.
			}
		}
	}
} // namespace nes::app
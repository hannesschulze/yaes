#pragma once

#include "nes/app/input/input-device.hh"
#include "nes/app/input/modifier-mask.hh"
#include "nes/app/input/input-event.hh"
#include "nes/app/input/key.hh"

namespace nes::app
{
	/// A keyboard input device.
	class input_device_keyboard : public input_device
	{
	public:
		/// Read the current state of a key.
		virtual auto read_key(key) -> bool = 0;

		/// Poll for events.
		///
		/// As long as there are any buffered key-down or key-up events, they will be returned. After that, this
		/// function will return input_event::type::none until there is another event.
		virtual auto poll_event() -> input_event = 0;

		auto read_buttons() -> sys::button_mask final
		{
			auto res = sys::button_mask{};
			if (read_key(key::letter_w)) { res.add(sys::buttons::up); }
			if (read_key(key::letter_a)) { res.add(sys::buttons::left); }
			if (read_key(key::letter_s)) { res.add(sys::buttons::down); }
			if (read_key(key::letter_d)) { res.add(sys::buttons::right); }
			if (read_key(key::letter_k)) { res.add(sys::buttons::b); }
			if (read_key(key::letter_l)) { res.add(sys::buttons::a); }
			if (read_key(key::space)) { res.add(sys::buttons::select); }
			if (read_key(key::enter)) { res.add(sys::buttons::start); }
			return res;
		}

		auto read_modifiers() -> modifier_mask
		{
			auto res = modifier_mask{};
			if (read_key(key::control_left) || read_key(key::control_right)) { res.add(modifiers::control); }
			if (read_key(key::shift_left) || read_key(key::shift_right)) { res.add(modifiers::shift); }
			if (read_key(key::alt_left) || read_key(key::alt_right)) { res.add(modifiers::alt); }
			return res;
		}

		auto get_name() const -> std::string_view final { return "Keyboard"; }

	protected:
		explicit input_device_keyboard() = default;
	};
} // namespace nes::app
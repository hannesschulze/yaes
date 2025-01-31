#pragma once

#include "nes/sys/types/button-mask.hh"

namespace nes::app
{
	/// An abstract input device which can be used to play games.
	class input_device
	{
	public:
		virtual ~input_device() = default;

		input_device(input_device const&) = delete;
		input_device(input_device&&) = delete;
		auto operator=(input_device const&) -> input_device& = delete;
		auto operator=(input_device&&) -> input_device& = delete;

		virtual auto read_key_up() -> bool = 0;
		virtual auto read_key_down() -> bool = 0;
		virtual auto read_key_left() -> bool = 0;
		virtual auto read_key_right() -> bool = 0;
		virtual auto read_key_a() -> bool = 0;
		virtual auto read_key_b() -> bool = 0;
		virtual auto read_key_select() -> bool = 0;
		virtual auto read_key_start() -> bool = 0;

		auto read_mask() -> sys::button_mask
		{
			auto res = sys::button_mask{};
			if (read_key_up()) { res.add(sys::buttons::up); }
			if (read_key_down()) { res.add(sys::buttons::down); }
			if (read_key_left()) { res.add(sys::buttons::left); }
			if (read_key_right()) { res.add(sys::buttons::right); }
			if (read_key_a()) { res.add(sys::buttons::a); }
			if (read_key_b()) { res.add(sys::buttons::b); }
			if (read_key_select()) { res.add(sys::buttons::select); }
			if (read_key_start()) { res.add(sys::buttons::start); }
			return res;
		}

	protected:
		explicit input_device() = default;
	};
} // namespace nes::app
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

		virtual auto read_buttons() -> sys::button_mask = 0;

	protected:
		explicit input_device() = default;
	};
} // namespace nes::app
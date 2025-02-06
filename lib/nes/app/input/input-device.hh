#pragma once

#include "nes/sys/types/button-mask.hh"
#include "nes/common/containers/string-view.hh"

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

		/// Read the controller button state.
		virtual auto read_buttons() -> sys::button_mask = 0;

		/// Human-readable name for the device.
		virtual auto get_name() const -> string_view = 0;

	protected:
		explicit input_device() = default;
	};
} // namespace nes::app
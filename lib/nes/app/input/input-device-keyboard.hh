#pragma once

#include "nes/app/input/input-device.hh"

namespace nes::app
{
	/// A keyboard input device.
	class input_device_keyboard : public input_device
	{
	public:
		virtual auto read_key_escape() -> bool = 0;

	protected:
		explicit input_device_keyboard() = default;
	};
} // namespace nes::app
#import "impl/input-device-gc-gamepad.hh"

namespace nes::app::mac
{
	input_device_gc_gamepad::input_device_gc_gamepad(GCExtendedGamepad* profile)
		: profile_{ profile }
	{
	}

	auto input_device_gc_gamepad::get_name() const -> string_view
	{
		return [[get_controller() vendorName] UTF8String];
	}

	auto input_device_gc_gamepad::get_index() const -> index
	{
		switch ([get_controller() playerIndex])
		{
			case GCControllerPlayerIndex1:
				return index::player_1;
			case GCControllerPlayerIndex2:
				return index::player_2;
			case GCControllerPlayerIndex3:
			case GCControllerPlayerIndex4:
			case GCControllerPlayerIndexUnset:
				break;
		}
		return index::unused;
	}

	auto input_device_gc_gamepad::set_index(index const idx) -> void
	{
		switch (idx)
		{
			case index::unused:
				[get_controller() setPlayerIndex:GCControllerPlayerIndexUnset];
				break;
			case index::player_1:
				[get_controller() setPlayerIndex:GCControllerPlayerIndex1];
				break;
			case index::player_2:
				[get_controller() setPlayerIndex:GCControllerPlayerIndex2];
				break;
		}
	}

	auto input_device_gc_gamepad::read_buttons() -> sys::button_mask
	{
		auto res = sys::button_mask{};
		if ([[[profile_ dpad] up] isPressed]) { res.add(sys::buttons::up); }
		if ([[[profile_ dpad] down] isPressed]) { res.add(sys::buttons::down); }
		if ([[[profile_ dpad] left] isPressed]) { res.add(sys::buttons::left); }
		if ([[[profile_ dpad] right] isPressed]) { res.add(sys::buttons::right); }
		if ([[profile_ buttonA] isPressed]) { res.add(sys::buttons::a); }
		if ([[profile_ buttonB] isPressed]) { res.add(sys::buttons::b); }
		if ([[profile_ leftTrigger] isPressed]) { res.add(sys::buttons::select); }
		if ([[profile_ rightTrigger] isPressed]) { res.add(sys::buttons::start); }
		return res;
	}
} // namespace nes::app::mac
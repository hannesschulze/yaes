#include "controller-adapter-gamepad.hh"

@implementation ControllerAdapterGamepad

- (instancetype)initWithProfile:(GCExtendedGamepad*)profile
{
	self = [super init];
	if (self)
	{
		_profile = profile;
	}
	return self;
}

- (nes::button_mask)readPressedButtons
{
	auto res = nes::button_mask{};
	if ([[[_profile dpad] up] isPressed]) { res.add(nes::buttons::up); }
	if ([[[_profile dpad] down] isPressed]) { res.add(nes::buttons::down); }
	if ([[[_profile dpad] left] isPressed]) { res.add(nes::buttons::left); }
	if ([[[_profile dpad] right] isPressed]) { res.add(nes::buttons::right); }
	if ([[_profile buttonA] isPressed]) { res.add(nes::buttons::a); }
	if ([[_profile buttonB] isPressed]) { res.add(nes::buttons::b); }
	if ([[_profile leftTrigger] isPressed]) { res.add(nes::buttons::select); }
	if ([[_profile rightTrigger] isPressed]) { res.add(nes::buttons::start); }
	return res;
}

@end
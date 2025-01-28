#import "controller-adapter-keyboard.hh"

@implementation ControllerAdapterKeyboard

- (instancetype)init
{
	self = [super init];
	if (self)
	{
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(handleKeyboardConnected:)
													 name:GCKeyboardDidConnectNotification
												   object:nil];
	}
	return self;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)handleKeyboardConnected:(NSNotification*)notification
{
	_keyboard = [GCKeyboard coalescedKeyboard];
}

- (nes::button_mask)readPressedButtons
{
	auto profile = [[self keyboard] keyboardInput];

	auto res = nes::button_mask{};
	if ([[profile buttonForKeyCode:GCKeyCodeUpArrow] isPressed]) { res.add(nes::buttons::up); }
	if ([[profile buttonForKeyCode:GCKeyCodeDownArrow] isPressed]) { res.add(nes::buttons::down); }
	if ([[profile buttonForKeyCode:GCKeyCodeLeftArrow] isPressed]) { res.add(nes::buttons::left); }
	if ([[profile buttonForKeyCode:GCKeyCodeRightArrow] isPressed]) { res.add(nes::buttons::right); }
	if ([[profile buttonForKeyCode:GCKeyCodeKeyZ] isPressed]) { res.add(nes::buttons::a); }
	if ([[profile buttonForKeyCode:GCKeyCodeKeyX] isPressed]) { res.add(nes::buttons::b); }
	if ([[profile buttonForKeyCode:GCKeyCodeReturnOrEnter] isPressed]) { res.add(nes::buttons::start); }
	if ([[profile buttonForKeyCode:GCKeyCodeSpacebar] isPressed]) { res.add(nes::buttons::select); }
	return res;
}

@end
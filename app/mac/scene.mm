#import "scene.hh"
#import "impl/display-spritekit.hh"
#import "impl/file-browser-posix.hh"
#import "impl/input-device-gc-keyboard.hh"
#import "impl/input-device-gc-gamepad.hh"
#import "impl/input-device-serial-controller.hh"
#import "nes/app/application.hh"
#import <vector>
#import <memory>
#import <optional>

namespace
{
    constexpr auto const serial_controller_path = "/dev/cu.usbserial-FTB6SPL3";
} // namespace

@implementation Scene
{
	std::unique_ptr<nes::app::mac::display_spritekit> _display;
    std::unique_ptr<nes::app::mac::file_browser_posix> _fileBrowser;
    std::unique_ptr<nes::app::mac::input_device_serial_controller> _serialController;
	std::unique_ptr<nes::app::mac::input_device_gc_keyboard> _keyboard;
	std::vector<std::unique_ptr<nes::app::mac::input_device_gc_gamepad>> _controllers;
	std::unique_ptr<nes::app::application> _app;
    std::optional<NSTimeInterval> _lastTimestamp;
}

- (instancetype)init
{
    self = [super initWithSize:CGSizeMake(nes::display::width, nes::display::height)];
    if (self)
    {
        auto node = [[SKSpriteNode alloc] init];
        [node setAnchorPoint:CGPointZero];
        [node setSize:[self size]];

        [self setScaleMode:SKSceneScaleModeAspectFit];
        [self addChild:node];

        _display = std::make_unique<nes::app::mac::display_spritekit>(node);
        _fileBrowser = std::make_unique<nes::app::mac::file_browser_posix>();
        _serialController = std::make_unique<nes::app::mac::input_device_serial_controller>(serial_controller_path);
		_keyboard = std::make_unique<nes::app::mac::input_device_gc_keyboard>([[GCKeyboard coalescedKeyboard] keyboardInput]);
        _app = std::make_unique<nes::app::application>(*_display, *_keyboard, *_fileBrowser);

        _app->add_controller(*_serialController);

		for (GCController* controller in [GCController controllers])
		{
			auto gamepad = [controller extendedGamepad];
			if (gamepad)
			{
				auto adapter = std::make_unique<nes::app::mac::input_device_gc_gamepad>(gamepad);
				_app->add_controller(*adapter);
				_controllers.emplace_back(std::move(adapter));
			}
		}

		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(handleKeyboardConnected:)
													 name:GCKeyboardDidConnectNotification
												   object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(handleControllerConnected:)
													 name:GCControllerDidConnectNotification
												   object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(handleControllerDisconnected:)
													 name:GCControllerDidDisconnectNotification
												   object:nil];
    }
    return self;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)keyUp:(NSEvent*)event
{
	// Handled by Game Controller
}

- (void)keyDown:(NSEvent*)event
{
	// Handled by Game Controller
}

- (void)update:(NSTimeInterval)currentTime
{
    auto const lastTimestamp = _lastTimestamp.value_or(currentTime);
    _lastTimestamp = currentTime;
    auto deltaMicroseconds = static_cast<nes::u32>((currentTime - lastTimestamp) * 1000. * 1000.);
	deltaMicroseconds = nes::min(deltaMicroseconds, nes::u32{ 50000 });
    _app->frame(deltaMicroseconds);
}

- (void)handleKeyboardConnected:(NSNotification*)notification
{
	_keyboard->set_profile([[GCKeyboard coalescedKeyboard] keyboardInput]);
}

- (void)handleControllerConnected:(NSNotification*)notification
{
	auto controller = (GCController*)[notification object];
	auto gamepad = [controller extendedGamepad];
	if (gamepad)
	{
		auto adapter = std::make_unique<nes::app::mac::input_device_gc_gamepad>(gamepad);
		_app->add_controller(*adapter);
		_controllers.emplace_back(std::move(adapter));
	}
}

- (void)handleControllerDisconnected:(NSNotification*)notification
{
	auto controller = (GCController*)[notification object];
	for (auto i = _controllers.size(); i > 0; --i)
	{
		auto& adapter = *_controllers[i - 1];
		if (adapter.get_controller() == controller)
		{
			_app->remove_controller(adapter);
			_controllers.erase(_controllers.begin() + static_cast<int>(i - 1));
		}
	}
}

@end
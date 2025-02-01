#import "scene.hh"
#import "nes/app/application.hh"
#import "nes/app/input/input-device-keyboard.hh"
#import "nes/app/input/input-device-controller.hh"
#import "nes/common/display.hh"
#import <memory>
#import <optional>

namespace
{
    class display_impl final : public nes::display
    {
        SKSpriteNode* node_;
        NSMutableData* buffer_front_;
        NSMutableData* buffer_back_;
        nes::u8* bytes_back_;

    public:
        explicit display_impl(SKSpriteNode* node)
            : node_{ node }
            , buffer_front_{ make_buffer() }
            , buffer_back_{ make_buffer() }
            , bytes_back_{ static_cast<nes::u8*>([buffer_back_ mutableBytes]) }
        {
        }

        ~display_impl() = default;

        virtual auto switch_buffers() -> void override
        {
            auto tmp = buffer_back_;
            buffer_back_ = buffer_front_;
            buffer_front_ = tmp;
            bytes_back_ = static_cast<nes::u8*>([buffer_back_ mutableBytes]);

			auto texture = [SKTexture textureWithData:buffer_front_ size:CGSizeMake(width, height) flipped:YES];
			[texture setFilteringMode:SKTextureFilteringNearest];
			[node_ setTexture:texture];
        }

		virtual auto set(nes::u32 x, nes::u32 y, nes::rgb value) -> void override
        {
            auto const offset = (y * width + x) * 4;
            bytes_back_[offset + 0] = value.r;
            bytes_back_[offset + 1] = value.g;
            bytes_back_[offset + 2] = value.b;
            bytes_back_[offset + 3] = 255;
        }

		virtual auto get(nes::u32 x, nes::u32 y) const -> nes::rgb override
        {
            auto const offset = (y * width + x) * 4;
            auto res = nes::rgb{};
            res.r = bytes_back_[offset + 0];
            res.g = bytes_back_[offset + 1];
            res.b = bytes_back_[offset + 2];
            return res;
        }

    private:
        static auto make_buffer() -> NSMutableData*
        {
            auto const length = NSUInteger{ width * height * 4 };
            return [NSMutableData dataWithLength:length];
        }
    };

	class input_device_keyboard_impl final : public nes::app::input_device_keyboard
	{
		GCKeyboardInput* profile_;

	public:
		explicit input_device_keyboard_impl(GCKeyboardInput* profile)
			: profile_{ profile }
		{
		}

		auto set_profile(GCKeyboardInput* profile) -> void { profile_ = profile; }

		auto read_key_up() -> bool override { return [[profile_ buttonForKeyCode:GCKeyCodeUpArrow] isPressed]; }
		auto read_key_down() -> bool override { return [[profile_ buttonForKeyCode:GCKeyCodeDownArrow] isPressed]; }
		auto read_key_left() -> bool override { return [[profile_ buttonForKeyCode:GCKeyCodeLeftArrow] isPressed]; }
		auto read_key_right() -> bool override { return [[profile_ buttonForKeyCode:GCKeyCodeRightArrow] isPressed]; }
		auto read_key_a() -> bool override { return [[profile_ buttonForKeyCode:GCKeyCodeKeyZ] isPressed]; }
		auto read_key_b() -> bool override { return [[profile_ buttonForKeyCode:GCKeyCodeKeyX] isPressed]; }
		auto read_key_select() -> bool override { return [[profile_ buttonForKeyCode:GCKeyCodeSpacebar] isPressed]; }
		auto read_key_start() -> bool override { return [[profile_ buttonForKeyCode:GCKeyCodeReturnOrEnter] isPressed]; }
		auto read_key_escape() -> bool override { return [[profile_ buttonForKeyCode:GCKeyCodeEscape] isPressed]; }
	};

	class input_device_controller_impl final : public nes::app::input_device_controller
	{
		GCExtendedGamepad* profile_;

	public:
		explicit input_device_controller_impl(GCExtendedGamepad* profile)
			: profile_{ profile }
		{
		}

		auto is_reliable() const -> bool override { return true; }
		auto get_controller() const -> GCController* { return [profile_ controller]; }
		auto get_name() const -> std::string_view override { return [[get_controller() vendorName] UTF8String]; }

		auto get_index() const -> index override
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

		auto set_index(index idx) -> void override
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

		auto read_key_up() -> bool override { return [[[profile_ dpad] up] isPressed]; }
		auto read_key_down() -> bool override { return [[[profile_ dpad] down] isPressed]; }
		auto read_key_left() -> bool override { return [[[profile_ dpad] left] isPressed]; }
		auto read_key_right() -> bool override { return [[[profile_ dpad] right] isPressed]; }
		auto read_key_a() -> bool override { return [[profile_ buttonA] isPressed]; }
		auto read_key_b() -> bool override { return [[profile_ buttonB] isPressed]; }
		auto read_key_select() -> bool override { return [[profile_ leftTrigger] isPressed]; }
		auto read_key_start() -> bool override { return [[profile_ rightTrigger] isPressed]; }
	};
} // namespace

@implementation Scene
{
	std::unique_ptr<display_impl> _display;
	std::unique_ptr<input_device_keyboard_impl> _keyboard;
	std::vector<std::unique_ptr<input_device_controller_impl>> _controllers;
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

        _display = std::make_unique<display_impl>(node);
		_keyboard = std::make_unique<input_device_keyboard_impl>([[GCKeyboard coalescedKeyboard] keyboardInput]);
        _app = std::make_unique<nes::app::application>(*_display, *_keyboard);

		for (GCController* controller in [GCController controllers])
		{
			auto gamepad = [controller extendedGamepad];
			if (gamepad)
			{
				auto adapter = std::make_unique<input_device_controller_impl>(gamepad);
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
    auto deltaMicroseconds = static_cast<nes::u64>((currentTime - lastTimestamp) * 1000. * 1000.);
	deltaMicroseconds = std::min(deltaMicroseconds, nes::u64{ 50000 });
    _app->frame(std::chrono::microseconds{ deltaMicroseconds });
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
		auto adapter = std::make_unique<input_device_controller_impl>(gamepad);
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
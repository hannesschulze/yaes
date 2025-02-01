#import "scene.hh"
#import "nes/app/application.hh"
#import "nes/app/input/input-device-keyboard.hh"
#import "nes/app/input/input-device-controller.hh"
#import "nes/app/input/input-buffer.hh"
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
		GCKeyboardInput* profile_{ nil };
		nes::app::input_buffer buffer_;

	public:
		explicit input_device_keyboard_impl(GCKeyboardInput* profile)
		{
			set_profile(profile);
		}

		~input_device_keyboard_impl()
		{
			set_profile(nil);
		}

		auto set_profile(GCKeyboardInput* profile) -> void
		{
			[profile_ setKeyChangedHandler:nil];
			profile_ = profile;
			[profile_ setKeyChangedHandler:^(GCKeyboardInput*, GCControllerButtonInput*, GCKeyCode key_code, BOOL pressed) {
			  handle_key_changed(key_code, pressed);
			}];
		}

		auto read_key(nes::app::key const key) -> bool override { return buffer_.read_key(key); }
		auto poll_event() -> nes::app::input_event override { return buffer_.poll_event(); }

	private:
		auto handle_key_changed(GCKeyCode const key_code, BOOL const pressed) -> void
		{
			auto const key = convert_key_code(key_code);
			if (key)
			{
				if (pressed)
				{
					buffer_.key_down(*key);
				}
				else
				{
					buffer_.key_up(*key);
				}
			}
		}

		auto convert_key_code(GCKeyCode const key_code) -> std::optional<nes::app::key>
		{
			if (key_code == GCKeyCodeZero) { return nes::app::key::digit_0; }
			if (key_code == GCKeyCodeOne) { return nes::app::key::digit_1; }
			if (key_code == GCKeyCodeTwo) { return nes::app::key::digit_2; }
			if (key_code == GCKeyCodeThree) { return nes::app::key::digit_3; }
			if (key_code == GCKeyCodeFour) { return nes::app::key::digit_4; }
			if (key_code == GCKeyCodeFive) { return nes::app::key::digit_5; }
			if (key_code == GCKeyCodeSix) { return nes::app::key::digit_6; }
			if (key_code == GCKeyCodeSeven) { return nes::app::key::digit_7; }
			if (key_code == GCKeyCodeEight) { return nes::app::key::digit_8; }
			if (key_code == GCKeyCodeNine) { return nes::app::key::digit_9; }
			if (key_code == GCKeyCodeKeyA) { return nes::app::key::letter_a; }
			if (key_code == GCKeyCodeKeyB) { return nes::app::key::letter_b; }
			if (key_code == GCKeyCodeKeyC) { return nes::app::key::letter_c; }
			if (key_code == GCKeyCodeKeyD) { return nes::app::key::letter_d; }
			if (key_code == GCKeyCodeKeyE) { return nes::app::key::letter_e; }
			if (key_code == GCKeyCodeKeyF) { return nes::app::key::letter_f; }
			if (key_code == GCKeyCodeKeyG) { return nes::app::key::letter_g; }
			if (key_code == GCKeyCodeKeyH) { return nes::app::key::letter_h; }
			if (key_code == GCKeyCodeKeyI) { return nes::app::key::letter_i; }
			if (key_code == GCKeyCodeKeyJ) { return nes::app::key::letter_j; }
			if (key_code == GCKeyCodeKeyK) { return nes::app::key::letter_k; }
			if (key_code == GCKeyCodeKeyL) { return nes::app::key::letter_l; }
			if (key_code == GCKeyCodeKeyM) { return nes::app::key::letter_m; }
			if (key_code == GCKeyCodeKeyN) { return nes::app::key::letter_n; }
			if (key_code == GCKeyCodeKeyO) { return nes::app::key::letter_o; }
			if (key_code == GCKeyCodeKeyP) { return nes::app::key::letter_p; }
			if (key_code == GCKeyCodeKeyQ) { return nes::app::key::letter_q; }
			if (key_code == GCKeyCodeKeyR) { return nes::app::key::letter_r; }
			if (key_code == GCKeyCodeKeyS) { return nes::app::key::letter_s; }
			if (key_code == GCKeyCodeKeyT) { return nes::app::key::letter_t; }
			if (key_code == GCKeyCodeKeyU) { return nes::app::key::letter_u; }
			if (key_code == GCKeyCodeKeyV) { return nes::app::key::letter_v; }
			if (key_code == GCKeyCodeKeyW) { return nes::app::key::letter_w; }
			if (key_code == GCKeyCodeKeyX) { return nes::app::key::letter_x; }
			if (key_code == GCKeyCodeKeyY) { return nes::app::key::letter_y; }
			if (key_code == GCKeyCodeKeyZ) { return nes::app::key::letter_z; }
			if (key_code == GCKeyCodeEscape) { return nes::app::key::escape; }
			if (key_code == GCKeyCodeHyphen) { return nes::app::key::dash; }
			if (key_code == GCKeyCodeEqualSign) { return nes::app::key::equal; }
			if (key_code == GCKeyCodeDeleteOrBackspace) { return nes::app::key::backspace; }
			if (key_code == GCKeyCodeTab) { return nes::app::key::tab; }
			if (key_code == GCKeyCodeReturnOrEnter) { return nes::app::key::enter; }
			if (key_code == GCKeyCodeSemicolon) { return nes::app::key::semicolon; }
			if (key_code == GCKeyCodeQuote) { return nes::app::key::apostrophe; }
			if (key_code == GCKeyCodeBackslash) { return nes::app::key::backslash; }
			if (key_code == GCKeyCodeComma) { return nes::app::key::comma; }
			if (key_code == GCKeyCodePeriod) { return nes::app::key::period; }
			if (key_code == GCKeyCodeSlash) { return nes::app::key::slash; }
			if (key_code == GCKeyCodeSpacebar) { return nes::app::key::space; }
			if (key_code == GCKeyCodeCapsLock) { return nes::app::key::caps_lock; }
			if (key_code == GCKeyCodeOpenBracket) { return nes::app::key::bracket_open; }
			if (key_code == GCKeyCodeCloseBracket) { return nes::app::key::bracket_close; }
			if (key_code == GCKeyCodeLeftControl) { return nes::app::key::control_left; }
			if (key_code == GCKeyCodeRightControl) { return nes::app::key::control_right; }
			if (key_code == GCKeyCodeLeftShift) { return nes::app::key::shift_left; }
			if (key_code == GCKeyCodeRightShift) { return nes::app::key::shift_right; }
			if (key_code == GCKeyCodeLeftAlt) { return nes::app::key::alt_left; }
			if (key_code == GCKeyCodeRightAlt) { return nes::app::key::alt_right; }
			if (key_code == GCKeyCodeLeftArrow) { return nes::app::key::arrow_left; }
			if (key_code == GCKeyCodeRightArrow) { return nes::app::key::arrow_right; }
			if (key_code == GCKeyCodeUpArrow) { return nes::app::key::arrow_up; }
			if (key_code == GCKeyCodeDownArrow) { return nes::app::key::arrow_down; }
			if (key_code == GCKeyCodeF1) { return nes::app::key::f1; }
			if (key_code == GCKeyCodeF2) { return nes::app::key::f2; }
			if (key_code == GCKeyCodeF3) { return nes::app::key::f3; }
			if (key_code == GCKeyCodeF4) { return nes::app::key::f4; }
			if (key_code == GCKeyCodeF5) { return nes::app::key::f5; }
			if (key_code == GCKeyCodeF6) { return nes::app::key::f6; }
			if (key_code == GCKeyCodeF7) { return nes::app::key::f7; }
			if (key_code == GCKeyCodeF8) { return nes::app::key::f8; }
			if (key_code == GCKeyCodeF9) { return nes::app::key::f9; }
			if (key_code == GCKeyCodeF10) { return nes::app::key::f10; }
			if (key_code == GCKeyCodeF11) { return nes::app::key::f11; }
			if (key_code == GCKeyCodeF12) { return nes::app::key::f12; }

			return std::nullopt;
		}
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

		auto read_buttons() -> nes::sys::button_mask override
		{
			auto res = nes::sys::button_mask{};
			if ([[[profile_ dpad] up] isPressed]) { res.add(nes::sys::buttons::up); }
			if ([[[profile_ dpad] down] isPressed]) { res.add(nes::sys::buttons::down); }
			if ([[[profile_ dpad] left] isPressed]) { res.add(nes::sys::buttons::left); }
			if ([[[profile_ dpad] right] isPressed]) { res.add(nes::sys::buttons::right); }
			if ([[profile_ buttonA] isPressed]) { res.add(nes::sys::buttons::a); }
			if ([[profile_ buttonB] isPressed]) { res.add(nes::sys::buttons::b); }
			if ([[profile_ leftTrigger] isPressed]) { res.add(nes::sys::buttons::select); }
			if ([[profile_ rightTrigger] isPressed]) { res.add(nes::sys::buttons::start); }
			return res;
		}
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
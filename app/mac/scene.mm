#import "scene.hh"
#import "nes/app/application.hh"
#import "nes/app/input/input-device-keyboard.hh"
#import "nes/app/input/input-device-controller.hh"
#import "nes/app/input/input-buffer.hh"
#import "nes/app/file-browser.hh"
#import "nes/common/display.hh"
#import "nes/common/path-view.hh"
#import <memory>
#import <iostream>
#import <optional>
#import <fcntl.h>
#import <unistd.h>
#import <termios.h>
#import <dirent.h>
#import <sys/ioctl.h>

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

	class input_device_controller_serial : public nes::app::input_device_controller
	{
		char const* path_;
		int fd_{ 0 };
		nes::sys::button_mask current_{};
		index index_{ index::unused };

	public:
		explicit input_device_controller_serial(char const* path)
			: path_{ path }
		{
			fd_ = open(path, O_RDONLY | O_NOCTTY | O_NDELAY | O_NONBLOCK);
			if (fd_ == -1)
			{
				perror("open controller");
				return;
			}

			if (!setup()) { close(); }
		}

		~input_device_controller_serial() override
		{
			close();
		}

		auto read_buttons() -> nes::sys::button_mask override
		{
			if (fd_ == -1) { return nes::sys::button_mask{}; }

			// Poll for any updates until we can't read anymore.
			while (true)
			{
				auto buf = nes::u8{};
				auto res = read(fd_, &buf, 1);
				if (res == -1)
				{
					if (errno == EAGAIN || errno == EWOULDBLOCK) { break; }

					perror("read from controller");
					close();
					break;
				}

				current_ = nes::sys::button_mask::from_raw_value(buf);
			}

			return current_;
		}

		auto is_reliable() const -> bool override { return fd_ != -1; }
		auto get_name() const -> std::string_view override { return path_; }
		auto set_index(index const index) -> void override { index_ = index; }
		auto get_index() const -> index override { return index_; }

	private:
		auto close() -> void
		{
			if (fd_ != -1)
			{
				::close(fd_);
				fd_ = -1;
			}
		}

		auto setup() -> bool
		{
			if (ioctl(fd_, TIOCEXCL) == -1)
			{
				perror("ioctl for controller");
				return false;
			}

			auto attr = termios{};
			if (tcgetattr(fd_, &attr) == -1)
			{
				perror("tcgetattr for controller");
				return false;
			}

			cfsetispeed(&attr, B57600);
			cfsetospeed(&attr, B57600);
			attr.c_cflag = attr.c_cflag | CLOCAL | CREAD;
			attr.c_cflag = (attr.c_cflag & static_cast<tcflag_t>(~CSIZE)) | CS8;

			if (tcsetattr(fd_, TCSANOW, &attr) == -1)
			{
				perror("tcsetattr for controller");
				return false;
			}

			return true;
		}
	};

    class file_browser_impl final : public nes::app::file_browser
    {
		std::vector<std::string> components_;
		std::vector<std::string_view> component_views_;
		DIR* it_{ nullptr };
		nes::u32 it_pos_{ 0 };

	public:
		explicit file_browser_impl()
		{
			if (chdir("/") == -1)
			{
				perror("chdir");
				std::abort();
			}

			it_ = opendir(".");
			if (!it_)
			{
				perror("opendir");
				std::abort();
			}
		}

		~file_browser_impl() override
		{
			closedir(it_);
		}

		auto get_path() const -> nes::path_view override
		{
			return nes::path_view{ component_views_.data(), static_cast<nes::u32>(component_views_.size()) };
		}

		auto read_item_count() -> nes::u32 override
		{
			while (read_next())
				;
			return it_pos_;
		}

		auto read_items(nes::u32 offset, item* buffer, nes::u32 buffer_size) -> nes::u32 override
		{
			seek(offset);
			auto count = nes::u32{ 0 };
			for (auto i = nes::u32{ 0 }; i < buffer_size; ++i)
			{
				auto item = read_next();
				if (!item) { break; }
				buffer[count++] = *item;
			}

			return count;
		}

		auto navigate_up() -> void override
		{
			if (components_.empty()) { return; }

			components_.erase(components_.begin() + static_cast<std::ptrdiff_t>(components_.size() - 1));
			component_views_.erase(component_views_.begin() + static_cast<std::ptrdiff_t>(component_views_.size() - 1));
			auto path = std::string{};
			for (auto const component : get_path())
			{
				path += "/";
				path += component;
			}
			path += "/";

			if (chdir(path.c_str()) == -1)
			{
				perror("chdir");
				std::abort();
			}

			reopen_dir();
		}

		auto navigate(std::string_view const item) -> void override
		{
			auto item_copy = std::string{ item };
			if (chdir(item_copy.c_str()) == -1)
			{
				perror("chdir");
				std::abort();
			}

			components_.emplace_back(std::move(item_copy));
			component_views_.clear();
			for (auto& str : components_) { component_views_.emplace_back(str); }

			reopen_dir();
		}

		auto load(std::string_view const item, nes::u8* buffer, nes::u32 const buffer_size) -> nes::u32 override
		{
			auto item_copy = std::string{ item };
			auto const fd = open(item_copy.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY | O_NONBLOCK);
			if (fd == -1)
			{
				perror("open");
				std::abort();
			}

			auto const res = read(fd, buffer, buffer_size);
			if (res < 0)
			{
				perror("read");
				std::abort();
			}

			auto remaining = char{};
			if (read(fd, &remaining, 1) != 0)
			{
				std::cerr << "Expected EOF" << std::endl;
				std::abort();
			}

			close(fd);

			return static_cast<nes::u32>(res);
		}

	private:
		auto reopen_dir() -> void
		{
			closedir(it_);
			it_ = opendir(".");
			if (!it_)
			{
				perror("opendir");
				std::abort();
			}
			it_pos_ = 0;
		}

		auto seek(nes::u32 const position) -> void
		{
			if (it_pos_ > position)
			{
				reopen_dir();
			}

			while (it_pos_ < position)
			{
				if (!read_next()) { break; }
			}
		}

		auto read_next() -> std::optional<item>
		{
			if (!it_) { return std::nullopt; }

			while (true)
			{
				errno = 0;
				auto entry = readdir(it_);
				if (!entry)
				{
					if (errno)
					{
						perror("readdir");
						std::abort();
					}

					return std::nullopt;
				}

				auto const name = std::string_view{ entry->d_name };
				if (name.length() > max_name_length) { continue; }
				if (name[0] == '.') { continue; }

				auto type = item_type::directory;
				if (entry->d_type == DT_DIR) { type = item_type::directory; }
				else if (entry->d_type == DT_REG) { type = item_type::file; }
				else { continue; }

				it_pos_ += 1;
				return item{ type, name };
			}
		}
    };
} // namespace

@implementation Scene
{
	std::unique_ptr<display_impl> _display;
    std::unique_ptr<file_browser_impl> _fileBrowser;
	std::unique_ptr<input_device_keyboard_impl> _keyboard;
    std::unique_ptr<input_device_controller_serial> _serialController;
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
        _fileBrowser = std::make_unique<file_browser_impl>();
		_keyboard = std::make_unique<input_device_keyboard_impl>([[GCKeyboard coalescedKeyboard] keyboardInput]);
        _serialController = std::make_unique<input_device_controller_serial>("/dev/cu.usbserial-FTB6SPL3");
        _app = std::make_unique<nes::app::application>(*_display, *_keyboard, *_fileBrowser);

        _app->add_controller(*_serialController);

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
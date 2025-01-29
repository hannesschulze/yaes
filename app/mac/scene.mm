#import "scene.hh"
#import "nes/nes.hh"
#import "nes/util/display.hh"
#import <memory>
#import <optional>
#import <iostream>

namespace
{
    class display_impl : public nes::display
    {
        SKSpriteNode* node_;
        NSMutableData* buffer_front_;
        NSMutableData* buffer_back_;
        std::uint8_t* bytes_back_;

    public:
        explicit display_impl(SKSpriteNode* node)
            : node_{ node }
            , buffer_front_{ make_buffer() }
            , buffer_back_{ make_buffer() }
            , bytes_back_{ static_cast<std::uint8_t*>([buffer_back_ mutableBytes]) }
        {
        }

        ~display_impl() = default;

        virtual auto switch_buffers() -> void override
        {
            auto tmp = buffer_back_;
            buffer_back_ = buffer_front_;
            buffer_front_ = tmp;
            bytes_back_ = static_cast<std::uint8_t*>([buffer_back_ mutableBytes]);

			auto texture = [SKTexture textureWithData:buffer_front_ size:CGSizeMake(width, height) flipped:YES];
			[texture setFilteringMode:SKTextureFilteringNearest];
			[node_ setTexture:texture];
        }

		virtual auto set(unsigned x, unsigned y, nes::rgb value) -> void override
        {
            auto const offset = (y * width + x) * 4;
            bytes_back_[offset + 0] = value.r;
            bytes_back_[offset + 1] = value.g;
            bytes_back_[offset + 2] = value.b;
            bytes_back_[offset + 3] = 255;
        }

		virtual auto get(unsigned x, unsigned y) const -> nes::rgb override
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
} // namespace

@implementation Scene
{
	std::unique_ptr<display_impl> _display;
	std::unique_ptr<nes::nes> _nes;
    std::optional<NSTimeInterval> _lastTimestamp;
}

- (instancetype)initWithFilePath:(NSString*)filePath
{
    self = [super initWithSize:CGSizeMake(nes::display::width, nes::display::height)];
    if (self)
    {
        auto node = [[SKSpriteNode alloc] init];
        [node setAnchorPoint:CGPointZero];
        [node setSize:[self size]];

        [self setScaleMode:SKSceneScaleModeAspectFit];
        [self addChild:node];

        auto cartridge = nes::cartridge::from_file([filePath UTF8String]);
        if (cartridge.get_status() != nes::status::success)
        {
            std::cerr << "Unable to load cartridge: " << to_string(cartridge.get_status()) << std::endl;
            std::abort();
        }

        _display = std::make_unique<display_impl>(node);
        _nes = std::make_unique<nes::nes>(std::move(cartridge), *self->_display);
    }
    return self;
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
	_nes->ref_controller_1().set_pressed(_primaryController ? [_primaryController readPressedButtons] : nes::button_mask{});
	_nes->ref_controller_2().set_pressed(_secondaryController ? [_secondaryController readPressedButtons] : nes::button_mask{});

    auto const lastTimestamp = _lastTimestamp.value_or(currentTime);
    _lastTimestamp = currentTime;
    auto deltaMicroseconds = static_cast<std::uint64_t>((currentTime - lastTimestamp) * 1000. * 1000.);
	deltaMicroseconds = std::min(deltaMicroseconds, std::uint64_t{ 50000 });
    _nes->step(std::chrono::microseconds{ deltaMicroseconds });

    if (_nes->get_status() != nes::status::success)
    {
        std::cerr << "Invalid state: " << to_string(_nes->get_status()) << std::endl;
        std::abort();
    }
}

@end
#import "scene.hh"
#import "nes/nes.hh"
#import "nes/util/display.hh"
#import "nes/util/rgb.hh"
#import <memory>
#import <optional>

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
        }

        auto actually_switch_buffers() -> void
        {
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
    std::unique_ptr<display_impl> display_;
    std::unique_ptr<nes::nes> nes_;
    std::optional<NSTimeInterval> last_time_;
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

        self->display_ = std::make_unique<display_impl>(node);
        self->nes_ = std::make_unique<nes::nes>(std::move(cartridge), *self->display_);
    }
    return self;
}

- (void)keyDown:(NSEvent*)event
{
    auto& controller_1 = self->nes_->get_controller_1();

    switch ([event keyCode])
    {
        case 0x7e: // up arrow
            controller_1.get_pressed().add(nes::buttons::up);
            break;
        case 0x7d: // down arrow
            controller_1.get_pressed().add(nes::buttons::down);
            break;
        case 0x7b: // left arrow
            controller_1.get_pressed().add(nes::buttons::left);
            break;
        case 0x7c: // right arrow
            controller_1.get_pressed().add(nes::buttons::right);
            break;
        case 0x6: // "Z"
            controller_1.get_pressed().add(nes::buttons::a);
            break;
        case 0x7: // "X"
            controller_1.get_pressed().add(nes::buttons::b);
            break;
        case 0x24: // return
            controller_1.get_pressed().add(nes::buttons::start);
            break;
        case 0x31: // space
            controller_1.get_pressed().add(nes::buttons::select);
            break;
        default:
            [super keyDown:event];
            break;
    }
}

- (void)keyUp:(NSEvent*)event
{
    auto& controller_1 = self->nes_->get_controller_1();

    switch ([event keyCode])
    {
        case 0x7e: // up arrow
            controller_1.get_pressed().remove(nes::buttons::up);
            break;
        case 0x7d: // down arrow
            controller_1.get_pressed().remove(nes::buttons::down);
            break;
        case 0x7b: // left arrow
            controller_1.get_pressed().remove(nes::buttons::left);
            break;
        case 0x7c: // right arrow
            controller_1.get_pressed().remove(nes::buttons::right);
            break;
        case 0x6: // "Z"
            controller_1.get_pressed().remove(nes::buttons::a);
            break;
        case 0x7: // "X"
            controller_1.get_pressed().remove(nes::buttons::b);
            break;
        case 0x24: // return
            controller_1.get_pressed().remove(nes::buttons::start);
            break;
        case 0x31: // space
            controller_1.get_pressed().remove(nes::buttons::select);
            break;
        default:
            [super keyUp:event];
            break;
    }
}

- (void)update:(NSTimeInterval)currentTime
{
    auto const last_time = self->last_time_.value_or(currentTime);
    last_time_ = currentTime;
    auto delta_us = static_cast<std::uint64_t>((currentTime - last_time) * 1000. * 1000.);
    delta_us = std::min(delta_us, std::uint64_t{ 50000 });
    self->nes_->step(std::chrono::microseconds{ delta_us });
    self->display_->actually_switch_buffers();
}

@end
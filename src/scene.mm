#import "scene.hh"
#import "nes/nes.hh"
#import "nes/display.hh"
#import "nes/util/rgb.hh"
#import <memory>

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

            auto texture = [SKTexture textureWithData:buffer_front_ size:CGSizeMake(width, height)];
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

- (void)update:(NSTimeInterval)currentTime
{
    [super update:currentTime];
}

@end
#import "impl/display-spritekit.hh"

namespace nes::app::mac
{
    namespace
    {
        auto make_buffer() -> NSMutableData*
		{
			auto const length = NSUInteger{ display::width * display::height * 4 };
			return [NSMutableData dataWithLength:length];
		}
    } // namespace

	display_spritekit::display_spritekit(SKSpriteNode* node)
		: node_{ node }
		, buffer_front_{ make_buffer() }
		, buffer_back_{ make_buffer() }
		, bytes_back_{ static_cast<u8*>([buffer_back_ mutableBytes]) }
	{
	}

	auto display_spritekit::switch_buffers() -> void
	{
		std::swap(buffer_back_, buffer_front_);
		bytes_back_ = static_cast<u8*>([buffer_back_ mutableBytes]);

		auto texture = [SKTexture textureWithData:buffer_front_ size:CGSizeMake(width, height) flipped:YES];
		[texture setFilteringMode:SKTextureFilteringNearest];
		[node_ setTexture:texture];
	}

	auto display_spritekit::set(u32 const x, u32 const y, rgb const value) -> void
	{
		auto const offset = (y * width + x) * 4;
		bytes_back_[offset + 0] = value.r;
		bytes_back_[offset + 1] = value.g;
		bytes_back_[offset + 2] = value.b;
		bytes_back_[offset + 3] = 255;
	}

	auto display_spritekit::get(u32 const x, u32 const y) const -> rgb
	{
		auto const offset = (y * width + x) * 4;
		auto res = rgb{};
		res.r = bytes_back_[offset + 0];
		res.g = bytes_back_[offset + 1];
		res.b = bytes_back_[offset + 2];
		return res;
	}
} // namespace nes::app::mac
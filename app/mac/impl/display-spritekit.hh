#pragma once

#import "nes/common/display.hh"

#import <SpriteKit/SpriteKit.h>

namespace nes::app::mac
{
	/// Display implementation rendering contents into a SpriteKit node.
	class display_spritekit final : public display
	{
		SKSpriteNode* node_;
		NSMutableData* buffer_front_;
		NSMutableData* buffer_back_;
		u8* bytes_back_;
		u8* bytes_front_;

	public:
		explicit display_spritekit(SKSpriteNode* node);

		auto switch_buffers() -> void override;
		auto set(u32 x, u32 y, rgb value) -> void override;
	};
} // namespace nes::app::mac
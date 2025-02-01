#pragma once

#include "nes/common/types.hh"

namespace nes::app
{
	struct image_tile;

	/// Non-owning copy of a 2-dimensional block of image tiles, forming an image.
	///
	/// This is similar in spirit to a string_view, but for image tiles.
	class image_view
	{
		image_tile const* data_{ nullptr };
		u32 width_{ 0 };
		u32 height_{ 0 };

	public:
		explicit image_view() = default;

		explicit image_view(image_tile const* data, u32 const width, u32 const height)
			: data_{ data }
			, width_{ width }
			, height_{ height }
		{
		}

		auto get_data() const -> image_tile const* { return data_; }
		auto get_width() const -> u32 { return width_; }
		auto get_height() const -> u32 { return height_; }
		auto get(u32 const x, u32 const y) const -> image_tile const& { return data_[y * height_ + x]; }
	};
} // namespace nes::app
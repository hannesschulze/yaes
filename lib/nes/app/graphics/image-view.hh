#pragma once

namespace nes::app
{
	struct image_tile;

	/// Non-owning copy of a 2-dimensional block of image tiles, forming an image.
	///
	/// This is similar in spirit to a string_view, but for image tiles.
	class image_view
	{
		image_tile const* data_{ nullptr };
		unsigned width_{ 0 };
		unsigned height_{ 0 };

	public:
		explicit image_view() = default;

		explicit image_view(image_tile const* data, unsigned const width, unsigned const height)
			: data_{ data }
			, width_{ width }
			, height_{ height }
		{
		}

		auto get_data() const -> image_tile const* { return data_; }
		auto get_width() const -> unsigned { return width_; }
		auto get_height() const -> unsigned { return height_; }
		auto get(unsigned const x, unsigned const y) const -> image_tile const& { return data_[y * height_ + x]; }
	};
} // namespace nes::app
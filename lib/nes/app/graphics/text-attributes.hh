#pragma once

#include "nes/common/types.hh"

namespace nes::app
{
	enum class ellipsize_mode
	{
		clip,
		start,
		middle,
		end,
	};

	enum class text_alignment
	{
		left,
		center,
		right,
	};

	class text_attributes
	{
		u32 max_width_{ 32 };
		ellipsize_mode ellipsize_mode_{ ellipsize_mode::clip };
		text_alignment alignment_{ text_alignment::left };

	public:
		explicit text_attributes() = default;

		auto get_max_width() const -> u32 { return max_width_; }
		auto get_ellipsize_mode() const -> ellipsize_mode { return ellipsize_mode_; }
		auto get_alignment() const -> text_alignment { return alignment_; }

		auto set_max_width(u32 const v) -> text_attributes& { max_width_ = v; return *this; }
		auto set_ellipsize_mode(ellipsize_mode const v) -> text_attributes& { ellipsize_mode_ = v; return *this; }
		auto set_alignment(text_alignment const v) -> text_attributes& { alignment_ = v; return *this; }
	};
} // namespace nes::app
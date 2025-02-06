#pragma once

#include "nes/common/types.hh"

namespace nes
{
	/// Counts frames.
	class fps_counter
	{
		static constexpr auto buffer_size = u32{ 20 };

		u32 frame_times_[buffer_size]{}; // The time until the next frame for the last <buffer_size> frames.
		u32 buffer_position_{ 0 }; // Position in the ring buffer.
		u32 buffered_fps_{ 0 }; // Delay updates to FPS to one update per round in the ring buffer.

	public:
		explicit fps_counter() = default;

		auto frame(u32 elapsed_time_us) -> void;
		auto get_fps() const -> u32 { return buffered_fps_; }
	};
} // namespace nes
#include "nes/common/fps-counter.hh"

namespace nes
{
	auto fps_counter::frame(std::chrono::microseconds const elapsed_time) -> void
	{
		if (elapsed_time.count() <= 0) { return; }

		if (buffer_position_ == 0)
		{
			// Update fps count.
			auto sum = u32{ 0 };
			for (auto i = u32{ 0 }; i < buffer_size; ++i) { sum += frame_times_[i]; }
			if (sum > 0)
			{
				buffered_fps_ = (buffer_size * 1000000) / sum;
			}
			else
			{
				buffered_fps_ = 0;
			}
		}

		// Add the frame time to the buffer, replacing any existing content in the ring buffer.
		auto const frame_time = static_cast<u32>(elapsed_time.count());
		frame_times_[buffer_position_] = frame_time;
		buffer_position_ = (buffer_position_ + 1) % buffer_size;
	}
} // namespace nes
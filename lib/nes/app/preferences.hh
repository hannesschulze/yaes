#pragma once

namespace nes::app
{
	/// Manages application preferences.
	class preferences
	{
		bool fps_counter_{ false };

	public:
		explicit preferences() = default;

		preferences(preferences const&) = delete;
		preferences(preferences&&) = delete;
		auto operator=(preferences const&) -> preferences& = delete;
		auto operator=(preferences&&) -> preferences& = delete;

		auto get_fps_counter() const -> bool { return fps_counter_; }
		auto set_fps_counter(bool const v) -> void { fps_counter_ = v; }
	};
} // namespace nes::app
#pragma once

#include "nes/common/types.hh"
#include "nes/common/containers/string-view.hh"
#include "nes/app/ui/screen.hh"
#include "nes/common/containers/string-view.hh"

namespace nes::app
{
	class input_device_keyboard;
	class file_browser;

	/// An popup showing the contents of a file.
	class screen_file_viewer final : public screen
	{
		static constexpr auto line_length = u32{ 24 };
		static constexpr auto buffer_length = u32{ 1024 };
		static constexpr auto max_line_count = u32{ 16 };

		input_device_keyboard& keyboard_;
		file_browser& file_browser_;
		u8 buffer_[buffer_length];
		string_view lines_[max_line_count];
		u32 line_count_{ 0 };

	public:
		explicit screen_file_viewer(input_device_keyboard&, file_browser&);

		auto load(string_view file_name) -> status;

		auto render(renderer&) -> void override;
		auto process_events() -> action override;
	};
} // namespace nes::app

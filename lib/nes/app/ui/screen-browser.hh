#pragma once

#include "nes/app/ui/screen.hh"
#include "nes/app/ui/selection.hh"
#include "nes/app/file-browser.hh"

namespace nes::app
{
	class input_device_keyboard;

	/// An interface which allows the user to browse the filesystem for a game.
	class screen_browser final : public screen
	{
		static constexpr auto page_size = u32{ 8 };

		struct item
		{
			bool is_parent{ false };
			file_browser::item data{ file_browser::item_type::directory, "" };
		};

		class selection_impl final : public selection<item, page_size>
		{
			file_browser& file_browser_;

		public:
			explicit selection_impl(file_browser& file_browser)
				: selection{ 2, 5, 28 }
				, file_browser_{ file_browser }
			{
			}

		protected:
			auto render_item(renderer&, item const&, i32 x, i32 y, u32 width, color) const -> void override;
			auto load_page(item(&)[page_size], u32 page) -> u32 override;
		};

		input_device_keyboard& keyboard_;
		file_browser& file_browser_;
		selection_impl selection_;
		u32 navigation_steps_{ 0 };

	public:
		explicit screen_browser(input_device_keyboard&, file_browser&);

		auto render(renderer&) -> void override;
		auto process_events() -> action override;

	private:
		auto refresh() -> void;
	};
} // namespace nes::app
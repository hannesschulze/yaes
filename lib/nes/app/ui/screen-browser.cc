#include "nes/app/ui/screen-browser.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/graphics/tiles/icons.hh"
#include "nes/app/input/input-device-keyboard.hh"
#include "nes/common/path-view.hh"

namespace nes::app
{
	auto screen_browser::selection_impl::render_item(
		renderer& renderer, item const& item, i32 const x, i32 const y, u32 const width,
		color const color) const -> void
	{
		switch (item.data.get_type())
		{
			case file_browser::item_type::file: renderer.render_mask_tile(x, y, tiles::icon_cartridge, color); break;
			case file_browser::item_type::directory: renderer.render_mask_tile(x, y, tiles::icon_directory, color); break;
		}

		renderer.render_text(x + 2, y, item.data.get_name(), color, text_attributes{}
			.set_ellipsize_mode(ellipsize_mode::end)
			.set_max_width(width - 2));
	}

	auto screen_browser::selection_impl::load_page(item (&items)[page_size], u32 const page) -> u32
	{
		auto count = u32{ 0 };
		auto position = page * page_size;
		if (!file_browser_.get_path().is_empty())
		{
			if (page == 0)
			{
				items[count].is_parent = true;
				items[count].data = file_browser::item{ file_browser::item_type::directory, "[Parent Directory]" };
				count += 1;
			}
			else
			{
				// First page has been missing an item, offset by one.
				position -= 1;
			}
		}

		file_browser_.seek(position);
		while (count < page_size)
		{
			auto item = file_browser::item{};
			if (!file_browser_.read_next(&item)) { break; }

			items[count].is_parent = false;
			items[count].data = item;
			count += 1;
		}

		return count;
	}

	screen_browser::screen_browser(input_device_keyboard& keyboard, file_browser& file_browser)
		: keyboard_{ keyboard }
		, file_browser_{ file_browser }
		, selection_{ file_browser }
	{
		refresh();
	}

	auto screen_browser::render(renderer& renderer) -> void
	{
		renderer.render_fill(color::background_primary);
		renderer.render_rect(0, 0, 32, 3, color::background_secondary);

		auto const page_length = renderer.render_text_format(
			30, 1, color::foreground_secondary, text_attributes{}.set_alignment(text_alignment::right),
			"Page {}/{}", selection_.get_current_page() + 1, selection_.get_page_count());

		renderer.render_text(
			2, 1, file_browser_.get_path().get_path(), color::foreground_primary,
			text_attributes{}.set_max_width(26 - page_length).set_ellipsize_mode(ellipsize_mode::middle));

		selection_.render(renderer);
	}

	auto screen_browser::process_events() -> action
	{
		while (auto const event = keyboard_.poll_event())
		{
			if (event == input_event::key_down(key::space))
			{
				selection_.go_next();
			}
			else if (event == input_event::key_down(key::arrow_up))
			{
				selection_.go_up();
			}
			else if (event == input_event::key_down(key::arrow_down))
			{
				selection_.go_down();
			}
			else if (event == input_event::key_down(key::backspace))
			{
				if (navigation_steps_ > 0)
				{
					if (auto const s = file_browser_.navigate_up(); s != status::success)
					{
						return action::show_error(s);
					}
					refresh();
					navigation_steps_ -= 1;
				}
				else
				{
					return action::go_to_title();
				}
			}
			else if (event == input_event::key_down(key::enter))
			{
				auto const selected = selection_.get_selected();
				if (selected)
				{
					if (selected->is_parent)
					{
						if (auto const s = file_browser_.navigate_up(); s != status::success)
						{
							return action::show_error(s);
						}
						refresh();
						if (navigation_steps_ > 0) { navigation_steps_ -= 1; }
					}
					else if (selected->data.get_type() == file_browser::item_type::directory)
					{
						if (auto const s = file_browser_.navigate(selected->data.get_name()); s != status::success)
						{
							return action::show_error(s);
						}
						refresh();
						navigation_steps_ += 1;
					}
					else if (selected->data.get_type() == file_browser::item_type::file)
					{
						return action::launch_game(selected->data.get_name());
					}
				}
			}
		}

		return action{};
	}

	auto screen_browser::refresh() -> void
	{
		file_browser_.seek(0);
		auto count = u32{ 0 };
		while (file_browser_.read_next(nullptr)) { count += 1; }
		// Round up to get page count.
		auto const page_count = (count + (page_size - 1)) / page_size;
		// Always show at least one page.
		selection_.set_page_count(std::max(u32{ 1 }, page_count));
	}
} // namespace nes::app
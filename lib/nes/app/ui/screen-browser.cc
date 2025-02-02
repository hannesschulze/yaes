#include "nes/app/ui/screen-browser.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/graphics/tiles/icons.hh"
#include "nes/app/input/input-device-keyboard.hh"
#include "nes/common/string-builder.hh"
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

	auto screen_browser::selection_impl::load_page(item (&items)[8], u32 const page) -> u32
	{
		auto offset = u32{ 0 };
		if (page == 0 && file_browser_.get_path().get_component_count() > 0)
		{
			items[0].is_parent = true;
			items[0].data = file_browser::item{ file_browser::item_type::directory, "[Parent Directory]" };
			offset = 1;
		}
		file_browser::item buffer[8];
		auto const count = file_browser_.read_items(page * 8, buffer, 8 - offset);
		for (auto i = u32{ 0 }; i < count; ++i)
		{
			items[i + offset].is_parent = false;
			items[i + offset].data = buffer[i];
		}
		return count + offset;
	}

	screen_browser::screen_browser(input_device_keyboard& keyboard, file_browser& file_browser)
		: keyboard_{ keyboard }
		, file_browser_{ file_browser }
		, selection_{ file_browser }
	{
		selection_.set_page_count(std::max(u32{ 1 }, (file_browser_.read_item_count() + 7) / 8));
	}

	auto screen_browser::render(renderer& renderer) -> void
	{
		renderer.render_fill(color::background_primary);
		renderer.render_rect(0, 0, 32, 3, color::background_secondary);

		auto const page_length = renderer.render_text_format(
			30, 1, color::foreground_secondary, text_attributes{}.set_alignment(text_alignment::right),
			"Page {}/{}", selection_.get_current_page() + 1, selection_.get_page_count());

		char path_buffer[32];
		auto path_builder = string_builder{ path_buffer, sizeof(path_buffer) };
		for (auto const component : file_browser_.get_path())
		{
			path_builder.append_format("> {} ", component);
		}
		path_builder.append_string(">");
		renderer.render_text(
			2, 1, path_builder.get_result(), color::foreground_primary,
			text_attributes{}.set_max_width(26 - page_length).set_ellipsize_mode(ellipsize_mode::end));

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
				return action::go_to_title();
			}
			else if (event == input_event::key_down(key::enter))
			{
				auto selected = selection_.get_selected();
				if (selected)
				{
					if (selected->is_parent)
					{
						file_browser_.navigate_up();
						selection_.set_page_count(std::max(u32{ 1 }, (file_browser_.read_item_count() + 7) / 8));
					}
					else
					{
						switch (selected->data.get_type())
						{
							case file_browser::item_type::directory:
								file_browser_.navigate(selected->data.get_name());
								selection_.set_page_count(std::max(u32{ 1 }, (file_browser_.read_item_count() + 7) / 8));
								break;
							case file_browser::item_type::file:
								return action::launch_game(selected->data.get_name());
						}
					}
				}
			}
		}

		return action{};
	}
} // namespace nes::app
#pragma once

#include "nes/app/ui/screen.hh"
#include "nes/app/ui/selection.hh"

namespace nes::app
{
	class input_device_keyboard;

	/// The main title screen for the emulator, used as an entrypoint for the UI.
	///
	/// Presents options to load games, adjust settings, and more.
	class screen_title final : public screen
	{
		enum class item
		{
			load_game,
			settings,
			help,
		};

		class selection_impl final : public selection<item, 3>
		{
		public:
			using selection::selection;

		protected:
			auto render_item(renderer&, item const&, i32 x, i32 y, u32 width, color) const -> void override;
			auto load_page(item(&)[3], u32 page) -> u32 override;
		};

		input_device_keyboard& keyboard_;
		selection_impl selection_{ 10, 16, 11 };

	public:
		explicit screen_title(input_device_keyboard&);

		auto render(renderer&) -> void override;
		auto process_events() -> action override;
	};
} // namespace nes::app
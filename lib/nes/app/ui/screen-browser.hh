#pragma once

#include "nes/app/ui/screen.hh"
#include "nes/app/ui/selection.hh"

namespace nes::app
{
	class input_device_keyboard;

	/// An interface which allows the user to browse the filesystem for a game.
	class screen_browser final : public screen
	{
		class selection_impl final : public selection<i32, 5>
		{
		public:
			using selection::selection;

		protected:
			auto render_item(renderer&, i32 const&, i32 x, i32 y, u32 width, color) const -> void override;
			auto load_page(i32(&)[5], u32 page) -> u32 override;
		};

		input_device_keyboard& keyboard_;
		selection_impl selection_{ 2, 5, 28 };

	public:
		explicit screen_browser(input_device_keyboard&);

		auto render(renderer&) -> void override;
		auto process_events() -> action override;
	};
} // namespace nes::app
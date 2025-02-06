#pragma once

#include "nes/app/ui/screen.hh"
#include "nes/app/ui/selection.hh"

namespace nes::app
{
	class input_manager;
	class preferences;

	/// Screen used to configure various emulator options.
	class screen_settings final : public screen
	{
		static constexpr auto page_size = u32{ 8 };

		enum class item
		{
			controller_1,
			controller_2,
			fps_counter,
		};

		class selection_impl final : public selection<item, page_size>
		{
			input_manager& input_manager_;
			preferences& preferences_;

		public:
			explicit selection_impl(input_manager& input_manager, preferences& preferences)
				: selection{ 2, 5, 28 }
				, input_manager_{ input_manager }
				, preferences_{ preferences }
			{
			}

		protected:
			auto render_item(renderer&, item const&, i32 x, i32 y, u32 width, color) const -> void override;
			auto load_page(item(&)[page_size], u32 page) -> u32 override;
		};

		input_manager& input_manager_;
		preferences& preferences_;
		selection_impl selection_;

	public:
		explicit screen_settings(input_manager&, preferences&);

		auto render(renderer&) -> void override;
		auto process_events() -> action override;
	};
} // namespace nes::app
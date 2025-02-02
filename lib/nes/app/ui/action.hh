#pragma once

#include <string_view>

namespace nes::app
{
	/// An action for the application to process after a scene has handled its events.
	class action
	{
	public:
		enum class type
		{
			none,
			go_to_title,
			go_to_browser,
			go_to_settings,
			go_to_help,
			launch_game,
		};

	private:
		type type_{ type::none };
		std::string_view file_name_;

	public:
		explicit action() = default;

		static auto go_to_title() -> action { return action{ type::go_to_title, "" }; }
		static auto go_to_browser() -> action { return action{ type::go_to_browser, "" }; }
		static auto go_to_settings() -> action { return action{ type::go_to_settings, "" }; }
		static auto go_to_help() -> action { return action{ type::go_to_help, "" }; }
		static auto launch_game(std::string_view const file_name) -> action { return action{ type::launch_game, file_name }; }

		auto get_type() const -> type { return type_; }
		auto get_file_name() const -> std::string_view { return file_name_; }

	private:
		explicit action(type const type, std::string_view const file_name)
			: type_{ type }
			, file_name_{ file_name }
		{
		}
	};
} // namespace nes::app
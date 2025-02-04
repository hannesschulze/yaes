#pragma once

#include "nes/common/status.hh"
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
			show_error,
		};

	private:
		type type_{ type::none };
		std::string_view file_name_;
		status error_{ status::success };

	public:
		explicit action() = default;

		static auto go_to_title() -> action { return action{ type::go_to_title, "", status::success }; }
		static auto go_to_browser() -> action { return action{ type::go_to_browser, "", status::success }; }
		static auto go_to_settings() -> action { return action{ type::go_to_settings, "", status::success }; }
		static auto go_to_help() -> action { return action{ type::go_to_help, "", status::success }; }

		static auto launch_game(std::string_view const file_name) -> action
		{
			return action{ type::launch_game, file_name, status::success };
		}

		static auto show_error(status const error)
		{
			return action{ type::show_error, "", error };
		}

		auto get_type() const -> type { return type_; }
		auto get_file_name() const -> std::string_view { return file_name_; }
		auto get_error() const -> status { return error_; }

	private:
		explicit action(type const type, std::string_view const file_name, status const error)
			: type_{ type }
			, file_name_{ file_name }
			, error_{ error }
		{
		}
	};
} // namespace nes::app
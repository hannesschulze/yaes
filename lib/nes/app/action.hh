#pragma once

#include "../common/status.hh"
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
			close_popup,
			cancel_quit,
			confirm_quit,
			launch_game,
			show_error,
		};

	private:
		type type_{ type::none };
		std::string_view file_name_;
		std::string_view message_;
		status error_{ status::success };

	public:
		explicit action() = default;

		static auto go_to_title() -> action { return action{ type::go_to_title }; }
		static auto go_to_browser() -> action { return action{ type::go_to_browser }; }
		static auto go_to_settings() -> action { return action{ type::go_to_settings }; }
		static auto go_to_help() -> action { return action{ type::go_to_help }; }
		static auto close_popup() -> action { return action{ type::close_popup }; }
		static auto cancel_quit() -> action { return action{ type::cancel_quit }; }
		static auto confirm_quit() -> action { return action{ type::confirm_quit }; }

		static auto launch_game(std::string_view const file_name) -> action
		{
			auto res = action{ type::launch_game };
			res.file_name_ = file_name;
			return res;
		}

		static auto show_error(std::string_view const message, status const error) -> action
		{
			auto res = action{ type::show_error };
			res.message_ = message;
			res.error_ = error;
			return res;
		}

		auto get_type() const -> type { return type_; }
		auto get_file_name() const -> std::string_view { return file_name_; }
		auto get_message() const -> std::string_view { return message_; }
		auto get_error() const -> status { return error_; }

	private:
		explicit action(type const type)
			: type_{ type }
		{
		}
	};
} // namespace nes::app
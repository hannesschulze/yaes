#pragma once

namespace nes::app
{
	/// An action for the application to process after a scene has handled its events.
	enum class action
	{
		none,
		go_to_title,
		go_to_browser,
		go_to_settings,
		go_to_help,
		launch_game,
	};
} // namespace nes::app
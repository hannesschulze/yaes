#pragma once

namespace nes::app
{
	/// An action for the application to process after a scene has handled its events.
	enum class action
	{
		none,
		load_game,
	};
} // namespace nes::app
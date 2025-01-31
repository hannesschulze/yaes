#pragma once

#include "nes/app/ui/action.hh"

namespace nes::app
{
	class renderer;

	/// A part of the UI which takes care of rendering and handling input events.
	///
	/// One screen may be shown at a time. It may be used as an overlay over an active game.
	class screen
	{
	public:
		virtual ~screen() = default;

		screen(screen const&) = delete;
		screen(screen&&) = delete;
		auto operator=(screen const&) -> screen& = delete;
		auto operator=(screen&&) -> screen& = delete;

		/// Render a frame with the current UI state.
		virtual auto render(renderer&) -> void = 0;

		/// Handle events and return an action for the application to execute.
		virtual auto process_events() -> action = 0;

	protected:
		explicit screen() = default;
	};
} // namespace nes::app
#pragma once

#include "nes/app/input/input-device.hh"
#include "nes/common/list.hh"
#include <string_view>

namespace nes::app
{
	/// A game controller input device.
	class input_device_controller : public input_device, public list<input_device_controller>::node
	{
	public:
		enum class index
		{
			unused = -1,
			player_1 = 0,
			player_2 = 1,
		};

		/// Indicates whether the controller can be trusted to actually exist.
		///
		/// If this returns false, the controller is not automatically selected on startup if there is no other
		/// controller already selected.
		virtual auto is_reliable() const -> bool = 0;

		/// Human-readable name for the controller.
		virtual auto get_name() const -> std::string_view = 0;

		/// Get the (persistent) player index for the controller, if already set before.
		virtual auto get_index() const -> index = 0;

		/// Set the (persistent) player index for the controller.
		virtual auto set_index(index) -> void = 0;

	protected:
		explicit input_device_controller() = default;
	};
} // namespace nes::app
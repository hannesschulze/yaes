#pragma once

#include "nes/app/input/key.hh"
#include "nes/common/list.hh"

namespace nes::app
{
	class input_event;

	class input_buffer
	{
		struct event final : list<event>::node
		{
			bool pressed{ false };
		};

		event key_events_[key_count]{};
		list<event> event_queue_;

	public:
		explicit input_buffer() = default;

		input_buffer(input_buffer const&) = delete;
		input_buffer(input_buffer&&) = delete;
		auto operator=(input_buffer const&) -> input_buffer& = delete;
		auto operator=(input_buffer&&) -> input_buffer& = delete;

		/// Record a key-down event.
		auto key_down(key) -> void;
		/// Record a key-up event.
		auto key_up(key) -> void;

		auto read_key(key) -> bool;
		auto poll_event() -> input_event;

	private:
		auto get_event(key) -> event&;
		auto get_key(event const&) -> key;
		auto set_pressed(key, bool) -> void;
	};
} // namespace nes::app
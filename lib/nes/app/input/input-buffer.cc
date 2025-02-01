#include "nes/app/input/input-buffer.hh"
#include "nes/app/input/input-event.hh"

namespace nes::app
{
	auto input_buffer::key_down(key const k) -> void
	{
		set_pressed(k, true);
	}

	auto input_buffer::key_up(key const k) -> void
	{
		set_pressed(k, false);
	}

	auto input_buffer::read_key(key const k) -> bool
	{
		return get_event(k).pressed;
	}

	auto input_buffer::poll_event() -> input_event
	{
		auto it = event_queue_.begin();
		if (it == event_queue_.end()) { return input_event{}; }

		auto const event = *it;
		auto const k = get_key(*event);
		auto const pressed = event->pressed;
		event_queue_.remove(it);

		return pressed ? input_event::key_down(k) : input_event::key_up(k);
	}

	auto input_buffer::get_event(key const k) -> event&
	{
		return key_events_[static_cast<u32>(k)];
	}

	auto input_buffer::get_key(event const& ev) -> key
	{
		return static_cast<key>(&ev - key_events_);
	}

	auto input_buffer::set_pressed(key const k, bool const pressed) -> void
	{
		auto& event = get_event(k);
		if (event.pressed == pressed) { return; }
		event.pressed = pressed;

		// Remove the event if it was not previously fetched (skipping it entirely), otherwise add a new event.
		if (!event_queue_.remove(event))
		{
			event_queue_.append(event);
		}
	}
} // namespace nes::app
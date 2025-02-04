#pragma once

#include "nes/app/input/input-device-controller.hh"

namespace nes::app::mac
{
	/// Input device for an NES controller plugged in via serial port.
	class input_device_serial_controller final : public input_device_controller
	{
		char const* path_;
		int fd_{ -1 };
		sys::button_mask current_{};
		index index_{ index::unused };

	public:
		explicit input_device_serial_controller(char const* path);
		~input_device_serial_controller() override;

		auto is_reliable() const -> bool override;
		auto get_name() const -> std::string_view override { return path_; }
		auto get_index() const -> index override { return index_; }
		auto set_index(index const v) -> void override { index_ = v; }
		auto read_buttons() -> sys::button_mask override;

	private:
		auto set_up() -> bool;
		auto close() -> void;
	};
} // namespace nes::app:mac
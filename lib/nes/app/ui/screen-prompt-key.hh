#pragma once

#include "nes/app/ui/screen.hh"
#include "nes/common/containers/string-builder.hh"
#include "nes/common/containers/string-view.hh"

namespace nes::app
{
	class input_device_keyboard;

	/// Prompts the user for a decryption key.
	class screen_prompt_key final : public screen
	{
		static constexpr auto max_key_length = u32{ 23 };

		input_device_keyboard& keyboard_;
		string_view file_name_;
		string_buffer<max_key_length> key_;

	public:
		explicit screen_prompt_key(input_device_keyboard&);

		auto reset(string_view file_name) -> void;

		auto render(renderer&) -> void override;
		auto process_events() -> action override;
	};
} // namespace nes::app
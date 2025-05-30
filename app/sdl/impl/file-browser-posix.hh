#pragma once

#include "nes/app/file-browser.hh"
#include "nes/common/containers/path-builder.hh"
#include <dirent.h>

namespace nes::app::sdl
{
	/// File browser implementation using the POSIX directory API.
	class file_browser_posix final : public file_browser
	{
        path_buffer<1024> path_;
		DIR* it_{ nullptr };
		u32 it_pos_{ 0 };

	public:
		explicit file_browser_posix();
		~file_browser_posix() override;

		auto get_path() const -> path_view override { return path_.get_components(); }
		auto seek(u32) -> void override;
		auto read_next(entry* out_entry) -> bool override;
		auto navigate_up() -> status override;
		auto navigate(string_view) -> status override;
		auto load(string_view, span<u8>, u32* out_length) -> status override;

	private:
		auto reopen_directory(string_view path) -> status;
	};
} // namespace nes::app::sdl

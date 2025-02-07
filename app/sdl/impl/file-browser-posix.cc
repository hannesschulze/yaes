#include "impl/file-browser-posix.hh"
#include <string>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

namespace nes::app::sdl
{
	file_browser_posix::file_browser_posix()
	{
		auto const initial_path = getenv("HOME");
		if (initial_path)
		{
			for (auto const component : path_view{ initial_path })
			{
				if (path_.push(component) != status::success) { break; }
			}
		}

		reopen_directory(path_.get_path());
	}

	file_browser_posix::~file_browser_posix()
	{
		if (it_) { closedir(it_); }
	}

	auto file_browser_posix::seek(u32 const pos) -> void
	{
		if (it_pos_ > pos)
		{
			reopen_directory(path_.get_path());
		}

		while (it_pos_ < pos)
		{
			if (!read_next(nullptr)) { break; }
		}
	}

	auto file_browser_posix::read_next(entry* out_entry) -> bool
	{
		if (!it_) { return false; }

		while (true)
		{
			errno = 0;
			auto const ent = readdir(it_);
			if (!ent)
			{
				if (errno) { perror("file_browser_posix: readdir"); }
				return false;
			}

			auto const name = string_view{ ent->d_name };
			if (name.get_length() > max_name_length) { continue; }
			if (name[0] == '.') { continue; }

			auto type = entry_type{};
			if (ent->d_type == DT_DIR) { type = entry_type::directory; }
			else if (ent->d_type == DT_REG) { type = entry_type::file; }
			else { continue; }

			it_pos_ += 1;
			if (out_entry) { *out_entry = entry{ type, name }; }
			return true;
		}
	}

	auto file_browser_posix::navigate_up() -> status
	{
		auto new_path = path_;
		if (auto const s = new_path.pop(); s != status::success) { return s; }
		if (auto const s = reopen_directory(new_path.get_path()); s != status::success) { return s; }
		path_ = new_path;
		return status::success;
	}

	auto file_browser_posix::navigate(string_view const item) -> status
	{
		auto new_path = path_;
		if (auto const s = new_path.push(item); s != status::success) { return s; }
		if (auto const s = reopen_directory(new_path.get_path()); s != status::success) { return s; }
		path_ = new_path;
		return status::success;
	}

	auto file_browser_posix::load(string_view const item, span<u8> const buffer, u32* out_length) -> status
	{
		auto file_path = path_;
		if (auto const s = file_path.push(item); s != status::success) { return s; }

		auto const file_path_str = std::string{ file_path.get_path().get_data(), file_path.get_path().get_length() };
		auto const fd = open(file_path_str.c_str(), O_RDONLY);
		if (fd == -1)
		{
			perror("file_browser_posix: open");
			return status::error_system_error;
		}

		auto const res = read(fd, buffer.get_data(), buffer.get_length());
		if (res < 0)
		{
			perror("file_browser_posix: read");
			close(fd);
			return status::error_system_error;
		}
		if (out_length) { *out_length = static_cast<u32>(res); }

		auto remaining = char{};
		if (read(fd, &remaining, 1) != 0)
		{
			// File was too long.
			close(fd);
			return status::error_invalid_ines_data;
		}

		close(fd);

		return status::success;
	}

	auto file_browser_posix::reopen_directory(string_view const path) -> status
	{
		auto const path_str = std::string{ path.get_data(), path.get_length() };
		auto const new_it = opendir(path_str.c_str());
		if (!new_it)
		{
			perror("file_browser_posix: opendir");
			return status::error_system_error;
		}

		if (it_) { closedir(it_); }
		it_ = new_it;
		it_pos_ = 0;
		return status::success;
	}
} // namespace nes::app::mac

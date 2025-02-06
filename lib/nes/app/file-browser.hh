#pragma once

#include "nes/common/containers/span.hh"
#include "nes/common/containers/string-view.hh"
#include "nes/common/types.hh"
#include "nes/common/status.hh"
#include "nes/common/utils.hh"

namespace nes
{
	class path_view;
} // namespace nes;

namespace nes::app
{
	/// A file browser that has a current directory whose items are iterated.
	class file_browser
	{
	public:
		static constexpr auto max_name_length = u32{ 64 };

		enum class entry_type
		{
			directory,
			file,
		};

		/// An entry in a directory.
		class entry
		{
			entry_type type_{ entry_type::directory };
			char name_[max_name_length + 1]{};

		public:
			explicit entry() = default;

			explicit entry(entry_type const type, string_view const name)
				: type_{ type }
			{
				copy(name.get_data(), name_, min(name.get_length(), max_name_length));
			}

			auto get_type() const -> entry_type { return type_; }
			auto get_name() const -> string_view { return name_; }
		};

		virtual ~file_browser() = default;

		file_browser(file_browser const&) = delete;
		file_browser(file_browser&&) = delete;
		auto operator=(file_browser const&) -> file_browser& = delete;
		auto operator=(file_browser&&) -> file_browser& = delete;

		/// Get the path components of the current working directory.
		virtual auto get_path() const -> path_view = 0;

		/// Go to directory entry at the given index.
		virtual auto seek(u32) -> void = 0;

		/// Read the directory entry at the current index and move the index forward by 1.
		virtual auto read_next(entry* out_entry) -> bool = 0;

		/// Change the current working directory to the parent directory.
		virtual auto navigate_up() -> status = 0;

		/// Change the current working directory to an item.
		virtual auto navigate(string_view) -> status = 0;

		/// Load a file in the current working directory into memory.
		virtual auto load(string_view, span<u8>, u32* out_length) -> status = 0;

	protected:
		explicit file_browser() = default;
	};
} // namespace nes::app
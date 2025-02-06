#pragma once

#include "nes/common/containers/span.hh"
#include "nes/common/types.hh"
#include "nes/common/status.hh"
#include <string_view>

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

		enum class item_type
		{
			directory,
			file,
		};

		class item
		{
			item_type type_{ item_type::directory };
			char name_[max_name_length + 1]{};

		public:
			explicit item() = default;

			explicit item(item_type const type, std::string_view const name)
				: type_{ type }
			{
				std::copy_n(name.begin(), std::min(static_cast<u32>(name.length()), max_name_length), name_);
			}

			auto get_type() const -> item_type { return type_; }
			auto get_name() const -> std::string_view { return name_; }
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

		/// Read the directory entry at the current index.
		virtual auto read_next(item* out_item) -> bool = 0;

		/// Change the current working directory to the parent directory.
		virtual auto navigate_up() -> status = 0;

		/// Change the current working directory to an item.
		virtual auto navigate(std::string_view) -> status = 0;

		/// Load a file in the current working directory into memory.
		virtual auto load(std::string_view, span<u8>, u32* out_length) -> status = 0;

	protected:
		explicit file_browser() = default;
	};
} // namespace nes::app
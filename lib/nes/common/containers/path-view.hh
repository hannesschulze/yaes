#pragma once

#include "nes/common/containers/string-view.hh"
#include "nes/common/types.hh"

namespace nes
{
	/// Non-owning copy of a path, useful for iterating over its components.
	class path_view
	{
		string_view path_;

	public:
		/// An iterator for the path components.
		class iterator
		{
			friend path_view;

			char const* current_{ nullptr }; // Start of the current component
			u32 component_length_{ 0 }; // Length of the current component
			u32 remaining_{ 0 }; // Number of characters remaining in the buffer, including the current component.

		public:
			auto operator==(iterator const& other) -> bool { return current_ == other.current_; }
			auto operator!=(iterator const& other) -> bool { return current_ != other.current_; }

			auto operator*() const -> string_view;
			auto operator++() -> iterator&;
			auto operator++(int) -> iterator;

		private:
			explicit iterator(char const* current, u32 const component_length, u32 const remaining)
				: current_{ current }
				, component_length_{ component_length }
				, remaining_{ remaining }
			{
			}
		};

		explicit path_view() = default;
		explicit path_view(string_view const path)
			: path_{ path }
		{
		}

		auto get_path() const -> string_view { return path_; }

		auto begin() const -> iterator;
		auto end() const -> iterator;
		auto is_empty() const -> bool { return begin() == end(); }
	};
} // namespace nes::app
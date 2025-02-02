#pragma once

#include "nes/common/types.hh"
#include <string_view>

namespace nes
{
	/// Non-owning copy of a path, consisting of multiple path components stored as string views.
	///
	/// This is similar in spirit to a string_view, but for collections of string_views instead of characters.
	class path_view
	{
		std::string_view const* components_{ nullptr };
		u32 component_count_{ 0 };

	public:
		/// An iterator for the path components.
		class iterator
		{
			friend path_view;

			std::string_view const* current_{ nullptr };
			u32 remaining_{ 0 };

		public:
			auto operator==(iterator const& other) -> bool { return current_ == other.current_; }
			auto operator!=(iterator const& other) -> bool { return current_ != other.current_; }

			auto operator*() const -> std::string_view { return current_ ? *current_ : ""; }

			auto operator++() -> iterator&
			{
				if (remaining_ > 0)
				{
					current_ += 1;
					remaining_ -= 1;
				}
				return *this;
			}

			auto operator++(int) -> iterator
			{
				auto res = *this;
				++*this;
				return res;
			}

		private:
			explicit iterator() = default;
			explicit iterator(std::string_view const* current, u32 const remaining)
				: current_{ current }
				, remaining_{ remaining }
			{
			}
		};

		explicit path_view() = default;

		explicit path_view(std::string_view const* components, u32 const component_count)
			: components_{ components }
			, component_count_{ component_count }
		{
		}

		auto get_components() const -> std::string_view const* { return components_; }
		auto get_component_count() const -> u32 { return component_count_; }
		auto get_component(u32 const index) const -> std::string_view { return components_[index]; }

		auto begin() const -> iterator { return iterator{ components_, component_count_ }; }
		auto end() const -> iterator { return iterator{ components_ + component_count_, 0 }; }
	};
} // namespace nes::app
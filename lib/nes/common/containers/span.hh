#pragma once

#include "nes/common/types.hh"
#include "nes/common/debug.hh"

namespace nes
{
	namespace detail
	{
		struct span_unchecked{};

		inline constexpr auto span_dynamic_length = u32{ 0xFFFF };

		template<typename T>
		struct remove_const
		{
			using type = T;
		};

		template<typename T>
		struct remove_const<T const>
		{
			using type = T;
		};

		template<typename T>
		using remove_const_type = typename remove_const<T>::type;
	} // namespace detail

	/// Implementation of a container similar to std::span in C++20.
	template<typename T, u32 Length = detail::span_dynamic_length>
	class span
	{
		template<typename U, u32 OtherLength>
		friend class span;

		T* data_{ nullptr };
		u32 length_{ 0 };

	public:
		using iterator = T*;

		explicit span()
		{
			static_assert(Length == 0 || Length == detail::span_dynamic_length);
		}

		explicit span(T* data, u32 const length)
			: span{ data, length, {} }
		{
			static_assert(Length == detail::span_dynamic_length);
		}

		template<u32 ActualLength>
		span(T (&data)[ActualLength])
			: span{ data, ActualLength, {} }
		{
			static_assert(Length == ActualLength || Length == detail::span_dynamic_length);
		}

		span(span<detail::remove_const_type<T>, Length> const other)
			: span{ other.data_, other.length_, {} }
		{
		}

		auto get_data() const -> T* { return data_; }
		auto get_length() const -> u32 { return length_; }

		auto begin() const -> iterator { return data_; }
		auto end() const -> iterator { return data_ + length_; }

		auto subspan(u32 const first, u32 const length) const -> span<T>
		{
			NES_ASSERT(first + length <= get_length() && "subspan out of bounds");
			return span{ data_ + first, length };
		}

		auto subspan(u32 const first) const -> span<T>
		{
			NES_ASSERT(first <= get_length() && "subspan out of bounds");
			return subspan(length_ - first);
		}

		template<u32 OtherLength>
		auto subspan(u32 const first) const -> span<T, OtherLength>
		{
			static_assert(OtherLength < Length);
			NES_ASSERT(first + OtherLength <= get_length() && "subspan out of bounds");
			return span<T, OtherLength>{ data_ + first, OtherLength, {} };
		}

		auto operator[](u32 const index) const -> T&
		{
			NES_ASSERT(index < get_length() && "index out of bounds");
			return data_[index];
		}

	private:
		explicit span(T* data, u32 const length, detail::span_unchecked)
			: data_{ data }
			, length_{ length }
		{
		}
	};
} // namespace nes
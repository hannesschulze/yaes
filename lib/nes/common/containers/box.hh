#pragma once

#include "nes/common/types.hh"
#include "nes/common/debug.hh"
#include "nes/common/utils.hh"

namespace nes
{
	/// Wrapper around an object, allowing for it to be uninitialized or reassigned even if the object type doesn't
	/// support it.
	///
	/// Basically a std::unique_ptr but stored in-place (and thus not movable).
	template<typename T>
	class box
	{
		bool initialized_{ false };
		alignas(T) u8 buffer_[sizeof(T)];

	public:
		/// Create an empty box.
		explicit box() = default;

		~box()
		{
			clear();
		}

		box(box const&) = delete;
		box(box&&) = delete;
		auto operator=(box const&) -> box& = delete;
		auto operator=(box&&) -> box& = delete;

		auto operator*() -> T&
		{
			NES_ASSERT(has_value() && "expected box to be non-empty");
			return *get();
		}

		auto operator*() const -> T const&
		{
			NES_ASSERT(has_value() && "expected box to be non-empty");
			return *get();
		}

		auto operator->() -> T*
		{
			NES_ASSERT(has_value() && "expected box to be non-empty");
			return get();
		}

		auto operator->() const -> T const*
		{
			NES_ASSERT(has_value() && "expected box to be non-empty");
			return get();
		}

		operator bool() const { return has_value(); }

		auto has_value() const -> bool { return initialized_; }
		auto get() -> T* { return has_value() ? launder(reinterpret_cast<T*>(buffer_)) : nullptr; }
		auto get() const -> T const* { return has_value() ? launder(reinterpret_cast<T const *>(buffer_)) : nullptr; }

		/// Construct a new value in-place, potentially replacing the old value.
		template<typename... Args>
		auto emplace(Args&&... args) -> void
		{
			clear();
			new (buffer_) T(args...);
			initialized_ = true;
		}

		/// Clear the boxed value.
		auto clear() -> void
		{
			if (initialized_) { (*this)->~T(); }
			initialized_ = false;
		}
	};
} // namespace nes
#pragma once

#include "nes/common/types.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"

namespace nes::app
{
	/// A selection menu managing a fixed number of items and pagination.
	template<typename Item, u32 PageSize>
	class selection
	{
		Item items_[PageSize];
		u32 count_{ 0 };
		u32 page_count_{ 0 };
		i32 x_{};
		i32 y_{};
		u32 width_{};
		i32 selected_{ -1 };
		i32 current_page_{ -1 };

	public:
		explicit selection(i32 const x, i32 const y, u32 const width)
			: x_{ x }
			, y_{ y }
			, width_{ width }
		{
		}

		virtual ~selection() = default;

		selection(selection const&) = delete;
		selection(selection&&) = delete;
		auto operator=(selection const&) -> selection& = delete;
		auto operator=(selection&&) -> selection& = delete;

		auto set_page_count(u32 const page_count) -> void
		{
			page_count_ = page_count;
			set_page(page_count_ > 0 ? 0 : -1);
		}

		auto get_page_count() const -> u32 { return page_count_; }
		auto get_current_page() const -> i32 { return current_page_; }
		auto get_selected() const -> Item const* { return selected_ >= 0 ? &items_[selected_] : nullptr; }

		auto render(renderer& renderer) const -> void
		{
			for (auto i = u32{ 0 }; i < count_; ++i)
			{
				auto const y = y_ + static_cast<i32>(3 * i);
				auto const is_selected = static_cast<i32>(i) == selected_;
				auto const color = is_selected ? color::accent_primary : color::foreground_primary;
				if (is_selected)
				{
					renderer.render_text(x_, y, ">", color);
				}

				render_item(renderer, items_[i], x_ + 2, y, width_ - 2, color);
			}
		}

		auto go_next() -> void
		{
			if (selected_ < 0) { return; }

			selected_ += 1;
			if (selected_ == static_cast<i32>(count_)) { selected_ = 0; }
		}

		auto go_up() -> void
		{
			if (selected_ <= 0)
			{
				if (current_page_ <= 0) { return; }

				set_page(current_page_ - 1, true);
				return;
			}
			selected_ -= 1;
		}

		auto go_down() -> void
		{
			if (selected_ < 0 || selected_ + 1 == static_cast<i32>(count_))
			{
				if (current_page_ < 0 || current_page_ + 1 == static_cast<i32>(page_count_)) { return; }

				set_page(current_page_ + 1, false);
				return;
			}
			selected_ += 1;
		}

	protected:
		/// Render an item's caption at the given position and width.
		virtual auto render_item(renderer&, Item const&, i32 x, i32 y, u32 width, color) const -> void = 0;

		/// Load the items for a page (max. PageSize items) and return the number of loaded items.
		virtual auto load_page(Item(&)[PageSize], u32 page) -> u32 = 0;

	private:
		auto set_page(i32 const page, bool const select_last = false) -> void
		{
			if (page < 0)
			{
				current_page_ = -1;
				count_ = 0;
				selected_ = -1;
				return;
			}

			current_page_ = page;
			count_ = load_page(items_, static_cast<u32>(page));
			if (count_ > 0)
			{
				selected_ = select_last ? static_cast<i32>(count_) - 1 : 0;
			}
			else
			{
				selected_ = -1;
			}
		}
	};
} // namespace nes::app
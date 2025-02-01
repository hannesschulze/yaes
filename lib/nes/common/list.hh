#pragma once

namespace nes
{
	/// An intrusive doubly-linked list.
	template<typename T>
	class list
	{
	public:
		/// Base class of nodes in the list.
		class node
		{
			friend list;

			list* list_{ nullptr };
			T* prev_{ nullptr };
			T* next_{ nullptr };

		public:
			virtual ~node() = default;

			node(node const&) = delete;
			node(node&&) = delete;
			auto operator=(node const&) -> node& = delete;
			auto operator=(node&&) -> node& = delete;

		protected:
			explicit node() = default;
		};

		/// An iterator for the linked list.
		class iterator
		{
			friend list;

			T* current_{ nullptr };

		public:
			auto operator==(iterator const& other) -> bool { return current_ == other.current_; }
			auto operator!=(iterator const& other) -> bool { return current_ != other.current_; }

			auto operator*() const -> T* { return current_; }

			auto operator++() -> iterator&
			{
				if (current_)
				{
					node& current_node = *current_;
					current_ = current_node.next_;
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
			explicit iterator(T* current)
				: current_{ current }
			{
			}
		};

	private:
		T* head_{ nullptr };
		T* tail_{ nullptr };

	public:
		explicit list() = default;

		~list()
		{
			while (begin() != end()) { remove(begin()); }
		}

		list(list const&) = delete;
		list(list&&) = delete;
		auto operator=(list const&) -> list& = delete;
		auto operator=(list&&) -> list& = delete;

		auto begin() const -> iterator { return iterator{ head_ }; }
		auto end() const -> iterator { return iterator{}; }

		/// Add an item to the end of the list, removing it from any other list it may be in.
		auto append(T& item) -> void
		{
			node& item_node = item;
			if (item_node.list_) { item_node.list_->remove(item); }

			item_node.list_ = this;
			if (tail_)
			{
				item_node.prev_ = tail_;
				tail_->next_ = &item;
			}
			else
			{
				head_ = &item;
			}
			tail_ = &item;
		}

		/// Remove an item from the list.
		auto remove(T& item) -> bool
		{
			node& item_node = item;
			if (!item_node.list_) { return false; }

			if (head_ == &item) { head_ = item_node.next_; }
			if (tail_ == &item) { tail_ = item_node.prev_; }
			if (item_node.prev_) { item_node.prev_->next_ = item_node.next_; }
			if (item_node.next_) { item_node.next_->prev_ = item_node.prev_; }

			item_node.next_ = nullptr;
			item_node.prev_ = nullptr;
			item_node.list_ = nullptr;

			return true;
		}

		/// Remove an item from the list by its iterator.
		auto remove(iterator it) -> bool
		{
			if (it != end()) { return remove(**it); }
			return false;
		}
	};
} // namespace nes
#pragma once
#ifndef C_Id
#define C_Id
#include <mutex>
#include <utility>

namespace PCL {
	template <typename index_type = std::uint_fast64_t>
	class C_Indexer {
	private:
		std::mutex
			synchronizer;
		const index_type
			upper_limit,
			starting_value;
		index_type
			current_index = 0;
	public:
		explicit C_Indexer(index_type ul) noexcept;
		explicit C_Indexer(index_type ll, index_type ul);
		C_Indexer(C_Indexer& obj);
		~C_Indexer() = default;
		index_type
			operator()(),
			get_last_index();
		void
			reset();
	};

	template <typename index_type>
	C_Indexer<index_type>::C_Indexer(index_type upper_lim) noexcept :
		upper_limit(upper_lim),
		starting_value(0) {}

	template <typename index_type>
	C_Indexer<index_type>::C_Indexer(index_type starting_val, index_type upper_lim) :
		upper_limit(upper_lim),
		starting_value(starting_val),
		current_index(starting_val) {}

	template <typename index_type>
	C_Indexer<index_type>::C_Indexer(C_Indexer<index_type>& obj) :
		upper_limit(obj.upper_limit),
		starting_value(obj.starting_value),
		current_index(obj.current_index) {}

	template <typename index_type>
	index_type C_Indexer<index_type>::operator()() {
		std::lock_guard<std::mutex> lg(synchronizer);
		if (current_index == upper_limit && upper_limit != starting_value) {
			throw std::runtime_error("Too many index requests!");
		}
		return std::exchange(current_index, current_index + 1);
	}

	template <typename index_type>
	index_type C_Indexer<index_type>::get_last_index() {
		std::lock_guard<std::mutex> lg(synchronizer);
		return current_index - 1;
	}

	template<typename index_type>
	inline void C_Indexer<index_type>::reset() {
		std::lock_guard<std::mutex> lg(synchronizer);
		current_index = starting_value;
	}
}

#endif // !C_Id
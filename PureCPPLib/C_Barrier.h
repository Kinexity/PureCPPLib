#pragma once
#ifndef C_Bar
#define C_Bar
#include <condition_variable>
#include <mutex>
#include <thread>
#include <cmath>
#include <cstdint>
#include <immintrin.h>

namespace PCL {
	class C_Barrier {
	public:
		explicit C_Barrier(std::uint_fast64_t thread_count);
		~C_Barrier() = default;
		void
			arrive_and_wait(),
			arrive_and_go();
	private:
		const std::uint_fast64_t thread_count;
		std::uint_fast64_t waiting_thread_count;
		bool flag;
		std::mutex mut_ex;
		std::condition_variable cond_var;
	};

	template <typename Condition_Type>
	void spin_wait_loop(Condition_Type break_condition) {
		while (!break_condition());
	};
}


#endif // !C_Bar
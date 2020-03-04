#pragma once
#ifndef C_La
#define C_La
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>

namespace PCL {
	class C_Latch {
	public:
		explicit C_Latch(std::uint_fast64_t thread_count);
		~C_Latch() = default;
		void
			wait(),
			notify();
		std::chrono::high_resolution_clock::duration
			wait_with_timer(std::chrono::milliseconds wait_duration, std::uint_fast64_t wait_intervals);
		bool
			waiting_state();
	private:
		const std::uint_fast64_t thread_count;
		unsigned waiting_thread_count;
		bool
			flag,
			failed = false;
		std::mutex mut_ex;
		std::condition_variable cond_var;
	};
}

#endif // !C_La
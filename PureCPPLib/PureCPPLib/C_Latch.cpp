#include "C_Latch.h"

namespace PCL {
	C_Latch::C_Latch(std::uint_fast64_t thread_count)
		:thread_count(thread_count)
		, waiting_thread_count(0)
		, flag(false) {}

	void C_Latch::wait() {
		std::unique_lock < std::mutex > guard(mut_ex);
		const bool expected = !flag;
		++waiting_thread_count;
		if (waiting_thread_count != thread_count) {
			cond_var.wait(guard, [&] { return flag == expected; });
		}
		else {
			waiting_thread_count = 0;
			flag = expected;
			cond_var.notify_all();
		}
	}

	std::chrono::high_resolution_clock::duration C_Latch::wait_with_timer(std::chrono::milliseconds wait_duration, std::uint_fast64_t wait_intervals) {
		std::unique_lock < std::mutex > guard(mut_ex);
		std::chrono::high_resolution_clock::time_point beg = std::chrono::high_resolution_clock::now();
		const bool expected = !flag;
		++waiting_thread_count;
		for (std::uint_fast64_t interval = 0; interval < wait_intervals; interval++) {
			if (waiting_thread_count == thread_count) {
				waiting_thread_count = 0;
				flag = expected;
				cond_var.notify_all();
				return std::chrono::high_resolution_clock::duration();
			}
			else {
				std::this_thread::sleep_for(wait_duration);
			}
		}
		failed = true;
		return std::chrono::high_resolution_clock::duration(std::chrono::high_resolution_clock::now() - beg);
	}

	bool C_Latch::waiting_state() {
		return failed;
	}

	void C_Latch::notify() {
		std::unique_lock < std::mutex > guard(mut_ex);
		const bool expected = !flag;
		++waiting_thread_count;
		if (waiting_thread_count == thread_count) {
			waiting_thread_count = 0;
			flag = expected;
			cond_var.notify_all();
		}
	}
}
#include "C_Barrier.h"
#include <iostream>

namespace PCL {
	C_Barrier::C_Barrier(std::uint_fast64_t thread_count)
		:thread_count(thread_count)
		, waiting_thread_count(0)
		, flag(false) {}

	void C_Barrier::arrive_and_wait() {
		std::unique_lock < std::mutex > guard(mut_ex);
		const bool expected = !flag;
		++waiting_thread_count;
		if (waiting_thread_count != thread_count) {
			cond_var.wait(guard, [&] {return flag == expected; });
		}
		else {
			waiting_thread_count = 0;
			flag = expected;
			cond_var.notify_all();
		}
	}

	void C_Barrier::arrive_and_go() {
		std::unique_lock < std::mutex > guard(mut_ex);
		++waiting_thread_count;
		if (waiting_thread_count != thread_count) {
			waiting_thread_count = 0;
			flag = !flag;
			cond_var.notify_all();
		}
	}
}
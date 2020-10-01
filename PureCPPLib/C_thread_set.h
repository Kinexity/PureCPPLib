#pragma once
#ifndef thr_set
#define thr_set
#include <thread>
#include <boost/thread/barrier.hpp>
#include <iostream>

namespace PCL {
	class C_thread_set {
	private:
		std::mutex
			synchronizer;
		std::unique_ptr<boost::barrier>
			end_barrier;
		template <typename Func_Ptr_Type, typename... Func_Args> void
			thread_func(Func_Ptr_Type func_ptr, Func_Args... args);
		bool
			async_started = false;
	public:
		C_thread_set() = default;
		~C_thread_set() = default;
		template <typename Func_Ptr_Type, typename... Func_Args> void
			run(Func_Ptr_Type func_ptr, ::uint_fast64_t num_of_threads, Func_Args... args);
		template <typename Func_Ptr_Type, typename... Func_Args> void
			start_async(Func_Ptr_Type func_ptr, ::uint_fast64_t num_of_threads, Func_Args... args);
		void
			wait_for_async();
	};

	template <typename Func_Ptr_Type, typename... Func_Args>
	void C_thread_set::thread_func(Func_Ptr_Type func_ptr, Func_Args... args) {
		std::invoke(func_ptr, args...);
		end_barrier->arrive_and_wait();
	}

	template <typename Func_Ptr_Type, typename... Func_Args>
	void C_thread_set::run(Func_Ptr_Type func_ptr, uint_fast64_t num_of_threads, Func_Args... args) {
		start_async(func_ptr, num_of_threads, args...);
		wait_for_async();
	}

	template<typename Func_Ptr_Type, typename ...Func_Args>
	inline void C_thread_set::start_async(Func_Ptr_Type func_ptr, ::uint_fast64_t num_of_threads, Func_Args... args) {
		std::unique_lock<std::mutex> lg(synchronizer);
		async_started = true;
		end_barrier = std::make_unique<boost::barrier>(num_of_threads + 1);
		for (uint_fast64_t thr_ord = 0; thr_ord < num_of_threads; thr_ord++) {
			if constexpr (sizeof...(Func_Args) == 0) {
				std::thread([&]()->void {
					std::invoke(func_ptr);
					end_barrier->wait();
				}).detach();
			}
			else {
				std::thread([&]()->void {
					std::invoke(func_ptr, args...);
					end_barrier->wait();
				}).detach();
			}
		}
	}

	inline void PCL::C_thread_set::wait_for_async() {
		if (async_started) {
			end_barrier->wait();
		}
		async_started = false;
	}
}
#endif // !thr_set
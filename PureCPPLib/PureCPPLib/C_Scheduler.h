#pragma once
#include <mutex>

namespace PCL {
	template <typename Return_Type, typename Callable_Type>
	class C_Scheduler {
	private:
		std::mutex
			mut_ex;
		Callable_Type
			callable;
	public:
		explicit C_Scheduler(Callable_Type&& callable__);
		explicit C_Scheduler(C_Scheduler<Return_Type, Callable_Type>&) = default;
		~C_Scheduler() = default;
		Return_Type
			operator()();
	};

	template <typename Return_Type, typename Callable_Type>
	C_Scheduler<Return_Type, Callable_Type>::C_Scheduler(Callable_Type&& callable__) :
	callable(callable__) {}

	template<typename Return_Type, typename Callable_Type>
	inline Return_Type C_Scheduler<Return_Type, Callable_Type>::operator()() {
		std::unique_lock<std::mutex> guard(mut_ex);
		return callable();
	}
}
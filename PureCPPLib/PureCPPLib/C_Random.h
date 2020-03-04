#pragma once
#ifndef c_rand
#define c_rand
#include <random>

namespace PCL {
	class C_Random {
	private:
		std::random_device
			r_d;
		std::mt19937_64
			gen{ r_d() };
	public:
		C_Random() = default;
		~C_Random() = default;
		uint_fast64_t
			operator()();
	};

	inline uint_fast64_t C_Random::operator()() {
		return gen();
	}
}
#endif // !c_rand
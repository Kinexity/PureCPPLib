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
		uint_fast64_t
			operator()() {
			return gen();
		};
	};
}

inline uint_fast64_t rnd() {
	static  PCL::C_Random rnd_eng;
	return rnd_eng();
}

#endif // !c_rand
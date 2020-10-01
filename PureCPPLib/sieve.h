#pragma once
#include <vector>
#include <cmath>

namespace PCL {
	inline std::vector<bool> sieve(size_t N) {
		std::vector<bool> bv(N, true);
		bv[0] = false;
		bv[1] = false;
		for (size_t i = 2; i < (size_t)std::ceil(std::sqrt(N)); i++) {
			if (bv[i] && i < (size_t)2ULL << 32) {
				for (size_t j = i * i; j < N; j += i) {
					bv[j] = false;
				}
			}
		}
		return std::move(bv);
	}
}
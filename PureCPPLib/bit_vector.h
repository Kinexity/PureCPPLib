#pragma once
#ifndef bitsize
#define bitsize(type) (8*sizeof(type))
#endif // !bitsize
#ifndef bs_
#define bs_
#include <cstdlib>
#include <memory>
#include <algorithm>
#include <execution>
#include <string>
#include <vector>
#include <intrin.h>
#include <immintrin.h>
#include <functional>
#include "bit_.h"

namespace PCL {
	using bitset_internal = uint_fast64_t;
	class bit_vector {
		friend
			class reference;
		reference
			bit_ref;
		std::vector<bitset_internal>
			bit_internal_vector;
	public:
		bit_vector(size_t bitset_size);
		~bit_vector() = default;
		bitset_internal
			* operator&();
		reference&
			operator[](size_t index);
		size_t
			size(), //returns size of bit_internal_vector in bytes
			byte_size(), //returns size of bitset in bits
			count(bool bit_value);
		void resize(size_t bit_size);
		bool
			operator=(bool value),
			operator==(bool value);
		decltype(bit_internal_vector)&
			data() { return bit_internal_vector; };
	};

	inline reference& bit_vector::operator[](size_t index) {
		return bit_ref[index];
	}

	inline uint_fast64_t bit_vector::size() {
		return bit_internal_vector.size() * bitsize(bitset_internal);
	}

	inline uint_fast64_t bit_vector::byte_size() {
		return bit_internal_vector.size();
	}

	inline bool bit_vector::operator=(bool value) {
		std::fill(std::execution::par_unseq, bit_internal_vector.begin(), bit_internal_vector.end(), (value ? ~bitset_internal() : bitset_internal()));
		return value;
	}

	inline bool bit_vector::operator==(bool value) {
		if (value) {
			return std::reduce(std::execution::par_unseq, bit_internal_vector.begin(), bit_internal_vector.end(), ~bitset_internal(), std::bit_and<>()) == ~bitset_internal();
		}
		else {
			return std::reduce(std::execution::par_unseq, bit_internal_vector.begin(), bit_internal_vector.end(), bitset_internal(), std::bit_or<>()) == bitset_internal();
		}
	}

	inline size_t bit_vector::count(bool bit_value) {
		auto sum = std::transform_reduce(std::execution::par_unseq, bit_internal_vector.begin(), bit_internal_vector.end(), size_t(), std::plus<size_t>(), [](auto subset) { return  __popcnt64(subset); });
		return (bit_value ? sum : size() - sum);
	}

	inline void bit_vector::resize(size_t bitset_size) {
		bit_internal_vector.resize(bitset_size / bitsize(bitset_internal) + (bitset_size % bitsize(bitset_internal) > 0));
	}

	inline bitset_internal* bit_vector::operator&() {
		return bit_internal_vector.data();
	}
}

#pragma message("PCL::bit_vector works only on x86 and other little endian architectures!")

#endif // !bs_
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
#include <intrin.h>
#include <immintrin.h>
#include "bit_.h"


namespace PCL {
	using bitset_internal = uint_fast64_t;
	template <::std::uint_fast64_t bitset_size>
	class bitset {
		const std::uint_fast64_t
			array_size = (bitset_size - bitset_size % bitsize(bitset_internal)) / bitsize(bitset_internal) + (bitset_size % bitsize(bitset_internal) > 0);
		reference<bitset_size>
			bit_ref;
		std::unique_ptr<bitset_internal[]>
			bitset_array;
	public:
		bitset();
		~bitset() = default;
		bitset_internal
			* operator&(),
			* release(); //returns ptr to bitset_array
		reference<bitset_size>&
			operator[](uint_fast64_t index);
		uint_fast64_t
			size(), //returns size of bitset_array in bytes
			byte_size(); //returns size of bitset in bits
		void
			operator=(bool value),
			reset();
		bool
			operator==(bool value);
		uint_fast64_t
			count(bool bit_value);
		template <uint_fast64_t bitset_size> friend
			class reference;
	};

	template<std::uint_fast64_t bitset_size>
	reference<bitset_size>& bitset<bitset_size>::operator[](std::uint_fast64_t index) {
		return bit_ref[index];
	}

	template<std::uint_fast64_t bitset_size>
	inline uint_fast64_t bitset<bitset_size>::size() {
		return bitset_size;
	}

	template<std::uint_fast64_t bitset_size>
	inline uint_fast64_t bitset<bitset_size>::byte_size() {
		return array_size;
	}

	template<std::uint_fast64_t bitset_size>
	inline void bitset<bitset_size>::operator=(bool value) {
		std::fill(std::execution::par, &bitset_array[0], &bitset_array[array_size], (value ? ~bitset_internal() : bitset_internal()));
	}

	template<std::uint_fast64_t bitset_size>
	inline void bitset<bitset_size>::reset() {
		bitset_array.reset(new bitset_internal[array_size]);
	}

	template<std::uint_fast64_t bitset_size>
	inline bool bitset<bitset_size>::operator==(bool value) {
		bitset_internal mask = (value ? ~bitset_internal() : bitset_internal());
		return std::all_of(std::execution::par, &bitset_array[0], &bitset_array[array_size], [&](bitset_internal compared) {return compared == mask; });
	}

	template<std::uint_fast64_t bitset_size>
	inline uint_fast64_t bitset<bitset_size>::count(bool bit_value) {
		uint_fast64_t sum = 0;
		for (uint_fast64_t index = 0; index < array_size; index++) {
			sum += _mm_popcnt_u64(bitset_array[index]);
		}
		return (bit_value ? sum : bitset_size - sum);
	}

	template <std::uint_fast64_t bitset_size>
	bitset<bitset_size>::bitset() :
		bit_ref(*this) {
		bitset_array = std::make_unique<bitset_internal[]>(array_size);
		std::fill(std::execution::par, &bitset_array[0], &bitset_array[array_size], bitset_internal());
	}

	template<std::uint_fast64_t bitset_size>
	inline bitset_internal* bitset<bitset_size>::release() {
		return bitset_array.release();
	}

	template<std::uint_fast64_t bitset_size>
	inline bitset_internal* bitset<bitset_size>::operator&() {
		return bitset_array.get();
	}
}

#pragma message("PCL::bitset works only on x86 and other little endian architectures!")

#endif // !bs_
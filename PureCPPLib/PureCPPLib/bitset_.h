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
#include "bit_.h"


namespace PCL {
	typedef unsigned char bitset_internal;
	template <std::uint_fast64_t bitset_size>
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
			*operator&(),
			*release(); //returns ptr to bitset_array
		reference<bitset_size>&
			operator[](uint_fast64_t index);
		uint_fast64_t
			size(), //returns size of bitset_array in bytes
			byte_size(); //returns size of bitset in bits
		void
			operator=(bool value),
			reset();
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
		std::fill(std::execution::par, &bitset_array[0], &bitset_array[array_size], (value ? 0xFF : 0x00));
	}

	template<std::uint_fast64_t bitset_size>
	inline void bitset<bitset_size>::reset() {
		bitset_array.reset(new bitset_internal[array_size]);
	}

	template <std::uint_fast64_t bitset_size>
	bitset<bitset_size>::bitset() :
		bit_ref(*this) {
		bitset_array = std::make_unique<bitset_internal[]>(array_size);
		std::fill(std::execution::par, &bitset_array[0], &bitset_array[array_size], 0x00);
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
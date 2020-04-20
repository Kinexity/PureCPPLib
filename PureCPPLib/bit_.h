#pragma once
#ifndef bitsize
#define bitsize(type) (8*sizeof(type))
#endif // !bitsize
#ifndef bt_
#define bt_
#include <intrin.h>
#include "bitset_.h"

namespace PCL {
	using bitset_internal = uint_fast64_t;
	template <std::uint_fast64_t bitset_size>
	class reference {
		template <std::uint_fast64_t bitset_size> friend
			class bitset;
	private:
		bitset<bitset_size>&
			bitset_reference;
		const std::uint_fast64_t
			array_size = (bitset_size - bitset_size % bitsize(bitset_internal)) / bitsize(bitset_internal) + static_cast<uint_fast64_t>(bitset_size % bitsize(bitset_internal) > 0);
		bitset_internal
			bit_mask[8] = {
			static_cast<bitset_internal>(0x01),
			static_cast<bitset_internal>(0x02),
			static_cast<bitset_internal>(0x04),
			static_cast<bitset_internal>(0x08),
			static_cast<bitset_internal>(0x10),
			static_cast<bitset_internal>(0x20),
			static_cast<bitset_internal>(0x40),
			static_cast<bitset_internal>(0x80)
		},
			special_mask[2] = {
			static_cast<bitset_internal>(0xFF),
			static_cast<bitset_internal>(0x00)
		};
		bitset_internal*
			bit_sector = nullptr;
		std::uint_fast32_t
			bit_pos_in_sector = 0;
		uint_fast64_t
			bit_index = 0;
		bool
			value = false;
	public:
		reference(bitset<bitset_size>& bs_ref);
		__cdecl ~reference() = default;
		operator bool();
		reference<bitset_size>
			&operator=(bool new_value) noexcept,
			&operator[](std::uint_fast64_t index),
			&operator~();
	};

	template<std::uint_fast64_t bitset_size>
	inline reference<bitset_size>::reference(bitset<bitset_size>& bs_ref) :
		bitset_reference(bs_ref) {}

	template <std::uint_fast64_t bitset_size>
	inline reference<bitset_size>::operator bool() {
		return (*bit_sector >> bit_pos_in_sector) & 0x1;
	}

	template<std::uint_fast64_t bitset_size>
	inline reference<bitset_size>& reference<bitset_size>::operator=(bool new_value) noexcept {
		bitset_internal& mask = special_mask[new_value];
		*bit_sector = ((*bit_sector ^ mask) | ((0x1 << bit_pos_in_sector) ^ mask)) ^ mask;
		return *this;
	}

	template<std::uint_fast64_t bitset_size>
	inline reference<bitset_size>& reference<bitset_size>::operator[](std::uint_fast64_t index) {
		bit_index = index;
		bit_pos_in_sector = bit_index % bitsize(bitset_internal);
		bit_sector = &(bitset_reference.bitset_array[bit_index / bitsize(bitset_internal)]);
		return *this;
	}

	template<std::uint_fast64_t bitset_size>
	inline reference<bitset_size>& reference<bitset_size>::operator~() {
		*bit_sector ^= bit_mask[bit_pos_in_sector];
		return *this;
	}
}

#endif // !bt_
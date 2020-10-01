#pragma once
#ifndef bitsize
#define bitsize(type) (8*sizeof(type))
#endif // !bitsize
#ifndef bt_
#define bt_
#include <intrin.h>
#include <vector>
#include <bit>

namespace PCL {
	using bitset_internal = uint_fast64_t;
	inline constexpr bitset_internal
		bit_mask[8] = {
		static_cast<bitset_internal>(0x01),
		static_cast<bitset_internal>(0x02),
		static_cast<bitset_internal>(0x04),
		static_cast<bitset_internal>(0x08),
		static_cast<bitset_internal>(0x10),
		static_cast<bitset_internal>(0x20),
		static_cast<bitset_internal>(0x40),
		static_cast<bitset_internal>(0x80)
	};
	class reference {
	private:
		std::vector<bitset_internal>&
			bit_internal_vector_ref;
		bitset_internal*
			bit_sector = nullptr;
		uint_fast32_t
			bit_pos_in_sector = 0;
	public:
		reference(std::vector<bitset_internal>& bs_ref);
		~reference() = default;
		operator bool();
		reference
			&operator=(bool new_value) noexcept,
			&operator[](size_t index),
			&operator~();
	};

	inline reference::reference(std::vector<bitset_internal>& bs_ref) :
		bit_internal_vector_ref(bs_ref) {}

	inline reference::operator bool() {
		return _bittest64((long long*)bit_sector, bit_pos_in_sector);
	}

	inline reference& reference::operator=(bool new_value) noexcept {
		if (new_value) {
			_bittestandset64((long long*)bit_sector, bit_pos_in_sector);
		}
		else {
			_bittestandreset64((long long*)bit_sector, bit_pos_in_sector);
		}
		return *this;
	}

	inline reference& reference::operator[](size_t index) {
		bit_pos_in_sector = index % bitsize(bitset_internal);
		bit_sector = &(bit_internal_vector_ref[index / bitsize(bitset_internal)]);
		return *this;
	}

	inline reference& reference::operator~() {
		_bittestandcomplement64((long long*)bit_sector, bit_pos_in_sector);
		return *this;
	}
}

#endif // !bt_
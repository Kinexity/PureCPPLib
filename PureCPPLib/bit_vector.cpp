#include "bit_vector.h"

namespace PCL {
	bit_vector::bit_vector(size_t bitset_size) :
		bit_ref(bit_internal_vector) {
		bit_internal_vector.resize(bitset_size / bitsize(bitset_internal) + (bitset_size % bitsize(bitset_internal) > 0));
	}
}
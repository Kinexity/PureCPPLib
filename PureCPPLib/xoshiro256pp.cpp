#include "xoshiro256pp.h"

uint64_t xoshiro256pp::next() {
	const uint64_t result = s[0] + s[3];

	const uint64_t t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = std::rotl(s[3], 45);

	return result;
}

xoshiro256pp::xoshiro256pp(std::array<uint64_t, 4> s_init) : s(s_init) {}

xoshiro256pp::xoshiro256pp() {
	std::random_device rd;
	std::mt19937_64 int_gen(rd());
	for (auto& elem : s) {
		elem = int_gen();
	}
}

void xoshiro256pp::jump() {
	uint64_t s0 = 0;
	uint64_t s1 = 0;
	uint64_t s2 = 0;
	uint64_t s3 = 0;
	for (int i = 0; i < JUMP.size(); i++)
		for (int b = 0; b < 64; b++) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}
			next();
		}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;
}

void xoshiro256pp::long_jump() {
	uint64_t s0 = 0;
	uint64_t s1 = 0;
	uint64_t s2 = 0;
	uint64_t s3 = 0;
	for (int i = 0; i < LONG_JUMP.size(); i++)
		for (int b = 0; b < 64; b++) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}
			next();
		}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;
}

uint64_t xoshiro256pp::operator()() {
	return next();
}

uint64_t xoshiro256pp::min() {
	return std::numeric_limits<uint64_t>::min();
}

uint64_t xoshiro256pp::max() {
	return std::numeric_limits<uint64_t>::max();
}

__m256i xoshiro256pp_avx2::next() {
	const __m256i result = _mm256_add_epi64(s[0], s[3]);

	const __m256i t = _mm256_slli_epi64(s[1], 17);

	s[2] = _mm256_xor_si256(s[2], s[0]);
	s[3] = _mm256_xor_si256(s[3], s[1]);
	s[1] = _mm256_xor_si256(s[1], s[2]);
	s[0] = _mm256_xor_si256(s[0], s[3]);

	s[2] = _mm256_xor_si256(s[2], t);

	s[3] = _mm256_or_si256(
		_mm256_slli_epi64(s[3], 45),
		_mm256_srli_epi64(s[3], 64 - 45)
	);

	return result;
}

xoshiro256pp_avx2::xoshiro256pp_avx2(std::array<__m256i, 4> s_init) {
	for (int i = 0; i < 4; ++i) {
		s[i] = s_init[i];
	}
}

xoshiro256pp_avx2::xoshiro256pp_avx2() {
	std::random_device rd;
	std::mt19937_64 int_gen(rd());
	for (auto& elem : s) {
		elem = _mm256_set_epi64x(int_gen(), int_gen(), int_gen(), int_gen());
	}
}

void xoshiro256pp_avx2::jump() {
	__m256i s0 = _mm256_setzero_si256();
	__m256i s1 = _mm256_setzero_si256();
	__m256i s2 = _mm256_setzero_si256();
	__m256i s3 = _mm256_setzero_si256();
	for (int i = 0; i < 4; i++) {
		for (int b = 0; b < 64; b++) {
			if (_mm256_extract_epi64(JUMP[i], 0) & (UINT64_C(1) << b)) {
				s0 = _mm256_xor_si256(s0, s[0]);
				s1 = _mm256_xor_si256(s1, s[1]);
				s2 = _mm256_xor_si256(s2, s[2]);
				s3 = _mm256_xor_si256(s3, s[3]);
			}
			next();
		}
	}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;
}

void xoshiro256pp_avx2::long_jump() {
	__m256i s0 = _mm256_setzero_si256();
	__m256i s1 = _mm256_setzero_si256();
	__m256i s2 = _mm256_setzero_si256();
	__m256i s3 = _mm256_setzero_si256();
	for (int i = 0; i < 4; i++) {
		for (int b = 0; b < 64; b++) {
			if (_mm256_extract_epi64(LONG_JUMP[i], 0) & (UINT64_C(1) << b)) {
				s0 = _mm256_xor_si256(s0, s[0]);
				s1 = _mm256_xor_si256(s1, s[1]);
				s2 = _mm256_xor_si256(s2, s[2]);
				s3 = _mm256_xor_si256(s3, s[3]);
			}
			next();
		}
	}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;
}

__m256i xoshiro256pp_avx2::operator()() {
	return next();
}

__m256i xoshiro256pp_avx2::min() {
	return _mm256_set1_epi64x(std::numeric_limits<uint64_t>::min());
}

__m256i xoshiro256pp_avx2::max() {
	return _mm256_set1_epi64x(std::numeric_limits<uint64_t>::max());
}
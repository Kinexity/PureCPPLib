#pragma once
#ifndef xo256_h
#define xo256_h
#include <stdint.h>
#include <bit>
#include <array>
#include <random>
#include <limits>
#include <immintrin.h>
#undef min
#undef max

class xoshiro256pp {
private:
	std::array<uint64_t, 4> s;
	static const std::array<uint64_t, 4> JUMP;
	static const std::array<uint64_t, 4> LONG_JUMP;

public:
	typedef uint64_t result_type;
	explicit xoshiro256pp(std::array<uint64_t, 4> s_init);
	xoshiro256pp();
	uint64_t next();
	void jump();
	void long_jump();
	uint64_t operator()();
	static uint64_t min();
	static uint64_t max();
};

inline const std::array<uint64_t, 4> xoshiro256pp::JUMP = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };
inline const std::array<uint64_t, 4> xoshiro256pp::LONG_JUMP = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

class xoshiro256pp_avx2 {
private:
    __m256i s[4];
    static const __m256i JUMP[4];
    static const __m256i LONG_JUMP[4];

public:
    typedef __m256i result_type;
    explicit xoshiro256pp_avx2(std::array<__m256i, 4> s_init);
    xoshiro256pp_avx2();
    __m256i next();
    void jump();
    void long_jump();
    __m256i operator()();
    static __m256i min();
    static __m256i max();
};

inline const __m256i xoshiro256pp_avx2::JUMP[4] = {
    _mm256_set1_epi64x(0x180ec6d33cfd0aba),
    _mm256_set1_epi64x(0xd5a61266f0c9392c),
    _mm256_set1_epi64x(0xa9582618e03fc9aa),
    _mm256_set1_epi64x(0x39abdc4529b1661c)
};

inline const __m256i xoshiro256pp_avx2::LONG_JUMP[4] = {
    _mm256_set1_epi64x(0x76e15d3efefdcbbf),
    _mm256_set1_epi64x(0xc5004e441c522fb3),
    _mm256_set1_epi64x(0x77710069854ee241),
    _mm256_set1_epi64x(0x39109bb02acbe635)
};

#endif // !xo256_h
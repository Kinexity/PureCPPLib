#pragma once
#include <immintrin.h>

inline __m256d&& operator+(const __m256d& vec1, const __m256d& vec2) {
	return _mm256_add_pd(vec1, vec2);
}

inline __m256d&& operator-(const __m256d& vec1, const __m256d& vec2) {
	return _mm256_sub_pd(vec1, vec2);
}

inline __m256d&& operator*(const __m256d& vec1, const __m256d& vec2) {
	return _mm256_mul_pd(vec1, vec2);
}

inline __m256d&& operator/(const __m256d& vec1, const __m256d& vec2) {
	return _mm256_div_pd(vec1, vec2);
}

inline __m256d&& operator+(const __m256d&& vec1, const __m256d&& vec2) {
	return _mm256_add_pd(vec1, vec2);
}

inline __m256d&& operator-(const __m256d&& vec1, const __m256d&& vec2) {
	return _mm256_sub_pd(vec1, vec2);
}

inline __m256d&& operator*(const __m256d&& vec1, const __m256d&& vec2) {
	return _mm256_mul_pd(vec1, vec2);
}

inline __m256d&& operator/(const __m256d&& vec1, const __m256d&& vec2) {
	return _mm256_div_pd(vec1, vec2);
}

inline __m256d&& sqrt(const __m256d&& vec) {
	return _mm256_sqrt_pd(vec);
}

inline __m256d&& sqrt(const __m256d& vec) {
	return _mm256_sqrt_pd(vec);
}
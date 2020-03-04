#include <immintrin.h>

template <typename T, size_t N>
class simd {
public:
	simd() = default;
	~simd() = default;
};

template <>
class simd<double, 4> {
private:
	__m256d simd_vec;
public:
	simd() = default;
	simd(__m256d a_sv);
	simd(double* v_ptr);
	~simd() = default;
	inline simd<double, 4>
		& operator+=(simd<double, 4>& sv_2),
		& operator-=(simd<double, 4>& sv_2),
		& operator*=(simd<double, 4>& sv_2),
		& operator/=(simd<double, 4>& sv_2),
		operator+(simd<double, 4>& sv_2),
		operator-(simd<double, 4>& sv_2),
		operator*(simd<double, 4>& sv_2),
		operator/(simd<double, 4>& sv_2);
	operator __m256d();
	friend simd256d fmadd(simd256d& v1, simd256d& v2, simd256d& v3);
};

using simd256d = simd<double, 4>;

inline simd<double, 4>::simd(__m256d a_sv) {
	simd_vec = a_sv;
}

inline simd<double, 4>::simd(double* v_ptr) {
	simd_vec = _mm256_load_pd(v_ptr);
}

inline simd<double, 4>& simd<double, 4>::operator+=(simd<double, 4>& sv_2) {
	simd_vec = _mm256_add_pd(simd_vec, sv_2.simd_vec);
	return *this;
}

inline simd<double, 4>& simd<double, 4>::operator-=(simd<double, 4>& sv_2) {
	simd_vec = _mm256_sub_pd(simd_vec, sv_2.simd_vec);
	return *this;
}

inline simd<double, 4> simd<double, 4>::operator+(simd<double, 4>& sv_2) {
	return _mm256_add_pd(simd_vec, sv_2.simd_vec);
}

inline simd<double, 4> simd<double, 4>::operator/(simd<double, 4>& sv_2) {
	return _mm256_div_pd(simd_vec, sv_2.simd_vec);
}

inline simd<double, 4>::operator __m256d() {
	return simd_vec;
}

inline simd<double, 4> simd<double, 4>::operator*(simd<double, 4>& sv_2) {
	return _mm256_mul_pd(simd_vec, sv_2.simd_vec);
}

inline simd<double, 4> simd<double, 4>::operator-(simd<double, 4>& sv_2) {
	return _mm256_sub_pd(simd_vec, sv_2.simd_vec);
}

inline simd<double, 4>& simd<double, 4>::operator/=(simd<double, 4>& sv_2) {
	simd_vec = _mm256_div_pd(simd_vec, sv_2.simd_vec);
	return *this;
}

inline simd<double, 4>& simd<double, 4>::operator*=(simd<double, 4>& sv_2) {
	simd_vec = _mm256_mul_pd(simd_vec, sv_2.simd_vec);
	return *this;
}
#pragma once
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <execution>
#include <iostream>
#include <atomic>
#include <boost/multiprecision/cpp_bin_float.hpp>
using float128 = boost::multiprecision::cpp_bin_float_quad;
using float256 = boost::multiprecision::number<boost::multiprecision::backends::cpp_bin_float<237, boost::multiprecision::backends::digit_base_2, void, boost::int32_t, -262142, 262143>, boost::multiprecision::et_off>;

template <typename NumType>
class polynomial {
private:
	std::vector<NumType> coefficients = { NumType() };
	void
		clear_coefficients_vec(std::vector<NumType>& vec);
public:
	polynomial() = default;
	polynomial(std::vector<NumType> vec);
	template <typename K>
	polynomial(std::initializer_list<K> vec);
	polynomial(NumType coefficient_value, size_t coefficient_index);
	~polynomial() = default;
	template <typename NumberNumTypeype>
	polynomial(NumberNumTypeype num);
	size_t
		degree() const;
	polynomial<NumType>
		operator+(polynomial<NumType>& poly_),
		operator-(polynomial<NumType>& poly_),
		operator*(polynomial<NumType>& poly_),
		operator/(NumType divisor),
		operator+(NumType divisor),
		operator-(NumType divisor),
		operator*(NumType divisor),
		& operator+=(polynomial<NumType>& poly_),
		& operator-=(polynomial<NumType>& poly_),
		& operator-=(polynomial<NumType>&& poly_),
		& operator*=(polynomial<NumType>& poly_),
		operator-(),
		operator+();
	std::pair<polynomial<NumType>, polynomial<NumType>>
		operator/(polynomial<NumType>& poly_);
	polynomial<NumType>
		integral(),
		derivate();
	template <typename NewNumType>
	operator polynomial<NewNumType>();
	template <typename K>
	K
		operator()(K arg);
	std::vector<NumType>&
		internal_vector();
	NumType
		operator[](size_t index);
	template <typename NumType>
	friend std::ostream& operator<<(std::ostream& os, polynomial<NumType> poly);
};

template<typename NumType>
inline void polynomial<NumType>::clear_coefficients_vec(std::vector<NumType>& vec) {
	while (vec.size() != 0 && vec[vec.size() - 1] == NumType()) {
		vec.pop_back();
	};
}

template<typename NumType>
inline polynomial<NumType>::polynomial(std::vector<NumType> vec) {
	coefficients = vec;
}

template<typename NumType>
inline polynomial<NumType>::polynomial(NumType coefficient_value, size_t coefficient_index) {
	coefficients.resize(coefficient_index + 1);
	coefficients[coefficient_index] = coefficient_value;
}

template<typename NumType>
template <typename K>
inline polynomial<NumType>::polynomial(std::initializer_list<K> vec) {
	coefficients = vec;
}

template<typename NumType>
inline size_t polynomial<NumType>::degree() const {
	if (coefficients.size() == 0 || coefficients.size() - 1 == 0 && coefficients.at(0) == 0) {
		//return -std::numeric_limits<size_t>::infinity();
		return size_t();
	}
	return coefficients.size() - 1;
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::operator+(polynomial<NumType>& poly_) {
	auto bigger = (coefficients.size() >= poly_.coefficients.size() ? coefficients : poly_.coefficients);
	auto smaller = (coefficients.size() >= poly_.coefficients.size() ? poly_.coefficients : coefficients);
	std::transform(std::execution::par_unseq, bigger.begin(), bigger.begin() + smaller.size(), smaller.begin(), bigger.begin(), std::plus<NumType>());
	clear_coefficients_vec(bigger);
	return bigger;
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::operator+() {
	return *this;
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::operator-() {
	auto coefs_copy = coefficients;
	std::for_each(std::execution::par_unseq, coefs_copy.begin(), coefs_copy.end(), std::negate<NumType>());
	return coefs_copy;
}

template<typename NumType>
inline std::pair<polynomial<NumType>, polynomial<NumType>> polynomial<NumType>::operator/(polynomial<NumType>& poly_) {
	polynomial<NumType>
		this_copy = *this,
		result;
	while (this_copy.degree() >= poly_.degree()) {
		auto coef = this_copy.internal_vector().back() / poly_.internal_vector().back();
		auto partial_result = polynomial<NumType>(coef, this_copy.degree() - poly_.degree());
		result += partial_result;
		this_copy -= poly_ * partial_result;
	}
	return { result, this_copy };
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::operator+(NumType value) {
	auto coeffs = coefficients;
	if (coeffs.size() < 1) {
		coeffs.resize(1);
	}
	coeffs[0] += value;
	return coeffs;
}

template<typename NumType, typename K>
inline polynomial<NumType> operator+(K value, polynomial<NumType> poly) {
	return poly + value;
}

template<typename NumType, typename K>
inline polynomial<NumType> operator-(K value, polynomial<NumType> poly) {
	return -poly + value;
}

template<typename NumType, typename K>
inline polynomial<NumType> operator*(K value, polynomial<NumType> poly) {
	return poly * value;
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::operator*(NumType multiplier) {
	auto coeffs = coefficients;
	std::for_each(std::execution::par_unseq, coeffs.begin(), coeffs.end(), [&](auto& value) { value *= multiplier; });
	return coeffs;
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::operator-(NumType value) {
	auto coeffs = coefficients;
	if (coeffs.size() < 1) {
		coeffs.resize(1);
	}
	coeffs[0] -= value;
	return coeffs;
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::operator/(NumType divisor) {
	auto coeffs = coefficients;
	std::for_each(std::execution::par_unseq, coeffs.begin(), coeffs.end(), [&](auto& value) { value /= divisor; });
	return coeffs;
}

template<typename NumType>
inline polynomial<NumType>& polynomial<NumType>::operator*=(polynomial<NumType>& poly_) {
	*this = *this * poly_;
	return *this;
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::integral() {
	std::vector<NumType> coefficients_copy = coefficients;
	std::for_each(std::execution::par_unseq, coefficients_copy.begin(), coefficients_copy.end(), [&](NumType& coef) {
		coef /= (std::distance(&coefficients_copy[0], &coef) + 1);
	});
	coefficients_copy.insert(coefficients_copy.begin(), NumType());
	return coefficients_copy;
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::derivate() {
	std::vector<NumType> coefficients_copy = coefficients;
	std::for_each(std::execution::par_unseq, coefficients_copy.begin(), coefficients_copy.end(), [&](NumType& coef) {
		coef *= std::distance(&coefficients_copy[0], &coef);
	});
	coefficients_copy.erase(coefficients_copy.begin());
	return polynomial<NumType>(coefficients_copy);
}

template<typename NumType>
template<typename NewNumType>
inline polynomial<NumType>::operator polynomial<NewNumType>() {
	std::vector<NewNumType> coefs(coefficients.size());
	std::transform(std::execution::par_unseq, coefficients.begin(), coefficients.end(), coefs.begin(), [&](auto elem) { return (NewNumType)elem; });
	return coefs;
}

template<typename NumType>
template <typename K>
inline K polynomial<NumType>::operator()(K arg) {
	K res = K();
	for (auto coef_it = coefficients.rbegin(); coef_it != coefficients.rend(); coef_it++) {
		res = fma(res, arg, *coef_it);
	}
	return res;
}

template<typename NumType>
inline std::vector<NumType>& polynomial<NumType>::internal_vector() {
	return coefficients;
}

template<typename NumType>
inline NumType polynomial<NumType>::operator[](size_t index) {
	return coefficients[index];
}

template<typename NumType>
inline polynomial<NumType>& polynomial<NumType>::operator-=(polynomial<NumType>& poly_) {
	*this = *this - poly_;
	return *this;
}

template<typename NumType>
inline polynomial<NumType>& polynomial<NumType>::operator-=(polynomial<NumType>&& poly_) {
	*this = *this - poly_;
	return *this;
}

template<typename NumType>
inline polynomial<NumType>& polynomial<NumType>::operator+=(polynomial<NumType>& poly_) {
	*this = *this + poly_;
	return *this;
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::operator-(polynomial<NumType>& poly_) {
	std::remove_reference_t<decltype(poly_)> poly_copy = poly_;
	auto bigger = (coefficients.size() >= poly_.coefficients.size() ? coefficients : (-poly_copy).coefficients);
	auto smaller = (coefficients.size() >= poly_.coefficients.size() ? (-poly_copy).coefficients : coefficients);
	std::transform(std::execution::par_unseq, bigger.begin(), bigger.begin() + smaller.size(), smaller.begin(), bigger.begin(), std::plus<NumType>());
	clear_coefficients_vec(bigger);
	return bigger;
}

template<typename NumType>
inline polynomial<NumType> polynomial<NumType>::operator*(polynomial<NumType>& poly_) {
	decltype(coefficients) bigger = (coefficients.size() >= poly_.coefficients.size() ? coefficients : poly_.coefficients);
	decltype(coefficients) smaller = (coefficients.size() >= poly_.coefficients.size() ? poly_.coefficients : coefficients);
	std::vector<NumType> result;
	result.resize(std::max(static_cast<int_fast64_t>(bigger.size()) + static_cast<int_fast64_t>(smaller.size()) - 1, int_fast64_t()));
	std::for_each(bigger.begin(), bigger.end(), [&](auto& val1) {
		if (val1 != NumType()) {
			std::for_each(smaller.begin(), smaller.end(), [&](auto& val2) {
				result[std::distance(&*bigger.begin(), &val1) + std::distance(&*smaller.begin(), &val2)] += val1 * val2;
			});
		}
	});
	return result;
}

template<typename NumType>
template<typename NumberNumTypeype>
inline polynomial<NumType>::polynomial(NumberNumTypeype num) {
	coefficients = { static_cast<NumType>(num) };
}


template <typename NumType>
std::ostream& operator<<(std::ostream& os, polynomial<NumType> poly) {
	if (poly.coefficients.size() == 0) {
		os << '0';
		return os;
	}
	os << "w(x) = ";
	for (int_fast64_t index = poly.coefficients.size() - 1; index >= 0; index--) {
		os << poly.coefficients[index] << " * x^" << index << (index != 0 ? " + " : "");
	}
	return os;
}

//template<typename NumNumTypeype, typename NumType>
//inline polynomial<NumType> operator+(NumNumTypeype number, polynomial<NumType> poly) {
//	return poly + number;
//}
//
//template<typename NumNumTypeype, typename NumType>
//inline polynomial<NumType> operator-(NumNumTypeype number, polynomial<NumType> poly) {
//	return -poly + number;
//}
//
//template<typename NumNumTypeype, typename NumType>
//inline polynomial<NumType> operator*(NumNumTypeype number, polynomial<NumType> poly) {
//	return poly * number;
//}

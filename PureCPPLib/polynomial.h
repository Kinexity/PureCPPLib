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

template <typename T>
class polynomial {
private:
	std::vector<T> coefficients;
public:
	polynomial() = default;
	polynomial(std::vector<T> vec);
	template <typename K>
	polynomial(std::initializer_list<K> vec);
	~polynomial() = default;
	template <typename NumberType>
	polynomial(NumberType num);
	size_t
		degree() const;
	polynomial<T>
		operator+(polynomial<T>& poly_),
		operator-(polynomial<T>& poly_),
		operator*(polynomial<T>& poly_),
		operator+(T divisor),
		operator-(T divisor),
		operator*(T divisor),
		operator/(T divisor),
		& operator+=(polynomial<T>& poly_),
		& operator-=(polynomial<T>& poly_),
		& operator*=(polynomial<T>& poly_)/*,
		&operator/=(polynomial<T>& poly_)*/;
	T
		operator()(T arg);
	template <typename PolyType>
	friend std::ostream& operator<<(std::ostream& os, polynomial<PolyType> poly);
};

template<typename T>
inline polynomial<T>::polynomial(std::vector<T> vec) {
	coefficients = vec;
}

template<typename T>
template <typename K>
inline polynomial<T>::polynomial(std::initializer_list<K> vec) {
	coefficients = vec;
}

template<typename T>
inline size_t polynomial<T>::degree() const {
	if (coefficients.size() - 1 == 0 && coefficients.at(0) == 0) {
		return -std::numeric_limits<size_t>::infinity();
	}
	return coefficients.size() - 1;
}

template<typename T>
inline polynomial<T> polynomial<T>::operator+(polynomial<T>& poly_) {
	auto bigger = (coefficients.size() >= poly_.coefficients.size() ? coefficients : poly_.coefficients);
	auto smaller = (coefficients.size() >= poly_.coefficients.size() ? poly_.coefficients : coefficients);
	std::transform(std::execution::par, bigger.begin(), bigger.begin() + smaller.size(), smaller.begin(), bigger.begin(), std::plus<T>());
	while (bigger.size() != 0 && bigger[bigger.size() - 1] == T()) {
		bigger.pop_back();
	};
	return bigger;
}

template<typename T>
inline polynomial<T> polynomial<T>::operator+(T value) {
	auto coeffs = coefficients;
	if (coeffs.size() < 1) {
		coeffs.resize(1);
	}
	coeffs[0] += value;
	if (coeffs.size() == 1 && coeffs[0] == T()) {
		coeffs.pop_back();
	}
	return coeffs;
}

template<typename T, typename K>
inline polynomial<T> operator+(K value, polynomial<T> poly) {
	return poly + value;
}

template<typename T, typename K>
inline polynomial<T> operator*(K value, polynomial<T> poly) {
	return poly * value;
}

template<typename T>
inline polynomial<T> polynomial<T>::operator*(T multiplier) {
	auto coeffs = coefficients;
	std::transform(std::execution::par, coeffs.begin(), coeffs.end(), coeffs.begin(), [&](auto value) { return value * multiplier; });
	return coeffs;
}

template<typename T>
inline polynomial<T> polynomial<T>::operator-(T value) {
	auto coeffs = coefficients;
	if (coeffs.size() < 1) {
		coeffs.resize(1);
	}
	coeffs[0] -= value;
	if (coeffs.size() == 1 && coeffs[0] == T()) {
		coeffs.pop_back();
	}
	return coeffs;
}

template<typename T>
inline polynomial<T> polynomial<T>::operator/(T divisor) {
	auto coeffs = coefficients;
	std::transform(std::execution::par, coeffs.begin(), coeffs.end(), coeffs.begin(), [&](auto value) { return value / divisor; });
	return coeffs;
}

template<typename T>
inline polynomial<T>& polynomial<T>::operator*=(polynomial<T>& poly_) {
	*this = *this * poly_;
	return *this;
}

template<typename T>
inline T polynomial<T>::operator()(T arg) {
	return std::transform_reduce(std::execution::par, coefficients.begin(), coefficients.end(), T(), std::plus<T>(), [&](T& coef) {
		size_t index = std::distance(&*coefficients.begin(), &coef);
		if constexpr (!std::is_same<T, float128>() && !std::is_same<T, float256>()) {
			return coefficients[index] * std::pow(arg, index);
		}
		else {
			return coefficients[index] * boost::multiprecision::pow(arg, index);
		}
	});
}

template<typename T>
inline polynomial<T>& polynomial<T>::operator-=(polynomial<T>& poly_) {
	*this = *this - poly_;
	return *this;
}

template<typename T>
inline polynomial<T>& polynomial<T>::operator+=(polynomial<T>& poly_) {
	*this = *this + poly_;
	return *this;
}

template<typename T>
inline polynomial<T> polynomial<T>::operator-(polynomial<T>& poly_) {
	auto bigger = (coefficients.size() >= poly_.coefficients.size() ? coefficients : poly_.coefficients);
	auto smaller = (coefficients.size() >= poly_.coefficients.size() ? poly_.coefficients : coefficients);
	std::transform(std::execution::par, bigger.begin(), bigger.begin() + smaller.size(), smaller.begin(), bigger.begin(), std::minus<T>());
	while (bigger.size() != 0 && bigger[bigger.size() - 1] == T()) {
		bigger.pop_back();
	};
	return bigger;
}

template<typename T>
inline polynomial<T> polynomial<T>::operator*(polynomial<T>& poly_) {
	decltype(coefficients) bigger = (coefficients.size() >= poly_.coefficients.size() ? coefficients : poly_.coefficients);
	decltype(coefficients) smaller = (coefficients.size() >= poly_.coefficients.size() ? poly_.coefficients : coefficients);
	std::vector<T> result;
	result.resize(std::max(static_cast<int_fast64_t>(bigger.size()) + static_cast<int_fast64_t>(smaller.size()) - 1, int_fast64_t()));
	std::for_each(bigger.begin(), bigger.end(), [&](auto& val1) {
		std::for_each(smaller.begin(), smaller.end(), [&](auto& val2) {
			result[std::distance(&*bigger.begin(), &val1) + std::distance(&*smaller.begin(), &val2)] += val1 * val2;
		});
	});
	return result;
}

template<typename T>
template<typename NumberType>
inline polynomial<T>::polynomial(NumberType num) {
	coefficients.push_back(static_cast<T>(num));
}


template <typename PolyType>
std::ostream& operator<<(std::ostream& os, polynomial<PolyType> poly) {
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

#pragma once
#ifndef n_a
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <execution>

#ifndef test
template <typename Type, typename Return_Type = double>
inline Return_Type numerical_integration(Type* start, Type* end) {
	return (2 * std::accumulate<Type*, Type>(&start[1], &end[-1], Type()) + start[0] + end[-1]) * static_cast<Return_Type>(0.5);
}

#elif defined(test)
template <typename Type, typename Return_Type = double>
inline Return_Type numerical_integration_test(Type* start, Type* end) {
	return (2 * std::reduce(std::execution::par ,&start[1], &end[-1], start[0] + end[-1])) * Return_Type(0.5);
}
#endif // test
#endif // !n_a
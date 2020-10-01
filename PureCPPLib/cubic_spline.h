#pragma once
#include "polynomial.h"

template <typename NumType>
class cubic_spline_fn {
private:
	std::vector<NumType> range_borders;
	std::vector<polynomial<NumType>> polys;
	polynomial<NumType>
		points_pair(NumType x1, NumType y1, NumType x2, NumType y2);
public:
	cubic_spline_fn(std::vector<NumType> x, std::vector<NumType> y);
	~cubic_spline_fn();
};

template<typename NumType>
inline polynomial<NumType> cubic_spline_fn<NumType>::points_pair(NumType x1, NumType y1, NumType x2, NumType y2) {
	return polynomial<NumType>();
}

template <typename NumType>
inline cubic_spline_fn<NumType>::cubic_spline_fn(std::vector<NumType> x, std::vector<NumType> y) {

}

template <typename NumType>
inline cubic_spline_fn<NumType>::~cubic_spline_fn() {}

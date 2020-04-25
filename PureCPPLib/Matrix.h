#pragma once
#ifndef MATRIX_H
#define MATRIX_H
#include <array>
#include <utility>
#include <execution>
#include <numeric>
#include <algorithm>

template <size_t Rows, size_t Columns, typename T = double>
class Matrix {
private:
	std::array<std::array<T, Columns>, Rows> internal_arrays;
	constexpr std::array<size_t, Columns> columns_indices = std::make_index_sequence<Columns>();
	constexpr std::array<size_t, Rows> rows_indices = std::make_index_sequence<Rows>();
public:
	T
		& operator()(size_t m_pos, size_t n_pos);
	Matrix<Columns, Rows, T>&&
		transpose();
};

template<size_t Rows, size_t n, typename T>
inline T& Matrix<Rows, n, T>::operator()(size_t m_pos, size_t n_pos) {
	return internal_arrays[m_pos][n_pos];
}

template<size_t Rows, size_t Columns, typename T>
inline Matrix<Columns, Rows, T>&& Matrix<Rows, Columns, T>::transpose() {
	Matrix<Columns, Rows, T> transposed_matrix;
	std::for_each(std::execution::par_unseq, rows_indices.begin(), rows_indices.end(), [&](size_t row_index) {
		std::for_each(std::execution::par_unseq, columns_indices.begin(), columns_indices.end(), [&](size_t column_index) {
			transposed_matrix(column_index, row_index) = internal_arrays[row_index][column_index];
		});
	});
}

#endif // !MATRIX_H
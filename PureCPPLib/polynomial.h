#pragma once
#include <vector>
#include <limits>

template <typename T>
class polynomial {
private:
	std::vector<T> coefficients;
public:
	polynomial();
	polynomial(std::vector<T> vec);
	~polynomial() = default;
	size_t
		degree() const;
	polynomial<T>
		operator+(polynomial<T>& poly_),
		operator-(polynomial<T>& poly_),
		operator*(polynomial<T>& poly_),
		operator/(polynomial<T>& poly_),
		&operator+=(polynomial<T>& poly_),
		&operator-=(polynomial<T>& poly_),
		&operator*=(polynomial<T>& poly_)/*,
		&operator/=(polynomial<T>& poly_)*/;
};

template<typename T>
inline polynomial<T>::polynomial() {
	coefficients.push_back(T());
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
	return polynomial<T>();
}

template<typename T>
inline polynomial<T>& polynomial<T>::operator*=(polynomial<T>& poly_)
{
	// TODO: tu wstawiæ instrukcjê return
}

template<typename T>
inline polynomial<T>& polynomial<T>::operator-=(polynomial<T>& poly_)
{
	// TODO: tu wstawiæ instrukcjê return
}

template<typename T>
inline polynomial<T>& polynomial<T>::operator+=(polynomial<T>& poly_)
{
	// TODO: tu wstawiæ instrukcjê return
}

template<typename T>
inline polynomial<T> polynomial<T>::operator-(polynomial<T>& poly_)
{
	return polynomial<T>();
}

template<typename T>
inline polynomial<T> polynomial<T>::operator*(polynomial<T>& poly_)
{
	return polynomial<T>();
}

template<typename T>
inline polynomial<T> polynomial<T>::operator/(polynomial<T>& poly_)
{
	return polynomial<T>();
}

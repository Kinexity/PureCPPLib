#pragma once
#ifndef CF_H
#define CF_H
#include <array>
#include <vector>
#include <numeric>
#include <algorithm>
#include <utility>
#include <cmath>
#include <iostream>
#include <dlib/optimization.h>

inline std::function<double(double, double, double, double, double)> _fn_;

typedef dlib::matrix<double, 1, 1> input_vector;
typedef dlib::matrix<double, 4, 1> parameter_vector;

double model(const input_vector& input, const parameter_vector& params) {
	return _fn_(input(0), params(0), params(1), params(2), params(4));
}

double residual(const std::pair<input_vector, double>& data, const parameter_vector& params) {
	return model(data.first, params) - data.second;
}

parameter_vector residual_derivative(const std::pair<input_vector, double>& data, const parameter_vector& params) {
	const double inv_gamma = params(0);
	const double x_0 = params(1);
	const double beta = params(2);
	const double x = data.first(0);
	return { inv_gamma / (x * (-std::log(x) + beta * inv_gamma * x_0 * x - beta * inv_gamma)) };
}

inline std::array<double, 3> curve_fit(std::function<double(double, double, double, double, double)> fitted_function, std::vector<double>& x_data, std::vector<double>& y_data, const parameter_vector starting_parameters) {
	const parameter_vector params = parameter_vector() + 1.;
	std::cout << "params: " << trans(params) << '\n';
	std::vector<std::pair<input_vector, double>> data;
	input_vector input;
	for (size_t index = 0; index < x_data.size(); index++) {
		data.push_back({ {x_data[index]},y_data[index] });
	}
	_fn_ = fitted_function;
	// Before we do anything, let's make sure that our derivative function defined above matches
		// the approximate derivative computed using central differences (via derivative()).  
		// If this value is big then it means we probably typed the derivative function incorrectly.
	std::cout << "derivative error: " << length(residual_derivative(data[0], params) -
		derivative(residual)(data[0], params)) << '\n';





	// Now let's use the solve_least_squares_lm() routine to figure out what the
	// parameters are based on just the data.
	parameter_vector x;
	x = 1;

	std::cout << "Use Levenberg-Marquardt" << '\n';
	// Use the Levenberg-Marquardt method to determine the parameters which
	// minimize the sum of all squared residuals.
	solve_least_squares_lm(dlib::objective_delta_stop_strategy(1e-7).be_verbose(),
		residual,
		residual_derivative,
		data,
		x);

	// Now x contains the solution.  If everything worked it will be equal to params.
	std::cout << "inferred parameters: " << trans(x) << '\n';
	std::cout << "solution error:      " << length(x - params) << '\n';
	std::cout << '\n';




	x = 1;
	std::cout << "Use Levenberg-Marquardt, approximate derivatives" << '\n';
	// If we didn't create the residual_derivative function then we could
	// have used this method which numerically approximates the derivatives for you.
	solve_least_squares_lm(dlib::objective_delta_stop_strategy(1e-7).be_verbose(),
		residual,
		derivative(residual),
		data,
		x);

	// Now x contains the solution.  If everything worked it will be equal to params.
	std::cout << "inferred parameters: " << trans(x) << '\n';
	std::cout << "solution error:      " << length(x - params) << '\n';
	std::cout << '\n';




	x = 1;
	std::cout << "Use Levenberg-Marquardt/quasi-newton hybrid" << '\n';
	// This version of the solver uses a method which is appropriate for problems
	// where the residuals don't go to zero at the solution.  So in these cases
	// it may provide a better answer.
	solve_least_squares(dlib::objective_delta_stop_strategy(1e-7).be_verbose(),
		residual,
		residual_derivative,
		data,
		x);

	// Now x contains the solution.  If everything worked it will be equal to params.
	std::cout << "inferred parameters: " << trans(x) << '\n';
	std::cout << "solution error:      " << length(x - params) << '\n';

	return {};
}
#endif // !CF_H
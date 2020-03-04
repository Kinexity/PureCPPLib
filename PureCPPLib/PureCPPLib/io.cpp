#include "io.h"

bool input_error() {
	bool
		state = std::cin.fail();
	if (std::cin.fail()) {
		std::cout << line << std::endl;
		std::cout << "Blad wczytywania danych!" << std::endl;
		std::cin.clear();
		std::cin.ignore(max_strm_size, '\n');
	}
	return state;
}

bool incorrect_value(bool expression_value) {
	if (expression_value) {
		std::cout << line << std::endl << "Bledny wybor!" << std::endl;
	}
	return expression_value;
}

std::wstring getenv_wstr(std::string env_name) {
	std::string temp = getenv(env_name.c_str());
	return std::wstring().assign(temp.begin(), temp.end());
}
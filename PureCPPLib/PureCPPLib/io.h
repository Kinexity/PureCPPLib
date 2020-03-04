#pragma once
#include <string>
#include <limits>
#include <iostream>
#pragma warning(disable:4996)

const std::string
line = "<---------------->";
constexpr uint_fast64_t
max_strm_size = std::numeric_limits<std::streamsize>::max();

bool input_error();

bool incorrect_value(bool expression_value);

std::wstring getenv_wstr(std::string env_name);
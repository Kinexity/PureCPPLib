#pragma once
#ifndef C_E_L
#define C_E_L
#include <fstream>
#include <string>
#include <chrono>
#include <mutex>
#include <iostream>
#include <sstream>
#include <exception>
#include <thread>
#include <chrono>
#include "C_Event_Log_Base.h"
#include "C_Event_Log_Buffer.h"
#undef max

std::string filename_string(std::string path_str);

#define _endl_ " (" << filename_string(__FILE__) << "; " << __LINE__ << ")" << std::endl<char, std::char_traits<char>>
#define checkpoint(event_log) event_log() << "checkpoint" << _endl_

namespace PCL {
	class C_Event_Log : public C_Event_Log_Base {
		bool
			expected_end = false;
		std::shared_ptr<std::ofstream>
			event_log_file = std::make_unique<std::ofstream>();
		std::shared_ptr<std::stringstream>
			temp_output = std::make_unique<std::stringstream>();
		bool
			fail = false;
	public:
		C_Event_Log(bool deb_mode = false);
		C_Event_Log(C_Event_Log&) = default;
		C_Event_Log(C_Event_Log&&) = default;
		~C_Event_Log();
		void
			finish_expected(),
			create_log_file();
	};
}

using namespace std::chrono;

template <bool is_important>
std::string operation_evaluation(bool result, std::string positive = "success", std::string negative = "fail") {
	if (result) {
		return std::string(" - ") + positive;
	}
	else {
		std::thread([&]()->void {
			std::this_thread::sleep_for(500ms);
			if (is_important) {
				throw std::runtime_error("Critical error occurred! Cannot continue without potential data damage!\nTake a look into log files for more info or send them to the developer.");
			}
		}).detach();
		return std::string(" - ") + negative;
	}
}

#endif // !C_E_L
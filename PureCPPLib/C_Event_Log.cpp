#include "C_Event_Log.h"

namespace PCL {
	C_Event_Log::C_Event_Log(bool deb_mode) :
	C_Event_Log_Base(deb_mode) {
		stream = temp_output;
	}

	C_Event_Log::~C_Event_Log() {
		if (expected_end) {
			*event_log_file << get_date_string(msg_type::log) << "Expected program shutdown!" << _endl_;
		}
		if (event_log_file->is_open()) {
			event_log_file->close();
		}
	}

	void C_Event_Log::finish_expected() {
		expected_end = true;
	}

	void C_Event_Log::create_log_file() {
		std::unique_lock<decltype(C_Event_Log_Base::mut_ex)> guard(mut_ex);
		event_log_file->open(std::string("logs/") + get_date_string().substr(0, 21) + std::string("Log_file.txt"));
		if (!event_log_file->is_open() || !*event_log_file) {
			throw std::runtime_error("Couldn't create log file!");
		}
		*event_log_file << temp_output->str();
		stream = event_log_file;
	}
}
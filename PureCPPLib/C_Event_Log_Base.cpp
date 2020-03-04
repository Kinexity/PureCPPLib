#include "C_Event_Log_Base.h"

namespace PCL {
	std::string mt_to_string(msg_type m_t) {
		switch (m_t) {
		case PCL::msg_type::error:
			return "ERR ";
			break;
		case PCL::msg_type::warning:
			return "WRN ";
			break;
		case PCL::msg_type::log:
			return "LOG ";
			break;
		case PCL::msg_type::none:
			return "";
			break;
		}
		return std::string();
	}

	std::string get_date_string(msg_type m_t) {
		const time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm date;
		::localtime_s(&date, &current_time);
		char buffer[100];
		strftime(buffer, 100, "%Y-%m-%d %H-%M-%S", &date);
		return std::string("[") + mt_to_string(m_t) + std::string(buffer) + std::string("] ");
	}
}
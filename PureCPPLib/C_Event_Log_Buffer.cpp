#include "C_Event_Log_Buffer.h"

namespace PCL {
	std::string C_Event_Log_Buffer::conditional_get_date_string(msg_type m_t) {
		return (date_strings ? (std::string("[") + get_date_string(m_t) + "] ") : "");
	};

	C_Event_Log_Buffer::C_Event_Log_Buffer(C_Event_Log_Base& el_obj, bool include_date_strings, std::string custom_message)
		: event_log_obj(el_obj),
		message(custom_message) {
		std::unique_lock<decltype(C_Event_Log_Base::mut_ex)> guard(mut_ex);
		date_strings = include_date_strings;
		stream = buffer;
		*buffer  << get_date_string() << "<" << message << ">" << std::endl;
	}

	C_Event_Log_Buffer::~C_Event_Log_Buffer() {
		std::unique_lock<decltype(C_Event_Log_Base::mut_ex)> guard(mut_ex);
		*buffer << get_date_string() << "</" << message << ">" << std::endl;
		if (!abort_flag) {
			event_log_obj << buffer->str();
		}
	}

	void C_Event_Log_Buffer::abort() {
		abort_flag = true;
	}
}
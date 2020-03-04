#pragma once
#ifndef C_ELB
#define C_ELB
#include <sstream>
#include <string>
#include <mutex>
#include "C_Event_Log.h"

namespace PCL {
	class C_Event_Log;

	class C_Event_Log_Buffer : public C_Event_Log_Base {
	private:
		C_Event_Log_Base &
			event_log_obj;
		const std::string
			message;
		std::shared_ptr<std::stringstream>
			buffer = std::make_shared<std::stringstream>();
		std::string
			conditional_get_date_string(msg_type m_t = msg_type::log);
		bool
			abort_flag = false;
	public:
		C_Event_Log_Buffer(C_Event_Log_Base& el_obj, bool include_date_strings, std::string custom_message = std::string("BUFFER OUTPUT"));
		C_Event_Log_Buffer(C_Event_Log_Buffer&) = default;
		~C_Event_Log_Buffer();
		void
			abort();
	};
}

#endif // !C_ELB
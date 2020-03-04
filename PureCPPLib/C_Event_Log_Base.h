#pragma once
#ifndef C_E_L_B
#define C_E_L_B
#include <mutex>
#include <iostream>
#include <fstream>
#include <memory>
#include <ctime>
#include <chrono>
#include <string>
#include <atomic>
#include <sstream>

namespace PCL {
	enum class msg_type {
		error,
		warning,
		log,
		none
	};

	std::string mt_to_string(msg_type m_t);

	class C_Event_Log_Line {
	private:

	public:
		C_Event_Log_Line() = default;
		~C_Event_Log_Line() = default;
		template <typename T> C_Event_Log_Line&
			operator<<(T str);
	};

	class C_Event_Log_Base {
	protected:
		std::mutex
			mut_ex;
		std::weak_ptr<std::ostream>
			stream;
		bool
			date_strings = true;
		const bool
			debug_mode;
	public:
		C_Event_Log_Base(bool deb_mode = false) noexcept;
		virtual ~C_Event_Log_Base() = default;
		template <typename T> C_Event_Log_Base&
			operator<<(T str);
		C_Event_Log_Base&
			operator()(msg_type m_t = msg_type::log);
	};

	template<typename T>
	inline C_Event_Log_Base& C_Event_Log_Base::operator<<(T str) {
		if (!stream.expired()) {
			*stream.lock() << str;
		}
		if (debug_mode) {
			std::cout << str;
		}
		return *this;
	}

	std::string get_date_string(msg_type m_t = msg_type::none);

	inline C_Event_Log_Base::C_Event_Log_Base(bool deb_mode) noexcept :
	debug_mode(deb_mode) {}

	inline C_Event_Log_Base& C_Event_Log_Base::operator()(msg_type m_t) {
		return (*this << get_date_string(m_t));
	}
}

#endif // !C_E_L_B
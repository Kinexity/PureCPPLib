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
	class C_Event_Log_Sync;
	class C_Event_Log_Sync_Packed;

	enum class msg_type {
		error,
		warning,
		log,
		none
	};

	std::string mt_to_string(msg_type m_t);

	class C_Event_Log_Base {
		friend class C_Event_Log_Sync;
	private:
		void
			acquire_line(std::string line_str);
	protected:
		std::mutex
			mut_ex;
		std::weak_ptr<std::ostream>
			stream;
		bool
			date_strings = true,
			put_date_string = true;
		const bool
			debug_mode;
	public:
		C_Event_Log_Base(bool deb_mode = false) noexcept;
		virtual ~C_Event_Log_Base() = default;
		template <typename T> C_Event_Log_Sync_Packed&&
			operator<<(T str);
		C_Event_Log_Sync_Packed&&
			operator()(msg_type m_t = msg_type::log);
	};

	class C_Event_Log_Sync {
		friend class C_Event_Log_Sync_Packed;
		friend class C_Event_Log_Base;
	private:
		C_Event_Log_Base&
			event_log_base_ref;
		std::stringstream
			str_stream;
	public:
		C_Event_Log_Sync(C_Event_Log_Base& ev_log_base_ref);
		~C_Event_Log_Sync();
		template <typename T> C_Event_Log_Sync&
			operator<<(T str);
	};

	class C_Event_Log_Sync_Packed {
		friend class C_Event_Log_Sync;
	private:
		std::shared_ptr<C_Event_Log_Sync>
			ptr;
	public:
		C_Event_Log_Sync_Packed(std::shared_ptr<C_Event_Log_Sync>& ptr_ref);
		~C_Event_Log_Sync_Packed() = default;
		template <typename T> C_Event_Log_Sync_Packed&&
			operator<<(T str);
	};

	template<typename T>
	inline C_Event_Log_Sync_Packed && C_Event_Log_Base::operator<<(T str) {
		std::shared_ptr<C_Event_Log_Sync> ptr = std::make_shared<C_Event_Log_Sync>(*this);
		*ptr << str;
		return C_Event_Log_Sync_Packed(ptr);
	}

	std::string get_date_string(msg_type m_t = msg_type::none);

	inline void C_Event_Log_Base::acquire_line(std::string line_str) {
		std::unique_lock<decltype(C_Event_Log_Base::mut_ex)> guard(mut_ex);
		if (!stream.expired()) {
			*stream.lock() << line_str;
		}
		if (debug_mode) {
			std::cout << line_str;
		}
	}

	inline C_Event_Log_Base::C_Event_Log_Base(bool deb_mode) noexcept :
	debug_mode(deb_mode) {}

	inline C_Event_Log_Sync_Packed && C_Event_Log_Base::operator()(msg_type m_t) {
		return (*this << get_date_string(m_t));
	}

	template<typename T>
	inline C_Event_Log_Sync & C_Event_Log_Sync::operator<<(T str) {
		str_stream << str;
		return *this;
	}
	template<typename T>

	inline C_Event_Log_Sync_Packed && C_Event_Log_Sync_Packed::operator<<(T str) {
		*ptr << str;
		return C_Event_Log_Sync_Packed(ptr);
	}
}

#endif // !C_E_L_B
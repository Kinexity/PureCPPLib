#pragma once
#ifndef C_XML_h
#define C_XML_h
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <variant>
#include <regex>
#include <tuple>
#include <list>
#include <variant>
#include <execution>
#include <iostream>
#include <map>
#include <sstream>
#include "adv_math.h"
#include "io.h"
#include "C_Event_Log.h"
#include <unordered_map>


namespace PCL {
	class XML_File;
	class XML_Node;
	class XML_Field;

	std::vector<std::string> string_to_path_vector(std::string str_path);

	std::string get_name(std::variant<XML_Node, XML_Field> xml_variant);

	enum class line_content_type {
		unknown,
		field,
		opening_tag,
		closing_tag
	};

	struct line_t {
		std::string
			content;
		line_content_type
			type_of_content = line_content_type::unknown;
	};

	class XML_Base {
	protected:
		XML_Base(XML_File& parent_file_ref, std::weak_ptr<XML_Node> parent_node_ptr, std::string new_name = "");
		XML_File&
			parent_file;
		C_Event_Log_Base&
			event_log;
		std::weak_ptr<XML_Node>
			parent_node;
	public:
		std::string
			name;
		virtual ~XML_Base() = default;
		std::string
			current_name();
		virtual
			std::string str() = 0;
	};

	class XML_Field : public XML_Base {
		friend std::ostream& operator<< (std::ostream& stream, const XML_Field& field);
	private:
		std::variant<std::string, char, double, long long, unsigned long long>
			value;
	public:
		XML_Field(XML_File& parent_file_ref, std::weak_ptr<XML_Node> parent_node_ptr, std::string field_name, std::variant<std::string, char, double, long long, unsigned long long> field_value);
		template<typename assigned_type>
		XML_Field&
			operator=(assigned_type assigned_value);
		std::string
			str();
		template <typename req_type>
		req_type&
			get();
		template <typename req_type>
		void
			convert();
		XML_Field& operator=(const XML_Field& copy_source);
	};

	class XML_Node : public XML_Base, public std::enable_shared_from_this<XML_Node> {
		friend class XML_File;
		friend std::ostream& operator<< (std::ostream& stream, const XML_Node& node);
	private:
		const uint_fast64_t
			depth = 0;
		std::map<std::string, std::shared_ptr<std::variant<XML_Node, XML_Field>>>
			elements;
		XML_Field
			line_to_field(std::string line_to_conv);
		std::weak_ptr<std::variant<XML_Node, XML_Field>>
			find(std::vector<std::string> path_elems_vec);
		std::vector<line_t>::iterator
			node_end(std::vector<line_t>::iterator first, std::vector<line_t>::iterator last);
		std::string
			get_section_name(std::string name_line);
	public:
		XML_Node() = default;
		XML_Node(XML_File& parent_file_ref, std::weak_ptr<XML_Node> parent_node_ptr, uint_fast64_t node_depth, std::vector<line_t>::iterator first, std::vector<line_t>::iterator last);
		XML_Node(XML_File& parent_file_ref, std::weak_ptr<XML_Node> parent_node_ptr, std::string node_name, uint_fast64_t node_depth);
		~XML_Node() = default;
		void
			insert(std::string name),
			insert(std::string name, std::variant<std::string, char, double, long long, unsigned long long> field_value),
			erase(std::string name),
			rename_element(std::string current_name, std::string new_name);
		std::string
			str();
		template <typename req_type> bool
			exists(std::string name);
		XML_Node& operator=(const XML_Node& copy_source);
		decltype(depth) get_depth() {
			return depth;
		}
	};

	class XML_File {
		friend class XML_Base;
		friend class XML_Node;
		friend std::ostream& operator<< (std::ostream& stream, const XML_File& node_ref);
	private:
		C_Event_Log_Base&
			event_log;
		bool
			is_good = true,
			any_runtime_errors = false,
			search_error = false;
		std::weak_ptr<std::variant<XML_Node, XML_Field>>
			in_operation;
		std::shared_ptr<std::variant<XML_Node, XML_Field>>
			first_node;
		std::fstream
			file;
		std::filesystem::path
			xml_file_path;
		std::vector<std::string>
			in_operation_path;
		line_content_type
			check_line_type(std::string line__);
		bool
			create(std::filesystem::path file_path);
	public:
		bool
			open(std::filesystem::path file_path),
			is_open(),
			close();
		template <typename req_type> bool
			exists(std::string name);
		void
			set_path(std::string elem_path, bool is_absolute = false),
			clear(),
			download(),
			upload();
		XML_Node&
			first();
		operator bool();
		template <typename req_type = XML_Node>
		req_type&
			get(std::string temp_path);
		template <typename req_type = XML_Node>
		req_type&
			get();
		XML_File(C_Event_Log_Base& event_log_ref);
		XML_File(C_Event_Log_Base& event_log_ref, std::filesystem::path file_path);
		~XML_File();
	};

	template<typename assigned_type>
	inline XML_Field & XML_Field::operator=(assigned_type assigned_value) {
		if (std::holds_alternative<assigned_type>(value)) {
			std::get<assigned_type>(value) = assigned_value;
		}
		return *this;
	}

	template<typename req_type>
	inline req_type & XML_Field::get() {
		if (std::holds_alternative<req_type>(value)) {
			return std::get<req_type>(value);
		}
		else {
			event_log() << "Type of value is not " << typeid(req_type).name() << _endl_;
		}

	}

	template<typename req_type>
	inline void XML_Field::convert() {
		if (std::holds_alternative<req_type>(value)) {
			return;
		}
		if (std::holds_alternative<std::string>(value)) {
			//if constexpr (req_type == ) {

			//}
		}
		if (std::holds_alternative<char>(value)) {

		}
		if (std::holds_alternative<double>(value)) {

		}
		if (std::holds_alternative<long long>(value)) {

		}
		if (std::holds_alternative<unsigned long long>(value)) {

		}
	}

	template<typename req_type>
	inline bool XML_File::exists(std::string name) {
		if (in_operation.lock() != nullptr) {
			if (std::holds_alternative<XML_Node>(*in_operation.lock())) {
				return std::get<XML_Node>(*in_operation.lock()).exists<req_type>(name);
			}
			else {
				return false;
			}
		}
		else {
			event_log() << "in_operation was nullptr" << _endl_;
			return false;
		}
	}

	template<typename req_type>
	inline req_type& XML_File::get() {
		if (std::holds_alternative<req_type>(*in_operation.lock())) {
			return std::get<req_type>(*in_operation.lock());
		}
		else {
			event_log() << "Type of in_operation is not " << typeid(req_type).name() << _endl_;
		}
	}

	template<typename req_type>
	inline req_type & XML_File::get(std::string temp_path) {
		auto path = string_to_path_vector(temp_path);
		if (path.size() == 0) {
			return std::get<req_type>(*in_operation.lock());
		}
		return std::get<req_type>(*(std::get<XML_Node>(*in_operation.lock()).find(path)).lock());
	}

	template<typename req_type>
	inline bool XML_Node::exists(std::string name) {
		auto it = elements.find(name);
		return it != elements.end() && std::holds_alternative<req_type>(*it->second);
	}
}

#endif // !C_XML_h
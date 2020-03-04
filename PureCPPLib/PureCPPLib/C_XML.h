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
#include <iostream>
#include "adv_math.h"
#include "io.h"
#include "C_Event_Log.h"
#include <unordered_map>


namespace PCL {
	class C_XML_File;
	class C_XML_Node;
	class C_XML_Field;

	std::vector<std::string> string_to_path_vector(std::string str_path);

	std::string get_name(std::variant<C_XML_Node, C_XML_Field> xml_variant);

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

	class C_XML_Base {
	protected:
		C_XML_Base(C_XML_File& parent_file_ref, C_XML_Node* parent_node_ptr, std::string new_name = "");
		C_XML_File&
			parent_file;
		C_Event_Log_Base&
			event_log;
		C_XML_Node*
			parent_node;
	public:
		std::string
			name;
		virtual ~C_XML_Base() = default;
		std::string
			current_name();
		virtual
			std::string str() = 0;
	};

	class C_XML_Field : public C_XML_Base {
		friend std::ostream& operator<< (std::ostream& stream, const C_XML_Field& field);
	private:
		std::variant<std::string, char, double, long long, unsigned long long>
			value;
	public:
		C_XML_Field(C_XML_File& parent_file_ref, C_XML_Node* parent_node_ptr, std::string field_name, std::variant<std::string, char, double, long long, unsigned long long> field_value);
		template<typename assigned_type>
		C_XML_Field&
			operator=(assigned_type assigned_value);
		std::string
			str();
		template <typename req_type>
		req_type&
			get();
		C_XML_Field& operator=(const C_XML_Field& copy_source);
	};

	class C_XML_Node : public C_XML_Base {
		friend class C_XML_File;
		friend std::ostream& operator<< (std::ostream& stream, const C_XML_Node& node);
	private:
		const uint_fast64_t
			depth = 0;
		std::unordered_map<std::string, std::variant<C_XML_Node, C_XML_Field>>
			elements;
		C_XML_Field
			line_to_field(std::string line_to_conv);
		std::variant<C_XML_Node, C_XML_Field>
			*find(std::vector<std::string> path_elems_vec);
		std::vector<line_t>::iterator
			node_end(std::vector<line_t>::iterator first, std::vector<line_t>::iterator last);
		std::string
			get_section_name(std::string name_line);
	public:
		C_XML_Node() = default;
		C_XML_Node(C_XML_File& parent_file_ref, C_XML_Node* parent_node_ptr, uint_fast64_t node_depth, std::vector<line_t>::iterator first, std::vector<line_t>::iterator last);
		C_XML_Node(C_XML_File& parent_file_ref, C_XML_Node* parent_node_ptr, std::string node_name, uint_fast64_t node_depth);
		~C_XML_Node() = default;
		void
			insert(std::string name),
			insert(std::string name, std::variant<std::string, char, double, long long, unsigned long long> field_value),
			erase(std::string name),
			rename_element(std::string current_name, std::string new_name);
		std::string
			str();
		template <typename req_type> bool
			exists(std::string name);
		C_XML_Node& operator=(const C_XML_Node& copy_source);
	};

	class C_XML_File {
		friend class C_XML_Base;
		friend class C_XML_Node;
		friend std::ostream& operator<< (std::ostream& stream, const C_XML_File& node_ref);
	private:
		C_Event_Log_Base&
			event_log;
		bool
			is_good = true,
			any_runtime_errors = false,
			search_error = false;
		std::variant<C_XML_Node, C_XML_Field>*
			in_operation = nullptr;
		std::unique_ptr<std::variant<C_XML_Node, C_XML_Field>>
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
		C_XML_Node&
			first();
		operator bool();
		template <typename req_type = C_XML_Node>
		req_type&
			get(std::string temp_path);
		template <typename req_type = C_XML_Node>
		req_type&
			get();
		template <typename req_type>
		req_type&
			main_node(std::string temp_path);
		C_XML_File(C_Event_Log_Base& event_log_ref);
		C_XML_File(C_Event_Log_Base& event_log_ref, std::filesystem::path file_path);
		~C_XML_File();
	};

	template<typename assigned_type>
	inline C_XML_Field & C_XML_Field::operator=(assigned_type assigned_value) {
		if (std::holds_alternative<assigned_type>(value)) {
			std::get<assigned_type>(value) = assigned_value;
		}
		return *this;
	}

	template<typename req_type>
	inline req_type & C_XML_Field::get() {
		if (std::holds_alternative<req_type>(value)) {
			return std::get<req_type>(value);
		}
		else {
			event_log() << "Type of value is not " << typeid(req_type).name() << _endl_;
		}

	}

	template<typename req_type>
	inline bool C_XML_File::exists(std::string name) {
		if (in_operation != nullptr) {
			if (std::holds_alternative<C_XML_Node>(*in_operation)) {
				return std::get<C_XML_Node>(*in_operation).exists<req_type>(name);
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
	inline req_type& C_XML_File::get() {
		if (std::holds_alternative<req_type>(*in_operation)) {
			return std::get<req_type>(*in_operation);
		}
		else {
			event_log() << "Type of in_operation is not " << typeid(req_type).name() << _endl_;
		}
	}

	template<typename req_type>
	inline req_type & C_XML_File::get(std::string temp_path) {
		auto path = string_to_path_vector(temp_path);
		if (path.size() == 0) {
			return std::get<req_type>(*in_operation);
		}
		return std::get<req_type>(*(std::get<C_XML_Node>(*in_operation).find(path)));
	}

	template<typename req_type>
	inline req_type & C_XML_File::main_node(std::string temp_path) {
		// TODO: tu wstawiæ instrukcjê return
	}

	template<typename req_type>
	inline bool C_XML_Node::exists(std::string name) {
		for (auto iter : elements) {
			if (std::holds_alternative<req_type>(iter.second)) {
				if (std::get<req_type>(iter.second).current_name() == name) {
					return true;
				}
			}
		}
		return false;
	}
}

#endif // !C_XML_h
#include "C_XML.h"

namespace PCL {
	std::vector<std::string> string_to_path_vector(std::string str_path) {
		std::cout << str_path << std::endl;
		char separator;
		size_t separator_pos;
		std::vector<std::string> path_vector;
		if (
			str_path.size() < 2 ||
			str_path.find(' ') != std::string::npos ||
			str_path.find('/') != std::string::npos && str_path.find('\\') != std::string::npos
			) {
			return path_vector;
		}
		separator = ((str_path.find('/') != std::string::npos) ? '/' : '\\');
		if (str_path[0] != separator) {
			str_path = separator + str_path;
		}
		if (str_path[str_path.size() - 1] != separator) {
			str_path += separator;
		}
		do {
			separator_pos = str_path.substr(1).find(separator) + 1;
			path_vector.push_back(str_path.substr(1, separator_pos - 1));
			str_path = str_path.substr(separator_pos);
		} while (str_path.size() >= 2);
		return path_vector;
	}

	std::string get_name(std::variant<C_XML_Node, C_XML_Field> xml_variant) {
		if (std::holds_alternative<C_XML_Node>(xml_variant)) {
			return std::get<C_XML_Node>(xml_variant).current_name();
		}
		else if (std::holds_alternative<C_XML_Field>(xml_variant)) {
			return std::get<C_XML_Field>(xml_variant).current_name();
		}
		return std::string();
	}

	line_content_type C_XML_File::check_line_type(std::string line__) {
		if (line__.find("</") != -1) {
			return line_content_type::closing_tag;
		}
		else if (line__.find("<") != -1) {
			return line_content_type::opening_tag;
		}
		else if (line__.find("=") != -1) {
			return line_content_type::field;
		}
		return line_content_type::unknown;
	}

	bool C_XML_File::open(std::filesystem::path file_path) {
		if (!std::filesystem::exists(xml_file_path)) {
			create(file_path);
		}
		file.open(file_path, std::ios::in | std::ios::out);
		if (file.is_open() && file) {
			event_log() << "File opening" << operation_evaluation<false>(file.is_open()) << _endl_;
			is_good = true;
			download();
		}
		else {
			is_good = false;
		}
		return is_good;
	}

	bool C_XML_File::is_open() {
		return file.is_open();
	}

	bool C_XML_File::create(std::filesystem::path file_path) {
		xml_file_path = file_path;
		file.open(xml_file_path, std::ios::out);
		event_log() << "Creating file" << operation_evaluation<false>(file.is_open() && file) << _endl_;
		if (file.is_open() && file) {
			is_good = true;
			close();
		}
		else {
			is_good = false;
		}
		return std::filesystem::exists(xml_file_path);
	}

	bool C_XML_File::close() {
		if (file.is_open()) {
			upload();
			file.close();
			event_log() << "File closing" << operation_evaluation<false>(!file.is_open()) << _endl_;
		}
		return !file.is_open();
	}

	void C_XML_File::upload() {
		if (file.is_open()) {
			file.close();
			file.open(xml_file_path, std::ios::out | std::ios::trunc);
			file << std::get<C_XML_Node>(*first_node).str();
			file.close();
			file.open(xml_file_path, std::ios::out | std::ios::in);
			event_log() << "Content uploaded" << _endl_;
		}
	}

	void C_XML_File::download() {
		if (file.is_open()) {
			std::vector<line_t>
				file_lines_vector;
			file.seekg(0);
			for (std::string str_line; std::getline(file, str_line);) {
				file_lines_vector.push_back(line_t{ str_line , check_line_type(str_line) });
			}
			event_log() << "Content downloaded: " << file_lines_vector.size() << " lines detected" << _endl_;
			first_node = std::make_unique<std::variant<C_XML_Node, C_XML_Field>>(C_XML_Node(*this, nullptr, 0, file_lines_vector.begin(), file_lines_vector.end()));
			in_operation = first_node.get();
			std::get<C_XML_Node>(*first_node).parent_node = &std::get<C_XML_Node>(*first_node);
			event_log() << "Content analysed" << _endl_;
		}
	}

	void C_XML_File::clear() {
		is_good = true;
		any_runtime_errors = false;
		search_error = false;
		event_log() << "Flags cleared" << _endl_;
	}

	C_XML_File::operator bool() {
		return !any_runtime_errors && is_good;
	}

	C_XML_File::C_XML_File(C_Event_Log_Base & event_log_ref) : event_log(event_log_ref) {}

	void C_XML_File::set_path(std::string field_path, bool is_absolute) {
		event_log() << "Setting path \"" << field_path << "\"" << _endl_;
		std::vector<std::string>
			path_elements = string_to_path_vector(field_path);
		if (is_absolute || in_operation == nullptr || in_operation == first_node.get()) {
			in_operation_path.resize(0);
			in_operation = first_node.get();
			if (path_elements.size() > 0) {
				in_operation = std::get<C_XML_Node>(*in_operation).find(path_elements);
			}
		}
		else {
			if (path_elements.size() > 0) {
				if (std::holds_alternative<C_XML_Node>(*in_operation)) {
					in_operation = std::get<C_XML_Node>(*in_operation).find(path_elements);
				}
				else {
					event_log(msg_type::warning) << "Path not found!" << _endl_;
				}
			}
		}
	}

	C_XML_Node & C_XML_File::first() {
		return std::get<C_XML_Node>(*first_node);
	}

	C_XML_File::C_XML_File(C_Event_Log_Base& event_log_ref, std::filesystem::path file_path) :
		event_log(event_log_ref),
		xml_file_path(file_path) {
		open(xml_file_path);
	}

	C_XML_File::~C_XML_File() {
		if (file.is_open()) {
			close();
		}
	}

	std::ostream & operator<<(std::ostream & stream, const C_XML_Field & field) {
		stream << field.name << "=";
		if (std::holds_alternative<std::string>(field.value)) {
			stream << "\"" << std::get<std::string>(field.value) << "\"";
		}
		else if (std::holds_alternative<char>(field.value)) {
			stream << "'" << std::get<char>(field.value) << "'";
		}
		else if (std::holds_alternative<double>(field.value)) {
			stream << std::get<double>(field.value);
		}
		else if (std::holds_alternative<long long>(field.value)) {
			stream << std::get<long long>(field.value);
		}
		else if (std::holds_alternative<unsigned long long>(field.value)) {
			stream << std::get<unsigned long long>(field.value);
		}
		return stream;
	}

	std::ostream & operator<<(std::ostream & stream, const C_XML_Node & node) {
		if (node.depth != 0) {
			stream << std::string(node.depth - 1, '\t') << "<" << node.name << ">" << std::endl;
		}
		for (auto& x : node.elements) {
			if (std::holds_alternative<C_XML_Field>(x.second)) {
				stream << std::string(node.depth, '\t') << std::get<C_XML_Field>(x.second) << std::endl;
			}
			else if (std::holds_alternative<C_XML_Node>(x.second)) {
				stream << std::get<C_XML_Node>(x.second) << std::endl;
			}
		}
		if (node.depth != 0) {
			stream << std::string(node.depth - 1, '\t') << "</" << node.name << ">";
		}
		return stream;
	}

	std::ostream & operator<<(std::ostream & stream, const C_XML_File & file_ref) {
		if (file_ref.first_node != nullptr) {
			return stream << std::get<C_XML_Node>(*(file_ref.first_node));
		}
		else {
			return stream;
		}
	}

	C_XML_Field C_XML_Node::line_to_field(std::string line_to_conv) {
		int_fast64_t
			begining_pos = std::string::npos,
			assignement_pos = std::string::npos;
		begining_pos = [&]()->int_fast64_t {
			for (auto pos = 0; pos < line_to_conv.length(); pos++) {
				if (math::interval<char, true, true>('A', 'Z') == line_to_conv[pos] || math::interval<char, true, true>('a', 'z') == line_to_conv[pos]) {
					return pos;
				}
			}
			return -1;
		}();
		line_to_conv = line_to_conv.substr(begining_pos);
		assignement_pos = line_to_conv.find("=");
		std::string
			value_str = line_to_conv.substr(assignement_pos + 1);
		if (value_str.find("\"") != std::string::npos && value_str.find_first_of("\"") != value_str.find_last_of("\"")) {
			return C_XML_Field(parent_file, this, line_to_conv.substr(0, assignement_pos), value_str.substr(1, value_str.size() - 2));
		}
		else if (value_str.find("'") != std::string::npos && value_str.find_first_of("'") != value_str.find_last_of("'")) {
			return C_XML_Field(parent_file, this, line_to_conv.substr(0, assignement_pos), value_str[1]);
		}
		else if (value_str.find(".") != std::string::npos) {
			return C_XML_Field(parent_file, this, line_to_conv.substr(0, assignement_pos), std::stod(value_str));
		}
		else {
			return C_XML_Field(parent_file, this, line_to_conv.substr(0, assignement_pos), std::stoll(value_str));
		}
	}

	std::vector<line_t>::iterator C_XML_Node::node_end(std::vector<line_t>::iterator first, std::vector<line_t>::iterator last) {
		uint_fast64_t
			relative_depth = 0;
		for (std::vector<line_t>::iterator it = first; it != last; it++) {
			if (it->type_of_content == line_content_type::opening_tag) {
				relative_depth++;
			}
			else if (it->type_of_content == line_content_type::closing_tag) {
				relative_depth--;
				if (relative_depth == 0) {
					return it;
				}
			}
		}
	}

	std::string C_XML_Node::get_section_name(std::string name_line) {
		return name_line.substr(name_line.find_first_of("<") + 1, name_line.find(">") - name_line.find_first_of("<") - 1);
	}

	void C_XML_Node::insert(std::string name, std::variant<std::string, char, double, long long, unsigned long long> field_value) {
		if (elements.count(name) == 0) {
			elements.insert({ name, C_XML_Field{ parent_file, this, name, field_value} });
			event_log() << "Field called \"" << name << "\" was inserted." << _endl_;
		}
		else {
			event_log(msg_type::warning) << "Field called \"" << name << "\" wasn't inserted! There is one already existing!" << _endl_;
		}
	}

	void C_XML_Node::insert(std::string name) {
		if (elements.count(name) == 0) {
			elements.insert({ name, C_XML_Node{ parent_file, this, name, depth + 1 } });
			event_log() << "Node called \"" << name << "\" was inserted." << _endl_;
		}
		else {
			event_log(msg_type::warning) << "Node called \"" << name << "\" wasn't inserted! There is one already existing!" << _endl_;
		}
	}

	void C_XML_Node::rename_element(std::string current_name, std::string new_name) {
		for (auto iter = elements.begin(); iter != elements.end(); iter++) {
			if (iter->first == current_name) {
				auto temp = iter->second;
				elements.erase(iter);
				if (std::holds_alternative<C_XML_Node>(temp)) {
					std::get<C_XML_Node>(temp).name = new_name;
					elements.insert({ new_name, temp });
				}
				else {
					std::get<C_XML_Field>(temp).name = new_name;
					elements.insert({ new_name, temp });
				}
				event_log() << "Object called \"" << current_name << "\" was renamed to \"" << "\"." << _endl_;
				return;
			}
		}
		event_log(msg_type::warning) << "Object called \"" << current_name << "\" doesn't exist! It couldn't be renamed!" << _endl_;
	}

	void C_XML_Node::erase(std::string name) {
		for (auto iter = elements.begin(); iter != elements.end(); iter++) {
			if (iter->first == name) {
				elements.erase(iter);
				event_log() << "Object called \"" << name << "\" was erased." << _endl_;
				return;
			}
		}
		event_log(msg_type::warning) << "Object called \"" << name << "\" doesn't exist! It couldn't be erased!" << _endl_;
	}

	C_XML_Node& C_XML_Node::operator=(const C_XML_Node & copy_source) {
		memcpy(this, &copy_source, sizeof(copy_source) - sizeof(copy_source.elements)); //!!!!
		elements = copy_source.elements;
		return *this;
	}

	std::string C_XML_Node::str() {
		std::string
			ret_str = "";
		if (depth != 0) {
			ret_str += (std::string(depth - 1, '\t') + "<" + name + ">\n");
		}
		for (auto& x : elements) {
			if (std::holds_alternative<C_XML_Field>(x.second)) {
				ret_str += (std::string(depth, '\t') + std::get<C_XML_Field>(x.second).str() + "\n");
			}
			else if (std::holds_alternative<C_XML_Node>(x.second)) {
				ret_str += (std::get<C_XML_Node>(x.second).str() + "\n");
			}
		}
		if (depth != 0) {
			ret_str += (std::string(depth - 1, '\t') + "</" + name + ">");
		}
		return ret_str;
	}

	std::variant<C_XML_Node, C_XML_Field>* C_XML_Node::find(std::vector<std::string> path_elems_vec) {
		const std::string
			searched_name = path_elems_vec[0];
		event_log() << "Searching through " << (depth == 0 ? "main" : name) << " node" << _endl_;
		event_log() << "Path vector size: " << path_elems_vec.size() << _endl_;
		if (searched_name != "") {
			for (auto& iter : elements) {
				if (std::holds_alternative<C_XML_Node>(iter.second) && std::get<C_XML_Node>(iter.second).current_name() == searched_name) {
					parent_file.in_operation_path.push_back(searched_name);
					if (path_elems_vec.size() == 1) {
						parent_file.search_error = false;
						return &(iter.second);
					}
					else {
						return std::get<C_XML_Node>(iter.second).find({ path_elems_vec.begin() + 1, path_elems_vec.end() });
					}
				}
				else if (std::holds_alternative<C_XML_Field>(iter.second) && std::get<C_XML_Field>(iter.second).current_name() == searched_name) {
					parent_file.in_operation_path.push_back(searched_name);
					if (path_elems_vec.size() == 1) {
						parent_file.search_error = false;
						return &(iter.second);
					}
					else {
						parent_file.search_error = true;
						event_log(msg_type::warning) << "Path not found! Encountered field instead of node!" << _endl_;
						return nullptr;
					}
				}
			}
		}
		else {
			if (parent_file.in_operation_path.size() > 0) {
				parent_file.in_operation_path.pop_back();
			}
			else {
				event_log(msg_type::warning) << "No parent directory! Continuing without error notification!" << _endl_;
			}
			return parent_node->find({ path_elems_vec.begin() + 1, path_elems_vec.end() });
		}
		parent_file.search_error = true;
		event_log(msg_type::warning) << "Path not found! Incorrect name!" << _endl_;
		return nullptr;
	}

	C_XML_Node::C_XML_Node(C_XML_File& parent_file_ref, C_XML_Node* parent_node_ptr, uint_fast64_t node_depth, std::vector<line_t>::iterator first, std::vector<line_t>::iterator last) :
		depth(node_depth),
		C_XML_Base(parent_file_ref, parent_node_ptr) {
		[[likely]] if (depth != 0) {
			name = get_section_name(first->content);
			event_log() << "Initializing node \"" << name << "\"" << _endl_;
			first++;
		}
		else {
			event_log() << "Initializing main node" << _endl_;
		}
		for (std::vector<line_t>::iterator it = first; it < last; it++) {
			if (it->type_of_content == line_content_type::field) {
				auto temp = line_to_field(it->content);
				elements.insert_or_assign(temp.current_name(), temp);
			}
			else if (it->type_of_content == line_content_type::opening_tag) {
				std::vector<line_t>::iterator
					it_end = node_end(it, last);
				elements.insert_or_assign(get_section_name(it->content), std::variant<C_XML_Node, C_XML_Field>(C_XML_Node(parent_file, this, depth + 1, it, it_end)));
				it = it_end;
			}
		}
	}

	C_XML_Node::C_XML_Node(C_XML_File & parent_file_ref, C_XML_Node* parent_node_ptr, std::string node_name, uint_fast64_t node_depth) :
		depth(node_depth),
		C_XML_Base(parent_file_ref, parent_node_ptr) {
		name = node_name;
	}

	C_XML_Field::C_XML_Field(C_XML_File& parent_file_ref, C_XML_Node* parent_node_ptr, std::string field_name, std::variant<std::string, char, double, long long, unsigned long long> field_value) :
		C_XML_Base(parent_file_ref, parent_node_ptr, field_name),
		value(field_value) {
		event_log() << "Initializing field \"" << name << "\"" << _endl_;
	}

	std::string C_XML_Field::str() {
		std::string
			ret_str = "";
		ret_str += (name + "=");
		if (std::holds_alternative<std::string>(value)) {
			ret_str += ("\"" + std::get<std::string>(value) + "\"");
		}
		else if (std::holds_alternative<char>(value)) {
			ret_str += std::string(std::string("'") + std::get<char>(value) + "'");
		}
		else if (std::holds_alternative<double>(value)) {
			ret_str += std::to_string(std::get<double>(value));
		}
		else if (std::holds_alternative<long long>(value)) {
			ret_str += std::to_string(std::get<long long>(value));
		}
		else if (std::holds_alternative<unsigned long long>(value)) {
			ret_str += std::to_string(std::get<unsigned long long>(value));
		}
		return ret_str;
	}

	C_XML_Field & C_XML_Field::operator=(const C_XML_Field & copy_source) {
		value = copy_source.value;
		return *this;
	}

	C_XML_Base::C_XML_Base(C_XML_File& parent_file_ref, C_XML_Node* parent_node_ptr, std::string new_name) :
		name(new_name),
		parent_file(parent_file_ref),
		event_log(parent_file_ref.event_log),
		parent_node(parent_node_ptr) {}

	std::string C_XML_Base::current_name() {
		return name;
	}
}
#include "module.h"

Module::Module(const std::string function_name, const std::string script_file)
	: function_name(function_name), script_file(script_file), results(nullptr) {
	ReadScriptHeader();
}

Module::~Module() {
	delete results;
}

void Module::ReadScriptHeader() {
	std::ifstream f;
	f.open(script_file.c_str());
	if (f.is_open()) {
		std::string params_line, return_line;
		getline(f, params_line);
		getline(f, return_line);

		// Remove whitespace in lines
		params_line.erase(std::remove(params_line.begin(), params_line.end(), ' '), params_line.end());
		return_line.erase(std::remove(return_line.begin(), return_line.end(), ' '), return_line.end());

		std::vector<std::string> params_str = ParseLine(params_line);
		std::vector<std::string> return_str = ParseLine(return_line);

		for (std::string param : params_str) {
			std::string name = param.substr(0, param.find('='));
			std::string type = param.substr(name.size(), param.size());

			param_names.push_back(name);
			param_types.push_back(ParseType(type));
		}

		for (std::string return_ : return_str) {
			return_types.push_back(ParseType(return_));
		}
	}
}

std::vector<std::string> Module::ParseLine(std::string line) {
	// Remove whitespace
	line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
	// Convert each character to lowercase
	std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) { return std::tolower(c); });

	std::string start_sym = ":";
	std::string delimiter = ",";

	// Ignore starting characters
	line = line.substr(line.find(start_sym), line.size());

	// Loop through any found tokens and add to result
	std::vector<std::string> tokens;
	size_t pos = 0;
	while ((pos = line.find(delimiter)) != std::string::npos) {
		tokens.push_back(line.substr(0, pos));
		line.erase(0, pos + delimiter.length());
	}
	return tokens;
}

json::value_t Module::ParseType(std::string type) {
	if (type == "object") {
		return json::value_t::object;
	}
	else if (type == "array") {
		return json::value_t::array;
	}
	else if (type == "string") {
		return json::value_t::string;
	}
	else if (type == "boolean") {
		return json::value_t::boolean;
	}
	else if (type == "int") {
		return json::value_t::number_integer;
	}
	else if (type == "float") {
		return json::value_t::number_float;
	}
	else {
		return json::value_t::null;
	}
}

std::string Module::FunctionName() {
	return function_name;
}

std::string Module::ScriptFile() {
	return script_file;
}

int Module::NumParams() {
	return (int)param_types.size();
}

int Module::NumReturns() {
	return (int)results->size();
}

const std::vector<std::string>* Module::ParamNames() {
	return &param_names;
}

const std::vector<json::value_t>* Module::ParamTypes() {
	return &param_types;
}

const std::vector<json::value_t>* Module::ReturnTypes() {
	return &return_types;
}

json* Module::Results() {
	return results;
}

void Module::Run(json* parameters) {
	py::scoped_interpreter guard{};

	std::stringstream script_path;
	script_path << SCRIPT_DIR << "." << script_file;

	py::module py_json = py::module::import("json"); // import python json package
	py::function f = py::module::import(script_path.str().c_str()).attr(function_name.c_str()); // import function with in script file and module name

	py::object py_args;
	if (!parameters->empty()) {
		py_args = py_json.attr("loads")(py::str(parameters->dump())); // parse json parameters from c to python
	}

	try {
		py::object obj = f(*py_args); // call function with unpacked args

		std::string str_json = py_json.attr("dumps")(obj).cast<std::string>(); // parse json returns from python to c

		results = new json(json::parse(str_json));
	}
	catch (std::runtime_error& e) {
		results = new json();
	}
}
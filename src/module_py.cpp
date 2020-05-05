#include "module_py.h"
#include "exceptions.h"

ModulePy::ModulePy(const ModulePy& module) 
	: ModulePy(module.function_name, module.script_file) {}

ModulePy::ModulePy(const std::string function_name) : ModulePy(function_name, function_name) {}

ModulePy::ModulePy(const std::string function_name, const std::string script_file) : Module(function_name, script_file) {
	ReadScriptHeader();
}

ModulePy::~ModulePy() {}

void ModulePy::ReadScriptHeader() {
	std::stringstream script_path;
	script_path << SCRIPT_DIR << "/" << script_file << ".py";

	std::ifstream f;
	f.open(script_path.str().c_str());
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
			std::string type = param.substr(name.size() + 1, param.size());

			param_names.push_back(name);
			param_types.push_back(ParseType(type));
		}

		for (std::string return_t : return_str) {
			return_types.push_back(ParseType(return_t));
		}
	}
}

std::vector<std::string> ModulePy::ParseLine(std::string line) {
	// Remove whitespace
	line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
	// Convert each character to lowercase
	std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) { return std::tolower(c); });

	std::string start_sym = ":";
	std::string delimiter = ",";

	// Ignore starting characters
	line = line.substr(line.find(start_sym) + 1, line.size());

	// Loop through any found tokens and add to result
	std::vector<std::string> tokens;
	size_t pos = 0;
	while ((pos = line.find(delimiter)) != std::string::npos) {
		tokens.push_back(line.substr(0, pos));
		line.erase(0, pos + delimiter.length());
	}
	
	if (line.length() != 0) {
		tokens.push_back(line);
	}
	return tokens;
}

json::value_t ModulePy::ParseType(std::string type) {
	if (type == "object") {
		return json::value_t::object;
	}
	else if (type == "array") {
		return json::value_t::array;
	}
	else if (type == "string") {
		return json::value_t::string;
	}
	else if (type == "bool") {
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

void ModulePy::Run() {
	if (ParamsCount() != 0) {
		throw MissingInputException("Expected at least one parameter, received none", this); // Module missing parameters.
	}
	else {
		json* empty_json = new json();
		Run(empty_json);
		delete empty_json;
	}
}

void ModulePy::Run(json* parameters) {
	py::scoped_interpreter guard{};

	std::stringstream script_path;
	script_path << SCRIPT_DIR << "." << script_file;

	py::module py_json = py::module::import("json"); // import python json package
	py::function f = py::module::import(script_path.str().c_str()).attr(function_name.c_str()); // import function with in script file and module name

	py::object py_args;
	if (parameters != nullptr && !parameters->empty()) {
		py_args = py_json.attr("loads")(py::str(parameters->dump())); // parse json parameters from c to python
	}

	try {
		py::object obj;
		if (ParamsCount() > 0) {
			obj = f(*py_args); // call function with unpacked args
		}
		else {
			obj = f(); // call function without parameters
		}

		std::string str_json = py_json.attr("dumps")(obj).cast<std::string>(); // parse json returns from python to c

		results = new json(json::parse(str_json));
	}
	catch (std::runtime_error) {
		results = nullptr;
	}

	/*std::cout << (int)ResultTypes()[0] << std::endl;
	if (ResultTypes() != return_types) {
		throw std::runtime_error("Resulting types do not match declared return types.");
	}*/
}
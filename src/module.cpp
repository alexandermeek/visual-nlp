#include "module.h"

Module::Module(const char* name, const char* script_file, Mod_Type type) : type(type) {
	strncpy_s(this->name, name, 31);
	strncpy_s(this->script_file, script_file, 99);
}

Module::~Module() {}

int Module::NumParams() {
	return (int)param_types.size();
}

int Module::NumReturns() {
	return (int)results.size();
}

json::value_t Module::Param(int pos) {
	return param_types.at(pos);
}

std::vector<json::value_t> Module::Params() {
	return param_types;
}

json::value_t Module::Return(int pos) {
	if (!results.empty()) {
		if (results.type() == json::value_t::array && pos > (results.size() - 1)) {
			return results[pos].type();
		}
	}
}

std::vector<json::value_t> Module::Returns() {
	std::vector<json::value_t> return_types;
	if (!results.empty() && results.type() == json::value_t::array) {
		for (int i = 0; i < results.size(); i++) {
			return_types.push_back(results[i].type());
		}
	}
	return return_types;
}

void Module::Run(json parameters) {
	py::scoped_interpreter guard{};
	
	std::stringstream module;
	module << SCRIPT_DIR << "." << script_file;

	py::module py_json = py::module::import("json"); // import python json package
	py::function f = py::module::import(module.str().c_str()).attr(name); // import function with in script file and module name

	py::object args = py_json.attr("loads")(py::str(parameters.dump())); // parse json parameters from c to python
	
	py::object obj = f(*args); // call function with unpacked args
	
	std::string str_json = py_json.attr("dumps")(obj).cast<std::string>(); // parse json returns from python to c
	results = json::parse(str_json);

	// debug purposes:
	std::cout << results.dump(4) << std::endl;
	std::vector<json::value_t> return_types = Returns();
	for (int i = 0; i < return_types.size(); i++) {
		std::cout << (int)return_types[i] << " ";
	}
}
#include "py_function.h"

PyFunction::PyFunction(const char* script_file, const char* function_name) {
	strncpy_s(this->script_file, script_file, 99);
	strncpy_s(this->function_name, function_name, 31);
}

PyFunction::~PyFunction() {}

json* PyFunction::Run(json* args) {
	py::scoped_interpreter guard{};

	py::module py_json = py::module::import("json"); // import python json package
	py::function f = py::module::import(script_file).attr(function_name); // import function with in script file and module name

	py::object py_args;
	if (!args->empty()) {
		py_args = py_json.attr("loads")(py::str(args->dump())); // parse json parameters from c to python
	}

	py::object obj = f(*py_args); // call function with unpacked args

	std::string str_json = py_json.attr("dumps")(obj).cast<std::string>(); // parse json returns from python to c

	json* json_result = new json(json::parse(str_json));

	return json_result;
}
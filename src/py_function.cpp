#include "py_function.h"

PyFunction::PyFunction(const std::string script_file, const std::string function_name)
	: script_file(script_file), function_name(function_name) {}

PyFunction::~PyFunction() {}

json* PyFunction::Call(json* args) {
	py::scoped_interpreter guard{};

	py::module py_json = py::module::import("json"); // import python json package
	py::function f = py::module::import(script_file.c_str()).attr(function_name.c_str()); // import function with in script file and module name

	py::object py_args;
	if (!args->empty()) {
		py_args = py_json.attr("loads")(py::str(args->dump())); // parse json parameters from c to python
	}

	py::object obj = f(*py_args); // call function with unpacked args

	std::string str_json = py_json.attr("dumps")(obj).cast<std::string>(); // parse json returns from python to c

	json* json_result = new json(json::parse(str_json));

	return json_result;
}
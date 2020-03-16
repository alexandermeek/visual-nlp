#ifndef PYFUNCTION_H
#define PYFUNCTION_H
#pragma once

#include <nlohmann/json.hpp>
#include <pybind11/embed.h>

#include <fstream>
#include <string>

namespace py = pybind11;
using json = nlohmann::json;

class PyFunction {
private:
	char script_file[100];
	char function_name[32];
public:
	PyFunction(const char* script_file, const char* function_name);
	~PyFunction();

	json* Run(json* args);
};
#endif // PYFUNCTION_H
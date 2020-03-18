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
	std::string script_file;
	std::string function_name;
public:
	PyFunction(const std::string script_file, const std::string function_name);
	~PyFunction();

	json* Call(json* args);
};
#endif // PYFUNCTION_H
#ifndef MODULE_H
#define MODULE_H
#pragma once

#include "py_function.h"

#include <nlohmann/json.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

namespace py = pybind11;
using json = nlohmann::json;

enum class Mod_Type { python };

class Module {
private:
	const char SCRIPT_DIR[100] = "scripts";

	std::string name;
	std::string script_file;
	Mod_Type type;

	json* results;

	std::vector<json::value_t> param_types;
public:
	Module(const std::string name, const std::string script_file, Mod_Type type);
	virtual ~Module();

	std::string Name();
	std::string ScriptFile();
	int NumParams();
	int NumReturns();
	json::value_t Param(int pos);
	std::vector<json::value_t> Params();
	json::value_t Return(int pos);
	std::vector<json::value_t> Returns();

	void Run(json* parameters);
};
#endif // MODULE_H


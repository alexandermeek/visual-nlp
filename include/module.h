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

	json* results;

	std::vector<json::value_t> param_types;
public:
	char name[32];
	char script_file[100];
	Mod_Type type;

	Module(const char* name, const char* script_file, Mod_Type type);
	virtual ~Module();

	int NumParams();
	int NumReturns();
	json::value_t Param(int pos);
	std::vector<json::value_t> Params();
	json::value_t Return(int pos);
	std::vector<json::value_t> Returns();

	void Run(json* parameters);
};
#endif // MODULE_H


#ifndef MODULE_H
#define MODULE_H
#pragma once

#include <nlohmann/json.hpp>
#include <pybind11/embed.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>

namespace py = pybind11;
using json = nlohmann::json;

class Module {
private:
	const std::string SCRIPT_DIR = "scripts";

	std::string script_file;
	std::string function_name;

	std::vector<std::string> param_names;
	std::vector<json::value_t> param_types;
	std::vector<json::value_t> return_types;

	json* results;

	void ReadScriptHeader();
	std::vector<std::string> ParseLine(std::string line);
	json::value_t ParseType(std::string type);
public:
	Module(const std::string name, const std::string script_file);
	virtual ~Module();

	std::string FunctionName();
	std::string ScriptFile();
	int NumParams();
	int NumReturns();
	const std::vector<std::string>* ParamNames();
	const std::vector<json::value_t>* ParamTypes();
	const std::vector<json::value_t>* ReturnTypes();

	json* Results();

	void Run(json* parameters);
};
#endif // MODULE_H


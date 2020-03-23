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
protected:
	const std::string SCRIPT_DIR = "scripts";

	std::string script_file;
	std::string function_name;

	std::vector<std::string> param_names;
	std::vector<json::value_t> param_types;
	std::vector<json::value_t> return_types;

	
public:
	Module(const std::string function_name);
	Module(const std::string function_name, const std::string script_file);
	virtual ~Module();

	virtual std::string FunctionName();
	virtual std::string ScriptFile();
	virtual int ParamsCount();
	virtual int ReturnsCount();
	virtual const std::vector<std::string>* ParamNames();
	virtual const std::vector<json::value_t>* ParamTypes();
	virtual const std::vector<json::value_t>* ReturnTypes();

	virtual json* Results();
	virtual std::vector<json::value_t> ResultTypes();
	void ClearResults();
	json* results;
	virtual void Run() = 0;
	virtual void Run(json* parameters) = 0;
};
#endif // MODULE_H


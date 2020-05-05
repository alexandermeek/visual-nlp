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

using json = nlohmann::json;

class Module {
protected:
	const std::string SCRIPT_DIR = "scripts";

	std::string script_file;
	std::string function_name;

	std::vector<std::string> param_names;
	std::vector<json::value_t> param_types;
	std::vector<json::value_t> return_types;

	std::vector<std::string> TypesToString(std::vector<json::value_t> types) const;

	json* custom_params;
	json* results;
public:
	Module(const Module& module);
	Module(const std::string function_name);
	Module(const std::string function_name, const std::string script_file);
	virtual ~Module();

	std::string FunctionName() const;
	std::string ScriptFile() const;
	int ParamsCount() const;
	int ReturnsCount() const;
	const std::vector<std::string>* ParamNames() const;
	const std::vector<json::value_t>* ParamTypes() const;
	const std::vector<json::value_t>* ReturnTypes() const;
	std::vector<std::string> ParamTypesToString() const;
	std::vector<std::string> ReturnTypesToString() const;

	std::string TypeToString(json::value_t data_type) const;

	void RemoveCustomParam(std::string param_name);
	void SetCustomParam(json param);
	json* CustomParams();
	bool HasCustomParam(std::string param_name);

	virtual json* Results();
	virtual std::vector<json::value_t> ResultTypes();
	void ClearResults();

	virtual void Run() = 0;
	virtual void Run(json* parameters) = 0;
};
#endif // MODULE_H


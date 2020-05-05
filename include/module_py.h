#ifndef MODULEPY_H
#define MODULEPY_H
#pragma once

#include "module.h"

#include <nlohmann/json.hpp>
#include <pybind11/embed.h>

namespace py = pybind11;
using json = nlohmann::json;

class ModulePy : public Module {
protected:
	void ReadScriptHeader();
	std::vector<std::string> ParseLine(std::string line);
	json::value_t ParseType(std::string type);
public:
	ModulePy(const ModulePy& module);
	ModulePy(const std::string function_name);
	ModulePy(const std::string function_name, const std::string script_file);
	virtual ~ModulePy();

	void Run();
	void Run(json* parameters);
};

#endif // MODULEPY_H
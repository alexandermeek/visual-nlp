#ifndef MODULELOADER_H
#define MODULELOADER_H
#pragma once

#include "module.h"
#include "module_py.h"

#include <nlohmann/json.hpp>

#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

using json = nlohmann::json;

class ModuleLoader {
private:
	const std::string SCRIPT_DIR = "scripts";

	std::string filename;
	std::vector<Module*> modules;

	json ModuleToJson(Module* module) const;
	Module* JsonToModule(json module_json) const;
public:
	ModuleLoader(std::string filename);
	virtual ~ModuleLoader();

	void Read();
	void Save();

	const std::vector<Module*>& Modules() const;
	int NumModules() const;

	void AddModule(Module* module);
	void RemoveModule(Module* module);
};

#endif
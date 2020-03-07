#ifndef MODULE_H
#define MODULE_H
#pragma once

#include "pybind11/embed.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

namespace py = pybind11;

enum class Mod_Type { python };

class Module {
private:
	const char SCRIPT_DIR[100] = "scripts";

	std::vector<std::string> param_types;
	std::vector<std::string> return_types;
public:
	char name[32];
	char script_file[100];
	Mod_Type type;

	Module(const char* name, const char* script_file, Mod_Type type);
	virtual ~Module();

	int NumParams();
	int NumReturns();
	std::string Param(int pos);
	std::vector<std::string>* Params();
	std::string Return(int pos);
	std::vector<std::string>* Returns();

	void Run();
};
#endif // MODULE_H


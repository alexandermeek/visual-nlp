#ifndef MODULE_H
#define MODULE_H
#pragma once

#include "pybind11/pybind11.h"
#include "pybind11/embed.h"

#include <vector>
#include <string>

namespace py = pybind11;

enum class Mod_Type { python };

class Module {
	std::vector<std::string> param_types;
	std::vector<std::string> return_types;
public:
	char name[32];
	Mod_Type type;

	Module(const char* name, Mod_Type type);
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


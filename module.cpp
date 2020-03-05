#include "module.h"

Module::Module(const char* name, Mod_Type type) : type(type) {
	strncpy_s(this->name, name, 31);
}

Module::~Module() {}

int Module::NumParams() {
	return (int)param_types.size();
}

int Module::NumReturns() {
	return (int)return_types.size();
}

std::string Module::Param(int pos) {
	return param_types.at(pos);
}

std::vector<std::string>* Module::Params() {
	return &param_types;
}

std::string Module::Return(int pos) {
	return return_types.at(pos);
}

std::vector<std::string>* Module::Returns() {
	return &return_types;
}
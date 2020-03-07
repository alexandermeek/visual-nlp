#include "module.h"

Module::Module(const char* name, const char* script_file, Mod_Type type) : type(type) {
	strncpy_s(this->name, name, 31);
	strncpy_s(this->script_file, script_file, 99);
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

void Module::Run() {
	py::scoped_interpreter guard{};
	
	std::stringstream module;
	module << SCRIPT_DIR << "." << script_file;

	py::module script = py::module::import(module.str().c_str());
	py::object result = script.attr(name)();
	

	
	//py::exec("print('Hello world')");
}
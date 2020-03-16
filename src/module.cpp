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
	return (int)results->size();
}

json::value_t Module::Param(int pos) {
	return param_types.at(pos);
}

std::vector<json::value_t> Module::Params() {
	return param_types;
}

json::value_t Module::Return(int pos) {
	if (!results->empty()) {
		if (results->type() == json::value_t::array && pos > (results->size() - 1)) {
			return results[pos].type();
		}
	}
}

std::vector<json::value_t> Module::Returns() {
	std::vector<json::value_t> return_types;
	if (!results->empty() && results->type() == json::value_t::array) {
		for (int i = 0; i < results->size(); i++) {
			return_types.push_back(results[i].type());
		}
	}
	return return_types;
}

void Module::Run(json* parameters) {
	std::stringstream script_path;
	script_path << SCRIPT_DIR << "." << script_file;

	PyFunction f(script_path.str().c_str(), name);

	results = f.Run(parameters);

	// debug purposes:
	std::cout << results->dump(4) << std::endl;
	std::vector<json::value_t> return_types = Returns();
	for (int i = 0; i < return_types.size(); i++) {
		std::cout << (int)return_types[i] << " ";
	}
}
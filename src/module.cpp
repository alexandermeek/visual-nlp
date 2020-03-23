#include "module.h"

Module::Module(const std::string function_name)
	: Module(function_name, function_name) {}

Module::Module(const std::string function_name, const std::string script_file)
	: function_name(function_name), script_file(script_file), results(nullptr) {}

Module::~Module() {
	delete results;
}

std::string Module::FunctionName() {
	return function_name;
}

std::string Module::ScriptFile() {
	return script_file;
}

int Module::ParamsCount() {
	return (int)param_types.size();
}

int Module::ReturnsCount() {
	return (int)return_types.size();
}

const std::vector<std::string>* Module::ParamNames() {
	return &param_names;
}

const std::vector<json::value_t>* Module::ParamTypes() {
	return &param_types;
}

const std::vector<json::value_t>* Module::ReturnTypes() {
	return &return_types;
}

json* Module::Results() {
	return results;
}

std::vector<json::value_t> Module::ResultTypes() {
	std::vector<json::value_t> result_types;
	if (results != nullptr && !results->empty()) {
		if (return_types.size() <= 1) {
			result_types.push_back(results->type());
		}
		else {
			for (int i = 0; i < results->size(); i++) {
				result_types.push_back(results[i].type());
			}
		}
	}
	return result_types;
}

void Module::ClearResults() {
	delete results;
	results = nullptr;
}
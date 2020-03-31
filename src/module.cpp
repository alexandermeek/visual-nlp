#include "module.h"

Module::Module(const std::string function_name)
	: Module(function_name, function_name) {}

Module::Module(const std::string function_name, const std::string script_file)
	: function_name(function_name), script_file(script_file), results(nullptr), custom_params(nullptr) {}

Module::~Module() {
	delete results;
	delete custom_params;
}

std::string Module::FunctionName() const {
	return function_name;
}

std::string Module::ScriptFile() const {
	return script_file;
}

int Module::ParamsCount() const {
	return (int)param_types.size();
}

int Module::ReturnsCount() const {
	return (int)return_types.size();
}

const std::vector<std::string>* Module::ParamNames() const {
	return &param_names;
}

const std::vector<json::value_t>* Module::ParamTypes() const {
	return &param_types;
}

const std::vector<json::value_t>* Module::ReturnTypes() const {
	return &return_types;
}

std::vector<std::string> Module::ParamTypesToString() const {
	return TypesToString(param_types);
}

std::vector<std::string> Module::ReturnTypesToString() const {
	return TypesToString(return_types);
}

std::string Module::TypeToString(json::value_t data_type) const {
	switch (data_type) {
	case json::value_t::null:
		return "null";
	case json::value_t::object:
		return "object";
	case json::value_t::array:
		return "array";
	case json::value_t::string:
		return "string";
	case json::value_t::boolean:
		return "boolean";
	case json::value_t::number_integer:
		return "integer";
	case json::value_t::number_unsigned:
		return "unsigned integer";
	case json::value_t::number_float:
		return "float";
	case json::value_t::discarded:
		return "discarded";
	default:
		return "";
	}
}

std::vector<std::string> Module::TypesToString(std::vector<json::value_t> types) const {
	std::vector<std::string> types_str;
	for (json::value_t t : types) {
		types_str.push_back(TypeToString(t));
	}
	return types_str;
}

void Module::SetCustomParam(json param) {
	assert(param.type() == json::value_t::object);

	if (custom_params == nullptr) {
		custom_params = new json();
	}

	for (json::iterator it = param.begin(); it != param.end(); ++it) {
		(*custom_params)[it.key()] = it.value();
	}
}

json* Module::CustomParams() {
	return custom_params;
}

json* Module::Results() const {
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
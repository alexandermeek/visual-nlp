#include "module_loader.h"

ModuleLoader::ModuleLoader(std::string filename) : filename(filename) {
	Read();
}

ModuleLoader::~ModuleLoader() {
	Save();
	for (Module* module : modules) {
		delete module;
	}
}

void ModuleLoader::Read() {
	json modules_json;

	std::stringstream file_path;
	file_path << SCRIPT_DIR << "/" << filename << ".json";

	std::ifstream file;
	file.open(file_path.str());
	if (file.is_open()) {
		file >> modules_json;
		file.close();

		modules.clear();
		for (auto& module : modules_json) {
			modules.push_back(JsonToModule(module));
		}
	}
}

void ModuleLoader::Save() {
	json modules_json;
	for (Module* module : modules) {
		modules_json.push_back(ModuleToJson(module));
	}

	std::ofstream file(filename);
	file << std::setw(4) << modules_json << std::endl;
	file.close();
}

void ModuleLoader::AddModule(Module* module) {
	modules.push_back(module);
}

void ModuleLoader::RemoveModule(Module* module) {
	modules.erase(std::find(modules.begin(), modules.end(), module));
}

const std::vector<Module*>& ModuleLoader::Modules() const {
	return modules;
}

int ModuleLoader::NumModules() const {
	return (int)modules.size();
}

json ModuleLoader::ModuleToJson(Module* module) const {
	json module_json;
	if (module) {
		if (dynamic_cast<ModulePy*>(module)) {
			module_json["module_type"] = "python";
		}
		module_json["script_file"] = module->ScriptFile();
		module_json["function_name"] = module->FunctionName();
	}
	return module_json;
}

Module* ModuleLoader::JsonToModule(json module_json) const {
	Module* module = nullptr;
	if (module_json["module_type"] == "python") {
		module = new ModulePy(module_json["function_name"], module_json["script_file"]);
	}
	return module;
}
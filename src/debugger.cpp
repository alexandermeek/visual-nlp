#include "debugger.h"

Debugger::logmap Debugger::log = {
	{ "Debugger", "begin" }
};

Debugger::Debugger() {}

Debugger::~Debugger() {}

void Debugger::Clear() {
	Debugger::log.clear();
}

void Debugger::Add(std::string name, std::string value) {
	Debugger::log[name] = value;
}

void Debugger::Remove(std::string name) {
	Debugger::log.erase(name);
}

int Debugger::size() {
	return (int)Debugger::log.size();
}

bool Debugger::empty() {
	return Debugger::log.empty();
}

std::map<std::string, std::string>::iterator Debugger::begin() {
	return Debugger::log.begin();
}

std::map<std::string, std::string>::iterator Debugger::end() {
	return Debugger::log.end();
}

void Debugger::Show(bool* p_open) {
	if (!ImGui::Begin("Debugger", p_open)) {
		ImGui::End();
		return;
	}
	for (log_item item : Debugger::log) {
		ImGui::Text("%s: %s", item.first.c_str(),item.second.c_str());
	}

	ImGui::End();
}
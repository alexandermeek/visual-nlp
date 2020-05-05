#ifndef DEBUGGER_H
#define DEBUGGER_H
#pragma once

#include <imgui/imgui.h>

#include <string>
#include <map>
#include <algorithm>
#include <sstream>

class Debugger {
private:
	typedef std::pair<std::string, std::string> log_item;
	typedef std::map<std::string, std::string> logmap;
	static std::map<std::string, std::string> log;
public:
	Debugger();
	~Debugger();

	void Clear();
	void Add(std::string name, std::string value);
	void Remove(std::string name);
	int size();
	bool empty();

	std::map<std::string, std::string>::iterator begin();
	std::map<std::string, std::string>::iterator end();

	void Show(bool* p_open);
};
#endif // DEBUGGER_H
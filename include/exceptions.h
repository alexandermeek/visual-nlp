#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#pragma once
#include "module.h"

#include <exception>

class MissingInputException : public std::exception {
public:
	const char*   msg;
	const Module* module;

	MissingInputException(const char* msg, const Module* module) : msg(msg), module(module) {}
	
	const char* what() const throw () {
		return "Missing input data to node.";
	}
};

class ModuleRuntimeError : public std::exception {
public:
	const char*   msg;
	const Module* module;

	ModuleRuntimeError(const char* msg, const Module* module) : msg(msg), module(module) {}
	
	const char* what() const throw () {
		return "Error while running module.";
	}
};
#endif // EXCEPTIONS_H
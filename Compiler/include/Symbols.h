#pragma once

#include "Platform.h"

#include <unordered_map>
#include <map>

class Function;
class File;
class Object;
class Type;

using DynamicType = std::variant<
	double,
	int,
	bool,
	Function*,
	std::string,
	std::vector<Type*>,
	File*,
	std::map<std::string, Type*>
>;

class Symbols {
public:
	Symbols(Symbols* parent = nullptr);

	typedef std::unordered_map<std::string, DynamicType> SymbolsMap;

	SymbolsMap::iterator get(const std::string& name);
	void set(const std::string& name, DynamicType value);
	bool remove(const std::string& name);
	
	SymbolsMap symbols;
	Symbols* parent;
};

#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <fstream>

#include "types.h"
#include "util.h"

/*
Reads config files
*/
class Config {
public:
	void init(const std::string filename);

	// name : type
	std::unordered_map<std::string, std::string> getObjects();

	int getIntAttribute(const std::string& objectName, const std::string& attributeName);
	float getFloatAttribute(const std::string& objectName, const std::string& attributeName);
	bool getBoolAttribute(const std::string& objectName, const std::string& attributeName);
	std::string getStringAttribute(const std::string& objectName, const std::string& attributeName);
	std::string getObjectAttribute(const std::string& objectName, const std::string& attributeName);

	void setIntAttribute(const std::string& objectName, const std::string& attributeName, int set);
	void setFloatAttribute(const std::string& objectName, const std::string& attributeName, float set);
	void setBoolAttribute(const std::string& objectName, const std::string& attributeName, bool set);
	void setStringAttribute(const std::string& objectName, const std::string& attributeName, const std::string& set);
	void setObjectAttribute(const std::string& objectName, const std::string& attributeName, const std::string& set);

	const std::string name_ = "CONFIG";

private:
	std::string filename_ = "";

	void parseFile();
	// name : type
	std::unordered_map<std::string, std::string> objects_ = {};


	std::string getAttributeString(const std::string& objectName, const std::string& attributeName);

	bool ignoreLine(const std::string& line);
	std::string getName(const std::string& line);
	std::string getType(const std::string& line);



};
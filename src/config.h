#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "types.h"
#include "util.h"

/*
Reads config files
*/
class Config {
public:
	void init(const std::string filename);

	std::vector<std::string> getObjectNames();

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

	void openFile();
	void pointToObject(const std::string& objectName);
	void pointToAttribute(const std::string& attributeName);
	void closeFile();

};
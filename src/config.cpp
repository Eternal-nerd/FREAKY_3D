#include "config.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Config::init(const std::string filename) {
	util::log(name_, "initializing config object for file: " + filename);
	
	filename_ = filename;
}


/*-----------------------------------------------------------------------------
------------------------------GETTERS------------------------------------------
-----------------------------------------------------------------------------*/
std::vector<std::string> Config::getObjectNames() {
	return { "" };
}

int Config::getIntAttribute(const std::string& objectName, const std::string& attributeName) {
	return 0;
}

float Config::getFloatAttribute(const std::string& objectName, const std::string& attributeName) {
	return 0.f;
}

bool Config::getBoolAttribute(const std::string& objectName, const std::string& attributeName) {
	return true;
}

std::string Config::getStringAttribute(const std::string& objectName, const std::string& attributeName) {
	return "";
}

std::string Config::getObjectAttribute(const std::string& objectName, const std::string& attributeName) {
	return "";
}

/*-----------------------------------------------------------------------------
------------------------------SETTERS------------------------------------------
-----------------------------------------------------------------------------*/
void Config::setIntAttribute(const std::string& objectName, const std::string& attributeName, int set) {

}

void Config::setFloatAttribute(const std::string& objectName, const std::string& attributeName, float set) {

}

void Config::setBoolAttribute(const std::string& objectName, const std::string& attributeName, bool set) {

}

void Config::setStringAttribute(const std::string& objectName, const std::string& attributeName, const std::string& set) {

}

void Config::setObjectAttribute(const std::string& objectName, const std::string& attributeName, const std::string& set) {

}

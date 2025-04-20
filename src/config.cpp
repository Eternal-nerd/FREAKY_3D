#include "config.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Config::init(const std::string filename) {
	util::log(name_, "initializing config object for file: " + filename);
	
	filename_ = filename;

	parseFile();

	// FIXME TESTING
    //setAttributeString("testRect", "positionX", "0.1");

	//std::string test = getAttributeString("TestElement", "happiness");

	//util::log(name_, "getAttributeString(TestElement, happiness): " + test);
}

void Config::parseFile() {
	util::log(name_, "parsing file: " + filename_);

	bool insideObject = false;
	std::string objectType = "";
	std::string objectName = "";
	std::ifstream inFile(filename_);
	std::string line;
	while (std::getline(inFile, line)) {
		// check if line is comment
		if (ignoreLine(line)) {
			continue;
		}

		if (!insideObject) {
			// reset object strings
			objectType="";
			objectName="";
			bool reading = false;
			bool typeRead = false;

			// loop over chars in line
			for (char c : line) {
				if (!std::isspace(c)) {
					reading = true;
					if (!typeRead) {
						objectType += c;
					}
					else {
						objectName += c;
					}
				}
				else {
					if (reading) {
						reading = false;
						if (typeRead) {
							// done reading both object type and name
							objects_[objectName] = objectType;
							break;
						}
						else {
							// object name needs reading
							typeRead = true;
							continue;
						}
					}
				}
			}

			insideObject = true;
		}
		else {
			if (line.find('}') != std::string::npos) {
				insideObject = false;
			}
		}
	}

	if (insideObject) {
		throw std::runtime_error("failed to parse configuration file: " + filename_ + " - object definition incomplete. ");
	}

	inFile.close();

}

/*-----------------------------------------------------------------------------
------------------------------GETTERS------------------------------------------
-----------------------------------------------------------------------------*/
std::unordered_map<std::string, std::string> Config::getObjects() {
	return objects_;
}

int Config::getIntAttribute(const std::string& objectName, const std::string& attributeName) {
	std::string retVal = getAttributeString(objectName, attributeName);
	return std::stoi(retVal);
}

float Config::getFloatAttribute(const std::string& objectName, const std::string& attributeName) {
	std::string retVal = getAttributeString(objectName, attributeName);
	return std::stof(retVal);
}

bool Config::getBoolAttribute(const std::string& objectName, const std::string& attributeName) {
	std::string retVal = getAttributeString(objectName, attributeName);
	return retVal == "true";
}

std::string Config::getStringAttribute(const std::string& objectName, const std::string& attributeName) {
	return getAttributeString(objectName, attributeName);
}

std::string Config::getObjectAttribute(const std::string& objectName, const std::string& attributeName) {
	return getAttributeString(objectName, attributeName);
}

/*-----------------------------------------------------------------------------
------------------------------SETTERS------------------------------------------
-----------------------------------------------------------------------------*/
void Config::setAttributeString(const std::string& objectName, const std::string& attributeName, const std::string& setString) {
    // FIXME
    util::log("DEBUG", "Change - Object: " + objectName + ", Attribute: " + attributeName + " to -> " + setString);

    // get config file contents into a string
    std::string fileContents = "";
    std::ifstream inFile(filename_);
    for (char c; inFile.get(c); fileContents.push_back(c)) {}
    inFile.close();

    // find position of object in file
    auto objectPos = fileContents.find(objectName);
    if (objectPos == std::string::npos) {
        throw std::runtime_error("failed to find object: " + objectName);
    }

    // find the attribute right after the object name
    auto attributePos = fileContents.find(attributeName);
    while (attributePos != std::string::npos) {
        if (attributePos > objectPos) {
            // move string pointer to 1 after equals sign
            while (fileContents[attributePos] != '=') {
                attributePos++;
            }
            // move past '='
            attributePos++;
            // move past whitespace
            while (std::isspace(fileContents[attributePos]))  {
                attributePos++;
            }
            // move to semicolon, incrementing count
            int replaceCount = 0;
            auto startPos = attributePos;
            while (fileContents[attributePos] != ';') {
                attributePos++;
                replaceCount++;
            }
            fileContents.replace(startPos, replaceCount, setString);
            break;
        }
        else {
            // find the next one
            attributePos = fileContents.find(attributeName, attributePos);
        }
    }
    
    // create temp file that will replace old one
    std::string tempFilename = filename_ + ".tmp";
    std::ofstream outFile(tempFilename);
    if (!outFile) {
        throw std::runtime_error("failed to open temp file for config rewrite");
    }
    outFile << fileContents;
    outFile.close();

    // delete original file
    int status = std::remove(filename_.c_str());
    if (status != 0) {
        throw std::runtime_error("failed to remove file: " + filename_);
    }

    // rename temp file
    std::rename(tempFilename.c_str(), filename_.c_str());
}



/*-----------------------------------------------------------------------------
------------------------------UTIL---------------------------------------------
-----------------------------------------------------------------------------*/
std::string Config::getAttributeString(const std::string& objectName, const std::string& attributeName) {

	std::string returnString = "";

	bool insideObject = false;
	bool insideDesiredObject = false;
	std::ifstream inFile(filename_);
	std::string line;
	while (std::getline(inFile, line)) {
		// check if line is comment
		if (ignoreLine(line)) {
			continue;
		}

		if (!insideObject) {
			insideDesiredObject = getName(line) == objectName;

			insideObject = true;
		}
		else {
			// be looking for attribute
			if (insideDesiredObject) {
				if (getName(line) == attributeName) {
					// disect line here and return attribute string
					int beginIndex = (int)line.find('=');
					int endIndex = (int)line.find(';');

					for (int i = beginIndex; i < endIndex; i++) {
						if (line[i] != ' ' && line[i] != '=' && line[i] != ';') {
							returnString += line[i];
						}
					}
					break;
				}
			}

			if (line.find('}') != std::string::npos) {
				insideObject = false;
				if (insideDesiredObject) {
					throw std::runtime_error("failed to find desired attribute");
				}
			}
		}
	}

	inFile.close();

	return returnString;
}

std::string Config::getName(const std::string& line) {
	std::string retVal = "";

	bool reading = false;
	bool readType = false;
	for (char c : line) {
		if (!std::isspace(c)) {
			reading = true;
			if (readType) {
				retVal += c;
			}
		}
		else {
			if (readType) {
				break;
			}
			if (reading) {
				readType = true;
			}
		}
	}

	return retVal;

}

std::string Config::getType(const std::string& line) {
	std::string retVal = "";

	bool reading = false;
	for (char c : line) {
		if (!std::isspace(c)) {
			reading = true;
			retVal += c;
		}
		else {
			if (reading) {
				break;
			}
		}
	}

	return retVal;
}


bool Config::ignoreLine(const std::string& line) {
	for (char c : line) {
		if (std::isspace(c)) {
			continue;
		}
		if (c == '!') {
			return true;
		}
		else {
			return false;
		}
	}
	return true;
}

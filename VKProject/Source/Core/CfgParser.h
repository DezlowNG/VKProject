#pragma once

#ifndef CFGPARSER_H
#define CFGPARSER_H

#include <iostream>
#include <fstream>

class CfgParser
{
public:
	CfgParser();
	~CfgParser();

	std::string GetConfigs() { return mConfigList; }
private:
	std::fstream mConfigFile;
	std::string mConfigList;
};

#endif
#include "CfgParser.h"

#include <vector>

CfgParser::CfgParser()
{
	mConfigFile = std::fstream("user.cfg", std::ios::in);

	if (!mConfigFile.is_open())
	{
		mConfigFile = std::fstream("user.cfg", std::ios::app | std::ios::out);
		mConfigFile << "MSAA=0\n";
		mConfigFile << "MIPMAPS=FALSE\n";
		mConfigFile << "ANISOTROPY=0\n";
		mConfigFile << "SAMPLE_RATE_SHADING=FALSE\n";
		mConfigFile.close();
	}

	mConfigFile = std::fstream("user.cfg", std::ios::in | std::ios::ate);
	size_t fileSize = static_cast<size_t>(mConfigFile.tellg());
	mConfigFile.seekg(0);

	std::vector<char> buffer(fileSize);

	mConfigFile.read(buffer.data(), fileSize);

	mConfigList = buffer.data();

	mConfigFile.close();
}

CfgParser::~CfgParser()
{
}

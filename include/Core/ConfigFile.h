#ifndef ConfigFile_h__
#define ConfigFile_h__

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "ResourceManager.h"


namespace dd
{

class ConfigFile : public Resource
{
	friend class ResourceManager;

private:
	ConfigFile(std::string path);

public:
	template <typename T>
	T GetValue(std::string key, T defaultValue)
	{
		return m_ptree.get<T>(key, defaultValue);
	}

private:
	boost::property_tree::ptree m_ptree;

};

};
#endif // ConfigFile_h__

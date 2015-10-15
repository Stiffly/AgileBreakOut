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
	T GetValue(std::string key, T defaultValue);
	template <typename T>
	void SetValue(std::string key, T value);

	void SaveToDisk();

private:
	std::string m_Path;
	boost::property_tree::ptree m_Ptree;
};

template <typename T>
T ConfigFile::GetValue(std::string key, T defaultValue)
{
	return m_Ptree.get<T>(key, defaultValue);
}

template <typename T>
void ConfigFile::SetValue(std::string key, T value)
{
	m_Ptree.put<T>(key, value);
}

};
#endif // ConfigFile_h__

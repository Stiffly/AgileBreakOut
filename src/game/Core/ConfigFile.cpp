#include "PrecompiledHeader.h"
#include "Core/ConfigFile.h"

dd::ConfigFile::ConfigFile(std::string path)
{
	m_Path = path;
	try {
		boost::property_tree::ini_parser::read_ini(path, m_Ptree);
	} catch (boost::property_tree::ptree_error& e) {
		LOG_ERROR("Failed to load %s", path.c_str());
	}
}

void dd::ConfigFile::SaveToDisk()
{
	boost::property_tree::ini_parser::write_ini(m_Path, m_Ptree);
}

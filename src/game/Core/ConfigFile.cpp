#include "PrecompiledHeader.h"
#include "Core/ConfigFile.h"

dd::ConfigFile::ConfigFile(std::string path)
{
	boost::property_tree::ini_parser::read_ini(path, m_ptree);
}
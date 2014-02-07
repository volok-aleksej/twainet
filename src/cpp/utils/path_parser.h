#ifndef PATH_PARSER_H
#define PATH_PARSER_H

#include <string>

class PathParser
{
public:
	PathParser(const std::string& filePath);

	std::string GetFolder();
	std::string GetFile();
	std::string GetFileName();
	std::string GetExtension();
private:
	std::string m_folder;
	std::string m_file;
	std::string m_nameFile;
	std::string m_extension;
};


#endif/*PATH_PARSER_H*/
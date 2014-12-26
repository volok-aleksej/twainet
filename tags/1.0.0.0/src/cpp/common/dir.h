#ifndef DIR_H
#define DIR_H

#include <string>

class Dir
{
public:
	static std::string GetCurrentDir();
	static std::string GetProcessDir();
	static std::string GetCommonDir();
};

#endif/*DIR_H*/
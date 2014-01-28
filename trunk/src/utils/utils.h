#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

class CommonUtils
{
public:
	static std::vector<std::string> DelimitString(const std::string& src, const std::string& delimit);
	static std::string ConcatString(const std::vector<std::string>& src, const std::string& delimit);
	static std::string FormatTime(time_t time);
};

#endif/*UTILS_H*/
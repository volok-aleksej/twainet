#include "utils.h"
#include <stdio.h>

std::vector<std::string> CommonUtils::DelimitString(const std::string& src, const std::string& delimit)
{
	std::vector<std::string> dest;
	size_t begin = 0, pos = 0;
	while (pos != -1)
	{
		pos = src.find(delimit.c_str(), begin);
		if(pos != -1)
		{
			dest.push_back(std::string(src.begin() + begin, src.begin() + pos));
			begin = pos + delimit.size();
		}
	}

	dest.push_back(std::string(src.begin() + begin, src.end()));
	return dest;
}

std::string CommonUtils::ConcatString(const std::vector<std::string>& src, const std::string& delimit)
{
	std::string dest;
	for(std::vector<std::string>::const_iterator it = src.begin();
		it != src.end(); it++)
	{
		if(it != src.begin())
		{
			dest.append(delimit.c_str());
		}

		dest.append(it->c_str());
	}

	return dest;
}

std::string CommonUtils::FormatTime(time_t time)
{
	std::string res = "";

	int sec = time % 60;
	int min = (time / 60) % 60;
	int hour = (time / (60 * 60)) % 60;
	int day = (time / (24 * 60 * 60)) % 60;

	char data[20] = {0};
#ifdef WIN32
	sprintf_s(data, 20, "%02d  %02d:%02d:%02d", day, hour, min, sec);
#else
	sprintf(data, "%02d  %02d:%02d:%02d", day, hour, min, sec);
#endif/*WIN32*/
	res = data;

	return res;
}

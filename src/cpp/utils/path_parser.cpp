#include "path_parser.h"

PathParser::PathParser(const std::string& filePath)
{
	std::string path = filePath;
	size_t pos = -1;
	do 
	{
		pos = path.find('\\', pos + 1);
		if(pos != -1)
		{
			path[pos] ='/';
		}
	} while (pos != -1);

	pos = path.find_last_of('/');
	if(pos == -1)
	{
		pos = 0;
	}

	m_folder.insert(m_folder.begin(), path.begin(), path.begin() + pos);
	if(m_folder.empty())
	{
		m_file.insert(m_file.begin(), path.begin() + pos, path.end());
	}
	else
	{
		m_file.insert(m_file.begin(), path.begin() + pos + 1, path.end());
	}
	pos = m_file.find_last_of('.');
	if(pos != -1)
	{
		m_nameFile.insert(m_nameFile.begin(), m_file.begin(), m_file.begin() + pos);
		m_extension.insert(m_extension.begin(), m_file.begin() + pos + 1, m_file.end());
	}
	else
	{
		m_nameFile = m_file;
	}
}

std::string PathParser::GetFolder()
{
	return m_folder;
}

std::string PathParser::GetFile()
{
	return m_file;
}

std::string PathParser::GetFileName()
{
	return m_nameFile;
}

std::string PathParser::GetExtension()
{
	return m_extension;
}

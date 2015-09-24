#include "file.h"

#include "utils/path_parser.h"

#ifdef WIN32
#include <direct.h>
#include <io.h>

#define access _access
#define mkdir(file, param) _mkdir(file)
#define fileno _fileno

#else
#include <unistd.h>
#endif/*WIN32*/

#include <sys/stat.h>

File::File(const std::string& filePath)
: m_file(0)
{
	PathParser parser(filePath);
	m_fileDir = parser.GetFolder();
	m_fileName = parser.GetFile();
}

File::~File()
{
	Close();
}

bool File::Delete()
{
	std::string filePath = m_fileDir + "/" + m_fileName;
	return ::remove(filePath.c_str()) == 0;
}

bool File::IsExist()
{
	std::string filePath = m_fileDir + "/" + m_fileName;
	return ::access(filePath.c_str(), 0) == 0;
}

bool File::Create()
{
	CreateDir();
	return Open("wb");
}

bool File::Write(const char* data, unsigned int len)
{
	if (m_file == 0 && !Open("a+b"))
	{
		return false;
	}

	size_t writelen = ::fwrite(data, sizeof(char), len, m_file);
	return writelen == len;
}

bool File::Read(char* data, unsigned int* len)
{
	if (m_file == 0 && !Open("rb"))
	{
		*len = 0;
		return false;
	}

	size_t readlen = ::fread(data, sizeof(char), *len, m_file);

	if(*len != readlen)
	{
		*len = (unsigned int)readlen;
		return true;
	}

	return true;
}

bool File::Open(const std::string& mode)
{
	Close();
	std::string filePath = m_fileDir + "/" + m_fileName;
#ifdef WIN32
	fopen_s(&m_file, filePath.c_str(), mode.c_str());
#else
	m_file = fopen(filePath.c_str(), mode.c_str());
#endif
	return m_file != 0;
}

bool File::Close()
{
	int res = -1;
	if(m_file != 0)
	{
		::fclose(m_file);
	}
	m_file = 0;

	return res == 0;
}

void File::CreateDir()
{
	size_t pos = -1;
	do 
	{
		std::string folder;
		pos = m_fileDir.find('/', pos + 1);
		if(pos != -1)
		{
			folder.append(m_fileDir.begin(), m_fileDir.begin() + pos);
		}
		else
		{
			folder.append(m_fileDir.begin(), m_fileDir.end());
		}

		if(::access(folder.c_str(), 0) != 0)
		{
			::mkdir(folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		}
	} while (pos != -1);
}

int File::GetFileSize()
{
	if (m_file == 0 && !Open("rb"))
	{
		return false;
	}

	struct stat st;
	::fstat(fileno(m_file), &st);
	return st.st_size;
}


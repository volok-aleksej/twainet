#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <string>

class File
{
public:
	File(const std::string& filePath);
	~File();

	bool Delete();
	bool IsExist();
	bool Create();
	bool Open(const std::string& mode);
	bool Close();
	bool Write(const char* data, unsigned int len);
	bool Read(char* data, unsigned int* len);
	int GetFileSize();
	void CreateDir();
private:
	FILE* m_file;
	std::string m_fileDir;
	std::string m_fileName;
};

#endif/*FILE_H*/
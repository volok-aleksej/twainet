#include "config.h"
#include "common/dir.h"
#include "common/common.h"
#include "common/file.h"
#include "utils/utils.h"

#define MAX_BUFFER_LEN 1024

Config::Config()
: m_file(Dir::GetConfigDir() + "/twainet.conf")
{
	m_file.Load();
}

Config::~Config()
{
	m_file.Save();
}

std::string Config::GetTrustedFileName()
{
	return m_file.getString("common", "trustedFileName", "twainet.trusted");
}

std::string Config::GetPluginsFileName()
{
    return m_file.getString("common", "pluginsFileName", "twainet.plugins");
}
    
int Config::GetLocalServerPort()
{
	return m_file.getLong("common", "localPort", g_localServerPort);
}

void Config::SetLocalServerPort(int port)
{
	m_file.setLong("common", "localPort", port);
}

FileConfig::FileConfig(const std::string& filePath)
: m_filePath(filePath){}

FileConfig::~FileConfig(){}

std::vector<std::string> FileConfig::Read()
{
    std::vector<std::string> dataContainer;
    File configFile(m_filePath);
    int filesize = configFile.GetFileSize(), filepos = 0;
    char* data = new char[MAX_BUFFER_LEN];
    char* dataPos = data;
    while(filepos < filesize)
    {
        unsigned int size = MAX_BUFFER_LEN - (unsigned int)(dataPos - data) ;
        memset(dataPos, 0, size);
        if(!configFile.Read(dataPos, &size))
        {
            break;
        }
        filepos += size;
        
        std::vector<std::string> lines = CommonUtils::DelimitString(data, "\n");
        int pos = 0;
        for(int i = 0; i < (int)lines.size() - 1; i++)
        {
            pos += lines[i].size() + 1;
            std::string line = lines[i];
            if(line[line.size() - 1] == '\r')
                line.erase(line.size() - 1, 1);
            if(line[0] == '#' || line.empty())
                continue;
            
            dataContainer.push_back(line);
        }
        
        int templen = filepos - pos;
        char* tempData = new char[templen];
        memcpy(tempData, data + pos, templen);
        memcpy(data, tempData, templen);
        delete tempData;
        dataPos = data + templen;
    }
    
    if(dataPos - data)
    {
        dataContainer.push_back(std::string(data, dataPos - data));
    }
    delete data;
    return dataContainer;
}

void FileConfig::Write(const std::string& description, const std::vector<std::string>& data)
{
    File configFile(m_filePath);
    configFile.Open("wt");
    configFile.Write("# ", 2);
    configFile.Write(description.c_str(), description.size());
    configFile.Write("\r\n", 2);
    for(std::vector<std::string>::const_iterator it = data.begin();
        it != data.end(); it++)
    {
        configFile.Write(it->c_str(), it->size());
        configFile.Write("\r\n", 1);
    }
}
#ifdef WIN32
#	include <windows.h>
#else
#	include <stdio.h>
#	include <string.h>
#	define MAX_PATH 255
#endif/*WIN32*/

#include "../../version.h"
#include "common/file.h"
#include "compiler/til_compiler.h"
#include "resource.h"
#include <vector>
#include <string>

void print_usage(const std::vector<std::string>& argvs)
{
#ifdef WIN32
     std::string appName = app_self_name();
#else
     std::string appName = argvs[0].c_str();
#endif
     printf("Twainet interface language compiler.\nVersion: %s\n", VER_NUMBER_STRING);
     printf("Usage: %s [-h][-d <target directory>] <filename>\n", appName.c_str());
     printf("Usage: %s [-h][-d <target directory>] <filename>\n", appName.c_str());
     printf("Example: %s -h\n", appName.c_str());
     printf("Example: %s -d src-gen example.til\n", appName.c_str());
     printf("\n");
}

int  main(int argc, char* argv[])
{
	std::vector<std::string> args;
	for(int i = 0; i < argc; i++)
		args.push_back(argv[i]);

	std::string full_name;
	std::string param;
	std::string target_dir;
	std::string source;
	if (args.size() > 0)
	{
		full_name = args[0];
	}

	if (args.size() > 1)
	{
		param = args[1];
	}
	
	if (param == "-h" || param == "h")
	{
		print_usage(args);
		return 0;
	}
	else if (param == "-d" || param == "d")
	{
	      target_dir = args[2];
	}
	else
	{
	      source = param;
	}
	
	if (args.size() > 3)
	{
		source = args[3];
	}
		
	File f(source);
	if(source.empty() || !f.IsExist())
	{
	      print_usage(args);
	      return 0;
	}
	
	TILCompiler compiler(source);
    compiler.Parse();
    
	//TODO: generate c++
	
	return 0;
}
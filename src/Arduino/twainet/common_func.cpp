#include <ESP8266TrueRandom.h>
#include "common_func.h"

byte uuidNumber[16];
    
String CreateGUID()
{
    ESP8266TrueRandom.uuid(uuidNumber);
    return ESP8266TrueRandom.uuidToString(uuidNumber);
}

String RandString(int size)
{
    String str;
	String GUID = CreateGUID();
	size_t charN = 0;
	char erazeChars[] = {'{', '}', '-'};
    const char* symb = GUID.c_str();
	while(*symb != 0)
	{
        if(*symb == '{' || *symb == '}' || *symb == '-')
        {
            continue;
        }
        str.concat(*symb);
	}

	return str;
}

int GetError()
{
    return 0;
}

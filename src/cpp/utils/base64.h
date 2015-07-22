#ifndef BASE64_H
#define BASE64_H

#ifndef WIN32
#	include <stdio.h>
#	include <string.h>
#endif/*WIN32*/

#include <string>

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string base64_decode(std::string const& encoded_string, unsigned char* out = NULL, int* len = NULL);


#endif/*BASE64_H*/
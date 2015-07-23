#ifndef BASE64_H
#define BASE64_H

#ifndef WIN32
#	include <stdio.h>
#	include <string.h>
#endif/*WIN32*/

#include <string>

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string base64_decode(std::string const& encoded_string, unsigned char* out = NULL, int* len = NULL);
int base64_ntop(unsigned char const *src, size_t srclength, char *target, size_t targsize);
int base64_pton(char const *src, unsigned char *target, size_t targsize);

#endif/*BASE64_H*/
#ifndef ZLIB_IMPLEMENT_H
#define ZLIB_IMPLEMENT_H

#include <cstdio>
#include <string>

int decompress (FILE* input, FILE* output);
int compress (FILE* input, FILE* output);
std::string decompress_string (const std::string& compressed_str);
std::string compress_string (const std::string& input_str);

#endif // ZLIB_IMPLEMENT_H

#ifndef ENCODING_UTILS_H
#define ENCODING_UTILS_H

#include <vector>
#include <string>

enum class Enc { UTF8_BOM, UTF8_NO_BOM, UTF16_LE, UTF16_BE, ANSI };

bool is_valid_utf8(const std::vector<unsigned char>& data);
bool likely_utf16_le(const std::vector<unsigned char>& data);
bool likely_utf16_be(const std::vector<unsigned char>& data);
Enc detect_encoding(const std::vector<unsigned char>& data);
std::string convert_to_utf8(const std::vector<unsigned char>& data, Enc e);

#endif // ENCODING_UTILS_H

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

bool naturalCompare(const std::string& a, const std::string& b);
std::vector<fs::path> GetTxtFiles(const fs::path& dir);

#endif // FILE_UTILS_H

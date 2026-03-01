#include "FileUtils.h"
#include <iostream>
#include <algorithm>

bool naturalCompare(const std::string& a, const std::string& b) {
    size_t i = 0, j = 0;
    auto is_digit = [](char ch) { return std::isdigit(static_cast<unsigned char>(ch)) != 0; };
    auto stripLeadingZeros = [](const std::string& s) {
        size_t pos = s.find_first_not_of('0');
        return pos == std::string::npos ? "0" : s.substr(pos);
    };
    while (i < a.size() && j < b.size()) {
        if (is_digit(a[i]) && is_digit(b[j])) {
            size_t si = i, sj = j;
            while (i < a.size() && is_digit(a[i])) ++i;
            while (j < b.size() && is_digit(b[j])) ++j;
            std::string numA = a.substr(si, i - si), numB = b.substr(sj, j - sj);
            std::string valA = stripLeadingZeros(numA), valB = stripLeadingZeros(numB);
            if (valA.size() != valB.size()) return valA.size() < valB.size();
            if (valA != valB) return valA < valB;
            if (numA.size() != numB.size()) return numA.size() < numB.size();
        }
        else {
            if (a[i] != b[j]) return static_cast<unsigned char>(a[i]) < static_cast<unsigned char>(b[j]);
            ++i; ++j;
        }
    }
    return a.size() < b.size();
}

std::vector<fs::path> GetTxtFiles(const fs::path& dir) {
    std::vector<fs::path> txts;
    if (!fs::exists(dir)) { std::cerr << "Directory not found: " << dir << "\n"; return txts; }
    try {
        auto options = fs::directory_options::skip_permission_denied;
        for (auto& ent : fs::recursive_directory_iterator(dir, options)) {
            try {
                if (ent.is_regular_file() && ent.path().extension() == ".txt" &&
                    !ent.path().filename().string().starts_with('.')) {
                    txts.push_back(ent.path());
                }
            }
            catch (...) {}
        }
    }
    catch (...) {}
    std::sort(txts.begin(), txts.end(), [](const fs::path& a, const fs::path& b) {
        return naturalCompare(a.filename().string(), b.filename().string());
    });
    return txts;
}

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "EncodingUtils.h"
#include "FileUtils.h"

int main() {
    std::string inputDir;
    std::cout << "Please enter the directory path containing the files to merge: ";
    std::getline(std::cin, inputDir);

    fs::path outPath = fs::path("data") / "combined.txt";
    // Ensure data directory exists
    if (!fs::exists("data")) {
        fs::create_directory("data");
    }

    std::ofstream out(outPath, std::ios::binary);
    if (!out) { std::cerr << "Cannot create output file\n"; return 1; }

    // Write UTF-8 BOM
    unsigned char bom[3] = { 0xEF,0xBB,0xBF };
    out.write(reinterpret_cast<const char*>(bom), 3);

    auto files = GetTxtFiles(inputDir);
    if (files.empty()) { std::cout << "No .txt files found\n"; return 0; }

    std::cout << "Files found (in natural order):\n";
    for (auto& p : files) std::cout << " - " << p.filename().string() << "\n";
    std::cout << "\nPress Enter to merge: "; std::string tmp; std::getline(std::cin, tmp);

    size_t processed = 0;
    for (auto& p : files) {
        std::ifstream in(p, std::ios::binary);
        if (!in) { std::cerr << "Cannot open: " << p << "\n"; continue; }

        std::vector<unsigned char> data((std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());
        in.close();

        Enc enc = detect_encoding(data);
        std::string utf8 = convert_to_utf8(data, enc);

        if (!utf8.empty()) {
            // Separator
            if (processed > 0) {
                std::string sep = "\n--- Merged File: " + p.filename().string() + " ---\n";
                out.write(sep.data(), (std::streamsize)sep.size());
            }
            // Write content
            out.write(utf8.data(), (std::streamsize)utf8.size());
            out.write("\n", 1); // Newline at the end of each file
        }

        ++processed;
    }
    out.close();

    std::cout << "\nDone! Files merged: " << processed << "\nOutput: " << fs::absolute(outPath) << "\n";
    std::cout << "Format: UTF-8 with BOM\n";
    return 0;
}

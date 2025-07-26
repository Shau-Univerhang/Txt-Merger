#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <cctype>

namespace fs = std::filesystem;

void SkipBomIfExists(std::ifstream& file) {
    char bom[3];
    file.read(bom, 3);
    if (file.gcount() == 3 &&
        static_cast<unsigned char>(bom[0]) == 0xEF &&
        static_cast<unsigned char>(bom[1]) == 0xBB &&
        static_cast<unsigned char>(bom[2]) == 0xBF) {
        return;
    }
    file.seekg(0);
}

// 修复后的目录遍历函数
std::vector<fs::path> GetTxtFiles(const fs::path& dir) {
    std::vector<fs::path> txtFiles;

    if (!fs::exists(dir)) {
        std::cerr << "错误：目录不存在 - " << dir << std::endl;
        return txtFiles;
    }

    try {
        auto options = fs::directory_options::skip_permission_denied;
        fs::recursive_directory_iterator it(dir, options);
        fs::recursive_directory_iterator end;

        for (const auto& entry : fs::recursive_directory_iterator(dir, options)) {
            try {
                if (entry.is_regular_file() &&
                    entry.path().extension() == ".txt" &&
                    !entry.path().filename().string().starts_with('.')) {
                    txtFiles.push_back(entry.path());
                }
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "警告：跳过不可访问路径 - " << e.path1() << ": " << e.what() << std::endl;
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "文件系统错误: " << e.what() << std::endl;
    }

    return txtFiles;
}

int main() {
    std::string inputDir;
    std::cout << "请输入要合并的目录路径: ";
    std::getline(std::cin, inputDir);

    const fs::path outputFile = "combined.txt";
    std::ofstream outFile(outputFile, std::ios::binary);

    if (!outFile) {
        std::cerr << "错误：无法创建输出文件 " << outputFile << std::endl;
        return 1;
    }

    auto txtFiles = GetTxtFiles(inputDir);
    if (txtFiles.empty()) {
        std::cout << "未找到任何.txt文件" << std::endl;
        return 0;
    }

    size_t processed = 0;
    for (const auto& filePath : txtFiles) {
        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile) {
            std::cerr << "警告：无法打开文件 " << filePath << " - 跳过" << std::endl;
            continue;
        }

        SkipBomIfExists(inFile);

        outFile << inFile.rdbuf();
        inFile.close();

        std::cout << "已合并: " << filePath << std::endl;
        processed++;
    }

    std::cout << "\n合并完成！处理文件数: " << processed
        << "\n输出文件: " << fs::absolute(outputFile)
        << "\n文件大小: " << fs::file_size(outputFile) << " 字节"
        << "\n编码: UTF-8 无 BOM" << std::endl;

    return 0;
}
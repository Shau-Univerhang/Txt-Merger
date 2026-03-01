#include "EncodingUtils.h"
#include <windows.h>

bool is_valid_utf8(const std::vector<unsigned char>& data) {
    size_t i = 0, n = data.size();
    while (i < n) {
        unsigned char c = data[i];
        if (c <= 0x7F) { i++; continue; }
        else if ((c >> 5) == 0x6) { if (i + 1 >= n || (data[i + 1] & 0xC0) != 0x80) return false; i += 2; }
        else if ((c >> 4) == 0xE) { if (i + 2 >= n || (data[i + 1] & 0xC0) != 0x80 || (data[i + 2] & 0xC0) != 0x80) return false; i += 3; }
        else if ((c >> 3) == 0x1E) { if (i + 3 >= n || (data[i + 1] & 0xC0) != 0x80 || (data[i + 2] & 0xC0) != 0x80 || (data[i + 3] & 0xC0) != 0x80) return false; i += 4; }
        else return false;
    }
    return true;
}

bool likely_utf16_le(const std::vector<unsigned char>& data) {
    if (data.size() < 4) return false;
    size_t zeroOdd = 0;
    for (size_t i = 1; i < data.size(); i += 2) if (data[i] == 0) ++zeroOdd;
    return zeroOdd > (data.size() / 10);
}

bool likely_utf16_be(const std::vector<unsigned char>& data) {
    if (data.size() < 4) return false;
    size_t zeroEven = 0;
    for (size_t i = 0; i < data.size(); i += 2) if (data[i] == 0) ++zeroEven;
    return zeroEven > (data.size() / 10);
}

Enc detect_encoding(const std::vector<unsigned char>& data) {
    if (data.size() >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) return Enc::UTF8_BOM;
    if (data.size() >= 2 && data[0] == 0xFF && data[1] == 0xFE) return Enc::UTF16_LE;
    if (data.size() >= 2 && data[0] == 0xFE && data[1] == 0xFF) return Enc::UTF16_BE;
    if (is_valid_utf8(data)) return Enc::UTF8_NO_BOM;
    if (likely_utf16_le(data)) return Enc::UTF16_LE;
    if (likely_utf16_be(data)) return Enc::UTF16_BE;
    return Enc::ANSI;
}

std::string convert_to_utf8(const std::vector<unsigned char>& data, Enc e) {
    if (data.empty()) return std::string();
    if (e == Enc::UTF8_BOM) return std::string(reinterpret_cast<const char*>(data.data() + 3), data.size() - 3);
    if (e == Enc::UTF8_NO_BOM) return std::string(reinterpret_cast<const char*>(data.data()), data.size());
    if (e == Enc::UTF16_LE || e == Enc::UTF16_BE) {
        size_t offset = 0;
        if (data.size() >= 2 && ((data[0] == 0xFF && data[1] == 0xFE) || (data[0] == 0xFE && data[1] == 0xFF))) offset = 2;
        size_t bytes = data.size() > offset ? data.size() - offset : 0;
        size_t units = bytes / 2;
        std::vector<wchar_t> wbuf(units);
        bool be = (e == Enc::UTF16_BE);
        for (size_t k = 0; k < units; ++k) {
            unsigned char b1 = data[offset + k * 2];
            unsigned char b2 = data[offset + k * 2 + 1];
            wchar_t wc = be ? ((b1 << 8) | b2) : ((b2 << 8) | b1);
            wbuf[k] = wc;
        }
        if (wbuf.empty()) return std::string();
        int needed = WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), (int)wbuf.size(), nullptr, 0, nullptr, nullptr);
        if (needed <= 0) return std::string();
        std::string out(needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), (int)wbuf.size(), &out[0], needed, nullptr, nullptr);
        return out;
    }
    // ANSI -> UTF-8
    int wneeded = MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<const char*>(data.data()), (int)data.size(), nullptr, 0);
    if (wneeded <= 0) return std::string(reinterpret_cast<const char*>(data.data()), data.size());
    std::wstring wbuf(wneeded, 0);
    MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<const char*>(data.data()), (int)data.size(), &wbuf[0], wneeded);
    int needed = WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), (int)wbuf.size(), nullptr, 0, nullptr, nullptr);
    if (needed <= 0) return std::string();
    std::string out(needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), (int)wbuf.size(), &out[0], needed, nullptr, nullptr);
    return out;
}

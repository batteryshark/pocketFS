#pragma once

#include <string>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

#define DEFAULT_ROOT_DEVICE_NAME "/"
#ifdef _WIN32
#define ROOT_PATH "\\"
#else
#define ROOT_PATH "/"
#endif

namespace PFSUtil{
    std::string GetDriveLetterFromPath(std::basic_string<char, std::char_traits<char>, std::allocator<char>> in_path);
    std::string NormalizePath(const char* in_path);
    std::vector<std::string> StringSplit(const std::string &s, char delim);
}


void sanitize_path(fs::path& inpath);
void windowize_path(fs::path& path) ;


void test_drive_letter_windows();

// -- Comparator Logic Setups
struct CaseInsensitiveComparator {
    bool operator()(const std::string& a, const std::string& b) const noexcept {
        return ::strcasecmp(a.c_str(), b.c_str()) < 0;
    }
};

struct CaseSensitiveComparator {
    bool operator()(const std::string& a, const std::string& b) const noexcept {
        return a == b; // Pedantic, I get it - we're doing this to swap out this or the other.
    }
};

#ifdef _MSC_VER
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif



template <typename T>
using PFSMap = std::map<std::string, T, CaseInsensitiveComparator>;
template <typename T>
using PFSMapCS = std::map<std::string, T, CaseSensitiveComparator>;


#include "PFSUtil.h"
#include <vector>
#include <string>
#include <sstream>

namespace PFSUtil{


// Note: DRIVE LETTERS ARE NOT SUBJECT TO CASE SENSITIVITY ON WINDOWS
// Therefore, a path of C:\foo will still resolve if given c:\foo because case sensitivity is only for directories.
// Also, this is Windows Only

std::string GetDriveLetterFromPath(std::string& in_path){
    std::string device_name;

    // Make path absolute if relative.
    fs::path working_path = in_path;
    if(working_path.is_relative()){
        working_path = absolute(working_path);
    }

    // If we still don't have a drive letter, fail out.
    size_t letter_offset = working_path.string().find_first_of(':');
    if( letter_offset == std::string::npos){
        return device_name;
    }
    // Drop back one position.
    letter_offset--;

    device_name = working_path.string().substr(letter_offset,2);
    return device_name;
}


std::string NormalizePath(const char* in_path){
    fs::path out_path = in_path;
    // TODO - Convert path to absolute in a sanitized manner.
    return out_path;
}

    std::vector<std::string> StringSplit(const std::string &s, char delim) {
        std::stringstream ss(s);
        std::string item;
        std::vector<std::string> elems;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
            // elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
        }
        return elems;
    }


}

/*
 Helper function to standardize an input path that will work predictably cross-platform.
 Windows paths have their drive letters turned into root directories (e.g. C:\app becomes /C/app).
 Relative paths are also expanded to absolute paths.
 */
void sanitize_path(fs::path& inpath) {
    // We need to make this an intake and get the root path / drive letter for the new mapping
    //
    // If our path is seemingly absolute (and yes, drive paths don't work right across all platforms)
    // We'll make some adjustments, otherwise, we'll resolve like a relative path.
    if ((inpath.string().find(':') != std::string::npos) || inpath.string()[0] == '/') {
        std::string spath = inpath.string();
        // A bit hacky, but we're gonna remove the drive letter approximation and just call it like another directory.
        spath.erase(remove(spath.begin(), spath.end(), ':'), spath.end());
        std::replace(spath.begin(), spath.end(), '\\', '/');
        if (spath[0] != '/') {
            spath = "/" + spath;
        }
        inpath = spath;
    }
    else {
        inpath = absolute(inpath);
    }
}

// Convert the given path to native windows.
void windowize_path(fs::path& path) {

    path = path.make_preferred();

    if (path.string().length() > 4) {

        // Reconstruct Drive Letter
        char drive_prefix[4] = "Z:\\";
        drive_prefix[0] = path.string().at(1);

        std::string path_leaf = path.string().substr(3);
        path = drive_prefix;
        path += path_leaf;
    }

}

void test_drive_letter_windows(){
    /*
    fs::path tp = R"(C:\users\batteryshark\happy.wav)";
    std::string res = PFSUtil::GetDriveLetterFromPath(tp.string());
    printf("Drive Letter is: %s\n",res.c_str());

    tp = R"(d:\users\batteryshark\happy.wav)";
    res = PFSUtil::GetDriveLetterFromPath(tp.string());
    printf("Drive Letter is: %s\n",res.c_str());

    tp = R"(\\??\\q:\users\batteryshark\happy.wav)";
    res = PFSUtil::GetDriveLetterFromPath(tp.string());
    printf("Drive Letter is: %s\n",res.c_str());

    tp = R"(\\.\X:/ABC/DEF)";
    res = PFSUtil::GetDriveLetterFromPath(tp.string());
    printf("Drive Letter is: %s\n",res.c_str());


    tp = R"(/path/to/stuff)";
    res = PFSUtil::GetDriveLetterFromPath(tp.string());
    printf("Drive Letter is: %s\n",res.c_str());
    */
}





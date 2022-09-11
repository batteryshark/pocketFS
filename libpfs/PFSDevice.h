#pragma once
#include "PFSUtil.h"
#include "DCache.h"
#include "DEntry.h"
#include "PFSData.h"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

typedef struct DeviceInfo{
    std::vector <std::string> entries;
    size_t num_directories = 0;
    size_t num_files = 0;
}DeviceInfo;


class DEntry;
class DCache;

class PFSDevice {
private:
    std::string name = DEFAULT_ROOT_DEVICE_NAME;
    bool case_sensitive = false;
    DEntry *root = nullptr;
    DCache* d_cache;
    PFSMap<DEntry *> GetAllEntries(DEntry *cnode, fs::path cpath);
public:
    DEntry* GetRootNode(){return this->root;}
    DeviceInfo* GetInfo();
    DEntry* QueryPath(const char* in_path);
    bool EntryExists(const char* in_path);
    bool RemoveEntry(const char* in_path, bool force_remove);
    bool AddEntry(const char* target_path, PFSData* file_data, bool create_parent, bool read_only);
    std::string GetDeviceName(){return this->name;}
    [[nodiscard]] bool IsCaseSensitiveFS() const{return this->case_sensitive;}
    PFSDevice(bool case_sensitive_filesystem, bool read_only_root, const char *device_name);
    ~PFSDevice();
};
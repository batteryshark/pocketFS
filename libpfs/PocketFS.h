#pragma once

#include "PFSDevice.h"
#include "PFSHandleManager.h"
#include "PFSConfig.h"

class PocketFS {
private:
    PFSConfig* config;
    PFSHandleManager handle_manager;
    PFSMap<PFSDevice*> devices;
    std::string persistence_path;
    PFSDevice* AddDevice(bool is_case_sensitive, bool is_readonly, const char* target_path);
    PFSDevice* SelectDevice(const char* target_path);
public:


    void PrintInfo();
    void SetPersistencePath(const char* new_persistence_path);
    bool MapPath(const char* src_path, const char* virtual_path, bool map_case_sensitive, bool map_read_only);
    PocketFS(const char* path_to_config, const char* persistence_base);
};

